
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

/*
   es.c : Gestion des affichages X
   I. Vatton - Juillet 87
   IV : Fevrier 92 introduction de la couleur
   IV : Aout 92 coupure des mots
   IV : Juin 93 polylines
 */

#include "thot_sys.h"
#ifdef SYSV
#endif
#include "constmedia.h"
#include "typemedia.h"
#include "frame.h"


#undef EXPORT
#define EXPORT extern
#include "font.var"
#include "frame.var"
#include "edit.var"
#include "thotcolor.var"

extern ThotColorStruct cblack;

/*debut */
#define	MAX_pile	50
#define	le_milieu(v1, v2)	((v1+v2)/2.0)
#define SEG_SPLINE      5
#define ALLOC_POINTS    300
static ThotPoint   *points;	/* Gestion des points de controle de courbes */
static int          npoints;
static int          MAX_points;

typedef struct pile_
  {
     float               x1, y1, x2, y2, x3, y3, x4, y4;
  }
Pile;
static Pile         pile[MAX_pile];
static int          pile_profond;

/*fin */

#include "font_f.h"
#include "context_f.h"
#include "memory_f.h"
#include "inites_f.h"
#include "buildlines_f.h"

#ifdef __STDC__
void                DrawPage (FILE * fout)
#else  /* __STDC__ */
void                DrawPage (fout)
FILE               *fout;

#endif /* __STDC__ */
{
}

/* ---------------------------------------------------------------------- */
/* |    FontOrig de'cale une position de chaine pour DrawString.        | */
/* |            (x,y) haut gauche est deplace sur le point de ref du    | */
/* |            1er char.                                               | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                FontOrig (ptrfont font, char firstchar, int *pX, int *pY)
#else  /* __STDC__ */
void                FontOrig (font, firstchar, pX, pY)
ptrfont             font;
char                firstchar;
int                *pX;
int                *pY;

#endif /* __STDC__ */
{
   if (!font)
      return;
#ifndef NEW_WILLOWS
   *pY += ((XFontStruct *) font)->ascent;
#endif /* NEW_WILLOWS */
}


/* ---------------------------------------------------------------------- */
/* |    ChargeCouleur charge la bonne colueur de trace'.                | */
/* |            Le parame`tre readonly indique s'il s'agit d'une boi^te | */
/* |            en lecture seule (1) ou non (0).                        | */
/* |            Le parame`tre RO indique s'il s'agit d'une boi^te en    | */
/* |            Read Only (1) ou non (0).                               | */
/* |            Le parame`tre active indique s'il s'agit d'une boi^te   | */
/* |            active (1) ou non (0).                                  | */
/* |            Le parame`tre fg indique la couleur du trace'.          | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         ChargeCouleur (int disp, int RO, int active, int fg)
#else  /* __STDC__ */
static void         ChargeCouleur (disp, RO, active, fg)
int                 disp;
int                 RO;
int                 active;
int                 fg;

#endif /* __STDC__ */
{
#ifdef NEW_WILLOWS
   (TtLineGC)->capabilities |= THOT_GC_FOREGROUND;
   (TtLineGC)->foreground = Pix_Color[fg];
#else  /* NEW_WILLOWS */
   if (active && ShowReference ())
     {
	if (TtWDepth == 1)
	   /* On modifie la trame des caracteres */
	   XSetFillStyle (TtDisplay, TtLineGC, FillTiled);
	else
	   /* Couleur des boites actives */
	   XSetForeground (TtDisplay, TtLineGC, Box_Color);
     }
   else if (RO && ShowReadOnly () && ColorPixel (fg) == cblack.pixel)
      /* Couleur du ReadOnly */
      XSetForeground (TtDisplay, TtLineGC, RO_Color);
   else
      /* Couleur de la boite */
      XSetForeground (TtDisplay, TtLineGC, ColorPixel (fg));
#endif /* NEW_WILLOWS */
}


/* ---------------------------------------------------------------------- */
/* |    preparerTrace positionne les attributs du ThotGC de trace'.             | */
/* |            Le parame`tre RO indique s'il s'agit d'une boi^te en    | */
/* |            Read Only (1) ou non (0).                               | */
/* |            Le parame`tre active indique s'il s'agit d'une boi^te   | */
/* |            active (1) ou non (0).                                  | */
/* |            Le parame`tre fg indique la couleur du trace'.          | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         preparerTrace (int disp, int style, int epais, int RO, int active, int fg)
#else  /* __STDC__ */
static void         preparerTrace (disp, style, epais, RO, active, fg)
int                 disp;
int                 style;
int                 epais;
int                 RO;
int                 active;
int                 fg;

#endif /* __STDC__ */
{
   char                dash[2];

#ifndef NEW_WILLOWS
   if (style == 0)
      XSetLineAttributes (TtDisplay, TtLineGC, epais, LineSolid, CapButt, JoinMiter);
   else
     {
	dash[0] = (char) (style * 4);
	dash[1] = (char) 4;
	XSetDashes (TtDisplay, TtLineGC, 0, dash, 2);
	XSetLineAttributes (TtDisplay, TtLineGC, epais, LineOnOffDash, CapButt, JoinMiter);
     }
#endif /* NEW_WILLOWS */
   /* Charge la bonne couleur */
   ChargeCouleur (disp, RO, active, fg);
}


/* ---------------------------------------------------------------------- */
/* |    finirTrace positionne les attributs du ThotGC de trace'.                | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         finirTrace (int disp, int RO, int active)
#else  /* __STDC__ */
static void         finirTrace (disp, RO, active)
int                 disp;
int                 RO;
int                 active;

#endif /* __STDC__ */
{
#ifndef NEW_WILLOWS
   if (TtWDepth == 1 && (active || RO))
      XSetFillStyle (TtDisplay, TtLineGC, FillSolid);
#endif /* NEW_WILLOWS */
}


/* ---------------------------------------------------------------------- */
/* |    Tracer trace un trait de x1,y1 a x2,y2 d'une e'paisseur epais   | */
/* |            dans la fenetree^tre frame.                                     | */
/* |            Le parame`tre style indique si le trait est continu (0) | */
/* |            ou pointille' simple (1) ou double (>1).                | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         Tracer (int frame, int x1, int y1, int x2, int y2)
#else  /* __STDC__ */
static void         Tracer (frame, x1, y1, x2, y2)
int                 frame;
int                 x1;
int                 y1;
int                 x2;
int                 y2;

#endif /* __STDC__ */
{
   x1 += FrameTable[frame].FrLeftMargin;
   y1 += FrameTable[frame].FrTopMargin;
   x2 += FrameTable[frame].FrLeftMargin;
   y2 += FrameTable[frame].FrTopMargin;
#ifdef NEW_WILLOWS
   WIN_GetDeviceContext (frame);
   WinLoadGC (WIN_curHdc, TtLineGC);
   MoveToEx (WIN_curHdc, x1, y1, NULL);
   LineTo (WIN_curHdc, x2, y2);
#else  /* NEW_WILLOWS */
   XDrawLine (TtDisplay, FrRef[frame], TtLineGC, x1, y1, x2, y2);
#endif /* NEW_WILLOWS */
}


/* ---------------------------------------------------------------------- */
/* |     SpaceToCar remplace dans la chaine de caracteres passee en     | */
/* |            parametre les caracteres espaces par les caracteres     | */
/* |            de visualisation des espaces correspondants.            | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         SpaceToCar (unsigned char *text)
#else  /* __STDC__ */
static void         SpaceToCar (text)
unsigned char      *text;

#endif /* __STDC__ */
{
   int                 i;

   if (text == NULL)
      return;

   i = 0;
   while (text[i] != 0)
     {
	switch (text[i])
	      {
		 case SAUT_DE_LIGNE:
		    text[i] = (unsigned char) SeeCtrlRC;
		    break;
		 case FINE:
		    text[i] = (unsigned char) SeeFine;
		    break;
		 case DEMI_CADRATIN:
		    text[i] = (unsigned char) SeeDemiCadratin;
		    break;
		 case BLANC_DUR:
		    text[i] = (unsigned char) SeeBlancDur;
		    break;
		 case BLANC:
		    text[i] = (unsigned char) SeeBlanc;
		    break;
	      }
	i++;
     }
}


/* ---------------------------------------------------------------------- */
/* |    DrawChar affiche le caracte`re car a` la position x,y de la        | */
/* |            framee^tre frame en utilisant la police de caracteres font.     | */
/* |            Le parame`tre RO indique s'il s'agit d'une boi^te en    | */
/* |            Read Only (1) ou non (0).                               | */
/* |            Le parame`tre active indique s'il s'agit d'une boi^te   | */
/* |            active (1) ou non (0).                                  | */
/* |            Le parame`tre fg indique la couleur du trace'.          | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                DrawChar (char car, int frame, int x, int y, ptrfont font, int RO, int active, int fg)
#else  /* __STDC__ */
void                DrawChar (car, frame, x, y, font, RO, active, fg)
char                car;
int                 frame;
int                 x;
int                 y;
ptrfont             font;
int                 RO;
int                 active;
int                 fg;

#endif /* __STDC__ */
{
   ThotWindow          w;

#ifdef NEW_WILLOWS
   char                str[2] =
   {car, 0};

#endif /* NEW_WILLOWS */

   w = FrRef[frame];
   if (w == None)
      return;

   ChargeCouleur (0, RO, active, fg);

#ifdef NEW_WILLOWS
   WIN_GetDeviceContext (frame);
   WinLoadGC (WIN_curHdc, TtLineGC);
   WinLoadFont (WIN_curHdc, font);
   TextOut (WIN_curHdc, x + FrameTable[frame].FrLeftMargin, y + FrameTable[frame].FrTopMargin, str, 1);
#else  /* NEW_WILLOWS */
   XSetFont (TtDisplay, TtLineGC, ((XFontStruct *) font)->fid);
   XDrawString (TtDisplay, w, TtLineGC, x + FrameTable[frame].FrLeftMargin, y + FrameTable[frame].FrTopMargin + FontBase (font), &car, 1);
#endif /* NEW_WILLOWS */

   finirTrace (0, RO, active);
}

/* ---------------------------------------------------------------------- */
/* |    DrawString affiche la chai^ne de caracte`res de longueur lg qui   | */
/* |            de'bute par buff[i] a` la position x,y dans la fenetree^tre     | */
/* |            frame en utilisant la police de caracte`res font.               | */
/* |            Le parame`tre lgboite donne la largeur de la boi^te     | */
/* |            en fin de traitement sinon 0. Ce parame`tre est         | */
/* |            utilise' uniquement par le formateur.                   | */
/* |            Le parame`tre bl indique qu'un blanc pre'ce`de la       | */
/* |            chai^ne transmise.                                      | */
/* |            Le parame`tre hyphen indique qu'un caracte`re           | */
/* |            d'hyphenation doit e^tre ajoute' en fin de chai^ne.     | */
/* |            Le parame`tre debutbloc vaut 1 quand le texte se trouve | */
/* |            en de'but de paragraphe pour interdire la justification | */
/* |            des premiers blancs au moment de l'impression.          | */
/* |            Le parame`tre RO indique s'il s'agit d'une boi^te en    | */
/* |            Read Only (1) ou non (0).                               | */
/* |            Le parame`tre active indique s'il s'agit d'une boi^te   | */
/* |            active (1) ou non (0).                                  | */
/* |            Le parame`tre fg indique la couleur du trace'.          | */
/* |            Retourne la largeur de la chaine affichee.              | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
int                 DrawString (char *buff, int i, int lg, int frame, int x, int y, ptrfont font, int lgboite, int bl, int hyphen, int debutbloc, int RO, int active, int fg)
#else  /* __STDC__ */
int                 DrawString (buff, i, lg, frame, x, y, font, lgboite, bl, hyphen, debutbloc, RO, active, fg)
char               *buff;
int                 i;
int                 lg;
int                 frame;
int                 x;
int                 y;
ptrfont             font;
int                 lgboite;
int                 bl;
int                 hyphen;
int                 debutbloc;
int                 RO;
int                 active;
int                 fg;

#endif /* __STDC__ */
{
   ThotWindow          w;
   char               *ptcar;
   int                 large;
   register int        j;

#ifdef NEW_WILLOWS
   SIZE                size;

#endif

   w = FrRef[frame];
   if (lg > 0 && w != None)
     {
	ptcar = &buff[i - 1];
#ifdef NEW_WILLOWS
	WIN_GetDeviceContext (frame);
	WinLoadFont (WIN_curHdc, font);
	/* GetTextExtentPoint32(WIN_curHdc, ptcar, lg, &size); */
	GetTextExtentPoint (WIN_curHdc, ptcar, lg, &size);
	large = size.cx;
#else  /* NEW_WILLOWS */
	XSetFont (TtDisplay, TtLineGC, ((XFontStruct *) font)->fid);

	/* On calcule la largeur de la chaine de caractere ecrite */
	large = 0;
	j = 0;
	while (j < lg)
	   large += CarWidth (ptcar[j++], font);
#endif /* !NEW_WILLOWS */

	ChargeCouleur (0, RO, active, fg);

	if (!ShowSpace)
	  {
	     /* Il faut visualiser les caracteres espaces */
	     ptcar = TtaGetMemory (lg + 1);
	     strncpy (ptcar, &buff[i - 1], lg);
	     ptcar[lg] = '\0';
	     SpaceToCar (ptcar);	/* remplace les carateres espaces */
#ifdef NEW_WILLOWS
	     WinLoadGC (WIN_curHdc, TtLineGC);
	     TextOut (WIN_curHdc, x + FrameTable[frame].FrLeftMargin, y + FrameTable[frame].FrTopMargin, ptcar, lg);
#else  /* NEW_WILLOWS */
	     XDrawString (TtDisplay, w, TtLineGC, x + FrameTable[frame].FrLeftMargin, y + FrameTable[frame].FrTopMargin + FontBase (font), ptcar, lg);
#endif /* NEW_WILLOWS */
	     TtaFreeMemory (ptcar);
	  }
	else
	  {
#ifdef NEW_WILLOWS
	     WinLoadGC (WIN_curHdc, TtLineGC);
	     TextOut (WIN_curHdc, x + FrameTable[frame].FrLeftMargin, y + FrameTable[frame].FrTopMargin, ptcar, lg);
#else  /* NEW_WILLOWS */
	     XDrawString (TtDisplay, w, TtLineGC, x + FrameTable[frame].FrLeftMargin, y + FrameTable[frame].FrTopMargin + FontBase (font), ptcar, lg);
#endif /* NEW_WILLOWS */
	  }
	if (hyphen)
	  {
	     /* trace le caractere hyphen */
#ifdef NEW_WILLOWS
	     TextOut (WIN_curHdc, x + large + FrameTable[frame].FrLeftMargin, y + FrameTable[frame].FrTopMargin, "\255", 1);
#else  /* NEW_WILLOWS */
	     XDrawString (TtDisplay, w, TtLineGC, x + large + FrameTable[frame].FrLeftMargin,
	     y + FrameTable[frame].FrTopMargin + FontBase (font), "\255", 1);
#endif /* NEW_WILLOWS */
	  }
	finirTrace (0, RO, active);

	return (large);
     }
   else
      return (0);
}

