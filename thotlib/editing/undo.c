/*
 *
 *  (c) COPYRIGHT INRIA 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * This module handles the Undo command
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

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "constres.h"
#include "creation_tv.h"
#include "modif_tv.h"
#include "select_tv.h"
#include "edit_tv.h"
#include "appdialogue_tv.h"

typedef struct _EditOperation *PtrEditOperation;

/* Description of an editing operation in the history of editing commands */
typedef struct _EditOperation
{
  PtrEditOperation EoNextOp;	      /* next operation in the editing
					 history */
  PtrEditOperation EoPreviousOp;      /* previous operation in the editing
					 history */
  boolean	   EoToBeContinued;   /* this operation is part of a sequence
					 to be undone at once */
  PtrElement	   EoParent;	      /* parent of elements to be inserted to
					 undo the operation */
  PtrElement	   EoPreviousSibling; /* previous sibling of first element to
					 be inserted to undo the operation */
  PtrElement	   EoCreatedElement;  /* element to be removed to undo the
					 operation */
  PtrElement	   EoSavedElement;    /* copy of the element to be inserted to
					 undo the operation */
  PtrElement       EoFirstSelectedEl; /* first selected element */
  int              EoFirstSelectedChar; /* index of first selected character in
					 the first selected element, if it's a
					 character string */
  PtrElement       EoLastSelectedEl;  /* last selected element */
  int              EoLastSelectedChar;/* index of last selected character in
					 the last selected element, if it's a
					 character string */
} EditOperation;

/* Current status of editing history */
   /* document whose editing history is recorded */
   static PtrDocument HistoryDoc = NULL;
   /* latest editing operation for that document */
   static PtrEditOperation LastEdit = NULL;
   /* number of editing commands recorded in the history */
   static int NbEditsInHistory = 0;
   /* a single editing command has started a sequence of editing operations */
   static boolean EditSequence = FALSE;
   /* if EditSequence, the first operation of the sequence is expected */
   static boolean FirstInSequence = FALSE;
   /* maximum number of editing operations recorded in the history */
#define MAX_EDIT_HISTORY_LENGTH 20

/****** Reset history (ClearHistory) when loading or reloading a document ****/

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
   fprintf (stderr, "**** Undo error %d ****\n", errorCode);
}

/*----------------------------------------------------------------------
   UpdateHistoryLength
   Add diff to variable NbEditsInHistory
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void UpdateHistoryLength (int diff, PtrDocument pDoc)
#else  /* __STDC__ */
static void UpdateHistoryLength (diff, pDoc)
     int diff;
     PtrDocument pDoc;

#endif /* __STDC__ */
{
   if (NbEditsInHistory == 0 && diff > 0)
     /* enable Undo command */
     /**********/;
   NbEditsInHistory += diff;
   if (NbEditsInHistory == 0 && diff < 0)
     /* disable Undo command */
     /**********/;
}

/*----------------------------------------------------------------------
   FreeSavedElementsHist
   free the saved elements associated with editing operation editOp
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void FreeSavedElementsHist (PtrEditOperation editOp, PtrDocument pDoc)
#else  /* __STDC__ */
static void FreeSavedElementsHist (editOp, pDoc)
PtrEditOperation editOp;
PtrDocument pDoc;

#endif /* __STDC__ */
{
   PtrElement	pEl;

   pEl = editOp->EoSavedElement;
   if (pEl)
      {
      /* if the saved selection is in the freed element, cancel it */
      if (editOp->EoFirstSelectedEl)
	 if (ElemIsWithinSubtree (editOp->EoFirstSelectedEl, pEl))
	    {
	    editOp->EoFirstSelectedEl = NULL;
	    editOp->EoLastSelectedEl = NULL;
	    }
      if (editOp->EoLastSelectedEl)
	 if (ElemIsWithinSubtree (editOp->EoLastSelectedEl, pEl))
	    {
	    editOp->EoFirstSelectedEl = NULL;
	    editOp->EoLastSelectedEl = NULL;
	    }
      /* free the saved element */
      DeleteElement (&pEl, pDoc);
      }
   editOp->EoSavedElement = NULL;
}

/*----------------------------------------------------------------------
   CancelAnEdit
   Remove and delete an editing operation from the history
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void CancelAnEdit (PtrEditOperation editOp, PtrDocument pDoc)
#else  /* __STDC__ */
static void CancelAnEdit (editOp, pDoc)
PtrEditOperation editOp;
PtrDocument pDoc;

