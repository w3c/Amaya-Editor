/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2003
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
		    int *wif, int *hif, unsigned long bgPixel, int *width,
		    int *height, int zoom)
{
#ifndef _WINDOWS
#ifndef _GTK
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
      pixmap = XCreatePixmap (TtDisplay, TtRootWindow, w, h, TtWDepth);
      XCopyPlane (TtDisplay, bitmap, pixmap, GCpicture, 0, 0, w, h, 0, 0, 1);
      XFreePixmap (TtDisplay, bitmap);
      return (pixmap);
    }
#else /* _GTK */
  return (Drawable)NULL;
#endif /* _GTK */
#else /* _WINDOWS */
  return NULL;
#endif /* !_WINDOWS */
}


/*----------------------------------------------------------------------
   XbmPrint produces postscript frome an xbm file                  
  ----------------------------------------------------------------------*/
void XbmPrint (char *fn, PictureScaling pres, int xif, int yif, int wif,
	       int hif, FILE *fd, unsigned int bgPixel)
{
#ifndef _WINDOWS
#ifndef _GTK
  XImage             *pict;
  Pixmap              pix;
  int                 delta;
  int                 xtmp, ytmp;
  int                 picW, picH;
  float               Scx, Scy;
  register int        i, j, nbb;
  register char      *pt, *pt1;
  int                 wim, him;

  i = XReadBitmapFile (TtDisplay, TtRootWindow, fn, &picW, &picH, &pix,
		       &xtmp, &ytmp);
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
      delta = (wif - picW) / 2;
      if (delta > 0)
	{
	  xif += delta;
	  wif = picW;
	}
      else
	{
	  xtmp = -delta;
	  picW = wif;
	}
      delta = (hif - picH) / 2;
      if (delta > 0)
	{
	  yif += delta;
	  hif = picH;
	}
      else
	{
	  ytmp = -delta;
	  picH = hif;
	}
      break;
    case ReScale:
      if ((float) picH / (float) picW <= (float) hif / (float) wif)
	{
	  Scx = (float) wif / (float) picW;
	  yif += (hif - (picH * Scx)) / 2;
	  hif = picH * Scx;
	}
      else
	{
	  Scy = (float) hif / (float) picH;
	  xif += (wif - (picW * Scy)) / 2;
	  wif = picW * Scy;
	}
      break;
    default:
      break;
    }
  
  if (pix != None)
    {
      pict = XGetImage (TtDisplay, pix, xtmp, ytmp, (unsigned int) picW,
			(unsigned int) picH, AllPlanes, XYPixmap);
      
      wim = pict->width;
      him = pict->height;
      fprintf (fd, "gsave %d -%d translate\n", xif, yif + hif);
      fprintf (fd, "%d %d %d %d DumpImage\n", pict->width, pict->height, wif, hif);
      
      nbb = (wim + 7) / 8;
      if (ImageByteOrder (TtDisplay) == LSBFirst)
	LittleXBigEndian ((unsigned char *) pict->data,
			  (long) (pict->bytes_per_line * him));
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
#endif /* !_GTK */
#endif /* !_WINDOWS */
}

/*----------------------------------------------------------------------
   IsXbmFormat check if the file header is of an xbm format        
  ----------------------------------------------------------------------*/
ThotBool IsXbmFormat (char *fn)
{
#ifndef _GTK
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
#else /* _GTK */
   return FALSE;
#endif /* !_GTK */
}

