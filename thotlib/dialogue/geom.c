/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 
/*
 * geom.c : boxes geometry handling, i.e. interracting with the user
 *          concerning a box position, or size.
 *
 * Authors: I. Vatton (INRIA)
 *          R. Guetari (W3C/INRIA) - Unicode, Windows version and Plug-ins
 *
 */

#include "thot_gui.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "frame.h"
#include "libmsg.h"
#include "message.h"

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "frame_tv.h"
static int          GridSize = 1;

#define DO_ALIGN(val) ((val + (GridSize/2)) / GridSize) * GridSize

#include "appli_f.h"
#include "buildboxes_f.h"
#include "content_f.h"
#include "font_f.h"
#include "memory_f.h"
#include "units_f.h"
#include "windowdisplay_f.h"

#ifdef _WINDOWS
#include "wininclude.h"
/* BOOL WIN_UserGeometry = FALSE ; */

extern int X_Pos;
extern int Y_Pos;

/*----------------------------------------------------------------------
  DrawOutline :                                                    
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void DrawOutline (HWND hwnd, POINT ptBeg, POINT ptEnd)
#else  /* __STDC__ */
static void DrawOutline (hwnd, ptBeg, ptEnd)
HWND  hwnd; 
POINT ptBeg; 
POINT ptEnd;
#endif /* __STDC__ */
{
     HDC hdc ;
	 POINT ptTab [2];

	 ptTab[0].x = ptBeg.x;
	 ptTab[0].y = ptBeg.y;
	 ptTab[1].x = ptEnd.x;
	 ptTab[1].y = ptEnd.y;

     hdc = GetDC (hwnd) ;

     SetROP2 (hdc, R2_NOT) ;
     SelectObject (hdc, GetStockObject (NULL_BRUSH)) ;
     /* SelectObject (hdc, GetStockObject (BLACK_PEN)) ; */
     Polyline (hdc, ptTab, 2) ;

     DeleteDC (hdc) ;
}

/*----------------------------------------------------------------------
  DrawOutpolygon :                                                    
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void DrawOutpolygon (HWND hwnd, POINT* pt, int nb)
#else  /* __STDC__ */
void DrawOutpolygon (hwnd, pt, nb)
HWND  hwnd; 
POINT pt; 
int   nb;
#endif /* __STDC__ */
{
     HDC hdc ;

     hdc = GetDC (hwnd) ;

     SetROP2 (hdc, R2_NOT) ;
     SelectObject (hdc, GetStockObject (NULL_BRUSH)) ;
     /* SelectObject (hdc, GetStockObject (BLACK_PEN)) ; */
     Polyline (hdc, pt, nb) ;

     DeleteDC (hdc) ;
}
#endif /* _WINDOWS */

/*----------------------------------------------------------------------
 *      BoxGeometry set the geometry of the fake box used to interract
 *		with the user.
 *		Last parameter withborder indicate whether the border should
 *		be drawn or not.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         BoxGeometry (int frame, int x, int y, int width, int height, int xr, int yr, ThotBool withborder)
#else  /* __STDC__ */
static void         BoxGeometry (frame, x, y, width, height, xr, yr, withborder)
int                 frame;
int                 x;
int                 y;
int                 width;
int                 height;
int                 xr;
int                 yr;
ThotBool            withborder;
#endif /* __STDC__ */

{
#define step 6

   if (width > 0)
      /*upper border */
      VideoInvert (frame, width, 1, x, y);
   if (height > 1)
     {
	VideoInvert (frame, 1, height - 1, x, y + 1);	/* left border */
	VideoInvert (frame, 1, height - 1, x + width - 1, y + 1);	/* right border */
     }
   if (width > 1)
      VideoInvert (frame, width - 1, 1, x + 1, y + height - 1);		/* bottom */
   /* reference point */
   VideoInvert (frame, HANDLE_WIDTH, HANDLE_WIDTH, xr, yr);
#ifdef IV
   if (withborder)
     {
	for (i = step; i < width; i += step)
	   VideoInvert (frame, 1, height - 1, x + i, y + 1);
	for (i = step; i < height; i += step)
	   VideoInvert (frame, width - 1, 1, x + 1, y + i);
     }
#endif
}

/*----------------------------------------------------------------------
  RedrawPolyLine shows the current state of the polyline (closed or
  not) in a given frame.
  This function returns the coordinates of the following control points :
  - x1, y1 predecessor of point
  - x2, y2 point
  - x3, y3 successor of point
  - xMin, yMin, xMax, yMax update the current bounding box excluding
  the selected point.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         RedrawPolyLine (int frame, int x, int y, PtrTextBuffer buffer, int nb, int point, ThotBool close, int *x1, int *y1, int *x2, int *y2, int *x3, int *y3, int *xMin, int *yMin, int *xMax, int *yMax)
#else  /* __STDC__ */
static void         RedrawPolyLine (frame, x, y, buffer, nb, point, close, x1, y1, x2, y2, x3, y3)
int                 frame;
int                 x;
int                 y;
PtrTextBuffer       buffer;
int                 nb;
int                 point;
ThotBool            close;
int                *x1;
int                *y1;
int                *x2;
int                *y2;
int                *x3;
int                *y3;
#endif /* __STDC__ */

{
  ThotPoint          *points;
  int                 i, j;
  PtrTextBuffer       adbuff;

  *x1 = *y1 = *x2 = *y2 = *x3 = *y3 = -1;
  /* allocate a table of points */
  points = (ThotPoint *) TtaGetMemory (sizeof (ThotPoint) * nb);
  adbuff = buffer;
  j = 1;
  for (i = 0; i < nb - 1; i++) {
      if (j >= adbuff->BuLength) {
         if (adbuff->BuNext != NULL) {
            /* change the buffer */
            adbuff = adbuff->BuNext;
            j = 0;
		 }
	  }
      points[i].x = x + PointToPixel (adbuff->BuPoints[j].XCoord / 1000);
      points[i].y = y + FrameTable[frame].FrTopMargin + PointToPixel (adbuff->BuPoints[j].YCoord / 1000);
      /* write down predecessor and sucessor of point */
      if (i + 1 == point) {
         /* selected point */
         *x2 = points[i].x;
         *y2 = points[i].y;
	  } else {
             if (points[i].x < *xMin)
                *xMin = points[i].x;
             if (points[i].x > *xMax)
                *xMax = points[i].x;
             if (points[i].y < *yMin)
                *yMin = points[i].y;
             if (points[i].y > *yMax)
                *yMax = points[i].y;
             if (i + 1 == point - 1) {
                /* predecessor */
                *x1 = points[i].x;
                *y1 = points[i].y;
			 } else if (i == point) {
                    /* succesor */
                    *x3 = points[i].x;
                    *y3 = points[i].y;
			 }
	  }
      j++;
  }

  /* Draw the border */
  if (close && nb > 3) {
     /* This is a closed polyline with more than 2 points */
     points[nb - 1].x = points[0].x;
     points[nb - 1].y = points[0].y;
     if (point == 1) {
        *x1 = points[nb - 2].x;
        *y1 = points[nb - 2].y;
	 }
     if (point == nb - 1) {
        *x3 = points[0].x;
        *y3 = points[0].y;
	 }
#    ifdef _WINDOWS
     DrawOutpolygon (FrRef [frame], points, nb);
#    else  /* !_WINDOWS */
     XDrawLines (TtDisplay, FrRef[frame], TtInvertGC, points, nb, CoordModeOrigin);
#    endif /* _WINDOWS */
  } else 
#        ifdef _WINDOWS 
         DrawOutpolygon (FrRef [frame], points, nb - 1);
#        else  /* _WINDOWS */
         XDrawLines (TtDisplay, FrRef[frame], TtInvertGC, points, nb - 1, CoordModeOrigin);
#        endif /* _WINDOWS */
  /* free the table of points */
  free ((STRING) points);
}


/*----------------------------------------------------------------------
  TranslatePoints updates Polyline buffers.
  pBbuff points to the first box buffer.
  ppbuff points to the first abstract box buffer.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         TranslatePoints (int dx, int dy, PtrTextBuffer pBbuff, PtrTextBuffer pPbuff, int nbPoints)
#else  /* __STDC__ */
static void         TranslatePoints (dx, dy, pBbuff, pPbuff, nbPoints)
int                 dx;
int                 dy;
PtrTextBuffer       pBbuff;
PtrTextBuffer       pPbuff;
int                 nbPoints;
#endif /* __STDC__ */
{
  int i, j;

  /* translate points */
  j = 1;

  for (i = 1; i < nbPoints; i++)
    {
      if (j >= pBbuff->BuLength)
	{
	  if (pBbuff->BuNext != NULL)
	    {
	      /* Next buffer */
	      pBbuff = pBbuff->BuNext;
	      pPbuff = pPbuff->BuNext;
	      j = 0;
	    }
	}
      if (dx != 0)
	{
	  pBbuff->BuPoints[j].XCoord = pBbuff->BuPoints[j].XCoord + dx;
	  pPbuff->BuPoints[j].XCoord = pBbuff->BuPoints[j].XCoord;
	}
      if (dy != 0)
	{
	  pBbuff->BuPoints[j].YCoord = pBbuff->BuPoints[j].YCoord + dy;
	  pPbuff->BuPoints[j].YCoord = pBbuff->BuPoints[j].YCoord;
	}
      j++;
    }
}

/*----------------------------------------------------------------------
  SetBoundingBox updates Polyline buffer and origins to fit exactely the
  box given by xMin,yMin xMax,yMax.
  xMin, yMin, xMax and yMax are expressed in Millipoints.
  xOrg and yOrg are expressed in pixels.
  If xMin differs from *xOrg, *xOrg and points are horizontally translated.
  If yMin differs from *yOrg, *yOrg and points are vertically translated.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                SetBoundingBox (int xMin, int xMax, int *xOrg, int yMin, int yMax, int *yOrg, PtrBox pBox, int nbPoints)
#else  /* __STDC__ */
void                SetBoundingBox (xMin, xMax, xOrg, yMin, yMax, yOrg, pBox, nbPoints)
int                 xMin;
int                 xMax;
int                *xOrg;
int                 yMin;
int                 yMax;
int                *yOrg;
PtrBox              pBox;
int                 nbPoints;
#endif /* __STDC__ */
{
  PtrTextBuffer     Pbuffer;
  PtrTextBuffer     Bbuffer;
  C_points         *controls;
  int               dx, dy;
  int               oldx, oldy;
  int               width, height;
  int               i;

  Bbuffer = pBox->BxBuffer;
  Pbuffer = pBox->BxAbstractBox->AbPolyLineBuffer;
  /* convert into Millipoints */
  oldx = PixelToPoint (*xOrg) * 1000;
  oldy = PixelToPoint (*yOrg) * 1000;
  width = xMax - xMin;
  height = yMax - yMin;

  /* update horizontal origin of the box */
  if (xMin != oldx)
    {
      dx = xMin - oldx;
      *xOrg = PointToPixel (xMin / 1000);
    }
  else
    dx = 0;
  
  /* update vertical origin of the box */
  if (yMin != oldy)
    {
      dy = yMin - oldy;
      *yOrg = PointToPixel (yMin / 1000);
    }
  else
    dy = 0;

  while (dx != 0 || dy != 0)
    {
      /* now move each point */
      TranslatePoints (dx, dy, Bbuffer, Pbuffer, nbPoints);

      /* For curves we need to compute control points */
      if (pBox->BxAbstractBox->AbPolyLineShape == 'B' ||
	  pBox->BxAbstractBox->AbPolyLineShape == 'A' ||
	  pBox->BxAbstractBox->AbPolyLineShape == 'F' ||
	  pBox->BxAbstractBox->AbPolyLineShape == 'D' ||
	  pBox->BxAbstractBox->AbPolyLineShape == 's')
	{
	  if (pBox->BxPictInfo != NULL)
	    free ((STRING) pBox->BxPictInfo);
	  controls = ComputeControlPoints (Bbuffer, pBox->BxNChars);
	  pBox->BxPictInfo = (int *) controls;
	  /* save previous xMin and yMin */
	  oldx = xMin;
	  oldy = yMin;
	  for (i = 0; i <  pBox->BxNChars; i++)
	    {
	      if (xMin > controls[i].lx)
		xMin = (int) controls[i].lx;
	      if (width < controls[i].lx)
		width = (int) controls[i].lx;
	      if (xMin > controls[i].rx)
		xMin = (int) controls[i].rx;
	      if (width < controls[i].rx)
		width = (int) controls[i].rx;
	      if (yMin > controls[i].ly)
		yMin = (int) controls[i].ly;
	      if (height < controls[i].ly)
		height = (int) controls[i].ly;
	      if (yMin > controls[i].ry)
		yMin = (int) controls[i].ry;
	      if (height < controls[i].ry)
		height = (int) controls[i].ry;
	    }
	  /* do we need to retranslate the box */
	  /* update horizontal origin of the box */
	  if (xMin != oldx)
	    {
	      dx = xMin - oldx;
	      *xOrg = PointToPixel (xMin / 1000);
	    }
	  else
	    dx = 0;
	  
	  /* update vertical origin of the box */
	  if (yMin != oldy)
	    {
	      dy = yMin - oldy;
	      *yOrg = PointToPixel (yMin / 1000);
	    }
	  else
	    dy = 0;
	}
      else
	{
	  dx = 0;
	  dy = 0;
	}
    }

  /* now update box width and height */
  if (width != Bbuffer->BuPoints[0].XCoord)
    {
      Bbuffer->BuPoints[0].XCoord = (int) width;
      Pbuffer->BuPoints[0].XCoord = (int) width;
    }
  if (height != Bbuffer->BuPoints[0].YCoord)
    {
      Bbuffer->BuPoints[0].YCoord = (int) height;
      Pbuffer->BuPoints[0].YCoord = (int) height;
    }
}


