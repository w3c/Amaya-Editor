/*
 * inites.c : module handling colors and patterns in the context of
 *	      drawing on a Postscript page (inites is for screen output).
 *
 * some functions are just fake ones, not used on Postcript output, but needed
 * at link time.
 */

#include "thot_sys.h"
#include "constmenu.h"
#include "constmedia.h"
#include "typemedia.h"
#include "frame.h"
#include "picture.h"
#include "message.h"
#include "pattern.h"

#undef EXPORT
#define EXPORT extern
#include "frame_tv.h"
#include "thotcolor_tv.h"


/** ----------------------------------------------------------------------
 *   FindOutColor finds the closest color by allocating it, or picking
 *              an already allocated color.
 *  ---------------------------------------------------------------------- **/
#ifdef __STDC__
void                FindOutColor (Display * dsp, Colormap colormap, ThotColorStruct * colr)
#else  /* __STDC__ */
void                FindOutColor (dsp, colormap, colr)
Display            *dsp;
Colormap            colormap;
ThotColorStruct    *colr;

#endif /* __STDC__ */
{
}

/** ----------------------------------------------------------------------
 *      InitDocColors initialize the Thot internal color table. (fake)
 *  ---------------------------------------------------------------------- **/
#ifdef __STDC__
void                InitDocColors (char *name)
#else  /* __STDC__ */
void                InitDocColors (name)
char               *name;

#endif /* __STDC__ */
{
}

/** ----------------------------------------------------------------------
 *      ColorPixel      returns the value of a color in Thot color table.
 *  ---------------------------------------------------------------------- **/
#ifdef __STDC__
unsigned long       ColorPixel (int num)
#else  /* __STDC__ */
unsigned long       ColorPixel (num)
int                 num;

#endif /* __STDC__ */
{
   return 0;
}


/** ----------------------------------------------------------------------
 *      ColorRGB        returns the Red Green and Blue values corresponding
 *		to color number num.
 *		If the color doesn't exist the function returns the values
 *		for the default color.
 *  ---------------------------------------------------------------------- **/
#ifdef __STDC__
void                ColorRGB (int num, unsigned short *red, unsigned short *green, unsigned short *blue)
#else  /* __STDC__ */
void                ColorRGB (num, red, green, blue)
int                 num;
unsigned short     *red;
unsigned short     *green;
unsigned short     *blue;

#endif /* __STDC__ */
{
   if (num < NColors && num >= 0)
     {
	*red = RGB_Table[num].red;
	*green = RGB_Table[num].green;
	*blue = RGB_Table[num].blue;
     }
   else
     {
	*red = RGB_Table[1].red;
	*green = RGB_Table[1].green;
	*blue = RGB_Table[1].blue;
     }
}


