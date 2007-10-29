/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2007
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

#ifdef _WX
#include "wx/wx.h"
#endif /* _WX */

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

#ifdef _GTK
#include <gdk/gdkx.h>
#endif /*_GTK*/
#ifdef _GL
#if defined (_MACOS) && defined (_WX) 
#include <gl.h>
#else /* _MACOS  && _WX */
#include <GL/gl.h>
#endif /* _MACOS */
#include "glwindowdisplay.h"
#ifdef _GTK
#include <gtkgl/gtkglarea.h>
#endif /*_GTK*/
#endif /*_GL*/
#ifdef _WINGUI
#include "winsys.h"
#include "wininclude.h"

#ifdef WM_MOUSELAST
#ifndef WM_MOUSEWHEEL
#define WM_MOUSEWHEEL WM_MOUSELAST+1 
#endif /*WM_MOUSEWHEEL*/
#endif /*WM_MOUSELAST*/
#endif /* _WINGUI */
#ifdef _GTK
static gchar *null_string;
#endif /*_GTK*/
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
#include "attrmenu.h"

#ifdef _WX
#include "AmayaWindow.h"
#include "AmayaFrame.h"
#include "AmayaCanvas.h"
#include "appdialogue_wx.h"
#include "message_wx.h"
#include "AmayaScrollBar.h"
#include "AmayaStatusBar.h"
#endif /* _WX */
#ifdef _GTK
#include "gtk-functions.h"
#endif /* _GTK */

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
#include "keyboards_f.h"
#include "memory_f.h"
#include "message_f.h"
#include "picture_f.h"
#include "scroll_f.h"
#include "selectmenu_f.h"
#include "structcommands_f.h"
#include "structcreation_f.h"
#include "structmodif_f.h"
#include "structselect_f.h"
#include "textcommands_f.h"
#include "thotmsg_f.h"
#include "uconvert_f.h"
#include "views_f.h"
#include "viewapi_f.h"
#include "xwindowdisplay_f.h"
#include "appdialogue_wx_f.h"

/* defined into amaya directory ...*/
extern void ZoomIn (Document document, View view);
extern void ZoomOut (Document document, View view);

#ifdef _GL
// This flag is used to recalculate the glcanvas after a RESIZE event
// because GTK&GL clear automaticaly the GL canvas just after the frame is resized.
// (it appends only on some hardware opengl implementations on Linux)
static ThotBool g_NeedRedisplayAllTheFrame[MAX_FRAME];
#endif /* _GL */

#ifdef _WINGUI
#define URL_TXTZONE     0
#define TITLE_TXTZONE   1
#define IDR_TOOLBAR    165
#define MENU_VIEWS            551
#define SHOW_STRUCTURE        553
#define SHOW_ALTERNATE        554
#define SHOW_LINKS            555
#define SHOW_TAB_OF_CONTENTS  556
#define CLOSE_STRUCTURE       232
#define CLOSE_ALTERNATE       272
#define CLOSE_LINKS           312
#define CLOSE_TAB_OF_CONTENTS 352
#define WM_ENTER (WM_USER)
#define MAX_MENUS 5

#define ToolBar_AutoSize(hwnd) \
    (void)SendMessage((hwnd), TB_AUTOSIZE, 0, 0L)

#define ToolBar_GetItemRect(hwnd, idButton, lprc) \
    (BOOL)SendMessage((hwnd), TB_GETITEMRECT, (WPARAM)idButton, (LPARAM)(LPRECT)lprc)

#define ToolBar_GetToolTips(hwnd) \
    (HWND)SendMessage((hwnd), TB_GETTOOLTIPS, 0, 0L)

#define ToolBar_ButtonStructSize(hwnd) \
    (void)SendMessage((hwnd), TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0L)

#define ToolBar_AddBitmap(hwnd, nButtons, lptbab) \
    (int)SendMessage((hwnd), TB_ADDBITMAP, (WPARAM)nButtons, (LPARAM)(LPTBADDBITMAP) lptbab)

#define ToolBar_AddString(hwnd, hinst, idString) \
    (int)SendMessage((hwnd), TB_ADDSTRING, (WPARAM)(HINSTANCE)hinst, (LPARAM)idString)

#define ToolTip_AddTool(hwnd, lpti) \
    (BOOL)SendMessage((hwnd), TTM_ADDTOOL, 0, (LPARAM) (LPTOOLINFO) lpti)

extern HWND      hwndClient;
extern HWND      ToolBar;
extern HWND      logoFrame;
extern HWND      StatusBar;
extern HWND      WIN_curWin;
extern HWND      currentDlg;
extern int       ReturnOption;
extern int       Window_Curs;

static HWND      hwndHead;
static char     *txtZoneLabel;
static char      URL_txt [500];
static ThotBool  paletteRealized = FALSE;

int         X_Pos;
int         Y_Pos;
int         cyToolBar;
TBADDBITMAP ThotTBBitmap;
static ThotWindow WIN_curWin = NULL;

static int FRWidth[MAX_FRAME];
static int FRHeight[MAX_FRAME];

/*----------------------------------------------------------------------
  Win_Scroll_visible : Tells if a scrollbar is currently visible or not
  ----------------------------------------------------------------------*/
static ThotBool Win_Scroll_visible (HWND scroll_hwnd)
{
  SCROLLINFO          scrollInfo;

  memset(&scrollInfo, 0, sizeof(scrollInfo));
  scrollInfo.cbSize = sizeof(scrollInfo);
  scrollInfo.fMask = SIF_RANGE;

  GetScrollInfo(scroll_hwnd, SB_CTL, &scrollInfo);

  if (scrollInfo.nMin == 0 && scrollInfo.nMax == 2) 
    return FALSE;
  else
    return TRUE;
}
/*----------------------------------------------------------------------
  WIN_GetDeviceContext selects a Device Context for a given
  thot window.
  ----------------------------------------------------------------------*/
void WIN_GetDeviceContext (int frame)
{
#ifndef _GLPRINT
  if (frame < 0 || frame > MAX_FRAME)
    {
      if (TtDisplay)
        return;
      TtDisplay = GetDC (WIN_curWin);
      return;
    }
  if (FrRef[frame])
    {
      /* release the previous Device Context. */
      if (TtDisplay)
        WIN_ReleaseDeviceContext ();
      /* load the new Context. */
      TtDisplay = GetDC (FrRef[frame]);
      if (TtDisplay != NULL)
        {
          WIN_curWin = FrRef[frame];
          SetICMMode (TtDisplay, ICM_ON);
        }
    }
#endif /*_GLPRINT*/
}

