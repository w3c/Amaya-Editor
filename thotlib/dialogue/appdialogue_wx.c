#ifdef _WX
  #include "wx/wx.h"
  #include "wx/bmpbuttn.h"
  #include "wx/spinctrl.h"
  #include "wx/socket.h"
#endif /* _WX */

#include "thot_gui.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "frame.h"
#include "registry.h"
#include "appdialogue.h"
#include "message.h"
#include "dialogapi.h"
#include "application.h"
#include "appdialogue_wx.h"
#include "windowtypes_wx.h"
#include "paneltypes_wx.h"
#include "message_wx.h"
#include "registry_wx.h"
#include "panel.h"
#include "thot_key.h"
#include "logdebug.h"

#include "appdialogue_f.h"
#include "appdialogue_wx_f.h"
#include "applicationapi_f.h"
#include "font_f.h"
#include "editcommands_f.h"
#include "profiles_f.h"
#include "displayview_f.h"
#include "input_f.h"

/* implement panel array */
#define THOT_EXPORT
#include "panel_tv.h"

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "edit_tv.h"
#include "appdialogue_tv.h"
#include "frame_tv.h"
#include "boxparams_f.h"
#include "thotmsg_f.h"
#include "dialogapi_f.h"

#include "AmayaApp.h"
#include "AmayaCanvas.h"
#include "AmayaFrame.h"
#include "AmayaNormalWindow.h"
#include "AmayaSimpleWindow.h"
#include "AmayaPage.h"
#include "AmayaToolBar.h"
#include "AmayaNotebook.h"
#include "AmayaPanel.h"
#include "AmayaSubPanel.h"
#include "AmayaXHTMLPanel.h"
#include "AmayaSubPanelManager.h"
#include "AmayaStatsThread.h"

static int g_back_action_id = -1;
static int g_forward_action_id = -1;
static int g_reload_action_id = -1;
static int g_stop_action_id = -1;
static int g_home_action_id = -1;
static int g_save_action_id = -1;
static int g_print_action_id = -1;
static int g_find_action_id = -1;
static int g_logo_action_id = -1;

static void TtaMakeWindowMenuBar( int window_id );
static void BuildPopdownWX ( int window_id, Menu_Ctl *ptrmenu, ThotMenu p_menu );

/* 
 * In this file there is a list of functions useful
 * to build step by step the user interface.
 *
 * The window (AmayaWindow) is the top container but 
 * this window is composed of several components :
 * +[AmayaWindow]-------------------------------------+
 * |+------------------------------------------------+|
 * || MenuBar                                        ||
 * |+------------------------------------------------+|
 * |+------------------------------------------------+|
 * || ToolBar                                        ||
 * |+------------------------------------------------+|
 * |+[AmayaPanel]--+ +[AmayaPage]-------------------+ |
 * ||              | |[AmayaFrame]                  | |
 * ||              | |                              | |
 * ||              | |    (view container)  'Top'   | |
 * ||              | |-----------SplitBar-----------| |
 * ||              | |[AmayaFrame]          'Bottom'| |
 * ||              | |                              | |
 * |+--------------+ +------------------------------+ |
 * |+------------------------------------------------+|
 * || StatusBar                                      ||
 * |+------------------------------------------------+|
 * +--------------------------------------------------+
 */

/*----------------------------------------------------------------------
  TtaShowWindow show or hide a window
  params:
    + int window_id
    + bool show : if true then show, if false then hide the window
  ----------------------------------------------------------------------*/
void TtaShowWindow( int window_id, ThotBool show )
{
#ifdef _WX  
  AmayaWindow * p_window = WindowTable[window_id].WdWindow;
  if (p_window == NULL)
    return;

  p_window->Show( show );
#endif /* _WX */
}

/*----------------------------------------------------------------------
  TtaMakeWindow create a AmayaWindow object and place it
  into WindowTable array
  returns:
 	+ the window id
        + 0 if too much created windows
  ----------------------------------------------------------------------*/
int TtaMakeWindow( int x, int y, int w, int h, int kind, int parent_window_id )
{
#ifdef _WX
  AmayaWindow * p_window = NULL;
  int window_id = TtaGetFreeWindowId();
 
  if (window_id >= MAX_WINDOW)
    return 0; /* there is no more free windows */

  /* get the parent window pointer */
  AmayaWindow * p_parent_window = TtaGetWindowFromId( parent_window_id );
  
  wxSize window_size;
  if (w != 0 && h != 0)
    window_size = wxSize(w, h);
  else
    window_size = wxSize(800, 600);

  wxPoint window_pos(x, y);

  /* Create the window */
  switch ( kind )
    {
    case WXAMAYAWINDOW_NORMAL:
      p_window = new AmayaNormalWindow( window_id,
					p_parent_window,
					window_pos,
					window_size );
      break;
    case WXAMAYAWINDOW_SIMPLE:
      p_window = new AmayaSimpleWindow( window_id,
					p_parent_window,
				        window_pos,
					window_size );
      break;
    }    

  if (!p_window)
    return -1; /* no enough memory */

  /* save the window reference into the global array */ 
  WindowTable[window_id].WdWindow = p_window;
  WindowTable[window_id].FrWidth  = p_window->GetSize().GetWidth();
  WindowTable[window_id].FrHeight = p_window->GetSize().GetHeight();
  WindowTable[window_id].WdStatus = p_window->GetStatusBar();

  // setup window panel
  AmayaPanel * p_panel = p_window->GetAmayaPanel();
  if (p_panel)
    {
      /* init default panel states */
      TtaSetEnvBoolean("OPEN_PANEL", TRUE, FALSE);
      TtaSetEnvBoolean("OPEN_PANEL_XHTML", TRUE, FALSE);
      TtaSetEnvBoolean("OPEN_PANEL_ATTRIBUTE", TRUE, FALSE);
      TtaSetEnvBoolean("OPEN_PANEL_XML", FALSE, FALSE);
      TtaSetEnvBoolean("OPEN_PANEL_MATHML", FALSE, FALSE);
      TtaSetEnvBoolean("OPEN_PANEL_SPECHAR", FALSE, FALSE);
      TtaSetEnvBoolean("OPEN_PANEL_APPLYCLASS", TRUE, FALSE);
      TtaSetEnvBoolean("OPEN_PANEL_COLORS", TRUE, FALSE);
      TtaSetEnvBoolean("OPEN_PANEL_CHARSTYLE", TRUE, FALSE);
      TtaSetEnvBoolean("OPEN_PANEL_FORMAT", TRUE, FALSE);
      
      /* open or close panels */
      ThotBool value;
      TtaGetEnvBoolean ("OPEN_PANEL", &value);
      if (value)
	p_window->OpenPanel();
      else
	p_window->ClosePanel();
      TtaGetEnvBoolean ("OPEN_PANEL_XHTML", &value);
      if (value)
	p_panel->OpenSubPanel( WXAMAYA_PANEL_XHTML );
      else
	p_panel->CloseSubPanel( WXAMAYA_PANEL_XHTML );
      TtaGetEnvBoolean ("OPEN_PANEL_ATTRIBUTE", &value);
      if (value)
	p_panel->OpenSubPanel( WXAMAYA_PANEL_ATTRIBUTE );
      else
	p_panel->CloseSubPanel( WXAMAYA_PANEL_ATTRIBUTE );
      TtaGetEnvBoolean ("OPEN_PANEL_XML", &value);
      if (value)
	p_panel->OpenSubPanel( WXAMAYA_PANEL_XML );
      else
	p_panel->CloseSubPanel( WXAMAYA_PANEL_XML );
      TtaGetEnvBoolean ("OPEN_PANEL_MATHML", &value);
      if (value)
	p_panel->OpenSubPanel( WXAMAYA_PANEL_MATHML );
      else
	p_panel->CloseSubPanel( WXAMAYA_PANEL_MATHML );
      TtaGetEnvBoolean ("OPEN_PANEL_COLORS", &value);
      if (value)
	p_panel->OpenSubPanel( WXAMAYA_PANEL_COLORS );
      else
	p_panel->CloseSubPanel( WXAMAYA_PANEL_COLORS );
      TtaGetEnvBoolean ("OPEN_PANEL_CHARSTYLE", &value);
      if (value)
	p_panel->OpenSubPanel( WXAMAYA_PANEL_CHARSTYLE );
      else
	p_panel->CloseSubPanel( WXAMAYA_PANEL_CHARSTYLE );
      TtaGetEnvBoolean ("OPEN_PANEL_FORMAT", &value);
      if (value)
	p_panel->OpenSubPanel( WXAMAYA_PANEL_FORMAT );
      else
	p_panel->CloseSubPanel( WXAMAYA_PANEL_FORMAT );
      TtaGetEnvBoolean ("OPEN_PANEL_APPLYCLASS", &value);
      if (value)
	p_panel->OpenSubPanel( WXAMAYA_PANEL_APPLYCLASS );
      else
	p_panel->CloseSubPanel( WXAMAYA_PANEL_APPLYCLASS );
      TtaGetEnvBoolean ("OPEN_PANEL_SPECHAR", &value);
      if (value)
	p_panel->OpenSubPanel( WXAMAYA_PANEL_SPECHAR );
      else
	p_panel->CloseSubPanel( WXAMAYA_PANEL_SPECHAR );
    }

  /* do not create menus for a simple window */
  if (kind != WXAMAYAWINDOW_SIMPLE)
    TtaMakeWindowMenuBar( window_id );
  
  return window_id;
#else
  return 0;
#endif /* #ifdef _WX */
}

/*----------------------------------------------------------------------
  BuildPopdownWX builds menus items and sub-menus and hangs it to the given p_menu
  ----------------------------------------------------------------------*/
