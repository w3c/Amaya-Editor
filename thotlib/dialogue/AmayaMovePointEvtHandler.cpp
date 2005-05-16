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
#include "AmayaMovePointEvtHandler.h"

IMPLEMENT_DYNAMIC_CLASS(AmayaMovePointEvtHandler, wxEvtHandler)

/*----------------------------------------------------------------------
 *  this is where the event table is declared
 *  the callbacks are assigned to an event type
 *----------------------------------------------------------------------*/
BEGIN_EVENT_TABLE(AmayaMovePointEvtHandler, wxEvtHandler)
  EVT_CHAR( AmayaMovePointEvtHandler::OnChar )

  EVT_LEFT_DOWN(	AmayaMovePointEvtHandler::OnMouseDown) // Process a wxEVT_LEFT_DOWN event. 
  EVT_LEFT_UP(		AmayaMovePointEvtHandler::OnMouseUp) // Process a wxEVT_LEFT_UP event. 
  EVT_LEFT_DCLICK(	AmayaMovePointEvtHandler::OnMouseDbClick) // Process a wxEVT_LEFT_DCLICK event. 
  EVT_MIDDLE_DOWN(	AmayaMovePointEvtHandler::OnMouseDown) // Process a wxEVT_MIDDLE_DOWN event. 
  EVT_MIDDLE_UP(	AmayaMovePointEvtHandler::OnMouseUp) // Process a wxEVT_MIDDLE_UP event. 
  EVT_MIDDLE_DCLICK(	AmayaMovePointEvtHandler::OnMouseDbClick) // Process a wxEVT_MIDDLE_DCLICK event. 
  EVT_RIGHT_DOWN(	AmayaMovePointEvtHandler::OnMouseDown) // Process a wxEVT_RIGHT_DOWN event. 
  EVT_RIGHT_UP(		AmayaMovePointEvtHandler::OnMouseUp) // Process a wxEVT_RIGHT_UP event. 
  EVT_RIGHT_DCLICK(	AmayaMovePointEvtHandler::OnMouseDbClick) // Process a wxEVT_RIGHT_DCLICK event. 
  EVT_MOTION(		AmayaMovePointEvtHandler::OnMouseMove) // Process a wxEVT_MOTION event. 
  EVT_MOUSEWHEEL(	AmayaMovePointEvtHandler::OnMouseWheel) // Process a wxEVT_MOUSEWHEEL event. 
END_EVENT_TABLE()

AmayaMovePointEvtHandler::AmayaMovePointEvtHandler() : wxEvtHandler()
{
}

