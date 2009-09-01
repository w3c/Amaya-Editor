/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2009
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
#include "registry.h"

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
         faite des marques de page et autres element a ignorer */
      pSibling = SiblingElement (pEl, TRUE);
      /* insere l'element dans l'arbre */
      InsertElementBefore (pEl, *pNew);
      if (pSibling == NULL)
        /* l'element devant lequel on colle n'est plus premier */
        ChangeFirstLast (pEl, pDoc, TRUE, TRUE);
    }
  else
    {
      /* teste d'abord si pEl est le dernier fils de son pere, abstraction
         faite des marques de page et autres elements a ignorer */
      pSibling = SiblingElement (pEl, FALSE);
      /* insere l'element dans l'arbre */
      InsertElementAfter (pEl, *pNew);
      if (pSibling == NULL)
        /* l'element devant lequel on colle n'est plus premier */
        ChangeFirstLast (pEl, pDoc, FALSE, TRUE);
    }
}

/*----------------------------------------------------------------------
  PasteAnElement  Paste element decribed by pSavedEl within (if within
  is TRUE), before (if before is TRUE) or after (if before is FALSE)
  element pEl in document pDoc.
  The parameter pasteOrig points to original element to be pasted when
  children are pasted instead of the saved element.
  When the parameter addedCell points to the new generated cell, this cell
  is pasted instead of the element pointed by pSavedEl.
  ----------------------------------------------------------------------*/
