
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

/*
   pses.c : Gestion de la generation du PostScrip
   Major changes:
   I. Vatton - Juillet 87
   IV : Fevrier 92 introduction de la couleur
   IV : Aout 92 coupure des mots
   IV : Juin 93 polylines
 */

#include "thot_sys.h"
#ifdef SYSV
#endif
#include "message.h"
#include "constmedia.h"
#include "typemedia.h"
#include "frame.h"
#include "picture.h"


#undef EXPORT
#define EXPORT extern
#include "font.var"
#include "frame.var"

#define VoidPixmap (Pixmap)(-1)
#define EmptyPixmap (Pixmap)(-2)
#define HL 4

typedef struct POINT_
  {
     float               x;
     float               y;
  }
POINT;

static char        *Patterns_PS[] =
{
   "2222222222222222",		/*horiz1 */
   "6666666666666666",
   "7777777777777777",
   "0000ff000000ff00",		/*vert1 */
   "00ffff0000ffff00",
   "ffffff00ffffff00",
   "1122448811224488",		/*left */
   "993366cc993366cc",
   "ddbb77eeddbb77ee",
   "8844221188442211",		/*right1 */
   "cc663399cc663399",
   "ee77bbddee77bbdd",
   "11ff111111ff1111",		/*square1 */
   "33ffff3333ffff33",
   "77ffffff77ffffff",
   "11aa44aa11aa44aa",		/*lozenge */
   "2222223e222222e3",		/*brick */
   "4244241c244442c1",		/*tile */
   "6688888884444222",		/*sea */
   "11b87c3a11a3c78b"		/*basket */
};

extern int          NbPage;
extern int          FindePage;
int                 X, Y;
static int          LastPageNumber, LastPageWidth, LastPageHeight;
static int          MemeBoite = 0;	/* MemeBoite = 1 si le texte continue le texte precedent */
static int          NbBcour;

/* Gestion des ressources fontes chargees */
static ptrfont      PolEnPs = NULL;
static char        *Scale = NULL;
static int          ColorPs = -1;

#include "buildlines_f.h"
#include "font_f.h"
#include "initpses_f.h"



/* ---------------------------------------------------------------------- */
/* |    WriteCar ecrit s1 ou s2 dans fout suivant la valeur de codage.  | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         WriteCar (FILE * fout, int codage, char *s1, char *s2)

#else  /* __STDC__ */
static void         WriteCar (fout, codage, s1, s2)
FILE               *fout;
int                 codage;
char               *s1;
char               *s2;

#endif /* __STDC__ */

{
   if (codage == 0)
      fputs (s1, fout);
   else
      fputs (s2, fout);
}

/* ---------------------------------------------------------------------- */
/* |    Transcode ecrit le code PostScript du caractere car.            | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void         Transcode (FILE * fout, int codage, char car)

#else  /* __STDC__ */
static void         Transcode (fout, codage, car)
FILE               *fout;
int                 codage;
char                car;

#endif /* __STDC__ */

{
   if (car >= ' ' && car <= '~' && car != '(' && car != ')' && car != '\\')
      fprintf (fout, "%c", car);
   else
      switch (car)
	    {
	       case '(':
		  fputs ("\\(", fout);
		  break;
	       case ')':
		  fputs ("\\)", fout);
		  break;
	       case '*':
		  WriteCar (fout, codage, "*", "\\267");	/* bullet */
		  break;
	       case '\\':
		  fputs ("\\\\", fout);
		  break;
	       default:
		  fprintf (fout, "\\%o", (unsigned char) car);
	    }
}				/*Transcode */


/* ---------------------------------------------------------------------- */
/* |    CouleurCourante compare le dernier RGB PostScript charge' avec  | */
/* |            le nouveau RGB demande'et si ne'cessaire le modifie.    | */
/* |            Le parametre num donne l'indice de la couleur dans la   | */
/* |            table des couleurs de Thot.                             | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void         CouleurCourante (FILE * fout, int num)

#else  /* __STDC__ */
static void         CouleurCourante (fout, num)
FILE               *fout;
int                 num;

#endif /* __STDC__ */

{
   unsigned short      red;
   unsigned short      green;
   unsigned short      blue;
   float               fact;

   /* Compare la couleur demandee avec la derniere chargee */
   if (num != ColorPs)
     {
	/* Demande le RGB de la couleur */
	ColorRGB (num, &red, &green, &blue);
	/* Insere la commande de chargement PostScript */
	fact = 255;
	fprintf (fout, "%f %f %f setrgbcolor\n", ((float) red) / fact,
		 ((float) green) / fact, ((float) blue) / fact);
	ColorPs = num;
     }
}


/* ---------------------------------------------------------------------- */
/* |    Remplir remplit la forme courante soit avec un motif noir et    | */
/* |            blanc, soit avec la couleur du trace', soit avec la     | */
/* |            couleur de fond ou laisse la forme transparente selon   | */
/* |            la valeur du parame`tre motif.                          | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void         Remplir (FILE * fout, int fg, int bg, int motif)

#else  /* __STDC__ */
static void         Remplir (fout, fg, bg, motif)
FILE               *fout;
int                 fg;
int                 bg;
int                 motif;

#endif /* __STDC__ */

{
   unsigned short      red;
   unsigned short      green;
   unsigned short      blue;
   float               fact;

   fact = 255;
   /* Est-ce qu'il faut remplir la forme courante */
   if (motif == 0)
      /* Pas de remplissage */
      fprintf (fout, "0\n");
   else if (motif == 1)
     {
	/* Demande le RGB de la couleur du trace */
	ColorRGB (fg, &red, &green, &blue);
	/* Insere la commande de chargement PostScript */
	fprintf (fout, "%f %f %f -1\n", ((float) red) / fact,
		 ((float) green) / fact, ((float) blue) / fact);
     }
   else if (motif == 2)
     {
	/* Demande le RGB de la couleur de fond */
	ColorRGB (bg, &red, &green, &blue);
	/* Insere la commande de chargement PostScript */
	fprintf (fout, "%f %f %f -1\n", ((float) red) / fact,
		 ((float) green) / fact, ((float) blue) / fact);
     }
   else if (motif >= 10)
     {
	/* Utilisation d'un pattern */
	/*fprintf(fout, "<d1e3c5885c3e1d88> 8 "); */
	fprintf (fout, "<%s> 8\n", Patterns_PS[motif - 10]);
     }
   else
      /* Level de gris */
      fprintf (fout, "%d\n", motif - 2);
}


/* ---------------------------------------------------------------------- */
/* |    FontCourante de'termine la fonte PostScript demande'e et e'crit | */
/* |            le changement de police courante si ne'cessaire.        | */
/* |            Retourne 0 s'il s'agit d'une police latine, 1 s'il      | */
/* |            s'agit d'une police grecque.                            | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static int          FontCourante (FILE * fout, ptrfont font)
#else  /* __STDC__ */
static int          FontCourante (fout, font)
FILE               *fout;
ptrfont             font;

#endif /* __STDC__ */
{
   int                 i, retour;
   char                c1, c2;

   /* On parcourt la table des polices */
   i = 0;
   retour = 0;			/* BUG */
   while ((TtFonts[i] != font) && (i < MAX_FONT))
     {
	i++;
     }
   if (i >= MAX_FONT)
      i = 0;
   i = i * 8;
   if (font != PolEnPs)
     {
	PolEnPs = font;
	if (TtPsFontName[i] == 'g')	/* Alphabet Grec */
	  {
	     c1 = TtPsFontName[i];
	     c2 = 'r';		/* La police Symbol n'existe qu'en un seul style */
	     retour = 1;
	  }
	else
	  {
	     /* Alphabet Latin */
	     c1 = TtPsFontName[i + 1];	/* famille Helvetica Times Courrier */
	     /* On convertit les minuscules en majuscules */
	     c2 = TtPsFontName[i + 2];	/* Style normal bold italique */
	     retour = 0;
	  }

	/* On note l'echelle courante */
	Scale = &TtPsFontName[i + 3];
	fprintf (fout, "%c%c%c %s sf\n", TtPsFontName[i], c1, c2, Scale);
	return retour;
     }
   /* Sinon on retourne simplement l'indicateur de famille de caracteres */
   else if (TtPsFontName[i] == 'g')
      return (1);
   else
      return (0);
}				/*FontCourante */

