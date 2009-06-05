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
#include "content.h"
#include "content_f.h"
#include "viewapi_f.h"

#ifdef _GL
#include "glwindowdisplay.h"
#endif /* _GL */

#include "logdebug.h"

#include "AmayaFrame.h"
#include "AmayaCanvas.h"
#include "AmayaEditShapeEvtHandler.h"
#include "svgedit.h"
ThotBool Angle_ratio = FALSE;

/*----------------------------------------------------------------------
 -----------------------------------------------------------------------*/
static void apply_deltas(int *x, int *y, int *lx, int *ly,
                         int dx, int dy,
                         int sx, int sy, float ratio)
{
  if (ratio)
    {
      /* The ratio = ly/lx must be preserved */
      if (sx == 0)
        {
          /* Vertical resize */
          (*ly)+=sy*dy;
          dx = (int)((*lx - (*ly)/ratio)/2);
          (*lx)=(int)((*ly)/ratio);
          sx = -1; // Force the change of coordinates
        }
      else if (sy == 0)
        {
          /* Horizontal resize */
          (*lx)+=sx*dx;
          dy = (int)((*ly - (*lx)*ratio)/2);
          (*ly)=(int)((*lx)*ratio);
          sy = -1; // Force the change of coordinates
        }
      else if (dx*dy*sx*sy <= 0)
        {
          /* A diagonal resize with contradictory directions */
          return;
        }
      else
        {
          /* Diagonal resize */
          if (ratio*abs(dx) < abs(dy))
            {
              /* Choose the horizontal delta */
              (*lx)+=sx*dx;
              dy = (int)(sy*((*lx)*ratio - *ly));
              (*ly)=(int)((*lx)*ratio);
            }
          else
            {
              /* Choose the vertical delta */
              (*ly)+=sy*dy;
              dx = (int)(sx*((*ly)/ratio - *lx));
              (*lx)=(int)((*ly)/ratio);
            }

        }
    }
  else
    {
      /* Resize without ratio */
      (*lx)+=sx*dx;
      (*ly)+=sy*dy;
    }

  /* Change the coordinates (*x,*y) of the box */
  if (sx<0)(*x)+=dx;
  if (sy<0)(*y)+=dy;
}

IMPLEMENT_DYNAMIC_CLASS(AmayaEditShapeEvtHandler, wxEvtHandler)

  /*----------------------------------------------------------------------
   *  this is where the event table is declared
   *  the callbacks are assigned to an event type
   *----------------------------------------------------------------------*/
  BEGIN_EVENT_TABLE(AmayaEditShapeEvtHandler, wxEvtHandler)
  EVT_KEY_DOWN ( AmayaEditShapeEvtHandler::OnKeyDown )
  EVT_LEFT_DOWN(	AmayaEditShapeEvtHandler::OnMouseDown)
  EVT_LEFT_UP(		AmayaEditShapeEvtHandler::OnMouseUp)
  EVT_LEFT_DCLICK(	AmayaEditShapeEvtHandler::OnMouseDbClick)
  EVT_MIDDLE_DOWN(	AmayaEditShapeEvtHandler::OnMouseDown)
  EVT_MIDDLE_UP(	AmayaEditShapeEvtHandler::OnMouseUp)
  EVT_MIDDLE_DCLICK(	AmayaEditShapeEvtHandler::OnMouseDbClick)
  EVT_RIGHT_DOWN(	AmayaEditShapeEvtHandler::OnMouseDown)
  EVT_RIGHT_UP(		AmayaEditShapeEvtHandler::OnMouseUp)
  EVT_RIGHT_DCLICK(	AmayaEditShapeEvtHandler::OnMouseDbClick)
  EVT_MOTION(		AmayaEditShapeEvtHandler::OnMouseMove)
  EVT_MOUSEWHEEL(	AmayaEditShapeEvtHandler::OnMouseWheel)
  END_EVENT_TABLE()

/*----------------------------------------------------------------------
*----------------------------------------------------------------------*/
  AmayaEditShapeEvtHandler::AmayaEditShapeEvtHandler() : wxEvtHandler()
{
}

/*----------------------------------------------------------------------
 *----------------------------------------------------------------------*/