/** ----------------------------------------------------------------------
 *      CreatePattern loads and return a pixmap pattern.
 *              active parameter indicate if the box is active.
 *              parameters fg, bg, and motif indicate respectively
 *              the drawing color, background color and the pattern.
 *  ---------------------------------------------------------------------- **/
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
   Pixmap              pattern;

   FgPixel = ColorPixel (fg);
   BgPixel = ColorPixel (bg);

   switch (motif)
	 {
	    case 1:
	       pattern = XCreatePixmapFromBitmapData (TtDisplay, TtRootWindow, (char *) gray8_bits, gray8_width,
			     gray8_height, FgPixel, BgPixel, TtWDepth);
	       break;
	    case 2:
	       pattern = XCreatePixmapFromBitmapData (TtDisplay, TtRootWindow, (char *) gray0_bits, gray0_width,
			     gray0_height, FgPixel, BgPixel, TtWDepth);
	       break;
	    case 3:
	       pattern = XCreatePixmapFromBitmapData (TtDisplay, TtRootWindow, (char *) gray1_bits, gray1_width,
			     gray1_height, FgPixel, BgPixel, TtWDepth);
	       break;
	    case 4:
	       pattern = XCreatePixmapFromBitmapData (TtDisplay, TtRootWindow, (char *) gray2_bits, gray2_width,
			     gray2_height, FgPixel, BgPixel, TtWDepth);
	       break;
	    case 5:
	       pattern = XCreatePixmapFromBitmapData (TtDisplay, TtRootWindow, (char *) gray3_bits, gray3_width,
			     gray3_height, FgPixel, BgPixel, TtWDepth);
	       break;
	    case 6:
	       pattern = XCreatePixmapFromBitmapData (TtDisplay, TtRootWindow, (char *) gray4_bits, gray4_width,
			     gray4_height, FgPixel, BgPixel, TtWDepth);
	       break;
	    case 7:
	       pattern = XCreatePixmapFromBitmapData (TtDisplay, TtRootWindow, (char *) gray5_bits, gray5_width,
			     gray5_height, FgPixel, BgPixel, TtWDepth);
	       break;
	    case 8:
	       pattern = XCreatePixmapFromBitmapData (TtDisplay, TtRootWindow, (char *) gray6_bits, gray6_width,
			     gray6_height, FgPixel, BgPixel, TtWDepth);
	       break;
	    case 9:
	       pattern = XCreatePixmapFromBitmapData (TtDisplay, TtRootWindow, (char *) gray7_bits, gray7_width,
			     gray7_height, FgPixel, BgPixel, TtWDepth);
	       break;
	    case 10:
	       pattern = XCreatePixmapFromBitmapData (TtDisplay, TtRootWindow, (char *) horiz1_bits, horiz1_width,
			    horiz1_height, FgPixel, BgPixel, TtWDepth);
	       break;
	    case 11:
	       pattern = XCreatePixmapFromBitmapData (TtDisplay, TtRootWindow, (char *) horiz2_bits, horiz2_width,
			    horiz2_height, FgPixel, BgPixel, TtWDepth);
	       break;
	    case 12:
	       pattern = XCreatePixmapFromBitmapData (TtDisplay, TtRootWindow, (char *) horiz3_bits, horiz3_width,
			    horiz3_height, FgPixel, BgPixel, TtWDepth);
	       break;
	    case 13:
	       pattern = XCreatePixmapFromBitmapData (TtDisplay, TtRootWindow, (char *) vert1_bits, vert1_width,
			     vert1_height, FgPixel, BgPixel, TtWDepth);
	       break;
	    case 14:
	       pattern = XCreatePixmapFromBitmapData (TtDisplay, TtRootWindow, (char *) vert2_bits, vert2_width,
			     vert2_height, FgPixel, BgPixel, TtWDepth);
	       break;
	    case 15:
	       pattern = XCreatePixmapFromBitmapData (TtDisplay, TtRootWindow, (char *) vert3_bits, vert3_width,
			     vert3_height, FgPixel, BgPixel, TtWDepth);
	       break;
	    case 16:
	       pattern = XCreatePixmapFromBitmapData (TtDisplay, TtRootWindow, (char *) left1_bits, left1_width,
			     left1_height, FgPixel, BgPixel, TtWDepth);
	       break;
	    case 17:
	       pattern = XCreatePixmapFromBitmapData (TtDisplay, TtRootWindow, (char *) left2_bits, left2_width,
			     left2_height, FgPixel, BgPixel, TtWDepth);
	       break;
	    case 18:
	       pattern = XCreatePixmapFromBitmapData (TtDisplay, TtRootWindow, (char *) left3_bits, left3_width,
			     left3_height, FgPixel, BgPixel, TtWDepth);
	       break;
	    case 19:
	       pattern = XCreatePixmapFromBitmapData (TtDisplay, TtRootWindow, (char *) right1_bits, right1_width,
			    right1_height, FgPixel, BgPixel, TtWDepth);
	       break;
	    case 20:
	       pattern = XCreatePixmapFromBitmapData (TtDisplay, TtRootWindow, (char *) right2_bits, right2_width,
			    right2_height, FgPixel, BgPixel, TtWDepth);
	       break;
	    case 21:
	       pattern = XCreatePixmapFromBitmapData (TtDisplay, TtRootWindow, (char *) right3_bits, right3_width,
			    right3_height, FgPixel, BgPixel, TtWDepth);
	       break;
	    case 22:
	       pattern = XCreatePixmapFromBitmapData (TtDisplay, TtRootWindow, (char *) square1_bits, square1_width,
			   square1_height, FgPixel, BgPixel, TtWDepth);
	       break;
	    case 23:
	       pattern = XCreatePixmapFromBitmapData (TtDisplay, TtRootWindow, (char *) square2_bits, square2_width,
			   square2_height, FgPixel, BgPixel, TtWDepth);
	       break;
	    case 24:
	       pattern = XCreatePixmapFromBitmapData (TtDisplay, TtRootWindow, (char *) square3_bits, square3_width,
			   square3_height, FgPixel, BgPixel, TtWDepth);
	       break;
	    case 25:
	       pattern = XCreatePixmapFromBitmapData (TtDisplay, TtRootWindow, (char *) lozenge_bits, lozenge_width,
			   lozenge_height, FgPixel, BgPixel, TtWDepth);
	       break;
	    case 26:
	       pattern = XCreatePixmapFromBitmapData (TtDisplay, TtRootWindow, (char *) brick_bits, brick_width,
			     brick_height, FgPixel, BgPixel, TtWDepth);
	       break;
	    case 27:
	       pattern = XCreatePixmapFromBitmapData (TtDisplay, TtRootWindow, (char *) tile_bits, tile_width,
			      tile_height, FgPixel, BgPixel, TtWDepth);
	       break;
	    case 28:
	       pattern = XCreatePixmapFromBitmapData (TtDisplay, TtRootWindow, (char *) sea_bits, sea_width,
			       sea_height, FgPixel, BgPixel, TtWDepth);
	       break;
	    case 29:
	       pattern = XCreatePixmapFromBitmapData (TtDisplay, TtRootWindow, (char *) basket_bits, basket_width,
			    basket_height, FgPixel, BgPixel, TtWDepth);
	       break;
	    default:
	       pattern = None;
	       break;
	 }

   return pattern;
}
