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
 * Colors Keyboard routines
 *
 * Author: I. Vatton (INRIA)
 *         R. Guetari (W3C/INRIA) - Windows NT/95 routines
 *
 */

#include "thot_gui.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "constmenu.h"
#include "appdialogue.h"
#include "message.h"
#include "dialog.h"
#include "thotcolor.h"
#ifdef _WINDOWS
#include "winsys.h"
#endif /* _WINDOWS */


#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "edit_tv.h"
#include "frame_tv.h"
#include "font_tv.h"
#include "select_tv.h"
#include "appdialogue_tv.h"
#include "thotcolor_tv.h"

#define COLORS_COL 8
#define MAX_ARGS 20

#ifdef _WINDOWS
#      define HORIZ_DIV  8
#      define VERT_DIV  19
#      define Button1 1		
#      ifdef _IDDONE_
#            undef _IDDONE_
#      endif /* _IDDONE_ */
#      define _IDDONE_ 100

static BOOL   wndRegistered = FALSE ;

extern LPCTSTR iconID;

#ifdef __STDC__
LRESULT CALLBACK ThotColorPaletteWndProc (HWND, UINT, WPARAM, LPARAM);
#else  /* __STDC__ */
LRESULT CALLBACK ThotColorPaletteWndProc ();
#endif /* __STDC__ */
#endif /* _WINDOWS */

static ThotWindow   Color_Window = 0;
static ThotWidget   Color_Palette;
static int          LastBg;
static int          LastFg;
static ThotGC       GCkey;

#include "config_f.h"
#include "appdialogue_f.h"
#include "actions_f.h"
#include "windowdisplay_f.h"
#include "font_f.h"
#include "changepresent_f.h"
#include "structselect_f.h"
#include "inites_f.h"

/*----------------------------------------------------------------------
   ThotSelectPalette
   Shows the current selection.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         ThotSelectPalette (int bground, int fground)
#else  /* __STDC__ */
static void         ThotSelectPalette (bground, fground)
int                 bground;
int                 fground;

#endif /* __STDC__ */
{
   int                 x, y;
   int                 wcase, hcase;
#  ifndef _WINDOWS 
   int                 w, h;
#  endif /* !_WINDOWS */

   if (TtWDepth == 1)
      /* Affiche le nom des couleurs sur un ecran N&B */
      wcase = CharacterWidth ('m', FontDialogue) * 12;
   else
      /* Affiche les couleurs sur un ecran couleur */
      wcase = CharacterWidth ('m', FontDialogue) * 4;

   hcase = FontHeight (FontDialogue);
   if (LastBg != -1 && LastBg != bground)
     {
	/* eteint le background precedent */
	x = (LastBg % COLORS_COL) * wcase;
	y = ((LastBg / COLORS_COL) + 1) * hcase;
#ifndef _WINDOWS
	x -= 2;
	y -= 2;
	w = wcase + 2;
	h = hcase + 2;
	XFillRectangle (TtDisplay, Color_Window, TtInvertGC, x + 2, y, w, 2);
	XFillRectangle (TtDisplay, Color_Window, TtInvertGC, x, y, 2, h);
	XFillRectangle (TtDisplay, Color_Window, TtInvertGC, x, y + h, w, 2);
	XFillRectangle (TtDisplay, Color_Window, TtInvertGC, x + w, y + 2, 2, h);
#endif /* _WINDOWS */
     }

   if (LastFg != -1 && LastFg != fground)
     {
	/* eteint le foreground precedent */
	x = (LastFg % COLORS_COL) * wcase;
	y = ((LastFg / COLORS_COL) + 1) * hcase;
#ifndef _WINDOWS
	w = wcase - 2;
	h = hcase - 2;
	XFillRectangle (TtDisplay, Color_Window, TtInvertGC, x + 2, y, w, 2);
	XFillRectangle (TtDisplay, Color_Window, TtInvertGC, x, y, 2, h);
	XFillRectangle (TtDisplay, Color_Window, TtInvertGC, x, y + h, w, 2);
	XFillRectangle (TtDisplay, Color_Window, TtInvertGC, x + w, y + 2, 2, h);
#endif /* _WINDOWS */
     }

   /* nouveau background */
   if (LastBg != bground)
     {
	LastBg = bground;
	if (LastBg != -1)
	  {
	     x = (LastBg % COLORS_COL) * wcase;
	     y = ((LastBg / COLORS_COL) + 1) * hcase;
#ifndef _WINDOWS
	     x -= 2;
	     y -= 2;
	     w = wcase + 2;
	     h = hcase + 2;
	     XFillRectangle (TtDisplay, Color_Window, TtInvertGC, x + 2, y, w, 2);
	     XFillRectangle (TtDisplay, Color_Window, TtInvertGC, x, y, 2, h);
	     XFillRectangle (TtDisplay, Color_Window, TtInvertGC, x, y + h, w, 2);
	     XFillRectangle (TtDisplay, Color_Window, TtInvertGC, x + w, y + 2, 2, h);
#endif /* _WINDOWS */
	  }
     }

   /* nouveau foreground */
   if (LastFg != fground)
     {
	LastFg = fground;
	if (LastFg != -1)
	  {
	     x = (LastFg % COLORS_COL) * wcase;
	     y = ((LastFg / COLORS_COL) + 1) * hcase;
#ifndef _WINDOWS
	     w = wcase - 2;
	     h = hcase - 2;
	     XFillRectangle (TtDisplay, Color_Window, TtInvertGC, x + 2, y, w, 2);
	     XFillRectangle (TtDisplay, Color_Window, TtInvertGC, x, y, 2, h);
	     XFillRectangle (TtDisplay, Color_Window, TtInvertGC, x, y + h, w, 2);
	     XFillRectangle (TtDisplay, Color_Window, TtInvertGC, x + w, y + 2, 2, h);
#endif /* _WINDOWS */
	  }
     }
}


