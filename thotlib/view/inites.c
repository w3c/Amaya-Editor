/*
 *
 *  (c) COPYRIGHT INRIA, Grif, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 
/*
 * inites.c : module handling colors and patterns in the context of
 *            drawing on a computer screen (initpses is for Postcript).
 *
 * Author: I. Vatton (INRIA)
 *         R. Guetari (W3C/INRIA) Windows NT/95 routines
 */

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

#include "registry_f.h"
#include "context_f.h"

static ThotColorStruct def_colrs[256];
static int          allocation_index[256];
static int          have_colors = 0;

#ifndef _WINDOWS
/*----------------------------------------------------------------------
   FindOutColor finds the closest color by allocating it, or picking
   an already allocated color.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                FindOutColor (Display* dsp, Colormap colormap, ThotColorStruct* colr)
#else  /* __STDC__ */
void                FindOutColor (dsp, colormap, colr)
Display            *dsp;
Colormap            colormap;
ThotColorStruct    *colr;

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

   match = XAllocColor (dsp, colormap, colr);
   if (match == 0)
     {
	NumCells = DisplayCells (dsp, TtScreen);
	if (!have_colors)
	  {
	     for (i = 0; i < NumCells; i++)
		def_colrs[i].pixel = i;
	     XQueryColors (dsp, colormap, def_colrs, NumCells);
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

#ifndef _WIN_PRINT
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
   else
     {
	*red   = RGB_Table[1].red;
	*green = RGB_Table[1].green;
	*blue  = RGB_Table[1].blue;
     }
}
#endif /* _WIN_PRINT */

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

#ifndef _WIN_PRINT
/*----------------------------------------------------------------------
   InitDocColors initialize the Thot internal color table.
   If ReduceColor environment setting is set, less color
   are allocated.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                InitDocColors (char *name)
#else  /* __STDC__ */
void                InitDocColors (name)
char               *name;

#endif /* __STDC__ */
{
   int                 i, j, k;
   char               *value;
   boolean             reducecolor;
   boolean             colormap_full;

   /* clean up everything with white */
   for (i = 2; i < NColors; i++)
       Pix_Color[i] = Pix_Color[0];

   reducecolor = FALSE;
   colormap_full = FALSE;
   value = TtaGetEnvString ("ReduceColor");
   if (value == NULL)
      reducecolor = FALSE;
   else if (!strcasecmp (value, "yes"))
      reducecolor = TRUE;
   else
      reducecolor = FALSE;

   /* set up black and white Pixels */
#  ifdef _WINDOWS
   Pix_Color[0] = RGB (255, 255, 255);
   Pix_Color[1] = RGB (0, 0, 0);
#  else  /* _WINDOWS */
   Pix_Color[0] = WhitePixel (TtDisplay, DefaultScreen (TtDisplay));
   Pix_Color[1] = BlackPixel (TtDisplay, DefaultScreen (TtDisplay));
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
}
#endif /* _WIN_PRINT */

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
char               *ColorName (int num)
#else  /* __STDC__ */
char               *ColorName (num)
int                 num;

#endif /* __STDC__ */
{
   if (num < NColors && num >= 0)
      return Color_Table[num];
   else
      return NULL;
}

#ifndef _WIN_PRINT
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
   if (num < NColors && num >= 0)
      return Pix_Color[num];
   else
      return (ThotColor) 0;
}
#endif /* _WIN_PRINT */

/*----------------------------------------------------------------------
   ColorNumber     lookup in Thot color table for an entry given it's
   name. Returns the index or -1 if not found.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                 ColorNumber (char *name)
#else  /* __STDC__ */
int                 ColorNumber (name)
char               *name;

