/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2009
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

#ifdef _WX
#include "wx/wx.h"
#include "wx/event.h"

#include "thot_gui.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "constmedia.h"
#include "typemedia.h"
#include "frame.h"
#include "libmsg.h"
#include "message.h"
#include "typeint.h"
#include "undo.h"

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "boxes_tv.h"
#include "frame_tv.h"

#include "frame_f.h"
#include "presentationapi_f.h"
#include "structcreation_f.h"
#include "boxlocate_f.h"
#include "geom_f.h"
#include "font_f.h"
#include "content_f.h"

#ifdef _GL
#include "glwindowdisplay.h"
#endif /* _GL */

#include "logdebug.h"


#include "AmayaFrame.h"
#include "AmayaCanvas.h"
#include "AmayaCreateShapeEvtHandler.h"

IMPLEMENT_DYNAMIC_CLASS(AmayaCreateShapeEvtHandler, wxEvtHandler)

  /*----------------------------------------------------------------------
   *  this is where the event table is declared
   *  the callbacks are assigned to an event type
   *----------------------------------------------------------------------*/
  BEGIN_EVENT_TABLE(AmayaCreateShapeEvtHandler, wxEvtHandler)
  EVT_KEY_DOWN ( AmayaCreateShapeEvtHandler::OnKeyDown )
  EVT_LEFT_DOWN(	AmayaCreateShapeEvtHandler::OnMouseDown)
  EVT_LEFT_UP(		AmayaCreateShapeEvtHandler::OnMouseUp)
  EVT_LEFT_DCLICK(	AmayaCreateShapeEvtHandler::OnMouseDbClick)
  EVT_MIDDLE_DOWN(	AmayaCreateShapeEvtHandler::OnMouseDown)
  EVT_MIDDLE_UP(	AmayaCreateShapeEvtHandler::OnMouseUp)
  EVT_MIDDLE_DCLICK(	AmayaCreateShapeEvtHandler::OnMouseDbClick)
  EVT_RIGHT_DOWN(	AmayaCreateShapeEvtHandler::OnMouseDown)
  EVT_RIGHT_UP(		AmayaCreateShapeEvtHandler::OnMouseUp)
  EVT_RIGHT_DCLICK(	AmayaCreateShapeEvtHandler::OnMouseDbClick)
  EVT_MOTION(		AmayaCreateShapeEvtHandler::OnMouseMove)
  EVT_MOUSEWHEEL(	AmayaCreateShapeEvtHandler::OnMouseWheel)
  END_EVENT_TABLE()

/*----------------------------------------------------------------------
*----------------------------------------------------------------------*/
  AmayaCreateShapeEvtHandler::AmayaCreateShapeEvtHandler() : wxEvtHandler()
{
}

/*----------------------------------------------------------------------
 *----------------------------------------------------------------------*/
AmayaCreateShapeEvtHandler::AmayaCreateShapeEvtHandler (AmayaFrame * p_frame,
                                                        Document doc, void *inverseCTM,
                                                        int ancestorX,
                                                        int ancestorY,
                                                        int canvasWidth,
                                                        int canvasHeight,
                                                        int shape_number,
                                                        float ratio_wh,
                                                        int *x1, int *y1, int *x2, int *y2,
                                                        ThotBool *created)
  : wxEvtHandler()
  ,pFrame(p_frame)
  ,frameId(p_frame->GetFrameId())
  ,shape(shape_number)
  ,created(created)
{
  document = doc;
  ratio = ratio_wh;
  x0 = ancestorX;
  y0 = ancestorY;
  px1 = x1;
  py1 = y1;
  px2 = x2;
  py2 = y2;
  finished = false;
  nb_points = 0;
  width = canvasWidth;
  height = canvasHeight;
  transform = inverseCTM;
  if (pFrame)
    {
      /* attach this handler to the canvas */
      AmayaCanvas * p_canvas = pFrame->GetCanvas();
      p_canvas->PushEventHandler(this);
#ifdef _WINDOWS
      // need to grab the mouse before changing the cursor on Windows
      p_canvas->CaptureMouse();
#endif /* _WINDOWS */
      if(shape == 9 || shape == 10)
        /* assign a ibeam mouse cursor */
        p_canvas->SetCursor( wxCursor(wxCURSOR_IBEAM) );
      else
        /* assign a cross mouse cursor */
        p_canvas->SetCursor( wxCursor(wxCURSOR_CROSS) );
#ifndef _WINDOWS
      p_canvas->CaptureMouse();
#endif /* _WINDOWS */
    }

  if(shape == 42)
    /* It is actually a box that allows to select graphical elements, use
       dash style */
    InitDrawing (2, 1, 0);
  else
    InitDrawing (5, 1, 0);
}

