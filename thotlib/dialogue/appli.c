/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2003
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

#ifdef _WINDOWS
  #include "winsys.h"
  #include "wininclude.h"

  #ifdef WM_MOUSELAST
    #ifndef WM_MOUSEWHEEL
      #define WM_MOUSEWHEEL WM_MOUSELAST+1 
    #endif /*WM_MOUSEWHEEL*/
  #endif /*WM_MOUSELAST*/
#endif /* _WINDOWS */

#ifdef _MOTIF
  #define MAX_ARGS 20
  static Time         T1, T2, T3;
  static XmString     null_string;
  static ThotBool     JumpInProgress = FALSE;
#endif /* _MOTIF */

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
  #include "AmayaFrame.h"
#endif /* _WX */

#ifdef _GTK
  #include "gtk-functions.h"
#endif /* _GTK */

#ifdef _MOTIF
  #include "input_f.h"
#endif /* _MOTIF */

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


/* defined into amaya directory ...*/
extern void ZoomIn (Document document, View view);
extern void ZoomOut (Document document, View view);


#ifdef _WINDOWS
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
#endif /* _WINDOWS */

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

  TtaQuit();
  return FALSE;
}

#ifdef _WINDOWS
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

 if (frame > 0 && frame <= MAX_FRAME)
   {
#ifndef _GL
     /* Do not redraw if the document is in NoComputedDisplay mode. */
     if (documentDisplayMode[FrameTable[frame].FrDoc - 1] != NoComputedDisplay &&
	 GetUpdateRect (w, &rect, FALSE))
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
	 DefRegion (frame, rect.left, rect.top, rect.right, rect.bottom);
	 EndPaint (w, &ps);
	 DisplayFrame (frame);
	 /* restore the previous clipping */
	 pFrame = &ViewFrameTable[frame - 1];
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
       DefRegion (frame, 0,  0, width, height);
       RebuildConcreteImage (frame);
       GL_Swap (frame);
     }
#endif/*_GL*/
   /* recompute the scroll bars */
  /*UpdateScrollbars (frame); Done in rebuildconcreteimage, no ?*/
}
#endif /* _WINDOWS */

/*----------------------------------------------------------------------
  XFlushOutput enforce updating of the calculated image for frame.
  ----------------------------------------------------------------------*/
void XFlushOutput (int frame)
{
#ifdef _MOTIF
   XFlush (TtDisplay);
#endif /* _MOTIF */
}

/*----------------------------------------------------------------------
   FrameToRedisplay effectue le traitement des expositions X11 des     
   frames de dialogue et de documents.                   
  ----------------------------------------------------------------------*/
void FrameToRedisplay (ThotWindow w, int frame, void *ev)
{
#if defined(_MOTIF) || defined(_GTK)
  XExposeEvent       *event = (XExposeEvent *) ev;
  ViewFrame          *pFrame;
  int                 xmin, xmax, ymin, ymax;
  int                 x, y, l, h;

  x = event->x;
  y = event->y;
  l = event->width;
  h = event->height;
  if (frame > 0 && frame <= MAX_FRAME && FrameTable[frame].FrDoc > 0 &&
      /* don't handle a document in mode NoComputedDisplay */
      documentDisplayMode[FrameTable[frame].FrDoc - 1] != NoComputedDisplay)
    {
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
      DefRegion (frame, x, y, x + l, y + h);
      RedrawFrameBottom (frame, 0, NULL);
      /* restore the previous clipping */
      pFrame = &ViewFrameTable[frame - 1];
      pFrame->FrClipXBegin = xmin;
      pFrame->FrClipXEnd = xmax;
      pFrame->FrClipYBegin = ymin;
      pFrame->FrClipYEnd = ymax;
    }
#endif /* #if defined(_MOTIF) || defined(_GTK) */
}


/*----------------------------------------------------------------------
  FrameRedraw
  ----------------------------------------------------------------------*/
void FrameRedraw (int frame, unsigned int width, unsigned int height)
{
#if defined(_MOTIF) || defined(_GTK) || defined(_WX)
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
#endif /* #if defined(_MOTIF) || defined(_GTK) || defined(_WX) */
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

  if (Printing || frame != Shared_Context)
    return;
  for (i = 0 ; i <= MAX_FRAME; i++)
    {  
#if defined(_MOTIF) || defined(_GTK) || defined(_WX)
      if (i != Shared_Context && FrameTable[i].WdFrame)
#endif /*#if defined(_MOTIF) || defined(_GTK) || defined(_WX) */
#ifdef _WINDOWS
      if (i != Shared_Context && GL_Context[i])
#endif /* _WINDOWS */
	{
    	  Shared_Context = i;
	  /* stop the loop */
	  i = MAX_FRAME + 1;
	} 
    }
#endif /*_NOSHARELIST*/
  FreeAllPicCacheFromFrame (frame);
#ifdef _WINDOWS
  /* make our context 'un-'current */
  /*wglMakeCurrent (NULL, NULL);*/
  /* delete the rendering context */
  if (GL_Context[frame])
    wglDeleteContext (GL_Context[frame]);
  /*if (GL_Windows[frame])*/
    /*ReleaseDC (hwndClient, GL_Windows[frame]);*/
  GL_Windows[frame] = 0;
  GL_Context[frame] = 0;
#endif /* _WINDOWS */
}

#ifdef _WINDOWS
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
#endif /* _WINDOWS */
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

static ThotBool  FrameResizedGTKInProgress = FALSE;
/*----------------------------------------------------------------------
  FrameResizedGTK When user resize window
  ----------------------------------------------------------------------*/
gboolean FrameResizedGTK (GtkWidget *w, GdkEventConfigure *event, gpointer data)
{
  int            frame;
  int            width, height;
#ifdef IV
  int         forever = 0;
#endif /* IV */
 
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
  if (w)
    if (GL_prepare (frame))
      {
	/* prevent flickering*/
	GL_SwapStop (frame); 
	FrameTable[frame].FrWidth = width;
	FrameTable[frame].FrHeight = height;
	GLResize (width, height, 0, 0);
	DefRegion (frame, 0, 0, width, height);
        FrameRedraw (frame, width, height);
#ifdef IV
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
		DefRegion (frame, 0, 0, width, height);
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
	  DefRegion (frame, 0, 0, 
		     FrameTable[frame].FrWidth, FrameTable[frame].FrHeight); 
	  RedrawFrameBottom (frame, 0, NULL);
	}
	GL_Swap (frame);
    }
#else /* _GL */
      DefRegion (frame, x, y, x + w, y + h);
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
ThotBool FrameExposeCallback (
    int frame,
    int x,
    int y,
    int w,
    int h )
{
  if (w <= 0 || h <= 0 || frame == 0 || frame > MAX_FRAME)
    return FALSE;
  if (FrameTable[frame].FrDoc == 0 ||
      documentDisplayMode[FrameTable[frame].FrDoc - 1] == NoComputedDisplay)
    return FALSE; 
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
	  DefRegion( frame, 0, 0, 
		     FrameTable[frame].FrWidth, FrameTable[frame].FrHeight ); 
	  RedrawFrameBottom( frame, 0, NULL );
	}
	GL_Swap( frame );
    }
#else /* _GL */
      DefRegion( frame, x, y, x + w, y + h );
      RedrawFrameBottom( frame, 0, NULL );
