/*
   pixmapdrvr.c : Auteur NabilLayaida Juin 1994.
   pixmapdrvr.c -- Implementation of X11 Pixmap Driver
   Major changes: 20-06 Error handling
   05-08 Color closeness management
   20-08 Pixmap V3.4.c integration
 */

#include "thot_gui.h"
#include "thot_sys.h"

#include "constmedia.h"
#include "typemedia.h"
#include "picture.h"
#include "frame.h"
#include "libmsg.h"
#include "message.h"

#define EXPORT extern
#include "imagedrvr.var"
#include "frame.var"

#include "xpmP.h"
#include "xpm.h"

#include "imagedrvr.f"
#include "font.f"
#ifdef __STDC__
extern void         ColorRGB (int, unsigned short *, unsigned short *, unsigned short *);

#else  /* __STDC__ */
extern void         ColorRGB ();

#endif /* __STDC__ */


/* ---------------------------------------------------------------------- */
/* |    PixmapCreateImage lit et retourne le Pixmap lu dans le fichier  | */
/* |            fn. Met a` jour xif, yif, wif, hif.                     | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
Drawable            PixmapCreateImage (char *fn, PictureScaling pres, int *xif, int *yif, int *wif, int *hif, unsigned long BackGroundPixel, Drawable * mask1)
#else  /* __STDC__ */
Drawable            PixmapCreateImage (fn, pres, xif, yif, wif, hif, BackGroundPixel, mask1)
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
#ifdef NEW_WILLOWS
   return (NULL);
#else  /* NEW_WILLOWS */
   int                 status;
   Pixmap              pixmap;
   XpmAttributes       att;
   unsigned long       valuemask = 0;

   /* parametres de chargement de l'image pixmap - repli couleurs */
   att.valuemask = valuemask;
   att.valuemask |= XpmRGBCloseness;
   att.valuemask |= XpmReturnPixels;
   att.red_closeness = 40000;
   att.green_closeness = 40000;
   att.blue_closeness = 40000;
   att.numsymbols = 1;
   att.mask_pixel = BackGroundPixel;

   /* chargement effectif de l'image par la bibliotheque Xpm rel 3.4c */
   status = XpmReadFileToPixmap (GDp (0), GRootW (0), fn, &pixmap, mask1, &att);
   if (status != XpmSuccess)
     {
       switch (status)
	 {   
	 case XpmColorError:
	   TtaDisplaySimpleMessage (INFO, LIB, XPM_COLOR_ERROR);
	   break;
	 case XpmOpenFailed:
	   TtaDisplaySimpleMessage (INFO, LIB, XPM_OPEN_FAILED);
	   break;
	 case XpmFileInvalid:
	   TtaDisplaySimpleMessage (INFO, LIB, XPM_FILE_INVALID);
	   break;
	 case XpmNoMemory:
	   TtaDisplaySimpleMessage (INFO, LIB, XPM_NO_MEMORY);
	   break;
	 case XpmColorFailed:
	   TtaDisplaySimpleMessage (INFO, LIB, XPM_COLOR_FAILED);
	   break;
	 }
       return (Drawable) None;
     }
   else
     {
       /* succes ou succes partiel => initialise la tailles de l'image */
	*wif = att.width;
	*hif = att.height;
	*xif = 0;
	*yif = 0;

	/* libere la structure temporaire attribut et sortie en beaute ! */
	XpmFreeAttributes (&att);
	att.valuemask = valuemask;/* reinitialise de valuemask avec 0 */
	return (Drawable) pixmap;
     }
#endif /* !NEW_WILLOWS */
}


