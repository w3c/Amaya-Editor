/*
 *
 *  (c) COPYRIGHT INRIA 1996-2009
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * This module handles the ChangeType, Surround, Copy, Cut, and Paste commands
 *
 * Authors: V. Quint (INRIA)
 *          S. Bonhomme (INRIA) - Separation between structured and
 *                                unstructured editing modes
 */
#include "thot_gui.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "constmenu.h"
#include "typemedia.h"
#include "language.h"
#include "libmsg.h"
#include "message.h"
#include "fileaccess.h"
#include "appaction.h"
#include "appdialogue.h"
#include "dialog.h"
#include "tree.h"
#include "content.h"
#include "registry.h"
#include "application.h"

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "constres.h"
#include "creation_tv.h"
#include "modif_tv.h"
#include "select_tv.h"
#include "edit_tv.h"
#include "appdialogue_tv.h"
#include "platform_tv.h"
#include "viewapi_f.h"
#include "svgedit.h"

extern int SelectedPointInPolyline;

/* isomorphism description */
typedef struct _IsomorphDesc *PtrIsomorphDesc;
typedef struct _IsomorphDesc
{
  int                 IDtypeNum;
  PtrSSchema          IDStructSch;
  int                 IDtypeNumIso;
  PtrSSchema          IDStructSchIso;
  PtrIsomorphDesc     IDNext;
}
IsomorphDesc;

typedef struct _ChoiceOptionDescr *PtrChoiceOptionDescr;
typedef struct _ChoiceOptionDescr
{
  int                 COtypeNum;
  PtrSSchema          COStructSch;
  PtrChoiceOptionDescr CONext;
}
ChoiceOptionDescr;

static PtrIsomorphDesc firstIsomorphDesc = NULL;

#define MAX_ITEMS_CHANGE_TYPE	25

/* types of elements proposed in the "Change Type" menu */
static int          NChangeTypeItems;
static int          ChangeTypeTypeNum[MAX_ITEMS_CHANGE_TYPE];
static PtrSSchema   ChangeTypeSSchema[MAX_ITEMS_CHANGE_TYPE];
static int          ChangeTypeMethod[MAX_ITEMS_CHANGE_TYPE];

/* transformation callback procedure */
static Func2        TransformIntoFunction = NULL;
/* copy & cut callback procedure */
static Proc1        CopyAndCutFunction = NULL;
static Proc3        CopyCellFunction = NULL;
static Proc3        CopyRowFunction = NULL;
static Proc5        NextCellInColumnFunction = NULL;

#define M_EQUIV 1
#define M_RESDYN 2

#include "absboxes_f.h"
#include "appdialogue_f.h"
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
#include "memory_f.h"
#include "paginate_f.h"
#include "presrules_f.h"
#include "references_f.h"
#include "res_f.h"
#include "schemas_f.h"
#include "search_f.h"
#include "structcreation_f.h"
#include "structmodif_f.h"
#include "structschema_f.h"
#include "structselect_f.h"
#include "textcommands_f.h"
#include "thotmsg_f.h"
#include "tree_f.h"
#include "unstructchange_f.h"
#include "undo_f.h"
#include "undoapi_f.h"
#include "views_f.h"

/*----------------------------------------------------------------------
  IsolateSelection
  Si la selection passee en parametre commence ou finit sur des   
  elements partiellement selectionnes, ces elements sont coupes   
  en deux et leurs paves egalement.                               
  ----------------------------------------------------------------------*/
void IsolateSelection (PtrDocument pDoc, PtrElement *pFirstSel,
                       PtrElement *pLastSel, int *firstChar, int *lastChar,
                       ThotBool createEmpty)
{
  PtrElement	       pEl;
  ThotBool	       done;

  if (*firstChar > 1 &&
      (*pFirstSel)->ElTerminal && (*pFirstSel)->ElLeafType == LtText)
    /* la selection courante commence a l'interieur du premier element */
    /* selectionne */
    /* coupe le premier element selectionne' */
    {
      SplitBeforeSelection (pFirstSel, firstChar, pLastSel, lastChar, pDoc);
      /* cree les paves de la deuxieme partie */
      CreateNewAbsBoxes (*pFirstSel, pDoc, 0);
      ApplDelayedRule (*pFirstSel, pDoc);
    }

  done = FALSE;
  if (createEmpty &&
      *firstChar == 1 && *lastChar <= 1 && *pFirstSel == *pLastSel &&
      (*pLastSel)->ElTerminal && (*pLastSel)->ElLeafType == LtText)
    {
      pEl = NewSubtree ((*pFirstSel)->ElTypeNumber, (*pFirstSel)->ElStructSchema,
                        pDoc, FALSE, TRUE, FALSE, TRUE);
      InsertElementBefore (*pFirstSel, pEl);
      CreateNewAbsBoxes (pEl, pDoc, 0);
      ApplDelayedRule (pEl, pDoc);
      *pFirstSel = pEl;
      *pLastSel = pEl;
      *firstChar = 0;
      *lastChar = 0;
      done = TRUE;
    }
  if (!done)
    if (*pLastSel)
      if ((*pLastSel)->ElTerminal && (*pLastSel)->ElLeafType == LtText &&
          *lastChar > 1 && *lastChar <= (*pLastSel)->ElVolume)
        SplitAfterSelection (*pLastSel, *lastChar, pDoc);
}

/*----------------------------------------------------------------------
  IsolatedPairedElem   verifie si l'element pEl est un element de 
  paire dont l'homologue ne serait pas dans la partie             
  selectionnee determinee par firstSel et lastSel.                
  ----------------------------------------------------------------------*/
static ThotBool IsolatedPairedElem (PtrElement pEl, PtrElement firstSel,
                                    PtrElement lastSel)
{
  PtrElement          pEl2, pAncest, pSel;
  ThotBool            alone;

  alone = FALSE;
  if (pEl->ElTerminal && pEl->ElLeafType == LtPairedElem)
    /* l'element est un element de paire */
    {
      /* le 2eme element de la paire */
      pEl2 = GetOtherPairedElement (pEl);
      /* Si le 2eme element de la paire n'existe pas, on accepte de */
      /* detruire cet element bien qu'il soit seul */
      if (pEl2 != NULL)
        /* le 2eme element de la paire est-il dans la selection ? */
        {
          /* a priori, non */
          alone = TRUE;
          /* parcourt la suite des elements selectionne's */
          pSel = firstSel;
          while (pSel != NULL && alone)
            {
              /* verifie si cet element selectionne' est le 2eme element */
              /* de la paire ou un de ses ascendants */
              pAncest = pEl2;
              /* remonte les ascendants du 2eme element de la paire */
              while (pAncest != NULL && alone)
                if (pAncest == pSel)
                  alone = FALSE;	/* trouve. Il n'est donc pas seul */
                else
                  pAncest = pAncest->ElParent;
              /* si on n'a pas encore trouve', on teste l'element */
              /* selectionne' suivant */
              if (alone)
                pSel = NextInSelection (pSel, lastSel);
            }
        }
    }
  return alone;
}


/*----------------------------------------------------------------------
  IsomorphicTypes indique si les types (pSS1, typeNum1) et	
  (pSS2, typeNum2) sont isomorphes on non.                        
  ----------------------------------------------------------------------*/
static ThotBool IsomorphicTypes (PtrSSchema pSS1, int typeNum1,
                                 PtrSSchema pSS2, int typeNum2)
{
  PtrSRule            pSRule1, pSRule2;
  PtrIsomorphDesc     pIsoD;
  ThotBool            ret;

  ret = FALSE;
  if (typeNum1 == typeNum2 && !strcmp (pSS1->SsName, pSS2->SsName))
    /* meme regle */
    ret = TRUE;
  else
    {
      pSRule1 = pSS1->SsRule->SrElem[typeNum1 - 1];
      pSRule2 = pSS2->SsRule->SrElem[typeNum2 - 1];
      if (pSRule1->SrConstruct == pSRule2->SrConstruct)
        switch (pSRule1->SrConstruct)
          {
          case CsNatureSchema:
            ret = (strcmp (pSRule1->SrName, pSRule2->SrName) == 0);
            break;
          case CsBasicElement:
            ret = (pSRule1->SrBasicType == pSRule2->SrBasicType);
            break;
          case CsReference:
            ret = (pSRule1->SrReferredType == pSRule2->SrReferredType &&
                   strcmp (pSRule1->SrRefTypeNat, pSRule2->SrRefTypeNat) == 0);
            break;
          case CsIdentity:
            ret = (pSRule1->SrIdentRule == pSRule2->SrIdentRule);
            break;
          case CsList:
            ret = IsomorphicTypes (pSS1, pSRule1->SrListItem, pSS2,
                                   pSRule2->SrListItem);
            break;
          case CsConstant:
            ret = (pSRule1->SrIndexConst == pSRule2->SrIndexConst);
            break;
          case CsEmpty:
          case CsAny:
            ret = TRUE;
            break;
          default:	/* CsChoice, CsAggregate, CsUnorderedAggregate */
            ret = FALSE;
            break;
          }
      /* on a trouve' un isomorphisme, on le note */
      if (ret)
        {
          pIsoD = (PtrIsomorphDesc) TtaGetMemory (sizeof (IsomorphDesc));
          if (pIsoD != NULL)
            {
              pIsoD->IDtypeNum = typeNum1;
              pIsoD->IDStructSch = pSS1;
              pIsoD->IDtypeNumIso = typeNum2;
              pIsoD->IDStructSchIso = pSS2;
              pIsoD->IDNext = firstIsomorphDesc;
              firstIsomorphDesc = pIsoD;
            }
        }
    }
  return ret;
}


/*----------------------------------------------------------------------
  RegisterIfIsomorphic    Si le type (pSS, typeNum) est isomorphe au 
  type de l'element pEl, mais different, on le met dans   
  la table qui va servir a construire le menu Change Type 
  Si ce type est un choix, on fait de meme avec toutes    
  ses options.                                            
  ----------------------------------------------------------------------*/
static void RegisterIfIsomorphic (PtrElement pEl, PtrSSchema pSS, int typeNum)
{
  PtrSRule            pSRule;
  int                 choice;
  ThotBool            found;
  int                 i;
  STRING	       strResDyn;

  /* on ne propose pas le type qu'a deja l'element */
  if (pEl->ElTypeNumber != typeNum ||
      strcmp (pEl->ElStructSchema->SsName, pSS->SsName))
    /* on ne fait rien si la table est pleine */
    if (NChangeTypeItems < MAX_ITEMS_CHANGE_TYPE - 1)
      {
        /* on cherche si ce type est deja dans la table */
        found = FALSE;
        for (i = 0; i < NChangeTypeItems && !found; i++)
          if (typeNum == ChangeTypeTypeNum[i] &&
              !strcmp (pSS->SsName, ChangeTypeSSchema[i]->SsName))
            found = TRUE;
        if (!found)
          {
            strResDyn = NULL;
            /* ce type n'est pas deja dans la table */
            if (IsomorphicTypes (pEl->ElStructSchema, pEl->ElTypeNumber,
                                 pSS, typeNum))
              /* ce type est isomorphe au type de l'element */
              /* on le met dans la table */
              {
                ChangeTypeTypeNum[NChangeTypeItems] = typeNum;
                ChangeTypeSSchema[NChangeTypeItems] = pSS;
                ChangeTypeMethod[NChangeTypeItems] = M_EQUIV;
                NChangeTypeItems++;
              }
            /* existe-t-il une relation facteur ou massif */
            else if (strResDyn != NULL &&
                     RestMatchElements ((Element)pEl, (Element)pEl, 
                                        (SSchema)pSS, typeNum))
              {
                ChangeTypeTypeNum[NChangeTypeItems] = typeNum;
                ChangeTypeSSchema[NChangeTypeItems] = pSS;
                ChangeTypeMethod[NChangeTypeItems] = M_RESDYN;
                NChangeTypeItems++;
              }
            else
              /* ce type n'est pas isomorphe, mais c'est peut-etre un
                 Choix */
              {
                pSRule = pSS->SsRule->SrElem[typeNum - 1];
                if (pSRule->SrConstruct == CsChoice)
                  /* c'est un CsChoix. On essaie de mettre chacune de */
                  /* ses options dans la table */
                  for (choice = 0; choice < pSRule->SrNChoices &&
                         NChangeTypeItems < MAX_ITEMS_CHANGE_TYPE - 1; choice++)
                    RegisterIfIsomorphic (pEl, pSS, pSRule->SrChoice[choice]);
              }
          }
      }
}


/*----------------------------------------------------------------------
  IsomorphicTransform transforme le sous-arbre dont la      	
  racine est pointee par pEl en un sous-arbre             
  isomorphe, dont la racine prend le type (pSS, typeNum). 
  ----------------------------------------------------------------------*/
static void IsomorphicTransform (PtrElement pEl, PtrSSchema pSS,
                                 int typeNum, PtrDocument pDoc)
{
  PtrElement          pChild;
  PtrSSchema          pNewSS;
  PtrAttribute        pAttr, pNextAttr, pAttrDouble, pNextDoubleAttr;
  PtrIsomorphDesc     pIsoD;
  PtrSRule            pSRule;
  int                 att, newType;
  ThotBool            found;

  if (pEl->ElTypeNumber == typeNum &&
      !strcmp (pEl->ElStructSchema->SsName, pSS->SsName))
    /* l'element a deja le type voulu, il n'y a rien a faire */
    return;
  pSRule = pSS->SsRule->SrElem[typeNum - 1];
  /* retire les attributs locaux propres a l'ancien type et qui */
  /* ne s'appliquent pas au nouveau type */
  pAttr = pEl->ElFirstAttr;
  while (pAttr != NULL)	/* examine tous les attributs de l'element */
    {
      pNextAttr = pAttr->AeNext;
      if (!pAttr->AeAttrSSchema->SsAttribute->TtAttr[pAttr->AeAttrNum - 1]->AttrGlobal)
        /* c'est un attribut local, il faut verifier */
        {
          found = FALSE;
          for (att = 0; att < pSRule->SrNLocalAttrs && !found; att++)
            if (pSRule->SrLocalAttr->Num[att] == pAttr->AeAttrNum)
              /* c'est bien un attribut local du type de l'element */
              found = TRUE;
          if (!found)
            /* l'attribut ne figure pas parmi les attributs locaux du */
            /* type de l'element, on le supprime */
            {
              RemoveAttribute (pEl, pAttr);
              DeleteAttribute (pEl, pAttr);
            }
        }
      /* passe a l'attribut suivant de l'element */
      pAttr = pNextAttr;
    }
  /* change le type de l'element */
  pEl->ElTypeNumber = typeNum;
  pEl->ElStructSchema = pSS;
  /* met les attributs imposes du nouveau type */
  AttachRequiredAttributes (pEl, pSRule, pSS, TRUE, pDoc);
  /* verifie qu'il n'y a pas d'attribut en double */
  pAttr = pEl->ElFirstAttr;
  while (pAttr != NULL)
    {
      pNextAttr = pAttr->AeNext;
      pAttrDouble = pNextAttr;
      while (pAttrDouble != NULL)
        {
          pNextDoubleAttr = pAttrDouble->AeNext;
          if (pAttrDouble->AeAttrNum == pAttr->AeAttrNum &&
              !strcmp (pAttrDouble->AeAttrSSchema->SsName,
                       pAttr->AeAttrSSchema->SsName))
            {
              if (pNextAttr == pAttrDouble)
                pNextAttr = pNextDoubleAttr;
              RemoveAttribute (pEl, pAttrDouble);
              DeleteAttribute (pEl, pAttrDouble);
            }
          pAttrDouble = pNextDoubleAttr;
        }
      pAttr = pNextAttr;
    }
  if (!pEl->ElTerminal)
    /* on transforme les fils de l'element */
    {
      pChild = pEl->ElFirstChild;
      while (pChild != NULL)
        {
          pNewSS = pChild->ElStructSchema;
          newType = pChild->ElTypeNumber;
          pIsoD = firstIsomorphDesc;
          while (pIsoD != NULL)
            {
              if (pIsoD->IDtypeNum == pChild->ElTypeNumber &&
                  !strcmp (pIsoD->IDStructSch->SsName,
                           pChild->ElStructSchema->SsName))
                {
                  newType = pIsoD->IDtypeNumIso;
                  pNewSS = pIsoD->IDStructSchIso;
                  pIsoD = NULL;
                }
              else
                pIsoD = pIsoD->IDNext;
            }
          IsomorphicTransform (pChild, pNewSS, newType, pDoc);
          pChild = pChild->ElNext;
        }
      /* on cree les elements obligatoires qui n'existent pas */
      CompleteElement (pEl, pDoc);
    }
}


/*----------------------------------------------------------------------
  SendEventSubTree sends the AppEvent.Pre event for the element pEl
  of the document pDoc and all enclosed elements.
  When inTable is TRUE row elements are not notified.
  When inRow is TRUE cell elements are not notified.
  Returns TRUE if the application refuses the delete operation  
  ----------------------------------------------------------------------*/
