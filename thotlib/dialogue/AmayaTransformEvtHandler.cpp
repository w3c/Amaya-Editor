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

static int m_mouse_x,m_mouse_y; 

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
						   Element el,
						   int transform_type,
						   int x1, int y1,
						   int x2, int y2,
						   int x3, int y3,
						   int x4, int y4)
  :wxEvtHandler()
  ,m_IsFinish(false)
  ,m_pFrame(p_frame)
  ,m_FrameId(p_frame->GetFrameId())
  ,m_document(doc)
  ,m_transform(transform)
  ,m_el(el)
  ,m_x0(ancestorX)
  ,m_y0(ancestorY)
  ,m_width(canvasWidth)
  ,m_height(canvasHeight)
  ,m_type(transform_type)
  ,m_x1(x1)
  ,m_y1(y1)
  ,m_x2(x2)
  ,m_y2(y2)
  ,m_x3(x3)
  ,m_y3(y3)
  ,m_x4(x4)
  ,m_y4(y4)
   
{
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
  if (IsFinish())return;
}

/*----------------------------------------------------------------------
 *       Class:  AmayaTransformEvtHandler
 *      Method:  OnMouseUp
 * Description:  handle mouse button up events
 -----------------------------------------------------------------------*/
void AmayaTransformEvtHandler::OnMouseUp( wxMouseEvent& event )
{
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
    m_pFrame->GetCanvas()->Refresh();
}

/*----------------------------------------------------------------------
 *       Class:  AmayaTransformEvtHandler
 *      Method:  OnMouseWheel
 * Description:  handle mouse wheel events
 -----------------------------------------------------------------------*/
void AmayaTransformEvtHandler::OnMouseWheel( wxMouseEvent& event )
{
}


#endif /* _WX */