static void BuildPopdownWX ( int window_id, Menu_Ctl *ptrmenu, ThotMenu p_menu )
{
#ifdef _WX
  Item_Ctl *   ptritem = ptrmenu->ItemsList;
  Menu_Ctl *   item_submenu = NULL;
  int          item_nb = 0;
  int          item_id = 0;
  char *       item_label = NULL;
  int          item_label_lg = 0;
  char *       item_equiv = NULL;
  char *       item_icon = NULL;
  char         item_type = ' ';
  int          item_action = 0;
  wxMenuItem * p_menu_item = NULL;
  wxMenu *     p_submenu = NULL;
  int          max_item_label_lg = 0;
  wxString     label;
  
  /* first of all check for the largest menuitem label */
  while (item_nb < ptrmenu->ItemsNb)
    {
      item_id       = ptritem[item_nb].ItemID;
      item_label    = TtaGetMessage (THOT, item_id);
     item_label_lg = strlen(item_label);
      if ( max_item_label_lg < item_label_lg )
	max_item_label_lg = item_label_lg;
      item_nb++;
    }
  
  /* now create the menu items */
  item_nb = 0;
  while (item_nb < ptrmenu->ItemsNb)
    {
      item_id      = ptritem[item_nb].ItemID;
      item_label   = TtaGetMessage (THOT, item_id);
      item_type    = ptritem[item_nb].ItemType;
      item_icon    = ptritem[item_nb].ItemIconName;

      label = TtaConvMessageToWX(item_label);
      label.Pad(max_item_label_lg-strlen(item_label)+1, wxChar(' '));

      switch (item_type)
	{
	case 'B': /* a normal menu item */
	  item_action  = ptritem[item_nb].ItemAction;
	  item_equiv   = MenuActionList[item_action].ActionEquiv;
	  label += TtaConvMessageToWX(item_equiv);
	  p_menu_item = new wxMenuItem(p_menu, item_id, label, _T(""), wxITEM_NORMAL);
	  break;

	case 'S': /* a separator */
	  item_action  = -1;
	  if (item_nb+1<ptrmenu->ItemsNb)
	    p_menu_item = new wxMenuItem(p_menu, wxID_SEPARATOR, _T(""), _T(""), wxITEM_SEPARATOR);
	  else
	    p_menu_item = NULL; /* do not add a separator if it's the last menu item */
	  break;

	case 'T': /* a toggle menu item (checkbox) */
	  item_action  = ptritem[item_nb].ItemAction;
	  item_equiv   = MenuActionList[item_action].ActionEquiv;
	  label += TtaConvMessageToWX(item_equiv);
	  p_menu_item = new wxMenuItem(p_menu, item_id, label, _T(""), wxITEM_CHECK);
	  break;

	case 'M': /* a submenu */
	  item_action  = -1;
	  item_submenu = ptritem[item_nb].SubMenu;
	  if (item_submenu->ItemsNb>0)
	    {
	      p_submenu = new wxMenu();
	      BuildPopdownWX( window_id, item_submenu, p_submenu );
	      p_menu_item = new wxMenuItem(p_menu, item_id, label, _T(""), wxITEM_NORMAL, p_submenu);
	    }
	  else
	    p_menu_item = NULL;
	  break;

	default: /* a unknown type */
	  item_action  = -1;
	  p_menu_item = NULL;
	  wxASSERT_MSG(FALSE, _T("Unknown menu item type"));
	  break;
	}

      if (item_action != -1)
	{
	  /* Is it the "Show/Hide panel" command */
	  if (!strcmp (MenuActionList[item_action].ActionName, "ShowPanel"))
	    WindowTable[window_id].MenuItemShowPanelID = item_id;
	  /* Is it the "Split/Unsplit view" command */
	  else if (!strcmp (MenuActionList[item_action].ActionName, "SplitUnsplitPage"))
	    WindowTable[window_id].MenuItemSplitViewID = item_id;
	  /* Is it the "Fullscreen on/off" command */
	  else if (!strcmp (MenuActionList[item_action].ActionName, "FullScreen"))
	    WindowTable[window_id].MenuItemFullScreenID = item_id;
	  /* Is it the "Paste" command */
	  else if (!strcmp (MenuActionList[item_action].ActionName, "PasteBuffer"))
	    WindowTable[window_id].MenuItemPaste = item_id;
	  /* Is it the "Undo" command */
	  else if (!strcmp (MenuActionList[item_action].ActionName, "TtcUndo"))
	    WindowTable[window_id].MenuItemUndo = item_id;
	  /* Is it the "Redo" command */
	  else if (!strcmp (MenuActionList[item_action].ActionName, "TtcRedo"))
	    WindowTable[window_id].MenuItemRedo = item_id;
#ifdef _MACOS
	  else if (!strcmp (MenuActionList[item_action].ActionName, "HelpAmaya"))
	    wxApp::s_macAboutMenuItemId = item_id;
	  else if (!strcmp (MenuActionList[item_action].ActionName, "ConfigAmaya"))
	    wxApp::s_macPreferencesMenuItemId = item_id;
	  else if (!strcmp (MenuActionList[item_action].ActionName, "AmayaClose"))
	    wxApp::s_macExitMenuItemId = item_id;
#endif /* _MACOS */
 	}

      if ( p_menu_item &&
	   item_icon[0] != '\0' &&
	   item_action != -1 &&
	   item_type != 'T' )
	{
	  wxBitmap menu_icon(TtaGetResourcePathWX(WX_RESOURCES_ICON_16X16,item_icon), wxBITMAP_TYPE_PNG);
	  if (menu_icon.Ok())
	    p_menu_item->SetBitmap( menu_icon );
	}

      if (p_menu_item)
	p_menu->Append(p_menu_item);

      item_nb++;
    }
#endif /* _WX */
}



/*----------------------------------------------------------------------
  TtaMakeWindowMenuBar creates the window menu bar widgets from the model (DocumentMenuList)
  ----------------------------------------------------------------------*/
static void TtaMakeWindowMenuBar( int window_id )
{
#ifdef _WX
  AmayaWindow        *p_window = TtaGetWindowFromId( window_id );
  Menu_Ctl           *ptrmenu = DocumentMenuList; /* this is the menus model */
  wxMenuBar          *p_menu_bar = p_window->GetMenuBar();

  if (p_menu_bar)
    /* destroy the old menubar, because we are rebuilding a new one */
    p_menu_bar->Destroy();
  p_menu_bar = new wxMenuBar( wxMB_DOCKABLE );

  while (ptrmenu)
    {
      /* Check if a menu has to be displayed. */
      if ( Prof_ShowMenu (ptrmenu) )
	{
	  wxMenu * p_menu = new wxMenu();
	  
	  /* remember the top menubar widgets because wxMenu doesn't have ids to identify it */
	  WindowTable[window_id].WdMenus[ptrmenu->MenuID] = p_menu;

	  /* remember specials menus */
	  if (ptrmenu->MenuContext) 
	    WindowTable[window_id].MenuContext = ptrmenu->MenuID;
	  else if (ptrmenu->MenuAttr)
	    WindowTable[window_id].MenuAttr = ptrmenu->MenuID;
	  else if (ptrmenu->MenuSelect) 
	    WindowTable[window_id].MenuSelect = ptrmenu->MenuID;
	  else if (ptrmenu->MenuHelp) 
	    WindowTable[window_id].MenuHelp = ptrmenu->MenuID;
	  
	  /* Now create the menu's widgets and hangs it to our p_menu */
	  BuildPopdownWX( window_id, ptrmenu, p_menu );
	  
	  /* Le menu contextuel ne doit pas etre accroche a notre bar de menu */
	  /* il sera affiche qd le boutton droit de la souris sera active */
	  if (!ptrmenu->MenuContext)
	    p_menu_bar->Append( p_menu,
				TtaConvMessageToWX( TtaGetMessage (THOT, ptrmenu->MenuID) ) );
 
	}
      ptrmenu = ptrmenu->NextMenu;
    }

  p_window->SetMenuBar( p_menu_bar );
#endif /* _WX */
}

/*----------------------------------------------------------------------
  TtaInitMenuItemStats enable/disable, toggle/untoggle menu items for the given doc
 ----------------------------------------------------------------------*/
void TtaInitMenuItemStats( int doc_id )
{
#ifdef _WX
  /* loop on each menu actions */
  int action_id = MAX_INTERNAL_CMD;
  while ( action_id < TtaGetMenuActionNumber() )
    {
      /* by default everything is active,
       * TtaSetMenuOff/TtaSetMenuOn will change the state further */
      MenuActionList[action_id].ActionActive[doc_id] = TRUE;
      MenuActionList[action_id].ActionToggle[doc_id] = FALSE;
      action_id++;
    }
#endif /* _WX */
}

/*----------------------------------------------------------------------
  TtaInitTopMenuStats enable/disable, toggle/untoggle top menu for the given doc
 ----------------------------------------------------------------------*/
void TtaInitTopMenuStats( int doc_id )
{
#ifdef _WX
  /* enable every menu */
  PtrDocument pDoc = LoadedDocument[doc_id-1];
  memset(pDoc->EnabledMenus, TRUE, sizeof(pDoc->EnabledMenus));
#endif /* _WX */
}

/*----------------------------------------------------------------------
  TtaRefreshMenuStats enable/disable top menubar menus for the given doc
 ----------------------------------------------------------------------*/
void TtaRefreshTopMenuStats( int doc_id, int menu_id )
{
#ifdef _WX
  int           window_id  = TtaGetDocumentWindowId( doc_id, -1 );
  AmayaWindow * p_window   = TtaGetWindowFromId(window_id);
  wxASSERT(p_window);
  wxMenuBar *   p_menu_bar = p_window->GetMenuBar();
  PtrDocument   pDoc       = LoadedDocument[doc_id-1];
  wxMenu *      p_top_menu = NULL;
  int           top_menu_pos = 0;

  /* do nothing if there is no menubar : it's the case of AmayaSimpleWindow (log, show apply style ...)*/
  if(!p_menu_bar)
    return;

  /* check that the current menu correspond to the current document
   * do nothing if the current document is not the refreshed one */
  Document      active_doc_id;
  View          view;
  FrameToView(TtaGiveActiveFrame(), &active_doc_id, &view);
  if (active_doc_id != doc_id)
    return;

  /* refresh only one menu ? */
  if (menu_id >= 0 && menu_id < MAX_MENU)
    {
      p_top_menu = WindowTable[window_id].WdMenus[menu_id];
      if (p_top_menu)
	{
	  // find the corrsponding menu position in the Top Menubar
	  top_menu_pos = 0;
	  while (top_menu_pos < p_menu_bar->GetMenuCount() && p_menu_bar->GetMenu(top_menu_pos) != p_top_menu)
        top_menu_pos++;
	  // we must check that the menu has been found because the contextual menu do not have a title
	  if (top_menu_pos >= 0 && top_menu_pos < p_menu_bar->GetMenuCount())
	    {
	      // it has been found, update it
	      p_menu_bar->EnableTop(top_menu_pos, pDoc->EnabledMenus[menu_id]);
	    }
	  else
	  {
		  wxASSERT_MSG(FALSE,_T("didn't find the top menu"));
	  }
	}
      return;
    }

  /* refresh every menus in the menubar */
  menu_id = 0;
  while (menu_id < MAX_MENU)
    {
      p_top_menu = WindowTable[window_id].WdMenus[menu_id];
      if (p_top_menu)
	{
	  // find the corrsponding menu position in the Top Menubar
	  top_menu_pos = 0;
	  while (top_menu_pos < p_menu_bar->GetMenuCount() && p_menu_bar->GetMenu(top_menu_pos) != p_top_menu)
        top_menu_pos++;
	  // we must check that the menu has been found because the contextual menu do not have a title
	  if (top_menu_pos >= 0 && top_menu_pos < p_menu_bar->GetMenuCount())
	    {
	      // it has been found, update it
	      p_menu_bar->EnableTop(top_menu_pos, pDoc->EnabledMenus[menu_id]);
	    }
	}
      menu_id++;
    }
#endif /* _WX */
}

  /* ---------------------------------------------------------------- */

