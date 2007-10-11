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

#include "AmayaNormalWindow.h"
#include "AmayaPanel.h"
#include "AmayaMathMLPanel.h"
#include "AmayaNotebook.h"
#include "AmayaPage.h"
#include "AmayaFrame.h"
#include "AmayaCallback.h"
#include "AmayaToolBar.h"
#include "AmayaQuickSplitButton.h"
#include "AmayaStatusBar.h"

#include "AmayaAttributePanel.h"
#include "AmayaApplyClassPanel.h"
#include "AmayaMathMLPanel.h"
#include "AmayaXHTMLPanel.h"
#include "AmayaExplorerPanel.h"
#include "AmayaXMLPanel.h"
#include "AmayaStylePanel.h"
#include "AmayaSpeCharPanel.h"

#include "AmayaClassicWindow.h"
#include "AmayaAdvancedWindow.h"


#ifdef _MACOS
/* Wrap-up to prevent an event when the tabs are switched on Mac */
static ThotBool  UpdateFrameUrl = TRUE;
#endif /* _MACOS */

#ifdef _WINDOWS
static  char      BufUrl[2048];
static  ThotBool  isBufUrl = 0;
#endif /* _WINDOWS */

/*----------------------------------------------------------------------
 *       Class:  AmayaNormalWindow
 *      Method:  CreateNormalWindow
 * Description:  create a new AmayaNormalWindow according prefs
 -----------------------------------------------------------------------*/
AmayaNormalWindow* AmayaNormalWindow::CreateNormalWindow(wxWindow * parent, wxWindowID id,
    const wxPoint& pos, const wxSize&  size, int kind)
{
  TtaSetEnvBoolean("ADVANCE_USER_INTERFACE", FALSE, FALSE);
  
  ThotBool b;
  TtaGetEnvBoolean("ADVANCE_USER_INTERFACE", &b);
  
  if(b)
    return new AmayaAdvancedWindow(parent, id, pos, size, kind);
  else
    return new AmayaClassicWindow(parent, id, pos, size, kind);  
}



int AmayaNormalWindow::s_normalWindowCount = 0;

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
m_pStatusBar(NULL),
m_pToolBarEditing(NULL),
m_pToolBarBrowsing(NULL),
m_pComboBox(NULL)
{
  s_normalWindowCount++;
}

/*----------------------------------------------------------------------
 *       Class:  AmayaNormalWindow
 *      Method:  ~AmayaNormalWindow
 * Description:  destructor
 -----------------------------------------------------------------------*/
AmayaNormalWindow::~AmayaNormalWindow()
{
  s_normalWindowCount--;
}


/*----------------------------------------------------------------------
 *       Class:  AmayaNormalWindow
 *      Method:  Initialize
 * Description:  Initialize common part of all AmayaNormalWindow-based.
 -----------------------------------------------------------------------*/
bool AmayaNormalWindow::Initialize()
{
  // Creation of the menubar
  TtaMakeWindowMenuBar(m_WindowId);

  // Creation of the statusbar
  m_pStatusBar = new AmayaStatusBar(this);
  SetStatusBar(m_pStatusBar);
  WindowTable[m_WindowId].WdStatus = m_pStatusBar;

  return AmayaWindow::Initialize();
}

/*----------------------------------------------------------------------
 *       Class:  AmayaNormalWindow
 *      Method:  CleanUp
 * Description:  check that there is no empty pages
 -----------------------------------------------------------------------*/
void AmayaNormalWindow::CleanUp()
{
  if(GetPageCount()==0)
    Close();
}

/*----------------------------------------------------------------------
 *       Class:  AmayaNormalWindow
 *      Method:  LoadConfig
 * Description:  Load the config from registry and initialize dependancies
 -----------------------------------------------------------------------*/
void AmayaNormalWindow::LoadConfig()
{
  RegisterToolPanel(new AmayaExplorerToolPanel);
  RegisterToolPanel(new AmayaXHTMLToolPanel);
  RegisterToolPanel(new AmayaAttributeToolPanel);
  RegisterToolPanel(new AmayaApplyClassToolPanel);
  RegisterToolPanel(new AmayaStyleToolPanel);
  RegisterToolPanel(new AmayaMathMLToolPanel);
  RegisterToolPanel(new AmayaSpeCharToolPanel);
  RegisterToolPanel(new AmayaXMLToolPanel);

  /* init default panel states */
  TtaSetEnvBoolean("OPEN_PANEL_EXPLORER", TRUE, FALSE);
  TtaSetEnvBoolean("OPEN_PANEL_XHTML", TRUE, FALSE);
  TtaSetEnvBoolean("OPEN_PANEL_ATTRIBUTE", TRUE, FALSE);
  TtaSetEnvBoolean("OPEN_PANEL_XML", FALSE, FALSE);
  TtaSetEnvBoolean("OPEN_PANEL_STYLE", TRUE, FALSE);
  TtaSetEnvBoolean("OPEN_PANEL_MATHML", FALSE, FALSE);
  TtaSetEnvBoolean("OPEN_PANEL_SPECHAR", FALSE, FALSE);
  TtaSetEnvBoolean("OPEN_PANEL_APPLYCLASS", TRUE, FALSE);
}

