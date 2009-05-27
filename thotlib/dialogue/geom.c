/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2009
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 
/*
 * geom.c : boxes geometry handling, i.e. interracting with the user
 *          concerning a box position, or size.
 *
 * Authors: I. Vatton (INRIA)
 *          R. Guetari (W3C/INRIA) - Unicode, Windows version and Plug-ins
 *          F. Wang - SVG editing
 *
 */
#ifdef _WX
#include "wx/wx.h"
#endif /* _WX */

#include "thot_gui.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "frame.h"
#include "libmsg.h"
#include "message.h"

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "boxes_tv.h"
#include "frame_tv.h"

#include "frame_f.h"
#include "presentationapi_f.h"
#include "structcreation_f.h"
#include "boxlocate_f.h"
#include "geom_f.h"

static int          GridSize = 1;
#define DO_ALIGN(val) ((val + (GridSize/2)) / GridSize) * GridSize
static PtrBox       BoxCanvas = NULL;

#include "appli_f.h"
#include "buildboxes_f.h"
#include "content_f.h"
#include "font_f.h"
#include "memory_f.h"
#include "units_f.h"
#include "windowdisplay_f.h"
#ifdef _GL
#if defined (_MACOS) && defined (_WX)
#include <gl.h>
#else /* _MACOS */
#include <GL/gl.h>
#endif /* _MACOS */
#include "glwindowdisplay.h"
#endif /*_GL*/
#ifdef _WX
#include "logdebug.h"
#include "message_wx.h"
#include "AmayaFrame.h"
#include "AmayaAddPointEvtHandler.h"
#include "AmayaMovePointEvtHandler.h"
#include "AmayaMovingBoxEvtHandler.h"
#include "AmayaResizingBoxEvtHandler.h"
#include "AmayaCreateShapeEvtHandler.h"
#include "AmayaCreatePathEvtHandler.h"
#include "AmayaTransformEvtHandler.h"
#include "AmayaEditPathEvtHandler.h"
#include "AmayaEditShapeEvtHandler.h"
#endif /* _WX */

/*----------------------------------------------------------------------
  TtaGridDoAlign export the macro DO_ALIGN used to align points on a grid
  ----------------------------------------------------------------------*/
int TtaGridDoAlign(int value)
{
  return DO_ALIGN(value);
}

/*----------------------------------------------------------------------
  VideoInvert switch to inverse video the area of frame located at
  (x,y) and of size width x height.
  ----------------------------------------------------------------------*/
static void VideoInvert (int frame, int width, int height, int x, int y)
{
  ThotWindow          w;
  w = (ThotWindow)FrameTable[frame].WdFrame;
  if (w != None)
    {
      GL_DrawEmptyRectangle (152, x, y, width, height, 0);
    }
}

/*----------------------------------------------------------------------
  BoxGeometry set the geometry of the fake box used to interract with
  the user.
  ----------------------------------------------------------------------*/
void BoxGeometry (int frame, int x, int y, int width, int height,
                  int xr, int yr)
{
  if (width > 0)
    /*upper border */
    VideoInvert (frame, width, 1, x, y);
  if (height > 1)
    {
      /* left border */
      VideoInvert (frame, 1, height - 1, x, y + 1);
      /* right border */
      VideoInvert (frame, 1, height - 1, x + width - 1, y + 1);
    }
  if (width > 1)
    /* bottom */
    VideoInvert (frame, width - 1, 1, x + 1, y + height - 1);
}

/*----------------------------------------------------------------------
  InvertEllipse set the geometry of the fake box used to interract with
  the user.
  ----------------------------------------------------------------------*/
void InvertEllipse (int frame, int x, int y, int width, int height,
                    int xr, int yr)
{
  ThotWindow      w;

  w = (ThotWindow)FrameTable[frame].WdFrame;
  if (w != None)
	  GL_DrawArc (x, y, width, height, 0, 360 * 64, 0, FALSE);
}


