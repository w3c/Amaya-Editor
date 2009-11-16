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
#include "windowdisplay_f.h"

#ifdef _GL
#include "glwindowdisplay.h"
#endif /* _GL */

#include "logdebug.h"

/* DELTA is the sensitivity toward mouse moves. */
#define DELTA 0
#define CURSOR_SIZE 8

#include "AmayaFrame.h"
#include "AmayaCanvas.h"
#include "AmayaTransformEvtHandler.h"
#include "AmayaTransformMouseCursors.h"

/*----------------------------------------------------------------------
  This module is called when the user want to apply transformations to an
  SVG element. Current interaction is indicated by several parameters:

  - Finished: Indicate whether the interaction must continue or stop.
  - ButtonDown: Indicate that the user is pressing a button of the mouse.
  - type: Indicate the type and sub-mode of the transformation:
  0) Translation
  1) Scaling
  2) Rotation
  3) Rotation, moving the center.
  4) Skewing
  5) Skewing along X axis, using the top arrow.
  6) Skewing along X axis, using the bottom arrow.
  7) Skewing along Y axis, using the left arrow.
  8) Skewing along Y axis, using the right arrow.
  9) Scaling: top border
  10) Scaling: bottom border
  11) Scaling: left border
  12) Scaling: right border
  13) Scaling: top left corner
  14) Scaling: top right corner
  15) Scaling: bottom right corner
  16) Scaling: bottom left corner
  17) Translate
  *----------------------------------------------------------------------*/
extern void GetPositionAndSizeInParentSpace(Document doc, Element el, float *X,
                                            float *Y, float *width,
                                            float *height);

IMPLEMENT_DYNAMIC_CLASS(AmayaTransformEvtHandler, wxEvtHandler)

  /*----------------------------------------------------------------------
   *  this is where the event table is declared
   *  the callbacks are assigned to an event type
   *----------------------------------------------------------------------*/
  BEGIN_EVENT_TABLE(AmayaTransformEvtHandler, wxEvtHandler)
  EVT_KEY_DOWN( AmayaTransformEvtHandler::OnChar )
  EVT_LEFT_DOWN(AmayaTransformEvtHandler::OnMouseDown) 
  EVT_LEFT_UP(AmayaTransformEvtHandler::OnMouseUp)
  EVT_LEFT_DCLICK(AmayaTransformEvtHandler::OnMouseDbClick)
  EVT_MIDDLE_DOWN(AmayaTransformEvtHandler::OnMouseDown)
  EVT_MIDDLE_UP(AmayaTransformEvtHandler::OnMouseUp)
  EVT_MIDDLE_DCLICK(AmayaTransformEvtHandler::OnMouseDbClick)
  EVT_RIGHT_DOWN(AmayaTransformEvtHandler::OnMouseDown)
  EVT_RIGHT_UP(AmayaTransformEvtHandler::OnMouseUp)
  EVT_RIGHT_DCLICK(AmayaTransformEvtHandler::OnMouseDbClick)
  EVT_MOTION(AmayaTransformEvtHandler::OnMouseMove)
  EVT_MOUSEWHEEL(AmayaTransformEvtHandler::OnMouseWheel)
  END_EVENT_TABLE()

  /*----------------------------------------------------------------------
   *----------------------------------------------------------------------*/
  AmayaTransformEvtHandler::AmayaTransformEvtHandler() : wxEvtHandler()
{
}

/*----------------------------------------------------------------------
 *----------------------------------------------------------------------*/
