/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2003
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 
/*
 * inites.c : module handling colors and patterns in the context of
 *            drawing on a Postscript page (inites is for screen output).
 *
 * some functions are just fake ones, not used on Postcript output, but needed
 * at link time.
 *
 * Authors: I. Vatton (INRIA)
 *
 */

#include "thot_sys.h"
#include "constmenu.h"
#include "constmedia.h"
#include "typemedia.h"
#include "frame.h"
#include "picture.h"
#include "message.h"
#include "pattern.h"

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "application.h"
#include "frame_tv.h"
#include "thotcolor_tv.h"


/*----------------------------------------------------------------------
   InitDocColors initialize the Thot internal color table. (fake)
  ----------------------------------------------------------------------*/
void InitDocColors (char *name)
{
   NbExtColors = 0;
   Max_Extend_Colors = 256;
   ExtRGB_Table = (RGBstruct *) TtaGetMemory (Max_Extend_Colors * sizeof (RGBstruct));
}

/*----------------------------------------------------------------------
 *      FreeDocColors frees the Thot predefined X-Window colors.
 ----------------------------------------------------------------------*/
void FreeDocColors ()
{
}


/*----------------------------------------------------------------------
   ColorName       returns the name of a color in Thot color table.
  ----------------------------------------------------------------------*/
char     *ColorName (int num)
{
   if (num < NColors && num >= 0)
      return Color_Table[num];
   else
      return NULL;
}

/*----------------------------------------------------------------------
   ColorPixel      returns the value of a color in Thot color table.
  ----------------------------------------------------------------------*/
unsigned long ColorPixel (int num)
{
   return 0;
}

/*----------------------------------------------------------------------
  TtaFreeThotColor frees the Thot Color.
 ----------------------------------------------------------------------*/
void TtaFreeThotColor (int num)
{
}

/*----------------------------------------------------------------------
  TtaGetThotColor returns the Thot Color.
  red, green, blue express the color RGB in 8 bits values
 ----------------------------------------------------------------------*/
int TtaGetThotColor (unsigned short red, unsigned short green, unsigned short blue)
{
   int                 i;

   for (i = 0; i < NColors; i++)
     if (red == RGB_Table[i].red &&
	 green == RGB_Table[i].green &&
	 blue == RGB_Table[i].blue)
       return (i);
   for (i = 0; i < NbExtColors; i++)
     if (red == ExtRGB_Table[i].red &&
	 green == ExtRGB_Table[i].green &&
	 blue == ExtRGB_Table[i].blue)
       return (i + NColors);

   /* else store the new RGB entry value */
   if (NbExtColors < Max_Extend_Colors)
     {
       i = NbExtColors;
       ExtRGB_Table[i].red = red;
       ExtRGB_Table[i].green = green;
       ExtRGB_Table[i].blue = blue;
       NbExtColors++;
       return (i + NColors);
     }
   else
     return (0);
}

/*----------------------------------------------------------------------
   TtaGiveThotRGB returns the Red Green and Blue values corresponding
   to color number num.
   If the color doesn't exist the function returns the values
   for the default color.
  ----------------------------------------------------------------------*/
void TtaGiveThotRGB (int num, unsigned short *red, unsigned short *green, unsigned short *blue)
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
   CreatePattern loads and return a pixmap pattern.
   active parameter indicate if the box is active.
   parameters fg, bg, and pattern indicate respectively
   the drawing color, background color and the pattern.
  ----------------------------------------------------------------------*/
