/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2003
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
#include "boxes_tv.h"
#include "frame_tv.h"

#include "frame_f.h"
#include "presentationapi_f.h"
#include "structcreation_f.h"
#include "boxlocate_f.h"

static int          GridSize = 1;
#define DO_ALIGN(val) ((val + (GridSize/2)) / GridSize) * GridSize

#include "appli_f.h"
#include "buildboxes_f.h"
#include "content_f.h"
#include "font_f.h"
#include "memory_f.h"
#include "units_f.h"
#include "windowdisplay_f.h"

#ifdef _GL
  #include <GL/gl.h>
  #include "glwindowdisplay.h"
#endif /*_GL*/

#ifdef _GTK
  #include <gdk/gdkx.h>
  #include <gtk/gtkprivate.h>
#endif /*_GTK*/

#ifdef _WINDOWS
#include "wininclude.h"

/* working display for geom.c */
static HDC     Gdc = NULL;
extern int     X_Pos;
extern int     Y_Pos;

/*----------------------------------------------------------------------
  DrawOutline :                                                    
  ----------------------------------------------------------------------*/
static void DrawOutline (HWND hwnd, POINT ptBeg, POINT ptEnd)
{
#ifndef _GL
  HDC hdc;
#endif /*_GL*/
  ThotPoint ptTab [2];

  ptTab[0].x = ptBeg.x;
  ptTab[0].y = ptBeg.y;
  ptTab[1].x = ptEnd.x;
  ptTab[1].y = ptEnd.y;

#ifdef _GL
  
  GL_DrawPolygon (ptTab, 2);

#else /*_GL*/

  hdc = GetDC (hwnd);
  SetROP2 (hdc, R2_NOT);
  SelectObject (hdc, GetStockObject (NULL_BRUSH));
  /* SelectObject (hdc, GetStockObject (BLACK_PEN)); */
  Polyline (hdc, ptTab, 2);
  DeleteDC (hdc);

#endif /*_GL*/
}

/*----------------------------------------------------------------------
  DrawOutpolygon :                                                    
  ----------------------------------------------------------------------*/
void DrawOutpolygon (HWND hwnd, POINT* pt, int nb)
{
  HDC hdc;

  hdc = GetDC (hwnd);
  SetROP2 (hdc, R2_NOT);
  SelectObject (hdc, GetStockObject (NULL_BRUSH));
  /* SelectObject (hdc, GetStockObject (BLACK_PEN)); */
  Polyline (hdc, pt, nb);  
  DeleteDC (hdc);
}
#endif /* _WINDOWS */

/*----------------------------------------------------------------------
  VideoInvert switch to inverse video the area of frame located at
  (x,y) and of size width x height.
  ----------------------------------------------------------------------*/
static void VideoInvert (int frame, int width, int height, int x, int y)
{
  ThotWindow          w;

  w = FrRef[frame];
  if (w != None)
    {
#ifdef _GL
      GL_DrawEmptyRectangle (152, x, y, width, height, 0);
#else /*_GL*/
      
#ifdef _WINDOWS
      PatBlt (Gdc, x, y, width, height, PATINVERT);
#endif /* _WINDOWS */
      
#ifdef _GTK
      gdk_draw_rectangle (w, TtInvertGC, TRUE, x, y, width, height);
#endif /* _GTK */

#ifdef _MOTIF     
      XFillRectangle (TtDisplay, w, TtInvertGC, x, y, width, height);
#endif /* _MOTIF */
      
#endif /*_GL*/
    }
}

/*----------------------------------------------------------------------
  BoxGeometry set the geometry of the fake box used to interract with
  the user.
  ----------------------------------------------------------------------*/