/*----------------------------------------------------------------------
  TtaRefreshMenuItemStats enable/disable, toggle/untoggle menu items widgets for the given doc
 ----------------------------------------------------------------------*/
void TtaRefreshMenuItemStats( int doc_id, Menu_Ctl * ptrmenu, int menu_item_id )
{
#ifdef _WX
  int           window_id  = TtaGetDocumentWindowId( doc_id, -1 );
  AmayaWindow * p_window   = TtaGetWindowFromId(window_id);
  
  if (!p_window)
	return;

  wxMenuBar *   p_menu_bar = p_window->GetMenuBar();
  wxMenuItem *  p_menu_item = NULL;
  Item_Ctl *    ptritem = NULL;
  Menu_Ctl *    item_submenu = NULL;
  int           item_nb = 0;
  int           item_id = menu_item_id;
  char          item_type = ' ';
  int           item_action = 0;
  ThotBool      item_enable = FALSE;
  ThotBool      item_toggle = FALSE;

  /* do nothing if there is no menubar : it's the case of AmayaSimpleWindow (log, show apply style ...)*/
  if(!p_menu_bar)
    return;

  /* check that the current menu correspond to the current document
   * do nothing if the current document is not the refreshed one */
  Document      active_doc_id;
  View          view;
  FrameToView(TtaGiveActiveFrame(), &active_doc_id, &view);
  if (active_doc_id != doc_id)
    return;
  
  if (ptrmenu == NULL)
    ptrmenu = DocumentMenuList; /* this is the menus model */

  /* try to refresh only the given menu item */
  if (menu_item_id != -1)
    {
      p_menu_item = p_menu_bar->FindItem(item_id);
      item_action = FindMenuActionFromMenuItemID(ptrmenu, item_id);
      if (item_action != -1)
	{
	  p_menu_item = p_menu_bar->FindItem(item_id);
	  wxASSERT(p_menu_item);

	  /* enable or disable the item */
	  item_enable = MenuActionList[item_action].ActionActive[doc_id];
	  p_menu_item->Enable(item_enable);

	  /* check or uncheck a checkable item */
	  if (p_menu_item->GetKind() == wxITEM_CHECK)
	    {
	      item_toggle = MenuActionList[item_action].ActionToggle[doc_id];	      
	      p_menu_item->Check(item_toggle);
	    }

	  /* refresh the corresponding toolbar tool */
	  TtaRefreshToolbarStats( item_action );
	}
      return;
    }


  /* refresh every entry */
  while (ptrmenu)
    {
      ptritem = ptrmenu->ItemsList;

      item_nb = 0;      
      while (item_nb < ptrmenu->ItemsNb)
	{
	  item_id      = ptritem[item_nb].ItemID;
	  item_type    = ptritem[item_nb].ItemType;
	  
	  switch (item_type)
	    {
	    case 'B': /* a normal menu item */
	      item_action  = ptritem[item_nb].ItemAction;
	      item_enable  = MenuActionList[item_action].ActionActive[doc_id];
	      p_menu_bar->Enable(item_id, item_enable);
	      /* refresh the corresponding toolbar tool */
	      TtaRefreshToolbarStats( item_action );
	      break;
	      	      
	    case 'T': /* a toggle menu item (checkbox) */
	      item_action  = ptritem[item_nb].ItemAction;
	      item_enable  = MenuActionList[item_action].ActionActive[doc_id];
	      item_toggle  = MenuActionList[item_action].ActionToggle[doc_id];
	      p_menu_bar->Check(item_id, item_toggle);
	      p_menu_bar->Enable(item_id, item_enable);
	      /* refresh the corresponding toolbar tool */
	      TtaRefreshToolbarStats( item_action );
	      break;
	      
	    case 'M': /* a submenu */
	      item_submenu = ptritem[item_nb].SubMenu;
	      TtaRefreshMenuItemStats( doc_id, item_submenu, menu_item_id );
	      break;

	    case 'S': /* a separator */
	      break;

	    default: /* a unknown type */
	      wxASSERT(FALSE);
	      break;
	    }
	  
	  item_nb++;
	}
      
      ptrmenu = ptrmenu->NextMenu;
    }
#endif /* _WX */
}

/*----------------------------------------------------------------------
  TtaRefreshToolbarStats enable/disable, toggle/untoggle toolbar items widgets for the given doc
 ----------------------------------------------------------------------*/
void TtaRefreshToolbarStats( int changed_action_id )
{
#ifdef _WX
  Document      doc_id;
  View          view;
  FrameToView(TtaGiveActiveFrame(), &doc_id, &view);

  int            window_id = TtaGetDocumentWindowId( doc_id, -1 );
  AmayaWindow *   p_window = TtaGetWindowFromId(window_id);
  wxASSERT(p_window);
  AmayaToolBar * p_toolbar = p_window->GetAmayaToolBar();
  ThotBool   action_enable = FALSE;

  /* do nothing if there is no toolbar : it's the case of AmayaSimpleWindow (log, show apply style ...)*/
  if(!p_toolbar)
    return;

  /* initialize toolbar actions id */
  if ( g_back_action_id == -1 )
    g_back_action_id = FindMenuAction("GotoPreviousHTML");
  if ( g_forward_action_id == -1 )
    g_forward_action_id = FindMenuAction("GotoNextHTML");
  if ( g_reload_action_id == -1 )
    g_reload_action_id = FindMenuAction("Reload");
  if ( g_stop_action_id == -1 )
    g_stop_action_id = FindMenuAction("StopTransfer");
  if ( g_home_action_id == -1 )
    g_home_action_id = FindMenuAction("GoToHome");
  if ( g_save_action_id == -1 )
    g_save_action_id = FindMenuAction("SaveDocument");
  if ( g_print_action_id == -1 )
    g_print_action_id = FindMenuAction("SetupAndPrint");
  if ( g_find_action_id == -1 )
    g_find_action_id = FindMenuAction("TtcSearchText");
  if ( g_logo_action_id == -1 )
    g_logo_action_id = FindMenuAction("HelpLocal");

  /* refresh the specified tool */
  if (changed_action_id == g_back_action_id)
    {
      action_enable = MenuActionList[changed_action_id].ActionActive[doc_id];
      p_toolbar->EnableTool(_T("wxID_TOOL_BACK"), action_enable);
    }
  else if (changed_action_id == g_forward_action_id)
    {
      action_enable = MenuActionList[changed_action_id].ActionActive[doc_id];
      p_toolbar->EnableTool(_T("wxID_TOOL_FORWARD"), action_enable);
    }
  else if (changed_action_id == g_reload_action_id)
    {
      action_enable = MenuActionList[changed_action_id].ActionActive[doc_id];
      p_toolbar->EnableTool(_T("wxID_TOOL_RELOAD"), action_enable);
    }
  else if (changed_action_id == g_stop_action_id)
    {
      action_enable = MenuActionList[changed_action_id].ActionActive[doc_id];
      p_toolbar->EnableTool(_T("wxID_TOOL_STOP"), action_enable);
    }
  else if (changed_action_id == g_home_action_id)
    {
      action_enable = MenuActionList[changed_action_id].ActionActive[doc_id];
      p_toolbar->EnableTool(_T("wxID_TOOL_HOME"), action_enable);
    }
  else if (changed_action_id == g_save_action_id)
    {
      action_enable = MenuActionList[changed_action_id].ActionActive[doc_id];
      p_toolbar->EnableTool(_T("wxID_TOOL_SAVE"), action_enable);
    }
  else if (changed_action_id == g_print_action_id)
    {
      action_enable = MenuActionList[changed_action_id].ActionActive[doc_id];
      p_toolbar->EnableTool(_T("wxID_TOOL_PRINT"), action_enable);
    }
  else if (changed_action_id == g_find_action_id)
    {
      action_enable = MenuActionList[changed_action_id].ActionActive[doc_id];
      p_toolbar->EnableTool(_T("wxID_TOOL_FIND"), action_enable);
    }
  else if (changed_action_id == g_logo_action_id)
    {
      action_enable = MenuActionList[changed_action_id].ActionActive[doc_id];
      p_toolbar->EnableTool(_T("wxID_TOOL_LOGO"), action_enable);
    }
#endif /* _WX */
}


/*----------------------------------------------------------------------
  TtaMakeFrame create a frame (view container)
  notice : a frame need to be attached to a window
  params:
    + Document doc : the document id
    + int schView : the view to attach (schema view)
    + const char * doc_name : 
    + int width, height :
  returns:
    + int * volume : Window volume in characters
    + the frame id
    + 0 if too much created views 
 ----------------------------------------------------------------------*/
int TtaMakeFrame( const char * schema_name,
                  int schView,
		  Document doc_id,
		  const char * doc_name,
		  int width,
		  int height,
		  int * volume,
		  const char * viewName,
		  int window_id, int page_id, int page_position )
{
#ifdef _WX

  /* finding a free frame id */
  int        frame_id = 1;
  ThotBool   found = FALSE;

  while (frame_id <= MAX_FRAME && !found)
    {
      found = (FrameTable[frame_id].FrDoc == 0 &&
	       FrameTable[frame_id].WdFrame != 0);
      if (!found)
        frame_id++;
    }
  if (!found)
    {
      frame_id = 1;
      while (frame_id <= MAX_FRAME && !found)
	{
	  /* finding a free frame id */
	  found = (FrameTable[frame_id].WdFrame == 0);
	  if (!found)
	    frame_id++;
	}
    }

  if (!found)
    return 0; /* too much created frames : bye bye ! */
  
  AmayaFrame * p_AmayaFrame = FrameTable[frame_id].WdFrame;
  if (p_AmayaFrame == NULL)
    {
      /* get the window parent */
      AmayaWindow * p_AmayaWindow = TtaGetWindowFromId(window_id);
      wxASSERT_MSG(p_AmayaWindow, _T("TtaMakeFrame: the window must be created before any frame"));
      
      /* on MacOSX Reparenting is forbidden, so we must give the right parent to the frame at creation */
      AmayaPage * p_page = p_AmayaWindow->GetPage(page_id);
      wxWindow * p_real_parent = NULL;
      if (p_page)
	p_real_parent = p_page->GetSplitterWindow(); /* it's a AmayaNormalWindow */
      else
	p_real_parent = p_AmayaWindow; /* it's a AmayaSimpleWindow */

      /* create the new frame */
      p_AmayaFrame = new AmayaFrame( frame_id, p_real_parent, p_AmayaWindow );
    }
  else
    {
      /* if a frame already exist, be sure to cleanup all its atributs */
      //      DestroyFrame( frame_id );
    }
  
  /* save frame parameters */
  FrameTable[frame_id].WdFrame 	      = p_AmayaFrame;
  FrameTable[frame_id].WdScrollH      = p_AmayaFrame->GetScrollbarH();
  FrameTable[frame_id].WdScrollV      = p_AmayaFrame->GetScrollbarV();
  FrameTable[frame_id].FrWindowId     = window_id; /* this attribut is set when TtaAttachFrame is called */
  FrameTable[frame_id].FrPageId       = page_id; /* this attribut is set when TtaAttachFrame is called */
  FrameTable[frame_id].FrTopMargin    = 0; // TODO
  FrameTable[frame_id].FrScrollOrg    = 0; // TODO
  FrameTable[frame_id].FrScrollWidth  = width; // TODO
  FrameTable[frame_id].FrWidth        = width; //width
  FrameTable[frame_id].FrHeight       = height; // height
  strcpy( FrameTable[frame_id].FrViewName, viewName );
  
  /* Initialise la visibilite et le zoom de la fenetre */
  /* get the old value : 0 if this is the first */
  int zoom, visilibity;
  GetFrameParams (frame_id, &visilibity, &zoom);
  /* get registry default values for visibility 
   * because visibility must be reinitialized for each frame */
  char * visiStr = TtaGetEnvString ("VISIBILITY");
  if (visiStr == NULL)
    visilibity = 5;
  else
    {
      visilibity = atoi(visiStr);
      if (visilibity < 0 || visilibity > 10)
	visilibity = 5;
    }
  InitializeFrameParams (frame_id, visilibity, zoom);

  /* the document title will be used to name the frame's page */
  p_AmayaFrame->SetFrameTitle( TtaConvMessageToWX( doc_name ) );
#ifdef _GL
  FrameTable[frame_id].Scroll_enabled   = TRUE;
#endif /* _GL */

  /* Window volume in characters */
  *volume = GetCharsCapacity (width * height, frame_id);
  FrameTable[frame_id].FrDoc   		= doc_id;
  FrameTable[frame_id].FrView   	= schView;

  return frame_id;
#else
  return 0;
#endif /* #ifdef _WX */
}

