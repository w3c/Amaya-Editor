/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * Warning:
 * This module is part of the Thot library, which was originally
 * developed in French. That's why some comments are still in
 * French, but their translation is in progress and the full module
 * will be available in English in the next release.
 * 
 */
 
/*
 * Author: N. Layaida (INRIA)
 *         R. Guetari (W3C/INRIA) Unicode and Windows version
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
#ifdef __STDC__
Drawable XbmCreate (STRING fn, PictInfo *imageDesc, int* xif, int* yif, int* wif, int* hif, unsigned long BackGroundPixel, ThotBitmap *mask1, int *width, int *height, int zoom)
#else  /* __STDC__ */
Drawable XbmCreate (fn, imageDesc, xif, yif, wif, hif, BackGroundPixel, mask1, width, height, zoom)
STRING              fn;
PictInfo           *imageDesc;
int                *xif;
int                *yif;
int                *wif;
int                *hif;
unsigned long       BackGroundPixel;
ThotBitmap         *mask1;
int                *width;
int                *height;
int                 zoom;
#endif /* __STDC__ */
{
  Pixmap              pixmap;
# ifndef _WINDOWS
  int                 status;
  int                 w, h;
  Pixmap              bitmap;
  int                 xHot, yHot;

  *mask1 = None;
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
      pixmap = XCreatePixmap (TtDisplay, TtRootWindow, w, h, DefaultDepth (TtDisplay, DefaultScreen (TtDisplay)));
      XCopyPlane (TtDisplay, bitmap, pixmap, GCpicture, 0, 0, w, h, 0, 0, 1);
      XFreePixmap (TtDisplay, bitmap);
#endif /* _GTK */
      return (pixmap);
    }
# else /* _WINDOWS */
  pixmap = (HBITMAP) 0;
# if 0
  HANDLE           hBmFileName ;
  BITMAPFILEHEADER bmFileHeader ;
  BITMAPINFOHEADER bmInfoHeader ;
  BITMAPINFO*      bmInfo ;
  BITMAP           bmp;
  DWORD            dwRead ;
  HGLOBAL          hMem1;
  HGLOBAL          hMem2;
  BYTE*            lpvBits;

  /* Retrieve a handle identifying the file. */ 
  hBmFileName  = CreateFile (fn, GENERIC_READ, FILE_SHARE_READ, (LPSECURITY_ATTRIBUTES) NULL, 
			     OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, (HANDLE) NULL); 
  
  /* Retrieve the BITMAPFILEHEADER structure. */ 
  ReadFile (hBmFileName, &bmFileHeader, sizeof (BITMAPFILEHEADER), &dwRead, (LPOVERLAPPED) NULL); 
  
  /* Retrieve the BITMAPFILEHEADER structure. */ 
  ReadFile (hBmFileName, &bmInfoHeader, sizeof (BITMAPINFOHEADER), &dwRead, (LPOVERLAPPED) NULL); 
  
  /* Allocate memory for the BITMAPINFO structure. */ 
  hMem1  = GlobalAlloc (GHND, sizeof(BITMAPINFOHEADER) + ((1<<bmInfoHeader.biBitCount) * sizeof(RGBQUAD))); 
  bmInfo = (BITMAPINFO*) GlobalLock (hMem1); 
  
  /* Load BITMAPINFOHEADER into the BITMAPINFO structure */ 
  bmInfo->bmiHeader.biSize          = bmInfoHeader.biSize; 
  bmInfo->bmiHeader.biWidth         = bmInfoHeader.biWidth; 
  bmInfo->bmiHeader.biHeight        = bmInfoHeader.biHeight; 
  bmInfo->bmiHeader.biPlanes        = bmInfoHeader.biPlanes; 
  bmInfo->bmiHeader.biBitCount      = bmInfoHeader.biBitCount; 
  bmInfo->bmiHeader.biCompression   = bmInfoHeader.biCompression; 
  bmInfo->bmiHeader.biSizeImage     = bmInfoHeader.biSizeImage; 
  bmInfo->bmiHeader.biXPelsPerMeter = bmInfoHeader.biXPelsPerMeter; 
  bmInfo->bmiHeader.biYPelsPerMeter = bmInfoHeader.biYPelsPerMeter; 
  bmInfo->bmiHeader.biClrUsed       = bmInfoHeader.biClrUsed; 
  bmInfo->bmiHeader.biClrImportant  = bmInfoHeader.biClrImportant; 
 
  /* 
   * Retrieve the color table. 
   * 1 << bmInfoHeader.biBitCount == 2 ^ bmInfoHeader.biBitCount 
   */ 
  ReadFile (hBmFileName, bmInfo->bmiColors, ((1<<bmInfoHeader.biBitCount) * sizeof(RGBQUAD)), 
	    &dwRead, (LPOVERLAPPED) NULL); 
  
  /* 
   * Allocate memory for the required number of 
   * bytes. 
   */ 
  hMem2   = GlobalAlloc (GHND, (bmFileHeader.bfSize - bmFileHeader.bfOffBits)); 
  lpvBits = (BYTE*) GlobalLock (hMem2); 
  
  /* Retrieve the bitmap data. */ 
  ReadFile (hBmFileName, lpvBits, (bmFileHeader.bfSize - bmFileHeader.bfOffBits), 
	    &dwRead, (LPOVERLAPPED) NULL); 
  
  /* Unlock the global memory objects and close the .BMP file. */ 
  GlobalUnlock (hMem1); 
  GlobalUnlock (hMem2); 
  CloseHandle (hBmFileName); 
  
  /* Create a bitmap from the data stored in the .BMP file. */ 
  pixmap = CreateDIBitmap (TtDisplay, &bmInfoHeader, CBM_INIT, lpvBits, bmInfo, DIB_RGB_COLORS);
  
  GetObject(pixmap, sizeof(BITMAP), (LPSTR) &bmp);
  *xif = 0;
  *yif = 0; 
  *wif = bmp.bmWidth;
  *hif = bmp.bmHeight;
