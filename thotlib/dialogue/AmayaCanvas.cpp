#ifdef _WX

#include "wx/wx.h"
#include "wx/button.h"
#include "wx/string.h"

#include "thot_gui.h"
#include "thot_sys.h"
#include "constmedia.h"

#include "AmayaApp.h"
#include "AmayaCanvas.h"
#include "AmayaFrame.h"
#include "AmayaPage.h"
#include "AmayaWindow.h"

#include "typemedia.h"
#include "appdialogue.h"
#include "dialog.h"
#include "application.h"
#include "dialog.h"
#include "document.h"
#include "message.h"
#include "libmsg.h"
#include "thot_key.h"
#include "frame.h"
#include "appdialogue_wx.h"

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

#ifdef _GL
  IMPLEMENT_DYNAMIC_CLASS(AmayaCanvas, wxGLCanvas)
#else /* _GL*/
  IMPLEMENT_DYNAMIC_CLASS(AmayaCanvas, wxPanel)
#endif /* _GL */
/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaCanvas
 *      Method:  AmayaCanvas
 * Description:  construct the canvas : its a wxGLCanvas if opengl is used or a wxPanel if not
 *--------------------------------------------------------------------------------------
 */

#ifdef _GL
AmayaCanvas::AmayaCanvas( wxWindow * p_parent_window,
			  AmayaFrame * p_parent_frame,
			  wxGLContext * p_shared_context )
  : wxGLCanvas( p_parent_window,
		p_shared_context,
		-1,
		wxDefaultPosition, wxDefaultSize, /*0*/ wxWANTS_CHARS , _T("AmayaCanvas"),
		AmayaApp::GetGL_AttrList() ),
#else // #ifdef _GL  
AmayaCanvas::AmayaCanvas( wxWindow * p_parent_window,
			  AmayaFrame * p_parent_frame )
  : wxPanel( p_parent_window ),
