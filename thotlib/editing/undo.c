/*
 *
 *  (c) COPYRIGHT INRIA 1998-2009
 *  Please read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * This module handles commands Undo and Redo
 *
 * Authors: V. Quint (INRIA)
 *          I. Vatton (INRIA)
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
#include "selection.h"

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "constres.h"
#include "creation_tv.h"
#include "modif_tv.h"
#include "select_tv.h"
#include "edit_tv.h"
#include "appdialogue_tv.h"

/* maximum number of editing operations recorded in the Undo or Redo queue */
#define MAX_EDIT_HISTORY_LENGTH 20
// When the lock is set, the history is not registered (Resizing, Moving)
static ThotBool Lock_History = FALSE;

#include "actions_f.h"
#include "appdialogue_f.h"
#include "applicationapi_f.h"
#include "attributes_f.h"
#include "callback_f.h"
#include "contentapi_f.h"
#include "displayview_f.h"
#include "documentapi_f.h"
#include "memory_f.h"
#include "structcommands_f.h"
#include "structcreation_f.h"
#include "structselect_f.h"
#include "tableH_f.h"
#include "tree_f.h"
#include "viewapi_f.h"

/*----------------------------------------------------------------------
  TtaSetWithHistory lock or unlock history management 
  (used when resizing or moving an object)
  ----------------------------------------------------------------------*/
void TtaLockHistory (ThotBool status)
{
  Lock_History = status;
}

/*----------------------------------------------------------------------
  HistError
  ----------------------------------------------------------------------*/
static void HistError (int errorCode)
{
#ifdef THOT_DEBUG
  fprintf (stderr, "**** Undo error %d ****\n", errorCode);
#endif /* THOT_DEBUG */
}

/*----------------------------------------------------------------------
  UpdateHistoryLength
  Add diff to variable pDoc->DocNbEditsInHistory
  ----------------------------------------------------------------------*/
static void UpdateHistoryLength (int diff, PtrDocument pDoc)
{
  if (pDoc->DocNbEditsInHistory == 0 && diff > 0)
    /* enable Undo command */
    SwitchUndo (pDoc, TRUE);
  pDoc->DocNbEditsInHistory += diff;
  if (pDoc->DocNbEditsInHistory == 0 && diff < 0)
    /* disable Undo command */
    SwitchUndo (pDoc, FALSE);
}

/*----------------------------------------------------------------------
  UpdateRedoLength
  Add diff to variable pDoc->DocNbUndone
  ----------------------------------------------------------------------*/
static void UpdateRedoLength (int diff, PtrDocument pDoc)
{
  if (pDoc->DocNbUndone == 0 && diff > 0)
    /* enable Redo command */
    SwitchRedo (pDoc, TRUE);
  pDoc->DocNbUndone += diff;
  if (pDoc->DocNbUndone == 0 && diff < 0)
    /* disable Redo command */
    SwitchRedo (pDoc, FALSE);
}

/*----------------------------------------------------------------------
  UnchainLatestOp
  Remove the last editing operation from the undo or redo queue
  (depending on parameter undo) and return it.
  ----------------------------------------------------------------------*/
static PtrEditOperation UnchainLatestOp (PtrDocument pDoc, ThotBool undo)
{
  PtrEditOperation	*editOp, last;

  if (undo)
    editOp = &(pDoc->DocLastEdit);
  else
    editOp = &(pDoc->DocLastUndone);
  last = *editOp;
  *editOp = (*editOp)->EoPreviousOp;
  if (*editOp)
    (*editOp)->EoNextOp = NULL;
  last->EoPreviousOp = NULL;
  return last;
}

/*----------------------------------------------------------------------
  CancelAnEdit
  Remove and delete an editing operation from the history
  ----------------------------------------------------------------------*/
static void CancelAnEdit (PtrEditOperation editOp, PtrDocument pDoc,
                          ThotBool undo)
{
  PtrElement		pEl;
  PtrAttribute         pAttr;
  PtrEditOperation	prevOp;

  /* Error if there is no current history */
  if ((undo && !pDoc->DocLastEdit) || (!undo && !pDoc->DocLastUndone))
    {
      HistError (1);
      return;
    }
  /* unchain the operation descriptor */
  if (editOp == pDoc->DocLastEdit)
    pDoc->DocLastEdit = pDoc->DocLastEdit->EoPreviousOp;
  else if (editOp == pDoc->DocLastUndone)
    pDoc->DocLastUndone = pDoc->DocLastUndone->EoPreviousOp;
  if (editOp->EoNextOp)
    editOp->EoNextOp->EoPreviousOp = editOp->EoPreviousOp;
  if (editOp->EoPreviousOp)
    editOp->EoPreviousOp->EoNextOp = editOp->EoNextOp;
  if (editOp->EoType == EtElement)
    {
      pEl = editOp->EoSavedElement;
      if (pEl)
        /* if the saved selection is in the freed element, cancel it */
        {
          /* first, get the delimiter that contains the selection */
          prevOp = editOp;
          do
            prevOp = prevOp->EoPreviousOp;
          while (prevOp && prevOp->EoType != EtDelimiter);
          /* then, check that selection */
          if (prevOp)
            if ((prevOp->EoFirstSelectedEl &&
                 ElemIsWithinSubtree (prevOp->EoFirstSelectedEl, pEl)) ||
                (prevOp->EoLastSelectedEl &&
                 ElemIsWithinSubtree (prevOp->EoLastSelectedEl, pEl)))
              {
                prevOp->EoFirstSelectedEl = NULL;
                prevOp->EoLastSelectedEl = NULL;
                prevOp->EoColumnSelected = FALSE;
              }
          /* free the saved element */
          DeleteElement (&pEl, pDoc);
        }
      editOp->EoSavedElement = NULL;
    }
  else if (editOp->EoType == EtAttribute)
    {
      pAttr = editOp->EoSavedAttribute;
      if (pAttr)
        /* free the saved attribute */
        DeleteAttribute (NULL, pAttr);
      editOp->EoSavedAttribute = NULL;
    }
  else if (editOp->EoType == EtChangeType)
    {
      editOp->EoChangedElement = NULL;
      editOp->EoElementType = 0;
    }
  else if (editOp->EoType == EtDelimiter)
    /* update the number of editing sequences remaining in the queue */
    {
      if (undo)
        {
          if (editOp->EoInitialSequence)
            {
              /* That's the first sequence registered since the document was loaded
                 or saved. The document is no longer modified */
              SetDocumentModified (pDoc, FALSE, 0);
              pDoc->DocUpdated = TRUE;
            }
          UpdateHistoryLength (-1, pDoc);
        }
      else
        UpdateRedoLength (-1, pDoc);
    }

  /* free the editing operation descriptor */
  TtaFreeMemory (editOp);
  editOp = NULL;
}

/*----------------------------------------------------------------------
  ClearRedoQueue
  Clear the Redo queue of document pDoc
  ----------------------------------------------------------------------*/
static void ClearRedoQueue (PtrDocument pDoc)
{
  PtrEditOperation editOp, previousEditOp;

  /* free all editing operations recorded in the Redo queue */
  editOp = pDoc->DocLastUndone;
  while (editOp)
    {
      previousEditOp = editOp->EoPreviousOp;
      CancelAnEdit (editOp, pDoc, FALSE);
      editOp = previousEditOp;
    }
  /* reiniatilize all variables representing the Redo queue */
  pDoc->DocLastUndone = NULL;
  pDoc->DocNbUndone = 0;
  /* disable the Redo command */
  SwitchRedo (pDoc, FALSE);
}

