/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2002
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

#else /* _WINDOWS */
#ifndef _GTK
#define MAX_ARGS 20
static Time         T1, T2, T3;
static XmString     null_string;
static ThotBool     JumpInProgress = FALSE;

#else /* _GTK */
#ifdef _GL
#include <gtkgl/gtkglarea.h>
#endif /*_GL*/
static gchar *null_string;
#endif /*_GTK*/
#endif /* _WINDOWS */

static char         OldMsgSelect[MAX_TXT_LEN];
static PtrDocument  OldDocMsgSelect;

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "boxes_tv.h"
#include "font_tv.h"
#include "edit_tv.h"
#include "frame_tv.h"
#include "appdialogue_tv.h"
#include "platform_tv.h"
#include "thotcolor_tv.h"

#include "appdialogue_f.h"
#include "inites_f.h"
#include "uconvert_f.h"
#include "picture_f.h"

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


/*----------------------------------------------------------------------
   WIN_GetDeviceContext selects a Device Context for a given
   thot window.                                                
  ----------------------------------------------------------------------*/
void WIN_GetDeviceContext (int frame)
{
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
}

/*----------------------------------------------------------------------
   WIN_ReleaseDeviceContext :  unselect the Device Context           
  ----------------------------------------------------------------------*/
void WIN_ReleaseDeviceContext (void)
{
  /* release the previous Device Context. */
  if (TtDisplay != NULL)
    {     
      SetICMMode (TtDisplay, ICM_OFF);
      ReleaseDC (WIN_curWin, TtDisplay);
    }
  TtDisplay = NULL;
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

#include "absboxes_f.h"
#ifdef _GTK
#include "gtk-functions.h"
#else /* _GTK */
#include "appli_f.h"
#include "input_f.h"
#endif /* _GTK */

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
#include "interface_f.h"
#include "keyboards_f.h"
#include "memory_f.h"
#include "message_f.h"
#include "scroll_f.h"
#include "selectmenu_f.h"
#include "structcommands_f.h"
#include "structcreation_f.h"
#include "structmodif_f.h"
#include "structselect_f.h"
#include "textcommands_f.h"
#include "thotmsg_f.h"
#include "views_f.h"
#include "viewapi_f.h"
#include "xwindowdisplay_f.h"


/*----------------------------------------------------------------------
  GetFrameNumber returns the Thot window number associated.
  ----------------------------------------------------------------------*/
int GetFrameNumber (ThotWindow win)
{
  int frame;

  for (frame = 1; frame <= MAX_FRAME; frame++)
    if (FrRef[frame] == win)
      return (frame);
  return (-1);
}

/*----------------------------------------------------------------------
   FrameToView retourne, sous la forme qui convient a l'API Thot, 
   les parametres identifiant le document et la vue        
   qui correspondent a une frame donnee.                   
  ----------------------------------------------------------------------*/
void FrameToView (int frame, int *doc, int *view)
{
   int                 i;
   PtrDocument         pDoc;

   *doc = FrameTable[frame].FrDoc;
   *view = 0;
   if (*doc == 0)
      return;
   else
     {
	pDoc = LoadedDocument[*doc - 1];
	*view = 0;
	if (pDoc != NULL)
	  {
	     GetViewFromFrame (frame, pDoc, &i);
	     *view = i;
	  }
     }
}


/*----------------------------------------------------------------------
   Evenement sur une frame document.                             
  ----------------------------------------------------------------------*/
void FrameKilled (int *w, int frame, int *info)

{
   /* Enleve la procedure de Callback */
   /* Detruit la fenetre si elle existe encore */
   if (frame > 0 && FrRef[frame] != 0)
      ViewClosed (frame);
}


/*----------------------------------------------------------------------
  Called when a clique is done on the up right corner cross
  Kill the current document                             
  ----------------------------------------------------------------------*/
#ifdef _GTK
gboolean KillFrameGTK (GtkWidget *widget, GdkEvent *event, gpointer f)
{
  PtrDocument         pDoc;
  int                 view, frame;

  frame = (int) f;
  if (frame <= MAX_FRAME)
    {
      GetDocAndView (frame, &pDoc, &view);
      CloseView (pDoc, view);
    }
  for (frame = 0; frame <= MAX_FRAME; frame++)
    if (FrRef[frame] != 0)
      /* there is still an active frame */
      return TRUE;
  TtaQuit();
  return FALSE;
}
#endif /* _GTK */

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
#endif /*_GL*/

 if (frame > 0 && frame <= MAX_FRAME)
 {
   /* Do not redraw if the document is in NoComputedDisplay mode. */
   if (documentDisplayMode[FrameTable[frame].FrDoc - 1] != NoComputedDisplay)
   {
#ifndef _GL
     BeginPaint (w, &ps);
     GetClientRect (w, &rect);
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
     DefRegion (frame, ps.rcPaint.left, ps.rcPaint.top, ps.rcPaint.right,
		ps.rcPaint.bottom);
     EndPaint (w, &ps);
     DisplayFrame (frame);
     /* restore the previous clipping */
     pFrame = &ViewFrameTable[frame - 1];
     pFrame->FrClipXBegin = xmin;
     pFrame->FrClipXEnd = xmax;
     pFrame->FrClipYBegin = ymin;
     pFrame->FrClipYEnd = ymax;
#else /*_GL*/
	 BeginPaint (w, &ps);
	 FrameTable[frame].DblBuffNeedSwap = TRUE;
     EndPaint (w, &ps);
#endif /*_GL*/
   }
 }
}

/*----------------------------------------------------------------------
   WIN_ChangeTaille : function called when a view is resized under    
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
<<<<<<< appli.c
   GL_MakeCurrent (frame);	
   GLResize (width, height, 0 ,0);
   /*Clear (frame, width, height, 0, 0);*/
   GL_ActivateDrawing (frame);
   /* need to recompute the content of the window */
   RebuildConcreteImage (frame);    
   GL_DrawAll ();
   GL_Swap (frame);
   glFinish();
=======
   if (GL_prepare (frame))
   {
     GLResize (width, height, 0 ,0);
     /* need to recompute the content of the window */
     DefRegion (frame, 0, 
 		0, width,
 		height);
      RedrawFrameBottom (frame, 0, NULL);
	  GL_realize (frame);
   }
>>>>>>> 1.367
#endif/*_GL*/
   /* recompute the scroll bars */
   UpdateScrollbars (frame);
}
#else /* _WINDOWS */

/*----------------------------------------------------------------------
  XFlushOutput enforce updating of the calculated image for frame.
  ----------------------------------------------------------------------*/
void XFlushOutput (int frame)
{
#ifndef _GTK
   XFlush (TtDisplay);
#endif /* _GTK */
}

/*----------------------------------------------------------------------
   FrameToRedisplay effectue le traitement des expositions X11 des     
   frames de dialogue et de documents.                   
  ----------------------------------------------------------------------*/
void FrameToRedisplay (ThotWindow w, int frame, void *ev)
{
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
}


/*----------------------------------------------------------------------
  FrameRedraw
  ----------------------------------------------------------------------*/
