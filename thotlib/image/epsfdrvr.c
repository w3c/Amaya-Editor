
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

/* epsfdrvr.c -- Implementation  X11 EPSF access */


#include "thot_sys.h"
#include "constmedia.h"
#include "libmsg.h"
#include "message.h"
#include "typemedia.h"
#include "picture.h"
#include "frame.h"

#define EXPORT extern
#include "imagedrvr.var"
#include "frame.var"
#include "font.var"


#include "imagedrvr.f"

#define ABS(x) (x<0?-x:x)
#define MAX(x,y) (x>y?x:y)

extern Pixmap       ImageEPSFPixmapID;

#ifdef __STDC__
extern void        *TtaGetMemory (unsigned int);
extern int          PixelEnPt (int, int);
extern int          PtEnPixel (int, int);

#else  /* __STDC__ */
extern void        *TtaGetMemory ();
extern int          PixelEnPt ();
extern int          PtEnPixel ();

#endif /* __STDC__ */


/* ------------------------------------------------------------------- */
/* | On recupere la bounding box contenue dans le fichier            | */
/* ------------------------------------------------------------------- */

#ifdef __STDC__
static int          GetBoundingBox (char *fn, int *xif, int *yif, int *wif, int *hif)

#else  /* __STDC__ */
static int          GetBoundingBox (fn, xif, yif, wif, hif)
char               *fn;
int                *xif;
int                *yif;
int                *wif;
int                *hif;

#endif /* __STDC__ */

{

#define BUFSIZE 1023
   FILE               *fin;
   int                 c;	/* modif postscript bea */
   char               *pt, buff[BUFSIZE];
   int                 X2, Y2;

   fin = fopen (fn, "r");
   if (!fin)
      return 0;

   pt = buff;
   for (c = getc (fin); c != EOF; c = getc (fin))
     {
	if (pt - buff < BUFSIZE - 2)
	   *pt++ = c;
	if (c == '\n')
	  {
	     *(--pt) = '\0';
	     pt = buff;
	     if ((buff[0] == '%')
		 && (sscanf (buff, "%%%%BoundingBox: %d %d %d %d", xif, yif, &X2, &Y2) == 4))
	       {

		  *wif = ABS (X2 - *xif) + 1;
		  *hif = ABS (Y2 - *yif) + 1;
		  fclose (fin);
		  return 1;
	       }
	  }
     }
   fclose (fin);
   return 0;
}

/* ------------------------------------------------------------------- */
/* | On initialise le driver                                         | */
/* ------------------------------------------------------------------- */

#ifdef __STDC__
int                 EPSFOpenImageDrvr (ImagingModel model)

#else  /* __STDC__ */
int                 EPSFOpenImageDrvr (model)
ImagingModel        model;

#endif /* __STDC__ */

{


   return EPSF_drvr;
}				/*EPSFOpenImageDrvr */

/* ------------------------------------------------------------------- */
/* | On ferme le driver                                              | */
/* ------------------------------------------------------------------- */

#ifdef __STDC__
void                EPSFCloseImageDrvr ()

#else  /* __STDC__ */
void                EPSFCloseImageDrvr ()
#endif				/* __STDC__ */

{
}				/*EPSFCloseImageDrvr */

/* ------------------------------------------------------------------- */
/* | On initialise le driver pour une image                          | */
/* ------------------------------------------------------------------- */

#ifdef __STDC__
void                EPSFInitImage ()

#else  /* __STDC__ */
void                EPSFInitImage ()
#endif				/* __STDC__ */

{
}				/*EPSFInitImage */

/* ---------------------------------------------------------------------- */
/* |    GetHexit recupere un chiffre hexa.                              | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static int          GetHexit (FILE * ifd)

#else  /* __STDC__ */
static int          GetHexit (ifd)
FILE               *ifd;

#endif /* __STDC__ */

{
   register int        i;
   register char       c;

   for (;;)
     {
	i = getc (ifd);
	if (i == EOF)
	  {
	     TtaDisplaySimpleMessage (INFO, LIB, EPSF_PREMAT_EOF);
	  }
	c = (char) i;
	if (c >= '0' && c <= '9')
	   return c - '0';
	else if (c >= 'A' && c <= 'F')
	   return c - 'A' + 10;
	else if (c >= 'a' && c <= 'f')
	   return c - 'a' + 10;
	/* Else ignore the rest. */
     }
}

#ifdef WWW_XWINDOWS
/* ------------------------------------------------------------------- */
/* | On recupere l'image contenue dans le fichier                    | */
/* ------------------------------------------------------------------- */

#ifdef __STDC__
XImage             *GetIncludeImage (char *fn, int *previewW, int *previewH)

#else  /* __STDC__ */
XImage             *GetIncludeImage (fn, previewW, previewH)
char               *fn;
int                *previewW;
int                *previewH;

#endif /* __STDC__ */