ThotBool SendEventSubTree (APPevent AppEvent, PtrDocument pDoc, PtrElement pEl,
                           int end, int info, ThotBool inTable,  ThotBool inRow)
{
  NotifyElement       notifyEl;
  PtrElement          pChild;
  ThotBool            ret;
  ThotBool            cancel, notify;

  /* envoie l'evenement appEvent.Pre a l'element pEl */
  notifyEl.event = AppEvent;
  notifyEl.document = (Document) IdentDocument (pDoc);
  notifyEl.element = (Element) pEl;
  notifyEl.info = info;
  notifyEl.elementType.ElTypeNum = pEl->ElTypeNumber;
  notifyEl.elementType.ElSSchema = (SSchema) (pEl->ElStructSchema);
  if (AppEvent == TteElemDelete)
    notifyEl.position = end;
  else
    notifyEl.position = 0;

  /* by default notify the application */
  notify = TRUE;
  if (TypeHasException (ExcIsTable, pEl->ElTypeNumber, pEl->ElStructSchema))
    inTable = TRUE;
  else if (TypeHasException (ExcIsRow, pEl->ElTypeNumber, pEl->ElStructSchema))
    {
      inRow = TRUE;
      if (inTable)
        notify = FALSE;
    }
  else if (inRow &&
           TypeHasException (ExcIsCell, pEl->ElTypeNumber, pEl->ElStructSchema))
    notify = FALSE;
  else if (inTable &&
           TypeHasException (ExcIsColHead, pEl->ElTypeNumber, pEl->ElStructSchema))
    notify = FALSE;
  if (notify)
    cancel = CallEventType ((NotifyEvent *) (&notifyEl), TRUE);
  else
    cancel = FALSE;
  if (pDoc->DocNotifyAll && !cancel && !pEl->ElTerminal)
    {
      /* le document demande un evenement pour chaque element du sous-arbre */
      pChild = pEl->ElFirstChild;
      /* envoie recursivement un evenement a chaque fils de pEl */
      while (pChild)
        {
          ret = SendEventSubTree (AppEvent, pDoc, pChild, 0, info,
                                  inTable, inRow);
          cancel = cancel | ret;
          if (cancel && pEl->ElStructSchema == NULL)
            // the parent element could be deleted stop here
            return (cancel);
          pChild = pChild->ElNext;
        }
    }
  return (cancel);
}


/*----------------------------------------------------------------------
  DoChangeType							
  ----------------------------------------------------------------------*/
static ThotBool DoChangeType (PtrElement pEl, PtrDocument pDoc, int newTypeNum,
                              PtrSSchema newSSchema)
{
  NotifyElement       notifyEl;
  PtrElement	       pDummyEl, nextEl;
  ThotBool            ret;

  ret = FALSE;
  /* envoie l'evenement TteElemChange.Pre a l'element pEl */
  notifyEl.event = TteElemChange;
  notifyEl.document = (Document) IdentDocument (pDoc);
  notifyEl.element = (Element) pEl;
  notifyEl.info = 0; /* not sent by undo */
  notifyEl.elementType.ElTypeNum = newTypeNum;
  notifyEl.elementType.ElSSchema = (SSchema) newSSchema;
  notifyEl.position = 0;
  if (!CallEventType ((NotifyEvent *) (&notifyEl), TRUE))
    {
      /* garde l'ancien type de l'element */
      notifyEl.elementType.ElTypeNum = pEl->ElTypeNumber;
      notifyEl.elementType.ElSSchema = (SSchema) (pEl->ElStructSchema);
      /* annule d'abord la selection */
      TtaClearViewSelections ();
      /* detruit les paves de l'element qui va changer de type */
      DestroyAbsBoxes (pEl, pDoc, TRUE);
      AbstractImageUpdated (pDoc);

      /* create a dummy element for renumbering the following elements */
      GetElement (&pDummyEl);
      pDummyEl->ElTypeNumber = pEl->ElTypeNumber;
      pDummyEl->ElStructSchema = pEl->ElStructSchema;

      /* on transforme le type de l'element, en changeant les types */
      /* qui doivent l'etre dans ses descendants */
      IsomorphicTransform (pEl, newSSchema, newTypeNum, pDoc);

      /* The following elements must take into account the deletion of the
         old type */
      nextEl = NextElement (pEl);
      UpdateNumbers (nextEl, pDummyEl, pDoc, TRUE);
      FreeElement (pDummyEl);

      /* transformation is done */
      ret = TRUE;
      RemoveExcludedElem (&pEl, pDoc);
      AttachMandatoryAttributes (pEl, pDoc);
      if (pDoc->DocSSchema != NULL)
        /* le document n'a pas ete ferme' entre temps */
        {
          /* envoie a l'application l'evenement TteElemChange.Post */
          notifyEl.event = TteElemChange;
          notifyEl.document = (Document) IdentDocument (pDoc);
          notifyEl.element = (Element) pEl;
          notifyEl.info = 0; /* not sent by undo */
          notifyEl.position = 0;
          CallEventType ((NotifyEvent *) (&notifyEl), FALSE);
          /* cree les paves de l'element et reaffiche */
          CreateAllAbsBoxesOfEl (pEl, pDoc);
          AbstractImageUpdated (pDoc);
          RedisplayDocViews (pDoc);
          /* si on est dans un element copie' par inclusion, on met a jour
             les copies de cet element. */
          RedisplayCopies (pEl, pDoc, TRUE);
          /* Renumbers the following element, taking into account the new
             type of the element */
          UpdateNumbers (nextEl, pEl, pDoc, TRUE);
          /* indique que le document est modifie' */
          SetDocumentModified (pDoc, TRUE, 30);
          SelectElementWithEvent (pDoc, pEl, TRUE, TRUE);
        }
    }
  return ret;
}


/*----------------------------------------------------------------------
  ChangeReferences cherche dans le sous-arbre de racine pElem tous
  les elements reference's et transfert sur eux les references qui
  sont dans le tampon Copier-Couper-Coller                        
  ----------------------------------------------------------------------*/
static void ChangeReferences (PtrElement pElem, PtrDocument * docsel)
{
  PtrElement          pChild;
  PtrReferredDescr    pRefD;
  PtrReference        pRef1, pRef, pNextRef;

  if (pElem->ElReferredDescr != NULL)
    /* cet element est reference'. CopyTree n'a pas copie' son */
    /* descripteur d'element reference', qui est encore partage' avec */
    /* celui de l'element source */
    /* traite les references a l'element source qui sont dans le */
    /* buffer de sauvegarde */
    {
      pRefD = pElem->ElReferredDescr;
      /* descripteur d'element reference' */
      pElem->ElReferredDescr = NULL;
      /* reste attache' a l'element source */
      pRef = pRefD->ReFirstReference;
      /* 1ere reference a l'element source */
      while (pRef != NULL)
        /* parcourt les references a l'element source */
        {
          pNextRef = pRef->RdNext;
          /* prepare la reference suivante */
          if (IsASavedElement (pRef->RdElement))
            /* la reference est dans le tampon, on la fait pointer vers */
            /* l'element traite' (pElem) */
            {
              if (pElem->ElReferredDescr == NULL)
                /* l'element n'a pas de descripteur d'element */
                /* reference',  on lui en affecte un */
                {
                  pElem->ElReferredDescr = NewReferredElDescr (*docsel);
                  pElem->ElReferredDescr->ReReferredElem = pElem;
                }
              /* lie le descripteur de reference et le descripteur */
              /* d'element reference' de l'element traite' */
              pRef1 = pRef;
              /* dechaine le descripteur de la chaine des references a */
              /* l'element source */
              if (pRef1->RdNext != NULL)
                pRef1->RdNext->RdPrevious = pRef1->RdPrevious;
              if (pRef1->RdPrevious == NULL)
                pRefD->ReFirstReference = pRef1->RdNext;
              else
                pRef1->RdPrevious->RdNext = pRef1->RdNext;
              /* le chaine en tete de la liste des references a */
              /* l'element traite' */
              pRef1->RdReferred = pElem->ElReferredDescr;
              pRef1->RdNext = pRef1->RdReferred->ReFirstReference;
              if (pRef1->RdNext != NULL)
                pRef1->RdNext->RdPrevious = pRef;
              pRef1->RdReferred->ReFirstReference = pRef;
              pRef1->RdPrevious = NULL;
            }
          pRef = pNextRef;
          /* passe a la reference suivante */
        }
    }
  if (!pElem->ElTerminal)
    /* element non terminal, on traite tous ses fils */
    {
      pChild = pElem->ElFirstChild;
      while (pChild != NULL)
        {
          ChangeReferences (pChild, docsel);
          pChild = pChild->ElNext;
        }
    }
}

/*----------------------------------------------------------------------
  FreeSavedElements libere la suite des elements sauve's par les	
  commandes Copy et Cut.						
  ----------------------------------------------------------------------*/
void FreeSavedElements ()
{
  PtrPasteElem        pPasteEl, pNextPasteEl;

  pPasteEl = FirstSavedElement;
  while (pPasteEl != NULL)
    {
      pNextPasteEl = pPasteEl->PeNext;
      if (pPasteEl->PeElement != NULL)
        DeleteElement (&pPasteEl->PeElement, NULL);
      TtaFreeMemory (pPasteEl);
      pPasteEl = pNextPasteEl;
    }
  if (FirstSavedElement)
    ReleaseSSchemasForSavedElements ();
  FirstSavedElement = NULL;
  DocOfSavedElements = NULL;
  /* no whole column is saved */
  WholeColumnSaved = FALSE;
  TableRowsSaved = FALSE;
  /* disable Paste command */
  if (ClipboardThot.BuLength != 0)
    /* switch the Paste entry in all documents */
    SwitchPaste (NULL, FALSE);
}

/*----------------------------------------------------------------------
  TtaIsColumnRowSaved answers TRUE if there is a column or a row in
  the Thot Clipboard.
  ----------------------------------------------------------------------*/
ThotBool TtaIsColumnSaved (Document document)
{
  PtrDocument         docsel;
  PtrElement          firstSelection, lastSelection;
  int                 firstChar, lastChar;

  if (!GetCurrentSelection (&docsel, &firstSelection, &lastSelection,
                            &firstChar, &lastChar))
    return (FALSE);
  else if (LoadedDocument[document - 1] != docsel)
    return (FALSE);
  else if (WholeColumnSaved)
    return (TRUE);
  else
    return (FALSE);
}

/*----------------------------------------------------------------------
  TtaIsColumnRowSaved answers TRUE if there is a column or a row in
  the Thot Clipboard.
  ----------------------------------------------------------------------*/
ThotBool TtaIsRowSaved (Document document)
{
  PtrDocument         docsel;
  PtrElement          firstSelection, lastSelection;
  int                 firstChar, lastChar;

  if (!GetCurrentSelection (&docsel, &firstSelection, &lastSelection,
                            &firstChar, &lastChar))
    return (FALSE);
  else if (LoadedDocument[document - 1] != docsel)
    return (FALSE);
  else if (TableRowsSaved)
    return (TRUE);
  else
    return (FALSE);
}

/*----------------------------------------------------------------------
  RegSSchemaDescent
  Check all descendants of element pEl and if their structure schema
  is not the same as their parent, register their schema.
  ----------------------------------------------------------------------*/
static void RegSSchemaDescent (PtrElement pEl)
{
  PtrElement pChild;

  if (!pEl->ElTerminal)
    {
      pChild = pEl->ElFirstChild;
      while (pChild)
        {
          if (pChild->ElStructSchema != pEl->ElStructSchema)
            RegisterSSchemaForSavedElements (pChild->ElStructSchema);
          RegSSchemaDescent (pChild);
          pChild = pChild->ElNext;
        }
    }
}

/*----------------------------------------------------------------------
  SaveElement     met l'element pointe' par pEl a la fin de la    
  liste des elements a copier.                    
  pParent est le pere de l'element original	
  ----------------------------------------------------------------------*/
static void SaveElement (PtrElement pEl, PtrElement pParent, int doc,
                         PtrElement origEl)
{
  PtrPasteElem        pPasteEl, pNewPasteEl;
  PtrElement          pAncest, pCell;
  int                 level, i;

  pNewPasteEl = (PtrPasteElem) TtaGetMemory (sizeof (PasteElemDescr));
  if (pNewPasteEl != NULL)
    {
      if (FirstSavedElement == NULL || TableRowsSaved)
        {
          if (TypeHasException (ExcIsRow, pEl->ElTypeNumber,
                                pEl->ElStructSchema))
            {
              if (FirstSavedElement == NULL)
                TableRowsSaved = TRUE;
            }
          else
            TableRowsSaved = FALSE;
        }

      if (FirstSavedElement == NULL)
        /* that's the first element saved */
        {
          /* enable the Paste command */
          if (ClipboardThot.BuLength == 0)
            /* switch the Paste entry in all documents */
            SwitchPaste (NULL, TRUE);
          FirstSavedElement = pNewPasteEl;
          pPasteEl = NULL;
          pNewPasteEl->PePrevious = NULL;
          pEl->ElPrevious = NULL;
          /* indicates whether a whole column is saved */
          WholeColumnSaved = WholeColumnSelected;
        }
      else
        {
          pPasteEl = FirstSavedElement;
          while (pPasteEl->PeNext != NULL)
            pPasteEl = pPasteEl->PeNext;
          pNewPasteEl->PePrevious = pPasteEl;
          pPasteEl->PeNext = pNewPasteEl;
          pEl->ElPrevious = pPasteEl->PeElement;
          pPasteEl->PeElement->ElNext = pEl;
        }

      /* notify the application that a cell of a whole row or a whole
         column is saved */
      if (CopyCellFunction &&
          TypeHasException (ExcIsCell, pEl->ElTypeNumber, pEl->ElStructSchema) &&
          WholeColumnSaved)
        (*(Proc3)CopyCellFunction) ((void*)pEl, (void*)doc, (void*)TableRowsSaved);
      else if (CopyCellFunction &&
               TypeHasException (ExcIsRow, pEl->ElTypeNumber,
                                 pEl->ElStructSchema) &&
               TableRowsSaved)
        {
          pCell = pEl->ElFirstChild;
          while (pCell)
            {
              if (TypeHasException (ExcIsCell, pCell->ElTypeNumber,
                                    pCell->ElStructSchema))
                /* notify the application for each enclosed cell */
                (*(Proc3)CopyCellFunction) ((void*)pCell, (void*)doc,
                                            (void*)TableRowsSaved);
              pCell = pCell->ElNext;
            }
        }
      if (CopyRowFunction &&
          origEl &&  /* don't notify for a Cut command */
          TypeHasException (ExcIsRow, pEl->ElTypeNumber,
                            pEl->ElStructSchema) &&
          TableRowsSaved)
        /* notify the application for the copied row */
        (*(Proc3)CopyRowFunction) ((void*)pEl, (void*) origEl, (void*)doc);

      pNewPasteEl->PeNext = NULL;
      pEl->ElNext = NULL;
      pNewPasteEl->PeElement = pEl;
      pAncest = pParent;
      for (i = 0; i < MAX_PASTE_LEVEL; i++)
        {
          if (pAncest == NULL)
            {
              pNewPasteEl->PeAscendTypeNum[i] = 0;
              pNewPasteEl->PeAscendSSchema[i] = NULL;
              pNewPasteEl->PeAscend[i] = NULL;
            }
          else
            {
              pNewPasteEl->PeAscendTypeNum[i] = pAncest->ElTypeNumber;
              pNewPasteEl->PeAscendSSchema[i] = pAncest->ElStructSchema;
              RegisterSSchemaForSavedElements (pAncest->ElStructSchema);
              pNewPasteEl->PeAscend[i] = pAncest;
              pAncest = pAncest->ElParent;
            }
        }
      level = 0;
      pAncest = pParent;
      while (pAncest != NULL)
        {
          level++;
          pAncest = pAncest->ElParent;
        }
      pNewPasteEl->PeElemLevel = level;
      /* register the structure schemas used by this element and its
         descendents */
      RegisterSSchemaForSavedElements (pEl->ElStructSchema);
      RegSSchemaDescent (pEl);
    }
}

/*----------------------------------------------------------------------
  CopyCommand traite la commande COPY				
  ----------------------------------------------------------------------*/
