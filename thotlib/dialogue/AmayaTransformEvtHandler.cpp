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


#include "AmayaFrame.h"
#include "AmayaCanvas.h"
#include "AmayaTransformEvtHandler.h"

/* Coordinates of the mouse */
static int lastX, lastY, m_mouse_x,m_mouse_y;

/* Coordinates of the center of rotation */
static int cx2, cy2;
static float cx, cy;

/* Coordinates of the box */
static int top2, left2, bottom2, right2;
static float top, left, bottom, right;

extern void GetPositionAndSizeInParentSpace(Document doc, Element el, float *X,
					    float *Y, float *width, float *height);
static ThotBool ButtonDown = TRUE;


/*----------------------------------------------------------------------
  DrawLine
  *----------------------------------------------------------------------*/
static void DrawRotationCenter(int frame)
{
#define CURSOR_SIZE 15

  glEnable(GL_COLOR_LOGIC_OP);
  glLogicOp(GL_XOR);

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
  GL_Swap (frame);
#endif /* WINDOWS */
}

/*----------------------------------------------------------------------
  DrawSkewArrows
 *----------------------------------------------------------------------*/
static void DrawSkewArrows(int frame)
{
  glEnable(GL_COLOR_LOGIC_OP);
  glLogicOp(GL_XOR);
  glColor4ub (127, 127, 127, 0);

  /*
             ----3-----
             |        |
             1        2
             |        |
             ----4-----

  */
  DrawArrow (frame, 1, 5,
	     left2 - CURSOR_SIZE/2,
	     (top2+bottom2)/2 - CURSOR_SIZE,
	     CURSOR_SIZE,
	     2*CURSOR_SIZE,
	     90, 3, 0);

  DrawArrow (frame, 1, 5,
	     right2 - CURSOR_SIZE/2,
	     (top2+bottom2)/2 - CURSOR_SIZE,
	     CURSOR_SIZE,
	     2*CURSOR_SIZE,
	     90, 3, 0);

  DrawArrow (frame, 1, 5,
	     (left2+right2)/2 - CURSOR_SIZE,
	     top2 - CURSOR_SIZE/2,
	     2*CURSOR_SIZE,
	     CURSOR_SIZE,
	     0, 3, 0);

  DrawArrow (frame, 1, 5,
	     (left2+right2)/2 - CURSOR_SIZE,
	     bottom2 - CURSOR_SIZE/2,
	     2*CURSOR_SIZE,
	     CURSOR_SIZE,
	     0, 3, 0);

  glDisable(GL_COLOR_LOGIC_OP);

#ifdef _WINDOWS
  GL_Swap (frame);
#endif /* WINDOWS */
}

/*----------------------------------------------------------------------
  IsNear
 *----------------------------------------------------------------------*/
static ThotBool IsNear(int x, int y)
{
  return (abs(m_mouse_x - x) + abs(m_mouse_y - y) <= CURSOR_SIZE);
}

IMPLEMENT_DYNAMIC_CLASS(AmayaTransformEvtHandler, wxEvtHandler)

/*----------------------------------------------------------------------
 *  this is where the event table is declared
 *  the callbacks are assigned to an event type
 *----------------------------------------------------------------------*/
BEGIN_EVENT_TABLE(AmayaTransformEvtHandler, wxEvtHandler)
EVT_CHAR( AmayaTransformEvtHandler::OnChar )

