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

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "appli_f.h"

#include "AmayaFrame.h"
#include "AmayaCanvas.h"

#include "wx/log.h"


/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaFrame
 *      Method:  AmayaFrame
 * Description:  just construct a frame : AmayaCanvas + 2 wxScrollBar into a wxFlexGridSizer
 *--------------------------------------------------------------------------------------
 */
AmayaFrame::AmayaFrame(
                int             frame_id
      	       ,wxWindow *      p_parent_window
	      )
	:  wxPanel( p_parent_window )
          ,m_FrameId( frame_id )
{
  // Create drawing area
  m_pCanvas = new AmayaCanvas( this );
  
  // Create vertical and horizontal scrollbars
  m_pScrollBarH = new wxScrollBar( this,
				   -1,
				   wxDefaultPosition,
				   wxDefaultSize,
				   wxSB_HORIZONTAL );
  m_pScrollBarV = new wxScrollBar( this,
				   -1,
				   wxDefaultPosition,
				   wxDefaultSize,
				   wxSB_VERTICAL );

  // Create a flexible sizer (first col and first row should be extensible)
  m_pFlexSizer = new wxFlexGridSizer(2,2);
  m_pFlexSizer->AddGrowableCol(0);
  m_pFlexSizer->AddGrowableRow(0);

  // Insert elements into sizer
  m_pFlexSizer->Add( m_pCanvas, 1, wxEXPAND );
  m_pFlexSizer->Add( m_pScrollBarV, 1, wxEXPAND );
  m_pFlexSizer->Add( m_pScrollBarH, 1, wxEXPAND );    

  SetSizer(m_pFlexSizer);
  
  SetAutoLayout(TRUE);
}


/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaFrame
 *      Method:  ~AmayaFrame
 * Description:  destructor do nothing for the moment
 *--------------------------------------------------------------------------------------
 */
AmayaFrame::~AmayaFrame()
{
}

#ifdef _GL

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaFrame
 *      Method:  SetCurrent
 * Description:  just give focus to this OpenGL canvas -> 
 *               now opengl commands are forwared to this canvas
 *--------------------------------------------------------------------------------------
 */
void AmayaFrame::SetCurrent()
{
  if (m_pCanvas)
    m_pCanvas->SetCurrent();
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaFrame
 *      Method:  SwapBuffers
 * Description:  swap the buffer because opengl draw into a backbuffer not visible
 *               to show this backbuffer this command must be called
 *--------------------------------------------------------------------------------------
 */
void AmayaFrame::SwapBuffers()
{
  if (m_pCanvas)
    m_pCanvas->SwapBuffers();
}
#endif // #ifdef _GL

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaFrame
 *      Method:  OnScroll
 * Description:  this method is called when a scroll event is comming
 *               ie : when a scrollbar is moved
 *--------------------------------------------------------------------------------------
 */
void AmayaFrame::OnScroll( wxScrollEvent& event )
{
  wxLogDebug( _T("FrameCanvas - OnScroll: h/v=%s pos=%d"),
     event.GetOrientation() == wxHORIZONTAL ? _T("h") : _T("v"),
     event.GetPosition() );

  if (event.GetOrientation() == wxHORIZONTAL)
   {
     FrameHScrolledCallback(
	m_FrameId,
	event.GetPosition(),
	m_pScrollBarH->GetPageSize() );
   }
  else if (event.GetOrientation() == wxVERTICAL)
   {
     FrameVScrolledCallback(
	m_FrameId,
	event.GetPosition() );
   }
  
  event.Skip();
}

/*----------------------------------------------------------------------
 *  this is where the event table is declared
 *  the callbacks are assigned to an event type
 *  example :
 *    + AmayaFrame::OnScroll is assigned to a scroll event 
 *----------------------------------------------------------------------*/
BEGIN_EVENT_TABLE(AmayaFrame, wxPanel)
  EVT_SCROLL( 		AmayaFrame::OnScroll ) // all scroll events
END_EVENT_TABLE()

#endif // #ifdef _WX
