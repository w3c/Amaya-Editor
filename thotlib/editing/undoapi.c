/*
 *
 *  (c) COPYRIGHT INRIA 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * This module Implements the Undo API
 *
 * Authors: S. Bonhomme (INRIA)
 *          
 */
#include "ustring.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"

#include "selection.h"
#include "application.h"
#include "document.h"
#include "undo_f.h"
#include "thotmsg_f.h"

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "edit_tv.h"
#undef THOT_EXPORT

/* ----------------------------------------------------------------------
   TtaOpenUndoSequence

   Open a sequence of editing operations in the history.

   Parameters:

   document: the concerned document
   firstSel: indicate the selection that must be set when the operation 
   will be undone. If null, the current selection is recorded.
   lastSel: indicate the selection that must be set when the operation 
   will be undone.
   firstSelChar: indicate the selection that must be set when the operation 
   will be undone.
   lastSelChar: indicate the selection that must be set when the operation 
   will be undone.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void          TtaOpenUndoSequence (Document document, Element firstSel, Element lastSel, int firstSelChar, int lastSelChar)
#else  /* __STDC__ */
void          TtaOpenUndoSequence (document, firstSel, lastSel, firstSelChar, lastSelChar)
Document document;
Element firstSel;
Element lastSel;
int firstSelChar;
int lastSelChar;
#endif /* __STDC__ */
{
  int i;

  if (document < 1 || document > MAX_DOCUMENTS)
    {
      TtaError (ERR_invalid_document_parameter);
    }
  else 
    {
      if (firstSel == NULL)
	{
	  /* gets the current selection */
	  /* gets the first selected element */
	  TtaGiveFirstSelectedElement (document, &firstSel, &firstSelChar, &i);
	  /* gets the last selected element */
	  TtaGiveLastSelectedElement (document, &lastSel, &i, &lastSelChar);
	}

      /* inits the history sequence */
      OpenHistorySequence (LoadedDocument [document - 1], 
			   (PtrElement)firstSel, (PtrElement)lastSel, 
			   firstSelChar, lastSelChar);	     
    }
}

/* ----------------------------------------------------------------------
   TtaCloseUndoSequence

   Close a sequence of editing operations in the history.

   Parameters:

   document: the concerned document
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void          TtaCloseUndoSequence (Document document)
#else /* __STDC__ */
void          TtaCloseUndoSequence (document)
Document document;
#endif /* __STDC__ */

{
  if (document < 1 || document > MAX_DOCUMENTS)
    {
      TtaError (ERR_invalid_document_parameter);
    }
  else 
    {
      CloseHistorySequence (LoadedDocument [document - 1]);	     
    }
}

/* ----------------------------------------------------------------------
   TtaRegisterElementReplace
    
   Register a single element replacement in the editing history
   
   Parameters:

   element: the created element
   document: the concerned document
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void         TtaRegisterElementReplace (Element element, Document document)
#else /* __STDC__ */
void         TtaRegisterElementReplace (element, document)
Element element;
Document document;
#endif /* __STDC__ */
{
  AddEditOpInHistory ((PtrElement)element, LoadedDocument [document - 1], 
		      TRUE,  /* the element has to be saved */ 
		      TRUE   /* the element have to be removed when undoing */
 		      );
}
/* ----------------------------------------------------------------------
   TtaRegisterElementCreate
    
   Register a single element creation in the editing history
   The registratration must be performed AFTER the element is inserted

   Parameters:

   element: the element to be deleted
   document: the concerned document
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void         TtaRegisterElementCreate (Element element, Document document)
#else /* __STDC__ */
void         TtaRegisterElementCreate (element, document)
Element element;
Document document;
#endif /* __STDC__ */
{
  AddEditOpInHistory ((PtrElement)element, LoadedDocument [document - 1], 
		      FALSE, /* the element does not have to be saved */ 
		      TRUE   /* the element will be removed when undoing */
 		      );
}

/* ----------------------------------------------------------------------
   TtaRegisterElementDelete
    
   Register a single element Deletion in the editing history
   The registratration must be performed BEFORE the element is actually
   removed from the structure.

   Parameters:

   element: the element to be deleted
   document: the concerned document
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void         TtaRegisterElementDelete (Element element, Document document)
#else /* __STDC__ */
void         TtaRegisterElementDelete (element, document)
Element element;
Document document;
#endif /* __STDC__ */
{
  AddEditOpInHistory ((PtrElement)element, LoadedDocument [document - 1], 
		      TRUE, /* the element has to be saved */ 
		      FALSE  /* the element wont be removed when undoing */
 		      );
}

/* ----------------------------------------------------------------------
   TtaClearUndoHistory

   Clears the last sequence of editing operarations registered in the
   editing history of document.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void         TtaClearUndoHistory (Document document)
#else /* __STDC__ */
void         TtaClearUndoHistory (document)
Document document;
#endif /* __STDC__ */
{
   ClearHistory (LoadedDocument [document - 1]);
}