AmayaTransformEvtHandler::AmayaTransformEvtHandler(AmayaFrame * p_frame,
                                                   Document doc,
                                                   void *CTM,
                                                   void *inverse,
                                                   int ancestorX,
                                                   int ancestorY,
                                                   int canvasWidth,
                                                   int canvasHeight,
                                                   int transform_type,
                                                   Element element,
                                                   ThotBool *transformApplied)
  :wxEvtHandler()
  ,Finished(false)
  ,pFrame(p_frame)
  ,FrameId(p_frame->GetFrameId())
  ,document(doc)
  ,CTM(CTM)
  ,inverse(inverse)
  ,x0(ancestorX)
  ,y0(ancestorY)
  ,width(canvasWidth)
  ,height(canvasHeight)
  ,type(transform_type)
  ,el(element)
  ,box(NULL)
  ,ButtonDown(false)
  ,hasBeenTransformed(transformApplied)
{
  PtrAbstractBox pAb;
  wxCursor       cursor;

  *hasBeenTransformed = FALSE;
  pAb = ((PtrElement)el)->ElAbstractBox[0];
  if (!pAb || !pAb->AbBox)
    {
      Finished = true;
      return;
    }

  box = pAb->AbBox;
  // get the minimum size of the box
  size = box->BxClipW;
  if (box->BxClipH < box->BxClipW)
    size = box->BxClipH;
  if (pFrame)
    {
      /* attach this handler to the canvas */
      AmayaCanvas * p_canvas = pFrame->GetCanvas();
      p_canvas->PushEventHandler(this);

      if (type > 0)
        {
          switch(type)
            {
            case 1:
              /* Scale */
              cursor = wxCursor(cursor_scale_xpm);
              break;

            case 2:
              /* Rotate */
              cursor = wxCursor(cursor_rotate_xpm);
              break;

            case 4:
              /* Skewing */
              cursor = wxCursor(cursor_skew_xpm);
              break;

            case 17:
              /* Translate */
              cursor = wxCursor(cursor_translate_xpm);
              break;

            default:
              cursor = wxCursor(wxCURSOR_CROSS);
              break;
            }
          /* assign a cross mouse cursor */
          p_canvas->SetCursor( cursor );
        }
      p_canvas->CaptureMouse();
    }
  AmayaTransformEvtHandler::UpdatePositions();
}

/*----------------------------------------------------------------------
 *----------------------------------------------------------------------*/
AmayaTransformEvtHandler::~AmayaTransformEvtHandler()
{
  /* Clear the center */
  if (type == 2 || type == 3)
    DrawRotationCenter();

  if (pFrame)
    {
      /* detach this handler from the canvas (restore default behaviour) */
      AmayaCanvas * p_canvas = pFrame->GetCanvas();
      p_canvas->ReleaseMouse();
      p_canvas->PopEventHandler(false /* do not delete myself */);
      
      /* restore the default cursor */
      if (type > 0)
        {
          TtaSetStatus (document, 1, "", NULL);
          p_canvas->SetCursor( wxNullCursor );
        }
    }
}

/*----------------------------------------------------------------------
 *----------------------------------------------------------------------*/
bool AmayaTransformEvtHandler::IsFinish()
{
  return Finished;
}

/*----------------------------------------------------------------------
 *----------------------------------------------------------------------*/
void AmayaTransformEvtHandler::OnChar( wxKeyEvent& event )
{
  if (event.GetKeyCode() !=  WXK_SHIFT)
    {
      TtaSetStatus (document, 1, "", NULL);
      Finished = true;
    }
}

/*----------------------------------------------------------------------
 *       Class:  AmayaTransformEvtHandler
 *      Method:  OnMouseDown
 * Description:  handle mouse button down events
 -----------------------------------------------------------------------*/
void AmayaTransformEvtHandler::OnMouseDown( wxMouseEvent& event )
{
  if (IsFinish())
    return;

  ButtonDown = true;
  lastX = mouse_x;
  lastY = mouse_y;
  switch(type)
    {
    case 2:
      /* Rotation: is the user clicking on the center ? */
      if (IsNear(cx2, cy2))
        type = 3;

      hasBeenRotated = false;
      break;

    case 1:
      /* Scaling: is the user clicking on an arrow ? */
      if (IsNear((left2+right2)/2, top2))
        type = 9;
      else if (IsNear((left2+right2)/2, bottom2))
        type = 10;
      else if (IsNear(left2, (top2+bottom2)/2))
        type = 11;
      else if (IsNear(right2, (top2+bottom2)/2))
        type = 12;
      else  if (IsNear(left2, top2))
        type = 13;
      else if (IsNear(right2, top2))
        type = 14;
      else if (IsNear(right2, bottom2))
        type = 15;
      else if (IsNear(left2, bottom2))
        type = 16;
      else Finished = true;

      /* Clear the arrows */
      DrawScalingArrows();
      break;

    case 4:
      /* Skewing: is the user clicking on an arrow ? */

      if (IsNear((left2+right2)/2, top2))
        type = 5;
      else if (IsNear((left2+right2)/2, bottom2))
        type = 6;
      else if (IsNear(left2, (top2+bottom2)/2))
        type = 7;
      else if (IsNear(right2, (top2+bottom2)/2))
        type = 8;
      else Finished = true;

      /* Clear the arrows */
      DrawSkewingArrows();
      break;
    }
}