{
#ifdef NEW_WILLOWS
   return (NULL);
#else  /* NEW_WILLOWS */

#define CONTIGUOUS 0

   FILE               *fin;
   int                 c;	/* modif postscript bea */
   char               *pt, buff[BUFSIZE];
   char               *imageMemory, *dest;
   float               W, H, BPS, L;
   int                 i, j, wline, nline;
   register int        val1, val2, nibble;
   int                 imageSize;
   XImage             *preview;

   fin = fopen (fn, "r");
   if (!fin)
      return None;

/**todo: on va d'abord chercher le numero de version EPSF */
   pt = buff;
   for (c = getc (fin); c != EOF; c = getc (fin))
     {
	if (pt - buff < BUFSIZE - 2)
	   *pt++ = c;
	if (c == '\n')
	  {
	     *(--pt) = '\0';
	     pt = buff;
	     if ((buff[0] == '%')
		 && (sscanf (buff, "%%%%BeginPreview: %f %f %f %f", &W, &H, &BPS, &L) == 4))
	       {
		  wline = W;
		  nline = H;
		  imageSize = (int) (((W + 7) / 8) * H);
		  imageMemory = (char *) TtaGetMemory (imageSize);
		  dest = imageMemory;
		  for (i = 0; i < nline; i++)
		    {
		       for (j = 0; j < wline; j += 8)
			 {
			    switch ((int) BPS)
				  {
				     case 1:
					val1 = GetHexit (fin) << 4;
					val1 += GetHexit (fin);
					*dest++ = val1;
					break;
				     case 2:
					val1 = GetHexit (fin) << 4;
					val1 += GetHexit (fin);
					val2 = GetHexit (fin) << 4;
					val2 += GetHexit (fin);
					val1 = (val1 & 0xAA);
					val2 = (val2 & 0xAA);
					nibble = (val1 & 0x80);
					nibble = nibble | ((val1 << 1) & 0x40);
					nibble = nibble | ((val1 << 2) & 0x20);
					nibble = nibble | ((val1 << 3) & 0x10);
					nibble = nibble | ((val2 >> 1) & 0x01);
					nibble = nibble | ((val2 >> 2) & 0x02);
					nibble = nibble | ((val2 >> 3) & 0x04);
					nibble = nibble | ((val2 >> 4) & 0x08);
					*dest++ = nibble;
					break;
				     case 4:
/**todo */
					val1 = GetHexit (fin) << 4;
					val1 += GetHexit (fin);
					val2 = GetHexit (fin) << 4;
					val2 += GetHexit (fin);
					val1 = GetHexit (fin) << 4;
					val1 += GetHexit (fin);
					val2 = GetHexit (fin) << 4;
					val2 += GetHexit (fin);
					break;
				     case 8:
/**todo */
					val1 = GetHexit (fin) << 4;
					val1 += GetHexit (fin);
					val2 = GetHexit (fin) << 4;
					val2 += GetHexit (fin);
					val1 = GetHexit (fin) << 4;
					val1 += GetHexit (fin);
					val2 = GetHexit (fin) << 4;
					val2 += GetHexit (fin);
					val1 = GetHexit (fin) << 4;
					val1 += GetHexit (fin);
					val2 = GetHexit (fin) << 4;
					val2 += GetHexit (fin);
					val1 = GetHexit (fin) << 4;
					val1 += GetHexit (fin);
					val2 = GetHexit (fin) << 4;
					val2 += GetHexit (fin);
					break;
				  }
			 }
		    }
		  if (ImageByteOrder (GDp (0)) == LSBFirst)
		     SwapAllBits ((unsigned char *) imageMemory, imageSize);

#ifndef NEW_WILLOWS
		  preview = XCreateImage (GDp (0), DefaultVisual (GDp (0), ThotScreen (0)), 1, XYPixmap,
			       0, imageMemory, wline, nline, 8, CONTIGUOUS);
#endif	/*  */
		  *previewW = wline;
		  *previewH = nline;
		  fclose (fin);
		  return preview;
	       }
	  }
     }
   fclose (fin);
   return None;
#endif /* !NEW_WILLOWS */
}
#endif /* WWW_XWINDOWS */



/* ------------------------------------------------------------------- */
/* | On lit une image contenue dans le fichier fn                    | */
/* ------------------------------------------------------------------- */

#ifdef __STDC__
ThotBitmap          EPSFCreateImage (char *fn, PictureScaling pres, int *xif, int *yif, int *wif, int *hif, unsigned long BackGroundPixel, Drawable * mask)

#else  /* __STDC__ */
ThotBitmap          EPSFCreateImage (fn, pres, xif, yif, wif, hif, BackGroundPixel, mask)
char               *fn;
PictureScaling           pres;
int                *xif;
int                *yif;
int                *wif;
int                *hif;
unsigned long       BackGroundPixel;
Drawable           *mask;

#endif /* __STDC__ */

