/*
 *
 *  (c) COPYRIGHT INRIA 1998.
 *  Please read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * This module handles commands Undo and Redo
 *
 * Authors: V. Quint (INRIA)
 *          I. Vatton (INRIA)
 */

#include "thot_sys.h"
#include "constmedia.h"
#include "constmenu.h"
#include "typemedia.h"
#include "modif.h"
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

#include "actions_f.h"
#include "appdialogue_f.h"
#include "attributes_f.h"
#include "callback_f.h"
#include "contentapi_f.h"
#include "memory_f.h"
#include "structcommands_f.h"
#include "structcreation_f.h"
#include "tree_f.h"

/*----------------------------------------------------------------------
   HistError
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void HistError (int errorCode)
#else  /* __STDC__ */
static void HistError (errorCode)
int errorCode;

#endif /* __STDC__ */
{
#ifdef THOT_DEBUG
   fprintf (stderr, "**** Undo error %d ****\n", errorCode);
#endif /* THOT_DEBUG */
}

/*----------------------------------------------------------------------
   UpdateHistoryLength
   Add diff to variable pDoc->DocNbEditsInHistory
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void UpdateHistoryLength (int diff, PtrDocument pDoc)
#else  /* __STDC__ */
static void UpdateHistoryLength (diff, pDoc)
     int diff;
     PtrDocument pDoc;

#endif /* __STDC__ */
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
#ifdef __STDC__
static void UpdateRedoLength (int diff, PtrDocument pDoc)
#else  /* __STDC__ */
static void UpdateRedoLength (diff, pDoc)
     int diff;
     PtrDocument pDoc;

#endif /* __STDC__ */
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
#ifdef __STDC__
static PtrEditOperation UnchainLatestOp (PtrDocument pDoc, ThotBool undo)
#else  /* __STDC__ */
static PtrEditOperation UnchainLatestOp (pDoc, undo)
PtrDocument pDoc;
ThotBool undo;

#endif /* __STDC__ */
{
   PtrEditOperation	*editOp, last;

   if (undo)
      editOp = &(pDoc->DocLastEdit);
   else
      editOp = &(pDoc->DocLastUndone);
   last = *editOp;
   *editOp = (*editOp)->EoPreviousOp;
   (*editOp)->EoNextOp = NULL;
   last->EoPreviousOp = NULL;
   return last;
}

/*----------------------------------------------------------------------
   CancelAnEdit
   Remove and delete an editing operation from the history
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void CancelAnEdit (PtrEditOperation editOp, PtrDocument pDoc, ThotBool undo)
#else  /* __STDC__ */
static void CancelAnEdit (editOp, pDoc, undo)
PtrEditOperation editOp;
PtrDocument pDoc;
ThotBool undo;

#endif /* __STDC__ */
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
         {
         /* if the saved selection is in the freed element, cancel it */
	 /* first, get the delimiter that contains the selection */
	 prevOp = editOp;
	 do
	    prevOp = prevOp->EoPreviousOp;
	 while (prevOp && prevOp->EoType != EtDelimiter);
	 /* then, check that selection */
	 if (prevOp)
	    {
            if (prevOp->EoFirstSelectedEl)
	       if (ElemIsWithinSubtree (prevOp->EoFirstSelectedEl, pEl))
	          {
	          prevOp->EoFirstSelectedEl = NULL;
	          prevOp->EoLastSelectedEl = NULL;
	          }
            if (prevOp->EoLastSelectedEl)
	       if (ElemIsWithinSubtree (prevOp->EoLastSelectedEl, pEl))
	          {
	          prevOp->EoFirstSelectedEl = NULL;
	          prevOp->EoLastSelectedEl = NULL;
	          }
	    }
         /* free the saved element */
         DeleteElement (&pEl, pDoc);
         }
      editOp->EoSavedElement = NULL;
      }

   if (editOp->EoType == EtAttribute)
      {
      pAttr = editOp->EoSavedAttribute;
      if (pAttr)
         /* free the saved attribute */
	 DeleteAttribute (NULL, pAttr);
      editOp->EoSavedAttribute = NULL;
      }

   if (editOp->EoType == EtDelimiter)
      /* update the number of editing sequences remaining in the queue */
      if (undo)
         UpdateHistoryLength (-1, pDoc);
      else
         UpdateRedoLength (-1, pDoc);

   /* free the editing operation descriptor */
   TtaFreeMemory (editOp);
   editOp = NULL;
}

