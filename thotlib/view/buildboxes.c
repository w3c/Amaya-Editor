
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */


/* 
   img.c : gestion des Images abstraites
   I. Vatton - Aout 86
   IV : Aout 92 dimensions minimales
   IV : Aout 92 coupure des mots
   IV : Juin 93 polylines
 */

#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "imagedrvr.h"
#include "language.h"
#include "libmsg.h"
#include "message.h"

#define EXPORT
#include "img.var"
#undef EXPORT
#define EXPORT extern
#include "font.var"

#include "appli.f"
#include "cmd.f"
#include "dep.f"
#include "des.f"
#include "font.f"
#include "imagedrvr.f"
#include "lig.f"
#include "memory.f"
#include "pos.f"
#include "rel.f"
#include "sel.f"
#include "select.f"
#include "textelem.f"
#include "visu.f"

#ifdef WWW_MSWINDOWS		/* map to MSVC library system calls */
/* #include <d:\msdev\include\math.h> */
#include <math.h>
#define M_PI        3.14159265358979323846	/* pi from linux math.h */
#define M_PI_2      1.57079632679489661923	/* pi/2h */
#endif /* WWW_MSWINDOWS */

/*debut */
#define		_2xPI		6.2832
#define		_1dSQR2		0.7071
#define		_SQR2		1.4142

#ifdef __STDC__
extern void         DefClip (int, int, int, int, int);
extern boolean      AfFinFenetre (int, int);
extern void         EndInsert (void);

#else
extern void         DefClip ();
extern boolean      AfFinFenetre ();
extern void         EndInsert ();

#endif

/* ---------------------------------------------------------------------- */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         control_points (float x, float y, float l1, float l2, float theta1, float theta2, C_points * cp)
#else  /* __STDC__ */
static void         control_points (x, y, l1, l2, theta1, theta2, cp)
float               x, y, l1, l2, theta1, theta2;
C_points           *cp;

#endif /* __STDC__ */
{
   float               s, theta, r;

   r = 1 - 0.45;
   /* 0 <= theta1, theta2 < 2PI */
   theta = (theta1 + theta2) / 2;

   if (theta1 > theta2)
     {
	s = sin ((double) (theta - theta2));
	theta1 = theta + M_PI_2;
	theta2 = theta - M_PI_2;
     }
   else
     {
	s = sin ((double) (theta2 - theta));
	theta1 = theta - M_PI_2;
	theta2 = theta + M_PI_2;
     }

   if (s > _1dSQR2)
      s = _SQR2 - s;
   s *= r;
   l1 *= s;
   l2 *= s;
   cp->lx = x + l1 * cos ((double) theta1);
   cp->ly = y - l1 * sin ((double) theta1);
   cp->rx = x + l2 * cos ((double) theta2);
   cp->ry = y - l2 * sin ((double) theta2);
}


/* ---------------------------------------------------------------------- */
/* |    PointsControle calcule les points de controle de la courbe.     | */
/* |            Le parametre nb contient le nombre de points + 1        | */
/* |            definis dans la polyline.                               | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
C_points           *PointsControle (PtrTextBuffer buffer, int nb)
#else  /* __STDC__ */
C_points           *PointsControle (buffer, nb)
PtrTextBuffer      buffer;
int                 nb;

#endif /* __STDC__ */
{
   C_points           *controls;
   PtrTextBuffer      adbuff;
   int                 i, j;
   float               dx, dy;
   float               x, y, x1, y1, x2, y2, x3, y3;
   float               l1, l2, theta1, theta2;

   /* alloue la liste des points de controle */
   controls = (C_points *) TtaGetMemory (sizeof (C_points) * nb);

   adbuff = buffer;
   j = 1;
   x1 = (float) PtEnPixel (adbuff->BuPoints[j].XCoord / 1000, 0);
   y1 = (float) PtEnPixel (adbuff->BuPoints[j].YCoord / 1000, 1);
   j++;
   x2 = (float) PtEnPixel (adbuff->BuPoints[j].XCoord / 1000, 0);
   y2 = (float) PtEnPixel (adbuff->BuPoints[j].YCoord / 1000, 1);
   if (nb < 3)
     {
	/* cas particulier des courbes avec 2 points */
	x3 = x2;
	y3 = y2;
     }
   else
     {
	j++;
	x3 = (float) PtEnPixel (adbuff->BuPoints[j].XCoord / 1000, 0);
	y3 = (float) PtEnPixel (adbuff->BuPoints[j].YCoord / 1000, 1);
     }

   dx = x1 - x2;
   dy = y2 - y1;
   l1 = sqrt ((double) (dx * dx + dy * dy));
   if (l1 == 0.0)
      theta1 = 0.0;
   else
      theta1 = atan2 ((double) dy, (double) dx);

   dx = x3 - x2;
   dy = y2 - y3;
   l2 = sqrt ((double) (dx * dx + dy * dy));
   if (l2 == 0.0)
      theta2 = 0.0;
   else
      theta2 = atan2 ((double) dy, (double) dx);

   /* -PI <= theta1, theta2 <= PI */
   /* 0 <= theta1, theta2 < 2PI */
   if (theta1 < 0)
      theta1 += _2xPI;
   if (theta2 < 0)
      theta2 += _2xPI;
   control_points (x2, y2, l1, l2, theta1, theta2, &controls[2]);

   nb--;			/* dernier point */
   x = x2;			/* memorise les points pour fermer la courbe */
   y = y2;
   for (i = 3; i <= nb; i++)
     {
	x2 = x3;
	y2 = y3;
	l1 = l2;
	if (theta2 >= M_PI)
	   theta1 = theta2 - M_PI;
	else
	   theta1 = theta2 + M_PI;

	if (i == nb)
	  {
	     /* Traitement du dernier point */
	     x3 = x1;
	     y3 = y1;
	  }
	else
	  {
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
	     x3 = (float) PtEnPixel (adbuff->BuPoints[j].XCoord / 1000, 0);
	     y3 = (float) PtEnPixel (adbuff->BuPoints[j].YCoord / 1000, 1);
	  }
	dx = x3 - x2;
	dy = y2 - y3;
	l2 = sqrt ((double) (dx * dx + dy * dy));
	if (l2 == 0.0)
	   theta2 = 0.0;
	else
	   theta2 = atan2 ((double) dy, (double) dx);
	if (theta2 < 0)
	   theta2 += _2xPI;
	control_points (x2, y2, l1, l2, theta1, theta2, &controls[i]);
     }

   /* Traitement du 1er point */
   x2 = x3;
   y2 = y3;
   l1 = l2;
   if (theta2 >= M_PI)
      theta1 = theta2 - M_PI;
   else
      theta1 = theta2 + M_PI;

   dx = x - x2;
   dy = y2 - y;
   l2 = sqrt ((double) (dx * dx + dy * dy));
   if (l2 == 0.0)
      theta2 = 0.0;
   else
      theta2 = atan2 ((double) dy, (double) dx);
   if (theta2 < 0)
      theta2 += _2xPI;
   control_points (x2, y2, l1, l2, theta1, theta2, &controls[1]);

   return (controls);
}
/*fin */

/* ---------------------------------------------------------------------- */
/* |    DimTexte calcule les dimensions : hauteur, base, largeur et     | */
/* |            nombre de blancs contenus (carbl).                      | */
/* |            Attention : large contient initialement 0 si la largeur | */
/* |            du blanc est celle de la fonte sinon la valeur imposee. | */
/* |            carbl contient initialement l'index du premier          | */
/* |            caractere du texte.                                     | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                DimTexte (PtrTextBuffer adbuff, int nbcar, ptrfont font, int *large, int *carbl)

#else  /* __STDC__ */
void                DimTexte (adbuff, nbcar, font, large, carbl)
PtrTextBuffer      adbuff;
int                 nbcar;
ptrfont             font;
int                *large;
int                *carbl;

#endif /* __STDC__ */

{
   int                 i, j;
   unsigned char       car;
   int                 lgcar;
   int                 lgbl;

   /* Calcule la largeur des blancs */
   if (*large == 0)
      lgbl = CarWidth (BLANC, font);
   else
      lgbl = *large;
   i = *carbl;			/* Index dans le buffer */
   *carbl = 0;
   *large = 0;

   j = 1;
   while (j <= nbcar)
     {
	/* On traite les differents caracteres */
	car = (unsigned char) (adbuff->BuContent[i - 1]);
	if (car == BLANC)
	  {
	     (*carbl)++;	/* caractere blanc */
	     lgcar = lgbl;
	  }
	else
	   lgcar = CarWidth (car, font);
	*large += lgcar;
	/* Caractere suivant */
	if (i >= adbuff->BuLength)
	  {
	     adbuff = adbuff->BuNext;
	     if (adbuff == NULL)
		j = nbcar;
	     i = 1;
	  }
	else
	   i++;
	j++;
     }
}				/* DimTexte */


/* ---------------------------------------------------------------------- */
/* |    EvalPic evalue les dimensions de la boite du pave Picture.        | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void         EvalPic (PtrAbstractBox adpave, int *large, int *haut)

#else  /* __STDC__ */
static void         EvalPic (adpave, large, haut)
PtrAbstractBox             adpave;
int                *large;
int                *haut;

#endif /* __STDC__ */

{
   ptrfont             font;
   PtrBox            pBo1;
   ImageDescriptor    *image;

   pBo1 = adpave->AbBox;
   image = (ImageDescriptor *) pBo1->BxImageDescriptor;
/***todo: revoir la condition suivante... definition d'une image vide */
   if (adpave->AbVolume == 0 || image == NULL)
     {
	font = pBo1->BxFont;
	*large = CarWidth (109, font);	/*'m' */
	*haut = FontHeight (font);
     }
   else
     {
	*large = image->wcf;
	*haut = image->hcf;
     }
}				/* EvalPic */


/* ---------------------------------------------------------------------- */
/* |    EvalSymb evalue les dimensions de la boite du pave Symbol.     | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                EvalSymb (PtrAbstractBox adpave, int *large, int *haut)

#else  /* __STDC__ */
void                EvalSymb (adpave, large, haut)
PtrAbstractBox             adpave;
int                *large;
int                *haut;

#endif /* __STDC__ */

{
   ptrfont             font;
   int                 hfont;
   float               valeur;

   font = adpave->AbBox->BxFont;
   hfont = FontHeight (font);
   if (adpave->AbVolume == 0)
     {
	/* Symbol vide */
	*large = CarWidth (BLANC, font);
	*haut = hfont * 2;
     }
   else
     {
	*haut = hfont * 2;
	valeur = 1 + ((float) (adpave->AbBox->BxHeight * 0.6) / (float) hfont);
	switch (adpave->AbShape)
	      {
		 case 'c':	/*integrale curviligne */
		 case 'i':	/*integrale */
		    *large = (int) ((float) (CarWidth (231, font)) * valeur);
		    *haut += hfont;
		    break;
		 case 'd':	/*integrale double */
		    *large = CarWidth (231, font) + CarWidth (231, font) / 2;
		    *haut += hfont;
		    break;
		 case 'r':	/*racine */
		    *large = hfont;
		    break;
		 case 'S':	/*sigma */
		 case 'P':	/*pi */
		 case 'I':	/*intersection */
		 case 'U':	/*union */
		    /* large := CarWidth(229, font) + 4; */
		    *large = CarWidth (229, font);
		    /* haut := FontHeight(font); */
		    *haut = hfont;
		    break;
		 case '<':
		 case 'h':
		 case '>':
		    *large = *haut;
		    *haut = hfont;
		    break;
		 case '(':
		 case ')':
		 case '[':
		 case ']':
		 case '{':
		 case '}':
		    *large = (int) ((float) CarWidth (230, font) * valeur);
		    *haut = hfont;
		    break;
		 case '^':
		 case 'v':
		 case 'V':
		    *large = CarWidth (109, font);	/*'m' */
		    break;
		 default:
		    *large = CarWidth (BLANC, font);
		    *haut = hfont;
		    break;
	      }
     }
}				/* EvalSymb */


/* ---------------------------------------------------------------------- */
/* |    EvalGraph evalue les dimensions de la boite du pave Graphique.  | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                EvalGraph (PtrAbstractBox adpave, int *large, int *haut)

#else  /* __STDC__ */
void                EvalGraph (adpave, large, haut)
PtrAbstractBox             adpave;
int                *large;
int                *haut;

#endif /* __STDC__ */

{
   ptrfont             font;
   int                 hfont;

   font = adpave->AbBox->BxFont;
   *large = CarWidth (109, font);	/*'m' */
   hfont = FontHeight (font);
   *haut = hfont * 2;
   switch (adpave->AbShape)
	 {
	    case '<':
	    case 'h':
	    case '>':
	    case 't':
	    case 'b':
	       *large = *haut;
	       *haut = hfont;
	       break;
	    case 'C':
	    case 'c':
	    case 'L':
	    case '/':
	    case '\\':
	    case 'O':
	    case 'o':
	    case 'E':
	    case 'e':
	       *large = *haut;
	       break;
	    default:
	       break;
	 }
}				/* EvalGraph */