/*----------------------------------------------------------------------
  Resizing
  This function returns the new dimensions of the box.
  ----------------------------------------------------------------------*/
static void Resizing ( int frame, int x, int y, int *width, int *height,
                       PtrBox box, int xmin, int xmax, int ymin, int ymax,
                       int xm, int ym, int percentW, int percentH )
{
#define C_TOP 0
#define C_HCENTER 1
#define C_BOTTOM 2
#define C_LEFT 0
#define C_VCENTER 1
#define C_RIGHT 2

  TTALOGDEBUG_7( TTA_LOG_SVGEDIT, _T("Resizing frame=%d xmin=%d xmax=%d ymin=%d ymax=%d xm=%d ym=%d"),
                 frame, xmin, xmax, ymin, ymax, xm, ym);

  AmayaFrame * p_frame = FrameTable[frame].WdFrame;
  AmayaResizingBoxEvtHandler * p_resizingBoxEvtHandler =
    new AmayaResizingBoxEvtHandler( p_frame,
                                    x, y, width, height,
                                    box, xmin, xmax, ymin, ymax,
                                    xm, ym, percentW, percentH );
  /* now wait for the polygon creation end */
  ThotEvent ev;
  while (!p_resizingBoxEvtHandler->IsFinish())
    TtaHandleOneEvent (&ev);
  delete p_resizingBoxEvtHandler;
}


/*----------------------------------------------------------------------
  GeometryResize draws a box at a specific origin location (x,y) in
  frame and of initial size (width, height) when interracting with
  the user to mofify the box geometry (button press).
  xmin, xmax, ymin, ymax are the maximum values allowed.
  xm and ym gives the initial mouse coordinates in the frame.
  Parameters percentW and percentH will be equal to zero if width
  and height are independent or will get the percent report (percentH = 100
  if height is equal to width).
  This function returns the new dimensions of the box.
  ----------------------------------------------------------------------*/
void GeometryResize (int frame, int x, int y, int *width, int *height,
                     PtrBox box, int xmin, int xmax, int ymin, int ymax,
                     int xm, int ym, int percentW, int percentH)
{
  Resizing (frame, x, y, width, height, box, xmin, xmax, ymin, ymax, xm, ym,
            percentW, percentH);
}


/*----------------------------------------------------------------------
  Moving
  This function returns the new position (x, y) of the reference point
  of the box.
  ----------------------------------------------------------------------*/
static void Moving (int frame, int *x, int *y, int width, int height,
                    PtrBox box, int xmin, int xmax, int ymin, int ymax,
                    int xm, int ym)
{ 
  TTALOGDEBUG_10( TTA_LOG_SVGEDIT, _T("Moving frame=%d width=%d height=%d box=%d xmin=%d xmax=%d ymin=%d ymax=%d xm=%d ym=%d"), frame, width, height, box, xmin, xmax, ymin, ymax, xm, ym);

  AmayaFrame * p_frame = FrameTable[frame].WdFrame;
  AmayaMovingBoxEvtHandler * p_movingBoxEvtHandler =
    new AmayaMovingBoxEvtHandler( p_frame,
                                  x, y, width, height,
                                  box, xmin, xmax, ymin, ymax,
                                  xm, ym, 0, 0);
  /* now wait for the polygon creation end */
  ThotEvent ev;
  while(!p_movingBoxEvtHandler->IsFinish())
    TtaHandleOneEvent (&ev);

  delete p_movingBoxEvtHandler;
}

/*----------------------------------------------------------------------
  GeometryMove draws a box at a specific origin location (x,y) in
  frame and of size (width, height) when interracting with
  the user to mofify the box position (button press).
  xmin, xmax, ymin, ymax are the maximum values allowed.
  xm and ym gives the initial mouse coordinates in the frame.
  This function returns the new position (x, y) of the origin.
  ----------------------------------------------------------------------*/
