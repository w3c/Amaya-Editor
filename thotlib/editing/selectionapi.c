/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 
#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "application.h"
#include "document.h"
#include "tree.h"
#include "view.h"
#include "typecorr.h"

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "edit_tv.h"
#include "select_tv.h"
extern int          UserErrorCode;

#include "applicationapi_f.h"
#include "structselect_f.h"
#include "thotmsg_f.h"
#include "viewapi_f.h"


/*----------------------------------------------------------------------
   TtaIsDocumentSelected answers TRUE if the document is selected.  
  ----------------------------------------------------------------------*/
#ifdef __STDC__
boolean             TtaIsDocumentSelected (Document document)
#else  /* __STDC__ */
boolean             TtaIsDocumentSelected (document)
Document            document;

#endif /* __STDC__ */
{
   PtrDocument         docsel;
   PtrElement          firstSelection, lastSelection;
   int                 firstChar, lastChar;

   if (!GetCurrentSelection (&docsel, &firstSelection, &lastSelection, &firstChar, &lastChar))
      return (FALSE);
   else if (LoadedDocument[document - 1] != docsel)
      return (FALSE);
   else
      return (TRUE);
}

/*----------------------------------------------------------------------
   TtaSelectElement

   Selects a single element. This element is highlighted in all views
   where it can be displayed. If it cannot be displayed in any existing
   view, a new view is eventually open for displaying it.

   Parameters:
   document: the document containing the element to be
   selected.
   selectedElement: the element to be selected. NULL for cancelling the
   selection in the document.

  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaSelectElement (Document document, Element selectedElement)
#else  /* __STDC__ */
void                TtaSelectElement (document, selectedElement)
Document            document;
Element             selectedElement;

#endif /* __STDC__ */
{
   UserErrorCode = 0;

   /* Checks the parameter document */
   if (document < 1 || document > MAX_DOCUMENTS)
      TtaError (ERR_invalid_document_parameter);
   else if (LoadedDocument[document - 1] == NULL)
      TtaError (ERR_invalid_document_parameter);
   else if (TtaGetDisplayMode (document) == DisplayImmediately)
      if (selectedElement == NULL)
	 /* Abort the selection */
	 ResetSelection (LoadedDocument[document - 1]);
      else
	 SelectElement (LoadedDocument[document - 1], (PtrElement) selectedElement, TRUE, FALSE);
   else
      NewSelection (document, selectedElement, 0, 0);
}

/*----------------------------------------------------------------------
   TtaSelectView posts the selected view in the document
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaSelectView (Document document, View view)
#else  /* __STDC__ */
void                TtaSelectView (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
  PtrDocument       pDoc;
  int               oldView;
  boolean           assoc;

   UserErrorCode = 0;
   GetActiveView (&pDoc, &oldView, &assoc);
   if (LoadedDocument[document - 1] == pDoc && !assoc && oldView != view)
     SelectedView = view;
}

/*----------------------------------------------------------------------
   TtaGetSelectedDocument returns the current selected document
  ----------------------------------------------------------------------*/
Document   TtaGetSelectedDocument ()
{
  PtrDocument       pDoc;
  int               oldView;
  boolean           assoc;

   UserErrorCode = 0;
   GetActiveView (&pDoc, &oldView, &assoc);
   if (pDoc == NULL)
     return (0);
   else
     return (IdentDocument (pDoc));
}

/*----------------------------------------------------------------------
   TtaSelectString

   Selects a substring within a Text basic element. This substring is
   highlighted in all views where it can be displayed.

   Parameters:
   document: the document containing the string to be selected.
   textElement: the Text element containing the string to be selected.
   firstCharacter: position within the text element of the first character
   to be selected.
   lastCharacter: position within the text element of the last character
   to be selected.

  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaSelectString (Document document, Element textElement, int firstCharacter, int lastCharacter)
#else  /* __STDC__ */
void                TtaSelectString (document, textElement, firstCharacter, lastCharacter)
Document            document;
Element             textElement;
int                 firstCharacter;
int                 lastCharacter;

#endif /* __STDC__ */
{
   UserErrorCode = 0;
   if (textElement == NULL)
      TtaError (ERR_invalid_parameter);
   else if (((PtrElement) textElement)->ElParent == NULL)
      TtaError (ERR_invalid_parameter);
   else if (!((PtrElement) textElement)->ElTerminal)
      TtaError (ERR_invalid_element_type);
   else if (((PtrElement) textElement)->ElLeafType != LtText)
      TtaError (ERR_invalid_element_type);
   /* Checks the parameter document */
   else if (document < 1 || document > MAX_DOCUMENTS)
      TtaError (ERR_invalid_document_parameter);
   else if (LoadedDocument[document - 1] == NULL)
      TtaError (ERR_invalid_document_parameter);
   else if (TtaGetDisplayMode (document) == DisplayImmediately)
      SelectString (LoadedDocument[document - 1],
	       (PtrElement) textElement, firstCharacter, lastCharacter);
   else
      NewSelection (document, textElement, firstCharacter, lastCharacter);
}

