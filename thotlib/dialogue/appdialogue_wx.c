
#ifdef _WX
  #include "wx/wx.h"
  #include "wx/bmpbuttn.h"
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

#include "appdialogue_f.h"
#include "appdialogue_wx_f.h"
#include "applicationapi_f.h"
#include "font_f.h"
#include "editcommands_f.h"
#include "profiles_f.h"
#include "displayview_f.h"

#define THOT_EXPORT extern
#include "frame_tv.h"
#include "boxparams_f.h"
#include "thotmsg_f.h"
#include "dialogapi_f.h"

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
  
  /* Create the window */
  switch ( kind )
    {
    case WXAMAYAWINDOW_NORMAL:
      p_window = new AmayaNormalWindow( window_id,
					p_parent_window,
					wxDefaultPosition,
					wxDefaultSize );
      break;
    case WXAMAYAWINDOW_SIMPLE:
      p_window = new AmayaSimpleWindow( window_id,
					p_parent_window,
					wxDefaultPosition,
					wxDefaultSize );
      break;
    }

  if (!p_window)
    return -1; /* no enough memory */
  
  if (w != 0 && h != 0)
    p_window->SetSize(-1, -1, w, h);
  else
    p_window->SetSize(-1, -1, 800, 600);
    
  /* save the window reference into the global array */ 
  WindowTable[window_id].WdWindow = p_window;
  WindowTable[window_id].FrWidth  = p_window->GetSize().GetWidth();
  WindowTable[window_id].FrHeight = p_window->GetSize().GetHeight();
  WindowTable[window_id].WdStatus = p_window->GetStatusBar();

  // show the window if not already shown
  TtaShowWindow( window_id, TRUE );

  return window_id;
#else
  return 0;
#endif /* #ifdef _WX */
}

/*----------------------------------------------------------------------
  TtaMakeMenuBar creates the frame
  ----------------------------------------------------------------------*/
