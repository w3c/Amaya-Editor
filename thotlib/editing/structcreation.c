/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2010
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * This module handles creation commands
 *
 * Authors: V. Quint (INRIA)
 *          S. Bonhomme (INRIA) - Separation between structured and
 *                                unstructured editing modes
 *
 */

#include "thot_gui.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "libmsg.h"
#include "message.h"
#include "typemedia.h"
#include "application.h"
#include "fileaccess.h"
#include "picture.h"
#include "appaction.h"
#include "dialog.h"
#include "appdialogue.h"

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "modif_tv.h"
#include "select_tv.h"
#include "platform_tv.h"
#include "edit_tv.h"
#include "frame_tv.h"
#include "appdialogue_tv.h"
#include "creation_tv.h"

typedef enum
  {
    InsertWithin, InsertBefore, InsertAfter, Separator
  }
MenuItemAction;

/* information about items of INSERT / PASTE / INCLUDE menus */
/* action to be performed for each item */
static MenuItemAction Action[MAX_MENU];

/* action as seen by the user */
static MenuItemAction UserAction[MAX_MENU];

/* element concerned by the action to be performed */
static PtrElement   ElemAction[MAX_MENU];

/* type of element to be created for each menu item */
static int          ElemTypeAction[MAX_MENU];

/* structure schema of elements to be created */
static PtrSSchema   SSchemaAction[MAX_MENU];
static ThotBool     createPasteMenuOK;

#include "absboxes_f.h"
#include "abspictures_f.h"
#include "appli_f.h"
#include "applicationapi_f.h"
#include "attributeapi_f.h"
#include "attributes_f.h"
#include "attrpresent_f.h"
#include "boxselection_f.h"
#include "buildboxes_f.h"
#include "callback_f.h"
#include "changeabsbox_f.h"
#include "config_f.h"
#include "content_f.h"
#include "creationmenu_f.h"
#include "createabsbox_f.h"
#include "createpages_f.h"
#include "documentapi_f.h"
#include "docs_f.h"
#include "displayview_f.h"
#include "editcommands_f.h"
#include "exceptions_f.h"
#include "frame_f.h"
#include "memory_f.h"
#include "paginate_f.h"
#include "presvariables_f.h"
#include "readpivot_f.h"
#include "references_f.h"
#include "schemas_f.h"
#include "structcommands_f.h"
#include "structcreation_f.h"
#include "structmodif_f.h"
#include "structselect_f.h"
#include "structschema_f.h"
#include "tableH_f.h"
#include "thotmsg_f.h"
#include "tree_f.h"
#include "undo_f.h"
#include "views_f.h"


/*----------------------------------------------------------------------
  NotifySubTree sends an event appEvent.Post for the element pEl
  of the document pDoc and if necessary all its children.
  Parameters origDoc (document from which comes pEl) and info (used
  by columns management) are given.
  When inTable is TRUE row elements are not notified.
  When inRow is TRUE cell elements are not notified.
  ----------------------------------------------------------------------*/
void NotifySubTree (APPevent appEvent, PtrDocument pDoc, PtrElement pEl,
                    int origDoc, int info, ThotBool inTable,  ThotBool inRow)
{
  NotifyElement       notifyEl;
  PtrElement          pChild, pNext;
  ThotBool            notify;

  if (pEl == NULL || pEl->ElStructSchema == NULL)
    return;
  /* send event appEvent.Post to element pEl */
  notifyEl.event = appEvent;
  notifyEl.document = (Document) IdentDocument (pDoc);
  notifyEl.element = (Element) pEl;
  notifyEl.elementType.ElTypeNum = pEl->ElTypeNumber;
  notifyEl.elementType.ElSSchema = (SSchema) (pEl->ElStructSchema);
  notifyEl.position = origDoc;
  notifyEl.info = info; /* not sent by undo */

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
           TypeHasException (ExcIsCell, pEl->ElTypeNumber,
                             pEl->ElStructSchema))
    notify = FALSE;
  else if (inTable &&
           TypeHasException (ExcIsColHead, pEl->ElTypeNumber,
                             pEl->ElStructSchema))
    notify = FALSE;
  if (notify)
    CallEventType ((NotifyEvent *) & notifyEl, FALSE);
  if (pDoc->DocNotifyAll)
    /* the document needs an event for each element in the subtree */
    if (pEl->ElStructSchema && !pEl->ElTerminal)
      /* the element has not been deleted by the callback */
      {
        pChild = pEl->ElFirstChild;
        while (pChild != NULL)
          {
            /* save pointer on next child, in case the current child
               is deleted by the application */
            pNext = pChild->ElNext;
            NotifySubTree (appEvent, pDoc, pChild, origDoc, info,
                           inTable, inRow);
            pChild = pNext;
          }
      }
}

/*----------------------------------------------------------------------
  TtaNotifySubTree sends an event appEvent.Pre or appEvent.Post for the
  element elem of the document doc and if necessary all its children.
  The parameter pre says if the notification is before or after the action.
  Returns TRUE if pre is TRUE and the application refuses the operation  
  ----------------------------------------------------------------------*/
ThotBool TtaNotifySubTree (APPevent appEvent, Document doc, Element elem,
                           ThotBool pre)
{
  UserErrorCode = 0;
  if (elem == NULL)
    TtaError (ERR_invalid_parameter);
  else if (doc < 1 || doc > MAX_DOCUMENTS)
    TtaError (ERR_invalid_document_parameter);
  else if (pre)
    return SendEventSubTree (appEvent, LoadedDocument[doc - 1], (PtrElement)elem,
                             0, 0, FALSE, FALSE);
  else
    NotifySubTree (appEvent, LoadedDocument[doc - 1], (PtrElement)elem,
                   0, 0, FALSE, FALSE);
  return FALSE;
}

/*----------------------------------------------------------------------
  InsertOption  met l'element pOption a la place de l'element de	
  type Choice pEl, sauf si celui-ci est un element d'agregat ou la	
  racine du schema de structure.		
  Dans ce cas, le nouvel element est chaine' comme premier fils de   
  l'element pEl.							
  ----------------------------------------------------------------------*/
void InsertOption (PtrElement pEl, PtrElement *pOption, PtrDocument pDoc)
{
  ThotBool            absBoxExist;
  int                 view;

  InsertElemInChoice (pEl, pOption, pDoc, FALSE);
  if (pEl == *pOption)
    /* il y a eu substitution */
    /* libere les paves de l'element Choice s'il en a */
    {
      /* cherche si l'element a au moins un pave */
      absBoxExist = FALSE;
      for (view = 0; view < MAX_VIEW_DOC && !absBoxExist; view++)
        if (pEl->ElAbstractBox[view] != NULL)
          absBoxExist = TRUE;
      if (absBoxExist)
        /* il a au moins un pave */
        /* detruit les paves de l'element dans toutes les vues */
        {
          DestroyAbsBoxes (pEl, pDoc, FALSE);
          AbstractImageUpdated (pDoc);
        }
    }
}

/*----------------------------------------------------------------------
  GetExternalTypeName retourne le pointeur typeName le nom qui doit
  etre presente a l'utilisateur pour l'element portant   
  le numero de type typeNum dans le schema de structure pSS.
  ----------------------------------------------------------------------*/
void GetExternalTypeName (PtrSSchema pSS, int typeNum, char **typeName)
{
  if (!TypeHasException (ExcHidden, typeNum, pSS))
    /* ce type d'element ne porte pas l'exception Hidden, on retourne
       le nom de la regle qui le definit */
    *typeName = pSS->SsRule->SrElem[typeNum - 1]->SrName;
  else
    /* ce type d'element porte l'exception Hidden */
    if (pSS->SsRule->SrElem[typeNum - 1]->SrConstruct == CsList)
      /* c'est une liste, on retourne le nom de ses elements */
      *typeName = pSS->SsRule->SrElem[pSS->SsRule->SrElem[typeNum - 1]->SrListItem - 1]->SrName;
    else if (pSS->SsRule->SrElem[typeNum - 1]->SrConstruct == CsAggregate ||
             pSS->SsRule->SrElem[typeNum - 1]->SrConstruct == CsUnorderedAggregate)
      /* c'est un agregat, on retourne le nom de son 1er element */
      *typeName = pSS->SsRule->SrElem[pSS->SsRule->SrElem[typeNum - 1]->SrComponent[0] - 1]->SrName;
    else
      /* ce n'est ni une liste ni un agregat, on ignore */
      /* l'exception Hidden */
      *typeName = pSS->SsRule->SrElem[typeNum - 1]->SrName;
}

/*----------------------------------------------------------------------
  BuildAbsBoxSpliText On a coupe' en deux la feuille de texte pEl.	
  construit les paves de la 2eme partie (pNewEl) et	
  met a jour ceux de la premiere partie.                  
  pNextEl pointe sur le frere suivant de pEl avant la     
  coupure.                                                
  ----------------------------------------------------------------------*/
void BuildAbsBoxSpliText (PtrElement pEl, PtrElement pNewEl,
                          PtrElement pNextEl, PtrDocument pDoc)
{
  int                 view;

  if (pNextEl == NULL)
    /* L'element divise' etait le dernier parmi ses freres. */
    /* La premiere partie n'est plus le dernier parmi ses freres */
    ChangeFirstLast (pEl, pDoc, FALSE, TRUE);
  if (pNewEl != NULL && pNewEl->ElStructSchema != NULL)
    /* cree dans toutes les views les paves de la deuxieme partie */
    CreateAllAbsBoxesOfEl (pNewEl, pDoc);
  /* change le volume de tous les paves de la 1e partie de texte */
  for (view = 0; view < MAX_VIEW_DOC; view++)
    UpdateAbsBoxVolume (pEl, view, pDoc);
}

/*----------------------------------------------------------------------
  SplitBeforeSelection     coupe en deux la feuille de texte	
  firstSel, a la position firstChar. Met a jour en	
  consequence les variables lastSel et lastChar si elles	
  representent un caractere dans la feuille coupee.       
  ----------------------------------------------------------------------*/
void SplitBeforeSelection (PtrElement *firstSel, int *firstChar,
                           PtrElement *lastSel,
                           int *lastChar, PtrDocument pDoc)
{
  PtrElement          pSecond, pNext;

  if ((*firstSel)->ElTerminal && (*firstSel)->ElLeafType == LtText &&
      *firstChar > 1)
    /* c'est bien une feuille de texte et il faut effectivement la couper */
    {
      /* on coupe en deux l'element feuille dans l'arbre abstrait */
      /* ce qui cree un deuxieme element feuille juste apres l'element */
      /* initial */
      pNext = SiblingElement (*firstSel, FALSE);
      SplitTextElement (*firstSel, *firstChar, pDoc, FALSE, &pSecond, FALSE);
      BuildAbsBoxSpliText (*firstSel, pSecond, pNext, pDoc);
      if (*firstSel == *lastSel)
        /* la fin de la selection est dans le nouvel element cree */
        {
          /* met a jour les variables representant la fin de la selection */
          *lastSel = pSecond;
          if (*lastChar > 0)
            *lastChar = *lastChar - *firstChar + 1;
        }
      /* on fait comme si la selection demarrait au debut de la feuille */
      /* de texte qui vient d'etre creee */
      *firstSel = pSecond;
      *firstChar = 1;
    }
}

/*----------------------------------------------------------------------
  SplitAfterSelection
  Coupe en deux l'element de texte lastSel.
  lastChar est le rang dans cet element du caractere ou on coupe.
  ----------------------------------------------------------------------*/
void SplitAfterSelection (PtrElement lastSel, int lastChar, PtrDocument pDoc)
{
  PtrElement          pNextEl, pSecond;

  if (lastChar > 1 && lastChar <= lastSel->ElTextLength)
    /* il faut effectivement couper l'element */
    {
      /* on cherche le frere suivant qui n'est pas un saut de page */
      pNextEl = SiblingElement (lastSel, FALSE);
      /* on coupe en deux la feuille de texte dans l'arbre abstrait */
      SplitTextElement (lastSel, lastChar, pDoc, FALSE, &pSecond, FALSE);
      /* construit les paves de la 2eme partie et met a jours ceux de */
      /* la premiere partie */
      BuildAbsBoxSpliText (lastSel, pSecond, pNextEl, pDoc);
    }
}


/*----------------------------------------------------------------------
  SameLeafType							
  ----------------------------------------------------------------------*/
static ThotBool     SameLeafType (LeafType type1, LeafType type2)
{
  if (type1 == type2)
    return TRUE;
  if ((type2 == LtGraphics || type2 == LtPolyLine || type2 == LtPath) &&
      (type1 == LtGraphics || type1 == LtPolyLine || type1 == LtPath))
    return TRUE;
   
  return FALSE;
}


/*----------------------------------------------------------------------
  CreateALeaf							
  Cree, pour le premier element selectionne', la descendance      
  prevue par le schema de structure, jusqu'a une feuille de       
  type leafType. pAB indique dans quelle vue on travaille.        
  Si l'element n'est pas vide, cree une feuille avant ou apres	
  (selon before) selection courante si c'est autorise' par le	
  schema de structure.						
  Retourne un pointeur sur le pave correspondant a cette feuille  
  ou NULL si la creation n'a pas pu se faire.                     
  ----------------------------------------------------------------------*/
