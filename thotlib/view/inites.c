/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 
/*
 * inites.c : module handling colors and patterns in the context of
 *            drawing on a computer screen (initpses is for Postcript).
 *
 * Authors: I. Vatton (INRIA)
 *          R. Guetari (W3C/INRIA) - Unicode and Windows version
 */

#include "ustring.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "constmenu.h"
#include "typemedia.h"
#include "frame.h"
#include "message.h"
#include "pattern.h"

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "frame_tv.h"
#include "thotcolor_tv.h"

#include "context_f.h"
#include "memory_f.h"
#include "inites_f.h"
#include "registry_f.h"
#include "ustring_f.h"
#ifdef _WINDOWS
#include "wininclude.h"
#endif /* _WINDOW S*/

static ThotColorStruct def_colrs[256];
static int             allocation_index[256];
static int             have_colors = 0;


#ifdef _WINDOWS

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
}
#else /* _WINDOWS */
/*----------------------------------------------------------------------
   FindOutColor finds the closest color by allocating it, or picking
   an already allocated color.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void       FindOutColor (Display* dsp, Colormap colormap, ThotColorStruct* colr)
#else  /* __STDC__ */
static void       FindOutColor (dsp, colormap, colr)
Display          *dsp;
Colormap          colormap;
ThotColorStruct  *colr;
#endif /* __STDC__ */
{
   int                 i, match;
#ifdef MORE_ACCURATE
   double              rd, gd, bd, dist, mindist;
#else
   int                 rd, gd, bd, dist, mindist;
#endif /* MORE_ACCURATE */
   int                 cindx;
   int                 NumCells;

#ifdef _GTK
 match =  gdk_colormap_alloc_color (colormap, colr, FALSE, TRUE);
#else /* _GTK */
 match = XAllocColor (dsp, colormap, colr);
#endif /* _GTK */
   NumCells = 0;
   if (match == 0)
     {
#ifdef _GTK
        NumCells = gdk_colormap_get_system_size ();
#else /* _GTK */
	NumCells = XDisplayCells (dsp, TtScreen);
#endif /* _GTK */
	if (!have_colors)
	  {
	     for (i = 0; i < NumCells; i++)
		def_colrs[i].pixel = i;
#ifdef _GTK
	     match =  gdk_colormap_alloc_colors (colormap, &def_colrs[0], NumCells, FALSE, TRUE, NULL); 
#else /* _GTK */
	     XQueryColors (dsp, colormap, def_colrs, NumCells);
#endif /* _GTK */
	     have_colors = 1;
	  }
#ifdef MORE_ACCURATE
	mindist = 196608.0;	/* 256.0 * 256.0 * 3.0 */
	cindx = colr->pixel;
	for (i = 0; i < NumCells; i++)
	  {
	     rd = (def_colrs[i].red - colr->red) / 256.0;
	     gd = (def_colrs[i].green - colr->green) / 256.0;
	     bd = (def_colrs[i].blue - colr->blue) / 256.0;
	     dist = (rd * rd) + (gd * gd) + (bd * bd);
	     if (dist < mindist)
	       {
		  mindist = dist;
		  cindx = def_colrs[i].pixel;
		  if (dist == 0.0)
		     break;
	       }
	  }
#else
	mindist = 196608;	/* 256 * 256 * 3 */
	cindx = colr->pixel;
	for (i = 0; i < NumCells; i++)
	  {
	     rd = ((int) (def_colrs[i].red >> 8) - (int) (colr->red >> 8));
	     gd = ((int) (def_colrs[i].green >> 8) - (int) (colr->green >> 8));
	     bd = ((int) (def_colrs[i].blue >> 8) - (int) (colr->blue >> 8));
	     dist = (rd * rd) + (gd * gd) + (bd * bd);
	     if (dist < mindist)
	       {
		  mindist = dist;
		  cindx = def_colrs[i].pixel;
		  if (dist == 0)
		     break;
	       }
	  }
#endif /* MORE_ACCURATE */
	colr->pixel = cindx;
	colr->red = def_colrs[cindx].red;
	colr->green = def_colrs[cindx].green;
	colr->blue = def_colrs[cindx].blue;
     }
   else
     {
	/*
	 * Keep a count of how many times we have allocated the
	 * same color, so we can properly free them later.
	 */
	allocation_index[match]++;

	/*
	 * If this is a new color, we've actually changed the default
	 * colormap, and may have to re-query it later.
	 */
	if (allocation_index[match] == 1)
	   have_colors = 0;
     }
}
#endif /* !_WINDOWS */


/*----------------------------------------------------------------------
   InstallColor try to install a color in the public colormap.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void InstallColor (int i)
#else  /* __STDC__ */
static void InstallColor (i)
int                 i;

#endif /* __STDC__ */
{

#  ifdef _WINDOWS
   Pix_Color[i] = RGB (RGB_Table[i].red, RGB_Table[i].green, RGB_Table[i].blue);
#  else  /* _WINDOWS */
   ThotColorStruct     col;

   if (Color_Table[i] != NULL)
     {
	/* load the color */
	col.red   = RGB_Table[i].red * 256;
	col.green = RGB_Table[i].green * 256;
	col.blue  = RGB_Table[i].blue * 256;

	/* Find closest color */
	FindOutColor (TtDisplay, TtCmap, &col);
	Pix_Color[i] = col.pixel;
	/* TODO: find the nearest color */
     }
#  endif /* _WINDOWS */
}

/*----------------------------------------------------------------------
   ApproximateColors : this function is called when there is no
   more free slot in the colormap. It adjusts the colors allocated
   upon startup depending on the existing colormap to minimize
   the distances between the requested one and existing one.
   The algorithm is based on the order of the corlor in Thot
   color base and should be far less expensive than an optimal
   (e.g. closest in cube) algorithm.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void ApproximateColors (void)
#else /* __STDC__ */
static void ApproximateColors ()
#endif /* __STDC__ */
{
   ThotColor  white = Pix_Color[0];
   ThotColor  col;
   int        line, b;

   for (line = 1; line < (NColors / 8); line++)
     {
	/*
	 * on each line, colors are stored from the brightest to
	 * the darkest. Unallocated cells are white.
	 * the fourth row should be allocated or the whole line is
	 * white.
	 */
	col = Pix_Color[line * 8 + 4];
	for (b = 4; b < 8; b++)
	   if (Pix_Color[line * 8 + b] != white)
	      col = Pix_Color[line * 8 + b];
	   else
	      Pix_Color[line * 8 + b] = col;

	col = Pix_Color[line * 8 + 4];
	for (b = 4; b >= 0; b--)
	   if (Pix_Color[line * 8 + b] != white)
	      col = Pix_Color[line * 8 + b];
	   else
	      Pix_Color[line * 8 + b] = col;
     }
}