/*----------------------------------------------------------------------
  PolyLineCreation interract with the user to read the point forming
  a polyline in a given frame.
  *xOrg and *yOrg values gives the position of the box pBox in the frame.
  These values can be changed if there is limits depend of an other box
  (draw != NULL).
  Bbuffer points to the first buffer of the concrete box.
  Pbuffer points to the first buffer of the abstract box, i.e.
  the list of control points modified. The first point in the
  list gives the maximum width and height of the polyline.
  If the parameter maxPoints is greater than 1 if the number of
  points to be selected must be equal to this value.
  This fonction update both  list of control points and
  returns the number of point in the polyline.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                 PolyLineCreation (int frame, int *xOrg, int *yOrg, PtrBox pBox, PtrAbstractBox draw, int maxPoints)
#else  /* __STDC__ */
int                 PolyLineCreation (frame, xOrg, yOrg, pBox, draw, maxPoints)
int                 frame;
int                *xOrg;
int                *yOrg;
PtrBox              pBox;
PtrAbstractBox      draw;
int                 maxPoints;
#endif /* __STDC__ */
{
  ThotWindow          w;
  ThotEvent           event;
  PtrTextBuffer       Pbuffer;
  PtrTextBuffer       Bbuffer;
  float               ratioX, ratioY;
  int                 width, height;
  int                 ret;
  int                 x, y;
  int                 newx, newy, lastx, lasty;
  int                 xMin, yMin, xMax, yMax;
  int                 x1, y1, nbpoints;
# ifdef _WINDOWS
  RECT  rect;
  POINT cursorPos;
  BOOL  input = FALSE;
# else  /* _WINDOWS */
  int        e, f;
  int        dx, dy;
  ThotWindow wdum;
# endif /* _WINDOWS */

  if (pBox == NULL || pBox->BxAbstractBox == NULL)
    return (0);
  Bbuffer = pBox->BxBuffer;
  Pbuffer = pBox->BxAbstractBox->AbPolyLineBuffer;

  /* Initialize the bounding box */
  draw = NULL;
  if (draw != NULL && draw->AbBox != NULL) {
     /* constraint is done by the draw element */
     x = draw->AbBox->BxXOrg;
     *xOrg = x;
     width = draw->AbBox->BxWidth;
     /* trasformation factor between the box and the abstract box */
     ratioX = 1.;
     y = draw->AbBox->BxYOrg;
     *yOrg = y;
     /* trasformation factor between the box and the abstract box */
     ratioY = 1.;
     height = draw->AbBox->BxHeight;
     /* min and max will give the polyline bounding box */
     xMin = PixelToPoint (x + width) * 1000;
     xMax = 0;
     yMin = PixelToPoint (y + height) * 1000;
     yMax = 0;
  } else { 
         /* constraint is done by the polyline element */
         x = *xOrg;
         width = Bbuffer->BuPoints[0].XCoord;
         /* trasformation factor between the box and the abstract box */
         ratioX = (float) Pbuffer->BuPoints[0].XCoord / (float) width;
         width = PointToPixel (width / 1000);
         y = *yOrg;
         height = Bbuffer->BuPoints[0].YCoord;
         /* trasformation factor between the box and the abstract box */   
         ratioY = (float) Pbuffer->BuPoints[0].YCoord / (float) height;
         height = PointToPixel (height / 1000);
         /* we don't need to compute the polyline bounding box */
         xMin = yMin = xMax = yMax = 0;
  }
  x1 = -1;
  y1 = -1;
  nbpoints = 1;
  /* need the window to change the cursor */
  w = FrRef[frame];
# ifdef _WINDOWS
  GetWindowRect (w, &rect);
  /* The grid stepping begins at the origin */
  lastx = x + rect.left; 
  lasty = y + rect.top + FrameTable[frame].FrTopMargin;
  if (!SetCursorPos (lastx, lasty))
    WinErrorBox (FrRef[frame], TEXT("PolyLineCreation (1)"));
# else /* !_WINDOWS */
  e = ButtonPressMask | ButtonReleaseMask | ButtonMotionMask;
  XMapRaised (TtDisplay, w);
  XFlush (TtDisplay);
  ThotGrab (w, HVCurs, e, 0);
  /* The grid stepping begins at the origin */
  lastx = x; 
  lasty = y + FrameTable[frame].FrTopMargin;
  XWarpPointer (TtDisplay, None, w, 0, 0, 0, 0, lastx, lasty);
  XFlush (TtDisplay);
# endif /* _WINDOWS */


  /* shows up limit borders */
  BoxGeometry (frame, x, y, width, height, x + width - 2, y + height - 2, TRUE);

  /* loop waiting for the user input */
  ret = 0;
  while (ret == 0) {
#     ifdef _WINDOWS
      SetCursor (LoadCursor (NULL, IDC_CROSS));
      /* ShowCursor (TRUE); */
      GetMessage (&event, NULL, 0, 0);
      if (event.message == WM_MOUSEMOVE) {
         GetCursorPos (&cursorPos);
         /* current pointer position */
         /* coordinate checking */
         newx = DO_ALIGN (cursorPos.x - x);
         newx += x;
         newy = DO_ALIGN (cursorPos.y - FrameTable[frame].FrTopMargin - y);
         newy += y;
         if ((newx - rect.left) < x || (newx - rect.left) > x + width || (newy - rect.top) < y || (newy - rect.top) > y + height) {
            /* CHKR_LIMIT to size of the box */
            /* new X valid position */
            if (newx - rect.left < x)
               newx = x + rect.left;
            else if (newx - rect.left > x + width)
                 newx = x + width + rect.left;
            else
                 newx += rect.left;
	  
            /* new Y valid position */
            if (newy - rect.top < y)
               newy = y + FrameTable[frame].FrTopMargin + rect.top;
            else if (newy - rect.top > y + height)
                 newy = y + height + FrameTable[frame].FrTopMargin + rect.top;
            else
                 newy += FrameTable[frame].FrTopMargin + rect.top;
	  
            if (!SetCursorPos (newx, newy))
               WinErrorBox (FrRef [frame], TEXT("PolyLineCreation (2)"));
		 } else 
              newy += FrameTable[frame].FrTopMargin;
		 

         /* refresh the display of teh two adjacent segments */
         if (x1 != -1 && (newx != lastx || newy != lasty)) {
            POINT ptBeg;
            POINT ptEnd;
	  
            ptBeg.x = x1 - rect.left;
            ptBeg.y = y1 - rect.top;
            ptEnd.x = lastx - rect.left;
            ptEnd.y = lasty - rect.top;
	  
            DrawOutline(FrRef [frame], ptBeg, ptEnd);
	  
            ptBeg.x = x1 - rect.left;
            ptBeg.y = y1 - rect.top;
            ptEnd.x = newx - rect.left;
            ptEnd.y = newy - rect.top;
	  
            DrawOutline(FrRef [frame], ptBeg, ptEnd);
		 }
         lastx = newx;
         lasty = newy;
	  } else {
           /* coordinate checking */
           newx = x + DO_ALIGN ((int) cursorPos.x - x);
           newy = y + DO_ALIGN ((int) cursorPos.y - FrameTable[frame].FrTopMargin - y);
           /* CHKR_LIMIT to size of the box */
           /* new X valid position */
           if (newx - rect.left < x)
              lastx = x + rect.left;
           else if (newx - rect.left > x + width)
                lastx = x + width + rect.left;
           else
                lastx = newx;
	
           /* new Y valid position */
           if (newy - rect.top< y)
              lasty = y + FrameTable[frame].FrTopMargin + rect.top;
           else if (newy - rect.top> y + height)
                lasty = y + height + FrameTable[frame].FrTopMargin + rect.top;
           else
                lasty = newy + FrameTable[frame].FrTopMargin;
	
           switch (event.message) {
                  case WM_LBUTTONDOWN:
                  case WM_MBUTTONDOWN:
                  case WM_RBUTTONDOWN:
                       input = TRUE;
                       if (newx - rect.left < x || newx - rect.left > x + width || newy - rect.top < y || newy - rect.top > y + height)
	                      if (!SetCursorPos (lastx, lasty))
	                         WinErrorBox (FrRef [frame], TEXT("PolyLineCreation (3)"));
	                   break;
	  
	              case WM_LBUTTONUP:
	              case WM_MBUTTONUP:
                  case WM_RBUTTONUP:
					   if (event.message == WM_RBUTTONUP && maxPoints == 0)
                          ret = 1;

					   if (input) {
                          input = FALSE;
	                      /* left button keep the last segment built */
	                      /* keep the new segment first point coordinates */
	                      x1 = lastx;
	                      y1 = lasty;
                          /* nbpoints++; */
	  
                          /* update the box buffer */
                          newx = PixelToPoint (lastx - x - rect.left) * 1000;
                          newy = PixelToPoint (lasty - FrameTable[frame].FrTopMargin - y - rect.top) * 1000;
                          /* register the min and the max */
                          if (newx < xMin)
                             xMin = newx;
                          if (newx > xMax)
                             xMax = newx;
                          if (newy < yMin)
                             yMin = newy;
                          if (newy > yMax)
                             yMax = newy;
                          AddPointInPolyline (Bbuffer, nbpoints, newx, newy);
                          /* update the abstract box buffer */
                          newx = (int) ((float) newx * ratioX);
                          newy = (int) ((float) newy * ratioY);
                          AddPointInPolyline (Pbuffer, nbpoints, newx, newy);
                          nbpoints++;
                          if (nbpoints > maxPoints && maxPoints != 0)
                             /* we have the right number of points */
                             ret = 1;
                          else if (maxPoints == 0 && event.message == WM_MBUTTONUP)
                               /* any other button : end of user input */
                               ret = 1;
					   }
                       break;
	  
                  /*case WM_RBUTTONUP:
                       if (maxPoints == 0)
                          ret = 1;
                       break;*/
	  
                  default: break;
		   }
	  }
#     else /* !_WINDOWS */
      if (XPending (TtDisplay) == 0)
	{
	  /* current pointer position */
	  XQueryPointer (TtDisplay, w, &wdum, &wdum, &dx, &dy, &newx, &newy, &e);
	  /* coordinate checking */
	  newx = DO_ALIGN (newx - x);
	  newx += x;
	  newy = DO_ALIGN (newy - FrameTable[frame].FrTopMargin - y);
	  newy += y;
	  if (newx < x || newx > x + width || newy < y || newy > y + height)
	    {
	      /* CHKR_LIMIT to size of the box */
	      /* new X valid position */
	      if (newx < x)
		newx = x;
	      else if (newx > x + width)
		newx = x + width;

	      /* new Y valid position */
               if (newy < y)
		 newy = y + FrameTable[frame].FrTopMargin;
               else if (newy > y + height)
		 newy = y + height + FrameTable[frame].FrTopMargin;
               else
		 newy += FrameTable[frame].FrTopMargin;
               XWarpPointer (TtDisplay, None, w, 0, 0, 0, 0, newx, newy);
            }
	  else
	    newy += FrameTable[frame].FrTopMargin;

	  /* refresh the display of teh two adjacent segments */
	  if (x1 != -1 && (newx != lastx || newy != lasty))
	    {
	      XDrawLine (TtDisplay, FrRef[frame], TtInvertGC, x1, y1, lastx, lasty);
	      XDrawLine (TtDisplay, FrRef[frame], TtInvertGC, x1, y1, newx, newy);
	      XFlush (TtDisplay);
            }
	  lastx = newx;
	  lasty = newy;
	}
      else
	{
	  XNextEvent (TtDisplay, &event);
	  /* coordinate checking */
	  newx = x + DO_ALIGN ((int) event.xmotion.x - x);
	  newy = y + DO_ALIGN ((int) event.xmotion.y - FrameTable[frame].FrTopMargin - y);
	  /* CHKR_LIMIT to size of the box */
	  /* new X valid position */
	  if (newx < x)
	    lastx = x;
	  else if (newx > x + width)
	    lastx = x + width;
	  else
	    lastx = newx;

	  /* new Y valid position */
	  if (newy < y)
	    lasty = y + FrameTable[frame].FrTopMargin;
	  else if (newy > y + height)
	    lasty = y + height + FrameTable[frame].FrTopMargin;
	  else
	    lasty = newy + FrameTable[frame].FrTopMargin;

	  switch (event.type)
	    {
	    case ButtonPress:
	      if (newx < x || newx > x + width || newy < y || newy > y + height)
		XWarpPointer (TtDisplay, None, w, 0, 0, 0, 0, lastx, lasty);
	      break;

	    case ButtonRelease:
	      if (event.xbutton.button != Button3 || nbpoints > 0)
		{
		  /* left button keep the last segment built */
		  /* keep the new segment first point coordinates */
		  x1 = lastx;
		  y1 = lasty;
		  nbpoints++;

		  /* update the box buffer */
		  newx = PixelToPoint (lastx - x) * 1000;
		  newy = PixelToPoint (lasty - FrameTable[frame].FrTopMargin - y) * 1000;
		  /* register the min and the max */
		  if (newx < xMin)
		    xMin = newx;
		  if (newx > xMax)
		    xMax = newx;
		  if (newy < yMin)
		    yMin = newy;
		  if (newy > yMax)
		    yMax = newy;
		  AddPointInPolyline (Bbuffer, nbpoints, newx, newy);
		  /* update the abstract box buffer */
		  newx = (int) ((float) newx * ratioX);
		  newy = (int) ((float) newy * ratioY);
		  AddPointInPolyline (Pbuffer, nbpoints, newx, newy);

		  if (nbpoints > maxPoints && maxPoints != 0)
		    /* we have the right number of points */
		    ret = 1;
		  else if (maxPoints == 0 &&
			   event.xbutton.button != Button1)
		    /* any other button : end of user input */
		    ret = 1;
		}
	      break;

	    case Expose:
	      f = GetWindowFrame (event.xexpose.window);
	      if (f <= MAX_FRAME + 1)
		FrameToRedisplay (event.xexpose.window, f, (XExposeEvent *) & event);
	      XtDispatchEvent (&event);
	      break;

	    default: break;
	    }
	}
#     endif /* _WINDOWS */
    }
  /* erase box frame */
# ifdef _WINDOWS
  SetCursor (LoadCursor (NULL, IDC_ARROW));
  /* ShowCursor (TRUE); */
#  endif /* *_WINDOWS */
  BoxGeometry (frame, x, y, width, height, x + width - 2, y + height - 2, TRUE);

  /* get back to previous state of the library */
# ifndef _WINDOWS
  ThotUngrab ();
  XFlush (TtDisplay);
# endif /* _WINDOWS */

  /* need at least 3 points for a valid polyline */
  if (nbpoints < 3)
    {
      TtaDisplaySimpleMessage (INFO, LIB, TMSG_TWO_POINTS_IN_POLYLINE_NEEDED);
      return (1);
    }
  else if (draw)
    {
      x = PixelToPoint (x) * 1000;
      y = PixelToPoint (y) * 1000;
      SetBoundingBox (xMin+x, xMax+x, xOrg, yMin+y, yMax+y, yOrg, pBox, nbpoints);
    }
    return (nbpoints);
}

