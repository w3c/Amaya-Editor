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
 * xpmhandler.c  Pixmap V3.4.c
 *
 * Author: N. Layaida (INRIA)
 *         R. Guetari (W3C/INRIA) - Adaptation to Windows platforms.
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

#include "xpmP.h"
#include "xpm.h"

#include "picture_f.h"
#include "inites_f.h"
#include "font_f.h"
#include "units_f.h"


/*----------------------------------------------------------------------
   XpmCreate reads and produces the bitmap read from the file      
   fn. updates the wif, hif, xif , yif                     
  ----------------------------------------------------------------------*/
#ifdef __STDC__
Drawable            XpmCreate (char *fn, PictureScaling pres, int *xif, int *yif, int *wif, int *hif, unsigned long BackGroundPixel, Drawable *mask1, int *width, int *height)
#else  /* __STDC__ */
Drawable            XpmCreate (fn, pres, xif, yif, wif, hif, BackGroundPixel, mask1, width, height)
char               *fn;
PictureScaling      pres;
int                *xif;
int                *yif;
int                *wif;
int                *hif;
unsigned long       BackGroundPixel;
Drawable           *mask1;
int                *width;
int                *height;
#endif /* __STDC__ */
{
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

# ifndef _WINDOWS
  status = XpmReadFileToPixmap (TtDisplay, TtRootWindow, fn, &pixmap, mask1, &att);
# endif  /* _WINDOWS */
  /* return image dimensions */
  *width = att.width;
  *height = att.height;
  
  if (status != XpmSuccess)
    {
      switch (status)
	{   
	case XpmColorError:
	  TtaDisplaySimpleMessage (INFO, LIB, TMSG_COLOR_INCORRECT);
	  break;
	case XpmOpenFailed:
	  TtaDisplaySimpleMessage (INFO, LIB, TMSG_XPM_OPEN_ERR);
	  break;
	case XpmFileInvalid:
	  TtaDisplaySimpleMessage (INFO, LIB, TMSG_XPM_FILE_INCORRECT);
	  break;
	case XpmNoMemory:
	  TtaDisplaySimpleMessage (INFO, LIB, TMSG_XPM_NO_MEM);
	  break;
	case XpmColorFailed:
	  TtaDisplaySimpleMessage (INFO, LIB, TMSG_XPM_COLOR_ERR);
	  break;
	}
      return ((Drawable) None);
    }
  else
    {
      *wif = att.width;
      *hif = att.height;
      *xif = 0;
      *yif = 0;
      
      /* frees the library's internal structures */
#     ifndef _WINDOWS
      XpmFreeAttributes (&att);
#     endif  /* _WINDOWS */
      att.valuemask = valuemask;/* reinitialises the value mask */
      return (Drawable) pixmap;
    }
}


/*----------------------------------------------------------------------
   XpmPrint converts an xpm file to PostScript.                    
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                XpmPrint (char *fn, PictureScaling pres, int xif, int yif, int wif, int hif, int PicXArea, int PicYArea, int PicWArea, int PicHArea, int fd, unsigned long BackGroundPixel)
#else  /* __STDC__ */
void                XpmPrint (fn, pres, xif, yif, wif, hif, PicXArea, PicYArea, PicWArea, PicHArea, fd, BackGroundPixel)
char               *fn;
PictureScaling           pres;
int                 xif;
int                 yif;
int                 wif;
int                 hif;
int                 PicXArea;
int                 PicYArea;
int                 PicWArea;
int                 PicHArea;
int                 fd;
unsigned long       BackGroundPixel;
#endif /* __STDC__ */
{
   int                 delta;
   int                 xtmp, ytmp;
   float               Scx, Scy;
   register int        i;
   unsigned int       *pt;
   unsigned char       pt1;
   int                 x, y;
   int                 wim ;
#  ifndef _WINDOWS 
   XpmAttributes       att;
   XpmInfo             info;
   ThotColorStruct     exactcolor;
#  endif /* !_WINDOWS */
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

#  ifndef _WINDOWS
   status = XpmReadFileToXpmImage (fn, &image, &info);
#  endif  /* _WINDOWS */

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

   /* reads the colorspace palette to produce the ps */

   for (i = 0; i < image.ncolors; i++)
     {
	if (strncmp (image.colorTable[i].c_color, "None", 4) == 0)

	  {
	     TtaGiveThotRGB ((int) BackGroundPixel, &red, &green, &blue);
	     colorTab[i].red = red;
	     colorTab[i].green = green;
	     colorTab[i].blue = blue;
#            ifndef _WINDOWS
	     colorTab[i].pixel = i;
	     /*NoneColor = i; */
	     /*MaskSet = 1; */
#            endif /* _WINDOWS */

	  }
	else
	  {
#            ifndef _WINDOWS
	     XParseColor (TtDisplay, TtCmap, image.colorTable[i].c_color, &exactcolor);
	     colorTab[i].pixel = i;
	     colorTab[i].red = exactcolor.red;
	     colorTab[i].green = exactcolor.green;
	     colorTab[i].blue = exactcolor.blue;
#            else  /* _WINDOWS */
#            endif /* _WINDOWS */

	  }
     }

   wim = image.width;
   /* generation of the poscript , header Dumpimage2 + dimensions  */
   /* + picture location. Each pixel = RRGGBB in  hexa    */
   fprintf ((FILE *) fd, "gsave %d -%d translate\n", PixelToPoint (xif), PixelToPoint (yif + hif));
   fprintf ((FILE *) fd, "%d %d %d %d DumpImage2\n", PicWArea, PicHArea, PixelToPoint (wif), PixelToPoint (hif));
   fprintf ((FILE *) fd, "\n");

   NbCharPerLine = wim;

   for (y = 0; y < hif; y++)
     {
	pt = (image.data + ((ytmp + y) * NbCharPerLine) + xtmp);
	for (x = 0; x < wif; x++)
	  {

	     /* RGB components generation */
	     pt1 = (unsigned char) (*pt);
	     fprintf ((FILE *) fd, "%02x%02x%02x",
		      (colorTab[pt1].red) & 0xff,
		      (colorTab[pt1].green) & 0xff,
		      (colorTab[pt1].blue) & 0xff);

	     pt++;
	  }
	fprintf ((FILE *) fd, "\n");
     }

   fprintf ((FILE *) fd, "\n");
   fprintf ((FILE *) fd, "grestore\n");
   fprintf ((FILE *) fd, "\n");
#  ifndef _WINDOWS
   XpmFreeXpmInfo (&info);
   XpmFreeXpmImage (&image);
   att.valuemask = valuemask;
#  endif /* _WINDOWS */
}			

/*----------------------------------------------------------------------
   IsXpmFormat check if the file header is of a pixmap                
  ----------------------------------------------------------------------*/
#ifdef __STDC__
boolean                IsXpmFormat (char *fn)
#else  /* __STDC__ */
boolean                IsXpmFormat (fn)
char               *fn;
#endif /* __STDC__ */
{
   FILE               *f;
   char                c;
   boolean                res;

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
