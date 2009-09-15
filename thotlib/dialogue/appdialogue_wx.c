/*
 * Copyright (c) INRIA 1996-2009
 */

/*
 * Handle WX windows and menu bars
 *
 * Authors: I. Vatton (INRIA), S. Gully (INRIA)
 *
 */

#include "wx/wx.h"
#include "wx/bmpbuttn.h"
#include "wx/spinctrl.h"
#include "wx/socket.h"
#include "wx/hashmap.h"

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
#include "glwindowdisplay.h"

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
#include "paneltypes_wx.h"

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
#include "AmayaStatusBar.h"
#include "AmayaPanel.h"
#include "AmayaExplorerPanel.h"
#include "AmayaXHTMLPanel.h"
#include "AmayaStatsThread.h"
#include "AmayaQuickSplitButton.h"
#include "AmayaActionEvent.h"
#include "AmayaHelpWindow.h"
#include "AmayaWindowIterator.h"
#include "selectionapi_f.h"


static int g_logerror_action_id = -1;

static void BuildPopdownWX ( int window_id, Menu_Ctl *ptrmenu, ThotMenu p_menu );
WX_DECLARE_STRING_HASH_MAP( int, wxStringIntMap );
wxStringIntMap g_iconSourceMap;

static int s_enumContextMenuResult;


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
  AmayaWindow * p_window = WindowTable[window_id].WdWindow;
  if (p_window == NULL)
    return;

  p_window->Show( show );
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
  AmayaWindow  *p_window = NULL;
  wxSize        window_size;
  int           window_id = TtaGetFreeWindowId();
  int           display_width_px, display_height_px;
#ifdef _MACOS
  int           min_y = 20;
#else /* _MACOS */
  int           min_y = 0;
#endif /* _MACOS */
 
  if (window_id >= MAX_WINDOW)
    return 0; /* there is no more free windows */

  /* get the parent window pointer */
  AmayaWindow * p_parent_window = TtaGetWindowFromId( parent_window_id );

  /* check if the window is not displayed out of the current screen */
  wxDisplaySize(&display_width_px, &display_height_px);
  if (w <= 0)
    w = 800;
  if (h <= 0)
    h = 600;
  if (w >= display_width_px)
    {
      w = display_width_px;
      x = 0;
    }
  else if (x < 0)
    x = 0;
  if (h >= display_height_px - min_y)
    {
      h = display_height_px - min_y;
      y = min_y;
    }
  else if (y < min_y)
    y = min_y;
    
  if (x + w > display_width_px)
    x = display_width_px - w;
  if (y + h > display_height_px)
    y = display_height_px - h;

  if (w > 0 && h > 0)
    window_size = wxSize(w, h);
  else
    window_size = wxSize(800, 600);
  wxPoint window_pos(x, y);

  /* Create the window */
  switch ( kind )
    {
    case WXAMAYAWINDOW_NORMAL:
    case WXAMAYAWINDOW_ANNOT:
    case WXAMAYAWINDOW_HELP:
    case WXAMAYAWINDOW_CSS:
      p_window = AmayaNormalWindow::CreateNormalWindow( p_parent_window, window_id,
                                        window_pos, window_size, kind );
      
      if (parent_window_id == 0)
        {
          // setup the maximized state (only for normal windows)
          ThotBool maximized;
          TtaGetEnvBoolean("WINDOW_MAXIMIZED", &maximized);
#ifndef _MACOS
          if (maximized)
            p_window->Maximize(maximized);
#endif /* !_MACOS */
        }
      break;
    case WXAMAYAWINDOW_SIMPLE:
      p_window = new AmayaSimpleWindow( p_parent_window, window_id,
                                        window_pos, window_size );
      break;
    }    
  
  if (!p_window)
    return -1; /* no enough memory */

  /* need to show the window now because if it's done later,
     the opengl canvas can't be correctly realized */
  TtaShowWindow( window_id, TRUE );

  return window_id;
}

/*----------------------------------------------------------------------
  BuildRecentDocMenu build the recent document menu
  ----------------------------------------------------------------------*/
static void BuildRecentDocMenu(int window_id, wxMenuItem* item)
{
  if(item)
    {
      AmayaNormalWindow * win = wxDynamicCast(WindowTable[window_id].WdWindow,
                                                      AmayaNormalWindow);
      if(win)
          win->PrepareRecentDocumentMenu(item);
    }
}

/*----------------------------------------------------------------------
  BuildPopdownWX builds menus items and sub-menus and hangs it to the given p_menu
  ----------------------------------------------------------------------*/
static void BuildPopdownWX ( int window_id, Menu_Ctl *ptrmenu, ThotMenu p_menu )
{
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
  wxString     menu_label;

  wxASSERT (p_menu);

#if defined (_MACOS) 
   if (ptrmenu->MenuHelp)
      {
        menu_label = TtaConvMessageToWX(TtaGetMessage (THOT, ptrmenu->MenuID));
        wxApp::s_macHelpMenuTitleName = menu_label;
      }
#endif /* _MACOS */

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
          label +=  TtaConvMessageToWX(item_equiv);
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
          /* Is it the "Paste" command */
          else if (!strcmp (MenuActionList[item_action].ActionName, "PasteBuffer"))
            WindowTable[window_id].MenuItemPaste = item_id;
          /* Is it the "Undo" command */
          else if (!strcmp (MenuActionList[item_action].ActionName, "TtcUndo"))
            WindowTable[window_id].MenuItemUndo = item_id;
          /* Is it the "Redo" command */
          else if (!strcmp (MenuActionList[item_action].ActionName, "TtcRedo"))
            WindowTable[window_id].MenuItemRedo = item_id;
#if defined (_MACOS) 
          /* this is for the MacOSX default lookandfeel */
          else if (!strcmp (MenuActionList[item_action].ActionName, "HelpAmaya"))
            wxApp::s_macAboutMenuItemId = item_id;
          else if (!strcmp (MenuActionList[item_action].ActionName, "ConfigAmaya"))
            wxApp::s_macPreferencesMenuItemId = item_id;
          else if (!strcmp (MenuActionList[item_action].ActionName, "AmayaClose"))
            wxApp::s_macExitMenuItemId = item_id;
#endif /* _MACOS */
          /* Is it the "ShowBrowsingBar" command */
          else if (!strcmp (MenuActionList[item_action].ActionName, "ShowBrowsingBar"))
            WindowTable[window_id].MenuItemShowToolBar[ToolBarBrowsing] = item_id;
          /* Is it the "ShowEditingBar" command */
          else if (!strcmp (MenuActionList[item_action].ActionName, "ShowEditingBar"))
            WindowTable[window_id].MenuItemShowToolBar[ToolBarEditing] = item_id;
        }
      
      if ( p_menu_item &&
           item_icon[0] != EOS && item_action != -1 && item_type != 'T' )
        {
          wxBitmap menu_icon(TtaGetResourcePathWX(WX_RESOURCES_ICON_16X16,item_icon),
                             wxBITMAP_TYPE_PNG);
          if (menu_icon.Ok())
            p_menu_item->SetBitmap( menu_icon );
        }
      
      if (p_menu_item)
        p_menu->Append(p_menu_item);
      
      if (item_action != -1)
        {
          /* Is it the Open recent menu ? */
          if (!strcmp (MenuActionList[item_action].ActionName, "OpenDoc"))
            BuildRecentDocMenu(window_id, p_menu_item);
        }
      
      item_nb++;
    }
}



/*----------------------------------------------------------------------
  TtaMakeWindowMenuBar creates the window menu bar widgets from the
  model (DocumentMenuList)
  ----------------------------------------------------------------------*/
