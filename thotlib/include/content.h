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
 
#ifndef _CONTENT_H_
#define _CONTENT_H_

#include "language.h"
#include "tree.h"
typedef enum _PicType
  {
    xbm_type, eps_type, xpm_type, gif_type, jpeg_type, png_type, unknown_type
  }
PicType;

#ifndef __CEXTRACT__
#ifdef __STDC__


/*----------------------------------------------------------------------
   TtaSetTextContent

   Changes the content of a Text basic element. The full content (if any) is
   deleted and replaced by the new one.
   This function can also be used for changing the content (the file name)
   of a Picture basic element.

   Parameters:
   element: the Text element to be modified.
   content: new content for that element.
   language: language of that Text element.
   document: the document containing that element.

  ----------------------------------------------------------------------*/
extern void         TtaSetTextContent (Element element, char *content, Language language, Document document);

/*----------------------------------------------------------------------
   TtaAppendTextContent

   Appends a character string at the end of a Text basic element.

   Parameters:
   element: the Text element to be modified.
   content: the character string to be appended.
   document: the document containing that element.

  ----------------------------------------------------------------------*/
extern void         TtaAppendTextContent (Element element, char *content, Document document);

/*----------------------------------------------------------------------
   TtaInsertTextContent

   Inserts a character string in a text basic element.

   Parameters:
   element: the Text element to be modified.
   position: rank of the character after which the new string must
   be inserted. 0 for inserting before the first character.
   content: the character string to be inserted.
   document: the document containing the text element.

  ----------------------------------------------------------------------*/
extern void         TtaInsertTextContent (Element element, int position, char *content, Document document);

/*----------------------------------------------------------------------
   TtaDeleteTextContent

   Deletes a character string in a text basic element.

   Parameters:
   element: the Text element to be modified.
   position: rank of the first character to be deleted.
   length: length of the character string to be deleted.
   document: the document containing the text element.

  ----------------------------------------------------------------------*/
extern void         TtaDeleteTextContent (Element element, int position, int length, Document document);

/*----------------------------------------------------------------------
   TtaSplitText

   Divides a text element into two elements.

   Parameters:
   element: the text element to be divided. A new text element containing
   the second part of the text is created as the next sibling.
   position: rank of the character after which the element must be cut.
   document: the document to which the element belongs.

  ----------------------------------------------------------------------*/
extern void         TtaSplitText (Element element, int position, Document document);

/*----------------------------------------------------------------------
   TtaMergeText

   Merges two text elements.

   Parameters:
   element: the first text element. Merging occurs only if
   the next sibling is a text element with the same attributes.
   document: the document to which the text element belongs.

   Return value:
   TRUE if merging has been done.

  ----------------------------------------------------------------------*/
extern boolean      TtaMergeText (Element element, Document document);

/*----------------------------------------------------------------------
   TtaSetGraphicsShape

   Changes the shape of a Graphics or Symbol basic element.

   Parameters:
   element: the element to be changed. This element must
   be a basic element of type Graphics or Symbol.
   shape: new shape for that element.
   document: the document containing that element.

  ----------------------------------------------------------------------*/
extern void         TtaSetGraphicsShape (Element element, char shape, Document document);

/*----------------------------------------------------------------------
   TtaAddPointInPolyline

   Adds a new point in a Polyline basic element.

   Parameters:
   element: the element to be changed. This element must
   be a basic element of type Polyline.
   document: the document containing the polyline element.
   rank: rank of the new point to be inserted. If rank is greater
   than the actual number of points, the new point is appended.
   rank must be strictly positive.
   x, y: coordinate of the new point, in millipoints, relatively to
   the upper left corner of the box. x and y
   must be positive or null.
   unit: UnPixel or UnPoint.

  ----------------------------------------------------------------------*/
extern void         TtaAddPointInPolyline (Element element, int rank, TypeUnit unit, int x, int y, Document document);

