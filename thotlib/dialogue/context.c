/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2004
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 
/*
 * context.c : 
 *     Handle the context of documents.
 *
 * Authors: I. Vatton (INRIA)
 *          R. Guetari (W3C/INRIA) - Unicode and Windows version
 */
#ifdef _WX
  #include "wx/wx.h"
  #include "wx/gdicmn.h"
#endif /* _WX */

#include "thot_gui.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "libmsg.h"
#include "message.h"
#include "picture.h"
#include "appdialogue.h"
#ifdef _WINGUI
  #include "wininclude.h"
#endif /* _WINGUI */


#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "frame_tv.h"
#include "boxes_tv.h"
#include "thotcolor_tv.h"
#include "appdialogue_tv.h"
#include "units_tv.h"

ThotColorStruct  cblack;
static ThotColorStruct  cwhite;
/*extern int              errno; allready declared ... do not compile with g++ */

#include "appli_f.h"
#include "checkermenu_f.h"
#include "editcommands_f.h"
#include "font_f.h"
#include "inites_f.h"
#include "memory_f.h"
#include "picture_f.h"
#include "registry_f.h"
#include "textcommands_f.h"


/*----------------------------------------------------------------------
 * FindColor looks for the named color ressource.
 * The result is the closest color found the Thot color table.
 ----------------------------------------------------------------------*/
static ThotBool FindColor (int disp, char *name, char *colorplace,
			   char *defaultcolor, ThotColor *colorpixel)
{
   int                 col;
   char               *value;
   unsigned short      red;
   unsigned short      green;
   unsigned short      blue;

   value = TtaGetEnvString (colorplace);
   /* do you need to take the default color? */
   if (value != NULL && value[0] != EOS)
     TtaGiveRGB (value, &red, &green, &blue);
   else if (defaultcolor != NULL)
     TtaGiveRGB (defaultcolor, &red, &green, &blue);
   else
     return (FALSE);

   col = TtaGetThotColor (red, green, blue);
   /* register the default background color */
   if (strcmp (colorplace, "BackgroundColor") == 0)
     DefaultBColor = col;
   /* register the default background color */
   else if (strcmp (colorplace, "ForegroundColor") == 0)
     DefaultFColor = col;
   else if (strcmp (colorplace, "BgSelectColor") == 0)
     BgSelColor = col;
   else if (strcmp (colorplace, "FgSelectColor") == 0)
     FgSelColor = col;
   
#ifdef _WINGUI 
   *colorpixel = col;
#endif /* _WINGUI */
#if defined(_GTK) || defined(_WX)
   *colorpixel = ColorPixel (col);
#endif /* defined(_GTK) || defined(_WX) */
   return (TRUE);

}

/*----------------------------------------------------------------------
 *      InitCurs load the cursors used by the graphic interface.
 ----------------------------------------------------------------------*/
static void InitCurs ()
{
#ifdef _GTK
   ArrowCurs = gdk_cursor_new (GDK_LEFT_PTR);
   WindowCurs = gdk_cursor_new (GDK_HAND2);
   VCurs = gdk_cursor_new (GDK_SB_V_DOUBLE_ARROW);
   HCurs = gdk_cursor_new (GDK_SB_H_DOUBLE_ARROW);
   HVCurs = gdk_cursor_new (GDK_PLUS);
   WaitCurs = gdk_cursor_new (GDK_CLOCK);
#endif /* _GTK */
}

/*----------------------------------------------------------------------
 *      TtaUpdateEditorColors Updates the X-Window colors according to
 *      the current chosen user values
 ----------------------------------------------------------------------*/
void TtaUpdateEditorColors (void)
{
  char   *name;
  ThotBool  found;

  name = TtaGetEnvString ("appname");
  /* background color */
  found = FindColor (0, name, "BackgroundColor", "LightGrey1", &White_Color);
  /* drawing color */
  found = FindColor (0, name, "ForegroundColor", "Black", &Black_Color);
  /* selection colors */
  found = FindColor (0, name, "FgSelectColor", "White", &White_Color);
  found = FindColor (0, name, "BgSelectColor", "#008BB2", &Black_Color);
  /* The reference color */
  found = FindColor (0, name, "ActiveBoxColor", "Red", &(Box_Color));
  /* color for read-only sections */
  found = FindColor (0, name, "ReadOnlyColor", "Black", &(RO_Color));
  /* color for the menu background */
  found = FindColor (0, name, "MenuBgColor", "Grey", &BgMenu_Color);
  /* color for the menu foregroundground */
  found = FindColor (0, name, "MenuFgColor", "Black", &FgMenu_Color);
  /* scrolls color */
  Scroll_Color = BgMenu_Color;
  /* color for the inactive entries */
  found = FindColor (0, name, "InactiveItemColor", "LightGrey2", &InactiveB_Color);
}

/*----------------------------------------------------------------------
 *      InitColors initializes the Thot predefined X-Window colors.
 ----------------------------------------------------------------------*/