void TtaMakeWindowMenuBar( int window_id )
{
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
          
          /* remember the top menubar widgets because wxMenu doesn't have
             ids to identify it */
          WindowTable[window_id].WdMenus[ptrmenu->MenuID] = p_menu;
          
          /* remember specials menus */
          if (ptrmenu->MenuContext) 
            WindowTable[window_id].MenuContext = ptrmenu->MenuID;
          else if (ptrmenu->MenuDocContext) 
            WindowTable[window_id].MenuDocContext = ptrmenu->MenuID;
          else if (ptrmenu->MenuAttr)
            WindowTable[window_id].MenuAttr = ptrmenu->MenuID;
          else if (ptrmenu->MenuSelect) 
            WindowTable[window_id].MenuSelect = ptrmenu->MenuID;
          else if (ptrmenu->MenuHelp)
            { 
               WindowTable[window_id].MenuHelp = ptrmenu->MenuID;
#if defined (_MACOS) 
             // wxApp::s_macHelpMenuTitleName = "Aide";
#endif /* _MACOS */ 
             }
          
          /* Now create the menu's widgets and hangs it to our p_menu */
          BuildPopdownWX( window_id, ptrmenu, p_menu );
          
          /* Le menu contextuel ne doit pas etre accroche a notre bar de menu */
          /* il sera affiche qd le boutton droit de la souris sera active */
          if (!ptrmenu->MenuContext && !ptrmenu->MenuDocContext)
            p_menu_bar->Append( p_menu,
                                TtaConvMessageToWX( TtaGetMessage (THOT, ptrmenu->MenuID) ) );
          
        }
      ptrmenu = ptrmenu->NextMenu;
    }
  
  p_window->SetMenuBar( p_menu_bar );
}

/*----------------------------------------------------------------------
  TtaInitMenuItemStats enable/disable, toggle/untoggle menu items for
  the given doc
  ----------------------------------------------------------------------*/
void TtaInitMenuItemStats( int doc_id )
{
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
}

/*----------------------------------------------------------------------
  TtaInitTopMenuStats enable/disable, toggle/untoggle top menu for the
  given doc
  ----------------------------------------------------------------------*/
void TtaInitTopMenuStats( int doc_id )
{
  /* enable every menu */
  PtrDocument pDoc = LoadedDocument[doc_id-1];
  if (pDoc && pDoc->EnabledMenus)
    memset(pDoc->EnabledMenus, TRUE, sizeof(pDoc->EnabledMenus));
}

/*----------------------------------------------------------------------
  TtaRefreshMenuStats enable/disable a top menu for the given doc
  or all menus (menu_id = -1)
  ----------------------------------------------------------------------*/
void TtaRefreshTopMenuStats( int doc_id, int menu_id )
{
  int           window_id  = TtaGetDocumentWindowId( doc_id, -1 );
  AmayaWindow * p_window   = TtaGetWindowFromId(window_id);
  wxMenuBar *   p_menu_bar;
  PtrDocument   pDoc       = LoadedDocument[doc_id-1];
  wxMenu *      p_top_menu = NULL;
  int           top_menu_pos = 0, top_menu_count;
  
  /* do nothing if there is no menubar : it's the case of
   * AmayaSimpleWindow (log, show apply style ...)*/
  if (p_window == NULL)
    return;
  else
    p_menu_bar = p_window->GetMenuBar();
  if (!p_menu_bar || doc_id <= 0)
    return;
  
  /* check that the current menu correspond to the current document
   * do nothing if the current document is not the refreshed one */
  Document      active_doc_id;
  View          view;
  FrameToView(TtaGiveActiveFrame(), &active_doc_id, &view);
  if (active_doc_id != doc_id)
    return;
  
  /* refresh only one menu ? */
  top_menu_count = (int)p_menu_bar->GetMenuCount();
  if (menu_id >= 0 && menu_id < MAX_MENU)
    {
      p_top_menu = WindowTable[window_id].WdMenus[menu_id];
      if (p_top_menu)
        {
          // find the corresponding menu position in the Top Menubar
          top_menu_pos = 0;
          while (top_menu_pos < top_menu_count &&
                 p_menu_bar->GetMenu(top_menu_pos) != p_top_menu)
            top_menu_pos++;
          // we must check that the menu has been found because
          // the contextual menu do not have a title
          if (top_menu_pos >= 0 && top_menu_pos < top_menu_count)
            // it has been found, update it
            p_menu_bar->EnableTop(top_menu_pos, (bool)pDoc->EnabledMenus[menu_id]);
          else
            wxASSERT_MSG(FALSE,_T("didn't find the top menu"));
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
          while (top_menu_pos < top_menu_count &&
                 p_menu_bar->GetMenu(top_menu_pos) != p_top_menu)
            top_menu_pos++;
          // we must check that the menu has been found because the
          // contextual menu do not have a title
          if (top_menu_pos >= 0 && top_menu_pos < top_menu_count)
            // it has been found, update it
            p_menu_bar->EnableTop(top_menu_pos, (bool)pDoc->EnabledMenus[menu_id]);
        }
      menu_id++;
    }
}


/*----------------------------------------------------------------------
  TtaRefreshMenuItemStats enable/disable, toggle/untoggle menu items
  widgets for the given doc or all items of all menus (menu_id = -1)
  ----------------------------------------------------------------------*/
void TtaRefreshMenuItemStats( int doc_id, void * context, int menu_item_id )
{
  int           window_id  = TtaGetDocumentWindowId( doc_id, -1 );
  Menu_Ctl     *ptrmenu = (Menu_Ctl *)context;
  AmayaWindow  *p_window   = TtaGetWindowFromId(window_id);
  
  if (!p_window)
    return;
  
  wxMenuBar *   p_menu_bar = p_window->GetMenuBar();
  wxMenuItem *  p_menu_item = NULL;
  Item_Ctl *    ptritem = NULL;
  Menu_Ctl *    item_submenu = NULL;
  int           item_nb = 0;
  int           item_id = menu_item_id;
  char          item_type = ' ';
  const char    *name;
  int           item_action = 0;
  ThotBool      item_enable = FALSE;
  ThotBool      item_toggle = FALSE;
  
  /* do nothing if there is no menubar: it's the case of
     AmayaSimpleWindow (log, show apply style ...)*/
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
        }
      return;
    }
  
  
  /* refresh every entry */
  while (ptrmenu)
    {
      if (!ptrmenu->MenuContext && !ptrmenu->MenuDocContext)
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
              name = MenuActionList[item_action].ActionName;
#ifdef _MACOS
              // sometimes the system desactivates these menu entries
              if (item_enable && name &&
                  (!strcmp (name, "TtcCopySelection") ||
                   !strcmp (name, "TtcCutSelection") ||
                   !strcmp (name, "PasteBuffer")))
                  p_menu_bar->Enable(item_id, false);
#endif /* _MACOS */
              p_menu_bar->Enable(item_id, (bool)item_enable);
              break;
              
            case 'T': /* a toggle menu item (checkbox) */
              item_action  = ptritem[item_nb].ItemAction;
              item_enable  = MenuActionList[item_action].ActionActive[doc_id];
              item_toggle  = MenuActionList[item_action].ActionToggle[doc_id];
              p_menu_bar->Check(item_id, (bool)item_toggle);
              p_menu_bar->Enable(item_id, (bool)item_enable);
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
      }
      ptrmenu = ptrmenu->NextMenu;
    }
}


/*----------------------------------------------------------------------
  TtaToggleLogError enables/disables the logerror button
  ----------------------------------------------------------------------*/
void TtaToggleLogError (Document doc_id, ThotBool enable)
{
  int             window_id = TtaGetDocumentWindowId( doc_id, -1 );
  AmayaWindow    *p_window = TtaGetWindowFromId(window_id);
  AmayaStatusBar *p_sbar = NULL;

  if (p_window)
    p_sbar = p_window->GetStatusBar();
  /* do nothing if there is no sbar: it's the case of
     AmayaSimpleWindow (log, show apply style ...)*/
  if(!p_sbar)
    return;

  /* initialize toolbar actions id */
  if ( g_logerror_action_id == -1 )
    g_logerror_action_id = FindMenuAction("ShowLogFile");

  if (g_logerror_action_id)
    {
      MenuActionList[g_logerror_action_id].ActionActive[doc_id] = enable;
      p_sbar->EnableLogError(enable);
    }
}

