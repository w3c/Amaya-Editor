/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
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

#include "thot_gui.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"

#include "libmsg.h"
#include "message.h"
#include "picture.h"
#include "appdialogue.h"

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "frame_tv.h"
#include "boxes_tv.h"
#include "thotcolor_tv.h"
#include "appdialogue_tv.h"

 ThotColorStruct  cblack;
static ThotColorStruct  cwhite;

#include "appli_f.h"
#include "checkermenu_f.h"
#include "editcommands_f.h"
#include "font_f.h"
#include "inites_f.h"
#include "memory_f.h"
#include "picture_f.h"
#include "registry_f.h"
#include "textcommands_f.h"
#include "ustring_f.h"

#ifdef _WINDOWS
#include "wininclude.h"

static int   palSize;
static int   initialized = 0;
int          nbPalEntries;
PALETTEENTRY palEntries[256];
int          nbSysColors;

/*----------------------------------------------------------------------
 *      WinCreateGC is an emulation of the XWindows XCreateGC under
 *         MS-Windows.
 ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotGC              WinCreateGC (void)
#else  /* __STDC__ */
ThotGC              WinCreateGC (void)
#endif				/* __STDC__ */
{
   ThotGC gc = (ThotGC) TtaGetMemory (sizeof (WIN_GC_BLK));
   return (gc);
}

/*----------------------------------------------------------------------
 *      WinInitColors initialize the color table depending on the
 *         device capabilities under MS-Windows.
 ----------------------------------------------------------------------*/
#ifdef __STDC__
void WinInitColors (void)
#else /* __STDC__ */
void WinInitColors ()
#endif /* __STDC__ */
{
   int        i;

   if (initialized)
      return;
   palSize = GetDeviceCaps (TtDisplay, SIZEPALETTE);
   if (palSize == 0)
     TtIsTrueColor = TRUE;
   else
     TtIsTrueColor = FALSE;

   /* Create a color palette for the Thot set of colors. */
   /* fill-in the Pix_Color table */
   for (i = 0; i < MAX_COLOR; i++) 
       Pix_Color[i] = RGB (RGB_Table[i].red, RGB_Table[i].green, RGB_Table[i].blue);

   /* set up the default background colors for all views. */
   for (i = 0; i < (sizeof (BackgroundColor) / sizeof (BackgroundColor[0])); i++)
       BackgroundColor[i] = 0;

   initialized = 1;
}
#endif /* _WINDOWS */

#ifndef _WINDOWS
/*----------------------------------------------------------------------
 * XWindowError is the X-Windows non-fatal errors handler.
 ----------------------------------------------------------------------*/
#ifdef __STDC__
static int          XWindowError (Display * dpy, XErrorEvent * err)
#else  /* __STDC__ */
static int          XWindowError (dpy, err)
Display            *dpy;
XErrorEvent        *err;

#endif /* __STDC__ */
{
   CHAR_T                msg[200];

   XGetErrorText (dpy, err->error_code, msg, 200);
   return (0);
}

/*----------------------------------------------------------------------
 * XWindowFatalError is the X-Windows fatal errors handler.
 ----------------------------------------------------------------------*/
#ifdef __STDC__
static int          XWindowFatalError (Display * dpy)
#else  /* __STDC__ */
static int          XWindowFatalError (dpy)
Display            *dpy;
#endif /* __STDC__ */
{
   extern int          errno;

   if (errno != EPIPE)
      TtaDisplayMessage (FATAL, TtaGetMessage (LIB, TMSG_LIB_X11_ERR), DisplayString (dpy));
   else
      TtaDisplayMessage (FATAL, TtaGetMessage (LIB, TMSG_LIB_X11_ERR), DisplayString (dpy));
   if (ThotLocalActions[T_backuponfatal] != NULL)
     (*ThotLocalActions[T_backuponfatal]) ();
   return (0);
}
#endif /* _WINDOWS */


