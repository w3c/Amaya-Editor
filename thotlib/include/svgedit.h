#ifdef _WX

#ifndef __SVGEDIT_H__
#define __SVGEDIT_H__

extern Element GetGraphicsUnit(Element element);

extern void GetArrowCoord(int *x1, int *y1, int *x2, int *y2);
extern ThotBool AskSurroundingBox(
                                  Document doc,
                                  Element svgAncestor,
                                  Element svgCanvas,
                                  int shape,
                                  int *x1, int *y1,
                                  int *x2, int *y2,
                                  int *x3, int *y3,
                                  int *x4, int *y4,
                                  int *lx, int *ly);

extern ThotBool AskShapePoints (Document doc,
                                Element svgAncestor,
                                Element svgCanvas,
                                int shape, Element el);

extern ThotBool AskTransform(Document doc,
                             Element svgAncestor,
                             Element svgCanvas,
                             int transform_type,
                             Element el
                             );

extern void TtaCoordinatesInParentSpace(Element el, float *x, float *y);

extern ThotBool GetAncestorCanvasAndObject(Document doc, Element *el,
                                           Element *svgAncestor,
                                           Element *svgCanvas);
extern void UpdateTransformMatrix(Document doc, Element el);
extern void UpdatePointsOrPathAttribute(Document doc, Element el,
					int w, int h, ThotBool withUndo);
extern void UpdateShapeElement(Document doc, Element el,
                               char shape,
                               int x, int y, int width, int height,
                               int rx, int ry);

extern void TtaUpdateMarkers (Element el, Document doc,
                              ThotBool clear, ThotBool rebuild);
/* function GenerateMarkers is defined in the SVG parser (module SVGbuilder.c) */
extern void GenerateMarkers (Element pEl, Document doc, Element marker,
                             int position);
#endif /* #ifndef __SVGEDIT_H__ */

#endif /* _WX */