/* ---------------------------------------------------------------------- */
/* |    DisplayUnderline affiche un souligne' sous la chaine de caracteres de     | */
/* |            longueur lg utilisant la police de caracteres font a`   | */
/* |            la position x, y dans la fenetre frame.                 | */
/* |            Le parame`tre RO indique s'il s'agit d'une boi^te en    | */
/* |            Read Only (1) ou non (0).                               | */
/* |            Le parame`tre active indique s'il s'agit d'une boi^te   | */
/* |            active (1) ou non (0).                                  | */
/* |            Le parame`tre fg indique la couleur du trace'.          | */
/* |            epais donne l'e'paisseur du trait: mince (0), epais (1) | */
/* |            Le parame`tre type est le type de souligne':            | */
/* |            - 0 = sans                                              | */
/* |            - 1 = souligne'                                         | */
/* |            - 2 = surligne'                                         | */
/* |            - 3 = biffe'                                            | */
/* ---------------------------------------------------------------------- */
/* -                             (x,y)                                    */
/* -          __________________+______________________________\_/__      */
/* -         /|\    I    I          /|\       /|\   /|\         |         */
/* -          |     I\  /I           |         |     |       ___|haut     */
/* -          |  ___I_\/_I_______    |ascent   |     |milieu   / \        */
/* -   height |     I    I  I  \     |         |bas  |                    */
/* -          |     I    I  I  |     |         |  __\|/                   */
/* -          |  ___I____I__I__/____\|/        |                          */
/* -          |             I             ____\|/                         */
/* -          |             I                                             */
/* -         \|/____________I_                                            */
/* -                                                                      */
/* - le + indique l'origine des distances (x,y)                           */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                DisplayUnderline (int frame, int x, int y, ptrfont font, int type, int epais, int lg, int RO, int active, int fg)
#else  /* __STDC__ */
void                DisplayUnderline (frame, x, y, font, type, epais, lg, RO, active, fg)
int                 frame;
int                 x;
int                 y;
ptrfont             font;
int                 type;
int                 epais;
int                 lg;
int                 RO;
int                 active;
int                 fg;

#endif /* __STDC__ */
{
   ThotWindow          w;
   int                 height;	/* hauteur de la fonte   */
   int                 ascent;	/* ascent  de la fonte   */
   int                 bas;	/* position du souligne' */
   int                 milieu;	/* position du biffe'    */
   int                 haut;	/* position du surligne' */
   int                 epaisseur;	/* epaisseur du trait    */
   int                 decal;	/* decalage du trait     */

   if (lg > 0)
     {
	w = FrRef[frame];
	if (w == None)
	   return;
	height = FontHeight (font);
	ascent = FontAscent (font);
	epaisseur = ((height / 20) + 1) * (epais + 1);	/* epaisseur proportionnelle a hauteur */
	decal = (2 - epais) * epaisseur;
	haut = y + decal;
	bas = y + ascent + decal;
	milieu = y + height / 2 + decal;

	/* pour avoir un souligne' independant de la fonte, */
	/* il faut mettre ici les lignes suivantes :        */
	/* (Valeur en dur pour Thot Marine) */
	/*         epaisseur = 1; */
	/*         haut = y + 2 * epaisseur; */
	/*         bas = y + ascent + 3;  */
	/* a mettre en commentaire ou pas suivant ce que l'on veut */

	preparerTrace (0, 0, epaisseur, RO, active, fg);
	switch (type)
	      {
		 case 1:
		    /* souligne */
		    Tracer (frame, x - lg, bas, x, bas);
		    break;

		 case 2:
		    /* surligne */
		    Tracer (frame, x - lg, haut, x, haut);
		    break;

		 case 3:
		    /* biffer */
		    Tracer (frame, x - lg, milieu, x, milieu);
		    break;

		 default:
		    /* sans souligne */
		    break;
	      }
	finirTrace (0, RO, active);
     }
}


/* ---------------------------------------------------------------------- */
/* |    DrawPoints trace un ligne de pointille's sur la longueur donne'e. | */
/* |            Le parame`tre RO indique s'il s'agit d'une boi^te en    | */
/* |            Read Only (1) ou non (0).                               | */
/* |            Le parame`tre active indique s'il s'agit d'une boi^te   | */
/* |            active (1) ou non (0).                                  | */
/* |            Le parame`tre fg indique la couleur du trace'.          | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                DrawPoints (int frame, int x, int y, int lgboite, int RO, int active, int fg)
#else  /* __STDC__ */
void                DrawPoints (frame, x, y, lgboite, RO, active, fg)
int                 frame;
int                 x;
int                 y;
int                 lgboite;
int                 RO;
int                 active;
int                 fg;

#endif /* __STDC__ */
{
   ThotWindow          w;
   ptrfont             font;
   int                 xcour, large, nb;
   char               *ptcar;

   font = ThotLoadFont ('L', 't', 0, 6, UnPoint, frame);
   if (lgboite > 0)
     {
	w = FrRef[frame];
	ptcar = " .";

	/* On calcule la largeur de la chaine de caracteres ecrite */
	large = CarWidth (' ', font) + CarWidth ('.', font);

	/* On calcule un nombre de fois que l'on peut ecrire la chaine */
	nb = lgboite / large;
	xcour = x + FrameTable[frame].FrLeftMargin + (lgboite % large);
	y += FrameTable[frame].FrTopMargin - FontBase (font);
#ifndef NEW_WILLOWS
	XSetFont (TtDisplay, TtLineGC, ((XFontStruct *) font)->fid);
#endif /* NEW_WILLOWS */
	ChargeCouleur (0, RO, active, fg);

	/* Trace des points */
	FontOrig (font, *ptcar, &x, &y);
	while (nb > 0)
	  {
#ifndef NEW_WILLOWS
	     XDrawString (TtDisplay, w, TtLineGC, xcour, y, ptcar, 2);
#endif /* NEW_WILLOWS */
	     xcour += large;
	     nb--;
	  }
	finirTrace (0, RO, active);
     }
}

/* ---------------------------------------------------------------------- */
/* |    DrawRadical affiche un signe Radical.                             | */
/* |            Le parame`tre RO indique s'il s'agit d'une boi^te en    | */
/* |            Read Only (1) ou non (0).                               | */
/* |            Le parame`tre active indique s'il s'agit d'une boi^te   | */
/* |            active (1) ou non (0).                                  | */
/* |            Le parame`tre fg indique la couleur du trace'.          | */
/* ---------------------------------------------------------------------- */


#ifdef __STDC__
void                DrawRadical (int frame, int epais, int x, int y, int l, int h, ptrfont font, int RO, int active, int fg)

#else  /* __STDC__ */
void                DrawRadical (frame, epais, x, y, l, h, font, RO, active, fg)
int                 frame;
int                 epais;
int                 x;
int                 y;
int                 l;
int                 h;
ptrfont             font;
int                 RO;
int                 active;
int                 fg;

#endif /* __STDC__ */

{
   int                 xm, fh;

   fh = FontHeight (font);
   xm = x + (fh / 2);
   preparerTrace (0, 0, 0, RO, active, fg);
   /* Partie descendante */
   Tracer (frame, x, y + (2 * (h / 3)), xm - (epais / 2), y + h);

   preparerTrace (0, 0, epais, RO, active, fg);
   /* Partie montante */
   Tracer (frame, x + (fh / 2), y + h, xm, y);
   /* Partie superieure */
   Tracer (frame, xm, y, x + l, y);
   finirTrace (0, RO, active);
}


/* ---------------------------------------------------------------------- */
/* |    DrawIntegral trace une integrale :                               | */
/* |            - simple si type = 0                                    | */
/* |            - curviligne si type = 1                                | */
/* |            - double si type = 2.                                   | */
/* |            Le parame`tre RO indique s'il s'agit d'une boi^te en    | */
/* |            Read Only (1) ou non (0).                               | */
/* |            Le parame`tre active indique s'il s'agit d'une boi^te   | */
/* |            active (1) ou non (0).                                  | */
/* |            Le parame`tre fg indique la couleur du trace'.          | */
/* ---------------------------------------------------------------------- */


#ifdef __STDC__
void                DrawIntegral (int frame, int epais, int x, int y, int l, int h, int type, ptrfont font, int RO, int active, int fg)

#else  /* __STDC__ */
void                DrawIntegral (frame, epais, x, y, l, h, type, font, RO, active, fg)
int                 frame;
int                 epais;
int                 x;
int                 y;
int                 l;
int                 h;
int                 type;
ptrfont             font;
int                 RO;
int                 active;
int                 fg;

#endif /* __STDC__ */

{
   int                 xm, yf, yend, exnum, delta;

   exnum = 0;

   xm = x + ((l - CarWidth ('\364', font)) / 2);
   yf = y - FontAscent (font) + CarAscent ('\363', font);
   DrawChar ('\363', frame, xm, yf, font, RO, active, fg);
   yend = y + h - CarHeight ('\365', font) - FontAscent (font) + CarAscent ('\365', font) - 1;
   DrawChar ('\365', frame, xm, yend, font, RO, active, fg);

   yf += CarHeight ('\363', font);
   delta = yend - yf;
   if (delta >= 0)
     {
	for (yf += CarAscent ('\364', font) - FontAscent (font),
	     yend -= CarHeight ('\364', font) - 1;
	     yf < yend;
	     yf += CarHeight ('\364', font), exnum++)
	   DrawChar ('\364', frame, xm, yf, font, RO, active, fg);
	if (exnum)
	   DrawChar ('\364', frame, xm, yend, font, RO, active, fg);
	else			/* on centre l'extension */
	   DrawChar ('\364', frame, xm, yf + ((delta - CarHeight ('\364', font)) / 2), font, RO, active, fg);
     }

   if (type == 2)		/* integrale double */
      DrawIntegral (frame, epais, x + (CarWidth ('\364', font) / 2),
		   y, l, h, -1, font, RO, active, fg);
   /*    ^^  hack !! */

   else if (type == 1)		/* curviligne */
      DrawChar ('o', frame, x + ((l - CarWidth ('o', font)) / 2),
	     y + (h - CarHeight ('o', font)) / 2 - FontAscent (font) + CarAscent ('o', font),
	     font, RO, active, fg);
}

/* ---------------------------------------------------------------------- */
/* |    AfMonoSymb trace un symbole constitue' d'un caractere.          | */
/* |            Le parame`tre RO indique s'il s'agit d'une boi^te en    | */
/* |            Read Only (1) ou non (0).                               | */
/* |            Le parame`tre active indique s'il s'agit d'une boi^te   | */
/* |            active (1) ou non (0).                                  | */
/* |            Le parame`tre fg indique la couleur du trace'.          | */
/* ---------------------------------------------------------------------- */


#ifdef __STDC__
static void         AfMonoSymb (char symb, int frame, int x, int y, int l, int h, int RO, int active, ptrfont font, int fg)

#else  /* __STDC__ */
static void         AfMonoSymb (symb, frame, x, y, l, h, RO, active, font, fg)
char                symb;
int                 frame;
int                 x;
int                 y;
int                 l;
int                 h;
int                 RO;
int                 active;
ptrfont             font;
int                 fg;

#endif /* __STDC__ */

{
   int                 xm, yf;

   xm = x + ((l - CarWidth (symb, font)) / 2);
   yf = y + ((h - CarHeight (symb, font)) / 2) - FontAscent (font) + CarAscent (symb, font);
   /*   ^^^^^^^^^ pour compenser le FontOrig dans Afcar */

   DrawChar (symb, frame, xm, yf, font, RO, active, fg);
}

/* ---------------------------------------------------------------------- */
/* |    DrawSigma trace un symbole Sigma.                                 | */
/* |            Le parame`tre active indique s'il s'agit d'une boi^te   | */
/* |            active (1) ou non (0).                                  | */
/* |            Le parame`tre fg indique la couleur du trace'.          | */
/* ---------------------------------------------------------------------- */


#ifdef __STDC__
void                DrawSigma (int frame, int x, int y, int l, int h, ptrfont font, int RO, int active, int fg)

#else  /* __STDC__ */
void                DrawSigma (frame, x, y, l, h, font, RO, active, fg)
int                 frame;
int                 x;
int                 y;
int                 l;
int                 h;
ptrfont             font;
int                 RO;
int                 active;
int                 fg;

#endif /* __STDC__ */

{
   int                 xm, ym, fh;

   fh = FontHeight (font);
   if (h < fh * 2 && l <= CarWidth ('\345', font))
     {
	/* Un seul caractere */
	AfMonoSymb ('\345', frame, x, y, l, h, RO, active, font, fg);
     }
   else
     {
	xm = x + (l / 3);
	ym = y + (h / 2) - 1;
	preparerTrace (0, 0, 0, RO, active, fg);
	/* Partie centrale */
	Tracer (frame, x, y + 1, xm, ym);
	Tracer (frame, x, y + h - 2, xm, ym);

	preparerTrace (0, 0, 2, RO, active, fg);
	/* Parties extremes */
	Tracer (frame, x, y, x + l, y);
	Tracer (frame, x, y + h - 2, x + l, y + h - 2);
	finirTrace (0, RO, active);
     }
}