#endif /* _GL */
  return TRUE;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void FrameResized (int *w, int frame, int *info)
{
#ifdef _MOTIF
  int                 n;
  unsigned short      width, height;
  Arg                 args[MAX_ARGS];

  n = 0;
  XtSetArg (args[n], XmNwidth, &width);
  n++;
  XtSetArg (args[n], XmNheight, &height);
  n++;
  XtGetValues ((ThotWidget) w, args, n);

  FrameRedraw (frame, width, height);
#endif /* _MOTIF */
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
ThotBool FrameResizedCallback(
    	int frame,
    	int new_width,
	int new_height )
{
  /* check if the frame is valide */
  if ( new_width <= 0 ||
       new_height <= 0 || 
       frame == 0 ||
       frame > MAX_FRAME ||
       FrameTable[frame].FrDoc == 0 ||
       documentDisplayMode[FrameTable[frame].FrDoc - 1] == NoComputedDisplay ||
       ( FrameTable[frame].FrWidth == new_width &&
	 FrameTable[frame].FrHeight == new_height)
     )
  {
    // frame should not be displayed
    return FALSE;
  }

#ifdef _GL
  if (GL_prepare( frame ))
  {
    /* prevent flickering*/
    GL_SwapStop( frame );
    FrameTable[frame].FrWidth = new_width;
    FrameTable[frame].FrHeight = new_height;
    GLResize( new_width, new_height, 0, 0 );
    DefRegion( frame, 0, 0, new_width, new_height );
    FrameRedraw( frame, new_width, new_height );
//    FrameTable[frame].DblBuffNeedSwap = TRUE; 
    GL_SwapEnable( frame );
    GL_Swap( frame );
//    FrameTable[frame].DblBuffNeedSwap = TRUE;  
  }
#else /* _GL*/
  FrameRedraw( frame, new_width, new_height );
#endif /* _GL */
  return TRUE;
}

#ifdef _WINDOWS
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
#endif /* _WINDOWS */

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
 * FrameHScrolledGTK
 * is the GTK wrapper of FrameHScrolledCallback
  ----------------------------------------------------------------------*/
#if defined(_GTK)
void FrameHScrolledGTK (GtkAdjustment *w, int frame)
{
  FrameHScrolledCallback( frame, w->value, w->page_size );
}
#endif /* #if defined(_GTK) */ 

/*----------------------------------------------------------------------
  FrameHScrolled is the motif version
  ----------------------------------------------------------------------*/
void FrameHScrolled (int *w, int frame, int *param)
{
#if defined(_MOTIF)
  int                 delta, l;
  int                 view;
#ifdef _MOTIF
  int                 n;
  Arg                 args[MAX_ARGS];
  XmScrollBarCallbackStruct *info;
#endif /* _MOTIF */
  NotifyWindow        notifyDoc;
  Document            doc;

  /* ne pas traiter si le document est en mode NoComputedDisplay */
  if (FrameTable[frame].FrDoc &&
      documentDisplayMode[FrameTable[frame].FrDoc - 1] == NoComputedDisplay)
    return;

#ifdef _MOTIF
  info = (XmScrollBarCallbackStruct *) param;
  if (info->reason == XmCR_DECREMENT)
    /* Deplacement en arriere d'un caractere de la fenetre */
    delta = -13;
  else if (info->reason == XmCR_INCREMENT)
    /* Deplacement en avant d'un caractere de la fenetre */
    delta = 13;
  else if (info->reason == XmCR_PAGE_DECREMENT)
    /* Deplacement en arriere du volume de la fenetre */
    delta = -FrameTable[frame].FrWidth;
  else if (info->reason == XmCR_PAGE_INCREMENT)
    /* Deplacement en avant du volume de la fenetre */
    delta = FrameTable[frame].FrWidth;
  else
    delta = MAX_SIZE;		/* indeterminee */
#endif /* _MOTIF */ 
  
#ifdef _GTK 
  /* delta is the position into the page */
  delta = (int)w->value;
#endif  /* _GTK */

  notifyDoc.event = TteViewScroll;
  FrameToView (frame, &doc, &view);
  notifyDoc.document = doc;
  notifyDoc.view = view;
  notifyDoc.verticalValue = 0;
  notifyDoc.horizontalValue = delta;
  if (!CallEventType ((NotifyEvent *) & notifyDoc, TRUE))
    {
#ifdef _MOTIF
      if (info->reason == XmCR_VALUE_CHANGED || info->reason == XmCR_DRAG)
	{
	  /* On recupere la largeur de l'ascenseur */
	  n = 0;
	  XtSetArg (args[n], XmNsliderSize, &l);
	  n++;
	  XtGetValues (FrameTable[frame].WdScrollH, args, n);
	  /* On regarde si le deplacement bute sur le bord droit */
	  if (info->value + l >= FrameTable[frame].FrWidth)
	      delta = FrameTable[frame].FrScrollWidth;
	    else
	      {
		/* translate the position in the scroll bar into a shift value in the document */
		delta = (int) ((float) (info->value * FrameTable[frame].FrScrollWidth) / (float) FrameTable[frame].FrWidth);
		delta = delta + FrameTable[frame].FrScrollOrg - ViewFrameTable[frame - 1].FrXOrg;
	      }
	}
      else if (info->reason == XmCR_TO_TOP)
	/* force the left alignment */
	delta = -FrameTable[frame].FrScrollWidth;
      else if (info->reason == XmCR_TO_BOTTOM)
	/* force the right alignment */
	delta = FrameTable[frame].FrScrollWidth;
#endif /* _MOTIF */

#ifdef _GTK 
       /* l is the width of the page */
       l = (int)w->page_size;
       /* On regarde si le deplacement bute sur le bord droit */
       if (w->value + l >= FrameTable[frame].FrWidth)	     
	 delta = FrameTable[frame].FrScrollWidth;
       else
	 {
	   /* translate the position in the scroll bar into a shift value in the document */
	   delta = (int) ((float) (w->value * FrameTable[frame].FrScrollWidth) / (float) FrameTable[frame].FrWidth);
	   delta = delta + FrameTable[frame].FrScrollOrg - ViewFrameTable[frame - 1].FrXOrg;
	 }
#endif /* _GTK */

       if (delta)
	 HorizontalScroll (frame, delta, 1);
       notifyDoc.document = doc;
       notifyDoc.view = view;
       notifyDoc.verticalValue = 0;
       notifyDoc.horizontalValue = delta;
       CallEventType ((NotifyEvent *) & notifyDoc, FALSE);
    }
#endif /* #if defined(_MOTIF) */  
} 

/*----------------------------------------------------------------------
  Demande de scroll vertical motif version.
  ----------------------------------------------------------------------*/
void FrameVScrolled (int *w, int frame, int *param)
{
#ifdef _MOTIF
  int                 delta;
  int                 h, y;
  int                 start, end, total;
  int                 n;
  int                 view;
  Arg                 args[MAX_ARGS];
  XmScrollBarCallbackStruct *infos;
  float               carparpix;
  NotifyWindow        notifyDoc;
  Document            doc;

  /* ne pas traiter si le document est en mode NoComputedDisplay */
  if (FrameTable[frame].FrDoc &&
      documentDisplayMode[FrameTable[frame].FrDoc - 1] == NoComputedDisplay)
    return;
  FrameToView (frame, &doc, &view);
  infos = (XmScrollBarCallbackStruct *) param;
  if (infos->reason == XmCR_DECREMENT)
    /* Deplacement en arriere d'un caractere de la fenetre */
    delta = -13;
  else if (infos->reason == XmCR_INCREMENT)
    /* Deplacement en avant d'un caractere de la fenetre */
    delta = 13;
  else if (infos->reason == XmCR_PAGE_DECREMENT)
    /* Deplacement en arriere du volume de la fenetre */
    delta = -FrameTable[frame].FrHeight;
  else if (infos->reason == XmCR_PAGE_INCREMENT)
    /* Deplacement en avant du volume de la fenetre */
    delta = FrameTable[frame].FrHeight;
  else
    delta = MAX_SIZE;		/* indeterminee */
  notifyDoc.event = TteViewScroll;
  notifyDoc.document = doc;
  notifyDoc.view = view;
  notifyDoc.verticalValue = delta;
  notifyDoc.horizontalValue = 0;
  if (!CallEventType ((NotifyEvent *) & notifyDoc, TRUE))
    {
      if (infos->reason == XmCR_VALUE_CHANGED ||
	  infos->reason == XmCR_DRAG)
	{
	  /* Deplacement absolu dans la vue du document */
	  delta = infos->value;
	  /* Recupere la hauteur de l'ascenseur */
	  n = 0;
	  XtSetArg (args[n], XmNsliderSize, &h);
	  n++;
	  XtGetValues (FrameTable[frame].WdScrollV, args, n);
	  /* Regarde ou se situe l'image abstraite dans le document */
	  n = PositionAbsBox (frame, &start, &end, &total);
	  /* au retour n = 0 si l'image est complete */
	  /* Calcule le nombre de caracteres represente par un pixel */
	  carparpix = (float) total / (float) FrameTable[frame].FrHeight;
	  y = (int) ((float) infos->value * carparpix);
	  if (n == 0 || (y >= start && y <= total - end))
	    {
	      /* On se deplace a l'interieur de l'image Concrete */
	      /* Calcule la portion de scroll qui represente l'image Concrete */
	      start = (int) ((float) start / carparpix);
	      end = (int) ((float) end / carparpix);
	      delta = FrameTable[frame].FrHeight - start - end;
	      /* Calcule la position demandee dans cette portion de scroll */
	      /* On detecte quand le deplacement bute en bas du document */
	      if (infos->value + h >= FrameTable[frame].FrHeight)
		y = delta;
	      else
		y = infos->value - start;
	      ShowYPosition (frame, y, delta);
	    }
	  else if (!JumpInProgress)
	    {
	      JumpInProgress = TRUE;
	      /* On regarde si le deplacement bute en bas du document */
	      if (delta + h >= FrameTable[frame].FrHeight - 4)
		delta = FrameTable[frame].FrHeight;
	      else if (delta >= 4)
		/* Ou plutot vers le milieu */
		delta += h / 2;
	      else
		delta = 0;
	      delta = (delta * 100) / FrameTable[frame].FrHeight;
	      JumpIntoView (frame, delta);
	      /* recompute the scroll bars */
	      UpdateScrollbars (frame);
	      JumpInProgress = FALSE;
	    }
	}
      else if (infos->reason == XmCR_TO_TOP)
	{
	  /* go to the document beginning */
	  JumpIntoView (frame, 0);
	  /* recompute the scroll bars */
	  UpdateScrollbars (frame);
	}
      else if (infos->reason == XmCR_TO_BOTTOM)
	{
	  /* go to the document end */
	  JumpIntoView (frame, 100);
	  /* recompute the scroll bars */
	  UpdateScrollbars (frame);
	}
      else if (delta)
	VerticalScroll (frame, delta, 1);
      notifyDoc.document = doc;
      notifyDoc.view = view;
      notifyDoc.verticalValue = delta;
      notifyDoc.horizontalValue = 0;
      CallEventType ((NotifyEvent *) & notifyDoc, FALSE);
    }
#endif /* _MOTIF */  
}

/*----------------------------------------------------------------------
 * FrameVScrolledGTK
 * is the GTK wrapper of FrameVScrolledCallback
  ----------------------------------------------------------------------*/
#ifdef _GTK
void FrameVScrolledGTK (GtkAdjustment *w, int frame)
{
  FrameVScrolledCallback (frame, w->value);
/*
  int        delta, x, y, width, height, viewed, left;

  if (FrameTable[frame].FrDoc &&
      documentDisplayMode[FrameTable[frame].FrDoc - 1] == NoComputedDisplay)
    return;
  ComputeDisplayedChars (frame, &x, &y, &width, &height);
  delta = (int) w->value - y;
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
      delta = (int) ((w->value / (float)FrameTable[frame].FrHeight) * 100);
      JumpIntoView (frame, delta);
    }
    */
}
#endif /*_GTK*/

/*----------------------------------------------------------------------
 * FrameVScrolledCallback (generic callback)
 * is called when the scrollbar position is changed
 * params :
 *   + int position : the new scrollbar position
 *   + int frame : the concerned frame
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
#ifdef _MOTIF
  XmScrollBarCallbackStruct infos;
#endif /* _MOTIF */
#if defined(_WINDOWS) || defined(_GTK) || defined(_WX)
  int                       delta;
#endif /* defined(_WINDOWS) || defined(_GTK) || defined(_WX) */
  int                       frame;
  
  if (document != 0)
    frame = GetWindowNumber (document, view);
  else
    frame = 0;

#ifdef _MOTIF
  infos.reason = XmCR_DECREMENT;
  FrameVScrolled (0, frame, (int *) &infos);
#endif /* _MOTIF */

#if defined(_WINDOWS) || defined(_GTK) || defined(_WX)
  delta = -13;
  VerticalScroll (frame, delta, 1);
#endif /*#if defined(_WINDOWS) || defined(_GTK) || defined(_WX) */
}

