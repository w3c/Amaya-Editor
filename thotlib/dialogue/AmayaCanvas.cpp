#ifdef _WX

#include "wx/wx.h"

#include "thot_gui.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "appdialogue.h"
#include "dialog.h"
#include "application.h"
#include "dialog.h"
#include "document.h"
#include "message.h"
#include "libmsg.h"
#include "thot_key.h"

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

#include "AmayaCanvas.h"
#include "AmayaFrame.h"
#include "AmayaPage.h"

/*
  WX_GL_RGBA 	        Use true colour
  WX_GL_BUFFER_SIZE 	Bits for buffer if not WX_GL_RGBA
  WX_GL_LEVEL 	        0 for main buffer, >0 for overlay, <0 for underlay
  WX_GL_DOUBLEBUFFER 	Use doublebuffer
  WX_GL_STEREO 	        Use stereoscopic display
  WX_GL_AUX_BUFFERS 	Number of auxiliary buffers (not all implementation support this option)
  WX_GL_MIN_RED 	Use red buffer with most bits (> MIN_RED bits)
  WX_GL_MIN_GREEN 	Use green buffer with most bits (> MIN_GREEN bits)
  WX_GL_MIN_BLUE 	Use blue buffer with most bits (> MIN_BLUE bits)
  WX_GL_MIN_ALPHA 	Use alpha buffer with most bits (> MIN_ALPHA bits)
  WX_GL_DEPTH_SIZE 	Bits for Z-buffer (0,16,32)
  WX_GL_STENCIL_SIZE 	Bits for stencil buffer
  WX_GL_MIN_ACCUM_RED 	Use red accum buffer with most bits (> MIN_ACCUM_RED bits)
  WX_GL_MIN_ACCUM_GREEN Use green buffer with most bits (> MIN_ACCUM_GREEN bits)
  WX_GL_MIN_ACCUM_BLUE 	Use blue buffer with most bits (> MIN_ACCUM_BLUE bits)
  WX_GL_MIN_ACCUM_ALPHA Use blue buffer with most bits (> MIN_ACCUM_ALPHA bits
*/
int AmayaCanvas::AttrList[] =
{
  WX_GL_RGBA,
  WX_GL_MIN_RED, 1,
  WX_GL_MIN_GREEN , 1,
  WX_GL_MIN_BLUE, 1,
  WX_GL_MIN_ALPHA, 1, /* don't change the position of the entry (8) */
  WX_GL_STENCIL_SIZE, 1,
  WX_GL_DOUBLEBUFFER,
  0
};

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
AmayaCanvas::AmayaCanvas( AmayaFrame * p_parent_window,
			  AmayaCanvas * p_shared_canvas )
#ifdef _GL
  : wxGLCanvas( wxDynamicCast(p_parent_window, wxWindow),
		wxDynamicCast(p_shared_canvas, wxGLCanvas),
		-1,
		wxDefaultPosition, wxDefaultSize, 0, _T("AmayaCanvas"),
		AttrList ),
#else // #ifdef _GL  
  : wxPanel( wxDynamicCast(p_parent_window,wxWindow) ),
#endif // #ifdef _GL
  m_pAmayaFrame( p_parent_window ),
  m_Init( false )
{
  m_Selecting = FALSE;
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
  // Do not treat this event if the canvas is not active (hiden)
  if (!IsParentPageActive())
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
  
  wxLogDebug(_T("AmayaCanvas::OnSize: frame=%d w=%d h=%d"),
        m_pAmayaFrame->GetFrameId(),
	event.GetSize().GetWidth(),
	event.GetSize().GetHeight() );
 
  // get the current frame id
  int frame = m_pAmayaFrame->GetFrameId();

  // get the new dimensions  
  int new_width, new_height;
  new_width = event.GetSize().GetWidth();
  new_height = event.GetSize().GetHeight();

  // call the generic callback
  FrameResizedCallback(
    	frame,
    	new_width,
	new_height );

  // resize the frame sizer to take into account scrollbar show/hide
  m_pAmayaFrame->Layout();
  Layout();

  //  forward the event to parents
  event.Skip();
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
  /*
   * Note that In a paint event handler, the application must
   * always create a wxPaintDC object, even if you do not use it.
   * Otherwise, under MS Windows, refreshing for this and
   * other windows will go wrong.
   */
  wxPaintDC dc(this);

  // Do not treat this event if the canvas is not active (hiden)
  if (!IsParentPageActive())
  {
    wxLogDebug( _T("AmayaCanvas::OnPaint : frame=%d (skip)"),
		m_pAmayaFrame->GetFrameId() );
    event.Skip();
    return;
  }

  wxLogDebug( _T("AmayaCanvas::OnPaint : frame=%d"),
     m_pAmayaFrame->GetFrameId() );

  // get the current frame id
  int frame = m_pAmayaFrame->GetFrameId();

  // initialize the canvas context
  Init(); 

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
    
    upd ++ ;
  }

  event.Skip();
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaCanvas
 *      Method:  OnMouse
 * Description:  handle mouse events
 *               call generic thot callbacks for mouse events
 *--------------------------------------------------------------------------------------
 */