/*----------------------------------------------------------------------
 *----------------------------------------------------------------------*/
AmayaCreateShapeEvtHandler::~AmayaCreateShapeEvtHandler()
{
  /* Clear the shape */
  DrawShape ();
  TtaSetStatus (document, 1, "", NULL);
  if (pFrame)
    {
      /* detach this handler from the canvas (restore default behaviour) */
      AmayaCanvas * p_canvas = pFrame->GetCanvas();
      p_canvas->PopEventHandler(false /* do not delete myself */);
      
      /* restore the default cursor */
      p_canvas->SetCursor( wxNullCursor );
      p_canvas->ReleaseMouse();
    }
}

/*----------------------------------------------------------------------
  IsFinish
  *----------------------------------------------------------------------*/
bool AmayaCreateShapeEvtHandler::IsFinish()
{
  return finished;
}

/*----------------------------------------------------------------------
  OnKeyDown
  *----------------------------------------------------------------------*/
void AmayaCreateShapeEvtHandler::OnKeyDown( wxKeyEvent& event )
{
  if (event.GetKeyCode() !=  WXK_SHIFT)
    {
      *created = FALSE;
      finished = true;
    }
}

/*----------------------------------------------------------------------
 *       Class:  AmayaCreateShapeEvtHandler
 *      Method:  OnMouseDown
 * Description:  handle mouse button down events
 -----------------------------------------------------------------------*/
void AmayaCreateShapeEvtHandler::OnMouseDown( wxMouseEvent& event )
{
  if (finished || nb_points != 0)
    return;
  /* Are we in the SVG ? */
  if (!MouseCoordinatesToSVG (document, pFrame, x0, y0, width, height,
                              transform, FALSE, NULL,
                              px1, py1, FALSE))
    return;

  nb_points = 1;
  *px2 = *px1;
  *py2 = *py1;
  if (shape == 10 || shape == 9)
    {
      /* Only one point is needed */
      *created = TRUE;
      finished = true;
    }
}

/*----------------------------------------------------------------------
 *       Class:  AmayaCreateShapeEvtHandler
 *      Method:  OnMouseUp
 * Description:  handle mouse button up events
 -----------------------------------------------------------------------*/
void AmayaCreateShapeEvtHandler::OnMouseUp( wxMouseEvent& event )
{
  if (finished || nb_points != 1)return;

  /* Are we in the SVG ? */
  if (!MouseCoordinatesToSVG (document, pFrame, x0, y0, width, height,
                              transform, FALSE, NULL,
                              px2, py2, FALSE))
    return;
  nb_points = 2;
  *created = TRUE;
  TtaSetStatus (document, 1, "", NULL);
  finished = true;
}

/*----------------------------------------------------------------------
 *       Class:  AmayaCreateShapeEvtHandler
 *      Method:  OnMouseDbClick
 * Description:  handle mouse dbclick events
 -----------------------------------------------------------------------*/
void AmayaCreateShapeEvtHandler::OnMouseDbClick( wxMouseEvent& event )
{
  TtaSetStatus (document, 1, "", NULL);
  finished = true;
}

/*----------------------------------------------------------------------
 *       Class:  AmayaCreateShapeEvtHandler
 *      Method:  OnMouseMove
 * Description:  handle mouse move events
 -----------------------------------------------------------------------*/
void AmayaCreateShapeEvtHandler::OnMouseMove( wxMouseEvent& event )
{
  int x, y, d;

  /* Clear the shape */
  DrawShape ();

  /* Get the coordinates of the mouse and display them in the status bar */
  x = event.GetX();
  y = event.GetY();

  if (event.ShiftDown() && nb_points > 0)
    {
      if (shape == 0 || shape == 12 || shape == 13)
        /* It's a line or an arrow and the shift button is pressed */
        ApproximateAngleOfLine (15, *px1, *py1, &x, &y);
      else
        {
          if (x - *px2 >= y - *py2)
            {
              d =  x - *px2;
              y = (int)(*py2 + (d * ratio));
            }
          else
            {
              d =  y - *py2;
              x = (int)(*px2 + (d / ratio));
            }
        }
    }
  
  MouseCoordinatesToSVG (document, pFrame, x0, y0, width, height,
                         transform, FALSE, TtaGetMessage (LIB, BUTTON_UP),
                         &x, &y, TRUE);
  if (nb_points == 0)
    {
      /* First point */
      *px1 = x;
      *py1 = y;
    }
  else
    {
      /* Second point */
      *px2 = x;
      *py2 = y;
    }

  /* Draw the shape */
  DrawShape ();
#ifndef _WINDOWS
  pFrame->GetCanvas()->Refresh();
#endif /* _WINDOWS */
}

