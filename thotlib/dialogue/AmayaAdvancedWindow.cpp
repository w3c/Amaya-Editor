#ifdef _WX

#include "wx/wx.h"
#include "wx/aui/auibook.h"
#include "wx/tglbtn.h"
#include "wx/string.h"
#include "wx/spinctrl.h"
#include "wx/xrc/xmlres.h"

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
#include "editcommands_f.h"

#include "AmayaAdvancedWindow.h"

#include "AmayaPanel.h"

#include "AmayaMathMLPanel.h"
#include "AmayaSpeCharPanel.h"
#include "AmayaStylePanel.h"

#include "AmayaAdvancedNotebook.h"
#include "AmayaPage.h"
#include "AmayaFrame.h"
#include "AmayaCallback.h"
#include "AmayaToolBar.h"
#include "AmayaQuickSplitButton.h"
#include "AmayaStatusBar.h"


IMPLEMENT_CLASS(AmayaAdvancedWindow, AmayaNormalWindow)


/*----------------------------------------------------------------------
 *       Class:  AmayaAdvancedWindow
 *      Method:  AmayaAdvancedWindow
 * Description:  create a new AmayaAdvancedWindow
 -----------------------------------------------------------------------*/
AmayaAdvancedWindow::AmayaAdvancedWindow ( wxWindow * parent, wxWindowID id
                                      ,const wxPoint& pos
                                      ,const wxSize&  size
                                      ,int kind
                                      ) :
AmayaNormalWindow( parent, id, pos, size, kind ),
m_manager(),
m_notebook(NULL)
{
  Initialize();
}

/*----------------------------------------------------------------------
 *       Class:  AmayaAdvancedWindow
 *      Method:  ~AmayaAdvancedWindow
 * Description:  destructor
 -----------------------------------------------------------------------*/
AmayaAdvancedWindow::~AmayaAdvancedWindow()
{
  SetAutoLayout(FALSE);
  m_manager.UnInit();
 
}

/*----------------------------------------------------------------------
 *       Class:  AmayaAdvancedWindow
 *      Method:  Initialize
 * Description:  Initialize the specific part of AmayaAdvancedWindow
 -----------------------------------------------------------------------*/
bool AmayaAdvancedWindow::Initialize()
{
  wxPanel *pane;

  // Setup AUI !
  m_manager.SetManagedWindow(this);
  
  // Create the notebook
  m_notebook = new AmayaAdvancedNotebook(this, wxID_ANY );
  m_manager.AddPane(m_notebook, wxAuiPaneInfo().Name(wxT("AmayaAdvancedNotebook")).
                                    CenterPane().PaneBorder(false));

  
  // Add toolbars to AUI
  if(HaveToolBarBrowsing())
    m_manager.AddPane(GetToolBarBrowsing(), wxAuiPaneInfo().
                  Name(wxT("Browsing")).Caption(wxT("Browsing")).ToolbarPane().Top().
                  Gripper(false).Row(0).Floatable(false).PaneBorder(false));
  if(HaveToolBarEditing())
    m_manager.AddPane(GetToolBarEditing(), wxAuiPaneInfo().
                  Name(wxT("Edition")).Caption(wxT("Edition")).ToolbarPane().Top().
                  Gripper(false).Row(1).Floatable(false).PaneBorder(false));

  wxAuiDockArt* art = m_manager.GetArtProvider();
  if(art)
    {
      art->SetMetric(wxAUI_DOCKART_CAPTION_SIZE, 18);
      wxFont font = art->GetFont(wxAUI_DOCKART_CAPTION_FONT);
      font.SetPointSize(10);
      font.SetWeight(wxFONTWEIGHT_BOLD);
      art->SetFont(wxAUI_DOCKART_CAPTION_FONT, font);
    }
  
  // Commit positions to AUI.
  m_manager.Update();
  
  return AmayaNormalWindow::Initialize();
}


/*----------------------------------------------------------------------
 *       Class:  AmayaAdvancedWindow
 *      Method:  CleanUp
 * Description:  check that there is no empty pages
 -----------------------------------------------------------------------*/
void AmayaAdvancedWindow::CleanUp()
{
  if(m_notebook)
    m_notebook->CleanUp();
  AmayaNormalWindow::CleanUp();
}

