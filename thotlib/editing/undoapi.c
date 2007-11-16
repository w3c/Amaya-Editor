/*
 *
 *  (c) COPYRIGHT INRIA 1996-2007
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * This module Implements the Undo API
 *
 * Authors: I. Vatton, S. Bonhomme (INRIA)
 *          
 */
#include "thot_gui.h"
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
#include "select_tv.h"


/* ----------------------------------------------------------------------
   TtaHasUndoSequence returns TRUE if a undo sequence is opened
  ----------------------------------------------------------------------*/
ThotBool TtaHasUndoSequence (Document document)
{

  if (document < 1 || document > MAX_DOCUMENTS)
    return FALSE;
  else if (LoadedDocument [document - 1])
    return (LoadedDocument [document - 1]->DocEditSequence);
  else
    return FALSE;
}


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
void TtaOpenUndoSequence (Document document, Element firstSel,
			  Element lastSel, int firstSelChar,
				   int lastSelChar)
{
  int           i;

  if (document < 1 || document > MAX_DOCUMENTS)
      TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument [document - 1] == NULL)
      TtaError (ERR_invalid_document_parameter);
  else 
    {
      if (firstSel == NULL)
	{
	  /* gets the current selection */
	  TtaGiveFirstSelectedElement (document, &firstSel, &firstSelChar, &i);
	  if (SelPosition)
	    {
	      lastSelChar = i;
	      lastSel = firstSel;
	    }
	  else
	    /* gets the last selected element */
	    TtaGiveLastSelectedElement (document, &lastSel, &i, &lastSelChar);
	}

      /* inits the history sequence */
      OpenHistorySequence (LoadedDocument [document - 1], 
			   (PtrElement)firstSel, (PtrElement)lastSel, NULL,
			   firstSelChar, lastSelChar);	     
    }
}

/*----------------------------------------------------------------------
   TtaExtendUndoSequence
   Reopen the latest sequence of editing operations in the history.
  ----------------------------------------------------------------------*/
void TtaExtendUndoSequence (Document document)
{
  if (document < 1 || document > MAX_DOCUMENTS)
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument [document - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  else 
    LoadedDocument [document - 1]->DocEditSequence = TRUE;
}

/* ----------------------------------------------------------------------
   TtaCloseUndoSequence

   Close a sequence of editing operations in the history.
   Parameters:
   document: the concerned document
   Return value:
       FALSE if the closed sequence is empty, TRUE otherwise
  ----------------------------------------------------------------------*/
ThotBool TtaCloseUndoSequence (Document document)
{
  ThotBool	result;

  result = FALSE;
  if (document < 1 || document > MAX_DOCUMENTS)
      TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument [document - 1] == NULL)
      TtaError (ERR_invalid_document_parameter);
  else 
      result = CloseHistorySequence (LoadedDocument [document - 1]);	     
  return result;
}

/*----------------------------------------------------------------------
  TtaSetInitialSequence
  Marks the current sequence as the initial sequence.
  ----------------------------------------------------------------------*/
void TtaSetInitialSequence (Document document)
{
  if (document < 1 || document > MAX_DOCUMENTS)
      TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument [document - 1] == NULL)
      TtaError (ERR_invalid_document_parameter);
  else 
    NewInitialSequence (LoadedDocument [document - 1]);
}

/* ----------------------------------------------------------------------
   TtaRegisterElementCreate
    
   Register a single element creation in the editing history
   Registration must be performed AFTER the element is inserted
   Parameters:
   element: the created element
   document: the concerned document
  ----------------------------------------------------------------------*/