/*----------------------------------------------------------------------
 *       Class:  AmayaTransformEvtHandler
 *      Method:  OnMouseUp
 * Description:  handle mouse button up events
 -----------------------------------------------------------------------*/
void AmayaTransformEvtHandler::OnMouseUp( wxMouseEvent& event )
{
  int   x, y;

  if (IsFinish())
    return;
  switch(type)
    {
    case 2:
      /* The user was rotating the shape */
      ButtonDown = false;
      if (!hasBeenRotated)
        {
          /* The user clicked but didn't move */
          TtaSetStatus (document, 1, "", NULL);
          Finished = true;
        }
      break;

    case 3:
      /* The user was moving the center: come back to the initial interface  */
      type = 2;
      /* Get the new coordinates in the mouse space... */
      x = cx2;
      y = cy2;
      /* ...and in the SVG canvas */
      MouseCoordinatesToSVG(document, pFrame, x0, y0, width, height,
                            inverse, TRUE, NULL,
                            &x, &y, FALSE);
      cx = (float)x;
      cy = (float)y;
      ButtonDown = false;
      break;

    case 5:
    case 6:
    case 7:
    case 8:
      /* The user was skewing the shape: come back to the initial interface */
      type = 4;
      ButtonDown = false;
      /* Redisplay the arrows */
      AmayaTransformEvtHandler::UpdatePositions();
      break;

    case 9:
    case 10:
    case 11:
    case 12:
    case 13:
    case 14:
    case 15:
    case 16:
      /* The user was scaling the shape: come back to the initial interface */
      type = 1;      
      ButtonDown = false;

      /* Redisplay the arrows */
      AmayaTransformEvtHandler::UpdatePositions();
      break;

    case 17:
      /* Translate */
      ButtonDown = false;
      break;

    default:
      TtaSetStatus (document, 1, "", NULL);
      Finished = true;
      break;
    }
}

/*----------------------------------------------------------------------
 *       Class:  AmayaTransformEvtHandler
 *      Method:  OnMouseDbClick
 * Description:  handle mouse dbclick events
 -----------------------------------------------------------------------*/
void AmayaTransformEvtHandler::OnMouseDbClick( wxMouseEvent& event )
{
  TtaSetStatus (document, 1, "", NULL);
  Finished = true;
}

/*----------------------------------------------------------------------
 *       Class:  AmayaTransformEvtHandler
 *      Method:  OnMouseMove
 * Description:  handle mouse move events
 -----------------------------------------------------------------------*/