/*----------------------------------------------------------------------
  ClearHistory
  Clear the editing history of document pDoc
  ----------------------------------------------------------------------*/
void ClearHistory (PtrDocument pDoc)
{
  PtrEditOperation editOp, previousEditOp;

  /* free all editing operations recorded in the Undo queue */
  editOp = pDoc->DocLastEdit;
  while (editOp)
    {
      previousEditOp = editOp->EoPreviousOp;
      CancelAnEdit (editOp, pDoc, TRUE);
      editOp = previousEditOp;
    }
  /* reiniatilize all variables representing the Undo queue */
  pDoc->DocLastEdit = NULL;
  pDoc->DocNbEditsInHistory = 0;
  pDoc->DocEditSequence = FALSE;
  /* disable the Undo command */
  SwitchUndo (pDoc, FALSE);
  /* Clear the Redo queue */
  ClearRedoQueue (pDoc);
}

/*----------------------------------------------------------------------
  ChangePointersOlderEdits
  If Op and older editing operations in the history refer to elements
  that have been copied from subtree pTree, change these reference to the
  corresponding copies
  If the selection saved in Op refers to elements that have been copied
  from subtree pTree, change the saved selection to the corresponding copies.
  ----------------------------------------------------------------------*/
static void ChangePointersOlderEdits (PtrEditOperation Op, PtrElement pTree)
{
  PtrEditOperation	editOp, prevOp;

  editOp = Op->EoPreviousOp;
  while (editOp)
    {
      if (editOp->EoType == EtElement)
        {
          if (editOp->EoParent &&
              ElemIsWithinSubtree(editOp->EoParent, pTree))
            editOp->EoParent = editOp->EoParent->ElCopy;
          if (editOp->EoPreviousSibling &&
              ElemIsWithinSubtree(editOp->EoPreviousSibling, pTree))
            editOp->EoPreviousSibling = editOp->EoPreviousSibling->ElCopy;
          if (editOp->EoCreatedElement &&
              ElemIsWithinSubtree(editOp->EoCreatedElement, pTree))
            editOp->EoCreatedElement = editOp->EoCreatedElement->ElCopy;
        }
      else if (editOp->EoType == EtDelimiter)
        {
          if (editOp->EoFirstSelectedEl &&
              ElemIsWithinSubtree(editOp->EoFirstSelectedEl, pTree))
            editOp->EoFirstSelectedEl = editOp->EoFirstSelectedEl->ElCopy;
          if (editOp->EoLastSelectedEl &&
              ElemIsWithinSubtree(editOp->EoLastSelectedEl, pTree))
            editOp->EoLastSelectedEl = editOp->EoLastSelectedEl->ElCopy;
        }
      else if (editOp->EoType == EtAttribute)
        {
          if (editOp->EoElement &&
              ElemIsWithinSubtree(editOp->EoElement, pTree))
            editOp->EoElement = editOp->EoElement->ElCopy;
        }
      else if (editOp->EoType == EtChangeType)
        {
          if (editOp->EoChangedElement &&
              ElemIsWithinSubtree(editOp->EoChangedElement, pTree))
            editOp->EoChangedElement = editOp->EoChangedElement->ElCopy;
        }
      editOp = editOp->EoPreviousOp;
    }

  if (Op->EoType == EtElement || editOp->EoType == EtChangeType)
    {
      /* if the current selection is in the copied element, set the saved
         selection to the equivalent elements in the copy */
      /* first, get the delimiter that contains the selection */
      prevOp = Op;
      do
        prevOp = prevOp->EoPreviousOp;
      while (prevOp && prevOp->EoType != EtDelimiter);
      /* then check that selection */
      if (prevOp)
        {
          if (prevOp->EoFirstSelectedEl)
            if (ElemIsWithinSubtree (prevOp->EoFirstSelectedEl, pTree))
              prevOp->EoFirstSelectedEl = prevOp->EoFirstSelectedEl->ElCopy;
          if (prevOp->EoLastSelectedEl)
            if (ElemIsWithinSubtree (prevOp->EoLastSelectedEl, pTree))
              prevOp->EoLastSelectedEl = prevOp->EoLastSelectedEl->ElCopy;
        }
    }
}

/*----------------------------------------------------------------------
  AddEditOpInHistory
  Register a single editing operation in the editing history.
  pEl: the element that has been (or will be) changed or deleted by the
  editing operation.
  pDoc: the document to which this element belongs.
  save: a copy of element pEl must be saved in order to allow it to be
  restored when the operation will be undone.
  removeWhenUndoing: element pEl to must be deleted when the operation will
  be undone.
  ----------------------------------------------------------------------*/
void AddEditOpInHistory (PtrElement pEl, PtrDocument pDoc, ThotBool save,
                         ThotBool removeWhenUndoing)
{
  PtrEditOperation	editOp;
  PtrElement		pCopy;

  if (!pEl)
    return;
  if (!pEl->ElStructSchema)
    return;
  /* error if no sequence open */
  if (!pDoc->DocEditSequence)
    {
      HistError (2);
      return;
    }
  /* create a new operation descriptor in the history */
  editOp = (PtrEditOperation) TtaGetMemory (sizeof (EditOperation));
  /* link the new operation descriptor in the history */
  editOp->EoPreviousOp = pDoc->DocLastEdit;
  if (pDoc->DocLastEdit)
    pDoc->DocLastEdit->EoNextOp = editOp;
  pDoc->DocLastEdit = editOp;
  editOp->EoNextOp = NULL;
  editOp->EoType = EtElement;
  editOp->EoInfo = 1;
  /* record the location in the abstract tree concerned by the operation */
  editOp->EoParent = pEl->ElParent;
  editOp->EoPreviousSibling = pEl->ElPrevious;
  if (removeWhenUndoing)
    editOp->EoCreatedElement = pEl;
  else
    editOp->EoCreatedElement = NULL;
  editOp->EoSavedElement = NULL;

  if (save)
    /* copy the element concerned by the operation and attach it to the
       operation descriptor */
    {
      /* do the copy */
      pCopy = CopyTree (pEl, pDoc, pEl->ElStructSchema, pDoc, pEl->ElParent,
                        FALSE, FALSE, TRUE, TRUE, FALSE);
      /* store the copy in the editing operation descriptor */
      editOp->EoSavedElement = pCopy;
      /* if older editing operations in the history refer to elements that
         have been copied, change these references to the copies */
      ChangePointersOlderEdits (editOp, pEl);
    }
}

/*----------------------------------------------------------------------
  AddChangeTypeOpInHistory
  Register a single editing operation in the editing history.
  pEl: the element that has been changed by the editing operation.
  pDoc: the document to which this element belongs.
  elType: the element type to be restored when the operation will be undone.
  ----------------------------------------------------------------------*/
void AddChangeTypeOpInHistory (PtrElement pEl, int elType, PtrDocument pDoc)
{
  PtrEditOperation	editOp;

  if (!pEl)
    return;
  if (!pEl->ElStructSchema)
    return;
  /* error if no sequence open */
  if (!pDoc->DocEditSequence)
    {
      HistError (2);
      return;
    }
  /* create a new operation descriptor in the history */
  editOp = (PtrEditOperation) TtaGetMemory (sizeof (EditOperation));
  /* link the new operation descriptor in the history */
  editOp->EoPreviousOp = pDoc->DocLastEdit;
  if (pDoc->DocLastEdit)
    pDoc->DocLastEdit->EoNextOp = editOp;
  pDoc->DocLastEdit = editOp;
  editOp->EoNextOp = NULL;
  editOp->EoType = EtChangeType;
  editOp->EoChangedElement = pEl;
  editOp->EoElementType = elType;
}