/*----------------------------------------------------------------------
   EndPalette
   Ends the display of the palette.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         EndPalette (ThotWidget w, int index, caddr_t call_d)

#else  /* __STDC__ */
static void         EndPalette (w, index, call_d)
ThotWidget          w;
int                 index;
caddr_t             call_d;

#endif /* __STDC__ */
{
#ifndef _WINDOWS
   XtPopdown (Color_Palette);
#endif
}


/*----------------------------------------------------------------------
   KillPalette
   kills the palette.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         KillPalette (ThotWidget w, int index, caddr_t call_d)

#else  /* __STDC__ */
static void         KillPalette (w, index, call_d)
ThotWidget          w;
int                 index;
caddr_t             call_d;

#endif /* __STDC__ */
{
   Color_Palette = 0;
   Color_Window = 0;
}


/*----------------------------------------------------------------------
   ColorsExpose
   redisplays a color keyboard.
  ----------------------------------------------------------------------*/
static void         ColorsExpose ()
{
   int                 max, y, w, h;
#  ifndef _WINDOWS 
   register int        i;
   int                 x;
   char               *ptr;
   int                 fground, bground;
#  endif /* !_WINDOWS */
   int                 wcase, hcase;

   if (TtWDepth == 1)
      /* Affiche le nom des couleurs sur un ecran N&B */
      wcase = CharacterWidth ('m', FontDialogue) * 12;
   else
      /* Affiche les couleurs sur un ecran couleur */
      wcase = CharacterWidth ('m', FontDialogue) * 4;

   w = wcase * COLORS_COL;
   hcase = FontHeight (FontDialogue);
   max = NumberOfColors ();
   y = hcase;
   h = (max + COLORS_COL - 1) / COLORS_COL * hcase + y;
#ifndef _WINDOWS
   XClearWindow (TtDisplay, Color_Window);

   /* entree couleur standard */
   if (SmallFontDialogue == NULL)
      KeyboardsLoadResources ();
   XSetForeground (TtDisplay, TtLineGC, ColorPixel (0));
   XFillRectangle (TtDisplay, Color_Window, TtLineGC, 0, 0, w, hcase - 2);
   XSetForeground (TtDisplay, TtLineGC, ColorPixel (1));
   ptr = TtaGetMessage (LIB, TMSG_STD_COLORS);
   WChaine (Color_Window, ptr,
	    (w / 2) - (XTextWidth ((XFontStruct *) SmallFontDialogue, ptr, strlen (ptr)) / 2),
	    0, FontDialogue, TtLineGC);

   /* grille */
   XSetLineAttributes (TtDisplay, TtLineGC, 1, LineSolid, CapButt, JoinMiter);
   for (x = wcase; x < w; x += wcase)
      XDrawLine (TtDisplay, Color_Window, TtLineGC, x, y, x, h);
   for (y = hcase; y < h; y += hcase)
      XDrawLine (TtDisplay, Color_Window, TtLineGC, 0, y, w, y);

   /* items */
   if (TtWDepth == 1)
      for (i = 0; i < max; i++)
	{
	   x = (i % COLORS_COL) * wcase;
	   y = ((i / COLORS_COL) + 1) * hcase;
	   WChaine (Color_Window, ColorName (i),
		    x + (wcase / 2) - (XTextWidth ((XFontStruct *) SmallFontDialogue, ColorName (i), strlen (ColorName (i))) / 2),
		    y, SmallFontDialogue, TtLineGC);
	}
   else
      for (i = 0; i < max; i++)
	{
	   x = (i % COLORS_COL) * wcase;
	   y = ((i / COLORS_COL) + 1) * hcase;
	   XSetForeground (TtDisplay, TtLineGC, ColorPixel (i));
	   XFillRectangle (TtDisplay, Color_Window, TtLineGC, x + 1, y + 1, wcase - 2, hcase - 2);
	}

   /* show the current selection */
   fground = LastFg;
   bground = LastBg;
   LastFg = -1;
   LastBg = -1;
   ThotSelectPalette (bground, fground);
#endif /* _WINDOWS */
}


