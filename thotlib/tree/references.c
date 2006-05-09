/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2005
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * traitement des references
 *
 * Author: V. Quint (INRIA)
 *
 */
#include "thot_gui.h"
#include "libmsg.h"
#include "thot_sys.h"
#include "message.h"
#include "constmedia.h"
#include "typemedia.h"
#include "fileaccess.h"
#include "typecorr.h"
#include "appstruct.h"

/*#define THOT_EXPORT*/
#define THOT_EXPORT extern	/* to avoid redefinitions */
#include "edit_tv.h"

#include "tree_f.h"
#include "references_f.h"
#include "memory_f.h"
#include "callback_f.h"
#include "content_f.h"
#include "structschema_f.h"
#include "readpivot_f.h"
#include "applicationapi_f.h"
#include "schemas_f.h"



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
  ----------------------------------------------------------------------*/
PtrReference  NextReferenceToEl (PtrElement pEl, PtrDocument pDoc,
                                 PtrReference pPrevRef)
{
  PtrReference        pRef;

  if (pPrevRef)
    pRef = pPrevRef->RdNext;
  else
    {
      pRef = NULL;
      /* premier appel de la fonction */
      if (pEl && pEl->ElReferredDescr)
        pRef = pEl->ElReferredDescr->ReFirstReference;
    }
  return pRef;
}

/*----------------------------------------------------------------------
  SearchElemLabel    cherche dans tout le sous arbre de pEl	
  l'element portant un label donne'.                              
  Retourne un pointeur sur cet element ou NULL si pas trouve'.    
  ----------------------------------------------------------------------*/
static PtrElement SearchElemLabel (PtrElement pEl, LabelString label)
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
  ----------------------------------------------------------------------*/
PtrElement ReferredElement (PtrReference pRef)
{
  PtrElement          pEl;
  PtrReferredDescr    pRefD;

  pEl = NULL;
  if (pRef && pRef->RdReferred)
    {
      pRefD = pRef->RdReferred;
      /* l'element reference' est dans le meme document */
      pEl = pRefD->ReReferredElem;
    }
  return pEl;
}


/*----------------------------------------------------------------------
  LinkReferredElDescr links the descriptor at the first position 
  in the document list of references.
  ----------------------------------------------------------------------*/
void LinkReferredElDescr (PtrReferredDescr pRefD, PtrDocument pDoc)
{
  /* The first descriptor is never used */
  pRefD->RePrevious = pDoc->DocReferredEl;
  pRefD->ReNext = pRefD->RePrevious->ReNext;
  pRefD->RePrevious->ReNext = pRefD;
  if (pRefD->ReNext)
    pRefD->ReNext->RePrevious = pRefD;
}

/*----------------------------------------------------------------------
  NewReferredElDescr creates a new descriptor and links it at the
  first position in the document list of references.
  ----------------------------------------------------------------------*/
PtrReferredDescr NewReferredElDescr (PtrDocument pDoc)
{
  PtrReferredDescr    pRefD;

  GetReferredDescr (&pRefD);
  LinkReferredElDescr (pRefD, pDoc);
  return pRefD;
}

/*----------------------------------------------------------------------
  UnlinkReferredElDescr unlinks the descriptor from the document
  list of references.
  ----------------------------------------------------------------------*/
void UnlinkReferredElDescr (PtrReferredDescr pRefD)
{
  if (pRefD->RePrevious)
    pRefD->RePrevious->ReNext = pRefD->ReNext;
  if (pRefD->ReNext)
    pRefD->ReNext->RePrevious = pRefD->RePrevious;
  pRefD->RePrevious = NULL;
  pRefD->ReNext = NULL;
}

/*----------------------------------------------------------------------
  DeleteReferredElDescr removes the descriptor and unlinks it
  from the document list of references.                                   
  ----------------------------------------------------------------------*/
