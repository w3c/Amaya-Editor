/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2004
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
#ifdef _WINGUI
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
#endif /* _WINGUI */

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
#ifdef _GTK
static ThotWidget   Color_Animation_Popup = NULL;
static ThotWindow   Color_Window2 = 0;
static ThotWidget   Color_Palette_Extended = NULL;
static ThotBool     ApplyFg = FALSE;
static ThotBool     ApplyBg = FALSE;
#endif /* _GTK */
#ifdef _WINGUI
static ThotGC       GCkey;
#endif /* _WINGUI */
static int          LastBg;
static int          LastFg;
static int          FgColor, BgColor;
static ThotBool     applyToSelection = TRUE;

#ifdef _WINGUI
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
#ifdef _GTK
   BgColor = bground;
   FgColor = fground;

   /* redraw the palette */
   gtk_widget_queue_draw (GTK_WIDGET(Color_Palette));
   return;
#endif /* _GTK */
#ifdef _WINGUI
   BgColor = bground;
   FgColor = fground;
#endif /* _WINGUI */
}

#ifdef _WINGUI
/*----------------------------------------------------------------------
   KillPalette
   kills the palette.
  ----------------------------------------------------------------------*/
static void KillPalette (ThotWidget w, int index, caddr_t call_d)
{
   Color_Palette = 0;
   Color_Window = 0;
}
#endif /* _WINGUI */

#ifdef _GTK
/*----------------------------------------------------------------------
  ColorExposeGTK redraw the color of the button color
  data is the id color
  widget is the drawing area
  ----------------------------------------------------------------------*/
gboolean ColorsExposeGTK (GtkWidget *widget, GdkEventExpose *ev, gpointer data)
{
  int color = (int) data;
  GdkWindow *window = GTK_WIDGET(widget)->window;
  gdk_rgb_gc_set_foreground (TtLineGC, ColorPixel (color));
  gdk_draw_rectangle (window, TtLineGC, TRUE, 0, 0, 20, 20);

  /*  fground = LastFg;
  bground = LastBg;
  LastFg = -1;
  LastBg = -1;*/
  /*  ThotSelectPalette (bground, fground);*/
  return TRUE;
}

/*----------------------------------------------------------------------
  ColorExposeFgBgGTK redraw the color of the foreground and background color
  data is the @ id color
  widget is the drawing area
  ----------------------------------------------------------------------*/
gboolean ColorsExposeFgBgGTK (GtkWidget *widget, GdkEventExpose *ev, gpointer data)
{
  int color = *((int *)data);
  GdkWindow *window = GTK_WIDGET(widget)->window;
  if (color==-1) color=0;
  gdk_rgb_gc_set_foreground (TtLineGC, ColorPixel (color));
  gdk_draw_rectangle (window, TtLineGC, TRUE, 0, 0, 40, 40);
  return TRUE;
}

/*----------------------------------------------------------------------
  2 callback to synchronize toggle selection button
  foreground color
  background color
  ----------------------------------------------------------------------*/
void ButtonFgToggledGTK (GtkWidget *w, GdkEventButton *event, gpointer data)
{
  GtkToggleButton * fgbutton;
  GtkToggleButton * bgbutton;
  fgbutton = (GtkToggleButton *)gtk_object_get_data (GTK_OBJECT(w), "FgToggle");
  bgbutton = (GtkToggleButton *)gtk_object_get_data (GTK_OBJECT(w), "BgToggle");
  
  ApplyFg = TRUE;
  ApplyBg = FALSE;
  gtk_toggle_button_set_active (bgbutton, FALSE);
  gtk_toggle_button_set_active (fgbutton, TRUE);
}

void ButtonBgToggledGTK (GtkWidget *w, GdkEventButton *event, gpointer data)
{


  GtkToggleButton * fgbutton;
  GtkToggleButton * bgbutton;
  fgbutton = (GtkToggleButton *)gtk_object_get_data (GTK_OBJECT(w), "FgToggle");
  bgbutton = (GtkToggleButton *)gtk_object_get_data (GTK_OBJECT(w), "BgToggle");

  ApplyBg = TRUE;
  ApplyFg = FALSE;
  gtk_toggle_button_set_active (fgbutton, FALSE); 
  gtk_toggle_button_set_active (bgbutton, TRUE);
}

/*----------------------------------------------------------------------
   ColorsPress
   handles a click on the color palette
  ----------------------------------------------------------------------*/
gboolean ColorsPressGTK (GtkWidget *widget, gpointer data)
{
  int color = (int)data;
  
  if (ApplyFg)
    {
      /* on assigne le foreground */
      FgColor = color;
      if (applyToSelection)
	{
	  /*ModifyColor (color, FALSE);*/
	  ThotSelectPalette (BgColor, color);
	}
    }

   if (ApplyBg)
    {
      /* on assigne le background */
      BgColor = color;
      if (applyToSelection)
	{
	  /*ModifyColor (color, TRUE);*/
	  ThotSelectPalette (color, FgColor);
	}
      
    }
  return TRUE;
}

