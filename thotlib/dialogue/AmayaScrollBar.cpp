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
#include "frame.h"
#include "thot_key.h"
#include "appdialogue_wx.h"
#ifdef _GL
  #include "glwindowdisplay.h"
#endif /* _GL */

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "frame_tv.h"
#include "select_tv.h"
#include "appli_f.h"
#include "views_f.h"
#include "structselect_f.h"
#include "appdialogue_f.h"
#include "appdialogue_wx_f.h"
#include "input_f.h"


#include "AmayaScrollBar.h"

IMPLEMENT_DYNAMIC_CLASS(AmayaScrollBar, wxScrollBar)

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaScrollBar
 *      Method:  AmayaScrollBar
 * Description:  
 *--------------------------------------------------------------------------------------
 */
AmayaScrollBar::AmayaScrollBar( wxWindow* p_parent_window,
				int parent_frame_id,
				long style )
  :  wxScrollBar( p_parent_window, -1, wxDefaultPosition, wxDefaultSize, style )
     ,m_ParentFrameID(parent_frame_id)
{
  wxLogDebug( _T("AmayaScrollBar::AmayaScrollBar()") );
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaScrollBar
 *      Method:  ~AmayaScrollBar
 * Description:  destructor do nothing for the moment
 *--------------------------------------------------------------------------------------
 */
AmayaScrollBar::~AmayaScrollBar()
{
  wxLogDebug( _T("AmayaScrollBar::~AmayaScrollBar()") );
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaScrollBar
 *      Method:  OnSetFocus
 * Description:  just refuse to get the focus because on windows, the scrollbar is blinking
 *--------------------------------------------------------------------------------------
 */
void AmayaScrollBar::OnSetFocus( wxFocusEvent & event )
{
  wxLogDebug( _T("AmayaScrollBar::OnSetFocus") );
  TtaRedirectFocus();
  // do not skip this event because we want stop focus
  //event.Skip();
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaScrollBar
 *      Method:  OnKillFocus
 * Description:  just refuse to get the focus because on windows, the scrollbar is blinking
 *--------------------------------------------------------------------------------------
 */
void AmayaScrollBar::OnKillFocus( wxFocusEvent & event )
{
  wxLogDebug( _T("AmayaScrollBar::OnKillFocus") );
  // do not skip this event because we want stop focus
  event.Skip();
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaScrollBar
 *      Method:  OnTop
 * Description:  
 *--------------------------------------------------------------------------------------
 */
void AmayaScrollBar::OnTop( wxScrollEvent& event )
{
  wxLogDebug( _T("AmayaScrollBar::OnTop") );
  event.Skip();
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaScrollBar
 *      Method:  OnBottom
 * Description:  
 *--------------------------------------------------------------------------------------
 */
void AmayaScrollBar::OnBottom( wxScrollEvent& event )
{
  wxLogDebug( _T("AmayaScrollBar::OnBottom") );
  event.Skip();
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaScrollBar
 *      Method:  OnLineUp
 * Description:  
 *--------------------------------------------------------------------------------------
 */
void AmayaScrollBar::OnLineUp( wxScrollEvent& event )
{
  wxLogDebug( _T("AmayaScrollBar::OnLineUp") );
  event.Skip();
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaScrollBar
 *      Method:  OnLineDown
 * Description:  
 *--------------------------------------------------------------------------------------
 */
void AmayaScrollBar::OnLineDown( wxScrollEvent& event )
{
  wxLogDebug( _T("AmayaScrollBar::OnLineDown") );
  event.Skip();
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaScrollBar
 *      Method:  OnThumbTrack
 * Description:  
 *--------------------------------------------------------------------------------------
 */
void AmayaScrollBar::OnThumbTrack( wxScrollEvent& event )
{
  wxLogDebug( _T("AmayaScrollBar::OnThumbTrack") );
  event.Skip();
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaScrollBar
 *      Method:  OnThumbRelease
 * Description:  
 *--------------------------------------------------------------------------------------
 */
void AmayaScrollBar::OnThumbRelease( wxScrollEvent& event )
{
  wxLogDebug( _T("AmayaScrollBar::OnThumbRelease") );
  event.Skip();
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaScrollBar
 *      Method:  OnScroll
 * Description:  
 *--------------------------------------------------------------------------------------
 */
void AmayaScrollBar::OnScroll( wxScrollEvent& event )
{
  wxLogDebug( _T("AmayaScrollBar::OnScroll") );

  if (event.GetOrientation() == wxHORIZONTAL)
   {
     FrameHScrolledCallback(
	m_ParentFrameID,
	event.GetPosition(),
	GetPageSize() );
     /* now repaint the canvas because wxWidgets is not able to know himself that the canvas has changed */
     GL_DrawAll();
   }
  else if (event.GetOrientation() == wxVERTICAL)
   {
     FrameVScrolledCallback(
	m_ParentFrameID,
	event.GetPosition() );
     /* now repaint the canvas because wxWidgets is not able to know himself that the canvas has changed */
     GL_DrawAll();
   }

  //  event.Skip();
}


/*----------------------------------------------------------------------
 *  this is where the event table is declared
 *  the callbacks are assigned to an event type
 *----------------------------------------------------------------------*/
BEGIN_EVENT_TABLE(AmayaScrollBar, wxScrollBar)
  EVT_SCROLL( AmayaScrollBar::OnScroll )
  EVT_SCROLL_TOP( AmayaScrollBar::OnTop ) /* Process wxEVT_SCROLL_TOP scroll-to-top events (minium position). */
  EVT_SCROLL_BOTTOM( AmayaScrollBar::OnBottom ) /* Process wxEVT_SCROLL_TOP scroll-to-bottom events (maximum position). */
  EVT_SCROLL_LINEUP( AmayaScrollBar::OnLineUp ) /* Process wxEVT_SCROLL_LINEUP line up events. */
  EVT_SCROLL_LINEDOWN( AmayaScrollBar::OnLineDown )
#if 0
  EVT_SCROLL_THUMBTRACK( AmayaScrollBar::OnThumbTrack ) /* Process wxEVT_SCROLL_THUMBTRACK thumbtrack events
							 (frequent events sent as the user drags the thumbtrack). */
  EVT_SCROLL_THUMBRELEASE( AmayaScrollBar::OnThumbRelease ) /* Process wxEVT_SCROLL_THUMBRELEASE thumb release events.*/
#endif /* 0 */

  EVT_SET_FOCUS( AmayaScrollBar::OnSetFocus )
  EVT_KILL_FOCUS( AmayaScrollBar::OnKillFocus )
  //  EVT_SCROLL_THUMBTRACK(    AmayaFrame::OnScrollLineUp )
  //  EVT_SCROLL_ENDSCROLL(     AmayaFrame::OnScrollLineDown )
  //  EVT_SCROLL( 		AmayaFrame::OnScroll ) // all scroll events
END_EVENT_TABLE()

#endif // #ifdef _WX
