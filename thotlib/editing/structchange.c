/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 
/*
 * This module handles the Paste command in structure mode
 *
 * Authors: I. Vatton (INRIA)
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
#include "edit_tv.h"
#include "appdialogue_tv.h"

#include "absboxes_f.h"
#include "actions_f.h"
#include "appli_f.h"
#include "applicationapi_f.h"
#include "attributeapi_f.h"
#include "attributes_f.h"
#include "attrpresent_f.h"
#include "boxselection_f.h"
#include "buildboxes_f.h"
#include "callback_f.h"
#include "changeabsbox_f.h"
#include "changepresent_f.h"
#include "contentapi_f.h"
#include "content_f.h"
#include "createabsbox_f.h"
#include "displayview_f.h"
#include "documentapi_f.h"
#include "editcommands_f.h"
#include "exceptions_f.h"
#include "externalref_f.h"
#include "fileaccess_f.h"
#include "memory_f.h"
#include "presrules_f.h"
#include "references_f.h"
#include "scroll_f.h"
#include "selectmenu_f.h"
#include "search_f.h"
#include "searchref_f.h"
#include "structcommands_f.h"
#include "structcreation_f.h"
#include "structlocate_f.h"
#include "structmodif_f.h"
#include "structschema_f.h"
#include "structselect_f.h"
#include "textcommands_f.h"
#include "thotmsg_f.h"
#include "tree_f.h"
#include "undo_f.h"
#include "views_f.h"

#ifdef IV

/*----------------------------------------------------------------------
   AppendToFreeList append element pFree to the element list	
   anchored at pAnchor						
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static void         AppendToFreeList (PtrElement pFree, PtrElement * pAnchor)

#else  /* __STDC__ */
static void         AppendToFreeList (pFree, pAnchor)
PtrElement          pFree;
PtrElement         *pAnchor;

#endif /* __STDC__ */

{
   PtrElement          pEl;

   if (pFree != NULL)
     {
	pFree->ElNext = NULL;
	if (*pAnchor == NULL)
	   *pAnchor = pFree;
	else
	  {
	     pEl = *pAnchor;
	     while (pEl->ElNext != NULL)
		pEl = pEl->ElNext;
	     pEl->ElNext = pFree;
	  }
     }
}
#endif /* IV */


