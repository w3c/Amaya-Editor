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
 * Author: I. Vatton (INRIA)
 *
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

ThotColorStruct     cblack;
static ThotColorStruct cwhite;

#include "appli_f.h"
#include "textcommands_f.h"
#include "editcommands_f.h"
#include "checkermenu_f.h"
#include "font_f.h"
#include "picture_f.h"
#include "inites_f.h"
#include "memory_f.h"
#include "registry_f.h"

#ifdef _WINDOWS
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
   ThotGC              gc = (ThotGC) TtaGetMemory (sizeof (WIN_GC_BLK));

   return (gc);
}

/*----------------------------------------------------------------------
 *      WinLoadGC has to be called before using an GC X-Windows
 *         emulation under MS-Windows.
 *   Full description of Device Context Attributes : Petzolt p 102
 ----------------------------------------------------------------------*/
#ifdef __STDC__
void                WinLoadGC (HDC hdc, ThotGC gc)
#else  /* __STDC__ */
void                WinLoadGC (hdc, gc)
HDC                 hdc;
ThotGC              gc;

#endif /* __STDC__ */
{
   if (gc->capabilities & THOT_GC_PEN)
      SelectObject (hdc, gc->pen);

   if (gc->capabilities & THOT_GC_FOREGROUND)
      SetTextColor (hdc, gc->foreground);

   if (gc->capabilities & THOT_GC_BACKGROUND) {
	SetBkMode (hdc, OPAQUE);
	SetBkColor (hdc, gc->background);
   } else 
	SetBkMode (hdc, TRANSPARENT);

   if (gc->capabilities & THOT_GC_FUNCTION)
      SetROP2 (hdc, gc->mode);
}

/*----------------------------------------------------------------------
 *      WinUnloadGC has to be called after using an GC X-Windows
 *         emulation under MS-Windows.
 ----------------------------------------------------------------------*/
#ifdef __STDC__
void                WinUnloadGC (HDC hdc, ThotGC gc)
#else  /* __STDC__ */
void                WinUnloadGC (gc)
HDC                 hdc;
ThotGC              gc;

#endif /* __STDC__ */
{
}

/*----------------------------------------------------------------------
 *      WinInitColors initialize the color table depending on the
 *         device capabilities under MS-Windows.
 ----------------------------------------------------------------------*/
void                WinInitColors (void)
{
   int                 i;
   LOGPALETTE         *ptrLogPal;
   static int          initialized = 0;

   if (initialized)
      return;

   fprintf (stderr, "WinInitColors\n");

   WIN_GetDeviceContext (-1);

   /*
    * Create initialize and install a color palette for
    * the Thot set of colors.
    */
   ptrLogPal = (LOGPALETTE *) TtaGetMemory (sizeof (LOGPALETTE) + MAX_COLOR * sizeof (PALETTEENTRY));
   ptrLogPal->palVersion             = 0x300;
   ptrLogPal->palNumEntries          = MAX_COLOR;
   ptrLogPal->palPalEntry[0].peRed   = 255;
   ptrLogPal->palPalEntry[0].peGreen = 255;
   ptrLogPal->palPalEntry[0].peBlue  = 255;
   ptrLogPal->palPalEntry[0].peFlags = 0;
   ptrLogPal->palPalEntry[1].peRed   = 0;
   ptrLogPal->palPalEntry[1].peGreen = 0;
   ptrLogPal->palPalEntry[1].peBlue  = 0;
   ptrLogPal->palPalEntry[1].peFlags = 0;

   for (i = 2; i < MAX_COLOR; i++) {
       ptrLogPal->palPalEntry[i].peRed = RGB_Table[i].red;
       ptrLogPal->palPalEntry[i].peGreen = RGB_Table[i].green;
       ptrLogPal->palPalEntry[i].peBlue = RGB_Table[i].blue;
       ptrLogPal->palPalEntry[i].peFlags = 0;
   }

   TtCmap = CreatePalette (ptrLogPal);
   if (TtCmap == NULL) {
      fprintf (stderr, "couldn't CreatePalette\n");
      WinErrorBox ();
   } else
        SelectPalette (WIN_curHdc, TtCmap, TRUE);

   TtaFreeMemory (ptrLogPal);

   /* fill-in the Pix_Color table */
   for (i = 0; i < (sizeof (Pix_Color) / sizeof (Pix_Color[0])); i++)
       Pix_Color[i] = PALETTERGB (RGB_Table[i].red, RGB_Table[i].green, RGB_Table[i].blue);
    /*
      Pix_Color[i] = GetNearestColor(WIN_curHdc,
      RGB(RGB_Table[i].red, RGB_Table[i].green, RGB_Table[i].blue));
      */

   /* initialize some standard colors. */
   Black_Color     = GetNearestColor (WIN_curHdc, PALETTERGB (0, 0, 0));
   White_Color     = GetNearestColor (WIN_curHdc, PALETTERGB (255, 255, 255));
   Scroll_Color    = GetNearestColor (WIN_curHdc, PALETTERGB (190, 190, 190));
   Button_Color    = GetNearestColor (WIN_curHdc, PALETTERGB (190, 190, 190));
   Select_Color    = GetNearestColor (WIN_curHdc, PALETTERGB (70, 130, 180));
   BgMenu_Color    = GetNearestColor (WIN_curHdc, PALETTERGB (190, 190, 190));
   Box_Color       = GetNearestColor (WIN_curHdc, PALETTERGB (255, 0, 0));
   RO_Color        = GetNearestColor (WIN_curHdc, PALETTERGB (0, 0, 205));
   InactiveB_Color = GetNearestColor (WIN_curHdc, PALETTERGB (255, 0, 0));

   /* set up the default background colors for all views. */
   for (i = 0; i < (sizeof (BackgroundColor) / sizeof (BackgroundColor[0])); i++)
       BackgroundColor[i] = White_Color;

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
   char                msg[200];

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

   perror ("*** Fatal Error");
   if (errno != EPIPE)
      TtaDisplayMessage (FATAL, TtaGetMessage (LIB, TMSG_LIB_X11_ERR), DisplayString (dpy));
   else
      TtaDisplayMessage (FATAL, TtaGetMessage (LIB, TMSG_LIB_X11_ERR), DisplayString (dpy));
   return (0);
}
#endif /* _WINDOWS */