# endif /* 0 */
  return pixmap;
# endif /* !_WINDOWS */
}


/*----------------------------------------------------------------------
   XbmPrint produces postscript frome an xbm file                  
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                XbmPrint (STRING fn, PictureScaling pres, int xif, int yif, int wif, int hif, int PicXArea, int PicYArea, int PicWArea, int PicHArea, int fd, unsigned int BackGroundPixel)
#else  /* __STDC__ */
void                XbmPrint (fn, pres, xif, yif, wif, hif, PicXArea, PicYArea, PicWArea, PicHArea, fd, BackGroundPixel)
STRING              fn;
PictureScaling      pres;
int                 xif;
int                 yif;
int                 wif;
int                 hif;
int                 PicXArea;
int                 PicYArea;
int                 PicWArea;
int                 PicHArea;
int                 fd;
unsigned int        BackGroundPixel;

#endif /* __STDC__ */
{
#ifdef _WINDOWS
   return;
#else  /* _WINDOWS */
   int                 delta;
   int                 xtmp, ytmp;
   float               Scx, Scy;
   XImage             *pict;
   register int        i, j, nbb;
   register char      *pt, *pt1;
   int                 wim, him;
   Pixmap              pix;

   i = XReadBitmapFile (TtDisplay, TtRootWindow, fn, &PicWArea, &PicHArea, &pix, &xtmp, &ytmp);
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
	fprintf ((FILE *) fd, "gsave %d -%d translate\n", PixelToPoint (xif), PixelToPoint (yif + hif));
	fprintf ((FILE *) fd, "%d %d %d %d DumpImage\n", pict->width, pict->height, PixelToPoint (wif), PixelToPoint (hif));

	nbb = (wim + 7) / 8;
	if (ImageByteOrder (TtDisplay) == LSBFirst)
	   LittleXBigEndian ((unsigned char *) pict->data, (long) (pict->bytes_per_line * him));
	for (j = 0, pt1 = pict->data; j < him; j++, pt1 += pict->bytes_per_line)
	  {
	     for (i = 0, pt = pt1; i < nbb; i++)
		fprintf ((FILE *) fd, "%02x", ((*pt++) & 0xff) ^ 0xff);
	     fprintf ((FILE *) fd, "\n");
	  }
	fprintf ((FILE *) fd, "grestore\n");

	/* frees the allocated space for the bitmap in memory */
	XDestroyImage (pict);
	XFreePixmap (TtDisplay, pix);
     }
#endif /* !_WINDOWS */
}

/*----------------------------------------------------------------------
   IsXbmFormat check if the file header is of an xbm format        
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool             IsXbmFormat (char* fn)
#else  /* __STDC__ */
ThotBool            IsXbmFormat (fn)
STRING              fn;

#endif /* __STDC__ */
{
#ifdef _WINDOWS
   return (FALSE);
#else  /* _WINDOWS */
   int                 status;
   int                 w, h;
   Pixmap              bitmap = None;
   int                 xHot, yHot;

   status = XReadBitmapFile (TtDisplay, TtRootWindow, fn, &w, &h, &bitmap, &xHot, &yHot);
   if (bitmap != None)
      XFreePixmap (TtDisplay, bitmap);
   return (status == BitmapSuccess);
#endif /* !_WINDOWS */
}