static void InitColors (char* name)
{
   ThotBool            found;
#ifdef _GTK
   GdkVisual          *vptr;
   GdkVisualType       vinfo;
   vptr = gdk_visual_get_best ();
   vinfo = gdk_visual_get_best_type ();
   if (vptr)
     TtIsTrueColor = (vinfo == GDK_VISUAL_TRUE_COLOR || vinfo == GDK_VISUAL_DIRECT_COLOR );
   else
     TtIsTrueColor = FALSE;
   /* Depending on the display Black and White order may be inverted */
  // if (XWhitePixel (TtDisplay, TtScreen) == 0)
  gdk_color_white (TtCmap, (GdkColor *)&cwhite);
  gdk_color_black (TtCmap, (GdkColor *)&cblack);
   /* Initialize colors for the application */
    Black_Color = 0x000000;
    FgMenu_Color = 0x000000;
    White_Color = 0xffffff;
    Scroll_Color =  0xffffff;
#endif /* _GTK */

   if (TtWDepth > 1)
     TtaUpdateEditorColors ();
   else
     {
     /* at least allocate the selection color */
       found = FindColor (0, name, "FgSelectColor", "White", &White_Color);
       found = FindColor (0, name, "BgSelectColor", "Black", &Black_Color);
     }
}

#if defined(_GTK) || defined(_WX)
/*----------------------------------------------------------------------
  InitGraphicContexts initialize the X-Windows graphic contexts and their
  Windows counterpart in Microsoft environment.
 ----------------------------------------------------------------------*/
static void InitGraphicContexts (void)
{
#ifdef _GTK
  int                 white;
  int                 black;
  ThotPixmap          pix;

  gdk_rgb_init ();

  white = ColorNumber ("White");
  black = ColorNumber ("Black");
  pix = CreatePattern (0, black, white, 6);
  
  /* Create a Graphic Context to write white on black. */
  TtWhiteGC = gdk_gc_new (DefaultDrawable);
  gdk_rgb_gc_set_background (TtWhiteGC, Black_Color);
  gdk_rgb_gc_set_foreground (TtWhiteGC, White_Color);
  gdk_gc_set_function (TtWhiteGC, GDK_COPY); 
  gdk_gc_set_exposures (TtWhiteGC, TRUE);

 
  /* Create a Graphic Context to write black on white. */
  TtBlackGC = gdk_gc_new (DefaultDrawable);
  gdk_rgb_gc_set_foreground (TtBlackGC, Black_Color);
  gdk_rgb_gc_set_background (TtBlackGC, White_Color);
  gdk_gc_set_function (TtBlackGC, GDK_COPY);
  
  /*
   * Create a Graphic Context to write black on white,
   * but with a specific 10101010 pattern.
   */
  TtLineGC = gdk_gc_new (DefaultDrawable);
  gdk_rgb_gc_set_foreground (TtLineGC, Black_Color);
  gdk_rgb_gc_set_background (TtLineGC, White_Color);
  gdk_gc_set_function (TtLineGC, GDK_COPY);
  gdk_gc_set_tile (TtLineGC, (GdkPixmap *)pix);

  /* Another Graphic Context to write black on white, for dialogs. */
  TtDialogueGC = gdk_gc_new (DefaultDrawable);
  gdk_rgb_gc_set_foreground (TtDialogueGC, Black_Color);
  gdk_rgb_gc_set_background (TtDialogueGC, White_Color);
  gdk_gc_set_function (TtDialogueGC, GDK_COPY);

  /*
    * A Graphic Context to show selected objects. On X-Windows,
    * the colormap indexes are XORed to show the object without
    * destroying the colors : XOR.XOR = I ...
    */
  TtInvertGC = gdk_gc_new (DefaultDrawable);
  if (TtWDepth > 1)
    gdk_rgb_gc_set_foreground (TtInvertGC, Black_Color);
  else
    gdk_rgb_gc_set_foreground (TtInvertGC, Black_Color);
  gdk_rgb_gc_set_background (TtInvertGC, White_Color);
  gdk_gc_set_function (TtInvertGC, GDK_INVERT);

  /*
   * A Graphic Context for trame objects.
   */
  TtGreyGC = gdk_gc_new (DefaultDrawable);
  gdk_rgb_gc_set_foreground (TtGreyGC, Black_Color);
  gdk_rgb_gc_set_background (TtGreyGC, White_Color);
  gdk_gc_set_function (TtGreyGC, GDK_COPY);
  gdk_gc_set_fill (TtGreyGC, GDK_TILED);

  gdk_pixmap_unref ((GdkPixmap *)pix);
#endif /* _GTK */
}
#endif /* defined(_GTK) || defined(_WX) */


/*----------------------------------------------------------------------
 *      ThotInitDisplay initialize all the output settings.
 ----------------------------------------------------------------------*/
