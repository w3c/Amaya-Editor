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
  p_AmayaWindow = new AmayaWindow( window_id, NULL, wxDefaultPosition, wxSize(800,600) );
  if (!p_AmayaWindow)
    return -1; /* no enough memory */
  
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
int TtaMakeFrame( Document doc, int view, int width, int height, int * volume )
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

  /* show the window before frame creation */
  p_AmayaWindow->Show();
  /* create the new frame (window_id is the parent) */
  AmayaFrame * p_AmayaFrame = new AmayaFrame( frame_id, p_AmayaWindow );
  
  /* save frame parameters */
  FrameTable[frame_id].WdFrame 		= p_AmayaFrame;
  FrameTable[frame_id].WdStatus 	= p_AmayaWindow->GetStatusBar();
  FrameTable[frame_id].WdScrollH 	= p_AmayaFrame->GetScrollbarH();
  FrameTable[frame_id].WdScrollV 	= p_AmayaFrame->GetScrollbarV();
  FrameTable[frame_id].FrDoc   		= doc;
  FrameTable[frame_id].FrView   	= view;// TODO 
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
  TtaAttachFrame( frame_id, window_id );
  
  return frame_id; 

#else
  return 0;
#endif /* #ifdef _WX */
}

/*----------------------------------------------------------------------
  TtaAttachFrame attachs a frame to a window
  params:
    + frame_id : the frame
    + window_id : the aimed window
  returns:
    + true if ok
    + false if it's impossible to attach the frame to the window
  ----------------------------------------------------------------------*/
bool TtaAttachFrame( int frame_id, int window_id /* TODO rajouter des parametres pour indiquer a quel endroit dans la window on attache la frame (partie haute, partie basse, quel onglet?, frame volante ?)*/ )
{
#ifdef _WX
  AmayaWindow * p_window = WindowsTable[window_id];

  if (p_window == NULL)
    return false;

  return p_window->AttachFrame( FrameTable[frame_id].WdFrame );  
#else
  return false;
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
bool TtaDetachFrame( int frame_id, int window_id )
{
#ifdef _WX
  return false;
#else
  return false;
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
bool TtaDetachPanel( int panel_id, int window_id )
{
#ifdef _WX
  return false;
#else
  return false;
#endif /* #ifdef _WX */
}
