/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2009
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

#ifdef _WX
#include "wx/wx.h"
#include "wx/tglbtn.h"
#include "wx/string.h"
#include "wx/spinctrl.h"
#include "wx/xrc/xmlres.h"
#include "wx/tokenzr.h"

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
#include "profiles_f.h"

#include "AmayaNormalWindow.h"
#include "AmayaPanel.h"
#include "AmayaMathMLPanel.h"
#include "AmayaClassicNotebook.h"
#include "AmayaPage.h"
#include "AmayaFrame.h"
#include "AmayaCallback.h"
#include "AmayaToolBar.h"
#include "AmayaQuickSplitButton.h"
#include "AmayaStatusBar.h"

#include "AmayaAttributePanel.h"
#include "AmayaApplyClassPanel.h"
#include "AmayaStylePanel.h"
#include "AmayaMathMLPanel.h"
#include "AmayaXHTMLPanel.h"
#include "AmayaElementPanel.h"
#include "AmayaExplorerPanel.h"
#include "AmayaSVGPanel.h"
#include "AmayaXMLPanel.h"
#include "AmayaSpeCharPanel.h"

#include "AmayaClassicWindow.h"
#include "AmayaAdvancedWindow.h"
#include "AmayaHelpWindow.h"

#include "AmayaWindowIterator.h"


#ifdef _MACOS
/* Wrap-up to prevent an event when the tabs are switched on Mac */
static ThotBool  UpdateFrameUrl = TRUE;
#endif /* _MACOS */

#ifdef _WINDOWS
static  char      BufUrl[2048];
static  ThotBool  isBufUrl = FALSE;
#endif /* _WINDOWS */

#define RECENT_DOC_ID     2000
#define RECENT_DOC_MAX_NB   16

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

  if (kind == WXAMAYAWINDOW_HELP)
    return new AmayaHelpWindow(parent, id, pos, size, kind);
  else if (b)
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
m_pToolBarEditing(NULL),
m_pToolBarBrowsing(NULL),
m_pComboBox(NULL)
{
  if (kind != WXAMAYAWINDOW_HELP && kind != WXAMAYAWINDOW_ANNOT)
    {
      m_haveTBEditing = Prof_ShowGUI("AmayaToolBarEditing");
      m_haveTBBrowsing = Prof_ShowGUI("AmayaToolBarBrowsing");
      s_normalWindowCount++;
    }
  else
    {
      m_haveTBEditing = false;
      m_haveTBBrowsing = false;
    }
}

/*----------------------------------------------------------------------
 *       Class:  AmayaNormalWindow
 *      Method:  ~AmayaNormalWindow
 * Description:  destructor
 -----------------------------------------------------------------------*/
AmayaNormalWindow::~AmayaNormalWindow()
{
  int kind = GetKind();
  if (kind != WXAMAYAWINDOW_HELP && kind != WXAMAYAWINDOW_ANNOT)
    s_normalWindowCount--;
}


/*----------------------------------------------------------------------
 *       Class:  AmayaNormalWindow
 *      Method:  Initialize
 * Description:  Initialize common part of all AmayaNormalWindow-based.
 -----------------------------------------------------------------------*/
bool AmayaNormalWindow::Initialize()
{
  return AmayaWindow::Initialize();
}

/*----------------------------------------------------------------------
 *       Class:  AmayaNormalWindow
 *      Method:  CreateStatusBar
 * Description:  
  -----------------------------------------------------------------------*/
AmayaStatusBar * AmayaNormalWindow::CreateStatusBar()
{
  return new AmayaStatusBar(this);
}

/*----------------------------------------------------------------------
 *       Class:  AmayaNormalWindow
 *      Method:  CreateMenuBar
 * Description:  
  -----------------------------------------------------------------------*/
void AmayaNormalWindow::CreateMenuBar()
{
  // Creation of the menubar
  TtaMakeWindowMenuBar(m_WindowId);

}

/*----------------------------------------------------------------------
 *       Class:  AmayaNormalWindow
 *      Method:  CleanUp
 * Description:  check that there is no empty pages
 -----------------------------------------------------------------------*/
void AmayaNormalWindow::CleanUp()
{
  if (GetPageCount() == 0)
    Close();
}

/*----------------------------------------------------------------------
 *       Class:  AmayaNormalWindow
 *      Method:  RegisterThotToolPanels
 * Description:  Register thot-side AmayaToolPanel.
 -----------------------------------------------------------------------*/