/*----------------------------------------------------------------------
   Apply the current selected fg and bg color to the current selected element
   FgColor is the foreground id color
   BgColor is the background id color
  ----------------------------------------------------------------------*/
gboolean ApplyColorToSelectedElementGTK (GtkWidget *widget, gpointer data)
{
  if (applyToSelection)
    {
      ModifyColor (FgColor, FALSE);
      ModifyColor (BgColor, TRUE);
    }
  return TRUE;
}

/*----------------------------------------------------------------------
   Apply the selected color to the fg or bg widget
   FgColor is the foreground id color
   BgColor is the background id color
   data is the color_selection reference
  ----------------------------------------------------------------------*/
gboolean ApplyExtendedColorGTK (GtkWidget *widget, gpointer data)
{
  gdouble couleur[4];
  unsigned short red;
  unsigned short green;
  unsigned short blue;
  int color_id;
  gtk_color_selection_get_color (GTK_COLOR_SELECTION (data), couleur);
  red = (unsigned short) couleur[0] * 255;
  green = (unsigned short) couleur[1] *255;
  blue = (unsigned short) couleur[2] * 255;
  
  color_id = TtaGetThotColor (red, green, blue);
  if (ApplyBg)
    {
      ThotSelectPalette (color_id, FgColor);
    }
  if (ApplyFg)
    {
      ThotSelectPalette (BgColor, color_id);
    }
  return TRUE;
}


/*----------------------------------------------------------------------
   Get the selected element bg and fg colors
   FgColor is the foreground id color
   BgColor is the background id color
  ----------------------------------------------------------------------*/
gboolean GetSelectedElementColorGTK (GtkWidget *widget, gpointer data)
{
  PtrDocument         pSelDoc;
  PtrElement          pFirstSel, pLastSel;
  int                 firstChar, lastChar;
  ThotBool            selectionOK;
  
  selectionOK = GetCurrentSelection (&pSelDoc, &pFirstSel, &pLastSel, &firstChar, &lastChar);
  if (!selectionOK) return TRUE;
  
  BgColor = pFirstSel->ElAbstractBox[0]->AbBackground;
  FgColor = pFirstSel->ElAbstractBox[0]->AbForeground;
  
  ThotSelectPalette (BgColor, FgColor);
  return TRUE;
}
#endif /* _GTK */

#ifdef _WINGUI
/*----------------------------------------------------------------------
   ColorsExpose
   redisplays a color keyboard.
  ----------------------------------------------------------------------*/
static void ColorsExpose ()
{
   int                 max, y, w, h;
   int                 wcase, hcase;

   if (TtWDepth == 1)
      /* Affiche le nom des couleurs sur un ecran N&B */
      wcase = CharacterWidth (109, DialogFont) * 12;
   else
      /* Affiche les couleurs sur un ecran couleur */
      wcase = CharacterWidth (109, DialogFont) * 4;

   w = wcase * COLORS_COL;
   hcase = FontHeight (DialogFont);
   max = NumberOfColors ();
   y = hcase;
   h = (max + COLORS_COL - 1) / COLORS_COL * hcase + y;
}


/*----------------------------------------------------------------------
   ColorsPress
   handles a click on the color palette
  ----------------------------------------------------------------------*/