/*----------------------------------------------------------------------
 *   TtaGiveRGB returns the RGB of the color.
 ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaGiveRGB (CHAR_T* colname, unsigned short *red, unsigned short *green, unsigned short *blue)
#else  /* __STDC__ */
void                TtaGiveRGB (colname, red, green, blue)
CHAR_T*             colname;
unsigned short*     red;
unsigned short*     green;
unsigned short*     blue;

#endif /* __STDC__ */
{
   int                 i;
#ifndef _WINDOWS 
   ThotColorStruct     color;

   /* Lookup the color name in the X color name database */
#ifdef _GTK
    if (gdk_color_parse (colname, &color))
#else /* _GTK */
   if (XParseColor (TtDisplay, TtCmap, colname, &color))
#endif /* _GTK */
     {
	/* normalize RGB color values to 8 bits values */
	color.red >>= 8;
	color.green >>= 8;
	color.blue >>= 8;
	*red = color.red;
	*green = color.green;
	*blue = color.blue;
     }
   else
     {
#endif /* _WINDOWS */
       /* Lookup the color name in the application color name database */
       ThotBool found = FALSE;
       for (i = 0; i < NColors && !found; i++)
           if (!ustrcasecmp (ColorName (i), colname)) {
              found = TRUE;
              *red   = RGB_Table[i].red;
              *green = RGB_Table[i].green;
              *blue  = RGB_Table[i].blue;
           }
#ifndef _WINDOWS
     }
#endif /* _WINDOWS */
}


/*----------------------------------------------------------------------
 * FindColor looks for the named color ressource.
 * The result is the closest color found the Thot color table.
 ----------------------------------------------------------------------*/
#ifdef __STDC__
static ThotBool     FindColor (int disp, CHAR_T* name, char* colorplace, CHAR_T* defaultcolor, ThotColor *colorpixel)
#else  /* __STDC__ */
static ThotBool     FindColor (disp, name, colorplace, defaultcolor, colorpixel)
int         disp;
CHAR_T*     name;
char*       colorplace;
CHAR_T*     defaultcolor;
ThotColor*  colorpixel;

#endif /* __STDC__ */
{
   int                 col;
   CHAR_T*             value;
   unsigned short      red;
   unsigned short      green;
   unsigned short      blue;

   value = TtaGetEnvString (colorplace);
   /* do you need to take the default color? */
   if (value == NULL && defaultcolor != NULL)
       value = TtaWCSdup (defaultcolor);

   if (value != NULL)
     {
	TtaGiveRGB (value, &red, &green, &blue);
	col = TtaGetThotColor (red, green, blue);
	/* register the default background color */
	if (strcmp (colorplace, "BackgroundColor") == 0)
	   DefaultBColor = col;
	/* register the default background color */
	else if (strcmp (colorplace, "ForegroundColor") == 0)
	   DefaultFColor = col;
#   ifdef _WINDOWS 
	*colorpixel = col;
#   else  /* _WINDOWS */
	*colorpixel = ColorPixel (col);
#   endif /* _WINDOWS */
	return (TRUE);
     }
   else
      return (FALSE);
}

/*----------------------------------------------------------------------
 *      InitCurs load the cursors used by the graphic interface.
 ----------------------------------------------------------------------*/
static void         InitCurs ()
{
#ifndef _WINDOWS
   WindowCurs = XCreateFontCursor (TtDisplay, XC_hand2);
   VCurs = XCreateFontCursor (TtDisplay, XC_sb_v_double_arrow);
   HCurs = XCreateFontCursor (TtDisplay, XC_sb_h_double_arrow);
   HVCurs = XCreateFontCursor (TtDisplay, XC_fleur);
   WaitCurs = XCreateFontCursor (TtDisplay, XC_watch);
#endif /* _WINDOWS */
}

/*----------------------------------------------------------------------
 *      TtaUpdateEditorColors Updates the X-Window colors according to
 *      the current chosen user values
 ----------------------------------------------------------------------*/