/*----------------------------------------------------------------------
  TtaMakePage create an empty page in the window
  params:
    + window_id : the window where the page should be attached
    + page_id : the page index into the window where the page must be inserted
  returns:
    + true if ok
    + false if it's impossible to create this page because another
      page exists at this place or the window is invalid
  ----------------------------------------------------------------------*/
ThotBool TtaMakePage( int window_id, int page_id )
{
#ifdef _WX
  AmayaWindow * p_window = TtaGetWindowFromId(window_id);
  if (p_window == NULL)
    return FALSE;

  if (p_window->GetKind() == WXAMAYAWINDOW_NORMAL)
    {
      AmayaPage * p_page = p_window->GetPage(page_id);
      if (!p_page)
	{
	  /* the page does not exist yet, just create it */
	  p_page = p_window->CreatePage();
	  /* and link it to the window */
	  p_window->AttachPage(page_id, p_page);
	  return TRUE;
	}      
    }
#endif /* _WX */
  return FALSE;
}

/*----------------------------------------------------------------------
  TtaAttachFrame attachs a frame to a window's page
  params:
    + frame_id : the frame
    + window_id : the window where the frame should be attached
    + page_id : the page index into the window where the frame should be attached
  returns:
    + true if ok
    + false if it's impossible to attach the frame to the window
  ----------------------------------------------------------------------*/
ThotBool TtaAttachFrame( int frame_id, int window_id, int page_id, int position )
{
#ifdef _WX
  if (!FrameTable[frame_id].WdFrame)
    return FALSE;
  
  AmayaWindow * p_window = TtaGetWindowFromId(window_id);
  if (p_window == NULL)
    return FALSE;
  
  if (p_window->GetKind() == WXAMAYAWINDOW_NORMAL)
    {
      AmayaPage * p_page = p_window->GetPage(page_id);
      wxASSERT_MSG(p_page, _T("TtaAttachFrame: the page doesn't exists"));
      
      /* now detach the old frame -> unsplit the page */
      //  p_page->DetachFrame( position );
      
      /* now attach the frame to this page */
      AmayaFrame * p_oldframe = NULL;
      p_oldframe = p_page->AttachFrame( FrameTable[frame_id].WdFrame, position );
      
      /* hide the previous frame */
      if (p_oldframe)
	p_oldframe->Hide();
    }
  else if ( p_window->GetKind() == WXAMAYAWINDOW_SIMPLE )
    {
      p_window->AttachFrame( FrameTable[frame_id].WdFrame );
    }

  /* update frame infos */
  FrameTable[frame_id].FrWindowId   	= window_id;
  FrameTable[frame_id].FrPageId         = page_id;
    
  // Popup the frame : bring it to top
  FrameTable[frame_id].WdFrame->RaiseFrame();
  
  /* wait for frame initialisation (needed by opengl) 
   * this function waits for complete widgets initialisation */
  /* notice: no not use safe yield here because it use a wxWindowDisabler and it makes menus blinking */
  wxYield();

  return TRUE;
#else
  return FALSE;
#endif /* #ifdef _WX */
}

/*----------------------------------------------------------------------
  TtaDetachFrame detachs a frame from a window
  params:
    + frame_id : the frame identifier

  returns:
    + true if ok
    + false if it's impossible to detach the frame
  ----------------------------------------------------------------------*/
ThotBool TtaDetachFrame( int frame_id )
{
#ifdef _WX
  int window_id        = FrameTable[frame_id].FrWindowId;
  int page_id          = FrameTable[frame_id].FrPageId;
  AmayaFrame * p_frame = FrameTable[frame_id].WdFrame;
  
  if (window_id < 0 || page_id < 0)
    return FALSE;

  AmayaWindow * p_window = WindowTable[window_id].WdWindow;
  if (p_window == NULL)
    return FALSE;
  
  AmayaFrame * p_detached_frame = NULL;
  if ( p_window->GetKind() == WXAMAYAWINDOW_SIMPLE )
    {
      /* this is a simple window, so detach directly the frame from it */
      p_detached_frame = p_window->DetachFrame();
      wxASSERT( p_detached_frame == p_frame || p_detached_frame == NULL );
    }
  else
  if ( p_window->GetKind() == WXAMAYAWINDOW_NORMAL )
    {
      AmayaPage * p_page = p_window->GetPage(page_id);
      if (!p_page)
	return FALSE;
      
      /* now detach the frame from this page */
      int position = p_page->GetFramePosition( p_frame );
      
      p_detached_frame = p_page->DetachFrame( position );
    }

  if (p_frame == p_detached_frame)
    {
      /* a frame hs been detached so update the FrameTable */
      /* update frame infos */
      FrameTable[frame_id].FrWindowId   = -1;
      FrameTable[frame_id].FrPageId     = -1;
      return TRUE;
    }

  return FALSE;
#else
  return FALSE;
#endif /* #ifdef _WX */
}

/*----------------------------------------------------------------------
  TtaDestroyFrame destroy a frame
  params:
    + frame_id : the frame identifier
  returns:
    + true
    + false
  ----------------------------------------------------------------------*/
ThotBool TtaDestroyFrame( int frame_id )
{
#ifdef _WX
  TTALOGDEBUG_1( TTA_LOG_DIALOG, _T("TtaDestroyFrame: frame_id=%d"), frame_id );

  int          window_id = FrameTable[frame_id].FrWindowId;
  AmayaFrame * p_frame   = FrameTable[frame_id].WdFrame;
  
  if (!p_frame)
    return FALSE;
  
  TtaDetachFrame( frame_id );
  TtaHandlePendingEvents();
  p_frame->FreeFrame();

  return TRUE;
#else /* _WX */
  return FALSE;
#endif /* _WX */
}

/*----------------------------------------------------------------------
  TtaCleanUpWindow check that there is no empty pages
  params:
    + int window_id : the window which contains the pages
  returns:
  ----------------------------------------------------------------------*/
void TtaCleanUpWindow( int window_id )
{
#ifdef _WX
  AmayaWindow * p_window = NULL;
  if (window_id == 0)
    {
      /* check every existing windows */
      window_id = 1;
      while ( window_id < MAX_WINDOW )
	{
	  TtaCleanUpWindow( window_id );
	  window_id++;
	}
    }
  else
    {
      /* check only one window */
      p_window = TtaGetWindowFromId( window_id );
      if (p_window)
	p_window->CleanUp();
    }
#endif /* _WX */
}

/*----------------------------------------------------------------------
  TtaClosePage close a page
  params:
    + int window_id : the window which contains the pages
    + int page_id : the page index (0 is the first one)
  returns:
    + true : page closed
    + false: not closed
  ----------------------------------------------------------------------*/
ThotBool TtaClosePage( int window_id, int page_id )
{
#ifdef _WX 
 AmayaWindow * p_window = TtaGetWindowFromId( window_id );
  if (p_window && page_id >= 0)
    return p_window->ClosePage( page_id );
  else
    return FALSE;
#else
  return FALSE;
#endif /* #ifdef _WX */
}

/*----------------------------------------------------------------------
  TtaGetActiveWindowId returns the last active window id
  params:
  returns:
    + int : last active window_id
  ----------------------------------------------------------------------*/
int TtaGetActiveWindowId()
{
#ifdef _WX 
  return AmayaWindow::GetActiveWindowId();
#else
  return -1;
#endif /* #ifdef _WX */
}

/*----------------------------------------------------------------------
  TtaGetActiveWindow returns the last active window
  params:
  returns:
    + int : last active window_id
  ----------------------------------------------------------------------*/
#ifdef _WX 
AmayaWindow * TtaGetActiveWindow()
{
  return AmayaWindow::GetActiveWindow();
}
#endif /* _WX */

/*----------------------------------------------------------------------
  TtaGetFreePageId returns a free page id for the given window
  params:
    + int window_id : the window which contains the pages
  returns:
    + int : the free page id
  ----------------------------------------------------------------------*/
int TtaGetFreePageId( int window_id )
{
#ifdef _WX
  AmayaWindow * p_window = WindowTable[window_id].WdWindow;
  if (p_window == NULL)
    return -1;

  /* just return the pages count (first page id is 0) */
  return p_window->GetPageCount();
#else
  return -1;
#endif /* #ifdef _WX */
}

/*----------------------------------------------------------------------
  TtaGetFreeWindowId returns a free window id
  params:
  returns:
    + int : the new window id or -1 if too much created window
  ----------------------------------------------------------------------*/
int TtaGetFreeWindowId()
{
#ifdef _WX
  int window_id = 1;
  while ( window_id < MAX_WINDOW )
    {
      if ( WindowTable[window_id].WdWindow == NULL )
	return window_id;
      window_id++;
    }
  return -1;
#else
  return -1;
#endif /* #ifdef _WX */
}