/*----------------------------------------------------------------------
  PolyLineModification interract with the user to move a point part
  a polyline in a given frame.
  *xOrg and *yOrg values gives the position of the box pBox in the frame.
  These values can be changed if there is limits depend of an other box
  (draw != NULL).
  Bbuffer points to the first buffer of the box, i.e. the list of control
  points as shown. The first point in the lists gives the maximum
  width and height of the polyline.
  Pbuffer points to the first buffer of the abtract box.
  nbpoints gives the number of points in the polyline.
  point expricitely indicate the point to be moved.
  This fonction update both  list of control points.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                PolyLineModification (int frame, int *xOrg, int *yOrg, PtrBox pBox, PtrAbstractBox draw, int nbpoints, int point, ThotBool close)
#else  /* __STDC__ */
void                PolyLineModification (frame, xOrg, yOrg, pBox, draw, nbpoints, point, close)
int                 frame;
int                *xOrg;
int                *yOrg;
PtrBox              pBox;
PtrAbstractBox      draw;
int                 nbpoints;
int                 point;
ThotBool            close;
#endif /* __STDC__ */
{
  ThotWindow          w;
  ThotEvent           event;
  PtrTextBuffer       Pbuffer;
  PtrTextBuffer       Bbuffer;
  float               ratioX, ratioY;
  int                 width, height;
  int                 ret;
  int                 newx, newy, lastx, lasty;
  int                 xMin, yMin, xMax, yMax;
  int                 x1, y1, x3, y3;
  int                 x, y, dx, dy;
  ThotBool            wrap;
# ifdef _WINDOWS
  RECT                rect;
  POINT               cursorPos;
  POINT               ptBegin, ptEnd;
# else  /* !_WINDOWS */
  int                 e, f;
# endif /* _WINDOWS */

  if (pBox == NULL || pBox->BxAbstractBox == NULL)
     return;

  Bbuffer = pBox->BxBuffer;
  Pbuffer = pBox->BxAbstractBox->AbPolyLineBuffer;

  /* Initialize the bounding box */
  draw = NULL;
  if (draw != NULL && draw->AbBox != NULL) {
     /* constraint is done by the draw element */
     x = draw->AbBox->BxXOrg;
     *xOrg = x;
     width = draw->AbBox->BxWidth;
     /* trasformation factor between the box and the abstract box */
     ratioX = 1.;
     y = draw->AbBox->BxYOrg;
     *yOrg = y;
     /* trasformation factor between the box and the abstract box */
     ratioY = 1.;
     height = draw->AbBox->BxHeight;
     /* min and max will give the polyline bounding box */
     xMin = PixelToPoint (x + width) * 1000;
     xMax = 0;
     yMin = PixelToPoint (y + height) * 1000;
     yMax = 0;
     dx = PixelToPoint ((x - *xOrg)) * 1000;
     dy = PixelToPoint ((y - *yOrg)) * 1000;
     TranslatePoints (dx, dy, Bbuffer, Pbuffer, nbpoints);
  } else {
         /* constraint is done by the polyline element */
         x = *xOrg;
         width = Bbuffer->BuPoints[0].XCoord;
         /* trasformation factor between the box and the abstract box */
         ratioX = (float) Pbuffer->BuPoints[0].XCoord / (float) width;
         width = PointToPixel (width / 1000);
         y = *yOrg;
         height = Bbuffer->BuPoints[0].YCoord;
         /* trasformation factor between the box and the abstract box */
         ratioY = (float) Pbuffer->BuPoints[0].YCoord / (float) height;
         height = PointToPixel (height / 1000);
         /* we don't need to compute the polyline bounding box */
         xMin = yMin = xMax = yMax = 0;
  }

  /* need the window to change the cursor */
  w = FrRef[frame];
# ifdef _WINDOWS
  if (!GetWindowRect (w, &rect))
     WinErrorBox (w, TEXT("PolyLineModification (1)"));

  wrap = FALSE;

  /* shows up limit borders */
  BoxGeometry (frame, x, y, width, height, x + width - 2, y + height - 2, TRUE);
  RedrawPolyLine (frame, x, y, Bbuffer, nbpoints, point, close, &x1, &y1, &lastx, &lasty, &x3, &y3, &xMin, &yMin, &xMax, &yMax);
  lastx += rect.left;
  lasty += rect.top;

  /* Loop waiting for user interraction */
  ret = 0;
  while (ret == 0) {
      SetCursor (LoadCursor (NULL, IDC_CROSS));
      /* ShowCursor (TRUE); */
      GetCursorPos (&cursorPos);
      
      /* check the coordinates */
      newx = x + DO_ALIGN (cursorPos.x - x);
      newy = y + DO_ALIGN (cursorPos.y - FrameTable[frame].FrTopMargin - y);
      /* are limited to the box size */
      /* Update the X position */
      if (newx - rect.left < x) {
         newx = x + rect.left;
         wrap = TRUE;
	  } else if (newx - rect.left > x + width) {
             newx = x + width + rect.left;
             wrap = TRUE;
	  }
      
      /* Update the Y position */
      if (newy - rect.top < y) {
         newy = y + FrameTable[frame].FrTopMargin + rect.top;
         wrap = TRUE;
	  } else if (newy - rect.top > y + height) {
             newy = y + height + FrameTable[frame].FrTopMargin + rect.top;
             wrap = TRUE;
	  } else
             newy += FrameTable[frame].FrTopMargin;

      GetMessage (&event, NULL, 0, 0);
      switch (event.message) {
             case WM_LBUTTONUP:
                  lastx = newx - rect.left;
                  lasty = newy - rect.top;
                  /* update the box buffer */
                  newx = PixelToPoint (lastx - x) * 1000;
                  newy = PixelToPoint (lasty - FrameTable[frame].FrTopMargin - y) * 1000;
                  /* register the min and the max */
                  if (newx < xMin)
                     xMin = newx;
                  if (newx > xMax)
                     xMax = newx;
                  if (newy < yMin)
                     yMin = newy;
                  if (newy > yMax)
                     yMax = newy;
                  ModifyPointInPolyline (Bbuffer, point, newx, newy);
                  /* update the abstract box buffer */
                  newx = (int) ((float) newx * ratioX);
                  newy = (int) ((float) newy * ratioY);
                  ModifyPointInPolyline (Pbuffer, point, newx, newy);
                  ret = 1;
                  break;
	  
             case WM_MOUSEMOVE:
                  /* shows the new adjacent segment position */
                  if (newx != lastx || newy != lasty) {
                     if (x1 != -1) {
                        ptBegin.x = x1;
						ptBegin.y = y1;
						ptEnd.x   = lastx - rect.left;
						ptEnd.y   = lasty - rect.top;
                        DrawOutline (w, ptBegin, ptEnd);
						ptEnd.x = newx - rect.left;
						ptEnd.y = newy - rect.top;
                        DrawOutline (w, ptBegin, ptEnd);
					 }
                     if (x3 != -1) {
                        ptBegin.x = lastx - rect.left;
						ptBegin.y = lasty - rect.top;
						ptEnd.x   = x3;
						ptEnd.y   = y3;
                        DrawOutline (w, ptBegin, ptEnd);
						ptBegin.x = newx - rect.left;
						ptBegin.y = newy - rect.top;
                        DrawOutline (w, ptBegin, ptEnd);
					 }
				  }
                  lastx = newx;
                  lasty = newy;
                  if (wrap)
                     wrap = FALSE;
                  break;
             /*	  
             case Expose:
                  f = GetWindowFrame (event.xexpose.window);
                  if (f <= MAX_FRAME + 1)
                     FrameToRedisplay (event.xexpose.window, f, (XExposeEvent *) & event);
                  XtDispatchEvent (&event);
                  break; */
	  
             default: break;
	  }
  }
  
  /* erase the box border */
  SetCursor (LoadCursor (NULL, IDC_ARROW));
  /* ShowCursor (TRUE); */
  BoxGeometry (frame, x, y, width, height, x + width - 2, y + height - 2, TRUE);

  /* returns to previous state of the Thot library */
  if (draw) {
     x = PixelToPoint (x) * 1000;
     y = PixelToPoint (y) * 1000;
     SetBoundingBox (xMin+x, xMax+x, xOrg, yMin+y, yMax+y, yOrg, pBox, nbpoints);
  }
# else  /* !_WINDOWS */
  e = ButtonPressMask | ButtonReleaseMask | ButtonMotionMask;
  XMapRaised (TtDisplay, w);
  XFlush (TtDisplay);
  ThotGrab (w, HVCurs, e, 0);
  wrap = FALSE;

  /* shows up limit borders */
  BoxGeometry (frame, x, y, width, height, x + width - 2, y + height - 2, TRUE);
  RedrawPolyLine (frame, x, y, Bbuffer, nbpoints, point, close,
		  &x1, &y1, &lastx, &lasty, &x3, &y3, &xMin, &yMin, &xMax, &yMax);
  /* the grid step begins at the origin of the box */
  XWarpPointer (TtDisplay, None, w, 0, 0, 0, 0, lastx, lasty);
  XFlush (TtDisplay);

  /* Loop waiting for user interraction */
  ret = 0;
  while (ret == 0)
    {
      XNextEvent (TtDisplay, &event);
      
      /* check the coordinates */
      newx = x + DO_ALIGN ((int) event.xmotion.x - x);
      newy = y + DO_ALIGN ((int) event.xmotion.y - FrameTable[frame].FrTopMargin - y);
      /* are limited to the box size */
      /* Update the X position */
      if (newx < x)
	{
	  newx = x;
	  wrap = TRUE;
	}
      else if (newx > x + width)
	{
	  newx = x + width;
	  wrap = TRUE;
	}
      
      /* Update the Y position */
      if (newy < y)
	{
	  newy = y + FrameTable[frame].FrTopMargin;
	  wrap = TRUE;
	}
      else if (newy > y + height)
	{
	  newy = y + height + FrameTable[frame].FrTopMargin;
	  wrap = TRUE;
	}
      else
	newy += FrameTable[frame].FrTopMargin;

      switch (event.type)
	{
	case ButtonRelease:
	  lastx = newx;
	  lasty = newy;
	  /* update the box buffer */
	  newx = PixelToPoint (lastx - x) * 1000;
	  newy = PixelToPoint (lasty - FrameTable[frame].FrTopMargin - y) * 1000;
	  /* register the min and the max */
	  if (newx < xMin)
	    xMin = newx;
	  if (newx > xMax)
	    xMax = newx;
	  if (newy < yMin)
	    yMin = newy;
	  if (newy > yMax)
	    yMax = newy;
	  ModifyPointInPolyline (Bbuffer, point, newx, newy);
	  /* update the abstract box buffer */
	  newx = (int) ((float) newx * ratioX);
	  newy = (int) ((float) newy * ratioY);
	  ModifyPointInPolyline (Pbuffer, point, newx, newy);
	  ret = 1;
	  break;
	  
	case MotionNotify:
	  /* shows the new adjacent segment position */
	  if (newx != lastx || newy != lasty)
	    {
	      if (x1 != -1)
		{
		  XDrawLine (TtDisplay, FrRef[frame], TtInvertGC, x1, y1, lastx, lasty);
		  XDrawLine (TtDisplay, FrRef[frame], TtInvertGC, x1, y1, newx, newy);
		}
	      if (x3 != -1)
		{
		  XDrawLine (TtDisplay, FrRef[frame], TtInvertGC, lastx, lasty, x3, y3);
		  XDrawLine (TtDisplay, FrRef[frame], TtInvertGC, newx, newy, x3, y3);
		}
	      XFlush (TtDisplay);
	    }
	  lastx = newx;
	  lasty = newy;
	  if (wrap)
	    {
	      XWarpPointer (TtDisplay, None, w, 0, 0, 0, 0, lastx, lasty);
	      wrap = FALSE;
	    }
	  break;
	  
	case Expose:
	  f = GetWindowFrame (event.xexpose.window);
	  if (f <= MAX_FRAME + 1)
	    FrameToRedisplay (event.xexpose.window, f, (XExposeEvent *) & event);
	  XtDispatchEvent (&event);
	  break;
	  
	default:
	  break;
	}
    }
  
  /* erase the box border */
  BoxGeometry (frame, x, y, width, height, x + width - 2, y + height - 2, TRUE);

  /* returns to previous state of the Thot library */
  ThotUngrab ();
  XFlush (TtDisplay);
  if (draw)
    {
      x = PixelToPoint (x) * 1000;
      y = PixelToPoint (y) * 1000;
      SetBoundingBox (xMin+x, xMax+x, xOrg, yMin+y, yMax+y, yOrg, pBox, nbpoints);
    }
# endif /* _WINDOWS */
}

