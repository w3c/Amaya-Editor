#ifdef _WX

#include "wx/wx.h"
#include "wx/menu.h"

#include "thot_gui.h"
#include "thot_sys.h"
#include "constmedia.h"

#include "AmayaCanvas.h"

#include "typemedia.h"
#include "appdialogue.h"
#include "dialog.h"
#include "application.h"
#include "dialog.h"
#include "document.h"
#include "message.h"
#include "libmsg.h"
#include "frame.h"

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
#include "appdialogue_wx_f.h"

#include "AmayaWindow.h"
#include "AmayaFrame.h"
//#include "AmayaCanvas.h"
#include "AmayaNotebook.h"
#include "AmayaPage.h"
#include "AmayaCallback.h"
#include "wx/log.h"

IMPLEMENT_DYNAMIC_CLASS(AmayaFrame, wxPanel)

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
     ,m_pMenuBar( NULL )
     ,m_IsActive( FALSE )
     ,m_HOldPosition( 0 )
     ,m_VOldPosition( 0 )
     ,m_ToDestroy( FALSE )
     ,m_FrameUrlEnable( FALSE )
{
  // Create the drawing area
  m_pCanvas = CreateDrawingArea();

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
  // notifie the page that this frame has die
  //  if ( GetPageParent() )
  //    GetPageParent()->DeletedFrame( this );

  // destroy the menu bar
  if (m_pMenuBar) m_pMenuBar->Destroy();
  m_pMenuBar = NULL;

  /* destroy the canvas (it should be automaticaly destroyed by 
     wxwidgets but due to a strange behaviour, I must explicitly delete it)*/
  m_pCanvas->Destroy();
  m_pCanvas = NULL;

  // the FrameTable array must be freed because WdFrame field is
  // used to know if the frame is still alive or not
  FrameTable[m_FrameId].WdFrame = 0;
}

AmayaCanvas * AmayaFrame::CreateDrawingArea()
{
  AmayaCanvas * p_canvas = NULL;

#ifdef _GL

#ifndef _NOSHARELIST
  // If opengl is used then try to share the context
  if ( GetSharedContext () == -1/* || GetSharedContext () == m_FrameId */)
    {
      /* there is no existing context, I need to create a first one and share it with others canvas */
      p_canvas = new AmayaCanvas( this );
      SetSharedContext( m_FrameId );
    }
  else
    {
      wxASSERT( FrameTable[GetSharedContext()].WdFrame != NULL );
      wxGLContext * p_SharedContext = FrameTable[GetSharedContext()].WdFrame->GetCanvas()->GetContext();
      wxASSERT( p_SharedContext );
      // create the new canvas with the opengl shared context
      p_canvas = new AmayaCanvas( this, p_SharedContext );
    }
#endif /*_NOSHARELIST*/

#ifdef _NOSHARELIST
  p_canvas = new AmayaCanvas( this );
#endif /* _NOSHARELIST */

  /* try to force opengl to use this canvas (initialize the opengl context) */
  p_canvas->SetCurrent();

#else /* _GL */
  p_canvas = new AmayaCanvas( this );
#endif /* _GL */
  return p_canvas;
}