static void BoxGeometry (int frame, int x, int y, int width, int height,
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
static void InvertEllipse (int frame, int x, int y, int width, int height,
			   int xr, int yr)
{
  ThotWindow      w;
#ifdef _WINDOWS
#ifndef _GL
  HRGN            rgn;
#endif /*_GL*/
#endif /* _WINDOWS */

  w = FrRef[frame];
  if (w != None)
    {
#ifdef _GL
	  GL_DrawArc (x, y, width, height, 0, 360 * 64, FALSE);
#else /*_GL*/
#ifdef _WINDOWS
	  rgn = CreateEllipticRgn (x, y, x + width, y + height);
	  if (rgn)
	  {
	    InvertRgn (Gdc, rgn);
	    DeleteObject (rgn);
	  }
	  rgn = CreateEllipticRgn (x + 1, y + 1, x + width - 2, y + height - 2);
	  if (rgn)
	  {
	    InvertRgn (Gdc, rgn);
	    DeleteObject (rgn);
	  }
#endif /* _WINDOWS */
    
#ifdef _GTK
      gdk_draw_arc (w, TtInvertGC, FALSE, x, y, width, height, 0, 360 * 64);
#endif /* _GTK */

#ifdef _MOTIF      
      XDrawArc (TtDisplay, w, TtInvertGC, x, y, width, height, 0, 360 * 64);
#endif /* _MOTIF */

#endif /*_GL*/
    }
}

/*----------------------------------------------------------------------
  RedrawPolyLine shows the current state of the polyline (closed or
  not) in a given frame.
  This function returns the coordinates of the following control points :
  - x1, y1 predecessor of point
  - x2, y2 point
  - x3, y3 successor of point
  ----------------------------------------------------------------------*/
static void RedrawPolyLine (int frame, int x, int y, PtrTextBuffer buffer,
			    int nb, int point, ThotBool close, int *x1,
			    int *y1, int *x2, int *y2, int *x3, int *y3)
{
  ThotWindow          w;
  ThotPoint          *points;
  int                 i, j;
  PtrTextBuffer       adbuff;

  w = FrRef[frame];
  *x1 = *y1 = *x2 = *y2 = *x3 = *y3 = -1;
  /* allocate a table of points */
  points = (ThotPoint *) TtaGetMemory (sizeof (ThotPoint) * nb);
  adbuff = buffer;
  j = 1;
  for (i = 0; i < nb - 1; i++)
    {
      if (j >= adbuff->BuLength &&
	  adbuff->BuNext != NULL)
	{
	  /* change the buffer */
	  adbuff = adbuff->BuNext;
	  j = 0;
	}
      points[i].x = x + PixelValue (adbuff->BuPoints[j].XCoord, UnPixel, NULL,
				    ViewFrameTable[frame - 1].FrMagnification);
      points[i].y = y + PixelValue (adbuff->BuPoints[j].YCoord, UnPixel, NULL,
				    ViewFrameTable[frame - 1].FrMagnification);
      /* write down predecessor and sucessor of point */
      if (i + 1 == point)
	{
	  /* selected point */
	  *x2 = points[i].x;
	  *y2 = points[i].y;
	}
      else
	{
	  if (i + 1 == point - 1)
	    {
	      /* predecessor */
	      *x1 = points[i].x;
	      *y1 = points[i].y;
	    }
	  else if (i == point)
	    {
	      /* succesor */
	      *x3 = points[i].x;
	      *y3 = points[i].y;
	    }
	}
      j++;
    }

  /* Draw the border */
  if (close && nb > 3)
    {
      /* This is a closed polyline with more than 2 points */
      points[nb - 1].x = points[0].x;
      points[nb - 1].y = points[0].y;
     if (point == 1)
       {
	 *x1 = points[nb - 2].x;
	 *y1 = points[nb - 2].y;
       }
     if (point == nb - 1)
       {
	 *x3 = points[0].x;
	 *y3 = points[0].y;
       }

#ifdef _GL

     /* GL_DrawPolygon (points, nb); */

#else /*_GL*/

#ifdef _WINDOWS
     DrawOutpolygon (w, points, nb);
#endif  /* !_WINDOWS */

#ifdef _MOTIF
     XDrawLines (TtDisplay, w, TtInvertGC, points, nb, CoordModeOrigin);
#endif /* _MOTIF */
     
#ifdef _GTK
     gdk_draw_polygon (FrRef[frame], TtInvertGC, FALSE, points, nb);
#endif /* _GTK */
     
#endif /*_GL*/
    }
  else 
#ifdef _GL
    /* GL_DrawPolygon (points, nb - 1); */
#else /*_GL*/
    
#ifdef _WINDOWS 
  DrawOutpolygon (w, points, nb - 1);
#endif /* _WINDOWS */
  
#ifdef _MOTIF
  XDrawLines (TtDisplay, w, TtInvertGC, points, nb - 1, CoordModeOrigin);
#endif /* _MOTIF */
  
#ifdef _GTK
  gdk_draw_lines(FrRef[frame], TtInvertGC, points, nb - 1);
#endif /* !_GTK */

#endif /*_GL*/
  /* free the table of points */
  free ((STRING) points);
}


/*----------------------------------------------------------------------
  AddPoints adds a set of new points in a Polyline
   - x, y and lastx, lasty points to the current point
   - the first inserted point will be linked to x1, y1 when x1!= -1
   - the last inserted point will be linked to x3, y3 when x3!= -1
   - point gives the index of the first inserted point
   - maxPoints gives the constraint when the number of points is fixed
     (0 if there is no constraint).
   - width and height give the limits of the box
   - Pbuffer and Bbuffer point to the current Abstract Box buffer and
     Box buffer.
  ----------------------------------------------------------------------*/
static void AddPoints (int frame, int x, int y, int x1, int y1, int x3,
		       int y3, int lastx, int lasty, int point,
		       int *nbpoints, int maxPoints, int width, int height,
		       PtrTextBuffer Pbuffer, PtrTextBuffer Bbuffer)
{
#if defined(_MOTIF) || defined(_WINDOWS) || defined(_GTK)
  
  ThotWindow          w;
#if defined(_MOTIF) || defined(_WINDOWS)
   ThotEvent           event;
#endif /* #if defined(_MOTIF) || defined(_WINDOWS) */
   
#ifdef _GTK
   ThotEvent           *event;
   GdkWindowPrivate    *xwindow;
#endif /*_GTK*/

   float               ratioX, ratioY;
  int                 ret;
  int                 newx, newy;
  ThotBool            input;
  ThotBool            wrap;

#ifdef _WINDOWS
  RECT                rect;
  POINT               cursorPos;
  POINT               ptBeg;
  POINT               ptEnd;
  POINT               ptCur;
  HCURSOR             cross;
#endif /* _WINDOWS */
  
#if defined(_MOTIF) || defined(_GTK)  
  int                 e, f;
#endif /* #if defined(_MOTIF) || defined(_GTK) */

  /* need the window to change the cursor */
  w = FrRef[frame];
  /* trasformation factor between the box and the abstract box */
  ratioX = (float) Pbuffer->BuPoints[0].XCoord / (float) Bbuffer->BuPoints[0].XCoord;
  /* trasformation factor between the box and the abstract box */
  ratioY = (float) Pbuffer->BuPoints[0].YCoord / (float) Bbuffer->BuPoints[0].YCoord;

#ifdef _WINDOWS
  GetWindowRect (w, &rect);
  /* The grid stepping begins at the origin */
  ptBeg.x = x1;
  ptBeg.y = y1;
  ptEnd.x = x3;
  ptEnd.y = y3;
  if (!SetCursorPos (lastx + rect.left, lasty + rect.top))
    WinErrorBox (w, "AddPoints (1)");
  cross = LoadCursor (NULL, IDC_CROSS);
#endif /* !_WINDOWS */

#ifdef _MOTIF
  e = PointerMotionMask | ButtonPressMask | ButtonReleaseMask | ButtonMotionMask;
  XMapRaised (TtDisplay, w);
  XFlush (TtDisplay);
  ThotGrab (w, HVCurs, e, 0);
  XWarpPointer (TtDisplay, None, w, 0, 0, 0, 0, lastx, lasty);
  XFlush (TtDisplay);
#endif /* _MOTIF */

#ifdef _GTK
  gdk_window_set_cursor (GTK_WIDGET(FrameTable[frame].WdFrame)->window, HVCurs);
  e = GDK_BUTTON_MOTION_MASK | GDK_POINTER_MOTION_MASK 
    | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK;
  ThotGrab (w, HVCurs, e, 0);
  xwindow = (GdkWindowPrivate*) w;
  XWarpPointer (GDK_DISPLAY(), 
		None, 
		xwindow->xwindow,
		0, 0, 0, 0, lastx, lasty);
#endif /* !_GTK */

  /* shows up limit borders */
  /*BoxGeometry (frame, x, y, width, height, x + width - 2, y + height - 2);*/
  /* loop waiting for the user input */
  ret = 0;
  /* take into account release button events that follow a press button event */
  input = FALSE;
  wrap = FALSE;
  while (ret == 0)
    {

#ifdef _WINDOWS
      GetMessage (&event, NULL, 0, 0);
	  SetCursor (cross);
      if (event.message == WM_MOUSEMOVE)
	{
	  GetCursorPos (&cursorPos);
	  /* current pointer position */
	  /* coordinate checking */
	  newx = DO_ALIGN (cursorPos.x -rect.left - x);
	  newx += x;
	  newy = DO_ALIGN (cursorPos.y -rect.top - y);
	  newy += y;
	  if (newx < x || newx > x + width ||
	      newy < y || newy > y + height)
	    {
	      /* CHKR_LIMIT to size of the box */
	      /* new X valid position */
	      if (newx < x)
		newx = x;
	      else if (newx > x + width)
		newx = x + width;
	      
	      /* new Y valid position */
	      if (newy < y)
		newy = y;
	      else if (newy > y + height)
		newy = y + height;
	  
	      if (!SetCursorPos (newx + rect.left, newy + rect.top))
		WinErrorBox (w, "AddPoints (2)");
	    }

         /* refresh the display of teh two adjacent segments */
	  if (newx != lastx || newy != lasty)
	    {
	      if (x1 != -1)
		{
		  /* remove previous line */
		  ptCur.x = lastx;
		  ptCur.y = lasty;
		  DrawOutline (w, ptBeg, ptCur);
		  /* add a new line */
		  ptCur.x = newx;
		  ptCur.y = newy;
		  DrawOutline (w, ptBeg, ptCur);
		}
	      if (x3 != -1)
		{
		  /* remove previous line */
		  ptCur.x = lastx;
		  ptCur.y = lasty;
		  DrawOutline (w, ptCur, ptEnd);
		  /* add a new line */
		  ptCur.x = newx;
		  ptCur.y = newy;
		  DrawOutline (w, ptCur, ptEnd);
		}	     
	   }
         lastx = newx;
         lasty = newy;
	}
      else
	{
	  /* coordinate checking */
	  newx = x + DO_ALIGN ((int) cursorPos.x - rect.left - x);
	  newy = y + DO_ALIGN ((int) cursorPos.y -rect.top - y);
	  /* CHKR_LIMIT to size of the box */
	  /* new X valid position */
	  if (newx < x)
	    {
	      lastx = x;
	      wrap = TRUE;
	    }
	  else if (newx > x + width)
	    {
	      lastx = x + width;
	      wrap = TRUE;
	    }
	  else
	    lastx = newx;
	
	  /* new Y valid position */
	  if (newy < y)
	    {
	      lasty = y;
	      wrap = TRUE;
	    }
	  else if (newy > y + height)
	    {
	      lasty = y + height;
	      wrap = TRUE;
	    }
	  else
	    lasty = newy;
	  
	  switch (event.message)
	    {
	    case WM_ENTER:
            case WM_SYSKEYDOWN:
	    case WM_KEYDOWN:
	      /* stop the creation of the polyline */
	      ret = 1;
	      break;

	    case WM_LBUTTONDOWN:
	    case WM_MBUTTONDOWN:
	    case WM_RBUTTONDOWN:
	      /* it's a press button event */
	      input = TRUE;
	      if (wrap)
		{
		  /* align the cursor position */
		  if (!SetCursorPos (lastx + rect.left, lasty + rect.top))
		    WinErrorBox (w, "AddPoints (3)");
		  wrap = FALSE;		  
		}
	      break;
	      
	    case WM_LBUTTONUP:
	    case WM_MBUTTONUP:
	    case WM_RBUTTONUP:
	      if (input)
		{
		  input = FALSE;
		  /* left button keep the last segment built */
		  /* keep the new segment first point coordinates */
		  x1 = lastx;
		  y1 = lasty;
		  ptBeg.x = x1;
		  ptBeg.y = y1;
		  (*nbpoints)++;
		  point++;

		  /* update the box buffer */
		  newx = LogicalValue (lastx - x, UnPixel, NULL,
				       ViewFrameTable[frame - 1].FrMagnification);
		  newy = LogicalValue (lasty - y, UnPixel, NULL,
				       ViewFrameTable[frame - 1].FrMagnification);
		  AddPointInPolyline (Bbuffer, point, newx, newy);
		  /* update the abstract box buffer */
		  newx = (int) ((float) newx * ratioX);
		  newy = (int) ((float) newy * ratioY);
		  AddPointInPolyline (Pbuffer, point, newx, newy);
		  if (*nbpoints > maxPoints && maxPoints != 0)
		    /* we have the right number of points */
		    ret = 1;
		  else if (maxPoints == 0 && event.message != WM_LBUTTONUP)
		    /* any other button : end of user input */
		    ret = 1;
		}
	      else
		{
		  newx = lastx;
		  newy = lasty;
		}
	      break;

		case WM_PAINT:
		  WIN_HandleExpose (w, frame, event.wParam, event.lParam);
	    default: break;
	    }
	}
#endif /* !_WINDOWS */

#ifdef _MOTIF

#ifdef _LSOSOS
      if (XPending (TtDisplay) == 0)
	{
	  /* current pointer position */
	  XQueryPointer (TtDisplay, w, &wdum, &wdum, &dx, &dy, &newx, &newy, &e);
	  /* coordinate checking */
	  newx = DO_ALIGN (newx - x);
	  newx += x;
	  newy = DO_ALIGN (newy - y);
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
		 newy = y;
               else if (newy > y + height)
		 newy = y + height;
               XWarpPointer (TtDisplay, None, w, 0, 0, 0, 0, newx, newy);
            }

	  /* Draw the new segments resulting of the new point */
	  if (newx != lastx || newy != lasty)
	    {
	      if (x1 != -1)
		{
		  XDrawLine (TtDisplay, w, TtInvertGC, x1, y1, lastx, lasty);
		  XDrawLine (TtDisplay, w, TtInvertGC, x1, y1, newx, newy);
		}
	      if (x3 != -1)
		{
		  XDrawLine (TtDisplay, w, TtInvertGC, lastx, lasty, x3, y3);
		  XDrawLine (TtDisplay, w, TtInvertGC, newx, newy, x3, y3);
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
	  newy = y + DO_ALIGN ((int) event.xmotion.y - y);
	  /* CHKR_LIMIT to size of the box */
	  /* new X valid position */
	  if (newx < x)
	    {
	      lastx = x;
	      wrap = TRUE;
	    }
	  else if (newx > x + width)
	    {
	      lastx = x + width;
	      wrap = TRUE;
	    }
	  else
	    lastx = newx;

	  /* new Y valid position */
	  if (newy < y)
	    {
	      lasty = y;
	      wrap = TRUE;
	    }
	  else if (newy > y + height)
	    {
	      lasty = y + height;
	      wrap = TRUE;
	    }
	  else
	    lasty = newy;

#else /* #ifdef _LSOSOS */
    
	  XNextEvent (TtDisplay, &event);
	  switch (event.type)
	    {
	    case KeyPress:
	      /* stop the creation of the polyline */
	      ret = 1;
	      break;

	    case ButtonPress:
	      /* it's a press button event */
	      input = TRUE;
	      break;

	    case ButtonRelease:
	      if (input)
		{
		  /* left button keep the last segment built */
		  /* keep the new segment first point coordinates */
		  x1 = lastx;
		  y1 = lasty;
		  (*nbpoints)++;
		  point++;
		  /* points are registerd */
		  input = TRUE;

		  /* update the box buffer */
		  newx = LogicalValue (lastx - x, UnPixel, NULL,
				       ViewFrameTable[frame - 1].FrMagnification);
		  newy = LogicalValue (lasty - y, UnPixel, NULL,
				       ViewFrameTable[frame - 1].FrMagnification);
		  AddPointInPolyline (Bbuffer, point, newx, newy);
		  /* update the abstract box buffer */
		  newx = (int) ((float) newx * ratioX);
		  newy = (int) ((float) newy * ratioY);
		  AddPointInPolyline (Pbuffer, point, newx, newy);

		  if (*nbpoints > maxPoints && maxPoints != 0)
		    /* we have the right number of points */
		    ret = 1;
		  else if (maxPoints == 0 && event.xbutton.button != Button1)
		    /* any other button : end of user input */
		    ret = 1;
		}
	      else
		{
		  newx = lastx;
		  newy = lasty;
		}
	      break;

	    case Expose:
	      f = GetWindowFrame (event.xexpose.window);
	      if (f <= MAX_FRAME + 1)
		FrameToRedisplay (event.xexpose.window, f, (XExposeEvent *) & event);
	      XtDispatchEvent (&event);
	      break;
	    case MotionNotify:
	      /* check the coordinates */
	      newx = x + DO_ALIGN ((int) event.xmotion.x - x);
	      newy = y + DO_ALIGN ((int) event.xmotion.y - y);
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
		  newy = y;
		  wrap = TRUE;
		}
	      else if (newy > y + height)
		{
		  newy = y + height;
		  wrap = TRUE;
		} 
	      
	      /* shows the new adjacent segment position */
	      if (1 || newx != lastx || newy != lasty)
		{
		  if (x1 != -1)
		    {
		      XDrawLine (TtDisplay, w, TtInvertGC, x1, y1, lastx, lasty);
		      XDrawLine (TtDisplay, w, TtInvertGC, x1, y1, newx, newy);
		    }
		  if (x3 != -1)
		    {
		      XDrawLine (TtDisplay, w, TtInvertGC, lastx, lasty, x3, y3);
		      XDrawLine (TtDisplay, w, TtInvertGC, newx, newy, x3, y3);
		    }
		}
	      lastx = newx;
	      lasty = newy;
	      if (wrap)
		{
		  XWarpPointer (TtDisplay, None, w, 0, 0, 0, 0, lastx, lasty);
		  wrap = FALSE;
		}
	      break;
	    default: 
	      break;
	    }
#endif /* #ifdef _LSOSOS */
#endif /* _MOTIF */	
    
#ifdef _GTK
	  event = gdk_event_get ();

	  if (event)
	    {
		/* we only deal with button press events */
	    switch (event->type)
	      {	 	      
	      case GDK_KEY_PRESS:
		  /* stop the creation of the polyline */
		  ret = 1;
		  break;
		case GDK_BUTTON_PRESS:
		  /* it's a press button event */
		  input = TRUE;
		  break;
		case  GDK_BUTTON_RELEASE:
		  if (input)
		    {
		      /* left button keep the last segment built */
		      /* keep the new segment first point coordinates */
		      x1 = lastx;
		      y1 = lasty;
		      (*nbpoints)++;
		      point++;
		      /* points are registered */
		      input = TRUE;
		      
		      /* update the box buffer */
		      newx = LogicalValue (lastx - x, UnPixel, NULL,
					   ViewFrameTable[frame - 1].FrMagnification);
		      newy = LogicalValue (lasty - y, UnPixel, NULL,
					   ViewFrameTable[frame - 1].FrMagnification);
		      AddPointInPolyline (Bbuffer, point, newx, newy);
		      /* update the abstract box buffer */
		      newx = (int) ((float) newx * ratioX);
		      newy = (int) ((float) newy * ratioY);
		      AddPointInPolyline (Pbuffer, point, newx, newy);
		      
		      if (*nbpoints > maxPoints && maxPoints != 0)
			/* we have the right number of points */
			ret = 1;
		      else if (maxPoints == 0 && (int) ((GdkEventButton *)event)->button != 1)
			/* any other button : end of user input */
			ret = 1;

		      input = FALSE;
		    }
		  break;
		  
		case GDK_EXPOSE:
		  f = GetWindowFrame (((GdkEventExpose *)event)->window);
		  if (f <= MAX_FRAME + 1)
		    FrameToRedisplay (((GdkEventExpose *)event)->window, 
				      f, 
				      (GdkEventExpose *) & event);
		  break;
	      case GDK_MOTION_NOTIFY:
	 	/* Check for window ID */
		if (((GdkEventMotion *)event)->window == w)
		  {
		    /* check the coordinates */
		    newx = x + DO_ALIGN ((int) ((GdkEventMotion *)event)->x - x);
		    newy = y + DO_ALIGN ((int) ((GdkEventMotion *)event)->y - y);
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
			newy = y;
	      wrap = TRUE;
	    }
	  else if (newy > y + height)
	    {
	      newy = y + height;
	      wrap = TRUE;
	    } 
	   
	  /* shows the new adjacent segment position */
	  if (1 || newx != lastx || newy != lasty)
	    {
	      if (x1 != -1)
		{
		  gdk_draw_line (w, TtInvertGC, x1, y1, lastx, lasty);
		  gdk_draw_line (w, TtInvertGC, x1, y1, newx, newy);
		}
	      if (x3 != -1)
		{
		  gdk_draw_line (w, TtInvertGC, lastx, lasty, x3, y3);
		  gdk_draw_line (w, TtInvertGC, newx, newy, x3, y3);
		}
	    }
	  lastx = newx;
	  lasty = newy;
	  if (wrap)
	    {
	      xwindow = (GdkWindowPrivate*) w;
	      XWarpPointer (GDK_DISPLAY(), 
			    None, 
			    xwindow->xwindow,
			    0, 0, 0, 0, lastx, lasty);
	      wrap = FALSE;
	    }
	      }
		break;
		default: 
		  break;
		}

#ifdef _GL
	    FrameTable[frame].DblBuffNeedSwap = TRUE;
	    GL_Swap (frame);
#endif /* _GL */

	}
#endif /* _GTK */

	}
      /* erase box frame */

  /*BoxGeometry (frame, x, y, width, height, x + width - 2, y + height - 2);*/
#ifdef _WINDOWS
  SetCursor (LoadCursor (NULL, IDC_ARROW));
#endif /* *_WINDOWS */

#ifdef _MOTIF
  ThotUngrab ();  
  XFlush (TtDisplay);
#endif /* #ifdef _MOTIF */
  
#ifdef _GTK
  ThotUngrab ();  
  gdk_window_set_cursor (GTK_WIDGET(FrameTable[frame].WdFrame)->window, ArrowCurs);
#endif /* !_GTK */

#endif /* #if defined(_MOTIF) || defined(_WINDOWS) || defined(_GTK) */
}


/*----------------------------------------------------------------------
  MoveApoint moves a point in a Polyline
   - x, y and lastx, lasty points to the current point
   - the first inserted point will be linked to x1, y1 when x1!= -1
   - the last inserted point will be linked to x3, y3 when x3!= -1
   - point gives the index of the first inserted point
   - x, y, width and height give the limits of the box
   - Pbuffer and Bbuffer point to the current Abstract Box buffer and
     Box buffer.
  ----------------------------------------------------------------------*/
static void MoveApoint (PtrBox box, int frame, int firstx, int firsty,
			int x1, int y1, int x3, int y3,
			int lastx, int lasty, int point,
                        int x, int y, int width, int height,
			PtrTextBuffer Pbuffer, PtrTextBuffer Bbuffer,
			int pointselect)
{
  ThotWindow          w;
#if defined(_MOTIF) || defined(_WINDOWS)
  ThotEvent           event;
#endif 
   
#ifdef _GTK
  ThotEvent           *event_tmp;
  ThotEvent           *event;
  GdkWindowPrivate    *xwindow;
#endif /*_GTK*/

  float               ratioX, ratioY;
  int                 ret;
  int                 newx, newy;
  ThotBool            input;
  ThotBool            wrap;

#ifdef _WINDOWS
  RECT                rect;
  POINT               cursorPos;
  POINT               ptBeg;
  POINT               ptEnd;
  POINT               ptCur;
  HCURSOR             cross;
#endif /* _WINDOWS */

#if defined(_MOTIF) || defined(_GTK)
  int                 e,f;
#endif /* #if defined(_MOTIF) || defined(_GTK) */

  /* need the window to change the cursor */
  w = FrRef[frame];
  newx = newy = 0;
  /* trasformation factor between the box and the abstract box */
  ratioX = (float) Pbuffer->BuPoints[0].XCoord / (float) Bbuffer->BuPoints[0].XCoord;
  /* trasformation factor between the box and the abstract box */
  ratioY = (float) Pbuffer->BuPoints[0].YCoord / (float) Bbuffer->BuPoints[0].YCoord;

#ifdef _WINDOWS
  GetWindowRect (w, &rect);
  /* The grid stepping begins at the origin */
  ptBeg.x = x1;
  ptBeg.y = y1;
  ptEnd.x = x3;
  ptEnd.y = y3;
  cross = LoadCursor (NULL, IDC_CROSS);
  SetCursorPos (lastx + rect.left, lasty + rect.top);
#endif /* !_WINDOWS */
  
#ifdef _MOTIF
  e = ButtonPressMask | ButtonReleaseMask | ButtonMotionMask;
  XMapRaised (TtDisplay, w);
  XFlush (TtDisplay);
  ThotGrab (w, HVCurs, e, 0);
  XWarpPointer (TtDisplay, None, w, 0, 0, 0, 0, lastx, lasty);
  XFlush (TtDisplay);
#endif /* _MOTIF */  
  
#ifdef _GTK
  gdk_window_set_cursor (GTK_WIDGET(FrameTable[frame].WdFrame)->window, HVCurs);
  e = GDK_BUTTON_MOTION_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK;
  ThotGrab (w, HVCurs, e, 0);
  xwindow = (GdkWindowPrivate*) w;
  XWarpPointer (GDK_DISPLAY(), 
		None, 
		xwindow->xwindow,
		0, 0, 0, 0, lastx, lasty);
#endif /* !_GTK */

  /* shows up limit borders */
  /*BoxGeometry (frame, x, y, width, height, x + width - 2, y + height - 2);*/
  /* loop waiting for the user input */
  ret = 0;
  /* take into account release button events that follow a press button event */
  input = FALSE;
  wrap = FALSE;
  while (ret == 0)
    {
#ifdef _WINDOWS
      /* current pointer position */
      GetCursorPos (&cursorPos);
      /* coordinate checking */
      newx = DO_ALIGN (cursorPos.x - rect.left - x);
      newx += x;
      newy = DO_ALIGN (cursorPos.y - rect.top - y);
      newy += y;
      if (newx < x || newx > x + width ||
	  newy < y || newy > y + height)
	{
	  /* CHKR_LIMIT to size of the box */
	  /* new X valid position */
	  if (newx < x)
	    newx = x;
	  else if (newx > x + width)
	    newx = x + width;
	      
	  /* new Y valid position */
	  if (newy < y)
	    newy = y;
	  else if (newy > y + height)
	    newy = y + height;
	  /*SetCursorPos (newx, newy);*/
	}
	  
      GetMessage (&event, NULL, 0, 0);
	  SetCursor (cross);
      switch (event.message)
	{
	case WM_LBUTTONUP:
	  lastx = newx;
	  lasty = newy;
	  /* update the box buffer */
	  newx = LogicalValue (lastx - firstx, UnPixel, NULL,
			       ViewFrameTable[frame - 1].FrMagnification);
	  newy = LogicalValue (lasty - firsty, UnPixel, NULL,
			       ViewFrameTable[frame - 1].FrMagnification);
	  ModifyPointInPolyline (Bbuffer, point, newx, newy);
	  /* update the abstract box buffer */
	  newx = (int) ((float) newx * ratioX);
	  newy = (int) ((float) newy * ratioY);
	  ModifyPointInPolyline (Pbuffer, point, newx, newy);
	  ret = 1;
	  break;

	case WM_MOUSEMOVE:
	  /* refresh the display of teh two adjacent segments */
	  if (newx != lastx || newy != lasty)
	    {
	      if (x1 != -1)
		{
		  /* remove previous line */
		  ptCur.x = lastx /*- rect.left*/;
		  ptCur.y = lasty /*- rect.top*/;
		  DrawOutline (w, ptBeg, ptCur);
		  /* add a new line */
		  ptCur.x = newx /*- rect.left*/;
		  ptCur.y = newy /*- rect.top*/;
		  DrawOutline (w, ptBeg, ptCur);
		}
	      if (x3 != -1)
		{
		  /* remove previous line */
		  ptCur.x = lastx;
		  ptCur.y = lasty;
		  DrawOutline (w, ptCur, ptEnd);
		  /* add a new line */
		  ptCur.x = newx;
		  ptCur.y = newy;
		  DrawOutline (w, ptCur, ptEnd);
		}	     
	   }
	  lastx = newx;
	  lasty = newy;
	  break;

	case WM_PAINT:
	  WIN_HandleExpose (w, frame, event.wParam, event.lParam);
	default: break;
	}
#endif /* !_WINDOWS */
      
#ifdef _MOTIF
      XNextEvent (TtDisplay, &event);
	  
      /* check the coordinates */
      newx = x + DO_ALIGN ((int) event.xmotion.x - x);
      newy = y + DO_ALIGN ((int) event.xmotion.y - y);
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
	  newy = y;
	  wrap = TRUE;
	}
      else if (newy > y + height)
	{
	  newy = y + height;
	  wrap = TRUE;
	}

      switch (event.type)
	{
	case ButtonRelease:
	  lastx = newx;
	  lasty = newy;
	  /* update the box buffer */
	  newx = LogicalValue (lastx - firstx, UnPixel, NULL,
			       ViewFrameTable[frame - 1].FrMagnification);
	  newy = LogicalValue (lasty - firsty, UnPixel, NULL,
			       ViewFrameTable[frame - 1].FrMagnification);
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
		  XDrawLine (TtDisplay, w, TtInvertGC, x1, y1, lastx, lasty);
		  XDrawLine (TtDisplay, w, TtInvertGC, x1, y1, newx, newy);
		}
	      if (x3 != -1)
		{
		  XDrawLine (TtDisplay, w, TtInvertGC, lastx, lasty, x3, y3);
		  XDrawLine (TtDisplay, w, TtInvertGC, newx, newy, x3, y3);
		}
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
	}
#endif /* _MOTIF */      
      
#ifdef _GTK
      event = gdk_event_get ();
      if (event)
	{

	  if (event->type == GDK_MOTION_NOTIFY)	
	    {
	      /* We take only last position update */
	      while (gdk_events_pending ()) 
		{
		  event_tmp = gdk_event_get ();
		  if (event_tmp)
		    event = event_tmp;
		}
	      if (event->type == GDK_MOTION_NOTIFY) 
		{
		  if (((GdkEventMotion *)event)->window == w)
		    {
		      /* check the coordinates */
		      newx = x + DO_ALIGN ((int) ((GdkEventMotion *)event)->x - x);
		      newy = y + DO_ALIGN ((int)((GdkEventMotion *)event)->y - y);
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
			  newy = y;
			  wrap = TRUE;
			}
		      else if (newy > y + height)
			{
			  newy = y + height;
			  wrap = TRUE;
			} 
		    }
		  /* shows the new adjacent segment position */
		  if (newx != lastx || newy != lasty)
		    {

#ifndef _GL
		      if (x1 != -1)
			{
			  gdk_draw_line (w, TtInvertGC, x1, y1, lastx, lasty);
			  gdk_draw_line (w, TtInvertGC, x1, y1, newx, newy);
			}
		      if (x3 != -1)
			{
			  gdk_draw_line (w, TtInvertGC, lastx, lasty, x3, y3);
			  gdk_draw_line (w, TtInvertGC, newx, newy, x3, y3);
			}
#else /* _GL */

		      lastx = newx;
		      lasty = newy;
		      /* update the box buffer */
		      newx = LogicalValue (lastx - firstx, UnPixel, NULL,
					   ViewFrameTable[frame - 1].FrMagnification);
		      newy = LogicalValue (lasty - firsty, UnPixel, NULL,
					   ViewFrameTable[frame - 1].FrMagnification);

		      ModifyPointInPolyline (Bbuffer, point, newx, newy);

		      /* update the abstract box buffer */
		      newx = (int) ((float) newx * ratioX);
		      newy = (int) ((float) newy * ratioY);

		      ModifyPointInPolyline (Pbuffer, point, newx, newy);

		      {
			PtrAbstractBox pAb;

			pAb = box->BxAbstractBox;
			if (pointselect != 0)
			  {
			    if (!box->BxAbstractBox->AbWidth.DimIsPosition && 
				box->BxAbstractBox->AbEnclosing)
			      /* this rule is applied to the parent */
			      pAb = box->BxAbstractBox->AbEnclosing;
			    switch (pointselect)
			      {
			      case 1:
			      case 7:
				if (box->BxHorizInverted)
				  NewDimension (pAb, newx, newy, frame, TRUE);
				else
				  NewPosition (pAb, newx, 0, newy, 0, frame, TRUE);
				break;
			      case 3:
			      case 5:
				if (box->BxHorizInverted)
				  NewPosition (pAb, newx, 0, newy, 0, frame, TRUE);
				else
				  NewDimension (pAb, newx, newy, frame, TRUE);
				break;
			      default: break;
			      }
			  }
			else
			  {
			    if (box->BxPictInfo != NULL)
			      {
				/* we have to recompute the current spline */
				free ((STRING) box->BxPictInfo);
				box->BxPictInfo = NULL;
			      }
			    NewContent (pAb);
			    APPgraphicModify (pAb->AbElement, pointselect, frame, FALSE, TRUE);
			  }


			DefRegion (frame, box->BxClipX - EXTRA_GRAPH,
				   box->BxClipY - EXTRA_GRAPH,
				   box->BxClipX + width + EXTRA_GRAPH,
				   box->BxClipY + height + EXTRA_GRAPH);
			FrameTable[frame].DblBuffNeedSwap = TRUE;
			GL_Swap (frame);
		      }
#endif /* _GL */
		      lastx = newx;
		      lasty = newy;
		      if (wrap)
			{
			  xwindow = (GdkWindowPrivate*) w;
			  XWarpPointer (GDK_DISPLAY(), 
					None, 
					xwindow->xwindow,
					0, 0, 0, 0, lastx, lasty);
			  wrap = FALSE;
			}
		    }
		}
	    }
	  if (event->type == GDK_BUTTON_RELEASE)
	    {
	      lastx = newx;
	      lasty = newy;
	      /* update the box buffer */
	      newx = LogicalValue (lastx - firstx, UnPixel, NULL,
				   ViewFrameTable[frame - 1].FrMagnification);
	      newy = LogicalValue (lasty - firsty, UnPixel, NULL,
				   ViewFrameTable[frame - 1].FrMagnification);
	      ModifyPointInPolyline (Bbuffer, point, newx, newy);
	      /* update the abstract box buffer */
	      newx = (int) ((float) newx * ratioX);
	      newy = (int) ((float) newy * ratioY);
	      ModifyPointInPolyline (Pbuffer, point, newx, newy);
	      ret = 1;
	    }
	   
	  if (event->type == GDK_EXPOSE)
	    {
	      f = GetWindowFrame (((GdkEventExpose *)event)->window);
	      if (f <= MAX_FRAME + 1)
		FrameToRedisplay (((GdkEventExpose *)event)->window, 
				  f, 
				  (GdkEventExpose *) & event);
	    }
	}