/*----------------------------------------------------------------------
 *       Class:  AmayaAdvancedWindow
 *      Method:  LoadConfig
 * Description:  Load the config from registry and initialize dependancies
 -----------------------------------------------------------------------*/
void AmayaAdvancedWindow::LoadConfig()
{
  AmayaNormalWindow::LoadConfig();
  
  // Load tool states and positions.
  AmayaAdvanceToolPanelMap::iterator it;
  for(it = m_panels.begin(); it!=m_panels.end(); ++it )
  {
    wxString name = wxT("AUI_") + it->second->GetToolPanelConfigKeyName();
    wxString str = TtaConvMessageToWX(TtaGetEnvString((const char*)name.mb_str(wxConvUTF8)));
    
    if(str.Trim().IsEmpty())
      str = it->second->GetDefaultAUIConfig();
    
    m_manager.LoadPaneInfo(str, m_manager.GetPane(it->second));
    
//    m_manager.GetPane(it->second).TopDockable(false).BottomDockable(false);
    
  }
  m_manager.Update();

  // Add menu items
  wxMenuBar* bar = GetMenuBar();
  if(bar)
    {
      wxMenu* menu = NULL;
      bar->FindItem(WindowTable[m_WindowId].MenuItemShowPanelID , &menu);
      if(menu)
        {
          menu->AppendSeparator();
          for(it = m_panels.begin(); it!=m_panels.end(); ++it )
          {
            int id = ::wxNewId();
            
            menu->AppendCheckItem(id, it->second->GetToolPanelName());
            menu->Check(id, it->second->IsVisible());
            m_panelMenus[id] = it->second;
            
            // Connect toggle menu event
            Connect(id, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(AmayaAdvancedWindow::OnToggleToolPanelMenu));
            // Connect toggle menu update event
            Connect(id, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(AmayaAdvancedWindow::OnUpdateToolPanelMenu));
          }
        }
    }
}

void AmayaAdvancedWindow::OnToggleToolPanelMenu(wxCommandEvent& event)
{
  m_manager.GetPane(m_panelMenus[event.GetId()]).Show(event.IsChecked());
  m_panelMenus[event.GetId()]->SetVisibleFlag(event.IsChecked());
  event.Skip();
  m_manager.Update();
}

void AmayaAdvancedWindow::OnUpdateToolPanelMenu(wxUpdateUIEvent& event)
{
  event.Check(m_manager.GetPane(m_panelMenus[event.GetId()]).IsShown());
}


/*----------------------------------------------------------------------
 *       Class:  AmayaAdvancedWindow
 *      Method:  SaveConfig
 * Description:  Save config to registry
 -----------------------------------------------------------------------*/
void AmayaAdvancedWindow::SaveConfig()
{
  // Save tool states and positions.
  AmayaAdvanceToolPanelMap::iterator it;
  for(it = m_panels.begin(); it!=m_panels.end(); ++it )
  {
    if(it->second)
      {
        wxString str = m_manager.SavePaneInfo(m_manager.GetPane(it->second)),
                 name = wxT("AUI_") + it->second->GetToolPanelConfigKeyName();
        
        TtaSetEnvString((const char*)name.mb_str(wxConvUTF8),
                        (char*)(const char*)str.mb_str(wxConvUTF8), TRUE);
      }
  }

  AmayaNormalWindow::SaveConfig();
}


/*----------------------------------------------------------------------
 *       Class:  AmayaAdvancedWindow
 *      Method:  SetPageIcon
 * Description:  
 -----------------------------------------------------------------------*/
void AmayaAdvancedWindow::SetPageIcon(int page_id, char *iconpath)
{
  int index = TtaGetIconIndex (iconpath);
  if (m_notebook && index > 0)
    m_notebook->SetPageImage (page_id, index);
}

/*----------------------------------------------------------------------
 *       Class:  AmayaAdvancedWindow
 *      Method:  GetPageContainer
 * Description:  Retrieve the window wichi will directly contain the page.
 -----------------------------------------------------------------------*/
wxWindow* AmayaAdvancedWindow::GetPageContainer()const
{
  return m_notebook;
}