static void TtaMakeMenuBar( int frame_id, const char * schema_name )
{
#ifdef _WX
  SchemaMenu_Ctl     *SCHmenu;
  Menu_Ctl           *ptrmenu;
  int                 i = 0;
  /* reference of the buildbing menu */
  int                 ref = frame_id + MAX_LocalMenu;
  int doc_id = FrameTable[frame_id].FrDoc;
  int schView = FrameTable[frame_id].FrView;

  AmayaFrame *        p_AmayaFrame = FrameTable[frame_id].WdFrame;
  ThotMenuBar         p_menu_bar   = p_AmayaFrame->GetMenuBar();

  /* Look for the menu list to be built */
  ptrmenu = NULL;
  ThotBool withMenu = TRUE;
  if (withMenu)
    {
      SCHmenu = SchemasMenuList;
      while (SCHmenu && ptrmenu == NULL)
	{
	  if (!strcmp (schema_name, SCHmenu->SchemaName))
	    /* that document has specific menus */
	    ptrmenu = SCHmenu->SchemaMenu;
	  else
	    /* next schema */
	    SCHmenu = SCHmenu->NextSchema;
	}
      if (ptrmenu == NULL)
	/* the document uses standard menus */
	ptrmenu = DocumentMenuList;
    }
  
  /**** Build menus ****/
  FrameTable[frame_id].FrMenus = ptrmenu;
  /* Initialise les menus dynamiques */
  FrameTable[frame_id].MenuAttr = -1;
  FrameTable[frame_id].MenuSelect = -1;
  FrameTable[frame_id].MenuPaste = -1;
  FrameTable[frame_id].MenuUndo = -1;
  FrameTable[frame_id].MenuRedo = -1;
  FrameTable[frame_id].MenuContext = -1;
  
  while (p_menu_bar && ptrmenu)
    {
      /* skip menus that concern another view */
      if ( (ptrmenu->MenuView == 0 || ptrmenu->MenuView == schView) &&
	   Prof_ShowMenu (ptrmenu))
	{
	  wxMenu * p_menu = new wxMenu;
	  
	  /* Le menu contextuel ne doit pas etre accroche a notre bar de menu */
	  /* il sera affiche qd le boutton droit de la souris sera active */
	  if (!ptrmenu->MenuContext)
	    {
	      p_menu_bar->Append( p_menu,
				  TtaConvMessageToWX( TtaGetMessage (THOT, ptrmenu->MenuID) ) );
	    }
	  
	  FrameTable[frame_id].WdMenus[i]      = p_menu;
	  FrameTable[frame_id].EnabledMenus[i] = TRUE;
	  
	  /* On note l'id du menu contextuel pour notre frame courante,
	   * c'est ce qui nous permettra de l'afficher plus tard */
	  if (ptrmenu->MenuContext) 
	    FrameTable[frame_id].MenuContext = ptrmenu->MenuID;

	  /* Evite la construction des menus dynamiques */
	  if (ptrmenu->MenuAttr)
	    FrameTable[frame_id].MenuAttr = ptrmenu->MenuID;
	  else if (ptrmenu->MenuSelect) 
	    FrameTable[frame_id].MenuSelect = ptrmenu->MenuID;
	  else 
	    /* on termine par la construction des widgets que l'on accroche a notre p_menu */
	    BuildPopdown (ptrmenu, ref, p_menu, frame_id, doc_id,
			  FALSE, FALSE);	      
	}
      ptrmenu = ptrmenu->NextMenu;
      ref += MAX_ITEM;
      i++;
    }
  
  /* enable other menu entries */
  while (i < MAX_MENU)
    {
      FrameTable[frame_id].EnabledMenus[i] = FALSE;
      i++;
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
		  int * volume )
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
      /* a new frame widget should be created */
      
      /* get the first existing window (a virtual parent) */
      /* the window parent is override when the frame is attached to a page */
      int window_id = 1;
      while ( window_id < MAX_WINDOW && !WindowTable[window_id].WdWindow )
	  window_id++;
      AmayaWindow * p_AmayaWindow = WindowTable[window_id].WdWindow; 
      wxASSERT_MSG(p_AmayaWindow, _T("TtaMakeFrame: the window must be created before any frame"));
      
      /* create the new frame (window_id is the parent) */
      p_AmayaFrame = new AmayaFrame( frame_id, p_AmayaWindow );
    }
  else
    {
      /* if a frame already exist, be sure to cleanup all its atributs */
      DestroyFrame( frame_id );
    }
  
  /* save frame parameters */
  FrameTable[frame_id].WdFrame 	      = p_AmayaFrame;
  FrameTable[frame_id].WdScrollH      = p_AmayaFrame->GetScrollbarH();
  FrameTable[frame_id].WdScrollV      = p_AmayaFrame->GetScrollbarV();
  FrameTable[frame_id].FrWindowId     = -1; /* this attribut is set when TtaAttachFrame is called */
  FrameTable[frame_id].FrPageId       = -1; /* this attribut is set when TtaAttachFrame is called */
  FrameTable[frame_id].FrTopMargin    = 0; // TODO
  FrameTable[frame_id].FrScrollOrg    = 0; // TODO
  FrameTable[frame_id].FrScrollWidth  = 0; // TODO
  FrameTable[frame_id].FrWidth        = 0; //width
  FrameTable[frame_id].FrHeight       = 0; // height
  
  /* get registry default values for visibility */
  char * visiStr = TtaGetEnvString ("VISIBILITY");
  int visiVal;
  if (visiStr == NULL)
    visiVal = 5;
  else
    {
      visiVal = atoi (visiStr);
      if (visiVal < 0 || visiVal > 10)
	visiVal = 5;
    }
  /* Initialise la visibilite et le zoom de la fenetre */
  InitializeFrameParams (frame_id, visiVal, 0);
  /* Initialise la couleur de fond */
  /* SG : not used */
  /*BackgroundColor[frame_id] = DefaultBColor;*/

  /* the document title will be used to name the frame's page */
  p_AmayaFrame->SetFrameTitle( TtaConvMessageToWX( doc_name ) );
#ifdef _GL
  FrameTable[frame_id].Scroll_enabled   = TRUE;
