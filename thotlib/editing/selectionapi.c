/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2008
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
#include "thot_gui.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "application.h"
#include "document.h"
#include "tree.h"
#include "view.h"
#include "typecorr.h"
#include "appdialogue.h"

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "edit_tv.h"
#include "modif_tv.h"
#include "select_tv.h"
#include "appdialogue_tv.h"
#include "frame_tv.h"

#include "applicationapi_f.h"
#include "displayview_f.h"
#include "exceptions_f.h"
#include "structselect_f.h"
#include "thotmsg_f.h"
#include "viewapi_f.h"
#include "views_f.h"

/*----------------------------------------------------------------------
  TtaIsDocumentSelected answers TRUE if the document is selected.  
  ----------------------------------------------------------------------*/
ThotBool TtaIsDocumentSelected (Document document)
{
  PtrDocument         docsel;
  PtrElement          firstSelection, lastSelection;
  int                 firstChar, lastChar;

  if (!GetCurrentSelection (&docsel, &firstSelection, &lastSelection,
                            &firstChar, &lastChar))
    return (FALSE);
  else if (LoadedDocument[document - 1] != docsel)
    return (FALSE);
  else
    return (TRUE);
}

/*----------------------------------------------------------------------
  TtaGetSelectedAttribute
  Return the selected attribute or NULL
  ----------------------------------------------------------------------*/
Attribute TtaGetSelectedAttribute ()
{
  if (AbsBoxSelectedAttr)
    return (Attribute)(AbsBoxSelectedAttr->AbCreatorAttr);
  else
    return NULL;
}

/*----------------------------------------------------------------------
  TtaIsColumnSelected Answers TRUE if a whole table column is selected
  ----------------------------------------------------------------------*/
