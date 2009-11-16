/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2009
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * Handle application frames
 *
 * Authors: I. Vatton (INRIA)
 *          R. Guetari (W3C/INRIA) - Windows version
 *          P. Cheyrou-Lagreze (INRIA) - OpenGL Version
 *          S. Gully (INRIA) - GTK and wxWidgets Versions
 */

#include "wx/wx.h"
#include "wx/utils.h"

#include "thot_gui.h"
#include "thot_sys.h"
#include "thot_key.h"
#include "constmedia.h"
#include "constmenu.h"
#include "typemedia.h"
#include "message.h"
#include "dialog.h"
#include "application.h"
#include "interface.h"
#include "document.h"
#include "view.h"
#include "appdialogue.h"
#include "thotcolor.h"
#include "picture.h"
#include "logdebug.h"
#include "appversion.h"

#ifdef _GL
#ifdef _MACOS
#include <gl.h>
#else /* _MACOS */
#include <GL/gl.h>
#endif /* _MACOS */
#include "glwindowdisplay.h"
#else /*_GL*/
#include "windowdisplay_f.h"
#endif /*_GL*/
static ThotBool     ComputeScrollBar = TRUE;

/* Focus change callback procedure */
static Proc1        ChangeFocusFunction = NULL;
static char         OldMsgSelect[MAX_TXT_LEN];
static PtrDocument  OldDocMsgSelect;

/* this is a buffer where the appversion string is stored 
 * to query this string use TtaGetAppVersion function */
static char     TtAppVersion[MAX_TXT_LEN];
static ThotBool TtAppVersion_IsInit = FALSE;

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "boxes_tv.h"
#include "font_tv.h"
#include "edit_tv.h"
#include "displayview_f.h"
#include "frame_tv.h"
#include "appdialogue_tv.h"
#include "platform_tv.h"
#include "thotcolor_tv.h"
#include "select_tv.h"
#include "attrmenu.h"

#include "AmayaWindow.h"
#include "AmayaFrame.h"
#include "AmayaCanvas.h"
#include "appdialogue_wx.h"
#include "message_wx.h"
#include "AmayaScrollBar.h"
#include "AmayaStatusBar.h"

#include "appli_f.h"
#include "absboxes_f.h"
#include "appdialogue_f.h"
#include "applicationapi_f.h"
#include "attrmenu_f.h"
#include "boxlocate_f.h"
#include "boxparams_f.h"
#include "boxselection_f.h"
#include "buildboxes_f.h"
#include "callback_f.h"
#include "context_f.h"
#include "editcommands_f.h"
#include "font_f.h"
#include "frame_f.h"
#include "inites_f.h"
#include "interface_f.h"
#include "memory_f.h"
#include "message_f.h"
#include "picture_f.h"
#include "scroll_f.h"
#include "structcommands_f.h"
#include "structcreation_f.h"
#include "structmodif_f.h"
#include "structselect_f.h"
#include "textcommands_f.h"
#include "thotmsg_f.h"
#include "uconvert_f.h"
#include "views_f.h"
#include "viewapi_f.h"
#include "appdialogue_wx_f.h"
#include "paneltypes_wx.h"

/* defined into amaya directory ...*/
extern void ZoomIn (Document document, View view);
extern void ZoomOut (Document document, View view);

#ifdef _GL
// This flag is used to recalculate the glcanvas after a RESIZE event
// because GTK&GL clear automaticaly the GL canvas just after the frame is resized.
// (it appends only on some hardware opengl implementations on Linux)
//static ThotBool g_NeedRedisplayAllTheFrame[MAX_FRAME];
#endif /* _GL */


/*----------------------------------------------------------------------
  GetFrameNumber returns the Thot window number associated.
  ----------------------------------------------------------------------*/
int GetFrameNumber (ThotWindow win)
{
  int frame;

  for (frame = 1; frame <= MAX_FRAME; frame++)
    if ((ThotWindow)FrameTable[frame].WdFrame == win)
      return (frame);  
  
  return (-1);
}


/*----------------------------------------------------------------------
  Evenement sur une frame document.                             
  ----------------------------------------------------------------------*/
void FrameKilled (int *w, int frame, int *info)

{
  /* Enleve la procedure de Callback */
  /* Detruit la fenetre si elle existe encore */
  if (frame > 0 && FrameTable[frame].WdFrame != 0)
    ViewClosed (frame);
}


/*----------------------------------------------------------------------
  KillFrameCallback (generic way)
  Called when a clique is done on the up right corner cross
  Kill the current document ( generic version : called by WX and GTK )
  ----------------------------------------------------------------------*/
ThotBool KillFrameCallback( int frame )
{
  PtrDocument         pDoc;
  int                 view;  
  if (frame <= MAX_FRAME)
    {
      GetDocAndView (frame, &pDoc, &view);
      CloseView (pDoc, view);
    }

  for (frame = 0; frame <= MAX_FRAME; frame++)
    if (FrameTable[frame].WdFrame != 0)
      /* there is still an active frame */
      return TRUE;

  return FALSE;
}



/*----------------------------------------------------------------------
  FrameRedraw
  ----------------------------------------------------------------------*/
void FrameRedraw (int frame, unsigned int width, unsigned int height)
{
  int                 dx, dy, view;
  NotifyWindow        notifyDoc;
  Document            doc;

  if (FrameTable[frame].FrDoc == 0)
    return;
  if (width > 0 && height > 0 && 
      documentDisplayMode[FrameTable[frame].FrDoc - 1] != NoComputedDisplay)
    /* ne pas traiter si le document est en mode NoComputedDisplay */
    {
      notifyDoc.event = TteViewResize;
      FrameToView (frame, &doc, &view);
      notifyDoc.document = doc;
      notifyDoc.view = view;
      dx = width - FrameTable[frame].FrWidth;
      dy = height - FrameTable[frame].FrHeight;
      notifyDoc.verticalValue = dy;
      notifyDoc.horizontalValue = dx;
      if (!CallEventType ((NotifyEvent *) & notifyDoc, TRUE))
        {
          /* close current insertion */
          FrameTable[frame].FrWidth = (int) width;
          FrameTable[frame].FrHeight = (int) height;
          /* Il faut reevaluer le contenu de la fenetre */
          RebuildConcreteImage (frame, FALSE);
          /* recompute the scroll bars */
          UpdateScrollbars (frame);
          notifyDoc.event = TteViewResize;
          notifyDoc.document = doc;
          notifyDoc.view = view;
          notifyDoc.verticalValue = dy;
          notifyDoc.horizontalValue = dx;
          CallEventType ((NotifyEvent *) & notifyDoc, FALSE);
        }
    }
}