EVT_LEFT_DOWN(	AmayaTransformEvtHandler::OnMouseDown) // Process a wxEVT_LEFT_DOWN event. 
EVT_LEFT_UP(		AmayaTransformEvtHandler::OnMouseUp) // Process a wxEVT_LEFT_UP event. 
EVT_LEFT_DCLICK(	AmayaTransformEvtHandler::OnMouseDbClick) // Process a wxEVT_LEFT_DCLICK event. 
EVT_MIDDLE_DOWN(	AmayaTransformEvtHandler::OnMouseDown) // Process a wxEVT_MIDDLE_DOWN event. 
EVT_MIDDLE_UP(	AmayaTransformEvtHandler::OnMouseUp) // Process a wxEVT_MIDDLE_UP event. 
EVT_MIDDLE_DCLICK(	AmayaTransformEvtHandler::OnMouseDbClick) // Process a wxEVT_MIDDLE_DCLICK event. 
EVT_RIGHT_DOWN(	AmayaTransformEvtHandler::OnMouseDown) // Process a wxEVT_RIGHT_DOWN event. 
EVT_RIGHT_UP(		AmayaTransformEvtHandler::OnMouseUp) // Process a wxEVT_RIGHT_UP event. 
EVT_RIGHT_DCLICK(	AmayaTransformEvtHandler::OnMouseDbClick) // Process a wxEVT_RIGHT_DCLICK event. 
EVT_MOTION(		AmayaTransformEvtHandler::OnMouseMove) // Process a wxEVT_MOTION event. 
EVT_MOUSEWHEEL(	AmayaTransformEvtHandler::OnMouseWheel) // Process a wxEVT_MOUSEWHEEL event. 
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
						   PtrBox box)
  :wxEvtHandler()
  ,m_IsFinish(false)
  ,m_pFrame(p_frame)
  ,m_FrameId(p_frame->GetFrameId())
  ,m_box(box)
  ,m_document(doc)
  ,m_CTM(CTM)
  ,m_inverse(inverse)
  ,m_x0(ancestorX)
  ,m_y0(ancestorY)
  ,m_width(canvasWidth)
  ,m_height(canvasHeight)
  ,m_type(transform_type)
  ,m_el(NULL)
   
{
  float x, y, width, height;

  if(m_box && m_box -> BxAbstractBox)
    m_el = (Element)(m_box -> BxAbstractBox-> AbElement);

  if (m_pFrame)
    {
      /* attach this handler to the canvas */
      AmayaCanvas * p_canvas = m_pFrame->GetCanvas();
      p_canvas->PushEventHandler(this);

      /* assign a cross mouse cursor */
      m_pFrame->GetCanvas()->SetCursor( wxCursor(wxCURSOR_CROSS) );
     
      m_pFrame->GetCanvas()->CaptureMouse();

    }


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
  GetPositionAndSizeInParentSpace(m_document, m_el,
				  &x, &y, &width, &height);
     
  cx = x + width/2;
  cy = y + height/2;

  switch(m_type)
    {
    case 2:
      /* It's a rotation: compute the position of the center */
      SVGToMouseCoordinates(m_document, m_pFrame,
			    m_x0, m_y0,
			    m_width, m_height,
			    m_CTM,
			    cx,
			    cy,
			    &cx2, &cy2);

#ifndef _WINDOWS
    m_pFrame->GetCanvas()->Refresh();
#endif /* WINDOWS */
      DrawRotationCenter(m_FrameId);
      break;


    case 4:
      /* It's a skewing: get the position of the arrows and draw them */

      left = x;
      top = y;

      SVGToMouseCoordinates(m_document, m_pFrame,
			    m_x0, m_y0,
			    m_width, m_height,
			    m_CTM,
			    left,
			    top,
			    &left2, &top2);
      
      right = x + width;
      bottom = y + height;

      SVGToMouseCoordinates(m_document, m_pFrame,
			    m_x0, m_y0,
			    m_width, m_height,
			    m_CTM,
			    right,
			    bottom,
			    &right2, &bottom2);

#ifndef _WINDOWS
    m_pFrame->GetCanvas()->Refresh();
#endif /* WINDOWS */
      DrawSkewArrows(m_FrameId);
      break;
      
    default:
      break;
    }

  ButtonDown = FALSE;
}

/*----------------------------------------------------------------------
 *----------------------------------------------------------------------*/
AmayaTransformEvtHandler::~AmayaTransformEvtHandler()
{
  /* Clear the center */
  if(m_type == 2 || m_type == 3)
    DrawRotationCenter(m_FrameId);

  if (m_pFrame)
    {
      /* detach this handler from the canvas (restore default behaviour) */
      AmayaCanvas * p_canvas = m_pFrame->GetCanvas();
      p_canvas->PopEventHandler(false /* do not delete myself */);
      
      /* restore the default cursor */
      m_pFrame->GetCanvas()->SetCursor( wxNullCursor );
      m_pFrame->GetCanvas()->ReleaseMouse();
    }

}

/*----------------------------------------------------------------------
 *----------------------------------------------------------------------*/
bool AmayaTransformEvtHandler::IsFinish()
{
  return m_IsFinish;
}

