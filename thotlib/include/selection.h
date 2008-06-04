/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996-2008
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

#ifndef _SELECTION_H_
#define _SELECTION_H_
#include "typebase.h"
#include "tree.h"
#include "document.h"

#ifndef __CEXTRACT__


/*----------------------------------------------------------------------
   TtaCancelSelection removes the current selection. 
  ----------------------------------------------------------------------*/
extern void TtaCancelSelection (Document document);

/*----------------------------------------------------------------------
   TtaIsDocumentSelected answers TRUE if the document is selected.  
  ----------------------------------------------------------------------*/
extern ThotBool TtaIsDocumentSelected (Document document);


/*----------------------------------------------------------------------
  TtaIsColumnSelected answers TRUE if a whole table column is selected.
  ----------------------------------------------------------------------*/
extern ThotBool TtaIsColumnSelected (Document document);

/*----------------------------------------------------------------------
  TtaIsColumnRowSaved answers TRUE if there is a column or a row in
  the Thot Clipboard.
  ----------------------------------------------------------------------*/
extern ThotBool TtaIsColumnSaved (Document document);

/*----------------------------------------------------------------------
  TtaIsColumnRowSaved answers TRUE if there is a column or a row in
  the Thot Clipboard.
  ----------------------------------------------------------------------*/
extern ThotBool TtaIsRowSaved (Document document);

/*----------------------------------------------------------------------
  TtaSelectWord selects the word around the current point in an element
  ----------------------------------------------------------------------*/
extern void TtaSelectWord (Element element, int pos, Document doc, View view);

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
extern void TtaSelectElement (Document document, Element selectedElement);

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
extern void TtaSelectElementWithoutPath (Document document, Element selectedElement);

/*----------------------------------------------------------------------
   TtaGetSelectedDocument returns the current selected document
  ----------------------------------------------------------------------*/
extern Document TtaGetSelectedDocument ();

/*----------------------------------------------------------------------
   TtaGetActiveView returns the selected document and the active view
   in that document
  ----------------------------------------------------------------------*/
extern void TtaGetActiveView (Document *doc, View *view);

/*----------------------------------------------------------------------
   TtaSelectView posts the selected view in the document
  ----------------------------------------------------------------------*/
extern void TtaSelectView (Document document, View view);

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
extern void TtaSelectString (Document document, Element textElement,
                             int firstCharacter, int lastCharacter);

/*----------------------------------------------------------------------
   TtaExtendSelection

   Extends the current selection to a given element.

   Parameters:
   document: the document for which the selection must be extended.
   element: the element to which the selection must be extended.
   lastCharacter: position within this element of the last character
   to be selected. 0 if the whole element must be selected.

  ----------------------------------------------------------------------*/
extern void TtaExtendSelection (Document document, Element element, int lastCharacter);

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
extern void TtaAddElementToSelection (Document document, Element element);

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
extern void TtaSelectInterval ();

/*----------------------------------------------------------------------
   TtaUnselect

   Turns the selection off for a document.
   Parameters:
   document: the document for which the selection must be turned off
   Return value:
   No return value
  ----------------------------------------------------------------------*/
extern void TtaUnselect (Document document);

/*----------------------------------------------------------------------
   TtaSetSelectionMode

   Change selection mode. If withMenu = FALSE 
   functions TtaSelectElement, TtaSelectString, TtaExtendSelection and
   TtaUnselect do not update the menus that depend on the selection
   and do not display the selection message. Default mode is withMenu = TRUE.
   Parameter:
   withMenu: the new selection mode.
  ----------------------------------------------------------------------*/
extern void TtaSetSelectionMode (ThotBool withMenu);

/*----------------------------------------------------------------------
   TtaIsSelectionEmpty

   Returns TRUE if there is a current selection and this selection is 
   empty (a caret).
  ----------------------------------------------------------------------*/
extern ThotBool TtaIsSelectionEmpty ();

/*----------------------------------------------------------------------
   TtaIsSelectionUnique

   Returns TRUE if there is a current selection and only one element is
   selected.
  ----------------------------------------------------------------------*/
extern ThotBool TtaIsSelectionUnique ();

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
extern void TtaGiveFirstSelectedElement (Document document,
                                         /*OUT*/ Element *selectedElement,
                                         /*OUT*/ int *firstCharacter,
                                         /*OUT*/ int *lastCharacter);

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
extern void TtaGiveNextSelectedElement (Document document,
                                        /*OUT*/ Element *selectedElement,
                                        /*OUT*/ int *firstCharacter,
                                        /*OUT*/ int *lastCharacter);

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
extern void TtaGiveNextElement (Document document, /*OUT*/ Element *element, Element last);

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
extern void TtaGiveLastSelectedElement (Document document,
                                        /*OUT*/ Element *selectedElement,
                                        /*OUT*/ int *firstCharacter,
                                        /*OUT*/ int *lastCharacter);

/*----------------------------------------------------------------------
   TtaSetMoveForwardCallback permet de connecter une fonction de
   l'application a la touche de deplacement du curseur vers l'avant
  ----------------------------------------------------------------------*/
extern void TtaSetMoveForwardCallback (Func callbackFunc);

/*----------------------------------------------------------------------
   TtaSetMoveBackwardCallback permet de connecter une fonction de
   l'application a la touche de deplacement du curseur vers l'arriere
  ----------------------------------------------------------------------*/
extern void TtaSetMoveBackwardCallback (Func callbackFunc);

/*----------------------------------------------------------------------
   TtaGetColumn
   Returns the column element which includes the element el.
  ----------------------------------------------------------------------*/
extern Element TtaGetColumn (Element el);

/*----------------------------------------------------------------------
   TtaSelectEnclosingColumn
   Select the enclosing column of the element.
  ----------------------------------------------------------------------*/
extern void TtaSelectEnclosingColumn (Element el);

/*----------------------------------------------------------------------
   TtaGetSelectedAttribute
   Return the selected attribute or NULL
  ----------------------------------------------------------------------*/
extern Attribute TtaGetSelectedAttribute ();
#endif /* __CEXTRACT__ */

#endif