/*----------------------------------------------------------------------
   TtaDeletePointInPolyline

   Deletes a point in a Polyline basic element.

   Parameters:
   element: the element to be changed. This element must
   be a basic element of type Polyline.
   rank: rank of the point to be deleted. If rank is greater
   than the actual number of points, the last point is deleted.
   rank must be strictly positive.
   document: the document containing the polyline element.

  ----------------------------------------------------------------------*/
extern void         TtaDeletePointInPolyline (Element element, int rank, Document document);

/*----------------------------------------------------------------------
   TtaModifyPointInPolyline

   Changes the coordinates of a point in a Polyline basic element.

   Parameters:
   element: the element to be changed. This element must
   be a basic element of type Polyline.
   rank: rank of the point to be modified. If rank is greater
   than the actual number of points, the last point is changed.
   rank must be strictly positive.
   x, y: new coordinates of the point, in millipoints, relatively to
   the upper left corner of the enclosing rectangle. x and y
   must be positive or null.
   document: the document containing the polyline element.
   unit: UnPixel or UnPoint.

  ----------------------------------------------------------------------*/
extern void         TtaModifyPointInPolyline (Element element, int rank, TypeUnit unit, int x, int y, Document document);

/*----------------------------------------------------------------------
   TtaChangeLimitOfPolyline

   Changes the coordinates of the lower left corner of the box containing
   a Polyline basic element.

   Parameters:
   element: the element to be changed. This element must
   be a basic element of type Polyline.
   unit: UnPixel or UnPoint.
   x, y: new coordinates of the lower left corner, in millipoints,
   relatively to the upper left corner of the box. x and y
   must be positive or null and the box must contain all
   points of the polyline.
   document: the document containing the polyline element.

  ----------------------------------------------------------------------*/
extern void         TtaChangeLimitOfPolyline (Element element, TypeUnit unit, int x, int y, Document document);

/*----------------------------------------------------------------------
   TtaCopyPage

   Copies the page element source into the page element destination.
   Both page elements must be in an abstract tree.

   Parameters:
   destination: identifier of the page element to be modified.
   source : identifier of the source page element.

  ----------------------------------------------------------------------*/
extern void         TtaCopyPage (Element destination, Element source);

/*----------------------------------------------------------------------
   TtaGetPictureType

   Returns the type of Picture element.

   Parameter:
   element: the element of interest. This element must be a Picture.

   Return value:
   PicType: type of the element.
  ----------------------------------------------------------------------*/
extern PicType      TtaGetPictureType (Element element);

/*----------------------------------------------------------------------
   TtaGetVolume

   Returns the current volume of the current element.

   Parameter:
   element: the element of interest.

   Return value:
   volume contained in the element.

  ----------------------------------------------------------------------*/
extern int          TtaGetVolume (Element element);

/*----------------------------------------------------------------------
   TtaGetTextLength

   Returns the length of a Text basic element.

   Parameter:
   element: the element of interest. This element must be a basic
   element of type Text.

   Return value:
   textLength (number of characters) of the character string
   contained in the element.

  ----------------------------------------------------------------------*/
extern int          TtaGetTextLength (Element element);

/*----------------------------------------------------------------------
   TtaGiveTextContent

   Returns the content of a Text basic element.

   Parameters:
   element: the element of interest. This element must be a basic
   element of type Text.
   buffer: the buffer that will contain the text.
   length: maximum length of that buffer.

   Return parameters:
   buffer: (the buffer contains the text).
   length: actual length of the text in the buffer.
   language: language of the text.

  ----------------------------------------------------------------------*/
extern void         TtaGiveTextContent (Element element, /*OUT*/ char *buffer, /*INOUT*/ int *length, /*OUT*/ Language *language);

/*----------------------------------------------------------------------
   TtaGiveSubString

   Returns a substring from a Text basic element.

   Parameters:
   element: the element of interest. This element must be a basic
   element of type Text.
   buffer: the buffer that will contain the substring. This buffer
   must be at least of size length.
   position: the rank of the first character of the substring.
   rank must be strictly positive.
   length: the length of the substring. Must be strictly positive.

   Return parameter:
   buffer: (the buffer contains the substring).

  ----------------------------------------------------------------------*/