void AmayaFrame::ReplaceDrawingArea( AmayaCanvas * p_new_canvas )
{
  wxASSERT( p_new_canvas );

  // detach scrollbar and canvas (delete canvas)
  m_pCanvas->Hide();
  m_pHSizer->Remove(0);
  m_pHSizer->Detach(0); // the scroll bar 

  // assign the new canvas
  m_pCanvas = p_new_canvas;

  // add the new canvas and old scrollbar to the sizer
  m_pHSizer->Add( m_pCanvas,     1, wxEXPAND );
  if (m_pScrollBarV)
    m_pHSizer->Add( m_pScrollBarV, 0, wxEXPAND );
  m_pHSizer->Layout();
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
AmayaCanvas * AmayaFrame::GetCanvas()
{
  return m_pCanvas;
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

	 m_pHSizer->SetItemMinSize( m_pScrollBarV,
				    wxSize( 0, m_pScrollBarV->GetSize().GetHeight() ) );
       }
      break;
    case 2:
       {
	 // do not remove the scrollbar if it doesnt exist
	 if (!m_pScrollBarH) return;

	 m_pVSizer->SetItemMinSize( m_pScrollBarH,
				    wxSize( m_pScrollBarV->GetSize().GetWidth(), 0 ) );
       }
      break;
   }
  m_pHSizer->Layout();
  m_pVSizer->Layout();
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
	 if (!m_pScrollBarV)
	   {
        
	     // Create vertical scrollbar
	     m_pScrollBarV = new wxScrollBar( this,
					      -1,
					      wxDefaultPosition,
					      wxDefaultSize,
					      wxSB_VERTICAL );
	     m_pHSizer->Add( m_pScrollBarV, 0, wxEXPAND );
	   }
	 else
	   {
	     m_pHSizer->SetItemMinSize( m_pScrollBarV,
					wxSize( 15, m_pScrollBarV->GetSize().GetHeight() ) );
	   }
	 
	 m_pScrollBarV->Show();
       }
      break;
    case 2:
       {
	 // do not create the scrollbar if it always exist
	 if (!m_pScrollBarH)
	   {
	     // Create vertical and horizontal scrollbars
	     m_pScrollBarH = new wxScrollBar( this,
					      -1,
					      wxDefaultPosition,
					      wxDefaultSize,
					      wxSB_HORIZONTAL );
	     m_pVSizer->Add( m_pScrollBarH, 0, wxEXPAND );
	   }
	 else
	   {
	     m_pVSizer->SetItemMinSize( m_pScrollBarH,
					wxSize( 15, m_pScrollBarV->GetSize().GetWidth() ) );
	   }
	 m_pScrollBarH->Show();
      }
      break;
   }
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
  if (m_pCanvas && m_pCanvas->IsInit())
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
  if (m_pCanvas && m_pCanvas->IsInit())
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
  wxLogDebug( _T("AmayaFrame::OnScroll: frame=%d h/v=%s pos=%d"),
     m_FrameId,
     event.GetOrientation() == wxHORIZONTAL ? _T("h") : _T("v"),
     event.GetPosition() );

  if (m_pScrollBarH && event.GetOrientation() == wxHORIZONTAL)
   {
     /*
     if (m_HOldPosition < event.GetPosition())
       m_HOldPosition = event.GetPosition()+13;
     else if (m_HOldPosition > event.GetPosition())
       m_HOldPosition = event.GetPosition()-13;
     */

     FrameHScrolledCallback(
	m_FrameId,
	event.GetPosition(),
	m_pScrollBarH->GetPageSize() );
   }
  else if (m_pScrollBarV && event.GetOrientation() == wxVERTICAL)
   {
     /*
     if (m_VOldPosition < event.GetPosition())
       m_VOldPosition = event.GetPosition()+13;
     else if (m_VOldPosition > event.GetPosition())
       m_VOldPosition = event.GetPosition()-13;
     */
     FrameVScrolledCallback(
	m_FrameId,
	event.GetPosition() );
   }
  
  event.Skip();
}


void AmayaFrame::OnScrollLineUp( wxScrollEvent& event )
{
  wxLogDebug( _T("AmayaFrame::OnScrollLineUp: frame=%d h/v=%s pos=%d"),
     m_FrameId,
     event.GetOrientation() == wxHORIZONTAL ? _T("h") : _T("v"),
     event.GetPosition() );

  event.Skip();
}

void AmayaFrame::OnScrollLineDown( wxScrollEvent& event )
{
  wxLogDebug( _T("AmayaFrame::OnScrollLineDown: frame=%d h/v=%s pos=%d"),
     m_FrameId,
     event.GetOrientation() == wxHORIZONTAL ? _T("h") : _T("v"),
     event.GetPosition() );

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
  wxLogDebug(_T("AmayaFrame::OnSize: frame=%d w=%d h=%d wc=%d, hc=%d"),
        m_FrameId,
	event.GetSize().GetWidth(),
	event.GetSize().GetHeight(),
	m_pHSizer->GetSize().GetWidth(),
	m_pHSizer->GetSize().GetHeight() );
 
  int w;
  int h;
  if (m_pScrollBarH && m_pScrollBarH->IsShown())
    {
      //       h = event.GetSize().GetHeight() - m_pScrollBarH->GetSize().GetHeight();
      //      m_pVSizer->SetItemMinSize( m_pScrollBarH, wxSize(10, m_pScrollBarH->GetSize().GetHeight() ) );
    }
  else
    {
      //      m_pVSizer->SetItemMinSize( m_pScrollBarH, wxSize(0,0) );
      //      h = event.GetSize().GetHeight();
    }

  if (m_pScrollBarV && m_pScrollBarV->IsShown())
    {
      // w = event.GetSize().GetWidth() - m_pScrollBarV->GetSize().GetWidth();
    }
  else
    {
      //m_pHSizer->SetItemMinSize( m_pScrollBarV, wxSize(0,0) );
      // w = event.GetSize().GetWidth();
    }

  /*  m_pHSizer->SetItemMinSize( m_pCanvas,
			     wxSize( m_pScrollBarV->IsShown() ? event.GetSize().GetWidth() - m_pScrollBarV->GetSize().GetWidth() : event.GetSize().GetWidth(),
			     m_pScrollBarH->IsShown() ? event.GetSize().GetHeight() - m_pScrollBarH->GetSize().GetWidth() : event.GetSize().GetHeight() ) );*/
  //  m_pHSizer->SetMinSize( wxSize(w,h) );
  //  m_pVSizer->SetMinSize( wxSize(w,h) );
  //  m_pHSizer->SetDimension(0, 0, 200, 200);
  /*
  wxLogDebug(_T("AmayaFrame::OnSize: frame=%d w=%d h=%d wc=%d, hc=%d"),
        m_FrameId,
	event.GetSize().GetWidth(),
	event.GetSize().GetHeight(),
	m_pHSizer->GetSize().GetWidth(),
	m_pHSizer->GetSize().GetHeight() );
  */
  //  GetSizer()->SetSizeHints(this);
  //  GetSizer()->Fit(this);

  /*  m_pHSizer->Layout();
  m_pVSizer->Layout();
  Layout();
  */

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
      if (page_id >= 0)
	p_notebook->SetPageText(page_id, wxString(m_PageTitle).Truncate(10) + _T("...") );
    }
  }

  // update also the window title
  SetWindowTitle( m_PageTitle );
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
	p_window->SetTitle( m_WindowTitle +
			    _T(" - Amaya ") +
			    wxString::FromAscii( HTAppVersion ) );
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
 *      Method:  SetFrameURL
 * Description:  setup the current frame url
 *--------------------------------------------------------------------------------------
 */