void DeleteReferredElDescr (PtrReferredDescr pRefD)
{
  if (pRefD)
    /* Le premier descripteur de la chaine des descripteurs du document */
    /* est bidon. */
    {
      UnlinkReferredElDescr (pRefD);
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
void DeleteAllReferences (PtrElement pEl)
{
  PtrReference        pRef, pNextRef;

  if (pEl && pEl->ElReferredDescr)
    {
      pRef = pEl->ElReferredDescr->ReFirstReference;
      while (pRef)
        {
          pNextRef = pRef->RdNext;
          if (pRef->RdAttribute)
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

/*----------------------------------------------------------------------
  DeleteReference de'chaine la reference pointee par pRef.          
  ----------------------------------------------------------------------*/
void DeleteReference (PtrReference pRef)
{
  if (pRef->RdPrevious == NULL)	/* premier de la chaine */
    {
      /* dechaine l'element du descripteur de reference */
      if (pRef->RdNext == NULL)
        {
          /* c'etait la seule reference, on la supprime */
          if (pRef->RdReferred)
            {
              pRef->RdReferred->ReFirstReference = NULL;
              DeleteAllReferences (pRef->RdReferred->ReReferredElem);
            }
        }
      else if (pRef->RdReferred)
        pRef->RdReferred->ReFirstReference = pRef->RdNext;
    }
  if (pRef->RdPrevious != NULL)
    pRef->RdPrevious->RdNext = pRef->RdNext;
  if (pRef->RdNext != NULL)
    pRef->RdNext->RdPrevious = pRef->RdPrevious;
  pRef->RdPrevious = NULL;
  pRef->RdNext = NULL;
  pRef->RdReferred = NULL;
}

/*----------------------------------------------------------------------
  CancelReference annule la reference de l'element pEl			
  (pEl doit etre terminal et de nature Refer).            
  ----------------------------------------------------------------------*/
void CancelReference (PtrElement pEl, PtrDocument pDoc)
{
  PtrElement          pAsc, pChild, pC1;
  PtrTextBuffer       pTxtBuf, pNextTxtBuf;
  PtrPathSeg          pPa, pNextPa;
  
  if (pEl != NULL)
    {
      if (pEl->ElSource == NULL)
        {
          if (pEl->ElReference != NULL && pEl->ElTerminal &&
              pEl->ElLeafType == LtReference)
            DeleteReference (pEl->ElReference);
        }
      else
        /* c'est une reference d'inclusion, on detruit tout le sous-arbre */
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
                    pTxtBuf = pEl->ElText;	   /* vide le 1er buffer */
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
                /* annule le contenu d'une polyline */
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
              case LtPath:
                if (pEl->ElFirstPathSeg)
                  {
                    pPa = pEl->ElFirstPathSeg;
                    pEl->ElFirstPathSeg = NULL;
                    /* met a jour le volume des elements ascendants */
                    pAsc = pEl->ElParent;
                    while (pAsc != NULL)
                      {
                        pAsc->ElVolume = pAsc->ElVolume - pEl->ElVolume;
                        pAsc = pAsc->ElParent;
                      }
                    pEl->ElVolume = 0;
                    /* libere les segments de path */
                    while (pPa != NULL)
                      {
                        pNextPa = pPa->PaNext;
                        FreePathSeg (pPa);
                        pPa = pNextPa;
                      }
                  }
                break;
              case LtSymbol:
              case LtGraphics:
                pEl->ElGraph = EOS;
                pEl->ElWideChar = 0;
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
void CopyReference (PtrReference pCopyRef, PtrReference pSourceRef, PtrElement *pEl)
{
  PtrReferredDescr    pRefD;

  pRefD = pCopyRef->RdReferred;
  pCopyRef->RdReferred = pSourceRef->RdReferred;
  pCopyRef->RdElement = *pEl;
  pCopyRef->RdTypeRef = pSourceRef->RdTypeRef;

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
  ----------------------------------------------------------------------*/
void TransferReferences (PtrElement pTarget, PtrDocument pDoc, PtrElement pEl,
                         PtrDocument pSourceDoc)
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
ThotBool SetReference (PtrElement pRefEl, PtrAttribute pRefAttr,
                       PtrElement pTargetEl, PtrDocument pDoc,
                       PtrDocument pTargetDoc, ThotBool ancestor,
                       ThotBool withAppEvent)
{
  PtrElement          pEl, pAsc;
  PtrSSchema          pSS;
  int                 typeNum;
  ThotBool            ret;
  PtrReference        pRef;
  NotifyOnTarget      notifyEl;

  ret = FALSE;
  if (pRefEl != NULL)
    {
      if (pRefEl->ElStructSchema->SsRule->SrElem[pRefEl->ElTypeNumber - 1]->SrRefImportedDoc)
        /* c'est une inclusion d'un document externe */
        if (pDoc == pTargetDoc)
          /* reference et element reference' sont dans le meme document */
          pTargetEl = NULL;	/* sort de la procedure sans rien faire d'autre */
    }
  if (pTargetEl != NULL && (pRefEl != NULL || pRefAttr != NULL))
    {
      if (!ancestor)
        pEl = pTargetEl;
      else
        /* cherche le type de l'element reference' prevu */
        {
          typeNum = 0;
          ReferredType (pRefEl, pRefAttr, &pSS, &typeNum, NULL);
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
              CancelReference (pRefEl, pDoc);

            }
          else
            {
              pRef = pRefAttr->AeAttrReference;
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
                  /* on ajoute la nouvelle reference en tete de la chaine des */
                  /* references a cet element */
                  pRef->RdNext = pEl->ElReferredDescr->ReFirstReference;
                  if (pRef->RdNext != NULL)
                    pRef->RdNext->RdPrevious = pRef;
                  pRef->RdReferred = pEl->ElReferredDescr;
                  pRef->RdReferred->ReFirstReference = pRef;
                }

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
  UpdateInclusionElements
  Up to date the value of inclusions that belong to the document
  pointed by pDoc.
  If removeExclusions is TRUE, the exclusions are removed from the documents
  opened temporarely.
  ----------------------------------------------------------------------*/
void UpdateInclusionElements (PtrDocument pDoc)
{
  PtrElement            pSource;
  PtrReference          pRef;
  PtrReferredDescr      pRefD;
  Document              doc;
  NotifyOnTarget	notifyEl;

  /* parcourt la chaine des descripteurs d'elements references */
  /* du document, pour traiter toutes les references */
  doc = IdentDocument (pDoc);
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
                pSource = ReferredElement (pRef);
                notifyEl.event = TteElemFetchInclude;
                notifyEl.target = (Element) pSource;
                notifyEl.targetdocument = doc;
                notifyEl.element = (Element) pRef->RdElement;
                notifyEl.document = doc;
                if (!CallEventType ((NotifyEvent *) & notifyEl, TRUE))
                  /* l'application accepte le traitement */ 
                  {
                    /* inclusion d'un document externe */
                    CopyIncludedElem (pRef->RdElement, pDoc);
		    
                    /* Envoyer le message post de TteElemFetchInclude */
                    pSource = ReferredElement (pRef);
                    notifyEl.event = TteElemFetchInclude;
                    notifyEl.target = (Element) pSource;
                    notifyEl.targetdocument = doc;
                    notifyEl.element = (Element) pRef->RdElement;
                    notifyEl.document = doc;
		    
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
}