void TtaRegisterElementCreate (Element element, Document document)
{
  if (document < 1 || document > MAX_DOCUMENTS)
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument [document - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  else 
    AddEditOpInHistory ((PtrElement)element, LoadedDocument [document - 1], 
			FALSE, /* the element does not have to be saved */ 
			TRUE   /* the element will be removed when undoing */
			);
}

/* ----------------------------------------------------------------------
   TtaRegisterElementDelete
    
   Register a single element Deletion in the editing history
   Registration must be performed BEFORE the element is actually
   removed from the structure.
   Parameters:
   element: the element to be deleted
   document: the concerned document
  ----------------------------------------------------------------------*/
void TtaRegisterElementDelete (Element element, Document document)
{
  if (document < 1 || document > MAX_DOCUMENTS)
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument [document - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  else 
    AddEditOpInHistory ((PtrElement)element, LoadedDocument [document - 1], 
			TRUE, /* the element has to be saved */ 
			FALSE  /* the element wont be removed when undoing */
			);
}

/* ----------------------------------------------------------------------
   TtaRegisterElementReplace
    
   Register a single element replacement in the editing history
   Registration must be performed BEFORE the element is actually
   replaced.
   Parameters:
   element: the replaced element
   document: the concerned document
  ----------------------------------------------------------------------*/
void TtaRegisterElementReplace (Element element, Document document)
{
  if (document < 1 || document > MAX_DOCUMENTS)
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument [document - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  else 
    AddEditOpInHistory ((PtrElement)element, LoadedDocument [document - 1], 
			TRUE,  /* the element has to be saved */ 
			TRUE   /* the element has to be removed when undoing */
			);
}

/* ----------------------------------------------------------------------
   TtaRegisterElementTypeChange
    
   Register a changing of element type in the editing history
   Parameters:
   element: the element concerned
   oldElementType: the element type to be restored when undoing
   document: the concerned document
  ----------------------------------------------------------------------*/
void TtaRegisterElementTypeChange (Element element, int oldElementType,
				   Document document)
{
  if (document < 1 || document > MAX_DOCUMENTS)
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument [document - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  else 
    AddChangeTypeOpInHistory ((PtrElement)element, oldElementType,
			      LoadedDocument[document-1]); 
}

/* ----------------------------------------------------------------------
   TtaRegisterAttributeCreate
    
   Register a single attribute creation in the editing history
   Registration must be performed AFTER the attribute is inserted
   Parameters:
   attribute: the created attribute
   element: the element to which the attribute has been attached
   document: the concerned document
  ----------------------------------------------------------------------*/
void TtaRegisterAttributeCreate (Attribute attribute, Element element,
				 Document document)
{
  if (document < 1 || document > MAX_DOCUMENTS)
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument [document - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  else 
    AddAttrEditOpInHistory ((PtrAttribute)attribute, (PtrElement)element,
			LoadedDocument[document-1], 
			FALSE, /* the attribute does not have to be saved */
			TRUE   /* the attribute will be removed when undoing */
			);
}

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
void TtaRegisterAttributeDelete (Attribute attribute, Element element,
				 Document document)
{
  if (document < 1 || document > MAX_DOCUMENTS)
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument [document - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  else 
    AddAttrEditOpInHistory ((PtrAttribute)attribute, (PtrElement)element,
                      LoadedDocument[document-1],
		      TRUE, /* the attribute has to be saved */ 
		      FALSE  /* the attribute wont be removed when undoing */
 		      );
}

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
void TtaRegisterAttributeReplace (Attribute attribute, Element element,
				  Document document)
{
  if (document < 1 || document > MAX_DOCUMENTS)
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument [document - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  else 
    AddAttrEditOpInHistory ((PtrAttribute)attribute,(PtrElement)element,
                      LoadedDocument[document-1], 
		      TRUE,  /* the attribute has to be saved */ 
		      TRUE   /* the attribute has to be removed when undoing */
 		      );
}

/* ----------------------------------------------------------------------
   TtaClearUndoHistory

   Clears all editing operations registered in the editing history of document.
  ----------------------------------------------------------------------*/
void TtaClearUndoHistory (Document document)
{
  if (document < 1 || document > MAX_DOCUMENTS)
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument [document - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  else 
    ClearHistory (LoadedDocument [document - 1]);
}

/* ----------------------------------------------------------------------
   TtaCancelLastRegisteredSequence

   Cancel the latest sequence of editing operations registered in the
   editing history of document.
  ----------------------------------------------------------------------*/
void TtaCancelLastRegisteredSequence (Document document)
{
  if (document < 1 || document > MAX_DOCUMENTS)
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument [document - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  else 
    CancelLastSequenceFromHistory (LoadedDocument [document - 1]);
}

/* ----------------------------------------------------------------------
   TtaChangeLastRegisteredAttr

   Change the most recent editing operation registered in the editing history
   of document, only if it's an attribute operation for element oldEl.
   In that case, make it related to element newEl and attribute newAttr.
  ----------------------------------------------------------------------*/
void TtaChangeLastRegisteredAttr (Element oldEl, Element newEl,
				  Attribute oldAttr, Attribute newAttr,
				  Document document)
{
  if (document < 1 || document > MAX_DOCUMENTS)
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument [document - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  else 
    ChangeLastRegisteredAttr ((PtrElement)oldEl, (PtrElement)newEl,
			      (PtrAttribute)oldAttr, (PtrAttribute)newAttr,
			      LoadedDocument [document - 1]);
}

/* ----------------------------------------------------------------------
   TtaReplaceLastRegisteredAttr

   Replace the latest operation registered in the editing history of document
   from an attribute value modification to an attribute value deletion.
  ----------------------------------------------------------------------*/
void TtaReplaceLastRegisteredAttr (Document document)
{
  if (document < 1 || document > MAX_DOCUMENTS)
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument [document - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  else 
    ReplaceLastRegisteredAttr (LoadedDocument [document - 1]);
}

/* ----------------------------------------------------------------------
   TtaCancelLastRegisteredOperation

   Cancel the latest operation registered in the editing history of document.
  ----------------------------------------------------------------------*/
void TtaCancelLastRegisteredOperation (Document document)
{
  if (document < 1 || document > MAX_DOCUMENTS)
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument [document - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  else 
    CancelLastEditFromHistory (LoadedDocument [document - 1]);
}

/* ----------------------------------------------------------------------
   TtaCancelLastRegisteredAttrOperation

   Cancel the latest attribute registered in the editing history of document.
  ----------------------------------------------------------------------*/
void TtaCancelLastRegisteredAttrOperation (Document document)
{
  if (document < 1 || document > MAX_DOCUMENTS)
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument [document - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  else 
    CancelLastAttrOperationFromHistory (LoadedDocument [document - 1]);
}

/* ----------------------------------------------------------------------
   TtaGetLastCreatedElemInHistory

   If the last operation recorded in the history is the creation of an
   element, return that element, otherwise return NULL.
  ----------------------------------------------------------------------*/
Element TtaGetLastCreatedElemInHistory (Document document)
{
  Element   el;

  el = NULL;
  if (document < 1 || document > MAX_DOCUMENTS)
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument [document - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  else 
    el = (Element) GetLastCreatedElemInHistory (LoadedDocument [document - 1]);
  return el;
}

/* ----------------------------------------------------------------------
   TtaChangeInfoLastRegisteredElem

   Change the info field associated with the latest operation registered in
   the editing history of document
  ----------------------------------------------------------------------*/
void TtaChangeInfoLastRegisteredElem (Document document, int newInfo)
{
  if (document < 1 || document > MAX_DOCUMENTS)
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument [document - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  else
    ChangeInfoLastRegisteredElem (LoadedDocument [document - 1], newInfo);
}

/* ----------------------------------------------------------------------
   TtaUndoNoRedo

   Undo the latest sequence of editing operations recorded in the history
   of document and forget about this sequence: it won't be redone by
   the next Redo command issued by the user.
  ----------------------------------------------------------------------*/
void TtaUndoNoRedo (Document document)
{
  if (document < 1 || document > MAX_DOCUMENTS)
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument [document - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  else 
    UndoNoRedo (document);
}
