/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2001.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * Handle application frames
 *
 * Authors: I. Vatton (INRIA)
 *          R. Guetari (W3C/INRIA) - Windows version
 *
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
#ifdef _WINDOWS
#include "winsys.h"
#include "wininclude.h"
#else /* _WINDOWS */
#define MAX_ARGS 20
static Time         T1;
static XmString  null_string;
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

static HWND      winCapture = (HWND) -1;
static HWND      hwndHead;
static char     *txtZoneLabel;
static char      URL_txt [500];
static int       oldXPos;
static int       oldYPos;
static ThotBool  fBlocking;
static ThotBool  moved = FALSE;
static ThotBool  firstTime = TRUE;
static ThotBool  paletteRealized = FALSE;
static ThotBool  AutoScroll = FALSE;

int         X_Pos;
int         Y_Pos;
int         cyToolBar;
DWORD       dwToolBarStyles   = WS_CHILD | WS_VISIBLE | CCS_TOP | TBSTYLE_TOOLTIPS;
DWORD       dwStatusBarStyles = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | CCS_BOTTOM | SBARS_SIZEGRIP;
TBADDBITMAP ThotTBBitmap;

#include "wininclude.h"
#include "appdialogue_f.h"
#include "inites_f.h"

#ifndef _WIN_PRINT
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
      GetClientRect (FrRef [frame], &cltRect);
      if (AutoScroll)
	ScrollDC (TtDisplay, xf - xd, yf - yd, NULL, &cltRect, NULL, NULL);
      else 
	ScrollWindowEx (FrRef [frame], xf - xd, yf - yd, NULL, &cltRect,
			NULL, NULL, SW_INVALIDATE);
    }
}
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
#else /* !_GTK */
#include "appli_f.h"
#include "input_f.h"
#endif /* !_GTK */

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
   FrameToView retourne, sous la forme qui convient a l'API Thot, 
   les parametres identifiant le document et la vue        
   qui correspondent a une frame donnee.                   
  ----------------------------------------------------------------------*/
