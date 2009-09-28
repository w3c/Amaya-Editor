/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2008
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

#ifdef _WX
#include "wx/wx.h"
#include "wx/button.h"
#include "wx/string.h"

#include "thot_gui.h"
#include "thot_sys.h"
#include "constmedia.h"

#include "typemedia.h"
#include "appdialogue.h"
#include "appdialogue_wx.h"
#include "dialog.h"
#include "application.h"
#include "dialog.h"
#include "document.h"
#include "message.h"
#include "libmsg.h"
#include "thot_key.h"
#include "frame.h"
#include "appdialogue_wx.h"
#include "logdebug.h"

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "edit_tv.h"
#include "appli_f.h"
#include "applicationapi_f.h"
#include "boxlocate_f.h"
#include "displayview_f.h"
#include "frame_tv.h"
#include "scroll_f.h"
#include "input_f.h"
#include "views_f.h"

#ifdef _GL
#include "glwindowdisplay.h"
#endif /*_GL*/

#include "AmayaApp.h"
#include "AmayaCanvas.h"
#include "AmayaFrame.h"
#include "AmayaPage.h"
#include "AmayaWindow.h"


#ifdef _GL
IMPLEMENT_DYNAMIC_CLASS(AmayaCanvas, wxGLCanvas)
#else /* _GL*/
  IMPLEMENT_DYNAMIC_CLASS(AmayaCanvas, wxPanel)
#endif /* _GL */


/*----------------------------------------------------------------------
  Class:  AmayaCanvas
  Method:  AmayaCanvas
  Description:  construct the canvas : its a wxGLCanvas if opengl is
  used or a wxPanel if not
  -----------------------------------------------------------------------*/
#ifdef _GL
AmayaCanvas::AmayaCanvas( wxWindow * p_parent_window,
                          AmayaFrame * p_parent_frame,
                          wxGLContext * p_shared_context )
  : wxGLCanvas( p_parent_window,
                p_shared_context,
                -1,
                wxDefaultPosition, wxDefaultSize, wxWANTS_CHARS , _T("AmayaCanvas"),
                AmayaApp::GetGL_AttrList() ),
#else // #ifdef _GL  
AmayaCanvas::AmayaCanvas( wxWindow * p_parent_window,
                          AmayaFrame * p_parent_frame )
  : wxPanel( p_parent_window ),
#endif // #ifdef _GL
    m_pAmayaFrame( p_parent_frame ),
    m_Init( false ),
    m_IsMouseSelecting( false ),
    m_MouseGrab (false)
{
#ifdef FORUMLARY_WIDGET_DEMO
  // demo de comment afficher des widgets dans une fenetre opengl
  // il faut creer un panel fils du canvas et y mettre un widget fils du panel
  wxWindow * p_panel = new wxPanel( this, -1, wxPoint(100,100), wxSize(50,50) );
  wxWindow * p_button = new wxButton( p_panel, -1, _T("Submit"), wxPoint(0,0) );
  p_panel->SetSize( p_button->GetSize() );
#endif /* FORUMLARY_WIDGET_DEMO */

  SetAutoLayout(TRUE);
  Layout();

  // we want this class receives timer events
  m_MouseMoveTimer.SetOwner(this);
}

/*----------------------------------------------------------------------
  Class:  AmayaCanvas
  Method:  ~AmayaCanvas
  Description:  destructor (do nothing)
  -----------------------------------------------------------------------*/
AmayaCanvas::~AmayaCanvas( )
{
  //  SetEventHandler( new wxEvtHandler() );
  TTALOGDEBUG_1( TTA_LOG_DIALOG, _T("AmayaCanvas::~AmayaCanvas(): frame=%d"), m_pAmayaFrame->GetFrameId() );
  m_pAmayaFrame = NULL;
  if (m_MouseGrab)
    {
      ReleaseMouse();
      m_MouseGrab = false;
    }
}

/*----------------------------------------------------------------------
  Class:  AmayaCanvas
  Method:  OnSize
  Description:  called when the canvas is resized
                just call the generic callback to resize and redraw the frame
  -----------------------------------------------------------------------*/
