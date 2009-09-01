/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2009
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * This module handles structure modifications.
 *
 * Authors: V. Quint (INRIA)
 *          S. Bonhomme (INRIA) - Separation between structured and
 *                                unstructured editing modes
 *
 */

#include "thot_gui.h"
#include "thot_sys.h"

#include "constmedia.h"
#include "typemedia.h"
#include "res.h"
#include "language.h"
#include "libmsg.h"
#include "message.h"
#include "fileaccess.h"
#include "appdialogue.h"
#include "content.h"
#include "registry.h"
#include "application.h"

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "modif_tv.h"
#include "creation_tv.h"
#include "select_tv.h"
#include "edit_tv.h"
#include "appdialogue_tv.h"

#include "absboxes_f.h"
#include "appli_f.h"
#include "appdialogue_f.h"
#include "applicationapi_f.h"
#include "attributeapi_f.h"
#include "attributes_f.h"
#include "attrpresent_f.h"
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
#include "docs_f.h"
#include "editcommands_f.h"
#include "exceptions_f.h"
#include "externalref_f.h"
#include "fileaccess_f.h"
#include "frame_f.h"
#include "memory_f.h"
#include "presrules_f.h"
#include "references_f.h"
#include "structselect_f.h"
#include "structschema_f.h"
#include "search_f.h"
#include "searchref_f.h"
#include "structcreation_f.h"
#include "structmodif_f.h"
#include "textcommands_f.h"
#include "thotmsg_f.h"
#include "tree_f.h"
#include "undo_f.h"
#include "views_f.h"

/*----------------------------------------------------------------------
  InitSave     initialise les variables decrivant la partie copiee
  ----------------------------------------------------------------------*/
void                InitSave ()
{
  /* pointeur sur le premier element sauve' */
  FirstSavedElement = NULL;
  /* le document d'ou` vient cette partie */
  DocOfSavedElements = NULL;
}

/*----------------------------------------------------------------------
  IsASavedElement   retourne vrai si l'element pointe par pEl se  
  trouve dans le buffer de Couper-Copier-Coller.                  
  ----------------------------------------------------------------------*/
ThotBool            IsASavedElement (PtrElement pEl)
{
  PtrElement          pE;
  PtrPasteElem        pPasteD;
  ThotBool            ret;

  ret = FALSE;
  if (pEl != NULL)
    {
      pE = pEl;
      /* cherche la racine de l'arbre auquel appartient l'element */
      while (pE->ElParent != NULL)
        pE = pE->ElParent;
      /* cherche si cette racine est dans la suite des elements sauves */
      pPasteD = FirstSavedElement;
      while (pPasteD != NULL && pPasteD->PeElement != pE)
        pPasteD = pPasteD->PeNext;
      if (pPasteD != NULL)
        if (pPasteD->PeElement == pE)
          /* l'element est dans les elements sauves */
          ret = TRUE;
    }
  return ret;
}

/*----------------------------------------------------------------------
  CopyLeaf                                                  
  ----------------------------------------------------------------------*/
static void CopyLeaf (PtrCopyDescr pCopyD, ThotBool redisplay)
{
  PtrTextBuffer       pBuf, pBufS;
  PtrDocument         pDocPav;
  int                 frame, pageHeight;

  /* parcourt la chaine de tous les descripteurs de copie de l'element */
  while (pCopyD != NULL)
    {
      if (pCopyD->CdCopiedAb != NULL)
        /* traite un pave' qui copie l'element */
        {
          /* detruit tous les buffers du pave qui copie */
          pBuf = pCopyD->CdCopiedAb->AbText;
          pCopyD->CdCopiedAb->AbText = NULL;
          while (pBuf != NULL)
            {
              pBufS = pBuf->BuNext;
              FreeTextBuffer (pBuf);
              pBuf = pBufS;
            }
          pCopyD->CdCopiedAb->AbVolume = 0;
          /* copie a nouveau l'element dans le pave */
          pDocPav = DocumentOfElement (pCopyD->CdCopiedAb->AbElement);
          ApplyCopy (pDocPav, pCopyD->CdCopyRule, pCopyD->CdCopiedAb, FALSE);
          pCopyD->CdCopiedAb->AbChange = TRUE;
          /* reaffiche le pave' mis a jour */
          frame = pDocPav->DocViewFrame[pCopyD->CdCopiedAb->AbDocView - 1];
          /* on ne s'occupe pas de la hauteur de page */
          pageHeight = 0;
          ChangeConcreteImage (frame, &pageHeight, pCopyD->CdCopiedAb);
          if (redisplay)
            DisplayFrame (frame);
          if (!pCopyD->CdCopiedAb->AbPresentationBox &&
              pCopyD->CdCopiedAb->AbElement->ElCopyDescr != NULL)
            /* le pave qui copie est le pave principal d'un element qui
               est lui-meme copie' */
            CopyLeaf (pCopyD->CdCopiedAb->AbElement->ElCopyDescr, redisplay);
        }
      /* copie suivante du meme element */
      pCopyD = pCopyD->CdNext;
    }
}


/*----------------------------------------------------------------------
  RedisplayCopies 						
  Si l'element pointe' par pEl est (ou est contenudans) un	
  element copie' par une regle de presentation Copy, reaffiche    
  tous les paves qui le copient.                        		
  Met egalement a jour toutes les inclusions qui copient          
  l'element. pDoc designe le document auquel appartient		
  l'element pointe' par pEl.                                      
  ----------------------------------------------------------------------*/
void RedisplayCopies (PtrElement pEl, PtrDocument pDoc, ThotBool redisplay)
{
  PtrReference        pRef;

  if (pEl != NULL)
    /* remonte jusqu'a la racine de l'arbre abstrait auquel appartient
       l'element traite' et cherche toutes les copies de ses ascendants */
    do
      {
        if (pEl->ElCopyDescr != NULL)
          /* refait toutes les copies de l'element */
          CopyLeaf (pEl->ElCopyDescr, redisplay);
        /* traite les inclusions de l'element */
        if (pEl->ElReferredDescr != NULL)
          /* cet element est reference' */
          {
            /* cherche toutes les references a cet element qui se trouvent
               dans un document charge' */
            pRef = NULL;
            do
              {
                pRef = NextReferenceToEl (pEl, pDoc, pRef);
                if (pRef  && pRef->RdTypeRef == RefInclusion)
                  /* c'est une inclusion */
                  if (pRef->RdElement && pRef->RdElement->ElSource)
                    UpdateIncludedElement (pRef->RdElement, pDoc);
              }
            while (pRef != NULL);
          }
        /* passe a l'element ascendant */
        pEl = pEl->ElParent;
      }
    while (pEl != NULL);
}