/* ---------------------------------------------------------------------- */
/* |    DrawPi trace un symbole PI.                                       | */
/* |            Le parame`tre RO indique s'il s'agit d'une boi^te en    | */
/* |            Read Only (1) ou non (0).                               | */
/* |            Le parame`tre active indique s'il s'agit d'une boi^te   | */
/* |            active (1) ou non (0).                                  | */
/* |            Le parame`tre fg indique la couleur du trace'.          | */
/* ---------------------------------------------------------------------- */



#ifdef __STDC__
void                DrawPi (int frame, int x, int y, int l, int h, ptrfont font, int RO, int active, int fg)

#else  /* __STDC__ */
void                DrawPi (frame, x, y, l, h, font, RO, active, fg)
int                 frame;
int                 x;
int                 y;
int                 l;
int                 h;
ptrfont             font;
int                 RO;
int                 active;
int                 fg;

#endif /* __STDC__ */

{
   int                 fh;

   fh = FontHeight (font);
   if (h < fh * 2 && l <= CarWidth ('\325', font))
     {
	/* Un seul caractere */
	AfMonoSymb ('\325', frame, x, y, l, h, RO, active, font, fg);
     }
   else
     {
	preparerTrace (0, 0, 0, RO, active, fg);
	/* Partie descendnte */
	Tracer (frame, x + 2, y + 1, x + 2, y + h);
	Tracer (frame, x + l - 3, y + 1, x + l - 3, y + h);

	preparerTrace (0, 0, 2, RO, active, fg);
	/* Partie haute */
	Tracer (frame, x + 1, y + 1, x + l, y);
	finirTrace (0, RO, active);
     }
}

/* ---------------------------------------------------------------------- */
/* |    DrawIntersection trace un symbole Intersection.                   | */
/* |            Le parame`tre RO indique s'il s'agit d'une boi^te en    | */
/* |            Read Only (1) ou non (0).                               | */
/* |            Le parame`tre active indique s'il s'agit d'une boi^te   | */
/* |            active (1) ou non (0).                                  | */
/* |            Le parame`tre fg indique la couleur du trace'.          | */
/* ---------------------------------------------------------------------- */


#ifdef __STDC__
void                DrawIntersection (int frame, int x, int y, int l, int h, ptrfont font, int RO, int active, int fg)

#else  /* __STDC__ */
void                DrawIntersection (frame, x, y, l, h, font, RO, active, fg)
int                 frame;
int                 x;
int                 y;
int                 l;
int                 h;
ptrfont             font;
int                 RO;
int                 active;
int                 fg;

#endif /* __STDC__ */

{
   int                 arc, fh;

   fh = FontHeight (font);
   if (h < fh * 2 && l <= CarWidth ('\307', font))
     {
	/* Un seul caractere */
	AfMonoSymb ('\307', frame, x, y, l, h, RO, active, font, fg);
     }
   else
     {
	/* rayon des arcs a 6mm */
	arc = h / 4;
	preparerTrace (0, 0, 2, RO, active, fg);
	/* Partie descendante */
	Tracer (frame, x + 1, y + arc, x + 1, y + h);
	Tracer (frame, x + l - 2, y + arc, x + l - 2, y + h);

	/* Partie haute */
#ifndef NEW_WILLOWS
	XDrawArc (TtDisplay, FrRef[frame], TtLineGC, x + 1, y + 1, l - 3, arc * 2, 0 * 64, 180 * 64);
#endif /* NEW_WILLOWS */
	finirTrace (0, RO, active);
     }
}

/* ---------------------------------------------------------------------- */
/* |    DrawUnion trace un symbole Union.                                 | */
/* |            Le parame`tre RO indique s'il s'agit d'une boi^te en    | */
/* |            Read Only (1) ou non (0).                               | */
/* |            Le parame`tre active indique s'il s'agit d'une boi^te   | */
/* |            active (1) ou non (0).                                  | */
/* |            Le parame`tre fg indique la couleur du trace'.          | */
/* ---------------------------------------------------------------------- */


#ifdef __STDC__
void                DrawUnion (int frame, int x, int y, int l, int h, ptrfont font, int RO, int active, int fg)

#else  /* __STDC__ */
void                DrawUnion (frame, x, y, l, h, font, RO, active, fg)
int                 frame;
int                 x;
int                 y;
int                 l;
int                 h;
ptrfont             font;
int                 RO;
int                 active;
int                 fg;

#endif /* __STDC__ */

{
   int                 arc, fh;

   fh = FontHeight (font);
   if (h < fh * 2 && l <= CarWidth ('\310', font))
     {
	/* Un seul caractere */
	AfMonoSymb ('\310', frame, x, y, l, h, RO, active, font, fg);
     }
   else
     {
	/* rayon des arcs a 3mm */
	arc = h / 4;
	preparerTrace (0, 0, 2, RO, active, fg);
	/* Partie descendante */
	Tracer (frame, x + 1, y, x + 1, y + h - arc);
	Tracer (frame, x + l - 2, y, x + l - 2, y + h - arc);

	/* Partie basse */
#ifndef NEW_WILLOWS
	XDrawArc (TtDisplay, FrRef[frame], TtLineGC, x + 1, y + h - arc * 2 - 2, l - 3, arc * 2, -0 * 64, -180 * 64);
#endif /* NEW_WILLOWS */
	finirTrace (0, RO, active);
     }
}

/* ---------------------------------------------------------------------- */
/* |    TraceFleche trace l'extremite de la fleche.                     | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         TraceFleche (int frame, int x1, int y1, int x2, int y2, int epais, int RO, int active, int fg)
#else  /* __STDC__ */
static void         TraceFleche (frame, x1, y1, x2, y2, epais, RO, active, fg)
int                 frame;
int                 x1, y1, x2, y2;
int                 epais;
int                 RO;
int                 active;
int                 fg;

#endif /* __STDC__ */
{
#ifndef NEW_WILLOWS
   float               x, y, xb, yb, dx, dy, l, sina, cosa;
   int                 xc, yc, xd, yd;
   float               large, haut;
   ThotPoint           point[3];
   Pixmap              modele;

   large = 5 + epais;
   haut = 10;
   dx = (float) (x2 - x1);
   dy = (float) (y1 - y2);
   l = sqrt ((double) (dx * dx + dy * dy));
   if (l == 0)
      return;
   sina = dy / l;
   cosa = dx / l;
   xb = x2 * cosa - y2 * sina;
   yb = x2 * sina + y2 * cosa;
   x = xb - haut;
   y = yb - large / 2;
   xc = FloatToInt (x * cosa + y * sina + .5);
   yc = FloatToInt (-x * sina + y * cosa + .5);
   y = yb + large / 2;
   xd = FloatToInt (x * cosa + y * sina + .5);
   yd = FloatToInt (-x * sina + y * cosa + .5);

   /* trace */
   point[0].x = x2;
   point[0].y = y2;
   point[1].x = xc;
   point[1].y = yc;
   point[2].x = xd;
   point[2].y = yd;
   modele = CreatePattern (0, RO, active, fg, fg, 1);
   if (modele != 0)
     {
	XSetTile (TtDisplay, TtGreyGC, modele);
	XFillPolygon (TtDisplay, FrRef[frame], TtGreyGC, point, 3, Convex, CoordModeOrigin);
	XFreePixmap (TtDisplay, modele);
     }
#endif /* NEW_WILLOWS */
}

/* ---------------------------------------------------------------------- */
/* |    DrawArrow trace une fle^che oriente'e en fonction de l'angle     | */
/* |            donne' :0 (fle^che vers la droite), 45, 90, 135, 180,   | */
/* |            225, 270 ou 315.                                        | */
/* |            Le parame`tre RO indique s'il s'agit d'une boi^te en    | */
/* |            Read Only (1) ou non (0).                               | */
/* |            Le parame`tre active indique s'il s'agit d'une boi^te   | */
/* |            active (1) ou non (0).                                  | */
/* |            Le parame`tre fg indique la couleur du trace'.          | */
/* ---------------------------------------------------------------------- */


#ifdef __STDC__
void                DrawArrow (int frame, int epais, int style, int x, int y, int l, int h, int orientation, int RO, int active, int fg)

#else  /* __STDC__ */
void                DrawArrow (frame, epais, style, x, y, l, h, orientation, RO, active, fg)
int                 frame;
int                 epais;
int                 style;
int                 x;
int                 y;
int                 l;
int                 h;
int                 orientation;
int                 RO;
int                 active;
int                 fg;

#endif /* __STDC__ */

{
   int                 xm, ym, xf, yf;

   if (epais <= 0)
      return;
   xm = x + ((l - epais) / 2);
   xf = x + l - 1;
   ym = y + ((h - epais) / 2);
   yf = y + h - 1;

   preparerTrace (0, style, epais, RO, active, fg);
   if (orientation == 0)
     {
	/* fleche vers la droite */
	Tracer (frame, x, ym, xf, ym);
	TraceFleche (frame, x, ym, xf, ym, epais, RO, active, fg);
     }
   else if (orientation == 45)
     {
	Tracer (frame, x, yf, xf - epais + 1, y);
	TraceFleche (frame, x, yf, xf - epais + 1, y, epais, RO, active, fg);
     }
   else if (orientation == 90)
     {
	/* fleche vers le haut */
	Tracer (frame, xm, y, xm, yf);	/* Trait central */
	TraceFleche (frame, xm, yf, xm, y, epais, RO, active, fg);
     }
   else if (orientation == 135)
     {
	Tracer (frame, x, y, xf - epais + 1, yf);
	TraceFleche (frame, xf - epais + 1, yf, x, y, epais, RO, active, fg);
     }
   else if (orientation == 180)
     {
	/* Trace une fleche vers la gauche */
	Tracer (frame, x, ym, xf, ym);
	TraceFleche (frame, xf, ym, x, ym, epais, RO, active, fg);
     }
   else if (orientation == 225)
     {
	Tracer (frame, x, yf, xf - epais + 1, y);
	TraceFleche (frame, xf - epais + 1, y, x, yf, epais, RO, active, fg);
     }
   else if (orientation == 270)
     {
	/* Trace une fleche vers le bas */
	Tracer (frame, xm, y, xm, yf);	/* Trait central */
	TraceFleche (frame, xm, y, xm, yf, epais, RO, active, fg);
     }
   else if (orientation == 315)
     {
	Tracer (frame, x, y, xf - epais + 1, yf);
	TraceFleche (frame, x, y, xf - epais + 1, yf, epais, RO, active, fg);
     }
   finirTrace (0, RO, active);
}

/* ---------------------------------------------------------------------- */
/* |    DrawBracket trace un symbole crochet ouvrant ou fermant.          | */
/* |            Le parame`tre RO indique s'il s'agit d'une boi^te en    | */
/* |            Read Only (1) ou non (0).                               | */
/* |            Le parame`tre active indique s'il s'agit d'une boi^te   | */
/* |            active (1) ou non (0).                                  | */
/* |            Le parame`tre fg indique la couleur du trace'.          | */
/* ---------------------------------------------------------------------- */


#ifdef __STDC__
void                DrawBracket (int frame, int epais, int x, int y, int l, int h, int sens, ptrfont font, int RO, int active, int fg)

#else  /* __STDC__ */
void                DrawBracket (frame, epais, x, y, l, h, sens, font, RO, active, fg)
int                 frame;
int                 epais;
int                 x;
int                 y;
int                 l;
int                 h;
int                 sens;
ptrfont             font;
int                 RO;
int                 active;
int                 fg;

#endif /* __STDC__ */

{
   int                 xm, yf, yend;

   if (FontHeight (font) >= h)
     {
	/* Avec un seul caractere */
	if (sens == 0)
	  {
	     /* Trace un crochet ouvrant */
	     xm = x + ((l - CarWidth ('[', font)) / 2);
	     yf = y + ((h - CarHeight ('[', font)) / 2) -
		FontAscent (font) + CarAscent ('[', font);
	     DrawChar ('[', frame, xm, yf, font, RO, active, fg);
	  }
	else
	  {
	     /* Trace un crochet fermant */
	     xm = x + ((l - CarWidth (']', font)) / 2);
	     yf = y + ((h - CarHeight (']', font)) / 2) -
		FontAscent (font) + CarAscent (']', font);
	     DrawChar (']', frame, xm, yf, font, RO, active, fg);
	  }
     }
   else
     {
	/* Avec plusieurs caracteres */
	if (sens == 0)
	  {
	     /* Trace un crochet ouvrant */
	     xm = x + ((l - CarWidth ('\351', font)) / 2);
	     yf = y - FontAscent (font) + CarAscent ('\351', font);
	     DrawChar ('\351', frame, xm, yf, font, RO, active, fg);
	     yend = y + h - CarHeight ('\353', font) -
		FontAscent (font) + CarAscent ('\353', font);
	     DrawChar ('\353', frame, xm, yend, font, RO, active, fg);
	     for (yf = yf + CarHeight ('\351', font) -
		  FontAscent (font) + CarAscent ('\352', font);
		  yf < yend;
		  yf += CarHeight ('\352', font))
		DrawChar ('\352', frame, xm, yf, font, RO, active, fg);
	  }
	else
	  {
	     /* Trace un crochet fermant */
	     xm = x + ((l - CarWidth ('\371', font)) / 2);
	     yf = y - FontAscent (font) + CarAscent ('\371', font);
	     DrawChar ('\371', frame, xm, yf, font, RO, active, fg);
	     yend = y + h - CarHeight ('\373', font) -
		FontAscent (font) + CarAscent ('\373', font);
	     DrawChar ('\373', frame, xm, yend, font, RO, active, fg);
	     for (yf = yf + CarHeight ('\371', font) -
		  FontAscent (font) + CarAscent ('\372', font);
		  yf < yend;
		  yf += CarHeight ('\372', font))
		DrawChar ('\372', frame, xm, yf, font, RO, active, fg);
	  }
     }
}

/* ---------------------------------------------------------------------- */
/* |    DrawParenthesis trace un symbole parenthese ouvrant ou fermant.    | */
/* |            Le parame`tre RO indique s'il s'agit d'une boi^te en    | */
/* |            Read Only (1) ou non (0).                               | */
/* |            Le parame`tre active indique s'il s'agit d'une boi^te   | */
/* |            active (1) ou non (0).                                  | */
/* |            Le parame`tre fg indique la couleur du trace'.          | */
/* ---------------------------------------------------------------------- */