/*----------------------------------------------------------------------
  TtaGetDocumentWindowId returns the current document window id
  params:
    + doc_id : the dcuemnt
    + schView the document schema view
      (if view == -1, just the doc_id is checked )
    (the view is needed because a document could have 2 view into 2 differents windows)
  returns:
    + int : the document window id
    + -1 if nothing is found
  ----------------------------------------------------------------------*/
int TtaGetDocumentWindowId( Document doc_id, int schView )
{
#ifdef _WX
  int        frame_id = 1;
  ThotBool   found = FALSE;
  while (frame_id <= MAX_FRAME && !found)
    {
      found = (FrameTable[frame_id].FrDoc == doc_id &&
	       (schView == -1 || FrameTable[frame_id].FrView == schView));
      if (!found)
        frame_id++;
    }
  if (!found)
    return -1;

  return FrameTable[frame_id].FrWindowId;
#else
  return -1;
#endif /* #ifdef _WX */
}

/*----------------------------------------------------------------------
  TtaGetDocumentPageId returns the current document+view page_id + page_position
  params:
    + doc_id : the docuemnt
    + schView the document schema view
      (if view == -1, just the doc_id is checked )
    (the view is needed because a document could have 2 view into 2 differents pages)
  returns:
    + int page_id : the document's view page id
    + int page_position : the document's view page position
  ----------------------------------------------------------------------*/
void TtaGetDocumentPageId( Document doc_id, int schView,
			   int * page_id,
			   int * page_position )
{
#ifdef _WX
  int        frame_id = 1;
  ThotBool   found = FALSE;
  while (frame_id <= MAX_FRAME && !found)
    {
      found = (FrameTable[frame_id].FrDoc == doc_id &&
	       (schView == -1 || FrameTable[frame_id].FrView == schView));
      if (!found)
        frame_id++;
    }
  if (!found)
    {
      *page_id = -1;
      *page_position = 0;
      return;
    }

  *page_id = FrameTable[frame_id].FrPageId;
  *page_position = 0;

  if (FrameTable[frame_id].FrWindowId <= 0)
    return;
  AmayaWindow * p_window = WindowTable[FrameTable[frame_id].FrWindowId].WdWindow;
  if (p_window == NULL)
    return;
  AmayaPage * p_page = p_window->GetPage( *page_id );
  if (!p_page)
    return;
  *page_position = p_page->GetFramePosition( FrameTable[frame_id].WdFrame );
#endif /* #ifdef _WX */
}

/*----------------------------------------------------------------------
  TtaGetFrameDocumentId returns the correspondig document id for the given frame id
  params:
    + frame_id : the frameid to lookfor corresponding document id
  returns:
    + int : the document id correspondig to the frame
    + -1 if nothing is found
  ----------------------------------------------------------------------*/
int TtaGetFrameDocumentId( int frame_id )
{
#ifdef _WX
  if (frame_id <= 0 || frame_id >= MAX_FRAME )
    return -1;
  return FrameTable[frame_id].FrDoc;
#else
  return -1;
#endif /* #ifdef _WX */
}

/*----------------------------------------------------------------------
  TtaGetFrameWindowParentId returns the correspondig parent window id for the given frame id
  params:
    + frame_id : the frameid to lookfor
  returns:
    + int : the parent window id correspondig to the frame
    + -1 if nothing is found
  ----------------------------------------------------------------------*/
int TtaGetFrameWindowParentId( int frame_id )
{
#ifdef _WX
  if (frame_id <= 0 || frame_id >= MAX_FRAME )
    return -1;
  return FrameTable[frame_id].FrWindowId;
#else
  return -1;
#endif /* #ifdef _WX */
}

/*----------------------------------------------------------------------
  TtaGetWindowFromId returns a window from its id
  params:
  returns:
  ----------------------------------------------------------------------*/
#ifdef _WX
AmayaWindow * TtaGetWindowFromId( int window_id )
{
  if (window_id <= 0 || window_id >= MAX_WINDOW )
    return NULL;
  return WindowTable[window_id].WdWindow;
}
#endif /* #ifdef _WX */

/*----------------------------------------------------------------------
  TtaGetFrameFromId returns a frame from its id
  params:
  returns:
  ----------------------------------------------------------------------*/
#ifdef _WX
AmayaFrame * TtaGetFrameFromId( int frame_id )
{
  if (frame_id <= 0 || frame_id >= MAX_FRAME )
    return NULL;
  return FrameTable[frame_id].WdFrame;
}
#endif /* #ifdef _WX */

/*----------------------------------------------------------------------
  TtaGetFrameId returns the frame id corresponding to a sepcific position
  params:
    - window_id : in which window
    - page_id : in which page 
    - position : 1 or 2
  returns:
    - int : the frame id (0 if not found)
  ----------------------------------------------------------------------*/
int TtaGetFrameId( int window_id, int page_id, int position )
{
#ifdef _WX  
  AmayaWindow * p_window = TtaGetWindowFromId( window_id );
  if (!p_window)
    return 0;
  AmayaPage * p_page = p_window->GetPage( page_id );
  if (!p_page)
    return 0;
  AmayaFrame * p_frame = p_page->GetFrame( position );
  if (!p_frame)
    return 0;
  return p_frame->GetFrameId();
#else /* _WX */
  return 0;
#endif /* _WX */
}

/*----------------------------------------------------------------------
  TtaCloseWindow generate a close event and forward it to the aimed window
  params:
  returns:
  ----------------------------------------------------------------------*/
void TtaCloseWindow( int window_id )
{
#ifdef _WX
  AmayaWindow * p_window = TtaGetWindowFromId(window_id);
  if (p_window)
    p_window->Close();
#endif /* #ifdef _WX */
}

/*----------------------------------------------------------------------
  TtaGetWindowFromId returns total number of open windows
  params:
  returns:
  ----------------------------------------------------------------------*/
int TtaGetWindowNumber( )
{
#ifdef _WX
  int window_id = 1;
  int nb_window = 0;
  while ( window_id < MAX_WINDOW )
    {
      if (TtaGetWindowFromId(window_id))
	nb_window++;
      window_id++;
    }
  return nb_window;
#else /* #ifdef _WX */
  return 1;
#endif /* #ifdef _WX */
}

/*----------------------------------------------------------------------
  TtaMakePanel create a panel (container)
  notice : a panel needs to be attached to a window
  returns:
 	+ the panel id
        + -1 if too much created panels
  ----------------------------------------------------------------------*/
int TtaMakePanel( const char * panel_title )
{
#ifdef _WX
  return -1;
#else
  return -1;
#endif /* #ifdef _WX */
}

/*----------------------------------------------------------------------
  TtaAttachPanel attachs a panel to a window
  notice : a panel needs to be attached to a window
  returns:
 	+ the panel id
        + -1 if too much created panels
  ----------------------------------------------------------------------*/
int TtaAttachPanel( int window_id )
{
#ifdef _WX
  return -1;
#else
  return -1;
#endif /* #ifdef _WX */
}

/*----------------------------------------------------------------------
  TtaDetachPanel detachs a panel from a window
  params:
    + panel_id : the panel identifier
    + window_id : windows identifier (if -1, the panel is
                  searched into the windows table)
  returns:
    + true if ok
    + false if it's impossible to attach the frame to the window
  ----------------------------------------------------------------------*/
ThotBool TtaDetachPanel( int panel_id, int window_id )
{
#ifdef _WX
  return FALSE;
#else
  return FALSE;
#endif /* #ifdef _WX */
}

/*----------------------------------------------------------------------
  TtaFrameIsActive check if the frame is active or not
  active = the frame has the focus
  params:
    + frame_id : frame identifier
  returns:
    + true if active
  ----------------------------------------------------------------------*/
ThotBool TtaFrameIsActive( int frame_id )
{
#ifdef _WX
  return TRUE;
#else
  return FALSE;
#endif /* #ifdef _WX */
}

/*----------------------------------------------------------------------
  TtaFrameIsClosed check if the frame is closed or not
  closed = not current document associated
  params:
    + frame_id : frame identifier
  returns:
    + true if closed
  ----------------------------------------------------------------------*/
ThotBool TtaFrameIsClosed( int frame_id )
{
#ifdef _WX
  return FrameTable[frame_id].FrDoc <= 0;
#else
  return TRUE;
#endif /* #ifdef _WX */
}

/*----------------------------------------------------------------------
  TtaSetURLBar setup the urlbar with a given list of urls (destroy existing urls)
  params:
    + frame_id : frame identifier
    + listUrl : the url list
    + procedure : the callback to activate when a url is selected
  returns:
  ----------------------------------------------------------------------*/
void TtaSetURLBar( int frame_id,
		   const char * listUrl,
		   void (*      procedure)() )
{
#ifdef _WX
  if (!FrameTable[frame_id].WdFrame || FrameTable[frame_id].FrWindowId == -1)
    return;
  AmayaWindow * p_window = WindowTable[FrameTable[frame_id].FrWindowId].WdWindow;
  if ( !p_window )
    return;

  /* setup the callback to activate when a url is selected */
  if (procedure)
    {
      int doc_id = FrameTable[frame_id].FrDoc;
      PtrDocument pDoc = LoadedDocument[doc_id-1];
      pDoc->Call_Text = procedure;
    }
    
  /* First of all empty the url bar */
  p_window->EmptyURLBar();

  /* Append URL from url list to the urlbar */
  const char *ptr, *ptr1;
  wxString urltoappend;
  ptr = listUrl;
  /* function will stop on double EOS */
  if (listUrl)
    {
      while (*ptr != EOS)
	{
	  ptr1 = ptr;
	  while (*ptr1 != EOS)
	      ptr1++;
	  urltoappend = TtaConvMessageToWX( ptr );
	  p_window->AppendURL( urltoappend );
	  ptr = ptr1 + 1;
	}
    }

  /* the first url in the list is the used one for the current frame */
  wxString firsturl = TtaConvMessageToWX( listUrl );

  /* setup the internal frame variable used to remember the frame's url string
   * this string is temporary and is updated each times the user modify the string.
   * when the user switch between frames, the window urlbar is updated with this string */
  FrameTable[frame_id].WdFrame->SetFrameURL( firsturl );
#endif /* #ifdef _WX */
}

/*----------------------------------------------------------------------
  APP_Callback_URLActivate - Callback to set url in box when Enter key pressed
  params:
    + frame_id : frame identifier
    + text : the new url text (UTF-8 encoding)
  returns:
  ----------------------------------------------------------------------*/
void APP_Callback_URLActivate (int frame_id, const char *text)
{
#if 0
#ifdef _WX
  Document            doc;
  View                view;

  CloseTextInsertion ();
  if (text && strlen(text) > 0)
    {
      FrameToView (frame_id, &doc, &view);
      PtrDocument pDoc = LoadedDocument[doc-1];
      if (pDoc->Call_Text)
	(*(Proc3)pDoc->Call_Text) ((void *)doc, (void *)view, (void *)text);
    }
#endif /* _WX */
#endif /* 0 */
}