/*----------------------------------------------------------------------
 *      FreeDocColors frees the Thot predefined X-Window colors.
 ----------------------------------------------------------------------*/
void         FreeDocColors ()
{
#ifndef _WIN_PRINT
#ifdef _WINDOWS
  /* free extended colors */
  if (!TtIsTrueColor && TtCmap && !DeleteObject (TtCmap))
    WinErrorBox (WIN_Main_Wd, TEXT("FreeDocColors (1)"));
  TtCmap = 0;
#else /* _WINDOWS */
  int        i;

#ifdef _GTK
  /* free standard colors */
  gdk_colors_free (TtCmap, &Pix_Color[0], NColors, (gulong)0 );

  /* free extended colors */
  for (i = 0; i < NbExtColors; i++)
    if (ExtColor[i])
      gdk_colors_free (TtCmap, &ExtColor[i], 1, (gulong)0);
#else /* _GTK */
  /* free standard colors */
  XFreeColors (TtDisplay, TtCmap, Pix_Color, NColors, (unsigned long) 0);

  /* free extended colors */
  for (i = 0; i < NbExtColors; i++)
    if (ExtColor[i])
      XFreeColors (TtDisplay, TtCmap, &ExtColor[i], 1, (unsigned long) 0);
#endif /* _GTK */
#endif /* _WINDOWS */
#endif /* _WIN_PRINT */

  TtaFreeMemory (ExtRGB_Table);
  TtaFreeMemory (ExtColor);
  TtaFreeMemory (ExtCount_Table);
  ExtRGB_Table = NULL;
  ExtColor = NULL;
  ExtCount_Table = NULL;
}

/*----------------------------------------------------------------------
   InitDocColors initialize the Thot internal color table.
   If ReduceColor environment setting is set, less color
   are allocated.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                InitDocColors (CHAR_T* name)
#else  /* __STDC__ */
void                InitDocColors (name)
CHAR_T*             name;

#endif /* __STDC__ */
{
#ifndef _WIN_PRINT
   int                 i, j, k;
   CHAR_T*             value;
   ThotBool            reducecolor;
   ThotBool            colormap_full;
#ifdef _GTK
   ThotColorStruct     gdkwhite, gdkblack;
#endif /* _GTK */

   /* clean up everything with white */
   for (i = 2; i < NColors; i++)
       Pix_Color[i] = Pix_Color[0];

   reducecolor = FALSE;
   colormap_full = FALSE;
   value = TtaGetEnvString ("ReduceColor");
   if (value == NULL)
      reducecolor = FALSE;
   else if (!ustrcasecmp (value, TEXT("yes")))
      reducecolor = TRUE;
   else
      reducecolor = FALSE;

   /* set up black and white Pixels */
#  ifdef _WINDOWS
   Pix_Color[0] = RGB (255, 255, 255);
   Pix_Color[1] = RGB (0, 0, 0);
#  else  /* _WINDOWS */
#ifdef _GTK
   gdk_color_white (TtCmap, &gdkwhite);  
   gdk_color_black (TtCmap, &gdkblack);
   Pix_Color[0] = gdkwhite.pixel;
   Pix_Color[1] = gdkblack.pixel;
#else /* _GTK */
   Pix_Color[0] = WhitePixel (TtDisplay, DefaultScreen (TtDisplay));
   Pix_Color[1] = BlackPixel (TtDisplay, DefaultScreen (TtDisplay));
#endif /* _GTK */
#  endif /* _WINDOWS */

   /* setup greyscale colors */
   for (i = 2; i < 8; i++)
       InstallColor (i);

   /* install the first row of primary colors */
   i = 4;
   for (i += 8; i < NColors; i += 8)
       InstallColor (i);

   /*
    * ApproximateColors is also point less but we can show
    * all the colors as issued from the primary, if allocated
    */
   if (colormap_full)
     {
	for (j = 1; j <= (NColors / 8); j++)
	   for (i = j * 8, k = 0; (i < NColors) && (k < 8); i++, k++)
	      Pix_Color[i] = Pix_Color[j * 8 + 4];
	return;
     }

   /* install the second row of colors */
   i = 2;
   for (i += 8; i < NColors; i += 8)
      InstallColor (i);

   /* install the the third and fourth rows of colors */
   i = 6;
   for (i += 8; i < NColors; i += 8)
      InstallColor (i);
   i = 0;
   for (i += 8; i < NColors; i += 8)
      InstallColor (i);

   /* here, if the user asked for reduced colormap, approximate colors */
   if (reducecolor)
      ApproximateColors ();
   else
     {
	/* install the last rows of colors */
	i = 3;
	for (i += 8; i < NColors; i += 8)
	   InstallColor (i);
	i = 7;
	for (i += 8; i < NColors; i += 8)
	   InstallColor (i);
	i = 5;
	for (i += 8; i < NColors; i += 8)
	   InstallColor (i);
	i = 1;
	for (i += 8; i < NColors; i += 8)
	   InstallColor (i);

	if (colormap_full)
	   ApproximateColors ();
     }

#endif /* _WIN_PRINT */
   NbExtColors = 0;
   ExtRGB_Table = (RGBstruct *) TtaGetMemory (256 * sizeof (RGBstruct));
   ExtColor = (ThotColor *) TtaGetMemory (256 * sizeof (ThotColor));
   ExtCount_Table = (int *) TtaGetMemory (256 * sizeof (int));
}

/*----------------------------------------------------------------------
   NumberOfColors  returns the number of colors in Thot color table.
  ----------------------------------------------------------------------*/
int                 NumberOfColors ()
{
   return NColors;
}


/*----------------------------------------------------------------------
   ColorName       returns the name of a color in Thot color table.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
CHAR_T*          ColorName (int num)
#else  /* __STDC__ */
CHAR_T*          ColorName (num)
int              num;