#ifdef __STDC__
void TtaUpdateEditorColors (void)
#else
void TtaUpdateEditorColors ()
#endif /* __STDC__ */
{
  CHAR_T*   app_name;
  ThotBool  found;

  app_name =  TtaGetEnvString ("appname");

  /* background color */
#       ifndef _WINDOWS
  found = FindColor (0, app_name, "BackgroundColor", TEXT("gainsboro"), &White_Color);
#       else  /* _WINDOWS */
  found = FindColor (0, app_name, "BackgroundColor", TEXT("LightGrey1"), &White_Color);
#       endif /* _WINDOWS */
  /* drawing color */
  found = FindColor (0, app_name, "ForegroundColor", TEXT("Black"), &Black_Color);
  /* color for the menu background */
  found = FindColor (0, app_name, "MenuBgColor", TEXT("Grey"), &BgMenu_Color);
  /* color for the menu foregroundground */
  found = FindColor (0, app_name, "MenuFgColor", TEXT("Black"), &FgMenu_Color);
  /* scrolls color */
  Scroll_Color = BgMenu_Color;
#ifdef _WINDOWS
   WinInitColors ();
#  endif /* _WINDOWS */
}

/*----------------------------------------------------------------------
 *      InitColors initializes the Thot predefined X-Window colors.
 ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         InitColors (CHAR_T* name)
#else  /* __STDC__ */
static void         InitColors (name)
CHAR_T*             name;