/*debut */
/* ---------------------------------------------------------------------- */
/* |    EvalPolyLine evalue les dimensions de la boite du PolyLine.     | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                EvalPolyLine (PtrAbstractBox adpave, int *large, int *haut)

#else  /* __STDC__ */
void                EvalPolyLine (adpave, large, haut)
PtrAbstractBox             adpave;
int                *large;
int                *haut;

#endif /* __STDC__ */

{
   int                 max;
   PtrTextBuffer      adbuff;

   /* Si le pave est vide on prend une dimension par defaut */
   adbuff = adpave->AbPolyLineBuffer;
   *large = 1;
   *haut = 1;
   max = adpave->AbVolume;
   if (max > 0 || adbuff != NULL)
     {
	/* La largeur est donnee par le point limite */
	*large = adbuff->BuPoints[0].XCoord;
	/* La hauteur est donnee par le point limite */
	*haut = adbuff->BuPoints[0].YCoord;

	/* Convertit en pixels */
	*large = PtEnPixel (*large / 1000, 0);
	*haut = PtEnPixel (*haut / 1000, 1);
     }
}				/* EvalPolyLine */
/*fin */

/* ---------------------------------------------------------------------- */
/* |    EvalText evalue les dimensions de la boite du pave Texte.       | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                EvalText (PtrAbstractBox adpave, int *large, int *haut, int *carbl)

#else  /* __STDC__ */
void                EvalText (adpave, large, haut, carbl)
PtrAbstractBox             adpave;
int                *large;
int                *haut;
int                *carbl;

#endif /* __STDC__ */

{
   ptrfont             font;
   int                 taille;

   font = adpave->AbBox->BxFont;
   *haut = FontHeight (font);

   /* Est-ce que le pave est vide ? */
   taille = adpave->AbVolume;
   if (taille == 0)
     {
	*large = CarWidth (109, font);	/*'m' */
	*carbl = 0;
     }
   else
     {
	/* Texte -> Calcule directement ses dimensions */
	*carbl = 1;		/* Index du premier caractere a traiter */
	*large = 0;		/* On prend la largeur reelle du blanc */
	DimTexte (adpave->AbText, taille, font, large, carbl);
     }
}				/* EvalText */


/* ---------------------------------------------------------------------- */
/* |    EvalComp e'value les dimensions du contenu du pave' compose'    | */
/* |            adpave dans la fenetree^tre frame.                              | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                EvalComp (PtrAbstractBox adpave, int frame, int *large, int *haut)

#else  /* __STDC__ */
void                EvalComp (adpave, frame, large, haut)
PtrAbstractBox             adpave;
int                 frame;
int                *large;
int                *haut;

#endif /* __STDC__ */

{
   int                 val, x, y;
   boolean             ajustif;
   boolean             encore;
   PtrAbstractBox             pavefils;
   PtrAbstractBox             premfils;
   PtrAbstractBox             lepave;
   PtrBox            ibox, box1;
   PtrLine            adligne;
   PtrBox            pBo1;

   boolean             absoluEnX;
   boolean             absoluEnY;

   box1 = NULL;
   pBo1 = adpave->AbBox;
   /* PcFirst fils vivant */
   premfils = adpave->AbFirstEnclosed;
   encore = True;
   while (encore)
      if (premfils == NULL)
	 encore = False;
      else if (premfils->AbDead)
	 premfils = premfils->AbNext;
      else
	 encore = False;

   /* Il est inutile de calculer les dimensions d'une boite eclatee */
   if (pBo1->BxType == BoGhost)
     {
	*large = 0;
	*haut = 0;
	/* Si la regle secable a supplante la regle de mise en lignes */
	if (adpave->AbInLine)
	  {
	     pBo1->BxFirstLine = NULL;
	     pBo1->BxLastLine = NULL;
	  }
     }
   /* La boite composee est vide */
   else if (premfils == NULL)
     {
	*large = 0;
	*haut = 0;
	if (adpave->AbInLine)
	  {
	     pBo1->BxType = BoBlock;
	     pBo1->BxFirstLine = NULL;
	     pBo1->BxLastLine = NULL;
	  }
     }
   else if (adpave->AbInLine)
      /* La boite composee est mise en ligne */
     {
	pBo1->BxType = BoBlock;
	if (pBo1->BxContentWidth)
	  {
	     /* On recherche la premiere boite a mettre en ligne */
	     encore = True;
	     while (encore)
		/* Est-ce que le pave est mort ? */
		if (premfils->AbDead)
		  {
		     box1 = Suivante (premfils);
		     encore = False;
		  }
		else if (premfils->AbBox->BxType == BoGhost)
		   /* On descend dans la hierarchie */
		   premfils = premfils->AbFirstEnclosed;
	     /* Sinon c'est la boite du pave */
		else
		  {
		     box1 = premfils->AbBox;
		     encore = False;
		  }

	     if (box1 != NULL)
	       {
		  /* Il y a au moins une boite non vide */
		  GetLine (&adligne);
		  adligne->LiFirstBox = box1;
		  adligne->LiXMax = 3000;	/* Dimension maximale possible */
#ifdef __COLPAGE__
		  RemplirLigne (adligne, FntrTable[frame - 1].FrAbstractBox, adpave->AbTruncatedTail, &encore, &ajustif);
#else  /* __COLPAGE__ */
		  RemplirLigne (adligne, FntrTable[frame - 1].FrAbstractBox, &encore, &ajustif);
#endif /* __COLPAGE__ */
		  adligne->LiXMax = adligne->LiRealLength;

		  /* evalue si le positionnement en X et en Y doit etre absolu */
		  XYEnAbsolu (pBo1, &absoluEnX, &absoluEnY);

		  Aligner (pBo1, adligne, 0, frame, absoluEnX, absoluEnY);
		  *large = adligne->LiXMax;
		  *haut = adligne->LiHeight;
	       }
	     else
	       {
		  *large = 0;
		  *haut = 0;
		  adligne = NULL;
	       }

	     pBo1->BxFirstLine = adligne;
	     pBo1->BxLastLine = adligne;
	  }
	else
	  {
	     pBo1->BxFirstLine = NULL;
	     pBo1->BxLastLine = NULL;
	     BlocDeLigne (pBo1, frame, haut);
	     /* Si la largeur du contenu depasse le minimum */
	     if (!adpave->AbWidth.DimIsPosition && adpave->AbWidth.DimMinimum
		 && pBo1->BxFirstLine != pBo1->BxLastLine)
	       {
		  /* Il faut prendre la largeur du contenu */
		  pBo1->BxContentWidth = True;
		  ReevalBloc (adpave, NULL, NULL, frame);
	       }
	     else
		*large = pBo1->BxWidth;
	  }
     }				/* FnLine */
   /* La boite est une composition geometrique */
   else
     {
	x = pBo1->BxXOrg;
	*large = x;
	y = pBo1->BxYOrg;
	*haut = y;
	/* On verifie l'englobement s'il est impose */
	pavefils = premfils;
	while (pavefils != NULL)
	  {
	     ibox = pavefils->AbBox;
	     if (!pavefils->AbDead && ibox != NULL)
	       {
		  /* Faut-il mettre a jour la position du pave ? */
		  if (pBo1->BxContentWidth && pavefils->AbHorizEnclosing && ibox->BxXOrg < x
		      && pavefils->AbWidth.DimAbRef != adpave)
		    {
		       /* Est-ce que la boite est mobile ? */
		       box1 = BoiteHInclus (ibox, NULL);
		       if (box1 != NULL)
			 {
			    lepave = box1->BxAbstractBox;
			    if (lepave->AbHorizPos.PosAbRef == NULL)
			       DepOrgX (ibox, NULL, x - ibox->BxXOrg, frame);
			 }
		    }
		  /* Faut-il mettre a jour la position du pave ? */
		  if (pBo1->BxContentHeight && pavefils->AbVertEnclosing && ibox->BxYOrg < y
		      && pavefils->AbHeight.DimAbRef != adpave)
		    {
		       /* Est-ce que la boite est mobile ? */
		       box1 = BoiteVInclus (ibox, NULL);
		       if (box1 != NULL)
			 {
			    lepave = box1->BxAbstractBox;
			    if (lepave->AbVertPos.PosAbRef == NULL)
			       DepOrgY (ibox, NULL, y - ibox->BxYOrg, frame);
			 }
		    }
	       }
	     pavefils = pavefils->AbNext;
	  }
	/* On evalue les dimensions reelles de la boite composee */
	pavefils = premfils;
	while (pavefils != NULL)
	  {
	     ibox = pavefils->AbBox;
	     if (!pavefils->AbDead && ibox != NULL)
	       {
		  /* La largeur de la boite composee peut dependre du pave ? */
		  if (pavefils->AbWidth.DimAbRef != adpave && pavefils->AbHorizEnclosing)
		    {
		       if (ibox->BxXOrg < 0)
			  val = ibox->BxWidth;
		       else
			  val = ibox->BxXOrg + ibox->BxWidth;
		       if (val > *large)
			  *large = val;
		    }
		  /* La hauteur de la boite composee peut dependre du pave ? */
		  if (pavefils->AbHeight.DimAbRef != adpave && pavefils->AbVertEnclosing)
		    {
		       if (ibox->BxYOrg < 0)
			  val = ibox->BxHeight;
		       else
			  val = ibox->BxYOrg + ibox->BxHeight;
		       if (val > *haut)
			  *haut = val;
		    }
	       }
	     pavefils = pavefils->AbNext;
	  }
	*large -= x;
	*haut -= y;
	/* Decale les boites incluses dont la position depend des dimensions */
	pavefils = premfils;
	if (Propage == ToSiblings)
	   while (pavefils != NULL)
	     {
		ibox = pavefils->AbBox;
		if (!pavefils->AbDead && ibox != NULL)
		  {
		     /* La position horizontale du pave depend de la largeur calculee? */
		     if (pBo1->BxContentWidth
			 && !XEnAbsolu (pBo1)
			 && pavefils->AbHorizPos.PosAbRef == adpave)
		       {
			  val = ibox->BxXOrg;	/* origine de la boite a deplacer */
			  if (pavefils->AbHorizPos.PosEdge == VertMiddle)
			     val += ibox->BxWidth / 2;
			  else if (pavefils->AbHorizPos.PosEdge == Right)
			     val += ibox->BxWidth;

			  if (pavefils->AbHorizPos.PosRefEdge == VertMiddle)
			    {
			       if (!ibox->BxHorizFlex)
				  DepOrgX (ibox, NULL, x + *large / 2 - val, frame);
			    }
			  else if (pavefils->AbHorizPos.PosRefEdge == Right)
			    {
			       if (!ibox->BxHorizFlex)
				  DepOrgX (ibox, NULL, x + *large - val, frame);
			    }
		       }

		     /* La position verticale du pave depend de la hauteur calculee ? */
		     if (pBo1->BxContentHeight
			 && !YEnAbsolu (pBo1)
			 && pavefils->AbVertPos.PosAbRef == adpave)
		       {
			  val = ibox->BxYOrg;	/* origine de la boite a deplacer */
			  if (pavefils->AbVertPos.PosEdge == HorizMiddle)
			     val += ibox->BxHeight / 2;
			  else if (pavefils->AbVertPos.PosEdge == Bottom)
			     val += ibox->BxHeight;

			  if (pavefils->AbVertPos.PosRefEdge == HorizMiddle)
			    {
			       if (!ibox->BxVertFlex)
				  DepOrgY (ibox, NULL, y + *haut / 2 - val, frame);
			    }
			  else if (pavefils->AbVertPos.PosRefEdge == Bottom)
			    {
			       if (!ibox->BxVertFlex)
				  DepOrgY (ibox, NULL, y + *haut - val, frame);
			    }
		       }
		  }
		pavefils = pavefils->AbNext;
	     }
     }				/* Composition geometrique */

   /* La boite composee est vide ? */
   if (premfils == NULL && adpave->AbVolume == 0)
     {
	EvalText (adpave, &x, &y, &val);
	if (*large == 0)
	   *large = x;
	if (*haut == 0)
	   *haut = y;
     }

}				/* EvalComp */

/* ---------------------------------------------------------------------- */
/* |    CreerBoite cree la boite qui est associee au pave donne en      | */
/* |            parametre et initialise son contenu :                   | */
/* |            - Calcule sa place en caracteres dans le document tout  | */
/* |            entier.                                                 | */
/* |            - Calcule les dimensions contraintes (heritees).        | */
/* |            - Cree les boites des paves englobes si le pave n'est   | */
/* |            pas terminal.                                           | */
/* |            - Calcule les dimensions reelles de la boite et choisit | */
/* |            entre les dimensions reelles et contraintes.            | */
/* |            - Calcule les axes de references de la boite.           | */
/* |            - Positionnne l'origine (haut gauche) de la boite par   | */
/* |            rapport a` la boite englobante si misenligne est faux.  | */
/* |            Les boites terminales sont doublement (avant et arriere)| */
/* |            chainees a` partir des champs BxPrevious BxNext de la     | */
/* |            boite du pave racine.                                   | */
/* |            On met a jour l'adresse de la boite dans le pave.       | */
/* |            La fonction rend l'adresse de la boite.                 | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static PtrBox     CreerBoite (PtrAbstractBox adpave, int frame, boolean misenligne, int *place)
#else  /* __STDC__ */
static PtrBox     CreerBoite (adpave, frame, misenligne, place)
PtrAbstractBox             adpave;
int                 frame;
boolean             misenligne;
int                *place;