/* ---------------------------------------------------------------------- */
/* |    DrawPage regarde s'il faut engendrer un showpage.                 | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                DrawPage (FILE * fout)

#else  /* __STDC__ */
void                DrawPage (fout)
FILE               *fout;

#endif /* __STDC__ */

{
   if (FindePage == 1)
     {
	NbPage++;
	fprintf (fout, "%d %d %d nwpage\n%%%%Page: %d %d\n", LastPageNumber, LastPageWidth, LastPageHeight, NbPage, NbPage);
	FindePage = 0;
	/* On force le chargement de la police en debut de page */
	PolEnPs = NULL;
	ColorPs = -1;
     }
}

/*debut */
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
/* |            Le parame`tre fg indique la couleur du trace'.          | */
/* |            Retourne la largeur de la chaine affichee.              | */
/* ---------------------------------------------------------------------- */
/**CO*/

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
   char               *ptcar;
   int                 j, codage, large;
   int                 blancnonjustifie;
   FILE               *fout;

   fout = (FILE *) FrRef[frame];
   codage = 0;			/* BUG */

   /* L'indicateur blancnonjustifie est positif s'il faut produire */
   /* un blanc dur et nul s'il faut produire un blanc justifie     */

   blancnonjustifie = debutbloc;

   /* Est-ce que l'on commence a traiter une nouvelle boite ? */
   if (MemeBoite == 0)
     {
	/* On debute la boite */
	MemeBoite = 1;
	X = PixelToPoint (x);
	Y = PixelToPoint (y + FontBase (font));
	NbBcour = 0;
	/* Faut-il ajouter un showpage ? */
	DrawPage (fout);

	/* Faut-il changer de RGB */
	CouleurCourante (fout, fg);

	/* Faut-il changer de police de caracteres courante */
	codage = FontCourante (fout, font);
	fprintf (fout, "(");
     }

   /* On ajoute les blancs justifies */
   if (bl > 0)
     {
	NbBcour++;
	Transcode (fout, codage, ' ');
     }

   /* On transmet les caracteres */
   ptcar = &buff[i - 1];
   for (j = 0; j < lg; j++)
     {
	/* On compte les blancs */
	if (ptcar[j] == ' ')
	   if (blancnonjustifie == 0)
	     {
		/* ecriture d'un blanc justifie */
		NbBcour++;
		Transcode (fout, codage, ptcar[j]);
	     }
	   else
	      /* ecriture d'un blanc dur */
	      fputs ("\\240", fout);
	else
	  {
	     blancnonjustifie = 0;
	     Transcode (fout, codage, ptcar[j]);
	  }
     }

   /* Faut-il ajouter le tiret d'hyphenation ? */
   if (hyphen)
      Transcode (fout, codage, '\255');

   /* Est-ce la fin de la boite ? */
   if (lgboite != 0)
     {
	lgboite = PixelToPoint (lgboite);
	/* Faut-il justifier les blancs ? */
	if (NbBcour == 0)
	   fprintf (fout, ") %d %d -%d s\n", lgboite, X, Y);
	else
	   fprintf (fout, ") %d %d %d -%d j\n", NbBcour, lgboite, X, Y);
	MemeBoite = 0;
     }

   if (lg > 0)
     {
	/* On calcule la largeur de la chaine de caractere ecrite */
	large = 0;
	j = 0;
	while (j < lg)
	   large += CarWidth (ptcar[j++], font);
	return (large);
     }
   else
      return (0);
}				/*DrawString */


/* ---------------------------------------------------------------------- */
/* |    DisplayUnderline trace un souligne                                        | */
/* ---------------------------------------------------------------------- */
/**CO*/

#ifdef __STDC__
void                DisplayUnderline (int frame, int x, int y, ptrfont font, int type, int epais, int lg, int RO, int func, int fg)

#else  /* __STDC__ */
void                DisplayUnderline (frame, x, y, font, type, epais, lg, RO, func, fg)
int                 frame;
int                 x;
int                 y;
ptrfont             font;
int                 type;
int                 epais;
int                 lg;
int                 RO;
int                 func;
int                 fg;

#endif /* __STDC__ */

{
   int                 height;	/* hauteur de la fonte   */
   int                 ascent;	/* ascent  de la fonte   */
   int                 bas;	/* position du souligne' */
   int                 milieu;	/* position du biffe'    */
   int                 haut;	/* position du surligne' */
   int                 epaisseur;	/* epaisseur du trait    */

   /*  int     decal; *//* decalage entre traits */
   int                 xdebut;	/* debut du trait        */
   int                 xfin;	/* fin dur trait         */
   FILE               *fout;

   fout = (FILE *) FrRef[frame];

   /* On doit etre sorti d'une boite */
   if (MemeBoite == 0)
     {
	height = FontHeight (font);
	ascent = FontAscent (font);
	epaisseur = ((height / 20) + 1) * (epais + 1);	/* epaisseur proportionnelle a hauteur */
	haut = y + (2 - epais) * epaisseur;
	bas = y + ascent + (2 - epais) * epaisseur;
	milieu = y + height / 2;
	/*decal = epaisseur; *//* decalage entre les traits */
	xdebut = X;		/* on recupere le X courant (cf DrawString) */
	xfin = X + PixelToPoint (lg);	/* on calcule la position de fin */

	/* Valeur en dur pour ESM */
	/*         epaisseur = 1; */
	/*         haut = y + 2 * epaisseur; */
	/*         bas = y + ascent + 3; */
	/* a mettre en commentaire ou pas suivant ce que l'on veut */

	switch (type)
	      {
		 case 0:	/* sans souligne */
		    break;

		 case 1:	/* souligne */
		    fprintf (fout, "%d -%d %d -%d %d %d %d Seg\n",
			     xfin, PixelToPoint (bas), xdebut, PixelToPoint (bas), 0, epaisseur, 2);
		    break;

		 case 2:	/* surligne */
		    fprintf (fout, "%d -%d %d -%d %d %d %d Seg\n",
			     xfin, PixelToPoint (haut), xdebut, PixelToPoint (haut), 0, epaisseur, 2);
		    break;

		 case 3:	/* biffer */
		    fprintf (fout, "%d -%d %d -%d %d %d %d Seg\n",
			     xfin, PixelToPoint (milieu), xdebut, PixelToPoint (milieu), 0, epaisseur, 2);
		    break;
	      }
     }
}				/*DisplayUnderline */


/* ---------------------------------------------------------------------- */
/* |    DrawRadical trace un radical.                                     | */
/* ---------------------------------------------------------------------- */
/**CO*/

#ifdef __STDC__
void                DrawRadical (int frame, int epais, int x, int y, int l, int h, ptrfont font, int RO, int func, int fg)

#else  /* __STDC__ */
void                DrawRadical (frame, epais, x, y, l, h, font, RO, func, fg)
int                 frame;
int                 epais;
int                 x;
int                 y;
int                 l;
int                 h;
ptrfont             font;
int                 RO;
int                 func;
int                 fg;

#endif /* __STDC__ */

{
   int                 ex, fh;
   FILE               *fout;

   fout = (FILE *) FrRef[frame];
   DrawPage (fout);		/* Faut-il ajouter un showpage ? */

   /* Faut-il changer de RGB */
   CouleurCourante (fout, fg);

   fh = FontHeight (font);
   ex = h / 3;
   if ((ex = h / 3) < 6)
      ex = 6;

   if (h <= (3 * fh))
      fprintf (fout, "%d -%d %d -%d %d -%d %d -%d r\n",
	       PixelToPoint (x + l), PixelToPoint (y),
	       PixelToPoint (x + (fh / 2)), PixelToPoint (y),
	       PixelToPoint (x + (fh / 4)), PixelToPoint (y + h),
	       PixelToPoint (x), PixelToPoint (y + (2 * (h / 3))));

   else
      fprintf (fout, "%d -%d %d -%d %d -%d %d -%d r\n",
	       PixelToPoint (x + l), PixelToPoint (y),
	       PixelToPoint (x + (fh / 2)), PixelToPoint (y),
	       PixelToPoint (x + (fh / 2)), PixelToPoint (y + h),
	       PixelToPoint (x), PixelToPoint (y + (2 * (h / 3))));

}				/*DrawRadical */

