/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * Warning:
 * This module is part of the Thot library, which was originally
 * developed in French. That's why some comments are still in
 * French, but their translation is in progress and the full module
 * will be available in English in the next release.
 * 
 */ 
 
/*
 * Handle application frames
 *
 * Author: I. Vatton (INRIA)
 *         R. Guetari (W3C/INRIA): Windows NT and Window 95 Routines
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

#define MAX_ARGS 20

#undef THOT_EXPORT
#define THOT_EXPORT extern

#ifndef _WINDOWS
static XmString  null_string;
#endif
static char         OldMsgSelect[MAX_TXT_LEN];
static PtrDocument  OldDocMsgSelect;


#include "boxes_tv.h"
#include "font_tv.h"
#include "edit_tv.h"
#include "frame_tv.h"
#include "appdialogue_tv.h"

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

extern HWND      hwndClient;
extern HWND      ToolBar;
extern HWND      logoFrame;
extern HWND      StatusBar;
extern HWND      currentWindow;
extern HWND      WIN_curWin;
extern HINSTANCE hInstance;
extern int       ReturnOption;
#ifndef _WIN_PRINT
extern int  Window_Curs;
#if 0
HBITMAP     appLogo = (HBITMAP)0;
int         cyLogo;
int         bmpID;
#endif /* 0 */
#endif /* !_WIN_PRINT */

static HWND      hwndHead;
static char*     txtZoneLabel;
static BOOL      paletteRealized = FALSE;

static char      URL_txt [500];
static char      doc_title [500];

static int       oldXPos;
static int       oldYPos;
static int       oldSPos = 0;

int         X_Pos;
int         Y_Pos;
int         cyToolBar;
int         CommandToString [MAX_BUTTON];
char        szTbStrings [4096];

boolean viewClosed = FALSE;
#ifdef THOT_TOOLTIPS
DWORD       dwToolBarStyles   = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | CCS_TOP | TBSTYLE_TOOLTIPS;
#else  /* !THOT_TOOLTIPS */
DWORD       dwToolBarStyles   = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | CCS_TOP;
#endif /* THOT_TOOLTIPS */
DWORD       dwStatusBarStyles = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | CCS_BOTTOM | SBARS_SIZEGRIP;
TBADDBITMAP ThotTBBitmap;

#ifdef THOT_TOOLTIPS
#ifdef __STDC__
BOOL InitToolTip (HWND hwndToolBar)
#else  /* __STDC__ */
BOOL InitToolTip (hwndToolBar)
HWND hwndToolBar;
#endif /* __STDC__ */
{
   BOOL     bSuccess ;
   HWND     hwndTT;
   TOOLINFO ti ;

   /* Fetch handle to tooltip control */
   hwndTT = ToolBar_GetToolTips (hwndToolBar) ;
   if (hwndTT == NULL) 
      return FALSE ;


   return bSuccess ;
}

#ifdef __STDC__
static void CopyToolTipText (LPTOOLTIPTEXT lpttt)
#else  /* __STDC__ */
static void CopyToolTipText (lpttt)
LPTOOLTIPTEXT lpttt;
#endif /* __STDC__ */
{
   int i ;
   int iButton = lpttt->hdr.idFrom ;
   int cb ;
   int cMax ;
   LPSTR pString ;
   LPSTR pDest = lpttt->lpszText ;

   /* Map command ID to string index */
   for (i = 0 ; CommandToString[i] != -1 ; i++) {
       if (CommandToString[i] == iButton) {
          iButton = i ;
          break ;
       }
   }

   /* To be safe, count number of strings in text */
   pString = &szTbStrings [0] ;
   cMax = 0 ;
   while (*pString != EOS) {
         cMax++ ;
         cb = lstrlen (pString) ;
         pString += (cb + 1) ;
   }

   /* Check for valid parameter */
   if (iButton > cMax)
      pString = "Invalid Button Index" ;
   else {
       /* Cycle through to requested string */
       pString = &szTbStrings [0] ;
       for (i = 0 ; i < iButton ; i++) {
           cb = lstrlen (pString) ;
           pString += (cb + 1) ;
       }
   }

   lstrcpy (pDest, pString) ;
}
#endif /* THOT_TOOLTIPS */
#endif /* _WINDOWS */

#include "absboxes_f.h"
#include "appli_f.h"
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
#include "input_f.h"
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
#include "viewcommands_f.h"
#include "viewapi_f.h"
#include "windowdisplay_f.h"

/*----------------------------------------------------------------------
   FrameToView retourne, sous la forme qui convient a l'API Thot, 
   les parametres identifiant le document et la vue        
   qui correspondent a une frame donnee.                   
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                FrameToView (int frame, int *doc, int *view)
#else  /* __STDC__ */
void                FrameToView (frame, doc, view)
int                 frame;
int                *doc;
int                *view;

