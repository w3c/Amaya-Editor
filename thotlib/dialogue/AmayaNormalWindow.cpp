#ifdef _WX

#include "wx/wx.h"
#include "wx/tglbtn.h"
#include "wx/string.h"

#include "thot_gui.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "appdialogue.h"
#include "dialog.h"
#include "selection.h"
#include "application.h"
#include "dialog.h"
#include "document.h"
#include "message.h"
#include "libmsg.h"
#include "frame.h"

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "font_tv.h"
#include "frame_tv.h"
#include "appdialogue_tv.h"

#include "font_f.h"
#include "appli_f.h"
#include "profiles_f.h"
#include "appdialogue_f.h"
#include "boxparams_f.h"
#include "dialogapi_f.h"
#include "callback_f.h"
#include "appdialogue_wx_f.h"
#include "displayview_f.h"
#include "appdialogue_wx.h"

#include "AmayaNormalWindow.h"
#include "AmayaPanel.h"
#include "AmayaNotebook.h"
#include "AmayaPage.h"
#include "AmayaFrame.h"
#include "AmayaCallback.h"
#include "AmayaURLBar.h"
#include "AmayaToolBar.h"
#include "AmayaQuickSplitButton.h"
#include "AmayaSubPanelManager.h"

IMPLEMENT_DYNAMIC_CLASS(AmayaNormalWindow, AmayaWindow)

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaNormalWindow
 *      Method:  AmayaNormalWindow
 * Description:  create a new AmayaNormalWindow
 *--------------------------------------------------------------------------------------
 */
