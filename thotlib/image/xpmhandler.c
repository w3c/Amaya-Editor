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
#include "units_f.h"


/*----------------------------------------------------------------------
   XpmCreate reads and produces the bitmap read from the file      
   fn. updates the wif, hif, xif , yif                     
  ----------------------------------------------------------------------*/
Drawable XpmCreate (char *fn, PictInfo *imageDesc, int *xif, int *yif,
		    int *wif, int *hif, unsigned long BackGroundPixel,
		    int *width, int *height, int zoom)
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
  att.mask_pixel = BackGroundPixel;

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
	       int hif, int PicXArea, int PicYArea, int PicWArea,
	       int PicHArea, FILE *fd, unsigned long BackGroundPixel)
{
#ifndef _WINDOWS 
   int                 delta;
   int                 xtmp, ytmp;
   float               Scx, Scy;
   register int        i;
   unsigned int       *pt;
   unsigned char       pt1;
   int                 x, y;
   int                 wim ;
   XpmAttributes       att;
   XpmInfo             info;
   ThotColorStruct     exactcolor;
   int                 status;
   unsigned long       valuemask = 0;
   ThotColorStruct     colorTab[256];
   XpmImage            image;
   unsigned int        NbCharPerLine;
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

   PicWArea = image.width;
   PicHArea = image.height;
   xtmp = 0;
   ytmp = 0;

   switch (pres)
     {
     case RealSize:
     case FillFrame:
     case XRepeat:
     case YRepeat:
       delta = (wif - PicWArea) / 2;
       if (delta > 0)
	 {
	   xif += delta;
	   wif = PicWArea;
	 }
       else
	 {
	   xtmp = -delta;
	   PicWArea = wif;
	 }
       delta = (hif - PicHArea) / 2;
       if (delta > 0)
	 {
	   yif += delta;
	   hif = PicHArea;
	 }
       else
	 {
	   
	   ytmp = -delta;
	   PicHArea = hif;
	 }
       break;
     case ReScale:
       if ((float) PicHArea / (float) PicWArea <= (float) hif / (float) wif)
	 {
	   Scx = (float) wif / (float) PicWArea;
	   yif += (int) ((hif - (PicHArea * Scx)) / 2);
	   hif = (int) (PicHArea * Scx);
	 }
       else
	 {
	   Scy = (float) hif / (float) PicHArea;
	   xif += (int) ((wif - (PicWArea * Scy)) / 2);
	   wif = (int) (PicWArea * Scy);
	 }
       break;
     default:
       break;
     }

   /* reads the colorspace palette to produce the ps */
   for (i = 0; i < (int) (image.ncolors); i++)
     {
	if (strncmp (image.colorTable[i].c_color, "None", 4) == 0)

	  {
	     TtaGiveThotRGB ((int) BackGroundPixel, &red, &green, &blue);
	     colorTab[i].red = (unsigned char) red;
	     colorTab[i].green = (unsigned char) green;
	     colorTab[i].blue = (unsigned char) blue;
	     colorTab[i].pixel = i;
	     /*NoneColor = i; */
	     /*MaskSet = 1; */

	  }
	else
	  {
	     XParseColor (TtDisplay, TtCmap, image.colorTable[i].c_color, &exactcolor);
	     colorTab[i].pixel = i;
	     colorTab[i].red = exactcolor.red;
	     colorTab[i].green = exactcolor.green;
	     colorTab[i].blue = exactcolor.blue;

	  }
     }

   wim = image.width;
   /* generation of the poscript , header Dumpimage2 + dimensions  */
   /* + picture location. Each pixel = RRGGBB in  hexa    */
   fprintf (fd, "gsave %d -%d translate\n", PixelToPoint (xif),
	    PixelToPoint (yif + hif));
   fprintf (fd, "%d %d %d %d DumpImage2\n", PicWArea, PicHArea,
	    PixelToPoint (wif), PixelToPoint (hif));
   fprintf (fd, "\n");

   NbCharPerLine = wim;

   for (y = 0; y < hif; y++)
     {
	pt = (image.data + ((ytmp + y) * NbCharPerLine) + xtmp);
	for (x = 0; x < wif; x++)
	  {

	     /* RGB components generation */
	     pt1 = (unsigned char) (*pt);
	     fprintf (fd, "%02x%02x%02x",
		      (colorTab[pt1].red) & 0xff,
		      (colorTab[pt1].green) & 0xff,
		      (colorTab[pt1].blue) & 0xff);

	     pt++;
	  }
	fprintf (fd, "\n");
     }

   fprintf (fd, "\n");
   fprintf (fd, "grestore\n");
   fprintf (fd, "\n");
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
  f = ufopen (fn, "r");
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