#endif /* __STDC__ */
{
   int                 i;
   PtrDocument         pDoc;
   boolean             assoc;

   *doc = FrameTable[frame].FrDoc;
   *view = 0;
   if (doc == 0)
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
#ifdef __STDC__
void                FrameKilled (int *w, int frame, int *info)

#else  /* __STDC__ */
void                FrameKilled (w, frame, info)
int                *w;
int                 frame;
int                *info;

#endif /* __STDC__ */

{
   /* Enleve la procedure de Callback */
   /* Detruit la fenetre si elle existe encore */
   if (FrRef[frame] != 0 && frame > 0)
      ViewClosed (frame);
}


#ifdef _WINDOWS
/*----------------------------------------------------------------------
   WIN_HandleExpose deal with the redrawing of the Client Area when  
   a WM_PAINT has been received in MS-Windows.             
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void WIN_HandleExpose (ThotWindow w, int frame, WPARAM wParam, LPARAM lParam)
#else  /* !__STDC__ */
void WIN_HandleExpose (w, frame, wParam, lParam)
ThotWindow w; 
int        frame; 
WPARAM     wParam; 
LPARAM     lParam;
#endif /* __STDC__ */
{
   PAINTSTRUCT         ps;
   RECT                rect;

   if (frame > 0 && frame <= MAX_FRAME) {
      /* Do not redraw if the document is in NoComputedDisplay mode. */
      if (documentDisplayMode[FrameTable[frame].FrDoc - 1] != NoComputedDisplay) {
         TtDisplay = BeginPaint (w, &ps);
         GetClientRect (w, &rect);
         DefRegion (frame, ps.rcPaint.left, ps.rcPaint.top, ps.rcPaint.right, ps.rcPaint.bottom);
         SwitchSelection (frame, FALSE);
         RedrawFrameBottom (frame, 0);
         SwitchSelection (frame, TRUE);
         EndPaint (w, &ps);
         WIN_ReleaseDeviceContext ();
      }
   }
}
#endif /* _WINDOWS */

#ifndef _WINDOWS
/*----------------------------------------------------------------------
   FrameToRedisplay effectue le traitement des expositions X11 des     
   frames de dialogue et de documents.                   
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                FrameToRedisplay (ThotWindow w, int frame, void *ev)
#else  /* __STDC__ */
void                FrameToRedisplay (w, frame, ev)
ThotWindow          w;
int                 frame;
void               *ev;

#endif /* __STDC__ */
{
   int                 x;
   int                 y;
   int                 l;
   int                 h;
   XExposeEvent       *event = (XExposeEvent *) ev;

   x = event->x;
   y = event->y;
   l = event->width;
   h = event->height;

   if (frame > 0 && frame <= MAX_FRAME)
     {
	/* ne pas traiter si le document est en mode NoComputedDisplay */
	if (documentDisplayMode[FrameTable[frame].FrDoc - 1] == DisplayImmediately)
	  {
	     DefRegion (frame, x, y, x + l, y + h);
	     SwitchSelection (frame, FALSE);
	     RedrawFrameBottom (frame, 0);
	     SwitchSelection (frame, TRUE);
	  }
     }
}
#endif /* !_WINDOWS */

#ifdef _WINDOWS
/*----------------------------------------------------------------------
   WIN_ChangeTaille : function called when a view is resized under    
   MS-Windows.                                                   
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void WIN_ChangeViewSize (int frame, int width, int height, int top_delta, int bottom_delta)
#else  /* !__STDC__ */
void WIN_ChangeViewSize (frame, width, height, top_delta, bottom_delta)
int frame; 
int width; 
int height; 
int top_delta; 
int bottom_delta;
#endif /* __STDC__ */
{
   int                 view;
   Document            doc;

   if ((width <= 0) || (height <= 0))
      return;

   if (documentDisplayMode[FrameTable[frame].FrDoc - 1] == NoComputedDisplay)
      return;

   FrameToView (frame, &doc, &view);
   FrameTable[frame].FrTopMargin = top_delta;
   FrameTable[frame].FrLeftMargin = 0;

   /* FrameTable[frame].FrWidth = (int) width - bottom_delta; */
   FrameTable[frame].FrWidth = (int) width - bottom_delta;
   FrameTable[frame].FrHeight = (int) height;

   /* need to recompute the content of the window */
   RebuildConcreteImage (frame);

   /* recompute the scroll bars */
    UpdateScrollbars (frame);
}
#endif /* _WINDOWS */

#ifndef _WINDOWS
/*----------------------------------------------------------------------
   Evenement sur une frame document.                              
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                FrameResized (int *w, int frame, int *info)
#else  /* __STDC__ */
void                FrameResized (w, frame, info)
int                *w;
int                 frame;
int                *info;

#endif /* __STDC__ */
{
   int                 n, dx, dy, view;
   Dimension           width, height;
   Arg                 args[MAX_ARGS];
   NotifyWindow        notifyDoc;
   Document            doc;

   n = 0;
   XtSetArg (args[n], XmNwidth, &width);
   n++;
   XtSetArg (args[n], XmNheight, &height);
   n++;
   XtGetValues ((ThotWidget) w, args, n);

   if ((width > 0) && (height > 0) && documentDisplayMode[FrameTable[frame].FrDoc - 1] != NoComputedDisplay)
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

	     /* Reevalue les ascenseurs */
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

#endif /* !_WINDOWS */

#ifdef _WINDOWS
/*----------------------------------------------------------------------
   Demande de scroll vertical.                                      
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void WIN_ChangeVScroll (int frame, int reason, int value)
#else  /* __STDC__ */
void WIN_ChangeVScroll (frame, reason, value)
int                 frame;
int                 reason;
int                 value;
#endif /* __STDC__ */
{
   int        delta, Xpos, Ypos, width, height;
   int        sPos, nbPages, remaining;

   /* do not redraw it if in NoComputedDisplay mode */
   if (documentDisplayMode[FrameTable[frame].FrDoc - 1] == NoComputedDisplay)
      return;

   switch (reason) {
          case SB_TOP:
               JumpIntoView (frame, 0);
               break;

          case SB_BOTTOM:
               JumpIntoView (frame, 100);
               break;

          case SB_LINEUP:
               delta = -13;
               VerticalScroll (frame, delta, TRUE);
               break;

          case SB_LINEDOWN:
               delta = 13;
               VerticalScroll (frame, delta, TRUE);
               break;

          case SB_PAGEUP:
               delta = -FrameTable[frame].FrHeight;
               VerticalScroll (frame, delta, TRUE);
               break;

          case SB_PAGEDOWN:
               delta = FrameTable[frame].FrHeight;
               VerticalScroll (frame, delta, TRUE);
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
			   if (nbPages <= 3) {
                  if (delta > 0)
                      delta = nbPages * FrameTable[frame].FrHeight + (int) ((remaining * FrameTable[frame].FrHeight) / height);
                  else 
                      delta = -(nbPages * FrameTable[frame].FrHeight + (int) ((remaining * FrameTable[frame].FrHeight) / height));
                  VerticalScroll (frame, delta, TRUE);
			   } else {
                     delta = (int) (((float)value / (float)FrameTable[frame].FrHeight) * 100) ;
                     JumpIntoView (frame, delta);
               }
               break;
   }
}

/*----------------------------------------------------------------------
   Demande de scroll vertical.                                      
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void WIN_ChangeHScroll (int frame, int reason, int value)
#else  /* __STDC__ */
void WIN_ChangeHScroll (frame, reason, value)
int                 frame;
int                 reason;
int                 value;
#endif /* __STDC__ */
{
   int      delta;

   /* do not redraw it if in NoComputedDisplay mode */
   if (documentDisplayMode[FrameTable[frame].FrDoc - 1] == NoComputedDisplay)
      return;

   switch (reason) {
   case SB_LINERIGHT:
        delta = 15;
        break;

   case SB_LINELEFT:
        delta = -15;
        break;
   }

   HorizontalScroll (frame, delta, TRUE) ;
   UpdateScrollbars (frame);
}
#endif /* _WINDOWS */

#ifndef _WINDOWS
/*----------------------------------------------------------------------
   Demande de scroll horizontal.                                    
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                FrameHScrolled (int *w, int frame, int *param)
#else  /* __STDC__ */
void                FrameHScrolled (w, frame, param)
int                *w;
int                 frame;
int                *param;

#endif /* __STDC__ */
{
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
		delta = FrameTable[frame].FrWidth;
	     else
		delta = info->value;
	     /* Cadre a la position demandee */
	     ShowXPosition (frame, delta, FrameTable[frame].FrWidth);
	  }
	else if (info->reason == XmCR_TO_TOP)
	   /* Cadre a gauche */
	   ShowXPosition (frame, 0, FrameTable[frame].FrWidth);
	else if (info->reason == XmCR_TO_BOTTOM)
	   /* Cadre a droite */
	   ShowXPosition (frame, FrameTable[frame].FrWidth, FrameTable[frame].FrWidth);
	else
	   HorizontalScroll (frame, delta, 1);

	notifyDoc.document = doc;
	notifyDoc.view = view;
	notifyDoc.verticalValue = 0;
	notifyDoc.horizontalValue = delta;
	CallEventType ((NotifyEvent *) & notifyDoc, FALSE);
     }
}				/*FrameHScrolled */