void AmayaNormalWindow::RegisterThotToolPanels()
{
  char         *s;

  s = TtaGetEnvString("CLASSIC_PANEL_ORDER");
  // detect an old panel configuration
  if (s == NULL || strstr (s, "AmayaXHTMLToolPanel"))
    TtaSetEnvString("CLASSIC_PANEL_ORDER",
                    "AmayaElementToolPanel;AmayaStyleToolPanel;AmayaApplyClassToolPanel;"
                    "AmayaAttributeToolPanel;AmayaSpeCharToolPanel;"
                    "StyleListToolPanel;AmayaExplorerToolPanel",
                    TRUE);

  RegisterToolPanelClass(CLASSINFO(AmayaExplorerToolPanel));
  RegisterToolPanelClass(CLASSINFO(AmayaElementToolPanel));
  RegisterToolPanelClass(CLASSINFO(AmayaAttributeToolPanel));
  RegisterToolPanelClass(CLASSINFO(AmayaApplyClassToolPanel));
  RegisterToolPanelClass(CLASSINFO(AmayaStyleToolPanel));
  RegisterToolPanelClass(CLASSINFO(AmayaSpeCharToolPanel));
}

/*----------------------------------------------------------------------
 *       Class:  AmayaNormalWindow
 *      Method:  LoadConfig
 * Description:  Load the config from registry and initialize dependancies
 -----------------------------------------------------------------------*/
void AmayaNormalWindow::LoadConfig()
{
  unsigned int  n;
  wxArrayString arr;
  ClassInfoSet::iterator it;
  char         *s;

  s = TtaGetEnvString("CLASSIC_PANEL_ORDER");
  wxString str = TtaConvMessageToWX(s);
  arr = wxStringTokenize(str, wxT(";"));

  for( it = g_AmayaToolPanelClassInfoSet.begin();
      it != g_AmayaToolPanelClassInfoSet.end(); ++it )
  {
      wxClassInfo *ci = *it;
      wxString name = ci->GetClassName();
      if(arr.Index(name, true)==wxNOT_FOUND)
        arr.Add(name);
  }
  
  
  for(n = 0; n<arr.GetCount(); n++)
    {
      wxClassInfo *ci = wxClassInfo::FindClass(arr[n]);
      if(ci && g_AmayaToolPanelClassInfoSet.find(ci)!=g_AmayaToolPanelClassInfoSet.end())
        {
          wxString name = ci->GetClassName();
          if (Prof_ShowGUI((const char*)wxString(name).mb_str(wxConvUTF8)))
            {
              wxObject* object = ci->CreateObject();
              AmayaToolPanel* panel = wxDynamicCast(object, AmayaToolPanel);
              if (panel)
                {
                  RegisterToolPanel(panel);
                  wxString str = wxT("OPEN_") + panel->GetToolPanelConfigKeyName();
                  TtaSetEnvBoolean((const char*)str.mb_str(wxConvUTF8),
                       panel->GetDefaultVisibilityState(), FALSE);
                }
              else
                delete object;
            }
        }
    }
}

/*----------------------------------------------------------------------
 *       Class:  AmayaNormalWindow
 *      Method:  SaveConfig
 * Description:  Save config to registry
 -----------------------------------------------------------------------*/
void AmayaNormalWindow::SaveConfig()
{
  int kind = GetKind();
  if (kind == WXAMAYAWINDOW_HELP || kind == WXAMAYAWINDOW_ANNOT)
    return; // don't save any preference
  if (SavePANEL_PREFERENCES)
    {
      if (IsToolBarShown(0))
        TtaSetEnvBoolean("BROWSE_TOOLBAR", TRUE, TRUE);
      else
        TtaSetEnvBoolean("BROWSE_TOOLBAR", FALSE, TRUE);
      if (IsToolBarShown(1))
        TtaSetEnvBoolean("EDIT_TOOLBAR", TRUE, TRUE);
      else
        TtaSetEnvBoolean("EDIT_TOOLBAR", FALSE, TRUE);
    }
  else
    {
      // by default display all available toolbars
      TtaSetEnvBoolean("BROWSE_TOOLBAR", TRUE, TRUE);
      TtaSetEnvBoolean("EDIT_TOOLBAR", TRUE, TRUE);
      // and set the default order
      TtaSetEnvString("CLASSIC_PANEL_ORDER",
                      "AmayaElementToolPanel;AmayaStyleToolPanel;AmayaApplyClassToolPanel;"
                      "StyleListToolPanel;AmayaExplorerToolPanel;AmayaAttributeToolPanel;"
                      "AmayaSpeCharToolPanel",
                       TRUE);
    }
  AmayaWindow::SaveConfig();
}