/*----------------------------------------------------------------------
  IsANewElement   retourne vrai si l'element pointe' par pEl est  
  l'un des nouveaux elements crees                                
  ----------------------------------------------------------------------*/
ThotBool IsANewElement (PtrElement pEl)
{
  int                 i;
  ThotBool            ret;

  ret = FALSE;
  for (i = 0; i < NCreatedElements && !ret; i++)
    if (CreatedElement[i] == pEl)
      ret = TRUE;
  return ret;
}

/*----------------------------------------------------------------------
  IsWithinANewElement    retourne vrai si l'element pEl est   	
  dans l'un des sous-arbres qu'on vient de creer                  
  ----------------------------------------------------------------------*/
ThotBool            IsWithinANewElement (PtrElement pEl)
{
  ThotBool            ret;

  if (pEl == NULL)
    ret = FALSE;
  else if (IsANewElement (pEl))
    ret = TRUE;
  else
    ret = IsWithinANewElement (pEl->ElParent);
  return ret;
}


/*----------------------------------------------------------------------
  UpdateRefAttributes 						
  Parcourt tout le sous-arbre de racine pRoot et, pour chaque	
  element qui est reference', fait recalculer la presentation     
  des attributs-reference qui pointent sur lui.			
  ----------------------------------------------------------------------*/
void UpdateRefAttributes (PtrElement pRoot, PtrDocument pDoc)
{
  PtrReference        pRef;
  PtrElement          pChild;

  if (pRoot->ElReferredDescr != NULL)
    /* l'element est reference' */
    {
      pRef = pRoot->ElReferredDescr->ReFirstReference;
      /* 1ere reference a l'element */
      while (pRef != NULL)
        /* parcourt les references a l'element */
        {
          if (pRef->RdAttribute != NULL)
            /* c'est une reference par attribut */
            if (!IsWithinANewElement (pRef->RdElement))
              /* l'element qui reference n'est pas dans les nouveaux
                 elements. s'il est dans le tampon, on n'y touche pas */
              if (!IsASavedElement (pRef->RdElement))
                /* on recalcule la presentation de l'element qui reference*/
                UpdatePresAttr (pRef->RdElement, pRef->RdAttribute,
                                pRef->RdElement, pDoc, FALSE, FALSE, NULL);
          /* reference suivante au meme element */
          pRef = pRef->RdNext;
        }
    }
  if (!pRoot->ElTerminal)
    /* element non terminal, on traite tous les fils */
    {
      pChild = pRoot->ElFirstChild;
      while (pChild != NULL)
        {
          UpdateRefAttributes (pChild, pDoc);
          pChild = pChild->ElNext;
        }
    }
}

/*----------------------------------------------------------------------
  AssignPairIdentifiers						
  Parcourt tout le sous-arbre de racine pRoot et, pour chaque     
  element de paire recalcule l'identificateur unique de la paire. 
  ----------------------------------------------------------------------*/
void AssignPairIdentifiers (PtrElement pRoot, PtrDocument pDoc)
{
  PtrElement          pPairEl, pChild;

  if (pRoot->ElTerminal)
    {
      if (pRoot->ElLeafType == LtPairedElem)
        if (pRoot->ElStructSchema->SsRule->SrElem[pRoot->ElTypeNumber - 1]->SrFirstOfPair)
          /* c'est un element de debut de paire */
          {
            /* on cherche son element de fin */
            pPairEl = GetOtherPairedElement (pRoot);
            /* calcule un nouvel identificateur */
            pDoc->DocMaxPairIdent++;
            pRoot->ElPairIdent = pDoc->DocMaxPairIdent;
            if (pPairEl != NULL)
              pPairEl->ElPairIdent = pRoot->ElPairIdent;
          }
    }
  else
    /* element non terminal, on traite tous les fils */
    {
      pChild = pRoot->ElFirstChild;
      while (pChild != NULL)
        {
          AssignPairIdentifiers (pChild, pDoc);
          pChild = pChild->ElNext;
        }
    }
}


/*----------------------------------------------------------------------
  RedisplayEmptyReferences					
  reaffiche sous la forme [?] tous les paves correspondant a une  
  reference a l'element pointe' par pEl ou a un element de son	
  sous-arbre et supprime la presentation de tous les attributs	
  references qui pointent sur cet element ou un element de son	
  sous-arbre.							
  ----------------------------------------------------------------------*/
