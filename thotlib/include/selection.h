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
extern boolean      TtaIsDocumentSelected (Document document);

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
   TtaSetSelectionMode

   Change selection mode. If withMenu = FALSE 
   functions TtaSelectElement, TtaSelectString, TtaExtendSelection and
   TtaUnselect do not update the menus that depend on the selection
   and do not display the selection message. Default mode is withMenu = TRUE.

   Parameter:
   withMenu: the new selection mode.
  ----------------------------------------------------------------------*/
extern void         TtaSetSelectionMode (boolean withMenu);

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

extern boolean      TtaIsDocumentSelected ( /*Document document */ );
extern Document   TtaGetSelectedDocument ();
extern void         TtaSelectElement ( /* Document document, Element selectedElement */ );
extern void         TtaSelectView (/*Document document, View view*/);
extern void         TtaSelectString ( /* Document document, Element textElement, int firstCharacter, int lastCharacter */ );
extern void         TtaExtendSelection ( /* Document document, Element element, int lastCharacter */ );
extern void         TtaSelectInterval ();
extern void         TtaUnselect ( /* Document document */ );
extern void         TtaSetSelectionMode ( /* boolean withMenu */ );
extern void         TtaGiveFirstSelectedElement ( /* Document document, Element *selectedElement, int *firstCharacter, int *lastCharacter */ );
extern void         TtaGiveNextSelectedElement ( /* Document document, Element *selectedElement, int *firstCharacter, int *lastCharacter */ );
extern void         TtaGiveLastSelectedElement ( /* Document document, Element *selectedElement, int *firstCharacter, int *lastCharacter */ );

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */

#endif