/*----------------------------------------------------------------------
   Demande de scroll vertical.                                      
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                FrameVScrolled (int *w, int frame, int *param)
#else  /* __STDC__ */
void                FrameVScrolled (w, frame, param)
int                *w;
int                 frame;
int                *param;

#endif /* __STDC__ */

{
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
   if (!CallEventType ((NotifyEvent *) & notifyDoc, TRUE)) {
      if (infos->reason == XmCR_VALUE_CHANGED || infos->reason == XmCR_DRAG) {
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

	     if (n == 0 || (y >= start && y <= total - end)) {
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
	     } else {
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
		      /* Mise a jour des bandes de scroll pour ajustement */
		      UpdateScrollbars (frame);
	     }
	  }
	else if (infos->reason == XmCR_TO_TOP)
	  {
	     /* Sauter au debut du document */
	     JumpIntoView (frame, 0);
	     /* Mise a jour des bandes de scroll pour ajustement */
	     UpdateScrollbars (frame);
	  }
	else if (infos->reason == XmCR_TO_BOTTOM)
	  {
	     /* Sauter a la fin du document */
	     JumpIntoView (frame, 100);
	     /* Mise a jour des bandes de scroll pour ajustement */
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
}				/*FrameVScrolled */

#endif /* !_WINDOWS */

/*----------------------------------------------------------------------
   PageUp scrolls one screen up.                                    
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtcPageUp (Document document, View view)
#else  /* __STDC__ */
void                TtcPageUp (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   int                 frame;
#  ifndef _WINDOWS
   XmScrollBarCallbackStruct infos;
#  else   /* _WINDOWS */
   int delta;
#  endif  /* _WINDOWS */

   if (document != 0)
      frame = GetWindowNumber (document, view);
   else
     frame = 0;
#  ifndef _WINDOWS
   infos.reason = XmCR_PAGE_DECREMENT;
   FrameVScrolled (0, frame, (int *) &infos);
#  else  /* _WINDOWS */
   delta = -FrameTable[frame].FrHeight;
   VerticalScroll (frame, delta, TRUE);
#  endif /* _WINDOWS */
}

/*----------------------------------------------------------------------
   PageDown scrolls one screen down.                                
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtcPageDown (Document document, View view)
#else  /* __STDC__ */
void                TtcPageDown (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   int                 frame;

#  ifndef _WINDOWS
   XmScrollBarCallbackStruct infos;
#  else   /* _WINDOWS */
   int delta;
#  endif  /* !_WINDOWS */

   if (document != 0)
      frame = GetWindowNumber (document, view);
   else
     frame = 0;
#  ifndef _WINDOWS
   infos.reason = XmCR_PAGE_INCREMENT;
   FrameVScrolled (0, frame, (int *) &infos);
#  else  /* _WINDOWS */
   delta = FrameTable[frame].FrHeight;
   VerticalScroll (frame, delta, TRUE);
#  endif /* _WINDOWS */
}


/*----------------------------------------------------------------------
   PageTop goes to the document top.                                
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtcPageTop (Document document, View view)
#else  /* __STDC__ */
void                TtcPageTop (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
#ifndef _WINDOWS
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
#ifdef __STDC__
void                TtcPageEnd (Document document, View view)
#else  /* __STDC__ */
void                TtcPageEnd (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
#ifndef _WINDOWS
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
void                InitializeOtherThings ()
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
#  ifndef _WINDOWS
   null_string = XmStringCreateSimple ("");
#  endif /* _WINDOWS */
   OldMsgSelect[0] = EOS;
   OldDocMsgSelect = NULL;
}


/*----------------------------------------------------------------------
   Map and raise the corresponding window.                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaRaiseView (Document document, View view)
#else  /* __STDC__ */
void                TtaRaiseView (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
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
}


/*----------------------------------------------------------------------
   TtaGetViewFrame retourne le widget du frame de la vue document.    
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotWidget          TtaGetViewFrame (Document document, View view)
#else  /* __STDC__ */
ThotWidget          TtaGetViewFrame (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   int                 frame;

   if (document == 0 && view == 0)
      return 0;
   else
      {
      frame = GetWindowNumber (document, view);
      if (frame == 0)
	 return 0;
      }
   /* Si les parametres sont invalides */
   if (frame > MAX_FRAME)
     {
	TtaError (ERR_invalid_parameter);
	return 0;
     }
   else
#     ifndef _WINDOWS
      return (FrameTable[frame].WdFrame);
#     else  /* _WINDOWS */
      return (FrMainRef[frame]);
#     endif /* _WINDOWS */
}

/*----------------------------------------------------------------------
   DisplaySelMessage affiche la se'lection donne'e en parame`tre (texte) dans 
   la fenetre active.                                            
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                DisplaySelMessage (char *text, PtrDocument pDoc)
#else  /* __STDC__ */
void                DisplaySelMessage (text, pDoc)
char               *text;
PtrDocument         pDoc;
#endif /* __STDC__ */
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
#ifdef __STDC__
void                TtaSetStatus (Document document, View view, CONST char *text, CONST char *name)
#else  /* __STDC__ */
void                TtaSetStatus (document, view, text, name)
Document            document;
View                view;
CONST char         *text;
CONST char         *name;
#endif /* __STDC__ */
{
   int                 frame;
   char                s[1024];

#  ifndef _WINDOWS
   Arg                 args[MAX_ARGS];
   XmString            title_string;
#  endif /* _WINDOWS */
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
	     if (name != NULL)
	       {
		  /* text est un format */
		  sprintf (s, text, name);
#                 ifndef _WINDOWS
		  title_string = XmStringCreateSimple (s);
#                 endif /* !_WINDOWS */
	       }
	     else
#               ifdef _WINDOWS
		strncpy (&s[0], text, sizeof (s));
#               else  /* !_WINDOWS */
		title_string = XmStringCreateSimple (text);
#               endif /* !_WINDOWS */

#            ifdef _WINDOWS
	     SendMessage (FrameTable[frame].WdStatus, SB_SETTEXT, (WPARAM) 0, (LPARAM) & s[0]);
		 SendMessage (FrameTable[frame].WdStatus, WM_PAINT, (WPARAM) 0, (LPARAM) 0);
#            else  /* !_WINDOWS */
	     XtSetArg (args[0], XmNlabelString, title_string);
	     XtSetValues (FrameTable[frame].WdStatus, args, 1);
	     XtManageChild (FrameTable[frame].WdStatus);
	     XmStringFree (title_string);
#            endif /* _WINDOWS */
	    }
     }
}


#ifdef _WINDOWS
#if 0
/*----------------------------------------------------------------------
  DrawBoxOutline :                                                    
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void DrawBoxOutline (HWND hwnd, POINT ptBeg, POINT ptEnd)
#else  /* __STDC__ */
static void DrawBoxOutline (hwnd, ptBeg, ptEnd)
HWND  hwnd; 
POINT ptBeg; 
POINT ptEnd;
#endif /* __STDC__ */
{
     HDC hdc ;

     hdc = GetDC (hwnd) ;

     SetROP2 (hdc, R2_NOT) ;
     SelectObject (hdc, GetStockObject (NULL_BRUSH)) ;
     /* SelectObject (hdc, GetStockObject (BLACK_PEN)) ; */
     Rectangle (hdc, ptBeg.x, ptBeg.y, ptEnd.x, ptEnd.y) ;

     DeleteDC (hwnd, hdc) ;
}
#endif /* 0 */

#ifndef _WIN_PRINT
/*----------------------------------------------------------------------
  WndProc :  The main MS-Windows event handler for the Thot Library.                                                    
  ----------------------------------------------------------------------*/
#ifdef __STDC__
LRESULT CALLBACK WndProc (HWND hwnd, UINT mMsg, WPARAM wParam, LPARAM lParam) 
#else  /* !__STDC__ */
LRESULT CALLBACK WndProc (hwnd, mMsg, wParam, lParam) 
HWND        hwnd; 
UINT        mMsg; 
WPARAM      wParam; 
LPARAM      lParam; 
#endif /* __STDC__ */
{
    HWND   hwndTextEdit;
#   ifdef  THOT_TOOLTIPS
    HWND   hwndToolTip ;
#   endif  /* THOT_TOOLTIPS */
    RECT   rect;
    int    doc, view ;
    char*  viewName ;
	static HPALETTE hPal;
	/*
	HDC    hDC, hMemDC;
	BITMAP bm;
	*/
    RECT   rWindow ;
    int  frame = GetMainFrameNumber (hwnd);
    GetWindowRect (hwnd, &rect);

    switch (mMsg) {
           case WM_CREATE:
                /* Create toolbar  */
                ThotTBBitmap.hInst = hInstance;
                ThotTBBitmap.nID   = IDR_TOOLBAR;
                ToolBar = CreateWindow (TOOLBARCLASSNAME, NULL, dwToolBarStyles,
                                        0, 0, 0, 0, hwnd, (HMENU) 1, hInstance, 0) ;

                SendMessage (ToolBar, TB_BUTTONSTRUCTSIZE, (WPARAM) sizeof (TBBUTTON), 0L);

                if ((SendMessage (ToolBar, TB_ADDBITMAP, (WPARAM) MAX_BUTTON, (LPARAM) (LPTBADDBITMAP) &ThotTBBitmap)) == -1)
                   WinErrorBox (NULL);

#               ifdef THOT_TOOLTIPS
                ToolBar_AddString (ToolBar, 0, &szTbStrings [0]);
                hwndToolTip = ToolBar_GetToolTips (ToolBar);
				 
                if (dwToolBarStyles & TBSTYLE_TOOLTIPS)
                   InitToolTip (ToolBar) ;	
#               endif /* THOT_TOOLTIPS */

                /* Create status bar  */
                StatusBar = CreateStatusWindow (dwStatusBarStyles, "", hwnd, 2) ;
                ShowWindow (StatusBar, SW_SHOWNORMAL);
                UpdateWindow (StatusBar);

                /* Create client window */
                hwndClient = CreateWindowEx (WS_EX_CLIENTEDGE, "ClientWndProc", NULL,
                                             WS_CHILD | WS_VISIBLE | WS_BORDER, 0, 0, 0, 0,
                                             hwnd, (HMENU) 2, hInstance, NULL) ;
                ShowWindow (hwndClient, SW_SHOWNORMAL);
                UpdateWindow (hwndClient);

                return 0 ;

           case WM_PALETTECHANGED: 
			   if ((HWND) wParam != hwnd) {
                  HDC hDC = GetDC (hwnd);
                  SelectPalette (hDC, hPal, 0);
                  if (RealizePalette (hDC))
                     UpdateColors (hDC);
                  ReleaseDC (hwnd, hDC);
               }
               break;

           case WM_VSCROLL:
                WIN_ChangeVScroll (frame, LOWORD (wParam), HIWORD (wParam));
                return 0;

           case WM_HSCROLL:
                WIN_ChangeHScroll (frame, LOWORD (wParam), HIWORD (wParam));
                return 0;

           case WM_ENTER:
                hwndTextEdit = GetFocus ();
                WIN_APP_TextCallback (hwndTextEdit, frame);
				SetFocus (FrRef [frame]);
                return 0;

           case WM_KEYDOWN:
                SendMessage (FrRef [frame], WM_KEYDOWN, wParam, lParam);
                return 0;

           case WM_CHAR:
                SendMessage (FrRef [frame], WM_CHAR, wParam, lParam);
                return 0;

#          ifdef THOT_TOOLTIPS		 
           case WM_NOTIFY: {
                LPNMHDR pnmh = (LPNMHDR) lParam ;
                int idCtrl = (int) wParam ;

                /*Toolbar notifications */
                if ((pnmh->code >= TBN_LAST) && (pnmh->code <= TBN_FIRST))
                   return ToolBarNotify (frame, hwnd, wParam, lParam) ;
		 
                /* Fetch tooltip text */
                if (pnmh->code == TTN_NEEDTEXT) {
                   LPTOOLTIPTEXT lpttt = (LPTOOLTIPTEXT) lParam ;
                   CopyToolTipText (lpttt) ;
                }
                return 0 ;
           }
#          endif /* THOT_TOOLTIPS */

           case WM_COMMAND:
                if (LOWORD (wParam) >= TBBUTTONS_BASE)
                   APP_ButtonCallback (FrameTable[frame].Button[LOWORD (wParam) - TBBUTTONS_BASE], frame, "\n");
                else 
                    WIN_ThotCallBack (hwnd, wParam, lParam);
                break;

           case WM_DESTROY:
                if (!viewClosed) {
                   FrameToView (frame, &doc, &view);
                   viewName = TtaGetViewName (doc, view);
                   if (!strcmp (viewName, "Formatted_view"))
                      TtcCloseDocument (doc, view);
                   else
                       TtcCloseView (doc, view);
                }

                viewClosed = FALSE;
                PostQuitMessage (0);
                break ;

           case WM_SIZE: {
                int    cx = LOWORD (lParam) ;
                int    cy = HIWORD (lParam) ;
                int    cyStatus ;
                int    cxVSB ;
                int    cyHSB ;
                int    cyTB ;
                int    x, y ;
                int    index = 0;
                int    cyTxtZone ;
                DWORD  dwStyle ;

#               if 0
				if (appLogo) 
                   GetObject (appLogo, sizeof (BITMAP), &bm);
#               endif /* 0 */

                /* Adjust toolbar size. */
                if (IsWindowVisible (WinToolBar[frame])) {
                   dwStyle = GetWindowLong (WinToolBar[frame], GWL_STYLE) ;

                   if (dwStyle & CCS_NORESIZE)
                      MoveWindow (WinToolBar[frame], 0, 0, cx, cyToolBar, FALSE) ;
                   else
                       ToolBar_AutoSize (WinToolBar[frame]) ;

                   InvalidateRect (WinToolBar[frame], NULL, TRUE) ;
                   GetWindowRect (WinToolBar[frame], &rWindow) ;
                   ScreenToClient (hwnd, (LPPOINT) &rWindow.left) ;
                   ScreenToClient (hwnd, (LPPOINT) &rWindow.right) ;
                   cyTB = rWindow.bottom - rWindow.top ;
                } else 
                      cyTB = 0 ;

                cyTxtZone = cyTB ;
				/* cyLogo    = cyTB ; */

                /* Adjust text zones */
                for (index = 0; index < MAX_TEXTZONE; index++) {
                    if (FrameTable[frame].Text_Zone[index] && IsWindowVisible (FrameTable[frame].Text_Zone[index])) {
                       MoveWindow (FrameTable[frame].Label[index], 15, cyTxtZone + 5, 70, 20, TRUE);
                       MoveWindow (FrameTable[frame].Text_Zone[index], 85, cyTxtZone + 5, cx - 100, 20, TRUE) ;
                       cyTxtZone += 25 ;
                    }
                }
/*
                if (appLogo) {
				   hDC = GetDC (FrMainRef [frame]);
				   hMemDC = CreateCompatibleDC (hDC);
				   SelectObject (hMemDC, appLogo);
				   BitBlt (hDC, 1, cyTB, bm.bmWidth, bm.bmHeight, hMemDC, 0, 0, SRCCOPY);
				   DeleteDC (hMemDC);
				   DeleteDC (hDC);
				}*/

                /* Adjust status bar size. */
                if (IsWindowVisible (FrameTable[frame].WdStatus)) {
                   GetWindowRect (FrameTable[frame].WdStatus, &rWindow) ;
                   cyStatus = rWindow.bottom - rWindow.top ;
                   MoveWindow (FrameTable[frame].WdStatus, 0, cy - cyStatus, cx, cyStatus, TRUE) ;
                } else
                      cyStatus = 0 ;

                /* Adjust Vertical scroll bar */
                MoveWindow (FrameTable[frame].WdScrollV, cx - 15, cyTxtZone, 15, cy - (cyStatus + cyTxtZone + 15), TRUE) ;
                
                /* Adjust Hoizontal scroll bar */
                MoveWindow (FrameTable[frame].WdScrollH, 0, cy - (cyStatus + 15), cx - 15, 15, TRUE) ;

                /* Adjust client window size. */
                GetWindowRect (FrameTable[frame].WdScrollV, &rWindow) ;
                cxVSB = rWindow.right - rWindow.left ;

                GetWindowRect (FrameTable[frame].WdScrollH, &rWindow) ;
                cyHSB = rWindow.bottom - rWindow.top ;

                x = 0 ;
                y = cyTxtZone ;
                cx = cx - cxVSB ;
                cy = cy - (cyStatus + cyTxtZone + cyHSB) ;
                MoveWindow (FrRef [frame], x, y, cx, cy, TRUE) ;

                SetScrollRange (FrameTable[frame].WdScrollV, SB_CTL, 0, cy, TRUE);
                SetScrollRange (FrameTable[frame].WdScrollH, SB_CTL, 0, cx, TRUE);

                return 0;
           }

           default: 
#                  if 0
                   GetWindowRect (WinToolBar[frame], &rWindow) ;
                   ScreenToClient (hwnd, (LPPOINT) &rWindow.left) ;
                   ScreenToClient (hwnd, (LPPOINT) &rWindow.right) ;
                   if ((frame != -1) && appLogo && FrameTable[frame].showLogo) {
                      hDC = GetDC (FrMainRef [frame]);
				      hMemDC = CreateCompatibleDC (hDC);
				      SelectObject (hMemDC, appLogo);
				      BitBlt (hDC, 1, cyLogo, bm.bmWidth, bm.bmHeight, hMemDC, 0, 0, SRCCOPY);
				      DeleteDC (hMemDC);
				      DeleteDC (hDC);
				   }
#                  endif /* 0 */
			       return (DefWindowProc (hwnd, mMsg, wParam, lParam)) ;
     }
}

/* -------------------------------------------------------------------
   ClientWndProc
   ------------------------------------------------------------------- */
#ifdef __STDC__
LRESULT CALLBACK ClientWndProc (HWND hwnd, UINT mMsg, WPARAM wParam, LPARAM lParam)
#else  /* !__STDC__ */
LRESULT CALLBACK ClientWndProc (hwnd, mMsg, wParam, lParam)
HWND   hwnd; 
UINT   mMsg; 
WPARAM wParam; 
LPARAM lParam;
#endif /* __STDC__ */
{
     HDC          saveHdc;	/* Used to save TtDisplay during current event processing */
     int          frame;
     int          status;
	 RECT         rect;

	 static POINT ptBegin;
	 static POINT ptEnd;
	 static BOOL  fBlocking;

     frame = GetFrameNumber (hwnd);
	 GetWindowRect (hwnd, &rect);

     /* do not handle events if the Document is in NoComputedDisplay mode. */

     if (frame != -1) {
        if (documentDisplayMode[FrameTable[frame].FrDoc - 1] == NoComputedDisplay)
           return (DefWindowProc (hwnd, mMsg, wParam, lParam));

        /*
         * If are waiting for the user to explicitely point to a document,
         * store the location and return.
         */
		
        if (ClickIsDone == 1 && ((mMsg == WM_LBUTTONDOWN) || (mMsg == WM_RBUTTONDOWN))) {
           ClickIsDone = 0;
           currentWindow = hwnd;
           ClickFrame = frame;
           ClickX = LOWORD (lParam);
           ClickY = HIWORD (lParam);
           return (DefWindowProc (hwnd, mMsg, wParam, lParam));
        }
		
     }

     switch (mMsg) {
            case WM_PAINT: 
	             /* Some part of the Client Area has to be repaint. */
                 saveHdc = TtDisplay;
                 WIN_HandleExpose (hwnd, frame, wParam, lParam);
                 TtDisplay = saveHdc;
                 return 0;

            case WM_SIZE: {
                 HWND hwndNotify = GetWindow (hwnd, GW_CHILD) ;
                 int  cx         = LOWORD (lParam) ;
                 int  cy         = HIWORD (lParam) ;

                 WIN_ChangeViewSize (frame, cx, cy, 0, 0) ;
                 WIN_ReleaseDeviceContext ();

                 return 0 ;
            }

            case WM_KEYDOWN:
            case WM_CHAR:
                 TtaAbortShowDialogue ();
                 WIN_CharTranslation (FrRef[frame], frame, mMsg, wParam, lParam);
                 return 0;

            case WM_LBUTTONDOWN:
                 /* Activate the client window */
                 SetFocus (FrRef[frame]);
                 /* stop any current insertion of text */
                 CloseInsertion ();

                 status = GetKeyState (VK_CONTROL);
                 if (HIBYTE (status)) {
                    /* changes the box position */
                    ApplyDirectTranslate (frame, LOWORD (lParam), HIWORD (lParam));
                    /* This is the beginning of a selection */
                 } else {
                      ClickFrame = frame;
                      oldXPos = ClickX = LOWORD (lParam);
                      oldYPos = ClickY = HIWORD (lParam);
                      LocateSelectionInView (frame, ClickX, ClickY, 2);
                 }
                 fBlocking = TRUE;
                 return 0;

            case WM_LBUTTONUP:
                 if (fBlocking)
                    fBlocking = FALSE;
                 return 0;

            case WM_LBUTTONDBLCLK:/* left double click handling */
				 ReturnOption = -1;
                 TtaAbortShowDialogue ();
	  
                 /* memorise la position de la souris */
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
                 if (HIBYTE (status)) {
                    /* changes the box size */
                    ApplyDirectResize (frame, LOWORD (lParam), HIWORD (lParam));
                    /* memorize the click position */
                 } else {
                      TtaAbortShowDialogue ();
                      LocateSelectionInView (frame, LOWORD (lParam), HIWORD (lParam), 0);
                 }
                 return 0;

            case WM_RBUTTONDOWN:
                 ClickFrame = frame;
                 ClickX = LOWORD (lParam);
                 ClickY = HIWORD (lParam);
				 return 0;

            case WM_MOUSEMOVE:
                 X_Pos = LOWORD (lParam) ;
                 Y_Pos = HIWORD (lParam) ;
                 if (fBlocking) {
                    if (((oldXPos <= X_Pos - 1) || (oldXPos >= X_Pos + 1)) && 
                        ((oldYPos <= Y_Pos - 1) || (oldYPos >= Y_Pos + 1)))	  
                       LocateSelectionInView (frame, X_Pos, Y_Pos, 0);
                 }
				 oldXPos = X_Pos;
				 oldYPos = Y_Pos;
                 return 0;

            case WM_DESTROY: 
                 WIN_ReleaseDeviceContext ();
                 PostQuitMessage (0);
                 return 0;
	       
          default:
               return (DefWindowProc (hwnd, mMsg, wParam, lParam)) ;
     }
}
#endif /* !_WIN_PRINT */
#endif /* _WINDOWS */

#ifndef _WINDOWS
/*----------------------------------------------------------------------
   Evenement sur une frame document.                              
   D.V. equivalent de la fontion MS-Windows ci dessus !           
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                FrameCallback (int frame, void *evnt)

#else  /* __STDC__ */
void                FrameCallback (frame, evnt)
int                 frame;
void               *event;

#endif /* __STDC__ */

{
   PtrDocument         docsel;
   PtrElement          firstSel, lastSel;
   ThotEvent           event;
   ThotEvent          *ev = (ThotEvent *) evnt;
   int                 firstCar, lastCar;
   int                 comm, dx, dy, sel;
   boolean             ok;

   /* ne pas traiter si le document est en mode NoComputedDisplay */
   if (documentDisplayMode[FrameTable[frame].FrDoc - 1] == NoComputedDisplay)
      return;
   /*_______> S'il n'y a pas d'evenement associe */
   else if (ev == NULL)
      return;
   /*_______> Si une designation de pave est attendue*/
   else if (ClickIsDone == 1 && ev->type == ButtonPress)
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
       switch (ev->xbutton.button)
	 {
	   /* ==========BOUTON GAUCHE========== */
	 case Button1:
	   /* Termine l'insertion courante s'il y en a une */
	   CloseInsertion ();
	   
	   /* Est-ce que la touche modifieur de geometrie est active ? */
	   if ((ev->xbutton.state & THOT_KEY_ControlMask) != 0)
	     {
	       /* On change la position d'une boite */
	       ApplyDirectTranslate (frame, ev->xbutton.x, ev->xbutton.y);
	     }
	   /* Est-ce un double clic */
	   else if (t1 + (Time) 500 > ev->xbutton.time)
	     {
	       TtaAbortShowDialogue ();
	       TtaFetchOneEvent (&event);
	       while (event.type != ButtonRelease)
		 {
		   TtaHandleOneEvent (&event);
		   TtaFetchOneEvent (&event);
		 }

	       /* memorise la position de la souris */
	       if (ClickFrame == frame
		   && (ClickX - ev->xbutton.x < 3 || ClickX - ev->xbutton.x > 3)
		   && (ClickY - ev->xbutton.y < 3 || ClickY - ev->xbutton.y > 3))
		 /* it's really a double click */
		 sel = 3;
	       else
		 sel = 2;
	       ClickFrame = frame;
	       ClickX = ev->xbutton.x;
	       ClickY = ev->xbutton.y;
	       LocateSelectionInView (frame, ClickX, ClickY, sel);
	     }
	   /* Sinon c'est une selection normale */
	   else
	     {
	       t1 = ev->xbutton.time;
	       ClickFrame = frame;
	       ClickX = ev->xbutton.x;
	       ClickY = ev->xbutton.y;
	       LocateSelectionInView (frame, ClickX, ClickY, 2);
	       
	       /* Regarde s'il s'agit d'un drag ou d'une simple marque d'insertion */
	       comm = 0;	/* il n'y a pas de drag */
	       TtaFetchOneEvent (&event);
	       while (event.type != ButtonRelease)
		 {
		   if (event.type == MotionNotify)
		     {
		       dx = event.xmotion.x - ClickX;
		       dy = event.xmotion.y - ClickY;
		       if (dx > 2 || dx < -2 || dy > 2 || dy < -2)
			 {
			   LocateSelectionInView (frame, event.xbutton.x, event.xbutton.y, 1);
			   comm = 1;	/* il y a un drag */
			 }
		     }
		   TtaHandleOneEvent (&event);
		   TtaFetchOneEvent (&event);
		 }	/*while */
	       TtaHandleOneEvent (&event);
	       
	       /* S'il y a un drag on termine la selection */
	       if (comm == 1)
		 LocateSelectionInView (frame, event.xbutton.x, event.xbutton.y, 0);
	     }
	   break;
	   
	   /* ==========BOUTON MILIEU========== */
	 case Button2:
	   /* Termine l'insertion courante s'il y en a une */
	   CloseInsertion ();
	   
	   /* Est-ce que la touche modifieur de geometrie est active ? */
	   if ((ev->xbutton.state & THOT_KEY_ControlMask) != 0)
	     {
	       /* On modifie les dimensions d'une boite */
	       ApplyDirectResize (frame, ev->xbutton.x, ev->xbutton.y);
	     }
	   else
	     {
	       TtaAbortShowDialogue ();
	       LocateSelectionInView (frame, ev->xbutton.x, ev->xbutton.y, 0);
	     }
	   break;
	   
	   /* ==========BOUTON DROIT========== */
	 case Button3:
	   /* Termine l'insertion courante s'il y en a une */
	   CloseInsertion ();
	   if ((ev->xbutton.state & THOT_KEY_ControlMask) != 0)
	     {
	       /* On modifie les dimensions d'une boite */
	       ApplyDirectResize (frame, ev->xbutton.x, ev->xbutton.y);
	     }
	   else if (!GetCurrentSelection (&docsel, &firstSel, &lastSel, &firstCar, &lastCar))
	     /* non, message 'Selectionnez' */
	     TtaDisplaySimpleMessage (INFO, LIB, TMSG_SEL_EL);
	   else if (docsel->DocReadOnly)
	     /* on ne peut inserer ou coller dans un document en lecture seule */
	     TtaDisplaySimpleMessage (INFO, LIB, TMSG_RO_DOC_FORBIDDEN);
	   else if (firstCar != 0 && firstSel->ElTerminal && firstSel->ElLeafType == LtPolyLine)
	     {
	       /* selection a l'interieur d'une polyline */
	       if (ThotLocalActions[T_editfunc] != NULL)
		 (*ThotLocalActions[T_editfunc]) (TEXT_INSERT);
	     }
	   else
	     {
	       TtaSetDialoguePosition ();
	       if (ThotLocalActions[T_insertpaste] != NULL)
		 (*ThotLocalActions[T_insertpaste]) (TRUE, FALSE, 'R', &ok);
	     }
	   
	 default:
	   break;
	 }
       break;

     case KeyPress:
       t1 = 0;
       TtaAbortShowDialogue ();
       XCharTranslation (ev);
       break;

     case EnterNotify:
       t1 = 0;
       break;

     case LeaveNotify:
       t1 = 0;
       break;

     default:
       break;
     }
}
#endif /* _WINDOWS */


