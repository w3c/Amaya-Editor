
/* epshandler.c -- Implementation EPS pictures */


#include "thot_sys.h"
#include "constmedia.h"
#include "libmsg.h"
#include "message.h"
#include "typemedia.h"
#include "picture.h"
#include "frame.h"

#define EXPORT extern
#include "picture_tv.h"
#include "frame_tv.h"
#include "font_tv.h"


#include "picture_f.h"
#include "font_f.h"
#include "memory_f.h"

#define ABS(x) (x<0?-x:x)
#define MAX(x,y) (x>y?x:y)

extern Pixmap       EpsfPictureLogo;


/* ------------------------------------------------------------------- */
/* | Find EPS bounding box.				             | */
/* ------------------------------------------------------------------- */
#ifdef __STDC__
static void FindBoundingBox (char *fn, int *xif, int *yif, int *wif, int *hif)
#else  /* __STDC__ */
static void FindBoundingBox (fn, xif, yif, wif, hif)
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
	      *(--pt) = '\0';
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
	     TtaDisplaySimpleMessage (INFO, LIB, PREMAT_EOF);
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



/* ------------------------------------------------------------------- */
/* | On lit une image contenue dans le fichier fn                    | */
/* ------------------------------------------------------------------- */
#ifdef __STDC__
ThotBitmap          EpsCreate (char *fn, PictureScaling pres, int *xif, int *yif, int *wif, int *hif, unsigned long BackGroundPixel, Drawable * PicMask)
#else  /* __STDC__ */
ThotBitmap          EpsCreate (fn, pres, xif, yif, wif, hif, BackGroundPixel, PicMask)
char               *fn;
PictureScaling           pres;
int                *xif;
int                *yif;
int                *wif;
int                *hif;
unsigned long       BackGroundPixel;
Drawable           *PicMask;
#endif /* __STDC__ */
{
#ifdef NEW_WILLOWS
   return (NULL);
#else  /* NEW_WILLOWS */

#ifndef NEW_WILLOWS
   *PicMask = None;
#endif

   FindBoundingBox (fn, xif, yif, wif, hif);
   *xif = PointToPixel (*xif);
   *yif = PointToPixel (*yif);
   *wif = PointToPixel (*wif);
   *hif = PointToPixel (*hif);
   return (ThotBitmap) EpsfPictureLogo;
#endif /* !NEW_WILLOWS */
}				/*EpsCreate */



/* ------------------------------------------------------------------- */
/* | On imprime une image                                            | */
/* ------------------------------------------------------------------- */
#ifdef __STDC__
void                EpsPrint (char *fn, PictureScaling pres, int xif, int yif, int wif, int hif, int PicXArea, int PicYArea, int PicWArea, int PicHArea, FILE * fd, unsigned long BackGroundPixel)
#else  /* __STDC__ */
void                EpsPrint (fn, pres, xif, yif, wif, hif, PicXArea, PicYArea, PicWArea, PicHArea, fd, BackGroundPixel)
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
   FindBoundingBox (fn, &PicXArea, &PicYArea, &PicWArea, &PicHArea);
   xif = PixelToPoint (xif);
   yif = PixelToPoint (yif);
   wif = PixelToPoint (wif);
   hif = PixelToPoint (hif);

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
	       x = xif - PicXArea + (wif - PicWArea) / 2;
	       y = yif + PicYArea - (hif - PicHArea) / 2;
	       /* on fait juste un translate au bon endroit */
	       fprintf (fd, "  %d %d translate\n", x, -y);
	       break;
	    case ReScale:
	       /* meme echelle X et Y, centre'  */
	       Scx = (float) wif / (float) PicWArea;
	       Scy = (float) hif / (float) PicHArea;
	       /* on cherche dans quel sens ca risque de coincer */
	       if (Scy <= Scx)
		 {
		    /* mise ajour des formules Nabil */
		    Scx = Scy;
		    x = (int) ((float) xif - (Scx * (float) PicXArea) + ((float) (wif - (PicWArea * Scx)) / 2.));
		    /* recentrer en X */
		    y = (int) ((float) yif + (Scy * (float) PicYArea));
		 }
	       else
		 {
		    Scy = Scx;
		    x = (int) ((float) xif - (Scx * (float) PicXArea));
		    /* recentrer en Y */
		    y = (int) ((float) yif + (Scy * (float) PicYArea) - ((float) (hif - (PicHArea * Scy)) / 2.));
		 }
	       /* on fait un translate et un scale */
	       fprintf (fd, "  %d %d translate %.4f %.4f scale\n", x, -y, Scx, Scy);
	       break;
	    case FillFrame:
	       /* remplissage du cadre (echelle XY) */
	       Scx = (float) wif / (float) PicWArea;
	       Scy = (float) hif / (float) PicHArea;
	       x = (int) ((float) xif - (Scx * (float) PicXArea));
	       y = (int) ((float) yif + (Scy * (float) PicYArea));
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
}				/*EpsPrint */

/* ------------------------------------------------------------------- */
/* | On teste si une image est bien du PostScript                    | */
/* ------------------------------------------------------------------- */
#ifdef __STDC__
boolean             IsEpsFormat (char *fn)
#else  /* __STDC__ */
boolean             IsEpsFormat (fn)
char               *fn;
#endif /* __STDC__ */
{
   FILE               *fin;
   int                 c;	/* modif postscript bea */
   boolean             res;

   res = FALSE;
   fin = fopen (fn, "r");
   if (fin)
     {
	c = getc (fin);
	if ((c != EOF) && (c == '%'))
	   /* on cherche %! dans les 2 premiers caracteres */
	  {
	     c = getc (fin);
	     if ((c != EOF) && (c == '!'))
		res = TRUE;
	  }
     }
   fclose (fin);
   return res;
}				/*IsEpsFormat */
