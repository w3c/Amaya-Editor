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
 * traitement des references
 *
 * Author: V. Quint (INRIA)
 *
 */

#include "ustring.h"
#include "libmsg.h"
#include "thot_sys.h"
#include "message.h"
#include "constmedia.h"
#include "typemedia.h"
#include "fileaccess.h"
#include "typecorr.h"
#include "appstruct.h"


#undef THOT_EXPORT
#define THOT_EXPORT
#include "edit_tv.h"

#include "tree_f.h"
#include "references_f.h"
#include "memory_f.h"
#include "callback_f.h"
#include "fileaccess_f.h"
#include "content_f.h"
#include "structschema_f.h"
#include "readpivot_f.h"
#include "applicationapi_f.h"
#include "schemas_f.h"


#define MAX_EXT_DOC 10


/*----------------------------------------------------------------------
   SearchElemLabel    cherche dans tout le sous arbre de pEl	
   l'element portant un label donne'.                              
   Retourne un pointeur sur cet element ou NULL si pas trouve'.    
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static PtrElement   SearchElemLabel (PtrElement pEl, LabelString label)

#else  /* __STDC__ */
static PtrElement   SearchElemLabel (pEl, label)
PtrElement          pEl;
LabelString         label;

#endif /* __STDC__ */

{
   PtrElement          result, pChild;

   result = NULL;
   if (strcmp (pEl->ElLabel, label) == 0)
      /* c'est l'element cherche' */
      result = pEl;
   else if (!pEl->ElTerminal)
      /* ce n'est pas une feuille, on cherche dans les sous-arbres */
      /* des fils */
     {
	pChild = pEl->ElFirstChild;
	while (pChild != NULL && result == NULL)
	  {
	     result = SearchElemLabel (pChild, label);
	     pChild = pChild->ElNext;
	  }
     }
   return result;
}

/*----------------------------------------------------------------------
   ReferredElement retourne un pointeur sur l'element qui est reference' 
   par la reference pRef. Retourne NULL si l'element       
   reference' n'est pas accessible.                        
   Si l'element reference' est dans un autre document,     
   retourne dans docIdent l'identificateur du document qui	
   contient l'element reference' et dans pDoc un pointeur  
   sur le contexte du document si ce document est charge'  
   (NULL si le document n'est pas charge').                
   Si l'element reference' est dans le meme document,      
   retourne dans docIdent une chaine vide et dans ce cas   
   pDoc est NULL.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
PtrElement          ReferredElement (PtrReference pRef, DocumentIdentifier * docIdent, PtrDocument * pDoc)
#else  /* __STDC__ */
PtrElement          ReferredElement (pRef, docIdent, pDoc)
PtrReference        pRef;
DocumentIdentifier *docIdent;
PtrDocument        *pDoc;
#endif /* __STDC__ */

{
   PtrElement          pEl;
   PtrElement          pE;
   PtrElement          pElRef;
   PtrReferredDescr    pRefD;
   PtrReferredDescr    pRefD1;
   PtrAttribute        pAttrRef;
   PtrDocument         pDocRef;
   int                 assoc;
   ThotBool            found;
   ThotBool            modif;

   pEl = NULL;
   modif = FALSE;
   ClearDocIdent (docIdent);
   *pDoc = NULL;
   if (pRef != NULL)
      if (pRef->RdReferred != NULL)
	{
	   pRefD1 = pRef->RdReferred;
	   if (!pRefD1->ReExternalRef)
	      /* l'element reference' est dans le meme document */
	      pEl = pRefD1->ReReferredElem;
	   else
	      /* l'element reference' est dans un autre document */
	      /* on retourne le nom du document externe */
	     {
		CopyDocIdent (docIdent, pRefD1->ReExtDocument);
		/* le document est-il charge'? */
		*pDoc = GetPtrDocument (pRefD1->ReExtDocument);
		if (*pDoc != NULL)
		   /* le document est charge' */
		   /* cherche dans ce document le descripteur d'element */
		   /* reference' */
		  {
		     pRefD = (*pDoc)->DocReferredEl;
		     /* saute le premier descripteur (bidon) */
		     if (pRefD != NULL)
			pRefD = pRefD->ReNext;
		     found = FALSE;
		     while (pRefD != NULL && !found)
		       {
			  if (!pRefD->ReExternalRef)
			     if (pRefD->ReReferredElem != NULL)
				if (strcmp (pRefD->ReReferredElem->ElLabel, pRefD1->ReReferredLabel) == 0)
				   found = TRUE;
			  if (!found)
			     pRefD = pRefD->ReNext;
		       }
		     if (found)
			/* pEl: l'element reference' */
			pEl = pRefD->ReReferredElem;
		     else
			/* essaie de retrouver l'element reference' en le cherchant */
			/* dans les arbres abstraits du document reference' */
		       {
			  if (pRef->RdAttribute == NULL)
			     /* c'est un attribut reference */
			    {
			       pElRef = pRef->RdElement;
			       pAttrRef = NULL;
			    }
			  else
			     /* c'est un element reference */
			    {
			       pElRef = NULL;
			       pAttrRef = pRef->RdAttribute;
			    }
			  pE = NULL;
			  /* on cherche d'abord dans l'arbre principal du document */
			  if ((*pDoc)->DocRootElement != NULL)
			     pE = SearchElemLabel ((*pDoc)->DocRootElement, pRefD1->ReReferredLabel);
			  /* on cherche danbs les arbres associes */
			  for (assoc = 0; pE == NULL && assoc < MAX_ASSOC_DOC; assoc++)
			     if ((*pDoc)->DocAssocRoot[assoc] != NULL)
				pE = SearchElemLabel ((*pDoc)->DocAssocRoot[assoc],
						   pRefD1->ReReferredLabel);
			  if (pE != NULL)
			     /* on a trouve' l'element reference'. On recree le lien */
			    {
			       pDocRef = DocumentOfElement (pRef->RdElement);
			       if (pDocRef != NULL)
				  modif = pDocRef->DocModified;
			       if (!SetReference (pElRef, pAttrRef, pE, pDocRef, *pDoc,
						  TRUE, FALSE))
				  pE = NULL;
			       else
				  pEl = pE;
			       if (pDocRef != NULL)
				  pDocRef->DocModified = modif;
			    }
			  if (pE == NULL)
			     /* l'element reference' n'existe pas dans le document */
			     /* reference', on annule cette reference. */
			     if (pElRef != NULL)
				CancelReference (pElRef, *pDoc);
			     else
				DeleteReference (pRef);
		       }
		  }
	     }
	}
   return pEl;
}