void ThotInitDisplay (char* name, int dx, int dy)
{
#ifdef _WINGUI
   WIN_GetDeviceContext (-1);
   TtWDepth = GetDeviceCaps (TtDisplay, PLANES);
   if (TtWDepth == 1)
     TtWDepth = GetDeviceCaps (TtDisplay, BITSPIXEL);

   /* test for 15 or 16 bits. Adapted from:
       Tim Lesher http://mlarchive.ima.com/windev/1999/0273.html */
   if (TtWDepth == 16)
   {
     LPBITMAPINFOHEADER pDib;
     HDC hdc;
     HBITMAP hbm;
     int iDibSize;

     iDibSize = sizeof (BITMAPINFOHEADER) + 256 * sizeof (DWORD);
     pDib = (LPBITMAPINFOHEADER) malloc(iDibSize);
     memset (pDib, 0, iDibSize);
        
     /* Use old-style BMIH for compatibility, and leave biBitCount zero */
     pDib->biSize = sizeof(BITMAPINFOHEADER);
        
    /* Need to call GetDIBits twice: the first one just fills in the */
    /* biBitCount member; the second fills in the bitfields or palette */
    hdc = GetDC (NULL);
    hbm = CreateCompatibleBitmap (hdc, 1, 1);
    GetDIBits (hdc, hbm, 0, 1, NULL, (LPBITMAPINFO) pDib, DIB_RGB_COLORS);
    GetDIBits (hdc, hbm, 0, 1, NULL, (LPBITMAPINFO) pDib, DIB_RGB_COLORS);
    DeleteObject (hbm);
    if (!ReleaseDC (NULL, hdc))
      WinErrorBox (NULL, "ReleaseDC: ThotInitDisplay");

    /* printf( "Current video mode is %lu-bit.\n", pDib->biBitCount ); */
    if (BI_BITFIELDS == pDib->biCompression)
      {
        DWORD * pdwFields = (DWORD*) (pDib+1);
	/* 0xf800: == 565 BGR;
	   0x7c00  == 555 BGR; */
	if (pdwFields[0] == 0x7c00)
	  TtWDepth = 15;
      }
    free (pDib);
   }

   InitDocColors (name);
   InitColors (name);   
   if (WIN_LastBitmap)
     DeleteObject (WIN_LastBitmap);
   WIN_LastBitmap = 0;
   InitCurs ();
   DOT_PER_INCH = GetDeviceCaps (TtDisplay, LOGPIXELSY);
   InitDialogueFonts (name);

   /* Initialization of Picture Drivers */
   InitPictureHandlers (FALSE);
   WIN_ReleaseDeviceContext ();
#endif /* _WINGUI */
   
#ifdef _GTK
   
   /* Declaration of a DefaultDrawable useful for the creation of Pixmap and the
      initialization of GraphicContexts */
   DefaultWindow = gtk_window_new (GTK_WINDOW_TOPLEVEL);
   gtk_widget_realize (DefaultWindow);
   DefaultDrawingarea = gtk_drawing_area_new();
   gtk_widget_set_parent (DefaultDrawingarea, DefaultWindow); 
   gtk_widget_realize (DefaultDrawingarea);
   DefaultDrawable = DefaultDrawingarea->window;
   /* int x, y, width, height, depth;
      gdk_window_get_geometry (DefaultDrawable, &x, &y, &width, &height, &depth);*/
   TtRootWindow = DefaultWindow->window;

#ifndef _GL
   gtk_widget_push_visual (gdk_imlib_get_visual ());
   gtk_widget_push_colormap (gdk_imlib_get_colormap ());
  
   TtWDepth = gdk_visual_get_best_depth (); 
   TtCmap = gdk_imlib_get_colormap ();
#else /*_GL*/
   TtWDepth = gdk_visual_get_best_depth (); 
   TtCmap = gdk_colormap_new (gdk_rgb_get_visual (), TRUE);
#endif /* _GL    */
   InitDocColors (name);
   InitColors (name);
   InitGraphicContexts ();
   InitCurs ();
   InitDialogueFonts (name);

   /* Initialization of Picture Drivers */
   InitPictureHandlers (FALSE);
#endif /* _GTK */
#ifdef _WX  
#ifdef _GL
   TtWDepth = wxDisplayDepth(); 
   /* not used : TtCmap = gdk_colormap_new (gdk_rgb_get_visual (), TRUE);*/
#endif /*_GL */

   InitDocColors (name);
   InitColors (name);
   InitGraphicContexts ();
   InitCurs ();
   InitDialogueFonts (name);

   /* Initialization of Picture Drivers */
   InitPictureHandlers (FALSE);
#endif /* _WX */
}

/*----------------------------------------------------------------------
 *      InitDocContexts initialize the frames' contexts
 ----------------------------------------------------------------------*/
void InitDocContexts ()
{
  int                 i;

  /* Initialize the memory allocator */
  InitKernelMemory ();

  /* Initialisation de la table des frames */
  for (i = 0; i <= MAX_FRAME; i++)
    {
#ifndef _WX
      FrRef[i] = 0;
#endif /* _WX */
#ifdef _WINGUI
      FrMainRef[i] = 0;
#endif /* _WINGUI */
    }
  PackBoxRoot = NULL;		/* Don't check enclosing for current boxes */
  DifferedPackBlocks = NULL;	/* Don't differ enclosing for current boxes */
  PackRows = TRUE;              /* Pack row elements */
  BoxCreating = FALSE;		/* No interractive creation yet */
  AnyWidthUpdate = FALSE;	/* No current width change */
  InitializeOtherThings ();
}