static void ColorsPress (int button, int x, int y)
{
  int                 color, li, co;

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

  switch (iMsg)
    {
    case WM_CREATE:
      /* get the default GUI font */
      cxBlock = 39;
      cyBlock = 15;
      hwnLButton = CreateWindow ("STATIC",
				 (applyToSelection || PalMessageSet1)
				 ? TtaGetMessage (LIB, TMSG_BUTTON_1)
				 : TtaGetMessage (LIB, TMSG_CPBUTTON_1), 
				 WS_CHILD | WS_VISIBLE | SS_LEFT, 5, 5, 315, 15,
				 hwnd, (HMENU) 99, hInstance, NULL);
      /* set the font of the window */
      WIN_SetDialogfont (hwnLButton);
      ShowWindow (hwnLButton, SW_SHOWNORMAL);
      UpdateWindow (hwnLButton);
      
      hwnRButton = CreateWindow ("STATIC",
				 (applyToSelection ||  PalMessageSet1)
				 ? TtaGetMessage (LIB, TMSG_BUTTON_2)
				 : TtaGetMessage (LIB, TMSG_CPBUTTON_2), 
				 WS_CHILD | WS_VISIBLE | SS_LEFT, 5, 20, 315, 20,
				 hwnd, (HMENU) 101, hInstance, NULL);
      /* set the font of the window */
      WIN_SetDialogfont (hwnRButton);
      ShowWindow (hwnRButton, SW_SHOWNORMAL);
      UpdateWindow (hwnRButton);
      
      hwnDefaultColors = CreateWindow ("STATIC", TtaGetMessage (LIB, TMSG_STD_COLORS), 
				       WS_CHILD | WS_VISIBLE | SS_CENTER | WS_BORDER, 0, 45, 320, 15,
				       hwnd, (HMENU) DEFAULTCOLOR, hInstance, NULL);
      /* set the font of the window */
      WIN_SetDialogfont (hwnDefaultColors);
      ShowWindow (hwnDefaultColors, SW_SHOWNORMAL);
      UpdateWindow (hwnDefaultColors);
      
      doneButton = CreateWindow ("BUTTON", TtaGetMessage (LIB, TMSG_DONE), 
				 WS_CHILD | BS_PUSHBUTTON | WS_VISIBLE | SS_CENTER,
				 110, 350, 80, 20, hwnd, 
				 (HMENU) _IDDONE_, hInstance, NULL);
      /* set the font of the window */
      WIN_SetDialogfont (doneButton);
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
#endif /* _WINGUI */

#ifdef _GTK
/*----------------------------------------------------------------------
   EndPalette
   Ends the display of the palette.
  ----------------------------------------------------------------------*/
static void EndPalette (ThotWidget w, int index, caddr_t call_d)
{
  if (Color_Animation_Popup)
    {
      gtk_widget_hide (Color_Animation_Popup);      
      Color_Animation_Popup = 0;      
    }  
   else
     gtk_widget_hide (Color_Palette);
}


/*----------------------------------------------------------------------
   CreateColorSelectionGTK
   creates the color selection palette (used to select a specifique color)
  ----------------------------------------------------------------------*/
gboolean CreateExtendedColorSelectionGTK (GtkWidget *widget, gpointer data)
{
  ThotWidget tmpw_vbox;
  ThotWidget tmpw_colsel;
  ThotWidget tmpw_frame;
  ThotWidget tmpw_hbox;
  ThotWidget tmpw_button;

  if (Color_Palette_Extended)
    {
      gtk_widget_show_all (Color_Palette_Extended);
      return TRUE;
    }

  /* create the window */
  Color_Palette_Extended = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_position (GTK_WINDOW (Color_Palette_Extended), GTK_WIN_POS_MOUSE);
  gtk_window_set_title (GTK_WINDOW (Color_Palette_Extended), TtaGetMessage (LIB, TMSG_COLORS));
  ConnectSignalGTK (GTK_OBJECT (Color_Palette_Extended),
		    "delete_event",
		    GTK_SIGNAL_FUNC(gtk_true),
		    (gpointer)NULL);
  
  /* create the vbox whitch containe all the widget */
  tmpw_vbox = gtk_vbox_new (FALSE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (tmpw_vbox), 5);
  gtk_container_add (GTK_CONTAINER (Color_Palette_Extended), tmpw_vbox);
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
  /*   ConnectSignalGTK (GTK_OBJECT (tmpw_colsel),
       "color_changed",
       GTK_SIGNAL_FUNC(callback_du_changement_de_couleur),
	(gpointer)tmpw_colsel);*/
  gtk_container_add (GTK_CONTAINER (tmpw_frame), tmpw_colsel);
  
  /* create the hbox for buttons */
  tmpw_hbox = gtk_hbox_new (FALSE, 0);
  gtk_box_pack_start (GTK_BOX (tmpw_vbox), tmpw_hbox, FALSE, FALSE, 0);
  /* add the buttons */
  tmpw_button = gtk_button_new_with_label (TtaGetMessage (LIB, TMSG_DONE));
  GTK_WIDGET_SET_FLAGS (GTK_WIDGET(tmpw_button), GTK_CAN_DEFAULT);
  gtk_widget_grab_default (GTK_WIDGET(tmpw_button));
  gtk_box_pack_start (GTK_BOX (tmpw_hbox), tmpw_button, FALSE, FALSE, 0);
  ConnectSignalGTK (GTK_OBJECT (tmpw_button),
		    "clicked",
		    GTK_SIGNAL_FUNC (CloseExtendedColorSelectionGTK),
		    (gpointer)NULL);
  tmpw_button = gtk_button_new_with_label (TtaGetMessage (LIB, TMSG_APPLY));
  GTK_WIDGET_SET_FLAGS (GTK_WIDGET(tmpw_button), GTK_CAN_DEFAULT);
  gtk_widget_grab_default (GTK_WIDGET(tmpw_button));
  gtk_box_pack_start (GTK_BOX (tmpw_hbox), tmpw_button, FALSE, FALSE, 0);
  ConnectSignalGTK (GTK_OBJECT (tmpw_button),
		    "clicked",
		    GTK_SIGNAL_FUNC (ApplyExtendedColorGTK),
		    (gpointer)tmpw_colsel);

  /* show it */
  gtk_widget_show_all(GTK_WIDGET(Color_Palette_Extended));
  return TRUE;
}

/*----------------------------------------------------------------------
  Close Extended Color Selector
  ----------------------------------------------------------------------*/
gboolean CloseExtendedColorSelectionGTK (GtkWidget *widget, gpointer data)
{
  gtk_widget_hide (Color_Palette_Extended);
  return TRUE;
}

/*----------------------------------------------------------------------
  Close Extended Color Selector
  ----------------------------------------------------------------------*/
gboolean Color_Dialogue_Quit (GtkWidget *widget, gpointer data)
{
  Color_Window = 0;  
  return TRUE;
}
#endif /* _GTK */

/*----------------------------------------------------------------------
   ThotCreatePalette
   creates the color palette. (user choose colors into a predefined palette)
  ----------------------------------------------------------------------*/
ThotBool ThotCreatePalette (int x, int y)
{
#ifdef _WINGUI
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
#endif /* _WINGUI */
#ifdef _GTK
   /* create the color selection in GTK, it's possible to add some elements by adding it into the tmpw_vbox container */
  GtkWidget *vbox1;
  GtkWidget *vbox2;
  GtkWidget *frame3;
  GtkWidget *frame2;
  GtkWidget *vbox4;
  GtkWidget *hbox4;
  GtkWidget *vbox5 = None;
  GtkWidget *button4;
  GtkWidget *hbox1;
  GtkWidget *button1;
  GtkWidget *button2;
  GtkWidget *button3;
  GtkWidget *color_draw;
  GtkWidget *color_button;
  GtkWidget *fixed1;
  GtkWidget *bg_color_draw;
  GtkWidget *fg_color_draw;
  int i, j;

  Color_Palette = gtk_window_new (GTK_WINDOW_TOPLEVEL);

  gtk_window_set_title (GTK_WINDOW (Color_Palette), TtaGetMessage (LIB, TMSG_COLORS));
  gtk_window_set_position (GTK_WINDOW (Color_Palette), GTK_WIN_POS_MOUSE);
  ConnectSignalGTK (GTK_OBJECT (Color_Palette),
		    "delete_event",
		    GTK_SIGNAL_FUNC(Color_Dialogue_Quit),
		    (gpointer)NULL);

  vbox1 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox1);
  gtk_container_add (GTK_CONTAINER (Color_Palette), vbox1);

  vbox2 = gtk_vbox_new (FALSE, 2);
  gtk_widget_show (vbox2);
  gtk_container_add (GTK_CONTAINER (vbox1), vbox2);


  /******** The foreground and background zone *********/
  frame3 = gtk_frame_new ("");
  gtk_widget_show (frame3);
  gtk_box_pack_start (GTK_BOX (vbox2), frame3, TRUE, TRUE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (frame3), 4);
  fixed1 = gtk_fixed_new ();
  gtk_widget_show (fixed1);
  gtk_container_add (GTK_CONTAINER (frame3), fixed1);

  /* background */
  bg_color_draw = gtk_toggle_button_new();
  gtk_widget_show (bg_color_draw);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (bg_color_draw), FALSE);
  ApplyBg = FALSE;
  gtk_container_set_border_width (GTK_CONTAINER (bg_color_draw), 5);
  gtk_fixed_put (GTK_FIXED (fixed1), bg_color_draw, 40, 40);

  /* foreground */
  fg_color_draw = gtk_toggle_button_new();
  gtk_widget_show (fg_color_draw);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (fg_color_draw), TRUE);
  ApplyFg = TRUE;
  gtk_container_set_border_width (GTK_CONTAINER (fg_color_draw), 5);
  gtk_fixed_put (GTK_FIXED (fixed1), fg_color_draw, 10, 10);

  /* drawing area background */
  color_draw = gtk_drawing_area_new ();
  gtk_drawing_area_size (GTK_DRAWING_AREA (color_draw), 40, 40);
  gtk_container_add (GTK_CONTAINER (bg_color_draw), color_draw);
  /* connecte the data to the drawing area */
  gtk_object_set_data (GTK_OBJECT(color_draw), "FgToggle", (gpointer)fg_color_draw);
  gtk_object_set_data (GTK_OBJECT(color_draw), "BgToggle", (gpointer)bg_color_draw);
  gtk_widget_set_events (GTK_WIDGET (color_draw), GDK_EXPOSURE_MASK | GDK_BUTTON_PRESS_MASK);
  /* connecte the expose event to redraw the good color */
  ConnectSignalGTK (GTK_OBJECT (color_draw),
		    "expose_event",
		    GTK_SIGNAL_FUNC (ColorsExposeFgBgGTK),
		    (gpointer)&BgColor);
  ConnectSignalGTK (GTK_OBJECT (color_draw),
		    "button_press_event",
		    GTK_SIGNAL_FUNC (ButtonBgToggledGTK),
		    (gpointer)NULL);

  /* drawing area foreground */
  color_draw = gtk_drawing_area_new ();
  gtk_drawing_area_size (GTK_DRAWING_AREA (color_draw), 40, 40);
  gtk_container_add (GTK_CONTAINER (fg_color_draw), color_draw);
  /* connecte the data to the drawing area */
  gtk_object_set_data (GTK_OBJECT(color_draw), "FgToggle", (gpointer)fg_color_draw);
  gtk_object_set_data (GTK_OBJECT(color_draw), "BgToggle", (gpointer)bg_color_draw);
  gtk_widget_set_events (GTK_WIDGET (color_draw), GDK_EXPOSURE_MASK | GDK_BUTTON_PRESS_MASK);
  /* connecte the expose event to redraw the good color */
  ConnectSignalGTK (GTK_OBJECT (color_draw),
		    "expose_event",
		    GTK_SIGNAL_FUNC (ColorsExposeFgBgGTK),
		    (gpointer)&FgColor);
  /* connect synchronize callback */
  ConnectSignalGTK (GTK_OBJECT (color_draw),
		    "button_press_event",
		    GTK_SIGNAL_FUNC (ButtonFgToggledGTK),
		    (gpointer)NULL);


  /******** the color palette *******/
  frame2 = gtk_frame_new (TtaGetMessage (LIB, TMSG_CPCOLORBUTTON));
  gtk_widget_show (frame2);
  gtk_box_pack_start (GTK_BOX (vbox2), frame2, TRUE, TRUE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (frame2), 5);

  /* vbox witch contain all the colors */
  vbox4 = gtk_vbox_new (FALSE, 5);
  gtk_widget_show (vbox4);
  gtk_container_add (GTK_CONTAINER (frame2), vbox4);
  gtk_container_set_border_width (GTK_CONTAINER (vbox4), 4);

  /* Default color */
  /* hbox witch contain the default color */
  hbox4 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox4);
  gtk_box_pack_start (GTK_BOX (vbox4), hbox4, TRUE, TRUE, 0);
  /* create a color button (default) */
  color_button = gtk_button_new_with_label (TtaGetMessage (LIB, TMSG_STD_COLORS));
  gtk_widget_show (color_button);
  /* on connecte le callback pour l'activation de la couleur par default */
  /* on passe la couleur -1 */
  ConnectSignalGTK (GTK_OBJECT (color_button),
		    "clicked",
		    GTK_SIGNAL_FUNC(ColorsPressGTK),
		    (gpointer)-1);
  gtk_box_pack_start (GTK_BOX (hbox4), color_button, FALSE, FALSE, 0);

  /* hbox witch contain all the colors columns */
  hbox4 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox4);
  gtk_box_pack_start (GTK_BOX (vbox4), hbox4, TRUE, TRUE, 0);
  j = 0;
  for (i = 1; i<=NumberOfColors (); i++)
    {
      if (i-1 == COLORS_COL*j)
	{
	  /* vbox witch contain one color column */
	  vbox5 = gtk_vbox_new (FALSE, 0);
	  gtk_widget_show (vbox5);
	  gtk_box_pack_start (GTK_BOX (hbox4), vbox5, FALSE, FALSE, 0);
	  j++;
	}
      /* create a color button */
      color_button = gtk_button_new ();
      gtk_widget_show (color_button);
      color_draw = gtk_drawing_area_new ();
      gtk_drawing_area_size (GTK_DRAWING_AREA (color_draw), 20, 20);
      gtk_container_add (GTK_CONTAINER (color_button), color_draw);
      gtk_widget_set_events (GTK_WIDGET (color_draw), GDK_EXPOSURE_MASK);
      /* connecte the expose event to redraw the good color */
      ConnectSignalGTK (GTK_OBJECT (color_draw),
			"expose_event",
			GTK_SIGNAL_FUNC (ColorsExposeGTK),
			(gpointer)i);
      /* connecte the clicked action to cahnge the color */
      ConnectSignalGTK (GTK_OBJECT (color_button),
			"clicked",
			GTK_SIGNAL_FUNC(ColorsPressGTK),
			(gpointer)i);
      gtk_box_pack_start (GTK_BOX (vbox5), GTK_WIDGET(color_button), FALSE, FALSE, 0);
    }


  /******** All the action buttons *********/
  button4 = gtk_button_new_with_label (TtaGetMessage (LIB, TMSG_CPMOREBUTTON));
  gtk_widget_show (button4);
  gtk_box_pack_start (GTK_BOX (vbox4), button4, FALSE, FALSE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (button4), 2);
  ConnectSignalGTK (GTK_OBJECT (button4),
		    "clicked",
		    GTK_SIGNAL_FUNC (CreateExtendedColorSelectionGTK),
		    (gpointer)NULL);

  hbox1 = gtk_hbox_new (TRUE, 0);
  gtk_widget_show (hbox1);
  gtk_box_pack_start (GTK_BOX (vbox1), hbox1, TRUE, TRUE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (hbox1), 5);

  button1 = gtk_button_new_with_label (TtaGetMessage (LIB, TMSG_APPLY));
  gtk_widget_show (button1);
  gtk_box_pack_start (GTK_BOX (hbox1), button1, FALSE, FALSE, 0); 
  gtk_container_set_border_width (GTK_CONTAINER (button1), 2);
  ConnectSignalGTK (GTK_OBJECT (button1),
		    "clicked",
		    GTK_SIGNAL_FUNC (ApplyColorToSelectedElementGTK),
		    (gpointer)NULL);

  button2 = gtk_button_new_with_label (TtaGetMessage (LIB, TMSG_CPGETBUTTON));
  gtk_widget_show (button2);
  gtk_box_pack_start (GTK_BOX (hbox1), button2, FALSE, FALSE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (button2), 2);
  ConnectSignalGTK (GTK_OBJECT (button2),
		    "clicked",
		    GTK_SIGNAL_FUNC (GetSelectedElementColorGTK),
		    (gpointer)NULL);

  button3 = gtk_button_new_with_label (TtaGetMessage (LIB, TMSG_DONE));
  gtk_container_set_border_width (GTK_CONTAINER (button3), 2);
  gtk_widget_show (button3);
  gtk_box_pack_start (GTK_BOX (hbox1), button3, FALSE, FALSE, 0);
  ConnectSignalGTK (GTK_OBJECT (button3),
		    "clicked",
		    GTK_SIGNAL_FUNC (EndPalette),
		    (gpointer)NULL);

  /* show all the dialogues */
  gtk_widget_set_uposition (Color_Palette, x, y);
  gtk_widget_show_all(GTK_WIDGET(Color_Palette));
  Color_Window = GTK_WIDGET (Color_Palette)->window;
  
  /* pas de selection precedente */
  LastBg = -1;
  LastFg = -1;  