/*----------------------------------------------------------------------
  WIN_ReleaseDeviceContext :  unselect the Device Context
  ----------------------------------------------------------------------*/
void WIN_ReleaseDeviceContext (void)
{
#ifndef _GLPRINT
  /* release the previous Device Context. */
  if (TtDisplay != NULL)
    {     
      SetICMMode (TtDisplay, ICM_OFF);
      ReleaseDC (WIN_curWin, TtDisplay);
    }
  TtDisplay = NULL;
#endif /*_GLPRINT*/
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
ThotBool RegisterWin95 (CONST WNDCLASS* lpwc)
{
  WNDCLASSEX wcex;

  wcex.style = lpwc->style;
  wcex.lpfnWndProc = lpwc->lpfnWndProc;
  wcex.cbClsExtra = lpwc->cbClsExtra;
  wcex.cbWndExtra = lpwc->cbWndExtra;
  wcex.hInstance = lpwc->hInstance;
  wcex.hIcon = lpwc->hIcon;
  wcex.hCursor = lpwc->hCursor;
  wcex.hbrBackground = lpwc->hbrBackground;
  wcex.lpszMenuName = lpwc->lpszMenuName;
  wcex.lpszClassName = lpwc->lpszClassName;
  /* Added elements for Windows 95. */
  wcex.cbSize = sizeof(WNDCLASSEX);
  wcex.hIconSm = LoadIcon (hInstance, IDI_APPLICATION);
  return RegisterClassEx( &wcex );
}

#ifndef _WIN_PRINT
#ifndef _GL
/*----------------------------------------------------------------------
  Clear clear the area of frame located at (x, y) and of size width x height.
  ----------------------------------------------------------------------*/
void Clear (int frame, int width, int height, int x, int y)
{
  ThotWindow          w;
  HBRUSH              hBrush;
  HBRUSH              hOldBrush;

  w = FrRef[frame];
  if (w != None)
    {
      hBrush = CreateSolidBrush (ColorPixel (BackgroundColor[frame]));
      hOldBrush = SelectObject (TtDisplay, hBrush);
      PatBlt (TtDisplay, x, y + FrameTable[frame].FrTopMargin, width, height, PATCOPY);
      SelectObject (TtDisplay, hOldBrush);
      DeleteObject (hBrush);
    }
}

/*----------------------------------------------------------------------
  Scroll do a scrolling/Bitblt of frame of a width x height area
  from (xd,yd) to (xf,yf).
  ----------------------------------------------------------------------*/
void Scroll (int frame, int width, int height, int xd, int yd, int xf, int yf)
{
  RECT cltRect;

  if (FrRef[frame])
    {
      GetClientRect (FrRef[frame], &cltRect);
      ScrollWindowEx (FrRef[frame], xf - xd, yf - yd, NULL, &cltRect,
                      NULL, NULL, SW_INVALIDATE);
    }
}
#endif /*_GL*/
#endif /* _WIN_PRINT */

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
ThotBool InitToolTip (HWND hwndToolBar)
{
  BOOL     bSuccess;
  HWND     hwndTT;
  TOOLINFO ti;

  /* Fetch handle to tooltip control */
  hwndTT = ToolBar_GetToolTips (hwndToolBar);
  if (hwndTT == NULL) 
    return FALSE;

  ZeroMemory (&ti, sizeof (TOOLINFO));
  ti.cbSize   = sizeof (TOOLINFO);
  ti.uFlags   = TTF_IDISHWND | TTF_CENTERTIP | TTF_SUBCLASS;
  ti.hwnd     = hwndToolBar;
  ti.uId      = 0;
  ti.lpszText = LPSTR_TEXTCALLBACK;
  bSuccess = ToolTip_AddTool (hwndTT, &ti);
  if (!bSuccess)
    return FALSE;

  return bSuccess;
}
#endif /* _WINGUI */

/*----------------------------------------------------------------------
  GetFrameNumber returns the Thot window number associated.
  ----------------------------------------------------------------------*/
int GetFrameNumber (ThotWindow win)
{
  int frame;

#ifndef _WX
  for (frame = 1; frame <= MAX_FRAME; frame++)
    if (FrRef[frame] == win)
      return (frame);
#else /* _WX */
  for (frame = 1; frame <= MAX_FRAME; frame++)
    if ((ThotWindow)FrameTable[frame].WdFrame == win)
      return (frame);  
#endif /* _WX */
  
  return (-1);
}


/*----------------------------------------------------------------------
  Evenement sur une frame document.                             
  ----------------------------------------------------------------------*/
void FrameKilled (int *w, int frame, int *info)

{
  /* Enleve la procedure de Callback */
  /* Detruit la fenetre si elle existe encore */
#ifndef _WX
  if (frame > 0 && FrRef[frame] != 0)
    ViewClosed (frame);
#else /* _WX */
  if (frame > 0 && FrameTable[frame].WdFrame != 0)
    ViewClosed (frame);
#endif /* _WX */
}


/*----------------------------------------------------------------------
  Called when a click is done on the up right corner cross
  Kill the current document ( GTK version )                            
  ----------------------------------------------------------------------*/
#ifdef _GTK
gboolean KillFrameGTK (GtkWidget *widget, GdkEvent *event, gpointer f)
{
  return KillFrameCallback( (intptr_t)f );
}
#endif /* _GTK */

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

#ifndef _WX  
  for (frame = 0; frame <= MAX_FRAME; frame++)
    if (FrRef[frame] != 0)
      /* there is still an active frame */
      return TRUE;
#else /* #ifndef _WX */
  for (frame = 0; frame <= MAX_FRAME; frame++)
    if (FrameTable[frame].WdFrame != 0)
      /* there is still an active frame */
      return TRUE;
#endif /* #ifndef _WX */

#ifndef _WX
  /* this is done into AmayaApp::OnExit */
  TtaQuit();
#endif /* _WX */
  return FALSE;
}



/*----------------------------------------------------------------------
  FrameRedraw
  ----------------------------------------------------------------------*/
void FrameRedraw (int frame, unsigned int width, unsigned int height)
{
#if defined(_GTK) || defined(_WX)
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
#endif /* #if defined(_GTK) || defined(_WX) */
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
#ifdef _WX
      if (i != GetSharedContext() && !TtaFrameIsClosed(i))
#endif /* _WX */
#if defined(_GTK)
        if (i != GetSharedContext() && FrameTable[i].WdFrame)
#endif /*#if defined(_GTK) */
#ifdef _WINGUI
          if (i != GetSharedContext() && GL_Context[i])
#endif /* _WINGUI */
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
#ifdef _WINGUI
  /* make our context 'un-'current */
  /*wglMakeCurrent (NULL, NULL);*/
  /* delete the rendering context */
  if (GL_Context[frame])
    wglDeleteContext (GL_Context[frame]);
  /*if (GL_Windows[frame])*/
  /*ReleaseDC (hwndClient, GL_Windows[frame]);*/
  GL_Windows[frame] = 0;
  GL_Context[frame] = 0;
#endif /* _WINGUI */
}