ThotPixmap       CreatePattern (int disp, int fg, int bg, int pattern)
{
   unsigned long       FgPixel;
   unsigned long       BgPixel;
   ThotPixmap          pat = (ThotPixmap) 0;
   FgPixel = ColorPixel (fg);
   BgPixel = ColorPixel (bg);

#ifdef _MOTIF
   switch (pattern)
	 {
	    case 1:
	       pat = XCreatePixmapFromBitmapData (TtDisplay, TtRootWindow, (char *) gray8_bits, gray8_width,
				  gray8_height, FgPixel, BgPixel, TtWDepth);
	       break;
	    case 2:
	       pat = XCreatePixmapFromBitmapData (TtDisplay, TtRootWindow, (char *) gray0_bits, gray0_width,
				  gray0_height, FgPixel, BgPixel, TtWDepth);
	       break;
	    case 3:
	       pat = XCreatePixmapFromBitmapData (TtDisplay, TtRootWindow, (char *) gray1_bits, gray1_width,
				  gray1_height, FgPixel, BgPixel, TtWDepth);
	       break;
	    case 4:
	       pat = XCreatePixmapFromBitmapData (TtDisplay, TtRootWindow, (char *) gray2_bits, gray2_width,
				  gray2_height, FgPixel, BgPixel, TtWDepth);
	       break;
	    case 5:
	       pat = XCreatePixmapFromBitmapData (TtDisplay, TtRootWindow, (char *) gray3_bits, gray3_width,
				  gray3_height, FgPixel, BgPixel, TtWDepth);
	       break;
	    case 6:
	       pat = XCreatePixmapFromBitmapData (TtDisplay, TtRootWindow, (char *) gray4_bits, gray4_width,
				  gray4_height, FgPixel, BgPixel, TtWDepth);
	       break;
	    case 7:
	       pat = XCreatePixmapFromBitmapData (TtDisplay, TtRootWindow, (char *) gray5_bits, gray5_width,
				  gray5_height, FgPixel, BgPixel, TtWDepth);
	       break;
	    case 8:
	       pat = XCreatePixmapFromBitmapData (TtDisplay, TtRootWindow, (char *) gray6_bits, gray6_width,
				  gray6_height, FgPixel, BgPixel, TtWDepth);
	       break;
	    case 9:
	       pat = XCreatePixmapFromBitmapData (TtDisplay, TtRootWindow, (char *) gray7_bits, gray7_width,
				  gray7_height, FgPixel, BgPixel, TtWDepth);
	       break;
	    case 10:
	       pat = XCreatePixmapFromBitmapData (TtDisplay, TtRootWindow, (char *) horiz1_bits, horiz1_width,
				 horiz1_height, FgPixel, BgPixel, TtWDepth);
	       break;
	    case 11:
	       pat = XCreatePixmapFromBitmapData (TtDisplay, TtRootWindow, (char *) horiz2_bits, horiz2_width,
				 horiz2_height, FgPixel, BgPixel, TtWDepth);
	       break;
	    case 12:
	       pat = XCreatePixmapFromBitmapData (TtDisplay, TtRootWindow, (char *) horiz3_bits, horiz3_width,
				 horiz3_height, FgPixel, BgPixel, TtWDepth);
	       break;
	    case 13:
	       pat = XCreatePixmapFromBitmapData (TtDisplay, TtRootWindow, (char *) vert1_bits, vert1_width,
				  vert1_height, FgPixel, BgPixel, TtWDepth);
	       break;
	    case 14:
	       pat = XCreatePixmapFromBitmapData (TtDisplay, TtRootWindow, (char *) vert2_bits, vert2_width,
				  vert2_height, FgPixel, BgPixel, TtWDepth);
	       break;
	    case 15:
	       pat = XCreatePixmapFromBitmapData (TtDisplay, TtRootWindow, (char *) vert3_bits, vert3_width,
				  vert3_height, FgPixel, BgPixel, TtWDepth);
	       break;
	    case 16:
	       pat = XCreatePixmapFromBitmapData (TtDisplay, TtRootWindow, (char *) left1_bits, left1_width,
				  left1_height, FgPixel, BgPixel, TtWDepth);
	       break;
	    case 17:
	       pat = XCreatePixmapFromBitmapData (TtDisplay, TtRootWindow, (char *) left2_bits, left2_width,
				  left2_height, FgPixel, BgPixel, TtWDepth);
	       break;
	    case 18:
	       pat = XCreatePixmapFromBitmapData (TtDisplay, TtRootWindow, (char *) left3_bits, left3_width,
				  left3_height, FgPixel, BgPixel, TtWDepth);
	       break;
	    case 19:
	       pat = XCreatePixmapFromBitmapData (TtDisplay, TtRootWindow, (char *) right1_bits, right1_width,
				 right1_height, FgPixel, BgPixel, TtWDepth);
	       break;
	    case 20:
	       pat = XCreatePixmapFromBitmapData (TtDisplay, TtRootWindow, (char *) right2_bits, right2_width,
				 right2_height, FgPixel, BgPixel, TtWDepth);
	       break;
	    case 21:
	       pat = XCreatePixmapFromBitmapData (TtDisplay, TtRootWindow, (char *) right3_bits, right3_width,
				 right3_height, FgPixel, BgPixel, TtWDepth);
	       break;
	    case 22:
	       pat = XCreatePixmapFromBitmapData (TtDisplay, TtRootWindow, (char *) square1_bits, square1_width,
				square1_height, FgPixel, BgPixel, TtWDepth);
	       break;
	    case 23:
	       pat = XCreatePixmapFromBitmapData (TtDisplay, TtRootWindow, (char *) square2_bits, square2_width,
				square2_height, FgPixel, BgPixel, TtWDepth);
	       break;
	    case 24:
	       pat = XCreatePixmapFromBitmapData (TtDisplay, TtRootWindow, (char *) square3_bits, square3_width,
				square3_height, FgPixel, BgPixel, TtWDepth);
	       break;
	    case 25:
	       pat = XCreatePixmapFromBitmapData (TtDisplay, TtRootWindow, (char *) lozenge_bits, lozenge_width,
				lozenge_height, FgPixel, BgPixel, TtWDepth);
	       break;
	    case 26:
	       pat = XCreatePixmapFromBitmapData (TtDisplay, TtRootWindow, (char *) brick_bits, brick_width,
				  brick_height, FgPixel, BgPixel, TtWDepth);
	       break;
	    case 27:
	       pat = XCreatePixmapFromBitmapData (TtDisplay, TtRootWindow, (char *) tile_bits, tile_width,
				   tile_height, FgPixel, BgPixel, TtWDepth);
	       break;
	    case 28:
	       pat = XCreatePixmapFromBitmapData (TtDisplay, TtRootWindow, (char *) sea_bits, sea_width,
				    sea_height, FgPixel, BgPixel, TtWDepth);
	       break;
	    case 29:
	       pat = XCreatePixmapFromBitmapData (TtDisplay, TtRootWindow, (char *) basket_bits, basket_width,
				 basket_height, FgPixel, BgPixel, TtWDepth);
	       break;
	    default:
	       pat = None;
	       break;
	 }
#endif /* #ifdef _MOTIF */
   return pat;
}