/* ---------------------------------------------------------------------- */
/* |    DrawIntegral trace une integrale :                               | */
/* |            - simple si type = 0                                    | */
/* |            - curviligne si type = 1                                | */
/* |            - double si type = 2                                    | */
/* ---------------------------------------------------------------------- */
/**CO*/

#ifdef __STDC__
void                DrawIntegral (int frame, int epais, int x, int y, int l, int h, int type, ptrfont font, int RO, int func, int fg)

#else  /* __STDC__ */
void                DrawIntegral (frame, epais, x, y, l, h, type, font, RO, func, fg)
int                 frame;
int                 epais;
int                 x;
int                 y;
int                 l;
int                 h;
int                 type;
ptrfont             font;
int                 RO;
int                 func;
int                 fg;

#endif /* __STDC__ */

{
   FILE               *fout;
   int                 ey, ym, yf;

#ifdef JA
   int                 lly;

#endif

   fout = (FILE *) FrRef[frame];
   /* Faut-il ajouter un showpage ? */
   DrawPage (fout);

   /* Faut-il changer de RGB */
   CouleurCourante (fout, fg);

#ifdef JA
   fprintf (fout, "/Symbol-fly %d ", FontHeight (font));
   lly = PixelToPoint (y + h);
   x = PixelToPoint (x);
   l = PixelToPoint (l);
   h = PixelToPoint (h);
   y = PixelToPoint (y);
   fprintf (fout, "(\\362) %d -%d %d %d flyshow\n", x, lly, l, h);
#else
   /* Faut-il changer de police de caracteres courante */
   FontCourante (fout, font);

   l--;
   h--;
   ey = FontHeight (font);
   h -= ey;
   y += FontBase (font);
   x = PixelToPoint (x + (l / 2));
   yf = PixelToPoint (y + h);
   ym = PixelToPoint (y + (h / 2));
   y = PixelToPoint (y) + 1;
   if (h < ey / 4)
     {
	/* Sur un seul caractere */
	if (type == 2)
	  {
	     /* Trace une integrale double */
	     fprintf (fout, "-%d %d (\\362) c\n", ym, x - PixelToPoint (CarWidth ('\362', font) / 4));
	     fprintf (fout, "-%d %d (\\362) c\n", ym, x + PixelToPoint (CarWidth ('\362', font) / 4));
	  }
	else
	  {
	     /* Trace une integrale simple ou circulaire */
	     fprintf (fout, "-%d %d (\\362) c\n", ym, x);
	     if (type == 1)
		fprintf (fout, "-%d %d (o) c\n", ym, x);
	  }
     }
   else
     {
	/* Sur deux caracteres ou plus */
	if (type == 2)
	  {
	     /* Trace une integrale double */
	     fprintf (fout, "%d -%d -%d %s (\\363) (\\364) (\\365) s3\n",
	      x - PixelToPoint (CarWidth ('\364', font) / 4), yf, y, Scale);
	     fprintf (fout, "%d -%d -%d %s (\\363) (\\364) (\\365) s3\n",
	      x + PixelToPoint (CarWidth ('\364', font) / 4), yf, y, Scale);
	  }
	else
	  {
	     /* Trace une integrale simple ou circulaire */
	     fprintf (fout, "%d -%d -%d %s (\\363) (\\364) (\\365) s3\n", x, yf, y, Scale);
	     if (type == 1)
		fprintf (fout, "-%d %d (o) c\n", ym, x);
	  }
     }
#endif
}				/*DrawIntegral */

/* ---------------------------------------------------------------------- */
/* |    DrawSigma trace un symbole sigma.                                 | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                DrawSigma (int frame, int x, int y, int l, int h, ptrfont font, int RO, int func, int fg)
#else  /* __STDC__ */
void                DrawSigma (frame, x, y, l, h, font, RO, func, fg)
int                 frame;
int                 x;
int                 y;
int                 l;
int                 h;
ptrfont             font;
int                 RO;
int                 func;
int                 fg;

#endif /* __STDC__ */
{
   FILE               *fout;

#ifdef JA
   int                 lly;

#endif

   fout = (FILE *) FrRef[frame];
   /* Faut-il ajouter un showpage ? */
   DrawPage (fout);

   /* Faut-il changer de RGB */
   CouleurCourante (fout, fg);

   /* On modifie la police courante */
   PolEnPs = NULL;
#ifdef JA
   fprintf (fout, "/Symbol-fly %d ", FontHeight (font));
   lly = PixelToPoint (y + h);
   x = PixelToPoint (x);
   y = PixelToPoint (y);
   l = PixelToPoint (l);
   h = PixelToPoint (h);
   fprintf (fout, "(\\345) %d -%d %d %d flyshow\n", x, lly, l, h);
#else
   fprintf (fout, "(Symbol) %.0f sf\n", FontHeight (font) * 0.9);
   x = PixelToPoint (x + (l / 2));
   y = PixelToPoint (y + h - FontHeight (font) + FontBase (font));
   fprintf (fout, "-%d %d (\\345) c\n", y, x);
#endif
}				/*DrawSigma */


/* ---------------------------------------------------------------------- */
/* |    DrawPi trace un symbole Pi.                                       | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                DrawPi (int frame, int x, int y, int l, int h, ptrfont font, int RO, int func, int fg)
#else  /* __STDC__ */
void                DrawPi (frame, x, y, l, h, font, RO, func, fg)
int                 frame;
int                 x;
int                 y;
int                 l;
int                 h;
ptrfont             font;
int                 RO;
int                 func;
int                 fg;

#endif /* __STDC__ */
{
   FILE               *fout;

   fout = (FILE *) FrRef[frame];
   DrawPage (fout);		/* Faut-il ajouter un showpage ? */

   /* Faut-il changer de RGB */
   CouleurCourante (fout, fg);

   /* On modifie la police courante */
   PolEnPs = NULL;
#ifdef JA
   fprintf (fout, "/Symbol-fly %d ", FontHeight (font));
   x = PixelToPoint (x);
   y = PixelToPoint (y + h);
   l = PixelToPoint (l);
   h = PixelToPoint (h);
   fprintf (fout, "(\\325) %d -%d %d %d flyshow\n", x, y, l, h);
#else
   fprintf (fout, "(Symbol) %.0f sf\n", FontHeight (font) * 0.9);
   x = PixelToPoint (x + (l / 2));
   y = PixelToPoint (y + h - FontHeight (font) + FontBase (font));
   fprintf (fout, "-%d %d (\\325) c\n", y, x);
#endif
}				/*DrawPi */

/* ---------------------------------------------------------------------- */
/* |    DrawUnion trace un symbole Union.                                 | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                DrawUnion (int frame, int x, int y, int l, int h, ptrfont font, int RO, int func, int fg)
#else  /* __STDC__ */
void                DrawUnion (frame, x, y, l, h, font, RO, func, fg)
int                 frame;
int                 x;
int                 y;
int                 l;
int                 h;
ptrfont             font;
int                 RO;
int                 func;
int                 fg;

#endif /* __STDC__ */
{
   FILE               *fout;

   fout = (FILE *) FrRef[frame];
   DrawPage (fout);		/* Faut-il ajouter un showpage ? */

   /* Faut-il changer de RGB */
   CouleurCourante (fout, fg);

   /* On modifie la police courante */
   PolEnPs = NULL;
#ifdef JA
   fprintf (fout, "/Symbol-fly %d ", FontHeight (font));
   x = PixelToPoint (x);
   y = PixelToPoint (y + h);
   l = PixelToPoint (l);
   h = PixelToPoint (h);
   fprintf (fout, "(\\325) %d -%d %d %d flyshow\n", x, y, l, h);
#else
   fprintf (fout, "(Symbol) %.0f sf\n", FontHeight (font) * 0.9);
   x = PixelToPoint (x + (l / 2));
   y = PixelToPoint (y + h - FontHeight (font) + FontBase (font));
   fprintf (fout, "-%d %d (\\310) c\n", y, x);
#endif
}				/*DrawUnion */

/* ---------------------------------------------------------------------- */
/* |    DrawIntersection trace un symbole Intersection.                   | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                DrawIntersection (int frame, int x, int y, int l, int h, ptrfont font, int RO, int func, int fg)
#else  /* __STDC__ */
void                DrawIntersection (frame, x, y, l, h, font, RO, func, fg)
int                 frame;
int                 x;
int                 y;
int                 l;
int                 h;
ptrfont             font;
int                 RO;
int                 func;
int                 fg;