{
#ifdef NEW_WILLOWS
   return (NULL);
#else  /* NEW_WILLOWS */

#ifndef NEW_WILLOWS
   *mask = None;
#endif
/**todo: tenir compte de pres */

   if (!GetBoundingBox (fn, xif, yif, wif, hif))
     {
	/* on ne l'a pas trouve */
	/* on rend le format A4 comme BoundingBox */

	*xif = 0;
	*yif = 0;
	*wif = 590;
	*hif = 840;

	TtaDisplayMessage (INFO, TtaGetMessage(LIB, EPSF_NO_BOUNDING_BOX_IN_PS_FILE), fn);
     }

   *xif = PtEnPixel (*xif, 1);
   *yif = PtEnPixel (*yif, 0);
   *wif = PtEnPixel (*wif, 1);
   *hif = PtEnPixel (*hif, 0);


   return (ThotBitmap) ImageEPSFPixmapID;

#endif /* !NEW_WILLOWS */
}				/*EPSFCreateImage */



/* ------------------------------------------------------------------- */
/* | On imprime une image                                            | */
/* ------------------------------------------------------------------- */

#ifdef __STDC__
void                EPSFPrintImage (char *fn, PictureScaling pres, int xif, int yif, int wif, int hif, int xcf, int ycf, int wcf, int hcf, FILE * fd, unsigned long BackGroundPixel)

#else  /* __STDC__ */
void                EPSFPrintImage (fn, pres, xif, yif, wif, hif, xcf, ycf, wcf, hcf, fd, BackGroundPixel)
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
FILE               *fd;
unsigned long       BackGroundPixel;

#endif /* __STDC__ */

{
#ifdef NEW_WILLOWS
   return;
#else  /* NEW_WILLOWS */
   float               Scx, Scy;
   int                 x, y;
   FILE               *fin;
   int                 c;

   /* On relit la bounding box au moment de l'impression */
   x = GetBoundingBox (fn, &xcf, &ycf, &wcf, &hcf);
   xif = PixelEnPt (xif, 1);
   yif = PixelEnPt (yif, 0);
   wif = PixelEnPt (wif, 1);
   hif = PixelEnPt (hif, 0);


   yif = yif + hif;

   fprintf (fd, "%%%% Including file %s\n", fn);
   fprintf (fd, "BEGINEPSFILE\n");

   /* on definit un clip avec l'image frame */

   fprintf (fd, " newpath %d %d moveto %d %d rlineto %d %d rlineto %d %d rlineto\n",
	    xif - 1, -yif, wif, 0, 0, hif, -wif, 0);
   fprintf (fd, "  closepath clip newpath\n");

   switch (pres)
	 {
	    case RealSize:
	       x = xif - xcf + (wif - wcf) / 2;
	       y = yif + ycf - (hif - hcf) / 2;
	       /* on fait juste un translate au bon endroit */
	       fprintf (fd, "  %d %d translate\n", x, -y);
	       break;
	    case ReScale:
	       /* meme echelle X et Y, centre'  */
	       Scx = (float) wif / (float) wcf;
	       Scy = (float) hif / (float) hcf;
	       /* on cherche dans quel sens ca risque de coincer */
	       if (Scy <= Scx)
		 {
		    /* mise ajour des formules Nabil */
		    Scx = Scy;
		    x = (int) ((float) xif - (Scx * (float) xcf) + ((float) (wif - (wcf * Scx)) / 2.));
		    /* recentrer en X */
		    y = (int) ((float) yif + (Scy * (float) ycf));
		 }
	       else
		 {
		    Scy = Scx;
		    x = (int) ((float) xif - (Scx * (float) xcf));
		    /* recentrer en Y */
		    y = (int) ((float) yif + (Scy * (float) ycf) - ((float) (hif - (hcf * Scy)) / 2.));
		 }
	       /* on fait un translate et un scale */
	       fprintf (fd, "  %d %d translate %.4f %.4f scale\n", x, -y, Scx, Scy);
	       break;
	    case FillFrame:
	       /* remplissage du cadre (echelle XY) */
	       Scx = (float) wif / (float) wcf;
	       Scy = (float) hif / (float) hcf;
	       x = (int) ((float) xif - (Scx * (float) xcf));
	       y = (int) ((float) yif + (Scy * (float) ycf));
	       /* on fait un translate et un scale */
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

#endif /* !NEW_WILLOWS */
}				/*EPSFPrintImage */

/* ------------------------------------------------------------------- */
/* | On teste si une image est bien du PostScript                    | */
/* ------------------------------------------------------------------- */

#ifdef __STDC__
boolean             EPSFIsFormat (char *fn)

#else  /* __STDC__ */
boolean             EPSFIsFormat (fn)
char               *fn;

#endif /* __STDC__ */

{
   FILE               *fin;
   int                 c;	/* modif postscript bea */
   boolean             res;

   res = False;
   fin = fopen (fn, "r");
   if (fin)
     {
	c = getc (fin);
	if ((c != EOF) && (c == '%'))
	   /* on cherche %! dans les 2 premiers caracteres */
	  {
	     c = getc (fin);
	     if ((c != EOF) && (c == '!'))
		res = True;
	  }
     }
   fclose (fin);
   return res;
}				/*EPSFIsFormat */