void AmayaTransformEvtHandler::OnMouseMove( wxMouseEvent& event )
{
  int      x1, y1, x2, y2;
  float    theta, dx1, dx2, dy1, dy2, d;
  float    det;
  float    skew, sx, sy;
  ThotBool shift = event.ShiftDown();
  char    *msg;

  /* Update the current mouse coordinates */
  mouse_x = event.GetX();
  mouse_y = event.GetY();

  if (!ButtonDown && type == 0)
    {
      /* Translate: when the module is called, the user has already
         pressed the button of the mouse. */
      lastX = mouse_x;
      lastY = mouse_y;
      ButtonDown = true;
    }

  if (ButtonDown && (abs(mouse_x -lastX) + abs(mouse_y - lastY) > DELTA))
    {
      /* The user is pressing the mouse button and moving */

      /* Translate the previous and current coordinates of the mouse
         into the correspoint of the SVG canvas:
         - previous position: (x1,y1)
         - new position: (x2,y2)
      */
      x1 = lastX;
      y1 = lastY;
      x2 = mouse_x;
      y2 = mouse_y;
      MouseCoordinatesToSVG(document, pFrame, x0, y0, width, height,
                            inverse, TRUE, NULL,
                            &x1, &y1, FALSE);

      if(type == 0)
        msg = TtaGetMessage (LIB, BUTTON_UP);
      else
        msg = TtaGetMessage (LIB, TMSG_DOUBLECLICK);

      MouseCoordinatesToSVG(document, pFrame, x0, y0, width, height,
                            inverse, TRUE, msg,
                            &x2, &y2, FALSE);

      /* If it is a rotation, clear the center */
      if (type == 2 || type == 3)
        DrawRotationCenter();

      switch(type)
        {
        case 0:
          /* Translate */
        case 17:
          /* Translate (using command) */
          /* We want to apply a translation such that (x1,y1) is moved to
             (x2,y2). Hence the vector of translation is simply defined by:

             ->  (x2 - x1)
             v = (y2 - y1)

             (x1,y1)----------------->(x2,y2)

          */

          TtaApplyMatrixTransform (document, el, 1, 0, 0, 1, x2 - x1, y2 - y1);

          *hasBeenTransformed = TRUE;
          break;

        case 2:
          /* Rotate */

          /*   We want to apply a rotation of angle theta and center (cx,cy)
               such that (x1,y1) is moved to (x2,y2)

               (dx1)               (x2,y2)
               v1= (dy1)                /
               -->    /___
               v2    /    _
               /     theta
               (dx2)            /        |
               v2= (dy2)           /         | 
               (cx,cy)-----------------(x1,y1)

               -->
               d = ||v1||*||v2||             v1
          */

          dx1 = (x1 - cx);
          dx2 = (x2 - cx);
          dy1 = (y1 - cy);
          dy2 = (y2 - cy);
          d = sqrt((dx1*dx1 + dy1*dy1)*(dx2*dx2 + dy2*dy2));
	  
          if (d > 0)
            {
              /* Here is how to get theta:

              {v1.v2 = d*cos(theta)
              {
              {             |dx1 dx2|
              {det(v1,v2) = |dy1 dy2| = d*sin(theta)

              => theta = sign(det(v1,v2))*acos((v1.v2)/d)
                        
              */
              det = dx1*dy2 - dy1*dx2;
              theta = acos((dx1*dx2 + dy1*dy2)/d);
              if (det < 0)
                theta = -theta;
              /* Apply rotate(theta,cx,cy) */
              TtaApplyMatrixTransform (document, el, 1, 0, 0, 1, -cx, -cy);
              TtaApplyMatrixTransform (document, el,
                                       cos(theta), sin(theta),
                                       -sin(theta), cos(theta),
                                       0,0);
              TtaApplyMatrixTransform (document, el, 1, 0, 0, 1, +cx, +cy);

              hasBeenRotated = true;
              *hasBeenTransformed = TRUE;
            }

          break;

        case 3:
          /* Moving center of rotation  */
          cx2 = mouse_x; // final value will be done on mouse up
          cy2 = mouse_y; // final value will be done on mouse up
          DrawRotationCenter();
          break;

        case 5:
        case 6:
          /* Skewing along X axis */
          /* Move the shape for its center to be at the origin */
          TtaApplyMatrixTransform (document, el, 1, 0, 0, 1, -cx, -cy);
          x1-=(int)cx;
          x2-=(int)cx;

          if (type == 5)
            /* Using the top arrow */
            y1 = y2 = (int)(top - cy);
          else
            /* Using the bottom arrow */
            y1 = y2 = (int)(bottom - cy);

          /* We want to apply a horizontal skew such that
             (x1,y1) is moved to (x2,y2) :

             (x2)   (1   skew) (x1)   y1=y2
             (y2) = (       1) (y1)
          */ 
          skew = ((float)(x2 - x1))/y1;
          TtaApplyMatrixTransform (document, el,
                                   1, 0,
                                   skew,
                                   1,
                                   0,0);

          /* Move the shape to its initial position */
          TtaApplyMatrixTransform (document, el, 1, 0, 0, 1, +cx, +cy);
          *hasBeenTransformed = TRUE;
          break;

        case 7:
        case 8:
          /* Skewing along Y axis */

          /* Move the shape for its center to be at the origin */
          TtaApplyMatrixTransform (document, el, 1, 0, 0, 1, -cx, -cy);
          y1-=(int)cy;
          y2-=(int)cy;

          if (type == 7)
            /* Using the left hand side arrow */
            x1 = x2 = (int)(left - cx);
          else
            /* Using the right hand side arrow */	 
            x1 = x2 = (int)(right - cx);

          /* We want to apply a vertical skew such that
             (x1,y1) is moved to (x2,y2) :

             (x2)   (1    0) (x1)     x1=x2
             (y2) = (skew 1) (y1)
          */ 
          skew = ((float)(y2 - y1))/x1;
          TtaApplyMatrixTransform (document, el, 1, skew, 0, 1, 0,0);

          /* Move the shape to its initial position */
          TtaApplyMatrixTransform (document, el, 1, 0, 0, 1, +cx, +cy);
          *hasBeenTransformed = TRUE;
          break;

        case 9:
          /* Scaling: top border */
          y1-=(int)bottom;
          y2-=(int)bottom;

          /* We want to apply a vertical scale such that
             (x1,y1) is moved to (x2,y2) :

             (x2)   (1    0) (x1)     x1=x2=0
             (y2) = (0   sy) (y1)
          */ 

          if (y1 != 0 && y2 != 0)
            {
              TtaApplyMatrixTransform (document, el, 1, 0, 0, 1, -cx, -bottom);
              sy = ((float)y2)/y1;

              if (!shift) 
                sx = 1;
              else
                /* Preserve aspect ratio */
                sx = sy;

              TtaApplyMatrixTransform (document, el, sx, 0, 0, sy, 0, 0);
              TtaApplyMatrixTransform (document, el, 1, 0, 0, 1, +cx, +bottom);
              *hasBeenTransformed = TRUE;
            }
          break;

        case 10:
          /* Scaling: bottom border */
          y1-=(int)top;
          y2-=(int)top;

          /* We want to apply a vertical scale such that
             (x1,y1) is moved to (x2,y2) :

             (x2)   (1    0) (x1)     x1=x2=0
             (y2) = (0   sy) (y1)
          */ 

          if (y1 != 0 && y2 != 0)
            {
              TtaApplyMatrixTransform (document, el, 1, 0, 0, 1, -cx, -top);
              sy = ((float)y2)/y1;

              if (!shift) 
                sx = 1;
              else
                /* Preserve aspect ratio */
                sx = sy;

              TtaApplyMatrixTransform (document, el, sx, 0, 0, sy, 0, 0);
              TtaApplyMatrixTransform (document, el, 1, 0, 0, 1, +cx, +top);
              *hasBeenTransformed = TRUE;
            }
          break;

        case 11:
          /* Scaling: left border */
          x1-=(int)right;
          x2-=(int)right;

          /* We want to apply an horizontal scale such that
             (x1,y1) is moved to (x2,y2) :

             (x2)   (sx  0) (x1)     y1=y2=0
             (y2) = (0   1) (y1)
          */ 

          if (x1 != 0 && x2 != 0)
            {
              TtaApplyMatrixTransform (document, el, 1, 0, 0, 1, -right, -cy);
              sx = ((float)x2)/x1;

              if (!shift) 
                sy = 1;
              else
                /* Preserve aspect ratio */
                sy = sx;

              TtaApplyMatrixTransform (document, el, sx, 0, 0, sy, 0, 0);
              TtaApplyMatrixTransform (document, el, 1, 0, 0, 1, +right, +cy);
              *hasBeenTransformed = TRUE;
            }
          break;

        case 12:
          /* Scaling: right border */
          x1-=(int)left;
          x2-=(int)left;

          /* We want to apply an horizontal scale such that
             (x1,y1) is moved to (x2,y2) :

             (x2)   (sx  0) (x1)     y1=y2=0
             (y2) = (0   1) (y1)
          */ 

          if (x1 != 0 && x2 != 0)
            {
              TtaApplyMatrixTransform (document, el, 1, 0, 0, 1, -left, -cy);
              sx = ((float)x2)/x1;

              if (!shift) 
                sy = 1;
              else
                /* Preserve aspect ratio */
                sy = sx;

              TtaApplyMatrixTransform (document, el, sx, 0, 0, sy, 0, 0);
              TtaApplyMatrixTransform (document, el, 1, 0, 0, 1, +left, +cy);
              *hasBeenTransformed = TRUE;
            }
          break;

        case 13:
          /* Scaling: top left corner */
          x1-=(int)right;
          x2-=(int)right;
          y1-=(int)bottom;
          y2-=(int)bottom;

          /* We want to apply a scale such that
             (x1,y1) is moved to (x2,y2) :

             (x2)   (sx  0) (x1)    
             (y2) = (0   sy) (y1)
          */ 

          if (x1 != 0 && x2 != 0 && y1 !=0 && y2 != 0)
            {
              TtaApplyMatrixTransform (document, el, 1, 0, 0, 1,
                                       -right, -bottom);
              sx = ((float)x2)/x1;
              sy = ((float)y2)/y1;

              if (shift) 
                /* Preserve aspect ratio */
                {
                  if (sx < sy)
                    sy = sx;
                  else
                    sx = sy;
                }

              TtaApplyMatrixTransform (document, el, sx, 0, 0, sy, 0, 0);
              TtaApplyMatrixTransform (document, el, 1, 0, 0, 1,
                                       +right, +bottom);
              *hasBeenTransformed = TRUE;
            }

          break;

        case 14:
          /* Scaling: top right corner */
          x1-=(int)left;
          x2-=(int)left;
          y1-=(int)bottom;
          y2-=(int)bottom;

          /* We want to apply a scale such that
             (x1,y1) is moved to (x2,y2) :

             (x2)   (sx  0) (x1)    
             (y2) = (0   sy) (y1)
          */ 

          if (x1 != 0 && x2 != 0 && y1 !=0 && y2 != 0)
            {
              TtaApplyMatrixTransform (document, el, 1, 0, 0, 1,
                                       -left, -bottom);
              sx = ((float)x2)/x1;
              sy = ((float)y2)/y1;

              if (shift) 
                /* Preserve aspect ratio */
                {
                  if (sx < sy)
                    sy = sx;
                  else
                    sx = sy;
                }

              TtaApplyMatrixTransform (document, el, sx, 0, 0, sy, 0, 0);
              TtaApplyMatrixTransform (document, el, 1, 0, 0, 1,
                                       +left, +bottom);
              *hasBeenTransformed = TRUE;
            }

          break;

        case 15:
          /* Scaling: bottom right corner */
          x1-=(int)left;
          x2-=(int)left;
          y1-=(int)top;
          y2-=(int)top;

          /* We want to apply a scale such that
             (x1,y1) is moved to (x2,y2) :

             (x2)   (sx  0) (x1)    
             (y2) = (0   sy) (y1)
          */ 

          if (x1 != 0 && x2 != 0 && y1 !=0 && y2 != 0)
            {
              TtaApplyMatrixTransform (document, el, 1, 0, 0, 1,
                                       -left, -top);
              sx = ((float)x2)/x1;
              sy = ((float)y2)/y1;

              if (shift) 
                /* Preserve aspect ratio */
                {
                  if (sx < sy)
                    sy = sx;
                  else
                    sx = sy;
                }

              TtaApplyMatrixTransform (document, el, sx, 0, 0, sy, 0, 0);
              TtaApplyMatrixTransform (document, el, 1, 0, 0, 1,
                                       +left, +top);
              *hasBeenTransformed = TRUE;
            }

          break;

        case 16:
          /* Scaling: bottom left corner */
          x1-=(int)right;
          x2-=(int)right;
          y1-=(int)top;
          y2-=(int)top;

          /* We want to apply a scale such that
             (x1,y1) is moved to (x2,y2) :

             (x2)   (sx  0) (x1)     y1=y2=0
             (y2) = (0   sy) (y1)
          */ 

          if (x1 != 0 && x2 != 0 && y1 !=0 && y2 != 0)
            {
              TtaApplyMatrixTransform (document, el, 1, 0, 0, 1,
                                       -right, -top);
              sx = ((float)x2)/x1;
              sy = ((float)y2)/y1;

              if (shift) 
                /* Preserve aspect ratio */
                {
                  if (sx < sy)
                    sy = sx;
                  else
                    sx = sy;
                }

              TtaApplyMatrixTransform (document, el, sx, 0, 0, sy, 0, 0);
              TtaApplyMatrixTransform (document, el, 1, 0, 0, 1,
                                       +right, +top);
              *hasBeenTransformed = TRUE;
            }
          break;

        default:
          break;
        }

      /* Redisplay the SVG canvas to see the transform applied to the object */
      DefBoxRegion (FrameId, box, -1, -1, -1, -1);
      RedrawFrameBottom (FrameId, 0, NULL);
      pFrame->GetCanvas()->Refresh();

      /* If it is a rotation, redraw the center */
      if (type == 2 || type == 3)
        DrawRotationCenter();

      /* Update the previous mouse coordinates */
      lastX = mouse_x;
      lastY = mouse_y;
    }
}