#ifdef __STDC__
void                DrawParenthesis (int frame, int epais, int x, int y, int l, int h, int sens, ptrfont font, int RO, int active, int fg)

#else  /* __STDC__ */
void                DrawParenthesis (frame, epais, x, y, l, h, sens, font, RO, active, fg)
int                 frame;
int                 epais;
int                 x;
int                 y;
int                 l;
int                 h;
int                 sens;
ptrfont             font;
int                 RO;
int                 active;
int                 fg;

#endif /* __STDC__ */

{
   int                 xm, yf, yend, exnum, delta;

   exnum = 0;

   if (FontHeight (font) >= h)
     {
	/* Avec un seul caractere */
	if (sens == 0)
	  {
	     /* Trace un parenthese ouvrant */
	     xm = x + ((l - CarWidth ('(', font)) / 2);
	     yf = y + ((h - CarHeight ('(', font)) / 2) - FontAscent (font) + CarAscent ('(', font);
	     DrawChar ('(', frame, xm, yf, font, RO, active, fg);
	  }
	else
	  {
	     /* Trace un parenthese fermant */
	     xm = x + ((l - CarWidth (')', font)) / 2);
	     yf = y + ((h - CarHeight (')', font)) / 2) - FontAscent (font) + CarAscent (')', font);
	     DrawChar (')', frame, xm, yf, font, RO, active, fg);
	  }
     }

   else
     {
	/* Avec plusieurs caracteres */
	if (sens == 0)
	  {
	     /* Trace un parenthese ouvrant */
	     xm = x + ((l - CarWidth ('\346', font)) / 2);
	     yf = y - FontAscent (font) + CarAscent ('\346', font);
	     DrawChar ('\346', frame, xm, yf, font, RO, active, fg);
	     yend = y + h - CarHeight ('\350', font) - FontAscent (font) + CarAscent ('\350', font) - 1;
	     DrawChar ('\350', frame, xm, yend, font, RO, active, fg);

	     yf += CarHeight ('\346', font);
	     delta = yend - yf;
	     if (delta >= 0)
	       {
		  for (yf += CarAscent ('\347', font) - FontAscent (font),
		       yend -= CarHeight ('\347', font) - 1;
		       yf < yend;
		       yf += CarHeight ('\347', font), exnum++)
		     DrawChar ('\347', frame, xm, yf, font, RO, active, fg);
		  if (exnum)
		     DrawChar ('\347', frame, xm, yend, font, RO, active, fg);
		  else		/* on centre l'extension */
		     DrawChar ('\347', frame, xm, yf + ((delta - CarHeight ('\347', font)) / 2), font, RO, active, fg);
	       }
	  }

	else
	  {
	     /* Trace un parenthese fermant */
	     xm = x + ((l - CarWidth ('\366', font)) / 2);
	     yf = y - FontAscent (font) + CarAscent ('\366', font);
	     DrawChar ('\366', frame, xm, yf, font, RO, active, fg);
	     yend = y + h - CarHeight ('\370', font) - FontAscent (font) + CarAscent ('\370', font) - 1;
	     DrawChar ('\370', frame, xm, yend, font, RO, active, fg);

	     yf += CarHeight ('\366', font);
	     delta = yend - yf;
	     if (delta >= 0)
	       {
		  for (yf += CarAscent ('\367', font) - FontAscent (font),
		       yend -= CarHeight ('\367', font) - 1;
		       yf < yend;
		       yf += CarHeight ('\367', font), exnum++)
		     DrawChar ('\367', frame, xm, yf, font, RO, active, fg);
		  if (exnum)
		     DrawChar ('\367', frame, xm, yend, font, RO, active, fg);
		  else		/* on centre l'extension */
		     DrawChar ('\367', frame, xm, yf + ((delta - CarHeight ('\367', font)) / 2), font, RO, active, fg);
	       }
	  }
     }
}


/* ---------------------------------------------------------------------- */
/* |    DrawBrace trace un symbole accolade ouvrant ou fermant.        | */
/* |            Le parame`tre RO indique s'il s'agit d'une boi^te en    | */
/* |            Read Only (1) ou non (0).                               | */
/* |            Le parame`tre active indique s'il s'agit d'une boi^te   | */
/* |            active (1) ou non (0).                                  | */
/* |            Le parame`tre fg indique la couleur du trace'.          | */
/* ---------------------------------------------------------------------- */


#ifdef __STDC__
void                DrawBrace (int frame, int epais, int x, int y, int l, int h, int sens, ptrfont font, int RO, int active, int fg)

#else  /* __STDC__ */
void                DrawBrace (frame, epais, x, y, l, h, sens, font, RO, active, fg)
int                 frame;
int                 epais;
int                 x;
int                 y;
int                 l;
int                 h;
int                 sens;
ptrfont             font;
int                 RO;
int                 active;
int                 fg;

#endif /* __STDC__ */

{
   int                 xm, ym, yf, yend, exnum, delta;

   exnum = 0;

   if (FontHeight (font) >= h)
     {
	/* Avec un seul caractere */
	if (sens == 0)
	  {
	     /* Trace un accolade ouvrant */
	     xm = x + ((l - CarWidth ('{', font)) / 2);
	     yf = y + ((h - CarHeight ('{', font)) / 2) - FontAscent (font) + CarAscent ('{', font);
	     DrawChar ('{', frame, xm, yf, font, RO, active, fg);
	  }
	else
	  {
	     /* Trace un accolade fermant */
	     xm = x + ((l - CarWidth ('}', font)) / 2);
	     yf = y + ((h - CarHeight ('}', font)) / 2) - FontAscent (font) + CarAscent ('}', font);
	     DrawChar ('}', frame, xm, yf, font, RO, active, fg);
	  }
     }

   else
     {
	/* Avec plusieurs caracteres */
	if (sens == 0)
	  {
	     /* Trace un accolade ouvrant */
	     /* le haut */
	     xm = x + ((l - CarWidth ('\354', font)) / 2);
	     yf = y - FontAscent (font) + CarAscent ('\354', font);
	     DrawChar ('\354', frame, xm, yf, font, RO, active, fg);
	     /* le milieu */
	     ym = y + ((h - CarHeight ('\355', font)) / 2) - FontAscent (font)
		+ CarAscent ('\355', font);
	     DrawChar ('\355', frame, xm, ym, font, RO, active, fg);
	     /* le bas */
	     yend = y + h - CarHeight ('\356', font) - FontAscent (font) + CarAscent ('\356', font);
	     DrawChar ('\356', frame, xm, yend, font, RO, active, fg);

	     /* complement superieur */
	     yf += CarHeight ('\354', font);
	     delta = ym - yf;
	     if (delta >= 0)
	       {
		  for (yf += CarAscent ('\357', font) - FontAscent (font),
		       ym -= CarHeight ('\357', font);
		       yf < ym;
		       yf += CarHeight ('\357', font), exnum++)
		     DrawChar ('\357', frame, xm, yf, font, RO, active, fg);
		  if (exnum)
		     DrawChar ('\357', frame, xm, ym, font, RO, active, fg);
		  else		/* on centre l'extension */
		     DrawChar ('\357', frame, xm, yf + ((delta - CarHeight ('\357', font)) / 2), font, RO, active, fg);
	       }
	     /* complement inferieur */
	     yf = ym + CarHeight ('\355', font) + CarHeight ('\357', font);
	     delta = yend - yf;
	     if (delta >= 0)
	       {
		  for (yf += CarAscent ('\357', font) - FontAscent (font),
		       yend -= CarHeight ('\357', font);
		       yf < yend;
		       yf += CarHeight ('\357', font), exnum++)
		     DrawChar ('\357', frame, xm, yf, font, RO, active, fg);
		  if (exnum)
		     DrawChar ('\357', frame, xm, yend, font, RO, active, fg);
		  else		/* on centre l'extension */
		     DrawChar ('\357', frame, xm, yf + ((delta - CarHeight ('\357', font)) / 2), font, RO, active, fg);
	       }
	  }

	else
	  {
	     /* Trace un accolade fermant */
	     /* le haut */
	     xm = x + ((l - CarWidth ('\374', font)) / 2);
	     yf = y - FontAscent (font) + CarAscent ('\374', font);
	     DrawChar ('\374', frame, xm, yf, font, RO, active, fg);
	     /* le milieu */
	     ym = y + ((h - CarHeight ('\375', font)) / 2)
		- FontAscent (font) + CarAscent ('\375', font);
	     DrawChar ('\375', frame, xm, ym, font, RO, active, fg);
	     /* le bas */
	     yend = y + h - CarHeight ('\376', font)
		- FontAscent (font) + CarAscent ('\376', font);
	     DrawChar ('\376', frame, xm, yend, font, RO, active, fg);
	     /* complement superieur */
	     yf += CarHeight ('\374', font);
	     delta = ym - yf;
	     if (delta >= 0)
	       {
		  for (yf += CarAscent ('\357', font) - FontAscent (font),
		       ym -= CarHeight ('\357', font);
		       yf < ym;
		       yf += CarHeight ('\357', font), exnum++)
		     DrawChar ('\357', frame, xm, yf, font, RO, active, fg);
		  if (exnum)
		     DrawChar ('\357', frame, xm, ym, font, RO, active, fg);
		  else		/* on centre l'extension */
		     DrawChar ('\357', frame, xm, yf + ((delta - CarHeight ('\357', font)) / 2), font, RO, active, fg);
	       }
	     /* complement inferieur */
	     yf = ym + CarHeight ('\375', font) + CarHeight ('\357', font);
	     delta = yend - yf;
	     if (delta >= 0)
	       {
		  for (yf += CarAscent ('\357', font) - FontAscent (font),
		       yend -= CarHeight ('\357', font);
		       yf < yend;
		       yf += CarHeight ('\357', font), exnum++)
		     DrawChar ('\357', frame, xm, yf, font, RO, active, fg);
		  if (exnum)
		     DrawChar ('\357', frame, xm, yend, font, RO, active, fg);
		  else		/* on centre l'extension */
		     DrawChar ('\357', frame, xm, yf + ((delta - CarHeight ('\357', font)) / 2), font, RO, active, fg);
	       }
	  }
     }
}

/* ---------------------------------------------------------------------- */
/* |    DrawRectangle trace un rectangle d'origine x, y et de dimensions  | */
/* |            larg, haut avec une e'paisseur epais dans la fenetree^tre       | */
/* |            d'indice frame.                                         | */
/* |            Le parame`tre RO indique s'il s'agit d'une boi^te en    | */
/* |            Read Only (1) ou non (0).                               | */
/* |            Le parame`tre active indique s'il s'agit d'une boi^te   | */
/* |            active (1) ou non (0).                                  | */
/* |            Les parame`tres fg, bg, motif indiquent la couleur du   | */
/* |            trace', la couleur du fond et le motif de remplissage.  | */
/* ---------------------------------------------------------------------- */


#ifdef __STDC__
void                DrawRectangle (int frame, int epais, int style, int x, int y, int larg, int haut, int RO, int active, int fg, int bg, int motif)

#else  /* __STDC__ */
void                DrawRectangle (frame, epais, style, x, y, larg, haut, RO, active, fg, bg, motif)
int                 frame;
int                 epais;
int                 style;
int                 x;
int                 y;
int                 larg;
int                 haut;
int                 RO;
int                 active;
int                 fg;
int                 bg;
int                 motif;

#endif /* __STDC__ */

{
   /*int eps2; */
   Pixmap              modele;

#ifdef NEW_WILLOWS
   HBRUSH              hBrush;

#endif

   larg = larg - epais - 1;
   haut = haut - epais - 1;
   x += epais / 2;
   y += epais / 2;
   /*eps2 = epais > 1; */

   /* On remplit le rectangle */
   modele = CreatePattern (0, RO, active, fg, bg, motif);
   if (modele != 0)
     {
#ifndef NEW_WILLOWS
	XSetTile (TtDisplay, TtGreyGC, modele);
	XFillRectangle (TtDisplay, FrRef[frame], TtGreyGC,
			x + FrameTable[frame].FrLeftMargin, y + FrameTable[frame].FrTopMargin, larg, haut);
	XFreePixmap (TtDisplay, modele);
#endif /* NEW_WILLOWS */
#ifdef NEW_WILLOWS
	WIN_GetDeviceContext (frame);
	WinLoadGC (WIN_curHdc, TtLineGC);
	hBrush = CreateSolidBrush (Pix_Color[bg]);
	hBrush = SelectObject (WIN_curHdc, hBrush);
	PatBlt (WIN_curHdc, x + FrameTable[frame].FrLeftMargin, y + FrameTable[frame].FrTopMargin, larg, haut, PATCOPY);
	hBrush = SelectObject (WIN_curHdc, hBrush);
	DeleteObject (hBrush);
#endif /* NEW_WILLOWS */
     }

   /* On trace le contour */
   if (epais > 0)
     {
	preparerTrace (0, style, epais, RO, active, fg);
#ifndef NEW_WILLOWS
	XDrawRectangle (TtDisplay, FrRef[frame], TtLineGC,
			x + FrameTable[frame].FrLeftMargin, y + FrameTable[frame].FrTopMargin, larg, haut);
#endif /* NEW_WILLOWS */
	finirTrace (0, RO, active);
     }
}

/* ---------------------------------------------------------------------- */
/* |    DrawDiamond trace un losange.                                     | */
/* |            Le parame`tre RO indique s'il s'agit d'une boi^te en    | */
/* |            Read Only (1) ou non (0).                               | */
/* |            Le parame`tre active indique s'il s'agit d'une boi^te   | */
/* |            active (1) ou non (0).                                  | */
/* |            Les parame`tres fg, bg, motif indiquent la couleur du   | */
/* |            trace', la couleur du fond et le motif de remplissage.  | */
/* ---------------------------------------------------------------------- */


#ifdef __STDC__
void                DrawDiamond (int frame, int epais, int style, int x, int y, int larg, int haut, int RO, int active, int fg, int bg, int motif)

#else  /* __STDC__ */
void                DrawDiamond (frame, epais, style, x, y, larg, haut, RO, active, fg, bg, motif)
int                 frame;
int                 epais;
int                 style;
int                 x;
int                 y;
int                 larg;
int                 haut;
int                 RO;
int                 active;
int                 fg;
int                 bg;
int                 motif;

