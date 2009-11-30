/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2008
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

#ifdef _WX
#include "wx/wx.h"
#include "wx/aui/auibook.h"
#include "wx/tglbtn.h"
#include "wx/string.h"
#include "wx/spinctrl.h"
#include "wx/tokenzr.h"
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

#include "archives.h"

#include "AmayaActionEvent.h"

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
  // Setup AUI !
  m_manager.SetManagedWindow(this);
  
  // Create the notebook
  m_notebook = new AmayaAdvancedNotebook(this, wxID_ANY );
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
  ThotBool open;
  wxString str, name;
  AmayaAdvanceToolPanelMap::iterator it, last;

  TtaGetEnvBoolean("OPEN_PANEL", &open);
  m_bShowPanels = open;

  str = TtaConvMessageToWX(TtaGetEnvString("AUI_DECORATION"));
  if (!str.IsEmpty())
    m_manager.LoadPerspective(str, false);
  
  // Load tool states and positions.
  last = m_panels.end();
  for (it = m_panels.begin(); it != last; ++it )
  {
    name = wxT("AUI_") + it->second->GetToolPanelConfigKeyName();
    str = TtaConvMessageToWX(TtaGetEnvString((const char*)name.mb_str(wxConvUTF8)));

    if (str.Trim().IsEmpty())
      str = it->second->GetDefaultAUIConfig();
    
    m_manager.LoadPaneInfo(str, m_manager.GetPane(it->second));
    
    name = wxT("OPEN_") + it->second->GetToolPanelConfigKeyName();
    TtaGetEnvBoolean((const char*)name.mb_str(wxConvUTF8), &open);
    it->second->SetVisibleFlag(open);
    m_manager.GetPane(it->second).Show(open && m_bShowPanels);
  }

  // Add toolbars to AUI
  if (HaveToolBarBrowsing())
    m_manager.AddPane(GetToolBarBrowsing(), wxAuiPaneInfo().
                  Name(wxT("Browsing")).Caption(wxT("Browsing")).ToolbarPane().Top().
                  Gripper(false).Row(0).Floatable(false).PaneBorder(false).Layer(2));
  if (HaveToolBarEditing())
    m_manager.AddPane(GetToolBarEditing(), wxAuiPaneInfo().
                  Name(wxT("Edition")).Caption(wxT("Edition")).ToolbarPane().Top().
                  Gripper(false).Row(1).Floatable(false).PaneBorder(false).Layer(1));
   
  m_manager.AddPane(m_notebook, wxAuiPaneInfo().Name(wxT("AmayaAdvancedNotebook")).
                                    CenterPane().PaneBorder(false));
  m_manager.Update();

  // Add menu items
  wxMenuBar* bar = GetMenuBar();
  if (bar)
    {
      wxMenu* menu = NULL;
      bar->FindItem(WindowTable[m_WindowId].MenuItemShowPanelID , &menu);
      if(menu)
        {
          menu->AppendSeparator();
          last = m_panels.end();
          for(it = m_panels.begin(); it != last; ++it )
          {
            int id = ::wxNewId();
            menu->AppendCheckItem(id, it->second->GetToolPanelName());
            menu->Check(id, it->second->IsShown());
            m_panelMenus[id] = it->second;
            
            // Connect toggle menu event
            Connect(id, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(AmayaAdvancedWindow::OnToggleToolPanelMenu));
            // Connect toggle menu update event
            Connect(id, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(AmayaAdvancedWindow::OnUpdateToolPanelMenu));
          }
        }
    }
  
  if (GetKind() == WXAMAYAWINDOW_ANNOT)
      HideToolPanels();
}

/*----------------------------------------------------------------------
 -----------------------------------------------------------------------*/
void AmayaAdvancedWindow::OnToggleToolPanelMenu(wxCommandEvent& event)
{
  int      id = event.GetId();
  Document doc;
  int      view;

  m_manager.GetPane(m_panelMenus[id]).Show(event.IsChecked());
  m_panelMenus[id]->SetVisibleFlag(event.IsChecked());
  //event.Skip(); doesn't work on MacOSX
  m_manager.Update();
  // test if the attribute panel should be updated
  if (event.IsChecked() && m_panelMenus[id]->GetToolPanelType() == WXAMAYA_PANEL_ATTRIBUTE)
    {
      TtaGetActiveView (&doc, &view);
      if (doc)
        TtaUpdateAttrMenu (doc);
    }
}

/*----------------------------------------------------------------------
 -----------------------------------------------------------------------*/
void AmayaAdvancedWindow::OnUpdateToolPanelMenu(wxUpdateUIEvent& event)
{
  event.Enable(m_bShowPanels);
  event.Check(m_manager.GetPane(m_panelMenus[event.GetId()]).IsShown());
}


/*----------------------------------------------------------------------
 *       Class:  AmayaAdvancedWindow
 *      Method:  SaveConfig
 * Description:  Save config to registry
 -----------------------------------------------------------------------*/
