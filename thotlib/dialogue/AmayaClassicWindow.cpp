#ifdef _WX

#include "wx/wx.h"
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

#include "AmayaClassicWindow.h"
#include "AmayaPanel.h"
//#include "AmayaClassicNotebook.h"
#include "AmayaAdvancedNotebook.h"
#include "AmayaPage.h"
#include "AmayaFrame.h"
#include "AmayaCallback.h"
#include "AmayaToolBar.h"
#include "AmayaQuickSplitButton.h"
#include "AmayaStatusBar.h"

#ifdef _MACOS
/* Wrap-up to prevent an event when the tabs are switched on Mac */
static ThotBool  UpdateFrameUrl = TRUE;
#endif /* _MACOS */

IMPLEMENT_CLASS(AmayaClassicWindow, AmayaNormalWindow)

/*----------------------------------------------------------------------
 *       Class:  AmayaClassicWindow
 *      Method:  AmayaClassicWindow
 * Description:  create a new AmayaClassicWindow
 -----------------------------------------------------------------------*/
AmayaClassicWindow::AmayaClassicWindow ( wxWindow * parent, wxWindowID id
                                      ,const wxPoint& pos
                                      ,const wxSize&  size
                                      ,int kind
                                      ) : 
   AmayaNormalWindow( parent, id, pos, size, kind ),
   m_pPanel(NULL),
   m_pNotebook(NULL),
   m_pSplitterWindow(NULL),
   m_SlashPos(0),
   m_pSplitPanelButton(NULL)
{
  Initialize();
}

/*----------------------------------------------------------------------
 *       Class:  AmayaClassicWindow
 *      Method:  ~AmayaClassicWindow
 * Description:  destructor
 -----------------------------------------------------------------------*/
AmayaClassicWindow::~AmayaClassicWindow()
{
  SetAutoLayout(FALSE);
}

/*----------------------------------------------------------------------
 *       Class:  AmayaClassicWindow
 *      Method:  Initialize
 * Description:  Initialize the specific part of AmayaClassicWindow
 -----------------------------------------------------------------------*/
bool AmayaClassicWindow::Initialize()
{
  // initialize default slashbar position
  TtaSetEnvInt("SLASH_PANEL_POS", 195, FALSE);
  // load slash position from registry
  TtaGetEnvInt ("SLASH_PANEL_POS", &m_SlashPos);

  // Create a background panel to contain everything : better look on windows
  wxBoxSizer * p_TopSizer = new wxBoxSizer ( wxVERTICAL );
  
  {
    // Create toolbars
    if(HaveToolBarBrowsing())
      p_TopSizer->Add( GetToolBarBrowsing(), 0, wxEXPAND);
    if(HaveToolBarEditing())
      p_TopSizer->Add( GetToolBarEditing(), 0, wxEXPAND);
    
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
      m_pSplitterWindow->SetMinimumPaneSize( 196 );
      m_pLayoutSizer->Add(m_pSplitterWindow, 1, wxEXPAND);
      {
        // Create a AmayaPanel to contains commands shortcut
        m_pPanel = new AmayaToolPanelBar( m_pSplitterWindow, -1, wxDefaultPosition, wxDefaultSize,
                                   wxTAB_TRAVERSAL |
                                   wxRAISED_BORDER |
                                   wxCLIP_CHILDREN );
        // Create the notebook
        m_pNotebook = new AmayaAdvancedNotebook( m_pSplitterWindow, wxID_ANY );

        ThotBool panel_opened;
        int kind = GetKind();
        TtaGetEnvBoolean ("OPEN_PANEL", &panel_opened);
        
        Freeze();
        ShowToolPanels();
        if (!panel_opened ||
            kind == WXAMAYAWINDOW_ANNOT ||
            kind == WXAMAYAWINDOW_HELP ||
            kind == WXAMAYAWINDOW_CSS)
          HideToolPanels();
        Thaw();
      }
      
    }
    p_TopSizer->Add(m_pLayoutSizer, 1, wxEXPAND);
  }  
  // Set the global top sizer
  SetSizer(p_TopSizer);
  
  SetAutoLayout(TRUE);
  
  return AmayaNormalWindow::Initialize();
}