void GeometryMove (int frame, int *x, int *y, int width, int height,
                   PtrBox box, int xmin, int xmax, int ymin, int ymax,
                   int xm, int ym)
{
  Moving (frame, x, y, width, height, box, xmin, xmax, ymin, ymax, xm, ym);
}

/*----------------------------------------------------------------------
  GeometryCreate draws a box at a specific origin location (x, y) in
  frame and of the initial size (width, height) when interracting with
  the user to mofify a box size (button press).
  xmin, xmax, ymin, ymax are the maximum values allowed.
  PosX, PosY, DimX, DimY will be different to zero if these positions or
  dimensions are user specified.
  Parameters percentW and percentH will be equal to zero if width
  and height are independent or will get the percent report (percentH = 100
  if height is equal to width).
  This function returns new values:
  - x, y the position of the origin.
  - width, height the dimension of the box.
  ----------------------------------------------------------------------*/
void GeometryCreate (int frame, int *x, int *y, int *width, int *height,
                     int xmin, int xmax, int ymin, int ymax, PtrBox box,
                     int PosX, int PosY, int DimX, int DimY, int percentW,
                     int percentH)
{
  ThotWindow          w;
  PtrAbstractBox      pAb;
  int                 xm, ym;
  int                 dx, dy;
  int                 ret;
  int                 xref, yref;
  ThotBool            isEllipse;

  pAb = box->BxAbstractBox;
  if (pAb && pAb->AbLeafType == LtCompound)
    pAb = pAb->AbFirstEnclosed;
  isEllipse = (pAb &&
               pAb->AbLeafType == LtGraphics &&
               (pAb->AbShape == 'a' || pAb->AbShape == 'c'));
  /* use relative coordinates */
  xm = ym = 0;
  /* default position */
  if (*x < xmin)
    *x = xmin;
  else if (*x > xmax)
    *x = xmax;
  if (*y < ymin)
    *y = ymin;
  else if (*y > ymax)
    *y = ymin;
  /* the grid origin is base on the englobing box origin */
  dx = DO_ALIGN (*x - xmin);
  *x = xmin + dx;
  dy = DO_ALIGN (*y - ymin);
  *y = ymin + dy;

  *width = 1;
  *height = 1;
  /* draw the current box geometry */
  switch (box->BxHorizEdge)
    {
    case Right:
      xref = *width;
      break;
    case VertMiddle:
      xref = *width / 2;
      break;
    case VertRef:
      xref = box->BxVertRef;
      *width = xref;
      break;
    default:
      xref = 0;
      break;
    }
  switch (box->BxVertEdge)
    {
    case Bottom:
      yref = *height;
      break;
    case HorizMiddle:
      yref = *height / 2;
      break;
    case HorizRef:
      yref = box->BxHorizRef;
      *height = yref;
      break;
    default:
      yref = 0;
      break;
    }
  /* change the cursor, modify library state */
  w = (ThotWindow)FrameTable[frame].WdFrame;

  if (isEllipse)
    InvertEllipse (frame, *x, *y, *width, *height, *x + xref, *y + yref);
  else
    BoxGeometry (frame, *x, *y, *width, *height, *x + xref, *y + yref);
  /* Loop on user input to keep the first point */
  ret = 0;

  /* switch off the old box geometry */
  if (isEllipse)
    InvertEllipse (frame, *x, *y, *width, *height, *x + xref, *y + yref);
  else
    BoxGeometry (frame, *x, *y, *width, *height, *x + xref, *y + yref);

  Resizing (frame, *x, *y, width, height, box, xmin, xmax, ymin, ymax, xm, ym, percentW, percentH);
}

/*----------------------------------------------------------------------
  GetArrowCoord

  Return the coordinates of the two points that allow to draw an arrow,
  from the coordinates of the two extremities:

  (x2,y2)                   (x1,y1) ---/              
  /                                /|       
  /               ===>             / |
  /                                /  (x2,y2)    
  /                                /       
  (x1,y1)

  ----------------------------------------------------------------------*/