/*----------------------------------------------------------------------
 * TtaGetThotColor returns the Thot Color.
 *            red, green, blue express the color RGB in 8 bits values
 ----------------------------------------------------------------------*/
#ifdef __STDC__
int                 TtaGetThotColor (unsigned short red, unsigned short green, unsigned short blue)
#else  /* __STDC__ */
int                 TtaGetThotColor (red, green, blue)
unsigned short      red;
unsigned short      green;
unsigned short      blue;
#endif /* __STDC__ */
{
   short               delred, delgreen, delblue;
   int                 i;
   unsigned int        dsquare;
   unsigned int        best_dsquare = (unsigned int) -1;
   int                 best;
   int                 maxcolor;

   /*
    * lookup for the color number among the color set allocated
    * by the application.
    * The lookup is based on a closest in cube algorithm hence
    * we try to get the closest color available for the display.
    */
   maxcolor = NumberOfColors ();
   best = 0;			/* best color in list not found */
   for (i = 0; i < maxcolor; i++)
     {
	TtaGiveThotRGB (i, &delred, &delgreen, &delblue);
	/* delred <<= 8; delgreen <<= 8; delblue <<= 8; */
	delred -= red;
	delgreen -= green;
	delblue -= blue;
	dsquare = delred * delred + delgreen * delgreen + delblue * delblue;
	if (dsquare < best_dsquare)
	  {
	     best = i;
	     best_dsquare = dsquare;
	  }
     }
   return (best);
}


/*----------------------------------------------------------------------
 *   TtaGiveRGB returns the RGB of the color.
 ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaGiveRGB (char *colname, unsigned short *red, unsigned short *green, unsigned short *blue)
#else  /* __STDC__ */
void                TtaGiveRGB (colname, red, green, blue)
char               *colname;
unsigned short     *red;
unsigned short     *green;
unsigned short     *blue;

#endif /* __STDC__ */
{
   int                 i, maxcolor;
   ThotColorStruct     color;

   /*
    * Lookup the color name in the application color name database
    */
   maxcolor = NumberOfColors ();
   for (i = 0; i < maxcolor; i++)
       if (!strcasecmp (ColorName (i), colname))
	  TtaGiveThotRGB (i, red, green, blue);

#ifndef _WINDOWS
   /* Lookup the color name in the X color name database */
   if (XParseColor (TtDisplay, TtCmap, colname, &color))
     {
	/* normalize RGB color values to 8 bits values */
	color.red >>= 8;
	color.green >>= 8;
	color.blue >>= 8;
	*red = color.red;
	*green = color.green;
	*blue = color.blue;
     }
#endif /* _WINDOWS */
}