/*----------------------------------------------------------------------
   PasteBeforeOrAfter

   Insere une copie des elements sauvegardes devant (ou derriere, si before
   est faux) l'element pEl dans le document pDoc.
   updateVol indique si le volume libre pour les paves des elements qui
   seront colles doit etre mis a jour ou s'il a deja ete calcule'.
   pSavedEl pointe sur le premier element de la sauvegarde qu'il faut inserer.
   On insere cet element et tous ceux qui le suivent.
   Retourne dans pFirstPastedEl un pointeur sur le premier element insere',
   ou NULL si rien n'a ete insere'.
   Au retour, si une insertion a eu lieu, pFirstFree contient un pointeur sur
   une suite d'elements a liberer apres reaffichage. Ce sont les elements
   supprimes par fusion de feuilles de texte voisines.
   Au retour firstPastedChar contient le rang du premier caractere colle'
   dans le premier element insere', ou 0 si tout le premier element insere'
   est nouveau. 
   Si typeNum n'est pas nul, on essaie de donner a l'element colle' le type
   de numero typeNum defini dans le schema de structure pSS
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         PasteBeforeOrAfter (PtrElement * pFirstPastedEl, PtrDocument pDoc, PtrElement pEl, ThotBool before, ThotBool updateVol, PtrElement pSavedEl, PtrElement * pFirstFree, int *firstPastedChar, int typeNum, PtrSSchema pSS)
#else  /* __STDC__ */
static void         PasteBeforeOrAfter (pFirstPastedEl, pDoc, pEl, before, updateVol, pSavedEl, pFirstFree, firstPastedChar, typeNum, pSS)
PtrElement         *pFirstPastedEl;
PtrDocument         pDoc;
PtrElement          pEl;
ThotBool            before;
ThotBool            updateVol;
PtrElement          pSavedEl;
PtrElement         *pFirstFree;
int                *firstPastedChar;
int                 typeNum;
PtrSSchema          pSS;
#endif /* __STDC__ */
{
   PtrSSchema          pSSSibling;
   PtrElement          pSavedChild, pSibling, pPastedEl, pDescRoot,
                       pParent, pDesc, pCopy, pClose, pCreatedEl,
                       pElAttr;
   PtrAttribute        pInheritLang, pLangAttr;
   NotifyElement       notifyEl;
   NotifyOnValue       notifyVal;
   Document            doc;
   int                 view, distance, numAssoc, i, NSiblings, originDoc,
                       siblingType;
   ThotBool            typeOK, creation, list, stop, optional, last, table;

   typeOK = FALSE;
   *firstPastedChar = 0;
   *pFirstPastedEl = NULL;
   pPastedEl = NULL;
   *pFirstFree = NULL;
   doc = IdentDocument (pDoc);
   if (pEl != NULL)
      if (pEl->ElParent != NULL)
	 /* on ne peut pas inserer a cote d'une racine */
	{
	   numAssoc = pEl->ElAssocNum;
	   /* cherche le dernier element sauvegarde' si on colle devant */
	   if (before && pSavedEl != NULL)
	     {
		last = TRUE;
		if (ThotLocalActions[T_lastsaved] != NULL)
		   (*ThotLocalActions[T_lastsaved]) (pSavedEl, &last);
		if (!last)
		   while (pSavedEl->ElNext != NULL)
		      pSavedEl = pSavedEl->ElNext;
	     }
	   /* calcule le volume que pourront prendre les paves des elements
	      colle's */
	   if (updateVol)
	     {
		if (!AssocView (pEl))
		   /* element de l'arbre principal */
		   for (view = 0; view < MAX_VIEW_DOC; view++)
		     {
			if (pDoc->DocView[view].DvPSchemaView > 0)
			   pDoc->DocViewFreeVolume[view] =
			      pDoc->DocViewVolume[view];
		     }
		else if (pDoc->DocAssocFrame[numAssoc - 1] > 0)
		   /* element associe */
		   pDoc->DocAssocFreeVolume[numAssoc - 1] =
		      pDoc->DocAssocVolume[numAssoc - 1];
	     }
	   /* boucle sur les elements sauvegarde's */
	   while (pSavedEl != NULL)
	     {
		/* on n'a pas cree d'element intermediaire */
		creation = FALSE;
		/* cherche le type du voisin immediat prevu par le schema de
		   structure */
		typeNum = 0;
		if (pEl != NULL)
		   SRuleForSibling (pDoc, pEl, before, 1, &typeNum, &pSS, &list,
				    &optional);
		if (typeNum == 0)
		   /* pas de voisin prevu explicitement */
		  {
		     /* l'element a coller est peut-etre autorise' comme une
		        inclusion associee au type d'un element ascendant */
		     if (AllowedIncludedElem (pDoc,
                                              pEl->ElParent,
					      pSavedEl->ElTypeNumber,
					      pSavedEl->ElStructSchema))
			/* c'est effectivement une inclusion autorisee */
			/* on ne peut pas mettre une inclusion directement */
			/* dans un element choix */
			if (pEl->ElParent->ElStructSchema->SsRule[pEl->ElParent->ElTypeNumber - 1].SrConstruct == CsChoice)
			   /* on ne colle plus rien */
			   pSavedEl = NULL;
			else
			   /* on peut mettre cette inclusion */
			   typeOK = TRUE;
		     else
			/* on ne colle plus rien */
			pSavedEl = NULL;
		  }

		if (pSavedEl != NULL)
		   /* il y a un voisin possible */
		  {
		     /* on calcule le nombre de freres qui precederont
		        l'element lorsqu'il sera colle' */
		     pSibling = pEl;
		     NSiblings = 0;
		     while (pSibling->ElPrevious != NULL)
		       {
			  NSiblings++;
			  pSibling = pSibling->ElPrevious;
		       }
		     if (!before)
			NSiblings++;
		     if (typeNum != 0)
			if (list)
			   typeOK = EquivalentSRules (typeNum, pSS,
						      pSavedEl->ElTypeNumber,
						   pSavedEl->ElStructSchema,
						      pEl->ElParent);
			else
			   typeOK = SameSRules (typeNum, pSS,
						pSavedEl->ElTypeNumber,
						pSavedEl->ElStructSchema);
		     if (!typeOK)
			/* l'element a coller n'a pas un type correct */
			/* Il est peut-etre autorise' par une inclusion */
			/* associee au type d'un element ascendant */
			typeOK = AllowedIncludedElem (pDoc,
                                                      pEl->ElParent,
						      pSavedEl->ElTypeNumber,
						  pSavedEl->ElStructSchema);
		     if (!typeOK)
			/* l'element a coller n'a pas un type correspondant au
			   voisin immediat prevu par le schema de structure.
			   si on colle a cote' d'un element d'agregat, chercher
			   si l'element a coller peut etre un voisin non
			   immediat : utile si la sauvegarde contient une suite
			   d'elements d'agregat dont certains sont absents */
		       {
			  distance = 1;
			  typeOK = FALSE;
			  do
			    {
			       /* distance du voisin prevu par le schema */
			       distance++;
			       SRuleForSibling (pDoc, pEl, before, distance,
			       &siblingType, &pSSSibling, &list, &optional);
			       if (siblingType > 0)
				  typeOK = EquivalentSRules (siblingType,
					 pSSSibling, pSavedEl->ElTypeNumber,
				   pSavedEl->ElStructSchema, pEl->ElParent);
			    }
			  while (!typeOK && siblingType != 0);
			  if (!typeOK)
			     /* s'il n'y a qu'un element a coller, cherche si
			        le contenu de cet element convient, a condition
			        que ce ne soit pas un parametre. */
			    {
			       if (pSavedEl->ElPrevious == NULL
				   && pSavedEl->ElNext == NULL
				   && !pSavedEl->ElTerminal
				   && pSavedEl->ElFirstChild != NULL
				   && !pSavedEl->ElStructSchema->SsRule[pSavedEl->ElTypeNumber - 1].SrParamElem)
				 {
				    pSavedChild = pSavedEl->ElFirstChild;
				    /* cherche le dernier element sauvegarde'
				       si on colle devant */
				    if (before)
				       while (pSavedChild->ElNext != NULL)
					  pSavedChild = pSavedChild->ElNext;
				    if (EquivalentSRules (typeNum, pSS,
						  pSavedChild->ElTypeNumber,
						pSavedChild->ElStructSchema,
							  pEl->ElParent))
				       /* type correct, on collera le contenu */
				      {
					 typeOK = TRUE;
					 pSavedEl = pSavedChild;
				      }
				 }
			       if (!typeOK)
				  /* essaie de creer une descendance jusqu'a
				     un element du type de celui a coller */
				 {
				    notifyEl.event = TteElemNew;
				    notifyEl.document = doc;
				    notifyEl.element = (Element) (pEl->ElParent);
				    notifyEl.elementType.ElTypeNum = typeNum;
				    notifyEl.elementType.ElSSchema = (SSchema) pSS;
				    notifyEl.position = NSiblings;
				    pDescRoot = NULL;
				    if (!CallEventType ((NotifyEvent *) (&notifyEl), TRUE))
				       pDescRoot = CreateDescendant (typeNum, pSS,
					      pDoc, &pDesc, pEl->ElAssocNum,
						     pSavedEl->ElTypeNumber,
						  pSavedEl->ElStructSchema);
				    if (pDescRoot != NULL)
				       /* on a pu creer la descendance voulue */
				      {
					 if (!list ||
					     (pSS->SsRule[typeNum - 1].SrConstruct != CsChoice &&
					      pSS->SsRule[typeNum - 1].SrConstruct != CsNatureSchema))
					    if (pDescRoot->ElTypeNumber != typeNum
						|| pDescRoot->ElStructSchema != pSS)
					      {
						 pParent = NewSubtree (typeNum,
								  pSS, pDoc,
						     pEl->ElAssocNum, FALSE,
							  TRUE, TRUE, TRUE);
						 InsertFirstChild (pParent, pDescRoot);
						 pDescRoot = pParent;
					      }
					 typeOK = TRUE;
					 pCreatedEl = pDesc;
					 while (pCreatedEl != NULL && typeOK)
					   {
					      table = FALSE;
					      if (ThotLocalActions[T_entertable] != NULL)
						 (*ThotLocalActions[T_entertable]) (pCreatedEl, &table);
					      if (table)
						 /* il y a une table, on abandonne */
						 typeOK = FALSE;
					      else
						 pCreatedEl = pCreatedEl->ElParent;
					   }
					 if (!typeOK)
					   {
					      /* detruit ce qui vient d'etre cree' */
					      DeleteElement (&pDescRoot, pDoc);
					      pDescRoot = NULL;
					   }
					 else
					    /* on a cree une descendance */
					    creation = TRUE;
				      }
				 }
			       if (!typeOK)
				  pSavedEl = NULL;
			    }
		       }
		     if (pSavedEl != NULL)
			/* l'element a coller a un type convenable */
		       {
			  if (creation)
			     /* etablit provisoirement le chainage des
			        ascendants pour que la copie des attributs
			        puisse se faire correctement */
			    {
			       pDescRoot->ElParent = pEl->ElParent;
			       pParent = pDesc->ElParent;
			       /* traite les attributs requis des elements crees */
			       AttachMandatoryAttributes (pParent, pDoc);
			    }
			  else
			    {
			       pParent = pEl->ElParent;
			       pDescRoot = NULL;
			    }
			  /* Cree une copie de l'element a coller.
			     Si l'element est reference', la copie devient
			     l'element reference'. Ne copie les attributs que
			     s'ils sont definis dans les schemas de structure
			     des elements englobants du document d'arrivee. */
			  pCopy = NULL;
			  if (pDoc->DocSSchema != NULL)
			     /* le document n'a pas ete ferme' entre temps */
			    {
			       notifyVal.event = TteElemPaste;
			       notifyVal.document = doc;
			       notifyVal.element = (Element) pParent;
			       notifyVal.target = (Element) pSavedEl;
			       notifyVal.value = NSiblings;
			       if (!CallEventType ((NotifyEvent *) (&notifyVal), TRUE))
				  pCopy = CopyTree (pSavedEl, DocOfSavedElements,
						    pEl->ElAssocNum,
					      pParent->ElStructSchema, pDoc,
						    pParent, TRUE, TRUE);
			    }
			  if (creation && pDescRoot != NULL)
			     /* maintenant que la copie est faite, on supprime
			        le chainage provisoire au pere pour que le
			        calcul du volume des ascendants soit correct */
			     pDescRoot->ElParent = NULL;
			  if (pCopy == NULL || (creation && pDescRoot == NULL))
			    {
			       if (pDescRoot != NULL)
				  DeleteElement (&pDescRoot, pDoc);
			       pSavedEl = NULL;
			    }
			  else
			     /* insere la copie dans l'arbre */
			    {
			       if (creation)
				  /* insere la copie a la place du dernier
				     element cree */
				 {
				    InsertElementAfter (pDesc, pCopy);
				    DeleteElement (&pDesc, pDoc);
				    pPastedEl = pDescRoot;
				 }
			       else
				  pPastedEl = pCopy;
			       if (before)
				  /* teste si pEl est le premier fils de son
				     pere, abstraction faite des sauts de page */
				 {
				    pClose = pEl->ElPrevious;
				    stop = FALSE;
				    do
				       if (pClose == NULL)
					  stop = TRUE;
				       else if (!pClose->ElTerminal || pClose->ElLeafType != LtPageColBreak)
					  stop = TRUE;
				       else
					  pClose = pClose->ElPrevious;
				    while (!stop);
				    InsertElementBefore (pEl, pPastedEl);
				    last = FALSE;
				    if (ThotLocalActions[T_lastsaved] != NULL)
				       (*ThotLocalActions[T_lastsaved]) (pSavedEl, &last);
				    if (last)
				       /* traitement particulier */
				       /* aux tableaux */
				       pSavedEl = pSavedEl->ElNext;
				    else
				       pSavedEl = pSavedEl->ElPrevious;
				    if (pClose == NULL)
				       /* l'element devant lequel */
				       /* on colle n'est plus */
				       /* premier */
				       ChangeFirstLast (pEl, pDoc, TRUE, TRUE);
				 }
			       else
				 {
				    /* teste si pEl est le dernier fils de son
				       pere, abstraction faite des sauts de page */
				    pClose = pEl->ElNext;
				    stop = FALSE;
				    do
				       if (pClose == NULL)
					  stop = TRUE;
				       else if (!pClose->ElTerminal ||
				       pClose->ElLeafType != LtPageColBreak)
					  stop = TRUE;
				       else
					  pClose = pClose->ElNext;
				    while (!stop);
				    InsertElementAfter (pEl, pPastedEl);
				    pSavedEl = pSavedEl->ElNext;
				    if (pClose == NULL)
				       /* l'element apres lequel on colle n'est
				          plus dernier */
				       ChangeFirstLast (pEl, pDoc, FALSE, TRUE);
				 }
			       /* Retire l'attribut Langue de pCopy si pCopy
			          herite la meme valeur */
			       /* cherche d'abord la valeur heritee */
			       pInheritLang = GetTypedAttrAncestor (pCopy, 1,
							    NULL, &pElAttr);
			       if (pInheritLang != NULL)
				 {
				    /* cherche l'attribut Langue de pCopy */
				    pLangAttr = GetTypedAttrForElem (pCopy, 1,
								     NULL);
				    if (pLangAttr != NULL)
				       /* compare les valeurs de ces 2 attributs */
				       if (TextsEqual (pInheritLang->AeAttrText, pLangAttr->AeAttrText))
					  /* attributs egaux, on supprime celui
					     de pCopy */
					  {
					  RemoveAttribute (pCopy, pLangAttr);
					  DeleteAttribute (pCopy, pLangAttr);
					  }
				 }
			       pEl = pCopy;
			       /* garde le pointeur sur le sous arbre colle' */
			       NCreatedElements++;
			       CreatedElement[NCreatedElements - 1] = pPastedEl;
			       if (ThotLocalActions[T_pastesiblingtable] != NULL)
				  (*ThotLocalActions[T_pastesiblingtable])
				     (pPastedEl, &pSavedEl, pDoc);
			    }
		       }
		  }
	     }
	   

	   /* traite dans les elements colle's toutes les references et les */
	   /* elements reference's ainsi que les exclusions */
	   for (i = 0; i < NCreatedElements; i++)
	     {
		CheckReferences (CreatedElement[i], pDoc);
		if (CreatedElement[i] == pPastedEl)
		  {
		     RemoveExcludedElem (&CreatedElement[i], pDoc);
		     if (CreatedElement[i] == NULL)
			/* l'element a ete retire' */
			pPastedEl = NULL;
		  }
		else
		   RemoveExcludedElem (&CreatedElement[i], pDoc);
	     }

	   /* affecte des identificateurs corrects a tous les elements de paire */
	   for (i = 0; i < NCreatedElements; i++)
	      AssignPairIdentifiers (CreatedElement[i], pDoc);

	   /* Note les references sortantes colle'es */
	   for (i = 0; i < NCreatedElements; i++)
	      RegisterExternalRef (CreatedElement[i], pDoc, TRUE);

	
	   /* envoie l'evenement ElemPaste.Post */
	   originDoc = IdentDocument (DocOfSavedElements);
	   if (before)
	      for (i = NCreatedElements - 1; i >= 0; i--)
		 NotifySubTree (TteElemPaste, pDoc, CreatedElement[i], originDoc);
	   else
	      for (i = 0; i < NCreatedElements; i++)
		 NotifySubTree (TteElemPaste, pDoc, CreatedElement[i], originDoc);

	   /* cree dans toutes les vues les paves de tous les elements colle's */
	   for (i = 0; i < NCreatedElements; i++)
	      if (CreatedElement[i] != NULL)
		{
		   /* cree dans toutes les vues les paves du nouvel element */
		   CreateNewAbsBoxes (CreatedElement[i], pDoc, 0);
		   /* calcule le volume que pourront prendre les paves des autres */
		   /* elements a coller */
		   if (!AssocView (pEl))
		      for (view = 0; view < MAX_VIEW_DOC; view++)
			{
			   if (CreatedElement[i]->ElAbstractBox[view] != NULL)
			      pDoc->DocViewFreeVolume[view] -= CreatedElement[i]->ElAbstractBox[view]->AbVolume;
			}
		   else if (CreatedElement[i]->ElAbstractBox[0] != NULL)
		      /* element affiche dans une vue associee */
		      pDoc->DocAssocFreeVolume[numAssoc - 1] -= CreatedElement[i]->ElAbstractBox[0]->AbVolume;
		}

	   /* applique les regles de presentation retardees qui restent encore */
	   for (i = 0; i < NCreatedElements; i++)
	      if (CreatedElement[i] != NULL)
		 ApplDelayedRule (CreatedElement[i], pDoc);

	   /* refait la presentation des attributs-reference qui pointent */
	   /* les elements colle's */
	   for (i = 0; i < NCreatedElements; i++)
	      if (CreatedElement[i] != NULL)
		 UpdateRefAttributes (CreatedElement[i], pDoc);

	   if (CreatedElement[0]!=NULL)
	     if (before)
	       *pFirstPastedEl = CreatedElement[NCreatedElements - 1];
	     else
	       *pFirstPastedEl = CreatedElement[0];
	   

#ifdef IV
	   /* cherche a fusionner les nouveaux elements avec leurs voisins */
	   for (i = NCreatedElements - 1; i >= 0; i--)
	      if (CreatedElement[i] != NULL)
		{
		   pPrevNew = CreatedElement[i]->ElPrevious;
		   *firstPastedChar = 0;
		   if (pPrevNew != NULL)
		     {
			*firstPastedChar = pPrevNew->ElTextLength + 1;
			if (IsIdenticalTextType (pPrevNew, pDoc, &pFree))
			   /* la fusion avec le precedent a eu lieu */
			  {
			     CreatedElement[i] = pPrevNew;
			     pPastedEl = pPrevNew;
			     /* chaine l'element libere' par la fusion */
			     AppendToFreeList (pFree, pFirstFree);
			  }
			else
			   /* pas de fusion */
			   *firstPastedChar = 0;
		     }
		   *pFirstPastedEl = pPastedEl;

		   /* cherche la fusion avec l'element suivant */
		   pFree = NULL;
		   if (IsIdenticalTextType (pPastedEl, pDoc, &pFree))
		     {
			if (i < NCreatedElements - 1)
			   if (pFree == CreatedElement[NCreatedElements - 1])
			      CreatedElement[NCreatedElements - 1] = pPastedEl;
			AppendToFreeList (pFree, pFirstFree);
		     }
		}
#endif
	}
}