/*----------------------------------------------------------------------
   TtaExtendSelection

   Extends the current selection to a given element.

   Parameters:
   document: the document for which the selection must be extended.
   element: the element to which the selection must be extended.
   lastCharacter: position within this element of the last character
   to be selected. 0 if the whole element must be selected.

  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaExtendSelection (Document document, Element element, int lastCharacter)
#else  /* __STDC__ */
void                TtaExtendSelection (document, element, lastCharacter)
Document            document;
Element             element;
int                 lastCharacter;

#endif /* __STDC__ */
{
   PtrDocument         pDoc;
   PtrElement          firstSelection, lastSelection;
   int                 firstChar, lastChar;
   boolean             ok;
   boolean             abort;

   UserErrorCode = 0;
   if (element == NULL)
      TtaError (ERR_invalid_parameter);
   /* Checks the parameter document */
   else if (document < 1 || document > MAX_DOCUMENTS)
      TtaError (ERR_invalid_document_parameter);
   else if (LoadedDocument[document - 1] == NULL)
      TtaError (ERR_invalid_document_parameter);
   else
      /* Parameter document is correct */
     {
	/* verifies if there is a selection */
	ok = GetCurrentSelection (&pDoc, &firstSelection, &lastSelection, &firstChar, &lastChar);
	if (TtaGetDisplayMode (document) == DisplayImmediately)
	   /* The document is with an immediat display mode */
	  {
	     if (ok)
		/* verifies that the selection is into the document */
		ok = (pDoc == LoadedDocument[document - 1]);
	  }
	/* The document is with deferred display mode */
	/* verifies if a selection is applied */
	else if (DemandeSelEnregistree (document, &abort))
	   /* There is an application, the extension is accepted if there is 
	      a request which is not for aborting */
	   ok = !abort;

	if (!ok)
	   /* Error: no selection */
	   TtaError (ERR_no_selection_in_document);
	else if (TtaGetDisplayMode (document) == DisplayImmediately)
	   ExtendSelection ((PtrElement) element, lastCharacter, FALSE, FALSE, FALSE);
	else
	   NewSelectionExtension (document, element, lastCharacter);
     }
}

/*----------------------------------------------------------------------
   TtaSelectInterval

   If a pair of paired elements is selected, select also all elements
   between the two paired elements. Nothing is done if the current
   selection is not a pair.

   Parameters:
   No parameter

   Return value:
   No return value

  ----------------------------------------------------------------------*/
void                TtaSelectInterval ()
{
   SelectPairInterval ();
}


/*----------------------------------------------------------------------
   TtaUnselect

   Turns the selection off for a document.

   Parameters:
   document: the document for which the selection must be turned off

   Return value:
   No return value

  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                TtaUnselect (Document document)

#else  /* __STDC__ */
void                TtaUnselect (document)
Document            document;

#endif /* __STDC__ */

{
   if (document < 1 || document > MAX_DOCUMENTS)
     {
	TtaError (ERR_invalid_document_parameter);
     }
   else if (LoadedDocument[document - 1] == NULL)
     {
	TtaError (ERR_invalid_document_parameter);
     }
   else
      /* Parameter document is correct */
   if (SelectedDocument == LoadedDocument[document - 1])
      CancelSelection ();
}


/*----------------------------------------------------------------------
   TtaSetSelectionMode

   Change selection mode. If withMenu = FALSE 
   functions TtaSelectElement, TtaSelectString, TtaExtendSelection and
   TtaUnselect do not update the menus that depend on the selection
   and do not display the selection message. Default mode is withMenu = TRUE.

   Parameter:
   withMenu: the new selection mode.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaSetSelectionMode (boolean withMenu)
#else  /* __STDC__ */
void                TtaSetSelectionMode (withMenu)
boolean             withMenu;

#endif /* __STDC__ */
{
   SelectionUpdatesMenus = withMenu;
}

/*----------------------------------------------------------------------
   TtaGiveFirstSelectedElement

   Returns the first element in the current selection in a given document.
   If this element is a Text element and if only a substring is selected,
   return also the rank of the first and last characters in the selection.

   Parameter:
   document: the document for which the selection is asked.

   Return values:
   selectedElement: the first selected element, NULL if no element is
   selected in the document.
   firstCharacter: rank of the first character in the selection, or 0 if the
   whole element is in the selection.
   lastCharacter: rank of the last character in the selection, or 0 if the
   whole element is in the selection.

  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                TtaGiveFirstSelectedElement (Document document, Element * selectedElement, int *firstCharacter, int *lastCharacter)

#else  /* __STDC__ */
void                TtaGiveFirstSelectedElement (document, selectedElement, firstCharacter, lastCharacter)
Document            document;
Element            *selectedElement;
int                *firstCharacter;
int                *lastCharacter;

#endif /* __STDC__ */

