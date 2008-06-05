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
  EVT_CHAR( AmayaResizingBoxEvtHandler::OnChar )

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
AmayaResizingBoxEvtHandler::AmayaResizingBoxEvtHandler( AmayaFrame * p_frame,
                                                        int *x, int *y, int *width, int *height,
                                                        PtrBox box, int xmin, int xmax, int ymin, int ymax,
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
    ,m_PercentW(percentW)
    ,m_PercentH(percentH)
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
  m_IsEllipse = (pAb && pAb->AbLeafType == LtGraphics && (pAb->AbShape == 'a' || pAb->AbShape == 'c'));

  /* Use the reference point to move the box */
  switch (m_Box->BxHorizEdge)
    {
    case Right:
      m_Xref = *m_pWidth;
      m_RefV = C_RIGHT;
      /* Box shortened when X delta increase */
      m_HDirection = -1;
      break;
    case VertMiddle:
      m_Xref = *m_pWidth / 2;
      m_RefV = C_VCENTER;
      /* Direction depends of original cursor location in the frame */
      if (m_Xm < *m_pX + m_Xref)
        m_HDirection = -1;
      else
        m_HDirection = 1;
      break;
    case VertRef:
      m_Xref = m_Box->BxVertRef;
      m_RefV = C_LEFT;
      /* Box increased when X delta increase */
      m_HDirection = 1;
      break;
    default:
      m_Xref = 0;
      m_RefV = C_LEFT;
      /* Box increased when X delta increase */
      m_HDirection = 1;
      break;
    }

  switch (m_Box->BxVertEdge)
    {
    case Bottom:
      m_Yref = *m_pHeight;
      m_RefH = C_BOTTOM;
      /* Box shortened when Y delta increase */
      m_VDirection = -1;
      break;
    case HorizMiddle:
      m_Yref = *m_pHeight / 2;
      m_RefH = C_HCENTER;
      /* Direction depends of original cursor location in the frame */
      if (m_Ym < *m_pY + m_Yref)
        m_VDirection = -1;
      else
        m_VDirection = 1;
      break;
    case HorizRef:
      m_Yref = m_Box->BxHorizRef;
      m_RefH = C_TOP;
      /* Box increased when Y delta increase */
      m_VDirection = 1;
      break;
    default:
      m_Yref = 0;
      m_RefH = C_TOP;
      /* Box increased when Y delta increase */
      m_VDirection = 1;
      break;
    }
  
  /* Shows the initial box size */
  if (m_IsEllipse)
    InvertEllipse (m_FrameId, *m_pX, *m_pY, *m_pWidth, *m_pHeight, *m_pX + m_Xref, *m_pY + m_Yref);
  else
    BoxGeometry (m_FrameId, *m_pX, *m_pY, *m_pWidth, *m_pHeight, *m_pX + m_Xref, *m_pY + m_Yref);
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
  if (IsFinish())
    return;

  int dx, dy, dl, dh, warpx, warpy;

  /* compute the deltas */
  dl = (int) (event.GetX() - m_Xm);
  dh = (int) (event.GetY() - m_Ym);
  if (m_PercentW != 0)
    {
      /* keep the greater value */
      if (dl < dh)
        dl = dh;
      dh = dl;
    }
  else if (m_PercentH != 0)
    {
      /* keep the greater value */
      if (dh < dl)
        dh = dl;
      dl = dh;
    }
  
  /* Check that size can be modified, and stay >= 0    */
  /* depending on base point and cursor position,      */
  /* increase or decreas width or height accordingly   */
  warpx = -1;
  warpy = -1;
  if (dl != 0)
    {
      if (m_Xmin == m_Xmax)
        /* X moves frozen */
        dl = 0;
      else if (m_RefV == C_VCENTER && *m_pWidth + (2 * dl * m_HDirection) < 0)
        {
          dl = - TtaGridDoAlign(*m_pWidth / 2) * m_HDirection;
          warpx = m_Xm + (dl * m_HDirection);
        }
      else if (*m_pWidth + (dl * m_HDirection) < 0)
        {
          dl = - TtaGridDoAlign(*m_pWidth) * m_HDirection;
          warpx = m_Xm + (dl * m_HDirection);
        }
    }
  
  if (dh != 0)
    {
      if (m_Ymin == m_Ymax)
        /* Y moves frozen */
        dh = 0;
      else if (m_RefH == C_HCENTER && *m_pHeight + (2 * dh * m_VDirection) < 0)
        {
          dh = - TtaGridDoAlign(*m_pHeight / 2) * m_VDirection;
          warpy = m_Ym + (dh * m_VDirection);
        }
      else if (*m_pHeight + dh < 0)
        {
          dh = - TtaGridDoAlign(*m_pHeight) * m_VDirection;
          warpy = m_Ym + (dh * m_VDirection);
        }
    }
  
  /* Compute the horizontal move of the origin */
  if (dl != 0)
    {
      dl = dl * m_HDirection;		/* Take care for direction */
      if (m_RefV == C_VCENTER)
        {
          dx = m_Xmin + TtaGridDoAlign(*m_pX - (dl / 2) - m_Xmin) - *m_pX;
          /* Check the move is within limits */
          if (*m_pX + dx < m_Xmin)
            dx = m_Xmin - *m_pX;		/*left side */
          if (*m_pX + *m_pWidth - dx > m_Xmax)
            dx = *m_pX + *m_pWidth - m_Xmin - TtaGridDoAlign(m_Xmax - m_Xmin); /*right side */
          
          /* modify width for real */
          dl = -(dx * 2);
          if (dx != 0)
            warpx = m_Xm - (dx * m_HDirection);
        }
      else if (m_RefV == C_RIGHT)
        {
          dx = m_Xmin + TtaGridDoAlign(*m_pX - dl - m_Xmin) - *m_pX;
          /* Check the move is within limits */
          if (*m_pX + dx < m_Xmin)
            dx = m_Xmin - *m_pX;		/*left side */
          
          /* modify width for real */
          dl = -dx;
          if (dx != 0)
            warpx = m_Xm + dx;
        }
      else
        {
          dx = 0;
          dl = m_Xmin + TtaGridDoAlign(*m_pX + *m_pWidth + dl - m_Xmin) - *m_pX - *m_pWidth;
          if (*m_pX + *m_pWidth + dl > m_Xmax)
            dl = m_Xmin + TtaGridDoAlign(m_Xmax - m_Xmin) - *m_pX - *m_pWidth; /*right side */
          if (dl != 0)
            warpx = m_Xm + dl;
        }
    }
  else
    dx = 0;
  
  /* Compute vertical move */
  if (dh != 0)
    {
      dh = dh * m_VDirection;	/* Take care for direction */
      if (m_RefH == C_HCENTER)
        {
          dy = m_Ymin + TtaGridDoAlign(*m_pY - (dh / 2) - m_Ymin) - *m_pY;
          /* Check the move is within limits */
          if (*m_pY + dy < m_Ymin)
            dy = m_Ymin - *m_pY;		/*upper border */
          if (*m_pY + *m_pHeight - dy > m_Ymax)
            dy = *m_pY + *m_pHeight - m_Ymin - TtaGridDoAlign(m_Ymax - m_Ymin);	/* bottom */
          /* change the height for real */
          dh = -(dy * 2);
          if (dy != 0)
            warpy = m_Ym - (dy * m_VDirection);
        }
      else if (m_RefH == C_BOTTOM)
        {
          dy = m_Ymin + TtaGridDoAlign(*m_pY - dh - m_Ymin) - *m_pY;
          /* Check the move is within limits */
          if (*m_pY + dy < m_Ymin)
            dy = m_Ymin - *m_pY;		/* upper border */
          /* change the height for real */
          dh = -dy;
          if (dy != 0)
            warpy = m_Ym + dy;
        }
      else
        {
          dy = 0;
          dh = m_Ymin + TtaGridDoAlign(*m_pY + *m_pHeight + dh - m_Ymin) - *m_pY - *m_pHeight;
          if (*m_pY + *m_pHeight + dh > m_Ymax)
            dh = m_Ymin + TtaGridDoAlign(m_Ymax - m_Ymin) - *m_pY - *m_pHeight;	/* bottom */
          if (dh != 0)
            warpy = m_Ym + dh;
        }
    }
  else
    dy = 0;
  
  /* Should we move the box */
  if ((dl != 0) || (dh != 0))
    {
      /* switch off the old box */
#ifndef _GL
      if (m_IsEllipse)
        InvertEllipse (m_FrameId, *m_pX, *m_pY, *m_pWidth, *m_pHeight, *m_pX + m_Xref, *m_pY + m_Yref);
      else
        BoxGeometry (m_FrameId, *m_pX, *m_pY, *m_pWidth, *m_pHeight, *m_pX + m_Xref, *m_pY + m_Yref);
#endif /* _GL */
      /* is there any dependence between height and width */
      *m_pWidth = *m_pWidth + dl;
      *m_pHeight = *m_pHeight + dh;
      if (m_PercentW != 0)
        *m_pWidth = *m_pHeight * m_PercentW / 100;
      else if (m_PercentH != 0)
        *m_pHeight = *m_pWidth * m_PercentH / 100;
      *m_pX = *m_pX + dx;
      *m_pY = *m_pY + dy;
      /* switch on the new one */
      switch (m_Box->BxHorizEdge)
        {
        case Right:
          m_Xref = *m_pWidth;
          break;
        case VertMiddle:
          m_Xref = *m_pWidth / 2;
          break;
        case VertRef:
          m_Xref = m_Box->BxVertRef;
          break;
        default:
          m_Xref = 0;
          break;
        }
      switch (m_Box->BxVertEdge)
        {
        case Bottom:
          m_Yref = *m_pHeight;
          break;
        case HorizMiddle:
          m_Yref = *m_pHeight / 2;
          break;
        case HorizRef:
          m_Yref = m_Box->BxHorizRef;
          break;
        default:
          m_Yref = 0;
          break;
        }  
#ifndef _GL
      if (m_IsEllipse)
        InvertEllipse (m_FrameId, *m_pX, *m_pY, *m_pWidth, *m_pHeight, *m_pX + m_Xref, *m_pY + m_Yref);
      else
        BoxGeometry (m_FrameId, *m_pX, *m_pY, *m_pWidth, *m_pHeight, *m_pX + m_Xref, *m_pY + m_Yref);
#else /* _GL */
      DefBoxRegion (m_FrameId, m_Box, -1, -1, -1, -1);
      if (m_PercentW)
        NewDimension (m_Box->BxAbstractBox, 0, *m_pHeight, m_FrameId, TRUE);
      else if (m_PercentH)
        NewDimension (m_Box->BxAbstractBox, *m_pWidth, 0, m_FrameId, TRUE);
      else
        NewDimension (m_Box->BxAbstractBox, *m_pWidth, *m_pHeight, m_FrameId, TRUE);
      
      FrameTable[m_FrameId].DblBuffNeedSwap = TRUE;
      GL_Swap (m_FrameId);
#endif /* _GL */
    }
  
  /* Should we move the cursor */
  if (warpx >= 0 || warpy >= 0)
    {
      if (warpx >= 0)
        m_Xm = warpx;
      if (warpy >= 0)
        m_Ym = warpy; 
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
