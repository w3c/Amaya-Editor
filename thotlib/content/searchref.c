/*
 *
 *  (c) COPYRIGHT INRIA, Grif, 1996.
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
 * References search
 *
 * Author: V. Quint (INRIA)
 *
 */

#include "thot_sys.h"
#include "libmsg.h"

#include "constmedia.h"
#include "typemedia.h"
#include "fileaccess.h"
#include "appaction.h"
#include "app.h"
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
#include "viewcommands_f.h"
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

#define MAX_ITEM_MENU_REF 10

#ifndef _WIN_PRINT
/*----------------------------------------------------------------------
   NextReferenceToEl retourne la prochaine reference qui designe	
   l'element pEl.                                             
   - pDoc est le document auquel appartient pEl.              
   - processNotLoaded indique si on prend en compte les 	
   documents referencant non charge's (TRUE) ou si au    	
   contraire on les ignore (FALSE).                      	
   - pPrevRef contient la reference courante a l'appel.    	
   Si pPrevRef est NULL et *pExtDoc est NULL, la fonction   
   retourne la premiere reference a` l'element pEl, sinon 	
   elle retourne la reference qui suit celle qui est     	
   pointee par pPrevRef.                                 	
   - pDocRef contient au retour un pointeur sur le         	
   contexte du document auquel appartient la reference   	
   retournee. Seulement si la valeur de retour n'est pas 	
   NULL.                                                 	
   - pExtDoc est le document externe precedemment traite' (si 
   nextExtDoc est TRUE) ou celui qu'on veut traiter   	
   (si nextExtDoc est FALSE). pExtDoc doit etre NULL si     
   nextExtDoc est TRUE et qu'on n'a pas encore traite'	
   de references externes.                               	
   Au retour, pExtDoc vaut NULL si la reference retournee   
   appartient au meme document que l'element pEl; sinon  	
   pExtDoc est le document externe auquel appartient la     
   reference trouvee. pExtDoc ne doit pas etre modifie'     
   entre les appels successifs de la fonction.           	
   - nextExtDoc indique si on passe au document         	
   referencant suivant celui decrit par pExtDoc (TRUE) ou si
   on traite le document decrit par pExtDoc (FALSE).        
   Retourne un pointeur sur la premiere reference trouvee. 	
   Si la valeur de retour est NULL et                      	
   si pExtDoc est NULL : on n'a rien trouve'.              
   si pExtDoc n'est pas NULL : il y a des references a`    
   l'element pEl dans le document designe' par pExtDoc
   mais ce document n'est pas charge' (cela ne se  	
   produit que si processNotLoaded est TRUE).       	
  ----------------------------------------------------------------------*/
#ifdef __STDC__
PtrReference        NextReferenceToEl (PtrElement pEl, PtrDocument pDoc, boolean processNotLoaded, PtrReference pPrevRef, PtrDocument * pDocRef, PtrExternalDoc * pExtDoc, boolean nextExtDoc)
#else  /* __STDC__ */
PtrReference        NextReferenceToEl (pEl, pDoc, processNotLoaded, pPrevRef, pDocRef, pExtDoc, nextExtDoc)
PtrElement          pEl;
PtrDocument         pDoc;
boolean             processNotLoaded;
PtrReference        pPrevRef;
PtrDocument        *pDocRef;
PtrExternalDoc     *pExtDoc;
boolean             nextExtDoc;

#endif /* __STDC__ */
{
   PtrReference        pRef;

   if (pPrevRef != NULL)
      pRef = pPrevRef->RdNext;
   else
     {
	pRef = NULL;
	if (*pExtDoc == NULL)
	   /* premier appel de la fonction */
	   if (pEl != NULL)
	      if (pEl->ElReferredDescr != NULL)
		{
		   pRef = pEl->ElReferredDescr->ReFirstReference;
		   *pDocRef = pDoc;
		}
     }
   if (pRef == NULL)
      /* c'etait la derniere reference dans ce document, cherche dans */
      /* un autre document la 1ere reference au meme element */
      pRef = SearchExternalReferenceToElem (pEl, pDoc, processNotLoaded,
					    pDocRef, pExtDoc, nextExtDoc);
   return pRef;
}
#endif /* _WIN_PRINT */

