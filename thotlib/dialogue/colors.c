/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2001
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * Colors Keyboard routines
 *
 * Authors: I. Vatton (INRIA)
 *          R. Guetari (W3C/INRIA) - Windows version
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
#include "wininclude.h"
#define HORIZ_DIV  8
#define VERT_DIV  19
#define Button1    1		
#ifdef _IDDONE_
#undef _IDDONE_
#endif /* _IDDONE_ */
#define _IDDONE_     100
#define DEFAULTCOLOR 103
static HWND   HwndColorPal  = NULL;
extern LPCTSTR iconID;
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

static ThotWindow   Color_Window = 0;
static ThotWidget   Color_Palette;
static ThotGC       GCkey;
static int          LastBg;
static int          LastFg;
static int          FgColor, BgColor;
static ThotBool     applyToSelection = TRUE;
#ifdef _WINDOWS
/* @@ JK: a quick, ugly hack just for selecting the messages drawn on the
color palette */
static ThotBool     PalMessageSet1 = TRUE;
static ThotBool     IsRegistered = FALSE;
#endif

#ifdef _GTK
#include "gtk-functions.h"
#endif /* _GTK */

#include "appdialogue_f.h"
#include "actions_f.h"
#include "changepresent_f.h"
#include "config_f.h"
#include "font_f.h"
#include "inites_f.h"
#include "presentmenu_f.h"
#include "structselect_f.h"
#include "xwindowdisplay_f.h"


/*----------------------------------------------------------------------
   ThotSelectPalette
   Shows the current selection.
  ----------------------------------------------------------------------*/
static void ThotSelectPalette (int bground, int fground)
{
#ifdef _WINDOWS
   BgColor = bground;
   FgColor = fground;
#else  /* _WINDOWS */
   int                 x, y;
   int                 wcase, hcase;
   int                 w, h;

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
	x -= 2;
	y -= 2;
	w = wcase + 2;
	h = hcase + 2;
#ifndef _GTK
	XFillRectangle (TtDisplay, Color_Window, TtInvertGC, x + 2, y, w, 2);
	XFillRectangle (TtDisplay, Color_Window, TtInvertGC, x, y, 2, h);
	XFillRectangle (TtDisplay, Color_Window, TtInvertGC, x, y + h, w, 2);
	XFillRectangle (TtDisplay, Color_Window, TtInvertGC, x + w, y + 2, 2, h);
#else /* _GTK */
	gdk_draw_rectangle (Color_Window, TtInvertGC, TRUE, x + 2, y, w, 2);
	gdk_draw_rectangle (Color_Window, TtInvertGC, TRUE, x, y, 2, h);
	gdk_draw_rectangle (Color_Window, TtInvertGC, TRUE, x, y + h, w, 2);
	gdk_draw_rectangle (Color_Window, TtInvertGC, TRUE, x + w, y + 2, 2, h);
#endif /* !_GTK */
     }

   if (LastFg != -1 && LastFg != fground)
     {
	/* eteint le foreground precedent */
	x = (LastFg % COLORS_COL) * wcase;
	y = ((LastFg / COLORS_COL) + 1) * hcase;
	w = wcase - 2;
	h = hcase - 2;
#ifndef _GTK
	XFillRectangle (TtDisplay, Color_Window, TtInvertGC, x + 2, y, w, 2);
	XFillRectangle (TtDisplay, Color_Window, TtInvertGC, x, y, 2, h);
	XFillRectangle (TtDisplay, Color_Window, TtInvertGC, x, y + h, w, 2);
	XFillRectangle (TtDisplay, Color_Window, TtInvertGC, x + w, y + 2, 2, h);
#else /* _GTK */
	gdk_draw_rectangle (Color_Window, TtInvertGC, TRUE, x + 2, y, w, 2);
	gdk_draw_rectangle (Color_Window, TtInvertGC, TRUE, x, y, 2, h);
	gdk_draw_rectangle (Color_Window, TtInvertGC, TRUE, x, y + h, w, 2);
	gdk_draw_rectangle (Color_Window, TtInvertGC, TRUE, x + w, y + 2, 2, h);
#endif /* !_GTK */
     }

   /* nouveau background */
   if (LastBg != bground)
     {
	LastBg = bground;
	if (LastBg != -1)
	  {
	     x = (LastBg % COLORS_COL) * wcase;
	     y = ((LastBg / COLORS_COL) + 1) * hcase;
	     x -= 2;
	     y -= 2;
	     w = wcase + 2;
	     h = hcase + 2;
#ifndef _GTK
	     XFillRectangle (TtDisplay, Color_Window, TtInvertGC, x + 2, y, w, 2);
	     XFillRectangle (TtDisplay, Color_Window, TtInvertGC, x, y, 2, h);
	     XFillRectangle (TtDisplay, Color_Window, TtInvertGC, x, y + h, w, 2);
	     XFillRectangle (TtDisplay, Color_Window, TtInvertGC, x + w, y + 2, 2, h);
#else /* _GTK */
	gdk_draw_rectangle (Color_Window, TtInvertGC, TRUE, x + 2, y, w, 2);
	gdk_draw_rectangle (Color_Window, TtInvertGC, TRUE, x, y, 2, h);
	gdk_draw_rectangle (Color_Window, TtInvertGC, TRUE, x, y + h, w, 2);
	gdk_draw_rectangle (Color_Window, TtInvertGC, TRUE, x + w, y + 2, 2, h);
#endif /* !_GTK */
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
	     w = wcase - 2;
	     h = hcase - 2;
#ifndef _GTK
	     XFillRectangle (TtDisplay, Color_Window, TtInvertGC, x + 2, y, w, 2);
	     XFillRectangle (TtDisplay, Color_Window, TtInvertGC, x, y, 2, h);
	     XFillRectangle (TtDisplay, Color_Window, TtInvertGC, x, y + h, w, 2);
	     XFillRectangle (TtDisplay, Color_Window, TtInvertGC, x + w, y + 2, 2, h);
#else /* _GTK */
	gdk_draw_rectangle (Color_Window, TtInvertGC, TRUE, x + 2, y, w, 2);
	gdk_draw_rectangle (Color_Window, TtInvertGC, TRUE, x, y, 2, h);
	gdk_draw_rectangle (Color_Window, TtInvertGC, TRUE, x, y + h, w, 2);
	gdk_draw_rectangle (Color_Window, TtInvertGC, TRUE, x + w, y + 2, 2, h);
#endif /* !_GTK */
	  }
     }