/*----------------------------------------------------------------------
  PolyLineExtension interract with the user to add points to
  an existing polyline in a given frame.
  *xOrg and *yOrg values gives the position of the box pBox in the frame.
  These values can be changed if there is limits depend of an other box
  (draw != NULL).
  Bbuffer points to the first buffer of the box, i.e. the list of control
  points as shown. The first point in the lists gives the maximum
  width and height of the polyline.
  Pbuffer points to the first buffer of the abtract box.
  nbpoints gives the number of points in the polyline.
  point expricitely indicate the point to be moved.
  close indicate whether the polyline is closed.
  This fonction update both  list of control points and
  returns the new number of points in the polyline.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                 PolyLineExtension (int frame, int *xOrg, int *yOrg, PtrBox pBox, PtrAbstractBox draw, int nbpoints, int point, ThotBool close)
#else  /* __STDC__ */
int                 PolyLineExtension (frame, xOrg, yOrg, pBox, draw, nbpoints, point, close)
int                 frame;
int                *xOrg;
int                *yOrg;
PtrBox              pBox;
PtrAbstractBox      draw;
int                 nbpoints;
int                 point;
ThotBool            close;
#endif /* __STDC__ */

{
  ThotWindow          w;
  PtrTextBuffer       Pbuffer;
  PtrTextBuffer       Bbuffer;
  float               ratioX, ratioY;
  int                 width, height;
  int                 dx, dy;
  int                 xMin, yMin, xMax, yMax;
  int                 x, y;
# ifndef _WINDOWS
  ThotWindow          wdum;
  ThotEvent           event;
  int                 e;
  int                 ret, f;
  int                 newx, newy;
  int                 lastx, lasty;
  int                 x1, y1, x3, y3;
  ThotBool            wrap;
  ThotBool            start = TRUE;
# endif /* _WINDOWS */

  if (pBox == NULL || pBox->BxAbstractBox == NULL)
    return (0);
  Bbuffer = pBox->BxBuffer;
  Pbuffer = pBox->BxAbstractBox->AbPolyLineBuffer;

  /* Initialize the bounding box */
  draw = NULL;
  if (draw != NULL && draw->AbBox != NULL) {
     /* constraint is done by the draw element */
     x = draw->AbBox->BxXOrg;
     *xOrg = x;
     width = draw->AbBox->BxWidth;
     /* trasformation factor between the box and the abstract box */
     ratioX = 1.;
     y = draw->AbBox->BxYOrg;
     *yOrg = y;
     /* trasformation factor between the box and the abstract box */
     ratioY = 1.;
     height = draw->AbBox->BxHeight;
     /* min and max will give the polyline bounding box */
     xMin = PixelToPoint (x + width) * 1000;
     xMax = 0;
     yMin = PixelToPoint (y + height) * 1000;
     yMax = 0;
     dx = PixelToPoint (x - *xOrg) * 1000;
     dy = PixelToPoint (y - *yOrg) * 1000;
     TranslatePoints (dx, dy, Bbuffer, Pbuffer, nbpoints);
  } else {
         /* constraint is done by the polyline element */
         x = *xOrg;
         width = Bbuffer->BuPoints[0].XCoord;
         /* trasformation factor between the box and the abstract box */
         ratioX = (float) Pbuffer->BuPoints[0].XCoord / (float) width;
         width = PointToPixel (width / 1000);
         y = *yOrg;
         height = Bbuffer->BuPoints[0].YCoord;
         /* trasformation factor between the box and the abstract box */
         ratioY = (float) Pbuffer->BuPoints[0].YCoord / (float) height;
         height = PointToPixel (height / 1000);
         /* we don't need to compute the polyline bounding box */
         xMin = yMin = xMax = yMax = 0;
  }

  /* need the window to change the cursor */
  w = FrRef[frame];

# ifdef _WINDOWS
# else  /* !_WINDOWS */
  e = ButtonPressMask | ButtonReleaseMask | ButtonMotionMask;
  XMapRaised (TtDisplay, w);
  XFlush (TtDisplay);
  ThotGrab (w, HVCurs, e, 0);
  wrap = FALSE;

  /* draw the box border */
  BoxGeometry (frame, x, y, width, height, x + width - 2, y + height - 2, TRUE);
  RedrawPolyLine (frame, x, y, Bbuffer, nbpoints, point, close,
		  &x1, &y1, &lastx, &lasty, &x3, &y3, &xMin, &yMin, &xMax, &yMax);
  /* the grid step begins at the origin */
  XFlush (TtDisplay);
  XWarpPointer (TtDisplay, None, w, 0, 0, 0, 0, lastx, lasty);
  x1 = lastx;
  y1 = lasty;

  /* Loop waiting for user input */
  ret = 0;
  while (ret == 0)
    {
      if (XPending (TtDisplay) == 0)
	{
	  /* current cursor location */
	  XQueryPointer (TtDisplay, w, &wdum, &wdum, &dx, &dy, &newx, &newy, &e);
	  /* check the coordinates */
	  newx = DO_ALIGN (newx - x);
	  newx += x;
	  newy = DO_ALIGN (newy - FrameTable[frame].FrTopMargin - y);
	  newy += y;
	  if (newx < x || newx > x + width || newy < y || newy > y + height)
	    {
	      /* CHKR_LIMIT them to the box area */
	      /* new X valid position */
	      if (newx < x)
		newx = x;
	      else if (newx > x + width)
		newx = x + width;
	      
	      /* new Y valid position */
	      if (newy < y)
		newy = y + FrameTable[frame].FrTopMargin;
	      else if (newy > y + height)
		newy = y + height + FrameTable[frame].FrTopMargin;
	      else
		newy += FrameTable[frame].FrTopMargin;
	      XWarpPointer (TtDisplay, None, w, 0, 0, 0, 0, newx, newy);
	    }
	  else
	    newy += FrameTable[frame].FrTopMargin;
	  
	  /* Draw the new segments resulting of the new point */
	  if (newx != lastx || newy != lasty)
	    {
	      if (x1 != -1)
		{
		  XDrawLine (TtDisplay, FrRef[frame], TtInvertGC, x1, y1, lastx, lasty);
		  XDrawLine (TtDisplay, FrRef[frame], TtInvertGC, x1, y1, newx, newy);
		}
	      if (x3 != -1)
		{
		  XDrawLine (TtDisplay, FrRef[frame], TtInvertGC, lastx, lasty, x3, y3);
		  XDrawLine (TtDisplay, FrRef[frame], TtInvertGC, newx, newy, x3, y3);
		}
	      
	      XFlush (TtDisplay);
	      lastx = newx;
	      lasty = newy;
	    }
	}
      else
	{
	  XNextEvent (TtDisplay, &event);
	  
	  /* check the coordinates */
	  newx = x + DO_ALIGN ((int) event.xmotion.x - x);
	  newy = y + DO_ALIGN ((int) event.xmotion.y - FrameTable[frame].FrTopMargin - y);
	  /* CHKR_LIMIT them to the box area */
	  if (newx < x)
	    {
	      newx = x;
	      wrap = TRUE;
	    }
	  else if (newx > x + width)
	    {
	      newx = x + width;
	      wrap = TRUE;
	    }

	  if (newy < y)
	    {
	      newy = y + FrameTable[frame].FrTopMargin;
	      wrap = TRUE;
	    }
	  else if (newy > y + height)
	    {
	      newy = y + height + FrameTable[frame].FrTopMargin;
	      wrap = TRUE;
	    }
	  else
	    newy += FrameTable[frame].FrTopMargin;

	  switch (event.type)
	    {
	    case ButtonPress:
	      lastx = newx;
	      lasty = newy;
	      start = FALSE;
	      if (wrap)
		{
		  XWarpPointer (TtDisplay, None, w, 0, 0, 0, 0, lastx, lasty);
		  wrap = FALSE;
		}
	      break;
	      
	    case ButtonRelease:
	      if (event.xbutton.button != Button3 || !start)
		{
		  /* left button : keep the new point */
		  /* write down the new segment start point */
		  lastx = newx;
		  lasty = newy;
		  x1 = lastx;
		  y1 = lasty;
		  nbpoints++;
		  point++;
		  start = FALSE;
		  /* update the box buffer */
		  newx = PixelToPoint (lastx - x) * 1000;
		  newy = PixelToPoint (lasty - FrameTable[frame].FrTopMargin - y) * 1000;
		  /* register the min and the max */
		  if (newx < xMin)
		    xMin = newx;
		  if (newx > xMax)
		    xMax = newx;
		  if (newy < yMin)
		    yMin = newy;
		  if (newy > yMax)
		    yMax = newy;
		  AddPointInPolyline (Bbuffer, point, newx, newy);
		  /* update the abstact box buffer */
		  newx = (int) ((float) newx * ratioX);
		  newy = (int) ((float) newy * ratioY);
		  AddPointInPolyline (Pbuffer, point, newx, newy);
		  
		  if (event.xbutton.button != Button1)
		    /* any other button : end of interraction */
		    ret = 1;
		}
	      break;

	    case Expose:
	      f = GetWindowFrame (event.xexpose.window);
	      if (f <= MAX_FRAME + 1)
		FrameToRedisplay (event.xexpose.window, f, (XExposeEvent *) & event);
	      XtDispatchEvent (&event);
	      break;
	      
	    default:
	      break;
	    }
	}
    }

  /* erase the box border */
  BoxGeometry (frame, x, y, width, height, x + width - 2, y + height - 2, TRUE);

  /* go back to previous state of Thot library */
  ThotUngrab ();
  XFlush (TtDisplay);
# endif /* _WINDOWS */
  /* a valid polyline need at least 3 points */
  if (nbpoints < 3)
    return (1);
  else if (draw)
    {
      x = PixelToPoint (x) * 1000;
      y = PixelToPoint (y) * 1000;
      SetBoundingBox (xMin+x, xMax+x, xOrg, yMin+y, yMax+y, yOrg, pBox, nbpoints);
    }
  return (nbpoints);
}

