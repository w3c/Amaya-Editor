#ifdef _WX

#include "wx/wx.h"
#include "wx/tglbtn.h"
#include "wx/string.h"
#include "wx/spinctrl.h"

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
#include "input_f.h"

#include "AmayaNormalWindow.h"
#include "AmayaPanel.h"
#include "AmayaNotebook.h"
#include "AmayaPage.h"
#include "AmayaFrame.h"
#include "AmayaCallback.h"
#include "AmayaToolBar.h"
#include "AmayaQuickSplitButton.h"
#include "AmayaStatusBar.h"

IMPLEMENT_CLASS(AmayaNormalWindow, AmayaWindow)

  /*----------------------------------------------------------------------
   *       Class:  AmayaNormalWindow
   *      Method:  AmayaNormalWindow
   * Description:  create a new AmayaNormalWindow
   -----------------------------------------------------------------------*/
  AmayaNormalWindow::AmayaNormalWindow ( wxWindow * parent, wxWindowID id
                                        ,const wxPoint& pos
                                        ,const wxSize&  size
                                        ,int kind
                                        ) : 
    AmayaWindow( parent, id, pos, size, kind ),
    m_pStatusBar(NULL)
{
  // initialize default slashbar position
  TtaSetEnvInt("SLASH_PANEL_POS", 195, FALSE);
  // load slash position from registry
  TtaGetEnvInt ("SLASH_PANEL_POS", &m_SlashPos);

  
  
  // Create a background panel to contain everything : better look on windows
  wxBoxSizer * p_TopSizer = new wxBoxSizer ( wxVERTICAL );
  
  {
    // Create the toolbar
    m_pToolBar = new AmayaToolBar( this, this );
    p_TopSizer->Add( m_pToolBar, 0, wxEXPAND);
    
    // Global layout
    m_pLayoutSizer = new wxBoxSizer(wxHORIZONTAL);
    {
      // create the quick split button used to show/hide the panel
      m_pSplitPanelButton = new AmayaQuickSplitButton(this, wxID_ANY, AmayaQuickSplitButton::wxAMAYA_QS_TOOLS);
      m_pLayoutSizer->Add(m_pSplitPanelButton, 0, wxEXPAND);
      
      // Create a splitted vertical window
      m_pSplitterWindow = new wxSplitterWindow( this, wxID_ANY,
                                                wxDefaultPosition, wxDefaultSize,
                                                wxSP_3DBORDER | wxSP_3DSASH | wxSP_3D /*| wxSP_PERMIT_UNSPLIT*/ );
      m_pSplitterWindow->SetMinimumPaneSize( 100 );
      m_pLayoutSizer->Add(m_pSplitterWindow, 1, wxEXPAND);
      {
        // Create a AmayaPanel to contains commands shortcut
        m_pPanel = new AmayaToolPanelBar( m_pSplitterWindow, -1, wxDefaultPosition, wxDefaultSize,
                                   wxTAB_TRAVERSAL
                                   | wxRAISED_BORDER
                                   | wxCLIP_CHILDREN );
        // Create the notebook
        m_pNotebook = new AmayaNotebook( m_pSplitterWindow, wxID_ANY );

        ThotBool panel_opened;
        TtaGetEnvBoolean ("OPEN_PANEL", &panel_opened);
        OpenPanel();
        if(!panel_opened || kind == WXAMAYAWINDOW_ANNOT || kind == WXAMAYAWINDOW_CSS)
          ClosePanel();
      }
      
    }
    p_TopSizer->Add(m_pLayoutSizer, 1, wxEXPAND);
  }  
  // Set the global top sizer
  SetSizer(p_TopSizer);
  
  
  // Creation of the statusbar
  m_pStatusBar = new AmayaStatusBar(this);
  SetStatusBar(m_pStatusBar);
  
  SetAutoLayout(TRUE);
}

/*----------------------------------------------------------------------
 *       Class:  AmayaNormalWindow
 *      Method:  ~AmayaNormalWindow
 * Description:  destructor
 -----------------------------------------------------------------------*/
