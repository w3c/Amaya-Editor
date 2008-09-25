/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2008
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

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
#include "logdebug.h"

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
#include "displayview_f.h"
#include "absboxes_f.h"

#include "AmayaScrollBar.h"

IMPLEMENT_DYNAMIC_CLASS(AmayaScrollBar, wxScrollBar)

/*----------------------------------------------------------------------
 *       Class:  AmayaScrollBar
 *      Method:  AmayaScrollBar
 * Description:  
  -----------------------------------------------------------------------*/
AmayaScrollBar::AmayaScrollBar( wxWindow* p_parent_window,
				int parent_frame_id,
				long style )
  :  wxScrollBar( p_parent_window, -1, wxDefaultPosition, wxDefaultSize, style )
     ,m_ParentFrameID(parent_frame_id)
     ,m_IgnoreNextScrollEvent(FALSE)
{
}

/*----------------------------------------------------------------------
 *       Class:  AmayaScrollBar
 *      Method:  ~AmayaScrollBar
 * Description:  destructor do nothing for the moment
  -----------------------------------------------------------------------*/
AmayaScrollBar::~AmayaScrollBar()
{
}

/*----------------------------------------------------------------------
 *       Class:  AmayaScrollBar
 *      Method:  OnSetFocus
 * Description:  just refuse to get the focus because on windows, the scrollbar is blinking
  -----------------------------------------------------------------------*/
void AmayaScrollBar::OnSetFocus( wxFocusEvent & event )
{
  TtaRedirectFocus();
  // do not skip this event because we want stop focus
  //event.Skip();
}

/*----------------------------------------------------------------------
 *       Class:  AmayaScrollBar
 *      Method:  OnKillFocus
 * Description:  just refuse to get the focus because on windows, the scrollbar is blinking
  -----------------------------------------------------------------------*/
void AmayaScrollBar::OnKillFocus( wxFocusEvent & event )
{
  // do not skip this event because we want stop focus
  event.Skip();
}

/*----------------------------------------------------------------------
 *       Class:  AmayaScrollBar
 *      Method:  OnTop
 * Description:  
  -----------------------------------------------------------------------*/
void AmayaScrollBar::OnTop( wxScrollEvent& event )
{
  TTALOGDEBUG_0( TTA_LOG_DIALOG, _T("AmayaScrollBar::OnTop") );

  if (event.GetOrientation() == wxVERTICAL)
    {
      JumpIntoView (m_ParentFrameID, 0);
    }
}

/*----------------------------------------------------------------------
 *       Class:  AmayaScrollBar
 *      Method:  OnBottom
 * Description:  
  -----------------------------------------------------------------------*/
void AmayaScrollBar::OnBottom( wxScrollEvent& event )
{
  TTALOGDEBUG_0( TTA_LOG_DIALOG, _T("AmayaScrollBar::OnBottom") );

  if (event.GetOrientation() == wxVERTICAL)
    {
      JumpIntoView (m_ParentFrameID, 100);
    }
}

/*----------------------------------------------------------------------
 *       Class:  AmayaScrollBar
 *      Method:  OnLineUp
 * Description:  
  -----------------------------------------------------------------------*/
void AmayaScrollBar::OnLineUp( wxScrollEvent& event )
{
  Document doc; 
  View     view;

  FrameToView( m_ParentFrameID, &doc, &view );
  if (event.GetOrientation() == wxVERTICAL)
      TtcLineUp (doc, view); 
  else
      TtcScrollLeft(doc, view);

  /* this flag is necessary because 2 events occure when up/down button
     is pressed (it's an optimisation)
     this hack works because OnLineDown is called before OnScroll,
     but becareful the events orders could change in future wxWidgets
     releases or can be platform specific
  */
  m_IgnoreNextScrollEvent = TRUE;
}

/*----------------------------------------------------------------------
 *       Class:  AmayaScrollBar
 *      Method:  OnLineDown
 * Description:  
  -----------------------------------------------------------------------*/
void AmayaScrollBar::OnLineDown( wxScrollEvent& event )
{
  Document doc; 
  View     view;

  FrameToView( m_ParentFrameID, &doc, &view );
  if (event.GetOrientation() == wxVERTICAL)
    TtcLineDown (doc, view);
  else
    TtcScrollRight(doc, view);
  
  /* this flag is necessary because 2 events occure when up/down button
     is pressed (it's an optimisation)
     this hack works because OnLineDown is called before OnScroll,
     but becareful the events orders could change in future wxWidgets
     releases or can be platform specific
  */
  m_IgnoreNextScrollEvent = TRUE;
}

/*----------------------------------------------------------------------
 *       Class:  AmayaScrollBar
 *      Method:  OnScrollUp
 * Description:  
  -----------------------------------------------------------------------*/