/*----------------------------------------------------------------------
   ClearRedoQueue
   Clear the Redo queue of document pDoc
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void ClearRedoQueue (PtrDocument pDoc)
#else  /* __STDC__ */
static void ClearRedoQueue (pDoc)
PtrDocument pDoc;

#endif /* __STDC__ */
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
#ifdef __STDC__
void ClearHistory (PtrDocument pDoc)
#else  /* __STDC__ */
void ClearHistory (pDoc)
PtrDocument pDoc;

#endif /* __STDC__ */
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
#ifdef __STDC__
static void ChangePointersOlderEdits (PtrEditOperation Op, PtrElement pTree)
#else  /* __STDC__ */
static void ChangePointersOlderEdits (Op, pTree)
PtrEditOperation Op;
PtrElement pTree;

#endif /* __STDC__ */
{
  PtrEditOperation	editOp, prevOp;

  editOp = Op->EoPreviousOp;
  while (editOp)
    {
    if (editOp->EoType == EtElement)
       {
       if (editOp->EoParent)
         if (ElemIsWithinSubtree(editOp->EoParent, pTree))
	   editOp->EoParent = editOp->EoParent->ElCopy;
       if (editOp->EoPreviousSibling)
         if (ElemIsWithinSubtree(editOp->EoPreviousSibling, pTree))
	   editOp->EoPreviousSibling = editOp->EoPreviousSibling->ElCopy;
       if (editOp->EoCreatedElement)
         if (ElemIsWithinSubtree(editOp->EoCreatedElement, pTree))
	   editOp->EoCreatedElement = editOp->EoCreatedElement->ElCopy;
       }
    if (editOp->EoType == EtDelimiter)
       {
       if (editOp->EoFirstSelectedEl)
         if (ElemIsWithinSubtree(editOp->EoFirstSelectedEl, pTree))
	   editOp->EoFirstSelectedEl = editOp->EoFirstSelectedEl->ElCopy;
       if (editOp->EoLastSelectedEl)
         if (ElemIsWithinSubtree(editOp->EoLastSelectedEl, pTree))
	   editOp->EoLastSelectedEl = editOp->EoLastSelectedEl->ElCopy;
       }
    if (editOp->EoType == EtAttribute)
       {
       if (editOp->EoElement)
	 if (ElemIsWithinSubtree(editOp->EoElement, pTree))
	   editOp->EoElement = editOp->EoElement->ElCopy;
       }
    editOp = editOp->EoPreviousOp;
    }

  if (Op->EoType == EtElement)
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
#ifdef __STDC__
void AddEditOpInHistory (PtrElement pEl, PtrDocument pDoc, ThotBool save,
			 ThotBool removeWhenUndoing)
#else  /* __STDC__ */
void AddEditOpInHistory (pEl, pDoc, save, removeWhenUndoing)
PtrElement pEl;
PtrDocument pDoc;
ThotBool save;
ThotBool removeWhenUndoing;

#endif /* __STDC__ */
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
       pCopy = CopyTree (pEl, pDoc, pEl->ElAssocNum, pEl->ElStructSchema,
		         pDoc, pEl->ElParent, FALSE, FALSE);
       /* store the copy in the editing operation descriptor */
       editOp->EoSavedElement = pCopy;
       /* if older editing operations in the history refer to elements that
	  have been copied, change these references to the copies */
       ChangePointersOlderEdits (editOp, pEl);
     }
}