void FrameRedraw (int frame, Dimension width, Dimension height)
{
   int                 dx, dy, view;
   NotifyWindow        notifyDoc;
   Document            doc;

   if (FrameTable[frame].FrDoc == 0)
     return;
   if ((width > 0) && (height > 0) && 
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
}



#ifdef _GTK
#ifdef _GL

/*----------------------------------------------------------------------
  DrawGL :
  After all transformation Draw all canvas
  TODO : Use of display lists in 
  the pbox tree rendering by redrawframebottom with another function
  (use of pAb->change ...)
  ----------------------------------------------------------------------*/
gboolean GL_DrawCallback (ThotWidget widget, 
			  GdkEventExpose *event, 
			  gpointer data)
{
  int frame;

  frame = (int ) gtk_object_get_data (GTK_OBJECT (widget), "frame");
  FrameTable[frame].DblBuffNeedSwap = TRUE;
  return TRUE;
}
/*----------------------------------------------------------------------
  GL_FocusIn :
  Manage Drawing Timer upon Frame focus by user
  ----------------------------------------------------------------------*/
gboolean GL_FocusIn (ThotWidget widget, GdkEventExpose *event, 
		     gpointer data)
{
  int      frame;
 
  frame = (int) data;
  ActiveFrame = frame;
  /*Start animation*/
  return TRUE;
}  
/*----------------------------------------------------------------------
  GL_FocusOut :
  Manage Drawing Timer upon Frame focus by user
  ----------------------------------------------------------------------*/
gboolean GL_FocusOut (ThotWidget widget, GdkEventExpose *event, 
		      gpointer data)
{
  int      frame;
 
  frame = (int) data;
  /* Stop animation*/
  return TRUE ;
}

/*----------------------------------------------------------------------
 GL_Destroy :
 Close Opengl pipeline
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
 GL_Init :
 Opengl pipeline state initialization
 ----------------------------------------------------------------------*/
gboolean  GL_Init (ThotWidget widget, 
		   GdkEventExpose *event, 
		   gpointer data)
{
  static ThotBool dialogfont_enabled = FALSE;
 
  if (gtk_gl_area_make_current (GTK_GL_AREA(widget)))
    {       

      SetGlPipelineState ();
      if (!dialogfont_enabled)
	  {
	    InitDialogueFonts ("");
	    dialogfont_enabled = TRUE;
	  } 

      /* gtk_idle_add (GTK_SIGNAL_FUNC (GL_DrawAll),  */
      /* 		    NULL);  */

      /* gtk_timeout_add (5,  */
      /*                        GL_DrawAll,  */
      /*                       (gpointer)   NULL);  */


      return TRUE;
    }
  else
    return TRUE;   
}
/*----------------------------------------------------------------------
  ExposeCallbackGTK : 
  When a part of the canvas is hidden by a window or menu 
  It permit to Redraw frames 
  ----------------------------------------------------------------------*/
gboolean ExposeCallbackGTK (ThotWidget widget, 
			    GdkEventExpose *event, 
			    gpointer data)
{
  int                 frame;
  int                 x;
  int                 y;
  int                 width;
  int                 height;

  
  if (event->count > 0)
      return TRUE;
  frame = (int) data;
  x = event->area.x;
  y = event->area.y;
  width = event->area.width;
  height = event->area.height;  
  if ((width <= 0) || 
      (height <= 0) || 
      !(frame > 0 && frame <= MAX_FRAME))
    return TRUE;
  if (documentDisplayMode[FrameTable[frame].FrDoc - 1] == NoComputedDisplay)
    return TRUE; 
  if (GL_prepare (frame))
      GL_Swap (frame);
  return TRUE;
}
/*----------------------------------------------------------------------
   FrameResizedGTK When user resize window
  ----------------------------------------------------------------------*/
gboolean FrameResizedGTK (GtkWidget *widget, 
			  GdkEventConfigure *event, 
			  gpointer data)
{
  int                 frame;
  Dimension           width, height;

  frame = (int )data;

  width = event->width;
  height = event->height;

  /* width = widget->allocation.width; */
  /*   height = widget->allocation.height; */

  FrameTable[frame].FrWidth = width;
  FrameTable[frame].FrHeight = height;
  
  if ((width <= 0) || 
      (height <= 0))
    return TRUE;

  if (widget)
    if (GL_prepare (frame))
      {
	GLResize (width, 
		  height, 
		  0, 0);
	DefRegion (frame, 
 		   0, 0,
 		   width, height);	
	FrameRedraw (frame, width, height);
	glFlush();
	glFinish ();
	GL_Swap (frame);
	FrameTable[frame].DblBuffNeedSwap = TRUE;
	while (gtk_events_pending ()) 
	  gtk_main_iteration ();
      }
  return TRUE;
}
#else /* _GL*/
/*----------------------------------------------------------------------
   FrameResizedGTK When user resize window
  ----------------------------------------------------------------------*/
gboolean FrameResizedGTK (GtkWidget *w, GdkEventConfigure *event, gpointer data)
{
  int         frame;
  Dimension   width, height;
 
  frame = (int )data;
  width = event->width;
  height = event->height; 
  if (FrameTable[frame].FrWidth == width
      && FrameTable[frame].FrHeight == height)
    return TRUE;
  FrameRedraw (frame, width, height);
  while (gtk_events_pending ()) 
     gtk_main_iteration ();
  return TRUE;
}

/*----------------------------------------------------------------------
  ExposeCallbackGTK : 
  When a part of the canvas is hidden by a window or menu 
  It permit to Redraw frames 
  ----------------------------------------------------------------------*/
gboolean ExposeCallbackGTK (ThotWidget widget, GdkEventExpose *event, gpointer data)
{
  int nframe;
  int                 x;
  int                 y;
  int                 l;
  int                 h;

  nframe = (int )data;
  x = event->area.x;
  y = event->area.y;
  l = event->area.width;
  h = event->area.height;
  
  
  if (nframe > 0 && nframe <= MAX_FRAME)
    { 
      DefRegion (nframe, x, y, l+x, y+h );
      RedrawFrameBottom (nframe, 0, NULL);
    }
  return FALSE;
}
#endif /* _GL */
#else /* _GTK */
/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void FrameResized (int *w, int frame, int *info)
{
   int                 n;
   Dimension           width, height;
   Arg                 args[MAX_ARGS];

   n = 0;
   XtSetArg (args[n], XmNwidth, &width);
   n++;
   XtSetArg (args[n], XmNheight, &height);
   n++;
   XtGetValues ((ThotWidget) w, args, n);

   FrameRedraw (frame, width, height);
}
#endif /* _GTK */
#endif /* _WINDOWS */

#ifdef _WINDOWS
/*----------------------------------------------------------------------
   Demande de scroll vertical.                                      
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
	GL_prepare (frame);
	GL_DrawAll (NULL, frame);
#endif /*_GL*/
       break;
     } 
}

/*----------------------------------------------------------------------
   Demande de scroll vertical.                                      
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

   HorizontalScroll (frame, delta, 1);
}
#endif /* _WINDOWS */

#ifndef _WINDOWS
/*----------------------------------------------------------------------
   Demande de scroll horizontal.                                    
  ----------------------------------------------------------------------*/