#ifdef _GL
static int Shared_Context=-1;
/*----------------------------------------------------------------------
  GL_DestroyFrame: Close Opengl pipeline
  ----------------------------------------------------------------------*/
void  GL_DestroyFrame (int frame)
{
#ifndef _NOSHARELIST
  int i;

  if (Printing || frame != GetSharedContext())
    return;
  for (i = 0 ; i <= MAX_FRAME; i++)
    {  
      if (i != GetSharedContext() && !TtaFrameIsClosed(i))
        {
          SetSharedContext(i);
          /* stop the loop */
          i = MAX_FRAME + 1;
        } 
    }
  if (i > MAX_FRAME)
    {
      /* if there is no more frame alive, just reinitialise the shared context to undefined (-1)
         the next created frame will share its context with others */
      SetSharedContext(-1);
    }
#endif /*_NOSHARELIST*/
  FreeAllPicCacheFromFrame (frame);
}

#ifndef _NOSHARELIST
/*----------------------------------------------------------------------
  GetSharedContext : get the name of the frame used as shared context
  ----------------------------------------------------------------------*/
int GetSharedContext ()
{
  return Shared_Context;
}

/*----------------------------------------------------------------------
  SetSharedContext : set the name of the frame used as shared context
  ----------------------------------------------------------------------*/
void SetSharedContext (int frame)
{
  Shared_Context = frame;
}
#endif /*_NOSHARELIST*/
#endif /* _GL */


static ThotBool Current_Expose = FALSE;
/*----------------------------------------------------------------------
  FrameExposeCallback (generic callback) 
  is called when a frame should be redisplayed
  params:
  + int frame : the frame to redisplay
  + int x, y, w, h : the area to redisplay
  return:
  + true if the frame hs been redisplayed
  + false if not
  ----------------------------------------------------------------------*/
ThotBool FrameExposeCallback ( int frame, int x, int y, int w, int h)
{
  ViewFrame           *pFrame;

  if (w <= 0 || h <= 0 || frame == 0 || frame > MAX_FRAME)
    return FALSE;
  if (FrameTable[frame].FrDoc == 0 ||
      documentDisplayMode[FrameTable[frame].FrDoc - 1] == NoComputedDisplay)
    return FALSE;

   if (Current_Expose)
     return FALSE;
  Current_Expose = TRUE;
  pFrame = &ViewFrameTable[frame - 1];
#ifdef _GL
  /* THIS JUST DOESN'T WORK !!!
     even when storing successive x,y and so on...
     it's just gtk and opengl mix bad...
     so the Xfree and gtk guys that tells us 
     it work, just have to come here and code it here
     with an hardware opengl implementation on their PC...
     They will see the Speed problem...*/
  if (GL_prepare (frame))
    {
//      if ( g_NeedRedisplayAllTheFrame[frame] || glhard() || GetBadCard() )
        {
          /* prevent flickering*/
          GL_SwapStop (frame);
          // we need to recalculate the glcanvas only once : after the RESIZE event
          // because GTK&GL clear automaticaly the GL canvas just after the frame is resized.
          // (it appends only on some hardware opengl implementations on Linux)
          //g_NeedRedisplayAllTheFrame[frame] = FALSE;
          
          // refresh the invalide frame content
          x += pFrame->FrXOrg;
          y += pFrame->FrYOrg;
          DefClip (frame, x, y, x + w, y + h);
          RedrawFrameBottom (frame, 0, NULL);
          GL_SwapEnable (frame);
        }
      // display the backbuffer
      GL_Swap (frame);
     }
#else /* _GL */
  x += pFrame->FrXOrg;
  y += pFrame->FrYOrg;
  DefClip (frame, x, y, x + w, y + h);
  RedrawFrameBottom (frame, 0, NULL);
#endif /* _GL */
  Current_Expose = FALSE;
  return TRUE;
}


/*----------------------------------------------------------------------
 * FrameResizedCallback (generic callback)
 * is called when a frame is resized
 * params:
 *   + int frame : the resized frame id
 *   + int new_width, int new_height : the new frame dimension
 * return:
 *   + true if the frame has been redisplayed
 *   + false if not
 ----------------------------------------------------------------------*/
ThotBool FrameResizedCallback (int frame, int new_width, int new_height)
{
  /* check if the frame is valide */
  if (new_width <= 0 ||
      new_height <= 0 || 
      frame == 0 ||
      frame > MAX_FRAME ||
      FrameTable[frame].FrDoc == 0 ||
      documentDisplayMode[FrameTable[frame].FrDoc - 1] == NoComputedDisplay)
    /* frame should not be displayed */
    return FALSE;
  else if ( (FrameTable[frame].FrWidth - new_width == new_height - FrameTable[frame].FrHeight)
            || (abs(FrameTable[frame].FrWidth - new_width) == FrameTable[frame].WdScrollH->GetSize().GetHeight()) 
            || (abs(FrameTable[frame].FrHeight - new_height) == FrameTable[frame].WdScrollV->GetSize().GetWidth())
            )
    {
      /* prevent an infinite loop */
      ComputeScrollBar = FALSE;
    }
  else
    ComputeScrollBar = TRUE;
  TTALOGDEBUG_3( TTA_LOG_DIALOG, _T("FrameResizedCallback: new_width=%d new_height=%d (ComputeScrollBar=%s)"),
                 new_width, new_height, ComputeScrollBar ? _T("TRUE") : _T("FALSE"));

  /* store new frame size */
  FrameTable[frame].FrWidth  = new_width;
  FrameTable[frame].FrHeight = new_height;

  /* redraw */
  if (GL_prepare (frame))
    {
      /* prevent flickering*/
      GLResize (new_width, new_height, 0, 0);
      DefClip (frame, -1, -1, -1, -1);
      FrameRedraw (frame, new_width, new_height);
      GL_SwapEnable (frame);
      GL_Swap (frame);

      //#if !defined(_MACOS) && !defined(_WINDOWS)
      // we need to recalculate the glcanvas after the RESIZE event
      // because GTK&GL clear automaticaly the GL canvas just after the frame is resized.
      // (it appends only on some hardware opengl implementations on Linux)
      //g_NeedRedisplayAllTheFrame[frame] = TRUE;
      //#endif /* !defined(_MACOS) && !defined(_WINDOWS) */
    }
  /* Ok now allow next UpdateScrollbar to hide/show scrollbars 
   * At this point, UpdateScrollbar is not more called, so infinite loop cannot apend */
  ComputeScrollBar = TRUE;
  return TRUE;
}