#endif /* _WINDOWS */
}



/*----------------------------------------------------------------------
   KillPalette
   kills the palette.
  ----------------------------------------------------------------------*/
static void KillPalette (ThotWidget w, int index, caddr_t call_d)
{
   Color_Palette = 0;
   Color_Window = 0;
}


/*----------------------------------------------------------------------
   ColorsExpose
   redisplays a color keyboard.
  ----------------------------------------------------------------------*/
#ifndef _GTK
static void ColorsExpose ()
#else /* _GTK */
static gboolean ColorsExposeGTK (GtkWidget *widget, GdkEventExpose *ev)
#endif /* !_GTK */
{
   int                 max, y, w, h;
#ifndef _WINDOWS 
   char               *ptr;
   register int        i;
   int                 x;
   int                 fground, bground;
#endif /* !_WINDOWS */
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
#ifndef _GTK
   XClearWindow (TtDisplay, Color_Window);
#endif /* !_GTK */

   /* entree couleur standard */
   if (SmallFontDialogue == NULL)
      KeyboardsLoadResources ();
#ifndef _GTK
   XSetForeground (TtDisplay, TtLineGC, ColorPixel (0));
   XFillRectangle (TtDisplay, Color_Window, TtLineGC, 0, 0, w, hcase - 2);
   XSetForeground (TtDisplay, TtLineGC, ColorPixel (1));
   ptr = TtaGetMessage (LIB, TMSG_STD_COLORS);
   WChaine (Color_Window, ptr,
	    (w / 2) - (XTextWidth ((XFontStruct *) SmallFontDialogue, ptr, strlen (ptr)) / 2),
	    0, FontDialogue, TtLineGC);
#else /* _GTK */
   gdk_rgb_gc_set_foreground (TtLineGC, ColorPixel (0));
   gdk_draw_rectangle (Color_Window, TtLineGC, TRUE, 0, 0, w, hcase - 2);
   gdk_rgb_gc_set_foreground (TtLineGC, ColorPixel (1));
   ptr = TtaGetMessage (LIB, TMSG_STD_COLORS);
   WChaine (Color_Window, ptr,
	    (w / 2) - (gdk_text_width (SmallFontDialogue, ptr, strlen (ptr)) / 2),
	    0, FontDialogue, TtLineGC);
#endif /* !_GTK */

   /* grille */
#ifndef _GTK
   XSetLineAttributes (TtDisplay, TtLineGC, 1, LineSolid, CapButt, JoinMiter);
   for (x = wcase; x < w; x += wcase)
      XDrawLine (TtDisplay, Color_Window, TtLineGC, x, y, x, h);
   for (y = hcase; y < h; y += hcase)
      XDrawLine (TtDisplay, Color_Window, TtLineGC, 0, y, w, y);
#else /* _GTK */
   gdk_gc_set_line_attributes (TtLineGC, 1, GDK_LINE_SOLID, GDK_CAP_BUTT, GDK_JOIN_MITER);
   for (x = wcase; x < w; x += wcase)
     gdk_draw_line (Color_Window, TtLineGC, x, y, x, h);
   for (y = hcase; y < h; y += hcase)
     gdk_draw_line (Color_Window, TtLineGC, 0, y, w, y);
#endif /* !_GTK */
   /* items */
   if (TtWDepth == 1)
      for (i = 0; i < max; i++)
	{
	   x = (i % COLORS_COL) * wcase;
	   y = ((i / COLORS_COL) + 1) * hcase;
#ifndef _GTK
	   WChaine (Color_Window, ColorName (i),
		    x + (wcase / 2) - (XTextWidth ((XFontStruct *) SmallFontDialogue, ColorName (i), strlen (ColorName (i))) / 2),
		    y, SmallFontDialogue, TtLineGC);
#else /* _GTK */
	   WChaine (Color_Window, ColorName (i),
		    x + (wcase / 2) - (gdk_text_width (SmallFontDialogue, ColorName (i), strlen (ColorName (i))) / 2),
		    y, SmallFontDialogue, TtLineGC);
#endif /* !_GTK */
	}
   else
      for (i = 0; i < max; i++)
	{
	   x = (i % COLORS_COL) * wcase;
	   y = ((i / COLORS_COL) + 1) * hcase;
#ifndef _GTK
	   XSetForeground (TtDisplay, TtLineGC, ColorPixel (i));
	   XFillRectangle (TtDisplay, Color_Window, TtLineGC, x + 1, y + 1, wcase - 2, hcase - 2);
#else /* _GTK */
	   gdk_rgb_gc_set_foreground (TtLineGC, ColorPixel (i));
	   gdk_draw_rectangle (Color_Window, TtLineGC, TRUE, x + 1, y + 1, wcase - 2, hcase - 2);
#endif /* !_GTK */
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
#ifndef _GTK
static void ColorsPress (int button, int x, int y)
#else /*_GTK*/
static gboolean ColorsPressGTK (GtkWidget *widget, GdkEventButton *event, gpointer data)
#endif /* !_GTK */
{
#ifdef _GTK
  int button = event->button;
  int x = event->x;
  int y = event->y;
#endif /* _GTK */
  int                 color, li, co;
#ifndef _WINDOWS
  int                 wcase, hcase;

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
#else  /* _WINDOWS  */
  if (y < 60 || y > 345)
    {
      if (button == Button1)
	{
	  /* couleur de trace' standard */
	  FgColor = -1;
	  if (applyToSelection)
	    {
	      ModifyColor (-1, FALSE);
	      ThotSelectPalette (LastBg, -1);
	    }
	}
      else
	{
	  /* couleur de fond standard */
	  BgColor = -1;
	  if (applyToSelection)
	    {
	      ModifyColor (-1, TRUE);
	      BgColor = -1;
	    }
	}
      return;
    } 
  li = (y - 60) / 15;
  co = x / 39;
  color = co + li * COLORS_COL;
#endif /* _WINDOWS */
  if (button == Button1)
    {
      /* selectionne la couleur de trace' */
      FgColor = color;
      if (applyToSelection)
	{
	  ModifyColor (color, FALSE);
	  ThotSelectPalette (LastBg, color);
	}
    }
  else
    {
      /* selectionne la couleur de fond */
      BgColor = color;
      if (applyToSelection)
	{
	  ModifyColor (color, TRUE);
	  ThotSelectPalette (color, LastFg);
	}
    }
}


#ifdef _WINDOWS
/*----------------------------------------------------------------------
 SelectANewFgColor
  ----------------------------------------------------------------------*/
static void SelectANewFgColor (HWND hwnd)
{
  HDC             hdc;
  HBRUSH          hBrush;
  HBRUSH          hOldBrush;
  HPEN            hPen;
  HPEN            hOldPen;
  int             x, y;
  int             color;
  unsigned short  red, green, blue;

  if ( LastFg != FgColor)
    {
      hdc = GetDC (hwnd);
      /* Switch off last FG color */
      if (LastFg >= 0 && LastFg < NColors)
	{
	  x = (LastFg % COLORS_COL) * 39;
	  y = (LastFg / COLORS_COL) * 15 + 60;
	  hBrush = CreateSolidBrush (ColorPixel (LastFg));
	  hOldBrush = SelectObject (hdc, hBrush);
	  Rectangle (hdc, x, y, x + 39, y + 15);
	  SelectObject (hdc, hOldBrush);
	  DeleteObject (hBrush);
	}
      /* Switch on last FG color */
      LastFg = FgColor;
      if (LastFg >= 0 && LastFg < NColors)
	{
	  x = (LastFg % COLORS_COL) * 39;
	  y = (LastFg / COLORS_COL) * 15 + 60;
	  x += 1;
	  y += 1;	  
	  red   = 255 - RGB_Table [LastFg].red;
	  green = 255 - RGB_Table [LastFg].green;
	  blue  = 255 - RGB_Table [LastFg].blue;
	  if (TtIsTrueColor)
	    hPen = CreatePen (PS_SOLID, 1, RGB (red, green, blue));
	  else
	    {
	      /* give the nearest color pixel */
	      color = TtaGetThotColor (red, green, blue);
	      hPen = CreatePen (PS_SOLID, 1, ColorPixel (color));
	    }
	  hBrush = CreateSolidBrush (ColorPixel (LastFg));
	  hOldPen = SelectObject (hdc, hPen);
	  hOldBrush = SelectObject (hdc, hBrush);
	  Rectangle (hdc, x, y, x + 37, y + 13);
	  SelectObject (hdc, hOldBrush);
	  DeleteObject (hBrush);
	  SelectObject (hdc, hOldPen);
	  DeleteObject (hPen);
	}
      DeleteDC (hdc);
    }
}

/*----------------------------------------------------------------------
 SelectANewBgColor
  ----------------------------------------------------------------------*/
static void SelectANewBgColor (HWND hwnd)
{
  HDC             hdc;
  HBRUSH          hBrush;
  HBRUSH          hOldBrush;
  HPEN            hPen;
  HPEN            hOldPen;
  int             x, y;
  int             color;
  unsigned short  red, green, blue;

  if ( LastBg != BgColor)
    {
      hdc = GetDC (hwnd);
      /* Switch off last BG color */
      if (LastBg >= 0 && LastBg < NColors)
	{
	  x = (LastBg % COLORS_COL) * 39;
	  y = (LastBg / COLORS_COL) * 15 + 60;      
	  hBrush = CreateSolidBrush (ColorPixel (LastBg));
	  hOldBrush = SelectObject (hdc, hBrush);
	  Rectangle (hdc, x, y, x + 39, y + 15);
	  SelectObject (hdc, hOldBrush);
	  DeleteObject (hBrush);
	}

      /* Switch on last FG color */
      LastBg = BgColor;
      if (LastBg >= 0 && LastBg < NColors)
	{
	  x = (LastBg % COLORS_COL) * 39;
	  y = (LastBg / COLORS_COL) * 15 + 60;
	  x += 1;
	  y += 1;
	  
	  hdc = GetDC (hwnd);
	  red   = 255 - RGB_Table [LastBg].red;
	  green = 255 - RGB_Table [LastBg].green;
	  blue  = 255 - RGB_Table [LastBg].blue;
	  if (TtIsTrueColor)
	    hPen = CreatePen (PS_SOLID, 1, RGB (red, green, blue));
	  else
	    {
	      /* give the nearest color pixel */
	      color = TtaGetThotColor (red, green, blue);
	      hPen = CreatePen (PS_SOLID, 1, ColorPixel (color));
	    }
	  hBrush = CreateSolidBrush (ColorPixel (LastBg));
	  hOldPen = SelectObject (hdc, hPen);
	  hOldBrush = SelectObject (hdc, hBrush);
	  Rectangle (hdc, x, y, x + 37, y + 13);
	  SelectObject (hdc, hOldBrush);
	  DeleteObject (hBrush);
	  SelectObject (hdc, hOldPen);
	  DeleteObject (hPen);
	}
      DeleteDC (hdc);
    }
}

/*----------------------------------------------------------------------
 ThotColorPaletteWndProc
  ----------------------------------------------------------------------*/
LRESULT CALLBACK ThotColorPaletteWndProc (HWND hwnd, UINT iMsg,
					  WPARAM wParam, LPARAM lParam)
{
  static int      cxBlock, cyBlock;
  HDC             hdc;
  PAINTSTRUCT     ps;
  HBRUSH          hBrush;
  HBRUSH          hOldBrush;
  int             x, y, nbPalEntries;
  int             yPos;
  HWND            hwnLButton;
  HWND            hwnRButton;
  HWND            hwnDefaultColors;
  HWND            doneButton;
  HFONT           newFont;

  switch (iMsg)
    {
    case WM_CREATE:
      /* get the default GUI font */
      newFont = GetStockObject (DEFAULT_GUI_FONT); 
      cxBlock = 39;
      cyBlock = 15;
      hwnLButton = CreateWindow ("STATIC",
				 (applyToSelection || PalMessageSet1)
				 ? TtaGetMessage (LIB, TMSG_BUTTON_1)
				 : TtaGetMessage (LIB, TMSG_CPBUTTON_1), 
				 WS_CHILD | WS_VISIBLE | SS_LEFT, 5, 5, 315, 15,
				 hwnd, (HMENU) 99, hInstance, NULL);
      /* set the font of the window */
      if(newFont)
         SendMessage (hwnLButton, WM_SETFONT, (WPARAM) newFont, MAKELPARAM(FALSE, 0));
      ShowWindow (hwnLButton, SW_SHOWNORMAL);
      UpdateWindow (hwnLButton);
      
      hwnRButton = CreateWindow ("STATIC",
				 (applyToSelection ||  PalMessageSet1)
				 ? TtaGetMessage (LIB, TMSG_BUTTON_2)
				 : TtaGetMessage (LIB, TMSG_CPBUTTON_2), 
				 WS_CHILD | WS_VISIBLE | SS_LEFT, 5, 20, 315, 20,
				 hwnd, (HMENU) 101, hInstance, NULL);
      /* set the font of the window */
      if(newFont)
         SendMessage (hwnRButton, WM_SETFONT, (WPARAM) newFont, MAKELPARAM(FALSE, 0));
      ShowWindow (hwnRButton, SW_SHOWNORMAL);
      UpdateWindow (hwnRButton);
      
      hwnDefaultColors = CreateWindow ("STATIC", TtaGetMessage (LIB, TMSG_STD_COLORS), 
				       WS_CHILD | WS_VISIBLE | SS_CENTER | WS_BORDER, 0, 45, 320, 15,
				       hwnd, (HMENU) DEFAULTCOLOR, hInstance, NULL);
      /* set the font of the window */
      if(newFont)
         SendMessage (hwnDefaultColors, WM_SETFONT, (WPARAM) newFont, MAKELPARAM(FALSE, 0));
      ShowWindow (hwnDefaultColors, SW_SHOWNORMAL);
      UpdateWindow (hwnDefaultColors);
      
      doneButton = CreateWindow ("BUTTON", TtaGetMessage (LIB, TMSG_DONE), 
				 WS_CHILD | BS_PUSHBUTTON | WS_VISIBLE | SS_CENTER,
				 110, 350, 80, 20, hwnd, 
				 (HMENU) _IDDONE_, hInstance, NULL);
      /* set the font of the window */
      if(newFont)
         SendMessage (doneButton, WM_SETFONT, (WPARAM) newFont, MAKELPARAM(FALSE, 0));
      ShowWindow (doneButton, SW_SHOWNORMAL);
      UpdateWindow (doneButton);
      break;
    
    case WM_LBUTTONDOWN:
      yPos = HIWORD (lParam);
      if (HIWORD (lParam) >= 40 && HIWORD (lParam) <= 345)
	{
	  ColorsPress (1, LOWORD (lParam), HIWORD (lParam));
	  SelectANewFgColor (hwnd);
	  if (!applyToSelection)
	    {
	      DeleteObject (TtCmap);
	      TtCmap = 0;
	      DestroyWindow (hwnd);
	    }
	} 
      break;
      
    case WM_MBUTTONDOWN:
    case WM_RBUTTONDOWN:
      ColorsPress (2, LOWORD (lParam), HIWORD (lParam));
      SelectANewBgColor (hwnd);
      if (!applyToSelection)
	{
	  DeleteObject (TtCmap);
	  TtCmap = 0;
	  DestroyWindow (hwnd);
	}
      break;
      
    case WM_PAINT:
      CheckTtCmap ();
      if (TtCmap) 
	{
	  hdc = BeginPaint (hwnd, &ps);
	  SelectPalette (hdc, TtCmap, FALSE);
	  nbPalEntries = RealizePalette (hdc);
	  for (y = 0; y < VERT_DIV; y++)
	    for (x = 0; x < HORIZ_DIV; x++)
	      {
		hBrush = CreateSolidBrush (PALETTEINDEX (x + y * HORIZ_DIV));
		hOldBrush = SelectObject (hdc, hBrush);
		Rectangle (hdc, x * cxBlock, (y * cyBlock) + 60,(x + 1) * cxBlock, (y + 1) * cyBlock + 60);
		SelectObject (hdc, hOldBrush);
		DeleteObject (hBrush);
		hBrush = (HBRUSH) 0;
	      }
	  /* display current colors */
	  LastFg = -1;
	  SelectANewFgColor (hwnd);
	  LastBg = -1;
	  SelectANewBgColor (hwnd);
	  EndPaint (hwnd, &ps);
	  DeleteDC (hdc);
	} 
      break;
	
    case WM_COMMAND:
      switch (LOWORD (wParam))
	{
	case _IDDONE_:
	  DeleteObject (TtCmap);
	  TtCmap = 0;
	  DestroyWindow (hwnd);
	  break;
	}
    case WM_CLOSE:
    case WM_DESTROY :
      HwndColorPal = (HWND) NULL;
      PostQuitMessage (0);
      break;
    }
  return DefWindowProc (hwnd, iMsg, wParam, lParam);
}
#else /* _WINDOWS */

/*----------------------------------------------------------------------
   EndPalette
   Ends the display of the palette.
  ----------------------------------------------------------------------*/
static void EndPalette (ThotWidget w, int index, caddr_t call_d)
{
#ifndef _GTK
  XtPopdown (Color_Palette);
#else /* !_GTK */
  gtk_widget_hide (Color_Palette);
#endif /* _GTK */
}

/*----------------------------------------------------------------------
   ColorsEvent
   handles the X events of the palette.
  ----------------------------------------------------------------------*/
void ColorsEvent (ThotEvent * event)
{
#ifndef _GTK
  if (event->xbutton.window == Color_Window && Color_Window != 0)
    {
      if (event->type == Expose)
	ColorsExpose ();
      else if (event->type == ButtonPress)
	ColorsPress (event->xbutton.button, event->xbutton.x, event->xbutton.y);
    }
#endif /* !_GTK */
}
#endif /* !_WINDOWS */

#ifdef _GTK
/*----------------------------------------------------------------------
   CreateColorSelectionGTK
   creates the color selection palette (used to select a specifique color)
  ----------------------------------------------------------------------*/
void CreateColorSelectionGTK (int x, int y)
{
  ThotWidget palette;
  ThotWidget tmpw_vbox;
  ThotWidget tmpw_colsel;
  ThotWidget tmpw_draw;
  ThotWidget tmpw_label;
  ThotWidget tmpw_frame;
  ThotWidget tmpw_hbox;
  ThotWidget tmpw_button;
  
  
  /* create the window */
  palette = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (Color_Palette), TtaGetMessage (LIB, TMSG_COLORS));
  ConnectSignalGTK (Color_Palette,
		    "delete_event",
		    GTK_SIGNAL_FUNC(gtk_true),
		    (gpointer)NULL);
  
  /* create the vbox whitch containe all the widget */
  tmpw_vbox = gtk_vbox_new (FALSE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (tmpw_vbox), 5);
  gtk_container_add (GTK_CONTAINER (Color_Palette), tmpw_vbox);
  /* create the frame for decoration */
  tmpw_frame = gtk_frame_new (NULL);
  gtk_box_pack_start (GTK_BOX (tmpw_vbox), tmpw_frame, TRUE, TRUE, 0);
  
  /* create the color selection */
  tmpw_colsel = gtk_color_selection_new ();
  /* set a high refresh rate (for powerfull computers) */
  gtk_color_selection_set_update_policy (GTK_COLOR_SELECTION (tmpw_colsel), GTK_UPDATE_CONTINUOUS);
  /* no opacity */
  gtk_color_selection_set_opacity (GTK_COLOR_SELECTION (tmpw_colsel), FALSE);
  /* set the curent edited color */
  /* gtk_color_selection_set_color (GTK_COLOR_SELECTION (tmpw_colsel), 0);*/
  /* to get the curent color, use: gtk_colorselection_get_color (colorsel, *color) */
  /* to connect a callback when the curent color change use the following code :*/
  /*   ConnectSignalGTK (tmpw_colsel,
       "color_changed",
       GTK_SIGNAL_FUNC(callback_du_changement_de_couleur),
	(gpointer)tmpw_colsel);*/

  gtk_container_add (GTK_CONTAINER (tmpw_frame), tmpw_colsel);
  
  /* create the hbox for buttons */
  tmpw_hbox = gtk_hbox_new (FALSE, 0);
  gtk_box_pack_start (GTK_BOX (tmpw_vbox), tmpw_hbox, FALSE, FALSE, 0);
  
  /* add the buttons */
  /*  tmpw_button = gtk_button_new_with_label (TtaGetMessage (LIB, TMSG_DONE));
  GTK_WIDGET_SET_FLAGS (GTK_WIDGET(tmpw_button), GTK_CAN_DEFAULT);
  gtk_widget_grab_default (GTK_WIDGET(tmpw_button));
  gtk_box_pack_start (GTK_BOX (tmpw_hbox), tmpw_button, FALSE, FALSE, 0);
  ConnectSignalGTK (tmpw_button,
		    "clicked",
		    GTK_SIGNAL_FUNC (EndPalette),
		    (gpointer)NULL);*/
  /* show it */
  gtk_widget_show_all(GTK_WIDGET(palette));
  /* move the colorpalette at the good position */
  gtk_widget_set_uposition (palette, x, y);
}
#endif /* _GTK */

/*----------------------------------------------------------------------
   ThotCreatePalette
   creates the color palette. (user choose colors into a predefined palette)
  ----------------------------------------------------------------------*/
ThotBool ThotCreatePalette (int x, int y)
{
#ifdef _WINDOWS
   WNDCLASSEX  wndThotPaletteClass;
   int   frame;
   MSG         msg;

   LastBg = -1;
   LastFg = -1;

   if (HwndColorPal == NULL)
     {
	   if (!IsRegistered)
	   {
		 IsRegistered = TRUE;
         wndThotPaletteClass.style         = CS_HREDRAW | CS_VREDRAW;
         wndThotPaletteClass.lpfnWndProc   = ThotColorPaletteWndProc;
         wndThotPaletteClass.cbClsExtra    = 0;
         wndThotPaletteClass.cbWndExtra    = 0;
         wndThotPaletteClass.hInstance     = hInstance;
         wndThotPaletteClass.hIcon         = LoadIcon (NULL, iconID);
         wndThotPaletteClass.hCursor       = LoadCursor (NULL, IDC_ARROW);
         wndThotPaletteClass.hbrBackground = (HBRUSH) GetStockObject (LTGRAY_BRUSH);
         wndThotPaletteClass.lpszMenuName  = NULL;
         wndThotPaletteClass.lpszClassName = "ThotColorPalette";
         wndThotPaletteClass.cbSize        = sizeof(WNDCLASSEX);
         wndThotPaletteClass.hIconSm       = LoadIcon (hInstance, iconID);
         if (!RegisterClassEx (&wndThotPaletteClass))
         return FALSE;
	   }
       HwndColorPal = CreateWindow ("ThotColorPalette", TtaGetMessage (LIB, TMSG_COLORS),
				    DS_MODALFRAME | WS_POPUP | 
				    WS_VISIBLE | WS_CAPTION | WS_SYSMENU,
				    ClickX, ClickY,
				    320, 400,
				    NULL, NULL, hInstance, NULL);
   
     }
   ShowWindow (HwndColorPal, SW_SHOWNORMAL);
   UpdateWindow (HwndColorPal);
   while (GetMessage (&msg, NULL, 0, 0))
     {
       frame = GetFrameNumber (msg.hwnd);
       TranslateMessage (&msg);
       DispatchMessage (&msg);
     }
#else  /* _WINDOWS */
#ifndef _GTK
   int                 n;
   int                 width, height;
   Arg                 args[MAX_ARGS];
   XmString            title_string;
   XmFontList          xfont;
   XGCValues           GCmodel;
   char                string[10];
   ThotWidget          w;
   ThotWidget          row;
   ThotWidget          frame;

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
   XtSetArg (args[n], XmNborderColor, BgMenu_Color);
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
   XtSetArg (args[n], XmNborderColor, BgMenu_Color);
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
   XtSetArg (args[n], XmNborderColor, BgMenu_Color);
   n++;
   XtSetArg (args[n], XmNforeground, FgMenu_Color);
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
   XtSetArg (args[n], XmNborderColor, BgMenu_Color);
   n++;
   XtSetArg (args[n], XmNforeground, FgMenu_Color);
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
   XtSetArg (args[n], XmNborderColor, BgMenu_Color);
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
   XtSetArg (args[n], XmNborderColor, BgMenu_Color);
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
   XtSetArg (args[n], XmNborderColor, BgMenu_Color);
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
   GCmodel.foreground = FgMenu_Color;
   GCmodel.background = BgMenu_Color;
   GCkey = XCreateGC (TtDisplay, TtRootWindow, GCForeground | GCBackground | GCFunction, &GCmodel);

/*** Cree le bouton Quit ***/
   n = 0;
   XtSetArg (args[n], XmNbackground, BgMenu_Color);
   n++;
   XtSetArg (args[n], XmNbottomShadowColor, BgMenu_Color);
   n++;
   XtSetArg (args[n], XmNforeground, FgMenu_Color);
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
#else /* !_GTK */
   /* create the color selection in GTK, it's possible to add some elements by adding it into the tmpw_vbox container */
   int                 width, height;
   ThotWidget tmpw_vbox;
   ThotWidget tmpw_colsel;
   ThotWidget tmpw_draw;
   ThotWidget tmpw_label;
   ThotWidget tmpw_frame;
   ThotWidget tmpw_hbox;
   ThotWidget tmpw_button;
 
   if (SmallFontDialogue == NULL)
     SmallFontDialogue = ReadFont ('L', 'H', 0, 9, UnPoint);

   /* create the window */
   Color_Palette = gtk_window_new (GTK_WINDOW_TOPLEVEL);
   gtk_window_set_title (GTK_WINDOW (Color_Palette), TtaGetMessage (LIB, TMSG_COLORS));
   ConnectSignalGTK (Color_Palette,
		     "delete_event",
		     GTK_SIGNAL_FUNC(gtk_true),
		     (gpointer)NULL);

   /* create the vbox whitch containe all the widget */
   tmpw_vbox = gtk_vbox_new (FALSE, 0);
   gtk_container_set_border_width (GTK_CONTAINER (tmpw_vbox), 5);
   gtk_container_add (GTK_CONTAINER (Color_Palette), tmpw_vbox);

   /* put the labels */
   tmpw_label = gtk_label_new (TtaGetMessage (LIB, TMSG_BUTTON_1));
   gtk_box_pack_start (GTK_BOX (tmpw_vbox), tmpw_label, FALSE, FALSE, 0);
   tmpw_label = gtk_label_new (TtaGetMessage (LIB, TMSG_BUTTON_2));
   gtk_box_pack_start (GTK_BOX (tmpw_vbox), tmpw_label, FALSE, FALSE, 0);
   
   /* Evalue la largeur et la hauteur de la palette */
   if (TtWDepth == 1)
      /* Affiche le nom des couleurs sur un ecran N&B */
      width = CharacterWidth ('m', FontDialogue) * 12 * COLORS_COL;
   else
      /* Affiche les couleurs sur un ecran couleur */
      width = CharacterWidth ('m', FontDialogue) * 4 * COLORS_COL;
   height = ((NumberOfColors () + COLORS_COL - 1) / COLORS_COL + 1) * FontHeight (FontDialogue);

   /* create the frame for decoration */
   tmpw_frame = gtk_frame_new (NULL);
   gtk_box_pack_start (GTK_BOX (tmpw_vbox), tmpw_frame, TRUE, TRUE, 0);

   /* put the drawing area */
   tmpw_draw = gtk_drawing_area_new ();
   gtk_drawing_area_size (tmpw_draw, width, height);
   gtk_container_add (GTK_CONTAINER (tmpw_frame), tmpw_draw);
   gtk_widget_set_events (GTK_WIDGET (tmpw_draw),  
			  GDK_BUTTON_PRESS_MASK	| GDK_EXPOSURE_MASK
			  );
   ConnectSignalGTK (tmpw_draw,
		     "expose_event",
		     GTK_SIGNAL_FUNC (ColorsExposeGTK),
		     (gpointer)tmpw_draw);
   ConnectSignalGTK (tmpw_draw,
		     "button_press_event",
		     GTK_SIGNAL_FUNC(ColorsPressGTK),
		     (gpointer)tmpw_draw);

   /* create the hbox for buttons */
   tmpw_hbox = gtk_hbox_new (FALSE, 0);
   gtk_box_pack_start (GTK_BOX (tmpw_vbox), tmpw_hbox, FALSE, FALSE, 0);

   /* add the quit button */
   tmpw_button = gtk_button_new_with_label (TtaGetMessage (LIB, TMSG_DONE));
   GTK_WIDGET_SET_FLAGS (GTK_WIDGET(tmpw_button), GTK_CAN_DEFAULT);
   gtk_widget_grab_default (GTK_WIDGET(tmpw_button));
   gtk_box_pack_start (GTK_BOX (tmpw_hbox), tmpw_button, FALSE, FALSE, 0);
   ConnectSignalGTK (tmpw_button,
		     "clicked",
		     GTK_SIGNAL_FUNC (EndPalette),
		     (gpointer)NULL);
   /* show it */
   gtk_widget_show_all(GTK_WIDGET(Color_Palette));
   /* move the colorpalette at the good position */
   gtk_widget_set_uposition (Color_Palette, x, y);

   Color_Window = GTK_WIDGET(tmpw_draw)->window;
   /* pas de selection precedente */
   LastBg = -1;
   LastFg = -1;
#endif /* _GTK */
#endif /* _WINDOWS */
   return TRUE;
}


/*----------------------------------------------------------------------
   TtcChangeColors
   the user wants to modify the colors (specific presentation) for the
   view "view" of document pDoc (if Assoc = FALSE) or the elements associated
   of number "view" (if Assoc = TRUE).
   Initializes and activates the corresponding menu.
  ----------------------------------------------------------------------*/
void TtcChangeColors (Document document, View view)
{
   PtrDocument         pSelDoc;
   PtrElement          pFirstSel, pLastSel;
   PtrAbstractBox      pAb;
   PtrDocument         pDoc;
   int                 firstChar, lastChar;
   int                 KbX, KbY;
   ThotBool            selectionOK;

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
	pFirstSel = pDoc->DocDocElement;
	selectionOK = TRUE;
     }

   if (selectionOK && pSelDoc == pDoc && NumberOfColors () > 0)
      /* il y a effectivement des couleurs disponibles */
     {
	/* Cree la palette si elle n'existe pas */
	if (Color_Window == 0)
	  {	    
	    ConfigKeyboard (&KbX, &KbY);
#ifndef _WINDOWS
	    ThotCreatePalette (KbX, KbY);
#endif /* _WINDOWS */
	  }
#ifndef _WINDOWS
#ifndef _GTK
	else
	  XtPopup (Color_Palette, XtGrabNonexclusive);
#else /* !_GTK */
	else
	  {
	    gtk_widget_show_all (GTK_WIDGET(Color_Palette));
	    gdk_window_raise (GTK_WIDGET(Color_Palette)->window);
	  }
#endif /* _GTK */
#endif /* _WINDOWS */
	
	/* recherche le pave concerne */
	if (view > 100)
	  pAb = AbsBoxOfEl (pFirstSel, 1);
	else
	  pAb = AbsBoxOfEl (pFirstSel, view);
	
	if (pAb != NULL)
	  ThotSelectPalette (pAb->AbBackground, pAb->AbForeground);
#ifdef _WINDOWS
	if (HwndColorPal == (HWND) 0) 
	  ThotCreatePalette (KbX, KbY);
	else 
	  SetForegroundWindow (HwndColorPal);
#endif /* _WINDOWS */
     }
}

/*----------------------------------------------------------------------
   TtcGetPaletteColors
   Displays the color palette and returns the foreground and background
   colors chosen by the user.
  ----------------------------------------------------------------------*/
void TtcGetPaletteColors (int *fg, int *bg, ThotBool palType)
{ 
#ifdef _WINDOWS
    PalMessageSet1 = palType;
    FgColor = BgColor = -1;
    applyToSelection = FALSE;
    ThotCreatePalette (200, 200);
    *fg = FgColor;
    *bg = BgColor;
    applyToSelection = TRUE;
#endif /* _WINDOWS */
}