/*----------------------------------------------------------------------
   SearchExternalReferenceToElem cherche un document qui contient des references a`  
   l'element pEl et, dans ce document, s'il est charge',	
   cherche la premiere reference qui pointe sur pEl.       	
   - pDocEl est le document auquel appartient pEl.         	
   - processNotLoaded indique si on prend en compte les    	
   documents referencant non charge's (TRUE) ou si au    	
   contraire on les ignore (FALSE).                      	
   - pDocRef contient au retour un pointeur sur le         	
   contexte du document auquel appartient la reference   	
   trouvee. Seulement si la valeur de retour n'est pas   	
   NULL.                                                 	
   - pExtDoc est le document externe precedemment traite' (si	
   nextExtDoc est TRUE) ou celui qu'on veut traiter   	
   (si nextExtDoc est FALSE). pExtDoc doit etre NULL si	
   nextExtDoc est TRUE et qu'on n'a pas encore traite'	
   de references externes.                               	
   - nextExtDoc indique si on passe au document         	
   referencant suivant celui decrit par pExtDoc (TRUE) ou si 	
   on traite le document decrit par pExtDoc (FALSE).         	
   Retourne un pointeur sur la premiere reference trouvee  	
   dans le document externe et pExtDoc designe alors ce        	
   document externe.                                       	
   Si la valeur de retour est NULL et                      	
   si pExtDoc est NULL : on n'a rien trouve'.               	
   si pExtDoc n'est pas NULL : il y a des references a`     	
   l'element pEl dans le document designe' par pExtDoc 	
   mais ce document n'est pas charge' (cela ne se  	
   produit que si processNotLoaded est TRUE).      	
  ----------------------------------------------------------------------*/

#ifdef __STDC__
PtrReference        SearchExternalReferenceToElem (PtrElement pEl, PtrDocument pDocEl, ThotBool processNotLoaded, PtrDocument * pDocRef, PtrExternalDoc * pExtDoc, ThotBool nextExtDoc)

#else  /* __STDC__ */
PtrReference        SearchExternalReferenceToElem (pEl, pDocEl, processNotLoaded, pDocRef, pExtDoc, nextExtDoc)
PtrElement          pEl;
PtrDocument         pDocEl;
ThotBool            processNotLoaded;
PtrDocument        *pDocRef;
PtrExternalDoc     *pExtDoc;
ThotBool            nextExtDoc;

#endif /* __STDC__ */

{
   PtrDocument         pDoc;
   PtrReferredDescr    pRefD;
   PtrReference        pRef;
   ThotBool            found;

   if (*pExtDoc == NULL)
      /* on n'a pas encore cherche' dans un autre document que celui qui */
      /* contient l'element pEl. On va chercher dans le premier document */
      /* externe qui contient des references a l'element pEl. */
     {
	if (pEl != NULL)
	   if (pEl->ElReferredDescr != NULL)
	      *pExtDoc = pEl->ElReferredDescr->ReExtDocRef;
     }
   else if (nextExtDoc)
      /* on prend le document referencant suivant */
      *pExtDoc = (*pExtDoc)->EdNext;
   if (*pExtDoc == NULL)
      /* il n'y a pas de document suivant qui contienne des references a */
      /* notre element. Il n'y a donc pas de reference suivante */
     {
	pRef = NULL;
	*pDocRef = NULL;
     }
   else
      /* il y a des references a notre element dans le document decrit par pExtDoc */
     {
	/* ce document est-il charge' ? */
	pDoc = GetPtrDocument ((*pExtDoc)->EdDocIdent);
	if (pDoc == NULL)
	   /* le document referencant n'est pas charge' */
	  {
	     if (processNotLoaded)
		/* on a trouve' */
	       {
		  pRef = NULL;
		  *pDocRef = NULL;
	       }
	     else
		/* On ignore les documents non charge', on cherche le document */
		/* referencant suivant */
		pRef = SearchExternalReferenceToElem (pEl, pDocEl, processNotLoaded, pDocRef, pExtDoc, TRUE);
	  }
	else
	   /* Le document referencant est charge'. */
	  {
	     /* On cherche pour ce document le descripteur d'element reference' */
	     /* qui represente l'element pointe' par pEl */
	     pRefD = pDoc->DocReferredEl;
	     /* saute le premier descripteur (bidon) */
	     if (pRefD != NULL)
		pRefD = pRefD->ReNext;
	     found = FALSE;
	     while (pRefD != NULL && !found)
	       {
		  if (pRefD->ReExternalRef)
		     if (strcmp (pRefD->ReReferredLabel, pEl->ElLabel) == 0)
			if (SameDocIdent (pRefD->ReExtDocument, pDocEl->DocIdent))
			   found = TRUE;
		  if (!found)
		     pRefD = pRefD->ReNext;
	       }
	     if (found)
	       {
		  *pDocRef = pDoc;
		  pRef = pRefD->ReFirstReference;
	       }
	     else
	       {
		  *pDocRef = NULL;
		  pRef = NULL;
	       }
	  }
     }
   return pRef;
}


/*----------------------------------------------------------------------
   NewReferredElDescr cree un descripteur d'element referenc'e et le met en    
   tete de la chaine pour le document pointe par pDoc.     
  ----------------------------------------------------------------------*/

#ifdef __STDC__
PtrReferredDescr    NewReferredElDescr (PtrDocument pDoc)

#else  /* __STDC__ */
PtrReferredDescr    NewReferredElDescr (pDoc)
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   PtrReferredDescr    pRefD;

   GetReferredDescr (&pRefD);
   /* insere le nouveau descripteur au debut de la chaine des */
   /* descripteurs de references du document. Le premier descripteur */
   /* de document est bidon. */
   pRefD->RePrevious = pDoc->DocReferredEl;
   pRefD->ReNext = pRefD->RePrevious->ReNext;
   pRefD->RePrevious->ReNext = pRefD;
   if (pRefD->ReNext != NULL)
      pRefD->ReNext->RePrevious = pRefD;

   return pRefD;
}

/*----------------------------------------------------------------------
   DeleteReferredElDescr supprime le descripteur d'element reference' pointe'
   par pRefD.                                                
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                DeleteReferredElDescr (PtrReferredDescr pRefD)

#else  /* __STDC__ */
void                DeleteReferredElDescr (pRefD)
PtrReferredDescr    pRefD;

#endif /* __STDC__ */

{
   PtrExternalDoc      pExtDoc, pNextExtDoc;

   if (pRefD != NULL)
      /* Le premier descripteur de la chaine des descripteurs du document */
      /* est bidon. */
     {
	pRefD->RePrevious->ReNext = pRefD->ReNext;
	if (pRefD->ReNext != NULL)
	   pRefD->ReNext->RePrevious = pRefD->RePrevious;
	pExtDoc = pRefD->ReExtDocRef;
	/* libere les descripteurs de documents referencant */
	while (pExtDoc != NULL)
	  {
	     pNextExtDoc = pExtDoc->EdNext;
	     FreeExternalDoc (pExtDoc);
	     pExtDoc = pNextExtDoc;
	  }
	/* libere le descripteur d'element reference' */
	FreeReferredDescr (pRefD);
     }
}

