#ifdef _WX

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
				wxWindowID id,
				long style )
  :  wxScrollBar( p_parent_window, id, wxDefaultPosition, wxDefaultSize, style )
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
  //event.Skip();
}



/*----------------------------------------------------------------------
 *  this is where the event table is declared
 *  the callbacks are assigned to an event type
 *----------------------------------------------------------------------*/
BEGIN_EVENT_TABLE(AmayaScrollBar, wxScrollBar)
  EVT_SET_FOCUS( AmayaScrollBar::OnSetFocus )
  EVT_KILL_FOCUS( AmayaScrollBar::OnKillFocus )
  //  EVT_SCROLL_THUMBTRACK(    AmayaFrame::OnScrollLineUp )
  //  EVT_SCROLL_ENDSCROLL(     AmayaFrame::OnScrollLineDown )
  //  EVT_SCROLL( 		AmayaFrame::OnScroll ) // all scroll events
END_EVENT_TABLE()

#endif // #ifdef _WX
