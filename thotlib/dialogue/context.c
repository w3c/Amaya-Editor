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
 *         R. Guetari (W3C/INRIA) Windows 95/NT routines.
 */

#include "ustring.h"
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

ThotColorStruct         cblack;
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

#ifdef _WINDOWS
#include "wininclude.h"

static int   palSize;
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
 *      WinLoadGC has to be called before using an GC X-Windows
 *         emulation under MS-Windows.
 *   Full description of Device Context Attributes : Petzolt p 102
 ----------------------------------------------------------------------*/
#ifdef __STDC__
void WinLoadGC (HDC hDC, int fg, int RO)
#else  /* __STDC__ */
void WinLoadGC (hDC, fg, RO)
HDC hDC;
int fg;
int RO;
#endif /* __STDC__ */
{
   
   if (TtLineGC.capabilities & THOT_GC_PEN) {
	  if (RO && fg == 1)
         TtLineGC.foreground = RO_Color;
      else
	      TtLineGC.foreground = fg;
   }

   if (TtLineGC.capabilities & THOT_GC_FOREGROUND)
      if (RO && fg == 1)
         SetTextColor (hDC, ColorPixel (RO_Color));
      else         
         SetTextColor (hDC, ColorPixel (fg));

   if (TtLineGC.capabilities & THOT_GC_BACKGROUND) {
      SetBkMode (hDC, OPAQUE);
      SetBkColor (hDC, TtLineGC.background);
   } else 
         SetBkMode (hDC, TRANSPARENT);
/*
   if (TtLineGC.capabilities & THOT_GC_FUNCTION)
      SetROP2 (TtDisplay, TtLineGC.mode);
	  */
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
   static int initialized = 0;

   if (initialized)
      return;

   WIN_GetDeviceContext (-1);
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
   WIN_ReleaseDeviceContext ();

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
void                TtaGiveRGB (STRING colname, unsigned short *red, unsigned short *green, unsigned short *blue)
#else  /* __STDC__ */
void                TtaGiveRGB (colname, red, green, blue)
STRING              colname;
unsigned short     *red;
unsigned short     *green;
unsigned short     *blue;

#endif /* __STDC__ */
{
   int                 i;
#ifndef _WINDOWS 
   ThotColorStruct     color;

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
   else
     {
#endif /* _WINDOWS */
       /* Lookup the color name in the application color name database */
       for (i = 0; i < NColors; i++)
	 if (!ustrcasecmp (ColorName (i), colname))
	   {
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
static boolean      FindColor (int disp, STRING name, STRING colorplace, STRING defaultcolor, ThotColor *colorpixel)
#else  /* __STDC__ */
static boolean      FindColor (disp, name, colorplace, defaultcolor, colorpixel)
int        disp;
STRING      name;
STRING      colorplace;
STRING      defaultcolor;
ThotColor* colorpixel;

#endif /* __STDC__ */
{
   int                 col;
   STRING              value;
   unsigned short      red;
   unsigned short      green;
   unsigned short      blue;

   value = TtaGetEnvString (colorplace);
   /* do you need to take the default color? */
   if (value == NULL && defaultcolor != NULL)
       value = defaultcolor;

   if (value != NULL)
     {
	TtaGiveRGB (value, &red, &green, &blue);
	col = TtaGetThotColor (red, green, blue);
	/* register the default background color */
	if (ustrcmp (colorplace, "BackgroundColor") == 0)
	   DefaultBColor = col;
	/* register the default background color */
	else if (ustrcmp (colorplace, "ForegroundColor") == 0)
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
  STRING app_name;
  boolean found;

  app_name =  TtaGetEnvString ("appname");

  /* background color */
#       ifndef _WINDOWS
  found = FindColor (0, app_name, "BackgroundColor", "gainsboro",
		     &White_Color);
#       else  /* _WINDOWS */
  found = FindColor (0, app_name, "BackgroundColor", "LightGrey1", 
		     &White_Color);
#       endif /* _WINDOWS */
  /* drawing color */
  found = FindColor (0, app_name, "ForegroundColor", "Black", &Black_Color);
  /* color for the menu background */
  found = FindColor (0, app_name, "MenuBgColor", "Grey", &BgMenu_Color);
  /* color for the menu foregroundground */
  found = FindColor (0, app_name, "MenuFgColor", "Black", &FgMenu_Color);
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
static void         InitColors (STRING name)
#else  /* __STDC__ */
static void         InitColors (name)
STRING              name;

#endif /* __STDC__ */
{
   boolean             found;
#ifndef _WINDOWS
   XVisualInfo        *vptr;
   XVisualInfo         vinfo;
   ThotColorStruct     col;
   int                 i;

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
#endif /* _WINDOWS */

   if (TtWDepth > 1)
     {
	/* background color */
#       ifndef _WINDOWS
	found = FindColor (0, name, "BackgroundColor", "gainsboro", &White_Color);
#       else  /* _WINDOWS */
	found = FindColor (0, name, "BackgroundColor", "LightGrey1", &White_Color);
#       endif /* _WINDOWS */
	/* drawing color */
	found = FindColor (0, name, "ForegroundColor", "Black", &Black_Color);
	/* color for the menu background */
	found = FindColor (0, name, "MenuBgColor", "Grey", &BgMenu_Color);
	/* color for the menu foregroundground */
	found = FindColor (0, name, "MenuFgColor", "Black", &FgMenu_Color);
	/* scrolls color */
	Scroll_Color = BgMenu_Color;
	/* color for the selection */
#       ifndef _WINDOWS
	found = FindColor (0, name, "DocSelectColor", "SteelBlue", &Select_Color);
#       else  /* _WINDOWS */
	found = FindColor (0, name, "DocSelectColor", "Blue", &Select_Color);
#       endif /* _WINDOWS */
	/* color for borders and buttons */
#       ifndef _WINDOWS
	found = FindColor (0, name, "InactiveItemColor", "LightGrey", &InactiveB_Color);
#       else  /* _WINDOWS */
	found = FindColor (0, name, "InactiveItemColor", "LightGrey1", &InactiveB_Color);
#       endif /* _WINDOWS */
     }
   else
      /* at least allocate the selection color */
      found = FindColor (0, name, "DocSelectColor", "White", &Select_Color);

   /* The reference color */
   found = FindColor (0, name, "ActiveBoxColor", "Red", &(Box_Color));
#  ifndef _WINDOWS
   if (!found)
      Box_Color = cwhite.pixel;
   else if (TtWDepth == 1)
      Box_Color = cblack.pixel;
#  endif /* !_WINDOWS */

   /* color for read-only sections */
   found = FindColor (0, name, "ReadOnlyColor", "Black", &(RO_Color));
#  ifndef _WINDOWS 
   if (!found)
      RO_Color = cwhite.pixel;
   else if (TtWDepth == 1)
      RO_Color = cblack.pixel;
#  else  /* _WINDOWS */
   /* TtDisplay = GetDC (WIN_Main_Wd) ; */
   WinInitColors ();
#  endif /* _WINDOWS */
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
#ifdef __STDC__
static void InitGraphicContexts (void)
#else /* __STDC__ */
static void InitGraphicContexts ()
#endif /* __STDC__ */
{
#  ifndef _WINDOWS
   unsigned long       valuemask;
   unsigned long       white;
   unsigned long       black;
   XGCValues           GCmodel;
   Pixmap              pix;
#  endif /* _WINDOWS */

   /* Create a Graphic Context to write white on black. */
#  ifndef _WINDOWS
   valuemask = GCForeground | GCBackground | GCFunction;
   white = ColorNumber ("White");
   black = ColorNumber ("Black");

   GCmodel.function = GXcopy;
   pix = CreatePattern (0, 0, 0, black, white, 6);	/* !!!! */
   GCmodel.foreground = White_Color;
   GCmodel.background = Black_Color;
   TtWhiteGC = XCreateGC (TtDisplay, TtRootWindow, valuemask, &GCmodel);
#  endif /* _WINDOWS */
#  ifdef _WINDOWS
   /*
   TtWhiteGC.capabilities = THOT_GC_PEN | THOT_GC_FOREGROUND | THOT_GC_BACKGROUND;
   TtWhiteGC.pen = GetStockObject (WHITE_PEN);
   TtWhiteGC.background = Black_Color;
   TtWhiteGC.foreground = White_Color;
   */
#  endif /* _WINDOWS */

   /* Create a Graphic Context to write black on white. */
#  ifndef _WINDOWS
   GCmodel.foreground = Black_Color;
   GCmodel.background = White_Color;
   TtBlackGC = XCreateGC (TtDisplay, TtRootWindow, valuemask, &GCmodel);
#  endif /* _WINDOWS */
#  ifdef _WINDOWS
   /*
   TtBlackGC.capabilities = THOT_GC_PEN | THOT_GC_FOREGROUND | THOT_GC_BACKGROUND;
   TtBlackGC.pen = GetStockObject (BLACK_PEN);
   TtBlackGC.background = White_Color;
   TtBlackGC.foreground = Black_Color;
   */
#  endif /* _WINDOWS */

   /*
    * Create a Graphic Context to write black on white,
    * but with a specific 10101010 pattern.
    */
#  ifndef _WINDOWS
   GCmodel.foreground = Black_Color;
   GCmodel.background = White_Color;
   TtLineGC = XCreateGC (TtDisplay, TtRootWindow, valuemask, &GCmodel);
   XSetTile (TtDisplay, TtLineGC, pix);
#  endif /* _WINDOWS */
#  ifdef _WINDOWS
   TtLineGC.capabilities = THOT_GC_FOREGROUND | THOT_GC_PEN;
   /* THOT_GC_BACKGROUND | THOT_GC_BRUSH | */ 
   /******************************************
   TtLineGC.pen = GetStockObject (BLACK_PEN);
   *******************************************/
   /* TtLineGC.background = White_Color; */
   /* TtLineGC.foreground = Black_Color; */
   TtLineGC.foreground = 1;
   /* !!!! WIN_LastBitmap created by pix = CreatePattern(...); */
   /* TtLineGC.brush = CreatePatternBrush(WIN_LastBitmap); */
#  endif /* _WINDOWS */

   /* Another Graphic Context to write black on white, for dialogs. */
#  ifndef _WINDOWS
   TtDialogueGC = XCreateGC (TtDisplay, TtRootWindow, valuemask, &GCmodel);
#  endif /* _WINDOWS */
#  ifdef _WINDOWS
   /*
   TtDialogueGC.capabilities = THOT_GC_FOREGROUND | THOT_GC_BACKGROUND | THOT_GC_PEN;
   TtDialogueGC.pen = GetStockObject (BLACK_PEN);
   TtDialogueGC.background = White_Color;
   TtDialogueGC.foreground = Black_Color;
   */
#  endif /* _WINDOWS */

   /*
    * A Graphic Context to show selected objects. On X-Windows,
    * the colormap indexes are XORed to show the object without
    * destroying the colors : XOR.XOR = I ...
    */
#  ifndef _WINDOWS
   GCmodel.function = GXinvert;
   GCmodel.plane_mask = Select_Color;
   if (TtWDepth > 1)
      GCmodel.foreground = Black_Color;
   else
      GCmodel.foreground = Select_Color;
   GCmodel.background = White_Color;
   TtInvertGC = XCreateGC (TtDisplay, TtRootWindow, valuemask | GCPlaneMask, &GCmodel);
#  endif /* _WINDOWS */
#  ifdef _WINDOWS
   /*
   TtDialogueGC.capabilities = THOT_GC_FOREGROUND | THOT_GC_BACKGROUND |
      THOT_GC_FUNCTION;
   TtDialogueGC.mode = R2_XORPEN;
   if (TtWDepth > 1)
      TtDialogueGC.foreground = Black_Color;
   else
      TtDialogueGC.foreground = Select_Color;
   TtDialogueGC.background = White_Color;
   */
#  endif /* _WINDOWS */

   /*
    * A Graphic Context for trame objects.
    */
#  ifndef _WINDOWS
   GCmodel.function = GXcopy;
   GCmodel.foreground = Black_Color;
   TtGreyGC = XCreateGC (TtDisplay, TtRootWindow, valuemask, &GCmodel);
   XSetFillStyle (TtDisplay, TtGreyGC, FillTiled);
   XFreePixmap (TtDisplay, pix);
#  endif /* _WINDOWS */
#  ifdef _WINDOWS
   /*
   TtGreyGC.capabilities = THOT_GC_FOREGROUND | THOT_GC_BACKGROUND |
   THOT_GC_PEN |
      THOT_GC_FUNCTION;
   TtBlackGC.background = White_Color;
   TtBlackGC.foreground = Black_Color;
   TtBlackGC.mode = R2_XORPEN;
   */
   /* !!!! WIN_LastBitmap created by pix = CreatePattern(...); */
   /* TtBlackGC.brush = CreatePatternBrush(WIN_LastBitmap); */
   if (WIN_LastBitmap && !DeleteObject (WIN_LastBitmap))
      WinErrorBox (WIN_Main_Wd);
   WIN_LastBitmap = 0;
#  endif
}


/*----------------------------------------------------------------------
 *      ThotInitDisplay initialize all the output settings.
 ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ThotInitDisplay (STRING name, int dx, int dy)
#else  /* __STDC__ */
void                ThotInitDisplay (name, dx, dy)
STRING              name;
int                 dx;
int                 dy;

#endif /* __STDC__ */
{
#  ifdef _WINDOWS
   HDC                 hdc;

   hdc = GetDC (WIN_Main_Wd);
   TtWDepth = GetDeviceCaps (hdc, PLANES);
   if (TtWDepth == 1)
      TtWDepth = GetDeviceCaps (hdc, BITSPIXEL);
#  endif /* _WINDOWS */

#  ifndef _WINDOWS
   XSetErrorHandler (XWindowError);
   XSetIOErrorHandler (XWindowFatalError);
   TtScreen = DefaultScreen (TtDisplay);
   TtWDepth = DefaultDepth (TtDisplay, TtScreen);
   TtRootWindow = RootWindow (TtDisplay, TtScreen);
   TtCmap = XDefaultColormap (TtDisplay, TtScreen);
#  endif

   InitDocColors (name);
   InitColors (name);
   InitGraphicContexts ();
   InitCurs ();

#  ifdef _WINDOWS 
   WIN_InitDialogueFonts (hdc, name);
   DeleteDC (hdc);
#  else /* !_WINDOWS */
   InitDialogueFonts (name);
#endif /* _WINDOWS */

   /* Initialization of Picture Drivers */
   InitPictureHandlers (FALSE);
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
   for (i = 0; i <= MAX_FRAME; i++) {
       FrRef[i] = 0;
#      ifdef _WINDOWS
       FrMainRef[i] = 0;
#      endif /* _WINDOWS */
   }
   PackBoxRoot = NULL;		/* Don't do englobing placement for current boxes */
   DifferedPackBlocks = NULL;	/* Don't differ englobing placement for current boxes */
   BoxCreating = FALSE;		/* no interractive creation yet */
#ifndef _WIN_PRINT
   InitializeOtherThings ();
#endif /* _WIN_PRINT */
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
				buffer = TtaGetMemory (nbitems + bytes_after);
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
}
#endif /* _WINDOWS */

