
#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "picture.h"
#include "frame.h"

#define EXPORT extern
#include "picture.var"
#include "frame.var"

#include "picture.f"
#include "font.f"

/* ---------------------------------------------------------------------- */
/* |    XbmCreate lit et retourne le bitmap lu dans le fichier  | */
/* |            fn. Met a` jour xif, yif, wif, hif.                     | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
Drawable            XbmCreate (char *fn, PictureScaling pres, int *xif, int *yif, int *wif, int *hif, unsigned long BackGroundPixel, Drawable * mask1)
#else  /* __STDC__ */
Drawable            XbmCreate (fn, pres, xif, yif, wif, hif, BackGroundPixel, mask1)
char               *fn;
PictureScaling           pres;
int                *xif;
int                *yif;
int                *wif;
int                *hif;
unsigned long       BackGroundPixel;
Drawable           *mask1;
#endif /* __STDC__ */
{
  Pixmap              pix;

#ifdef NEW_WILLOWS
  return (Drawable)None;
#else  /* NEW_WILLOWS */
  int                 status;
  int                 w, h;
  Pixmap              bitmap;
  int                 xHot, yHot;

  *mask1 = None;

  status = XReadBitmapFile (TtDisplay, TtRootWindow, fn, &w, &h, &bitmap, &xHot, &yHot);
  if (status != BitmapSuccess)
    return (Drawable)None;
  else
    {
      *xif = 0;
      *yif = 0;
      *wif = w;
      *hif = h;
      
      pix = XCreatePixmap (TtDisplay, TtRootWindow, w, h, DefaultDepth (TtDisplay, DefaultScreen (TtDisplay)));
      XCopyPlane (TtDisplay, bitmap, pix, GCpicture, 0, 0, w, h, 0, 0, 1);
      XFreePixmap (TtDisplay, bitmap);
      return pix;
    }
#endif /* !NEW_WILLOWS */
}


/* ---------------------------------------------------------------------- */
/* |    XbmPrint convertit un bitmap en PostScript.             | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                XbmPrint (char *fn, PictureScaling pres, int xif, int yif, int wif, int hif, int PicXArea, int PicYArea, int PicWArea, int PicHArea, int fd, unsigned int BackGroundPixel)
#else  /* __STDC__ */
void                XbmPrint (fn, pres, xif, yif, wif, hif, PicXArea, PicYArea, PicWArea, PicHArea, fd, BackGroundPixel)
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
unsigned int        BackGroundPixel;
#endif /* __STDC__ */
{
#ifdef NEW_WILLOWS
   return;
#else  /* NEW_WILLOWS */
   int                 delta;
   int                 xtmp, ytmp;
   float               Scx, Scy;
   XImage             *Im;
   register int        i, j, nbb;
   register char      *pt, *pt1;
   int                 wim, him;
   Pixmap              pix;

   /* on va transformer le bitmap du fichier en un pixmap */
   i = XReadBitmapFile (TtDisplay, TtRootWindow, fn, &PicWArea, &PicHArea, &pix, &xtmp, &ytmp);
   if (i != BitmapSuccess)
      return;
   xtmp = 0;
   ytmp = 0;

   switch (pres)
	 {
	    case RealSize:
	       /* on centre l'image en x */
	       /* quelle place a-t-on de chaque cote ? */
	       delta = (wif - PicWArea) / 2;
	       if (delta > 0)
		 {
		    /* on a de la place entre l'if et le cf */
		    /* on a pas besoin de retailler dans le pixmap */
		    /* on va afficher le pixmap dans une boite plus petite */
		    /* decale'e de delta vers le centre */
		    xif += delta;
		    /* la largeur de la boite est celle du cf */
		    wif = PicWArea;
		 }
	       else
		 {
		    /* on a pas de place entre l'if et le cf */
		    /* on va retailler dans le pixmap pour que ca rentre */
		    /* on sauve delta pour savoir ou couper */
		    xtmp = -delta;
		    /* on met a jour PicWArea pour etre coherent */
		    PicWArea = wif;
		 }
	       /* on centre l'image en y */
	       delta = (hif - PicHArea) / 2;
	       if (delta > 0)
		 {
		    /* on a de la place entre l'if et le cf */
		    /* on a pas besoin de retailler dans le pixmap */
		    /* on va afficher le pixmap dans une boite plus petite */
		    /* decale'e de delta vers le centre */
		    yif += delta;
		    /* la hauteur de la boite est celle du cf */
		    hif = PicHArea;
		 }
	       else
		 {
		    /* on a pas de place entre l'if et le cf */
		    /* on va retailler dans le pixmap pour que ca rentre */
		    /* on sauve delta pour savoir ou couper */

		    ytmp = -delta;
		    /* on met a jour PicHArea pour etre coherent */
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
	    case FillFrame:
	       /* DumpImage fait du plein cadre avec wif et hif */
	       break;
	    default:
	       break;
	 }

   if (pix != None)
     {
	/* on transforme le pixmap en Ximage */
	/* si xtmp ou ytmp sont non nuls, ca veut dire qu'on retaille */
	/* dans le pixmap (cas RealSize) */
	Im = XGetImage (TtDisplay, pix, xtmp, ytmp,
			(unsigned int) PicWArea, (unsigned int) PicHArea,
			AllPlanes, XYPixmap);

	wim = Im->width;
	him = Im->height;
	fprintf ((FILE *) fd, "gsave %d -%d translate\n", PixelToPoint (xif), PixelToPoint (yif + hif));
	fprintf ((FILE *) fd, "%d %d %d %d DumpImage\n", Im->width, Im->height, PixelToPoint (wif), PixelToPoint (hif));

	nbb = (wim + 7) / 8;
	if (ImageByteOrder (TtDisplay) == LSBFirst)
	   SwapAllBits ((unsigned char *) Im->data, (long) (Im->bytes_per_line * him));
	for (j = 0, pt1 = Im->data; j < him; j++, pt1 += Im->bytes_per_line)
	  {
	     for (i = 0, pt = pt1; i < nbb; i++)
		fprintf ((FILE *) fd, "%02x", ((*pt++) & 0xff) ^ 0xff);
	     fprintf ((FILE *) fd, "\n");
	  }
	fprintf ((FILE *) fd, "grestore\n");
	XDestroyImage (Im);
	XFreePixmap (TtDisplay, pix);
     }
#endif /* !NEW_WILLOWS */
}

/* ---------------------------------------------------------------------- */
/* |    IsXbmFormat teste si un fichier contient un bitmap X11.      | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
boolean             IsXbmFormat (char *fn)
#else  /* __STDC__ */
boolean             IsXbmFormat (fn)
char               *fn;
#endif /* __STDC__ */
{
#ifdef NEW_WILLOWS
   return (FALSE);
#else  /* NEW_WILLOWS */
   int                 status;
   int                 w, h;
   Pixmap              bitmap = None;
   int                 xHot, yHot;

   status = XReadBitmapFile (TtDisplay, TtRootWindow, fn, &w, &h, &bitmap, &xHot, &yHot);
   if (bitmap != None)
      XFreePixmap (TtDisplay, bitmap);
   return (status == BitmapSuccess);
#endif /* !NEW_WILLOWS */
}
