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
 * epshandler.c -- Implementation of EPS pictures
 *
 * Authors: I. Vatton, N. Layaida (INRIA)
 *
 */


#include "thot_sys.h"
#include "constmedia.h"
#include "libmsg.h"
#include "message.h"
#include "typemedia.h"
#include "picture.h"
#include "frame.h"

#define THOT_EXPORT extern
#include "picture_tv.h"
#include "frame_tv.h"
#include "font_tv.h"
#include "units_f.h"


#include "picture_f.h"
#include "font_f.h"
#include "units_f.h"
#include "memory_f.h"

#define ABS(x) (x<0?-x:x)
#define MAX(x,y) (x>y?x:y)

extern Pixmap       EpsfPictureLogo;


/*----------------------------------------------------------------------
   Find EPS picture bounding box.      		             
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         GetPictureBoundaries (char *fn, int *xif, int *yif, int *wif, int *hif)
#else  /* __STDC__ */
static void         GetPictureBoundaries (fn, xif, yif, wif, hif)
char               *fn;
int                *xif;
int                *yif;
int                *wif;
int                *hif;

#endif /* __STDC__ */
{

#define BUFSIZE 1023
   FILE               *fin;
   int                 c;
   char               *pt, buff[BUFSIZE];
   int                 X2, Y2;

   *xif = 0;
   *yif = 0;
   *wif = 590;
   *hif = 840;
   fin = fopen (fn, "r");
   if (fin)
     {
	pt = buff;
	for (c = getc (fin); c != EOF; c = getc (fin))
	  {
	     if (pt - buff < BUFSIZE - 2)
		*pt++ = c;
	     if (c == '\n')
	       {
		  *(--pt) = EOS;
		  pt = buff;
		  if ((buff[0] == '%')
		      && (sscanf (buff, "%%%%BoundingBox: %d %d %d %d", xif, yif, &X2, &Y2) == 4))
		    {
		       *wif = ABS (X2 - *xif) + 1;
		       *hif = ABS (Y2 - *yif) + 1;
		    }
	       }
	  }
	fclose (fin);
     }
}


/*----------------------------------------------------------------------
   Read the bounding box of an eps file  no picture to produce here
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBitmap          EpsCreate (char *fn, PictureScaling pres, int *xif, int *yif, int *wif, int *hif, unsigned long BackGroundPixel, ThotBitmap *PicMask, int *width, int *height)
#else  /* __STDC__ */
ThotBitmap          EpsCreate (fn, pres, xif, yif, wif, hif, BackGroundPixel, PicMask, width, height)
char               *fn;
PictureScaling      pres;
int                *xif;
int                *yif;
int                *wif;
int                *hif;
unsigned long       BackGroundPixel;
ThotBitmap         *PicMask;
int                *width;
int                *height;
#endif /* __STDC__ */
{
#ifdef _WINDOWS
   return (NULL);
#else  /* _WINDOWS */

#ifndef _WINDOWS
   *PicMask = None;
#endif

   GetPictureBoundaries (fn, xif, yif, wif, hif);
   *xif = PointToPixel (*xif);
   *yif = PointToPixel (*yif);
   *wif = PointToPixel (*wif);
   *hif = PointToPixel (*hif);
   *width = *wif;
   *height = *hif;
   return ((ThotBitmap) EpsfPictureLogo);
#endif /* !_WINDOWS */
}			

/*----------------------------------------------------------------------
   Print the eps picture with the right scale and positions    
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                EpsPrint (char *fn, PictureScaling pres, int xif, int yif, int wif, int hif, int PicXArea, int PicYArea, int PicWArea, int PicHArea, FILE * fd, unsigned long BackGroundPixel)
#else  /* __STDC__ */
void                EpsPrint (fn, pres, xif, yif, wif, hif, PicXArea, PicYArea, PicWArea, PicHArea, fd, BackGroundPixel)
char               *fn;
PictureScaling      pres;
int                 xif;
int                 yif;
int                 wif;
int                 hif;
int                 PicXArea;
int                 PicYArea;
int                 PicWArea;
int                 PicHArea;
FILE               *fd;
unsigned long       BackGroundPixel;