#ifndef _GTK
void FrameHScrolled (int *w, int frame, int *param)
#else /* _GTK */
void FrameHScrolledGTK (GtkAdjustment *w, int frame)
#endif /* _GTK */
{
   int                 delta, l;
   int                 view;
#ifndef _GTK
   int                 n;
   Arg                 args[MAX_ARGS];
   XmScrollBarCallbackStruct *info;
#endif /* _GTK */
   NotifyWindow        notifyDoc;
   Document            doc;

   /* ne pas traiter si le document est en mode NoComputedDisplay */
   if (FrameTable[frame].FrDoc &&
       documentDisplayMode[FrameTable[frame].FrDoc - 1] == NoComputedDisplay)
      return;
#ifndef _GTK
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
#else /* _GTK */
   /* delta is the position into the page */
   delta = w->value;
#endif  /* _GTK */

   notifyDoc.event = TteViewScroll;
   FrameToView (frame, &doc, &view);
   notifyDoc.document = doc;
   notifyDoc.view = view;
   notifyDoc.verticalValue = 0;
   notifyDoc.horizontalValue = delta;
   if (!CallEventType ((NotifyEvent *) & notifyDoc, TRUE))
     {
#ifndef _GTK
       if (info->reason == XmCR_VALUE_CHANGED || info->reason == XmCR_DRAG)
	 {
	   /* On recupere la largeur de l'ascenseur */
	   n = 0;
	   XtSetArg (args[n], XmNsliderSize, &l);
	   n++;
	   XtGetValues (FrameTable[frame].WdScrollH, args, n);
#else /* _GTK */
	   /* l is the width of the page */
	   l = w->page_size;
	   /*	   l = FrameTable[frame].FrWidth;*/
#endif /* _GTK */
#ifndef _GTK
	   /* On regarde si le deplacement bute sur le bord droit */
	   if (info->value + l >= FrameTable[frame].FrWidth)
#else /* _GTK */
	   /* On regarde si le deplacement bute sur le bord droit */
	   if (w->value + l >= FrameTable[frame].FrWidth)	     
#endif /* _GTK */
       	     delta = FrameTable[frame].FrScrollWidth;
	   else
	     {
#ifndef _GTK
	       /* translate the position in the scroll bar into a shift value in the document */
	       delta = (int) ((float) (info->value * FrameTable[frame].FrScrollWidth) / (float) FrameTable[frame].FrWidth);
#else /* _GTK */
	       /* translate the position in the scroll bar into a shift value in the document */
	       delta = (int) ((float) (w->value * FrameTable[frame].FrScrollWidth) / (float) FrameTable[frame].FrWidth);
#endif /* _GTK */
	       delta = FrameTable[frame].FrScrollOrg + delta - ViewFrameTable[frame - 1].FrXOrg;
	     }
#ifndef _GTK
	 }
       else if (info->reason == XmCR_TO_TOP)
	 /* force the left alignment */
	 delta = -FrameTable[frame].FrScrollWidth;
       else if (info->reason == XmCR_TO_BOTTOM)
	 /* force the right alignment */
	 delta = FrameTable[frame].FrScrollWidth;
#endif /* _GTK */
       HorizontalScroll (frame, delta, 1);
       notifyDoc.document = doc;
       notifyDoc.view = view;
       notifyDoc.verticalValue = 0;
       notifyDoc.horizontalValue = delta;
       CallEventType ((NotifyEvent *) & notifyDoc, FALSE);
     }
} 

/*----------------------------------------------------------------------
   Demande de scroll vertical.                                      
  ----------------------------------------------------------------------*/
#ifndef _GTK
void FrameVScrolled (int *w, int frame, int *param)
{
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
  if (documentDisplayMode[FrameTable[frame].FrDoc - 1] == NoComputedDisplay)
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
      else
	VerticalScroll (frame, delta, 1);
    notifyDoc.document = doc;
    notifyDoc.view = view;
    notifyDoc.verticalValue = delta;
    notifyDoc.horizontalValue = 0;
    CallEventType ((NotifyEvent *) & notifyDoc, FALSE);
    }
}
#else /* _GTK */

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void FrameVScrolledGTK (GtkAdjustment *w, int frame)
{
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
      VerticalScroll (frame, delta, 1);
    } 
  else 
    {
      delta = (int) ((w->value / (float)FrameTable[frame].FrHeight) * 100);
      JumpIntoView (frame, delta);
    }
}

#endif /*_GTK*/
#endif /* _WINDOWS */

/*----------------------------------------------------------------------
   TtcLineUp scrolls one line up.                                    
  ----------------------------------------------------------------------*/
void TtcLineUp (Document document, View view)
{
#if !defined(_WINDOWS) && !defined(_GTK)
  XmScrollBarCallbackStruct infos;
#else /* _GTK */
  int                       delta;
#endif /* _WINDOWS && _GTK */
  int                       frame;
  
  if (document != 0)
    frame = GetWindowNumber (document, view);
  else
    frame = 0;

#if !defined(_WINDOWS) && !defined(_GTK)
  infos.reason = XmCR_DECREMENT;
  FrameVScrolled (0, frame, (int *) &infos);
#else /* _WINDOWS && _GTK */
  delta = -13;
  VerticalScroll (frame, delta, 1);
#endif /* _WINDOWS && GTK */
}

/*----------------------------------------------------------------------
   TtcLineDown scrolls one line down.                                
  ----------------------------------------------------------------------*/
void TtcLineDown (Document document, View view)
{
#if !defined(_WINDOWS) && !defined(_GTK)
  XmScrollBarCallbackStruct infos;
#else /* _WINDOWS && _GTK */
  int                       delta;
#endif /* _WINDOWS && _GTK */
  int                       frame;

  if (document != 0)
    frame = GetWindowNumber (document, view);
  else
    frame = 0;

#if !defined(_WINDOWS) && !defined(_GTK)
  infos.reason = XmCR_INCREMENT;
  FrameVScrolled (0, frame, (int *) &infos);
#else /* _WINDOWS && _GTK */
  delta = 13;
  VerticalScroll (frame, delta, 1);
#endif /* _WINDOWS && _GTK */
}

/*----------------------------------------------------------------------
   TtcScrollLeft scrolls one position left.                                    
  ----------------------------------------------------------------------*/
void TtcScrollLeft (Document document, View view)
{
#if !defined(_WINDOWS) && !defined(_GTK)
  XmScrollBarCallbackStruct infos;
#else /* _WINDOWS && _GTK */
  int                       delta;
#endif /* _WINDOWS && GTK */
  int                       frame;
  
  if (document != 0)
    frame = GetWindowNumber (document, view);
  else
    frame = 0;

#if !defined(_WINDOWS) && !defined(_GTK)
  infos.reason = XmCR_DECREMENT;
  FrameHScrolled (0, frame, (int *) &infos);
#else /* _WINDOWS && _GTK */
  delta = -13;
  HorizontalScroll (frame, delta, 1);
#endif /* _WINDOWS && _GTK */
}

/*----------------------------------------------------------------------
   TtcScrollRight scrolls one position right.                                
  ----------------------------------------------------------------------*/
void TtcScrollRight (Document document, View view)
{
#if !defined(_WINDOWS) && !defined(_GTK)
  XmScrollBarCallbackStruct infos;
#else /* _WINDOWS && _GTK */
  int                       delta;
#endif /* _WINDOWS && GTK */
  int                       frame;

  if (document != 0)
    frame = GetWindowNumber (document, view);
  else
    frame = 0;

#if !defined(_WINDOWS) && !defined(_GTK)
  infos.reason = XmCR_INCREMENT;
  FrameHScrolled (0, frame, (int *) &infos);
#else  /* _WINDOWS && GTK */
  delta = 13;
  HorizontalScroll (frame, delta, 1);
#endif /* _WINDOWS */
}

/*----------------------------------------------------------------------
   PageUp scrolls one screen up.                                    
  ----------------------------------------------------------------------*/
void TtcPageUp (Document document, View view)
{
#if !defined(_WINDOWS) && !defined(_GTK)
   XmScrollBarCallbackStruct infos;
#else   /* _WINDOWS && GTK */
   int                       delta;
#endif  /* _WINDOWS && GTK */
   int                       frame;

   if (document != 0)
      frame = GetWindowNumber (document, view);
   else
     frame = 0;
#if !defined(_WINDOWS) && !defined(_GTK)
   infos.reason = XmCR_PAGE_DECREMENT;
   FrameVScrolled (0, frame, (int *) &infos);
#else  /* _WINDOWS && GTK */
   delta = -FrameTable[frame].FrHeight;
   VerticalScroll (frame, delta, 1);
#endif /* _WINDOWS && GTK */
}

/*----------------------------------------------------------------------
   PageDown scrolls one screen down.                                
  ----------------------------------------------------------------------*/