/*----------------------------------------------------------------------
  TtcLineDown scrolls one line down.                                
  ----------------------------------------------------------------------*/
void TtcLineDown (Document document, View view)
{
#ifdef _MOTIF
  XmScrollBarCallbackStruct infos;
#endif /* _MOTIF */
#if defined(_WINDOWS) || defined(_GTK) || defined(_WX)
  int                       delta;
#endif /* #if defined(_WINDOWS) || defined(_GTK) || defined(_WX) */
  int                       frame;

  if (document != 0)
    frame = GetWindowNumber (document, view);
  else
    frame = 0;

#ifdef _MOTIF
  infos.reason = XmCR_INCREMENT;
  FrameVScrolled (0, frame, (int *) &infos);
#endif /* _MOTIF */
  
#if defined(_WINDOWS) || defined(_GTK) || defined(_WX)
  delta = 13;
  VerticalScroll (frame, delta, 1);
#endif /* #if defined(_WINDOWS) || defined(_GTK) || defined(_WX)*/
}

/*----------------------------------------------------------------------
  TtcScrollLeft scrolls one position left.                                    
  ----------------------------------------------------------------------*/
void TtcScrollLeft (Document document, View view)
{
#ifdef _MOTIF
  XmScrollBarCallbackStruct infos;
#endif /* _MOTIF */
#if defined(_WINDOWS) || defined(_GTK) || defined(_WX)
  int                       delta;
#endif /* #if defined(_WINDOWS) || defined(_GTK) || defined(_WX) */
  int                       frame;
  
  if (document != 0)
    frame = GetWindowNumber (document, view);
  else
    frame = 0;

#ifdef _MOTIF
  infos.reason = XmCR_DECREMENT;
  FrameHScrolled (0, frame, (int *) &infos);
#endif /* _MOTIF */

#if defined(_WINDOWS) || defined(_GTK) || defined(_WX)
  delta = -13;
  HorizontalScroll (frame, delta, 1);
#endif /* #if defined(_WINDOWS) || defined(_GTK) || defined(_WX) */
}

/*----------------------------------------------------------------------
  TtcScrollRight scrolls one position right.                                
  ----------------------------------------------------------------------*/
void TtcScrollRight (Document document, View view)
{
#ifdef _MOTIF
  XmScrollBarCallbackStruct infos;
#endif /* _MOTIF */
#if defined(_WINDOWS) || defined(_GTK) || defined(_WX)
  int                       delta;
#endif /* #if defined(_WINDOWS) || defined(_GTK) || defined(_WX) */
  int                       frame;

  if (document != 0)
    frame = GetWindowNumber (document, view);
  else
    frame = 0;

#ifdef _MOTIF
  infos.reason = XmCR_INCREMENT;
  FrameHScrolled (0, frame, (int *) &infos);
#endif /* _MOTIF */

#if defined(_WINDOWS) || defined(_GTK) || defined(_WX) 
  delta = 13;
  HorizontalScroll (frame, delta, 1);
#endif /* #if defined(_WINDOWS) || defined(_GTK) || defined(_WX) */
}

/*----------------------------------------------------------------------
  PageUp scrolls one screen up.                                    
  ----------------------------------------------------------------------*/
void TtcPageUp (Document document, View view)
{
#ifdef _MOTIF
  XmScrollBarCallbackStruct infos;
#endif /* _MOTIF */
#if defined(_WINDOWS) || defined(_GTK) || defined(_WX)
  int                       delta;
#endif  /* #if defined(_WINDOWS) || defined(_GTK) || defined(_WX) */
  int                       frame;

  if (document != 0)
    frame = GetWindowNumber (document, view);
  else
    frame = 0;
#ifdef _MOTIF
  infos.reason = XmCR_PAGE_DECREMENT;
  FrameVScrolled (0, frame, (int *) &infos);
#endif /* _MOTIF */
  
#if defined(_WINDOWS) || defined(_GTK) || defined(_WX)
  delta = -FrameTable[frame].FrHeight;
  VerticalScroll (frame, delta, 1);
#endif /*#if defined(_WINDOWS) || defined(_GTK) || defined(_WX) */
}

