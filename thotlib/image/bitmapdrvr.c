
#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "picture.h"
#include "frame.h"

#define EXPORT extern
#include "imagedrvr.var"
#include "frame.var"

#include "imagedrvr.f"
#include "font.f"

/* ---------------------------------------------------------------------- */
/* |    BitmapCreateImage lit et retourne le bitmap lu dans le fichier  | */
/* |            fn. Met a` jour xif, yif, wif, hif.                     | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
Drawable            BitmapCreateImage (char *fn, PictureScaling pres, int *xif, int *yif, int *wif, int *hif, unsigned long BackGroundPixel, Drawable * mask1)
#else  /* __STDC__ */
Drawable            BitmapCreateImage (fn, pres, xif, yif, wif, hif, BackGroundPixel, mask1)
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

  status = XReadBitmapFile (GDp (0), GRootW (0), fn, &w, &h, &bitmap, &xHot, &yHot);
  if (status != BitmapSuccess)
    return (Drawable)None;
  else
    {
      *xif = 0;
      *yif = 0;
      *wif = w;
      *hif = h;
      
      pix = XCreatePixmap (GDp (0), GRootW (0), w, h, DefaultDepth (GDp (0), DefaultScreen (GDp (0))));
      XCopyPlane (GDp (0), bitmap, pix, GCpicture, 0, 0, w, h, 0, 0, 1);
      XFreePixmap (GDp (0), bitmap);
      return pix;
    }
#endif /* !NEW_WILLOWS */
}


/* ---------------------------------------------------------------------- */
/* |    BitmapPrintImage convertit un bitmap en PostScript.             | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                BitmapPrintImage (char *fn, PictureScaling pres, int xif, int yif, int wif, int hif, int xcf, int ycf, int wcf, int hcf, int fd, unsigned int BackGroundPixel)
#else  /* __STDC__ */
void                BitmapPrintImage (fn, pres, xif, yif, wif, hif, xcf, ycf, wcf, hcf, fd, BackGroundPixel)
char               *fn;
PictureScaling           pres;
int                 xif;
int                 yif;
int                 wif;
int                 hif;
int                 xcf;
int                 ycf;
int                 wcf;
int                 hcf;
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
   i = XReadBitmapFile (GDp (0), GRootW (0), fn, &wcf, &hcf, &pix, &xtmp, &ytmp);
   if (i != BitmapSuccess)
      return;
   xtmp = 0;
   ytmp = 0;

   switch (pres)
	 {
	    case RealSize:
	       /* on centre l'image en x */
	       /* quelle place a-t-on de chaque cote ? */
	       delta = (wif - wcf) / 2;
	       if (delta > 0)
		 {
		    /* on a de la place entre l'if et le cf */
		    /* on a pas besoin de retailler dans le pixmap */
		    /* on va afficher le pixmap dans une boite plus petite */
		    /* decale'e de delta vers le centre */
		    xif += delta;
		    /* la largeur de la boite est celle du cf */
		    wif = wcf;
		 }
	       else
		 {
		    /* on a pas de place entre l'if et le cf */
		    /* on va retailler dans le pixmap pour que ca rentre */
		    /* on sauve delta pour savoir ou couper */
		    xtmp = -delta;
		    /* on met a jour wcf pour etre coherent */
		    wcf = wif;
		 }
	       /* on centre l'image en y */
	       delta = (hif - hcf) / 2;
	       if (delta > 0)
		 {
		    /* on a de la place entre l'if et le cf */
		    /* on a pas besoin de retailler dans le pixmap */
		    /* on va afficher le pixmap dans une boite plus petite */
		    /* decale'e de delta vers le centre */
		    yif += delta;
		    /* la hauteur de la boite est celle du cf */
		    hif = hcf;
		 }
	       else
		 {
		    /* on a pas de place entre l'if et le cf */
		    /* on va retailler dans le pixmap pour que ca rentre */
		    /* on sauve delta pour savoir ou couper */

		    ytmp = -delta;
		    /* on met a jour hcf pour etre coherent */
		    hcf = hif;
		 }
	       break;
	    case ReScale:
	       if ((float) hcf / (float) wcf <= (float) hif / (float) wif)
		 {
		    Scx = (float) wif / (float) wcf;
		    yif += (hif - (hcf * Scx)) / 2;
		    hif = hcf * Scx;
		 }
	       else
		 {
		    Scy = (float) hif / (float) hcf;
		    xif += (wif - (wcf * Scy)) / 2;
		    wif = wcf * Scy;
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
	Im = XGetImage (GDp (0), pix, xtmp, ytmp,
			(unsigned int) wcf, (unsigned int) hcf,
			AllPlanes, XYPixmap);

	wim = Im->width;
	him = Im->height;
	fprintf ((FILE *) fd, "gsave %d -%d translate\n", PixelEnPt (xif, 1), PixelEnPt (yif + hif, 0));
	fprintf ((FILE *) fd, "%d %d %d %d DumpImage\n", Im->width, Im->height, PixelEnPt (wif, 1), PixelEnPt (hif, 0));

	nbb = (wim + 7) / 8;
	if (ImageByteOrder (GDp (0)) == LSBFirst)
	   SwapAllBits ((unsigned char *) Im->data, (long) (Im->bytes_per_line * him));
	for (j = 0, pt1 = Im->data; j < him; j++, pt1 += Im->bytes_per_line)
	  {
	     for (i = 0, pt = pt1; i < nbb; i++)
		fprintf ((FILE *) fd, "%02x", ((*pt++) & 0xff) ^ 0xff);
	     fprintf ((FILE *) fd, "\n");
	  }
	fprintf ((FILE *) fd, "grestore\n");
	XDestroyImage (Im);
	XFreePixmap (GDp (0), pix);
     }
#endif /* !NEW_WILLOWS */
}

/* ---------------------------------------------------------------------- */
/* |    BitmapIsFormat teste si un fichier contient un bitmap X11.      | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
boolean             BitmapIsFormat (char *fn)
#else  /* __STDC__ */
boolean             BitmapIsFormat (fn)
char               *fn;
#endif /* __STDC__ */
{
#ifdef NEW_WILLOWS
   return (False);
#else  /* NEW_WILLOWS */
   int                 status;
   int                 w, h;
   Pixmap              bitmap = None;
   int                 xHot, yHot;

   status = XReadBitmapFile (GDp (0), GRootW (0), fn, &w, &h, &bitmap, &xHot, &yHot);
   if (bitmap != None)
      XFreePixmap (GDp (0), bitmap);
   return (status == BitmapSuccess);
#endif /* !NEW_WILLOWS */
}