/*----------------------------------------------------------------------
 * FrameHScrolledCallback (generic callback)
 * is called when the horizontal scrollbar has changed
 * params:
 *   + int frame : the concerned frame
 *   + int position : the new position in the page
 *   + int page_size : the width of the page
 ----------------------------------------------------------------------*/
void FrameHScrolledCallback( int frame, int position, int page_size )
{
  int                 delta, l;
  int                 view;
  NotifyWindow        notifyDoc;
  Document            doc;

  /* ne pas traiter si le document est en mode NoComputedDisplay */
  if (FrameTable[frame].FrDoc &&
      documentDisplayMode[FrameTable[frame].FrDoc - 1] == NoComputedDisplay)
    return;

  /* delta is the position into the page */
  delta = position;

  notifyDoc.event = TteViewScroll;
  FrameToView (frame, &doc, &view);
  notifyDoc.document = doc;
  notifyDoc.view = view;
  notifyDoc.verticalValue = 0;
  notifyDoc.horizontalValue = delta;
  if (!CallEventType ((NotifyEvent *) & notifyDoc, TRUE))
    {
      /* l is the width of the page */
      l = page_size;
      /* On regarde si le deplacement bute sur le bord droit */
      if (position + l >= FrameTable[frame].FrWidth)	     
        delta = FrameTable[frame].FrScrollWidth;
      else
        {
          /* translate the position in the scroll bar into a shift value in the document */
          delta = (int) ((float) (position * FrameTable[frame].FrScrollWidth) / (float) FrameTable[frame].FrWidth);
          delta = delta + FrameTable[frame].FrScrollOrg - ViewFrameTable[frame - 1].FrXOrg;
        }

      if (delta)
        HorizontalScroll (frame, delta, 1);
      notifyDoc.document = doc;
      notifyDoc.view = view;
      notifyDoc.verticalValue = 0;
      notifyDoc.horizontalValue = delta;
      CallEventType ((NotifyEvent *) & notifyDoc, FALSE);
    }
}


/*----------------------------------------------------------------------
 * FrameVScrolledCallback (generic callback)
 * is called when the scrollbar position is changed
 * params:
 *   + int position: the new scrollbar position
 *   + int frame: the concerned frame
 ----------------------------------------------------------------------*/
void FrameVScrolledCallback (int frame, int position)
{
  int delta, x, y, width, height, left, viewed;

  if (FrameTable[frame].FrDoc &&
      documentDisplayMode[FrameTable[frame].FrDoc - 1] == NoComputedDisplay)
    return;
  ComputeDisplayedChars (frame, &x, &y, &width, &height);
  delta = position - y;
  viewed = abs (delta) / height;
  left   = abs (delta) - (int)(height * viewed);
  if (viewed <= 3)
    {
      if (delta > 0)
        delta = viewed * FrameTable[frame].FrHeight 
          + (int) ((left * FrameTable[frame].FrHeight) / height);
      else 
        delta = -(viewed * FrameTable[frame].FrHeight 
                  + (int) ((left * FrameTable[frame].FrHeight) / height));
      if (delta)
        VerticalScroll (frame, delta, 1);
    } 
  else 
    {
      delta = (int)(((float)position / (float)FrameTable[frame].FrHeight) * 100);
      JumpIntoView (frame, delta);
    }
}

/*----------------------------------------------------------------------
  TtcLineUp scrolls one line up.                                    
  ----------------------------------------------------------------------*/
void TtcLineUp (Document document, View view)
{
  int          delta;
  int          frame;
 
  if (document != 0)
    frame = GetWindowNumber (document, view);
  else
    frame = 0;
  delta = -13;
  VerticalScroll (frame, delta, 1);
}

/*----------------------------------------------------------------------
  TtcLineDown scrolls one line down.                                
  ----------------------------------------------------------------------*/
void TtcLineDown (Document document, View view)
{
  int          delta;
  int          frame;

  if (document != 0)
    frame = GetWindowNumber (document, view);
  else
    frame = 0;
  delta = 13;
  VerticalScroll (frame, delta, 1);
}

/*----------------------------------------------------------------------
  TtcScrollLeft scrolls one position left.                                    
  ----------------------------------------------------------------------*/
void TtcScrollLeft (Document document, View view)
{
  int          delta;
  int          frame;
  
  if (document != 0)
    frame = GetWindowNumber (document, view);
  else
    frame = 0;
  delta = -13;
  HorizontalScroll (frame, delta, 1);
}

/*----------------------------------------------------------------------
  TtcScrollRight scrolls one position right.                                
  ----------------------------------------------------------------------*/
void TtcScrollRight (Document document, View view)
{
  int          delta;
  int          frame;

  if (document != 0)
    frame = GetWindowNumber (document, view);
  else
    frame = 0;
  delta = 13;
  HorizontalScroll (frame, delta, 1);
}

/*----------------------------------------------------------------------
  PageUp scrolls one screen up.                                    
  ----------------------------------------------------------------------*/
void TtcPageUp (Document document, View view)
{
  int          delta;
  int          frame;

  if (document == 0)
    return;
  frame = GetWindowNumber (document, view);
  delta = -FrameTable[frame].FrHeight;
  VerticalScroll (frame, delta, 1);
}

/*----------------------------------------------------------------------
  PageDown scrolls one screen down.                                
  ----------------------------------------------------------------------*/
void TtcPageDown (Document document, View view)
{
  int          delta;
  int          frame;

  if (document == 0)
    return;
  frame = GetWindowNumber (document, view);
  delta = FrameTable[frame].FrHeight;
  VerticalScroll (frame, delta, 1);
}


/*----------------------------------------------------------------------
  PageTop goes to the document top.                                
  ----------------------------------------------------------------------*/
