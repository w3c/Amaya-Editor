/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * Warning:
 * This module is part of the Thot library, which was originally
 * developed in French. That's why some comments are still in
 * French, but their translation is in progress and the full module
 * will be available in English in the next release.
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

#include "thot_sys.h"
#include "constmedia.h"
#include "constmenu.h"
#include "typemedia.h"
#include "language.h"
#include "modif.h"
#include "appaction.h"
#include "app.h"
#include "fileaccess.h"
#include "libmsg.h"
#include "message.h"
#include "appdialogue.h"
#include "dialog.h"
#include "tree.h"
#undef EXPORT
#define EXPORT extern
#include "creation_tv.h"
#include "modif_tv.h"
#include "select_tv.h"
#include "edit_tv.h"
#include "appdialogue_tv.h"

#include "appli_f.h"
#include "applicationapi_f.h"
#include "tree_f.h"
#include "attrpresent_f.h"
#include "attributes_f.h"
#include "search_f.h"
#include "textcommands_f.h"
#include "editcommands_f.h"
#include "contentapi_f.h"
#include "structcreation_f.h"
#include "createabsbox_f.h"
#include "views_f.h"
#include "callback_f.h"
#include "exceptions_f.h"
#include "absboxes_f.h"
#include "buildboxes_f.h"
#include "memory_f.h"
#include "structmodif_f.h"
#include "changeabsbox_f.h"
#include "changepresent_f.h"
#include "presrules_f.h"
#include "references_f.h"
#include "externalref_f.h"
#include "boxselection_f.h"
#include "structselect_f.h"
#include "selectmenu_f.h"
#include "fileaccess_f.h"
#include "structschema_f.h"
#include "content_f.h"
#include "searchref_f.h"
#include "unstructlocate_f.h"
#include "structcommands_f.h"

/*----------------------------------------------------------------------
   InsertPastedElement
   Insere l'element pNew devant ou derriere (selon before) l'element pEl du
   document pDoc.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         InsertPastedElement (PtrElement pEl, boolean before,
					 PtrElement pNew, PtrDocument pDoc)
#else
static void         InsertPastedElement (pEl, before, pNew, pDoc)
PtrElement          pEl;
boolean             before;
PtrElement          pNew;
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   PtrElement          pSibling;

   if (before)
     {
	/* teste d'abord si pEl est le premier fils de son pere, abstraction
	   faite des marques de page */
	pSibling = pEl->ElPrevious;
	BackSkipPageBreak (&pSibling);
	/* insere l'element dans l'arbre */
	InsertElementBefore (pEl, pNew);
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
	InsertElementAfter (pEl, pNew);
	if (pSibling == NULL)
	   /* l'element devant lequel on colle n'est plus premier */
	   ChangeFirstLast (pEl, pDoc, FALSE, TRUE);
     }
}


/*----------------------------------------------------------------------
   PasteAnElement  Paste element decribed by pSavedEl before       
   (if before is TRUE) or after (if before is FALSE)       
   element pEl in document pDoc.                           
  ----------------------------------------------------------------------*/
#ifdef __STDC__

static PtrElement   PasteAnElement (PtrElement pEl, PtrPasteElem pSavedEl,
				    boolean before, PtrDocument pDoc)