#endif // #ifdef _GL
  m_pAmayaFrame( p_parent_frame ),
    m_Init( false ),
    m_IsMouseSelecting( false )
{
#ifdef FORUMLARY_WIDGET_DEMO
  // demo de comment afficher des widgets dans uen fenetre opengl
  // il faut creer un panel fils du canvas et y mettre un widget fils du panel
  wxWindow * p_panel = new wxPanel( this, -1, wxPoint(100,100), wxSize(50,50) );
  wxWindow * p_button = new wxButton( p_panel, -1, _T("Submit"), wxPoint(0,0) );
  p_panel->SetSize( p_button->GetSize() );
#endif /* FORUMLARY_WIDGET_DEMO */

  // we want this class receives timer events
  m_MouseMoveTimer.SetOwner(this);
  
  SetAutoLayout(TRUE);
  Layout();
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaCanvas
 *      Method:  ~AmayaCanvas
 * Description:  destructor (do nothing)
 *--------------------------------------------------------------------------------------
 */
AmayaCanvas::~AmayaCanvas( )
{
  //  SetEventHandler( new wxEvtHandler() );
  wxLogDebug( _T("AmayaCanvas::~AmayaCanvas(): frame=%d"),
	      m_pAmayaFrame->GetFrameId() );
  /*
  AmayaPage * p_page = m_pAmayaFrame->GetPageParent();
  if (p_page)
    {
      AmayaFrame * p_frame = p_page->GetFrame(1);
      if (p_frame)
	{
	  p_frame->SetCurrent();
	  p_frame->SetActive(TRUE);
	}
	  
    }
    Reparent(NULL);*/

  m_pAmayaFrame = NULL;
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaCanvas
 *      Method:  OnSize
 * Description:  called when the canvas is resized
 *               just call the generic callback to resize and redraw the frame
 *--------------------------------------------------------------------------------------
 */
void AmayaCanvas::OnSize( wxSizeEvent& event )
{
#ifdef _GL
  // this is also necessary to update the context on some platforms
  wxGLCanvas::OnSize(event);
#endif /* _GL */

  // Do not treat this event if the canvas is not active (hiden)
  if (!IsParentFrameActive())
  {
    wxLogDebug(_T("AmayaCanvas::OnSize: frame=%d w=%d h=%d (skip)"),
	       m_pAmayaFrame->GetFrameId(),
	       event.GetSize().GetWidth(),
	       event.GetSize().GetHeight() );
    event.Skip();
    return;
  }
  
  // do not resize while opengl is not initialized
  if (!m_Init)
  {
    wxLogDebug(_T("AmayaCanvas::OnSize: frame=%d w=%d h=%d (skip)"),
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
  // dont use event.GetSize() becaus it returns bad value ...
  new_width = GetClientSize().GetWidth();
  new_height = GetClientSize().GetHeight();
 
  // call the generic callback
  FrameResizedCallback(
    	frame,
    	new_width,
	new_height );
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaCanvas
 *      Method:  OnPaint
 * Description:  called when the canvas needs to be repainted
 *               just call the generic callback to redraw a the frame : FrameExposeCallback
 *--------------------------------------------------------------------------------------
 */
void AmayaCanvas::OnPaint( wxPaintEvent& event )
{
  // Do not treat this event if the canvas is not active (hiden)
  if (!IsParentFrameActive())
  {
    wxLogDebug( _T("AmayaCanvas::OnPaint : frame=%d (skip)"),
		m_pAmayaFrame->GetFrameId() );
    event.Skip();
    return;
  }

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
    wxLogDebug( _T("AmayaCanvas::OnPaint : frame=%d [x=%d, y=%d, w=%d, h=%d]"), m_pAmayaFrame->GetFrameId(), x, y, w, h );
    
    upd ++ ;
  }

  // not necesarry : cf cube.cpp sample
  //  event.Skip();
}


/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaCanvas
 *      Method:  OnMouseDbClick
 * Description:  handle mouse dbclick button events
 *--------------------------------------------------------------------------------------
 */
void AmayaCanvas::OnMouseDbClick( wxMouseEvent& event )
{
  // Do not treat this event if the canvas is not active (hiden)
  if (!IsParentFrameActive())
  {
    wxLogDebug( _T("AmayaCanvas::OnMouse : frame=%d (skip)"),
		m_pAmayaFrame->GetFrameId() );
    event.Skip();
    return;
  }

  int frame = m_pAmayaFrame->GetFrameId();

  int thot_mod_mask = THOT_NO_MOD;
  if (event.ControlDown())
    thot_mod_mask |= THOT_MOD_CTRL;
  if (event.AltDown())
    thot_mod_mask |= THOT_MOD_ALT;
  if (event.ShiftDown())
    thot_mod_mask |= THOT_MOD_SHIFT;

  wxLogDebug( _T("AmayaCanvas - wxEVT_LEFT_DCLICK || wxEVT_MIDDLE_DCLICK || wxEVT_RIGHT_DCLICK") );
  FrameButtonDClickCallback( frame,
			     event.GetButton(),
			     thot_mod_mask,
			     event.GetX(), event.GetY() );

#ifndef _WINDOWS
  // force the focus because on debian woody, the focus do not come in when clicking on the canvas
  SetFocus();
#endif /* _WINDOWS */
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaCanvas
 *      Method:  OnMouseMove
 * Description:  handle mouse move events
 *--------------------------------------------------------------------------------------
 */
void AmayaCanvas::OnMouseMove( wxMouseEvent& event )
{
  if ( m_IsMouseSelecting && !m_MouseMoveTimer.IsRunning() )
    {
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

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaCanvas
 *      Method:  OnTimerMouseMove
 * Description:  handle mouse move events, do not generate a move event each time the mouse move
 *               wait 10ms before generate a move event.
 *--------------------------------------------------------------------------------------
 */
void AmayaCanvas::OnTimerMouseMove( wxTimerEvent& event )
{
  wxLogDebug( _T("AmayaCanvas::OnTimerMouseMove: x=%d y=%d"), m_LastMouseMoveX, m_LastMouseMoveY );

  int frame = m_pAmayaFrame->GetFrameId();
  FrameMotionCallback( frame,
		       m_LastMouseMoveModMask,
		       m_LastMouseMoveX,
		       m_LastMouseMoveY );
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaCanvas
 *      Method:  OnMouseWheel
 * Description:  handle mouse Wheel events
 *--------------------------------------------------------------------------------------
 */
void AmayaCanvas::OnMouseWheel( wxMouseEvent& event )
{
  // Do not treat this event if the canvas is not active (hiden)
  if (!IsParentFrameActive())
  {
    wxLogDebug( _T("AmayaCanvas::OnMouse : frame=%d (skip)"),
		m_pAmayaFrame->GetFrameId() );
    event.Skip();
    return;
  }

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
  
  wxLogDebug( _T("AmayaCanvas - wxEVT_MOUSEWHEEL: frame=%d direction=%s delta=%d"),
	      m_pAmayaFrame->GetFrameId(),
	      direction > 0 ? _T("up") : _T("down"),
	      delta );
  
  FrameMouseWheelCallback( frame,
			   thot_mod_mask,
			   direction,
			   delta,
			   event.GetX(), event.GetY() );
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaCanvas
 *      Method:  OnMouseUp
 * Description:  handle mouse button up events
 *--------------------------------------------------------------------------------------
 */
void AmayaCanvas::OnMouseUp( wxMouseEvent& event )
{
  // Do not treat this event if the canvas is not active (hiden)
  if (!IsParentFrameActive())
  {
    wxLogDebug( _T("AmayaCanvas::OnMouse : frame=%d (skip)"),
		m_pAmayaFrame->GetFrameId() );
    event.Skip();
    return;
  }

  int frame = m_pAmayaFrame->GetFrameId();

  int thot_mod_mask = THOT_NO_MOD;
  if (event.ControlDown())
    thot_mod_mask |= THOT_MOD_CTRL;
  if (event.AltDown())
    thot_mod_mask |= THOT_MOD_ALT;
  if (event.ShiftDown())
    thot_mod_mask |= THOT_MOD_SHIFT;

  wxLogDebug( _T("AmayaCanvas - wxEVT_LEFT_UP || wxEVT_MIDDLE_UP || wxEVT_RIGHT_UP") );

  m_IsMouseSelecting = false;
  m_MouseMoveTimer.Stop();
  
  FrameButtonUpCallback( frame,
			 event.GetButton(),
			 thot_mod_mask,
			 event.GetX(), event.GetY() );

#ifndef _WINDOWS
  // force the focus because on debian woody, the focus do not come in when clicking on the canvas
  SetFocus();
#else
  // force the focus when clicking on the canvas because the focus is locked on panel buttons
  TtaRedirectFocus();
#endif /* _WINDOWS */
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaCanvas
 *      Method:  OnMouseDown
 * Description:  handle mouse button down events
 *--------------------------------------------------------------------------------------
 */
void AmayaCanvas::OnMouseDown( wxMouseEvent& event )
{
  // Do not treat this event if the canvas is not active (hiden)
  if (!IsParentFrameActive())
  {
    wxLogDebug( _T("AmayaCanvas::OnMouse : frame=%d (skip)"),
		m_pAmayaFrame->GetFrameId() );
    event.Skip();
    return;
  }

  m_IsMouseSelecting = true;
  m_MouseMoveTimer.Stop();

  int frame = m_pAmayaFrame->GetFrameId();

  int thot_mod_mask = THOT_NO_MOD;
  if (event.ControlDown())
    thot_mod_mask |= THOT_MOD_CTRL;
  if (event.AltDown())
    thot_mod_mask |= THOT_MOD_ALT;
  if (event.ShiftDown())
    thot_mod_mask |= THOT_MOD_SHIFT;

  m_pAmayaFrame->SetActive( TRUE );

  FrameButtonDownCallback( frame,
			   event.GetButton(),
			   thot_mod_mask,
			   event.GetX(), event.GetY() );

#ifndef _WINDOWS
  // force the focus because on debian woody, the focus do not come in when clicking on the canvas
  SetFocus();
#else
  // force the focus when clicking on the canvas because the focus is locked on panel buttons
  TtaRedirectFocus();
#endif /* _WINDOWS */
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaCanvas
 *      Method:  OnIdle
 * Description:  call GL_DrawAll to draw opengl stuff (used for animations)
 *               this is called everytime the system is idle
 *               TODO y a surrement des choses a optimiser dans le coin 
 *--------------------------------------------------------------------------------------
 */
void AmayaCanvas::OnIdle( wxIdleEvent& event )
{
  // Do not treat this event if the canvas is not active (hiden)
  if (!IsParentFrameActive())
  {
    event.Skip();
    return;
  }

#ifdef _GL
  GL_DrawAll();
#endif /* _GL */

  event.Skip();
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaCanvas
 *      Method:  Init
 * Description:  initialize the opengl canvas only once !
 *               this is called when the first OnPaint method is called
 *--------------------------------------------------------------------------------------
 */
void AmayaCanvas::Init()
{
  // do not initialize twice
  if (m_Init)
    return;
  m_Init = true;

  wxLogDebug( _T("AmayaCanvas::Init (init opengl canvas) : frame=%d"),
      m_pAmayaFrame->GetFrameId() );
  wxLogDebug(_T("AmayaCanvas::Init - frame=%d w=%d h=%d"),
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

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaCanvas
 *      Method:  IsParentFrameActive
 * Description:  test if the parent frame which contains this canvas is active or not
 *               this depends on window type
 *--------------------------------------------------------------------------------------
 */
bool AmayaCanvas::IsParentFrameActive()
{
  if (!m_pAmayaFrame)
    return FALSE;
  
  AmayaWindow * p_window = m_pAmayaFrame->GetWindowParent();
  if (!p_window)
    return false;

  switch( p_window->GetKind() )
    {
    case WXAMAYAWINDOW_SIMPLE:
      {
	// we draw something only when the window is shown (everytime maybe ...)
	return p_window->IsShown();
      }
      break;
    case WXAMAYAWINDOW_NORMAL:
      {
	AmayaPage * p_page = m_pAmayaFrame->GetPageParent();
	if (!p_page)
	  return FALSE;
	// if we are closing the page, continue to draw into because maybe the page has been modified so a dialog is poped up
	// we need to draw the page else a gray page will be shown when the document was modified.
	return (p_page->IsSelected() /*&& !p_page->IsClosed()*/);
      }      
      break;
    }

  return false;
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaCanvas
 *      Method:  IsInit
 * Description:  test if the opengl canvas is initialized or not
 *               (if the canvas is not ini each opengl command which are sent will be ignored)
 *--------------------------------------------------------------------------------------
 */
bool AmayaCanvas::IsInit()
{
  return m_Init;
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaCanvas
 *      Method:  OnChar
 * Description:  manage keyboard events, should propagate upward
 *--------------------------------------------------------------------------------------
 */
void AmayaCanvas::OnChar(wxKeyEvent& event)
{
  event.ResumePropagation(wxEVENT_PROPAGATE_MAX);
  event.Skip();
}

/*----------------------------------------------------------------------
 *  this is where the event table is declared
 *  the callbacks are assigned to an event type
 *----------------------------------------------------------------------*/
#ifdef _GL
BEGIN_EVENT_TABLE(AmayaCanvas, wxGLCanvas)
#else // #ifdef _GL
BEGIN_EVENT_TABLE(AmayaCanvas, wxPanel)
#endif // #ifdef _GL
  EVT_SIZE( 		AmayaCanvas::OnSize )
  EVT_PAINT( 		AmayaCanvas::OnPaint )

  // what mouse event type is managed ? comment what is not managed
  EVT_LEFT_DOWN(	AmayaCanvas::OnMouseDown) // Process a wxEVT_LEFT_DOWN event. 
  EVT_LEFT_UP(		AmayaCanvas::OnMouseUp) // Process a wxEVT_LEFT_UP event. 
  EVT_LEFT_DCLICK(	AmayaCanvas::OnMouseDbClick) // Process a wxEVT_LEFT_DCLICK event. 
  EVT_MIDDLE_DOWN(	AmayaCanvas::OnMouseDown) // Process a wxEVT_MIDDLE_DOWN event. 
  EVT_MIDDLE_UP(	AmayaCanvas::OnMouseUp) // Process a wxEVT_MIDDLE_UP event. 
  EVT_MIDDLE_DCLICK(	AmayaCanvas::OnMouseDbClick) // Process a wxEVT_MIDDLE_DCLICK event. 
  EVT_RIGHT_DOWN(	AmayaCanvas::OnMouseDown) // Process a wxEVT_RIGHT_DOWN event. 
  EVT_RIGHT_UP(		AmayaCanvas::OnMouseUp) // Process a wxEVT_RIGHT_UP event. 
  EVT_RIGHT_DCLICK(	AmayaCanvas::OnMouseDbClick) // Process a wxEVT_RIGHT_DCLICK event. 
  EVT_MOTION(		AmayaCanvas::OnMouseMove) // Process a wxEVT_MOTION event. 
  EVT_MOUSEWHEEL(	AmayaCanvas::OnMouseWheel) // Process a wxEVT_MOUSEWHEEL event. 

  EVT_IDLE(             AmayaCanvas::OnIdle) // Process a wxEVT_IDLE event
  
  EVT_TIMER( -1,        AmayaCanvas::OnTimerMouseMove)

  //   EVT_CHAR( AmayaCanvas::OnChar )
END_EVENT_TABLE()

#endif // #ifdef _WX