void TtcPageTop (Document document, View view)
{
  int          frame;

  if (document != 0)
    frame = GetWindowNumber (document, view);
  else
    frame = 0;
  JumpIntoView (frame, 0);
}

/*----------------------------------------------------------------------
  PageEnd goes to the document end.                                
  ----------------------------------------------------------------------*/
void TtcPageEnd (Document document, View view)
{
  int          frame;

  if (document != 0)
    frame = GetWindowNumber (document, view);
  else
    frame = 0;
  JumpIntoView (frame, 100);
}


/*----------------------------------------------------------------------
  InitializeOtherThings initialise les contextes complementaires.     
  ----------------------------------------------------------------------*/
void InitializeOtherThings ()
{
  int                 i;

  /* Initialisation de la table des widgets de frames */
  for (i = 0; i <= MAX_FRAME; i++)
    /* fill with 0 all the fields */
    memset (&FrameTable[i], 0, sizeof(Frame_Ctl));

  memset( WindowTable, 0, sizeof(Window_Ctl)*(MAX_WINDOW+1) );

  ClickIsDone = 0;
  ClickFrame = 0;
  ClickX = 0;
  ClickY = 0;
  /* message de selection vide */
  OldMsgSelect[0] = EOS;
  OldDocMsgSelect = NULL;
}

/*----------------------------------------------------------------------
  Map and raise the corresponding window.
  ----------------------------------------------------------------------*/
void TtaRaiseView (Document document, View view)
{
  int                 frame_id;
  UserErrorCode = 0;
  frame_id = GetWindowNumber (document, view);
  if (frame_id != 0)
    {
      /* don't remember the last configuration */
      /* we have document and view so it's possibe to know the configuration */
      PtrDocument pDoc    = LoadedDocument[document - 1];
      int         schView = pDoc->DocView[view - 1].DvPSchemaView;
      int window_id;
      int page_id;
      int page_position;
      window_id = TtaGetDocumentWindowId( document, schView );
      TtaGetDocumentPageId( document, schView, &page_id, &page_position );
      
      /* TODO: the page position should depends of the current active view */
      if (page_position == 0)
        page_position = 2;
      TtaAttachFrame (frame_id, window_id, page_id, page_position, 1);
    }
}

/*----------------------------------------------------------------------
  DisplaySelMessage affiche la se'lection donne'e en parame`tre (texte) dans 
  la fenetre active.
  @todo Supprimer tous les appels à cette fonction                                            
  ----------------------------------------------------------------------*/
void DisplaySelMessage (char *text, PtrDocument pDoc)
{
  int                 doc;

  if (ActiveFrame &&
      pDoc && pDoc->DocSSchema &&
      (strcmp (OldMsgSelect, text) || pDoc != OldDocMsgSelect) &&
      strcmp (pDoc->DocSSchema->SsName, "TextFile"))
    {
      /* recupere le document concerne */
      doc = IdentDocument(pDoc);
      TtaSetStatus ((Document) doc, 1, text, NULL);
      /* sel old message */
      strncpy (OldMsgSelect, text, MAX_TXT_LEN);
      OldDocMsgSelect = pDoc;     
    }
}

/*----------------------------------------------------------------------
  TtaSetStatus displays a status message into a document window.
  ----------------------------------------------------------------------*/
void TtaSetStatus (Document document, View view, const char *text, const char *name)
{
  int                 frame, length;
  char                *s = NULL;

  if (document == 0)
    return;
  else
    {
      /* clean up old message */
      OldMsgSelect[0] = EOS;
      frame = GetWindowNumber (document, view);
      if (frame == 0)
        /* try to display in document 1 */
        frame = GetWindowNumber (1, view);
      if (frame != 0)
        {
          length = strlen (text) + 1;
          if (name == NULL)
            {
              s = (char *)TtaGetMemory (length);
              sprintf (s, "%s", text);
            }
          else
            {
              s = (char *)TtaGetMemory (length + strlen (name));
              sprintf (s, text, name);
            }
          if (FrameTable[frame].WdFrame)
            FrameTable[frame].WdFrame->SetStatusBarText( TtaConvMessageToWX( s ) );
          TtaFreeMemory (s);
          s = NULL;
        }
    }
}


/*----------------------------------------------------------------------
  TtaSetStatusSelectedElement Set the current selected element in the status bar.
  ----------------------------------------------------------------------*/
void TtaSetStatusSelectedElement(Document document, View view, Element elem)
{
  AmayaWindow       *window;
  AmayaStatusBar    *statusbar;
  int                frame;
  
  frame = GetWindowNumber (document, view);
  if (frame == 0)
    /* try to display in document 1 */
    frame = GetWindowNumber (1, view);

  if (FrameTable[frame].WdFrame)
    {
      window = wxDynamicCast(wxGetTopLevelParent(FrameTable[frame].WdFrame), AmayaWindow);
      if (window)
        {
          statusbar = wxDynamicCast(window->GetStatusBar(), AmayaStatusBar);
          if (statusbar)
            {
              if (frame == ActiveFrame ||
                  FrameTable[frame].FrDoc == FrameTable[ActiveFrame].FrDoc)
                statusbar->SetSelectedElement (elem);
              else
                statusbar->SetSelectedElement (NULL);
            }
        }
    }
}



/*----------------------------------------------------------------------
 * FrameButtonDownCallback (generic callback)
 * is called when a mouse button is pressed
 * params:
 *   + int frame : where it's pressed
 *   + int thot_button_id : what is pressed
 *   [THOT_LEFT_BUTTON || THOT_MIDDLE_BUTTON || THOT_RIGHT_BUTTON ]
 *   + int thot_mod_mask : what key is currently pressed
 *   [THOT_MOD_CTRL || THOT_MOD_ALT || THOT_MOD_SHIFT]
 *   + int x,y : the mouse position in the frame
 * return:
 *   + TRUE : if the event must be forwarded to other widgets
 *   + FALSE : if the event is cought
 ----------------------------------------------------------------------*/