void RedisplayEmptyReferences (PtrElement pEl, PtrDocument *pDoc,
                               ThotBool redisplay)
{
  PtrReference        pRef;
  PtrElement          pElRef, pChild;
  PtrAbstractBox      pAb;
  PtrAttribute        pAttr, pPrevAttr;
  int                 view, frame, h;
  ThotBool            stop;

  if (pEl->ElReferredDescr != NULL)
    /* cet element est reference' */
    {
      pRef = NULL;
      pRef = NextReferenceToEl (pEl, *pDoc, pRef);
      /* parcourt la chaine des elements qui le referencent */
      while (pRef)
        {
          pElRef = pRef->RdElement;
          /* un element qui reference */
          if (pElRef)
            {
              if (pRef->RdAttribute != NULL)
                /* reference par attribut */
                /* retire la presentation de cet attribut et reaffiche */
                {
                  /* cherche d'abord cet attribut parmi ceux de l'element */
                  pPrevAttr = NULL;
                  pAttr = pElRef->ElFirstAttr;
                  stop = FALSE;
                  do
                    {
                      if (pAttr == NULL)
                        stop = TRUE;
                      else if (pAttr == pRef->RdAttribute)
                        stop = TRUE;	/* c'est l'attribut cherche' */
                      else
                        {
                          /* passe a l'attribut suivant de l'element */
                          pPrevAttr = pAttr;
                          pAttr = pAttr->AeNext;
                        }
                    }
                  while (!stop);
                  /* retire provisoirement l'attribut de l'element */
                  if (pAttr != NULL)
                    {
                      if (pPrevAttr == NULL)
                        pElRef->ElFirstAttr = pAttr->AeNext;
                      else
                        pPrevAttr->AeNext = pAttr->AeNext;
                    }
                  /* recalcule la presentation de l'element sans l'attribut */
                  UpdatePresAttr (pElRef, pRef->RdAttribute, pElRef, *pDoc,
                                  TRUE, FALSE, NULL);
                  AbstractImageUpdated (*pDoc);
                  if (redisplay)
                    RedisplayDocViews (*pDoc);
                  /* remet l'attribut a l'element */
                  if (pAttr != NULL)
                    {
                      if (pPrevAttr == NULL)
                        pElRef->ElFirstAttr = pAttr;
                      else
                        pPrevAttr->AeNext = pAttr;
                    }
                }
              else if (pElRef->ElTerminal && pElRef->ElLeafType == LtReference)
                /* si c'est une inclusion, on ne fait rien */
                /* reference par element reference */
                {
                  /* parcourt toutes les vues */
                  for (view = 0; view < MAX_VIEW_DOC; view++)
                    if (pElRef->ElAbstractBox[view] != NULL)
                      /* l'element qui reference a un pave dans cette vue */
                      {
                        pAb = pElRef->ElAbstractBox[view];
                        if (pAb->AbLeafType == LtText)
                          /* change le contenu du pave reference */
                          {
                            pAb->AbText->BuContent[0] = '[';
                            pAb->AbText->BuContent[1] = '?';
                            pAb->AbText->BuContent[2] = ']';
                            pAb->AbText->BuContent[3] = EOS;
                            pAb->AbText->BuLength = 3;
                            pAb->AbVolume = 3;
                            /* reaffiche le pave */
                            pAb->AbChange = TRUE;
                            frame = (*pDoc)->DocViewFrame[view];
                            h = 0;
                            /* on ignore la hauteur de page */
                            ChangeConcreteImage (frame, &h,
                                                 pElRef->ElAbstractBox[view]);
                            if (redisplay)
                              DisplayFrame (frame);
                          }
                      }
                  RedisplayCopies (pElRef, *pDoc, redisplay);
                }
            }
          /* reference suivante */
          pRef = NextReferenceToEl (pEl, *pDoc, pRef);
        }
    }
  /* traite les fils de l'element */
  if (!pEl->ElTerminal)
    {
      pChild = pEl->ElFirstChild;
      while (pChild != NULL)
        {
          RedisplayEmptyReferences (pChild, pDoc, redisplay);
          pChild = pChild->ElNext;
        }
    }
}

/*----------------------------------------------------------------------
  TtcHolophrast   holophraste les elements selectionnes           
  ----------------------------------------------------------------------*/
void TtcHolophrast (Document document, View view)
{
  PtrElement          firstSel, lastSel, pEl;
  PtrDocument         pDoc;
  int                 firstChar, lastChar;
  ThotBool            done, canHolophrast;

  /* y-a-t'il une selection courante ? */
  if (!GetCurrentSelection (&pDoc, &firstSel, &lastSel, &firstChar, &lastChar))
    return;
  else
    {
      /* eteint la selection */
      TtaClearViewSelections ();
      /* si la racine d'un arbre abstrait est selectionne'e, on prend tous
         les fils du premier niveau */
      if (firstSel != NULL)
        if (firstSel->ElParent == NULL)
          if (!firstSel->ElTerminal && firstSel->ElFirstChild != NULL)
            {
              firstSel = firstSel->ElFirstChild;
              lastSel = firstSel;
              while (lastSel->ElNext != NULL)
                lastSel = lastSel->ElNext;
            }
      /* premier element selectionne */
      pEl = firstSel;
      /* traite successivement tous les elements de la selection */
      done = FALSE;
      while (pEl != NULL)
        {
          /* on n'holophraste pas les sauts de page */
          if (!pEl->ElTerminal || pEl->ElLeafType != LtPageColBreak)
            /* on n'holophraste pas certains elements de tableau */
            {
              canHolophrast = TRUE;
              if (canHolophrast)
                {
                  done = TRUE;
                  /* detruit les paves de l'element courant */
                  DestroyAbsBoxes (pEl, pDoc, TRUE);
                  /* signale au mediateur les paves detruits pour qu'il */
                  /* detruise les boites correspondantes */
                  AbstractImageUpdated (pDoc);
                  /* marque l'element comme holophraste' */
                  pEl->ElHolophrast = TRUE;
                  /* cree tous les paves de l'element */
                  CreateNewAbsBoxes (pEl, pDoc, 0);
                }
            }
          /* passe a l 'element suivant de la selection */
          pEl = NextInSelection (pEl, lastSel);
        }
      /* reaffiche toutes les vues */
      AbstractImageUpdated (pDoc);
      RedisplayDocViews (pDoc);
      /* rallume toute la selection */
      HighlightSelection (FALSE, TRUE);
      /* met a jour les menus qui dependent de la selection dans toutes */
      /* les vues ouvertes */
      PrepareSelectionMenu ();
      if (ThotLocalActions[T_chattr] != NULL)
        (*(Proc1)ThotLocalActions[T_chattr]) ((void *)pDoc);
    }
}

/*----------------------------------------------------------------------
  DeHolophrast   deholophraste l'element pEl du document pDoc.    
  ----------------------------------------------------------------------*/
void DeHolophrast (PtrElement pEl, PtrDocument pDoc)
{
  /* detruit les paves de l'element */
  DestroyAbsBoxes (pEl, pDoc, TRUE);
  /* signale au mediateur les paves detruits pour qu'il detruise les boites
     correspondantes */
  AbstractImageUpdated (pDoc);
  pEl->ElHolophrast = FALSE;
  /* cree tous les paves de l'element */
  CreateNewAbsBoxes (pEl, pDoc, 0);
  /* reaffiche toutes les vues */
  AbstractImageUpdated (pDoc);
  RedisplayDocViews (pDoc);
}

/*----------------------------------------------------------------------
  CompleteElement cree dans l'element pEl les elements absents    
  obligatoires.                                           
  Retourne TRUE si au moins un element a ete cree', FALSE 
  si rien n'a ete cree'.                                  
  ----------------------------------------------------------------------*/
