/*
 *
 *  (c) COPYRIGHT INRIA, Grif, 1996.
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
 * Author: I. Vatton (INRIA)
 *
 */

#include "thot_sys.h"
#include "constmenu.h"
#include "constmedia.h"
#include "typemedia.h"
#include "frame.h"
#include "picture.h"
#include "message.h"
#ifndef _WIN_PRINT
#include "pattern.h"
#endif /* _WIN_PRINT */
#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "frame_tv.h"
#include "thotcolor_tv.h"


/*----------------------------------------------------------------------
   FindOutColor finds the closest color by allocating it, or picking
   an already allocated color.
  ----------------------------------------------------------------------*/
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

/*----------------------------------------------------------------------
   InitDocColors initialize the Thot internal color table. (fake)
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                InitDocColors (char *name)
#else  /* __STDC__ */
void                InitDocColors (name)
char               *name;

#endif /* __STDC__ */
{
}

/*----------------------------------------------------------------------
   ColorPixel      returns the value of a color in Thot color table.
  ----------------------------------------------------------------------*/

#ifdef __STDC__
unsigned long       ColorPixel (int num)
#else  /* __STDC__ */
unsigned long       ColorPixel (num)
int                 num;

#endif /* __STDC__ */
{
   return 0;
}


/*----------------------------------------------------------------------
   TtaGiveThotRGB        returns the Red Green and Blue values corresponding
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


/*----------------------------------------------------------------------
   CreatePattern loads and return a pixmap pattern.
   active parameter indicate if the box is active.
   parameters fg, bg, and pattern indicate respectively
   the drawing color, background color and the pattern.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
unsigned long       CreatePattern (int disp, int RO, int active, int fg, int bg, int pattern)
#else  /* __STDC__ */
unsigned long       CreatePattern (disp, RO, active, fg, bg, pattern)
int                 disp;
int                 RO;
int                 active;
int                 fg;
int                 bg;
int                 pattern;

#endif /* __STDC__ */
{
   unsigned long       FgPixel;
   unsigned long       BgPixel;
   Pixmap              pat;

#  ifdef _WINDOWS
   BITMAP              bitmap = {0, 0, 0, 1, 1, 0};
   HBITMAP             hBitmap;
#  endif /* _WINDOWS */

   FgPixel = ColorPixel (fg);
   BgPixel = ColorPixel (bg);

#ifndef _WINDOWS
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
#endif
#ifdef _WINDOWS
#if 0
   if (WIN_LastBitmap != 0) {
      DeleteObject (WIN_LastBitmap);
      WIN_LastBitmap = 0;
   }
   switch (pattern) {
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
	       hBitmap = CreateBitmapIndirect (&bitmap);
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
#  endif /* 0 */
#endif
   return pat;
}