#endif /* __STDC__ */
{
   FILE               *fout;

   fout = (FILE *) FrRef[frame];
   DrawPage (fout);		/* Faut-il ajouter un showpage ? */

   /* Faut-il changer de RGB */
   CouleurCourante (fout, fg);

   /* On modifie la police courante */
   PolEnPs = NULL;
   fprintf (fout, "(Symbol) %.0f sf\n", FontHeight (font) * 0.9);
   x = PixelToPoint (x + (l / 2));
   y = PixelToPoint (y + h - FontHeight (font) + FontBase (font));
   fprintf (fout, "-%d %d (\\307) c\n", y, x);
}				/*DrawIntersection */


/* ---------------------------------------------------------------------- */
/* |    DrawArrow trace une fleche orientee en fonction de l'angle donne | */
/* |            (0 correspond a` une fleche vers la droite, 45, 90, 135,| */
/* |             180, 225, 270, 315).                                   | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                DrawArrow (int frame, int epais, int style, int x, int y, int l, int h, int orientation, int RO, int func, int fg)
#else  /* __STDC__ */
void                DrawArrow (frame, epais, style, x, y, l, h, orientation, RO, func, fg)
int                 frame;
int                 epais;
int                 style;
int                 x;
int                 y;
int                 l;
int                 h;
int                 orientation;
int                 RO;
int                 func;
int                 fg;

#endif /* __STDC__ */
{
   int                 xm, ym, xf, yf, lg;
   FILE               *fout;

   if (l == 0 && h == 0)
      return;			/* ce n'est pas la peine de se fatiguer */

   fout = (FILE *) FrRef[frame];
   DrawPage (fout);
   /* Faut-il ajouter un showpage ? */

   if (epais <= 0)
      return;

   /* Faut-il changer de RGB */
   CouleurCourante (fout, fg);

#ifdef JA
   fprintf (fout, "/Symbol-fly %d ", epais * 10);
   l = PixelToPoint (l);
   h = PixelToPoint (h);
#else
   l--;
   h--;
   xm = PixelToPoint (x + l / 2);
   xf = PixelToPoint (x + l);
   ym = PixelToPoint (y + h / 2);
   yf = PixelToPoint (y + h);
   lg = HL + epais;		/* longueur de la tete de fleche */
#endif
   x = PixelToPoint (x);
   y = PixelToPoint (y);

   if (orientation == 0)
     {
#ifdef JA
	fprintf (fout, "(\\256) %d -%d %d %d flyshow\n", x, yf, l, h);
#else
	/* Trace une fleche vers la droite */
	fprintf (fout, "%d -%d %d -%d %d %d %d Seg\n", x, ym, xf, ym, style, epais, 2);
	fprintf (fout, "%d %d -%d %d -%d %d %d %d arr\n", style, x, ym, xf, ym, epais, lg, lg);
#endif
     }
   else if (orientation == 45)
     {
	fprintf (fout, "%d -%d %d -%d %d %d %d Seg\n", x, yf, xf, y, style, epais, 2);
	fprintf (fout, "%d %d -%d %d -%d %d %d %d arr\n", style, x, yf, xf, y, epais, lg, lg);
     }
   else if (orientation == 90)
     {
#ifdef JA
	fprintf (fout, "(\\335) %d -%d %d %d flyshow\n", x, yf, l, h);
#else
	/* Trace une fleche vers le haut */
	fprintf (fout, "%d -%d %d -%d %d %d %d Seg\n", xm, yf, xm, y, style, epais, 2);
	fprintf (fout, "%d %d -%d %d -%d %d %d %d arr\n", style, xm, yf, xm, y, epais, lg, lg);
#endif
     }
   else if (orientation == 135)
     {
	fprintf (fout, "%d -%d %d -%d %d %d %d Seg\n", xf, yf, x, y, style, epais, 2);
	fprintf (fout, "%d %d -%d %d -%d %d %d %d arr\n", style, xf, yf, x, y, epais, lg, lg);
     }
   else if (orientation == 180)
     {
#ifdef JA
	fprintf (fout, "(\\254) %d -%d %d %d flyshow\n", x, yf, l, h);
#else
	/* Trace une fleche vers la gauche */
	fprintf (fout, "%d -%d %d -%d %d %d %d Seg\n", xf, ym, x, ym, style, epais, 2);
	fprintf (fout, "%d %d -%d %d -%d %d %d %d arr\n", style, xf, ym, x, ym, epais, lg, lg);
#endif
     }
   else if (orientation == 225)
     {
	fprintf (fout, "%d -%d %d -%d %d %d %d Seg\n", xf, y, x, yf, style, epais, 2);
	fprintf (fout, "%d %d -%d %d -%d %d %d %d arr\n", style, xf, y, x, yf, epais, lg, lg);
     }
   else if (orientation == 270)
     {
#ifdef JA
	fprintf (fout, "(\\257) %d -%d %d %d flyshow\n", x, yf, l, h);
#else
	/* Trace une fleche vers le bas */
	fprintf (fout, "%d -%d %d -%d %d %d %d Seg\n", xm, y, xm, yf, style, epais, 2);
	fprintf (fout, "%d %d -%d %d -%d %d %d %d arr\n", style, xm, y, xm, yf, epais, lg, lg);
#endif
     }
   else if (orientation == 315)
     {
	fprintf (fout, "%d -%d %d -%d %d %d %d Seg\n", x, y, xf, yf, style, epais, 2);
	fprintf (fout, "%d %d -%d %d -%d %d %d %d arr\n", style, x, y, xf, yf, epais, lg, lg);
     }
}				/*DrawArrow */

/* ---------------------------------------------------------------------- */
/* |    DrawBracket trace un symbole crochet ouvrant ou fermant.          | */
/* ---------------------------------------------------------------------- */
/**CO*/

#ifdef __STDC__
void                DrawBracket (int frame, int epais, int x, int y, int l, int h, int sens, ptrfont font, int RO, int func, int fg)

#else  /* __STDC__ */
void                DrawBracket (frame, epais, x, y, l, h, sens, font, RO, func, fg)
int                 frame;
int                 epais;
int                 x;
int                 y;
int                 l;
int                 h;
int                 sens;
ptrfont             font;
int                 RO;
int                 func;
int                 fg;

#endif /* __STDC__ */

{
   int                 ey, yf;
   FILE               *fout;

   fout = (FILE *) FrRef[frame];
   /* Faut-il ajouter un showpage ? */
   DrawPage (fout);

   /* Faut-il changer de RGB */
   CouleurCourante (fout, fg);

   /* Faut-il changer de police de caracteres courante */
   FontCourante (fout, font);

#ifdef JA
   fprintf (fout, "/Symbol-fly %d ", FontHeight (font));
   l = PixelToPoint (l);
   h = PixelToPoint (h);
   y = PixelToPoint (y);
   x = PixelToPoint (x);
   if (sens == 0)
      fprintf (fout, "(\\133) %d -%d %d %d flyshow\n", x, yf, l, h);
   else
      fprintf (fout, "(\\135) %d -%d %d %d flyshow\n", x, yf, l, h);
#else
   l--;
   h--;
   ey = FontHeight (font);
   h -= ey;
   y += FontBase (font);
   yf = PixelToPoint (y + h);
   y = PixelToPoint (y) + 1;
   x = PixelToPoint (x);

   if (h < ey / 4)
     {
	/* Sur un seul caractere */
	if (sens == 0)
	   fprintf (fout, "-%d %d ([) c\n", yf, x);
	else
	   fprintf (fout, "-%d %d (])c\n", yf, x);
     }
   else
     {
	/* Sur deux caracteres ou plus */
	if (sens == 0)		/* Trace un crochet ouvrant */
	   fprintf (fout, "%d -%d -%d %s (\\351) (\\352) (\\353) s3\n", x + 1, yf, y, Scale);
	else
	   fprintf (fout, "%d -%d -%d %s (\\371) (\\372) (\\373) s3\n", x, yf, y, Scale);
     }
#endif
}				/*DrawBracket */