ThotBool CompleteElement (PtrElement pEl, PtrDocument pDoc)
{
  PtrElement          pComponent, pNewEl, pLastEl, pPrev, pClose;
  PtrElement          pSibling, pPage;
  PtrSRule            pSRule;
  NotifyElement       notifyEl;
  Document            doc;
  int                 nElExist, nElems, min, comp, NSiblings;
  ThotBool            ret, found;

  ret = FALSE;
  /* regle definissant le type de l'element dans son schema de structure */
  pSRule = pEl->ElStructSchema->SsRule->SrElem[pEl->ElTypeNumber - 1];
  while (pSRule->SrConstruct == CsIdentity)
    pSRule = pEl->ElStructSchema->SsRule->SrElem[pSRule->SrIdentRule - 1];
  /* traitement selon le constructeur : seuls les agregats et les listes */
  /* demandent un traitement. */
  doc = IdentDocument (pDoc);
  switch (pSRule->SrConstruct)
    {
    case CsAggregate:
    case CsUnorderedAggregate:
      /* C'est un agregat, creer les composants obligatoires absents */
      pPrev = NULL;
      /* premier composant existant de l'agregat */
      pComponent = pEl->ElFirstChild;
      /* on verifie tous les composants indique's dans la regle de */
      /* structure */
      for (comp = 0; comp < pSRule->SrNComponents; comp++)
        /* on ne s'occupe pas des composants optionnels */
        if (!pSRule->SrOptComponent[comp])
          /* on ne cree pas les composants qui ont l'exception NoCreate */
          if (!TypeHasException (ExcNoCreate,
                                 pSRule->SrComponent[comp],
                                 pEl->ElStructSchema))
            /* on ne cree pas les composants exclus */
            if (!ExcludedType (pEl, pSRule->SrComponent[comp],
                               pEl->ElStructSchema))
              {
                /* on saute les marques de page entre composants */
                FwdSkipPageBreak (&pComponent);
                /* cherche si ce composant existe deja dans l'element */
                found = FALSE;
                if (pComponent != NULL)
                  /* il y a au moins un element fils dans l'agregat */
                  {
                    if (pSRule->SrConstruct == CsAggregate)
                      /* c'est un agregat ordonne' */
                      {
                        if (EquivalentSRules (pSRule->SrComponent[comp],
                                              pEl->ElStructSchema,
                                              pComponent->ElTypeNumber,
                                              pComponent->ElStructSchema,
                                              pEl))
                          /* le composant cherche' est a sa place */
                          {
                            found = TRUE;
                            /* passe a l'element fils suivant */
                            pPrev = pComponent;
                            pComponent = pComponent->ElNext;
                          }
                      }
                    if (pSRule->SrConstruct == CsUnorderedAggregate)
                      /* agregat sans ordre, cherche parmi tous les */
                      /* elements fils de l'agregat */
                      {
                        pComponent = pEl->ElFirstChild;
                        while (pComponent != NULL && !found)
                          {
                            if (EquivalentSRules (pSRule->SrComponent[comp],
                                                  pEl->ElStructSchema,
                                                  pComponent->ElTypeNumber,
                                                  pComponent->ElStructSchema,
                                                  pEl))
                              /* c'est le composant cherche' */
                              {
                                found = TRUE;
                                pPrev = pComponent;
                              }
                            /* passe a l'element fils suivant */
                            pComponent = pComponent->ElNext;
                          }
                        if (!found)
                          pComponent = pEl->ElFirstChild;
                      }
                    if (!found)
                      {
                        if (pPrev == NULL &&
                            pComponent->ElPrevious != NULL)
                          {
                            pPage = pComponent->ElPrevious;
                            if (pPage->ElTerminal)
                              if (pPage->ElLeafType == LtPageColBreak)
                                if (pPage->ElPageType == PgBegin)
                                  /* on inserera apres les
                                     sauts de pages du debut */
                                  pPrev = pPage;
                          }
                      }
                  }
                else if (pPrev == NULL && pEl->ElParent == NULL)
                  /* on est sur l'element racine et il est vide */
                  {
                    pComponent = pEl->ElFirstChild;
                    SkipPageBreakBegin (&pComponent);
                    pPrev = pComponent;
                    /* on inserera apres les sauts de pages du debut */
                  }
                if (!found)
                  /* le composant cherche' n'existe pas */
                  {
                    /* envoie l'evenement TteElemNew pour la
                       creation de ce composant */
                    notifyEl.event = TteElemNew;
                    notifyEl.document = doc;
                    notifyEl.element = (Element) pEl;
                    notifyEl.info = 0; /* not sent by undo */
                    notifyEl.elementType.ElTypeNum = pSRule->SrComponent[comp];
                    notifyEl.elementType.ElSSchema = (SSchema) (pEl->ElStructSchema);
                    NSiblings = 0;
                    if (pPrev != NULL)
                      {
                        pSibling = pPrev;
                        NSiblings++;
                        while (pSibling->ElPrevious != NULL)
                          {
                            NSiblings++;
                            pSibling = pSibling->ElPrevious;
                          }
                      }
                    notifyEl.position = NSiblings;
                    pNewEl = NULL;
                    if (!CallEventType ((NotifyEvent *) (&notifyEl), TRUE))
                      /* l'application est d'accord pour la
                         creation du composant absent. On le
                         cree avec sa descendance */
                      pNewEl = NewSubtree (pSRule->SrComponent[comp],
                                           pEl->ElStructSchema, pDoc,
                                           TRUE, TRUE, TRUE, TRUE);
                    if (pNewEl != NULL)
                      /* on met l'element cee' dans l'arbre */
                      {
                        ret = TRUE;
                        if (pPrev == NULL)
                          /* c'est le 1er composant de l'agregat */
                          {
                            InsertFirstChild (pEl, pNewEl);
                            pClose = SiblingElement (pNewEl, FALSE);
                            if (pClose)
                              /* l'element suivant le nouvel element n'est plus premier */
                              ChangeFirstLast (pClose, pDoc, TRUE, TRUE);
                          }
                        else
                          {
                            InsertElementAfter (pPrev, pNewEl);
                            if (!SiblingElement (pNewEl, FALSE))
                              /* l'element precedent le nouvel element n'est plus dernier */
                              ChangeFirstLast (pPrev, pDoc, FALSE, TRUE);
                          }
                        /* traite les exclusions des elements crees */
                        RemoveExcludedElem (&pNewEl, pDoc);
                        if (pNewEl != NULL)
                          {
                            /* traite les attributs requis des
                               elements crees */
                            AttachMandatoryAttributes (pNewEl, pDoc);
                            if (pDoc->DocSSchema != NULL)
                              /* le document n'a pas ete ferme' entre temps */
                              {
                                pPrev = pNewEl;
                                /* traitement des exceptions */
                                CreationExceptions (pNewEl, pDoc);
                                NotifySubTree (TteElemNew, pDoc, pNewEl, 0, 0,
                                               FALSE, FALSE);
                              }
                          }
                      }
                  }
              }
      break;

    case CsList:
      /* C'est une liste, on cree le nombre d'elements minimum
         indique' dans la regle de structure */
      if (!TypeHasException (ExcNoCreate, pSRule->SrListItem,
                             pEl->ElStructSchema))
        if (!ExcludedType (pEl, pSRule->SrListItem,
                           pEl->ElStructSchema))
          {
            /* On compte d'abord les elements existants : nElExist */
            pComponent = pEl->ElFirstChild;
            nElExist = 0;
            pLastEl = NULL;
            while (pComponent != NULL)
              {
                if (pComponent->ElTerminal &&
                    pComponent->ElLeafType == LtPageColBreak)
                  /* ignore les marques page */
                  {
                    /* si c'est une page de debut d'element, on
                       creera les nouveaux elements de liste apres
                       cette marque de page */
                    if (pComponent->ElPageType == PgBegin)
                      pLastEl = pComponent;
                  }
                else
                  {
                    /* ignore les elements qui ne sont pas du type
                       (ou equivalent) prevu par la regle liste. Les
                       inclusions peuvent produire de tels elements */
                    if (EquivalentSRules (pSRule->SrListItem,
                                          pEl->ElStructSchema,
                                          pComponent->ElTypeNumber,
                                          pComponent->ElStructSchema, pEl))
                      nElExist++;
                    pLastEl = pComponent;
                  }
                pComponent = pComponent->ElNext;
              }
            if (pSRule->SrMinItems == 0)
              /* pas de minimum indique'. Le minimum est 1 */
              min = 1;
            else
              min = pSRule->SrMinItems;
            if (nElExist < min)
              /* il manque des element, on les cree */
              /* pLastEl pointe sur le dernier element existant de la liste */
              for (nElems = nElExist; nElems < min; nElems++)
                {
                  /* envoie l'evenement de creation d'un element */
                  notifyEl.event = TteElemNew;
                  notifyEl.document = doc;
                  notifyEl.element = (Element) pEl;
                  notifyEl.info = 0; /* not sent by undo */
                  notifyEl.elementType.ElTypeNum = pSRule->SrListItem;
                  notifyEl.elementType.ElSSchema = (SSchema) (pEl->ElStructSchema);
                  NSiblings = 0;
                  if (pLastEl != NULL)
                    {
                      pSibling = pLastEl;
                      NSiblings++;
                      while (pSibling->ElPrevious != NULL)
                        {
                          NSiblings++;
                          pSibling = pSibling->ElPrevious;
                        }
                    }
                  notifyEl.position = NSiblings;
                  pComponent = NULL;
                  if (!CallEventType ((NotifyEvent *) (&notifyEl), TRUE))
                    /* l'application ne s'y oppose pas, on cree
                       l'element */
                    pComponent = NewSubtree (pSRule->SrListItem,
                                             pEl->ElStructSchema, pDoc,
                                             TRUE, TRUE, TRUE, TRUE);
                  if (pComponent != NULL)
                    /* on insere le nouvel element dans l'arbre */
                    {
                      ret = TRUE;
                      if (pLastEl == NULL)
                        /* la liste est vide, on insere en tete */
                        {
                          InsertFirstChild (pEl, pComponent);
                          pClose = SiblingElement (pComponent, FALSE);
                          if (pClose != NULL)
                            /* l'element suivant le nouvel	element n'est plus premier */
                            ChangeFirstLast (pClose, pDoc, TRUE, TRUE);
                        }
                      else
                        {
                          InsertElementAfter (pLastEl, pComponent);
                          pClose = SiblingElement (pComponent, FALSE);
                          if (pClose == NULL)
                            /* l'element precedent le nouvel element n'est plus dernier */
                            ChangeFirstLast (pLastEl, pDoc, FALSE, TRUE);
                        }
                      /* traite les exclusions des elements crees */
                      RemoveExcludedElem (&pComponent, pDoc);
                      if (pComponent != NULL)
                        {
                          pLastEl = pComponent;
                          /* traite les attributs requis des
                             elements crees */
                          AttachMandatoryAttributes (pComponent, pDoc);
                          if (pDoc->DocSSchema != NULL)
                            /* le document n'a pas ete ferme'
                               entre temps */
                            {
                              /* traitement des exceptions */
                              CreationExceptions (pComponent, pDoc);
                              NotifySubTree (TteElemNew, pDoc, pComponent,
                                             0, 0, FALSE, FALSE);
                            }
                        }
                    }
                }
          }
      break;

    default:
      break;
    }
  return ret;
}