/*----------------------------------------------------------------------
  UserGeometryCreate draw a box at a specific (x,y) location in
  frame and of size width x height when interracting with
  the user to mofify a box size (button press).
  xmin, xmax, ymin, ymax are the maximum values allowed.
  PosX, PosY, DimX, DimY will be different to zero if these positions or
  dimensions are user specified.
  Parameters percentW and percentH will be equal to zero if width
  and height are independent or will get the percent report (percentH = 100
  if height is equal to width).
  this function returns the new values upon button release.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                UserGeometryCreate (int frame, int *x, int *y, int xr, int yr, int *width, int *height, int xmin, int xmax, int ymin, int ymax, int PosX, int PosY, int DimX, int DimY, int percentW, int percentH)
#else  /* __STDC__ */
void                UserGeometryCreate (frame, x, y, xr, yr, width, height, xmin, xmax, ymin, ymax, PosX, PosY, DimX, DimY, percentW, percentH)
int                 frame;
int                *x;
int                *y;
int                 xr;
int                 yr;
int                *width;
int                *height;
int                 xmin;
int                 xmax;
int                 ymin;
int                 ymax;
int                 PosX;
int                 PosY;
int                 DimX;
int                 DimY;
int                 percentW;
int                 percentH;
#endif /* __STDC__ */

