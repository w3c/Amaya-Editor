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

extern void         TtaSetTextContent (Element element, char *content, Language language, Document document);
extern void         TtaAppendTextContent (Element element, char *content, Document document);
extern void         TtaInsertTextContent (Element element, int position, char *content, Document document);
extern void         TtaDeleteTextContent (Element element, int position, int length, Document document);
extern void         TtaSplitText (Element element, int position, Document document);
extern boolean      TtaMergeText (Element element, Document document);
extern void         TtaSetGraphicsShape (Element element, char shape, Document document);
extern void         TtaAddPointInPolyline (Element element, int rank, TypeUnit unit, int x, int y, Document document);
extern void         TtaDeletePointInPolyline (Element element, int rank, Document document);
extern void         TtaModifyPointInPolyline (Element element, int rank, TypeUnit unit, int x, int y, Document document);
extern void         TtaChangeLimitOfPolyline (Element element, TypeUnit unit, int x, int y, Document document);
extern void         TtaCopyPage (Element destination, Element source);
extern PicType      TtaGetPictureType (Element element);
extern int          TtaGetTextLength (Element element);
extern void         TtaGiveTextContent (Element element, char *buffer, int *length, Language * language);
extern void         TtaGiveSubString (Element element, char *buffer, int position, int length);
extern char         TtaGetGraphicsShape (Element element);
extern int          TtaGetPolylineLength (Element element);
extern void         TtaGivePolylinePoint (Element element, int rank, TypeUnit unit, int *x, int *y);
extern int          TtaGetPageNumber (Element pageElement);
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