/*----------------------------------------------------------------------
 *       Class:  AmayaAdvancedWindow
 *      Method:  AttachPage
 * Description:  really attach a page to the current window
 -----------------------------------------------------------------------*/
bool AmayaAdvancedWindow::AttachPage( int position, AmayaPage * p_page )
{
  bool ret;
  if (!m_notebook)
    ret = false;
  else
    {
      /* notebook is a new parent for the page
       * warning: AmayaPage original parent must be a wxNotbook */
      //    p_page->Reparent( m_notebook );
      p_page->SetContainer( m_notebook );
    
      /* insert the page in the notebook */
      ret = m_notebook->InsertPage( position,
                                     p_page,
                                     _T(""),  /* this is the page name */
                                     false,
                                     0 ); /* this is the default image id */

      // update the pages ids
      m_notebook->UpdatePageId();

      // the inserted page should be forced to notebook size
      m_notebook->Layout();
      TTALOGDEBUG_2( TTA_LOG_DIALOG, _T("AmayaAdvancedWindow::AttachPage - pagesize: w=%d h=%d"),
                     p_page->GetSize().GetWidth(),
                     p_page->GetSize().GetHeight());

      SetAutoLayout(TRUE);
    }
  return ret;
}


/*----------------------------------------------------------------------
 *       Class:  AmayaAdvancedWindow
 *      Method:  DetachPage
 * Description:  
 -----------------------------------------------------------------------*/
bool AmayaAdvancedWindow::DetachPage( int position )
{
  return false;
}


/*----------------------------------------------------------------------
 *       Class:  AmayaAdvancedWindow
 *      Method:  ClosePage
 * Description:  ferme une page
 -----------------------------------------------------------------------*/
bool AmayaAdvancedWindow::ClosePage( int page_id )
{
  // flush all pending events
  wxTheApp->Yield();

  if (m_notebook == NULL)
    return true;
  else
  {
    if(m_notebook->ClosePage(page_id))
    {
      /** \todo Test if no more page is present on the window.*/
      return true;
    }
    else
      return false;
  }
}

/*----------------------------------------------------------------------
 *       Class:  AmayaAdvancedWindow
 *      Method:  CloseAllButPage
 * Description:  ferme toutes les pages sauf une
 -----------------------------------------------------------------------*/
bool AmayaAdvancedWindow::CloseAllButPage( int position )
{
  if(m_notebook)
    return m_notebook->CloseAllButPage(position);
  else
    return false;
}

/*----------------------------------------------------------------------
 *       Class:  AmayaAdvancedWindow
 *      Method:  GetPage
 * Description:  search the page at given position
 -----------------------------------------------------------------------*/
AmayaPage * AmayaAdvancedWindow::GetPage( int position ) const
{
  TTALOGDEBUG_0( TTA_LOG_DIALOG, _T("AmayaAdvancedWindow::GetPage") );
  if (!m_notebook)
    return NULL;
  if (GetPageCount() <= position)
    return NULL;
  return (AmayaPage *)m_notebook->GetPage(position);
}


/*----------------------------------------------------------------------
 *       Class:  AmayaAdvancedWindow
 *      Method:  GetPage
 * Description:  how many page into the window
 -----------------------------------------------------------------------*/
int AmayaAdvancedWindow::GetPageCount() const
{
  if (!m_notebook)
    return 0;
  return (int)m_notebook->GetPageCount();
}


/*----------------------------------------------------------------------
 *       Class:  AmayaAdvancedWindow
 *      Method:  OnClose
 * Description:  Intercept the CLOSE event and prevent it if ncecessary.
  -----------------------------------------------------------------------*/
void AmayaAdvancedWindow::OnClose(wxCloseEvent& event)
{
  if (s_normalWindowCount==1)
    TtaSetEnvBoolean("OPEN_PANEL", ToolPanelsShown(), TRUE);
  
  if(m_notebook)
  {
    // Intend to close the notebook
    if(!m_notebook->Close())
    {
      event.Veto();
      return;
    }
    m_notebook->Destroy();
    m_notebook = NULL;
  }
  event.Skip();
}


/*----------------------------------------------------------------------
 *       Class:  AmayaAdvancedWindow
 *      Method:  GetActivePage
 * Description:  return the current selected page
 -----------------------------------------------------------------------*/
