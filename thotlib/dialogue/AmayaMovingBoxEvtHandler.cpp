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
#include "AmayaMovingBoxEvtHandler.h"

IMPLEMENT_DYNAMIC_CLASS(AmayaMovingBoxEvtHandler, wxEvtHandler)

/*----------------------------------------------------------------------
 *  this is where the event table is declared
 *  the callbacks are assigned to an event type
 *----------------------------------------------------------------------*/
BEGIN_EVENT_TABLE(AmayaMovingBoxEvtHandler, wxEvtHandler)
  EVT_KEY_DOWN( AmayaMovingBoxEvtHandler::OnChar )
  EVT_LEFT_DOWN(	AmayaMovingBoxEvtHandler::OnMouseDown) // Process a wxEVT_LEFT_DOWN event. 
  EVT_LEFT_UP(		AmayaMovingBoxEvtHandler::OnMouseUp) // Process a wxEVT_LEFT_UP event. 
  EVT_LEFT_DCLICK(	AmayaMovingBoxEvtHandler::OnMouseDbClick) // Process a wxEVT_LEFT_DCLICK event. 
  EVT_MIDDLE_DOWN(	AmayaMovingBoxEvtHandler::OnMouseDown) // Process a wxEVT_MIDDLE_DOWN event. 
  EVT_MIDDLE_UP(	AmayaMovingBoxEvtHandler::OnMouseUp) // Process a wxEVT_MIDDLE_UP event. 
  EVT_MIDDLE_DCLICK(	AmayaMovingBoxEvtHandler::OnMouseDbClick) // Process a wxEVT_MIDDLE_DCLICK event. 
  EVT_RIGHT_DOWN(	AmayaMovingBoxEvtHandler::OnMouseDown) // Process a wxEVT_RIGHT_DOWN event. 
  EVT_RIGHT_UP(		AmayaMovingBoxEvtHandler::OnMouseUp) // Process a wxEVT_RIGHT_UP event. 
  EVT_RIGHT_DCLICK(	AmayaMovingBoxEvtHandler::OnMouseDbClick) // Process a wxEVT_RIGHT_DCLICK event. 
  EVT_MOTION(		AmayaMovingBoxEvtHandler::OnMouseMove) // Process a wxEVT_MOTION event. 
  EVT_MOUSEWHEEL(	AmayaMovingBoxEvtHandler::OnMouseWheel) // Process a wxEVT_MOUSEWHEEL event. 