void AmayaAdvancedWindow::SaveConfig()
{
  wxString str, name;
  wxArrayString arr;
  unsigned int  n, max;
  AmayaAdvanceToolPanelMap::iterator it, last;
  
  // Save tool states and positions.
  last =  m_panels.end();
  for (it = m_panels.begin(); it != last; ++it )
  {
    if (it->second)
      {
        str = m_manager.SavePaneInfo(m_manager.GetPane(it->second));
        name = wxT("AUI_") + it->second->GetToolPanelConfigKeyName();
        arr.Clear();
        arr = wxStringTokenize(str, wxT(";"));
        str.Empty();
        
        // Filter each config variable
        max = arr.GetCount();
        for ( n = 0; n < max; n++)
          {
            wxString &s = arr[n];
            if (s.StartsWith(wxT("dir")) ||
                s.StartsWith(wxT("layer")) ||
                s.StartsWith(wxT("row")) ||
                s.StartsWith(wxT("pos")))
              str += s + wxT(";");
          }
        
        TtaSetEnvString((const char*)name.mb_str(wxConvUTF8),
                        (char*)(const char*)str.mb_str(wxConvUTF8), TRUE);
        // Say if the panel is on/off
        name = wxT("OPEN_") + it->second->GetToolPanelConfigKeyName();
        TtaSetEnvBoolean((const char*)name.mb_str(wxConvUTF8),
                         (ThotBool)it->second->IsShown(), TRUE);
      }
  }

  // Little hack to save manager perspective without panel description.
  str = m_manager.SavePerspective();
  wxStringTokenizer tkz(str, wxT("|"));
  str = wxT("");
  if (SavePANEL_PREFERENCES)
    {
      while (tkz.HasMoreTokens())
        {
          wxString token = tkz.GetNextToken();
          if (token.Find(wxT("name="))==wxNOT_FOUND)
            str += wxT("|") + token;
        }
      if (str[0]==wxT('|'))
        str.Remove(0, 1);
    }
  TtaSetEnvString("AUI_DECORATION", (char*)(const char*)str.mb_str(wxConvUTF8), TRUE);

  // Commit config
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
 *      Method:  CreatePage
 * Description:  create a new AmayaPage.
 *               it's possible to attach automaticaly this page to the window or not
 -----------------------------------------------------------------------*/
AmayaPage * AmayaAdvancedWindow::CreatePage( Document doc, bool attach, int position )
{
  return AmayaNormalWindow::DoCreatePage(m_notebook, doc, attach, position);
}

/*----------------------------------------------------------------------
 *       Class:  AmayaAdvancedWindow
 *      Method:  GetPageContainer
 * Description:  Return the container of AmayaPages
 -----------------------------------------------------------------------*/
AmayaPageContainer* AmayaAdvancedWindow::GetPageContainer()
{
  return m_notebook;
}

/*----------------------------------------------------------------------
 *       Class:  AmayaAdvancedWindow
 *      Method:  GetPageContainer
 * Description:  Return the container of AmayaPages
 -----------------------------------------------------------------------*/
const AmayaPageContainer* AmayaAdvancedWindow::GetPageContainer()const
{
  return m_notebook;
}

/*----------------------------------------------------------------------
 *       Class:  AmayaAdvancedWindow
 *      Method:  OnClose
 * Description:  Intercept the CLOSE event and prevent it if ncecessary.
  -----------------------------------------------------------------------*/
void AmayaAdvancedWindow::OnClose(wxCloseEvent& event)
{
  if (s_normalWindowCount == 1)
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
 *      Method:  IsToolBarShown
 * Description:  Query if a toolbar is shown
 -----------------------------------------------------------------------*/
bool AmayaAdvancedWindow::IsToolBarShown(int toolbarID)
{
  wxRect r;
  wxString str;

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
  wxString str;
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
  RefreshShowToolBarToggleMenu(toolbarID);
  m_manager.Update();
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
          PaneBorder(false).FloatingPosition(100,100).
          LeftDockable().RightDockable().Right().
          TopDockable(false).BottomDockable(false).
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

  m_strPanelPerspective = m_manager.SavePerspective();
  
  AmayaAdvanceToolPanelMap::iterator it, last;
  last = m_panels.end();
  for (it = m_panels.begin(); it != last; ++it )
  {
    wxAuiPaneInfo& pane = m_manager.GetPane(it->second);
    pane.Hide();
  }
  m_manager.Update();
  m_bShowPanels = false;
}


/*----------------------------------------------------------------------
 *       Class:  AmayaAdvancedWindow
 *      Method:  ShowToolPanels
 * Description:  open the side panel
 -----------------------------------------------------------------------*/
void AmayaAdvancedWindow::ShowToolPanels()
{
  TTALOGDEBUG_0( TTA_LOG_PANELS, _T("AmayaAdvancedWindow::ShowToolPanels") );

  AmayaAdvanceToolPanelMap::iterator it, last;
  last = m_panels.end();
  for(it = m_panels.begin(); it != last; ++it )
  {
    wxAuiPaneInfo& pane = m_manager.GetPane(it->second);
    if (it->second && it->second->IsShown())
      pane.Show();
  }
  m_manager.LoadPerspective(m_strPanelPerspective);
//  m_manager.Update();
  m_bShowPanels = true;
}


/*----------------------------------------------------------------------
 *       Class:  AmayaAdvancedWindow
 *      Method:  ToolPanelsShown
 * Description:  returns true if the side panel is opened
 -----------------------------------------------------------------------*/
bool AmayaAdvancedWindow::ToolPanelsShown()
{
  return m_bShowPanels;
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
  AmayaStatusBar* status = GetStatusBar();
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
 *----------------------------------------------------------------------*/
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
BEGIN_EVENT_TABLE(AmayaAdvancedWindow, AmayaNormalWindow)
  
  EVT_SIZE(AmayaAdvancedWindow::OnSize)
  EVT_CLOSE(AmayaAdvancedWindow::OnClose )
  EVT_NOTEBOOK_PAGE_CHANGED( wxID_ANY,    AmayaAdvancedWindow::OnNotebookPageChanged )
  
  EVT_COMBOBOX( XRCID("wxID_TOOL_URL"),   AmayaAdvancedWindow::OnURLSelected )
  EVT_TEXT_ENTER( XRCID("wxID_TOOL_URL"), AmayaAdvancedWindow::OnURLTextEnter )
  EVT_TEXT( XRCID("wxID_TOOL_URL"),       AmayaAdvancedWindow::OnURLText )
END_EVENT_TABLE()


#endif /* #ifdef _WX */