/*----------------------------------------------------------------------
  BreakElement
  Split the pSplitEl element and complete to replicate the pElReplicate
  element.
  Return TRUE if okay
  select: select the new element
  ----------------------------------------------------------------------*/
ThotBool BreakElement (PtrElement pElReplicate, PtrElement pSplitEl,
                       int splitIndex, ThotBool block, ThotBool select)
{
  PtrElement      firstSel, lastSel, pAncest, pE, pNextEl, pPrevEl, pPrev,
    pNext, pChild, pEl2, pEl, pCompleteElem, pSibling, pClose;
  PtrElement      *list;
  PtrDocument     pDoc;
  NotifyElement   notifyEl;
  int             firstChar, lastChar, NSiblings, nextChar, view;
  int             nbEl, i, j;
  ThotBool        ret, ok;
  DisplayMode     dispMode;
  Document        doc;

  ret = FALSE;
  NSiblings = 0;
  CloseTextInsertion ();
  pAncest = NULL;
  /* y-a-t'il une selection courante ? */
  if (!GetCurrentSelection (&pDoc, &firstSel, &lastSel, &firstChar, &lastChar))
    return ret;
  if (!pDoc->DocReadOnly)
    {
      doc = IdentDocument (pDoc);
      if (pSplitEl)
        {
          firstSel = pSplitEl;
          firstChar = splitIndex;
        }
      if (pElReplicate)
        {
          if (pElReplicate->ElTypeNumber == pElReplicate->ElStructSchema->SsRootElem)
            /* that's the root element. Don't duplicate it */
            ok = FALSE;
          else
            {
              ok = TRUE;
              pAncest = pElReplicate->ElParent;
              pEl = firstSel;
              if (firstChar == 0 ||
                  (pEl->ElTerminal && pEl->ElLeafType != LtText &&
                   firstChar == 1))
                /* la selection est en debut d'element */
                {
                  /* tant qu'il n'y a pas de frere precedent, on remonte au
                     pere */
                  while (pEl->ElParent != NULL && pEl->ElPrevious == NULL)
                    pEl = pEl->ElParent;
                  if (pEl->ElPrevious == NULL)
                    /* il n'y a pas de frere precedent,on ne peut pas diviser*/
                    ok = FALSE;
                }
            }
        }
      else
        ok = CanSplitElement (firstSel, firstChar, block, &pAncest, &pEl,
                              &pElReplicate);
      if (ok)
        {
          /* demande a l'application si on peut creer ce type d'element */
          notifyEl.event = TteElemNew;
          notifyEl.document = doc;
          notifyEl.element = (Element) (pElReplicate->ElParent);
          notifyEl.info = 0; /* not sent by undo */
          notifyEl.elementType.ElTypeNum = pElReplicate->ElTypeNumber;
          notifyEl.elementType.ElSSchema = (SSchema) pElReplicate->ElStructSchema;
          pSibling = pElReplicate;
          NSiblings = 1;
          while (pSibling->ElPrevious != NULL)
            {
              NSiblings++;
              pSibling = pSibling->ElPrevious;
            }
          notifyEl.position = NSiblings;
          if (CallEventType ((NotifyEvent *) (&notifyEl), TRUE))
            /* l'application refuse */
            pAncest = NULL;
        }
      if (pAncest != NULL)
        {
          pPrev = pEl->ElPrevious;
          TtaClearViewSelections ();	/* cancel current selection */
          pNext = pEl;
          nextChar = firstChar;

          AddEditOpInHistory (pElReplicate, pDoc, TRUE, TRUE);
          /* we may need need to split a text leaf */
          if (pEl->ElTerminal)
            {
              pPrev = pEl;
              if (pEl->ElLeafType == LtText)
                /* selection is within a text leaf */
                {
                  if (pEl->ElNext &&
                      (firstChar == 0 || firstChar > pEl->ElTextLength))
                    /* the selection is at the end of the text leaf or
                       within an empty text leaf */
                    {
                      pNext = pEl->ElNext;
                      nextChar = 1;
                    }
                  else if (firstChar < 1)
                    /* the selection is at the beginning of the text leaf */
                    {
                      nextChar = 1;
                    }
                  else
                    /* the selection is within the text leaf */
                    {
                      SplitTextElement (pEl, firstChar, pDoc, TRUE,
                                        &pNext, TRUE);
                      if (pEl == NULL || pEl->ElStructSchema == NULL)
                        /* pEl has been deleted by the application.
                           we are lost! */
                        return ret;
                      else
                        /* modifie le volume des paves contenant la 1ere
                           partie du texte */
                        for (view = 0; view < MAX_VIEW_DOC; view++)
                          if (pNext && pNext->ElTextLength > 0)
                            UpdateAbsBoxVolume (pEl, view, pDoc);
                      nextChar = 1;
                    }
                }
              else if (pEl->ElLeafType == LtPicture && firstChar > 0 &&
                       pEl->ElNext != NULL )
                {
                  pNext = pEl->ElNext;
                  if (pNext->ElTerminal && pNext->ElLeafType == LtText)
                    nextChar = 1;
                  else
                    nextChar = 0;
                }
            }
          else if (firstChar > 0 && pEl->ElNext != NULL &&
                   TypeHasException (ExcIsImg, pEl->ElTypeNumber,
                                     pEl->ElStructSchema))
            /* end of an HTML <img> with a following sibling */
            {
              if (firstChar > 0 && firstSel->ElNext)
                /* we are after the image. Split before the next
                   sibling */
                {
                  pPrev = pEl;
                  pNext = pEl->ElNext;
                  if (pNext->ElTerminal && pNext->ElLeafType == LtText)
                    nextChar = 1;
                  else
                    nextChar = 0;
                }
            }

          pClose = SiblingElement (pPrev, FALSE);
          /* enregistre les elements preexistants */
          pE = pPrev->ElNext;
          nbEl = 0;
          while (pE != NULL)
            {
              nbEl++;
              pE = pE->ElNext;
            }
          list = (PtrElement *) TtaGetMemory (nbEl * sizeof (PtrElement));
          pE = pPrev->ElNext;
          nbEl = 0;
          while (pE != NULL)
            {
              list[nbEl++] = pE;
              pE = pE->ElNext;
            }
	  
          /* detruit les paves des elements qui vont etre deplaces au */
          /* niveau le plus bas. La notification n'est faite que sur  */
          /* les elements preexistants */
          pE = pPrev->ElNext;
          pPrevEl = NULL;
          i = 0;
          while (pE != NULL)
            {
              DestroyAbsBoxes (pE, pDoc, TRUE);
              pNextEl = pE->ElNext;
              /* signale a l'application qu'on va retirer l'element */
              j = i;
              while (j < nbEl && list[j] != pE)
                j++;
              if (j < nbEl)
                {
                  notifyEl.event = TteElemDelete;
                  notifyEl.document = doc;
                  notifyEl.element = (Element) pE;
                  notifyEl.info = 0; /* not sent by undo */
                  notifyEl.elementType.ElTypeNum = pE->ElTypeNumber;
                  notifyEl.elementType.ElSSchema = (SSchema)pE->ElStructSchema;
                  pSibling = pE;
                  NSiblings = 0;
                  while (pSibling->ElPrevious != NULL)
                    {
                      NSiblings++;
                      pSibling = pSibling->ElPrevious;
                    }
                  notifyEl.position = TTE_TOOLKIT_DELETE;
                  if (CallEventType ((NotifyEvent *) (&notifyEl), TRUE))
                    /* l'application refuse de continuer */
                    {
                      CancelLastEditFromHistory (pDoc);
                      return (ret);
                    }
                }
              if (pE->ElStructSchema != NULL)
                {
                  RemoveElement (pE);
                  /* signale a l'application qu'on a retire' un element */
                  if (j < nbEl)
                    {
                      notifyEl.element = (Element) pPrev->ElParent;
                      notifyEl.position = NSiblings;
                      notifyEl.info = 2; /* element removed by BreakElement */
                      CallEventType ((NotifyEvent *) (&notifyEl), FALSE);
                    }
                  if (pPrevEl != NULL)
                    InsertElementAfter (pPrevEl, pE);
                  pPrevEl = pE;
                }
              /* previous managed item in the list */
              if (j < nbEl)
                i = j;
              pE = pNextEl;
            }
          TtaFreeMemory (list);
          AbstractImageUpdated (pDoc);
          if (pClose != NULL)
            /* l'element pPrev devient le dernier fils de son pere */
            ChangeFirstLast (pPrev, pDoc, FALSE, FALSE);
          /* Reconstruction d'une structure parallele */
          pE = pPrev->ElParent;
          pEl2 = pE;
          pChild = pNext;
          while ((pE != NULL) && (pE != pAncest))
            {
              if (pE != pElReplicate)
                {
                  /* signale a l'application qu'on va creer un element */
                  notifyEl.event = TteElemNew;
                  notifyEl.document = doc;
                  notifyEl.element = (Element) (pE->ElParent);
                  notifyEl.info = 0; /* not sent by undo */
                  notifyEl.elementType.ElTypeNum = pE->ElTypeNumber;
                  notifyEl.elementType.ElSSchema = (SSchema)pE->ElStructSchema;
                  pSibling = pE;
                  NSiblings = 1;
                  while (pSibling->ElPrevious != NULL)
                    {
                      NSiblings++;
                      pSibling = pSibling->ElPrevious;
                    }
                  notifyEl.position = NSiblings;
                  if (CallEventType ((NotifyEvent *) (&notifyEl), TRUE))
                    /* l'application refuse de continuer */
                    {
                      CancelLastEditFromHistory (pDoc);
                      return (ret);
                    }
                }
              pEl2 = ReplicateElement (pE, pDoc);
              InsertFirstChild (pEl2, pChild);
              CompleteElement (pEl2, pDoc);
              pChild = pEl2;
              if (pE != pElReplicate)
                {
                  /* les freres suivant l'element duplique' sont transfere's */
                  /* comme freres suivants du clone */
                  pClose = pE->ElNext;
                  if (pClose != NULL)
                    {
                      pPrevEl = pEl2;
                      while (pClose != NULL)
                        {
                          DestroyAbsBoxes (pClose, pDoc, TRUE);
                          pNextEl = pClose->ElNext;
                          /* signale a l'appli qu'on va retirer l'element */
                          notifyEl.event = TteElemDelete;
                          notifyEl.document = doc;
                          notifyEl.element = (Element) pClose;
                          notifyEl.info = 0; /* not sent by undo */
                          notifyEl.elementType.ElTypeNum =pClose->ElTypeNumber;
                          notifyEl.elementType.ElSSchema = (SSchema) pClose->ElStructSchema;
                          pSibling = pClose;
                          NSiblings = 0;
                          while (pSibling->ElPrevious != NULL)
                            {
                              NSiblings++;
                              pSibling = pSibling->ElPrevious;
                            }
                          notifyEl.position = TTE_TOOLKIT_DELETE;
                          if (CallEventType ((NotifyEvent *) (&notifyEl),TRUE))
                            /* l'application refuse de continuer */
                            {
                              CancelLastEditFromHistory (pDoc);
                              return (ret);
                            }
			  
                          RemoveElement (pClose);
                          /* notify the removed element */
                          notifyEl.element = (Element) (pE->ElParent);
                          notifyEl.position = NSiblings;
                          notifyEl.info = 2; /* element removed */
                          CallEventType ((NotifyEvent *) (&notifyEl), FALSE);
                          InsertElementAfter (pPrevEl, pClose);
                          pPrevEl = pClose;
                          pClose = pNextEl;
                        }
                      AbstractImageUpdated (pDoc);
                      /* l'element pE devient le dernier fils de son pere */
                      ChangeFirstLast (pE, pDoc, FALSE, FALSE);
                    }
                }
              pEl2 = pE;
              pE = pE->ElParent;
            }
          pClose = SiblingElement (pEl2, FALSE);
          InsertElementAfter (pEl2, pChild);
          if (pClose == NULL)
            /* l'element pEl2 n'est plus le dernier fils de son pere */
            ChangeFirstLast (pEl2, pDoc, FALSE, TRUE);
          /* traite les exclusions des elements crees */
          RemoveExcludedElem (&pChild, pDoc);
          /* traite les attributs requis des elements crees */
          AttachMandatoryAttributes (pChild, pDoc);
          if (pDoc->DocSSchema == NULL)
            /* le document a ete ferme' entre temps */
            ret = FALSE;
          else
            {
              /* traitement des exceptions des nouveaux elements cree's */
              CreationExceptions (pChild, pDoc);
              /* complete l'element qui a ete coupe' en deux */
              pCompleteElem = NULL;
              pE = pPrev->ElParent;
              while ((pE != NULL) && (pE != pAncest))
                {
                  if (CompleteElement (pE, pDoc))
                    pCompleteElem = pE;
                  pE = pE->ElParent;
                }
              /* traitement des exceptions de l'element qui a ete coupe' */
              CreationExceptions (pElReplicate, pDoc);
              /* envoie un evenement ElemPaste.Post a l'application */
              NotifySubTree (TteElemPaste, pDoc, pChild, 0, 1, FALSE, FALSE);
              /* reconstruit les paves des elements qui ont ete complete's */
              /* cree les paves du nouvel element */
              CreateAllAbsBoxesOfEl (pChild, pDoc);
              CreateAllAbsBoxesOfEl (pElReplicate, pDoc);
              if (pCompleteElem != NULL)
                CreateAllAbsBoxesOfEl (pCompleteElem, pDoc);
              AbstractImageUpdated (pDoc);
              RedisplayDocViews (pDoc);
              /* si on est dans un element copie' par inclusion,   */
              /* on met a jour les copies de cet element.          */
              RedisplayCopies (pEl2, pDoc, TRUE);
              RedisplayCopies (pElReplicate, pDoc, TRUE);
              dispMode = TtaGetDisplayMode (doc);
              if ( dispMode == DisplayImmediately )
                TtaSetDisplayMode (doc, DeferredDisplay);
              UpdateNumbers (NextElement (pChild), pEl2, pDoc, TRUE);
              if ( dispMode == DisplayImmediately )
                TtaSetDisplayMode (doc, DisplayImmediately);		
              /* indiquer que le document est modifie' */
              SetDocumentModified (pDoc, TRUE, 30);
              if (select && pNext != NULL)
                {
                  if (nextChar == 0 &&
                      !TypeHasException (ExcIsImg, pNext->ElTypeNumber,
                                         pNext->ElStructSchema))
                    SelectElementWithEvent (pDoc, pNext, TRUE, TRUE);
                  else
                    {
                      if (nextChar <= 1)
                        /* select the beginning of the leaf in element pNext */
                        {
                          while (pNext && !pNext->ElTerminal &&
                                 pNext->ElFirstChild)
                            pNext = pNext->ElFirstChild;
                          if (pNext->ElTerminal &&
                              pNext->ElLeafType == LtPicture)
                            nextChar = 0;
                        }
                      SelectPositionWithEvent (pDoc, pNext, nextChar, TRUE);
                    }
                }
              ret = TRUE;
              AddEditOpInHistory (pElReplicate->ElNext, pDoc, FALSE, TRUE);
            }
          for (view = 0; view < MAX_VIEW_DOC; view++)
            if (pEl->ElAbstractBox[view] != NULL)
              RedisplayCopies (pEl->ElAbstractBox[view]->AbElement, pDoc,
                               TRUE);
        }
    }
  return ret;
}