/*----------------------------------------------------------------------
   DeleteAllReferences annule toutes les references a` l'element pointe' par   
   pEl et, si ce n'est pas un element exportable, supprime 
   le descripteur d'element reference' qui lui est         
   attache'.                                               
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                DeleteAllReferences (PtrElement pEl)

#else  /* __STDC__ */
void                DeleteAllReferences (pEl)
PtrElement          pEl;

#endif /* __STDC__ */

{
   PtrReference        pRef, pNextRef;

   if (pEl != NULL)
     {
	if (pEl->ElReferredDescr != NULL)
	  {
	     pRef = pEl->ElReferredDescr->ReFirstReference;
	     while (pRef != NULL)
	       {
		  pNextRef = pRef->RdNext;
		  if (pRef->RdAttribute != NULL)
		     /* c'est un attribut-reference, on supprime l'attribut */
		     {
		     RemoveAttribute (pRef->RdElement, pRef->RdAttribute);
		     DeleteAttribute (pRef->RdElement, pRef->RdAttribute);
		     }
		  else
		    {
		       pRef->RdReferred = NULL;
		       pRef->RdNext = NULL;
		       pRef->RdPrevious = NULL;
		    }
		  pRef = pNextRef;
	       }

	  }
     }
}

/*----------------------------------------------------------------------
   DeleteReference de'chaine la reference pointee par pRef.          
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                DeleteReference (PtrReference pRef)

#else  /* __STDC__ */
void                DeleteReference (pRef)
PtrReference        pRef;

#endif /* __STDC__ */

{
   if (pRef->RdPrevious == NULL)	/* premier de la chaine */
      /* dechaine l'element du descripteur de reference */
      if (pRef->RdNext == NULL)
	{
	   /* c'etait la seule reference, on la supprime */
	   if (pRef->RdReferred != NULL)
	     {
		pRef->RdReferred->ReFirstReference = NULL;
		if (pRef->RdReferred->ReExternalRef)
		   DeleteReferredElDescr (pRef->RdReferred);
		else
		   DeleteAllReferences (pRef->RdReferred->ReReferredElem);
	     }
	}
      else if (pRef->RdReferred != NULL)
	 pRef->RdReferred->ReFirstReference = pRef->RdNext;
   if (pRef->RdPrevious != NULL)
      pRef->RdPrevious->RdNext = pRef->RdNext;
   if (pRef->RdNext != NULL)
      pRef->RdNext->RdPrevious = pRef->RdPrevious;
   pRef->RdPrevious = NULL;
   pRef->RdNext = NULL;
   pRef->RdReferred = NULL;
   pRef->RdInternalRef = TRUE;
}

/*----------------------------------------------------------------------
   CancelReference annule la reference de l'element pEl			
   (pEl doit etre terminal et de nature Refer).            
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CancelReference (PtrElement pEl, PtrDocument pDoc)
#else  /* __STDC__ */
void                CancelReference (pEl, pDoc)
PtrElement          pEl;
PtrDocument         pDoc;
#endif /* __STDC__ */

{
   PtrElement          pAsc, pChild, pC1;
   PtrTextBuffer       pTxtBuf, pNextTxtBuf;

   if (pEl != NULL)
     {
	if (pEl->ElSource == NULL)
	  {
	     if (pEl->ElReference != NULL && pEl->ElTerminal &&
		 pEl->ElLeafType == LtReference)
		DeleteReference (pEl->ElReference);
	  }
	else
	   /* c'est une reference d'inclusion, on detruit tout le sous-arbre, */
	   /* qui est une copie de l'element qui etait reference' */
	  {
	     DeleteReference (pEl->ElSource);
	     if (pEl->ElTerminal)
		switch (pEl->ElLeafType)
		      {
			 case LtPicture:
			 case LtText:
			    if (pEl->ElText != NULL)
			       /* annule le contenu d'un texte ou d'une image */
			      {
				 pTxtBuf = pEl->ElText;		/* vide le 1er buffer */
				 pEl->ElTextLength = 0;
				 /* met a jour le volume des elements ascendants */
				 pAsc = pEl->ElParent;
				 while (pAsc != NULL)
				   {
				      pAsc->ElVolume = pAsc->ElVolume - pEl->ElVolume;
				      pAsc = pAsc->ElParent;
				   }
				 pEl->ElVolume = 0;
				 pTxtBuf->BuLength = 0;
				 pTxtBuf->BuContent[0] = EOS;
				 pTxtBuf = pTxtBuf->BuNext;
				 /* libere les autres buffers */
				 while (pTxtBuf != NULL)
				   {
				      pNextTxtBuf = pTxtBuf->BuNext;
				      DeleteTextBuffer (&pTxtBuf);
				      pTxtBuf = pNextTxtBuf;
				   }
			      }
			    break;
			 case LtPolyLine:
			    /* annule le contenu d'une poly line */
			    if (pEl->ElPolyLineBuffer != NULL)
			      {
				 /* met a jour le volume des elements ascendants */
				 pAsc = pEl->ElParent;
				 while (pAsc != NULL)
				   {
				      pAsc->ElVolume -= pEl->ElVolume;
				      pAsc = pAsc->ElParent;
				   }
				 pEl->ElVolume = 0;
				 /* vide le 1er buffer */
				 pTxtBuf = pEl->ElPolyLineBuffer;
				 pTxtBuf->BuLength = 0;
				 pTxtBuf = pTxtBuf->BuNext;
				 /* libere les buffers suivants */
				 while (pTxtBuf != NULL)
				   {
				      pNextTxtBuf = pTxtBuf->BuNext;
				      DeleteTextBuffer (&pTxtBuf);
				      pTxtBuf = pNextTxtBuf;
				   }
			      }
			    pEl->ElNPoints = 0;
			    pEl->ElPolyLineType = EOS;
			    break;
			 case LtSymbol:
			 case LtGraphics:
			    pEl->ElGraph = EOS;
			    break;
			 default:
			    break;
		      }
	     else
		/* ce n'est pas une feuille */
	       {
		  pChild = pEl->ElFirstChild;
		  pEl->ElFirstChild = NULL;
		  while (pChild != NULL)
		    {
		       pC1 = pChild;
		       pChild = pC1->ElNext;
		       DeleteElement (&pC1, pDoc);
		    }
	       }
	  }
     }
}