#endif /* _GTK */
      
    }
  /* erase box frame */
  /*BoxGeometry (frame, x, y, width, height, x + width - 2, y + height - 2);*/
#ifdef _WINDOWS
  SetCursor (LoadCursor (NULL, IDC_ARROW));
#endif /* *_WINDOWS */
  
#ifdef _MOTIF
  ThotUngrab ();
  XFlush (TtDisplay);
#endif /* #ifdef _MOTIF */
  
#ifdef _GTK
  ThotUngrab ();
  gdk_window_set_cursor (GTK_WIDGET(FrameTable[frame].WdFrame)->window, ArrowCurs);
#endif /* _GTK */
}


/*----------------------------------------------------------------------
  PolyLineCreation interacts with the user to read points forming
  a polyline in a given frame.
  *xOrg and *yOrg values give the position of the box pBox in the frame.
  Bbuffer points to the first buffer of the concrete box.
  Pbuffer points to the first buffer of the abstract box, i.e.
  the list of control points modified. The first point in the
  list gives the maximum width and height of the polyline.
  If the parameter maxPoints is greater than 1 if the number of
  points to be selected must be equal to this value.
  This fonction updates both list of control points and
  returns the number of point in the polyline.
  ----------------------------------------------------------------------*/
int PolyLineCreation (int frame, int *xOrg, int *yOrg, PtrBox pBox,
		      int maxPoints)
{
  PtrTextBuffer       Pbuffer;
  PtrTextBuffer       Bbuffer;
  int                 width, height;
  int                 x, y;
  int                 lastx, lasty;
  int                 nbpoints;

  if (pBox == NULL || pBox->BxAbstractBox == NULL)
    return (0);
  Bbuffer = pBox->BxBuffer;
  Pbuffer = pBox->BxAbstractBox->AbPolyLineBuffer;

  /* Initialize the bounding box */
  /* constraint is done by the polyline element */
  x = *xOrg;
  width = Bbuffer->BuPoints[0].XCoord;
  width = PixelValue (width, UnPixel, NULL,
		      ViewFrameTable[frame - 1].FrMagnification);
  y = *yOrg;
  height = Bbuffer->BuPoints[0].YCoord;
  height = PixelValue (height, UnPixel, NULL,
		       ViewFrameTable[frame - 1].FrMagnification);

#if defined(_WINDOWS) && !defined(_GL)
  Gdc = GetDC (FrRef[frame]);
#endif /* _WINDOWS */
  
  nbpoints = 1;
  lastx = x; 
  lasty = y;
  AddPoints (frame, x, y, -1, -1, -1, -1, lastx, lasty, 1, &nbpoints, maxPoints, width, height,
	     Pbuffer, Bbuffer);

#if defined(_WINDOWS) && !defined(_GL)
  ReleaseDC (FrRef[frame], Gdc);
#endif /* _WINDOWS */
  
  return (nbpoints);
}