#endif /* __STDC__ */
{
#ifdef _WINDOWS
   return;
#else  /* _WINDOWS */
   float               Scx, Scy;
   int                 x, y;
   FILE               *fin;
   int                 c;

   /* Read the picture boundaries */

   GetPictureBoundaries (fn, &PicXArea, &PicYArea, &PicWArea, &PicHArea);

   xif = PixelToPoint (xif);
   yif = PixelToPoint (yif);
   wif = PixelToPoint (wif);
   hif = PixelToPoint (hif);

   yif = yif + hif;

   fprintf (fd, "%%%% Including file %s\n", fn);
   fprintf (fd, "BEGINEPSFILE\n");

   /* define the picture clipping */

   fprintf (fd, " newpath %d %d moveto %d %d rlineto %d %d rlineto %d %d rlineto\n",
	    xif - 1, -yif, wif, 0, 0, hif, -wif, 0);
   fprintf (fd, "  closepath clip newpath\n");

   switch (pres)
	 {
	    case RealSize:
	    case XRepeat:
	    case YRepeat:
	    case FillFrame:
	       x = xif - PicXArea + (wif - PicWArea) / 2;
	       y = yif + PicYArea - (hif - PicHArea) / 2;
	       /* translate the picture to the right position */
	       fprintf (fd, "  %d %d translate\n", x, -y);
	       break;
	    case ReScale:
	       /* the same scale for x and y and center the picture */

	       Scx = (float) wif / (float) PicWArea;
	       Scy = (float) hif / (float) PicHArea;

	       if (Scy <= Scx)
		 {
		    /* updating the formulas : N */
		    Scx = Scy;
		    x = (int) ((float) xif - (Scx * (float) PicXArea) + ((float) (wif - (PicWArea * Scx)) / 2.));
		    /* recenter in X */
		    y = (int) ((float) yif + (Scy * (float) PicYArea));
		 }
	       else
		 {
		    Scy = Scx;
		    x = (int) ((float) xif - (Scx * (float) PicXArea));
		    /* recenter in  Y */
		    y = (int) ((float) yif + (Scy * (float) PicYArea) - ((float) (hif - (PicHArea * Scy)) / 2.));
		 }
	       /* we translate then we scale  */
	       fprintf (fd, "  %d %d translate %.4f %.4f scale\n", x, -y, Scx, Scy);
	       break;
	    default:
	       break;
	 }
   fin = fopen (fn, "r");
   if (fin)
     {
	c = getc (fin);
	for (; c != EOF;)
	  {
	     putc ((char) c, fd);
	     c = getc (fin);
	  }
	fclose (fin);
     }
   fprintf (fd, "\n");
   fprintf (fd, "%%%% end of file %s\n", fn);
   fprintf (fd, "ENDEPSFILE\n");

#endif /* !_WINDOWS */
}			

/*----------------------------------------------------------------------
   Chech if the picture header is of an eps file                   
  ----------------------------------------------------------------------*/
#ifdef __STDC__
boolean             IsEpsFormat (char *fn)
#else  /* __STDC__ */
boolean             IsEpsFormat (fn)
char               *fn;

#endif /* __STDC__ */
{
   FILE               *fin;
   int                 c;
   boolean             res;

   res = FALSE;
   fin = fopen (fn, "r");
   if (fin)
     {
	/* search for %! signature of the eps and ps files */
	c = getc (fin);
	if ((c != EOF) && (c == '%'))
	  {
	     c = getc (fin);
	     if ((c != EOF) && (c == '!'))
		res = TRUE;
	  }
     }
   fclose (fin);
   return res;
}
