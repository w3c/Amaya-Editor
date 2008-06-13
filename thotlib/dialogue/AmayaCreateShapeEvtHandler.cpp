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
EVT_CHAR( AmayaCreateShapeEvtHandler::OnChar )

EVT_LEFT_DOWN(	AmayaCreateShapeEvtHandler::OnMouseDown) // Process a wxEVT_LEFT_DOWN event. 
EVT_LEFT_UP(		AmayaCreateShapeEvtHandler::OnMouseUp) // Process a wxEVT_LEFT_UP event. 
EVT_LEFT_DCLICK(	AmayaCreateShapeEvtHandler::OnMouseDbClick) // Process a wxEVT_LEFT_DCLICK event. 
EVT_MIDDLE_DOWN(	AmayaCreateShapeEvtHandler::OnMouseDown) // Process a wxEVT_MIDDLE_DOWN event. 
EVT_MIDDLE_UP(	AmayaCreateShapeEvtHandler::OnMouseUp) // Process a wxEVT_MIDDLE_UP event. 
EVT_MIDDLE_DCLICK(	AmayaCreateShapeEvtHandler::OnMouseDbClick) // Process a wxEVT_MIDDLE_DCLICK event. 
EVT_RIGHT_DOWN(	AmayaCreateShapeEvtHandler::OnMouseDown) // Process a wxEVT_RIGHT_DOWN event. 
EVT_RIGHT_UP(		AmayaCreateShapeEvtHandler::OnMouseUp) // Process a wxEVT_RIGHT_UP event. 
EVT_RIGHT_DCLICK(	AmayaCreateShapeEvtHandler::OnMouseDbClick) // Process a wxEVT_RIGHT_DCLICK event. 
EVT_MOTION(		AmayaCreateShapeEvtHandler::OnMouseMove) // Process a wxEVT_MOTION event. 
EVT_MOUSEWHEEL(	AmayaCreateShapeEvtHandler::OnMouseWheel) // Process a wxEVT_MOUSEWHEEL event. 
END_EVENT_TABLE()

/*----------------------------------------------------------------------
 *----------------------------------------------------------------------*/
AmayaCreateShapeEvtHandler::AmayaCreateShapeEvtHandler() : wxEvtHandler()
{
}

/*----------------------------------------------------------------------
 *----------------------------------------------------------------------*/
AmayaCreateShapeEvtHandler::AmayaCreateShapeEvtHandler(AmayaFrame * p_frame, int *x1, int *y1, int *x2, int *y2, int *nb_points, Document doc, int shape_number)
  : wxEvtHandler()
  ,m_IsFinish(false)
  ,m_pFrame(p_frame)
  ,m_FrameId(p_frame->GetFrameId())
  ,m_xmin(*x1)
  ,m_ymin(*y1)
  ,m_xmax(*x2)
  ,m_ymax(*y2)
  ,m_x1(x1)
  ,m_y1(y1)
  ,m_x2(x2)
  ,m_y2(y2)
  ,m_LastX(*x1)
  ,m_LastY(*y1)
  ,m_ShapeNumber(shape_number)
  ,m_NbPoints(nb_points)
  ,m_document(doc)
    
{

  *m_NbPoints = 0;
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
AmayaCreateShapeEvtHandler::~AmayaCreateShapeEvtHandler()
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
bool AmayaCreateShapeEvtHandler::IsFinish()
{
  
  return m_IsFinish;
}

/*----------------------------------------------------------------------
 *----------------------------------------------------------------------*/
void AmayaCreateShapeEvtHandler::OnChar( wxKeyEvent& event )
{
  m_IsFinish = true;
}

/*----------------------------------------------------------------------
 *       Class:  AmayaCreateShapeEvtHandler
 *      Method:  OnMouseDown
 * Description:  handle mouse button down events
 -----------------------------------------------------------------------*/
void AmayaCreateShapeEvtHandler::OnMouseDown( wxMouseEvent& event )
{
}

/*----------------------------------------------------------------------
 *       Class:  AmayaCreateShapeEvtHandler
 *      Method:  OnMouseUp
 * Description:  handle mouse button up events
 -----------------------------------------------------------------------*/
void AmayaCreateShapeEvtHandler::OnMouseUp( wxMouseEvent& event )
{
  if (IsFinish())
    return;

  (*m_NbPoints)++;

 
  /* update the box buffer */
  switch(*m_NbPoints)
    {
    case 1:
      *m_x1 = m_LastX;
      *m_y1 = m_LastY;
      break;

    case 2:
      *m_x2 = m_LastX;
      *m_y2 = m_LastY;
      m_IsFinish = true;
      break;

    }
}

/*----------------------------------------------------------------------
 *       Class:  AmayaCreateShapeEvtHandler
 *      Method:  OnMouseDbClick
 * Description:  handle mouse dbclick events
 -----------------------------------------------------------------------*/
void AmayaCreateShapeEvtHandler::OnMouseDbClick( wxMouseEvent& event )
{
  m_IsFinish = true;
}

/*----------------------------------------------------------------------
 *       Class:  AmayaCreateShapeEvtHandler
 *      Method:  OnMouseMove
 * Description:  handle mouse move events
 -----------------------------------------------------------------------*/
void AmayaCreateShapeEvtHandler::OnMouseMove( wxMouseEvent& event )
{
  int x_mouse,y_mouse,newx,newy;

  char buffer[100];

  /* check the coordinates */
  x_mouse = m_xmin + TtaGridDoAlign( (int)(event.GetX() - m_xmin) );
  y_mouse = m_ymin + TtaGridDoAlign( (int)(event.GetY() - m_ymin) );

  if(*m_NbPoints == 1)
    {
      switch(m_ShapeNumber)
	{
	default:
	  break;
	 
	}
    }

  newx = LogicalValue (x_mouse - m_xmin, UnPixel, NULL,
  ViewFrameTable[m_FrameId - 1].FrMagnification);

  if(newx < 0)newx = 0;
  if(newx > (m_xmax - m_xmin))newx = (m_xmax - m_xmin);

  newy = LogicalValue (y_mouse - m_ymin, UnPixel, NULL,
  ViewFrameTable[m_FrameId - 1].FrMagnification);  

  if(newy < 0)newy = 0;
  if(newy > (m_ymax - m_ymin))newy = (m_ymax - m_ymin);

  sprintf(buffer, "(%d,%d)", newx, newy);
  TtaSetStatus (m_document, 1, buffer, NULL);

  m_LastX = newx;
  m_LastY = newy;

  m_pFrame->GetCanvas()->Refresh();
}

/*----------------------------------------------------------------------
 *       Class:  AmayaCreateShapeEvtHandler
 *      Method:  OnMouseWheel
 * Description:  handle mouse wheel events
 -----------------------------------------------------------------------*/
void AmayaCreateShapeEvtHandler::OnMouseWheel( wxMouseEvent& event )
{
}

#endif /* _WX */
