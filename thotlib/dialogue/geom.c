/*
 *
 *  (c) COPYRIGHT INRIA, Grif, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 
/*
 * geom.c : boxes geometry handling, i.e. interracting with the user
 *          concerning a box position, or size.
 *
 * Authors: I. Vatton (INRIA)
 *          R. Guetari (W3C/INRIA): Windows NT/95
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
#include "windowdisplay_f.h"
#include "font_f.h"
#include "units_f.h"
#include "memory_f.h"
#include "content_f.h"

#ifdef _WINDOWS
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
     Polyline (hdc, &ptTab, 2) ;

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
static void         BoxGeometry (int frame, int x, int y, int width, int height, int xr, int yr, boolean withborder)
#else  /* __STDC__ */
static void         BoxGeometry (frame, x, y, width, height, xr, yr, withborder)
int                 frame;
int                 x;
int                 y;
int                 width;
int                 height;
int                 xr;
int                 yr;
boolean             withborder;
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
 *      RedrawPolyLine shows the current state of the polyline (closed or
 *		not) in a given frame.
 *	This function returns the coordinates of the following control points :
 *	- x1, y1 predecessor of point
 *      - x2, y2 point
 *      - x3, y3 successor of point
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         RedrawPolyLine (int frame, int x, int y, PtrTextBuffer buffer, int nb, int point, boolean close, int *x1, int *y1, int *x2, int *y2, int *x3, int *y3)
#else  /* __STDC__ */
static void         RedrawPolyLine (frame, x, y, buffer, nb, point, close, x1, y1, x2, y2, x3, y3)
int                 frame;
int                 x;
int                 y;
PtrTextBuffer       buffer;
int                 nb;
int                 point;
boolean             close;
int                *x1;
int                *y1;
int                *x2;
int                *y2;
int                *x3;
int                *y3;
#endif /* __STDC__ */