void GetArrowCoord(int *x1, int *y1, int *x2, int *y2)
{
  int dx, dy, x0, y0;
#define size 7
#define lambda 5

  dx = *x2 - *x1;
  dy = *y2 - *y1;
  x0 = *x2;
  y0 = *y2;

  if (dx == 0 && dy == 0)
    return;

  if (abs(lambda*dy) < abs(dx))
    {
      /* Almost horizontal arrow */
      *x1 = x0 + size * (dx < 0 ? 1 : -1);
      *x2 = *x1;
      
      *y1 = y0 - size;
      *y2 = y0 + size;
      return;
    }

  if (abs(lambda*dx) < abs(dy))
    {
      /* Almost vertical arrow */
      *y1 = y0 + size * (dy < 0 ? 1 : -1);
      *y2 = *y1;

      *x1 = x0 - size;
      *x2 = x0 + size;
      return;
    }

  if (dx < 0)
    {
      *x1 = x0;
      *x2 = x0 + size;
      *y2 = y0;
      *y1 = y0 + size * (dy < 0 ? 1 : -1);
    }
  else
    {
      *x1 = x0;
      *x2 = x0 - size;
      *y2 = y0;
      *y1 = y0 + size * (dy < 0 ? 1 : -1);
    }
}

/*----------------------------------------------------------------------
  ShapeCreation

  Call the handler that allows the user to draw a shape in an SVG canvas.
  ancestorX and ancestorY are the absolute position of the SVG ancestor,
  transform is a the matrix that allows to get coordinates of a point in
  the Canvas from its coordinates in the Ancestor.
  Parameters lx and ly give the initial expected width height and are used
  to calculate the width/height ratio.

  --------------------
  |  |------|        |
  |  |Canvas| Ancestor
  |  |------|        |
  --------------------

  The returned value are the one of the surrounding rectangle
  1--lx---2
  |       |
  ly Shape |
  |       |
  3-------4
  ----------------------------------------------------------------------*/