/*----------------------------------------------------------------------
 *       Class:  AmayaNormalWindow
 *      Method:  SaveConfig
 * Description:  Save config to registry
 -----------------------------------------------------------------------*/
void AmayaNormalWindow::SaveConfig()
{
  AmayaWindow::SaveConfig();
}


/*----------------------------------------------------------------------
 *       Class:  AmayaNormalWindow
 *      Method:  GetToolBarEditing
 * Description:  Return the toolbar panel for edition and create it if needed.
 -----------------------------------------------------------------------*/
wxPanel* AmayaNormalWindow::GetToolBarEditing()
{
  if(!m_pToolBarEditing)
    m_pToolBarEditing = wxXmlResource::Get()->LoadPanel(this, wxT("wxID_PANEL_TOOLBAR_EDITING"));
  return m_pToolBarEditing;
}

/*----------------------------------------------------------------------
 *       Class:  AmayaNormalWindow
 *      Method:  GetToolBarBrowsing
 * Description:  Return a toolbar panel for browsing and create it if needed.
 -----------------------------------------------------------------------*/
wxPanel* AmayaNormalWindow::GetToolBarBrowsing()
{
  if(!m_pToolBarBrowsing)
    {
      m_pToolBarBrowsing = wxXmlResource::Get()->LoadPanel(this, wxT("wxID_PANEL_TOOLBAR_BROWSING"));
      m_pComboBox = XRCCTRL(*m_pToolBarBrowsing, "wxID_TOOL_URL", wxComboBox);
    }
  return m_pToolBarBrowsing;
}


/*----------------------------------------------------------------------
 *       Class:  AmayaNormalWindow
 *      Method:  Unused
 * Description:  Unused function, just force to link RTTI dependancies
 -----------------------------------------------------------------------*/