extern void         TtaGiveSubString (Element element, /*OUT*/ char *buffer, int position, int length);

/*----------------------------------------------------------------------
   TtaGetGraphicsShape

   Returns the content of a Graphics or Symbol basic element.

   Parameter:
   element: the element of interest. This element must be a basic
   element of type Graphics or Symbol.

   Return value:
   a single character representing the shape of the graphics element or
   symbol contained in the element.

  ----------------------------------------------------------------------*/
extern char         TtaGetGraphicsShape (Element element);

/*----------------------------------------------------------------------
   TtaGetPolylineLength

   Returns the number of points in a Polyline basic element.

   Parameter:
   element: the Polyline element. This element must
   be a basic element of type Polyline.

  ----------------------------------------------------------------------*/
extern int          TtaGetPolylineLength (Element element);

/*----------------------------------------------------------------------
   TtaGivePolylinePoint

   Returns the coordinates of a point in a Polyline basic element.

   Parameters:
   element: the Polyline element. This element must
   be a basic element of type Polyline.
   rank: rank of the point in the PloyLine. If rank is greater
   than the actual number of points, an error is raised.
   rank must be strictly positive.
   unit: UnPixel or UnPoint.

   Return values:
   x, y: coordinates of the point, in unit, relatively to
   the upper left corner of the enclosing rectangle.

  ----------------------------------------------------------------------*/
extern void         TtaGivePolylinePoint (Element element, int rank, TypeUnit unit, /*OUT*/ int *x, /*OUT*/ int *y);

/*----------------------------------------------------------------------
   TtaGetPageNumber

   Returns the page number of a Page basic element.

   Parameter:
   pageElement: the page element.

   Return value:
   page number of that page element.

  ----------------------------------------------------------------------*/
extern int          TtaGetPageNumber (Element pageElement);

/*----------------------------------------------------------------------
   TtaGetPageView

   Returns the view corresponding to a Page basic element.

   Parameter:
   pageElement: the page element.

   Return value:
   view of that page.

  ----------------------------------------------------------------------*/
extern int          TtaGetPageView (Element pageElement);

#else  /* __STDC__ */

extern void         TtaSetTextContent ( /* Element element, char *content, Language language, Document document */ );
extern void         TtaAppendTextContent ( /* Element element, char *content, Document document */ );
extern void         TtaInsertTextContent ( /* Element element, int position, char *content, Document document */ );
extern void         TtaDeleteTextContent ( /* Element element, int position, int length, Document document */ );
extern void         TtaSplitText ( /* Element element, int position, Document document */ );
extern boolean      TtaMergeText ( /* Element element, Document document */ );
extern void         TtaSetGraphicsShape ( /* Element element, char shape, Document document */ );
extern void         TtaAddPointInPolyline ( /* Element element, int rank, TypeUnit unit, int x, int y, Document document */ );
extern void         TtaDeletePointInPolyline ( /* Element element, int rank, Document document */ );
extern void         TtaModifyPointInPolyline ( /* Element element, int rank, TypeUnit unit, int x, int y, Document document */ );
extern void         TtaChangeLimitOfPolyline ( /* Element element, TypeUnit unit, int x, int y, Document document */ );
extern void         TtaCopyPage ( /* Element destination, Element source */ );
extern PicType      TtaGetPictureType (/* Element element */);
extern int          TtaGetVolume (/* Element element */);
extern int          TtaGetTextLength ( /* Element element */ );
extern void         TtaGiveTextContent ( /* Element element, char *buffer, int *length, Language *language */ );
extern void         TtaGiveSubString ( /* Element element, char *buffer, int position, int length */ );
extern char         TtaGetGraphicsShape ( /* Element element */ );
extern int          TtaGetPolylineLength ( /* Element element */ );
extern void         TtaGivePolylinePoint ( /* Element element, int rank, TypeUnit unit, int *x, int *y */ );
extern int          TtaGetPageNumber ( /* Element pageElement */ );
extern int          TtaGetPageView ( /* Element pageElement */ );

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */

#endif