void AmayaCanvas::OnMouse( wxMouseEvent& event )
{
  // Do not treat this event if the canvas is not active (hiden)
  if (!IsParentPageActive())
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

  // if a click is done into the canvas then activate the frame
  if ( ( event.GetEventType() == wxEVT_LEFT_DOWN ||
	 event.GetEventType() == wxEVT_MIDDLE_DOWN ||
	 event.GetEventType() == wxEVT_RIGHT_DOWN ) ||
       ( event.GetEventType() == wxEVT_LEFT_DCLICK ||
	 event.GetEventType() == wxEVT_MIDDLE_DCLICK ||
	 event.GetEventType() == wxEVT_RIGHT_DCLICK ) )
    {
      m_pAmayaFrame->SetActive( TRUE );
    }
 
  // BUTTON DOWN 
  if ( event.GetEventType() == wxEVT_LEFT_DOWN ||
       event.GetEventType() == wxEVT_MIDDLE_DOWN ||
       event.GetEventType() == wxEVT_RIGHT_DOWN )
  {
    wxLogDebug( _T("AmayaCanvas - wxEVT_LEFT_DOWN || wxEVT_MIDDLE_DOWN || wxEVT_RIGHT_DOWN") );      
    
    if ( !FrameButtonDownCallback( 
	    frame,
	    event.GetButton(),
	    thot_mod_mask,
	    event.GetX(), event.GetY() ) )
    {
      return;
    }
  }

  // DOUBLE CLICK
  if ( event.GetEventType() == wxEVT_LEFT_DCLICK ||
       event.GetEventType() == wxEVT_MIDDLE_DCLICK ||
       event.GetEventType() == wxEVT_RIGHT_DCLICK )  
  {
    wxLogDebug( _T("AmayaCanvas - wxEVT_LEFT_DCLICK || wxEVT_MIDDLE_DCLICK || wxEVT_RIGHT_DCLICK") );
    if ( !FrameButtonDClickCallback( 
	    frame,
	    event.GetButton(),
	    thot_mod_mask,
	    event.GetX(), event.GetY() ) )
    {
      return;      
    }	
  }

  // BUTTON UP
  if ( event.GetEventType() == wxEVT_LEFT_UP ||
       event.GetEventType() == wxEVT_MIDDLE_UP ||
       event.GetEventType() == wxEVT_RIGHT_UP )    
  {
    wxLogDebug( _T("AmayaCanvas - wxEVT_LEFT_UP || wxEVT_MIDDLE_UP || wxEVT_RIGHT_UP") );

    if ( !FrameButtonUpCallback( 
	frame,
       	event.GetButton(),
	thot_mod_mask,
	event.GetX(), event.GetY() ) )
    {
      return;      
    }
  }

  // MOUSE HAS MOVED
  if ( event.GetEventType() == wxEVT_MOTION )    
  {
    if ( !FrameMotionCallback( 
	frame,
	thot_mod_mask,
	event.GetX(), event.GetY() ) )
    {
      return;
    }
  }

#if 0
  // ENTER WINDOW
  if ( event.GetEventType() == wxEVT_ENTER_WINDOW )    
    wxLogDebug( _T("AmayaCanvas - wxEVT_ENTER_WINDOW") );

  // LEAVE WINDOW  
  if ( event.GetEventType() == wxEVT_LEAVE_WINDOW )    
    wxLogDebug( _T("AmayaCanvas - wxEVT_LEAVE_WINDOW") );
#endif /* 0 */
  
  // MOUSE WHEEL  
  if ( event.GetEventType() == wxEVT_MOUSEWHEEL )  
  {
    int direction = event.GetWheelRotation();
    int delta     = event.GetWheelDelta();

    wxLogDebug( _T("AmayaCanvas - wxEVT_MOUSEWHEEL: frame=%d direction=%s delta=%d"),
	m_pAmayaFrame->GetFrameId(),
       	direction > 0 ? _T("up") : _T("down"),
	delta );

    if ( !FrameMouseWheelCallback( 
      frame,
      thot_mod_mask,
      direction,
      delta,
      event.GetX(), event.GetY() ) )
    {
      return;
    }
  }
  
  // forward current event to parent widgets
  event.Skip();
} 