/*----------------------------------------------------------------------
  ChangeAttrPointersOlderEdits
  If Op and older editing operations in the history refer to attribute
  pAttr, change these reference to the corresponding copy pCopyAttr.
  ----------------------------------------------------------------------*/
static void ChangeAttrPointersOlderEdits (PtrEditOperation Op,
                                          PtrAttribute pAttr, PtrAttribute pCopyAttr)
{
  PtrEditOperation	editOp;

  editOp = Op->EoPreviousOp;
  while (editOp)
    {
      if (editOp->EoType == EtAttribute)
        {
          if (editOp->EoCreatedAttribute)
            if (editOp->EoCreatedAttribute == pAttr)
              editOp->EoCreatedAttribute = pCopyAttr;
        }
      editOp = editOp->EoPreviousOp;
    }
}

/*----------------------------------------------------------------------
  AddAttrEditOpInHistory
  Register in the editing history a single editing operation for an
  attribute
  pAttr: the attribute that has been (or will be) changed or deleted by the
  editing operation.
  pEl: the element to wich the attribute is associated
  pDoc: the document to which this element belongs.
  save: a copy of attribute pAttr must be saved in order to allow it to be
  restored when the operation will be undone.
  removeWhenUndoing: attribute pAttr to must be deleted when the operation
  will be undone.
  ----------------------------------------------------------------------*/
void AddAttrEditOpInHistory (PtrAttribute pAttr, PtrElement pEl,
                             PtrDocument pDoc, ThotBool save,
                             ThotBool removeWhenUndoing)
{
  PtrEditOperation	editOp = NULL;
  PtrAttribute		pCopy, pElAttr;
  int                  rank;
  ThotBool		done;

  if (!pEl && !pAttr)
    return;
  /* noting to to if no sequence open */
  if (!pDoc->DocEditSequence)
    return;

  /* if the previous operation recorded is about the same attribute for the
     same element, just modify the previous operation */
  done = FALSE;
  if (pDoc->DocLastEdit)
    {
      editOp = pDoc->DocLastEdit;
      if (editOp->EoType == EtAttribute && editOp->EoElement == pEl)
        /* the operation previously recorded is related to an attribute for
           the same element */
        {
          if (editOp->EoCreatedAttribute && !editOp->EoSavedAttribute &&
              editOp->EoCreatedAttribute->AeAttrSSchema == pAttr->AeAttrSSchema &&
              editOp->EoCreatedAttribute->AeAttrNum == pAttr->AeAttrNum)
            /* the previous operation was the creation of this attribute */
            {
              if (!save)
                /* creating the same attribute again? Ignore */
                done = TRUE;
              else if (removeWhenUndoing)
                /* replacing the attribute. No need to record it */
                done = TRUE;
              else
                /* deleting the same attribute. Remove the previous record
                   about creating the attribute */
                {
                  pDoc->DocLastEdit = editOp->EoPreviousOp;
                  editOp->EoPreviousOp->EoNextOp = NULL;
                  editOp->EoElement = NULL;
                  editOp->EoCreatedAttribute = NULL;
                  TtaFreeMemory (editOp);
                  done = TRUE;
                }
            }
          else if (editOp->EoSavedAttribute && !editOp->EoCreatedAttribute &&
                   editOp->EoSavedAttribute->AeAttrSSchema == pAttr->AeAttrSSchema &&
                   editOp->EoSavedAttribute->AeAttrNum == pAttr->AeAttrNum)
            /* the previous operation was the deletion of this attribute */
            {
              if (!removeWhenUndoing)
                /* deleting the same attribute again? Ignore */
                done = TRUE;
              else if (save)
                /* replacing a deleted attribute? Ignore */
                done = TRUE;
              else
                /* creating the same attribute */
                {
                  editOp->EoCreatedAttribute = pAttr;
                  done = TRUE;
                }
            }
          else if (editOp->EoSavedAttribute && editOp->EoCreatedAttribute &&
                   editOp->EoSavedAttribute->AeAttrSSchema == pAttr->AeAttrSSchema &&
                   editOp->EoSavedAttribute->AeAttrNum == pAttr->AeAttrNum)
            /* the previous operation was a replacement of this attribute */
            {
              if (!save)
                /* creating the same attribute? Ignore */
                done = TRUE;
              else if (removeWhenUndoing)
                /* replacing this attribute again */
                {
                  editOp->EoCreatedAttribute = pAttr;
                  done = TRUE;
                }
              else
                /* deleting this attribute. It's like a creation */
                {
                  editOp->EoCreatedAttribute = NULL;
                  done = TRUE;
                }
            }
        }
    }

  if (!done)
    {
      /* create a new operation descriptor in the history */
      editOp = (PtrEditOperation) TtaGetMemory (sizeof (EditOperation));
      /* link the new operation descriptor in the history */
      editOp->EoPreviousOp = pDoc->DocLastEdit;
      if (pDoc->DocLastEdit)
        pDoc->DocLastEdit->EoNextOp = editOp;
      pDoc->DocLastEdit = editOp;
      editOp->EoNextOp = NULL;
      editOp->EoType = EtAttribute;
      editOp->EoElement = pEl;
      editOp->EoCreatedAttribute = NULL;
      editOp->EoSavedAttribute = NULL;
      /* get the rank of pAttr among the attributes associated with the
         element */
      pElAttr = pEl->ElFirstAttr;
      rank = 0;
      while (pElAttr && pElAttr != pAttr)
        {
          pElAttr = pElAttr->AeNext;
          rank++;
        }
      editOp->EoAttrRank = rank;
      if (removeWhenUndoing)
        editOp->EoCreatedAttribute = pAttr;
      if (save)
        /* copy the attribute concerned by the operation and attach it to the
           operation descriptor */
        {
          pCopy = AddAttrToElem (NULL, pAttr, NULL);
          editOp->EoSavedAttribute = pCopy;
          /* if older editing operations in the history refer to attribute
             that has been copied, change these references to the copy */
          ChangeAttrPointersOlderEdits (editOp, pAttr, pCopy);
        }
    }
}

/*----------------------------------------------------------------------
  ChangeLastRegisteredAttr
  Change the most recent editing operation registered in the editing history
  of document, only if it's an attribute operation for element oldEl.
  In that case, make it related to element newEl and attribute newAttr.
  ----------------------------------------------------------------------*/
void ChangeLastRegisteredAttr (PtrElement oldEl, PtrElement newEl, PtrAttribute oldAttr,
                               PtrAttribute newAttr, PtrDocument pDoc)
{
  ThotBool	done;

  done = FALSE;
  if (pDoc->DocLastEdit)
    if (pDoc->DocLastEdit->EoType == EtAttribute)
      if (pDoc->DocLastEdit->EoElement == oldEl)
        {
          pDoc->DocLastEdit->EoElement = newEl;
          if (newAttr)
            {
              pDoc->DocLastEdit->EoCreatedAttribute = newAttr;
              /* if older editing operations in the history refer to the
                 attribute that has been copied, change these references to the
                 copy */
              ChangeAttrPointersOlderEdits (pDoc->DocLastEdit, oldAttr,
                                            newAttr);
            }
          done = TRUE;
        }
  if (!done)
    if (pDoc->DocLastUndone)
      if (pDoc->DocLastUndone->EoType == EtAttribute)
        if (pDoc->DocLastUndone->EoElement == oldEl)
          {
            pDoc->DocLastUndone->EoElement = newEl;
            if (newAttr)
              {
                pDoc->DocLastUndone->EoCreatedAttribute = newAttr;
                /* if older editing operations in the history refer to the
                   attribute that has been copied, change these references to
                   the copy */
                ChangeAttrPointersOlderEdits (pDoc->DocLastUndone, oldAttr,
                                              newAttr);
              }
            done = TRUE;
          }
}