/*----------------------------------------------------------------------
   ChangeAttrPointersOlderEdits
   If Op and older editing operations in the history refer to attribute
   pAttr, change these reference to the corresponding copy pCopyAttr.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void ChangeAttrPointersOlderEdits (PtrEditOperation Op, PtrAttribute pAttr, PtrAttribute pCopyAttr)
#else  /* __STDC__ */
static void ChangeAttrPointersOlderEdits (Op, pAttr, pCopyAttr)
PtrEditOperation Op;
PtrAttribute pAttr;
PtrAttribute pCopyAttr;

#endif /* __STDC__ */
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
#ifdef __STDC__
void AddAttrEditOpInHistory (PtrAttribute pAttr, PtrElement pEl, PtrDocument pDoc, ThotBool save, ThotBool removeWhenUndoing)
#else  /* __STDC__ */
void AddAttrEditOpInHistory (pAttr, pEl, pDoc, save, removeWhenUndoing)
PtrAttribute pAttr;
PtrElement pEl;
PtrDocument pDoc;
ThotBool save;
ThotBool removeWhenUndoing;

#endif /* __STDC__ */
{
   PtrEditOperation	editOp = NULL;
   PtrAttribute		pCopy, pOldAttr;
   ThotBool		merge;

   if (!pEl && !pAttr)
      return;
   /* noting to to if no sequence open */
   if (!pDoc->DocEditSequence)
      return;

   /* if the previous operation recorded is about the same attribute for the
      same element, just modify the previous operation */
   merge = FALSE;
   if (!save || !removeWhenUndoing)
     if (pDoc->DocLastEdit)
       {
       editOp = pDoc->DocLastEdit;
       if (editOp->EoType == EtAttribute)
	  if (editOp->EoElement == pEl)
	     {
	     pOldAttr = NULL;
	     if (save && editOp->EoSavedAttribute == NULL)
		pOldAttr = editOp->EoCreatedAttribute;
	     else if (removeWhenUndoing && editOp->EoCreatedAttribute == NULL)
		pOldAttr = editOp->EoSavedAttribute;
	     if (pOldAttr)
		if (pAttr->AeAttrSSchema == pOldAttr->AeAttrSSchema)
		   if (pAttr->AeAttrNum == pOldAttr->AeAttrNum)
		      merge = TRUE;
	     }
       }

   if (!merge)
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
      }

   if (removeWhenUndoing)
      editOp->EoCreatedAttribute = pAttr;

   if (save)
     /* copy the attribute concerned by the operation and attach it to the
        operation descriptor */
     {
     pCopy = AddAttrToElem (NULL, pAttr, NULL);
     editOp->EoSavedAttribute = pCopy;
     /* if older editing operations in the history refer to attribute that
	has been copied, change these references to the copy */
     ChangeAttrPointersOlderEdits (editOp, pAttr, pCopy);
     }
}

/*----------------------------------------------------------------------
   ChangeLastRegisteredAttr
   Change the most recent editing operation registered in the editing history
   of document, only if it's an attribute operation for element oldEl.
   In that case, make it related to element newEl and attribute newAttr.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void ChangeLastRegisteredAttr (PtrElement oldEl, PtrElement newEl, PtrAttribute oldAttr, PtrAttribute newAttr, PtrDocument pDoc)
#else  /* __STDC__ */
void ChangeLastRegisteredAttr (oldEl, newEl, oldAttr, newAttr, pDoc)
PtrElement oldEl;
PtrElement newEl;
PtrAttribute oldAttr;
PtrAttribute newAttr;
PtrDocument pDoc;

#endif /* __STDC__ */
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
#ifdef __STDC__
void ReplaceLastRegisteredAttr (PtrDocument pDoc)
#else  /* __STDC__ */
void ReplaceLastRegisteredAttr (pDoc)
PtrDocument pDoc;

#endif /* __STDC__ */
{
   if (pDoc->DocLastEdit)
     if (pDoc->DocLastEdit->EoType == EtAttribute)
        if (pDoc->DocLastEdit->EoSavedAttribute)
           if (pDoc->DocLastEdit->EoCreatedAttribute)
	      pDoc->DocLastEdit->EoCreatedAttribute = NULL;
}