void TtcPageDown (Document document, View view)
{
#if !defined(_WINDOWS) && !defined(_GTK)
   XmScrollBarCallbackStruct infos;
#else   /* _WINDOWS && GTK */
   int                       delta;
#endif  /* _WINDOWS && GTK */
  int                        frame;

   if (document != 0)
      frame = GetWindowNumber (document, view);
   else
     frame = 0;
#if !defined(_WINDOWS) && !defined(_GTK)
   infos.reason = XmCR_PAGE_INCREMENT;
   FrameVScrolled (0, frame, (int *) &infos);
#else  /* _WINDOWS && GTK */
   delta = FrameTable[frame].FrHeight;
   VerticalScroll (frame, delta, 1);
#endif /* _WINDOWS && GTK */
}


/*----------------------------------------------------------------------
   PageTop goes to the document top.                                
  ----------------------------------------------------------------------*/
void TtcPageTop (Document document, View view)
{
#if !defined(_WINDOWS) && !defined(_GTK)
   XmScrollBarCallbackStruct infos;
#endif  /* _WINDOWS && GTK */
  int                        frame;

  if (document != 0)
    frame = GetWindowNumber (document, view);
  else
    frame = 0;

#if !defined(_WINDOWS) && !defined(_GTK)
   infos.reason = XmCR_TO_TOP;
   FrameVScrolled (0, frame, (int *) &infos);
#else   /* _WINDOWS && GTK */
  JumpIntoView (frame, 0);
#endif /* _WINDOWS && GTK */
}

/*----------------------------------------------------------------------
   PageEnd goes to the document end.                                
  ----------------------------------------------------------------------*/
void TtcPageEnd (Document document, View view)
{
#if !defined(_WINDOWS) && !defined(_GTK)
   XmScrollBarCallbackStruct infos;
#endif  /* _WINDOWS && GTK */
  int                        frame;

   if (document != 0)
      frame = GetWindowNumber (document, view);
   else
     frame = 0;

#if !defined(_WINDOWS) && !defined(_GTK)
   infos.reason = XmCR_TO_BOTTOM;
   FrameVScrolled (0, frame, (int *) &infos);
#else /* _WINDOWS && _GTK */
   JumpIntoView (frame, 100);
#endif /* _WINDOWS && _GTK */
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
	FrameTable[i].WdFrame = 0;
	FrameTable[i].FrDoc = 0;
     }

   ClickIsDone = 0;
   ClickFrame = 0;
   ClickX = 0;
   ClickY = 0;
   /* message de selection vide */
#ifndef _WINDOWS
#ifndef _GTK
   null_string = XmStringCreateSimple ("");
#else /* _GTK */
   null_string = (gchar *)"";
#endif /* _GTK */
#endif /* _WINDOWS */
   OldMsgSelect[0] = EOS;
   OldDocMsgSelect = NULL;
}

/*----------------------------------------------------------------------
   TtaChangeWindowTitle
   if view == 0, changes the title of all windows of document
   otherwise change the window title of the specified view.
  ----------------------------------------------------------------------*/