/*----------------------------------------------------------------------
   ThotGrab fait un XGrabPointer.                                  
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ThotGrab (ThotWindow win, ThotCursor cursor, long events, int disp)
#else  /* __STDC__ */
void                ThotGrab (win, cursor, events, disp)
ThotWindow          win;
ThotCursor          cursor;
long                events;
int                 disp;

#endif /* __STDC__ */
{
#ifndef _WINDOWS
   XGrabPointer (TtDisplay, win, FALSE, events, GrabModeAsync, GrabModeAsync,
		 win, cursor, CurrentTime);
#endif /* _WINDOWS */
}


/*----------------------------------------------------------------------
   ThotUngrab est une fonction d'interface pour UnGrab.            
  ----------------------------------------------------------------------*/
void                ThotUngrab ()
{
#ifndef _WINDOWS
   XUngrabPointer (TtDisplay, CurrentTime);
#endif /* _WINDOWS */
}

#ifndef _WIN_PRINT
/*----------------------------------------------------------------------
   TtaGetThotWindow recupere le numero de la fenetre.           
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotWindow          TtaGetThotWindow (int frame)
#else  /* __STDC__ */
ThotWindow          TtaGetThotWindow (frame)
int                 frame;

#endif /* __STDC__ */
{
   return FrRef[frame];
}

