/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2008
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
#include "AmayaAddPointEvtHandler.h"

IMPLEMENT_DYNAMIC_CLASS(AmayaAddPointEvtHandler, wxEvtHandler)

/*----------------------------------------------------------------------
 *  this is where the event table is declared
 *  the callbacks are assigned to an event type
 *----------------------------------------------------------------------*/
BEGIN_EVENT_TABLE(AmayaAddPointEvtHandler, wxEvtHandler)
  EVT_KEY_DOWN( AmayaAddPointEvtHandler::OnChar )

  EVT_LEFT_DOWN(	AmayaAddPointEvtHandler::OnMouseDown) // Process a wxEVT_LEFT_DOWN event. 
  EVT_LEFT_UP(		AmayaAddPointEvtHandler::OnMouseUp) // Process a wxEVT_LEFT_UP event. 
  EVT_LEFT_DCLICK(	AmayaAddPointEvtHandler::OnMouseDbClick) // Process a wxEVT_LEFT_DCLICK event. 
  EVT_MIDDLE_DOWN(	AmayaAddPointEvtHandler::OnMouseDown) // Process a wxEVT_MIDDLE_DOWN event. 
  EVT_MIDDLE_UP(	AmayaAddPointEvtHandler::OnMouseUp) // Process a wxEVT_MIDDLE_UP event. 
  EVT_MIDDLE_DCLICK(	AmayaAddPointEvtHandler::OnMouseDbClick) // Process a wxEVT_MIDDLE_DCLICK event. 
  EVT_RIGHT_DOWN(	AmayaAddPointEvtHandler::OnMouseDown) // Process a wxEVT_RIGHT_DOWN event. 
  EVT_RIGHT_UP(		AmayaAddPointEvtHandler::OnMouseUp) // Process a wxEVT_RIGHT_UP event. 
  EVT_RIGHT_DCLICK(	AmayaAddPointEvtHandler::OnMouseDbClick) // Process a wxEVT_RIGHT_DCLICK event. 
  EVT_MOTION(		AmayaAddPointEvtHandler::OnMouseMove) // Process a wxEVT_MOTION event. 
  EVT_MOUSEWHEEL(	AmayaAddPointEvtHandler::OnMouseWheel) // Process a wxEVT_MOUSEWHEEL event. 
END_EVENT_TABLE()

/*----------------------------------------------------------------------
 *----------------------------------------------------------------------*/
AmayaAddPointEvtHandler::AmayaAddPointEvtHandler() : wxEvtHandler()
{
}

/*----------------------------------------------------------------------
 *----------------------------------------------------------------------*/
AmayaAddPointEvtHandler::AmayaAddPointEvtHandler( AmayaFrame *p_frame,
                                                  int x, int y, int x1, int y1, int x3,
                                                  int y3, int lastx, int lasty, int point,
                                                  int *nbpoints, int maxPoints, int width, int height,
                                                  PtrTextBuffer Pbuffer, PtrTextBuffer Bbuffer )
  : wxEvtHandler()
    ,m_IsFinish(false)
    ,m_pFrame(p_frame)
    ,m_FrameId(p_frame->GetFrameId())
    ,m_X(x)
    ,m_Y(y)
    ,m_X1(x1)
    ,m_Y1(y1)
    ,m_X3(x3)
    ,m_Y3(y3)
    ,m_LastX(lastx)
    ,m_LastY(lasty)
    ,m_Point(point)
    ,m_NbPoints(nbpoints)
    ,m_MaxPoints(maxPoints)
    ,m_Width(width)
    ,m_Height(height)
    ,m_Pbuffer(Pbuffer)
    ,m_Bbuffer(Bbuffer)
    
{
  TTALOGDEBUG_0( TTA_LOG_SVGEDIT, _T("AmayaAddPointEvtHandler::AmayaAddPointEvtHandler"));

  /* trasformation factor between the box and the abstract box */
  m_RatioX = (float) m_Pbuffer->BuPoints[0].XCoord / (float) m_Bbuffer->BuPoints[0].XCoord;
  /* trasformation factor between the box and the abstract box */
  m_RatioY = (float) m_Pbuffer->BuPoints[0].YCoord / (float) m_Bbuffer->BuPoints[0].YCoord;

  if (m_pFrame)
    {
      // attach this handler to the canvas
      AmayaCanvas * p_canvas = m_pFrame->GetCanvas();
      p_canvas->PushEventHandler(this);
      
      // assign a cross mouse cursor
      m_pFrame->GetCanvas()->SetCursor( wxCursor(wxCURSOR_CROSS) );
      
      m_pFrame->GetCanvas()->CaptureMouse();
   }
}