/*----------------------------------------------------------------------
   FindReference cherche une reference a` l'element selectionne'.       
   A l'appel:                                                   
   - pPrevRef: pointeur sur la derniere reference trouvee  
   ou NULL si on cherche la premiere reference (dans ce  
   cas, pExtDoc doit aussi etre NULL).                   
   - pReferredEl: l'element dont on cherche les references,
   NULL si on cherche la premiere reference.             
   - pDocReferredEl: le document auquel appartient         
   l'element dont on cherche les references, NULL si on  
   cherche la premiere reference.                        
   - pExtDoc: pointeur sur le descripteur de document      
   externe contenant la reference pPrevRef, NULL si on   
   cherche la premiere reference.                        
   - nextExtDoc indique si on cherche une reference        
   dans le document externe decrit par pExtDoc           
   (FALSE) ou dans le document externe suivant.          
   Au retour:                                                   
   - pPrevRef: pointeur sur la reference trouvee ou NULL   
   si on n'a pas trouve' de reference.                   
   - pDocPrevRef: document auquel appartient la            
   reference trouvee.                                    
   - pReferredEl: l'element dont on cherche les references.
   - pDocReferredEl: le document auquel appartient         
   l'element dont on cherche les references.             
   - pExtDoc: document externe dans lequel on a trouve'    
   une reference. Peut etre non NULL meme si pPrevRef    
   est NULL, dans le cas ou ce document externe n'est    
   pas charge':                                          
   Si pPrevRef est NULL et                                 
   si pExtDoc est NULL : on n'a rien trouve'.          
   si pExtDoc n'est pas NULL : il y a des              
   references dans le document designe' par        
   pExtDoc, mais ce document n'est pas charge'     
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                FindReference (PtrReference * pPrevRef, PtrDocument * pDocPrevRef, PtrElement * pReferredEl, PtrDocument * pDocReferredEl, PtrExternalDoc * pExtDoc, boolean nextExtDoc)
#else  /* __STDC__ */
void                FindReference (pPrevRef, pDocPrevRef, pReferredEl, pDocReferredEl, pExtDoc, nextExtDoc)
PtrReference       *pPrevRef;
PtrDocument        *pDocPrevRef;
PtrElement         *pReferredEl;
PtrDocument        *pDocReferredEl;
PtrExternalDoc     *pExtDoc;
boolean             nextExtDoc;