/*----------------------------------------------------------------------
  ReplaceLastRegisteredAttr
 
  Replace the latest operation registered in the editing history of document:
  an attribute value modification becomes an attribute value deletion.
  ----------------------------------------------------------------------*/
void ReplaceLastRegisteredAttr (PtrDocument pDoc)
{
  if (pDoc->DocLastEdit)
    if (pDoc->DocLastEdit->EoType == EtAttribute)
      if (pDoc->DocLastEdit->EoSavedAttribute)
        if (pDoc->DocLastEdit->EoCreatedAttribute)
          pDoc->DocLastEdit->EoCreatedAttribute = NULL;
}

/*----------------------------------------------------------------------
  ChangeInfoLastRegisteredElem

  If the latest operation registered for element pDoc is for an element
  set the info field of this operation the the new value provided.
  ----------------------------------------------------------------------*/
void ChangeInfoLastRegisteredElem (PtrDocument pDoc, int newInfo)
{
  if (pDoc->DocLastEdit)
    if (pDoc->DocLastEdit->EoType == EtElement)
      pDoc->DocLastEdit->EoInfo = newInfo;
}

/*----------------------------------------------------------------------
  CancelLastEditFromHistory
  Cancel the most recent editing operation registered in the editing history.
  ----------------------------------------------------------------------*/
void CancelLastEditFromHistory (PtrDocument pDoc)
{
  if (!pDoc->DocLastEdit)
    /* history empty. Error */
    {
      HistError (6);
      return;
    }
  if (pDoc->DocLastEdit->EoType == EtElement)
    {
      /* change the pointers in older edits that refer to the saved elements
         that will be released */
      if (pDoc->DocLastEdit->EoSavedElement)
        ChangePointersOlderEdits (pDoc->DocLastEdit,
                                  pDoc->DocLastEdit->EoSavedElement);
    }
  else if (pDoc->DocLastEdit->EoType != EtAttribute &&
           pDoc->DocLastEdit->EoType != EtChangeType)
    /* Not an operation on elements or attributes */
    return;

  /* Remove the latest operation descriptor */
  CancelAnEdit (pDoc->DocLastEdit, pDoc, TRUE);
}

/*----------------------------------------------------------------------
  CancelLastAttrOperationFromHistory
  Cancel the most recent attribute change registered in the editing history.
  ----------------------------------------------------------------------*/
void CancelLastAttrOperationFromHistory (PtrDocument pDoc)
{
  if (!pDoc->DocLastEdit)
    /* history empty. Error */
    {
      HistError (6);
      return;
    }
  if (pDoc->DocLastEdit->EoType == EtAttribute)
    /* Remove the latest operation descriptor */
    CancelAnEdit (pDoc->DocLastEdit, pDoc, TRUE);
}

/*----------------------------------------------------------------------
  GetLastCreatedElemInHistory
  If the last operation recorded in the history is the creation of an
  element, return that element, otherwise return NULL.
  ----------------------------------------------------------------------*/
PtrElement GetLastCreatedElemInHistory (PtrDocument pDoc)
{
  PtrElement    pEl;

  pEl = NULL;
  if (pDoc->DocLastEdit)
    /* history is not empty */
    if (pDoc->DocLastEdit->EoType == EtElement)
      /* the last item record an element */
      if (!pDoc->DocLastEdit->EoSavedElement)
        pEl = pDoc->DocLastEdit->EoCreatedElement;
  return pEl;
}

/*----------------------------------------------------------------------
  CancelOldestSequence

  Cancel the oldest sequence in the Undo (if undo==TRUE) or Redo queue.
  ----------------------------------------------------------------------*/
static void CancelOldestSequence (PtrDocument pDoc, ThotBool undo)
{
  PtrEditOperation    editOp, nextOp;

  if (undo)
    editOp = pDoc->DocLastEdit;
  else
    editOp = pDoc->DocLastUndone;
  /* get the oldest descriptor */
  while (editOp->EoPreviousOp)
    editOp = editOp->EoPreviousOp;
  /* delete the oldest descriptor and all following descriptors up to the
     first Delimiter (excluded) */
  do
    {
      nextOp = editOp->EoNextOp;
      CancelAnEdit (editOp, pDoc, undo);
      editOp = nextOp;
      if (editOp && editOp->EoType == EtDelimiter)
        editOp = NULL;
    }
  while (editOp);
}

/*----------------------------------------------------------------------
  CheckUniqueInit
  If Op is a Delimiter considered as the beginning of an initial sequence,
  make sure that there is no other Delimiter in the list that is considered
  as the beginning of an initial sequence.
  ----------------------------------------------------------------------*/
static void CheckUniqueInit (PtrEditOperation Op)
{
  PtrEditOperation	editOp;

  if (Op->EoType != EtDelimiter || !Op->EoInitialSequence)
    return;

  editOp = Op->EoPreviousOp;
  while (editOp)
    {
      if (editOp->EoType == EtDelimiter)
        editOp->EoInitialSequence = FALSE;
      editOp = editOp->EoPreviousOp;
    }
}

/*----------------------------------------------------------------------
  NewInitialSequence
  Mark the current sequence as the initial sequence.
  ----------------------------------------------------------------------*/
void NewInitialSequence (PtrDocument pDoc)
{
  PtrEditOperation	editOp;

  if (pDoc->DocLastEdit)
    {
      editOp = pDoc->DocLastEdit;
      if (editOp && editOp->EoType == EtDelimiter)
        editOp->EoInitialSequence = TRUE;
      editOp = editOp->EoPreviousOp;
      while (editOp)
        {
          if (editOp->EoType == EtDelimiter)
            editOp->EoInitialSequence = FALSE;
          editOp = editOp->EoPreviousOp;
        }
    }
  if (pDoc->DocLastUndone)
    {
      editOp = pDoc->DocLastUndone;
      if (editOp && editOp->EoType == EtDelimiter)
        editOp->EoInitialSequence = TRUE;
      editOp = editOp->EoPreviousOp;
      while (editOp)
        {
          if (editOp->EoType == EtDelimiter)
            editOp->EoInitialSequence = FALSE;
          editOp = editOp->EoPreviousOp;
        }
    }
}

/*----------------------------------------------------------------------
  OpenHistorySequence
  Open a sequence of editing operations in the history.
  firstSel, lastSel, firstSelChar, lastSelChar: indicate the selection
  that must be set when the operation will be undone.
  ----------------------------------------------------------------------*/