void FrameToView (int frame, int *doc, int *view)
{
   int                 i;
   PtrDocument         pDoc;
   ThotBool            assoc;

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
	     GetViewFromFrame (frame, pDoc, &i, &assoc);
	     if (assoc)
		*view = i + 100;
	     else
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


#ifdef _WINDOWS
/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
static void   CopyToolTipText (int frame, LPTOOLTIPTEXT lpttt)
{
   char      *pString;
   char      *pDest = lpttt->lpszText;
   int        i;
   int        iButton = lpttt->hdr.idFrom;

   /* Map command ID to string index */
   for (i = 1; FrameTable[frame].ButtonId[i] != -1; i++) {
       if (FrameTable[frame].ButtonId[i] == iButton) {
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
 RECT                rect;
 ViewFrame          *pFrame;
 int                 xmin, xmax, ymin, ymax;

 if (frame > 0 && frame <= MAX_FRAME)
 {
   /* Do not redraw if the document is in NoComputedDisplay mode. */
   if (documentDisplayMode[FrameTable[frame].FrDoc - 1] != NoComputedDisplay)
   {
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
   int                 view;
   Document            doc;

   if ((width <= 0) || (height <= 0))
      return;

   if (documentDisplayMode[FrameTable[frame].FrDoc - 1] == NoComputedDisplay)
      return;

   FrameToView (frame, &doc, &view);
   FrameTable[frame].FrTopMargin = top_delta;

   /* FrameTable[frame].FrWidth = (int) width - bottom_delta; */
   FrameTable[frame].FrWidth = (int) width - bottom_delta;
   FrameTable[frame].FrHeight = (int) height;

   /* need to recompute the content of the window */
   RebuildConcreteImage (frame);

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

/*----------------------------------------------------------------------
   FrameResized Evenement sur une frame document.                              
  ----------------------------------------------------------------------*/
#ifdef _GTK
void FrameResized (GtkWidget *w, GdkEventConfigure *event, gpointer data)
{
  int frame;
  Dimension           width, height;
 
  frame = (int )data;
  width = w->allocation.width;
  height = w->allocation.height;
  FrameRedraw (frame, width, height);
}
#else /* _GTK */
void    FrameResized (int *w, int frame, int *info)
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
#endif /* !_WINDOWS */

#ifdef _WINDOWS
/*----------------------------------------------------------------------
   Demande de scroll vertical.                                      
  ----------------------------------------------------------------------*/
void WIN_ChangeVScroll (int frame, int reason, int value)
{
   int        delta, Xpos, Ypos, width, height;
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
       ComputeDisplayedChars (frame, &Xpos, &Ypos, &width, &height);
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
	 } else {
	   delta = (int) (((float)value / (float)FrameTable[frame].FrHeight) * 100);
	   JumpIntoView (frame, delta);
	 }
       break;
   }
}

/*----------------------------------------------------------------------
   Demande de scroll vertical.                                      
  ----------------------------------------------------------------------*/
void WIN_ChangeHScroll (int frame, int reason, int value)
{
   int        delta = 0, width = 1076, Xpos, Ypos, height;
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
       ComputeDisplayedChars (frame, &Xpos, &Ypos, &width, &height);
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
void FrameHScrolled (int *w, int frame, int *param)
{
#ifndef _GTK
   int                 delta, l;
   int                 n, view;
   Arg                 args[MAX_ARGS];
   NotifyWindow        notifyDoc;
   Document            doc;
   XmScrollBarCallbackStruct *info;

   info = (XmScrollBarCallbackStruct *) param;
   /* ne pas traiter si le document est en mode NoComputedDisplay */
   if (documentDisplayMode[FrameTable[frame].FrDoc - 1] == NoComputedDisplay)
      return;

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

   notifyDoc.event = TteViewScroll;
   FrameToView (frame, &doc, &view);
   notifyDoc.document = doc;
   notifyDoc.view = view;
   notifyDoc.verticalValue = 0;
   notifyDoc.horizontalValue = delta;
   if (!CallEventType ((NotifyEvent *) & notifyDoc, TRUE))
     {
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
	       delta = FrameTable[frame].FrScrollOrg + delta - ViewFrameTable[frame - 1].FrXOrg;
	     }
	 }
       else if (info->reason == XmCR_TO_TOP)
	 /* force the left alignment */
	 delta = -FrameTable[frame].FrScrollWidth;
       else if (info->reason == XmCR_TO_BOTTOM)
	 /* force the right alignment */
	 delta = FrameTable[frame].FrScrollWidth;

       HorizontalScroll (frame, delta, 1);
       notifyDoc.document = doc;
       notifyDoc.view = view;
       notifyDoc.verticalValue = 0;
       notifyDoc.horizontalValue = delta;
       CallEventType ((NotifyEvent *) & notifyDoc, FALSE);
     }
#endif /* _GTK */
}


/*----------------------------------------------------------------------
   Demande de scroll vertical.                                      
  ----------------------------------------------------------------------*/
void FrameVScrolled (int *w, int frame, int *param)
{
#ifndef _GTK
  int                 delta;
  int                 n, view;
  int                 h, y;
  int                 start, end, total;
  Arg                 args[MAX_ARGS];
  float               carparpix;
  NotifyWindow        notifyDoc;
  Document            doc;
  XmScrollBarCallbackStruct *infos;

  infos = (XmScrollBarCallbackStruct *) param;
  /* ne pas traiter si le document est en mode NoComputedDisplay */
  if (documentDisplayMode[FrameTable[frame].FrDoc - 1] == NoComputedDisplay)
    return;

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
  FrameToView (frame, &doc, &view);
  notifyDoc.document = doc;
  notifyDoc.view = view;
  notifyDoc.verticalValue = delta;
  notifyDoc.horizontalValue = 0;
  if (!CallEventType ((NotifyEvent *) & notifyDoc, TRUE))
    {
      if (infos->reason == XmCR_VALUE_CHANGED || infos->reason == XmCR_DRAG)
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
	  /* au retour n = 0 si l'Picture est complete */
	  /* Calcule le nombre de caracteres represente par un pixel */
	  carparpix = (float) total / (float) FrameTable[frame].FrHeight;
	  y = (int) ((float) infos->value * carparpix);
      
	  if (n == 0 || (y >= start && y <= total - end))
	    {
	      /* On se deplace a l'interieur de l'Picture Concrete */
	      /* Calcule la portion de scroll qui represente l'Picture Concrete */
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
	  else
	    {
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
#endif /* _GTK */
}
#endif /* !_WINDOWS */

/*----------------------------------------------------------------------
   TtcLineUp scrolls one line up.                                    
  ----------------------------------------------------------------------*/
void TtcLineUp (Document document, View view)
{
#ifndef _WINDOWS
  XmScrollBarCallbackStruct infos;
#else   /* _WINDOWS */
  int                       delta;
#endif  /* _WINDOWS */
  int                       frame;
  
  if (document != 0)
    frame = GetWindowNumber (document, view);
  else
    frame = 0;

#ifndef _WINDOWS
  infos.reason = XmCR_DECREMENT;
  FrameVScrolled (0, frame, (int *) &infos);
#else  /* _WINDOWS */
  delta = -13;
  VerticalScroll (frame, delta, 1);
#endif /* _WINDOWS */
}

/*----------------------------------------------------------------------
   TtcLineDown scrolls one line down.                                
  ----------------------------------------------------------------------*/
void TtcLineDown (Document document, View view)
{
#ifndef _WINDOWS
  XmScrollBarCallbackStruct infos;
#else   /* _WINDOWS */
  int                       delta;
#endif  /* _WINDOWS */
  int                       frame;

  if (document != 0)
    frame = GetWindowNumber (document, view);
  else
    frame = 0;

#ifndef _WINDOWS
  infos.reason = XmCR_INCREMENT;
  FrameVScrolled (0, frame, (int *) &infos);
#else  /* _WINDOWS */
  delta = 13;
  VerticalScroll (frame, delta, 1);
#endif /* _WINDOWS */
}

/*----------------------------------------------------------------------
   TtcScrollLeft scrolls one position left.                                    
  ----------------------------------------------------------------------*/
void TtcScrollLeft (Document document, View view)
{
#ifndef _WINDOWS
  XmScrollBarCallbackStruct infos;
#else   /* _WINDOWS */
  int                       delta;
#endif  /* _WINDOWS */
  int                       frame;
  
  if (document != 0)
    frame = GetWindowNumber (document, view);
  else
    frame = 0;

#ifndef _WINDOWS
  infos.reason = XmCR_DECREMENT;
  FrameHScrolled (0, frame, (int *) &infos);
#else  /* _WINDOWS */
  delta = -13;
  HorizontalScroll (frame, delta, 1);
#endif /* _WINDOWS */
}

/*----------------------------------------------------------------------
   TtcScrollRight scrolls one position right.                                
  ----------------------------------------------------------------------*/
void TtcScrollRight (Document document, View view)
{
#ifndef _WINDOWS
  XmScrollBarCallbackStruct infos;
#else   /* _WINDOWS */
  int                       delta;
#endif  /* _WINDOWS */
  int                       frame;

  if (document != 0)
    frame = GetWindowNumber (document, view);
  else
    frame = 0;

#ifndef _WINDOWS
  infos.reason = XmCR_INCREMENT;
  FrameHScrolled (0, frame, (int *) &infos);
#else  /* _WINDOWS */
  delta = 13;
  HorizontalScroll (frame, delta, 1);
#endif /* _WINDOWS */
}

/*----------------------------------------------------------------------
   PageUp scrolls one screen up.                                    
  ----------------------------------------------------------------------*/
void TtcPageUp (Document document, View view)
{
   int                 frame;
#ifndef _WINDOWS
   XmScrollBarCallbackStruct infos;
#else   /* _WINDOWS */
   int delta;
#endif  /* _WINDOWS */

   if (document != 0)
      frame = GetWindowNumber (document, view);
   else
     frame = 0;
#ifndef _WINDOWS
   infos.reason = XmCR_PAGE_DECREMENT;
   FrameVScrolled (0, frame, (int *) &infos);
#else  /* _WINDOWS */
   delta = -FrameTable[frame].FrHeight;
   VerticalScroll (frame, delta, 1);
#endif /* _WINDOWS */
}

/*----------------------------------------------------------------------
   PageDown scrolls one screen down.                                
  ----------------------------------------------------------------------*/
void TtcPageDown (Document document, View view)
{
   int                 frame;

#ifndef _WINDOWS
   XmScrollBarCallbackStruct infos;
#else   /* _WINDOWS */
   int delta;
#endif  /* !_WINDOWS */

   if (document != 0)
      frame = GetWindowNumber (document, view);
   else
     frame = 0;
#ifndef _WINDOWS
   infos.reason = XmCR_PAGE_INCREMENT;
   FrameVScrolled (0, frame, (int *) &infos);
#else  /* _WINDOWS */
   delta = FrameTable[frame].FrHeight;
   VerticalScroll (frame, delta, 1);
#endif /* _WINDOWS */
}


/*----------------------------------------------------------------------
   PageTop goes to the document top.                                
  ----------------------------------------------------------------------*/
void TtcPageTop (Document document, View view)
{
#ifdef _WINDOWS
   int                 frame;

   if (document != 0)
      frame = GetWindowNumber (document, view);
   else
     frame = 0;
   JumpIntoView (frame, 0);
#else  /* !_WINDOWS */
   XmScrollBarCallbackStruct infos;
   int                 frame;

   if (document != 0)
      frame = GetWindowNumber (document, view);
   else
     frame = 0;
   infos.reason = XmCR_TO_TOP;
   FrameVScrolled (0, frame, (int *) &infos);
#endif /* _WINDOWS */
}

/*----------------------------------------------------------------------
   PageEnd goes to the document end.                                
  ----------------------------------------------------------------------*/
void TtcPageEnd (Document document, View view)
{
#ifdef _WINDOWS
   int                 frame;

   if (document != 0)
      frame = GetWindowNumber (document, view);
   else
     frame = 0;
   JumpIntoView (frame, 100);
#else  /* !_WINDOWS */
   XmScrollBarCallbackStruct infos;
   int                 frame;

   if (document != 0)
      frame = GetWindowNumber (document, view);
   else
     frame = 0;
   infos.reason = XmCR_TO_BOTTOM;
   FrameVScrolled (0, frame, (int *) &infos);
#endif /* _WINDOWS */
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
   null_string = XmStringCreateSimple ("");
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
	/* traite les vues de l'arbre principal */
	for (v = 0; v < MAX_VIEW_DOC; v++)
	  if (pDoc->DocView[v].DvPSchemaView > 0)
	    ChangeFrameTitle (pDoc->DocViewFrame[v], title);
	/* traite les vues des elements associes */
	for (v = 0; v < MAX_ASSOC_DOC; v++)
	  if (pDoc->DocAssocRoot[v] != NULL)
	    if (pDoc->DocAssocFrame[v] != 0)
	       ChangeFrameTitle (pDoc->DocAssocFrame[v], title);
      }
}

/*----------------------------------------------------------------------
   Map and raise the corresponding window.                          
  ----------------------------------------------------------------------*/
void TtaRaiseView (Document document, View view)
{
#ifndef _GTK
   int                 idwindow;
   ThotWidget          w;

   UserErrorCode = 0;
   idwindow = GetWindowNumber (document, view);
   if (idwindow != 0)
     {
	w = FrameTable[idwindow].WdFrame;
#   ifndef _WINDOWS
	if (w != 0)
	   XMapRaised (TtDisplay, XtWindowOfObject (XtParent (XtParent (XtParent (w)))));
#   else  /* _WINDOWS */
	SetForegroundWindow (FrMainRef [idwindow]);
#endif /* _WINDOWS */
     }
#endif /*_GTK */
}

/*----------------------------------------------------------------------
   DisplaySelMessage affiche la se'lection donne'e en parame`tre (texte) dans 
   la fenetre active.                                            
  ----------------------------------------------------------------------*/
void DisplaySelMessage (char *text, PtrDocument pDoc)
{
   int                 doc;
   int                 view;

   if (ActiveFrame != 0 && (strcmp (OldMsgSelect, text) ||pDoc != OldDocMsgSelect))
     {
	/* recupere le document concerne */
	doc = FrameTable[ActiveFrame].FrDoc;
	for (view = 1; view <= MAX_VIEW_DOC; view++)
	  {
	     TtaSetStatus ((Document) doc, view, text, NULL);
	     if (view < MAX_ASSOC_DOC)
               TtaSetStatus ((Document) doc, view + 100, text, NULL);
	  }
	/* sel old message */
	strncpy (OldMsgSelect, text, MAX_TXT_LEN);
	OldDocMsgSelect = pDoc;
     }
}

/*----------------------------------------------------------------------
   TtaSetStatus affiche le status de la vue du document.                      
  ----------------------------------------------------------------------*/
void TtaSetStatus (Document document, View view, CONST char *text,
		   CONST char *name)
{
   int                 frame;
   char              s[MAX_LENGTH];
#ifndef _WINDOWS
   Arg                 args[MAX_ARGS];
   XmString            title_string;
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
#ifdef _WINDOWS
	     if (name)
	       /* text est un format */
	       sprintf (s, text, name);
	     else
	       strncpy (s, text, MAX_LENGTH);

	     SendMessage (FrameTable[frame].WdStatus, SB_SETTEXT, (WPARAM) 0, (LPARAM) & s[0]);
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
#endif /* _GTK */
#endif /* _WINDOWS */
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
  int                 index = 0;
  int                 cyTxtZone;
  DWORD               dwStyle;
  ThotBool            assoc;

  frame = GetMainFrameNumber (hwnd);
  if (frame != -1)
    currentFrame = frame;
  GetWindowRect (hwnd, &rect);
  switch (mMsg)
  {
  case WM_CREATE:
    /* Create toolbar  */
    ThotTBBitmap.hInst = hInstance;
    ThotTBBitmap.nID   = IDR_TOOLBAR;    
    ToolBar = CreateWindow (TOOLBARCLASSNAME, NULL, dwToolBarStyles,
			    0, 0, 0, 0, hwnd, (HMENU) 1, hInstance, 0);
    SendMessage (ToolBar, TB_BUTTONSTRUCTSIZE, (WPARAM) sizeof (TBBUTTON), 0L);
    if ((SendMessage (ToolBar, TB_ADDBITMAP, (WPARAM) MAX_BUTTON, (LPARAM) (LPTBADDBITMAP) &ThotTBBitmap)) == -1)
      WinErrorBox (NULL, "WndProc: WM_CREATE");
    
    hwndToolTip = ToolBar_GetToolTips (ToolBar);
    if (dwToolBarStyles & TBSTYLE_TOOLTIPS)
      InitToolTip (ToolBar);	
    
    /* Create status bar  */
    StatusBar = CreateStatusWindow (dwStatusBarStyles, "", hwnd, 2);
    ShowWindow (StatusBar, SW_SHOWNORMAL);
    UpdateWindow (StatusBar);
    
    /* Create client window */
    hwndClient = CreateWindowEx (WS_EX_CLIENTEDGE, "ClientWndProc", NULL,
				 WS_CHILD | WS_VISIBLE | WS_BORDER, 0, 0, 0, 0,
				 hwnd, (HMENU) 2, hInstance, NULL);
    ShowWindow (hwndClient, SW_SHOWNORMAL);
    UpdateWindow (hwndClient);
    SetWindowText (hwnd, wTitle);
    DragAcceptFiles (hwnd, TRUE);
    return 0L;

  case WM_PALETTECHANGED: 
    if ((HWND) wParam != hwnd)
      {
	HDC hDC = GetDC (hwnd);
	SelectPalette (hDC, TtCmap, FALSE);
	if (RealizePalette (hDC))
	  UpdateColors (hDC);
	ReleaseDC (hwnd, hDC);
      }
    return 0L;

  case WM_VSCROLL:
    WIN_ChangeVScroll (frame, LOWORD (wParam), HIWORD (wParam));
    return 0L;

  case WM_HSCROLL:
    WIN_ChangeHScroll (frame, LOWORD (wParam), HIWORD (wParam));
    return 0L;

  case WM_ENTER:
    hwndTextEdit = GetFocus ();
    APP_TextCallback (hwndTextEdit, frame, NULL);
    if (frame != -1)
      SetFocus (FrRef [frame]);
    return 0L;

  case WM_SYSKEYDOWN:
  case WM_KEYDOWN:
    if (frame != -1)
      SetFocus (FrRef [frame]);
    SendMessage (FrRef [frame], mMsg, wParam, lParam);
    return 0L;

  case WM_DROPFILES:
    if (frame != -1)
      SetFocus (FrRef [frame]);
    SendMessage (FrRef [frame], mMsg, wParam, lParam);
    return 0L;
	
  case WM_IME_CHAR:
  case WM_SYSCHAR:
  case WM_CHAR:
    if (frame != -1)
      SetFocus (FrRef [frame]);
    SendMessage (FrRef [frame], mMsg, wParam, lParam);
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
	GetDocAndView (frame, &pDoc, &view, &assoc);
	if (pDoc && view)
	  CloseView (pDoc, view, assoc);
	if (FrameTable[frame].FrDoc == 0)
	  FrMainRef[frame] = 0;
 
	if (mMsg == WM_DESTROY)
	  {
	    for (frameNDX = 0; frameNDX <= MAX_FRAME; frameNDX++)
	      if (FrMainRef[frameNDX] != 0)
		{
		  /* there is still an active frame */
		  FrMainRef[frame] = 0;
		  return 0L;
		}
	    TtaQuit();
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
    for (index = 0; index < MAX_TEXTZONE; index++)
      {
	if (FrameTable[frame].Text_Zone[index] &&
	    IsWindowVisible (FrameTable[frame].Text_Zone[index]))
	  {
	    MoveWindow (FrameTable[frame].Label[index], 15, cyTxtZone + 5, 70, 20, TRUE);
	    MoveWindow (FrameTable[frame].Text_Zone[index], 85, cyTxtZone + 5, cx - 100, 20, TRUE);
	    cyTxtZone += 25;
	  }
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
  int          delta;
  int          key;
  int          document, view;
  UINT         i, nNumFiles;
  RECT         rect;
  RECT         cRect;

  frame = GetFrameNumber (hwnd);
  if (frame != -1) 
    currentFrame = frame;
  
  
  /* do not handle events if the Document is in NoComputedDisplay mode. */
  if (frame != -1)
    {
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

  /* if (frame != -1 && winCapture != hwnd && fBlocking) { */
  if (wParam & MK_LBUTTON && frame != -1 && winCapture != (HWND) -1)
    {
      if (winCapture != hwnd)
	{
      GetWindowRect (hwnd, &rect);
      GetClientRect (hwnd, &cRect);
	  GetCursorPos (&ptCursor);
	  if (ptCursor.y > rect.bottom || ptCursor.y < rect.top)
	    {
	      if (ptCursor.y > rect.bottom)
		{
		  delta = 13;
		  Y_Pos = cRect.bottom;
		}
	      if (ptCursor.y < rect.top)
		{
		  delta = -13;
		  Y_Pos = 0;
		}
	      
	      if (ptCursor.x > rect.right)
		X_Pos = cRect.right;
	      VerticalScroll (frame, delta, 1);
	      LocateSelectionInView (frame, X_Pos, Y_Pos, 0);
	      /* if (wParam & MK_LBUTTON) */
	      SendMessage (hwnd, WM_MOUSEMOVE, 0, 0L);
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
      FrameToView (frame, &document, &view);
      if (FrameTable[frame].Call_Text[0] != NULL)
	for (i = 0; i < nNumFiles; i++)
	  {
	    DragQueryFile ((HDROP)wParam, i, DroppedFileName, MAX_PATH + 1);
	    /* call the first text-zone entry with the current text */
	    (*FrameTable[frame].Call_Text[0]) (document, view, DroppedFileName);
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
	  key = (int) wParam;
	  if (WIN_TtaHandleMultiKeyEvent (mMsg, wParam, lParam, (int *)&key))
	    WIN_CharTranslation (FrRef[frame], frame, mMsg, (WPARAM) key, lParam, TRUE);
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

    case WM_LBUTTONDOWN:
      /* Activate the client window */
      SetFocus (FrRef[frame]);
      ClickFrame = frame;
      oldXPos = ClickX = LOWORD (lParam);
      oldYPos = ClickY = HIWORD (lParam);
      /* stop any current insertion of text */
      CloseInsertion ();
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
	  fBlocking = TRUE;
	  moved = FALSE;
	}
      return 0;

    case WM_LBUTTONUP:
      X_Pos = LOWORD (lParam);
      Y_Pos = HIWORD (lParam);
      ReleaseCapture ();
      winCapture = (HWND) -1;
      firstTime = TRUE;
      if (fBlocking)
	fBlocking = FALSE;
      /* is it a single click */
      fBlocking = TRUE;
      if (!moved)	  
	LocateSelectionInView (frame, ClickX, ClickY, 4);
      if (AutoScroll)
	AutoScroll = FALSE;
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
	LocateSelectionInView (frame, ClickX, ClickY, 5);
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

    case WM_MOUSEMOVE:
      /* SetActiveWindow (FrMainRef[frame]); 
	 SetFocus (hwnd); */
      X_Pos = LOWORD (lParam);
      Y_Pos = HIWORD (lParam);
      if (wParam & MK_LBUTTON)
	{
	  if ((oldXPos <= X_Pos - 1 || oldXPos >= X_Pos + 1) ||  
	      (oldYPos <= Y_Pos - 1 || oldYPos >= Y_Pos + 1))
	    {
	      LocateSelectionInView (frame, X_Pos, Y_Pos, 0);
	      moved = TRUE;
	    }
	}
      else
	fBlocking = FALSE;
      oldXPos = X_Pos;
      oldYPos = Y_Pos;
      return 0;

    case WM_NCMOUSEMOVE:
      if (firstTime && fBlocking)
	{
	  winCapture = GetCapture ();
	  AutoScroll = TRUE;
	  firstTime = FALSE;
	  SetCapture (hwnd);
	}
      return 0;

    case WM_CLOSE:
    case WM_DESTROY: 
      if (frame > 0 && frame <= MAX_FRAME)
	FrRef[frame] = 0;
      PostQuitMessage (0);
      return 0;
      
    default:
      break;
    }
  return (DefWindowProc (hwnd, mMsg, wParam, lParam));
}
#endif /* _WINDOWS */

#ifndef _WINDOWS
/*----------------------------------------------------------------------
   Evenement sur une frame document.                              
   D.V. equivalent de la fontion MS-Windows ci dessus !           
  ----------------------------------------------------------------------*/
void FrameCallback (int frame, void *evnt)
{
  ThotEvent           event;
  ThotEvent          *ev = (ThotEvent *) evnt;
  Document            document;
  View                view;
  int                 comm, dx, dy, sel, h;

  /* ne pas traiter si le document est en mode NoComputedDisplay */
  if (FrameTable[frame].FrDoc == 0 ||
      documentDisplayMode[FrameTable[frame].FrDoc - 1] == NoComputedDisplay)
    return;
  /*_______> S'il n'y a pas d'evenement associe */
  else if (ev == NULL)
    return;
  /*_______> Si une designation de pave est attendue*/
  else if (ClickIsDone == 1 &&
	   (ev->type == ButtonPress || ev->type == KeyPress))
    {
      ClickIsDone = 0;
      ClickFrame = frame;
      ClickX = ev->xbutton.x;
      ClickY = ev->xbutton.y;
      return;
    }

  /* S'il y a un TtaWaitShowDialogue en cours on n'autorise pas de changer */
  /* la selection courante. */
  if (TtaTestWaitShowDialogue ()
      && (ev->type != ButtonPress || (ev->xbutton.state & THOT_KEY_ControlMask) == 0))
    return;
  
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
	      T1 = 0;
	    }
	  else if ((ev->xbutton.state & THOT_KEY_ShiftMask) != 0)
	    {
	      /* a selection extension */
	      TtaAbortShowDialogue ();
	      LocateSelectionInView (frame, ev->xbutton.x, ev->xbutton.y, 0);
	      FrameToView (frame, &document, &view);
	      TtcCopyToClipboard (document, view);
	      T1 = 0;
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
			  if (event.xmotion.y > h)
			    TtcLineDown (document, view);
			  else if (event.xmotion.y < 0)
			    TtcLineUp (document, view);
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
	      T1 = 0;
	    }
	  else
	    {
	      if (T1 + (Time) DoubleClickDelay > ev->xbutton.time)
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
	      T1 = ev->xbutton.time;
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
	      T1 = 0;
	    }
	  else
	    {
	      if (T1 + (Time) DoubleClickDelay > ev->xbutton.time)
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
	      T1 = ev->xbutton.time;
	      ClickFrame = frame;
	      ClickX = ev->xbutton.x;
	      ClickY = ev->xbutton.y;
	      LocateSelectionInView (frame, ClickX, ClickY, 6);
	    }
	default:
	  break;
	}
      break;

    case KeyPress:
      T1 = 0;
      TtaAbortShowDialogue ();
#ifndef _GTK
      CharTranslation ((ThotKeyEvent *)ev);
#endif /* _GTK */
      break;
    case EnterNotify:
    case LeaveNotify:
      T1 = 0;
      break;
    default:
      break;
    }
}
#endif /* _WINDOWS */


/*----------------------------------------------------------------------
   ThotGrab fait un XGrabPointer.                                  
  ----------------------------------------------------------------------*/
void ThotGrab (ThotWindow win, ThotCursor cursor, long events, int disp)
{
#ifndef _WINDOWS
   XGrabPointer (TtDisplay, win, FALSE, events, GrabModeAsync, GrabModeAsync,
		 win, cursor, CurrentTime);
#endif /* _WINDOWS */
}


/*----------------------------------------------------------------------
   ThotUngrab est une fonction d'interface pour UnGrab.            
  ----------------------------------------------------------------------*/
void ThotUngrab ()
{
#ifndef _WINDOWS
   XUngrabPointer (TtDisplay, CurrentTime);
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
#  ifndef _WINDOWS
   Drawable            drawable;

   drawable = TtaGetThotWindow (thotThotWindowid);
   XDefineCursor (TtDisplay, drawable, WaitCurs);
#  else  /* _WINDOWS */
   SetCursor (LoadCursor (NULL, IDC_WAIT));
   ShowCursor (TRUE);
#  endif /* _WINDOWS */
}

/*----------------------------------------------------------------------
   ResetCursorWatch enleve le curseur "montre".                 
  ----------------------------------------------------------------------*/
void ResetCursorWatch (int thotThotWindowid)
{
#  ifndef _WINDOWS
   Drawable            drawable;

   drawable = TtaGetThotWindow (thotThotWindowid);
   XUndefineCursor (TtDisplay, drawable);
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
#  endif  /* _WINDOWS */

   UserErrorCode = 0;
   /* verifie le parametre document */
   if (document == 0 && view == 0)
     {
	for (frame = 1; frame <= MAX_FRAME; frame++)
	  {
#        ifndef _WINDOWS
	     drawable = TtaGetThotWindow (frame);
	     if (drawable != 0)
		XDefineCursor (TtDisplay, drawable, WaitCurs);
#        endif /* _WINDOWS */
	  }
     }
   else
     {
	frame = GetWindowNumber (document, view);
#   ifndef _WINDOWS
	if (frame != 0)
	   XDefineCursor (TtDisplay, TtaGetThotWindow (frame), WaitCurs);
#   endif /* _WINDOWS */
     }
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void TtaResetCursor (Document document, View view)
{
   int                 frame;
#  ifndef _WINDOWS 
   Drawable            drawable;
#  endif /* _WINDOWS */

   UserErrorCode = 0;
   /* verifie le parametre document */
   if (document == 0 && view == 0)
     {
	for (frame = 1; frame <= MAX_FRAME; frame++)
	  {
#ifndef _WINDOWS
	     drawable = TtaGetThotWindow (frame);
	     if (drawable != 0)
		XUndefineCursor (TtDisplay, drawable);
#endif /* _WINDOWS */
	  }
     }
   else
     {
	frame = GetWindowNumber (document, view);
#ifndef _WINDOWS
	if (frame != 0)
	   XUndefineCursor (TtDisplay, TtaGetThotWindow (frame));
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

   if (ClickIsDone == 1)
     {
       *frame = 0;
       *pave = NULL;
     }

   /* Changement du curseur */
#  ifdef _WINDOWS
   cursor = LoadCursor (hInstance, MAKEINTRESOURCE (Window_Curs));
#  else  /* !_WINDOWS */
   for (i = 1; i <= MAX_FRAME; i++)
     {
       drawable = TtaGetThotWindow (i);
       if (drawable != 0)
          XDefineCursor (TtDisplay, drawable, WindowCurs);
     }
#  endif /* !_WINDOWS */

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
#endif /* !_WINDOWS */
     }

   /* Restauration du curseur */
   for (i = 1; i <= MAX_FRAME; i++)
     {
#ifndef _WINDOWS
       drawable = TtaGetThotWindow (i);
       if (drawable != 0)
	 XUndefineCursor (TtDisplay, drawable);
#endif /* _WINDOWS */
     }

   *frame = ClickFrame;
   if (ClickFrame > 0 && ClickFrame <= MAX_FRAME)
     *pave = GetClickedAbsBox (ClickFrame, ClickX, ClickY);
   else
     *pave = NULL;
}

/*----------------------------------------------------------------------
   Modifie le titre de la fenetre d'indice frame.                     
  ----------------------------------------------------------------------*/
void ChangeFrameTitle (int frame, char *text)
{
#  ifdef _WINDOWS
   if (FrMainRef [frame] != 0)
      SetWindowText (FrMainRef[frame], text);
#  else  /* !_WINDOWS */
#  ifndef _GTK
   int                 n;
   Arg                 args[MAX_ARGS];
   ThotWidget          w;

   w = FrameTable[frame].WdFrame;
   if (w != 0)
     {
        w = XtParent (XtParent (XtParent (w)));
        n = 0;
        XtSetArg (args[n], XmNtitle, text);
        n++;
        XtSetArg (args[n], XmNiconName, text);
        n++;
        XtSetValues (w, args, n);
     }
#   endif /* _GTK */
#   endif /* _WINDOWS */
}

/*----------------------------------------------------------------------
   La frame d'indice frame devient la fenetre active.               
  ----------------------------------------------------------------------*/
void ChangeSelFrame (int frame)
{
#ifndef _GTK
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
	     if (w != 0)
		XMapRaised (TtDisplay, XtWindowOfObject (XtParent (XtParent (XtParent (w)))));
#endif /* _WINDOWS */
	  }
     }
#endif /* _GTK */
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
}

/*----------------------------------------------------------------------
   DefineClipping limite la zone de reaffichage sur la fenetre frame et   
   recalcule ses limites sur l'image concrete.             
  ----------------------------------------------------------------------*/
void  DefineClipping (int frame, int orgx, int orgy, int *xd, int *yd, int *xf, int *yf, int raz)
{
   int              clipx, clipy, clipwidth, clipheight;
#ifndef _WINDOWS
#ifdef _GTK 
   GdkRectangle      rect;
#else /* _GTK */
   XRectangle        rect;
#endif /* _GTK */

#endif /* _WINDOWS */

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
/*
	gdk_gc_set_clip_rectangle (TtGraphicGC, &rect);*/
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
     }
}

/*----------------------------------------------------------------------
   RemoveClipping annule le rectangle de clipping de la fenetre frame.  
  ----------------------------------------------------------------------*/
void RemoveClipping (int frame)
{
#ifndef _WINDOWS
#ifdef _GTK
  GdkRectangle         rect;

 rect.x = 0;
 rect.y = 0;
 rect.width = MAX_SIZE;
 rect.height = MAX_SIZE;

 gdk_gc_set_clip_rectangle (TtLineGC, &rect);
 /*  gdk_gc_set_clip_rectangle (TtGraphicGC, &rect);*/
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
}

/*----------------------------------------------------------------------
   UpdateScrollbars met a jour les bandes de defilement de la fenetretre    
  ----------------------------------------------------------------------*/
void UpdateScrollbars (int frame)
{
#ifndef _GTK
   int                 Xpos, Ypos;
   int                 width, height;
   int                 l, h;
   ThotWidget          hscroll, vscroll;
#ifndef _WINDOWS
   Arg                 args[MAX_ARGS];
   int                 n;
#else /* _WINDOWS */
   RECT                rWindow;
   SCROLLINFO          scrollInfo;
#endif /* _WINDOWS */

   /* Demande le volume affiche dans la fenetre */
   ComputeDisplayedChars (frame, &Xpos, &Ypos, &width, &height);
   hscroll = FrameTable[frame].WdScrollH;
   vscroll = FrameTable[frame].WdScrollV;

#ifndef _WINDOWS
   l = FrameTable[frame].FrWidth;
   h = FrameTable[frame].FrHeight;
   n = 0;
   if (width + Xpos <= l)
     {
	XtSetArg (args[n], XmNminimum, 0);
	n++;
	XtSetArg (args[n], XmNmaximum, l);
	n++;
	XtSetArg (args[n], XmNvalue, Xpos);
	n++;
	XtSetArg (args[n], XmNsliderSize, width);
	n++;
	XtSetValues (hscroll, args, n);
     }

   n = 0;
   if (height + Ypos <= h)
     {
	XtSetArg (args[n], XmNminimum, 0);
	n++;
	XtSetArg (args[n], XmNmaximum, h);
	n++;
	XtSetArg (args[n], XmNvalue, Ypos);
	n++;
	XtSetArg (args[n], XmNsliderSize, height);
	n++;
	XtSetValues (vscroll, args, n);
     }
#else  /* _WINDOWS */
   GetWindowRect (FrRef[frame], &rWindow);
   h = rWindow.bottom - rWindow.top;
   l = rWindow.right - rWindow.left;
   scrollInfo.cbSize = sizeof (SCROLLINFO);
   scrollInfo.fMask  = SIF_PAGE | SIF_POS | SIF_RANGE;
   scrollInfo.nMin   = 0;

   if (width + Xpos <= l) {
      scrollInfo.nMax   = l;
      scrollInfo.nPage  = width;
      scrollInfo.nPos   = Xpos;
      SetScrollInfo (FrameTable[frame].WdScrollH, SB_CTL, &scrollInfo, TRUE);
   }

   if (height + Ypos <= h) {
      scrollInfo.nMax   = h;
      scrollInfo.nPage  = height;
      scrollInfo.nPos   = Ypos;
      SetScrollInfo (FrameTable[frame].WdScrollV, SB_CTL, &scrollInfo, TRUE);
   }
#endif /* _WINDOWS */
#endif /* _GTK */
}