ThotBool FrameButtonDownCallback (int frame, int thot_button_id,
                                 int thot_mod_mask, int x, int y )
{
#if !defined (_MACOS)
  Document       document;
  View           view;
#endif /* !_MACOS */
  PtrBox         box;
  int            ctrlpt;
    
  /* Amaya is waiting for a click selection ? */
  if (ClickIsDone == 1)
    {
      ClickIsDone = 0;
      ClickFrame = frame;
      ClickX = x;
      ClickY = y;
      return TRUE;
    }

  switch( thot_button_id )
    {
    case THOT_LEFT_BUTTON:
      {
        /* Stop any current insertion of text */
        CloseTextInsertion ();
        box = IsSelectingControlPoint (frame, x, y, &ctrlpt);
        if (box)
          {
            ApplyDirectResize(box, frame, ctrlpt, x, y);
          }
        else if ((thot_mod_mask & THOT_MOD_SHIFT) == THOT_MOD_SHIFT)
          {
            /* a selection extension */
            TtaAbortShowDialogue ();
            LocateSelectionInView (frame, x, y, 1, &Dragging);
#if !defined (_WINDOWS) && !defined (_MACOS)
            FrameToView (frame, &document, &view);
            DoCopyToClipboard (document, view, FALSE, TRUE);
#endif /* _WINDOWS */
            FrameRedraw (frame,
                         FrameTable[frame].FrWidth,
                         FrameTable[frame].FrHeight);
            
          }
#if !defined (_MACOS)
        else if ((thot_mod_mask & THOT_MOD_CTRL) == THOT_MOD_CTRL)
          {	
            ClickFrame = frame;
            ClickX = x;
            ClickY = y;
            if (LocateSelectionInView (frame, ClickX, ClickY, 8, &Dragging))
              return FALSE;
            /* open in a new tab */
            FrameToView (frame, &document, &view);
            TtaExecuteMenuAction ("FollowTheLinkNewTab", document, view, FALSE);
          }
#endif /* MACOS */
        else if (!SelPosition ||
                 ClickFrame != frame || ClickX != x || ClickY != y)
          {
            /* a simple selection */
            ClickFrame = frame;
            ClickX = x;
            ClickY = y;
            /* it's important to setup Dragging before the call of
               LocateSelectionInView because LocateSelectionInView will
               handle gui events (keyup) and Dragging variable
               * will not be unset => cause a infinit selection ! */
            Dragging = TRUE;
            if (LocateSelectionInView (frame, ClickX, ClickY, 2, &Dragging))
              return FALSE;
          }
      }
      break;
      
    case THOT_MIDDLE_BUTTON:
      {
        ClickFrame = frame;
        ClickX = x;
        ClickY = y;
        if (LocateSelectionInView (frame, ClickX, ClickY, 5, &Dragging))
          {
          return FALSE;
          }
      }
      break;
      
    case THOT_RIGHT_BUTTON:
      {
        ClickFrame = frame;
        ClickX = x;
        ClickY = y;
        if (LocateSelectionInView (frame, ClickX, ClickY, 6, &Dragging))
          {
            return FALSE;
          }
      }
      break;
    }
  return TRUE;
}

/*----------------------------------------------------------------------
 * FrameButtonUpCallback (generic callback)
 * is called when a mouse button is released
 * params:
 *   + int frame : where it's released
 *   + int thot_button_id : what is releaseded
 *   [THOT_LEFT_BUTTON || THOT_MIDDLE_BUTTON || THOT_RIGHT_BUTTON ]
 *   + int thot_mod_mask : what key is currently pressed
 *   [THOT_MOD_CTRL || THOT_MOD_ALT || THOT_MOD_SHIFT]
 *   + int x,y : the mouse position in the frame
 * return:
 *   + TRUE : if the event must be forwarded to other widgets
 *   + FALSE : if the event is cought
 ----------------------------------------------------------------------*/
ThotBool FrameButtonUpCallback( int frame, int thot_button_id,
                               int thot_mod_mask, int x, int y )
{
#if !defined(_WINDOWS) && !defined(_MACOS)
  Document   document;
  View       view;
#endif /* !_WINDOWS && !_MACOS*/
  if (Dragging)
    {
      Dragging = FALSE;
#if !defined(_WINDOWS) && !defined(_MACOS)
      FrameToView (frame, &document, &view);
      DoCopyToClipboard (document, view, FALSE, TRUE);
#endif /* _WINDOWS && _MACOS */
    }
  if (thot_button_id == THOT_LEFT_BUTTON)
    {
      ClickFrame = frame;
      ClickX = x;
      ClickY = y;
      if (LocateSelectionInView (frame, ClickX, ClickY, 4, &Dragging))
        return FALSE;
      // SG: j'ai commente la ligne suivante car si le document est modifie 
      // et qu'on desire suivre un lien, un evenement keyup est generer
      // et tue le dialogue qui demande si on veut sauver.
      //    TtaAbortShowDialogue ();
    }
  return TRUE;
}

/*----------------------------------------------------------------------
 * FrameButtonDClickCallback (generic callback)
 * is called when a mouse button is double clicked
 * params:
 *   + int frame : where it's dclicked
 *   + int thot_button_id : what is dclicked
 *   [THOT_LEFT_BUTTON || THOT_MIDDLE_BUTTON || THOT_RIGHT_BUTTON ]
 *   + int thot_mod_mask : what key is currently pressed
 *   [THOT_MOD_CTRL || THOT_MOD_ALT || THOT_MOD_SHIFT]
 *   + int x,y : the mouse position in the frame
 * return:
 *   + TRUE : if the event must be forwarded to other widgets
 *   + FALSE : if the event is cought
 ----------------------------------------------------------------------*/
ThotBool FrameButtonDClickCallback( int frame, int thot_button_id,
                                   int thot_mod_mask, int x, int y )
{
#if !defined (_WINDOWS) && !defined (_MACOS)
  Document   document;
  View       view;
#endif /* !_WINDOWS && !_MACOS*/

  switch (thot_button_id)
    {
    case THOT_LEFT_BUTTON:
      {
        ClickFrame = frame;
        ClickX = x;
        ClickY = y;
        if (LocateSelectionInView (frame, ClickX, ClickY, 3, &Dragging))
	  return FALSE;
#if !defined (_WINDOWS) && !defined (_MACOS)
	if (Dragging)
	  {
	    /* a word is probably selected, copy it into clipboard */
	    FrameToView (frame, &document, &view);
	    if (document && view)
	      DoCopyToClipboard (document, view, FALSE, TRUE);
	  }
#endif /* _WINDOWS */
      }
      break;
    
    case THOT_MIDDLE_BUTTON:
      {
        /* handle a simple selection */
        ClickFrame = frame;
        ClickX = x;
        ClickY = y;
        LocateSelectionInView (frame, ClickX, ClickY, 5, &Dragging);
      }
      break;
    
    case THOT_RIGHT_BUTTON:
      {
        /* handle a simple selection */
        ClickFrame = frame;
        ClickX = x;
        ClickY = y;
        LocateSelectionInView (frame, ClickX, ClickY, 6, &Dragging);
      }
      break;
    }
  return TRUE;
}