#endif /* __STDC__ */
{
   PtrAbstractBox             pavefils;
   PtrBox            ibox;
   PtrBox            srcbox;
   int                 large, i;
   int                 haut;
   TypeUnit            unit;
   boolean             setlarge;
   boolean             sethaut;
   boolean             eclate;
   ptrfont             font;
   PtrBox            pBo1;
   ImageDescriptor    *image;
   char                alphabet;

   if (adpave->AbDead)
      return (NULL);

   srcbox = FntrTable[frame - 1].FrAbstractBox->AbBox;
   /* Chargement de la fonte attachee au pave */
   haut = adpave->AbSize;
   unit = adpave->AbSizeUnit;
   haut += FntrTable[frame - 1].FrMagnification;

   if (adpave->AbLeafType == LtText || adpave->AbLeafType == LtSymbol || adpave->AbLeafType == LtCompound)
     {
	if (adpave->AbLeafType == LtText)
	   if (adpave->AbLanguage == '\0')
	      alphabet = 'L';
	   else
	      alphabet = TtaGetAlphabet (adpave->AbLanguage);
	else if (adpave->AbLeafType == LtSymbol)
	   alphabet = 'G';
	else if (adpave->AbLeafType == LtCompound)
	   alphabet = 'L';
	/* teste l'unite */
	font = ChargeFonte (alphabet, adpave->AbFont, adpave->AbHighlight, haut, unit, frame);
     }
   else
      font = FontMenu;

   /* Creation */
   pBo1 = adpave->AbBox;
   if (pBo1 == NULL)
     {
	pBo1 = GetBox (adpave);
	adpave->AbBox = pBo1;
     }

   if (pBo1 != NULL)
     {
	pBo1->BxFont = font;
	pBo1->BxUnderline = adpave->AbUnderline;
	pBo1->BxThickness = adpave->AbThickness;
	eclate = False;		/* A priori la boite n'est pas eclatee */

	/* Dimensionnement de la boite par contraintes */
	/* Il faut initialiser le trace reel et l'indication */
	/* des reperes inverses (en cas de boite elastique)  */
	/* avant d'evaluer la dimension de la boite si elle  */
	/* est de type graphique */
	if (adpave->AbLeafType == LtGraphics)
	  {
	     adpave->AbRealShape = adpave->AbShape;
	     pBo1->BxHorizInverted = False;
	     pBo1->BxVertInverted = False;
	  }

	setlarge = Dimensionner (adpave, frame, True);
	sethaut = Dimensionner (adpave, frame, False);
	pBo1->BxXToCompute = False;
	pBo1->BxYToCompute = False;

	/* On construit le chainage des boites terminales pour affichage */
	/* et on calcule la position des paves dans le document.         */
	if (adpave->AbFirstEnclosed == NULL)
	  {
	     /* On note pour l'affichage que cette boite est nouvelle */
	     pBo1->BxNew = True;

	     /* Est-ce un document vide ? */
	     if (srcbox == NULL)
	       {
		  pBo1->BxPrevious = NULL;
		  pBo1->BxNext = NULL;
		  /* On modifie le chainage a partir de la boite racine */
		  /* BxNext(RlRoot) -> Debut du chainage               */
		  /* BxPrevious(RlRoot) -> Fin du chainage                 */
	       }
	     else
	       {
		  /* On ajoute la boite a la fin du chainage */
		  ibox = srcbox->BxPrevious;
		  pBo1->BxPrevious = ibox;
		  if (ibox != NULL)
		     ibox->BxNext = pBo1;
		  srcbox->BxPrevious = pBo1;
		  if (srcbox->BxNext == NULL)
		     srcbox->BxNext = pBo1;
	       }
	     pBo1->BxIndChar = 0;
	     *place += adpave->AbVolume;
	  }

	/* Evaluation du contenu de la boite */
	switch (adpave->AbLeafType)
	      {
		 case LtPageColBreak:
		    pBo1->BxBuffer = NULL;
		    pBo1->BxNChars = adpave->AbVolume;
		    large = 0;
		    haut = 0;
		    break;
		 case LtText:
		    pBo1->BxBuffer = adpave->AbText;
		    pBo1->BxNChars = adpave->AbVolume;
		    pBo1->BxFirstChar = 1;
		    pBo1->BxSpaceWidth = 0;
		    EvalText (adpave, &large, &haut, &i);
		    pBo1->BxNSpaces = i;
		    break;
		 case LtPicture:
		    pBo1->BxType = BoPicture;
		    image = (ImageDescriptor *) adpave->AbImageDescriptor;
		    pBo1->BxImageDescriptor = adpave->AbImageDescriptor;
		    if (!adpave->AbPresentationBox && adpave->AbVolume != 0 && pBo1->BxImageDescriptor != NULL)
		      {

			 /* Il faut se proteger des boites de taille negative */
			 if (pBo1->BxWidth < 0)
			    ChangeLargeur (pBo1, pBo1, NULL, 10 - pBo1->BxWidth, 0, frame);
			 if (pBo1->BxHeight < 0)
			    ChangeHauteur (pBo1, pBo1, NULL, 10 - pBo1->BxHeight, frame);
		      }

		    if (image->imagePixmap == None)
		       ReadImage (frame, pBo1, image);
		    EvalPic (adpave, &large, &haut);
		    break;
		 case LtSymbol:
		    pBo1->BxBuffer = NULL;
		    pBo1->BxNChars = adpave->AbVolume;
		    /* Les reperes de la boite (elastique) ne sont pas inverses */
		    pBo1->BxHorizInverted = False;
		    pBo1->BxVertInverted = False;
		    EvalSymb (adpave, &large, &haut);
		    break;
		 case LtGraphics:
		    pBo1->BxBuffer = NULL;
		    pBo1->BxNChars = adpave->AbVolume;
		    EvalGraph (adpave, &large, &haut);
		    break;
		 case LtPlyLine:
		    /* Prend une copie des points de controle */
		    pBo1->BxBuffer = CopieTexte (adpave->AbPolyLineBuffer, NULL);
		    pBo1->BxNChars = adpave->AbVolume;	/* Nombre de points */
		    pBo1->BxImageDescriptor = NULL;
		    pBo1->BxXRatio = 1;
		    pBo1->BxYRation = 1;
		    EvalPolyLine (adpave, &large, &haut);
		    break;
		 case LtCompound:
		    /* Si le pave est mis en ligne et secable -> la boite est eclatee */
		    if (misenligne && adpave->AbAcceptLineBreak && adpave->AbFirstEnclosed != NULL)
		      {
			 eclate = True;
			 pBo1->BxType = BoGhost;
		      }
		    /* Il faut creer les boites des paves inclus */
		    pavefils = adpave->AbFirstEnclosed;
		    while (pavefils != NULL)
		      {
			 ibox = CreerBoite (pavefils, frame, eclate || adpave->AbInLine, place);
			 pavefils = pavefils->AbNext;
		      }
		    pBo1->BxSpaceWidth = 0;	/* pas d'englobement vertical en cours */
		    pBo1->BxNPixels = 0;	/* pas d'englobement horizontal en cours */
		    EvalComp (adpave, frame, &large, &haut);
		    break;
		 default:
		    break;
	      }

	/* Dimensionnement de la boite par le contenu ? */
	ChangeLgContenu (pBo1, pBo1, large, 0, frame);
	/* Il est possible que le changement de largeur de la boite modifie */
	/* indirectement (parce que la boite contient un bloc de ligne) la  */
	/* hauteur du contenu de la boite.                                  */
	if (setlarge && sethaut && adpave->AbLeafType == LtCompound)
	   EvalComp (adpave, frame, &large, &haut);
	ChangeHtContenu (pBo1, pBo1, haut, frame);

	/* Positionnement des axes de la boite construite */
	PlacerAxe (adpave->AbVertRef, pBo1, frame, True);

	/* On traite differemmment la base d'un bloc de lignes */
	/* s'il depend de la premiere boite englobee           */
	if (adpave->AbLeafType != LtCompound
	    || !adpave->AbInLine
	    || adpave->AbHorizRef.PosAbRef != adpave->AbFirstEnclosed)
	   PlacerAxe (adpave->AbHorizRef, pBo1, frame, False);

	/* Positionnement des origines de la boite construite */
	if (!misenligne)
	  {
	     Positionner (adpave->AbHorizPos, pBo1, frame, True);
	     Positionner (adpave->AbVertPos, pBo1, frame, False);
	  }
	else
	  {
	     if (!adpave->AbHorizEnclosing)
		Positionner (adpave->AbHorizPos, pBo1, frame, True);
	     if (!adpave->AbVertEnclosing)
		Positionner (adpave->AbVertPos, pBo1, frame, False);
	  }
	adpave->AbNew = False;	/* la regle de creation est interpretee */
     }
   return pBo1;
}				/* CreerBoite */

/*debut - deplacement de la procedure */
/* ---------------------------------------------------------------------- */
/* |    DesLigne cherche l'adresse de la ligne englobant la boite       | */
/* |            designee.                                               | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
PtrLine            DesLigne (PtrBox ibox)

#else  /* __STDC__ */
PtrLine            DesLigne (ibox)
PtrBox            ibox;

#endif /* __STDC__ */

{
   PtrLine            adligne;
   int                 by, ly;
   boolean             encore;
   PtrBox            box1;
   PtrBox            box2;
   PtrAbstractBox             adpave;
   PtrBox            pBo1;


   /* Recherche la ligne englobante */
   adligne = NULL;
   adpave = NULL;
   if (ibox != NULL)
     {
	if (ibox->BxAbstractBox != NULL)
	   adpave = ibox->BxAbstractBox->AbEnclosing;
     }

   /* On regarde si la boite appartient a un bloc de lignes */
   if (adpave != NULL)
     {
	if (adpave->AbBox == NULL)
	   adpave = NULL;
	/* Est-ce une boite fille d'une boite eclatee ? */
	else if (adpave->AbBox->BxType == BoGhost)
	  {
	     /* Si oui on saute les niveaux des paves eclates */
	     encore = True;
	     while (encore)
	       {
		  adpave = adpave->AbEnclosing;
		  if (adpave == NULL)
		     encore = False;
		  else if (adpave->AbBox == NULL)
		    {
		       adpave = NULL;
		       encore = False;
		    }
		  else if (adpave->AbBox->BxType != BoGhost)
		     encore = False;
	       }
	  }
	/* Est-ce que la boite est directement incluse dans une ligne */
	else if (!adpave->AbInLine)
	   adpave = NULL;
     }

   if (adpave != NULL)
     {
	pBo1 = adpave->AbBox;
	/* by = valeur en y de l'origine la boite par rapport au bloc */
	by = ibox->BxYOrg - pBo1->BxYOrg;
	adligne = pBo1->BxFirstLine;
	/* ly =  valeur en y du bas de la ligne par rapport au bloc */
	if (adligne == NULL)
	   ly = by + 1;
	else
	   ly = adligne->LiYOrg + adligne->LiHeight;

	/* On saute toutes les lignes qui precedent */
	while (by >= ly)
	  {
	     adligne = adligne->LiNext;
	     if (adligne == NULL)
		ly = by + 1;
	     else
		ly = adligne->LiYOrg + adligne->LiHeight;
	  }

	/* On verifie que la ligne contient la boite ibox */
	encore = True;
	while (encore && adligne != NULL)
	  {
	     /* On recherche la boite dans la ligne */
	     if (adligne->LiFirstPiece != NULL)
		box2 = adligne->LiFirstPiece;
	     else
		box2 = adligne->LiFirstBox;

	     /* Boucle sur les boites de la ligne */
	     do
	       {
		  if (box2->BxType == BoSplit)
		     box1 = box2->BxNexChild;
		  else
		     box1 = box2;
		  if (box1 == ibox)
		    {
		       /* On a trouve la ligne */
		       encore = False;
		       box1 = adligne->LiLastBox;
		    }
		  /* Sinon on passe a la boite suivante */
		  else
		     box2 = Suivante (box2->BxAbstractBox);
	       }
	     while (!(box1 == adligne->LiLastBox
		      || box1 == adligne->LiLastPiece || box2 == NULL));

	     if (encore)
	       {
		  adligne = adligne->LiNext;	/* On passe a la ligne suivante */
		  if (adligne != NULL)
		     /* sauf si la ligne est au-dela */
		     if (adligne->LiYOrg > by)
			adligne = NULL;
	       }
	  }
     }
   return adligne;
}
/*fin */