/*----------------------------------------------------------------------
   CopyReference                                                        
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CopyReference (PtrReference pCopyRef, PtrReference pSourceRef, PtrElement * pEl)
#else  /* __STDC__ */
void                CopyReference (pCopyRef, pSourceRef, pEl)
PtrReference        pCopyRef;
PtrReference        pSourceRef;
PtrElement         *pEl;
#endif /* __STDC__ */
{
   PtrReferredDescr    pRefD;

   pRefD = pCopyRef->RdReferred;
   pCopyRef->RdReferred = pSourceRef->RdReferred;
   pCopyRef->RdElement = *pEl;
   pCopyRef->RdTypeRef = pSourceRef->RdTypeRef;
   pCopyRef->RdInternalRef = pSourceRef->RdInternalRef;

   if (pCopyRef->RdReferred != NULL)
      if (pCopyRef->RdReferred != pRefD)
	 /* la copie reference maintenant un element different */
	 /* met la nouvelle reference en tete de la chaine des references */
	 /* qui designent le meme element */
	{
	   pCopyRef->RdNext = pCopyRef->RdReferred->ReFirstReference;
	   if (pCopyRef->RdNext != NULL)
	      pCopyRef->RdNext->RdPrevious = pCopyRef;
	   pCopyRef->RdReferred->ReFirstReference = pCopyRef;
	}
}

/*----------------------------------------------------------------------
   TransferReferences cherche dans le sous-arbre de racine pTarget tous
   les elements reference's et transfere sur eux les       
   references qui sont dans l'arbre copie'.                
   Dans tous les cas, coupe le lien d'inclusion de tous    
   les elements inclus du sous-arbre de pTarget.           
   Toutes les references externes contenues dans le        
   sous-arbre sont e'galement annule'es.                   
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TransferReferences (PtrElement pTarget, PtrDocument pDoc, PtrElement pEl, PtrDocument pSourceDoc)
#else  /* __STDC__ */
void                TransferReferences (pTarget, pDoc, pEl, pSourceDoc)
PtrElement          pTarget;
PtrDocument         pDoc;
PtrElement          pEl;
PtrDocument         pSourceDoc;
#endif /* __STDC__ */
{
   PtrElement          pChild;
   PtrReferredDescr    pDescRef;
   PtrReference        pRef, pNextRef;

   if (pTarget->ElReferredDescr != NULL)
      /* cet element est reference'. CopyTree n'a pas copie' son */
      /* descripteur d'element reference', qui est encore partage' avec */
      /* celui de l'element source */
      /* traite les references a l'element source qui sont dans la */
      /* copie */
     {
	pDescRef = pTarget->ElReferredDescr;	/* descripteur d'element reference' */
	pTarget->ElReferredDescr = NULL;	/* reste attache' a l'element source */
	pRef = pDescRef->ReFirstReference;	/* 1ere reference a l'element source */
	while (pRef != NULL)
	   /* parcourt les references a l'element source */
	  {
	     pNextRef = pRef->RdNext;	/* prepare la reference suivante */
	     if (ElemIsWithinSubtree (pRef->RdElement, pEl))
		/* la reference est dans la copie, on la fait pointer vers */
		/* l'element traite' (pTarget) */
	       {
		  if (pTarget->ElReferredDescr == NULL)
		     /* l'element n'a pas de descripteur d'element reference', */
		     /* on lui en affecte un */
		    {
		       pTarget->ElReferredDescr = NewReferredElDescr (pDoc);
		       pTarget->ElReferredDescr->ReExternalRef = FALSE;
		       pTarget->ElReferredDescr->ReReferredElem = pTarget;
		    }
		  /* lie le descripteur de reference et le descripteur */
		  /* d'element reference' de l'element traite' */
		  /* dechaine le descripteur de la chaine des references a */
		  /* l'element source */
		  if (pRef->RdNext != NULL)
		     pRef->RdNext->RdPrevious = pRef->RdPrevious;
		  if (pRef->RdPrevious == NULL)
		     pDescRef->ReFirstReference = pRef->RdNext;
		  else
		     pRef->RdPrevious->RdNext = pRef->RdNext;
		  /* le chaine en tete de la liste des references a */
		  /* l'element traite' */
		  pRef->RdReferred = pTarget->ElReferredDescr;
		  pRef->RdNext = pRef->RdReferred->ReFirstReference;
		  if (pRef->RdNext != NULL)
		     pRef->RdNext->RdPrevious = pRef;
		  pRef->RdReferred->ReFirstReference = pRef;
		  pRef->RdPrevious = NULL;
	       }
	     /* passe a la reference suivante */
	     pRef = pNextRef;
	  }
     }
   if (!pTarget->ElTerminal)
      /* element non terminal, on traite tous ses fils */
     {
	pChild = pTarget->ElFirstChild;
	while (pChild != NULL)
	  {
	     TransferReferences (pChild, pDoc, pEl, pSourceDoc);
	     pChild = pChild->ElNext;
	  }
     }
   else
      /* on traite les references seulement s'il s'agit de references externes */
   if (pTarget->ElLeafType == LtReference)
      /* c'est un element reference */
      if (pDoc != pSourceDoc)
	 /* annule le lien de la copie a sa source */
	{
	   DeleteReference (pTarget->ElReference);
	   FreeReference (pTarget->ElReference);
	}
}