void AmayaCanvas::OnSize( wxSizeEvent& event )
{
#ifdef _GL
  // this is also necessary to update the context on some platforms
  wxGLCanvas::OnSize(event);
#endif /* _GL */

  // do not resize while opengl is not initialized
  if (!m_Init)
    {
      TTALOGDEBUG_3( TTA_LOG_DIALOG, _T("AmayaCanvas::OnSize: frame=%d w=%d h=%d (skip)"),
                     m_pAmayaFrame->GetFrameId(),
                     event.GetSize().GetWidth(),
                     event.GetSize().GetHeight() );
      event.Skip();
      return;
    }
  

  // get the current frame id
  int frame = m_pAmayaFrame->GetFrameId();

  // get the new dimensions  
  int new_width, new_height;
  // dont use event.GetSize() because it returns bad value ...
  new_width = GetClientSize().GetWidth();
  new_height = GetClientSize().GetHeight();
 
  // call the generic callback
  if (new_width != FrameTable[frame].FrWidth ||
      new_height != FrameTable[frame].FrHeight)
  FrameResizedCallback( frame, new_width, new_height );
}

/*----------------------------------------------------------------------
  Class:  AmayaCanvas
  Method:  OnPaint
  Description:  called when the canvas needs to be repainted
                just call the generic callback to redraw a the frame:
                FrameExposeCallback
  -----------------------------------------------------------------------*/
void AmayaCanvas::OnPaint( wxPaintEvent& event )
{
  /*
   * Note that In a paint event handler, the application must
   * always create a wxPaintDC object, even if you do not use it.
   * Otherwise, under MS Windows, refreshing for this and
   * other windows will go wrong.
   */
  wxPaintDC dc(this);

  // initialize the canvas context
  Init(); 

  // get the current frame id
  int frame = m_pAmayaFrame->GetFrameId();

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
      TTALOGDEBUG_5( TTA_LOG_DRAW, _T("AmayaCanvas::OnPaint : frame=%d [x=%d, y=%d, w=%d, h=%d]"), m_pAmayaFrame->GetFrameId(), x, y, w, h );
    
      upd ++ ;
    }

  // not necesarry : cf cube.cpp sample
  //  event.Skip();
}

/*----------------------------------------------------------------------
  Class:  AmayaCanvas
  Method:  OnMouseDbClick
  Description:  handle mouse dbclick button events
  -----------------------------------------------------------------------*/
void AmayaCanvas::OnMouseDbClick( wxMouseEvent& event )
{
  int frame = m_pAmayaFrame->GetFrameId();

  int thot_mod_mask = THOT_NO_MOD;
  if (event.ControlDown())
    thot_mod_mask |= THOT_MOD_CTRL;
  if (event.AltDown())
    thot_mod_mask |= THOT_MOD_ALT;
  if (event.ShiftDown())
    thot_mod_mask |= THOT_MOD_SHIFT;

  if (m_MouseGrab)
    {
      m_MouseGrab = false;
      ReleaseMouse();
    }
  TTALOGDEBUG_0( TTA_LOG_DRAW, _T("AmayaCanvas - wxEVT_LEFT_DCLICK || wxEVT_MIDDLE_DCLICK || wxEVT_RIGHT_DCLICK") );
  FrameButtonDClickCallback( frame,
                             event.GetButton(),
                             thot_mod_mask,
                             event.GetX(), event.GetY() );

#ifndef _WINDOWS
  // force the focus because on debian woody, the focus do not come in when clicking on the canvas
  TtaRedirectFocus();
#endif /* _WINDOWS */
}

/*----------------------------------------------------------------------
  Class:  AmayaCanvas
  Method:  OnMouseMove
  Description:  handle mouse move events
  -----------------------------------------------------------------------*/
void AmayaCanvas::OnMouseMove( wxMouseEvent& event )
{
  if (m_IsMouseSelecting && event.m_leftDown &&
      !m_MouseMoveTimer.IsRunning() )
    {
      if (!m_MouseGrab)
        {
          m_MouseGrab = true;
          CaptureMouse();
        }
      m_LastMouseMoveModMask = THOT_NO_MOD;
      if (event.ControlDown())
        m_LastMouseMoveModMask |= THOT_MOD_CTRL;
      if (event.AltDown())
        m_LastMouseMoveModMask |= THOT_MOD_ALT;
      if (event.ShiftDown())
        m_LastMouseMoveModMask |= THOT_MOD_SHIFT;
      
      m_LastMouseMoveX = event.GetX();
      m_LastMouseMoveY = event.GetY();
      
      // start the timer
      m_MouseMoveTimer.Start( 10, wxTIMER_ONE_SHOT );
    }
}