/*----------------------------------------------------------------------
   CancelLastEditFromHistory
   Cancel the most recent editing operation registered in the editing history.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void CancelLastEditFromHistory (PtrDocument pDoc)
#else  /* __STDC__ */
void CancelLastEditFromHistory (pDoc)
PtrDocument pDoc;

#endif /* __STDC__ */
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
   else if (pDoc->DocLastEdit->EoType != EtAttribute)
     /* Not an operation on elements or attributes */
      return;

   /* Remove the latest operation descriptor */
   CancelAnEdit (pDoc->DocLastEdit, pDoc, TRUE);
}

/*----------------------------------------------------------------------
   CancelOldestSequence

   Cancel the oldest sequence in the Undo (if undo==TRUE) or Redo queue.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void CancelOldestSequence (PtrDocument pDoc, ThotBool undo)
#else  /* __STDC__ */
static void CancelOldestSequence (pDoc, undo)
PtrDocument pDoc;
ThotBool undo;
#endif /* __STDC__ */
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
   OpenHistorySequence
   Open a sequence of editing operations in the history.
   firstSel, lastSel, firstSelChar, lastSelChar: indicate the selection
	 that must be set when the operation will be undone.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void OpenHistorySequence (PtrDocument pDoc, PtrElement firstSel, PtrElement lastSel, int firstSelChar, int lastSelChar)