#endif /* __STDC__ */

{
#ifndef NEW_WILLOWS
   ThotPoint           point[5];
   Pixmap              modele;

   larg = larg - epais - 1;
   haut = haut - epais - 1;
   x += epais / 2;
   y += epais / 2;

   point[0].x = x + (larg / 2) + FrameTable[frame].FrLeftMargin;
   point[0].y = y + FrameTable[frame].FrTopMargin;
   point[4].x = point[0].x;
   point[4].y = point[0].y;
   point[1].x = x + larg + FrameTable[frame].FrLeftMargin;
   point[1].y = y + (haut / 2) + FrameTable[frame].FrTopMargin;
   point[2].x = point[0].x;
   point[2].y = y + haut + FrameTable[frame].FrTopMargin;
   point[3].x = x + FrameTable[frame].FrLeftMargin;
   point[3].y = point[1].y;

   /* On remplit le losange */
   modele = CreatePattern (0, RO, active, fg, bg, motif);
   if (modele != 0)
     {
	XSetTile (TtDisplay, TtGreyGC, modele);
	XFillPolygon (TtDisplay, FrRef[frame], TtGreyGC,
		      point, 5, Convex, CoordModeOrigin);
	XFreePixmap (TtDisplay, modele);
     }

   /* On trace le contour */
   if (epais > 0)
     {
	preparerTrace (0, style, epais, RO, active, fg);
	XDrawLines (TtDisplay, FrRef[frame], TtLineGC,
		    point, 5, CoordModeOrigin);
	finirTrace (0, RO, active);
     }
#endif /* NEW_WILLOWS */
}

/*debut */
/* ---------------------------------------------------------------------- */
/* |    DrawSegments trace des lignes brise'es.                            | */
/* |            Le parame`tre buffer pointe sur le 1er buffer qui       | */
/* |            contient la liste des points de contro^le et le         | */
/* |            parame`tre nb donne le nombre de points.                | */
/* |            Le premier point doone la limite de la polyline.        | */
/* |            Le parame`tre RO indique s'il s'agit d'une boi^te en    | */
/* |            Read Only (1) ou non (0).                               | */
/* |            Le parame`tre active indique s'il s'agit d'une boi^te   | */
/* |            active (1) ou non (0).                                  | */
/* |            Les parame`tres fg indique la couleur du trace'.        | */
/* |            Le parametre fleche indique :                           | */
/* |            - s'il ne faut pas tracer de fleche (0)                 | */
/* |            - s'il faut tracer une fleche vers l'avant (1)          | */
/* |            - s'il faut tracer une fleche vers l'arriere (2)        | */
/* |            - s'il faut tracer une fleche dans les deux sens (3)    | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                DrawSegments (int frame, int epais, int style, int x, int y, PtrTextBuffer buffer, int nb, int RO, int active, int fg, int fleche)

#else  /* __STDC__ */
void                DrawSegments (frame, epais, style, x, y, buffer, nb, RO, active, fg, fleche)
int                 frame;
int                 epais;
int                 style;
int                 x;
int                 y;
PtrTextBuffer      buffer;
int                 nb;
int                 RO;
int                 active;
int                 fg;
int                 fleche;

#endif /* __STDC__ */

{
#ifndef NEW_WILLOWS
   ThotPoint          *points;
   int                 i, j;
   PtrTextBuffer      adbuff;

   if (epais == 0)
      return;

   /* Alloue une table de points */
   points = (ThotPoint *) TtaGetMemory (sizeof (ThotPoint) * (nb - 1));
  /***x += epais/2;
  y += epais/2;***/
   adbuff = buffer;
   j = 1;
   for (i = 1; i < nb; i++)
     {
	if (j >= adbuff->BuLength)
	  {
	     if (adbuff->BuNext != NULL)
	       {
		  /* Changement de buffer */
		  adbuff = adbuff->BuNext;
		  j = 0;
	       }
	  }
	points[i - 1].x = x + FrameTable[frame].FrLeftMargin + PointToPixel (adbuff->BuPoints[j].XCoord / 1000);
	points[i - 1].y = y + FrameTable[frame].FrTopMargin + PointToPixel (adbuff->BuPoints[j].YCoord / 1000);
	j++;
     }


   /* fleche vers l'arriere  */
   if (fleche == 2 || fleche == 3)
      TraceFleche (frame, points[1].x, points[1].y, points[0].x, points[0].y, epais, RO, active, fg);

   /* On trace le contour */
   preparerTrace (0, style, epais, RO, active, fg);
   XDrawLines (TtDisplay, FrRef[frame], TtLineGC, points, nb - 1, CoordModeOrigin);
   finirTrace (0, RO, active);

   /* fleche vers l'avant */
   if (fleche == 1 || fleche == 3)
      TraceFleche (frame, points[nb - 3].x, points[nb - 3].y, points[nb - 2].x, points[nb - 2].y, epais, RO, active, fg);

   /* Libere la table de points */
   free ((char *) points);
#endif /* NEW_WILLOWS */
}

/* ---------------------------------------------------------------------- */
/* |    DrawPolygon trace un polygone.                                   | */
/* |            Le parame`tre buffer pointe sur le 1er buffer qui       | */
/* |            contient la liste des points de contro^le et le         | */
/* |            parame`tre nb donne le nombre de points.                | */
/* |            Le premier point doone la limite de la polyline.        | */
/* |            Le parame`tre RO indique s'il s'agit d'une boi^te en    | */
/* |            Read Only (1) ou non (0).                               | */
/* |            Le parame`tre active indique s'il s'agit d'une boi^te   | */
/* |            active (1) ou non (0).                                  | */
/* |            Les parame`tres fg, bg, motif indiquent la couleur du   | */
/* |            trace', la couleur du fond et le motif de remplissage.  | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                DrawPolygon (int frame, int epais, int style, int x, int y, PtrTextBuffer buffer, int nb, int RO, int active, int fg, int bg, int motif)

#else  /* __STDC__ */
void                DrawPolygon (frame, epais, style, x, y, buffer, nb, RO, active, fg, bg, motif)
int                 frame;
int                 epais;
int                 style;
int                 x;
int                 y;
PtrTextBuffer      buffer;
int                 nb;
int                 RO;
int                 active;
int                 fg;
int                 bg;
int                 motif;

#endif /* __STDC__ */

{
#ifndef NEW_WILLOWS
   ThotPoint          *points;
   int                 i, j;
   PtrTextBuffer      adbuff;
   Pixmap              modele;

   /* Alloue une table de points */
   points = (ThotPoint *) TtaGetMemory (sizeof (ThotPoint) * nb);
  /***x += epais/2;
  y += epais/2;***/
   adbuff = buffer;
   j = 1;
   for (i = 1; i < nb; i++)
     {
	if (j >= adbuff->BuLength)
	  {
	     if (adbuff->BuNext != NULL)
	       {
		  /* Changement de buffer */
		  adbuff = adbuff->BuNext;
		  j = 0;
	       }
	  }
	points[i - 1].x = x + FrameTable[frame].FrLeftMargin + PointToPixel (adbuff->BuPoints[j].XCoord / 1000);
	points[i - 1].y = y + FrameTable[frame].FrTopMargin + PointToPixel (adbuff->BuPoints[j].YCoord / 1000);
	j++;
     }
   /* Ferme le polygone */
   points[nb - 1].x = points[0].x;
   points[nb - 1].y = points[0].y;

   /* On remplit le polygone */
   modele = CreatePattern (0, RO, active, fg, bg, motif);
   if (modele != 0)
     {
	XSetTile (TtDisplay, TtGreyGC, modele);
	XFillPolygon (TtDisplay, FrRef[frame], TtGreyGC, points, nb, Complex, CoordModeOrigin);
	XFreePixmap (TtDisplay, modele);
     }

   /* On trace le contour */
   if (epais > 0)
     {
	preparerTrace (0, style, epais, RO, active, fg);
	XDrawLines (TtDisplay, FrRef[frame], TtLineGC, points, nb, CoordModeOrigin);
	finirTrace (0, RO, active);
     }
   /* Libere la table de points */
   free ((char *) points);
#endif /* NEW_WILLOWS */
}


/* ---------------------------------------------------------------------- */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static boolean      NouveauPoint (int x, int y)
#else  /* __STDC__ */
static boolean      NouveauPoint (x, y)
int                 x, y;

#endif /* __STDC__ */
{
#ifndef NEW_WILLOWS
   ThotPoint          *tmp;
   int                 taille;

   if (npoints >= MAX_points)
     {
	taille = MAX_points + ALLOC_POINTS;
	if ((tmp = (ThotPoint *) realloc (points, taille * sizeof (ThotPoint))) == 0)
	   return (FALSE);
	else
	  {
	     /* la reallocation a reussi */
	     points = tmp;
	     MAX_points = taille;
	  }
     }

   /* ignore identical points */
   if (npoints > 0 && points[npoints - 1].x == x && points[npoints - 1].y == y)
      return (FALSE);

   points[npoints].x = x;
   points[npoints].y = y;
   npoints++;
#endif /* NEW_WILLOWS */
   return (TRUE);
}

/* ---------------------------------------------------------------------- */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         empile (float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4)
#else  /* __STDC__ */
static void         empile (x1, y1, x2, y2, x3, y3, x4, y4)
float               x1, y1, x2, y2, x3, y3, x4, y4;

#endif /* __STDC__ */
{
   Pile               *pile_haut;

   if (pile_profond == MAX_pile)
      return;

   pile_haut = &pile[pile_profond];
   pile_haut->x1 = x1;
   pile_haut->y1 = y1;
   pile_haut->x2 = x2;
   pile_haut->y2 = y2;
   pile_haut->x3 = x3;
   pile_haut->y3 = y3;
   pile_haut->x4 = x4;
   pile_haut->y4 = y4;
   pile_profond++;
}

/* ---------------------------------------------------------------------- */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static boolean      depile (float *x1, float *y1, float *x2, float *y2, float *x3, float *y3, float *x4, float *y4)
#else  /* __STDC__ */
static boolean      depile (x1, y1, x2, y2, x3, y3, x4, y4)
float              *x1, *y1, *x2, *y2, *x3, *y3, *x4, *y4;

#endif /* __STDC__ */
{
   Pile               *pile_haut;

   if (pile_profond == 0)
      return (FALSE);

   pile_profond--;
   pile_haut = &pile[pile_profond];
   *x1 = pile_haut->x1;
   *y1 = pile_haut->y1;
   *x2 = pile_haut->x2;
   *y2 = pile_haut->y2;
   *x3 = pile_haut->x3;
   *y3 = pile_haut->y3;
   *x4 = pile_haut->x4;
   *y4 = pile_haut->y4;
   return (TRUE);
}


/* ---------------------------------------------------------------------- */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         Segmente (float a1, float b1, float a2, float b2, float a3, float b3, float a4, float b4)
#else  /* __STDC__ */
static void         Segmente (a1, b1, a2, b2, a3, b3, a4, b4)
float               a1, b1, a2, b2, a3, b3, a4, b4;

#endif /* __STDC__ */
{
   register float      tx, ty;
   float               x1, y1, x2, y2, x3, y3, x4, y4;
   float               sx1, sy1, sx2, sy2;
   float               tx1, ty1, tx2, ty2, xmid, ymid;

   pile_profond = 0;
   empile (a1, b1, a2, b2, a3, b3, a4, b4);

   while (depile (&x1, &y1, &x2, &y2, &x3, &y3, &x4, &y4))
     {
	if (fabs (x1 - x4) < SEG_SPLINE && fabs (y1 - y4) < SEG_SPLINE)
	   NouveauPoint (FloatToInt (x1), FloatToInt (y1));
	else
	  {
	     tx = le_milieu (x2, x3);
	     ty = le_milieu (y2, y3);
	     sx1 = le_milieu (x1, x2);
	     sy1 = le_milieu (y1, y2);
	     sx2 = le_milieu (sx1, tx);
	     sy2 = le_milieu (sy1, ty);
	     tx2 = le_milieu (x3, x4);
	     ty2 = le_milieu (y3, y4);
	     tx1 = le_milieu (tx2, tx);
	     ty1 = le_milieu (ty2, ty);
	     xmid = le_milieu (sx2, tx1);
	     ymid = le_milieu (sy2, ty1);

	     empile (xmid, ymid, tx1, ty1, tx2, ty2, x4, y4);
	     empile (x1, y1, sx1, sy1, sx2, sy2, xmid, ymid);
	  }
     }
}


/* ---------------------------------------------------------------------- */
/* |    DrawCurb trace une courbe ouverte.                              | */
/* |            Le parame`tre buffer pointe sur le 1er buffer qui       | */
/* |            contient la liste des points de contro^le et le         | */
/* |            parame`tre nb donne le nombre de points.                | */
/* |            Le premier point donne la limite de la polyline.        | */
/* |            Le parame`tre RO indique s'il s'agit d'une boi^te en    | */
/* |            Read Only (1) ou non (0).                               | */
/* |            Le parame`tre active indique s'il s'agit d'une boi^te   | */
/* |            active (1) ou non (0).                                  | */
/* |            Le parame`tre fg donne la couleur du trace'             | */
/* |            Le parametre fleche indique :                           | */
/* |            - s'il ne faut pas tracer de fleche (0)                 | */
/* |            - s'il faut tracer une fleche vers l'avant (1)          | */
/* |            - s'il faut tracer une fleche vers l'arriere (2)        | */
/* |            - s'il faut tracer une fleche dans les deux sens (3)    | */
/* |            Le parametre controls contient les points de controle.  | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                DrawCurb (int frame, int epais, int style, int x, int y, PtrTextBuffer buffer, int nb, int RO, int active, int fg, int fleche, C_points * controls)

#else  /* __STDC__ */
void                DrawCurb (frame, epais, style, x, y, buffer, nb, RO, active, fg, fleche, controls)
int                 frame;
int                 epais;
int                 style;
int                 x;
int                 y;
PtrTextBuffer      buffer;
int                 nb;
int                 RO;
int                 active;
int                 fg;
int                 fleche;
C_points           *controls;