#endif /* __STDC__ */
{
   ThotBool            found;
#ifndef _WINDOWS
#ifdef _GTK
   GdkVisual          *vptr;
   GdkVisualType       vinfo;
   int                 test;
#else /* _GTK */
   XVisualInfo        *vptr;
   XVisualInfo         vinfo;
#endif /* _GTK */
   ThotColorStruct     col;
   int                 i;

#ifdef _GTK
   vptr = gdk_visual_get_best ();
   vinfo = gdk_visual_get_best_type ();
   if (vptr)
     {
       TtIsTrueColor = (vinfo == GDK_VISUAL_TRUE_COLOR || vinfo == GDK_VISUAL_DIRECT_COLOR );
      // XFree (vptr);
     }
   else
     TtIsTrueColor = FALSE;
   /* Depending on the display Black and White order may be inverted */
  // if (XWhitePixel (TtDisplay, TtScreen) == 0)
  test =  gdk_color_white (TtCmap, &cwhite);
  test = gdk_color_black (TtCmap, &cblack);
   if (cwhite.pixel == 0)
     {
       /* je ne sais pas ce qu'il faut faire */
       /*if (!XAllocNamedColor (TtDisplay, TtCmap, "White", &cwhite, &col))
	   TtaDisplaySimpleMessage (FATAL, LIB, TMSG_NOT_ENOUGH_MEMORY);
	if (!XAllocNamedColor (TtDisplay, TtCmap, "Black", &cblack, &col))
	   TtaDisplaySimpleMessage (FATAL, LIB, TMSG_NOT_ENOUGH_MEMORY);
     }
   else
     {
	if (!XAllocNamedColor (TtDisplay, TtCmap, "Black", &cblack, &col))
	   TtaDisplaySimpleMessage (FATAL, LIB, TMSG_NOT_ENOUGH_MEMORY);
	if (!XAllocNamedColor (TtDisplay, TtCmap, "White", &cwhite, &col))
	   TtaDisplaySimpleMessage (FATAL, LIB, TMSG_NOT_ENOUGH_MEMORY)*/;
     }
   /* Initialize colors for the application */
   /*   Black_Color  = cblack.pixel;
	FgMenu_Color = Select_Color = cblack.pixel;
	White_Color  = cwhite.pixel;
	Scroll_Color = BgMenu_Color = cwhite.pixel; */
   /* a modifier */
    Black_Color = 0x000000;
    FgMenu_Color = Select_Color =0x000000;
    White_Color = 0xffffff;
    Scroll_Color =  0xffffff;
#else /* _GTK */
   vinfo.visualid = XVisualIDFromVisual (XDefaultVisual (TtDisplay, TtScreen));
   vptr = XGetVisualInfo (TtDisplay, VisualIDMask, &vinfo, &i);
   if (vptr)
     {
       TtIsTrueColor = (vptr->class == TrueColor || vptr->class == DirectColor);
       XFree (vptr);
     }
   else
     TtIsTrueColor = FALSE;

   /* Depending on the display Black and White order may be inverted */
   if (XWhitePixel (TtDisplay, TtScreen) == 0)
     {
	if (!XAllocNamedColor (TtDisplay, TtCmap, "White", &cwhite, &col))
	   TtaDisplaySimpleMessage (FATAL, LIB, TMSG_NOT_ENOUGH_MEMORY);
	if (!XAllocNamedColor (TtDisplay, TtCmap, "Black", &cblack, &col))
	   TtaDisplaySimpleMessage (FATAL, LIB, TMSG_NOT_ENOUGH_MEMORY);
     }
   else
     {
	if (!XAllocNamedColor (TtDisplay, TtCmap, "Black", &cblack, &col))
	   TtaDisplaySimpleMessage (FATAL, LIB, TMSG_NOT_ENOUGH_MEMORY);
	if (!XAllocNamedColor (TtDisplay, TtCmap, "White", &cwhite, &col))
	   TtaDisplaySimpleMessage (FATAL, LIB, TMSG_NOT_ENOUGH_MEMORY);
     }
   /* Initialize colors for the application */
   Black_Color  = cblack.pixel;
   FgMenu_Color = Select_Color = cblack.pixel;
   White_Color  = cwhite.pixel;
   Scroll_Color = BgMenu_Color = cwhite.pixel;
#endif /* _GTK */
#endif /* _WINDOWS */

   if (TtWDepth > 1)
     {
#ifndef _WINDOWS
#ifdef _GTK
	/* background color */
	found = FindColor (0, name, "BackgroundColor", "Grey", &White_Color);
	/* color for the selection */
	found = FindColor (0, name, "DocSelectColor", "SteelBlue", &Select_Color);
#else /* _GTK */
	/* background color */
	found = FindColor (0, name, "BackgroundColor", "gainsboro", &White_Color);
	/* color for borders and buttons */
	found = FindColor (0, name, "DocSelectColor", "SteelBlue", &Select_Color);
#endif /* _GTK */
#else  /* _WINDOWS */
	/* background color */
	found = FindColor (0, name, "BackgroundColor", TEXT("LightGrey1"), &White_Color);
	/* color for borders and buttons */
	found = FindColor (0, name, "DocSelectColor", TEXT("Blue"), &Select_Color);
#endif /* _WINDOWS */
	/* color for the selection */
	found = FindColor (0, name, "InactiveItemColor", TEXT("LightGrey1"), &InactiveB_Color);
	/* drawing color */
	found = FindColor (0, name, "ForegroundColor", TEXT("Black"), &Black_Color);
	/* color for the menu background */
	found = FindColor (0, name, "MenuBgColor", TEXT("Grey"), &BgMenu_Color);
	/* color for the menu foregroundground */
	found = FindColor (0, name, "MenuFgColor", TEXT("Black"), &FgMenu_Color);
	/* scrolls color */
	Scroll_Color = BgMenu_Color;
     }
   else
      /* at least allocate the selection color */
      found = FindColor (0, name, "DocSelectColor", TEXT("White"), &Select_Color);

   /* The reference color */
   found = FindColor (0, name, "ActiveBoxColor", TEXT("Red"), &(Box_Color));

   /* color for read-only sections */
   found = FindColor (0, name, "ReadOnlyColor", TEXT("Black"), &(RO_Color));

#ifndef _WINDOWS
   if (!found)
      Box_Color = cwhite.pixel;
   else if (TtWDepth == 1)
      Box_Color = cblack.pixel;
   if (!found)
      RO_Color = cwhite.pixel;
   else if (TtWDepth == 1)
      RO_Color = cblack.pixel;
#  else  /* _WINDOWS */
   WinInitColors ();
#  endif /* _WINDOWS */
}

/*----------------------------------------------------------------------
 *      InitGraphicContexts initialize the X-Windows graphic contexts and their Windows
 *	counterpart in Microsoft environment.
 ----------------------------------------------------------------------*/