{
#ifndef _WINDOWS
   ThotPoint          *points;
   int                 i, j;
   PtrTextBuffer       adbuff;

   *x1 = *y1 = *x2 = *y2 = *x3 = *y3 = -1;
   /* allocate a table of points */
   points = (ThotPoint *) TtaGetMemory (sizeof (ThotPoint) * (nb));
   adbuff = buffer;
   j = 1;
   for (i = 1; i < nb; i++)
     {
	if (j >= adbuff->BuLength)
	  {
	     if (adbuff->BuNext != NULL)
	       {
		  /* change the buffer */
		  adbuff = adbuff->BuNext;
		  j = 0;
	       }
	  }
	points[i - 1].x = x + FrameTable[frame].FrLeftMargin + PointToPixel (adbuff->BuPoints[j].XCoord / 1000);
	points[i - 1].y = y + FrameTable[frame].FrTopMargin + PointToPixel (adbuff->BuPoints[j].YCoord / 1000);
	/* write down predecessor and sucessor of point */
	if (i == point - 1)
	  {
	     *x1 = points[i - 1].x;
	     *y1 = points[i - 1].y;
	  }
	else if (i == point)
	  {
	     *x2 = points[i - 1].x;
	     *y2 = points[i - 1].y;
	  }
	if (i == point + 1)
	  {
	     *x3 = points[i - 1].x;
	     *y3 = points[i - 1].y;
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
	XDrawLines (TtDisplay, FrRef[frame], TtInvertGC, points, nb, CoordModeOrigin);
     }
   else
      XDrawLines (TtDisplay, FrRef[frame], TtInvertGC, points, nb - 1, CoordModeOrigin);

   /* free the table of points */
   free ((char *) points);
#endif	/* _WINDOWS */
}

/*----------------------------------------------------------------------
 *      PolyLineCreation interract with the user to read the point forming
 * 		a polyline in a given frame.
 *		x and y values gives the position of the box in the frame.
 *		Pbuffer points to the first buffer of the abstract box, i.e.
 *		the list of control points modified. The first point in the
 *		list gives the maximum width and height of the polyline.
 *		This fonction update both  list of control points and
 *		returns the number of point in the polyline.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                 PolyLineCreation (int frame, int x, int y, PtrTextBuffer Pbuffer, PtrTextBuffer Bbuffer)
#else  /* __STDC__ */
int                 PolyLineCreation (frame, x, y, Pbuffer, Bbuffer)
int                 frame;
int                 x;
int                 y;
PtrTextBuffer       Pbuffer;
PtrTextBuffer       Bbuffer;
#endif /* __STDC__ */

{
   float               ratioX, ratioY;
   int                 width, height;
   int                 e, dx, dy;
   int                 ret, f;
   int                 newx, newy, lastx, lasty;
   int                 x1, y1, nbpoints;
   ThotWindow          w, wdum;
   ThotEvent           event;
#  ifdef _WINDOWS
   RECT  rect;
   POINT cursorPos;
#  endif /* _WINDOWS */

   /* box size */
   width = Bbuffer->BuPoints[0].XCoord;
   height = Bbuffer->BuPoints[0].YCoord;
   /* computes the trasformation foctors between the box and the abstract box */
   ratioX = (float) Pbuffer->BuPoints[0].XCoord / (float) width;
   ratioY = (float) Pbuffer->BuPoints[0].YCoord / (float) height;
   width = PointToPixel (width / 1000);
   height = PointToPixel (height / 1000);

   /* change the cursor */
   w = FrRef[frame];
#  ifdef _WINDOWS
   GetWindowRect (w, &rect);
#  else /* !_WINDOWS */
   e = ButtonPressMask | ButtonReleaseMask | ButtonMotionMask;
   XMapRaised (TtDisplay, w);
   XFlush (TtDisplay);
   ThotGrab (w, HVCurs, e, 0);
#  endif /* _WINDOWS */

   /* The grid stepping begins at the origin */
#  ifdef _WINDOWS
   lastx = x + rect.left; 
   lasty = y + rect.top;
#  else /* !_WINDOWS */
   lastx = x + FrameTable[frame].FrLeftMargin; 
   lasty = y + FrameTable[frame].FrTopMargin;
#  endif /* !_WINDOWS */
   x1 = -1;
   y1 = -1;
   nbpoints = 1;
#  ifdef _WINDOWS
   if (!SetCursorPos (lastx, lasty))
      WinErrorBox (FrRef[frame]);
#  else /* !_WINDOWS */
   XWarpPointer (TtDisplay, None, w, 0, 0, 0, 0, lastx, lasty);
   XFlush (TtDisplay);
#  endif /* _WINDOWS */

   /* shows up the box border */
   /*Clear(frame, width, height, x, y); */
   BoxGeometry (frame, x, y, width, height, x + width - 2, y + height - 2, TRUE);

   /* loop waiting for the user input */
   ret = 0;
   while (ret == 0) {
#        ifdef _WINDOWS
         SetCursor (LoadCursor (NULL, IDC_CROSS));
         ShowCursor (TRUE);
         GetMessage (&event, NULL, 0, 0);
#        if 0
         eventCopy = event;
         TtaHandleOneWindowEvent (&event);
         if (eventCopy.message == WM_LBUTTONDOWN || eventCopy.message == WM_MBUTTONDOWN || eventCopy.message == WM_RBUTTONDOWN) {
             newx = x + DO_ALIGN ((int) ClickX - FrameTable[frame].FrLeftMargin - x);
             newy = y + DO_ALIGN ((int) ClickY - FrameTable[frame].FrTopMargin - y);
             /* CHKR_LIMIT to size of the box */
             if (newx < x)
                lastx = x + FrameTable[frame].FrLeftMargin;	/* nouvelle position en X valide */
             else if (newx > x + width)
                  lastx = x + width + FrameTable[frame].FrLeftMargin;	/* nouvelle position en X valide */
             else
                  lastx = newx + FrameTable[frame].FrLeftMargin;

             if (newy < y)
                lasty = y + FrameTable[frame].FrTopMargin;	/* nouvelle position en Y valide */
             else if (newy > y + height)
                  lasty = y + height + FrameTable[frame].FrTopMargin;	/* nouvelle position en Y valide */
             else
                  lasty = newy + FrameTable[frame].FrTopMargin;

             x1 = lastx;
             y1 = lasty;
             nbpoints++;

             /* update the box buffer */
             newx = PixelToPoint (lastx - FrameTable[frame].FrLeftMargin - x) * 1000;
             newy = PixelToPoint (lasty - FrameTable[frame].FrTopMargin - y) * 1000;
             AddPointInPolyline (Bbuffer, nbpoints, newx, newy);
             /* update the abstract box buffer */
             newx = (int) ((float) newx * ratioX);
             newy = (int) ((float) newy * ratioY);
             AddPointInPolyline (Pbuffer, nbpoints, newx, newy);
             if (eventCopy.message == WM_MBUTTONDOWN || eventCopy.message == WM_RBUTTONDOWN) 
                ret = 1;
         } else if (eventCopy.message == WM_MOUSEMOVE) {
                newx = DO_ALIGN (X_Pos - FrameTable[frame].FrLeftMargin - x);
                newx += x;
                newy = DO_ALIGN (Y_Pos - FrameTable[frame].FrTopMargin - y);
                newy += y;
                if (newx < x || newx > x + width || newy < y || newy > y + height) {
                   /* CHKR_LIMIT to size of the box */
                   if (newx < x)
                      newx = x + FrameTable[frame].FrLeftMargin;		/* nouvelle position en X valide */
                   else if (newx > x + width)
                        newx = x + width + FrameTable[frame].FrLeftMargin;		/* nouvelle position en X valide */
                   else
                        newx += FrameTable[frame].FrLeftMargin;

                   if (newy < y)
                      newy = y + FrameTable[frame].FrTopMargin;	/* nouvelle position en Y valide */
                   else if (newy > y + height)
                        newy = y + height + FrameTable[frame].FrTopMargin;		/* nouvelle position en Y valide */
                   else
                        newy += FrameTable[frame].FrTopMargin;
                } else {
                       newx += FrameTable[frame].FrLeftMargin;
                       newy += FrameTable[frame].FrTopMargin;
                }

                /* refresh the display of teh two adjacent segments */
                if (nbpoints > 1 && x1 != -1 && (newx != lastx || newy != lasty)) {
                   POINT ptBeg;
				   POINT ptEnd;

				   ptBeg.x = x1;
				   ptBeg.y = y1;
				   ptEnd.x = lastx;
				   ptEnd.y = lasty;

				   DrawOutline(FrRef [frame], ptBeg, ptEnd);

				   ptBeg.x = x1;
				   ptBeg.y = y1;
				   ptEnd.x = newx;
				   ptEnd.y = newy;

				   DrawOutline(FrRef [frame], ptBeg, ptEnd);
                }
	            lastx = newx;
	            lasty = newy;
         }
#        endif /* 0 */
         if (event.message == WM_MOUSEMOVE) {
            GetCursorPos (&cursorPos);
            /* current pointer position */
            newx = cursorPos.x ;
			newy = cursorPos.y;
            /* coordinate checking */
            newx = DO_ALIGN (newx - FrameTable[frame].FrLeftMargin - x);
            newx += x;
            newy = DO_ALIGN (newy - FrameTable[frame].FrTopMargin - y);
            newy += y;
            if ((newx - rect.left) < x || (newx - rect.left) > x + width || (newy - rect.top) < y || (newy - rect.top) > y + height) {
               /* CHKR_LIMIT to size of the box */
               if (newx - rect.left < x)
                  newx = x + FrameTable[frame].FrLeftMargin + rect.left;		/* nouvelle position en X valide */
               else if (newx - rect.left > x + width)
                    newx = x + width + FrameTable[frame].FrLeftMargin + rect.left;		/* nouvelle position en X valide */
               else
                    newx += FrameTable[frame].FrLeftMargin;

               if (newy - rect.top < y)
                  newy = y + FrameTable[frame].FrTopMargin + rect.top;	/* nouvelle position en Y valide */
               else if (newy - rect.top > y + height)
                    newy = y + height + FrameTable[frame].FrTopMargin + rect.top;		/* nouvelle position en Y valide */
               else
                    newy += FrameTable[frame].FrTopMargin;

			   if (!SetCursorPos (newx, newy))
                  WinErrorBox (FrRef [frame]);
            } else {
                   newx += FrameTable[frame].FrLeftMargin;
                   newy += FrameTable[frame].FrTopMargin;
            }

            /* refresh the display of teh two adjacent segments */
            if (x1 != -1 && (newx != lastx || newy != lasty)) {
                   POINT ptBeg;
				   POINT ptEnd;

				   ptBeg.x = x1 - rect.left;
				   ptBeg.y = y1 - rect.top;
				   ptEnd.x = lastx - rect.left;
				   ptEnd.y = lasty - rect.top;
				   /* ptBeg.x = x1;
				   ptBeg.y = y1;
				   ptEnd.x = lastx;
				   ptEnd.y = lasty; */

				   DrawOutline(FrRef [frame], ptBeg, ptEnd);

				   ptBeg.x = x1 - rect.left;
				   ptBeg.y = y1 - rect.top;
				   ptEnd.x = newx - rect.left;
				   ptEnd.y = newy - rect.top;
				   /* ptBeg.x = x1;
				   ptBeg.y = y1;
				   ptEnd.x = newx;
				   ptEnd.y = newy; */

				   DrawOutline(FrRef [frame], ptBeg, ptEnd);
            }
	        lastx = newx;
	        lasty = newy;
         } else {
                /* coordinate checking */
                newx = x + DO_ALIGN ((int) cursorPos.x - FrameTable[frame].FrLeftMargin - x);
                newy = y + DO_ALIGN ((int) cursorPos.y - FrameTable[frame].FrTopMargin - y);
                /* CHKR_LIMIT to size of the box */
                if (newx - rect.left < x)
                   lastx = x + FrameTable[frame].FrLeftMargin + rect.left;	/* nouvelle position en X valide */
                else if (newx - rect.left > x + width)
                     lastx = x + width + FrameTable[frame].FrLeftMargin + rect.left;	/* nouvelle position en X valide */
                else
                     lastx = newx + FrameTable[frame].FrLeftMargin;

                if (newy - rect.top< y)
                   lasty = y + FrameTable[frame].FrTopMargin + rect.top;	/* nouvelle position en Y valide */
                else if (newy - rect.top> y + height)
                     lasty = y + height + FrameTable[frame].FrTopMargin + rect.top;	/* nouvelle position en Y valide */
                else
                     lasty = newy + FrameTable[frame].FrTopMargin;

				switch (event.message) {
                       case WM_LBUTTONDOWN:
                       case WM_MBUTTONDOWN:
                       case WM_RBUTTONDOWN:
                            if (newx - rect.left< x || newx - rect.left > x + width || newy - rect.top < y || newy - rect.top > y + height)
                               if (!SetCursorPos (lastx, lasty))
                                  WinErrorBox (FrRef [frame]);
                            break;

                       case WM_LBUTTONUP:
                       case WM_MBUTTONUP:
                            /* left button keep the last segment built */
                            /* keep the new segment first point coordinates */
                            x1 = lastx;
                            y1 = lasty;
                            nbpoints++;

                            /* update the box buffer */
                            newx = PixelToPoint (lastx - FrameTable[frame].FrLeftMargin - x - rect.left) * 1000;
                            newy = PixelToPoint (lasty - FrameTable[frame].FrTopMargin - y - rect.top) * 1000;
                            AddPointInPolyline (Bbuffer, nbpoints, newx, newy);
                           /* update the abstract box buffer */
                            newx = (int) ((float) newx * ratioX);
                            newy = (int) ((float) newy * ratioY);
                            AddPointInPolyline (Pbuffer, nbpoints, newx, newy);

                            if (event.message == WM_MBUTTONUP)
                               /* any other button : end of user input */
                               ret = 1;
                            break;

                       case WM_RBUTTONUP:
                            ret = 1;
                            break;

                       default: break;
	            }
         }
#        else /* !_WINDOWS */
         if (XPending (TtDisplay) == 0) {
            /* current pointer position */
            XQueryPointer (TtDisplay, w, &wdum, &wdum, &dx, &dy, &newx, &newy, &e);
            /* coordinate checking */
            newx = DO_ALIGN (newx - FrameTable[frame].FrLeftMargin - x);
            newx += x;
            newy = DO_ALIGN (newy - FrameTable[frame].FrTopMargin - y);
            newy += y;
            if (newx < x || newx > x + width || newy < y || newy > y + height) {
               /* CHKR_LIMIT to size of the box */
               if (newx < x)
                  newx = x + FrameTable[frame].FrLeftMargin;		/* nouvelle position en X valide */
               else if (newx > x + width)
                    newx = x + width + FrameTable[frame].FrLeftMargin;		/* nouvelle position en X valide */
               else
                    newx += FrameTable[frame].FrLeftMargin;

               if (newy < y)
                  newy = y + FrameTable[frame].FrTopMargin;	/* nouvelle position en Y valide */
               else if (newy > y + height)
                    newy = y + height + FrameTable[frame].FrTopMargin;		/* nouvelle position en Y valide */
               else
                    newy += FrameTable[frame].FrTopMargin;
               XWarpPointer (TtDisplay, None, w, 0, 0, 0, 0, newx, newy);
            } else {
                   newx += FrameTable[frame].FrLeftMargin;
                   newy += FrameTable[frame].FrTopMargin;
            }

            /* refresh the display of teh two adjacent segments */
            if (x1 != -1 && (newx != lastx || newy != lasty)) {
               XDrawLine (TtDisplay, FrRef[frame], TtInvertGC, x1, y1, lastx, lasty);
               XDrawLine (TtDisplay, FrRef[frame], TtInvertGC, x1, y1, newx, newy);
               XFlush (TtDisplay);
            }
	        lastx = newx;
	        lasty = newy;
         } else {
                XNextEvent (TtDisplay, &event);
                /* coordinate checking */
                newx = x + DO_ALIGN ((int) event.xmotion.x - FrameTable[frame].FrLeftMargin - x);
                newy = y + DO_ALIGN ((int) event.xmotion.y - FrameTable[frame].FrTopMargin - y);
                /* CHKR_LIMIT to size of the box */
                if (newx < x)
                   lastx = x + FrameTable[frame].FrLeftMargin;	/* nouvelle position en X valide */
                else if (newx > x + width)
                     lastx = x + width + FrameTable[frame].FrLeftMargin;	/* nouvelle position en X valide */
                else
                     lastx = newx + FrameTable[frame].FrLeftMargin;

                if (newy < y)
                   lasty = y + FrameTable[frame].FrTopMargin;	/* nouvelle position en Y valide */
                else if (newy > y + height)
                     lasty = y + height + FrameTable[frame].FrTopMargin;	/* nouvelle position en Y valide */
                else
                     lasty = newy + FrameTable[frame].FrTopMargin;

                switch (event.type) {
                       case ButtonPress:
                            if (newx < x || newx > x + width || newy < y || newy > y + height)
                               XWarpPointer (TtDisplay, None, w, 0, 0, 0, 0, lastx, lasty);
                            break;

                       case ButtonRelease:
                            if (event.xbutton.button != Button3) {
                               /* left button keep the last segment built */
                               /* keep the new segment first point coordinates */
                               x1 = lastx;
                               y1 = lasty;
                               nbpoints++;

                               /* update the box buffer */
                               newx = PixelToPoint (lastx - FrameTable[frame].FrLeftMargin - x) * 1000;
                               newy = PixelToPoint (lasty - FrameTable[frame].FrTopMargin - y) * 1000;
                               AddPointInPolyline (Bbuffer, nbpoints, newx, newy);
                              /* update the abstract box buffer */
                               newx = (int) ((float) newx * ratioX);
                               newy = (int) ((float) newy * ratioY);
                               AddPointInPolyline (Pbuffer, nbpoints, newx, newy);

                               if (event.xbutton.button != Button1)
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
#        endif /* _WINDOWS */
   }
   /* erase box frame */
#  ifdef _WINDOWS
   SetCursor (LoadCursor (NULL, IDC_ARROW));
#  endif /* *_WINDOWS */
   BoxGeometry (frame, x, y, width, height, x + width - 2, y + height - 2, TRUE);

   /* get back to previous state of the library */
#  ifndef _WINDOWS
   ThotUngrab ();
   XFlush (TtDisplay);
#  endif /* _WINDOWS */
   /* need at least 3 points for a valid polyline */
   if (nbpoints < 3) {
      TtaDisplaySimpleMessage (INFO, LIB, TMSG_TWO_POINTS_IN_POLYLINE_NEEDED);
      return 1;
   } else
         return nbpoints;
}

#ifndef _WIN_PRINT
/*----------------------------------------------------------------------
 *      PolyLineModification interract with the user to move a point part
 * 		a polyline in a given frame.
 *		x and y values gives the position of the box in the frame.
 *		Pbuffer points to the first buffer of the abstract box, i.e.
 *		the list of control points modified. Pbuffer points to
 *		the first buffer of the box, i.e. the list of control points
 *		as shown. The first point in the lists gives the maximum
 *		width and height of the polyline.
 *		nbpoints gives the number of points in the polyline.
 *		point expricitely indicate the point to be moved.
 *		This fonction update both  list of control points.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                PolyLineModification (int frame, int x, int y, PtrTextBuffer Pbuffer, PtrTextBuffer Bbuffer, int nbpoints, int point, boolean close)
#else  /* __STDC__ */
void                PolyLineModification (frame, x, y, Pbuffer, Bbuffer, nbpoints, point, close)
int                 frame;
int                 x;
int                 y;
PtrTextBuffer       Pbuffer;
PtrTextBuffer       Bbuffer;
int                 nbpoints;
int                 point;
boolean             close;
#endif /* __STDC__ */
{
#ifndef _WINDOWS
   float               ratioX, ratioY;
   int                 width, height;
   int                 e;
   int                 ret, f;
   int                 newx, newy, lastx, lasty;
   int                 x1, y1, x3, y3;
   ThotWindow          w;
   ThotEvent              event;
   boolean             wrap;

   /* box size */
   width = Bbuffer->BuPoints[0].XCoord;
   height = Bbuffer->BuPoints[0].YCoord;
   /* distortion ratios between the abstract box and the box */
   ratioX = (float) Pbuffer->BuPoints[0].XCoord / (float) width;
   ratioY = (float) Pbuffer->BuPoints[0].YCoord / (float) height;
   width = PointToPixel (width / 1000);
   height = PointToPixel (height / 1000);

   /* change cursor size */
   e = ButtonPressMask | ButtonReleaseMask | ButtonMotionMask;
   w = FrRef[frame];
   XMapRaised (TtDisplay, w);
   XFlush (TtDisplay);
   wrap = FALSE;
   ThotGrab (w, HVCurs, e, 0);

   /* shows up the box border */
   /*Clear(frame, width, height, x, y); */
   BoxGeometry (frame, x, y, width, height, x + width - 2, y + height - 2, TRUE);
   RedrawPolyLine (frame, x, y, Bbuffer, nbpoints, point, close,
		   &x1, &y1, &lastx, &lasty, &x3, &y3);
   /* the grid step begins at the origin of the box */
   XWarpPointer (TtDisplay, None, w, 0, 0, 0, 0, lastx, lasty);
   XFlush (TtDisplay);

   /* Loop waiting for user interraction */
   ret = 0;
   while (ret == 0)
     {
	XNextEvent (TtDisplay, &event);

	/* check the coordinates */
	newx = x + DO_ALIGN ((int) event.xmotion.x - FrameTable[frame].FrLeftMargin - x);
	newy = y + DO_ALIGN ((int) event.xmotion.y - FrameTable[frame].FrTopMargin - y);
	/* are limited to the box size */
	/* Update the X position */
	if (newx < x)
	  {
	     newx = x + FrameTable[frame].FrLeftMargin;
	     wrap = TRUE;
	  }
	else if (newx > x + width)
	  {
	     newx = x + width + FrameTable[frame].FrLeftMargin;
	     wrap = TRUE;
	  }
	else
	   newx += FrameTable[frame].FrLeftMargin;

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
		    newx = PixelToPoint (lastx - FrameTable[frame].FrLeftMargin - x) * 1000;
		    newy = PixelToPoint (lasty - FrameTable[frame].FrTopMargin - y) * 1000;
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
	      }			/*switch */
     }

   /* erase the box border */
   BoxGeometry (frame, x, y, width, height, x + width - 2, y + height - 2, TRUE);

   /* returns to previous state of the Thot library */
   ThotUngrab ();
   XFlush (TtDisplay);
#endif /* _WINDOWS */
}				/*PolyModification */
#endif /* _WIN_PRINT */

/*----------------------------------------------------------------------
 *      PolyLineExtension interract with the user to add points to
 * 		an existing polyline in a given frame.
 *		x and y values gives the position of the box in the frame.
 *		Pbuffer points to the first buffer of the abstract box, i.e.
 *		the list of control points modified. Pbuffer points to
 *		the first buffer of the box, i.e. the list of control points
 *		as shown. The first point in the lists gives the maximum
 *		width and height of the polyline.
 *		nbpoints gives the number of points in the polyline.
 *		point expricitely indicate the point to be moved.
 *		close indicate whether the polyline is closed.
 *		This fonction update both  list of control points and
 *		returns the new number of points in the polyline.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                 PolyLineExtension (int frame, int x, int y, PtrTextBuffer Pbuffer, PtrTextBuffer Bbuffer, int nbpoints, int point, boolean close)
#else  /* __STDC__ */
int                 PolyLineExtension (frame, x, y, Pbuffer, Bbuffer, nbpoints, point, close)
int                 frame;
int                 x;
int                 y;
PtrTextBuffer       Pbuffer;
PtrTextBuffer       Bbuffer;
int                 nbpoints;
int                 point;
boolean             close;
#endif /* __STDC__ */

{
#ifndef _WINDOWS
   float               ratioX, ratioY;
   int                 width, height;
   int                 e, dx, dy;
   int                 ret, f;
   int                 newx, newy, lastx, lasty;
   int                 x1, y1, x3, y3;
   ThotWindow          w, wdum;
   ThotEvent              event;
   boolean             wrap;

   /* the box size */
   width = Bbuffer->BuPoints[0].XCoord;
   height = Bbuffer->BuPoints[0].YCoord;
   /* the box size *ompute the distortion ratios between box and abstract box */
   ratioX = (float) Pbuffer->BuPoints[0].XCoord / (float) width;
   ratioY = (float) Pbuffer->BuPoints[0].YCoord / (float) height;
   width = PointToPixel (width / 1000);
   height = PointToPixel (height / 1000);

   /* change the cursor */
   e = ButtonPressMask | ButtonReleaseMask | ButtonMotionMask;
   w = FrRef[frame];
   wrap = FALSE;
   XMapRaised (TtDisplay, w);
   XFlush (TtDisplay);
   ThotGrab (w, HVCurs, e, 0);

   /* draw the box border */
   /*Clear(frame, width, height, x, y); */
   BoxGeometry (frame, x, y, width, height, x + width - 2, y + height - 2, TRUE);
   RedrawPolyLine (frame, x, y, Bbuffer, nbpoints, point, close,
		   &x1, &y1, &lastx, &lasty, &x3, &y3);
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
	     newx = DO_ALIGN (newx - FrameTable[frame].FrLeftMargin - x);
	     newx += x;
	     newy = DO_ALIGN (newy - FrameTable[frame].FrTopMargin - y);
	     newy += y;
	     if (newx < x || newx > x + width || newy < y || newy > y + height)
	       {
		  /* CHKR_LIMIT them to the box area */
		  if (newx < x)
		     newx = x + FrameTable[frame].FrLeftMargin;
		  else if (newx > x + width)
		     newx = x + width + FrameTable[frame].FrLeftMargin;		/* nouvelle position en X valide */
		  else
		     newx += FrameTable[frame].FrLeftMargin;

		  if (newy < y)
		     newy = y + FrameTable[frame].FrTopMargin;
		  else if (newy > y + height)
		     newy = y + height + FrameTable[frame].FrTopMargin;		/* nouvelle position en Y valide */
		  else
		     newy += FrameTable[frame].FrTopMargin;
		  XWarpPointer (TtDisplay, None, w, 0, 0, 0, 0, newx, newy);
	       }
	     else
	       {
		  newx += FrameTable[frame].FrLeftMargin;
		  newy += FrameTable[frame].FrTopMargin;
	       }

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
	     newx = x + DO_ALIGN ((int) event.xmotion.x - FrameTable[frame].FrLeftMargin - x);
	     newy = y + DO_ALIGN ((int) event.xmotion.y - FrameTable[frame].FrTopMargin - y);
	     /* CHKR_LIMIT them to the box area */
	     if (newx < x)
	       {
		  newx = x + FrameTable[frame].FrLeftMargin;
		  wrap = TRUE;
	       }
	     else if (newx > x + width)
	       {
		  newx = x + width + FrameTable[frame].FrLeftMargin;
		  wrap = TRUE;
	       }
	     else
		newx += FrameTable[frame].FrLeftMargin;

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
			 if (wrap)
			   {
			      XWarpPointer (TtDisplay, None, w, 0, 0, 0, 0, lastx, lasty);
			      wrap = FALSE;
			   }
			 break;

		      case ButtonRelease:
			 if (event.xbutton.button != Button3)
			   {
			      /* left button : keep the new point */
			      /* write down the new segment start point */
			      lastx = newx;
			      lasty = newy;
			      x1 = lastx;
			      y1 = lasty;
			      nbpoints++;
			      point++;
			      /* update the box buffer */
			      newx = PixelToPoint (lastx - FrameTable[frame].FrLeftMargin - x) * 1000;
			      newy = PixelToPoint (lasty - FrameTable[frame].FrTopMargin - y) * 1000;
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
		   }		/*switch */
	  }
     }

   /* erase the box border */
   BoxGeometry (frame, x, y, width, height, x + width - 2, y + height - 2, TRUE);

   /* go back to previous state of Thot library */
   ThotUngrab ();
   XFlush (TtDisplay);
   /* a valid polyline need at least 3 points */
   if (nbpoints < 3)
      return 1;
   else
      return nbpoints;
#endif /* _WINDOWS */
}

#ifndef _WIN_PRINT
/*----------------------------------------------------------------------
 *      UserGeometryCreate draw a box at a specific (x,y) location in
 *		frame and of size width x height when interracting with
 *		the user to mofify a box size (button press).
 *		xmin, xmax, ymin, ymax are the maximum values allowed.
 *		this function returns the new values upon button release.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                UserGeometryCreate (int frame, int *x, int *y, int xr, int yr, int *width, int *height, int xmin, int xmax, int ymin, int ymax, int PosX, int PosY, int DimX, int DimY)
#else  /* __STDC__ */
void                UserGeometryCreate (frame, x, y, xr, yr, width, height, xmin, xmax, ymin, ymax, PosX, PosY, DimX, DimY)
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
#endif /* __STDC__ */

{
   int                 xm, ym;
   int                 ret, e, dx, dy;
   int                 nx, ny, f;
   ThotEvent              event;
   ThotWindow          w, wdum;
   int                 RightOrLeft, BottomOrTop;
   int                 warpx, warpy;
#  ifdef _WINDOWS
   RECT            rect;
   POINT cursorPos;     
#  endif /* _WINDOWS */

   /* use relative coordinates */
   xr -= *x;
   yr -= *y;
   xm = ym = 0;

   /* change the cursor, modify library state */
   w = FrRef[frame];
#  ifdef _WINDOWS
   /* if (!GetWindowPlacement (w, &wp)) */
   if (!GetWindowRect (w, &rect))
      WinErrorBox (w);
#  else /* !_WINDOWS */
   e = ButtonPressMask | ButtonReleaseMask | ButtonMotionMask;
   ThotGrab (w, HVCurs, e, 0);
#  endif /* !_WINDOWS */

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

#  ifdef _WINDOWS 
   /* if (!SetCursorPos (*x + wp.rcNormalPosition.left, *y + wp.rcNormalPosition.top)) */
   if (!SetCursorPos (*x + rect.left, *y + rect.top))
   /* if (!SetCursorPos (*x, *y)) */
      WinErrorBox (FrRef [frame]);
#  else  /* !_WINDOWS */
   XMapRaised (TtDisplay, w);
   XWarpPointer (TtDisplay, None, w, 0, 0, 0, 0, *x + FrameTable[frame].FrLeftMargin, *y + FrameTable[frame].FrTopMargin);
   XFlush (TtDisplay);
#  endif /* !_WINDOWS */

   /* draw the current box geometry */
   BoxGeometry (frame, *x, *y, *width, *height, *x + xr, *y + yr, FALSE);

   /* Loop on user input */

   ret = 0;
   while (ret == 0) {
#        ifdef _WINDOWS
         SetCursor (LoadCursor (NULL, IDC_CROSS));
         ShowCursor (TRUE);
         GetMessage (&event, NULL, 0, 0);
         if (event.message == WM_MOUSEMOVE) {
            GetCursorPos (&cursorPos);
            /* check the coordinates are withing limits */
            nx = DO_ALIGN (cursorPos.x - FrameTable[frame].FrLeftMargin - xmin);
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
                             xm = xmin + DO_ALIGN ((int) cursorPos.x - FrameTable[frame].FrLeftMargin - xmin);
                          else
                              xm = *x;
                          if (PosY)
                             ym = ymin + DO_ALIGN ((int) cursorPos.y - FrameTable[frame].FrTopMargin - ymin);
                          else
                              ym = *y;
			      
                          /* check the coordinates */
                          if ((xm - rect.left) < xmin || (xm - rect.left) > xmax || !PosX || (ym - rect.top) < ymin || (ym - rect.top) > ymax || !PosY) {
                             SetCursorPos (xm + rect.left, ym + rect.top);
                          } else
                               ret = 1;
                          break;

                     default: break;
              }
         }
#        else  /* !_WINDOWS */
         if (XPending (TtDisplay) == 0) {
            /* get the cursor location */
            XQueryPointer (TtDisplay, w, &wdum, &wdum, &dx, &dy, &nx, &ny, &e);
            /* check the coordinates are withing limits */
            nx = DO_ALIGN (nx - FrameTable[frame].FrLeftMargin - xmin);
            nx += xmin;
            ny = DO_ALIGN (ny - FrameTable[frame].FrTopMargin - ymin);
            ny += ymin;
            if (nx < xmin || nx > xmax || ny < ymin || ny > ymax)
               XWarpPointer (TtDisplay, None, w, 0, 0, 0, 0, *x + FrameTable[frame].FrLeftMargin, *y + FrameTable[frame].FrTopMargin);
            else if ((nx != *x && PosX) || (ny != *y && PosY)) {
                 BoxGeometry (frame, *x, *y, *width, *height, *x + xr, *y + yr, FALSE);
                 if (PosX)
                    *x = nx;
                 if (PosY)
                    *y = ny;

                 BoxGeometry (frame, *x, *y, *width, *height, *x + xr, *y + yr, FALSE);
                 XFlush (TtDisplay);
                 /* the postion is fixed */
                 if (!PosX || !PosY)
                    XWarpPointer (TtDisplay, None, w, 0, 0, 0, 0, *x + FrameTable[frame].FrLeftMargin, *y + FrameTable[frame].FrTopMargin);
            }
	     } else {
                XNextEvent (TtDisplay, &event);

                /* we only deal with button press events */
                switch (event.type) {
                       case ButtonPress:
                            if (PosX)
                               xm = xmin + DO_ALIGN ((int) event.xmotion.x - FrameTable[frame].FrLeftMargin - xmin);
                            else
                                xm = *x;
                            if (PosY)
                               ym = ymin + DO_ALIGN ((int) event.xmotion.y - FrameTable[frame].FrTopMargin - ymin);
                            else
                                ym = *y;
			      
                            /* check the coordinates */
                            if (xm < xmin || xm > xmax || !PosX || ym < ymin || ym > ymax || !PosY) {
                               XWarpPointer (TtDisplay, None, w, 0, 0, 0, 0, xm + FrameTable[frame].FrLeftMargin,
     	                       ym + FrameTable[frame].FrTopMargin);
                            } else
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
#        endif /* !_WINDOWS */
   }

   /* new values for the box geometry */
   BoxGeometry (frame, *x, *y, *width, *height, *x + xr, *y + yr, FALSE);	/*Ancienne */
#  ifndef _WINDOWS 
   *x = xm;
   *y = ym; 
#  else  /* _WINDOWS */
   *x = xm - rect.left;
   *y = ym - rect.top;
#  endif /* _WINDOWS */
   xr = 2;
   yr = 2;
   xm += FrameTable[frame].FrLeftMargin;
   ym += FrameTable[frame].FrTopMargin;
   BoxGeometry (frame, *x, *y, *width, *height, *x + xr, *y + yr, FALSE);
   /* indicate that the original position is the reference */
   RightOrLeft = 2;
   BottomOrTop = 2;

   /* New loop waiting for the second point definition */
   ret = 0;
   while (ret == 0) {
#        ifdef _WINDOWS
         SetCursor (LoadCursor (NULL, IDC_CROSS));
         ShowCursor (TRUE);
         GetMessage (&event, NULL, 0, 0);
         switch (event.message) {
                case WM_LBUTTONUP:
                case WM_MBUTTONUP:
                case WM_RBUTTONUP:
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
                        if (DimX)
                           *width = 0;
                        if (DimY)
                           *height = 0;
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
                        BoxGeometry (frame, *x, *y, *width, *height, *x + xr, *y + yr, FALSE); /*Ancienne */
                        BoxGeometry (frame, nx, ny, dx, dy, nx + xr, ny + yr, FALSE); /*Nouvelle */
                        *x = nx;
                        *y = ny;
                        *width = dx;
                        *height = dy;
                     }   

                     /* should we move the cursor */
                     if (warpx >= 0 || warpy >= 0) {
                        if (warpx >= 0)
                           xm = warpx + FrameTable[frame].FrLeftMargin;
                        if (warpy >= 0)
                           ym = warpy + FrameTable[frame].FrTopMargin;
                        if (!SetCursorPos (xm, ym))
                           WinErrorBox (FrRef [frame]);
                     }
                     break;

                default: break;
         }
#        else  /* !_WINDOWS */
         XNextEvent (TtDisplay, &event);
         /* On analyse le type de l'evenement */
         switch (event.type) {
                case ButtonRelease:
                     ret = 1;

                case MotionNotify:
                     warpx = -1;
                     warpy = -1;

                     /* check this is the correct window */
                     if (event.xmotion.window == w) {
                        /* new cursor location */
                        if (DimX) {
                           dx = (int) event.xmotion.x - xm;
                           nx = xmin + DO_ALIGN (*x + *width + dx - xmin);
                           dx = nx - *x - *width;
                        } else
                              dx = 0;

                        if (DimY) {
                           dy = (int) event.xmotion.y - ym;
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
                           if (DimX)
                              *width = 0;
                           if (DimY)
                              *height = 0;
                           BoxGeometry (frame, *x, *y, *width, *height, *x + xr, *y + yr, FALSE);
                        }
                     } else {
                            dx = 0;
                            dy = 0;
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
                        BoxGeometry (frame, *x, *y, *width, *height, *x + xr, *y + yr, FALSE); /*Ancienne */
                        BoxGeometry (frame, nx, ny, dx, dy, nx + xr, ny + yr, FALSE); /*Nouvelle */
                        XFlush (TtDisplay);
                        *x = nx;
                        *y = ny;
                        *width = dx;
                        *height = dy;
                     }   

                     /* should we move the cursor */
                     if (warpx >= 0 || warpy >= 0) {
                        if (warpx >= 0)
                           xm = warpx + FrameTable[frame].FrLeftMargin;
                        if (warpy >= 0)
                           ym = warpy + FrameTable[frame].FrTopMargin;
                        XWarpPointer (TtDisplay, None, w, 0, 0, 0, 0, xm, ym);
                     }
                     break;

                default: break;
         }
#        endif /* !_WINDOWS */
   }

   /* Erase the box drawing */
   BoxGeometry (frame, *x, *y, *width, *height, *x + xr, *y + yr, FALSE);
#  ifndef _WINDOWS 
   /* restore state of the Thot Library */
   ThotUngrab ();
   XFlush (TtDisplay);
#  else  /* _WINDOWS */
   w = FrRef[frame];
#  endif /* _WINDOWS */
}

/*----------------------------------------------------------------------
 *      UserGeometryMove draw a box at a specific (x,y) location in
 *		frame and of size width x height when interracting with
 *		the user to mofify the box position (button press).
 *		xmin, xmax, ymin, ymax are the maximum values allowed.
 *		xm and ym gives the initial mouse coordinates in the frame.
 *		this function returns the new position upon button release.
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
#ifndef _WINDOWS
   int                 ret, e, dx, dy, nx, ny;
   ThotEvent              event;
   ThotWindow          w;
   int                 warpx, warpy;

   /* reset the cursor coordinate in the frame */
   xm += FrameTable[frame].FrLeftMargin;
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
   e = ButtonPressMask | ButtonReleaseMask | ButtonMotionMask;
   w = FrRef[frame];
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

#endif /* _WINDOWS */
}