/*----------------------------------------------------------------------
 *       Class:  AmayaClassicWindow
 *      Method:  CleanUp
 * Description:  check that there is no empty pages
 -----------------------------------------------------------------------*/
void AmayaClassicWindow::CleanUp()
{
  if(m_pNotebook)
    m_pNotebook->CleanUp();
  AmayaNormalWindow::CleanUp();
}

/*----------------------------------------------------------------------
 *       Class:  AmayaClassicWindow
 *      Method:  LoadConfig
 * Description:  Load the config from registry and initialize dependancies
 -----------------------------------------------------------------------*/
void AmayaClassicWindow::LoadConfig()
{
  AmayaNormalWindow::LoadConfig();
  
  m_pPanel->LoadConfig();
  
  // TODO : load the global config to open or close the toolPanelBar
}

/*----------------------------------------------------------------------
 *       Class:  AmayaClassicWindow
 *      Method:  SaveConfig
 * Description:  Save config to registry
 -----------------------------------------------------------------------*/
void AmayaClassicWindow::SaveConfig()
{
  // TODO : save the global config about the toolPanelBar
  int kind = GetKind();
  if (kind != WXAMAYAWINDOW_ANNOT && kind != WXAMAYAWINDOW_HELP)
    m_pPanel->SaveConfig();
  AmayaNormalWindow::SaveConfig();
}


/*----------------------------------------------------------------------
 *       Class:  AmayaClassicWindow
 *      Method:  RegisterToolPanel
 * Description:  Add a new tool panel to the window
 -----------------------------------------------------------------------*/
bool AmayaClassicWindow::RegisterToolPanel(AmayaToolPanel* tool)
{
  return m_pPanel->AddPanel(tool);
}

/*----------------------------------------------------------------------
 *       Class:  AmayaClassicWindow
 *      Method:  SetPageIcon
 * Description:  
 -----------------------------------------------------------------------*/
void AmayaClassicWindow::SetPageIcon(int page_id, char *iconpath)
{
  int index = TtaGetIconIndex (iconpath);
  if (m_pNotebook && index > 0)
    m_pNotebook->SetPageImage (page_id, index);
}

/*----------------------------------------------------------------------
 *       Class:  AmayaClassicWindow
 *      Method:  CreatePage
 * Description:  create a new AmayaPage.
 *               it's possible to attach automaticaly this page to the window or not
 -----------------------------------------------------------------------*/
AmayaPage * AmayaClassicWindow::CreatePage( Document doc, bool attach, int position )
{
  return AmayaNormalWindow::DoCreatePage(m_pNotebook, doc, attach, position);
}

/*----------------------------------------------------------------------
 *       Class:  AmayaClassicWindow
 *      Method:  GetPageContainer
 * Description:  Return the container of AmayaPages
 -----------------------------------------------------------------------*/
const AmayaPageContainer* AmayaClassicWindow::GetPageContainer()const
{
  return m_pNotebook;
}
/*----------------------------------------------------------------------
 *       Class:  AmayaClassicWindow
 *      Method:  GetPageContainer
 * Description:  Return the container of AmayaPages
 -----------------------------------------------------------------------*/
AmayaPageContainer* AmayaClassicWindow::GetPageContainer()
{
  return m_pNotebook;
}

/*----------------------------------------------------------------------
 *       Class:  AmayaClassicWindow
 *      Method:  OnClose
 * Description:  Intercept the CLOSE event and prevent it if ncecessary.
  -----------------------------------------------------------------------*/
void AmayaClassicWindow::OnClose(wxCloseEvent& event)
{
  if (s_normalWindowCount == 1)
    {
      int kind = GetKind();
      if (kind != WXAMAYAWINDOW_ANNOT && kind != WXAMAYAWINDOW_HELP)
        {
          SaveConfig();
          TtaSetEnvBoolean("OPEN_PANEL", ToolPanelsShown(), TRUE);
        }
    }
  
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
  event.Skip();
}


