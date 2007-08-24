/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2005
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
void InitDocColors (const char *name)
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
const char     *ColorName (int num)
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
ThotPixmap CreatePattern (int disp, int fg, int bg, int pattern)
{
   return (ThotPixmap) 0;
}