#endif /* _GL */

  /* Window volume in characters */
  *volume = GetCharsCapacity (width * height * 5);
  FrameTable[frame_id].FrDoc   		= doc_id;
  FrameTable[frame_id].FrView   	= schView;
  TtaMakeMenuBar( frame_id, schema_name );

  return frame_id;
#else
  return 0;
#endif /* #ifdef _WX */
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
  
  AmayaWindow * p_window = WindowTable[window_id].WdWindow;
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
	}
      
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
  wxSafeYield();
	
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
  wxLogDebug(_T("TtaDestroyFrame: frame_id=%d"), frame_id);

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
  while ( window_id < MAX_WINDOW && TtaGetWindowFromId( window_id ) )
    {
      window_id++;
    }
  return TtaGetWindowFromId(window_id) ? MAX_WINDOW : window_id - 1;
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
  TtaInitializeURLBar initialize urlbar with given parameters
  this will add the urlbar to the toolbar if it doesn't exist yet
  params:
    + frame_id : frame identifier
    + label : the new url entry
    + editable
    + procedure: procedure to be executed when the new entry is changed by the user.
  returns:
  ----------------------------------------------------------------------*/
void TtaInitializeURLBar( int          frame_id,
			  const char * label,
			  ThotBool     editable,
			  void (*      procedure)() )
{
#ifdef _WX
  if (!FrameTable[frame_id].WdFrame || FrameTable[frame_id].FrWindowId == -1)
    return;
  AmayaWindow * p_window = WindowTable[FrameTable[frame_id].FrWindowId].WdWindow;
  if ( !p_window )
    return;
  
  // add the url to the toolbar if it's not allready done
  p_window->SetupURLBar();

  // setup the callback to activate when a url is selected
  FrameTable[frame_id].Call_Text = procedure;

  // setup the enable/disable flag of the url bar
  FrameTable[frame_id].WdFrame->SetFrameEnableURL( TRUE );
#endif /* #ifdef _WX */
}

/*----------------------------------------------------------------------
  TtaSetURLBar setup the urlbar with a given list of urls (destroy existing urls)
  params:
    + frame_id : frame identifier
    + listUrl : the url list
  returns:
  ----------------------------------------------------------------------*/
void TtaSetURLBar( int frame_id,
		   const char * listUrl )
{
#ifdef _WX
  if (!FrameTable[frame_id].WdFrame || FrameTable[frame_id].FrWindowId == -1)
    return;
  AmayaWindow * p_window = WindowTable[FrameTable[frame_id].FrWindowId].WdWindow;
  if ( !p_window )
    return;

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

  wxLogDebug( _T("TtaSetURLBar:")+
	      wxString(_T(" url="))+firsturl );
#endif /* #ifdef _WX */
}

/*----------------------------------------------------------------------
  APP_Callback_URLActivate - Callback to set url in box when Enter key pressed
  params:
    + frame_id : frame identifier
    + text : the new url text
  returns:
  ----------------------------------------------------------------------*/
void APP_Callback_URLActivate (int frame_id, const char *text)
{
  Document            doc;
  View                view;

  CloseTextInsertion ();
  if (text && strlen(text) > 0)
    {
      FrameToView (frame_id, &doc, &view);
      if (FrameTable[frame_id].Call_Text)
	(*(Proc3)FrameTable[frame_id].Call_Text) ((void *)doc, (void *)view, (void *)text);
    }
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
#ifdef _WX
  Document            document;
  View                view;

  if ( button_id < MAX_BUTTON &&
       button_id > 0 )
    {
      if ( !FrameTable[frame_id].EnabledButton[button_id] )
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
    }
#endif /* _WX */
}

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
	  FrameTable[frame_id].EnabledButton[button_id] = status;
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
}

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
	  wxASSERT( p_panel );
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
	  p_subpanel->RefreshCheckButtonState( p_checked_array );
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
			   int button_id )
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
	      p_checked_array[button_id] = !status;
	      TtaRefreshPanelButton( doc, view, panel_type );
	    }
	}
    }
#endif /* _WX */
}


/*----------------------------------------------------------------------
  TtaSetupPanel - 
  init the panel for the given window
  params:
    + type : the panel type
    + window_id : window identifier
  returns:
  ----------------------------------------------------------------------*/