void AmayaNormalWindow::Unused()
{
  new AmayaToolBarEditing;
  new AmayaToolBarBrowsing;
  new AmayaMathMLToolBar;
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
 * Description:  create a new AmayaPage.
 *               it's possible to attach automaticaly this page to the window or not
 -----------------------------------------------------------------------*/
AmayaPage * AmayaNormalWindow::CreatePage( bool attach, int position )
{
  AmayaPage * page = new AmayaPage( GetPageContainer(), this );
  
  if (attach)
    AttachPage( position, page );
  
  return page;
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
  TTALOGDEBUG_0( TTA_LOG_DIALOG, _T("AmayaNormalWindow::SetURL - ")+new_url);
#ifdef _MACOS
  UpdateFrameUrl = FALSE;
#endif /* _MACOS */
  if(m_pComboBox)
    {
      if (m_pComboBox->FindString(new_url) == wxNOT_FOUND)
        m_pComboBox->Append(new_url);
      // new url should exists into combobox items so just select it.
      m_pComboBox->SetStringSelection( new_url );
    }
#ifdef _MACOS
  UpdateFrameUrl = TRUE;
#endif /* _MACOS */
}

/*----------------------------------------------------------------------
 *       Class:  AmayaNormalWindow
 *      Method:  GetURL
 * Description:  get the current url value
 -----------------------------------------------------------------------*/
wxString AmayaNormalWindow::GetURL( )
{
  if(m_pComboBox)
    return m_pComboBox->GetValue( );
  else
    return wxT("");
}

/*----------------------------------------------------------------------
 *       Class:  AmayaNormalWindow
 *      Method:  AppendURL
 * Description:  TODO
 -----------------------------------------------------------------------*/
void AmayaNormalWindow::AppendURL ( const wxString & new_url )
{
  if(m_pComboBox)
    m_pComboBox->Append( new_url );
}

/*----------------------------------------------------------------------
 *       Class:  AmayaNormalWindow
 *      Method:  EmptyURLBar
 * Description:  remove all items in the url bar
 -----------------------------------------------------------------------*/
void AmayaNormalWindow::EmptyURLBar()
{
  if(m_pComboBox)
    m_pComboBox->Clear();
}


/*----------------------------------------------------------------------
 *       Class:  AmayaNormalWindow
 *      Method:  RefreshShowToolBarToggleMenu
 -----------------------------------------------------------------------*/
void AmayaNormalWindow::RefreshShowToolBarToggleMenu(int toolbarID)
{
  TTALOGDEBUG_1( TTA_LOG_DIALOG, _T("AmayaNormalWindow::RefreshShowToolBarToggleMenu %d"), toolbarID );

  // update menu items of each documents
  int doc_id    = 1;
  int frame_id  = 0;
  int window_id = GetWindowId();
  int itemID    = WindowTable[window_id].MenuItemShowToolBar[toolbarID];
  int action    = FindMenuActionFromMenuItemID(NULL, itemID);
  ThotBool on   = IsToolBarShown(toolbarID);
    
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
 *      Method:  ToggleFullScreen
 * Description:  switch on/off fullscreen state
 -----------------------------------------------------------------------*/
void AmayaNormalWindow::ToggleFullScreen()
{
  printf("AmayaNormalWindow::ToggleFullScreen\n");
  AmayaWindow::ToggleFullScreen();
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
 *      Method:  RefreshShowToolPanelToggleMenu
 * Description:  is called to toggle on/off the "Show/Hide panel" menu item depeding on
 *               the panel showing state.
 -----------------------------------------------------------------------*/
void AmayaNormalWindow::RefreshShowToolPanelToggleMenu()
{
  TTALOGDEBUG_0( TTA_LOG_DIALOG, _T("AmayaNormalWindow::RefreshShowToolPanelToggleMenu") );

  // update menu items of each documents
  int doc_id    = 1;
  int frame_id  = 0;
  int window_id = GetWindowId();
  int itemID    = WindowTable[window_id].MenuItemShowPanelID;
  int action    = FindMenuActionFromMenuItemID(NULL, itemID);
  ThotBool on   = ToolPanelsShown();
    
  while ( action >= 0 && doc_id < MAX_DOCUMENTS )
    {
      if (LoadedDocument[doc_id-1])
        {
          frame_id = LoadedDocument[doc_id-1]->DocViewFrame[0];
          if (FrameTable[frame_id].FrWindowId == window_id &&
			  MenuActionList[action].ActionToggle[doc_id] != on)
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
 *      Method:  OnURLSelected
 * Description:  Called when the user select a new url
 *               there is a bug in wxWidgets on GTK version, this event is 
 *               called to often : each times user move the mouse with button pressed.
  -----------------------------------------------------------------------*/
void AmayaNormalWindow::OnURLSelected( wxCommandEvent& event )
{
  GotoSelectedURL();
}

/*----------------------------------------------------------------------
 *       Class:  AmayaNormalWindow
 *      Method:  OnURLText
 * Description:  Called when the url text is changed
 *               Just update the current frame internal url variable
  -----------------------------------------------------------------------*/
void AmayaNormalWindow::OnURLText( wxCommandEvent& event )
{
#ifdef _MACOS
  if (UpdateFrameUrl)
#endif /* _MACOS */
    {
      AmayaFrame * p_frame = GetActiveFrame();
      if (p_frame)
        p_frame->UpdateFrameURL(m_pComboBox->GetValue());
      event.Skip();
    }
}

/*----------------------------------------------------------------------
 *       Class:  AmayaNormalWindow
 *      Method:  OnURLText
 * Description:  the user has typed ENTER with his keyboard or clicked on validate button =>
 *               simply activate the callback
  -----------------------------------------------------------------------*/
void AmayaNormalWindow::OnURLTextEnter( wxCommandEvent& event )
{
   // TtaDisplayMessage (INFO, buffer);
  GotoSelectedURL();
  // do not skip this event because we don't want to propagate this event
  // event.Skip();
}

/*----------------------------------------------------------------------
 *       Class:  AmayaNormalWindow
 *      Method:  GotoSelectedURL
 * Description:  validate the selection
  -----------------------------------------------------------------------*/
void AmayaNormalWindow::GotoSelectedURL()
{
  Document doc;
  View     view;

  FrameToView (TtaGiveActiveFrame(), &doc, &view);
  CloseTextInsertion ();

  /* call the callback  with the url selected text */
  PtrDocument pDoc = LoadedDocument[doc-1];
  wxASSERT(pDoc);
  if (pDoc && pDoc->Call_Text)
    {
      char buffer[2048];
      strcpy(buffer, (m_pComboBox->GetValue()).mb_str(wxConvUTF8));
// patch to go-round a bug on Windows (TEXT_ENTER event called twice)
#ifdef _WINDOWS
    if (isBufUrl == FALSE)
    {
      isBufUrl = TRUE;
        (*(Proc3)pDoc->Call_Text) ((void *)doc, (void *)view, (void *)buffer);
       strcpy (BufUrl, buffer);
    }
    else if (strcmp (buffer, BufUrl) != 0)
    {
        (*(Proc3)pDoc->Call_Text) ((void *)doc, (void *)view, (void *)buffer);
       strcpy (BufUrl, buffer);
    }
#else /* _WINDOWS */
        (*(Proc3)pDoc->Call_Text) ((void *)doc, (void *)view, (void *)buffer);
#endif /* _WINDOWS */
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
   
  EVT_COMBOBOX( XRCID("wxID_TOOL_URL"),   AmayaNormalWindow::OnURLSelected )
  EVT_TEXT_ENTER( XRCID("wxID_TOOL_URL"), AmayaNormalWindow::OnURLTextEnter )
  EVT_TEXT( XRCID("wxID_TOOL_URL"),       AmayaNormalWindow::OnURLText )
  
END_EVENT_TABLE()

  
  
  
#endif /* #ifdef _WX */