void CopyCommand ()
{
  PtrElement          firstSel, lastSel, pEl, pCopy, pE, pSecond, row;
  PtrElement          enclosingCell;
  PtrPasteElem        pSave;
  PtrDocument         pSelDoc;
  int                 firstChar, lastChar, doc;
  ThotBool            fakeCell;

  pCopy = NULL;
  fakeCell = FALSE;
  /* y-a-t'il une selection courante ? */
  if (!GetCurrentSelection (&pSelDoc, &firstSel, &lastSel, &firstChar,
                            &lastChar))
    return;
  else
    {
      /* on sauve les elements selectionnes */
      if (LastSelectedChar > 0 && lastChar == 0)
        lastChar = LastSelectedChar;
      if (firstSel->ElTerminal
          && firstSel->ElLeafType == LtText
          && firstSel->ElTextLength > 0
          && firstSel->ElTextLength < firstChar)
        /* la selection commence apres l'element complet */
        {
          firstSel = NextElement (firstSel);
          firstChar = 0;
        }
      if (firstSel &&
          (firstSel != lastSel || firstChar < lastChar - 1 ||
           firstChar == 0))
        /* il y a plus d'un caractere selectionne' */
        {
          /* libere la sauvegarde de l'Editeur */
          /* sauvegarde la partie selectionnee */
          FreeSavedElements ();
          /* document d'ou vient la partie sauvee */
          DocOfSavedElements = pSelDoc;
          doc = IdentDocument (DocOfSavedElements);

          // start a new copy
          TtaExecuteMenuAction ("StartACopyCommand", doc, 1, TRUE);

          /* tell the application what document the saved elements
             come from */
          if (CopyAndCutFunction)
            (*(Proc1)CopyAndCutFunction) ((void *) doc);
          pEl = NULL;
          if (WholeColumnSelected && FirstSelectedColumn && firstSel &&
              NextCellInColumnFunction)
            /* copying all the cells of a table column */
            {
              row = NULL;
              (*(Proc5)NextCellInColumnFunction) ((void*)(&pEl), (void*)(&row),
                                                  (void*)FirstSelectedColumn, (void*)doc, (void*)(&fakeCell));
            }
          else
            /* first selected element */
            pEl = firstSel;
          enclosingCell = NULL;
          while (pEl)
            {
              /* copy the element with its attributes */
              /* if referred loose the reference */
              if (enclosingCell == NULL && !WholeColumnSelected &&
                  TypeHasException (ExcIsCell, pEl->ElTypeNumber,
                                    pEl->ElStructSchema))
                {
                  enclosingCell = pEl;
                  pEl = pEl->ElFirstChild;
                }
              if (IsolatedPairedElem (pEl, firstSel, lastSel))
                pCopy = NULL;
              /* send the ElemCopy.Pre event */
              else if (SendEventSubTree (TteElemCopy, pSelDoc, pEl, 0, 0,
                                         FALSE, FALSE))
                /* application do not agree to copy of this element */
                pCopy = NULL;
              else
                /* the application agrees */
                {
                  if (WholeColumnSelected && fakeCell)
                    /* we are copying a table column and the current element is
                       an empty cell (a placeholder) generated by function
                       NextCellInColumn. We take that element */
                    pCopy = pEl;
                  else
                    /* create a deep copy of the element */
                    pCopy = CopyTree (pEl, pSelDoc, pEl->ElStructSchema,
                                      pSelDoc, NULL, FALSE, TRUE, FALSE, TRUE, TRUE);
                }
              if (pCopy)
                /* store the copy into the list of saved elements */
                {
                  if (pEl == firstSel && firstChar > 1)
                    /* only a sub-string of the first element was selected */
                    {
                      pE = pCopy;
                      /* split the string */
                      SplitTextElement (pCopy, firstChar,
                                        pSelDoc, FALSE, &pSecond, FALSE);
                      pCopy = pSecond;
                      /* remove the first part */
                      DeleteElement (&pE, pSelDoc);
                      if (firstSel == lastSel)
                        /* the end of the selection is within the element */
                        lastChar = lastChar - firstChar + 1;
                    }
                  if (pEl == lastSel &&
                      lastChar > 0 &&
                      // GetSelection return sometimes LastSelectedChar - 1
                      // if only one character is selection if will be 0
                      lastChar <= pCopy->ElTextLength)
                    /* only a sub-string of the last element was selected */
                    {
                      SplitTextElement (pCopy, lastChar,
                                        pSelDoc, FALSE, &pE, FALSE);
                      /* remove the last part */
                      DeleteElement (&pE, pSelDoc);
                    }
                  SaveElement (pCopy, pEl->ElParent, doc, pEl);
                }
		  
              /* next selected element */
              if (enclosingCell)
                {
                  /* copying the children of a table cell */
                  pEl = pEl->ElNext;
                  if (pEl == NULL)
                    {
                      pEl = NextInSelection (enclosingCell, lastSel);
                      enclosingCell = NULL;
                    }
                }
              else if (NextCellInColumnFunction &&
                       TypeHasException (ExcIsCell, pEl->ElTypeNumber,
                                         pEl->ElStructSchema) &&
                       WholeColumnSelected && FirstSelectedColumn)
                /* copying all the cells of a table column */
                (*(Proc5)NextCellInColumnFunction) ((void*)(&pEl),
                                                    (void*)(&row), (void*)FirstSelectedColumn, (void*)doc,
                                                    (void*)(&fakeCell));
              else
                /* normal case. Take the next element in the current
                   selection */
                pEl = NextInSelection (pEl, lastSel);
            }
        }
      /* look for and change references */
      pSave = FirstSavedElement;
      while (pSave)
        {
          /* update references */
          ChangeReferences (pSave->PeElement, &pSelDoc);
          /* send the ElemCopy.Post event to the whole subtree */
          NotifySubTree (TteElemCopy, pSelDoc, pSave->PeElement, 0, 0,
                         FALSE, FALSE);
          /* next subtree */
          pSave = pSave->PeNext;
        }
      /* labels will change  with the paste */
      ChangeLabel = TRUE;
     }
}


/*----------------------------------------------------------------------
  NextElemToBeCut
  Returns the next element to be cut. Return the enclosing cell
  when the content of a cell is deleted instead of the cell itself.
  ----------------------------------------------------------------------*/
static PtrElement NextElemToBeCut (PtrElement pEl, PtrElement lastSel,
                                   PtrDocument pSelDoc, PtrElement pSave,
                                   PtrElement *enclosingCell)
{
  ThotBool            stop;
  PtrElement          pNext;

  stop = FALSE;
  pNext = pEl;
  do
    if (pNext == NULL)
      stop = TRUE;
    else if ((pNext->ElTerminal && pNext->ElLeafType == LtPageColBreak) ||
             !CanCutElement (pNext, pSelDoc, pSave))
      {
        /* skip this element */
        if (*enclosingCell)
          {
            pNext = pNext->ElNext;
            if (pNext == NULL)
              {
                pNext = NextInSelection (*enclosingCell, lastSel);
                *enclosingCell = NULL;
              }
          }
        else
          pNext = NextInSelection (pNext, lastSel);
      }
    else if (!WholeColumnSelected &&
             TypeHasException (ExcIsCell, pNext->ElTypeNumber,
                               pNext->ElStructSchema))
      {
        if (pNext->ElFirstChild)
          {
            *enclosingCell = pNext;
            pNext = pNext->ElFirstChild;
            stop = TRUE;
          }
        else
          /* skip the cell */
          pNext = NextInSelection (pNext, lastSel);
      }
    else
      /* can remove this element */
      stop = TRUE;
  while (!stop);
  return pNext;
}


/*----------------------------------------------------------------------
  NextNotPage returns the next element which is not a page.
  The parameter sibling is TRUE when only direct sibling are checked.
  ----------------------------------------------------------------------*/
PtrElement NextNotPage (PtrElement pEl, ThotBool sibling)
{
  PtrElement          pNext;
  ThotBool            stop;

  if (pEl == NULL)
    return NULL;
  if (sibling)
    pNext = pEl->ElNext;
  else
    pNext = NextElement (pEl);
  stop = FALSE;
  do
    if (pNext == NULL)
      stop = TRUE;
    else if (pNext->ElTerminal && pNext->ElLeafType == LtPageColBreak)
      {
        if (sibling)
          pNext = pNext->ElNext;
        else
          pNext = NextElement (pNext);
      }
    else
      stop = TRUE;
  while (!stop);
  return pNext;
}

/*----------------------------------------------------------------------
  PreviousNotPage returns the previous element which is not a page.
  The parameter sibling is TRUE when only direct sibling are checked.
  ----------------------------------------------------------------------*/
PtrElement PreviousNotPage (PtrElement pEl, ThotBool sibling)
{
  PtrElement          pPrev;
  ThotBool            stop;

  if (pEl == NULL)
    return NULL;
  if (sibling)
    pPrev = pEl->ElPrevious;
  else
    pPrev = PreviousElement (pEl);
  stop = FALSE;
  do
    if (pPrev == NULL)
      stop = TRUE;
    else if (pPrev->ElTerminal && pPrev->ElLeafType == LtPageColBreak)
      {
        if (sibling)
          pPrev = pPrev->ElPrevious;
        else
          pPrev = PreviousElement (pPrev);
      }
    else
      stop = TRUE;
  while (!stop);
  return pPrev;
}

/*----------------------------------------------------------------------
  ProcessFirstLast    Verifie si l'element pPrev devient le   	
  premier parmi ses freres et si pNext devient le dernier 
  Fait reevaluer les regles de presentation "IF First" et 
  "If Last" de ces elements si nececessaire.              
  ----------------------------------------------------------------------*/
void ProcessFirstLast (PtrElement pPrev, PtrElement pNext, PtrDocument pDoc)
{
  if (pNext)
    {
      if (!SiblingElement (pNext, TRUE))
        /* l'element qui suit la partie detruite devient premier */
        ChangeFirstLast (pNext, pDoc, TRUE, FALSE);
    }
  if (pPrev)
    {
      if (!SiblingElement (pPrev, FALSE))
        /* l'element qui precede la partie detruite devient dernier */
        ChangeFirstLast (pPrev, pDoc, FALSE, FALSE);
    }
}

#define MAX_ANCESTOR 10

/*----------------------------------------------------------------------
  CutCommand  cuts the current selection.
  The parameter save is TRUE when the cut contents is saved for future
  paste.
  The parameter replace is TRUE when the cut contents will be replaced.
  Return TRUE if the selection is moved to the next element
  ----------------------------------------------------------------------*/