/*----------------------------------------------------------------------
 *       Class:  AmayaClassicWindow
 *      Method:  IsToolBarShown
 * Description:  Query if a toolbar is shown
 -----------------------------------------------------------------------*/
bool AmayaClassicWindow::IsToolBarShown(int toolbarID)
{
  switch(toolbarID)
  {
    case 0:
      if(HaveToolBarBrowsing())
        return GetSizer()->IsShown(GetToolBarBrowsing());
      break;
    case 1:
      if(HaveToolBarEditing())
        return GetSizer()->IsShown(GetToolBarEditing());
      break;
    default:
      break;
  }
  return false;
}

/*----------------------------------------------------------------------
 *       Class:  AmayaClassicWindow
 *      Method:  ShowToolBar
 * Description:  Toggle a toolbar state (shown/hidden)
 -----------------------------------------------------------------------*/
void AmayaClassicWindow::ShowToolBar(int toolbarID, bool bShow)
{
  switch(toolbarID)
  {
    case 0:
      if(HaveToolBarBrowsing())
        GetSizer()->Show(GetToolBarBrowsing(), bShow);
      break;
    case 1:
      if(HaveToolBarEditing())
        GetSizer()->Show(GetToolBarEditing(), bShow);
      break;
    default:
      break;
  }
  Layout();
  RefreshShowToolBarToggleMenu(toolbarID);
}


/*----------------------------------------------------------------------
 *       Class:  AmayaClassicWindow
 *      Method:  ToggleFullScreen
 * Description:  switch on/off fullscreen state
 -----------------------------------------------------------------------*/
void AmayaClassicWindow::ToggleFullScreen()
{
  ShowToolBar(0, IsFullScreen());
  ShowToolBar(1, IsFullScreen());
  AmayaNormalWindow::ToggleFullScreen();
}


/*----------------------------------------------------------------------
 *       Class:  AmayaClassicWindow
 *      Method:  OnSplitterPosChanged
 * Description:  this method is called when the splitter position has changed
 -----------------------------------------------------------------------*/
void AmayaClassicWindow::OnSplitterPosChanged( wxSplitterEvent& event )
{
  TTALOGDEBUG_1( TTA_LOG_DIALOG, _T("AmayaClassicWindow::OnSplitterPosChanged now = %d"), event.GetSashPosition() );
  
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
 *       Class:  AmayaClassicWindow
 *      Method:  OnSize
 * Description:  Handle window resizing event
 -----------------------------------------------------------------------*/
void AmayaClassicWindow::OnSize(wxSizeEvent& event)
{
  if(!strcmp(TtaGetEnvString("TOOLPANEL_LAYOUT"), "RIGHT") && m_pSplitterWindow!=NULL)
    {
      m_pSplitterWindow->SetSashPosition(GetSize().x-m_SlashPos);
    }
    
  event.Skip();
}


/*----------------------------------------------------------------------
 *       Class:  AmayaClassicWindow
 *      Method:  OnSplitterDClick
 * Description:  called when a double click is done on the splitbar
 *               detach the panel area (hide it)
 -----------------------------------------------------------------------*/
void AmayaClassicWindow::OnSplitterDClick( wxSplitterEvent& event )
{
  TTALOGDEBUG_0( TTA_LOG_DIALOG, _T("AmayaClassicWindow::OnSplitterDClick") );
  m_pSplitterWindow->Unsplit( m_pPanel );
}

/*----------------------------------------------------------------------
 *       Class:  AmayaClassicWindow
 *      Method:  OnSplitPanelButton
 * Description:  this method is called when the button for quick split is pushed
 -----------------------------------------------------------------------*/
void AmayaClassicWindow::OnSplitPanelButton( wxCommandEvent& event )
{
  if ( event.GetId() != m_pSplitPanelButton->GetId() )
    {
      event.Skip();
      return;
    }

  TTALOGDEBUG_0( TTA_LOG_DIALOG, _T("AmayaClassicWindow::OnSplitPanelButton") );

  if (!m_pSplitterWindow->IsSplit())
    ShowToolPanels();
  else
    HideToolPanels();

  // do not skip this event because on windows, the callback is called twice
  //event.Skip();
}

/*----------------------------------------------------------------------
 *       Class:  AmayaClassicWindow
 *      Method:  HideToolPanels
 * Description:  close the side panel
 -----------------------------------------------------------------------*/
void AmayaClassicWindow::HideToolPanels()
{
  TTALOGDEBUG_0( TTA_LOG_PANELS, _T("AmayaClassicWindow::HideToolPanels") );

  if (m_pSplitterWindow && ToolPanelsShown())
    {
      m_pSplitterWindow->Unsplit( m_pPanel );

      // refresh the corresponding menu item state
      RefreshShowToolPanelToggleMenu();
    }
}

/*----------------------------------------------------------------------
 *       Class:  AmayaClassicWindow
 *      Method:  ShowToolPanels
 * Description:  open the side panel
 -----------------------------------------------------------------------*/
void AmayaClassicWindow::ShowToolPanels()
{
  TTALOGDEBUG_0( TTA_LOG_PANELS, _T("AmayaClassicWindow::ShowToolPanels") );
  
  if (!ToolPanelsShown())
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
      RefreshShowToolPanelToggleMenu();
    }
}