/*----------------------------------------------------------------------
   RegisterAnExternalRef   note la reference externe pRef dans le  
   contexte du document pDoc. Cette reference sortante est         
   enregistree dans la liste des references nouvelles si IsNew est   
   vrai, dans la liste des references detruites sinon.             
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         RegisterAnExternalRef (PtrReference pRef, PtrDocument pDoc, ThotBool IsNew)
#else  /* __STDC__ */
static void         RegisterAnExternalRef (pRef, pDoc, IsNew)
PtrReference        pRef;
PtrDocument         pDoc;
ThotBool            IsNew;
#endif /* __STDC__ */
{
   PtrOutReference     pRefSort, pRefSortPrec;
   PtrReferredDescr    pDElemRef;
   ThotBool            found;
   PtrDocument         pDocRef;
   PtrReferredDescr    pRefD;

   if (pRef != NULL)
      if (pRef->RdReferred != NULL && !pRef->RdInternalRef)
	 /* c'est une reference sortante */
	 if (pRef->RdNext == NULL && pRef->RdPrevious == NULL)
	    /* c'est la seule reference a cet element */
	   {
	      /* descripteur de l'element reference' */
	      pDElemRef = pRef->RdReferred;
	      if (!IsNew)
		 /* il s'agit d'une reference detruite, on verifie d'abord que */
		 /* cette reference est connue du document reference', si le */
		 /* document reference' est charge' */
		{
		   pDocRef = GetPtrDocument (pDElemRef->ReExtDocument);
		   if (pDocRef != NULL)
		      /* le document est bien charge', on peut verifier. */
		      /* On cherche dans ce document le descripteur d'element */
		      /* reference' */
		     {
			pRefD = pDocRef->DocReferredEl;
			/* saute le premier descripteur (bidon) */
			if (pRefD != NULL)
			   pRefD = pRefD->ReNext;
			found = FALSE;
			while (pRefD != NULL && !found)
			  {
			     if (!pRefD->ReExternalRef)
				if (pRefD->ReReferredElem != NULL)
				   if (strcmp (pRefD->ReReferredElem->ElLabel, pDElemRef->ReReferredLabel) == 0)
				      found = TRUE;
			     if (!found)
				pRefD = pRefD->ReNext;
			  }
			if (!found)
			   /* le descripteur d'element reference' n'existe pas, on ne */
			   /* fait rien */
			   return;
		     }
		}
	      /* verifie d'abord que cette reference sortante n'est pas */
	      /* dans la liste des references detruites si c'est une */
	      /* creation, ou l'inverse */
	      if (IsNew)
		 /* creation, on examine la liste des references detruites */
		 pRefSort = pDoc->DocDeadOutRef;
	      else
		 /* destruction, on examine la liste des references creees */
		 pRefSort = pDoc->DocNewOutRef;
	      found = FALSE;
	      pRefSortPrec = NULL;
	      while (pRefSort != NULL && !found)
		{
		   if (strcmp (pRefSort->OrLabel, pDElemRef->ReReferredLabel) == 0)
		      if (SameDocIdent (pRefSort->OrDocIdent, pDElemRef->ReExtDocument))
			 found = TRUE;
		   if (!found)
		     {
			pRefSortPrec = pRefSort;
			pRefSort = pRefSort->OrNext;
		     }
		}
	      if (found)
		 /* la reference sortante est presente dans l'autre liste */
		{
		   /* on la dechaine de sa liste */
		   if (pRefSortPrec == NULL)
		      /* c'etait la premiere de la liste */
		     {
			if (IsNew)
			   pDoc->DocDeadOutRef = pRefSort->OrNext;
			else
			   pDoc->DocNewOutRef = pRefSort->OrNext;
		     }
		   else
		      pRefSortPrec->OrNext = pRefSort->OrNext;
		   /* on la libere */
		   FreeOutputRef (pRefSort);
		   pRefSort = NULL;
		}
	      else
		 /* la reference sortante n'est pas dans l'autre liste */
		{
		   /* acquiert un descripteur de ref sortante */
		   GetOutputRef (&pRefSort);
		   /* remplit ce descripteur */
		   strncpy (pRefSort->OrLabel, pDElemRef->ReReferredLabel, MAX_LABEL_LEN);
		   CopyDocIdent (&(pRefSort->OrDocIdent), pDElemRef->ReExtDocument);
		   /* on l'insere en tete de sa liste */
		   if (IsNew)
		     {
			pRefSort->OrNext = pDoc->DocNewOutRef;
			pDoc->DocNewOutRef = pRefSort;
		     }
		   else
		     {
			pRefSort->OrNext = pDoc->DocDeadOutRef;
			pDoc->DocDeadOutRef = pRefSort;
		     }
		}
	   }
}


/*----------------------------------------------------------------------
   RegisterExternalRef        note dans le contexte du document pDoc  
   toutes les references sortantes qui sont dans le sous-arbre de  
   racine pEl, cet element compris. Ces references sortantes sont  
   enregistrees dans la liste des references nouvelles si IsNew est  
   vrai, dans la liste des references detruites sinon.             
   On prend egalement en compte les attributs reference sortant.   
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                RegisterExternalRef (PtrElement pEl, PtrDocument pDoc, ThotBool IsNew)
#else  /* __STDC__ */
void                RegisterExternalRef (pEl, pDoc, IsNew)
PtrElement          pEl;
PtrDocument         pDoc;
ThotBool            IsNew;
#endif /* __STDC__ */
{
   PtrReference        pRef;
   PtrElement          pChild;
   PtrAttribute        pAttr;

   /* l'element traite' est-il une reference ? */
   if (pEl->ElTerminal && pEl->ElLeafType == LtReference)
      /* c'est un element reference */
      pRef = pEl->ElReference;
   else
      /* c'est peut-etre une inclusion */
      pRef = pEl->ElSource;
   if (pRef != NULL)
      /* c'est bien une reference ou une inclusion */
      if (!pRef->RdInternalRef)
	 /* c'est une reference sortante */
	 RegisterAnExternalRef (pRef, pDoc, IsNew);
   /* cherche tous les attributs reference de l'element */
   pAttr = pEl->ElFirstAttr;
   while (pAttr != NULL)
     {
	if (pAttr->AeAttrType == AtReferenceAttr)
	   if (pAttr->AeAttrReference != NULL)
	      if (!pAttr->AeAttrReference->RdInternalRef)
		 /* c'est un attribut reference externe, on le note */
		 RegisterAnExternalRef (pAttr->AeAttrReference, pDoc, IsNew);
	/* passe a l'attribut suivant */
	pAttr = pAttr->AeNext;
     }
   /* traite les fils de l'element */
   if (!pEl->ElTerminal)
     {
	pChild = pEl->ElFirstChild;
	while (pChild != NULL)
	  {
	     RegisterExternalRef (pChild, pDoc, IsNew);
	     pChild = pChild->ElNext;
	  }
     }
}


/*----------------------------------------------------------------------
   	AddDocOfExternalRef   ajoute le document dont l'identificateur est	
   	docIdent a la liste des documents contenant des references	
   	externes a l'element pEl qui appartient au document pDoc2.	
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                AddDocOfExternalRef (PtrElement pEl, DocumentIdentifier docIdent, PtrDocument pDoc2)
#else  /* __STDC__ */
void                AddDocOfExternalRef (pEl, docIdent, pDoc2)
PtrElement          pEl;
DocumentIdentifier  docIdent;
PtrDocument         pDoc2;
#endif /* __STDC__ */

{
   ThotBool            found;
   PtrExternalDoc      pExtDoc;


   if (pEl != NULL)
      if (pEl->ElReferredDescr != NULL)
	 if (ustrcmp (pDoc2->DocIdent, docIdent) != 0)
	    if (pEl->ElReferredDescr->ReExtDocRef == NULL)
	       /* cet element n'est encore reference' par aucun document externe, */
	       /* on lui acquiert un premier descripteur de document externe      */
	      {
		 GetExternalDoc (&pExtDoc);
		 pEl->ElReferredDescr->ReExtDocRef = pExtDoc;
		 if (pExtDoc != NULL)
		    CopyDocIdent (&(pExtDoc->EdDocIdent), docIdent);
	      }
	    else
	       /* cherche si le document qui reference l'element est deja dans */
	       /* la liste des documents externes qui referencent l'element    */
	      {
		 pExtDoc = pEl->ElReferredDescr->ReExtDocRef;
		 found = FALSE;
		 do
		    if (SameDocIdent (pExtDoc->EdDocIdent, docIdent))
		       found = TRUE;
		    else if (pExtDoc->EdNext != NULL)
		       pExtDoc = pExtDoc->EdNext;
		 while (!found && pExtDoc->EdNext != NULL) ;
		 if (!found)
		    found = SameDocIdent (pExtDoc->EdDocIdent, docIdent);
		 if (!found)
		    /* ajoute un descripteur de document externe qui reference */
		    /* l'element */
		   {
		      GetExternalDoc (&pExtDoc->EdNext);
		      if (pExtDoc->EdNext != NULL)
			 CopyDocIdent (&(pExtDoc->EdNext->EdDocIdent), docIdent);
		   }
	      }
}