void OpenHistorySequence (PtrDocument pDoc, PtrElement firstSel, PtrElement lastSel,
                          PtrAttribute attr, int firstSelChar, int lastSelChar)
{
  PtrEditOperation	editOp;

  /* can not open a sequence if a sequence is already open */
  if (pDoc->DocEditSequence)
    {
      if (!Lock_History)
        HistError (8); // accept multiple open
      return;
    }
  pDoc->DocEditSequence = TRUE;

  /* create a new operation descriptor in the history */
  editOp = (PtrEditOperation) TtaGetMemory (sizeof (EditOperation));
  /* link the new operation descriptor in the history */
  editOp->EoPreviousOp = pDoc->DocLastEdit;
  if (pDoc->DocLastEdit)
    pDoc->DocLastEdit->EoNextOp = editOp;
  pDoc->DocLastEdit = editOp;
  editOp->EoNextOp = NULL;
  editOp->EoType = EtDelimiter;
  editOp->EoFirstSelectedEl = firstSel;
  if (TtaIsGraphics ((Element)firstSel))
    // don't register the specific point
    editOp->EoFirstSelectedChar = 0;
  else
    editOp->EoFirstSelectedChar = firstSelChar;
  editOp->EoLastSelectedEl = lastSel;
  if (TtaIsGraphics ((Element)lastSel))
    // don't register the specific point
    editOp->EoLastSelectedChar = 0;
  else
    editOp->EoLastSelectedChar = lastSelChar;
  editOp->EoColumnSelected = WholeColumnSelected;
  if (attr)
    {
      editOp->EoSelectedAttrSch = attr->AeAttrSSchema;
      editOp->EoSelectedAttr = attr->AeAttrNum;
    }
  else
    {
      editOp->EoSelectedAttrSch = NULL;
      editOp->EoSelectedAttr = 0;
    }
  editOp->EoInitialSequence = !pDoc->DocModified;
  CheckUniqueInit (editOp);
  /* update the number of editing commands remaining in the history */
  UpdateHistoryLength (1, pDoc);
  /* If the history is too long, cancel the oldest sequence in the history */
  if (pDoc->DocNbEditsInHistory > MAX_EDIT_HISTORY_LENGTH)
    CancelOldestSequence (pDoc, TRUE);
}

/*----------------------------------------------------------------------
  CloseHistorySequence
  Close a sequence of editing operations in the history.
  return FALSE if the Sequence is empty.
  ----------------------------------------------------------------------*/
ThotBool CloseHistorySequence (PtrDocument pDoc)
{
  ThotBool	result;

  result = FALSE;
  if (pDoc)
    {
      /* error if no sequence open */
      if (!pDoc->DocEditSequence)
        HistError (9);
      else
        {
          if (pDoc->DocLastEdit && pDoc->DocLastEdit->EoType == EtDelimiter)
            /* empty sequence, remove it */
            CancelAnEdit (pDoc->DocLastEdit, pDoc, TRUE);
          else
            {
              result = TRUE;
              /* Clear the Redo queue */
              ClearRedoQueue (pDoc);
            }

          if (Lock_History)
            return result; // don't close

          /* sequence closed */
          pDoc->DocEditSequence = FALSE;
          /* Trigger the autosave procedure if the number of modifications has been reached */
          /* DocBackUpInterval = 0 means no automatic save */
          if (DocBackUpInterval > 0)
            {
              pDoc->DocNTypedChars += 1;
              if (pDoc->DocNTypedChars >= DocBackUpInterval)
                if (ThotLocalActions[T_autosave] != NULL)
                  {
                    (*(Proc1)ThotLocalActions[T_autosave]) ((void *)(Document) IdentDocument (pDoc));
                    pDoc->DocNTypedChars = 0;
                  }
            }
        } 
    }
  return result;
}

/*----------------------------------------------------------------------
  CancelLastSequenceFromHistory
  Cancel the last sequence of editing operations registered in the
  editing history of document.
  ----------------------------------------------------------------------*/
void CancelLastSequenceFromHistory (PtrDocument pDoc)
{
  ThotBool	stop;

  stop = FALSE;
  while (!stop)
    {
      if (!pDoc->DocLastEdit)
        /* history is empty */
        return;
      if (pDoc->DocLastEdit->EoType == EtDelimiter)
        stop = TRUE;
      CancelAnEdit (pDoc->DocLastEdit, pDoc, TRUE);
    }
}

/*----------------------------------------------------------------------
  AttachAttr
  Attach attribute pAttr to element pEl after the attribute of rank rank
  among all attributes already attached to pEl.
  ----------------------------------------------------------------------*/
static void AttachAttr (PtrElement pEl, PtrAttribute pAttr, int rank,
                        Document doc)
{
  PtrAttribute    pA;

  UndisplayInheritedAttributes (pEl, pAttr, doc, FALSE);
  if (rank <= 0 || pEl->ElFirstAttr == NULL)
    {
      pAttr->AeNext = pEl->ElFirstAttr;
      pEl->ElFirstAttr = pAttr;
    }
  else
    {
      pA = pEl->ElFirstAttr;
      rank --;
      while (pA->AeNext != NULL && rank)
        {
          pA = pA->AeNext;
          rank --;
        }
      pAttr->AeNext = pA->AeNext;
      pA->AeNext = pAttr;
    }
  pAttr->AeDefAttr = FALSE;
  if (pAttr->AeAttrType == AtReferenceAttr)
    if (pAttr->AeAttrReference != NULL)
      pAttr->AeAttrReference->RdElement = pEl;
  DisplayAttribute (pEl, pAttr, doc);
}

/*----------------------------------------------------------------------
  UndoOperation

  Undo the latest editing operation registered in the Undo (if undo==TRUE)
  or Redo (if redo==FALSE) queue of document doc.
  If reverse, the editing operation descriptor will decribe the reverse
  editing operation when returning.
  ----------------------------------------------------------------------*/