/*----------------------------------------------------------------------
 *       Class:  AmayaClassicWindow
 *      Method:  ToolPanelsShown
 * Description:  returns true if the side panel is opened
 -----------------------------------------------------------------------*/
bool AmayaClassicWindow::ToolPanelsShown()
{
  return m_pSplitterWindow!=NULL?m_pSplitterWindow->IsSplit():false;
}


/*----------------------------------------------------------------------
 *       Class:  AmayaClassicWindow
 *      Method:  GetToolPanel
 * Description:  return the window's panel (exists only on AmayaClassicWindow)
 -----------------------------------------------------------------------*/
AmayaToolPanel* AmayaClassicWindow::GetToolPanel(int kind)
{
  return m_pPanel->GetToolPanel(kind);
}

/*----------------------------------------------------------------------
 *       Class:  AmayaClassicWindow
 *      Method:  UpdateToolPanelLayout
 * Description:  Intend to change the layout of tool bar panel
 -----------------------------------------------------------------------*/
void AmayaClassicWindow::UpdateToolPanelLayout()
{
  HideToolPanels();
  ShowToolPanels();
}


/*----------------------------------------------------------------------
 *       Class:  AmayaClassicWindow
 *      Method:  OnNotebookPageChanged
 * Description:  is called when the notebook changes of page.
 -----------------------------------------------------------------------*/
void AmayaClassicWindow::OnNotebookPageChanged( wxNotebookEvent& event )
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
 *  this is where the event table is declared
 *  the callbacks are assigned to an event type
 *----------------------------------------------------------------------*/
BEGIN_EVENT_TABLE(AmayaClassicWindow, AmayaNormalWindow)
   
  EVT_CLOSE(AmayaClassicWindow::OnClose )
  EVT_SIZE(AmayaClassicWindow::OnSize)

  EVT_SPLITTER_SASH_POS_CHANGED(wxID_ANY, AmayaClassicWindow::OnSplitterPosChanged )
  EVT_SPLITTER_DCLICK(wxID_ANY,           AmayaClassicWindow::OnSplitterDClick )

  EVT_BUTTON( wxID_ANY,                   AmayaClassicWindow::OnSplitPanelButton)
  EVT_NOTEBOOK_PAGE_CHANGED( wxID_ANY,    AmayaClassicWindow::OnNotebookPageChanged )
  
  EVT_COMBOBOX( XRCID("wxID_TOOL_URL"),   AmayaClassicWindow::OnURLSelected )
  EVT_TEXT_ENTER( XRCID("wxID_TOOL_URL"), AmayaClassicWindow::OnURLTextEnter )
  EVT_TEXT( XRCID("wxID_TOOL_URL"),       AmayaClassicWindow::OnURLText )
  
END_EVENT_TABLE()


#endif /* #ifdef _WX */