{
  int                 xm, ym;
  int                 ret, dx, dy;
  int                 nx, ny;
  ThotEvent           event;
  ThotWindow          w;
  int                 RightOrLeft, BottomOrTop;
  int                 warpx, warpy;
# ifdef _WINDOWS
  RECT            rect;
  POINT cursorPos;    
# else  /* _WINDOWS */
  int        e, f;
  ThotWindow wdum;
# endif /* _WINDOWS */

  /* use relative coordinates */
  xr -= *x;
  yr -= *y;
  xm = ym = 0;

  /* change the cursor, modify library state */
  w = FrRef[frame];
#ifdef _WINDOWS
  /* if (!GetWindowPlacement (w, &wp)) */
  if (!GetWindowRect (w, &rect))
    WinErrorBox (w, TEXT("UserGeometryCreate (1)"));
#else /* !_WINDOWS */
  e = ButtonPressMask | ButtonReleaseMask | ButtonMotionMask;
  ThotGrab (w, HVCurs, e, 0);
#endif /* !_WINDOWS */

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

#ifdef _WINDOWS 
  /* if (!SetCursorPos (*x + wp.rcNormalPosition.left, *y + wp.rcNormalPosition.top)) */
  if (!SetCursorPos (*x + rect.left, *y + rect.top))
    /* if (!SetCursorPos (*x, *y)) */
    WinErrorBox (FrRef [frame], TEXT("UserGeometryCreate (2)"));
#else  /* !_WINDOWS */
  XMapRaised (TtDisplay, w);
  XWarpPointer (TtDisplay, None, w, 0, 0, 0, 0, *x, *y + FrameTable[frame].FrTopMargin);
  XFlush (TtDisplay);
#endif /* !_WINDOWS */

  /* draw the current box geometry */
  BoxGeometry (frame, *x, *y, *width, *height, *x + xr, *y + yr, FALSE);

  /* Loop on user input to keep the first point */
  ret = 0;
  while (ret == 0)
    {
#ifdef _WINDOWS
      SetCursor (LoadCursor (NULL, IDC_CROSS));
      /* ShowCursor (TRUE); */
      GetMessage (&event, NULL, 0, 0);
      if (event.message == WM_MOUSEMOVE) {
         GetCursorPos (&cursorPos);
         /* check the coordinates are withing limits */
         nx = DO_ALIGN (cursorPos.x - xmin);
         nx += xmin;
         ny = DO_ALIGN (cursorPos.y - FrameTable[frame].FrTopMargin - ymin);
         ny += ymin;
         if ((nx - rect.left) < xmin || (nx - rect.left) > xmax || (ny - rect.top) < ymin || (ny - rect.top) > ymax)
            SetCursorPos (*x + rect.left, *y + rect.top);
         else if ((nx != *x && PosX) || (ny != *y && PosY)) {
              BoxGeometry (frame, *x, *y, *width, *height, *x + xr, *y + yr, FALSE);
         if (PosX)
            *x = nx - rect.left;
         if (PosY)
            *y = ny - rect.top;

         BoxGeometry (frame, *x, *y, *width, *height, *x + xr, *y + yr, FALSE);
         /* the postion is fixed */
         if (!PosX || !PosY)
            SetCursorPos (*x + rect.left, *y + rect.top);
		 }
	  } else {
             switch (event.message) {
                    case WM_LBUTTONDOWN:
                    case WM_MBUTTONDOWN:
                    case WM_RBUTTONDOWN:	  
                         GetCursorPos (&cursorPos);
                         if (PosX)
                            xm = xmin + DO_ALIGN ((int) cursorPos.x - xmin);
                         else
                              xm = *x;
                         if (PosY)
                            ym = ymin + DO_ALIGN ((int) cursorPos.y - FrameTable[frame].FrTopMargin - ymin);
                         else
                             ym = *y;
			      
                         /* check the coordinates */
                         if ((xm - rect.left) < xmin || (xm - rect.left) > xmax || !PosX || (ym - rect.top) < ymin || (ym - rect.top) > ymax || !PosY) {
                            SetCursorPos (xm + rect.left, ym + rect.top);
                            if (!PosX && !PosY)
                               ret = 1;
						 } else
                                ret = 1;
	                     break; 

                    default: break;
			 } 
      } 
#else  /* !_WINDOWS */
      if (XPending (TtDisplay) == 0)
	{
	  /* get the cursor location */
	  XQueryPointer (TtDisplay, w, &wdum, &wdum, &dx, &dy, &nx, &ny, &e);
	  /* check the coordinates are withing limits */
	  nx = DO_ALIGN (nx - xmin);
	  nx += xmin;
	  ny = DO_ALIGN (ny - FrameTable[frame].FrTopMargin - ymin);
	  ny += ymin;
	  if (nx < xmin || nx > xmax || ny < ymin || ny > ymax)
	    XWarpPointer (TtDisplay, None, w, 0, 0, 0, 0, *x, *y + FrameTable[frame].FrTopMargin);
	  else if ((nx != *x && PosX) || (ny != *y && PosY))
	    {
	      BoxGeometry (frame, *x, *y, *width, *height, *x + xr, *y + yr, FALSE);
	      if (PosX)
		*x = nx;
	      if (PosY)
		*y = ny;

	      BoxGeometry (frame, *x, *y, *width, *height, *x + xr, *y + yr, FALSE);
	      XFlush (TtDisplay);
	      /* the postion is fixed */
	      if (!PosX || !PosY)
		XWarpPointer (TtDisplay, None, w, 0, 0, 0, 0, *x, *y + FrameTable[frame].FrTopMargin);
            }
	}
      else
	{
	  XNextEvent (TtDisplay, &event);

	  /* we only deal with button press events */
	  switch (event.type)
	    {
	    case ButtonPress:
	      if (PosX)
		xm = xmin + DO_ALIGN ((int) event.xmotion.x - xmin);
	      else
		xm = *x;
	      if (PosY)
		ym = ymin + DO_ALIGN ((int) event.xmotion.y - FrameTable[frame].FrTopMargin - ymin);
	      else
		ym = *y;
	    
	      /* check the coordinates */
	      if (xm < xmin || xm > xmax || !PosX || ym < ymin || ym > ymax || !PosY)
		{
		  XWarpPointer (TtDisplay, None, w, 0, 0, 0, 0,
				xm,
				ym + FrameTable[frame].FrTopMargin);
		  if (!PosX && !PosY)
		    ret = 1;
		}
	      else
		ret = 1;
	      break;
	  case Expose:
	    f = GetWindowFrame (event.xexpose.window);
	    if (f <= MAX_FRAME + 1)
	      FrameToRedisplay (event.xexpose.window, f, (XExposeEvent *) & event);
	    XtDispatchEvent (&event);
	    break;
	  default: break;
	  }
	}
#endif /* !_WINDOWS */
    }

  /* switch off the old box geometry */
  BoxGeometry (frame, *x, *y, *width, *height, *x + xr, *y + yr, FALSE);
#ifndef _WINDOWS 
  *x = xm;
  *y = ym; 
#else  /* _WINDOWS */
  *x = xm - rect.left;
  *y = ym - rect.top;
#endif /* _WINDOWS */
  xr = 2;
  yr = 2;
  ym += FrameTable[frame].FrTopMargin;
  BoxGeometry (frame, *x, *y, *width, *height, *x + xr, *y + yr, FALSE);
  /* indicate that the original position is the reference */
  RightOrLeft = 2;
  BottomOrTop = 2;

  /* New loop waiting for the second point definition */
  ret = 0;
  while (ret == 0) {
#ifdef _WINDOWS
    SetCursor (LoadCursor (NULL, IDC_CROSS));
    /* ShowCursor (TRUE); */
    GetMessage (&event, NULL, 0, 0);
    switch (event.message) {
           case WM_LBUTTONUP:
           case WM_MBUTTONUP:
           case WM_RBUTTONUP:
                GetCursorPos (&cursorPos);
                SetCursor (LoadCursor (NULL, IDC_ARROW));
                /* ShowCursor (TRUE); */
                ret = 1;
                break;
           case WM_MOUSEMOVE:
                warpx = -1;
                warpy = -1;
      
                if (DimX) {
                   dx = (int) event.pt.x - xm;
                   nx = xmin + DO_ALIGN (*x + *width + dx - xmin);
                   dx = nx - *x - *width;
				} else
                       dx = 0;
      
                if (DimY) {
                   dy = (int) event.pt.y - ym;
                   ny = ymin + DO_ALIGN (*y + *height + dy - ymin);
                   dy = ny - *y - *height;
				} else
                       dy = 0;
      
                xm += dx;
                ym += dy;
                /* Echange the defaults values by the actual one */
                /* if the width and height are non zero */
                if ((dx != 0 || !DimX) && (dy != 0 || !DimY) && RightOrLeft == 2) {
                   RightOrLeft = 1;
                   BottomOrTop = 1;
                   BoxGeometry (frame, *x, *y, *width, *height, *x + xr, *y + yr, FALSE);
                   if (DimX) {
                      *width = 0;
                      if (percentH != 0)
                         *height = 0;
				   }

                   if (DimY) {
                      *height = 0;
                      if (percentW != 0)
                         *width = 0;
				   } 
                   BoxGeometry (frame, *x, *y, *width, *height, *x + xr, *y + yr, FALSE);
				} 
      
                /* left or right position to modify */
                if (dx != 0)
                   if (RightOrLeft > 0) { /* right */
                      /* should we swap left and right values */
                      if (dx < 0 && -dx > *width) {
                         RightOrLeft = 0;
                         nx = *x + *width + dx; /* new origin, check that we are still within limits */
                         if (nx < xmin) {
                            nx = xmin;
                            warpx = xmin;
						 } 
                         dx = *x - nx; /* new width */
					  } else {
                             nx = *x; /* new origin */
                             dx += *width; /* new width, check that we are still within limits */
                             if (nx + dx > xmax) {
                                dx = xmin + DO_ALIGN (xmax - xmin) - nx;
                                warpx = xmax;
							 } 
					  } 
				   } else {
                          /* left, treatment is similar to right */
                          if (dx > *width) {
                             RightOrLeft = 1;
                             nx = *x + *width;
                             dx -= *width;
                             if (nx + dx > xmax) {
                                dx = xmin + DO_ALIGN (xmax - xmin) - nx;
                                warpx = xmax;
							 }
						  } else {
                                 nx = *x + dx;
                                 if (nx < xmin) {
                                    nx = xmin;
                                    warpx = xmin;
								 } 
                                 dx = *x + *width - nx;
						  } 
				   }  
                else {
                     nx = *x;
                     dx = *width;
				} 

                /* Top or bottom CHKR_LIMIT to modify */
                if (dy != 0)
                   if (BottomOrTop > 0) { /* bottom */
                      /* should we swap bottom and top ? */
                      if (dy < 0 && -dy > *height) {
                         BottomOrTop = 0;
                         ny = *y + *height + dy; /* new origin, check for limits */
                         if (ny < ymin) {
                            ny = ymin;
                            warpy = ymin;
						 } 
                         dy = *y - ny; /* new height */
					  } else {
                             ny = *y; /*new origin */
                             dy += *height; /*new height, check for limits */
                             if (ny + dy > ymax) {
                                dy = ymin + DO_ALIGN (ymax - ymin) - ny;
                                warpy = ymax;
							 } 
					  } 
				   } else {
                          /* Top treatemnt similar to bottom */
                          if (dy > *height) {
                             BottomOrTop = 1;
                             ny = *y + *height;
                             dy -= *height;
                             if (ny + dy > ymax) {
                                dy = ymin + DO_ALIGN (ymax - ymin) - ny;
                                warpy = ymax;
							 } 
						  } else {
                                 ny = *y + dy;
                                 if (ny < ymin) {
                                    ny = ymin;
                                    warpy = ymin;
								 }
                                 dy = *y + *height - ny;
						  } 
				   } 
                else {
                     ny = *y;
                     dy = *height;
				}   

                /* should we redraw the box because geometry has changed */
                if (nx != *x || ny != *y || dx != *width || dy != *height) {
                   /* switch off the old box */
                   BoxGeometry (frame, *x, *y, *width, *height, *x + xr, *y + yr, FALSE);
                   /* is there any dependence between height and width */
                   if (percentW != 0)
                      dx = dy * percentW / 100;
                   else if (percentH != 0)
                        dy = dx * percentH / 100;
                   /* switch on the new box */
                   BoxGeometry (frame, nx, ny, dx, dy, nx + xr, ny + yr, FALSE);
                   *x = nx;
                   *y = ny;
                   *width = dx;
                   *height = dy;
				}    
      
                /* should we move the cursor */
                if (warpx >= 0 || warpy >= 0) {
                   if (warpx >= 0)
                      xm = warpx;
                   if (warpy >= 0)
                      ym = warpy + FrameTable[frame].FrTopMargin;
                   if (!SetCursorPos (xm, ym))
                      WinErrorBox (FrRef [frame], TEXT("UserGeometryCreate (3)"));
				} 
                break;
      
           default: break;
    }
#else  /* !_WINDOWS */
    XNextEvent (TtDisplay, &event);
    /* On analyse le type de l'evenement */
    switch (event.type)
      {
      case ButtonRelease:
	ret = 1;
      case MotionNotify:
	warpx = -1;
	warpy = -1;

	/* check this is the correct window */
	if (event.xmotion.window == w)
	  {
	    /* new cursor location */
	    if (DimX)
	      {
		dx = (int) event.xmotion.x - xm;
		nx = xmin + DO_ALIGN (*x + *width + dx - xmin);
		dx = nx - *x - *width;
	      }
	    else
	      dx = 0;

	    if (DimY)
	      {
		dy = (int) event.xmotion.y - ym;
		ny = ymin + DO_ALIGN (*y + *height + dy - ymin);
		dy = ny - *y - *height;
	      }
	    else
	      dy = 0;

	    xm += dx;
	    ym += dy;
	    /* Echange the defaults values by the actual one */
	    /* if the width and height are non zero */
	    if ((dx != 0 || !DimX) && (dy != 0 || !DimY) && RightOrLeft == 2)
	      {
		RightOrLeft = 1;
		BottomOrTop = 1;
		BoxGeometry (frame, *x, *y, *width, *height, *x + xr, *y + yr, FALSE);
		if (DimX)
		  {
		    *width = 0;
		    if (percentH != 0)
		      *height = 0;
		  }
		if (DimY)
		  {
		    *height = 0;
		    if (percentW != 0)
		      *width = 0;
		  }
		BoxGeometry (frame, *x, *y, *width, *height, *x + xr, *y + yr, FALSE);
	      }
	  }
	else
	  {
	    dx = 0;
	    dy = 0;
	  }
	 
	/* left or right position to modify */
	if (dx != 0)
	  if (RightOrLeft > 0)
	    { /* right */
	      /* should we swap left and right values */
	      if (dx < 0 && -dx > *width)
		{
		  RightOrLeft = 0;
		  /* new origin, check that we are still within limits */
		  nx = *x + *width + dx;
		  if (nx < xmin)
		    {
		      nx = xmin;
		      warpx = xmin;
		    }
		  dx = *x - nx; /* new width */
		}
	      else
		{
		  nx = *x; /* new origin */
		  /* new width, check that we are still within limits */
		  dx += *width;
		  if (nx + dx > xmax)
		    {
		      dx = xmin + DO_ALIGN (xmax - xmin) - nx;
		      warpx = xmax;
		    }
		}
	    }
	  else
	    {
	      /* left, treatment is similar to right */
	      if (dx > *width)
		{
		  RightOrLeft = 1;
		  nx = *x + *width;
		  dx -= *width;
		  if (nx + dx > xmax)
		    {
		      dx = xmin + DO_ALIGN (xmax - xmin) - nx;
		      warpx = xmax;
		    }
		}
	      else
		{
		  nx = *x + dx;
		  if (nx < xmin)
		    {
		      nx = xmin;
		      warpx = xmin;
		    }
		  dx = *x + *width - nx;
		}
	    } 
	else
	  {
	    nx = *x;
	    dx = *width;
	  }

	/* Top or bottom CHKR_LIMIT to modify */
	if (dy != 0)
	  if (BottomOrTop > 0)
	    { /* bottom */
	      /* should we swap bottom and top ? */
	      if (dy < 0 && -dy > *height)
		{
		  BottomOrTop = 0;
		  ny = *y + *height + dy; /* new origin, check for limits */
		  if (ny < ymin)
		    {
		      ny = ymin;
		      warpy = ymin;
		    }
		  dy = *y - ny; /* new height */
		}
	      else
		{
		  ny = *y; /*new origin */
		  dy += *height; /*new height, check for limits */
		  if (ny + dy > ymax)
		    {
		      dy = ymin + DO_ALIGN (ymax - ymin) - ny;
		      warpy = ymax;
		    }
		}
	    }
	  else
	    {
	      /* Top treatemnt similar to bottom */
	      if (dy > *height)
		{
		  BottomOrTop = 1;
		  ny = *y + *height;
		  dy -= *height;
		  if (ny + dy > ymax)
		    {
		      dy = ymin + DO_ALIGN (ymax - ymin) - ny;
		      warpy = ymax;
		    }
		}
	      else
		{
		  ny = *y + dy;
		  if (ny < ymin)
		    {
		      ny = ymin;
		      warpy = ymin;
		    }
		  dy = *y + *height - ny;
		}
	    }
	else
	  {
	    ny = *y;
	    dy = *height;
	  }  

	/* should we redraw the box because geometry has changed */
	if (nx != *x || ny != *y || dx != *width || dy != *height)
	  {
	    /* switch off the old box */
	    BoxGeometry (frame, *x, *y, *width, *height, *x + xr, *y + yr, FALSE);
	    /* is there any dependence between height and width */
	    if (percentW != 0)
	      dx = dy * percentW / 100;
	    else if (percentH != 0)
	      dy = dx * percentH / 100;
	    /* switch on the new box */
	    BoxGeometry (frame, nx, ny, dx, dy, nx + xr, ny + yr, FALSE);
	    XFlush (TtDisplay);
	    *x = nx;
	    *y = ny;
	    *width = dx;
	    *height = dy;
	  }   

	/* should we move the cursor */
	if (warpx >= 0 || warpy >= 0)
	  {
	    if (warpx >= 0)
	      xm = warpx;
	    if (warpy >= 0)
	      ym = warpy + FrameTable[frame].FrTopMargin;
	    XWarpPointer (TtDisplay, None, w, 0, 0, 0, 0, xm, ym);
	  }
	break;
      default: break;
      }
#endif /* !_WINDOWS */
  }

  /* Erase the box drawing */
  BoxGeometry (frame, *x, *y, *width, *height, *x + xr, *y + yr, FALSE);
#ifndef _WINDOWS 
  /* restore state of the Thot Library */
  ThotUngrab ();
  XFlush (TtDisplay);
#else  /* _WINDOWS */
  w = FrRef[frame];
#endif /* _WINDOWS */
}