/*----------------------------------------------------------------------
 *       Class:  AmayaTransformEvtHandler
 *      Method:  OnMouseWheel
 * Description:  handle mouse wheel events
 -----------------------------------------------------------------------*/
void AmayaTransformEvtHandler::OnMouseWheel( wxMouseEvent& event )
{
}

/*----------------------------------------------------------------------
  DrawRotationCenter
  Draw/Clear the cross that indicate the center of rotation.
  *----------------------------------------------------------------------*/
void AmayaTransformEvtHandler::DrawRotationCenter()
{
  InitDrawing (5, 1, 0);
  glEnable(GL_COLOR_LOGIC_OP);
#ifdef _WINDOWS
  glLogicOp(GL_COPY_INVERTED);
#else /* _WINDOWS */
  glLogicOp(GL_XOR);
#endif /* _WINDOWS */

  glColor4ub (127, 127, 127, 0);

  /* Horizontal line */
  glBegin(GL_LINE_STRIP);
  glVertex2i(cx2-CURSOR_SIZE, cy2);
  glVertex2i(cx2+CURSOR_SIZE, cy2);
  glEnd ();

  /* Vertical line */
  glBegin(GL_LINE_STRIP);
  glVertex2i(cx2, cy2-CURSOR_SIZE);
  glVertex2i(cx2, cy2+CURSOR_SIZE);
  glEnd ();

  glDisable(GL_COLOR_LOGIC_OP);

#ifdef _WINDOWS
  GL_Swap (FrameId);
#endif /* WINDOWS */
}