/*----------------------------------------------------------------------
 * FrameMotionCallback (generic callback)
 * is called when the mouse moves
 * params:
 *   + int frame :
 *   + int thot_mod_mask : what key is currently pressed
 *   [THOT_MOD_CTRL || THOT_MOD_ALT || THOT_MOD_SHIFT]
 *   + int x,y : the mouse position in the frame
 * return:
 *   + TRUE : if the event must be forwarded to other widgets
 *   + FALSE : if the event is cought
 ----------------------------------------------------------------------*/
ThotBool FrameMotionCallback (int frame, int thot_mod_mask, int x, int y )
{
  if (Dragging)
    {      
      Document            doc;
      int                 view;
      ViewFrame          *pFrame;
      static int          Motion_y = 0;
      static int          Motion_x = 0;
  
      if ( x != Motion_x || y != Motion_y )
        {
          Motion_y = y;
          Motion_x = x;
          FrameToView (frame, &doc, &view);
          pFrame = &ViewFrameTable[frame - 1];
          /* generate a scroll if necessary */
          if (Motion_y > FrameTable[frame].FrHeight)
            {
#ifndef _GL
              if (pFrame->FrAbstractBox &&
                  pFrame->FrAbstractBox->AbBox &&
                  pFrame->FrYOrg + FrameTable[frame].FrHeight < pFrame->FrAbstractBox->AbBox->BxHeight)
                TtcLineDown (doc, view);
#else /* _GL */
              if (pFrame->FrAbstractBox &&
                  pFrame->FrAbstractBox->AbBox &&
                  pFrame->FrYOrg + FrameTable[frame].FrHeight < pFrame->FrAbstractBox->AbBox->BxClipH)
                TtcLineDown (doc, view);
#endif /* _GL */
              else
                {
                  /* stop the scrolling */
                  //Dragging = FALSE;
                  Motion_y = FrameTable[frame].FrHeight;
                }
            }
          else if (Dragging && Motion_y < 0)
            {
              if (pFrame->FrYOrg > 0)
                TtcLineUp (doc, view);
              else
                {
                  /* stop the scrolling */
                  //Dragging = FALSE;
                  Motion_y = 0;
                }
            }

          if (FrameTable[frame].FrScrollWidth > FrameTable[frame].FrWidth)
            {
              if (Motion_x > FrameTable[frame].FrWidth)
                {
                  if (pFrame->FrXOrg + FrameTable[frame].FrWidth < FrameTable[frame].FrScrollWidth)
                    TtcScrollRight (doc, view);
                  else
                    {
                      //Dragging = FALSE;
                      Motion_x = FrameTable[frame].FrWidth;
                    }
                }
              else if (Motion_x < 1)
                {
                  if (pFrame->FrXOrg > 0)
                    TtcScrollLeft (doc, view);
                  else
                    {
                      //Dragging = FALSE;
                      Motion_x = 0;
                    }
                }
            }
          if (Dragging)
            LocateSelectionInView (frame,  Motion_x, Motion_y, 0, &Dragging);
        }
    }
  return TRUE;
}

/*----------------------------------------------------------------------
 * FrameMouseWheelCallback (generic callback)
 * is called when the mouse wheel is activated
 * params:
 *   + int frame :
 *   + int thot_mod_mask : what key is currently pressed
 *   [THOT_MOD_CTRL || THOT_MOD_ALT || THOT_MOD_SHIFT]
 *   + int x,y : the mouse position in the frame
 *   + int direction : >0 if wheel up <0 if wheel down
 *   + int delta : wheel delta, normally 120.
 * return:
 *   + TRUE : if the event must be forwarded to other widgets
 *   + FALSE : if the event is cought
 ----------------------------------------------------------------------*/
ThotBool FrameMouseWheelCallback( int frame, int thot_mod_mask,
                                 int direction, int delta,
                                 int x, int y )
{
  Document   document;
  View       view;

  if (direction > 0)
    {
      /* wheel mice up*/
      FrameToView (frame, &document, &view);
      if (thot_mod_mask & THOT_MOD_CTRL)
        /* if CTRL is down then zoom */
        ZoomOut (document, view);	   
      else if (thot_mod_mask & THOT_MOD_SHIFT)
        HorizontalScroll (frame, -39, 1);
      else
        VerticalScroll (frame, -39, 1);
    }
  else
    {
      /* wheel mice down */
      FrameToView (frame, &document, &view); 
      if (thot_mod_mask & THOT_MOD_CTRL)
        /* if CTRL is down then zoom */
        ZoomIn (document, view);
      else if (thot_mod_mask & THOT_MOD_SHIFT)
        HorizontalScroll (frame, 39, 1);          
      else
        VerticalScroll (frame, 39, 1);
    }
  return TRUE;
}

/*----------------------------------------------------------------------
  TtaGetThotWindow recupere le numero de la fenetre.           
  ----------------------------------------------------------------------*/
ThotWindow TtaGetThotWindow (int frame)
{
  return (ThotWindow)FrameTable[frame].WdFrame;
}

/*----------------------------------------------------------------------
  SetCursorWatch affiche le curseur "montre".                  
  ----------------------------------------------------------------------*/
void SetCursorWatch (int thotThotWindowid)
{
//  ::wxBeginBusyCursor();
}

/*----------------------------------------------------------------------
  ResetCursorWatch enleve le curseur "montre".                 
  ----------------------------------------------------------------------*/