#endif /* __STDC__ */
{
   PtrTextBuffer      adbuff;
   int                 i, j;
   float               x1, y1, x2, y2;
   float               cx1, cy1, cx2, cy2;

   if (epais == 0)
      return;

   /* alloue la liste des points */
   npoints = 0;
   MAX_points = ALLOC_POINTS;
#ifndef NEW_WILLOWS
   points = (ThotPoint *) TtaGetMemory (sizeof (ThotPoint) * MAX_points);
#endif /* NEW_WILLOWS */

  /*** x += epais/2;
  y += epais/2 ; ***/
   adbuff = buffer;
   j = 1;
   x1 = (float) (x + FrameTable[frame].FrLeftMargin + PointToPixel (adbuff->BuPoints[j].XCoord / 1000));
   y1 = (float) (y + FrameTable[frame].FrTopMargin + PointToPixel (adbuff->BuPoints[j].YCoord / 1000));
   j++;
   cx1 = (controls[j].lx * 3 + x1 - x - FrameTable[frame].FrLeftMargin) / 4 + x + FrameTable[frame].FrLeftMargin;
   cy1 = (controls[j].ly * 3 + y1 - y - FrameTable[frame].FrTopMargin) / 4 + y + FrameTable[frame].FrTopMargin;
   x2 = (float) (x + FrameTable[frame].FrLeftMargin + PointToPixel (adbuff->BuPoints[j].XCoord / 1000));
   y2 = (float) (y + FrameTable[frame].FrTopMargin + PointToPixel (adbuff->BuPoints[j].YCoord / 1000));
   cx2 = (controls[j].lx * 3 + x2 - x - FrameTable[frame].FrLeftMargin) / 4 + x + FrameTable[frame].FrLeftMargin;
   cy2 = (controls[j].ly * 3 + y2 - y - FrameTable[frame].FrTopMargin) / 4 + y + FrameTable[frame].FrTopMargin;

   /* fleche vers l'arriere  */
   if (fleche == 2 || fleche == 3)
      TraceFleche (frame, FloatToInt (cx1), FloatToInt (cy1), (int) x1, (int) y1, epais, RO, active, fg);

   for (i = 2; i < nb; i++)
     {
	Segmente (x1, y1, cx1, cy1, cx2, cy2, x2, y2);

	/* passe aux points suivants */
	x1 = x2;
	y1 = y2;
	cx1 = controls[i].rx + x + FrameTable[frame].FrLeftMargin;
	cy1 = controls[i].ry + y + FrameTable[frame].FrTopMargin;
	if (i < nb - 1)
	  {
	     /* ce n'est pas le dernier tour */
	     j++;
	     if (j >= adbuff->BuLength)
	       {
		  if (adbuff->BuNext != NULL)
		    {
		       /* Changement de buffer */
		       adbuff = adbuff->BuNext;
		       j = 0;
		    }
	       }
	     x2 = (float) (x + FrameTable[frame].FrLeftMargin + PointToPixel (adbuff->BuPoints[j].XCoord / 1000));
	     y2 = (float) (y + FrameTable[frame].FrTopMargin + PointToPixel (adbuff->BuPoints[j].YCoord / 1000));
	     if (i == nb - 2)
	       {
		  cx1 = (controls[i].rx * 3 + x1 - x - FrameTable[frame].FrLeftMargin) / 4 + x + FrameTable[frame].FrLeftMargin;
		  cy1 = (controls[i].ry * 3 + y1 - y - FrameTable[frame].FrTopMargin) / 4 + y + FrameTable[frame].FrTopMargin;
		  cx2 = (controls[i].rx * 3 + x2 - x - FrameTable[frame].FrLeftMargin) / 4 + x + FrameTable[frame].FrLeftMargin;
		  cy2 = (controls[i].ry * 3 + y2 - y - FrameTable[frame].FrTopMargin) / 4 + y + FrameTable[frame].FrTopMargin;
	       }
	     else
	       {
		  cx2 = controls[i + 1].lx + x + FrameTable[frame].FrLeftMargin;
		  cy2 = controls[i + 1].ly + y + FrameTable[frame].FrTopMargin;
	       }
	  }
     }
   NouveauPoint ((int) x2, (int) y2);

   /* On trace le contour */
   preparerTrace (0, style, epais, RO, active, fg);
#ifndef NEW_WILLOWS
   XDrawLines (TtDisplay, FrRef[frame], TtLineGC, points, npoints, CoordModeOrigin);
#endif /* NEW_WILLOWS */

   /* fleche vers l'avant */
   if (fleche == 1 || fleche == 3)
      TraceFleche (frame, FloatToInt (cx2), FloatToInt (cy2), (int) x2, (int) y2, epais, RO, active, fg);

   finirTrace (0, RO, active);
   /* Libere la table de points */
   free ((char *) points);
}

/* ---------------------------------------------------------------------- */
/* |    DrawSpline trace une courbe fermee.                               | */
/* |            Le parame`tre buffer pointe sur le 1er buffer qui       | */
/* |            contient la liste des points de contro^le et le         | */
/* |            parame`tre nb donne le nombre de points.                | */
/* |            Le premier point donne la limite de la polyline.        | */
/* |            Le parame`tre RO indique s'il s'agit d'une boi^te en    | */
/* |            Read Only (1) ou non (0).                               | */
/* |            Le parame`tre active indique s'il s'agit d'une boi^te   | */
/* |            active (1) ou non (0).                                  | */
/* |            Les parame`tres fg, bg, motif indiquent la couleur du   | */
/* |            trace', la couleur du fond et le motif de remplissage.  | */
/* |            Le parametre controls contient les points de controle.  | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                DrawSpline (int frame, int epais, int style, int x, int y, PtrTextBuffer buffer, int nb, int RO, int active, int fg, int bg, int motif, C_points * controls)

#else  /* __STDC__ */
void                DrawSpline (frame, epais, style, x, y, buffer, nb, RO, active, fg, bg, motif, controls)
int                 frame;
int                 epais;
int                 style;
int                 x;
int                 y;
PtrTextBuffer      buffer;
int                 nb;
int                 RO;
int                 active;
int                 fg;
int                 bg;
int                 motif;
C_points           *controls;

#endif /* __STDC__ */
{
   PtrTextBuffer      adbuff;
   int                 i, j;
   float               x1, y1, x2, y2;
   float               cx1, cy1, cx2, cy2;
   Pixmap              modele;

   /* alloue la liste des points */
   npoints = 0;
   MAX_points = ALLOC_POINTS;
#ifndef NEW_WILLOWS
   points = (ThotPoint *) TtaGetMemory (sizeof (ThotPoint) * MAX_points);
#endif /* NEW_WILLOWS */

  /***x += epais/2;
  y += epais/2;***/
   adbuff = buffer;
   j = 1;
   x1 = (float) (x + FrameTable[frame].FrLeftMargin + PointToPixel (adbuff->BuPoints[j].XCoord / 1000));
   y1 = (float) (y + FrameTable[frame].FrTopMargin + PointToPixel (adbuff->BuPoints[j].YCoord / 1000));
   cx1 = controls[j].rx + x + FrameTable[frame].FrLeftMargin;
   cy1 = controls[j].ry + y + FrameTable[frame].FrTopMargin;
   j++;
   x2 = (float) (x + FrameTable[frame].FrLeftMargin + PointToPixel (adbuff->BuPoints[j].XCoord / 1000));
   y2 = (float) (y + FrameTable[frame].FrTopMargin + PointToPixel (adbuff->BuPoints[j].YCoord / 1000));
   cx2 = controls[j].lx + x + FrameTable[frame].FrLeftMargin;
   cy2 = controls[j].ly + y + FrameTable[frame].FrTopMargin;

   for (i = 2; i < nb; i++)
     {
	Segmente (x1, y1, cx1, cy1, cx2, cy2, x2, y2);

	/* passe aux points suivants */
	x1 = x2;
	y1 = y2;
	cx1 = controls[i].rx + x + FrameTable[frame].FrLeftMargin;
	cy1 = controls[i].ry + y + FrameTable[frame].FrTopMargin;
	if (i < nb - 1)
	  {
	     /* ce n'est pas le dernier tour */
	     j++;
	     if (j >= adbuff->BuLength)
	       {
		  if (adbuff->BuNext != NULL)
		    {
		       /* Changement de buffer */
		       adbuff = adbuff->BuNext;
		       j = 0;
		    }
	       }
	     x2 = (float) (x + FrameTable[frame].FrLeftMargin + PointToPixel (adbuff->BuPoints[j].XCoord / 1000));
	     y2 = (float) (y + FrameTable[frame].FrTopMargin + PointToPixel (adbuff->BuPoints[j].YCoord / 1000));
	     cx2 = controls[i + 1].lx + x + FrameTable[frame].FrLeftMargin;
	     cy2 = controls[i + 1].ly + y + FrameTable[frame].FrTopMargin;
	  }
	else
	  {
	     /* boucle sur le point d'origine */
	     x2 = (float) (x + FrameTable[frame].FrLeftMargin + PointToPixel (buffer->BuPoints[1].XCoord / 1000));
	     y2 = (float) (y + FrameTable[frame].FrTopMargin + PointToPixel (buffer->BuPoints[1].YCoord / 1000));
	     cx2 = controls[1].lx + x + FrameTable[frame].FrLeftMargin;
	     cy2 = controls[1].ly + y + FrameTable[frame].FrTopMargin;
	  }
     }

   /* Ferme le contour */
   Segmente (x1, y1, cx1, cy1, cx2, cy2, x2, y2);
   NouveauPoint ((int) x2, (int) y2);

   /* On remplit le polygone */
   modele = CreatePattern (0, RO, active, fg, bg, motif);
#ifndef NEW_WILLOWS
   if (modele != 0)
     {
	XSetTile (TtDisplay, TtGreyGC, modele);
	XFillPolygon (TtDisplay, FrRef[frame], TtGreyGC, points, npoints, Complex, CoordModeOrigin);
	XFreePixmap (TtDisplay, modele);
     }
#endif /* NEW_WILLOWS */

   /* On trace le contour */
   if (epais > 0)
     {
	preparerTrace (0, style, epais, RO, active, fg);
#ifndef NEW_WILLOWS
	XDrawLines (TtDisplay, FrRef[frame], TtLineGC, points, npoints, CoordModeOrigin);
#endif /* NEW_WILLOWS */
	finirTrace (0, RO, active);
     }

   /* Libere la table de points */
   free ((char *) points);
}
/*fin */

/* ---------------------------------------------------------------------- */
/* |    DrawOval trace un rectangle aux angles arrondis.                 | */
/* |            Le parame`tre RO indique s'il s'agit d'une boi^te en    | */
/* |            Read Only (1) ou non (0).                               | */
/* |            Le parame`tre active indique s'il s'agit d'une boi^te   | */
/* |            active (1) ou non (0).                                  | */
/* |            Les parame`tres fg, bg, motif indiquent la couleur du   | */
/* |            trace', la couleur du fond et le motif de remplissage.  | */
/* ---------------------------------------------------------------------- */


#ifdef __STDC__
void                DrawOval (int frame, int epais, int style, int x, int y, int larg, int haut, int RO, int active, int fg, int bg, int motif)

#else  /* __STDC__ */
void                DrawOval (frame, epais, style, x, y, larg, haut, RO, active, fg, bg, motif)
int                 frame;
int                 epais;
int                 style;
int                 x;
int                 y;
int                 larg;
int                 haut;
int                 RO;
int                 active;
int                 fg;
int                 bg;
int                 motif;

#endif /* __STDC__ */

{
#ifndef NEW_WILLOWS
   int                 arc, xf, yf;
   XArc                xarc[4];
   XSegment            seg[4];
   Pixmap              modele;
   ThotPoint           point[13];

   larg -= epais;
   haut -= epais;
   x += epais / 2;
   y += epais / 2;
   /* On definit la longueur des arcs */
   /* rayon des arcs a 3mm */
   arc = (3 * DOT_PER_INCHE) / 25.4 + 0.5;
   xf = x + larg - 1;
   yf = y + haut - 1;

   xarc[0].x = x + FrameTable[frame].FrLeftMargin;
   xarc[0].y = y + FrameTable[frame].FrTopMargin;
   xarc[0].width = arc * 2;
   xarc[0].height = xarc[0].width;
   xarc[0].angle1 = 90 * 64;
   xarc[0].angle2 = 90 * 64;

   xarc[1].x = xf - arc * 2 + FrameTable[frame].FrLeftMargin;
   xarc[1].y = xarc[0].y;
   xarc[1].width = xarc[0].width;
   xarc[1].height = xarc[0].width;
   xarc[1].angle1 = 0;
   xarc[1].angle2 = xarc[0].angle2;

   xarc[2].x = xarc[0].x;
   xarc[2].y = yf - arc * 2 + FrameTable[frame].FrTopMargin;
   xarc[2].width = xarc[0].width;
   xarc[2].height = xarc[0].width;
   xarc[2].angle1 = 180 * 64;
   xarc[2].angle2 = xarc[0].angle2;

   xarc[3].x = xarc[1].x;
   xarc[3].y = xarc[2].y;
   xarc[3].width = xarc[0].width;
   xarc[3].height = xarc[0].width;
   xarc[3].angle1 = 270 * 64;
   xarc[3].angle2 = xarc[0].angle2;

   seg[0].x1 = x + arc + FrameTable[frame].FrLeftMargin;
   seg[0].x2 = xf - arc + FrameTable[frame].FrLeftMargin;
   seg[0].y1 = y + FrameTable[frame].FrTopMargin;
   seg[0].y2 = seg[0].y1;

   seg[1].x1 = xf + FrameTable[frame].FrLeftMargin;
   seg[1].x2 = seg[1].x1;
   seg[1].y1 = y + arc + FrameTable[frame].FrTopMargin;
   seg[1].y2 = yf - arc + FrameTable[frame].FrTopMargin;

   seg[2].x1 = seg[0].x1;
   seg[2].x2 = seg[0].x2;
   seg[2].y1 = yf + FrameTable[frame].FrTopMargin;
   seg[2].y2 = seg[2].y1;

   seg[3].x1 = x + FrameTable[frame].FrLeftMargin;
   seg[3].x2 = seg[3].x1;
   seg[3].y1 = seg[1].y1;
   seg[3].y2 = seg[1].y2;

   /* On remplit la figure */
   modele = CreatePattern (0, RO, active, fg, bg, motif);
   if (modele != 0)
     {
	/* Polygone inscrit: (seg0)       */
	/*                   0--1         */
	/*                10-|  |-3       */
	/*         (seg3) |       |(seg1) */
	/*                9--|  |-4       */
	/*                   7--6         */
	/*                   (seg2)       */
	point[0].x = seg[0].x1;
	point[0].y = seg[0].y1;

	point[1].x = seg[0].x2;
	point[1].y = point[0].y;
	point[2].x = point[1].x;
	point[2].y = seg[1].y1;

	point[3].x = seg[1].x1;
	point[3].y = point[2].y;
	point[4].x = point[3].x;
	point[4].y = seg[1].y2;

	point[5].x = seg[2].x2;
	point[5].y = point[4].y;
	point[6].x = point[5].x;
	point[6].y = seg[2].y2;

	point[7].x = seg[2].x1;
	point[7].y = point[6].y;
	point[8].x = point[7].x;
	point[8].y = seg[3].y2;

	point[9].x = seg[3].x2;
	point[9].y = point[8].y;
	point[10].x = point[9].x;
	point[10].y = seg[3].y1;

	point[11].x = point[0].x;
	point[11].y = point[10].y;
	point[12].x = point[0].x;
	point[12].y = point[0].y;

	XSetTile (TtDisplay, TtGreyGC, modele);
	XFillPolygon (TtDisplay, FrRef[frame], TtGreyGC,
		      point, 13, Convex, CoordModeOrigin);
	/* Trace quatre arcs de cercle */
	XFillArcs (TtDisplay, FrRef[frame], TtGreyGC, xarc, 4);
	XFreePixmap (TtDisplay, modele);
     }

   /* On trace le contour */
   if (epais > 0)
     {
	preparerTrace (0, style, epais, RO, active, fg);
	XDrawArcs (TtDisplay, FrRef[frame], TtLineGC, xarc, 4);
	XDrawSegments (TtDisplay, FrRef[frame], TtLineGC, seg, 4);
	finirTrace (0, RO, active);
     }
#endif /* NEW_WILLOWS */
}