/* ---------------------------------------------------------------------- */
/* |    MajBox met a jour les informations d'une boite terminale (nombre| */
/* |            de caracteres, nombre de blancs, largeur).              | */
/* |            La largeur ajoutee est dx pour la boite coupee ou       | */
/* |            entiere, just ou dx (si just est 0) pour la boite de    | */
/* |            coupure.                                                | */
/* |           Si BCOUP est vrai la mise a jour modifie la coupure entre| */
/* |            deux boites, donc seule la boite coupee est mise a jour.| */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                MajBox (PtrBox ibox, PtrLine adligne, int dcar, int dbl, int dx, int just, int dy, int frame, boolean BCOUP)

#else  /* __STDC__ */
void                MajBox (ibox, adligne, dcar, dbl, dx, just, dy, frame, BCOUP)
PtrBox            ibox;
PtrLine            adligne;
int                 dcar;
int                 dbl;
int                 dx;
int                 just;
int                 dy;
int                 frame;
boolean             BCOUP;

#endif /* __STDC__ */

{
   int                 j;
   PtrBox            box1;
   PtrLine            ligne;
   Propagation      savpropage;
   PtrBox            pBo2;
   PtrAbstractBox             pPa1;
   AbPosition        *pPavP1;
   AbDimension       *pPavD1;

   ligne = adligne;

   /* Traitement particulier aux boites de coupure */
   if (ibox->BxType == BoPiece || ibox->BxType == BoDotted)
     {
	/* Mise a jour de sa boite mere (boite coupee) */
	pBo2 = ibox->BxAbstractBox->AbBox;
	pBo2->BxNChars += dcar;
	pBo2->BxNSpaces += dbl;
	pBo2->BxWidth += dx;
	pBo2->BxHeight += dy;
	/* Faut-il mettre a jour la base ? */
	pPa1 = pBo2->BxAbstractBox;
	pPavP1 = &pPa1->AbHorizRef;
	if (pPavP1->PosAbRef == NULL)
	  {
	     j = FontBase (pBo2->BxFont) - pBo2->BxHorizRef;
	     DepBase (pPa1->AbBox, NULL, j, frame);
	  }
	else if (pPavP1->PosAbRef == pBo2->BxAbstractBox)
	   if (pPavP1->PosRefEdge == HorizMiddle)
	      DepBase (pPa1->AbBox, NULL, dy / 2, frame);
	   else if (pPavP1->PosRefEdge == Bottom)
	      DepBase (pPa1->AbBox, NULL, dy, frame);

	/* Mise a jour des positions des boites suivantes */
	box1 = ibox->BxNexChild;
	while (box1 != NULL)
	  {
	     box1->BxIndChar += dcar;
	     box1 = box1->BxNexChild;
	  }
     }

   /* Traitement sur la boite passee en parametre */
   pPa1 = ibox->BxAbstractBox;
   /* Mise a jour de la boite elle-meme */
   if (pPa1->AbLeafType == LtText)
     {
	ibox->BxNSpaces += dbl;
	ibox->BxNChars += dcar;
     }

   /* Est-ce que la largeur de la boite depend de son contenu ? */
   pPavD1 = &(ibox->BxAbstractBox->AbWidth);
   if (ibox->BxContentWidth || (!pPavD1->DimIsPosition && pPavD1->DimMinimum))
      /* Blanc entre deux boites de coupure */
      if (BCOUP && ligne != NULL)
	{
	   /* Il faut mettre a jour la largeur de la boite coupee */
	   if ((ibox->BxType == BoSplit) || (just == 0))
	      ibox->BxWidth += dx;
	   else
	      ibox->BxWidth += just;
	   /* Puis refaire la mise en lignes */
	   ReevalBloc (pPa1->AbEnclosing, ligne, ibox, frame);
	}
   /* Box coupee */
      else if (ibox->BxType == BoSplit)
	{
	   savpropage = Propage;
	   Propage = ToSiblings;
	   if (dx != 0)
	      ChangeLgContenu (ibox, ibox, ibox->BxWidth + dx, 0, frame);
	   if (dy != 0)
	      ChangeHtContenu (ibox, ibox, ibox->BxHeight + dy, frame);
	   Propage = savpropage;
	   /* Faut-il mettre a jour le bloc de ligne englobant ? */
	   if (Propage == ToAll)
	     {
		ligne = DesLigne (ibox->BxNexChild);
		ReevalBloc (pPa1->AbEnclosing, ligne, ibox, frame);
	     }
	}
   /* Box entiere ou de coupure */
      else
	{
	   if (just != 0)
	      ChangeLgContenu (ibox, ibox, ibox->BxWidth + just, dbl, frame);
	   else if (dx != 0)
	      ChangeLgContenu (ibox, ibox, ibox->BxWidth + dx, dbl, frame);
	   if (dy != 0)
	      ChangeHtContenu (ibox, ibox, ibox->BxHeight + dy, frame);
	}
   else if (ibox->BxContentHeight || (!ibox->BxAbstractBox->AbHeight.DimIsPosition && ibox->BxAbstractBox->AbHeight.DimMinimum))
     {
	/* La hauteur de la boite depend de son contenu */
	if (dy != 0)
	   ChangeHtContenu (ibox, ibox, ibox->BxHeight + dy, frame);
     }
   else if (ibox->BxWidth > 0 && ibox->BxHeight > 0)
      /* Si la largeur de la boite ne depend pas de son contenu  */
      /* on doit forcer le reaffichage jusqua la fin de la boite */
      DefClip (frame, ibox->BxXOrg, ibox->BxYOrg, ibox->BxXOrg + ibox->BxWidth, ibox->BxYOrg + ibox->BxHeight);
}				/* MajBox */

/*debut */
/* ---------------------------------------------------------------------- */
/* |    FreePolyline libe`re la liste des buffers attache's a` la       | */
/* |            la boi^te Polyline.                                     | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void         FreePolyline (PtrBox ibox)

#else  /* __STDC__ */
static void         FreePolyline (ibox)
PtrBox            ibox;

#endif /* __STDC__ */

{
   PtrTextBuffer      adbuff;

   if (ibox->BxBuffer != NULL)
     {
	/* Transformation polyline en graphique simple */
	/* il faut liberer les buffers */
	ibox->BxNChars = ibox->BxAbstractBox->AbVolume;
	ibox->BxXRatio = 1;
	ibox->BxYRation = 1;
	adbuff = ibox->BxBuffer;
	while (adbuff != NULL)
	  {
	     ibox->BxBuffer = adbuff->BuNext;
	     FreeBufTexte (adbuff);
	     adbuff = ibox->BxBuffer;
	  }
     }

   if (ibox->BxImageDescriptor != NULL)
     {
	/* libere les points de controle */
	free ((char *) ibox->BxImageDescriptor);
	ibox->BxImageDescriptor = NULL;
     }
}				/*FreePolyline */
/*fin */


/* ---------------------------------------------------------------------- */
/* |    DispBoite libere toutes les boites correpondant aux paves inclus| */
/* |            dans adpave y compris celle du pave passe' en parametre | */
/* |            et poste le booleen Recree si la boite doit etre recree | */
/* |            immediatement apres.                                    | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                DispBoite (PtrAbstractBox adpave, boolean Recree, int frame)

#else  /* __STDC__ */
void                DispBoite (adpave, Recree, frame)
PtrAbstractBox             adpave;
boolean             Recree;
int                 frame;

#endif /* __STDC__ */

{
   PtrAbstractBox             pavefils;
   boolean             chgDS;
   boolean             chgFS;
   PtrBox            pBo1;

   if (adpave != NULL)
     {
	if (adpave->AbBox != NULL)
	  {
	     /* Liberation des lignes et boites coupees */
	     pBo1 = adpave->AbBox;
	     if (pBo1->BxType == BoBlock)
		DispBloc (pBo1, frame, pBo1->BxFirstLine, &chgDS, &chgFS);

	     else if (adpave->AbLeafType == LtPlyLine)
		FreePolyline (pBo1);
	     pavefils = adpave->AbFirstEnclosed;
	     adpave->AbNew = Recree;

	     if (Recree)
	       {
		  /* Faut-il restaurer les regles d'une boite elastique */
		  if (pBo1->BxHorizFlex && pBo1->BxHorizInverted)
		     HorizInverse (pBo1, OpHorizDep);

		  if (pBo1->BxVertFlex && pBo1->BxVertInverted)
		     VertInverse (pBo1, OpVertDep);
	       }

	     /* Liberation des boites des paves inclus */
	     while (pavefils != NULL)
	       {
		  DispBoite (pavefils, Recree, frame);
		  pavefils = pavefils->AbNext;	/* while */
	       }

	     /* Suppression des references a pBo1 dans la selection */
	     if (FntrTable[frame - 1].FrSelectionBegin.VsBox == pBo1)
		FntrTable[frame - 1].FrSelectionBegin.VsBox = NULL;
	     if (FntrTable[frame - 1].FrSelectionEnd.VsBox == pBo1)
		FntrTable[frame - 1].FrSelectionEnd.VsBox = NULL;

	     /* Liberation des liens eventuels hors hierarchie */
	     RazHorsEnglobe (pBo1);

	     /* Liberation de la boite */
	     adpave->AbBox = FreeBox (adpave->AbBox);
	     adpave->AbBox = NULL;
	  }
     }
}

/* ---------------------------------------------------------------------- */
/* |    PaveVide retourne la valeur Vrai si pave est vide.              | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static boolean      PaveVide (PtrAbstractBox pave)

#else  /* __STDC__ */
static boolean      PaveVide (pave)
PtrAbstractBox             pave;

#endif /* __STDC__ */

{
   PtrAbstractBox             pavefils;
   boolean             fini;

   pavefils = pave->AbFirstEnclosed;
   fini = pavefils == NULL;
   while (!fini)
      if (pavefils->AbDead)
	{
	   pavefils = pavefils->AbNext;
	   fini = pavefils == NULL;
	}
      else
	 fini = True;

   return (pavefils == NULL);
}

/* ---------------------------------------------------------------------- */
/* |    PaveAvant recherche le dernier pave terminal vivant avant le pave| */
/* |            designe'.                                               | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static PtrAbstractBox      PaveAvant (PtrAbstractBox pave)

#else  /* __STDC__ */
static PtrAbstractBox      PaveAvant (pave)
PtrAbstractBox             pave;

#endif /* __STDC__ */

{
   PtrAbstractBox             pavevo;
   PtrAbstractBox             cepave;
   boolean             encore;
   PtrAbstractBox             result;

   pavevo = pave->AbPrevious;
   cepave = NULL;
   encore = True;

   /* On recherche en arriere un pave vivant */
   while (encore)
      if (pavevo == NULL)
	 encore = False;
      else if (pavevo->AbBox == NULL)
	 pavevo = pavevo->AbPrevious;
      else if (pavevo->AbFirstEnclosed == NULL)
	 encore = False;
      else
	{
	   /* On descend la hierarchie pour prendre son dernier fils */
	   cepave = pavevo->AbFirstEnclosed;
	   while (cepave != NULL && cepave != pave)
	     {
		pavevo = cepave;
		cepave = cepave->AbNext;

		/* On parcours la liste des fils */
		while (cepave != NULL && cepave != pave)
		  {
		     pavevo = cepave;
		     cepave = cepave->AbNext;
		  }

		/* On descend d'un niveau */
		if (pavevo->AbBox != NULL)
		   cepave = pavevo->AbFirstEnclosed;
	     }

	   cepave = pavevo->AbEnclosing;
	   if (pavevo->AbBox != NULL)
	      encore = False;	/* On a trouve */
	   else
	     {
		/* Si ce dernier fils est mort -> on prend le dernier */
		/* pave vivant avant lui, ou a defaut l'englobant     */
		while (encore)
		   if (pavevo->AbPrevious == NULL)
		     {
			encore = False;
			pavevo = NULL;	/* C'est l'englobant */
		     }
		   else if (pavevo->AbPrevious->AbBox != NULL)
		      encore = False;
		   else
		      pavevo = pavevo->AbPrevious;

		if (pavevo != NULL)
		   pavevo = PaveAvant (pavevo);
	     }
	}

   /* Resultat de la recherche */
   if (pavevo != NULL)
      result = pavevo;
   else if (cepave != NULL)
      result = cepave;
   else
     {
	/* On n'a pas trouve de precedent -> On remonte dans la hierarchie */
	cepave = pave->AbEnclosing;
	if (cepave != NULL)
	   result = PaveAvant (cepave);
	else
	   result = NULL;
     }
   return result;
}				/* PaveAvant */


/* ---------------------------------------------------------------------- */
/* |    VerifPos reevalue les regles par defaut des paves suivants si le| */
/* |            pave cree ou detruit est positionne par une regle par   | */
/* |            defaut.                                                 | */
/* ---------------------------------------------------------------------- */


#ifdef __STDC__
static void         VerifPos (PtrAbstractBox adpave, int frame)

#else  /* __STDC__ */
static void         VerifPos (adpave, frame)
PtrAbstractBox             adpave;
int                 frame;

#endif /* __STDC__ */