#ifdef __STDC__
static void InitGraphicContexts (void)
#else /* __STDC__ */
static void InitGraphicContexts ()
#endif /* __STDC__ */
{
#ifndef _WINDOWS
#ifndef _GTK
  unsigned long       valuemask;
  XGCValues           GCmodel;
#endif /* _GTK */
  int                 white;
  int                 black;
  Pixmap              pix;
#endif /* _WINDOWS */

#ifndef _WINDOWS
#ifdef _GTK
   gdk_rgb_init ();
   white = ColorNumber ("White");
   black = ColorNumber ("Black");
   pix = CreatePattern (0, 0, 0, black, white, 6);

  /* Create a Graphic Context to write white on black. */
  TtWhiteGC = gdk_gc_new (DefaultDrawable);
  gdk_rgb_gc_set_background (TtWhiteGC, Black_Color);
  gdk_rgb_gc_set_foreground (TtWhiteGC, White_Color);
  gdk_gc_set_function (TtWhiteGC, GDK_COPY); 

 
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
  gdk_gc_set_tile (TtLineGC, pix);

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
    gdk_rgb_gc_set_foreground (TtInvertGC, Select_Color);
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

  gdk_pixmap_unref (pix);
#else /* _GTK */
   valuemask = GCForeground | GCBackground | GCFunction;
   white = ColorNumber ("White");
   black = ColorNumber ("Black");
   pix = CreatePattern (0, 0, 0, black, white, 6);

   GCmodel.function = GXcopy;
   GCmodel.foreground = White_Color;
   GCmodel.background = Black_Color;
   TtWhiteGC = XCreateGC (TtDisplay, TtRootWindow, valuemask, &GCmodel);

   /* Create a Graphic Context to write black on white. */
   GCmodel.foreground = Black_Color;
   GCmodel.background = White_Color;
   TtBlackGC = XCreateGC (TtDisplay, TtRootWindow, valuemask, &GCmodel);

   /*
    * Create a Graphic Context to write black on white,
    * but with a specific 10101010 pattern.
    */
   GCmodel.foreground = Black_Color;
   GCmodel.background = White_Color;
   TtLineGC = XCreateGC (TtDisplay, TtRootWindow, valuemask, &GCmodel);
   XSetTile (TtDisplay, TtLineGC, pix);

   /* Another Graphic Context to write black on white, for dialogs. */
   TtDialogueGC = XCreateGC (TtDisplay, TtRootWindow, valuemask, &GCmodel);

   /*
    * A Graphic Context to show selected objects. On X-Windows,
    * the colormap indexes are XORed to show the object without
    * destroying the colors : XOR.XOR = I ...
    */
   GCmodel.function = GXinvert;
   GCmodel.plane_mask = Select_Color;
   if (TtWDepth > 1)
      GCmodel.foreground = Black_Color;
   else
      GCmodel.foreground = Select_Color;
   GCmodel.background = White_Color;
   TtInvertGC = XCreateGC (TtDisplay, TtRootWindow, valuemask | GCPlaneMask, &GCmodel);

   /*
    * A Graphic Context for trame objects.
    */
   GCmodel.function = GXcopy;
   GCmodel.foreground = Black_Color;
   TtGreyGC = XCreateGC (TtDisplay, TtRootWindow, valuemask, &GCmodel);
   XSetFillStyle (TtDisplay, TtGreyGC, FillTiled);
   XFreePixmap (TtDisplay, pix);
#endif /* _GTK */

#else /* _WINDOWS */
   TtLineGC.capabilities = THOT_GC_FOREGROUND | THOT_GC_PEN;
   TtLineGC.foreground = 1;

   if (WIN_LastBitmap && !DeleteObject (WIN_LastBitmap))
      WinErrorBox (WIN_Main_Wd, TEXT("InitGraphicContexts"));
   WIN_LastBitmap = 0;
#endif /* _WINDOWS */
}