static void UndoOperation (ThotBool undo, Document doc, ThotBool reverse)
{
  PtrEditOperation	editOp;
  PtrElement		pEl, pSibling,
    newParent, newPreviousSibling, newCreatedElement,
    newSavedElement;
  PtrAttribute          SavedAttribute, CreatedAttribute, pAttr;
  PtrDocument		pDoc;
  PtrEditOperation	queue;
  AttributeType         attrType;
  NotifyElement	        notifyEl;
  NotifyOnValue         notifyGraph;
  NotifyAttribute	      notifyAttr;
  int			              i, nSiblings, newType;
  ThotBool              replacePoly, status = FALSE;

  newParent = NULL;
  newPreviousSibling = NULL;
  newCreatedElement = NULL;
  newSavedElement = NULL;
  SavedAttribute = NULL;
  CreatedAttribute = NULL;
  newType = 0;
  pDoc = LoadedDocument [doc - 1];
  if (undo)
    editOp = pDoc->DocLastEdit;
  else
    editOp = pDoc->DocLastUndone;
  if (editOp == NULL)
    return;

  if (editOp->EoType == EtDelimiter)
    /* end of a sequence */
    {
      /* enable structure checking */
      TtaSetStructureChecking (TRUE, doc);
      /*TtaSetDisplayMode (doc, DisplayImmediately);*/
      /* set the selection that is recorded */
      if (editOp->EoFirstSelectedEl && editOp->EoLastSelectedEl)
        {
          /* Send events TteElemSelect.Pre */
          notifyEl.event = TteElemSelect;
          notifyEl.document = doc;
          notifyEl.element = (Element) (editOp->EoFirstSelectedEl);
          notifyEl.info = 1; /* sent by undo */
          notifyEl.elementType.ElTypeNum =
            editOp->EoFirstSelectedEl->ElTypeNumber;
          notifyEl.elementType.ElSSchema =
            (SSchema) (editOp->EoFirstSelectedEl->ElStructSchema);
          notifyEl.position = 0;
          CallEventType ((NotifyEvent *) & notifyEl, TRUE);
          if (editOp->EoSelectedAttrSch)
            {
              attrType.AttrSSchema = (SSchema)(editOp->EoSelectedAttrSch);
              attrType.AttrTypeNum = editOp->EoSelectedAttr;
              pAttr = (PtrAttribute) TtaGetAttribute ((Element)(editOp->EoFirstSelectedEl),
                                                      attrType);
              HighlightAttrSelection (LoadedDocument[doc - 1],
                                      editOp->EoFirstSelectedEl,
                                      pAttr,
                                      editOp->EoFirstSelectedChar,
                                      editOp->EoLastSelectedChar);
            }
          else if (editOp->EoFirstSelectedEl == editOp->EoLastSelectedEl &&
                   editOp->EoFirstSelectedEl->ElTerminal &&
                   editOp->EoFirstSelectedEl->ElLeafType == LtPicture)
            {
              i = editOp->EoFirstSelectedChar - 1;
              TtaSelectString (doc, (Element)(editOp->EoFirstSelectedEl),
                               editOp->EoFirstSelectedChar, i);              
            }
          else if (editOp->EoFirstSelectedChar > 0)
            {
              if (editOp->EoFirstSelectedEl == editOp->EoLastSelectedEl)
                i = editOp->EoLastSelectedChar;
              else
                i = TtaGetElementVolume ((Element)(editOp->EoFirstSelectedEl))
                  + 1;
              TtaSelectString (doc, (Element)(editOp->EoFirstSelectedEl),
                               editOp->EoFirstSelectedChar, i);
            }
          else
            TtaSelectElement (doc, (Element)(editOp->EoFirstSelectedEl));
          /* Send events TteElemSelect.Post */
          notifyEl.event = TteElemSelect;
          notifyEl.document = doc;
          notifyEl.element = (Element) (editOp->EoFirstSelectedEl);
          notifyEl.info = 1; /* sent by undo */
          notifyEl.elementType.ElTypeNum = editOp->EoFirstSelectedEl->ElTypeNumber;
          notifyEl.elementType.ElSSchema = (SSchema) (editOp->EoFirstSelectedEl->ElStructSchema);
          notifyEl.position = 0;
          CallEventType ((NotifyEvent *) & notifyEl, FALSE);
          if (editOp->EoFirstSelectedEl != editOp->EoLastSelectedEl)
            {
              /* Send event TteElemExtendSelect. Pre */
              notifyEl.event = TteElemExtendSelect;
              notifyEl.document = doc;
              notifyEl.element = (Element)(editOp->EoLastSelectedEl);
              notifyEl.info = 1; /* sent by undo */
              notifyEl.elementType.ElTypeNum = editOp->EoLastSelectedEl->ElTypeNumber;
              notifyEl.elementType.ElSSchema = (SSchema) (editOp->EoLastSelectedEl->ElStructSchema);
              notifyEl.position = 0;
              CallEventType ((NotifyEvent *) & notifyEl, TRUE);
              TtaExtendSelection (doc, (Element)(editOp->EoLastSelectedEl),
                                  editOp->EoLastSelectedChar);
              /* Send event TteElemExtendSelect. Post */
              notifyEl.event = TteElemExtendSelect;
              notifyEl.document = doc;
              notifyEl.info = 1; /* sent by undo */
              notifyEl.element = (Element)(editOp->EoLastSelectedEl);
              notifyEl.elementType.ElTypeNum = editOp->EoLastSelectedEl->ElTypeNumber;
              notifyEl.elementType.ElSSchema = (SSchema) (editOp->EoLastSelectedEl->ElStructSchema);
              notifyEl.position = 0;
              CallEventType ((NotifyEvent *) & notifyEl, FALSE);
            }
          if (editOp->EoColumnSelected && FirstSelectedColumn)
            /* select the whole column */
            TtaSelectEnclosingColumn ((Element)(editOp->EoFirstSelectedEl));
        }
      if (editOp->EoInitialSequence)
        {
          /* That's the first sequence registered since the document was loaded
             or saved. The document is no longer modified */
          SetDocumentModified (LoadedDocument[doc - 1], FALSE, 0);
          LoadedDocument[doc - 1]->DocUpdated = TRUE;
        }
    }
  else if (editOp->EoType == EtAttribute)
    {
      SavedAttribute = editOp->EoSavedAttribute;
      CreatedAttribute = editOp->EoCreatedAttribute;
      editOp->EoSavedAttribute = NULL;
      
      notifyAttr.document = doc;
      notifyAttr.element = (Element) (editOp->EoElement);
      notifyAttr.info = 1; /* sent by undo */
      /* delete the attribute that has to be removed from the element */
      if (editOp->EoElement && editOp->EoCreatedAttribute)
        {
          /* tell the application that an attribute will be removed */
          if (SavedAttribute)
            notifyAttr.event = TteAttrModify;	    
          else
            notifyAttr.event = TteAttrDelete;
          notifyAttr.attribute = (Attribute) (editOp->EoCreatedAttribute);
          notifyAttr.attributeType.AttrSSchema = (SSchema) (editOp->EoCreatedAttribute->AeAttrSSchema);
          notifyAttr.attributeType.AttrTypeNum = editOp->EoCreatedAttribute->AeAttrNum;
          status = CallEventAttribute (&notifyAttr, TRUE);
          if (reverse)
            {
              editOp->EoSavedAttribute = AddAttrToElem (NULL, editOp->EoCreatedAttribute, NULL);
              /* if older editing operations in the queue refer to the
                 attribute that has been copied, change these references to the
                 copy */
              if (undo)
                queue = pDoc->DocLastUndone;
              else
                queue = pDoc->DocLastEdit;
              ChangeAttrPointersOlderEdits (queue, editOp->EoCreatedAttribute,
                                            editOp->EoSavedAttribute);
            }
          /* remove the attribute */
          TtaRemoveAttribute ((Element) (editOp->EoElement),
                              (Attribute)(editOp->EoCreatedAttribute), doc);
          if (reverse)
            editOp->EoCreatedAttribute = NULL;
          if (!SavedAttribute)
            {
              notifyAttr.attribute = NULL;
              /* tell the application that an attribute has been removed */
              CallEventAttribute (&notifyAttr, FALSE);
            }
        }

      /* mark the document as modified */
      TtaSetDocumentModified (doc);

      /* put the saved attribute (if any) on the element */
      if (editOp->EoElement && SavedAttribute)
        {
          if (!CreatedAttribute)
            {
              /* tell the application that an attribute will be created */
              notifyAttr.event = TteAttrCreate;
              notifyAttr.attribute = NULL;
              notifyAttr.info = 1; /* sent by undo */
              notifyAttr.attributeType.AttrSSchema = (SSchema) (SavedAttribute->AeAttrSSchema);
              notifyAttr.attributeType.AttrTypeNum = SavedAttribute->AeAttrNum;
              CallEventAttribute (&notifyAttr, TRUE);
            }
          /* put the attribute on the element */
          AttachAttr (editOp->EoElement, SavedAttribute, editOp->EoAttrRank, doc);
          if (reverse)
            editOp->EoCreatedAttribute = SavedAttribute;
          /* tell the application that an attribute has been put */
          notifyAttr.attribute = (Attribute) (SavedAttribute);
          CallEventAttribute (&notifyAttr, FALSE);	    
        }
      /* Too late to mark the document as modified */
      //TtaSetDocumentModified (doc);
    }
  else if (editOp->EoType == EtElement)
    {
      /* delete the element that has to be removed from the abstract tree */
      if (reverse)
        {
          newParent = NULL;
          newPreviousSibling = NULL;
          newCreatedElement = NULL;
          newSavedElement = NULL;
        }
      replacePoly = FALSE;
      if (editOp->EoCreatedElement)
        {
          pEl = editOp->EoCreatedElement;
          if (editOp->EoSavedElement &&
              pEl->ElTerminal && pEl->ElLeafType == LtPolyLine &&
              editOp->EoSavedElement->ElTerminal &&
              editOp->EoSavedElement->ElLeafType == LtPolyLine)
            replacePoly = TRUE;
          /* tell the application that an element will be removed from the
             abstract tree. Last parameter indicates that this event comes
             from the undo/redo command. */
          if (editOp->EoInfo == 0)
            SendEventSubTree (TteElemDelete, pDoc, pEl, TTE_STANDARD_DELETE_LAST_ITEM,
                              1, FALSE, FALSE);
          else
            SendEventSubTree (TteElemDelete, pDoc, pEl, TTE_STANDARD_DELETE_LAST_ITEM,
                              editOp->EoInfo, FALSE, FALSE);
          if (pEl->ElStructSchema)
            {
              /* prepare event TteElemDelete to be sent to the application */
              notifyEl.event = TteElemDelete;
              notifyEl.document = doc;
              notifyEl.element = (Element) (pEl->ElParent);
              if (editOp->EoInfo == 0)
                notifyEl.info = 1; /* sent by undo */
              else
                notifyEl.info = editOp->EoInfo;
              notifyEl.elementType.ElTypeNum = pEl->ElTypeNumber;
              notifyEl.elementType.ElSSchema = (SSchema) (pEl->ElStructSchema);
              nSiblings = 0;
              pSibling = pEl;
              while (pSibling->ElPrevious != NULL)
                {
                  nSiblings++;
                  pSibling = pSibling->ElPrevious;
                }
              notifyEl.position = nSiblings;
              /* remove the element */
              if (!reverse)
                TtaDeleteTree ((Element)pEl, doc);
              else
                {
                  newPreviousSibling = pEl->ElPrevious;
                  newParent = pEl->ElParent;
                  newSavedElement = pEl;
                  TtaRemoveTree ((Element)pEl, doc);
                }
          /* tell the application that an element has been removed */
          CallEventType ((NotifyEvent *) (&notifyEl), FALSE);
            }
          editOp->EoCreatedElement = NULL;
        }

      /* mark the document as modified */
      TtaSetDocumentModified (doc);

      /* insert the saved element in the abstract tree */
      if (editOp->EoSavedElement)
        {
          pEl = editOp->EoSavedElement;
          if (editOp->EoPreviousSibling)
            TtaInsertSibling ((Element)(pEl),
                              (Element)(editOp->EoPreviousSibling),FALSE, doc);
          else
            TtaInsertFirstChild ((Element *)&(pEl),
                                 (Element)(editOp->EoParent), doc);
          /* send event ElemPaste.Post to the application. 1 means that this
             is not really a Paste operation but an Undo operation. */
          if (editOp->EoInfo == 0)
            NotifySubTree (TteElemPaste, pDoc, pEl, 0, 1, FALSE, FALSE);
          else
            NotifySubTree (TteElemPaste, pDoc, pEl, 0, editOp->EoInfo, FALSE, FALSE);
          /******/
          if (replacePoly)
            {
              notifyGraph.event = TteElemGraphModify;
              notifyGraph.document = doc;
              notifyGraph.element = (Element)(pEl->ElParent); /***** all ascendants ***/
              notifyGraph.target = (Element) pEl;
              notifyGraph.value = 0;   /******/
              CallEventType ((NotifyEvent *) & notifyGraph, FALSE);
            }
          /******/
          if (reverse)
            newCreatedElement = pEl;
          editOp->EoSavedElement = NULL;
        }
      if (reverse)
        {
          editOp->EoParent = newParent;
          editOp->EoPreviousSibling = newPreviousSibling;
          editOp->EoCreatedElement = newCreatedElement;
          editOp->EoSavedElement = newSavedElement;
        }
      /* too late to mark the document as modified */
      //TtaSetDocumentModified (doc);
    }
  else if (editOp->EoType == EtChangeType)
    {
      pEl = editOp->EoChangedElement;
      if (pEl)
        {
          newType = pEl->ElTypeNumber;
          TtaChangeTypeOfElement ((Element)pEl, doc, editOp->EoElementType);
          if (reverse)
            editOp->EoElementType = newType;
          else
            editOp->EoElementType = 0;
          /* mark the document as modified */
          TtaSetDocumentModified (doc);
        }
    }
}

