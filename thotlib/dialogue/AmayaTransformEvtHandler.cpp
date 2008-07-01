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
#include "AmayaTransformEvtHandler.h"

static int lastX, lastY, m_mouse_x,m_mouse_y;
extern void GetPositionAndSizeInParentSpace(Document doc, Element el, float *X,
				       float *Y, float *width, float *height);
static ThotBool move = TRUE;


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
						   void *transform,
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
  ,m_transform(transform)
  ,m_x0(ancestorX)
  ,m_y0(ancestorY)
  ,m_width(canvasWidth)
  ,m_height(canvasHeight)
  ,m_type(transform_type)
  ,m_el(NULL)
   
{
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

  move = FALSE;
}

/*----------------------------------------------------------------------
 *----------------------------------------------------------------------*/
AmayaTransformEvtHandler::~AmayaTransformEvtHandler()
{

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
  move = TRUE;
  lastX = m_mouse_x;
  lastY = m_mouse_y;

  if (IsFinish())return;
}

/*----------------------------------------------------------------------
 *       Class:  AmayaTransformEvtHandler
 *      Method:  OnMouseUp
 * Description:  handle mouse button up events
 -----------------------------------------------------------------------*/
void AmayaTransformEvtHandler::OnMouseUp( wxMouseEvent& event )
{
  m_IsFinish = true;
  if (IsFinish())return;
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
  float x, y, width, height, cx, cy;
  int x1,y1,x2,y2;
  float theta, dx1, dx2, dy1, dy2, d;
  float det;

#define DELTA 50

  m_mouse_x = event.GetX();
  m_mouse_y = event.GetY();



  if(move && abs(m_mouse_x -lastX) + abs(m_mouse_y - lastY) > DELTA)
    {
      x1 = lastX;
      y1 = lastY;
      x2 = m_mouse_x;
      y2 = m_mouse_y;

      /* Get the coordinates in the SVG canvas */

      MouseCoordinatesToSVG(m_document, m_pFrame,
			m_x0, m_y0,
			m_width, m_height,
			m_transform,
			TRUE, &x1, &y1);


      MouseCoordinatesToSVG(m_document, m_pFrame,
			m_x0, m_y0,
			m_width, m_height,
			m_transform,
			TRUE, &x2, &y2);

      switch(m_type)
	{
	case 0:
	  TtaApplyMatrixTransform (m_document, m_el, 1, 0, 0, 1,
				   x2 - x1,
				   y2 - y1
				   );
	  break;

	case 1:
	  break;

	case 2:
	  /*
	    Rectangle arround the object:

	            (x,y)-----width---------| 
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

	  /*
                (dx1)               (x2,y2)
             v1=(dy1)                /
                               -->  /___
                               v2  /    _
                                  /     theta
                (dx2)            /        |
             v2=(dy2)           /         | 
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
			       cos(theta), sin(theta), -sin(theta), cos(theta),
			       +cx,+cy);
	    }

	  break;


	default:
	  break;
	}


      DefBoxRegion (m_FrameId, m_box, -1, -1, -1, -1);
      RedrawFrameBottom (m_FrameId, 0, NULL);
      m_pFrame->GetCanvas()->Refresh();

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

  
  float scale;

    {
      scale = (event.GetWheelRotation() > 0 ? SCALE_ : 1/SCALE_);

      TtaApplyMatrixTransform (m_document, m_el, scale, 0, 0, scale,
			       0,
			       0
			       );

      DefBoxRegion (m_FrameId, m_box, -1, -1, -1, -1);
      RedrawFrameBottom (m_FrameId, 0, NULL);
      m_pFrame->GetCanvas()->Refresh();
    }
}


#endif /* _WX */
