/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2001.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * Author: I. Vatton, N. Layaida (INRIA)
 *         R. Guetari (W3C/INRIA) Windows version
 */

#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "picture.h"
#include "frame.h"

#define THOT_EXPORT extern
#include "picture_tv.h"
#include "frame_tv.h"

#include "picture_f.h"
#include "font_f.h"
#include "units_f.h"	    

/*----------------------------------------------------------------------
   XbmCreate reads and produces the bitmap read from the file      
   fn. updates the wif, hif, xif , yif                     
  ----------------------------------------------------------------------*/
Drawable XbmCreate (char *fn, PictInfo *imageDesc, int *xif, int *yif,
		    int *wif, int *hif, unsigned long BackGroundPixel,
		    int *width, int *height, int zoom)
{
#ifndef _WINDOWS
  Pixmap              pixmap;
  Pixmap              bitmap;
  int                 status;
  int                 w, h;
  int                 xHot, yHot;

  status = XReadBitmapFile (TtDisplay, TtRootWindow, fn, &w, &h, &bitmap, &xHot, &yHot);
  if (status != BitmapSuccess)
    return ((Drawable) None);
  else
    {
      *xif = 0;
      *yif = 0;
      *wif = w;
      *hif = h;
      *width = w;
      *height = h;
#ifndef _GTK
      pixmap = XCreatePixmap (TtDisplay, TtRootWindow, w, h, TtWDepth);
      XCopyPlane (TtDisplay, bitmap, pixmap, GCpicture, 0, 0, w, h, 0, 0, 1);
      XFreePixmap (TtDisplay, bitmap);
#endif /* _GTK */
      return (pixmap);
    }
#else /* _WINDOWS */
  return NULL;
#endif /* !_WINDOWS */
}


/*----------------------------------------------------------------------
   XbmPrint produces postscript frome an xbm file                  
  ----------------------------------------------------------------------*/
void XbmPrint (char *fn, PictureScaling pres, int xif, int yif, int wif,
	       int hif, int PicXArea, int PicYArea, int PicWArea,
	       int PicHArea, FILE *fd, unsigned int BackGroundPixel)
{
#ifdef _WINDOWS
   return;
#else  /* _WINDOWS */
   XImage             *pict;
   Pixmap              pix;
   int                 delta;
   int                 xtmp, ytmp;
   float               Scx, Scy;
   register int        i, j, nbb;
   register char      *pt, *pt1;
   int                 wim, him;

   i = XReadBitmapFile (TtDisplay, TtRootWindow, fn, &PicWArea, &PicHArea,
			&pix, &xtmp, &ytmp);
   if (i != BitmapSuccess)
      return;
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
		    yif += (hif - (PicHArea * Scx)) / 2;
		    hif = PicHArea * Scx;
		 }
	       else
		 {
		    Scy = (float) hif / (float) PicHArea;
		    xif += (wif - (PicWArea * Scy)) / 2;
		    wif = PicWArea * Scy;
		 }
	       break;
	    default:
	       break;
	 }

   if (pix != None)
     {
	pict = XGetImage (TtDisplay, pix, xtmp, ytmp,
			  (unsigned int) PicWArea, (unsigned int) PicHArea,
			  AllPlanes, XYPixmap);

	wim = pict->width;
	him = pict->height;
	fprintf (fd, "gsave %d -%d translate\n", PixelToPoint (xif), PixelToPoint (yif + hif));
	fprintf (fd, "%d %d %d %d DumpImage\n", pict->width, pict->height, PixelToPoint (wif), PixelToPoint (hif));

	nbb = (wim + 7) / 8;
	if (ImageByteOrder (TtDisplay) == LSBFirst)
	   LittleXBigEndian ((unsigned char *) pict->data, (long) (pict->bytes_per_line * him));
	for (j = 0, pt1 = pict->data; j < him; j++, pt1 += pict->bytes_per_line)
	  {
	     for (i = 0, pt = pt1; i < nbb; i++)
		fprintf (fd, "%02x", ((*pt++) & 0xff) ^ 0xff);
	     fprintf (fd, "\n");
	  }
	fprintf (fd, "grestore\n");

	/* frees the allocated space for the bitmap in memory */
	XDestroyImage (pict);
	XFreePixmap (TtDisplay, pix);
     }
#endif /* !_WINDOWS */
}

/*----------------------------------------------------------------------
   IsXbmFormat check if the file header is of an xbm format        
  ----------------------------------------------------------------------*/
ThotBool             IsXbmFormat (char *fn)
{
#ifdef _WINDOWS
   return (FALSE);
#else  /* _WINDOWS */
   Pixmap              bitmap = None;
   int                 status;
   int                 w, h;
   int                 xHot, yHot;

   status = XReadBitmapFile (TtDisplay, TtRootWindow, fn, &w, &h, &bitmap, &xHot, &yHot);
   if (bitmap != None)
      XFreePixmap (TtDisplay, bitmap);
   return (status == BitmapSuccess);
#endif /* !_WINDOWS */
}