/*----------------------------------------------------------------------
  MoveEditToRedoQueue
  Move the latest undone edit of document pDoc from the Undo queue to
  the Redo queue.
  ----------------------------------------------------------------------*/
static void MoveEditToRedoQueue (PtrDocument pDoc)
{
  PtrEditOperation	editOp;

  /* remove the latest edit operation from the Undo queue */
  editOp = UnchainLatestOp (pDoc, TRUE);
  /* insert it in the Redo queue */
  editOp->EoPreviousOp = pDoc->DocLastUndone;
  if (pDoc->DocLastUndone)
    pDoc->DocLastUndone->EoNextOp = editOp;
  pDoc->DocLastUndone = editOp;
  editOp->EoNextOp = NULL;
}

/*----------------------------------------------------------------------
  MoveEditToUndoQueue
  Move the latest redone edit of document pDoc from the Redo queue to
  the Undo queue.
  ----------------------------------------------------------------------*/
static void MoveEditToUndoQueue (PtrDocument pDoc)
{
  PtrEditOperation	editOp;

  /* remove the latest edit operation from the Redo queue */
  editOp = UnchainLatestOp (pDoc, FALSE);
  /* insert it in the Undo queue */
  editOp->EoPreviousOp = pDoc->DocLastEdit;
  if (pDoc->DocLastEdit)
    pDoc->DocLastEdit->EoNextOp = editOp;
  pDoc->DocLastEdit = editOp;
  editOp->EoNextOp = NULL;
}

/*----------------------------------------------------------------------
  OpenRedoSequence
  Open a sequence of editing operations in the Redo queue.
  ----------------------------------------------------------------------*/
static void OpenRedoSequence (Document doc)
{
  PtrDocument		    pDoc;
  PtrEditOperation	editOp;
  Element		        firstSel, lastSel;
  PtrAttribute      pAttr;
  int			          firstSelChar, lastSelChar, i;

  pDoc = LoadedDocument [doc - 1];
  /* create a new operation descriptor, a Delimiter */
  editOp = (PtrEditOperation) TtaGetMemory (sizeof (EditOperation));
  editOp->EoType = EtDelimiter;
  /* insert the new operation descriptor in the Redo queue */
  editOp->EoPreviousOp = pDoc->DocLastUndone;
  if (pDoc->DocLastUndone)
    pDoc->DocLastUndone->EoNextOp = editOp;
  pDoc->DocLastUndone = editOp;
  editOp->EoNextOp = NULL;
  /* store the current selection in this descriptor */
  if (AbsBoxSelectedAttr)
    {
      pAttr = AbsBoxSelectedAttr->AbCreatorAttr;
      editOp->EoSelectedAttrSch = pAttr->AeAttrSSchema;
      editOp->EoSelectedAttr = pAttr->AeAttrNum;
      editOp->EoFirstSelectedEl = AbsBoxSelectedAttr->AbElement;
      editOp->EoFirstSelectedChar = FirstSelectedCharInAttr;
      editOp->EoLastSelectedEl = AbsBoxSelectedAttr->AbElement;
      editOp->EoLastSelectedChar = LastSelectedCharInAttr;
      editOp->EoColumnSelected = FALSE;
    }
  else
    {
      editOp->EoSelectedAttrSch = NULL;
      editOp->EoSelectedAttr = 0;
      TtaGiveFirstSelectedElement (doc, &firstSel, &firstSelChar, &i);
      TtaGiveLastSelectedElement (doc, &lastSel, &i, &lastSelChar);
      editOp->EoFirstSelectedEl = (PtrElement)firstSel;
      if (TtaIsGraphics (firstSel))
        // don't register the specific point
        editOp->EoFirstSelectedChar = 0;
      else
        editOp->EoFirstSelectedChar = firstSelChar;
      editOp->EoLastSelectedEl = (PtrElement)lastSel;
      if (TtaIsGraphics (lastSel))
        // don't register the specific point
        editOp->EoLastSelectedChar = 0;
      else
        editOp->EoLastSelectedChar = lastSelChar;
      editOp->EoColumnSelected = WholeColumnSelected;
    }
  editOp->EoInitialSequence = !pDoc->DocModified;
  CheckUniqueInit (editOp);
  /* update the number of editing sequences registered in the Redo queue */
  UpdateRedoLength (1, pDoc);
  /* If the Redo queue is too long, cancel the oldest sequence */
  if (pDoc->DocNbUndone > MAX_EDIT_HISTORY_LENGTH)
    CancelOldestSequence (pDoc, FALSE);
}

