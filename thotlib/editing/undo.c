/*
 *
 *  (c) COPYRIGHT INRIA 1998.
 *  Please read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * This module handles the Undo command
 *
 * Authors: V. Quint (INRIA)
 *          I. Vatton (INRIA)
 */

#include "ustring.h"
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

/* maximum number of editing operations recorded in the history */
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
   fprintf (stderr, "**** Undo error %d ****\n", errorCode);
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
   PtrElement		pEl;
   PtrAttribute         pAttr;
   PtrEditOperation	prevOp;

   /* Error if there is no current history */
   if (!pDoc->DocLastEdit)
     {
      HistError (1);
      return;
     }
   /* unchain the operation descriptor */
   if (editOp == pDoc->DocLastEdit)
      pDoc->DocLastEdit = pDoc->DocLastEdit->EoPreviousOp;
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
      /* update the number of editing commands remaining in the history */
      UpdateHistoryLength (-1, pDoc);

   /* free the editing operation descriptor */
   TtaFreeMemory (editOp);
   editOp = NULL;
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
   PtrEditOperation editOp, nextEditOp;

   /* free all editing operation recorded in the current history */
   editOp = pDoc->DocLastEdit;
   while (editOp)
      {
      nextEditOp = editOp->EoNextOp;
      CancelAnEdit (editOp, pDoc);
      editOp = nextEditOp;
      }
   /* reiniatilize all variable representing the current history */
   pDoc->DocLastEdit = NULL;
   pDoc->DocNbEditsInHistory = 0;
   pDoc->DocEditSequence = FALSE;
   /* disable Undo and Redo commands */
   SwitchUndo (pDoc, FALSE);
   SwitchRedo (pDoc, FALSE);
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
void AddEditOpInHistory (PtrElement pEl, PtrDocument pDoc, boolean save,
			 boolean removeWhenUndoing)
#else  /* __STDC__ */
void AddEditOpInHistory (pEl, pDoc, save, removeWhenUndoing)
PtrElement pEl;
PtrDocument pDoc;
boolean save;
boolean removeWhenUndoing;