AmayaNormalWindow::~AmayaNormalWindow()
{
  int window_id = GetWindowId();

  if (window_id == 1)
    TtaSetEnvBoolean("OPEN_PANEL", IsPanelOpened(), TRUE);
  SetAutoLayout(FALSE);
}

/*----------------------------------------------------------------------
 *       Class:  AmayaNormalWindow
 *      Method:  SetPageIcon
 * Description:  
 -----------------------------------------------------------------------*/
void AmayaNormalWindow::SetPageIcon(int page_id, char *iconpath)
{
  int index = TtaGetIconIndex (iconpath);
  if (m_pNotebook && index > 0)
    m_pNotebook->SetPageImage (page_id, index);
}

/*----------------------------------------------------------------------
 *       Class:  AmayaNormalWindow
 *      Method:  GetAmayaStatusBar
 * Description:  
 -----------------------------------------------------------------------*/
AmayaStatusBar * AmayaNormalWindow::GetAmayaStatusBar()
{
  return m_pStatusBar;
}

/*----------------------------------------------------------------------
 *       Class:  AmayaNormalWindow
 *      Method:  CreatePage
 * Description:  create a new AmayaPage, the notebook will be the parent page
 *               it's possible to attach automaticaly this page to the window or not
 -----------------------------------------------------------------------*/
AmayaPage * AmayaNormalWindow::CreatePage( bool attach, int position )
{
  AmayaPage * p_page = new AmayaPage( m_pNotebook, this );
  
  if (attach)
    AttachPage( position, p_page );
  
  return p_page;
}

/*----------------------------------------------------------------------
 *       Class:  AmayaNormalWindow
 *      Method:  AttachPage
 * Description:  really attach a page to the current window
 -----------------------------------------------------------------------*/
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

/*----------------------------------------------------------------------
 *       Class:  AmayaNormalWindow
 *      Method:  DetachPage
 * Description:  
 -----------------------------------------------------------------------*/
bool AmayaNormalWindow::DetachPage( int position )
{
  return false;
}

/*----------------------------------------------------------------------
 *       Class:  AmayaNormalWindow
 *      Method:  ClosePage
 * Description:  ferme une page
 -----------------------------------------------------------------------*/
bool AmayaNormalWindow::ClosePage( int page_id )
{
  // flush all pending events
  wxTheApp->Yield();

  if (m_pNotebook == NULL)
    return true;
  else
  {
    if(m_pNotebook->ClosePage(page_id))
    {
      /** \todo Test if no more page is present on the window.*/
      return true;
    }
    else
      return false;
  }
}

/*----------------------------------------------------------------------
 *       Class:  AmayaNormalWindow
 *      Method:  CloseAllButPage
 * Description:  ferme toutes les pages sauf une
 -----------------------------------------------------------------------*/
bool AmayaNormalWindow::CloseAllButPage( int position )
{
  if(m_pNotebook)
    return m_pNotebook->CloseAllButPage(position);
  else
    return false;
}

/*----------------------------------------------------------------------
 *       Class:  AmayaNormalWindow
 *      Method:  GetPage
 * Description:  search the page at given position
 -----------------------------------------------------------------------*/
AmayaPage * AmayaNormalWindow::GetPage( int position ) const
{
  TTALOGDEBUG_0( TTA_LOG_DIALOG, _T("AmayaNormalWindow::GetPage") );
  if (!m_pNotebook)
    return NULL;
  if (GetPageCount() <= position)
    return NULL;
  return (AmayaPage *)m_pNotebook->GetPage(position);
}

/*----------------------------------------------------------------------
 *       Class:  AmayaNormalWindow
 *      Method:  GetPage
 * Description:  how many page into the window
 -----------------------------------------------------------------------*/
int AmayaNormalWindow::GetPageCount() const
{
  if (!m_pNotebook)
    return 0;
  return (int)m_pNotebook->GetPageCount();
}