/*----------------------------------------------------------------------
  PolyLineModification interacts with the user to move a point of
  a polyline in a given frame.
  *xOrg and *yOrg values give the position of the box pBox in the frame.
  Bbuffer points to the first buffer of the box, i.e. the list of control
  points as shown. The first point in the lists gives the maximum
  width and height of the polyline.
  Pbuffer points to the first buffer of the abtract box.
  nbpoints gives the number of points in the polyline.
  point is the rank of the point to be moved.
  This function updates both lists of control points.
  ----------------------------------------------------------------------*/
void PolyLineModification (int frame, int *xOrg, int *yOrg, PtrBox pBox,
			   int nbpoints, int point, ThotBool close)
{
  PtrTextBuffer       Pbuffer;
  PtrTextBuffer       Bbuffer;
  PtrAbstractBox      draw;
  int                 width, height;
  int                 lastx, lasty;
  int                 x1, y1, x3, y3;
  int                 x, y;

  if (pBox == NULL || pBox->BxAbstractBox == NULL)
     return;

  Bbuffer = pBox->BxBuffer;
  Pbuffer = pBox->BxAbstractBox->AbPolyLineBuffer;

  /* constraint is given by the enclosing Graphics root element */
  draw = GetParentDraw (pBox);
  if (draw)
    {
      x = draw->AbBox->BxXOrg;
      y =  draw->AbBox->BxYOrg;
      width = draw->AbBox->BxW;
      height = draw->AbBox->BxH;
    }
  else
    {
      x = pBox->BxAbstractBox->AbEnclosing->AbBox->BxXOrg;
      y =  pBox->BxAbstractBox->AbEnclosing->AbBox->BxYOrg;
      width = pBox->BxAbstractBox->AbEnclosing->AbBox->BxW;
      height = pBox->BxAbstractBox->AbEnclosing->AbBox->BxH;
    }
  width = PixelValue (width, UnPixel, NULL,
		      ViewFrameTable[frame - 1].FrMagnification);
  height = PixelValue (height, UnPixel, NULL,
		       ViewFrameTable[frame - 1].FrMagnification);

#if defined(_WINDOWS) && !defined(_GL)
  Gdc = GetDC (FrRef[frame]);
#endif /* _WINDOWS */

  /* get the current point */
  RedrawPolyLine (frame, *xOrg, *yOrg, Bbuffer, nbpoints, point, close,
  &x1, &y1, &lastx, &lasty, &x3, &y3);
  MoveApoint (pBox, frame, *xOrg, *yOrg, x1, y1, x3, y3, lastx, lasty, point, x, y, width, height, Pbuffer, Bbuffer, 0);

#if defined(_WINDOWS) && !defined(_GL)
  ReleaseDC (FrRef[frame], Gdc);
#endif /* _WINDOWS */

  if (pBox->BxPictInfo != NULL)
    {
      /* we have to recompute the current spline */
      free ((STRING) pBox->BxPictInfo);
      pBox->BxPictInfo = NULL;
    }
}


/*----------------------------------------------------------------------
  PolyLineExtension interract with the user to add points to
  an existing polyline in a given frame.
  *xOrg and *yOrg values give the position of the box pBox in the frame.
  Bbuffer points to the first buffer of the box, i.e. the list of control
  points as shown. The first point in the lists gives the maximum
  width and height of the polyline.
  Pbuffer points to the first buffer of the abtract box.
  nbpoints gives the number of points in the polyline.
  point expricitely indicate the point to be moved.
  close indicate whether the polyline is closed.
  This fonction updates both  list of control points and
  returns the new number of points in the polyline.
  ----------------------------------------------------------------------*/
int PolyLineExtension (int frame, int *xOrg, int *yOrg, PtrBox pBox,
		       int nbpoints, int point, ThotBool close)
{
  PtrTextBuffer       Pbuffer;
  PtrTextBuffer       Bbuffer;
  int                 width, height;
  int                 x, y;
  int                 lastx, lasty;
  int                 x1, y1, x3, y3;

  if (pBox == NULL || pBox->BxAbstractBox == NULL)
    return (0);
  Bbuffer = pBox->BxBuffer;
  Pbuffer = pBox->BxAbstractBox->AbPolyLineBuffer;

  /* Initialize the bounding box */
  /* constraint is done by the polyline element */
  x = *xOrg;
  width = Bbuffer->BuPoints[0].XCoord;
  width = PixelValue (width, UnPixel, NULL,
		      ViewFrameTable[frame - 1].FrMagnification);
  y = *yOrg;
  height = Bbuffer->BuPoints[0].YCoord;
  height = PixelValue (height, UnPixel, NULL,
		       ViewFrameTable[frame - 1].FrMagnification);

#if defined(_WINDOWS) && !defined(_GL)
  Gdc = GetDC (FrRef[frame]);
#endif /* _WINDOWS */

  RedrawPolyLine (frame, x, y, Bbuffer, nbpoints, point, close,
		  &x1, &y1, &lastx, &lasty, &x3, &y3);
  x1 = lastx;
  y1 = lasty;
  AddPoints (frame, x, y, x1, y1, x3, y3, lastx, lasty, point, &nbpoints, 0, width, height,
	     Pbuffer, Bbuffer);

#if defined(_WINDOWS) && !defined(_GL)
  ReleaseDC (FrRef[frame], Gdc);
#endif /* _WINDOWS */

  return (nbpoints);
}


/*----------------------------------------------------------------------
  LineCreation interracts with the user to read points forming
  a line in a given frame.
  *x1 and *y1 values give the initial position of the box pBox in the frame
  This fonction returns position of extremities: x1, y1 and x2, y2.
  ----------------------------------------------------------------------*/
int LineCreation (int frame, PtrBox pBox, int *x1, int *y1, int *x2,
		  int *y2)
{
  PtrTextBuffer       pBuffer;
  PtrAbstractBox      draw;
  int                 width, height;
  int                 x, y;
  int                 lastx, lasty;
  int                 nbpoints;

  if (pBox == NULL || pBox->BxAbstractBox == NULL ||
	  pBox->BxAbstractBox->AbEnclosing == NULL)
    return (0);

  /* Allocate a polyline buffer to simulate a polyline */ 
  GetTextBuffer (&pBuffer);
  /* get draw limits */
  draw = GetParentDraw (pBox);
  if (draw)
    {
      width = draw->AbBox->BxW;
      height = draw->AbBox->BxH;
    }
  else
    {
      width = pBox->BxAbstractBox->AbEnclosing->AbBox->BxW;
      height = pBox->BxAbstractBox->AbEnclosing->AbBox->BxH;
    }
  pBuffer->BuPoints[0].XCoord = LogicalValue (width, UnPixel, NULL,
					      ViewFrameTable[frame - 1].FrMagnification);
  pBuffer->BuPoints[0].YCoord = LogicalValue (height, UnPixel, NULL,
					      ViewFrameTable[frame - 1].FrMagnification);
  x = *x1;
  y = *y1;

#if defined(_WINDOWS) && !defined(_GL)
  Gdc = GetDC (FrRef[frame]);
#endif /* _WINDOWS */

  nbpoints = 1;
  lastx = x; 
  lasty = y;
  AddPoints (frame, x, y, -1, -1, -1, -1, lastx, lasty, 1, &nbpoints, 2, width, height,
	     pBuffer, pBuffer);
  *x1 = PixelValue (pBuffer->BuPoints[1].XCoord, UnPixel, NULL,
		    ViewFrameTable[frame - 1].FrMagnification);
  *y1 = PixelValue (pBuffer->BuPoints[1].YCoord, UnPixel, NULL,
		    ViewFrameTable[frame - 1].FrMagnification);
  *x2 = PixelValue (pBuffer->BuPoints[2].XCoord, UnPixel, NULL,
		    ViewFrameTable[frame - 1].FrMagnification);
  *y2 = PixelValue (pBuffer->BuPoints[2].YCoord, UnPixel, NULL,
		    ViewFrameTable[frame - 1].FrMagnification);

#if defined(_WINDOWS) && !defined(_GL)
  ReleaseDC (FrRef[frame], Gdc);
#endif /* _WINDOWS */

  /* Free the buffer */
  FreeTextBuffer (pBuffer);
  return (nbpoints);
}

/*----------------------------------------------------------------------
  LineModification interracts with the user to move a point of
  a line in a given frame.
  The parameter point locates the modified reference point.
  Values xi, yi give the initial position of the box origin in the window.
  This fonction returns the new position of the extremity: xi, yi.
  ----------------------------------------------------------------------*/
