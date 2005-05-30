/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2005
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * References search
 *
 * Author: V. Quint (INRIA)
 *
 */
#include "thot_gui.h"
#include "thot_sys.h"
#include "libmsg.h"

#include "constmedia.h"
#include "typemedia.h"
#include "fileaccess.h"
#include "appaction.h"
#include "appstruct.h"
#include "message.h"
#include "typecorr.h"
#include "appdialogue.h"
#include "labelAllocator.h"

#define THOT_EXPORT extern
#include "platform_tv.h"
#include "edit_tv.h"
#include "modif_tv.h"
#include "appdialogue_tv.h"

#include "tree_f.h"
#include "attrpresent_f.h"
#include "search_f.h"
#include "searchmenu_f.h"
#include "structcreation_f.h"
#include "createabsbox_f.h"
#include "scroll_f.h"
#include "views_f.h"
#include "callback_f.h"
#include "exceptions_f.h"
#include "absboxes_f.h"
#include "memory_f.h"
#include "structmodif_f.h"
#include "changeabsbox_f.h"
#include "labelalloc_f.h"

#include "references_f.h"
#include "externalref_f.h"
#include "schemas_f.h"
#include "boxselection_f.h"
#include "structselect_f.h"
#include "fileaccess_f.h"
#include "structschema_f.h"
#include "docs_f.h"
#include "readpivot_f.h"
#include "searchref_f.h"

#define MAX_ITEM_MENU_REF 10
#ifdef IV
/*----------------------------------------------------------------------
   FindReference cherche une reference a` l'element selectionne'.       
   A l'appel:                                                   
   - pPrevRef: pointeur sur la derniere reference trouvee  
   ou NULL si on cherche la premiere reference (dans ce  
   cas, pExtDoc doit aussi etre NULL).                   
   - pReferredEl: l'element dont on cherche les references,
   NULL si on cherche la premiere reference.             
   - pDoc: le document auquel appartient         
   l'element dont on cherche les references, NULL si on  
   cherche la premiere reference.                        
   Au retour:                                                   
   - pPrevRef: pointeur sur la reference trouvee ou NULL   
   si on n'a pas trouve' de reference.                   
   - pReferredEl: l'element dont on cherche les references.
  ----------------------------------------------------------------------*/
void FindReference (PtrReference *pPrevRef, PtrDocument pDoc,
		    PtrElement *pReferredEl)
{
  PtrElement          firstSel;
  PtrElement          lastSel;
  PtrElement          pEl;
  PtrDocument         pSelDoc;
  PtrElement          pAncest;
  int                 firstChar;
  int                 lastChar;
  ThotBool            ok;
  ThotBool            found;

  ok = TRUE;
  if (*pPrevRef)
    /* on a deja une reference courante */
    *pPrevRef = NextReferenceToEl (*pReferredEl, pDoc, *pPrevRef);
  else
    /* pas de reference courante */
    {
      /* prend la selection courante */
      ok = GetCurrentSelection (&pSelDoc, &firstSel, &lastSel, &firstChar,
				&lastChar);
      if (ok)
	/* cherche le premier element reference' qui englobe la */
	/* selection courante */
	{
	  pEl = firstSel;
	  found = FALSE;
	  do
	    {
	      if (pEl->ElReferredDescr &&
		  /* l'element a un descripteur d'element reference' */
		  pEl->ElReferredDescr->ReFirstReference)
		/* l'element est effectivement reference' */
		found = TRUE;
	      if (!found)
		/* l'element n'est pas reference', on passe au pere */
		pEl = pEl->ElParent;
	    }
	  while (!found && pEl);
	  if (found)
	    /* on a trouve' un ascendant reference' */
	    {
	      /* conserve un pointeur sur l'element dont on cherche les */
	      /* references ainsi que sur son document */
	      *pReferredEl = pEl;
	      /* cherche la premiere reference a cet element */
	      *pPrevRef = NextReferenceToEl (*pReferredEl, pDoc, *pPrevRef);
	    }
	}
    }
  if (*pPrevRef != NULL)
    /* on a trouve' une reference */
    {
      ok = TRUE;
      /* on ignore les references qui sont a l'interieur d'une inclusion */
      if ((*pPrevRef)->RdElement != NULL)
	{
	  pAncest = (*pPrevRef)->RdElement->ElParent;
	  while (pAncest->ElParent != NULL && pAncest->ElSource == NULL)
	    pAncest = pAncest->ElParent;
	  if (pAncest->ElSource != NULL)
	    /* on est dans une inclusion */
	    ok = FALSE;
	}
      /* on ignore les references dans les partie cachees */
      if (ok)
	if (ElementIsHidden ((*pPrevRef)->RdElement))
	  /* on est dans une partie cachee */
	  ok = FALSE;
      /* on ignore les references qui sont dans le tampon de Copier-Couper */
      if (ok)
	if (IsASavedElement ((*pPrevRef)->RdElement))
	  ok = FALSE;
      if (!ok)
	/* cherche la reference suivante au meme element */
	FindReference (pPrevRef, pDoc, pReferredEl);
      else
	/* selectionne la reference trouvee */
	SelectElementWithEvent (pDoc, (*pPrevRef)->RdElement, FALSE, FALSE);
    }
}
#endif