/*----------------------------------------------------------------------
   	SetReference cherche parmi les elements ascendants de l'element	
   	pTargetEl un element du type des elements pointes par l'element	
   	reference pRefEl ou par l'attribut reference pRefAttr (des deux	
   	pointeurs, on choisit celui qui n'est pas NULL). Si ancestor	
   	est Faux, on ne cherche pas parmi les ascendants: on prend	
   	directement pTargetEl.						
   	pDoc est document contenant la reference (element pRefEl ou	
   	attribut pRefAttr).						
   	pTargetDoc est le document contenant l'element pTargetEl.	
   	Si trouve', fait pointer l'element reference pRefEl ou		
   	l'attribut reference pRefAttr sur cet element et retourne	
   	'Vrai'. Sinon retourne 'Faux'.					
  ----------------------------------------------------------------------*/

#ifdef __STDC__
ThotBool            SetReference (PtrElement pRefEl, PtrAttribute pRefAttr, PtrElement pTargetEl, PtrDocument pDoc, PtrDocument pTargetDoc, ThotBool ancestor, ThotBool withAppEvent)

#else  /* __STDC__ */
ThotBool            SetReference (pRefEl, pRefAttr, pTargetEl, pDoc, pTargetDoc, ancestor, withAppEvent)
PtrElement          pRefEl;
PtrAttribute        pRefAttr;
PtrElement          pTargetEl;
PtrDocument         pDoc;
PtrDocument         pTargetDoc;
ThotBool            ancestor;
ThotBool            withAppEvent;

#endif /* __STDC__ */

{
   PtrElement          pEl, pAsc;
   PtrSSchema          pSS;
   int                 typeNum;
   ThotBool            ret, stop;
   PtrReference        pRef;
   PtrReferredDescr    pRefD;
   NotifyOnTarget      notifyEl;

   ret = FALSE;
   if (pRefEl != NULL)
     {
	if (pRefEl->ElStructSchema->SsRule[pRefEl->ElTypeNumber - 1].SrRefImportedDoc)
	   /* c'est une inclusion d'un document externe */
	   if (pDoc == pTargetDoc)
	      /* reference et element reference' sont dans le meme document */
	     {
		TtaDisplaySimpleMessage (INFO, LIB, TMSG_SELF_DOC_INCLUSION_IMP);
		pTargetEl = NULL;	/* sort de la procedure sans rien faire d'autre */
	     }
     }
   if (pTargetEl != NULL && (pRefEl != NULL || pRefAttr != NULL))
     {
	if (!ancestor)
	   pEl = pTargetEl;
	else
	   /* cherche le type de l'element reference' prevu */
	  {
	     typeNum = 0;
	     ReferredType (pRefEl, pRefAttr, &pSS, &typeNum);
	     /* cherche un element englobant du type prevu pour la reference */
	     if (typeNum == 0)
		/* pas de type precise' pour l'element reference' */
		pEl = pTargetEl;
	     else
		pEl = GetTypedAncestor (pTargetEl, typeNum, pSS);
	  }
	/* On ne peut, dans tout les cas, referencer un element inclus */
	if (pEl != NULL)
	  {
	     if (pEl->ElIsCopy)
		/* l'element a copier est dans un element copie'. Est-il */
		/* dans un element copie' par inclusion ? */
	       {
		  pAsc = pEl;
		  while (pAsc != NULL)
		     if (pAsc->ElSource != NULL)
			/* l'ascendant est un element inclus */
		       {
			  pAsc = NULL;	/* stop */
			  pEl = NULL;	/* abandon */
			  TtaDisplaySimpleMessage (INFO, LIB, TMSG_A_COPY);
			  /* message 'Designez l'original' */
		       }
		     else
			/* examine l'ascendant suivant */
			pAsc = pAsc->ElParent;
	       }
	  }
	/* si un evenement est demande', on l'envoie */
	if (pEl != NULL && withAppEvent && pRefEl != NULL)
	   /* on ne l'envoie que si ce n'est ni une inclusion ni un attribut */
	   if (pRefAttr == NULL && pRefEl->ElSource == NULL)
	     {
		notifyEl.event = TteElemSetReference;
		notifyEl.document = (Document) IdentDocument (pDoc);
		notifyEl.element = (Element) pRefEl;
		notifyEl.target = (Element) pEl;
		notifyEl.targetdocument = (Document) IdentDocument (pTargetDoc);
		if (CallEventType ((NotifyEvent *) & notifyEl, TRUE))
		   pEl = NULL;
	     }
	if (pEl != NULL)
	   /* l'element a referencer est pointe' par pEl */
	   /* annule d'abord la reference existante */
	  {
	     if (pRefEl != NULL)
	       {
		  if (pRefEl->ElSource != NULL)
		     pRef = pRefEl->ElSource;
		  else
		     pRef = pRefEl->ElReference;
		  if (ancestor)
		     /* si c'est une reference externe, note cette reference dans */
		     /* la liste des references sortantes detruites du document */
		     RegisterAnExternalRef (pRef, pDoc, FALSE);
		  CancelReference (pRefEl, pDoc);

	       }
	     else
	       {
		  pRef = pRefAttr->AeAttrReference;
		  RegisterAnExternalRef (pRef, pDoc, FALSE);
		  if (pRef != NULL)
		     DeleteReference (pRef);
	       }
	     /* enregistre la nouvelle reference */
	     if (pRef != NULL)
	       {
		  if (pEl->ElReferredDescr == NULL)
		     /* cet element n'a pas encore de descripteur d'element */
		     /* reference', on lui en associe un */
		    {
		       pEl->ElReferredDescr = NewReferredElDescr (pTargetDoc);
		       pEl->ElReferredDescr->ReReferredElem = pEl;
		       strncpy (pEl->ElReferredDescr->ReReferredLabel, pEl->ElLabel, MAX_LABEL_LEN);
		    }
		  if (pDoc == pTargetDoc)
		     /* reference et element reference' dans le meme document */
		    {
		       pRef->RdInternalRef = TRUE;
		       /* on ajoute la nouvelle reference en tete de la chaine des */
		       /* references a cet element */
		       pRef->RdNext = pEl->ElReferredDescr->ReFirstReference;
		       if (pRef->RdNext != NULL)
			  pRef->RdNext->RdPrevious = pRef;
		       pRef->RdReferred = pEl->ElReferredDescr;
		       pRef->RdReferred->ReFirstReference = pRef;
		    }
		  else
		     /* reference et element reference' dans des */
		     /* documents differents */
		    {
		       pRef->RdInternalRef = FALSE;
		       AddDocOfExternalRef (pEl, pDoc->DocIdent, pTargetDoc);
		       /* cet element reference' est-il deja represente' comme */
		       /*element reference' externe dans le document qui reference */
		       pRefD = pDoc->DocReferredEl->ReNext;
		       stop = FALSE;
		       do
			 {
			    if (pRefD == NULL)
			       stop = TRUE;
			    else if (pRefD->ReExternalRef)
			       if (strcmp (pRefD->ReReferredLabel, pEl->ElLabel) == 0)
				  if (SameDocIdent (pRefD->ReExtDocument,
						    pTargetDoc->DocIdent))
				     stop = TRUE;
			    if (!stop)
			       pRefD = pRefD->ReNext;
			 }
		       while (!stop);
		       if (pRefD == NULL)
			  /* l'element reference' n'est pas encore represente'. */
			  /* on lui ajoute un descripteur dans le document de la */
			  /* reference */
			 {
			    pRefD = NewReferredElDescr (pDoc);
			    pRefD->ReExternalRef = TRUE;
			    strncpy (pRefD->ReReferredLabel, pEl->ElLabel, MAX_LABEL_LEN);
			    CopyDocIdent (&(pRefD->ReExtDocument), pTargetDoc->DocIdent);
			 }
		       /* on ajoute la nouvelle reference en tete de la chaine */
		       /* des references a cet element */
		       pRef->RdNext = pRefD->ReFirstReference;
		       if (pRef->RdNext != NULL)
			  pRef->RdNext->RdPrevious = pRef;
		       pRef->RdReferred = pRefD;
		       pRef->RdReferred->ReFirstReference = pRef;
		    }
		  if (pRefEl != NULL)
		     if (pRefEl->ElStructSchema->SsRule[pRefEl->ElTypeNumber - 1].
			 SrRefImportedDoc)
			/* c'est un renvoi a un document importe' */
		       {
			  pRef->RdTypeRef = RefInclusion;
			  pRef->RdInternalRef = FALSE;
		       }
		  if (pRef != NULL)
		     if (ancestor)
			/* si c'est une reference externe, note cette reference */
			/*dans la liste des references sortantes creees du document */
			RegisterAnExternalRef (pRef, pDoc, TRUE);
		  ret = TRUE;
		  if (withAppEvent && pRefEl != NULL)
		     /* on n'envoie un evenement que si ce n'est ni une */
		     /* inclusion ni un attribut */
		     if (pRefAttr == NULL && pRefEl->ElSource == NULL)
		       {
			  notifyEl.event = TteElemSetReference;
			  notifyEl.document = (Document) IdentDocument (pDoc);
			  notifyEl.element = (Element) pRefEl;
			  notifyEl.target = (Element) pEl;
			  notifyEl.targetdocument = (Document) IdentDocument (pTargetDoc);
			  CallEventType ((NotifyEvent *) & notifyEl, FALSE);
		       }
	       }
	  }
     }
   return ret;
}