#else  /* __STDC__ */
static PtrElement   PasteAnElement (pEl, pSavedEl, before, pDoc)
PtrElement          pEl;
PtrPasteElem        pSavedEl;
boolean             before;
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   PtrElement          pElem, pChild, pPasted, pOrig, pParent, pSibling,
                       pAncest, pE, pElAttr;
   PtrAttribute        pInheritLang, pLangAttr;
   PtrPasteElem        pPasteD;
   NotifyOnValue       notifyVal;
   NotifyElement       notifyEl;
   int                 NSiblings, i, asc;
   boolean             stop, ok, possible;

   pPasted = NULL;
   pAncest = NULL;
   pOrig = pSavedEl->PeElement;
   /* futur pere de l'element colle' */
   pParent = pEl->ElParent;
   /* on calcule le nombre de freres qui precederont l'element */
   /* lorsqu'il sera mis dans l'arbre abstrait */
   NSiblings = 0;
   pSibling = pEl;
   while (pSibling->ElPrevious != NULL)
     {
	NSiblings++;
	pSibling = pSibling->ElPrevious;
     }
   if (!before)
      NSiblings++;
   pElem = pEl;
   /* verifie si l'element peut etre colle' au meme niveau que pEl */
   ok = AllowedSibling (pEl, pDoc, pOrig->ElTypeNumber,
			pOrig->ElStructSchema, before, TRUE, FALSE);
   if (!ok)
      /* l'element ne peut pas etre colle' au meme niveau */
      /* s'il faut coller en debut ou fin d'element, on essaie de remonter */
      /* d'un ou plusieurs niveaux */
     {
        pElem = pEl;
        while (!ok && pElem != NULL)
	   if ((before && pElem->ElPrevious == NULL) ||
	       (!before && pElem->ElNext == NULL))
	     {
	        pElem = pElem->ElParent;
	        if (pElem != NULL)
		   ok = AllowedSibling (pElem, pDoc, pOrig->ElTypeNumber,
				   pOrig->ElStructSchema, before, TRUE, FALSE);
	     }
	   else
	     pElem = NULL;
     }

   if (!ok)
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
		       pSavedEl->PeAscendSSchema[asc], before, TRUE, FALSE))
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
		     /* cet ascendant convient, on va creer un element de
		        ce type */
		    {
		       stop = TRUE;
		       /* demande a l'application si on peut creer ce type
		          d'element */
		       notifyEl.event = TteElemNew;
		       notifyEl.document = (Document) IdentDocument (pDoc);
		       notifyEl.element = (Element) (pParent);
		       notifyEl.elementType.ElTypeNum = pSavedEl->PeAscendTypeNum[asc];
		       notifyEl.elementType.ElSSchema = (SSchema) (pSavedEl->PeAscendSSchema[asc]);
		       notifyEl.position = NSiblings;
		       if (CallEventType ((NotifyEvent *) & notifyEl, TRUE))
			  /* l'application refuse */
			  stop = TRUE;
		       else
			 {
			    pAncest = NewSubtree (pSavedEl->PeAscendTypeNum[asc],
						  pSavedEl->PeAscendSSchema[asc], pDoc, pEl->ElAssocNum, FALSE,
						  TRUE, TRUE, TRUE);
			    if (pAncest != NULL)
			      {
				 /* on insere ce nouvel element dans l'arbre abstrait */
				 InsertPastedElement (pElem, before, pAncest, pDoc);
				 /* on envoie un evenement ElemNew.Post a l'application */
				 notifyEl.event = TteElemNew;
				 notifyEl.document = (Document) IdentDocument (pDoc);
				 notifyEl.element = (Element) pAncest;
				 notifyEl.elementType.ElTypeNum = pAncest->ElTypeNumber;
				 notifyEl.elementType.ElSSchema = (SSchema) (pAncest->ElStructSchema);
				 notifyEl.position = 0;
				 CallEventType ((NotifyEvent *) & notifyEl, FALSE);
				 ok = TRUE;
				 /* on cree les ascendants intermediaires */
				 pParent = pAncest;
				 while (asc > 0)
				   {
				      asc--;
				      /* demande a l'application si on peut creer ce type d'elem. */
				      notifyEl.event = TteElemNew;
				      notifyEl.document = (Document) IdentDocument (pDoc);
				      notifyEl.element = (Element) (pParent);
				      notifyEl.elementType.ElTypeNum = pSavedEl->PeAscendTypeNum[asc];
				      notifyEl.elementType.ElSSchema =
					 (SSchema) (pSavedEl->PeAscendSSchema[asc]);
				      notifyEl.position = NSiblings;
				      if (CallEventType ((NotifyEvent *) & notifyEl, TRUE))
					 /* l'application refuse */
					{
					   DeleteElement (&pAncest);
					   stop = TRUE;
					   ok = FALSE;
					   asc = 0;
					}
				      else
					 /* l'application accepte, on cree l'element */
					{
					   pE = NewSubtree (pSavedEl->PeAscendTypeNum[asc],
							    pSavedEl->PeAscendSSchema[asc], pDoc, pEl->ElAssocNum,
						   FALSE, TRUE, TRUE, TRUE);
					   /* on insere ce nouvel element dans l'arbre abstrait */
					   InsertFirstChild (pParent, pE);
					   /* on envoie un evenement ElemNew.Post a l'appli */
					   notifyEl.event = TteElemNew;
					   notifyEl.document = (Document) IdentDocument (pDoc);
					   notifyEl.element = (Element) pE;
					   notifyEl.elementType.ElTypeNum = pE->ElTypeNumber;
					   notifyEl.elementType.ElSSchema = (SSchema) (pE->ElStructSchema);
					   notifyEl.position = 0;
					   if (pE->ElTypeNumber == pE->ElStructSchema->SsRootElem)
					      /* root element in a different structure schema */
					      /* Put number of elements in the "position" field */
					      notifyEl.position = pE->ElStructSchema->SsNObjects;
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
	if (!CallEventType ((NotifyEvent *) (&notifyVal), TRUE))
	   /* l'application accepte */
	  {
	     /* Cree une copie de l'element a coller. */
	     /* Si l'element est reference', la copie devient l'element reference' */
	     /* Ne copie les attributs que s'ils sont definis dans les schemas de */
	     /* structure des elements englobants du document d'arrivee. */
	     pPasted = CopyTree (pOrig, DocOfSavedElements, pElem->ElAssocNum,
			pParent->ElStructSchema, pDoc, pParent, TRUE, TRUE);
	     if (pPasted != NULL)
	       {
		  /* insere la copie dans l'arbre */
		  if (pAncest == NULL)
		     InsertPastedElement (pElem, before, pPasted, pDoc);
		  else
		     InsertFirstChild (pParent, pPasted);
		  /* RemoveElement l'attribut Langue de l'element colle', s'il
		     herite de la meme valeur */
		  /* cherche d'abord la valeur heritee */
		  pInheritLang = GetTypedAttrAncestor (pPasted, 1, NULL, &pElAttr);
		  if (pInheritLang != NULL)
		    {
		       /* cherche l'attribut Langue de pPasted */
		       pLangAttr = GetTypedAttrForElem (pPasted, 1, NULL);
		       if (pLangAttr != NULL)
			  /* compare les valeurs de ces 2 attributs */
			  if (TextsEqual (pInheritLang->AeAttrText,
					  pLangAttr->AeAttrText))
			     /* attributs egaux, on supprime celui de pPasted */
			     DeleteAttribute (pPasted, pLangAttr);
		    }
		  /* garde le pointeur sur le sous-arbre colle' */
		  if (pAncest == NULL)
		    {
		       CreatedElement[NCreatedElements] = pPasted;
		       NCreatedElements++;
		       if (ThotLocalActions[T_pastesiblingtable] != NULL)
			  (*ThotLocalActions[T_pastesiblingtable]) (pPasted, &pOrig, pDoc);
		    }
		  else
		    {
		       CreatedElement[NCreatedElements] = pAncest;
		       NCreatedElements++;
		       if (ThotLocalActions[T_pastesiblingtable] != NULL)
			  (*ThotLocalActions[T_pastesiblingtable]) (pAncest, &pOrig, pDoc);
		    }
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
	     pElem = pEl;
	     pChild = pOrig->ElFirstChild;
	     while (pChild != NULL)
	       {
		  pPasteD->PeElement = pChild;
		  pPasted = PasteAnElement (pElem, pPasteD, before, pDoc);
		  if (!before && pPasted != NULL)
		     pElem = pPasted;
		  pChild = pChild->ElNext;
	       }
	     TtaFreeMemory ((char *) pPasteD);
	  }
     }
   return pPasted;
}


/*----------------------------------------------------------------------
   PasteCommand
   traite la commande PASTE en mode non structure'
  ----------------------------------------------------------------------*/
void                PasteCommand ()
{
   PtrDocument         pDoc;
   PtrElement          firstSel, lastSel, pEl, pPasted, pClose, pFollowing,
                       pNextEl, pFree;
   PtrPasteElem        pPasteD;
   int                 firstChar, lastChar, numAssoc, view, i;
   boolean             ok, elemIsBefore;

   if (FirstSavedElement == NULL)
      TtaDisplaySimpleMessage (INFO, LIB, TMSG_NOTHING_TO_PASTE);
   else if (GetCurrentSelection (&pDoc, &firstSel, &lastSel, &firstChar, &lastChar))
      /* on ne peut coller dans un document en lecture seule */
      if (pDoc->DocReadOnly)
	 TtaDisplaySimpleMessage (INFO, LIB, TMSG_RO_DOC_FORBIDDEN);
      else
	{
	   /*calcule le volume que pourront prendre les paves des elements colles */
	   numAssoc = firstSel->ElAssocNum;
	   if (!AssocView (firstSel))
	      /* element de l'arbre principal */
	      for (view = 0; view < MAX_VIEW_DOC; view++)
		{
		   if (pDoc->DocView[view].DvPSchemaView > 0)
		      pDoc->DocViewFreeVolume[view] = pDoc->DocViewVolume[view];
		}
	   else
	      /* element associe */
	   if (pDoc->DocAssocFrame[numAssoc - 1] > 0)
	      pDoc->DocAssocFreeVolume[numAssoc - 1] = pDoc->DocAssocVolume[numAssoc - 1];

	   pFollowing = NULL;
	   pNextEl = NULL;
	   if (firstChar < 2)
	      /* on veut coller avant l'element firstSel */
	     {
		pEl = firstSel;
		elemIsBefore = TRUE;
		/* l'element qui suivra la partie collee est le 1er element de la */
		/* selection courante */
		pNextEl = firstSel;
	     }
	   else if (firstSel->ElTerminal && firstSel->ElLeafType == LtText &&
		    firstSel->ElTextLength < firstChar)
	      /* on veut coller apres l'element firstSel */
	     {
		pEl = firstSel;
		elemIsBefore = FALSE;
		pNextEl = NextElement (pEl);
	     }
	   else
	      /* on veut coller au milieu d'une feuille de texte */
	      /* il faut couper cette feuille en deux */
	     {
		/* Si l'element a couper est le dernier, il ne le sera plus */
		/* apres la coupure. Teste si le dernier selectionne' est le */
		/* dernier fils de son pere, abstraction faite des marques de */
		/* page */
		pClose = firstSel->ElNext;
		FwdSkipPageBreak (&pClose);
		/* coupe la feuille de texte */
		SplitTextElement (firstSel, firstChar, pDoc, TRUE);
		/* deuxieme partie du texte */
		pFollowing = firstSel->ElNext;
		/* met a jour la selection */
		if (firstSel == lastSel)
		  {
		     lastSel = pFollowing;
		     lastChar = lastChar - firstChar + 1;
		  }
		pEl = firstSel;
		elemIsBefore = FALSE;
		/* l'element qui suivra la partie collee est la deuxieme partie */
		/* de l'element qu'on vient de couper en deux */
		pNextEl = pFollowing;
	     }

	   NCreatedElements = 0;

	   /* boucle sur les elements a coller et les colle un a un */
	   pPasteD = FirstSavedElement;
	   if (elemIsBefore && pPasteD != NULL)
	      /* on colle devant un element. On commencera par coller le dernier */
	      /* element a coller et on continuera en arriere */
	      while (pPasteD->PeNext != NULL)
		 pPasteD = pPasteD->PeNext;
	   ok = FALSE;
	   do
	     {
		pPasted = PasteAnElement (pEl, pPasteD, elemIsBefore, pDoc);
		if (pPasted == NULL)
		   /* echec */
		   if (!elemIsBefore && pNextEl != NULL)
		      /* on essayait de coller apres le dernier colle' */
		      /* on va essayer de coller le meme element avant l'element */
		      /* qui doit suivre la partie collee */
		      pPasted = PasteAnElement (pNextEl, pPasteD, TRUE, pDoc);
		if (pPasted != NULL)
		   /* a copy of element pPasteD has been sucessfully pasted */
		  {
		     ok = TRUE;
		     pEl = pPasted;
		  }
		if (elemIsBefore)
		   pPasteD = pPasteD->PePrevious;
		else
		   pPasteD = pPasteD->PeNext;
	     }
	   while (pPasteD != NULL);

	   if (!ok)
	      /* echec */
	     {
		TtaDisplayMessage (INFO, TtaGetMessage (LIB, TMSG_PASTING_EL_IMP),
				   FirstSavedElement->PeElement->ElStructSchema->SsRule[FirstSavedElement->PeElement->ElTypeNumber - 1].SrName);
		if (pFollowing != NULL)
		   /* on avait coupe' en deux un element de texte. On recolle les */
		   /* deux morceaux */
		  {
		     MergeTextElements (firstSel, &pFree, pDoc, TRUE, FALSE);
		     DeleteElement (&pFree);
		     pFree = NULL;
		  }
	     }
	   else
	      /* on a effectivement colle' le contenu du buffer */
	     {
		/* il faudra changer les labels lors du prochain Coller */
		ChangeLabel = TRUE;
		if (pFollowing != NULL)
		   /* on avait coupe' en deux un element de texte */
		  {
		     /* construit les paves du texte coupe' en deux */
		     BuildAbsBoxSpliText (firstSel, pFollowing, pClose, pDoc);
		  }
		/* traite dans les elements colle's toutes les references et les */
		/* elements reference's ainsi que les exclusions */
		for (i = 0; i < NCreatedElements; i++)
		  {
		     CheckReferences (CreatedElement[i], pDoc);
		     RemoveExcludedElem (&CreatedElement[i]);
		  }
		/* affecte des identificateurs corrects a tous les elements de paire */
		for (i = 0; i < NCreatedElements; i++)
		   AssignPairIdentifiers (CreatedElement[i], pDoc);
		/* Note les references sortantes colle'es */
		for (i = 0; i < NCreatedElements; i++)
		   RegisterExternalRef (CreatedElement[i], pDoc, TRUE);
		/* envoie l'evenement ElemPaste.Post */
		for (i = 0; i < NCreatedElements; i++)
		   NotifySubTree (TteElemPaste, pDoc, CreatedElement[i],
				  IdentDocument (DocOfSavedElements));
		/* cree dans toutes les vues les paves de tous les elements colle's */
		for (i = 0; i < NCreatedElements; i++)
		   if (CreatedElement[i] != NULL)
		     {
			/* cree dans toutes les vues les paves du nouvel element */
			CreateNewAbsBoxes (CreatedElement[i], pDoc, 0);
			/* calcule le volume que pourront prendre les paves des autres */
			/* elements a coller */
			if (!AssocView (pPasted))
			   for (view = 0; view < MAX_VIEW_DOC; view++)
			     {
				if (CreatedElement[i]->ElAbstractBox[view] != NULL)
				   pDoc->DocViewFreeVolume[view] -=
				      CreatedElement[i]->ElAbstractBox[view]->AbVolume;
			     }
			else
			   /* element affiche dans une vue associee */
			if (CreatedElement[i]->ElAbstractBox[0] != NULL)
			   pDoc->DocAssocFreeVolume[numAssoc - 1] -=
			      CreatedElement[i]->ElAbstractBox[0]->AbVolume;
		     }
		/* applique les regle de presentation retardees qui restent encore */
		for (i = 0; i < NCreatedElements; i++)
		   if (CreatedElement[i] != NULL)
		      ApplDelayedRule (CreatedElement[i], pDoc);
		/* refait la presentation des attributs-reference qui pointent */
		/* les elements colle's */
		for (i = 0; i < NCreatedElements; i++)
		   if (CreatedElement[i] != NULL)
		      UpdateRefAttributes (CreatedElement[i], pDoc);

		/* cherche a fusionner avec leurs voisins les elements de texte */
		/* qui viennent d'etre colle's et reaffiche le document */
		TtaClearViewSelections ();
		if (NCreatedElements > 0)
		  {
		    if (elemIsBefore)
		      MergeAndSelect (pDoc, CreatedElement[NCreatedElements - 1], CreatedElement[0], 0, 0);
		    else
		      MergeAndSelect (pDoc, CreatedElement[0], CreatedElement[NCreatedElements - 1], 0, 0);

		    pDoc->DocModified = TRUE;	/* le document est modifie' */
		    pDoc->DocNTypedChars += 20;
		  }

		/* Reaffiche les numeros suivants qui changent */
		for (i = 0; i < NCreatedElements; i++)
		  {
		     RedisplayCopies (CreatedElement[i], pDoc, TRUE);
		     UpdateNumbers (CreatedElement[i], CreatedElement[i], pDoc, TRUE);
		  }
	     }
	}
}


/*----------------------------------------------------------------------
   SelectSiblings
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                SelectSiblings (PtrElement * firstEl, PtrElement * lastEl,
				    int *firstChar, int *lastChar)
#else  /* __STDC__ */
void                SelectSiblings (firstEl, lastEl, firstChar, lastChar)
PtrElement         *firstEl;
PtrElement         *lastEl;
int                *firstChar;
int                *lastChar;

#endif /* __STDC__ */

{
   PtrElement          pParent;

   if ((*firstEl)->ElParent != (*lastEl)->ElParent)
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
			    (*lastChar == 0 || *lastChar > (*lastEl)->ElTextLength) &&
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
			   /* cet ascendant (pParent) du dernier element n'est */
			   /* pas un ascendant du premnier */
			  {
			     /* on retient pour l'instant pParent et on va regarder */
			     /* si son pere est un ascendant du premier */
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
   de l'element pEl (qui fait partie de l'element CsList pListEl) doit 
   creer un element de meme type que pEl ou un element d'un type different.
   Retourne le type de l'element a creer dans (typeNum, pSS).
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         ReturnCreateNewElem (PtrElement pListEl, PtrElement pEl,
			      boolean begin, PtrDocument pDoc, int *typeNum,
					 PtrSSchema * pSS)
#else  /* __STDC__ */
static void         ReturnCreateNewElem (pListEl, pEl, begin, pDoc, typeNum,
					 pSS)
PtrElement          pListEl;
PtrElement          pEl;
boolean             begin;
PtrDocument         pDoc;
int                *typeNum;
PtrSSchema         *pSS;

#endif /* __STDC__ */

{
   int                 TypeListe, TypeElListe, TypeEl;
   PtrSSchema          pSSList;
   SRule              *pRegle;

   pSSList = pListEl->ElStructSchema;
   *pSS = pEl->ElStructSchema;
   *typeNum = pEl->ElTypeNumber;
   TypeListe = GetTypeNumIdentity (pListEl->ElTypeNumber, pSSList);
   /* le type des elements qui constituent la liste */
   TypeElListe = pSSList->SsRule[TypeListe - 1].SrListItem;
   /* on traverse les regles d'Identite' */
   TypeEl = GetTypeNumIdentity (TypeElListe, pSSList);
   /* la regle qui definit les elements de la liste */
   pRegle = &pSSList->SsRule[TypeEl - 1];
   if (pRegle->SrConstruct == CsChoice)
      if (pRegle->SrNChoices > 0)
	 /* c'est une liste de choix, on retient la 1ere option de ce choix */
	 if (AllowedSibling (pEl, pDoc, pRegle->SrChoice[0], pSSList, begin, TRUE,
			     FALSE))
	   {
	      *typeNum = pRegle->SrChoice[0];
	      *pSS = pSSList;
	   }
}

/*----------------------------------------------------------------------
   L'utilisateur a frappe' la touche "Return".
   Traitement en mode non structure'.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtcCreateElement (Document doc, View view)
#else
void                TtcCreateElement (doc, view)
   Document doc  ; 
   View     view ;
#endif
{
   PtrElement          firstSel, lastSel, pListEl, pE, pNew, pSibling,
                       pClose, pAncest, pElem, pParent, pElDelete, pPrevious,
                       pNext, pElReplicate;
   PtrDocument         pDoc;
   PtrSSchema          pSS;
   NotifyElement       notifyEl;
   int                 firstChar, lastChar, NSiblings, typeNum;
   boolean             ok, replicate, createAfter, selBegin, selEnd, ready,
                       empty;

   if (!GetCurrentSelection (&pDoc, &firstSel, &lastSel, &firstChar, &lastChar))
      TtaDisplaySimpleMessage (INFO, LIB, TMSG_SEL_EL);
   else if (pDoc->DocReadOnly)
      TtaDisplaySimpleMessage (INFO, LIB, TMSG_RO_DOC_FORBIDDEN);
   else
      /* il y a bien une selection et le document est modifiable */
     {
	pListEl = NULL;
	createAfter = TRUE;
	replicate = TRUE;
	ready = FALSE;
	empty = FALSE;
	pElDelete = NULL;

	/* si la selection ne comprend qu'un element vide, on essaie de */
	/* remplacer cet element vide par un autre au niveau superieur */
	if (firstSel == lastSel && firstSel->ElVolume == 0)
	  {
	     empty = TRUE;
	     pElem = firstSel;
	     while (pElem->ElParent != NULL && EmptyElement (pElem->ElParent))
		pElem = pElem->ElParent;
	     if (pElem != NULL)
		if (pElem->ElParent != NULL)
		  {
		     pParent = pElem->ElParent;
		     pListEl = AncestorList (pParent);
		     if (pListEl == NULL)
		       {
			  pParent = pParent->ElParent;
			  if (pParent != NULL)
			     pListEl = AncestorList (pParent);
		       }
		     if (pListEl != NULL)
		       {
			  if (pElem->ElPrevious != NULL && pElem->ElNext == NULL)
			    {
			       /* detruire pElem et creer un frere suivant a pParent */
			       ready = TRUE;
			       pElDelete = pElem;
			       createAfter = TRUE;
			       pElReplicate = pParent;
			    }
			  else if (pElem->ElNext != NULL && pElem->ElPrevious == NULL)
			    {
			       /* detruire pElem et creer un frere precedent a pParent */
			       ready = TRUE;
			       pElDelete = pElem;
			       createAfter = FALSE;
			       pElReplicate = pParent;
			    }
			  else
			    {
			       /* try to split element pParent before element pElem */
			       if (BreakElement (pParent, pElem, 0, FALSE))
				 {
				    /* element pParent has been split */
				    ready = TRUE;
				    pElDelete = pElem;
				    createAfter = TRUE;
				    pElReplicate = pParent;
				 }
			       else
				  /* cannot split element */
				  pListEl = NULL;
			    }
		       }
		     if (pListEl == NULL)
		       {
			  pListEl = AncestorList (pElem);
			  if (pListEl != NULL)
			    {
			       ready = TRUE;
			       pElDelete = NULL;
			       createAfter = TRUE;
			       pElReplicate = pElem;
			    }
		       }
		  }
	     if (ready)
	       {
		  replicate = FALSE;
		  ReturnCreateNewElem (pListEl, pElReplicate, !createAfter, pDoc,
				       &typeNum, &pSS);
	       }
	     else
		pListEl = NULL;
	  }

	if (!ready && !empty)
	  {
	     /* la selection commence-t-elle en tete d'un element ? */
	     selBegin = FALSE;
	     if (firstSel == lastSel && firstSel->ElPrevious == NULL)
		if (firstSel->ElTerminal)
		   if (firstSel->ElLeafType == LtText && firstChar <= 1)
		      selBegin = TRUE;
		   else if (firstSel->ElLeafType == LtPicture && firstChar ==0)
		      selBegin = TRUE;

	     /* la selection est-t-elle a la fin de la derniere feuille de */
	     /* texte ou image d'un element ? */
	     selEnd = FALSE;
	     if (firstSel == lastSel && lastSel->ElNext == NULL)
		if (lastSel->ElTerminal)
		   if (lastSel->ElLeafType == LtText &&
		       firstChar > lastSel->ElTextLength)
		      selEnd = TRUE;
		   else if (firstSel->ElLeafType == LtPicture && firstChar > 0)
		      selEnd = TRUE;

	     /* Si la selection ne commence ni en tete ni en queue, on */
	     /* essaie de couper un paragraphe en deux */
	     if (!selBegin && !selEnd)
		if (BreakElement (NULL, firstSel, firstChar, TRUE))
		   return;

	     /* on cherche l'element CsList ascendant qui permet de creer un */
	     /* element voisin */
	     if (lastSel->ElTerminal && lastSel->ElLeafType == LtPageColBreak)
		/* on ne replicate pas les sauts de pages */
		pListEl = NULL;
	     else
	       {
		  pListEl = AncestorList (lastSel);
		  /* si c'est la fin d'une liste de Textes on remonte */
		  if (pListEl != NULL)
		     if (lastSel->ElTerminal &&
			 (lastSel->ElLeafType == LtText || lastSel->ElLeafType == LtPicture))
			if (pListEl == lastSel->ElParent)
			   if (lastSel->ElNext == NULL || selBegin)
			      pListEl = AncestorList (pListEl);
	       }
	     /* verifie si les elements a doubler portent l'exception NoCreate */
	     if (pListEl != NULL)
	       {
		  pE = lastSel;
		  pElReplicate = NULL;
		  do
		    {
		       if (TypeHasException (ExcNoCreate, pE->ElTypeNumber, pE->ElStructSchema))
			  /* abandon */
			  pListEl = NULL;
		       else
			 {
			    pElReplicate = pE;
			    pE = pE->ElParent;
			 }
		    }
		  while (pE != pListEl && pListEl != NULL);
		  /* a priori, on creera le meme type d'element */
		  typeNum = pElReplicate->ElTypeNumber;
		  pSS = pElReplicate->ElStructSchema;
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
	if (pListEl != NULL)
	  {
	     /* demande a l'application si on peut creer ce type d'element */
	     notifyEl.event = TteElemNew;
	     notifyEl.document = (Document) IdentDocument (pDoc);
	     notifyEl.element = (Element) (pElReplicate->ElParent);
	     notifyEl.elementType.ElTypeNum = typeNum;
	     notifyEl.elementType.ElSSchema = (SSchema) pSS;
	     pSibling = pElReplicate;
	     NSiblings = 0;
	     while (pSibling->ElPrevious != NULL)
	       {
		  NSiblings++;
		  pSibling = pSibling->ElPrevious;
	       }
	     NSiblings++;
	     notifyEl.position = NSiblings;
	     if (CallEventType ((NotifyEvent *) (&notifyEl), TRUE))
		/* l'application refuse */
		pListEl = NULL;
	  }
	if (pListEl != NULL)
	  {
	     if (IgnoreReadOnlyParent)
		ok = !ElementIsHidden (pListEl);
	     else
		ok = !ElementIsReadOnly (pListEl);
	     if (ok && pElDelete != NULL)
		/* on va detruire un sous arbre vide. */
		/* envoie l'evenement ElemDelete.Pre */
		ok = !SendEventSubTree (TteElemDelete, pDoc, pElDelete, TTE_STANDARD_DELETE_LAST_ITEM);
	     if (ok)
	       {
		  /* annule d'abord la selection */
		  TtaClearViewSelections ();
		  if (pElDelete != NULL)
		     /* detruire le sous-arbre qu'on remplace */
		    {
		       pPrevious = PreviousNotPage (pElDelete);
		       pNext = NextNotPage (pElDelete);
		       DestroyAbsBoxes (pElDelete, pDoc, TRUE);
		       AbstractImageUpdated (pDoc);
		       /* prepare l'evenement ElemDelete.Post */
		       notifyEl.event = TteElemDelete;
		       notifyEl.document = (Document) IdentDocument (pDoc);
		       notifyEl.element = (Element) (pElDelete->ElParent);
		       notifyEl.elementType.ElTypeNum = pElDelete->ElTypeNumber;
		       notifyEl.elementType.ElSSchema = (SSchema) (pElDelete->ElStructSchema);
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
		       UpdateNumbers (pClose, pElDelete, pDoc, TRUE);
		       RedisplayCopies (pElDelete, pDoc, TRUE);
		       DeleteElement (&pElDelete);
		       /* envoie l'evenement ElemDelete.Post a l'application */
		       CallEventType ((NotifyEvent *) (&notifyEl), FALSE);
		       if (pNext != NULL)
			  if (PreviousNotPage (pNext) == NULL)
			     /* l'element qui suit l'element detruit devient premier */
			     ChangeFirstLast (pNext, pDoc, TRUE, FALSE);
		       if (pPrevious != NULL)
			  if (NextNotPage (pPrevious) == NULL)
			     /* l'element qui precede l'element detruit devient dernier */
			     ChangeFirstLast (pPrevious, pDoc, FALSE, FALSE);
		    }
		  if (!replicate)
		    {
		       pE = pElReplicate;
		       pNew = NewSubtree (typeNum, pSS, pDoc,
				    pE->ElAssocNum, TRUE, TRUE, TRUE, TRUE);
		    }
		  else
		    {
		       /* Reconstruction d'une structure parallele */
		       pNew = NewSubtree (lastSel->ElTypeNumber, lastSel->ElStructSchema, pDoc,
			       lastSel->ElAssocNum, TRUE, TRUE, TRUE, TRUE);
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
		  /* traite les exclusions des elements crees */
		  RemoveExcludedElem (&pNew);
		  /* traite les attributs requis des elements crees */
		  AttachMandatoryAttributes (pNew, pDoc);
		  if (pDoc->DocSSchema != NULL)
		     /* le document n'a pas ete ferme' entre temps */
		    {
		       /* traitement des exceptions */
		       CreationExceptions (pNew, pDoc);
		       /* Mise a jour des images abstraites */
		       CreateAllAbsBoxesOfEl (pNew, pDoc);
		       /* cree les paves du nouvel element et */
		       /* met a jour ses voisins */
		       AbstractImageUpdated (pDoc);
		       /* indique au Mediateur les modifications */
		       RedisplayDocViews (pDoc);
		       /* si on est dans un element copie' par inclusion, */
		       /* on met a jour les copies de cet element. */
		       RedisplayCopies (pNew, pDoc, TRUE);
		       UpdateNumbers (NextElement (pNew), pNew, pDoc, TRUE);
		       /* Indiquer que le document est modifie' */
		       pDoc->DocModified = TRUE;
		       pDoc->DocNTypedChars += 30;
		       /* envoie un evenement ElemNew.Post a l'application */
		       NotifySubTree (TteElemNew, pDoc, pNew, 0);
		       /* Replace la selection */
		       SelectElementWithEvent (pDoc, FirstLeaf (pNew), TRUE, TRUE);
		    }
	       }
	  }
     }
}

/*----------------------------------------------------------------------
   AscentChildOfParagraph  return the ancestor of element pEl      
   (or pEl itself) whose parent has exception              
   ExcParagraphBreak.                                      
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static PtrElement   AscentChildOfParagraph (PtrElement pEl)
#else
static PtrElement   AscentChildOfParagraph (pEl)
PtrElement          pEl;

#endif /* __STDC__ */
{
   PtrElement          pAncest, pParent;
   boolean             stop;

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
#ifdef __STDC__
static PtrElement   NextSiblingNotPage (PtrElement pEl)
#else  /* __STDC__ */
static PtrElement   NextSiblingNotPage (pEl)
PtrElement          pEl;

#endif /* __STDC__ */

{
   PtrElement          pNext;
   boolean             stop;

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
   BackSpace key.                                          
   If not before, the current selection is at the end of   
   element pEl and the user has hit the Delete key.        
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                DeleteNextChar (int frame, PtrElement pEl, boolean before)
#else
void                DeleteNextChar (frame, pEl, before)
int                 frame;
PtrElement          pEl;
boolean             before;

#endif /* __STDC__ */
{
   PtrElement          pSibling, pNext, pPrev, pE, pElem, pParent, pS,
                       pSel, pSuccessor;
   PtrDocument         pDoc;
   NotifyElement       notifyEl;
   NotifyOnValue       notifyVal;
   int                 nSiblings;
   boolean             stop;

   if (pEl == NULL)
      return;
   if (before)
      pSel = NULL;
   else
      pSel = pEl;
   pElem = NULL;
   pDoc = DocumentOfElement (pEl);

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
	  }
     }
   while (pParent != NULL && pSibling == NULL);
   if (pParent == NULL || pSibling == NULL)
      return;
   if (pSibling != NULL && pParent != pEl && pElem != NULL)
      if (pSibling->ElTerminal)
	 pSibling = NULL;
      else
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
		if (!AllowedSibling (pSibling, pDoc, pElem->ElTypeNumber, pElem->ElStructSchema,
				     FALSE, FALSE, FALSE))
		   pSibling = NULL;
	     }
	}
   if (pSibling == NULL || pParent == pEl)
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
		  else if (TypeHasException (ExcParagraphBreak, pParent->ElTypeNumber,
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
	      pSibling = AscentChildOfParagraph (PreviousLeaf (pElem));
	   else
	     {
		pSibling = pElem;
		pElem = AscentChildOfParagraph (NextLeaf (pSibling));
	     }
	else
	  {
	     if (!pSibling->ElTerminal)
		if (before)
		   pSibling = PreviousLeaf (pElem);
		else
		   pSibling = NextLeaf (pElem);
	     if (!pSibling->ElTerminal)
	       {
		  if (pSibling->ElFirstChild == NULL)
		    {
		       /* pSibling is empty. Delete it */
		       SelectElement (pDoc, pSibling, TRUE, TRUE);
		       CutCommand (FALSE);
		    }
	       }
	     else
	       {
		  if (pSibling->ElLeafType == LtPairedElem)
		     /* skip the paired element */
		     DeleteNextChar (frame, pSibling, before);
		  else if (pSibling->ElLeafType == LtText)
		     if (before)
		       {
			  /* set selection after the last character of the string */
			  MoveCaret (pDoc, pSibling, pSibling->ElTextLength + 1);
			  /* simulate a backspace */
			  InsertChar (frame, '\177', -1);
		       }
		     else
		       {
			  /* set selection before the first character of the string */
			  SelectString (pDoc, pSibling, 1, 1);
			  /* simulate a delete */
			  TtcDeleteSelection (0, 0);
		       }
		  else if (pSibling->ElLeafType == LtPicture)
		    {
			  /* set selection before the first character of the string */
			  SelectElement (pDoc, pSibling, FALSE, FALSE);
			  /* simulate a delete */
			  TtcDeleteSelection (0, 0);
		    }
	       }
	     /* done */
	     pElem = NULL;
	     pSibling = NULL;
	  }
     }

   if (pElem != NULL && pSibling != NULL)
      if (AllowedSibling (pSibling, pDoc, pElem->ElTypeNumber, pElem->ElStructSchema,
			  FALSE, FALSE, FALSE))
	{
	   TtaClearViewSelections ();	/* annule d'abord la selection */
	   pParent = pElem->ElParent;
	   while (pElem != NULL)
	     {
		pNext = pElem->ElNext;
		/* envoie l'evenement ElemDelete.Pre a l'application */
		if (!SendEventSubTree (TteElemDelete, pDoc, pElem, TTE_STANDARD_DELETE_LAST_ITEM))
		  {
		     /* detruit les paves de l'element qui va etre deplace' */
		     DestroyAbsBoxes (pElem, pDoc, TRUE);
		     AbstractImageUpdated (pDoc);
		     /* prepare l'evenement ElemDelete.Post */
		     notifyEl.event = TteElemDelete;
		     notifyEl.document = (Document) IdentDocument (pDoc);
		     notifyEl.element = (Element) (pElem->ElParent);
		     notifyEl.elementType.ElTypeNum = pElem->ElTypeNumber;
		     notifyEl.elementType.ElSSchema = (SSchema) (pElem->ElStructSchema);
		     nSiblings = 0;
		     pS = pElem;
		     while (pS->ElPrevious != NULL)
		       {
			  nSiblings++;
			  pS = pS->ElPrevious;
		       }
		     notifyEl.position = nSiblings;
		     pSuccessor = NextElement (pEl);
		     /* retire l'element de l'arbre abstrait */
		     RemoveElement (pElem);
		     UpdateNumbers (pSuccessor, pElem, pDoc, TRUE);
		     RedisplayCopies (pElem, pDoc, TRUE);
		     /* envoie l'evenement ElemDelete.Post a l'application */
		     CallEventType ((NotifyEvent *) (&notifyEl), FALSE);
		     /* envoie un evenement ElemPaste.Pre a l'application */
		     notifyVal.event = TteElemPaste;
		     notifyVal.document = (Document) IdentDocument (pDoc);
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
		     if (CallEventType ((NotifyEvent *) (&notifyVal), TRUE))
			/* l'application refuse, on libere l'element */
			DeleteElement (&pElem);
		     else
		       {
			  /* l'application accepte */
			  /* insere l'element a sa nouvelle position */
			  InsertElementAfter (pSibling, pElem);
			  NotifySubTree (TteElemPaste, pDoc, pElem, 0);
		       }
		  }
		pSibling = pElem;
		if (pSel == NULL)
		   pSel = pElem;
		/* passe a l'element suivant */
		if (pNext == NUL)
		   pElem = NULL;
		else if (AllowedSibling (pSibling, pDoc, pNext->ElTypeNumber,
				pNext->ElStructSchema, FALSE, FALSE, FALSE))
		   pElem = pNext;
		else
		   pElem = NULL;
	     }
	   /* detruit les elements qui ont ete vide's */
	   pPrev = NULL;
	   while (pParent->ElFirstChild == NULL)
	     {
		pE = pParent;
		pParent = pE->ElParent;
		/* envoie l'evenement ElemDelete.Pre et demande a */
		/* l'application si elle est d'accord pour detruire l'elem. */
		if (!SendEventSubTree (TteElemDelete, pDoc, pE, TTE_STANDARD_DELETE_LAST_ITEM))
		  {
		     /* cherche l'element qui precede l'element a detruire */
		     pPrev = PreviousNotPage (pE);
		     DestroyAbsBoxes (pE, pDoc, TRUE);
		     AbstractImageUpdated (pDoc);
		     pNext = NextElement (pE);
		     /* prepare l'evenement ElemDelete.Post */
		     notifyEl.event = TteElemDelete;
		     notifyEl.document = (Document) IdentDocument (pDoc);
		     notifyEl.element = (Element) pParent;
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
		     /* retire l'element courant de l'arbre */
		     RemoveElement (pE);
		     UpdateNumbers (pNext, pE, pDoc, TRUE);
		     DeleteElement (&pE);
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
	   pDoc->DocModified = TRUE;
	   pDoc->DocNTypedChars += 30;
	   /* selectionne */
	   if (pSel != NULL)
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
	}
}

/*----------------------------------------------------------------------
   NoStructureLoadResources connecte les fonctions d'edition et de    
   selection structuree.                                  
  ----------------------------------------------------------------------*/
void                NoStructSelectLoadResources ()
{
   if (ThotLocalActions[T_cmdpaste] == NULL)
     {
	/* connecte les action d'edition structuree */
	TteConnectAction (T_selecbox, (Proc) GetClickedBox);
	TteConnectAction (T_selectsiblings, (Proc) SelectSiblings);
	TteConnectAction (T_deletenextchar, (Proc) DeleteNextChar);
	TteConnectAction (T_cmdpaste, (Proc) PasteCommand);
	TteConnectAction (T_enter, (Proc) TtcCreateElement);
	if (MenuActionList[CMD_CreateElement].Call_Action != NULL)
	   MenuActionList[CMD_CreateElement].Call_Action = (Proc) TtcCreateElement;
	StructSelectionMode = FALSE;
     }
}