PtrAbstractBox CreateALeaf (PtrAbstractBox pAB, int *frame, LeafType leafType,
                            ThotBool before)
{
  PtrElement        pEl, lastSel, pLeaf, pE, pC, pChild, pNextEl, pSibling;
  PtrAbstractBox    pCreatedAB;
  PtrSSchema        pSS;
  PtrDocument       pDoc;
  Document          doc;
  NotifyElement     notifyEl;
  int               lType, ruleNum;
  int               view, firstChar, lastChar, nNew, i, nSiblings;
  ThotBool          ident, isList, stop, empty, optional, opened;

  pCreatedAB = NULL;
  *frame = 0;
  lType = 0;
  nNew = 0;
  /* regarde s'il y a une selection pour l'editeur */
  if (!GetCurrentSelection (&pDoc, &pEl, &lastSel, &firstChar, &lastChar))
    /* il n'y en a pas, message d'erreur et fin */
    return pCreatedAB;
  else if (!pDoc->DocReadOnly && !ElementIsReadOnly (pEl))
    /* il y a bien une selection, on travaille sur le premier element */
    /* de la selection */
    /* on ne peut inserer ou coller dans un document en lecture seule */
    {
      doc = IdentDocument (pDoc);
      if (pEl && pEl->ElTerminal && pEl->ElLeafType == LtPicture)
        {
          // create around the IMG instead of the PICTURE element
          pE = pEl->ElParent;
          if (pE &&
              TypeHasException (ExcIsImg, pE->ElTypeNumber, pE->ElStructSchema))
            pEl = pE;
        }
      pE = NULL;
      pLeaf = NULL;
      empty = TRUE;
      /* determine le type de l'element feuille a creer */
      switch (leafType)
        {
        case LtText:
          lType = CharString + 1;
          break;
        case LtGraphics:
          lType = GraphicElem + 1;
          break;
        case LtSymbol:
          lType = Symbol + 1;
          break;
        case LtPicture:
          lType = tt_Picture + 1;
          break;
        case LtPolyLine:
          lType = GraphicElem + 1;
          break;
        case LtPath:
          lType = GraphicElem + 1;
          break;
        default:
          break;
        }
      /* verifie qu'on peut bien creer ce type de feuille ici */
      if (!ExcludedType (pEl, lType, NULL))
        {
          if (pEl->ElTerminal)
            /* on n'insere pas dans une feuille protegee en ecriture ni */
            /* dans une constante */
            if (SameLeafType (pEl->ElLeafType, leafType) &&
                !pEl->ElIsCopy &&
                !pEl->ElHolophrast &&
                pEl->ElAccess != ReadOnly &&
                pEl->ElStructSchema->SsRule->SrElem[pEl->ElTypeNumber - 1]->SrConstruct != CsConstant)
              /* element de la nature cherchee */
              empty = TRUE;
            else
              /* on creera une feuille devant */
              empty = FALSE;
          else
            /* element non terminal */
            /* cherche si la descendance de l'element se reduit a un */
            /* element vide ou a une feuille vide */
            {
              pChild = pEl->ElFirstChild;
              stop = FALSE;
              empty = TRUE;
              while (pChild != NULL && !stop)
                /* saute les marques de page */
                {
                  pNextEl = pChild;
                  FwdSkipPageBreak (&pNextEl);
                  if (pNextEl == NULL)
                    /* il n'y a que des marques de pages, l'element est vide */
                    {
                      pChild = pChild->ElParent;
                      stop = TRUE;
                    }
                  else
                    /* il y a un fils qui n'est pas une marque de page */
                    {
                      pChild = pNextEl;
                      /* saute les marques de page qui le suivent */
                      FwdSkipPageBreak (&pNextEl);
                      if (pNextEl != NULL)
                        pNextEl = pNextEl->ElNext;
                      if (pNextEl != NULL)
                        /* le fils a un frere */
                        {
                          pChild = NULL;
                          empty = FALSE;
                        }
                      else
                        {
                          if (pChild->ElTerminal)
                            {
                              switch (pChild->ElLeafType)
                                {
                                case LtPicture:
                                case LtText:
                                  if (pChild->ElTextLength == 0)
                                    /* la descendance se reduit a une feuille vide */
                                    stop = TRUE;
                                  else
                                    /* la feuille n'est pas vide */
                                    {
                                      pChild = NULL;
                                      empty = FALSE;
                                    }
                                  break;
                                case LtPolyLine:
                                  if (pChild->ElNPoints == 0)
                                    stop = TRUE;
                                  else
                                    {
                                      pChild = NULL;
                                      empty = FALSE;
                                    }
                                  break;
                                case LtPath:
                                  if (pChild->ElVolume == 0)
                                    stop = TRUE;
                                  else
                                    {
                                      pChild = NULL;
                                      empty = FALSE;
                                    }
                                  break;
                                case LtSymbol:
                                case LtGraphics:
                                case LtCompound:
                                  if (pChild->ElGraph == EOS)
                                    /* la descendance se reduit a une feuille vide */
                                    stop = TRUE;
                                  else
                                    /* la feuille n'est pas vide */
                                    {
                                      pChild = NULL;
                                      empty = FALSE;
                                    }
                                  break;
                                default:
                                  pChild = NULL;
                                  empty = FALSE;
                                  break;
                                }
                              if (stop &&
                                  (pChild->ElIsCopy || pChild->ElHolophrast ||
                                   pChild->ElStructSchema->SsRule->SrElem[pChild->ElTypeNumber - 1]->SrConstruct == CsConstant ||
                                   ElementIsReadOnly (pChild)))
                                {
                                  stop = FALSE;
                                  pChild = NULL;
                                  empty = FALSE;
                                }
                            }
                          else if (pChild->ElFirstChild == NULL)
                            /* la descendance se reduit a un element vide */
                            stop = TRUE;
                          else
                            pChild = pChild->ElFirstChild;
                        }
                    }
                }
              if (pChild != NULL)
                pEl = pChild;
            }

          /* Determine la vue dans laquelle l'utilisateur travaille */
          if (pAB != NULL)
            /* on prend la vue choisie par l'utilisateur */
            view = pAB->AbDocView;
          else
            /* pas de selection */
            /* cherche la premiere vue ou l'element a un pave */
            {
              view = 0;
              do
                view++;
              while (pEl->ElAbstractBox[view - 1] == NULL && view != MAX_VIEW_DOC);
            }

          /* on cree une descendance pour cet element */
          if (!empty)
            /* l'element a deja une descendance */
            /* essaie de creer une feuille du type voulu devant l'element */
            /* (ou apres, selon before). */
            {
              do
                {
                  SRuleForSibling (pDoc, pEl, before, 1, &ruleNum, &pSS, &isList, &optional);
                  if (ruleNum == 0)
                    /* pas de voisin possible a ce niveau */
                    /* essaie au niveau superieur si c'est le premier element */
                    /* saute les marques de pages qui precedent l'element */
                    {
                      if (before)
                        {
                          pNextEl = pEl->ElPrevious;
                          BackSkipPageBreak (&pNextEl);
                        }
                      else
                        {
                          pNextEl = pEl->ElNext;
                          FwdSkipPageBreak (&pNextEl);
                        }
                      if (pNextEl == NULL)
                        pEl = pEl->ElParent;
                      else
                        pEl = NULL;
                    }
                }
              while (ruleNum == 0 && pEl != NULL);

              if (ruleNum > 0 && pEl != NULL)
                /* il y a un voisin possible */
                if (EquivalentSRules (ruleNum, pSS, lType, pEl->ElStructSchema, pEl->ElParent))
                  /* le voisin possible est du type voulu */
                  /* cree la feuille demandee */
                  {
                    /* envoie d'abord l'evenement ElemNew.Pre */
                    notifyEl.event = TteElemNew;
                    notifyEl.document = doc;
                    notifyEl.element = (Element) (pEl->ElParent);
                    notifyEl.info = 0; /* not sent by undo */
                    notifyEl.elementType.ElTypeNum = lType;
                    notifyEl.elementType.ElSSchema = (SSchema) (pEl->ElStructSchema);
                    nSiblings = 0;
                    pSibling = pEl;
                    while (pSibling->ElPrevious != NULL)
                      {
                        nSiblings++;
                        pSibling = pSibling->ElPrevious;
                      }
                    if (!before)
                      nSiblings++;
                    notifyEl.position = nSiblings;
                    if (!CallEventType ((NotifyEvent *) & notifyEl, TRUE))
                      {
                        pE = NewSubtree (lType, pEl->ElStructSchema, pDoc,
                                         TRUE, TRUE, TRUE, TRUE);
                        CancelSelection ();
                        /* si la selection commence a l'interieur d'une feuille */
                        /* de texte, on coupe cette feuille en deux */
                        if (pEl->ElTerminal && pEl->ElLeafType == LtText)
                          {
                            if (before)
                              {
                                if (firstChar > 1)
                                  {
                                    if (firstChar > pEl->ElTextLength)
                                      /* insert after the selected element */
                                      before = FALSE;
                                    else
                                      {
                                        lastChar = 0;
                                        /* empeche la coupure apres le dernier */
                                        /* caractere selectionne' */
                                        IsolateSelection (pDoc, &pEl, &lastSel, &firstChar, &lastChar, FALSE);
                                      }
                                  }
                              }
                            else if (lastChar <= pEl->ElTextLength)
                              {
                                firstChar = 0;
                                /* empeche la coupure avant le premier */
                                /* caractere selectionne' */
                                IsolateSelection (pDoc, &pEl, &lastSel, &firstChar, &lastChar, FALSE);
                              }
                          }
                        nNew = 1;
                        pLeaf = pE;
                        if (!SameSRules (ruleNum, pSS, lType, pEl->ElStructSchema))
                          /* le voisin prevu n'est pas du type de la feuille creee, */
                          /* cree l'element englobant de la feuille sauf si c'est */
                          /* un choix dans une liste */
                          if (!isList)
                            {
                              notifyEl.event = TteElemNew;
                              notifyEl.document = doc;
                              notifyEl.element = (Element) (pEl->ElParent);
                              notifyEl.info = 0; /* not sent by undo */
                              notifyEl.elementType.ElTypeNum = ruleNum;
                              notifyEl.elementType.ElSSchema = (SSchema) pSS;
                              notifyEl.position = nSiblings;
                              if (CallEventType ((NotifyEvent *) & notifyEl, TRUE))
                                {
                                  DeleteElement (&pE, pDoc);
                                  pE = NULL;
                                  nNew = 0;
                                }
                              else
                                {
                                  // add a new child
                                  pE = NewSubtree (ruleNum, pSS, pDoc, FALSE,
                                                   TRUE, TRUE, TRUE);
                                  InsertChildFirst (pE, pLeaf, &pLeaf, pDoc);
#ifdef IV
                                  // should we add this ???? register the update
                                  opened = pDoc->DocEditSequence;
                                  if (!opened)
                                    OpenHistorySequence (pDoc, pEl, pEl, NULL,
                                                         firstChar, lastChar);
                                  AddEditOpInHistory (pLeaf, pDoc, FALSE, TRUE);
                                  if (!opened)
                                    CloseHistorySequence (pDoc);
#endif
                                }
                            }
                        if (pE != NULL)
                          {
                            if (before)
                              InsertElementBefore (pEl, pE);
                            else
                              InsertElementAfter (pEl, pE);
                          }
                      }
                  }
            }
          else
            /* l'element a une descendance vide */
            {
              if (pEl->ElTerminal && SameLeafType (pEl->ElLeafType, leafType))
                /* la feuille du type voulu existe deja */
                {
                  SelectElement (pDoc, pEl, FALSE, FALSE, TRUE);
                  pLeaf = pEl;
                }
              else
                /* pas de feuille du type voulu, on essaie d'en creer une */
                {
                  notifyEl.event = TteElemNew;
                  notifyEl.document = doc;
                  notifyEl.element = (Element) (pEl->ElParent);
                  notifyEl.info = 0; /* not sent by undo */
                  notifyEl.elementType.ElTypeNum = lType;
                  notifyEl.elementType.ElSSchema = (SSchema) (pEl->ElStructSchema);
                  notifyEl.position = 0;
                  if (CallEventType ((NotifyEvent *) & notifyEl, TRUE))
                    pE = NULL;
                  else
                    pE = CreateDescendant (pEl->ElTypeNumber,
                                           pEl->ElStructSchema, pDoc, &pLeaf,
                                           lType, pEl->ElStructSchema, pEl);
                  if (pE != NULL)
                    {
                      CancelSelection ();
                      nNew = 1;
                      /* chaine l'element cree', suivant son constructeur */
                      if (pEl->ElStructSchema->SsRule->SrElem[pEl->ElTypeNumber - 1]->SrConstruct == CsChoice ||
                          (pEl->ElStructSchema->SsRule->SrElem[pEl->ElTypeNumber - 1]->SrConstruct == CsAny &&
                           TypeHasException (ExcIsPlaceholder, pEl->ElTypeNumber, pEl->ElStructSchema)))
                        {
                          if (pLeaf == pE)
                            ident = TRUE;
                          else
                            ident = FALSE;
                          /* register changes */
                          opened = pDoc->DocEditSequence;
                          if (!opened)
                            OpenHistorySequence (pDoc, pEl, pEl, NULL,
                                                 firstChar, lastChar);
                          AddEditOpInHistory (pEl, pDoc, TRUE, FALSE);
                          InsertOption (pEl, &pE, pDoc);
                          AddEditOpInHistory (pE, pDoc, FALSE, TRUE);
                          if (pEl == pE && ident)
                            pLeaf = pEl;
                          if (!opened)
                            CloseHistorySequence (pDoc);
                        }
                      else
                        /* compte les elements nouvellement crees au 1er niveau */
                        {
                          pC = pE;
                          while (pC->ElNext != NULL)
                            {
                              nNew++;
                              pC = pC->ElNext;
                            }
                          InsertFirstChild (pEl, pE);
                          opened = pDoc->DocEditSequence;
                          if (!opened)
                            OpenHistorySequence (pDoc, pEl, pEl, NULL,
                                                 firstChar, lastChar);
                          AddEditOpInHistory (pE, pDoc, FALSE, TRUE);
                          if (!opened)
                            CloseHistorySequence (pDoc);
                        }
                    }
                }
            }
          *frame = pDoc->DocViewFrame[view - 1];
          if (pE != NULL)
            RemoveExcludedElem (&pE, pDoc);
          /* cree les paves des nouveaux elements et les affiche */
          if (pE != NULL)
            {
              /* traite les attributs requis des elements crees */
              AttachMandatoryAttributes (pE, pDoc);
              if (pDoc->DocSSchema == NULL)
                /* le document a ete ferme' entre temps */
                pLeaf = NULL;
              else
                {
                  SetDocumentModified (pDoc, TRUE, 10);
                  /* le document est modifie' */
                  FirstCreation = TRUE;
                  for (i = 1; i <= nNew; i++)
                    {
                      /* envoie un evenement ElemNew pour tous les elements crees */
                      NotifySubTree (TteElemNew, pDoc, pE, 0, 0, FALSE, FALSE);
                      CreateAllAbsBoxesOfEl (pE, pDoc);
                      if (i < nNew)
                        pE = pE->ElNext;
                    }
                  FirstCreation = FALSE;
                  AbstractImageUpdated (pDoc);
                  RedisplayDocViews (pDoc);
                  /* Reaffiche les copies des elements */
                  /* contenant le nouvel element */
                  RedisplayCopies (pE, pDoc, TRUE);
                  /* met a jour les numeros qui suivent */
                  UpdateNumbers (NextElement (pE), pE, pDoc, TRUE);
                }
            }
          if (pLeaf != NULL)
            pCreatedAB = pLeaf->ElAbstractBox[view - 1];
        }
    }
  /* saute les paves de presentation */
  stop = FALSE;
  do
    {
      if (pCreatedAB == NULL)
        stop = TRUE;
      else if (!pCreatedAB->AbPresentationBox)
        stop = TRUE;
      if (!stop)
        pCreatedAB = pCreatedAB->AbNext;
    }
  while (!stop);
  
  return pCreatedAB;
}

/*----------------------------------------------------------------------
  RedisplayNewContent	redisplays element pEl in all views, except in	
  view skipView.							
  ----------------------------------------------------------------------*/
void RedisplayNewContent (PtrElement pEl, PtrDocument pDoc, int dVol,
                          int skipView, PtrAbstractBox pAbEl)
{
  PtrAbstractBox      pAb;
  ThotPictInfo       *picture1, *picture2;
  int                 view, frame, h;

  for (view = 0; view < MAX_VIEW_DOC; view++)
    if (pEl->ElAbstractBox[view] != NULL)
      /* un pave correspondant existe dans la vue view */
      {
        /* met a jour le volume dans les paves englobants */
        pAb = pEl->ElAbstractBox[view]->AbEnclosing;
        while (pAb != NULL)
          {
            pAb->AbVolume += dVol;
            pAb = pAb->AbEnclosing;
          }

        if (view + 1 != skipView)
          /* met a jour le contenu et le volume et demande le */
          /* reaffichage du pave sauf pour la vue ou la saisie a ete */
          /* faite par le mediateur */
          {
            pAb = pEl->ElAbstractBox[view];
            /* on saute les paves de pre'sentation, pour trouver le pave' */
            /* principal de l'element, celui qui doit recevoir le nouveau */
            /* contenu. */
            while (pAb->AbPresentationBox && pAb->AbElement == pEl)
              pAb = pAb->AbNext;
            /* s'il n'y a pas de pave' principal, il n'y rien a faire */
            if (pAb->AbElement == pEl && !pAb->AbPresentationBox)
              {
                pAb->AbVolume += dVol;
                pAb->AbChange = TRUE;
                switch (pEl->ElLeafType)
                  {
                  case LtPicture:
                    if (pAbEl != NULL)
                      {
                        picture1 = (ThotPictInfo *) pAbEl->AbPictInfo;
                        picture2 = (ThotPictInfo *) pAb->AbPictInfo;
                        /* on fait une maj de pAb->AbPictInfo */
                        if (picture2 != NULL)
                          {
                            picture2->PicType = picture1->PicType;
                            picture2->PicPresent = picture1->PicPresent;
                            picture2->PicPixmap = picture1->PicPixmap;
                            picture2->PicWArea = picture1->PicWArea;
                            picture2->PicHArea = picture1->PicHArea;
                            picture2->PicWidth = picture1->PicWidth;
                            picture2->PicHeight = picture1->PicHeight;
                          }
                      }
                    break;
                  case LtText:
                    pAb->AbText = pEl->ElText;
                    pAb->AbLang = pEl->ElLanguage;
                    break;
                  case LtPolyLine:
                    pAb->AbLeafType = LtPolyLine;
                    pAb->AbPolyLineBuffer = pEl->ElPolyLineBuffer;
                    pAb->AbPolyLineShape = pEl->ElPolyLineType;
                    pAb->AbVolume = pEl->ElNPoints;
                    break;
                  case LtPath:
                    pAb->AbLeafType = LtPath;
                    pAb->AbFirstPathSeg = pEl->ElFirstPathSeg;
                    pAb->AbVolume = pEl->ElVolume;
                    break;
                  case LtSymbol:
                    pAb->AbLeafType = pEl->ElLeafType;
                    pAb->AbShape = pEl->ElGraph;
                    break;
                  case LtGraphics:
                    pAb->AbLeafType = pEl->ElLeafType;
                    pAb->AbShape = pEl->ElGraph;
                    pAb->AbGraphScript = 'G';
                    if (pEl->ElLeafType == LtGraphics && 
			(pEl->ElGraph == 2 || pEl->ElGraph == 3))
                      /* rectangle with rounded corners
		       */
                      {
                        pAb->AbRx = 0;
                        pAb->AbRy = 0;
                      }
                    break;
                  default:
                    break;
                  }

                /* memorise le pave a reafficher */
                frame = pDoc->DocViewFrame[view];
                if (pAb)
                  {
                    /* @@@@@@ generate a crash if the enclosing box is not created */
                    while (pAb->AbEnclosing &&
                           pAb->AbEnclosing->AbNew || pAb->AbEnclosing->AbDead)
                      pAb = pAb->AbEnclosing;
                    ClearViewSelection (frame);
                    h = 0;
                    /* on ne s'occupe pas de la hauteur de page */
                    ChangeConcreteImage (frame, &h, pAb);
                    if (TtaGetDisplayMode (FrameTable[frame].FrDoc) ==
                        DisplayImmediately)
                      DisplayFrame (frame);
                  }
              }
          }	/* fin mise a jour du contenu pour la vue */
      } /* fin boucle de parcours des vues */

  /* si l'element modifie' appartient soit a un element copie' */
  /* dans des paves par une regle Copy, soit a un element inclus */
  /* dans d'autres, il faut reafficher ses copies */
  RedisplayCopies (pEl, pDoc, TRUE);
  /* effectue eventuellement une sauvegarde automatique */
  /* DocBackUpInterval = 0 signifie pas de sauvegarde automatique */
  /* this code is now obsolete */
  /*
    if (DocBackUpInterval > 0)
    if (pDoc->DocNTypedChars >= DocBackUpInterval)
    {
    (*ThotLocalActions[T_writedocument]) (pDoc, 1);
    pDoc->DocNTypedChars = 0;
    }
  */
}

/*----------------------------------------------------------------------
  NewContent
  Transmit the content (text, symbol, image) change of the element or
  the attribute.
  The parameter pAb gets the modified abstract box. This abstract box
  gets the new volume and the new set of text buffers.
  ----------------------------------------------------------------------*/
