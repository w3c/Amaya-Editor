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

#include "AmayaWindow.h"
#include "AmayaFrame.h"
#include "AmayaCanvas.h"
#include "AmayaNotebook.h"
#include "AmayaPage.h"

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
          ,m_PageTitle()
          ,m_WindowTitle()
          ,m_pPageParent( NULL )
{
#if 0
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

  Layout();
#endif
  // Create drawing area
  m_pCanvas = new AmayaCanvas( this );
  
  m_pScrollBarV = NULL; 
  m_pScrollBarH = NULL;
  
  m_pHSizer = new wxBoxSizer ( wxHORIZONTAL );
  m_pVSizer = new wxBoxSizer ( wxVERTICAL );
  m_pHSizer->Add( m_pCanvas, 1, wxEXPAND );
  m_pVSizer->Add( m_pHSizer, 1, wxEXPAND );
  
  // The scrollbars are added when ShowScrollbar is called
  ShowScrollbar( 1 );
  ShowScrollbar( 2 );

  SetSizer(m_pVSizer);
  
  m_pVSizer->Fit(this);
  m_pVSizer->Layout();
  
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

/*
 *--------------------------------------------------------------------------------------
 * Description:  some attribut getters ...
 *--------------------------------------------------------------------------------------
 */
int AmayaFrame::GetFrameId()
{
  return m_FrameId;
}
wxScrollBar * AmayaFrame::GetScrollbarH()
{
  return m_pScrollBarH;
}
wxScrollBar * AmayaFrame::GetScrollbarV()
{
  return m_pScrollBarV;
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaFrame
 *      Method:  HideScrollbar
 * Description:  hide a scrollbar (remove it from the sizer ...)
 *      params:
 *            + int scrollbar_id:
 *            + 1 => Vertical scrollbar
 *            + 2 => Horizontal scrollbar
 *--------------------------------------------------------------------------------------
 */
void AmayaFrame::HideScrollbar( int scrollbar_id )
{
  switch( scrollbar_id )
   {
    case 1:
       {
	 // do not remove the scrollbar if it doesnt exist
	 if (!m_pScrollBarV) return;

         m_pHSizer->Remove(m_pScrollBarV);
	 m_pScrollBarV = NULL;
//	 m_pScrollBarV->Hide();
       }
      break;
    case 2:
       {
	 // do not remove the scrollbar if it doesnt exist
	 if (!m_pScrollBarH) return;

	 m_pVSizer->Remove(m_pScrollBarH);
	 m_pScrollBarH = NULL;
//	 m_pScrollBarH->Hide();
       }
      break;
   }
  
/*  m_pHSizer->Layout();
  m_pVSizer->Layout();
  Layout();
  SetAutoLayout(TRUE);*/
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaFrame
 *      Method:  ShowScrollbar
 * Description:  show a scrollbar (add it to the sizer)
 *      params:
 *            + int scrollbar_id:
 *            + 1 => Vertical scrollbar
 *            + 2 => Horizontal scrollbar
 *--------------------------------------------------------------------------------------
 */
void AmayaFrame::ShowScrollbar( int scrollbar_id )
{
  switch( scrollbar_id )
   {
    case 1:
       {
	 // do not create the scrollbar if it always exist
	 if (m_pScrollBarV) return;
        
	 // Create vertical scrollbar
	 m_pScrollBarV = new wxScrollBar( this,
				   -1,
				   wxDefaultPosition,
				   wxDefaultSize,
				   wxSB_VERTICAL );
  
	 m_pHSizer->Add( m_pScrollBarV, 0, wxEXPAND );
	 m_pScrollBarV->Show();
       }
      break;
    case 2:
       {
	 // do not create the scrollbar if it always exist
	 if (m_pScrollBarH) return;
	 

	 // Create vertical and horizontal scrollbars
	 m_pScrollBarH = new wxScrollBar( this,
				   -1,
				   wxDefaultPosition,
				   wxDefaultSize,
				   wxSB_HORIZONTAL );
  
	 m_pVSizer->Add( m_pScrollBarH, 0, wxEXPAND );
	 m_pScrollBarH->Show();
       }
      break;
   }
/*  m_pVSizer->Fit(this);
  m_pVSizer->Layout();
  SetAutoLayout(TRUE);*/
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
  wxLogDebug( _T("FrameCanvas::OnScroll: frame=%d h/v=%s pos=%d"),
     m_FrameId,
     event.GetOrientation() == wxHORIZONTAL ? _T("h") : _T("v"),
     event.GetPosition() );

  if (m_pScrollBarH && event.GetOrientation() == wxHORIZONTAL)
   {
     FrameHScrolledCallback(
	m_FrameId,
	event.GetPosition(),
	m_pScrollBarH->GetPageSize() );
   }
  else if (m_pScrollBarV && event.GetOrientation() == wxVERTICAL)
   {
     FrameVScrolledCallback(
	m_FrameId,
	event.GetPosition() );
   }
  
  event.Skip();
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaFrame
 *      Method:  OnMouse
 * Description:  nothing is done for the moment (mouse event are not caught here -> see AmayaCanvas)
 *--------------------------------------------------------------------------------------
 */
void AmayaFrame::OnMouse( wxMouseEvent& event )
{
  // MOUSE WHEEL  
//if ( event.GetEventType() == wxEVT_MOUSEWHEEL )  
  {
    int direction = event.GetWheelRotation();
    int delta     = event.GetWheelDelta();

    wxLogDebug( _T("AmayaFrame::wxEVT_MOUSEWHEEL: frame=%d direction=%s delta=%d"),
	m_FrameId,
       	direction > 0 ? _T("up") : _T("down"),
	delta );    
/*
    if ( !FrameMouseWheelCallback( 
      frame,
      thot_mod_mask,
      direction,
      delta,
      event.GetX(), event.GetY() ) )
    {
      return;
    }*/
  }
  
  // forward current event to parent widgets
  event.Skip();
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaFrame
 *      Method:  OnSize
 * Description:  nothing is done for the moment
 *--------------------------------------------------------------------------------------
 */
void AmayaFrame::OnSize( wxSizeEvent& event )
{
  wxLogDebug(_T("AmayaFrame::OnSize: frame=%d w=%d h=%d"),
        m_FrameId,
	event.GetSize().GetWidth(),
	event.GetSize().GetHeight() );

//  Layout();

  // forward current event to parent widgets
  event.Skip();
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaFrame
 *      Method:  SetPageTitle
 * Description:  set the page name (tab name)
 *--------------------------------------------------------------------------------------
 */
void AmayaFrame::SetPageTitle(const wxString & page_name)
{
  m_PageTitle = page_name;

  AmayaPage * p_page = GetPageParent();
  if (p_page)
  {
    AmayaNotebook * p_notebook = p_page->GetNotebookParent();
    if (p_notebook)
    {
      int page_id = p_notebook->GetPageId(p_page);
      if (page_id != -1)
      {
	p_notebook->SetPageText(page_id, m_PageTitle);
      }
    }
  }

  // update also the window title
  SetWindowTitle( m_PageTitle + wxString(_T(" - Amaya 8.2")) );
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaFrame
 *      Method:  GetPageTitle
 * Description:  get the page name (tab name)
 *--------------------------------------------------------------------------------------
 */
wxString AmayaFrame::GetPageTitle()
{
  return m_PageTitle;
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaFrame
 *      Method:  SetWindowTitle
 * Description:  set the top window name
 *--------------------------------------------------------------------------------------
 */
void AmayaFrame::SetWindowTitle(const wxString & window_name)
{
  m_WindowTitle = window_name;

  // check if this frame's page is active or not
  AmayaPage * p_page = GetPageParent();
  if (p_page)
  {
    if ( p_page->IsSelected() )
    {
      // if the frame's page is active then update the window name
      AmayaWindow * p_window = p_page->GetWindowParent();
      if (p_window)
	p_window->SetTitle( m_WindowTitle );
    }
  }
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaFrame
 *      Method:  GetWindowTitle
 * Description:  get the top window name
 *--------------------------------------------------------------------------------------
 */
wxString AmayaFrame::GetWindowTitle()
{
  return m_WindowTitle;
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaFrame
 *      Method:  SetPageParent / GetPageParent
 * Description:  set/get the parent page (tab)
 *--------------------------------------------------------------------------------------
 */
void AmayaFrame::SetPageParent( AmayaPage * p_page )
{
  m_pPageParent = p_page;
}
AmayaPage * AmayaFrame::GetPageParent()
{
  return m_pPageParent;
}

/*----------------------------------------------------------------------
 *  this is where the event table is declared
 *  the callbacks are assigned to an event type
 *  example :
 *    + AmayaFrame::OnScroll is assigned to a scroll event 
 *----------------------------------------------------------------------*/
BEGIN_EVENT_TABLE(AmayaFrame, wxPanel)
  EVT_SCROLL( 		AmayaFrame::OnScroll ) // all scroll events
  EVT_MOUSEWHEEL(	AmayaFrame::OnMouse) // Process a wxEVT_MOUSEWHEEL event. 
  EVT_SIZE( 		AmayaFrame::OnSize )
END_EVENT_TABLE()

#endif // #ifdef _WX