/*----------------------------------------------------------------------
 *----------------------------------------------------------------------*/
AmayaAddPointEvtHandler::~AmayaAddPointEvtHandler()
{
  TTALOGDEBUG_0( TTA_LOG_SVGEDIT, _T("AmayaAddPointEvtHandler::~AmayaAddPointEvtHandler") );

  if (m_pFrame)
    {
      // detach this handler from the canvas (restore default behaviour)
      AmayaCanvas * p_canvas = m_pFrame->GetCanvas();
      p_canvas->PopEventHandler(false /* do not delete myself */);
      
      // restore the default cursor
      m_pFrame->GetCanvas()->SetCursor( wxNullCursor );

      m_pFrame->GetCanvas()->ReleaseMouse();
    }
}

/*----------------------------------------------------------------------
 *----------------------------------------------------------------------*/
bool AmayaAddPointEvtHandler::IsFinish()
{
  return m_IsFinish;
}

/*----------------------------------------------------------------------
 *----------------------------------------------------------------------*/
void AmayaAddPointEvtHandler::OnChar( wxKeyEvent& event )
{
  m_IsFinish = true;
}

/*----------------------------------------------------------------------
 *       Class:  AmayaAddPointEvtHandler
 *      Method:  OnMouseDown
 * Description:  handle mouse button down events
  -----------------------------------------------------------------------*/
void AmayaAddPointEvtHandler::OnMouseDown( wxMouseEvent& event )
{
  //  TTALOGDEBUG_0( TTA_LOG_SVGEDIT, _T("AmayaAddPointEvtHandler::OnMouseDown") );
  //  event.Skip();
}

/*----------------------------------------------------------------------
 *       Class:  AmayaAddPointEvtHandler
 *      Method:  OnMouseUp
 * Description:  handle mouse button up events
  -----------------------------------------------------------------------*/
void AmayaAddPointEvtHandler::OnMouseUp( wxMouseEvent& event )
{
  if (IsFinish())
    return;

  int newx, newy;
  int newx1, newy1;

#ifdef _GL
  //  InitDrawing (5, 2, FgSelColor);
  //  GL_DrawLine(m_X1, m_Y1, m_LastX, m_LastY, FALSE);
#endif /* _GL */

  m_X1 = m_LastX;
  m_Y1 = m_LastY;
  (*m_NbPoints)++;
  m_Point++;

  /* update the box buffer */
  newx = LogicalValue (m_LastX - m_X, UnPixel, NULL,
		       ViewFrameTable[m_FrameId - 1].FrMagnification);
  newy = LogicalValue (m_LastY - m_Y, UnPixel, NULL,
		       ViewFrameTable[m_FrameId - 1].FrMagnification);

  AddPointInPolyline (m_Bbuffer, m_Point, newx, newy, FALSE, FALSE);

  /* update the abstract box buffer */
  newx1 = (int) ((float) newx * m_RatioX);
  newy1 = (int) ((float) newy * m_RatioY);
  AddPointInPolyline (m_Pbuffer, m_Point, newx1, newy1, FALSE, FALSE);

  TTALOGDEBUG_4( TTA_LOG_SVGEDIT, _T("AmayaAddPointEvtHandler::OnMouseUp newx=%d newy=%d newx1=%d newy1=%d"),
		 newx, newy, newx1, newy1 );

  if (*m_NbPoints > m_MaxPoints && m_MaxPoints != 0)
    /* we have the right number of points */
    m_IsFinish = true;
  else if (m_MaxPoints == 0 && event.GetButton() != wxMOUSE_BTN_LEFT)
    /* any other button : end of user input */
    m_IsFinish = true;
}

/*----------------------------------------------------------------------
 *       Class:  AmayaAddPointEvtHandler
 *      Method:  OnMouseDbClick
 * Description:  handle mouse dbclick events
  -----------------------------------------------------------------------*/
void AmayaAddPointEvtHandler::OnMouseDbClick( wxMouseEvent& event )
{
  m_IsFinish = true;
}

/*----------------------------------------------------------------------
 *       Class:  AmayaAddPointEvtHandler
 *      Method:  OnMouseMove
 * Description:  handle mouse move events
  -----------------------------------------------------------------------*/