void NewContent (PtrAbstractBox pAb)
{
  PtrDocument         pDoc;
  PtrElement          pEl, pAncest;
  PtrAttribute        pAttr, pNewAttr;
  unsigned char       text[10];
  char               *name;
  int                 dVol, len, view;
  ThotBool            selInAttr;

  /* cherche le document auquel appartient le pave */
  pEl = pAb->AbElement;
  pDoc = DocumentOfElement (pEl);
  if (pAb->AbPresentationBox && pAb->AbCanBeModified)
    /* c'est un pave' affichant la valeur d'un attribut */
    {
      if (pAb->AbCreatorAttr)
        {
          /* check if that attribute is selected */
          selInAttr = (pAb == AbsBoxSelectedAttr);
          pAttr = pAb->AbCreatorAttr;
          view = pAb->AbDocView;
          GetAttribute (&pNewAttr);
          pNewAttr->AeAttrSSchema = pAttr->AeAttrSSchema;
          pNewAttr->AeAttrNum = pAttr->AeAttrNum;
          pNewAttr->AeDefAttr = pAttr->AeDefAttr;
          pNewAttr->AeAttrType = pAttr->AeAttrType;
          switch (pNewAttr->AeAttrType)
            {
            case AtNumAttr:
              CopyBuffer2MBs (pAb->AbText, 0, text, 9);
              sscanf ((char*)text, "%d", (int *)&pNewAttr->AeAttrValue);
              break;
            case AtTextAttr:
              if (pNewAttr->AeAttrText == NULL)
                GetTextBuffer (&pNewAttr->AeAttrText);
              else
                ClearText (pNewAttr->AeAttrText);
              /* special treatments for id, name and accesskey attributes */
              name = pAttr->AeAttrSSchema->SsAttribute->TtAttr[pAttr->AeAttrNum - 1]->AttrName;
              if (!strcmp (name, "accesskey") &&
                  !strcmp (pAttr->AeAttrSSchema->SsName, "HTML"))
                /* only one character is allowed */
                pAb->AbText->BuContent[1] = EOS;
              CopyTextToText (pAb->AbText, pNewAttr->AeAttrText, &len);
              if (!strcmp (name, "id") ||
                  !strcmp (name, "xml:id") ||
                  (!strcmp (name, "name") &&
                   !strcmp (pAttr->AeAttrSSchema->SsName, "HTML")))
                TtaIsValidID ((Attribute)pNewAttr, TRUE);
              /* the Abstract box will be regerated by AttachAttrWithValue */
              ClearText (pAb->AbText);
              break;
            default:
              break;
            }
          /* redisplay the attribute */
          AttachAttrWithValue (pEl, pDoc, pNewAttr, TRUE);
          if (pNewAttr->AeAttrType == AtTextAttr && pAb->AbText &&
              pNewAttr->AeAttrText->BuLength != pAb->AbText->BuLength)
            {
              /* to be sure that the abstract box is updated */
              ClearText (pAb->AbText);
              CopyTextToText (pNewAttr->AeAttrText, pAb->AbText, &len);
            }
          CloseHistorySequence (pDoc);
          pAttr = AttributeValue (pEl, pNewAttr);
          DeleteAttribute (NULL, pNewAttr);
          /* apply these changes */
          AbstractImageUpdated (pDoc);
          RedisplayDocViews (pDoc);
          if (selInAttr)
            HighlightAttrSelection (pDoc, pEl, pAttr, FirstSelectedCharInAttr,
                                    LastSelectedCharInAttr);
        }
    }
  else if (pEl->ElTerminal)
    /* il s'agit d'une feuille */
    /* mise a jour de l'element de l'arbre abstrait */
    {
      /* dVol: difference de volume du pave */
      dVol = 0;
      switch (pAb->AbLeafType)
        {
        case LtPicture:
          pEl->ElTextLength = pAb->AbVolume;
          pEl->ElVolume = 100;
          dVol = pAb->AbVolume - pEl->ElVolume;
          pEl->ElText->BuLength = ustrlen (pEl->ElText->BuContent);
          break;
        case LtText:
          dVol = pAb->AbVolume - pEl->ElTextLength;
          pEl->ElTextLength = pAb->AbVolume;
          pEl->ElVolume = pEl->ElTextLength;
          pEl->ElText = pAb->AbText;
          break;
        case LtPolyLine:
          if (pEl->ElLeafType == LtGraphics)
            /* un graphique simple a transformer en Polyline */
            {
              pEl->ElLeafType = LtPolyLine;
              if (pEl->ElGraph == EOS)
                pEl->ElNPoints = 0;
              else
                pEl->ElNPoints = 1;
              pEl->ElPolyLineType = EOS;
            }
          dVol = pAb->AbVolume - pEl->ElNPoints;
          pEl->ElNPoints = pAb->AbVolume;
          pEl->ElVolume = pAb->AbVolume;
          pEl->ElPolyLineBuffer = pAb->AbPolyLineBuffer;
          pEl->ElPolyLineType = pAb->AbPolyLineShape;
          break;
        case LtSymbol:
        case LtGraphics:
          if (pAb->AbLeafType == LtGraphics &&
              pEl->ElLeafType == LtPolyLine)
            /* une Polyline a transformer en graphique simple */
            {
              dVol = -pEl->ElNPoints;
              if (pAb->AbShape != EOS)
                dVol++;
              ClearText (pEl->ElPolyLineBuffer);
              FreeTextBuffer (pEl->ElPolyLineBuffer);
              pEl->ElLeafType = LtGraphics;
            }
          else
            {
              if (pEl->ElGraph == EOS && pAb->AbShape != EOS)
                dVol = 1;
              if (pEl->ElGraph != EOS && pAb->AbShape == EOS)
                dVol = -1;
            }
          /* don't change a symbol representing a wide char */
          if (pEl->ElLeafType != LtSymbol || pEl->ElGraph != '?')
            {
              pEl->ElGraph = pAb->AbShape;
              /* pDoc->DocNTypedChars += 5; */
              pEl->ElVolume += dVol;
            }
          break;
        default:
          break;
        }
      /* ajoute le volume a celui des elements englobants */
      if (dVol != 0)
        {
          /* pDoc->DocNTypedChars += abs (dVol); */
          pAncest = pEl->ElParent;
          while (pAncest != NULL)
            {
              pAncest->ElVolume = pAncest->ElVolume + dVol;
              pAncest = pAncest->ElParent;
            }
        }
      SetDocumentModified (pDoc, TRUE, 0);
      /* traite les paves correspondant dans les autres vues */
      RedisplayNewContent (pEl, pDoc, dVol, pAb->AbDocView, pAb);
    }
}

/*----------------------------------------------------------------------
  LinkReference remplit un element ou un attribut de type reference     
  en demandant a l'utilisateur quel est l'element qui est reference'.
  pEl: pointeur sur l'element de type reference a remplir (si pAttr est NULL)
  ou pointeur sur un element qui portera l'attribut pAttr (si pAttr
	n'est pas NULL).				
  pAttr: pointeur sur l'attribut de type reference a remplir (si pas NULL).
  pDoc: pointeur sur le document auquel appartient cet element	ou cet
  attribut.						
  ----------------------------------------------------------------------*/
ThotBool LinkReference (PtrElement pEl, PtrAttribute pAttr, PtrDocument pDoc)
{
  PtrElement          pModifiedElem;
  PtrDocument         pSelDoc;
  PtrSSchema          pSS;
  PtrReference        pRef;
  PtrAbstractBox      pAb;
  int                 view, referredTypeNum, frame;
  ThotBool            again, new_, ret;

  ret = FALSE;
  pModifiedElem = NULL;
  pRef = NULL;

  /* cherche le type d'element reference' */
  referredTypeNum = 0;
  if (pAttr == NULL)
    ReferredType (pEl, NULL, &pSS, &referredTypeNum, pDoc);
  else
    ReferredType (NULL, pAttr, &pSS, &referredTypeNum, pDoc);
  new_ = FALSE;
  again = TRUE;
  if (!new_ && again)

    {
      if (pAttr != NULL)
        pEl = NULL;
      /* Pour designer la cible, l'utilisateur peut faire defiler le */
      /* document et ainsi faire afficher des graphiques "UserSpecified" */
      /* qui ne doivent pas etre traite's comme tels (ils ont ete cree's */
      /* pre'alablement). Il faut donc annuler FirstCreation */
      FirstCreation = FALSE;
      /* demande la selection d'un pave de la cible */
      GiveClickedAbsBox (&frame, &pAb);
      if (pAb != NULL)
        /* une selection de pave a bien ete faite */
        {
          if (pEl != NULL)
            {
              if (pEl->ElSource != NULL)
                /* c'est une inclusion d'element */
                /* pRef : pointeur sur le descripteur de reference */
                pRef = pEl->ElSource;
              else
                /* c'est un element reference */
                /* pRef : pointeur sur le descripteur de reference */
                pRef = pEl->ElReference;
            }
          if (pAttr != NULL)
            /* c'est un attribut reference */
            /* pRef : pointeur sur le descripteur de reference */
            pRef = pAttr->AeAttrReference;
          if (pRef != NULL)
            {
              /* si l'utilisateur n'a pas selectionne de pave, */
              /* on abandonne */
              if (pAb != NULL)
                /* un pave a ete selectionne' */
                /* par l'utilisateur */
                {
                  /* cherche le document (pointeur pSelDoc) auquel */
                  /* appartient la fenetre ou l'utilisateur a designe' */
                  /* un pave. */
                  GetDocAndView (frame, &pSelDoc, &view);
                  ret = SetReference (pEl, pAttr, pAb->AbElement, pDoc,
                                      pSelDoc, TRUE, TRUE);
                  if (ret)
                    {
                      /* une reference modifiee vaut 10 caracteres saisis */
                      SetDocumentModified (pDoc, TRUE, 10);
                      if (pEl != NULL)
                        /* si le lien est une inclusion de document */
                        /* externe, applique les regles de transmission */
                        /* des compteurs et des contenus d'elements */
                        {
                          ApplyTransmitRules (pEl, pDoc);
                          RepApplyTransmitRules (pEl, pEl, pDoc);
                        }
                    }
                }
            }
        }
    }
  if (pModifiedElem != NULL)
    {
      /* c'est trop tot pour creer les paves : la reference n'est */
      /* pas encore insereree dans l'arbre abstrait */
      /* garde le pointeur sur le sous arbre cree */
      NCreatedElements++;
      CreatedElement[NCreatedElements - 1] = pModifiedElem;
    }
  return ret;
}

/*----------------------------------------------------------------------
  AddChoiceMenuItem ajoute le nom item comme nouvelle entree dans le	
  menu contenu dans le buffer menuBuf.			
  ----------------------------------------------------------------------*/
static void AddChoiceMenuItem (Name item, int *menuInd, char *menuBuf)
{
  int                 len;

  len = strlen (item) + 1; 
  if (len + *menuInd < MAX_TXT_LEN)
    {
      strcpy (menuBuf + *menuInd, item);
      (*menuInd) += len;
    }
}


/*----------------------------------------------------------------------
  UserElementName retourne dans le buffer ret le nom qui doit etre	
  presente' a l'utilisateur pour l'element pEl.			
  begin indique si on s'interesse plutot au debut de l'element ou 
  la fin de l'element.                                            
  ----------------------------------------------------------------------*/
static void UserElementName (PtrElement pEl, ThotBool begin, char **ret)
{
  PtrSRule            pSRule;
  PtrElement          pChild;

  if (!TypeHasException (ExcHidden, pEl->ElTypeNumber, pEl->ElStructSchema))
    /* ce type d'element ne porte pas l'exception Hidden */
    {
      /* par defaut on retourne le type de l'element lui-meme */
      *ret = pEl->ElStructSchema->SsRule->SrElem[pEl->ElTypeNumber - 1]->SrName;
      /* la regle qui definit le type de l'element */
      pSRule = pEl->ElStructSchema->SsRule->SrElem[pEl->ElTypeNumber - 1];
      if (pSRule->SrConstruct == CsChoice)
        if (pEl->ElFirstChild != NULL)
          /* c'est un choix avec un fils */
          if (pEl->ElSource == NULL)
            /* ce n'est pas une inclusion, on prend le nom du fils */
            *ret = pEl->ElFirstChild->ElStructSchema->SsRule->SrElem[pEl->ElFirstChild->ElTypeNumber - 1]->SrName;
    }
  else
    /* ce type d'element porte l'exception Hidden */
    if (pEl->ElTerminal || pEl->ElFirstChild == NULL)
      /* l'element n'a pas de fils, on retourne quand meme */
      /* le type de l'element lui-meme */
      *ret = pEl->ElStructSchema->SsRule->SrElem[pEl->ElTypeNumber - 1]->SrName;
    else
      {
        /* on retourne le type du premier ou dernier fils de */
        /* l'element selon le booleen begin */
        pChild = pEl->ElFirstChild;
        if (!begin)
          while (pChild->ElNext != NULL)
            pChild = pChild->ElNext;
        UserElementName (pChild, begin, ret);
      }
}


/*----------------------------------------------------------------------
  TteItemMenuInsert       envoie l'evenement ElemMenu.Pre qui     
  indique que l'editeur va mettre dans le menu TextInserting	
  l'item pour la creation d'un element de type (pSS, typeNum)
  comme frere precedent (si action == InsertAfter) ou suivant
  (si action == InsertBefore) de l'element pEl et retourne	
  la reponse de l'application.				
  ----------------------------------------------------------------------*/
static ThotBool TteItemMenuInsert (PtrSSchema pSS, int typeNum, PtrElement pEl,
                                   PtrDocument pDoc, MenuItemAction action)
{
  NotifyElement       notifyEl;
  PtrElement          pSibling;
  int                 nSiblings;
  ThotBool            ok;

  notifyEl.event = TteElemMenu;
  notifyEl.document = (Document) IdentDocument (pDoc);
  if (action == InsertWithin)
    notifyEl.element = (Element) pEl;
  else
    notifyEl.element = (Element) (pEl->ElParent);
  notifyEl.info = 0; /* not sent by undo */
  notifyEl.elementType.ElTypeNum = typeNum;
  notifyEl.elementType.ElSSchema = (SSchema) (pSS);
  pSibling = pEl;
  nSiblings = 0;
  if (action != InsertWithin)
    {
      while (pSibling->ElPrevious != NULL)
        {
          nSiblings++;
          pSibling = pSibling->ElPrevious;
        }
      if (action == InsertAfter)
        nSiblings++;
    }
  notifyEl.position = nSiblings;
  ok = !CallEventType ((NotifyEvent *) & notifyEl, TRUE);
  return ok;
}


/*----------------------------------------------------------------------
  MenuChoixElem construit dans le buffer menuBuf le menu permettant  
  de choisir le type de l'element defini par la regle de numero   
  rule dans le schema de structure pSS.				
  Retourne le nombre d'entrees du menu construit ou 0 si on n'a   
  pas pu construire un menu.                                      
  Si succes, au retour menuTitle contient le titre a donner au    
  menu.                                                           
  ----------------------------------------------------------------------*/
int MenuChoixElem (PtrSSchema pSS, int rule, PtrElement pEl, char *menuBuf,
                   char *menuTitle, PtrDocument pDoc)
{
  PtrElement          pAncest, pPrevEl;
  int                 i, menuInd, nItems, typeNum;
  ThotBool            ok;
  PtrSRule            pSRule;
  PtrSSchema          pAncSS;
  char               *typeName = NULL;

  NatureChoice = FALSE;
  nItems = 0;
  ChoiceMenuDocument = pDoc;
  /* la regle definissant le type de l'element */
  pSRule = pSS->SsRule->SrElem[rule - 1];
  /* Si ce n'est pas une regle de choix, on ne fait rien */
  if (pSRule->SrConstruct == CsChoice)
    {
      /* determine d'abord le type d'element a creer */
      if (pSRule->SrNChoices == -1)
        /* c'est une regle NATURE */
        {
          /* on activera le formulaire de saisie des noms de nature */
          NatureChoice = TRUE;
          ChoiceMenuSSchema[1] = pEl->ElStructSchema;
        }
      else
        {
          if (pSRule->SrNChoices == 0)
            /* c'est une regle UNIT */
            {
              /* demande a l'utilisateur le type d'element a creer */
              /* cree le menu des noms de type possibles */
              menuInd = 0;
              /* d'abord, la partie fixe: types de base */
              typeNum = CharString + 1;
              if (!ExcludedType (pEl, typeNum, NULL))
                if (TteItemMenuInsert (pSS, typeNum, pEl, pDoc, InsertWithin))
                  {
                    AddChoiceMenuItem (TtaGetMessage (LIB, TMSG_TEXT), &menuInd, menuBuf);
                    ChoiceMenuSSchema[nItems] = pSS;
                    ChoiceMenuTypeNum[nItems] = typeNum;
                    nItems++;
                  }
              typeNum = GraphicElem + 1;
              if (!ExcludedType (pEl, typeNum, NULL))
                if (TteItemMenuInsert (pSS, typeNum, pEl, pDoc, InsertWithin))
                  {
                    AddChoiceMenuItem (TtaGetMessage (LIB, TMSG_GRAPHIC),
                                       &menuInd, menuBuf);
                    ChoiceMenuSSchema[nItems] = pSS;
                    ChoiceMenuTypeNum[nItems] = typeNum;
                    nItems++;
                  }
              typeNum = Symbol + 1;
              if (!ExcludedType (pEl, typeNum, NULL))
                if (TteItemMenuInsert (pSS, typeNum, pEl, pDoc, InsertWithin))
                  {
                    AddChoiceMenuItem (TtaGetMessage (LIB, TMSG_SYMBOL), &menuInd,
                                       menuBuf);
                    ChoiceMenuSSchema[nItems] = pSS;
                    ChoiceMenuTypeNum[nItems] = typeNum;
                    nItems++;
                  }
              typeNum = tt_Picture + 1;
              if (!ExcludedType (pEl, typeNum, NULL))
                if (TteItemMenuInsert (pSS, typeNum, pEl, pDoc, InsertWithin))
                  {
                    AddChoiceMenuItem (TtaGetMessage (LIB, TMSG_PICTURE), &menuInd,
                                       menuBuf);
                    ChoiceMenuSSchema[nItems] = pSS;
                    ChoiceMenuTypeNum[nItems] = typeNum;
                    nItems++;
                  }
              /* partie variable: les unites exportees par les schemas */
              /* de structure des elements englobants */
              pAncest = pEl;
              pPrevEl = NULL;
              while (pAncest != NULL)
                /* teste tous les elements englobants */
                {
                  if (pPrevEl == NULL)
                    ok = TRUE;
                  else
                    ok = pAncest->ElStructSchema != pPrevEl->ElStructSchema;
                  if (ok)
                    /* l'element englobant appartient a un schema different */
                    /* de l'element traite' precedemment */
                    {
                      pAncSS = pAncest->ElStructSchema;
                      /* si c'est une extension de schema, on revient au */
                      /* schema de base pour examiner toutes les extensions */
                      while (pAncSS->SsPrevExtens != NULL)
                        pAncSS = pAncSS->SsPrevExtens;
                      /* cherche les unites definies dans ce schema et dans */
                      /* ses extensions */
                      do
                        {
                          /* cherche les unites definies dans le schema */
                          for (i = 1; i <= pAncSS->SsNRules; i++)
                            if (pAncSS->SsRule->SrElem[i - 1]->SrUnitElem)
                              if (nItems < MAX_MENU)
                                if (!ExcludedType (pEl, i, pAncSS))
                                  /* met l'unite dans le menu */
                                  {
                                    GetExternalTypeName (pAncSS, i, &typeName);
                                    if (TteItemMenuInsert (pAncSS, i, pEl, pDoc, InsertWithin))
                                      {
                                        AddChoiceMenuItem (typeName, &menuInd, menuBuf);
                                        nItems++;
                                        ChoiceMenuSSchema[nItems - 1] = pAncSS;
                                        ChoiceMenuTypeNum[nItems - 1] = i;
                                      }
                                  }
                          /* passe a l'extension de schema suivante */
                          pAncSS = pAncSS->SsNextExtens;
                        }
                      while (pAncSS != NULL);
                    }
                  pPrevEl = pAncest;
                  pAncest = pAncest->ElParent;
                  /* passe a l'element englobant */
                }
              strcpy (menuTitle, TtaGetMessage (LIB, TMSG_EL_TYPE));
            }
          else
            /* c'est un choix avec indication des types possibles */
            /* n cree le menu des noms de type possibles */
            {
              menuInd = 0;
              typeNum = 0;
              while (typeNum < pSRule->SrNChoices)
                {
                  if (nItems < MAX_MENU)
                    if (!TypeHasException (ExcNoCreate, pSRule->SrChoice[typeNum], pSS))
                      /* pas d'exception interdisant a l'utilisateur */
                      /* de creer ce type d'element */
                      if (!ExcludedType (pEl, pSRule->SrChoice[typeNum], pSS))
                        {
                          GetExternalTypeName (pSS, pSRule->SrChoice[typeNum], &typeName);
                          if (TteItemMenuInsert (pSS, pSRule->SrChoice[typeNum],
                                                 pEl, pDoc, InsertWithin))
                            {
                              AddChoiceMenuItem (typeName, &menuInd, menuBuf);
                              nItems++;
                              ChoiceMenuSSchema[nItems - 1] = pSS;
                              ChoiceMenuTypeNum[nItems - 1] = pSRule->SrChoice[typeNum];
                            }
                        }
                  typeNum++;
                }
              strcpy (menuTitle, pSRule->SrName);
            }
        }
    }
  return nItems;
}


/*----------------------------------------------------------------------
  ChoiceMenuCallback      traite le retour du menu choix d'element. 
  item: numero de l'entree choisie dans le menu, ou 0 si aucune   
  entree n'a ete choisie.                                 
  natureName: pointeur sur le nom de la nature choisie, si une	
  nature a ete saisie.					
  ----------------------------------------------------------------------*/
