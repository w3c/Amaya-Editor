
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

/*=======================================================================*/
/*|                                                                     | */
/*|                            Projet THOT                              | */
/*|                                                                     | */
/*|     Module d'initialisation des e/s PostScript.                     | */
/*|                                                                     | */
/*|                                                                     | */
/*|                     I. Vatton       Fevrier 92                      | */
/*|                                                                     | */
/*|     France Logiciel numero de depot 88-39-001-00                    | */
/*|                                                                     | */
/*=======================================================================*/

#include "thot_sys.h"
#include "constmenu.h"
#include "constmedia.h"
#include "typemedia.h"
#include "frame.h"
#include "imagedrvr.h"
#include "message.h"
#include "pattern.h"

#undef EXPORT
#define EXPORT extern
#include "frame.var"
#include "thotcolor.var"


/* ---------------------------------------------------------------------- */
/* | FindOutColor finds the closest color by allocating it, or picking  | */
/* |            an already allocated color.                             | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                FindOutColor (Display * dsp, Colormap colormap, ThotColorStruct * colr)
#else  /* __STDC__ */
void                FindOutColor (dsp, colormap, colr)
Display            *dsp;
Colormap            colormap;
ThotColorStruct    *colr;

#endif /* __STDC__ */
{
}

/* ---------------------------------------------------------------------- */
/* |    InitDocColors initialise les couleurs des documents.            | */
/* |    La proce'dure lit le fichier des couleurs et cre'e la table     | */
/* |    des couleurs.                                                   | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                InitDocColors (char *name)
#else  /* __STDC__ */
void                InitDocColors (name)
char               *name;

#endif /* __STDC__ */
{
}

/* ---------------------------------------------------------------------- */
/* |    ColorPixel      retourne la valeur de la couleur de numero num. | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
unsigned long       ColorPixel (int num)
#else  /* __STDC__ */
unsigned long       ColorPixel (num)
int                 num;

#endif /* __STDC__ */
{
   return 0;
}


/* ---------------------------------------------------------------------- */
/* |    ColorRGB        retourne les trois composantes rouge-vert-bleu  | */
/* |            de la couleur de numero num.                            | */
/* |            Si la couleur n'existe pas le proce'dure rend les       | */
/* |            trois composantes de la couleur par de'faut.            | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                ColorRGB (int num, unsigned short *red, unsigned short *green, unsigned short *blue)
#else  /* __STDC__ */
void                ColorRGB (num, red, green, blue)
int                 num;
unsigned short     *red;
unsigned short     *green;
unsigned short     *blue;

#endif /* __STDC__ */
{
   if (num < NbColors && num >= 0)
     {
	*red = RGB_Table[num].red;
	*green = RGB_Table[num].green;
	*blue = RGB_Table[num].blue;
     }
   else
     {
	*red = RGB_Table[1].red;
	*green = RGB_Table[1].green;
	*blue = RGB_Table[1].blue;
     }
}


/* ---------------------------------------------------------------------- */
/* |    CreatePattern charge et retourne une pixmap du motif donne'     | */
/* |            Le parame`tre active indique s'il s'agit d'une boi^te   | */
/* |            active (1) ou non (0).                                  | */
/* |            Les parame`tres fg, bg, motif indiquent la couleur du   | */
/* |            trace', la couleur du fond et le motif.                 | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
unsigned long       CreatePattern (int disp, int RO, int active, int fg, int bg, int motif)
#else  /* __STDC__ */
unsigned long       CreatePattern (disp, RO, active, fg, bg, motif)
int                 disp;
int                 RO;
int                 active;
int                 fg;
int                 bg;
int                 motif;