AmayaPage * AmayaAdvancedWindow::GetActivePage() const
{
  TTALOGDEBUG_0( TTA_LOG_DIALOG, _T("AmayaAdvancedWindow::GetActivePage") );
  if (!m_notebook)
    return NULL;
  return (m_notebook->GetSelection() >= 0) ? GetPage(m_notebook->GetSelection()) : NULL;
}


/*----------------------------------------------------------------------
 *       Class:  AmayaAdvancedWindow
 *      Method:  IsToolBarShown
 * Description:  Query if a toolbar is shown
 -----------------------------------------------------------------------*/
bool AmayaAdvancedWindow::IsToolBarShown(int toolbarID)
{
  switch(toolbarID)
  {
    case 0:
      if(HaveToolBarBrowsing())
        return m_manager.GetPane(GetToolBarBrowsing()).IsShown();
      break;
    case 1:
      if(HaveToolBarEditing())
        return m_manager.GetPane(GetToolBarEditing()).IsShown();
      break;
    default:
      break;
  }
  return false;
}

/*----------------------------------------------------------------------
 *       Class:  AmayaAdvancedWindow
 *      Method:  ShowToolBar
 * Description:  Toggle a toolbar state (shown/hidden)
 -----------------------------------------------------------------------*/
void AmayaAdvancedWindow::ShowToolBar(int toolbarID, bool bShow)
{
  switch(toolbarID)
  {
    case 0:
      if(HaveToolBarBrowsing())
        m_manager.GetPane(GetToolBarBrowsing()).Show(bShow);
      break;
    case 1:
      if(HaveToolBarEditing())
        m_manager.GetPane(GetToolBarEditing()).Show(bShow);
      break;
    default:
      break;
  }
  m_manager.Update();
  RefreshShowToolBarToggleMenu(toolbarID);
}


/*----------------------------------------------------------------------
 *       Class:  AmayaAdvancedWindow
 *      Method:  ToggleFullScreen
 * Description:  switch on/off fullscreen state
 -----------------------------------------------------------------------*/
void AmayaAdvancedWindow::ToggleFullScreen()
{
  ShowToolBar(0, IsFullScreen());
  ShowToolBar(1, IsFullScreen());
  AmayaNormalWindow::ToggleFullScreen();
}

/*----------------------------------------------------------------------
 *       Class:  AmayaAdvancedWindow
 *      Method:  RegisterToolPanel
 * Description:  Add a new tool panel to the window
 -----------------------------------------------------------------------*/
bool AmayaAdvancedWindow::RegisterToolPanel(AmayaToolPanel* tool)
{
  tool->Create(this, wxID_ANY);
  if(tool->CanResize())
    tool->Fit();
  
  m_manager.AddPane(tool, wxAuiPaneInfo().
          Name(tool->GetToolPanelConfigKeyName()).Caption(tool->GetToolPanelName()).
          PaneBorder(false).
          LeftDockable().RightDockable().Right().
          Resizable(tool->CanResize()).
          CloseButton(true).PinButton());
  
  m_panels[tool->GetToolPanelType()] = tool;
  return true;
}


/*----------------------------------------------------------------------
 *       Class:  AmayaAdvancedWindow
 *      Method:  HideToolPanels
 * Description:  close the side panel
 -----------------------------------------------------------------------*/
void AmayaAdvancedWindow::HideToolPanels()
{
  TTALOGDEBUG_0( TTA_LOG_PANELS, _T("AmayaAdvancedWindow::HideToolPanels") );

  AmayaAdvanceToolPanelMap::iterator it;
  for(it = m_panels.begin(); it!=m_panels.end(); ++it )
  {
    wxAuiPaneInfo& pane = m_manager.GetPane(it->second);
    pane.Hide();
  }
  m_manager.Update();
}


/*----------------------------------------------------------------------
 *       Class:  AmayaAdvancedWindow
 *      Method:  ShowToolPanels
 * Description:  open the side panel
 -----------------------------------------------------------------------*/