void ChoiceMenuCallback (int item, char *natureName)
{
#ifdef IV
  Name             SSchemaName;

  if (natureName != NULL)
    /* le CsChoice etait une regle NATURE */
    {
      if (natureName[0] != EOS)
        /* l'utilisateur a saisi le nom d'une nature */
        {
          /* on suppose que ce mon est dans la langue de */
          /* l'utilisateur: on le traduit en nom interne */
          ConfigSSchemaInternalName (natureName, SSchemaName, FALSE);
          if (SSchemaName[0] == EOS)
            /* ce nom n'est pas dans le fichier langue, on le */
            /* prend tel quel */
            strncpy (SSchemaName, natureName, MAX_NAME_LENGTH);
          /* cree une nouvelle nature */
          ChosenTypeNum = CreateNature (NULL, SSchemaName, NULL,
                                        ChoiceMenuSSchema[0],
                                        ChoiceMenuDocument);
          ChosenTypeSSchema = ChoiceMenuSSchema[0];
          AddSchemaGuestViews (ChoiceMenuDocument,
                               ChosenTypeSSchema->SsRule->SrElem[ChosenTypeNum - 1]->SrSSchemaNat);
        }
    }
  else
    {
      ChosenTypeNum = ChoiceMenuTypeNum[item - 1];
      ChosenTypeSSchema = ChoiceMenuSSchema[item - 1];
    }
#endif /* IV */
}

/*----------------------------------------------------------------------
  CreeChoix       
  Si l'element pEl est de type choix, demande a l'utilisateur	
  le type d'element qu'il veut creer et cree la descendance	
  de l'element pointe par pEl, selon le choix de l'utilisateur. 
  Retourne dans pLeaf un pointeur sur le premier element cree'.	
  La fonction rend faux si rien n'a ete cree, vrai si au moins un    
  element a ete cree.                                                
  ----------------------------------------------------------------------*/
static ThotBool CreeChoix (PtrDocument pDoc, PtrElement *pEl, PtrElement *pLeaf,
                           ThotBool desc)
{
  PtrElement          pNewEl, pChild, pRet;
  int                 choiceTypeNum;
  char                menuBuf[MAX_TXT_LEN];
  char                menuTitle[MAX_TXT_LEN];
  int                 nItems;
  ThotBool            ret, ok, stop;
  NotifyElement       notifyEl;
  PtrSRule            pSRule;
  ThotBool            menu;

  ret = FALSE;
  pRet = NULL;
  nItems = 0;
  stop = FALSE;
  while (!stop)
    /* boucle sur les creations d'elements */
    {
      ChosenTypeNum = 0;
      choiceTypeNum = (*pEl)->ElTypeNumber;
      /* schema de structure de l'element dont on */
      /* cree le contenu */
      ChosenTypeSSchema = (*pEl)->ElStructSchema;
      *pLeaf = NULL;
      do
        /* boucle pour l'enchainement des choix successifs */
        {
          menu = FALSE;
          pSRule = ChosenTypeSSchema->SsRule->SrElem[choiceTypeNum - 1];
          /* si ce n'est pas une regle de choix, on ne fait rien */
          if (pSRule->SrConstruct == CsChoice)
            {
              if (pSRule->SrNChoices == -1)
                /* c'est une regle NATURE */
                {
                  /* on activera le formulaire de saisie des noms de nature */
                  menu = TRUE;
                  NatureChoice = TRUE;
                  ChoiceMenuSSchema[1] = (*pEl)->ElStructSchema;
                  ChoiceMenuDocument = pDoc;
                }
              else
                {
                  menu = TRUE;
                  nItems = MenuChoixElem (ChosenTypeSSchema, choiceTypeNum,
                                          *pEl, menuBuf, menuTitle, pDoc);
                }
            }
          choiceTypeNum = 0;
          ChosenTypeNum = 0;
          if (menu)
            /* cree et active le menu */
            BuildChoiceMenu (menuBuf, menuTitle, nItems, NatureChoice);
          if (ChosenTypeNum > 0)
            /* verifie que cette nature n'est pas exclue */
            if (ExcludedType (*pEl, ChosenTypeNum, (*pEl)->ElStructSchema))
              ChosenTypeNum = 0;
          if (ChosenTypeNum != 0)
            /* le type de l'element est maintenant connu, on cree l'element */
            {
              notifyEl.event = TteElemNew;
              notifyEl.document = (Document) IdentDocument (pDoc);
              notifyEl.element = (Element) (*pEl);
              notifyEl.info = 0; /* not sent by undo */
              notifyEl.elementType.ElTypeNum = ChosenTypeNum;
              notifyEl.elementType.ElSSchema = (SSchema) ChosenTypeSSchema;
              notifyEl.position = 0;
              pNewEl = NULL;
              if (!CallEventType ((NotifyEvent *) & notifyEl, TRUE))
                pNewEl = NewSubtree (ChosenTypeNum, ChosenTypeSSchema,
                                     pDoc, desc, TRUE, TRUE, TRUE);
              if (pNewEl != NULL)
                {
                  InsertOption (*pEl, &pNewEl, pDoc);
                  *pEl = pNewEl;
                  AttachMandatoryAttributes (*pEl, pDoc);
                  if (pRet == NULL)
                    pRet = *pEl;
                  *pLeaf = FirstLeaf (*pEl);
                }
            }
          /* encore une boucle pour voir si le type choisi est un choix */
          choiceTypeNum = ChosenTypeNum;
        }
      while (ChosenTypeNum != 0 && choiceTypeNum != 0);
      stop = TRUE;
      if (*pLeaf == NULL)
        *pLeaf = *pEl;
      else
        {
          if (desc)
            if ((*pLeaf)->ElTerminal)
              if ((*pLeaf)->ElLeafType == LtReference)
                /* on a cree une reference, on demande l'element */
                /* reference' */
                ret = LinkReference (*pLeaf, NULL, pDoc);
          ret = TRUE;
          /* enchaine les menus de creation de choix */
          /* Si l'element cree' de plus bas niveau est encore un choix */
          /* et qu'on n'a pas cree' de freres, on demande a nouveau a */
          /* l'utilisateur le type de cet element */
          if ((*pLeaf)->ElStructSchema->
              SsRule->SrElem[(*pLeaf)->ElTypeNumber - 1]->SrConstruct == CsChoice)
            /* c'est un choix */
            {
              if (*pEl == *pLeaf)
                ok = TRUE;
              else
                {
                  if ((*pEl)->ElTerminal)
                    pChild = NULL;
                  else
                    pChild = (*pEl)->ElFirstChild;
                  ok = pChild != NULL;
                  /* y a-t-il plus d'un element a chaque niveau ou */
                  /* des elements ont ete cree's ? */
                  while (ok && pChild != NULL)
                    {
                      ok = pChild->ElNext == NULL;
                      if (pChild->ElTerminal)
                        pChild = NULL;
                      else
                        pChild = pChild->ElFirstChild;
                    }
                }
              if (ok)
                /* il n'y a qu'un element a chaque niveau */
                {
                  stop = FALSE;
                  *pEl = *pLeaf;
                }
            }
        }
    }
  if (pRet != NULL)
    /* envoie l'evenement ElemNew.Post */
    NotifySubTree (TteElemNew, pDoc, pRet, 0, 0, FALSE, FALSE);
  *pEl = pRet;
  return ret;
}

/*----------------------------------------------------------------------
  CreateSibling  Cree un element devant (ou derriere, selon before)	
  l'element pointe par pEl dans le document pointe par pDoc.         
  typeNum est le type de l'element a creer et pSS un pointeur sur	
  le schema de structure qui definit ce type.                        
  Si inclusion est vrai, c'est un element inclus de ce type qui est  
  cree', sinon c'est un sous-arbre de ce type qui est cree'.         
  Les paves des elements crees sont crees pour toutes les vues si    
  createAbsBox est Vrai.						
  Retourne un pointeur sur le premier element feuille cree, ou NULL  
  si rien n'a ete creee.                                             
  ----------------------------------------------------------------------*/
PtrElement CreateSibling (PtrDocument pDoc, PtrElement pEl, ThotBool before,
                          ThotBool createAbsBox, int typeNum, PtrSSchema pSS,
                          ThotBool inclusion)
{
  PtrElement          pLeaf, pNew, pElem, pNextEl, pSibling, pRet,
    notifiedElem;
  int                 counterNum, nSiblings;
  PtrPSchema          pPSchema;
  ThotBool            ok, page;
  int                 schView;
  PtrSRule            pSRule;
  NotifyElement       notifyEl;

  pRet = NULL;
  notifiedElem = NULL;
  page = FALSE;
  if (typeNum == PageBreak + 1)
    /* on va creer une marque de page */
    /* on prend pour la page le schema de structure du document, pour */
    /* appliquer les regles de presentation des pages definies pour le */
    /* document, et non les regles par defaut de la nature courante */
    {
      page = TRUE;
      pSS = pDoc->DocDocElement->ElStructSchema;
    }
  if (typeNum != 0 && pSS != NULL)
    {
      pNew = NULL;
      if (pEl != NULL)
        /* cree le sous-arbre, ou l'element seul si c'est une inclusion */
        {
          /* envoie d'abord l'evenement ElemNew.Pre */
          if (inclusion)
            notifyEl.event = TteElemInclude;
          else
            notifyEl.event = TteElemNew;
          notifyEl.document = (Document) IdentDocument (pDoc);
          notifyEl.element = (Element) (pEl->ElParent);
          notifyEl.info = 0; /* not sent by undo */
          notifyEl.elementType.ElTypeNum = typeNum;
          notifyEl.elementType.ElSSchema = (SSchema) pSS;
          pSibling = pEl;
          nSiblings = 0;
          while (pSibling->ElPrevious != NULL)
            {
              nSiblings++;
              pSibling = pSibling->ElPrevious;
            }
          if (!before)
            nSiblings++;
          notifyEl.position = nSiblings;
          if (!CallEventType ((NotifyEvent *) & notifyEl, TRUE))
            {
              pNew = NewSubtree (typeNum, pSS, pDoc, (ThotBool)(!inclusion),
                                 TRUE, TRUE, TRUE);
              if (inclusion)
                /* dans le cas d'une inclusion, si l'element a inclure */
                /* est identique a un element d'une autre nature, c'est */
                /* cet element d'une autre nature qui est inclus. */
                {
                  pSRule = pSS->SsRule->SrElem[typeNum - 1];
                  if (pSRule->SrConstruct == CsIdentity)
                    if (pSS->SsRule->SrElem[pSRule->SrIdentRule - 1]->SrConstruct == CsNatureSchema)
                      {
                        pElem = NewSubtree (pSRule->SrIdentRule, pSS, pDoc,
                                            FALSE, TRUE, TRUE, TRUE);
                        InsertFirstChild (pNew, pElem);
                      }
                }
            }
        }
      if (pNew != NULL)
        {
          if (page)
            pLeaf = pNew;
          else
            /* cherche le premier element feuille cree */
            pLeaf = FirstLeaf (pNew);
          pRet = pNew;
          pSRule = pSS->SsRule->SrElem[typeNum - 1];
          if (inclusion && pRet != NULL)
            /* associe un bloc reference a l'element cree' */
            {
              GetReference (&pLeaf->ElSource);
              if (pLeaf->ElSource != NULL)
                {
                  pLeaf->ElSource->RdElement = pLeaf;
                  pLeaf->ElSource->RdTypeRef = RefInclusion;
                  /* demande a l'utilisateur l'element a inclure */
                  if (!LinkReference (pLeaf, NULL, pDoc))
                    /* si l'utilisateur n'a pas designe' un element
                       correct, on annule */
                    {
                      DeleteElement (&pNew, pDoc);
                      pRet = NULL;
                    }
                }
            }
          if (pRet != NULL)
            /* insere l'element cree dans l'arbre abstrait */
            {
              if (before)
                /* insere devant pEl */
                /* teste si pEl est le premier fils de son pere,
                   abstraction faite des marques de page */
                {
                  pNextEl = SiblingElement (pEl, TRUE);
                  InsertElementBefore (pEl, pNew);
                }
              else
                /* insere apres pEl */
                /* on teste si pEl est le dernier fils de son pere,abstraction */
                /* faite des marques de page */
                {
                  pNextEl = SiblingElement (pEl, FALSE);
                  InsertElementAfter (pEl, pNew);
                }
              /* si c'est un choix, active le menu correspondant et cree le */
              /* type choisi par l'utilisateur. */
              /* Si c'est une reference, demande a l'utilisateur l'element */
              /* designe' par la reference. */
              if (!inclusion)
                {
                  switch (pLeaf->ElStructSchema->SsRule->SrElem[pLeaf->ElTypeNumber - 1]->SrConstruct)
                    {
                    case CsChoice:
                      ok = CreeChoix (pDoc, &pLeaf, &pRet,
                                      (ThotBool)(!inclusion));
                      if (!ok)
                        /* l'utilisateur a abandonne' la creation de cet element */
                        {
                          /* supprime le sous-arbre cree */
                          DeleteElement (&pNew, pDoc);
                          pRet = NULL;
                        }
                      else
                        /* CreeChoix a deja envoye' l'evenement */
                        /* TteElemNew pour l'element pLeaf */
                        notifiedElem = pLeaf;
                      break;
                    case CsReference:
                      LinkReference (pLeaf, NULL, pDoc);
                      break;
                    default:
                      break;
                    }
                }
              if (pNew != NULL)
                /* Traite les exclusions dans l'element cree */
                RemoveExcludedElem (&pNew, pDoc);
              else
                pRet = NULL;
              if (inclusion && pLeaf->ElSource != NULL)
                {
                  /* l'element a inclure est connu, on le copie */
                  if (inclusion)
                    CopyIncludedElem (pLeaf, pDoc);
                  else
                    {
                      ApplyTransmitRules (pLeaf, pDoc);
                      RepApplyTransmitRules (pLeaf, pLeaf, pDoc);
                    }
                }
              if (pNew != NULL)
                {
                  pRet = FirstLeaf (pNew);
                  if (pNew->ElTypeNumber == PageBreak + 1)
                    /* on a cree une marque de page, on l'initialise */
                    {
                      pNew->ElPageType = PgUser;
                      /* numero de la vue active dans son schema de pres. */
                      schView = AppliedView (pNew, NULL, pDoc, SelectedView);
                      /* la nouvelle page concerne la vue active */
                      pNew->ElViewPSchema = schView;
                      /* cherche le compteur de page a appliquer */
                      counterNum = GetPageCounter (pNew, pDoc, schView,
                                                   &pPSchema);
                      if (counterNum == 0)
                        pNew->ElPageNumber = 1;
                      /* page non numerotee */
                      else
                        /* calcule la valeur du compteur */
                        pNew->ElPageNumber = CounterVal (counterNum, pNew->ElStructSchema, pPSchema, pNew, schView, pDoc);
                    }
                  else if (pNextEl == NULL)
                    /* l'element pEl n'est plus le dernier fils de son pere */
                    ChangeFirstLast (pEl, pDoc, before, TRUE);
                  /* garde le pointeur sur le sous-arbre cree */
                  NCreatedElements++;
                  CreatedElement[NCreatedElements - 1] = pNew;
                  /* verifie les attributs requis des elements crees */
                  AttachMandatoryAttributes (pNew, pDoc);
                  if (pDoc->DocSSchema == NULL)
                    /* le document a ete ferme' entre temps */
                    pRet = NULL;
                  else
                    {
                      /* traitement des exceptions */
                      CreationExceptions (pNew, pDoc);
                      /* si CreeChoix a deja envoye' l'evenement TteElemNew */
                      /* pour l'element pNew, il ne faut pas l'envoyer a nouveau */
                      if (pNew != notifiedElem)
                        {
                          if (inclusion)
                            {
                              notifyEl.event = TteElemInclude;
                              notifyEl.document = (Document) IdentDocument (pDoc);
                              notifyEl.element = (Element) pNew;
                              notifyEl.info = 0; /* not sent by undo */
                              notifyEl.elementType.ElTypeNum = pNew->ElTypeNumber;
                              notifyEl.elementType.ElSSchema = (SSchema) (pNew->ElStructSchema);
                              notifyEl.position = 0;
                              CallEventType ((NotifyEvent *) & notifyEl, FALSE);
                            }
                          else
                            NotifySubTree (TteElemNew, pDoc, pNew, 0, 0, FALSE, FALSE);
                        }
                      if (createAbsBox)
                        /* cree les paves du nouvel element et les affiche */
                        CreateAllAbsBoxesOfEl (pNew, pDoc);
                    }
                }
            }
        }
    }
  return pRet;
}

/*----------------------------------------------------------------------
  CreateWithinElement
  Cree un contenu vide pour l'element pEL
  dans le document pointe par pDoc. Les paves des elements crees	
  sont crees pour toutes les vues si createAbsBox est Vrai.          
  Si inclusion est vrai, c'est un element inclus qui est cree'.      
  Retourne un pointeur sur le premier element feuille cree, ou NULL  
  si rien n'a ete cree.                                              
  ----------------------------------------------------------------------*/