#endif /* __STDC__ */
{
   if (num < NColors && num >= 0)
      return Color_Table[num];
   else
      return NULL;
}

/*----------------------------------------------------------------------
   ColorPixel      returns the value of a color in Thot color table.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotColor ColorPixel (int num)
#else  /* __STDC__ */
ThotColor ColorPixel (num)
int                 num;
#endif /* __STDC__ */
{
#ifdef _GTK 
  unsigned short   red, green, blue;
  ThotColor        color;

  /* Return the value of a color in 3 octets */
  TtaGiveThotRGB(num, &red, &green, &blue);
  color = 0; 
  color |= (red & 0xFF);
  color <<= 8;
  color |= (green & 0xFF);
  color <<= 8;
  color |= (blue & 0xFF);
  return (color);
#else /* _GTK */
   if (num < NColors && num >= 0)
    return (Pix_Color[num]);
  else if (num < NColors + NbExtColors && num >= 0)
    return (ExtColor[num - NColors]);
  else
    return ((ThotColor) 0);
#endif /* _GTK */
}

/*----------------------------------------------------------------------
  TtaFreeThotColor frees the Thot Color.
 ----------------------------------------------------------------------*/
#ifdef __STDC__
void             TtaFreeThotColor (int num)
#else  /* __STDC__ */
int              TtaFreeThotColor (num)
int              num;
#endif /* __STDC__ */
{
  if (num < NColors + NbExtColors && num >= NColors)
    {
      num -= NColors;
      if (ExtCount_Table[num] == 1)
	{
#ifndef _WINDOWS
#ifdef _GTK
	  gdk_colors_free (TtCmap, &ExtColor[num], 1, (gulong)0);
#else /* _GTK */
	  XFreeColors (TtDisplay, TtCmap, &ExtColor[num], 1, (unsigned long) 0);
#endif /* _GTK */
#endif /* _WINDOWS */
	  ExtColor[num] = (ThotColor) 0;
	}
      ExtCount_Table[num]--;
    }
}

/*----------------------------------------------------------------------
  TtaGetThotColor returns the Thot Color.
  red, green, blue express the color RGB in 8 bits values
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
   int                 best;
   int                 i, prev;
   unsigned int        dsquare;
   unsigned int        best_dsquare = (unsigned int) -1;
#ifndef _WINDOWS
   ThotColorStruct     col;
#endif /* _WINDOWS */
   ThotBool            found;

   /*
    * lookup for the color number among the color set allocated
    * by the application.
    * The lookup is based on a closest in cube algorithm hence
    * we try to get the closest color available for the display.
    */

   best = 0;			/* best color in list not found */
   for (i = 0; i < NColors; i++)
     {
	delred = RGB_Table[i].red - red;
	delgreen = RGB_Table[i].green - green;
	delblue = RGB_Table[i].blue - blue;
	dsquare = delred * delred + delgreen * delgreen + delblue * delblue;
	if (dsquare < best_dsquare)
	  {
	     best = i;
	     best_dsquare = dsquare;
	  }
     }

   if (best_dsquare != 0)
     {
       prev = 256;
       /* look in the extended table */
       for (i = 0; i < NbExtColors; i++)
	 if (ExtCount_Table[i] > 0)
	   {
	     delred = ExtRGB_Table[i].red - red;
	     delgreen = ExtRGB_Table[i].green - green;
	     delblue = ExtRGB_Table[i].blue - blue;
	     dsquare  = delred * delred + delgreen * delgreen + delblue * delblue;
	     if (dsquare < best_dsquare)
	       {
		 best = i + NColors;
		 best_dsquare = dsquare;
	       }
	   }
	 else if (prev == 256)
	   /* get the first empty entry */
	   prev = i;

       if (prev == 256)
	 /* this is the first empty entry */
	 prev = NbExtColors;
	 
       if (best_dsquare != 0 && prev < 256)
	 {
	   /* try to allocate the right color */
#ifdef _WINDOWS
	   ExtColor[prev] = RGB ((BYTE)red, (BYTE)green, (BYTE)blue);
#else  /* _WINDOWS */
	   col.red   = red * 256;
	   col.green = green * 256;
	   col.blue  = blue * 256;
	   FindOutColor (TtDisplay, TtCmap, &col);
	   ExtColor[prev] = col.pixel;
#endif /* _WINDOWS */
	   /* check if this color is already in the table */
	   found = FALSE;
	   for (i = 0; i < NColors && !found; i++)
	     found = (ExtColor[prev] == Pix_Color[i]);
	   for (i = 0; i < NbExtColors && !found; i++)
	     found = (ExtColor[prev] == ExtColor[i]);

	   if (!found)
	     {
#ifdef _WINDOWS
	       ExtRGB_Table[prev].red = red;
	       ExtRGB_Table[prev].green = green;
	       ExtRGB_Table[prev].blue = blue;
#else  /* _WINDOWS */
	       ExtRGB_Table[prev].red = col.red / 256;
	       ExtRGB_Table[prev].green = col.green / 256;
	       ExtRGB_Table[prev].blue = col.blue / 256;
#endif /* _WINDOWS */
	       best = prev + NColors;
	       ExtCount_Table[prev] = 1;
	       if (prev == NbExtColors)
		 NbExtColors++;
	     }
	   else if (best >= NColors)
	     /* it's an already allocated extended color */
	     ExtCount_Table[best - NColors]++;
	 }
       else
	 ExtCount_Table[best - NColors]++;
     }

   return (best);
}

/*----------------------------------------------------------------------
   TtaGiveThotRGB returns the Red Green and Blue values corresponding
   to color number num.
   If the color doesn't exist the function returns the values
   for the default color.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaGiveThotRGB (int num, unsigned short *red, unsigned short *green, unsigned short *blue)
#else  /* __STDC__ */
void                TtaGiveThotRGB (num, red, green, blue)
int                 num;
unsigned short     *red;
unsigned short     *green;
unsigned short     *blue;
#endif /* __STDC__ */
{
  if (num < NColors && num >= 0)
    {
      *red   = RGB_Table[num].red;
      *green = RGB_Table[num].green;
      *blue  = RGB_Table[num].blue;
    }
  else if (num < NColors + NbExtColors && num >= 0)
    {
      num -= NColors;
      *red   = ExtRGB_Table[num].red;
      *green = ExtRGB_Table[num].green;
      *blue  = ExtRGB_Table[num].blue;
    }
  else
    {
      *red   = RGB_Table[1].red;
      *green = RGB_Table[1].green;
      *blue  = RGB_Table[1].blue;
    }
}