/*----------------------------------------------------------------------
   ColorsPress
   handles a click on the color palette
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         ColorsPress (int button, int x, int y)
#else  /* __STDC__ */
static void         ColorsPress (button, x, y)
int                 button;
int                 x;
int                 y;

#endif /* __STDC__ */
{
   int                 color, li, co;
#  ifndef _WINDOWS
   int                 wcase, hcase;
#  endif /* !_WINDOWS */

#  ifndef _WINDOWS
   if (TtWDepth == 1)
      /* Affiche le nom des couleurs sur un ecran N&B */
      wcase = CharacterWidth ('m', FontDialogue) * 12;
   else
      /* Affiche les couleurs sur un ecran couleur */
      wcase = CharacterWidth ('m', FontDialogue) * 4;
   hcase = FontHeight (FontDialogue);

   /* Regarde si on n'a pas clique dans le titre */
   if (y < hcase)
     {
	if (button == Button1)
	  {
	     /* couleur de trace' standard */
	     ModifyColor (-1, FALSE);
	     ThotSelectPalette (LastBg, -1);
	  }
	else
	  {
	     /* couleur de fond standard */
	     ModifyColor (-1, TRUE);
	     ThotSelectPalette (-1, LastFg);
	  }
	return;
     }

   li = x / wcase;
   co = (y - hcase) / hcase;
   color = co * COLORS_COL + li;

#  else  /* _WINDOWS  */

   if (y < 45 || y > 330) {
	  if (button == Button1) {
	     /* couleur de trace' standard */
	     ModifyColor (-1, FALSE);
	     ThotSelectPalette (LastBg, -1);
	  } else {
	       /* couleur de fond standard */
	       ModifyColor (-1, TRUE);
	       ThotSelectPalette (-1, LastFg);
	  }
	  return;
   } 
   li = (y - 45) / 15;
   co = x / 35;
   color = co + li * COLORS_COL;
#  endif /* _WINDOWS */
   if (button == Button1)
     {
	/* selectionne la couleur de trace' */
	ModifyColor (color, FALSE);
	ThotSelectPalette (LastBg, color);
     }
   else
     {
	/* selectionne la couleur de fond */
	ModifyColor (color, TRUE);
	ThotSelectPalette (color, LastFg);
     }

}