/*----------------------------------------------------------------------
  DrawSkewingArrows
  Draw/Clear the four arrows arround the objet that indicate the skewing
  direction.
  *----------------------------------------------------------------------*/
void AmayaTransformEvtHandler::DrawSkewingArrows()
{
  InitDrawing (5, 1, 0);

  glEnable(GL_COLOR_LOGIC_OP);
#ifdef _WINDOWS
  glLogicOp(GL_COPY_INVERTED);
#else /* _WINDOWS */
  glLogicOp(GL_XOR);
#endif /* _WINDOWS */

  glColor4ub (127, 127, 127, 0);

  /*
    ----3-----
    |        |
    1        2
    |        |
    ----4-----

  */

  /* 1 */
  DrawArrow (FrameId, 1, 5,
             left2 - CURSOR_SIZE/2,
             (top2+bottom2)/2 - CURSOR_SIZE,
             CURSOR_SIZE,
             2*CURSOR_SIZE,
             90, 3, 0);

  /* 2 */
  DrawArrow (FrameId, 1, 5,
             right2 - CURSOR_SIZE/2,
             (top2+bottom2)/2 - CURSOR_SIZE,
             CURSOR_SIZE,
             2*CURSOR_SIZE,
             90, 3, 0);

  /* 3 */
  DrawArrow (FrameId, 1, 5,
             (left2+right2)/2 - CURSOR_SIZE,
             top2 - CURSOR_SIZE/2,
             2*CURSOR_SIZE,
             CURSOR_SIZE,
             0, 3, 0);

  /* 4 */
  DrawArrow (FrameId, 1, 5,
             (left2+right2)/2 - CURSOR_SIZE,
             bottom2 - CURSOR_SIZE/2,
             2*CURSOR_SIZE,
             CURSOR_SIZE,
             0, 3, 0);

  glDisable(GL_COLOR_LOGIC_OP);

#ifdef _WINDOWS
  GL_Swap (FrameId);
#endif /* WINDOWS */
}