{
   PtrAbstractBox             cepave;

   /* Est-ce que la boite avait une regle par defaut ? */
   if (adpave->AbEnclosing != NULL)
      if (!adpave->AbEnclosing->AbInLine
	  && !(adpave->AbEnclosing->AbBox->BxType == BoGhost))
	{
	   if (adpave->AbHorizPos.PosAbRef == NULL)
	     {
		/* On recherche le pave suivant ayant la meme regle par defaut */
		cepave = adpave->AbNext;
		while (cepave != NULL)
		  {
		     if (cepave->AbHorizPos.PosAbRef == NULL)
		       {
			  /* Reevalue la regle du premier pave suivant non mort */
			  if (!cepave->AbDead && cepave->AbBox != NULL
			  /* si ce pave ne vient pas d'etre cree                */
			      && cepave->AbNum == 0)
			    {
			       /* Nouvelle position horizontale */
			       RazPosition (cepave->AbBox, True);
			       Positionner (cepave->AbHorizPos, cepave->AbBox, frame, True);
			    }	/*if !cepave->AbDead */
			  cepave = NULL;

		       }
		     else
			cepave = cepave->AbNext;
		  }
	     }
	   if (adpave->AbVertPos.PosAbRef == NULL)
	     {
		/* On recherche le pave suivant ayant la meme regle par defaut */
		cepave = adpave->AbNext;
		while (cepave != NULL)
		  {
		     if (cepave->AbVertPos.PosAbRef == NULL)
		       {
			  /* Reevalue la regle du premier pave suivant non mort */
			  if (!cepave->AbDead && cepave->AbBox != NULL
			  /* si ce pave ne vient pas d'etre cree                */
			      && cepave->AbNum == 0)
			    {
			       /* Nouvelle position verticale */
			       RazPosition (cepave->AbBox, False);
			       Positionner (cepave->AbVertPos, cepave->AbBox, frame, False);
			    }	/*if !cepave->AbDead */
			  cepave = NULL;
		       }
		     else
			cepave = cepave->AbNext;
		  }
	     }
	}
}

/* ---------------------------------------------------------------------- */
/* |    EvalChange traite les modifications d'un pave correspondant a`  | */
/* |            la fenetre frame. Rend la valeur vrai s'il y a          | */
/* |            modification sur la boite du pave.                      | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static boolean      EvalChange (PtrAbstractBox adpave, int frame)
#else  /* __STDC__ */
static boolean      EvalChange (adpave, frame)
PtrAbstractBox             adpave;
int                 frame;