#endif /* __STDC__ */
{
   /* Error if there is no current history or if the current history is not
      related to that document */
   if (!HistoryDoc || HistoryDoc != pDoc)
     {
      HistError (1);
      return;
     }
   /* unchain the operation descriptor */
   if (editOp == LastEdit)
      LastEdit = LastEdit->EoPreviousOp;
   if (editOp->EoNextOp)
      editOp->EoNextOp->EoPreviousOp = editOp->EoPreviousOp;
   if (editOp->EoPreviousOp)
      editOp->EoPreviousOp->EoNextOp = editOp->EoNextOp;
   /* free the saved elements associated with that editing operation */
   FreeSavedElementsHist (editOp, pDoc);
   /* free the editing operation descriptor */
   TtaFreeMemory (editOp);
   editOp = NULL;
   /* update the number of editing operations remaining in the history */
   UpdateHistoryLength (-1, pDoc);
}

/*----------------------------------------------------------------------
   ClearHistory
   Clear the current history
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void ClearHistory (PtrDocument pDoc)
#else  /* __STDC__ */
static void ClearHistory (pDoc)
PtrDocument pDoc;

#endif /* __STDC__ */
{
   PtrEditOperation editOp, nextEditOp;

   /* free all editing operation recorded in the current history */
   editOp = LastEdit;
   while (editOp)
     {
       nextEditOp = editOp->EoNextOp;
       CancelAnEdit (editOp, HistoryDoc);
       editOp = nextEditOp;
     }
   /* reiniatilize all variable representing the current history */
   HistoryDoc = NULL;
   LastEdit = NULL;
   NbEditsInHistory = 0;
   EditSequence = FALSE;
   FirstInSequence = FALSE;
}

/*----------------------------------------------------------------------
   ChangePointersOlderEdits
   If Op and older editing operations in the history refer to elements
   that have been copied from subtree pEl, change these reference to the
   corresponding copies
   If the selection saved in Op refers to elements that have been copied
   from subtree pEl, change the saved selection to the corresponding copies.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void ChangePointersOlderEdits (PtrEditOperation Op, PtrElement pTree)
#else  /* __STDC__ */
static void ChangePointersOlderEdits (Op, pTree)
PtrEditOperation Op;
PtrElement pTree;

#endif /* __STDC__ */
{
  PtrEditOperation	editOp;

  editOp = Op->EoPreviousOp;
  while (editOp)
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
    if (editOp->EoFirstSelectedEl)
      if (ElemIsWithinSubtree(editOp->EoFirstSelectedEl, pTree))
	editOp->EoFirstSelectedEl = editOp->EoFirstSelectedEl->ElCopy;
    if (editOp->EoLastSelectedEl)
      if (ElemIsWithinSubtree(editOp->EoLastSelectedEl, pTree))
	editOp->EoLastSelectedEl = editOp->EoLastSelectedEl->ElCopy;
    editOp = editOp->EoPreviousOp;
    }
  /* if the current selection is in the copied element, set the saved
     selection to the equivalent elements in the copy */
  if (Op->EoFirstSelectedEl)
     if (ElemIsWithinSubtree (Op->EoFirstSelectedEl, pTree))
	Op->EoFirstSelectedEl = Op->EoFirstSelectedEl->ElCopy;
  if (Op->EoLastSelectedEl)
     if (ElemIsWithinSubtree (Op->EoLastSelectedEl, pTree))
	Op->EoLastSelectedEl = Op->EoLastSelectedEl->ElCopy;
}

/*----------------------------------------------------------------------
   AddEditOpInHistory
   Register a single editing operation in the editing history.
   pEl: the elements that has been (or will be) changed or deleted by the
	 editing operation.
   pDoc: the document to which this element belong.
   save: a copy of element pEl must be saved in order to allow it to be
	 restored when the operation will be undone.
   removeWhenUndoing: element pEl to must be deleted when the operation will
	 be undone.
   firstSel, lastSel, firstSelChar, lastSelChar: indicate the selection
	 that must be set when the operation will be undone.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void AddEditOpInHistory (PtrElement pEl, PtrDocument pDoc, boolean save,
			 boolean removeWhenUndoing, PtrElement firstSel,
		         PtrElement lastSel, int firstSelChar, int lastSelChar)
#else  /* __STDC__ */
void AddEditOpInHistory (pEl, pDoc, save, removeWhenUndoing,
			 firstSel, lastSel, firstSelChar, lastSelChar)