/*----------------------------------------------------------------------
 *      ThotInitDisplay initialize all the output settings.
 ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ThotInitDisplay (CHAR_T* name, int dx, int dy)
#else  /* __STDC__ */
void                ThotInitDisplay (name, dx, dy)
CHAR_T*             name;
int                 dx;
int                 dy;

#endif /* __STDC__ */
{
#ifdef _WINDOWS
   WIN_GetDeviceContext (-1);
   TtWDepth = GetDeviceCaps (TtDisplay, PLANES);
   if (TtWDepth == 1)
     TtWDepth = GetDeviceCaps (TtDisplay, BITSPIXEL);

   InitDocColors (name);
   InitColors (name);
   InitGraphicContexts ();
   InitCurs ();
   WIN_InitDialogueFonts (TtDisplay, name);

   /* Initialization of Picture Drivers */
   InitPictureHandlers (FALSE);
   WIN_ReleaseDeviceContext ();
#else /* _WINDOWS */
#ifdef _GTK

  int x, y, width, height, depth;

  /* Declaration of a DefaultDrawable useful for the creation of Pixmap and the
     initialization of GraphicContexts */
  DefaultWindow = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_widget_realize (DefaultWindow);
  DefaultDrawingarea = gtk_drawing_area_new();
  gtk_widget_set_parent (DefaultDrawingarea,DefaultWindow); 
  gtk_widget_realize (DefaultDrawingarea);
  DefaultDrawable = DefaultDrawingarea->window;
  gdk_window_get_geometry (DefaultDrawable,&x, &y, &width, &height, &depth);
  TtWDepth = depth; 
  TtCmap =  gdk_colormap_get_system ();

   InitDocColors (name);
   InitColors (name);
   InitGraphicContexts ();
   InitDialogueFonts (name);
#else /* _GTK */
   XSetErrorHandler (XWindowError);
   XSetIOErrorHandler (XWindowFatalError);
   TtScreen = DefaultScreen (TtDisplay);
   TtWDepth = DefaultDepth (TtDisplay, TtScreen);
   TtRootWindow = RootWindow (TtDisplay, TtScreen);
   TtCmap = XDefaultColormap (TtDisplay, TtScreen);

   InitDocColors (name);
   InitColors (name);
   InitGraphicContexts ();
   InitCurs ();
   InitDialogueFonts (name);

   /* Initialization of Picture Drivers */
   InitPictureHandlers (FALSE);
#endif /* _GTK */
#  endif /* _WINDOWS */
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
      FrRef[i] = 0;
#ifdef _WINDOWS
      FrMainRef[i] = 0;
#endif /* _WINDOWS */
    }
  PackBoxRoot = NULL;		/* Don't check enclosing for current boxes */
  DifferedPackBlocks = NULL;	/* Don't differ enclosing for current boxes */
  BoxCreating = FALSE;		/* No interractive creation yet */
  AnyWidthUpdate = FALSE;	/* No current width change */
  InitializeOtherThings ();
}

#ifndef _WINDOWS
/*----------------------------------------------------------------------
 *      SelectionEvents handle the X-Windows selection events.
 ----------------------------------------------------------------------*/