#endif /* __STDC__ */
{
   PtrElement          firstSel;
   PtrElement          lastSel;
   PtrElement          pEl;
   PtrDocument         pSelDoc;
   int                 firstChar;
   int                 lastChar;
   PtrElement          pAncest;
   boolean             ok;
   boolean             found;

   ok = TRUE;
   if (*pPrevRef != NULL || *pExtDoc != NULL)
      /* on a deja une reference courante */
      *pPrevRef = NextReferenceToEl (*pReferredEl, *pDocReferredEl, TRUE,
			       *pPrevRef, pDocPrevRef, pExtDoc, nextExtDoc);
   else
      /* pas de reference courante */
     {
	/* prend la selection courante */
	ok = GetCurrentSelection (&pSelDoc, &firstSel, &lastSel, &firstChar, &lastChar);
	if (!ok)
	   /* pas de selection, message 'Selectionnez' */
	   TtaDisplaySimpleMessage (INFO, LIB, TMSG_SEL_EL);
	else
	   /* cherche le premier element reference' qui englobe la */
	   /* selection courante */
	  {
	     pEl = firstSel;
	     found = FALSE;
	     do
	       {
		  if (pEl->ElReferredDescr != NULL)
		     /* l'element a un descripteur d'element reference' */
		     if (pEl->ElReferredDescr->ReFirstReference != NULL ||
			 pEl->ElReferredDescr->ReExtDocRef != NULL)
			/* l'element est effectivement reference' */
			found = TRUE;
		  if (!found)
		     /* l'element n'est pas reference', on passe au pere */
		     pEl = pEl->ElParent;
	       }
	     while (!found && pEl != NULL);
	     if (found)
		/* on a trouve' un ascendant reference' */
	       {
		  /* conserve un pointeur sur l'element dont on cherche les */
		  /* references ainsi que sur son document */
		  *pReferredEl = pEl;
		  *pDocReferredEl = pSelDoc;
		  /* cherche la premiere reference a cet element */
		  *pPrevRef = NextReferenceToEl (*pReferredEl, *pDocReferredEl,
			 TRUE, *pPrevRef, pDocPrevRef, pExtDoc, nextExtDoc);
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
	   FindReference (pPrevRef, pDocPrevRef, pReferredEl, pDocReferredEl,
			  pExtDoc, TRUE);
	else
	   /* selectionne la reference trouvee */
	   SelectElementWithEvent (*pDocPrevRef, (*pPrevRef)->RdElement, FALSE, FALSE);
     }
}


/*----------------------------------------------------------------------
   FindReferredEl cherche l'element qui est reference' par la         
   reference selectionnee ou par un attribut reference du  
   premier element selectionne'.                           
  ----------------------------------------------------------------------*/
void                FindReferredEl ()
{
   PtrElement          firstSel;
   PtrElement          lastSel;
   int                 firstChar;
   int                 lastChar;
   PtrDocument         pSelDoc;
   PtrElement          pEl;
   PtrAttribute        pAttr;
   PtrReference        pRef;
   DocumentIdentifier  docIdent;
   PtrDocument         pDoc;
   int                 nMenuItems;
   char                menuBuf[MAX_TXT_LEN];
   int                 menuBufLen;
   PtrReference        pRefTable[MAX_ITEM_MENU_REF];
   int                 chosenItem;

   /* y-a-t'il une selection au niveau editeur ou mediateur ? */
   if (!GetCurrentSelection (&pSelDoc, &firstSel, &lastSel, &firstChar, &lastChar))
      TtaDisplaySimpleMessage (INFO, LIB, TMSG_SEL_EL);
   else
     {
	pEl = NULL;
	pRef = NULL;
	ClearDocIdent (&docIdent);
	nMenuItems = 0;
	menuBufLen = 0;
	if (firstSel->ElTerminal && firstSel->ElLeafType == LtReference)
	   /* la selection est un element reference */
	   if (firstSel->ElReference != NULL && nMenuItems < MAX_ITEM_MENU_REF &&
	       menuBufLen + MAX_NAME_LENGTH < MAX_TXT_LEN)
	     {
		pRefTable[nMenuItems] = firstSel->ElReference;
		nMenuItems++;
		strcpy (menuBuf + menuBufLen, firstSel->ElStructSchema->SsRule[firstSel->ElTypeNumber - 1].SrName);
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
			strcpy (menuBuf + menuBufLen, pAttr->AeAttrSSchema->SsAttribute[pAttr->AeAttrNum - 1].AttrName);
			menuBufLen += strlen (pAttr->AeAttrSSchema->SsAttribute[pAttr->AeAttrNum - 1].AttrName) + 1;
		     }
	     pAttr = pAttr->AeNext;
	  }
	if (nMenuItems == 0)
	   pRef = NULL;
	else if (nMenuItems == 1)
	   pRef = pRefTable[0];
	else
	  {
	     BuildReferenceMenu (menuBuf, nMenuItems, &chosenItem);
	     pRef = pRefTable[chosenItem];
	  }
	if (pRef != NULL)
	   /* c'est bien une reference qui est selectionnee */
	  {
	     /* l'element reference' est pointe' par pEl */
	     pEl = ReferredElement (pRef, &docIdent, &pDoc);
	     if (pEl == NULL)
		/* il n'y a pas d'element reference' */
		if (DocIdentIsNull (docIdent) || pDoc != NULL)
		   /* ce n'est pas une reference externe ou c'est une reference vide */
		   TtaDisplaySimpleMessage (INFO, LIB, TMSG_EMPTY_REF);
		else
		   /* l'element reference' est dans un autre document qui */
		   /* n'est pas charge' */
		   /* on proposera ce nom comme nom par defaut lorsque */
		   /* l'utilisateur demandera a ouvrir un document */
		  {
		    LoadDocument (&pDoc, docIdent);
		    if (pDoc != NULL)
		      {
			/* annule la selection */
			TtaClearViewSelections ();
			/* le chargement du document a reussi */
			pEl = ReferredElement (pRef, &docIdent, &pDoc);
			/* s'il s'agit d'une inclusion de */
			/* document, applique les regles Transmit */
			ApplyTransmitRules (pRef->RdElement, pSelDoc);
		      }
		  }
	     else if (IsASavedElement (pEl))
	       {
		 /* l'element reference est-il dans le buffer de sauvegarde ? */
		 pEl = NULL;
		 /* message 'CsReference vide' */
		 TtaDisplaySimpleMessage (INFO, LIB, TMSG_EMPTY_REF);
	       }
	  }

	if (pEl != NULL)
	  {
	     if (!DocIdentIsNull (docIdent))
		/* l'element reference' est dans un autre document */
		pSelDoc = GetPtrDocument (docIdent);
	     SelectElementWithEvent (pSelDoc, pEl, TRUE, TRUE);
	     /* dans le cas ou c'est un element d'une paire de marques, on */
	     /* selectionne l'intervalle compris entre ces marques. */
	     SelectPairInterval ();
	  }
     }
}


/*----------------------------------------------------------------------
   RegisterPastedReferredElem					
   	L'element pEl vient d'etre colle'				
   dans le document pDoc et l'original vient d'un document		
   different. S'il possede des elements reference's par d'autres   
   documents, on note dans le contexte du document pDoc que cet    
   element a change' de document. Ce sera utile lorsqu'on sauvera  
   le document pDoc pour mettre a jour les fichiers .REF des       
   documents referencant les elements deplace's.                   
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         RegisterPastedReferredElem (PtrElement pEl, PtrDocument pDoc, LabelString oldLabel)
#else  /* __STDC__ */
static void         RegisterPastedReferredElem (pEl, pDoc, oldLabel)
PtrElement          pEl;
PtrDocument         pDoc;
LabelString         oldLabel;

#endif /* __STDC__ */
{
   PtrChangedReferredEl pChnRef;

   if (pEl->ElReferredDescr != NULL)
      if (pEl->ElReferredDescr->ReExtDocRef != NULL)
	 /* l'element est reference' par d'autres documents */
	{
	   /* acquiert un descripteur et le remplit */
	   GetChangedReferredEl (&pChnRef);
	   strncpy (pChnRef->CrOldLabel, oldLabel, MAX_LABEL_LEN);
	   strncpy (pChnRef->CrNewLabel, pEl->ElLabel, MAX_LABEL_LEN);
	   CopyDocIdent (&pChnRef->CrOldDocument, DocOfSavedElements->DocIdent);
	   CopyDocIdent (&pChnRef->CrNewDocument, pDoc->DocIdent);
	   pChnRef->CrReferringDoc = NULL;
	   /* chaine ce descripteur */
	   pChnRef->CrNext = pDoc->DocChangedReferredEl;
	   pDoc->DocChangedReferredEl = pChnRef;
	   /* copie la liste des documents qui referencent l'element */
	   CopyDescrExtDoc (pEl, pChnRef);
	}
}

/*----------------------------------------------------------------------*/
  /* UpdateReferences      */    
   /* traitement particulier a faire sur les references si */
   /* la copie n'est pas dans le meme document que l'original */
   /* et que l'on a fait un copier/coller d'un element LtReference */
   /* ou d'un element portant un attribut reference, */
/*----------------------------------------------------------------------*/
#ifdef __STDC__
static void                UpdateReferences (PtrElement pRoot, PtrDocument pDoc, PtrReference pRef)
#else  /* __STDC__ */
static void                updateReferences (pRoot, pDoc, pRef)
PtrElement          pRoot;
PtrDocument         pDoc;
PtrReference        pRef;

#endif /* __STDC__ */

{
   PtrElement          pElRef;
   DocumentIdentifier  docIdentRef;
   PtrDocument         pDocRef;
   boolean             sameDoc;

	   if (pRef->RdReferred != NULL)
	     {
		pElRef = ReferredElement (pRef, &docIdentRef, &pDocRef);
		if (pElRef == NULL && DocIdentIsNull (docIdentRef))
		   /* la reference ne designe rien */
		   pRef->RdReferred = NULL;
		else
		  {
		     if (pElRef != NULL)
			if (!IsASavedElement (pElRef))
			   if (IsWithinANewElement (pElRef))
			      /* l'element reference' est aussi colle' */
			      pDocRef = pDoc;
		     if (pRef->RdInternalRef)
			/* l'original etait une reference interne a son document */
			/* d'origine, l'element reference' appartient donc au */
			/* document d'origine */
			pDocRef = DocOfSavedElements;
		     else
			/* l'original etait une reference externe a son document */
			/* d'origine */
		     if (SameDocIdent (pDoc->DocIdent, docIdentRef))
			/* l'element reference' et la reference sont dans le */
			/* meme document */
			pDocRef = pDoc;
		     if (pDocRef != NULL)
			/* le document contenant l'element reference' est charge' */
		       {
			  /* si l'original n'est pas une reference interne, */
			  /* l'element reference' ne peut pas avoir ete copie' en */
			  /* meme temps */
			  if (!pRef->RdInternalRef)
			    {
			     /* lie la reference a l'element designe' par l'original */
		             if (pRef->RdAttribute != NULL)
		               pRoot = NULL;
			     SetReference (pRoot, pRef->RdAttribute, pElRef, pDoc, pDocRef, FALSE, FALSE);
			    }
			  else
			     /* la reference originale etait une reference interne */
			    {
			       sameDoc = FALSE;
			       if (pElRef != NULL)
				  if (!IsASavedElement (pElRef))
				     /* l'element reference' est aussi colle', on ne fait */
				     /* rien : ce cas est traite' plus haut */
				     sameDoc = IsWithinANewElement (pElRef);
			       if (!sameDoc)

				  /* etablit le lien entre la reference copie'e et */
				  /* l'element reference */
				 {
				   if (pRef->RdAttribute != NULL)
				     pRoot = NULL;
				  SetReference (pRoot, pRef->RdAttribute, pElRef, pDoc, pDocRef, FALSE, FALSE);
				 }
			    }
		       }
		  }
	     }
}

/*----------------------------------------------------------------------
   CheckReferences        On vient de coller le sous-arbre de racine	
   pRoot dans le document pDoc. Verifie la coherence des elements	
   reference's et des references presents dans ce sous-arbre.      
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CheckReferences (PtrElement pRoot, PtrDocument pDoc)
#else  /* __STDC__ */
void                CheckReferences (pRoot, pDoc)
PtrElement          pRoot;
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   PtrElement          pEl, pElRef, pSource;
   PtrReference        pRef, pNextRef;
   PtrAttribute        pAttr;
   PtrDocument         pDocRef, pNextDocRef;
   boolean             attrRef;
   PtrExternalDoc      pExtDoc;
   PtrElement          pElemRef;
   int                 l;
   LabelString         oldLabel;
   PtrReference        pRefClipboard;

   /* on se souvient du label de l'original */
   strncpy (oldLabel, pRoot->ElLabel, MAX_LABEL_LEN);
   if (ChangeLabel || pDoc != DocOfSavedElements)
     {
	/* on affecte un nouveau label a l'element */
	l = NewLabel (pDoc);
	ConvertIntToLabel (l, pRoot->ElLabel);
     }
   if (pRoot->ElReferredDescr != NULL)
      /* l'original de l'element colle' est reference' ou possede un label */
     {
	if (pRoot->ElReferredDescr->ReExternalRef)
	   pSource = NULL;
	else
	   pSource = pRoot->ElReferredDescr->ReReferredElem;	/* l'element original */

	pExtDoc = NULL;
	pRoot->ElReferredDescr = NULL;
	/* l'element colle' n'est pas reference', il prend le label de son     */
	/* element original (celui qui est dans le tampon Couper-Copier)       */
	/* si la commande precedente etait Couper et s'il ne change pas de     */
	/* document. Dans les autres cas (commande precedente Copier ou        */
	/* changement de document), il prend un nouveau label.                 */

	/* alloue a l'element un descripteur d'element reference' */
	pRoot->ElReferredDescr = NewReferredElDescr (pDoc);
	pRoot->ElReferredDescr->ReExternalRef = FALSE;
	pRoot->ElReferredDescr->ReReferredElem = pRoot;
	if (!ChangeLabel && pSource != NULL && pDoc == DocOfSavedElements)
	   /* l'element prend le meme label que l'element original */
	   strncpy (pRoot->ElLabel, pSource->ElLabel, MAX_LABEL_LEN);
	if (!ChangeLabel && pSource != NULL)
	  {
	     /* l'element prend les memes descripteurs de documents */
	     /* externes referencants que l'element original */
	     pExtDoc = pSource->ElReferredDescr->ReExtDocRef;
	     while (pExtDoc != NULL)
	       {
		  /* on ne considere pas le document lui-meme comme externe... */
		  if (!SameDocIdent (pExtDoc->EdDocIdent, pDoc->DocIdent))
		     AddDocOfExternalRef (pRoot, pExtDoc->EdDocIdent, pDoc);
		  pExtDoc = pExtDoc->EdNext;
	       }
	  }

	/* on cherche toutes les references a l'element original et on les */
	/* fait pointer sur l'element colle'. */
	/* cherche d'abord la premiere reference */
	pRef = NextReferenceToEl (pSource, DocOfSavedElements, FALSE, NULL, &pDocRef, &pExtDoc, TRUE);
	pNextDocRef = pDocRef;	/* a priori la reference suivante sera dans le */
	/* meme document */
	while (pRef != NULL)
	  {
	     /* cherche la reference suivante a l'original avant de modifier */
	     /* la reference courante */
	     pNextRef = NextReferenceToEl (pSource, DocOfSavedElements, FALSE, pRef, &pNextDocRef, &pExtDoc, TRUE);
	     /* traite la reference courante */
	     /* si elle est dans le tampon, on n'y touche pas : sa copie dans */
	     /* le document ou on colle a deja ete traitee ou sera traitee dans */
	     /* cette boucle */
	     if (!IsASavedElement (pRef->RdElement))
		/* on fait pointer la reference sur l'element colle' */
	       {
		  if (IsWithinANewElement (pRef->RdElement))
		     pDocRef = pDoc;
		  if (pRef->RdAttribute != NULL)
		     pElemRef = NULL;
		  else
		     pElemRef = pRef->RdElement;
		  SetReference (pElemRef, pRef->RdAttribute, pRoot, pDocRef, pDoc, FALSE, FALSE);
		  /* si c'est une reference par attribut, verifie la */
		  /* validite de l'attribut dans le cas des extensions de */
		  /* cellule des tableaux */
		  if (pRef->RdAttribute != NULL)
		     if (ThotLocalActions[T_checkextens] != NULL)
			(*ThotLocalActions[T_checkextens])
			   (pRef->RdAttribute, pRef->RdElement, pRef->RdElement, TRUE);
	       }
	     pRef = pNextRef;	/* passe a la reference suivante */
	     pDocRef = pNextDocRef;
	  }
	if (!ChangeLabel && pDoc != DocOfSavedElements)
	   /* l'element a change' de document, on le note dans le */
	   /* contexte de son nouveau document */
	   RegisterPastedReferredElem (pRoot, pDoc, oldLabel);
     }
   /* Traite les attributs de type reference porte's par l'element */
   pAttr = pRoot->ElFirstAttr;
   while (pAttr != NULL)
      /* examine tous les attributs de l'element */
     {
	if (pAttr->AeAttrType == AtReferenceAttr)
	   /* c'est un attribut de type reference */
	   /* cherche l'element reference', pElRef */
	  {
	     pElRef = NULL;
	     if (pAttr->AeAttrReference != NULL)
		 if (pAttr->AeAttrReference->RdReferred != NULL)
		   {
		     if (!pAttr->AeAttrReference->RdReferred->ReExternalRef)
		       pElRef = pAttr->AeAttrReference->RdReferred->ReReferredElem;
			/* si l'element reference' est aussi colle', */
		     /* on ne fait rien: ce cas est traite' plus haut */
		     if (pElRef != NULL)
		       if (!IsASavedElement (pElRef))
			 if (!IsWithinANewElement (pElRef))
			   {
			     /* verifie la validite de l'attribut dans le cas des */
			     /* extensions de cellule des tableaux */
			     if (ThotLocalActions[T_checkextens] != NULL)
			       (*ThotLocalActions[T_checkextens])
				 (pAttr, pRoot, pRoot, TRUE);
			     if (DocOfSavedElements != pDoc)
			       /* reference et objet reference' sont */
			       /* dans des documents differents, on */
			       /* supprime l'attribut, sauf dans le */
			       /* cas particulier des tableaux. */
			       {
				 attrRef = TRUE;
				 if (ThotLocalActions[T_refattr] != NULL)
				   (*ThotLocalActions[T_refattr])
				     (pAttr, &attrRef);
				 if (!attrRef)
				   DeleteAttribute (pRoot, pAttr);
			       }
			   }
		     /* on traite le cas des references qui pointent sur de nouveaux elements */
		     UpdateReferences (pRoot, pDoc, pAttr->AeAttrReference);
		   }  
	  }
	pAttr = pAttr->AeNext;
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

   /* on n'a pas de traitement particulier a faire sur les references si */
   /* la copie est dans le meme document que l'original */
   if (DocOfSavedElements != pDoc)
     {
	/* l'element traite' est-il une reference ? */
	if (pRoot->ElTerminal && pRoot->ElLeafType == LtReference)
	   /* c'est un element reference */
	   pRef = pRoot->ElReference;
	else			/* c'est peut-etre une inclusion */
	   pRef = pRoot->ElSource;
	if (pRef != NULL)	/* c'est une reference, on */
	                        /*  met a jour la reference si besoin */
	   UpdateReferences (pRoot, pDoc, pRef);
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
