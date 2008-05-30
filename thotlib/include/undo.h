/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2005
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

#ifndef _UNDO_H_
#define _UNDO_H_

#ifndef __CEXTRACT__

/*----------------------------------------------------------------------
  TtaSetWithHistory lock or unlock history management 
  (used when resizing or moving an object)
  ----------------------------------------------------------------------*/
extern void TtaLockHistory (ThotBool status);

/* ----------------------------------------------------------------------
   TtaHasUndoSequence returns TRUE if a undo sequence is opened
  ----------------------------------------------------------------------*/
extern ThotBool TtaHasUndoSequence (Document document);

/* ----------------------------------------------------------------------
   TtaOpenUndoSequence

   Open a sequence of editing operations in the history.
   Parameters:
   document: the concerned document
   firstSel: indicate the selection that must be set when the operation 
   will be undone. If null, the current selction is recorded.
   lastSel: indicate the selection that must be set when the operation 
   will be undone.
   firstSelChar: indicate the selection that must be set when the operation 
   will be undone.
   lastSelChar: indicate the selection that must be set when the operation 
   will be undone.
  ----------------------------------------------------------------------*/
extern void TtaOpenUndoSequence (Document document, Element firstSel,
				 Element lastSel, int firstSelChar, int lastSelChar);

/*----------------------------------------------------------------------
   TtaExtendUndoSequence

   Reopen the latest sequence of editing operations in the history.
  ----------------------------------------------------------------------*/
extern void TtaExtendUndoSequence (Document document);

/* ----------------------------------------------------------------------
   TtaCloseUndoSequence

   Close a sequence of editing operations in the history.
   Parameters:
   document: the concerned document
   Return value:
       FALSE if the closed sequence is empty, TRUE otherwise
  ----------------------------------------------------------------------*/
extern ThotBool TtaCloseUndoSequence (Document document);


/*----------------------------------------------------------------------
  TtaSetInitialSequence
  Marks the current sequence as the initial sequence.
  ----------------------------------------------------------------------*/
extern void TtaSetInitialSequence (Document document);

/* ----------------------------------------------------------------------
   TtaRegisterElementCreate
   
   Register a single element creation in the editing history
   Registration must be performed AFTER the element is inserted
   Parameters:
   element: the created element
   document: the concerned document
  ----------------------------------------------------------------------*/
extern void TtaRegisterElementCreate (Element element, Document document);

/* ----------------------------------------------------------------------
   TtaRegisterElementDelete
   
   Register a single element Deletion in the editing history
   Registration must be performed BEFORE the element is actually
   removed from the structure.
   Parameters:
   element: the element to be deleted
   document: the concerned document
  ----------------------------------------------------------------------*/
extern void TtaRegisterElementDelete (Element element, Document document);

/* ----------------------------------------------------------------------
   TtaRegisterElementReplace

   Register a single element replacement in the editing history
   Registration must be performed BEFORE the element is actually
   replaced.
   Parameters:
   element: the created element
   document: the concerned document
  ----------------------------------------------------------------------*/
extern void TtaRegisterElementReplace (Element element, Document document);

/* ----------------------------------------------------------------------
   TtaRegisterElementTypeChange
    
   Register a changing of element type in the editing history
   Parameters:
   element: the element concerned
   oldElementType: the element type to be restored when undoing
   document: the concerned document
  ----------------------------------------------------------------------*/
extern void TtaRegisterElementTypeChange (Element element, int oldElementType,
				          Document document);

/* ----------------------------------------------------------------------
   TtaRegisterAttributeCreate
 
   Register a single attribute creation in the editing history
   Registration must be performed AFTER the attribute is inserted
   Parameters:
   attribute: the created attribute
   element: the element to which the attribute has been attached
   document: the concerned document
  ----------------------------------------------------------------------*/
extern void TtaRegisterAttributeCreate (Attribute attribute,
					Element element, Document document);

/* ----------------------------------------------------------------------
   TtaRegisterAttributeDelete
 
   Register a single attribute Deletion in the editing history
   Registration must be performed BEFORE the attribute is actually
   removed from the structure.
   Parameters:
   attribute: the attribute to be deleted
   element: the element to which this attribute is attached
   document: the concerned document
  ----------------------------------------------------------------------*/
extern void TtaRegisterAttributeDelete (Attribute attribute,
					Element element, Document document);

/* ----------------------------------------------------------------------
   TtaRegisterAttributeReplace
 
   Register a single attribute replacement in the editing history
   Registration must be performed BEFORE the element is actually
   replaced.
   Parameters:
   attribute: the attribute
   element: the element to which the attribute is attached
   document: the concerned document
  ----------------------------------------------------------------------*/
extern void TtaRegisterAttributeReplace (Attribute attribute,
					 Element element, Document document);

/* ----------------------------------------------------------------------
   TtaClearUndoHistory

   Clears all editing operations registered in the editing history of document.
  ----------------------------------------------------------------------*/
extern void TtaClearUndoHistory (Document document);

/* ----------------------------------------------------------------------
   TtaCancelLastRegisteredSequence
 
   Cancel the last sequence of editing operations registered in the
   editing history of document.
  ----------------------------------------------------------------------*/
extern void TtaCancelLastRegisteredSequence (Document document);

/* ----------------------------------------------------------------------
   TtaChangeLastRegisteredAttr
 
   Change the most recent editing operation registered in the editing history
   of document, only if it's an attribute operation for element oldEl.
   In that case, make it related to element newEl and attribute newAttr.
  ----------------------------------------------------------------------*/
extern void TtaChangeLastRegisteredAttr (Element oldEl, Element newEl,
					 Attribute oldAttr, Attribute newAttr,
					 Document document);

/* ----------------------------------------------------------------------
   TtaReplaceLastRegisteredAttr
 
   Replace the latest operation registered in the editing history of document
   from an attribute value modification to an attribute value deletion.
  ----------------------------------------------------------------------*/
extern void TtaReplaceLastRegisteredAttr (Document document);

/* ----------------------------------------------------------------------
   TtaCancelLastRegisteredOperation
 
   Cancel the latest operation registered in the editing history of document.
  ----------------------------------------------------------------------*/
extern void TtaCancelLastRegisteredOperation (Document document);

/* ----------------------------------------------------------------------
   TtaCancelLastRegisteredAttrOperation

   Cancel the latest attribute registered in the editing history of document.
  ----------------------------------------------------------------------*/
extern void TtaCancelLastRegisteredAttrOperation (Document document);

/* ----------------------------------------------------------------------
   TtaGetLastCreatedElemInHistory

   If the last operation recorded in the history is the creation of an
   element, return that element, otherwise return NULL.
  ----------------------------------------------------------------------*/
extern Element TtaGetLastCreatedElemInHistory (Document document);

/* ----------------------------------------------------------------------
   TtaChangeInfoLastRegisteredElem

   Change the info field associated with the latest operation registered in
   the editing history of document
  ----------------------------------------------------------------------*/
extern void TtaChangeInfoLastRegisteredElem (Document document, int newInfo);

/* ----------------------------------------------------------------------
   TtaUndoNoRedo
 
   Undo the latest sequence of editing operations recorded in the history
   of document and forget about this sequence: it won't be redone by
   the next Redo command issued by the user.
  ----------------------------------------------------------------------*/
extern void TtaUndoNoRedo (Document document);

#endif /* __CEXTRACT__ */

#endif


