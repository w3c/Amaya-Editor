/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2003
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
#ifdef _WINDOWS
#include "wininclude.h"
#endif /* _WINDOWS */


#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "frame_tv.h"
#include "boxes_tv.h"
#include "thotcolor_tv.h"
#include "appdialogue_tv.h"
#include "units_tv.h"

ThotColorStruct  cblack;
static ThotColorStruct  cwhite;
extern int              errno;

#include "appli_f.h"
#include "checkermenu_f.h"
#include "editcommands_f.h"
#include "font_f.h"
#include "inites_f.h"
#include "memory_f.h"
#include "picture_f.h"
#include "registry_f.h"
#include "textcommands_f.h"

#ifndef _WINDOWS
#ifndef _GTK
/*----------------------------------------------------------------------
 * XWindowError is the X-Windows non-fatal errors handler.
 ----------------------------------------------------------------------*/
static int XWindowError (Display *dpy, XErrorEvent *err)
{
   char                msg[200];

   XGetErrorText (dpy, err->error_code, msg, 200);
   return (0);
}

/*----------------------------------------------------------------------
 * XWindowFatalError is the X-Windows fatal errors handler.
 ----------------------------------------------------------------------*/
static int XWindowFatalError (Display * dpy)
{
  if (errno != EPIPE)
    TtaDisplayMessage (FATAL, TtaGetMessage (LIB, TMSG_LIB_X11_ERR),
		       DisplayString (dpy));
  else
    TtaDisplayMessage (FATAL, TtaGetMessage (LIB, TMSG_LIB_X11_ERR),
		       DisplayString (dpy));
  CloseTextInsertion ();
  if (ThotLocalActions[T_backuponfatal] != NULL)
    (*ThotLocalActions[T_backuponfatal]) ();
  return (0);
}
#endif /* !_GTK */
#endif /* _WINDOWS */


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
#ifdef _WINDOWS 
   *colorpixel = col;
#else  /* _WINDOWS */
   *colorpixel = ColorPixel (col);
#endif /* _WINDOWS */
   return (TRUE);

}

/*----------------------------------------------------------------------
 *      InitCurs load the cursors used by the graphic interface.
 ----------------------------------------------------------------------*/
static void         InitCurs ()
{
#ifndef _WINDOWS
#ifndef _GTK
   WindowCurs = XCreateFontCursor (TtDisplay, XC_hand2);
   VCurs = XCreateFontCursor (TtDisplay, XC_sb_v_double_arrow);
   HCurs = XCreateFontCursor (TtDisplay, XC_sb_h_double_arrow);
   HVCurs = XCreateFontCursor (TtDisplay, /*XC_fleur*/XC_plus);
   WaitCurs = XCreateFontCursor (TtDisplay, XC_watch);
#else /* !_GTK */
   ArrowCurs = gdk_cursor_new (GDK_LEFT_PTR);
   WindowCurs = gdk_cursor_new (GDK_HAND2);
   VCurs = gdk_cursor_new (GDK_SB_V_DOUBLE_ARROW);
   HCurs = gdk_cursor_new (GDK_SB_H_DOUBLE_ARROW);
   HVCurs = gdk_cursor_new (GDK_PLUS);
   WaitCurs = gdk_cursor_new (GDK_CLOCK);
#endif /* _GTK */
#endif /* _WINDOWS */
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
#ifndef _WINDOWS
#ifdef _GTK
   GdkVisual          *vptr;
   GdkVisualType       vinfo;
#else /* _GTK */
   XVisualInfo        *vptr;
   XVisualInfo         vinfo;
   ThotColorStruct     col;
   int                 i;
#endif /* _GTK */

#ifdef _GTK
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
   Black_Color = Box_Color = RO_Color = cblack.pixel;
   FgMenu_Color = cblack.pixel;
   White_Color  = cwhite.pixel;
   Scroll_Color = BgMenu_Color = cwhite.pixel;
#endif /* _GTK */
#endif /* _WINDOWS */
   if (TtWDepth > 1)
     TtaUpdateEditorColors ();
   else
     {
     /* at least allocate the selection color */
       found = FindColor (0, name, "FgSelectColor", "White", &White_Color);
       found = FindColor (0, name, "BgSelectColor", "Black", &Black_Color);
     }
}