/*----------------------------------------------------------------------
  UserGeometryMove draw a box at a specific (x,y) location in
  frame and of size width x height when interracting with
  the user to mofify the box position (button press).
  xmin, xmax, ymin, ymax are the maximum values allowed.
  xm and ym gives the initial mouse coordinates in the frame.
  this function returns the new position upon button release.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                UserGeometryMove (int frame, int *x, int *y, int width, int height, int xr, int yr, int xmin, int xmax, int ymin, int ymax, int xm, int ym)
#else  /* __STDC__ */
void                UserGeometryMove (frame, x, y, width, height, xr, yr, xmin, xmax, ymin, ymax, xm, ym)
int                 frame;
int                *x;
int                *y;
int                 width;
int                 height;
int                 xr;
int                 yr;
int                 xmin;
int                 xmax;
int                 ymin;
int                 ymax;
int                 xm;
int                 ym;
#endif /* __STDC__ */
{
   int                 ret, dx, dy, nx, ny;
   ThotEvent              event;
   ThotWindow          w;
   int                 warpx, warpy;
#  ifdef _WINDOWS
   RECT                rect;
   POINT               cursorPos;
#  else  /* !_WINDOWS */
   int                 e;
#  endif /* _WINDOWS */

   /* reset the cursor coordinate in the frame */
   ym += FrameTable[frame].FrTopMargin;

   /* Slight shift for drawing */
   if (xr == *x)
      xr += 2;
   else if (xr == *x + width)
      xr -= 2;

   if (yr == *y)
      yr += 2;
   else if (yr == *y + height)
      yr -= 2;

   /* Pick the correct cursor */
   w = FrRef[frame];
#  ifdef _WINDOWS
  if (!GetWindowRect (w, &rect))
     WinErrorBox (w, TEXT("UserGeometryMove (1)"));

  ret = 0;
  while (ret == 0) {
        GetMessage (&event, NULL, 0, 0);

        switch (event.message) {
               case WM_LBUTTONUP:
                    ret = 1;
                    break;

               case WM_MOUSEMOVE:
                    SetCursor (LoadCursor (NULL, IDC_CROSS));
                    /* ShowCursor (TRUE); */
                    GetCursorPos (&cursorPos);
                    ScreenToClient (w, &cursorPos);
                    /* compute the new box origin */
                    nx = *x + cursorPos.x - xm;
                    dx = xmin + DO_ALIGN (nx - xmin) - *x;
                    ny = *y + cursorPos.y - ym;
                    dy = ymin + DO_ALIGN (ny - ymin) - *y;
                    nx = dx + *x;
                    ny = dy + *y;
                    /* Checks for limits */
                    warpx = -1;
                    warpy = -1;
                    if (xmin == xmax) {
                       nx = xmin; /*left side */
                       warpx = xm;
					} else if (nx < xmin) {
                           nx = xmin; /*left side */
                           warpx = xm;
					} else if (nx + width > xmax) {
                           if (xmin + width > xmax) {
                              nx = xmin; /*overflow on left side */
                              warpx = xm;
						   } else {
                                  nx = xmin + DO_ALIGN (xmax - width - xmin); /*cote droit */
                                  warpx = xm + nx - *x;
						   }
					} else
                           xm += dx; /* New cursor location */

                    dx = nx - *x;
                    if (ymin == ymax) {
                       ny = ymin; /*upper border */
                       warpy = ym;
					} else if (ny < ymin) {
                           ny = ymin; /*upper border */
                           warpy = ym;
					} else if (ny + height > ymax) {
                           if (ymin + height > ymax) {
                              ny = ymin; /*overflow on upper border */
                              warpy = ym;
						   } else {
                                  ny = ymin + DO_ALIGN (ymax - height - ymin); /*cote inferieur */
                                  warpy = ym + ny - *y;
						   }
					} else
                           ym += dy; /* New cursor location */
                    dy = ny - *y;

                    /* Should we move the box */
                    if ((dx != 0) || (dy != 0)) {
                       BoxGeometry (frame, *x, *y, width, height, xr, yr, FALSE); /*Ancienne */
                       xr += dx;
                       yr += dy;
                       BoxGeometry (frame, nx, ny, width, height, xr, yr, FALSE); /*Nouvelle */
                       *x = nx;
                       *y = ny;
					}

                    /* Should we move the cursor */
                    if (warpx >= 0 || warpy >= 0) {
                       if (warpx >= 0)
                          xm = warpx;
                       if (warpy >= 0)
                          ym = warpy;
					}
		            break;
                default: break;
		 }
   }
   /* Erase the box drawing */
   BoxGeometry (frame, *x, *y, width, height, xr, yr, FALSE);
#  else  /* !_WINDOWS */
   e = ButtonPressMask | ButtonReleaseMask | ButtonMotionMask;
   if ((xmin >= *x) && (xmax <= *x + width))
      ThotGrab (w, VCurs, e, 0);
   else
     {
	if ((ymin >= *y) && (ymax <= *y + height))
	   ThotGrab (w, HCurs, e, 0);
	else
	   ThotGrab (w, HVCurs, e, 0);
     }

   /* Shows the initial box size */
   BoxGeometry (frame, *x, *y, width, height, xr, yr, FALSE);

   /* Loop on user interraction */
   ret = 0;
   while (ret == 0)
     {
	XNextEvent (TtDisplay, &event);
	switch (event.type)
	      {
		 case ButtonRelease:
		    ret = 1;
		 case MotionNotify:

		    /* Check for window ID */
		    if (event.xmotion.window == w)
		      {
			 /* compute the new box origin */
			 nx = *x + (int) event.xmotion.x - xm;
			 dx = xmin + DO_ALIGN (nx - xmin) - *x;
			 ny = *y + (int) event.xmotion.y - ym;
			 dy = ymin + DO_ALIGN (ny - ymin) - *y;
		      }
		    else
		      {
			 dx = dy = 0;
		      }

		    nx = dx + *x;
		    ny = dy + *y;

		    /* Checks for limits */
		    warpx = -1;
		    warpy = -1;
		    if (xmin == xmax)
		      {
			 nx = xmin;	/*left side */
			 warpx = xm;
		      }
		    else if (nx < xmin)
		      {
			 nx = xmin;	/*left side */
			 warpx = xm;
		      }
		    else if (nx + width > xmax)
		      {
			 if (xmin + width > xmax)
			   {
			      nx = xmin;	/*overflow on left side */
			      warpx = xm;
			   }
			 else
			   {
			      nx = xmin + DO_ALIGN (xmax - width - xmin);		/*cote droit */
			      warpx = xm + nx - *x;
			   }
		      }
		    else
		       xm += dx;	/* New cursor location */

		    dx = nx - *x;
		    if (ymin == ymax)
		      {
			 ny = ymin;	/*upper border */
			 warpy = ym;
		      }
		    else if (ny < ymin)
		      {
			 ny = ymin;	/*upper border */
			 warpy = ym;
		      }
		    else if (ny + height > ymax)
		      {
			 if (ymin + height > ymax)
			   {
			      ny = ymin;	/*overflow on upper border */
			      warpy = ym;
			   }
			 else
			   {
			      ny = ymin + DO_ALIGN (ymax - height - ymin);	/*cote inferieur */
			      warpy = ym + ny - *y;
			   }
		      }
		    else
		       ym += dy;	/* New cursor location */
		    dy = ny - *y;

		    /* Should we move the box */
		    if ((dx != 0) || (dy != 0))
		      {
			 BoxGeometry (frame, *x, *y, width, height, xr, yr, FALSE);	/*Ancienne */
			 xr += dx;
			 yr += dy;
			 BoxGeometry (frame, nx, ny, width, height, xr, yr, FALSE);	/*Nouvelle */
			 XFlush (TtDisplay);
			 *x = nx;
			 *y = ny;

		      }

		    /* Should we move the cursor */
		    if (warpx >= 0 || warpy >= 0)
		      {
			 if (warpx >= 0)
			    xm = warpx;
			 if (warpy >= 0)
			    ym = warpy;
			 XWarpPointer (TtDisplay, None, w, 0, 0, 0, 0, xm, ym);
		      }
		    break;
		 default:
		    break;
	      }
     }

   /* Erase the box drawing */
   BoxGeometry (frame, *x, *y, width, height, xr, yr, FALSE);

   /* restore the Thot Library state */
   ThotUngrab ();
   XFlush (TtDisplay);
#  endif /* _WINDOWS */
}