ThotBool CutCommand (ThotBool save, ThotBool replace)
{
  PtrElement          firstSel, lastSel, pEl, pE, pPrev, pNext, pParent;
  PtrElement          pS, pSS, pParentEl, pFree, pF, pF1, pPrevPage, elSaved;
  PtrElement          pLastSave, pSel, pEl1, pA, firstSelInit, lastSelInit;
  PtrElement          pAncestor[MAX_ANCESTOR];
  PtrElement          pAncestorPrev[MAX_ANCESTOR], pAncestorNext[MAX_ANCESTOR];
  PtrElement          enclosingCell, cellCleared, row;
  PtrPasteElem        pSave;
  DisplayMode         dispMode;
  Document            doc;
  PtrDocument         pSelDoc;
  NotifyElement       notifyEl;
  int                 firstChar, lastChar, nextChar, NSiblings, last, i;
  int                 firstCharInit, lastCharInit, prevDepth, nextDepth;
  ThotBool            oneAtLeast, cutPage, stop, pageSelected, cutAll,
                      recorded, lock, fakeCell, pEfake, ok, selNext,
                      mergeBlocks;

  pPrev = NULL;
  pNext = NULL;
  pParent = NULL;
  elSaved = NULL;
  pLastSave = NULL;
  nextChar = 0;
  pPrevPage = NULL;
  last = 0;
  lock = TRUE;
  cellCleared = NULL;
  pParentEl = NULL;
  selNext = FALSE;
  /* is there a current selection? */
  if (!GetCurrentSelection (&pSelDoc, &firstSel, &lastSel, &firstChar,
                            &lastChar))
    /* no selection */
    return FALSE;
  if (pSelDoc->DocReadOnly)
    /* document is read-only */
    return FALSE;

  /* cherche si l'un des elements selectionne's est protege' */
  stop = FALSE;
  if (firstSel && firstSel->ElTerminal && firstSel->ElLeafType == LtText &&
      firstChar > 1 && ElementIsReadOnly (firstSel))
    /* the selection begins with a substring of a ReadOnly
       element. This substring can't be deleted */
    stop = TRUE;
  else if (lastSel && lastSel->ElTerminal && lastSel->ElLeafType == LtText &&
           lastChar > 1 && lastChar <= lastSel->ElVolume &&
           ElementIsReadOnly (lastSel))
    /* the selection ends with a substring of a ReadOnly
       element. This substring can't be deleted */
    stop = TRUE;
  else if (firstSel && firstSel == lastSel && firstSel->ElTerminal &&
           firstSel->ElLeafType == LtPicture)
    {
      // remove the IMG element instead of the PICTURE
      pEl = firstSel->ElParent;
      if (TypeHasException (ExcIsImg, pEl->ElTypeNumber, pEl->ElStructSchema))
        firstSel = lastSel = pEl;
    }
  pEl = firstSel;
  while (!stop && pEl != NULL)
    if (ElementIsReadOnly (pEl->ElParent))
      stop = TRUE;
    else
      pEl = NextInSelection (pEl, lastSel);
  if (stop)
    /* there are read-only elements in the current selection */
    return FALSE;

  /* conserve la selection initiale pour pouvoir la
     retablir au cas ou la commande ne ferait rien */
  firstSelInit = firstSel;
  lastSelInit = lastSel;
  firstCharInit = firstChar;
  lastCharInit = lastChar;
  /* annule la selection */
  TtaClearViewSelections ();
  /* encore rien detruit */
  oneAtLeast = FALSE;
  cutPage = FALSE;
  /* traitement special pour les pages dans les structures qui le demandent */
  ExcCutPage (&firstSel, &lastSel, pSelDoc, &save, &cutPage);
  /* "remonte" la selection au niveau des freres si c'est possible */
  if (!replace)
    SelectSiblings (&firstSel, &lastSel, &firstChar, &lastChar);

  /* Si tout le contenu d'un element est selectionne', on detruit l'element
     englobant la selection, sauf s'il est indestructible. */
  if (firstSel && firstChar <= 1 &&
      (lastChar == 0 || lastChar > lastSel->ElTextLength))
    /* le premier et le dernier element de la selection sont selectionnes en
       entier */
    {
      cutAll = FALSE;
      if (firstSel->ElPrevious == NULL && lastSel->ElNext == NULL
          && firstSel->ElParent == lastSel->ElParent
          && firstSel->ElParent != NULL)
        /* la selection commence par le premier fils et se termine avec le
           dernier fils du meme pere. On verifie tous les elements
           intermediaires */
        {
          cutAll = TRUE;
          pEl = firstSel;
          do
            if (!CanCutElement (pEl, pSelDoc, NULL))
              /* cannot cut this element */
              cutAll = FALSE;
            else
              {
                /* next selected element */
                pEl1 = pEl->ElNext;
                pEl = NextInSelection (pEl, lastSel);
                if (pEl != NULL && pEl != pEl1)
                  /* cannot cut this element */
                  cutAll = FALSE;
              }
          while (cutAll && pEl != NULL);
        }

      if (cutAll && !replace &&
          firstSel->ElStructSchema &&
          strcmp (firstSel->ElStructSchema->SsName, "SVG") &&
          CanCutElement (firstSel->ElParent, pSelDoc, NULL) &&
          !TypeHasException (ExcIsCell, firstSel->ElParent->ElTypeNumber,
                             firstSel->ElParent->ElStructSchema))
        {
          /* cut the parent instead of all children */
          firstSel = firstSel->ElParent;
          /* get the parent as long as there is only one child */
          while (firstSel &&
                 firstSel->ElPrevious == NULL &&
                 firstSel->ElNext == NULL &&
                 firstSel->ElParent != NULL &&
                 CanCutElement (firstSel->ElParent, pSelDoc, NULL))
            firstSel = firstSel->ElParent;
          lastSel = firstSel;
          /* the whole element is selected */
          firstChar = 0;
          lastChar = 0;
        }
    }

  /* don't delete the root element or an isolated cell or a mandatory element,
     but remove its contents */
  stop = FALSE;
  do
    if (firstSel && firstSel == lastSel && 
        (firstSel->ElParent == NULL ||
         (TypeHasException (ExcIsCell, firstSel->ElTypeNumber,
                          firstSel->ElStructSchema) && !WholeColumnSelected) ||
         !CanCutElement (firstSel, pSelDoc, NULL)))
      {
        if (firstSel->ElTerminal || firstSel->ElFirstChild == NULL)
          {
            /* empty element */
            firstSel = NULL;
            elSaved = NULL;
            stop = TRUE;
          }
        else
          {
            /* remove the content */
            if (TypeHasException (ExcIsCell, firstSel->ElTypeNumber,
                                  firstSel->ElStructSchema))
              /* note that a cell is cleared for the future selection */
              cellCleared = firstSel;
            firstSel = firstSel->ElFirstChild;
            lastSel = firstSel;
            while (lastSel->ElNext != NULL)
              lastSel = lastSel->ElNext;
          }
      }
    else
      stop = TRUE;
  while (!stop);

  if (firstSel)
    {
      /* cherche l'element qui precede la partie selectionnee */
      pPrev = PreviousNotPage (firstSel, !replace);
      /* cherche le premier element apres la selection */
      pNext = NextNotPage (lastSel, replace);
      nextChar = 0;
      if (firstSel->ElTerminal &&
          firstSel->ElLeafType == LtText &&
          firstSel->ElTextLength > 0 &&
          firstSel->ElTextLength < firstChar &&
          firstSel != lastSel)
        /* debut de la selection apres l'element complet */
        {
          firstSel = NextInSelection (firstSel, lastSel);
          firstChar = 0;
          pPrev = firstSel;
        }
    }

  if (!firstSel)
    return FALSE;

  doc = IdentDocument (pSelDoc);
  if (save)
    // start a new copy
    TtaExecuteMenuAction ("StartACopyCommand", doc, 1, TRUE);
  dispMode = TtaGetDisplayMode (doc);
  /* lock tables formatting */
  TtaGiveTableFormattingLock (&lock);
  if (!lock)
    {
      if (dispMode == DisplayImmediately)
        TtaSetDisplayMode (doc, DeferredDisplay);
      /* table formatting is not loked, lock it now */
      TtaLockTableFormatting ();
    }

  /* open the sequence of editing operations for the history */
  OpenHistorySequence (pSelDoc, firstSelInit, lastSelInit, NULL, firstCharInit,
                       lastCharInit);
  recorded = FALSE;
  if (firstChar > 1)
    /* la selection commence a l'interieur d'un element */
    /* coupe le premier element selectionne */
    {
      AddEditOpInHistory (firstSel, pSelDoc, TRUE, TRUE);
      recorded = TRUE;
      pPrev = firstSel;
      SplitBeforeSelection (&firstSel, &firstChar, &lastSel,&lastChar,pSelDoc);
    }

  /* On a selectionne' seulement une marque de page ? */
  pageSelected = cutPage;
  if (firstSel == lastSel && firstSel->ElTerminal &&
      firstSel->ElLeafType == LtPageColBreak)
    pageSelected = TRUE;
  /* record the sibling of the ancestors */
  if (firstSel == NULL)
    pParent = NULL;
  else
    pParent = firstSel->ElParent;
  pAncestorPrev[0] = pPrev;
  pAncestorNext[0] = pNext;
  pPrev = firstSel;
  pNext = lastSel;
  if (replace)
    pAncestor[0] = NULL;
  else
    {
      for (i = 0; i < MAX_ANCESTOR; i++)
        {
          pAncestor[i] = pParent;
          if (pParent)
            pParent = pParent->ElParent;
          if (i > 0)
            {
              if (pPrev == NULL)
                pAncestorPrev[i] = NULL;
              else
                {
                  pPrev = pPrev->ElParent;
                  if (pPrev == NULL)
                    pAncestorPrev[i] = NULL;
                  else
                    pAncestorPrev[i] = pPrev->ElPrevious;
                }
              if (pNext == NULL)
                pAncestorNext[i] = NULL;
              else
                {
                  pNext = pNext->ElParent;
                  if (pNext == NULL)
                    pAncestorNext[i] = NULL;
                  else
                    pAncestorNext[i] = pNext->ElNext;
                }
            }
        }
    }

  mergeBlocks = FALSE;
  if (!IsXMLEditMode ())
    /* we are in text mode. check whether the elements to be deleted cross
       a block boundary */
    {
      pE = firstSel;
      while (pE && !TypeHasException (ExcParagraphBreak, pE->ElTypeNumber,
                                      pE->ElStructSchema))
        pE = pE->ElParent;
      if (pE && pE != firstSel)
        /* the first element is within a block element */
        {
          pF = lastSel;
          while (pF && !TypeHasException (ExcParagraphBreak, pF->ElTypeNumber,
                                          pF->ElStructSchema))
            pF = pF->ElParent;
          if (pF && pF != lastSel && pF != pE)
            /* the last element is within another block element. We will
               try to merge these block elements, after the deletion of the
               current selection */
            mergeBlocks = TRUE;
        }
    }

  pEl = firstSel;	/* first selected element */
  pS = NULL;
  elSaved = NULL;
  pLastSave = NULL;
  pFree = NULL;
  enclosingCell = NULL;
  fakeCell = FALSE;
  if (WholeColumnSelected && FirstSelectedColumn)
    {
      /* send the ElemDelete.Pre event for the column head and check if the
         delete operation is accepted */
      notifyEl.event = TteElemDelete;
      notifyEl.document = doc;
      notifyEl.element = (Element) FirstSelectedColumn;
      notifyEl.info = 0; /* not sent by undo */
      notifyEl.elementType.ElTypeNum = FirstSelectedColumn->ElTypeNumber;
      notifyEl.elementType.ElSSchema = (SSchema) (FirstSelectedColumn->ElStructSchema);
      NSiblings = 0;
      pF = FirstSelectedColumn;
      while (pF->ElPrevious != NULL)
        {
          NSiblings++;
          pF = pF->ElPrevious;
        }
      notifyEl.position = NSiblings;
      if (CallEventType ((NotifyEvent *) (&notifyEl), TRUE))
        /* the delete is refused */
        pEl = NULL;
      else if (NextCellInColumnFunction)
        {
          row = NULL;
          (*(Proc5)NextCellInColumnFunction) ((void*)(&pEl), (void*)(&row),
                                              (void*)FirstSelectedColumn,
                                              (void*)doc, (void*)(&fakeCell));
        }
    }

  /* handle all selected elements */
  while (pEl && pEl->ElStructSchema)
    {
      if (!pageSelected)
        /* On ne detruit pas les marques de pages,
           sauf si rien d'autre n'a ete selectionne'.
           On ne detruit pas non plus les elements
           indestructibles */
        pEl = NextElemToBeCut (pEl, lastSel, pSelDoc, elSaved, &enclosingCell);
      if (pEl)
        {
          /* pE : pointeur sur l'element a detruire */
          pE = pEl;
          pEfake = fakeCell;
          if (pE == lastSel)
            {
              /* that's the last selected element */
              if (lastSel->ElTerminal &&
                  lastSel->ElLeafType == LtText &&
                  lastChar > 1 &&
                  lastChar <= lastSel->ElTextLength)
                /* la selection se termine a l'interieur d'un element,
                   on le coupe en deux */
                {
                  AddEditOpInHistory (lastSel, pSelDoc, TRUE, FALSE);
                  recorded = TRUE;
                  SplitAfterSelection (lastSel, lastChar, pSelDoc);
                  pNext = lastSel->ElNext;
                  AddEditOpInHistory (pNext, pSelDoc, FALSE, TRUE);
                  pAncestorNext[0] = pNext;
                }
              pEl = NULL;
            }
          else if (pageSelected && !cutPage &&
                   pE->ElTypeNumber == PageBreak+1)
            {
              /* remove the page break */
              pPrevPage = pE->ElPrevious;
              /* no new element in the selection */
              pEl = NULL;
            }
          else
            {
              /* next selected element */
              if (enclosingCell)
                {
                  /* removing enclosed elements of a cell */
                  pEl = pEl->ElNext;
                  if (pEl == NULL)
                    {
                      pEl = NextInSelection (enclosingCell, lastSel);
                      enclosingCell = NULL;
                    }
                }
              else if (NextCellInColumnFunction &&
                       WholeColumnSelected && FirstSelectedColumn)
                /* deleting all cells of a table column */
                (*(Proc5)NextCellInColumnFunction) ((void*)(&pEl),
                                     (void*)(&row), (void*)FirstSelectedColumn,
                                     (void*)doc, (void*)(&fakeCell));
              else
                {
                  pEl = NextInSelection (pEl, lastSel);
                  if (pEl && pEl == lastSel &&
                      pEl->ElTerminal &&
                      pEl->ElLeafType == LtText &&
                      lastChar <= 1)
                    /* this is the last selected element, but selection
                       ends before its first char. do not delete it */
                    pEl = NULL;
                }
            }

          /* verifie qu'il ne s'agit pas d'un element de paire dont
             l'homologue ne serait pas dans la selection */
          if (!IsolatedPairedElem (pE, pE, lastSel))
            {
              if (!pEfake &&
                  NextInSelection (pE, lastSel) == NULL)
                last = TTE_STANDARD_DELETE_LAST_ITEM;
              else
                last = TTE_STANDARD_DELETE_FIRST_ITEMS;
            }
          if (pEfake)
            {
              ok = TRUE;
              pParentEl = NULL;
            }
          else
            ok = !SendEventSubTree (TteElemDelete, pSelDoc, pE, last, 0,
                                    FALSE, FALSE);
          if (ok)
            {
              if (!pEfake)
                {
                  /* delete abstract boxes of the element */
                  DestroyAbsBoxes (pE, pSelDoc, TRUE);
                  /* conserve un pointeur sur le pere */
                  pParentEl = pE->ElParent;
                  /* send the ElemDelete.Pre event and check
                     if the delete operation is accepted */
                  notifyEl.event = TteElemDelete;
                  notifyEl.document = doc;
                  notifyEl.element = (Element) pParentEl;
                  notifyEl.info = 0; /* not sent by undo */
                  notifyEl.elementType.ElTypeNum = pE->ElTypeNumber;
                  notifyEl.elementType.ElSSchema = (SSchema) (pE->ElStructSchema);
                  NSiblings = 0;
                  pF = pE;
                  while (pF->ElPrevious != NULL)
                    {
                      NSiblings++;
                      pF = pF->ElPrevious;
                    }
                  notifyEl.position = NSiblings;

                  if (!recorded)
                    {
                      /* record that deletion in the history */
                      AddEditOpInHistory (pE, pSelDoc, TRUE,FALSE);
                      if (WholeColumnSelected)
                        /* change the value of "info" in the latest cell
                           deletion recorded in the Undo queue. The goal is
                           to allow procedure CellPasted to regenerate only
                           one column head when undoing the operation */
                        TtaChangeInfoLastRegisteredElem (doc, 3);
                    }
                  recorded = FALSE;

                  /* retire l'element courant de l'arbre */
                  pA = GetOtherPairedElement (pE);
                  RemoveElement (pE);
                  /* Si c'est un membre d'une paire de marques, indique a
                     l'autre membre qu'il doit etre detruit aussi */
                  if (pA != NULL)
                    pA->ElOtherPairedEl = pA;
                  SetDocumentModified (pSelDoc, TRUE, 30);
                  CallEventType ((NotifyEvent *) (&notifyEl), FALSE);
                  oneAtLeast = TRUE;
                  if (pageSelected &&
                      pE->ElTypeNumber == PageBreak + 1)
                    /* essaie de fusionner l'element qui precede le saut de
                       page supprime' avec celui qui suit */
                    if (pPrevPage != NULL)
                      /* il y a un elem. precedent */
                      {
                        nextChar = pPrevPage->ElTextLength + 1;
                        if (!IsIdenticalTextType (pPrevPage, pSelDoc, &pF))
                          /* il n'y a pas eu de fusion */
                          nextChar = 0;
                        else
                          /* il y a eu fusion */
                          {
                            if (pPrevPage == pPrev)
                              pNext = pPrev;
                            if (pF != NULL)
                              /* chaine l'element libere' par la fusion a
                                 la fin de la liste des elements a liberer*/
                              {
                                pF->ElNext = NULL;
                                /* il est le dernier de la liste */
                                if (pFree == NULL)
                                  /* la liste est vide */
                                  pFree = pF;
                                else
                                  /* cherche la fin de la liste */
                                  {
                                    pF1 = pFree;
                                    while (pF1->ElNext != NULL)
                                      pF1 = pF1->ElNext;
                                    /* chaine l'element en fin de liste */
                                    pF1->ElNext = pF;
                                  }
                              }
                          }
                      }
                }
              if (save)
                {
                  if (pS == NULL)
                    {
                      /* libere l'ancienne sauvegarde */
                      FreeSavedElements ();
                      /* document d'ou vient la partie sauvee */
                      DocOfSavedElements = pSelDoc;
                      /* tell the application what document the saved elements
                         come from */
                      if (CopyAndCutFunction)
                        (*(Proc1)CopyAndCutFunction) ((void *) doc);
                    }
                  /* il ne faudra pas changer les labels des elements
                     exportables inseres par la prochaine commande Paste */
                  ChangeLabel = FALSE;
                  ok = SendEventSubTree (TteElemCopy, pSelDoc, pE, 0, 0,
                                         FALSE, FALSE);
                  if (!ok)
                    /* save the current element */
                    SaveElement (pE, pParentEl, doc, NULL);
                }
              if (pS == NULL)
                elSaved = pE;
              else
                {
                  pS->ElNext = pE;
                  pE->ElPrevious = pS;
                }
              pS = pE;
              pLastSave = pE;
              /* keep the link to parent */
              pE->ElParent = pParentEl;
            }

          if (last == TTE_STANDARD_DELETE_LAST_ITEM && !cutPage)
            pEl = NULL;
        }
    }

  if (WholeColumnSelected && FirstSelectedColumn)
    {
      /* delete abstract boxes of the column head */
      DestroyAbsBoxes (FirstSelectedColumn, pSelDoc, TRUE);
      notifyEl.event = TteElemDelete;
      notifyEl.document = doc;
      notifyEl.element = (Element) FirstSelectedColumn->ElParent;
      notifyEl.info = 0; /* not sent by undo */
      notifyEl.elementType.ElTypeNum = FirstSelectedColumn->ElTypeNumber;
      notifyEl.elementType.ElSSchema = (SSchema) (FirstSelectedColumn->ElStructSchema);
      NSiblings = 0;
      pF = FirstSelectedColumn;
      while (pF->ElPrevious != NULL)
        {
          NSiblings++;
          pF = pF->ElPrevious;
        }
      notifyEl.position = NSiblings;
      /* record that deletion in the history */
      AddEditOpInHistory (FirstSelectedColumn, pSelDoc, TRUE, FALSE);
      RemoveElement (FirstSelectedColumn);
      /* send the ElemDelete.Post event */
      CallEventType ((NotifyEvent *) (&notifyEl), FALSE);
    }

  /* les elements a couper ont ete coupe's. Verifie que les elements
     suivant et precedent n'ont pas ete detruits par les actions
     declanchees par les evenements TteElemDelete. Cherche d'abord le
     premier ancetre qui soit encore dans le document */
  pParent = NULL;
  for (i = 0; i < MAX_ANCESTOR && pParent == NULL; i++)
    {
      if (pAncestor[i] == NULL)
        {
          i = MAX_ANCESTOR;
          pParent = pParentEl;
        }
      else if (pAncestor[i]->ElStructSchema &&
               DocumentOfElement (pAncestor[i]) == pSelDoc)
        pParent = pAncestor[i];
    }

  if (pAncestorNext[0] && pAncestorNext[0]->ElStructSchema &&
      DocumentOfElement (pAncestorNext[0]) == pSelDoc)
    pNext = pAncestorNext[0];
  else
    {
      pAncestorNext[0] = NULL;
      pNext = NULL;
    }
  for (i = 0; i < MAX_ANCESTOR && pNext == NULL; i++)
    {
      if (pAncestor[i] == NULL)
        i = MAX_ANCESTOR;
      else if (pAncestorNext[i])
        {
          if (pAncestorNext[i]->ElStructSchema &&
              DocumentOfElement (pAncestorNext[i]) == pSelDoc)
            pNext = pAncestorNext[i];
          else if (pAncestorPrev[i] &&
                   pAncestorPrev[i]->ElStructSchema &&
                   DocumentOfElement (pAncestorPrev[i]) == pSelDoc &&
                   pAncestorPrev[i]->ElNext)
            pNext = pAncestorPrev[i]->ElNext;
        }
    }

  if (pAncestorPrev[0] && pAncestorPrev[0]->ElStructSchema &&
      DocumentOfElement (pAncestorPrev[0]) == pSelDoc)
    pPrev = pAncestorPrev[0];
  else
    {
      pAncestorPrev[0] = NULL;
      pPrev = NULL;
    }
  for (i = 0; i < MAX_ANCESTOR && pPrev == NULL; i++)
    {
      if (pAncestor[i] == NULL)
        i = MAX_ANCESTOR;
      else if (pAncestorPrev[i])
        {
          if (pAncestorPrev[i]->ElStructSchema &&
              DocumentOfElement (pAncestorPrev[i]) == pSelDoc)
            pPrev = pAncestorPrev[i];
          else if (pAncestorNext[i] &&
                   pAncestorNext[i]->ElStructSchema &&
                   DocumentOfElement (pAncestorNext[i]) == pSelDoc &&
                   pAncestorNext[i]->ElPrevious)
            pPrev = pAncestorNext[i]->ElPrevious;
        }
    }

  /* in text mode, merge blocks if the deleted elements were crossing
     block boudaries */
  if (mergeBlocks)
    {
      AbstractImageUpdated (pSelDoc);
      DeleteNextChar (1, pAncestorPrev[0], FALSE);
    } 

  if (!replace)
    CloseHistorySequence (pSelDoc);
 
  /* reaffiche les paves qui copient les elements detruits */
  if (oneAtLeast)
    {
      pS = elSaved;
      while (pS != NULL)
        /* parcourt la chaine des elements detruits */
        {
          RedisplayCopies (pS, pSelDoc, TRUE);
          /* element detruit suivant */
          pS = pS->ElNext;
        }
      /* renumerote la suite */
      pE = elSaved;
      pS = pNext;
      if (pS == NULL)
        pS = NextElement (pPrev);
      if (pS != NULL)
        while (pE != NULL)
          {
            UpdateNumbers (pS, pE, pSelDoc, TRUE);
            pE = pE->ElNext;
          }
      pNext = pS;
      /* annule les pointeurs vers le pere */
      pS = elSaved;
      while (pS != NULL)
        /* parcourt la chaine des elements detruits */
        {
          pS->ElParent = NULL;
          pS = pS->ElNext;
          /* element detruit suivant */
        }

      /* some element were destroyed, finish the work: check if sibling
         elements become first or last */
      ProcessFirstLast (pPrev, pNext, pSelDoc);
      /* reaffiche toutes les vues */
      AbstractImageUpdated (pSelDoc);
      RedisplayDocViews (pSelDoc);
      /* libere les elements qui ont ete fusionnes */
      if (pFree != NULL)
        {
          pF = pFree;
          while (pF != NULL)
            {
              pF1 = pF->ElNext;
              /* element suivant a liberer */
              DeleteElement (&pF, pSelDoc);
              /* libere l'element courant */
              pF = pF1;
              /* passe au suivant */
                }
        }
 
      /* la renumerotation est faite plus haut. Reaffiche les references
         aux elements detruits et enregistre les references sortantes
         coupees ainsi que les elements coupe's qui sont reference's par
         d'autres documents */
      pS = elSaved;
      while (pS != NULL)
        /* parcourt la chaine des elements detruits */
        {
          RedisplayEmptyReferences (pS, &pSelDoc, TRUE);
          pS = pS->ElNext;
          /* element detruit suivant */
        }
      /* Retransmet les valeurs des compteurs et attributs TRANSMIT si il
         y a des elements apres */
      if (pPrev != NULL)
        pSS = pPrev;
      else
        pSS = pParent;
      pS = elSaved;
      while (pS != NULL)
        /* parcourt la chaine des elements detruits */
        {
          if ((pS->ElStructSchema->SsRule->SrElem[pS->ElTypeNumber - 1]->SrRefImportedDoc))
            RepApplyTransmitRules (pS, pSS, pSelDoc);
          pS = pS->ElNext;
          /* element detruit suivant */
        }
    }
  if (!lock)
    {
      /* unlock table formatting */
      TtaUnlockTableFormatting ();
      if (dispMode == DisplayImmediately)
        TtaSetDisplayMode (doc, DisplayImmediately);
    }

  /* close the command */
  /* did the selection change? */
  GetCurrentSelection (&pSelDoc, &firstSel, &lastSel, &firstChar, &lastChar);
  if (oneAtLeast &&
      (firstSel == elSaved || ElemIsWithinSubtree (firstSel, elSaved) ||
       lastSel == pLastSave || ElemIsWithinSubtree (lastSel, pLastSave)))
    {
      /* the selection points to deleted elements. Set a new selection */
      /* first, get the depth of the previous and next elements */
      prevDepth = 0;
      pE = pPrev;
      while (pE)
        {
          prevDepth++;
          pE = pE->ElParent;
        }
      nextDepth = 0;
      pE = pNext;
      while (pE)
        {
          nextDepth++;
          pE = pE->ElParent;
        }
      if (cellCleared)
        SelectElementWithEvent (pSelDoc, cellCleared, TRUE, TRUE);
      else if (replace && pParent && pParent->ElFirstChild == NULL)
        /* select the empty parent */
        SelectElementWithEvent (pSelDoc, pParent, TRUE, TRUE);
      else if (replace && pPrev && prevDepth >= nextDepth)
        {
          /* try first to select the end of the previous element */
          pSel = LastLeaf (pPrev);
          if (pSel->ElTerminal && pSel->ElLeafType == LtText)
            SelectPositionWithEvent (pSelDoc, pSel, pSel->ElTextLength + 1, TRUE);
          else
            SelectElementWithEvent (pSelDoc, pSel, FALSE, TRUE);
        }
      else if (pNext && nextDepth >= prevDepth)
        {
          /* there is a next element and it's deeper in the abstract tree.
             Select its beginning */
          if (nextChar == 0)
            {
              /* select the following element */
              pSel = FirstLeaf (pNext);
              selNext = TRUE;
              /* Select the first character or the whole element */
              if (pSel->ElTerminal && pSel->ElLeafType == LtText)
                SelectPositionWithEvent (pSelDoc, pSel, 1, TRUE);
              else if (pSel->ElTerminal && pSel->ElLeafType == LtSymbol)
                SelectPositionWithEvent (pSelDoc, pSel, 1, TRUE);
              else
                SelectElementWithEvent (pSelDoc, pSel, TRUE, TRUE);
            }
          else
            SelectPositionWithEvent (pSelDoc, pNext, nextChar, TRUE);
        }
      else if (pPrev)
        /* no following element, select the previous */
        {
          pSel = LastLeaf (pPrev);
          if (pSel->ElTerminal && pSel->ElLeafType == LtText)
            SelectPositionWithEvent (pSelDoc, pSel, pSel->ElTextLength + 1, TRUE);
          else
            SelectElementWithEvent (pSelDoc, pSel, FALSE, TRUE);
        }
      else
        /* no previous, select the parent */
        SelectElementWithEvent (pSelDoc, pParent, TRUE, TRUE);
    }
  else
    /* reset the selection */
    HighlightSelection (FALSE, FALSE);

      
  if (save)
    {
      /* look for and change references */
      pSave = FirstSavedElement;
      while (pSave)
        {
          /* update references */
          //ChangeReferences (pSave->PeElement, &pSelDoc);
          /* send the ElemCopy.Post event to the whole subtree */
          NotifySubTree (TteElemCopy, pSelDoc, pSave->PeElement, 0, 0,
                         FALSE, FALSE);
          /* next subtree */
          pSave = pSave->PeNext;
        }
    }
  else
    {
      /* libere les elements coupe's */
      pE = elSaved;
      while (pE)
        {
          pS = pE->ElNext;
          DeleteElement (&pE, pSelDoc);
          pE = pS;
        }
    }
  return selNext;
}