#endif /* _GL */

#ifdef _GL
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

  pFrame = &ViewFrameTable[frame - 1];
#ifdef _GL
  /* THIS JUST DOESN'T WORK !!!
     even when storing successive x,y and so on...
     it's just gtk and opengl mix bad...
     so the Xfree and gtk guys that tells us 
     it work, just have to come here and code it here
     with an hardware opengl implementation on their PC...
     They will see the Speed problem...*/
  /*if (event->count > 0)*/
  /*    return TRUE; */
  //if (GL_prepare (frame))
    {
      /* prevent flickering*/
      GL_SwapStop (frame);
      if ( g_NeedRedisplayAllTheFrame[frame] || glhard() || GetBadCard() )
        {
          // we need to recalculate the glcanvas only once : after the RESIZE event
          // because GTK&GL clear automaticaly the GL canvas just after the frame is resized.
          // (it appends only on some hardware opengl implementations on Linux)
          g_NeedRedisplayAllTheFrame[frame] = FALSE;
          
          // refresh the invalide frame content
          x += pFrame->FrXOrg;
          y += pFrame->FrYOrg;
          DefClip (frame, x, y, x + w, y + h);
          RedrawFrameBottom (frame, 0, NULL);
        }
   
      // display the backbuffer
      GL_SwapEnable (frame);
      GL_Swap (frame);
    }
#else /* _GL */
  x += pFrame->FrXOrg;
  y += pFrame->FrYOrg;
  DefClip (frame, x, y, x + w, y + h);
  RedrawFrameBottom (frame, 0, NULL);
#endif /* _GL */
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
  //if (GL_prepare( frame))
    {
      /* prevent flickering*/
      GL_SwapStop (frame);
      GLResize (new_width, new_height, 0, 0);
      DefClip (frame, -1, -1, -1, -1);
      FrameRedraw (frame, new_width, new_height);
      //GL_SwapEnable (frame);
      //GL_Swap (frame);

      //#if !defined(_MACOS) && !defined(_WINDOWS)
      // we need to recalculate the glcanvas after the RESIZE event
      // because GTK&GL clear automaticaly the GL canvas just after the frame is resized.
      // (it appends only on some hardware opengl implementations on Linux)
      g_NeedRedisplayAllTheFrame[frame] = TRUE;
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

  if (document != 0)
    frame = GetWindowNumber (document, view);
  else
    frame = 0;
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

  if (document != 0)
    frame = GetWindowNumber (document, view);
  else
    frame = 0;
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
#ifdef _WX
    /* fill with 0 all the fields */
    memset (&FrameTable[i], 0, sizeof(Frame_Ctl));
#else /* _WX */
  {
    FrameTable[i].WdFrame = 0;
    FrameTable[i].FrDoc = 0;
  }
#endif /* _WX */      

#ifdef _WX
  memset( WindowTable, 0, sizeof(Window_Ctl)*(MAX_WINDOW+1) );
#endif /* _WX */
  ClickIsDone = 0;
  ClickFrame = 0;
  ClickX = 0;
  ClickY = 0;
  /* message de selection vide */
#ifdef _GTK
  null_string = (gchar *)"";
#endif /* _GTK */
  OldMsgSelect[0] = EOS;
  OldDocMsgSelect = NULL;
}

/*----------------------------------------------------------------------
  Map and raise the corresponding window.
  ----------------------------------------------------------------------*/
void TtaRaiseView (Document document, View view)
{
  int                 frame_id;
  ThotWidget          w;

  UserErrorCode = 0;
  frame_id = GetWindowNumber (document, view);
  if (frame_id != 0)
    {
      w = FrameTable[frame_id].WdFrame;
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
      
      TtaAttachFrame( frame_id,
                      window_id,
                      page_id,
                      page_position );
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
void TtaSetStatus (Document document, View view, char *text, char *name)
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
          /* 
           * do not use the FrameTable[frame].WdStatus field because it's simplier
           * to update only the frame's parent window
           */
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
#ifdef _WX
  AmayaWindow       *window;
  AmayaStatusBar    *statusbar;
  PtrElement         pEl = (PtrElement)elem;
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
            statusbar->SetSelectedElement (elem);
          if (pEl && pEl->ElAbstractBox[0])
            {
#ifdef IV
              // Update the current color in the tool panel
              AmayaToolPanel* panel = window->GetPanel (WXAMAYA_PANEL_STYLE);
              if (panel)
                panel->SetColor (pEl->ElAbstractBox[0]->AbForeground);
#endif
            }
        }
    }
#endif /* _WX */
}



/*----------------------------------------------------------------------
  GtkLiningSelection 

  When user hold clicked a mouse button in order to extend a selection
  and scroll in the meantime,
  those functions are called by a timer each 100 ms
  in order to repeat user action until he released the button
  or move away from the widget.
  ----------------------------------------------------------------------*/
static ThotBool     Selecting = FALSE;


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
#ifdef _WX
#if !defined (_WINDOWS) && !defined (_MACOS)
  Document   document;
  View       view;
#endif /* !_WINDOWS && ! _MACOS */
  
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
        /* Est-ce que la touche modifieur de geometrie est active ? */
        if ((thot_mod_mask & THOT_MOD_CTRL) == THOT_MOD_CTRL)
          {
            /* moving a box */     
            ApplyDirectTranslate (frame, x, y);
          }
        else if ((thot_mod_mask & THOT_MOD_SHIFT) == THOT_MOD_SHIFT)
          {
            /* a selection extension */
            TtaAbortShowDialogue ();
            LocateSelectionInView (frame, x, y, 1);
#if !defined (_WINDOWS) && !defined (_MACOS)
            FrameToView (frame, &document, &view);
            DoCopyToClipboard (document, view, FALSE, TRUE);
#endif /* _WINDOWS */
          }
        else
          {
            /* a simple selection */
            ClickFrame = frame;
            ClickX = x;
            ClickY = y;
            /* it's important to setup Selecting before the call of LocateSelectionInView 
             * because LocateSelectionInView will handle gui events (keyup) and Selecting variable
             * will not be unset => cause a infinit selection ! */
            Selecting = TRUE;
            LocateSelectionInView (frame, ClickX, ClickY, 2);
          }
      }
      break;
      
    case THOT_MIDDLE_BUTTON:
      {
        if (thot_mod_mask & THOT_MOD_CTRL)
          {
            /* resizing a box */
            ApplyDirectResize (frame, x, y);
          }
        else
          {
            ClickFrame = frame;
            ClickX = x;
            ClickY = y;
            LocateSelectionInView (frame, ClickX, ClickY, 5);	     
          }
      }
      break;
      
    case THOT_RIGHT_BUTTON:
      {
        if (thot_mod_mask & THOT_MOD_CTRL)
          {
            /* resize a box */
            ApplyDirectResize (frame, x, y);
          }
        else
          {
            ClickFrame = frame;
            ClickX = x;
            ClickY = y;
            LocateSelectionInView (frame, ClickX, ClickY, 6);
          }
      }
      break;
    }
  