#endif /* __STDC__ */
{
   int                 large, haut;
   int                 nbbl;
   PtrLine            adligne;
   PtrAbstractBox             pavevois;
   PtrBox            nextbox;
   PtrBox            box;
   PtrBox            srcbox;
   PtrBox            lastbox;
   int                 i, dcar, just;
   TypeUnit            unit;
   Propagation      savpropage;
   boolean             condition;
   ViewFrame            *pFe1;
   PtrBox            ibox;
   AbDimension       *pPavD1;
   PtrLine            pLi1;
   AbPosition        *pPavP1;
   boolean             result;
   boolean             absoluEnX;
   boolean             absoluEnY;

   pFe1 = &FntrTable[frame - 1];
   lastbox = NULL;
   nbbl = 0;			/* nombre de blancs */
   dcar = 0;			/* nombre de caracteres */
   just = 0;			/* largeur avec des blancs justifies */
   large = 0;
   haut = 0;
   srcbox = pFe1->FrAbstractBox->AbBox;	/* boite de la racine */
   ibox = adpave->AbBox;	/* boite du pave */
   result = False;

   /* On prepare le reaffichage */
   if (!adpave->AbNew
       && (adpave->AbDead || adpave->AbChange
	   || adpave->AbWidthChange || adpave->AbHeightChange || adpave->AbHorizPosChange
	   || adpave->AbVertPosChange || adpave->AbHorizRefChange || adpave->AbVertRefChange
	   || adpave->AbAspectChange || adpave->AbSizeChange))
     {
	/* Est-ce que la boite est coupee ? */
	box = ibox;
	/* Si la boite est coupee on prend la premiere boite de coupure */
	if (box->BxType == BoSplit)
	   box = box->BxNexChild;
	if ((box->BxWidth > 0 && box->BxHeight > 0) || box->BxType == BoPicture)
	   DefClip (frame, box->BxXOrg, box->BxYOrg, box->BxXOrg + box->BxWidth,
		    box->BxYOrg + box->BxHeight);
     }

   /* NOUVEAU AbstractBox */
   if (adpave->AbNew)
     {
	/* On situe la boite dans le chainage des boites terminales */
	pavevois = PaveAvant (adpave);
	/* 1ere boite precedente */
	if (pavevois != NULL)
	   box = pavevois->AbBox;
	else
	   box = NULL;

	if (box != NULL)
	  {
	     i = 0;		/* Place dans le document */
	     /* Est-ce que la boite est coupee ? */
	     if (box->BxType == BoSplit)
		while (box->BxNexChild != NULL)
		   box = box->BxNexChild;
	     nextbox = box->BxNext;	/* 1ere boite suivante */
	  }
	else
	  {
	     i = 0;		/* Place dans le document */
	     if (srcbox != NULL)
		nextbox = srcbox->BxNext;	/* suivante */
	     else
		nextbox = NULL;
	  }

	/* Est-ce la boite racine ? */
	pavevois = adpave->AbEnclosing;
	if (pavevois == NULL)
	   condition = False;
	else
	  {
	     /* Est-ce que la boite englobante doit etre eclatee ? */
	     if (pavevois->AbAcceptLineBreak && pavevois->AbEnclosing != NULL)
		if (pavevois->AbEnclosing->AbInLine
		    || pavevois->AbEnclosing->AbBox->BxType == BoGhost)
		   pavevois->AbBox->BxType = BoGhost;

	     /* L'indicateur de mise en lignes depend de la boite englobante */
	     condition = pavevois->AbInLine || pavevois->AbBox->BxType == BoGhost;

	     /* Faut-il dechainer la boite englobante ? */
	     if ((nextbox != NULL) && (nextbox == pavevois->AbBox))
		nextbox = nextbox->BxNext;
	     /* On etabli le chainage pour inserer en fin les nouvelles boites */
	     /* Faut-il dechainer la boite englobante ? */
	     if (box == NULL)
		srcbox->BxNext = NULL;		/* debut du chainage */
	     lastbox = srcbox->BxPrevious;		/* on memorise la derniere boite */
	     srcbox->BxPrevious = box;	/* fin provisoire du chainage */
	  }

	/* Faut-il dechainer la boite englobante ? */
	EvalAffich = False;	/* On arrete l'evaluation du reaffichage */
	savpropage = Propage;
	Propage = ToSiblings;	/* Limite la propagation sur le descendance */
	ibox = CreerBoite (adpave, frame, condition, &i);

	EvalAffich = True;	/* On retablit l'evaluation du reaffichage */

	/* Mise a jour de la liste des boites terminales */
	if (srcbox != NULL && nextbox != NULL)
	  {
	     /* On ajoute en fin de chainage */
	     nextbox->BxPrevious = srcbox->BxPrevious;
	     /* derniere boite chainee */
	     if (srcbox->BxPrevious != NULL)
		srcbox->BxPrevious->BxNext = nextbox;
	     srcbox->BxPrevious = lastbox;
	     /* nouvelle fin de chainage */
	     if (srcbox->BxNext == NULL)
		srcbox->BxNext = nextbox;
	  }

	/* Si la boite a ete creee (box<>NULL) */
	if (ibox == NULL)
	   Propage = savpropage;	/* Restaure la regle de propagation */
	else
	  {
	     /* On place les origines des boites par rapport a la racine de la vue */
	     /* si la boite n'a pas deja ete placee en absolu */
	     XYEnAbsolu (ibox, &absoluEnX, &absoluEnY);
	     Propage = savpropage;	/* Restaure la regle de propagation */
	     if (!absoluEnX || !absoluEnY)
	       {
		  /* Initialise le placement des boites creees */
		  MarqueAPlacer (adpave, !absoluEnX, !absoluEnY);
		  /* La boite racine va etre placee */
		  ibox->BxXToCompute = False;
		  ibox->BxYToCompute = False;
		  Placer (adpave, pFe1->FrVisibility, frame, !absoluEnX, !absoluEnY);
	       }

	     /* On prepare le reaffichage */
	     if (pavevois == NULL)
		condition = True;
	     else
		condition = !pavevois->AbInLine;
	     if (condition)
	       {
		  DefClip (frame, ibox->BxXOrg, ibox->BxYOrg, ibox->BxXOrg + ibox->BxWidth,
			   ibox->BxYOrg + ibox->BxHeight);
	       }

	     /* On verifie la coherence des positions par defaut */
/***VerifPos(adpave, frame);***/
	     adpave->AbChange = False;
	     result = True;
	  }
     }
   /* AbstractBox MORT */
   else if (adpave->AbDead)
     {
	if (adpave->AbLeafType == LtPlyLine)
	   FreePolyline (ibox);	/* libere la liste des buffers de la boite */

	/* On situe la boite dans le chainage des boites terminales */
	pavevois = PaveAvant (adpave);

	/* On recherche la derniere boite terminale avant */
	if (pavevois == NULL)
	   box = srcbox;	/* debut du chainage */
	else
	  {
	     box = pavevois->AbBox;
	     /* Est-ce que la boite est coupee ? */
	     if (box->BxType == BoSplit)
		while (box->BxNexChild != NULL)
		   box = box->BxNexChild;
	  }

	/* Est-ce que la boite englobante devient terminale ? */
	pavevois = adpave->AbEnclosing;
	if (PaveVide (pavevois))
	  {
	     nextbox = pavevois->AbBox;
	     /* Si la boite etait eclatee, elle ne l'est plus */
	     if (nextbox->BxType == BoGhost)
		nextbox->BxType = BoComplete;

	     /* On ne chaine qu'une seule fois la boite englobante */
	     if (nextbox != box->BxNext)
	       {
		  if (box == srcbox)
		     nextbox->BxPrevious = NULL;
		  else
		     nextbox->BxPrevious = box;

		  /* On defait l'ancien chainage */
		  box->BxNext->BxPrevious = box->BxNext;
		  box->BxNext = nextbox;
	       }
	     box = nextbox;
	  }

	/* On recherche la premiere boite terminale apres */
	pavevois = adpave;
	nextbox = NULL;
	while (nextbox == NULL)
	  {
	     while (pavevois->AbEnclosing != NULL && pavevois->AbNext == NULL)
		pavevois = pavevois->AbEnclosing;
	     pavevois = pavevois->AbNext;
	     if (pavevois == NULL)
		nextbox = srcbox;	/* 1ere boite suivante */
	     else
	       {
		  while (pavevois->AbBox != NULL && pavevois->AbFirstEnclosed != NULL)
		     pavevois = pavevois->AbFirstEnclosed;
		  nextbox = pavevois->AbBox;
	       }
	  }

	/* Est-ce que la boite est coupee ? */
	if (nextbox->BxType == BoSplit)
	   nextbox = nextbox->BxNexChild;

	/* Destruction */
	RazLiens (ibox);
	RazDim (ibox, True, frame);
	RazDim (ibox, False, frame);
	DispBoite (adpave, False, frame);

	/* Mise a jour de la liste des boites terminales */
	/* premiere boite de la liste */
	if (box == srcbox)
	   nextbox->BxPrevious = NULL;
	else
	   nextbox->BxPrevious = box;
	/* derniere boite de la liste */
	if (nextbox == srcbox)
	   box->BxNext = NULL;
	else
	   box->BxNext = nextbox;

	/* On verifie la coherence des positions par defaut */
/***VerifPos(adpave, frame);*/
	result = True;
     }
   else
     {
	/* CHANGEMENT GRAPHIQUE */
	if (adpave->AbAspectChange)
	  {
	     adpave->AbAspectChange = False;
	     if (adpave->AbLeafType == LtPicture
		 && ((ImageDescriptor *) ibox->BxImageDescriptor)->imageType == Bitmap_drvr)
	       {
		  /* Il faut forcer le rechargement des images */

		  SetCursorWatch (frame);
		  ReadImage (frame, ibox, (ImageDescriptor *) ibox->BxImageDescriptor);
		  ResetCursorWatch (frame);
	       }
	     else if (ibox->BxType == BoSplit)
	       {
		  /* Si la boite est coupee on etend le clipping jusqu'a */
		  /* la derniere boite de coupure non vide */
		  box = ibox;
		  result = True;
		  while (result)
		    {
		       if (box->BxNexChild == NULL)
			  result = False;
		       else if (box->BxNexChild->BxNChars == 0)
			  result = False;
		       else
			  box = box->BxNexChild;
		    }
		  if (box->BxWidth > 0 && box->BxHeight > 0)
		     DefClip (frame, box->BxXOrg, box->BxYOrg, box->BxXOrg + box->BxWidth,
			      box->BxYOrg + box->BxHeight);
	       }
	  }
	/* CONTENU DU PAVE MODIFIE */
	if (adpave->AbChange || adpave->AbSizeChange)
	  {
	     /* On verifie que la fonte attachee au pave est chargee */
	     haut = adpave->AbSize;
	     unit = adpave->AbSizeUnit;
	     haut += pFe1->FrMagnification;

	     if (adpave->AbLeafType == LtText)
		ibox->BxFont = ChargeFonte (TtaGetAlphabet (adpave->AbLanguage), adpave->AbFont,
				    adpave->AbHighlight, haut, unit, frame);
	     else if (adpave->AbLeafType == LtSymbol)
		ibox->BxFont = ChargeFonte ('G', adpave->AbFont,
				    adpave->AbHighlight, haut, unit, frame);
	     else
		ibox->BxFont = ChargeFonte ('L', 'T', 0, haut, unit, frame);

	     /* On transmet l'information souligne dans la boite */
	     ibox->BxUnderline = adpave->AbUnderline;
	     ibox->BxThickness = adpave->AbThickness;

	     /* On teste le type du pave pour mettre a jour la boite */
	     if (adpave->AbLeafType == LtCompound)
	       {
		  /* Si c'est un bloc de lignes qui est mis a jour */
		  if (adpave->AbChange && adpave->AbInLine
		      && ibox->BxType != BoGhost)
		     ReevalBloc (adpave, NULL, ibox, frame);
	       }
	     else
	       {
		  switch (adpave->AbLeafType)
			{
			   case LtPageColBreak:
			      large = 0;
			      haut = 0;
			      break;
			   case LtText:
			      EvalText (adpave, &large, &haut, &nbbl);

			      /* Si la boite est justifiee */
			      if (ibox->BxSpaceWidth != 0)
				{
				   i = ibox->BxSpaceWidth - CarWidth (BLANC, ibox->BxFont);
				   /* Ecart de largeur */
				   just = large + i * nbbl + ibox->BxNPixels - ibox->BxWidth;
				   large = ibox->BxWidth;	/* On prend la largeur justifiee */
				}
			      dcar = adpave->AbVolume - ibox->BxNChars;		/* ecart de caracteres */
			      nbbl -= ibox->BxNSpaces;	/* ecart de blancs */
			      ibox->BxBuffer = adpave->AbText;
			      break;
			   case LtPicture:
			      if (adpave->AbChange)
				{
				   SetCursorWatch (frame);
				   ReadImage (frame, ibox, (ImageDescriptor *) ibox->BxImageDescriptor);
				   ResetCursorWatch (frame);
				   EvalPic (adpave, &large, &haut);
				}
			      else
				{
				   large = ibox->BxWidth;
				   haut = ibox->BxHeight;
				}
			      break;
			   case LtSymbol:
			      EvalSymb (adpave, &large, &haut);
			      break;
			   case LtGraphics:
			      /* Si le trace graphique a change */
			      if (adpave->AbChange)
				{
				   /* Si transformation polyline en graphique simple */
				   FreePolyline (ibox);
				   adpave->AbRealShape = adpave->AbShape;

				   /* remonte a la recherche d'un ancetre elastique */
				   pavevois = adpave;
				   while (pavevois != NULL)
				     {
					box = pavevois->AbBox;
					if (box->BxHorizFlex || box->BxVertFlex)
					  {
					     AjusteTrace (adpave, box->BxHorizInverted, box->BxVertInverted, False);
					     pavevois = NULL;	/* on arrete */
					  }
					else
					   pavevois = pavevois->AbEnclosing;
				     }
				}
			      EvalGraph (adpave, &large, &haut);
			      break;
			   case LtPlyLine:
			      if (adpave->AbChange)
				{
				   /* Libere les anciens buffers */
				   FreePolyline (ibox);
				   /* Recopie les buffers du pave */
				   ibox->BxBuffer = CopieTexte (adpave->AbPolyLineBuffer, NULL);
				   ibox->BxNChars = adpave->AbVolume;

				   /* remonte a la recherche d'un ancetre elastique */
				   pavevois = adpave;
				   while (pavevois != NULL)
				     {
					box = pavevois->AbBox;
					if (box->BxHorizFlex || box->BxVertFlex)
					  {
					     AjusteTrace (adpave, box->BxHorizInverted, box->BxVertInverted, False);
					     pavevois = NULL;	/* on arrete */
					  }
					else
					   pavevois = pavevois->AbEnclosing;
				     }
				}
			      EvalPolyLine (adpave, &large, &haut);
			      break;
			   default:
			      break;
			}

		  /* On modifie la boite  */
		  pPavD1 = &adpave->AbWidth;
		  if (pPavD1->DimIsPosition)
		     large = 0;
		  else if (pPavD1->DimAbRef != NULL || pPavD1->DimValue > 0)
		     large = 0;
		  else
		     large -= ibox->BxWidth;	/* ecart de largeur */
		  pPavD1 = &adpave->AbHeight;
		  if (pPavD1->DimIsPosition)
		     haut = 0;
		  else if (pPavD1->DimAbRef != NULL || pPavD1->DimValue > 0)
		     haut = 0;
		  else
		     haut -= ibox->BxHeight;	/* ecart de hauteur */
		  adligne = NULL;
		  MajBox (ibox, adligne, dcar, nbbl, large, just, haut, frame, False);
		  result = True;
	       }

	     if (adpave->AbChange)
		adpave->AbChange = False;
	     if (adpave->AbSizeChange)
	       {
		  adpave->AbSizeChange = False;
		  /* Il faut regarder les consequences du changement de taille */
		  pPavD1 = &adpave->AbWidth;
		  if (!pPavD1->DimIsPosition)
		    {
		       if (pPavD1->DimAbRef != NULL || pPavD1->DimValue != 0)
			  adpave->AbWidthChange = (pPavD1->DimUnit == UnRelative);
		       if (adpave->AbHorizPos.PosAbRef != NULL)
			  adpave->AbHorizPosChange = (adpave->AbHorizPos.PosUnit == UnRelative);
		    }

		  pPavD1 = &adpave->AbHeight;
		  if (!pPavD1->DimIsPosition)
		    {
		       if (pPavD1->DimAbRef != NULL || pPavD1->DimValue != 0)
			  adpave->AbHeightChange = (pPavD1->DimUnit == UnRelative);
		       if (adpave->AbVertPos.PosAbRef != NULL)
			  adpave->AbVertPosChange = (adpave->AbVertPos.PosUnit == UnRelative);
		    }

		  adpave->AbHorizRefChange = (adpave->AbHorizRef.PosUnit == UnRelative);
		  adpave->AbVertRefChange = (adpave->AbVertRef.PosUnit == UnRelative);
	       }
	  }
	/* CHANGEMENT DE LARGEUR */
	if (adpave->AbWidthChange)
	  {
	     /* Annulation ancienne largeur */
	     RazDim (ibox, True, frame);
	     /* Nouvelle largeur */
	     condition = Dimensionner (adpave, frame, True);
	     if (condition || (!adpave->AbWidth.DimIsPosition && adpave->AbWidth.DimMinimum))
	       {
		  switch (adpave->AbLeafType)
			{
			   case LtText:
			      EvalText (adpave, &large, &haut, &i);
			      break;
			   case LtPicture:
			      EvalPic (adpave, &large, &haut);
			      break;
			   case LtSymbol:
			      EvalSymb (adpave, &large, &haut);
			      break;
			   case LtGraphics:
			      EvalGraph (adpave, &large, &haut);
			      break;
			   case LtCompound:
			      if (adpave->AbInLine)
				{
				   /* La regle mise en lignes est prise en compte? */
				   if (ibox->BxType != BoGhost)
				      ReevalBloc (adpave, NULL, NULL, frame);
				   large = ibox->BxWidth;
				}
			      else
				 EvalComp (adpave, frame, &large, &haut);
			      break;
			   default:
			      large = ibox->BxWidth;
			      break;
			}

		  /* Mise a jour de la largeur du contenu */
		  ChangeLgContenu (ibox, NULL, large, 0, frame);
		  result = True;
	       }
	     /* La boite ne depend pas de son contenu */
	     else
		result = True;
	  }
	/* CHANGEMENT DE HAUTEUR */
	if (adpave->AbHeightChange)
	  {
	     /* Annulation ancienne hauteur */
	     RazDim (ibox, False, frame);
	     /* Nouvelle hauteur */
	     condition = Dimensionner (adpave, frame, False);
	     if (condition || (!adpave->AbHeight.DimIsPosition && adpave->AbHeight.DimMinimum))
	       {
		  switch (adpave->AbLeafType)
			{
			   case LtText:
			      EvalText (adpave, &large, &haut, &i);
			      break;
			   case LtPicture:
			      EvalPic (adpave, &large, &haut);
			      break;
			   case LtSymbol:
			      EvalSymb (adpave, &large, &haut);
			      break;
			   case LtGraphics:
			      EvalGraph (adpave, &large, &haut);
			      break;
			   case LtCompound:
			      if (adpave->AbInLine)
				{
				   /* On evalue la hauteur du bloc de ligne */
				   adligne = ibox->BxLastLine;
				   if (adligne == NULL || ibox->BxType == BoGhost)
				      haut = ibox->BxHeight;
				   else
				     {
					pLi1 = adligne;
					haut = pLi1->LiYOrg + pLi1->LiHeight;
				     }
				}
			      else
				 EvalComp (adpave, frame, &large, &haut);
			      break;
			   default:
			      haut = ibox->BxHeight;
			      break;
			}


		  /* Mise a jour de la hauteur du contenu */
		  ChangeHtContenu (ibox, NULL, haut, frame);
		  result = True;
	       }
	     /* La boite ne depend pas de son contenu */
	     else
		result = True;
	  }
	/* CHANGEMENT DE POSITION HORIZONTALE */
	if (adpave->AbHorizPosChange)
	  {
	     /* Les cas de coherence sur les boites elastiques */
	     /* Les reperes Position et Dimension doivent etre differents */
	     /* Ces reperes ne peuvent pas etre l'axe de reference        */
	     if (adpave->AbWidth.DimIsPosition
		 && (adpave->AbHorizPos.PosEdge == adpave->AbWidth.DimPosition.PosEdge
		     || adpave->AbHorizPos.PosEdge == VertMiddle
		     || adpave->AbHorizPos.PosEdge == VertRef))
	       {
		  if (adpave->AbWidth.DimPosition.PosEdge == Left)
		     adpave->AbHorizPos.PosEdge = Right;
		  else if (adpave->AbWidth.DimPosition.PosEdge == Right)
		     adpave->AbHorizPos.PosEdge = Left;
		  adpave->AbHorizPosChange = False;
	       }
	     else if (adpave->AbEnclosing == NULL)
		condition = True;
	     /* La regle de position est annulee par la mise en lignes */
	     else if (adpave->AbEnclosing->AbInLine
		      || adpave->AbEnclosing->AbBox->BxType == BoGhost)
		if (!adpave->AbHorizEnclosing)
		   condition = True;
		else
		   condition = False;
	     else
		condition = True;
	     if (condition)
	       {
		  /* Annulation ancienne position horizontale */
		  RazPosition (ibox, True);
		  /* Nouvelle position horizontale */
		  Positionner (adpave->AbHorizPos, ibox, frame, True);
		  result = True;
	       }
	     else
		adpave->AbHorizPosChange = False;
	  }
	/* CHANGEMENT DE POSITION VERTICALE */
	if (adpave->AbVertPosChange)
	  {
	     /* Les cas de coherence sur les boites elastiques */
	     /* Les reperes Position et Dimension doivent etre differents */
	     /* Ces reperes ne peuvent pas etre l'axe de reference        */
	     if (adpave->AbHeight.DimIsPosition
		 && (adpave->AbVertPos.PosEdge == adpave->AbHeight.DimPosition.PosEdge
		     || adpave->AbVertPos.PosEdge == HorizMiddle
		     || adpave->AbVertPos.PosEdge == HorizRef))
	       {
		  if (adpave->AbHeight.DimPosition.PosEdge == Top)
		     adpave->AbVertPos.PosEdge = Bottom;
		  else if (adpave->AbHeight.DimPosition.PosEdge == Bottom)
		     adpave->AbVertPos.PosEdge = Top;
		  adpave->AbVertPosChange = False;
	       }
	     else if (adpave->AbEnclosing == NULL)
		condition = True;
	     /* La regle de position est annulee par la mise en lignes */
	     else if (adpave->AbEnclosing->AbInLine
		      || adpave->AbEnclosing->AbBox->BxType == BoGhost)
	       {
		  if (!adpave->AbHorizEnclosing)
		    {
		       pPavP1 = &adpave->AbVertPos;
		       adligne = DesLigne (ibox);
		       if (adligne != NULL)
			 {
			    i = PixelValue (pPavP1->PosDistance, pPavP1->PosUnit, adpave);
			    adligne->LiYOrg += i;
			    EnglLigne (ibox, frame, adpave->AbEnclosing);
			 }
		    }
		  condition = False;
	       }
	     else
		condition = True;

	     if (condition)
	       {
		  /* Annulation ancienne position verticale */
		  RazPosition (ibox, False);
		  /* Nouvelle position verticale */
		  Positionner (adpave->AbVertPos, ibox, frame, False);
		  result = True;
	       }
	     else
		adpave->AbVertPosChange = False;
	  }
	/* CHANGEMENT D'AXE HORIZONTAL */
	if (adpave->AbHorizRefChange)
	  {
	     /* Annulation ancien axe horizontal */
	     RazAxe (ibox, False);
	     /* Nouvel axe horizontal */
	     PlacerAxe (adpave->AbHorizRef, ibox, frame, False);
	     result = True;
	  }
	/* CHANGEMENT D'AXE VERTICAL */
	if (adpave->AbVertRefChange)
	  {
	     /* Annulation ancien axe verticale */
	     RazAxe (ibox, True);
	     /* Nouvel axe vertical */
	     PlacerAxe (adpave->AbVertRef, ibox, frame, True);
	     result = True;
	  }
     }
   return result;
}				/* EvalChange */

