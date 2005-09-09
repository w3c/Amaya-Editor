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
#include "message_wx.h"
#include "libmsg.h"
#include "frame.h"
#include "logdebug.h"

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "edit_tv.h"
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
#include "AmayaParams.h"
#include "appdialogue_wx_f.h"
#include "displayview_f.h"
#include "appdialogue_wx.h"

#include "AmayaNormalWindow.h"
#include "AmayaPanel.h"
#include "AmayaNotebook.h"
#include "AmayaPage.h"
#include "AmayaFrame.h"
#include "AmayaCallback.h"
#include "AmayaToolBar.h"
#include "AmayaQuickSplitButton.h"
#include "AmayaSubPanelManager.h"
#include "AmayaStatusBar.h"

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
  AmayaWindow( window_id, p_parent_window, pos, size, WXAMAYAWINDOW_NORMAL )
{
  // initialize default slashbar position
  TtaSetEnvInt("SLASH_PANEL_POS", 195, FALSE);
  // load slash position from registry
  TtaGetEnvInt ("SLASH_PANEL_POS", &m_SlashPos);

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


  // Create the notebook
  m_pNotebook                              = new AmayaNotebook( m_pNotebookPanel, this );
  // Create a sizer to layout the notebook in the panel
  wxBoxSizer * p_NotebookSizer             = new wxBoxSizer ( wxHORIZONTAL );
  p_NotebookSizer->Add(m_pNotebook, 1, wxEXPAND | wxALL, 0);
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
  // do not split the panel by default
  m_pSplitterWindow->Unsplit( m_pPanel );

  // Creation of frame sizer to contains differents frame areas
  wxBoxSizer * p_SizerFrame = new wxBoxSizer ( wxHORIZONTAL );

  // create the quick split button used to show/hide the panel
  m_pSplitPanelButton = new AmayaQuickSplitButton( p_TopParent, AmayaQuickSplitButton::wxAMAYA_QS_VERTICAL, 4 );
  p_SizerFrame->Add( m_pSplitPanelButton, 0, wxALL | wxEXPAND, 0 );
  m_pSplitPanelButton->ShowQuickSplitButton( true );
  
  // add the splitter window to the top sizer : panel + notebook
  p_SizerFrame->Add( m_pSplitterWindow, 1, wxALL | wxEXPAND, 0 );


  // Creation of the top sizer to contain toolbar and framesizer
  wxBoxSizer * p_TopLayoutSizer = new wxBoxSizer ( wxVERTICAL );
  p_TopLayoutSizer->Add( p_SizerFrame, 1, wxALL | wxEXPAND, 0 );
  p_TopParent->SetSizer(p_TopLayoutSizer);
  //  p_TopLayoutSizer->Fit(p_TopParent);

  // Create the toolbar
  m_pToolBar = new AmayaToolBar( p_TopParent, this );
  p_TopLayoutSizer->Prepend( m_pToolBar, 0, wxALL | wxEXPAND, 1 );

  // Creation of the statusbar
  m_pStatusBar = new AmayaStatusBar(this);
  SetStatusBar(m_pStatusBar);
  
  SetAutoLayout(TRUE);
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
  delete m_pStatusBar;
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaNormalWindow
 *      Method:  GetAmayaStatusBar
 * Description:  
 *--------------------------------------------------------------------------------------
 */
AmayaStatusBar * AmayaNormalWindow::GetAmayaStatusBar()
{
  return m_pStatusBar;
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
  AmayaPage * p_page = new AmayaPage( m_pNotebook, this );
  
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

    // update the pages ids
    m_pNotebook->UpdatePageId();

    // the inserted page should be forced to notebook size
    m_pNotebook->Layout();
    TTALOGDEBUG_2( TTA_LOG_DIALOG, _T("AmayaNormalWindow::AttachPage - pagesize: w=%d h=%d"),
		p_page->GetSize().GetWidth(),
		p_page->GetSize().GetHeight());

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
  bool dummy = false;
  AmayaPage * p_page  = NULL;
  int old_page_id = m_pNotebook->GetSelection(); 
  p_page = GetPage( page_id );
  // close it
  p_page->DoClose(dummy);
  if (p_page->IsClosed())
    {
      m_pNotebook->DeletePage(page_id);
      m_pNotebook->UpdatePageId();
      //TtaHandlePendingEvents ();
      
      // we force old_page_id to regivefocus to the last active page before the close
      old_page_id--; // a page has been deleted so the old page position should be -1
      if (old_page_id < 0)
	old_page_id = 0;
      if (old_page_id >= (int)m_pNotebook->GetPageCount())
	old_page_id = m_pNotebook->GetPageCount()-1;
      if (old_page_id >= 0)
	m_pNotebook->SetSelection( old_page_id );

      // here GetSelection should return the right value acording to SetSelection above.
      if (m_pNotebook->GetSelection() >= 0)
	{
	  AmayaPage * p_selected_page = (AmayaPage *)m_pNotebook->GetPage(m_pNotebook->GetSelection());
	  if (p_selected_page)
	    {
	      p_selected_page->SetSelected( TRUE );
	      
	      // try to avoid refresh because it forces a total canvas redraw (it's not very optimized)
	      // the page need a refresh to repaint its canvas
	      //p_selected_page->Refresh();
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
  TTALOGDEBUG_0( TTA_LOG_DIALOG, _T("AmayaNormalWindow::GetPage") );
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
 *      Method:  DoClose
 * Description:  close every pages contained by the notebook
 *--------------------------------------------------------------------------------------
 */
void AmayaNormalWindow::DoClose(bool & veto)
{
  m_IsClosing = TRUE;

  // Ask the notebook to close its pages
  if (m_pNotebook)
    m_pNotebook->DoClose( veto );

  // simulate a idle event to force the windows to be closed
  // (maybe a bug in wxWindow)
  wxIdleEvent idle_event;
  wxPostEvent(this, idle_event);

  m_IsClosing = FALSE;
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaNormalWindow
 *      Method:  OnMenuItem
 * Description:  this method is called when a menu item is pressed
 *               it will generate an event to differe the menu item action
 *--------------------------------------------------------------------------------------
 */
void AmayaNormalWindow::OnMenuItem( wxCommandEvent& event )
{
  wxMenu * p_menu = (wxMenu *)event.GetEventObject();
  long     id     = event.GetId();
  int action_id   = -1;

  action_id = FindMenuActionFromMenuItemID (DocumentMenuList, id);
  // Todo: tester si on fait ttcCopy ttcpaste ttccut sur un widget
#ifdef IV
#if defined(_WINDOWS) || defined(_UNIX)
  /* do not allow CTRL-C CTRL-X CTRL-V in "text" widgets */
  wxWindow *       p_win_focus         = wxWindow::FindFocus();
  wxTextCtrl *     p_text_ctrl         = wxDynamicCast(p_win_focus, wxTextCtrl);
  wxComboBox *     p_combo_box         = wxDynamicCast(p_win_focus, wxComboBox);
  wxSpinCtrl *     p_spinctrl          = wxDynamicCast(p_win_focus, wxSpinCtrl);
  if (( p_text_ctrl || p_combo_box || p_spinctrl ) && (event.CmdDown() &&
          (thot_keysym == 'C' || thot_keysym == 'X' || thot_keysym == 'V' ||
           thot_keysym == 'c' || thot_keysym == 'x' || thot_keysym == 'v')) )
    {
      event.Skip();
      return true;      
    }
#endif /* _WINDOWS */
#endif /* IV */

  TTALOGDEBUG_2( TTA_LOG_DIALOG, _T("AmayaNormalWindow::OnMenuItem id=%d action_id=%d"), id, action_id );
  
  /* if this menu is the context menu it's possible that the current active document is not the wanted one */
  wxMenu *   p_context_menu = TtaGetContextMenu( GetWindowId() );
  Document   doc;
  View       view;
  if (p_menu && p_menu == p_context_menu)
    FrameToView (m_pNotebook->GetMContextFrame(), &doc, &view);
  else
    FrameToView (TtaGiveActiveFrame(), &doc, &view);
  AmayaWindow::DoAmayaAction( action_id, doc, view );
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
  TTALOGDEBUG_0( TTA_LOG_DIALOG, _T("AmayaNormalWindow::GetActivePage") );
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
  TTALOGDEBUG_0( TTA_LOG_DIALOG, _T("AmayaNormalWindow::GetActiveFrame") );

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
  if (m_pToolBar)
    m_pToolBar->SetURLValue( new_url );
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
  if (m_pToolBar)
    return m_pToolBar->GetURLValue();
  else
    return wxString(_T(""));
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
  if (m_pToolBar)
    m_pToolBar->AppendURL( new_url );
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
  if (m_pToolBar)
    m_pToolBar->ClearURL();
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
  bool dummy = false;
  int         page_id = 0;
  AmayaPage * p_page  = NULL;
  while ( page_id < GetPageCount() )
    {
      p_page = GetPage( page_id );
      if ( !p_page->GetFrame(1) && !p_page->GetFrame(2) )
	{
	  // the page do not have anymore frames !
	  // close it
	  p_page->DoClose(dummy);
	  m_pNotebook->DeletePage(page_id);
	  m_pNotebook->UpdatePageId();
	  TtaHandlePendingEvents ();
	}
      else
	page_id++;
    }

  // now check that notebook is not empty
  if (GetPageCount() == 0)
    DoClose(dummy);
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
  TTALOGDEBUG_1( TTA_LOG_DIALOG, _T("AmayaNormalWindow::OnMenuOpen - menu_id=%d"), event.GetMenuId() );
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
  TTALOGDEBUG_1( TTA_LOG_DIALOG, _T("AmayaNormalWindow::OnMenuClose - menu_id=%d"), event.GetMenuId() );
  event.Skip();
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaNormalWindow
 *      Method:  OnMenuHighlight
 * Description:  
 *--------------------------------------------------------------------------------------
 */
void AmayaNormalWindow::OnMenuHighlight( wxMenuEvent& event )
{
  TTALOGDEBUG_1( TTA_LOG_DIALOG, _T("AmayaNormalWindow::OnMenuHighlight - menu_id=%d"), event.GetMenuId() );
  AmayaFrame * p_frame = GetActiveFrame();
  if (p_frame)
    p_frame->RefreshStatusBarText();
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
  TTALOGDEBUG_1( TTA_LOG_DIALOG, _T("AmayaNormalWindow::OnSplitterPosChanged now = %d"), event.GetSashPosition() );
  m_SlashPos = event.GetSashPosition();

  // save slash position into registry 
  TtaSetEnvInt("SLASH_PANEL_POS", m_SlashPos, TRUE);

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
  TTALOGDEBUG_0( TTA_LOG_DIALOG, _T("AmayaNormalWindow::OnSplitterDClick") );
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

  TTALOGDEBUG_0( TTA_LOG_DIALOG, _T("AmayaNormalWindow::OnSplitPanelButton") );

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
  TTALOGDEBUG_0( TTA_LOG_PANELS, _T("AmayaNormalWindow::ClosePanel") );

  if (IsPanelOpened())
    {
      m_pSplitterWindow->Unsplit( m_pPanel );
      m_pPanel->ShowWhenUnsplit( false );

      // refresh the corresponding menu item state
      RefreshShowPanelToggleMenu();

      TtaSetEnvBoolean("OPEN_PANEL", IsPanelOpened(), TRUE);
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
  TTALOGDEBUG_0( TTA_LOG_PANELS, _T("AmayaNormalWindow::OpenPanel") );
  
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

      TtaSetEnvBoolean("OPEN_PANEL", IsPanelOpened(), TRUE);
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
  TTALOGDEBUG_0( TTA_LOG_DIALOG, _T("AmayaNormalWindow::RefreshShowPanelToggleMenu") );

  // update menu items of each documents
  int doc_id    = 1;
  int frame_id  = 0;
  int window_id = GetWindowId();
  int itemID    = WindowTable[window_id].MenuItemShowPanelID;
  int action    = FindMenuActionFromMenuItemID(NULL, itemID);
  ThotBool on   = IsPanelOpened();
    
  while ( action >= 0 && doc_id < MAX_DOCUMENTS )
    {
      if (LoadedDocument[doc_id-1])
        {
          frame_id = LoadedDocument[doc_id-1]->DocViewFrame[0];
          if (FrameTable[frame_id].FrWindowId == window_id)
            {
              /* toggle the menu item of every documents */
              MenuActionList[action].ActionToggle[doc_id] = on;
              TtaRefreshMenuItemStats( doc_id, NULL, itemID );
            }
        }
      doc_id++;
    }
}

/*----------------------------------------------------------------------
 *  this is where the event table is declared
 *  the callbacks are assigned to an event type
 *----------------------------------------------------------------------*/
BEGIN_EVENT_TABLE(AmayaNormalWindow, AmayaWindow)
  EVT_MENU_OPEN(  AmayaNormalWindow::OnMenuOpen )
  EVT_MENU_CLOSE( AmayaNormalWindow::OnMenuClose )
  EVT_MENU_HIGHLIGHT_ALL( AmayaNormalWindow::OnMenuHighlight )
  EVT_MENU( -1,   AmayaNormalWindow::OnMenuItem ) 
  //  EVT_CLOSE(      AmayaNormalWindow::OnClose )

  EVT_SPLITTER_SASH_POS_CHANGED( -1, 	AmayaNormalWindow::OnSplitterPosChanged )
  EVT_SPLITTER_DCLICK( -1, 		AmayaNormalWindow::OnSplitterDClick )

  EVT_BUTTON( -1,                       AmayaNormalWindow::OnSplitPanelButton)
END_EVENT_TABLE()

#endif /* #ifdef _WX */