void TtaChangeWindowTitle (Document document, View view, char *title)
{
  int          idwindow, v;
  PtrDocument  pDoc;
  
  if (view > 0)
    {
      idwindow = GetWindowNumber (document, view);
      if (idwindow > 0) 
	ChangeFrameTitle (idwindow, title);
    }
  else
    {
      pDoc = LoadedDocument[document - 1];
      /* traite les vues du document */
      for (v = 0; v < MAX_VIEW_DOC; v++)
	if (pDoc->DocView[v].DvPSchemaView > 0)
	  ChangeFrameTitle (pDoc->DocViewFrame[v], title);
    }
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
	w = FrameTable[idwindow].WdFrame;
#ifndef _WINDOWS
#ifndef _GTK
	if (w != 0)
	   XMapRaised (TtDisplay, XtWindowOfObject (XtParent (XtParent (XtParent (w)))));
#else /* _GTK */
	if (w!=0)
	  gdk_window_raise (GTK_WIDGET(w->parent->parent->parent)->window);
#endif /* _GTK */
#else  /* _WINDOWS */
	SetForegroundWindow (FrMainRef [idwindow]);
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


   if (ActiveFrame != 0 &&
       (strcmp (OldMsgSelect, text) || pDoc != OldDocMsgSelect))
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
#ifndef _WINDOWS
#ifndef _GTK
   Arg                 args[MAX_ARGS];
   XmString            title_string;
#else /* _GTK */
   gchar * title_string;
#endif /* _GTK */
#endif /* _WINDOWS */

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
	     s = TtaGetMemory (length);
#ifdef _WINDOWS
	     if (name)
	       /* text est un format */
	       sprintf (s, text, name);
	     else
	       strncpy (s, text, length);

	     SendMessage (FrameTable[frame].WdStatus, SB_SETTEXT, (WPARAM) 0, (LPARAM) s);
	     SendMessage (FrameTable[frame].WdStatus, WM_PAINT, (WPARAM) 0, (LPARAM) 0);
#else  /* _WINDOWS */
#ifndef _GTK
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
#else /* _GTK */
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
#endif /* _WINDOWS */
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
  WndProc:  The main MS-Windows event handler for the Thot Library
  ----------------------------------------------------------------------*/
LRESULT CALLBACK WndProc (HWND hwnd, UINT mMsg, WPARAM wParam, LPARAM lParam) 
{
  PtrDocument         pDoc;
  HWND                hwndTextEdit;
  HWND                hwndToolTip;
  RECT                rect;
  RECT                rWindow;
  LPNMHDR             pnmh;
  int                 frame;
  int                 view;
  int                 frameNDX;
  int                 idCtrl;
  int                 cx;
  int                 cy;
  int                 cyStatus;
  int                 cxVSB;
  int                 cyHSB;
  int                 cyTB;
  int                 x, y;
  int                 cyTxtZone;
  DWORD               dwStyle;
  DWORD               dwToolBarStyles;
  DWORD               dwStatusBarStyles;

  frame = GetMainFrameNumber (hwnd);
  if (frame != -1)
    ActiveFrame = frame;
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
		MessageBox(NULL, "ERROR!", "Failed to create new client window in function WndProc()", MB_OK); 
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
    ActiveFrame = frame;
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
     case CBN_KILLFOCUS:
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
	GL_Win32ContextClose (frame, hwnd);
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
    /* Adjust toolbar size. */
    if (IsWindowVisible (WinToolBar[frame]))
      {
	dwStyle = GetWindowLong (WinToolBar[frame], GWL_STYLE);
	if (dwStyle & CCS_NORESIZE)
	  MoveWindow (WinToolBar[frame], 0, 0, cx, cyToolBar, FALSE);
	else
	  ToolBar_AutoSize (WinToolBar[frame]);

	InvalidateRect (WinToolBar[frame], NULL, TRUE);
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
	    MoveWindow (FrameTable[frame].Text_Zone, 85, cyTxtZone + 5, cx - 100, 20, TRUE);
	    cyTxtZone += 25;
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

    /* Adjust Vertical scroll bar */
    MoveWindow (FrameTable[frame].WdScrollV, cx - 15,
		cyTxtZone, 15, cy - (cyStatus + cyTxtZone + 15), TRUE);
    
    /* Adjust Hoizontal scroll bar */
    MoveWindow (FrameTable[frame].WdScrollH, 0,
		cy - (cyStatus + 15), cx - 15, 15, TRUE);
    
    /* Adjust client window size. */
    GetWindowRect (FrameTable[frame].WdScrollV, &rWindow);
    cxVSB = rWindow.right - rWindow.left;

    GetWindowRect (FrameTable[frame].WdScrollH, &rWindow);
    cyHSB = rWindow.bottom - rWindow.top;

    x = 0;
    y = cyTxtZone;
    cx = cx - cxVSB;
    cy = cy - (cyStatus + cyTxtZone + cyHSB);
    MoveWindow (FrRef [frame], x, y, cx, cy, TRUE);

    SetScrollRange (FrameTable[frame].WdScrollV, SB_CTL, 0, cy, TRUE);
    SetScrollRange (FrameTable[frame].WdScrollH, SB_CTL, 0, cx, TRUE);

	SetFocus (FrRef [frame]);
	ActiveFrame = frame;
    return 0L;

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
  ThotBool     isSpecial;
  /* Used to limit selection extension
	on mouse move  */
  static int       oldXPos;
  static int       oldYPos;

  frame = GetFrameNumber (hwnd);
  /* do not handle events if the Document is in NoComputedDisplay mode. */
  if (frame != -1)
    {
      ActiveFrame = frame;
      FrameToView (frame, &document, &view);
      if (documentDisplayMode[FrameTable[frame].FrDoc - 1] == NoComputedDisplay)
	return (DefWindowProc (hwnd, mMsg, wParam, lParam));
      
      /*
       * If are waiting for the user to explicitely point to a document,
       * store the location and return.
       */
      if (ClickIsDone == 1 &&
	  (mMsg == WM_LBUTTONDOWN || mMsg == WM_RBUTTONDOWN ||
	   mMsg == WM_SYSKEYDOWN || mMsg == WM_KEYDOWN))
	{
	  ClickIsDone = 0;
	  ClickFrame = frame;
	  ClickX = LOWORD (lParam);
	  ClickY = HIWORD (lParam);
	  return (DefWindowProc (hwnd, mMsg, wParam, lParam));
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
    case WM_CREATE:
      DragAcceptFiles (hwnd, TRUE);
      return 0;

    case WM_PAINT: 
      /* Some part of the Client Area has to be repaint. */
      WIN_HandleExpose (hwnd, frame, wParam, lParam);
      return 0;

    case WM_SIZE:
      cx         = LOWORD (lParam);
      cy         = HIWORD (lParam);
      WIN_ChangeViewSize (frame, cx, cy, 0, 0);
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
	  if (wParam >= 48 && wParam <= 57)
	    {
	    /* handling Ctrl 0-9 or Alt 0-9 */
	      key_menu = GetKeyState (VK_MENU);
  	      key = GetKeyState (VK_CONTROL);
		  /* is it an Alt-GR modifier? (WIN32 interprets this
		     as having both a control + menu key pressed down) */
	      if (HIBYTE (key_menu) && HIBYTE (key))
			return 0;
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
		    return 0;
		}
	    }
	  else
	    isSpecial = TRUE;
	  key = (int) wParam;
	  if (WIN_TtaHandleMultiKeyEvent (mMsg, wParam, lParam, (int *)&key))
	    WIN_CharTranslation (FrRef[frame], frame, mMsg, (WPARAM) key,
                             lParam, isSpecial);
	  if (wParam != VK_MENU)
	    return 0;
	}
      break;
    case WM_SYSCHAR:
    case WM_CHAR:
      key = (int) wParam;
      if (WIN_TtaHandleMultiKeyEvent (mMsg, wParam, lParam, (int *)&key))
	WIN_CharTranslation (FrRef[frame], frame, mMsg, (WPARAM) key, lParam, FALSE);
      if (wParam != VK_MENU)
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
      return 0;


    case WM_NCMOUSEMOVE:
		/* Mouse move outside client area*/
      return 0;
    case WM_MOUSEMOVE:
		/* Mouse move inside client area*/
      return 0;

    default:
      break;
    }
#ifdef _GL2
  GL_DrawAll (NULL, -1);
#endif /*GL*/
  return (DefWindowProc (hwnd, mMsg, wParam, lParam));
}
#else /* _WINDOWS */
#ifdef _GTK
/*----------------------------------------------------------------------
   GtkLiningSelection 

   When user hold clicked a mouse button in order to extend a selection
   and scroll in the meantime,
   those functions are called by a timer each 100 ms
   in order to repeat user action until he released the button
   or move away from the widget.
----------------------------------------------------------------------*/
static int          Motion_y = 0;
static int          Motion_x = 0;
static ThotBool     Selecting = FALSE;

gboolean GtkLiningSelection (gpointer data)
{
  Document            doc; 
  ViewFrame          *pFrame;
  int                 frame;
  int                 view;

   frame = (int) data;
   if (frame < 1)
     return FALSE;
   FrameToView (frame, &doc, &view);
   pFrame = &ViewFrameTable[frame - 1];
   /* generate a scroll if necessary */
   if (Motion_y > FrameTable[frame].FrHeight)
     {
       if (pFrame->FrAbstractBox &&
	   pFrame->FrAbstractBox->AbBox &&
	   pFrame->FrYOrg + FrameTable[frame].FrHeight < pFrame->FrAbstractBox->AbBox->BxHeight)
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
#endif /* _GTK */

/*----------------------------------------------------------------------
   Evenement sur une frame document.                              
   D.V. equivalent de la fontion MS-Windows ci dessus !        
   GTK: fonction qui traite les click de la souris sauf la selection   
  ----------------------------------------------------------------------*/
#ifndef _GTK
void FrameCallback (int frame, void *evnt)
#else /* _GTK */
gboolean FrameCallbackGTK (GtkWidget *widget, GdkEventButton *event, gpointer data)
#endif /* _GTK */
{
#ifndef _GTK
  ThotEvent           event;
  ThotEvent          *ev = (ThotEvent *) evnt;
  int                 comm, dx, dy, sel, h;
#else /* _GTK */
  int                 frame;
  GtkEntry            *textzone;
  static int          timer = None;
#endif /* _GTK */
  Document            document;
  View                view;

#ifdef _GTK
  frame = (int )data;
#endif /* _GTK */

#ifndef _GTK
  /* ne pas traiter si le document est en mode NoComputedDisplay */
  if (FrameTable[frame].FrDoc == 0 ||
      documentDisplayMode[FrameTable[frame].FrDoc - 1] == NoComputedDisplay)
    return;
  else if (ev == NULL)
    return;
  else if (ClickIsDone == 1 &&
	   (ev->type == ButtonPress || ev->type == KeyPress))
#else /* _GTK */
  frame = (int )data;
  /* ne pas traiter si le document est en mode NoComputedDisplay */
  if (FrameTable[frame].FrDoc == 0 ||
      documentDisplayMode[FrameTable[frame].FrDoc - 1] == NoComputedDisplay)
    return FALSE;
  else if (event == NULL)
    return FALSE;
  else if (ClickIsDone == 1 && event->type == GDK_BUTTON_PRESS)
#endif /* _GTK */
    /* Amaya is waiting for a selection */
      {
	ClickIsDone = 0;
	ClickFrame = frame;
#ifndef _GTK
	ClickX = ev->xbutton.x;
	ClickY = ev->xbutton.y;
	return;
#else /* _GTK */
	ClickX = event->x;
        ClickY = event->y;
        return FALSE;
#endif /* _GTK */
      }

  /* S'il y a un TtaWaitShowDialogue en cours on n'autorise pas de changer */
  /* la selection courante. */
#ifndef _GTK
  if (TtaTestWaitShowDialogue ()
      && (ev->type != ButtonPress || (ev->xbutton.state & THOT_KEY_ControlMask) == 0))
    return;
#else /* _GTK */
  if (TtaTestWaitShowDialogue ()
      && (event->type != GDK_BUTTON_PRESS || 
	  event->type != GDK_2BUTTON_PRESS ||
	  event->type != GDK_MOTION_NOTIFY || 
	  (event->state & GDK_CONTROL_MASK ) == 0))
    return FALSE;
  /* 
     Set the drawing area Focused
     By setting the focus on the text zone
     If another action specifically need focus, 
     w'ell grab it with the action
  */
  textzone = gtk_object_get_data (GTK_OBJECT (widget), "Text_catcher");
  gtk_widget_grab_focus (GTK_WIDGET(textzone));  
#endif /* _GTK */

#ifndef _GTK
  switch (ev->type)
    {
    case ButtonPress:
      /*_____________________________________________________*/
      /* Termine l'insertion courante s'il y en a une */
      CloseInsertion ();
      switch (ev->xbutton.button)
	{
	case Button1:
	  /* ==========LEFT BUTTON========== */	  
	  /* Est-ce que la touche modifieur de geometrie est active ? */	  
	  if ((ev->xbutton.state & THOT_KEY_ControlMask) != 0)
	    {
	      /* moving a box */
	      ApplyDirectTranslate (frame, ev->xbutton.x, ev->xbutton.y);
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
	      ApplyDirectResize (frame, ev->xbutton.x, ev->xbutton.y);
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
	  /* ==========RIGHT BUTTON========== */
	  if ((ev->xbutton.state & THOT_KEY_ControlMask) != 0)
	    {
	      /* resize a box */
	      ApplyDirectResize (frame, ev->xbutton.x, ev->xbutton.y);
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
#else /* _GTK */
  switch (event->type)
    {
    case GDK_BUTTON_PRESS:
      /*_____________________________________________________*/
      /* Termine l'insertion courante s'il y en a une */
      CloseInsertion ();
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
	  /* Est-ce que la touche modifieur de geometrie est active ? */	  
	  if ((event->state & GDK_CONTROL_MASK ) == GDK_CONTROL_MASK)
	    /* moving a box */     
	    ApplyDirectTranslate (frame, event->x, event->y);
	  else if ((event->state & GDK_SHIFT_MASK ) == GDK_SHIFT_MASK)
	    {
	      /* a selection extension */
	      TtaAbortShowDialogue ();
	      LocateSelectionInView (frame, event->x, event->y, 0);
	      FrameToView (frame, &document, &view);
	      TtcCopyToClipboard (document, view);
	    }
	  else
	    {
	      /* a simple selection */
	      ClickFrame = frame;
	      ClickX = event->x;
	      ClickY = event->y;
	      LocateSelectionInView (frame, ClickX, ClickY, 2);
	      Selecting = TRUE;
	    }
	  break;
	case 2:
	  /* ==========MIDDLE BUTTON========== */
	  if ((event->state & GDK_CONTROL_MASK) != 0)
	    /* resizing a box */
	    ApplyDirectResize (frame, event->x, event->y);
	  else
	    {
	      ClickFrame = frame;
	      ClickX = event->x;
	      ClickY = event->y; 
		  LocateSelectionInView (frame, ClickX, ClickY, 5);	     
	    }
	  break;
	case 3:
	  /* ==========RIGHT BUTTON========== */
	  if ((event->state & GDK_CONTROL_MASK) != 0)
	    /* resize a box */
	    ApplyDirectResize (frame, event->x, event->y);
	  else
	    {
	      ClickFrame = frame;
	      ClickX = event->x;
	      ClickY = event->y;
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
	      TtcLineUp(document, view); 
	      TtcLineUp(document, view); 
	      TtcLineUp(document, view); 
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
	      TtcLineDown (document, view); 
	      TtcLineDown (document, view); 
	      TtcLineDown (document, view); 
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
	  ClickX = event->x;
	  ClickY = event->y;
	  LocateSelectionInView (frame, ClickX, ClickY, 3);
	  break;
	case 2:
	  /* MIDDLE BUTTON */
	  /* handle a simple selection */
	  ClickFrame = frame;
	  ClickX = event->x;
	  ClickY = event->y;
	  LocateSelectionInView (frame, ClickX, ClickY, 5);
	  break;
	case 3:
	  /* RIGHT BUTTON */
	  /* handle a simple selection */
	  ClickFrame = frame;
	  ClickX = event->x;
	  ClickY = event->y;
	  LocateSelectionInView (frame, ClickX, ClickY, 6);
	  break;
	}
      break;
    case GDK_MOTION_NOTIFY:
      /* extend the current selection */
      Motion_y = event->y;
      Motion_x = event->x;
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
      Motion_y = event->y;
      Motion_x = event->x;
      if (timer != None)
	{
	  gtk_timeout_remove (timer);
	  timer = None;
	} 
      else if (event->button == 1)
	{
	  ClickFrame = frame;
	  ClickX = event->x;
	  ClickY = event->y;
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
}
#endif /* _WINDOWS */


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
#ifndef _WINDOWS
#ifndef _GTK 
  XGrabPointer (TtDisplay, win, FALSE, events, GrabModeAsync, GrabModeAsync,
		win, cursor, CurrentTime);
#else /* _GTK */
  gdk_pointer_grab (win, FALSE, events, win, cursor, GDK_CURRENT_TIME);
#endif /* _GTK */
#endif /* _WINDOWS */
}


/*----------------------------------------------------------------------
   ThotUngrab est une fonction d'interface pour UnGrab.            
  ----------------------------------------------------------------------*/
void ThotUngrab ()
{
#ifndef _WINDOWS
#ifndef _GTK
  XUngrabPointer (TtDisplay, CurrentTime);
#else /* _GTK */
  gdk_pointer_ungrab (GDK_CURRENT_TIME);
#endif /* _GTK */
#endif /* _WINDOWS */
}

/*----------------------------------------------------------------------
   TtaGetThotWindow recupere le numero de la fenetre.           
  ----------------------------------------------------------------------*/
ThotWindow TtaGetThotWindow (int frame)
{
  return FrRef[frame];
}

/*----------------------------------------------------------------------
   SetCursorWatch affiche le curseur "montre".                  
  ----------------------------------------------------------------------*/
void SetCursorWatch (int thotThotWindowid)
{
#ifndef _WINDOWS
   Drawable            drawable;
#ifdef _GTK   
   int                 frame;
   ThotWidget          w;
#endif /* _GTK */

  
   drawable = (Drawable)TtaGetThotWindow (thotThotWindowid);
#ifndef _GTK
   XDefineCursor (TtDisplay, drawable, WaitCurs);
#else /*_GTK */
    for (frame = 1; frame <= MAX_FRAME; frame++)
	{
	    w = FrameTable[frame].WdFrame;
	    if (w != NULL)
		if (w->window != NULL)
		    gdk_window_set_cursor (GTK_WIDGET(w)->window, WaitCurs);
	}
#endif /* _GTK */
#else  /* _WINDOWS */
   SetCursor (LoadCursor (NULL, IDC_WAIT));
   ShowCursor (TRUE);
#endif /* _WINDOWS */
}

/*----------------------------------------------------------------------
   ResetCursorWatch enleve le curseur "montre".                 
  ----------------------------------------------------------------------*/
void ResetCursorWatch (int thotThotWindowid)
{  
#ifndef _WINDOWS
   Drawable            drawable;
#ifdef _GTK
   int                 frame;
   ThotWidget          w;
#endif /* _GTK */

   drawable = (Drawable) TtaGetThotWindow (thotThotWindowid);
#ifndef _GTK
   XUndefineCursor (TtDisplay, drawable);
#else /* GTK */
    for (frame = 1; frame <= MAX_FRAME; frame++)
	{
	    w = FrameTable[frame].WdFrame;
	    if (w != NULL)
		if (w->window != NULL)
		    gdk_window_set_cursor (GTK_WIDGET(w)->window, ArrowCurs);
	}
#endif /* _GTK */
#  else  /* _WINDOWS */
   ShowCursor (FALSE);
   SetCursor (LoadCursor (NULL, IDC_ARROW));
#  endif /* _WINDOWS */
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void TtaSetCursorWatch (Document document, View view)
{
   int                 frame;
#  ifndef _WINDOWS
   Drawable            drawable;
#ifdef _GTK
   ThotWidget          w;
#endif /* _GTK */
#  endif  /* _WINDOWS */

   UserErrorCode = 0;
   /* verifie le parametre document */
   if (document == 0 && view == 0)
     {
	for (frame = 1; frame <= MAX_FRAME; frame++)
	  {
#ifndef _WINDOWS
	     drawable = (Drawable)TtaGetThotWindow (frame);
	     if (drawable != 0)
#ifndef _GTK
		XDefineCursor (TtDisplay, drawable, WaitCurs);
#else /* _GTK */
		 {
		     w = FrameTable[frame].WdFrame;
		     gdk_window_set_cursor (GTK_WIDGET(w)->window, WaitCurs);
		 }
#endif /* _GTK */
#endif /* _WINDOWS */
	  }
     }
   else
   {
       frame = GetWindowNumber (document, view);
#   ifndef _WINDOWS
       if (frame != 0)
#ifndef _GTK
	   XDefineCursor (TtDisplay, TtaGetThotWindow (frame), WaitCurs);
#else /* _GTK */
	   {
		w = FrameTable[frame].WdFrame;
		gdk_window_set_cursor (GTK_WIDGET(w)->window, WaitCurs);
	   }
#endif /* _GTK */
#   endif /* _WINDOWS */
     }
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void TtaResetCursor (Document document, View view)
{
   int                 frame;
#ifndef _WINDOWS
   Drawable            drawable;
#ifdef _GTK
   ThotWidget          w;
#endif /* _GTK */
#endif /* _WINDOWS */

   UserErrorCode = 0;
   /* verifie le parametre document */
   if (document == 0 && view == 0)
     {
	for (frame = 1; frame <= MAX_FRAME; frame++)
	  {
#ifndef _WINDOWS
	     drawable = (Drawable)TtaGetThotWindow (frame);
	     if (drawable != 0)
#ifndef _GTK
		XUndefineCursor (TtDisplay, drawable);
#else /* _GTK */
	    {
		w = FrameTable[frame].WdFrame;
		if (w != NULL)
		    if (w->window != NULL)
			gdk_window_set_cursor(GTK_WIDGET(w)->window, ArrowCurs);
	    }  
#endif /* _GTK */
#endif /* _WINDOWS */
	  }
     }
   else
     {
	frame = GetWindowNumber (document, view);
#ifndef _WINDOWS
	if (frame != 0)
#ifndef _GTK
	   XUndefineCursor (TtDisplay, TtaGetThotWindow (frame));
#else /* _GTK */
	    {
		w = FrameTable[frame].WdFrame;
		if (w != NULL)
		    if (w->window != NULL)
			gdk_window_set_cursor(GTK_WIDGET(w)->window, ArrowCurs);
	    }  
#endif /* _GTK */
#endif /* _WINDOWS */
     }
}

/*----------------------------------------------------------------------
   GiveClickedAbsBox retourne l'identification de la fenetre et du pave 
   designe.                                                
  ----------------------------------------------------------------------*/
void GiveClickedAbsBox (int *frame, PtrAbstractBox *pave)
{
#ifndef _WINDOWS
   ThotEvent           event;
   Drawable            drawable;
#else  /* _WINDOWS */
   MSG                 event;
   HCURSOR             cursor;          
   int                 curFrame;
#endif /* _WINDOWS */
   int                 i;
#ifdef _GTK   
   ThotWidget          w;
#endif /* _GTK */

   if (ClickIsDone == 1)
     {
       *frame = 0;
       *pave = NULL;
     }

   /* Changement du curseur */
#ifdef _WINDOWS
   cursor = LoadCursor (hInstance, MAKEINTRESOURCE (Window_Curs));
#else  /* _WINDOWS */
   for (i = 1; i <= MAX_FRAME; i++)
     {
       drawable = (Drawable)TtaGetThotWindow (i);
       if (drawable != 0)
#ifndef _GTK
	   XDefineCursor (TtDisplay, drawable, WindowCurs);
#else /* _GTK */   
	   {
	     w = FrameTable[i].WdFrame;
	     if (w && w->window)
	       gdk_window_set_cursor(GTK_WIDGET(w)->window, WindowCurs);
	   }
#endif /* _GTK */
     }
#endif /* _WINDOWS */

   /* Boucle d'attente de designation */
   ClickIsDone = 1;
   ClickFrame = 0;
   ClickX = 0;
   ClickY = 0;
   while (ClickIsDone == 1)
     {
#ifndef _WINDOWS 
       TtaFetchOneEvent (&event);
       TtaHandleOneEvent (&event);
#else /* _WINDOWS */
       GetMessage (&event, NULL, 0, 0);
       curFrame = GetFrameNumber (event.hwnd);
       TtaHandleOneEvent (&event);
       SetCursor (cursor);
#endif /* _WINDOWS */
     }

   /* Restauration du curseur */
   for (i = 1; i <= MAX_FRAME; i++)
     {
#ifndef _WINDOWS
       drawable = (Drawable)TtaGetThotWindow (i);
       if (drawable != 0)
#ifndef _GTK
	 XUndefineCursor (TtDisplay, drawable);
#else /* _GTK */
	   {
	       w = FrameTable[i].WdFrame;  
	       if (w != NULL)
		   if (w->window != NULL)
		       gdk_window_set_cursor(GTK_WIDGET(w)->window, ArrowCurs);
	   }
#endif /* _GTK */
#endif /* _WINDOWS */
     }

   *frame = ClickFrame;
   if (ClickFrame > 0 && ClickFrame <= MAX_FRAME)
     *pave = GetClickedAbsBox (ClickFrame, ClickX, ClickY);
   else
     *pave = NULL;
}

/*----------------------------------------------------------------------
  Change the window title of the frame            
  ----------------------------------------------------------------------*/
void ChangeFrameTitle (int frame, char *text)
{
  unsigned char      *title;
#ifndef _WINDOWS
  ThotWidget          w;
#endif /* _WINDOWS */
#if !defined(_GTK) && !defined(_WINDOWS)
  int                 n;
  Arg                 args[MAX_ARGS];
#endif /* _GTK  && _WINDOWS*/

#ifdef _I18N_
  title = TtaConvertMbsToByte (text, ISO_8859_1);
#else /* _I18N_ */
  title = text;
#endif /* _I18N_ */

#ifdef _WINDOWS
  if (FrMainRef [frame])
    SetWindowText (FrMainRef[frame], title);
#else  /* _WINDOWS */
  w = FrameTable[frame].WdFrame;
  if (w)
    {
#ifdef _GTK
       w = gtk_widget_get_toplevel (w);
       gtk_window_set_title (GTK_WINDOW(w), title);
#else /* _GTK */
      w = XtParent (XtParent (XtParent (w)));
      n = 0;
      XtSetArg (args[n], XmNtitle, title);
      n++;
      XtSetArg (args[n], XmNiconName, title);
      n++;
      XtSetValues (w, args, n);
#endif /* _GTK */
    }
#endif /* _WINDOWS */
#ifdef _I18N_
  TtaFreeMemory (title);
#endif /* _I18N_ */
}

/*----------------------------------------------------------------------
   La frame d'indice frame devient la fenetre active.               
  ----------------------------------------------------------------------*/
void ChangeSelFrame (int frame)
{
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
#ifndef _WINDOWS
#ifndef _GTK
	     if (w != 0)
	       XMapRaised (TtDisplay, XtWindowOfObject (XtParent (XtParent (XtParent (w)))));
#else /* _GTK */

#endif /* _GTK */
#endif /* _WINDOWS */
	  }
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
	if (FrRef[f] != 0 && FrRef[f] == w)
	   break;
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
#ifndef _WINDOWS
   *width = FrameTable[frame].FrWidth;
   *height = FrameTable[frame].FrHeight;
#else  /* _WINDOWS */
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
   DefineClipping limite la zone de reaffichage sur la fenetre frame et   
   recalcule ses limites sur l'image concrete.             
  ----------------------------------------------------------------------*/
void  DefineClipping (int frame, int orgx, int orgy, int *xd, int *yd, int *xf, int *yf, int raz)
{
   int              clipx, clipy, clipwidth, clipheight;

#ifndef _GL 
#ifndef _WINDOWS
#ifdef _GTK
   GdkRectangle      rect;
#else /* _GTK */
   XRectangle        rect;
#endif /* _GTK */
#endif /* _WINDOWS */
#endif /* _GL */

   if (*xd < *xf && *yd < *yf && orgx < *xf && orgy < *yf) {
	/* On calcule le rectangle de clipping su la fenetre */
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
#else  /* _WINDOWS */ 
#ifdef _GTK 
	rect.x = clipx;
	rect.y = clipy;
	rect.width = clipwidth;
	rect.height = clipheight;
	gdk_gc_set_clip_rectangle (TtLineGC, &rect);	
	gdk_gc_set_clip_rectangle (TtGreyGC, &rect);
	gdk_gc_set_clip_rectangle (TtGraphicGC, &rect);
#else /* _GTK */
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
#endif /* _GTK */
#endif /* _WINDOWS */
	if (raz > 0)
	  Clear (frame, clipwidth, clipheight, clipx, clipy);
#else /* _GL */
	glEnable (GL_SCISSOR_TEST);
	glScissor (clipx,
		    FrameTable[frame].FrHeight
		    + FrameTable[frame].FrTopMargin
		    - (clipy + clipheight),
		    clipwidth,
		    clipheight);
	
	/*
	  g_print ("\n%i x, %i y, %i W, %i H, %i FW, %i FH\t", 
		 clipx,
		    FrameTable[frame].FrHeight
		    + FrameTable[frame].FrTopMargin
		    - (clipy + clipheight),
		    clipwidth,
		    clipheight,
		 FrameTable[frame].FrWidth,
		 FrameTable[frame].FrHeight);
	*/
	if (raz > 0)
	  /*ClearAll (frame);*/
	  Clear (frame, clipwidth, clipheight, 
		 clipx, clipy);
#endif /*_GL*/
     }
}

/*----------------------------------------------------------------------
   RemoveClipping annule le rectangle de clipping de la fenetre frame.  
  ----------------------------------------------------------------------*/
void RemoveClipping (int frame)
{
#ifndef _GL
#ifndef _WINDOWS
#ifdef _GTK
 GdkRectangle         rect;

 rect.x = 0;
 rect.y = 0;
 rect.width = MAX_SIZE;
 rect.height = MAX_SIZE;

 gdk_gc_set_clip_rectangle (TtLineGC, &rect);
 gdk_gc_set_clip_rectangle (TtGraphicGC, &rect);
 gdk_gc_set_clip_rectangle (TtGreyGC, &rect);


#else /* _GTK */
   XRectangle          rect;

   rect.x = 0;
   rect.y = 0;
   rect.width = MAX_SIZE;
   rect.height = MAX_SIZE;
   XSetClipRectangles (TtDisplay, TtLineGC, 0, 0, &rect, 1, Unsorted);
   XSetClipRectangles (TtDisplay, TtGraphicGC, 0, 0, &rect, 1, Unsorted); 
   XSetClipRectangles (TtDisplay, TtGreyGC, 0, 0, &rect, 1, Unsorted);
   XFlushOutput (frame);
#endif /* _GTK */
#else  /* _WINDOWS */
   SelectClipRgn(TtDisplay, NULL); 
   if (clipRgn && !DeleteObject (clipRgn))
      WinErrorBox (NULL, "RemoveClipping");
   clipRgn = (HRGN) 0;
#endif /* _WINDOWS */
#else /* _GL */
   glDisable (GL_SCISSOR_TEST);
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
  ThotWidget          hscroll, vscroll;
#ifndef _WINDOWS
#ifndef _GTK
  Arg                 args[MAX_ARGS];
  int                 n;
#else /* _GTK */
  GtkAdjustment      *tmpw;  
#endif /* _GTK */
#else /* _WINDOWS */
  SCROLLINFO          scrollInfo;
#endif /* _WINDOWS */

  if (FrameUpdating ||
    documentDisplayMode[FrameTable[frame].FrDoc - 1] 
      == NoComputedDisplay)
    return;
  /* Demande le volume affiche dans la fenetre */
  ComputeDisplayedChars (frame, &x, &y, &width, &height);
  hscroll = FrameTable[frame].WdScrollH;
  vscroll = FrameTable[frame].WdScrollV;
#ifndef _WINDOWS
  l = FrameTable[frame].FrWidth;
  h = FrameTable[frame].FrHeight;
#ifndef _GTK
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
#else /*_GTK*/
 

  if ((width + vscroll->allocation.width) > l && 
      x == 0 && 
      width > 60)
    {
      if (GTK_WIDGET_VISIBLE(GTK_WIDGET (hscroll)))
	{	  
	  gtk_widget_hide (GTK_WIDGET (hscroll));
#ifdef _GL
	  gl_window_resize(frame, 0, -hscroll->allocation.height);
#endif /*_GL*/
	}
    }  
  else 
    if (width + x <= l)
      {
	tmpw = gtk_range_get_adjustment (GTK_RANGE (hscroll));
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
#ifdef _GL
	    gl_window_resize(frame, 0, hscroll->allocation.height);
#endif /*_GL*/
	  }  
      }
  
  if ((height + hscroll->allocation.height) > h && 
      y == 0)
    {
      if (GTK_WIDGET_VISIBLE(GTK_WIDGET (vscroll)))
	{
	  gtk_widget_hide (GTK_WIDGET (vscroll));
#ifdef _GL
	  gl_window_resize (frame, -vscroll->allocation.width, 0);
#endif /*_GL*/
	}
    }  
    else 
      if (height + y <= h)
	{
	  tmpw = gtk_range_get_adjustment (GTK_RANGE (vscroll));
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
#ifdef _GL
	      gl_window_resize (frame, vscroll->allocation.width, 0);
#endif /*_GL*/
	    }
	}
  
#endif /*_GTK*/  
#else  /* _WINDOWS */
  l = FrameTable[frame].FrWidth;
  h = FrameTable[frame].FrHeight;

  scrollInfo.cbSize = sizeof (SCROLLINFO);
  scrollInfo.fMask  = SIF_PAGE | SIF_POS | SIF_RANGE;
  scrollInfo.nMin   = 0;
  if (width >= l && x == 0 && width > 60)
    /*hide*/
    ShowScrollBar (FrameTable[frame].WdScrollH, SB_CTL, FALSE);
  else if (width + x <= l)
    {
      /*show*/
      ShowScrollBar (FrameTable[frame].WdScrollH, SB_CTL, TRUE);
      scrollInfo.nMax   = l;
      scrollInfo.nPage  = width;
      scrollInfo.nPos   = x;	 
      SetScrollInfo (FrameTable[frame].WdScrollH, SB_CTL, &scrollInfo, TRUE);	
    }
  else
    /*show*/
    ShowScrollBar (FrameTable[frame].WdScrollH, SB_CTL, FALSE);
   
  if (height >= h && y == 0)
    /*hide*/
    ShowScrollBar(FrameTable[frame].WdScrollV, SB_CTL, TRUE);
  else if (height + y <= h)
    {
      /*show*/
      ShowScrollBar (FrameTable[frame].WdScrollV, SB_CTL, TRUE);
      scrollInfo.nMax   = h;
      scrollInfo.nPage  = height;
      scrollInfo.nPos   = y;
      SetScrollInfo (FrameTable[frame].WdScrollV, SB_CTL, &scrollInfo, TRUE);
    }
  else
    /*show*/
    ShowScrollBar (FrameTable[frame].WdScrollV, SB_CTL, TRUE);
#endif /* _WINDOWS */
}