void ResetCursorWatch (int thotThotWindowid)
{
//  ::wxEndBusyCursor();
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void TtaSetCursorWatch (Document document, View view)
{
#ifdef TODO
  int                 frame;
  UserErrorCode = 0;
  /* verifie le parametre document */
  if (document == 0 && view == 0)
    {
      for (frame = 1; frame <= MAX_FRAME; frame++)
        {
          /* TODO
          if(FrameTable[frame].WdFrame)
            FrameTable[frame].WdFrame->SetCursor(WaitCursor);
           */
        }
    }
  else
    {
      frame = GetWindowNumber (document, view);
      if(frame !=0 && FrameTable[frame].WdFrame)
        {
          /* TODO
           FrameTable[frame].WdFrame->SetCursor(WaitCursor);
           */
        }
    }
#endif /* TODO */
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void TtaResetCursor (Document document, View view)
{
#ifdef TODO
  int                 frame;
  UserErrorCode = 0;
  /* verifie le parametre document */
  if (document == 0 && view == 0)
    {
      for (frame = 1; frame <= MAX_FRAME; frame++)
        {
          /* TODO
          if(FrameTable[frame].WdFrame)
            FrameTable[frame].WdFrame->SetCursor(NormalCursor);
           */
        }
    }
  else
    {
      frame = GetWindowNumber (document, view);
      if(frame !=0 && FrameTable[frame].WdFrame)
        {
          /* TODO
           FrameTable[frame].WdFrame->SetCursor(NormalCursor);
           */
        }
    }
#endif /* TODO */
}

/*----------------------------------------------------------------------
  GiveClickedAbsBox returns the window and the abstract box selected.
  ----------------------------------------------------------------------*/
void GiveClickedAbsBox (int *frame, PtrAbstractBox *pAb)
{
  ThotEvent           event;
  int                 i;
  ThotFrame           w;

  if (ClickIsDone == 1)
    {
      *frame = 0;
      *pAb = NULL;
    }

  /* change the cursor */
  for (i = 1; i <= MAX_FRAME; i++)
    {
      w = FrameTable[i].WdFrame;
      if (w)
        w->GetCanvas()->SetCursor( wxCursor(wxCURSOR_CROSS) );
    }

  /* wait the click on the target */
  ClickIsDone = 1;
  ClickFrame = 0;
  ClickX = 0;
  ClickY = 0;
  while (ClickIsDone == 1)
    {
      TtaHandleOneEvent (&event);
    }

  /* restore the cursor */
  for (i = 1; i <= MAX_FRAME; i++)
    {
      w = FrameTable[i].WdFrame;
      if (w)
        w->GetCanvas()->SetCursor( wxNullCursor );
    }

  *frame = ClickFrame;
  if (ClickFrame > 0 && ClickFrame <= MAX_FRAME)
    *pAb = GetClickedAbsBox (ClickFrame, ClickX, ClickY);
  else
    *pAb = NULL;
}

/*----------------------------------------------------------------------
  Change the window title of the frame            
  ----------------------------------------------------------------------*/
void ChangeFrameTitle (int frame, unsigned char *text, CHARSET encoding)
{
  unsigned char      *title = NULL;
  AmayaFrame         *p_frame;

  /* Disabled for the moment, impossible to Save As otherwise */
  /*
    wxASSERT_MSG( encoding == UTF_8, _T("Encoding should be UTF8 !") );
  */
  title = text;

  p_frame = FrameTable[frame].WdFrame;
  if ( p_frame )
    p_frame->SetFrameTitle( TtaConvMessageToWX( (char *)title ) );

  if (title != text)
    TtaFreeMemory (title);
}

/*----------------------------------------------------------------------
  La frame d'indice frame devient la fenetre active.               
  ----------------------------------------------------------------------*/
void ChangeSelFrame (int frame)
{
  Document            doc, olddoc;
  PtrDocument         docsel;
  PtrElement          first, last;
  View                view;
  //AmayaFrame         *p_frame;
  int                 oldframe, firstChar, lastChar;

  if (ActiveFrame != frame)
    {
      CloseTextInsertion ();
      oldframe = ActiveFrame;
      FrameToView (oldframe, &olddoc, &view);
      ActiveFrame = frame;
      FrameToView (frame, &doc, &view);
      // set the new focus
      if (ChangeFocusFunction &&
          doc && LoadedDocument[doc-1]->DocTypeName &&
          strcmp (LoadedDocument[doc-1]->DocTypeName, "log") &&
          olddoc && LoadedDocument[olddoc-1]->DocTypeName &&
          strcmp (LoadedDocument[olddoc-1]->DocTypeName, "log"))
        (*(Proc1)ChangeFocusFunction) ((void *) doc);

      /* update the class list */
      TtaExecuteMenuAction ("ApplyClass", doc, 1, FALSE);
      TtaRefreshElementMenu (doc, 1);
      UpdateAttrMenu (LoadedDocument[doc-1], TRUE);
      // update the show errors
      TtaExecuteMenuAction ("UpdateShowError", doc, 1, FALSE);
      TtaExecuteMenuAction ("UpdateStyleList", doc, 1, FALSE);
      GetCurrentSelection (&docsel, &first, &last, &firstChar, &lastChar);
      if (LoadedDocument[doc-1] != docsel)
        TtaSetStatusSelectedElement (doc, 1, NULL);
      else
        TtaSetStatusSelectedElement (doc, 1, (Element)first);
      /* the active frame changed so update the application focus */
      /*p_frame = TtaGetFrameFromId(frame);
      if (p_frame)
      p_frame->GetCanvas()->SetFocus();*/
    }
}

/*----------------------------------------------------------------------
  TtaSetFocusChange registers the function to be called when the document
  focus changes:
  void procedure (Docucment doc)
  ----------------------------------------------------------------------*/
void TtaSetFocusChange (Proc1 procedure)
{
  ChangeFocusFunction = procedure;
}

/*----------------------------------------------------------------------
  GetWindowFrame retourne l'indice de la table des Cadres associe'    
  a` la fenetre w.                                        
  ----------------------------------------------------------------------*/
int GetWindowFrame (ThotWindow w)
{
  int                 f;

  /* On recherche l'indice de la fenetre */
  for (f = 0; f <= MAX_FRAME; f++)
    {
      if (FrameTable[f].WdFrame != 0 && (ThotWindow)FrameTable[f].WdFrame == w)
        break;
    }
  return (f);
}

/*----------------------------------------------------------------------
  GetSizesFrame retourne les dimensions de la fenetre d'indice frame.        
  ----------------------------------------------------------------------*/
void GetSizesFrame (int frame, int *width, int *height)
{
  *width = FrameTable[frame].FrWidth;
  *height = FrameTable[frame].FrHeight;
}

/*----------------------------------------------------------------------
  DefineClipping defines the window area to be redisplayed and updates
  the values in the concrete image.
  ----------------------------------------------------------------------*/
void  DefineClipping (int frame, int orgx, int orgy, int *xd, int *yd,
                      int *xf, int *yf, int raz)
{
  int              clipx, clipy, clipwidth, clipheight;

  if (*xd < *xf && *yd < *yf && orgx < *xf && orgy < *yf) 
    {
      /* compute the clipping area in the window */
      clipx = *xd - orgx;
      if (clipx < 0)
        {
          *xd -= clipx;
          clipx = 0;
        }
      clipy = *yd - orgy;
      if (clipy < 0)
        {
          *yd -= clipy;
          clipy = 0;
        }

      clipwidth = FrameTable[frame].FrWidth + orgx;
      if (*xf > clipwidth)
        *xf = clipwidth;
      clipheight = FrameTable[frame].FrHeight + orgy;
      if (*yf > clipheight)
        *yf = clipheight;
      clipwidth = *xf - *xd;
      clipheight = *yf - *yd;
      clipy += FrameTable[frame].FrTopMargin;

#ifndef _GL
      if (raz > 0)
        Clear (frame, clipwidth, clipheight, clipx, clipy);
#else /* _GL */
      GL_SetClipping (clipx,
                      FrameTable[frame].FrHeight + FrameTable[frame].FrTopMargin
                      - (clipy + clipheight),
                      clipwidth,
                      clipheight); 
      if (raz > 0 && GL_prepare (frame))
        {
#ifdef _GL_COLOR_DEBUG
          float tmp[4];
          glGetFloatv( GL_COLOR_CLEAR_VALUE, tmp );
          TTALOGDEBUG_5( TTA_LOG_DRAW, _T("glClear CLEAR_VALUE(%f,%f,%f,%f) - frame=%d"),tmp[0],tmp[1],tmp[2],tmp[3], frame);
#endif /* _GL_COLOR_DEBUG */
          glClear( GL_COLOR_BUFFER_BIT );
        }
#endif /*_GL*/
    }
}

/*----------------------------------------------------------------------
  RemoveClipping annule le rectangle de clipping de la fenetre frame.  
  ----------------------------------------------------------------------*/
void RemoveClipping (int frame)
{
#ifdef _GL
  GL_UnsetClipping ();
#endif /*_GL*/
}


/*----------------------------------------------------------------------
  UpdateScrollbars met a jour les bandes de defilement de la fenetre    
  ----------------------------------------------------------------------*/
void UpdateScrollbars (int frame)
{
  int                 x, y;
  int                 width, height;
  int                 l, h;
  ThotScrollBar       hscroll, vscroll;

  /* prevent infinite loop (hide/show) when scrollbars are regenerated by a resize event */
  if (!ComputeScrollBar)
    return;

  if (FrameUpdating ||
      documentDisplayMode[FrameTable[frame].FrDoc - 1] == NoComputedDisplay)
    return;
#ifdef _GL
  if (FrameTable[frame].Scroll_enabled == FALSE || FrameTable[frame].WdFrame == NULL)
    return;
#endif /* _GL */
  
  /* Get the displayed volume */
  ComputeDisplayedChars (frame, &x, &y, &width, &height);
  hscroll = FrameTable[frame].WdScrollH;
  vscroll = FrameTable[frame].WdScrollV;
  if (hscroll == NULL || vscroll == NULL)
    return;

  l = FrameTable[frame].FrWidth;
  h = FrameTable[frame].FrHeight;
  /*
    virtual void SetScrollbar(int position, int thumbSize, int range, int pageSize, const bool refresh = true)
    Sets the scrollbar properties.
    Parameters
    position : The position of the scrollbar in scroll units.
    thumbSize : The size of the thumb, or visible portion of the scrollbar, in scroll units.
    range : The maximum position of the scrollbar.
    pageSize : The size of the page size in scroll units. This is the number of units the scrollbar will scroll when it is paged up or down. Often it is the same as the thumb size.
    refresh : true to redraw the scrollbar, false otherwise.
  */
  if (width < l || x > 0)
    {
      FrameTable[frame].WdFrame->ShowScrollbar(2);
      FrameTable[frame].WdScrollH->SetScrollbar( x, width, l, width-13 );
    }
  else
    FrameTable[frame].WdFrame->HideScrollbar(2);    

  if (height < h || y > 0)
    {
      /* slider smaller than the scroll window */
      FrameTable[frame].WdFrame->ShowScrollbar(1);
      FrameTable[frame].WdScrollV->SetScrollbar( y, height, h, height );
    }
  else
    FrameTable[frame].WdFrame->HideScrollbar(1);
}

/*----------------------------------------------------------------------
  This function returns the current appversion comming from configure
  (if was :
  #ifdef _WX
  #define HTAppVersion  "9.0.1"
  #else
  #define HTAppVersion  "8.7.2"
  #endif
  )
  ----------------------------------------------------------------------*/
const char * TtaGetAppVersion()
{
  if (!TtAppVersion_IsInit)
    {
      if (APP_MINVER == 0 && APP_SUBVER == 0)
        sprintf (TtAppVersion, "%d", APP_MAJVER);
      else if (APP_SUBVER == 0)
        sprintf (TtAppVersion, "%d.%d", APP_MAJVER, APP_MINVER);
      else
        sprintf (TtAppVersion, "%d.%d.%d", APP_MAJVER, APP_MINVER, APP_SUBVER);
      TtAppVersion_IsInit = TRUE;
    }
  return TtAppVersion;
}

/*----------------------------------------------------------------------
  This function returns the app name
  ( if was : #define HTAppName     "amaya" )
  ----------------------------------------------------------------------*/
const char * TtaGetAppName()
{
  return "amaya";
}

/*----------------------------------------------------------------------
  This function returns the app date
  (it was : #define HTAppDate     __DATE__ )
  ----------------------------------------------------------------------*/
const char * TtaGetAppDate()
{
  return __DATE__;
}

/*----------------------------------------------------------------------
  This function returns the app year
  ----------------------------------------------------------------------*/
const char * TtaGetAppYear()
{
  static char date[] = __DATE__;
  char* s = date;
  
  s = strchr(s, ' ');
  s = strchr(s+1, ' ');
  
  return s+1;
}