PtrElement CreateWithinElement (PtrDocument pDoc, PtrElement pEl,
                                ThotBool createAbsBox, ThotBool inclusion)
{
  PtrElement          p, p1, pLeaf, pPrevEl, pNextEl, pSibling, pL;
  int                 i, j, minNum, nSiblings;
  ThotBool            found, cree;
  PtrSRule            pSRule;
  NotifyElement       notifyEl;

  pLeaf = NULL;
  p = NULL;
  cree = FALSE;
  pSRule = pEl->ElStructSchema->SsRule->SrElem[pEl->ElTypeNumber - 1];
  switch (pSRule->SrConstruct)
    {
    case CsChoice:
      /* c'est une regle de choix */
      if (pEl->ElFirstChild == NULL)
        {
          if (inclusion)
            {
              /* associe un bloc reference a l'element cree' */
              GetReference (&pEl->ElSource);
              if (pEl->ElSource != NULL)
                {
                  pEl->ElSource->RdElement = pEl;
                  pEl->ElSource->RdTypeRef = RefInclusion;
                  /* demande a l'utilisateur l'element a inclure */
                  if (LinkReference (pEl, NULL, pDoc))
                    {
                      /* supprime les anciens paves */
                      TtaClearViewSelections ();
                      /* annule d'abord la selection */
                      DestroyAbsBoxes (pEl, pDoc, FALSE);
                      AbstractImageUpdated (pDoc);
                      /* supprime la copie de l'ancien element inclus */
                      if (!pEl->ElTerminal)
                        {
                          pSibling = pEl->ElFirstChild;
                          while (pSibling != NULL)
                            {
                              pL = pSibling->ElNext;
                              DeleteElement (&pSibling, pDoc);
                              pSibling = pL;
                            }
                        }
                      /* effectue la copie de l'arbre abstrait */
                      /* de l'element inclus */
                      if (inclusion)
                        CopyIncludedElem (pEl, pDoc);
                      pLeaf = FirstLeaf (pEl);
                      CreateAllAbsBoxesOfEl (pEl, pDoc);
                      AbstractImageUpdated (pDoc);
                      RedisplayDocViews (pDoc);
                    }
                }
            }
          else
            {
              cree = CreeChoix (pDoc, &pEl, &pLeaf, TRUE);
              pLeaf = FirstLeaf (pEl);
            }
          if (!cree)
            {
              pLeaf = NULL;
              /*Ret = NULL; */
            }
          else
            /* traitement des exceptions */
            {
              /* traite les exclusions des elements crees */
              RemoveExcludedElem (&pEl, pDoc);
              if (pEl != NULL)
                {
                  /* traite les attributs requis des elements crees */
                  AttachMandatoryAttributes (pEl, pDoc);
                  if (pDoc->DocSSchema == NULL)
                    /* le document a ete ferme' entre temps */
                    pLeaf = NULL;
                  else
                    {
                      CreationExceptions (pEl, pDoc);
                      /* garde le pointeur sur le sous arbre cree */
                      NCreatedElements++;
                      CreatedElement[NCreatedElements - 1] = pEl;
                      if (createAbsBox)
                        /* cree les paves du nouvel element */
                        CreateAllAbsBoxesOfEl (pEl, pDoc);
                    }
                }
            }
        }
      break;

    case CsAggregate:
    case CsUnorderedAggregate:
      /* C'est un agregat, creer les elements absents */
      if (!inclusion)
        {
          pPrevEl = NULL;
          /* premier element existant de l'agregat */
          p = pEl->ElFirstChild;
          for (i = 0; i < pSRule->SrNComponents; i++)
            /* on ne cree pas les composants qui ont l'exception NoCreate */
            if (!TypeHasException (ExcNoCreate, pSRule->SrComponent[i],
                                   pEl->ElStructSchema))
              /* on ne cree pas les composants exclus */
              if (!ExcludedType (pEl, pSRule->SrComponent[i],
                                 pEl->ElStructSchema))
                {
                  /* saute les marques de page */
                  FwdSkipPageBreak (&p);
                  /* cherche si ce composant existe deja */
                  found = FALSE;
                  if (p != NULL)
                    /* il y a au moins un element dans l'agregat */
                    {
                      if (pSRule->SrConstruct == CsAggregate)
                        {
                          /* c'est un agregat ordonne' */
                          if (EquivalentSRules (pSRule->SrComponent[i],
                                                pEl->ElStructSchema,
                                                p->ElTypeNumber,
                                                p->ElStructSchema, pEl))
                            /* le composant cherche' est a sa place */
                            {
                              found = TRUE;
                              pPrevEl = p;
                              p = p->ElNext;
                            }
                        }
                      if (pSRule->SrConstruct == CsUnorderedAggregate)
                        /* agregat sans ordre, cherche parmi tous les */
                        /* elements fils de l'agregat */
                        {
                          p = pEl->ElFirstChild;
                          while (p != NULL && !found)
                            if (EquivalentSRules (pSRule->SrComponent[i],
                                                  pEl->ElStructSchema,
                                                  p->ElTypeNumber,
                                                  p->ElStructSchema, pEl))
                              /* c'est le composant cherche' */
                              {
                                found = TRUE;
                                pPrevEl = p;
                                p = p->ElNext;
                              }
                            else
                              /* on passe au fils suivant */
                              p = p->ElNext;
                          if (!found)
                            p = pEl->ElFirstChild;
                        }
                      if (!found)
                        {
                          if (pPrevEl == NULL && p->ElPrevious != NULL)
                            {
                              if (p->ElPrevious->ElTerminal)
                                if (p->ElPrevious->ElLeafType == LtPageColBreak)
                                  if (p->ElPrevious->ElPageType == PgBegin)
                                    pPrevEl = p->ElPrevious;
                              /* on inserera apres les sauts de pages du debut */
                            }
                        }
                    }
                  else if (pPrevEl == NULL && pEl->ElParent == NULL)
                    /* on est sur l'element racine et il est vide */
                    {
                      p = pEl->ElFirstChild;
                      SkipPageBreakBegin (&p);
                      pPrevEl = p;
                      /* on inserera apres les sauts de pages du debut */
                    }
                  if (!found)
                    /* le composant cherche' n'existe pas */
                    {
                      notifyEl.event = TteElemNew;
                      notifyEl.document = (Document) IdentDocument (pDoc);
                      notifyEl.element = (Element) pEl;
                      notifyEl.info = 0; /* not sent by undo */
                      notifyEl.elementType.ElTypeNum = pSRule->SrComponent[i];
                      notifyEl.elementType.ElSSchema = (SSchema) (pEl->ElStructSchema);
                      nSiblings = 0;
                      if (pPrevEl != NULL)
                        {
                          pSibling = pPrevEl;
                          nSiblings++;
                          while (pSibling->ElPrevious != NULL)
                            {
                              nSiblings++;
                              pSibling = pSibling->ElPrevious;
                            }
                        }
                      notifyEl.position = nSiblings;
                      p1 = NULL;
                      if (!CallEventType ((NotifyEvent *) & notifyEl, TRUE))
                        /* on en cree un avec sa descendance */
                        p1 = NewSubtree (pSRule->SrComponent[i],
                                         pEl->ElStructSchema, pDoc,
                                         TRUE, TRUE, TRUE, TRUE);
                      if (p1 != NULL)
                        /* chaine dans l'arbre l'element cree */
                        {
                          if (pLeaf == NULL)
                            pLeaf = FirstLeaf (p1);
                          if (pPrevEl == NULL)
                            {
                              InsertFirstChild (pEl, p1);
                              pNextEl = SiblingElement (p1, FALSE);
                              if (pNextEl != NULL)
                                /* l'element suivant le nouvel element */
                                /* n'est plus premier */
                                ChangeFirstLast (pNextEl, pDoc, TRUE, TRUE);
                            }
                          else
                            {
                              InsertElementAfter (pPrevEl, p1);
                              pNextEl = SiblingElement (p1, FALSE);
                              if (pNextEl == NULL)
                                /* l'element precedent le nouvel */
                                /* element n'est plus dernier */
                                ChangeFirstLast (pPrevEl, pDoc, FALSE, TRUE);
                            }
                          /* traite les exclusions des elements crees */
                          RemoveExcludedElem (&p1, pDoc);
                          if (p1 != NULL)
                            {
                              /* traite les attributs requis des elements crees */
                              AttachMandatoryAttributes (p1, pDoc);
                              if (pDoc->DocSSchema == NULL)
                                /* le document a ete ferme' entre temps */
                                pLeaf = NULL;
                              else
                                {
                                  pPrevEl = p1;
                                  /* traitement des exceptions */
                                  CreationExceptions (p1, pDoc);
                                  NotifySubTree (TteElemNew, pDoc, p1, 0, 0,
                                                 FALSE, FALSE);
                                  if (createAbsBox)
                                    /* cree les paves du nouvel element */
                                    CreateAllAbsBoxesOfEl (p1, pDoc);
                                  /* garde le pointeur sur le sous arbre cree */
                                  NCreatedElements++;
                                  CreatedElement[NCreatedElements - 1] = p1;
                                }
                            }
                        }
                    }
                }
        }
      break;

    case CsList:
      /* C'est une liste, on cree le nombre minimum d'elem. */
      if (!inclusion)
        if (!TypeHasException (ExcNoCreate, pSRule->SrListItem,
                               pEl->ElStructSchema))
          if (!ExcludedType (pEl, pSRule->SrListItem, pEl->ElStructSchema))
            /* compte les elements existants : j */
            {
              p = pEl->ElFirstChild;
              j = 0;
              p1 = NULL;
              while (p != NULL)
                {
                  /* ignore les marques page */
                  if (p->ElTerminal && p->ElLeafType == LtPageColBreak)
                    /* si c'est une page de debut d'element, on creera les */
                    /* nouveaux elements de liste apres cette marque de page */
                    {
                      if (p->ElPageType == PgBegin)
                        p1 = p;
                    }
                  else
                    {
                      /* ignore les elements qui ne sont pas du type (ou */
                      /* equivalent) prevu par la regle liste. Les */
                      /* inclusions peuvent produire de tels elements */
                      if (EquivalentSRules (pSRule->SrListItem,
                                            pEl->ElStructSchema,
                                            p->ElTypeNumber,
                                            p->ElStructSchema, pEl))
                        j++;
                      p1 = p;
                    }
                  p = p->ElNext;
                }
              if (pSRule->SrMinItems == 0)
                minNum = 1;
              else
                minNum = pSRule->SrMinItems;
              if (j < minNum)
                /* il en manque, on cree ce qui manque */
                /* p1 pointe sur le dernier element existant de la liste */
                for (i = j; i < minNum; i++)
                  {
                    notifyEl.event = TteElemNew;
                    notifyEl.document = (Document) IdentDocument (pDoc);
                    notifyEl.element = (Element) pEl;
                    notifyEl.info = 0; /* not sent by undo */
                    notifyEl.elementType.ElTypeNum = pSRule->SrListItem;
                    notifyEl.elementType.ElSSchema = (SSchema) (pEl->ElStructSchema);
                    nSiblings = 0;
                    if (p1 != NULL)
                      {
                        pSibling = p1;
                        nSiblings++;
                        while (pSibling->ElPrevious != NULL)
                          {
                            nSiblings++;
                            pSibling = pSibling->ElPrevious;
                          }
                      }
                    notifyEl.position = nSiblings;
                    p = NULL;
                    if (!CallEventType ((NotifyEvent *) & notifyEl, TRUE))
                      p = NewSubtree (pSRule->SrListItem, pEl->ElStructSchema,
                                      pDoc, TRUE, TRUE, TRUE, TRUE);
                    if (p != NULL)
                      {
                        if (pLeaf == NULL)
                          pLeaf = FirstLeaf (p);
                        if (p1 == NULL)
                          {
                            InsertFirstChild (pEl, p);
                            pNextEl = SiblingElement(p, FALSE);
                            if (pNextEl != NULL)
                              /* l'element suivant le nouvel element n'est */
                              /* plus premier */
                              ChangeFirstLast (pNextEl, pDoc, TRUE, TRUE);
                          }
                        else
                          {
                            InsertElementAfter (p1, p);
                            pNextEl = SiblingElement(p, FALSE);
                            if (pNextEl == NULL)
                              /* l'element precedent le nouvel element n'est */
                              /* plus dernier */
                              ChangeFirstLast (p1, pDoc, FALSE, TRUE);
                          }
                        /* traite les exclusions des elements crees */
                        RemoveExcludedElem (&p, pDoc);
                        if (p != NULL)
                          {
                            p1 = p;
                            /* traite les attributs requis des elements crees*/
                            AttachMandatoryAttributes (p, pDoc);
                            if (pDoc->DocSSchema == NULL)
                              /* le document a ete ferme' entre temps */
                              pLeaf = NULL;
                            else
                              {
                                /* traitement des exceptions */
                                CreationExceptions (p, pDoc);
                                NotifySubTree (TteElemNew, pDoc, p, 0, 0,
                                               FALSE, FALSE);
                                if (createAbsBox)
                                  /* cree les paves du nouvel element */
                                  CreateAllAbsBoxesOfEl (p, pDoc);
                                /* garde le pointeur sur le sous arbre cree */
                                NCreatedElements++;
                                CreatedElement[NCreatedElements - 1] = p;
                              }
                          }
                      }
                  }
            }
      break;

    case CsReference:
      if (!inclusion)
        /* demande a l'utilisateur l'element reference' */
        {
          LinkReference (pEl, NULL, pDoc);
          /* supprime les anciens paves de la reference */
          pLeaf = FirstLeaf (pEl);
          /* annule d'abord la selection */
          TtaClearViewSelections ();
          DestroyAbsBoxes (pEl, pDoc, FALSE);
          AbstractImageUpdated (pDoc);
          if (createAbsBox)
            /* cree les paves de la valeur de la reference */
            CreateAllAbsBoxesOfEl (pEl, pDoc);
          /* garde le pointeur sur le sous arbre cree */
          NCreatedElements++;
          CreatedElement[NCreatedElements - 1] = pEl;
        }
      break;

    case CsNatureSchema:
      /* ce cas ne doit pas se presenter */
      break;

    case CsIdentity:
      if (!inclusion)
        {
          p = NULL;	/* on ne fait rien s'il y a deja un contenu */
          if (pEl->ElFirstChild == NULL)
            /* on ne fait rien si l'element a l'exception NoCreate */
            if (!TypeHasException (ExcNoCreate, pSRule->SrIdentRule,
                                   pEl->ElStructSchema))
              if (!ExcludedType (pEl, pSRule->SrIdentRule, pEl->ElStructSchema))
                {
                  cree =
                    pEl->ElStructSchema->SsRule->SrElem[pSRule->SrIdentRule - 1]->SrConstruct == CsBasicElement ||
                    pEl->ElStructSchema->SsRule->SrElem[pSRule->SrIdentRule - 1]->SrConstruct == CsConstant ||
                    pEl->ElStructSchema->SsRule->SrElem[pSRule->SrIdentRule - 1]->SrConstruct == CsReference ||
                    pEl->ElStructSchema->SsRule->SrElem[pSRule->SrIdentRule - 1]->SrConstruct == CsChoice;
                  notifyEl.event = TteElemNew;
                  notifyEl.document = (Document) IdentDocument (pDoc);
                  notifyEl.element = (Element) pEl;
                  notifyEl.info = 0; /* not sent by undo */
                  notifyEl.elementType.ElTypeNum = pSRule->SrIdentRule;
                  notifyEl.elementType.ElSSchema = (SSchema) (pEl->ElStructSchema);
                  notifyEl.position = 0;
                  p = NULL;
                  if (!CallEventType ((NotifyEvent *) & notifyEl, TRUE))
                    p = NewSubtree (pSRule->SrIdentRule,
                                    pEl->ElStructSchema, pDoc,
                                    TRUE, cree, TRUE, TRUE);
                }
          if (p != NULL)
            {
              pLeaf = FirstLeaf (p);
              InsertFirstChild (pEl, p);
              /* traite les eclusions des elements crees */
              RemoveExcludedElem (&p, pDoc);
              if (p != NULL)
                {
                  pNextEl = SiblingElement (p, FALSE);
                  if (pNextEl != NULL)
                    /* l'element suivant le nouvel element n'est */
                    /* plus premier */
                    ChangeFirstLast (pNextEl, pDoc, TRUE, TRUE);
                  /* traite les attributs requis des elements crees */
                  AttachMandatoryAttributes (p, pDoc);
                  if (pDoc->DocSSchema == NULL)
                    /* le document a ete ferme' entre temps */
                    pLeaf = NULL;
                  else
                    {
                      /* traitement des exceptions */
                      CreationExceptions (p, pDoc);
                      NotifySubTree (TteElemNew, pDoc, p, 0, 0, FALSE, FALSE);
                      if (createAbsBox)
                        /* cree les paves du nouvel element et de sa descendance */
                        CreateAllAbsBoxesOfEl (pEl, pDoc);
                      /* garde le pointeur sur le sous arbre cree */
                      NCreatedElements++;
                      CreatedElement[NCreatedElements - 1] = p;
                    }
                }
            }
        }
      break;

    case CsBasicElement:
      break;

    default:
      break;
    }
  return pLeaf;
}

/*----------------------------------------------------------------------
  CanCreateWithinElement   Indique si la commande Creer Dedans	
  creerait effectivement des elements a l'interieur de    
  l'element pEl.						
  ----------------------------------------------------------------------*/
static ThotBool CanCreateWithinElement (PtrElement pEl, ThotBool inclusion)
{
  PtrElement          p;
  int                 i, j, minNum;
  ThotBool            result, found;
  PtrSRule            pSRule;

  result = FALSE;
  if (!pEl->ElIsCopy && !ElementIsReadOnly (pEl))
    /* on ne peut rien faire dans une copie protegee ou un sous-arbre en */
    /* lecture seule */
    {
      pSRule = pEl->ElStructSchema->SsRule->SrElem[pEl->ElTypeNumber - 1];
      if (inclusion)
        /* dans le cas d'une inclusion, CreateWithinElement ne traite que les Choix */
        {
          if (pSRule->SrConstruct == CsChoice)
            result = TRUE;
        }
      else
        /* traitement specifique selon le constructeur de l'element */
        switch (pSRule->SrConstruct)
          {
          case CsChoice:
            /* c'est un choix */
            /* on ne cree dans un choix que s'il n'a pas deja un fils */
            if (pEl->ElFirstChild == NULL)
              result = TRUE;
            break;
          case CsAggregate:
          case CsUnorderedAggregate:
            /* C'est un agregat, CreateWithinElement creera les elements absents */
            /* examine tous les composants possibles de l'agregat */
            for (i = 0; i < pSRule->SrNComponents && !result; i++)
              /* on ne cree pas les composants qui ont l'exception NoCreate */
              if (!TypeHasException (ExcNoCreate, pSRule->SrComponent[i],
                                     pEl->ElStructSchema))
                /* on ne cree pas les composants exclus */
                if (!ExcludedType (pEl, pSRule->SrComponent[i], pEl->ElStructSchema))
                  {
                    /* premier element existant de l'agregat */
                    p = pEl->ElFirstChild;
                    /* cherche si le composant possible existe deja */
                    found = FALSE;
                    while (p != NULL && !found)
                      if (EquivalentSRules (pSRule->SrComponent[i], pEl->ElStructSchema, p->ElTypeNumber, p->ElStructSchema, pEl))
                        /* c'est le composant cherche' */
                        {
                          found = TRUE;
                          p = p->ElNext;
                        }
                      else
                        /* on passe au fils suivant */
                        p = p->ElNext;
                    if (!found)
                      /* le composant cherche' n'existe pas, on peut le creer */
                      result = TRUE;
                  }
            break;
          case CsList:
            /* C'est une liste, on verifie le nombre minimum d'elem. */
            if (!TypeHasException (ExcNoCreate, pSRule->SrListItem, pEl->ElStructSchema))
              if (!ExcludedType (pEl, pSRule->SrListItem, pEl->ElStructSchema))
                /* compte les elements existants : j */
                {
                  p = pEl->ElFirstChild;
                  j = 0;
                  while (p != NULL)
                    {
                      /* ignore les marques page */
                      if (!p->ElTerminal || p->ElLeafType != LtPageColBreak)
                        /* ignore les elements qui ne sont pas du type (ou */
                        /* equivalent) prevu par la regle liste. Les */
                        /* inclusions peuvent produire de tels elements */
                        if (EquivalentSRules (pSRule->SrListItem, pEl->ElStructSchema,
                                              p->ElTypeNumber, p->ElStructSchema, pEl))
                          j++;
                      p = p->ElNext;
                    }
                  if (pSRule->SrMinItems == 0)
                    minNum = 1;
                  else
                    minNum = pSRule->SrMinItems;
                  if (j < minNum)
                    /* il en manque */
                    result = TRUE;
                }
            break;
          case CsReference:
            result = TRUE;
            break;
          case CsNatureSchema:
            /* ce cas ne doit pas se presenter */
            break;
          case CsIdentity:
            /* on ne peut rien creer s'il y a deja un contenu */
            if (pEl->ElFirstChild == NULL)
              /* on ne cree rien si l'element a l'exception NoCreate */
              if (!TypeHasException (ExcNoCreate, pSRule->SrIdentRule, pEl->ElStructSchema))
                if (!ExcludedType (pEl, pSRule->SrIdentRule, pEl->ElStructSchema))
                  result = TRUE;
            break;
          case CsBasicElement:
            /* parmi les types de bases, seules les images sont traitees par */
            /* CreateWithinElement */
            if (pSRule->SrBasicType == tt_Picture)
              result = TRUE;
            break;
          case CsPairedElement:
            /* on ne peut rien creer dans un element de paire */
            result = FALSE;
          default:
            break;
          }
    }
  return result;
}