void AmayaFrame::SetFrameURL( const wxString & new_url )
{
  wxLogDebug( _T("AmayaFrame::SetFrameURL - frame=%d")+
	      wxString(_T(" url="))+new_url, GetFrameId() );
  m_FrameUrl = new_url;
  
  // update the window url if the frame is active
  if ( IsActive() && GetPageParent() )
    GetPageParent()->SetWindowURL( m_FrameUrl );
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaFrame
 *      Method:  GetFrameURL
 * Description:  return the corresponding document url
 *--------------------------------------------------------------------------------------
 */
wxString AmayaFrame::GetFrameURL()
{
  return m_FrameUrl;
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaFrame
 *      Method:  SetFrameEnableURL
 * Description:  force the urlbar to be enable or disable for the current frame
 *               exemple : the source vue of a document doen't have an urlbar
 *--------------------------------------------------------------------------------------
 */
void AmayaFrame::SetFrameEnableURL( bool urlenabled )
{
  m_FrameUrlEnable = urlenabled;

  // update the window url if the frame is active
  if ( IsActive() && GetPageParent() )
    GetPageParent()->SetWindowEnableURL( GetFrameEnableURL() );
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaFrame
 *      Method:  GetFrameEnableURL
 * Description:  get the frame url status (enable or disable)
 *               exemple : the source vue of a document doen't have an urlbar
 *--------------------------------------------------------------------------------------
 */
bool AmayaFrame::GetFrameEnableURL( )
{
  return m_FrameUrlEnable;
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

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaWindow
 *      Method:  GetWindowParent
 * Description:  return the window parent
 *--------------------------------------------------------------------------------------
 */
AmayaWindow * AmayaFrame::GetWindowParent()
{
  AmayaPage * p_page = GetPageParent();
  if (p_page)
  {
    return p_page->GetWindowParent();
  }
  else
    return NULL;
}


void AmayaFrame::OnClose(wxCloseEvent& event)
{
  wxLogDebug( _T("AmayaFrame::OnClose: frame=%d"), m_FrameId );
  

  PtrDocument         pDoc;
  int                 view;
  GetDocAndView (m_FrameId, &pDoc, &view);
  CloseView (pDoc, view);
  
  //  DestroyChildren();
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaFrame
 *      Method:  AppendSubMenu
 * Description:  TODO
 *--------------------------------------------------------------------------------------
 */
wxMenuItem * AmayaFrame::AppendSubMenu (
    wxMenu * 		p_menu_parent,
    long                id,
    const wxString & 	label,
    const wxString & 	help )
{
  wxMenu * p_submenu = new wxMenu( );
  
  p_menu_parent->Append( id, label, p_submenu, help );

  return p_menu_parent->FindItem( id );
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaFrame
 *      Method:  AppendMenuItem
 * Description:  TODO
 *--------------------------------------------------------------------------------------
 */
wxMenuItem * AmayaFrame::AppendMenuItem ( 
    wxMenu * 		p_menu_parent,
    long 		id,
    const wxString & 	label,
    const wxString & 	help,
    wxItemKind 		kind,
    const AmayaContext & context )
{
  /*  if ( kind != wxITEM_SEPARATOR )
  {
    wxASSERT( id+MENU_ITEM_START < MENU_ITEM_END );
    id += MENU_ITEM_START;
    }*/
  wxMenuItem * p_menu_item = new wxMenuItem( p_menu_parent,
					     id,
					     label,
					     help,
					     kind );
  p_menu_item->SetRefData( new AmayaContext(context) );

  p_menu_parent->Append( p_menu_item );

  // TODO : call callbacks
  if ( kind == wxITEM_SEPARATOR )
  {
    // do not call callback if it's a separator
  }
  return p_menu_item;
}

wxMenuBar * AmayaFrame::GetMenuBar()
{
  if (!m_pMenuBar)
    // Create the empty menu bar
    m_pMenuBar =  new wxMenuBar( /*wxMB_DOCKABLE*/ );

  return m_pMenuBar;
}

void AmayaFrame::OnMenuItem( wxCommandEvent& event )
{
  wxMenu * p_menu = (wxMenu *)event.GetEventObject();
  long     id     = event.GetId();
  wxLogDebug( _T("AmayaFrame::OnMenuItem : p_menu = 0x%x\tmenuid = %d"), p_menu, id );
}

void AmayaFrame::SetActive( bool active )
{
  // Update the m_IsActive atribute
  m_IsActive = active;

  // the window's menubar must be updated with the new active frame's one
  AmayaWindow * p_window = GetWindowParent();
  if ( !p_window )
    return;

  if ( !IsActive() )
    {
      // the window menubar is removed if it's the same as current unactive frame
      if (p_window->GetMenuBar() == GetMenuBar())
	p_window->SetMenuBar( NULL );
      return;
    }
  
  // the main menubar is replaced by the current frame one
  p_window->SetMenuBar( GetMenuBar() );

  // the main statusbar text is replaced by the current frame one
  wxStatusBar * p_statusbar = p_window->GetStatusBar();
  if ( p_statusbar )
    {
      p_statusbar->SetStatusText( m_StatusBarText );
    }

  // update the window title
  SetWindowTitle( GetWindowTitle() );
  
  // this frame is active update its page
  AmayaPage * p_page = GetPageParent();
  if (p_page)
    p_page->SetActiveFrame( this );
}

bool AmayaFrame::IsActive()
{
  return m_IsActive;
}

void AmayaFrame::RaiseFrame()
{
 // this frame is active update its page
  AmayaPage * p_page = GetPageParent();
  if (p_page)
      p_page->RaisePage();
}

void AmayaFrame::SetStatusBarText( const wxString & text )
{
  // the new text is assigned
  m_StatusBarText = text;

  if ( IsActive() )
    {
      // the window's menubar must be updated with the new active frame's one
      AmayaWindow * p_window = GetWindowParent();
      if ( !p_window )
	return;
      
      // the main statusbar text is replaced by the current frame one
      wxStatusBar * p_statusbar = p_window->GetStatusBar();
      if ( p_statusbar )
	{
	  p_statusbar->SetStatusText( m_StatusBarText );
	}
    }
}

void AmayaFrame::DestroyFrame()
{
  // Detach the window menu bar to avoid  probleme when
  // AmayaWindow will be deleted.
  // (because the menu bar is owned by AmayaFrame)
  if (GetWindowParent())
    GetWindowParent()->DesactivateMenuBar();
  

  /* first of all clean up the menubar */
  if (m_pMenuBar)
    m_pMenuBar->Destroy();
  m_pMenuBar = NULL;

  
  // Create a new drawing area
  ReplaceDrawingArea( CreateDrawingArea() );

  // do not delete realy the frame becaus there is a strang bug
  //  Destroy();

  // Reactivate the menu bar (nothing is done if the window is goind to die)
  if (GetWindowParent())
    GetWindowParent()->ActivateMenuBar();

  // the frame is destroyed so it hs no more page parent
  SetPageParent( NULL );
}

void AmayaFrame::OnIdle( wxIdleEvent& event )
{
  //  if ( m_ToDestroy )
  //    Destroy();
  //  else
    event.Skip();
}

/*----------------------------------------------------------------------
 *  this is where the event table is declared
 *  the callbacks are assigned to an event type
 *  example :
 *    + AmayaFrame::OnScroll is assigned to a scroll event 
 *----------------------------------------------------------------------*/
BEGIN_EVENT_TABLE(AmayaFrame, wxPanel)
  //  EVT_SCROLL_THUMBTRACK(    AmayaFrame::OnScrollLineUp )
  //  EVT_SCROLL_ENDSCROLL(     AmayaFrame::OnScrollLineDown )
  EVT_SCROLL( 		AmayaFrame::OnScroll ) // all scroll events
  EVT_CLOSE( 		AmayaFrame::OnClose )
  EVT_MENU( -1,         AmayaFrame::OnMenuItem )

  EVT_SIZE( 		AmayaFrame::OnSize )

  EVT_IDLE(             AmayaFrame::OnIdle) // Process a wxEVT_IDLE event

END_EVENT_TABLE()

#endif // #ifdef _WX