/*----------------------------------------------------------------------
  Essaie de fusionner tous les elements feuilles texte du         
  sous-arbre de racine pEl avec leurs voisins, s'ils ont les      
  memes attributs et les memes regles de presentation specifique. 
  ----------------------------------------------------------------------*/
static void MergeTextLeaves (PtrElement *pEl, PtrElement *pFirstFree,
                             PtrDocument *pDoc)
{
  PtrElement          pPrev, pFree, pChild;

  if ((*pEl)->ElTerminal)
    /* c'est un element feuille, essaie de le fusionner avec ses voisins
       si les attributs et les regles de presentation specifique sont les
       memes */
    {
      if ((*pEl)->ElLeafType == LtText)
        {
          pPrev = (*pEl)->ElPrevious;
          /* cherche une fusion avec le precedent */
          if (IsIdenticalTextType (pPrev, *pDoc, &pFree))
            {
              if (pFree != NULL)
                /* chaine l'element a liberer */
                KeepFreeElements (pFree, pFirstFree);
              *pEl = pPrev;
            }
        }
    }
  else
    /* ce n'est pas une feuille, traite tous les descendants */
    {
      pChild = (*pEl)->ElFirstChild;
      while (pChild != NULL)
        {
          MergeTextLeaves (&pChild, pFirstFree, pDoc);
          pChild = pChild->ElNext;
        }
    }
}