/* ---------------------------------------------------------------------- */
/* |    DrawParenthesis trace un symbole parenthese ouvrant ou fermant.    | */
/* ---------------------------------------------------------------------- */
/**CO*/

#ifdef __STDC__
void                DrawParenthesis (int frame, int epais, int x, int y, int l, int h, int sens, ptrfont font, int RO, int func, int fg)

#else  /* __STDC__ */
void                DrawParenthesis (frame, epais, x, y, l, h, sens, font, RO, func, fg)
int                 frame;
int                 epais;
int                 x;
int                 y;
int                 l;
int                 h;
int                 sens;
ptrfont             font;
int                 RO;
int                 func;
int                 fg;

#endif /* __STDC__ */

{
   int                 ey, yf;
   FILE               *fout;

   fout = (FILE *) FrRef[frame];
   /* Faut-il ajouter un showpage ? */
   DrawPage (fout);

   /* Faut-il changer de RGB */
   CouleurCourante (fout, fg);

   /* Faut-il changer de police de caracteres courante */
   FontCourante (fout, font);

#ifdef JA
   fprintf (fout, "/Symbol-fly %d ", FontHeight (font));
   l = PixelToPoint (l);
   h = PixelToPoint (h);
   y = PixelToPoint (y);
   x = PixelToPoint (x);
   if (sens == 0)
      fprintf (fout, "(\\50) %d -%d %d %d flyshow\n", x, yf, l, h);
   else
      fprintf (fout, "(\\51) %d -%d %d %d flyshow\n", x, yf, l, h);
#else
   l--;
   h--;
   ey = FontHeight (font);
   h -= ey;
   y += FontBase (font);
   x = PixelToPoint (x);
   yf = PixelToPoint (y + h);
   y = PixelToPoint (y) + 1;

   if (h < ey / 4)
     {
	/* Sur un seul caractere */
	if (sens == 0)
	   fprintf (fout, "-%d %d (\\() c\n", yf, x);
	else
	   fprintf (fout, "-%d %d (\\)) c\n", yf, x);
     }
   else
     {
	/* Sur deux caracteres ou plus */
	if (sens == 0)
	   /* Trace un crochet ouvrant */
	   fprintf (fout, "%d -%d -%d %s (\\346) (\\347) (\\350) s3\n", x + 1, yf, y, Scale);
	else
	   fprintf (fout, "%d -%d -%d %s (\\366) (\\367) (\\370) s3\n", x, yf, y, Scale);
     }
#endif
}				/*DrawParenthesis */

/* ---------------------------------------------------------------------- */
/* |    DrawBrace trace un symbole accolade ouvrant ou fermant.        | */
/* ---------------------------------------------------------------------- */
/**CO*/

#ifdef __STDC__
void                DrawBrace (int frame, int epais, int x, int y, int l, int h, int sens, ptrfont font, int RO, int func, int fg)

#else  /* __STDC__ */
void                DrawBrace (frame, epais, x, y, l, h, sens, font, RO, func, fg)
int                 frame;
int                 epais;
int                 x;
int                 y;
int                 l;
int                 h;
int                 sens;
ptrfont             font;
int                 RO;
int                 func;
int                 fg;

#endif /* __STDC__ */

{
   int                 ey, yf;
   FILE               *fout;

   fout = (FILE *) FrRef[frame];
   /* Faut-il ajouter un showpage ? */
   DrawPage (fout);

   /* Faut-il changer de RGB */
   CouleurCourante (fout, fg);

   /* Faut-il changer de police de caracteres courante */
   FontCourante (fout, font);

#ifdef JA
   fprintf (fout, "/Symbol-fly %d ", FontHeight (font));
   l = PixelToPoint (l);
   h = PixelToPoint (h);
   y = PixelToPoint (y);
   x = PixelToPoint (x);
   if (sens == 0)
      fprintf (fout, "(\\173) %d -%d %d %d flyshow\n", x, yf, l, h);
   else
      fprintf (fout, "(\\175) %d -%d %d %d flyshow\n", x, yf, l, h);
#else
   l--;
   h--;
   ey = FontHeight (font);
   h -= ey;
   y += FontBase (font);
   x = PixelToPoint (x + (l / 2));
   yf = PixelToPoint (y + h);
   y = PixelToPoint (y) + 1;

   if (h < ey - 1)
     {
	/* Sur un seul caractere */
	if (sens == 0)
	   fprintf (fout, "-%d %d ({) c\n", yf, x);
	else
	   fprintf (fout, "-%d %d (}) c\n", yf, x);
     }
   else
     {
	/* Sur deux caracteres ou plus */
	if (sens == 0)
	   /* Trace un crochet ouvrant */
	   fprintf (fout, "%d -%d -%d %s (\\354) (\\355) (\\356) (\\357) s4\n", x, yf, y, Scale);
	else
	   fprintf (fout, "%d -%d -%d %s (\\374) (\\375) (\\376) (\\357) s4\n", x, yf, y, Scale);
     }
#endif
}				/*DrawBrace */

/* ---------------------------------------------------------------------- */
/* |    DrawRectangle trace un rectangle d'origine x, y et de dimensions  | */
/* |            larg, haut avec une e'paisseur epais dans la fenetree^tre       | */
/* |            d'indice frame.                                         | */
/* |            Les parame`tres fg, bg, motif indiquent la couleur du   | */
/* |            trace', la couleur du fond et le motif de remplissage.  | */
/* ---------------------------------------------------------------------- */
/**CO*/

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
   int                 xf, yf;
   FILE               *fout;

   fout = (FILE *) FrRef[frame];
   /* Faut-il ajouter un showpage ? */
   DrawPage (fout);

   /* Faut-il changer de RGB */
   if (epais > 0)
      CouleurCourante (fout, fg);

   xf = PixelToPoint (x + larg);
   yf = PixelToPoint (y + haut);
   x = PixelToPoint (x);
   y = PixelToPoint (y);

   Remplir (fout, fg, bg, motif);
   fprintf (fout, "%d -%d %d -%d %d -%d  %d -%d %d %d %d Poly\n", x, y, x, yf, xf, yf, xf, y, style, epais, 4);
}

/*debut */
/* ---------------------------------------------------------------------- */
/* |    DrawSegments trace des lignes brise'es.                            | */
/* |            Le parame`tre buffer pointe sur le 1er buffer qui       | */
/* |            contient la liste des points de contro^le et le         | */
/* |            parame`tre nb donne le nombre de points.                | */
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
   int                 i, j;
   float               xp, yp;
   int                 prevx, prevy;
   int                 lg;
   PtrTextBuffer      adbuff;
   FILE               *fout;

   fout = (FILE *) FrRef[frame];
   /* Faut-il ajouter un showpage ? */
   DrawPage (fout);

   if (epais == 0)
      return;

   lg = HL + epais;

   /* Faut-il changer de RGB */
   CouleurCourante (fout, fg);
   x = PixelToPoint (x);
   y = PixelToPoint (y);
   adbuff = buffer;

   /* fleche vers l'arriere  */
   if (fleche == 2 || fleche == 3)
      fprintf (fout, "%d %d -%d %d -%d %d %d %d arr\n", style,
	       FloatToInt ((float) buffer->BuPoints[2].XCoord / 1000 + x),
	       FloatToInt ((float) buffer->BuPoints[2].YCoord / 1000 + y),
	       FloatToInt ((float) buffer->BuPoints[1].XCoord / 1000 + x),
      FloatToInt ((float) buffer->BuPoints[1].YCoord / 1000 + y), epais, lg, lg);

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
	if (i == nb - 1)
	  {
	     /* conserve les derniers points pour tracer les fleches */
	     prevx = FloatToInt (xp);
	     prevy = FloatToInt (yp);
	  }
	/* Coordonnees d'un nouveau point */
	xp = (float) adbuff->BuPoints[j].XCoord / 1000 + x;
	yp = (float) adbuff->BuPoints[j].YCoord / 1000 + y;
	fprintf (fout, "%f -%f\n", xp, yp);
	j++;
     }
   /* Caracteristiques du trace */
   fprintf (fout, " %d %d %d Seg\n", style, epais, nb - 1);

   /* fleche vers l'avant  */
   j--;
   if (fleche == 1 || fleche == 3)
      fprintf (fout, "%d %d -%d %d -%d %d %d %d arr\n", style, prevx, prevy,
	       FloatToInt (xp), FloatToInt (yp), epais, lg, lg);
}