/*----------------------------------------------------------------------
  CreateOrPasteInText Cree (ou collee ou inclut, selon les booleens   
  create et paste) dans l'element de texte ou se trouve la selection 
  courante.                                                          
  S'il s'agit d'une creation, paste est faux.                        
  Si page est vrai, on cree une Marque de Page, sinon on cree        
  l'element prevu par le schema de structure.                        
  Si paste est vrai, page est ignore'.                               
  Retourne un pointeur sur l'element cree' (ou colle'), et dans pFree
  un pointeur sur la chaine des elements a liberer apres affichage.  
  ----------------------------------------------------------------------*/
PtrElement CreateOrPasteInText (ThotBool create, ThotBool paste,
                                ThotBool page, PtrElement *pFree)
{
  ThotBool            isList, optional;
  PtrElement          firstSel, lastSel, pNew, pNextEl, pFollow;
  PtrDocument         pDoc;
  int                 firstChar, lastChar, typeNum;
  PtrSSchema          pSS;

  if (paste && FirstSavedElement == NULL)
    return (NULL);
  *pFree = NULL;
  pNew = NULL;
  GetCurrentSelection (&pDoc, &firstSel, &lastSel, &firstChar, &lastChar);
  /* teste si le dernier selectionne' est le dernier fils de son pere, */
  /* abstraction faite des marques de page */
  pNextEl = SiblingElement (lastSel, FALSE);
  SplitTextElement (lastSel, lastChar, pDoc, TRUE, &pFollow, FALSE);
  if (create)
    {
      if (page)
        {
          typeNum = PageBreak + 1;
          pSS = NULL;
        }
      else
        SRuleForSibling (pDoc, lastSel, FALSE, 1, &typeNum, &pSS, &isList, &optional);
      pNew = CreateSibling (pDoc, lastSel, FALSE, TRUE, typeNum, pSS, FALSE);
    }
  else if (paste)
    pNew = NULL;
  else
    /* inclusion */
    {
      SRuleForSibling (pDoc, lastSel, FALSE, 1, &typeNum, &pSS, &isList, &optional);
      pNew = CreateSibling (pDoc, lastSel, FALSE, TRUE, typeNum, pSS, TRUE);
    }
  if (pNew == NULL)
    /* echec insertion, recolle les 2 parties de texte */
    {
      MergeTextElements (lastSel, pFree, pDoc, TRUE, FALSE);
      DeleteElement (pFree, pDoc);
      *pFree = NULL;
    }
  else
    /* insertion reussie. Construit les paves du texte coupe' en deux */
    BuildAbsBoxSpliText (lastSel, pFollow, pNextEl, pDoc);
  return pNew;
}

/*----------------------------------------------------------------------
  AddInsertMenuItem ajoute une nouvelle entree dans le menu contenu  
  dans le buffer menuBuf. La nouvelle entree est formee de la     
  concatenation des mots word1, word2 et word3 separes par des	
  blancs.								
  Retourne Faux si le menu est sature' et que la nouvelle entree  
  n'a pas pu etre ajoutee.                                        
  ----------------------------------------------------------------------*/
static ThotBool AddInsertMenuItem (const Name word1, const Name word2, const Name word3,
                                   int *prevMenuInd, int *nItems,
                                   int *menuInd, char *menuBuf)
{
  int                 j;
  ThotBool            ret;

  /* si on ne peut pas mettre au moins 10 caracteres, on ne met pas */
  /* l'entree dans le menu */
  if (*menuInd > MAX_TXT_LEN - 10 || *nItems >= MAX_MENU)
    ret = FALSE;
  else
    {
      ret = TRUE;
      *prevMenuInd = *menuInd;
      /* pour le cas ou l'on supprime l'item... */
      /* copie le premier mot */
      j = strlen (word1) + 1;
      if (j >= MAX_TXT_LEN - *menuInd)
        j = MAX_TXT_LEN - *menuInd;
      strncpy (&menuBuf[*menuInd], word1, j);
      *menuInd += j;
      menuBuf[*menuInd - 1] = EOS;

      /* copie le deuxieme mot */
      if (*menuInd < MAX_TXT_LEN - 1 && word2[0] != EOS)
        {
          strcat (&menuBuf[*menuInd - 1], " ");
          j = strlen (word2) + 1;
          if (j >= MAX_TXT_LEN - *menuInd)
            j = MAX_TXT_LEN - *menuInd;
          strncpy (&menuBuf[*menuInd], word2, j);
          *menuInd += j;
          menuBuf[*menuInd - 1] = EOS;
        }

      /* copie le troisieme mot */
      if (*menuInd < MAX_TXT_LEN - 1 && word3[0] != EOS)
        {
          strcat (&menuBuf[*menuInd - 1], " ");
          j = strlen (word3) + 1;
          if (j >= MAX_TXT_LEN - *menuInd)
            j = MAX_TXT_LEN - *menuInd;
          strncpy (&menuBuf[*menuInd], word3, j);
          *menuInd += j;
          menuBuf[*menuInd - 1] = EOS;
        }
      (*nItems)++;
    }
  return ret;
}


/*----------------------------------------------------------------------
  CheckAction       
  Detecte les insertions Avant/Apres dupliquees et les supprime.
  ----------------------------------------------------------------------*/
static void CheckAction (int *prevMenuInd, int *menuInd, int *nItems)
{
  int                 i;
  ThotBool            found;
  char               *typeName1 = NULL, *typeName2 = NULL;

  found = FALSE;
  if (UserAction[*nItems - 1] == InsertAfter ||
      UserAction[*nItems - 1] == InsertBefore)
    {
      i = *nItems - 1;
      while (i > 0 && !found)
        {
          i--;
          if (UserAction[i] == UserAction[*nItems - 1]
              && ElemTypeAction[i] == ElemTypeAction[*nItems - 1]
              && SSchemaAction[i] == SSchemaAction[*nItems - 1])
            {
              UserElementName (ElemAction[i], (ThotBool)(Action[i] == InsertBefore), &typeName1);
              UserElementName (ElemAction[*nItems - 1], (ThotBool)(Action[i] == InsertBefore), &typeName2);
              if (strcmp (typeName1, typeName2) == 0)
                found = TRUE;
            }
        }
    }
  if (found)
    {
      (*nItems)--;
      *menuInd = *prevMenuInd;
    }
}


/*----------------------------------------------------------------------
  AddSeparatorInMenu                                               
  ----------------------------------------------------------------------*/
static void AddSeparatorInMenu (int *prevMenuInd, int *nItems, int *menuInd,
                                char * menuBuf)
{
  int                 nItemsAvant;

  nItemsAvant = *nItems;
  InsertSeparatorInMenu (prevMenuInd, nItems, menuInd, menuBuf);
  if (nItemsAvant != *nItems)
    {
      Action[*nItems - 1] = Separator;
      UserAction[*nItems - 1] = Separator;
    }
}


/*----------------------------------------------------------------------
  AddItemWithinSiblimg ajoute une entree au menu pour la creation    
  (ou le collage) d'un element a l'interieur et  en debut	
  de l'element pEl (si before est 'vrai'), en fin de	
  l'element pEl (si before est 'faux').			
  ----------------------------------------------------------------------*/
static void AddItemWithinSiblimg (PtrElement pEl, ThotBool before, int *menuInd,
                                  int *nItems, int *prevMenuInd,
                                  ThotBool *separatorBefore,
                                  ThotBool *separatorAfter, char *menuBuf,
                                  PtrDocument pDoc)

{
  int                 distance, typeNum;
  PtrSSchema          pSS;
  ThotBool            isList, optional;
  Name                N;
  char               *typeName = NULL;
  MenuItemAction      action;

  typeNum = 0;
  do
    if (pEl->ElTerminal)
      /* c'est une feuille, echec */
      pEl = NULL;
    else if (ElementIsReadOnly (pEl))
      pEl = NULL;
    else
      {
        /* premier fils */
        pEl = pEl->ElFirstChild;
        FwdSkipPageBreak (&pEl);
        if (pEl != NULL)
          {
            if (!before)
              /* cherche le dernier fils */
              {
                while (pEl->ElNext != NULL)
                  pEl = pEl->ElNext;
                BackSkipPageBreak (&pEl);
              }
            if (pEl != NULL)
              SRuleForSibling (pDoc, pEl, before, 1, &typeNum, &pSS, &isList, &optional);
          }
      }
  while (typeNum == 0 && pEl != NULL);
  if (typeNum > 0)
    /* il y a un voisin possible au niveau inferieur */
    if (!TypeHasException (ExcNoCreate, typeNum, pSS))
      /* pas d'exception interdisant a l'utilisateur de creer ce type */
      /* d'element */
      if (!ExcludedType (pEl, typeNum, pSS))
        /* on cree une nouvelle entree dans le menu */
        {
          if (before)
            strncpy (N, TtaGetMessage (LIB, TMSG_AFTER), MAX_NAME_LENGTH);
          else
            strncpy (N, TtaGetMessage (LIB, TMSG_BEFORE), MAX_NAME_LENGTH);
          GetExternalTypeName (pSS, typeNum, &typeName);
          if (*separatorBefore)
            {
              /* un separateur est demande' avant cette entree, on le met */
              AddSeparatorInMenu (prevMenuInd, nItems, menuInd, menuBuf);
              *separatorBefore = FALSE;
            }
          if (before)
            action = InsertBefore;
          else
            action = InsertAfter;
          /* envoie l'evenement item a creer */
          if (TteItemMenuInsert (pSS, typeNum, pEl, pDoc, action))
            if (AddInsertMenuItem (typeName, N, "", prevMenuInd, nItems,
                                   menuInd, menuBuf))
              {
                if (before)
                  {
                    Action[*nItems - 1] = InsertBefore;
                    UserAction[*nItems - 1] = InsertAfter;
                  }
                else
                  {
                    Action[*nItems - 1] = InsertAfter;
                    UserAction[*nItems - 1] = InsertBefore;
                  }
                ElemAction[*nItems - 1] = pEl;
                ElemTypeAction[*nItems - 1] = typeNum;
                SSchemaAction[*nItems - 1] = pSS;
                *separatorAfter = TRUE;
                CheckAction (prevMenuInd, menuInd, nItems);
                /* si le voisin possible est un composant optionnel */
                /* d'agregat, ajoute des entrees pour les voisins */
                /* possibles suivants */
                if (!isList && optional)
                  {
                    distance = 1;
                    if (before)
                      strncpy (N, TtaGetMessage (LIB, TMSG_AFTER), MAX_NAME_LENGTH);
                    else
                      strncpy (N, TtaGetMessage (LIB, TMSG_BEFORE), MAX_NAME_LENGTH);
                    do
                      /* boucle sur les voisins suivants */
                      {
                        distance++;
                        SRuleForSibling (pDoc, pEl, before, distance, &typeNum, &pSS, &isList, &optional);
                        if (typeNum > 0)
                          if (!TypeHasException (ExcNoCreate, typeNum, pSS))
                            /* pas d'exception interdisant a l'utilisateur */
                            /* de creer ce type d'element */
                            if (!ExcludedType (pEl, typeNum, pSS))
                              {
                                /* envoie l'evenement item a creer */
                                if (before)
                                  action = InsertBefore;
                                else
                                  action = InsertAfter;
                                if (TteItemMenuInsert (pSS, typeNum, pEl, pDoc, action))
                                  {
                                    GetExternalTypeName (pSS, typeNum, &typeName);

                                    if (!AddInsertMenuItem (typeName, N, "",
                                                            prevMenuInd, nItems,
                                                            menuInd, menuBuf))
                                      /* menu sature' */
                                      optional = FALSE;
                                    else
                                      {
                                        if (before)
                                          {
                                            Action[*nItems - 1] = InsertBefore;
                                            UserAction[*nItems - 1] = InsertAfter;
                                          }
                                        else
                                          {
                                            Action[*nItems - 1] = InsertAfter;
                                            UserAction[*nItems - 1] = InsertBefore;
                                          }
                                        ElemAction[*nItems - 1] = pEl;
                                        ElemTypeAction[*nItems - 1] = typeNum;
                                        SSchemaAction[*nItems - 1] = pSS;
                                        CheckAction (prevMenuInd, menuInd, nItems);
                                      }
                                  }
                              }
                      }
                    while (!isList && optional && typeNum != 0);
                  }
              }
        }
}

#ifdef IV
/*----------------------------------------------------------------------
  PageBreakSiblingAllowed indique s'il est possible de creer un	
  saut de page comme frere de l'element pEl appartenant au	
  document pDoc.							
  ----------------------------------------------------------------------*/
static ThotBool PageBreakSiblingAllowed (PtrElement pEl, PtrDocument pDoc)
{
  ThotBool            allowed, withPages;
  PtrElement          pE;
  PtrAbstractBox      pAb;
  PtrPSchema          pPS;

  allowed = FALSE;
  if (pEl->ElParent != NULL)
    /* l'element a un pere (ce n'est pas une racine) */
    if (!ElementIsReadOnly (pEl->ElParent))
      /* le pere n'est pas en Read-Only */
      {
        /* verifie si le schema de presentation definit des pages pour */
        /* la vue selectionnee */
        pPS = PresentationSchema (pDoc->DocSSchema, pDoc);
        withPages = pPS->PsPaginatedView[pDoc->DocView[SelectedView - 1].
                                         DvPSchemaView - 1];
        if (withPages)
          /* le schema de presentation du document definit bien */
          /* des pages pour la vue ou` l'utilisateur a selectionne' */
          {
            allowed = TRUE;
            /* verifie si un ascendant a la regle PageBreak: No */
            pE = pEl->ElParent;
            while (pE != NULL && allowed)
              {
                pAb = pE->ElAbstractBox[SelectedView - 1];
                if (pAb && !pAb->AbAcceptPageBreak)
                  allowed = FALSE;
                else
                  pE = pE->ElParent;
              }
          }
      }
  return allowed;
}
#endif

/*----------------------------------------------------------------------
  CreationExceptions						
  ----------------------------------------------------------------------*/
void CreationExceptions (PtrElement pEl, PtrDocument pDoc)
{
  ThotBool            b;

  if (pEl->ElTypeNumber == PageBreak + 1)
    InsertPageInTable (pEl, pDoc, 0, &b);
}

#ifdef IV
/*----------------------------------------------------------------------
  CreateInsertPageMenu cree le menu d'insertion des pages en fonction de     
  la selection courante.                                             
  ----------------------------------------------------------------------*/
void CreateInsertPageMenu ()
{
  PtrElement          firstSel, lastSel;
  PtrDocument         pDoc;
  int                 firstChar, lastChar, nItems, prevMenuInd, menuInd;
  Name                titre;
  char                menuBuf[MAX_TXT_LEN];
  char               *typeName = NULL;

  menuInd = 0;
  prevMenuInd = 0;
  nItems = 0;
  /* y-a-t'il une selection courante ? */
  if (!GetCurrentSelection (&pDoc, &firstSel, &lastSel, &firstChar, &lastChar))
    /* non, message 'Selectionnez' */
    return;
  else if (!pDoc->DocReadOnly)
    /* construit le menu d'insertion ou de collage */
    {
      /* entree suivante : 'Page avant premier selectionne' sauf si */
      /* on est dans un element protege' */
      if (PageBreakSiblingAllowed (firstSel, pDoc))
        /* envoie l'evenement item a creer */
        if (TteItemMenuInsert ((pDoc)->DocSSchema, PageBreak + 1,
                               firstSel, pDoc, InsertBefore))
          {
            UserElementName (firstSel, TRUE, &typeName);
            if (AddInsertMenuItem (TtaGetMessage (LIB, TMSG_PAGE_BRK),
                                   TtaGetMessage (LIB, TMSG_BEFORE), typeName, &prevMenuInd,
                                   &nItems, &menuInd, menuBuf))
              {
                Action[nItems - 1] = InsertBefore;
                UserAction[nItems - 1] = InsertBefore;
                ElemAction[nItems - 1] = firstSel;
                ElemTypeAction[nItems - 1] = PageBreak + 1;
                SSchemaAction[nItems - 1] = NULL;
              }
          }

      /* entree suivante : 'Page apres premier selectionne' sauf si on est */
      /* dans un element protege' */
      if (PageBreakSiblingAllowed (lastSel, pDoc))
        /* envoie l'evenement item a creer */
        if (TteItemMenuInsert ((pDoc)->DocSSchema, PageBreak + 1,
                               lastSel, pDoc, InsertAfter))
          {
            UserElementName (lastSel, FALSE, &typeName);
            if (AddInsertMenuItem (TtaGetMessage (LIB, TMSG_PAGE_BRK),
                                   TtaGetMessage (LIB, TMSG_AFTER), typeName, &prevMenuInd,
                                   &nItems, &menuInd, menuBuf))
              {
                Action[nItems - 1] = InsertAfter;
                UserAction[nItems - 1] = InsertAfter;
                ElemAction[nItems - 1] = lastSel;
                ElemTypeAction[nItems - 1] = PageBreak + 1;
                SSchemaAction[nItems - 1] = NULL;
              }
          }

      strncpy (titre, TtaGetMessage (LIB, TMSG_INSERT), MAX_NAME_LENGTH);
      if (nItems > 0)
        BuildPasteMenu (NumMenuInsert, menuBuf, titre, nItems, 'L');
    }
}
#endif

/*----------------------------------------------------------------------
  CreatePasteIncludeCmd    traite les commandes INSERT, PASTE et INCLUDE.     
  create est vrai s'il faut creer de nouveaux elements (appel par	
  la commande INSERT).                                            
  Si 'create' est faux, il faut coller les elements sauve's (appel
  par la commande PASTE) si 'paste' est vrai, ou inclure une      
  copie d'element si 'paste' est faux.                            
  Au retour, ret= vrai si le travail a ete fait, faux sinon.	
  ----------------------------------------------------------------------*/
