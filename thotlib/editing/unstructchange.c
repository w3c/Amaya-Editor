/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2004
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * This module handles the Paste command in unstructured mode.
 *
 * Authors: V. Quint (INRIA)
 *          S. Bonhomme (INRIA) - Separation between structured and
 *                                unstructured editing modes
 *
 */
#include "thot_gui.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "constmenu.h"
#include "typemedia.h"
#include "language.h"
#include "appaction.h"
#include "appstruct.h"
#include "fileaccess.h"
#include "libmsg.h"
#include "message.h"
#include "appdialogue.h"
#include "dialog.h"
#include "tree.h"
#include "content.h"

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "creation_tv.h"
#include "modif_tv.h"
#include "select_tv.h"
#include "frame_tv.h"
#include "edit_tv.h"
#include "appdialogue_tv.h"

#include "absboxes_f.h"
#include "actions_f.h"
#include "appli_f.h"
#include "applicationapi_f.h"
#include "attrpresent_f.h"
#include "attributeapi_f.h"
#include "attributes_f.h"
#include "boxselection_f.h"
#include "buildboxes_f.h"
#include "callback_f.h"
#include "changeabsbox_f.h"
#include "changepresent_f.h"
#include "content_f.h"
#include "contentapi_f.h"
#include "createabsbox_f.h"
#include "displayview_f.h"
#include "documentapi_f.h"
#include "editcommands_f.h"
#include "exceptions_f.h"
#include "externalref_f.h"
#include "docs_f.h"
#include "fileaccess_f.h"
#include "memory_f.h"
#include "presrules_f.h"
#include "references_f.h"
#include "scroll_f.h"
#include "search_f.h"
#include "searchref_f.h"
#include "schemas_f.h"
#include "selectmenu_f.h"
#include "structcommands_f.h"
#include "structcreation_f.h"
#include "structmodif_f.h"
#include "structschema_f.h"
#include "structselect_f.h"
#include "tableH_f.h"
#include "textcommands_f.h"
#include "thotmsg_f.h"
#include "tree_f.h"
#include "undo_f.h"
#include "undoapi_f.h"
#include "unstructlocate_f.h"
#include "views_f.h"
#include "viewapi_f.h"

/*----------------------------------------------------------------------
   InsertPastedElement
   Insere l'element pNew devant ou derriere (selon before) l'element pEl du
   document pDoc.
  ----------------------------------------------------------------------*/