/*----------------------------------------------------------------------
   CopyDescrExtDoc    attache a un descripteur d'element detruit ou   
   deplace' une copie de la chaine des descripteurs de documents   
   externes referencant l'element pEl                              
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                CopyDescrExtDoc (PtrElement pEl, PtrChangedReferredEl pChngRef)

#else  /* __STDC__ */
void                CopyDescrExtDoc (pEl, pChngRef)
PtrElement          pEl;
PtrChangedReferredEl pChngRef;

#endif /* __STDC__ */

{
   PtrExternalDoc      pExtDocSrc, pExtDoc, pPrevExtDoc;

   pExtDocSrc = pEl->ElReferredDescr->ReExtDocRef;
   pPrevExtDoc = NULL;
   while (pExtDocSrc != NULL)
     {
	GetExternalDoc (&pExtDoc);
	CopyDocIdent (&(pExtDoc->EdDocIdent), pExtDocSrc->EdDocIdent);
	if (pPrevExtDoc == NULL)
	   pChngRef->CrReferringDoc = pExtDoc;
	else
	   pPrevExtDoc->EdNext = pExtDoc;
	pExtDoc->EdNext = NULL;
	pPrevExtDoc = pExtDoc;
	pExtDocSrc = pExtDocSrc->EdNext;
     }
}

/*----------------------------------------------------------------------
   RegisterDeletedReferredElem     note dans le contexte du document pDoc  
   tous les elements reference's par d'autres documents qui sont   
   dans le sous-arbre de racine pEl, cet element compris. Ces      
   elements reference's sont enregistre's dans la liste des        
   elements reference's detruits.                                  
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                RegisterDeletedReferredElem (PtrElement pEl, PtrDocument pDoc)

#else  /* __STDC__ */
void                RegisterDeletedReferredElem (pEl, pDoc)
PtrElement          pEl;
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   PtrElement          pChild;
   PtrChangedReferredEl pChngRef;

   if (pEl->ElReferredDescr != NULL)
      if (pEl->ElReferredDescr->ReExtDocRef != NULL)
	 /* cet element est reference' par d'autres documents */
	{
	   /* acquiert un descripteur et le remplit */
	   GetChangedReferredEl (&pChngRef);
	   strncpy (pChngRef->CrOldLabel, pEl->ElLabel, MAX_LABEL_LEN);
	   pChngRef->CrNewLabel[0] = EOS;
	   CopyDocIdent (&(pChngRef->CrOldDocument), pDoc->DocIdent);
	   ClearDocIdent (&(pChngRef->CrNewDocument));
	   pChngRef->CrReferringDoc = NULL;
	   /* chaine ce descripteur */
	   pChngRef->CrNext = pDoc->DocChangedReferredEl;
	   pDoc->DocChangedReferredEl = pChngRef;
	   /* copie la liste des documents qui referencent l'element */
	   CopyDescrExtDoc (pEl, pChngRef);
	}
   /* traite les fils de l'element */
   if (!pEl->ElTerminal)
     {
	pChild = pEl->ElFirstChild;
	while (pChild != NULL)
	  {
	     RegisterDeletedReferredElem (pChild, pDoc);
	     pChild = pChild->ElNext;
	  }
     }
}

/*----------------------------------------------------------------------
UpdateInclusionElements		Up to date the value of inclusions that 
belong to the document pointed by pDoc. If loadExternalDoc is TRUE, the
inclusions whose the sources belong to another document, are up to date
too. In this case, the others documents are opened temporarely. If 
removeExclusions is TRUE, the exclusions are removed from the documents
opened temporarely.
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void	UpdateInclusionElements (PtrDocument pDoc, ThotBool loadExternalDoc,
ThotBool removeExclusions)
#else  /* __STDC__ */
void	UpdateInclusionElements (pDoc, loadExternalDoc, removeExclusions)
PtrDocument	pDoc;
ThotBool		loadExternalDoc;
ThotBool		removeExclusions;
#endif /* __STDC__ */