/*----------------------------------------------------------------------
  PageDown scrolls one screen down.                                
  ----------------------------------------------------------------------*/
void TtcPageDown (Document document, View view)
{
#ifdef _MOTIF
  XmScrollBarCallbackStruct infos;
#endif /* _MOTIF */
#if defined(_WINDOWS) || defined(_GTK) || defined(_WX)
  int                       delta;
#endif  /* #if defined(_WINDOWS) || defined(_GTK) || defined(_WX) */
  int                        frame;

  if (document != 0)
    frame = GetWindowNumber (document, view);
  else
    frame = 0;
#ifdef _MOTIF
  infos.reason = XmCR_PAGE_INCREMENT;
  FrameVScrolled (0, frame, (int *) &infos);
#endif /* _MOTIF */
  
#if defined(_WINDOWS) || defined(_GTK) || defined(_WX)
  delta = FrameTable[frame].FrHeight;
  VerticalScroll (frame, delta, 1);
#endif /* #if defined(_WINDOWS) || defined(_GTK) || defined(_WX) */
}


/*----------------------------------------------------------------------
  PageTop goes to the document top.                                
  ----------------------------------------------------------------------*/
void TtcPageTop (Document document, View view)
{
#ifdef _MOTIF
  XmScrollBarCallbackStruct infos;
#endif /* _MOTIF */
  int                        frame;

  if (document != 0)
    frame = GetWindowNumber (document, view);
  else
    frame = 0;

#ifdef _MOTIF
  infos.reason = XmCR_TO_TOP;
  FrameVScrolled (0, frame, (int *) &infos);
#endif /* _MOTIF */
  
#if defined(_WINDOWS) || defined(_GTK) || defined(_WX)
  JumpIntoView (frame, 0);
#endif /* #if defined(_WINDOWS) || defined(_GTK) || defined(_WX) */
}

/*----------------------------------------------------------------------
  PageEnd goes to the document end.                                
  ----------------------------------------------------------------------*/
void TtcPageEnd (Document document, View view)
{
#ifdef _MOTIF
  XmScrollBarCallbackStruct infos;
#endif /* _MOTIF */
  int                        frame;

  if (document != 0)
    frame = GetWindowNumber (document, view);
  else
    frame = 0;

#ifdef _MOTIF
  infos.reason = XmCR_TO_BOTTOM;
  FrameVScrolled (0, frame, (int *) &infos);
#endif /* _MOTIF */
  
#if defined(_WINDOWS) || defined(_GTK) || defined(_WX)
  JumpIntoView (frame, 100);
#endif /* #if defined(_WINDOWS) || defined(_GTK) || defined(_WX) */
}




/*----------------------------------------------------------------------
  InitializeOtherThings initialise les contextes complementaires.     
  ----------------------------------------------------------------------*/
void InitializeOtherThings ()
{
  int                 i;

  /* Initialisation de la table des widgets de frames */
  for (i = 0; i <= MAX_FRAME; i++)
    {
#ifndef _WX
      FrameTable[i].WdFrame = 0;
      FrameTable[i].FrDoc = 0;
#endif /* #ifndef _WX */

#ifdef _WX
      /* fill with 0 all the fields */
      memset( &FrameTable[i], 0, sizeof(Frame_Ctl) );
#endif /* _WX */      
    }

  ClickIsDone = 0;
  ClickFrame = 0;
  ClickX = 0;
  ClickY = 0;
  /* message de selection vide */
#ifdef _MOTIF
  null_string = XmStringCreateSimple ("");
#endif /* _MOTIF */
  
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
  int                 idwindow;
  ThotWidget          w;

  UserErrorCode = 0;
  idwindow = GetWindowNumber (document, view);
  if (idwindow != 0)
    {
#ifndef _WX // TODO      
      w = FrameTable[idwindow].WdFrame;
#endif //#ifndef _WX // TODO
      
#ifdef _MOTIF
      if (w != 0)
	XMapRaised (TtDisplay, XtWindowOfObject (XtParent (XtParent (XtParent (w)))));
#endif /* _MOTIF */
      
#ifdef _GTK
      if (w != 0)
	gdk_window_show (gtk_widget_get_parent_window(GTK_WIDGET(w)));
#endif /* _GTK */

#ifdef _WINDOWS
      {
	OpenIcon (FrMainRef[idwindow]);
	SetForegroundWindow (FrMainRef[idwindow]);
      }
#endif /* _WINDOWS */
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
#ifdef _MOTIF
  Arg                 args[MAX_ARGS];
  XmString            title_string;
#endif /* _MOTIF */

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
	if (FrameTable[frame].WdStatus != 0)
	  {
	    length = strlen (text) + 1;
	    if (name)
	      length += strlen (name);
	    s = (char *)TtaGetMemory (length);
#ifdef _WINDOWS
	    if (name)
	      /* text est un format */
	      sprintf (s, text, name);
	    else
	      strncpy (s, text, length);

	    SendMessage (FrameTable[frame].WdStatus, SB_SETTEXT, (WPARAM) 0, (LPARAM) s);
	    SendMessage (FrameTable[frame].WdStatus, WM_PAINT, (WPARAM) 0, (LPARAM) 0);
#endif /* _WINDOWS */

#ifdef _MOTIF
	    if (name)
	      {
		/* text est un format */
		sprintf (s, text, name);
		title_string = XmStringCreateSimple (s);
	      }
	    else
	      title_string = XmStringCreateSimple (text);
	    XtSetArg (args[0], XmNlabelString, title_string);
	    XtSetValues (FrameTable[frame].WdStatus, args, 1);
	    XtManageChild (FrameTable[frame].WdStatus);
	    XmStringFree (title_string);
#endif /* _MOTIF */

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
	    wxCSConv conv_ascii(_T("ISO-8859-1")); 
	    if (name)
	    {
	      /* text est un format */
	      sprintf (s, text, name);
	      FrameTable[frame].WdStatus->SetStatusText(
	        wxString((char *)s, conv_ascii) );
	    }
	    else
              FrameTable[frame].WdStatus->SetStatusText(
                wxString((char *)name, conv_ascii) );
#endif /* _WX */
	    
      TtaFreeMemory (s);
	  }
      
    }
}



#ifdef _WINDOWS
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
  if (IsWindowVisible (WinToolBar[frame]))
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


  if (Win_Scroll_visible (FrameTable[frame].WdScrollV)) 
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

  if (Win_Scroll_visible (FrameTable[frame].WdScrollH)) 
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
      ToolBar = CreateWindow (TOOLBARCLASSNAME, NULL, dwToolBarStyles,
			      0, 0, 0, 0, hwnd, (HMENU) 1, hInstance, 0);

      SendMessage (ToolBar, TB_BUTTONSTRUCTSIZE, (WPARAM) sizeof (TBBUTTON), 0L);
      if ((SendMessage (ToolBar, TB_ADDBITMAP, (WPARAM) MAX_BUTTON,
			(LPARAM) (LPTBADDBITMAP) &ThotTBBitmap)) == -1)
	WinErrorBox (NULL, "WndProc: WM_CREATE cannot create toolbar");
    
      hwndToolTip = ToolBar_GetToolTips (ToolBar);
      if (dwToolBarStyles & TBSTYLE_TOOLTIPS)
	InitToolTip (ToolBar);	
    
      /* Create status bar  */
      dwStatusBarStyles = WS_CHILD | WS_VISIBLE | CCS_BOTTOM | SBARS_SIZEGRIP;
      StatusBar = CreateStatusWindow (dwStatusBarStyles, "", hwnd, 2);
      ShowWindow (StatusBar, SW_SHOWNORMAL);
      UpdateWindow (StatusBar); 
      hwndClient = 0;
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
      /*SetFocus (FrRef [frame]);
      ActiveFrame = frame;*/
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
		/*disable focus selection on text zon */
		SendMessage (GetWindow(FrameTable[frame].Text_Zone, GW_CHILD),  
					EM_SETSEL,     1,      1);  
      cx         = LOWORD (lParam);
      cy         = HIWORD (lParam);
      WIN_ChangeViewSize (frame, cx, cy, 0, 0);
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
      CloseInsertion ();
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
      CloseInsertion ();
      
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
      CloseInsertion ();
		    
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
#endif /* _WINDOWS */