/*----------------------------------------------------------------------
  TtaSetCopyAndCutFunction registers the function to be called when
  a Copy or Cut operation is executed:
  void procedure (Docucment doc)
  ----------------------------------------------------------------------*/
void TtaSetCopyAndCutFunction (Proc1 procedure)
{
  CopyAndCutFunction = procedure;
}

/*----------------------------------------------------------------------
  TtaSetCopyCellFunction registers the function to be called when
  a cell of a row or a column is copied:
  void procedure (Element el, Docucment doc, ThotBool inRow)
  ----------------------------------------------------------------------*/
void TtaSetCopyCellFunction (Proc3 procedure)
{
  CopyCellFunction = procedure;
}

/*----------------------------------------------------------------------
  TtaSetCopyRowFunction registers the function to be called when
  a row of a table is copied:
  void procedure (Element copy, Element orig, Docucment doc)
  ----------------------------------------------------------------------*/
void TtaSetCopyRowFunction (Proc3 procedure)
{
  CopyRowFunction = procedure;
}

/*----------------------------------------------------------------------
  TtaSetNextCellInColumnFunction registers the function to be called when
  a cell of a table is copied:
  ----------------------------------------------------------------------*/
void TtaSetNextCellInColumnFunction (Proc5 procedure)
{
  NextCellInColumnFunction = procedure;
}

/*----------------------------------------------------------------------
  EmptyElement    retourne vrai si l'element pEl est vide.        
  ----------------------------------------------------------------------*/
ThotBool EmptyElement (PtrElement pEl)
{
  ThotBool            empty;
  PtrElement          pChild;

  if (pEl->ElVolume == 0)
    empty = TRUE;
  else if (TypeHasException (ExcEmptyGraphic, pEl->ElTypeNumber, pEl->ElStructSchema))
    empty = TRUE;
  else if (pEl->ElStructSchema->SsRule->SrElem[pEl->ElTypeNumber - 1]->SrConstruct == CsConstant)
    empty = TRUE;
  else if (pEl->ElTerminal)
    empty = FALSE;
  else
    {
      empty = TRUE;
      pChild = pEl->ElFirstChild;
      while (pChild != NULL && empty)
        {
          empty = EmptyElement (pChild);
          pChild = pChild->ElNext;
        }
    }
  return empty;
}


/*----------------------------------------------------------------------
  CanSurround indique si un element de type (typeNum, pSS) peut etre 
  le pere des elements freres compris entre l'element     
  firstEl et l'elemnt lastEl, dans le document pDoc.      
  ----------------------------------------------------------------------*/
static ThotBool CanSurround (int typeNum, PtrSSchema pSS, PtrElement firstEl,
                             PtrElement lastEl, PtrDocument pDoc)
{
  PtrElement          pEl, pElSurround;
  ThotBool            ok;

  ok = TRUE;
  /* on teste d'abord le constructeur du candidat englobant */
  switch (pSS->SsRule->SrElem[typeNum - 1]->SrConstruct)
    {
    case CsConstant:
    case CsReference:
    case CsBasicElement:
    case CsPairedElement:
    case CsEmpty:
      /* types sans descendance possible, inutile d'aller plus loin */
      ok = FALSE;
      break;
    case CsChoice:
      if (pSS->SsRule->SrElem[typeNum - 1]->SrNChoices == -1)
        /* NATURE, on n'a pas envie de changer de schema de structure */
        ok = FALSE;
      break;
    default:
      /* on va regarder de plus pres... */
      ok = TRUE;
      break;
    }
  if (ok && firstEl != NULL)
    /* le constructeur du candidat englobant permet une descendance */
    /* on verifie que les candidats englobe's peuvent etre ses */
    /* descendants */
    {
      /* on cree un element du type du candidat englobant */
      pElSurround = NewSubtree (typeNum, pSS, pDoc, FALSE, TRUE, FALSE,
                                FALSE);
      /* on insere temporairement cet element dans l'arbre, comme frere */
      /* precedent du premier element a englober */
      InsertElementBefore (firstEl, pElSurround);
      pEl = firstEl;
      /* on parcourt tous les candidats englobe's */
      while (pEl != NULL && ok)
        {
          /* on verifie si un element du type de cet element a englober */
          /* peut etre un fils du candidat englobant */
          ok = AllowedFirstChild (pElSurround, pDoc, pEl->ElTypeNumber,
                                  pEl->ElStructSchema, TRUE, FALSE);
          if (ok)
            {
              /* passe a l'element suivant */
              if (pEl == lastEl)
                pEl = NULL;
              else
                pEl = pEl->ElNext;
            }
        }
      /* retire et libere l'element cree' temporairement */
      DeleteElement (&pElSurround, pDoc);
    }
  return ok;
}

/*----------------------------------------------------------------------
  DoSurround
  ----------------------------------------------------------------------*/
static ThotBool DoSurround (PtrElement firstEl, PtrElement lastEl,
                            int firstChar, int lastChar, PtrDocument pDoc,
                            int typeNum, PtrSSchema pSS)
{
  PtrElement          pSibling, pEl, pRoot, pElSurround, pNext, pPrev,
    pEl1;
  NotifyElement       notifyEl;
  int                 NSiblings;
  ThotBool            unit, splitElem, ok;

  /* l'element devant lequel on va creer le nouvel element englobant */
  pEl1 = firstEl;
  /* on verifie qu'on produit bien une structure correcte */
  splitElem = FALSE;
  ok = FALSE;
  do
    {
      ok = AllowedSibling (pEl1, pDoc, typeNum, pSS, TRUE, FALSE, FALSE);
      if (!ok)
        {
          if ((firstEl->ElTerminal && firstEl->ElLeafType == LtText &&
               firstChar > 1) ||
              (pEl1->ElPrevious != NULL))
            splitElem = TRUE;
          pEl1 = pEl1->ElParent;
        }
    }
  while (pEl1 != NULL && !ok);
  if (pEl1 == NULL || !ok)
    return FALSE;

  /* demande a l'application si on peut creer ce type d'element */
  notifyEl.event = TteElemNew;
  notifyEl.document = (Document) IdentDocument (pDoc);
  notifyEl.element = (Element) (pEl1->ElParent);
  notifyEl.info = 0; /* not sent by undo */
  notifyEl.elementType.ElTypeNum = typeNum;
  notifyEl.elementType.ElSSchema = (SSchema) pSS;
  pSibling = pEl1;
  NSiblings = 1;
  while (pSibling->ElPrevious != NULL)
    {
      NSiblings++;
      pSibling = pSibling->ElPrevious;
    }
  notifyEl.position = NSiblings;
  if (CallEventType ((NotifyEvent *) & notifyEl, TRUE))
    /* l'application refuse la creation de ce type d'element */
    pEl1 = NULL;
  else
    /* l'application accepte la creation de ce type d'element */
    {
      /* demande a l'application si elle accepte de deplacer les elements
         qui doivent l'etre */
      pEl = firstEl;
      while (pEl != NULL)
        {
          notifyEl.event = TteElemMove;
          notifyEl.document = (Document) IdentDocument (pDoc);
          notifyEl.element = (Element) pEl;
          notifyEl.info = 0; /* not sent by undo */
          notifyEl.elementType.ElTypeNum = pEl->ElTypeNumber;
          notifyEl.elementType.ElSSchema = (SSchema) (pEl->ElStructSchema);
          notifyEl.position = 0;
          if (CallEventType ((NotifyEvent *) & notifyEl, TRUE))
            /* l'application refuse */
            {
              pEl = NULL;
              pEl1 = NULL;
            }
          else
            /* l'application accepte. Passe a l'element suivant */
            if (pEl == lastEl)
              pEl = NULL;
            else
              pEl = pEl->ElNext;
        }
    }
  if (pEl1 == NULL)
    return FALSE;
  else
    {
      /* on cree un element du type choisi par l'utilisateur */
      pElSurround = NewSubtree (typeNum, pSS, pDoc, FALSE, TRUE, TRUE, TRUE);
      pRoot = pElSurround;
      unit = FALSE;
      if (strcmp (pEl1->ElParent->ElStructSchema->SsName, "Template") &&
          strcmp (pElSurround->ElStructSchema->SsName,
                  pEl1->ElParent->ElStructSchema->SsName))
        {
          /* cet element appartient a un schema de structure different de */
          /* celui de l'element qui devrait devenir son pere */
          if (pElSurround->ElStructSchema->SsRule->SrElem[pElSurround->ElTypeNumber - 1]->SrUnitElem)
            /* c'est une unite', on le note */
            unit = TRUE;
          else if (pElSurround->ElTypeNumber != pElSurround->ElStructSchema->SsRootElem)
            /* ce n'est pas la racine de son schema */
            {
              /* on cree un element racine du schema */
              pRoot = NewSubtree (pElSurround->ElStructSchema->SsRootElem,
                                  pElSurround->ElStructSchema, pDoc,
                                  FALSE, TRUE, TRUE, TRUE);
              /* l'element qui va englober les elements selectionne's */
              /* devient le fils de ce nouvel element */
              InsertFirstChild (pRoot, pElSurround);
            }
        }

      if (pRoot == NULL || pElSurround == NULL)
        /* impossible de creer l'element voulu, abandon */
        return FALSE;
      /* on a cree' l'element voulu */
      TtaClearViewSelections ();	/* on annule d'abord la selection */
      /* on coupe eventuellement les atomes de texte */
      SplitBeforeSelection (&firstEl, &firstChar, &lastEl, &lastChar, pDoc);
      SplitAfterSelection (lastEl, lastChar, pDoc);

      if (splitElem)
        /* coupe l'element en deux */
        if (BreakElement (pEl1, firstEl, firstChar, FALSE, FALSE))
          pEl1 = pEl1->ElNext;

      /* on detruit les paves des elements qui vont etre deplaces */
      pEl = firstEl;
      while (pEl != NULL)
        {
          DestroyAbsBoxes (pEl, pDoc, TRUE);
          if (pEl == lastEl)
            pEl = NULL;
          else
            pEl = pEl->ElNext;
        }
      AbstractImageUpdated (pDoc);

      /* on insere l'element cree' comme frere precedent du premier */
      /* element a englober */
      InsertElementBefore (pEl1, pRoot);
      AddEditOpInHistory (pRoot, pDoc, FALSE, TRUE);
      pEl = firstEl;
      pPrev = NULL;
      /* on parcourt tous les elements a englober et on les deplace */
      /* pour les inserer comme fils de l'element englobant */
      while (pEl != NULL)
        {
          pNext = pEl->ElNext;
          AddEditOpInHistory (pEl, pDoc, TRUE, FALSE);
          /* retire un element de l'arbre abstrait */
          RemoveElement (pEl);
          /* si c'est une feuille unite', elle prend le schema de structure
             de son futur pere. */
          if (unit)
            if (pEl->ElTerminal)
              if (pEl->ElLeafType == LtText ||
                  pEl->ElLeafType == LtPicture ||
                  pEl->ElLeafType == LtPolyLine ||
                  pEl->ElLeafType == LtPath ||
                  pEl->ElLeafType == LtSymbol ||
                  pEl->ElLeafType == LtGraphics)
                pEl->ElStructSchema = pElSurround->ElStructSchema;
          /* place l'element a sa nouvelle position */
          if (pPrev == NULL)
            InsertFirstChild (pElSurround, pEl);
          else
            InsertElementAfter (pPrev, pEl);
          pPrev = pEl;
          /* on envoie un evenement ElemMove.Post a l'application */
          notifyEl.event = TteElemMove;
          notifyEl.document = (Document) IdentDocument (pDoc);
          notifyEl.element = (Element) pEl;
          notifyEl.info = 0; /* not sent by undo */
          notifyEl.elementType.ElTypeNum = pEl->ElTypeNumber;
          notifyEl.elementType.ElSSchema = (SSchema) (pEl->ElStructSchema);
          notifyEl.position = 0;
          CallEventType ((NotifyEvent *) & notifyEl, FALSE);
          /* passe a l'element suivant */
          if (pEl == lastEl)
            pEl = NULL;
          else
            pEl = pNext;
        }
      /* on cree les elements manquants dans l'element qui vient */
      /* d'etre cree', et on lui met les attributs obligatoires */
      CompleteElement (pElSurround, pDoc);
      RemoveExcludedElem (&pRoot, pDoc);
      if (pDoc->DocCheckingMode & COMPLETE_CHECK_MASK)
        /*if (FullStructureChecking)*/
        AttachMandatoryAttributes (pElSurround, pDoc);
      if (pDoc->DocSSchema != NULL)
        /* le document n'a pas ete ferme' pendant que l'utilisateur avait */
        /* le controle pour donner les valeurs des attributs obligatoires */
        {
          /* on envoie un evenement ElemNew.Post a l'application */
          notifyEl.event = TteElemNew;
          notifyEl.document = (Document) IdentDocument (pDoc);
          notifyEl.element = (Element) pElSurround;
          notifyEl.info = 0; /* not sent by undo */
          notifyEl.elementType.ElTypeNum = pElSurround->ElTypeNumber;
          notifyEl.elementType.ElSSchema = (SSchema) (pElSurround->ElStructSchema);
          notifyEl.position = 0;
          CallEventType ((NotifyEvent *) & notifyEl, FALSE);
          /* cree les paves du nouvel element et reaffiche */
          CreateAllAbsBoxesOfEl (pRoot, pDoc);
          AbstractImageUpdated (pDoc);
          RedisplayDocViews (pDoc);
          /* si on est dans un element copie' par inclusion, on met a jour
             les copies de cet element. */
          RedisplayCopies (pRoot, pDoc, TRUE);
          /* on met a jour les numeros affectes par la creation des */
          /* nouveaux elements */
          UpdateNumbers (NextElement (pRoot), pRoot, pDoc, TRUE);
          /* on indique que le document est modifie' */
          SetDocumentModified (pDoc, TRUE, 30);
          /* on selectionne l'element englobant */
          SelectElementWithEvent (pDoc, pElSurround, TRUE, TRUE);
        }
    }
  return TRUE;
}