AmayaEditShapeEvtHandler::AmayaEditShapeEvtHandler (AmayaFrame * p_frame,
                                                    Document doc,
                                                    void *inverse,
                                                    int ancestorX,
                                                    int ancestorY,
                                                    int canvasWidth,
                                                    int canvasHeight,
                                                    Element element,
                                                    int point,
                                                    ThotBool *hasBeenEdited)
  : wxEvtHandler()
  ,finished(false)
  ,pFrame(p_frame)
  ,frameId(p_frame->GetFrameId())
  ,inverse(inverse)
  ,x0(ancestorX)
  ,y0(ancestorY)
  ,width(canvasWidth)
  ,height(canvasHeight)
  ,el(element)
  ,point(point)
  ,hasBeenEdited(hasBeenEdited)
  ,x_org(0)
  ,y_org(0)
{
  *hasBeenEdited = FALSE;
  buttonDown = false;
  document = doc;
  if (pFrame)
    {
      /* attach this handler to the canvas */
      AmayaCanvas * p_canvas = pFrame->GetCanvas();
      p_canvas->PushEventHandler(this);

      /* assign a cross mouse cursor */
      pFrame->GetCanvas()->SetCursor( wxCursor(wxCURSOR_CROSS) );
      pFrame->GetCanvas()->CaptureMouse();
    }

  /* Get the box of the SVG element */
  e_ab = ((PtrElement)el)->ElAbstractBox[0];
  if (!e_ab || !(e_ab->AbBox))
    {
      finished = true;
      return;
    }
  e_box = e_ab->AbBox;

  /* Get the GRAPHICS leaf */
  leaf = GetGraphicsUnit(el);
  if (!leaf)
    {
      finished = true;
      return;
    }

  /* Get the box of the SVG element */
  ab = ((PtrElement)leaf)->ElAbstractBox[0];
  if (!ab || !(ab->AbBox))
    {
      finished = true;
      return;
    }

  box = ab->AbBox;
  shape = ab->AbShape;
  // should rx and ry be updated with the same ratio
  Angle_ratio = (point == 10 && box->BxRx <= 2 && box->BxRy <= 2);
  if (point != 9 && point != 10 && (box->BxXOrg || box->BxYOrg))
    {
      /* Move the origin */
      x_org = box->BxXOrg;
      y_org = box->BxYOrg;
      if (shape != 1 && shape != 'C' && shape != 'a' &&  shape != 'c' &&
          shape != 'g' && shape != 7 && shape != 8)
         {
          box->BxXOrg = 0;
          box->BxYOrg = 0;
          TtaAppendMatrixTransform (document, el, 1, 0, 0, 1, x_org, y_org);
          RedisplayLeaf ((PtrElement) leaf, document, 0);
          DefBoxRegion (frameId, box, -1, -1, -1, -1);
          RedrawFrameBottom (frameId, 0, NULL);
          pFrame->GetCanvas()->Refresh();
        }
    }
}

/*----------------------------------------------------------------------
 *----------------------------------------------------------------------*/
AmayaEditShapeEvtHandler::~AmayaEditShapeEvtHandler()
{
  if (pFrame)
    {
      /* detach this handler from the canvas (restore default behaviour) */
      AmayaCanvas * p_canvas = pFrame->GetCanvas();
      p_canvas->ReleaseMouse();
      p_canvas->PopEventHandler(false /* do not delete myself */);
      
      /* restore the default cursor */
      p_canvas->SetCursor( wxNullCursor );
    }
}

/*----------------------------------------------------------------------
  IsFinish
  *----------------------------------------------------------------------*/
bool AmayaEditShapeEvtHandler::IsFinish()
{
  return finished;
}

/*----------------------------------------------------------------------
  OnKeyDown
  *----------------------------------------------------------------------*/
void AmayaEditShapeEvtHandler::OnKeyDown( wxKeyEvent& event )
{
  if (event.GetKeyCode() !=  WXK_SHIFT)
    {
      *hasBeenEdited = FALSE;
      TtaSetStatus (document, 1, "", NULL);
      finished = true;
    }
}

/*----------------------------------------------------------------------
 *       Class:  AmayaEditShapeEvtHandler
 *      Method:  OnMouseDown
 * Description:  handle mouse button down events
 -----------------------------------------------------------------------*/
void AmayaEditShapeEvtHandler::OnMouseDown( wxMouseEvent& event )
{
}

/*----------------------------------------------------------------------
 *       Class:  AmayaEditShapeEvtHandler
 *      Method:  OnMouseUp
 * Description:  handle mouse button up events
 -----------------------------------------------------------------------*/
