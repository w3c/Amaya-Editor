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
   TtaUndo

   Undo the latest sequence of editing operations recorded in the history and
   remove it from the history.
 
   Parameters:
   document: the concerned document
   ---------------------------------------------------------------------- */
#ifdef __STDC__
void          TtaUndo (Document document)
#else  /* __STDC__ */
void          TtaUndo (document)
Document doc;

#endif /* __STDC__ */
{
  if (document < 1 || document > MAX_DOCUMENTS)
    {
      TtaError (ERR_invalid_document_parameter);
    }
  else
    {
      TtcUndo (document, 0);
    }
}


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
#ifdef __STDC__
void          TtaOpenUndoSequence (Document document, Element firstSel,Element lastSel, int firstSelChar, int lastSelChar)
#else  /* __STDC__ */
void          TtaOpenUndoSequence (document, firstSel, lastSel, firstSelChar, lastSelChar)
Document document;
Element firstSel;
Element lastSel;
int firstSelChar;
int lastSelChar;
#endif /* __STDC__ */
{
  int fc;
  int lc;
  Element nextSel;

  if (document < 1 || document > MAX_DOCUMENTS)
    {
      TtaError (ERR_invalid_document_parameter);
    }
  else 
    {
      if (firstSel == NULL)
	{
	  /* gets the current selection */
	  /* gets the first selected */
	  TtaGiveFirstSelectedElement (document, &firstSel, 
				       &fc, &lc);
	  nextSel = firstSel;
	  firstSelChar = fc;
	  /* lookup the last selected */
	  while (nextSel != NULL)
	    {
	      lastSel = nextSel;
	      TtaGiveFirstSelectedElement (document, &nextSel, 
					   &fc, &lc);
	      lastSelChar = lc;
	    }
	}

      /* inits the history sequence */
      if (firstSel != NULL)
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

   document: the concerned document
   element: the created element
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void         TtaRegisterElementReplace (Document document, Element element)
#else /* __STDC__ */
void         TtaRegisterElementReplace (document, element)
Document document;
Element element;
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

   document: the concerned document
   element: the element to be deleted
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void         TtaRegisterElementCreate (Document document, Element element)
#else /* __STDC__ */
void         TtaRegisterElementCreate (document, element)
Document document;
Element element;
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

   document: the concerned document
   element: the element to be deleted
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void         TtaRegisterElementDelete (Document document, Element element)
#else /* __STDC__ */
void         TtaRegisterElementDelete (document, element)
Document document;
Element element;
#endif /* __STDC__ */
{
  AddEditOpInHistory ((PtrElement)element, LoadedDocument [document - 1], 
		      TRUE, /* the element has to be saved */ 
		      FALSE  /* the element wont be removed when undoing */
 		      );
}

/* ----------------------------------------------------------------------
   TtaClearHistory

   Clears the previous registered sequence of editing operarations

  ----------------------------------------------------------------------*/
#ifdef __STDC__
void         TtaClearHistory ()
#else /* __STDC__ */
void         TtaClearHistory ()
#endif /* __STDC__ */
{
   ClearHistory ();
}