static PtrElement PasteAnElement (PtrElement pEl, PtrPasteElem pSavedEl,
                                  ThotBool within, ThotBool before,
                                  ThotBool *cancelled, PtrDocument pDoc,
                                  PtrElement *pasteOrig, PtrElement addedCell)
{
  PtrElement          pElem, pChild, pPasted, pOrig, pParent, pSibling,
    pAncest, pE, pElAttr, newElement;
  PtrAttribute        pInheritLang, pLangAttr;
  PtrPasteElem        pPasteD;
  PtrSSchema          pSS;
  NotifyOnValue       notifyVal;
  NotifyElement       notifyEl;
  int                 NSiblings, i, asc, nR;
  ThotBool            stop, ok, possible;

  pPasted = NULL;
  pAncest = NULL;
  pElem = NULL;
  *cancelled = FALSE;
  if (addedCell)
    pOrig = addedCell;
  else if (*pasteOrig)
    pOrig = *pasteOrig;
  else
    pOrig = pSavedEl->PeElement;

  /* don't paste a cell if the enclosing row is not pasted */
  if (!TypeHasException (ExcIsCell, pEl->ElTypeNumber,
                         pEl->ElStructSchema) &&
      TypeHasException (ExcIsCell, pOrig->ElTypeNumber,
                        pOrig->ElStructSchema))
    {
      if (*pasteOrig == NULL && pOrig && pOrig->ElFirstChild)
        {
          /* paste children of the cell instead of the cell itself */
          pOrig = pOrig->ElFirstChild;
          if (pOrig->ElStructSchema &&
              !strcmp (pOrig->ElStructSchema->SsName, "MathML"))
            /* go down one more step */
            pOrig = pOrig->ElFirstChild;
          /* this is the original of the new pasted element */
          *pasteOrig = pOrig;
        }
    }
  else if (*pasteOrig == NULL &&
           !TypeHasException (ExcIsRow, pEl->ElTypeNumber,
                              pEl->ElStructSchema) &&
           TypeHasException (ExcIsRow, pOrig->ElTypeNumber,
                             pOrig->ElStructSchema))
    {
      /* paste included cells instead of the row itself */
      pChild = pOrig->ElFirstChild;
      while (pChild && pEl)
        {
          /* paste the contents of the cell instead of the cell itself */
          pElem = PasteAnElement (pEl, NULL, within, before,
                                  cancelled, pDoc, pasteOrig, pChild);
          pEl = pElem;
          /* force to paste after now */
          before = FALSE;
          if (*pasteOrig && (*pasteOrig)->ElNext)
            *pasteOrig = (*pasteOrig)->ElNext;
          else
            pChild = pChild->ElNext;
        }
      return pElem;
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
                          nR = CreateNature (NULL, pSS->SsName, NULL,
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
                                      nR = CreateNature (NULL, pSS->SsName, NULL, pDoc->DocSSchema, pDoc);
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
                              TRUE, FALSE, TRUE, TRUE);
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
    pNextEl, pFree, pSplitText, pSel, pasteOrig;
  PtrElement          pColHead, pNextCol, pNewCol, pRow, pNextRow, pTable,
    pRealCol, addedCell, pCell, pBlock;
  PtrPasteElem        pPasteD;
  PtrAttribute        colspanAttr;
  ElementType         cellType;
  DisplayMode         dispMode;
  Document            doc;
  int                 firstChar, lastChar, view, i, nRowsTempCol, info = 0;
  int                 colspan, rowspan, back, rowType;
  ThotBool            ok, before, within, lock, cancelled, first, beginning;
  ThotBool            savebefore, withinTable, histOpen;

  before = FALSE;
  withinTable = FALSE;
  pColHead = pRow = pNextRow = pTable = pRealCol = NULL;
  if (FirstSavedElement == NULL)
    return;
  cellType.ElTypeNum = 0;
  cellType.ElSSchema = NULL;
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

          NCreatedElements = 0;
          pSplitText = NULL;
          pNextEl = NULL;
          doc = IdentDocument (pDoc);
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
          histOpen = TtaHasUndoSequence (doc);
          if (!histOpen)
            OpenHistorySequence (pDoc, firstSel, lastSel, NULL, firstChar,
                                 lastChar-1);
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
                {
                  before = beginning;
                  if (TableRowsSaved && before)
                    {
                      /* check if there is a previous cell */
                      pEl = pCell->ElPrevious;
                      while (pEl && before)
                        {
                          if (TypeHasException (ExcIsCell, pCell->ElTypeNumber,
                                                pCell->ElStructSchema))
                            before = FALSE;
                          else
                            pEl = pEl->ElPrevious;
                        }
                    }
                  else
                    withinTable = TRUE;
                }
              else
                before = FALSE;
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
                  GetCellSpans (pCell, &colspan, &rowspan, &colspanAttr);
                  if (colspan == 0)
                    /* the cell spans up to the last column of the table */
                    {
                      colspan = 1;
                      do
                        {
                          pNextCol = NextColumnInTable (pColHead, pTable);
                          if (pNextCol)
                            {
                              colspan++;
                              pColHead = pNextCol;
                            }
                        }
                      while (pNextCol);
                      /* pColHead is now the last column head of the table */
                      if (colspanAttr)
                        /* change the value of the colspan attribute to match
                           the actual spanning of the cell */
                        {
                          AddAttrEditOpInHistory (colspanAttr, pCell, pDoc,
                                                  TRUE, TRUE);
                          colspanAttr->AeAttrValue = colspan;
                        }
                    }
                  else
                    while (colspan > 1 && pColHead)
                      {
                        pColHead = NextColumnInTable (pColHead, pTable);
                        colspan--;
                      }
                }
              /* create and insert a column head for the pasted column */
              pNewCol = NewSubtree (pColHead->ElTypeNumber,
                                    pColHead->ElStructSchema, pDoc, TRUE, TRUE, TRUE, TRUE);
              if (before)
                InsertElementBefore (pColHead, pNewCol);
              else
                InsertElementAfter (pColHead, pNewCol);
              CreatedElement[NCreatedElements] = pNewCol;
              /* create the box of the new column head to allow a cell with
                 colspan=0 to refer to it and to be displayed correctly */
              CreateNewAbsBoxes (pNewCol, pDoc, 0);
              NCreatedElements++;

              /* current row */
              pRow = NULL;
              if (pCell && pColHead)
                /* get the first row in the table */
                {
                  /* Caution, there may be several types of rows, such as
                     MTR and MLABELEDTR */
                  rowType = GetElemWithException (ExcIsRow,
                                                  pCell->ElStructSchema);
                  if (rowType)
                    pRow = FwdSearchTypedElem (pColHead, rowType,
                                               pCell->ElStructSchema, NULL);
                }
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
                   ((firstSel->ElLeafType == LtPicture && firstChar == 1) ||
                    (firstSel->ElLeafType == LtText &&
                     (firstChar > firstSel->ElTextLength || firstSel->ElTextLength == 0))))
            /* the right edge of an image is selected or the selection starts
               at the end of a text element. Paste after the selected element */
            {
              pEl = firstSel;
              if (pEl->ElLeafType == LtPicture)
                {
                  // select the parent IMG is needed
                  pEl = pEl->ElParent;
                  if (pEl == NULL ||
                      !TypeHasException (ExcIsImg, pEl->ElTypeNumber, pEl->ElStructSchema))
                    pEl = firstSel;
                }
              within = FALSE;
              before = FALSE;
              pNextEl = NextElement (pEl);
            }
          else if (!firstSel->ElTerminal &&
                   !strcmp (firstSel->ElStructSchema->SsName, "SVG"))
            {
              before = FALSE;
              pEl = firstSel;
              if (TypeHasException (ExcIsDraw, pEl->ElTypeNumber, pEl->ElStructSchema))
                {
                  within = TRUE;
                  pNextEl = NULL;
                  pEl = (PtrElement)TtaGetLastChild ((Element)pEl);
                }
              else
                {
                  within = FALSE;
                  /* the element that will follow the pasted elements is the first
                     one in the current selection */
                  pNextEl = firstSel;
                }
            }
          else if (firstChar < 2)
            /* paste before element firstSel */
            {
              pEl = firstSel;
              if (pEl->ElLeafType == LtPicture)
                {
                  // select the parent IMG is needed
                  pEl = pEl->ElParent;
                  if (pEl == NULL ||
                      !TypeHasException (ExcIsImg, pEl->ElTypeNumber, pEl->ElStructSchema))
                    pEl = firstSel;
                }
              within = FALSE;
              before = TRUE;
              /* the element that will follow the pasted elements is the first
                 one in the current selection */
              pNextEl = firstSel;
            }
          else
            /* the user wants to paste in the middle of a text leaf */
            {
              /* if the element to be split is the last child of its parent,
                 it will change status after the split */
              pClose = firstSel->ElNext;
              FwdSkipPageBreak (&pClose); /* skip page breaks */
              pSplitText = firstSel;
              /* record the text leaf before it is split, to be able to restore
                 it when undoing the operation */
              AddEditOpInHistory (firstSel, pDoc, TRUE, TRUE);
              /* split the text leaf */
              SplitTextElement (firstSel, firstChar, pDoc, TRUE, &pFollowing,
                                FALSE);
              /* record the new text leaf that has been created by split. It
                 has to be deleted when undoing the command */
              AddEditOpInHistory (firstSel->ElNext, pDoc, FALSE, TRUE);
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
          pPasteD = FirstSavedElement;
          first = TRUE;
          if (!within && before && pPasteD && !WholeColumnSaved)
            /* pasting before an existing element. We will start by pasting
               the last element and we will proceed backwards */
            while (pPasteD->PeNext)
              pPasteD = pPasteD->PeNext;
          ok = FALSE;
          pasteOrig = NULL;
          addedCell = NULL; /* no cell generated */
          nRowsTempCol = 0;
          savebefore = before;
          rowspan = back = 1;
          do
            {
              if (WholeColumnSaved && withinTable)
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
                      GetCellSpans (pEl, &colspan, &rowspan, &colspanAttr);
                      if ((colspan == 0 && (back > 0 || !before)) ||
                          (colspan > 1 &&
                           ((back > 0 && (colspan - back > 1 || before)) ||
                            (back == 0 && !before))))
                        /* extend this previous cell instead of pasting the
                           new cell */
                        {
                          pEl = NULL;
                          /* move to the bottom of this cell, but stay in
                             the same block of rows */
                          pBlock = pRow->ElParent;
                          while (pRow && (rowspan > 1 || rowspan == 0))
                            {
                              pNextRow = NextRowInTable (pRow, pTable);
                              if (pNextRow->ElParent != pBlock)
                                /* the next row is in a different block. Stop*/
                                rowspan = 1;
                              else
                                /* skip the next row and the next cell from the
                                   clipboard */
                                {
                                  pRow = pNextRow;
                                  if (pPasteD)
                                    pPasteD = pPasteD->PeNext;
                                  if (rowspan > 0)
                                    rowspan--;
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
                                          &cancelled, pDoc, &pasteOrig, addedCell);
              else
                pPasted = NULL;
              if (pPasted == NULL && !WholeColumnSaved && !cancelled &&
                  /* failure, but it's not on application's refusal */
                  !within && !before && pNextEl)
                /* we were trying to paste after the last pasted element.
                   We will try to paste the same element before the element
                   that is supposed to follow the pasted elements */
                pPasted = PasteAnElement (pNextEl, pPasteD, within, TRUE,
                                          &cancelled, pDoc, &pasteOrig, addedCell);
              if (pPasted)
                /* a copy of element pPasteD has been sucessfully pasted */
                {
                  ok = TRUE;
                  pEl = pPasted;
                  if (within)
                    /* next element will be pasted after the previous one*/
                    {
                      within = FALSE;
                      nRowsTempCol = 1;
                      before = FALSE;
                      pRealCol = pColHead;
                    }
                  else if (WholeColumnSaved && pNextRow)
                    {
                      /* get the last row of the pasted cell */
                      GetCellSpans (pPasted, &colspan, &rowspan, &colspanAttr);
                      if (rowspan == 0)
                        {
                          /* infinite vertical spanning. Skip all remaining
                             rows in this block of rows */
                          while (pNextRow->ElNext)
                            pNextRow = pNextRow->ElNext;
                          /* get the first row in the next block, if any */
                          pNextRow = NextRowInTable (pNextRow, pTable);
                        }
                      else
                        while (pNextRow && rowspan > 1)
                          {
                            pNextRow = NextRowInTable (pNextRow, pTable);
                            rowspan--;
                          }
                    }
                }

              /* get the next element to be pasted */
              if (!within && before && !WholeColumnSaved)
                {
                  if (pasteOrig && pasteOrig->ElNext)
                    pasteOrig = pasteOrig->ElNext;
                  else
                    {
                      pasteOrig = NULL;
                      if (addedCell)
                        /* remove this generated cell */
                        DeleteElement (&addedCell, pDoc);
                      else
                        pPasteD = pPasteD->PePrevious;
                    }
                }
              else
                {
                  if (pasteOrig && pasteOrig->ElNext)
                    pasteOrig = pasteOrig->ElNext;
                  else
                    {
                      pasteOrig = NULL;
                      if (addedCell)
                        /* remove this generated cell */
                        DeleteElement (&addedCell, pDoc);
                      else
                        if (pPasteD)
                          pPasteD = pPasteD->PeNext;
                    }
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
              /* and send event ElemPaste.Post */
              for (i = 0; i < NCreatedElements; i++)
                if (CreatedElement[i])
                  {
                    if (WholeColumnSaved)
                      {
                        /* change the value of "info" in the latest cell
                           deletion recorded in the Undo queue */
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
                      AddEditOpInHistory (CreatedElement[i], pDoc, FALSE,TRUE);
                  }

              TtaClearViewSelections ();
              for (i = 0; i < NCreatedElements; i++)
                if (CreatedElement[i] != NULL)
                  {
                    /* create the abstract boxes of the new elements in all
                       views */
                    CreateNewAbsBoxes (CreatedElement[i], pDoc, 0);
                    /* compute the volume that the created abstract box can
                       use*/
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

          /* close the history sequence after applications have possibly
             registered more changes to the pasted elements */
          if (!histOpen)
            CloseHistorySequence (pDoc);

          if (!lock)
            {
              /* unlock table formatting */
              TtaUnlockTableFormatting ();
              if (dispMode == DisplayImmediately)
                TtaSetDisplayMode (doc, DisplayImmediately);
            }

          /* set the selection at the end of the pasted elements */
          if (NCreatedElements > 0)
            {
              pSel = NULL;
              if (savebefore)
                {
                  for (i = 0; i < NCreatedElements && !pSel; i++)
                    if (CreatedElement[i] &&
                        !TypeHasException (ExcIsColHead,
                                           CreatedElement[i]->ElTypeNumber,
                                           CreatedElement[i]->ElStructSchema))
                      pSel = CreatedElement[i];
                }
              else
                {
                  for (i = NCreatedElements - 1; i >= 0 && !pSel; i--)
                    if (CreatedElement[i] &&
                        !TypeHasException (ExcIsColHead,
                                           CreatedElement[i]->ElTypeNumber,
                                           CreatedElement[i]->ElStructSchema))
                      pSel = CreatedElement[i];
                }
              if (pSel &&
                  !pSel->ElTerminal && pSel->ElStructSchema &&
                  strcmp (pSel->ElStructSchema->SsName, "SVG"))
                pSel = LastLeaf (pSel);
              if (pSel && pSel->ElTypeNumber == CharString + 1)
                SelectPositionWithEvent (pDoc, pSel, pSel->ElTextLength+1, TRUE);
              else
                SelectElementWithEvent (pDoc, pSel, TRUE, FALSE);
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
  if (FirstSelectedColumn)
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
  int	            nComp, i;
  PtrSSchema       pSSList;
  PtrSRule         pRegle;

  pSSList = pListEl->ElStructSchema;
  *pSS = pEl->ElStructSchema;
  if (GetElementConstruct (pListEl, &nComp) == CsAny)
    /* Don't check further for xml elements (CsAny) */
    {
      *typeNum = 0;
      for (i = 1; i < pSSList->SsNRules && *typeNum <= 0; i++)
        if (pSSList->SsRule->SrElem[i - 1]->SrConstruct == CsAny &&
            TypeHasException (ExcIsPlaceholder, i, pSSList))
          *typeNum = i;
      if (*typeNum == 0)
        *typeNum = pEl->ElTypeNumber;
    }
  else
    /* it's really a List */
    {
      *typeNum = pEl->ElTypeNumber;
      // detect the creation of non template element within a template
      if (!strcmp (pSSList->SsName, "Template") &&
          pSSList != pEl->ElStructSchema)
        {
          // look for an element of the target language
          do 
            {
              pListEl =  pListEl->ElParent;
              if (pListEl &&  pListEl->ElStructSchema)
                pSSList = pListEl->ElStructSchema;
            }
          while  (!strcmp (pSSList->SsName, "Template"));
        }

      TypeListe = GetTypeNumIdentity (pListEl->ElTypeNumber, pSSList);
      /* le type des elements qui constituent la liste */
      TypeElListe = pSSList->SsRule->SrElem[TypeListe - 1]->SrListItem;
      /* on traverse les regles d'Identite' */
      TypeEl = GetTypeNumIdentity (TypeElListe, pSSList);
      /* la regle qui definit les elements de la liste */
      pRegle = pSSList->SsRule->SrElem[TypeEl - 1];
      if (pRegle->SrConstruct == CsChoice)
        {
          if (pRegle->SrNChoices > 0)
            /* c'est une liste de choix, on retient la 1ere option de ce choix*/
            if (AllowedSibling (pEl, pDoc, pRegle->SrChoice[0], pSSList, begin,
                                TRUE, FALSE))
              {
                *typeNum = pRegle->SrChoice[0];
                *pSS = pSSList;
              }
        }
      else if (pRegle->SrConstruct == CsAny)
        /* it's a list of Any */
        {
          *typeNum = 0;
          for (i = 1; i < pSSList->SsNRules && *typeNum <= 0; i++)
            if (pSSList->SsRule->SrElem[i - 1]->SrConstruct == CsAny &&
                TypeHasException (ExcIsPlaceholder, i, pSSList))
              *typeNum = i;
          if (*typeNum == 0)
            *typeNum = pEl->ElTypeNumber;
        }
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
  DuplicateAttrWithExc
  If element oldEl has an attribute with exception ExcDuplicateAttr, element
  newEl receives a copy of this attribute.
  ----------------------------------------------------------------------*/
static void DuplicateAttrWithExc (PtrElement newEl, PtrElement oldEl)
{
  PtrAttribute        pAttr, pAttr2;
  int                 len;

  pAttr = oldEl->ElFirstAttr;
  pAttr2 = NULL;
  while (pAttr && !pAttr2)
    {
      if (AttrHasException (ExcDuplicateAttr, pAttr->AeAttrNum,
                            pAttr->AeAttrSSchema))
        {
          GetAttribute (&pAttr2);	
          /* copy the attribute */
          *pAttr2 = *pAttr;	
          if (pAttr2->AeAttrType == AtTextAttr)
            /* it's a text attribute and it does not yet have a buffer */
            pAttr2->AeAttrText = NULL;
          pAttr2->AeNext = NULL;
        }
      else
        pAttr = pAttr->AeNext;
    }
  if (pAttr2)
    {
      if (pAttr2->AeAttrType == AtTextAttr)
        /* it's a text attribute, we attach a text buffer to it */
        if (pAttr->AeAttrText != NULL)
          {
            GetTextBuffer (&pAttr2->AeAttrText);
            CopyTextToText (pAttr->AeAttrText, pAttr2->AeAttrText, &len);
          }
      /* attach the attribute to its element */
      pAttr2->AeNext = newEl->ElFirstAttr;
      newEl->ElFirstAttr = pAttr2;
    }
}

/*----------------------------------------------------------------------
  ParentNotTemplate returns the first ancestor of element pEl that is not
  a Template element.
  ----------------------------------------------------------------------*/
static PtrElement ParentNotTemplate (PtrElement pEl)
{

  pEl = pEl->ElParent;
  while (pEl && pEl->ElStructSchema &&
         !strcmp (pEl->ElStructSchema->SsName, "Template"))
    pEl = pEl->ElParent;
  return pEl;
}

/*----------------------------------------------------------------------
  IsXMLEditMode returns the current edit mode
  ----------------------------------------------------------------------*/
ThotBool IsXMLEditMode ()
{
  PtrDocument         pDoc;
  PtrElement          firstSel, lastSel;
  int                 firstChar, lastChar;
  ThotBool            edit;

  TtaGetEnvBoolean ("XML_EDIT_MODE", &edit);
  if (!edit)
    {
      // check if the insert point is within a XML structure
      if (GetCurrentSelection (&pDoc, &firstSel, &lastSel, &firstChar, &lastChar))
        {
          if (firstSel && firstSel->ElStructSchema &&
              (!strcmp (firstSel->ElStructSchema->SsName, "XML") ||
               !strcmp (firstSel->ElStructSchema->SsName, "SVG") ||
               !strcmp (firstSel->ElStructSchema->SsName, "MathML")))
            // force the XML edit mode
            return TRUE;
        }
    }
  return edit;
}



/*----------------------------------------------------------------------
  AllChildrenReadOnly
  ----------------------------------------------------------------------*/
static ThotBool AllChildrenReadOnly (PtrElement el)
{
  PtrElement prev, next;

  if (el)
    {
      prev = el->ElPrevious;
      while (prev)
        {
          if (ElementIsReadOnly (prev))
            prev = prev->ElPrevious;
          else
            return FALSE;
        }
      next = el->ElNext;
      while (next)
        {
          if (ElementIsReadOnly (next))
            next = next->ElNext;
          else
            return FALSE;
        }
    }
  return TRUE;
}

/*----------------------------------------------------------------------
  TtcCreateElement handles the Return (or Enter) key.
  ----------------------------------------------------------------------*/
void TtcCreateElement (Document doc, View view)
{
  PtrElement          firstSel, lastSel, pListEl, pE, pE1, pNew;
  PtrElement           pSibling, parent;
  PtrElement          pClose, pAncest, pElem, pParent, pElDelete, pPrevious;
  PtrElement          pNext, pElReplicate, pAggregEl, pSib, firstEl, lastEl;
  PtrDocument         pDoc;
  PtrSSchema          pSS;
  NotifyElement       notifyEl;
  int                 firstChar, lastChar, NSiblings, i, j;
  int                 frame, typeNum, nComp;
  ThotBool            ok, replicate, createAfter, selBegin, selEnd, ready;
  ThotBool            empty, list, optional, deleteEmpty, histSeq,
                      xmlmode, previous, following, specialBreak,
                      insertionPoint, extendHistory, prevHist;
  ThotBool            lock = TRUE, isTemplate;
  DisplayMode         dispMode;

  if (!GetCurrentSelection (&pDoc, &firstSel, &lastSel, &firstChar, &lastChar))
    return;
  if (pDoc->DocReadOnly)
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
  extendHistory = FALSE;

  // check editing within template instances
  if (firstSel == lastSel &&
      ((firstChar == 0 && lastChar == 0 && ElementIsReadOnly (firstSel)) ||
       firstChar >= firstSel->ElVolume) &&
      firstSel->ElParent && firstSel->ElStructSchema &&
      (!strcmp (firstSel->ElStructSchema->SsName, "Template") ||
      (ElementIsReadOnly (firstSel->ElParent) &&
       !strcmp (firstSel->ElParent->ElStructSchema->SsName, "Template"))))
    {
      // move the selection to a level that allows editing
      if (firstSel->ElLeafType == LtText)
        // don't duplicate a text element
        firstSel = firstSel->ElParent;
      while (firstSel->ElParent && AllChildrenReadOnly (firstSel) &&
             ElementIsReadOnly (firstSel->ElParent))
        firstSel = firstSel->ElParent;
      lastSel = firstSel;
      firstChar = lastChar = 0;
    }

  xmlmode = IsXMLEditMode ();
  if (!xmlmode && firstSel == lastSel && firstSel->ElStructSchema &&
      !strcmp (firstSel->ElStructSchema->SsName, "Template"))
    xmlmode = TRUE;
  if (!xmlmode)
    /* We should perhaps do the following whatever the editing mode,
       AmayaLite or not ... */
    {
      /* if the selection is not an insertion point nor an empty element,
         first delete the selected content/elements */
      insertionPoint = (firstSel == lastSel  &&
                        firstSel->ElTerminal &&
                        ((firstSel->ElLeafType == LtText && SelPosition)     ||
                         (firstSel->ElLeafType == LtPicture && SelPosition)  ||
                         firstSel->ElLeafType == LtGraphics ||
                         firstSel->ElLeafType == LtPolyLine ||
                         firstSel->ElLeafType == LtPath     ||
                         firstSel->ElLeafType == LtSymbol));
      empty = (firstSel == lastSel &&
               (firstSel->ElVolume == 0 ||
                TypeHasException (ExcIsBreak, firstSel->ElTypeNumber,
                                  firstSel->ElStructSchema)));
      if (!insertionPoint && !empty)
        {
          // get the future insert point
          if (firstChar == 0 && lastChar == 0)
            {
              if (ElementIsReadOnly (firstSel->ElParent) ||
                  ElementIsReadOnly (lastSel->ElParent))
                return;
              dispMode = TtaGetDisplayMode (doc);
              if (dispMode == DisplayImmediately)
                TtaSetDisplayMode (doc, DeferredDisplay);
              // detect if a template element is created
              isTemplate = (firstSel->ElStructSchema &&
                             !strcmp (firstSel->ElStructSchema->SsName, "Template"));
              ok = FALSE;
              if (!TypeHasException (ExcIsCell, firstSel->ElTypeNumber, firstSel->ElStructSchema))
                // duplicate first the first selected element
                ok = CreateNewElement (firstSel->ElTypeNumber, firstSel->ElStructSchema,
                                       pDoc, !isTemplate);
              if (ok)
                TtaExtendUndoSequence (doc);
              // keep the new selection
              GetCurrentSelection (&pDoc, &pE, &pE1, &i, &j);
              if (!isTemplate && ok && pE != firstSel)
                {
                  // restore the initial selection
                  SelectElement (pDoc, firstSel, TRUE, FALSE, TRUE);
                  if (lastSel != firstSel)
                    ExtendSelection (lastSel, 0, TRUE, FALSE, FALSE);
                  CutCommand (FALSE, FALSE);
                }
              // set the new selection
              SelectElement (pDoc, pE, TRUE, TRUE, TRUE);
              if (dispMode == DisplayImmediately)
                TtaSetDisplayMode (doc, dispMode);
              // check if the undo history is not closed
              if (TtaHasUndoSequence (doc))
                TtaCloseUndoSequence (doc);
              return;
            }
          /* delete the selected content/elements the usual way */
          TtcDeleteSelection (doc, view);
          /* the Cut command has closed its history sequence. We will need
             to extend this sequence instead of opening a new one for the
             elements that will be created/deleted in the following */
          extendHistory = TRUE;
          /* get the new selection after the deletion */
          GetCurrentSelection (&pDoc, &firstSel, &lastSel, &firstChar, &lastChar);
          if (firstSel == lastSel &&
              (TypeHasException (ExcIsCell, firstSel->ElTypeNumber, firstSel->ElStructSchema) ||
               TypeHasException (ExcIsRow, firstSel->ElTypeNumber, firstSel->ElStructSchema)))
            return;
        }
    }
  else if (firstSel && firstSel->ElStructSchema &&
           !strcmp (firstSel->ElStructSchema->SsName, "MathML") &&
           firstSel->ElParent && firstSel->ElParent->ElStructSchema &&
           !strcmp (firstSel->ElParent->ElStructSchema->SsName, "MathML"))
    // don't manage this action within a MathML construction
    return;

  firstEl = firstSel;
  lastEl = lastSel;
  if (!ElementIsReadOnly (firstSel) && AscentReturnCreateNL (firstSel))
    {
      /* one of the ancestors of the first selected element says that the
         Return key should generate a "new line" character */
      InsertChar (GetWindowNumber (doc, view), '\n', -1);
      CloseTextInsertion ();
    }
  else if (firstSel && firstSel->ElParent && !ElementIsReadOnly (firstSel->ElParent))
    {
      /* lock the table formatting */
      TtaGiveTableFormattingLock (&lock);
      if (!lock)
        /* table formatting is not loked, lock it now */
        TtaLockTableFormatting ();

      pListEl = NULL;
      pAggregEl = NULL;
      createAfter = TRUE;
      replicate = TRUE;
      ready = FALSE;
      list = TRUE;
      pElDelete = NULL;
      pElReplicate = NULL;
      typeNum = 0;
      pSS = NULL;
      deleteEmpty = FALSE;
      histSeq = FALSE;	/* no new history sequence open */
      prevHist = TtaHasUndoSequence (doc);	/* is there a history sequence open */
      if (firstChar > 0 && firstChar == lastChar)
        lastChar--;

      /* si la selection ne comprend qu'un element vide ou un <br/>, on essaie
         de remplacer cet element par un autre au niveau superieur */
      empty = (firstSel == lastSel &&
               (firstSel->ElVolume == 0 ||
                TypeHasException (ExcIsBreak, firstSel->ElTypeNumber,
                                  firstSel->ElStructSchema)));
      if (empty)
        {
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
                if (pParent->ElParent &&
                    GetElementConstruct (pParent->ElParent, &nComp) == CsAny)
                  pListEl = pParent->ElParent;
                else
                  {
                    pAncest = pParent;
                    pListEl = AncestorList (pAncest);
                  }
                if (TypeHasException (ExcNoBreakByReturn, pParent->ElTypeNumber,
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
                                if (!prevHist && !histSeq)
                                  {
                                    if (extendHistory)
                                      TtaExtendUndoSequence (doc);
                                    else
                                      OpenHistorySequence (pDoc, firstSel,
                                           lastSel, NULL, firstChar, lastChar);
                                    histSeq = TRUE;
                                  }
                                if (BreakElement (pParent->ElParent, pElem,
                                                  0, FALSE, TRUE))
                                  /* element pParent has been split */
                                  {
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
                    if (pElem->ElPrevious && !pElem->ElNext)
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
                            pAncest = pElReplicate->ElParent;
                            //pAncest = ParentNotTemplate (pAncest);
                            while (pAncest && pAncest != pListEl)
                              {
                                pElReplicate = pAncest;
                                pAncest = pAncest->ElParent;
                              }
                          }
                      }
                    else if (pElem->ElNext && !pElem->ElPrevious &&
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
                        pAncest = pElReplicate->ElParent;
                        //pAncest = ParentNotTemplate (pAncest);
                        while (pAncest != pListEl)
                          {
                            pElReplicate = pAncest;
                            pAncest = pAncest->ElParent;
                          }
                      }
                    else if (!TypeHasException (ExcNoCreate,
                                                pParent->ElTypeNumber,
                                                pParent->ElStructSchema))
                      /* try to split element pParent before element pElem */
                      {
                        /* store the editing operation in the history */
                        if (!prevHist && !histSeq)
                          {
                            if (extendHistory)
                              TtaExtendUndoSequence (doc);
                            else
                              OpenHistorySequence (pDoc, firstSel, lastSel,
                                                   NULL, firstChar, lastChar);
                            histSeq = TRUE;
                          }
                        if (BreakElement (pParent, pElem, 0, FALSE, FALSE))
                          /* element pParent has been split */
                          {
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
                if (list && pListEl == NULL)
                  {
                    pAncest = pElem;
                    pListEl = AncestorList (pAncest);
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
          if (ready && list && pListEl && pElReplicate)
            {
              replicate = FALSE;
              ReturnCreateNewElem (pListEl, pElReplicate,
                                   (ThotBool)!createAfter, pDoc,
                                   &typeNum, &pSS);
            }
          else
            pListEl = NULL;
        }
	
      selBegin = FALSE;
      selEnd = FALSE;
      if (!ready && !empty)
        {
          /* La selection commence-t-elle en tete ou en queue d'element? */
          pListEl = NULL;
          ok = TRUE;
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
                        /* no previous sibling and at the beginning */
                        selBegin = TRUE;
                      if (firstSel->ElNext == NULL &&
                          firstChar > firstSel->ElTextLength)
                        /* no next sibling and at the end */
                        selEnd = TRUE;
                    }
                  else if (firstSel->ElLeafType == LtPicture)
                    {
                      // check previous and next siblings of the picture
                      parent = firstSel->ElParent;
                      if (TypeHasException (ExcIsImg, parent->ElTypeNumber,
                                            parent->ElStructSchema))
                        firstEl = lastEl = parent;
                      if (firstEl->ElPrevious == NULL && firstChar == 0)
                        /* no previous and selection is on the left border */
                        selBegin = TRUE;
                      if (firstEl->ElNext == NULL && firstChar > 0)
                        /* no next and selection is on the right border */
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
                        /* use the enclosing element */
                        firstEl = lastEl = firstSel->ElParent;
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

          specialBreak = FALSE;
          if (!xmlmode)
            {
              /* if we are within a Paragraph (or equivalent) at any level,
                 we split that element */
              pE = firstEl->ElParent;
              previous = FALSE;
              following = FALSE;
              while (pE &&
                     !TypeHasException (ExcParagraphBreak, pE->ElTypeNumber,
                                        pE->ElStructSchema))
                {
                  if (selBegin && pE->ElPrevious)
                    previous = TRUE;
                  if (selEnd && pE->ElNext)
                    following = TRUE;
                  pE = pE->ElParent;
                }
              if (pE)
                /* there is an enclosing Paragraph (or equivalent) element,
                   we can split it */
                {
                  if (previous)
                    selBegin = FALSE;
                  if (following)
                    selEnd = FALSE;
                }
              /* if the Paragraph (or equivalent) element is within a HTML <li>
                 we split that <li> element */
              previous = FALSE;
              following = FALSE;
              pE1 = pE;
              while (pE1 &&
                     !TypeHasException (ExcListItemBreak, pE1->ElTypeNumber,
                                        pE1->ElStructSchema))
                {
                  if (selBegin && pE1->ElPrevious)
                    previous = TRUE;
                  if (selEnd && pE1->ElNext)
                    following = TRUE;
                  pE1 = pE1->ElParent;
                }
              if (pE1)
                {
                  pE = pE1;
                  if (previous)
                    selBegin = FALSE;
                  if (following)
                    selEnd = FALSE;
                }
              if (pE)
                /* there is an enclosing Paragraph or <li> element, we
                   split it */
                {
                  specialBreak = TRUE;
                  pElReplicate = pE;
                  pListEl = pE->ElParent;
                  //pListEl = ParentNotTemplate(pE);
                }
            }

          /* Si la selection ne commence ni en tete ni en queue, on */
          /* essaie de couper un element en deux */
          if (!selBegin && !selEnd &&
              (specialBreak || CanSplitElement (firstEl, firstChar, TRUE,
                                                &pAncest, &pE, &pElReplicate)))
            {
              /* register the operation in history */
              if (!prevHist && !histSeq)
                {
                  if (extendHistory)
                    TtaExtendUndoSequence (doc);
                  else
                    OpenHistorySequence (pDoc, firstSel, lastSel, NULL,
                                         firstChar, lastChar);
                  histSeq = TRUE;
                }
              if (!specialBreak)
                pElReplicate = NULL;
              if (BreakElement (pElReplicate, firstEl, firstChar, TRUE, TRUE))
                {
                  if (histSeq)
                    CloseHistorySequence (pDoc);
                  if (!lock)
                    /* unlock table formatting */
                    TtaUnlockTableFormatting ();
                  return;
                }
              else
                {
                  ok = FALSE;
                  pListEl = NULL;
                }
            }

          /* on cherche l'element CsList ascendant qui permet de creer un */
          /* element voisin */
          if (lastEl->ElTerminal && lastEl->ElLeafType == LtPageColBreak)
            /* on ne duplique pas les sauts de pages */
            pListEl = NULL;
          else if (ok && !pListEl)
            {
              if (lastEl->ElParent &&
                  GetElementConstruct (lastEl->ElParent, &nComp) == CsAny)
                pListEl = ParentNotTemplate(lastEl);
              else
                {
                  pAncest = lastEl;
                  /* ignore the next Template ancestors */
                  //pAncest = ParentNotTemplate (pAncest);
                  pListEl = AncestorList (pAncest);
                }
              /* si c'est la fin d'une liste de Textes on remonte */
              if (pListEl != NULL)
                {
                  if (lastEl->ElTerminal &&
                      (lastEl->ElNext == NULL || selBegin) &&
                      pListEl == ParentNotTemplate(lastEl) &&
                      GetElementConstruct (lastEl->ElParent, &nComp) != CsAny &&
                      !TypeHasException (ExcReturnCreateWithin,
                                         pListEl->ElTypeNumber,
                                         pListEl->ElStructSchema))
                    {
                      pE1 = pListEl->ElParent;//pE1 = ParentNotTemplate(pListEl);
                      if (pE1 && GetElementConstruct (pE1, &nComp) == CsAny)
                        pListEl = pE1;
                      else
                        {
                          pAncest = pListEl;
                          /* ignore the next Template ancestors */
                          //pAncest = ParentNotTemplate (pAncest);
                          pListEl = AncestorList (pAncest);
                        }
                    }
                }
              else
                {
                  /* There is no List ancestor, search an Any parent */
                  if (lastEl->ElTerminal)
                    pListEl = ParentAny (lastEl->ElParent);
                  else
                    pListEl = ParentAny (lastEl);
                  if (pListEl != NULL)
                    {
                      if (lastEl->ElTerminal &&
                          pListEl == ParentNotTemplate(lastEl) &&
                          (lastEl->ElNext == NULL || selBegin) &&
                          !TypeHasException (ExcReturnCreateWithin,
                                             pListEl->ElTypeNumber,
                                             pListEl->ElStructSchema))
                        pListEl = ParentAny (pListEl);
                    }
                }
            }

          /* verifie si les elements a doubler portent l'exception NoCreate */
          if (pListEl)
            {
              pE = lastEl;
              pElReplicate = NULL;
              do
                {
                  if (pE->ElStructSchema &&
                      ElementIsReadOnly (pE) &&
                      !strcmp (pE->ElStructSchema->SsName, "Template"))
                    {
                      pElReplicate = pE;
                      pE = NULL;
                    }
                  else if (TypeHasException (ExcNoCreate, pE->ElTypeNumber,
                                             pE->ElStructSchema))
                    /* abort */
                    pListEl = NULL;
                  else
                    {
                      pElReplicate = pE;
                      pE = pE->ElParent;
                    }
                }
              while (pE != pListEl && pListEl && pE);

              /* a priori, on creera le meme type d'element */
              if (pElReplicate)
                {
                  if (TypeHasException (ExcNoReplicate, pElReplicate->ElTypeNumber,
                                        pElReplicate->ElStructSchema) ||
                      (pElReplicate->ElStructSchema &&
                       (!strcmp (pElReplicate->ElStructSchema->SsName, "MathML") ||
                        !strcmp (pElReplicate->ElStructSchema->SsName, "SVG"))))
                    {
                      if (!selBegin)
                      selEnd = TRUE;
                    }
                  else
                    typeNum = pElReplicate->ElTypeNumber;
                  pSS = pElReplicate->ElStructSchema;
                }
            }

          if (pListEl)
            {
              /* verifie si la selection est en fin ou debut de paragraphe */
              if (selEnd && pElReplicate)
                /* verifie s'il faut creer le meme type d'element ou un type */
                /* different */
                {
                  replicate = FALSE;
                  createAfter = TRUE;
                  ReturnCreateNewElem (pListEl, pElReplicate, FALSE, pDoc,
                                       &typeNum, &pSS);
                }
              else if (selBegin && pElReplicate)
                {
                  replicate = FALSE;
                  createAfter = FALSE;
                  ReturnCreateNewElem (pListEl, pElReplicate, TRUE, pDoc,
                                       &typeNum, &pSS);
                }
            }
        }
      /* verifie que la liste ne depasse pas deja la longueur maximum */
      if (pListEl)
        if (!CanChangeNumberOfElem (pListEl, 1))
          pListEl = NULL;
      if (pListEl || pAggregEl)
        {
          if (pListEl == NULL)
            pListEl = pAggregEl;
          /* demande a l'application si on peut creer ce type d'element */
          notifyEl.event = TteElemNew;
          notifyEl.document = doc;
          notifyEl.element = (Element) (pElReplicate->ElParent);
          notifyEl.info = 0; /* not sent by undo */
          notifyEl.elementType.ElTypeNum = typeNum;
          notifyEl.elementType.ElSSchema = (SSchema) pSS;
          pSibling = pElReplicate;
          NSiblings = 0;
          while (pSibling->ElPrevious)
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
      if (pListEl)
        {
          //ok = !ElementIsReadOnly (pListEl);
          ok = !ElementIsReadOnly (pElReplicate->ElParent);
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
              if (pElDelete)
                /* detruire le sous-arbre qu'on remplace */
                {
                  deleteEmpty = TRUE;
                  if (!prevHist && !histSeq)
                    {
                      /* handle the remaining unlock of table formatting */
                      if (extendHistory)
                        TtaExtendUndoSequence (doc);
                      else
                        OpenHistorySequence (pDoc, firstSel, lastSel, NULL,
                                             firstChar, lastChar);
                      histSeq = TRUE;
                    }
                  AddEditOpInHistory (pElDelete, pDoc, TRUE, FALSE);
                  pPrevious = PreviousNotPage (pElDelete, TRUE);
                  pNext = NextNotPage (pElDelete, FALSE);
                  DestroyAbsBoxes (pElDelete, pDoc, TRUE);
                  AbstractImageUpdated (pDoc);
                  /* prepare l'evenement ElemDelete.Post */
                  notifyEl.event = TteElemDelete;
                  notifyEl.document = doc;
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
                  if (ElemIsWithinSubtree (firstSel, pElDelete))
                    firstSel = NULL;
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
                    if (SiblingElement (pNext, TRUE) == NULL)
                      /* l'element qui suit l'element detruit devient premier*/
                      ChangeFirstLast (pNext, pDoc, TRUE, FALSE);
                  if (pPrevious != NULL)
                    if (SiblingElement (pPrevious, FALSE) == NULL)
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
                  pNew = NewSubtree (lastEl->ElTypeNumber,
                                     lastEl->ElStructSchema, pDoc,
                                     TRUE, TRUE, TRUE, TRUE);
                  DuplicateAttrWithExc (pNew, lastEl);
                  pE = lastEl;
                  while (pE->ElParent != pListEl && pE->ElParent &&
                         pE->ElParent->ElStructSchema &&
                         strcmp (pE->ElParent->ElStructSchema->SsName, "Template"))
                    {
                      pE = pE->ElParent;
                      pAncest = ReplicateElement (pE, pDoc);
                      InsertFirstChild (pAncest, pNew);
                      pNew = pAncest;
                    }
                }
              /* Insertion du nouvel element */
              if (ElemDoesNotCount (pNew, !createAfter))
                pSib = NULL;
              else
                pSib = pE;
              if (createAfter)
                {
                  if (pSib)
                    {
                      pClose = SiblingElement (pSib, FALSE);
                      if (pClose)
                        pSib = NULL;
                      else
                        /* no significant sibling after */
                        if (ElemDoesNotCount (pSib, FALSE))
                          /* this element does not count as last element. Look for
                             the one that is really considered as the last child */
                          pSib =  SiblingElement (pSib, TRUE);
                    }
                  InsertElementAfter (pE, pNew);
                  if (pSib)
                    /* element pSib is no longer the last child */
                    ChangeFirstLast (pSib, pDoc, FALSE, TRUE);
                }
              else
                {
                  if (pSib)
                    {
                      pClose = SiblingElement (pSib, TRUE);
                      if (pClose)
                        pSib = NULL;
                      else
                        /* no significant sibling before */
                        if (ElemDoesNotCount (pSib, TRUE))
                          /* this element does not count as first element. Look for
                             the one that is really considered as the first child */
                          pSib =  SiblingElement (pSib, FALSE);
                    }
                  InsertElementBefore (pE, pNew);
                  if (pSib)
                    /* element pSib is no longer the first child */
                    ChangeFirstLast (pSib, pDoc, TRUE, TRUE);
                }
              if (!prevHist && !histSeq)
                {
                  if (extendHistory)
                    TtaExtendUndoSequence (doc);
                  else
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
                      TtaUnlockTableFormatting ();
                      lock = TRUE; /* unlock is done */
                    }

                  if (dispMode == DisplayImmediately)
                    TtaSetDisplayMode (doc, dispMode);
                  /* restore a selection */
                  if (!xmlmode && !empty && selBegin && firstSel)
                    {
                      /* set the caret at the position it was before */
                      if (!firstSel->ElTerminal)
                        SelectElement (pDoc, firstSel, TRUE, TRUE, TRUE);
                      else if (firstSel->ElLeafType == LtText)
                        MoveCaret (pDoc, firstSel, 1);
                      else if (firstSel->ElLeafType == LtPicture)
                        MoveCaret (pDoc, firstSel, 0);
                    }
                  else
                    /* set the selection within the new element */
                    SelectElementWithEvent (pDoc, FirstLeaf (pNew), TRUE, TRUE);
                }
            }
        }
      if (!lock)
        /* handle the remaining unlock of table formatting */
        TtaUnlockTableFormatting ();
      if (histSeq)
        CloseHistorySequence (pDoc);
    }
}

/*----------------------------------------------------------------------
  AscentChildOfParagraph  return the ancestor of element pEl
  (or pEl itself) whose parent has exception ExcParagraphBreak.             
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
  DeleteNextChar  If before, the current selection is at the      
  beginning of element pEl and the user has hit the       
  BackSpace key. Merging with previous element.
  If not before, the current selection is at the end of   
  element pEl and the user has hit the Delete key.  Merging with
  next element.
  ----------------------------------------------------------------------*/
void DeleteNextChar (int frame, PtrElement pEl, ThotBool before)
{
  PtrElement          pSibling, pNext, pPrev, pE, pE1, pElem, pParent, pS;
  PtrElement          pSel, pSuccessor, pLeaf;
  PtrElement         *list;
  PtrDocument         pDoc;
  NotifyElement       notifyEl;
  NotifyOnValue       notifyVal;
  Document            doc;
  int                 nSiblings;
  int                 nbEl, j, firstChar, lastChar;
  ThotBool            stop, ok, isRow, xmlmode, selHead, histOpen;

  if (pEl == NULL)
    return;
  /* pSel: element to be selected when finished */
  pSel = pEl;
  selHead = before;
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
        pSibling = PreviousNotPage (pParent, TRUE);
      else
        pSibling = NextNotPage (pParent, TRUE);
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
  while (pParent && pSibling == NULL);

  if (pParent == NULL || pSibling == NULL)
    return;

  /* We do nothing if the parent or the sibling elements are read-only */
  if (TtaIsReadOnly((Element) pParent) || TtaIsReadOnly((Element) pSibling))
    return;

  /* determine the current selection */
  firstChar = 0;  lastChar= 0;
  if (pEl->ElTerminal)
    {
      if (pEl->ElLeafType == LtText)
        {
          if (before)
            firstChar = 1;
          else
            firstChar = pEl->ElVolume + 1;
          lastChar = firstChar - 1;
        }
      else if (pEl->ElLeafType == LtPicture)
        {
          if (before)
            firstChar = 0;
          else
            firstChar = 1;
          lastChar = firstChar;
        }
    }
  doc = IdentDocument (pDoc);

  if ((pSibling->ElVolume == 0 && pParent->ElVolume > 0) ||
      pSibling->ElStructSchema->SsRule->SrElem[pSibling->ElTypeNumber - 1]->SrConstruct == CsConstant)
    /* BackSpace at the beginning of a non empty element (pParent) whose
       previous sibling (pSibling) is empty, or
       Delete at the end of a non empty element (pParent) whose next sibling
       (pSibling) is empty.
       Delete the empty sibling */
    {
      histOpen = TtaHasUndoSequence (doc);
      if (!histOpen)
        OpenHistorySequence (pDoc, pEl, pEl, NULL, firstChar, lastChar);
      /* record the element to be deleted in the history */
      AddEditOpInHistory (pSibling, pDoc, TRUE, FALSE);
      TtaDeleteTree ((Element)pSibling, doc);
      if (!histOpen)
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

  /* In standard mode, we merge two block elements */
  xmlmode = IsXMLEditMode ();
  if (!xmlmode)
    {
      /* get the ancestor block element */
      pE = pEl;
      while (pE && pE->ElParent &&
             !TypeHasException (ExcParagraphBreak,
                                pE->ElParent->ElTypeNumber,
                                pE->ElParent->ElStructSchema))
        pE = pE->ElParent;
      if (pE && pE->ElParent)
        /* we have found an ancestor block. use it instead of the original
           element */
        pElem = pE;

      /* get the lowest level block element in the sibling element */
      pE = pSibling;
      pE1 = NULL;
      while (pE && !pE->ElTerminal)
        {
          if (TypeHasException (ExcParagraphBreak, pE->ElTypeNumber,
                                pE->ElStructSchema))
            /* this is the lowest level block element seen so far */
            pE1 = pE;
          pE = pE->ElFirstChild;
          if (before && pE)
            while (pE->ElNext)
              pE = pE->ElNext;
        }
      if (pE1)
        pSibling = pE1;

      /* Are we in the special case of a BackSpace at the beginning of a list
         item? */
      if (before &&     /* BackSpace */
          pElem && pElem->ElParent && pElem->ElParent->ElParent)
        {
          pE = pElem->ElParent->ElParent;
          if (pE && pE->ElParent &&
              TypeHasException (ExcListItemBreak, pE->ElTypeNumber,
                                pE->ElStructSchema))
            /* we are at the beginning of another list item */
            {
              if (pSibling && pSibling->ElParent && pE != pSibling->ElParent &&
                  TypeHasException (ExcListItemBreak,
                                    pSibling->ElParent->ElTypeNumber,
                                    pSibling->ElParent->ElStructSchema))
                /* the previous element is a list item */
                {
                  /* move the content of the current list item within
                     the previous list item */
                  before = FALSE;
                  pElem = pSibling;
                  pSibling = pE;
                }
            }
        }
    }

  if (pSibling && pParent != pEl && pElem)
    {
      if (pSibling->ElTerminal)
        /* don't merge a structured element with a text string */
        pSibling = NULL;
      else
        /* check whether the SSchema allows elements to be merged, i.e. can
           children of element pSibling become siblings of element pElem? */
        {
          pSibling = pSibling->ElFirstChild;
          if (pSibling)
            {
              if (before)
                while (pSibling->ElNext)
                  pSibling = pSibling->ElNext;
              if (!AllowedSibling (pElem, pDoc, pSibling->ElTypeNumber,
                               pSibling->ElStructSchema, before, FALSE,FALSE))
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
            pSibling = PreviousNotPage (pElem, TRUE);
          else
            pSibling = NextNotPage (pElem, TRUE);
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
            pLeaf = NextLeaf (pElem);
            pSibling = AscentChildOfParagraph (pLeaf);
            if (pSibling == NULL && pLeaf->ElVolume == 0)
              {
                pElem = pLeaf;
                pSibling = pParent;
              }
            else if (ElemIsAnAncestor (pSibling, pElem))
              pSibling = pLeaf;
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
              (pSibling->ElAccess == ReadOnly &&
               pSibling->ElVolume == 1))
            {
              if (pSibling->ElFirstChild == NULL)
                /* pSibling is empty. Delete it */
                {
                  /* record the element to be deleted in the history */
                  histOpen = TtaHasUndoSequence (doc);
                  if (!histOpen)
                  OpenHistorySequence (pDoc, pEl, pEl, NULL, firstChar, lastChar);
                  AddEditOpInHistory (pSibling, pDoc, TRUE, FALSE);
                  TtaDeleteTree ((Element)pSibling, doc);
                  if (!histOpen)
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
                                             pSibling->ElTextLength + 1, FALSE);
                    /* simulate a backspace */
                    InsertChar (frame, '\177', -1);
                  }
                else
                  {
                    /* set selection before the first character of the
                       string */
                    if (pSibling->ElVolume == 0)
                      SelectElement (pDoc, pSibling, FALSE, FALSE, TRUE);
                    else
                      SelectString (pDoc, pSibling, 1, 0);
                    /* simulate a delete */
                    ContentEditing (TEXT_DEL);
                    //TtcDeleteSelection (IdentDocument (pDoc), 0);
                  }
              else if (strcmp (pSibling->ElStructSchema->SsName, "SVG"))
                /* don't delete a graphic element when the user enters
                   Backspace at the beginning of a svg:text element */
                {
                  /* set selection before the first character of the string */
                  SelectElement (pDoc, pSibling, FALSE, FALSE, TRUE);
                  /* and delete the selected element */
                  CutCommand (FALSE, FALSE);
                }
            }
          /* done */
          pElem = NULL;
          pSibling = NULL;
        }
    }

  if (pElem != NULL && pSibling != NULL &&
      AllowedSibling (pElem, pDoc, pSibling->ElTypeNumber,
                      pSibling->ElStructSchema, before, FALSE, FALSE))
    {
      /* switch selection off */
      TtaClearViewSelections ();
      /* count elements to be moved */
      pE = pSibling;
      nbEl = 0;
      while (pE)
        {
          nbEl++;
          if (before)
            pE = pE->ElPrevious;
          else
            pE = pE->ElNext;
        }
      /* get a buffer for keeping track of all element to be moved */
      list = (PtrElement *) TtaGetMemory (nbEl * sizeof (PtrElement));
      pE = pSibling;
      nbEl = 0;
      while (pE)
        {
          list[nbEl++] = pE;
          if (before)
            pE = pE->ElPrevious;
          else
            pE = pE->ElNext;
        }
      pParent = pSibling->ElParent;
      isRow = TypeHasException (ExcIsRow, pParent->ElTypeNumber,
                                pParent->ElStructSchema);

      /* start history sequence */
      histOpen = TtaHasUndoSequence (doc);
      if (!histOpen)
        OpenHistorySequence (pDoc, pEl, pEl, NULL, firstChar, lastChar);
      /* move all these elements */
      j = 0;
      while (pSibling)
        {
          j++;
          if (j < nbEl)
            pNext = list[j];
          else
            pNext = NULL;
          /* Send event ElemDelete.Pre to application for existing elements */
          if (isRow || !SendEventSubTree (TteElemDelete, pDoc, pSibling,
                                          TTE_STANDARD_DELETE_LAST_ITEM,
                                          0, FALSE, FALSE))
            {
              /* kill all abstract boxes for the element to be moved */
              DestroyAbsBoxes (pSibling, pDoc, TRUE);
              AbstractImageUpdated (pDoc);
              if (!isRow)
                {
                  /* prepare the event ElemDelete.Post */
                  notifyEl.event = TteElemDelete;
                  notifyEl.document = doc;
                  notifyEl.element = (Element) (pSibling->ElParent);
                  notifyEl.info = 0; /* not sent by undo */
                  notifyEl.elementType.ElTypeNum = pSibling->ElTypeNumber;
                  notifyEl.elementType.ElSSchema =
                    (SSchema) (pSibling->ElStructSchema);
                  nSiblings = 0;
                  pS = pSibling;
                  while (pS->ElPrevious != NULL)
                    {
                      nSiblings++;
                      pS = pS->ElPrevious;
                    }
                  notifyEl.position = nSiblings;
                }
              pSuccessor = NextElement (pSibling);
              /* record the element to be deleted in the history */
              AddEditOpInHistory (pSibling, pDoc, TRUE, FALSE);
              /* remove the element from the tree */
              RemoveElement (pSibling);
              UpdateNumbers (pSuccessor, pSibling, pDoc, TRUE);
              RedisplayCopies (pSibling, pDoc, TRUE);
              if (!isRow)
                {
                  /* send the event ElemDelete.Post to the application */
                  CallEventType ((NotifyEvent *) (&notifyEl), FALSE);
                  /* send the event ElemPaste.Pre to the application */
                  notifyVal.event = TteElemPaste;
                  notifyVal.document = doc;
                  notifyVal.element = (Element) (pElem->ElParent);
                  notifyVal.target = (Element) pSibling;
                  nSiblings = 1;
                  pS = pElem;
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
              if (ok || (pSibling->ElVolume == 0))
                /* the application refuses to paste this element or this
                   element is empty, free it */
                DeleteElement (&pSibling, pDoc);
              else if (pElem && pElem->ElParent != pSibling)
                {
                  /* application accepts */
                  /* inserts the element at its new position */
                  if (before)
                    InsertElementBefore (pElem, pSibling);
                  else
                    InsertElementAfter (pElem, pSibling);
                  CreateAllAbsBoxesOfEl (pSibling, pDoc);
                  /* record the inserted element in the history */
                  AddEditOpInHistory (pSibling, pDoc, FALSE, TRUE);
                  if (!isRow)
                    NotifySubTree (TteElemPaste, pDoc, pSibling, 0, 0, FALSE, FALSE);
                }
            }
          if (pSibling)
            pElem = pSibling;
          /* passe a l'element suivant */
          if (pNext == NULL)
            pSibling = NULL;
          else if (AllowedSibling (pElem, pDoc, pNext->ElTypeNumber,
                                   pNext->ElStructSchema, FALSE, FALSE,FALSE))
            pSibling = pNext;
          else
            pSibling = NULL;
        }
      TtaFreeMemory (list);

      /* remove all elements that are now empty */
      pPrev = NULL;
      while (pParent && pParent->ElFirstChild == NULL)
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
              pPrev = PreviousNotPage (pE, FALSE);
              DestroyAbsBoxes (pE, pDoc, TRUE);
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
      AbstractImageUpdated (pDoc);
      if (pPrev != NULL)
        /* verifie si l'element precedent devient dernier parmi */
        /* ses freres */
        ProcessFirstLast (pPrev, NULL, pDoc);
      AbstractImageUpdated (pDoc);
      RedisplayDocViews (pDoc);
      /* si on est dans un element copie' par inclusion, on met a jour
         les copies de cet element. */
      pE = pElem->ElParent;
      RedisplayCopies (pE, pDoc, TRUE);
      /* indique que le document est modifie' */
      SetDocumentModified (pDoc, TRUE, 30);

      /* set the selection */
      if (pSel && pSel->ElStructSchema)
        {
          if (pSel->ElVolume > 0)
            /* the first element moved is not empty. Select its first
               or last character, depending on "selHead" */
            {
              pSel = FirstLeaf (pSel);
              if (!pSel->ElTerminal)
                SelectElement (pDoc, pSel, TRUE, TRUE, TRUE);
              else if (pSel->ElLeafType == LtText)
                {
                if (selHead)
                  MoveCaret (pDoc, pSel, 1);
                else
                  MoveCaret (pDoc, pSel, pSel->ElTextLength + 1);
                }
              else if (pSel->ElLeafType == LtPicture)
                {
                if (selHead)
                  MoveCaret (pDoc, pSel, 0);
                else
                  MoveCaret (pDoc, pSel, 1);
                }
              else if (pSel->ElLeafType != LtPairedElem)
                SelectElement (pDoc, pSel, TRUE, TRUE, TRUE);
              else if (pSel->ElPrevious != NULL)
                {
                if (pSel->ElPrevious->ElTerminal &&
                    pSel->ElPrevious->ElLeafType == LtText)
                  MoveCaret (pDoc, pSel->ElPrevious,
                             pSel->ElPrevious->ElTextLength + 1);
                else
                  SelectElement (pDoc, pSel->ElPrevious, TRUE, TRUE, TRUE);
                }
              else if (pSel->ElNext != NULL)
                {
                if (pSel->ElNext->ElTerminal &&
                    pSel->ElNext->ElLeafType == LtText)
                  MoveCaret (pDoc, pSel->ElNext, 1);
                else
                  SelectElement (pDoc, pSel->ElNext, TRUE, TRUE, TRUE);
                }
              else
                SelectElement (pDoc, pSel->ElParent, TRUE, TRUE, TRUE);
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
                    SelectElement (pDoc, pSel, TRUE, TRUE, TRUE);
                }
              else if (pSel->ElNext != NULL)
                {
                  pSel = FirstLeaf (pSel->ElNext);
                  if (pSel->ElTerminal && pSel->ElLeafType == LtText)
                    MoveCaret (pDoc, pSel, 1);
                  else
                    SelectElement (pDoc, pSel, TRUE, TRUE, TRUE);
                }
              else
                SelectElement (pDoc, pSel, TRUE, TRUE, TRUE);
            }
        }
      /* end of command: close history sequence */
      if (!histOpen)
        CloseHistorySequence (pDoc);
    }
}

/*----------------------------------------------------------------------
  NoStructureLoadResources
  connects unstructured editing and selection functions.
  ----------------------------------------------------------------------*/
void NoStructSelectLoadResources ()
{
  if (MenuActionList[CMD_CreateElement].Call_Action == NULL)
    {
      MenuActionList[CMD_CreateElement].Call_Action = (Proc)TtcCreateElement;
      InitSelection ();
    }
}
