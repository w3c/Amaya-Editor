/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

#ifndef _UNDO_H_
#define _UNDO_H_

#ifndef __CEXTRACT__
#ifdef __STDC__

/* ----------------------------------------------------------------------
   TtaOpenUndoSequence

   Open a sequence of editing operations in the history.

   Parameters:

   document:the concerned document
   firstSel: indicate the selection that must be set when the operation 
   will be undone. If null, the current selction is recorded.
   lastSel:indicate the selection that must be set when the operation 
   will be undone.
   firstSelChar:indicate the selection that must be set when the operation 
   will be undone.
   lastSelChar: indicate the selection that must be set when the operation 
   will be undone.
  ----------------------------------------------------------------------*/
extern void          TtaOpenUndoSequence (Document document, Element firstSel,Element lastSel, int firstSelChar, int lastSelChar);

/* ----------------------------------------------------------------------
   TtaCloseUndoSequence

   Close a sequence of editing operations in the history.

   Parameters:

   document: the concerned document
  ----------------------------------------------------------------------*/
extern void          TtaCloseUndoSequence (Document document);

/* ----------------------------------------------------------------------
   TtaRegisterElementCreate
   
   Register a single element creation in the editing history
   The registratration must be performed AFTER the element is inserted

   Parameters:

   element: the created element
   document: the concerned document
  ----------------------------------------------------------------------*/
extern void         TtaRegisterElementCreate (Element element, Document document);

/* ----------------------------------------------------------------------
   TtaRegisterElementDelete
   
   Register a single element Deletion in the editing history
   The registratration must be performed BEFORE the element is actually
   removed from the structure.

   Parameters:

   element: the element to be deleted
   document: the concerned document
  ----------------------------------------------------------------------*/
extern void         TtaRegisterElementDelete (Element element, Document document);

/* ----------------------------------------------------------------------
   TtaRegisterElementReplace

   Register a single element replacement in the editing history

   Parameters:

   element: the created element
   document: the concerned document
  ----------------------------------------------------------------------*/
extern void        TtaRegisterElementReplace (Element element, Document document);

/* ----------------------------------------------------------------------
   TtaClearUndoHistory

   Clears the last sequence of editing operarations registered in the
   editing history of document.
  ----------------------------------------------------------------------*/
extern void         TtaClearUndoHistory (Document document);

#else  /* __STDC__ */
extern void          TtaOpenUndoSequence (/* Document document, Element firstSel,Element lastSel, int firstSelChar, int lastSelChar */);
extern void          TtaCloseUndoSequence (/* Document document */);
extern void         TtaRegisterElementCreate (/* Element element, Document document */);
extern void         TtaRegisterElementDelete (/* Element element, Document document */);
extern void         TtaRegisterElementReplace (/* Element element, Document document */);
extern void         TtaClearUndoHistory (/* Document document */);
#endif /* __STDC__ */
#endif /* __CEXTRACT__ */

#endif