{
   PtrDocument           pSourceDoc;
   PtrDocument           pExternalDoc[MAX_EXT_DOC];
   PtrElement            pSource;
   PtrReference          pRef;
   PtrReferredDescr      pRefD;
   SRule                *pSRule;
   DocumentIdentifier    docIdent;
   ThotBool              ok = FALSE;
   int                   d, extDocNum;
   NotifyOnTarget	 notifyEl;

   /* nettoie la table des documents externes charge's */
   for (extDocNum = 0; extDocNum < MAX_EXT_DOC; extDocNum++)
      pExternalDoc[extDocNum] = NULL;

   /* parcourt la chaine des descripteurs d'elements references */
   /* du document, pour traiter toutes les references */
   pRefD = pDoc->DocReferredEl->ReNext;
   while (pRefD != NULL)
     {
        pRef = pRefD->ReFirstReference;
        /* premiere reference a l'element reference' courant */
        /* parcourt toutes les references designant l'element */
        /* reference' courant */
        while (pRef != NULL)
          {
             if (pRef->RdTypeRef == RefInclusion)
                /* il s'agit bien d'une reference d'inclusion */
                if (pRef->RdElement->ElSource == pRef)
                   /* c'est bien une inclusion avec expansion, */
                   /* on copie l'arbre abstrait de l'element inclus */
                  {
                     pSource = ReferredElement (pRef,
                                                &docIdent,
                                                &pSourceDoc);

                     /* Ici il faut faire beaucoup d'attention car le target 
                        de l'inclusion ou source peut etre dans le meme document
                        ou dans un document externe. */
                      
                     notifyEl.event = TteElemFetchInclude;
                     notifyEl.target = (Element) pSource;
                     if (docIdent[0] == EOS)
                          notifyEl.targetdocument = (Document) IdentDocument (pDoc);
                     else if (pSourceDoc != NULL)
                          notifyEl.targetdocument = (Document) IdentDocument (pSourceDoc);
                     else
                          notifyEl.targetdocument = (Document) 0;
                     notifyEl.element = (Element) pRef->RdElement;
                     notifyEl.document = (Document) IdentDocument (pDoc);
                     /* Ici c'est possible que notifyEl.target ou notifyEl.targetdocument
                        soient egaux a NULL. Par exemple, quand la cible ou source d'une
                        inclusion n'est pas dans le meme document que l'inclusion. */

                     if (!CallEventType ((NotifyEvent *) & notifyEl, TRUE))
                        /* l'application accepte le traitement */ 
                       {
                          if (loadExternalDoc)
                             /* l'element inclus est-il accessible ? */
                            {
                               if (pSource == NULL)
                                  if (!DocIdentIsNull (docIdent))
                                     if (pSourceDoc == NULL)
                                        /* il y a bien un objet a inclure qui */
                                        /* appartient au document docIdent et ce */
                                        /* document n'est pas charge'. */
                                        /* On le charge. Cherche une entree libre */
                                        /* dans la table des documents externes */
                                        /* charge's. */
                                       {
                                          extDocNum = 0;
                                          while ( (pExternalDoc[extDocNum] != NULL) &&
                                                  (extDocNum < MAX_EXT_DOC - 1) )
                                             extDocNum++;
                                          if (pExternalDoc[extDocNum] == NULL)
                                             /* on a trouve' une entree libre, on */
                                             /* charge  le document externe */
                                            {
                                               CreateDocument (&pExternalDoc[extDocNum]);
                                               if (pExternalDoc[extDocNum] != NULL)
                                                 {
                                                    CopyDocIdent (&pExternalDoc[extDocNum]->DocIdent,
                                                                  docIdent);
                                                    ok = OpenDocument (
                                                            NULL,
                                                            pExternalDoc[extDocNum],
                                                            FALSE,
                                                            FALSE,
                                                            NULL,
                                                            FALSE,
                                                            removeExclusions);
                                                 }
                                               if (pExternalDoc[extDocNum] != NULL)
                                                 {
                                                    CopyDocIdent (
                                                       &pExternalDoc[extDocNum]->DocIdent,
                                                       docIdent);
                                                    if (!ok)
                                                      {
                                                         /* echec a l'ouverture du */
                                                         /* document. */
                                                         TtaDisplayMessage (
                                                             INFO,
                                                             TtaGetMessage (
                                                                LIB,
                                                                TMSG_LIB_MISSING_FILE),
                                                             docIdent);
                                                         FreeDocument (pExternalDoc[extDocNum]);
                                                         pExternalDoc[extDocNum] = NULL;
                                                      }
                                                 }
                                            }
                                       }
                               pSRule = &pRef->RdElement->ElStructSchema->SsRule[pRef->RdElement->ElTypeNumber - 1];
                            }
                          /* inclusion d'un document externe */
                          CopyIncludedElem (pRef->RdElement, pDoc);

                          /* Envoyer le message post de TteElemFetchInclude */
                          pSource = ReferredElement (pRef,
                                                &docIdent,
                                                &pSourceDoc);

                          notifyEl.event = TteElemFetchInclude;
                          notifyEl.target = (Element) pSource;
                          if (docIdent[0] == EOS)
                               notifyEl.targetdocument = (Document) IdentDocument (pDoc);
                          else if (pSourceDoc != NULL)
                               notifyEl.targetdocument = (Document) IdentDocument (pSourceDoc);
                          else
                               notifyEl.targetdocument = (Document) 0;
                          notifyEl.element = (Element) pRef->RdElement;
                          notifyEl.document = (Document) IdentDocument (pDoc);

                          /* Ici notifyEl.target et notifyEl.targetdocument
                             sont differents de NULL si loadExternalDoc == TRUE.
                             Dans le cas ou la cible (source) d'une
                             inclusion n'est pas dans le meme document que l'inclusion,
                             le document externe a ete ouvert temporairement. */

                          CallEventType ((NotifyEvent *) & notifyEl, FALSE);
                       } 
                  }
             pRef = pRef->RdNext;
             /* passe a la reference suivante */
          }
        pRefD = pRefD->ReNext;
        /* passe au descripteur d'element reference' suivant */
     }

   /* on decharge les documents externes qui ont ete charge's */
   /* pour copier des elements inclus */
   if (loadExternalDoc)
      for (extDocNum = 0; extDocNum < MAX_EXT_DOC; extDocNum++)
         if (pExternalDoc[extDocNum] != NULL)
           {
              DeleteAllTrees (pExternalDoc[extDocNum]);
              FreeDocumentSchemas (pExternalDoc[extDocNum]);
              /* cherche le document dans la table */
              /* des documents */
              d = 0;
              while ( (LoadedDocument[d] != pExternalDoc[extDocNum]) &&
                      (d < MAX_DOCUMENTS - 1) )
                 d++;
              /* libere l'entree de la table des documents */
              if (LoadedDocument[d] == pExternalDoc[extDocNum])
                 LoadedDocument[d] = NULL;
              /* libere tout le document */
              FreeDocument (pExternalDoc[extDocNum]);
           }

}