/*----------------------------------------------------------------------
  APP_Callback_ToolBarButtonActivate - this callback is activated when a toolbar button has been pressed
  params:
    + frame_id : frame identifier (current active frame)
    + button_id : the button position
  returns:
  ----------------------------------------------------------------------*/
void APP_Callback_ToolBarButtonActivate (int frame_id, int button_id)
{
#if 0
#ifdef _WX
  Document            document;
  View                view;

  if ( button_id < MAX_BUTTON &&
       button_id > 0 )
    {
      int doc_id = TtaGetFrameDocumentId( frame_id );
      PtrDocument pDoc = LoadedDocument[doc_id-1];

      if ( !pDoc->EnabledButton[button_id] )
	  return; /* the button is not active */

      // get the parent window
      int window_id = FrameTable[frame_id].FrWindowId;
      if ( window_id < 0 )
	return; /* there is no parents */

      CloseTextInsertion ();
      FrameToView (frame_id, &document, &view);
      TtaSetButtonActivatedStatus (TRUE);
      if (WindowTable[window_id].Call_Button[button_id])
	(*(Proc2)WindowTable[window_id].Call_Button[button_id]) ((void *)document, (void *)view);
      TtaSetButtonActivatedStatus (FALSE);

      /* then give focus to canvas */
      TtaRedirectFocus();
    }
#endif /* _WX */
#endif /* 0 */
}

#if 0
/*----------------------------------------------------------------------
  TtaAddToolBarButton - 
  add a toolbar button to a window
  params:
    + window_id : window identifier
  returns:
    + int button_id : the button identifier used to change its state or bitmap
                      0 is an invalide value
  ----------------------------------------------------------------------*/
int TtaAddToolBarButton( int window_id,
			 ThotIcon picture,
			 char * tooltip,
			 char * functionName,
			 void (*procedure) (),
			 ThotBool status )
{
#ifdef _WX
  AmayaWindow * p_window = TtaGetWindowFromId(window_id);
  wxASSERT( p_window );
  if ( !p_window || p_window->GetKind() == WXAMAYAWINDOW_SIMPLE )
    return 0;

  /* get the window's toolbar */
  AmayaToolBar * p_toolbar = p_window->GetAmayaToolBar();
  if ( !p_toolbar )
    return 0;

  // Setup callback into the window callback list
  int button_id = 1;
  while ( button_id < MAX_BUTTON && WindowTable[window_id].Call_Button[button_id])
    button_id++;
  if ( button_id >= MAX_BUTTON )
    {
      wxASSERT_MSG(FALSE, _T("Too much toolbar buttons !"));
      return 0;
    }

  // Init existing window's frames default values
  int        frame_id = 1;
  while (frame_id <= MAX_FRAME)
    {
      if ( FrameTable[frame_id].FrWindowId == window_id )
	{
	  int doc_id = TtaGetFrameDocumentId( frame_id );
	  PtrDocument pDoc = LoadedDocument[doc_id-1];
	  pDoc->EnabledButton[button_id] = status;
	  /*FrameTable[frame_id].CheckedButton[button_id] = ??? ;*/
	}
      frame_id++;
    }
  
  // Add a new tool to the toolbar
  if ( picture )
    {
      wxBitmapButton * p_button = new wxBitmapButton( p_toolbar
						      ,button_id /* the id used to identify the button when clicked */
						      ,*picture
						      ,wxDefaultPosition
						      ,wxDefaultSize
						      /* with CVSHEAD, wxNO_BORDER style is not correctly rendered, the bitmap is cropped */
						      /* when setting wxBU_AUTODRAW flag on windows, wxNO_BORDER flag is ignore ... */
							  /* but when setting only wxNO_BORDER button enable/disable states are not correctly displayed on window */
						      ,wxBU_AUTODRAW | wxNO_BORDER );
      p_button->SetToolTip( TtaConvMessageToWX( tooltip ) );
      p_toolbar->AddTool( p_button );
      WindowTable[window_id].Button[button_id]               = p_button;
      WindowTable[window_id].Button[button_id]->Enable( status );
      WindowTable[window_id].Call_Button[button_id]          = procedure;
      p_window->Layout();
    }
  else
    {
      // no picture ? it's a separator
      p_toolbar->AddSeparator();
      WindowTable[window_id].Button[button_id] = NULL;
    }

  return button_id;
#else /* _WX */
  return 0;
#endif /* _WX */
  return 0;
}
#endif /* 0 */

/*----------------------------------------------------------------------
  TtaRefreshPanelButton - 
  refresh the button widgets of the frame's panel
  params:
    + panel_type : the panel type
  returns:
  ----------------------------------------------------------------------*/
void TtaRefreshPanelButton( Document doc, View view, int panel_type )
{
#ifdef _WX
  int frame_id = -1;
  if (doc == 0 && view == 0)
    TtaError (ERR_invalid_parameter);
  else
    {
      frame_id = GetWindowNumber (doc, view);
      if (frame_id <= 0 || frame_id > MAX_FRAME)
	TtaError (ERR_invalid_parameter);
      else if (FrameTable[frame_id].WdFrame != 0 && FrameTable[frame_id].WdFrame->IsActive())
	{
	  /* get the frame's window parent */
	  AmayaWindow * p_window = TtaGetWindowFromId( FrameTable[frame_id].FrWindowId );
	  wxASSERT( p_window );
	  if ( !p_window )
	    return;
	  /* get the window's panel */
	  AmayaPanel * p_panel = p_window->GetAmayaPanel();
      /* it is possible to have no panel, for example with AmayaSimpleWindow (ShowAppliedStyle) */
	  if ( !p_panel )
	    return;

	  /* get the subpanel depending on panel_type */
	  AmayaSubPanel * p_subpanel = NULL;
	  bool * p_checked_array     = NULL;
	  switch (panel_type)
	    {
	    case WXAMAYA_PANEL_XHTML:
	      p_subpanel      = p_panel->GetXHTMLPanel();
	      p_checked_array = FrameTable[frame_id].CheckedButton_Panel_XHTML;
	      break;
	    }
	  wxASSERT( p_subpanel );
	  if (!p_subpanel)
	    return;

	  /* refresh the subpanel with button stats */
	  AmayaParams p;
	  p.param1 = (void*)p_checked_array;
	  AmayaSubPanelManager::GetInstance()->SendDataToPanel( p_subpanel->GetPanelType(), p );
	}
    }
#endif /* _WX */
}

/*----------------------------------------------------------------------
  TtaSwitchPanelButton - 
  switch on/off a button in a given panel
  params:
    + panel_type : the panel type
  returns:
  ----------------------------------------------------------------------*/
void TtaSwitchPanelButton( Document doc, View view,
			   int panel_type,
			   int button_id,
			   ThotBool value )
{
#ifdef _WX
  int frame_id = -1;
  if (doc == 0 && view == 0)
    TtaError (ERR_invalid_parameter);
  else
    {
      frame_id = GetWindowNumber (doc, view);
      if (frame_id <= 0 || frame_id > MAX_FRAME)
	TtaError (ERR_invalid_parameter);
      else if (FrameTable[frame_id].WdFrame != 0)
	{
	  bool * p_enable_array  = NULL;
	  bool * p_checked_array = NULL;

	  switch (panel_type)
	    {
	    case WXAMAYA_PANEL_XHTML:
	      p_enable_array  = FrameTable[frame_id].EnabledButton_Panel_XHTML;
	      p_checked_array = FrameTable[frame_id].CheckedButton_Panel_XHTML;
	      break;
	    }

	  /* switch the button */
	  if (p_checked_array)
	    {
	      bool status = p_checked_array[button_id];
	      p_checked_array[button_id] = value;
	      TtaRefreshPanelButton( doc, view, panel_type );
	    }
	}
    }
#endif /* _WX */
}


#if 0
/*----------------------------------------------------------------------
  TtaSetupPanel - 
  init the panel for the given window
  params:
    + type : the panel type
    + window_id : window identifier
  returns:
  ----------------------------------------------------------------------*/
void TtaSetupPanel( int panel_type,
		    int button_id,
		    const char * tooltip,
		    void (*procedure) () )
{
#ifdef _WX
  wxASSERT( button_id >= 0 && button_id < MAX_BUTTON );
  // register the callback & tooltips
  PanelTable[WXAMAYA_PANEL_XHTML].Tooltip_Panel[button_id] = tooltip;
  PanelTable[WXAMAYA_PANEL_XHTML].Call_Panel[button_id]    = procedure;
#endif /* _WX */
}
#endif /* 0 */

#if 0
/*----------------------------------------------------------------------
  TtaRefreshPanelTooltips
  this function refresh the window's panel tooltips because the tooltips
  tables is filled after windows (and panel) creation
  params:
    + window_id : the window to refresh (1 window = 1 panel)
  returns:
  ----------------------------------------------------------------------*/
void TtaRefreshPanelTooltips( int window_id )
{
#ifdef _WX
  /* get window */
  AmayaWindow * p_window = TtaGetWindowFromId(window_id);
  wxASSERT( p_window );
  if ( !p_window )
    return;
  
  /* get panel's window */
  AmayaPanel * p_panel = p_window->GetAmayaPanel();
  wxASSERT( p_panel );
  if ( !p_panel )
    return;  
  
  /* refresh panel tooltips */
  p_panel->RefreshToolTips();
#endif /* _WX */
}
#endif /* 0 */

#if 0
/*----------------------------------------------------------------------
  APP_Callback_PanelButtonActivate - this callback is activated when a tool has been pressed
  params:
    + frame_id : frame identifier (current active frame)
    + button_id : the button position
  returns:
  ----------------------------------------------------------------------*/
void APP_Callback_PanelButtonActivate (int type, int frame_id, int button_id)
{
#ifdef _WX
  Document            document;
  View                view;

  if ( button_id < MAX_BUTTON &&
       button_id >= 0 )
    {
      CloseTextInsertion ();
      FrameToView (frame_id, &document, &view);
      TtaSetButtonActivatedStatus (TRUE);
      Proc2 p_callback = (Proc2)PanelTable[WXAMAYA_PANEL_XHTML].Call_Panel[button_id];
      if (p_callback)
	(*(Proc2)p_callback) ((void *)document, (void *)view);
      TtaSetButtonActivatedStatus (FALSE);

      /* give focus to canvas */
      TtaRedirectFocus();
    }
#endif /* _WX */
}
#endif /* 0 */

/*----------------------------------------------------------------------
  TtaRegisterWidgetWX - 
  this function register a new widget into the corresponding thotlib catalog.
  params:
    + ref : the catalogue reference
    + p_widget : the widget pointer
  returns:
    + true/false
  ----------------------------------------------------------------------*/