/* ---------------------------------------------------------------------- */
/* |    DrawPolygon trace un polygone.                                   | */
/* |            Le parame`tre buffer pointe sur le 1er buffer qui       | */
/* |            contient la liste des points de contro^le et le         | */
/* |            parame`tre nb donne le nombre de points.                | */
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
   int                 i, j;
   float               xp, yp;
   PtrTextBuffer      adbuff;
   FILE               *fout;

   fout = (FILE *) FrRef[frame];
   /* Faut-il ajouter un showpage ? */
   DrawPage (fout);

   /* Faut-il changer de RGB */
   CouleurCourante (fout, fg);
   Remplir (fout, fg, bg, motif);
   x = PixelToPoint (x);
   y = PixelToPoint (y);
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
	/* Coordonnees d'un nouveau point */
	xp = (float) adbuff->BuPoints[j].XCoord / 1000. + x;
	yp = (float) adbuff->BuPoints[j].YCoord / 1000. + y;
	fprintf (fout, "%f -%f\n", xp, yp);
	j++;
     }
   /* Caracteristiques du trace */
   fprintf (fout, "%d %d %d  Poly\n", style, epais, nb - 1);
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
/* |            Le parame`tre fg donne la couleur du trace'.            | */
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
   int                 lg;
   int                 lastx, lasty, newx, newy;
   float               x1, y1, x2, y2, x3, y3;
   FILE               *fout;

   fout = (FILE *) FrRef[frame];
   /* Faut-il ajouter un showpage ? */
   DrawPage (fout);
   if (epais == 0)
      return;

   /* Faut-il changer de RGB */
   CouleurCourante (fout, fg);

   lg = HL + epais;
   x = PixelToPoint (x);
   y = PixelToPoint (y);
   j = 1;
   i = 2;
   adbuff = buffer;
   lastx = adbuff->BuPoints[j].XCoord;
   lasty = adbuff->BuPoints[j].YCoord;
   j++;
   newx = adbuff->BuPoints[j].XCoord;
   newy = adbuff->BuPoints[j].YCoord;
   /* points de repere du premier arc de courbe */
   x1 = (float) lastx / 1000 + x;
   y1 = (float) lasty / 1000 + y;
   x2 = (float) (PixelToPoint ((int) (controls[i].lx * 3000)) + lastx) / 4000 + x;
   y2 = (float) (PixelToPoint ((int) (controls[i].ly * 3000)) + lasty) / 4000 + y;
   x3 = (float) (PixelToPoint ((int) (controls[i].lx * 3000)) + newx) / 4000 + x;
   y3 = (float) (PixelToPoint ((int) (controls[i].ly * 3000)) + newy) / 4000 + y;

   /* fleche vers l'arriere  */
   if (fleche == 2 || fleche == 3)
      fprintf (fout, "%d %d -%d %d -%d %d %d %d arr\n", style, FloatToInt (x2), FloatToInt (y2), FloatToInt (x1), FloatToInt (y1), epais, lg, lg);

   for (i = 2; i < nb; i++)
     {
	/* les 3 points pour tracer un arc de courbe */
	fprintf (fout, "%f -%f %f -%f %f -%f\n", x3, y3, x2, y2, x1, y1);
	/* on passe a l'arc de courbe suivant */
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
	lastx = newx;
	lasty = newy;
	newx = adbuff->BuPoints[j].XCoord;
	newy = adbuff->BuPoints[j].YCoord;
	x1 = (float) lastx / 1000 + x;
	y1 = (float) lasty / 1000 + y;
	if (i < nb - 2)
	  {
	     x2 = (float) PixelToPoint ((int) (controls[i].rx * 1000)) / 1000 + x;
	     y2 = (float) PixelToPoint ((int) (controls[i].ry * 1000)) / 1000 + y;
	     x3 = (float) PixelToPoint ((int) (controls[i + 1].lx * 1000)) / 1000 + x;
	     y3 = (float) PixelToPoint ((int) (controls[i + 1].ly * 1000)) / 1000 + y;
	  }
	else if (i == nb - 2)
	  {
	     x2 = (float) (PixelToPoint ((int) (controls[i].rx * 3000)) + lastx) / 4000 + x;
	     y2 = (float) (PixelToPoint ((int) (controls[i].ry * 3000)) + lasty) / 4000 + y;
	     x3 = (float) (PixelToPoint ((int) (controls[i].rx * 3000)) + newx) / 4000 + x;
	     y3 = (float) (PixelToPoint ((int) (controls[i].ry * 3000)) + newy) / 4000 + y;
	  }
     }
   fprintf (fout, "%f -%f %d %d %d Curv\n", x1, y1, style, epais, nb - 1);

   /* fleche vers l'avant */
   if (fleche == 1 || fleche == 3)
      fprintf (fout, "%d %d -%d %d -%d %d %d %d arr\n", style, FloatToInt (x3), FloatToInt (y3), FloatToInt (x1), FloatToInt (y1), epais, lg, lg);


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
   float               x0, y0, x1, y1, x2, y2, x3, y3;
   FILE               *fout;

   fout = (FILE *) FrRef[frame];
   /* Faut-il ajouter un showpage ? */
   DrawPage (fout);

   /* Faut-il changer de RGB */
   CouleurCourante (fout, fg);
   Remplir (fout, fg, bg, motif);
   x = PixelToPoint (x);
   y = PixelToPoint (y);
   j = 1;
   i = 1;
   adbuff = buffer;
   x0 = x1 = (float) adbuff->BuPoints[j].XCoord / 1000 + x;
   y0 = y1 = (float) adbuff->BuPoints[j].YCoord / 1000 + y;
   x2 = (float) PixelToPoint ((int) ((controls[i].rx * 1000))) / 1000 + x;
   y2 = (float) PixelToPoint ((int) ((controls[i].ry * 1000))) / 1000 + y;

   for (i = 2; i < nb; i++)
     {
	x3 = (float) PixelToPoint ((int) ((controls[i].lx * 1000))) / 1000 + x;
	y3 = (float) PixelToPoint ((int) ((controls[i].ly * 1000))) / 1000 + y;
	fprintf (fout, "%f -%f %f -%f %f -%f\n", x3, y3, x2, y2, x1, y1);
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
	x1 = (float) adbuff->BuPoints[j].XCoord / 1000 + x;
	y1 = (float) adbuff->BuPoints[j].YCoord / 1000 + y;
	x2 = (float) PixelToPoint ((int) ((controls[i].rx * 1000))) / 1000 + x;
	y2 = (float) PixelToPoint ((int) ((controls[i].ry * 1000))) / 1000 + y;
     }

   /* Ferme le contour */
   x3 = (float) PixelToPoint ((int) ((controls[1].lx * 1000))) / 1000 + x;
   y3 = (float) PixelToPoint ((int) ((controls[1].ly * 1000))) / 1000 + y;
   fprintf (fout, "%f -%f %f -%f %f -%f\n", x3, y3, x2, y2, x1, y1);
   fprintf (fout, "%f -%f %d %d %d Splin\n", x0, y0, style, epais, nb);

}
/*fin */

/* ---------------------------------------------------------------------- */
/* |    DrawDiamond trace un losange.                                     | */
/* |            Les parame`tres fg, bg, motif indiquent la couleur du   | */
/* |            trace', la couleur du fond et le motif de remplissage.  | */
/* ---------------------------------------------------------------------- */
/**CO*/

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
   int                 xm, xf, ym, yf;
   FILE               *fout;

   fout = (FILE *) FrRef[frame];
   /* Faut-il ajouter un showpage ? */
   DrawPage (fout);

   /* Faut-il changer de RGB */
   CouleurCourante (fout, fg);
   xm = PixelToPoint (x + larg / 2);
   ym = PixelToPoint (y + haut / 2);
   xf = PixelToPoint (x + larg);
   yf = PixelToPoint (y + haut);
   x = PixelToPoint (x);
   y = PixelToPoint (y);

   Remplir (fout, fg, bg, motif);
   fprintf (fout, "%d -%d %d -%d %d -%d %d -%d %d %d %d Poly\n", xm, y, x, ym, xm, yf, xf, ym, style, epais, 4);
}

