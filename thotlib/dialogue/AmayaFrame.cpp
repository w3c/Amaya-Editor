#ifdef _WX

#include "wx/wx.h"
#include "wx/menu.h"
#include "wx/string.h"

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

#include "AmayaWindow.h"
#include "AmayaFrame.h"
#include "AmayaNotebook.h"
#include "AmayaPage.h"
#include "AmayaCallback.h"
//#include "AmayaTextGraber.h"
#include "AmayaScrollBar.h"
#include "wx/log.h"

IMPLEMENT_DYNAMIC_CLASS(AmayaFrame, wxPanel)

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaFrame
 *      Method:  AmayaFrame
 * Description:  just construct a frame : AmayaCanvas + 2 AmayaScrollBar into a wxFlexGridSizer
 *--------------------------------------------------------------------------------------
 */
AmayaFrame::AmayaFrame(
                 int             frame_id
		,AmayaWindow *   p_parent_window
	      )
  :  wxPanel( p_parent_window, -1, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL
#ifndef _WINDOWS
  | wxRAISED_BORDER
#endif /* _WINDOWS */
  )
     ,m_FrameId( frame_id )
     ,m_FrameTitle()
     ,m_WindowTitle()
     ,m_pPageParent( NULL )
     ,m_pMenuBar( NULL )
     ,m_IsActive( FALSE )
     ,m_HOldPosition( 0 )
     ,m_VOldPosition( 0 )
     ,m_ToDestroy( FALSE )
     ,m_FrameUrlEnable( FALSE )
{
  wxLogDebug( _T("AmayaFrame::AmayaFrame() - frame_id=%d"), m_FrameId );

  // Create the drawing area
  m_pCanvas = CreateDrawingArea();

  m_pScrollBarV = NULL; 
  m_pScrollBarH = NULL;

  // create a textctrl widget.
  // it will receive every keybord events in order to handle unicode.
  // do not hide this widget because it can't get events when hidden
  /*  m_pTextGraber = new AmayaTextGraber( GetFrameId(),
				       this,
				       -1,
				       _T(""),
				       wxPoint(-10,-10),
				       wxSize(0,0),
				       wxNO_BORDER | wxTRANSPARENT_WINDOW | wxNO_FULL_REPAINT_ON_RESIZE |
				       wxWANTS_CHARS | wxTE_PROCESS_ENTER | wxTE_PROCESS_TAB );
  */

  // create horizontal siser and vertical sizer
  m_pHSizer = new wxBoxSizer ( wxHORIZONTAL );
  m_pVSizer = new wxBoxSizer ( wxVERTICAL );
  if (m_pCanvas)
    m_pHSizer->Add( m_pCanvas, 1, wxEXPAND );
  // now add widgets to vertical sizer
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
  wxLogDebug( _T("AmayaFrame::~AmayaFrame() - frame_id=%d"), m_FrameId );

  // notifie the page that this frame has die
  //  if ( GetPageParent() )
  //    GetPageParent()->DeletedFrame( this );

  // destroy the menu bar
  if (m_pMenuBar)
    m_pMenuBar->Destroy();
  m_pMenuBar = NULL;

  /* destroy the canvas (it should be automaticaly destroyed by 
     wxwidgets but due to a strange behaviour, I must explicitly delete it)*/
  if (m_pCanvas)
    m_pCanvas->Destroy();
  m_pCanvas = NULL;

  // it's possible to fall here if a frame is a child of a page but is not deleted
  // then if the page is closed, the frame is deleted by wxWidgets because the frame is a child of the page.
  // it's important to free the corresponding frame context
  //DestroyFrame( m_FrameId );
  memset(&FrameTable[GetFrameId()], 0, sizeof(Frame_Ctl));
  //  FrameTable[GetFrameId()].WdFrame = NULL;
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
      p_canvas = new AmayaCanvas( this, this );
      SetSharedContext( m_FrameId );
    }
  else
    {
      wxASSERT( FrameTable[GetSharedContext()].WdFrame != NULL );
      wxGLContext * p_SharedContext = FrameTable[GetSharedContext()].WdFrame->GetCanvas()->GetContext();
      wxASSERT( p_SharedContext );
      // create the new canvas with the opengl shared context
      p_canvas = new AmayaCanvas( this, this, p_SharedContext );
    }
