/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
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
 *          R. Guetari (W3C/INRIA) Windows routines.
 *
 */

#include "ustring.h"
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

#ifdef _WINDOWS
#ifndef gray0_width
#define gray0_width 16
#endif /* gray0_width */

#ifndef gray0_height
#define gray0_height 16
#endif /* gray0_height */

#ifndef gray1_width
#define gray1_width 16
#endif /* gray1_width */

#ifndef gray1_height
#define gray1_height 16
#endif /* gray1_height */

#ifndef gray2_width
#define gray2_width 16
#endif /* gray2_width */

#ifndef gray2_height
#define gray2_height 16
#endif /* gray2_height */

#ifndef gray3_width
#define gray3_width 16
#endif /* gray3_width */

#ifndef gray3_height
#define gray3_height 16
#endif /* gray3_height */

#ifndef gray4_width
#define gray4_width 16
#endif /* gray4_width */

#ifndef gray4_height
#define gray4_height 16
#endif /* gray4_height */

#ifndef gray5_width
#define gray5_width 16
#endif /* gray5_width */

#ifndef gray5_height
#define gray5_height 16
#endif /* gray5_height */

#ifndef gray6_width
#define gray6_width 16
#endif /* gray5_width */

#ifndef gray6_height
#define gray6_height 16
#endif /* gray6_height */

#ifndef gray7_width
#define gray7_width 16
#endif /* gray6_width */

#ifndef gray7_height
#define gray7_height 16
#endif /* gray7_height */

#ifndef gray8_width
#define gray8_width 16
#endif /* gray1_width */

#ifndef gray8_height
#define gray8_height 16
#endif /* gray1_height */

#ifndef horiz1_width
#define horiz1_width 16
#endif /* horiz1_width */

#ifndef horiz1_height
#define horiz1_height 16
#endif /* horiz1_height */

#ifndef horiz2_width
#define horiz2_width 16
#endif /* horiz2_width */

#ifndef horiz2_height
#define horiz2_height 16
#endif /* horiz2_height */

#ifndef horiz3_width
#define horiz3_width 16
#endif /* horiz3_width */

#ifndef horiz3_height
#define horiz3_height 16
#endif /* horiz3_height */

#ifndef vert1_width
#define vert1_width 16
#endif /* vert1_width */

#ifndef vert1_height
#define vert1_height 16
#endif /* vert1_height */

#ifndef vert2_width
#define vert2_width 16
#endif /* vert2_width */

#ifndef vert2_height
#define vert2_height 16
#endif /* vert2_height */

#ifndef vert3_width
#define vert3_width 16
#endif /* vert3_width */

#ifndef vert3_height
#define vert3_height 16
#endif /* vert3_height */

#ifndef left1_width
#define left1_width 16
#endif /* left1_width */

#ifndef left1_height
#define left1_height 16
#endif /* left1_height */

#ifndef left2_width
#define left2_width 16
#endif /* left2_width */

#ifndef left2_height
#define left2_height 16
#endif /* left2_height */

#ifndef left3_width
#define left3_width 16
#endif /* left3_width */

#ifndef left3_height
#define left3_height 16
#endif /* left3_height */

#ifndef right1_width
#define right1_width 16
#endif /* right1_width */

#ifndef right1_height
#define right1_height 16
#endif /* right1_height */

#ifndef right2_width
#define right2_width 16
#endif /* right2_width */

#ifndef right2_height
#define right2_height 16
#endif /* right2_height */

#ifndef right3_width
#define right3_width 16
#endif /* right3_width */

#ifndef right3_height
#define right3_height 16
#endif /* right3_height */

#ifndef square1_width
#define square1_width 16
#endif /* square1_width */

#ifndef square1_height
#define square1_height 16
#endif /* square1_height */

#ifndef square2_width
#define square2_width 16
#endif /* square2_width */

#ifndef square2_height
#define square2_height 16
#endif /* square2_height */

#ifndef square3_width
#define square3_width 16
#endif /* square3_width */

#ifndef square3_height
#define square3_height 16
#endif /* square3_height */

#ifndef lozenge_width
#define lozenge_width 16
#endif /* lozenge_width */

#ifndef lozenge_height
#define lozenge_height 16
#endif /* lozenge_height */

#ifndef brick_width
#define brick_width 16
#endif /* brick_width */

#ifndef brick_height
#define brick_height 16
#endif /* brick_height */

#ifndef tile_width
#define tile_width 16
#endif /* tile_width */

#ifndef tile_height
#define tile_height 16
#endif /* tile_height */

#ifndef sea_width
#define sea_width 16
#endif /* sea_width */

#ifndef sea_height
#define sea_height 16
#endif /* sea_height */

#ifndef basket_width
#define basket_width 16
#endif /* basket_width */

#ifndef basket_height
#define basket_height 16
#endif /* basket_height */

extern unsigned short gray0_bits[16];
extern unsigned short gray1_bits[16];
extern unsigned short gray2_bits[16];
extern unsigned char  gray3_bits[32];
extern unsigned short gray4_bits[16];
extern unsigned char  gray5_bits[32];
extern unsigned short gray6_bits[16];
extern unsigned short gray7_bits[16];
extern unsigned short gray8_bits[16];
extern unsigned char  horiz1_bits[32];
extern unsigned char  horiz2_bits[32];
extern unsigned char  horiz3_bits[32];
extern unsigned char  vert1_bits[32];
extern unsigned char  vert2_bits[32];
extern unsigned char  vert3_bits[32];
extern unsigned char  left1_bits[32];
extern unsigned char  left2_bits[32];
extern unsigned char  left3_bits[32];
extern unsigned char  right1_bits[32];
extern unsigned char  right2_bits[32];
extern unsigned char  right3_bits[32];
extern unsigned char  square1_bits[32];
extern unsigned char  square2_bits[32];
extern unsigned char  square3_bits[32];
extern unsigned char  lozenge_bits[32];
extern unsigned char  brick_bits[32];
extern unsigned char  tile_bits[32];
extern unsigned char  sea_bits[32];
extern unsigned char  basket_bits[32];
#endif /* _WINDOWS */

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
   NbExtColors = 0;
   ExtRGB_Table = (RGBstruct *) TtaGetMemory (256 * sizeof (RGBstruct));
}

/*----------------------------------------------------------------------
 *      FreeDocColors frees the Thot predefined X-Window colors.
 ----------------------------------------------------------------------*/
void         FreeDocColors ()
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
  TtaFreeThotColor frees the Thot Color.
 ----------------------------------------------------------------------*/
#ifdef __STDC__
void             TtaFreeThotColor (int num)
#else  /* __STDC__ */
int              TtaGetThotColor (num)
int              num;
#endif /* __STDC__ */
{
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
   if (NbExtColors < 256)
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
   CreatePattern loads and return a pixmap pattern.
   active parameter indicate if the box is active.
   parameters fg, bg, and pattern indicate respectively
   the drawing color, background color and the pattern.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
#ifdef _WINDOWS
Pixmap
#else  /* _WINDOWS */
unsigned long       
#endif /* _WINDOWS */
                    CreatePattern (int disp, int RO, int active, int fg, int bg, int pattern)
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
   return pat;
}
