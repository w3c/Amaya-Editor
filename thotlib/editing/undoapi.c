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
 *          I. Vatton (INRIA)
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
   TtaWithinUndoSequence returns TRUE if a undo sequence is opened
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool      TtaWithinUndoSequence (Document document)
#else  /* __STDC__ */
ThotBool      TtaWithinUndoSequence (document)
Document document;
#endif /* __STDC__ */
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
      TtaError (ERR_invalid_document_parameter);
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

   Return value:
       FALSE if the closed sequence is empty, TRUE otherwise
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool      TtaCloseUndoSequence (Document document)
#else /* __STDC__ */
ThotBool      TtaCloseUndoSequence (document)
Document document;
#endif /* __STDC__ */

{
  ThotBool	result;

  result = FALSE;
  if (document < 1 || document > MAX_DOCUMENTS)
    {
      TtaError (ERR_invalid_document_parameter);
    }
  else 
    {
      result = CloseHistorySequence (LoadedDocument [document - 1]);	     
    }
  return result;
}

/* ----------------------------------------------------------------------
   TtaRegisterElementCreate
    
   Register a single element creation in the editing history
   Registration must be performed AFTER the element is inserted

   Parameters:

   element: the created element
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
   Registration must be performed BEFORE the element is actually
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
   TtaRegisterElementReplace
    
   Register a single element replacement in the editing history
   Registration must be performed BEFORE the element is actually
   replaced.
   
   Parameters:
   element: the replaced element
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
		      TRUE   /* the element has to be removed when undoing */
 		      );
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
#ifdef __STDC__
void         TtaRegisterAttributeCreate (Attribute attribute, Element element, Document document)
#else /* __STDC__ */
void         TtaRegisterAttributeCreate (attribute, element, document)
Attribute attribute;
Element element;
Document document;
#endif /* __STDC__ */
{
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
#ifdef __STDC__
void         TtaRegisterAttributeDelete (Attribute attribute, Element element, Document document)
#else /* __STDC__ */
void         TtaRegisterAttributeDelete (attribute, element, document)
Attribute attribute;
Element element;
Document document;
#endif /* __STDC__ */
{
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
#ifdef __STDC__
void         TtaRegisterAttributeReplace (Attribute attribute, Element element, Document document)
#else /* __STDC__ */
void         TtaRegisterAttributeReplace (attribute, element, document)
Attribute attribute;
Element element;
Document document;
#endif /* __STDC__ */
{
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
#ifdef __STDC__
void         TtaClearUndoHistory (Document document)
#else /* __STDC__ */
void         TtaClearUndoHistory (document)
Document document;
#endif /* __STDC__ */
{
   ClearHistory (LoadedDocument [document - 1]);
}

/* ----------------------------------------------------------------------
   TtaCancelLastRegisteredSequence

   Cancel the latest sequence of editing operations registered in the
   editing history of document.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void         TtaCancelLastRegisteredSequence (Document document)
#else /* __STDC__ */
void         TtaCancelLastRegisteredSequence (document)
Document document;
#endif /* __STDC__ */
{
   CancelLastSequenceFromHistory (LoadedDocument [document - 1]);
}

/* ----------------------------------------------------------------------
   TtaChangeLastRegisteredAttr

   Change the most recent editing operation registered in the editing history
   of document, only if it's an attribute operation for element oldEl.
   In that case, make it related to element newEl and attribute newAttr.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void         TtaChangeLastRegisteredAttr (Element oldEl, Element newEl, Attribute oldAttr, Attribute newAttr, Document document)
#else /* __STDC__ */
void         TtaChangeLastRegisteredAttr (oldEl, newEl, oldAttr, newAttr, document)
Element oldEl;
Element newEl;
Attribute oldAttr;
Attribute newAttr;
Document document;
#endif /* __STDC__ */
{
   ChangeLastRegisteredAttr ((PtrElement)oldEl, (PtrElement)newEl, (PtrAttribute)oldAttr, (PtrAttribute)newAttr, LoadedDocument [document - 1]);
}

/* ----------------------------------------------------------------------
   TtaReplaceLastRegisteredAttr

   Replace the latest operation registered in the editing history of document
   from an attribute value modification to an attribute value deletion.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void         TtaReplaceLastRegisteredAttr (Document document)
#else /* __STDC__ */
void         TtaReplaceLastRegisteredAttr (document)
Document document;
#endif /* __STDC__ */
{
   ReplaceLastRegisteredAttr (LoadedDocument [document - 1]);
}

/* ----------------------------------------------------------------------
   TtaCancelLastRegisteredOperation

   Cancel the latest operation registered in the editing history of document.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void         TtaCancelLastRegisteredOperation (Document document)
#else /* __STDC__ */
void         TtaCancelLastRegisteredOperation (document)
Document document;
#endif /* __STDC__ */
{
   CancelLastEditFromHistory (LoadedDocument [document - 1]);
}

/* ----------------------------------------------------------------------
   TtaUndoNoRedo

   Undo the latest sequence of editing operations recorded in the history
   of document and forget about this sequence: it won't be redone by
   the next Redo command issued by the user.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void         TtaUndoNoRedo (Document document)
#else /* __STDC__ */
void         TtaUndoNoRedo (document)
Document document;
#endif /* __STDC__ */
{
   UndoNoRedo (document);
}