/*----------------------------------------------------------------------
  Class:  AmayaCanvas
  Method:  OnTimerMouseMove
  Description:  handle mouse move events, do not generate a move event
                each time the mouse move
                wait 10ms before generate a move event.
  -----------------------------------------------------------------------*/
void AmayaCanvas::OnTimerMouseMove( wxTimerEvent& event )
{
  TTALOGDEBUG_2( TTA_LOG_DRAW, _T("AmayaCanvas::OnTimerMouseMove: x=%d y=%d"), m_LastMouseMoveX, m_LastMouseMoveY );

  int frame = m_pAmayaFrame->GetFrameId();
  FrameMotionCallback( frame,
                       m_LastMouseMoveModMask,
                       m_LastMouseMoveX,
                       m_LastMouseMoveY );
}

/*----------------------------------------------------------------------
  Class:  AmayaCanvas
  Method:  OnMouseWheel
  Description:  handle mouse Wheel events
  -----------------------------------------------------------------------*/
void AmayaCanvas::OnMouseWheel( wxMouseEvent& event )
{
  int frame = m_pAmayaFrame->GetFrameId();

  int thot_mod_mask = THOT_NO_MOD;
  if (event.ControlDown())
    thot_mod_mask |= THOT_MOD_CTRL;
  if (event.AltDown())
    thot_mod_mask |= THOT_MOD_ALT;
  if (event.ShiftDown())
    thot_mod_mask |= THOT_MOD_SHIFT;

  int direction = event.GetWheelRotation();
  int delta     = event.GetWheelDelta();
  
  TTALOGDEBUG_3( TTA_LOG_DRAW, _T("AmayaCanvas - wxEVT_MOUSEWHEEL: frame=%d direction=%s delta=%d"),
                 m_pAmayaFrame->GetFrameId(),
                 direction > 0 ? _T("up") : _T("down"),
                 delta );
  
  if (m_MouseGrab)
    {
      m_MouseGrab = false;
      ReleaseMouse();
    }
  FrameMouseWheelCallback( frame,
                           thot_mod_mask,
                           direction,
                           delta,
                           event.GetX(), event.GetY() );
  //GL_Swap( frame );
}

/*----------------------------------------------------------------------
  Class:  AmayaCanvas
  Method:  OnMouseUp
  Description:  handle mouse button up events
  -----------------------------------------------------------------------*/
void AmayaCanvas::OnMouseUp( wxMouseEvent& event )
{
  int frame = m_pAmayaFrame->GetFrameId();

  int thot_mod_mask = THOT_NO_MOD;
  if (event.ControlDown())
    thot_mod_mask |= THOT_MOD_CTRL;
  if (event.AltDown())
    thot_mod_mask |= THOT_MOD_ALT;
  if (event.ShiftDown())
    thot_mod_mask |= THOT_MOD_SHIFT;

  TTALOGDEBUG_0( TTA_LOG_DRAW, _T("AmayaCanvas - wxEVT_LEFT_UP || wxEVT_MIDDLE_UP || wxEVT_RIGHT_UP") );
  if (m_MouseGrab)
    {
      m_MouseGrab = false;
      ReleaseMouse();
    }
  if (m_IsMouseSelecting)
    {
      m_IsMouseSelecting = false;
      m_MouseMoveTimer.Stop();
  
      if (!FrameButtonUpCallback( frame, event.GetButton(),
                             thot_mod_mask,
                                 event.GetX(), event.GetY() ))
        event.Skip(false);
      else
        event.Skip();
      // force the focus when clicking on the canvas because the focus is locked on panel buttons
      TtaRedirectFocus();
    }
  else
    event.Skip();
}

/*----------------------------------------------------------------------
  Class:  AmayaCanvas
  Method:  OnMouseDown
  Description:  handle mouse button down events
  -----------------------------------------------------------------------*/