#endif /* __STDC__ */
{
   PtrEditOperation	editOp;
   PtrElement		pCopy;

   if (!pEl)
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
void AddAttrEditOpInHistory (PtrAttribute pAttr, PtrElement pEl, PtrDocument pDoc, boolean save, boolean removeWhenUndoing)
#else  /* __STDC__ */
void AddAttrEditOpInHistory (pAttr, pEl, pDoc, save, removeWhenUndoing)
PtrAttribute pAttr;
PtrElement pEl;
PtrDocument pDoc;
boolean save;
boolean removeWhenUndoing;

#endif /* __STDC__ */
{
   PtrEditOperation	editOp;
   PtrAttribute		pCopy;

   if (!pEl && !pAttr)
      return;
   /* error if no sequence open */
   if (!pDoc->DocEditSequence)
     {
      HistError (3);
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
   editOp->EoType = EtAttribute;
   editOp->EoElement = pEl;
   if (removeWhenUndoing)
      editOp->EoCreatedAttribute = pAttr;
   else
      editOp->EoCreatedAttribute = NULL;

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
   else
     editOp->EoSavedAttribute = NULL;
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
     /* Not an operation on elements or attributes. Error */
     {
      HistError (7);
      return;
     }

   /* Remove the latest operation descriptor */
   CancelAnEdit (pDoc->DocLastEdit, pDoc);
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
  PtrEditOperation	editOp, nextOp;

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
      {
      /* get the last descriptor */
      while (editOp->EoPreviousOp)
	 editOp = editOp->EoPreviousOp;
      /* delete the last descriptor and all following descriptors up to the
	 first Delimiter */
      do
	 {
	 nextOp = editOp->EoNextOp;
         CancelAnEdit (editOp, pDoc);
	 editOp = nextOp;
	 if (editOp && editOp->EoType == EtDelimiter)
	    editOp = NULL;
	 }
      while (editOp);
      }
}

/*----------------------------------------------------------------------
   CloseHistorySequence
   Close a sequence of editing operations in the history.
   return FALSE if the Sequence is empty.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
boolean CloseHistorySequence (PtrDocument pDoc)
#else  /* __STDC__ */
boolean CloseHistorySequence (pDoc)
PtrDocument pDoc;

#endif /* __STDC__ */
{
  boolean	result;

  result = FALSE;
  /* error if no sequence open */
  if (!pDoc->DocEditSequence)
     HistError (9);
  else
     {
     if (pDoc->DocLastEdit->EoType == EtDelimiter)
        /* empty sequence, remove it */
        CancelAnEdit (pDoc->DocLastEdit, pDoc);
     else
	result = TRUE;
     /* sequence closed */
     pDoc->DocEditSequence = FALSE;
     }
  return result;
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
   /*********/
   CancelAnEdit (pDoc->DocLastEdit, pDoc);
}


/*----------------------------------------------------------------------
   TtcUndo
   Undo the latest sequence of editing operations recorded in the history and
   remove it from the history.
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
   NotifyAttribute	notifyAttr;
   int			i, nSiblings;
   boolean		doit;

   pDoc = LoadedDocument [doc - 1];
   if (!pDoc->DocLastEdit)
     /* history is empty */
      return;

   TtaSetDisplayMode (doc, DeferredDisplay);

   /* Start a new sequence in the Redo queue */
   /***********/

   /* Undo all operations belonging to a sequence of editing operations */
   doit = TRUE;
   while (doit)
      {
      if (pDoc->DocLastEdit->EoType == EtDelimiter)
	 {
	 /* end of undo sequence */
	 TtaSetDisplayMode (doc, DisplayImmediately);
         /* set the selection that is recorded */
         if (pDoc->DocLastEdit->EoFirstSelectedEl &&
	     pDoc->DocLastEdit->EoLastSelectedEl)
	   {
	   /* Send events TteElemSelect.Pre */
	   notifyEl.event = TteElemSelect;
           notifyEl.document = doc;
           notifyEl.element = (Element) (pDoc->DocLastEdit->EoFirstSelectedEl);
           notifyEl.elementType.ElTypeNum =
			    pDoc->DocLastEdit->EoFirstSelectedEl->ElTypeNumber;
           notifyEl.elementType.ElSSchema =
	      (SSchema) (pDoc->DocLastEdit->EoFirstSelectedEl->ElStructSchema);
           notifyEl.position = 0;
           CallEventType ((NotifyEvent *) & notifyEl, TRUE);
           if (pDoc->DocLastEdit->EoFirstSelectedChar > 0)
             {
             if (pDoc->DocLastEdit->EoFirstSelectedEl ==
					   pDoc->DocLastEdit->EoLastSelectedEl)
   	        i = pDoc->DocLastEdit->EoLastSelectedChar;
             else
   	        i = TtaGetTextLength ((Element)(pDoc->DocLastEdit->EoFirstSelectedEl));
             TtaSelectString (doc,
			      (Element)(pDoc->DocLastEdit->EoFirstSelectedEl),
			      pDoc->DocLastEdit->EoFirstSelectedChar, i);
             }
           else
             TtaSelectElement (doc,
			      (Element)(pDoc->DocLastEdit->EoFirstSelectedEl));
	   /* Send events TteElemSelect.Post */
	   notifyEl.event = TteElemSelect;
           notifyEl.document = doc;
           notifyEl.element = (Element) (pDoc->DocLastEdit->EoFirstSelectedEl);
           notifyEl.elementType.ElTypeNum =
			    pDoc->DocLastEdit->EoFirstSelectedEl->ElTypeNumber;
           notifyEl.elementType.ElSSchema =
	      (SSchema) (pDoc->DocLastEdit->EoFirstSelectedEl->ElStructSchema);
           notifyEl.position = 0;
           CallEventType ((NotifyEvent *) & notifyEl, FALSE);
           if (pDoc->DocLastEdit->EoFirstSelectedEl !=
					   pDoc->DocLastEdit->EoLastSelectedEl)
	     {
	     /* Send event TteElemExtendSelect. Pre */
	     notifyEl.event = TteElemExtendSelect;
             notifyEl.document = doc;
             notifyEl.element = (Element)(pDoc->DocLastEdit->EoLastSelectedEl);
             notifyEl.elementType.ElTypeNum =
			     pDoc->DocLastEdit->EoLastSelectedEl->ElTypeNumber;
             notifyEl.elementType.ElSSchema =
	       (SSchema) (pDoc->DocLastEdit->EoLastSelectedEl->ElStructSchema);
             notifyEl.position = 0;
             CallEventType ((NotifyEvent *) & notifyEl, TRUE);
             TtaExtendSelection (doc,
				(Element)(pDoc->DocLastEdit->EoLastSelectedEl),
			        pDoc->DocLastEdit->EoLastSelectedChar);
	     /* Send event TteElemExtendSelect. Post */
	     notifyEl.event = TteElemExtendSelect;
             notifyEl.document = doc;
             notifyEl.element = (Element)(pDoc->DocLastEdit->EoLastSelectedEl);
             notifyEl.elementType.ElTypeNum =
			     pDoc->DocLastEdit->EoLastSelectedEl->ElTypeNumber;
             notifyEl.elementType.ElSSchema =
	       (SSchema) (pDoc->DocLastEdit->EoLastSelectedEl->ElStructSchema);
             notifyEl.position = 0;
             CallEventType ((NotifyEvent *) & notifyEl, FALSE);
	     }
	   }
         doit = FALSE;
	 }
      if (pDoc->DocLastEdit->EoType == EtAttribute)
	 {
	 notifyAttr.document = doc;
	 notifyAttr.element = (Element) (pDoc->DocLastEdit->EoElement);
	 /* delete the attribute that has to be removed from the element */
	 if (pDoc->DocLastEdit->EoElement &&
	     pDoc->DocLastEdit->EoCreatedAttribute)
	    {
	    /* tell the application that an attribute will be removed */
	    notifyAttr.event = TteAttrDelete;
	    notifyAttr.attribute =
			   (Attribute) (pDoc->DocLastEdit->EoCreatedAttribute);
	    notifyAttr.attributeType.AttrSSchema =
	      (SSchema) (pDoc->DocLastEdit->EoCreatedAttribute->AeAttrSSchema);
	    notifyAttr.attributeType.AttrTypeNum =
			      pDoc->DocLastEdit->EoCreatedAttribute->AeAttrNum;
	    CallEventAttribute (&notifyAttr, TRUE);
	    /* remove the attribute */
	    TtaRemoveAttribute ((Element) (pDoc->DocLastEdit->EoElement),
		     (Attribute)(pDoc->DocLastEdit->EoCreatedAttribute), doc);
	    notifyAttr.attribute = NULL;
	    /* tell the application that an attribute has been removed */
	    CallEventAttribute (&notifyAttr, FALSE);	    
	    }
	 /* put the saved attribute (if any) on the element */
	 if (pDoc->DocLastEdit->EoElement &&
	     pDoc->DocLastEdit->EoSavedAttribute)
	    {
	    /* tell the application that an attribute will be created */
	    notifyAttr.event = TteAttrCreate;
	    notifyAttr.attribute = NULL;
	    notifyAttr.attributeType.AttrSSchema =
		(SSchema) (pDoc->DocLastEdit->EoSavedAttribute->AeAttrSSchema);
	    notifyAttr.attributeType.AttrTypeNum =
				pDoc->DocLastEdit->EoSavedAttribute->AeAttrNum;
	    CallEventAttribute (&notifyAttr, TRUE);
	    /* put the attribute on the element */
	    TtaAttachAttribute ((Element)(pDoc->DocLastEdit->EoElement),
			(Attribute)(pDoc->DocLastEdit->EoSavedAttribute), doc);
	    /* tell the application that an attribute has been put */
	    notifyAttr.attribute =
			     (Attribute) (pDoc->DocLastEdit->EoSavedAttribute);
	    CallEventAttribute (&notifyAttr, FALSE);	    
	    /* the attribute is no longer associated with the history block */
	    pDoc->DocLastEdit->EoSavedAttribute = NULL;
	    }
	 }
      if (pDoc->DocLastEdit->EoType == EtElement)
	 {
         /* delete the element that has to be removed from the abstract tree */
         if (pDoc->DocLastEdit->EoCreatedElement)
            {
	    pEl = pDoc->DocLastEdit->EoCreatedElement;
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
	    /* remove an element */
            TtaDeleteTree ((Element)pEl, doc);
	    /* tell the application that an element has been removed */
	    CallEventType ((NotifyEvent *) (&notifyEl), FALSE);
	    pDoc->DocLastEdit->EoCreatedElement = NULL;
	    }

         /* insert the saved element in the abstract tree */
         if (pDoc->DocLastEdit->EoSavedElement)
            {
            if (pDoc->DocLastEdit->EoPreviousSibling)
               TtaInsertSibling ((Element)(pDoc->DocLastEdit->EoSavedElement),
			      (Element)(pDoc->DocLastEdit->EoPreviousSibling),
			      FALSE, doc);
            else
               TtaInsertFirstChild ((Element *)&(pDoc->DocLastEdit->EoSavedElement),
				  (Element)(pDoc->DocLastEdit->EoParent), doc);
            /* send event ElemPaste.Post to the application */
            NotifySubTree (TteElemPaste, pDoc,
			   pDoc->DocLastEdit->EoSavedElement, 0);
            pDoc->DocLastEdit->EoSavedElement = NULL;
            }
	 }
   
      /* the most recent editing operation in the history has been undone.
         Remove it from the editing history and put it in the Redo queue */
      MoveEditToRedoQueue (pDoc);
      }
}

/*----------------------------------------------------------------------
   TtcRedo
   Redo the latest sequence of editing operations undone by the TtcUndo.
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
/*****
   PtrElement		pEl;
   NotifyElement	notifyEl;
   NotifyAttribute	notifyAttr;
*****/
   if (!pDoc->DocLastUndone)
     /* no undone command */
      return;

   pDoc = LoadedDocument [doc - 1];
   
   /*********/
}