void AmayaAddPointEvtHandler::OnMouseMove( wxMouseEvent& event )
{
  int newx, newy;
  ThotBool wrap;

  /* check the coordinates */
  newx = m_X + TtaGridDoAlign( (int)(event.GetX() - m_X) );
  newy = m_Y + TtaGridDoAlign( (int)(event.GetY() - m_Y) );
  //  TTALOGDEBUG_2( TTA_LOG_SVGEDIT, _T("AmayaAddPointEvtHandler::OnMouseMove newx=%d newy=%d"), newx, newy );

  /* Update the X position */
  if (newx < m_X)
    {
      newx = m_X;
      wrap = TRUE;
    }
  else if (newx > m_X + m_Width)
    {
      newx = m_X + m_Width;
      wrap = TRUE;
    }
  
  /* Update the Y position */
  if (newy < m_Y)
    {
      newy = m_Y;
      wrap = TRUE;
    }
  else if (newy > m_Y + m_Height)
    {
      newy = m_Y + m_Height;
      wrap = TRUE;
    }

  /* shows the new adjacent segment position */

  /* SG: this doesn't work yet because I must write code which create, merge and swap GL buffers :
   *    - one for the background picture and allready drawn segments
   *    - one for the current edited segment (following the mouse on the screen ) */

#ifndef _GL
  //  wxClientDC dc(m_pFrame->GetCanvas());
#endif /* _GL */
  if (m_X1 != -1)
    {
#ifdef _GL
      //GL_DrawLine(m_X1, m_Y1, m_LastX, m_LastY, FALSE);
      //GL_DrawLine(m_X1, m_Y1, newx, newy, FALSE);
#else /* _GL */
      //dc.DrawLine(m_X1, m_Y1, m_LastX, m_LastY);
      //dc.DrawLine(m_X1, m_Y1, newx, newy);
#endif /* _GL */
    }
  if (m_X3 != -1)
    {
#ifdef _GL
      //GL_DrawLine(m_LastX, m_LastY, m_X3, m_Y3, FALSE);
      //GL_DrawLine(newx, newy, m_X3, m_Y3, FALSE);
#else /* _GL */
      //dc.DrawLine(m_LastX, m_LastY, m_X3, m_Y3);
      //dc.DrawLine(newx, newy, m_X3, m_Y3);
#endif /* _GL */
    }

  m_LastX = newx;
  m_LastY = newy;
  if (wrap)
    {
      //      xwindow = (GdkWindowPrivate*) w;
      //      XWarpPointer (GDK_DISPLAY(), 
      //		    None, 
      //		    xwindow->xwindow,
      //		    0, 0, 0, 0, lastx, lasty);
      wrap = FALSE;
    }
  m_pFrame->GetCanvas()->Refresh();
}

/*----------------------------------------------------------------------
 *       Class:  AmayaAddPointEvtHandler
 *      Method:  OnMouseWheel
 * Description:  handle mouse wheel events
  -----------------------------------------------------------------------*/
void AmayaAddPointEvtHandler::OnMouseWheel( wxMouseEvent& event )
{
  //  TTALOGDEBUG_0( TTA_LOG_SVGEDIT, _T("AmayaAddPointEvtHandler::OnMouseWheel") );
  //  event.Skip();
}

#if 0
/*----------------------------------------------------------------------
 *       Class:  AmayaAddPointEvtHandler
 *      Method:  OnPaint
 * Description:  
  -----------------------------------------------------------------------*/
void AmayaAddPointEvtHandler::OnPaint( wxPaintEvent& event )
{
  /*
   * Note that In a paint event handler, the application must
   * always create a wxPaintDC object, even if you do not use it.
   * Otherwise, under MS Windows, refreshing for this and
   * other windows will go wrong.
   */
  wxPaintDC dc(this);

  // get the current frame id
  int frame = m_FrameId;

  int x,y,w,h;                             // Dimensions of client area in pixels
  wxRegionIterator upd(GetUpdateRegion()); // get the update rect list
  while (upd)
  {
    x = upd.GetX();
    y = upd.GetY();
    w = upd.GetW();
    h = upd.GetH();
    
    // call the generic callback to really display the frame
    FrameExposeCallback ( frame, x, y, w, h );
    TTALOGDEBUG_5( TTA_LOG_SVGEDIT, _T("AmayaAddPointEvtHandler::OnPaint : frame=%d [x=%d, y=%d, w=%d, h=%d]"), m_pAmayaFrame->GetFrameId(), x, y, w, h );
    
    upd ++ ;
  }

  // not necesarry : cf cube.cpp sample
  //  event.Skip();
}
#endif /* 0 */

#endif /* _WX */