#ifndef _WINDOWS
/*----------------------------------------------------------------------
  InitGraphicContexts initialize the X-Windows graphic contexts and their
  Windows counterpart in Microsoft environment.
 ----------------------------------------------------------------------*/
static void InitGraphicContexts (void)
{
#ifndef _GTK
  unsigned long       valuemask;
  XGCValues           GCmodel;
#endif /* _GTK */
  int                 white;
  int                 black;
  Pixmap              pix;

#ifdef _GTK
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


#else /* _GTK */
   valuemask = GCForeground | GCBackground | GCFunction;
   white = ColorNumber ("White");
   black = ColorNumber ("Black");
   pix = CreatePattern (0, black, white, 6);

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
   GCmodel.plane_mask = Black_Color;
   GCmodel.foreground = Black_Color;
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
}
#endif /* _WINDOWS */


/*----------------------------------------------------------------------
 *      ThotInitDisplay initialize all the output settings.
 ----------------------------------------------------------------------*/
void ThotInitDisplay (char* name, int dx, int dy)
{
#ifdef _WINDOWS
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

	iDibSize = sizeof( BITMAPINFOHEADER ) + 256 * sizeof ( DWORD );
	pDib = (LPBITMAPINFOHEADER) malloc( iDibSize );
    memset( pDib, 0, iDibSize );
        
    /* Use old-style BMIH for compatibility, and leave biBitCount zero */
    pDib->biSize = sizeof( BITMAPINFOHEADER );
        
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
    free( pDib );
   }

   InitDocColors (name);
   InitColors (name);   
   if (WIN_LastBitmap)
     DeleteObject (WIN_LastBitmap);
   WIN_LastBitmap = 0;
   InitCurs ();
   DOT_PER_INCH = GetDeviceCaps(TtDisplay, LOGPIXELSY);
   InitDialogueFonts (name);

   /* Initialization of Picture Drivers */
   InitPictureHandlers (FALSE);
   WIN_ReleaseDeviceContext ();
#else /* _WINDOWS */
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
  PackRows = TRUE;              /* Pack row elements */
  BoxCreating = FALSE;		/* No interractive creation yet */
  AnyWidthUpdate = FALSE;	/* No current width change */
  InitializeOtherThings ();
}

#ifndef _WINDOWS

#ifndef _GTK
/*----------------------------------------------------------------------
 *      SelectionEvents handle the X-Windows selection events.
 ----------------------------------------------------------------------*/
void SelectionEvents (void *ev)
{
   XSelectionRequestEvent *request;
   XSelectionEvent     notify;
   ThotWindow          w, wind;
   Atom                type;
   XSelectionEvent    *event = (XSelectionEvent *) ev;
   unsigned long       nbitems, bytes_after;
   unsigned char      *buffer;
   unsigned char      *partbuffer;
   int                 format, r, frame;

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
	       buffer = (unsigned char *) XFetchBytes (TtDisplay, &r);
	       if (buffer != NULL)
		 {
		   /* returns the cut buffer */
		   if (ThotLocalActions[T_pasteclipboard] != NULL)
		     (*ThotLocalActions[T_pasteclipboard]) (buffer, r);
		 }
	     }
	   else
	     {
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
		       buffer = TtaGetMemory (nbitems + bytes_after);
		       strcpy (buffer, partbuffer);
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
		       strcpy (&buffer[256 * 4], partbuffer);
		       nbitems = (256 * 4) + nbitems;
		     }
		   else
		     buffer = partbuffer;
		   /* paste the content of the selection */
		   if (ThotLocalActions[T_pasteclipboard])
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
}
#endif /* _GTK */
#endif /* _WINDOWS */