/*----------------------------------------------------------------------
   PasteWithin
   Insere les elements sauvegardes. L'insertion a lieu dans l'element pEl
   dans le document pDoc.
   Au retour, pFirstPastedEl est un pointeur sur le premier element insere',
   ou NULL si rien n'a ete insere'.
   Si une insertion a eu lieu, pFirstFree contient un pointeur sur une suite
   d'elements a liberer apres reaffichage. Ce sont les elements supprimes par
   fusion d'elements texte voisins.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                PasteWithin (PtrElement * pFirstPastedEl, PtrDocument pDoc, PtrElement pEl, PtrElement * pFirstFree)
#else  /* __STDC__ */
void                PasteWithin (pFirstPastedEl, pDoc, pEl, pFirstFree)
PtrElement         *pFirstPastedEl;
PtrDocument         pDoc;
PtrElement          pEl;
PtrElement         *pFirstFree;

#endif /* __STDC__ */
{
   PtrElement          pPastedEl, pDescRoot, pSavedEl, pSavedChild, pElem,
                       pParent, pDesc, pSibling, pSib, pChild, pCopy, pElAttr;
   PtrSSchema          pSS;
   PtrAttribute        pInheritLang, pLangAttr;
   NotifyElement       notifyEl;
   NotifyOnValue       notifyVal;
   Document            doc;
   int                 view, firstChar, typeNum;
   ThotBool            ok, sameType, creation, empty, replace;

   *pFirstPastedEl = NULL;
   *pFirstFree = NULL;
   doc = IdentDocument (pDoc);
   ok = FALSE;
   creation = FALSE;
   replace = FALSE;
   pDescRoot = NULL;
   pCopy = NULL;
   if (FirstSavedElement == NULL)
      return;
   pSavedEl = FirstSavedElement->PeElement;
   if (pEl->ElStructSchema->SsRule[pEl->ElTypeNumber - 1].SrConstruct ==
       CsReference &&
       pSavedEl->ElTypeNumber == pEl->ElTypeNumber &&
       pSavedEl->ElStructSchema->SsCode == pEl->ElStructSchema->SsCode &&
       FirstSavedElement->PeNext == NULL)
      /* l'element dans lequel on colle est une reference du meme type
         que l'element a coller, et il n'y a qu'un element a coller */
     {
	/* on remplacera l'element pEl par une copie de l'element a coller */
	ok = TRUE;
	replace = TRUE;
     }
   else if (!pEl->ElTerminal)
      /* on ne colle rien dans une feuille */
     {
	empty = TRUE;
	if (pEl->ElFirstChild != NULL)
	  {
	     /* saute les marques de pages contenues dans l'element */
	     pChild = pEl->ElFirstChild;
	     FwdSkipPageBreak (&pChild);
	     if (pChild != NULL)
		/* l'element a deja un contenu, on colle devant son premier
		   fils */
	       {
		  empty = FALSE;
		  if (FirstSavedElement != NULL)
		     PasteBeforeOrAfter (pFirstPastedEl, pDoc, pChild, TRUE,
			     TRUE, FirstSavedElement->PeElement, pFirstFree,
					 &firstChar, 0, NULL);
	       }
	  }
	if (empty)
	  {
	     /* on n'insere que dans un element vide qui n'est pas une feuille */
	     sameType = SameSRules (pEl->ElTypeNumber, pEl->ElStructSchema,
				    pSavedEl->ElTypeNumber,
				    pSavedEl->ElStructSchema);
	     /* l'element sauvegarde' peut-il etre un descendant direct de
	        l'element ou on veut le coller ? */
	     ok = EquivalentSRules (pEl->ElTypeNumber, pEl->ElStructSchema,
				    pSavedEl->ElTypeNumber,
				    pSavedEl->ElStructSchema, pEl);
	     if (ok)
		/* on n'accepte pas qu'ils soient de meme type */
		ok = !sameType;
	     if (!ok)
		/* ce n'est pas un descendant direct, cherche si l'element
		   sauvegarde' peut descendre de l'element ou l'on colle par
		   une regle liste ou agregat */
	       {
		  typeNum = pEl->ElTypeNumber;
		  pSS = pEl->ElStructSchema;
		  ListOrAggregateRule (pDoc, pSavedEl, &typeNum, &pSS);
		  ok = pSS != NULL;
	       }
	     if (!ok)
		/* s'il n'y a qu'un element a copier, cherche si le contenu de
		   cet element convient */
		if (pSavedEl->ElPrevious == NULL
		    && pSavedEl->ElNext == NULL
		    && !pSavedEl->ElTerminal
		    && pSavedEl->ElFirstChild != NULL)
		  {
		     pSavedChild = pSavedEl->ElFirstChild;
		     if (EquivalentSRules (pEl->ElTypeNumber,
					   pEl->ElStructSchema,
					   pSavedChild->ElTypeNumber,
					   pSavedChild->ElStructSchema, pEl))
			/* type correct, on collera le contenu */
		       {
			  ok = TRUE;
			  pSavedEl = pSavedChild;
			  if (pEl->ElTypeNumber == pSavedChild->ElTypeNumber)
			     if (pEl->ElStructSchema->SsCode ==
				 pSavedChild->ElStructSchema->SsCode)
				/* le contenu est du meme type que l'element a
				   l'interieur duquel on colle. On prend le
				   contenu du contenu */
				if (pSavedEl->ElTerminal)
				   pSavedEl = NULL;
				else
				   pSavedEl = pSavedEl->ElFirstChild;
		       }
		  }
	     if (!ok)
	       {
		  /* l'element a coller est peut-etre autorise' par une
		     inclusion associee au type d'un element ascendant */
		  ok = AllowedIncludedElem (pDoc, pEl, pSavedEl->ElTypeNumber, pSavedEl->ElStructSchema);
		  /* on ne peut pas mettre une inclusion directement dans un
		     element Choix */
		  if (pEl->ElStructSchema->SsRule[pEl->ElTypeNumber - 1].SrConstruct == CsChoice)
		     ok = FALSE;
	       }
	     if (!ok && !sameType)
		/* essaie de creer une descendance de l'element ou l'on colle
		   jusqu'a un element (pDesc) du meme type que l'element
		   sauvegarde' */
	       {
		  notifyEl.event = TteElemNew;
		  notifyEl.document = doc;
		  notifyEl.element = (Element) pEl;
		  notifyEl.elementType.ElTypeNum = pEl->ElTypeNumber;
		  notifyEl.elementType.ElSSchema = (SSchema) (pEl->ElStructSchema);
		  notifyEl.position = 0;
		  pDescRoot = NULL;
		  if (!CallEventType ((NotifyEvent *) (&notifyEl), TRUE))
		     pDescRoot = CreateDescendant (pEl->ElTypeNumber,
					  pEl->ElStructSchema, pDoc, &pDesc,
				    pEl->ElAssocNum, pSavedEl->ElTypeNumber,
						   pSavedEl->ElStructSchema);
		  if (pDescRoot != NULL)
		    {
		       /* on a cree une descendance */
		       ok = TRUE;
		       creation = TRUE;
		    }
	       }
	     if (!ok && sameType)
		/* remplacer l'element ou on colle par l'element de la
		   sauvegarde sauf si l'element ou l'on colle est la racine */
		if (pEl->ElParent != NULL)
		  {
		     ok = TRUE;
		     replace = TRUE;
		  }
	  }
     }
   if (ok && pSavedEl != NULL)
      /* le 1er element a coller a un type convenable */
     {
	if (creation)
	   /* on a cree' des elements intermediaires */
	  {
	     /* chaine temporairement les elements crees pour que CopyTree
	        copie correctement les attributs */
	     pDescRoot->ElParent = pEl;
	     /* pParent : futur pere de la copie des elements sauvegarde's */
	     pParent = pDesc->ElParent;
	     /* supprime les elements crees au dernier niveau : ils vont etre
	        remplaces par la copie des elements sauvegarde's */
	     pSibling = pDesc->ElPrevious;
	     while (pSibling != NULL)
	       {
		  pSib = pSibling;
		  pSibling = pSibling->ElPrevious;
		  DeleteElement (&pSib, pDoc);
	       }
	     pSibling = pDesc->ElNext;
	     while (pSibling != NULL)
	       {
		  pSib = pSibling;
		  pSibling = pSibling->ElNext;
		  DeleteElement (&pSib, pDoc);
	       }
	     DeleteElement (&pDesc, pDoc);
	  }
	else
	  {
	     pDescRoot = NULL;
	     pParent = pEl;
	  }
	/* cree une copie du 1er element a coller. Si l'element est
	   reference', la copie devient l'element reference'. Ne copie les
	   attributs que s'ils sont definis dans les schemas de structure des
	   elements englobants du document d'arrivee. */
	pSS = pParent->ElStructSchema;
	notifyVal.event = TteElemPaste;
	notifyVal.document = doc;
	notifyVal.element = (Element) pEl;
	notifyVal.target = (Element) pSavedEl;
	notifyVal.value = 0;
	if (!CallEventType ((NotifyEvent *) (&notifyVal), TRUE))
	   pCopy = CopyTree (pSavedEl, DocOfSavedElements, pEl->ElAssocNum,
			     pSS, pDoc, pParent, TRUE, TRUE);
	if (pCopy == NULL || (creation && pDescRoot == NULL))
	  {
	     if (pDescRoot != NULL)
		DeleteElement (&pDescRoot, pDoc);
	  }
	else
	  {
	     if (creation)
	       {
		  InsertFirstChild (pParent, pCopy);
		  pPastedEl = pDescRoot;
	       }
	     else
		pPastedEl = pCopy;
	     /* insere dans l'arbre les elements cree's */
	     if (replace)
	       {
		  pSib = pEl->ElPrevious;
		  pSibling = pEl->ElNext;
		  pParent = pEl->ElParent;
		  CancelSelection ();
		  /* detruit les paves de l'element remplace' */
		  DestroyAbsBoxes (pEl, pDoc, FALSE);
		  /* signale au mediateur les paves detruits pour qu'il */
		  /* detruise les boites correspondantes */
		  AbstractImageUpdated (pDoc);
		  /* supprime de l'arbre abstrait l'element a remplacer */
		  DeleteElement (&pEl, pDoc);
		  /* insere dans l'arbre abstrait l'element qui remplace */
		  pEl = pPastedEl;
		  if (pSib != NULL)
		     InsertElementAfter (pSib, pPastedEl);
		  else if (pSibling != NULL)
		     InsertElementBefore (pSibling, pPastedEl);
		  else
		     InsertFirstChild (pParent, pPastedEl);
	       }
	     else if (pEl->ElStructSchema->SsRule[pEl->ElTypeNumber - 1].SrConstruct == CsChoice)
	       {
		  InsertOption (pEl, &pPastedEl, pDoc);
		  if (!creation)
		     pCopy = pPastedEl;
	       }
	     else
		InsertFirstChild (pEl, pPastedEl);
	     /* RemoveElement l'attribut Langue de pCopy si pCopy herite la
	        meme valeur */
	     /* cherche d'abord la valeur heritee */
	     pInheritLang = GetTypedAttrAncestor (pCopy, 1, NULL, &pElAttr);
	     if (pInheritLang != NULL)
	       {
		  /* cherche l'attribut Langue de pCopy */
		  pLangAttr = GetTypedAttrForElem (pCopy, 1, NULL);
		  if (pLangAttr != NULL)
		     /* compare les valeurs de ces 2 attributs */
		     if (TextsEqual (pInheritLang->AeAttrText,
				     pLangAttr->AeAttrText))
			/* attributs egaux, on supprime celui de pCopy */
			{
			RemoveAttribute (pCopy, pLangAttr);
			DeleteAttribute (pCopy, pLangAttr);
			}
	       }
	     /* DeleteElement les elements exclus dans le sous-arbre copie' */
	     RemoveExcludedElem (&pPastedEl, pDoc);
	     *pFirstPastedEl = pPastedEl;
	     if (pPastedEl != NULL)
	       {
		  /* garde le pointeur sur le sous arbre cree */
		  NCreatedElements++;
		  CreatedElement[NCreatedElements - 1] = pPastedEl;
		  /* colle les elements suivants de la sauvegarde */
		  pElem = NULL;
		  if (pSavedEl != NULL)
		     if (pSavedEl->ElNext != NULL)
			PasteBeforeOrAfter (&pElem, pDoc, pCopy, FALSE, FALSE,
					    pSavedEl->ElNext, pFirstFree,
					    &firstChar, 0, NULL);
		  if (pElem == NULL)
		     /* PasteBeforeOrAfter n'a pas ete appele' ou il n'a
		        rien fait */
		    {
		       /* affecte des identificateurs corrects a tous les
		          elements de paire */
		       AssignPairIdentifiers (pPastedEl, pDoc);
		       /* purge les references */
		       CheckReferences (pCopy, pDoc);
		       /* Note les references sortantes creees */
		       RegisterExternalRef (pCopy, pDoc, TRUE);
		       /* envoie l'evenement ElemPaste.Post a l'application */
		       NotifySubTree (TteElemPaste, pDoc, pCopy,
				      IdentDocument (DocOfSavedElements));
		       /* calcule le volume que pourront prendre les paves
		          les paves des elements colles */
		       if (!AssocView (pEl))
			  for (view = 0; view < MAX_VIEW_DOC; view++)
			    {
			       if (pDoc->DocView[view].DvPSchemaView > 0)
				  pDoc->DocViewFreeVolume[view] =
				     pDoc->DocViewVolume[view];
			    }
		       else
			  /* element associe */
		       if (pDoc->DocAssocFrame[pEl->ElAssocNum - 1] > 0)
			  pDoc->DocAssocFreeVolume[pEl->ElAssocNum - 1] =
			     pDoc->DocAssocVolume[pEl->ElAssocNum - 1];
		       /* cree dans toutes les vues les paves des nouveaux
		          elements */
		       CreateNewAbsBoxes (pPastedEl, pDoc, 0);
		       /* applique les regles retardees concernant les paves
		          cree's */
		       ApplDelayedRule (pPastedEl, pDoc);
		       /* refait la presentation des attributs-reference qui
		          pointent sur les elements colle's */
		       UpdateRefAttributes (pCopy, pDoc);
		    }
	       }
	  }
     }
}