ThotBool ShapeCreation (int frame, Document doc,  void *inverseCTM,
                        PtrBox svgBox, int ancestorX, int ancestorY,
                        int shape,
                        int *x1, int *y1, int *x2, int *y2,
                        int *x3, int *y3, int *x4, int *y4,
                        int *lx, int *ly)
{
  AmayaFrame                 *p_frame;
  AmayaCreateShapeEvtHandler *p_CreateShapeEvtHandler;
  float                       ratio;
  int                         canvasWidth, canvasHeight;
  ThotEvent                   ev;
  ThotBool                    created = FALSE;

  // register the current svg canvas
  BoxCanvas = NULL;
  SVGCreating = TRUE;

  /* Create the handler */
  canvasWidth = svgBox->BxW;
  canvasHeight = svgBox->BxH;
  if (svgBox)
    {
      // MBP apply to the leaf box and not control points
      ancestorX += svgBox->BxLMargin + svgBox->BxLBorder + svgBox->BxLPadding;
      ancestorY += svgBox->BxTMargin + svgBox->BxTBorder + svgBox->BxTPadding;
    }
  p_frame = FrameTable[frame].WdFrame;
  // 
  if (*lx > 0 && *ly > 0)
    ratio = (float)(*ly) / (float)(*lx);
  else
    ratio = 1.;
  p_CreateShapeEvtHandler = new AmayaCreateShapeEvtHandler( p_frame,
                                                            doc,
                                                            inverseCTM,
                                                            ancestorX,
                                                            ancestorY,
                                                            canvasWidth,
                                                            canvasHeight,
                                                            shape,
                                                            ratio,
                                                            x1, y1,
                                                            x4, y4,
                                                            &created
                                                            );

  /* Wait until the end of the interactive contruction */
  while(!p_CreateShapeEvtHandler->IsFinish())
    TtaHandleOneEvent (&ev);
  
  SVGCreating = FALSE;
  delete p_CreateShapeEvtHandler;
  if (!created)
    {
      BoxCanvas = NULL;
      return FALSE;
    }

  /* Compute the position of point (2) and (3) */
  *x2 = *x4;
  *y2 = *y1;
  *x3 = *x1;
  *y3 = *y4;
  /* Convert the coordinates x1,y1 x2,y2 x3,y3 x4,y4 */
  MouseCoordinatesToSVG(doc, p_frame, ancestorX, ancestorY, canvasWidth, canvasHeight,
                        inverseCTM, TRUE, NULL, x1, y1, FALSE);
  MouseCoordinatesToSVG(doc, p_frame, ancestorX, ancestorY, canvasWidth, canvasHeight,
                        inverseCTM, TRUE, NULL, x2, y2, FALSE);
  MouseCoordinatesToSVG(doc, p_frame, ancestorX, ancestorY, canvasWidth, canvasHeight,
                        inverseCTM, TRUE, NULL, x3, y3, FALSE);
  MouseCoordinatesToSVG(doc, p_frame, ancestorX, ancestorY, canvasWidth, canvasHeight,
                        inverseCTM, TRUE, NULL, x4, y4, FALSE);
  MouseCoordinatesToSVG(doc, p_frame, ancestorX, ancestorY, canvasWidth, canvasHeight,
                        inverseCTM, TRUE, NULL, lx, ly, FALSE);

  /* Size of the construct */
  *lx = abs(*x4 - *x1);
  *ly = abs(*y4 - *y1);
  if (*lx == 0 && *ly == 0)
    {
      *lx = 20;
      *ly = (int)(20 * ratio);
    }
  else if (*lx == 0)
    *lx = (int)(*ly / ratio);
  else if (*ly == 0)
    *ly = (int)(20 * ratio);

  /* Some shapes have specific contrainsts.  */
  if (shape == 9 || shape == 10)
    {
      /* text, foreign object (one point is expected) */
      created = TRUE;
    }
  else if(shape == 42 || shape == -1 || shape == -2)
    {
      /* selection, svg, template (a bounding box is expected) */
      if(*x4 < *x1)
        {
          *x1 = *x4;
          *x4 = *x1 + *lx;
        }

      if(*y4 < *y1)
        {
          *y1 = *y4;
          *y4 = *y1 + *ly;
        }

      created = TRUE;
    }
  else if (shape != 0 && (shape < 12 || shape > 14))
    {
      /* Shape */
      if (shape == 20)
        /* equilateral triangle
	        
          /\       ^
         /  \      |  ly        (ly)^2 + (lx/2)^2 = (lx)^2
        /    \     |         => ly = srqt(3)*lx/2
       .------.    v

        <------>
        lx      */
        *lx = (int) (floor(2 *  *ly / sqrt((float)3)));
	
      else if (shape == 3 || shape == 15 || shape == 16 || shape == 23)
        {
          /* *lx and *ly must be equal (square, circle...) */
          if (*ly < *lx)*lx=*ly; else *ly = *lx;
        }

      /* Invert the coordinates of (1) and (4) if necessary */
      if (*x4 < *x1)*x4 = *x1 - *lx;
      else *x4 = *x1 + *lx;

      if (*y4 < *y1)*y4 = *y1 - *ly;
      else *y4 = *y1 + *ly;

      /* Check that the shape is not too small */
#define MINSIZE 5
      created = (*lx >= MINSIZE && *ly >= MINSIZE);
    }
  else
    /* Line, arrows etc. Check that it is not reduced to one point */
    created = (*lx > 0 || *ly > 0);

  if (!created)
    {
      BoxCanvas = NULL;
      return FALSE;
    }
  TtaSetStatus (doc, 1, "", NULL);
  BoxCanvas = NULL;
  return TRUE;
}

/*----------------------------------------------------------------------
  TransformSVG
  ----------------------------------------------------------------------*/