void AmayaCanvas::OnMouseDown( wxMouseEvent& event )
{
  int thot_mod_mask = THOT_NO_MOD;

  m_IsMouseSelecting = true;
  m_MouseMoveTimer.Stop();
  // force the mouse release

  int frame = m_pAmayaFrame->GetFrameId();
  TTALOGDEBUG_1( TTA_LOG_DIALOG, _T("AmayaCanvas::OnMouseDown : frame=%d"), frame );
#ifdef _MACOS
  if (!event.CmdDown())
#endif /* _MACOS */
  if (event.ControlDown())
    thot_mod_mask |= THOT_MOD_CTRL;
  if (event.AltDown())
    thot_mod_mask |= THOT_MOD_ALT;
  if (event.ShiftDown())
    thot_mod_mask |= THOT_MOD_SHIFT;

  m_pAmayaFrame->SetActive( TRUE );

#ifdef _MACOS
  if (event.CmdDown() && event.GetButton() == THOT_LEFT_BUTTON)
     FrameButtonDownCallback( frame,
                              THOT_RIGHT_BUTTON,
                              thot_mod_mask,
                              event.GetX(), event.GetY() );
   else
#endif /* _MACOS */
  FrameButtonDownCallback( frame,
                           event.GetButton(),
                           thot_mod_mask,
                           event.GetX(), event.GetY() );

#if !defined (_MACOS)
  if (!(event.GetButton() == THOT_LEFT_BUTTON &&
	((thot_mod_mask & THOT_MOD_CTRL) == THOT_MOD_CTRL)))
    {	
      // force the focus when clicking on the canvas because the focus is locked on panel buttons
      TtaRedirectFocus();
      event.Skip();
    }
#else /* !_MACOS */
  // force the focus when clicking on the canvas because the focus is locked on panel buttons
  TtaRedirectFocus();
  event.Skip();
#endif /* _LINUX */

}

/*----------------------------------------------------------------------
  Class:  AmayaCanvas
  Method:  OnIdle
  Description:  call GL_DrawAll to draw opengl stuff (used for animations)
                this is called everytime the system is idle
  -----------------------------------------------------------------------*/
void AmayaCanvas::OnIdle( wxIdleEvent& event )
{
  // idle events are no more used for animation. 
  // animation is managed by a timer
#if 0
  // Do not treat this event if the canvas is not active (hidden)
  if (!IsParentFrameActive())
    {
      event.Skip();
      return;
    }

#ifdef _GL
  GL_DrawAll();
#endif /* _GL */
#endif /* 0 */
  event.Skip();
}
/*----------------------------------------------------------------------
  Class:  AmayaCanvas
  Method:  Init
  Description:  initialize the opengl canvas only once !
                this is called when the first OnPaint method is called
  -----------------------------------------------------------------------*/
void AmayaCanvas::Init()
{
  // do not initialize twice
  if (m_Init)
    return;
  m_Init = true;

  TTALOGDEBUG_1( TTA_LOG_DRAW, _T("AmayaCanvas::Init (init opengl canvas) : frame=%d"), m_pAmayaFrame->GetFrameId() );
  TTALOGDEBUG_3( TTA_LOG_DRAW, _T("AmayaCanvas::Init - frame=%d w=%d h=%d"),
                 m_pAmayaFrame->GetFrameId(),
                 GetSize().GetWidth(),
                 GetSize().GetHeight() );

#ifdef _GL
  SetCurrent();
  SetGlPipelineState ();
#endif /* _GL */

  /* 
  // now the frame is initialized, show it
  int frame_id     = m_pAmayaFrame->GetFrameId();
  int view         = FrameTable[frame_id].FrView;
  PtrDocument pDoc = LoadedDocument[FrameTable[frame_id].FrDoc - 1];;
  ShowFrameData( frame_id, pDoc, view );
  */
  
  // simulate a size event to refresh the canvas 
  wxSizeEvent event( GetSize() );
  wxPostEvent(this, event );
}



/*----------------------------------------------------------------------
  Class:  AmayaCanvas
  Method:  IsInit
  Description:  test if the opengl canvas is initialized or not
                (if the canvas is not ini each opengl command which are
                sent will be ignored)
  -----------------------------------------------------------------------*/
bool AmayaCanvas::IsInit()
{
  return m_Init;
}

/*----------------------------------------------------------------------
  Class:  AmayaCanvas
  Method:  OnChar
  Description:  manage keyboard events, should propagate upward
  -----------------------------------------------------------------------*/
void AmayaCanvas::OnChar(wxKeyEvent& event)
{
  event.ResumePropagation(wxEVENT_PROPAGATE_MAX);
  event.Skip();
}

/*----------------------------------------------------------------------
  Class:  AmayaCanvas
  Method:  OnEraseBackground
  Description:  
  -----------------------------------------------------------------------*/