/*----------------------------------------------------------------------
   ColorNumber     lookup in Thot color table for an entry given it's
   name. Returns the index or -1 if not found.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                 ColorNumber (STRING name)
#else  /* __STDC__ */
int                 ColorNumber (name)
STRING              name;

#endif /* __STDC__ */
{
   int                 i;
   ThotBool            found;

   found = FALSE;
   i = 0;
   if (Color_Table[i] == NULL)
      return -1;		/* the table is empty */
   do
      if (ustrcasecmp (Color_Table[i], name) == 0)
	 found = TRUE;
      else
	 i++;
   while (!found && i < NColors);
   if (found)
      return i;
   else
      return -1;
}


/*----------------------------------------------------------------------
   NumberOfPatterns        returns the number of pattern available.
  ----------------------------------------------------------------------*/
int                 NumberOfPatterns ()
{
   return NbPatterns;
}


/*----------------------------------------------------------------------
   PatternName     returns the name of a pattern available.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
STRING              PatternName (int num)
#else  /* __STDC__ */
STRING              PatternName (num)
int                 num;

#endif /* __STDC__ */
{
   if (num < NumberOfPatterns () && num >= 0)
      return Patterns[num];
   else
      return NULL;
}


/*----------------------------------------------------------------------
   PatternNumber   lookup fo a pattern given it's name. Returns the
   index or -1 if not found.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                 PatternNumber (STRING name)
#else  /* __STDC__ */
int                 PatternNumber (name)
STRING              name;

#endif /* __STDC__ */
{
   int                 i;
   int                 max;
   ThotBool            found;

   found = FALSE;
   i = 0;
   max = NumberOfPatterns ();
   do
      if (ustrcmp (Patterns[i], name) == 0)
	 found = TRUE;
      else
	 i++;
   while (!found && i < max);
   if (found)
      return i;
   else
      return -1;
}

/*----------------------------------------------------------------------
   CreatePattern loads and return a pixmap pattern.
   active parameter indicate if the box is active.
   parameters fg, bg, and motif indicate respectively
   the drawing color, background color and the pattern.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
Pixmap      CreatePattern (int disp, int RO, int active, int fg, int bg, int motif)
#else  /* __STDC__ */
Pixmap      CreatePattern (disp, RO, active, fg, bg, motif)
int         disp;
int         RO;
int         active;
int         fg;
int         bg;
int         motif;