/*----------------------------------------------------------------------
   CanCopyOrCut
   verifie qu'on peut copier ou couper la partie de socument selectionne'e
   indique'e en parametre.
   Rend le resultat dans la variable result.
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                CanCopyOrCut (ThotBool * result, PtrDocument pDoc, PtrElement firstSel, PtrElement lastSel, int firstChar, int lastChar)
#else  /* __STDC__ */
void                CanCopyOrCut (result, pDoc, firstSel, lastSel, firstChar, lastChar)
ThotBool           *result;
PtrDocument         pDoc;
PtrElement          firstSel;
PtrElement          lastSel;
int                 firstChar;
int                 lastChar;

#endif /* __STDC__ */

{
   PtrElement          pParent, pEl;
   int                 elemType, elemTypeId;
   ThotBool            pageBreak, pairedElem;

   *result = TRUE;
   if (firstSel != lastSel)
      if (firstSel->ElParent != lastSel->ElParent)
	 *result = FALSE;
      else
	{
	   pParent = firstSel->ElParent;
	   elemType = pParent->ElTypeNumber;
	   elemTypeId = GetTypeNumIdentity (elemType, pParent->ElStructSchema);
	   *result = (pParent->ElStructSchema->SsRule[elemTypeId - 1].SrConstruct == CsList ||
		      elemTypeId == CharString + 1);
	}
   if (!*result)
      TtaDisplaySimpleMessage (INFO, LIB, TMSG_COPYING_DIFFERENT_COMPONENTS_IMP);
   else
     {
	/* verifie que les elements selectionnes contiennent autre chose */
	/* que des elements de paires ou des pages */
	*result = FALSE;
	pageBreak = FALSE;
	pairedElem = FALSE;
	/* examine les elements selectionne's */
	pEl = firstSel;
	while (pEl != NULL && !*result)
	  {
	     if (pEl->ElTerminal && pEl->ElLeafType == LtPairedElem)
		/* c'est un element de paire */
		pairedElem = TRUE;
	     else if (pEl->ElTerminal && pEl->ElLeafType == LtPageColBreak)
		/* c'est un saut de page */
		pageBreak = TRUE;
	     else
		/* ce n'est ni un element de paire ni un saut de page */
		*result = TRUE;
	     /* passe a l'element selectionne' suivant */
	     pEl = NextInSelection (pEl, lastSel);
	  }
	if (!*result)
	  {
	     if (pageBreak)
		TtaDisplaySimpleMessage (INFO, LIB, TMSG_COPYING_PAGE_BRK_IMP);
	     if (pairedElem)
		TtaDisplaySimpleMessage (INFO, LIB, TMSG_DON_T_COPY_PAIRED_EL);
	  }
     }
}