/*----------------------------------------------------------------------
 *       Class:  AmayaNormalWindow
 *      Method:  OnClose
 * Description:  Intercept the CLOSE event and prevent it if ncecessary.
  -----------------------------------------------------------------------*/
void AmayaNormalWindow::OnClose(wxCloseEvent& event)
{
  if(m_pNotebook)
  {
    // Intend to close the notebook
    if(!m_pNotebook->Close())
    {
      event.Veto();
      return;
    }
    m_pNotebook->Destroy();
    m_pNotebook = NULL;
  }
  Destroy();
}


/*----------------------------------------------------------------------
 *       Class:  AmayaNormalWindow
 *      Method:  OnMenuItem
 * Description:  this method is called when a menu item is pressed
 *               it will generate an event to differe the menu item action
 -----------------------------------------------------------------------*/
void AmayaNormalWindow::OnMenuItem( wxCommandEvent& event )
{
  wxMenu * p_menu = (wxMenu *)event.GetEventObject();
  long     id     = event.GetId();
  int action_id   = -1;

  action_id = FindMenuActionFromMenuItemID (DocumentMenuList, id);
  /* do not allow CTRL-C CTRL-X CTRL-V in "text" widgets */
  wxWindow *       p_win_focus         = wxWindow::FindFocus();
  wxTextCtrl *     p_text_ctrl         = wxDynamicCast(p_win_focus, wxTextCtrl);
  wxComboBox *     p_combo_box         = wxDynamicCast(p_win_focus, wxComboBox);
  wxSpinCtrl *     p_spinctrl          = wxDynamicCast(p_win_focus, wxSpinCtrl);
  if (( p_text_ctrl || p_combo_box || p_spinctrl ) &&
      action_id >= 0 && action_id < MaxMenuAction && 
      MenuActionList[action_id].ActionName)
    {
      if (p_text_ctrl)
        {
          if (!strcmp (MenuActionList[action_id].ActionName, "TtcCutSelection"))
            {
              p_text_ctrl->Cut();
              return;
            }
          else if (!strcmp (MenuActionList[action_id].ActionName, "TtcCopySelection"))
            {
              p_text_ctrl->Copy();
              return;
            }
          else if (!strcmp (MenuActionList[action_id].ActionName, "PasteBuffer"))
            {
              p_text_ctrl->Paste();
              return;
            }
          else if (!strcmp (MenuActionList[action_id].ActionName, "TtcUndo"))
            {
              p_text_ctrl->Undo();
              return;
            }
          else if (!strcmp (MenuActionList[action_id].ActionName, "TtcRedo"))
            {
              p_text_ctrl->Redo();
              return;
            }
        }
      else if (p_combo_box)
        {
          if (!strcmp (MenuActionList[action_id].ActionName, "TtcCutSelection"))
            {
              p_combo_box->Cut();
              return;
            }
          else if (!strcmp (MenuActionList[action_id].ActionName, "TtcCopySelection"))
            {
              p_combo_box->Copy();
              return;
            }
          else if (!strcmp (MenuActionList[action_id].ActionName, "PasteBuffer"))
            {
              p_combo_box->Paste();
              return;
            }
          else if (!strcmp (MenuActionList[action_id].ActionName, "TtcUndo"))
            {
              p_combo_box->Undo();
              return;
            }
          else if (!strcmp (MenuActionList[action_id].ActionName, "TtcRedo"))
            {
              p_combo_box->Redo();
              return;
            }
        }
      else
        {
          event.Skip();
          return;
        }
    }

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

/*----------------------------------------------------------------------
 *       Class:  AmayaNormalWindow
 *      Method:  GetActivePage
 * Description:  return the current selected page
 -----------------------------------------------------------------------*/
AmayaPage * AmayaNormalWindow::GetActivePage() const
{
  TTALOGDEBUG_0( TTA_LOG_DIALOG, _T("AmayaNormalWindow::GetActivePage") );
  if (!m_pNotebook)
    return NULL;
  return (m_pNotebook->GetSelection() >= 0) ? GetPage(m_pNotebook->GetSelection()) : NULL;
}

/*----------------------------------------------------------------------
 *       Class:  AmayaNormalWindow
 *      Method:  GetActiveFrame
 * Description:  return the current selected frame
 -----------------------------------------------------------------------*/
AmayaFrame * AmayaNormalWindow::GetActiveFrame() const
{
  TTALOGDEBUG_0( TTA_LOG_DIALOG, _T("AmayaNormalWindow::GetActiveFrame") );

  AmayaPage * p_page = GetActivePage();
  if (p_page)
    return p_page->GetActiveFrame();
  else
    return NULL;
}

/*----------------------------------------------------------------------
 *       Class:  AmayaNormalWindow
 *      Method:  SetURL
 * Description:  set the current url value
 -----------------------------------------------------------------------*/
void AmayaNormalWindow::SetURL ( const wxString & new_url )
{
  // do not update window url if the url is empty
  //if (new_url.IsEmpty() )
  //  return;

  if (m_pToolBar)
    m_pToolBar->SetURLValue( new_url );
}

/*----------------------------------------------------------------------
 *       Class:  AmayaNormalWindow
 *      Method:  GetURL
 * Description:  get the current url value
 -----------------------------------------------------------------------*/
wxString AmayaNormalWindow::GetURL( )
{
  if (m_pToolBar)
    return m_pToolBar->GetURLValue();
  else
    return wxString(_T(""));
}

/*----------------------------------------------------------------------
 *       Class:  AmayaNormalWindow
 *      Method:  AppendURL
 * Description:  TODO
 -----------------------------------------------------------------------*/
void AmayaNormalWindow::AppendURL ( const wxString & new_url )
{
  if (m_pToolBar)
    m_pToolBar->AppendURL( new_url );
}

/*----------------------------------------------------------------------
 *       Class:  AmayaNormalWindow
 *      Method:  EmptyURLBar
 * Description:  remove all items in the url bar
 -----------------------------------------------------------------------*/
void AmayaNormalWindow::EmptyURLBar()
{
  if (m_pToolBar)
    m_pToolBar->ClearURL();
}

/*----------------------------------------------------------------------
 *       Class:  AmayaNormalWindow
 *      Method:  GetAmayaToolBar
 * Description:  return the current toolbar
 -----------------------------------------------------------------------*/
AmayaToolBar * AmayaNormalWindow::GetAmayaToolBar()
{
  return m_pToolBar;
}


/*----------------------------------------------------------------------
 *       Class:  AmayaNormalWindow
 *      Method:  CleanUp
 * Description:  check that there is no empty pages
 -----------------------------------------------------------------------*/
void AmayaNormalWindow::CleanUp()
{
  if(m_pNotebook)
    m_pNotebook->CleanUp();

  if(GetPageCount()==0)
    Close();
}

/*----------------------------------------------------------------------
 *       Class:  AmayaNormalWindow
 *      Method:  OnMenuOpen
 * Description:  
 -----------------------------------------------------------------------*/
#ifdef __WXDEBUG__
void AmayaNormalWindow::OnMenuOpen( wxMenuEvent& event )
{
  TTALOGDEBUG_1( TTA_LOG_DIALOG, _T("AmayaNormalWindow::OnMenuOpen - menu_id=%d"), event.GetMenuId() );
  event.Skip();
}
#endif /* __WXDEBUG__ */

/*----------------------------------------------------------------------
 *       Class:  AmayaNormalWindow
 *      Method:  OnMenuClose
 * Description:  
 -----------------------------------------------------------------------*/
#ifdef __WXDEBUG__
void AmayaNormalWindow::OnMenuClose( wxMenuEvent& event )
{
  TTALOGDEBUG_1( TTA_LOG_DIALOG, _T("AmayaNormalWindow::OnMenuClose - menu_id=%d"), event.GetMenuId() );
  event.Skip();
}
#endif /* __WXDEBUG__ */

/*----------------------------------------------------------------------
 *       Class:  AmayaNormalWindow
 *      Method:  OnMenuHighlight
 * Description:  
 -----------------------------------------------------------------------*/
void AmayaNormalWindow::OnMenuHighlight( wxMenuEvent& event )
{
  TTALOGDEBUG_1( TTA_LOG_DIALOG, _T("AmayaNormalWindow::OnMenuHighlight - menu_id=%d"), event.GetMenuId() );
  AmayaFrame * p_frame = GetActiveFrame();
  if (p_frame)
    p_frame->RefreshStatusBarText();
}

/*----------------------------------------------------------------------
 *       Class:  AmayaNormalWindow
 *      Method:  OnSplitterPosChanged
 * Description:  this method is called when the splitter position has changed
 -----------------------------------------------------------------------*/
void AmayaNormalWindow::OnSplitterPosChanged( wxSplitterEvent& event )
{
  TTALOGDEBUG_1( TTA_LOG_DIALOG, _T("AmayaNormalWindow::OnSplitterPosChanged now = %d"), event.GetSashPosition() );
  
  if(!strcmp(TtaGetEnvString("TOOLPANEL_LAYOUT"), "LEFT"))
    {
      m_SlashPos = event.GetSashPosition();
    }
  else
    {
      m_SlashPos = m_pSplitterWindow->GetSize().x - event.GetSashPosition();
    }

  // save slash position into registry 
  TtaSetEnvInt("SLASH_PANEL_POS", m_SlashPos, TRUE);
  //  event.Skip();
}

/*----------------------------------------------------------------------
 *       Class:  AmayaNormalWindow
 *      Method:  OnSplitterDClick
 * Description:  called when a double click is done on the splitbar
 *               detach the panel area (hide it)
 -----------------------------------------------------------------------*/
void AmayaNormalWindow::OnSplitterDClick( wxSplitterEvent& event )
{
  TTALOGDEBUG_0( TTA_LOG_DIALOG, _T("AmayaNormalWindow::OnSplitterDClick") );
  m_pSplitterWindow->Unsplit( m_pPanel );
}

/*----------------------------------------------------------------------
 *       Class:  AmayaNormalWindow
 *      Method:  OnSplitPanelButton
 * Description:  this method is called when the button for quick split is pushed
 -----------------------------------------------------------------------*/
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

/*----------------------------------------------------------------------
 *       Class:  AmayaNormalWindow
 *      Method:  ClosePanel
 * Description:  close the side panel
 -----------------------------------------------------------------------*/
void AmayaNormalWindow::ClosePanel()
{
  int window_id = GetWindowId();
  TTALOGDEBUG_0( TTA_LOG_PANELS, _T("AmayaNormalWindow::ClosePanel") );

  if (IsPanelOpened())
    {
      m_pSplitterWindow->Unsplit( m_pPanel );

      // refresh the corresponding menu item state
      RefreshShowPanelToggleMenu();

      if (window_id == 1)
        TtaSetEnvBoolean("OPEN_PANEL", IsPanelOpened(), TRUE);
    }
}

/*----------------------------------------------------------------------
 *       Class:  AmayaNormalWindow
 *      Method:  OpenPanel
 * Description:  open the side panel
 -----------------------------------------------------------------------*/
void AmayaNormalWindow::OpenPanel()
{
  int window_id = GetWindowId();
  TTALOGDEBUG_0( TTA_LOG_PANELS, _T("AmayaNormalWindow::OpenPanel") );
  
  if (!IsPanelOpened())
    {
      if(!strcmp(TtaGetEnvString("TOOLPANEL_LAYOUT"), "LEFT"))
        {
          // Open tool panel bar at left.
          m_pSplitterWindow->SplitVertically( m_pPanel,
                                              m_pNotebook,
                                              m_SlashPos );
          m_pLayoutSizer->Show(m_pSplitPanelButton, true);
        }
      else
        {
          // Open tool panel bar at right.
          m_pSplitterWindow->SplitVertically( m_pNotebook,
                                              m_pPanel,
                                              m_pSplitterWindow->GetSize().x-m_SlashPos);
          m_pLayoutSizer->Show(m_pSplitPanelButton, false);
          
        }
      m_pPanel->Layout();
      // refresh the corresponding menu item state
      RefreshShowPanelToggleMenu();

      if (window_id == 1)
        TtaSetEnvBoolean("OPEN_PANEL", IsPanelOpened(), TRUE);
    }
}

/*----------------------------------------------------------------------
 *       Class:  AmayaNormalWindow
 *      Method:  IsPanelOpened
 * Description:  returns true if the side panel is opened
 -----------------------------------------------------------------------*/
bool AmayaNormalWindow::IsPanelOpened()
{
  return m_pSplitterWindow->IsSplit();
}


/*----------------------------------------------------------------------
 *       Class:  AmayaNormalWindow
 *      Method:  GetAmayaPanel
 * Description:  return the window's panel (exists only on AmayaNormalWindow)
 -----------------------------------------------------------------------*/
AmayaToolPanelBar * AmayaNormalWindow::GetToolPanelBar() const
{
  return m_pPanel;
}


/*----------------------------------------------------------------------
 *       Class:  AmayaNormalWindow
 *      Method:  RefreshToolPanelBar
 * Description:  Intend to change the layout of tool bar panel
 -----------------------------------------------------------------------*/
void AmayaNormalWindow::RefreshToolPanelBar()
{
  ClosePanel();
  OpenPanel();
}

/*----------------------------------------------------------------------
 *       Class:  AmayaNormalWindow
 *      Method:  RefreshShowPanelToggleMenu
 * Description:  is called to toggle on/off the "Show/Hide panel" menu item depeding on
 *               the panel showing state.
 -----------------------------------------------------------------------*/
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
 *       Class:  AmayaNormalWindow
 *      Method:  OnNotebookPageChanged
 * Description:  is called when the notebook changes of page.
 -----------------------------------------------------------------------*/
void AmayaNormalWindow::OnNotebookPageChanged( wxNotebookEvent& event )
{
  AmayaStatusBar* status = GetAmayaStatusBar();
  if(status){
    Document   doc;
    View       view;
    FrameToView (TtaGiveActiveFrame(), &doc, &view);
    Element elem = 0;
    int first, last;
    if(doc)
      TtaGiveFirstSelectedElement(doc, &elem, &first, &last);
    if(elem)
      status->SetSelectedElement(elem);
    else
      status->SetSelectedElement(NULL);
  }
}


/*----------------------------------------------------------------------
 *  this is where the event table is declared
 *  the callbacks are assigned to an event type
 *----------------------------------------------------------------------*/
BEGIN_EVENT_TABLE(AmayaNormalWindow, AmayaWindow)
#ifdef __WXDEBUG__
  EVT_MENU_OPEN(  AmayaNormalWindow::OnMenuOpen )
  EVT_MENU_CLOSE( AmayaNormalWindow::OnMenuClose )
#endif /* __WXDEBUG__ */
  
  EVT_MENU_HIGHLIGHT_ALL( AmayaNormalWindow::OnMenuHighlight )
  EVT_MENU(wxID_ANY,   AmayaNormalWindow::OnMenuItem )
   
  EVT_CLOSE(AmayaNormalWindow::OnClose )

  EVT_SPLITTER_SASH_POS_CHANGED(wxID_ANY, 	AmayaNormalWindow::OnSplitterPosChanged )
  EVT_SPLITTER_DCLICK(wxID_ANY, 		AmayaNormalWindow::OnSplitterDClick )

  EVT_BUTTON( wxID_ANY,                       AmayaNormalWindow::OnSplitPanelButton)
  EVT_NOTEBOOK_PAGE_CHANGED( wxID_ANY, AmayaNormalWindow::OnNotebookPageChanged )
END_EVENT_TABLE()

  
  
  
#endif /* #ifdef _WX */