/*----------------------------------------------------------------------
 *      UserGeometryResize draw a box at a specific (x,y) location in
 *		frame and of size width x height when interracting with
 *		the user to mofify the box geometry (button press).
 *		xr and yr gives the initial mouse reference point.
 *		xmin, xmax, ymin, ymax are the maximum values allowed.
 *		xm and ym gives the initial mouse coordinates in the frame.
 *		this function returns the new geometry upon button release.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                UserGeometryResize (int frame, int x, int y, int *width, int *height, int xr, int yr, int xmin, int xmax, int ymin, int ymax, int xm, int ym)
#else  /* __STDC__ */
void                UserGeometryResize (frame, x, y, width, height, xr, yr, xmin, xmax, ymin, ymax, xm, ym)
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
#endif /* __STDC__ */

{
#ifndef _WINDOWS
#define C_TOP 0
#define C_HCENTER 1
#define C_BOTTOM 2
#define C_LEFT 0
#define C_VCENTER 1
#define C_RIGHT 2

   int                 ret, e, dx, dy, dl, dh;
   int                 ref_h, ref_v, HorizontalDirection, VerticalDirection;
   ThotEvent              event;
   ThotWindow          w;
   int                 warpx, warpy;

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
   xm += FrameTable[frame].FrLeftMargin;
   ym += FrameTable[frame].FrTopMargin;

   /* Shows the initial box size */
   BoxGeometry (frame, x, y, *width, *height, xr, yr, FALSE);
   e = ButtonPressMask | ButtonReleaseMask | ButtonMotionMask;

   /* select the correct cursor */
   w = FrRef[frame];
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
		      {
			 dl = dh = 0;
		      }

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
				 dx = x + *width - xmin - DO_ALIGN (xmax - xmin);		/*cote droit */

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
				 dl = xmin + DO_ALIGN (xmax - xmin) - x - *width;		/*cote droit */
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
				 dy = y + *height - ymin - DO_ALIGN (ymax - ymin);	/*cote inferieur */
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
				 dh = ymin + DO_ALIGN (ymax - ymin) - y - *height;	/*cote inf */
			      if (dh != 0)
				 warpy = ym + dh;
			   }
		      }
		    else
		       dy = 0;

		    /* Should we move the box */
		    if ((dl != 0) || (dh != 0))
		      {
			 BoxGeometry (frame, x, y, *width, *height, xr, yr, FALSE);	/*Ancienne */
			 *width += dl;
			 *height += dh;
			 x += dx;
			 y += dy;
			 BoxGeometry (frame, x, y, *width, *height, xr, yr, FALSE);	/*Nouvelle */
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

   /* restore the prvious state of the Thot Library */
   ThotUngrab ();
   XFlush (TtDisplay);
#endif /* _WINDOWS */
}
#endif /* _WIN_PRINT */