/*----------------------------------------------------------------------
  UserGeometryResize draw a box at a specific (x,y) location in
  frame and of size width x height when interracting with
  the user to mofify the box geometry (button press).
  xr and yr gives the initial mouse reference point.
  xmin, xmax, ymin, ymax are the maximum values allowed.
  xm and ym gives the initial mouse coordinates in the frame.
  Parameters percentW and percentH will be equal to zero if width
  and height are independent or will get the percent report (percentH = 100
  if height is equal to width).
  this function returns the new geometry upon button release.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                UserGeometryResize (int frame, int x, int y, int *width, int *height, int xr, int yr, int xmin, int xmax, int ymin, int ymax, int xm, int ym, int percentW, int percentH)
#else  /* __STDC__ */
void                UserGeometryResize (frame, x, y, width, height, xr, yr, xmin, xmax, ymin, ymax, xm, ym, percentW, percentH)
int                 frame;
int                 x;
int                 y;
int                *width;
int                *height;
int                 xr;
int                 yr;
int                 xmin;
int                 xmax;
int                 ymin;
int                 ymax;
int                 xm;
int                 ym;
int                 percentW;
int                 percentH;
#endif /* __STDC__ */
{
#define C_TOP 0
#define C_HCENTER 1
#define C_BOTTOM 2
#define C_LEFT 0
#define C_VCENTER 1
#define C_RIGHT 2

  int                 ret, dx, dy, dl, dh;
  int                 ref_h, ref_v, HorizontalDirection, VerticalDirection;
  ThotEvent           event;
  ThotWindow          w;
  int                 warpx, warpy;
# ifdef _WINDOWS
  RECT                rect;
  POINT               cursorPos;
# else  /* _WINDOWS */
  int e;
# endif /* _WINDOWS */

  /* Use the reference point to move the box */
  if (xr == x)
    {
      ref_v = C_LEFT;
      xr += 2;		/* small shift for drawing */
      /* Box increased when X delta increase */
      HorizontalDirection = 1;
    }
  else if (xr == x + *width)
    {
      ref_v = C_RIGHT;
      xr -= 2;		/* small shift for drawing */
      /* Box shortened when X delta increase */
      HorizontalDirection = -1;
    }
  else
    {
      ref_v = C_VCENTER;
      /* Direction depends of original cursor location in the frame */
      if (xm < xr)
	HorizontalDirection = -1;
      else
	HorizontalDirection = 1;
    }

  if (yr == y)
    {
      ref_h = C_TOP;
      yr += 2;		/* small shift for drawing */
      /* Box increased when Y delta increase */
      VerticalDirection = 1;
    }
  else if (yr == y + *height)
    {
      ref_h = C_BOTTOM;
      yr -= 2;		/* small shift for drawing */
      /* Box shortened when Y delta increase */
      VerticalDirection = -1;
    }
  else
    {
      ref_h = C_HCENTER;
      /* Direction depends of original cursor location in the frame */
      if (ym < yr)
	VerticalDirection = -1;
      else
	VerticalDirection = 1;
    }

  /* reset the cursor coordinate in the frame */
  ym += FrameTable[frame].FrTopMargin;

  /* Shows the initial box size */
  BoxGeometry (frame, x, y, *width, *height, xr, yr, FALSE);

  /* select the correct cursor */
  w = FrRef[frame];

# ifdef _WINDOWS
  if (!GetWindowRect (w, &rect))
     WinErrorBox (w, TEXT("UserGeometryResize (1)"));

  ret = 0;
  while (ret == 0) {
        GetMessage (&event, NULL, 0, 0);

        switch (event.message) {
               case WM_MBUTTONUP:
               case WM_RBUTTONUP:
                    ret = 1;
                    break;

               case WM_MOUSEMOVE:
                    SetCursor (LoadCursor (NULL, IDC_CROSS));
                    /* ShowCursor (TRUE); */
                    GetCursorPos (&cursorPos);
                    ScreenToClient (w, &cursorPos);
                    dl = cursorPos.x - xm;
                    dh = cursorPos.y - ym;
                    /* Check that size can be modified, and stay >= 0    */
                    /* depending on base point and cursor position,      */
                    /* increase or decreas width or height accordingly   */
                    warpx = -1;
                    warpy = -1;
                    if (dl != 0)
                       if (xmin == xmax) /* X moves frozen */
                          dl = 0;
                      else if (ref_v == C_VCENTER && *width + (2 * dl * HorizontalDirection) < 0) {
                           dl = -DO_ALIGN (*width / 2) * HorizontalDirection;
                           warpx = xm + (dl * HorizontalDirection);
					  } else if (*width + (dl * HorizontalDirection) < 0) {
                             dl = -DO_ALIGN (*width) * HorizontalDirection;
                             warpx = xm + (dl * HorizontalDirection);
					  }

                    if (dh != 0)
                       if (ymin == ymax) /* Y moves frozen */
                          dh = 0;
                       else if (ref_h == C_HCENTER && *height + (2 * dh * VerticalDirection) < 0) {
                            dh = -(*height * VerticalDirection * GridSize) / (2 * GridSize);
                            warpy = ym + (dh * VerticalDirection);
					   } else if (*height + dh < 0) {
                              dh = -(*height * VerticalDirection * GridSize) / GridSize;
                              warpy = ym + (dh * VerticalDirection);
					   }

                         /* Compute the horizontal move of the origin */
                    if (dl != 0) {
                       dl = dl * HorizontalDirection; /* Take care for direction */
                       if (ref_v == C_VCENTER) {
                          dx = xmin + DO_ALIGN (x - (dl / 2) - xmin) - x;
                          /* Check the move is within limits */
                          if (x + dx < xmin)
                             dx = xmin - x; /*left side */
                          if (x + *width - dx > xmax)
                             dx = x + *width - xmin - DO_ALIGN (xmax - xmin); /*right side */
		  
                          /* modify width for real */
                          dl = -(dx * 2);
                          if (dx != 0)
                              warpx = xm - (dx * HorizontalDirection);
					   } else if (ref_v == C_RIGHT) {
                              dx = xmin + DO_ALIGN (x - dl - xmin) - x;
                              /* Check the move is within limits */
                              if (x + dx < xmin)
                                 dx = xmin - x; /*left side */

                              /* modify width for real */
                              dl = -dx;
                              if (dx != 0)
                                 warpx = xm + dx;
					   } else {
                              dx = 0;
                              dl = xmin + DO_ALIGN (x + *width + dl - xmin) - x - *width;
                              if (x + *width + dl > xmax)
                                 dl = xmin + DO_ALIGN (xmax - xmin) - x - *width; /*right side */
                              if (dl != 0)
                                 warpx = xm + dl;
					   }
					} else
                         dx = 0;

                    /* Compute vertical move */
                    if (dh != 0) {
                       dh = dh * VerticalDirection;	/* Take care for direction */
                       if (ref_h == C_HCENTER) {
                          dy = ymin + DO_ALIGN (y - (dh / 2) - ymin) - y;
                          /* Check the move is within limits */
                          if (y + dy < ymin)
                             dy = ymin - y; /*upper border */
                          if (y + *height - dy > ymax)
                             dy = y + *height - ymin - DO_ALIGN (ymax - ymin);	/*bottom */
                          /* change the height for real */
                          dh = -(dy * 2);
                          if (dy != 0)
                             warpy = ym - (dy * VerticalDirection);
					   } else if (ref_h == C_BOTTOM) {
                              dy = ymin + DO_ALIGN (y - dh - ymin) - y;
                              /* Check the move is within limits */
                              if (y + dy < ymin)
                                 dy = ymin - y; /*upper border */
                              /* change the height for real */
                              dh = -dy;
                              if (dy != 0)
                                 warpy = ym + dy;
					   } else {
                              dy = 0;
                              dh = ymin + DO_ALIGN (y + *height + dh - ymin) - y - *height;
                              if (y + *height + dh > ymax)
                                 dh = ymin + DO_ALIGN (ymax - ymin) - y - *height; /*bottom */
                              if (dh != 0)
                                 warpy = ym + dh;
					   }
					} else
                         dy = 0;
                    /* Should we move the box */
                    if ((dl != 0) || (dh != 0)) {
                       /* switch off the old box */
                       BoxGeometry (frame, x, y, *width, *height, xr, yr, FALSE);
                       /* is there any dependence between height and width */
                       *width += dl;
                       *height += dh;
                       if (percentW != 0)
                          *width = *height * percentW / 100;
                       else if (percentH != 0)
                            *height = *width * percentH / 100;
                       x += dx;
                       y += dy;
                       /* switch on the new one */
                       BoxGeometry (frame, x, y, *width, *height, xr, yr, FALSE);
					}

                    /* Should we move the cursor */
                    if (warpx >= 0 || warpy >= 0) {
                       if (warpx >= 0)
                          xm = warpx;
                       if (warpy >= 0)
                          ym = warpy;

                       /* if (!SetCursorPos (xm, ym))
                          WinErrorBox (FrRef [frame]); */
					}
                    break;
            default:  break;
        } 
  }

  /* Erase the box drawing */
  SetCursor (LoadCursor (NULL, IDC_ARROW));
  /* ShowCursor (TRUE); */
  BoxGeometry (frame, x, y, *width, *height, xr, yr, FALSE);
# else  /* _WINDOWS */
  e = ButtonPressMask | ButtonReleaseMask | ButtonMotionMask;
  if (xmin == xmax)
    ThotGrab (w, VCurs, e, 0);
  else if (ymin == ymax)
    ThotGrab (w, HCurs, e, 0);
  else
    ThotGrab (w, HVCurs, e, 0);

  /* Loop on user interraction */
  ret = 0;
  while (ret == 0)
    {
      /* X11R4 bug fix, if events are used too rapidly sometimes    */
      /* an event with event.xbutton.y=1 arose. Solution, pick only */
      /* MotionNotify events from the queue !                       */
      XNextEvent (TtDisplay, &event);
      if (event.type == MotionNotify)	
	/* On pique le dernier evenement de la file */
	while (XPending (TtDisplay))
	  XNextEvent (TtDisplay, &event);

      switch (event.type)
	{
	case ButtonRelease:
	  ret = 1;
	case MotionNotify:
	  if (event.xbutton.window == w)
	    {
	      /* compute the deltas */
	      dl = (int) event.xmotion.x - xm;
	      dh = (int) event.xmotion.y - ym;
	    }
	  else
	    dl = dh = 0;

	  /* Check that size can be modified, and stay >= 0    */
	  /* depending on base point and cursor position,      */
	  /* increase or decreas width or height accordingly   */
	  warpx = -1;
	  warpy = -1;
	  if (dl != 0)
	    if (xmin == xmax)
	      /* X moves frozen */
	      dl = 0;
	    else if (ref_v == C_VCENTER
		     && *width + (2 * dl * HorizontalDirection) < 0)
	      {
		dl = -DO_ALIGN (*width / 2) * HorizontalDirection;
		warpx = xm + (dl * HorizontalDirection);
	      }
	    else if (*width + (dl * HorizontalDirection) < 0)
	      {
		dl = -DO_ALIGN (*width) * HorizontalDirection;
		warpx = xm + (dl * HorizontalDirection);
	      }

	  if (dh != 0)
	    if (ymin == ymax)
	      /* Y moves frozen */
	      dh = 0;
	    else if (ref_h == C_HCENTER
		     && *height + (2 * dh * VerticalDirection) < 0)
	      {
		dh = -(*height * VerticalDirection * GridSize) / (2 * GridSize);
		warpy = ym + (dh * VerticalDirection);
	      }
	    else if (*height + dh < 0)
	      {
		dh = -(*height * VerticalDirection * GridSize) / GridSize;
		warpy = ym + (dh * VerticalDirection);
	      }

	  /* Compute the horizontal move of the origin */
	  if (dl != 0)
	    {
	      dl = dl * HorizontalDirection;		/* Take care for direction */
	      if (ref_v == C_VCENTER)
		{
		  dx = xmin + DO_ALIGN (x - (dl / 2) - xmin) - x;
		  /* Check the move is within limits */
		  if (x + dx < xmin)
		    dx = xmin - x;		/*left side */
		  if (x + *width - dx > xmax)
		    dx = x + *width - xmin - DO_ALIGN (xmax - xmin); /*right side */
		  
		  /* modify width for real */
		  dl = -(dx * 2);
		  if (dx != 0)
		    warpx = xm - (dx * HorizontalDirection);
		}
	      else if (ref_v == C_RIGHT)
		{
		  dx = xmin + DO_ALIGN (x - dl - xmin) - x;
		  /* Check the move is within limits */
		  if (x + dx < xmin)
		    dx = xmin - x;		/*left side */

		  /* modify width for real */
		  dl = -dx;
		  if (dx != 0)
		    warpx = xm + dx;
		}
	      else
		{
		  dx = 0;
		  dl = xmin + DO_ALIGN (x + *width + dl - xmin) - x - *width;
		  if (x + *width + dl > xmax)
		    dl = xmin + DO_ALIGN (xmax - xmin) - x - *width; /*right side */
		  if (dl != 0)
		    warpx = xm + dl;
		}
	    }
	  else
	    dx = 0;

	  /* Compute vertical move */
	  if (dh != 0)
	    {
	      dh = dh * VerticalDirection;	/* Take care for direction */
	      if (ref_h == C_HCENTER)
		{
		  dy = ymin + DO_ALIGN (y - (dh / 2) - ymin) - y;
		  /* Check the move is within limits */
		  if (y + dy < ymin)
		    dy = ymin - y;		/*upper border */
		  if (y + *height - dy > ymax)
		    dy = y + *height - ymin - DO_ALIGN (ymax - ymin);	/*bottom */
		  /* change the height for real */
		  dh = -(dy * 2);
		  if (dy != 0)
		    warpy = ym - (dy * VerticalDirection);
		}
	      else if (ref_h == C_BOTTOM)
		{
		  dy = ymin + DO_ALIGN (y - dh - ymin) - y;
		  /* Check the move is within limits */
		  if (y + dy < ymin)
		    dy = ymin - y;		/*upper border */
		  /* change the height for real */
		  dh = -dy;
		  if (dy != 0)
		    warpy = ym + dy;
		}
	      else
		{
		  dy = 0;
		  dh = ymin + DO_ALIGN (y + *height + dh - ymin) - y - *height;
		  if (y + *height + dh > ymax)
		    dh = ymin + DO_ALIGN (ymax - ymin) - y - *height;	/*bottom */
		  if (dh != 0)
		    warpy = ym + dh;
		}
	    }
	  else
	    dy = 0;

	  /* Should we move the box */
	  if ((dl != 0) || (dh != 0))
	    {
	      /* switch off the old box */
	      BoxGeometry (frame, x, y, *width, *height, xr, yr, FALSE);
	      /* is there any dependence between height and width */
	      *width += dl;
	      *height += dh;
	      if (percentW != 0)
		*width = *height * percentW / 100;
	      else if (percentH != 0)
		*height = *width * percentH / 100;
	      x += dx;
	      y += dy;
	      /* switch on the new one */
	      BoxGeometry (frame, x, y, *width, *height, xr, yr, FALSE);
	    }

	  /* Should we move the cursor */
	  if (warpx >= 0 || warpy >= 0)
	    {
	      if (warpx >= 0)
		xm = warpx;
	      if (warpy >= 0)
		ym = warpy;
	      XWarpPointer (TtDisplay, None, w, 0, 0, 0, 0, xm, ym);
	    }
	  break;
	default:
	  break;
	}
    }

  /* Erase the box drawing */
  BoxGeometry (frame, x, y, *width, *height, xr, yr, FALSE);

  /* restore the previous state of the Thot Library */
  ThotUngrab ();
  XFlush (TtDisplay);
#endif /* _WINDOWS */
}