#endif /*_NOSHARELIST*/

#ifdef _NOSHARELIST
  p_canvas = new AmayaCanvas( this, this );
#endif /* _NOSHARELIST */

#else /* _GL */
  p_canvas = new AmayaCanvas( this, this );
#endif /* _GL */
  return p_canvas;
}

void AmayaFrame::ReplaceDrawingArea( AmayaCanvas * p_new_canvas )
{
  if( !p_new_canvas )
    return;

  // detach scrollbar and canvas (delete canvas)
  if (m_pCanvas)
  {
    m_pCanvas->Hide();
    m_pHSizer->Remove(0);
  }
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
AmayaScrollBar * AmayaFrame::GetScrollbarH()
{
  return m_pScrollBarH;
}
AmayaScrollBar * AmayaFrame::GetScrollbarV()
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
				    wxSize( 0, /*m_pScrollBarV->GetSize().GetHeight()*/ -1 ) );
       }
      break;
    case 2:
       {
	 // do not remove the scrollbar if it doesnt exist
	 if (!m_pScrollBarH) return;

	 m_pVSizer->SetItemMinSize( m_pScrollBarH,
				    wxSize( -1 /*m_pScrollBarV->GetSize().GetWidth()*/, 0 ) );
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
	     m_pScrollBarV = new AmayaScrollBar( this, GetFrameId(), wxSB_VERTICAL );
	     m_pHSizer->Add( m_pScrollBarV, 0, wxEXPAND );
	   }
	 else
	   {
	     m_pHSizer->SetItemMinSize( m_pScrollBarV,
					wxSize( 15, -1/*m_pScrollBarV->GetSize().GetHeight()*/ ) );
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
	     m_pScrollBarH = new AmayaScrollBar( this, GetFrameId(), wxSB_HORIZONTAL );
	     m_pVSizer->Add( m_pScrollBarH, 0, wxEXPAND );
	   }
	 else
	   {
	     m_pVSizer->SetItemMinSize( m_pScrollBarH,
					wxSize( 15, /*m_pScrollBarV->GetSize().GetWidth()*/ -1 ) );
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
  if ( DisplayIsReady() )
  {
#ifdef _GL_DEBUG
    wxLogDebug( _T("AmayaFrame::SetCurrent()[OK] - frame_id=%d"), m_FrameId );
#endif /* _GL_DEBUG */
    m_pCanvas->SetCurrent();
  }
  else
  {
#ifdef _GL_DEBUG
    wxLogDebug( _T("AmayaFrame::SetCurrent()[!OK] - frame_id=%d"), m_FrameId );
#endif /* _GL_DEBUG */
  }
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaFrame
 *      Method:  DisplayIsReady
 * Description:  return true if the canvas is ready to recived drawing instructions
 *               usefull with opengl because on certains implementations, it's important to wait 
 *               for initilaisation before sending commands to opengl.
 *--------------------------------------------------------------------------------------
 */
bool AmayaFrame::DisplayIsReady()
{
  return (m_pCanvas && m_pCanvas->IsInit());
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
 
  // forward current event to parent widgets
  event.Skip();
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaFrame
 *      Method:  SetFrameTitle
 * Description:  set the frame name =>
 *               used to set the page name (tab name) or the window parent name
 *--------------------------------------------------------------------------------------
 */
void AmayaFrame::SetFrameTitle(const wxString & frame_name)
{
  m_FrameTitle = frame_name;

  // do not update window title if the title is empty
  if ( m_FrameTitle.IsEmpty() && frame_name.IsEmpty() )
    return;

  AmayaPage * p_page = GetPageParent();
  if (p_page)
  {
    AmayaNotebook * p_notebook = p_page->GetNotebookParent();
    if (p_notebook)
      {
	int page_id = p_notebook->GetPageId(p_page);
	if (page_id >= 0)
	  {
	    // setup the page tooltip
	    // the tooltip should be shown only when the mouse is over the tab,
	    // maybe hook the tooltip to an image which could be shown into each tab. (as mozilla do)
	    // p_page->SetToolTip( m_FrameTitle );

	    // setup the tab title
	    wxString frame_title = GetFrameTitle();
	    p_notebook->SetPageText( page_id,
				     wxString(frame_title).Truncate(15) + (frame_title.Len() > 15 ? _T("...") : _T("")) );
	  }
      }
  }

  // update also the window title
  SetWindowTitle( m_FrameTitle );
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaFrame
 *      Method:  GetFrameTitle
 * Description:  get the frame name 
 *               used to setup the page name or window parent name
 *--------------------------------------------------------------------------------------
 */
wxString AmayaFrame::GetFrameTitle()
{
  if (GetMasterFrameId() == GetFrameId())
    return m_FrameTitle;
  else
    {
      AmayaFrame * p_frame = TtaGetFrameFromId(GetMasterFrameId());
      if (p_frame)
	return p_frame->m_FrameTitle;
      else
	return m_FrameTitle;
    }
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
  
  AmayaWindow * p_window = GetWindowParent();
  if (!p_window)
    return;

  if (p_window->GetKind() == WXAMAYAWINDOW_NORMAL)
    {
      // check if this frame's page is active or not
      AmayaPage * p_page = GetPageParent();
      if ( !p_page || !p_page->IsSelected() )
	return;
    }

  p_window->SetTitle( m_WindowTitle +
		      _T(" - Amaya ") +
		      wxString::FromAscii( HTAppVersion ) );
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
 *      Method:  UpdateFrameURL
 * Description:  just update the internal frame url value
 *               (the master frame is updated.)
 *--------------------------------------------------------------------------------------
 */
void AmayaFrame::UpdateFrameURL( const wxString & new_url )
{
  if (GetMasterFrameId() == GetFrameId())
    m_FrameUrl = new_url;
  else
    {
      AmayaFrame * p_frame = TtaGetFrameFromId(GetMasterFrameId());
      if (p_frame)
	p_frame->m_FrameUrl = new_url;
      else
	m_FrameUrl = new_url;
    }
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
  wxLogDebug(_T("AmayaFrame::GetFrameURL - frameid=%d url=")+m_FrameUrl, GetFrameId());
  if (GetMasterFrameId() == GetFrameId())
    return m_FrameUrl;
  else
    {
      AmayaFrame * p_frame = TtaGetFrameFromId(GetMasterFrameId());
      if (p_frame)
	return p_frame->m_FrameUrl;
      else
	return m_FrameUrl;
    }
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
  if (GetMasterFrameId() == GetFrameId())
    return m_FrameUrlEnable;
  else
    {
      AmayaFrame * p_frame = TtaGetFrameFromId(GetMasterFrameId());
      if (p_frame)
	return p_frame->m_FrameUrlEnable;
      else
	return m_FrameUrlEnable;
    }
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
  return TtaGetWindowFromId( TtaGetFrameWindowParentId(GetFrameId()) );
}

void AmayaFrame::OnClose(wxCloseEvent& event)
{
  wxLogDebug( _T("AmayaFrame::OnClose: frame=%d"), m_FrameId );

  // activate the frame just before closing it
  // so the user can see what it's beeing closed 
  SetActive( TRUE );
  if ( GetPageParent() )
    GetPageParent()->RaisePage();

  PtrDocument         pDoc;
  int                 view;
  GetDocAndView (m_FrameId, &pDoc, &view);
  CloseView (pDoc, view);
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

void AmayaFrame::SetActive( bool active )
{
  // do nothing if the frame stat doesnt change
  //if (m_IsActive == active)
  //  return;
	
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
  SetFrameTitle( GetFrameTitle() );
  
  // this frame is active update its page
  AmayaPage * p_page = GetPageParent();
  if (p_page)
    {
      p_page->SetActiveFrame( this );

      // setup the right frame url into the main window urlbar
      p_page->SetWindowURL( p_page->GetActiveFrame()->GetFrameURL() );

      // setup the enable/disable state of urlbar
      p_page->SetWindowEnableURL( p_page->GetActiveFrame()->GetFrameEnableURL() );

      // setup the enable/disable state of the toolbar buttons
      p_page->SetWindowEnableToolBarButtons( p_page->GetActiveFrame()->GetMasterFrameId() );
    }

  // update internal thotlib global var : ActiveFrame
  ChangeSelFrame(GetFrameId());

  // give focuse to this frame
  //  DistributeFocus();
}

void AmayaFrame::DistributeFocus()
{
  wxLogDebug( _T("AmayaFrame::DistributeFocus - frame=%d"), GetFrameId() );
  /*
  if (m_pTextGraber)
    m_pTextGraber->SetFocus();
  */
}

bool AmayaFrame::IsActive()
{
  return m_IsActive;
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaFrame
 *      Method:  IsMenuActive
 * Description:  returns true if the frame's menu is the top menu of the parent window
 *--------------------------------------------------------------------------------------
 */
bool AmayaFrame::IsMenuActive()
{
  AmayaWindow * p_window = GetWindowParent();
  if ( !p_window )
    return false;
  return (p_window->GetMenuBar() == GetMenuBar());
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaFrame
 *      Method:  RaiseFrame
 * Description:  popup the frame container and activate it
 *--------------------------------------------------------------------------------------
 */
void AmayaFrame::RaiseFrame()
{
  AmayaWindow * p_window = GetWindowParent();
  if ( !p_window )
    return;
  
  if ( p_window->GetKind() == WXAMAYAWINDOW_NORMAL)
    {
      // raise the page parent
      AmayaPage * p_page = GetPageParent();
      if (p_page)
	p_page->RaisePage();
    }
  else if ( p_window->GetKind() == WXAMAYAWINDOW_SIMPLE)
    {
      // raise the window parent
      p_window->Raise();
      SetActive( TRUE );
    }
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaFrame
 *      Method:  SetStatusBarText
 * Description:  
 *--------------------------------------------------------------------------------------
 */
void AmayaFrame::SetStatusBarText( const wxString & text )
{
  wxLogDebug( _T("AmayaFrame::SetStatusBarText - len=%d"), text.Length() );

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

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaFrame
 *      Method:  FreeFrame
 * Description:  
 *--------------------------------------------------------------------------------------
 */
void AmayaFrame::FreeFrame()
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
  // there is a strange bug :
  // the drawind area could not be reused (sometimes black background), it must be recreated.
  //  ReplaceDrawingArea( CreateDrawingArea() );

  // do not delete realy the frame because there is a strange bug
  // Destroy();

  // Reactivate the menu bar (nothing is done if the window is goind to die)
  if (GetWindowParent())
    GetWindowParent()->ActivateMenuBar();

  // the frame is destroyed so it hs no more page parent
  SetPageParent( NULL );
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaFrame
 *      Method:  OnIdle
 * Description:  
 *--------------------------------------------------------------------------------------
 */
void AmayaFrame::OnIdle( wxIdleEvent& event )
{
  //  if ( m_ToDestroy )
  //    Destroy();
  //  else
    event.Skip();
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaFrame
 *      Method:  OnContextMenu
 * Description:  
 *--------------------------------------------------------------------------------------
 */
void AmayaFrame::OnContextMenu( wxContextMenuEvent & event )
{
  wxLogDebug( _T("AmayaFrame::OnContextMenu - (x,y)=(%d,%d)"),
	      event.GetPosition().x,
	      event.GetPosition().y );

  // no contextual menu into a frame

  // this should be activated but becarfull because when contextual menu is shown,
  // "mouse up" event is not proceded until the contextual menu is closed
  // if you try to delete the current canvas with a command form the contextual
  // menu this will crash amaya because a "mouse Up" will be generated just 
  // when the menu is hidden, so when the command is proceded , so when the canvas is allready closed

  // to activate the contextual menu into the frame just uncomment the following line
  //  event.Skip();  
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaFrame
 *      Method:  GetMasterFrameId
 * Description:  
 *--------------------------------------------------------------------------------------
 */
int AmayaFrame::GetMasterFrameId()
{
  AmayaPage * p_page = GetPageParent();
  if (p_page)
    return p_page->GetMasterFrameId();
  else
    return -1;
}

/*----------------------------------------------------------------------
 *  this is where the event table is declared
 *  the callbacks are assigned to an event type
 *  example :
 *    + AmayaFrame::OnScroll is assigned to a scroll event 
 *----------------------------------------------------------------------*/
BEGIN_EVENT_TABLE(AmayaFrame, wxPanel)
  EVT_CLOSE( 		AmayaFrame::OnClose )
  EVT_SIZE( 		AmayaFrame::OnSize )
  EVT_IDLE(             AmayaFrame::OnIdle ) // Process a wxEVT_IDLE event
  EVT_CONTEXT_MENU(     AmayaFrame::OnContextMenu )
END_EVENT_TABLE()

#endif // #ifdef _WX