/*----------------------------------------------------------------------
   FindReferredEl cherche l'element qui est reference' par la         
   reference selectionnee ou par un attribut reference du  
   premier element selectionne'.                           
  ----------------------------------------------------------------------*/
void FindReferredEl ()
{
   PtrElement          firstSel;
   PtrElement          lastSel;
   int                 firstChar;
   int                 lastChar;
   PtrDocument         pSelDoc;
   PtrElement          pEl;
   PtrAttribute        pAttr;
   PtrReference        pRef;
   int                 nMenuItems;
   char                menuBuf[MAX_TXT_LEN];
   int                 menuBufLen;
   PtrReference        pRefTable[MAX_ITEM_MENU_REF];

   /* y-a-t'il une selection au niveau editeur ou mediateur ? */
   if (GetCurrentSelection (&pSelDoc, &firstSel, &lastSel, &firstChar, &lastChar))
     {
	pEl = NULL;
	pRef = NULL;
	nMenuItems = 0;
	menuBufLen = 0;
	if (firstSel->ElTerminal && firstSel->ElLeafType == LtReference)
	   /* la selection est un element reference */
	   if (firstSel->ElReference != NULL && nMenuItems < MAX_ITEM_MENU_REF &&
	       menuBufLen + MAX_NAME_LENGTH < MAX_TXT_LEN)
	     {
		pRefTable[nMenuItems] = firstSel->ElReference;
		nMenuItems++;
		strcpy (menuBuf + menuBufLen,
			firstSel->ElStructSchema->SsRule->SrElem[firstSel->ElTypeNumber - 1]->SrName);
		menuBufLen += strlen (menuBuf) + 1;
	     }
	if (firstSel->ElSource != NULL)
	   /* la selection est un element inclus */
	   if (firstSel->ElSource != NULL && nMenuItems < MAX_ITEM_MENU_REF &&
	       menuBufLen + MAX_NAME_LENGTH < MAX_TXT_LEN)
	     {
		pRefTable[nMenuItems] = firstSel->ElSource;
		nMenuItems++;
	     }
	/* cherche si l'element a un attribut reference */
	pAttr = firstSel->ElFirstAttr;
	while (pAttr != NULL)
	  {
	     if (pAttr->AeAttrType == AtReferenceAttr)
		/* c'est un attribut reference */
		if (AttrHasException (ExcActiveRef, pAttr->AeAttrNum, pAttr->AeAttrSSchema))
		   /* cet attribut a l'exception ActiveRef, on le prend */
		   if (pAttr->AeAttrReference != NULL)
		     {
			pRefTable[nMenuItems] = pAttr->AeAttrReference;
			nMenuItems++;
			strcpy (menuBuf + menuBufLen,
				pAttr->AeAttrSSchema->SsAttribute->TtAttr[pAttr->AeAttrNum - 1]->AttrName);
			menuBufLen += strlen (pAttr->AeAttrSSchema->SsAttribute->TtAttr[pAttr->AeAttrNum - 1]->AttrName) + 1;
		     }
	     pAttr = pAttr->AeNext;
	  }
	if (nMenuItems == 0)
	   pRef = NULL;
	else if (nMenuItems == 1)
	   pRef = pRefTable[0];

	if (pEl)
	  {
	     SelectElementWithEvent (pSelDoc, pEl, TRUE, TRUE);
	     /* dans le cas ou c'est un element d'une paire de marques, on */
	     /* selectionne l'intervalle compris entre ces marques. */
	     SelectPairInterval ();
	  }
     }
}

/*----------------------------------------------------------------------
   CheckReferences        On vient de coller le sous-arbre de racine	
   pRoot dans le document pDoc. Verifie la coherence des elements	
   reference's et des references presents dans ce sous-arbre.      
  ----------------------------------------------------------------------*/