#ifdef __STDC__
void                SelectionEvents (void *ev)
#else  /* __STDC__ */
void                SelectionEvents (ev)
void               *ev;
#endif /* __STDC__ */
{
#ifndef _GTK
   XSelectionRequestEvent *request;
   XSelectionEvent     notify;
   ThotWindow          w, wind;
   Atom                type;
   int                 format, r, frame;
   unsigned long       nbitems, bytes_after;
   USTRING             buffer;
   XSelectionEvent    *event = (XSelectionEvent *) ev;

   switch (event->type)
	 {
	    case SelectionClear:
	       /* lost selection, need to free the buffer */
	       w = ((XSelectionClearEvent *) event)->window;
	       wind = 0;
	       frame = 0;
	       while (wind == 0 && frame <= MAX_FRAME)
		 {
		    if (w == FrRef[frame])
		       wind = w;
		    frame++;
		 }
	       if (w == wind && event->display == TtDisplay)
		 {
		    if (Xbuffer != NULL)
		      {
			 /* free the buffer */
			 free (Xbuffer);
			 Xbuffer = NULL;
			 ClipboardLength = 0;
		      }
		 }
	       break;

	    case SelectionNotify:
	       /* receive the XBuffer, paste it in the document */
	       /* verify that one frame is concerned by the action */
	       w = event->requestor;
	       wind = 0;
	       frame = 0;
	       while (wind == 0 && frame <= MAX_FRAME)
		 {
		    if (w == FrRef[frame])
		       wind = w;
		    frame++;
		 }
	       if (w == wind && event->display == TtDisplay)
		 {
		    if (event->property == None)
		      {
			 /* No current selection, look for the cut buffer */
			 buffer = (USTRING) XFetchBytes (TtDisplay, &r);
			 if (buffer != NULL)
			   {
			      /* returns the cut buffer */
			      if (ThotLocalActions[T_pasteclipboard] != NULL)
				 (*ThotLocalActions[T_pasteclipboard]) (buffer, r);
			   }
		      }
		    else
		      {
			USTRING partbuffer;
			/* receive the data */
			r = XGetWindowProperty (event->display, 
						event->requestor,
						event->property, 
						(long) 0, 
						(long) 256, 
						FALSE,
						AnyPropertyType, 
						&type, 
						&format, 
						&nbitems,
						&bytes_after, 
						&partbuffer);
			if (r == Success && type != None && format == 8)
			  {
			    if (bytes_after > 0)
			      {
				buffer = TtaAllocString (nbitems + bytes_after);
				ustrcpy (buffer, partbuffer);
				r = XGetWindowProperty (event->display, 
							event->requestor,
							event->property, 
							(long) 256, 
							(long) bytes_after, 
							FALSE,
							AnyPropertyType, 
							&type, 
							&format, 
							&nbitems,
							&bytes_after, 
							&partbuffer);
				ustrcpy (&buffer[256 * 4], partbuffer);
				nbitems = (256 * 4) + nbitems;
			      }
			    else
			      buffer = partbuffer;
			    /* paste the content of the selection */
			    if (ThotLocalActions[T_pasteclipboard] != NULL)
			      (*ThotLocalActions[T_pasteclipboard]) (buffer, (int) nbitems);
			    if (buffer != partbuffer)
			      TtaFreeMemory (buffer);
			  }
		      }
		 }
	       break;

	    case SelectionRequest:
	       /* Asking for selection : copy the cut buffer content in the Xbuffer */
	       w = ((XSelectionRequestEvent *) event)->owner;
	       wind = 0;
	       frame = 0;
	       while (wind == 0 && frame <= MAX_FRAME)
		 {
		    if (w == FrRef[frame])
		       wind = w;
		    frame++;
		 }
	       if (w == wind && event->display == TtDisplay)
		 {
		    request = (XSelectionRequestEvent *) event;
		    /* Build the Notify event */
		    notify.type = SelectionNotify;
		    notify.display = request->display;
		    notify.requestor = request->requestor;
		    notify.selection = request->selection;
		    notify.target = request->target;
		    notify.time = request->time;

		    if (Xbuffer == NULL)
		      {
			 /* selection is empty, so empty the cut buffer */
			 XStoreBuffer (request->display, NULL, 0, 0);
			 notify.property = None;
			 XSendEvent (request->display, request->requestor, TRUE, NoEventMask, (ThotEvent *) & notify);
		      }
		    else if (request->property == None)
		      {
			 /* there is no such property */
			 XStoreBuffer (request->display, Xbuffer, ClipboardLength, 0);
		      }
		    else
		      {
			 /* store the value in the given property */
			 XChangeProperty (request->display, request->requestor, request->property,
					  XA_STRING, 8, PropModeReplace, Xbuffer, ClipboardLength);
			 /* signal the completion of the action */
			 notify.property = request->property;
			 XSendEvent (request->display, request->requestor, TRUE, NoEventMask, (ThotEvent *) & notify);
		      }
		 }
	       break;
	 }
#endif /* _GTK */
}
#endif /* _WINDOWS */