/*----------------------------------------------------------------------
  TtaSetLockButton button
  ----------------------------------------------------------------------*/
void TtaSetLockButton (Document doc_id, int status)
{
  int             window_id = TtaGetDocumentWindowId( doc_id, -1 );
  AmayaWindow    *p_window = TtaGetWindowFromId(window_id);
  AmayaStatusBar *p_sbar = NULL;

  if (p_window)
    p_sbar = p_window->GetStatusBar();
  /* do nothing if there is no sbar: it's the case of
     AmayaSimpleWindow (log, show apply style ...)*/
  if(!p_sbar)
    return;
  p_sbar->EnableLock(status);
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
int TtaMakeFrame( const char * schema_name, int schView,
                  Document doc_id, const char * doc_name,
                  int width, int height, int * volume, const char * viewName,
                  int window_id, int page_id, int page_position )
{
  /* finding a free frame id */
  int        frame_id = 1;
  ThotBool   found = FALSE;

  /* Create empty page to embed new frame */
  TtaMakePage(window_id, page_id, doc_id);

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
      
      /* on MacOSX Reparenting is forbidden, so we must give the
         right parent to the frame at creation */
      AmayaSplittablePage * p_page = wxDynamicCast(p_AmayaWindow->GetPage(page_id),
                                                   AmayaSplittablePage);
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
  FrameTable[frame_id].FrWindowId     = window_id; /* TtaAttachFrame sets it */
  FrameTable[frame_id].FrPageId       = page_id; /* TtaAttachFrame sets it */
  FrameTable[frame_id].FrPagePos      = page_position;
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
  if (!strcmp (doc_name, "STYLE.LST"))
    p_AmayaFrame->SetFrameTitle( TtaConvMessageToWX( TtaGetMessage (LIB, TMSG_CSSStyle) ) );
  else
    p_AmayaFrame->SetFrameTitle( TtaConvMessageToWX( doc_name ) );
#ifdef _GL
  FrameTable[frame_id].Scroll_enabled   = TRUE;
#endif /* _GL */

  /* Window volume in characters */
  *volume = GetCharsCapacity (width * height, frame_id);
  FrameTable[frame_id].FrDoc   		= doc_id;
  FrameTable[frame_id].FrView   	= schView;

  return frame_id;
}

/*----------------------------------------------------------------------
  TtaMakePage create an empty page in the window
  params:
  + window_id : the window where the page should be attached
  + page_id : the page index into the window where the page must be inserted
  + doc_id : the document id of the future attached document.
  returns:
  + true if ok
  + false if it's impossible to create this page because another
  page exists at this place or the window is invalid
  ----------------------------------------------------------------------*/
ThotBool TtaMakePage( int window_id, int page_id, Document doc)
{
  int kind;

  AmayaWindow * p_window = TtaGetWindowFromId(window_id);
  if (p_window == NULL)
    return FALSE;
  kind = p_window->GetKind();
  if (kind == WXAMAYAWINDOW_NORMAL ||
      kind == WXAMAYAWINDOW_ANNOT ||
      kind == WXAMAYAWINDOW_HELP ||
      kind == WXAMAYAWINDOW_CSS)
    {
      AmayaPage * p_page = p_window->GetPage(page_id);
      if (!p_page)
        {
          /* the page does not exist yet, just create it */
          p_page = p_window->CreatePage(doc);
          /* and link it to the window */
          p_window->AttachPage(page_id, p_page);
          return TRUE;
        }
    }
  return FALSE;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
int TtaGetIconIndex (const char * filename)
{
  wxString path = TtaConvMessageToWX(filename);
  wxStringIntMap::iterator iter =  g_iconSourceMap.find(path);
  if (iter != g_iconSourceMap.end())
    return iter->second;
  else
    {
      wxImage img(TtaConvMessageToWX(filename), wxBITMAP_TYPE_ANY);
      wxIcon icon;
      if(img.IsOk())
        {
          img.Rescale(16, 16, wxIMAGE_QUALITY_HIGH);
          icon.CopyFromBitmap(wxBitmap(img));
          int index = ((AmayaApp *)wxTheApp)->GetDocumentIconList()->Add(icon);
          g_iconSourceMap[path] = index;
          return  index;
        }
    }
  return 0;
}

/*----------------------------------------------------------------------
  TtaSetPageIcon setup the page icon
    returns:
  ----------------------------------------------------------------------*/
void TtaSetPageIcon( Document doc, View view, char *iconpath)
{
  int            frame;
  AmayaWindow   *p_AmayaWindow;

  wxIcon icon;

  UserErrorCode = 0;
  /* verifie le parametre document */
  if (doc == 0 && view == 0)
    TtaError (ERR_invalid_parameter);
  frame = GetWindowNumber (doc, view);
  if (iconpath)
    {
      p_AmayaWindow = TtaGetWindowFromId( FrameTable[frame].FrWindowId );
      if (p_AmayaWindow)
        p_AmayaWindow->SetPageIcon (FrameTable[frame].FrPageId, iconpath);
    }
  else
    {
      // display the default icon
      AmayaFrame * p_frame = FrameTable[frame].WdFrame;
      if (p_frame)
        p_frame->UpdateFrameIcon();
    }
}

/*----------------------------------------------------------------------
  TtaAttachFrame attachs a frame to a window's page
  params:
  + frame_id : the frame
  + window_id : the window where the frame should be attached
  + page_id : the page index into the window where the frame should be attached
  + split: 0 for horizontal, 1 if for vertical
  returns:
  + true if ok
  + false if it's impossible to attach the frame to the window
  ----------------------------------------------------------------------*/
ThotBool TtaAttachFrame( int frame_id, int window_id, int page_id, int position,
                         int split)
{
  int kind;

  if (!FrameTable[frame_id].WdFrame)
    return FALSE;
  
  AmayaWindow * p_window = TtaGetWindowFromId(window_id);
  if (p_window == NULL)
    return FALSE;
  
  kind = p_window->GetKind();
  if (kind == WXAMAYAWINDOW_NORMAL ||
      kind == WXAMAYAWINDOW_ANNOT ||
      kind == WXAMAYAWINDOW_HELP ||
      kind == WXAMAYAWINDOW_CSS)
    {
      AmayaPage * p_page = p_window->GetPage(page_id);
      wxASSERT_MSG(p_page, _T("TtaAttachFrame: the page doesn't exists"));
      if (p_page == NULL)
        return FALSE;
      /* now detach the old frame -> unsplit the page */
      //  p_page->DetachFrame( position );
      
      /* now attach the frame to this page */
      AmayaFrame * p_oldframe = NULL;
      p_oldframe = p_page->AttachFrame( FrameTable[frame_id].WdFrame, position, split );
    }
  else if ( p_window->GetKind() == WXAMAYAWINDOW_SIMPLE )
    {
      p_window->AttachFrame( FrameTable[frame_id].WdFrame );
    }

  /* update frame infos */
  FrameTable[frame_id].FrWindowId   	  = window_id;
  FrameTable[frame_id].FrPageId         = page_id;
  FrameTable[frame_id].FrPagePos        = position;
    
  // Popup the frame : bring it to top
  FrameTable[frame_id].WdFrame->RaiseFrame();
  
  /* wait for frame initialisation (needed by opengl) 
   * this function waits for complete widgets initialisation */
  /* notice: no not use safe yield here because it use a
     wxWindowDisabler and it makes menus blinking */
  wxYield();
  return TRUE;
}

/*----------------------------------------------------------------------
  TtaRefreshActiveFrame force a refresh of window widgets
  ----------------------------------------------------------------------*/
void TtaRefreshActiveFrame ()
{
  if (FrameTable[ActiveFrame].WdFrame)
    FrameTable[ActiveFrame].WdFrame->SetActive(TRUE);
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
  int kind;
  int window_id        = FrameTable[frame_id].FrWindowId;
  int page_id          = FrameTable[frame_id].FrPageId;
  int position         = FrameTable[frame_id].FrPagePos;
  AmayaFrame * p_frame = FrameTable[frame_id].WdFrame;
  
  if (window_id < 0 || page_id < 0 || position == 0)
    return FALSE;

  AmayaWindow * p_window = WindowTable[window_id].WdWindow;
  if (p_window == NULL)
    return FALSE;
  
  AmayaFrame * p_detached_frame = NULL;
  kind = p_window->GetKind();
  if (kind == WXAMAYAWINDOW_SIMPLE)
    {
      /* this is a simple window, so detach directly the frame from it */
      p_detached_frame = p_window->DetachFrame();
      wxASSERT( p_detached_frame == p_frame || p_detached_frame == NULL );
    }
  else if (kind == WXAMAYAWINDOW_NORMAL ||
           kind == WXAMAYAWINDOW_ANNOT ||
           kind == WXAMAYAWINDOW_HELP ||
           kind == WXAMAYAWINDOW_CSS)
    {
      AmayaPage * p_page = p_window->GetPage(page_id);
      if (!p_page)
        return FALSE;
      
      /* now detach the frame from this page */      
      p_detached_frame = p_page->DetachFrame( position );
    }

  if (p_frame == p_detached_frame)
    {
      /* a frame has been detached so update the FrameTable */
      /* update frame infos */
      FrameTable[frame_id].FrWindowId   = -1;
      FrameTable[frame_id].FrPageId     = -1;
      FrameTable[frame_id].FrPagePos    = 0;
      return TRUE;
    }

  return FALSE;
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
  TTALOGDEBUG_1( TTA_LOG_DIALOG, _T("TtaDestroyFrame: frame_id=%d"), frame_id );

  AmayaFrame * p_frame   = FrameTable[frame_id].WdFrame;  
  wxASSERT(p_frame);
  if (!p_frame)
    return FALSE;

  p_frame->Show();
  p_frame->FreeFrame();
  
  return TRUE;
}

/*----------------------------------------------------------------------
  TtaCleanUpWindow check that there is no empty pages
  params:
  + int window_id : the window which contains the pages
  returns:
  ----------------------------------------------------------------------*/
void TtaCleanUpWindow( int window_id )
{
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
  AmayaWindow *p_window = TtaGetWindowFromId( window_id );

  if (p_window && page_id >= 0)
    {
/*       if (p_window->GetPageCount() == 1) */
/* 	{ */
/* 	  TtaCloseWindow (window_id); */
/* 	  return TRUE; */
/* 	} */
/*       else */
	return p_window->ClosePage( page_id );
    }
  else
    return FALSE;
}

/*----------------------------------------------------------------------
  TtaCloseAllPageButThis close all pages of a window but one
  params:
  + int window_id : the window which contains the pages
  + int page_id : the page index (0 is the first one)
  returns:
  + true : pages closed
  + false: not closed
  ----------------------------------------------------------------------*/
ThotBool TtaCloseAllPageButThis( int window_id, int page_id )
{
  AmayaWindow * p_window = TtaGetWindowFromId( window_id );
  if (p_window && page_id >= 0)
    return p_window->CloseAllButPage( page_id );
  else
    return FALSE;
}

/*----------------------------------------------------------------------
  TtaGetActiveWindowId returns the last active window id
  params:
  returns:
  + int : last active window_id
  ----------------------------------------------------------------------*/
int TtaGetActiveWindowId()
{
  return AmayaWindow::GetActiveWindowId();
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
  AmayaWindow * p_window = WindowTable[window_id].WdWindow;
  if (p_window == NULL)
    return -1;

  /* just return the pages count (first page id is 0) */
  return p_window->GetPageCount();
}

/*----------------------------------------------------------------------
  TtaGetHelpWindowId returns the help window id
  params:
  returns:
  + int : the new window id or -1 if too much created window
  ----------------------------------------------------------------------*/
int TtaGetHelpWindowId()
{
  int window_id = 1;
  while ( window_id < MAX_WINDOW )
    {
      if ( WindowTable[window_id].WdWindow &&
           WindowTable[window_id].WdWindow->GetKind() == WXAMAYAWINDOW_HELP)
        return window_id;
      window_id++;
    }
  return -1;
}

/*----------------------------------------------------------------------
  TtaGetAnnotWindowId returns the annotation window id
  params:
  returns:
  + int : the new window id or -1 if too much created window
  ----------------------------------------------------------------------*/
int TtaGetAnnotWindowId()
{
  int window_id = 1;
  while ( window_id < MAX_WINDOW )
    {
      if ( WindowTable[window_id].WdWindow &&
           WindowTable[window_id].WdWindow->GetKind() == WXAMAYAWINDOW_ANNOT)
        return window_id;
      window_id++;
    }
  return -1;
}

/*----------------------------------------------------------------------
  TtaGetFreeWindowId returns a free window id
  params:
  returns:
  + int : the new window id or -1 if too much created window
  ----------------------------------------------------------------------*/
int TtaGetFreeWindowId()
{
  int window_id = 1;
  while ( window_id < MAX_WINDOW )
    {
      if ( WindowTable[window_id].WdWindow == NULL )
        return window_id;
      window_id++;
    }
  return -1;
}

/*----------------------------------------------------------------------
  TtaGetDocumentWindowId returns the current document window id
  params:
  + doc_id : the document
  + schView the document schema view
  (if view == -1, just the doc_id is checked )
  (the view is needed because a document could have 2 view into 2 differents windows)
  returns:
  + int : the document window id
  + -1 if nothing is found
  ----------------------------------------------------------------------*/
int TtaGetDocumentWindowId( Document doc_id, int schView )
{
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
}

/*----------------------------------------------------------------------
  TtaUniqueTabInWindow returns TRUE if only one tab in the window
  params:
  + doc_id : the document
  ----------------------------------------------------------------------*/
ThotBool TtaUniqueTabInWindow( Document doc_id )
{
  ThotBool   found = FALSE;
  int        frame_id = 1;
  int        window_id = 0;

  window_id = TtaGetDocumentWindowId( doc_id, -1 );
  while (frame_id <= MAX_FRAME && !found)
    {
      found = (FrameTable[frame_id].FrWindowId == window_id &&
               FrameTable[frame_id].FrDoc != doc_id &&
               FrameTable[frame_id].FrPagePos == 1);
      if (!found)
        frame_id++;
    }
  return !found;
}

/*----------------------------------------------------------------------
  TtaGetDocumentPageId returns the current document+view page_id + page_position
  params:
  + doc_id : the document
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
  *page_position = FrameTable[frame_id].FrPagePos;

  if (FrameTable[frame_id].FrWindowId <= 0)
    return;
  AmayaWindow * p_window = WindowTable[FrameTable[frame_id].FrWindowId].WdWindow;
  if (p_window == NULL)
    return;
  AmayaPage * p_page = p_window->GetPage( *page_id );
  if (!p_page)
    return;
  *page_position = p_page->GetFramePosition( FrameTable[frame_id].WdFrame );
}

/*----------------------------------------------------------------------
  TtaGetFrameDocumentId returns the correspondig document id for the
  given frame id
  params:
  + frame_id : the frameid to lookfor corresponding document id
  returns:
  + int : the document id correspondig to the frame
  + -1 if nothing is found
  ----------------------------------------------------------------------*/
int TtaGetFrameDocumentId( int frame_id )
{
  if (frame_id <= 0 || frame_id >= MAX_FRAME )
    return -1;
  return FrameTable[frame_id].FrDoc;
}

/*----------------------------------------------------------------------
  TtaGetFrameWindowParentId returns the correspondig parent window id
  for the given frame id
  params:
  + frame_id : the frameid to lookfor
  returns:
  + int : the parent window id correspondig to the frame
  + -1 if nothing is found
  ----------------------------------------------------------------------*/
int TtaGetFrameWindowParentId( int frame_id )
{
  if (frame_id <= 0 || frame_id >= MAX_FRAME )
    return -1;
  return FrameTable[frame_id].FrWindowId;
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
#endif /* _WX */

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
#endif /* _WX */

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
}

/*----------------------------------------------------------------------
  TtaCloseWindow generate a close event and forward it to the aimed window
  params:
  returns:
  ----------------------------------------------------------------------*/
void TtaCloseWindow( int window_id )
{
  // handle all pending events before closing the current window
  TtaHandlePendingEvents ();
  AmayaWindow * p_window = TtaGetWindowFromId(window_id);
  if (p_window)
    p_window->Close();
}

/*----------------------------------------------------------------------
  TtaEmptyURLBar cleanup url list in window's combobox
  params:
  returns:
  ----------------------------------------------------------------------*/
void TtaEmptyURLBar( int window_id )
{
  AmayaWindow * p_window = TtaGetWindowFromId(window_id);
  if (p_window)
    p_window->EmptyURLBar();
}

/*----------------------------------------------------------------------
  TtaGetWindowFromId returns total number of open windows
  params:
  returns:
  ----------------------------------------------------------------------*/
int TtaGetWindowNumber( )
{
  int window_id = 1;
  int nb_window = 0;
  while ( window_id < MAX_WINDOW )
    {
      if (TtaGetWindowFromId(window_id))
        nb_window++;
      window_id++;
    }
  return nb_window;
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
  return FrameTable[frame_id].FrDoc <= 0;
}

/*----------------------------------------------------------------------
  TtaSetURLBar setup the urlbar with a given list of urls (destroy existing urls)
  params:
  + frame_id : frame identifier
  + listUrl : the url list
  + procedure : the callback to activate when a url is selected
  returns:
  ----------------------------------------------------------------------*/
void TtaSetURLBar( int frame_id, const char * listUrl, void (* procedure)())
{
  const    char *ptr, *ptr1;
  wxString urltoappend, firsturl;

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
  ptr = listUrl;
  /* function will stop on double EOS */
  if (listUrl)
    {
      while (*ptr != EOS)
        {
          ptr1 = ptr;
          while (*ptr1 != EOS)
            ptr1++;
          if (strcmp (ptr, "empty"))
            {
            //  urltoappend = TtaConvMessageToWX( "" );
            //else
              urltoappend = TtaConvMessageToWX( ptr );
              p_window->AppendURL( urltoappend );
            }
          ptr = ptr1 + 1;
        }
    }

  /* the first url in the list is the used one for the current frame */
  if (listUrl && !strcmp (listUrl, "empty"))
    firsturl = TtaConvMessageToWX( "" );
  else
    firsturl = TtaConvMessageToWX( listUrl );

  /* setup the internal frame variable used to remember the frame's url string
   * this string is temporary and is updated each times the user modify the string.
   * when the user switch between frames, the window urlbar is updated with this string */
  FrameTable[frame_id].WdFrame->SetFrameURL( firsturl );
}

/*----------------------------------------------------------------------
  TtaSwitchPanelButton - 
  switch on/off a button in a given panel
  params:
  + panel_type : the panel type
  returns:
  ----------------------------------------------------------------------*/
void TtaSwitchPanelButton (Document doc, View view, int panel_type,
                           int button_id, ThotBool value)
{
  static int  idaction[WXAMAYA_PANEL_XHTML_SUB+1];
  static bool isinit = false;
  
  if(!isinit)
    {
      idaction[WXAMAYA_PANEL_XHTML_DIV]    = FindMenuAction("CreateDivision");
      idaction[WXAMAYA_PANEL_XHTML_H1]     = FindMenuAction("CreateHeading1");
      idaction[WXAMAYA_PANEL_XHTML_H2]     = FindMenuAction("CreateHeading2");
      idaction[WXAMAYA_PANEL_XHTML_H3]     = FindMenuAction("CreateHeading3");
      idaction[WXAMAYA_PANEL_XHTML_TABLE]  = FindMenuAction("CreateTable");
      idaction[WXAMAYA_PANEL_XHTML_TARGET] = FindMenuAction("CreateTarget");
      idaction[WXAMAYA_PANEL_XHTML_LINK]   = FindMenuAction("CreateOrChangeLink");
      idaction[WXAMAYA_PANEL_XHTML_BULLET] = FindMenuAction("CreateList");
      idaction[WXAMAYA_PANEL_XHTML_NL]     = FindMenuAction("CreateNumberedList");
      idaction[WXAMAYA_PANEL_XHTML_DL]     = FindMenuAction("CreateDefinitionList");
      idaction[WXAMAYA_PANEL_XHTML_DT]     = FindMenuAction("CreateDefinitionTerm");
      idaction[WXAMAYA_PANEL_XHTML_DD]     = FindMenuAction("CreateDefinitionDef");
      idaction[WXAMAYA_PANEL_XHTML_IMG]    = FindMenuAction("CreateImage");
      idaction[WXAMAYA_PANEL_XHTML_OBJ]    = FindMenuAction("CreateObject");
      idaction[WXAMAYA_PANEL_XHTML_STRONG] = FindMenuAction("SetOnOffStrong");
      idaction[WXAMAYA_PANEL_XHTML_EMPH]   = FindMenuAction("SetOnOffEmphasis");
      idaction[WXAMAYA_PANEL_XHTML_CODE]   = FindMenuAction("SetOnOffCode");
      idaction[WXAMAYA_PANEL_XHTML_INS]    = FindMenuAction("SetOnOffINS");
      idaction[WXAMAYA_PANEL_XHTML_DEL]    = FindMenuAction("SetOnOffDEL");
      idaction[WXAMAYA_PANEL_XHTML_SUB]    = FindMenuAction("SetOnOffSub");
      idaction[WXAMAYA_PANEL_XHTML_SUP]    = FindMenuAction("SetOnOffSup");
      isinit = true;
    }
  
  if (doc == 0 && view == 0)
    TtaError (ERR_invalid_parameter);
  else
    {
      
      switch (panel_type)
        {
        case WXAMAYA_PANEL_XHTML:
          switch(button_id)
            {
              case WXAMAYA_PANEL_XHTML_EMPH:
              case WXAMAYA_PANEL_XHTML_STRONG:
              case WXAMAYA_PANEL_XHTML_INS:
              case WXAMAYA_PANEL_XHTML_DEL:
              case WXAMAYA_PANEL_XHTML_CODE:
              case WXAMAYA_PANEL_XHTML_SUB:
              case WXAMAYA_PANEL_XHTML_SUP:
                MenuActionList[idaction[button_id]].ActionToggle[doc] = value;
                break;
            }
          break;
        }
    }
}

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


#ifdef _WX
/*----------------------------------------------------------------------
  TtaGetDocContextMenu - 
  this function returns the contextual doc menu of the given window
  params:
  + window_id : the parent window of the active frame
  returns:
  + wxMenu * : a pointer on a wxMenu, call PopupMenu to show it.
  ----------------------------------------------------------------------*/
wxMenu * TtaGetDocContextMenu( int window_id )
{
  int menu_id = WindowTable[window_id].MenuDocContext;

  if (menu_id)
    return WindowTable[window_id].WdMenus[menu_id];
  else
    return NULL;
}
#endif /* _WX */



/*----------------------------------------------------------------------
  TtaToggleOnOffSidePanel
  execute the open/close panel action
  this methode is directly connected to a menu item action
  params:
  returns:
  ----------------------------------------------------------------------*/
void TtaToggleOnOffSidePanel( int frame_id )
{
  /* get the parent window */
  AmayaWindow * p_window = TtaGetWindowFromId(TtaGetFrameWindowParentId(frame_id));
  if (!p_window)
    {
      wxASSERT(false);
      return;
    }

  /* close or open the panel depending on panel state */
  if (p_window->ToolPanelsShown())
    p_window->HideToolPanels();
  else
    p_window->ShowToolPanels();
}

/*----------------------------------------------------------------------
  TtaToggleOnOffToolBar
  Open or close toolbar.
  \param frame_id Frame identifier
  \param toolbar_id Toolbar identifier:
    - 0 Browsing toolbar
    - 1 Edition toolbar
  ----------------------------------------------------------------------*/
void TtaToggleToolbar( int frame_id, int toolbar_id )
{
  /* get the parent window */
  AmayaWindow * p_window = TtaGetWindowFromId(TtaGetFrameWindowParentId(frame_id));
  if (!p_window)
    {
      wxASSERT(false);
      return;
    }

  /* close or open the panel depending on panel state */
  if (p_window->IsToolBarShown(toolbar_id))
    p_window->HideToolBar(toolbar_id);
  else
    p_window->ShowToolBar(toolbar_id);
}



/*----------------------------------------------------------------------
  TtaSplitViewHorizontally
  this methode is directly connected to a menu item action
  params:
  returns:
  ----------------------------------------------------------------------*/
void TtaSplitViewHorizontally( int frame_id )
{
  AmayaFrame * p_frame = FrameTable[frame_id].WdFrame;
  if (!p_frame)
    {
      wxASSERT(false);
      return;
    }

  AmayaSplittablePage * p_page = wxDynamicCast(p_frame->GetPageParent(),
                                                AmayaSplittablePage);
  if (p_page)
    {
      // simulate a split action
      wxMouseEvent event_mouse( wxEVT_LEFT_DCLICK );
      wxPostEvent( p_page->GetQuickSplitButton ( TRUE ), event_mouse );
      //p_page->DoBottomSplitButtonAction();
    }
}

/*----------------------------------------------------------------------
  TtaSplitViewVertically
  this methode is directly connected to a menu item action
  params:
  returns:
  ----------------------------------------------------------------------*/
void TtaSplitViewVertically( int frame_id )
{
  AmayaFrame * p_frame = FrameTable[frame_id].WdFrame;
  if (!p_frame)
    {
      wxASSERT(false);
      return;
    }

  AmayaSplittablePage * p_page = wxDynamicCast(p_frame->GetPageParent(),
                                                AmayaSplittablePage);
  if (p_page)
    {
      // simulate a split action
      wxMouseEvent event_mouse( wxEVT_LEFT_DCLICK );
      wxPostEvent( p_page->GetQuickSplitButton ( FALSE ), event_mouse );
      //p_page->DoRightSplitButtonAction();
    }
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
  /* wait for frame initialisation (needed by opengl) */
  TtaHandlePendingEvents();
  /* wait for frame initialisation (needed by opengl) 
   * this function waits for complete widgets initialisation */
  wxYield();
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
  p_window->RefreshShowToolPanelToggleMenu();
}

/*----------------------------------------------------------------------
  TtaToggleOnOffFullScreen
  execute the fullscreen action
  this methode is directly connected to a menu item action
  params:
  returns:
  ----------------------------------------------------------------------*/
void TtaToggleOnOffFullScreen( int frame_id )
{
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
}

/*----------------------------------------------------------------------
  TtaGetFullScreenState()
  return the fullscreen state of a frame's window
  params:
  returns:
  ----------------------------------------------------------------------*/
ThotBool TtaGetFullScreenState(int frame_id )
{
  AmayaFrame * p_frame = FrameTable[frame_id].WdFrame;
  if (!p_frame)
    {
      wxASSERT(false);
      return FALSE;
    }

  AmayaWindow * p_window = p_frame->GetWindowParent();
  if (!p_window)
    {
      wxASSERT(false);
      return FALSE;
    }

  return p_window->IsFullScreen();
}

/*----------------------------------------------------------------------
  TtaRegisterOpenURLCallback
  params:
  returns:
  ----------------------------------------------------------------------*/
void TtaRegisterOpenURLCallback( void (*callback) (void *) )
{
  /* register openurl callback in order to call it when twice amaya instance are running */
  ((AmayaApp *)wxTheApp)->RegisterOpenURLCallback( callback );
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
  if (!TtaIsSelectionUnique () &&
      (panel_type == WXAMAYA_PANEL_STYLE ||
       panel_type == WXAMAYA_PANEL_ATTRIBUTE ||
       panel_type == WXAMAYA_PANEL_XML))
    return;
  AmayaWindow * activeWindow = TtaGetActiveWindow();
  if(activeWindow)
    activeWindow->SendDataToPanel(panel_type, params);
}
#endif /* _WX */

/*----------------------------------------------------------------------
  TtaCheckLostFocus
  give focus to active canvas if nothing is focused,
  it can occurs because of wxWidgets bugs
  params:
  returns:
  ----------------------------------------------------------------------*/
void TtaCheckLostFocus()
{
  wxWindow *focus = wxWindow::FindFocus();
  if (!focus)
    {
      TTALOGDEBUG_0( TTA_LOG_FOCUS, _T("TtaCheckLostFocus, focus was lost!") );
      TtaRedirectFocus();
    }
}

/*----------------------------------------------------------------------
  TtaRedirectFocus
  give focus to active canvas in order to be able to enter text
  params:
  returns:
  ----------------------------------------------------------------------*/
void TtaRedirectFocus ()
{
  int active_frame_id = TtaGiveActiveFrame();

  AmayaFrame * p_frame = TtaGetFrameFromId( active_frame_id );
  if (p_frame)
    p_frame->GetCanvas()->SetFocus();
  TTALOGDEBUG_1( TTA_LOG_FOCUS, _T("TtaRedirectFocus activeframe=%d"), active_frame_id );
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
ThotBool TtaHandleUnicodeKey (wxKeyEvent& event)
{
  int thot_keycode = event.GetKeyCode();
  int thot_keysym = event.GetUnicodeKey();
  int ret = 0;

  if (thot_keysym != 0 &&
	  ((thot_keysym >= WXK_START && thot_keysym <= WXK_COMMAND) ||
	  (thot_keysym >= WXK_NUMPAD0 && thot_keysym <= WXK_NUMPAD9) ||
	  !TtaIsSpecialKey(thot_keycode)) &&
      (!event.CmdDown() || event.AltDown())
#if !defined(_MACOS) && !defined(_WINDOWS)
       && !event.AltDown()
#endif /* _MACOS */
       )
    {
      wxWindow *p_win_focus = wxWindow::FindFocus();
      wxTextCtrl *p_text_ctrl = wxDynamicCast(p_win_focus, wxTextCtrl);
      wxComboBox *p_combo_box = wxDynamicCast(p_win_focus, wxComboBox);
      wxSpinCtrl *p_spinctrl = wxDynamicCast(p_win_focus, wxSpinCtrl);
      // do not proceed any characteres if the focused widget is a textctrl or a combobox or a spinctrl
      if (!p_text_ctrl && !p_combo_box && !p_spinctrl)
        {
          wxButton *p_button = wxDynamicCast(p_win_focus, wxButton);
          wxCheckListBox *p_check_listbox = wxDynamicCast(p_win_focus, wxCheckListBox);
          // do not proceed "space" key if the focused widget is a button or a wxCheckListBox
          if (!(thot_keycode == WXK_SPACE &&
                (p_button || p_check_listbox)))
            {
              int thotMask = 0;
              if ((event.CmdDown() || event.ControlDown()) && !event.AltDown())
                thotMask |= THOT_MOD_CTRL;
              if (event.AltDown() && !event.CmdDown())
                thotMask |= THOT_MOD_ALT;
              if (event.ShiftDown())
                thotMask |= THOT_MOD_SHIFT; 
              ret = ThotInput (TtaGiveActiveFrame(), thot_keysym, 0, thotMask,
                               thot_keycode, FALSE);
              if (ret == 3)
                {
                /* if a simple caractere has been entred, give focus to canvas
                 * it resolves accesibility problems when the focus is blocked on a panel */
                TtaRedirectFocus();
              
              // try to redraw something because when a key in pressed a long time
              // the ThotInput action is repeted but nothing is shown on the screen 
              // before the user release the key.
              GL_DrawAll();
              return true;
                }
              else 
                return false;
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
  int    thot_keysym = event.GetKeyCode();  
  int    thotMask = 0;

#ifdef _MACOS
  if ((thot_keysym == WXK_WINDOWS_MENU || thot_keysym == WXK_F2)
       && event.ControlDown())
     {
       //event.Skip();
       return false;      
     }
#endif /* _MACOS */

  if (event.CmdDown() || event.ControlDown())
    thotMask |= THOT_MOD_CTRL;
  if (event.AltDown())
    thotMask |= THOT_MOD_ALT;
  if (event.ShiftDown())
    thotMask |= THOT_MOD_SHIFT;
  
  /* +/= key generate '+' key code, but is should generates '=' value */
  if (thot_keysym == '+' && !event.ShiftDown())
    thot_keysym = '=';
  
  /* do not allow CTRL-C CTRL-X CTRL-V in "text" widgets */
  wxWindow *       p_win_focus         = wxWindow::FindFocus();
  wxTextCtrl *     p_text_ctrl         = wxDynamicCast(p_win_focus, wxTextCtrl);
  wxComboBox *     p_combo_box         = wxDynamicCast(p_win_focus, wxComboBox);
  wxSpinCtrl *     p_spinctrl          = wxDynamicCast(p_win_focus, wxSpinCtrl);
  if (( p_text_ctrl || p_combo_box || p_spinctrl ) && event.CmdDown()
#ifdef _WINDOWS
	  && !event.AltDown()
#endif /* _WINDOWS */
	  )
    {
      if (p_combo_box)
        {
#ifndef _WINDOWS
	  // Windows already intercepts the command
          if (thot_keysym == 67) // Ctrl C
            p_combo_box->Copy();
          else if (thot_keysym == 86) // Ctrl V
            p_combo_box->Paste();
          else if (thot_keysym == 88) // Ctrl X
            p_combo_box->Cut();
          else if (thot_keysym == 90) // Ctrl Z
            p_combo_box->Undo();
#endif /* _WINDOWS */
        }
      else if (p_text_ctrl)
        {
          if (thot_keysym == 67) // Ctrl C
            p_text_ctrl->Copy();
          else if (thot_keysym == 86) // Ctrl V
            p_text_ctrl->Paste();
          else if (thot_keysym == 88) // Ctrl X
            p_text_ctrl->Cut();
          else if (thot_keysym == 90) // Ctrl Z
            p_text_ctrl->Undo();
#ifdef _WINDOWS
		  else
		    return false;
#endif /* _WINDOWS */
        }
      return true;
    }

#ifdef _MACOS
  if (( p_text_ctrl || p_combo_box || p_spinctrl ) && event.AltDown())
    {
      // compound character
      event.Skip();
      return true;      
    }
  // on windows, CTRL+ALT is equivalent to ALTGR key
  if (!TtaIsSpecialKey(thot_keysym) &&
      event.ControlDown() && !event.AltDown())
       // this is for the Windows menu shortcuts, ALT+F => should open File menu
#else /* _MACOS */
  // on windows, CTRL+ALT is equivalent to ALTGR key
  if (!TtaIsSpecialKey(thot_keysym) &&
      ((event.CmdDown() && !event.AltDown()) ||
       (event.AltDown() && !event.CmdDown() && (thot_keysym < 'A' || thot_keysym > 'Z'))))
       // this is for the Windows menu shortcuts, ALT+F => should open File menu
#endif /* _MACOS */
    {
      // le code suivant permet de convertir les majuscules
      // en minuscules pour les racourcis clavier specifiques a amaya.
      // OnKeyDown recoit tout le temps des majuscule que Shift soit enfonce ou pas.
      if (!event.ShiftDown())
        {
          // shift key was not pressed
          // force the lowercase
          wxString s((wxChar)thot_keysym);
          if (s.IsAscii())
            {
              TTALOGDEBUG_1( TTA_LOG_KEYINPUT, _T("TtaHandleShortcutKey : thot_keysym=%x s=")+s, thot_keysym );
              s.MakeLower();
              thot_keysym = s.GetChar(0);
            }
        }
      // Call the generic function for key events management
      ThotInput (TtaGiveActiveFrame(), (int)thot_keysym, 0, thotMask,
                 (int)thot_keysym, TRUE);
      
      // try to redraw something because when a key in pressed a long time
      // the ThotInput action is repeted but nothing is shown on the screen 
      // before the user release the key.
      GL_DrawAll();
      
      return true;
    }
  else if ((event.CmdDown() || event.AltDown()) &&
           (thot_keysym == (int) WXK_RIGHT ||
            thot_keysym == (int) WXK_LEFT ||
            thot_keysym == (int) WXK_RETURN ||
            thot_keysym == (int) WXK_DOWN ||
            thot_keysym == (int) WXK_UP ||
            thot_keysym == (int) WXK_HOME ||
            /*thot_keysym == (int) WXK_INSERT ||*/
            thot_keysym == (int) WXK_END))
    {
      /* it is now the turn of special key shortcuts : CTRL+RIGHT, CTRL+ENTER ...*/
      TTALOGDEBUG_1( TTA_LOG_KEYINPUT, _T("TtaHandleShortcutKey : special shortcut thot_keysym=%x"), thot_keysym );
      ThotInput (TtaGiveActiveFrame(), thot_keysym, 0, thotMask, thot_keysym, TRUE);
      
      // try to redraw something because when a key in pressed a long time
      // the ThotInput action is repeted but nothing is shown on the screen 
      // before the user release the key.
      GL_DrawAll();
      
      return true;
    }
  else if ( thot_keysym == (int) WXK_F2 ||
            thot_keysym == (int) WXK_F5 ||
            thot_keysym == (int) WXK_F7 ||
            thot_keysym == (int) WXK_F11 ||
            thot_keysym == (int) WXK_F12)
    {
      ThotInput (TtaGiveActiveFrame(), thot_keysym, 0, thotMask, thot_keysym, TRUE);
      
      // try to redraw something because when a key in pressed a long time
      // the ThotInput action is repeted but nothing is shown on the screen 
      // before the user release the key.
      GL_DrawAll();
      
      return true;
    }
  else
    return false;
}
#endif /* _WX */

/*----------------------------------------------------------------------
  TtaFrameIsShown returs TRUE is the frame is shown
  ----------------------------------------------------------------------*/
ThotBool TtaFrameIsShown (int frame)
{
#ifdef _WX
  // do not draw anything if the animated canvas page is not raidsed
  if (frame > 0 && FrameTable[frame].WdFrame &&
      FrameTable[frame].WdFrame->GetPageParent() &&
      FrameTable[frame].WdFrame->GetPageParent()->IsShown())
    return TRUE;
#endif /* _WX */
  return FALSE;
}

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
  int thot_keysym;
  int frame;
  bool proceed_key;

  if (!event.AltDown() &&
	  TtaIsSpecialKey(event.GetKeyCode()))
    {
      thot_keysym = event.GetKeyCode(); 
#ifdef _MACOS
      if ((thot_keysym == WXK_WINDOWS_MENU || thot_keysym == WXK_F2)
	  && event.ControlDown())
        {
          //event.Skip();
          return false;      
	}
#endif /* _MACOS */
      proceed_key = ( thot_keysym == WXK_INSERT   ||
                      thot_keysym == WXK_DELETE   ||
                      thot_keysym == WXK_HOME     ||
                      thot_keysym == WXK_PRIOR    ||
                      thot_keysym == WXK_NEXT     ||
#ifdef _MACOS
                      thot_keysym == WXK_PAGEUP   ||
                      thot_keysym == WXK_PAGEDOWN ||
#endif /* _MACOS */
                      thot_keysym == WXK_END      ||
                      thot_keysym == WXK_LEFT     ||
                      thot_keysym == WXK_RIGHT    ||
                      thot_keysym == WXK_UP       ||
                      thot_keysym == WXK_DOWN     ||
                      thot_keysym == WXK_ESCAPE   ||
                      thot_keysym == WXK_BACK     ||
                      thot_keysym == WXK_RETURN   ||
                      thot_keysym == WXK_NUMPAD_ENTER ||
                      thot_keysym == WXK_TAB );

#ifdef _MACOS
      if (proceed_key && thot_keysym == WXK_PAGEUP)
        thot_keysym = WXK_PRIOR;
      else if (proceed_key && thot_keysym == WXK_PAGEDOWN)
        thot_keysym = WXK_NEXT;
#endif /* _MACOS */
      
      wxWindow *p_win_focus = wxWindow::FindFocus();
      wxGLCanvas *p_gl_canvas = wxDynamicCast(p_win_focus, wxGLCanvas);
      wxSplitterWindow *p_splitter = wxDynamicCast(p_win_focus, wxSplitterWindow);
      wxNotebook *p_notebook = wxDynamicCast(p_win_focus, wxNotebook);
      wxScrollBar *p_scrollbar = wxDynamicCast(p_win_focus, wxScrollBar);
      wxTextCtrl *p_text_ctrl = wxDynamicCast(p_win_focus, wxTextCtrl);
      wxComboBox *p_combo_box = wxDynamicCast(p_win_focus, wxComboBox);
      wxSpinCtrl *p_spinctrl = wxDynamicCast(p_win_focus, wxSpinCtrl);
      
      if (p_win_focus)
        TTALOGDEBUG_1( TTA_LOG_FOCUS, _T("focus = %s"), p_win_focus->GetClassInfo()->GetClassName())
      else
        TTALOGDEBUG_0( TTA_LOG_FOCUS, _T("no focus"))
    
      if ((p_text_ctrl || p_combo_box || p_spinctrl) &&
	  (thot_keysym == WXK_RETURN || thot_keysym == WXK_TAB))
        {
	  return FALSE;
	}

      /* do not allow special key outside the canvas */
      if (!p_gl_canvas && !p_splitter && !p_notebook &&
          !p_scrollbar && proceed_key)
        {
          event.Skip();
          return true;      
        }
      
      if (proceed_key)
        {
          int  thotMask = 0;

          if (event.CmdDown())
            thotMask |= THOT_MOD_CTRL;
          if (event.ControlDown())
            thotMask |= THOT_MOD_CTRL;
          if (event.AltDown())
            thotMask |= THOT_MOD_ALT;
          if (event.ShiftDown())
            thotMask |= THOT_MOD_SHIFT;

          TTALOGDEBUG_1( TTA_LOG_KEYINPUT, _T("TtaHandleSpecialKey: thot_keysym=%x"), thot_keysym);
          frame = TtaGiveActiveFrame();
          ThotInput (frame, thot_keysym, 0, thotMask, thot_keysym, TRUE);
          // try to redraw something because when a key in pressed a long time
          // the ThotInput action is repeted but nothing is shown on the screen 
          // before the user release the key.
          GL_DrawAll();
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
  if (wx_keycode >= WXK_NUMPAD0 && wx_keycode <= WXK_NUMPAD9)
    return TRUE;
  else
    return ( wx_keycode == WXK_BACK ||
             wx_keycode == WXK_TAB  ||
             wx_keycode == WXK_RETURN ||
             wx_keycode == WXK_ESCAPE ||
             /*wx_keycode == WXK_INSERT  ||*/
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
      pThread->SetConcurrency(0);
      if ( pThread->Create() != wxTHREAD_NO_ERROR )
        {
          TTALOGDEBUG_0( TTA_LOG_SOCKET, _T("TtaSendStatsInfo -> Cant't create thread") );      
        }
      if ( pThread->Run() != wxTHREAD_NO_ERROR )
        {
          TTALOGDEBUG_0( TTA_LOG_SOCKET, _T("TtaSendStatsInfo -> Cant't start thread") );
        }
      /* the stats request has been send, we desactivate the 
       * SEND_STATS flag to avoid counting a amaya user twice */
      TtaSetEnvBoolean ("SEND_STATS", FALSE, TRUE);
      
      /* remember the request has been already send to stats server in oder to count
       * only on time each users */
      TtaSetEnvString ("VERSION", (char *)TtaGetAppVersion(), TRUE);
    }
#endif /* SEND_STATS */
}


#ifdef _WX
static PopupDocContextMenuFuction s_PopupDocContextMenuFuction = NULL;

/*----------------------------------------------------------------------
  TtaSetPopupDocContextMenuFunction()
  Set the function to popup a context menu
  ----------------------------------------------------------------------*/
void TtaSetPopupDocContextMenuFunction(PopupDocContextMenuFuction fn)
{
  s_PopupDocContextMenuFuction = fn;
}


/*----------------------------------------------------------------------
  TtaPopupDocContextMenu(int, wxWindow*)
  Popup a context menu
  ----------------------------------------------------------------------*/
void TtaPopupDocContextMenu(int document, int view, int window, void* win,
			    int x, int y)
{
  if (s_PopupDocContextMenuFuction && win)
    s_PopupDocContextMenuFuction (document, view, window, win, x, y);
}
#endif /* _WX */


/*----------------------------------------------------------------------
  TtaResetEnumContextMenu()
  Reset the return value of a popup menu 
  (id value between 1000 and 2000, -1 if cancelled)
  ----------------------------------------------------------------------*/
void TtaResetEnumContextMenu()
{
  s_enumContextMenuResult = -1;
}

/*----------------------------------------------------------------------
  TtaSetEnumContextMenu()
  Set the return value of a popup menu.
  ----------------------------------------------------------------------*/
void TtaSetEnumContextMenu(int res)
{
  s_enumContextMenuResult = res;
}

/*----------------------------------------------------------------------
  TtaGetEnumContextMenu()
  Get the return value of a popup menu.
  ----------------------------------------------------------------------*/
int TtaGetEnumContextMenu()
{
  return s_enumContextMenuResult;
}


#ifdef _WX
/*----------------------------------------------------------------------
  s_documentPageTypeFunction
  Callback pointer to query an AmayaPage type from the document id.
  ----------------------------------------------------------------------*/
static DocumentPageTypeFunction s_documentPageTypeFunction = NULL;

/*----------------------------------------------------------------------
  TtaRegisterDocumentPageTypeFunction()
  Register the callback to query what type of AmayaPage should be used
  to show a document.
  ----------------------------------------------------------------------*/
void TtaRegisterDocumentPageTypeFunction(DocumentPageTypeFunction fct)
{
  s_documentPageTypeFunction = fct;
}

/*----------------------------------------------------------------------
  TtaGetDocumentPageType()
  Query what type of AmayaPage shoud be used to show a document.
  ----------------------------------------------------------------------*/
int TtaGetDocumentPageType(Document doc)
{
  if(s_documentPageTypeFunction)
    return s_documentPageTypeFunction(doc);
  else
    return WXAMAYAPAGE_SIMPLE;
}

/*----------------------------------------------------------------------
  TtaPostMenuAction execute the corresponding menu action when idle.
  ----------------------------------------------------------------------*/
void TtaPostMenuAction (const char *actionName, Document doc, View view,
                           ThotBool force)
{
  AmayaActionEvent evt(actionName, doc, view, force);
  AmayaApp::PostAmayaAction(evt);
}


/*----------------------------------------------------------------------
  TtaPostMenuActionById execute the corresponding menu action when idle.
  ----------------------------------------------------------------------*/
void TtaPostMenuActionById (int actionId, Document doc, View view,
                           ThotBool force)
{
  AmayaActionEvent evt(actionId, doc, view, force);
  AmayaApp::PostAmayaAction(evt);
}

/*----------------------------------------------------------------------
  TtaCloseAllHelpWindows Closes all opened help windows.
  ----------------------------------------------------------------------*/
void TtaCloseAllHelpWindows ()
{
  AmayaWindowIterator it;
  for( it.first(); !it.isDone(); it.next() )
    {
      AmayaWindow* win = (AmayaWindow*)it.currentElement();
      if(win)
        {
          if(wxDynamicCast(win, AmayaHelpWindow))
            win->Close();
        }
    }
}

/*----------------------------------------------------------------------
  TtaRaiseDoctypePanels
  ----------------------------------------------------------------------*/
void TtaRaiseDoctypePanels(int doctype)
{
  if (!TtaIsSelectionUnique ())
    return;
  AmayaWindow * activeWindow = TtaGetActiveWindow();
  if(activeWindow)
    activeWindow->RaiseDoctypePanels(doctype);
  TtaRedirectFocus ();
}

#endif /* _WX */