/* ---------------------------------------------------------------------- */
/* |    DrawEllips trace une ellipse (cas particulier un cercle).        | */
/* |            Le parame`tre RO indique s'il s'agit d'une boi^te en    | */
/* |            Read Only (1) ou non (0).                               | */
/* |            Le parame`tre active indique s'il s'agit d'une boi^te   | */
/* |            active (1) ou non (0).                                  | */
/* |            Les parame`tres fg, bg, motif indiquent la couleur du   | */
/* |            trace', la couleur du fond et le motif de remplissage.  | */
/* ---------------------------------------------------------------------- */


#ifdef __STDC__
void                DrawEllips (int frame, int epais, int style, int x, int y, int larg, int haut, int RO, int active, int fg, int bg, int motif)

#else  /* __STDC__ */
void                DrawEllips (frame, epais, style, x, y, larg, haut, RO, active, fg, bg, motif)
int                 frame;
int                 epais;
int                 style;
int                 x;
int                 y;
int                 larg;
int                 haut;
int                 RO;
int                 active;
int                 fg;
int                 bg;
int                 motif;

#endif /* __STDC__ */

{
   Pixmap              modele;

   larg -= epais + 1;
   haut -= epais + 1;
   x += epais / 2 + FrameTable[frame].FrLeftMargin;
   y += epais / 2 + FrameTable[frame].FrTopMargin;

   /* On remplit le rectangle */
   modele = CreatePattern (0, RO, active, fg, bg, motif);
#ifndef NEW_WILLOWS
   if (modele != 0)
     {
	XSetTile (TtDisplay, TtGreyGC, modele);
	XFillArc (TtDisplay, FrRef[frame], TtGreyGC,
		  x, y, larg, haut, 0, 360 * 64);
	XFreePixmap (TtDisplay, modele);
     }
#endif /* NEW_WILLOWS */

   /* On trace le contour */
   if (epais > 0)
     {
	preparerTrace (0, style, epais, RO, active, fg);
#ifndef NEW_WILLOWS
	XDrawArc (TtDisplay, FrRef[frame], TtLineGC, x, y, larg, haut, 0, 360 * 64);
#endif /* NEW_WILLOWS */
	finirTrace (0, RO, active);
     }
}

/* ---------------------------------------------------------------------- */
/* |    DrawLine trace une horizontale sur le bord superieur, au milieu  | */
/* |            ou sur le bord inferieur.                               | */
/* |            Le parame`tre RO indique s'il s'agit d'une boi^te en    | */
/* |            Read Only (1) ou non (0).                               | */
/* |            Le parame`tre active indique s'il s'agit d'une boi^te   | */
/* |            active (1) ou non (0).                                  | */
/* |            Le parame`tre fg indique la couleur du trace'.          | */
/* ---------------------------------------------------------------------- */


#ifdef __STDC__
void                DrawLine (int frame, int epais, int style, int x, int y, int l, int h, int cadrage, int RO, int active, int fg)

#else  /* __STDC__ */
void                DrawLine (frame, epais, style, x, y, l, h, cadrage, RO, active, fg)
int                 frame;
int                 epais;
int                 style;
int                 x;
int                 y;
int                 l;
int                 h;
int                 cadrage;
int                 RO;
int                 active;
int                 fg;

#endif /* __STDC__ */

{
   register int        Y;

   if (cadrage == 1)
      Y = y + (h - epais) / 2;
   else if (cadrage == 2)
      Y = y + h - epais - 1;	/* + ou - beau le moins un ... */
   else
      Y = y;
   if (epais > 0)
     {
	preparerTrace (0, style, epais, RO, active, fg);
	Tracer (frame, x, Y, x + l - 1, Y);
	finirTrace (0, RO, active);
     }
}

/* ---------------------------------------------------------------------- */
/* |    DrawTrait trace une verticale a` gauche, au milieu ou a` droite.  | */
/* |            Le parame`tre RO indique s'il s'agit d'une boi^te en    | */
/* |            Read Only (1) ou non (0).                               | */
/* |            Le parame`tre active indique s'il s'agit d'une boi^te   | */
/* |            active (1) ou non (0).                                  | */
/* |            Le parame`tre fg indique la couleur du trace'.          | */
/* ---------------------------------------------------------------------- */


#ifdef __STDC__
void                DrawTrait (int frame, int epais, int style, int x, int y, int l, int h, int cadrage, int RO, int active, int fg)

#else  /* __STDC__ */
void                DrawTrait (frame, epais, style, x, y, l, h, cadrage, RO, active, fg)
int                 frame;
int                 epais;
int                 style;
int                 x;
int                 y;
int                 l;
int                 h;
int                 cadrage;
int                 RO;
int                 active;
int                 fg;

#endif /* __STDC__ */

{
   register int        X;

   if (cadrage == 1)
      X = x + (l - epais) / 2;
   else if (cadrage == 2)
      X = x + l - epais;
   else
      X = x;

   if (epais > 0)
     {
	preparerTrace (0, style, epais, RO, active, fg);
	Tracer (frame, X, y, X, y + h);
	finirTrace (0, RO, active);
     }
}


/* ---------------------------------------------------------------------- */
/* |    DrawSlash trace une diagonale dans le sens precise'.            | */
/* |            Le parame`tre RO indique s'il s'agit d'une boi^te en    | */
/* |            Read Only (1) ou non (0).                               | */
/* |            Le parame`tre active indique s'il s'agit d'une boi^te   | */
/* |            active (1) ou non (0).                                  | */
/* |            Le parame`tre indique la couleur du trace'.             | */
/* ---------------------------------------------------------------------- */


#ifdef __STDC__
void                DrawSlash (int frame, int epais, int style, int x, int y, int l, int h, int sens, int RO, int active, int fg)

#else  /* __STDC__ */
void                DrawSlash (frame, epais, style, x, y, l, h, sens, RO, active, fg)
int                 frame;
int                 epais;
int                 style;
int                 x;
int                 y;
int                 l;
int                 h;
int                 sens;
int                 RO;
int                 active;
int                 fg;

#endif /* __STDC__ */

{
   int                 xf, yf;

   xf = x + l - 1 - epais;
   yf = y + h - 1 - epais;
   if (epais > 0)
     {
	preparerTrace (0, style, epais, RO, active, fg);
	if (sens == 0)
	   Tracer (frame, x, yf, xf, y);
	else
	   Tracer (frame, x, y, xf, yf);
	finirTrace (0, RO, active);
     }
}


/* ---------------------------------------------------------------------- */
/* |    DrawCorner trace deux bords de rectangle.                           | */
/* |            Le parame`tre RO indique s'il s'agit d'une boi^te en    | */
/* |            Read Only (1) ou non (0).                               | */
/* |            Le parame`tre active indique s'il s'agit d'une boi^te   | */
/* |            active (1) ou non (0).                                  | */
/* |            Le parame`tre fg indique la couleur du trace'.          | */
/* ---------------------------------------------------------------------- */


#ifdef __STDC__
void                DrawCorner (int frame, int epais, int style, int x, int y, int l, int h, int coin, int RO, int active, int fg)

#else  /* __STDC__ */
void                DrawCorner (frame, epais, style, x, y, l, h, coin, RO, active, fg)
int                 frame;
int                 epais;
int                 style;
int                 x;
int                 y;
int                 l;
int                 h;
int                 coin;
int                 RO;
int                 active;
int                 fg;

#endif /* __STDC__ */

{
#ifndef NEW_WILLOWS
   ThotPoint           point[3];
   int                 xf, yf;

   if (epais <= 0)
      return;

   x += FrameTable[frame].FrLeftMargin;
   y += FrameTable[frame].FrTopMargin;
   xf = x + l - 1 - epais;
   yf = y + h - 1 - epais;

   preparerTrace (0, style, epais, RO, active, fg);
   switch (coin)
	 {
	    case 0:
	       point[0].x = x;
	       point[0].y = y;
	       point[1].x = xf;
	       point[1].y = y;
	       point[2].x = xf;
	       point[2].y = yf;
	       break;
	    case 1:
	       point[0].x = xf;
	       point[0].y = y;
	       point[1].x = xf;
	       point[1].y = yf;
	       point[2].x = x;
	       point[2].y = yf;
	       break;
	    case 2:
	       point[0].x = xf;
	       point[0].y = yf;
	       point[1].x = x;
	       point[1].y = yf;
	       point[2].x = x;
	       point[2].y = y;
	       break;
	    case 3:
	       point[0].x = x;
	       point[0].y = yf;
	       point[1].x = x;
	       point[1].y = y;
	       point[2].x = xf;
	       point[2].y = y;
	       break;
	 }
   XDrawLines (TtDisplay, FrRef[frame], TtLineGC,
	       point, 3, CoordModeOrigin);
   finirTrace (0, RO, active);
#endif /* NEW_WILLOWS */
}

/* ---------------------------------------------------------------------- */
/* |    DrawRectangleFrame trace un rectangle a bords arrondis (diametre 3mm)  | */
/* |            avec un trait horizontal a 6mm du bord superieur.       | */
/* |            Le parame`tre RO indique s'il s'agit d'une boi^te en    | */
/* |            Read Only (1) ou non (0).                               | */
/* |            Le parame`tre active indique s'il s'agit d'une boi^te   | */
/* |            active (1) ou non (0).                                  | */
/* |            Les parame`tres fg, bg, motif indiquent la couleur du   | */
/* |            trace', la couleur du fond et le motif de remplissage.  | */
/* ---------------------------------------------------------------------- */


#ifdef __STDC__
void                DrawRectangleFrame (int frame, int epais, int style, int x, int y, int larg, int haut, int RO, int active, int fg, int bg, int motif)

#else  /* __STDC__ */
void                DrawRectangleFrame (frame, epais, style, x, y, larg, haut, RO, active, fg, bg, motif)
int                 frame;
int                 epais;
int                 style;
int                 x;
int                 y;
int                 larg;
int                 haut;
int                 RO;
int                 active;
int                 fg;
int                 bg;
int                 motif;

#endif /* __STDC__ */