/* ---------------------------------------------------------------------- */
/* |    DrawOval trace un rectangle aux bords arrondis.                  | */
/* |            Les parame`tres fg, bg, motif indiquent la couleur du   | */
/* |            trace', la couleur du fond et le motif de remplissage.  | */
/* ---------------------------------------------------------------------- */
/**CO*/

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
   int                 arc, xf, yf;
   FILE               *fout;

   fout = (FILE *) FrRef[frame];
   /* Faut-il ajouter un showpage ? */
   DrawPage (fout);

   /* Faut-il changer de RGB */
   if (epais > 0)
      CouleurCourante (fout, fg);

   arc = 3 * 72 / 25.4;
   xf = PixelToPoint (x + larg - 1);
   yf = PixelToPoint (y + haut - 1);
   x = PixelToPoint (x);
   y = PixelToPoint (y);

   Remplir (fout, fg, bg, motif);
   fprintf (fout, "%d %d %d -%d %d -%d %d -%d %d -%d %d -%d %d -%d %d ov\n",
	    style, epais,
	    /*5 */ x, y, /*4 */ x, yf, /*3 */ xf, yf, /*2 */ xf, y, /*1 */ x, y, /*o */ x, yf - arc,
	    arc);
}

/* ---------------------------------------------------------------------- */
/* |    DrawEllips trace une ellipse (cas particulier un cercle).        | */
/* |            Les parame`tres fg, bg, motif indiquent la couleur du   | */
/* |            trace', la couleur du fond et le motif de remplissage.  | */
/* ---------------------------------------------------------------------- */
/**CO*/

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
   int                 xm, ym;
   FILE               *fout;

   fout = (FILE *) FrRef[frame];
   DrawPage (fout);

   /* Faut-il changer de RGB */
   if (epais > 0)
      CouleurCourante (fout, fg);
   larg = larg / 2;
   haut = haut / 2;
   xm = PixelToPoint (x + larg);
   ym = PixelToPoint (y + haut);
   larg = PixelToPoint (larg);
   haut = PixelToPoint (haut);

   Remplir (fout, fg, bg, motif);
   if (larg == haut)
     {
	/* On trace un cercle */
	fprintf (fout, "%d %d %d -%d %d cer\n", style, epais, xm, ym, larg);
     }
   else
     {
	/* On trace une ellipse */
	fprintf (fout, "%d %d %d %d %d %d ellipse\n", style, epais,
		 xm, -ym, larg, haut);
     }
}

/* ---------------------------------------------------------------------- */
/* |    DrawCorner trace deux bords de rectangle. func indique s'il s'agit  | */
/* |            d'une boite active (1) ou non (0).                      | */
/* ---------------------------------------------------------------------- */
/**CO*/

#ifdef __STDC__
void                DrawCorner (int frame, int epais, int style, int x, int y, int l, int h, int coin, int RO, int func, int fg)

#else  /* __STDC__ */
void                DrawCorner (frame, epais, style, x, y, l, h, coin, RO, func, fg)
int                 frame;
int                 epais;
int                 style;
int                 x;
int                 y;
int                 l;
int                 h;
int                 coin;
int                 RO;
int                 func;
int                 fg;

#endif /* __STDC__ */

{
   int                 xf, yf;
   FILE               *fout;

   fout = (FILE *) FrRef[frame];
   DrawPage (fout);

   if (epais <= 0)
      return;

   /* Faut-il changer de RGB */
   CouleurCourante (fout, fg);
   xf = PixelToPoint (x + l);
   yf = PixelToPoint (y + h);
   x = PixelToPoint (x);
   y = PixelToPoint (y);

   switch (coin)
	 {
	    case 0:		/* Haut + Droite */
	       fprintf (fout, "%d -%d %d -%d %d -%d %d %d %d Seg\n", x, y, xf, y, xf, yf, style, epais, 3);
	       break;
	    case 1:		/* Droite + Bas */
	       fprintf (fout, "%d -%d %d -%d %d -%d %d %d %d Seg\n", xf, y, xf, yf, x, yf, style, epais, 3);
	       break;
	    case 2:		/* Bas + Gauche */
	       fprintf (fout, "%d -%d %d -%d %d -%d %d %d %d Seg\n", xf, yf, x, yf, x, y, style, epais, 3);
	       break;
	    case 3:		/* Gauche + Haut */
	       fprintf (fout, "%d -%d %d -%d %d -%d %d %d %d Seg\n", x, yf, x, y, xf, y, style, epais, 3);
	       break;
	 }
}

/* ---------------------------------------------------------------------- */
/* |    DrawRectangleFrame trace un rectangle a bords arrondis (diametre 3mm)  | */
/* |            avec un trait horizontal a 6mm du bord superieur.       | */
/* |            Les parame`tres fg, bg, motif indiquent la couleur du   | */
/* |            trace', la couleur du fond et le motif de remplissage.  | */
/* ---------------------------------------------------------------------- */
/**CO*/

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
   int                 arc, xf, yf;
   FILE               *fout;

   fout = (FILE *) FrRef[frame];
   DrawPage (fout);

   /* Faut-il changer de RGB */
   if (epais > 0)
      CouleurCourante (fout, fg);

   arc = 3 * 72 / 25.4;
   xf = PixelToPoint (x + larg - 1);
   yf = PixelToPoint (y + haut - 1);
   x = PixelToPoint (x);
   y = PixelToPoint (y);

   Remplir (fout, fg, bg, motif);
   fprintf (fout, "%d %d %d -%d %d -%d %d -%d %d -%d %d -%d %d -%d %d ov\n",
	  style, epais, x, y, x, yf, xf, yf, xf, y, x, y, x, yf - arc, arc);

   y += 2 * arc;
   fprintf (fout, "%d -%d %d -%d %d %d %d Seg\n", x, y, xf, y, style, epais, 2);
}

/* ---------------------------------------------------------------------- */
/* |    DrawEllipsFrame trace une ellipse avec trait horizontal a 7mm    | */
/* |            sous le sommet (pour SFGL).                             | */
/* |            Les parame`tres fg, bg, motif indiquent la couleur du   | */
/* |            trace', la couleur du fond et le motif de remplissage.  | */
/* ---------------------------------------------------------------------- */
/**CO*/

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
   int                 px7mm, shiftX;
   int                 xm, ym;
   FILE               *fout;
   double              A;

   fout = (FILE *) FrRef[frame];
   DrawPage (fout);

   /* Faut-il changer de RGB */
   if (epais > 0)
      CouleurCourante (fout, fg);

   larg = larg / 2;
   haut = haut / 2;
   xm = PixelToPoint (x + larg);
   ym = PixelToPoint (y + haut);
   larg = PixelToPoint (larg);
   haut = PixelToPoint (haut);

   Remplir (fout, fg, bg, motif);
   if (larg == haut)
     {
	/* On trace un cercle */
	fprintf (fout, "%d %d %d -%d %d cer\n", style, epais, xm, ym, larg);
     }
   else
     {
	/* On trace une ellipse */
	fprintf (fout, "%d %d %d %d %d %d ellipse\n", style, epais, xm, -ym, larg, haut);
     }
   px7mm = 7 * 72 / 25.4 + 0.5;
   if (haut > px7mm)
     {
	y = (ym - haut + px7mm);
	A = ((double) haut - px7mm) / haut;
	shiftX = larg * sqrt (1 - A * A) + 0.5;
	fprintf (fout, "%d -%d  %d -%d %d %d %d Seg\n",
		 xm - shiftX, y, xm + shiftX, y, style, epais, 2);
     }
}

/* ---------------------------------------------------------------------- */
/* |    DrawHorizontalLine trace une horizontale sur le bord superieur, au milieu  | */
/* |            ou sur le bord inferieur.                               | */
/* ---------------------------------------------------------------------- */
/**CO*/

#ifdef __STDC__
void                DrawHorizontalLine (int frame, int epais, int style, int x, int y, int l, int h, int cadrage, int RO, int func, int fg)

#else  /* __STDC__ */
void                DrawHorizontalLine (frame, epais, style, x, y, l, h, cadrage, RO, func, fg)
int                 frame;
int                 epais;
int                 style;
int                 x;
int                 y;
int                 l;
int                 h;
int                 cadrage;
int                 RO;
int                 func;
int                 fg;

#endif /* __STDC__ */