/*----------------------------------------------------------------------
   StructPasteCommand
   traite la commande PASTE en mode structure'
  ----------------------------------------------------------------------*/
void                StructPasteCommand ()
{
   ThotBool            ok;

   if (FirstSavedElement == NULL)
      TtaDisplaySimpleMessage (INFO, LIB, TMSG_NOTHING_TO_PASTE);
   else
     {
	if (ThotLocalActions[T_insertpaste] != NULL)
	   (*ThotLocalActions[T_insertpaste]) (FALSE, TRUE, TEXT('L'), &ok);
	else
	   ok = FALSE;
	if (ok)
	   /* on a effectivement colle' le contenu du buffer, il faudra */
	   /* changer les labels lors du prochain Coller */
	   ChangeLabel = TRUE;
	else if (FirstSavedElement->PeElement != NULL)
	   TtaDisplayMessage (INFO, TtaGetMessage (LIB, TMSG_PASTING_EL_IMP),
			      FirstSavedElement->PeElement->ElStructSchema->SsRule[FirstSavedElement->PeElement->ElTypeNumber - 1].SrName);
     }
}


/*----------------------------------------------------------------------
   StructReturnKey
   L'utilisateur a frappe' la touche "Return". Traitement en mode structure'
  ----------------------------------------------------------------------*/