/*----------------------------------------------------------------------
 *       Class:  AmayaNormalWindow
 *      Method:  SendDataToPanel
 * Description:  send data to specified panel
 -----------------------------------------------------------------------*/
void AmayaNormalWindow::SendDataToPanel(int panel_type, AmayaParams& params)
{
  AmayaToolPanel* panel;
  
  int kind = GetKind();
  if (kind == WXAMAYAWINDOW_HELP || kind == WXAMAYAWINDOW_ANNOT)
    return;
  switch(panel_type)
  {
    case WXAMAYA_PANEL_XHTML:
    case WXAMAYA_PANEL_MATHML:
    case WXAMAYA_PANEL_XML:
    case WXAMAYA_PANEL_SVG:
      panel = GetToolPanel(WXAMAYA_PANEL_ELEMENTS);
      if(panel)
        ((AmayaElementToolPanel*)panel)->SendDataToPanel(panel_type, params);
      break;
    default:
      panel = GetToolPanel(panel_type);
      if(panel)
        panel->SendDataToPanel(params);
      break;      
  }
}

/*----------------------------------------------------------------------
 *       Class:  AmayaNormalWindow
 *      Method:  RaisePanel
 * Description:  Raiser the specified panel
 -----------------------------------------------------------------------*/
void AmayaNormalWindow::RaisePanel(int panel_type)
{
  AmayaToolPanel* panel;
  
  int kind = GetKind();
  if (kind == WXAMAYAWINDOW_HELP || kind == WXAMAYAWINDOW_ANNOT)
    return;
  switch(panel_type)
  {
    case WXAMAYA_PANEL_XHTML:
    case WXAMAYA_PANEL_MATHML:
    case WXAMAYA_PANEL_XML:
    case WXAMAYA_PANEL_SVG:
      panel = GetToolPanel(WXAMAYA_PANEL_ELEMENTS);
      if(panel)
        ((AmayaElementToolPanel*)panel)->RaisePanel(panel_type);
      break;
    default:
      panel = GetToolPanel(panel_type);
      if(panel)
        panel->Raise();
      break;      
  }
}

/*----------------------------------------------------------------------
 *       Class:  AmayaNormalWindow
 *      Method:  RaiseDoctypePanels
 * Description:  Raiser the specified panels
 -----------------------------------------------------------------------*/
void AmayaNormalWindow::RaiseDoctypePanels(int doctype)
{
  AmayaToolPanel* panel;
  
  panel = GetToolPanel(WXAMAYA_PANEL_ELEMENTS);
  if(panel)
    ((AmayaElementToolPanel*)panel)->RaiseDoctypePanels(doctype);
  
  panel = GetToolPanel(WXAMAYA_PANEL_STYLE);
  if(panel)
    ((AmayaStyleToolPanel*)panel)->RaiseDoctypePanels(doctype);
  
}


/*----------------------------------------------------------------------
 *       Class:  AmayaNormalWindow
 *      Method:  GetToolBarEditing
 * Description:  Return the toolbar panel for edition and create it if needed.
 -----------------------------------------------------------------------*/
wxPanel* AmayaNormalWindow::GetToolBarEditing()
{
  ThotBool show;

  int kind = GetKind();
  if (kind == WXAMAYAWINDOW_HELP || kind == WXAMAYAWINDOW_ANNOT)
    return NULL;
  if (!m_pToolBarEditing && m_haveTBEditing)
    {
      m_pToolBarEditing = wxXmlResource::Get()->LoadPanel(this, wxT("wxID_PANEL_TOOLBAR_EDITING"));
      TtaGetEnvBoolean ("EDIT_TOOLBAR", &show);
      if (!show)
        m_pToolBarEditing->Hide();
    }
  return m_pToolBarEditing;
}

/*----------------------------------------------------------------------
 *       Class:  AmayaNormalWindow
 *      Method:  GetToolBarBrowsing
 * Description:  Return a toolbar panel for browsing and create it if needed.
 -----------------------------------------------------------------------*/
