
#ifdef _WX
  #include "wx/wx.h"
#endif /* _WX */

#include "thot_gui.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "frame.h"
#include "registry.h"
#include "appdialogue.h"
#include "message.h"

#include "appdialogue_f.h"
#include "appdialogue_wx_f.h"
#include "font_f.h"
#include "profiles_f.h"

#define THOT_EXPORT extern
#include "frame_tv.h"
#include "boxparams_f.h"

#include "AmayaFrame.h"
#include "AmayaWindow.h"
#include "AmayaPage.h"

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
  AmayaWindow * p_window = WindowsTable[window_id];
  if (p_window == NULL)
    return;

  p_window->Show( show );
#endif /* _WX */
}

/*----------------------------------------------------------------------
  TtaMakeWindow create a AmayaWindow object and place it
  into WindowsTable array
  returns:
 	+ the window id
        + 0 if too much created windows
  ----------------------------------------------------------------------*/
int TtaMakeWindow( )
{
#ifdef _WX
  AmayaWindow * p_AmayaWindow = NULL;
  int window_id = 1;
 
  /* look for a free id */
  while (window_id < MAX_WINDOW && WindowsTable[window_id] != NULL)
    window_id++;
  if (window_id >= MAX_WINDOW)
    return 0; /* there is no more free windows */

  /* Create the window */
  p_AmayaWindow = new AmayaWindow( window_id, NULL, wxDefaultPosition, wxDefaultSize );
  if (!p_AmayaWindow)
    return -1; /* no enough memory */
  
  p_AmayaWindow->SetSize(-1, -1, 800, 600);
  
  /* save the window reference into the global array */ 
  WindowsTable[window_id] = p_AmayaWindow;

  // show it
  TtaShowWindow( window_id, TRUE );
  
  return window_id;
#else
  return 0;
#endif /* #ifdef _WX */
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
      found = (FrameTable[frame_id].FrDoc == 0 && FrameTable[frame_id].WdFrame != 0);
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

      /* get the document's top window */
      int window_id = TtaGetWindowId( doc_id );
      AmayaWindow * p_AmayaWindow = WindowsTable[window_id]; 
      
      /* create the new frame (window_id is the parent) */
      p_AmayaFrame = new AmayaFrame( frame_id, p_AmayaWindow );


      /* --------------------------------------------------- */
      /* create the menu for this frame */
      /* --------------------------------------------------- */
      SchemaMenu_Ctl     *SCHmenu;
      Menu_Ctl           *ptrmenu;
      int                 i = 0;
      /* reference du menu construit */
      int                 ref = frame_id + MAX_LocalMenu;
      ThotMenuBar         p_menu_bar = p_AmayaFrame->GetMenuBar();
      
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

      while (p_menu_bar && ptrmenu)
	{
	  /* skip menus that concern another view */
	  if ( (ptrmenu->MenuView == 0 || ptrmenu->MenuView == schView) &&
	       Prof_ShowMenu (ptrmenu))
	    {
	      wxMenu * p_menu = new wxMenu;
	      p_menu_bar->Append( p_menu,
				  wxString( TtaGetMessage (THOT, ptrmenu->MenuID),
					    AmayaWindow::conv_ascii) );

	      FrameTable[frame_id].WdMenus[i]      = p_menu;
	      FrameTable[frame_id].EnabledMenus[i] = TRUE;
	      /* Evite la construction des menus dynamiques */
	      if (ptrmenu->MenuAttr)
		FrameTable[frame_id].MenuAttr = ptrmenu->MenuID;
	      else if (ptrmenu->MenuSelect) 
		FrameTable[frame_id].MenuSelect = ptrmenu->MenuID;
	      else 
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

      /* --------------------------------------------------- */


      /* save frame parameters */
      FrameTable[frame_id].WdFrame 	  = p_AmayaFrame;
      //      FrameTable[frame_id].WdStatus 	  = p_AmayaWindow->GetStatusBar(); /* this attribut is set when TtaAttachFrame is called */
      FrameTable[frame_id].WdScrollH 	  = p_AmayaFrame->GetScrollbarH();
      FrameTable[frame_id].WdScrollV 	  = p_AmayaFrame->GetScrollbarV();
      FrameTable[frame_id].FrWindowId     = -1; /* this attribut is set when TtaAttachFrame is called */
      FrameTable[frame_id].FrPageId       = -1; /* this attribut is set when TtaAttachFrame is called */
      FrameTable[frame_id].FrTopMargin 	  = 0;// TODO
      FrameTable[frame_id].FrScrollOrg 	  = 0;// TODO
      FrameTable[frame_id].FrScrollWidth  = 0;// TODO
      FrameTable[frame_id].FrWidth        = width;// TODO
      FrameTable[frame_id].FrHeight 	  = height;// TODO

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
    }
  
  /* the document title will be used to name the frame's page */
  p_AmayaFrame->SetPageTitle( wxString(doc_name, AmayaWindow::conv_ascii) );

  /* Window volume in characters */
  *volume = GetCharsCapacity (width * height * 5);
  FrameTable[frame_id].FrDoc   		= doc_id;
  FrameTable[frame_id].FrView   	= schView;
#ifdef _GL
  FrameTable[frame_id].Scroll_enabled   = TRUE;
#endif /* _GL */

  /* show it ... */
  //p_AmayaFrame->Show();

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
  
  AmayaWindow * p_window = WindowsTable[window_id];
  if (p_window == NULL)
    return FALSE;
  
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

  /* update frame infos */
  /*  FrameTable[frame_id].WdStatus 	= p_window->GetStatusBar(); */
  FrameTable[frame_id].FrWindowId   	= window_id;
  FrameTable[frame_id].FrPageId         = page_id;

  p_page->Show();

  /* wait for frame initialisation (needed by opengl) */
  TtaHandlePendingEvents();
  
  return TRUE;
#else
  return FALSE;
#endif /* #ifdef _WX */
}

/*----------------------------------------------------------------------
  TtaDetachFrame detachs a frame from a window
  params:
    + frame_id : the frame identifier
    + window_id : windows identifier (if -1, the frame is
                  searched into the windows table)
  returns:
    + true if ok
    + false if it's impossible to attach the frame to the window
  ----------------------------------------------------------------------*/
ThotBool TtaDetachFrame( int window_id, int page_id, int position )
{
#ifdef _WX
  AmayaWindow * p_window = WindowsTable[window_id];
  if (p_window == NULL)
    return FALSE;
  
  AmayaPage * p_page = p_window->GetPage(page_id);
  if (!p_page)
    return FALSE;

  /* now detach the frame from this page */
  AmayaFrame * p_frame = NULL;
  p_frame = p_page->DetachFrame( position );

  if (p_frame)
    {
      /* a frame hs been detached so get his frame id and update the FrameTable */
      int frame_id = p_frame->GetFrameId();
   
      /* update frame infos */
      //      FrameTable[frame_id].WdStatus 	= NULL;
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

  AmayaWindow * p_window = WindowsTable[window_id];
  if (p_window == NULL)
    return FALSE;
  
  AmayaPage * p_page = p_window->GetPage(page_id);
  if (!p_page)
    return FALSE;


  /* now detach the frame from this page */
  int position = p_page->GetFramePosition( p_frame );
  AmayaFrame * p_detached_frame = NULL;
  p_detached_frame = p_page->DetachFrame( position );

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

  AmayaFrame * p_frame = FrameTable[frame_id].WdFrame;
  
  if (!p_frame)
    return FALSE;
  
  //  TtaDetachFrame( frame_id );

  //  TtaHandlePendingEvents();
  p_frame->DestroyFrame();

  return TRUE;
#else /* _WX */
  return FALSE;
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
  AmayaWindow * p_window = WindowsTable[window_id];
  if (p_window == NULL)
    return -1;

  /* just return the pages count (first page id is 0) */
  return p_window->GetPageCount();
#else
  return -1;
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