static void InsertPastedElement (PtrElement pEl, ThotBool within,
				 ThotBool before,
				 PtrElement *pNew, PtrDocument pDoc)
{
   PtrElement          pSibling;

   if (within)
     {
	if (pEl->ElStructSchema->SsRule->SrElem[pEl->ElTypeNumber - 1]->SrConstruct ==
                                                                     CsChoice)
           InsertOption (pEl, pNew, pDoc);
	else
           InsertFirstChild (pEl, *pNew);
     }
   else if (before)
     {
	/* teste d'abord si pEl est le premier fils de son pere, abstraction
	   faite des marques de page */
	pSibling = pEl->ElPrevious;
	BackSkipPageBreak (&pSibling);
	/* insere l'element dans l'arbre */
	InsertElementBefore (pEl, *pNew);
	if (pSibling == NULL)
	   /* l'element devant lequel on colle n'est plus premier */
	   ChangeFirstLast (pEl, pDoc, TRUE, TRUE);
     }
   else
     {
	/* teste d'abord si pEl est le dernier fils de son pere, abstraction
	   faite des marques de page */
	pSibling = pEl->ElNext;
	FwdSkipPageBreak (&pSibling);
	/* insere l'element dans l'arbre */
	InsertElementAfter (pEl, *pNew);
	if (pSibling == NULL)
	   /* l'element devant lequel on colle n'est plus premier */
	   ChangeFirstLast (pEl, pDoc, FALSE, TRUE);
     }
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
static void NotifyHExtension (PtrElement pCell, Document doc)
{
  PtrSSchema          pSS;
  PtrAttribute        pAttr;
  NotifyAttribute     notifyAttr;
  int                 attrHSpan;

  if (pCell)
    {
      pSS = pCell->ElStructSchema;
      attrHSpan = GetAttrWithException (ExcColSpan, pSS);
      if (attrHSpan)
	{
	  /* is this attribute attached to the cell */
	  pAttr = pCell->ElFirstAttr;
	  while (pAttr)
	    {
	      if (pAttr->AeAttrNum == attrHSpan && pAttr->AeAttrSSchema == pSS)
		{
		  notifyAttr.event = TteAttrModify;
		  notifyAttr.document = doc;
		  notifyAttr.element = (Element) pCell;
		  notifyAttr.info = 0; /* not sent by undo */
		  notifyAttr.attribute = (Attribute) pAttr;
		  notifyAttr.attributeType.AttrSSchema = (SSchema) pSS;
		  notifyAttr.attributeType.AttrTypeNum = pAttr->AeAttrNum;
		  /* notify .PRE */
		  if (!CallEventAttribute (&notifyAttr, TRUE))
		    {
		      if (pAttr->AeAttrValue > 1)
			{
			  pAttr->AeAttrValue++;
			  AddAttrEditOpInHistory (pAttr, pCell,
						  LoadedDocument[doc -1],
						  TRUE, TRUE);
			}
		      /* notify .POST */
		      CallEventAttribute (&notifyAttr, FALSE);
		    }
		  pAttr = NULL;
		}
	      else
		pAttr = pAttr->AeNext;
	    }
	}
    }
}

/*----------------------------------------------------------------------
  PasteAnElement  Paste element decribed by pSavedEl within (if within
  is TRUE), before (if before is TRUE) or after (if before is FALSE)
  element pEl in document pDoc.
  The parameter cellChild should point to the pasted enclosing cell when
  children of the cell are pasted instead of the cell itself. This parameter
  could also point to the enclosing row.
  When the parameter addedCell points to the new generated cell, this cell
  is pasted instead of the element pointed by pSavedEl.
  ----------------------------------------------------------------------*/
static PtrElement PasteAnElement (PtrElement pEl, PtrPasteElem pSavedEl,
				  ThotBool within, ThotBool before,
				  ThotBool *cancelled, PtrDocument pDoc,
				  PtrElement *cellChild, PtrElement addedCell)
{
   PtrElement          pElem, pChild, pPasted, pOrig, pParent, pSibling,
                       pAncest, pE, pElAttr, newElement;
   PtrAttribute        pInheritLang, pLangAttr;
   PtrPasteElem        pPasteD;
   PtrSSchema          pSS;
   NotifyOnValue       notifyVal;
   NotifyElement       notifyEl;
   int                 NSiblings, i, asc, nR;
   ThotBool            stop, ok, possible, isCell;

   pPasted = NULL;
   pAncest = NULL;
   *cancelled = FALSE;
   if (addedCell)
     pOrig = addedCell;
   else
     {
       pOrig = pSavedEl->PeElement;
       isCell = TypeHasException (ExcIsCell, pOrig->ElTypeNumber,
				  pOrig->ElStructSchema);
       /* don't paste a cell if the enclosing row is not pasted */
       if (cellChild == NULL && isCell)
	 {
	   if (pOrig && pOrig->ElFirstChild)
	     {
	       /* paste children of the cell instead of the cell itself */
	       pOrig = pOrig->ElFirstChild;
	       if (pOrig->ElStructSchema &&
		   !strcmp (pOrig->ElStructSchema->SsName, "MathML"))
		 /* go down one mor step */
		 pOrig = pOrig->ElFirstChild;
	       *cellChild = pOrig;
	       isCell = FALSE;
	     }
	 }
     }
   ok = FALSE;
   if (within)
     {
       /* verifie si l'element peut etre un fils de pEl */
       ok = AllowedFirstChild (pEl, pDoc, pOrig->ElTypeNumber,
			       pOrig->ElStructSchema, TRUE, FALSE);
       if (!ok)
	 /* refus.  Essaie de le coller devant pEl */
	 {
	   within = FALSE;
	   before = TRUE;
	 }
     }
   if (!ok)
     /* verifie si l'element peut etre colle' au meme niveau que pEl */
     ok = AllowedSibling (pEl, pDoc, pOrig->ElTypeNumber,
			  pOrig->ElStructSchema, before, TRUE, FALSE);
   
   pElem = pEl;
   if (!ok)
     /* l'element ne peut pas etre colle' au meme niveau */
     /* s'il faut coller en debut ou fin d'element, on essaie de remonter */
     /* d'un ou plusieurs niveaux */
     {
       while (!ok && pElem)
	 if ((before && pElem->ElPrevious == NULL) ||
	     (!before && pElem->ElNext == NULL))
	   {
	     pElem = pElem->ElParent;
	     if (pElem)
	       ok = AllowedSibling (pElem, pDoc, pOrig->ElTypeNumber,
				    pOrig->ElStructSchema, before,
				    TRUE, FALSE);
	   }
	 else
	   pElem = NULL;
     }
   
   if (pElem == NULL)
     pElem = pEl;
   
   /* futur pere de l'element colle' */
   if (within)
     pParent = pElem;
   else
     pParent = pElem->ElParent;
   /* on calcule le nombre de freres qui precederont l'element */
   /* lorsqu'il sera mis dans l'arbre abstrait */
   NSiblings = 0;
   if (!within)
     {
       pSibling = pElem;
       while (pSibling->ElPrevious != NULL)
	 {
	   NSiblings++;
	   pSibling = pSibling->ElPrevious;
	 }
       if (!before)
         NSiblings++;
     }

   if (!ok && !addedCell)
     {
       /* essaie de creer des elements englobants pour l'element a coller */
       /* on se fonde pour cela sur le type des anciens elements ascendants */
       /* de l'element a coller */
       stop = FALSE;
       /* on commence par l'ancien element pere de l'element a coller */
       asc = 0;
       while (!stop)
	 {
	   if (pSavedEl->PeAscendTypeNum[asc] == 0 ||
	       pSavedEl->PeAscendSSchema[asc] == NULL)
	     /* on a examine' tous les anciens ascendants, sans succes */
	     stop = TRUE;
	   else
	     {
	       pElem = pEl;
	       possible = FALSE;
	       /* l'element englobant de l'element a coller peut-il etre un
		  voisin de l'element a cote' duquel on colle ? */
	       if (AllowedSibling (pElem, pDoc, pSavedEl->PeAscendTypeNum[asc],
				   pSavedEl->PeAscendSSchema[asc], before,
				   TRUE, FALSE))
		 /* oui ! */
		 possible = TRUE;
	       else
		 /* non, on regarde s'il peut etre un voisin d'un ascendant
		    de l'element a cote' duquel on colle */
		 {
		   while (!possible && pElem != NULL)
		     if ((before && pElem->ElPrevious == NULL) ||
			 (!before && pElem->ElNext == NULL))
		       {
			 pElem = pElem->ElParent;
			 if (pElem != NULL)
			   possible = AllowedSibling (pElem, pDoc,
					    pSavedEl->PeAscendTypeNum[asc],
					    pSavedEl->PeAscendSSchema[asc],
					    before, TRUE, FALSE);
		       }
		     else
		       pElem = NULL;
		 }
	       if (!possible)
		 /* cet ascendant ne convient pas, on essaie l'ascendant
		    de niveau superieur */
		 {
		   if (asc >= MAX_PASTE_LEVEL - 1)
		     /* il n'y en a plus. on arrete */
		     stop = TRUE;
		   else
		     asc++;
		 }
	       else
		 /* cet ascendant convient, on va creer un element de ce type*/
		 {
		   stop = TRUE;
		   /* Look for the Structure schema of the element to be
		      created */
		   pSS = pSavedEl->PeAscendSSchema[asc];
		   if (pDoc != DocOfSavedElements)
		     {
		     if (strcmp (pDoc->DocSSchema->SsName, pSS->SsName) == 0)
		       pSS = pDoc->DocSSchema;
		     else
		       {
			 /* loads the structure and presentation schemes for
			    the new element */
			 /* no preference for the presentation scheme */
			 nR = CreateNature (pSS->SsName, NULL,
					    pDoc->DocSSchema, pDoc);
			 if (nR != 0)
			   {
			   /* schemes are loaded, changes the structure schema
			      of the copy */
			   pSS = pDoc->DocSSchema->SsRule->SrElem[nR - 1]->SrSSchemaNat;
			   AddSchemaGuestViews (pDoc, pSS);
			   }
		       }
		     }
		   /* check if the application allows the creation of the element */
		   notifyEl.event = TteElemNew;
		   notifyEl.document = (Document) IdentDocument (pDoc);
		   notifyEl.element = (Element) (pParent);
		   notifyEl.info = 0; /* not sent by undo */
		   notifyEl.elementType.ElTypeNum = pSavedEl->PeAscendTypeNum[asc];
		   notifyEl.elementType.ElSSchema = (SSchema) (pSS);
		   notifyEl.position = NSiblings;
		   if (CallEventType ((NotifyEvent *) & notifyEl, TRUE))
		     /* l'application refuse */
		     {
		       stop = TRUE;
		       *cancelled = TRUE;
		     }
		   else
		     {
		       pAncest = NewSubtree (pSavedEl->PeAscendTypeNum[asc],
					     pSS, pDoc, FALSE, TRUE, TRUE,
					     TRUE);
		       if (pAncest != NULL)
			 {
			   /* on insere ce nouvel element dans l'arbre
			      abstrait */
			   InsertPastedElement (pElem, within, before,
						&pAncest, pDoc);
			   /* on envoie un evenement ElemNew.Post a
			      l'application */
			   notifyEl.event = TteElemNew;
			   notifyEl.document = (Document) IdentDocument (pDoc);
			   notifyEl.element = (Element) pAncest;
			   notifyEl.info = 0; /* not sent by undo */
			   notifyEl.elementType.ElTypeNum =
			                                 pAncest->ElTypeNumber;
			   notifyEl.elementType.ElSSchema =
			                   (SSchema) (pAncest->ElStructSchema);
			   notifyEl.position = 0;
			   CallEventType ((NotifyEvent *) & notifyEl, FALSE);
			   ok = TRUE;
			   /* si on vient de creer un element d'une nature
			      differente de son pere et que l'element a coller
			      est de cette nature, on essaie de coller comme
			      fils de l'element qu'on vient de creer */
			   if (pAncest->ElParent &&
			       pAncest->ElStructSchema != pAncest->ElParent->ElStructSchema &&
			       pAncest->ElStructSchema == pOrig->ElStructSchema)
			     if (AllowedFirstChild (pAncest, pDoc,
						    pOrig->ElTypeNumber,
						    pOrig->ElStructSchema,
						    TRUE, FALSE))
			       /* ca marche. On ne cree pas les intermediaires */
			       asc = 0;

			   /* on cree les ascendants intermediaires */
			   pParent = pAncest;
			   while (asc > 0)
			     {
			       asc--;
			       pSS = pSavedEl->PeAscendSSchema[asc];
			       if (pDoc != DocOfSavedElements)
				 {
				 if (strcmp (pDoc->DocSSchema->SsName,
					      pSS->SsName) == 0)
				   pSS = pDoc->DocSSchema;
				 else
				   {
				     /* loads the structure and presentation schemes for the new element */
				     /* no preference for the presentation scheme */
				     nR = CreateNature (pSS->SsName, NULL, pDoc->DocSSchema, pDoc);
				     if (nR != 0)
				       {
				       /* schemes are loaded, changes the structure scheme of the copy */
				       pSS = pDoc->DocSSchema->SsRule->SrElem[nR - 1]->SrSSchemaNat;
				       AddSchemaGuestViews (pDoc, pSS);
				       }
				   }
				 }
			       /* demande a l'application si on peut creer ce type d'elem. */
			       notifyEl.event = TteElemNew;
			       notifyEl.document = (Document) IdentDocument (pDoc);
			       notifyEl.element = (Element) (pParent);
			       notifyEl.info = 0; /* not sent by undo */
			       notifyEl.elementType.ElTypeNum = pSavedEl->PeAscendTypeNum[asc];
			       notifyEl.elementType.ElSSchema = (SSchema) pSS;
			       notifyEl.position = NSiblings;
			       if (CallEventType ((NotifyEvent *) & notifyEl, TRUE))
				 /* l'application refuse */
				 {
				   DeleteElement (&pAncest, pDoc);
				   stop = TRUE;
				   ok = FALSE;
				   *cancelled = TRUE;
				   asc = 0;
				 }
			       else
				 /* l'application accepte, on cree l'element */
				 {
				   pE = NewSubtree (pSavedEl->PeAscendTypeNum[asc],
						    pSS, pDoc, FALSE, TRUE,
						    TRUE, TRUE);
				   /* on insere ce nouvel element dans l'arbre abstrait */
				   InsertFirstChild (pParent, pE);
				   /* on envoie un evenement ElemNew.Post a l'appli */
				   notifyEl.event = TteElemNew;
				   notifyEl.document = (Document) IdentDocument (pDoc);
				   notifyEl.element = (Element) pE;
				   notifyEl.info = 0; /* not sent by undo */
				   notifyEl.elementType.ElTypeNum = pE->ElTypeNumber;
				   notifyEl.elementType.ElSSchema = (SSchema) (pE->ElStructSchema);
				   notifyEl.position = 0;
				   if (pE->ElTypeNumber == pE->ElStructSchema->SsRootElem)
				     /* root element in a different structure schema */
				     /* Put number of elements in the "position" field */
				     notifyEl.position = 0;
				   CallEventType ((NotifyEvent *) & notifyEl, FALSE);
				   /* passe au niveau inferieur */
				   pParent = pE;
				 }
			     }
			 }
		     }
		 }
	     }
	 }
     }

   if (!ok)
     /* on essaie de couper en deux un element englobant la position ou on */
     /* essaie de coller */
     pElem = pEl;

   if (ok)
     {
       if (pAncest != NULL)
	 /* on a cree' des elements ascendant, l'element a coller sera
	    insere' comme premier fils */
	 NSiblings = 0;
       /* envoie un evenement a l'application */
       notifyVal.event = TteElemPaste;
       notifyVal.document = (Document) IdentDocument (pDoc);
       notifyVal.element = (Element) pParent;
       notifyVal.target = (Element) pOrig;
       notifyVal.value = NSiblings;
       if (CallEventType ((NotifyEvent *) (&notifyVal), TRUE))
	 /* l'application refuse */
	 *cancelled = TRUE;
       else
	 /* l'application accepte */
	 {
	   /* Cree une copie de l'element a coller. */
	   /* Si l'element est reference', la copie devient l'element
	      reference' */
	   /* Ne copie les attributs que s'ils sont definis dans les schemas */
	   /* de structure des elements englobants du document d'arrivee. */
	   pPasted = CopyTree (pOrig, DocOfSavedElements,
			       pParent->ElStructSchema, pDoc, pParent, TRUE,
			       TRUE, FALSE, TRUE);
	   if (pPasted != NULL)
	     {
	       /* insere la copie dans l'arbre */
	       if (pAncest == NULL)
		 {
		   InsertPastedElement (pElem, within, before, &pPasted, pDoc);
		   newElement = pPasted;
		 }
	       else
		 {
		   InsertFirstChild (pParent, pPasted);
		   newElement = pAncest;
		 }
	       /* Retire l'attribut Langue de l'element colle', s'il
		  herite de la meme valeur */
	       /* cherche d'abord la valeur heritee */
	       pInheritLang = GetTypedAttrAncestor (pPasted, 1, NULL,&pElAttr);
	       if (pInheritLang != NULL)
		 {
		   /* cherche l'attribut Langue de pPasted */
		   pLangAttr = GetTypedAttrForElem (pPasted, 1, NULL);
		   if (pLangAttr != NULL)
		     /* compare les valeurs de ces 2 attributs */
		     if (TextsEqual (pInheritLang->AeAttrText,
				     pLangAttr->AeAttrText))
		       /* attributs egaux, on supprime celui de pPasted */
		       {
			 RemoveAttribute (pPasted, pLangAttr);
			 DeleteAttribute (pPasted, pLangAttr);
		       }
		 }
	       /* garde le pointeur sur le sous-arbre colle' */
	       CreatedElement[NCreatedElements] = newElement;
	       NCreatedElements++;
	     }
	 }
     }
   else
     {
       if (!pOrig->ElTerminal)
	 /* try to paste the content of the element instead of the element */
	 /* itself */
	 {
	   pPasteD = (PtrPasteElem) TtaGetMemory (sizeof (PasteElemDescr));
	   pPasteD->PePrevious = NULL;
	   pPasteD->PeNext = NULL;
	   pPasteD->PeElemLevel = 0;
	   for (i = 0; i < MAX_PASTE_LEVEL; i++)
	     {
	       pPasteD->PeAscendTypeNum[i] = 0;
	       pPasteD->PeAscendSSchema[i] = NULL;
	       pPasteD->PeAscend[i] = NULL;
	     }
	   /* paste all the children here */
	   pChild = pOrig->ElFirstChild;
	   pElem = pEl;
	   pPasted = NULL;
	   if (before && pChild != NULL)
	     /* insert first the last element before */
	     while (pChild->ElNext != NULL)
	       pChild = pChild->ElNext;
	   while (pChild && pElem)
	     {
	       pPasteD->PeElement = pChild;
	       /* don't check enclosed cells */
	       pElem = PasteAnElement (pElem, pPasteD, within, before,
				       cancelled, pDoc, &pChild, NULL);
	       if (pElem)
		 {
		   /* pointer to the first element in the inserted list */
		   pPasted = pElem;
		   within = FALSE;
		   /* next element to be inserted */
		   if (before)
		     pChild = pChild->ElPrevious;
		   else
		     pChild = pChild->ElNext;
		 }
	     }
	   TtaFreeMemory (pPasteD);
	 }
     }
   return pPasted;
}


/*----------------------------------------------------------------------
  PasteCommand
  ----------------------------------------------------------------------*/
void PasteCommand ()
{
  PtrDocument         pDoc;
  PtrElement          firstSel, lastSel, pEl, pPasted, pClose, pFollowing,
                      pNextEl, pFree, pSplitText, pSel, cellChild;
  PtrElement          pColHead, pNextCol, pRow, pNextRow, pTable, pRealCol,
                      pCe, addedCell, pCell, extendedCell[500];
  PtrPasteElem        pPasteD;
  ElementType         cellType;
  DisplayMode         dispMode;
  Document            doc;
  int                 firstChar, lastChar, view, i, nRowsTempCol, info = 0;
  int                 colspan, rowspan, back, nbextended;
  ThotBool            ok, before, within, lock, cancelled, first, beginning;
  ThotBool            savebefore;

  before = FALSE;
  nbextended = 0;
  pColHead = pRow = pNextRow = pTable = NULL;
  if (FirstSavedElement == NULL)
    return;
  if (GetCurrentSelection (&pDoc, &firstSel, &lastSel, &firstChar, &lastChar))
    {
      /* cannot paste within a read only document */
      if (!pDoc->DocReadOnly)
	{
	  /* compute the view volume */
	  for (view = 0; view < MAX_VIEW_DOC; view++)
	    {
	      if (pDoc->DocView[view].DvPSchemaView > 0)
		pDoc->DocViewFreeVolume[view] = pDoc->DocViewVolume[view];
	    }

	  pSplitText = NULL;
	  pNextEl = NULL;
	  doc = IdentDocument (pDoc);
	  dispMode = TtaGetDisplayMode (doc);
	  /* lock tables formatting */
	  if (ThotLocalActions[T_islock])
	    {
	      (*(Proc1)ThotLocalActions[T_islock]) ((void *)&lock);
	      if (!lock)
		{
		  if (dispMode == DisplayImmediately)
		    TtaSetDisplayMode (doc, DeferredDisplay);
		  /* table formatting is not loked, lock it now */
		  (*ThotLocalActions[T_lock]) ();
		}
	    }

	  pCell = NULL;
	  if (WholeColumnSaved || TableRowsSaved)
	    {
	      pCell = firstSel;
	      beginning = (firstChar < 2);
	      /* look for the enclosing cell */
	      while (pCell &&
		     !TypeHasException (ExcIsCell, pCell->ElTypeNumber,
					pCell->ElStructSchema))
		{
		  if (pCell->ElPrevious)
		    /* the selection is not at the beginning of the cell */
		    beginning = FALSE;
		  pCell = pCell->ElParent;
		}
	      if (pCell)
		before = beginning;
	    }

	  if (WholeColumnSaved && pCell)
	    {
	      /* paste a column in a table */
	      /* look for the current column position */
	      pColHead = GetColHeadOfCell (pCell);
	      pTable = pColHead;
	      while (pTable &&
		     !TypeHasException (ExcIsTable,
					pTable->ElTypeNumber,
					pTable->ElStructSchema))
		pTable = pTable->ElParent;

	      cellType.ElTypeNum = pCell->ElTypeNumber;
	      cellType.ElSSchema = (SSchema) pCell->ElStructSchema;
	      if (!before)
		{
		  /* get the last column spanned by the cell */
		  GetCellSpans (pCell, &colspan, &rowspan);
		  if (colspan == 0)
		    return;
		  while (colspan > 1 && pColHead)
		    {
		      pColHead = NextColumnInTable (pColHead, pTable);
		      colspan--;
		    }
		}
	      /* current row */
	      pRow = pCell->ElParent;
	      if (pRow && pColHead)
		/* get the first row in the table */
		pRow = FwdSearchTypedElem (pColHead, pRow->ElTypeNumber,
					   pRow->ElStructSchema);
	      else
		pRow = NULL;
	      /* change the selection to paste a whole column */
	      pEl = pCell;
	      within = FALSE;
	      pNextEl = NULL;
	    }
	  else if (TableRowsSaved && pCell)
	    /* the clipboard contains a (sequence of) table rows */
	    /* and the current selection is within a table cell */
	    {
	      pRow = pCell->ElParent;
	      pEl = pRow;
	      within = FALSE;
	      pNextEl = NULL;
	    }
	  else if (firstChar == 0 && lastChar == 0 && firstSel == lastSel &&
		   firstSel->ElVolume == 0 && !firstSel->ElTerminal)
	    /* an empty non terminal element is selected. Paste within it */
	    {
	      pEl = firstSel;
	      within = TRUE;
	    }
	  else if (firstSel->ElTerminal &&
		   ((firstSel->ElLeafType == LtPicture &&
		     firstChar == 1) ||
		    (firstSel->ElLeafType == LtText &&
		     firstChar > firstSel->ElTextLength)))
	    /* the right edge of an image is selected or the selection starts
	       at the end of a text element. Paste after the selected element*/
	    {
	      pEl = firstSel;
	      within = FALSE;
	      before = FALSE;
	      pNextEl = NextElement (pEl);
	    }
	  else if (firstChar < 2)
	    /* paste before element firstSel */
	    {
	      pEl = firstSel;
	      within = FALSE;
	      before = TRUE;
	      /* the element that will follow the pasted elements is the first
		 one in the current selection */
	      pNextEl = firstSel;
	    }
	  else
	    /* the user wants to paste in the middle of a text leaf. Split it*/
	    {
	      /* if the element to be split is the last child of its parent,
		 it will change status after the split */
	      pClose = firstSel->ElNext;
	      FwdSkipPageBreak (&pClose); /* skip page breaks */
	      pSplitText = firstSel;
	      SplitTextElement (firstSel, firstChar, pDoc, TRUE, &pFollowing,
				FALSE);
	      /* update the current selection */
	      if (firstSel == lastSel)
		{
		  lastSel = pFollowing;
		  lastChar = lastChar - firstChar + 1;
		}
	      firstSel = firstSel->ElNext;
	      firstChar = 1;
	      pEl = pSplitText;
	      within = FALSE;
	      before = FALSE;
	      /* the element that will follow the pasted elements is the second
		 part of the split text */
	      pNextEl = pFollowing;
	    }

          /* take all elements to be pasted and paste them one after the
	     other */
	  NCreatedElements = 0;
	  pPasteD = FirstSavedElement;
	  first = TRUE;
	  if (!within && before && pPasteD && !WholeColumnSaved)
	    /* pasting before an existing element. We will start by pasting
	       the last element and we will proceed backwards */
	    while (pPasteD->PeNext)
	      pPasteD = pPasteD->PeNext;
	  ok = FALSE;
	  cellChild = NULL;
	  addedCell = NULL; /* no cell generated */
	  nRowsTempCol = 0;
	  do
	    {
	      if (WholeColumnSaved)
		{
		  /* look for the cell in that row and that column or
		     in a previous column */
		  pEl = GetCellInRow (pRow, pColHead, TRUE, &back);
		  if (pEl == NULL && pRow)
		    /* that row contains no cell in that column or in any
		       preceding column */
		    {
		      pNextCol = pColHead;
		      while (pEl == NULL && pNextCol)
			{
			  /* paste before the cell in the next column */
			  savebefore = before;
			  before = TRUE;
			  nRowsTempCol = 1;
			  pRealCol = pColHead;
			  pNextCol = NextColumnInTable (pNextCol, pTable);
			  if (pNextCol)
			    pEl = GetCellInRow (pRow, pNextCol, FALSE, &back);
			}
		    }
		  else
		    {
		      GetCellSpans (pEl, &colspan, &rowspan);
		      if ((!before && (colspan == 0 || colspan - back > 1)) ||
			  (before && back > 0))
			/* extend this cell instead of pasting the new cell */
			{
			  extendedCell[nbextended] = pEl;
			  nbextended++;
			  pEl = NULL;
			  /* move to the bottom of this cell */
			  if (rowspan == 0)
			    /* infinite vertical spanning. Stop */
			    pRow = NULL;
			  else
			    while (pRow && rowspan > 1)
			      {
				pRow = NextRowInTable (pRow, pTable);
				if (pPasteD)
				  pPasteD = pPasteD->PeNext;
				if (rowspan > 0)
				  rowspan--;
			      }
			}
		      else if (before && back == 0 &&
			       (rowspan > 1 || rowspan == 0))
			/* there is a cell here and we can paste a new cell
			   before it, but it spans several rows. In these
			   spanned rows, we will paste cells after the
			   previous column */
			{
			  pRealCol = pColHead;
			  /* get the previous column */
			  pColHead = pColHead->ElPrevious;
			  if (pColHead)
			    /* there is a previous column */
			    {
			      before = FALSE;
			      if (rowspan != 0)
				{
			          nRowsTempCol = rowspan;
			          savebefore = TRUE;
				}
			    }
			  else
			    /* no previous column. get the next column that
			       has a cell in the current row and paste before
			       in the spanned rows */
			    {
			      pColHead = pRealCol;
			      do
				{
				  pCe = GetCellInRow (pRow, pColHead, FALSE,
						      &back);
				  if (!pCe)
				    pColHead = NextColumnInTable (pColHead,
								  pTable);
				}
			      while (!pCe && pColHead);
			      if (pColHead)
				{
				  before = TRUE;
				  if (rowspan != 0)
				    {
				      nRowsTempCol = rowspan;
				      savebefore = TRUE;
				    }
				}
			      else
				/* no cell in the spanned row */
				{
				  /* What sould we do? */;
				} 
			    }
			}
		    }
		  if (pRow)
		    pNextRow = NextRowInTable (pRow, pTable);
		  else
		    pNextRow = NULL;
		}
	      if (pEl)
		pPasted = PasteAnElement (pEl, pPasteD, within, before,
				      &cancelled, pDoc, &cellChild, addedCell);
	      else
		pPasted = NULL;
	      if (pPasted == NULL && !WholeColumnSaved && !cancelled &&
		  /* failure, but it's not on application's refusal */
		  !within && !before && pNextEl)
		/* we were trying to paste after the last pasted element.
		   We will try to paste the same element before the element
		   that is supposed to follow the pasted elements */
		pPasted = PasteAnElement (pNextEl, pPasteD, within, TRUE,
				      &cancelled, pDoc, &cellChild, addedCell);
	      if (pPasted)
		/* a copy of element pPasteD has been sucessfully pasted */
		{
		  ok = TRUE;
		  pEl = pPasted;
		  if (within)
		    /* next element will be pasted after the previous one*/
		    {
		      within = FALSE;
		      savebefore = before;
		      nRowsTempCol = 1;
		      before = FALSE;
		      pRealCol = pColHead;
		    }
		  else if (WholeColumnSaved)
		    {
		      /* get the last column of the cell */
		      GetCellSpans (pPasted, &colspan, &rowspan);
		      while (pNextRow && ((rowspan > 1) || (rowspan == 0)))
			{
			  pNextRow = NextRowInTable (pNextRow, pTable);
			  if (rowspan > 0)
			    rowspan--;
			}
		    }
		}

	      /* get the next pasted element */
	      if (!within && before && !WholeColumnSaved)
		{
		  if (cellChild)
		    {
		      cellChild = cellChild->ElPrevious;
		      if (cellChild == NULL)
			pPasteD = pPasteD->PePrevious;
		    }
		  else
		    pPasteD = pPasteD->PePrevious;
		}
	      else
		{
		  if (cellChild)
		    {
		      cellChild = cellChild->ElNext;
		      if (cellChild == NULL)
			pPasteD = pPasteD->PeNext;
		    }
		  else if (addedCell)
		    /* remove this generated cell */
		    DeleteElement (&addedCell, pDoc);
		  else
		    pPasteD = pPasteD->PeNext;
		}

	      pRow = pNextRow;
	      if (pRow && pPasteD == NULL)
		/* there are more rows than pasted cell. Add empty cells */
		addedCell = NewSubtree (cellType.ElTypeNum,
					(PtrSSchema) cellType.ElSSchema, pDoc,
					TRUE, TRUE, TRUE, TRUE);
	      if (nRowsTempCol > 0)
		{
		  nRowsTempCol --;
		  if (nRowsTempCol == 0)
		    {
		      before = savebefore;
		      pColHead = pRealCol;
		    }
		}
	    }
	  while (pPasteD || addedCell);

	if (ok)
	  /* we have successfully pasted the contents of the buffer */
	  {
	    /* labels will have to be change if the same elements are pasted
	       again later */
	    ChangeLabel = TRUE;
	    if (pSplitText != NULL)
	      /* a text element was split to insert the pasted element */
	      {
		/* build the abstract boxes of the split text */
		BuildAbsBoxSpliText (pSplitText, pFollowing, pClose, pDoc);
	      }
	    /* process all references ans exclusions in the pasted elements,
	       based on their new context */
	    for (i = 0; i < NCreatedElements; i++)
	      {
		CheckReferences (CreatedElement[i], pDoc);
		RemoveExcludedElem (&CreatedElement[i], pDoc);
	      }
	    /* set IDs to all paired elements */
	    for (i = 0; i < NCreatedElements; i++)
	      AssignPairIdentifiers (CreatedElement[i], pDoc);
	    /* register the pasted elements in the editing history */
	    OpenHistorySequence (pDoc, firstSel, lastSel, NULL, firstChar,
				 lastChar-1);
	    /* send event ElemPaste.Post */
	    for (i = 0; i < NCreatedElements; i++)
	      if (CreatedElement[i])
	        {
		  if (WholeColumnSaved)
		    {
		      /* change the value of "info" in the latest cell
			 deletion recorded in the Undo queue.
			 The goal is to allow procedure CellPasted
			 to regenerate only one column head when
			 undoing the operation */
		      if (first)
			{
			  info = 4;
			  first = FALSE;
			}
		      else
			info = 3;
		      TtaChangeInfoLastRegisteredElem (doc, info);
		    }
		  NotifySubTree (TteElemPaste, pDoc, CreatedElement[i],
				 IdentDocument (DocOfSavedElements), info,
				 FALSE, FALSE);
		  if (CreatedElement[i]->ElStructSchema == NULL)
		    /* application has deleted that element */
		    CreatedElement[i] = NULL;
		  else
		    AddEditOpInHistory (CreatedElement[i], pDoc, FALSE, TRUE);
		}

	    for (i = 0; i < nbextended; i++)
	      if (extendedCell[i])
	        {
		  NotifyHExtension (extendedCell[i], doc);
		  extendedCell[i] = NULL;
		}

	    /* close the history sequence after applications have possibly
	       registered more changes to the pasted elements */
	    CloseHistorySequence (pDoc);
	    
	    TtaClearViewSelections ();
	    for (i = 0; i < NCreatedElements; i++)
	      if (CreatedElement[i] != NULL)
		{
		  /* create the abstract boxes of the new elements in all views */
		  CreateNewAbsBoxes (CreatedElement[i], pDoc, 0);
		  /* compute the volume that the created abstract box can use*/
		  for (view = 0; view < MAX_VIEW_DOC; view++)
		    {
		      if (CreatedElement[i]->ElAbstractBox[view] != NULL)
			pDoc->DocViewFreeVolume[view] -=
			  CreatedElement[i]->ElAbstractBox[view]->AbVolume;
		    }
		}
	    /* Apply all delayed presentation rules if some are left */
	    for (i = 0; i < NCreatedElements; i++)
	      if (CreatedElement[i] != NULL)
		ApplDelayedRule (CreatedElement[i], pDoc);
	    
	    AbstractImageUpdated (pDoc);
	    /* display the new elements */
	    RedisplayDocViews (pDoc);

	    for (i = 0; i < NCreatedElements; i++)
	      if (CreatedElement[i] != NULL)
		{
		  /* update the presentation of reference attributes that
		     point at pasted elements */
		  UpdateRefAttributes (CreatedElement[i], pDoc);
		}
	  }
	else
	  /* failure */
	  {
	    if (pSplitText != NULL)
	      /* A text element was split. Merge the two pieces back. */
	      {
		MergeTextElements (pSplitText, &pFree, pDoc, TRUE, FALSE);
		DeleteElement (&pFree, pDoc);
		pFree = NULL;
	      }
	  }

	if (!lock)
	  {
	    /* unlock table formatting */
	    (*ThotLocalActions[T_unlock]) ();
	    if (dispMode == DisplayImmediately)
	      TtaSetDisplayMode (doc, DisplayImmediately);
	  }

	/* set the selection at the end of the pasted elements */
	if (NCreatedElements > 0)
	  {
	    pSel = NULL;
	    if (before)
	      {
		for (i = 0; i < NCreatedElements && !pSel; i++)
		  if (CreatedElement[i] != NULL)
		    pSel = CreatedElement[i];
	      }
	    else
	      {
		for (i = NCreatedElements - 1; i >= 0 && !pSel; i--)
		  if (CreatedElement[i] != NULL)
		    pSel = CreatedElement[i];
	      }
	    if (pSel)
	      if (!pSel->ElTerminal)
		pSel = LastLeaf (pSel);
	    if (pSel)
	      SelectString (pDoc, pSel, pSel->ElTextLength + 1,
			    pSel->ElTextLength);
	    
	    SetDocumentModified (pDoc, TRUE, 20);
	    
	    /* update the counter values that follow the pasted elements */
	    for (i = 0; i < NCreatedElements; i++)
	      if (CreatedElement[i] != NULL)
		{
		  RedisplayCopies (CreatedElement[i], pDoc, TRUE);
		  UpdateNumbers (CreatedElement[i], CreatedElement[i], pDoc,
				 TRUE);
		}
	  }
      }
    }
}

/*----------------------------------------------------------------------
   SelectSiblings
  ----------------------------------------------------------------------*/
void SelectSiblings (PtrElement *firstEl, PtrElement *lastEl,
		     int *firstChar, int *lastChar)
{
   PtrElement          pParent;

   if (!SelContinue)
     /* discrete selection. don't do anything */
     return;
   if (SelectedColumn)
     /* We are in column selection mode */
     {
       /* if the selection starts at the beginning of an element, select
	  this element, but don't go higher than the cell element */
       if ((*firstEl)->ElPrevious == NULL && *firstChar <= 1)
	 {
	   pParent = *firstEl;
	   do
	     {
	       if (TypeHasException (ExcIsCell, pParent->ElTypeNumber,
				     pParent->ElStructSchema))
		 /* this is a cell. Stop */
		 pParent = NULL;
	       else
		 {
		   /* use this element as the beginning of the selection
		      and check the upper level */
		   *firstEl = pParent;
		   *firstChar = 0;
		   pParent = pParent->ElParent;
		 }
	     }
	   while (pParent && !pParent->ElPrevious);
	 }
       /* if the selection ends at the end of an element, select
	  this element, but don't go higher than the cell element */
       if ((*lastEl)->ElNext == NULL && (*lastChar == 0 ||
					 *lastChar > (*lastEl)->ElTextLength))
	 {
	   pParent = *lastEl;
	   do
	     {
	       if (TypeHasException (ExcIsCell, pParent->ElTypeNumber,
				     pParent->ElStructSchema))
		 /* this is a cell. Stop */
		 pParent = NULL;
	       else
		 {
		   /* use this element as the end of the selection */
		   *lastEl = pParent;
		   *lastChar = 0;
		   pParent = pParent->ElParent;
		 }
	     }
	   while (pParent && !pParent->ElNext);
	 }
       return;
     }
   else if ((*firstEl)->ElParent != (*lastEl)->ElParent)
      /* essaie de ramener la selection a une suite de freres */
     {
	if ((*firstEl)->ElPrevious == NULL && *firstChar <= 1)
	  {
	     /* remonte les ascendants du premier element */
	     pParent = (*firstEl)->ElParent;
	     while (pParent != NULL)
		if (ElemIsAnAncestor (pParent, *lastEl))
		   /* cet ascendant (pParent) du premier element est aussi */
		   /*  un ascendant du dernier */
		  {
		     while ((*lastEl)->ElNext == NULL &&
			    (*lastChar == 0 ||
			     *lastChar > (*lastEl)->ElTextLength) &&
			    (*lastEl)->ElParent != pParent)
		       {
			  *lastEl = (*lastEl)->ElParent;
			  *lastChar = 0;
		       }
		     /* on a fini */
		     pParent = NULL;
		  }
		else
		   /* cet ascendant (pParent) du premier element n'est pas */
		   /* un ascendant du dernier */
		  {
		     /* on retient pour l'instant pParent et on va regarder */
		     /*  si son pere est un ascendant du dernier */
		     if ((*firstEl)->ElPrevious == NULL)
		       {
			  *firstChar = 0;
			  *firstEl = pParent;
			  pParent = pParent->ElParent;
		       }
		     else
			pParent = NULL;
		  }
	  }
	if ((*firstEl)->ElParent != (*lastEl)->ElParent)
	   /* essaie de remonter la selection du dernier */
	  {
	     if ((*lastEl)->ElNext == NULL)
		if (*lastChar == 0 || *lastChar > (*lastEl)->ElTextLength)
		  {
		     /* remonte les ascendants du dernier element */
		     pParent = (*lastEl)->ElParent;
		     while (pParent != NULL)
			if (ElemIsAnAncestor (pParent, *firstEl))
			   /* cet ascendant (pParent) du dernier element est */
			   /* aussi un ascendant du premier */
			   pParent = NULL;	/* on a fini */
			else
			   /* cet ascendant (pParent) du dernier element */
			   /* n'est pas un ascendant du premnier */
			  {
			     /* on retient pour l'instant pParent et on va */
			     /* voir si son pere est un ascendant du premier */
			     if ((*lastEl)->ElNext == NULL)
			       {
				  *lastEl = pParent;
				  pParent = pParent->ElParent;
				  *lastChar = 0;
			       }
			     else
				pParent = NULL;
			  }
		  }
	  }
     }
}


/*----------------------------------------------------------------------
   ReturnCreateNewElem
   verifie si la touche Return frappee a la fin (ou au debut, selon begin)
   de l'element pEl (qui fait partie de l'element liste pListEl) doit 
   creer un element de meme type que pEl ou un element d'un type different.
   Retourne le type de l'element a creer dans (typeNum, pSS).
  ----------------------------------------------------------------------*/
static void ReturnCreateNewElem (PtrElement pListEl, PtrElement pEl,
				 ThotBool begin, PtrDocument pDoc, int *typeNum,
				 PtrSSchema *pSS)
{
   int              TypeListe, TypeElListe, TypeEl;
   int	            nComp;
   PtrSSchema       pSSList;
   PtrSRule         pRegle;

   pSSList = pListEl->ElStructSchema;
   *pSS = pEl->ElStructSchema;
   *typeNum = pEl->ElTypeNumber;
   if (GetElementConstruct (pListEl, &nComp) == CsAny)
     /* Don't check further for xml elements (CsAny) */
     return;
   TypeListe = GetTypeNumIdentity (pListEl->ElTypeNumber, pSSList);
   /* le type des elements qui constituent la liste */
   TypeElListe = pSSList->SsRule->SrElem[TypeListe - 1]->SrListItem;
   /* on traverse les regles d'Identite' */
   TypeEl = GetTypeNumIdentity (TypeElListe, pSSList);
   /* la regle qui definit les elements de la liste */
   pRegle = pSSList->SsRule->SrElem[TypeEl - 1];
   if (pRegle->SrConstruct == CsChoice)
      if (pRegle->SrNChoices > 0)
	 /* c'est une liste de choix, on retient la 1ere option de ce choix */
	 if (AllowedSibling (pEl, pDoc, pRegle->SrChoice[0], pSSList, begin,
			     TRUE,
			     FALSE))
	   {
	      *typeNum = pRegle->SrChoice[0];
	      *pSS = pSSList;
	   }
}

/*----------------------------------------------------------------------
  AscentReturnCreateNL
  returns the ancestor of element pEl which has an exception
  ExcReturnCreateNL.
  ----------------------------------------------------------------------*/
static PtrElement AscentReturnCreateNL (PtrElement pEl)
{
  PtrElement          pAncest;
  ThotBool            stop;

  stop = FALSE;
  if (pEl == NULL)
    return pEl;
  else
    {
      pAncest = pEl->ElParent;
      while (!stop && pAncest)
	{
	  if (TypeHasException (ExcReturnCreateNL, pAncest->ElTypeNumber,
				pAncest->ElStructSchema))
	    stop = TRUE;
	  else
	    pAncest = pAncest->ElParent;
	}
      return pAncest;
    }
}


/*----------------------------------------------------------------------
   EmptyOrConstants
   Returns TRUE if the next (or previous, depending on before) sibling of
   element pEl is absent or contains only constants
  ----------------------------------------------------------------------*/
static ThotBool EmptyOrConstants (PtrElement pEl)
{
   PtrElement          child;
   ThotBool	       ret;

   if (pEl->ElStructSchema->SsRule->SrElem[pEl->ElTypeNumber - 1]->SrConstruct == CsConstant)
      ret = TRUE;
   else
      if (pEl->ElTerminal)
	 ret = (pEl->ElVolume == 0);
      else
         {
         child = pEl->ElFirstChild;
         ret = TRUE;
         while (child && ret)
	   {
	   ret = EmptyOrConstants (child);
	   child = child->ElNext;
	   }
         }
   return ret;
}

/*----------------------------------------------------------------------
   NoSignificantSibling
   Returns TRUE if the next (or previous, depending on before) sibling of
   element pEl is absent or contains only constants
  ----------------------------------------------------------------------*/
static ThotBool NoSignificantSibling (PtrElement pEl, ThotBool before)
{
   PtrElement          pSibling;
   ThotBool	           ret;

   ret = FALSE;
   if (before)
      pSibling = pEl->ElPrevious;
   else
      pSibling = pEl->ElNext;

   if (pSibling == NULL)
      ret = TRUE;
   else
      if (EmptyOrConstants (pSibling))
	 ret = TRUE;
   return ret;
}

/*----------------------------------------------------------------------
  TtcInsertLineBreak handles the key "Control Return".
  ----------------------------------------------------------------------*/
void TtcInsertLineBreak (Document doc, View view)
{
  if (MenuActionList[0].Call_Action)
    (*(Proc3)MenuActionList[0].Call_Action) (
		(void *)doc,
		(void *)view,
		(void *)BREAK_LINE);
}

/*----------------------------------------------------------------------
  TtcCreateElement handles the key "Return".
  ----------------------------------------------------------------------*/
void TtcCreateElement (Document doc, View view)
{
  PtrElement          firstSel, lastSel, pListEl, pE, pNew, pSibling;
  PtrElement          pClose, pAncest, pElem, pParent, pElDelete, pPrevious;
  PtrElement          pNext, pElReplicate, pAggregEl;
  PtrDocument         pDoc;
  PtrSSchema          pSS;
  NotifyElement       notifyEl;
  int                 firstChar, lastChar, NSiblings;
  int                 frame, typeNum, nComp;
  ThotBool            ok, replicate, createAfter, selBegin, selEnd, ready;
  ThotBool            empty, list, optional, deleteEmpty, histSeq;
  ThotBool            lock = TRUE;
  DisplayMode         dispMode;

  if (!GetCurrentSelection (&pDoc, &firstSel, &lastSel, &firstChar, &lastChar))
    return;
  /* Check if we are changing the active frame */
  frame = GetWindowNumber (doc, view);
  if (frame != ActiveFrame)
    {
      /* yes close the previous insertion */
      CloseTextInsertion ();
      if (ActiveFrame > 0 && FrameTable[ActiveFrame].FrDoc != doc)
	return;
      else
	/* use the right frame */
	ActiveFrame = frame;
    }
  if (!ElementIsReadOnly (firstSel) && AscentReturnCreateNL (firstSel))
    /* one of the ancestors of the first selected element says that the
       Return key should generate a "new line" character */
    InsertChar (GetWindowNumber (doc, view), '\n', -1);
  else if (firstSel->ElParent && !ElementIsReadOnly (firstSel->ElParent))
    {
      /* lock the table formatting */
      if (ThotLocalActions[T_islock])
	{
	  (*(Proc1)ThotLocalActions[T_islock]) ((void *)&lock);
	  if (!lock)
	    /* table formatting is not locked, lock it now */
	    (*ThotLocalActions[T_lock]) ();
	}
      pListEl = NULL;
      pAggregEl = NULL;
      createAfter = TRUE;
      replicate = TRUE;
      ready = FALSE;
      empty = FALSE;
      list = TRUE;
      pElDelete = NULL;
      pElReplicate = NULL;
      typeNum = 0;
      pSS = NULL;
      deleteEmpty = FALSE;
      histSeq = FALSE;	/* no history sequence open */
      if (firstChar > 0 && firstChar == lastChar)
	lastChar--;

      /* si la selection ne comprend qu'un element vide, on essaie de */
      /* remplacer cet element vide par un autre au niveau superieur */
      if (firstSel == lastSel && firstSel->ElVolume == 0)
	{
	  empty = TRUE;
	  pElem = firstSel;
	  while (pElem->ElParent && EmptyElement (pElem->ElParent) &&
		 !TypeHasException (ExcNoBreakByReturn,
				    pElem->ElParent->ElTypeNumber,
				    pElem->ElParent->ElStructSchema))
	    pElem = pElem->ElParent;
	  if (pElem != NULL)
	    if (pElem->ElParent != NULL)
	      {
		pParent = pElem->ElParent;
		pListEl = AncestorList (pParent);
		if (TypeHasException (ExcNoBreakByReturn,pParent->ElTypeNumber,
				      pParent->ElStructSchema))
		  /* the parent element can't be split with the Return key.
		     Do not delete the empty element, but create a new copy
		     of it right after. */
		  {
		    ready = TRUE;
		    pElDelete = NULL;
		    pListEl = NULL;
		    createAfter = TRUE;
		    pElReplicate = pElem;
		  }
		else if (pListEl == NULL)
		  {
		    if (GetElementConstruct (pParent->ElParent, &nComp) == CsAggregate)
		      {
			SRuleForSibling (pDoc, pParent, FALSE, 1, &typeNum,
					 &pSS, &list, &optional);
			if (typeNum > 0)
			  if (TypeHasException (ExcNoCreate, typeNum,pSS))
			    typeNum = 0;
			if (typeNum == 0)
			  {
			    list = TRUE;
			    pListEl = NULL;
			    /* try to split before element pElem */
			    if (pElem->ElPrevious != NULL &&
				pElem->ElNext != NULL)
			      {
				/* store the editing operation in the history*/
				if (!histSeq)
				  {
				    OpenHistorySequence (pDoc, firstSel, lastSel,
						 NULL, firstChar, lastChar);
				    histSeq = TRUE;
				  }
				AddEditOpInHistory (pParent->ElParent, pDoc,
						    TRUE, TRUE);
				if (!BreakElement (pParent->ElParent, pElem,
						   0, FALSE, TRUE))
				  /* operation failed, remove it from history*/
				  CancelLastEditFromHistory (pDoc);
				else
				  /* element pParent has been split */
				  {
				    /* record the element that has been
				       created by BreakElement: it has to be
				       deleted when undoing the command */
				    AddEditOpInHistory (pParent->ElParent->ElNext, pDoc, FALSE, TRUE);
				    SRuleForSibling (pDoc, pParent, FALSE, 1,
						     &typeNum, &pSS, &list, &optional);
				    if (typeNum > 0)
				      {
					pAggregEl = pParent->ElParent;
					ready = TRUE;
					pElDelete = pElem;
					pElReplicate = pParent;
					createAfter = TRUE;
					replicate = FALSE;
					list = FALSE;
				      }
				  }
			      }
			  }
			else if (!list &&
				 !TypeHasException (ExcNoCreate,
						    pParent->ElTypeNumber,
						    pParent->ElStructSchema))
			  {
			    pAggregEl = pParent->ElParent;
			    ready = TRUE;
			    pElDelete = pElem;
			    pElReplicate = pParent;
			    replicate = FALSE;
			    if (pElem->ElPrevious != NULL &&
				pElem->ElNext == NUL)
			      createAfter = TRUE;
			    else if (pElem->ElNext != NULL &&
				     pElem->ElPrevious == NULL)
			      createAfter = FALSE;
			    else
			      {
				list = TRUE;
				pAggregEl = NULL;
			      }
			  }
		      }
		    if (pAggregEl == NULL)
		      {
			pParent = pParent->ElParent;
			if (pParent != NULL)
			  pListEl = AncestorList (pParent);
		      }
		    /* Specific treatment for xml */
		    if (pListEl == NULL && pAggregEl == NULL)
		      {
			pParent = pElem->ElParent;		
			pListEl = ParentAny (pElem);
		      }
		  }
		if (list && pListEl != NULL)
		  {
		    if (pElem->ElPrevious != NULL &&
			NoSignificantSibling (pElem, FALSE))
		      {
			if (!TypeHasException (ExcNoCreate,
					       pParent->ElTypeNumber,
					       pParent->ElStructSchema))
			  {
			    /* detruire pElem et creer un frere suivant
			       a pParent */
			    ready = TRUE;
			    pElDelete = pElem;
			    createAfter = TRUE;
			    pElReplicate = pParent;
			    while (pElReplicate->ElParent != pListEl)
			      pElReplicate = pElReplicate->ElParent;
			  }
		      }
		    else if (pElem->ElNext != NULL &&
			     NoSignificantSibling (pElem, TRUE) &&
			     !TypeHasException (ExcNoCreate,
						pParent->ElTypeNumber,
						pParent->ElStructSchema))
		      {
			/* detruire pElem et creer un frere precedent
			   a pParent */
			ready = TRUE;
			pElDelete = pElem;
			createAfter = FALSE;
			pElReplicate = pParent;
			while (pElReplicate->ElParent != pListEl)
			  pElReplicate = pElReplicate->ElParent;
		      }
		    else if (!TypeHasException (ExcNoCreate,
						pParent->ElTypeNumber,
						pParent->ElStructSchema))
		      /* try to split element pParent before element pElem */
		      {
			/* store the editing operation in the history */
			if (!histSeq)
			  {
			    OpenHistorySequence (pDoc, firstSel, lastSel, NULL,
						 firstChar, lastChar);
			    histSeq = TRUE;
			  }
			AddEditOpInHistory (pParent, pDoc, TRUE, TRUE);
			
			if (BreakElement (pParent, pElem, 0, FALSE, FALSE))
			  /* element pParent has been split */
			  {
			    /* record the element that has been
			       created by BreakElement: it has to be
			       deleted when undoing the command */
			    AddEditOpInHistory (pParent->ElNext, pDoc, FALSE,
						TRUE);
			    ready = TRUE;
			    pElDelete = pElem;
			    createAfter = TRUE;
			    pElReplicate = pParent;
			  }
			else
			  /* cannot split element */
			  {
			    /* remove operation from history */
			    CancelLastEditFromHistory (pDoc);
			    pListEl = NULL;
			  }
		      }
		  }
		if (list && pListEl == NULL)
		  {
		    pListEl = AncestorList (pElem);
		    if (pListEl != NULL)
		      if (!TypeHasException (ExcNoCreate,
					     pElem->ElTypeNumber,
					     pElem->ElStructSchema))
			{
			  ready = TRUE;
			  pElDelete = NULL;
			  createAfter = TRUE;
			  pElReplicate = pElem;
			}
		  }
	      }
	  if (ready && list)
	    {
	      replicate = FALSE;
	      ReturnCreateNewElem (pListEl, pElReplicate,
				   (ThotBool)!createAfter, pDoc,
				   &typeNum, &pSS);
	    }
	  else
	    pListEl = NULL;
	}
	
      if (!ready && !empty)
	{
	  /* La selection commence-t-elle en tete ou en queue d'element? */
	  selBegin = FALSE;
	  selEnd = FALSE;
	  if (firstSel == lastSel)
	    /* only one element selected */
	    {
	      if (firstSel->ElVolume > 0 && EmptyOrConstants (firstSel))
		/* the element includes only constants */
		{
		  selBegin = TRUE;
		  selEnd = TRUE;
		}
	      else if (firstSel->ElTerminal)
		{
		  if (firstSel->ElLeafType == LtText)
		    {
		      if (firstSel->ElPrevious == NULL && firstChar <= 1)
			/* no previous and at the beginning */
			selBegin = TRUE;
		      if (firstSel->ElNext == NULL &&
			  firstChar > firstSel->ElTextLength)
			/* no next and at the end */
			selEnd = TRUE;
		    }
		  else if (firstSel->ElLeafType == LtPicture)
		    {
		      if (firstSel->ElPrevious == NULL && firstChar == 0)
			/* no previous and a selection at the left border */
			selBegin = TRUE;
		      if (firstSel->ElNext == NULL && firstChar > 0)
			/* no next and a selection at the right border */
			selEnd = TRUE;
		    }
		  else
		    {
		      if ((firstSel->ElLeafType == LtGraphics ||
			   firstSel->ElLeafType == LtPolyLine ||
			   firstSel->ElLeafType == LtPath) &&
			  firstChar == 0 &&
			  firstSel->ElPrevious == NULL &&
			  firstSel->ElNext == NULL &&
			  firstSel->ElParent)
			/* select the enclosing element */
			firstSel = lastSel = firstSel->ElParent;
		      selBegin = TRUE;
		      selEnd = TRUE;
		    }
		}
	      else if (TypeHasException (ExcIsDraw, firstSel->ElTypeNumber,
					 firstSel->ElStructSchema))
		{
		  selBegin = TRUE;
		  selEnd = TRUE;
		}
	    }

	  /* Si la selection ne commence ni en tete ni en queue, on */
	  /* essaie de couper un paragraphe en deux */
	  if (!selBegin && !selEnd &&
	      CanSplitElement (firstSel, firstChar, TRUE, &pAncest, &pE,
			       &pElReplicate))
	    {
	      /* register the operation in history */
	      if (!histSeq)
		{
		  OpenHistorySequence (pDoc, firstSel, lastSel, NULL,
				       firstChar, lastChar);
		  histSeq = TRUE;
		}
	      AddEditOpInHistory (pElReplicate, pDoc, TRUE, TRUE);
	      
	      if (BreakElement (NULL, firstSel, firstChar, TRUE, TRUE))
		{
		  /* record the element that has been created by
		     BreakElement: it has to be deleted when undoing the
		     command */
		  AddEditOpInHistory (pElReplicate->ElNext, pDoc, FALSE, TRUE);
		  CloseHistorySequence (pDoc);
		  if (!lock)
		    /* unlock table formatting */
		    (*ThotLocalActions[T_unlock]) ();
		  return;
		}
	      else
		/* remove operation from history */
		CancelLastEditFromHistory (pDoc);
	    }

	  /* on cherche l'element CsList ascendant qui permet de creer un */
	  /* element voisin */
	  if (lastSel->ElTerminal && lastSel->ElLeafType == LtPageColBreak)
	    /* on ne duplique pas les sauts de pages */
	    pListEl = NULL;
	  else
	    {
	      pListEl = AncestorList (lastSel);
	      /* si c'est la fin d'une liste de Textes on remonte */
	      if (pListEl != NULL)
		{
		  if (lastSel->ElTerminal &&
		      /*(lastSel->ElLeafType == LtText ||
			lastSel->ElLeafType == LtPicture) &&*/
		      pListEl == lastSel->ElParent &&
		      (lastSel->ElNext == NULL || selBegin) &&
		      !TypeHasException (ExcReturnCreateWithin,
					 pListEl->ElTypeNumber,
					 pListEl->ElStructSchema))
		    pListEl = AncestorList (pListEl);
		}
	      else
		{
		  /* There is no List ancestor, search an Any parent */
		  if (lastSel->ElTerminal)
		    pListEl = ParentAny (lastSel->ElParent);
		  else
		    pListEl = ParentAny (lastSel);
		  if (pListEl != NULL)
		    {
		      if (lastSel->ElTerminal &&
			  pListEl == lastSel->ElParent &&
			  (lastSel->ElNext == NULL || selBegin) &&
			  !TypeHasException (ExcReturnCreateWithin,
					     pListEl->ElTypeNumber,
					     pListEl->ElStructSchema))
			pListEl = ParentAny (pListEl);
		    }
		}
	    }

	  /* verifie si les elements a doubler portent l'exception NoCreate */
	  if (pListEl != NULL)
	    {
	      pE = lastSel;
	      pElReplicate = NULL;
	      do
		{
		  if (TypeHasException (ExcNoCreate, pE->ElTypeNumber,
					pE->ElStructSchema))
		    /* abort */
		    pListEl = NULL;
		  else
		    {
		      pElReplicate = pE;
		      pE = pE->ElParent;
		    }
		}
	      while (pE != pListEl && pListEl != NULL && pE != NULL);

	      /* a priori, on creera le meme type d'element */
	      if (pElReplicate)
		{
		  if (TypeHasException (ExcNoReplicate, pElReplicate->ElTypeNumber,
					pElReplicate->ElStructSchema))
		    selEnd = TRUE;
		  else
		    typeNum = pElReplicate->ElTypeNumber;
		  pSS = pElReplicate->ElStructSchema;
		}
	    }

	  if (pListEl != NULL)
	    {
	      /* verifie si la selection est en fin ou debut de paragraphe */
	      if (selEnd)
		/* verifie s'il faut creer le meme type d'element ou un type */
		/* different */
		{
		  replicate = FALSE;
		  createAfter = TRUE;
		  ReturnCreateNewElem (pListEl, pElReplicate, FALSE, pDoc,
				       &typeNum, &pSS);
		}
	      else if (selBegin)
		{
		  replicate = FALSE;
		  createAfter = FALSE;
		  ReturnCreateNewElem (pListEl, pElReplicate, TRUE, pDoc,
				       &typeNum, &pSS);
		}
	    }
	}
      /* verifie que la liste ne depasse pas deja la longueur maximum */
      if (pListEl != NULL)
	if (!CanChangeNumberOfElem (pListEl, 1))
	  pListEl = NULL;
      if (pListEl != NULL || pAggregEl != NULL)
	{
	  if (pListEl == NULL)
	    pListEl = pAggregEl;
	  /* demande a l'application si on peut creer ce type d'element */
	  notifyEl.event = TteElemNew;
	  notifyEl.document = (Document) IdentDocument (pDoc);
	  notifyEl.element = (Element) (pElReplicate->ElParent);
	  notifyEl.info = 0; /* not sent by undo */
	  notifyEl.elementType.ElTypeNum = typeNum;
	  notifyEl.elementType.ElSSchema = (SSchema) pSS;
	  pSibling = pElReplicate;
	  NSiblings = 0;
	  while (pSibling->ElPrevious != NULL)
	    {
	      NSiblings++;
	      pSibling = pSibling->ElPrevious;
	    }
	  if (createAfter)
	    NSiblings++;
	  notifyEl.position = NSiblings;
	  if (CallEventType ((NotifyEvent *) (&notifyEl), TRUE))
	    /* l'application refuse */
	    pListEl = NULL;
	}
      if (pListEl != NULL)
	{
	  ok = !ElementIsReadOnly (pListEl);
	  if (ok && pElDelete != NULL)
	    /* on va detruire un sous arbre vide. */
	    /* envoie l'evenement ElemDelete.Pre */
	    ok = !SendEventSubTree (TteElemDelete, pDoc, pElDelete,
				    TTE_STANDARD_DELETE_LAST_ITEM, 0,
				    FALSE, FALSE);
	  if (ok)
	    {
	      /* annule d'abord la selection */
	      TtaClearViewSelections ();
	      if (pElDelete != NULL)
		/* detruire le sous-arbre qu'on remplace */
		{
		  deleteEmpty = TRUE;
		  if (!histSeq)
		    {
		      OpenHistorySequence (pDoc, firstSel, lastSel, NULL,
					   firstChar, lastChar);
		      histSeq = TRUE;
		    }
		  AddEditOpInHistory (pElDelete, pDoc, TRUE, FALSE);
		  
		  pPrevious = PreviousNotPage (pElDelete);
		  pNext = NextNotPage (pElDelete);
		  DestroyAbsBoxes (pElDelete, pDoc, TRUE);
		  AbstractImageUpdated (pDoc);
		  /* prepare l'evenement ElemDelete.Post */
		  notifyEl.event = TteElemDelete;
		  notifyEl.document = (Document) IdentDocument (pDoc);
		  notifyEl.element = (Element) (pElDelete->ElParent);
		  notifyEl.info = 0; /* not sent by undo */
		  notifyEl.elementType.ElTypeNum = pElDelete->ElTypeNumber;
		  notifyEl.elementType.ElSSchema =
		                        (SSchema) (pElDelete->ElStructSchema);
		  pSibling = pElDelete;
		  NSiblings = 0;
		  while (pSibling->ElPrevious != NULL)
		    {
		      NSiblings++;
		      pSibling = pSibling->ElPrevious;
		    }
		  notifyEl.position = NSiblings;
		  pClose = NextElement (pElDelete);

		  /* retire l'element de l'arbre abstrait */
		  RemoveElement (pElDelete);
		  dispMode = TtaGetDisplayMode (doc);
		  if (dispMode == DisplayImmediately)
		    TtaSetDisplayMode (doc, DeferredDisplay);
		  UpdateNumbers (pClose, pElDelete, pDoc, TRUE);
		  if (dispMode == DisplayImmediately)
		    TtaSetDisplayMode (doc, dispMode);

		  RedisplayCopies (pElDelete, pDoc, TRUE);
		  DeleteElement (&pElDelete, pDoc);
		  /* envoie l'evenement ElemDelete.Post a l'application */
		  CallEventType ((NotifyEvent *) (&notifyEl), FALSE);
		  if (pNext != NULL)
		    if (PreviousNotPage (pNext) == NULL)
		      /* l'element qui suit l'element detruit devient premier*/
		      ChangeFirstLast (pNext, pDoc, TRUE, FALSE);
		  if (pPrevious != NULL)
		    if (NextNotPage (pPrevious) == NULL)
		      /* l'element qui precede l'element detruit devient
			 dernier */
		      ChangeFirstLast (pPrevious, pDoc, FALSE, FALSE);
		}
	      if (!replicate)
		{
		  pE = pElReplicate;
		  pNew = NewSubtree (typeNum, pSS, pDoc, TRUE, TRUE, TRUE,
				     TRUE);
		}
	      else
		{
		  /* Reconstruction d'une structure parallele */
		  pNew = NewSubtree (lastSel->ElTypeNumber,
				     lastSel->ElStructSchema, pDoc,
				     TRUE, TRUE, TRUE, TRUE);
		  pE = lastSel;
		  while (pE->ElParent != pListEl)
		    {
		      pE = pE->ElParent;
		      pAncest = ReplicateElement (pE, pDoc);
		      InsertFirstChild (pAncest, pNew);
		      pNew = pAncest;
		    }
		}
	      /* Insertion du nouvel element */
	      if (createAfter)
		{
		  pClose = pE->ElNext;
		  FwdSkipPageBreak (&pClose);
		  InsertElementAfter (pE, pNew);
		  if (pClose == NULL)
		    /* l'element pE n'est plus le dernier fils de son pere */
		    ChangeFirstLast (pE, pDoc, FALSE, TRUE);
		}
	      else
		{
		  pClose = pE->ElPrevious;
		  InsertElementBefore (pE, pNew);
		  if (pClose == NULL)
		    /* l'element pE n'est plus le premier fils de son pere */
		    ChangeFirstLast (pE, pDoc, TRUE, TRUE);
		}
	      if (!histSeq)
		{
		  OpenHistorySequence (pDoc, firstSel, lastSel, NULL,
				       firstChar, lastChar);
		  histSeq = TRUE;
		}
	      AddEditOpInHistory (pNew, pDoc, FALSE, TRUE);
	      /* traite les exclusions des elements crees */
	      RemoveExcludedElem (&pNew, pDoc);
	      /* traite les attributs requis des elements crees */
	      AttachMandatoryAttributes (pNew, pDoc);
	      if (pDoc->DocSSchema != NULL)
		/* le document n'a pas ete ferme' entre temps */
		{
		  /* traitement des exceptions */
		  CreationExceptions (pNew, pDoc);

		  /* Avoid too many redisplay when updating a table */
		  dispMode = TtaGetDisplayMode (doc);
		  if (dispMode == DisplayImmediately)
		    TtaSetDisplayMode (doc, DeferredDisplay);
		  /* envoie un evenement ElemNew.Post a l'application */
		  NotifySubTree (TteElemNew, pDoc, pNew, 0, 0, FALSE, FALSE);
		  /* Mise a jour des images abstraites */
		  CreateAllAbsBoxesOfEl (pNew, pDoc);
		  /* generate abstract boxes */
		  AbstractImageUpdated (pDoc);
		  /* update boxes */
		  RedisplayDocViews (pDoc);
		  /* si on est dans un element copie' par inclusion, */
		  /* on met a jour les copies de cet element. */
		  RedisplayCopies (pNew, pDoc, TRUE);
		  UpdateNumbers (NextElement (pNew), pNew, pDoc, TRUE);

		  /* Set the document modified */
		  SetDocumentModified (pDoc, TRUE, 30);
		  if (!lock)
		    {
		      /* unlock table formatting */
		      (*ThotLocalActions[T_unlock]) ();
		      lock = TRUE; /* unlock is done */
		    }

		  if (dispMode == DisplayImmediately)
		    TtaSetDisplayMode (doc, dispMode);
		  /* restore a selection */
		  SelectElementWithEvent (pDoc, FirstLeaf (pNew), TRUE, TRUE);
		}
	    }
	}
      if (!lock)
	/* handle the remaining unlock of table formatting */
	(*ThotLocalActions[T_unlock]) ();
      if (histSeq)
	CloseHistorySequence (pDoc);
    }
}

/*----------------------------------------------------------------------
   AscentChildOfParagraph  return the ancestor of element pEl      
   (or pEl itself) whose parent has exception              
   ExcParagraphBreak.                                      
  ----------------------------------------------------------------------*/
static PtrElement  AscentChildOfParagraph (PtrElement pEl)
{
  PtrElement          pAncest, pParent;
  ThotBool            stop;

  stop = FALSE;
  pAncest = pEl;
  do
    {
      pParent = pAncest->ElParent;
      if (pParent == NULL)
	{
	  stop = TRUE;
	  pAncest = NULL;
	}
      else if (TypeHasException (ExcParagraphBreak, pParent->ElTypeNumber,
				 pParent->ElStructSchema))
	stop = TRUE;
      else
	pAncest = pParent;
    }
  while (!stop);
  return pAncest;
}

/*----------------------------------------------------------------------
   NextSiblingNotPage retourne l'element frere suivant pEl qui     
   n'est pas un saut de page.                              
  ----------------------------------------------------------------------*/
static PtrElement NextSiblingNotPage (PtrElement pEl)
{
   PtrElement          pNext;
   ThotBool            stop;

   pNext = pEl->ElNext;
   stop = FALSE;
   do
      if (pNext == NULL)
	 stop = TRUE;
      else if (pNext->ElTerminal && pNext->ElLeafType == LtPageColBreak)
	 pNext = pNext->ElNext;
      else
	 stop = TRUE;
   while (!stop);
   return pNext;
}

/*----------------------------------------------------------------------
   DeleteNextChar  If before, the current selection is at the      
   beginning of element pEl and the user has hit the       
   BackSpace key. Merging with previous element.
   If not before, the current selection is at the end of   
   element pEl and the user has hit the Delete key.  Merging with
   next element.
  ----------------------------------------------------------------------*/
void DeleteNextChar (int frame, PtrElement pEl, ThotBool before)
{
   PtrElement          pSibling, pNext, pPrev, pE, pElem, pParent, pS;
   PtrElement          pSel, pSuccessor, pLeaf;
   PtrElement         *list;
   PtrDocument         pDoc;
   NotifyElement       notifyEl;
   NotifyOnValue       notifyVal;
   Document            doc;
   int                 nSiblings;
   int                 nbEl, j, firstChar, lastChar;
   ThotBool            stop, ok, isRow;

   if (pEl == NULL)
      return;
   /* pSel: element to be selected at the end */
   if (before)
      pSel = NULL;
   else
      pSel = pEl;
   pElem = NULL;
   pDoc = DocumentOfElement (pEl);

   /* look for the first ancestor with a previous (if before) or next sibling:
      pParent */
   pParent = pEl->ElParent;
   if (pParent &&
       pParent->ElStructSchema->SsRule->SrElem[pParent->ElTypeNumber - 1]->SrConstruct == CsConstant)
     /* delete the constant itself */
     pEl = pParent;
   else
     pParent = pEl;

   do
     {
       if (before)
	 pSibling = PreviousNotPage (pParent);
       else
	 pSibling = NextSiblingNotPage (pParent);
       if (pSibling == NULL)
	 {
	   pElem = pParent;
	   pParent = pElem->ElParent;
	   if (pParent &&
	       TypeHasException (ExcIsCell,
				pParent->ElTypeNumber,
				 pParent->ElStructSchema))
	   /* DeleteNextChar cannot cross a cell limit */
	   pParent = NULL;
	 }
     }
   while (pParent != NULL && pSibling == NULL);

   if (pParent == NULL || pSibling == NULL)
     return;

   /* determine the current selection */
   firstChar = 0;  lastChar= 0;
   if (pEl->ElTerminal && pEl->ElLeafType == LtText)
     {
       if (before)
	 firstChar = 1;
       else
	 firstChar = pEl->ElVolume + 1;
       lastChar = firstChar - 1;
     }

   doc = IdentDocument (pDoc);
   if (before &&
       ((pSibling->ElVolume == 0 && pParent->ElVolume > 0) ||
	pSibling->ElStructSchema->SsRule->SrElem[pSibling->ElTypeNumber - 1]->SrConstruct == CsConstant))
     /* BackSpace at the beginning of a non empty element (pParent) whose
        previous sibling (pSibling) is empty.  Delete the empty sibling */
     {
       OpenHistorySequence (pDoc, pEl, pEl, NULL, firstChar, lastChar);
       /* record the element to be deleted in the history */
       AddEditOpInHistory (pSibling, pDoc, TRUE, FALSE);
       TtaDeleteTree ((Element)pSibling, doc);
       CloseHistorySequence (pDoc);
       return;
     }

   /* if elements pSibling and pParent have a common ancestor with
      exception ParagraphBreak, don't merge them. Just delete the
      next or previous character */
   pE = CommonAncestor (pSibling, pElem);
   while (pE)
     {
       if (TypeHasException (ExcParagraphBreak, pE->ElTypeNumber,
			     pE->ElStructSchema))
	  {
          pSibling = NULL;
	  pE = NULL;
	  }
       else
	  pE = pE->ElParent;
     }

   if (pSibling != NULL && pParent != pEl && pElem != NULL)
     {
       if (pSibling->ElTerminal)
	 /* don't merge a structured element with a text string */
	 pSibling = NULL;
       else
	 /* check whether the SSchema allows elements to be merged, i.e. can
	    children of element pSibling become siblings of element pElem? */
	 {
	   pSibling = pSibling->ElFirstChild;
	   if (pSibling != NULL)
	     {
	       if (before)
		 while (pSibling->ElNext != NULL)
		   pSibling = pSibling->ElNext;
	       else
		 {
		   pE = pElem;
		   pElem = pSibling;
		   pSibling = pE;
		 }
	       if (!AllowedSibling (pSibling, pDoc, pElem->ElTypeNumber,
				    pElem->ElStructSchema, FALSE, FALSE,FALSE))
		 /* not allowed */
		 pSibling = NULL;
	     }
	 }
     }

   if (pSibling == NULL || pParent == pEl)
     /* don't merge elements. Just delete the previous or next character */
     {
       stop = FALSE;
       pElem = pEl;
       do
	 {
	   if (before)
	     pSibling = PreviousNotPage (pElem);
	   else
	     pSibling = NextSiblingNotPage (pElem);
	   if (pSibling != NULL)
	     stop = TRUE;
	   else
	     {
	       pParent = pElem->ElParent;
	       if (pParent == NULL)
		 {
		   stop = TRUE;
		   pElem = NULL;
		 }
	       else if (TypeHasException (ExcParagraphBreak,
					  pParent->ElTypeNumber,
					  pParent->ElStructSchema))
		 stop = TRUE;
	       else
		 pElem = pParent;
	     }
	 }
       while (!stop);

       if (pElem == NULL)
	 return;
       if (pSibling == NULL)
	 if (before)
	   {
	     pLeaf = PreviousLeaf (pElem);
	     pSibling = AscentChildOfParagraph (pLeaf);
	   }
	 else
	   {
	     pSibling = pElem;
	     pLeaf = NextLeaf (pSibling);
	     pElem = AscentChildOfParagraph (pLeaf);
	     if (pElem == NULL && pLeaf->ElVolume == 0)
	       {
		 pSibling = pLeaf;
		 pElem = pParent;
	       }
	     else if (ElemIsAnAncestor (pElem, pSibling))
	       pElem = pLeaf;
	   }
       else
	 {
	   if (!pSibling->ElTerminal)
	     {
	       if (before)
		 pSibling = PreviousLeaf (pElem);
	       else
		 pSibling = NextLeaf (pElem);
	       if (!pSibling)
		 return;
	     }
	   if (!pSibling->ElTerminal ||
	       (pSibling->ElAccess == AccessReadOnly &&
		pSibling->ElVolume == 1))
	     {
	       if (pSibling->ElFirstChild == NULL)
		 /* pSibling is empty. Delete it */
		 {
		   /* record the element to be deleted in the history */
		   OpenHistorySequence (pDoc, pEl, pEl, NULL, firstChar, lastChar);
		   AddEditOpInHistory (pSibling, pDoc, TRUE,FALSE);
		   TtaDeleteTree ((Element)pSibling, doc);
		   CloseHistorySequence (pDoc);
		 }
	     }
	   else
	     {
	       if (pSibling->ElLeafType == LtPairedElem)
		 /* skip the paired element */
		 DeleteNextChar (frame, pSibling, before);
	       else if (pSibling->ElTypeNumber == CharString + 1)
		 if (before)
		   {
		     /* set selection after the last character of the string */
		     SelectPositionWithEvent (pDoc, pSibling,
					      pSibling->ElTextLength + 1);
		     /* simulate a backspace */
		     InsertChar (frame, '\177', -1);
		   }
		 else
		   {
		     /* set selection before the first character of the
			string */
		     if (pSibling->ElVolume == 0)
		       SelectElement (pDoc, pSibling, FALSE, FALSE);
		     else
		       SelectString (pDoc, pSibling, 1, 0);
		     /* simulate a delete */
		     TtcDeleteSelection (IdentDocument (pDoc), 0);
		   }
	       else if (strcmp (pSibling->ElStructSchema->SsName, "SVG"))
		 /* don't delete a graphic element when the user enters
		    Backspace at the beginning of a svg:text element */
		 {
		   /* set selection before the first character of the string */
		   SelectElement (pDoc, pSibling, FALSE, FALSE);
		   /* and delete the selected element */
		   CutCommand (FALSE);
		 }
	     }
	   /* done */
	   pElem = NULL;
	   pSibling = NULL;
	 }
     }

   if (pElem != NULL && pSibling != NULL &&
       AllowedSibling (pSibling, pDoc, pElem->ElTypeNumber,
		       pElem->ElStructSchema, FALSE, FALSE, FALSE))
     {
       /* switch selection off */
       TtaClearViewSelections ();
       pE = pElem;
       nbEl = 0;
       while (pE != NULL)
	 {
	   nbEl++;
	   pE = pE->ElNext;
	 }
       list = (PtrElement *) TtaGetMemory (nbEl * sizeof (PtrElement));
       pE = pElem;
       nbEl = 0;
       while (pE != NULL)
	 {
	   /* enregistre les elements preexistants */
	   list[nbEl++] = pE;
	   pE = pE->ElNext;
	 }
       pParent = pElem->ElParent;
       isRow = TypeHasException (ExcIsRow, pParent->ElTypeNumber,
				 pParent->ElStructSchema);

       /* start history sequence */
       OpenHistorySequence (pDoc, pEl, pEl, NULL, firstChar, lastChar);

       j = 0;
       while (pElem != NULL)
	 {
	   j++;
	   if (j < nbEl)
	     pNext = list[j];
	   else
	     pNext = NULL;
	   /* Send event ElemDelete.Pre to application for existing elements */
	   if (isRow || !SendEventSubTree (TteElemDelete, pDoc, pElem,
					   TTE_STANDARD_DELETE_LAST_ITEM,
					   0, FALSE, FALSE))
	     {
	       /* detruit les paves de l'element qui va etre deplace' */
	       DestroyAbsBoxes (pElem, pDoc, TRUE);
	       AbstractImageUpdated (pDoc);
	       if (!isRow)
		 {
		   /* prepare the event ElemDelete.Post */
		   notifyEl.event = TteElemDelete;
		   notifyEl.document = doc;
		   notifyEl.element = (Element) (pElem->ElParent);
		   notifyEl.info = 0; /* not sent by undo */
		   notifyEl.elementType.ElTypeNum = pElem->ElTypeNumber;
		   notifyEl.elementType.ElSSchema =
		                          (SSchema) (pElem->ElStructSchema);
		   nSiblings = 0;
		   pS = pElem;
		   while (pS->ElPrevious != NULL)
		     {
		       nSiblings++;
		       pS = pS->ElPrevious;
		     }
		   notifyEl.position = nSiblings;
		 }
	       pSuccessor = NextElement (pElem);
	       /* record the element to be deleted in the history */
	       AddEditOpInHistory (pElem, pDoc, TRUE, FALSE);
	       /* retire l'element de l'arbre abstrait */
	       RemoveElement (pElem);
	       UpdateNumbers (pSuccessor, pElem, pDoc, TRUE);
	       RedisplayCopies (pElem, pDoc, TRUE);
	       if (!isRow)
		 {
		   /* send the event ElemDelete.Post to the application */
		   CallEventType ((NotifyEvent *) (&notifyEl), FALSE);
		   /* send the event ElemPaste.Pre to the application */
		   notifyVal.event = TteElemPaste;
		   notifyVal.document = doc;
		   notifyVal.element = (Element) (pSibling->ElParent);
		   notifyVal.target = (Element) pElem;
		   nSiblings = 1;
		   pS = pSibling;
		   while (pS->ElPrevious != NULL)
		     {
		       nSiblings++;
		       pS = pS->ElPrevious;
		     }
		   notifyVal.value = nSiblings;
		   ok = CallEventType ((NotifyEvent *) (&notifyVal), TRUE);
		 }
	       else
		 ok = FALSE;
	       if (ok || (pElem->ElVolume == 0))
		 /* the application refuses to paste this element or this
		    element is empty, free it */
		 DeleteElement (&pElem, pDoc);
	       else if (pSibling && pSibling->ElParent != pElem)
		 {
		   /* l'application accepte */
		   /* insere l'element a sa nouvelle position */
		   InsertElementAfter (pSibling, pElem);
		   /* record the inserted element in the history */
		   AddEditOpInHistory (pElem, pDoc, FALSE, TRUE);
		   if (!isRow)
		     NotifySubTree (TteElemPaste, pDoc, pElem, 0, 0, FALSE, FALSE);
		 }
	     }
	   if (pElem != NULL)
	     pSibling = pElem;
	   if (pSel == NULL)
	     pSel = pElem;
	   /* passe a l'element suivant */
	   if (pNext == NULL)
	     pElem = NULL;
	   else if (AllowedSibling (pSibling, pDoc, pNext->ElTypeNumber,
				    pNext->ElStructSchema, FALSE, FALSE,FALSE))
	     pElem = pNext;
	   else
	     pElem = NULL;
	 }
       TtaFreeMemory (list);

       /* detruit les elements qui ont ete vide's */
       pPrev = NULL;
       while (pParent != NULL && pParent->ElFirstChild == NULL)
	 {
	   pE = pParent;
	   pParent = pE->ElParent;
	   /* envoie l'evenement ElemDelete.Pre et demande a */
	   /* l'application si elle est d'accord pour detruire l'elem. */
	   if (!SendEventSubTree (TteElemDelete, pDoc, pE,
				  TTE_STANDARD_DELETE_LAST_ITEM, 0,
				  FALSE, FALSE))
	     {
	     /* cherche l'element qui precede l'element a detruire */
	     pPrev = PreviousNotPage (pE);
	     DestroyAbsBoxes (pE, pDoc, TRUE);
	     AbstractImageUpdated (pDoc);
	     pNext = NextElement (pE);
     	     /* prepare l'evenement ElemDelete.Post */
       	     notifyEl.event = TteElemDelete;
       	     notifyEl.document = doc;
       	     notifyEl.element = (Element) pParent;
	     notifyEl.info = 0; /* not sent by undo */
       	     notifyEl.elementType.ElTypeNum = pE->ElTypeNumber;
       	     notifyEl.elementType.ElSSchema = (SSchema) (pE->ElStructSchema);
       	     nSiblings = 0;
       	     pS = pE;
       	     while (pS->ElPrevious != NULL)
       	       {
       		  nSiblings++;
       		  pS = pS->ElPrevious;
       	       }
       	     notifyEl.position = nSiblings;
       	     /* record the element the element that will be deleted */
       	     AddEditOpInHistory (pE, pDoc, TRUE, FALSE);
       	     /* retire l'element courant de l'arbre */
       	     RemoveElement (pE);
       	     UpdateNumbers (pNext, pE, pDoc, TRUE);
       	     DeleteElement (&pE, pDoc);
       	     /* envoie l'evenement ElemDelete.Post */
       	     CallEventType ((NotifyEvent *) (&notifyEl), FALSE);
             }
         }
       /* reaffiche ce qui doit l'etre */
       pE = pSibling->ElParent;
       CreateAllAbsBoxesOfEl (pE, pDoc);
       if (pPrev != NULL)
	  /* verifie si l'element precedent devient dernier parmi */
	  /* ses freres */
	  ProcessFirstLast (pPrev, NULL, pDoc);
       AbstractImageUpdated (pDoc);
       RedisplayDocViews (pDoc);
       /* si on est dans un element copie' par inclusion, on met a jour
          les copies de cet element. */
       RedisplayCopies (pE, pDoc, TRUE);
       /* indique que le document est modifie' */
       SetDocumentModified (pDoc, TRUE, 30);

       /* selectionne */
       if (!pSel)
          {
          pSel = pSibling;
          before = !before;
          }
       if (pSel && pSel->ElStructSchema)
          {
          if (pSel->ElVolume > 0)
             /* the first element moved is not empty. Select its first
	        or last character, depending on "before" */
	     {
	     pSel = FirstLeaf (pSel);
	     if (!pSel->ElTerminal)
	        SelectElement (pDoc, pSel, TRUE, TRUE);
	     else if (pSel->ElLeafType == LtText)
	        if (before)
	           MoveCaret (pDoc, pSel, 1);
	        else
	           MoveCaret (pDoc, pSel, pSel->ElTextLength + 1);
	     else if (pSel->ElLeafType != LtPairedElem)
	        SelectElement (pDoc, pSel, TRUE, TRUE);
	     else if (pSel->ElPrevious != NULL)
	        if (pSel->ElPrevious->ElTerminal &&
	            pSel->ElPrevious->ElLeafType == LtText)
	           MoveCaret (pDoc, pSel->ElPrevious,
	                      pSel->ElPrevious->ElTextLength + 1);
	        else
		   SelectElement (pDoc, pSel->ElPrevious, TRUE, TRUE);
	     else if (pSel->ElNext != NULL)
		if (pSel->ElNext->ElTerminal &&
		    pSel->ElNext->ElLeafType == LtText)
		   MoveCaret (pDoc, pSel->ElNext, 1);
		else
		   SelectElement (pDoc, pSel->ElNext, TRUE, TRUE);
	     else
		SelectElement (pDoc, pSel->ElParent, TRUE, TRUE);
	     }
	  else
	     /* the first element moved is empty. Select the closest
	        character */
	     {
	     if (pSel->ElPrevious != NULL)
	        {
	        pSel = LastLeaf (pSel->ElPrevious);
	        if (pSel->ElTerminal && pSel->ElLeafType == LtText)
	           MoveCaret (pDoc, pSel, pSel->ElTextLength + 1);
	        else
	           SelectElement (pDoc, pSel, TRUE, TRUE);
	        }
	     else if (pSel->ElNext != NULL)
	        {
	        pSel = FirstLeaf (pSel->ElNext);
	        if (pSel->ElTerminal && pSel->ElLeafType == LtText)
	           MoveCaret (pDoc, pSel, 1);
	        else
	           SelectElement (pDoc, pSel, TRUE, TRUE);
	        }
	     else
	        SelectElement (pDoc, pSel, TRUE, TRUE);
	     }
	  }
       /* end of command: close history sequence */
       CloseHistorySequence (pDoc);
     }
}

/*----------------------------------------------------------------------
   NoStructureLoadResources
   connects unstructured editing and selection functions.
  ----------------------------------------------------------------------*/
void NoStructSelectLoadResources ()
{
   if (ThotLocalActions[T_selectsiblings] == NULL)
     {
	TteConnectAction (T_selectsiblings, (Proc) SelectSiblings);
	TteConnectAction (T_checksel, (Proc) CheckSelectedElement);
	TteConnectAction (T_resetsel, (Proc) ResetSelection);
	TteConnectAction (T_selstring, (Proc) SelectString);
	TteConnectAction (T_deletenextchar, (Proc) DeleteNextChar);
	TteConnectAction (T_cmdpaste, (Proc) PasteCommand);
	TteConnectAction (T_enter, (Proc) TtcCreateElement);
	MenuActionList[CMD_CreateElement].Call_Action = (Proc)TtcCreateElement;
	InitSelection ();
     }
}