/*----------------------------------------------------------------------
  DrawScalingArrows
  Draw/Clear the arrows arround the objet that indicate the scaling
  direction.
  *----------------------------------------------------------------------*/
void AmayaTransformEvtHandler::DrawScalingArrows()
{
  InitDrawing (5, 1, 0);

  glEnable(GL_COLOR_LOGIC_OP);
#ifdef _WINDOWS
  glLogicOp(GL_COPY_INVERTED);
#else /* _WINDOWS */
  glLogicOp(GL_XOR);
#endif /* _WINDOWS */

  glColor4ub (127, 127, 127, 0);

  /*
    5---3----6
    |        |
    1        2
    |        |
    8---4----7

  */

  /* 1 */
  DrawResizeTriangle (FrameId, CURSOR_SIZE,
                      left2, (top2+bottom2)/2,
                      0, 0, 1);

  /* 2 */
  DrawResizeTriangle (FrameId, CURSOR_SIZE,
                      right2, (top2+bottom2)/2,
                      0, 0, 3);

  /* 3 */
  DrawResizeTriangle (FrameId, CURSOR_SIZE,
                      (left2+right2)/2, top2,
                      0, 0, 0);

  /* 4 */
  DrawResizeTriangle (FrameId, CURSOR_SIZE,
                      (left2+right2)/2, bottom2,
                      0, 0, 2);

  /* 5*/
  DrawResizeTriangle (FrameId, CURSOR_SIZE,
                      left2, top2,
                      0, 0, 4);

  /* 6*/
  DrawResizeTriangle (FrameId, CURSOR_SIZE,
                      right2, top2,
                      0, 0, 5);

  /* 7*/
  DrawResizeTriangle (FrameId, CURSOR_SIZE,
                      right2, bottom2,
                      0, 0, 7);
  /* 8 */
  DrawResizeTriangle (FrameId, CURSOR_SIZE,
                      left2, bottom2,
                      0, 0, 6);

  glDisable(GL_COLOR_LOGIC_OP);
#ifdef _WINDOWS
  GL_Swap (FrameId);
#endif /* WINDOWS */
}

