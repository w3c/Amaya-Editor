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
#include "appli_f.h"
#include "applicationapi_f.h"
#include "boxlocate_f.h"
#include "displayview_f.h"
#include "frame_tv.h"
#include "scroll_f.h"
#include "input_f.h"

#ifdef _GL
  #include "glwindowdisplay.h"
#endif /*_GL*/

#include "AmayaCanvas.h"
#include "AmayaFrame.h"

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaCanvas
 *      Method:  AmayaCanvas
 * Description:  construct the canvas : its a wxGLCanvas if opengl is used or a wxPanel if not
 *--------------------------------------------------------------------------------------
 */
AmayaCanvas::AmayaCanvas( AmayaFrame * p_parent_window )
#ifdef _GL
  : wxGLCanvas( (wxWindow*)p_parent_window, (wxGLCanvas*)NULL, -1, wxDefaultPosition, wxSize(200,200) ),
#else // #ifdef _GL  
  : wxPanel( p_parent_window ),
#endif // #ifdef _GL
  m_pAmayaFrame( p_parent_window ),
  m_Init( false )
{
  m_Selecting = FALSE;
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
  wxLogDebug(_T("AmayaCanvas - OnSize: w=%d h=%d"),
	event.GetSize().GetWidth(),
	event.GetSize().GetHeight() );
  
  // get the current frame id
  int frame = m_pAmayaFrame->GetFrameId();

  // get the new dimensions  
  int new_width, new_height;
  new_width = event.GetSize().GetWidth();
  new_height = event.GetSize().GetHeight();

  // a virer qd on aura fixer le prb du premier resize ...
  if (new_width<200 && new_height<200) 
  {
    event.Skip();
    return;
  }

  // call the generic callback
  FrameResizedCallback(
    	frame,
    	new_width,
	new_height );

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
  wxLogDebug( _T("AmayaCanvas - OnPaint") ); 

  /*
   * Note that In a paint event handler, the application must
   * always create a wxPaintDC object, even if you do not use it.
   * Otherwise, under MS Windows, refreshing for this and
   * other windows will go wrong.
   */
  wxPaintDC dc(this);

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
  int frame = m_pAmayaFrame->GetFrameId();

  int thot_mod_mask = THOT_NO_MOD;
  if (event.ControlDown())
    thot_mod_mask |= THOT_MOD_CTRL;
  if (event.AltDown())
    thot_mod_mask |= THOT_MOD_ALT;
  if (event.ShiftDown())
    thot_mod_mask |= THOT_MOD_SHIFT;
 
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

  // ENTER WINDOW
  if ( event.GetEventType() == wxEVT_ENTER_WINDOW )    
    wxLogDebug( _T("AmayaCanvas - wxEVT_ENTER_WINDOW") );

  // LEAVE WINDOW  
  if ( event.GetEventType() == wxEVT_LEAVE_WINDOW )    
    wxLogDebug( _T("AmayaCanvas - wxEVT_LEAVE_WINDOW") );

  // MOUSE WHEEL  
  if ( event.GetEventType() == wxEVT_MOUSEWHEEL )  
  {
    int direction = event.GetWheelRotation();
    int delta     = event.GetWheelDelta();

    wxLogDebug( _T("AmayaCanvas - wxEVT_MOUSEWHEEL: direction=%s delta=%d"),
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
  wxLogDebug( _T("AmayaCanvas - OnChar = %x"), event.GetKeyCode() );

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

  // Call the generic function for key events management
  ThotInput (frame, thot_keysym, 0, thot_mask, thot_keysym);

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
  wxLogDebug( _T("AmayaCanvas - OnKeyDown = %x"), event.GetKeyCode() );
  
  bool skip = TRUE; // by default forward this event (should generate OnChar event)
  int keycode =  event.GetKeyCode();
  
  // test if the key is not a char (special)
  if ( keycode == WXK_F2 	||
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
       keycode == WXK_F16 	||
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
#if 0
	  if (wParam >= 48 && wParam <= 57)
	    {
	      /* handling Ctrl 0-9 or Alt 0-9 */
	      key_menu = GetKeyState (VK_MENU);
  	      key = GetKeyState (VK_CONTROL);
	      /* is it an Alt-GR modifier? (WIN32 interprets this
		  as having both a control + menu key pressed down) */
	      if (HIBYTE (key_menu) && HIBYTE (key))
		    done = TRUE;
	      /* is a control key pressed? */
	      if (HIBYTE (key))
 		    isSpecial = FALSE;
	      else
		  {
		    /* is a menu key pressed? */
		    if (HIBYTE (key_menu))
		       isSpecial = FALSE;
		    else
		       /* don't handle a simple 0-9 */
		       done = TRUE;
		  }
	    }
	  else
	    isSpecial = TRUE;
#endif
  } 
    if (!skip)
    {
//      key = (int) wParam;
//      done = WIN_CharTranslation (FrRef[frame], frame, mMsg, (WPARAM) key,
//				  lParam, isSpecial);
      
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

      // Call the generic function for key events management
      ThotInput (frame, thot_keysym, 0, thot_mask, thot_keysym);

    }
  

  event.Skip();
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaCanvas
 *      Method:  OnInit
 * Description:  TODO n'est jamais appele ...
 *--------------------------------------------------------------------------------------
 */
void AmayaCanvas::OnInit( wxInitDialogEvent& event )
{
  wxLogDebug( _T("AmayaCanvas - OnInit") );
  event.Skip();
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaCanvas
 *      Method:  OnActivate
 * Description:  TODO n'est jamais appele ...
 *--------------------------------------------------------------------------------------
 */
void AmayaCanvas::OnActivate( wxActivateEvent& event )
{
  wxLogDebug( 	_T("AmayaCanvas - OnActivate: %s"),
      		event.GetActive() ? _T("yes") : _T("no") );
  event.Skip();
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaCanvas
 *      Method:  OnClose
 * Description:  rien a faire ...
 *--------------------------------------------------------------------------------------
 */
void AmayaCanvas::OnClose( wxCloseEvent& event )
{
  wxLogDebug( _T("AmayaCanvas - OnClose: %s") );
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
  if (m_Init) return;
  m_Init = true;
  
  SetCurrent();
  SetGlPipelineState ();
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
  
  EVT_INIT_DIALOG(	AmayaCanvas::OnInit ) // canvas creation
  EVT_ACTIVATE(		AmayaCanvas::OnActivate )
  EVT_CLOSE(		AmayaCanvas::OnClose )
  
END_EVENT_TABLE()

#endif // #ifdef _WX