ThotBool TransformSVG (int frame, Document doc, 
                       void *CTM, void *inverse, PtrBox svgBox,
                       int ancestorX, int ancestorY,
                       int transform_type, Element el)
{
  AmayaFrame               *p_frame;
  AmayaTransformEvtHandler *p_TransformEvtHandler;
  int                       canvasWidth, canvasHeight;
  ThotEvent                 ev;
  ThotBool                  transformApplied;

  // register the current svg canvas
  BoxCanvas = svgBox;

  canvasWidth = svgBox->BxW;
  canvasHeight = svgBox->BxH;
  p_frame = FrameTable[frame].WdFrame;
  p_TransformEvtHandler = new AmayaTransformEvtHandler(p_frame,
                                                       doc,
                                                       CTM,
                                                       inverse,
                                                       ancestorX,
                                                       ancestorY,
                                                       canvasWidth,
                                                       canvasHeight,
                                                       transform_type,
                                                       el,
                                                       &transformApplied);

  while(!p_TransformEvtHandler->IsFinish())
    TtaHandleOneEvent (&ev);
  
  delete p_TransformEvtHandler;
  BoxCanvas = NULL;
  return transformApplied;
}

/*----------------------------------------------------------------------
  ShapeEdit
  ----------------------------------------------------------------------*/
ThotBool ShapeEdit (int frame, Document doc,  void *inverse, PtrBox svgBox,
                    int ancestorX, int ancestorY,
                    Element el, int point_edited)
{
  AmayaFrame               *p_frame;
  AmayaEditShapeEvtHandler *p_EditShapeEvtHandler;
  int                       canvasWidth, canvasHeight;
  ThotEvent                 ev;
  ThotBool                  hasBeenEdited;

  // register the current svg canvas
  BoxCanvas = svgBox;

  canvasWidth = svgBox->BxW;
  canvasHeight = svgBox->BxH;
  p_frame = FrameTable[frame].WdFrame;
  p_EditShapeEvtHandler = new AmayaEditShapeEvtHandler(p_frame,
                                                       doc,
                                                       inverse,
                                                       ancestorX,
                                                       ancestorY,
                                                       canvasWidth,
                                                       canvasHeight,
                                                       el,
                                                       point_edited,
                                                       &hasBeenEdited);
  
  while(!p_EditShapeEvtHandler->IsFinish())
    TtaHandleOneEvent (&ev);
  
  delete p_EditShapeEvtHandler;
  BoxCanvas = NULL;
  return hasBeenEdited;
}

/*----------------------------------------------------------------------
  PathEdit
  ----------------------------------------------------------------------*/
ThotBool PathEdit (int frame, Document doc,  void *inverse, PtrBox svgBox,
                   int ancestorX, int ancestorY,
                   Element el, int point_edited)
{
  AmayaFrame              *p_frame;
  AmayaEditPathEvtHandler *p_EditPathEvtHandler;
  int                      canvasWidth, canvasHeight;
  ThotEvent                ev;
  ThotBool                 transformApplied;

  // register the current svg canvas
  BoxCanvas = NULL;

  canvasWidth = svgBox->BxW;
  canvasHeight = svgBox->BxH;
  if (svgBox)
    {
      // MBP apply to the leaf box and not control points
      ancestorX += svgBox->BxLMargin + svgBox->BxLBorder + svgBox->BxLPadding;
      ancestorY +=  svgBox->BxTMargin + svgBox->BxTBorder + svgBox->BxTPadding;
    }
  p_frame = FrameTable[frame].WdFrame;
  p_EditPathEvtHandler = new AmayaEditPathEvtHandler(p_frame, doc, inverse,
                                                     ancestorX, ancestorY,
                                                     canvasWidth, canvasHeight,
                                                     el, point_edited,
                                                     &transformApplied);
  
  while(!p_EditPathEvtHandler->IsFinish())
    TtaHandleOneEvent (&ev);
  
  delete p_EditPathEvtHandler;
  BoxCanvas = NULL;
  return transformApplied;
}

/*----------------------------------------------------------------------
  PathCreation
  ----------------------------------------------------------------------*/
