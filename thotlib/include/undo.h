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
   TtaUndo

   Undo the latest sequence of editing operations recorded in the history and
   remove it from the history.
 
   Parameters:
   document: the concerned document
   ---------------------------------------------------------------------- */
extern void          TtaUndo (Document doc);

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

   document: the concerned document
   element: the created element
  ----------------------------------------------------------------------*/
extern void         TtaRegisterElementCreate (Document document, Element element);

/* ----------------------------------------------------------------------
   RegisterElementDelete
   
   Register a single element Deletion in the editing history
   The registratration must be performed BEFORE the element is actually
   removed from the structure.

   Parameters:

   document: the concerned document
   element: the element to be deleted
  ----------------------------------------------------------------------*/
extern void         TtaRegisterElementDelete (Document document, Element element);

/* ----------------------------------------------------------------------
   TtaRegisterElementReplace

   Register a single element replacement in the editing history

   Parameters:

   document: the concerned document
   element: the created element
  ----------------------------------------------------------------------*/
extern void        TtaRegisterElementReplace (Document document, Element element);

/* ----------------------------------------------------------------------
   TtaClearHistory

   Clears the previous registered sequence of editing operarations

  ----------------------------------------------------------------------*/
extern void         TtaClearHistory ();

#else  /* __STDC__ */
extern void          TtaUndo (/* Document doc */);
extern void          TtaOpenUndoSequence (/* Document document, Element firstSel,Element lastSel, int firstSelChar, int lastSelChar */);
extern void          TtaCloseUndoSequence (/* Document document */);
extern void         TtaRegisterElementCreate (/* Document document, Element element */);
extern void         TtaRegisterElementDelete (/* Document document, Element element */);
extern void         TtaRegisterElementReplace (/* Document document, Element element */);
extern void         TtaClearHistory ();
#endif /* __STDC__ */
#endif /* __CEXTRACT__ */

#endif