#endif /* _WX */
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
#ifdef _WX
#if !defined(_WINDOWS) && !defined(_MACOS)
  Document   document;
  View       view;
#endif /* !_WINDOWS && !_MACOS*/
  if (Selecting)
    {
      Selecting = FALSE;
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
      LocateSelectionInView (frame, ClickX, ClickY, 4);
      // SG: j'ai commente la ligne suivante car si le document est modifie 
      // et qu'on desire suivre un lien, un evenement keyup est generer
      // et tue le dialogue qui demande si on veut sauver.
      //    TtaAbortShowDialogue ();
    }
#endif /* _WX */
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
#ifdef _WX
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
        LocateSelectionInView (frame, ClickX, ClickY, 3);
#if !defined (_WINDOWS) && !defined (_MACOS)
        /* a word is probably selected, copy it into clipboard */
        FrameToView (frame, &document, &view);
        if (document && view)
          DoCopyToClipboard (document, view, FALSE, TRUE);
#endif /* _WINDOWS */
      }
      break;
    
    case THOT_MIDDLE_BUTTON:
      {
        /* handle a simple selection */
        ClickFrame = frame;
        ClickX = x;
        ClickY = y;
        LocateSelectionInView (frame, ClickX, ClickY, 5);
      }
      break;
    
    case THOT_RIGHT_BUTTON:
      {
        /* handle a simple selection */
        ClickFrame = frame;
        ClickX = x;
        ClickY = y;
        LocateSelectionInView (frame, ClickX, ClickY, 6);
      }
      break;
    }
#endif /* _WX */
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
#ifdef _WX
  if ( Selecting )
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
                  Selecting = FALSE;
                  Motion_y = FrameTable[frame].FrHeight;
                }
            }
          else if (Motion_y < 0)
            {
              if (pFrame->FrYOrg > 0)
                TtcLineUp (doc, view);
              else
                {
                  /* stop the scrolling */
                  Selecting = FALSE;
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
                      Selecting = FALSE;
                      Motion_x = FrameTable[frame].FrWidth;
                    }
                }
              else if (Motion_x < 1)
                {
                  if (pFrame->FrXOrg > 0)
                    TtcScrollLeft (doc, view);
                  else
                    {
                      Selecting = FALSE;
                      Motion_x = 0;
                    }
                }
            }
          if (Selecting)
            {
              LocateSelectionInView (frame,  Motion_x, Motion_y, 0);
            }
        }      
    }
#endif /* _WX */
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
ThotBool FrameMouseWheelCallback( 
                                 int frame,
                                 int thot_mod_mask,
                                 int direction,
                                 int delta,
                                 int x, int y )
{
#ifdef _WX
  Document   document;
  View       view;

  if (direction > 0)
    {
      /* wheel mice up*/
      FrameToView (frame, &document, &view);
      if (thot_mod_mask & THOT_MOD_CTRL)
        {
          /* if CTRL is down then zoom */
          ZoomOut (document, view);	   
        }
      else
        { 
          VerticalScroll (frame, -39, 1);
        }
    }
  else
    {
      /* wheel mice down */
      FrameToView (frame, &document, &view); 
      if (thot_mod_mask & THOT_MOD_CTRL)
        {
          /* if CTRL is down then zoom */
          ZoomIn (document, view);
        }
      else
        {
          VerticalScroll (frame, 39, 1);
        }
    }	
#endif /* _WX */
  return TRUE;
}

/*----------------------------------------------------------------------
  Evenement sur une frame document.                              
  D.V. equivalent de la fontion MS-Windows ci dessus !        
  GTK: fonction qui traite les click de la souris sauf la selection   
  ----------------------------------------------------------------------*/