#ifdef _WINDOWS
#ifdef __STDC__
ThotWindow TtaGetThotWinMainWindow (int frame)
#else  /* __STDC__ */
ThotWindow TtaGetThotWinMainWindow (frame)
int                 frame;

#endif /* __STDC__ */
{
   return FrMainRef[frame];
}
#endif /* _WINDOWS */

/*----------------------------------------------------------------------
   SetCursorWatch affiche le curseur "montre".                  
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                SetCursorWatch (int thotThotWindowid)
#else  /* __STDC__ */
void                SetCursorWatch (thotThotWindowid)
int                 thotThotWindowid;

#endif /* __STDC__ */
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
#ifdef __STDC__
void                ResetCursorWatch (int thotThotWindowid)
#else  /* __STDC__ */
void                ResetCursorWatch (thotThotWindowid)
int                 thotThotWindowid;

#endif /* __STDC__ */
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
#endif /* _WIN_PRINT */

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaSetCursorWatch (Document document, View view)

#else  /* __STDC__ */
void                TtaSetCursorWatch (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
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
#ifdef __STDC__
void                TtaResetCursor (Document document, View view)

#else  /* __STDC__ */
void                TtaResetCursor (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
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
#ifdef __STDC__
void                GiveClickedAbsBox (int *frame, PtrAbstractBox *pave)

#else  /* __STDC__ */
void                GiveClickedAbsBox (frame, pave)
int                *frame;
PtrAbstractBox     *pave;

#endif /* __STDC__ */

{
#ifndef _WINDOWS
   ThotEvent           event;
   Drawable            drawable;
#else  /* _WINDOWS */
   MSG                 event;
   HCURSOR             cursor;          
#endif /* _WINDOWS */
   int                 i;

   if (ClickIsDone == 1)
     {
       *frame = 0;
       *pave = NULL;
     }

   /* Changement du curseur */
#  ifdef _WINDOWS
#  ifndef _WIN_PRINT 
   cursor = LoadCursor (hInstance, MAKEINTRESOURCE (Window_Curs));
#  endif /* _WIN_PRINT */
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
#        ifndef _WINDOWS 
         TtaFetchOneEvent (&event);
         TtaHandleOneEvent (&event);
#        else /* _WINDOWS */
         GetMessage (&event, NULL, 0, 0);
         TtaHandleOneWindowEvent (&event);
         SetCursor (cursor);
#        endif /* !_WINDOWS */
     }

   /* Restauration du curseur */
   for (i = 1; i <= MAX_FRAME; i++)
     {
#      ifndef _WINDOWS
       drawable = TtaGetThotWindow (i);
       if (drawable != 0)
	 XUndefineCursor (TtDisplay, drawable);
#      endif /* _WINDOWS */
     }

   *frame = ClickFrame;
   if (ClickFrame > 0 && ClickFrame <= MAX_FRAME)
     *pave = GetClickedAbsBox (ClickFrame, ClickX, ClickY);
   else
     *pave = NULL;
}

#ifndef _WIN_PRINT
/*----------------------------------------------------------------------
   Modifie le titre de la fenetre d'indice frame.                     
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ChangeFrameTitle (int frame, char *text)

#else  /* __STDC__ */
void                ChangeFrameTitle (frame, text)
int                 frame;
char               *text;

#endif /* __STDC__ */
{
#ifndef _WINDOWS
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
#endif /* _WINDOWS */
}				/*ChangeFrameTitle */
#endif /* _WIN_PRINT */

/*----------------------------------------------------------------------
   La frame d'indice frame devient la fenetre active.               
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ChangeSelFrame (int frame)
#else  /* __STDC__ */
void                ChangeSelFrame (frame)
int                 frame;

#endif /* __STDC__ */
{
   ThotWidget          w;

   if (ActiveFrame != frame)
     {
	ActiveFrame = frame;
	if (frame != 0)
	  {
	     w = FrameTable[frame].WdFrame;
#ifndef _WINDOWS
	     if (w != 0)
		XMapRaised (TtDisplay, XtWindowOfObject (XtParent (XtParent (XtParent (w)))));
#endif /* _WINDOWS */
	  }
     }
}				/*ChangeSelFrame */


/*----------------------------------------------------------------------
   GetWindowFrame retourne l'indice de la table des Cadres associe'    
   a` la fenetre w.                                        
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                 GetWindowFrame (ThotWindow w)
#else  /* __STDC__ */
int                 GetWindowFrame (w)
ThotWindow          w;

#endif /* __STDC__ */
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
#ifdef __STDC__
int GetWindowWinMainFrame (ThotWindow w)
#else  /* __STDC__ */
int GetWindowWinMainFrame (w)
ThotWindow w;
#endif /* __STDC__ */
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

#ifndef _WIN_PRINT
/*----------------------------------------------------------------------
   GetSizesFrame retourne les dimensions de la fenetre d'indice frame.        
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                GetSizesFrame (int frame, int *width, int *height)

#else  /* __STDC__ */
void                GetSizesFrame (frame, width, height)
int                 frame;
int                *width;
int                *height;

#endif /* __STDC__ */

{
#  ifndef _WINDOWS
   *width = FrameTable[frame].FrWidth;
   *height = FrameTable[frame].FrHeight;
#  else  /* _WINDOWS */
   RECT rWindow ;

   GetClientRect (FrRef[frame], &rWindow) ;
   *height = rWindow.bottom - rWindow.top ;
   *width  = rWindow.right - rWindow.left ;
#  endif /* _WINDOWS */
}

/*----------------------------------------------------------------------
   DefineClipping limite la zone de reaffichage sur la fenetre frame et   
   recalcule ses limites sur l'image concrete.             
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                DefineClipping (int frame, int orgx, int orgy, int *xd, int *yd, int *xf, int *yf, int raz)

#else  /* __STDC__ */
void                DefineClipping (frame, orgx, orgy, xd, yd, xf, yf, raz)
int                 frame;
int                 orgx;
int                 orgy;
int                *xd;
int                *yd;
int                *xf;
int                *yf;
int                 raz;

#endif /* __STDC__ */
{
   int              clipx, clipy, clipwidth, clipheight;
#  ifdef _WINDOWS
#  else  /* !_WINDOWS */
   XRectangle        rect;
#  endif /* _WINDOWS */

   if (*xd < *xf && *yd < *yf && orgx < *xf && orgy < *yf) {
	/* On calcule le rectangle de clipping su la fenetre */
	clipx = *xd - orgx;
	if (clipx < 0) {
	   *xd -= clipx;
	   clipx = 0;
	}

	clipy = *yd - orgy;
	if (clipy < 0) {
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
#   ifndef _WINDOWS
	rect.x = 0;
	rect.y = 0;
	rect.width = clipwidth;
	rect.height = clipheight;
	XSetClipRectangles (TtDisplay, TtLineGC, clipx + FrameTable[frame].FrLeftMargin,
		 clipy + FrameTable[frame].FrTopMargin, &rect, 1, Unsorted);
	XSetClipRectangles (TtDisplay, TtGreyGC, clipx + FrameTable[frame].FrLeftMargin,
		 clipy + FrameTable[frame].FrTopMargin, &rect, 1, Unsorted);
	XSetClipRectangles (TtDisplay, TtGraphicGC, clipx + FrameTable[frame].FrLeftMargin,
		 clipy + FrameTable[frame].FrTopMargin, &rect, 1, Unsorted);
#   else  /* _WINDOWS */ 
    clipRgn = CreateRectRgn (clipx + FrameTable[frame].FrLeftMargin, clipy + FrameTable[frame].FrTopMargin, 
                             clipx + FrameTable[frame].FrLeftMargin + clipwidth, clipy + FrameTable[frame].FrTopMargin + clipheight);
    SelectClipRgn(TtDisplay, clipRgn); 
#   endif /* _WINDOWS */
	if (raz > 0)
#      ifdef _WINDOWS 
	   Clear (frame, clipwidth, clipheight, clipx + FrameTable[frame].FrLeftMargin, clipy + FrameTable[frame].FrTopMargin);
#      else  /* _WINDOWS */
	   Clear (frame, clipwidth, clipheight, clipx, clipy);
#      endif /* _WINDOWS */
     }
}

/*----------------------------------------------------------------------
   RemoveClipping annule le rectangle de clipping de la fenetre frame.  
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                RemoveClipping (int frame)
#else  /* __STDC__ */
void                RemoveClipping (frame)
int                 frame;

#endif /* __STDC__ */
{
#  ifndef _WINDOWS
   XRectangle          rect;

   rect.x = 0;
   rect.y = 0;
   rect.width = MAX_SIZE;
   rect.height = MAX_SIZE;
   XSetClipRectangles (TtDisplay, TtLineGC, 0, 0, &rect, 1, Unsorted);
   XSetClipRectangles (TtDisplay, TtGraphicGC, 0, 0, &rect, 1, Unsorted); 
   XSetClipRectangles (TtDisplay, TtGreyGC, 0, 0, &rect, 1, Unsorted);
   XFlushOutput (frame);
#  else  /* _WINDOWS */
   WIN_GetDeviceContext (frame);
   SelectClipRgn(TtDisplay, NULL); 
   if (clipRgn && !DeleteObject (clipRgn))
      WinErrorBox (NULL);
   clipRgn = (HRGN) 0;
   WIN_ReleaseDeviceContext ();
#  endif /* _WINDOWS */
}

/*----------------------------------------------------------------------
   UpdateScrollbars met a jour les bandes de defilement de la fenetretre    
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                UpdateScrollbars (int frame)

#else  /* __STDC__ */
void                UpdateScrollbars (frame)
int                 frame;

#endif /* __STDC__ */

{
   int                 Xpos, Ypos;
   int                 width, height;
   int                 l, h;
   ThotWidget          hscroll, vscroll;

#  ifndef _WINDOWS
   Arg                 args[MAX_ARGS];
   int                 n;
#  endif /* _WINDOWS */

#  ifdef _WINDOWS
   RECT       rWindow ;
   SCROLLINFO scrollInfo;
#  endif /* _WINDOWS */

   /* Demande le volume affiche dans la fenetre */
   ComputeDisplayedChars (frame, &Xpos, &Ypos, &width, &height);
   hscroll = FrameTable[frame].WdScrollH;
   vscroll = FrameTable[frame].WdScrollV;

#  ifndef _WINDOWS
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
#  else  /* _WINDOWS */
   GetWindowRect (FrRef[frame], &rWindow) ;
   h = rWindow.bottom - rWindow.top ;
   l = rWindow.right - rWindow.left ;
   scrollInfo.cbSize = sizeof (SCROLLINFO);
   scrollInfo.fMask  = SIF_PAGE | SIF_POS | SIF_RANGE ;
   scrollInfo.nMin   = 0;

   if (width + Xpos <= l) {
      scrollInfo.nMax   = l;
      scrollInfo.nPage  = width ;
      scrollInfo.nPos   = Xpos ;
      SetScrollInfo (FrameTable[frame].WdScrollH, SB_CTL, &scrollInfo, TRUE);
   }

   if (height + Ypos <= h) {
      scrollInfo.nMax   = h;
      scrollInfo.nPage  = height ;
      scrollInfo.nPos   = Ypos ;
      SetScrollInfo (FrameTable[frame].WdScrollV, SB_CTL, &scrollInfo, TRUE);
   }
#  endif /* _WINDOWS */
}				/*UpdateScrollbars */
#endif /* _WIN_PRINT */

/* End Of Module Thot */