/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaCanvas
 *      Method:  OnChar
 * Description:  called when a wxKeyEvent is comming - is called after OnKeyDown
 *               TODO a debuguer ...
 *--------------------------------------------------------------------------------------
 */
void AmayaCanvas::OnChar( wxKeyEvent& event )
{
  // Do not treat this event if the canvas is not active (hiden)
  if (!IsParentPageActive())
  {
    wxLogDebug( _T("AmayaCanvas::OnChar : frame=%d char=%x (skip)"),
		m_pAmayaFrame->GetFrameId(),
		event.GetKeyCode() );

    event.Skip();
    return;
  }

  wxLogDebug( _T("AmayaCanvas::OnChar : frame=%d char=%x"),
      m_pAmayaFrame->GetFrameId(),
      event.GetKeyCode() );
  
  int frame = m_pAmayaFrame->GetFrameId();
  int thot_mask = 0;

  // wxkeycodes are directly mapped to thot keysyms :
  // no need to convert the wxwindows keycodes
  int thot_keysym = event.GetKeyCode();
 
  // convert wx key stats to thot key stats 
  if (event.ControlDown())
    thot_mask |= THOT_MOD_CTRL;
  if (event.AltDown())
    thot_mask |= THOT_MOD_ALT;
  if (event.ShiftDown())
    thot_mask |= THOT_MOD_SHIFT;

  // check if the keycode is a valid char
  wxString s((wxChar)thot_keysym);
  if (s.IsAscii())
    {
      wxLogDebug( _T("IsAscii yes: s=")+s );
      // Call the generic function for key events management
      ThotInput (frame, thot_keysym, 0, thot_mask, thot_keysym);
    }

  event.Skip();
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaCanvas
 *      Method:  OnKeyDown
 * Description:  called when a key is pressed - if event.skip() is not called
 *               the event is not forwarded to OnChar
 *               TODO a debuguer ... et creer une fonction generique
 *--------------------------------------------------------------------------------------
 */
void AmayaCanvas::OnKeyDown( wxKeyEvent& event )
{
  // Do not treat this event if the canvas is not active (hiden)
  if (!IsParentPageActive())
  {
    wxLogDebug( _T("AmayaCanvas::OnKeyDown : frame=%d key=%x (skip)"),
		m_pAmayaFrame->GetFrameId(),
		event.GetKeyCode() );

    event.Skip();
    return;
  }

  wxLogDebug( _T("AmayaCanvas::OnKeyDown : frame=%d key=%x"),
      m_pAmayaFrame->GetFrameId(),
      event.GetKeyCode() );

  bool skip = TRUE; // by default forward this event (should generate OnChar event)
  int keycode =  event.GetKeyCode();
  
  // test if the key is a special one
  // + shortcut : CTRL+XXX ALT+XXX
  // + special key : FXX HOME END ...
  if ( event.ControlDown()      ||
       event.AltDown()          ||
       /*       keycode == WXK_F2 	||
       keycode == WXK_F3 	||
       keycode == WXK_F4 	||
       keycode == WXK_F5 	||
       keycode == WXK_F6 	||
       keycode == WXK_F7 	||
       keycode == WXK_F8 	||
       keycode == WXK_F9 	||
       keycode == WXK_F10 	||
       keycode == WXK_F11 	||
       keycode == WXK_F12 	||
       keycode == WXK_F13 	||
       keycode == WXK_F14 	||
       keycode == WXK_F15 	||
       keycode == WXK_F16 	||*/
       keycode == WXK_INSERT 	||
       keycode == WXK_DELETE 	||
       keycode == WXK_HOME 	||
       keycode == WXK_END 	||
       keycode == WXK_PRIOR 	||
       keycode == WXK_NEXT 	||
       keycode == WXK_LEFT 	||
       keycode == WXK_RIGHT 	||
       keycode == WXK_UP 	||
       keycode == WXK_DOWN )
  {
    skip = FALSE;
  } 

    if (!skip)
    {
      int frame = m_pAmayaFrame->GetFrameId();
      int thot_mask = 0;

      // wxkeycodes are directly mapped to thot keysyms :
      // no need to convert the wxwindows keycodes
      // todo : verifier que le thot_keysym est ok sur otutes les plateformes
      // Ctrl-A doit generer : thot_mask=1 et thot_keysym=97

      int thot_keysym = event.GetKeyCode();

      // convert wx key stats to thot key stats 
      if (event.ControlDown())
	thot_mask |= THOT_MOD_CTRL;
      if (event.AltDown())
	thot_mask |= THOT_MOD_ALT;
      if (event.ShiftDown())
	thot_mask |= THOT_MOD_SHIFT;

      if (!event.ShiftDown())
	{
	  // shift key was not pressed
	  // force the lowercase
	  wxString s((wxChar)thot_keysym);
	  if (s.IsAscii())
	    {
	      wxLogDebug( _T("IsAscii yes: s=")+s );
	      s.MakeLower();
	      wxChar c = s.GetChar(0);
	      thot_keysym = (int)c;
	    }
	}

      // Call the generic function for key events management
      ThotInput (frame, thot_keysym, 0, thot_mask, thot_keysym);
    }
    else
      event.Skip();
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
  if (!IsParentPageActive())
  {
    event.Skip();
    return;
  }

#if 0  
  wxLogDebug( _T("AmayaCanvas::OnIdle : isactive=%s"),
              IsParentPageActive() ? _T("true") : _T("false") );
#endif /* 0 */

  GL_DrawAll();
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
  
  SetCurrent();
  SetGlPipelineState ();

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
  /*
  // followed by a paint event
  wxPaintEvent event2;
  wxPostEvent(this, event2 );
  // simulate a size event to refresh the canvas 
  wxSizeEvent event3( GetSize() );
  wxPostEvent(this, event3 );
  // followed by a paint event
  wxPaintEvent event4;
  wxPostEvent(this, event4 );
  // simulate a size event to refresh the canvas 
  wxSizeEvent event5( GetSize() );
  wxPostEvent(this, event5 );
  */
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaCanvas
 *      Method:  IsParentPageActive
 * Description:  test if the page which contains this canvas is selected or not
 *               maybe it needs to be optimised : remove some test ...
 *--------------------------------------------------------------------------------------
 */
bool AmayaCanvas::IsParentPageActive()
{
  if (!m_pAmayaFrame)
    return FALSE;

  AmayaPage * p_page = m_pAmayaFrame->GetPageParent();
  if (!p_page)
    return FALSE;

  return p_page->IsSelected();
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
  EVT_LEFT_DOWN(	AmayaCanvas::OnMouse) // Process a wxEVT_LEFT_DOWN event. 
  EVT_LEFT_UP(		AmayaCanvas::OnMouse) // Process a wxEVT_LEFT_UP event. 
  EVT_LEFT_DCLICK(	AmayaCanvas::OnMouse) // Process a wxEVT_LEFT_DCLICK event. 
  EVT_MIDDLE_DOWN(	AmayaCanvas::OnMouse) // Process a wxEVT_MIDDLE_DOWN event. 
  EVT_MIDDLE_UP(	AmayaCanvas::OnMouse) // Process a wxEVT_MIDDLE_UP event. 
  EVT_MIDDLE_DCLICK(	AmayaCanvas::OnMouse) // Process a wxEVT_MIDDLE_DCLICK event. 
  EVT_RIGHT_DOWN(	AmayaCanvas::OnMouse) // Process a wxEVT_RIGHT_DOWN event. 
  EVT_RIGHT_UP(		AmayaCanvas::OnMouse) // Process a wxEVT_RIGHT_UP event. 
  EVT_RIGHT_DCLICK(	AmayaCanvas::OnMouse) // Process a wxEVT_RIGHT_DCLICK event. 
  EVT_MOTION(		AmayaCanvas::OnMouse) // Process a wxEVT_MOTION event. 
  EVT_ENTER_WINDOW(	AmayaCanvas::OnMouse) // Process a wxEVT_ENTER_WINDOW event. 
  EVT_LEAVE_WINDOW(	AmayaCanvas::OnMouse) // Process a wxEVT_LEAVE_WINDOW event. 
  EVT_MOUSEWHEEL(	AmayaCanvas::OnMouse) // Process a wxEVT_MOUSEWHEEL event. 
//  EVT_MOUSE_EVENTS(	AmayaCanvas::OnMouse) // Process all mouse events. 
   

  EVT_KEY_DOWN(		AmayaCanvas::OnKeyDown) // Process a wxEVT_KEY_DOWN event (any key has been pressed). 
//  EVT_KEY_UP(		AmayaCanvas::OnChar) // Process a wxEVT_KEY_UP event (any key has been released). 
  EVT_CHAR(		AmayaCanvas::OnChar) // Process a wxEVT_CHAR event. 

  EVT_IDLE(             AmayaCanvas::OnIdle) // Process a wxEVT_IDLE event
  
END_EVENT_TABLE()

#endif // #ifdef _WX