{
   PtrDocument         pDoc;
   PtrElement          firstSelection, lastSelection;
   int                 firstChar, lastChar;
   boolean             ok;

   UserErrorCode = 0;
   *selectedElement = NULL;
   *firstCharacter = 0;
   *lastCharacter = 0;
   /* Checks the parameter document */
   if (document < 1 || document > MAX_DOCUMENTS)
     {
	TtaError (ERR_invalid_document_parameter);
     }
   else if (LoadedDocument[document - 1] == NULL)
     {
	TtaError (ERR_invalid_document_parameter);
     }
   else
      /* Parameter document is correct */
     {
	ok = GetCurrentSelection (&pDoc, &firstSelection, &lastSelection, &firstChar, &lastChar);
	if (ok)
	   if (pDoc == LoadedDocument[document - 1])
	     {
		*selectedElement = (Element) firstSelection;
		*firstCharacter = firstChar;
		if (lastSelection == firstSelection)
		   if (lastChar > 1)
		      *lastCharacter = lastChar - 1;
		if (firstChar != 0 && lastChar == 0)
		   *lastCharacter = firstSelection->ElVolume;
	     }
     }
}


/*----------------------------------------------------------------------
   TtaGiveNextSelectedElement

   Returns the element that follows a given element in the current selection
   in a given document. If this element is a Text element and if only a
   substring is selected, return also the rank of the first and last
   characters in the selection.

   Parameters:
   document: the document for which the selection is asked.
   selectedElement: the current element.

   Return parameters:
   selectedElement: the next selected element, NULL if
   this was the last selected element in the document.
   firstCharacter: rank of the first character in the selection, or 0 if the
   whole element is in the selection.
   lastCharacter: rank of the last character in the selection, or 0 if the
   whole element is in the selection.

  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaGiveNextSelectedElement (Document document, Element * selectedElement, int *firstCharacter, int *lastCharacter)

#else  /* __STDC__ */
void                TtaGiveNextSelectedElement (document, selectedElement, firstCharacter, lastCharacter)
Document            document;
Element            *selectedElement;
int                *firstCharacter;
int                *lastCharacter;

#endif /* __STDC__ */

{
   PtrDocument         pDoc;
   PtrElement          pEl;
   PtrElement          firstSelection, lastSelection;
   int                 firstChar, lastChar;
   boolean             ok;

   UserErrorCode = 0;
   *firstCharacter = 0;
   *lastCharacter = 0;
   /* Checks the parameter document */
   if (document < 1 || document > MAX_DOCUMENTS)
     {
	TtaError (ERR_invalid_document_parameter);
     }
   else if (LoadedDocument[document - 1] == NULL)
     {
	TtaError (ERR_invalid_document_parameter);
     }
   else
      /* Parameter document is correct */
     {
	ok = GetCurrentSelection (&pDoc, &firstSelection, &lastSelection, &firstChar, &lastChar);
	if (!ok)
	   *selectedElement = NULL;
	else if (pDoc != LoadedDocument[document - 1])
	   *selectedElement = NULL;
	else
	  {
	     pEl = NextInSelection ((PtrElement) * selectedElement, lastSelection);
	     *selectedElement = (Element) pEl;
	     if (pEl == lastSelection && lastChar > 0)
	       {
		  *firstCharacter = 1;
		  *lastCharacter = lastChar - 1;
	       }
	  }
     }
}


/*----------------------------------------------------------------------
   TtaGiveLastSelectedElement

   Returns the last element in the current selection in a given document.
   If this element is a Text element and if only a substring is selected,
   return also the rank of the first and last characters in the selection.

   Parameter:
   document: the document for which the selection is asked.

   Return values:
   selectedElement: the last selected element, NULL if no element is
   selected in the document.
   firstCharacter: rank of the first character in the selection, or 0 if the
   whole element is in the selection.
   lastCharacter: rank of the last character in the selection, or 0 if the
   whole element is in the selection.

  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                TtaGiveLastSelectedElement (Document document, Element * selectedElement, int *firstCharacter, int *lastCharacter)

#else  /* __STDC__ */
void                TtaGiveLastSelectedElement (document, selectedElement, firstCharacter, lastCharacter)
Document            document;
Element            *selectedElement;
int                *firstCharacter;
int                *lastCharacter;

#endif /* __STDC__ */

{
   PtrDocument         pDoc;
   PtrElement          firstSelection, lastSelection;
   int                 firstChar, lastChar;
   boolean             ok;

   UserErrorCode = 0;
   *selectedElement = NULL;
   *firstCharacter = 0;
   *lastCharacter = 0;
   /* Checks the parameter document */
   if (document < 1 || document > MAX_DOCUMENTS)
     {
	TtaError (ERR_invalid_document_parameter);
     }
   else if (LoadedDocument[document - 1] == NULL)
     {
	TtaError (ERR_invalid_document_parameter);
     }
   else
      /* Parameter document is correct */
     {
	ok = GetCurrentSelection (&pDoc, &firstSelection, &lastSelection, &firstChar, &lastChar);
	if (ok)
	   if (pDoc == LoadedDocument[document - 1])
	     {
		*selectedElement = (Element) lastSelection;
		if (lastSelection == firstSelection)
		  {
		     *firstCharacter = firstChar;
		     if (lastChar > 1)
			*lastCharacter = lastChar - 1;
		  }
		if (lastChar > 0)
		   *lastCharacter = lastChar - 1;
	     }
     }
}

/* End of  module */