/*----------------------------------------------------------------------
  IsNear
  Check that the mouse is near to the point (x, y).
  *----------------------------------------------------------------------*/
bool AmayaTransformEvtHandler::IsNear(int x, int y)
{
  if (size < CURSOR_SIZE*2)
    return (abs(mouse_x - x) + abs(mouse_y - y) <= size);
  else
    return (abs(mouse_x - x) + abs(mouse_y - y) <= CURSOR_SIZE*2);
}

/*----------------------------------------------------------------------
  UpdatePositions
  Update the variables that indicate the position of the object and redraw
  the indicators: arrows, center of rotation...
  *----------------------------------------------------------------------*/
void AmayaTransformEvtHandler::UpdatePositions()
{
  float x, y, w, h;
  /*
    Rectangle arround the object:
	
    (x,y)-----width--------| 
    |                      |
    |                      |
    |       (cx,cy)        height
    |                      |
    |                      |
    |----------------------|

  */
  GetPositionAndSizeInParentSpace(document, el, &x, &y, &w, &h);
  cx = x + w/2;
  cy = y + h/2;
  switch(type)
    {
    case 2:
      /* It's a rotation: compute the position of the center */
      SVGToMouseCoordinates(document, pFrame, x0, y0, width, height,
                            CTM, cx, cy, &cx2, &cy2);

#ifndef _WINDOWS
      pFrame->GetCanvas()->Refresh();
#endif /* WINDOWS */
      DrawRotationCenter();
      break;

    case 1:
    case 4:
      /* It's a scaling or a skewing:
         get the position of the arrows */
      left = x;
      top = y;
      SVGToMouseCoordinates(document, pFrame, x0, y0, width, height,
                            CTM, left, top, &left2, &top2);
      
      right = x + w;
      bottom = y + h;
      SVGToMouseCoordinates(document, pFrame, x0, y0, width, height,
                            CTM, right, bottom, &right2, &bottom2);

#ifndef _WINDOWS
      pFrame->GetCanvas()->Refresh();
#endif /* WINDOWS */

      /* Draw the arrows */
      if (type == 4)
        DrawSkewingArrows();
      else
        DrawScalingArrows();
      break;
      
    default:
      break;
    }
}
#endif /* _WX */