#ifndef _WINDOWS
/*----------------------------------------------------------------------
   ColorsEvent
   handles the X events of the palette.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ColorsEvent (ThotEvent * event)
#else  /* __STDC__ */
void                ColorsEvent (event)
ThotEvent             *event;

#endif /* __STDC__ */
{
   if (event->xbutton.window == Color_Window && Color_Window != 0)
     {
	if (event->type == Expose)
	   ColorsExpose ();
	else if (event->type == ButtonPress)
	   ColorsPress (event->xbutton.button, event->xbutton.x, event->xbutton.y);
     }
}
#endif /* !_WINDOWS */


/*----------------------------------------------------------------------
   ThotCreatePalette
   creates the color palette.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         ThotCreatePalette (int x, int y)
#else  /* __STDC__ */
static void         ThotCreatePalette (x, y)
int                 x;
int                 y;

#endif /* __STDC__ */
{
#  ifndef _WINDOWS
   int                 n;
   int                 width, height;
   ThotWidget          w;
   ThotWidget          row;
   ThotWidget          frame;
   Arg                 args[MAX_ARGS];
   XmString            title_string;
   XmFontList          xfont;
   XGCValues           GCmodel;
   char                string[10];

   xfont = XmFontListCreate ((XFontStruct *) FontDialogue, XmSTRING_DEFAULT_CHARSET);
   if (SmallFontDialogue == NULL)
      SmallFontDialogue = ReadFont ('L', 'H', 0, 9, UnPoint);

   n = 0;
   sprintf (string, "+%d+%d", x, y);
   XtSetArg (args[n], XmNx, (Position) x);
   n++;
   XtSetArg (args[n], XmNy, (Position) y);
   n++;
   XtSetArg (args[n], XmNallowShellResize, TRUE);
   n++;
   XtSetArg (args[n], XmNuseAsyncGeometry, TRUE);
   n++;
   Color_Palette = XtCreatePopupShell (TtaGetMessage (LIB, TMSG_COLORS),
			   applicationShellWidgetClass, RootShell, args, n);
/*** Cree la palette dans sa frame ***/
   n = 0;
   XtSetArg (args[n], XmNbackground, BgMenu_Color);
   n++;
   XtSetArg (args[n], XmNborderColor, Button_Color);
   n++;
   XtSetArg (args[n], XmNfontList, xfont);
   n++;
   title_string = XmStringCreateSimple (TtaGetMessage (LIB, TMSG_COLORS));
   XtSetArg (args[n], XmNdialogTitle, title_string);
   n++;
   XtSetArg (args[n], XmNautoUnmanage, FALSE);
   n++;
   XtSetArg (args[n], XmNmarginWidth, 0);
   n++;
   XtSetArg (args[n], XmNmarginHeight, 0);
   n++;
   XtSetArg (args[n], XmNspacing, 0);
   n++;
   XtSetArg (args[n], XmNresizePolicy, XmRESIZE_GROW);
   n++;
   w = XmCreateBulletinBoard (Color_Palette, "Dialogue", args, n);
   XtManageChild (w);
   XtAddCallback (w, XmNdestroyCallback, (XtCallbackProc) KillPalette, NULL);
   XmStringFree (title_string);

/*** Cree un Row-Column pour ajouter les labels et le bouton Quit ***/
/*** au dessus et en dessous des touches du clavier.    ***/
   n = 0;
   XtSetArg (args[n], XmNbackground, BgMenu_Color);
   n++;
   XtSetArg (args[n], XmNborderColor, Button_Color);
   n++;
   XtSetArg (args[n], XmNadjustLast, FALSE);
   n++;
   XtSetArg (args[n], XmNmarginWidth, 0);
   n++;
   XtSetArg (args[n], XmNmarginHeight, 0);
   n++;
   XtSetArg (args[n], XmNspacing, 0);
   n++;
   XtSetArg (args[n], XmNpacking, XmPACK_TIGHT);
   n++;
   XtSetArg (args[n], XmNorientation, XmVERTICAL);
   n++;
   XtSetArg (args[n], XmNresizeHeight, TRUE);
   n++;
   row = XmCreateRowColumn (w, "Dialogue", args, n);
   XtManageChild (row);

   /* Les labels */
   n = 0;
   XtSetArg (args[n], XmNborderColor, Button_Color);
   n++;
   XtSetArg (args[n], XmNforeground, Black_Color);
   n++;
   XtSetArg (args[n], XmNfontList, xfont);
   n++;
   XtSetArg (args[n], XmNbackground, BgMenu_Color);
   n++;
   title_string = XmStringCreateSimple (TtaGetMessage (LIB, TMSG_BUTTON_1));
   XtSetArg (args[n], XmNlabelString, title_string);
   n++;
   w = XmCreateLabel (row, "Thot_MSG", args, n);
   XtManageChild (w);
   XmStringFree (title_string);
   n = 0;
   XtSetArg (args[n], XmNborderColor, Button_Color);
   n++;
   XtSetArg (args[n], XmNforeground, Black_Color);
   n++;
   XtSetArg (args[n], XmNfontList, xfont);
   n++;
   XtSetArg (args[n], XmNbackground, BgMenu_Color);
   n++;
   title_string = XmStringCreateSimple (TtaGetMessage (LIB, TMSG_BUTTON_2));
   XtSetArg (args[n], XmNlabelString, title_string);
   n++;
   w = XmCreateLabel (row, "Thot_MSG", args, n);
   XtManageChild (w);
   XmStringFree (title_string);

   /* Evalue la largeur et la hauteur de la palette */
   n = 0;
   if (TtWDepth == 1)
      /* Affiche le nom des couleurs sur un ecran N&B */
      width = CharacterWidth ('m', FontDialogue) * 12 * COLORS_COL;
   else
      /* Affiche les couleurs sur un ecran couleur */
      width = CharacterWidth ('m', FontDialogue) * 4 * COLORS_COL;
   height = ((NumberOfColors () + COLORS_COL - 1) / COLORS_COL + 1) * FontHeight (FontDialogue);

/*** Cree un DrawingArea pour contenir les touches de la palette ***/
   n = 0;
   XtSetArg (args[n], XmNborderColor, Button_Color);
   n++;
   XtSetArg (args[n], XmNfontList, xfont);
   n++;
   XtSetArg (args[n], XmNbackground, BgMenu_Color);
   n++;
   XtSetArg (args[n], XmNmarginWidth, 0);
   n++;
   XtSetArg (args[n], XmNmarginHeight, 0);
   n++;
   XtSetArg (args[n], XmNwidth, (Dimension) width);
   n++;
   XtSetArg (args[n], XmNheight, (Dimension) height);
   n++;
   XtSetArg (args[n], XmNkeyboardFocusPolicy, XmPOINTER);
   n++;
   frame = XmCreateFrame (row, "Frame", args, n);
   XtManageChild (frame);
   n = 0;
   XtSetArg (args[n], XmNborderColor, Button_Color);
   n++;
   XtSetArg (args[n], XmNfontList, xfont);
   n++;
   XtSetArg (args[n], XmNbackground, BgMenu_Color);
   n++;
   XtSetArg (args[n], XmNmarginWidth, 0);
   n++;
   XtSetArg (args[n], XmNmarginHeight, 0);
   n++;
   XtSetArg (args[n], XmNwidth, (Dimension) width);
   n++;
   XtSetArg (args[n], XmNheight, (Dimension) height);
   n++;
   XtSetArg (args[n], XmNkeyboardFocusPolicy, XmPOINTER);
   n++;
   frame = XmCreateDrawingArea (frame, "", args, n);
   XtManageChild (frame);

/*** Cree un Row-Column pour contenir le bouton Quit ***/
   n = 0;
   XtSetArg (args[n], XmNborderColor, Button_Color);
   n++;
   XtSetArg (args[n], XmNfontList, xfont);
   n++;
   XtSetArg (args[n], XmNbackground, BgMenu_Color);
   n++;
   XtSetArg (args[n], XmNorientation, XmHORIZONTAL);
   n++;
   XtSetArg (args[n], XmNmarginWidth, 100);
   n++;
   XtSetArg (args[n], XmNmarginHeight, 0);
   n++;
   row = XmCreateRowColumn (row, "Dialogue", args, n);
   XtManageChild (row);

   /* Cree si necessaire le contexte graphique des cles */
   GCmodel.function = GXcopy;
   GCmodel.foreground = Black_Color;
   GCmodel.background = BgMenu_Color;
   GCkey = XCreateGC (TtDisplay, TtRootWindow, GCForeground | GCBackground | GCFunction, &GCmodel);

/*** Cree le bouton Quit ***/
   n = 0;
   XtSetArg (args[n], XmNbackground, Button_Color);
   n++;
   XtSetArg (args[n], XmNbottomShadowColor, BgMenu_Color);
   n++;
   XtSetArg (args[n], XmNforeground, Black_Color);
   n++;
   XtSetArg (args[n], XmNfontList, xfont);
   n++;
   w = XmCreatePushButton (row, TtaGetMessage (LIB, TMSG_DONE), args, n);
   XtManageChild (w);
   XtAddCallback (w, XmNactivateCallback, (XtCallbackProc) EndPalette, NULL);

   /* Definit le bouton d'annulation comme bouton par defaut */
   n = 0;
   XtSetArg (args[n], XmNdefaultButton, w);
   n++;
   XtSetValues (Color_Palette, args, n);


   /* Force la largeur et la hauteur de la palette */
   XtSetArg (args[n], XmNwidth, (Dimension) width);
   n++;
   XtSetArg (args[n], XmNheight, (Dimension) height);
   n++;
   XtSetValues (frame, args, n);

   XmFontListFree (xfont);
   /* affiche la palette */
   XtPopup (Color_Palette, XtGrabNonexclusive);
   Color_Window = XtWindowOfObject (frame);
   /* pas de selection precedente */
   LastBg = -1;
   LastFg = -1;
#  else  /* _WINDOWS */
   WNDCLASSEX  wndThotPaletteClass ;
   static char szAppName[] = "ThotColorPalette" ;
   HWND        hwnColorPal;
   MSG         msg;
   if (!wndRegistered) {
	  wndRegistered = TRUE;
      wndThotPaletteClass.style         = CS_HREDRAW | CS_VREDRAW ;
      wndThotPaletteClass.lpfnWndProc   = ThotColorPaletteWndProc ;
      wndThotPaletteClass.cbClsExtra    = 0 ;
      wndThotPaletteClass.cbWndExtra    = 0 ;
      wndThotPaletteClass.hInstance     = hInstance ;
      wndThotPaletteClass.hIcon         = LoadIcon (NULL, iconID) ;
      wndThotPaletteClass.hCursor       = LoadCursor (NULL, IDC_ARROW) ;
      wndThotPaletteClass.hbrBackground = (HBRUSH) GetStockObject (WHITE_BRUSH) ;
      wndThotPaletteClass.lpszMenuName  = NULL ;
      wndThotPaletteClass.lpszClassName = szAppName ;
      wndThotPaletteClass.cbSize        = sizeof(WNDCLASSEX);
      wndThotPaletteClass.hIconSm       = LoadIcon (hInstance, iconID) ;

      if (!RegisterClassEx (&wndThotPaletteClass))
         return FALSE;
   }

   hwnColorPal = CreateWindow (szAppName, TtaGetMessage (LIB, TMSG_COLORS),
                               DS_MODALFRAME | WS_POPUP | 
                               WS_VISIBLE | WS_CAPTION | WS_SYSMENU,
                               ClickX, ClickY,
                               280, 385,
                               NULL, NULL, hInstance, NULL) ;

   ShowWindow (hwnColorPal, SW_SHOWNORMAL) ;
   UpdateWindow (hwnColorPal) ;

   while (GetMessage (&msg, NULL, 0, 0)) {
         TranslateMessage (&msg) ;
         DispatchMessage (&msg) ;
   }
#  endif /* _WINDOWS */
}