{
#ifndef NEW_WILLOWS
   int                 arc, arc2, xf, yf;
   XArc                xarc[4];
   XSegment            seg[5];
   Pixmap              modele;
   ThotPoint           point[13];

   larg -= epais;
   haut -= epais;
   x += FrameTable[frame].FrLeftMargin + epais / 2;
   y += FrameTable[frame].FrTopMargin + epais / 2;
   /* rayon des arcs a 3mm */
   arc = (3 * DOT_PER_INCHE) / 25.4 + 0.5;
   arc2 = 2 * arc;

   xf = x + larg;
   yf = y + haut;

   xarc[0].x = x;
   xarc[0].y = y;
   xarc[0].width = arc2;
   xarc[0].height = arc2;
   xarc[0].angle1 = 90 * 64;
   xarc[0].angle2 = 90 * 64;

   xarc[1].x = xf - arc2;
   xarc[1].y = y;
   xarc[1].width = xarc[0].width;
   xarc[1].height = xarc[0].width;
   xarc[1].angle1 = 0;
   xarc[1].angle2 = xarc[0].angle2;

   xarc[2].x = x;
   xarc[2].y = yf - arc2;
   xarc[2].width = xarc[0].width;
   xarc[2].height = xarc[0].width;
   xarc[2].angle1 = 180 * 64;
   xarc[2].angle2 = xarc[0].angle2;

   xarc[3].x = xarc[1].x;
   xarc[3].y = xarc[2].y;
   xarc[3].width = xarc[0].width;
   xarc[3].height = xarc[0].width;
   xarc[3].angle1 = 270 * 64;
   xarc[3].angle2 = xarc[0].angle2;

   seg[0].x1 = x + arc;
   seg[0].y1 = y;
   seg[0].x2 = xf - arc;
   seg[0].y2 = y;

   seg[1].x1 = xf;
   seg[1].y1 = y + arc;
   seg[1].x2 = xf;
   seg[1].y2 = yf - arc;

   seg[2].x1 = seg[0].x1;
   seg[2].y1 = yf;
   seg[2].x2 = seg[0].x2;
   seg[2].y2 = yf;

   seg[3].x1 = x;
   seg[3].y1 = seg[1].y1;
   seg[3].x2 = x;
   seg[3].y2 = seg[1].y2;

   /* trait horizontal a 6mm du haut */
   if (arc2 < haut / 2)
     {
	/* pas en dessous de la mi-hauteur */
	seg[4].x1 = x;
	seg[4].y1 = y + arc2;
	seg[4].x2 = xf;
	seg[4].y2 = y + arc2;
     }

   /* On remplit la figure */
   modele = CreatePattern (0, RO, active, fg, bg, motif);

   if (modele != 0)
     {
	/* Polygone inscrit: (seg0)       */
	/*                   0--1         */
	/*                10-|  |-3       */
	/*         (seg3) |       |(seg1) */
	/*                9--|  |-4       */
	/*                   7--6         */
	/*                   (seg2)       */
	point[0].x = seg[0].x1;
	point[0].y = seg[0].y1;

	point[1].x = seg[0].x2;
	point[1].y = point[0].y;
	point[2].x = point[1].x;
	point[2].y = seg[1].y1;

	point[3].x = seg[1].x1;
	point[3].y = point[2].y;
	point[4].x = point[3].x;
	point[4].y = seg[1].y2;

	point[5].x = seg[2].x2;
	point[5].y = point[4].y;
	point[6].x = point[5].x;
	point[6].y = seg[2].y2;

	point[7].x = seg[2].x1;
	point[7].y = point[6].y;
	point[8].x = point[7].x;
	point[8].y = seg[3].y2;

	point[9].x = seg[3].x2;
	point[9].y = point[8].y;
	point[10].x = point[9].x;
	point[10].y = seg[3].y1;

	point[11].x = point[0].x;
	point[11].y = point[10].y;
	point[12].x = point[0].x;
	point[12].y = point[0].y;

	XSetTile (TtDisplay, TtGreyGC, modele);
	XFillPolygon (TtDisplay, FrRef[frame], TtGreyGC,
		      point, 13, Convex, CoordModeOrigin);
	/* Trace quatre arcs de cercle */
	XFillArcs (TtDisplay, FrRef[frame], TtGreyGC, xarc, 4);
	XFreePixmap (TtDisplay, modele);
     }

   /* On trace le contour */

   if (epais > 0)
     {
	preparerTrace (0, style, epais, RO, active, fg);
	XDrawArcs (TtDisplay, FrRef[frame], TtLineGC, xarc, 4);
	if (arc2 < haut / 2)
	   XDrawSegments (TtDisplay, FrRef[frame], TtLineGC, seg, 5);
	else
	   XDrawSegments (TtDisplay, FrRef[frame], TtLineGC, seg, 4);
	finirTrace (0, RO, active);
     }
#endif /* NEW_WILLOWS */
}

/* ---------------------------------------------------------------------- */
/* |    DrawEllipsFrame trace une ellipse avec trait horizontal a 7mm    | */
/* |            sous le sommet (pour SFGL).                             | */
/* |            Le parame`tre RO indique s'il s'agit d'une boi^te en    | */
/* |            Read Only (1) ou non (0).                               | */
/* |            Le parame`tre active indique s'il s'agit d'une boi^te   | */
/* |            active (1) ou non (0).                                  | */
/* |            Les parame`tres fg, bg, motif indiquent la couleur du   | */
/* |            trace', la couleur du fond et le motif de remplissage.  | */
/* ---------------------------------------------------------------------- */


#ifdef __STDC__
void                DrawEllipsFrame (int frame, int epais, int style, int x, int y, int larg, int haut, int RO, int active, int fg, int bg, int motif)

#else  /* __STDC__ */
void                DrawEllipsFrame (frame, epais, style, x, y, larg, haut, RO, active, fg, bg, motif)
int                 frame;
int                 epais;
int                 style;
int                 x;
int                 y;
int                 larg;
int                 haut;
int                 RO;
int                 active;
int                 fg;
int                 bg;
int                 motif;

#endif /* __STDC__ */

{
#ifndef NEW_WILLOWS
   int                 px7mm, shiftX;
   double              A;
   Pixmap              modele;

   larg -= epais + 1;
   haut -= epais + 1;
   x += FrameTable[frame].FrLeftMargin + epais / 2;
   y += FrameTable[frame].FrTopMargin + epais / 2;

   /* On remplit le rectangle */
   modele = CreatePattern (0, RO, active, fg, bg, motif);
   if (modele != 0)
     {
	XSetTile (TtDisplay, TtGreyGC, modele);
	XFillArc (TtDisplay, FrRef[frame], TtGreyGC,
		  x, y, larg, haut, 0, 360 * 64);
	XFreePixmap (TtDisplay, modele);
     }

   /* On trace le contour */
   if (epais > 0)
     {
	preparerTrace (0, style, epais, RO, active, fg);
	XDrawArc (TtDisplay, FrRef[frame], TtLineGC,
		  x, y, larg, haut, 0, 360 * 64);

	px7mm = (7 * DOT_PER_INCHE) / 25.4 + 0.5;
	if (haut > 2 * px7mm)
	  {
	     A = ((double) haut - 2 * px7mm) / haut;
	     A = 1.0 - sqrt (1 - A * A);
	     shiftX = larg * A * 0.5 + 0.5;
	     XDrawLine (TtDisplay, FrRef[frame], TtLineGC,
			x + shiftX, y + px7mm, x + larg - shiftX, y + px7mm);
	  }
	finirTrace (0, RO, active);
     }
#endif /* NEW_WILLOWS */
}
/* les procedures PSPageInfo et  psBoundingBox sont vides */
/* leur contenu est non vide dans pses.c */
/* on les garde ici pour maintenir l'identite des interfaces */
/* entre es.c et pses.c */

#ifdef __STDC__
void                PSPageInfo (int pagenum, int width, int height)

#else  /* __STDC__ */
void                PSPageInfo (pagenum, width, height)
int                 pagenum;
int                 width;
int                 height;

#endif /* __STDC__ */

{
}

#ifdef __STDC__
void                psBoundingBox (int frame, int width, int height)

#else  /* __STDC__ */
void                psBoundingBox (frame, width, height)
int                 frame;
int                 width;
int                 height;

#endif /* __STDC__ */

{
}


/* ---------------------------------------------------------------------- */
/* |    Clear nettoie la surface designee de la fenetre frame.          | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                Clear (int frame, int larg, int haut, int x, int y)

#else  /* __STDC__ */
void                Clear (frame, larg, haut, x, y)
int                 frame;
int                 larg;
int                 haut;
int                 x;
int                 y;

#endif /* __STDC__ */

{
   ThotWindow          w;

#ifdef NEW_WILLOWS
   HBRUSH              hBrush;

#endif /* NEW_WILLOWS */

   w = FrRef[frame];
   if (w != None)
     {
#ifndef NEW_WILLOWS
	XClearArea (TtDisplay, w, x + FrameTable[frame].FrLeftMargin, y + FrameTable[frame].FrTopMargin, larg, haut, FALSE);
#endif /* NEW_WILLOWS */
#ifdef NEW_WILLOWS
	WIN_GetDeviceContext (frame);
	hBrush = CreateSolidBrush (BackgroundColor[frame]);
	hBrush = SelectObject (WIN_curHdc, hBrush);
	PatBlt (WIN_curHdc, x + FrameTable[frame].FrLeftMargin, y + FrameTable[frame].FrTopMargin, larg, haut, PATCOPY);
	hBrush = SelectObject (WIN_curHdc, hBrush);
	DeleteObject (hBrush);
#endif /* NEW_WILLOWS */
     }
}


/* ---------------------------------------------------------------------- */
/* |    WChaine affiche la chaine a` la position x,y de la fenetre w    | */
/* |            en utilisant la police de caracteres font.              | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                WChaine (ThotWindow w, char *chaine, int x, int y, ptrfont font, ThotGC GClocal)
#else  /* __STDC__ */
void                WChaine (w, chaine, x, y, font, GClocal)
ThotWindow          w;
char               *chaine;
int                 x;
int                 y;
ptrfont             font;
ThotGC              GClocal;

#endif /* __STDC__ */
{
#ifndef NEW_WILLOWS
   XSetFont (TtDisplay, GClocal, ((XFontStruct *) font)->fid);
   FontOrig (font, chaine[0], &x, &y);
   XDrawString (TtDisplay, w, GClocal, x, y, chaine, strlen (chaine));
#endif /* NEW_WILLOWS */
#ifdef NEW_WILLOWS
   /* GetWinDeviceContext(w);
      WinLoadGC(WIN_curHdc, GClocal);
      WinLoadFont(WIN_curHdc, font);
      TextOut(WIN_curHdc, x, y, chaine, strlen(chaine)); */
#endif /* NEW_WILLOWS */
}


/* ---------------------------------------------------------------------- */
/* |    Invideo met en inversion video la surface designee de la        | */
/* |            frame frame.                                            | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                Invideo (int frame, int larg, int haut, int x, int y)
#else  /* __STDC__ */
void                Invideo (frame, larg, haut, x, y)
int                 frame;
int                 larg;
int                 haut;
int                 x;
int                 y;

#endif /* __STDC__ */
{
   ThotWindow          w;

   w = FrRef[frame];
   if (w != None)
     {				/* blindage necessaire */
#ifndef NEW_WILLOWS
	XFillRectangle (TtDisplay, w, TtInvertGC, x + FrameTable[frame].FrLeftMargin, y + FrameTable[frame].FrTopMargin, larg, haut);
#endif /* NEW_WILLOWS */
#ifdef NEW_WILLOWS
	WIN_GetDeviceContext (frame);
	PatBlt (WIN_curHdc, x + FrameTable[frame].FrLeftMargin, y + FrameTable[frame].FrTopMargin, larg, haut, PATINVERT);
#endif /* NEW_WILLOWS */
     }
}


/* ---------------------------------------------------------------------- */
/* |    Scroll deplace le contenu du rectangle de la fenetre d'indice   | */
/* |            frame des coordonnees xd,yd aux coordonnees xf,yf.      | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                Scroll (int frame, int larg, int haut, int xd, int yd, int xf, int yf)
#else  /* __STDC__ */
void                Scroll (frame, larg, haut, xd, yd, xf, yf)
int                 frame;
int                 larg;
int                 haut;
int                 xd;
int                 yd;
int                 xf;
int                 yf;

#endif /* __STDC__ */
{
   ThotWindow          w;

   w = FrRef[frame];
   if (w != None)
#ifndef NEW_WILLOWS
      XCopyArea (TtDisplay, w, w, TtWhiteGC, xd + FrameTable[frame].FrLeftMargin, yd + FrameTable[frame].FrTopMargin, larg, haut, xf + FrameTable[frame].FrLeftMargin, yf + FrameTable[frame].FrTopMargin);
#endif /* NEW_WILLOWS */
#ifdef NEW_WILLOWS
   WIN_GetDeviceContext (frame);
   BitBlt (WIN_curHdc, xf + FrameTable[frame].FrLeftMargin, yf + FrameTable[frame].FrTopMargin, larg, haut,
	   WIN_curHdc, xd + FrameTable[frame].FrLeftMargin, yd + FrameTable[frame].FrTopMargin, SRCCOPY);
#endif /* NEW_WILLOWS */
}


/* ---------------------------------------------------------------------- */
/* |    FinDeChaine teste si la chaine chaine se termine par suffix.    | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
int                 FinDeChaine (char *chaine, char *suffix)
#else  /* __STDC__ */
int                 FinDeChaine (chaine, suffix)
char               *chaine;
char               *suffix;

#endif /* __STDC__ */
{
   int                 long_chaine, long_suf;

   long_chaine = strlen (chaine);
   long_suf = strlen (suffix);
   if (long_chaine < long_suf)
      return 0;
   else
      return (strcmp (chaine + long_chaine - long_suf, suffix) == 0);
}


/* ---------------------------------------------------------------------- */
/* |    XFlushOutput force la mise a jour de l'ecran correspondant a`   | */
/* |            la fenetree^tre frame.                                  | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                XFlushOutput (int frame)
#else  /* __STDC__ */
void                XFlushOutput (frame)
int                 frame;

#endif /* __STDC__ */
{
#ifndef NEW_WILLOWS
   XFlush (TtDisplay);
#endif /* NEW_WILLOWS */
}


/* ---------------------------------------------------------------------- */
/* |    Trame remplit le rectangle de la fenetree^tre w ou d'indice frame       | */
/* |            (si w=0) de'fini par x, y, large, haut avec le motif    | */
/* |            donne'.                                                 | */
/* |            Le parame`tre RO indique s'il s'agit d'une boi^te en    | */
/* |            Read Only (1) ou non (0).                               | */
/* |            Le parame`tre active indique s'il s'agit d'une boi^te   | */
/* |            active (1) ou non (0).                                  | */
/* |            Les parame`tres fg, bg, motif indiquent la couleur du   | */
/* |            trace', la couleur du fond et le motif de remplissage.  | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                Trame (int frame, int x, int y, int large, int haut, ThotWindow w, int RO, int active, int fg, int bg, int motif)
#else  /* __STDC__ */
void                Trame (frame, x, y, large, haut, w, RO, active, fg, bg, motif)
int                 frame;
int                 x;
int                 y;
int                 large;
int                 haut;
ThotWindow          w;
int                 RO;
int                 active;
int                 fg;
int                 bg;
int                 motif;

#endif /* __STDC__ */
{
   Pixmap              modele;

   /* On remplit le rectangle de la fenetre designee */
   modele = CreatePattern (0, RO, active, fg, 0, motif);
#ifndef NEW_WILLOWS
   if (modele != 0)
     {
	XSetTile (TtDisplay, TtGreyGC, modele);
	if (w != 0)
	   XFillRectangle (TtDisplay, w, TtGreyGC, x, y, large, haut);
	else
	   XFillRectangle (TtDisplay, FrRef[frame], TtGreyGC, x + FrameTable[frame].FrLeftMargin, y + FrameTable[frame].FrTopMargin, large, haut);
	XFreePixmap (TtDisplay, modele);
     }
#endif /* NEW_WILLOWS */
}