wxPanel* AmayaNormalWindow::GetToolBarBrowsing()
{
  ThotBool show;

  int kind = GetKind();
  if (kind == WXAMAYAWINDOW_HELP || kind == WXAMAYAWINDOW_ANNOT)
    return NULL;
  if (!m_pToolBarBrowsing && m_haveTBBrowsing)
    {
      m_pToolBarBrowsing = wxXmlResource::Get()->LoadPanel(this, wxT("wxID_PANEL_TOOLBAR_BROWSING"));
      m_pComboBox = XRCCTRL(*m_pToolBarBrowsing, "wxID_TOOL_URL", wxComboBox);
      TtaGetEnvBoolean ("BROWSE_TOOLBAR", &show);
      if (!show)
        m_pToolBarBrowsing->Hide();
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
 *      Method:  GetActivePage
 * Description:  return the current selected page
 -----------------------------------------------------------------------*/
AmayaPage * AmayaNormalWindow::GetActivePage() const
{
  TTALOGDEBUG_0( TTA_LOG_DIALOG, _T("AmayaNormalWindow::GetActivePage") );
  if (!GetPageContainer())
    return NULL;
  return (GetPageContainer()->GetSelection() >= 0) ? GetPage(GetPageContainer()->GetSelection()) : NULL;
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
 *      Method:  DoCreatePage
 * Description:  Really create a page and return it.
 -----------------------------------------------------------------------*/
AmayaPage* AmayaNormalWindow::DoCreatePage( wxWindow* parent, Document doc, bool attach, int position)
{
  AmayaPage * page = AmayaPage::CreateAmayaPage(parent, this, doc );
  
  if (attach)
    AttachPage( position, page );
  
  return page;
}

/*----------------------------------------------------------------------
 *       Class:  AmayaNormalWindow
 *      Method:  AttachPage
 * Description:  really attach a page to the current window
 -----------------------------------------------------------------------*/
bool AmayaNormalWindow::AttachPage( int position, AmayaPage * p_page )
{
  bool ret;
  if (!GetPageContainer())
    ret = false;
  else
    {
      /* notebook is a new parent for the page
       * warning: AmayaPage original parent must be a wxNotbook */
      //    p_page->Reparent( m_pNotebook );
      p_page->SetContainer( GetPageContainer() );
    
      /* insert the page in the notebook */
      ret = GetPageContainer()->InsertPage( position,
                                     p_page,
                                     _T(""),  /* this is the page name */
                                     false,
                                     0 ); /* this is the default image id */

      // update the pages ids
      GetPageContainer()->UpdatePageId();

      // the inserted page should be forced to notebook size
      GetPageContainer()->Layout();
      TTALOGDEBUG_2( TTA_LOG_DIALOG, _T("AmayaNormalWindow::AttachPage - pagesize: w=%d h=%d"),
                     p_page->GetSize().GetWidth(),
                     p_page->GetSize().GetHeight());

      SetAutoLayout(TRUE);
    }
  return ret;
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

  if (GetPageContainer() == NULL)
    return true;
  else
    return GetPageContainer()->ClosePage(page_id);
}

/*----------------------------------------------------------------------
 *       Class:  AmayaNormalWindow
 *      Method:  CloseAllButPage
 * Description:  ferme toutes les pages sauf une
 -----------------------------------------------------------------------*/
bool AmayaNormalWindow::CloseAllButPage( int position )
{
  if(GetPageContainer())
    return GetPageContainer()->CloseAllButPage(position);
  else
    return false;
}

/*----------------------------------------------------------------------
 *       Class:  AmayaNormalWindow
 *      Method:  GetPage
 * Description:  search the page at given position
 -----------------------------------------------------------------------*/
AmayaPage * AmayaNormalWindow::GetPage( int position )const
{
  TTALOGDEBUG_0( TTA_LOG_DIALOG, _T("AmayaNormalWindow::GetPage") );
  if (!GetPageContainer())
    return NULL;
  if (GetPageCount() <= position)
    return NULL;
  return (AmayaPage *)GetPageContainer()->GetPage(position);
}

/*----------------------------------------------------------------------
 *       Class:  AmayaNormalWindow
 *      Method:  GetPage
 * Description:  how many page into the window
 -----------------------------------------------------------------------*/
int AmayaNormalWindow::GetPageCount() const
{
  if (!GetPageContainer())
    return 0;
  return (int)GetPageContainer()->GetPageCount();
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
  
  m_enteredURL = new_url;
  
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
    {
      wxString path = m_pComboBox->GetValue ();
      return path.Trim(TRUE).Trim(FALSE);
    }
  else
    return wxT("");
}

/*----------------------------------------------------------------------
 *       Class:  AmayaNormalWindow
 *      Method:  AppendURL
 * Description:  Append an url to the urlbar and to the MRU menu
 -----------------------------------------------------------------------*/
void AmayaNormalWindow::AppendURL ( const wxString & new_url )
{
  // Add the url to the MRU menu
  wxMenu     *menu;
  wxMenuItem *item;
  if(m_URLs.GetCount() < RECENT_DOC_MAX_NB)
    {
      if(m_URLs.Index(new_url)==wxNOT_FOUND)
        {
          if(GetMenuBar() && GetMenuBar()->GetMenu(0))
            {
              item = GetMenuBar()->GetMenu(0)->FindItem(RECENT_DOC_ID, &menu);
              if(item)
                {
                  if(m_URLs.IsEmpty())
                    menu->Destroy(RECENT_DOC_ID);
                  menu->Append(RECENT_DOC_ID+m_URLs.GetCount(), new_url);
                }
            }
          m_URLs.Add(new_url);
        }
    }
  
  // Add the url to the urlbar
  if(m_pComboBox)
    m_pComboBox->Append( new_url );
}

/*----------------------------------------------------------------------
 *       Class:  AmayaNormalWindow
 *      Method:  EmptyURLBar
 * Description:  remove all items in the url bar
 *               remove MRU from menu.
 -----------------------------------------------------------------------*/
void AmayaNormalWindow::EmptyURLBar()
{
  // Remove MRU from menu
  wxMenu     *menu;
  wxMenuItem *item;
  if(GetMenuBar() && GetMenuBar()->GetMenu(0))
    {
      item = GetMenuBar()->GetMenu(0)->FindItem(RECENT_DOC_ID, &menu);
      if(item && !m_URLs.IsEmpty())
        {
          for(int i=RECENT_DOC_ID; i<RECENT_DOC_ID+RECENT_DOC_MAX_NB; i++)
            {
              if(menu->FindItem(i))
                menu->Destroy(i);
              else
                break;
            }
          menu->Append(RECENT_DOC_ID,
              TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_NO_RECENT_DOC)))
              ->Enable(false);
        }
    }
  m_URLs.Clear();

  // Cleare url bar
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

  if (action >= 0)
    while (doc_id < MAX_DOCUMENTS)
      {
        if (LoadedDocument[doc_id - 1])
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
  GotoSelectedURL (TRUE);
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
      if (p_frame && m_pComboBox)
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
  GotoSelectedURL (FALSE);
  // do not skip this event because we don't want to propagate this event
  // event.Skip();
}

/*----------------------------------------------------------------------
 *       Class:  AmayaNormalWindow
 *      Method:  GotoSelectedURL
 * Description:  validate the selection
  -----------------------------------------------------------------------*/
void AmayaNormalWindow::GotoSelectedURL(ThotBool noreplace)
{
  Document doc;
  View     view;
  char     buffer[MAX_LENGTH];
      
  FrameToView (TtaGiveActiveFrame(), &doc, &view);
  CloseTextInsertion ();
  /* call the callback  with the url selected text */
  PtrDocument pDoc = LoadedDocument[doc-1];
  wxASSERT(pDoc);
  if (pDoc && pDoc->Call_Text)
    {
      if(m_pComboBox)
        strncpy(buffer, (m_pComboBox->GetValue()).Trim(TRUE).Trim(FALSE).mb_str(wxConvUTF8), MAX_LENGTH-1);
      else
        strncpy(buffer, m_enteredURL.Trim(TRUE).Trim(FALSE).mb_str(wxConvUTF8), MAX_LENGTH-1);
      buffer[MAX_LENGTH-1] = EOS;
      // patch to go-round a bug on Windows (TEXT_ENTER event called twice)
#ifdef _WINDOWS
      if (isBufUrl == FALSE)
        {
          isBufUrl = TRUE;
          (*(Proc4)pDoc->Call_Text) ((void *)doc, (void *)view, (void *)buffer,  (void *)noreplace);
          strcpy (BufUrl, buffer);
          isBufUrl = FALSE;
        }
      else if (strcmp (buffer, BufUrl) != 0)
        {
          (*(Proc4)pDoc->Call_Text) ((void *)doc, (void *)view, (void *)buffer,  (void *)noreplace);
          strcpy (BufUrl, buffer);
        }
#else /* _WINDOWS */
      (*(Proc4)pDoc->Call_Text) ((void *)doc, (void *)view, (void *)buffer,  (void *)noreplace);
#endif /* _WINDOWS */
    }
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
  wxMenu *         p_context_menu      = TtaGetContextMenu( GetWindowId() );
  Document   doc;
  View       view;

#ifdef _WINDOWS
  // a patch to manage enter in the URL combo
  if (p_combo_box && id == 1)
	  GotoSelectedURL (FALSE);
#endif /* _WINDOWS */
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
  if (p_menu && p_menu == p_context_menu)
    FrameToView (GetPageContainer()->GetMContextFrame(), &doc, &view);
  else
    FrameToView (TtaGiveActiveFrame(), &doc, &view);
  AmayaWindow::DoAmayaAction( action_id, doc, view );
  if (action_id == -1)
    // Open recent documents
    event.Skip();
  else if (MenuActionList[action_id].ActionName &&
           (!strcmp (MenuActionList[action_id].ActionName, "TtcPreviousElement") ||
            !strcmp (MenuActionList[action_id].ActionName, "TtcNextElement")))
  event.Skip(false);
}

/*----------------------------------------------------------------------
 *       Class:  AmayaNormalWindow
 *      Method:  PrepareRecentDocumentMenu
 * Description:  Modify the menu to prepare insertion of the MRU
 -----------------------------------------------------------------------*/
void AmayaNormalWindow::PrepareRecentDocumentMenu(wxMenuItem* item)
{
  if (item)
    {
      wxMenu* menu = item->GetMenu();
      menu->AppendSeparator();
      menu->Append(RECENT_DOC_ID,
          TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_NO_RECENT_DOC)))
          ->Enable(false);
    }
}