/*----------------------------------------------------------------------
 ThotColorPaletteWndProc
  ----------------------------------------------------------------------*/
#ifdef _WINDOWS
#ifdef __STDC__
LRESULT CALLBACK ThotColorPaletteWndProc (HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
#else  /* __STDC__ */
LRESULT CALLBACK ThotColorPaletteWndProc (HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
#endif /* __STDC__ */
{
     static BOOL fState[HORIZ_DIV][VERT_DIV] ;
     static int  cxBlock, cyBlock;
	 static BOOL inColorArea = FALSE;
     HDC         hdc ;
     PAINTSTRUCT ps ;
	 HBRUSH      hBrush;
	 HBRUSH      hOldBrush;
     int         i, x, y, nbPalEntries;
	 HWND        hwnLButton;
	 HWND        hwnRButton;
	 HWND        hwnDefaultColors;
	 HWND        doneButton;

     switch (iMsg) {
	      case WM_CREATE:
			   cxBlock = 35 ;
			   cyBlock = 15 ;

			   hwnLButton = CreateWindow ("STATIC", TtaGetMessage (LIB, TMSG_BUTTON_1), 
				                          WS_CHILD | WS_VISIBLE | SS_LEFT, 0, 0, 274, 15,
										  hwnd, (HMENU) 99, hInstance, NULL) ;
               ShowWindow (hwnLButton, SW_SHOWNORMAL);
               UpdateWindow (hwnLButton);

			   hwnRButton = CreateWindow ("STATIC", TtaGetMessage (LIB, TMSG_BUTTON_2), 
				                          WS_CHILD | WS_VISIBLE | SS_LEFT, 0, 15, 274, 15,
										  hwnd, (HMENU) 101, hInstance, NULL) ;
               ShowWindow (hwnRButton, SW_SHOWNORMAL);
               UpdateWindow (hwnRButton);

			   hwnDefaultColors = CreateWindow ("STATIC", "                   Default Colors", 
				                                WS_CHILD | WS_VISIBLE | SS_LEFT, 0, 30, 274, 15,
										        hwnd, (HMENU) 103, hInstance, NULL) ;
               ShowWindow (hwnDefaultColors, SW_SHOWNORMAL);
               UpdateWindow (hwnDefaultColors);

			   doneButton = CreateWindow ("BUTTON", TtaGetMessage (LIB, TMSG_DONE), 
                                          WS_CHILD | BS_PUSHBUTTON | WS_VISIBLE,
                                          115, 340, 50, 20, hwnd, 
                                          (HMENU) _IDDONE_, hInstance, NULL) ;
               ShowWindow (doneButton, SW_SHOWNORMAL);
               UpdateWindow (doneButton);

			   return 0;

          case WM_LBUTTONDOWN:
			   if (HIWORD (lParam) >= 45 && HIWORD (lParam) <= 330) 
			      ColorsPress (1, LOWORD (lParam), HIWORD (lParam));
			   return 0;

          case WM_MBUTTONDOWN:
			   ColorsPress (2, LOWORD (lParam), HIWORD (lParam));
			   return 0;

          case WM_RBUTTONDOWN:
			   ColorsPress (3, LOWORD (lParam), HIWORD (lParam));
			   return 0;

          case WM_PAINT:
               ptrLogPal = (PLOGPALETTE) LocalAlloc (LMEM_FIXED, sizeof(LOGPALETTE) + MAX_COLOR * sizeof(PALETTEENTRY));


               ptrLogPal->palVersion    = 0x300;
               ptrLogPal->palNumEntries = MAX_COLOR;
       
               for (i = 0; i < MAX_COLOR; i++) {
                   ptrLogPal->palPalEntry[i].peRed   = (BYTE) RGB_Table[i].red;
                   ptrLogPal->palPalEntry[i].peGreen = (BYTE) RGB_Table[i].green;
                   ptrLogPal->palPalEntry[i].peBlue  = (BYTE) RGB_Table[i].blue;
                   ptrLogPal->palPalEntry[i].peFlags = PC_RESERVED;
               }

               TtCmap = CreatePalette (ptrLogPal);
               LocalFree (ptrLogPal);
      
               if (TtCmap == NULL) 
                  WinErrorBox (WIN_Main_Wd);
               else {
			   
               hdc = BeginPaint (hwnd, &ps) ;
               SelectPalette (hdc, TtCmap, FALSE);
               nbPalEntries = RealizePalette (hdc);

               for (y = 0 ; y < VERT_DIV ; y++)
                   for (x = 0 ; x < HORIZ_DIV ; x++) {
                       hBrush = CreateSolidBrush (PALETTEINDEX (x + y * HORIZ_DIV)) ;
                       hOldBrush = SelectObject (hdc, hBrush);
                       Rectangle (hdc, x * cxBlock, (y * cyBlock) + 45,(x + 1) * cxBlock, (y + 1) * cyBlock + 45) ;
                       SelectObject (hdc, hOldBrush);
                       if (!DeleteObject (hBrush))
                          WinErrorBox (WIN_Main_Wd);
                   }
			   
               EndPaint (hwnd, &ps);
               DeleteDC (hdc);
               } 
               return 0 ;

		  case WM_COMMAND:
			   switch (LOWORD (wParam)) {
			          case _IDDONE_:
                           if (!DeleteObject (TtCmap))
                              WinErrorBox (WIN_Main_Wd);
                           DestroyWindow (hwnd);
						   return 0;
			   }

          case WM_DESTROY :
               PostQuitMessage (0) ;
               return 0 ;
	 }
     return DefWindowProc (hwnd, iMsg, wParam, lParam) ;
}
#endif /* _WINDOWS */


/*----------------------------------------------------------------------
   TtcChangeColors
   the user wants to modify the colors (specific presentation) for the
   view "view" of document pDoc (if Assoc = FALSE) or the elements associated
   of number "view" (if Assoc = TRUE).
   Initializes and activates the corresponding menu.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtcChangeColors (Document document, View view)
#else  /* __STDC__ */
void                TtcChangeColors (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   PtrDocument         pSelDoc;
   PtrElement          pFirstSel, pLastSel;
   PtrAbstractBox      pAb;
   PtrDocument         pDoc;
   int                 firstChar, lastChar;
   int                 KbX, KbY;
   boolean             selectionOK;

#ifndef _WINDOWS
   if (ThotLocalActions[T_colors] == NULL)
     {
	/* Connecte le traitement des evenements */
	TteConnectAction (T_colors, (Proc) ColorsEvent);
     }
#endif /* _WINDOWS */

   pDoc = LoadedDocument[document - 1];
   /* demande quelle est la selection courante */
   selectionOK = GetCurrentSelection (&pSelDoc, &pFirstSel, &pLastSel, &firstChar, &lastChar);
   if (!selectionOK)
     {
	/* par defaut la racine du document */
	pSelDoc = pDoc;
	pFirstSel = pDoc->DocRootElement;
	selectionOK = TRUE;
     }

   if (selectionOK && pSelDoc == pDoc && NumberOfColors () > 0)
      /* il y a effectivement des couleurs disponibles */
     {
	/* Cree la palette si elle n'existe pas */
	if (Color_Window == 0)
	  {	    
	    ConfigKeyboard (&KbX, &KbY);
	    ThotCreatePalette (KbX, KbY);
	  }
#ifndef _WINDOWS
	else
	   XtPopup (Color_Palette, XtGrabNonexclusive);
#endif /* _WINDOWS */

	/* recherche le pave concerne */
	if (view > 100)
	   pAb = AbsBoxOfEl (pFirstSel, 1);
	else
	   pAb = AbsBoxOfEl (pFirstSel, view);

	if (pAb != NULL)
	   ThotSelectPalette (pAb->AbBackground, pAb->AbForeground);
     }
}