/*----------------------------------------------------------------------
  SelectRange
  Select all elements between firstSel and lastSel in document pDoc
  Redisplay all views.
  ----------------------------------------------------------------------*/
void SelectRange (PtrDocument pDoc, PtrElement firstSel, PtrElement lastSel,
                  int firstChar, int lastChar)
{
  PtrElement          pEl, SelectedEl[MAX_SEL_ELEM];
  /* pointeurs sur les elements selectionnes si SelContinue est faux */
  int                 NSelectedEls, index, prevLen;
  ThotBool            discreteSelection;

  if (firstSel == NULL)
    return;

  if (ElemIsAnAncestor (firstSel, lastSel))
    {
      /* firstSel is a parent of lastSel */
      lastSel = firstSel;
      firstChar = lastChar = 0;
    }
  else if (ElemIsAnAncestor (lastSel, firstSel))
    {
      /* lastSel is a parent of firstSel */
      firstSel = lastSel;
      firstChar = lastChar = 0;
    }

  discreteSelection = !SelContinue;
  NSelectedEls = 0;
  pEl = firstSel;
  while (pEl != NULL)
    {
      if (pEl->ElStructSchema == NULL)
        /* element pEl has been freed by application during merging */
        pEl = NULL;
      else
        {
	    
          if (discreteSelection)
            /* la selection est discontinue, on met a jour la liste des */
            /* elements a re-selectionner */
            SelectedEl[NSelectedEls++] = pEl;
          /* cherche l'element a traiter ensuite */
          pEl = NextInSelection (pEl, lastSel);
        }
    }


  /* reaffiche toutes les vues */
  AbstractImageUpdated (pDoc);
  RedisplayDocViews (pDoc);

  /* set the new selection */
  if (!firstSel->ElTerminal)
    SelectElement (pDoc, firstSel, TRUE, TRUE, TRUE);
  else if (firstChar > 1)
    {
      if (firstSel == lastSel)
        prevLen = lastChar - 1;
      else
        prevLen = 0;
      SelectString (pDoc, firstSel, firstChar, prevLen);
    }
  else if ((lastChar == 0 && firstChar == 0) || lastSel != firstSel)
    SelectElement (pDoc, firstSel, TRUE, TRUE, TRUE);
  else
    SelectString (pDoc, firstSel, 1, lastChar);

  if (lastSel != firstSel)
    {
      if (discreteSelection)
        for (index = 1; index <= NSelectedEls; index++)
          AddInSelection (SelectedEl[index - 1],
                          (ThotBool)(index == NSelectedEls));
      else if (lastChar == 0)
        ExtendSelection (lastSel, lastChar, TRUE, FALSE, FALSE);
      else
        ExtendSelection (lastSel, lastChar, TRUE, FALSE, FALSE);
    }
}

/*----------------------------------------------------------------------
  TtaBreakElement
  Split the pSplitEl element and complete to replicate the pElReplicate
  element.
  block is TRUE if the split element is a block
  Return TRUE if okay
  select: select the new element
  ----------------------------------------------------------------------*/
ThotBool TtaBreakElement (Element replicate, Element split,
                          int splitIndex, ThotBool block, ThotBool select)
{
  ThotBool res = FALSE;

  UserErrorCode = 0;
  if (replicate == NULL)
    TtaError (ERR_invalid_parameter);
  else if (split == NULL)
    TtaError (ERR_invalid_parameter);
  else if (!ElemIsAnAncestor((PtrElement) replicate, (PtrElement) split))
    TtaError (ERR_invalid_parameter);
  else
    res = BreakElement ((PtrElement) replicate, (PtrElement) split,
                        splitIndex, block, select);
  return res;
}