#ifdef _GTK
gboolean FrameCallbackGTK (GtkWidget *widget, GdkEventButton *event,
                           gpointer data)
{
  intptr_t            frame;
  GtkEntry            *textzone;
  static int          timer = None;
  Document            document;
  View                view;

  frame = (intptr_t)data;
#ifdef _GL
  GL_prepare (frame);  
#endif /* _GL */
  frame = (intptr_t)data;
  if (FrameTable[frame].FrDoc == 0 ||
      documentDisplayMode[FrameTable[frame].FrDoc - 1] == NoComputedDisplay)
    /* don't manage a document with NoComputedDisplay mode */
    return FALSE;
  else if (event == NULL)
    /* a TtaWaitShowDialogue in progress, don't change the selection */
    return FALSE;
  else if (ClickIsDone == 1 && event->type == GDK_BUTTON_PRESS)
    /* Amaya is waiting for a click selection */
    {
      ClickIsDone = 0;
      ClickFrame = frame;
      ClickX = (int)event->x;
      ClickY = (int)event->y;
      return FALSE;
    }
  else if (TtaTestWaitShowDialogue ()
           && (event->type != GDK_BUTTON_PRESS || 
               event->type != GDK_2BUTTON_PRESS ||
               event->type != GDK_MOTION_NOTIFY || 
               (event->state & GDK_CONTROL_MASK ) == 0))
    {
      /* a TtaWaitShowDialogue in progress, don't change the selection */
      return FALSE;
    }
  
  /* 
     Set the drawing area Focused
     By setting the focus on the text zone
     If another action specifically need focus, 
     w'ell grab it with the action
  */
  textzone = (GtkEntry*)gtk_object_get_data (GTK_OBJECT (widget), "Text_catcher");
  gtk_widget_grab_focus (GTK_WIDGET(textzone));  

  switch (event->type)
    {
    case GDK_BUTTON_PRESS:
      /* drag is finished */
      if (timer != None)
        {
          gtk_timeout_remove (timer);
          timer = None;
          Selecting = FALSE;
        } 
      switch (event->button)
        {
          /* ==========LEFT BUTTON========== */
        case 1:
          /* stop any current insertion of text */
          CloseTextInsertion ();
          /* Est-ce que la touche modifieur de geometrie est active ? */
	
          if ((event->state & GDK_CONTROL_MASK ) == GDK_CONTROL_MASK)
            /* moving a box */     
            ApplyDirectTranslate (frame, (int)event->x, (int)event->y);
          else if ((event->state & GDK_SHIFT_MASK ) == GDK_SHIFT_MASK)
            {
              /* a selection extension */
              TtaAbortShowDialogue ();
              LocateSelectionInView (frame, (int)event->x, (int)event->y, 1);
              FrameToView (frame, &document, &view);
              DoCopyToClipboard (document, view, FALSE, TRUE);
            }
          else
            {
              /* a simple selection */
              ClickFrame = frame;
              ClickX = (int)event->x;
              ClickY = (int)event->y;
              LocateSelectionInView (frame, ClickX, ClickY, 2);
              Selecting = TRUE;
            }
          break;
        case 2:
          /* ==========MIDDLE BUTTON========== */
          if ((event->state & GDK_CONTROL_MASK) != 0)
            /* resizing a box */
            ApplyDirectResize (frame, (int)event->x, (int)event->y);
          else
            {
              ClickFrame = frame;
              ClickX = (int)event->x;
              ClickY = (int)event->y; 
              LocateSelectionInView (frame, ClickX, ClickY, 5);	     
            }
          break;
        case 3:
          /* ==========RIGHT BUTTON========== */
          if ((event->state & GDK_CONTROL_MASK) != 0)
            /* resize a box */
            ApplyDirectResize (frame, (int)event->x, (int)event->y);
          else
            {
              ClickFrame = frame;
              ClickX = (int)event->x;
              ClickY = (int)event->y;
              LocateSelectionInView (frame, ClickX, ClickY, 6);
            }
          break;	
        case 4:
          /* wheel mice up */
          FrameToView (frame, &document, &view);
          if ((event->state & GDK_CONTROL_MASK) != 0)
            {
              ;
#ifdef _ZOOMSCROLL
              ZoomIn (document, view);	   
#endif /* _ZOOMSCROLL    */
            }
          else
            { 
              VerticalScroll (frame, -39, 1);
            }	  
          break;
        case 5:
          /* wheel mice down */
          FrameToView (frame, &document, &view); 
          if ((event->state & GDK_CONTROL_MASK) != 0)
            {
              ;
#ifdef _ZOOMSCROLL
              ZoomOut (document, view);	   
#endif /* _ZOOMSCROLL    */      
            }
          else
            {
              VerticalScroll (frame, 39, 1);
            }
          break;
        default:
          break;
        }
      break;
    case GDK_2BUTTON_PRESS:
      /*======== DOUBLE CLICK ===========*/
      switch (event->button)
        {
        case 1:
          /* LEFT BUTTON */
          ClickFrame = frame;
          ClickX = (int)event->x;
          ClickY = (int)event->y;
          LocateSelectionInView (frame, ClickX, ClickY, 3);
          break;
        case 2:
          /* MIDDLE BUTTON */
          /* handle a simple selection */
          ClickFrame = frame;
          ClickX = (int)event->x;
          ClickY = (int)event->y;
          LocateSelectionInView (frame, ClickX, ClickY, 5);
          break;
        case 3:
          /* RIGHT BUTTON */
          /* handle a simple selection */
          ClickFrame = frame;
          ClickX = (int)event->x;
          ClickY = (int)event->y;
          LocateSelectionInView (frame, ClickX, ClickY, 6);
          break;
        }
      break;
    case GDK_MOTION_NOTIFY: 
      /* extend the current selection */
      if (Selecting == TRUE)
        {
          /* We add a callback timer caller */
          if (timer == None)
            timer = gtk_timeout_add (100,  GtkLiningSelection, 
                                     (gpointer) frame);
        }
      break;
    case GDK_BUTTON_RELEASE:
      /* if a button release, we save the selection in the clipboard */
      /* drag is finished */
      /* we stop the callback calling timer */
      Selecting = FALSE;
      if (timer != None)
        {
          gtk_timeout_remove (timer);
          timer = None;
          FrameToView (frame, &document, &view);
          DoCopyToClipboard (document, view, FALSE, TRUE);
        } 
      else if (event->button == 1)
        {
          ClickFrame = frame;
          ClickX = (int)event->x;
          ClickY = (int)event->y;
          LocateSelectionInView (frame, ClickX, ClickY, 4);
          TtaAbortShowDialogue ();
        }
      break;
    case GDK_KEY_PRESS: 
      TtaAbortShowDialogue ();
      CharTranslationGTK (widget, (GdkEventKey*)event, data);
      break;
    default:
      break;
    }
  return TRUE;
}

/*----------------------------------------------------------------------
  DragCallbackGTK handles drag and drop                     
  ----------------------------------------------------------------------*/
gboolean DragCallbackGTK (GtkWidget *widget, GdkDragContext *drag_context,
                          gint x, gint y, guint time, gpointer user_data)
{
  /* TODO */
  return FALSE;
}
  