/* ---------------------------------------------------------------------- */
/* |    PixmapPrintImage convertit un Pixmap en PostScript.             | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                PixmapPrintImage (char *fn, PictureScaling pres, int xif, int yif, int wif, int hif, int xcf, int ycf, int wcf, int hcf, int fd, unsigned long BackGroundPixel)
#else  /* __STDC__ */
void                PixmapPrintImage (fn, pres, xif, yif, wif, hif, xcf, ycf, wcf, hcf, fd, BackGroundPixel)
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
unsigned long       BackGroundPixel;
#endif /* __STDC__ */
{
#ifdef NEW_WILLOWS
   return;
#else  /* NEW_WILLOWS */
   int                 delta;
   int                 xtmp, ytmp;
   float               Scx, Scy;
   register int        i;
   unsigned int       *pt;
   unsigned char       pt1;
   int                 x, y;
   int                 wim /*, him */ ;
   XpmAttributes       att;
   int                 status;
   unsigned long       valuemask = 0;
   ThotColorStruct     colorTab[256];
   ThotColorStruct     exactcolor;
   XpmImage            image;
   XpmInfo             info;
   unsigned int        NbCharPerLine;
   unsigned short      red, green, blue;

   /* initialisation des parametres de chargement de l'image pour le print */
   valuemask |= XpmExactColors;
   valuemask |= XpmColorTable;
   valuemask |= XpmReturnColorTable;
   valuemask |= XpmReturnPixels;
   valuemask |= XpmHotspot;
   valuemask |= XpmCharsPerPixel;

   /* lire la pixmap sous forme de donnees et une table de couleurs   */
   /* nous evite d'allouer les couleurs a l'ecran mais de les transformer */
   /* directement en ps, fonction de bas niveau de la bibliotheque pixmap */

   status = XpmReadFileToXpmImage (fn, &image, &info);

   if (status < XpmSuccess)
     return;

   wcf = image.width;
   hcf = image.height;
   xtmp = 0;
   ytmp = 0;

   /* en fonction de la presentation definit les dimensions en sorties ps */
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
	       /* clacul des proportions dans le cas d'un zoom ! */

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

   /* NL plus besoin de faire des transformations */
   /* on ne transforme plus le pixmap en Ximage */
   /* si xtmp ou ytmp sont non nuls, ca veut dire qu'on retaille */
   /* dans le pixmap (cas RealSize) */


   /* chargement de la colortab et remplacement  de la couleur transparente par celle */
   /* du BackgoundColor defini par l'editeur Thot */

   for (i = 0; i < image.ncolors; i++)
     {
	if (strncmp (image.colorTable[i].c_color, "None", 4) == 0)

	  {
	     ColorRGB ((int) BackGroundPixel, &red, &green, &blue);
	     colorTab[i].pixel = i;
	     colorTab[i].red = red;
	     colorTab[i].green = green;
	     colorTab[i].blue = blue;
	     /*NoneColor = i; */
	     /*MaskSet = 1; */

	  }
	else
	  {
	     XParseColor (GDp (0), cmap (0), image.colorTable[i].c_color, &exactcolor);
	     colorTab[i].pixel = i;
	     colorTab[i].red = exactcolor.red;
	     colorTab[i].green = exactcolor.green;
	     colorTab[i].blue = exactcolor.blue;

	  }
     }

   wim = image.width;
   /*him = image.height; */



   /* generation du poscript , header Dumpimage2 + dimensions  */
   /* + deplacement dans la page chaque pt = RRGGBB en hexa    */


   fprintf ((FILE *) fd, "gsave %d -%d translate\n", PixelEnPt (xif, 1), PixelEnPt (yif + hif, 0));
   fprintf ((FILE *) fd, "%d %d %d %d DumpImage2\n", wcf, hcf, PixelEnPt (wif, 1), PixelEnPt (hif, 0));
   fprintf ((FILE *) fd, "\n");

   NbCharPerLine = wim;

   for (y = 0; y < hif; y++)
     {
	pt = (image.data + ((ytmp + y) * NbCharPerLine) + xtmp);
	for (x = 0; x < wif; x++)
	  {

	     /* generation des composantes RGB de l'image dans le poscript */
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

   XpmFreeXpmInfo (&info);
   XpmFreeXpmImage (&image);
   att.valuemask = valuemask;


#endif /* !NEW_WILLOWS */
}				/*PixmapPrintImage */

/* ---------------------------------------------------------------------- */
/* |    PixmapIsFormat teste si un fichier contient un Pixmap X11.      | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
Bool                PixmapIsFormat (char *fn)
#else  /* __STDC__ */
Bool                PixmapIsFormat (fn)
char               *fn;
#endif /* __STDC__ */
{
   FILE               *f;
   char                c;
   Bool                res;

   res = False;
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
		     res = True;
	       }
	  }
     }
   fclose (f);
   return res;


}				/*PixmapIsFormat */