/*debut */
/* ---------------------------------------------------------------------- */
/* | DiffereEnglobement  enregistre les englobements diffe're's.        | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                DiffereEnglobement (PtrBox ibox, boolean EnX)

#else  /* __STDC__ */
void                DiffereEnglobement (ibox, EnX)
PtrBox            ibox;
boolean             EnX;

#endif /* __STDC__ */

{
   int                 i = 0;
   PtrDimRelations      addim;
   PtrDimRelations      cedim;
   boolean             acreer;

   /* On recherche une entree libre */
   cedim = NULL;
   addim = RetardeEngl;
   acreer = True;
   while (acreer && addim != NULL)
     {
	i = 0;
	cedim = addim;

	while (i < MAX_RELAT_DIM && addim->DimRTable[i] != NULL)
	  {
	     if (addim->DimRTable[i] == ibox && addim->DimRSame[i] == EnX)
		return;		/* La boite est deja enregistree */
	     else
		i++;
	  }

	if (i == MAX_RELAT_DIM)
	   addim = addim->DimRNext;	/* Bloc suivant */
	else
	   acreer = False;
     }

   /* Faut-il creer un nouveau bloc de relations ? */
   if (acreer)
     {
	GetBDim (&addim);
	if (cedim == NULL)
	   RetardeEngl = addim;
	else
	   cedim->DimRNext = addim;
	i = 0;
     }

   addim->DimRTable[i] = ibox;
   addim->DimRSame[i] = EnX;	/* englobement en X */
}
/*fin */

/* ---------------------------------------------------------------------- */
/* | TraiteEnglobement traite les contraintes d'englobement diffe're'es | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                TraiteEnglobement (int frame)

#else  /* __STDC__ */
void                TraiteEnglobement (frame)
int                 frame;

#endif /* __STDC__ */

{
   int                 i;
   PtrDimRelations      addim;
   PtrDimRelations      pTa1;

   /* Il faut reevaluer la dimension des boites dont le contenu est */
   /* englobe et depend de relations hors-structure.                */
   addim = RetardeEngl;
   while (addim != NULL)
     {
	/* On traite toutes les boites enregistrees */
	pTa1 = addim;
	i = 1;
	while (i <= MAX_RELAT_DIM)
	  {
	     if (pTa1->DimRTable[i - 1] == NULL)
		i = MAX_RELAT_DIM;
	     else if (pTa1->DimRTable[i - 1]->BxAbstractBox == NULL)
		;		/* le pave n'existe plus */
	     else if (pTa1->DimRSame[i - 1])
		Englobx (pTa1->DimRTable[i - 1]->BxAbstractBox, NULL, frame);
	     else
		Engloby (pTa1->DimRTable[i - 1]->BxAbstractBox, NULL, frame);
	     i++;		/* Entree suivante */
	  }

	/* On passe au bloc suivant */
	addim = addim->DimRNext;
     }

   /* On libere les blocs */
   while (RetardeEngl != NULL)
     {
	addim = RetardeEngl;
	RetardeEngl = RetardeEngl->DimRNext;
	FreeBDim (&addim);
     }
}				/* TraiteEnglobement */

/* ---------------------------------------------------------------------- */
/* |    ModFenetre reevalue une vue de document suite a` la modification| */
/* |            de frame. Reaffiche la vue dans la fenetre supposee     | */
/* |            nettoyee.                                               | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                ModFenetre (int frame)

#else  /* __STDC__ */
void                ModFenetre (frame)
int                 frame;

#endif /* __STDC__ */

{
   boolean             condition;
   ViewFrame            *pFe1;
   PtrAbstractBox             pPa1;
   PtrBox            pBo1;
   boolean             savepret;
   int                 large, haut;

   pFe1 = &FntrTable[frame - 1];
   if (pFe1->FrAbstractBox != NULL)
     {
	pPa1 = pFe1->FrAbstractBox;
	if (pPa1->AbBox != NULL)
	  {
	     pBo1 = pPa1->AbBox;
	     savepret = pFe1->FrReady;
	     pFe1->FrReady = False;	/* La frame n'est pas affichable */
	     /* Faut-il changer les dimensions de la boite document */
	     if (!pBo1->BxContentWidth
		 || (!pPa1->AbWidth.DimIsPosition && pPa1->AbWidth.DimMinimum))
	       {
		  /* la dimension de la boite depend de la fenetre */
		  condition = Dimensionner (pPa1, frame, True);
		  /* S'il y a une regle de minimum il faut la verifier */
		  if (!condition)
		    {
		       EvalComp (pPa1, frame, &large, &haut);
		       ChangeLgContenu (pBo1, pBo1, large, 0, frame);
		    }
	       }
	     if (!pBo1->BxContentHeight
		 || (!pPa1->AbHeight.DimIsPosition && pPa1->AbHeight.DimMinimum))
	       {
		  /* la dimension de la boite depend de la fenetre */
		  condition = Dimensionner (pPa1, frame, False);
		  /* S'il y a une regle de minimum il faut la verifier */
		  if (!condition)
		    {
		       EvalComp (pPa1, frame, &large, &haut);
		       ChangeHtContenu (pBo1, pBo1, haut, frame);
		    }
	       }

	     /* Faut-il deplacer la boite document dans la fenetre? */
	     Positionner (pPa1->AbHorizPos, pBo1, frame, True);
	     Positionner (pPa1->AbVertPos, pBo1, frame, False);

	     /* On elimine le scroll horizontal */
	     DimFenetre (frame, &large, &haut);
	     if (pFe1->FrXOrg != 0)
	       {
		  pFe1->FrXOrg = 0;
		  /* Force le reaffichage */
		  DefClip (frame, 0, 0, large, haut);
	       }
	     /* Si la vue n'est pas coupee en tete on elimine le scroll vertical */
	     if (!pPa1->AbTruncatedHead && pFe1->FrYOrg != 0)
	       {
		  pFe1->FrYOrg = 0;
		  /* Force le reaffichage */
		  DefClip (frame, 0, 0, large, haut);
	       }
	     pFe1->FrReady = savepret;	/* La frame est affichable */

	     /* Traitement des englobements retardes */
	     TraiteEnglobement (frame);

	     /* Retire la selection eventuelle */
	     SetSelect (frame, False);
	     /* Affichage de toute la fenetre */
	     AfFinFenetre (frame, 0);
	     /* Restaure la selection */
/**MIN*/ VisuSelect (pPa1, False);
	     /*else
	        SetSelect(frame, True); */

	  }
     }
}				/* ModFenetre */

/* ---------------------------------------------------------------------- */
/* |    RazVue libere toutes les boites de la vue dont on donne         | */
/* |            le pave racine.                                         | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                RazVue (int frame)

#else  /* __STDC__ */
void                RazVue (frame)
int                 frame;

#endif /* __STDC__ */

{
   ViewFrame            *pFe1;
   ViewFrame            *pFe2;

   pFe1 = &FntrTable[frame - 1];
   if (pFe1->FrAbstractBox != NULL)
     {
	pFe1->FrReady = False;	/* La frame n'est pas affichable */
	/* Faut-il retirer les marques de selection dans la fenetre */
	EndInsert ();
	ResetSelect (frame);
	/* Liberation de la hierarchie */
	DispBoite (pFe1->FrAbstractBox, False, frame);
	pFe1->FrAbstractBox = NULL;
	pFe1->FrReady = True;	/* La frame est affichable */
	DefClip (frame, -1, -1, -1, -1);	/* effacer effectivement */
	pFe2 = &FntrTable[frame - 1];
	SetClip (frame, pFe2->FrXOrg, pFe2->FrYOrg, &pFe2->FrClipXBegin, &pFe2->FrClipYBegin,
		 &pFe2->FrClipXEnd, &pFe2->FrClipYEnd, 1);
     }
}				/* RazVue */

/* ---------------------------------------------------------------------- */
/* |    RecursChange rend la valeur Vrai si les changements ont une     | */
/* |            re'percution sur le pave englobant.                     | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static boolean      RecursChange (PtrAbstractBox adpave, int frame)

#else  /* __STDC__ */
static boolean      RecursChange (adpave, frame)
PtrAbstractBox             adpave;
int                 frame;

#endif /* __STDC__ */

{
   PtrAbstractBox             pavefils;
   PtrAbstractBox             pavprem;
   PtrAbstractBox             premcree;
   PtrLine            premligne;
   PtrBox            box;
   boolean             mortcree;
   boolean             resultat;
   boolean             change;
   PtrAbstractBox             pPa1;
   PtrAbstractBox             pPa2;
   PtrBox            pBo1;
   Propagation      savpropage;

   pPa1 = adpave;
   change = False;
   if (pPa1->AbDead && (pPa1->AbNew || pPa1->AbBox == NULL))
      resultat = False;
   else if (pPa1->AbDead || pPa1->AbNew)
     {
	resultat = EvalChange (adpave, frame);
	savpropage = Propage;
	Propage = ToAll;	/* On passe en mode normal de propagation */
	VerifPos (adpave, frame);
	Propage = savpropage;
     }
   /* On traite tous les paves descendants modifies avant le pave lui-meme */
   else
     {
	/* On limite l'englobement a la boite courante */
	box = Englobement;
	Englobement = adpave->AbBox;

	/* Traitement des creations */
	pavefils = pPa1->AbFirstEnclosed;
	premcree = NULL;
	mortcree = False;
	while (pavefils != NULL)
	  {
	     if (pavefils->AbNew)
	       {
		  /* On marque le pave qui vient d'etre cree */
		  /* pour eviter sa reinterpretation         */
		  pavefils->AbNum = 1;
		  change = RecursChange (pavefils, frame);
		  /* Une modification a repercurter sur l'englobante */
		  if (change && !mortcree)
		    {
		       mortcree = True;
		       premcree = pavefils;
		    }
	       }
	     else
		pavefils->AbNum = 0;
	     pavefils = pavefils->AbNext;
	  }

	/* Traitement des autres modifications */
	pavefils = pPa1->AbFirstEnclosed;
	pavprem = NULL;
	while (pavefils != NULL)
	  {
	     /* On evite la reinterpretation des paves crees */
	     if (pavefils->AbNum == 0)
		change = RecursChange (pavefils, frame);

	     /* On enregistre le premier pave cree ou modifie */
	     if (premcree == pavefils && pavprem == NULL)
		pavprem = premcree;
	     else if (change)
	       {
		  /* Une modification a repercuter sur l'englobante */
		  mortcree = True;
		  if (pavprem == NULL)
		     pavprem = pavefils;
	       }

	     pavefils = pavefils->AbNext;
	  }
	/* Les liens entre boites filles ont peut-etre ete modifies */
	if (mortcree)
	  {
	     pavefils = pPa1->AbFirstEnclosed;
	     while (pavefils != NULL)
	       {
		  pPa2 = pavefils;
		  if (pPa2->AbBox != NULL)
		    {
		       pBo1 = pPa2->AbBox;
		       pBo1->BxHorizInc = NULL;
		       pBo1->BxVertInc = NULL;
		    }

		  pavefils = pPa2->AbNext;
	       }
	  }

	/* On restaure l'ancienne limite de l'englobement */
	Englobement = box;

	/* Faut-il reevaluer la boite composee ? */
	resultat = EvalChange (adpave, frame);
	if (mortcree || resultat)
	  {
	     pBo1 = pPa1->AbBox;
	     resultat = True;
	     /* Mise a jour d'un bloc de lignes */
	     if (pBo1->BxType == BoBlock && pavprem != NULL)
	       {
		  if (pavprem == premcree || pavprem->AbDead)
		    {
		       /* On prend la derniere boite avant */
		       pavprem = pavprem->AbPrevious;
		       if (pavprem == NULL)
			  premligne = NULL;	/* toutes les lignes */
		       else
			 {
			    box = pavprem->AbBox;
			    if (box != NULL)
			      {
				 /* S'il s'agit d'une boite de texte coupee */
				 /* on recherche la derniere boite de texte */
				 if (box->BxType == BoSplit)
				    while (box->BxNexChild != NULL)
				       box = box->BxNexChild;
				 premligne = DesLigne (box);
			      }
			    else
			       premligne = NULL;
			 }

		    }
		  else
		    {
		       /* On prend la derniere boite avant */
		       box = pavprem->AbBox;
		       if (box != NULL)
			 {
			    if (box->BxType == BoSplit && box->BxNexChild != NULL)
			       box = box->BxNexChild;
			    premligne = DesLigne (box);
			 }
		       else
			  premligne = NULL;
		    }

		  ReevalBloc (adpave, premligne, pPa1->AbBox, frame);
	       }
	     /* Mise a jour d'une boite composee */
	     else if (!pPa1->AbInLine
		      && adpave->AbBox->BxType != BoGhost
		      && adpave->AbLeafType == LtCompound)
	       {
		  Englobx (adpave, pPa1->AbBox, frame);
		  Engloby (adpave, pPa1->AbBox, frame);
	       }
	  }
     }

   /* On signale s'il y a eu modification du pave */

   return resultat;
}				/* RecursChange */