#endif /* __STDC__ */
{
   unsigned long       FgPixel;
   unsigned long       BgPixel;
   Pixmap              trame;

   FgPixel = ColorPixel (fg);
   BgPixel = ColorPixel (bg);

   switch (motif)
	 {
	    case 1:
	       trame = XCreatePixmapFromBitmapData (GDp (disp), GRootW (disp), (char *) gray8_bits, gray8_width,
			     gray8_height, FgPixel, BgPixel, Gdepth (disp));
	       break;
	    case 2:
	       trame = XCreatePixmapFromBitmapData (GDp (disp), GRootW (disp), (char *) gray0_bits, gray0_width,
			     gray0_height, FgPixel, BgPixel, Gdepth (disp));
	       break;
	    case 3:
	       trame = XCreatePixmapFromBitmapData (GDp (disp), GRootW (disp), (char *) gray1_bits, gray1_width,
			     gray1_height, FgPixel, BgPixel, Gdepth (disp));
	       break;
	    case 4:
	       trame = XCreatePixmapFromBitmapData (GDp (disp), GRootW (disp), (char *) gray2_bits, gray2_width,
			     gray2_height, FgPixel, BgPixel, Gdepth (disp));
	       break;
	    case 5:
	       trame = XCreatePixmapFromBitmapData (GDp (disp), GRootW (disp), (char *) gray3_bits, gray3_width,
			     gray3_height, FgPixel, BgPixel, Gdepth (disp));
	       break;
	    case 6:
	       trame = XCreatePixmapFromBitmapData (GDp (disp), GRootW (disp), (char *) gray4_bits, gray4_width,
			     gray4_height, FgPixel, BgPixel, Gdepth (disp));
	       break;
	    case 7:
	       trame = XCreatePixmapFromBitmapData (GDp (disp), GRootW (disp), (char *) gray5_bits, gray5_width,
			     gray5_height, FgPixel, BgPixel, Gdepth (disp));
	       break;
	    case 8:
	       trame = XCreatePixmapFromBitmapData (GDp (disp), GRootW (disp), (char *) gray6_bits, gray6_width,
			     gray6_height, FgPixel, BgPixel, Gdepth (disp));
	       break;
	    case 9:
	       trame = XCreatePixmapFromBitmapData (GDp (disp), GRootW (disp), (char *) gray7_bits, gray7_width,
			     gray7_height, FgPixel, BgPixel, Gdepth (disp));
	       break;
	    case 10:
	       trame = XCreatePixmapFromBitmapData (GDp (disp), GRootW (disp), (char *) horiz1_bits, horiz1_width,
			    horiz1_height, FgPixel, BgPixel, Gdepth (disp));
	       break;
	    case 11:
	       trame = XCreatePixmapFromBitmapData (GDp (disp), GRootW (disp), (char *) horiz2_bits, horiz2_width,
			    horiz2_height, FgPixel, BgPixel, Gdepth (disp));
	       break;
	    case 12:
	       trame = XCreatePixmapFromBitmapData (GDp (disp), GRootW (disp), (char *) horiz3_bits, horiz3_width,
			    horiz3_height, FgPixel, BgPixel, Gdepth (disp));
	       break;
	    case 13:
	       trame = XCreatePixmapFromBitmapData (GDp (disp), GRootW (disp), (char *) vert1_bits, vert1_width,
			     vert1_height, FgPixel, BgPixel, Gdepth (disp));
	       break;
	    case 14:
	       trame = XCreatePixmapFromBitmapData (GDp (disp), GRootW (disp), (char *) vert2_bits, vert2_width,
			     vert2_height, FgPixel, BgPixel, Gdepth (disp));
	       break;
	    case 15:
	       trame = XCreatePixmapFromBitmapData (GDp (disp), GRootW (disp), (char *) vert3_bits, vert3_width,
			     vert3_height, FgPixel, BgPixel, Gdepth (disp));
	       break;
	    case 16:
	       trame = XCreatePixmapFromBitmapData (GDp (disp), GRootW (disp), (char *) left1_bits, left1_width,
			     left1_height, FgPixel, BgPixel, Gdepth (disp));
	       break;
	    case 17:
	       trame = XCreatePixmapFromBitmapData (GDp (disp), GRootW (disp), (char *) left2_bits, left2_width,
			     left2_height, FgPixel, BgPixel, Gdepth (disp));
	       break;
	    case 18:
	       trame = XCreatePixmapFromBitmapData (GDp (disp), GRootW (disp), (char *) left3_bits, left3_width,
			     left3_height, FgPixel, BgPixel, Gdepth (disp));
	       break;
	    case 19:
	       trame = XCreatePixmapFromBitmapData (GDp (disp), GRootW (disp), (char *) right1_bits, right1_width,
			    right1_height, FgPixel, BgPixel, Gdepth (disp));
	       break;
	    case 20:
	       trame = XCreatePixmapFromBitmapData (GDp (disp), GRootW (disp), (char *) right2_bits, right2_width,
			    right2_height, FgPixel, BgPixel, Gdepth (disp));
	       break;
	    case 21:
	       trame = XCreatePixmapFromBitmapData (GDp (disp), GRootW (disp), (char *) right3_bits, right3_width,
			    right3_height, FgPixel, BgPixel, Gdepth (disp));
	       break;
	    case 22:
	       trame = XCreatePixmapFromBitmapData (GDp (disp), GRootW (disp), (char *) square1_bits, square1_width,
			   square1_height, FgPixel, BgPixel, Gdepth (disp));
	       break;
	    case 23:
	       trame = XCreatePixmapFromBitmapData (GDp (disp), GRootW (disp), (char *) square2_bits, square2_width,
			   square2_height, FgPixel, BgPixel, Gdepth (disp));
	       break;
	    case 24:
	       trame = XCreatePixmapFromBitmapData (GDp (disp), GRootW (disp), (char *) square3_bits, square3_width,
			   square3_height, FgPixel, BgPixel, Gdepth (disp));
	       break;
	    case 25:
	       trame = XCreatePixmapFromBitmapData (GDp (disp), GRootW (disp), (char *) lozenge_bits, lozenge_width,
			   lozenge_height, FgPixel, BgPixel, Gdepth (disp));
	       break;
	    case 26:
	       trame = XCreatePixmapFromBitmapData (GDp (disp), GRootW (disp), (char *) brick_bits, brick_width,
			     brick_height, FgPixel, BgPixel, Gdepth (disp));
	       break;
	    case 27:
	       trame = XCreatePixmapFromBitmapData (GDp (disp), GRootW (disp), (char *) tile_bits, tile_width,
			      tile_height, FgPixel, BgPixel, Gdepth (disp));
	       break;
	    case 28:
	       trame = XCreatePixmapFromBitmapData (GDp (disp), GRootW (disp), (char *) sea_bits, sea_width,
			       sea_height, FgPixel, BgPixel, Gdepth (disp));
	       break;
	    case 29:
	       trame = XCreatePixmapFromBitmapData (GDp (disp), GRootW (disp), (char *) basket_bits, basket_width,
			    basket_height, FgPixel, BgPixel, Gdepth (disp));
	       break;
	    default:
	       trame = None;
	       break;
	 }

   return trame;
}
/*fin */