#endif /* __STDC__ */
{
   int                 i;
   boolean             found;

   found = FALSE;
   i = 0;
   if (Color_Table[i] == NULL)
      return -1;		/* the table is empty */
   do
      if (strcmp (Color_Table[i], name) == 0)
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
char               *PatternName (int num)
#else  /* __STDC__ */
char               *PatternName (num)
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
int                 PatternNumber (char *name)
#else  /* __STDC__ */
int                 PatternNumber (name)
char               *name;

#endif /* __STDC__ */
{
   int                 i;
   int                 max;
   boolean             found;

   found = FALSE;
   i = 0;
   max = NumberOfPatterns ();
   do
      if (strcmp (Patterns[i], name) == 0)
	 found = TRUE;
      else
	 i++;
   while (!found && i < max);
   if (found)
      return i;
   else
      return -1;
}

#ifndef _WIN_PRINT
/*----------------------------------------------------------------------
   CreatePattern loads and return a pixmap pattern.
   active parameter indicate if the box is active.
   parameters fg, bg, and motif indicate respectively
   the drawing color, background color and the pattern.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
unsigned long       CreatePattern (int disp, int RO, int active, int fg, int bg, int motif)
#else  /* __STDC__ */
unsigned long       CreatePattern (disp, RO, active, fg, bg, motif)
int                 disp;
int                 RO;
int                 active;
int                 fg;
int                 bg;
int                 motif;

#endif /* __STDC__ */
{
   unsigned long       FgPixel;
   unsigned long       BgPixel;
   Pixmap              pixmap;

#  ifdef _WINDOWS
   BITMAP              bitmap = {0, 0, 0, 1, 1, 0};
   HBITMAP             hBitmap;
#  endif /* _WINDOWS */

#ifdef bug649
   if (TtWDepth == 1)
     {
	/* Black and White display */
	FgPixel = ColorPixel (ColorNumber ("Black"));
	BgPixel = ColorPixel (ColorNumber ("White"));
     }
   else
#endif
   if (active && ShowReference ())
     {
	/* Color for active boxes */
	FgPixel = Box_Color;
	BgPixel = ColorPixel (bg);
     }
   else if (RO && ShowReadOnly ())
     {
	/* Color for read only boxes */
	FgPixel = RO_Color;
	BgPixel = ColorPixel (bg);
     }
   else
     {
	FgPixel = ColorPixel (fg);
	BgPixel = ColorPixel (bg);
     }

#ifndef _WINDOWS
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
#endif
#ifdef _WINDOWS
   if (WIN_LastBitmap != 0)
     {
	if (!DeleteObject (WIN_LastBitmap))
       WinErrorBox (WIN_Main_Wd);
	WIN_LastBitmap = 0;
     }
   switch (motif)
	 {
	    case 1:
	       bitmap.bmWidth = gray8_width;
	       bitmap.bmHeight = gray8_height;
	       bitmap.bmWidthBytes = gray8_width / 4;
	       hBitmap = CreateBitmapIndirect (&bitmap);
	       SetBitmapBits (hBitmap, sizeof (gray8_bits), gray8_bits);
	       break;
	    case 2:
	       bitmap.bmWidth = gray0_width;
	       bitmap.bmHeight = gray0_height;
	       bitmap.bmWidthBytes = sizeof (gray0_bits);
	       /* hBitmap = CreateBitmapIndirect (&bitmap); */
	       hBitmap = CreateBitmap (gray8_width, gray8_height, 1, 1, NULL);
	       SetBitmapBits (hBitmap, sizeof (gray0_bits), gray0_bits);
	       break;
	    case 3:
	       bitmap.bmWidth = gray1_width;
	       bitmap.bmHeight = gray1_height;
	       bitmap.bmWidthBytes = gray1_width / 4;
	       hBitmap = CreateBitmapIndirect (&bitmap);
	       SetBitmapBits (hBitmap, sizeof (gray1_bits), gray1_bits);
	       break;
	    case 4:
	       bitmap.bmWidth = gray2_width;
	       bitmap.bmHeight = gray2_height;
	       bitmap.bmWidthBytes = gray2_width / 4;
	       hBitmap = CreateBitmapIndirect (&bitmap);
	       SetBitmapBits (hBitmap, sizeof (gray2_bits), gray2_bits);
	       break;
	    case 5:
	       bitmap.bmWidth = gray3_width;
	       bitmap.bmHeight = gray3_height;
	       bitmap.bmWidthBytes = gray3_width / 4;
	       hBitmap = CreateBitmapIndirect (&bitmap);
	       SetBitmapBits (hBitmap, sizeof (gray3_bits), gray3_bits);
	       break;
	    case 6:
	       bitmap.bmWidth = gray4_width;
	       bitmap.bmHeight = gray4_height;
	       bitmap.bmWidthBytes = gray4_width / 4;
	       hBitmap = CreateBitmapIndirect (&bitmap);
	       SetBitmapBits (hBitmap, sizeof (gray4_bits), gray4_bits);
	       break;
	    case 7:
	       bitmap.bmWidth = gray5_width;
	       bitmap.bmHeight = gray5_height;
	       bitmap.bmWidthBytes = gray5_width / 4;
	       hBitmap = CreateBitmapIndirect (&bitmap);
	       SetBitmapBits (hBitmap, sizeof (gray5_bits), gray5_bits);
	       break;
	    case 8:
	       bitmap.bmWidth = gray6_width;
	       bitmap.bmHeight = gray6_height;
	       bitmap.bmWidthBytes = gray6_width / 4;
	       hBitmap = CreateBitmapIndirect (&bitmap);
	       SetBitmapBits (hBitmap, sizeof (gray6_bits), gray6_bits);
	       break;
	    case 9:
	       bitmap.bmWidth = gray7_width;
	       bitmap.bmHeight = gray7_height;
	       bitmap.bmWidthBytes = gray7_width / 4;
	       hBitmap = CreateBitmapIndirect (&bitmap);
	       SetBitmapBits (hBitmap, sizeof (gray7_bits), gray7_bits);
	       break;
	    case 10:
	       bitmap.bmWidth = horiz1_width;
	       bitmap.bmHeight = horiz1_height;
	       bitmap.bmWidthBytes = horiz1_width / 4;
	       hBitmap = CreateBitmapIndirect (&bitmap);
	       SetBitmapBits (hBitmap, sizeof (horiz1_bits), horiz1_bits);
	       break;
	    case 11:
	       bitmap.bmWidth = horiz2_width;
	       bitmap.bmHeight = horiz2_height;
	       bitmap.bmWidthBytes = horiz2_width / 4;
	       hBitmap = CreateBitmapIndirect (&bitmap);
	       SetBitmapBits (hBitmap, sizeof (horiz2_bits), horiz2_bits);
	       break;
	    case 12:
	       bitmap.bmWidth = horiz3_width;
	       bitmap.bmHeight = horiz3_height;
	       bitmap.bmWidthBytes = horiz3_width / 4;
	       hBitmap = CreateBitmapIndirect (&bitmap);
	       SetBitmapBits (hBitmap, sizeof (horiz3_bits), horiz3_bits);
	       break;
	    case 13:
	       bitmap.bmWidth = vert1_width;
	       bitmap.bmHeight = vert1_height;
	       bitmap.bmWidthBytes = vert1_width / 4;
	       hBitmap = CreateBitmapIndirect (&bitmap);
	       SetBitmapBits (hBitmap, sizeof (vert1_bits), vert1_bits);
	       break;
	    case 14:
	       bitmap.bmWidth = vert2_width;
	       bitmap.bmHeight = vert2_height;
	       bitmap.bmWidthBytes = vert2_width / 4;
	       hBitmap = CreateBitmapIndirect (&bitmap);
	       SetBitmapBits (hBitmap, sizeof (vert2_bits), vert2_bits);
	       break;
	    case 15:
	       bitmap.bmWidth = vert3_width;
	       bitmap.bmHeight = vert3_height;
	       bitmap.bmWidthBytes = vert3_width / 4;
	       hBitmap = CreateBitmapIndirect (&bitmap);
	       SetBitmapBits (hBitmap, sizeof (vert3_bits), vert3_bits);
	       break;
	    case 16:
	       bitmap.bmWidth = left1_width;
	       bitmap.bmHeight = left1_height;
	       bitmap.bmWidthBytes = left1_width / 4;
	       hBitmap = CreateBitmapIndirect (&bitmap);
	       SetBitmapBits (hBitmap, sizeof (left1_bits), left1_bits);
	       break;
	    case 17:
	       bitmap.bmWidth = left2_width;
	       bitmap.bmHeight = left2_height;
	       bitmap.bmWidthBytes = left2_width / 4;
	       hBitmap = CreateBitmapIndirect (&bitmap);
	       SetBitmapBits (hBitmap, sizeof (left2_bits), left2_bits);
	       break;
	    case 18:
	       bitmap.bmWidth = left3_width;
	       bitmap.bmHeight = left3_height;
	       bitmap.bmWidthBytes = left3_width / 4;
	       hBitmap = CreateBitmapIndirect (&bitmap);
	       SetBitmapBits (hBitmap, sizeof (left3_bits), left3_bits);
	       break;
	    case 19:
	       bitmap.bmWidth = right1_width;
	       bitmap.bmHeight = right1_height;
	       bitmap.bmWidthBytes = right1_width / 4;
	       hBitmap = CreateBitmapIndirect (&bitmap);
	       SetBitmapBits (hBitmap, sizeof (right1_bits), right1_bits);
	       break;
	    case 20:
	       bitmap.bmWidth = right2_width;
	       bitmap.bmHeight = right2_height;
	       bitmap.bmWidthBytes = right2_width / 4;
	       hBitmap = CreateBitmapIndirect (&bitmap);
	       SetBitmapBits (hBitmap, sizeof (right2_bits), right2_bits);
	       break;
	    case 21:
	       bitmap.bmWidth = right3_width;
	       bitmap.bmHeight = right3_height;
	       bitmap.bmWidthBytes = right3_width / 4;
	       hBitmap = CreateBitmapIndirect (&bitmap);
	       SetBitmapBits (hBitmap, sizeof (right3_bits), right3_bits);
	       break;
	    case 22:
	       bitmap.bmWidth = square1_width;
	       bitmap.bmHeight = square1_height;
	       bitmap.bmWidthBytes = square1_width / 4;
	       hBitmap = CreateBitmapIndirect (&bitmap);
	       SetBitmapBits (hBitmap, sizeof (square1_bits), square1_bits);
	       break;
	    case 23:
	       bitmap.bmWidth = square2_width;
	       bitmap.bmHeight = square2_height;
	       bitmap.bmWidthBytes = square2_width / 4;
	       hBitmap = CreateBitmapIndirect (&bitmap);
	       SetBitmapBits (hBitmap, sizeof (square2_bits), square2_bits);
	       break;
	    case 24:
	       bitmap.bmWidth = square3_width;
	       bitmap.bmHeight = square3_height;
	       bitmap.bmWidthBytes = square3_width / 4;
	       hBitmap = CreateBitmapIndirect (&bitmap);
	       SetBitmapBits (hBitmap, sizeof (square3_bits), square3_bits);
	       break;
	    case 25:
	       bitmap.bmWidth = lozenge_width;
	       bitmap.bmHeight = lozenge_height;
	       bitmap.bmWidthBytes = lozenge_width / 4;
	       hBitmap = CreateBitmapIndirect (&bitmap);
	       SetBitmapBits (hBitmap, sizeof (lozenge_bits), lozenge_bits);
	       break;
	    case 26:
	       bitmap.bmWidth = brick_width;
	       bitmap.bmHeight = brick_height;
	       bitmap.bmWidthBytes = brick_width / 4;
	       hBitmap = CreateBitmapIndirect (&bitmap);
	       SetBitmapBits (hBitmap, sizeof (brick_bits), brick_bits);
	       hBitmap = CreateBitmapIndirect (&bitmap);
	       SetBitmapBits (hBitmap, sizeof (brick_bits), brick_bits);
	       break;
	    case 27:
	       bitmap.bmWidth = tile_width;
	       bitmap.bmHeight = tile_height;
	       bitmap.bmWidthBytes = tile_width / 4;
	       hBitmap = CreateBitmapIndirect (&bitmap);
	       SetBitmapBits (hBitmap, sizeof (tile_bits), tile_bits);
	       break;
	    case 28:
	       bitmap.bmWidth = sea_width;
	       bitmap.bmHeight = sea_height;
	       bitmap.bmWidthBytes = sea_width / 4;
	       hBitmap = CreateBitmapIndirect (&bitmap);
	       SetBitmapBits (hBitmap, sizeof (sea_bits), sea_bits);
	       break;
	    case 29:
	       bitmap.bmWidth = basket_width;
	       bitmap.bmHeight = basket_height;
	       bitmap.bmWidthBytes = basket_width / 4;
	       hBitmap = CreateBitmapIndirect (&bitmap);
	       SetBitmapBits (hBitmap, sizeof (basket_bits), basket_bits);
	       break;
	    default:
	       hBitmap = 0;
	       break;
	 }
   WIN_LastBitmap = hBitmap;
   pixmap = hBitmap ;
#endif
   return ((unsigned long) pixmap);
}
#endif /* _WIN_PRINT */
