/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * Warning:
 * This module is part of the Thot library, which was originally
 * developed in French. That's why some comments are still in
 * French, but their translation is in progress and the full module
 * will be available in English in the next release.
 * 
 */
 
#ifndef _SELECTION_H_
#define _SELECTION_H_
#include "typebase.h"
#include "tree.h"
#include "document.h"

#ifndef __CEXTRACT__
#ifdef __STDC__

/*----------------------------------------------------------------------
   TtaIsDocumentSelected answers TRUE if the document is selected.  
  ----------------------------------------------------------------------*/
extern ThotBool     TtaIsDocumentSelected (Document document);

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
extern void         TtaSelectElement (Document document, Element selectedElement);

/*----------------------------------------------------------------------
   TtaGetSelectedDocument returns the current selected document
  ----------------------------------------------------------------------*/
extern Document   TtaGetSelectedDocument ();

/*----------------------------------------------------------------------
   TtaSelectView posts the selected view in the document
  ----------------------------------------------------------------------*/
extern void         TtaSelectView (Document document, View view);

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
extern void         TtaSelectString (Document document, Element textElement, int firstCharacter, int lastCharacter);

/*----------------------------------------------------------------------
   TtaExtendSelection

   Extends the current selection to a given element.

   Parameters:
   document: the document for which the selection must be extended.
   element: the element to which the selection must be extended.
   lastCharacter: position within this element of the last character
   to be selected. 0 if the whole element must be selected.

  ----------------------------------------------------------------------*/
extern void         TtaExtendSelection (Document document, Element element, int lastCharacter);

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
extern void         TtaAddElementToSelection (Document document, Element element);

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
extern void         TtaSelectInterval ();

/*----------------------------------------------------------------------
   TtaUnselect

   Turns the selection off for a document.

   Parameters:
   document: the document for which the selection must be turned off

   Return value:
   No return value

  ----------------------------------------------------------------------*/
extern void         TtaUnselect (Document document);

/*----------------------------------------------------------------------
  TtaSwitchSelection switches on or off the selection in the current 
  document view according to the toShow value:
  - TRUE if on
  - FALSE if off
  ----------------------------------------------------------------------*/
extern void         TtaSwitchSelection (Document document, View view, ThotBool toShow);

/*----------------------------------------------------------------------
   TtaSetSelectionMode

   Change selection mode. If withMenu = FALSE 
   functions TtaSelectElement, TtaSelectString, TtaExtendSelection and
   TtaUnselect do not update the menus that depend on the selection
   and do not display the selection message. Default mode is withMenu = TRUE.

   Parameter:
   withMenu: the new selection mode.
  ----------------------------------------------------------------------*/
extern void         TtaSetSelectionMode (ThotBool withMenu);

/*----------------------------------------------------------------------
   TtaIsSelectionEmpty

   Returns TRUE if there is a current selection and this selection is 
   empty (a caret).
  ----------------------------------------------------------------------*/
extern ThotBool     TtaIsSelectionEmpty ();

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
extern void         TtaGiveFirstSelectedElement (Document document, /*OUT*/ Element * selectedElement, /*OUT*/ int *firstCharacter, /*OUT*/ int *lastCharacter);

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
extern void         TtaGiveNextSelectedElement (Document document, /*OUT*/ Element * selectedElement, /*OUT*/ int *firstCharacter, /*OUT*/ int *lastCharacter);

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
extern void         TtaGiveNextElement (Document document, /*OUT*/ Element * element, Element last);

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
extern void         TtaGiveLastSelectedElement (Document document, /*OUT*/ Element * selectedElement, /*OUT*/ int *firstCharacter, /*OUT*/ int *lastCharacter);

#else  /* __STDC__ */

extern ThotBool     TtaIsDocumentSelected ( /*Document document */ );
extern Document     TtaGetSelectedDocument ();
extern void         TtaSelectElement ( /* Document document, Element selectedElement */ );
extern void         TtaSelectView (/*Document document, View view*/);
extern void         TtaSelectString ( /* Document document, Element textElement, int firstCharacter, int lastCharacter */ );
extern void         TtaExtendSelection ( /* Document document, Element element, int lastCharacter */ );
extern void         TtaAddElementToSelection (/* Document document, Element element */ );
extern void         TtaSelectInterval ();
extern void         TtaUnselect ( /* Document document */ );
extern void         TtaSwitchSelection (/* Document document, View view, ThotBool toShow */);
extern void         TtaSetSelectionMode ( /* ThotBool withMenu */ );
extern ThotBool     TtaIsSelectionEmpty ();
extern void         TtaGiveFirstSelectedElement ( /* Document document, Element *selectedElement, int *firstCharacter, int *lastCharacter */ );
extern void         TtaGiveNextSelectedElement ( /* Document document, Element *selectedElement, int *firstCharacter, int *lastCharacter */ );
extern void         TtaGiveNextElement ( /* Document document, Element * element, Element last */ );
extern void         TtaGiveLastSelectedElement ( /* Document document, Element *selectedElement, int *firstCharacter, int *lastCharacter */ );

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */

#endif