AmayaNormalWindow::AmayaNormalWindow (  int             window_id
					,wxWindow *     p_parent_window
					,const wxPoint& pos
					,const wxSize&  size
					) : 
  AmayaWindow( window_id, p_parent_window, pos, size, WXAMAYAWINDOW_NORMAL ),
  m_pURLBar( NULL ),
  m_pDummyMenuBar( NULL ),
  m_SlashPos( 150 )
{
  // Create a background panel to contain everything : better look on windows
  wxBoxSizer * p_TopSizer = new wxBoxSizer ( wxVERTICAL );
  SetSizer(p_TopSizer);
  wxPanel * p_TopParent = new wxPanel( this, -1, wxDefaultPosition, wxDefaultSize,
	                                   wxTAB_TRAVERSAL | wxCLIP_CHILDREN | wxNO_BORDER);
  p_TopSizer->Add( p_TopParent, 1, wxALL | wxEXPAND, 0 );

  // Create a splitted vertical window
  m_pSplitterWindow = new wxSplitterWindow( p_TopParent, -1,
                      		            wxDefaultPosition, wxDefaultSize,
                     		            wxSP_3DBORDER | wxSP_3DSASH | wxSP_3D /*| wxSP_PERMIT_UNSPLIT*/ );
  m_pSplitterWindow->SetMinimumPaneSize( 100 );
  
  // Create a background panel to contains the notebook
  m_pNotebookPanel = new wxPanel( m_pSplitterWindow, -1, wxDefaultPosition, wxDefaultSize,
				  wxTAB_TRAVERSAL | wxCLIP_CHILDREN | wxNO_BORDER);

  // Create the notebook with its special sizer
  m_pNotebook                              = new AmayaNotebook( m_pNotebookPanel, this );
  wxNotebookSizer * p_SpecialNotebookSizer = new wxNotebookSizer( m_pNotebook );

  // Create a sizer to layout the notebook in the panel
  wxBoxSizer * p_NotebookSizer             = new wxBoxSizer ( wxHORIZONTAL );
  p_NotebookSizer->Add(p_SpecialNotebookSizer, 1, wxEXPAND | wxALL, 0);
  m_pNotebookPanel->SetSizer(p_NotebookSizer);
  m_pNotebookPanel->Layout();
  
  // Create a AmayaPanel to contains commands shortcut
  m_pPanel = new AmayaPanel( m_pSplitterWindow, this, -1, wxDefaultPosition, wxDefaultSize,
	  wxTAB_TRAVERSAL
	  | wxRAISED_BORDER
	  | wxCLIP_CHILDREN );

  // Split the Notebook and the AmayaPanel
  m_pSplitterWindow->SplitVertically(
      m_pPanel,
      m_pNotebookPanel,
      m_SlashPos );  

  // by default close the side panel 
  ClosePanel();

  // Creation of frame sizer to contains differents frame areas
  wxBoxSizer * p_SizerFrame = new wxBoxSizer ( wxHORIZONTAL );

  // create the quick split button used to show/hide the panel
  m_pSplitPanelButton = new AmayaQuickSplitButton( p_TopParent, AmayaQuickSplitButton::wxAMAYA_QS_VERTICAL, 4 );
  p_SizerFrame->Add( m_pSplitPanelButton, 0, wxALL | wxEXPAND, 0 );
  m_pSplitPanelButton->ShowQuickSplitButton( true );
  
  // add the splitter window to the top sizer : panel + notebook
  p_SizerFrame->Add( m_pSplitterWindow, 1, wxALL | wxEXPAND, 0 );

  // Create the toolbar
  m_pToolBar = new AmayaToolBar( p_TopParent, this );

  // Creation of the top sizer to contain toolbar and framesizer
  wxBoxSizer * p_TopLayoutSizer = new wxBoxSizer ( wxVERTICAL );
  p_TopLayoutSizer->Add( m_pToolBar, 0, wxALL | wxEXPAND, 1 );
  p_TopLayoutSizer->Add( p_SizerFrame, 1, wxALL | wxEXPAND, 0 );
  p_TopParent->SetSizer(p_TopLayoutSizer);
  //  p_TopLayoutSizer->Fit(p_TopParent);

  // Creation of the statusbar
  CreateStatusBar( 1 );

  SetAutoLayout(TRUE);

  // NOTICE : the menu bar is created for each AmayaFrame, 
  //          the menu bar is not managed by the window
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaNormalWindow
 *      Method:  ~AmayaNormalWindow
 * Description:  destructor
 *--------------------------------------------------------------------------------------
 */
AmayaNormalWindow::~AmayaNormalWindow()
{
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaNormalWindow
 *      Method:  SetupURLBar
 * Description:  create and add the url to the toolbar if it's not allready done
 *--------------------------------------------------------------------------------------
 */
void AmayaNormalWindow::SetupURLBar()
{
  if ( !m_pURLBar )
    {
      // Create the url entry and add it to the toolbar
      m_pURLBar = new AmayaURLBar( m_pToolBar, this );
      m_pToolBar->AddSpacer();

      m_pToolBar->AddTool( m_pURLBar, TRUE );
    }
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaNormalWindow
 *      Method:  CreatePage
 * Description:  create a new AmayaPage, the notebook will be the parent page
 *               it's possible to attach automaticaly this page to the window or not
 *--------------------------------------------------------------------------------------
 */
AmayaPage * AmayaNormalWindow::CreatePage( bool attach, int position )
{
  AmayaPage * p_page = new AmayaPage( m_pNotebook );
  
  if (attach)
    AttachPage( position, p_page );
  
  return p_page;
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaNormalWindow
 *      Method:  AttachPage
 * Description:  really attach a page to the current window
 *--------------------------------------------------------------------------------------
 */
bool AmayaNormalWindow::AttachPage( int position, AmayaPage * p_page )
{
  bool ret;
  if (!m_pNotebook)
    ret = false;
  else
  {
    /* notebook is a new parent for the page
     * warning: AmayaPage original parent must be a wxNotbook */
//    p_page->Reparent( m_pNotebook );
    p_page->SetNotebookParent( m_pNotebook );
    
    /* insert the page in the notebook */
    ret = m_pNotebook->InsertPage( position,
       	                           p_page,
                                   _T(""),  /* this is the page name */
				   false,
				   0 ); /* this is the default image id */
    //ret = m_pNotebook->AddPage( p_page, _T("Nom de la Page") );

    // update the pages ids
    m_pNotebook->UpdatePageId();

    // the inserted page should be forced to notebook size
    m_pNotebook->Layout();
    wxLogDebug( _T("AmayaNormalWindow::AttachPage - pagesize: w=%d h=%d"),
		p_page->GetSize().GetWidth(),
		p_page->GetSize().GetHeight() );

    SetAutoLayout(TRUE);
  }
  return ret;
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaNormalWindow
 *      Method:  DetachPage
 * Description:  
 *--------------------------------------------------------------------------------------
 */
bool AmayaNormalWindow::DetachPage( int position )
{
  return false;
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaNormalWindow
 *      Method:  ClosePage
 * Description:  ferme une page
 *--------------------------------------------------------------------------------------
 */
bool AmayaNormalWindow::ClosePage( int page_id )
{
  AmayaPage * p_page  = NULL;
  p_page = GetPage( page_id );
  // close it
  p_page->Close();
  if (p_page->IsClosed())
    {
      m_pNotebook->DeletePage(page_id);
      m_pNotebook->UpdatePageId();
      TtaHandlePendingEvents ();
      
      // we force page_id-1 because m_pNotebook->GetSelection() returns a bad value ...
      // OnPageChanged event is not generated so the selected page id is not updated 
      // internaly into wxWidgets. It's bug!
      // page_id-1 is the precedent page : it supposes that the notebook behaviour
      // is to switch to precedent page when a page is deleted ... maybe it's not the case 
      // on every plateforme.
      if (page_id - 1 >= 0)
	m_pNotebook->SetSelection( page_id - 1 );

      // here GetSelection should return the right value acording to SetSelection above.
      if (m_pNotebook->GetSelection() >= 0)
	{
	  AmayaPage * p_selected_page = (AmayaPage *)m_pNotebook->GetPage(m_pNotebook->GetSelection());
	  if (p_selected_page)
	    {
	      p_selected_page->SetSelected( TRUE );
	      // the page need a refresh to repaint its canvas
	      p_selected_page->Refresh();
	    }
	}

      return true;
    }
  else
    return false;
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaNormalWindow
 *      Method:  GetPage
 * Description:  search the page at given position
 *--------------------------------------------------------------------------------------
 */
AmayaPage * AmayaNormalWindow::GetPage( int position ) const
{
  wxLogDebug( _T("AmayaNormalWindow::GetPage") );
  if (!m_pNotebook)
    return NULL;
  if (GetPageCount() <= position)
    return NULL;
  return (AmayaPage *)m_pNotebook->GetPage(position);
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaNormalWindow
 *      Method:  GetPage
 * Description:  how many page into the window
 *--------------------------------------------------------------------------------------
 */
int AmayaNormalWindow::GetPageCount() const
{
  if (!m_pNotebook)
    return 0;
  return (int)m_pNotebook->GetPageCount();
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaNormalWindow
 *      Method:  AppendMenu
 * Description:  TODO
 *--------------------------------------------------------------------------------------
 */
void AmayaNormalWindow::AppendMenu ( wxMenu * p_menu, const wxString & label )
{
  GetMenuBar()->Append( p_menu,
			label );
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaNormalWindow
 *      Method:  AppendMenuItem
 * Description:  TODO
 *--------------------------------------------------------------------------------------
 */
void AmayaNormalWindow::AppendMenuItem ( 
    wxMenu * 		p_menu_parent,
    long 		id,
    const wxString & 	label,
    const wxString & 	help,
    wxItemKind 		kind,
    const AmayaCParam & callback )
{
  p_menu_parent->Append(
     id,
     label,
     help,
     kind );
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaNormalWindow
 *      Method:  OnClose
 * Description:  just close the contained AmayaPage
 *--------------------------------------------------------------------------------------
 */
void AmayaNormalWindow::OnClose(wxCloseEvent& event)
{
  // remove the menu when the window is going to die
  DesactivateMenuBar();

  m_IsClosing = TRUE;

  // Ask the notebook to close its pages
  if (m_pNotebook)
    m_pNotebook->OnClose( event );

  // simulate a idle event to force the windows to be closed
  // (maybe a bug in wxWindow)
  wxIdleEvent idle_event;
  wxPostEvent(this, idle_event);

  m_IsClosing = FALSE;
  
  // reactivate the menubar if the windows is still alive
  ActivateMenuBar();

  // !! DO NOT SKIP THE EVENT !!
  // the event is skiped or vetoed into childs widgets (notebook) depending of document modification status
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaNormalWindow
 *      Method:  OnMenuItem
 * Description:  TODO
 *--------------------------------------------------------------------------------------
 */
void AmayaNormalWindow::OnMenuItem( wxCommandEvent& event )
{
  wxMenu * p_menu = (wxMenu *)event.GetEventObject();
  long     id     = event.GetId();
  
  wxMenuItem * p_menu_item = NULL;
  
  // try to find the menu item from the top level menubar (necessary for _WINDOWS)
  if (GetMenuBar())
    p_menu_item = GetMenuBar()->FindItem(id);
  // then try to find the menu item from the parent menu (necessary for popup menu)
  if (!p_menu_item)
    p_menu_item = p_menu->FindItem(id);
  
  if (!p_menu_item)
  {
    wxASSERT_MSG(FALSE,_T("Menu item doesnt existe"));
    return;
  }

  AmayaContext * p_context = (AmayaContext *)p_menu_item->GetRefData();
  if (p_context)
    {
      void * p_data = p_context->GetData();
      
      // call the generic callback
      CallMenuWX( (ThotWidget)p_menu_item, p_data );
    }
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaNormalWindow
 *      Method:  GetActivePage
 * Description:  return the current selected page
 *--------------------------------------------------------------------------------------
 */
AmayaPage * AmayaNormalWindow::GetActivePage() const
{
  wxLogDebug( _T("AmayaNormalWindow::GetActivePage") );
  if (!m_pNotebook)
    return NULL;
  return (m_pNotebook->GetSelection() >= 0) ? GetPage(m_pNotebook->GetSelection()) : NULL;
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaNormalWindow
 *      Method:  GetActiveFrame
 * Description:  return the current selected frame
 *--------------------------------------------------------------------------------------
 */
AmayaFrame * AmayaNormalWindow::GetActiveFrame() const
{
  wxLogDebug( _T("AmayaNormalWindow::GetActiveFrame") );

  AmayaPage * p_page = GetActivePage();
  if (p_page)
    return p_page->GetActiveFrame();
  else
    return NULL;
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaNormalWindow
 *      Method:  SetURL
 * Description:  set the current url value
 *--------------------------------------------------------------------------------------
 */
void AmayaNormalWindow::SetURL ( const wxString & new_url )
{
  if (m_pURLBar)
    m_pURLBar->SetValue( new_url );

  // Just select url
  //m_pURLBar->SetSelection( 0, new_url.Length() );
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaNormalWindow
 *      Method:  GetURL
 * Description:  get the current url value
 *--------------------------------------------------------------------------------------
 */
wxString AmayaNormalWindow::GetURL( )
{
  if (m_pURLBar)
    return m_pURLBar->GetValue();
  else
    return wxString(_T(""));
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaNormalWindow
 *      Method:  SetWindowEnableURL
 * Description:  set the current url status (enable or disable)
 *--------------------------------------------------------------------------------------
 */
void AmayaNormalWindow::SetEnableURL( bool urlenabled )
{
  if (m_pURLBar)
    m_pURLBar->Enable( urlenabled );
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaNormalWindow
 *      Method:  AppendURL
 * Description:  TODO
 *--------------------------------------------------------------------------------------
 */
void AmayaNormalWindow::AppendURL ( const wxString & new_url )
{
  if (m_pURLBar)
    m_pURLBar->Append( new_url );
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaNormalWindow
 *      Method:  EmptyURLBar
 * Description:  remove all items in the url bar
 *--------------------------------------------------------------------------------------
 */
void AmayaNormalWindow::EmptyURLBar()
{
  if (m_pURLBar)
    m_pURLBar->Clear();
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaNormalWindow
 *      Method:  SetMenuBar
 * Description:  override the wxFrame::SetMenuBar methode and check if the menu bar is NULL
 *               if NULL then replace the menubar with a dummy menu bar to avoid ugly effects when closing a frame
 *--------------------------------------------------------------------------------------
 */
void AmayaNormalWindow::SetMenuBar( wxMenuBar * p_menu_bar )
{
  if ( p_menu_bar )
    {
      wxFrame::SetMenuBar( p_menu_bar );

      // the menu need to be synchronized with FrameTable.EnabledMenus array
      TtaRefreshMenuStats( p_menu_bar );

      // create a dummy menu bar to avoid ugly effects when a frame is closed
      if ( m_pDummyMenuBar )
	delete m_pDummyMenuBar;
      m_pDummyMenuBar = new wxMenuBar();
      int menu_id = 0;
      wxMenu * p_menu = NULL;
      while ( menu_id < p_menu_bar->GetMenuCount() )
	{
	  p_menu = new wxMenu();
	  m_pDummyMenuBar->Append( p_menu , p_menu_bar->GetLabelTop(menu_id) );
	  menu_id++;
	}
    }
  else
    {
      // setup the dummy menubar
      wxFrame::SetMenuBar( m_pDummyMenuBar );

      // disable dummy menu entries
      // this should be done after SetMenuBar because on MSW it does'nt work before. 
      int menu_id = 0;
      while ( menu_id < m_pDummyMenuBar->GetMenuCount() )
	{
	  m_pDummyMenuBar->EnableTop(menu_id, FALSE);
	  menu_id++;
	}
    }
}


/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaNormalWindow
 *      Method:  DesactivateMenuBar
 * Description:  desactivate the current window menu bar
 *               (used when the windows is going to be closed)
 *--------------------------------------------------------------------------------------
 */
void AmayaNormalWindow::DesactivateMenuBar()
{
  SetMenuBar( NULL );
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaNormalWindow
 *      Method:  ActivateMenuBar
 * Description:  activate the menubar : look what is the current active frame and get
 *               its menu bar.
 *--------------------------------------------------------------------------------------
 */
void AmayaNormalWindow::ActivateMenuBar()
{
  /* this is only used into normal window */
  if ( GetKind() != WXAMAYAWINDOW_NORMAL )
    return;

  // TODO : aller chercher la bonne bar de menu
  
  // If the current windows is goind to die, do not activate the menubar
  if (m_IsClosing)
    return;

  Document doc;
  View     view;
  TtaGetActiveView( &doc, &view );
  
  if ( doc < 1 || doc > MAX_DOCUMENTS )
    return;

  int window_id;
  int page_id;
  int page_position;
  window_id = TtaGetDocumentWindowId( doc, view );
  TtaGetDocumentPageId( doc, view, &page_id, &page_position );

  AmayaWindow * p_window = WindowTable[window_id].WdWindow;

  if ( p_window != this )
    {
      // the active page is not into the current window
      // so exit
      return;
    }

  AmayaPage * p_AmayaPage = p_window->GetPage( page_id );

  if (!p_AmayaPage)
    return;

  AmayaFrame * p_AmayaFrame = p_AmayaPage->GetFrame( 1 /* TODO : indiquer la frame top ou bottom en fonction de la vue (view) */ );

  if (!p_AmayaFrame)
    return;

  SetMenuBar( p_AmayaFrame->GetMenuBar() );
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaNormalWindow
 *      Method:  GetAmayaToolBar
 * Description:  return the current toolbar
 *--------------------------------------------------------------------------------------
 */
AmayaToolBar * AmayaNormalWindow::GetAmayaToolBar()
{
  return m_pToolBar;
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaNormalWindow
 *      Method:  CleanUp
 * Description:  check that there is no empty pages
 *--------------------------------------------------------------------------------------
 */
void AmayaNormalWindow::CleanUp()
{
  int         page_id = 0;
  AmayaPage * p_page  = NULL;
  while ( page_id < GetPageCount() )
    {
      p_page = GetPage( page_id );
      if ( !p_page->GetFrame(1) && !p_page->GetFrame(2) )
	{
	  // the page do not have anymore frames !
	  // close it
	  p_page->Close();
	  m_pNotebook->DeletePage(page_id);
	  m_pNotebook->UpdatePageId();
	  TtaHandlePendingEvents ();
	}
      else
	page_id++;
    }



  // now check that notebook is not empty
  if (GetPageCount() == 0)
    Close();
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaNormalWindow
 *      Method:  OnMenuOpen
 * Description:  
 *--------------------------------------------------------------------------------------
 */
void AmayaNormalWindow::OnMenuOpen( wxMenuEvent& event )
{
  wxLogDebug( _T("AmayaNormalWindow::OnMenuOpen - menu_id=%d"), event.GetMenuId() );
  event.Skip();
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaNormalWindow
 *      Method:  OnMenuClose
 * Description:  
 *--------------------------------------------------------------------------------------
 */
void AmayaNormalWindow::OnMenuClose( wxMenuEvent& event )
{
  wxLogDebug( _T("AmayaNormalWindow::OnMenuClose - menu_id=%d"), event.GetMenuId() );
  event.Skip();
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaNormalWindow
 *      Method:  OnSplitterPosChanged
 * Description:  this method is called when the splitter position has changed
 *--------------------------------------------------------------------------------------
 */
void AmayaNormalWindow::OnSplitterPosChanged( wxSplitterEvent& event )
{
  wxLogDebug( _T("AmayaNormalWindow::OnSplitterPosChanged now = %d"), event.GetSashPosition() );
  m_SlashPos = event.GetSashPosition();
  //  event.Skip();
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaNormalWindow
 *      Method:  OnSplitterDClick
 * Description:  called when a double click is done on the splitbar
 *               detach the panel area (hide it)
 *--------------------------------------------------------------------------------------
 */
void AmayaNormalWindow::OnSplitterDClick( wxSplitterEvent& event )
{
  wxLogDebug( _T("AmayaNormalWindow::OnSplitterDClick") );
  m_pSplitterWindow->Unsplit( m_pPanel );
  m_pPanel->ShowWhenUnsplit( false );
  //  event.Skip();  
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaNormalWindow
 *      Method:  OnSplitPanelButton
 * Description:  this method is called when the button for quick split is pushed
 *--------------------------------------------------------------------------------------
 */
void AmayaNormalWindow::OnSplitPanelButton( wxCommandEvent& event )
{
  if ( event.GetId() != m_pSplitPanelButton->GetId() )
    {
      event.Skip();
      return;
    }

  wxLogDebug( _T("AmayaNormalWindow::OnSplitPanelButton") );

  if (!m_pSplitterWindow->IsSplit())
    OpenPanel();
  else
    ClosePanel();

  // do not skip this event because on windows, the callback is called twice
  //event.Skip();
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaNormalWindow
 *      Method:  ClosePanel
 * Description:  close the side panel
 *--------------------------------------------------------------------------------------
 */
void AmayaNormalWindow::ClosePanel()
{
  wxLogDebug( _T("AmayaNormalWindow::ClosePanel") );

  if (IsPanelOpened())
    {
      m_pSplitterWindow->Unsplit( m_pPanel );
      m_pPanel->ShowWhenUnsplit( false );

      // refresh the corresponding menu item state
      RefreshShowPanelToggleMenu();
    }
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaNormalWindow
 *      Method:  OpenPanel
 * Description:  open the side panel
 *--------------------------------------------------------------------------------------
 */
void AmayaNormalWindow::OpenPanel()
{
  wxLogDebug( _T("AmayaNormalWindow::OpenPanel") );
  
  if (!IsPanelOpened())
    {
      m_pSplitterWindow->SplitVertically( m_pPanel,
					  m_pNotebookPanel,
					  m_SlashPos ); 
      m_pPanel->ShowWhenUnsplit( true );

      // now check panels to know if a refresh is needed
      AmayaSubPanelManager::GetInstance()->CheckForDoUpdate();

      // refresh the corresponding menu item state
      RefreshShowPanelToggleMenu();
    }
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaNormalWindow
 *      Method:  IsPanelOpened
 * Description:  returns true if the side panel is opened
 *--------------------------------------------------------------------------------------
 */
bool AmayaNormalWindow::IsPanelOpened()
{
  return m_pSplitterWindow->IsSplit();
}


/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaNormalWindow
 *      Method:  GetAmayaPanel
 * Description:  return the window's panel (exists only on AmayaNormalWindow)
 *--------------------------------------------------------------------------------------
 */
AmayaPanel * AmayaNormalWindow::GetAmayaPanel() const
{
  return m_pPanel;
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaNormalWindow
 *      Method:  RefreshShowPanelToggleMenu
 * Description:  is called to toggle on/off the "Show/Hide panel" menu item depeding on
 *               the panel showing state.
 *--------------------------------------------------------------------------------------
 */
void AmayaNormalWindow::RefreshShowPanelToggleMenu()
{
  wxLogDebug( _T("AmayaNormalWindow::RefreshShowPanelToggleMenu") );
  

  // update menu items of each window's frames
  int         page_id = 0;
  AmayaPage * p_page  = NULL;
  int         frame_id = 0;
  Document document;
  View view;
  int menuID;
  int itemID;
  ThotBool on;
  while ( page_id < GetPageCount() )
    {
      p_page = GetPage( page_id );
      frame_id = p_page->GetFrame(1)->GetFrameId();
      if (frame_id <=0)
	{
	  wxASSERT_MSG(false,_T("this page exists without a frame ?"));
	  continue;
	}

      FrameToView (frame_id, &document, &view);
      menuID = FrameTable[frame_id].MenuShowPanelID;
      itemID = FrameTable[frame_id].MenuItemShowPanelID;
      on = IsPanelOpened();
      TtaSetToggleItem( document, view, menuID, itemID, on );

      page_id++;
    }
}

/*----------------------------------------------------------------------
 *  this is where the event table is declared
 *  the callbacks are assigned to an event type
 *----------------------------------------------------------------------*/
BEGIN_EVENT_TABLE(AmayaNormalWindow, AmayaWindow)
  EVT_MENU_OPEN(  AmayaNormalWindow::OnMenuOpen )
  EVT_MENU_CLOSE( AmayaNormalWindow::OnMenuClose )
  EVT_MENU( -1,   AmayaNormalWindow::OnMenuItem ) 
  EVT_CLOSE(      AmayaNormalWindow::OnClose )

  EVT_SPLITTER_SASH_POS_CHANGED( -1, 	AmayaNormalWindow::OnSplitterPosChanged )
  EVT_SPLITTER_DCLICK( -1, 		AmayaNormalWindow::OnSplitterDClick )

  EVT_BUTTON( -1,                       AmayaNormalWindow::OnSplitPanelButton)
END_EVENT_TABLE()

#endif /* #ifdef _WX */
