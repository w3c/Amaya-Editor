/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2001.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * xpmhandler.c  Pixmap V3.4.c
 *
 * Author: I. Vatton, N. Layaida (INRIA)
 */

#include "thot_gui.h"
#include "thot_sys.h"

#include "constmedia.h"
#include "typemedia.h"
#include "picture.h"
#include "frame.h"
#include "libmsg.h"
#include "message.h"

#define THOT_EXPORT extern
#include "picture_tv.h"
#include "frame_tv.h"

#include "xpm.h"

#include "picture_f.h"
#include "inites_f.h"
#include "font_f.h"
#include "gifhandler_f.h"
#include "units_f.h"


/*----------------------------------------------------------------------
   XpmCreate reads and produces the bitmap read from the file      
   fn. updates the wif, hif, xif , yif                     
  ----------------------------------------------------------------------*/
Drawable XpmCreate (char *fn, PictInfo *imageDesc, int *xif, int *yif,
		    int *wif, int *hif, int bgColor, int *width,
		    int *height, int zoom)
{
#ifdef _WINDOWS
  *width = 0;
  *height = 0;
  *wif = 0;
  *hif = 0;
  *xif = 0;
  *yif = 0;
  return (NULL);
#else /* !_WINDOWS */
  int                 status;
  Pixmap              pixmap;
  XpmAttributes       att;
  unsigned long       valuemask = 0;

  /* pixmap loading parameters passed to the library */
  att.valuemask = valuemask;
  att.valuemask |= XpmRGBCloseness;
  att.valuemask |= XpmReturnPixels;
  att.red_closeness = 40000;
  att.green_closeness = 40000;
  att.blue_closeness = 40000;
  att.numsymbols = 1;
  att.mask_pixel = ColorPixel (bgColor);

  status = XpmReadFileToPixmap (TtDisplay, TtRootWindow, fn, &pixmap,
				(Pixmap *) &(imageDesc->PicMask), &att);
  /* return image dimensions */
  *width = att.width;
  *height = att.height;
  
  if (status != XpmSuccess)
    return ((Drawable) None);
  else
    {
      *wif = att.width;
      *hif = att.height;
      *xif = 0;
      *yif = 0;
      
      /* frees the library's internal structures */
      XpmFreeAttributes (&att);
      att.valuemask = valuemask;/* reinitialises the value mask */
      return (Drawable) pixmap;
    }
#endif  /* _WINDOWS */
}


/*----------------------------------------------------------------------
   XpmPrint converts an xpm file to PostScript.                    
  ----------------------------------------------------------------------*/
void XpmPrint (char *fn, PictureScaling pres, int xif, int yif, int wif,
	       int hif, FILE *fd, int bgColor)
{
#ifndef _WINDOWS 
   register int        i;
   XpmAttributes       att;
   XpmInfo             info;
   ThotColorStruct     color;
   int                 status;
   int                 picW, picH;
   unsigned long       valuemask = 0;
   ThotColorStruct     colorTab[256];
   XpmImage            image;
   unsigned short      red, green, blue;

   /* pixmap loading parameters passed to the library */

   valuemask |= XpmExactColors;
   valuemask |= XpmColorTable;
   valuemask |= XpmReturnColorTable;
   valuemask |= XpmReturnPixels;
   valuemask |= XpmHotspot;
   valuemask |= XpmCharsPerPixel;

   status = XpmReadFileToXpmImage (fn, &image, &info);
   if (status < XpmSuccess)
     return;

   picW = image.width;
   picH = image.height;
   /* reads the colorspace palette to produce the ps */
   for (i = 0; i < image.ncolors; i++)
     {
	if (strncmp (image.colorTable[i].c_color, "None", 4) == 0)
	  {
	     TtaGiveThotRGB (bgColor, &red, &green, &blue);
	     colorTab[i].red = red;
	     colorTab[i].green = green;
	     colorTab[i].blue = blue;
	     colorTab[i].pixel = i;
	  }
	else
	  {
	     XParseColor (TtDisplay, TtCmap, image.colorTable[i].c_color, &color);
	     colorTab[i].pixel = i;
	     colorTab[i].red = color.red;
	     colorTab[i].green = color.green;
	     colorTab[i].blue = color.blue;
	  }
     }
   DataToPrint (image.data, pres, xif, yif, wif, hif, picW, picH, fd,
		image.ncolors, -1, bgColor, image.colorTable);
   XpmFreeXpmInfo (&info);
   XpmFreeXpmImage (&image);
   att.valuemask = valuemask;
#endif /* _WINDOWS */
}			

/*----------------------------------------------------------------------
   IsXpmFormat check if the file header is of a pixmap                
  ----------------------------------------------------------------------*/
ThotBool IsXpmFormat (char *fn)
{
  FILE               *f;
  char                c;
  ThotBool            res;

  res = FALSE;
  f = fopen (fn, "r");
  if (f != NULL)
    {
      c = getc (f);
      if ((c != EOF) && (c == '/'))
	{
	  c = getc (f);
	  if ((c != EOF) && (c == '*'))
	    {
	      c = getc (f);
	      if ((c != EOF) && (c == ' '))
		res = TRUE;
	    }
	}
    }
  fclose (f);
  return res;
}			