void LineModification (int frame, PtrBox pBox, int point, int *xi, int *yi)
{
  ViewFrame          *pFrame;
  PtrTextBuffer       pBuffer;
  PtrAbstractBox      draw;
  int                 width, height;
  int                 x1, y1, x3, y3;
  int                 x, y, xorg, yorg;
  int                 lastx, lasty;

  *xi = *yi = 0;
  if (pBox == NULL || pBox->BxAbstractBox == NULL || pBox->BxAbstractBox->AbEnclosing == NULL)
    return;

  /* Allocate a polyline buffer to simulate a polyline */ 
  GetTextBuffer (&pBuffer);
  /* get draw limits */
  draw = GetParentDraw (pBox);
  if (draw)
    {
      xorg = draw->AbBox->BxXOrg;
      yorg =  draw->AbBox->BxYOrg;
      width = draw->AbBox->BxW;
      height = draw->AbBox->BxH;
    }
  else
    {
      xorg = pBox->BxAbstractBox->AbEnclosing->AbBox->BxXOrg;
      yorg =  pBox->BxAbstractBox->AbEnclosing->AbBox->BxYOrg;
      width = pBox->BxAbstractBox->AbEnclosing->AbBox->BxW;
      height = pBox->BxAbstractBox->AbEnclosing->AbBox->BxH;
    }
  pBuffer->BuPoints[0].XCoord = LogicalValue (width, UnPixel, NULL,
					      ViewFrameTable[frame - 1].FrMagnification);
  pBuffer->BuPoints[0].YCoord = LogicalValue (height, UnPixel, NULL,
					      ViewFrameTable[frame - 1].FrMagnification);

  /* store current points in the buffer:
     positions are relative to the parent box origin */
  pFrame = &ViewFrameTable[frame - 1];
  x = pBox->BxXOrg - xorg;
  y =  pBox->BxYOrg - yorg;
  switch (point)
    {
    case 1:
      x1 = x + pBox->BxWidth;
      y1 = y + pBox->BxHeight;
      break;
    case 3:
      x1 = x;
      x += pBox->BxWidth;
      y1 = y + pBox->BxHeight;
      break;
    case 5:
      x1 = x;
      x += pBox->BxWidth;
      y1 = y;
      y += pBox->BxHeight;
      break;
    case 7:
      x1 = x + pBox->BxWidth;
      y1 = y;
      y += pBox->BxHeight;
      break;
    default: break;
    }
  pBuffer->BuPoints[1].XCoord = LogicalValue (x, UnPixel, NULL,
					      ViewFrameTable[frame - 1].FrMagnification);
  pBuffer->BuPoints[1].YCoord = LogicalValue (y, UnPixel, NULL,
					      ViewFrameTable[frame - 1].FrMagnification);
  pBuffer->BuPoints[2].XCoord = LogicalValue (x1, UnPixel, NULL,
					      ViewFrameTable[frame - 1].FrMagnification);
  pBuffer->BuPoints[2].YCoord = LogicalValue (y1, UnPixel, NULL,
					      ViewFrameTable[frame - 1].FrMagnification);

#if defined(_WINDOWS) && !defined(_GL)
  Gdc = GetDC (FrRef[frame]);
#endif /* _WINDOWS */

  /* get the current point */
  xorg -= pFrame->FrXOrg;
  yorg -= pFrame->FrYOrg;
  RedrawPolyLine (frame, xorg, yorg, pBuffer, 3, 1, FALSE,
		  &x1, &y1, &lastx, &lasty, &x3, &y3);
  MoveApoint (pBox, frame, xorg, yorg, x1, y1, x3, y3, lastx, lasty, 1,
	      xorg, yorg, width, height, pBuffer, pBuffer, point);
  *xi = PixelValue (pBuffer->BuPoints[1].XCoord, UnPixel, NULL,
		    ViewFrameTable[frame - 1].FrMagnification);
  *yi = PixelValue (pBuffer->BuPoints[1].YCoord, UnPixel, NULL,
		    ViewFrameTable[frame - 1].FrMagnification);

#if defined(_WINDOWS) && !defined(_GL)
  ReleaseDC (FrRef[frame], Gdc);
#endif /* _WINDOWS */

  /* Free the buffer */
  FreeTextBuffer (pBuffer);
}


/*----------------------------------------------------------------------
  Resizing
  This function returns the new dimensions of the box.
  ----------------------------------------------------------------------*/