void AmayaEditShapeEvtHandler::OnMouseUp( wxMouseEvent& event )
{
  TtaSetStatus (document, 1, "", NULL);
  finished = true;
}

/*----------------------------------------------------------------------
 *       Class:  AmayaEditShapeEvtHandler
 *      Method:  OnMouseDbClick
 * Description:  handle mouse dbclick events
 -----------------------------------------------------------------------*/
void AmayaEditShapeEvtHandler::OnMouseDbClick( wxMouseEvent& event )
{
  TtaSetStatus (document, 1, "", NULL);
  finished = true;
}

/*----------------------------------------------------------------------
 *       Class:  AmayaEditShapeEvtHandler
 *      Method:  OnMouseMove
 * Description:  handle mouse move events
 -----------------------------------------------------------------------*/
void AmayaEditShapeEvtHandler::OnMouseMove( wxMouseEvent& event )
{
#define RATIO_EQUILATERAL sqrt((float)3)/2.

  ThotBool same_size;
  int      rx, ry, lx, ly, x, y;
  int      x1, y1, x2, y2, dx, dy;
  float    ratio = 0.;

  if (IsFinish())
    return;
  /* DELTA is the sensitivity toward mouse moves. */
#define DELTA 0

  /* Update the current mouse coordinates */
  mouse_x = event.GetX();
  mouse_y = event.GetY();
  if (!buttonDown)
    {
      lastX = mouse_x;
      lastY = mouse_y;
      buttonDown = true;
    }

  if ((abs(mouse_x -lastX) + abs(mouse_y - lastY) > DELTA))
    {
      x1 = lastX;
      y1 = lastY;
      x2 = mouse_x;
      y2 = mouse_y;
      MouseCoordinatesToSVG (document, pFrame, x0, y0, width, height,
                             inverse, TRUE, NULL,
                             &x1, &y1, FALSE);
      MouseCoordinatesToSVG (document, pFrame, x0, y0, width, height,
                             inverse, TRUE, NULL,
                             &x2, &y2, FALSE);

      dx = x2 - x1;
      dy = y2 - y1;

      /*               lx
       *         <------------->
       *                       
       *       (x,y)------O-rx-.    ^
       *         |             |    |
       *         |             ry   |
       *         |             |    |
       *         |             O    |    
       *         |             |    |    
       *         |             |    |ly    
       *         |             |    |    
       *         |             |    |    
       *         |             |    |    
       *         |             |    |    
       *         .-------------.    v
       */


      lx = box->BxW;
      ly = box->BxH;
      x = x_org;
      y = y_org;

      /*                                   9 = Rx handle       */
      /*                                  /                    */
      /*            1-------------2------O------3              */
      /*            |                           O              */
      /*            |                           |\             */
      /*            8                           4 \            */
      /*            |                           |  10 = Ry     */
      /*            |                           |       Handle */
      /*            7-------------6-------------5              */

      switch(shape)
        {
        case 1: /* square */
        case 2: /* Parallelogram */
        case 3: /* Trapezium */
        case 'C': /* rectangle */
        case 'a': /* circle */
        case 'c': /* ellipse */
        case 'L': /* diamond */
        case 4: /* Equilateral triangle (ly = lx*R) */
        case 5: /* Isosceles triangle */
        case 6: /* Rectangle triangle */
        case 7: /* square */
        case 8: /* rectangle */
          if (shape == 1 || shape == 'C') /* square and rectangle with rounded corner */
            {
              rx = box->BxRx;
              ry = box->BxRy;
              if (rx != -1 && ry == -1)
                ry = rx;
              else if (rx == -1 && ry != -1)
                rx = ry;
            }
          else if (shape == 2) /* parallelogram */
            rx = box->BxRx;
          else if (shape == 3) /* trapezium */
            {
              rx = box->BxRx;
              ry = box->BxRy;
            }

          if (1 <= point && point <= 8)
            {
              same_size = (shape == 1 || shape == 7 || shape == 'a');
              if (same_size)ratio = 1.;
            }
          else
            same_size = (box->BxRx == -1 || box->BxRy == -1);

          if (shape == 4)
            ratio = RATIO_EQUILATERAL;

          if (shape == 6 && point == 5)
            {
              /* The point is actually the middle of the hypot */
              dx *= 2;
              dy *= 2;
            }

          switch(point)
            {
            case 1:
              apply_deltas(&x, &y, &lx, &ly, dx, dy, -1, -1, ratio);
              break;
	      
            case 2:
              apply_deltas(&x, &y, &lx, &ly, dx, dy, 0, -1, ratio);
              break;
	      
            case 3:
              apply_deltas(&x, &y, &lx, &ly, dx, dy, +1, -1, ratio);
              break;

            case 4:
              apply_deltas(&x, &y, &lx, &ly, dx, dy, +1, 0, ratio);
              break;
	      
            case 5:
              apply_deltas(&x, &y, &lx, &ly, dx, dy, +1, +1, ratio);
              break;
	      
            case 6:
              apply_deltas(&x, &y, &lx, &ly, dx, dy, 0, +1, ratio);
              break;
	      
            case 7:
              apply_deltas(&x, &y, &lx, &ly, dx, dy, -1, +1, ratio);
              break;
	      
            case 8:
              apply_deltas(&x, &y, &lx, &ly, dx, dy, -1, 0, ratio);
              break;
	      
            case 9:
              // change the arc size
              if (shape == 1 || shape == 'C') /* Square Rectangle */
                {
                  rx -= dx;
                  if (rx > lx/2)
                    rx = lx/2;
                  if (same_size)
                    {
                      if (rx > ly/2)
                        rx = ly/2;
                      ry = rx;
                    }
                }
              else if (shape == 2) /* Parallelogram */
                rx += dx;
              break;
	      
            case 10:
              // change the arc size
              if (shape == 1 || shape == 'C') /* Square Rectangle */
                {
                  ry += dy;
                  if (ry > ly/2)
                    ry = ly/2;
                  if (same_size || Angle_ratio)
                    {
                      if (ry > lx/2)
                        ry = lx/2;
                      rx = ry;
                    }
                }
              else if (shape == 2) /* Parallelogram */
                rx -= dx;
              break;
            }
	  
          if (lx < 0){lx = 0; x = x_org;}
          if (ly < 0){ly = 0; y = y_org;}
          if (shape == 1 || shape == 'C') /* Square Rectangle */
            {
              if (rx > lx/2)
                rx = lx/2;
              if (ry > ly/2)
                ry = ly/2;
	      
              if (box->BxRx != -1)
                box->BxRx = rx;
              if (box->BxRy != -1)
                box->BxRy = ry;
            }
          else if (shape == 2) /* Parallelogram */
            {
              if (rx < 0)rx = 0;
              if (rx > lx)rx = lx;
              box->BxRx = rx;
            }
          else if (shape == 3) /* Trapezium */
            {
              if (x+abs(rx) > x+lx-abs(ry))
                {
                  box->BxRx = (rx < 0 ? -1 : 1)*(lx/2);
                  box->BxRy = (ry < 0 ? -1 : 1)*(lx/2);
                }
              else
                {
                  box->BxRx = rx;
                  box->BxRy = ry;
                }
            }
          break;

        default:
          break;
        }
      
      if (shape != 1 && shape != 'C' && shape != 'a' && shape != 'c' &&
          shape != 'g' && shape != 7 && shape != 8)
        {
          TtaAppendMatrixTransform (document, el, 1, 0, 0, 1, x - x_org, y - y_org);
          box->BxXOrg = 0;
          box->BxYOrg = 0;
        }
      else if (point != 9 && point != 10)
        {
          box->BxXOrg = x;
          box->BxYOrg = y;
        }
      x_org = x;
      y_org = y;
      box->BxW = lx;
      box->BxH = ly;
      box->BxWidth = lx;
      box->BxHeight = ly;
      e_box->BxW = lx;
      e_box->BxH = ly;
      e_box->BxWidth = lx;
      e_box->BxHeight = ly;
      /* Redisplay the GRAPHICS leaf */
      RedisplayLeaf ((PtrElement) leaf, document, 0);

      /* Update the previous mouse coordinates */
      lastX = mouse_x;
      lastY = mouse_y;

      *hasBeenEdited = TRUE;
    }
}

/*----------------------------------------------------------------------
 *       Class:  AmayaEditShapeEvtHandler
 *      Method:  OnMouseWheel
 * Description:  handle mouse wheel events
 -----------------------------------------------------------------------*/
void AmayaEditShapeEvtHandler::OnMouseWheel( wxMouseEvent& event )
{
}
#endif /* _WX */
