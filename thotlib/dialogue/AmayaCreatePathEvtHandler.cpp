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
#include "AmayaCreatePathEvtHandler.h"

static int m_mouse_x,m_mouse_y; 
static int points;
static int lastX, lastY;

void DrawPathFragment (int x1, int y1, int x2, int y2, int shape/*, int p*/)
{ 
  glEnable(GL_COLOR_LOGIC_OP);
  glLogicOp(GL_XOR);
  glColor4ub (127, 127, 127, 0);

  switch(shape)
    {
    default:
      glBegin(GL_LINE);
      glVertex2i(x1, y1);
      glVertex2i(x2, y2);
      glEnd (); 
      break;
    }

  glLogicOp(GL_COPY);
  glDisable(GL_COLOR_LOGIC_OP);
}


IMPLEMENT_DYNAMIC_CLASS(AmayaCreatePathEvtHandler, wxEvtHandler)

/*----------------------------------------------------------------------
 *  this is where the event table is declared
 *  the callbacks are assigned to an event type
 *----------------------------------------------------------------------*/
BEGIN_EVENT_TABLE(AmayaCreatePathEvtHandler, wxEvtHandler)
EVT_CHAR( AmayaCreatePathEvtHandler::OnChar )

EVT_LEFT_DOWN(	AmayaCreatePathEvtHandler::OnMouseDown) // Process a wxEVT_LEFT_DOWN event. 
EVT_LEFT_UP(		AmayaCreatePathEvtHandler::OnMouseUp) // Process a wxEVT_LEFT_UP event. 
EVT_LEFT_DCLICK(	AmayaCreatePathEvtHandler::OnMouseDbClick) // Process a wxEVT_LEFT_DCLICK event. 
EVT_MIDDLE_DOWN(	AmayaCreatePathEvtHandler::OnMouseDown) // Process a wxEVT_MIDDLE_DOWN event. 
EVT_MIDDLE_UP(	AmayaCreatePathEvtHandler::OnMouseUp) // Process a wxEVT_MIDDLE_UP event. 
EVT_MIDDLE_DCLICK(	AmayaCreatePathEvtHandler::OnMouseDbClick) // Process a wxEVT_MIDDLE_DCLICK event. 
EVT_RIGHT_DOWN(	AmayaCreatePathEvtHandler::OnMouseDown) // Process a wxEVT_RIGHT_DOWN event. 
EVT_RIGHT_UP(		AmayaCreatePathEvtHandler::OnMouseUp) // Process a wxEVT_RIGHT_UP event. 
EVT_RIGHT_DCLICK(	AmayaCreatePathEvtHandler::OnMouseDbClick) // Process a wxEVT_RIGHT_DCLICK event. 
EVT_MOTION(		AmayaCreatePathEvtHandler::OnMouseMove) // Process a wxEVT_MOTION event. 
EVT_MOUSEWHEEL(	AmayaCreatePathEvtHandler::OnMouseWheel) // Process a wxEVT_MOUSEWHEEL event. 
END_EVENT_TABLE()

/*----------------------------------------------------------------------
 *----------------------------------------------------------------------*/
AmayaCreatePathEvtHandler::AmayaCreatePathEvtHandler() : wxEvtHandler()
{
}

/*----------------------------------------------------------------------
 *----------------------------------------------------------------------*/
AmayaCreatePathEvtHandler::AmayaCreatePathEvtHandler(AmayaFrame * p_frame, int x_min, int y_min, int x_max, int y_max,  PtrTextBuffer Pbuffer, Document doc, int shape_number)
  : wxEvtHandler()
  ,m_IsFinish(false)
  ,m_pFrame(p_frame)
  ,m_FrameId(p_frame->GetFrameId())
  ,m_xmin(x_min)
  ,m_ymin(y_min)
  ,m_xmax(x_max)
  ,m_ymax(y_max)
  ,m_ShapeNumber(shape_number)
  ,m_Pbuffer(Pbuffer)
  ,m_document(doc)
    
{
  points = 0;

  if (m_pFrame)
    {
      /* attach this handler to the canvas */
      AmayaCanvas * p_canvas = m_pFrame->GetCanvas();
      p_canvas->PushEventHandler(this);

      /* assign a cross mouse cursor */
      m_pFrame->GetCanvas()->SetCursor( wxCursor(wxCURSOR_CROSS) );
      m_pFrame->GetCanvas()->CaptureMouse();
    }
}

/*----------------------------------------------------------------------
 *----------------------------------------------------------------------*/
AmayaCreatePathEvtHandler::~AmayaCreatePathEvtHandler()
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
bool AmayaCreatePathEvtHandler::IsFinish()
{
  return m_IsFinish;
}

/*----------------------------------------------------------------------
 *----------------------------------------------------------------------*/
void AmayaCreatePathEvtHandler::OnChar( wxKeyEvent& event )
{
  m_IsFinish = true;
}

/*----------------------------------------------------------------------
 *       Class:  AmayaCreatePathEvtHandler
 *      Method:  OnMouseDown
 * Description:  handle mouse button down events
 -----------------------------------------------------------------------*/
void AmayaCreatePathEvtHandler::OnMouseDown( wxMouseEvent& event )
{
}

/*----------------------------------------------------------------------
 *       Class:  AmayaCreatePathEvtHandler
 *      Method:  OnMouseUp
 * Description:  handle mouse button up events
 -----------------------------------------------------------------------*/
void AmayaCreatePathEvtHandler::OnMouseUp( wxMouseEvent& event )
{
  if (IsFinish())
    return;


  lastX = m_mouse_x;
  lastY = m_mouse_y;
  points++;
  
  AddPointInPolyline (m_Pbuffer, points, m_mouse_x, m_mouse_y);


}

/*----------------------------------------------------------------------
 *       Class:  AmayaCreatePathEvtHandler
 *      Method:  OnMouseDbClick
 * Description:  handle mouse dbclick events
 -----------------------------------------------------------------------*/
void AmayaCreatePathEvtHandler::OnMouseDbClick( wxMouseEvent& event )
{
  m_IsFinish = true;
}

/*----------------------------------------------------------------------
 *       Class:  AmayaCreatePathEvtHandler
 *      Method:  OnMouseMove
 * Description:  handle mouse move events
 -----------------------------------------------------------------------*/
void AmayaCreatePathEvtHandler::OnMouseMove( wxMouseEvent& event )
{
  if(points > 0)
    DrawPathFragment (lastX, lastY, m_mouse_x, m_mouse_y, m_ShapeNumber
		      /*, int p*/);

  m_mouse_x = event.GetX();
  m_mouse_y = event.GetY();

  if(points > 0)
    DrawPathFragment (lastX, lastY, m_mouse_x, m_mouse_y, m_ShapeNumber
		    /*, int p*/);

  MouseCoordinatesToSVG(m_document, m_pFrame, m_xmin, m_xmax, m_ymin, m_ymax,
			FALSE, &m_mouse_x, &m_mouse_y);

  m_pFrame->GetCanvas()->Refresh();
}

/*----------------------------------------------------------------------
 *       Class:  AmayaCreatePathEvtHandler
 *      Method:  OnMouseWheel
 * Description:  handle mouse wheel events
 -----------------------------------------------------------------------*/
void AmayaCreatePathEvtHandler::OnMouseWheel( wxMouseEvent& event )
{
}

#endif /* _WX */