void CreatePasteIncludeCmd (ThotBool create, ThotBool paste, char button,
                            ThotBool *ret)
{
  PtrElement          firstSel, lastSel, pEl, pSibling, pAncest;
  PtrDocument         pDoc;
  int                 firstChar, lastChar, menuRef, nItems, prevMenuInd,
    menuInd, i, distance, typeNum;
  Name                menuTitle;
  char                menuBuf[MAX_TXT_LEN];
  ThotBool            isList, optional, ok;
  PtrSRule            pSRule, pParentSRule;
  PtrSSchema          pSS, pAncestSS, pSSExt;
  Name                N;
  char               *typeName1 = NULL, *typeName2 = NULL;
  ThotBool            separatorAfter, separatorBefore, protectedElem;

  createPasteMenuOK = FALSE;
  /* y-a-t'il une selection ? */
  if (!GetCurrentSelection (&pDoc, &firstSel, &lastSel, &firstChar, &lastChar))
    return;
  else if (!pDoc->DocReadOnly)
    /* construit le menu d'insertion ou de collage */
    {
      if (create)
        menuRef = NumMenuInsert;
      else if (paste)
        menuRef = NumMenuPaste;
      else
        menuRef = NumMenuInclude;
      nItems = 0;
      menuInd = 0;
      prevMenuInd = 0;
      separatorAfter = FALSE;
      separatorBefore = FALSE;

      /* premiere entree du menu : 'Dans selection' si la selection */
      /* comporte au moins un element pour lequel la commande CreateWithinElement */
      /* creera quelque chose */
      pEl = firstSel;
      ok = FALSE;
      while (pEl != NULL && !ok)
        {
          ok = CanCreateWithinElement (pEl, (ThotBool)(!create && !paste));
          if (!ok)
            pEl = NextInSelection (pEl, lastSel);
        }

      if (ok)
        if (AddInsertMenuItem (TtaGetMessage (LIB, TMSG_WITHIN_SEL),
                               "", "", &prevMenuInd, &nItems, &menuInd, menuBuf))
          {
            Action[nItems - 1] = InsertWithin;
            UserAction[nItems - 1] = InsertWithin;
            ElemAction[nItems - 1] = firstSel;
            /* il faudra mettre un separateur apres cette entree s'il */
            /* y a d'autres entrees apres */
            separatorAfter = TRUE;
          }

      /* la prochaine entree (s'il y en a une) devra etre precedee d'un */
      /* separateur si les entrees precedentes l'ont demande' */
      separatorBefore = separatorAfter;
      separatorAfter = FALSE;
      /* entrees suivantes : inclusions avant 1er selectionne' */
      protectedElem = FALSE;
      if (firstSel->ElTypeNumber == (CharString + 1) && firstChar > 1)
        /* la selection commence a l'interieur d'une chaine de */
        /* caracteres, on ne peut pas inserer si la chaine est protegee */
        protectedElem = ElementIsReadOnly (firstSel);
      else
        /* la selection commence au debut de l'element */
        protectedElem = CannotInsertNearElement (firstSel,
                                                 TRUE); /* Before eleement */

      if (!protectedElem)
        /* l'element n'est pas protege' */
        /* examine les inclusions de tous les ascendants */
        pEl = firstSel->ElParent;
      while (pEl != NULL)
        {
          /* regle de structure de l'ascendant courant */
          pAncestSS = pEl->ElStructSchema;
          pSRule = pAncestSS->SsRule->SrElem[pEl->ElTypeNumber - 1];
          pSSExt = NULL;
          /* on n'a pas encore traite' les inclusions de schema */
          /* parcourt la liste des inclusions definies dans la regle */
          do
            {
              if (pSRule != NULL)
                for (i = 0; i < pSRule->SrNInclusions; i++)
                  {
                    if (!ExcludedType (firstSel->ElParent, pSRule->SrInclusion[i], pAncestSS))
                      /* cette inclusion n'est pas une exclusion */
                      if (!TypeHasException (ExcNoCreate, pSRule->SrInclusion[i],
                                             pAncestSS))
                        /* pas d'exception interdisant a l'utilisateur de creer ce */
                        /* type d'element */
                        {
                          ok = TRUE;
                          /* on ne propose les elements de paire que pour une creation */
                          /* et si la selection n'est pas vide */
                          if (pAncestSS->SsRule->SrElem[pSRule->SrInclusion[i] - 1]->
                              SrConstruct == CsPairedElement)
                            {
                              if (!create)
                                ok = FALSE;	/* c'est pour la commande Coller ou Inclure */
                              else if (firstSel == lastSel &&
                                       firstSel->ElTerminal &&
                                       firstSel->ElLeafType == LtText &&
                                       firstSel->ElTextLength < firstChar)
                                ok = FALSE;	/* selection vide */
                            }
                          if (ok)
                            /* on ajoute une entree au menu */
                            {
                              GetExternalTypeName (pAncestSS, pSRule->SrInclusion[i], &typeName1);
                              pAncest = firstSel;
                              /* on ne peut pas inserer comme descendant direct */
                              /* d'un noeud CsChoice ou CsIdentity' */
                              if (pAncest->ElParent != NULL)
                                {
                                  pParentSRule = pAncest->ElParent->ElStructSchema->SsRule->SrElem[pAncest->ElParent->ElTypeNumber - 1];
                                  if (pParentSRule->SrConstruct == CsChoice)
                                    /* le pere est un choix, on inserera a son niveau */
                                    pAncest = pAncest->ElParent;
                                  else if (pParentSRule->SrConstruct == CsIdentity)
                                    if (EquivalentSRules (pAncest->ElTypeNumber, pAncest->ElStructSchema,
                                                          pAncest->ElParent->ElTypeNumber, pAncest->ElParent->ElStructSchema, pAncest))
                                      /* l'element et son pere sont de type equivalent */
                                      pAncest = pAncest->ElParent;
                                }
                              UserElementName (pAncest, TRUE, &typeName2);
                              if (separatorBefore)
                                {
                                  /* un separateur est demande' avant cette entree, on le met */
                                  AddSeparatorInMenu (&prevMenuInd, &nItems, &menuInd, menuBuf);
                                  separatorBefore = FALSE;
                                }
                              if (pAncestSS->SsRule->SrElem[pSRule->SrInclusion[i] - 1]->
                                  SrConstruct == CsPairedElement)
                                /* une paire de marques autour de la selection */
                                {
                                  strncpy (N, TtaGetMessage (LIB, TMSG_AROUND), MAX_NAME_LENGTH);
                                  typeName2 = NULL;
                                  ok = TteItemMenuInsert (pAncestSS, pSRule->SrInclusion[i] + 1, lastSel, pDoc, InsertAfter);
                                }
                              else
                                strncpy (N, TtaGetMessage (LIB, TMSG_BEFORE), MAX_NAME_LENGTH);
                              if (ok)
                                /* envoie l'evenement item a creer */
                                if (TteItemMenuInsert (pAncestSS, pSRule->SrInclusion[i],
                                                       pAncest, pDoc, InsertBefore))
                                  if (AddInsertMenuItem (typeName1, N, typeName2, &prevMenuInd,
                                                         &nItems, &menuInd, menuBuf))
                                    {
                                      Action[nItems - 1] = InsertBefore;
                                      UserAction[nItems - 1] = InsertBefore;
                                      ElemAction[nItems - 1] = pAncest;
                                      ElemTypeAction[nItems - 1] = pSRule->SrInclusion[i];
                                      SSchemaAction[nItems - 1] = pAncestSS;
                                      CheckAction (&prevMenuInd, &menuInd, &nItems);
                                      separatorAfter = TRUE;
                                    }
                            }
                        }
                  }
              /* passe au schema d'extension suivant */
              if (pSSExt == NULL)
                pSSExt = (pDoc)->DocSSchema->SsNextExtens;
              else
                pSSExt = pSSExt->SsNextExtens;
              if (pSSExt != NULL)
                {
                  pAncestSS = pSSExt;
                  pSRule = ExtensionRule (pEl->ElStructSchema, pEl->ElTypeNumber, pSSExt);
                }
            }
          while (pSSExt != NULL);
          /* passe a l'ascendant */
          pEl = pEl->ElParent;
        }

      /* entrees suivantes : 'Avant premier selectionne' et */
      /* 'Avant englobant', sur InsertionLevels niveaux maximum */
      pEl = firstSel;
      /* boucle sur les niveaux */
      i = 0;
      do
        /* Y a-t-il un voisin direct possible pour l'element pEl ? */
        {
          SRuleForSibling (pDoc, pEl, TRUE, 1, &typeNum, &pSS, &isList, &optional);
          if (typeNum > 0)
            /* il y a un voisin possible */
            {
              if (!TypeHasException (ExcNoCreate, typeNum, pSS))
                /* pas d'exception interdisant a l'utilisateur de creer ce */
                /* type d'element */
                if (!ExcludedType (pEl->ElParent, typeNum, pSS))
                  /* si inclusion, on ne propose pas les references */
                  if (create || paste || pSS->SsRule->SrElem[typeNum - 1]->SrConstruct != CsReference)
                    {
                      ok = TRUE;
                      if (pEl == firstSel && pEl->ElTypeNumber == (CharString + 1) && firstChar > 1)
                        /* la selection commence a l'interieur d'une chaine de */
                        /* caracteres, on ne peut pas inserer si elle est protegee */
                        ok = !ElementIsReadOnly (firstSel);
                      else
                        /* Before element. */
                        ok = !CannotInsertNearElement (pEl, TRUE);
                      if (ok &&
                          /* on ne propose les elements de paire que pour une */
                          /* creation et si la selection n'est pas vide */
                          pSS->SsRule->SrElem[typeNum - 1]->SrConstruct == CsPairedElement)
                        {
                          if (!create)
                            ok = FALSE;	/* c'est pour la commande Coller ou Inclure */
                          else if (firstSel == lastSel &&
                                   firstSel->ElTerminal &&
                                   firstSel->ElLeafType == LtText &&
                                   firstSel->ElTextLength < firstChar)
                            ok = FALSE;	/* selection vide */
                        }
                      if (ok)
                        /* ajoute une entree pour ce voisin */
                        {
                          GetExternalTypeName (pSS, typeNum, &typeName1);
                          UserElementName (pEl, TRUE, &typeName2);
                          if (separatorBefore)
                            {
                              /* un separateur est demande' avant cette entree, on le met */
                              AddSeparatorInMenu (&prevMenuInd, &nItems, &menuInd, menuBuf);
                              separatorBefore = FALSE;
                            }
                          if (pSS->SsRule->SrElem[typeNum - 1]->SrConstruct == CsPairedElement)
                            /* une paire de marques autour de la selection */
                            {
                              strncpy (N, TtaGetMessage (LIB, TMSG_AROUND), MAX_NAME_LENGTH);
                              typeName2 = NULL;
                              ok = TteItemMenuInsert (pSS, typeNum + 1, lastSel, pDoc, InsertAfter);
                            }
                          else
                            strncpy (N, TtaGetMessage (LIB, TMSG_BEFORE), MAX_NAME_LENGTH);
                          if (ok &&
                              /* envoie l'evenement item a creer */
                              TteItemMenuInsert (pSS, typeNum, pEl, pDoc, InsertBefore))
                            {
                              if (!AddInsertMenuItem (typeName1, N, typeName2, &prevMenuInd,
                                                      &nItems, &menuInd, menuBuf))
                                /* menu sature' */
                                pEl = NULL;
                              else
                                {
                                  Action[nItems - 1] = InsertBefore;
                                  UserAction[nItems - 1] = InsertBefore;
                                  ElemAction[nItems - 1] = pEl;
                                  ElemTypeAction[nItems - 1] = typeNum;
                                  SSchemaAction[nItems - 1] = pSS;
                                  separatorAfter = TRUE;
                                }
                            }
                        }
                    }

              /* si le voisin possible est un composant d'agregat, ajoute */
              /* des entrees pour les voisins possibles suivants */
              if (!isList)
                {
                  distance = 1;
                  do
                    /* boucle sur les voisins suivants */
                    {
                      distance++;
                      SRuleForSibling (pDoc, pEl, TRUE, distance, &typeNum, &pSS, &isList, &optional);
                      if (typeNum > 0 &&
                          !TypeHasException (ExcNoCreate, typeNum, pSS) &&
                          /* pas d'exception interdisant a l'utilisateur */
                          /* de creer ce type d'element */
                          /* si inclusion, on ne propose pas les ref. */
                          !ExcludedType (pEl->ElParent, typeNum, pSS) &&
                          (create || paste ||
                           pSS->SsRule->SrElem[typeNum - 1]->SrConstruct != CsReference))
                        {
                          GetExternalTypeName (pSS, typeNum, &typeName1);
                          UserElementName (pEl, TRUE, &typeName2);
                          /* envoie l'evenement item a creer */
                          if (TteItemMenuInsert (pSS, typeNum, pEl, pDoc, InsertBefore))
                            {
                              if (!AddInsertMenuItem (typeName1,
                                                      TtaGetMessage (LIB, TMSG_BEFORE),
                                                      typeName2, &prevMenuInd, &nItems,
                                                      &menuInd, menuBuf))
                                /* menu sature' */
                                {
                                  pEl = NULL;
                                  optional = FALSE;
                                }
                              else
                                {
                                  Action[nItems - 1] = InsertBefore;
                                  UserAction[nItems - 1] = InsertBefore;
                                  ElemAction[nItems - 1] = pEl;
                                  ElemTypeAction[nItems - 1] = typeNum;
                                  SSchemaAction[nItems - 1] = pSS;
                                  CheckAction (&prevMenuInd, &menuInd, &nItems);
                                }
                            }
                        }
                    }
                  while (!isList && pEl != NULL && typeNum != 0);
                }
              /* passe au niveau superieur */
              i++;
            }
          if (firstChar > 1 || pEl == NULL)
            /* la selection commence a l'interieur d'une feuille de */
            /* texte, on ne creera pas aux niveaux superieurs */
            pEl = NULL;
          else
            /* saute les marques de pages precedentes */
            {
              pSibling = pEl->ElPrevious;
              BackSkipPageBreak (&pSibling);
              if (pSibling == NULL)
                /* pas de predecesseur, essaie d'ajouter une entree */
                /* pour creer un predecesseur de l'englobant */
                pEl = pEl->ElParent;
              else
                {
                  if (typeNum == 0)
                    /* il y a un predecesseur et on ne peut pas creer */
                    /* de voisin, essaie d'ajouter une entree pour */
                    /* creer un element a la fin du predecesseur */
                    AddItemWithinSiblimg (pSibling, FALSE, &menuInd, &nItems,
                                          &prevMenuInd, &separatorBefore, &separatorAfter, menuBuf, pDoc);
                  pEl = NULL;
                }
            }
        }
      while (i < InsertionLevels && pEl != NULL);

      /* entrees suivantes : inclusions apres dernier selectionne' */
      separatorBefore = separatorAfter;
      separatorAfter = FALSE;
      protectedElem = FALSE;
      if ((lastSel)->ElTypeNumber == (CharString + 1) && lastChar <= (lastSel)->ElTextLength)
        /* la selection finit a l'interieur d'une chaine de caracteres, */
        /* on ne peut pas inserer si la chaine est protegee */
        protectedElem = ElementIsReadOnly (lastSel);
      else
        /* la selection finit a la fin de l'element */
        protectedElem = CannotInsertNearElement (lastSel,
                                                 FALSE); /* After element */
      if (!protectedElem)
        /* l'element n'est pas protege' */
        /* examine les inclusions de tous les ascendants */
        pEl = lastSel->ElParent;
      while (pEl != NULL)	/* boucle sur les ascendants */
        {
          /* regle de structuration de l'ascendant courant */
          pAncestSS = pEl->ElStructSchema;
          pSRule = pAncestSS->SsRule->SrElem[pEl->ElTypeNumber - 1];
          pSSExt = NULL;	/* on n'a pas encore traite' les extensions de schema */
          /* parcourt la liste de ses extensions definies dans la regle */
          do
            {
              if (pSRule != NULL)
                for (i = 0; i < pSRule->SrNInclusions; i++)
                  {
                    if (!ExcludedType (lastSel->ElParent, pSRule->SrInclusion[i], pAncestSS))
                      /* cette extension n'est pas une exclusion */
                      if (!TypeHasException (ExcNoCreate, pSRule->SrInclusion[i],
                                             pAncestSS)
                          /* pas d'exception interdisant a l'utilisateur de creer ce */
                          /* type d'element, on ajoute une entree au menu */
                          && pAncestSS->SsRule->SrElem[pSRule->SrInclusion[i] - 1]->
                          SrConstruct != CsPairedElement)
                        /* pas d'insertion d'une paire APRES */
                        {
                          GetExternalTypeName (pAncestSS, pSRule->SrInclusion[i], &typeName1);
                          pAncest = lastSel;
                          /* on ne peut pas inserer comme descendant direct */
                          /* d'un noeud CsChoice ou CsIdentity' */
                          if (pAncest->ElParent != NULL)
                            {
                              pParentSRule = pAncest->ElParent->ElStructSchema->SsRule->SrElem[pAncest->ElParent->ElTypeNumber - 1];
                              if (pParentSRule->SrConstruct == CsChoice)
                                /* le pere est un choix, on inserera a son niveau */
                                pAncest = pAncest->ElParent;
                              else if (pParentSRule->SrConstruct == CsIdentity)
                                if (EquivalentSRules (pAncest->ElTypeNumber, pAncest->ElStructSchema,
                                                      pAncest->ElParent->ElTypeNumber, pAncest->ElParent->ElStructSchema, pAncest))
                                  /* l'element et son pere sont de type equivalent */
                                  pAncest = pAncest->ElParent;
                            }
                          UserElementName (pAncest, FALSE, &typeName2);
                          if (separatorBefore)
                            {
                              /* un separateur est demande' avant cette entree, on le met */
                              AddSeparatorInMenu (&prevMenuInd, &nItems, &menuInd, menuBuf);
                              separatorBefore = FALSE;
                            }
                          /* envoie l'evenement item a creer */
                          if (TteItemMenuInsert (pAncestSS, pSRule->SrInclusion[i],
                                                 pAncest, pDoc, InsertAfter))
                            if (AddInsertMenuItem (typeName1, TtaGetMessage (LIB, TMSG_AFTER), typeName2,
                                                   &prevMenuInd, &nItems, &menuInd, menuBuf))
                              {
                                Action[nItems - 1] = InsertAfter;
                                UserAction[nItems - 1] = InsertAfter;
                                ElemAction[nItems - 1] = pAncest;
                                ElemTypeAction[nItems - 1] = pSRule->SrInclusion[i];
                                SSchemaAction[nItems - 1] = pAncestSS;
                                separatorAfter = TRUE;
                                CheckAction (&prevMenuInd, &menuInd, &nItems);
                              }
                        }
                  }
              /* passe au schema d'extension suivant */
              if (pSSExt == NULL)
                pSSExt = (pDoc)->DocSSchema->SsNextExtens;
              else
                pSSExt = pSSExt->SsNextExtens;
              if (pSSExt != NULL)
                {
                  pAncestSS = pSSExt;
                  pSRule = ExtensionRule (pEl->ElStructSchema, pEl->ElTypeNumber, pSSExt);
                }
            }
          while (pSSExt != NULL);
          /* passe a l'ascendant */
          pEl = pEl->ElParent;
        }

      /* entrees suivantes : 'Apres dernier selectionne' et */
      /* 'Apres englobant', sur InsertionLevels niveaux maximum */
      pEl = lastSel;
      /* boucle sur les niveaux successifs */
      i = 0;
      do
        /* Y a-t-il un voisin possible pour l'element ? */
        {
          SRuleForSibling (pDoc, pEl, FALSE, 1, &typeNum, &pSS, &isList, &optional);
          if (typeNum > 0)
            /* il y a un voisin possible */
            {
              if (!TypeHasException (ExcNoCreate, typeNum, pSS))
                /* pas d'exception interdisant a l'utilisateur de creer */
                /* ce type d'element */
                if (!ExcludedType (pEl->ElParent, typeNum, pSS))
                  {
                    protectedElem = FALSE;
                    if (pEl == lastSel && pEl->ElTypeNumber == (CharString + 1) &&
                        lastChar <= (lastSel)->ElTextLength)
                      /* la selection finit a l'interieur d'une chaine de car., */
                      /* on ne peut pas inserer si la chaine est protegee */
                      protectedElem = ElementIsReadOnly (lastSel);
                    else
                      /* la selection finit a la fin de l'element */
                      protectedElem = CannotInsertNearElement (pEl,
                                                               FALSE); /* After element */
                    if (!protectedElem)
                      /* l'element n'est pas protege' */
                      /* si inclusion, on ne propose pas les references */
                      /* on ne propose pas non plus l'insertion d'une paire APRES */
                      if ((create || paste || pSS->SsRule->SrElem[typeNum - 1]->SrConstruct != CsReference)
                          && pSS->SsRule->SrElem[typeNum - 1]->SrConstruct != CsPairedElement)
                        {
                          GetExternalTypeName (pSS, typeNum, &typeName1);
                          UserElementName (pEl, FALSE, &typeName2);
                          if (separatorBefore)
                            {
                              /* un separateur est demande' avant cette entree, on le met */
                              AddSeparatorInMenu (&prevMenuInd, &nItems, &menuInd, menuBuf);
                              separatorBefore = FALSE;
                            }
                          /* envoie l'evenement item a creer */
                          if (TteItemMenuInsert (pSS, typeNum, pEl, pDoc, InsertAfter))
                            {
                              if (!AddInsertMenuItem (typeName1, TtaGetMessage (LIB, TMSG_AFTER), typeName2,
                                                      &prevMenuInd, &nItems, &menuInd, menuBuf))
                                /* menu sature' */
                                pEl = NULL;
                              else
                                {
                                  Action[nItems - 1] = InsertAfter;
                                  UserAction[nItems - 1] = InsertAfter;
                                  ElemAction[nItems - 1] = pEl;
                                  ElemTypeAction[nItems - 1] = typeNum;
                                  SSchemaAction[nItems - 1] = pSS;
                                  separatorAfter = TRUE;
                                  CheckAction (&prevMenuInd, &menuInd, &nItems);
                                }
                            }
                        }
                  }
              /* si le voisin possible est un composant d'agregat, ajoute des */
              /* entrees pour les voisins possibles suivants */
              if (!isList)
                {
                  distance = 1;
                  do
                    /* boucle sur les voisins suivants */
                    {
                      distance++;
                      SRuleForSibling (pDoc, pEl, FALSE, distance, &typeNum, &pSS, &isList, &optional);
                      if (typeNum > 0 &&
                          !TypeHasException (ExcNoCreate, typeNum, pSS) &&
                          /* pas d'exception interdisant a l'utilisateur */
                          /* de creer ce type d'element */
                          !ExcludedType (pEl->ElParent, typeNum, pSS) &&
                          /* si inclusion, on ne propose pas les ref. */
                          (create || paste
                           || pSS->SsRule->SrElem[typeNum - 1]->SrConstruct != CsReference))
                        {
                          GetExternalTypeName (pSS, typeNum, &typeName1);
                          UserElementName (pEl, FALSE, &typeName2);
                          /* envoie l'evenement item a creer */
                          if (TteItemMenuInsert (pSS, typeNum, pEl, pDoc, InsertAfter))
                            {
                              if (!AddInsertMenuItem (typeName1,
                                                      TtaGetMessage (LIB, TMSG_AFTER),
                                                      typeName2, &prevMenuInd, &nItems,
                                                      &menuInd, menuBuf))
                                /* menu sature' */
                                {
                                  pEl = NULL;
                                  optional = FALSE;
                                }
                              else
                                {
                                  Action[nItems - 1] = InsertAfter;
                                  UserAction[nItems - 1] = InsertAfter;
                                  ElemAction[nItems - 1] = pEl;
                                  ElemTypeAction[nItems - 1] = typeNum;
                                  SSchemaAction[nItems - 1] = pSS;
                                }
                            }
                        }
                    }
                  while (!isList && pEl != NULL && typeNum != 0);
                }
              i++;
            }
          if ((lastChar > 0 && lastChar <= (lastSel)->ElTextLength) || pEl == NULL)
            /* la selection se termine a l'interieur d'une feuille de */
            /* texte, on ne creera pas aux niveaux superieurs */
            pEl = NULL;
          else
            /* saute les marques de pages
             * suivantes */
            {
              pSibling = pEl->ElNext;
              FwdSkipPageBreak (&pSibling);
              if (pSibling == NULL)
                pEl = pEl->ElParent;
              else
                {
                  if (typeNum == 0)
                    AddItemWithinSiblimg (pSibling, TRUE, &menuInd, &nItems,
                                          &prevMenuInd, &separatorBefore, &separatorAfter, menuBuf, pDoc);
                  pEl = NULL;
                }
            }
        }
      while (i < InsertionLevels && pEl != NULL);

      if (nItems > 0)
        {
          createPasteMenuOK = FALSE;
          if (create)
            strncpy (menuTitle, TtaGetMessage (LIB, TMSG_INSERT), MAX_NAME_LENGTH);
          else if (paste)
            strncpy (menuTitle, TtaGetMessage (LIB, TMSG_PASTE), MAX_NAME_LENGTH);
          else
            strncpy (menuTitle, TtaGetMessage (LIB, TMSG_INCLUDE), MAX_NAME_LENGTH);
	    
          BuildPasteMenu (menuRef, menuBuf, menuTitle, nItems, button);
        }
    }
  *ret = createPasteMenuOK;
  return;
}