#ifdef _GTK
/*----------------------------------------------------------------------
  GtkLiningSelection 

  When user hold clicked a mouse button in order to extend a selection
  and scroll in the meantime,
  those functions are called by a timer each 100 ms
  in order to repeat user action until he released the button
  or move away from the widget.
  ----------------------------------------------------------------------*/
static ThotBool     Selecting = FALSE;

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
  Document   document;
  View       view;
 
  /* drag is finished */
/*  if (timer != None)
  {
    // TODO : gtk_timeout_remove (timer);
    timer = None;
    Selecting = FALSE;
  }
  */

  switch( thot_button_id )
  {
    case THOT_LEFT_BUTTON:
    {
      /* stop any current insertion of text */
      CloseInsertion ();

      /* Est-ce que la touche modifieur de geometrie est active ? */
      if (thot_mod_mask & THOT_MOD_CTRL)
      {
	/* moving a box */     
	ApplyDirectTranslate (frame, x, y);
      }
      else if (thot_mod_mask & THOT_MOD_SHIFT)
      {
	/* a selection extension */
	TtaAbortShowDialogue ();
	LocateSelectionInView (frame, x, y, 0);
	FrameToView (frame, &document, &view);
	TtcCopyToClipboard (document, view);
      }
      else
      {
	/* a simple selection */
	ClickFrame = frame;
	ClickX = x;
	ClickY = y;
	LocateSelectionInView (frame, ClickX, ClickY, 2);
/* TODO	m_Selecting = TRUE; */
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
  Document   document;
  View       view;

  /* if a button release, we save the selection in the clipboard */
  /* drag is finished */
  /* we stop the callback calling timer */
/*  Selecting = FALSE;
    if (timer != None)
  {
    gtk_timeout_remove (timer);
    timer = None;
    FrameToView (frame, &document, &view);
    TtcCopyToClipboard (document, view);
  } 
  else */
  if (thot_button_id == THOT_LEFT_BUTTON)
  {
    ClickFrame = frame;
    ClickX = x;
    ClickY = y;
    LocateSelectionInView (frame, ClickX, ClickY, 4);
    TtaAbortShowDialogue ();
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
ThotBool FrameButtonDClickCallback( 
    int frame,
    int thot_button_id,
    int thot_mod_mask,
    int x, int y )
{
  switch (thot_button_id)
  {
    case THOT_LEFT_BUTTON:
    {
      ClickFrame = frame;
      ClickX = x;
      ClickY = y;
      LocateSelectionInView (frame, ClickX, ClickY, 3);
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
  Document   document;
  View       view;

  if (direction > 0)
  {
    // wheel mice up
    FrameToView (frame, &document, &view);
    if (thot_mod_mask & THOT_MOD_CTRL)
    {
      // if CTRL is down then zoom in
      ZoomIn (document, view);
    }
    else
    { 
      VerticalScroll (frame, -39, 1);
    }
  }
  else
  {
    // wheel mice down
    FrameToView (frame, &document, &view); 
    if (thot_mod_mask & THOT_MOD_CTRL)
    {
      ZoomOut (document, view);	   
    }
    else
    {
      VerticalScroll (frame, 39, 1);
    }
  }	
  return TRUE;
}

/*----------------------------------------------------------------------
  Evenement sur une frame document.                              
  D.V. equivalent de la fontion MS-Windows ci dessus !        
  GTK: fonction qui traite les click de la souris sauf la selection   
  ----------------------------------------------------------------------*/
#if defined(_MOTIF) || defined(_GTK)
#ifdef _MOTIF
void FrameCallback (int frame, void *evnt)
#endif /* _MOTIF */
#ifdef _GTK
gboolean FrameCallbackGTK (GtkWidget *widget, GdkEventButton *event, gpointer data)
#endif /* _GTK */
#else /* #if defined(_MOTIF) || defined(_GTK) */
void FrameCallback (int frame, void *evnt)
#endif /* #if defined(_MOTIF) || defined(_GTK) */
{
#if defined(_MOTIF) || defined(_GTK)
#ifdef _MOTIF
  ThotEvent           event;
  ThotEvent          *ev = (ThotEvent *) evnt;
  int                 comm, dx, dy, sel, h;
#endif /* _MOTIF */
#ifdef _GTK
  int                 frame;
  GtkEntry            *textzone;
  static int          timer = None;
#endif /* _GTK */
  Document            document;
  View                view;

#ifdef _GTK
  frame = (int )data;
#endif /* _GTK */

#ifdef _GL
  GL_prepare (frame);  
#endif /* _GL */

#ifdef _MOTIF
  if (FrameTable[frame].FrDoc == 0 ||
      documentDisplayMode[FrameTable[frame].FrDoc - 1] == NoComputedDisplay)
    /* don't manage a document with NoComputedDisplay mode */
    return;
  else if (ev == NULL)
    return;
  else if (ClickIsDone == 1 && ev->type == ButtonPress)
    /* Amaya is waiting for a click selection */
    {
      ClickIsDone = 0;
      ClickFrame = frame;
      ClickX = (int)ev->xbutton.x;
      ClickY = (int)ev->xbutton.y;
      return;
    }
  else if (TtaTestWaitShowDialogue ()
      && (ev->type != ButtonPress || (ev->xbutton.state & THOT_KEY_ControlMask) == 0))
    /* a TtaWaitShowDialogue in progress, don't change the selection */
    return;
#endif /* _MOTIF */
  
#ifdef _GTK
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
    /* a TtaWaitShowDialogue in progress, don't change the selection */
    return FALSE;
  /* 
     Set the drawing area Focused
     By setting the focus on the text zone
     If another action specifically need focus, 
     w'ell grab it with the action
  */
  textzone = (GtkEntry*)gtk_object_get_data (GTK_OBJECT (widget), "Text_catcher");
  gtk_widget_grab_focus (GTK_WIDGET(textzone));  
#endif /* _GTK */

#ifdef _MOTIF
  switch (ev->type)
    {
    case ButtonPress:
      switch (ev->xbutton.button)
	{
	case Button1:
	  /* stop any current insertion of text */
	  CloseInsertion ();
	  /* ==========LEFT BUTTON========== */	  
	  /* Est-ce que la touche modifieur de geometrie est active ? */
	  if ((ev->xbutton.state & THOT_KEY_ControlMask) != 0)
	    {
	      /* moving a box */
	      ApplyDirectTranslate (frame, (int)ev->xbutton.x, (int)ev->xbutton.y);
	      T1 = T2 = T3 = 0;
	    }
	  else if ((ev->xbutton.state & THOT_KEY_ShiftMask) != 0)
	    {
	      /* a selection extension */
	      TtaAbortShowDialogue ();
	      LocateSelectionInView (frame, ev->xbutton.x, ev->xbutton.y, 0);
	      FrameToView (frame, &document, &view);
	      TtcCopyToClipboard (document, view);
	      T1 = T2 = T3 = 0;
	    }
	  else if (T1 + (Time) DoubleClickDelay > ev->xbutton.time)
	    {
	      /* double click */
	      TtaAbortShowDialogue ();
	      TtaFetchOneEvent (&event);
	      while (event.type != ButtonRelease)
		{
		  TtaHandleOneEvent (&event);
		  TtaFetchOneEvent (&event);
		}

	      /* register the cursor position */
	      if (ClickFrame == frame &&
		  (ClickX-ev->xbutton.x < 3 || ClickX-ev->xbutton.x > 3) &&
		  (ClickY-ev->xbutton.y < 3 || ClickY-ev->xbutton.y > 3))
		/* it's really a double click */
		sel = 3;
	      else
		sel = 2;
	      ClickFrame = frame;
	      ClickX = ev->xbutton.x;
	      ClickY = ev->xbutton.y;
	      LocateSelectionInView (frame, ClickX, ClickY, sel);
	    }
	  else
	    {
	      /* a simple selection */
	      T1 = ev->xbutton.time;
	      ClickFrame = frame;
	      ClickX = ev->xbutton.x;
	      ClickY = ev->xbutton.y;
	      LocateSelectionInView (frame, ClickX, ClickY, 2);
	      /* is it a drag or a simple selection? */
	      comm = 0;	/* il n'y a pas de drag */
	      TtaFetchOneEvent (&event);
	      FrameToView (frame, &document, &view);
	      h = FrameTable[frame].FrHeight;
	      while (event.type != ButtonRelease &&
		     event.type != ButtonPress)
		{
		  if (event.type == MotionNotify ||
		      (event.type != ConfigureNotify &&
		       event.type != MapNotify &&
		       event.type != UnmapNotify &&
		       event.type != DestroyNotify &&
		       /*event.type != NoExpose && */
		       (event.xmotion.y > h || event.xmotion.y < 0)))
		    {
		      dx = event.xmotion.x - ClickX;
		      dy = event.xmotion.y - ClickY;
		      if (dx > 1 || dx < -1 || dy > 1 || dy < -1 ||
			  event.xmotion.y > h || event.xmotion.y < 0)
			{
			  LocateSelectionInView (frame, event.xbutton.x, event.xbutton.y, 1);
			  comm = 1;	/* il y a un drag */
			  /* generate a scroll if necessary */
			  if (event.xmotion.y > h)
			    TtcLineDown (document, view);
			  else if (event.xmotion.y <= 1)
			    TtcLineUp (document, view);
			  if (FrameTable[frame].FrScrollWidth > FrameTable[frame].FrWidth)
			    {
			      if (event.xmotion.x > FrameTable[frame].FrWidth)
				TtcScrollRight (document, view);
			      else if (event.xmotion.x <= 1)
				TtcScrollLeft (document, view);
			    }
			}
		    }
		  TtaHandleOneEvent (&event);
		  TtaFetchOrWaitEvent (&event);
		}
	      TtaHandleOneEvent (&event);
	       
	      /* S'il y a un drag on termine la selection */
	      FrameToView (frame, &document, &view);
	      if (comm == 1)
		LocateSelectionInView (frame, event.xbutton.x, event.xbutton.y, 0);
	      else if (comm == 0)
		/* click event */
		LocateSelectionInView (frame, event.xbutton.x, event.xbutton.y, 4);
	      
	      if (comm != 0)
		TtcCopyToClipboard (document, view);
	    }
	  break;
	case Button2:
	  /* ==========MIDDLE BUTTON========== */
	  if ((ev->xbutton.state & THOT_KEY_ControlMask) != 0)
	    {
	      /* resizing a box */
	      ApplyDirectResize (frame, (int)ev->xbutton.x, (int)ev->xbutton.y);
	      T1 = T2 = T3 = 0;
	    }
	  else
	    {
	      if (T2 + (Time) DoubleClickDelay > ev->xbutton.time)
		{
		  /* double click */
		  TtaFetchOneEvent (&event);
		  while (event.type != ButtonRelease)
		    {
		      TtaHandleOneEvent (&event);
		      TtaFetchOneEvent (&event);
		    }
		  /* register the cursor position */
		  if (ClickFrame == frame &&
		      (ClickX - ev->xbutton.x < 3 ||
		       ClickX - ev->xbutton.x > 3) &&
		      (ClickY - ev->xbutton.y < 3 ||
		       ClickY-ev->xbutton.y > 3))
		    /* ignore double clicks */
		    return;
		}
	      /* handle a simple selection */
	      T2 = ev->xbutton.time;
	      ClickFrame = frame;
	      ClickX = ev->xbutton.x;
	      ClickY = ev->xbutton.y;
	      LocateSelectionInView (frame, ClickX, ClickY, 5);
	    }
	  break;
	case Button3:
	  /* stop any current insertion of text */
	  CloseInsertion ();
	  /* ==========RIGHT BUTTON========== */
	  if ((ev->xbutton.state & THOT_KEY_ControlMask) != 0)
	    {
	      /* resize a box */
	      ApplyDirectResize (frame, (int)ev->xbutton.x, (int)ev->xbutton.y);
	      T1 = T2 = T3 = 0;
	    }
	  else
	    {
	      if (T3 + (Time) DoubleClickDelay > ev->xbutton.time)
		{
		  /* double click */
		  TtaFetchOneEvent (&event);
		  while (event.type != ButtonRelease)
		    {
		      TtaHandleOneEvent (&event);
		      TtaFetchOneEvent (&event);
		    }
		  /* register the cursor position */
		  if (ClickFrame == frame &&
		      (ClickX - ev->xbutton.x < 3 ||
		       ClickX - ev->xbutton.x > 3) &&
		      (ClickY - ev->xbutton.y < 3 ||
		       ClickY-ev->xbutton.y > 3))
		    /* ignore double clicks */
		    return;
		}
	      /* handle a simple selection */
	      T3 = ev->xbutton.time;
	      ClickFrame = frame;
	      ClickX = ev->xbutton.x;
	      ClickY = ev->xbutton.y;
	      LocateSelectionInView (frame, ClickX, ClickY, 6);
	    }
	  break;
	default:
	  break;
	}
      break;
    case KeyPress:
      T1 = T2 = T3 = 0;
      TtaAbortShowDialogue ();
      CharTranslation ((ThotKeyEvent *)ev);
      break;
    default:
      break;
    }
#endif /* _MOTIF */
  
#ifdef _GTK
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
	  CloseInsertion ();
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
#endif /* _GTK */

#endif /* #if defined(_MOTIF) || defined(_GTK) */  
}

#ifdef _GTK
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
#ifdef _MOTIF
  XGrabPointer (TtDisplay, win, FALSE, events, GrabModeAsync, GrabModeAsync,
		win, cursor, CurrentTime);
#endif /* _MOTIF */
  
#ifdef _GTK
  gdk_pointer_grab (win, FALSE, (GdkEventMask)events, win, cursor, GDK_CURRENT_TIME);
#endif /* _GTK */
}


/*----------------------------------------------------------------------
  ThotUngrab est une fonction d'interface pour UnGrab.            
  ----------------------------------------------------------------------*/
void ThotUngrab ()
{
#ifdef _MOTIF
  XUngrabPointer (TtDisplay, CurrentTime);
#endif /* _MOTIF */
  
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
#if defined(_GTK) || defined(_MOTIF)

  Drawable            drawable;
#ifdef _GTK
  int                 frame;
  ThotWidget          w;
#endif /* _GTK */

  
  drawable = (Drawable)TtaGetThotWindow (thotThotWindowid);
#ifdef _MOTIF
  XDefineCursor (TtDisplay, drawable, WaitCurs);
#endif /* #ifdef _MOTIF */

#ifdef _GTK
  for (frame = 1; frame <= MAX_FRAME; frame++)
    {
      w = FrameTable[frame].WdFrame;
      if (w != NULL)
	if (w->window != NULL)
	  gdk_window_set_cursor (GTK_WIDGET(w)->window, WaitCurs);
    }
#endif /* _GTK */
  
#endif /* #if defined(_GTK) || defined(_MOTIF) */

#ifdef _WINDOWS
  SetCursor (LoadCursor (NULL, IDC_WAIT));
  ShowCursor (TRUE);
#endif /* #ifdef _WINDOWS */  
}

/*----------------------------------------------------------------------
  ResetCursorWatch enleve le curseur "montre".                 
  ----------------------------------------------------------------------*/
void ResetCursorWatch (int thotThotWindowid)
{  
#if defined(_GTK) || defined(_MOTIF)
  Drawable            drawable;
#ifdef _GTK
  int                 frame;
  ThotWidget          w;
#endif /* _GTK */

  drawable = (Drawable) TtaGetThotWindow (thotThotWindowid);
#ifdef _MOTIF
  XUndefineCursor (TtDisplay, drawable);
#endif /* _MOTIF */
  
#ifdef _GTK
  for (frame = 1; frame <= MAX_FRAME; frame++)
    {
      w = FrameTable[frame].WdFrame;
      if (w != NULL)
	if (w->window != NULL)
	  gdk_window_set_cursor (GTK_WIDGET(w)->window, ArrowCurs);
    }
#endif /* _GTK */
  
#endif /* #if defined(_GTK) || defined(_MOTIF) */

#ifdef _WINDOWS
  ShowCursor (FALSE);
  SetCursor (LoadCursor (NULL, IDC_ARROW));
#endif /* _WINDOWS */
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void TtaSetCursorWatch (Document document, View view)
{
  int                 frame;
#if defined(_GTK) || defined(_MOTIF)
  Drawable            drawable;
#ifdef _GTK
  ThotWidget          w;
#endif /* _GTK */
#endif  /* #if defined(_GTK) || defined(_MOTIF) */

  UserErrorCode = 0;
  /* verifie le parametre document */
  if (document == 0 && view == 0)
    {
      for (frame = 1; frame <= MAX_FRAME; frame++)
	{
#if defined(_GTK) || defined(_MOTIF)
	  drawable = (Drawable)TtaGetThotWindow (frame);
	  if (drawable != 0)
#ifdef _MOTIF
	    XDefineCursor (TtDisplay, drawable, WaitCurs);
#endif /* _MOTIF */
#ifdef _GTK
	  {
	    w = FrameTable[frame].WdFrame;
	    gdk_window_set_cursor (GTK_WIDGET(w)->window, WaitCurs);
	  }
#endif /* _GTK */
#endif /* #if defined(_GTK) || defined(_MOTIF) */
	}
    }
  else
    {
      frame = GetWindowNumber (document, view);
#if defined(_GTK) || defined(_MOTIF)
      if (frame != 0)
#ifdef _MOTIF
	XDefineCursor (TtDisplay, TtaGetThotWindow (frame), WaitCurs);
#endif /* _MOTIF */
#ifdef _GTK
      {
	w = FrameTable[frame].WdFrame;
	gdk_window_set_cursor (GTK_WIDGET(w)->window, WaitCurs);
      }
#endif /* _GTK */
#endif /* #if defined(_GTK) || defined(_MOTIF) */
    }
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void TtaResetCursor (Document document, View view)
{
  int                 frame;
#if defined(_GTK) || defined(_MOTIF)
  Drawable            drawable;
#ifdef _GTK
  ThotWidget          w;
#endif /* _GTK */
#endif /* #if defined(_GTK) || defined(_MOTIF) */

  UserErrorCode = 0;
  /* verifie le parametre document */
  if (document == 0 && view == 0)
    {
      for (frame = 1; frame <= MAX_FRAME; frame++)
	{
#if defined(_GTK) || defined(_MOTIF)
	  drawable = (Drawable)TtaGetThotWindow (frame);
	  if (drawable != 0)
#ifdef _MOTIF
	    XUndefineCursor (TtDisplay, drawable);
#endif /* _MOTIF */
#ifdef _GTK
	  {
	    w = FrameTable[frame].WdFrame;
	    if (w != NULL)
	      if (w->window != NULL)
		gdk_window_set_cursor(GTK_WIDGET(w)->window, ArrowCurs);
	  }  
#endif /* _GTK */
#endif /* #if defined(_GTK) || defined(_MOTIF) */
	}
    }
  else
    {
      frame = GetWindowNumber (document, view);
#if defined(_GTK) || defined(_MOTIF)
      if (frame != 0)
#ifdef _MOTIF
	XUndefineCursor (TtDisplay, TtaGetThotWindow (frame));
#endif /* _MOTIF */
#ifdef _GTK
      {
	w = FrameTable[frame].WdFrame;
	if (w != NULL)
	  if (w->window != NULL)
	    gdk_window_set_cursor(GTK_WIDGET(w)->window, ArrowCurs);
      }  
#endif /* _GTK */
#endif /* #if defined(_GTK) || defined(_MOTIF) */
    }
}

/*----------------------------------------------------------------------
  GiveClickedAbsBox returns the window and the abstract box selected.
  ----------------------------------------------------------------------*/
void GiveClickedAbsBox (int *frame, PtrAbstractBox *pAb)
{
#if defined(_GTK) || defined(_MOTIF)
  ThotEvent           event;
  Drawable            drawable;
  int                 i;
#ifdef _GTK   
  ThotWidget          w;
#endif /* _GTK */
#endif /* #if defined(_GTK) || defined(_MOTIF) */

#ifdef _WINDOWS
  MSG                 event;
  HCURSOR             cursor;          
  int                 curFrame;
#endif /* _WINDOWS */

  if (ClickIsDone == 1)
    {
      *frame = 0;
      *pAb = NULL;
    }

  /* Change the cursor */
#ifdef _WINDOWS
  cursor = LoadCursor (hInstance, MAKEINTRESOURCE (Window_Curs));
#endif  /* _WINDOWS */
  
#if defined(_GTK) || defined(_MOTIF)  
  for (i = 1; i <= MAX_FRAME; i++)
    {
      drawable = (Drawable)TtaGetThotWindow (i);
      if (drawable)
#ifdef _MOTIF
	XDefineCursor (TtDisplay, drawable, WindowCurs);
#endif /* _MOTIF */
#ifdef _GTK
      {
	w = FrameTable[i].WdFrame;
	if (w && w->window)
	  gdk_window_set_cursor(GTK_WIDGET(w)->window, WindowCurs);
      }
#endif /* _GTK */
    }
#endif /* #if defined(_GTK) || defined(_MOTIF) */

  /* wait the click on the target */
  ClickIsDone = 1;
  ClickFrame = 0;
  ClickX = 0;
  ClickY = 0;
  while (ClickIsDone == 1)
    {
#if defined(_GTK) || defined(_MOTIF)  
      TtaFetchOneEvent (&event);
      TtaHandleOneEvent (&event);
#endif /* #if defined(_GTK) || defined(_MOTIF) */
      
#ifdef _WINDOWS
      GetMessage (&event, NULL, 0, 0);
      curFrame = GetFrameNumber (event.hwnd);
      TtaHandleOneEvent (&event);
      SetCursor (cursor);
#endif /* _WINDOWS */
    }

#if defined(_GTK) || defined(_MOTIF)  
  /* Restore the cursor */
  for (i = 1; i <= MAX_FRAME; i++)
    {
      drawable = (Drawable)TtaGetThotWindow (i);
      if (drawable)
#ifdef _MOTIF
	XUndefineCursor (TtDisplay, drawable);
#endif /* _MOTIF */
      
#ifdef _GTK

      {
	w = FrameTable[i].WdFrame;  
	if (w != NULL)
	  if (w->window != NULL)
	    gdk_window_set_cursor(GTK_WIDGET(w)->window, ArrowCurs);
      }
#endif /* _GTK */
    }
#endif /* #if defined(_GTK) || defined(_MOTIF) */

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
  unsigned char      *title;
#if defined(_GTK) || defined(_MOTIF)  
  ThotWidget          w;
#endif /* #if defined(_GTK) || defined(_MOTIF) */

#ifdef _MOTIF
  int                 n;
  Arg                 args[MAX_ARGS];
#endif /* _MOTIF */

#ifdef _I18N_
  CHAR_T             *ptr;

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
#else /* _I18N_ */
  title = text;
#endif /* _I18N_ */

#ifdef _WINDOWS
  if (FrMainRef [frame])
    SetWindowText (FrMainRef[frame], title);
#endif /* _WINDOWS */

#if defined(_GTK) || defined(_MOTIF)  
  w = FrameTable[frame].WdFrame;
  if (w)
    {
#ifdef _GTK
      w = gtk_widget_get_toplevel (w);
      gtk_window_set_title (GTK_WINDOW(w), (gchar *)title);
#endif /* _GTK */
#ifdef _MOTIF
      w = XtParent (XtParent (XtParent (w)));
      n = 0;
      XtSetArg (args[n], XmNtitle, title);
      n++;
      XtSetArg (args[n], XmNiconName, title);
      n++;
      XtSetValues (w, args, n);
#endif /* _MOTIF */
    }
#endif /* #if defined(_GTK) || defined(_MOTIF) */

#if defined(_WX)
  AmayaFrame * p_frame = FrameTable[frame].WdFrame;
  if ( p_frame )
    {
      // used to convert text format
      wxCSConv conv_ascii(_T("ISO-8859-1")); 
      p_frame->SetTitle( wxString((char *)title,conv_ascii) );
    }
#endif /* #if defined(_WX) */
  
  if (title != text)
    TtaFreeMemory (title);
}

/*----------------------------------------------------------------------
  La frame d'indice frame devient la fenetre active.               
  ----------------------------------------------------------------------*/
void ChangeSelFrame (int frame)
{
#ifndef _WX // TODO
  ThotWidget          w;
  Document            doc;

  if (ActiveFrame != frame)
    {
      /* check if a new document is selected */
      if (ActiveFrame == 0)
	doc = 0;
      else
	doc = FrameTable[frame].FrDoc;
      ActiveFrame = frame;
      if (frame != 0 && FrameTable[frame].FrDoc != doc)
	{
	  /* raise the new document */
	  w = FrameTable[frame].WdFrame;
#ifdef _MOTIF
	  if (w != 0)
	    XMapRaised (TtDisplay, XtWindowOfObject (XtParent (XtParent (XtParent (w)))));
#endif /* _MOTIF */
	}
    }
#endif //#ifndef _WX // TODO
  
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

#ifdef _WINDOWS
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
#endif /* _WINDOWS */

/*----------------------------------------------------------------------
  GetSizesFrame retourne les dimensions de la fenetre d'indice frame.        
  ----------------------------------------------------------------------*/
void GetSizesFrame (int frame, int *width, int *height)
{
#ifdef _GL
  *width = FrameTable[frame].FrWidth;
  *height = FrameTable[frame].FrHeight;

#else /*_GL*/

#if defined(_GTK) || defined(_MOTIF) || defined(_WX) 
  *width = FrameTable[frame].FrWidth;
  *height = FrameTable[frame].FrHeight;
#endif /* #if defined(_GTK) || defined(_MOTIF) || defined(_WX) */
  
#ifdef _WINDOWS
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
#endif /* _WINDOWS */

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
#if defined(_GTK) || defined(_MOTIF)   
#ifdef _GTK
  GdkRectangle      rect;
#endif /* _GTK */
#ifdef _MOTIF
  XRectangle        rect;
#endif /* _MOTIF */
#endif /* #if defined(_GTK) || defined(_MOTIF) */
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
      
#ifdef _WINDOWS
      if (!(clipRgn = CreateRectRgn (clipx, clipy, 
				     clipx + clipwidth, clipy + clipheight)))
	WinErrorBox (NULL, "DefineClipping");
#endif  /* _WINDOWS */ 
      
#if defined(_GTK) || defined(_MOTIF)
      
#ifdef _GTK 
      rect.x = clipx;
      rect.y = clipy;
      rect.width = clipwidth;
      rect.height = clipheight;
      gdk_gc_set_clip_rectangle (TtLineGC, &rect);	
      gdk_gc_set_clip_rectangle (TtGreyGC, &rect);
      gdk_gc_set_clip_rectangle (TtGraphicGC, &rect);
#endif /* _GTK */
      
#ifdef _MOTIF
      rect.x = 0;
      rect.y = 0;
      rect.width = clipwidth;
      rect.height = clipheight;
      XSetClipRectangles (TtDisplay, TtLineGC, clipx,
			  clipy + FrameTable[frame].FrTopMargin, &rect, 1, Unsorted);
      XSetClipRectangles (TtDisplay, TtGreyGC, clipx,
			  clipy + FrameTable[frame].FrTopMargin, &rect, 1, Unsorted);
      XSetClipRectangles (TtDisplay, TtGraphicGC, clipx,
			  clipy + FrameTable[frame].FrTopMargin, &rect, 1, Unsorted);
#endif /* _MOTIF */
      
#endif /* #if defined(_GTK) || defined(_MOTIF)  */
      
      if (raz > 0)
	Clear (frame, clipwidth, clipheight, clipx, clipy);
#else /* _GL */
      GL_SetClipping (clipx,
		      FrameTable[frame].FrHeight + FrameTable[frame].FrTopMargin
		      - (clipy + clipheight),
		      clipwidth,
		      clipheight); 

      if (raz > 0 && GL_prepare (frame))
	glClear (GL_COLOR_BUFFER_BIT);
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
  
#ifdef _MOTIF
  XRectangle          rect;

  rect.x = 0;
  rect.y = 0;
  rect.width = MAX_SIZE;
  rect.height = MAX_SIZE;
  XSetClipRectangles (TtDisplay, TtLineGC, 0, 0, &rect, 1, Unsorted);
  XSetClipRectangles (TtDisplay, TtGraphicGC, 0, 0, &rect, 1, Unsorted); 
  XSetClipRectangles (TtDisplay, TtGreyGC, 0, 0, &rect, 1, Unsorted);
  XFlushOutput (frame);
#endif /* _MOTIF */

#ifdef _WINDOWS
  SelectClipRgn(TtDisplay, NULL); 
  if (clipRgn && !DeleteObject (clipRgn))
    WinErrorBox (NULL, "RemoveClipping");
  clipRgn = (HRGN) 0;
#endif /* _WINDOWS */

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

#ifdef _MOTIF
  Arg                 args[MAX_ARGS];
  int                 n;
#endif /* _MOTIF */
#ifdef _GTK
  GtkAdjustment      *tmpw;  
#endif /* _GTK */ 
#ifdef _WINDOWS
  SCROLLINFO          scrollInfo;
  ThotBool            need_resize;
#endif /* _WINDOWS */

  if (FrameUpdating ||
      documentDisplayMode[FrameTable[frame].FrDoc - 1] 
      == NoComputedDisplay)
    return;

#ifdef _GL
  if (FrameTable[frame].Scroll_enabled == FALSE)
    return;
#endif /* _GL */
  
  /* Get the displayed volume */
  ComputeDisplayedChars (frame, &x, &y, &width, &height);
  hscroll = FrameTable[frame].WdScrollH;
  vscroll = FrameTable[frame].WdScrollV;
  if (hscroll == NULL || vscroll == NULL)
    return;

#if defined(_GTK) || defined(_MOTIF) || defined(_WX)

  l = FrameTable[frame].FrWidth;
  h = FrameTable[frame].FrHeight;

#ifdef _WX
  if (width < l)
  {
//    FrameTable[frame].WdFrame->m_pFlexSizer->Remove( hscroll );
//    FrameTable[frame].WdFrame->m_pFlexSizer->Remove( vscroll );
//    FrameTable[frame].WdFrame->m_pFlexSizer->Add( vscroll, 1, wxEXPAND );
//    FrameTable[frame].WdFrame->m_pFlexSizer->Add( hscroll, 1, wxEXPAND );
//    FrameTable[frame].WdFrame->m_pFlexSizer->Layout();
    hscroll->SetScrollbar( x, width, l, width );
    hscroll->Show( true );
  }
  else
  {
    hscroll->Show( false );
//    FrameTable[frame].WdFrame->m_pFlexSizer->Remove(hscroll);
//    FrameTable[frame].WdFrame->m_pFlexSizer->Layout();
  }
  if (height < h)
  {
    vscroll->SetScrollbar( y, height, h, height );
    vscroll->Show( true );
  }
  else
    vscroll->Show( false );
#endif /*_WX*/
  
#ifdef _MOTIF
  if (width + x <= l)
    {
      n = 0;
      XtSetArg (args[n], XmNminimum, 0);n++;
      XtSetArg (args[n], XmNmaximum, l);n++;
      XtSetArg (args[n], XmNvalue, x);n++;
      XtSetArg (args[n], XmNsliderSize, width);n++;
      XtSetValues (hscroll, args, n);
    }
  if (height + y <= h)
    {
      n = 0;
      XtSetArg (args[n], XmNminimum, 0);n++;
      XtSetArg (args[n], XmNmaximum, h);n++;
      XtSetArg (args[n], XmNvalue, y);n++;
      XtSetArg (args[n], XmNsliderSize, height);n++;
      XtSetValues (vscroll, args, n);
    }
#endif /* _MOTIF */

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

#endif /* #if defined(_GTK) || defined(_MOTIF) || defined(_WX) */

#ifdef _WINDOWS
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
#endif /* _WINDOWS */
}