/*----------------------------------------------------------------------
  SearchChoiceRules cherche les regles CsChoice qui derivent de la   
  regle (pSS, typeNum) et qui menent au type de l'element pEl.	
  ----------------------------------------------------------------------*/
static ThotBool SearchChoiceRules (PtrSSchema pSS, int typeNum,
                                   PtrElement pEl, int *param)
{
  PtrSRule             pSRule;
  PtrChoiceOptionDescr pChoiceD, *Anchor;
  int                  choice;
  ThotBool             found, doit;

  found = FALSE;
  Anchor = (PtrChoiceOptionDescr *) param;
  if (typeNum != pEl->ElTypeNumber ||
      strcmp (pSS->SsName, pEl->ElStructSchema->SsName))
    /* on n'est pas arrive' encore au type de l'element pEl */
    {
      pSRule = pSS->SsRule->SrElem[typeNum - 1];
      doit = TRUE;
      if (pSRule->SrRecursive)
        {
          /* regle recursive */
          if (pSRule->SrRecursDone)
            /* elle a deja ete rencontree, on ne fait rien */
            doit = FALSE;
          else
            /* elle n'a pas encore ete rencontree, on la traite */
            pSRule->SrRecursDone = TRUE;
        }
      if (doit)
        /* traitement selon le constructeur du type */
        switch (pSRule->SrConstruct)
          {
          case CsIdentity:
            /* CsIdentity', on continue */
            found = SearchChoiceRules (pSS, pSRule->SrIdentRule,
                                       pEl, (int *) Anchor);
            break;
          case CsList:
            /* CsList, on continue */
            found = SearchChoiceRules (pSS, pSRule->SrListItem, pEl,
                                       (int *) Anchor);
            break;
          case CsChoice:
            if (pSRule->SrNChoices == 0)
              /* SRule UNIT */
              {
                if (pEl->ElStructSchema->SsRule->SrElem[pEl->ElTypeNumber - 1]->SrUnitElem)
                  /* l'element est une unite', on a trouve' la regle */
                  found = TRUE;
              }
            else if (pSRule->SrNChoices > 0)
              /* SRule CsChoice avec options explicites */
              {
                /* on verifie d'abord si le type de l'element est une */
                /* des options de ce choix */
                if (!strcmp (pSS->SsName, pEl->ElStructSchema->SsName))
                  for (choice = 0; choice < pSRule->SrNChoices &&
                         !found; choice++)
                    if (pSRule->SrChoice[choice] == pEl->ElTypeNumber)
                      /* c'est effectivement une des options */
                      found = TRUE;
                if (!found)
                  /* on n'a pas trouve'. On cherche a partir des
                     regles qui definissent les options du choix */
                  for (choice = 0; choice < pSRule->SrNChoices &&
                         !found; choice++)
                    found = SearchChoiceRules (pSS,
                                               pSRule->SrChoice[choice],
						  
                                               pEl, (int *) Anchor);
              }
            if (found)
              /* cette regle CsChoice mene au type de pEl */
              /* on l'enregistre dans la liste des regles Choix */
              /* traversees */
              {
                pChoiceD = (PtrChoiceOptionDescr) TtaGetMemory (sizeof (ChoiceOptionDescr));
                if (pChoiceD != NULL)
                  {
                    pChoiceD->COtypeNum = typeNum;
                    pChoiceD->COStructSch = pSS;
                    pChoiceD->CONext = *Anchor;
                    *Anchor = pChoiceD;
                  }
              }
            break;
          default:
            break;
          }
      if (pSRule->SrRecursive && pSRule->SrRecursDone && doit)
        pSRule->SrRecursDone = FALSE;
    }
  param = (int *) Anchor;
  return found;
}


/*----------------------------------------------------------------------
  BuildChangeTypeTable						
  ----------------------------------------------------------------------*/
static void BuildChangeTypeTable (PtrElement pEl)
{
  PtrIsomorphDesc      pIsoD, pNextIsoD;
  PtrChoiceOptionDescr pChoicesFound, pChoiceD, pOldChoiceD;
  PtrSRule             pSRule;
  PtrSSchema           pSSasc;
  PtrElement           pAncest, pPrev;
  int                  choice, typeNum;
  STRING	        strResDyn;

  NChangeTypeItems = 0;	/* la table est vide pour l'instant */
  if (pEl != NULL)
    if (pEl->ElParent != NULL)
      /* on cherche les regles CsChoice qui permettent de passer du pere */
      /* de l'element pEl a l'element pEl lui-meme */
      {
        pChoicesFound = NULL;
        if (SearchChoiceRules (pEl->ElParent->ElStructSchema,
                               pEl->ElParent->ElTypeNumber,
                               pEl, (int *) &pChoicesFound))
          /* on a trouve' au moins une regle CsChoice dont pEl est issu */
          {
            /* on vide la liste des descripteurs de types isomorphes */
            pIsoD = firstIsomorphDesc;
            while (pIsoD != NULL)
              {
                pNextIsoD = pIsoD->IDNext;
                TtaFreeMemory ( pIsoD);
                pIsoD = pNextIsoD;
              }
            firstIsomorphDesc = NULL;
            strResDyn = NULL;
            if (strResDyn!= NULL)
              /* on initialise la transformation automatique */
              RestInitMatch ((Element)pEl, (Element)pEl);

            /* on commence par remplir la table des types a proposer dans
               le menu de changement de types */
            /* on parcourt la liste des regles choix trouvees */
            pChoiceD = pChoicesFound;
            while (pChoiceD != NULL)
              {
                /* la regle CsChoice courante */
                pSRule = pChoiceD->COStructSch->SsRule->SrElem[pChoiceD->COtypeNum - 1];
                if (pSRule->SrNChoices == 0)
                  /* c'est une regle UNIT */
                  {
                    /* cherche toutes les unites definies dans les schemas de */
                    /* structure des elements ascendants */
                    pAncest = pEl;
                    pPrev = NULL;
                    /* examine tous les elements ascendants tant que la table */
                    /* n'est pas pleine */
                    while (pAncest != NULL &&
                           NChangeTypeItems < MAX_ITEMS_CHANGE_TYPE - 1)
                      {
                        if (pPrev == NULL ||
                            pAncest->ElStructSchema != pPrev->ElStructSchema)
                          /* cet element englobant appartient a un
                             schema different de l'element traite'
                             precedemment. On examine les unites
                             definies dans ce schema */
                          {
                            pSSasc = pAncest->ElStructSchema;
                            /* si c'est une extension de schema, on
                               revient au schema de base pour examiner
                               toutes les extensions */
                            while (pSSasc->SsPrevExtens != NULL)
                              pSSasc = pSSasc->SsPrevExtens;
                            /* cherche les unites definies dans ce
                               schema et dans ses extensions */
                            do
                              {
                                /* boucle sur toutes les regles du schema */
                                for (typeNum = 1; typeNum <= pSSasc->SsNRules &&
                                       NChangeTypeItems < MAX_ITEMS_CHANGE_TYPE - 1; typeNum++)
                                  if (pSSasc->SsRule->SrElem[typeNum - 1]->SrUnitElem)
                                    /* cette regle definit une unite'. On essaie de la */
                                    /* mettre dans la table associee au menu */
                                    RegisterIfIsomorphic (pEl, pSSasc, typeNum);
                                /* passe a l'extension de schema suivante */
                                pSSasc = pSSasc->SsNextExtens;
                              }
                            while (pSSasc != NULL);
                          }
                        /* passe a l'element ascendant du dessus */
                        pPrev = pAncest;
                        pAncest = pAncest->ElParent;
                      }
                  }
                else
                  /* c'est un choix a options explicites */
                  /* on essaie de mettre dans la table associee au menu
                     toutes les options du choix */
                  for (choice = 0; choice < pSRule->SrNChoices &&
                         NChangeTypeItems < MAX_ITEMS_CHANGE_TYPE - 1;
                       choice++)
                    RegisterIfIsomorphic (pEl, pChoiceD->COStructSch,
                                          pSRule->SrChoice[choice]);
                /* passe a la regle CsChoice suivante */
                pOldChoiceD = pChoiceD;
                pChoiceD = pChoiceD->CONext;
                TtaFreeMemory ( pOldChoiceD);
              }
          }
      }
}

/*----------------------------------------------------------------------
  ChangeTypeOfElements						
  ----------------------------------------------------------------------*/
static ThotBool ChangeTypeOfElements (PtrElement firstEl, PtrElement lastEl,
                                      PtrDocument pDoc, int newTypeNum,
                                      PtrSSchema newSSchema)
{
  PtrElement          pEl;
  Element             El;
  ElementType         elType;
  int                 ent, method = 0;
  ThotBool            ok;
  ThotBool            done = FALSE;

  /* Don't do anything if it's not a sequence of sibling elements */
  if (firstEl->ElParent == lastEl->ElParent)
    /* on ne peut rien faire si le pere est protege' */
    if (!ElementIsReadOnly (firstEl->ElParent))
      {
        /* on essaie de les englober dans un element du type demande' */
        /* verifie si ce type d'elements accepte les elements voulus */
        /* comme fils */
        if (CanSurround (newTypeNum, newSSchema, firstEl, lastEl, pDoc))
          /* verifie si ce type peut etre un fils du pere des elements a */
          /* englober */
          if (AllowedFirstChild (firstEl->ElParent, pDoc, newTypeNum,
                                 newSSchema, FALSE, FALSE))
            done = DoSurround (firstEl, lastEl, 0, 0, pDoc, newTypeNum,
                               newSSchema);
        if (!done)
          /* essaie de changer le type des elements selectionne's */
          {
            if (lastEl == firstEl)
              /* un seul element selectionne', on essaie de changer son type */
              pEl = firstEl;
            else
              /* plusieurs freres selectionn'es */
              if (firstEl->ElPrevious == NULL && lastEl->ElNext == NULL)
                /* tous les freres sont selectionne's. On essaie de changer
                   le type de leur pere */
                pEl = firstEl->ElParent;
              else
                /* tous les freres ne sont pas selectionne's: on ne fait rien */
                pEl = NULL;
            while (pEl != NULL && !done)
              {
                if (ElementIsReadOnly (pEl->ElParent))
                  pEl = NULL;
                else
                  {
                    /* on cree la table des types equivalents a celui de pEl */
                    BuildChangeTypeTable (pEl);
                    /* le type cible est-il dans cette table ? */
                    ok = FALSE;
                    for (ent = 0; !ok && ent < NChangeTypeItems; ent++)
                      {
                        if (newTypeNum == ChangeTypeTypeNum[ent] &&
                            !strcmp (newSSchema->SsName,
                                     ChangeTypeSSchema[ent]->SsName))
                          {
                            method = ChangeTypeMethod[ent];
                            ok = TRUE;
                          }
                      }
					if (!ok && NChangeTypeItems == 0 && pEl->ElParent &&
						pEl->ElParent->ElStructSchema &&
						!strcmp (pEl->ElParent->ElStructSchema->SsName, "Template") /*&&
						!strcmp (newSSchema->SsName,
                                     pEl->ElStructSchema->SsName)*/)
					{
						// Perhaps the right test is the CsAny constructor
						ok = TRUE;
						if (IsomorphicTypes (pEl->ElStructSchema, pEl->ElTypeNumber,
                                 newSSchema, newTypeNum))
						  method = M_EQUIV;
					}
                    if (ok)
                      /* le type est dans la table, on effectue le
                         changement */
                      {
                        /* store the command in the history */
                        AddEditOpInHistory (pEl, pDoc, TRUE, TRUE);
                        done = FALSE;
                        switch (method)
                          {
                          case M_EQUIV :
                            done = DoChangeType (pEl, pDoc, newTypeNum,
                                                 newSSchema);
                            break;
                          case M_RESDYN :
                            done = RestChangeType((Element)pEl, IdentDocument (pDoc),
                                                  newTypeNum, (SSchema)newSSchema);
                            break;
                          }
                        if (!done)
                          CancelLastEditFromHistory (pDoc);
                      }
                  }
                if (!done && pEl)
                  {
                    /* on essaie de changer le type du pere si on est sur
                       un fils unique */
                    if (pEl && pEl->ElPrevious == NULL && pEl->ElNext == NULL)
                      pEl = pEl->ElParent;
                    else
                      pEl = NULL;
                  }
              }
          }
        if (!done)
          /* si l'element est vide, essaie de creer un element du tye */
          /* voulu a l'interieur de l'element vide */
          if (lastEl == firstEl && !firstEl->ElTerminal &&
              firstEl->ElFirstChild == NULL)
            {
              AddEditOpInHistory (firstEl, pDoc, TRUE, TRUE);
              elType.ElSSchema = (SSchema) newSSchema;
              elType.ElTypeNum = newTypeNum;
              El = TtaCreateDescentWithContent (IdentDocument (pDoc),
                                                (Element) firstEl, elType);
              if (El != NULL)
                /* on a pu creer l'element du type voulu */
                done = TRUE;
              else
                CancelLastEditFromHistory (pDoc);
            }
        if (!done)
          /* on essaie d'englober les elements selectionne's dans un element */
          /* du type demande', eventuellement en eclatant les englobants */
          /* verifie si ce type d'elements accepte les elements voulus */
          /* comme fils */
          if (CanSurround (newTypeNum, newSSchema, firstEl, lastEl, pDoc))
            done = DoSurround (firstEl, lastEl, 0, 0, pDoc, newTypeNum,
                               newSSchema);
      }
  return done;
}

/*----------------------------------------------------------------------
  CanInsertBySplitting
  ----------------------------------------------------------------------*/
static ThotBool CanInsertBySplitting (PtrElement *pEl, int charSplit,
                                      ThotBool *splitElem, PtrElement *pSplitEl,
                                      PtrElement *pElSplit, ThotBool createAfter,
                                      int typeNum, PtrSSchema pSS, PtrDocument pDoc)
{
  PtrElement	pElem, Sibling, pList, pF, pSplit;
  ThotBool	ok;

  Sibling = NULL;
  *splitElem = FALSE;
  *pSplitEl = NULL;
  *pElSplit = NULL;
  ok = FALSE;
  pElem = *pEl;
  do
    {
      /* isolate pElem from its sibling. If pElem is a component */
      /* of an aggregate, AllowedSibling would always say no otherwise */
      if (*splitElem)
        {
          if (createAfter)
            {
              Sibling = pElem->ElNext;
              pElem->ElNext = NULL;
            }
          else
            {
              Sibling = pElem->ElPrevious;
              pElem->ElPrevious = NULL;
            }
        }
      ok = AllowedSibling (pElem, pDoc, typeNum, pSS, (ThotBool)(!createAfter), TRUE,FALSE);
      /* restore link with sibling */
      if (*splitElem)
        {
          if (createAfter)
            pElem->ElNext = Sibling;
          else
            pElem->ElPrevious = Sibling;
        }

      if (ok)
        *pEl = pElem;
      else
        {
          if ((pElem->ElTerminal && pElem->ElLeafType == LtText && charSplit > 0)
              || *splitElem ||
              (createAfter && pElem->ElNext) ||
              (!createAfter && pElem->ElPrevious))
            {
              if (*splitElem && pElem != *pSplitEl)
                {
                  if (pElem == *pSplitEl)
                    pElem = NULL;
                }
              else
                {
                  if (createAfter && charSplit == 0 && !*splitElem)
                    pSplit = pElem->ElNext;
                  else
                    pSplit = pElem;
                  if (CanSplitElement (pSplit, charSplit, FALSE, &pList, &pF,
                                       pSplitEl))
                    {
                      *splitElem = TRUE;
                      if (*pElSplit == NULL)
                        *pElSplit = pSplit;
                    }
                  else
                    pElem = NULL;
                }
            }
          if (pElem != NULL)
            pElem = pElem->ElParent;
        }
    }
  while (pElem != NULL && !ok);
  return ok;
}

/*----------------------------------------------------------------------
  CreateNewElement
  The user wants to create a new given typed element (typeNum, pSS), for
  the pDoc document, near the current selection.
  before is set TRUE when the new element should be inserted before,
  FALSE if it should replace the selection.
  ----------------------------------------------------------------------*/