#else  /* __STDC__ */
void OpenHistorySequence (pDoc, firstSel, lastSel, firstSelChar, lastSelChar)
PtrDocument pDoc;
PtrElement firstSel;
PtrElement lastSel;
int firstSelChar;
int lastSelChar;
#endif /* __STDC__ */
{
  PtrEditOperation	editOp;

  /* can not open a sequence if a sequence is already open */
  if (pDoc->DocEditSequence)
    {
      HistError (8);
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
  editOp->EoFirstSelectedChar = firstSelChar;
  editOp->EoLastSelectedEl = lastSel;
  editOp->EoLastSelectedChar = lastSelChar;
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
#ifdef __STDC__
ThotBool CloseHistorySequence (PtrDocument pDoc)
#else  /* __STDC__ */
ThotBool CloseHistorySequence (pDoc)
PtrDocument pDoc;

#endif /* __STDC__ */
{
  ThotBool	result;

  result = FALSE;
  /* error if no sequence open */
  if (!pDoc->DocEditSequence)
     HistError (9);
  else
     {
     if (pDoc->DocLastEdit->EoType == EtDelimiter)
        /* empty sequence, remove it */
        CancelAnEdit (pDoc->DocLastEdit, pDoc, TRUE);
     else
        {
	result = TRUE;
        /* Clear the Redo queue */
        ClearRedoQueue (pDoc);
	}
     /* sequence closed */
     pDoc->DocEditSequence = FALSE;
     }
  return result;
}

/*----------------------------------------------------------------------
   CancelLastSequenceFromHistory
   Cancel the last sequence of editing operations registered in the
   editing history of document.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CancelLastSequenceFromHistory (PtrDocument pDoc)
#else  /* __STDC__ */
void                CancelLastSequenceFromHistory (pDoc)
PtrDocument         pDoc;

#endif /* __STDC__ */
{
   ThotBool	stop;

   if (!pDoc->DocLastEdit)
      /* history is empty */
      return;
   stop = FALSE;
   while (!stop)
      {
      if (pDoc->DocLastEdit->EoType == EtDelimiter)
	 stop = TRUE;
      CancelAnEdit (pDoc->DocLastEdit, pDoc, TRUE);
      }
}

/*----------------------------------------------------------------------
   UndoOperation

   Undo the latest editing operation registered in the Undo (if undo==TRUE)
   or Redo (if redo==FALSE) queue of document doc.
   If reverse, the editing operation descriptor will decribe the reverse
   editing operation when returning.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void	UndoOperation (ThotBool undo, Document doc, ThotBool reverse)
#else  /* __STDC__ */
static void	UndoOperation (undo, doc, reverse)
ThotBool undo;
Document doc;
ThotBool reverse;

#endif /* __STDC__ */
{
   PtrEditOperation	editOp;
   PtrElement		pEl, pSibling,
                        newParent, newPreviousSibling, newCreatedElement,
                        newSavedElement;
   PtrAttribute         SavedAttribute, CreatedAttribute;
   PtrDocument		pDoc;
   PtrEditOperation	queue;
   NotifyElement	notifyEl;
   NotifyAttribute	notifyAttr;
   int			i, nSiblings;

   newParent = NULL;
   newPreviousSibling = NULL;
   newCreatedElement = NULL;
   newSavedElement = NULL;
   pDoc = LoadedDocument [doc - 1];
   if (undo)
      editOp = pDoc->DocLastEdit;
   else
      editOp = pDoc->DocLastUndone;

   if (editOp->EoType == EtDelimiter)
      /* end of a sequence */
      {
      /* enable structure checking */
      TtaSetStructureChecking (TRUE, doc);
      TtaSetDisplayMode (doc, DisplayImmediately);
      /* set the selection that is recorded */
      if (editOp->EoFirstSelectedEl && editOp->EoLastSelectedEl)
        {
        /* Send events TteElemSelect.Pre */
        notifyEl.event = TteElemSelect;
        notifyEl.document = doc;
        notifyEl.element = (Element) (editOp->EoFirstSelectedEl);
        notifyEl.elementType.ElTypeNum =
			editOp->EoFirstSelectedEl->ElTypeNumber;
        notifyEl.elementType.ElSSchema =
			(SSchema) (editOp->EoFirstSelectedEl->ElStructSchema);
        notifyEl.position = 0;
        CallEventType ((NotifyEvent *) & notifyEl, TRUE);
        if (editOp->EoFirstSelectedChar > 0)
          {
          if (editOp->EoFirstSelectedEl == editOp->EoLastSelectedEl)
   	     i = editOp->EoLastSelectedChar;
          else
   	     i = TtaGetTextLength ((Element)(editOp->EoFirstSelectedEl));
          TtaSelectString (doc, (Element)(editOp->EoFirstSelectedEl),
			   editOp->EoFirstSelectedChar, i);
          }
        else
          TtaSelectElement (doc, (Element)(editOp->EoFirstSelectedEl));
        /* Send events TteElemSelect.Post */
        notifyEl.event = TteElemSelect;
        notifyEl.document = doc;
        notifyEl.element = (Element) (editOp->EoFirstSelectedEl);
        notifyEl.elementType.ElTypeNum =
			editOp->EoFirstSelectedEl->ElTypeNumber;
        notifyEl.elementType.ElSSchema =
			(SSchema) (editOp->EoFirstSelectedEl->ElStructSchema);
        notifyEl.position = 0;
        CallEventType ((NotifyEvent *) & notifyEl, FALSE);
        if (editOp->EoFirstSelectedEl != editOp->EoLastSelectedEl)
          {
          /* Send event TteElemExtendSelect. Pre */
          notifyEl.event = TteElemExtendSelect;
          notifyEl.document = doc;
          notifyEl.element = (Element)(editOp->EoLastSelectedEl);
          notifyEl.elementType.ElTypeNum =
			editOp->EoLastSelectedEl->ElTypeNumber;
          notifyEl.elementType.ElSSchema =
			(SSchema) (editOp->EoLastSelectedEl->ElStructSchema);
          notifyEl.position = 0;
          CallEventType ((NotifyEvent *) & notifyEl, TRUE);
          TtaExtendSelection (doc, (Element)(editOp->EoLastSelectedEl),
			      editOp->EoLastSelectedChar);
          /* Send event TteElemExtendSelect. Post */
          notifyEl.event = TteElemExtendSelect;
          notifyEl.document = doc;
          notifyEl.element = (Element)(editOp->EoLastSelectedEl);
          notifyEl.elementType.ElTypeNum =
			editOp->EoLastSelectedEl->ElTypeNumber;
          notifyEl.elementType.ElSSchema =
			(SSchema) (editOp->EoLastSelectedEl->ElStructSchema);
          notifyEl.position = 0;
          CallEventType ((NotifyEvent *) & notifyEl, FALSE);
          }
        }
      }

   if (editOp->EoType == EtAttribute)
      {
      SavedAttribute = editOp->EoSavedAttribute;
      CreatedAttribute = editOp->EoCreatedAttribute;
      editOp->EoSavedAttribute = NULL;

      notifyAttr.document = doc;
      notifyAttr.element = (Element) (editOp->EoElement);
      /* delete the attribute that has to be removed from the element */
      if (editOp->EoElement && editOp->EoCreatedAttribute)
         {
         /* tell the application that an attribute will be removed */
	 if (SavedAttribute)
            notifyAttr.event = TteAttrModify;	    
	 else
            notifyAttr.event = TteAttrDelete;
         notifyAttr.attribute = (Attribute) (editOp->EoCreatedAttribute);
         notifyAttr.attributeType.AttrSSchema =
			(SSchema) (editOp->EoCreatedAttribute->AeAttrSSchema);
         notifyAttr.attributeType.AttrTypeNum =
			editOp->EoCreatedAttribute->AeAttrNum;
         CallEventAttribute (&notifyAttr, TRUE);
         if (reverse)
	    {
            editOp->EoSavedAttribute = AddAttrToElem (NULL,
					     editOp->EoCreatedAttribute, NULL);
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
      /* put the saved attribute (if any) on the element */
      if (editOp->EoElement && SavedAttribute)
         {
	 if (!CreatedAttribute)
	    {
            /* tell the application that an attribute will be created */
            notifyAttr.event = TteAttrCreate;
            notifyAttr.attribute = NULL;
            notifyAttr.attributeType.AttrSSchema =
				    (SSchema) (SavedAttribute->AeAttrSSchema);
            notifyAttr.attributeType.AttrTypeNum = SavedAttribute->AeAttrNum;
            CallEventAttribute (&notifyAttr, TRUE);
	    }
         /* put the attribute on the element */
         TtaAttachAttribute ((Element)(editOp->EoElement),
			     (Attribute)(SavedAttribute), doc);
	 if (reverse)
	    editOp->EoCreatedAttribute = SavedAttribute;
         /* tell the application that an attribute has been put */
         notifyAttr.attribute = (Attribute) (SavedAttribute);
         CallEventAttribute (&notifyAttr, FALSE);	    
         }
      /* mark the document as modified */
      TtaSetDocumentModified (doc);
      }

   if (editOp->EoType == EtElement)
      {
      /* delete the element that has to be removed from the abstract tree */
      if (reverse)
	 {
         newParent = NULL;
         newPreviousSibling = NULL;
         newCreatedElement = NULL;
         newSavedElement = NULL;
	 }
      if (editOp->EoCreatedElement)
         {
         pEl = editOp->EoCreatedElement;
         /* tell the application that an element will be removed from the
            abstract tree */
         SendEventSubTree (TteElemDelete, pDoc, pEl,
			   TTE_STANDARD_DELETE_LAST_ITEM);
         /* prepare event TteElemDelete to be sent to the application */
         notifyEl.event = TteElemDelete;
         notifyEl.document = doc;
         notifyEl.element = (Element) (pEl->ElParent);
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
	 notifyEl.info = 1;
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
         editOp->EoCreatedElement = NULL;
         }
      /* insert the saved element in the abstract tree */
      if (editOp->EoSavedElement)
         {
         if (editOp->EoPreviousSibling)
            TtaInsertSibling ((Element)(editOp->EoSavedElement),
			      (Element)(editOp->EoPreviousSibling),FALSE, doc);
         else
            TtaInsertFirstChild ((Element *)&(editOp->EoSavedElement),
				 (Element)(editOp->EoParent), doc);
         /* send event ElemPaste.Post to the application. -1 means that this
	    is not really a Paste operation but an Undo operation. */
         NotifySubTree (TteElemPaste, pDoc, editOp->EoSavedElement, -1);
	 if (reverse)
	    newCreatedElement = editOp->EoSavedElement;
         editOp->EoSavedElement = NULL;
         }
      if (reverse)
	 {
         editOp->EoParent = newParent;
         editOp->EoPreviousSibling = newPreviousSibling;
         editOp->EoCreatedElement = newCreatedElement;
         editOp->EoSavedElement = newSavedElement;
	 }
      /* mark the document as modified */
      TtaSetDocumentModified (doc);
      }
}

/*----------------------------------------------------------------------
   MoveEditToRedoQueue
   Move the latest undone edit of document pDoc from the Undo queue to
   the Redo queue.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void MoveEditToRedoQueue (PtrDocument pDoc)
#else  /* __STDC__ */
static void MoveEditToRedoQueue (pDoc)
PtrDocument pDoc;

#endif /* __STDC__ */
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
#ifdef __STDC__
static void MoveEditToUndoQueue (PtrDocument pDoc)
#else  /* __STDC__ */
static void MoveEditToUndoQueue (pDoc)
PtrDocument pDoc;

#endif /* __STDC__ */
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
#ifdef __STDC__
static void OpenRedoSequence (Document doc)

#else  /* __STDC__ */
static void OpenRedoSequence (doc)
Document doc;
#endif /* __STDC__ */
{
  PtrDocument		pDoc;
  PtrEditOperation	editOp;
  Element		firstSel, lastSel;
  int			firstSelChar, lastSelChar, i;

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
  TtaGiveFirstSelectedElement (doc, &firstSel, &firstSelChar, &i);
  TtaGiveLastSelectedElement (doc, &lastSel, &i, &lastSelChar);
  editOp->EoFirstSelectedEl = (PtrElement)firstSel;
  editOp->EoFirstSelectedChar = firstSelChar;
  editOp->EoLastSelectedEl = (PtrElement)lastSel;
  editOp->EoLastSelectedChar = lastSelChar;
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
#ifdef __STDC__
void                UndoNoRedo (Document doc)
#else  /* __STDC__ */
void                UndoNoRedo (doc)
Document            doc;

#endif /* __STDC__ */
{
   PtrDocument          pDoc;
   ThotBool		doit;

   pDoc = LoadedDocument [doc - 1];
   if (!pDoc->DocLastEdit)
     /* history is empty */
      return;
   TtaSetDisplayMode (doc, DeferredDisplay);
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
}

/*----------------------------------------------------------------------
   TtcUndo
   Undo the latest sequence of editing operations recorded in the history
   of document doc and register it in the Redo queue.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtcUndo (Document doc, View view)
#else  /* __STDC__ */
void                TtcUndo (doc, view)
Document            doc;
View                view;

#endif /* __STDC__ */
{
   PtrDocument          pDoc;
   ThotBool		doit;

   pDoc = LoadedDocument [doc - 1];
   if (!pDoc->DocLastEdit)
     /* history is empty */
      return;

   /* Start a new sequence in the Redo queue */
   OpenRedoSequence (doc);

   TtaUnselect (doc);
   TtaSetDisplayMode (doc, DeferredDisplay);
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
   TtaSetDisplayMode (doc, DisplayImmediately);
}

/*----------------------------------------------------------------------
   TtcRedo
   Redo the latest sequence of editing operations undone by the TtcUndo
   and register it in the editing history.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtcRedo (Document doc, View view)
#else  /* __STDC__ */
void                TtcRedo (doc, view)
Document            doc;
View                view;

#endif /* __STDC__ */
{
   PtrDocument          pDoc;
   Element		firstSel, lastSel;
   int			firstSelChar, lastSelChar, i;
   ThotBool		doit;

   pDoc = LoadedDocument [doc - 1];
   if (!pDoc->DocLastUndone)
     /* no undone command */
      return;

   /* Start a new sequence in the Undo queue */
   TtaGiveFirstSelectedElement (doc, &firstSel, &firstSelChar, &i);
   TtaGiveLastSelectedElement (doc, &lastSel, &i, &lastSelChar);
   TtaUnselect (doc);
   OpenHistorySequence (pDoc, (PtrElement)firstSel, (PtrElement)lastSel,
			firstSelChar, lastSelChar);
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
   TtaSetDisplayMode (doc, DisplayImmediately);
}
