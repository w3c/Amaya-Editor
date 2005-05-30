/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2005
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * epshandler.c -- Implementation of EPS pictures
 *
 * Authors: I. Vatton, N. Layaida (INRIA)
 *
 */
#include "thot_gui.h"
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

#ifndef ABS
#define ABS(x) (x<0?-x:x)
#endif
extern ThotPixmap       EpsfPictureLogo;

/*----------------------------------------------------------------------
   Find EPS picture bounding box.      		             
  ----------------------------------------------------------------------*/
static void GetPictureBoundaries (char *fn, int *xif, int *yif, int *wif,
				  int *hif)
{

#define BUFSIZE 1023
  FILE               *fin;
  char               *pt; 
  char                buff[BUFSIZE];
  int                 c;
  int                 X2, Y2;

  *xif = 0;
  *yif = 0;
  *wif = 590;
  *hif = 840;
  fin = TtaReadOpen (fn);
  if (fin)
    {
      pt = buff;
      for (c = getc (fin); c != EOF; c = getc (fin))
	{
	  if (pt - buff < BUFSIZE - 2)
	    *pt++ = (char) c;
	  if (c == '\n')
	    {
	      *(--pt) = EOS;
	      pt = buff;
	      if (buff[0] == '%' &&
		  sscanf (buff, "%%%%BoundingBox: %d %d %d %d", xif,
			  yif, &X2, &Y2) == 4)
		{
		  *wif = ABS (X2 - *xif) + 1;
		  *hif = ABS (Y2 - *yif) + 1;
		}
	    }
	}
      TtaReadClose (fin);
    }
}


/*----------------------------------------------------------------------
   Read the bounding box of an eps file  no picture to produce here
  ----------------------------------------------------------------------*/
ThotDrawable EpsCreate (char *fn, ThotPictInfo *imageDesc, int *xif, int *yif,
		    int *wif, int *hif, int bgColor, int *width,
		    int *height, int zoom)
{
#ifdef _WINGUI
   return (NULL);
#else  /* _WINGUI */
   GetPictureBoundaries (fn, xif, yif, wif, hif);
   *width = *wif;
   *height = *hif;
   return ((ThotDrawable) EpsfPictureLogo);
#endif /* _WINGUI */
}			

/*----------------------------------------------------------------------
   Print the eps picture with the right scale and positions    
  ----------------------------------------------------------------------*/
void EpsPrint (char *fn, PictureScaling pres, int xif, int yif, int wif,
	       int hif, FILE *fd, int bgColor)
{
#ifndef _WINGUI
   float               Scx, Scy;
   FILE               *fin;
   int                 x, y;
   int                 picX, picY, picW, picH;
   int                 c;

   /* Read the picture boundaries */
   GetPictureBoundaries (fn, &picX, &picY, &picW, &picH);
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
       x = xif - picX + (wif - picW) / 2;
       y = yif + picY - (hif - picH) / 2;
       /* translate the picture to the right position */
       fprintf (fd, "  %d %d translate\n", x, -y);
       break;
     case ReScale:
       /* the same scale for x and y and center the picture */
       
       Scx = (float) wif / (float) picW;
       Scy = (float) hif / (float) picH;
       
       if (Scy <= Scx)
	 {
	   /* updating the formulas : N */
	   Scx = Scy;
	   x = (int) ((float) xif - (Scx * (float) picX) + ((float) (wif - (picW * Scx)) / 2.));
	   /* recenter in X */
	   y = (int) ((float) yif + (Scy * (float) picY));
	 }
       else
	 {
	   Scy = Scx;
	   x = (int) ((float) xif - (Scx * (float) picX));
	   /* recenter in  Y */
	   y = (int) ((float) yif + (Scy * (float) picY) - ((float) (hif - (picH * Scy)) / 2.));
	 }
       /* we translate then we scale  */
       fprintf (fd, "  %d %d translate %.4f %.4f scale\n", x, -y, Scx, Scy);
       break;
     default:
       break;
     }
   fin = TtaReadOpen (fn);
   if (fin)
     {
       c = getc (fin);
       for (; c != EOF;)
	 {
	   putc ((char) c, fd);
	   c = getc (fin);
	 }
       TtaReadClose (fin);
     }
   fprintf (fd, "\n");
   fprintf (fd, "%%%% end of file %s\n", fn);
   fprintf (fd, "ENDEPSFILE\n");
#endif /* _WINGUI */
}			

/*----------------------------------------------------------------------
   Chech if the picture header is of an eps file                   
  ----------------------------------------------------------------------*/
ThotBool IsEpsFormat (char *fn)
{
   FILE               *fin;
   int                 c;
   ThotBool            res;

   res = FALSE;
   fin = TtaReadOpen (fn);
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
   TtaReadClose (fin);
   return res;
}