ThotBool CreateNewElement (int typeNum, PtrSSchema pSS, PtrDocument pDoc,
                       ThotBool before)
{
  PtrElement          firstSel, lastSel, pNew, pF, pSibling, pEl, pSecond;
  PtrElement          pElem, pElSplit, pSplitEl, pNextEl, pParent;
  ElementType	        elType;
  PtrDocument         pSelDoc;
  Document            doc = IdentDocument (pDoc);
  NotifyElement       notifyEl;
  int                 firstChar, lastChar, origFirstChar, origLastChar,
    NSiblings, ancestorRule, rule, nComp, nAncest, i;
#define MAX_ANCESTORS_CREATE 10
  int                 ancest[MAX_ANCESTORS_CREATE];
  ThotBool            InsertionPoint, ok, createAfter, splitElem, elConst;
  ThotBool            empty, selHead, selTail, done, deleted, histOpen;

  nAncest = 0;
  NSiblings = 0;
  if (!GetCurrentSelection (&pSelDoc, &firstSel, &lastSel, &firstChar,
                            &lastChar))
    {
      if (GetDeferredSelection (doc, &firstSel, &lastSel, &firstChar,
                                &lastChar))
        pSelDoc = pDoc;
    }
  if (pSelDoc != pDoc)
    /* the document asking for the creation of a new element is NOT the */
    /* document containing the current selection */
    return FALSE;
  else if (pSelDoc->DocReadOnly)
    return FALSE;
  else if ((TypeHasException (ExcIsRow, firstSel->ElTypeNumber,
                              firstSel->ElStructSchema) ||
            TypeHasException (ExcIsColHead, firstSel->ElTypeNumber,
                              firstSel->ElStructSchema) ||
            TypeHasException (ExcIsCell, firstSel->ElTypeNumber,
                              firstSel->ElStructSchema)) &&
           (pSS != firstSel->ElStructSchema ||
            typeNum != firstSel->ElTypeNumber))
    // don't create another kind of element within a table
    return FALSE;
  else
    /* there is a selection and the document can be modified */
    {
      elConst = FALSE;
      empty = FALSE;
      ok = FALSE;
      InsertionPoint = (firstSel == lastSel  &&
                        firstSel->ElTerminal &&
                        ((firstSel->ElLeafType == LtText && firstChar > 0 &&
                          SelPosition)                      ||
                         (firstSel->ElLeafType == LtPicture && SelPosition)  ||
                         firstSel->ElLeafType == LtGraphics ||
                         firstSel->ElLeafType == LtPolyLine ||
                         firstSel->ElLeafType == LtPath     ||
                         firstSel->ElLeafType == LtSymbol     ));
      /* Peut-on considerer la selection courante comme un simple point */
      /* d'insertion ? */
      if (!InsertionPoint && firstSel == lastSel &&
          /* un seul element selectionne' */
          GetElementConstruct (firstSel, &nComp) == CsConstant)
        /* c'est une constante, on va creer le nouvel element devant */
        {
          InsertionPoint = TRUE;
          elConst = TRUE;
        }
      if (!InsertionPoint && firstSel == lastSel &&
          EmptyElement (firstSel))
        /* c'est un element vide unique */
        {
          InsertionPoint = TRUE;
          empty = TRUE;
        }

      if (!InsertionPoint)
        /* il n'y a pas un simple point d'insertion, mais du texte et/ou */
        /* un ou des elements selectionne's */
        {
          /* Coupe les elements du debut et de la fin de la selection s'ils */
          /* sont partiellement selectionnes */
          TtaClearViewSelections ();
          IsolateSelection (pSelDoc, &firstSel, &lastSel, &firstChar,
                            &lastChar, FALSE);
          AbstractImageUpdated (pSelDoc);
          RedisplayDocViews (pSelDoc);
          if (lastSel->ElTerminal && lastSel->ElLeafType == LtText &&
              lastChar >= lastSel->ElTextLength)
            {
              lastChar = 0;
              if (firstSel == lastSel && firstChar == 1)
                firstChar = 0; 
            }
          if (before ||  (pSS && !strcmp (pSS->SsName, "Template")))
            /* don't try to surround or transform template elements */
            InsertionPoint = TRUE;
          else if (firstChar <= 1 && lastChar == 0)
            /* first and last selected elements are complete */
            {
              /* on essaie d'abord de transformer l'ensemble des elements
                 selectionne's en un element du type demande' */
              /* remonte la selection courante sur une suite de freres si
                 c'est equivalent */
              if (firstSel->ElParent != lastSel->ElParent)
                {
                  /* essaie de ramener la selection a une suite de freres */
                  firstChar = 0;
                  lastChar = 0;
                  SelectSiblings (&firstSel, &lastSel, &firstChar, &lastChar);
                }

              if (firstSel->ElParent != lastSel->ElParent)
                ok = FALSE;
              else
                {
                  histOpen = TtaHasUndoSequence (doc);
                  if (!histOpen)
                    OpenHistorySequence (pSelDoc, firstSel, lastSel, NULL,
                                         firstChar, lastChar);
                  ok = ChangeTypeOfElements (firstSel, lastSel, pSelDoc,
                                             typeNum, pSS);
                  if (!histOpen)
                    CloseHistorySequence (pSelDoc);
                }
              if (!ok && typeNum > 7 /* should be a constant */)
                /* ca n'a pas marche'. essaie les transformations de */
                /* type par patterns */
                {
                  elType.ElTypeNum = typeNum;
                  elType.ElSSchema = (SSchema) pSS;
                  
                  if (TransformIntoFunction != NULL)
                    ok = (*(Func2)TransformIntoFunction) (
                                        (void*)&elType,
                                        /* SG : the @ should be passed in order to be c++ compliant */
                                         (void*)((Document) IdentDocument (pSelDoc)));
                }
              /* si ca n'a pas marche' et si plusieurs elements sont
                 selectionne's, on essaie de transformer chaque element
                 selectionne' en un element du type demande' */
              if (!ok)
                if (firstSel != lastSel)
                  {
                    /* store the command in the history */
                  histOpen = TtaHasUndoSequence (doc);
                  if (!histOpen)
                    OpenHistorySequence (pSelDoc, firstSel, lastSel, NULL,
                                         firstChar, lastChar);
                    pEl = firstSel;
                    do
                      {
                        /* essaie de transformer un element */
                        ok = ChangeTypeOfElements (pEl, pEl, pSelDoc, typeNum,
                                                   pSS);
                        if (!ok)
                          CancelLastEditFromHistory (pSelDoc);
                        /* cherche l'element suivant de la selection */
                        pEl = NextInSelection (pEl, lastSel);
                      }
                    while (pEl != NULL);
                    if (!histOpen)
                      CloseHistorySequence (pSelDoc);
                  }
            }
          else
            /* on ne fait rien */
            return FALSE;
        }

      if (InsertionPoint)
        /* il y a un simple point d'insertion */
        {
          origFirstChar = firstChar;
          origLastChar = lastChar;
          if (origFirstChar > 0 && origFirstChar == origLastChar)
            origLastChar--;
          /* verifie si l'element a creer porte l'exception NoCreate */
          if (TypeHasException (ExcNoCreate, typeNum, pSS))
            /* abandon */
            return FALSE;
	  
          if (elConst || empty)
            {
              selHead = TRUE;
              selTail = FALSE;
            }
          else
            {
              /* la selection commence-t-elle en tete d'un element ? */
              selHead = (firstSel == lastSel &&
                         firstSel->ElPrevious == NULL &&
                         lastSel->ElTerminal &&
                         ((lastSel->ElLeafType == LtText && firstChar <= 1) ||
                          (lastSel->ElLeafType == LtPicture && firstChar == 0) ||
                          lastSel->ElLeafType == LtGraphics ||
                          lastSel->ElLeafType == LtPolyLine ||
                          lastSel->ElLeafType == LtPath     ||
                          lastSel->ElLeafType == LtSymbol ));
              /* la selection est-t-elle a la fin de la derniere feuille
                 de texte d'un element */
              selTail = (firstSel == lastSel &&
                         lastSel->ElNext == NULL &&
                         lastSel->ElTerminal &&
                         ((lastSel->ElLeafType == LtText &&
                           firstChar > lastSel->ElTextLength) ||
                          (lastSel->ElLeafType == LtPicture &&
                           firstChar > 0) ||
                          lastSel->ElLeafType == LtGraphics ||
                          lastSel->ElLeafType == LtPolyLine ||
                          lastSel->ElLeafType == LtPath     ||
                          lastSel->ElLeafType == LtSymbol ));
            }
	  
          /* verifie si la selection est en fin ou debut de paragraphe */
          if (selHead)
            {
              pEl = firstSel;
              firstChar = 0;
              createAfter = FALSE;
            }
          else if (selTail)
            {
              pEl = lastSel;
              firstChar = 0;
              createAfter = TRUE;
            }
          else
            {
              pEl = firstSel;
              createAfter = !before;
              if (lastSel->ElTerminal &&
                  (lastSel->ElLeafType == LtText ||
                   lastSel->ElLeafType == LtPicture))
                  {
                    if (lastChar == 0)
                      // beginning of the element
                      createAfter = FALSE;
                    else
                      // end of the element
                      createAfter = TRUE;
                  }
            }
          /* on verifie si on peut couper un element ascendant en deux et
             creer le nouvel element entre les deux parties obtenues */
          ok = CanInsertBySplitting (&pEl, firstChar, &splitElem,
                                     &pSplitEl, &pElSplit, createAfter,
                                     typeNum, pSS, pSelDoc);
          if (ok && empty && !EmptyElement (pEl))
            empty = FALSE;
          if (pEl->ElTypeNumber == 1 && pEl->ElParent && (selHead || selTail))
            // check the upper level
            pParent = pEl->ElParent->ElParent;
          else if (pEl->ElStructSchema &&
                   !strcmp (pEl->ElStructSchema->SsName, "Template"))
            pParent = NULL;
          else
            pParent = pEl->ElParent;
          if (!ok && pParent && pParent->ElStructSchema &&
              !strcmp (pParent->ElStructSchema->SsName, "Template"))
            {
              // let amaya check the validity
              if (pParent = pEl->ElParent->ElParent)
                pEl = pEl->ElParent;
              ok = TRUE;
            }
          ancestorRule = 0;
          if (!ok)
            /* si l'element a creer apparait dans le schema de structure
               comme un element de liste ou d'agregat, on essaie de creer
               une telle liste ou un tel agregat */
            {
              rule = typeNum;
              nAncest = 0;
              while (rule > 0 && !ok)
                {
                  /* sauf au premier tour, on cherche d'abord une regle
                     d'identite' */
                  if (nAncest > 0)
                    ancestorRule = IdentRuleOfElem (rule, pSS);
                  /* si on n'a pas trouve', on cherche une regle List */
                  if (ancestorRule == 0)
                    ancestorRule = ListRuleOfElem (rule, pSS);
                  /* si on n'a pas trouve', on cherche une regle Aggregate */
                  if (ancestorRule == 0)
                    ancestorRule = AggregateRuleOfElem (rule, pSS);
                  if (ancestorRule > 0 && ancestorRule == pSS->SsRootElem)
                    /* don't create a root element */
                    ancestorRule = 0;
                  if (ancestorRule == 0)
                    rule = 0;
                  else
                    {
                      ok = CanInsertBySplitting (&pEl, firstChar, &splitElem,
                                                 &pSplitEl, &pElSplit, createAfter,
                                                 ancestorRule, pSS, pSelDoc);
                      if (!ok)
                        {
                          /* is this rule already in the stack? */
                          for (i = 0; i < nAncest && rule; i++)
                            if (ancest[i] == ancestorRule)
                              rule = 0;   /* yes, stop to avoid recursion */
                          if (rule)
                            {
                              if (nAncest >= MAX_ANCESTORS_CREATE - 1)
                                /* stack overflow. Stop */
                                rule = 0;
                              else
                                /* put that rule on the stack */
                                {
                                  ancest[nAncest++] = ancestorRule;
                                  rule = ancestorRule;
                                }
                            }
                        }
                    }
                }
            }
	  
          if (ok && pEl)
            {
              /* demande a l'application si on peut creer ce type d'element */
              notifyEl.event = TteElemNew;
              notifyEl.document = doc;
              notifyEl.element = (Element) (pEl->ElParent);
              notifyEl.info = 0; /* not sent by undo */
              notifyEl.elementType.ElTypeNum = typeNum;
              notifyEl.elementType.ElSSchema = (SSchema) pSS;
              pF = pEl;
              NSiblings = 0;
              while (pF->ElPrevious != NULL)
                {
                  NSiblings++;
                  pF = pF->ElPrevious;
                }
              if (createAfter)
                NSiblings++;
              notifyEl.position = NSiblings;
              if (CallEventType ((NotifyEvent *) (&notifyEl), TRUE))
                /* l'application refuse */
                pEl = NULL;
            }

          pNew = NULL;
          if (ok && pEl)
            {
              /* After element */
              histOpen = TtaHasUndoSequence (doc);
              if (!histOpen)
                OpenHistorySequence (pSelDoc, firstSel, lastSel, NULL,
                                     origFirstChar, origLastChar);

              ok = !CannotInsertNearElement (pEl, FALSE);
              if (ok)
                {
                  done = FALSE;
                  if (splitElem)
                    /* coupe l'element en deux */
                    {
                      ok = BreakElement (pSplitEl, pElSplit, firstChar, FALSE,
                                         FALSE);
                      if (ok)
                        {
                          createAfter = TRUE;
                          if (ancestorRule > 0)
                            {
                              pElem = pSplitEl;
                              do
                                {
                                  ok = AllowedSibling (pElem, pSelDoc, typeNum,
                                                       pSS, (ThotBool)(!createAfter),
                                                       TRUE, FALSE);
                                  if (ok)
                                    {
                                      pEl = pElem;
                                      done = TRUE;
                                    }
                                  else if (pElem->ElTerminal)
                                    pElem = NULL;
                                  else
                                    {
                                      pElem = pElem->ElFirstChild;
                                      while (pElem && pElem->ElNext != NULL)
                                        pElem = pElem->ElNext;
                                    }
                                }
                              while (!ok && pElem != NULL);
                            }
                        }
                    }
                  if (!done)
                    if (ancestorRule > 0)
                      /* intermediate elements have to be created */
                      {
                        /* first, create the high-level element */
                        pNew = NewSubtree (ancestorRule, pSS, pSelDoc,
                                           FALSE, TRUE, TRUE, TRUE);
                        /* create intermediate elements following the stack */
                        pParent = pNew;
                        elType.ElSSchema = (SSchema) pSS;
                        for (i = nAncest; i > 0 && pParent; i--)
                          {
                            elType.ElTypeNum = ancest[i - 1];
                            TtaCreateDescent (IdentDocument (pSelDoc),
                                              (Element) pParent, elType);
                            pParent = SearchTypedElementInSubtree (pParent,
                                                                   ancest[i - 1], pSS);
                          }
                        if (pParent)
                          /* create the low-level element (the element of the
                             desired type) */
                          {
                            elType.ElTypeNum = typeNum;
                            TtaCreateDescentWithContent (IdentDocument(pSelDoc),
                                                         (Element) pParent, elType);
                          }
                        ok = TRUE;
                      }
                }
              if (ok)
                {
                  /* annule la selection */
                  TtaClearViewSelections ();
                  if (!splitElem)
                    if (firstSel->ElTerminal &&
                        firstSel->ElLeafType == LtText &&
                        firstChar > 1 &&
                        firstChar <= firstSel->ElTextLength && !splitElem)
                      {
                        /* store the editing operation in the history */
                        AddEditOpInHistory (firstSel, pSelDoc, TRUE, FALSE);
                        pNextEl = firstSel->ElNext;
                        SplitTextElement (firstSel, firstChar, pSelDoc, TRUE,
                                          &pSecond, FALSE);
                        AddEditOpInHistory (firstSel, pSelDoc, FALSE, TRUE);
                        AddEditOpInHistory (pSecond, pSelDoc, FALSE, TRUE);
                        BuildAbsBoxSpliText (firstSel, pSecond, pNextEl,
                                             pSelDoc);
                      }
                  if (pNew == NULL)
                    pNew = NewSubtree (typeNum, pSS, pSelDoc, TRUE, TRUE,
                                       TRUE, TRUE);
		  
                  /* Insertion du nouvel element */
                  if (createAfter)
                    {
                      pSibling = SiblingElement (pEl, FALSE);
                      InsertElementAfter (pEl, pNew);
                      AddEditOpInHistory (pNew, pSelDoc, FALSE, TRUE);
                      if (pSibling == NULL)
                        /* l'element pEl n'est plus le dernier fils de
                           son pere */
                        ChangeFirstLast (pEl, pSelDoc, FALSE, TRUE);
                    }
                  else
                    {
                      pSibling = SiblingElement (pEl, TRUE);
                      InsertElementBefore (pEl, pNew);
                      if (empty)
                        {
                          /* on doit detruire l'element vide devant lequel on
                             vient de creer un nouvel element */
                          /* verifie si l'element a detruire porte l'exception
                             NoCut */
                          if (TypeHasException (ExcNoCut, pEl->ElTypeNumber,
                                                pEl->ElStructSchema) ||
                              TypeHasException (ExcEmptyGraphic, pEl->ElTypeNumber,
                                                pEl->ElStructSchema))
                            empty = FALSE;
                          else
                            /* envoie l'evenement ElemDelete.Pre a
                               l'application */
                            if (SendEventSubTree (TteElemDelete, pSelDoc, pEl,
                                                  TTE_STANDARD_DELETE_LAST_ITEM,
                                                  0, FALSE, FALSE))
                              /* l'application refuse de detruire cet element */
                              empty = FALSE;
                        }
                      /* register the new element in the editing history */
                      AddEditOpInHistory (pNew, pSelDoc, FALSE, TRUE);       
                      deleted = FALSE;
                      if (empty)
                        /* on detruit l'element vide devant lequel on
                           vient de creer un nouvel element */
                        {
                          /* register the empty element that will be deleted*/
                          AddEditOpInHistory (pEl, pSelDoc, TRUE, FALSE);
                          /* detruit les paves de l'element vide a detruire */
                          DestroyAbsBoxes (pEl, pSelDoc, TRUE);
                          AbstractImageUpdated (pSelDoc);
                          /* prepare l'evenement ElemDelete.Post */
                          notifyEl.event = TteElemDelete;
                          notifyEl.document = (Document) IdentDocument (pSelDoc);
                          notifyEl.element = (Element) (pEl->ElParent);
                          notifyEl.info = 0; /* not sent by undo */
                          notifyEl.elementType.ElTypeNum = pEl->ElTypeNumber;
                          notifyEl.elementType.ElSSchema =
                            (SSchema) (pEl->ElStructSchema);
                          notifyEl.position = NSiblings + 1;
                          pNextEl = NextElement (pEl);
                          /* retire l'element de l'arbre abstrait */
                          RemoveElement (pEl);
                          UpdateNumbers (pNextEl, pEl, pSelDoc, TRUE);
                          RedisplayCopies (pEl, pSelDoc, TRUE);
                          DeleteElement (&pEl, pSelDoc);
                          /* envoie l'evenement ElemDelete.Post a l'application*/
                          CallEventType ((NotifyEvent *) (&notifyEl), FALSE);
                          deleted = TRUE;
                        }
                      if (!deleted && pSibling == NULL)
                        /* l'element pEl n'est plus le premier fils de
                           son pere */
                        ChangeFirstLast (pEl, pSelDoc, TRUE, TRUE);
                    }
                  if (pNew && pNew->ElStructSchema)
                    {
                      /* traite les exclusions des elements crees */
                      RemoveExcludedElem (&pNew, pSelDoc);
                      /* traite les attributs requis des elements crees */
                      if (pDoc->DocCheckingMode & COMPLETE_CHECK_MASK)
                        /*if (FullStructureChecking)*/
                        AttachMandatoryAttributes (pNew, pSelDoc);
                      if (pSelDoc->DocSSchema != NULL)
                        /* le document n'a pas ete ferme' entre temps */
                        {
                          /* traitement des exceptions */
                          CreationExceptions (pNew, pSelDoc);
                          pParent = pNew->ElParent;
                          pSibling = pNew->ElPrevious;
                          /* cree les paves du nouvel element et */
                          /* send an event TteElemNew  Post to application */
                          NotifySubTree (TteElemNew, pSelDoc, pNew, 0, 0,
                                         FALSE, FALSE);
                          // detect if the image is replaced by an external doc
                          if (typeNum == LtPicture + 1 ||
                              TypeHasException (ExcIsImg, typeNum, pSS))
                            {
                              if (pSibling)
                                pF = pSibling->ElNext;
                              else if (pParent)
                                pF = pParent->ElFirstChild;
                              else
                                pF = pNew;
                              if (pF != pNew)
                                {
                                  if (pNew == GetLastCreatedElemInHistory (pSelDoc))
                                    {
                                    CancelLastEditFromHistory (pSelDoc);
                                    AddEditOpInHistory (pF, pSelDoc, FALSE, TRUE);
                                    }
                                  pNew = pF;
                                }
                            }

                          if (pNew && pNew->ElParent)
                            {
                              /* the element still exists */
                              CreateAllAbsBoxesOfEl (pNew, pSelDoc);
                              /* Mise a jour des images abstraites */
                              AbstractImageUpdated (pSelDoc);
                              /* indique au Mediateur les modifications */
                              RedisplayDocViews (pSelDoc);
                              /* si on est dans un element copie' par inclusion,
                                 on met a jour les copies de cet element. */
                              RedisplayCopies (pNew, pSelDoc, TRUE);
                              UpdateNumbers (NextElement (pNew), pNew, pSelDoc,
                                             TRUE);
                              /* Indiquer que le document est modifie' */
                              SetDocumentModified (pSelDoc, TRUE, 30);
                              /* Replace la selection */
                              pEl = SearchTypedElementInSubtree (pNew, typeNum,
                                                                 pSS);
                              if (!pEl)
                                if (pSS->SsRule->SrElem[typeNum - 1]->SrConstruct == CsNatureSchema)
                                  if (pNew)
                                    pEl = pNew;
                              if (pEl)
                                {
                                  if (pEl->ElTerminal &&
                                      pEl->ElLeafType == LtPicture)
                                    /* we have created a picture element */
                                    /* select its end (right edge) */
                                    SelectElementWithEvent (pSelDoc, pEl,
                                                            FALSE, TRUE);
                                  else
                                    /* select the beginning of the first leaf
                                       of the created element */
                                    SelectElementWithEvent (pSelDoc,
                                                            FirstLeaf (pEl), TRUE, TRUE);
                                }
                            }
                        }
                    }
                }
              if (!histOpen)
                CloseHistorySequence (pSelDoc);
            }
        }
    }
  return ok;
}