void TtaSetupPanel( int panel_type,
		    int window_id,
		    int button_id,
		    const char * tooltip,
		    void (*procedure) () )
{
#ifdef _WX
  AmayaWindow * p_window = TtaGetWindowFromId(window_id);
  wxASSERT( p_window );
  if ( !p_window || p_window->GetKind() == WXAMAYAWINDOW_SIMPLE )
    return;

  // register the callback
  switch (panel_type)
    {
    case WXAMAYA_PANEL_XHTML:
      WindowTable[window_id].Tooltip_Panel_XHTML[button_id] = tooltip;
      WindowTable[window_id].Call_Panel_XHTML[button_id]    = procedure;
      break;
    }
#endif /* _WX */
}

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
      // get the parent window
      int window_id = FrameTable[frame_id].FrWindowId;
      if ( window_id < 0 )
	return; /* there is no parents */

      CloseTextInsertion ();
      FrameToView (frame_id, &document, &view);
      TtaSetButtonActivatedStatus (TRUE);
      switch (type)
	{
	case WXAMAYA_PANEL_XHTML:
	  if (WindowTable[window_id].Call_Panel_XHTML[button_id])
	    (*(Proc2)WindowTable[window_id].Call_Panel_XHTML[button_id]) ((void *)document, (void *)view);
	  break;
	}
      TtaSetButtonActivatedStatus (FALSE);
    }
#endif /* _WX */
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
  this function returns the contextual menu of the active frame into the given window
  params:
    + window_id : the parent window of the active frame
  returns:
    + wxMenu * : a pointer on a wxMenu, call PopupMenu to show it.
  ----------------------------------------------------------------------*/
wxMenu * TtaGetContextMenu( int window_id, int page_id, int frame_id )
{
  int menu_id = 0;
  if (frame_id == -1)
    {
      if (window_id == -1)
	{
	  wxASSERT_MSG(FALSE, _T("Context menu cannot be created"));
	  return NULL;
	}
      AmayaWindow * p_window = TtaGetWindowFromId( window_id );
      if (page_id == -1)
	{
	  // take the current active frame of the current active page
	  if ( p_window && p_window->GetActiveFrame() )
	    frame_id = p_window->GetActiveFrame()->GetFrameId();
	}
      else
	{
	  // take the current active frame of the given page
	  AmayaPage * p_page = p_window->GetPage( page_id );
	  if (!p_page)
	    {
	      wxASSERT_MSG(FALSE, _T("Context menu cannot be created (given page does not exists)"));
	      return NULL;
	    }
	  frame_id = p_page->GetActiveFrame()->GetFrameId();
	}
    }

  if (frame_id)
    menu_id = FrameTable[frame_id].MenuContext;
  if (menu_id)
    return FrameTable[frame_id].WdMenus[menu_id];
  else
    return NULL;
}
#endif /* _WX */


#ifdef _WX
/*----------------------------------------------------------------------
  TtaRefreshMenuStats - 
  this function should be called to synchronize the menu stats (enable/disable)
  with FrameTable.EnabledMenus array.
  params:
    + p_menu_bar : the menu bar to synchronize
  returns:
  ----------------------------------------------------------------------*/
void TtaRefreshMenuStats( wxMenuBar * p_menu_bar )
{
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

  // the frame owner has been found, update it !
  int menu_id = 0;
  int top_menu_id = 0;
  wxMenu * p_menu = NULL;
  while ( menu_id < MAX_MENU )
    {
      p_menu = FrameTable[frame_id].WdMenus[menu_id];
      if (p_menu)
	{
	  // find the corrsponding menu position in the Top Menubar
	  top_menu_id = p_menu_bar->FindMenu(p_menu->GetTitle());
	  // we must check that the menu has been found because the contextual menu do not have a title
	  if (top_menu_id >= 0)
	    {
	      // it has been found, update it
	      p_menu_bar->EnableTop(top_menu_id, FrameTable[frame_id].EnabledMenus[menu_id]);
	    }
	}
      menu_id++;
    }
}
#endif /* _WX */
