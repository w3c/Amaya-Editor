/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2004
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * Handle application frames
 *
 * Authors: I. Vatton (INRIA)
 *          R. Guetari (W3C/INRIA) - Windows version
 *          P. Cheyrou-Lagreze (INRIA) - OpenGL Version
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
#ifdef _GTK
  #include <gdk/gdkx.h>
#endif /*_GTK*/
#ifdef _GL
  #include <GL/gl.h>
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

static char         OldMsgSelect[MAX_TXT_LEN];
static PtrDocument  OldDocMsgSelect;

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

#ifdef _WX
  #include "AmayaWindow.h"
  #include "AmayaFrame.h"
  #include "AmayaCanvas.h"
  #include "appdialogue_wx.h"
  #include "message_wx.h"
  #include "AmayaScrollBar.h"
#endif /* _WX */
#ifdef _GTK
  #include "gtk-functions.h"
#endif /* _GTK */

#include "appli_f.h"
#include "absboxes_f.h"
#include "appdialogue_f.h"
#include "applicationapi_f.h"
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
  Called when a clique is done on the up right corner cross
  Kill the current document ( GTK version )                            
  ----------------------------------------------------------------------*/
#ifdef _GTK
gboolean KillFrameGTK (GtkWidget *widget, GdkEvent *event, gpointer f)
{
  return KillFrameCallback( (int)f );
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

#ifdef _WINGUI
/*----------------------------------------------------------------------
  GetMainFrameNumber returns the Thot window number associated to an
   MS-Windows window.
  ----------------------------------------------------------------------*/
int GetMainFrameNumber (ThotWindow win)
{
   int frame;

   for (frame = 0; frame <= MAX_FRAME; frame++)
       if (FrMainRef[frame] == win)
	  return (frame);
   return -1;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
static void CopyToolTipText (int frame, LPTOOLTIPTEXT lpttt)
{
  char      *pString;
  char      *pDest = lpttt->lpszText;
  int        i;
  int        iButton = lpttt->hdr.idFrom;

  /* Map command ID to string index */
  for (i = 1; FrameTable[frame].ButtonId[i] != -1; i++)
    {
      if (FrameTable[frame].ButtonId[i] == iButton)
	{
	  iButton = i;
	  break;
	}
    }
  pString = FrameTable[frame].TbStrings[iButton];
  strcpy (pDest, pString);
}

/*----------------------------------------------------------------------
   WIN_HandleExpose deal with the redrawing of the Client Area when  
   a WM_PAINT has been received in MS-Windows.             
  ----------------------------------------------------------------------*/
void WIN_HandleExpose (ThotWindow w, int frame, WPARAM wParam, LPARAM lParam)
{
 PAINTSTRUCT         ps;
#ifndef _GL
 RECT                rect;
 ViewFrame          *pFrame;
 int                 xmin, xmax, ymin, ymax;
#else
 HDC                 hDC;
#endif /*_GL*/

 /* Do not redraw if the document is in NoComputedDisplay mode. */
 if (frame > 0 && frame <= MAX_FRAME &&
     documentDisplayMode[FrameTable[frame].FrDoc - 1] != NoComputedDisplay )
   {
#ifndef _GL
     if (GetUpdateRect (w, &rect, FALSE))
       {
	 BeginPaint (w, &ps);
	 /* save the previous clipping */
	 pFrame = &ViewFrameTable[frame - 1];
	 xmin = pFrame->FrClipXBegin;
	 xmax = pFrame->FrClipXEnd;
	 ymin = pFrame->FrClipYBegin;
	 ymax = pFrame->FrClipYEnd;
	 pFrame = &ViewFrameTable[frame - 1];
	 pFrame->FrClipXBegin = 0;
	 pFrame->FrClipXEnd = 0;
	 pFrame->FrClipYBegin = 0;
	 pFrame->FrClipYEnd = 0;
	 DefClip (frame, rect.left + pFrame->FrXOrg,
		  rect.top + pFrame->FrYOrg,
		  rect.right + pFrame->FrXOrg,
		  rect.bottom + pFrame->FrYOrg);
	 DisplayFrame (frame);
	 EndPaint (w, &ps);
	 /* restore the previous clipping */
	 pFrame->FrClipXBegin = xmin;
	 pFrame->FrClipXEnd = xmax;
	 pFrame->FrClipYBegin = ymin;
	 pFrame->FrClipYEnd = ymax;
	 return;
       }
#else /*_GL*/
     hDC = BeginPaint (w, &ps);
     if (GetBadCard())
	 {
	   DefClip (frame, -1, -1, -1, -1);
	   DisplayFrame (frame);
	 }
     WinGL_Swap (hDC);
     EndPaint (w, &ps);
     ReleaseDC (w, hDC);
     return;
#endif /*_GL*/
   }
 BeginPaint (w, &ps);
 EndPaint (w, &ps);
}

/*----------------------------------------------------------------------
   WIN_ChangeViewSize: function called when a view is resized under    
   MS-Windows.                                                   
  ----------------------------------------------------------------------*/
void WIN_ChangeViewSize (int frame, int width, int height, int top_delta,
						 int bottom_delta)
{
   if ((width <= 0) || (height <= 0))
      return;
   if (documentDisplayMode[FrameTable[frame].FrDoc - 1] == NoComputedDisplay)
      return;
   
   FrameTable[frame].FrTopMargin = top_delta;
   FrameTable[frame].FrWidth = (int) width - bottom_delta;
   FrameTable[frame].FrHeight = (int) height;

#ifndef _GL
   /* need to recompute the content of the window */
   RebuildConcreteImage (frame);
#else /*_GL*/
   if (GL_prepare (frame))
     {
       GL_SwapEnable (frame);
       GLResize (width, height, 0 ,0);
       ClearAll (frame);
       GL_ActivateDrawing (frame);
       /* redisplay the whole window */
       DefClip (frame, -1, -1, -1, -1);
       RebuildConcreteImage (frame);
       GL_Swap (frame);
       /* recompute the scroll bars */
       /*UpdateScrollbars (frame); Done in rebuildconcreteimage, no ?*/
     }
#endif/*_GL*/
}
#endif /* _WINGUI */


/*----------------------------------------------------------------------
   FrameToRedisplay effectue le traitement des expositions X11 des     
   frames de dialogue et de documents.                   
  ----------------------------------------------------------------------*/
void FrameToRedisplay (ThotWindow w, int frame, void *ev)
{
#if defined(_GTK)
  XExposeEvent       *event = (XExposeEvent *) ev;
  ViewFrame          *pFrame;
  int                 xmin, xmax, ymin, ymax;
  int                 x, y, l, h;

  if (frame > 0 && frame <= MAX_FRAME && FrameTable[frame].FrDoc > 0 &&
      /* don't handle a document in mode NoComputedDisplay */
      documentDisplayMode[FrameTable[frame].FrDoc - 1] != NoComputedDisplay)
    {
      pFrame = &ViewFrameTable[frame - 1];
      x = event->x + pFrame->FrXOrg;
      y = event->y + pFrame->FrYOrg;
      l = event->width;
      h = event->height;
      /* save the previous clipping */
      xmin = pFrame->FrClipXBegin;
      xmax = pFrame->FrClipXEnd;
      ymin = pFrame->FrClipYBegin;
      ymax = pFrame->FrClipYEnd;
      pFrame = &ViewFrameTable[frame - 1];
      pFrame->FrClipXBegin = 0;
      pFrame->FrClipXEnd = 0;
      pFrame->FrClipYBegin = 0;
      DefClip (frame, x, y, x + l, y + h);
      RedrawFrameBottom (frame, 0, NULL);
      /* restore the previous clipping */
      pFrame = &ViewFrameTable[frame - 1];
      pFrame->FrClipXBegin = xmin;
      pFrame->FrClipXEnd = xmax;
      pFrame->FrClipYBegin = ymin;
      pFrame->FrClipYEnd = ymax;
    }
#endif /* #if defined(_GTK) */
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
	  RebuildConcreteImage (frame);	   
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

#ifdef _WINGUI
/*----------------------------------------------------------------------
  GL_Win32ContextInit : Turn a win32 windows into an opengl drawing canvas, 
  setting up pxel format,
  Creating the frame number if needed.
  ----------------------------------------------------------------------*/
void GL_Win32ContextInit (HWND hwndClient, int frame)
{
  static ThotBool dialogfont_enabled = FALSE;
  int frame_index;
  HGLRC hGLRC;
  HDC hDC;
  ThotBool found;

  hDC = 0;
  hDC = GetDC (hwndClient);	
  if (!hDC) 
    {      
      MessageBox(NULL, "ERROR!", "No device context", MB_OK); 
      return;    
    }
  else if (dialogfont_enabled)
    {
      for (frame_index = 0 ; frame_index <= MAX_FRAME; frame_index++)
	{  
	  if (GL_Windows[frame_index] == hDC) 
	    return;
	}
    }
  found = FALSE;       
  if (frame <= 0)
    {
      frame = 1;
      while (frame <= MAX_FRAME && !found)
	{
	  /* Seeks a free frame slot */
	  found = (FrameTable[frame].FrDoc == 0 && FrameTable[frame].WdFrame != 0);
	  if (!found)
	    frame++;
	}	
      if (!found)
	{
	  frame = 1;
	  while (frame <= MAX_FRAME && !found)
	    {
	      /* Seeks a free frame slot */
	      found = (FrameTable[frame].WdFrame == 0);
	      if (!found)
		frame++;
	    }
	}
    }
  GL_SetupPixelFormat (hDC);
  hGLRC = wglCreateContext (hDC);
  GL_Windows[frame] = hDC;
  GL_Context[frame] = hGLRC;
  if (wglMakeCurrent (hDC, hGLRC))
    {
      SetGlPipelineState ();
      if (!dialogfont_enabled)
	{
	  InitDialogueFonts ("");
	  dialogfont_enabled = TRUE;
	  for (frame_index = 0 ; frame_index <= MAX_FRAME; frame_index++)
	    {  
	      if (frame_index != frame)
		{
		  GL_Windows[frame_index] = 0;
		  GL_Context[frame_index] = 0;
		}
	    }
	}
    }
#ifndef _NOSHARELIST
  if (Shared_Context != -1 && Shared_Context != frame) 
    wglShareLists (GL_Context[Shared_Context], hGLRC);
  else
    Shared_Context = frame;
#endif /*_NOSHARELIST*/
  ActiveFrame = frame;
  ReleaseDC (hwndClient, GL_Windows[frame]);
}
#endif /* _WINGUI */
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

#ifdef _GTK
#ifdef _GL
/*----------------------------------------------------------------------
  DrawGL:
  ----------------------------------------------------------------------*/
gboolean GL_DrawCallback (ThotWidget widget, GdkEventExpose *event, 
			  gpointer data)
{
  DefClip ((int ) data, -1, -1, -1, -1);
  GL_DrawAll (/*(int ) data*/);
  return TRUE;
}

/*----------------------------------------------------------------------
  GL_Destroy: Close Opengl pipeline
  ----------------------------------------------------------------------*/
gboolean  GL_Destroy (ThotWidget widget, GdkEventExpose *event, 
		      gpointer data)
{
  int      frame;
 
  frame = (int) data;
  FreeAllPicCacheFromFrame (frame);
  return TRUE;
}

/*----------------------------------------------------------------------
  GL_Init: Opengl pipeline state initialization
  ----------------------------------------------------------------------*/
gboolean GL_Init (ThotWidget widget, GdkEventExpose *event, gpointer data)
{
  while (!gtk_gl_area_make_current (GTK_GL_AREA(widget)))
    ;
  SetGlPipelineState ();
  return TRUE;   
}

#endif /* _GL */

static ThotBool  FrameResizedGTKInProgress = FALSE;
/*----------------------------------------------------------------------
  FrameResizedGTK When user resize window
  ----------------------------------------------------------------------*/
gboolean FrameResizedGTK (GtkWidget *w, GdkEventConfigure *event, gpointer data)
{
  ViewFrame          *pFrame;
  int            frame;
  int            width, height;
 
  frame = (int) data;
  width = event->width;
  height = event->height; 
  if (width <= 0 || height <= 0 || frame == 0 || frame > MAX_FRAME)
    return FALSE;
  if (FrameTable[frame].FrDoc == 0 ||
      documentDisplayMode[FrameTable[frame].FrDoc - 1] == NoComputedDisplay)
    return FALSE; 
  if (FrameTable[frame].FrWidth == width && FrameTable[frame].FrHeight == height)
    return FALSE;
  if (FrameResizedGTKInProgress)
    return FALSE;
  else
    FrameResizedGTKInProgress = TRUE;
#ifdef _GL
  pFrame = &ViewFrameTable[frame - 1];
  if (w)
    if (GL_prepare (frame))
      {
	/* prevent flickering*/
	GL_SwapStop (frame); 
	FrameTable[frame].FrWidth = width;
	FrameTable[frame].FrHeight = height;
	GLResize (width, height, 0, 0);
	DefClip (frame, pFrame->FrXOrg, pFrame->FrYOrg,
		 width + pFrame->FrXOrg, height + pFrame->FrYOrg);
        FrameRedraw (frame, width, height);
#ifdef IV
	int         forever = 0;
	/*FrameRedraw can modify Size by hiding scrollbars
	  so until sizes are stabilized, we resize. 
	  if it never stabilizes itself, we stop at at
	  the tenth resize*/
	while (gtk_events_pending ())
	  gtk_main_iteration ();

	while ((width != w->allocation.width ||
	       height != w->allocation.height) && 
	       forever < 10)
	  {	     
	    width = w->allocation.width; 
	    height = w->allocation.height; 
	    if (GL_prepare (frame))
	      {
		FrameTable[frame].FrWidth = width;
		FrameTable[frame].FrHeight = height;
		GLResize (width, height, 0, 0);
		DefClip (frame, pFrame->FrXOrg, pFrame->FrYOrg,
			 width + pFrame->FrXOrg, height + pFrame->FrYOrg);
		FrameRedraw (frame, width, height);
	      }
	    while (gtk_events_pending ())
	      gtk_main_iteration ();

	    forever++;
	  }
#endif
	FrameTable[frame].DblBuffNeedSwap = TRUE; 
	GL_SwapEnable (frame);
	if (GL_prepare (frame))
	  {
	    GL_Swap (frame);
	    FrameTable[frame].DblBuffNeedSwap = TRUE; 
	  }
      }
#else /* _GL*/
  FrameRedraw (frame, width, height);
#endif /* _GL */
  FrameResizedGTKInProgress = FALSE;
  return TRUE;
}

/*----------------------------------------------------------------------
  ExposeCallbackGTK : 
  When a part of the canvas is hidden by a window or menu 
  It permit to Redraw frames 
  ----------------------------------------------------------------------*/
gboolean ExposeCallbackGTK (ThotWidget widget, GdkEventExpose *event, gpointer data)
{
  ViewFrame           *pFrame;
  int                  frame;
  unsigned int         x, y;
  unsigned int         w, h;

  frame = (int) data;
  x = event->area.x;
  y = event->area.y;
  w = event->area.width;
  h = event->area.height;
  if (w <= 0 || h <= 0 || frame == 0 || frame > MAX_FRAME)
    return TRUE;
  if (FrameTable[frame].FrDoc == 0 ||
      documentDisplayMode[FrameTable[frame].FrDoc - 1] == NoComputedDisplay)
    return TRUE;

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
  if (GL_prepare (frame))
    {
      if (glhard () || GetBadCard ()) 
	{
	  x = pFrame->FrXOrg;
	  y = pFrame->FrYOrg;
	  w = FrameTable[frame].FrWidth;
	  h = FrameTable[frame].FrHeight;
	  DefClip (frame, x, y, x + w, y + h);
	  RedrawFrameBottom (frame, 0, NULL);
	}
	GL_Swap (frame);
    }
#else /* _GL */
  x += pFrame->FrXOrg;
  y += pFrame->FrYOrg;
  DefClip (frame, x, y, x + w, y + h);
  RedrawFrameBottom (frame, 0, NULL);
#endif /* _GL */
  return FALSE;
}
#endif /* _GTK */

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
  if (GL_prepare (frame))
    {
      if (glhard () || GetBadCard ()) 
	{
	  x = pFrame->FrXOrg;
	  y = pFrame->FrYOrg;
	  w = FrameTable[frame].FrWidth;
	  h = FrameTable[frame].FrHeight;
	  DefClip (frame, x, y, x + w, y + h);
	  RedrawFrameBottom (frame, 0, NULL);
	}
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
      documentDisplayMode[FrameTable[frame].FrDoc - 1] == NoComputedDisplay ||
      ( FrameTable[frame].FrWidth == new_width &&
	FrameTable[frame].FrHeight == new_height))
    /* frame should not be displayed */
    return FALSE;
  else if (FrameTable[frame].FrWidth - new_width == new_height -FrameTable[frame].FrHeight)
    /* prevent an infinite loop */
    return FALSE;
    
#ifdef _GL
  if (GL_prepare( frame))
    {
      /* prevent flickering*/
      GL_SwapStop (frame);
      FrameTable[frame].FrWidth = new_width;
      FrameTable[frame].FrHeight = new_height;
      GLResize (new_width, new_height, 0, 0);
      DefClip (frame, -1, -1, -1, -1);
      FrameRedraw (frame, new_width, new_height);
      GL_SwapEnable (frame);
      GL_Swap (frame);
    }
#else /* _GL*/
  FrameRedraw (frame, new_width, new_height);
#endif /* _GL */
  return TRUE;
}

#ifdef _WINGUI
/*----------------------------------------------------------------------
  WIN_ChangeVScroll generates a vertical scroll.          
  ----------------------------------------------------------------------*/
void WIN_ChangeVScroll (int frame, int reason, int value)
{
  int        delta, x, y, width, height;
  int        sPos, nbPages, remaining;

  if (frame < 0)
    return;
  /* do not redraw it if in NoComputedDisplay mode */
  if (documentDisplayMode[FrameTable[frame].FrDoc - 1] == NoComputedDisplay)
    return;

  switch (reason)
    {
    case SB_TOP:
      JumpIntoView (frame, 0);
      break;
       
    case SB_BOTTOM:
      JumpIntoView (frame, 100);
      break;
       
    case SB_LINEUP:
      delta = -13;
      VerticalScroll (frame, delta, 1);
      break;
       
    case SB_LINEDOWN:
      delta = 13;
      VerticalScroll (frame, delta, 1);
      break;
       
    case SB_PAGEUP:
      delta = -FrameTable[frame].FrHeight;
      VerticalScroll (frame, delta, 1);
      break;
       
    case SB_PAGEDOWN:
      delta = FrameTable[frame].FrHeight;
      VerticalScroll (frame, delta, 1);
      break;
       
    case SB_ENDSCROLL:
      break;
       
    case SB_THUMBPOSITION:
    case SB_THUMBTRACK:
      ComputeDisplayedChars (frame, &x, &y, &width, &height);
      sPos = GetScrollPos (FrameTable[frame].WdScrollV, SB_CTL);
      delta = value - sPos;
      nbPages = abs (delta) / height;
      remaining = abs (delta) - (height * nbPages);
      if (nbPages <= 3)
	{
	  if (delta > 0)
	    delta = nbPages * FrameTable[frame].FrHeight + (int) ((remaining * FrameTable[frame].FrHeight) / height);
	  else 
	    delta = -(nbPages * FrameTable[frame].FrHeight + (int) ((remaining * FrameTable[frame].FrHeight) / height));
	  VerticalScroll (frame, delta, 1);
	} 
      else 
	{
	  delta = (int) (((float)value / (float)FrameTable[frame].FrHeight) * 100);
	  JumpIntoView (frame, delta);
	}
#ifdef _GL
      GL_DrawAll (/*NULL, frame*/);
#endif /*_GL*/
      break;
    } 
}

/*----------------------------------------------------------------------
  WIN_ChangeHScroll generates a horizontal scroll.          
  ----------------------------------------------------------------------*/
void WIN_ChangeHScroll (int frame, int reason, int value)
{
  int        delta = 0, width = 1076, x, y, height;
  int        sPos, nbPages, remaining;

  /* do not redraw it if in NoComputedDisplay mode */
  if (documentDisplayMode[FrameTable[frame].FrDoc - 1] == NoComputedDisplay)
    return;

  switch (reason)
    {
    case SB_LINERIGHT:
      delta = 13;
      break;
       
    case SB_LINELEFT:
      delta = -13;
      break;
       
    case SB_PAGERIGHT:
      delta = FrameTable[frame].FrWidth;
      break;
       
    case SB_PAGELEFT:
      delta = -FrameTable[frame].FrWidth;
      break;
       
    case SB_THUMBPOSITION:
    case SB_THUMBTRACK:
      ComputeDisplayedChars (frame, &x, &y, &width, &height);
      sPos = GetScrollPos (FrameTable[frame].WdScrollH, SB_CTL);
      delta = value - sPos;
      nbPages = abs (delta) / width;
      remaining = abs (delta) - (width * nbPages);
      if (nbPages <= 3)
	{
	  if (delta > 0)
	    delta = nbPages * FrameTable[frame].FrWidth + (int) ((remaining * FrameTable[frame].FrWidth) / width);
	  else 
	    delta = -(nbPages * FrameTable[frame].FrWidth + (int) ((remaining * FrameTable[frame].FrWidth) / width));
	}
      else
	delta = (int) (((float)value / (float)FrameTable[frame].FrWidth) * 100);
      break;
    }
  if (delta)
    HorizontalScroll (frame, delta, 1);
}
#endif /* _WINGUI */

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

#ifdef _GTK
/*----------------------------------------------------------------------
 * FrameHScrolledGTK
 * is the GTK wrapper of FrameHScrolledCallback
  ----------------------------------------------------------------------*/
void FrameHScrolledGTK (GtkAdjustment *w, int frame)
{
  FrameHScrolledCallback (frame, (int)w->value, (int)w->page_size);
}

/*----------------------------------------------------------------------
 * FrameVScrolledGTK
 * is the GTK wrapper of FrameVScrolledCallback
  ----------------------------------------------------------------------*/
void FrameVScrolledGTK (GtkAdjustment *w, int frame)
{
  FrameVScrolledCallback (frame, (int)w->value);
}
#endif /*_GTK*/

/*----------------------------------------------------------------------
 * FrameVScrolledCallback (generic callback)
 * is called when the scrollbar position is changed
 * params:
 *   + int position: the new scrollbar position
 *   + int frame: the concerned frame
  ----------------------------------------------------------------------*/
void FrameVScrolledCallback (int frame, int position)
{
  int delta, x, y, width, height, viewed, left;

  if (FrameTable[frame].FrDoc &&
      documentDisplayMode[FrameTable[frame].FrDoc - 1] == NoComputedDisplay)
    return;
  ComputeDisplayedChars (frame, &x, &y, &width, &height);
  delta = position - y;
  viewed = abs (delta) / height;
  left = abs (delta) - (height * viewed);
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
#if defined(_WINGUI) || defined(_GTK) || defined(_WX)
  int          delta;
#endif /* defined(_WINGUI) || defined(_GTK) || defined(_WX) */
  int          frame;
  
  if (document != 0)
    frame = GetWindowNumber (document, view);
  else
    frame = 0;
#if defined(_WINGUI) || defined(_GTK) || defined(_WX)
  delta = -13;
  VerticalScroll (frame, delta, 1);
#endif /*#if defined(_WINGUI) || defined(_GTK) || defined(_WX) */
}

/*----------------------------------------------------------------------
  TtcLineDown scrolls one line down.                                
  ----------------------------------------------------------------------*/
void TtcLineDown (Document document, View view)
{
#if defined(_WINGUI) || defined(_GTK) || defined(_WX)
  int          delta;
#endif /* defined(_WINGUI) || defined(_GTK) || defined(_WX) */
  int          frame;

  if (document != 0)
    frame = GetWindowNumber (document, view);
  else
    frame = 0;
#if defined(_WINGUI) || defined(_GTK) || defined(_WX)
  delta = 13;
  VerticalScroll (frame, delta, 1);
#endif /* #if defined(_WINGUI) || defined(_GTK) || defined(_WX)*/
}

/*----------------------------------------------------------------------
  TtcScrollLeft scrolls one position left.                                    
  ----------------------------------------------------------------------*/
void TtcScrollLeft (Document document, View view)
{
#if defined(_WINGUI) || defined(_GTK) || defined(_WX)
  int          delta;
#endif /* defined(_WINGUI) || defined(_GTK) || defined(_WX) */
  int          frame;
  
  if (document != 0)
    frame = GetWindowNumber (document, view);
  else
    frame = 0;
#if defined(_WINGUI) || defined(_GTK) || defined(_WX)
  delta = -13;
  HorizontalScroll (frame, delta, 1);
#endif /* #if defined(_WINGUI) || defined(_GTK) || defined(_WX) */
}

/*----------------------------------------------------------------------
  TtcScrollRight scrolls one position right.                                
  ----------------------------------------------------------------------*/
void TtcScrollRight (Document document, View view)
{
#if defined(_WINGUI) || defined(_GTK) || defined(_WX)
  int          delta;
#endif /* defined(_WINGUI) || defined(_GTK) || defined(_WX) */
  int          frame;

  if (document != 0)
    frame = GetWindowNumber (document, view);
  else
    frame = 0;
#if defined(_WINGUI) || defined(_GTK) || defined(_WX) 
  delta = 13;
  HorizontalScroll (frame, delta, 1);
#endif /* #if defined(_WINGUI) || defined(_GTK) || defined(_WX) */
}

/*----------------------------------------------------------------------
  PageUp scrolls one screen up.                                    
  ----------------------------------------------------------------------*/
void TtcPageUp (Document document, View view)
{
#if defined(_WINGUI) || defined(_GTK) || defined(_WX)
  int          delta;
#endif /* defined(_WINGUI) || defined(_GTK) || defined(_WX) */
  int          frame;

  if (document != 0)
    frame = GetWindowNumber (document, view);
  else
    frame = 0;
#if defined(_WINGUI) || defined(_GTK) || defined(_WX)
  delta = -FrameTable[frame].FrHeight;
  VerticalScroll (frame, delta, 1);
#endif /*#if defined(_WINGUI) || defined(_GTK) || defined(_WX) */
}

/*----------------------------------------------------------------------
  PageDown scrolls one screen down.                                
  ----------------------------------------------------------------------*/
void TtcPageDown (Document document, View view)
{
#if defined(_WINGUI) || defined(_GTK) || defined(_WX)
  int          delta;
#endif /* defined(_WINGUI) || defined(_GTK) || defined(_WX) */
  int          frame;

  if (document != 0)
    frame = GetWindowNumber (document, view);
  else
    frame = 0;
#if defined(_WINGUI) || defined(_GTK) || defined(_WX)
  delta = FrameTable[frame].FrHeight;
  VerticalScroll (frame, delta, 1);
#endif /* #if defined(_WINGUI) || defined(_GTK) || defined(_WX) */
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
#if defined(_WINGUI) || defined(_GTK) || defined(_WX)
  JumpIntoView (frame, 0);
#endif /* #if defined(_WINGUI) || defined(_GTK) || defined(_WX) */
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
#if defined(_WINGUI) || defined(_GTK) || defined(_WX)
  JumpIntoView (frame, 100);
#endif /* #if defined(_WINGUI) || defined(_GTK) || defined(_WX) */
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
#ifdef _WX
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
#endif /* _WX */
#ifdef _GTK
      if (w != 0)
	gdk_window_show (gtk_widget_get_parent_window(GTK_WIDGET(w)));
#endif /* _GTK */
#ifdef _WINGUI
      {
	OpenIcon (FrMainRef[frame_id]);
	SetForegroundWindow (FrMainRef[frame_id]);
      }
#endif /* _WINGUI */
    }
}

/*----------------------------------------------------------------------
  DisplaySelMessage affiche la se'lection donne'e en parame`tre (texte) dans 
  la fenetre active.                                            
  ----------------------------------------------------------------------*/
void DisplaySelMessage (char *text, PtrDocument pDoc)
{
  int                 doc;
  int                 view;


  if (ActiveFrame &&
      pDoc && pDoc->DocSSchema &&
      (strcmp (OldMsgSelect, text) || pDoc != OldDocMsgSelect) &&
      strcmp (pDoc->DocSSchema->SsName, "TextFile"))
    {
      /* recupere le document concerne */
      doc = FrameTable[ActiveFrame].FrDoc;
      for (view = 1; view <= MAX_VIEW_DOC; view++)
	TtaSetStatus ((Document) doc, view, text, NULL);
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
  char                *s;

#ifdef _GTK
  gchar * title_string;
#endif /* _GTK */

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
#ifndef _WX
	if (FrameTable[frame].WdStatus != 0)
#endif /* _WX */
	  {
	    length = strlen (text) + 1;
	    if (name)
	      length += strlen (name);
	    s = (char *)TtaGetMemory (length);
#ifdef _WINGUI
	    if (name)
	      /* text est un format */
	      sprintf (s, text, name);
	    else
	      strncpy (s, text, length);

	    SendMessage (FrameTable[frame].WdStatus, SB_SETTEXT, (WPARAM) 0, (LPARAM) s);
	    SendMessage (FrameTable[frame].WdStatus, WM_PAINT, (WPARAM) 0, (LPARAM) 0);
#endif /* _WINGUI */
#ifdef _GTK
	    if (name)
	      {
		/* text est un format */
		sprintf (s, text, name);
		title_string = s;
	      }
	    else
	      title_string = text;
	    gtk_statusbar_pop (GTK_STATUSBAR(FrameTable[frame].WdStatus),
			       (guint)gtk_object_get_data (GTK_OBJECT(FrameTable[frame].WdStatus), "MainSerie"));
	    gtk_statusbar_push (GTK_STATUSBAR(FrameTable[frame].WdStatus),
				(guint)gtk_object_get_data (GTK_OBJECT(FrameTable[frame].WdStatus), "MainSerie"),
				title_string);
	    gtk_widget_show_all (GTK_WIDGET(FrameTable[frame].WdStatus));
#endif /* _GTK */

#ifdef _WX
	    if (name)
	      /* text est un format */
	      sprintf (s, text, name);
	    else
	      strncpy (s, text, length);

	    /* 
	     * do not use the FrameTable[frame].WdStatus field because it's simplier
	     * to update only the frame's parent window
	     */
	    FrameTable[frame].WdFrame->SetStatusBarText( TtaConvMessageToWX( s ) );
#endif /* _WX */
	    
	    TtaFreeMemory (s);
	  }
      
    }
}



#ifdef _WINGUI
/*----------------------------------------------------------------------
  CheckTtCmap creates the Thot palette if it doesn't exist
  ----------------------------------------------------------------------*/
void CheckTtCmap ()
{
  int     i;

  if (TtCmap == NULL)
    {
      ptrLogPal = (PLOGPALETTE) LocalAlloc (LMEM_FIXED,
					    sizeof(LOGPALETTE) + MAX_COLOR * sizeof(PALETTEENTRY));
	  
      ptrLogPal->palVersion    = 0x300;
      ptrLogPal->palNumEntries = MAX_COLOR;
	  
      for (i = 0; i < MAX_COLOR; i++)
	{
	  ptrLogPal->palPalEntry[i].peRed   = (BYTE) RGB_Table[i].red;
	  ptrLogPal->palPalEntry[i].peGreen = (BYTE) RGB_Table[i].green;
	  ptrLogPal->palPalEntry[i].peBlue  = (BYTE) RGB_Table[i].blue;
	  ptrLogPal->palPalEntry[i].peFlags = PC_RESERVED;
	}
      TtCmap = CreatePalette (ptrLogPal);
      LocalFree (ptrLogPal);
    }
}
/*----------------------------------------------------------------------
  Wnd_ResizeContent : resize toolbar, statusbar, and canvas frame
  ----------------------------------------------------------------------*/
static void Wnd_ResizeContent (HWND hwnd, int cx, int cy, int frame)
{
  RECT                rWindow;
  int                 cxVSB;
  int                 cyHSB;
  int                 cyTB;
  DWORD               dwStyle;
  int                 x,y,cyStatus,cyTxtZone;

  FRWidth[frame] = cx;
  FRHeight[frame] = cy;
  /* Adjust toolbar size. */
  if (WinToolBar[frame] && IsWindowVisible (WinToolBar[frame]))
    {
      dwStyle = GetWindowLong (WinToolBar[frame], GWL_STYLE);
      if (dwStyle & CCS_NORESIZE)
	MoveWindow (WinToolBar[frame], 0, 0, cx, cyToolBar, FALSE);
      else
	ToolBar_AutoSize (WinToolBar[frame]);

      InvalidateRect (WinToolBar[frame], NULL, FALSE);
      GetWindowRect (WinToolBar[frame], &rWindow);
      ScreenToClient (hwnd, (LPPOINT) &rWindow.left);
      ScreenToClient (hwnd, (LPPOINT) &rWindow.right);
      cyTB = rWindow.bottom - rWindow.top;
    }
  else 
    cyTB = 0;
    
  cyTxtZone = cyTB;
  /* Adjust text zones */
  if (FrameTable[frame].Text_Zone &&
      IsWindowVisible (FrameTable[frame].Text_Zone))
    {
      MoveWindow (FrameTable[frame].Label, 15, cyTxtZone + 5, 70, 20, TRUE);
      MoveWindow (FrameTable[frame].Text_Zone, 85, cyTxtZone + 5, cx - 85, 20, TRUE);
      cyTxtZone += 28;
    }

  /* Adjust status bar size. */
  if (IsWindowVisible (FrameTable[frame].WdStatus))
    {
      GetWindowRect (FrameTable[frame].WdStatus, &rWindow);
      cyStatus = rWindow.bottom - rWindow.top;
      MoveWindow (FrameTable[frame].WdStatus, 0, cy - cyStatus, cx, cyStatus, TRUE);
    }
  else
    cyStatus = 0;


  if (FrameTable[frame].WdScrollV && Win_Scroll_visible (FrameTable[frame].WdScrollV)) 
    {
      /* Adjust Vertical scroll bar */
      MoveWindow (FrameTable[frame].WdScrollV, cx - 15,
		  cyTxtZone, 15, cy - (cyStatus + cyTxtZone + 15), TRUE);
      /* Adjust client window size. */
      GetWindowRect (FrameTable[frame].WdScrollV, &rWindow);
      cxVSB = rWindow.right - rWindow.left;
    }
  else
    cxVSB = 0;

  if (FrameTable[frame].WdScrollH && Win_Scroll_visible (FrameTable[frame].WdScrollH)) 
    {
      /* Adjust Hoizontal scroll bar */
      MoveWindow (FrameTable[frame].WdScrollH, 0,
		  cy - (cyStatus + 15), cx - 15, 15, TRUE);
      GetWindowRect (FrameTable[frame].WdScrollH, &rWindow);
      cyHSB = rWindow.bottom - rWindow.top;
    }   
  else
    cyHSB = 0;

  x = 0;
  y = cyTxtZone;
  cx = cx - cxVSB;
  cy = cy - (cyStatus + cyTxtZone + cyHSB);
  MoveWindow (FrRef [frame], x, y, cx, cy, TRUE);
}

/*----------------------------------------------------------------------
  WndProc:  The main MS-Windows event handler for the Thot Library
  ----------------------------------------------------------------------*/
LRESULT CALLBACK WndProc (HWND hwnd, UINT mMsg, WPARAM wParam, LPARAM lParam)
{
  PtrDocument         pDoc;
  HWND                hwndTextEdit;
  HWND                hwndToolTip;
  RECT                rect;
  LPNMHDR             pnmh;
  int                 frame;
  int                 view;
  int                 frameNDX;
  int                 idCtrl;
  int                 cx;
  int                 cy;
  DWORD               dwToolBarStyles;
  DWORD               dwStatusBarStyles;

  frame = GetMainFrameNumber (hwnd);
  GetWindowRect (hwnd, &rect);

  switch (mMsg)
    {
    case WM_CREATE:
      /* Create toolbar  */
      ThotTBBitmap.hInst = hInstance;
      ThotTBBitmap.nID  = IDR_TOOLBAR;
	
      dwToolBarStyles = WS_CHILD | WS_VISIBLE | CCS_TOP | TBSTYLE_TOOLTIPS;
      dwToolBarStyles = dwToolBarStyles | TBSTYLE_FLAT;
	  if (ToolBar == 0)
	  {
      ToolBar = CreateWindow (TOOLBARCLASSNAME, NULL, dwToolBarStyles,
			      0, 0, 0, 0, hwnd, (HMENU) 1, hInstance, 0);

      SendMessage (ToolBar, TB_BUTTONSTRUCTSIZE, (WPARAM) sizeof (TBBUTTON), 0L);
      if ((SendMessage (ToolBar, TB_ADDBITMAP, (WPARAM) MAX_BUTTON,
			(LPARAM) (LPTBADDBITMAP) &ThotTBBitmap)) == -1)
	WinErrorBox (NULL, "WndProc: WM_CREATE cannot create toolbar");
    
      hwndToolTip = ToolBar_GetToolTips (ToolBar);
      if (dwToolBarStyles & TBSTYLE_TOOLTIPS)
	InitToolTip (ToolBar);	
	  }
      /* Create status bar  */
      dwStatusBarStyles = WS_CHILD | WS_VISIBLE | CCS_BOTTOM | SBARS_SIZEGRIP;
      StatusBar = CreateStatusWindow (dwStatusBarStyles, "", hwnd, 2);
      ShowWindow (StatusBar, SW_SHOWNORMAL);
      UpdateWindow (StatusBar);
      hwndClient = CreateWindowEx (WS_EX_ACCEPTFILES, "ClientWndProc", NULL,
				   WS_CHILD | WS_BORDER,
				   0, 0, 0, 0, hwnd, (HMENU) 2, hInstance, NULL);
#ifdef _GL
      if (!hwndClient) 
	{      
	  MessageBox(NULL, "ERROR!", "Failed to create new client window", MB_OK); 
	  return 0;
	}
      /* initialize OpenGL rendering */
      GL_Win32ContextInit (hwndClient, frame);
#endif /*_GL*/
      ShowWindow (hwndClient, SW_SHOWNORMAL);
      UpdateWindow (hwndClient);
      SetWindowText (hwnd, wTitle);
      DragAcceptFiles (hwnd, TRUE);
      return 0L;

    case WM_PALETTECHANGED: 
#ifndef _GL
      if ((HWND) wParam != hwnd)
	{
	  HDC hDC = GetDC (hwnd);
	  SelectPalette (hDC, TtCmap, FALSE);
	  if (RealizePalette (hDC))
	    UpdateColors (hDC);
	  ReleaseDC (hwnd, hDC);
	}
      return 0L;
#endif /*_GL*/
      
#ifdef WM_MOUSEWHEEL
    case WM_MOUSEWHEEL:
      {
	short int ydelta;
      
	ydelta = - (HIWORD (wParam));
	VerticalScroll (frame, ydelta, 1);
      }
      return 0L;
#endif /*WM_MOUSEWHEEL*/

    case WM_VSCROLL:
      WIN_ChangeVScroll (frame, LOWORD (wParam), HIWORD (wParam));
      return 0L;

    case WM_HSCROLL:
      WIN_ChangeHScroll (frame, LOWORD (wParam), HIWORD (wParam));
      return 0L;

    case WM_ENTER:
      hwndTextEdit = GetFocus ();
      if (frame != -1)
	{
	  ActiveFrame = frame;
	  APP_TextCallback (hwndTextEdit, frame, NULL);
	  SetFocus (FrRef [frame]);
	}
      return 0L;

    case WM_SYSKEYDOWN:
    case WM_KEYDOWN:
      if (frame != -1)
	{
	  SetFocus (FrRef [frame]);
	  SendMessage (FrRef [frame], mMsg, wParam, lParam);
	}
      return 0L;

    case WM_DROPFILES:
      if (frame != -1)
	{
	  SetFocus (FrRef [frame]);
	  SendMessage (FrRef [frame], mMsg, wParam, lParam);
	}
      return 0L;
	
    case WM_IME_CHAR:
    case WM_SYSCHAR:
    case WM_CHAR:
      if (frame != -1)
	{
	  SetFocus (FrRef [frame]);
	  ActiveFrame = frame;
	  SendMessage (FrRef [frame], mMsg, wParam, lParam);
	}
      return 0L;

    case WM_NOTIFY:
      pnmh = (LPNMHDR) lParam;
      idCtrl = (int) wParam;
      /*Toolbar notifications */
      if ((pnmh->code >= TBN_LAST) && (pnmh->code <= TBN_FIRST))
	return ToolBarNotify (frame, hwnd, wParam, lParam);
      /* Fetch tooltip text */
      if (pnmh->code == TTN_NEEDTEXT)
	{
	  LPTOOLTIPTEXT lpttt = (LPTOOLTIPTEXT) lParam;
	  CopyToolTipText (frame, lpttt);
	}
      return 0L;

    case WM_COMMAND:

      switch (HIWORD(wParam))
	{
	case CBN_SELENDOK:
	case CBN_SELENDCANCEL:
	/*case CBN_KILLFOCUS:*/
	case CBN_CLOSEUP:
	case CBN_DROPDOWN:
	  SendMessage (FrameTable[frame].Text_Zone, mMsg, wParam, lParam);
	  break;
	}
      if (LOWORD (wParam) >= TBBUTTONS_BASE)
	{
	  buttonCommand = TRUE;
	  APP_ButtonCallback (FrameTable[frame].Button[LOWORD (wParam) - TBBUTTONS_BASE], frame, "\n");
	}
      else
	{
	  buttonCommand = FALSE;
	  WIN_ThotCallBack (hwnd, wParam, lParam);
	}
      return 0L;

    case WM_CLOSE:
    case WM_DESTROY:
      if (frame >= 0 && frame <= MAX_FRAME)
	{
#ifdef _GL 
	  GL_DestroyFrame (frame);	
#endif /*_GL*/
	  GetDocAndView (frame, &pDoc, &view);
	  if (pDoc && view)
	    CloseView (pDoc, view);
	  if (FrameTable[frame].FrDoc == 0)
	    {
	      FrMainRef[frame] = 0;
	      DestroyWindow (hwnd);
	    }
	  if (mMsg == WM_DESTROY)
	    {
	      for (frameNDX = 1; frameNDX <= MAX_FRAME; frameNDX++)
		if (FrMainRef[frameNDX] != 0)
		  /* there is still an active frame */
		  break;
	      if (frameNDX > MAX_FRAME || FrMainRef[frameNDX] == 0)
		{
		  UnregisterClass ("Amaya",hInstance);
		  UnregisterClass ("ClientWndProc",hInstance);
		  UnregisterClass ("WNDIALOGBOX",hInstance);
		  if (mMsg == WM_DESTROY)
		    PostQuitMessage (0);
		  TtaQuit();
		}
	    }
	}
      if (mMsg == WM_DESTROY)
	PostQuitMessage (0);
      return 0L;
   
    case WM_SIZE:
      cx = LOWORD (lParam);
      cy = HIWORD (lParam);
      Wnd_ResizeContent (hwnd, cx, cy, frame);
      return 0;

#ifdef _GL
   case WM_EXITSIZEMOVE : 
      /* Some part of the Client Area has to be repaint. */	
	  DefWindowProc (hwnd, mMsg, wParam, lParam);
	  DefClip (frame, -1, -1, -1, -1);
	  if (GL_prepare (frame))
	  {
		RedrawFrameBottom (frame, 0, NULL);
		GL_Swap (frame);
	  }
      return 0;
#endif /*_GL*/

    default: 
      return (DefWindowProc (hwnd, mMsg, wParam, lParam));
    }
}
/* -------------------------------------------------------------------
   ClientWndProc
   ------------------------------------------------------------------- */
LRESULT CALLBACK ClientWndProc (HWND hwnd, UINT mMsg, WPARAM wParam, LPARAM lParam)
{
  char         DroppedFileName [MAX_PATH + 1];
  POINT        ptCursor;
  int          cx;
  int          cy;
  int          frame;
  int          status;
  int          key;
  int          key_menu;
  int          document, view;
  UINT         i, nNumFiles;
  RECT         rect;
  RECT         cRect;
  ThotBool     isSpecial, done;
  /* Used to limit selection extension
     on mouse move  */
  static int       oldXPos;
  static int       oldYPos;

  frame = GetFrameNumber (hwnd);  

  /* do not handle events if the Document is in NoComputedDisplay mode. */
  if (frame != -1)
    {
      /*ActiveFrame = frame;*/
      FrameToView (frame, &document, &view);
      if (documentDisplayMode[FrameTable[frame].FrDoc - 1] == NoComputedDisplay)
	     return (DefWindowProc (hwnd, mMsg, wParam, lParam));
      /*
       * If are waiting for the user to explicitely point to a document,
       * store the location and return.
       */
      if (ClickIsDone == 1 &&
	  (mMsg == WM_LBUTTONDOWN || mMsg == WM_RBUTTONDOWN))
	{
	  ClickIsDone = 0;
	  ClickFrame = frame;
	  ClickX = LOWORD (lParam);
	  ClickY = HIWORD (lParam);
	  return 0/*DefWindowProc (hwnd, mMsg, wParam, lParam)*/;
	}
    } 

  /* Handle the Scroll Drag */
  if (wParam & MK_LBUTTON && frame != -1)
    {
      /* if scroll concerns this window*/
      if (GetCapture () == hwnd)
	{
	  GetWindowRect (hwnd, &rect);
	  GetCursorPos (&ptCursor);	  
	  /* generate a scroll if necessary 
	     (cursor position outside client bounds)*/
	  if (ptCursor.y > rect.bottom ||
	      ptCursor.y < rect.top ||
	      ptCursor.x > rect.right ||
	      ptCursor.x < rect.left)
	    {
	      GetClientRect (hwnd, &cRect);
	      if (ptCursor.y > rect.bottom)
		{
		  Y_Pos = cRect.bottom;
		  TtcLineDown (document, view);
		}
	      else if (ptCursor.y < rect.top)
		{
		  Y_Pos = 0;
		  TtcLineUp (document, view);
		}
	      if (ptCursor.x > rect.right)
		{
		  X_Pos = cRect.right;
		  TtcScrollRight (document, view);
		}
	      else if (ptCursor.x < rect.left)
		{
		  X_Pos = 0;
		  TtcScrollLeft (document, view);
		}
	      LocateSelectionInView (frame, X_Pos, Y_Pos, 0);
	    }
	  else
	    {
	      /* Just Extending Selection*/
	      if (mMsg == WM_MOUSEMOVE)
		{	
		  X_Pos = LOWORD (lParam);
		  Y_Pos = HIWORD (lParam);
		  if ((oldXPos <= X_Pos - 1 || oldXPos >= X_Pos + 1) ||  
		      (oldYPos <= Y_Pos - 1 || oldYPos >= Y_Pos + 1))
		    {
		      LocateSelectionInView (frame, X_Pos, Y_Pos, 0);
		      oldXPos = X_Pos;
		      oldYPos = Y_Pos;
		    }
		}
	    }
	}
    }
  switch (mMsg)
    {
	case WM_ERASEBKGND:
      return 1;

    case WM_PAINT:
      /* Some part of the Client Area has to be repaint. */	
      WIN_HandleExpose (hwnd, frame, wParam, lParam);	   
      return 0;

    case WM_SIZE:
	  if (frame != -1)
	  {
		 /*disable focus selection on text zon */
		SendMessage (GetWindow(FrameTable[frame].Text_Zone, GW_CHILD),  
					EM_SETSEL, 1, 1);  
        cx         = LOWORD (lParam);
        cy         = HIWORD (lParam);
        WIN_ChangeViewSize (frame, cx, cy, 0, 0);
	  }
      return 0;

    case WM_CREATE:
      DragAcceptFiles (hwnd, TRUE);
      return 0;

    case WM_DROPFILES:
      nNumFiles = DragQueryFile ((HDROP)wParam, 0xFFFFFFFF, NULL, 0);
      if (FrameTable[frame].Call_Text != NULL)
	for (i = 0; i < nNumFiles; i++)
	  {
	    DragQueryFile ((HDROP)wParam, i, DroppedFileName, MAX_PATH + 1);
	    /* call the first text-zone entry with the current text */
	    (*FrameTable[frame].Call_Text) (document, view, DroppedFileName);
	  }
      DragFinish ((HDROP)wParam);
      return 0;

    case WM_SYSKEYDOWN:
    case WM_KEYDOWN:
      if (wParam == VK_F2     ||
	  wParam == VK_F3     ||
	  wParam == VK_F4     ||
	  wParam == VK_F5     ||
	  wParam == VK_F6     ||
	  wParam == VK_F7     ||
	  wParam == VK_F8     ||
	  wParam == VK_F9     ||
	  wParam == VK_F10    ||
	  wParam == VK_F11    ||
	  wParam == VK_F12    ||
	  wParam == VK_F13    ||
	  wParam == VK_F14    ||
	  wParam == VK_F15    ||
	  wParam == VK_F16    ||
	  (wParam >= 48 && wParam <= 57)      ||
	  wParam == VK_INSERT ||
	  wParam == VK_DELETE ||
	  wParam == VK_HOME   ||
	  wParam == VK_END    ||
	  wParam == VK_PRIOR  ||
	  wParam == VK_NEXT   ||
	  wParam == VK_LEFT   ||
	  wParam == VK_RIGHT  ||
	  wParam == VK_UP     ||
	  wParam == VK_DOWN)
	{
	  done = FALSE;
	  if (wParam >= 48 && wParam <= 57)
	    {
	      /* handling Ctrl 0-9 or Alt 0-9 */
	      key_menu = GetKeyState (VK_MENU);
  	      key = GetKeyState (VK_CONTROL);
	      /* is it an Alt-GR modifier? (WIN32 interprets this
		  as having both a control + menu key pressed down) */
	      if (HIBYTE (key_menu) && HIBYTE (key))
		    done = TRUE;
	      /* is a control key pressed? */
	      if (HIBYTE (key))
 		    isSpecial = FALSE;
	      else
		  {
		    /* is a menu key pressed? */
		    if (HIBYTE (key_menu))
		       isSpecial = FALSE;
		    else
		       /* don't handle a simple 0-9 */
		       done = TRUE;
		  }
	    }
	  else
	    isSpecial = TRUE;
	  if (!done)
	  {
	    key = (int) wParam;
	    done = WIN_CharTranslation (FrRef[frame], frame, mMsg, (WPARAM) key,
				        lParam, isSpecial);
	    if (done)
		    return 0;
	  }
	}
      break;
    case WM_SYSCHAR:
    case WM_CHAR:
    key = (int) wParam;
      done = WIN_CharTranslation (FrRef[frame], frame, mMsg, (WPARAM) key,
				  lParam, FALSE);
     
      if (done /*|| (GetKeyState (VK_MENU) && wParam == VK_SPACE)*/)
	  return 0;
      break;

#ifdef IME_INPUT
    case WM_IME_COMPOSITION:
      {
	HIMC hIMC = ImmGetContext(hwnd);
	if (lParam & GCS_RESULTSTR)
	  {
	    CHAR_T str[128];
	    int len, i;

	    len = ImmGetCompositionStringW(hIMC, GCS_RESULTSTR, NULL, 0);
	    /* need to check (len/2) < 128; len is strangely in bytes rather than wchars */
	    ImmGetCompositionStringW(hIMC, GCS_RESULTSTR, str, len);
	    ImmReleaseContext(hwnd, hIMC);
	    len /= 2;
	    /* actually move the characters to the document */
	    for (i=0; i<len; i++)
	      InsertChar(frame, str[i], -1);
	    return 0;
	  }
      }
      break;
#endif /* IME_INPUT */

    case WM_LBUTTONDOWN:
      /* Activate the client window */
      SetFocus (FrRef[frame]);
      SetCapture (hwnd);
      /* stop any current insertion of text in the old frame */
      ActiveFrame = ClickFrame;
      CloseTextInsertion ();
      ClickFrame = frame;
      ActiveFrame = frame;
      oldXPos = ClickX = LOWORD (lParam);
      oldYPos = ClickY = HIWORD (lParam);
      status = GetKeyState (VK_SHIFT);
      if (HIBYTE (status))
	LocateSelectionInView (frame, ClickX, ClickY, 0);
      else
	{
	  status = GetKeyState (VK_CONTROL);
	  if (HIBYTE (status))
	    /* changes the box position */
	    ApplyDirectTranslate (frame, ClickX, ClickY);
	  /* This is the beginning of a selection */
	  else
	    LocateSelectionInView (frame, ClickX, ClickY, 2);
	}
      return 0;

    case WM_LBUTTONUP:
      X_Pos = LOWORD (lParam);
      Y_Pos = HIWORD (lParam);
      if (GetCapture () == hwnd)
	{
	  ReleaseCapture ();
	  /*End Selection*/
	  LocateSelectionInView (frame, ClickX, ClickY, 4);
	}
      return 0;

    case WM_LBUTTONDBLCLK:
      /* left double click handling */
      ReturnOption = -1;
      /* get the mouse position */
      ClickFrame = frame;
      ClickX     = LOWORD (lParam);
      ClickY     = HIWORD (lParam);
      LocateSelectionInView (frame, ClickX, ClickY, 3);
      return 0;
      
    case WM_MBUTTONDOWN:
      ClickFrame = frame;
      ClickX = LOWORD (lParam);
      ClickY = HIWORD (lParam);
      /* stop any current insertion of text */
      CloseTextInsertion ();
      
      /* if the CTRL key is pressed this is a size change */
      status = GetKeyState (VK_CONTROL);
      if (HIBYTE (status))
	{
	  /* changes the box size */
	  ApplyDirectResize (frame, ClickX, ClickY);
	  /* memorize the click position */
	}
      else
	LocateSelectionInView (frame, ClickX, ClickY, 6);
      return 0;

    case WM_RBUTTONDOWN:
      ClickFrame = frame;
      ClickX = LOWORD (lParam);
      ClickY = HIWORD (lParam);
      /* stop any current insertion of text */
      CloseTextInsertion ();
		    
      /* if the CTRL key is pressed this is a size change */
      status = GetKeyState (VK_CONTROL);
      if (HIBYTE (status))
	{
	  /* changes the box size */
	  /* ApplyDirectResize (frame, LOWORD (lParam), HIWORD (lParam)); */
	  ApplyDirectResize (frame, ClickX, ClickY);
	  /* memorize the click position */
	}
      else
	LocateSelectionInView (frame, ClickX, ClickY, 6);
      return 0;

    case WM_MBUTTONDBLCLK:
    case WM_RBUTTONDBLCLK:
      /* left double click handling */
       break;


    case WM_NCMOUSEMOVE:
      /* Mouse move outside client area*/
       break;
    case WM_MOUSEMOVE:
      /* Mouse move inside client area*/
       break;

    default:
      break;
    }

  return (DefWindowProc (hwnd, mMsg, wParam, lParam));
}
#endif /* _WINGUI */

/*----------------------------------------------------------------------
  GtkLiningSelection 

  When user hold clicked a mouse button in order to extend a selection
  and scroll in the meantime,
  those functions are called by a timer each 100 ms
  in order to repeat user action until he released the button
  or move away from the widget.
  ----------------------------------------------------------------------*/
#if defined(_GTK) || defined(_WX)
  static ThotBool     Selecting = FALSE;
#endif /* defined(_GTK) || defined(_WX) */

#ifdef _GTK
gboolean GtkLiningSelection (gpointer data)
{
  Document            doc; 
  ViewFrame          *pFrame;
  int                 frame;
  int                 view;
  static int          Motion_y = 0;
  static int          Motion_x = 0;
  GdkModifierType state = (GdkModifierType)GDK_BUTTON1_MOTION_MASK;
  int x,y;
  
  frame = (int) data;
  if (frame < 1)
    return FALSE;

  gdk_window_get_pointer ((FrameTable[frame].WdFrame)->window, 
			  &x, 
			  &y, 
			  &state);
  if (x != Motion_x || y != Motion_y)
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
#else /* _GL */
	    if (pFrame->FrAbstractBox &&
		pFrame->FrAbstractBox->AbBox &&
		pFrame->FrYOrg + FrameTable[frame].FrHeight < pFrame->FrAbstractBox->AbBox->BxClipH)
#endif /* _GL */
	      TtcLineDown (doc, view);
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
	  TtcCopyToClipboard (doc, view);
	}
      /* As this is a timeout function, return TRUE so that it
	 continues to get called */
      return Selecting;
    }
  else
    return TRUE;   
  
}
#endif /* _GTK */

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
ThotBool FrameButtonDownCallback( 
    int frame,
    int thot_button_id,
    int thot_mod_mask,
    int x, int y )
{
#ifdef _WX
  Document   document;
  View       view;
 
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
      /* stop any current insertion of text */
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
	LocateSelectionInView (frame, x, y, 0);
#ifndef _WINDOWS
	FrameToView (frame, &document, &view);
	TtcCopyToClipboard (document, view);
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
ThotBool FrameButtonUpCallback( 
    int frame,
    int thot_button_id,
    int thot_mod_mask,
    int x, int y )
{
#ifdef _WX
  /* if a button release, we save the selection in the clipboard */
  /* drag is finished */
  /* we stop the callback calling timer */
/*  Selecting = FALSE;
    if (timer != None)
  {
  Document   document;
  View       view;
    gtk_timeout_remove (timer);
    timer = None;
    FrameToView (frame, &document, &view);
    TtcCopyToClipboard (document, view);
  }
  else */

  Document   document;
  View       view;

  if ( Selecting )
    {
      Selecting = FALSE;
#ifndef _WINDOWS
      FrameToView (frame, &document, &view);
      TtcCopyToClipboard (document, view);
#endif /* _WINDOWS */
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
ThotBool FrameButtonDClickCallback( 
    int frame,
    int thot_button_id,
    int thot_mod_mask,
    int x, int y )
{
#ifdef _WX
  Document   document;
  View       view;

  switch (thot_button_id)
  {
    case THOT_LEFT_BUTTON:
    {
      ClickFrame = frame;
      ClickX = x;
      ClickY = y;
      LocateSelectionInView (frame, ClickX, ClickY, 3);
#ifndef _WINDOWS
      /* a word is probably selected, copy it into clipboard */
      FrameToView (frame, &document, &view);
      TtcCopyToClipboard (document, view);
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
ThotBool FrameMotionCallback( 
    int frame,
    int thot_mod_mask,
    int x, int y )
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
#ifndef _WINDOWS
	      TtcCopyToClipboard (doc, view);
#endif /* _WINDOWS */
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
  int                 frame;
  GtkEntry            *textzone;
  static int          timer = None;
  Document            document;
  View                view;

  frame = (int )data;
#ifdef _GL
  GL_prepare (frame);  
#endif /* _GL */
  frame = (int )data;
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
	      LocateSelectionInView (frame, (int)event->x, (int)event->y, 0);
	      FrameToView (frame, &document, &view);
	      TtcCopyToClipboard (document, view);
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
	  TtcCopyToClipboard (document, view);
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
  CHAR_T             *ptr;
#ifdef _WX
  AmayaFrame         *p_frame;
#else /* _WX */
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
  s = (unsigned char *)TtaGetMemory (strlen ((const char *)title) + strlen (HTAppVersion) + 10);
  sprintf ((char *)s, "%s - Amaya %s", title, HTAppVersion);
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
  Document            doc;

  if (ActiveFrame != frame)
    {
      /* check if a new document is selected */
      if (ActiveFrame == 0)
	doc = 0;
      else
	doc = FrameTable[frame].FrDoc;
      ActiveFrame = frame;
    }
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
#ifdef _GL
  *width = FrameTable[frame].FrWidth;
  *height = FrameTable[frame].FrHeight;
#else /*_GL*/
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
#endif /*_GL*/
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
	  wxLogDebug( _T("glClear CLEAR_VALUE(%f,%f,%f,%f) - frame=%d"),tmp[0],tmp[1],tmp[2],tmp[3], frame );
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
  /*
   * SG : there is a missing feature in wxWidgets, it's not possible to parameter step increment.
   * It's why when clicking up/down on the scrollbar, the increment is 1pixel.
   * I've patched wxWidgets to fix the step increment to 8, better than 1 ...
   */
  if (width < l)
  {
    FrameTable[frame].WdFrame->ShowScrollbar(2);
    FrameTable[frame].WdScrollH->SetScrollbar( x, width, l, width );
  }
  else
    FrameTable[frame].WdFrame->HideScrollbar(2);    

  if (height < h)
  {
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
  need_resize = FALSE;
  l = FrameTable[frame].FrWidth;
  h = FrameTable[frame].FrHeight;
  memset(&scrollInfo, 0, sizeof(scrollInfo));	
  scrollInfo.cbSize = sizeof (SCROLLINFO);
  scrollInfo.fMask  = SIF_PAGE | SIF_POS | SIF_RANGE;
  if (width >= l && x == 0 && width > 60)
    {
      /*hide*/
      if (Win_Scroll_visible (FrameTable[frame].WdScrollH))
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
    /*show*/
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
#endif /* _WINGUI */
}