void                StructReturnKey ()
{
  PtrDocument         pDoc;
  PtrElement          firstSel, lastSel, pElReplicate, pSibling, pListEl,
    pEl, pNewEl, pClose, pNewAncest;
  NotifyElement       notifyEl;
  Document            doc;
  int                 firstChar, lastChar, NSiblings;
  ThotBool            ok, histSeq;
   
  ok = FALSE;
  histSeq = FALSE;
  if (!GetCurrentSelection (&pDoc, &firstSel, &lastSel, &firstChar, &lastChar))
    TtaDisplaySimpleMessage (INFO, LIB, TMSG_SEL_EL);
  else if (pDoc->DocReadOnly)
    TtaDisplaySimpleMessage (INFO, LIB, TMSG_RO_DOC_FORBIDDEN);
  else
    {
      /* on essaie d'abord de diviser un element */
      if (CanSplitElement (firstSel, firstChar, TRUE, &pNewAncest, &pEl,
				     &pElReplicate))
        {
	  OpenHistorySequence (pDoc, firstSel,
			       lastSel, firstChar, lastChar - 1);
	  histSeq = TRUE;
	  AddEditOpInHistory (firstSel, pDoc, TRUE, TRUE);
  	  
	  AddEditOpInHistory (pElReplicate, pDoc, TRUE, TRUE);
	  ok = BreakElement (NULL, NULL, 0, TRUE, TRUE);
	  if (ok)
	    {	
	      AddEditOpInHistory (pElReplicate->ElNext, pDoc, FALSE, TRUE);
	    }
	  else
	    {
	      CancelLastEditFromHistory (pDoc);
	    }
	}
      if (!ok)
	{
	  /* ca n'a pas marche', on va essayer de creer un element de meme type */
	  /* que le dernier element selectionne' */
	  pElReplicate = NULL;
	  doc = IdentDocument (pDoc);
	  if (lastSel->ElTerminal &&
	      ((lastSel->ElLeafType == LtText &&
		(lastChar > 0 && lastChar <= lastSel->ElTextLength)) ||
	       lastSel->ElLeafType == LtPageColBreak))
	    /* on est au milieur d'une feuille de texte ou sur un saut de page
	       on abandonne. */
	    pListEl = NULL;
	  else
	    {
	      /* on recherche l'ascendant Liste qui permettrait de creer un
		 nouvel element de meme type */
	      pListEl = AncestorList (lastSel);
	      if (pListEl != NULL)
		if (lastSel->ElTerminal && lastSel->ElLeafType == LtText &&
		    pListEl == lastSel->ElParent && lastSel->ElNext == NULL)
		  /* on est a la fin d'une feuille de texte, fille de
		     l'element liste trouve'. On cherche un element liste plus
		     haut dans l'arbre */
		  pListEl = AncestorList (pListEl);
	      if (pListEl != NULL && !CanChangeNumberOfElem (pListEl, 1))
		/* la liste a atteint son nombre max. d'elements. Abandon */
		pListEl = NULL;
	    }
	  if (pListEl != NULL)
	    {
	      pEl = lastSel;
	      do
		if (TypeHasException (ExcNoCreate, pEl->ElTypeNumber,
				      pEl->ElStructSchema))
		  /* abandon */
		  pListEl = NULL;
		else
		  {
		    pElReplicate = pEl;
		    pEl = pEl->ElParent;
		  }
	      while (pListEl != NULL && pEl != pListEl);
	      if (pListEl != NULL)
		{
		  /* demande a l'application si on peut creer ce type d'element */
		  notifyEl.event = TteElemNew;
		  notifyEl.document = doc;
		  notifyEl.element = (Element) (pElReplicate->ElParent);
		  notifyEl.elementType.ElTypeNum = pElReplicate->ElTypeNumber;
		  notifyEl.elementType.ElSSchema = (SSchema) (pElReplicate->ElStructSchema);
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
	    }
	  if (pListEl != NULL)
	    {
	      /* On determine l'element apres lequel  */
	      /* l'insertion aura effectivement lieu. */
	      pEl = lastSel;
	      while (pEl->ElParent != pListEl)
		pEl = pEl->ElParent;
	      ok = !CannotInsertNearElement (pEl,
					     FALSE); /* After element */
	      if (ok)
		{
		  TtaClearViewSelections ();
		  /* cree un element du meme type que le dernier element
		     de la selection */
		  pNewEl = NewSubtree (lastSel->ElTypeNumber,
				       lastSel->ElStructSchema, pDoc,
				       lastSel->ElAssocNum, TRUE, TRUE, TRUE, TRUE);
		  pEl = lastSel;
		  /* cree des ascendants de meme type */
		  while (pEl->ElParent != pListEl)
		    {
		      pEl = pEl->ElParent;
		      pNewAncest = ReplicateElement (pEl, pDoc);
		      InsertFirstChild (pNewAncest, pNewEl);
		      pNewEl = pNewAncest;
		    }
		  /* Insere les elements cree's */
		  pClose = pEl->ElNext;
		  FwdSkipPageBreak (&pClose);
		  InsertElementAfter (pEl, pNewEl);
		  if (pClose == NULL)
		    /* l'element pEl n'est plus le dernier fils de son pere */
		    ChangeFirstLast (pEl, pDoc, 0, TRUE);
		  if (!histSeq)
		    {
		      OpenHistorySequence (pDoc, firstSel,
					   lastSel, firstChar, lastChar - 1);
		      histSeq = TRUE;
		    }
		  AddEditOpInHistory (pNewEl, pDoc, FALSE, TRUE);
		  /* traite les exclusions des elements crees */
		  RemoveExcludedElem (&pNewEl, pDoc);
		  /* traite les attributs requis des elements crees */
		  AttachMandatoryAttributes (pNewEl, pDoc);
		  if (pDoc->DocSSchema != NULL)
		    /* le document n'a pas ete ferme' entre temps */
		    {
		      /* traitement des exceptions */
		      CreationExceptions (pNewEl, pDoc);
		      /* mise a jour des images abstraites */
		      CreateAllAbsBoxesOfEl (pNewEl, pDoc);
		      /* cree les paves du nouvel element et met a jour ses
			 voisins */
		      AbstractImageUpdated (pDoc);
		      /* affiche les nouveaux elements */
		      RedisplayDocViews (pDoc);
		      /* si on est dans un element copie' par inclusion, on
			 met a jour les copies de cet element. */
		      RedisplayCopies (pEl, pDoc, TRUE);
		      pSibling = NextElement (pNewEl);
		      UpdateNumbers (pSibling, pEl, pDoc, TRUE);
		      /* marque le document est modifie' */
		      SetDocumentModified (pDoc, TRUE, 20);
		      /* envoie un evenement ElemNew.Post a l'application */
		      NotifySubTree (TteElemNew, pDoc, pNewEl, 0);
		      /* place la selection */
		      SelectElementWithEvent (pDoc, FirstLeaf (pNewEl), TRUE,
					      TRUE);
		    }
		}
	    }
	}
      if (histSeq)
	CloseHistorySequence (pDoc);
    }
}

/*----------------------------------------------------------------------
   StructureLoadResources
   connects specific editing and selection functions.
  ----------------------------------------------------------------------*/

void                StructSelectLoadResources ()
{
   if (ThotLocalActions[T_selecbox] == NULL)
     {
	/* connecte les action d'edition structuree */
	TteConnectAction (T_selecbox, (Proc) GetClickedStructBox);
	TteConnectAction (T_switchsel, (Proc) SwitchSelection);
	TteConnectAction (T_checksel, (Proc) CheckSelectedElement);
	TteConnectAction (T_resetsel, (Proc) ResetSelection);
	TteConnectAction (T_selstring, (Proc) SelectString);
	TteConnectAction (T_extendsel, (Proc) ExtendSelection);
	TteConnectAction (T_showbox, (Proc) ShowBox);
	TteConnectAction (T_pastesibling, (Proc) PasteBeforeOrAfter);
	TteConnectAction (T_pastewithin, (Proc) PasteWithin);
	TteConnectAction (T_cancopyorcut, (Proc) CanCopyOrCut);
	TteConnectAction (T_cmdpaste, (Proc) StructPasteCommand);
	TteConnectAction (T_enter, (Proc) StructReturnKey);
	MenuActionList[CMD_CreateElement].Call_Action = (Proc) StructReturnKey;
	MenuActionList[CMD_CreateElement].User_Action = (UserProc) NULL;
	InitSelection ();
     }
}