ThotBool TtaIsColumnSelected (Document document)
{
  PtrDocument         docsel;
  PtrElement          first, last;
  int                 firstChar, lastChar;

  if (!GetCurrentSelection (&docsel, &first, &last, &firstChar, &lastChar))
    return (FALSE);
  else if (LoadedDocument[document - 1] != docsel)
    return (FALSE);
  else if (WholeColumnSelected)
    return (TRUE);
  else
    return (FALSE);
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
void TtaSelectElement (Document document, Element selectedElement)
{
  DisplayMode         dispMode;

  UserErrorCode = 0;
  if (selectedElement && ((PtrElement) selectedElement)->ElParent == NULL)
    TtaError (ERR_invalid_parameter);
  /* Checks the parameter document */
  else if (document < 1 || document > MAX_DOCUMENTS)
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument[document - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  else
    {
      dispMode = TtaGetDisplayMode (document);
      if (dispMode == DisplayImmediately)
        {
        if (selectedElement == NULL)
          /* Abort the selection */
          ResetSelection (LoadedDocument[document - 1]);
        else
          {
            SelectElement (LoadedDocument[document - 1],
                           (PtrElement) selectedElement, TRUE, FALSE, TRUE);
#ifdef _WX
            // update the status bar
            TtaSetStatusSelectedElement (document, 1, selectedElement);
#endif /* _WX */
          }
        }
      else
        {
          CancelSelection ();
          NewSelection (document, selectedElement, NULL, 0, 0);
        }
    }
}

/*----------------------------------------------------------------------
  TtaSelectElementWithoutPath

  Selects a single element but don't update the selection path.
  The element is highlighted in all views
  where it can be displayed. If it cannot be displayed in any existing
  view, a new view is eventually open for displaying it.
  Parameters:
  document: the document containing the element to be
  selected.
  selectedElement: the element to be selected. NULL for cancelling the
  selection in the document.
  ----------------------------------------------------------------------*/
void TtaSelectElementWithoutPath (Document document, Element selectedElement)
{
  DisplayMode         dispMode;

  UserErrorCode = 0;
  if (selectedElement && ((PtrElement) selectedElement)->ElParent == NULL)
    TtaError (ERR_invalid_parameter);
  /* Checks the parameter document */
  else if (document < 1 || document > MAX_DOCUMENTS)
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument[document - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  else
    {
      dispMode = TtaGetDisplayMode (document);
      if (dispMode == DisplayImmediately && selectedElement)
        SelectElement (LoadedDocument[document - 1],
                       (PtrElement) selectedElement, TRUE, FALSE, FALSE);
    }
}

/*----------------------------------------------------------------------
  TtaSelectView posts the selected view in the document
  ----------------------------------------------------------------------*/
void TtaSelectView (Document document, View view)
{
  PtrDocument       pDoc;
  int               oldView;

  UserErrorCode = 0;
  if (document)
    {
      GetActiveView (&pDoc, &oldView);
      if (LoadedDocument[document - 1] == pDoc && oldView != view)
        SelectedView = view;
    }
}

/*----------------------------------------------------------------------
  TtaGetSelectedDocument returns the current selected document
  ----------------------------------------------------------------------*/
Document TtaGetSelectedDocument ()
{
  PtrDocument       pDoc;
  int               oldView;

  UserErrorCode = 0;
  GetActiveView (&pDoc, &oldView);
  if (pDoc == NULL)
    return (0);
  else
    return (IdentDocument (pDoc));
}

/*----------------------------------------------------------------------
  TtaGetActiveView returns the selected document and the active view
  in that document
  ----------------------------------------------------------------------*/
void TtaGetActiveView (Document *doc, View *view)
{
  PtrDocument     pDoc;
  int             docView;

  UserErrorCode = 0;
  *doc = 0;
  *view = 0;
  GetDocAndView (ActiveFrame, &pDoc, &docView);
  //GetActiveView (&pDoc, &docView);
  if (pDoc)
    {
      *doc = IdentDocument (pDoc);
      *view = (View) docView;
    }
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
void  TtaSelectString (Document document, Element textElement,
                       int firstCharacter, int lastCharacter)
{
  DisplayMode         dispMode;

  UserErrorCode = 0;
  if (textElement == NULL)
    TtaError (ERR_invalid_parameter);
  else if (((PtrElement) textElement)->ElParent == NULL)
    TtaError (ERR_invalid_parameter);
  else if (!((PtrElement) textElement)->ElTerminal)
    TtaError (ERR_invalid_element_type);
  else if (((PtrElement) textElement)->ElLeafType != LtText &&
           ((PtrElement) textElement)->ElLeafType != LtSymbol &&
           ((PtrElement) textElement)->ElLeafType != LtPicture)
    TtaError (ERR_invalid_element_type);
  /* Checks the parameter document */
  else if (document < 1 || document > MAX_DOCUMENTS)
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument[document - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  else
    {
      dispMode = TtaGetDisplayMode (document);
      if (dispMode == DisplayImmediately)
        {
          SelectString (LoadedDocument[document - 1],
                        (PtrElement) textElement, firstCharacter, lastCharacter);
        }
      else
        NewSelection (document, textElement, NULL, firstCharacter, lastCharacter);
    }
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
void TtaExtendSelection (Document document, Element element, int lastCharacter)
{
  PtrDocument         pDoc;
  PtrElement          firstSelection, lastSelection;
  DisplayMode         dispMode;
  int                 firstChar, lastChar;
  ThotBool            ok;
  ThotBool            abort;

  UserErrorCode = 0;
  if (element == NULL)
    TtaError (ERR_invalid_parameter);
  else if (((PtrElement) element)->ElParent == NULL)
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
      ok = GetCurrentSelection (&pDoc, &firstSelection, &lastSelection,
                                &firstChar, &lastChar);
      dispMode = TtaGetDisplayMode (document);
      if (dispMode == DisplayImmediately)
        /* The command can be executed */
        {
          if (ok)
            /* verifies that the selection is into the document */
            ok = (pDoc == LoadedDocument[document - 1]);
        }
      /* verifies if a selection is applied */
      else if (IsSelectionRegistered (document, &abort))
        /* There is an application, the extension is accepted if there is 
           a request which is not for aborting */
        ok = !abort;
      
      if (!ok)
        /* Error: no selection */
        TtaError (ERR_no_selection_in_document);
      else if (dispMode == DisplayImmediately)
        ExtendSelection ((PtrElement) element, lastCharacter, TRUE, FALSE, FALSE);
      else
        NewSelectionExtension (document, element, lastCharacter);
    }
}

/*----------------------------------------------------------------------
  TtaAddElementToSelection

  Add a new element to the current selection.  The new selection
  may then contain separate elements.
  Parameters:
  document: the document to which the element belongs.  This element must
	belong to the same document as the elements selected by previous calls
	to TtaAddElementToSelection and by the last call to TtaSelectElement.
  element: the element to be added to the current selection
  ----------------------------------------------------------------------*/
void TtaAddElementToSelection (Document document, Element element)
{
  PtrDocument         pDoc;
  PtrElement          firstSelection, lastSelection;
  DisplayMode         dispMode;
  int                 firstChar, lastChar;
  ThotBool            ok;

  UserErrorCode = 0;
  if (element == NULL)
    TtaError (ERR_invalid_parameter);
  else if (((PtrElement) element)->ElParent == NULL)
    TtaError (ERR_invalid_parameter);
  /* Checks the parameter document */
  else if (document < 1 || document > MAX_DOCUMENTS)
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument[document - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  else
    /* Parameter document is correct */
    {
      /* is there a current selection */
      ok = GetCurrentSelection (&pDoc, &firstSelection, &lastSelection,
                                &firstChar, &lastChar);
      dispMode = TtaGetDisplayMode (document);
      if (ok)
        {
          if (dispMode == NoComputedDisplay)
            /* accept only if immediate display mode */
            ok = FALSE;
          else
            /* is the current selection is in the same document? */
            ok = (pDoc == LoadedDocument[document - 1]);
        }
      if (!ok)
        /* Error: no selection */
        TtaError (ERR_no_selection_in_document);
      else
        AddInSelection ((PtrElement) element, FALSE);
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
void TtaSelectInterval ()
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
void TtaUnselect (Document document)
{
  if (document < 1 || document > MAX_DOCUMENTS)
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument[document - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  /* Parameter document is correct */
  else if (SelectedDocument == LoadedDocument[document - 1])
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
void TtaSetSelectionMode (ThotBool withMenu)
{
  SelectionUpdatesMenus = withMenu;
}

/*----------------------------------------------------------------------
  TtaIsSelectionEmpty

  Returns TRUE if there is a current selection and this selection is
  empty (a caret).
  ----------------------------------------------------------------------*/
ThotBool TtaIsSelectionEmpty ()
{
  if (SelectedDocument == NULL && DocSelectedAttr == NULL)
    return FALSE;
  else if (SelPosition)
    return TRUE;
  else if (FirstSelectedElement &&
           FirstSelectedElement == LastSelectedElement &&
           FirstSelectedElement->ElVolume == 0)
    return TRUE;
  else
    return FALSE;
}

/*----------------------------------------------------------------------
   TtaIsSelectionUnique

   Returns TRUE if there is a current selection and only one element is
   selected.
  ----------------------------------------------------------------------*/
ThotBool TtaIsSelectionUnique ()
{
  if (SelectedDocument == NULL && DocSelectedAttr == NULL)
    return FALSE;
  else if (FirstSelectedElement &&
           (FirstSelectedElement == LastSelectedElement ||
            WholeColumnSelected))
    return TRUE;
  else
    return FALSE;
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
  firstCharacter: rank of the first selected character in the element, or
  0 if the whole element is selected.
  lastCharacter: rank of the last selected character in the element, or
  0 if the whole element is in the selection, or firstCharacter - 1 for
  a position.
  ----------------------------------------------------------------------*/
void TtaGiveFirstSelectedElement (Document document, Element *selectedElement,
                                  int *firstCharacter, int *lastCharacter)
{
  PtrDocument         pDoc;
  PtrElement          firstSelection, lastSelection;
  int                 firstChar, lastChar;
  ThotBool            ok;

  UserErrorCode = 0;
  *selectedElement = NULL;
  *firstCharacter = 0;
  *lastCharacter = 0;
  /* Checks the parameter document */
  if (document < 1 || document > MAX_DOCUMENTS)
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument[document - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  else
    {
      /* Parameter document is correct */
      ok = GetCurrentSelection (&pDoc, &firstSelection, &lastSelection,
                                &firstChar, &lastChar);
      if (ok)
        if (pDoc == LoadedDocument[document - 1])
          {
            *selectedElement = (Element) firstSelection;
            *firstCharacter = firstChar;
            if (lastSelection == firstSelection)
              {
                /* only one element */
                if (SelPosition)
                  /* a position */
                  *lastCharacter = firstChar - 1;
                else
                  *lastCharacter = lastChar;
              }
            else if (firstChar > 0 && lastChar == 0)
              /* the selection starts in the middle of the element */
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
void TtaGiveNextSelectedElement (Document document, Element *selectedElement,
                                 int *firstCharacter, int *lastCharacter)
{
  PtrDocument         pDoc;
  PtrElement          pEl;
  PtrElement          firstSelection, lastSelection;
  int                 firstChar, lastChar;
  ThotBool            ok;

  UserErrorCode = 0;
  *firstCharacter = 0;
  *lastCharacter = 0;
  /* Checks the parameter document */
  if (document < 1 || document > MAX_DOCUMENTS)
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument[document - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  else
    {
      /* Parameter document is correct */
      ok = GetCurrentSelection (&pDoc, &firstSelection, &lastSelection,
                                &firstChar, &lastChar);
      if (!ok)
        *selectedElement = NULL;
      else if (pDoc != LoadedDocument[document - 1])
        *selectedElement = NULL;
      else
        {
          pEl = NextInSelection ((PtrElement) *selectedElement, lastSelection);
          *selectedElement = (Element) pEl;
          if (pEl == lastSelection && lastChar > 0)
            {
              *firstCharacter = 1;
              *lastCharacter = lastChar;
            }
        }
    }
}


/*----------------------------------------------------------------------
  TtaGiveNextElement

  Returns the element that follows a given element in the selection order
  Parameters:
  document: the document for which the selection is asked.
  element: the current element.
  last: end of the range to be searched, usually last selected element.
  Return parameters:
  element: the next element in the selection order. NULL if not found.
  ----------------------------------------------------------------------*/
void TtaGiveNextElement (Document document, Element *element, Element last)
{
  PtrElement          pEl;

  UserErrorCode = 0;
  /* Checks the parameter document */
  if (document < 1 || document > MAX_DOCUMENTS)
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument[document - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  else if (last == NULL)
    TtaError (ERR_invalid_parameter);
  else
    /* Parameters document and last are correct */
    {
      pEl = NextInSelection ((PtrElement) (*element), (PtrElement)last);
      *element = (Element) pEl;
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
  firstCharacter: rank of the first selected character in the element, or
  0 if the whole element is in the selection.
  lastCharacter: rank of the last selected character in the element, or
  0 if the whole element is in the selection, or firstCharacter - 1 for
  a position.
  ----------------------------------------------------------------------*/
void TtaGiveLastSelectedElement (Document document, Element *selectedElement,
                                 int *firstCharacter, int *lastCharacter)
{
  PtrDocument         pDoc;
  PtrElement          firstSelection, lastSelection;
  int                 firstChar, lastChar;
  ThotBool            ok;

  UserErrorCode = 0;
  *selectedElement = NULL;
  *firstCharacter = 0;
  *lastCharacter = 0;
  /* Checks the parameter document */
  if (document < 1 || document > MAX_DOCUMENTS)
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument[document - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  else
    {
      /* Parameter document is correct */
      ok = GetCurrentSelection (&pDoc, &firstSelection, &lastSelection,
                                &firstChar, &lastChar);
      if (ok)
        if (pDoc == LoadedDocument[document - 1])
          {
            *selectedElement = (Element) lastSelection;
            if (lastSelection == firstSelection)
              {
                /* only one element */
                *firstCharacter = firstChar;
                if (SelPosition)
                  /* a position */
                  *lastCharacter = firstChar - 1;
                else
                  *lastCharacter = lastChar;
              }
            else if (lastChar > 0)
              {
                /* the selection ends in the middle of the element */
                *firstCharacter = 1;
                *lastCharacter = lastChar;
              }
          }
    }
}