PtrElement pEl;
PtrDocument pDoc;
boolean save;
boolean removeWhenUndoing;
PtrElement firstSel;
PtrElement lastSel;
int firstSelChar;
int lastSelChar;

#endif /* __STDC__ */
{
   PtrEditOperation	editOp;
   PtrElement		pCopy;

   if (!pEl)
      return;
   /* if an editing sequence is open, changing document is not allowed */
   if (EditSequence && HistoryDoc != pDoc)
      {
      HistError (2);
      return;
      }
   /* if this operation does not concern the same document as the previous
      ones, clear the current editing history and start a new one */
   if (HistoryDoc && HistoryDoc != pDoc)
      ClearHistory (pDoc);
   HistoryDoc = pDoc;
   /* create a new operation descriptor in the history */
   editOp = (PtrEditOperation) TtaGetMemory (sizeof (EditOperation));
   /* link the new operation descriptor in the history */
   editOp->EoPreviousOp = LastEdit;
   if (LastEdit)
      LastEdit->EoNextOp = editOp;
   LastEdit = editOp;
   editOp->EoNextOp = NULL;
   if (EditSequence && !FirstInSequence)
      editOp->EoToBeContinued = TRUE;
   else
      editOp->EoToBeContinued = FALSE;
   /* record the location in the abstract tree concerned by the operation */
   editOp->EoParent = pEl->ElParent;
   editOp->EoPreviousSibling = pEl->ElPrevious;
   if (removeWhenUndoing)
      editOp->EoCreatedElement = pEl;
   else
      editOp->EoCreatedElement = NULL;
   editOp->EoSavedElement = NULL;
   editOp->EoFirstSelectedEl = firstSel;
   editOp->EoFirstSelectedChar = firstSelChar;
   editOp->EoLastSelectedEl = lastSel;
   editOp->EoLastSelectedChar = lastSelChar;
   UpdateHistoryLength (1, pDoc);

   if (save)
     /* copy the elements concerned by the operation and attach them to the
        operation descriptor */
     {
       /* do the copy */
       pCopy = CopyTree (pEl, pDoc, pEl->ElAssocNum, pEl->ElStructSchema,
		         pDoc, pEl->ElParent, FALSE, FALSE);
       /* store the copy in the editing operation descriptor */
       editOp->EoSavedElement = pCopy;
       /* if older editing operations in the history refer to elements that
	  have been copied, change these reference to the copies */
       ChangePointersOlderEdits (editOp, pEl);
     }

   /* if a sequence of operations is open, there is now at least one element
      in the sequence */
   if (EditSequence)
      FirstInSequence = FALSE;

   /* If the history is too long, cancel the oldest operation in the history */
   if (NbEditsInHistory > MAX_EDIT_HISTORY_LENGTH)
      {
      while (editOp->EoPreviousOp)
	editOp = editOp->EoPreviousOp;
      CancelAnEdit (editOp, pDoc);
      }
}

/*----------------------------------------------------------------------
   CancelLastEditFromHistory
   Cancel the most recent editing operation in the editing history.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void CancelLastEditFromHistory (PtrDocument pDoc)
#else  /* __STDC__ */
void CancelLastEditFromHistory (pDoc)
PtrDocument pDoc;

#endif /* __STDC__ */
{
   /* the cancelled operation must be for the same document */
   if (HistoryDoc != pDoc)
     {
      HistError (5);
      return;
     }
   if (!LastEdit)
     /* history empty. Error */
     {
      HistError (6);
      return;
     }
   /* change the pointers in older edits that refer to the saved elements
      that will be released */
   if (LastEdit->EoSavedElement)
      ChangePointersOlderEdits (LastEdit, LastEdit->EoSavedElement);
   if (EditSequence && !LastEdit->EoToBeContinued)
     /* the latest operation was the first in a sequence. The next one
	will again be the first */
     FirstInSequence = TRUE;
   /* Remove the latest operation descriptor */
   CancelAnEdit (LastEdit, pDoc);
}