/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
gboolean FocusInCallbackGTK (GtkWidget *widget, GdkEventFocus *event,
                             gpointer user_data)
{
  gtk_object_set_data (GTK_OBJECT(widget), "Active", (gpointer)TRUE);
  return FALSE;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
gboolean FocusOutCallbackGTK (GtkWidget *widget, GdkEventFocus *event,
                              gpointer user_data)
{
  gtk_object_set_data (GTK_OBJECT(widget), "Active", (gpointer)FALSE);
  return FALSE;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
gboolean ButtonPressCallbackGTK (GtkWidget *widget, GdkEventButton *event,
                                 gpointer user_data)
{
  gtk_object_set_data (GTK_OBJECT(widget), "ButtonPress", (gpointer)TRUE);    
  return FALSE;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
gboolean ButtonReleaseCallbackGTK (GtkWidget *widget, GdkEventButton *event,
                                   gpointer user_data)
{
  gtk_object_set_data (GTK_OBJECT(widget), "ButtonRelease", (gpointer)TRUE);    
  return FALSE;
}
#endif /*_GTK*/


/*----------------------------------------------------------------------
  ThotGrab fait un XGrabPointer.                                  
  ----------------------------------------------------------------------*/
void ThotGrab (ThotWindow win, ThotCursor cursor, long events, int disp)
{
#ifdef _GTK
  gdk_pointer_grab (win, FALSE, (GdkEventMask)events, win, cursor,
                    GDK_CURRENT_TIME);
#endif /* _GTK */
}


/*----------------------------------------------------------------------
  ThotUngrab est une fonction d'interface pour UnGrab.            
  ----------------------------------------------------------------------*/
void ThotUngrab ()
{
#ifdef _GTK
  gdk_pointer_ungrab (GDK_CURRENT_TIME);
#endif /* _GTK */
}

/*----------------------------------------------------------------------
  TtaGetThotWindow recupere le numero de la fenetre.           
  ----------------------------------------------------------------------*/
ThotWindow TtaGetThotWindow (int frame)
{
#ifndef _WX  
  return FrRef[frame];
#else /* _WX */
  return (ThotWindow)FrameTable[frame].WdFrame;
#endif /* _WX */
}

/*----------------------------------------------------------------------
  SetCursorWatch affiche le curseur "montre".                  
  ----------------------------------------------------------------------*/
void SetCursorWatch (int thotThotWindowid)
{
#ifdef _GTK
  Drawable            drawable;
  int                 frame;
  ThotWidget          w;
  
  drawable = (Drawable)TtaGetThotWindow (thotThotWindowid);
  for (frame = 1; frame <= MAX_FRAME; frame++)
    {
      w = FrameTable[frame].WdFrame;
      if (w  && w->window)
        gdk_window_set_cursor (GTK_WIDGET(w)->window, WaitCurs);
    }
#endif /* _GTK */

#ifdef _WINGUI
  SetCursor (LoadCursor (NULL, IDC_WAIT));
  ShowCursor (TRUE);
#endif /* #ifdef _WINGUI */  
}

/*----------------------------------------------------------------------
  ResetCursorWatch enleve le curseur "montre".                 
  ----------------------------------------------------------------------*/
void ResetCursorWatch (int thotThotWindowid)
{  
#ifdef _GTK
  Drawable            drawable;
  int                 frame;
  ThotWidget          w;

  drawable = (Drawable) TtaGetThotWindow (thotThotWindowid);
  for (frame = 1; frame <= MAX_FRAME; frame++)
    {
      w = FrameTable[frame].WdFrame;
      if (w && w->window)
        gdk_window_set_cursor (GTK_WIDGET(w)->window, ArrowCurs);
    }
#endif /* _GTK */

#ifdef _WINGUI
  ShowCursor (FALSE);
  SetCursor (LoadCursor (NULL, IDC_ARROW));
#endif /* _WINGUI */
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void TtaSetCursorWatch (Document document, View view)
{
  int                 frame;
#ifdef _GTK
  Drawable            drawable;
  ThotWidget          w;
#endif /* _GTK */

  UserErrorCode = 0;
  /* verifie le parametre document */
  if (document == 0 && view == 0)
    {
      for (frame = 1; frame <= MAX_FRAME; frame++)
        {
#ifdef _GTK
          drawable = (Drawable)TtaGetThotWindow (frame);
          if (drawable != 0)
            {
              w = FrameTable[frame].WdFrame;
              gdk_window_set_cursor (GTK_WIDGET(w)->window, WaitCurs);
            }
#endif /* _GTK */
        }
    }
  else
    {
      frame = GetWindowNumber (document, view);
#ifdef _GTK
      if (frame != 0)
        {
          w = FrameTable[frame].WdFrame;
          gdk_window_set_cursor (GTK_WIDGET(w)->window, WaitCurs);
        }
#endif /* _GTK */
    }
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void TtaResetCursor (Document document, View view)
{
  int                 frame;
#ifdef _GTK
  Drawable            drawable;
  ThotWidget          w;
#endif /* _GTK */

  UserErrorCode = 0;
  /* verifie le parametre document */
  if (document == 0 && view == 0)
    {
      for (frame = 1; frame <= MAX_FRAME; frame++)
        {
#ifdef _GTK
          drawable = (Drawable)TtaGetThotWindow (frame);
          if (drawable != 0)
            {
              w = FrameTable[frame].WdFrame;
              if (w != NULL)
                if (w->window != NULL)
                  gdk_window_set_cursor(GTK_WIDGET(w)->window, ArrowCurs);
            }
#endif /* _GTK */
        }
    }
  else
    {
      frame = GetWindowNumber (document, view);
#ifdef _GTK
      if (frame != 0)
        {
          w = FrameTable[frame].WdFrame;
          if (w != NULL)
            if (w->window != NULL)
              gdk_window_set_cursor(GTK_WIDGET(w)->window, ArrowCurs);
        }
#endif /* _GTK */
    }
}

/*----------------------------------------------------------------------
  GiveClickedAbsBox returns the window and the abstract box selected.
  ----------------------------------------------------------------------*/
void GiveClickedAbsBox (int *frame, PtrAbstractBox *pAb)
{
#if defined(_WX) | defined(_GTK)
  ThotEvent           event;
  int                 i;
#endif /* _WX | _GTK */
#if defined(_WX)
  ThotFrame           w;
#endif /* _WX */
#ifdef _GTK   
  Drawable            drawable;
  ThotWidget          w;
#endif /* _GTK */
#ifdef _WINGUI
  MSG                 event;
  HCURSOR             cursor;          
  int                 curFrame;
#endif /* _WINGUI */

  if (ClickIsDone == 1)
    {
      *frame = 0;
      *pAb = NULL;
    }

  /* Change the cursor */
#ifdef _WINGUI
  cursor = LoadCursor (hInstance, MAKEINTRESOURCE (Window_Curs));
#endif  /* _WINGUI */
#ifdef _GTK
  for (i = 1; i <= MAX_FRAME; i++)
    {
      drawable = (Drawable)TtaGetThotWindow (i);
      if (drawable)
        {
          w = FrameTable[i].WdFrame;
          if (w && w->window)
            gdk_window_set_cursor(GTK_WIDGET(w)->window, WindowCurs);
        }
    }
#endif /* _GTK */

#ifdef _WX
  /* change the cursor */
  for (i = 1; i <= MAX_FRAME; i++)
    {
      w = FrameTable[i].WdFrame;
      if (w)
        w->GetCanvas()->SetCursor( wxCursor(wxCURSOR_CROSS) );
    }
#endif /* _WX */

  /* wait the click on the target */
  ClickIsDone = 1;
  ClickFrame = 0;
  ClickX = 0;
  ClickY = 0;
  while (ClickIsDone == 1)
    {
#if defined(_GTK) | defined(_WX)
      TtaHandleOneEvent (&event);
#endif /* _GTK | _WX */
#ifdef _WINGUI
      GetMessage (&event, NULL, 0, 0);
      curFrame = GetFrameNumber (event.hwnd);
      TtaHandleOneEvent (&event);
      SetCursor (cursor);
#endif /* _WINGUI */
    }

#ifdef _GTK
  /* Restore the cursor */
  for (i = 1; i <= MAX_FRAME; i++)
    {
      drawable = (Drawable)TtaGetThotWindow (i);
      if (drawable)
        {
          w = FrameTable[i].WdFrame;  
          if (w != NULL)
            if (w->window != NULL)
              gdk_window_set_cursor(GTK_WIDGET(w)->window, ArrowCurs);
        }
    }
#endif /* _GTK */

#ifdef _WX
  /* restore the cursor */
  for (i = 1; i <= MAX_FRAME; i++)
    {
      w = FrameTable[i].WdFrame;
      if (w)
        w->GetCanvas()->SetCursor( wxNullCursor );
    }
#endif /* _WX */

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
#ifdef _WX
  AmayaFrame         *p_frame;
#else /* _WX */
  CHAR_T             *ptr;
  unsigned char      *s;
#if defined(_GTK)
  ThotWidget          w;
#endif /* #if defined(_GTK) */
#endif /* _WX */

#ifndef _WX
  if (encoding == TtaGetDefaultCharset ())
    title = text;
  else if (encoding == UTF_8)
    title = TtaConvertMbsToByte (text, TtaGetDefaultCharset ());
  else
    {
      ptr = TtaConvertByteToWC (text, encoding);
      title = TtaConvertWCToByte (ptr, TtaGetDefaultCharset ());
      TtaFreeMemory (ptr);
    }
#else /* _WX */
  /* Disabled for the moment, impossible to Save As otherwise */
  /*
    wxASSERT_MSG( encoding == UTF_8, _T("Encoding should be UTF8 !") );
  */
  title = text;
#endif /* _WX */

#ifdef _WX
  p_frame = FrameTable[frame].WdFrame;
  if ( p_frame )
    p_frame->SetFrameTitle( TtaConvMessageToWX( (char *)title ) );
#else /* _WX */
  /* Add the Amaya version */
  s = (unsigned char *)TtaGetMemory (strlen ((const char *)title) + strlen (TtaGetAppVersion()) + 10);
  sprintf ((char *)s, "%s - Amaya %s", title, TtaGetAppVersion());
#ifdef _WINGUI
  if (FrMainRef [frame])
    SetWindowText (FrMainRef[frame], s);
#endif /* _WINGUI */
#ifdef _GTK
  w = FrameTable[frame].WdFrame;
  if (w)
    {
      w = gtk_widget_get_toplevel (w);
      gtk_window_set_title (GTK_WINDOW(w), (gchar *)s);
    }
#endif /* _GTK */
  TtaFreeMemory (s);
#endif /* _WX */

  if (title != text)
    TtaFreeMemory (title);
}

/*----------------------------------------------------------------------
  La frame d'indice frame devient la fenetre active.               
  ----------------------------------------------------------------------*/
void ChangeSelFrame (int frame)
{
  Document            doc, olddoc;
  View                view;
  int                 oldframe;

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
#ifdef _WX
      /* update the class list */
      TtaExecuteMenuAction ("ApplyClass", doc, 1, FALSE);
      TtaRefreshElementMenu (doc, 1);
      UpdateAttrMenu (LoadedDocument[doc-1], TRUE);
#endif /* _WX */
      /* the active frame changed so update the application focus */
      TtaRedirectFocus();
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
#ifndef _WX
      if (FrRef[f] != 0 && FrRef[f] == w)
        break;
#else /* _WX */
      if (FrameTable[f].WdFrame != 0 && (ThotWindow)FrameTable[f].WdFrame == w)
        break;
#endif /* _WX */
    }
  return (f);
}

#ifdef _WINGUI
/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
int GetWindowWinMainFrame (ThotWindow w)
{
  int                 f;

  /* On recherche l'indice de la fenetre */
  for (f = 0; f <= MAX_FRAME; f++) {
    if (FrMainRef[f] != 0 && FrMainRef[f] == w)
      break;
  }
  return (f);
}
#endif /* _WINGUI */

/*----------------------------------------------------------------------
  GetSizesFrame retourne les dimensions de la fenetre d'indice frame.        
  ----------------------------------------------------------------------*/
void GetSizesFrame (int frame, int *width, int *height)
{
#if defined(_GTK) || defined(_WX) 
  *width = FrameTable[frame].FrWidth;
  *height = FrameTable[frame].FrHeight;
#endif /* #if defined(_GTK) || defined(_WX) */
#ifdef _WINGUI
  RECT rWindow;

  if (GetClientRect (FrRef[frame], &rWindow) != 0)
    {
      *height = rWindow.bottom - rWindow.top;
      *width  = rWindow.right - rWindow.left;
    }
  else
    {
      *height = 0;
      *width = 0;
    }
#endif /* _WINGUI */
}

/*----------------------------------------------------------------------
  DefineClipping defines the window area to be redisplayed and updates
  the values in the concrete image.
  ----------------------------------------------------------------------*/
void  DefineClipping (int frame, int orgx, int orgy, int *xd, int *yd,
                      int *xf, int *yf, int raz)
{
  int              clipx, clipy, clipwidth, clipheight;

#ifndef _GL 
#ifdef _GTK
  GdkRectangle      rect;
#endif /* _GTK */
#endif /* _GL */

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
#ifdef _WINGUI
      if (!(clipRgn = CreateRectRgn (clipx, clipy, 
                                     clipx + clipwidth, clipy + clipheight)))
        WinErrorBox (NULL, "DefineClipping");
#endif  /* _WINGUI */ 
#ifdef _GTK 
      rect.x = clipx;
      rect.y = clipy;
      rect.width = clipwidth;
      rect.height = clipheight;
      gdk_gc_set_clip_rectangle (TtLineGC, &rect);	
      gdk_gc_set_clip_rectangle (TtGreyGC, &rect);
      gdk_gc_set_clip_rectangle (TtGraphicGC, &rect);
#endif /* _GTK */      
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
#ifndef _GL
#ifdef _GTK
  GdkRectangle         rect;

  rect.x = 0;
  rect.y = 0;
  rect.width = MAX_SIZE;
  rect.height = MAX_SIZE;

  gdk_gc_set_clip_rectangle (TtLineGC, &rect);
  gdk_gc_set_clip_rectangle (TtGraphicGC, &rect);
  gdk_gc_set_clip_rectangle (TtGreyGC, &rect);
#endif /* _GTK */
#ifdef _WINGUI
  SelectClipRgn(TtDisplay, NULL); 
  if (clipRgn && !DeleteObject (clipRgn))
    WinErrorBox (NULL, "RemoveClipping");
  clipRgn = (HRGN) 0;
#endif /* _WINGUI */
#else /* _GL */
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
#ifdef _GTK
  GtkAdjustment      *tmpw;  
#endif /* _GTK */ 
#ifdef _WINGUI
  SCROLLINFO          scrollInfo;
  ThotBool            need_resize;
#endif /* _WINGUI */

#ifdef _WX
  /* prevent infinite loop (hide/show) when scrollbars are regenerated by a resize event */
  if (!ComputeScrollBar)
    return;
#endif /* _WX */

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

#if defined(_GTK) || defined(_WX)
  l = FrameTable[frame].FrWidth;
  h = FrameTable[frame].FrHeight;

#ifdef _WX
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
#endif /*_WX*/

#ifdef _GTK
  if (width + x <= l &&
      (width + vscroll->allocation.width <= l || 
       x || width <= 60))
    {
      tmpw = gtk_range_get_adjustment (GTK_RANGE (hscroll));
      if (tmpw)
        {
          tmpw->lower = (gfloat) 0;
          tmpw->upper = (gfloat) l;
          tmpw->page_size = (gfloat) width;
          tmpw->page_increment = (gfloat) width-13;
          tmpw->step_increment = (gfloat) 8;
          tmpw->value = (gfloat) x;
          gtk_adjustment_changed (tmpw);
          if (GTK_WIDGET_VISIBLE(GTK_WIDGET (hscroll)) == FALSE)
            {
              gtk_widget_show (GTK_WIDGET (hscroll));
              gtk_widget_draw_default (GTK_WIDGET (hscroll));
            }
        }
    }
  else
    /*if ((width + vscroll->allocation.width) > l && 
      x == 0 && width > 60)*/
    {
      if (GTK_WIDGET_VISIBLE(GTK_WIDGET (hscroll)))
        gtk_widget_hide (GTK_WIDGET (hscroll));
    }  
  
  if (height + y <= h &&
      (height + hscroll->allocation.height <= h || y))
    {
      tmpw = gtk_range_get_adjustment (GTK_RANGE (vscroll));
      if (tmpw)
        {
          tmpw->lower = (gfloat) 0;
          tmpw->upper = (gfloat) h;
          tmpw->page_size = (gfloat) height;
          tmpw->page_increment = (gfloat) height;
          tmpw->step_increment = (gfloat) 6;
          tmpw->value = (gfloat) y;
          gtk_adjustment_changed (tmpw);
          if (GTK_WIDGET_VISIBLE(GTK_WIDGET (vscroll)) == FALSE)
            {
              gtk_widget_show (GTK_WIDGET (vscroll));
              gtk_widget_draw_default (GTK_WIDGET (vscroll));
            }
        }
    }
  else
    /*if ((height + hscroll->allocation.height) > h && y == 0)*/
    {
      if (GTK_WIDGET_VISIBLE(GTK_WIDGET (vscroll)))
        gtk_widget_hide (GTK_WIDGET (vscroll));
    }  
#endif /*_GTK*/  
#endif /* #if defined(_GTK) || defined(_WX) */

#ifdef _WINGUI
  if (!ComputeScrollBar)
	return;
  ComputeScrollBar = FALSE;
  need_resize = FALSE;
  l = FrameTable[frame].FrWidth;
  h = FrameTable[frame].FrHeight;
  memset(&scrollInfo, 0, sizeof(scrollInfo));	
  scrollInfo.cbSize = sizeof (SCROLLINFO);
  scrollInfo.fMask  = SIF_PAGE | SIF_POS | SIF_RANGE;
  if (width >= l - 4 && x == 0 && width > 60)
    {
      /*hide*/
      if (Win_Scroll_visible (FrameTable[frame].WdScrollH) == TRUE)
        {
          scrollInfo.nMax = 2;
          SetScrollInfo (FrameTable[frame].WdScrollH, SB_CTL, &scrollInfo, TRUE);
          ShowScrollBar (FrameTable[frame].WdScrollH, SB_CTL, FALSE);
          need_resize = TRUE;
        }
    }
  else if (width + x <= l)
    {
      /*show*/
      if (Win_Scroll_visible (FrameTable[frame].WdScrollH) == FALSE)
        {
          CloseTextInsertion ();
          ShowScrollBar (FrameTable[frame].WdScrollH, SB_CTL, TRUE);
          need_resize = TRUE;
        }
      scrollInfo.nMax   = l;
      scrollInfo.nPage  = width;
      scrollInfo.nPos   = x;	 
      SetScrollInfo (FrameTable[frame].WdScrollH, SB_CTL, &scrollInfo, TRUE);
    }
  else
    /*update*/
    if (Win_Scroll_visible (FrameTable[frame].WdScrollH) == FALSE)
      {
        CloseTextInsertion ();
        ShowScrollBar (FrameTable[frame].WdScrollH, SB_CTL, FALSE);
        need_resize = TRUE;
      }
   
  if (height >= h && y == 0)
    { 
      /*hide*/
      if (Win_Scroll_visible (FrameTable[frame].WdScrollV))
        {
          scrollInfo.nMax = 2;
          SetScrollInfo (FrameTable[frame].WdScrollV, SB_CTL, &scrollInfo, TRUE);
          ShowScrollBar(FrameTable[frame].WdScrollV, SB_CTL, FALSE);
          need_resize = TRUE;
        }
    }
  else if (height + y <= h)
    {
      /*show*/
      if (Win_Scroll_visible (FrameTable[frame].WdScrollV) == FALSE)
        {
          CloseTextInsertion ();
          ShowScrollBar (FrameTable[frame].WdScrollV, SB_CTL, TRUE);
          need_resize = TRUE;
        }
      scrollInfo.nMax   = h;
      scrollInfo.nPage  = height;
      scrollInfo.nPos   = y;
      SetScrollInfo (FrameTable[frame].WdScrollV, SB_CTL, &scrollInfo, TRUE);
	 
    }
  else
    /*show*/
    if (Win_Scroll_visible (FrameTable[frame].WdScrollV) == FALSE)
      {
        CloseTextInsertion ();
        ShowScrollBar (FrameTable[frame].WdScrollV, SB_CTL, TRUE);
        need_resize = TRUE;
      }

  if (need_resize)
    {
      Wnd_ResizeContent (FrMainRef[frame], 
                         FRWidth[frame], 
                         FRHeight[frame],  
                         frame);
    }
  ComputeScrollBar = TRUE;
#endif /* _WINGUI */
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