/*----------------------------------------------------------------------
 *       Class:  AmayaNormalWindow
 *      Method:  OnRecentDocMenu
 * Description:  
 -----------------------------------------------------------------------*/
void AmayaNormalWindow::OnRecentDocMenu(wxCommandEvent& event)
{
  int id = event.GetId() - RECENT_DOC_ID;
  if(id < (int)m_URLs.GetCount())
    {
      wxString str = m_URLs[id];
      if(str)
        {
          SetURL(str);
          GotoSelectedURL (TRUE);
        }
    }
}

/*----------------------------------------------------------------------
 *       Class:  AmayaNormalWindow
 *      Method:  OnClose
 * Description:  
 -----------------------------------------------------------------------*/
void AmayaNormalWindow::OnClose(wxCloseEvent& event)
{
  event.Skip();
  
  if(!IsKindOf(CLASSINFO(AmayaHelpWindow)))
    {
      AmayaWindowIterator it;
      AmayaHelpWindow* help = NULL;
      for ( it.first(); !it.isDone(); it.next() )
        {
          AmayaWindow* win = (AmayaWindow*)it.currentElement();
          if (win && win != this)
            {
              if (!win->IsKindOf(CLASSINFO(AmayaHelpWindow)))
                // There is at least one window not help nor closed.
                // Dont do anything
                return;
              else
                help = (AmayaHelpWindow*)win;
            }
        }
      if (help)
        {
          AmayaFrame* frame = help->GetActiveFrame();
          if(frame)
            {
              int id = frame->GetMasterFrameId();
              TtaPostMenuAction("CloseHelpWindow", FrameTable[id].FrDoc, FrameTable[id].FrView, TRUE);
            }
        }
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
  
  EVT_MENU(wxID_ANY,   AmayaNormalWindow::OnMenuItem )
  EVT_MENU_HIGHLIGHT_ALL( AmayaNormalWindow::OnMenuHighlight )
   
  EVT_COMBOBOX( XRCID("wxID_TOOL_URL"),   AmayaNormalWindow::OnURLSelected )
  EVT_TEXT_ENTER( XRCID("wxID_TOOL_URL"), AmayaNormalWindow::OnURLTextEnter )
  EVT_TEXT( XRCID("wxID_TOOL_URL"),       AmayaNormalWindow::OnURLText )
  
  EVT_CLOSE(AmayaNormalWindow::OnClose)
  
  EVT_MENU_RANGE(RECENT_DOC_ID, RECENT_DOC_ID+RECENT_DOC_MAX_NB,
                                        AmayaNormalWindow::OnRecentDocMenu)
  
END_EVENT_TABLE()

  
  
  
#endif /* #ifdef _WX */