ThotBool TtaRegisterWidgetWX( int ref, void * p_widget )
{
#ifdef _WX
  struct Cat_Context *catalogue;

  if (ref == 0)
    {
      TtaError (ERR_invalid_reference);
      return FALSE;
    }
  catalogue = CatEntry (ref);
  if (catalogue == NULL)
    {
      TtaError (ERR_cannot_create_dialogue);
      return FALSE;
    }
   else
     {
       /* this catalogue has already a associated widget ? */
       if (catalogue->Cat_Widget)
	 {
	   /* yes ! destroy the old dialogue */
	   TtaDestroyDialogue (ref);
	 }

       /* now the catalogue is free , register the widget */
       catalogue->Cat_Widget       = (ThotWindow)p_widget;
       catalogue->Cat_ParentWidget = NULL;
       catalogue->Cat_Ref          = ref;
       catalogue->Cat_Type         = CAT_DIALOG;
     }
  return TRUE;
#else /* _WX */
  return FALSE;
#endif /* _WX */
}

#ifdef _WX
/*----------------------------------------------------------------------
  TtaGetContextMenu - 
  this function returns the contextual menu of the given window
  params:
    + window_id : the parent window of the active frame
  returns:
    + wxMenu * : a pointer on a wxMenu, call PopupMenu to show it.
  ----------------------------------------------------------------------*/
wxMenu * TtaGetContextMenu( int window_id )
{
  int menu_id = WindowTable[window_id].MenuContext;

  if (menu_id)
    return WindowTable[window_id].WdMenus[menu_id];
  else
    return NULL;
}
#endif /* _WX */

#if 0
#ifdef _WX
/*----------------------------------------------------------------------
  TtaRefreshMenuStats - 
  this function should be called to synchronize the menu states (enable/disable)
  with FrameTable.EnabledMenus array.
  params:
    + p_menu_bar : the menu bar to synchronize
  returns:
  ----------------------------------------------------------------------*/
void TtaRefreshMenuStats( wxMenuBar * p_menu_bar )
{
  wxASSERT_MSG(FALSE, _T("TtaRefreshMenuStats: to remove"));
  // find the frame owner
  int        frame_id = 1;
  ThotBool   found = FALSE;
  while (frame_id <= MAX_FRAME && !found)
    {
      found = (FrameTable[frame_id].WdFrame && FrameTable[frame_id].WdFrame->GetMenuBar() == p_menu_bar);
      if (!found)
        frame_id++;
    }
  if (!found)
    {
      wxASSERT_MSG(FALSE, _T("Trying to refresh an orphan menubar"));
      return;
    }

  int doc_id       = TtaGetFrameDocumentId( frame_id );
  PtrDocument pDoc = LoadedDocument[doc_id-1];
  int window_id    = TtaGetFrameWindowParentId(frame_id);

  // the frame owner has been found, update it !
  int menu_id = 0;
  int top_menu_id = 0;
  wxMenu * p_menu = NULL;
  while ( menu_id < MAX_MENU )
    {
      p_menu = WindowTable[window_id].WdMenus[menu_id];
      if (p_menu)
	{
	  // find the corrsponding menu position in the Top Menubar
	  top_menu_id = p_menu_bar->FindMenu(p_menu->GetTitle());
	  // we must check that the menu has been found because the contextual menu do not have a title
	  if (top_menu_id >= 0)
	    {
	      // it has been found, update it
	      p_menu_bar->EnableTop(top_menu_id, pDoc->EnabledMenus[menu_id]);
	    }
	}
      menu_id++;
    }
}
#endif /* _WX */
#endif /* 0 */

/*----------------------------------------------------------------------
  TtaToggleOnOffSidePanel
  execute the open/close panel action
  this methode is directly connected to a menu item action
  params:
  returns:
  ----------------------------------------------------------------------*/
void TtaToggleOnOffSidePanel( int frame_id )
{
#ifdef _WX
  /* get the parent window */
  AmayaWindow * p_window = TtaGetWindowFromId(TtaGetFrameWindowParentId(frame_id));
  if (!p_window)
    {
      wxASSERT(false);
      return;
    }

  /* close or open the panel depending on panel state */
  if (p_window->IsPanelOpened())
    p_window->ClosePanel();
  else
    p_window->OpenPanel();
#endif /* _WX */
}

/*----------------------------------------------------------------------
  TtaToggleOnOffSplitView
  execute the split/unsplit action
  this methode is directly connected to a menu item action
  params:
  returns:
  ----------------------------------------------------------------------*/
void TtaToggleOnOffSplitView( int frame_id )
{
#ifdef _WX
  AmayaFrame * p_frame = FrameTable[frame_id].WdFrame;
  if (!p_frame)
    {
      wxASSERT(false);
      return;
    }

  AmayaPage * p_page = p_frame->GetPageParent();
  if (!p_page)
    {
      wxASSERT(false);
      return;
    }

  // simulate a split action
  p_page->DoSplitUnsplit();
#endif /* _WX */
}

/*----------------------------------------------------------------------
  TtaDoPostFrameCreation
  Misc actions to do after the frame creation
  - wait for opengl initialisation
  - give focus to the created frame
  - refresh specific menu items : "Show panel" toggle
  params:
  returns:
  ----------------------------------------------------------------------*/
void TtaDoPostFrameCreation( int frame_id )
{
#ifdef _WX
  /* wait for frame initialisation (needed by opengl) */
  TtaHandlePendingEvents();
  /* wait for frame initialisation (needed by opengl) 
   * this function waits for complete widgets initialisation */
  wxSafeYield();
  /* fix accessibility problem : force the created frame to get the focus */
  TtaRedirectFocus();

  /* get the parent window */
  AmayaWindow * p_window = TtaGetWindowFromId(TtaGetFrameWindowParentId(frame_id));
  if (!p_window)
    {
      wxASSERT(false);
      return;
    }
  /* refresh specific menu item states */
  p_window->RefreshShowPanelToggleMenu();
  p_window->RefreshFullScreenToggleMenu();
#endif /* _WX */
}

/*----------------------------------------------------------------------
  TtaToggleOnOffSplitView
  execute the split/unsplit action
  this methode is directly connected to a menu item action
  params:
  returns:
  ----------------------------------------------------------------------*/
void TtaToggleOnOffFullScreen( int frame_id )
{
#ifdef _WX
  AmayaFrame * p_frame = FrameTable[frame_id].WdFrame;
  if (!p_frame)
    {
      wxASSERT(false);
      return;
    }

  AmayaWindow * p_window = p_frame->GetWindowParent();
  if (!p_window)
    {
      wxASSERT(false);
      return;
    }

  p_window->ToggleFullScreen();
#endif /* _WX */
}

/*----------------------------------------------------------------------
  TtaRegisterOpenURLCallback
  params:
  returns:
  ----------------------------------------------------------------------*/
void TtaRegisterOpenURLCallback( void (*callback) (void *) )
{
#ifdef _WX
  /* register openurl callback in order to call it when twice amaya instance are running */
  ((AmayaApp *)wxTheApp)->RegisterOpenURLCallback( callback );
#endif /* _WX */
}

/*----------------------------------------------------------------------
  TtaSendDataToPanel
  interface to send data to panel manager
  params:
  returns:
  ----------------------------------------------------------------------*/
#ifdef _WX
void TtaSendDataToPanel( int panel_type, AmayaParams& params )
{
  AmayaSubPanelManager::GetInstance()->SendDataToPanel( panel_type, params );
}
#endif /* _WX */

/*----------------------------------------------------------------------
  TtaRedirectFocus
  give focus to active canvas in order to be able to enter text
  params:
  returns:
  ----------------------------------------------------------------------*/
void TtaRedirectFocus()
{
#ifdef _WX
  int active_frame_id = TtaGiveActiveFrame();
  AmayaFrame * p_frame = TtaGetFrameFromId( active_frame_id );
  if (p_frame)
    p_frame->GetCanvas()->SetFocus();
  TTALOGDEBUG_1( TTA_LOG_FOCUS, _T("TtaRedirectFocus activeframe=%d"), active_frame_id );
#endif /* _WX */
}

/*----------------------------------------------------------------------
  TtaHandleUnicodeKey
  this function handle unicode characters
  params:
  returns:
   - true if the charactere has been handled by the frame
   - false if the event must be forwarded to parents (event.Skip())
  ----------------------------------------------------------------------*/
#ifdef _WX
ThotBool TtaHandleUnicodeKey( wxKeyEvent& event )
{
  if ((event.GetUnicodeKey()!=0) && !TtaIsSpecialKey(event.GetKeyCode()) && !event.ControlDown() && !event.AltDown())
    {
      wxWindow *       p_win_focus         = wxWindow::FindFocus();
      wxTextCtrl *     p_text_ctrl         = wxDynamicCast(p_win_focus, wxTextCtrl);
      wxComboBox *     p_combo_box         = wxDynamicCast(p_win_focus, wxComboBox);
      wxSpinCtrl *     p_spinctrl          = wxDynamicCast(p_win_focus, wxSpinCtrl);
      // do not proceed any characteres if the focused widget is a textctrl or a combobox or a spinctrl
      if (!p_text_ctrl && !p_combo_box && !p_spinctrl)
	{
	  wxButton *       p_button            = wxDynamicCast(p_win_focus, wxButton);
	  wxCheckListBox * p_check_listbox     = wxDynamicCast(p_win_focus, wxCheckListBox);
	  // do not proceed "space" key if the focused widget is a button or a wxCheckListBox
	  if ( !(event.GetKeyCode() == WXK_SPACE && (p_button || p_check_listbox)) )
	    {
	      int thot_keysym = event.GetUnicodeKey();  
	      int thotMask = 0;
	      if (event.ControlDown())
		thotMask |= THOT_MOD_CTRL;
	      if (event.AltDown())
		thotMask |= THOT_MOD_ALT;
	      if (event.ShiftDown())
		thotMask |= THOT_MOD_SHIFT; 

	      if ( ThotInput (TtaGiveActiveFrame(), thot_keysym, 0, thotMask, thot_keysym) == 3 )
		/* if a simple caractere has been entred, give focus to canvas
		 * it resolves accesibility problems when the focus is blocked on a panel */
		TtaRedirectFocus();

	      return true;
	    }
	  else
	    event.Skip();	  
	}
      else
	event.Skip();
    }
  return false;
}
#endif /* _WX */


/*----------------------------------------------------------------------
  TtaHandleShortcutKey
  this function handle shortcuts
  params:
  returns:
   - true if the charactere has been handled by the frame
   - false if the event must be forwarded to parents (event.Skip())
  ----------------------------------------------------------------------*/
