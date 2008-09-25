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
#include "undo.h"
#include "picture.h"

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

#define C_TOP 0
#define C_HCENTER 1
#define C_BOTTOM 2
#define C_LEFT 0
#define C_VCENTER 1
#define C_RIGHT 2

#include "AmayaFrame.h"
#include "AmayaCanvas.h"
#include "AmayaResizingBoxEvtHandler.h"
static int      Waiting = 0;

IMPLEMENT_DYNAMIC_CLASS(AmayaResizingBoxEvtHandler, wxEvtHandler)

/*----------------------------------------------------------------------
 *  this is where the event table is declared
 *  the callbacks are assigned to an event type
 *----------------------------------------------------------------------*/
BEGIN_EVENT_TABLE(AmayaResizingBoxEvtHandler, wxEvtHandler)
  EVT_KEY_DOWN( AmayaResizingBoxEvtHandler::OnChar )
  EVT_LEFT_DOWN( AmayaResizingBoxEvtHandler::OnMouseDown) // Process a wxEVT_LEFT_DOWN event. 
  EVT_LEFT_UP( AmayaResizingBoxEvtHandler::OnMouseUp) // Process a wxEVT_LEFT_UP event. 
  EVT_LEFT_DCLICK( AmayaResizingBoxEvtHandler::OnMouseDbClick) // Process a wxEVT_LEFT_DCLICK event. 
  EVT_MIDDLE_DOWN( AmayaResizingBoxEvtHandler::OnMouseDown) // Process a wxEVT_MIDDLE_DOWN event. 
  EVT_MIDDLE_UP( AmayaResizingBoxEvtHandler::OnMouseUp) // Process a wxEVT_MIDDLE_UP event. 
  EVT_MIDDLE_DCLICK( AmayaResizingBoxEvtHandler::OnMouseDbClick) // Process a wxEVT_MIDDLE_DCLICK event. 
  EVT_RIGHT_DOWN( AmayaResizingBoxEvtHandler::OnMouseDown) // Process a wxEVT_RIGHT_DOWN event. 
  EVT_RIGHT_UP( AmayaResizingBoxEvtHandler::OnMouseUp) // Process a wxEVT_RIGHT_UP event. 
  EVT_RIGHT_DCLICK( AmayaResizingBoxEvtHandler::OnMouseDbClick) // Process a wxEVT_RIGHT_DCLICK event. 
  EVT_MOTION( AmayaResizingBoxEvtHandler::OnMouseMove) // Process a wxEVT_MOTION event. 
  EVT_MOUSEWHEEL(	AmayaResizingBoxEvtHandler::OnMouseWheel) // Process a wxEVT_MOUSEWHEEL event. 