/*----------------------------------------------------------------------
  UndoNoRedo
  Undo the latest sequence of editing operations recorded in the history
  of document doc and forget about this sequence: it won't be redone by
  the next Redo command issued by the user.
  ----------------------------------------------------------------------*/
void UndoNoRedo (Document doc)
{
  PtrDocument          pDoc;
  DisplayMode          dispMode;
  ThotBool		         doit, lock;

  pDoc = LoadedDocument [doc - 1];
  if (!pDoc->DocLastEdit)
    /* history is empty */
    return;
  dispMode = TtaGetDisplayMode (doc);
  if (dispMode != DeferredDisplay)
    TtaSetDisplayMode (doc, DeferredDisplay);

  /* lock tables formatting */
  TtaGiveTableFormattingLock (&lock);
  if (!lock)
    /* table formatting is not locked, lock it now */
    TtaLockTableFormatting ();
 
  /* disable structure checking */
  TtaSetStructureChecking (FALSE, doc);

  /* Undo the latest editing operations up to the first sequence delimiter */
  doit = TRUE;
  while (doit)
    {
      UndoOperation (TRUE, doc, FALSE);
      if (pDoc->DocLastEdit->EoType == EtDelimiter)
        /* end of sequence */
        doit = FALSE;
      /* the most recent editing operation in the history has been undone.
         Remove it from the editing history */
      CancelAnEdit (pDoc->DocLastEdit, pDoc, TRUE);
    }

  if (!lock)
    /* unlock table formatting */
    TtaUnlockTableFormatting ();
  if (dispMode != DeferredDisplay)
    TtaSetDisplayMode (doc, dispMode);
}

/*----------------------------------------------------------------------
  TtcUndo
  Undo the latest sequence of editing operations recorded in the history
  of document doc and register it in the Redo queue.
  ----------------------------------------------------------------------*/
void TtcUndo (Document doc, View view)
{
  PtrDocument          pDoc;
  DisplayMode          dispMode;
  ThotBool		doit, lock;

  lock = TRUE;
  pDoc = LoadedDocument [doc - 1];
  if (pDoc == NULL || !pDoc->DocLastEdit)
    /* history is empty */
    return;
  if (pDoc->DocEditSequence)
    {
      /* the last history sequence was not closed */
      printf ("ERR: Undo an open history sequence\n*/");
      CloseHistorySequence (pDoc);
      if (!pDoc->DocLastEdit)
        return;
    }

  /* Start a new sequence in the Redo queue */
  OpenRedoSequence (doc);

  dispMode = TtaGetDisplayMode (doc);
  if (dispMode != DeferredDisplay)
    TtaSetDisplayMode (doc, DeferredDisplay);
  TtaUnselect (doc);
  /* lock tables formatting */
  TtaGiveTableFormattingLock (&lock);
  if (!lock)
    /* table formatting is not locked, lock it now */
    TtaLockTableFormatting ();
  /* disable structure checking */
  TtaSetStructureChecking (FALSE, doc);

  /* Undo all operations belonging to a sequence of editing operations */
  doit = TRUE;
  while (doit)
    {
      UndoOperation (TRUE, doc, TRUE);
      if (pDoc->DocLastEdit->EoType == EtDelimiter)
        /* end of sequence */
        {
          doit = FALSE;
          CancelAnEdit (pDoc->DocLastEdit, pDoc, TRUE);
        }
      else
        /* the most recent editing operation in the history has been undone.
           Remove it from the editing history and put it in the Redo queue */
        MoveEditToRedoQueue (pDoc);
    }

  if (!lock)
    /* unlock table formatting */
    TtaUnlockTableFormatting ();
  if (dispMode != DeferredDisplay)
    TtaSetDisplayMode (doc, dispMode);

  // Perhaps some contextual menus change
  TtaExecuteMenuAction ("UpdateContextSensitiveMenus", doc, 1, TRUE);
}

/*----------------------------------------------------------------------
  TtcRedo
  Redo the latest sequence of editing operations undone by the TtcUndo
  and register it in the editing history.
  ----------------------------------------------------------------------*/
void TtcRedo (Document doc, View view)
{
  PtrDocument          pDoc;
  Element		firstSel, lastSel;
  PtrAttribute         pAttr;
  DisplayMode          dispMode;
  int			firstSelChar, lastSelChar, i;
  ThotBool		doit;

  pDoc = LoadedDocument [doc - 1];
  if (!pDoc->DocLastUndone)
    /* no undone command */
    return;

  /* Start a new sequence in the Undo queue */
  if (AbsBoxSelectedAttr)
    {
      pAttr = AbsBoxSelectedAttr->AbCreatorAttr;
      firstSelChar = FirstSelectedCharInAttr;
      lastSelChar = LastSelectedCharInAttr;
      firstSel = (Element)(AbsBoxSelectedAttr->AbElement);
      lastSel = (Element)(AbsBoxSelectedAttr->AbElement);
    }
  else
    {
      pAttr = NULL;
      TtaGiveFirstSelectedElement (doc, &firstSel, &firstSelChar, &i);
      TtaGiveLastSelectedElement (doc, &lastSel, &i, &lastSelChar);
    }
  TtaUnselect (doc);
  OpenHistorySequence (pDoc, (PtrElement)firstSel, (PtrElement)lastSel,
                       pAttr, firstSelChar, lastSelChar);
  dispMode = TtaGetDisplayMode (doc);
  if (dispMode != DeferredDisplay)
    TtaSetDisplayMode (doc, DeferredDisplay);
  /* disable structure checking */
  TtaSetStructureChecking (FALSE, doc);

  /* Undo all operations belonging to a sequence of editing operations */
  doit = TRUE;
  while (doit)
    {
      UndoOperation (FALSE, doc, TRUE);
      if (pDoc->DocLastUndone->EoType == EtDelimiter)
        /* end of sequence */
        {
          doit = FALSE;
          CancelAnEdit (pDoc->DocLastUndone, pDoc, FALSE);
        }  
      else
        /* the most recent editing operation in the history has been redone.
           Remove it from the Redo queue and put it in the Undo queue */
        MoveEditToUndoQueue (pDoc);
    }
  /* close sequence in Undo queue */
  pDoc->DocEditSequence = FALSE;
  if (dispMode != DeferredDisplay)
    TtaSetDisplayMode (doc, dispMode);

  // Perhaps some contextual menus change
  TtaExecuteMenuAction ("UpdateContextSensitiveMenus", doc, 1, TRUE);
}