ThotBool PathCreation (int frame, Document doc,  void *inverseCTM,
                       PtrBox svgBox, int ancestorX, int ancestorY,
                       int shape, Element el)
{
  AmayaFrame                *p_frame;
  AmayaCreatePathEvtHandler *p_CreatePathEvtHandler;
  int                        canvasWidth, canvasHeight;
  ThotEvent                  ev;
  ThotBool                   created;

  // register the current svg canvas
  BoxCanvas = NULL;
  SVGCreating = TRUE;

  canvasWidth = svgBox->BxW;
  canvasHeight = svgBox->BxH;
  if (svgBox)
    {
      // MBP apply to the leaf box and not control points
      ancestorX += svgBox->BxLMargin + svgBox->BxLBorder + svgBox->BxLPadding;
      ancestorY += svgBox->BxTMargin + svgBox->BxTBorder + svgBox->BxTPadding;
    }
  p_frame = FrameTable[frame].WdFrame;
  p_CreatePathEvtHandler = new AmayaCreatePathEvtHandler(p_frame, doc, inverseCTM,
                                                          ancestorX, ancestorY,
                                                         canvasWidth, canvasHeight,
                                                         shape, el, &created);
  while(!p_CreatePathEvtHandler->IsFinish())
    TtaHandleOneEvent (&ev);
  
  SVGCreating = FALSE;
  delete p_CreatePathEvtHandler;
  BoxCanvas = NULL;
  return created;
}


/*----------------------------------------------------------------------
  MouseCoordinatesToSVG
  Convert the mouse coordinates (x,y) into the one in the SVG canvas and
  display them into the status bar. If convert is TRUE, then x and y are
  modified.
  Return TRUE if the mouse is inside the SVG Canvas 
  ----------------------------------------------------------------------*/
ThotBool MouseCoordinatesToSVG (Document doc, AmayaFrame *p_frame,
                                int x0, int y0, int width, int height,
                                void *inverseCTM, ThotBool convert,
                                char *msg, int *x, int *y,
                                ThotBool displayCoordinates)
{
  int      frame;
  int      newx, newy;
  int      newx2, newy2;
  char     buffer[200];
  float    a, b, c, d, e, f;
  ThotBool inside = TRUE;

  if (inverseCTM)
    {
      /* Get the coefficients of the Matrix */
      a = ((PtrTransform)(inverseCTM))->AMatrix;
      b = ((PtrTransform)(inverseCTM))->BMatrix;
      c = ((PtrTransform)(inverseCTM))->CMatrix;
      d = ((PtrTransform)(inverseCTM))->DMatrix;
      e = ((PtrTransform)(inverseCTM))->EMatrix;
      f = ((PtrTransform)(inverseCTM))->FMatrix;
    }
  else
    {
      /* By default, the transfom matrix is the identity */
      a = 1; b = 0; c = 0; d = 1; e = 0; f = 0;
    }

  if (BoxCanvas)
    {
      // take into account MBP of the canvas
      x0 = x0 + BoxCanvas->BxLMargin + BoxCanvas->BxLBorder + BoxCanvas->BxLPadding;
      y0 = y0 + BoxCanvas->BxTMargin + BoxCanvas->BxTBorder + BoxCanvas->BxTPadding;
    }
  frame = p_frame->GetFrameId();
  width = LogicalValue (width, UnPixel, NULL,
                        ViewFrameTable[frame - 1].FrMagnification);
  height = LogicalValue (height, UnPixel, NULL,
                         ViewFrameTable[frame - 1].FrMagnification);
  /* Mouse coordinates to SVG ancestor coordinates */
  newx = LogicalValue (*x - x0, UnPixel, NULL,
                       ViewFrameTable[frame - 1].FrMagnification);
  newy = LogicalValue (*y - y0, UnPixel, NULL,
                       ViewFrameTable[frame - 1].FrMagnification);
  /* SVG ancestor coordinates to SVG canvas */
  newx2 = (int)(a * newx + c * newy + e);
  newy2 = (int)(b * newx + d * newy + f);
  /* Modify x and y if asked */
  if (convert)
    {
      *x = newx2;
      *y = newy2;
    }

  /* Check whether the point is inside the Canvas */
  if (newx2 < 0)
    {
      newx2 = 0;
      inside = FALSE;
    }
  else if (newx2 > width)
    {
      newx2 = width;
      inside = FALSE;
    }
  if (newy2 < 0)
    {
      newy2 = 0;
      inside = FALSE;
    }
  else if (newy2 > height)
    {
      newy2 = height;
      inside = FALSE;
    }

  /* Display the coordinates in the status bar */
  if (msg)
    {
      if (displayCoordinates)
        {
          sprintf(buffer, "(%d,%d) - ", newx2, newy2);
          strcat(buffer, msg);
        }
      else
        strcpy(buffer, msg);
      TtaSetStatus (doc, 1, buffer, NULL);
    }
  return inside;
}