{
   int                 ym, yf, xf;
   FILE               *fout;

   fout = (FILE *) FrRef[frame];
   /* Faut-il ajouter un showpage ? */
   DrawPage (fout);

   if (epais <= 0)
      return;

   /* Faut-il changer de RGB */
   CouleurCourante (fout, fg);

   l--;
   h--;
   xf = PixelToPoint (x + l);
   ym = PixelToPoint (y + h / 2);
   yf = PixelToPoint (y + h);
   x = PixelToPoint (x);
   y = PixelToPoint (y);

   if (cadrage == 0)
      fprintf (fout, "%d -%d %d -%d %d %d %d Seg\n", xf, y, x, y, style, epais, 2);
   else if (cadrage == 1)
      fprintf (fout, "%d -%d %d -%d %d %d %d Seg\n", xf, ym, x, ym, style, epais, 2);
   else
      fprintf (fout, "%d -%d %d -%d %d %d %d Seg\n", xf, yf, x, yf, style, epais, 2);
}				/*DrawHorizontalLine */

/* ---------------------------------------------------------------------- */
/* |    DrawVerticalLine trace une verticale sur le bord gauche, au milieu ou sur| */
/* |            le bords droit.                                         | */
/* ---------------------------------------------------------------------- */
/**CO*/

#ifdef __STDC__
void                DrawVerticalLine (int frame, int epais, int style, int x, int y, int l, int h, int cadrage, int RO, int func, int fg)

#else  /* __STDC__ */
void                DrawVerticalLine (frame, epais, style, x, y, l, h, cadrage, RO, func, fg)
int                 frame;
int                 epais;
int                 style;
int                 x;
int                 y;
int                 l;
int                 h;
int                 cadrage;
int                 RO;
int                 func;
int                 fg;

#endif /* __STDC__ */

{
   int                 xm, yf, xf;
   FILE               *fout;

   if (epais <= 0)
      return;

   fout = (FILE *) FrRef[frame];
   /* Faut-il ajouter un showpage ? */
   DrawPage (fout);

   /* Faut-il changer de RGB */
   CouleurCourante (fout, fg);

   l--;
   h--;
   xf = PixelToPoint (x + l);
   xm = PixelToPoint (x + l / 2);
   yf = PixelToPoint (y + h);
   x = PixelToPoint (x);
   y = PixelToPoint (y);

   if (cadrage == 0)
      fprintf (fout, "%d -%d %d -%d %d %d %d Seg\n", x, yf, x, y, style, epais, 2);
   else if (cadrage == 1)
      fprintf (fout, "%d -%d %d -%d %d %d %d Seg\n", xm, yf, xm, y, style, epais, 2);
   else
      fprintf (fout, "%d -%d %d -%d %d %d %d Seg\n", xf, yf, xf, y, style, epais, 2);
}				/*DrawVerticalLine */

/* ---------------------------------------------------------------------- */
/* |    DrawPoints trace un ligne de pointilles sur la longueur donnee.   | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                DrawPoints (int frame, int x, int y, int lgboite, int RO, int func, int fg)
#else  /* __STDC__ */
void                DrawPoints (frame, x, y, lgboite, RO, func, fg)
int                 frame;
int                 x;
int                 y;
int                 lgboite;
int                 RO;
int                 func;
int                 fg;

#endif /* __STDC__ */
{
   int                 xcour, ycour;	/*codage */
   FILE               *fout;
   ptrfont             font;

   /* Faut-il changer de RGB */
   CouleurCourante (fout, fg);

   font = (ptrfont) ThotLoadFont ('L', 't', 0, 6, UnPoint, frame);
   if (lgboite > 0)
     {
	fout = (FILE *) FrRef[frame];
	/* On charge la fonte courante */
	/*codage = FontCourante(fout,font); */
	xcour = PixelToPoint (x);
	ycour = PixelToPoint (y);

	fprintf (fout, "%d -%d %d Pes\n", xcour, ycour, PixelToPoint (lgboite));
     }
}				/*DrawPoints */

/* ---------------------------------------------------------------------- */
/* |    DrawSlash trace une diagonale dans le sens precise'.            | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                DrawSlash (int frame, int epais, int style, int x, int y, int l, int h, int sens, int RO, int func, int fg)
#else  /* __STDC__ */
void                DrawSlash (frame, epais, style, x, y, l, h, sens, RO, func, fg)
int                 frame;
int                 epais;
int                 style;
int                 x;
int                 y;
int                 l;
int                 h;
int                 sens;
int                 RO;
int                 func;
int                 fg;

#endif /* __STDC__ */
{
   int                 xf, yf;
   FILE               *fout;

   fout = (FILE *) FrRef[frame];
   /* Faut-il ajouter un showpage ? */
   DrawPage (fout);

   if (epais <= 0)
      return;

   /* Faut-il changer de RGB */
   CouleurCourante (fout, fg);

   l--;
   h--;
   xf = PixelToPoint (x + l);
   yf = PixelToPoint (y + h);
   x = PixelToPoint (x);
   y = PixelToPoint (y);

   if (sens == 0)
      fprintf (fout, "%d -%d %d -%d %d %d %d Seg\n", x, yf, xf, y, style, epais, 2);
   else
      fprintf (fout, "%d -%d %d -%d %d %d %d Seg\n", x, y, xf, yf, style, epais, 2);
}				/*DrawSlash */

/* ---------------------------------------------------------------------- */
/* |    PSPageInfo stocke les numeros, largeur et hauteur de la page    | */
/* |            que DrawPage utilisera                                    | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                PSPageInfo (int pagenum, int width, int height)
#else  /* __STDC__ */
void                PSPageInfo (pagenum, width, height)
int                 pagenum;
int                 width;
int                 height;

#endif /* __STDC__ */
{
   LastPageNumber = pagenum;
   LastPageWidth = width;
   LastPageHeight = height;
}

/* ---------------------------------------------------------------------- */
/* |    psBoundingBox sort un commentaire PostScript utilise' par le    | */
/* |            filtre de de'coupage special Grands Graphiques.         | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                psBoundingBox (int frame, int width, int height)
#else  /* __STDC__ */
void                psBoundingBox (frame, width, height)
int                 frame;
int                 width;
int                 height;

#endif /* __STDC__ */
{
   FILE               *fout;

   fout = (FILE *) FrRef[frame];
   /* Compte tenu du fait que l'origine des coordonnees PostScript */
   /* correspond au coin bas-gauche de la page, qu'une hauteur de  */
   /* page normale represente 2970 mm (soit 2970*72/254 = 841 pts) */
   /* que Thot ajoute une marge de 50 pts en haut et a gauche de   */
   /* l'image produite, la boundingBox est egale a :              */
   fprintf (fout, "%%%%BoundingBox: %d %d %d %d\n",
	    50, 791 - PixelToPoint (height),
	    50 + PixelToPoint (width), 791);
}

/* ---------------------------------------------------------------------- */
/* |    EndOfString teste si la chaine chaine se termine par suffix.    | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
int                 EndOfString (char *chaine, char *suffix)
#else  /* __STDC__ */
int                 EndOfString (chaine, suffix)
char               *chaine;
char               *suffix;

#endif /* __STDC__ */
{
   int                 long_chaine, long_suf;

   long_chaine = strlen (chaine);
   long_suf = strlen (suffix);
   return (strcmp (chaine + long_chaine - long_suf, suffix) == 0);
}


/* ---------------------------------------------------------------------- */
/* |    Trame remplit le rectangle de la fenetree^tre w ou d'indice frame       | */
/* |            (si w=0) de'fini par x, y, large, haut avec le motif    | */
/* |            donne'.                                                 | */
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
   int                 xf, yf;
   FILE               *fout;

   fout = (FILE *) FrRef[frame];
   /* Faut-il ajouter un showpage ? */
   DrawPage (fout);

   /* Faut-il changer de RGB */
   CouleurCourante (fout, fg);

   if (motif >= 0)
     {
	xf = PixelToPoint (x + large - 1);
	yf = PixelToPoint (y + haut - 1);
	x = PixelToPoint (x);
	y = PixelToPoint (y);
	fprintf (fout, "%d %d -%d %d -%d %d -%d %d -%d trm\n", motif, x, yf, xf, yf, xf, y, x, y);
     }
}				/*Trame */