/*----------------------------------------------------------------------
 *       Class:  AmayaCreateShapeEvtHandler
 *      Method:  OnMouseWheel
 * Description:  handle mouse wheel events
 -----------------------------------------------------------------------*/
void AmayaCreateShapeEvtHandler::OnMouseWheel( wxMouseEvent& event )
{
}

/*----------------------------------------------------------------------
  -----------------------------------------------------------------------*/
void AmayaCreateShapeEvtHandler::DrawShape ()
{ 
  int lx,ly;
  int x1, x2, y1, y2, x3, y3, x4, y4, x5, y5, x6, y6,tmp;

  if (nb_points == 0)
    return;
  glEnable (GL_COLOR_LOGIC_OP);
#ifdef _WINDOWS
  glLogicOp (GL_COPY_INVERTED);
#else /* _WINDOWS */
  glLogicOp (GL_XOR);
#endif /* _WINDOWS */
  glColor4ub (127, 127, 127, 0);

  lx = abs(*px2 - *px1);
  ly = abs(*py2 - *py1);
  if (!(shape == 0 || (shape >= 12 && shape <= 14)))
    {
      /* It's a shape drawn in a rectangle */

      if (shape == 20)
        /* equilateral triangle */
        lx = (int) (floor(2 *  ly / sqrt((float)3)));
      else if (shape == 3 || shape == 15 || shape == 16 || shape == 23)
        {
          /* lx and ly must be equal (square, circle...) */
          if (ly < lx)lx=ly; else ly = lx;
        }

      if (*px2 < *px1)
        *px2 = *px1 - lx;
      else
        *px2 = *px1 + lx;
      if (*py2 < *py1)
        *py2 = *py1 - ly;
      else
        *py2 = *py1 + ly;
    }
  
  x1 = *px1;
  x2 = *px2;
  y1 = *py1;
  y2 = *py2;
  switch(shape)
    {
      /* Line */
    case 0:
      glBegin(GL_LINE_STRIP);
      glVertex2i(x1, y1);
      glVertex2i(x2, y2);
      glEnd (); 
      break;

    case 12: /* Simple Arrow */
      x3 = x1; y3 = y1;
      x4 = x2; y4 = y2;
      GetArrowCoord(&x3, &y3, &x4, &y4);

      glBegin(GL_LINE_STRIP);
      glVertex2i(x1, y1);
      glVertex2i(x2, y2);
      glEnd (); 
	      
      glBegin(GL_LINE_STRIP);
      glVertex2i(x3, y3);
      glVertex2i(x2, y2);
      glVertex2i(x4, y4);

      glEnd ();
      break;

    case 13: /* Double Arrow */
      x3 = x1; y3 = y1;
      x4 = x2; y4 = y2;
      GetArrowCoord(&x3, &y3, &x4, &y4);

      x5 = x2; y5 = y2;
      x6 = x1; y6 = y1;
      GetArrowCoord(&x5, &y5, &x6, &y6);

      glBegin(GL_LINE_STRIP);
      glVertex2i(x1, y1);
      glVertex2i(x2, y2);
      glEnd (); 


      glBegin(GL_LINE_STRIP);
      glVertex2i(x3, y3);
      glVertex2i(x2, y2);
      glVertex2i(x4, y4);
      glEnd (); 

      glBegin(GL_LINE_STRIP);
      glVertex2i(x5, y5);
      glVertex2i(x1, y1);
      glVertex2i(x6, y6);
      glEnd (); 

      break;

    case 14: /* Zigzag */
      glBegin(GL_LINE_STRIP);
      glVertex2i(x1,y1);
      glVertex2i(x1,(y1+y2)/2);
      glVertex2i(x2,(y1+y2)/2);
      glVertex2i(x2,y2);
      glEnd (); 
      break;

      /* template */
    case -2:

      /* svg */
    case -1:

      /* Selection */
    case 42:

      /* Square */
    case 15:

      /* Rectangle */
    case 1:
      glBegin(GL_LINE_LOOP);
      glVertex2i(x1, y1);
      glVertex2i(x2, y1);
      glVertex2i(x2, y2);
      glVertex2i(x1, y2);
      glEnd (); 
      break;

      /* Rounded Square */
    case 16:
      /* Rounded-Rectangle */
    case 2:
      if (lx > 5 && ly > 5)
        {
          if (x2<x1){tmp=x1;x1=x2;x2=tmp;}
          if (y2<y1){tmp=y1;y1=y2;y2=tmp;}

          glBegin(GL_LINE_LOOP);
          glVertex2i(x1+5, y1);
          glVertex2i(x2-5, y1);
          glVertex2i(x2, y1+5);
          glVertex2i(x2, y2-5);
          glVertex2i(x2-5, y2);
          glVertex2i(x1+5, y2);
          glVertex2i(x1, y2-5);
          glVertex2i(x1, y1+5);
          glEnd();
        }
      break;

      /* Circle */
    case 3:
      /* Ellipse */
    case 4:
      if (x2<x1)x1=x2;
      if (y2<y1)y1=y2;
      GL_DrawArc (x1, y1, lx, ly, 0, 360, 0, FALSE);
      break;

    case 17: /* diamond */
      glBegin(GL_LINE_LOOP);
      glVertex2i(x1, (y1+y2)/2);
      glVertex2i((x1+x2)/2, y1);
      glVertex2i(x2, (y1+y2)/2);
      glVertex2i((x1+x2)/2, y2);
      glEnd ();
      break;

    case 18: /* trapezium */
      glBegin(GL_LINE_LOOP);
      glVertex2i(x1, y2);
      glVertex2i((3*x1+x2)/4, y1);
      glVertex2i((x1+3*x2)/4, y1);
      glVertex2i(x2, y2);
      glEnd (); 
      break;

    case 19: /* parallelogram */
      glBegin(GL_LINE_LOOP);
      glVertex2i((3*x1+x2)/4, y1);
      glVertex2i(x2, y1);
      glVertex2i((3*x2+x1)/4, y2);
      glVertex2i(x1, y2);
      glEnd (); 
      break;

    case 20: /* equilateral triangle */
      glBegin(GL_LINE_LOOP);
      glVertex2i((x1+x2)/2, y1);
      glVertex2i(x2, y2);
      glVertex2i(x1, y2);
      glEnd (); 
      break;

    case 21: /* isosceles triangle */
      glBegin(GL_LINE_LOOP);
      glVertex2i((x1+x2)/2, y1);
      glVertex2i(x2, y2);
      glVertex2i(x1, y2);
      glEnd (); 
      break;

    case 22: /* rectangle triangle */
      glBegin(GL_LINE_LOOP);
      glVertex2i(x1, y1);
      glVertex2i(x2, y1);
      glVertex2i(x1, y2);
      glEnd (); 
      break;

    case 23: /* cube */
    case 24: /* parallelepiped */
      glBegin(GL_LINE_STRIP);
      glVertex2i(x1, (y1*3+y2)/4);
      glVertex2i((3*x1+x2)/4, y1);
      glVertex2i(x2, y1);
      glVertex2i(x2, (3*y2+y1)/4);
      glVertex2i((3*x2+x1)/4, y2);
      glVertex2i(x1,y2);
      glVertex2i(x1, (3*y1+y2)/4);
      glVertex2i((3*x2+x1)/4, (3*y1+y2)/4);
      glVertex2i((3*x2+x1)/4, y2);
      glEnd (); 

      glBegin(GL_LINE_STRIP);
      glVertex2i((3*x2+x1)/4, (3*y1+y2)/4);
      glVertex2i(x2, y1);
      glEnd (); 
      break;

    case 25: /* Cylinder */
      glBegin(GL_LINE_STRIP);
      glVertex2i(x1, (y1*5+y2)/6);
      glVertex2i(x1, (5*y2+y1)/6);
      glEnd (); 

      glBegin(GL_LINE_STRIP);
      glVertex2i(x2,(5*y1+y2)/6);
      glVertex2i(x2,(5*y2+y1)/6);
      glEnd (); 
      if (x2 < x1)
        x1=x2;
      if (y2 < y1)
        {
          GL_DrawArc (x1, y2, lx, ly/3, 0, 180, 0, FALSE);
          GL_DrawArc (x1, (y2+2*y1)/3, lx, ly/3, 0, 360, 0, FALSE);
        }
      else
        {
          GL_DrawArc (x1, y1, lx, ly/3, 0, 360, 0, FALSE);
          GL_DrawArc (x1, (2*y2+y1)/3, lx, ly/3, 0, -180, 0, FALSE);
        }
      break;

    default:
      /* shape = 9 = Foreign object */
      /* shape = 10 = Text */
      break;
    }
  glLogicOp(GL_COPY);
  glDisable(GL_COLOR_LOGIC_OP);
#ifdef _WINDOWS
  GL_Swap (frameId);
#endif /* WINDOWS */
}

#endif /* _WX */