/*----------------------------------------------------------------------
  MouseCoordinatesToSVG
  ----------------------------------------------------------------------*/
void SVGToMouseCoordinates (Document doc, AmayaFrame *p_frame,
                            int x0, int y0, int width, int height,
                            void *CTM, float x, float y,
                            int *newx, int *newy)
{
  int   frame;
  float a, b, c, d, e, f;

  if (CTM)
    {
      /* Get the coefficients of the Matrix */
      a = ((PtrTransform)(CTM))->AMatrix;
      b = ((PtrTransform)(CTM))->BMatrix;
      c = ((PtrTransform)(CTM))->CMatrix;
      d = ((PtrTransform)(CTM))->DMatrix;
      e = ((PtrTransform)(CTM))->EMatrix;
      f = ((PtrTransform)(CTM))->FMatrix;
    }
  else
    {
      /* By default, the transfom matrix is the identity */
      a = 1; b = 0; c = 0; d = 1; e = 0; f = 0;
    }

  if (BoxCanvas)
    {
      // take into account MBP of the canvas
      x0 = x0 + BoxCanvas->BxLMargin + BoxCanvas->BxLBorder + BoxCanvas->BxLPadding;
      y0 = y0 + BoxCanvas->BxTMargin + BoxCanvas->BxTBorder + BoxCanvas->BxTPadding;
    }

  frame = p_frame->GetFrameId();
  *newx = x0 + PixelValue ( (int)(a * x + c * y + e),
                            UnPixel, NULL,
                            ViewFrameTable[frame - 1].FrMagnification);

  *newy = y0 + PixelValue ( (int)(b * x + d * y + f),
                            UnPixel, NULL,
                            ViewFrameTable[frame - 1].FrMagnification);
}

/*----------------------------------------------------------------------
  ApproximateAngleOfLine
  ----------------------------------------------------------------------*/
void ApproximateAngleOfLine (int T, int x1, int y1, int *x2, int *y2)
{
  float local_x, local_y, r;
  float theta;

  local_x = *x2 - x1;
  local_y = *y2 - y1;
  r = sqrt((float)(local_x*local_x + local_y*local_y));
  if (r == 0)
    return;

  /*
    We work in the system of coordinates of origin (x1,y1)

    |    O (local_x, local_y)
    | r /
    |  /
    | /theta
    |/
    ------
    (0,0)
  */

  /* Get the angle */
  theta = 180*acos(local_x/r)/M_PI;
  if (local_y < 0)theta = -theta;

  /* Approximate the angle */
  
#ifdef _WINDOWS
  theta = floor(theta/T)*T;
#else /* _WINDOWS */
  theta = round(theta/T)*T;
#endif /* _WINDOWS */

  theta = (M_PI*theta)/180;

  /* Update the coordinates */
  *x2 = x1 + (int)(cos(theta)*r);
  *y2 = y1 + (int)(sin(theta)*r);
}