void AmayaScrollBar::OnScrollUp( wxScrollEvent& event )
{
  Document doc; 
  View     view;

  FrameToView (m_ParentFrameID, &doc, &view);
  if (event.GetOrientation() == wxVERTICAL)
    {
      TtcPageUp (doc, view);
      /* this flag is necessary because 2 events occure when up/down button
         is pressed (it's an optimisation)
         this hack works because OnLineDown is called before OnScroll,
         but becareful the events orders could change in future wxWidgets
         releases or can be platform specific
      */
      m_IgnoreNextScrollEvent = TRUE;
    }
  else
    OnScroll (event);
}

/*----------------------------------------------------------------------
 *       Class:  AmayaScrollBar
 *      Method:  OnScrollDown
 * Description:  
  -----------------------------------------------------------------------*/
void AmayaScrollBar::OnScrollDown( wxScrollEvent& event )
{
  Document doc; 
  View     view;

  FrameToView (m_ParentFrameID, &doc, &view);
  if (event.GetOrientation() == wxVERTICAL)
    {
      TtcPageDown (doc, view);
      /* this flag is necessary because 2 events occure when up/down button
         is pressed (it's an optimisation)
         this hack works because OnLineDown is called before OnScroll,
         but becareful the events orders could change in future wxWidgets
         releases or can be platform specific
      */
      m_IgnoreNextScrollEvent = TRUE;
    }
  else if (event.GetOrientation() == wxVERTICAL)
    OnScroll (event);
}

/*----------------------------------------------------------------------
 *       Class:  AmayaScrollBar
 *      Method:  OnScroll
 * Description:  
  -----------------------------------------------------------------------*/
void AmayaScrollBar::OnScroll( wxScrollEvent& event )
{
  /* this flag is necessary because 2 events occure when up/down button
     is pressed (it's an optimisation)
     this hack works because OnLineDown is called before OnScroll,
     but becareful the events orders could change in future wxWidgets
     releases or can be platform specific
  */
  if (m_IgnoreNextScrollEvent)
    {
      m_IgnoreNextScrollEvent = FALSE;
      event.Skip();
      return;
    }
  
  if (event.GetOrientation() == wxHORIZONTAL)
    {
      TTALOGDEBUG_3( TTA_LOG_DIALOG, _T("AmayaScrollBar::OnScroll [wxHORIZONTAL][frameid=%d][pos=%d][pagesize=%d]"), m_ParentFrameID, event.GetPosition(), GetPageSize() );
      FrameHScrolledCallback( m_ParentFrameID,
                              event.GetPosition(),
                              GetPageSize() );
    }
  else if (event.GetOrientation() == wxVERTICAL)
    {
      TTALOGDEBUG_3( TTA_LOG_DIALOG, _T("AmayaScrollBar::OnScroll [wxVERTICAL][frameid=%d][pos=%d][pagesize=%d]"), m_ParentFrameID, event.GetPosition(), GetPageSize() );
      FrameVScrolledCallback( m_ParentFrameID,
                              event.GetPosition() );
    }
}

/*----------------------------------------------------------------------
 *  this is where the event table is declared
 *  the callbacks are assigned to an event type
 *----------------------------------------------------------------------*/
BEGIN_EVENT_TABLE(AmayaScrollBar, wxScrollBar)
  EVT_SCROLL_TOP( AmayaScrollBar::OnTop ) /* wxEVT_SCROLL_TOP (minium position) */
  EVT_SCROLL_BOTTOM( AmayaScrollBar::OnBottom ) /* wxEVT_SCROLL_TOP (maximum position) */
  EVT_SCROLL_LINEUP( AmayaScrollBar::OnLineUp ) /* wxEVT_SCROLL_LINEUP */
  EVT_SCROLL_LINEDOWN( AmayaScrollBar::OnLineDown )
  EVT_SCROLL_PAGEUP( AmayaScrollBar::OnScrollUp )
  EVT_SCROLL_PAGEDOWN( AmayaScrollBar::OnScrollDown )
  EVT_SCROLL_THUMBTRACK( AmayaScrollBar::OnScroll ) /* wxEVT_SCROLL_THUMBTRACK (frequent events sent as the user drags the thumbtrack). */
  EVT_SCROLL_THUMBRELEASE( AmayaScrollBar::OnScroll ) /* wxEVT_SCROLL_THUMBRELEASE thumb release events.*/

  EVT_SCROLL_CHANGED( AmayaScrollBar::OnScroll )
  EVT_SET_FOCUS( AmayaScrollBar::OnSetFocus )
  EVT_KILL_FOCUS( AmayaScrollBar::OnKillFocus )
  //  EVT_SCROLL_THUMBTRACK(    AmayaFrame::OnScrollLineUp )
  //  EVT_SCROLL_ENDSCROLL(     AmayaFrame::OnScrollLineDown )
  //  EVT_SCROLL( 		AmayaFrame::OnScroll ) // all scroll events
END_EVENT_TABLE()

#endif // #ifdef _WX