static void Resizing (int frame, int *x, int *y, int *width, int *height,
		      PtrBox box, int xmin, int xmax, int ymin, int ymax,
		      int xm, int ym, int percentW, int percentH)
{
#define C_TOP 0
#define C_HCENTER 1
#define C_BOTTOM 2
#define C_LEFT 0
#define C_VCENTER 1
#define C_RIGHT 2

#if defined(_MOTIF) 
   ThotEvent           event;
#endif /* #if defined(_MOTIF) */
   
#ifdef _GTK
   ThotEvent           *event_tmp;
   ThotEvent           *event;
   GdkWindowPrivate    *xwindow;
#endif /*_GTK*/
   
  ThotWindow          w;
  PtrAbstractBox      pAb;
  int                 ret, dx, dy, dl, dh;
  int                 ref_h, ref_v;
  int                 hDirection, vDirection;
  int                 xref, yref;
  int                 warpx, warpy;

#ifdef _WINDOWS
  ThotEvent			  event;
  POINT               cursorPos;
#endif /* _WINDOWS */

  ThotBool           isEllipse;

  pAb = box->BxAbstractBox;
  if (pAb && pAb->AbLeafType == LtCompound)
    pAb = pAb->AbFirstEnclosed;
  isEllipse = (pAb &&
	       pAb->AbLeafType == LtGraphics &&
	       (pAb->AbShape == 'a' || pAb->AbShape == 'c'));
  /* select the correct cursor */
  w = FrRef[frame];
  /* Use the reference point to move the box */
  switch (box->BxHorizEdge)
    {
    case Right:
      xref = *width;
      ref_v = C_RIGHT;
      /* Box shortened when X delta increase */
      hDirection = -1;
      break;
    case VertMiddle:
      xref = *width / 2;
      ref_v = C_VCENTER;
      /* Direction depends of original cursor location in the frame */
      if (xm < *x + xref)
	hDirection = -1;
      else
	hDirection = 1;
      break;
    case VertRef:
      xref = box->BxVertRef;
      ref_v = C_LEFT;
      /* Box increased when X delta increase */
      hDirection = 1;
      break;
    default:
      xref = 0;
      ref_v = C_LEFT;
      /* Box increased when X delta increase */
      hDirection = 1;
      break;
    }
  switch (box->BxVertEdge)
    {
    case Bottom:
      yref = *height;
      ref_h = C_BOTTOM;
      /* Box shortened when Y delta increase */
      vDirection = -1;
      break;
    case HorizMiddle:
      yref = *height / 2;
      ref_h = C_HCENTER;
      /* Direction depends of original cursor location in the frame */
      if (ym < *y + yref)
	vDirection = -1;
      else
	vDirection = 1;
      break;
    case HorizRef:
      yref = box->BxHorizRef;
      ref_h = C_TOP;
      /* Box increased when Y delta increase */
      vDirection = 1;
      break;
    default:
      yref = 0;
      ref_h = C_TOP;
      /* Box increased when Y delta increase */
      vDirection = 1;
      break;
    }  

  /* Shows the initial box size */
  if (isEllipse)
    InvertEllipse (frame, *x, *y, *width, *height, *x + xref, *y + yref);
  else
    BoxGeometry (frame, *x, *y, *width, *height, *x + xref, *y + yref);
  ret = 0;
  while (ret == 0)
    {

#ifdef _WINDOWS
      GetMessage (&event, NULL, 0, 0);
      switch (event.message)
	{
	case WM_LBUTTONUP:
	case WM_MBUTTONUP:
	case WM_RBUTTONUP:
	  ret = 1;
	  break;
	  
	case WM_MOUSEMOVE:
	  GetCursorPos (&cursorPos);
	  dl = cursorPos.x - xm;
	  dh = cursorPos.y - ym;
	  if (percentW != 0)
	    {
	      /* keep the greater value */
	      if (dl < dh)
		dl = dh;
	      dh = dl;
	    }
	  else if (percentH != 0)
	    {
	      /* keep the greater value */
	      if (dh < dl)
		dh = dl;
	      dl = dh;
	    }
	  /* Check that size can be modified, and stay >= 0    */
	  /* depending on base point and cursor position,      */
	  /* increase or decreas width or height accordingly   */
	  warpx = -1;
	  warpy = -1;
	  if (dl != 0)
	    if (xmin == xmax) /* X moves frozen */
	      dl = 0;
	    else if (ref_v == C_VCENTER && *width + (2 * dl * hDirection) < 0)
	      {
		dl = -DO_ALIGN (*width / 2) * hDirection;
		warpx = xm + (dl * hDirection);
	      }
	    else if (*width + (dl * hDirection) < 0)
	      {
		dl = -DO_ALIGN (*width) * hDirection;
		warpx = xm + (dl * hDirection);
	      }

	  if (dh != 0)
	    if (ymin == ymax) /* Y moves frozen */
	      dh = 0;
	    else if (ref_h == C_HCENTER && *height + (2 * dh * vDirection) < 0)
	      {
		dh = -(*height * vDirection * GridSize) / (2 * GridSize);
		warpy = ym + (dh * vDirection);
	      }
	    else if (*height + dh < 0)
	      {
		dh = -(*height * vDirection * GridSize) / GridSize;
		warpy = ym + (dh * vDirection);
	      }

	  /* Compute the horizontal move of the origin */
	  if (dl != 0)
	    {
	      dl = dl * hDirection; /* Take care for direction */
	      if (ref_v == C_VCENTER)
		{
		  dx = xmin + DO_ALIGN (*x - (dl / 2) - xmin) - *x;
		  /* Check the move is within limits */
		  if (*x + dx < xmin)
		    dx = xmin - *x; /*left side */
		  if (*x + *width - dx > xmax)
		    dx = *x + *width - xmin - DO_ALIGN (xmax - xmin); /*right side */
		  
		  /* modify width for real */
		  dl = -(dx * 2);
		  if (dx != 0)
		    warpx = xm - (dx * hDirection);
		}
	      else if (ref_v == C_RIGHT)
		{
		  dx = xmin + DO_ALIGN (*x - dl - xmin) - *x;
		  /* Check the move is within limits */
		  if (*x + dx < xmin)
		    dx = xmin - *x; /*left side */

		  /* modify width for real */
		  dl = -dx;
		  if (dx != 0)
		    warpx = xm + dx;
		}
	      else
		{
		  dx = 0;
		  dl = xmin + DO_ALIGN (*x + *width + dl - xmin) - *x - *width;
		  if (*x + *width + dl > xmax)
		    dl = xmin + DO_ALIGN (xmax - xmin) - *x - *width; /*right side */
		  if (dl != 0)
		    warpx = xm + dl;
		}
	    }
	  else
	    dx = 0;
	  
	  /* Compute vertical move */
	  if (dh != 0)
	    {
	      dh = dh * vDirection;	/* Take care for direction */
	      if (ref_h == C_HCENTER)
		{
		  dy = ymin + DO_ALIGN (*y - (dh / 2) - ymin) - *y;
		  /* Check the move is within limits */
		  if (*y + dy < ymin)
		    dy = ymin - *y; /*upper border */
		  if (*y + *height - dy > ymax)
		    dy = *y + *height - ymin - DO_ALIGN (ymax - ymin);	/*bottom */
		  /* change the height for real */
		  dh = -(dy * 2);
		  if (dy != 0)
		    warpy = ym - (dy * vDirection);
		}
	      else if (ref_h == C_BOTTOM)
		{
		  dy = ymin + DO_ALIGN (*y - dh - ymin) - *y;
		  /* Check the move is within limits */
		  if (*y + dy < ymin)
		    dy = ymin - *y; /*upper border */
		  /* change the height for real */
		  dh = -dy;
		  if (dy != 0)
		    warpy = ym + dy;
		}
	      else
		{
		  dy = 0;
		  dh = ymin + DO_ALIGN (*y + *height + dh - ymin) - *y - *height;
		  if (*y + *height + dh > ymax)
		    dh = ymin + DO_ALIGN (ymax - ymin) - *y - *height; /*bottom */
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
	      if (isEllipse)
		InvertEllipse (frame, *x, *y, *width, *height, *x + xref, *y + yref);
	      else
		BoxGeometry (frame, *x, *y, *width, *height, *x + xref, *y + yref);
	      /* is there any dependence between height and width */
	      *width = *width + dl;
	      *height = *height + dh;
	      if (percentW != 0)
		{
		  *width = *height * percentW / 100;
		  dx = dy *  percentW / 100;
		}
	      else if (percentH != 0)
		{
		  *height = *width * percentH / 100;
		  dy = dx *  percentH / 100;
		}
	      *x = *x + dx;
	      *y = *y + dy;
	      /* switch on the new one */
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
		  break;
		default:
		  yref = 0;
		  break;
		}  
	      if (isEllipse)
		InvertEllipse (frame, *x, *y, *width, *height, *x + xref, *y + yref);
	      else
		BoxGeometry (frame, *x, *y, *width, *height, *x + xref, *y + yref);
	    }

	  /* Should we move the cursor */
	  if (warpx >= 0 || warpy >= 0)
	    {
	      if (warpx >= 0)
		xm = warpx /*+ rect.left*/;
	      if (warpy >= 0)
		ym = warpy /*+ rect.top*/;
	    }
	  break;

	case WM_PAINT:
	  WIN_HandleExpose (w, frame, event.wParam, event.lParam);
	default:  break;
        }
#endif /* _WINDOWS */

#ifdef _MOTIF
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
	      if (percentW != 0)
		{
		  /* keep the greater value */
		  if (dl < dh)
		    dl = dh;
		  dh = dl;
		}
	      else if (percentH != 0)
		{
		  /* keep the greater value */
		  if (dh < dl)
		    dh = dl;
		  dl = dh;
		}
	    }
	  else
	    dl = dh = 0;

	  /* Check that size can be modified, and stay >= 0    */
	  /* depending on base point and cursor position,      */
	  /* increase or decreas width or height accordingly   */
	  warpx = -1;
	  warpy = -1;
	  if (dl != 0)
	    {
	    if (xmin == xmax)
	      /* X moves frozen */
	      dl = 0;
	    else if (ref_v == C_VCENTER && *width + (2 * dl * hDirection) < 0)
	      {
		dl = -DO_ALIGN (*width / 2) * hDirection;
		warpx = xm + (dl * hDirection);
	      }
	    else if (*width + (dl * hDirection) < 0)
	      {
		dl = -DO_ALIGN (*width) * hDirection;
		warpx = xm + (dl * hDirection);
	      }
	    }

	  if (dh != 0)
	    {
	    if (ymin == ymax)
	      /* Y moves frozen */
	      dh = 0;
	    else if (ref_h == C_HCENTER && *height + (2 * dh * vDirection) < 0)
	      {
		dh = -DO_ALIGN (*height / 2) * vDirection;
		warpy = ym + (dh * vDirection);
	      }
	    else if (*height + dh < 0)
	      {
		dh = -DO_ALIGN (*height) * vDirection;
		warpy = ym + (dh * vDirection);
	      }
	    }

	  /* Compute the horizontal move of the origin */
	  if (dl != 0)
	    {
	      dl = dl * hDirection;		/* Take care for direction */
	      if (ref_v == C_VCENTER)
		{
		  dx = xmin + DO_ALIGN (*x - (dl / 2) - xmin) - *x;
		  /* Check the move is within limits */
		  if (*x + dx < xmin)
		    dx = xmin -* x;		/*left side */
		  if (*x + *width - dx > xmax)
		    dx = *x + *width - xmin - DO_ALIGN (xmax - xmin); /*right side */
		  
		  /* modify width for real */
		  dl = -(dx * 2);
		  if (dx != 0)
		    warpx = xm - (dx * hDirection);
		}
	      else if (ref_v == C_RIGHT)
		{
		  dx = xmin + DO_ALIGN (*x - dl - xmin) - *x;
		  /* Check the move is within limits */
		  if (*x + dx < xmin)
		    dx = xmin - *x;		/*left side */

		  /* modify width for real */
		  dl = -dx;
		  if (dx != 0)
		    warpx = xm + dx;
		}
	      else
		{
		  dx = 0;
		  dl = xmin + DO_ALIGN (*x + *width + dl - xmin) - *x - *width;
		  if (*x + *width + dl > xmax)
		    dl = xmin + DO_ALIGN (xmax - xmin) - *x - *width; /*right side */
		  if (dl != 0)
		    warpx = xm + dl;
		}
	    }
	  else
	    dx = 0;

	  /* Compute vertical move */
	  if (dh != 0)
	    {
	      dh = dh * vDirection;	/* Take care for direction */
	      if (ref_h == C_HCENTER)
		{
		  dy = ymin + DO_ALIGN (*y - (dh / 2) - ymin) - *y;
		  /* Check the move is within limits */
		  if (*y + dy < ymin)
		    dy = ymin - *y;		/*upper border */
		  if (*y + *height - dy > ymax)
		    dy = *y + *height - ymin - DO_ALIGN (ymax - ymin);	/*bottom */
		  /* change the height for real */
		  dh = -(dy * 2);
		  if (dy != 0)
		    warpy = ym - (dy * vDirection);
		}
	      else if (ref_h == C_BOTTOM)
		{
		  dy = ymin + DO_ALIGN (*y - dh - ymin) - *y;
		  /* Check the move is within limits */
		  if (*y + dy < ymin)
		    dy = ymin - *y;		/*upper border */
		  /* change the height for real */
		  dh = -dy;
		  if (dy != 0)
		    warpy = ym + dy;
		}
	      else
		{
		  dy = 0;
		  dh = ymin + DO_ALIGN (*y + *height + dh - ymin) - *y - *height;
		  if (*y + *height + dh > ymax)
		    dh = ymin + DO_ALIGN (ymax - ymin) - *y - *height;	/*bottom */
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
	      if (isEllipse)
		InvertEllipse (frame, *x, *y, *width, *height, *x + xref, *y + yref);
	      else
		BoxGeometry (frame, *x, *y, *width, *height, *x + xref, *y + yref);
	      /* is there any dependence between height and width */
	      *width = *width + dl;
	      *height = *height + dh;
	      if (percentW != 0)
		*width = *height * percentW / 100;
	      else if (percentH != 0)
		*height = *width * percentH / 100;
	      *x = *x + dx;
	      *y = *y + dy;
	      /* switch on the new one */
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
		  break;
		default:
		  yref = 0;
		  break;
		}  
	      if (isEllipse)
		InvertEllipse (frame, *x, *y, *width, *height, *x + xref, *y + yref);
	      else
		BoxGeometry (frame, *x, *y, *width, *height, *x + xref, *y + yref);
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
#endif /* _MOTIF */

#ifdef _GTK
      event = gdk_event_get ();
      if (event)
	{
	  if (event->type == GDK_MOTION_NOTIFY)	
	    {
	      /* We take only last position update */
	      while (gdk_events_pending ()) 
		{
		  event_tmp = gdk_event_get ();
		  if (event_tmp)
		    event = event_tmp;
		}
	      if (event->type == GDK_MOTION_NOTIFY) 
		{
		  /* Check for window ID */
		  if (((GdkEventMotion *)event)->window == w)
		    {
		      /* compute the deltas */
		      dl = (int) ((GdkEventMotion *)event)->x - xm;
		      dh = (int) ((GdkEventMotion *)event)->y - ym;
		      if (percentW != 0)
			{
			  /* keep the greater value */
			  if (dl < dh)
			    dl = dh;
			  dh = dl;
			}
		      else if (percentH != 0)
			{
			  /* keep the greater value */
			  if (dh < dl)
			    dh = dl;
			  dl = dh;
			}
		    }
		  else
		    dl = dh = 0;

		  /* Check that size can be modified, and stay >= 0    */
		  /* depending on base point and cursor position,      */
		  /* increase or decreas width or height accordingly   */
		  warpx = -1;
		  warpy = -1;
		  if (dl != 0)
		    {
		      if (xmin == xmax)
			/* X moves frozen */
			dl = 0;
		      else if (ref_v == C_VCENTER && *width + (2 * dl * hDirection) < 0)
			{
			  dl = -DO_ALIGN (*width / 2) * hDirection;
			  warpx = xm + (dl * hDirection);
			}
		      else if (*width + (dl * hDirection) < 0)
			{
			  dl = -DO_ALIGN (*width) * hDirection;
			  warpx = xm + (dl * hDirection);
			}
		    }

		  if (dh != 0)
		    {
		      if (ymin == ymax)
			/* Y moves frozen */
			dh = 0;
		      else if (ref_h == C_HCENTER && *height + (2 * dh * vDirection) < 0)
			{
			  dh = -DO_ALIGN (*height / 2) * vDirection;
			  warpy = ym + (dh * vDirection);
			}
		      else if (*height + dh < 0)
			{
			  dh = -DO_ALIGN (*height) * vDirection;
			  warpy = ym + (dh * vDirection);
			}
		    }

		  /* Compute the horizontal move of the origin */
		  if (dl != 0)
		    {
		      dl = dl * hDirection;		/* Take care for direction */
		      if (ref_v == C_VCENTER)
			{
			  dx = xmin + DO_ALIGN (*x - (dl / 2) - xmin) - *x;
			  /* Check the move is within limits */
			  if (*x + dx < xmin)
			    dx = xmin -* x;		/*left side */
			  if (*x + *width - dx > xmax)
			    dx = *x + *width - xmin - DO_ALIGN (xmax - xmin); /*right side */
		  
			  /* modify width for real */
			  dl = -(dx * 2);
			  if (dx != 0)
			    warpx = xm - (dx * hDirection);
			}
		      else if (ref_v == C_RIGHT)
			{
			  dx = xmin + DO_ALIGN (*x - dl - xmin) - *x;
			  /* Check the move is within limits */
			  if (*x + dx < xmin)
			    dx = xmin - *x;		/*left side */

			  /* modify width for real */
			  dl = -dx;
			  if (dx != 0)
			    warpx = xm + dx;
			}
		      else
			{
			  dx = 0;
			  dl = xmin + DO_ALIGN (*x + *width + dl - xmin) - *x - *width;
			  if (*x + *width + dl > xmax)
			    dl = xmin + DO_ALIGN (xmax - xmin) - *x - *width; /*right side */
			  if (dl != 0)
			    warpx = xm + dl;
			}
		    }
		  else
		    dx = 0;

		  /* Compute vertical move */
		  if (dh != 0)
		    {
		      dh = dh * vDirection;	/* Take care for direction */
		      if (ref_h == C_HCENTER)
			{
			  dy = ymin + DO_ALIGN (*y - (dh / 2) - ymin) - *y;
			  /* Check the move is within limits */
			  if (*y + dy < ymin)
			    dy = ymin - *y;		/*upper border */
			  if (*y + *height - dy > ymax)
			    dy = *y + *height - ymin - DO_ALIGN (ymax - ymin);	/*bottom */
			  /* change the height for real */
			  dh = -(dy * 2);
			  if (dy != 0)
			    warpy = ym - (dy * vDirection);
			}
		      else if (ref_h == C_BOTTOM)
			{
			  dy = ymin + DO_ALIGN (*y - dh - ymin) - *y;
			  /* Check the move is within limits */
			  if (*y + dy < ymin)
			    dy = ymin - *y;		/*upper border */
			  /* change the height for real */
			  dh = -dy;
			  if (dy != 0)
			    warpy = ym + dy;
			}
		      else
			{
			  dy = 0;
			  dh = ymin + DO_ALIGN (*y + *height + dh - ymin) - *y - *height;
			  if (*y + *height + dh > ymax)
			    dh = ymin + DO_ALIGN (ymax - ymin) - *y - *height;	/*bottom */
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
#ifndef _GL
		      if (isEllipse)
			InvertEllipse (frame, *x, *y, *width, *height, *x + xref, *y + yref);
		      else
			BoxGeometry (frame, *x, *y, *width, *height, *x + xref, *y + yref);
#endif /* _GL */
		      /* is there any dependence between height and width */
		      *width = *width + dl;
		      *height = *height + dh;
		      if (percentW != 0)
			*width = *height * percentW / 100;
		      else if (percentH != 0)
			*height = *width * percentH / 100;
		      *x = *x + dx;
		      *y = *y + dy;
		      /* switch on the new one */
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
			  break;
			default:
			  yref = 0;
			  break;
			}  
#ifndef _GL
		      if (isEllipse)
			InvertEllipse (frame, *x, *y, *width, *height, *x + xref, *y + yref);
		      else
			BoxGeometry (frame, *x, *y, *width, *height, *x + xref, *y + yref);
#else /* _GL */
		      DefRegion (frame,
				 box->BxClipX-1, box->BxClipY-1, 
				 box->BxClipW, box->BxClipH+2);

		      if (percentW)
			NewDimension (box->BxAbstractBox, 0, *height, frame, TRUE);
		      else if (percentH)
			NewDimension (box->BxAbstractBox, *width, 0, frame, TRUE);
		      else
			NewDimension (box->BxAbstractBox, *width, *height, frame, TRUE);

		      FrameTable[frame].DblBuffNeedSwap = TRUE;
		      GL_Swap (frame);
#endif /* _GL */
		    }

		  /* Should we move the cursor */
		  if (warpx >= 0 || warpy >= 0)
		    {
		      if (warpx >= 0)
			xm = warpx;
		      if (warpy >= 0)
			ym = warpy; 
	      
		      xwindow = (GdkWindowPrivate*) w;
		      XWarpPointer (GDK_DISPLAY(), 
				    None, 
				    xwindow->xwindow,
				    0, 0, 0, 0, xm, ym);
		    }
		}
	    }
	  if (event->type ==  GDK_BUTTON_RELEASE)
	    ret = 1;

	}
#endif /* !_GTK */
      
    }

#ifndef _GL
  /* Erase the box drawing */
  if (isEllipse)
    InvertEllipse (frame, *x, *y, *width, *height, *x + xref, *y + yref);
  else
    BoxGeometry (frame, *x, *y, *width, *height, *x + xref, *y + yref);
#endif /* _GL */
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
  ThotWindow       w;
#ifdef _WINDOWS
  POINT            cursorPos;
#endif /* _WINDOWS */

#if defined (_MOTIF) || defined(_GTK)  
  int              e;
#endif /* #if defined (_MOTIF) || defined(_GTK) */

  /* select the correct cursor */
  w = FrRef[frame];
#ifdef _WINDOWS
#ifndef _GL
  Gdc = GetDC (w);
#endif /*_GL*/
  SetCursor (LoadCursor (NULL, IDC_CROSS));
  GetCursorPos (&cursorPos);
  xm = cursorPos.x;
  ym = cursorPos.y;
  Resizing (frame, &x, &y, width, height, box, xmin, xmax, ymin, ymax, xm, ym,
	  percentW, percentH);
  /* Erase the box drawing */
  SetCursor (LoadCursor (NULL, IDC_ARROW));
#ifndef _GL
  ReleaseDC (w, Gdc);
#endif /*_GL*/
#endif /* _WINDOWS */
  
#ifdef _MOTIF
  e = ButtonPressMask | ButtonReleaseMask | ButtonMotionMask;
  if (xmin == xmax)
    ThotGrab (w, VCurs, e, 0);
  else if (ymin == ymax)
    ThotGrab (w, HCurs, e, 0);
  else
    ThotGrab (w, HVCurs, e, 0);
#endif /* _MOTIF */

#ifdef _GTK
 e = GDK_BUTTON_MOTION_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK;
 if (xmin == xmax)
    ThotGrab (w, VCurs, e, 0);
  else if (ymin == ymax)
    ThotGrab (w, HCurs, e, 0);
  else
    ThotGrab (w, HVCurs, e, 0);
  Resizing (frame, &x, &y, width, height, box, xmin, xmax, ymin, ymax, xm, ym,
	  percentW, percentH);
  /* restore the previous state of the Thot Library */
  ThotUngrab ();
#endif /*_GTK*/
  
#ifdef _MOTIF
  Resizing (frame, &x, &y, width, height, box, xmin, xmax, ymin, ymax, xm, ym,
	  percentW, percentH);
  /* restore the previous state of the Thot Library */
  ThotUngrab ();

  XFlush (TtDisplay);
#endif /* _MOTIF */

}