/*----------------------------------------------------------------------
   OpenHistorySequence
   Open a sequence of editing operations in the history.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void OpenHistorySequence (PtrDocument pDoc)
#else  /* __STDC__ */
void OpenHistorySequence (pDoc)
PtrDocument pDoc;

#endif /* __STDC__ */
{

  /* can not open a sequence if a sequence is already open */
  if (EditSequence)
    {
      HistError (7);
      return;
    }
  /* if it's for a different document, clear the current history and start
     a new one for the document of interest */
  if (HistoryDoc && HistoryDoc != pDoc)
     ClearHistory (pDoc);
  HistoryDoc = pDoc;
  EditSequence = TRUE;
  FirstInSequence = TRUE;
}

/*----------------------------------------------------------------------
   CloseHistorySequence
   Close a sequence of editing operations in the history.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void CloseHistorySequence (PtrDocument pDoc)
#else  /* __STDC__ */
void CloseHistorySequence (pDoc)
PtrDocument pDoc;

#endif /* __STDC__ */
{

  /* error if no sequence open */
   if (!EditSequence)
     {
      HistError (8);
      return;
     }
   /* error if changing document */
   if (!HistoryDoc || HistoryDoc != pDoc)
     {
      HistError (9);
      return;
     }
   /* sequence closed */
   EditSequence = FALSE;
}



/*----------------------------------------------------------------------
   TtcUndo
   Undo the latest sequence of editing operations recorded in the history and
   remove it from the istory.
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
   PtrElement		pEl, pSibling;
   NotifyElement	notifyEl;
   int			i, nSiblings;
   boolean		doit;

   if (!LastEdit)
     /* history is empty */
      return;

   pDoc = LoadedDocument [doc - 1];
   if (HistoryDoc != pDoc)
     /* current history is not related to the document issuing the Undo
	command */
      return;

   /* Undo all operations belonging to a sequence of editing operations */
   doit = TRUE;
   while (doit)
      {
      /* delete the element that have to be removed from the abstract tree */
      if (LastEdit->EoCreatedElement)
         {
	 pEl = LastEdit->EoCreatedElement;
	 /* tell the application that an element will be removed from the
	    abstract tree */
	 SendEventSubTree (TteElemDelete, pDoc, pEl, TTE_STANDARD_DELETE_LAST_ITEM);
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
	 /* remove an element */
         TtaDeleteTree ((Element)pEl, doc);
	 /* tell the application that an element has been removed */
	 CallEventType ((NotifyEvent *) (&notifyEl), FALSE);
	 }

      /* insert the saved element in the abstract tree */
      if (LastEdit->EoSavedElement)
         {
         if (LastEdit->EoPreviousSibling)
            TtaInsertSibling ((Element)(LastEdit->EoSavedElement),
			  (Element)(LastEdit->EoPreviousSibling), FALSE, doc);
         else
            TtaInsertFirstChild (&(LastEdit->EoSavedElement),
				 (Element)(LastEdit->EoParent), doc);
         /* send event ElemPaste.Post to the application */
         NotifySubTree (TteElemPaste, pDoc, LastEdit->EoSavedElement, 0);
         }
      LastEdit->EoSavedElement = NULL;

      /* set the selection that is recorded */
      if (LastEdit->EoFirstSelectedEl && LastEdit->EoLastSelectedEl)
	{
        if (LastEdit->EoFirstSelectedChar > 0)
          {
          if (LastEdit->EoFirstSelectedEl == LastEdit->EoLastSelectedEl)
   	     i = LastEdit->EoLastSelectedChar;
          else
   	     i = TtaGetTextLength (LastEdit->EoFirstSelectedEl);
          TtaSelectString (doc, LastEdit->EoFirstSelectedEl,
			   LastEdit->EoFirstSelectedChar, i);
          }
        else
          TtaSelectElement (doc, LastEdit->EoFirstSelectedEl);
        if (LastEdit->EoFirstSelectedEl != LastEdit->EoLastSelectedEl)
          TtaExtendSelection (doc, LastEdit->EoLastSelectedEl,
			      LastEdit->EoLastSelectedChar);
	}
   
      doit = LastEdit->EoToBeContinued;
   
      /* the most recent editing operation in the history has been undone.
         Remove it from the history */
      CancelAnEdit (LastEdit, pDoc);
      if (!LastEdit)
         doit = FALSE;
      }
}
