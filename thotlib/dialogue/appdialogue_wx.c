
#ifdef _WX
#include "wx/wx.h"
#endif /* _WX */

#include "thot_gui.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "frame.h"

#include "appdialogue_wx_f.h"
#include "font_f.h"

#define THOT_EXPORT extern
#include "frame_tv.h"

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
void TtaShowWindow( int window_id, bool show )
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
    + int view : the view to attach
  returns:
    + the frame id
    + 0 if too much created views
  ----------------------------------------------------------------------*/
int TtaMakeFrame( Document doc,
                  int view,
		  const char * doc_name,
		  int width,
		  int height,
		  int * volume )
{
#ifdef _WX
  
  /* finding a free frame id */
  int    frame_id = 1;
  bool   found = false;
  while (frame_id <= MAX_FRAME && !found)
    {
      found = (FrameTable[frame_id].FrDoc == 0 && FrameTable[frame_id].WdFrame == 0);
      if (!found)
        frame_id++;
    }

  if (!found)
    return 0; // too much created frames : bye bye !

  /* get the document's top window */
  int window_id = TtaGetWindowId( doc );
  AmayaWindow * p_AmayaWindow = WindowsTable[window_id]; 

  /* create the new frame (window_id is the parent) */
  AmayaFrame * p_AmayaFrame = new AmayaFrame( frame_id, p_AmayaWindow );
  
  /* the document title will be used to name the frame's page */
  p_AmayaFrame->SetPageTitle( wxString(doc_name, AmayaWindow::conv_ascii) );
  
  /* save frame parameters */
  FrameTable[frame_id].WdFrame 		= p_AmayaFrame;
  FrameTable[frame_id].WdStatus 	= p_AmayaWindow->GetStatusBar(); /* this attribut is set when TtaAttachFrame is called */
  FrameTable[frame_id].WdScrollH 	= p_AmayaFrame->GetScrollbarH();
  FrameTable[frame_id].WdScrollV 	= p_AmayaFrame->GetScrollbarV();
  FrameTable[frame_id].FrWindowId   	= -1; /* this attribut is set when TtaAttachFrame is called */
  FrameTable[frame_id].FrPageId         = -1; /* this attribut is set when TtaAttachFrame is called */
  FrameTable[frame_id].FrDoc   		= doc;
  FrameTable[frame_id].FrView   	= view;
  FrameTable[frame_id].FrTopMargin 	= 0;// TODO
  FrameTable[frame_id].FrScrollOrg 	= 0;// TODO
  FrameTable[frame_id].FrScrollWidth 	= 0;// TODO
  FrameTable[frame_id].FrWidth 		= width;// TODO
  FrameTable[frame_id].FrHeight 	= height;// TODO
#ifdef _GL
  FrameTable[frame_id].Scroll_enabled   = TRUE;
#endif /* _GL */

  /* Window volume in characters */
  *volume = GetCharsCapacity (width * height * 5);

  /* attach this frame to the document window */
/*  TtaAttachFrame( frame_id, window_id );*/
  
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
ThotBool TtaAttachFrame( int frame_id, int window_id, int page_id /* TODO rajouter des parametres pour indiquer a quel endroit dans la window on attache la frame (partie haute, partie basse, quel onglet?, frame volante ?)*/ )
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

  /* now attach the frame to this page */
  /* TODO : attacher la frame en haut ou en bas 
   *        suivant le comportement a adopter */
  p_page->AttachTopFrame( FrameTable[frame_id].WdFrame );

  /* update frame infos */
  FrameTable[frame_id].WdStatus 	= p_window->GetStatusBar();
  FrameTable[frame_id].FrWindowId   	= window_id;
  FrameTable[frame_id].FrPageId         = page_id;
  
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
ThotBool TtaDetachFrame( int frame_id, int window_id )
{
#ifdef _WX
  return FALSE;
#else
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