END_EVENT_TABLE()

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
AmayaMovingBoxEvtHandler::AmayaMovingBoxEvtHandler() : wxEvtHandler()
{
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
AmayaMovingBoxEvtHandler::AmayaMovingBoxEvtHandler( AmayaFrame * p_frame,
                                                    int *x, int *y, int width, int height,
                                                    PtrBox box, int xmin, int xmax, int ymin, int ymax,
                                                    int xm, int ym, int xref, int yref )
  : wxEvtHandler()
    ,m_IsFinish(false)
    ,m_pFrame(p_frame)
    ,m_FrameId(p_frame->GetFrameId())
    ,m_pX(x)
    ,m_pY(y)
    ,m_Width(width)
    ,m_Height(height)
    ,m_Box(box)
    ,m_Xmin(xmin)
    ,m_Xmax(xmax)
    ,m_Ymin(ymin)
    ,m_Ymax(ymax)
    ,m_Xm(xm)
    ,m_Ym(ym)    
    ,m_Xref(xref)
    ,m_Yref(yref)
{
  TTALOGDEBUG_0( TTA_LOG_SVGEDIT, _T("AmayaMovingBoxEvtHandler::AmayaMovingBoxEvtHandler"));

  if (m_pFrame)
    {
      // attach this handler to the canvas
      AmayaCanvas * p_canvas = m_pFrame->GetCanvas();
      p_canvas->PushEventHandler(this);
printf("Move capture\n");
      p_canvas->CaptureMouse();
      // assign a cross mouse cursor
      p_canvas->SetCursor( wxCursor(wxCURSOR_CROSS) );
    }
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
AmayaMovingBoxEvtHandler::~AmayaMovingBoxEvtHandler()
{
  TTALOGDEBUG_0( TTA_LOG_SVGEDIT, _T("AmayaMovingBoxEvtHandler::~AmayaMovingBoxEvtHandler") );

  if (m_pFrame)
    {
      // detach this handler from the canvas (restore default behaviour)
      AmayaCanvas * p_canvas = m_pFrame->GetCanvas();
      p_canvas->ReleaseMouse();
      p_canvas->PopEventHandler(false /* do not delete myself */);
      
      // restore the default cursor
      p_canvas->SetCursor( wxNullCursor );
    }
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
bool AmayaMovingBoxEvtHandler::IsFinish()
{
  return m_IsFinish;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void AmayaMovingBoxEvtHandler::OnChar( wxKeyEvent& event )
{
  m_IsFinish = true;
}

/*----------------------------------------------------------------------
 *       Class:  AmayaMovingBoxEvtHandler
 *      Method:  OnMouseDown
 * Description:  handle mouse button down events
  -----------------------------------------------------------------------*/
void AmayaMovingBoxEvtHandler::OnMouseDown( wxMouseEvent& event )
{
  //  TTALOGDEBUG_0( TTA_LOG_SVGEDIT, _T("AmayaMovingBoxEvtHandler::OnMouseDown") );
  //  event.Skip();
  m_IsFinish = true;
}

/*----------------------------------------------------------------------
 *       Class:  AmayaMovingBoxEvtHandler
 *      Method:  OnMouseUp
 * Description:  handle mouse button up events
  -----------------------------------------------------------------------*/
void AmayaMovingBoxEvtHandler::OnMouseUp( wxMouseEvent& event )
{
  m_IsFinish = true;
}

/*----------------------------------------------------------------------
 *       Class:  AmayaMovingBoxEvtHandler
 *      Method:  OnMouseDbClick
 * Description:  handle mouse dbclick events
  -----------------------------------------------------------------------*/
void AmayaMovingBoxEvtHandler::OnMouseDbClick( wxMouseEvent& event )
{
  //  m_IsFinish = true;
  m_IsFinish = true;
}

/*----------------------------------------------------------------------
 *       Class:  AmayaMovingBoxEvtHandler
 *      Method:  OnMouseMove
 * Description:  handle mouse move events
  -----------------------------------------------------------------------*/
void AmayaMovingBoxEvtHandler::OnMouseMove( wxMouseEvent& event )
{
  if (IsFinish())
    return;

  int newx, newy, dx, dy, wrapx, wrapy;
  
  /* compute the new box origin */
  newx = *m_pX + event.GetX() - m_Xm;
  dx = m_Xmin + TtaGridDoAlign(newx - m_Xmin) - *m_pX;
  newy = *m_pY + event.GetY() - m_Ym;
  dy = m_Ymin + TtaGridDoAlign(newy - m_Ymin) - *m_pY;

  newx = dx + *m_pX;
  newy = dy + *m_pY;

  /* Checks for limits */
  wrapx = -1;
  wrapy = -1;
  if (m_Xmin == m_Xmax)
    {
      newx = m_Xmin;	/*left side */
      wrapx = m_Xm;
    }
  else if (newx < m_Xmin)
    {
      newx = m_Xmin;	/*left side */
      wrapx = m_Xm;
    }
  else if (newx + m_Width > m_Xmax)
    {
      if (m_Xmin + m_Width > m_Xmax)
        {
          newx = m_Xmin;	/*overflow on left side */
          wrapx = m_Xm;
        }
      else
        {
          newx = m_Xmin + TtaGridDoAlign(m_Xmax - m_Width - m_Xmin); /*cote droit */
          wrapx = m_Xm + newx - *m_pX;
        }
    }
  else
    m_Xm += dx;	/* New cursor location */
  dx = newx - *m_pX;

  if (m_Ymin == m_Ymax)
    {
      newy = m_Ymin;	/*upper border */
      wrapy = m_Ym;
    }
  else if (newy < m_Ymin)
    {
      newy = m_Ymin;	/*upper border */
      wrapy = m_Ym;
    }
  else if (newy + m_Height > m_Ymax)
    {
      if (m_Ymin + m_Height > m_Ymax)
        {
          newy = m_Ymin;	/*overflow on upper border */
          wrapy = m_Ym;
        }
      else
        {
          newy = m_Ymin + TtaGridDoAlign(m_Ymax - m_Height - m_Ymin);  /* bottom border */
          wrapy = m_Ym + newy - *m_pY;
        }
    }
  else
    m_Ym += dy;	/* New cursor location */
  dy = newy - *m_pY;


  /* Should we move the box */
  if ((dx != 0) || (dy != 0))
    {
#ifdef _GL
      DefBoxRegion (m_FrameId, m_Box, -1, -1, -1, -1);
      NewPosition (m_Box->BxAbstractBox, 
                   newx + (ViewFrameTable[m_FrameId - 1]).FrXOrg, m_Xref, 
                   newy + (ViewFrameTable[m_FrameId - 1]).FrYOrg, m_Yref, 
                   m_FrameId, TRUE);
      FrameTable[m_FrameId].DblBuffNeedSwap = TRUE;
      GL_Swap (m_FrameId);
#endif /* _GL */
      *m_pX = newx;
      *m_pY = newy;
    }
  
  /* Should we move the cursor */
  if (wrapx >= 0 || wrapy >= 0)
    {
      if (wrapx >= 0)
        m_Xm = wrapx;
      if (wrapy >= 0)
        m_Ym = wrapy;
      /*
        xwindow = (GdkWindowPrivate*) w;
        XWarpPointer (GDK_DISPLAY(), 
        None, 
        xwindow->xwindow,
        0, 0, 0, 0, xm, ym);
      */
    }
}

/*----------------------------------------------------------------------
 *       Class:  AmayaMovingBoxEvtHandler
 *      Method:  OnMouseWheel
 * Description:  handle mouse wheel events
  -----------------------------------------------------------------------*/
void AmayaMovingBoxEvtHandler::OnMouseWheel( wxMouseEvent& event )
{
  //  TTALOGDEBUG_0( TTA_LOG_SVGEDIT, _T("AmayaMovingBoxEvtHandler::OnMouseWheel") );
  //  event.Skip();
}

#if 0
/*----------------------------------------------------------------------
 *       Class:  AmayaMovingBoxEvtHandler
 *      Method:  OnPaint
 * Description:  
  -----------------------------------------------------------------------*/
void AmayaMovingBoxEvtHandler::OnPaint( wxPaintEvent& event )
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
    TTALOGDEBUG_5( TTA_LOG_SVGEDIT, _T("AmayaMovingBoxEvtHandler::OnPaint : frame=%d [x=%d, y=%d, w=%d, h=%d]"), m_pAmayaFrame->GetFrameId(), x, y, w, h );
    
    upd ++ ;
  }

  // not necesarry : cf cube.cpp sample
  //  event.Skip();
}
#endif /* 0 */

#endif /* _WX */