void CheckReferences (PtrElement pRoot, PtrDocument pDoc)
{
   PtrElement          pEl, pElRef, pSource;
   PtrReference        pRef, pNextRef;
   PtrAttribute        pAttr, pNextAttr;
   PtrElement          pElemRef;
   LabelString         oldLabel;
   PtrReference        pRefClipboard;
   PtrDocument         pDocRef;
   int                 l;
   ThotBool            delAttr;

   /* on se souvient du label de l'original */
   strncpy (oldLabel, pRoot->ElLabel, MAX_LABEL_LEN);
   if (ChangeLabel || pDoc != DocOfSavedElements)
     {
	/* on affecte un nouveau label a l'element */
	l = NewLabel (pDoc);
	ConvertIntToLabel (l, pRoot->ElLabel);
     }
   if (pRoot->ElReferredDescr != NULL)
      /* l'original de l'element colle' est reference' */
     {
       /* l'element original */
       pSource = pRoot->ElReferredDescr->ReReferredElem;
       pRoot->ElReferredDescr = NULL;
       /* l'element colle' n'est pas reference', il prend le label de son
	  element original (celui qui est dans le tampon Couper-Copier)
	  si la commande precedente etait Couper et s'il ne change pas de
	  document. Dans les autres cas (commande precedente Copier ou
	  changement de document), il prend un nouveau label. */

       /* alloue a l'element un descripteur d'element reference' */
       pRoot->ElReferredDescr = NewReferredElDescr (pDoc);
       pRoot->ElReferredDescr->ReReferredElem = pRoot;
       if (!ChangeLabel && pSource != NULL && pDoc == DocOfSavedElements)
	 /* l'element prend le meme label que l'element original */
	 strncpy (pRoot->ElLabel, pSource->ElLabel, MAX_LABEL_LEN);

       /* on cherche toutes les references a l'element original et on les */
       /* fait pointer sur l'element colle'. */
       /* cherche d'abord la premiere reference */
       pDocRef = DocOfSavedElements;
       pRef = NextReferenceToEl (pSource, DocOfSavedElements, NULL);
       while (pRef)
	  {
	     /* cherche la reference suivante a l'original avant de modifier */
	     /* la reference courante */
	     pNextRef = NextReferenceToEl (pSource, DocOfSavedElements, pRef);
	     /* traite la reference courante */
	     /* si elle est dans le tampon, on n'y touche pas : sa copie dans
	        le document ou on colle a deja ete traitee ou sera traitee
		dans cette boucle */
	     if (!IsASavedElement (pRef->RdElement))
		/* on fait pointer la reference sur l'element colle' */
	       {
		  if (IsWithinANewElement (pRef->RdElement))
		     pDocRef = pDoc;
		  if (pRef->RdAttribute)
		     pElemRef = NULL;
		  else
		     pElemRef = pRef->RdElement;
		  SetReference (pElemRef, pRef->RdAttribute, pRoot, pDocRef,
				pDoc, FALSE, FALSE);
	       }
	     pRef = pNextRef;	/* passe a la reference suivante */
	     pDocRef = DocOfSavedElements;
	  }
     }
   /* Traite les attributs de type reference porte's par l'element */
   pAttr = pRoot->ElFirstAttr;
   while (pAttr != NULL)
      /* examine tous les attributs de l'element */
     {
     	pNextAttr = pAttr->AeNext;
	if (pAttr->AeAttrType == AtReferenceAttr)
	   /* c'est un attribut de type reference */
	   /* cherche l'element reference', pElRef */
	  {
	     pElRef = NULL;
	     delAttr = FALSE;
	     if (pAttr->AeAttrReference != NULL)
		 if (pAttr->AeAttrReference->RdReferred != NULL)
		   {
		     pElRef = pAttr->AeAttrReference->RdReferred->ReReferredElem;
		     /* si l'element reference' est aussi colle', */
		     /* on ne fait rien: ce cas est traite' plus haut */
		     if (pElRef != NULL)
		       if (!IsASavedElement (pElRef))
			 if (!IsWithinANewElement (pElRef))
			   {
			     if (DocOfSavedElements != pDoc)
			       /* reference et objet reference' sont */
			       /* dans des documents differents, on */
			       /* supprime l'attribut, sauf dans le */
			       /* cas particulier des tableaux. */
			       {
				 RemoveAttribute (pRoot, pAttr);
				 delAttr = TRUE;
			       }
			   }
		     if (delAttr)
			DeleteAttribute (pRoot, pAttr);
		   }  
	  }
	pAttr = pNextAttr;
     }
   /* Lorsqu'on fait un copier/coller d'un element LtReference, */
   /* il faut chainer les references entre element ancien et */
   /* element nouveau : la reference de la copie qui est dans */
   /* le tampon est dechainee entre les deux elements. */
   /* pRoot est le nouvel element cree : son descripteur */
   /* a ete chaine devant le descripteur de la copie qui est */
   /* dans le tampon, qui est devant celui de l'element copie */
   if (pRoot->ElTerminal && pRoot->ElLeafType == LtReference)
     {
	/* c'est un element reference */
	pRef = pRoot->ElReference;
	if (pRef != NULL)	/*  on cherche le suivant */
	  {
	     pRefClipboard = pRef->RdNext;
	     if (pRefClipboard != NULL
		 && IsASavedElement (pRefClipboard->RdElement))
		/* il faut retirer pRefClipboard du chainage */
	       {
		  pRef->RdNext = pRefClipboard->RdNext;
		  if (pRefClipboard->RdNext != NULL)
		     pRefClipboard->RdNext->RdPrevious = pRef;
	       }
	  }
     }

   if (!pRoot->ElTerminal && pRoot->ElSource == NULL)
      /* ce n'est ni une inclusion ni un terminal, on traite tous les fils */
     {
	pEl = pRoot->ElFirstChild;
	while (pEl != NULL)
	  {
	     CheckReferences (pEl, pDoc);
	     pEl = pEl->ElNext;
	  }
     }
}