/*----------------------------------------------------------------------
  InsertSecondPairedElem                                          
  la marque debut d'une paire (pEl) vient d'etre creee dans le	
  le document pDoc - on ajoute la marque de fin correspondante apres 
  pElem (apres le caractere de rang rank si rank != 0).		
  ----------------------------------------------------------------------*/
static void InsertSecondPairedElem (PtrElement pEl, PtrDocument pDoc,
                                    PtrElement pElem, int rank)
{
  PtrElement          pSecondEl, pSibling;
  PtrSSchema          pSS;
  PtrSRule            pParentSRule;
  int                 typeNum, nSiblings;
  NotifyElement       notifyEl;
  ThotBool            split;

  pSS = pEl->ElStructSchema;
  /* la regle de structure du deuxieme element de la paire suit */
  /* immediatement celle du premier dans le schema de structure */
  typeNum = pEl->ElTypeNumber + 1;
  if (pElem->ElTypeNumber == (CharString + 1) &&
      rank > 1 && rank <= pElem->ElVolume)
    split = TRUE;
  else
    {
      split = FALSE;
      /* on ne peut pas inserer comme descendant direct */
      /* d'un element Choice ou Identity */
      if (pElem->ElParent != NULL)
        {
          pParentSRule = pElem->ElParent->ElStructSchema->SsRule->SrElem[pElem->ElParent->ElTypeNumber - 1];
          if (pParentSRule->SrConstruct == CsChoice)
            /* le pere est un choix, on inserera a son niveau */
            pElem = pElem->ElParent;
          else if (pParentSRule->SrConstruct == CsIdentity)
            if (EquivalentSRules (pElem->ElTypeNumber, pElem->ElStructSchema,
                                  pElem->ElParent->ElTypeNumber, pElem->ElParent->ElStructSchema, pElem))
              /* l'element et son pere sont de type equivalent */
              pElem = pElem->ElParent;
        }
    }
  if (split)
    {
      /* envoie l'evenement ElemNew.Pre pour la creation du second element */
      notifyEl.event = TteElemNew;
      notifyEl.document = (Document) IdentDocument (pDoc);
      notifyEl.element = (Element) (pElem->ElParent);
      notifyEl.info = 0; /* not sent by undo */
      notifyEl.elementType.ElTypeNum = typeNum;
      notifyEl.elementType.ElSSchema = (SSchema) pSS;
      nSiblings = 0;
      if (pElem != NULL)
        {
          pSibling = pElem;
          nSiblings++;
          while (pSibling->ElPrevious != NULL)
            {
              nSiblings++;
              pSibling = pSibling->ElPrevious;
            }
        }
      notifyEl.position = nSiblings;
      if (CallEventType ((NotifyEvent *) & notifyEl, TRUE))
        /* l'application a pris la main */
        return;
      /* c'est une feuille de texte */
      /* couper a` l'endroit ou` finit la selection */
      SplitAfterSelection (pElem, rank, pDoc);
    }
  /* cree la marque de fin et l'insere apres pElem */
  pSecondEl = NewSubtree (typeNum, pSS, pDoc, TRUE, TRUE, TRUE, TRUE);
  if (pSecondEl != NULL)
    /* creation reussie */
    {
      /* insere la nouvelle marque dans l'arbre */
      InsertElementAfter (pElem, pSecondEl);
      /* met le meme identificateur que la marque de debut */
      pSecondEl->ElPairIdent = pEl->ElPairIdent;
      /* chaine les deux marques ensemble */
      pSecondEl->ElOtherPairedEl = pEl;
      pEl->ElOtherPairedEl = pSecondEl;
      /* envoie l'evenement ElemNew.Post pour la marque de fin */
      NotifySubTree (TteElemNew, pDoc, pSecondEl, 0, 0, FALSE, FALSE);
      /* cree les paves de la nouvelle marque */
      CreateAllAbsBoxesOfEl (pSecondEl, pDoc);
    }
}


/*----------------------------------------------------------------------
  CreatePasteIncludeMenuCallback traite le retour des menus INSERT, PASTE et       
  INCLUDE.                                                        
  create est vrai s'il faut creer de nouveaux elements (appel par   
  la commande INSERT).                                            
  Si create est faux, il faut coller les elements sauve's (appel    
  par la commande PASTE) si paste est vrai, ou inclure une        
  copie d'element si paste est faux.                              
  item est le numero de l'entree choisie dans le menu.             
  ----------------------------------------------------------------------*/
void CreatePasteIncludeMenuCallback (ThotBool create, ThotBool paste, int item)
{
  PtrElement          firstSel, lastSel, newsel, pEl, pLeaf, pFollow,
    pFree, pNext, pNextEl;
  PtrDocument         pDoc;
  int                 firstChar, lastChar, i, char1, chosen;
  ThotBool            withinImage, createPage;

  if (FirstSavedElement == NULL && paste)
    return;
  createPasteMenuOK = FALSE;
  char1 = 0;
  withinImage = FALSE;
  /* demande la selection courante */
  if (GetCurrentSelection (&pDoc, &firstSel, &lastSel, &firstChar, &lastChar))
    /* on ne peut inserer ou coller dans un document en lecture seule */
    if (!pDoc->DocReadOnly)
      {
        if (item > 0)
          {
            /* traitement selon l'entree choisie */
            chosen = item - 1;
            pFree = NULL;
            NCreatedElements = 0;
            FirstCreation = create;		/* c'est une creation de nouveaux elements */
            newsel = NULL;
            /* on n'a encore rien cree */
            switch (Action[chosen])
              {
              case InsertBefore:
                /* ElemIsBefore */
                createPage = ElemTypeAction[chosen] == PageBreak + 1;
                if (ElemAction[chosen] != firstSel || firstChar < 2)
                  /* avant un element complet */
                  if (create)
                    newsel = CreateSibling (pDoc, ElemAction[chosen],
                                            TRUE, TRUE, ElemTypeAction[chosen],
                                            SSchemaAction[chosen], FALSE);
                  else if (paste)
                    newsel = NULL;
                  else
                    /* inclusion */
                    newsel = CreateSibling (pDoc, ElemAction[chosen],
                                            TRUE, TRUE, ElemTypeAction[chosen],
                                            SSchemaAction[chosen], TRUE);
                else
                  {
                    if (firstSel->ElTerminal
                        && firstSel->ElLeafType == LtText
                        && firstSel->ElTextLength < firstChar)
                      /* apres l'element complet */
                      if (create)
                        newsel = CreateSibling (pDoc, firstSel, FALSE,
                                                TRUE, ElemTypeAction[chosen],
                                                SSchemaAction[chosen], FALSE);
                      else if (paste)
                        newsel = NULL;
                      else
                        /* inclusion */
                        newsel = CreateSibling (pDoc, firstSel, FALSE, TRUE,
                                                ElemTypeAction[chosen],
                                                SSchemaAction[chosen], TRUE);
                    else
                      /* au milieu d'une feuille de texte */
                      /* a si l'element a couper est le dernier, il ne le */
                      /* sera plus apres la coupure. Teste si le dernier */
                      /* selectionne' est le dernier fils de son pere, */
                      /* abstraction faite des  marques de page */
                      {
                        pNextEl = SiblingElement (firstSel, FALSE);
                        /* coupe la feuille de texte */
                        SplitTextElement (firstSel, firstChar,
                                          pDoc, TRUE, &pFollow,
                                          FALSE);
                        /* met a jour la selection */
                        if (firstSel == lastSel)
                          {
                            lastSel = pFollow;
                            lastChar = lastChar - firstChar + 1;
                          }
                        /* insere devant la 2eme partie */
                        if (create)
                          newsel = CreateSibling (pDoc, pFollow, TRUE, TRUE,
                                                  ElemTypeAction[chosen],
                                                  SSchemaAction[chosen], FALSE);
                        else if (paste)
                          newsel = NULL;
                        else
                          /* inclusion */
                          newsel = CreateSibling (pDoc, pFollow, TRUE, TRUE,
                                                  ElemTypeAction[chosen],
                                                  SSchemaAction[chosen], TRUE);
                        if (newsel == NULL)
                          /* echec insertion, recolle les 2 parties de texte */
                          {
                            MergeTextElements (firstSel, &pFree, pDoc, TRUE, FALSE);
                            DeleteElement (&pFree, pDoc);
                            pFree = NULL;
                          }
                        else
                          /* insertion reussie */
                          /* construit les paves du texte coupe' en deux */
                          BuildAbsBoxSpliText (firstSel, pFollow, pNextEl, pDoc);
                      }
                  }
                break;
              case InsertAfter:
                /* InsertAfter */
                createPage = ElemTypeAction[chosen] == PageBreak + 1;
                if (ElemAction[chosen] != lastSel
                    || lastChar == 0
                    || lastChar > lastSel->ElTextLength)
                  if (create)
                    newsel = CreateSibling (pDoc, ElemAction[chosen],
                                            FALSE, TRUE, ElemTypeAction[chosen],
                                            SSchemaAction[chosen], FALSE);
                  else if (paste)
                    newsel = NULL;
                  else
                    newsel = CreateSibling (pDoc, ElemAction[chosen],
                                            FALSE, TRUE, ElemTypeAction[chosen],
                                            SSchemaAction[chosen], TRUE);
                else
                  /* au milieu d'une feuille de texte */
                  newsel = CreateOrPasteInText (create, paste, createPage, &pFree);
                break;
              case InsertWithin:
                /* InsertWithin */
                pEl = firstSel;	/* premier element selectionne */
                if (create)
                  {
                    if (pEl->ElTerminal && pEl->ElLeafType == LtPicture)
                      {
                        /* traitement particulier des images */
                        newsel = pEl;
                        pEl = NULL;
                        withinImage = TRUE;
                      }

                    /* cree a l'interieur de tous les elements selectionnes */
                    /* du niveau le plus eleve */
                    while (pEl != NULL)
                      /* traite l'element courant */
                      {
                        pLeaf = CreateWithinElement (pDoc, pEl, TRUE, FALSE);
                        if (newsel == NULL)
                          newsel = pLeaf;
                        /* 1ere feuille creee */
                        /* cherche l'element a traiter apres l'element courant */
                        pEl = NextInSelection (pEl, lastSel);
                      }
                  }
                else if (paste)
                  newsel = NULL;
                else
                  /* inclusion */
                  {
                    pLeaf = CreateWithinElement (pDoc, pEl, TRUE, TRUE);
                    if (pEl->ElAbstractBox == pLeaf->ElAbstractBox)
                      {
                        if (newsel == NULL)
                          newsel = pLeaf;
                        /* 1ere feuille creee */
                        pEl = NULL;
                      }
                    else
                      {
                        newsel = NULL;
                        createPasteMenuOK = FALSE;
                      }
                  }
                break;
              default:
                newsel = NULL;
                break;
              }
            FirstCreation = FALSE;
            if (newsel != NULL && !withinImage)
              /* on a pu creer ou coller */
              {
                createPasteMenuOK = TRUE;
                /* on ne s'interesse aux marques que s'il s'agit d'une */
                /* creation (on ignore Coller et Inclure) */
                if (create)
                  if (newsel->ElStructSchema->SsRule->SrElem[newsel->ElTypeNumber - 1]->SrConstruct == CsPairedElement)
                    /* on vient de creer un element d'une paire */
                    InsertSecondPairedElem (newsel, pDoc, lastSel, lastChar);
                SetDocumentModified (pDoc, TRUE, 20);
                /* Reaffiche les vues du document */
                TtaClearViewSelections ();
                AbstractImageUpdated (pDoc);
                RedisplayDocViews (pDoc);
                while (pFree != NULL)
                  {
                    pNext = pFree->ElNext;
                    DeleteElement (&pFree, pDoc);
                    pFree = pNext;
                  }
                /* Reaffiche les numeros suivants qui changent */
                for (i = 0; i < NCreatedElements; i++)
                  {
                    RedisplayCopies (CreatedElement[i], pDoc, TRUE);
                    UpdateNumbers (CreatedElement[i], CreatedElement[i], pDoc, TRUE);
                  }
                /* selectionne la premiere feuille cree ou le premier element */
                /* colle', si ce n'est pas une constante */
                if (newsel->ElStructSchema->SsRule->SrElem[newsel->ElTypeNumber - 1]->SrConstruct
                    == CsConstant)
                  {
                    char1 = 0;
                    newsel = newsel->ElParent;
                  }
                if (newsel != NULL && !withinImage)
                  {
                    if (char1 == 0)
                      SelectElementWithEvent (pDoc, newsel, TRUE, TRUE);
                    else
                      SelectStringWithEvent (pDoc, newsel, char1, 0);
                  }

              }

            if (newsel &&
                newsel->ElTerminal && newsel->ElLeafType == LtPicture)
              /* traitement particulier des images inserees et vides */
              ContentEditing (TEXT_INSERT);
          }
      }

}