AmayaMovePointEvtHandler::AmayaMovePointEvtHandler(AmayaFrame * p_frame,
                                                   PtrBox box,
                                                   int firstx, int firsty,
                                                   int x1, int y1, int x3, int y3,
                                                   int lastx, int lasty, int point,
                                                   int x, int y, int width, int height,
                                                   PtrTextBuffer Pbuffer, PtrTextBuffer Bbuffer,
                                                   int pointselect )
  : wxEvtHandler()
    ,m_IsFinish(false)
    ,m_pFrame(p_frame)
    ,m_FrameId(p_frame->GetFrameId())
    ,m_X(x)
    ,m_Y(y)
    ,m_Width(width)
    ,m_Height(height)
    ,m_X1(x1)
    ,m_Y1(y1)
    ,m_X3(x3)
    ,m_Y3(y3)
    ,m_FirstX(firstx)
    ,m_FirstY(firsty)
    ,m_LastX(lastx)
    ,m_LastY(lasty)
    ,m_Point(point)
    ,m_PointSelect(pointselect)
    ,m_Pbuffer(Pbuffer)
    ,m_Bbuffer(Bbuffer)
    ,m_Box(box)
{
  TTALOGDEBUG_0( TTA_LOG_SVGEDIT, _T("AmayaMovePointEvtHandler::AmayaMovePointEvtHandler"));

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

AmayaMovePointEvtHandler::~AmayaMovePointEvtHandler()
{
  TTALOGDEBUG_0( TTA_LOG_SVGEDIT, _T("AmayaMovePointEvtHandler::~AmayaMovePointEvtHandler") );

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

bool AmayaMovePointEvtHandler::IsFinish()
{
  return m_IsFinish;
}

void AmayaMovePointEvtHandler::OnChar( wxKeyEvent& event )
{
  m_IsFinish = true;
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaMovePointEvtHandler
 *      Method:  OnMouseDown
 * Description:  handle mouse button down events
 *--------------------------------------------------------------------------------------
 */
void AmayaMovePointEvtHandler::OnMouseDown( wxMouseEvent& event )
{
  //  TTALOGDEBUG_0( TTA_LOG_SVGEDIT, _T("AmayaMovePointEvtHandler::OnMouseDown") );
  //  event.Skip();
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaMovePointEvtHandler
 *      Method:  OnMouseUp
 * Description:  handle mouse button up events
 *--------------------------------------------------------------------------------------
 */
void AmayaMovePointEvtHandler::OnMouseUp( wxMouseEvent& event )
{
  if (IsFinish())
    return;

  if (m_Box->BxAbstractBox->AbEnclosing &&
      m_Box->BxAbstractBox->AbEnclosing->AbBox)
    /* suppose the enclosing box has the same width and height as
       the polyline box */
    {
      m_Box->BxAbstractBox->AbEnclosing->AbBox->BxHeight = m_Box->BxHeight;
      m_Box->BxAbstractBox->AbEnclosing->AbBox->BxH      = m_Box->BxH;
      m_Box->BxAbstractBox->AbEnclosing->AbBox->BxWidth  = m_Box->BxWidth;
      m_Box->BxAbstractBox->AbEnclosing->AbBox->BxW      = m_Box->BxW;
    }

  m_IsFinish = true;
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaMovePointEvtHandler
 *      Method:  OnMouseDbClick
 * Description:  handle mouse dbclick events
 *--------------------------------------------------------------------------------------
 */
void AmayaMovePointEvtHandler::OnMouseDbClick( wxMouseEvent& event )
{
  m_IsFinish = true;
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaMovePointEvtHandler
 *      Method:  OnMouseMove
 * Description:  handle mouse move events
 *--------------------------------------------------------------------------------------
 */
void AmayaMovePointEvtHandler::OnMouseMove( wxMouseEvent& event )
{
  int newx, newy, newx1, newy1;
  ThotBool wrap;
  
  /* check the coordinates */
  newx = m_X + TtaGridDoAlign( (int)(event.GetX() - m_X) );
  newy = m_Y + TtaGridDoAlign( (int)(event.GetY() - m_Y) );
  
  /* are limited to the box size */
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
  if (newx != m_LastX || newy != m_LastY)
    {
      m_LastX = newx;
      m_LastY = newy;		      
      /* update the box buffer */
      newx = LogicalValue (m_LastX - m_FirstX, UnPixel, NULL,
                           ViewFrameTable[m_FrameId - 1].FrMagnification);
      newy = LogicalValue (m_LastY - m_FirstY, UnPixel, NULL,
                           ViewFrameTable[m_FrameId - 1].FrMagnification);
      
      if (m_PointSelect == 0)
        /* it's really a polyline, not a line */
        {
          if (m_LastX - m_FirstX > m_Box->BxWidth)
            {
              m_Box->BxWidth = m_LastX - m_FirstX;
              m_Box->BxW = m_Box->BxWidth;
            }
          if (m_LastY - m_FirstY > m_Box->BxHeight)
            {
              m_Box->BxHeight = m_LastY - m_FirstY;
              m_Box->BxH  = m_Box->BxHeight;
            }
          if (m_LastX < m_Box->BxClipX + EXTRA_GRAPH)
            m_Box->BxClipX = m_LastX - EXTRA_GRAPH;
          if (m_LastY < m_Box->BxClipY + EXTRA_GRAPH)
            m_Box->BxClipY = m_LastY - EXTRA_GRAPH;
        }
      
      ModifyPointInPolyline (m_Bbuffer, m_Point, newx, newy);
      
      /* update the abstract box buffer */
      newx1 = (int) ((float) newx * m_RatioX);
      newy1 = (int) ((float) newy * m_RatioY);
      
      ModifyPointInPolyline (m_Pbuffer, m_Point, newx1, newy1);
      
      if (m_PointSelect == 0)
        {
          if (m_Box->BxPictInfo != NULL)
            {
              /* we have to recompute the current spline */
              free ((STRING) m_Box->BxPictInfo);
              m_Box->BxPictInfo = NULL;
            }
        }
      
      DefBoxRegion (m_FrameId, m_Box, 0, 0, m_Width, m_Height);
      RedrawFrameBottom (m_FrameId, 0, NULL);
      FrameTable[m_FrameId].DblBuffNeedSwap = TRUE;
      GL_Swap (m_FrameId);

      if (wrap)
        {
          /*          xwindow = (GdkWindowPrivate*) w;
          XWarpPointer (GDK_DISPLAY(), 
                        None, 
                        xwindow->xwindow,
                        0, 0, 0, 0, lastx, lasty);*/
          wrap = FALSE;
        }
    }
  
  m_pFrame->GetCanvas()->Refresh();
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaMovePointEvtHandler
 *      Method:  OnMouseWheel
 * Description:  handle mouse wheel events
 *--------------------------------------------------------------------------------------
 */
void AmayaMovePointEvtHandler::OnMouseWheel( wxMouseEvent& event )
{
  //  TTALOGDEBUG_0( TTA_LOG_SVGEDIT, _T("AmayaMovePointEvtHandler::OnMouseWheel") );
  //  event.Skip();
}

#endif /* _WX */