/* ----------------------------------------------------------------------
   TtaCreateElement

   Create an element of a given type and insert it at the current position
   within a given document. The current position is defined by the current
   selection.
   If the current position is a single position (insertion point) the new
   element is simply inserted at that position. If one or several characters
   and/or elements are selected, the new element is created at that position
   and the selected characters/elements become the content of the new element,
   provided the  structure schema allows it.
   Parameters:
   elementType: type of the element to be created.
   document: the document for which the element is created.
   Return TRUE if the action is done
   ---------------------------------------------------------------------- */
ThotBool TtaCreateElement (ElementType elementType, Document document)
{
  ThotBool done = FALSE;

  UserErrorCode = 0;
  if (elementType.ElSSchema == NULL)
    TtaError (ERR_invalid_parameter);
  else if (document < 1 || document > MAX_DOCUMENTS)
    /* Checks the parameter document */
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument[document - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  else if (elementType.ElTypeNum < 1 ||
           elementType.ElTypeNum > ((PtrSSchema) (elementType.ElSSchema))->SsNRules)
    /* Parameter document is ok */
    TtaError (ERR_invalid_element_type);
  else
    done = CreateNewElement (elementType.ElTypeNum,
                             (PtrSSchema) (elementType.ElSSchema),
                             LoadedDocument[document - 1], FALSE);
  return done;
}


/* ----------------------------------------------------------------------
   TtaInsertElement

   Create an element of a given type and insert it at the current position within
   a given document. The current position is defined by the current selection.
   If the current position is a single position (insertion point) the new element
   is simply inserted at that position. If one or several characters and/or
   elements are selected, the new element is created before the first selected
   character/element and the selected characters/elements are not changed.
   Parameters:
   elementType: type of the element to be created.
   document: the document for which the element is created.
   Return TRUE if the action is done
   ---------------------------------------------------------------------- */
ThotBool TtaInsertElement (ElementType elementType, Document document)
{
  ThotBool done = FALSE;

  UserErrorCode = 0;
  if (elementType.ElSSchema == NULL)
    TtaError (ERR_invalid_parameter);
  else if (document < 1 || document > MAX_DOCUMENTS)
    /* Checks the parameter document */
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument[document - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  else if (elementType.ElTypeNum < 1 ||
           elementType.ElTypeNum > ((PtrSSchema) (elementType.ElSSchema))->SsNRules)
    /* Parameter document is ok */
    TtaError (ERR_invalid_element_type);
  else
    done = CreateNewElement (elementType.ElTypeNum,
                             (PtrSSchema) (elementType.ElSSchema),
                             LoadedDocument[document - 1], TRUE);
  return done;
}

/* ----------------------------------------------------------------------
   TtaInsertAnyElement

   Create an element whose type is defined in the structure schema and insert
   it at the current position within a given document. The current position is
   defined by the current selection.
   If the current position is a single position (insertion point) the new
   element is simply inserted at that position. If one or several characters
   and/or elements are selected, the new element is created before the first
   selected character/element if before is TRUE, after the last selected
   character/element if before is FALSE.
   The selection is set within the new element.
   Parameters:
   document: the document for which the element is created.
   ---------------------------------------------------------------------- */
void TtaInsertAnyElement (Document document, ThotBool before)
{
  PtrDocument     pDoc, pSelDoc;
  PtrElement      firstSel, lastSel, pNew, pSibling, pSel, pNextEl, pSecond;
  int             firstChar, lastChar, typeNum, nSiblings;
  PtrSSchema      pSS;
  ThotBool        isList, optional, histOpen;;
  NotifyElement   notifyEl;

  ThotBool newPointCreated;

  UserErrorCode = 0;
  if (document < 1 || document > MAX_DOCUMENTS)
    /* Checks the parameter document */
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument[document - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  else
    /* Parameter document is ok */
    {
      pDoc = LoadedDocument[document - 1];
      if (documentDisplayMode[document - 1] == DeferredDisplay &&
          NewDocSelection[document - 1].SDSelActive)
        {
          firstSel = (PtrElement) (NewDocSelection[document - 1].SDElemSel);
          if (firstSel == NULL)
            /* there is no selection */
            return;
          if (NewDocSelection[document - 1].SDElemExt)
            lastSel = (PtrElement) (NewDocSelection[document - 1].SDElemExt);
          else
            lastSel = firstSel;
          firstChar = NewDocSelection[document - 1].SDFirstChar;
          lastChar = NewDocSelection[document - 1].SDLastChar;
          pSelDoc = pDoc;
        }
      else if (!GetCurrentSelection (&pSelDoc, &firstSel, &lastSel, &firstChar,
                                     &lastChar))
        /* there is no selection */
        return;
      else if (pSelDoc != pDoc)
        /* the document asking for the creation of a new element is NOT the */
        /* document containing the current selection */
        return;
      else if (pSelDoc->DocReadOnly)
        /* the document can not be modified */
        return;

      if (firstSel == lastSel && firstSel->ElTerminal &&
          (firstSel->ElLeafType == LtPolyLine || 
           firstSel->ElLeafType == LtPath)
          && firstChar >= 1)
        {
          newPointCreated = TtaInsertPointInCurve (document,
                                                   (Element)firstSel,
                                                   before, &firstChar);
          
          ChangeSelection (TtaGiveActiveFrame(),
                           firstSel->ElAbstractBox[0],
                           firstChar, FALSE,
                           TRUE, FALSE, FALSE);
          SelectedPointInPolyline = firstChar;
          
          /* Update the attribute */
          if (newPointCreated)
            {
              Element el = TtaGetParent((Element)firstSel);
              UpdatePointsOrPathAttribute(document, el, 0, 0, TRUE);
              TtaUpdateMarkers(el, document, TRUE, TRUE);
              TtaSetDocumentModified(document);
            }
          return;
        }

      if (before)
        {
          if (firstSel && firstSel->ElTypeNumber == (CharString + 1))
            firstSel = firstSel->ElParent;
          if (firstSel && ElementIsReadOnly (firstSel->ElParent))
            /* the parent can not be modified */
            return;
          SRuleForSibling (pDoc, firstSel, TRUE, 1, &typeNum, &pSS, &isList,
                           &optional);
        }
      else
        {
          if (lastSel && lastSel->ElTypeNumber == (CharString + 1))
            lastSel = lastSel->ElParent;
          if (lastSel && ElementIsReadOnly (lastSel->ElParent))
            /* the parent can not be modified */
            return;
          SRuleForSibling (pDoc, lastSel, FALSE, 1, &typeNum, &pSS, &isList,
                           &optional);
        }
      if (typeNum == 0 || pSS == NULL)
        /* no sibling allowed */
        return;
      if (TypeHasException (ExcNoCreate, typeNum, pSS))
        /* the user is not allowed to create this type of element */
        return;
      if (ExcludedType (firstSel->ElParent, typeNum, pSS))
        /* the parent element excludes this type of element */
        return;
      /* send event ElemNew.Pre to the application */
      notifyEl.event = TteElemNew;
      notifyEl.document = document;
      notifyEl.element = (Element) (firstSel->ElParent);
      notifyEl.info = 0; /* not sent by undo */
      notifyEl.elementType.ElTypeNum = typeNum;
      notifyEl.elementType.ElSSchema = (SSchema) (pSS);
      nSiblings = 0;
      if (before)
        pSibling = firstSel;
      else
        pSibling = lastSel;
      while (pSibling->ElPrevious != NULL)
        {
          nSiblings++;
          pSibling = pSibling->ElPrevious;
        }
      if (!before)
        nSiblings++;
      notifyEl.position = nSiblings;
      if (CallEventType ((NotifyEvent *) & notifyEl, TRUE))
        /* application does not accept element creation */
        return;

      histOpen = TtaHasUndoSequence (document);
      if (!histOpen)
        OpenHistorySequence (pDoc, firstSel, lastSel, NULL, firstChar,
                             lastChar);
      TtaClearViewSelections ();

      /* if the selection is in a TEXT leaf, split the leaf */
      if (before && firstSel->ElTerminal &&
          firstSel->ElLeafType == LtText && firstChar > 1 &&
          firstChar <= firstSel->ElTextLength + 1)
        {
          if (firstChar == firstSel->ElTextLength + 1)
            /* insertion before the caret, which is at the end of a TEXT
               leaf. Insert after that text leaf */
            {
              before = FALSE;
              lastSel = firstSel;
            }
          else
            {
              /* store the editing operation in the history */
              AddEditOpInHistory (firstSel, pDoc, TRUE, FALSE);
              pNextEl = firstSel->ElNext;
              SplitTextElement (firstSel, firstChar, pDoc, TRUE, &pSecond,
                                FALSE);
              AddEditOpInHistory (firstSel, pDoc, FALSE, TRUE);
              AddEditOpInHistory (pSecond, pDoc, FALSE, TRUE);
              BuildAbsBoxSpliText (firstSel, pSecond, pNextEl, pDoc);
              firstSel = pSecond;
            }
        }
      else if (!before && lastSel->ElTerminal &&
               lastSel->ElLeafType == LtText &&
               lastChar <= lastSel->ElTextLength)
        {
          if (lastChar == 0 && firstChar == 1 && firstSel == lastSel)
            /* insertion after the caret which is at the beginning of a
               TEXT leaf. Insert before that text leaf */
            {
              before = TRUE;
              firstSel = lastSel;
            }
          else if (lastChar == lastSel->ElTextLength &&
                   firstSel == lastSel && firstChar > lastChar)
            /* a caret at the end of a TEXT leaf */
            {
              before = FALSE;
            }
          else if (lastChar <= lastSel->ElTextLength && lastChar > 0)
            {
              if (firstSel == lastSel && lastChar < firstChar)
                /* it's just a caret */
                lastChar = firstChar;
              /* store the editing operation in the history */
              AddEditOpInHistory (lastSel, pDoc, TRUE, FALSE);
              pNextEl = lastSel->ElNext;
              SplitTextElement (lastSel, lastChar, pDoc, TRUE, &pSecond,
                                FALSE);
              AddEditOpInHistory (lastSel, pDoc, FALSE, TRUE);
              AddEditOpInHistory (pSecond, pDoc, FALSE, TRUE);
              BuildAbsBoxSpliText (lastSel, pSecond, pNextEl, pDoc);
            }
        }

      pNew = NewSubtree (typeNum, pSS, pDoc, TRUE, TRUE, TRUE, TRUE);
      if (before)
        {
          pSibling = SiblingElement (firstSel, TRUE);
          InsertElementBefore (firstSel, pNew);
          AddEditOpInHistory (pNew, pDoc, FALSE, TRUE);
          if (pSibling == NULL)
            /* firstSel is no longer the first child of its parent */
            ChangeFirstLast (firstSel, pDoc, TRUE, TRUE);
        }
      else
        {
          pSibling = SiblingElement (lastSel, FALSE);
          InsertElementAfter (lastSel, pNew);
          AddEditOpInHistory (pNew, pDoc, FALSE, TRUE);
          if (pSibling == NULL)
            /* lastSel is no longer the last child of its parent */
            ChangeFirstLast (lastSel, pDoc, FALSE, TRUE);	      
        }
      /* remove exclusions from the created element */
      RemoveExcludedElem (&pNew, pDoc);
      if (pDoc->DocCheckingMode & COMPLETE_CHECK_MASK)
        /*if (FullStructureChecking)*/
        AttachMandatoryAttributes (pNew, pDoc);
      if (pDoc->DocSSchema != NULL)
        /* the document has not been closed while waiting for mandatory
           attributes */
        {
          CreationExceptions (pNew, pDoc);
          /* send an event ElemNew.Post to application */
          NotifySubTree (TteElemNew, pSelDoc, pNew, 0, 0, FALSE, FALSE);
          CreateAllAbsBoxesOfEl (pNew, pDoc);
          AbstractImageUpdated (pDoc);
          RedisplayDocViews (pDoc);
          RedisplayCopies (pNew, pDoc, TRUE);
          UpdateNumbers (NextElement (pNew), pNew, pDoc, TRUE);
          if (pNew && pNew->ElParent)
            {
              SetDocumentModified (pDoc, TRUE, 30);
              /* set a new selection */
              pSel = FirstLeaf (pNew);
              if (pSel->ElTerminal && pSel->ElLeafType == LtText)
                SelectPositionWithEvent (pDoc, pSel, 1, TRUE);
              else
                SelectElementWithEvent (pDoc, pSel, TRUE, TRUE);
            }
        }
      if (!histOpen)
        CloseHistorySequence (pDoc);  
    }
}

/*----------------------------------------------------------------------
  TtaSetTransformCallback permet de connecter une fonction de l'application
  au changement de type d'element
  ----------------------------------------------------------------------*/
void  TtaSetTransformCallback (Func2 callbackFunc)
{
  TransformIntoFunction = callbackFunc;
}