/*----------------------------------------------------------------------
  Moving
  This function returns the new position (x, y) of the reference point of the box.
  ----------------------------------------------------------------------*/
static void Moving (int frame, int *x, int *y, int width, int height,
		    PtrBox box, int xmin, int xmax, int ymin, int ymax,
		    int xm, int ym)
{
#if defined(_MOTIF) || defined(_WINDOWS)
   ThotEvent           event;
#endif /* #if defined(_MOTIF) || defined(_WINDOWS) */

#ifdef _GTK
   ThotEvent           *event_tmp;
   ThotEvent           *event;
   GdkWindowPrivate    *xwindow;
#endif /*_GTK*/

   ThotWindow          w;
   PtrAbstractBox      pAb;
   int                 ret, dx, dy, newx, newy;
   int                 warpx, warpy;
   int                 xref, yref;
#ifdef _WINDOWS
   POINT               cursorPos;
#endif /* _WINDOWS */
  ThotBool           isEllipse;

  pAb = box->BxAbstractBox;
  if (pAb && pAb->AbLeafType == LtCompound)
    pAb = pAb->AbFirstEnclosed;
  isEllipse = (pAb &&
	       pAb->AbLeafType == LtGraphics &&
	       (pAb->AbShape == 'a' || pAb->AbShape == 'c'));
   /* Pick the correct cursor */
   w = FrRef[frame];
  /* draw the current box geometry */
  switch (box->BxHorizEdge)
    {
    case Right:
      xref = width;
      break;
    case VertMiddle:
      xref = width / 2;
      break;
    case VertRef:
      xref = box->BxVertRef;
      break;
    default:
      xref = 0;
      break;
    }
  switch (box->BxVertEdge)
    {
    case Bottom:
      yref = height;
      break;
    case HorizMiddle:
      yref = height / 2;
      break;
    case HorizRef:
      yref = box->BxHorizRef;
      break;
    default:
      yref = 0;
      break;
    }  
  if (isEllipse)
    InvertEllipse (frame, *x, *y, width, height, *x + xref, *y + yref);
  else
    BoxGeometry (frame, *x, *y, width, height, *x + xref, *y + yref);
  /* Loop on user interraction */
  ret = 0;
  while (ret == 0)
    {
#ifdef _WINDOWS
      /*GetMessage (&event, NULL, 0, 0);*/
	  GetMessage (&event, NULL, 0, 0); // force unicode version
      switch (event.message)
	{
	case WM_LBUTTONUP:
	  ret = 1;
	  break;

	case WM_MOUSEMOVE:
	  GetCursorPos (&cursorPos);
	  ScreenToClient (w, &cursorPos);
	  /* compute the new box origin */
	  newx = *x + cursorPos.x - xm;
	  dx = xmin + DO_ALIGN (newx - xmin) - *x;
	  newy = *y + cursorPos.y - ym;
	  dy = ymin + DO_ALIGN (newy - ymin) - *y;
	  newx = dx + *x;
	  newy = dy + *y;
	  /* Checks for limits */
	  warpx = -1;
	  warpy = -1;
	  if (xmin == xmax)
	    {
	      newx = xmin; /*left side */
	      warpx = xm;
	    }
	  else if (newx < xmin)
	    {
	      newx = xmin; /*left side */
	      warpx = xm;
	    }
	  else if (newx + width > xmax)
	    {
	      if (xmin + width > xmax)
		{
		  newx = xmin; /*overflow on left side */
		  warpx = xm;
		}
	      else
		{
		  newx = xmin + DO_ALIGN (xmax - width - xmin); /*cote droit */
		  warpx = xm + newx - *x;
		}
	    }
	  else
	    xm += dx; /* New cursor location */

	  dx = newx - *x;
	  if (ymin == ymax)
	    {
	      newy = ymin; /*upper border */
	      warpy = ym;
	    }
	  else if (newy < ymin)
	    {
	      newy = ymin; /*upper border */
	      warpy = ym;
	    }
	  else if (newy + height > ymax)
	    {
	      if (ymin + height > ymax)
		{
		  newy = ymin; /*overflow on upper border */
		  warpy = ym;
		}
	      else
		{
		  newy = ymin + DO_ALIGN (ymax - height - ymin); /*cote inferieur */
		  warpy = ym + newy - *y;
		}
	    }
	  else
	    ym += dy; /* New cursor location */
	  dy = newy - *y;

	  /* Should we move the box */
	  if ((dx != 0) || (dy != 0))
	    {
	      if (isEllipse)
		{
		  /* old box */
		  InvertEllipse (frame, *x, *y, width, height, *x + xref, *y + yref);
		  /* new box */
		  InvertEllipse (frame, newx, newy, width, height, newx + xref, newy + yref);
		}
	      else
		{
		  /* old box */
		  BoxGeometry (frame, *x, *y, width, height, *x + xref, *y + yref);
		  /* new box */
		  BoxGeometry (frame, newx, newy, width, height, newx + xref, newy + yref);
		}
	      *x = newx;
	      *y = newy;
	    }

	  /* Should we move the cursor */
	  if (warpx >= 0 || warpy >= 0)
	    {
	      if (warpx >= 0)
		xm = warpx;
	      if (warpy >= 0)
		ym = warpy;
	    }
	  break;

	case WM_PAINT:
	  WIN_HandleExpose (w, frame, event.wParam, event.lParam);
	default: break;
	}
#endif /* !_WINDOWS */

#ifdef _MOTIF
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
	      newx = *x + (int) event.xmotion.x - xm;
	      dx = xmin + DO_ALIGN (newx - xmin) - *x;
	      newy = *y + (int) event.xmotion.y - ym;
	      dy = ymin + DO_ALIGN (newy - ymin) - *y;
	    }
	  else
	    {
	      dx = dy = 0;
	    }
	  
	  newx = dx + *x;
	  newy = dy + *y;
	  
	  /* Checks for limits */
	  warpx = -1;
	  warpy = -1;
	  if (xmin == xmax)
	    {
	      newx = xmin;	/*left side */
	      warpx = xm;
	    }
	  else if (newx < xmin)
	    {
	      newx = xmin;	/*left side */
	      warpx = xm;
	    }
	  else if (newx + width > xmax)
	    {
	      if (xmin + width > xmax)
		{
		  newx = xmin;	/*overflow on left side */
		  warpx = xm;
		}
	      else
		{
		  newx = xmin + DO_ALIGN (xmax - width - xmin); /*cote droit */
		  warpx = xm + newx - *x;
		}
	    }
	  else
	    xm += dx;	/* New cursor location */
	  
	  dx = newx - *x;
	  if (ymin == ymax)
	    {
	      newy = ymin;	/*upper border */
	      warpy = ym;
	    }
	  else if (newy < ymin)
	    {
	      newy = ymin;	/*upper border */
	      warpy = ym;
	    }
	  else if (newy + height > ymax)
	    {
	      if (ymin + height > ymax)
		{
		  newy = ymin;	/*overflow on upper border */
		  warpy = ym;
		}
	      else
		{
		  newy = ymin + DO_ALIGN (ymax - height - ymin);  /* bottom border */
		  warpy = ym + newy - *y;
		}
	    }
	  else
	    ym += dy;	/* New cursor location */
	  dy = newy - *y;
	  
	  /* Should we move the box */
	  if ((dx != 0) || (dy != 0))
	    {
	      if (isEllipse)
		{
		  /* old box */
		  InvertEllipse (frame, *x, *y, width, height, *x + xref, *y + yref);
		  /* new box */
		  InvertEllipse (frame, newx, newy, width, height, newx + xref, newy + yref);
		}
	      else
		{
		  /* old box */
		  BoxGeometry (frame, *x, *y, width, height, *x + xref, *y + yref);
		  /* new box */
		  BoxGeometry (frame, newx, newy, width, height, newx + xref, newy + yref);
		}
	      XFlush (TtDisplay);
	      *x = newx;
	      *y = newy;
	      
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
#endif /* #ifdef _MOTIF */
      
#ifdef _GTK
      event = gdk_event_get ();
      if (event)
	{
	  if (event->type == GDK_MOTION_NOTIFY) 
	    {	

	      /* We take only last position update */
	      while (gdk_events_pending ()) 
		{
		  event_tmp = gdk_event_get ();
		  if (event_tmp)
		    event = event_tmp;
		}

	      if (event->type == GDK_MOTION_NOTIFY) 
		{	
		  /* Check for window ID */
		  if (((GdkEventMotion *)event)->window == w)
		    {
		      /* compute the new box origin */
		      newx = *x + (int) ((GdkEventMotion *)event)->x - xm;
		      dx = xmin + DO_ALIGN (newx - xmin) - *x;
		      newy = *y + (int)  ((GdkEventMotion *)event)->y - ym;
		      dy = ymin + DO_ALIGN (newy - ymin) - *y;
		    }
		  else
		    {
		      dx = dy = 0;
		    }	  
		  newx = dx + *x;
		  newy = dy + *y;
	    
		  /* Checks for limits */
		  warpx = -1;
		  warpy = -1;
		  if (xmin == xmax)
		    {
		      newx = xmin;	/*left side */
		      warpx = xm;
		    }
		  else if (newx < xmin)
		    {
		      newx = xmin;	/*left side */
		      warpx = xm;
		    }
		  else if (newx + width > xmax)
		    {
		      if (xmin + width > xmax)
			{
			  newx = xmin;	/*overflow on left side */
			  warpx = xm;
			}
		      else
			{
			  newx = xmin + DO_ALIGN (xmax - width - xmin); /*cote droit */
			  warpx = xm + newx - *x;
			}
		    }
		  else
		    xm += dx;	/* New cursor location */
	    
		  dx = newx - *x;
		  if (ymin == ymax)
		    {
		      newy = ymin;	/*upper border */
		      warpy = ym;
		    }
		  else if (newy < ymin)
		    {
		      newy = ymin;	/*upper border */
		      warpy = ym;
		    }
		  else if (newy + height > ymax)
		    {
		      if (ymin + height > ymax)
			{
			  newy = ymin;	/*overflow on upper border */
			  warpy = ym;
			}
		      else
			{
			  newy = ymin + DO_ALIGN (ymax - height - ymin);  /* bottom border */
			  warpy = ym + newy - *y;
			}
		    }
		  else
		    ym += dy;	/* New cursor location */
		  dy = newy - *y;
	    
		  /* Should we move the box */
		  if ((dx != 0) || (dy != 0))
		    {
#ifndef _GL
		      if (isEllipse)
			{
			  /* old box */
			  InvertEllipse (frame, *x, *y, width, height, *x + xref, *y + yref);
			  /* new box */
			  InvertEllipse (frame, newx, newy, width, height, newx + xref, newy + yref);
			}
		      else
			{
			  /* old box */
			  BoxGeometry (frame, *x, *y, width, height, *x + xref, *y + yref);
			  /* new box */
			  BoxGeometry (frame, newx, newy, width, height, newx + xref, newy + yref);
			}
#else /* _GL */
		    DefRegion (frame,
			       box->BxClipX-1, box->BxClipY-1, 
			       box->BxClipW, box->BxClipH+2);

		    NewPosition (box->BxAbstractBox, 
				 newx + (ViewFrameTable[frame - 1]).FrXOrg, xref, 
				 newy + (ViewFrameTable[frame - 1]).FrYOrg, yref, 
				 frame, TRUE);

		    FrameTable[frame].DblBuffNeedSwap = TRUE;
		    GL_Swap (frame);
#endif /* _GL */

		      *x = newx;
		      *y = newy;
		    }
	    
		  /* Should we move the cursor */
		  if (warpx >= 0 || warpy >= 0)
		    {
		      if (warpx >= 0)
			xm = warpx;
		      if (warpy >= 0)
			ym = warpy;

		      xwindow = (GdkWindowPrivate*) w;
		      XWarpPointer (GDK_DISPLAY(), 
				    None, 
				    xwindow->xwindow,
				    0, 0, 0, 0, xm, ym);
		    }
		  }
		}
	  if (event->type ==  GDK_BUTTON_RELEASE)
	    ret = 1;
	}
#endif /* _GTK */
    }

  /* erase the box drawing */
  if (isEllipse)
    InvertEllipse (frame, *x, *y, width, height, *x + xref, *y + yref);
  else
    BoxGeometry (frame, *x, *y, width, height, *x + xref, *y + yref);
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
   ThotWindow          w;
#if defined(_MOTIF) || defined(_GTK)
   int                 e;
#endif /* #if defined(_MOTIF) || defined(_GTK) */

  /* Pick the correct cursor */
  w = FrRef[frame];

#ifdef _WINDOWS
#ifndef _GL
  Gdc = GetDC (w);
#endif /*_GL*/
  SetCursor (LoadCursor (NULL, IDC_CROSS));
  Moving (frame, x, y, width, height, box, xmin, xmax, ymin, ymax, xm, ym);
  SetCursor (LoadCursor (NULL, IDC_ARROW));
#ifndef _GL
  ReleaseDC (w, Gdc);
#endif _GL
#endif /* !_WINDOWS */

#ifdef _MOTIF
  e = ButtonPressMask | ButtonReleaseMask | ButtonMotionMask;
  if ((xmin >= *x) && (xmax <= *x + width))
    ThotGrab (w, VCurs, e, 0);
  else if ((ymin >= *y) && (ymax <= *y + height))
    ThotGrab (w, HCurs, e, 0);
  else
    ThotGrab (w, HVCurs, e, 0);
  Moving (frame, x, y, width, height, box, xmin, xmax, ymin, ymax, xm, ym);
   /* restore the Thot Library state */
  ThotUngrab ();
#endif /* #ifdef _MOTIF */
  
#ifdef _GTK
 e = GDK_BUTTON_MOTION_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK;
 if ((xmin >= *x) && (xmax <= *x + width))
    ThotGrab (w, VCurs, e, 0);
  else if ((ymin >= *y) && (ymax <= *y + height))
    ThotGrab (w, HCurs, e, 0);
  else
    ThotGrab (w, HVCurs, e, 0);
  Moving (frame, x, y, width, height, box, xmin, xmax, ymin, ymax, xm, ym);
   /* restore the Thot Library state */
  ThotUngrab ();
#endif /*_GTK*/

#ifdef _MOTIF
  XFlush (TtDisplay);
#endif /* _MOTIF */

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
#if defined(_WINDOWS) || defined(_MOTIF)
   ThotEvent           event;
#endif /*#if defined(_WINDOWS) || defined(_MOTIF) */

#ifdef _GTK
   ThotEvent           *event;
   GdkWindowPrivate    *xwindow, *xwindow2;
#endif /*_GTK*/

   PtrAbstractBox      pAb;
  int                 xm, ym;
  int                 dx, dy;
  int                 ret;
  int                 newx, newy;
  int                 xref, yref;
#ifdef _WINDOWS
  RECT                rect;
  POINT               cursorPos;
  HCURSOR             cross;
#endif /* _WINDOWS */

#if defined(_GTK) || defined(_MOTIF)  
  ThotWindow          wdum;
  int                 e, f;
#endif /* #if defined(_GTK) || defined(_MOTIF)   */
  
  ThotBool           isEllipse;

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
  w = FrRef[frame];

#ifdef _WINDOWS
  cross = LoadCursor (NULL, IDC_CROSS);
#ifndef _GL
  Gdc = GetDC (w);
#endif /*_GL*/
  GetWindowRect (w, &rect);
  SetCursorPos (*x + rect.left, *y + rect.top);
#endif /* !_WINDOWS */

#ifdef _MOTIF
  e = ButtonPressMask | ButtonReleaseMask | ButtonMotionMask;
  ThotGrab (w, HVCurs, e, 0);
  XMapRaised (TtDisplay, w);
  XWarpPointer (TtDisplay, None, w, 0, 0, 0, 0, *x, *y);
  XFlush (TtDisplay);
#endif /* _MOTIF*/
  
#ifdef _GTK
  e = GDK_BUTTON_MOTION_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK;
  ThotGrab (w, HVCurs, e, 0);
#endif /* !_GTK */

  if (isEllipse)
    InvertEllipse (frame, *x, *y, *width, *height, *x + xref, *y + yref);
  else
    BoxGeometry (frame, *x, *y, *width, *height, *x + xref, *y + yref);
  /* Loop on user input to keep the first point */
  ret = 0;
  while (ret == 0)
    {
#ifdef _WINDOWS
     /*GetMessage (&event, NULL, 0, 0);*/
	 GetMessage (&event, NULL, 0, 0); // force unicode version
	 SetCursor (cross);
      if (event.message == WM_MOUSEMOVE)
	{
	  GetCursorPos (&cursorPos);
	  /* check the coordinates are withing limits */
	  newx = DO_ALIGN (cursorPos.x - xmin);
	  newx += xmin;
	  newy = DO_ALIGN (cursorPos.y - ymin);
	  newy += ymin;
	  if (newx - rect.left < xmin || newx - rect.left > xmax ||
	      newy - rect.top < ymin || newy - rect.top > ymax)
            SetCursorPos (*x + rect.left, *y + rect.top);
	  else if ((newx != *x && PosX) || (newy != *y && PosY))
	    {
	      if (isEllipse)
		InvertEllipse (frame, *x, *y, *width, *height, *x + xref, *y + yref);
	      else
	      BoxGeometry (frame, *x, *y, *width, *height, *x + xref, *y + yref);
	      if (PosX)
		*x = newx - rect.left;
	      if (PosY)
		*y = newy - rect.top;
	      
	      if (isEllipse)
		InvertEllipse (frame, *x, *y, *width, *height, *x + xref, *y + yref);
	      else
		BoxGeometry (frame, *x, *y, *width, *height, *x + xref, *y + yref);
	      /* the postion is fixed */
	      if (!PosX || !PosY)
		SetCursorPos (*x + rect.left, *y + rect.top);
	    }
	}
      else
	{
	  switch (event.message)
	    {
	    case WM_LBUTTONDOWN:
	    case WM_MBUTTONDOWN:
	    case WM_RBUTTONDOWN:	  
	      GetCursorPos (&cursorPos);
	      if (PosX)
		xm = xmin + DO_ALIGN ((int) cursorPos.x - xmin);
	      else
		xm = *x;
	      if (PosY)
		ym = ymin + DO_ALIGN ((int) cursorPos.y - ymin);
	      else
		ym = *y;
	      
	      /* check the coordinates */
	      if ((xm - rect.left) < xmin || (xm - rect.left) > xmax || !PosX ||
		  (ym - rect.top) < ymin || (ym - rect.top) > ymax || !PosY)
		{
		  SetCursorPos (xm + rect.left, ym + rect.top);
		  if (!PosX && !PosY)
		    ret = 1;
		}
	      else
		ret = 1;
	      break; 
	      
		case WM_PAINT:
		  WIN_HandleExpose (w, frame, event.wParam, event.lParam);
	    default: break;
	    } 
	} 
#endif /* !_WINDOWS */

#ifdef _MOTIF
      if (XPending (TtDisplay) == 0)
	{
	  /* get the cursor location */
	  XQueryPointer (TtDisplay, w, &wdum, &wdum, &dx, &dy, &newx, &newy, &e);
	  /* check the coordinates are withing limits */
	  newx = DO_ALIGN (newx - xmin);
	  newx += xmin;
	  newy = DO_ALIGN (newy - ymin);
	  newy += ymin;
	  if (newx < xmin || newx > xmax || newy < ymin || newy > ymax)
	    XWarpPointer (TtDisplay, None, w, 0, 0, 0, 0, *x, *y);
	  else if ((newx != *x && PosX) || (newy != *y && PosY))
	    {
	      if (isEllipse)
		InvertEllipse (frame, *x, *y, *width, *height, *x + xref, *y + yref);
	      else
		BoxGeometry (frame, *x, *y, *width, *height, *x + xref, *y + yref);
	      if (PosX)
		*x = newx;
	      if (PosY)
		*y = newy;

	      if (isEllipse)
		InvertEllipse (frame, *x, *y, *width, *height, *x + xref, *y + yref);
	      else
		BoxGeometry (frame, *x, *y, *width, *height, *x + xref, *y + yref);
	      XFlush (TtDisplay);
	      /* the postion is fixed */
	      if (!PosX || !PosY)
		XWarpPointer (TtDisplay, None, w, 0, 0, 0, 0, *x, *y);
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
		ym = ymin + DO_ALIGN ((int) event.xmotion.y - ymin);
	      else
		ym = *y;
	    
	      /* check the coordinates */
	      if (xm < xmin || xm > xmax ||
		  !PosX ||
		  ym < ymin || ym > ymax || !PosY)
		{
		  XWarpPointer (TtDisplay, None, w, 0, 0, 0, 0, xm, ym);
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
#endif /*#ifdef _MOTIF*/

#ifdef _GTK
   if (gdk_events_pending () == 0)
	{ 
	  /* current pointer position */
	  xwindow = (GdkWindowPrivate*) w;
	  xwindow2 = (GdkWindowPrivate*) wdum;
	  xwindow2 = TtaGetMemory (sizeof(GdkWindowPrivate*));
	  XQueryPointer (GDK_DISPLAY(), 
			 xwindow->xwindow, 
			 &(xwindow2->xwindow), 
			 &(xwindow2->xwindow), 
			 &dx, &dy, 
			 &newx, &newy, 
			 &e);
	  TtaFreeMemory (xwindow2);
	  /* coordinate in limits */
	   newx = DO_ALIGN (newx - xmin);
	  newx += xmin;
	  newy = DO_ALIGN (newy - ymin);
	  newy += ymin;

	  if (newx < xmin || newx > xmax || newy < ymin || newy > ymax)
	    {	      
	      xwindow = (GdkWindowPrivate*) w;
	      XWarpPointer (GDK_DISPLAY(), 
			    None, 
			    xwindow->xwindow,
			    0, 0, 0, 0, *x, *y);
	    }
	  else if ((newx != *x && PosX) || (newy != *y && PosY))
	    {
	      if (isEllipse)
		InvertEllipse (frame, *x, *y, *width, *height, *x + xref, *y + yref);
	      else
		BoxGeometry (frame, *x, *y, *width, *height, *x + xref, *y + yref);
	      if (PosX)
		*x = newx;
	      if (PosY)
		*y = newy;

	      if (isEllipse)
		InvertEllipse (frame, *x, *y, *width, *height, *x + xref, *y + yref);
	      else
		BoxGeometry (frame, *x, *y, *width, *height, *x + xref, *y + yref);
	      XFlush (TtDisplay);
	      /* the postion is fixed */
	      if (!PosX || !PosY)
		{
		  xwindow = (GdkWindowPrivate*) w;
		  XWarpPointer (GDK_DISPLAY(), 
				None, 
				xwindow->xwindow,
				0, 0, 0, 0, *x, *y);
		}
            }
	}
      else
	{
	  event = gdk_event_get ();
	  if (event)
	    {
	    /* we only deal with button press events */
	  switch (event->type)
	    {
	    case GDK_BUTTON_PRESS:
	      if (PosX)
		xm = xmin + DO_ALIGN ((int) ((GdkEventButton *)event)->x - xmin);
	      else
		xm = *x;
	      if (PosY)
		ym = ymin + DO_ALIGN ((int) ((GdkEventButton *)event)->y - ymin);
	      else
		ym = *y;
	    
	      /* check the coordinates */
	      if (xm < xmin || xm > xmax ||
		  !PosX ||
		  ym < ymin || ym > ymax || !PosY)
		{
		  xwindow = (GdkWindowPrivate*) w;
		  XWarpPointer (GDK_DISPLAY(), 
				None, 
				xwindow->xwindow,
				0, 0, 0, 0, xm, ym);
		  if (!PosX && !PosY)
		    ret = 1;
		}
	      else
		ret = 1;
	      break; 
	    case GDK_MOTION_NOTIFY:	  
	    /* Check for window ID */
	    if (((GdkEventMotion *)event)->window == w)
	      {
		newx = xmin + DO_ALIGN ((int) ((GdkEventMotion *)event)->x - xmin);
		newy = ymin + DO_ALIGN ((int)((GdkEventMotion *)event)->y - ymin);
	      }

	  case GDK_EXPOSE:
	    f = GetWindowFrame (((GdkEventExpose *)event)->window);
	    if (f <= MAX_FRAME + 1)
	      FrameToRedisplay (((GdkEventExpose *)event)->window, 
				f, 
				(GdkEventExpose *) & event);
	    /*XtDispatchEvent (&event);*/
	    break;
	  default: break;
	  }

#ifdef _GL
	    FrameTable[frame].DblBuffNeedSwap = TRUE;
	    GL_Swap (frame);
#endif /* _GL */
	    }

	}
#endif /* !_GTK */

    }

  /* switch off the old box geometry */
  if (isEllipse)
    InvertEllipse (frame, *x, *y, *width, *height, *x + xref, *y + yref);
  else
    BoxGeometry (frame, *x, *y, *width, *height, *x + xref, *y + yref);

#ifdef _WINDOWS 
  *x = xm - rect.left;
  *y = ym - rect.top;
  SetCursor (LoadCursor (NULL, IDC_CROSS));
#endif /* _WINDOWS */

#if defined(_GTK) || defined(_MOTIF)  
  *x = xm;
  *y = ym; 
#endif /* #if defined(_GTK) || defined(_MOTIF) */
  
  Resizing (frame, x, y, width, height, box, xmin, xmax, ymin, ymax, xm, ym, percentW, percentH);

#ifdef _WINDOWS 
#ifndef _GL
  ReleaseDC (w, Gdc);
#endif /*_GL*/
  SetCursor (LoadCursor (NULL, IDC_ARROW));
#endif /* _WINDOWS */

#if defined(_GTK) || defined(_MOTIF)  
  /* restore state of the Thot Library */
  ThotUngrab ();
#ifdef _MOTIF
  XFlush (TtDisplay);
#endif /* _MOTIF */
#endif /* #if defined(_GTK) || defined(_MOTIF) */
}

