/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 
/* 
 * variables representing elements saved by commands Cut or Copy
 */

/* descriptor of first element saved */
THOT_EXPORT PtrPasteElem FirstSavedElement;
/* indicates whether a whole column is saved */
THOT_EXPORT ThotBool     WholeColumnSaved;
/* indicates whether a (sequence of) table row(s) is saved */
THOT_EXPORT ThotBool     TableRowsSaved;
/* the document from which saved elements have been copied or cut */
THOT_EXPORT PtrDocument  DocOfSavedElements;
/* Thot Clipboard */
THOT_EXPORT struct _TextBuffer ClipboardThot;