/*----------------------------------------------------------------------
 *      FindColor looks for the named color ressource.
 *         The result is the closest color found the Thot color table.
 ----------------------------------------------------------------------*/
#ifdef __STDC__
static boolean      FindColor (int disp, char *name, char *colorplace, char *defaultcolor, unsigned long *colorpixel)
#else  /* __STDC__ */
static boolean      FindColor (disp, name, colorplace, defaultcolor, colorpixel)
int                 disp;
char               *name;
char               *colorplace;
char               *defaultcolor;
unsigned long      *colorpixel;

#endif /* __STDC__ */
{
   int                 col;
   char               *value;
   unsigned short      red;
   unsigned short      green;
   unsigned short      blue;

   value = (char *) TtaGetEnvString (colorplace);
   /* faut-il prendre la valeur par defaut ? */
   if (value == NULL && defaultcolor != NULL)
       value = defaultcolor;

   if (value != NULL)
     {
	TtaGiveRGB (value, &red, &green, &blue);
	col = TtaGetThotColor (red, green, blue);
	/* register the default background color */
	if (strcmp (colorplace, "BackgroundColor") == 0)
	   DefaultBColor = col;
	*colorpixel = ColorPixel (col);
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
 *      InitColors initialize the Thot predefined X-Window colors.
 ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         InitColors (char *name)
#else  /* __STDC__ */
static void         InitColors (name)
char               *name;

#endif /* __STDC__ */
{
   boolean             found;
#ifndef _WINDOWS
   ThotColorStruct     col;

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
   Black_Color = cblack.pixel;
   Button_Color = Select_Color = cblack.pixel;
   White_Color = cwhite.pixel;
   Scroll_Color = BgMenu_Color = cwhite.pixel;
#endif /* _WINDOWS */

   if (TtWDepth > 1)
     {
	/* background color */
	found = FindColor (0, name, "BackgroundColor", "gainsboro", &White_Color);
	/* drawing color */
	found = FindColor (0, name, "ForegroundColor", "black", &Black_Color);
	/* scrolls color */
	found = FindColor (0, name, "ScrollColor", "grey", &Scroll_Color);
	/* color for the selection menu */
	found = FindColor (0, name, "MenuBgColor", "grey", &BgMenu_Color);
	/* olor for the selection */
	found = FindColor (0, name, "DocSelectColor", "SteelBlue", &Select_Color);
	/* color for borders and buttons */
	found = FindColor (0, name, "ButtonColor", "grey", &Button_Color);
	/* color for incative options and buttons */
	found = FindColor (0, name, "InactiveItemColor", "LightGrey", &InactiveB_Color);
     }
   else
     {
      /* at least allocate the selection color */
      found = FindColor (0, name, "DocSelectColor", "White", &Select_Color);

      /* color for borders and buttons */
      found = FindColor (0, name, "ButtonColor", "White", &Button_Color);
      }
   /* The reference color */
   found = FindColor (0, name, "ActiveBoxColor", "Red", &(Box_Color));
#ifndef _WINDOWS
   if (!found)
      Box_Color = cwhite.pixel;
   else if (TtWDepth == 1)
      Box_Color = cblack.pixel;

   /* color for read-only sections */
   found = FindColor (0, name, "ReadOnlyColor", "MediumBlue", &(RO_Color));
   if (!found)
      RO_Color = cwhite.pixel;
   else if (TtWDepth == 1)
      RO_Color = cblack.pixel;
#else  /* _WINDOWS */
   WIN_curHdc = GetDC (WIN_Main_Wd) ;
   WinInitColors ();
#endif /* _WINDOWS */
}


/*----------------------------------------------------------------------
 *  ShowReference returns True if there exists a color for active boxes.
 ----------------------------------------------------------------------*/
boolean             ShowReference ()
{
#ifndef _WINDOWS
   if (Box_Color == cwhite.pixel)
      return (FALSE);
   else
      return (TRUE);
#endif /* _WINDOWS */
}


/*----------------------------------------------------------------------
 *  ShowReference returns True if there exists a color for read-only parts.
 ----------------------------------------------------------------------*/
boolean             ShowReadOnly ()
{
#ifndef _WINDOWS
   if (RO_Color == cwhite.pixel)
      return (FALSE);
   else
      return (TRUE);
#endif /* _WINDOWS */
}
/*fin */

/*----------------------------------------------------------------------
 *      InitGraphicContexts initialize the X-Windows graphic contexts and their Windows
 *	counterpart in Microsoft environment.
 ----------------------------------------------------------------------*/
static void         InitGraphicContexts ()
{
#ifndef _WINDOWS
   unsigned long       valuemask;
   unsigned long       white;
   unsigned long       black;
   XGCValues           GCmodel;
   Pixmap              pix;

#endif /* _WINDOWS */

   /*
    * Create a Graphic Context to write white on black.
    */
#ifndef _WINDOWS
   valuemask = GCForeground | GCBackground | GCFunction;
   white = ColorNumber ("White");
   black = ColorNumber ("Black");

   GCmodel.function = GXcopy;
   pix = CreatePattern (0, 0, 0, black, white, 6);	/* !!!! */
   GCmodel.foreground = White_Color;
   GCmodel.background = Black_Color;
   TtWhiteGC = XCreateGC (TtDisplay, TtRootWindow, valuemask, &GCmodel);
#endif /* _WINDOWS */
#ifdef _WINDOWS
   TtWhiteGC.capabilities = THOT_GC_PEN | THOT_GC_FOREGROUND | THOT_GC_BACKGROUND;
   TtWhiteGC.pen = GetStockObject (WHITE_PEN);
   TtWhiteGC.background = Black_Color;
   TtWhiteGC.foreground = White_Color;
#endif /* _WINDOWS */

   /*
    * Create a Graphic Context to write black on white.
    */
#ifndef _WINDOWS
   GCmodel.foreground = Button_Color;
   GCmodel.background = White_Color;
   TtBlackGC = XCreateGC (TtDisplay, TtRootWindow, valuemask, &GCmodel);
#endif /* _WINDOWS */
#ifdef _WINDOWS
   TtBlackGC.capabilities = THOT_GC_PEN | THOT_GC_FOREGROUND | THOT_GC_BACKGROUND;
   TtBlackGC.pen = GetStockObject (BLACK_PEN);
   TtBlackGC.background = White_Color;
   TtBlackGC.foreground = Black_Color;
#endif /* _WINDOWS */

   /*
    * Create a Graphic Context to write black on white,
    * but with a specific 10101010 pattern.
    */
#ifndef _WINDOWS
   GCmodel.foreground = Black_Color;
   GCmodel.background = White_Color;
   TtLineGC = XCreateGC (TtDisplay, TtRootWindow, valuemask, &GCmodel);
   XSetTile (TtDisplay, TtLineGC, pix);
#endif /* _WINDOWS */
#ifdef _WINDOWS
   TtLineGC.capabilities = THOT_GC_FOREGROUND |
   /* THOT_GC_BACKGROUND | THOT_GC_BRUSH | */ THOT_GC_PEN;
   TtLineGC.pen = GetStockObject (BLACK_PEN);
   /* TtLineGC.background = White_Color; */
   TtLineGC.foreground = Black_Color;
   /* !!!! WIN_LastBitmap created by pix = CreatePattern(...); */
   /* TtLineGC.brush = CreatePatternBrush(WIN_LastBitmap); */
#endif /* _WINDOWS */

   /*
    * Another Graphic Context to write black on white, for dialogs.
    */
#ifndef _WINDOWS
   TtDialogueGC = XCreateGC (TtDisplay, TtRootWindow, valuemask, &GCmodel);
#endif /* _WINDOWS */
#ifdef _WINDOWS
   TtDialogueGC.capabilities = THOT_GC_FOREGROUND | THOT_GC_BACKGROUND | THOT_GC_PEN;
   TtDialogueGC.pen = GetStockObject (BLACK_PEN);
   TtDialogueGC.background = White_Color;
   TtDialogueGC.foreground = Black_Color;
#endif /* _WINDOWS */

   /*
    * A Graphic Context to show selected objects. On X-Windows,
    * the colormap indexes are XORed to show the object without
    * destroying the colors : XOR.XOR = I ...
    */
#ifndef _WINDOWS
   GCmodel.function = GXinvert;
   GCmodel.plane_mask = Select_Color;
   if (TtWDepth > 1)
      GCmodel.foreground = Black_Color;
   else
      GCmodel.foreground = Select_Color;
   GCmodel.background = White_Color;
   TtInvertGC = XCreateGC (TtDisplay, TtRootWindow, valuemask | GCPlaneMask, &GCmodel);
#endif /* _WINDOWS */
#ifdef _WINDOWS
   TtDialogueGC.capabilities = THOT_GC_FOREGROUND | THOT_GC_BACKGROUND |
      THOT_GC_FUNCTION;
   TtDialogueGC.mode = R2_XORPEN;
   if (TtWDepth > 1)
      TtDialogueGC.foreground = Black_Color;
   else
      TtDialogueGC.foreground = Select_Color;
   TtDialogueGC.background = White_Color;
#endif /* _WINDOWS */

   /*
    * A Graphic Context for trame objects.
    */
#ifndef _WINDOWS
   GCmodel.function = GXcopy;
   GCmodel.foreground = Black_Color;
   TtGreyGC = XCreateGC (TtDisplay, TtRootWindow, valuemask, &GCmodel);
   XSetFillStyle (TtDisplay, TtGreyGC, FillTiled);
   XFreePixmap (TtDisplay, pix);
#endif /* _WINDOWS */
#ifdef _WINDOWS
   TtGreyGC.capabilities = THOT_GC_FOREGROUND | THOT_GC_BACKGROUND |
   /* THOT_GC_BRUSH | */ THOT_GC_PEN |
      THOT_GC_FUNCTION;
   TtBlackGC.background = White_Color;
   TtBlackGC.foreground = Black_Color;
   TtBlackGC.mode = R2_XORPEN;
   /* !!!! WIN_LastBitmap created by pix = CreatePattern(...); */
   /* TtBlackGC.brush = CreatePatternBrush(WIN_LastBitmap); */
   DeleteObject (WIN_LastBitmap);
   WIN_LastBitmap = 0;
#endif
}


/*----------------------------------------------------------------------
 *      ThotInitDisplay initialize all the output settings.
 ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ThotInitDisplay (char *name, int dx, int dy)
#else  /* __STDC__ */
void                ThotInitDisplay (name, dx, dy)
char               *name;
int                 dx;
int                 dy;

#endif /* __STDC__ */
{
#ifdef _WINDOWS
   HDC                 hdc;

   hdc = GetDC (WIN_Main_Wd);
   TtWDepth = GetDeviceCaps (hdc, PLANES);
   if (TtWDepth == 1)
      TtWDepth = GetDeviceCaps (hdc, BITSPIXEL);

   ReleaseDC (WIN_Main_Wd, hdc);
#endif /* _WINDOWS */

#ifndef _WINDOWS
   XSetErrorHandler (XWindowError);
   XSetIOErrorHandler (XWindowFatalError);
   TtScreen = DefaultScreen (TtDisplay);
   TtWDepth = DefaultDepth (TtDisplay, TtScreen);
   TtRootWindow = RootWindow (TtDisplay, TtScreen);
   TtCmap = XDefaultColormap (TtDisplay, TtScreen);
#endif

   InitDocColors (name);
   InitColors (name);
   InitGraphicContexts ();
   InitCurs ();
   InitDialogueFonts (name);

   /* Initialization of Picture Drivers */
   InitPictureHandlers (FALSE);
}

/*----------------------------------------------------------------------
 *      InitDocContexts initialize the frames' contexts
 ----------------------------------------------------------------------*/
void                InitDocContexts ()
{
   int                 i;

   /* Initialize the memory allocator */
   InitKernelMemory ();
   /* Initialisation de la table des frames */
   for (i = 0; i <= MAX_FRAME; i++)
       FrRef[i] = 0;
   PackBoxRoot = NULL;		/* Don't do englobing placement for current boxes */
   DifferedPackBlocks = NULL;	/* Don't differ englobing placement for current boxes */
   BoxCreating = FALSE;		/* no interractive creation yet */
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
   XSelectionRequestEvent *request;
   XSelectionEvent     notify;
   ThotWindow          w, wind;
   Atom                type;
   int                 format, r, frame;
   unsigned long       nbitems, bytes_after;
   unsigned char      *buffer;
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
			 r = XGetWindowProperty (event->display, event->requestor,
			       event->property, (long) 0, (long) 256, FALSE,
				  AnyPropertyType, &type, &format, &nbitems,
						 &bytes_after, &buffer);
			 if (r == Success && type != None && format == 8)
			   {
			      /* paste the content of the selection */
			      if (ThotLocalActions[T_pasteclipboard] != NULL)
				 (*ThotLocalActions[T_pasteclipboard]) (buffer, (int) nbitems);
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
#endif /* _WINDOWS */