/*----------------------------------------------------------------------
 *----------------------------------------------------------------------*/
void AmayaTransformEvtHandler::OnChar( wxKeyEvent& event )
{
  m_IsFinish = true;
}

/*----------------------------------------------------------------------
 *       Class:  AmayaTransformEvtHandler
 *      Method:  OnMouseDown
 * Description:  handle mouse button down events
 -----------------------------------------------------------------------*/
void AmayaTransformEvtHandler::OnMouseDown( wxMouseEvent& event )
{
  if (IsFinish())return;

  ButtonDown = TRUE;
  lastX = m_mouse_x;
  lastY = m_mouse_y;

  switch(m_type)
    {
    case 2:
      /* Rotation: is the user clicking on the center ? */
      if(IsNear(cx2, cy2))
	 m_type = 3;
      break;

    case 4:
      /* Skewing: is the user clicking on an arrow ? */

      if(IsNear((left2+right2)/2, top2))
	m_type = 5;
      else if(IsNear((left2+right2)/2, bottom2))
	m_type = 6;
      else if(IsNear(left2, (top2+bottom2)/2))
	m_type = 7;
      else if(IsNear(right2, (top2+bottom2)/2))
	m_type = 8;
      else return;

      /* Clear the arrows */
      DrawSkewArrows(m_FrameId);
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
  int x,y;

  if (IsFinish())return;

  if(m_type == 3)
    {
      /* The user was moving the center */
      m_type = 2;

      /* Get the new coordinates */
      x = cx2;
      y = cy2;

      MouseCoordinatesToSVG(m_document, m_pFrame,
			    m_x0, m_y0,
			    m_width, m_height,
			    m_inverse,
			    TRUE, &x, &y);

      cx = x;
      cy = y;

      ButtonDown = FALSE;
      return;

    }

  m_IsFinish = true;

}

/*----------------------------------------------------------------------
 *       Class:  AmayaTransformEvtHandler
 *      Method:  OnMouseDbClick
 * Description:  handle mouse dbclick events
 -----------------------------------------------------------------------*/
void AmayaTransformEvtHandler::OnMouseDbClick( wxMouseEvent& event )
{
  m_IsFinish = true;
}

/*----------------------------------------------------------------------
 *       Class:  AmayaTransformEvtHandler
 *      Method:  OnMouseMove
 * Description:  handle mouse move events
 -----------------------------------------------------------------------*/
void AmayaTransformEvtHandler::OnMouseMove( wxMouseEvent& event )
{
  int x1,y1,x2,y2;
  float theta, dx1, dx2, dy1, dy2, d;
  float det;
  float skew;

#define DELTA 10

  m_mouse_x = event.GetX();
  m_mouse_y = event.GetY();

  if(!ButtonDown)
    {
      lastX = m_mouse_x;
      lastY = m_mouse_y;

      if(m_type == 0 || m_type == 1)
	  /* For translate and scale, the module is called just after the user
	     click on a shape, so the button is actually already down */
	  ButtonDown = TRUE;

    }
  else if(abs(m_mouse_x -lastX) + abs(m_mouse_y - lastY) > DELTA)
    {
      x1 = lastX;
      y1 = lastY;
      x2 = m_mouse_x;
      y2 = m_mouse_y;

      /* Get the coordinates in the SVG canvas */

      MouseCoordinatesToSVG(m_document, m_pFrame,
			    m_x0, m_y0,
			    m_width, m_height,
			    m_inverse,
			    TRUE, &x1, &y1);

      MouseCoordinatesToSVG(m_document, m_pFrame,
			    m_x0, m_y0,
			    m_width, m_height,
			    m_inverse,
			    TRUE, &x2, &y2);

      /* Clear the center */
      if(m_type == 2 || m_type == 3)
	DrawRotationCenter(m_FrameId);

      switch(m_type)
	{
	case 0:
	  /* Translate */
	  TtaApplyMatrixTransform (m_document, m_el, 1, 0, 0, 1,
				   x2 - x1,
				   y2 - y1
				   );
	  break;

	case 1:
	  /* Scale */
	  break;

	case 2:
	  /* Rotate */

	  /*
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
	  
	  if(d > 0)
	    {
	      /*
		{v1.v2 = d*cos(theta)
		{
		{             |dx1 dx2|
		{det(v1,v2) = |dy1 dy2| = d*sin(theta)

		=> theta = sign(det(v1,v2))*acos((v1.v2)/d)
                        
	      */
	      det = dx1*dy2 - dy1*dx2;
	      theta = acos((dx1*dx2 + dy1*dy2)/d);
	      if(det < 0)theta = -theta;

	      /* rotate(theta,cx,cy) */
	      TtaApplyMatrixTransform (m_document, m_el, 1, 0, 0, 1, -cx, -cy);
	      TtaApplyMatrixTransform (m_document, m_el,
				       cos(theta), sin(theta),
				       -sin(theta), cos(theta),
				       0,0);
	      TtaApplyMatrixTransform (m_document, m_el, 1, 0, 0, 1, +cx, +cy);
	    }

	  break;

	case 3:
	  /* Moving center of rotation  */
	  if(MouseCoordinatesToSVG(m_document, m_pFrame,
				   m_x0, m_y0,
				   m_width, m_height,
				   m_inverse,
				   FALSE, &m_mouse_x, &m_mouse_y))
	    {
	      cx2 = m_mouse_x;
	      cy2 = m_mouse_y;
	    }
	  break;

	case 5:
	case 6:
	  /* Skewing along X axis */

	  /* Take the center of the shape as the origin */
	  TtaApplyMatrixTransform (m_document, m_el, 1, 0, 0, 1, -cx, -cy);
	  x1-=(int)cx;
	  x2-=(int)cx;

	  if(m_type == 5)
	    y1 = y2 = (int)(top - cy);
	  else
	    y1 = y2 = (int)(bottom - cy);

	  /* 
	     (x2)   (1   skew) (x1)   y1=y2
	     (y2) = (       1) (y1)
	  */ 
	  skew = ((float)(x2 - x1))/y1;
	  TtaApplyMatrixTransform (m_document, m_el,
				   1, 0,
				   skew,
				   1,
				   0,0);

	  /* Move the shape to its initial position */
	  TtaApplyMatrixTransform (m_document, m_el, 1, 0, 0, 1, +cx, +cy);
	  break;

	case 7:
	case 8:
	  /* Skewing along Y axis */

	  /* Take the center of the shape as the origin */
	  TtaApplyMatrixTransform (m_document, m_el, 1, 0, 0, 1, -cx, -cy);
	  y1-=(int)cy;
	  y2-=(int)cy;

	  if(m_type == 7)
	    x1 = x2 = (int)(left - cx);
	  else
	    x1 = x2 = (int)(right - cx);

	  /*
	  (x2)   (1    0) (x1)     x1=x2
	  (y2) = (skew 1) (y1)
	  */ 
	  skew = ((float)(y2 - y1))/x1;
	  TtaApplyMatrixTransform (m_document, m_el,
				   1,
				   skew,
				   0, 1,
				   0,0);

	  /* Move the shape to its initial position */
	  TtaApplyMatrixTransform (m_document, m_el, 1, 0, 0, 1, +cx, +cy);
	  break;

	default:
	  break;
	}

//#ifndef _WINDOWS
	//DefBoxRegion (m_FrameId, m_box, -1, -1, -1, -1);
    //RedrawFrameBottom (m_FrameId, 0, NULL);
    m_pFrame->GetCanvas()->Refresh();
//#endif /* WINDOWS */

      /* Redraw the center of rotation */
      if(m_type == 2 || m_type == 3)
		DrawRotationCenter(m_FrameId);

      lastX = m_mouse_x;
      lastY = m_mouse_y;

    }

}

/*----------------------------------------------------------------------
 *       Class:  AmayaTransformEvtHandler
 *      Method:  OnMouseWheel
 * Description:  handle mouse wheel events
 -----------------------------------------------------------------------*/
void AmayaTransformEvtHandler::OnMouseWheel( wxMouseEvent& event )
{
#define SCALE_ 1.1

  return;
  
  float scale;

  {
    scale = (event.GetWheelRotation() > 0 ? SCALE_ : 1/SCALE_);

    TtaApplyMatrixTransform (m_document, m_el, scale, 0, 0, scale,
			     0,
			     0
			     );
  }
}


#endif /* _WX */