#endif /* _GTK */
  return TRUE;
}

#ifdef _GTK
/*----------------------------------------------------------------------
   ThotCreatePaletteModal
   creates the color palette. (user choose colors into a predefined palette)
  ----------------------------------------------------------------------*/
ThotBool ThotCreateModalPalette (int x, int y)
{
  /* create the color selection in GTK, it's possible to add some elements by adding it into the tmpw_vbox container */
  GtkWidget *vbox1;
  GtkWidget *vbox2;
  GtkWidget *frame3;
  GtkWidget *frame2;
  GtkWidget *vbox4;
  GtkWidget *hbox4;
  GtkWidget *vbox5 = None;
  GtkWidget *button4;
  GtkWidget *hbox1;
  GtkWidget *button1;
  GtkWidget *button2;
  GtkWidget *button3;
  GtkWidget *color_draw;
  GtkWidget *color_button;
  GtkWidget *fixed1;
  GtkWidget *bg_color_draw;
  GtkWidget *fg_color_draw;
  int i, j;
  
  Color_Animation_Popup = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  
  gtk_window_set_modal (GTK_WINDOW (Color_Animation_Popup), TRUE);
  
  gtk_window_set_title (GTK_WINDOW (Color_Animation_Popup), TtaGetMessage (LIB, TMSG_COLORS));
  gtk_window_set_position (GTK_WINDOW (Color_Animation_Popup), GTK_WIN_POS_MOUSE);
  ConnectSignalGTK (GTK_OBJECT (Color_Animation_Popup),
		    "delete_event",
		    GTK_SIGNAL_FUNC(gtk_true),
		    (gpointer)NULL);

  vbox1 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox1);
  gtk_container_add (GTK_CONTAINER (Color_Animation_Popup), vbox1);

  vbox2 = gtk_vbox_new (FALSE, 2);
  gtk_widget_show (vbox2);
  gtk_container_add (GTK_CONTAINER (vbox1), vbox2);


  /******** The foreground and background zone *********/
  frame3 = gtk_frame_new ("");
  gtk_widget_show (frame3);
  gtk_box_pack_start (GTK_BOX (vbox2), frame3, TRUE, TRUE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (frame3), 4);
  fixed1 = gtk_fixed_new ();
  gtk_widget_show (fixed1);
  gtk_container_add (GTK_CONTAINER (frame3), fixed1);

  /* background */
  bg_color_draw = gtk_toggle_button_new();
  gtk_widget_show (bg_color_draw);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (bg_color_draw), FALSE);
  ApplyBg = FALSE;
  gtk_container_set_border_width (GTK_CONTAINER (bg_color_draw), 5);
  gtk_fixed_put (GTK_FIXED (fixed1), bg_color_draw, 40, 40);

  /* foreground */
  fg_color_draw = gtk_toggle_button_new();
  gtk_widget_show (fg_color_draw);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (fg_color_draw), TRUE);
  ApplyFg = TRUE;
  gtk_container_set_border_width (GTK_CONTAINER (fg_color_draw), 5);
  gtk_fixed_put (GTK_FIXED (fixed1), fg_color_draw, 10, 10);

  /* drawing area background */
  color_draw = gtk_drawing_area_new ();
  gtk_drawing_area_size (GTK_DRAWING_AREA (color_draw), 40, 40);
  gtk_container_add (GTK_CONTAINER (bg_color_draw), color_draw);
  /* connecte the data to the drawing area */
  gtk_object_set_data (GTK_OBJECT(color_draw), "FgToggle", (gpointer)fg_color_draw);
  gtk_object_set_data (GTK_OBJECT(color_draw), "BgToggle", (gpointer)bg_color_draw);
  gtk_widget_set_events (GTK_WIDGET (color_draw), GDK_EXPOSURE_MASK | GDK_BUTTON_PRESS_MASK);
  /* connecte the expose event to redraw the good color */
  ConnectSignalGTK (GTK_OBJECT (color_draw),
		    "expose_event",
		    GTK_SIGNAL_FUNC (ColorsExposeFgBgGTK),
		    (gpointer)&BgColor);
  ConnectSignalGTK (GTK_OBJECT (color_draw),
		    "button_press_event",
		    GTK_SIGNAL_FUNC (ButtonBgToggledGTK),
		    (gpointer)NULL);

  /* drawing area foreground */
  color_draw = gtk_drawing_area_new ();
  gtk_drawing_area_size (GTK_DRAWING_AREA (color_draw), 40, 40);
  gtk_container_add (GTK_CONTAINER (fg_color_draw), color_draw);
  /* connecte the data to the drawing area */
  gtk_object_set_data (GTK_OBJECT(color_draw), "FgToggle", (gpointer)fg_color_draw);
  gtk_object_set_data (GTK_OBJECT(color_draw), "BgToggle", (gpointer)bg_color_draw);
  gtk_widget_set_events (GTK_WIDGET (color_draw), GDK_EXPOSURE_MASK | GDK_BUTTON_PRESS_MASK);
  /* connecte the expose event to redraw the good color */
  ConnectSignalGTK (GTK_OBJECT (color_draw),
		    "expose_event",
		    GTK_SIGNAL_FUNC (ColorsExposeFgBgGTK),
		    (gpointer)&FgColor);
  /* connect synchronize callback */
  ConnectSignalGTK (GTK_OBJECT (color_draw),
		    "button_press_event",
		    GTK_SIGNAL_FUNC (ButtonFgToggledGTK),
		    (gpointer)NULL);


  /******** the color palette *******/
  frame2 = gtk_frame_new (TtaGetMessage (LIB, TMSG_CPCOLORBUTTON));
  gtk_widget_show (frame2);
  gtk_box_pack_start (GTK_BOX (vbox2), frame2, TRUE, TRUE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (frame2), 5);

  /* vbox witch contain all the colors */
  vbox4 = gtk_vbox_new (FALSE, 5);
  gtk_widget_show (vbox4);
  gtk_container_add (GTK_CONTAINER (frame2), vbox4);
  gtk_container_set_border_width (GTK_CONTAINER (vbox4), 4);

  /* Default color */
  /* hbox witch contain the default color */
  hbox4 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox4);
  gtk_box_pack_start (GTK_BOX (vbox4), hbox4, TRUE, TRUE, 0);
  /* create a color button (default) */
  color_button = gtk_button_new_with_label (TtaGetMessage (LIB, TMSG_STD_COLORS));
  gtk_widget_show (color_button);
  /* on connecte le callback pour l'activation de la couleur par default */
  /* on passe la couleur -1 */
  ConnectSignalGTK (GTK_OBJECT (color_button),
		    "clicked",
		    GTK_SIGNAL_FUNC(ColorsPressGTK),
		    (gpointer)-1);
  gtk_box_pack_start (GTK_BOX (hbox4), color_button, FALSE, FALSE, 0);

  /* hbox witch contain all the colors columns */
  hbox4 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox4);
  gtk_box_pack_start (GTK_BOX (vbox4), hbox4, TRUE, TRUE, 0);
  j = 0;
  for (i = 1; i<=NumberOfColors (); i++)
    {
      if (i-1 == COLORS_COL*j)
	{
	  /* vbox witch contain one color column */
	  vbox5 = gtk_vbox_new (FALSE, 0);
	  gtk_widget_show (vbox5);
	  gtk_box_pack_start (GTK_BOX (hbox4), vbox5, FALSE, FALSE, 0);
	  j++;
	}
      /* create a color button */
      color_button = gtk_button_new ();
      gtk_widget_show (color_button);
      color_draw = gtk_drawing_area_new ();
      gtk_drawing_area_size (GTK_DRAWING_AREA (color_draw), 20, 20);
      gtk_container_add (GTK_CONTAINER (color_button), color_draw);
      gtk_widget_set_events (GTK_WIDGET (color_draw), GDK_EXPOSURE_MASK);
      /* connecte the expose event to redraw the good color */
      ConnectSignalGTK (GTK_OBJECT (color_draw),
			"expose_event",
			GTK_SIGNAL_FUNC (ColorsExposeGTK),
			(gpointer)i);
      /* connecte the clicked action to cahnge the color */
      ConnectSignalGTK (GTK_OBJECT (color_button),
			"clicked",
			GTK_SIGNAL_FUNC(ColorsPressGTK),
			(gpointer)i);
      gtk_box_pack_start (GTK_BOX (vbox5), GTK_WIDGET(color_button), FALSE, FALSE, 0);
    }


  /******** All the action buttons *********/
  button4 = gtk_button_new_with_label (TtaGetMessage (LIB, TMSG_CPMOREBUTTON));
  gtk_widget_show (button4);
  gtk_box_pack_start (GTK_BOX (vbox4), button4, FALSE, FALSE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (button4), 2);
  ConnectSignalGTK (GTK_OBJECT (button4),
		    "clicked",
		    GTK_SIGNAL_FUNC (CreateExtendedColorSelectionGTK),
		    (gpointer)NULL);

  hbox1 = gtk_hbox_new (TRUE, 0);
  gtk_widget_show (hbox1);
  gtk_box_pack_start (GTK_BOX (vbox1), hbox1, TRUE, TRUE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (hbox1), 5);

  button1 = gtk_button_new_with_label (TtaGetMessage (LIB, TMSG_APPLY));
  gtk_widget_show (button1);
  gtk_box_pack_start (GTK_BOX (hbox1), button1, FALSE, FALSE, 0); 
  gtk_container_set_border_width (GTK_CONTAINER (button1), 2);
  ConnectSignalGTK (GTK_OBJECT (button1),
		    "clicked",
		    GTK_SIGNAL_FUNC (ApplyColorToSelectedElementGTK),
		    (gpointer)NULL);

  button2 = gtk_button_new_with_label (TtaGetMessage (LIB, TMSG_CPGETBUTTON));
  gtk_widget_show (button2);
  gtk_box_pack_start (GTK_BOX (hbox1), button2, FALSE, FALSE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (button2), 2);
  ConnectSignalGTK (GTK_OBJECT (button2),
		    "clicked",
		    GTK_SIGNAL_FUNC (GetSelectedElementColorGTK),
		    (gpointer)NULL);

  button3 = gtk_button_new_with_label (TtaGetMessage (LIB, TMSG_DONE));
  gtk_container_set_border_width (GTK_CONTAINER (button3), 2);
  gtk_widget_show (button3);
  gtk_box_pack_start (GTK_BOX (hbox1), button3, FALSE, FALSE, 0);
  ConnectSignalGTK (GTK_OBJECT (button3),
		    "clicked",
		    GTK_SIGNAL_FUNC (EndPalette),
		    (gpointer)NULL);

  /* show all the dialogues */
  gtk_widget_set_uposition (Color_Animation_Popup, x, y);
  gtk_widget_show_all(GTK_WIDGET(Color_Animation_Popup));
  Color_Window2 = GTK_WIDGET (Color_Animation_Popup)->window;

  /* pas de selection precedente */
  LastBg = -1;
  LastFg = -1;

  return TRUE;
}
#endif /* _GTK */


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
#ifdef _GTK
	    ThotCreatePalette (KbX, KbY);