void AmayaAdvancedWindow::ShowToolPanels()
{
  TTALOGDEBUG_0( TTA_LOG_PANELS, _T("AmayaAdvancedWindow::ShowToolPanels") );

  AmayaAdvanceToolPanelMap::iterator it;
  for(it = m_panels.begin(); it!=m_panels.end(); ++it )
  {
    wxAuiPaneInfo& pane = m_manager.GetPane(it->second);
    pane.Show();
  }
  m_manager.Update();
}


/*----------------------------------------------------------------------
 *       Class:  AmayaAdvancedWindow
 *      Method:  ToolPanelsShown
 * Description:  returns true if the side panel is opened
 -----------------------------------------------------------------------*/
bool AmayaAdvancedWindow::ToolPanelsShown()
{
  AmayaAdvanceToolPanelMap::iterator it;
  for(it = m_panels.begin(); it!=m_panels.end(); ++it )
  {
    if(it->second && it->second->IsShown())
      return true;
  }  
  return false;
}


/*----------------------------------------------------------------------
 *       Class:  AmayaAdvancedWindow
 *      Method:  GetToolPanel
 * Description:  return the window's panel (exists only on AmayaClassicWindow)
 -----------------------------------------------------------------------*/
AmayaToolPanel* AmayaAdvancedWindow::GetToolPanel(int kind)
{
  return m_panels[kind];
}

/*----------------------------------------------------------------------
 *       Class:  AmayaAdvancedWindow
 *      Method:  UpdateToolPanelLayout
 * Description:  Intend to change the layout of tool bar panel
 -----------------------------------------------------------------------*/
void AmayaAdvancedWindow::UpdateToolPanelLayout()
{
  m_manager.Update();
}

/*----------------------------------------------------------------------
 *       Class:  AmayaAdvancedWindow
 *      Method:  OnNotebookPageChanged
 * Description:  is called when the notebook changes of page.
 -----------------------------------------------------------------------*/
void AmayaAdvancedWindow::OnNotebookPageChanged( wxNotebookEvent& event )
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
 *       Class:  AmayaAdvancedWindow
 *      Method:  OnMenuItem
 * Description:  this method is called when a menu item is pressed
 *               it will generate an event to differe the menu item action
 -----------------------------------------------------------------------*/
void AmayaAdvancedWindow::OnMenuItem( wxCommandEvent& event )
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
    FrameToView (m_notebook->GetMContextFrame(), &doc, &view);
  else
    FrameToView (TtaGiveActiveFrame(), &doc, &view);
  AmayaWindow::DoAmayaAction( action_id, doc, view );
  event.Skip();
}

void AmayaAdvancedWindow::OnSize(wxSizeEvent& event)
{
  if(HaveToolBarBrowsing())
    {
      wxAuiPaneInfo& pane1 = m_manager.GetPane(GetToolBarBrowsing());
      pane1.BestSize(GetClientSize().x, -1);
    }
  if(HaveToolBarEditing())
    {
      wxAuiPaneInfo& pane2 = m_manager.GetPane(GetToolBarEditing());
      pane2.BestSize(GetClientSize().x, -1);
    }
  m_manager.Update();
  event.Skip();
}

/*----------------------------------------------------------------------
 *  this is where the event table is declared
 *  the callbacks are assigned to an event type
 *----------------------------------------------------------------------*/
BEGIN_EVENT_TABLE(AmayaAdvancedWindow, AmayaWindow)

  EVT_MENU(wxID_ANY,   AmayaAdvancedWindow::OnMenuItem )
  
  EVT_SIZE(AmayaAdvancedWindow::OnSize)
   
  EVT_CLOSE(AmayaAdvancedWindow::OnClose )

  EVT_NOTEBOOK_PAGE_CHANGED( wxID_ANY,    AmayaAdvancedWindow::OnNotebookPageChanged )
  
  EVT_COMBOBOX( XRCID("wxID_TOOL_URL"),   AmayaAdvancedWindow::OnURLSelected )
  EVT_TEXT_ENTER( XRCID("wxID_TOOL_URL"), AmayaAdvancedWindow::OnURLTextEnter )
  EVT_TEXT( XRCID("wxID_TOOL_URL"),       AmayaAdvancedWindow::OnURLText )
  
END_EVENT_TABLE()


#endif /* #ifdef _WX */