void AmayaCanvas::OnEraseBackground(wxEraseEvent& WXUNUSED(event))
{
  // Do nothing, to avoid flashing.
  TTALOGDEBUG_1( TTA_LOG_DRAW, _T("AmayaCanvas::OnEraseBackground frame=%d"), m_pAmayaFrame->GetFrameId() );
}



/*----------------------------------------------------------------------
  Class:  AmayaCanvas
  Method:  OnContextMenu
  Description:  
 -----------------------------------------------------------------------*/
void AmayaCanvas::OnContextMenu( wxContextMenuEvent & event )
{
  TTALOGDEBUG_2( TTA_LOG_DIALOG, _T("AmayaCanvas::OnContextMenu - (x,y)=(%d,%d)"),
                 event.GetPosition().x,
                 event.GetPosition().y );

  AmayaWindow* wind = wxDynamicCast(wxGetTopLevelParent(this), AmayaWindow);
  
  int      window_id = wind->GetWindowId();
  int      page_id   = 0;
  wxPoint  point     = event.GetPosition();
  Document document;
  View     view;
  FrameToView (m_pAmayaFrame->GetFrameId(), &document, &view);
  
  TTALOGDEBUG_1( TTA_LOG_DIALOG, _T("AmayaCanvas::OnContextMenu - page_id=%d"), page_id);

  if (page_id >= 0 && document)
    TtaPopupDocContextMenu(document, view, window_id, this, point.x, point.y);
  event.Skip();
}

/*----------------------------------------------------------------------
   This is where the event table is declared
   the callbacks are assigned to an event type
 *----------------------------------------------------------------------*/
#ifdef _GL
BEGIN_EVENT_TABLE(AmayaCanvas, wxGLCanvas)
#else // #ifdef _GL
  BEGIN_EVENT_TABLE(AmayaCanvas, wxPanel)
#endif // #ifdef _GL
  EVT_SIZE(AmayaCanvas::OnSize )
  EVT_PAINT(AmayaCanvas::OnPaint )
  EVT_ERASE_BACKGROUND(AmayaCanvas::OnEraseBackground)

  // what mouse event type is managed ? comment what is not managed
  EVT_LEFT_DOWN(AmayaCanvas::OnMouseDown) // Process a wxEVT_LEFT_DOWN event. 
  EVT_LEFT_UP(AmayaCanvas::OnMouseUp) // Process a wxEVT_LEFT_UP event. 
  EVT_LEFT_DCLICK(AmayaCanvas::OnMouseDbClick) // Process a wxEVT_LEFT_DCLICK event. 
  EVT_MIDDLE_DOWN(AmayaCanvas::OnMouseDown) // Process a wxEVT_MIDDLE_DOWN event. 
  EVT_MIDDLE_UP(AmayaCanvas::OnMouseUp) // Process a wxEVT_MIDDLE_UP event. 
  EVT_MIDDLE_DCLICK(AmayaCanvas::OnMouseDbClick) // Process a wxEVT_MIDDLE_DCLICK event. 
  EVT_RIGHT_DOWN(AmayaCanvas::OnMouseDown) // Process a wxEVT_RIGHT_DOWN event. 
  EVT_RIGHT_UP(AmayaCanvas::OnMouseUp) // Process a wxEVT_RIGHT_UP event. 
  EVT_RIGHT_DCLICK(AmayaCanvas::OnMouseDbClick) // Process a wxEVT_RIGHT_DCLICK event. 
  EVT_MOTION(AmayaCanvas::OnMouseMove) // Process a wxEVT_MOTION event. 
  //EVT_ENTER_WINDOW(AmayaCanvas::OnMouseMove) // Process a wxEVT_MOTION event. 
  //EVT_LEAVE_WINDOW(AmayaCanvas::OnMouseMove) // Process a wxEVT_MOTION event. 
  EVT_MOUSEWHEEL(AmayaCanvas::OnMouseWheel) // Process a wxEVT_MOUSEWHEEL event. 

  EVT_IDLE(AmayaCanvas::OnIdle) // Process a wxEVT_IDLE event
  EVT_TIMER( -1,AmayaCanvas::OnTimerMouseMove)
  EVT_CONTEXT_MENU(AmayaCanvas::OnContextMenu)
  
  //   EVT_CHAR(AmayaCanvas::OnChar )
END_EVENT_TABLE()

#endif // #ifdef _WX