#endif /* _GTK */
	  }
#ifdef _GTK
	else
	  {
	    gtk_widget_show_all (GTK_WIDGET(Color_Palette));
	    gdk_window_raise (GTK_WIDGET(Color_Palette)->window);
	  }
#endif /* _GTK */
	
	/* recherche le pave concerne */
	if (view > 100)
	  pAb = AbsBoxOfEl (pFirstSel, 1);
	else
	  pAb = AbsBoxOfEl (pFirstSel, view);
	
	if (pAb != NULL)
	  ThotSelectPalette (pAb->AbBackground, pAb->AbForeground);
#ifdef _WINGUI
	if (HwndColorPal == (HWND) 0) 
	  ThotCreatePalette (KbX, KbY);
	else 
	  SetForegroundWindow (HwndColorPal);
#endif /* _WINGUI */
     }
}

/*----------------------------------------------------------------------
   TtcGetPaletteColors
   Displays the color palette and returns the foreground and background
   colors chosen by the user.
  ----------------------------------------------------------------------*/
void TtcGetPaletteColors (int *fg, int *bg, ThotBool palType)
{ 
#ifdef _WINGUI
    PalMessageSet1 = palType;
    FgColor = BgColor = -1;
    applyToSelection = FALSE;
    ThotCreatePalette (200, 200);
    *fg = FgColor;
    *bg = BgColor;
    applyToSelection = TRUE;
#endif /* _WINGUI */

#ifdef _GTK
    /* PalMessageSet1 = palType; */
    FgColor = BgColor = -1;
    applyToSelection = FALSE;

    ThotCreateModalPalette (200, 200);

    while (Color_Animation_Popup)
      while (gtk_events_pending ())
	gtk_main_iteration ();
  

    *fg = FgColor;
    *bg = BgColor;
    applyToSelection = TRUE;
    /**/
#endif /* _GTK */
}
