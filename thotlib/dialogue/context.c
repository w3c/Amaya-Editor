/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2008
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
#ifdef _GTK
  static ThotColorStruct  cwhite;
#endif /* _GTK */
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
#include "context_f.h"


/*----------------------------------------------------------------------
 * FindColor looks for the named color ressource.
 * The result is the closest color found the Thot color table.
 ----------------------------------------------------------------------*/
static ThotBool FindColor (const char *colorplace,
                           const char *defaultcolor, ThotColor *colorpixel)
{
   int                 col;
   char               *value;
   unsigned short      red;
   unsigned short      green;
   unsigned short      blue;
   char                *temp;
   

   value = TtaGetEnvString (colorplace);
   /* do you need to take the default color? */
   if (value != NULL && value[0] != EOS)
     TtaGiveRGB (value, &red, &green, &blue);
   else if (defaultcolor)
     {
       temp = TtaStrdup(defaultcolor);
       TtaGiveRGB (temp, &red, &green, &blue);
       TtaFreeMemory(temp);
     }
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
   else if (strcmp (colorplace, "ResizeBgSelectColor") == 0)
     ResizeBgSelColor = col;
   else if (strcmp (colorplace, "ResizeFgSelectColor") == 0)
     ResizeFgSelColor = col;
   
#ifdef _WINGUI 
   *colorpixel = col;
#else /* _WINGUI */
   *colorpixel = ColorPixel (col);
#endif /* _WINGUI */
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
  ThotBool  found;

  /* selection colors */
  found = FindColor ("FgSelectColor", "White", &White_Color);
  found = FindColor ("BgSelectColor", "#008BB2", &Black_Color);
  /* resize selection colors */
  found = FindColor ("ResizeFgSelectColor", "Black", &White_Color);
  found = FindColor ("ResizeBgSelectColor", "SpringGreen",   &Black_Color);
  /* background color */
  found = FindColor ("BackgroundColor", "LightGrey1", &White_Color);
  /* drawing color */
  found = FindColor ("ForegroundColor", "Black", &Black_Color);
  /* The reference color */
  found = FindColor ("ActiveBoxColor", "Red", &(Box_Color));
  /* color for read-only sections */
  found = FindColor ("ReadOnlyColor", "Black", &(RO_Color));
  /* color for the menu background */
  found = FindColor ("MenuBgColor", "Grey", &BgMenu_Color);
  /* color for the menu foregroundground */
  found = FindColor ("MenuFgColor", "Black", &FgMenu_Color);
  /* scrolls color */
  Scroll_Color = BgMenu_Color;
  /* color for the inactive entries */
  found = FindColor ("InactiveItemColor", "LightGrey2", &InactiveB_Color);
  
  WindowBColor = -1; /* color will be defined with the toolbar background */
}

/*----------------------------------------------------------------------
 *      InitColors initializes the Thot predefined X-Window colors.
 ----------------------------------------------------------------------*/
static void InitColors ()
{
  ThotBool            found;

  if (TtWDepth > 1)
    TtaUpdateEditorColors ();
  else
    {
      /* at least allocate the selection color */
      found = FindColor ("FgSelectColor", "White", &White_Color);
      found = FindColor ("BgSelectColor", "Black", &Black_Color);
    }
}


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
   InitColors ();
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

#ifdef _WX
#ifdef IV
   int display_width_px, display_height_px;
   int display_width_mm, display_height_mm;
   wxDisplaySize(&display_width_px, &display_height_px);
   wxDisplaySizeMM(&display_width_mm, &display_height_mm);
   DOT_PER_INCH = (int)((((float)display_width_px)*25.4) / ((float)display_width_mm));
   DOT_PER_INCH = ApproximateDotPerInch(DOT_PER_INCH);
   printf ("DOT_PER_INCH=%d\n",DOT_PER_INCH);
#endif
   DOT_PER_INCH=96;
#ifdef _GL
   TtWDepth = wxDisplayDepth(); 
#endif /*_GL */
   InitDocColors (name);
   InitColors ();
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

/*----------------------------------------------------------------------
 * Returns a dpi approximation because some server X returns bad values...
 ----------------------------------------------------------------------*/
int ApproximateDotPerInch( int dpi )
{
  static int dpi_table[] = { 75, 90, 96, 100, 110, 120, 120 /* terminal value is doubled */ };
  unsigned int i = 0;
  while ( i < sizeof(dpi_table)-1 )
    {
      if (dpi <= (dpi_table[i]+dpi_table[i+1]-1)/2 )
        return dpi_table[i];
      i++;
    }
  return dpi;
}