END_EVENT_TABLE()

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
AmayaResizingBoxEvtHandler::AmayaResizingBoxEvtHandler() : wxEvtHandler()
{
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
AmayaResizingBoxEvtHandler::AmayaResizingBoxEvtHandler (AmayaFrame * p_frame, int x, int y,
                                                        int *width, int *height, PtrBox box,
                                                        int xmin, int xmax, int ymin, int ymax,
                                                        int xm, int ym, int percentW, int percentH )
  : wxEvtHandler()
    ,m_IsFinish(false)
    ,m_pFrame(p_frame)
    ,m_FrameId(p_frame->GetFrameId())
    ,m_pX(x)
    ,m_pY(y)
    ,m_pWidth(width)
    ,m_pHeight(height)
    ,m_Box(box)
    ,m_Xmin(xmin)
    ,m_Xmax(xmax)
    ,m_Ymin(ymin)
    ,m_Ymax(ymax)
    ,m_Xm(xm)
    ,m_Ym(ym)    
{
  TTALOGDEBUG_0( TTA_LOG_SVGEDIT, _T("AmayaResizingBoxEvtHandler::AmayaResizingBoxEvtHandler"));
  if (m_pFrame)
    {
      // attach this handler to the canvas
      AmayaCanvas * p_canvas = m_pFrame->GetCanvas();
      p_canvas->PushEventHandler(this);
      TtaOpenUndoSequence (FrameTable[m_FrameId].FrDoc, NULL, NULL, 0, 0);
      // keep the history open until the button is up
      TtaLockHistory (TRUE);
      
      // assign a cross mouse cursor
      m_pFrame->GetCanvas()->SetCursor( wxCursor(wxCURSOR_CROSS) );
#ifndef _MACOS
      m_pFrame->GetCanvas()->CaptureMouse();
#endif /* _MACOS */
      // waiting for a release
      Waiting = 1;
    }

  // check if this box contains an ellipse
  PtrAbstractBox pAb;
  pAb = m_Box->BxAbstractBox;
  if (pAb && pAb->AbLeafType == LtCompound)
    pAb = pAb->AbFirstEnclosed;
  m_IsEllipse = (pAb && pAb->AbLeafType == LtGraphics && pAb->AbShape == 'a');

  /* locate the mouse position in the box */
  if (xm < m_Box->BxClipX + *m_pWidth/2 - DELTA_SEL)
    m_HDirection = -1;
  else if (xm < m_Box->BxClipX + *m_pWidth/2 + DELTA_SEL)
    m_HDirection = 0;
  else
    m_HDirection = 1;

  if (ym < m_Box->BxClipY + *m_pHeight/2 - DELTA_SEL)
    m_VDirection = -1;
  else if (ym < m_Box->BxClipY + *m_pHeight/2 + DELTA_SEL)
    m_VDirection = 0;
  else
    m_VDirection = 1;
  /* Shows the initial box size */
  BoxGeometry (m_FrameId, m_pX, m_pY, *m_pWidth, *m_pHeight, m_pX + m_Xref, m_pY);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
AmayaResizingBoxEvtHandler::~AmayaResizingBoxEvtHandler()
{
  TTALOGDEBUG_0( TTA_LOG_SVGEDIT, _T("AmayaResizingBoxEvtHandler::~AmayaResizingBoxEvtHandler") );

  if (m_pFrame)
    {
      // detach this handler from the canvas (restore default behaviour)
      AmayaCanvas * p_canvas = m_pFrame->GetCanvas();
      p_canvas->PopEventHandler(false /* do not delete myself */);
      
      // restore the default cursor
      m_pFrame->GetCanvas()->SetCursor( wxNullCursor );
      if (Waiting)
        {
          // should we release
#ifndef _MACOS
          m_pFrame->GetCanvas()->ReleaseMouse();
#endif /* _MACOS */
          Waiting = 0;
        }
    }
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
bool AmayaResizingBoxEvtHandler::IsFinish()
{
  return m_IsFinish;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void AmayaResizingBoxEvtHandler::OnChar( wxKeyEvent& event )
{
  if(event.GetKeyCode() !=  WXK_SHIFT)
    m_IsFinish = true;
}

/*----------------------------------------------------------------------
 *       Class:  AmayaResizingBoxEvtHandler
 *      Method:  OnMouseDown
 * Description:  handle mouse button down events
  -----------------------------------------------------------------------*/
void AmayaResizingBoxEvtHandler::OnMouseDown( wxMouseEvent& event )
{
  //  TTALOGDEBUG_0( TTA_LOG_SVGEDIT, _T("AmayaResizingBoxEvtHandler::OnMouseDown") );
  //  event.Skip();
}

/*----------------------------------------------------------------------
 *       Class:  AmayaResizingBoxEvtHandler
 *      Method:  OnMouseUp
 * Description:  handle mouse button up events
  -----------------------------------------------------------------------*/
void AmayaResizingBoxEvtHandler::OnMouseUp( wxMouseEvent& event )
{
  // close the history now
  TtaLockHistory (FALSE);
  TtaCloseUndoSequence (FrameTable[m_FrameId].FrDoc);
  m_IsFinish = true;
}

/*----------------------------------------------------------------------
 *       Class:  AmayaResizingBoxEvtHandler
 *      Method:  OnMouseDbClick
 * Description:  handle mouse dbclick events
  -----------------------------------------------------------------------*/
void AmayaResizingBoxEvtHandler::OnMouseDbClick( wxMouseEvent& event )
{
  //  m_IsFinish = true;
}

/*----------------------------------------------------------------------
 *       Class:  AmayaResizingBoxEvtHandler
 *      Method:  OnMouseMove
 * Description:  handle mouse move events
  -----------------------------------------------------------------------*/
void AmayaResizingBoxEvtHandler::OnMouseMove( wxMouseEvent& event )
{
  int           dx, dy, dl, dh, warpx, warpy;
  bool          ratio;
  ThotPictInfo *imageDesc;

  if (IsFinish())
    return;

  /* compute the deltas */
  ratio = m_Box && m_Box->BxType == BoPicture && event.ShiftDown();
  warpx = (int)event.GetX();
  warpy = (int)event.GetY();
  dl = (warpx - m_Xm) * m_HDirection;
  dh = (warpy - m_Ym) * m_VDirection;
  m_Xm = warpx;
  m_Ym = warpy;
  if (ratio)
    {
      /* keep the greater value */
      if (dl < dh)
        dl = 0;
      else
        dh = 0;
    }
  
  /* Check that size can be modified, and stay >= 0    */
  /* depending on base point and cursor position,      */
  /* increase or decreas width or height accordingly   */
  if (dl)
    {
      if (m_Xmin == m_Xmax)
        /* X moves frozen */
        dl = 0;
      else if (*m_pWidth + dl < 10)
        dl = 0;
    }
  
  if (dh)
    {
      if (m_Ymin == m_Ymax)
        /* Y moves frozen */
        dh = 0;
      else if (*m_pHeight + dh < 10)
        dh = 0;
    }
  
  dx = 0;
  dy = 0;
  if (m_IsEllipse)
    {
      // keep the ratio of the circle
      if (dl > dh)
        dh = dl;
      else
        dl = dh;
    }

  /* Should we move the box */
  if (dl || dh)
    {
      /* is there any dependence between height and width */
      *m_pWidth = *m_pWidth + dl;
      *m_pHeight = *m_pHeight + dh;

      // apply the ratio
      if (ratio)
        {
          imageDesc = (ThotPictInfo *) m_Box->BxPictInfo;
          if (imageDesc)
            {
              if (dl)
                *m_pHeight = *m_pWidth * imageDesc->PicHeight /imageDesc->PicWidth;
              else
                *m_pWidth = *m_pHeight * imageDesc->PicWidth /imageDesc->PicHeight;
            }
        }

      m_pX = m_pX + dx;
      m_pY = m_pY + dy;
      DefBoxRegion (m_FrameId, m_Box, -1, -1, -1, -1);
      NewDimension (m_Box->BxAbstractBox, *m_pWidth, *m_pHeight, m_FrameId, TRUE);
      
      FrameTable[m_FrameId].DblBuffNeedSwap = TRUE;
      GL_Swap (m_FrameId);
    }

}

/*----------------------------------------------------------------------
 *       Class:  AmayaResizingBoxEvtHandler
 *      Method:  OnMouseWheel
 * Description:  handle mouse wheel events
  -----------------------------------------------------------------------*/
void AmayaResizingBoxEvtHandler::OnMouseWheel( wxMouseEvent& event )
{
  //  TTALOGDEBUG_0( TTA_LOG_SVGEDIT, _T("AmayaResizingBoxEvtHandler::OnMouseWheel") );
  //  event.Skip();
}

#endif /* _WX */