#endif /* __STDC__ */
{
   unsigned long       FgPixel;
   unsigned long       BgPixel;
   Pixmap              pixmap;
#ifdef _WINDOWS
   BITMAP              bitmap = {0, 0, 0, 1, 1, 0};
   HBITMAP             hBitmap;
#endif /* _WINDOWS */

#ifdef _GTK
   ThotColorStruct     gdkFgPixel;
   ThotColorStruct     gdkBgPixel;

#endif /* _GTK */

#ifndef _WIN_PRINT
   if (active)
     {
	/* Color for active boxes */
	FgPixel = Box_Color;
	BgPixel = ColorPixel (bg);
     }
   else if (RO)
     {
	/* Color for read only boxes */
	FgPixel = RO_Color;
	BgPixel = ColorPixel (bg);
     }
   else
#endif /* _WIN_PRINT */
     {
	FgPixel = ColorPixel (fg);
	BgPixel = ColorPixel (bg);
     }

#ifdef _GTK
   gdkFgPixel.pixel = gdk_rgb_xpixel_from_rgb (FgPixel);
   gdkBgPixel.pixel = gdk_rgb_xpixel_from_rgb (BgPixel);
#endif /* _GTK */

#ifdef _WINDOWS
   if (WIN_LastBitmap != 0)
     {
	if (!DeleteObject (WIN_LastBitmap))
#ifndef _WIN_PRINT
	  WinErrorBox (WIN_Main_Wd, TEXT("CreatePattern (1)"));
#endif /* _WIN_PRINT */
	;
	WIN_LastBitmap = 0;
     }
   switch (motif)
	 {
	    case 1:
	       bitmap.bmType       = 0;
           bitmap.bmPlanes     = 1;
           bitmap.bmBitsPixel  = 1;
	       bitmap.bmWidth      = gray8_width;
	       bitmap.bmHeight     = gray8_height;
	       bitmap.bmWidthBytes = 2;
           bitmap.bmBits       = gray8_bits;
	       hBitmap = CreateBitmapIndirect (&bitmap);
	       break;
	    case 2:
	       bitmap.bmType       = 0;
           bitmap.bmPlanes     = 1;
           bitmap.bmBitsPixel  = 1;
	       bitmap.bmWidth      = gray0_width;
	       bitmap.bmHeight     = gray0_height;
	       /* bitmap.bmWidthBytes = gray0_width / 4; */
	       bitmap.bmWidthBytes = 2;
           bitmap.bmBits       = gray0_bits;
	       hBitmap = CreateBitmapIndirect (&bitmap);
	       break;
	    case 3:
	       bitmap.bmType       = 0;
           bitmap.bmPlanes     = 1;
           bitmap.bmBitsPixel  = 1;
	       bitmap.bmWidth      = gray1_width;
	       bitmap.bmHeight     = gray1_height;
	       bitmap.bmWidthBytes = 2;
           bitmap.bmBits       = gray1_bits;
	       hBitmap = CreateBitmapIndirect (&bitmap);
	       break;
	    case 4:
	       bitmap.bmType       = 0;
           bitmap.bmPlanes     = 1;
           bitmap.bmBitsPixel  = 1;
	       bitmap.bmWidth      = gray2_width;
	       bitmap.bmHeight     = gray2_height;
	       bitmap.bmWidthBytes = 2;
           bitmap.bmBits       = gray2_bits;
	       hBitmap = CreateBitmapIndirect (&bitmap);
	       break;
	    case 5:
	       bitmap.bmType       = 0;
           bitmap.bmPlanes     = 1;
           bitmap.bmBitsPixel  = 1;
	       bitmap.bmWidth      = gray3_width;
	       bitmap.bmHeight     = gray3_height;
	       bitmap.bmWidthBytes = 2;
           bitmap.bmBits       = gray3_bits;
	       hBitmap = CreateBitmapIndirect (&bitmap);
	       break;
	    case 6:
	       bitmap.bmType       = 0;
           bitmap.bmPlanes     = 1;
           bitmap.bmBitsPixel  = 1;
	       bitmap.bmWidth      = gray4_width;
	       bitmap.bmHeight     = gray4_height;
	       bitmap.bmWidthBytes = 2;
           bitmap.bmBits       = gray4_bits;
	       hBitmap = CreateBitmapIndirect (&bitmap);
	       break;
	    case 7:
	       bitmap.bmType       = 0;
           bitmap.bmPlanes     = 1;
           bitmap.bmBitsPixel  = 1;
	       bitmap.bmWidth      = gray5_width;
	       bitmap.bmHeight     = gray5_height;
	       bitmap.bmWidthBytes = 2;
           bitmap.bmBits       = gray5_bits;
	       hBitmap = CreateBitmapIndirect (&bitmap);
	       break;
	    case 8:
	       bitmap.bmType       = 0;
           bitmap.bmPlanes     = 1;
           bitmap.bmBitsPixel  = 1;
	       bitmap.bmWidth      = gray6_width;
	       bitmap.bmHeight     = gray6_height;
	       bitmap.bmWidthBytes = 2;
           bitmap.bmBits       = gray6_bits;
	       hBitmap = CreateBitmapIndirect (&bitmap);
	       break;
	    case 9:
	       bitmap.bmType       = 0;
           bitmap.bmPlanes     = 1;
           bitmap.bmBitsPixel  = 1;
	       bitmap.bmWidth      = gray7_width;
	       bitmap.bmHeight     = gray7_height;
	       bitmap.bmWidthBytes = 2;
           bitmap.bmBits       = gray7_bits;
	       hBitmap = CreateBitmapIndirect (&bitmap);
	       break;
	    case 10:
	       bitmap.bmType       = 0;
           bitmap.bmPlanes     = 1;
           bitmap.bmBitsPixel  = 1;
	       bitmap.bmWidth      = horiz1_width;
	       bitmap.bmHeight     = horiz1_height;
	       bitmap.bmWidthBytes = 2;
           bitmap.bmBits       = horiz1_bits;
	       hBitmap = CreateBitmapIndirect (&bitmap);
	       break;
	    case 11:
	       bitmap.bmType       = 0;
           bitmap.bmPlanes     = 1;
           bitmap.bmBitsPixel  = 1;
	       bitmap.bmWidth      = horiz2_width;
	       bitmap.bmHeight     = horiz2_height;
	       bitmap.bmWidthBytes = 2;
           bitmap.bmBits       = horiz2_bits;
	       hBitmap = CreateBitmapIndirect (&bitmap);
	       break;
	    case 12:
	       bitmap.bmType       = 0;
           bitmap.bmPlanes     = 1;
           bitmap.bmBitsPixel  = 1;
	       bitmap.bmWidth      = horiz3_width;
	       bitmap.bmHeight     = horiz3_height;
	       bitmap.bmWidthBytes = 2;
           bitmap.bmBits       = horiz3_bits;
	       hBitmap = CreateBitmapIndirect (&bitmap);
	       break;
	    case 13:
	       bitmap.bmType       = 0;
           bitmap.bmPlanes     = 1;
           bitmap.bmBitsPixel  = 1;
	       bitmap.bmWidth      = vert1_width;
	       bitmap.bmHeight     = vert1_height;
	       bitmap.bmWidthBytes = 2;
           bitmap.bmBits       = vert1_bits;
	       hBitmap = CreateBitmapIndirect (&bitmap);
	       break;
	    case 14:
	       bitmap.bmType       = 0;
           bitmap.bmPlanes     = 1;
           bitmap.bmBitsPixel  = 1;
	       bitmap.bmWidth      = vert2_width;
	       bitmap.bmHeight     = vert2_height;
	       bitmap.bmWidthBytes = 2;
           bitmap.bmBits       = vert2_bits;
	       hBitmap = CreateBitmapIndirect (&bitmap);
	       break;
	    case 15:
	       bitmap.bmType       = 0;
           bitmap.bmPlanes     = 1;
           bitmap.bmBitsPixel  = 1;
	       bitmap.bmWidth      = vert3_width;
	       bitmap.bmHeight     = vert3_height;
	       bitmap.bmWidthBytes = 2;
           bitmap.bmBits       = vert3_bits;
	       hBitmap = CreateBitmapIndirect (&bitmap);
	       break;
	    case 16:
	       bitmap.bmType       = 0;
           bitmap.bmPlanes     = 1;
           bitmap.bmBitsPixel  = 1;
	       bitmap.bmWidth      = left1_width;
	       bitmap.bmHeight     = left1_height;
	       bitmap.bmWidthBytes = 2;
           bitmap.bmBits       = left1_bits;
	       hBitmap = CreateBitmapIndirect (&bitmap);
	       break;
	    case 17:
	       bitmap.bmType       = 0;
           bitmap.bmPlanes     = 1;
           bitmap.bmBitsPixel  = 1;
	       bitmap.bmWidth      = left2_width;
	       bitmap.bmHeight     = left2_height;
	       bitmap.bmWidthBytes = 2;
           bitmap.bmBits       = left2_bits;
	       hBitmap = CreateBitmapIndirect (&bitmap);
	       break;
	    case 18:
	       bitmap.bmType       = 0;
           bitmap.bmPlanes     = 1;
           bitmap.bmBitsPixel  = 1;
	       bitmap.bmWidth      = left3_width;
	       bitmap.bmHeight     = left3_height;
	       bitmap.bmWidthBytes = 2;
           bitmap.bmBits       = left3_bits;
	       hBitmap = CreateBitmapIndirect (&bitmap);
	       break;
	    case 19:
	       bitmap.bmType       = 0;
           bitmap.bmPlanes     = 1;
           bitmap.bmBitsPixel  = 1;
	       bitmap.bmWidth      = right1_width;
	       bitmap.bmHeight     = right1_height;
	       bitmap.bmWidthBytes = 2;
           bitmap.bmBits       = right1_bits;
	       hBitmap = CreateBitmapIndirect (&bitmap);
	       break;
	    case 20:
	       bitmap.bmType       = 0;
           bitmap.bmPlanes     = 1;
           bitmap.bmBitsPixel  = 1;
	       bitmap.bmWidth      = right2_width;
	       bitmap.bmHeight     = right2_height;
	       bitmap.bmWidthBytes = 2;
           bitmap.bmBits       = right2_bits;
	       hBitmap = CreateBitmapIndirect (&bitmap);
	       break;
	    case 21:
	       bitmap.bmType       = 0;
           bitmap.bmPlanes     = 1;
           bitmap.bmBitsPixel  = 1;
	       bitmap.bmWidth      = right3_width;
	       bitmap.bmHeight     = right3_height;
	       bitmap.bmWidthBytes = 2;
           bitmap.bmBits       = right3_bits;
	       hBitmap = CreateBitmapIndirect (&bitmap);
	       break;
	    case 22:
	       bitmap.bmType       = 0;
           bitmap.bmPlanes     = 1;
           bitmap.bmBitsPixel  = 1;
	       bitmap.bmWidth      = square1_width;
	       bitmap.bmHeight     = square1_height;
	       bitmap.bmWidthBytes = 2;
           bitmap.bmBits       = square1_bits;
	       hBitmap = CreateBitmapIndirect (&bitmap);
	       break;
	    case 23:
	       bitmap.bmType       = 0;
           bitmap.bmPlanes     = 1;
           bitmap.bmBitsPixel  = 1;
	       bitmap.bmWidth      = square2_width;
	       bitmap.bmHeight     = square2_height;
	       bitmap.bmWidthBytes = 2;
           bitmap.bmBits       = square2_bits;
	       hBitmap = CreateBitmapIndirect (&bitmap);
	       break;
	    case 24:
	       bitmap.bmType       = 0;
           bitmap.bmPlanes     = 1;
           bitmap.bmBitsPixel  = 1;
	       bitmap.bmWidth      = square3_width;
	       bitmap.bmHeight     = square3_height;
	       bitmap.bmWidthBytes = 24;
           bitmap.bmBits       = square3_bits;
	       hBitmap = CreateBitmapIndirect (&bitmap);
	       break;
	    case 25:
	       bitmap.bmType       = 0;
           bitmap.bmPlanes     = 1;
           bitmap.bmBitsPixel  = 1;
	       bitmap.bmWidth      = lozenge_width;
	       bitmap.bmHeight     = lozenge_height;
	       bitmap.bmWidthBytes = 2;
           bitmap.bmBits       = lozenge_bits;
	       hBitmap = CreateBitmapIndirect (&bitmap);
	       break;
	    case 26:
	       bitmap.bmType       = 0;
           bitmap.bmPlanes     = 1;
           bitmap.bmBitsPixel  = 1;
	       bitmap.bmWidth      = brick_width;
	       bitmap.bmHeight     = brick_height;
	       bitmap.bmWidthBytes = 2;
           bitmap.bmBits       = brick_bits;
	       hBitmap = CreateBitmapIndirect (&bitmap);
	       break;
	    case 27:
	       bitmap.bmType       = 0;
           bitmap.bmPlanes     = 1;
           bitmap.bmBitsPixel  = 1;
	       bitmap.bmWidth      = tile_width;
	       bitmap.bmHeight     = tile_height;
	       bitmap.bmWidthBytes = 2;
           bitmap.bmBits       = tile_bits;
	       hBitmap = CreateBitmapIndirect (&bitmap);
	       break;
	    case 28:
	       bitmap.bmType       = 0;
           bitmap.bmPlanes     = 1;
           bitmap.bmBitsPixel  = 1;
	       bitmap.bmWidth      = sea_width;
	       bitmap.bmHeight     = sea_height;
	       bitmap.bmWidthBytes = 2;
           bitmap.bmBits       = sea_bits;
	       hBitmap = CreateBitmapIndirect (&bitmap);
	       break;
	    case 29:
	       bitmap.bmType       = 0;
           bitmap.bmPlanes     = 1;
           bitmap.bmBitsPixel  = 1;
	       bitmap.bmWidth      = basket_width;
	       bitmap.bmHeight     = basket_height;
	       bitmap.bmWidthBytes = 2;
           bitmap.bmBits       = basket_bits;
	       hBitmap = CreateBitmapIndirect (&bitmap);
	       break;
	    default:
	       hBitmap = 0;
	       break;
	 }
   /* WIN_LastBitmap = hBitmap; */
   pixmap = hBitmap;
#else /* _WINDOWS */
#ifdef _GTK
   switch (motif)
	 {
	    case 1:
	       pixmap = gdk_pixmap_create_from_data (DefaultDrawable, (char *) gray8_bits, gray8_width,
				  gray8_height, TtWDepth, &gdkFgPixel, &gdkBgPixel);
	       break;
	    case 2:
	       pixmap = gdk_pixmap_create_from_data (DefaultDrawable, (char *) gray0_bits, gray0_width,
				  gray0_height, TtWDepth, &gdkFgPixel, &gdkBgPixel);
	       break;
	    case 3:
	       pixmap = gdk_pixmap_create_from_data (DefaultDrawable, (char *) gray1_bits, gray1_width,
				  gray1_height, TtWDepth, &gdkFgPixel, &gdkBgPixel);
	       break;
	    case 4:
	       pixmap = gdk_pixmap_create_from_data (DefaultDrawable, (char *) gray2_bits, gray2_width,
				  gray2_height, TtWDepth, &gdkFgPixel, &gdkBgPixel);
	       break;
	    case 5:
	       pixmap = gdk_pixmap_create_from_data (DefaultDrawable, (char *) gray3_bits, gray3_width,
				  gray3_height, TtWDepth, &gdkFgPixel, &gdkBgPixel);
	       break;
	    case 6:
	       pixmap = gdk_pixmap_create_from_data (DefaultDrawable, (char *) gray4_bits, gray4_width,
				  gray4_height, TtWDepth, &gdkFgPixel, &gdkBgPixel);
	       break;
	    case 7:
	       pixmap = gdk_pixmap_create_from_data (DefaultDrawable, (char *) gray5_bits, gray5_width,
				  gray5_height, TtWDepth, &gdkFgPixel, &gdkBgPixel);
	       break;
	    case 8:
	       pixmap = gdk_pixmap_create_from_data (DefaultDrawable, (char *) gray6_bits, gray6_width,
				  gray6_height, TtWDepth, &gdkFgPixel, &gdkBgPixel);
	       break;
	    case 9:
	       pixmap = gdk_pixmap_create_from_data (DefaultDrawable, (char *) gray7_bits, gray7_width,
				  gray7_height, TtWDepth, &gdkFgPixel, &gdkBgPixel);
	       break;
	    case 10:
	       pixmap = gdk_pixmap_create_from_data (DefaultDrawable, (char *) horiz1_bits, horiz1_width,
				 horiz1_height, TtWDepth, &gdkFgPixel, &gdkBgPixel);
	       break;
	    case 11:
	       pixmap = gdk_pixmap_create_from_data (DefaultDrawable, (char *) horiz2_bits, horiz2_width,
				 horiz2_height, TtWDepth, &gdkFgPixel, &gdkBgPixel);
	       break;
	    case 12:
	       pixmap = gdk_pixmap_create_from_data (DefaultDrawable, (char *) horiz3_bits, horiz3_width,
				 horiz3_height, TtWDepth, &gdkFgPixel, &gdkBgPixel);
	       break;
	    case 13:
	       pixmap = gdk_pixmap_create_from_data (DefaultDrawable, (char *) vert1_bits, vert1_width,
				  vert1_height, TtWDepth, &gdkFgPixel, &gdkBgPixel);
	       break;
	    case 14:
	       pixmap = gdk_pixmap_create_from_data (DefaultDrawable, (char *) vert2_bits, vert2_width,
				  vert2_height, TtWDepth, &gdkFgPixel, &gdkBgPixel);
	       break;
	    case 15:
	       pixmap = gdk_pixmap_create_from_data (DefaultDrawable, (char *) vert3_bits, vert3_width,
				  vert3_height, TtWDepth, &gdkFgPixel, &gdkBgPixel);
	       break;
	    case 16:
	       pixmap = gdk_pixmap_create_from_data (DefaultDrawable, (char *) left1_bits, left1_width,
				  left1_height, TtWDepth, &gdkFgPixel, &gdkBgPixel);
	       break;
	    case 17:
	       pixmap = gdk_pixmap_create_from_data (DefaultDrawable, (char *) left2_bits, left2_width,
				  left2_height, TtWDepth, &gdkFgPixel, &gdkBgPixel);
	       break;
	    case 18:
	       pixmap = gdk_pixmap_create_from_data (DefaultDrawable, (char *) left3_bits, left3_width,
				  left3_height, TtWDepth, &gdkFgPixel, &gdkBgPixel);
	       break;
	    case 19:
	       pixmap = gdk_pixmap_create_from_data (DefaultDrawable, (char *) right1_bits, right1_width,
				 right1_height, TtWDepth, &gdkFgPixel, &gdkBgPixel);
	       break;
	    case 20:
	       pixmap = gdk_pixmap_create_from_data (DefaultDrawable, (char *) right2_bits, right2_width,
				 right2_height, TtWDepth, &gdkFgPixel, &gdkBgPixel);
	       break;
	    case 21:
	       pixmap = gdk_pixmap_create_from_data (DefaultDrawable, (char *) right3_bits, right3_width,
				 right3_height, TtWDepth, &gdkFgPixel, &gdkBgPixel);
	       break;
	    case 22:
	       pixmap = gdk_pixmap_create_from_data (DefaultDrawable, (char *) square1_bits, square1_width,
				square1_height, TtWDepth, &gdkFgPixel, &gdkBgPixel);
	       break;
	    case 23:
	       pixmap = gdk_pixmap_create_from_data (DefaultDrawable, (char *) square2_bits, square2_width,
				square2_height, TtWDepth, &gdkFgPixel, &gdkBgPixel);
	       break;
	    case 24:
	       pixmap = gdk_pixmap_create_from_data (DefaultDrawable, (char *) square3_bits, square3_width,
				square3_height, TtWDepth, &gdkFgPixel, &gdkBgPixel);
	       break;
	    case 25:
	       pixmap = gdk_pixmap_create_from_data (DefaultDrawable, (char *) lozenge_bits, lozenge_width,
				lozenge_height, TtWDepth, &gdkFgPixel, &gdkBgPixel);
	       break;
	    case 26:
	       pixmap = gdk_pixmap_create_from_data (DefaultDrawable, (char *) brick_bits, brick_width,
				  brick_height, TtWDepth, &gdkFgPixel, &gdkBgPixel);
	       break;
	    case 27:
	       pixmap = gdk_pixmap_create_from_data (DefaultDrawable, (char *) tile_bits, tile_width,
				   tile_height, TtWDepth, &gdkFgPixel, &gdkBgPixel);
	       break;
	    case 28:
	       pixmap = gdk_pixmap_create_from_data (DefaultDrawable, (char *) sea_bits, sea_width,
				    sea_height, TtWDepth, &gdkFgPixel, &gdkBgPixel);
	       break;
	    case 29:
	       pixmap = gdk_pixmap_create_from_data (DefaultDrawable, (char *) basket_bits, basket_width,
				 basket_height, TtWDepth, &gdkFgPixel, &gdkBgPixel);
	       break;
	    default:
	       pixmap = None;
	       break;
	 }
#else /* _GTK */
   switch (motif)
	 {
	    case 1:
	       pixmap = XCreatePixmapFromBitmapData (TtDisplay, TtRootWindow, (char *) gray8_bits, gray8_width,
				  gray8_height, FgPixel, BgPixel, TtWDepth);
	       break;
	    case 2:
	       pixmap = XCreatePixmapFromBitmapData (TtDisplay, TtRootWindow, (char *) gray0_bits, gray0_width,
				  gray0_height, FgPixel, BgPixel, TtWDepth);
	       break;
	    case 3:
	       pixmap = XCreatePixmapFromBitmapData (TtDisplay, TtRootWindow, (char *) gray1_bits, gray1_width,
				  gray1_height, FgPixel, BgPixel, TtWDepth);
	       break;
	    case 4:
	       pixmap = XCreatePixmapFromBitmapData (TtDisplay, TtRootWindow, (char *) gray2_bits, gray2_width,
				  gray2_height, FgPixel, BgPixel, TtWDepth);
	       break;
	    case 5:
	       pixmap = XCreatePixmapFromBitmapData (TtDisplay, TtRootWindow, (char *) gray3_bits, gray3_width,
				  gray3_height, FgPixel, BgPixel, TtWDepth);
	       break;
	    case 6:
	       pixmap = XCreatePixmapFromBitmapData (TtDisplay, TtRootWindow, (char *) gray4_bits, gray4_width,
				  gray4_height, FgPixel, BgPixel, TtWDepth);
	       break;
	    case 7:
	       pixmap = XCreatePixmapFromBitmapData (TtDisplay, TtRootWindow, (char *) gray5_bits, gray5_width,
				  gray5_height, FgPixel, BgPixel, TtWDepth);
	       break;
	    case 8:
	       pixmap = XCreatePixmapFromBitmapData (TtDisplay, TtRootWindow, (char *) gray6_bits, gray6_width,
				  gray6_height, FgPixel, BgPixel, TtWDepth);
	       break;
	    case 9:
	       pixmap = XCreatePixmapFromBitmapData (TtDisplay, TtRootWindow, (char *) gray7_bits, gray7_width,
				  gray7_height, FgPixel, BgPixel, TtWDepth);
	       break;
	    case 10:
	       pixmap = XCreatePixmapFromBitmapData (TtDisplay, TtRootWindow, (char *) horiz1_bits, horiz1_width,
				 horiz1_height, FgPixel, BgPixel, TtWDepth);
	       break;
	    case 11:
	       pixmap = XCreatePixmapFromBitmapData (TtDisplay, TtRootWindow, (char *) horiz2_bits, horiz2_width,
				 horiz2_height, FgPixel, BgPixel, TtWDepth);
	       break;
	    case 12:
	       pixmap = XCreatePixmapFromBitmapData (TtDisplay, TtRootWindow, (char *) horiz3_bits, horiz3_width,
				 horiz3_height, FgPixel, BgPixel, TtWDepth);
	       break;
	    case 13:
	       pixmap = XCreatePixmapFromBitmapData (TtDisplay, TtRootWindow, (char *) vert1_bits, vert1_width,
				  vert1_height, FgPixel, BgPixel, TtWDepth);
	       break;
	    case 14:
	       pixmap = XCreatePixmapFromBitmapData (TtDisplay, TtRootWindow, (char *) vert2_bits, vert2_width,
				  vert2_height, FgPixel, BgPixel, TtWDepth);
	       break;
	    case 15:
	       pixmap = XCreatePixmapFromBitmapData (TtDisplay, TtRootWindow, (char *) vert3_bits, vert3_width,
				  vert3_height, FgPixel, BgPixel, TtWDepth);
	       break;
	    case 16:
	       pixmap = XCreatePixmapFromBitmapData (TtDisplay, TtRootWindow, (char *) left1_bits, left1_width,
				  left1_height, FgPixel, BgPixel, TtWDepth);
	       break;
	    case 17:
	       pixmap = XCreatePixmapFromBitmapData (TtDisplay, TtRootWindow, (char *) left2_bits, left2_width,
				  left2_height, FgPixel, BgPixel, TtWDepth);
	       break;
	    case 18:
	       pixmap = XCreatePixmapFromBitmapData (TtDisplay, TtRootWindow, (char *) left3_bits, left3_width,
				  left3_height, FgPixel, BgPixel, TtWDepth);
	       break;
	    case 19:
	       pixmap = XCreatePixmapFromBitmapData (TtDisplay, TtRootWindow, (char *) right1_bits, right1_width,
				 right1_height, FgPixel, BgPixel, TtWDepth);
	       break;
	    case 20:
	       pixmap = XCreatePixmapFromBitmapData (TtDisplay, TtRootWindow, (char *) right2_bits, right2_width,
				 right2_height, FgPixel, BgPixel, TtWDepth);
	       break;
	    case 21:
	       pixmap = XCreatePixmapFromBitmapData (TtDisplay, TtRootWindow, (char *) right3_bits, right3_width,
				 right3_height, FgPixel, BgPixel, TtWDepth);
	       break;
	    case 22:
	       pixmap = XCreatePixmapFromBitmapData (TtDisplay, TtRootWindow, (char *) square1_bits, square1_width,
				square1_height, FgPixel, BgPixel, TtWDepth);
	       break;
	    case 23:
	       pixmap = XCreatePixmapFromBitmapData (TtDisplay, TtRootWindow, (char *) square2_bits, square2_width,
				square2_height, FgPixel, BgPixel, TtWDepth);
	       break;
	    case 24:
	       pixmap = XCreatePixmapFromBitmapData (TtDisplay, TtRootWindow, (char *) square3_bits, square3_width,
				square3_height, FgPixel, BgPixel, TtWDepth);
	       break;
	    case 25:
	       pixmap = XCreatePixmapFromBitmapData (TtDisplay, TtRootWindow, (char *) lozenge_bits, lozenge_width,
				lozenge_height, FgPixel, BgPixel, TtWDepth);
	       break;
	    case 26:
	       pixmap = XCreatePixmapFromBitmapData (TtDisplay, TtRootWindow, (char *) brick_bits, brick_width,
				  brick_height, FgPixel, BgPixel, TtWDepth);
	       break;
	    case 27:
	       pixmap = XCreatePixmapFromBitmapData (TtDisplay, TtRootWindow, (char *) tile_bits, tile_width,
				   tile_height, FgPixel, BgPixel, TtWDepth);
	       break;
	    case 28:
	       pixmap = XCreatePixmapFromBitmapData (TtDisplay, TtRootWindow, (char *) sea_bits, sea_width,
				    sea_height, FgPixel, BgPixel, TtWDepth);
	       break;
	    case 29:
	       pixmap = XCreatePixmapFromBitmapData (TtDisplay, TtRootWindow, (char *) basket_bits, basket_width,
				 basket_height, FgPixel, BgPixel, TtWDepth);
	       break;
	    default:
	       pixmap = None;
	       break;
	 }
   XFlush (TtDisplay);
#endif /* _GTK */
#endif /* _WINDOWS */
   return (pixmap);
}