/* ---------------------------------------------------------------------- */
/* |    RazElasticite annule l'e'lasticite' de la boite s'il s'agit     | */
/* |            d'une boite elastique et les regles de position et de   | */
/* |            dimension sont a` reevaluer. Traite recusivement        | */
/* |            les paves fils.                                         | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void         RazElasticite (PtrAbstractBox adpave, int frame)

#else  /* __STDC__ */
static void         RazElasticite (adpave, frame)
PtrAbstractBox             adpave;
int                 frame;

#endif /* __STDC__ */

{
   PtrAbstractBox             pavefils;
   PtrBox            pBo1;

   if (adpave->AbNew)
      return;
   else if (adpave->AbBox != NULL)
     {
	pBo1 = adpave->AbBox;
	/* Faut-il reevaluer les regles d'une boite elastique */
	if (pBo1->BxHorizFlex && adpave->AbWidthChange)
	  {
	     DefClip (frame, pBo1->BxXOrg, pBo1->BxYOrg,
		  pBo1->BxXOrg + pBo1->BxWidth, pBo1->BxYOrg + pBo1->BxHeight);

	     /* Annulation de la position */
	     if (adpave->AbHorizPosChange)
	       {
		  RazPosition (pBo1, True);
		  DepOrgX (pBo1, NULL, -pBo1->BxXOrg, frame);
	       }

	     /* Annulation ancienne largeur */
	     RazDim (pBo1, True, frame);
	     /* Reinitialisation du trace reel */
/**PL*/ AjusteTrace (adpave, pBo1->BxHorizInverted, pBo1->BxVertInverted, False);
	  }

	if (pBo1->BxVertFlex && adpave->AbHeightChange)
	  {
	     if (!pBo1->BxHorizFlex || !adpave->AbWidthChange)
		DefClip (frame, pBo1->BxXOrg, pBo1->BxYOrg,
		  pBo1->BxXOrg + pBo1->BxWidth, pBo1->BxYOrg + pBo1->BxHeight);
	     /* Annulation et reevaluation de la position */
	     if (adpave->AbVertPosChange)
	       {
		  RazPosition (pBo1, False);
		  DepOrgY (pBo1, NULL, -pBo1->BxYOrg, frame);
	       }

	     /* Annulation ancienne hauteur */
	     RazDim (pBo1, False, frame);
	     /* Reinitialisation du trace reel */
/**PL*/ AjusteTrace (adpave, pBo1->BxHorizInverted, pBo1->BxVertInverted, False);
	  }

	/* Traitement des paves fils */
	pavefils = adpave->AbFirstEnclosed;
	while (pavefils != NULL)
	  {
	     RazElasticite (pavefils, frame);
	     pavefils = pavefils->AbNext;
	  }
     }
}				/*RazElasticite */

/* ---------------------------------------------------------------------- */
/* |    ModifVue traite la mise a` jour d'une hierachie de paves.       | */
/* |            Pendant la creation d'une arborescence de boites on     | */
/* |            place chaque boite a` l'interieur de son englobante     | */
/* |            (Propage=ToSiblings).                                  | */
/* |            Quand tous les placements relatifs sont termine's on    | */
/* |            place les boites dans la vue entiere du document en     | */
/* |            cumulant les placements relatifs.                       | */
/* |            Les autre modifications vont provoquer des mises a` jour| */
/* |            sur la descendance (Propage = ToChildren) puis en fin de | */
/* |            traitement seulement, la mise a` jour des boites        | */
/* |            englobantes.                                            | */
/* |            En fonctionnement normal les modifications sont         | */
/* |            immediatement propagees sur toutes les boites           | */
/* |            (Propage = ToAll).                                    | */
/* |            Si le parametre page est non nul, il indique la hauteur | */
/* |            maximum d'une page (ce parametre est eventuellement mis | */
/* |            a` jour au retour).                                     | */
/* |            La fonction ve'rifie que cette limite est respecte'e.   | */
/* |            Elle rend la valeur Vrai si l'image n'est pas coupee.   | */
/* |            Si le retour est Faux, tous les paves dont la boite     | */
/* |            coupe la limite ou de'borde sont marque's.              | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
boolean             ModifVue (int frame, int *page, PtrAbstractBox Pv)

#else  /* __STDC__ */
boolean             ModifVue (frame, page, Pv)
int                 frame;
int                *page;
PtrAbstractBox             Pv;

#endif /* __STDC__ */

{
   boolean             change;
   PtrAbstractBox             pavepere;
   PtrAbstractBox             pavefils;
   PtrLine            adligne;
   PtrBox            box;
   ViewFrame            *pFe1;
   PtrAbstractBox             pPa1;
   PtrBox            pBo1;
   PtrAbstractBox             pPa2;
   PtrBox            pBo2;
   boolean             result;
   int                 savevisu = 0;
   int                 savezoom = 0;

/**************** test VQ **************
  MotifLister(2);
  TtaWaitShowDialogue();
***************************************/
   result = True;
   adligne = NULL;
   /* Pas de pave en parametre */
   if (Pv == NULL)
      TtaDisplaySimpleMessage (LIB, INFO, LIB_VIEW_IS_EMPTY);
   /* Le numero de frame est erronne */
   else if (frame < 1 || frame > MAX_FRAME)
      TtaDisplaySimpleMessage (LIB, INFO, LIB_INCOR_WINDOW_NUMBER);
   else
     {
	pFe1 = &FntrTable[frame - 1];
	pPa1 = Pv;
	/* La vue n'est pas cree a la racine */
	if (pFe1->FrAbstractBox == NULL && (pPa1->AbEnclosing != NULL
		 || pPa1->AbPrevious != NULL || pPa1->AbNext != NULL))
	   TtaDisplaySimpleMessage (LIB, INFO, LIB_VIEW_IS_MODIFIED_BEFORE_CREA);
	/* On detruit toute la vue */
	else if (pPa1->AbEnclosing == NULL && pPa1->AbDead)
	  {
	     if (pPa1 == pFe1->FrAbstractBox)
		RazVue (frame);
	     else
		TtaDisplaySimpleMessage (LIB, INFO, LIB_VIEW_IS_MODIFIED_BEFORE_CREA);
	  }
	/* La vue est deja cree */
	else if (pFe1->FrAbstractBox != NULL && pPa1->AbEnclosing == NULL
		 && pPa1->AbNew)
	   TtaDisplaySimpleMessage (LIB, INFO, LIB_NEW_VIEW_CANNOT_REPLACE_THE_OLD_ONE);
	/* Dans les autres cas */
	else
	  {
	     /* Traitement de la premiere creation */
	     if (pFe1->FrAbstractBox == NULL)
	       {
		  DefClip (frame, 0, 0, 0, 0);
		  pFe1->FrXOrg = 0;
		  pFe1->FrYOrg = 0;
		  pFe1->FrAbstractBox = Pv;
		  pFe1->FrSelectOneBox = False;
		  pFe1->FrSelectionBegin.VsBox = NULL;
		  pFe1->FrSelectionEnd.VsBox = NULL;
		  pFe1->FrReady = True;
		  pFe1->FrSelectShown = False;
	       }

	     /* On prepare le traitement de l'englobement apres modification */
	     pFe1->FrReady = False;	/* La frame n'est pas affichable */
	     if (*page != 0)
		/* changement de la signification de la valeur de page */
		/* si negatif : mode pagination sans evaluation de coupure */
		/* si egal 0 : pas mode pagination */
		/* si superieur a 0 : mode pagination et coupure demandee */
	       {
		  /* La pagination ignore le zoom et la visibilite courants */
		  savevisu = pFe1->FrVisibility;
		  pFe1->FrVisibility = 5;
		  savezoom = pFe1->FrMagnification;
		  pFe1->FrMagnification = 0;
	       }
	     Propage = ToChildren;	/* Limite la propagation */
	     /* On note le premier pave a examiner pour l'englobante */
	     pavepere = pPa1->AbEnclosing;

	     /* On prepare la mise a jour d'un bloc de lignes */
	     if ((pavepere != NULL) && (pavepere->AbBox != NULL))
	       {
		  /* L'englobement doit etre repris a partir de boite du pave pere */
		  Englobement = pavepere->AbBox;

		  /* On est dans une boite eclatee, on remonte jusqu'au bloc de lignes */
		  while (pavepere->AbBox->BxType == BoGhost)
		     pavepere = pavepere->AbEnclosing;

		  /* On prepare la mise a jour d'un bloc de lignes */
		  if (pavepere->AbInLine
		      || pavepere->AbBox->BxType == BoGhost)
		     if (Pv->AbNew || Pv->AbDead)
		       {
			  /* Il faut refaire la mise en lignes sur la boite qui precede */
			  pavefils = Pv->AbPrevious;
			  if (pavefils == NULL)
			     adligne = NULL;	/* toutes les lignes */
			  else
			    {
			       box = pavefils->AbBox;
			       if (box->BxType == BoSplit)
				  while (box->BxNexChild != NULL)
				     box = box->BxNexChild;
			       adligne = DesLigne (box);
			    }
		       }
		     else
		       {
			  /* Il faut refaire la mise en lignes sur la premiere boite contenue */
			  box = Pv->AbBox;
			  if (box->BxType == BoSplit)
			     box = box->BxNexChild;
			  adligne = DesLigne (box);
		       }
	       }

	     /* Il faut annuler l'elasticite des boites dont les regles */
	     /* de position et de dimension sont a reevaluer, sinon on  */
	     /* risque d'alterer les nouvelles regles de position et de */
	     /* dimension en propageant les autres modifications sur    */
	     /* ces anciennes boites elastiques de l'image abstraite    */
	     RazElasticite (Pv, frame);

	     /* On traite toutes les modifications signalees */
	     change = RecursChange (Pv, frame);

	     /* Les modifications sont traitees */
	     Propage = ToAll;	/* On passe en mode normal de propagation */

	     /* Traitement des englobements retardes */
	     TraiteEnglobement (frame);


	     /* On ne limite plus le traitement de l'englobement */
	     Englobement = NULL;

	     /* Si l'image concrete est videe de son contenu */
	     if (pFe1->FrAbstractBox->AbBox->BxNext == NULL)
	       {
		  /* On annule le decalage de la fenetre dans l'image concrete */
		  pFe1->FrXOrg = 0;
		  pFe1->FrYOrg = 0;
	       }
	     /* Faut-il verifier l'englobement ? */
	     if (change && pavepere != NULL)
	       {
		  /* On saute les boites fantomes de la mise en lignes */
		  if (pavepere->AbBox->BxType == BoGhost)
		    {
		       while (pavepere->AbBox->BxType == BoGhost)
			  pavepere = pavepere->AbEnclosing;
		       adligne = pavepere->AbBox->BxFirstLine;
		    }

		  pBo1 = pavepere->AbBox;
		  /* Mise a jour d'un bloc de lignes */
		  if (pBo1->BxType == BoBlock)
		     ReevalBloc (pavepere, adligne, NULL, frame);

		  /* Mise a jour d'une boite composee */
		  else
		    {
		       /* Les liens entre boites filles ont peut-etre ete modifies */
		       pavefils = pPa1->AbFirstEnclosed;
		       while (pavefils != NULL)
			 {
			    pPa2 = pavefils;
			    if (pPa2->AbBox != NULL)
			      {
				 pBo2 = pPa2->AbBox;
				 pBo2->BxHorizInc = NULL;
				 pBo2->BxVertInc = NULL;
			      }
			    pavefils = pPa2->AbNext;
			 }

		       Englobx (pavepere, NULL, frame);
		       Engloby (pavepere, NULL, frame);
		    }
	       }
	     /* Est-ce que l'on a de nouvelles boites dont le contenu est */
	     /* englobe et depend de relations hors-structure ?           */
	     TraiteEnglobement (frame);
	     /* Verification de la mise en page */
	     if (*page > 0)	/* changement de la signification de page */
		/* si superieur a 0 : mode pagination et coupure demandee */
		result = MarqueCoupure (pFe1->FrAbstractBox, page);
	     if (*page != 0)
	       {
		  /* retablit le zoom et la visibilite courants */
		  pFe1->FrVisibility = savevisu;
		  pFe1->FrMagnification = savezoom;
	       }
	     pFe1->FrReady = True;	/* La frame est affichable */
	  }
     }
   return result;
}				/* ModifVue */

/* End Of Module img */