#ifdef _WX
ThotBool TtaHandleShortcutKey( wxKeyEvent& event )
{
  wxChar thot_keysym = event.GetKeyCode();  
  
  int thotMask = 0;
  if (event.ControlDown())
    thotMask |= THOT_MOD_CTRL;
  if (event.AltDown())
    thotMask |= THOT_MOD_ALT;
  if (event.ShiftDown())
    thotMask |= THOT_MOD_SHIFT;
  
#ifdef _WINDOWS
  /* on windows, +/= key generate '+' key code, but is should generates '=' value */
  if (thot_keysym == '+' && !event.ShiftDown())
    thot_keysym = '=';

  /* do not allow CTRL-C CTRL-X CTRL-V in "text" widgets */
  wxWindow *       p_win_focus         = wxWindow::FindFocus();
  wxTextCtrl *     p_text_ctrl         = wxDynamicCast(p_win_focus, wxTextCtrl);
  wxComboBox *     p_combo_box         = wxDynamicCast(p_win_focus, wxComboBox);
  wxSpinCtrl *     p_spinctrl          = wxDynamicCast(p_win_focus, wxSpinCtrl);
  if (( p_text_ctrl || p_combo_box || p_spinctrl )
	  && (event.ControlDown() && (thot_keysym == 'C' || thot_keysym == 'X' || thot_keysym == 'V')) )
  {
    event.Skip();
    return true;      
  }
#endif /* _WINDOWS */

  // on windows, CTRL+ALT is equivalent to ALTGR key
  if ( ((event.ControlDown() && !event.AltDown()) || (event.AltDown() && !event.ControlDown()))
       && !TtaIsSpecialKey(thot_keysym)
       // this is for the Windows menu shortcuts, 
       // ALT+F => should open File menu
       && !(thot_keysym >= 'A' && thot_keysym <= 'Z' && event.AltDown() && !event.ControlDown())  
       )
    {
      // le code suivant permet de convertire les majuscules
      // en minuscules pour les racourcis clavier specifiques a amaya.
      // OnKeyDown recoit tout le temps des majuscule que Shift soit enfonce ou pas.
      if (!event.ShiftDown())
	{
	  // shift key was not pressed
	  // force the lowercase
	  wxString s(thot_keysym);
	  if (s.IsAscii())
	    {
	      TTALOGDEBUG_1( TTA_LOG_KEYINPUT, _T("TtaHandleShortcutKey : thot_keysym=%x s=")+s, thot_keysym );
	      s.MakeLower();
	      thot_keysym = s.GetChar(0);
	    }
	}
      // Call the generic function for key events management
      ThotInput (TtaGiveActiveFrame(), (int)thot_keysym, 0, thotMask, (int)thot_keysym);
      return true;
    }
  /* it is now the turn of special key shortcuts : CTRL+RIGHT, CTRL+ENTER ...*/
  else if ((event.ControlDown() || event.AltDown()) &&
	   (thot_keysym == WXK_RIGHT ||
	    thot_keysym == WXK_LEFT ||
	    thot_keysym == WXK_RETURN ||
	    thot_keysym == WXK_DOWN ||
	    thot_keysym == WXK_UP ||
	    thot_keysym == WXK_HOME ||
	    thot_keysym == WXK_END))
    {
      TTALOGDEBUG_1( TTA_LOG_KEYINPUT, _T("TtaHandleShortcutKey : special shortcut thot_keysym=%x"), thot_keysym );
      ThotInput (TtaGiveActiveFrame(), thot_keysym, 0, thotMask, thot_keysym);
      return true;
    }
    else if ( thot_keysym == WXK_F2 ||
	      /*	      thot_keysym == WXK_F3 ||*/
	      /*	      thot_keysym == WXK_F4 ||*/
	      thot_keysym == WXK_F5 ||
	      /*	      thot_keysym == WXK_F6 ||*/
	      thot_keysym == WXK_F7 ||
	      /*	      thot_keysym == WXK_F8 ||*/
	      /*	      thot_keysym == WXK_F9 ||*/
	      /*	      thot_keysym == WXK_F10 ||*/
	      thot_keysym == WXK_F11 ||
	      thot_keysym == WXK_F12 )
    {
      ThotInput (TtaGiveActiveFrame(), thot_keysym, 0, thotMask, thot_keysym);
      return true;
      }
  else
    return false;
}
#endif /* _WX */


/*----------------------------------------------------------------------
  TtaHandleSpecialKey
  proceed the special charactere ( F2, TAB ...) if it is one
  params:
  returns:
   - true if the charactere has been handled by the frame
   - false if the event must be forwarded to parents (event.Skip())
  ----------------------------------------------------------------------*/
#ifdef _WX
ThotBool TtaHandleSpecialKey( wxKeyEvent& event )
{
  if ( !event.ControlDown() && !event.AltDown() && TtaIsSpecialKey(event.GetKeyCode()))
    {
      int thot_keysym = event.GetKeyCode();  
      
      bool proceed_key = (
			  thot_keysym == WXK_INSERT ||
			  thot_keysym == WXK_DELETE ||
			  thot_keysym == WXK_HOME   ||
			  thot_keysym == WXK_PRIOR  ||
			  thot_keysym == WXK_NEXT   ||
			  thot_keysym == WXK_END    ||
			  thot_keysym == WXK_LEFT   ||
			  thot_keysym == WXK_RIGHT  ||
			  thot_keysym == WXK_UP     ||
			  thot_keysym == WXK_DOWN   ||
			  thot_keysym == WXK_ESCAPE ||
			  thot_keysym == WXK_BACK   ||
			  thot_keysym == WXK_RETURN ||
			  thot_keysym == WXK_TAB );
      
      wxWindow *       p_win_focus         = wxWindow::FindFocus();
      wxPanel *        p_panel             = wxDynamicCast(p_win_focus, wxPanel);
      wxGLCanvas *     p_gl_canvas         = wxDynamicCast(p_win_focus, wxGLCanvas);
      wxTextCtrl *     p_text_ctrl         = wxDynamicCast(p_win_focus, wxTextCtrl);
      wxComboBox *     p_combo_box         = wxDynamicCast(p_win_focus, wxComboBox);
      wxSpinCtrl *     p_spinctrl          = wxDynamicCast(p_win_focus, wxSpinCtrl);
      wxSplitterWindow * p_splitter        = wxDynamicCast(p_win_focus, wxSplitterWindow);
      wxNotebook *     p_notebook          = wxDynamicCast(p_win_focus, wxNotebook);
      wxScrollBar *    p_scrollbar         = wxDynamicCast(p_win_focus, wxScrollBar);
#if 0
      /* allow other widgets to handel special keys only when the key is not F2 */
      if ((p_combo_box || p_text_ctrl || p_spinctrl) && proceed_key && thot_keysym != WXK_F2)
	{
	  event.Skip();
	  return true;
	}
#endif /* 0 */

      if (p_win_focus)
	TTALOGDEBUG_1( TTA_LOG_FOCUS, _T("focus = %s"), p_win_focus->GetClassInfo()->GetClassName())
      else
	TTALOGDEBUG_0( TTA_LOG_FOCUS, _T("no focus"))

      /* do not allow special key outside the canvas */
      if (!p_gl_canvas && !p_splitter && !p_notebook && !p_scrollbar && proceed_key )
	{
	  event.Skip();
	  return true;      
	}
      
#if 0
	  /* j'ai supprime cette partir du code car qd le notebook a le focus (c'est assez aleatoire...),
	   * tous les caracteres speciaux ne peuvent pas etre entres car il sont captures par le notbook
	   * en commentant cette partie du code, je laisse passer touts les caracteres qd le notebook a le focus. */
#ifdef _WINDOWS
      /* on windows, when the notebook is focused, the RIGHT and LEFT key are forwarded to wxWidgets,
	 we must ignore it */
      if ( p_notebook && proceed_key )
	{
	  event.Skip();
	  return true;
	}
#endif /* _WINDOWS */
#endif /* 0 */

      if ( proceed_key )
	{
	  int thotMask = 0;
	  if (event.ControlDown())
	    thotMask |= THOT_MOD_CTRL;
	  if (event.AltDown())
	    thotMask |= THOT_MOD_ALT;
	  if (event.ShiftDown())
	    thotMask |= THOT_MOD_SHIFT;
	  TTALOGDEBUG_1( TTA_LOG_KEYINPUT, _T("TtaHandleSpecialKey: thot_keysym=%x"), thot_keysym);
	  ThotInput (TtaGiveActiveFrame(), thot_keysym, 0, thotMask, thot_keysym);
	  return true;
	}
      else
	return false;
    }
  else
    return false;
}
#endif /* _WX */


/*----------------------------------------------------------------------
  TtaIsSpecialKey
  this function returns true if this key is special
  params:
  returns:
  ----------------------------------------------------------------------*/
#ifdef _WX
ThotBool TtaIsSpecialKey( int wx_keycode )
{
  return ( wx_keycode == WXK_BACK ||
	   wx_keycode == WXK_TAB  ||
           wx_keycode == WXK_RETURN ||
           wx_keycode == WXK_ESCAPE ||
           /*wx_keycode == WXK_SPACE  ||*/
           wx_keycode == WXK_DELETE ||
	   (wx_keycode >= WXK_START && wx_keycode <= WXK_COMMAND)
	   );
}
#endif /* _WX */

/*----------------------------------------------------------------------
  TtaSendStatsInfo()
  send a quick and small request to a server fo statistic purposes
  ----------------------------------------------------------------------*/
void TtaSendStatsInfo()
{
#ifdef SEND_STATS
#ifdef _WX
  if (Printing)
    return;
  /* default value for SEND_STATS is "Yes" */
  TtaSetEnvBoolean ("SEND_STATS", TRUE, FALSE);
  
  /* we send a request if this is the first time this amaya version is launched (for statistique purpose)
   * ( it just send a simple http request to wam.inrialpes.fr )*/
  ThotBool send_stats;
  TtaGetEnvBoolean ("SEND_STATS", &send_stats);
  char * amaya_version = TtaGetEnvString ("VERSION");
  if ( send_stats ||
       !amaya_version || strcmp(amaya_version, TtaGetAppVersion()) != 0 )
    {
      TTALOGDEBUG_0( TTA_LOG_SOCKET, _T("TtaSendStatsInfo") );
      
	  wxSocketBase::Initialize();

      AmayaStatsThread * pThread = new AmayaStatsThread();
      if ( pThread->Create() != wxTHREAD_NO_ERROR )
	{
	  TTALOGDEBUG_0( TTA_LOG_SOCKET, _T("TtaSendStatsInfo -> Cant't create thread") );      
	}
      if ( pThread->Run() != wxTHREAD_NO_ERROR )
	{
	  TTALOGDEBUG_0( TTA_LOG_SOCKET, _T("TtaSendStatsInfo -> Cant't start thread") );
	}
      
      /* remember the request has been already send to stats server in oder to count
       * only on time each users */
      TtaSetEnvString ("VERSION", (char *)TtaGetAppVersion(), TRUE);
    }
#endif /* _WX */
#endif /* SEND_STATS */
}
