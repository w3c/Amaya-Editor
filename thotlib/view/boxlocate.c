
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

/* 
   des.c : gestion des designations de boites.
   I. Vatton - Mars 85
   IV : Juin 93 polylines
 */

#include "libmsg.h"
#include "thot_sys.h"
#include "functions.h"
#include "constmedia.h"
#include "typemedia.h"
#include "message.h"
#include "appdialogue.h"

#define EXPORT extern
#include "img.var"
#include "frame.var"
#include "environ.var"
#include "appdialogue.var"

#include "appli.f"
#include "creation.f"
#include "commun.f"
#include "dep.f"
#include "des.f"
#include "docvues.f"
#include "appexec.f"
#include "environ.f"
#include "font.f"
#include "geom.f"
#include "imabs.f"
#include "img.f"
#include "lig.f"
#include "modpres.f"
#include "sel.f"

#ifdef WWW_MSWINDOWS		/* map to MSVC library system calls */
#include <math.h>
#endif /* WWW_MSWINDOWS */

#define YFACTOR 200		/* penalisation en Y */
#define ASIZE 3			/* taille des ancres */
#define MAXLINE 500
#define MAXVERTS 100

#ifdef __STDC__
extern boolean      ExceptTypeElem (int, int, PtrSSchema);
extern void         DefClip (int, int, int, int, int);
extern boolean      AfFinFenetre (int, int);

#else  /* __STDC__ */
extern boolean      ExceptTypeElem ();
extern void         DefClip ();
extern boolean      AfFinFenetre ();

#endif /* __STDC__ */

/* ---------------------------------------------------------------------- */
/* |    ecrete rend 0 si val dans l'intervalle [-limit,+limit] et sinon | */
/* |            abs(val)-limit. Avec limit = 0, donne la valeur absolue.| */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static int          ecrete (int val, int limit)
#else  /* __STDC__ */
static int          ecrete (val, limit)
int                 val;
int                 limit;

#endif /* __STDC__ */
{
   if (val > limit)
      return (val - limit);
   else if (val < -limit)
      return (-val - limit);
   else
      return (0);
}

/* ---------------------------------------------------------------------- */
/* |    DistAncre calcule la distance d'une ancre x,y au point          | */
/* |            Xpoint,Ypoint. La fonction rend la distance minimale    | */
/* |            entre la distance calcule'e et dist.                    | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static int          DistAncre (int Xpoint, int Ypoint, int x, int y, int dist)
#else  /* __STDC__ */
static int          DistAncre (Xpoint, Ypoint, x, y, dist)
int                 Xpoint;
int                 Ypoint;
int                 x;
int                 y;
int                 dist;

#endif /* __STDC__ */
{
   int                 d;

   d = ecrete (x - Xpoint, ASIZE - 1) + YFACTOR * ecrete (y - Ypoint, ASIZE - 1);
   if (d < dist)
      return (d);
   else
      return (dist);
}

/* ---------------------------------------------------------------------- */
/* |    DistBox calcule la distance d'un point x, y a` une boi^te non   | */
/* |            graphique. Si la boi^te contient du texte, on favorise  | */
/* |            la distance en X.                                       | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
int                 DistBox (int x, int y, int X, int Y, int W, int H)
#else  /* __STDC__ */
int                 DistBox (x, y, X, Y, W, H)
int                 x;
int                 y;
int                 X;
int                 Y;
int                 W;
int                 H;

#endif /* __STDC__ */
{
   int                 d;

   /* centrer la boite */
   W /= 2;
   X += W;
   H /= 2;
   Y += H;

   d = ecrete (x - X, W) + YFACTOR * ecrete (y - Y, H);
   return (d);
}

/* ---------------------------------------------------------------------- */
/* |    DistGraphique calcule la distance d'un point Xpoint,Ypoint a`   | */
/* |            un point de la boite graphique (min des distances aux   | */
/* |            ancres).                                                | */
/* |            Cette selection est limitee aux boites terminale.       | */
/* |            Rend la distance de la boite au point.                  | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
int                 DistGraphique (int Xpoint, int Ypoint, PtrBox ibox, int val)
#else  /* __STDC__ */
int                 DistGraphique (Xpoint, Ypoint, ibox, val)
int                 Xpoint;
int                 Ypoint;
PtrBox            ibox;
int                 val;

#endif /* __STDC__ */
{
   int                 distance;
   int                 X, Y, W, H;

   /* centrer la boite */
   W = ibox->BxWidth / 2;
   X = ibox->BxXOrg + W;
   H = ibox->BxHeight / 2;
   Y = ibox->BxYOrg + H;
   distance = 1000;

   switch (val)
	 {
	    case 1:		/* racine .. */
	       distance = DistAncre (Xpoint, Ypoint, X - W + H / 3, Y + H, distance);
	       distance = DistAncre (Xpoint, Ypoint, X - W + (2 * H) / 3, Y - H, distance);
	       distance = DistAncre (Xpoint, Ypoint, X + W, Y - H, distance);
	       break;

	    case 'c':		/* cercle */
	       if (W < H)
		  H = W;
	       else
		  W = H;
	       /* ATTENTION: on continue en sequence, */
	       /* vu qu'un cercle est aussi une ellipse */
	    case 'C':		/* ellipse */
	       distance = DistAncre (Xpoint, Ypoint, X - W, Y, distance);
	       distance = DistAncre (Xpoint, Ypoint, X + W, Y, distance);
	       distance = DistAncre (Xpoint, Ypoint, X, Y - H, distance);
	       distance = DistAncre (Xpoint, Ypoint, X, Y + H, distance);
	       H = (71 * H) / 100;
	       W = (71 * W) / 100;
	       distance = DistAncre (Xpoint, Ypoint, X - W, Y + H, distance);
	       distance = DistAncre (Xpoint, Ypoint, X - W, Y - H, distance);
	       distance = DistAncre (Xpoint, Ypoint, X + W, Y + H, distance);
	       distance = DistAncre (Xpoint, Ypoint, X + W, Y - H, distance);
	       break;

	    case 'L':		/* losange */
	       distance = DistAncre (Xpoint, Ypoint, X - W, Y, distance);
	       distance = DistAncre (Xpoint, Ypoint, X + W, Y, distance);
	       distance = DistAncre (Xpoint, Ypoint, X, Y - H, distance);
	       distance = DistAncre (Xpoint, Ypoint, X, Y + H, distance);
	       break;

	    case 't':		/* trait horiz en haut */
	       distance = DistAncre (Xpoint, Ypoint, X - W, Y - H, distance);
	       distance = DistAncre (Xpoint, Ypoint, X + W, Y - H, distance);
	       distance = DistAncre (Xpoint, Ypoint, X, Y - H, distance);
	       break;

	    case 'b':		/* trait horiz en bas */
	       distance = DistAncre (Xpoint, Ypoint, X - W, Y + H, distance);
	       distance = DistAncre (Xpoint, Ypoint, X + W, Y + H, distance);
	       distance = DistAncre (Xpoint, Ypoint, X, Y + H, distance);
	       break;

	    case 'h':		/* trait horizontal centre ou fleche */
	    case '<':
	    case '>':
	       distance = DistAncre (Xpoint, Ypoint, X - W, Y, distance);
	       distance = DistAncre (Xpoint, Ypoint, X + W, Y, distance);
	       distance = DistAncre (Xpoint, Ypoint, X, Y, distance);
	       break;

	    case 'v':
	    case 'V':		/* trait vertical centre ou fleche ^ */
	    case '^':
	       distance = DistAncre (Xpoint, Ypoint, X, Y - H, distance);
	       distance = DistAncre (Xpoint, Ypoint, X, Y + H, distance);
	       distance = DistAncre (Xpoint, Ypoint, X, Y, distance);
	       break;

	    case 'l':		/* trait vertical gauche */
	       distance = DistAncre (Xpoint, Ypoint, X - W, Y - H, distance);
	       distance = DistAncre (Xpoint, Ypoint, X - W, Y + H, distance);
	       distance = DistAncre (Xpoint, Ypoint, X - W, Y, distance);
	       break;
	    case 'r':		/* trait vertical droit */
	       distance = DistAncre (Xpoint, Ypoint, X + W, Y - H, distance);
	       distance = DistAncre (Xpoint, Ypoint, X + W, Y + H, distance);
	       distance = DistAncre (Xpoint, Ypoint, X + W, Y, distance);
	       break;

	    case '/':		/* diagonale ou fleche vers le haut */
	    case 'E':
	    case 'o':
	       distance = DistAncre (Xpoint, Ypoint, X - W, Y + H, distance);
	       distance = DistAncre (Xpoint, Ypoint, X + W, Y - H, distance);
	       distance = DistAncre (Xpoint, Ypoint, X, Y, distance);
	       break;

	    case '\\':		/* diagonale ou fleche vers le bas */
	    case 'e':
	    case 'O':
	       distance = DistAncre (Xpoint, Ypoint, X - W, Y - H, distance);
	       distance = DistAncre (Xpoint, Ypoint, X + W, Y + H, distance);
	       distance = DistAncre (Xpoint, Ypoint, X, Y, distance);
	       break;
	    default:		/* rectangle ou autre */
	       distance = DistAncre (Xpoint, Ypoint, X - W, Y + H, distance);
	       distance = DistAncre (Xpoint, Ypoint, X - W, Y - H, distance);
	       distance = DistAncre (Xpoint, Ypoint, X + W, Y + H, distance);
	       distance = DistAncre (Xpoint, Ypoint, X + W, Y - H, distance);
	       break;
	 }
   return (distance);
}				/*DistGraphique */

/* ---------------------------------------------------------------------- */
/* |    pointOnLine teste qu'un point x,y est sur un segment P1(x1,y1)  | */
/* |            P2(x2,y2) avec une certaine precision DELTA_SEL.         | */
/* |            On teste l'appartenance du point a` un rectangle cree   | */
/* |            par e'largissement de DELTA_SEL autour des deux points.  | */
/* |            Le test est fait apres une rotation permettant de se    | */
/* |            ramener a` un rectangle horizontal.                     | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static boolean      pointOnLine (int x, int y, int x1, int y1, int x2, int y2)
#else  /* __STDC__ */
static boolean      pointOnLine (x, y, x1, y1, x2, y2)
int                 x;
int                 y;
int                 x1;
int                 y1;
int                 x2;
int                 y2;

#endif /* __STDC__ */
{
   int                 dX, dY, nX, nY;
   double              R, C, S;

   x -= x1;
   y -= y1;			/* translation */
   dX = x2 - x1;
   dY = y2 - y1;
   /* rotation ramenant le 2e point sur l'horizontale */
   R = sqrt ((double) dX * dX + dY * dY);
   if (R == 0.0)
      return False;
   C = dX / R;
   S = dY / R;
   nX = x * C + y * S;
   nY = y * C - x * S;
   /* test */
   return (nY <= DELTA_SEL
	   && nY >= -DELTA_SEL
	   && nX >= -DELTA_SEL
	   && nX <= R + DELTA_SEL);
}


/* ---------------------------------------------------------------------- */
/* |    DansLaPolyLine indique si le point x,y se trouve a`             | */
/* |     l'inte'rieur de la polyline ou non.                            | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static boolean      DansLaPolyLine (PtrAbstractBox pave, int x, int y)

#else  /* __STDC__ */
static boolean      DansLaPolyLine (pave, x, y)
PtrAbstractBox             pave;
int                 x;
int                 y;

#endif /* __STDC__ */
{

   PtrTextBuffer      buff, maxbuff;
   int                 croise;
   int                 i, max;
   int                 prevX, prevY;
   int                 nextX, nextY;
   PtrBox            box;
   boolean             OK;

   box = pave->AbBox;
   x -= box->BxXOrg;
   y -= box->BxYOrg;
   max = box->BxNChars;
   if (max < 4)
      /* il n'y a pas de surface a l'interieur de la polyline */
      return (False);

   /* premier et dernier point de la polyline */
   maxbuff = buff = box->BxBuffer;
   i = 1;
   while (maxbuff->BuNext != NULL)
      maxbuff = maxbuff->BuNext;
   max = maxbuff->BuLength - 1;

   croise = 0;
   nextX = PtEnPixel (buff->BuPoints[i].XCoord / 1000, 0);
   nextY = PtEnPixel (buff->BuPoints[i].YCoord / 1000, 1);
   prevX = PtEnPixel (maxbuff->BuPoints[max].XCoord / 1000, 0);
   prevY = PtEnPixel (maxbuff->BuPoints[max].YCoord / 1000, 1);
   if ((prevY >= y) != (nextY >= y))
     {
	/* y entre nextY et prevY */
	if ((OK = (prevX >= x)) == (nextX >= x))
	  {
	     /* x du meme cote des deux extremites */
	     if (OK)
		/* nextX et prevX >= x */
		croise++;
	  }
	else
	   /* x entre deux extremites */
	   croise += (prevX - (prevY - y) * (nextX - prevX) / (nextY - prevY)) >= x;
     }

   i++;
   while (i <= max || buff != maxbuff)
     {
	prevX = nextX;
	prevY = nextY;
	nextX = PtEnPixel (buff->BuPoints[i].XCoord / 1000, 0);
	nextY = PtEnPixel (buff->BuPoints[i].YCoord / 1000, 1);
	if (prevY >= y)
	  {
	     while ((i <= max || buff != maxbuff) && (nextY >= y))
	       {
		  i++;		/* changement de point */
		  if (i >= buff->BuLength && buff != maxbuff)
		    {
		       buff = buff->BuNext;	/* passe au buffer suivant */
		       i = 0;
		    }
		  prevY = nextY;
		  prevX = nextX;
		  nextX = PtEnPixel (buff->BuPoints[i].XCoord / 1000, 0);
		  nextY = PtEnPixel (buff->BuPoints[i].YCoord / 1000, 1);
	       }

	     if (i > max && buff == maxbuff)
		break;

	     if ((OK = (prevX >= x)) == (nextX >= x))
	       {
		  if (OK)
		     croise++;
	       }
	     else
		croise += (prevX - (prevY - y) * (nextX - prevX) / (nextY - prevY)) >= x;
	  }
	else
	  {
	     while ((i <= max || buff != maxbuff) && (nextY < y))
	       {
		  i++;		/* changement de point */
		  if (i >= buff->BuLength && buff != maxbuff)
		    {
		       buff = buff->BuNext;	/* passe au buffer suivant */
		       i = 0;
		    }
		  prevY = nextY;
		  prevX = nextX;
		  nextX = PtEnPixel (buff->BuPoints[i].XCoord / 1000, 0);
		  nextY = PtEnPixel (buff->BuPoints[i].YCoord / 1000, 1);
	       }

	     if (i > max && buff == maxbuff)
		break;

	     if ((OK = (prevX >= x)) == (nextX >= x))
	       {
		  if (OK)
		     croise++;
	       }
	     else
		croise += (prevX - (prevY - y) * (nextX - prevX) / (nextY - prevY)) >= x;
	  }
     }
   OK = (boolean) (croise & 0x01);
   return (OK);
}


/* ---------------------------------------------------------------------- */
/* |    SurLaPolyLine teste qu'un point x,y est sur un segment de la    | */
/* |            boi^te polyline.                                        | */
/* |    Si oui, retourne l'adresse de la boi^te correspondante et le    | */
/* |    point de contro^le se'lectionne' (0 pour toute la boi^te).      | */
/* |    sinon, la valeur NULL.                                          | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static PtrBox     SurLaPolyLine (PtrAbstractBox pave, int x, int y, int *pointselect)
#else  /* __STDC__ */
static PtrBox     SurLaPolyLine (pave, x, y, pointselect)
PtrAbstractBox             pave;
int                 x;
int                 y;
int                *pointselect;

#endif /* __STDC__ */
{
   int                 i, j, nb;
   int                 X1, Y1;
   int                 X2, Y2;
   PtrTextBuffer      adbuff;
   PtrBox            box;
   boolean             OK;

   Y1 = 0;
   box = pave->AbBox;
   nb = box->BxNChars;
   if (nb < 3)
      /* il n'y a pas au moins un segment defini */
      return (NULL);

   /* On calcule le point de controle de la polyline le plus proche */
   adbuff = box->BxBuffer;
   x -= box->BxXOrg;
   y -= box->BxYOrg;
   X1 = -1;			/* Pas de point X1, Y1 au depart */
   X2 = -1;
   *pointselect = 0;
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

	/* Teste si le point est sur ce segment */
	X2 = PtEnPixel (adbuff->BuPoints[j].XCoord / 1000, 0);
	Y2 = PtEnPixel (adbuff->BuPoints[j].YCoord / 1000, 1);
	if (x >= X2 - DELTA_SEL && x <= X2 + DELTA_SEL && y >= Y2 - DELTA_SEL && y <= Y2 + DELTA_SEL)
	  {
	     /* La selection porte sur un point de controle particulier */
	     *pointselect = i;
	     return (box);
	  }
	else if (X1 == -1)
	   OK = False;
	else
	   OK = pointOnLine (x, y, X1, Y1, X2, Y2);

	if (OK)
	   /* Le point est sur ce segment -> le test est fini */
	   return (box);
	else
	  {
	     j++;
	     X1 = X2;
	     Y1 = Y2;
	  }
     }
   /* traite le cas particulier des polylines fermees */
   if (pave->AbPolyLineShape == 'p' || pave->AbPolyLineShape == 's')
     {
	X2 = PtEnPixel (box->BxBuffer->BuPoints[1].XCoord / 1000, 0);
	Y2 = PtEnPixel (box->BxBuffer->BuPoints[1].YCoord / 1000, 1);
	OK = pointOnLine (x, y, X1, Y1, X2, Y2);
	if (OK)
	   return (box);
     }
   return (NULL);
}

/* ---------------------------------------------------------------------- */
/* |    DansLeGraphique indique si le point x,y se trouve a`            | */
/* |     l'inte'rieur du graphique ou non.                              | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static boolean      DansLeGraphique (PtrAbstractBox pave, int x, int y)
#else  /* __STDC__ */
static boolean      DansLeGraphique (pave, x, y)
PtrAbstractBox             pave;
int                 x;
int                 y;

#endif /* __STDC__ */
{
   int                 point[8][2];
   int                 croise;
   int                 i, max;
   int                 prevX, prevY;
   int                 nextX, nextY;
   int                 arc;
   float               val1, val2;
   PtrBox            box;
   boolean             OK;

   box = pave->AbBox;
   x -= box->BxXOrg;
   y -= box->BxYOrg;
   max = 0;

   /* Est-ce un point caracteristique specifique du graphique ? */
   switch (pave->AbRealShape)
	 {
	    case ' ':
	    case 'R':
	    case '0':
	    case '1':
	    case '2':
	    case '3':
	    case '4':
	    case '5':
	    case '6':
	    case '7':
	    case '8':		/* rectangles */
	       point[0][0] = 0;
	       point[0][1] = 0;
	       point[1][0] = 0;
	       point[1][1] = box->BxHeight;
	       point[2][0] = box->BxWidth;
	       point[2][1] = box->BxHeight;
	       point[3][0] = box->BxWidth;
	       point[3][1] = 0;
	       max = 3;
	       break;
	    case 'C':
	    case 'P':		/* rectangles aux bords arrondis */
	       arc = (3 * PTS_POUCE) / 25.4 + 0.5;
	       point[0][0] = 0;
	       point[0][1] = arc;
	       point[1][0] = 0;
	       point[1][1] = box->BxHeight - arc;
	       point[2][0] = arc;
	       point[2][1] = box->BxHeight;
	       point[3][0] = box->BxWidth - arc;
	       point[3][1] = box->BxHeight;
	       point[4][0] = box->BxWidth;
	       point[4][1] = box->BxHeight - arc;
	       point[5][0] = box->BxWidth;
	       point[5][1] = arc;
	       point[6][0] = box->BxWidth - arc;
	       point[6][1] = 0;
	       point[7][0] = arc;
	       point[7][1] = 0;
	       max = 7;
	       break;
	    case 'L':		/* losange */
	       point[0][0] = 0;
	       point[0][1] = box->BxHeight / 2;
	       point[1][0] = box->BxWidth / 2;
	       point[1][1] = box->BxHeight;
	       point[2][0] = box->BxWidth;
	       point[2][1] = box->BxHeight / 2;
	       point[3][0] = box->BxWidth / 2;
	       point[3][1] = 0;
	       max = 3;
	       break;
	    case 'c':
	    case 'Q':		/* ellipses */
	       val1 = x - ((float) box->BxWidth / 2);
	       val2 = (y - ((float) box->BxHeight / 2)) * ((float) box->BxWidth / (float) box->BxHeight);
	       val1 = val1 * val1 + val2 * val2;
	       val2 = (float) box->BxWidth / 2;
	       val2 = val2 * val2;
	       if (val1 <= val2)
		  return (True);	/* le point est dans le cercle */
	       else
		  return (False);	/* le point est hors du cercle */
	       break;
	    default:
	       break;
	 }
   if (max < 2)
      /* il n'y a pas de surface */
      return (False);

   /* premier et dernier point de la polyline */
   i = 0;
   croise = 0;
   nextX = point[0][0];
   nextY = point[0][1];
   prevX = point[max][0];
   prevY = point[max][1];
   if ((prevY >= y) != (nextY >= y))
     {
	/* y entre nextY et prevY */
	if ((OK = (prevX >= x)) == (nextX >= x))
	  {
	     /* x du meme cote des deux extremites */
	     if (OK)
		/* nextX et prevX >= x */
		croise++;
	  }
	else
	   /* x entre deux extremites */
	   croise += (prevX - (prevY - y) * (nextX - prevX) / (nextY - prevY)) >= x;
     }

   i++;
   while (i <= max)
     {
	prevX = nextX;
	prevY = nextY;
	nextX = point[i][0];
	nextY = point[i][1];
	if (prevY >= y)
	  {
	     while ((i <= max) && (nextY >= y))
	       {
		  i++;		/* changement de point */
		  prevY = nextY;
		  prevX = nextX;
		  nextX = point[i][0];
		  nextY = point[i][1];
	       }

	     if (i > max)
		break;

	     if ((OK = (prevX >= x)) == (nextX >= x))
	       {
		  if (OK)
		     croise++;
	       }
	     else
		croise += (prevX - (prevY - y) * (nextX - prevX) / (nextY - prevY)) >= x;
	  }
	else
	  {
	     while ((i <= max) && (nextY < y))
	       {
		  i++;		/* changement de point */
		  prevY = nextY;
		  prevX = nextX;
		  nextX = point[i][0];
		  nextY = point[i][1];
	       }

	     if (i > max)
		break;

	     if ((OK = (prevX >= x)) == (nextX >= x))
	       {
		  if (OK)
		     croise++;
	       }
	     else
		croise += (prevX - (prevY - y) * (nextX - prevX) / (nextY - prevY)) >= x;
	  }
     }
   OK = (boolean) (croise & 0x01);
   return (OK);
}


/* ---------------------------------------------------------------------- */
/* |    SurLeGraphique teste si le point x,y appartient au pave'        | */
/* |    graphique pave.                                                 | */
/* |    Si oui, retourne l'adresse de la boi^te correspondante          | */
/* |    sinon, la valeur NULL.                                          | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static PtrBox     SurLeGraphique (PtrAbstractBox pave, int x, int y)
#else  /* __STDC__ */
static PtrBox     SurLeGraphique (pave, x, y)
PtrAbstractBox             pave;
int                 x;
int                 y;

#endif /* __STDC__ */
{
   PtrBox            box;
   int                 ptc;
   int                 arc;

   /* coordonnees relatives a la boite (calculs plus simples) */
   box = pave->AbBox;
   x -= box->BxXOrg;
   y -= box->BxYOrg;

   /* On note eventuellement le point caracteristique selectionne */
   /*            1-------------2-------------3                  */
   /*            |                           |                  */
   /*            |                           |                  */
   /*            8                           4                  */
   /*            |                           |                  */
   /*            |                           |                  */
   /*            7-------------6-------------5                  */

   if (x < DELTA_SEL)
      if (y < DELTA_SEL)
	 ptc = 1;
      else if (y > box->BxHeight / 2 - DELTA_SEL &&
	       y < box->BxHeight / 2 + DELTA_SEL)
	 ptc = 8;
      else if (y > box->BxHeight - 10)
	 ptc = 7;
      else
	 ptc = 0;
   else if (x > box->BxWidth / 2 - DELTA_SEL &&
	    x < box->BxWidth / 2 + DELTA_SEL)
      if (y < DELTA_SEL)
	 ptc = 2;
      else if (y > box->BxHeight - DELTA_SEL)
	 ptc = 6;
      else
	 ptc = 0;
   else if (x > box->BxWidth - DELTA_SEL)
      if (y < DELTA_SEL)
	 ptc = 3;
      else if (y > box->BxHeight / 2 - DELTA_SEL &&
	       y < box->BxHeight / 2 + DELTA_SEL)
	 ptc = 4;
      else if (y > box->BxHeight - 10)
	 ptc = 5;
      else
	 ptc = 0;
   else
      ptc = 0;

   /* Est-ce un point caracteristique specifique du graphique ? */
   switch (pave->AbRealShape)
	 {
	    case ' ':
	    case 'R':
	    case '0':
	    case '1':
	    case '2':
	    case '3':
	    case '4':
	    case '5':
	    case '6':
	    case '7':
	    case '8':
	       if (pointOnLine (x, y, 0, 0, box->BxWidth, 0)
		   || pointOnLine (x, y, 0, box->BxHeight, box->BxWidth, box->BxHeight)
		   || pointOnLine (x, y, 0, 0, 0, box->BxHeight)
		   || pointOnLine (x, y, box->BxWidth, 0, box->BxWidth, box->BxHeight))
		  return (box);
	       break;
	    case 'L':
	       if (pointOnLine (x, y, 0, box->BxHeight / 2, box->BxWidth / 2, 0)
		   || pointOnLine (x, y, 0, box->BxHeight / 2, box->BxWidth / 2, box->BxHeight)
		   || pointOnLine (x, y, box->BxWidth, box->BxHeight / 2, box->BxWidth / 2, 0)
		   || pointOnLine (x, y, box->BxWidth, box->BxHeight / 2, box->BxWidth / 2, box->BxHeight))
		  return (box);
	       break;
	    case 'C':
	    case 'P':
	       arc = (3 * PTS_POUCE) / 25.4 + 0.5;
	       if (pointOnLine (x, y, arc, 0, box->BxWidth - arc, 0)
		   || pointOnLine (x, y, 0, arc, 0, box->BxHeight - arc)
		   || pointOnLine (x, y, arc, box->BxHeight, box->BxWidth - arc, box->BxHeight)
		   || pointOnLine (x, y, box->BxWidth, arc, box->BxWidth, box->BxHeight - arc))
		  return (box);
	       break;
	    case 'c':
	    case 'Q':
	       if (ptc == 2 || ptc == 4 || ptc == 6 || ptc == 8)
		  return (box);
	       break;
	    case 'W':
	       if (ptc == 1 || ptc == 3 || ptc == 5 ||
		   pointOnLine (x, y, 0, 0, box->BxWidth, 0) ||
	       pointOnLine (x, y, box->BxWidth, 0, box->BxWidth, box->BxHeight))
		  return (box);
	       break;
	    case 'X':
	       if (ptc == 3 || ptc == 5 || ptc == 7 ||
		   pointOnLine (x, y, box->BxWidth, 0, box->BxWidth, box->BxHeight) ||
	       pointOnLine (x, y, box->BxWidth, box->BxHeight, 0, box->BxHeight))
		  return (box);
	       break;
	    case 'Y':
	       if (ptc == 1 || ptc == 5 || ptc == 7 ||
		   pointOnLine (x, y, box->BxWidth, box->BxHeight, 0, box->BxHeight) ||
		   pointOnLine (x, y, 0, box->BxHeight, 0, 0))
		  return (box);
	       break;
	    case 'Z':
	       if (ptc == 1 || ptc == 3 || ptc == 7 ||
		   pointOnLine (x, y, 0, box->BxHeight, 0, 0) ||
		   pointOnLine (x, y, 0, 0, box->BxWidth, 0))
		  return (box);
	       break;
	    case 'h':
	    case '<':
	    case '>':
	       if (ptc == 4 || ptc == 8 ||
		   pointOnLine (x, y, 0, box->BxHeight / 2, box->BxWidth, box->BxHeight / 2))
		  return (box);
	       break;
	    case 't':
	       if (ptc == 1 || ptc == 2 || ptc == 3 ||
		   pointOnLine (x, y, 0, 0, box->BxWidth, 0))
		  return (box);
	       break;
	    case 'b':
	       if (ptc == 5 || ptc == 6 || ptc == 7 ||
	       pointOnLine (x, y, box->BxWidth, box->BxHeight, 0, box->BxHeight))
		  return (box);
	       break;
	    case 'v':
	    case '^':
	    case 'V':
	       if (ptc == 2 || ptc == 6 || pointOnLine (x, y, box->BxWidth / 2, 0,
					      box->BxWidth / 2, box->BxHeight))
		  return (box);
	       break;
	    case 'l':
	       if (ptc == 1 || ptc == 7 || ptc == 8 ||
		   pointOnLine (x, y, 0, box->BxHeight, 0, 0))
		  return (box);
	       break;
	    case 'r':
	       if (ptc == 3 || ptc == 4 || ptc == 5 ||
	       pointOnLine (x, y, box->BxWidth, 0, box->BxWidth, box->BxHeight))
		  return (box);
	       break;
	    case '\\':
	    case 'O':
	    case 'e':
	       if (ptc == 1 || ptc == 5 ||
		   pointOnLine (x, y, 0, 0, box->BxWidth, box->BxHeight))
		  return (box);
	       break;
	    case '/':
	    case 'o':
	    case 'E':
	       if (ptc == 3 || ptc == 7 ||
		   pointOnLine (x, y, 0, box->BxHeight, box->BxWidth, 0))
		  return (box);
	       break;
	    default:
	       break;
	 }
   return (NULL);
}				/*SurLeGraphique */

/* ---------------------------------------------------------------------- */
/* |    DansLaBoite teste si le point x, y appartient au pave'          | */
/* |    adpave.                                                         | */
/* |    Si oui, retourne l'adresse du pave' correspondant               | */
/* |    sinon, la valeur NULL.                                          | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
PtrBox            DansLaBoite (PtrAbstractBox adpave, int Xmin, int Xmax, int y, int *pointselect)
#else  /* __STDC__ */
PtrBox            DansLaBoite (adpave, Xmin, Xmax, y, pointselect)
PtrAbstractBox             adpave;
int                 Xmin;
int                 Xmax;
int                 y;
int                *pointselect;

#endif /* __STDC__ */
{
   PtrBox            ibox;

   *pointselect = 0;

   if (adpave->AbBox == NULL)
      return (NULL);
   else
     {
	ibox = adpave->AbBox;
	/* Est-ce une boite de coupure incluse ? */
	if (ibox->BxType == BoSplit)
	  {
	     for (ibox = ibox->BxNexChild; ibox != NULL; ibox = ibox->BxNexChild)
	       {
		  if (ibox->BxNChars > 0 &&
		      ibox->BxXOrg <= Xmax &&
		      ibox->BxXOrg + ibox->BxWidth >= Xmin &&
		      ibox->BxYOrg <= y &&
		      ibox->BxYOrg + ibox->BxHeight >= y)
		     return (ibox);
	       }
	     return (NULL);
	  }
	/* C'est une boite eclatee ? */
	else if (ibox->BxType == BoGhost)
	   return (NULL);
	/* Si le pave englobe le point designe */
	else if (ibox->BxXOrg <= Xmax
		 && ibox->BxXOrg + ibox->BxWidth >= Xmin
		 && ibox->BxYOrg <= y
		 && ibox->BxYOrg + ibox->BxHeight >= y)
	   /* Si c'est un pave graphique */
	   if (adpave->AbLeafType == LtGraphics && adpave->AbVolume != 0)
	     {
		ibox = SurLeGraphique (adpave, Xmax, y);
		if (ibox != NULL)
		   return (ibox);
		/* le point n'est pas sur un des segments */
		if (DansLeGraphique (adpave, Xmax, y))
		   return (adpave->AbBox);
		else
		   return (ibox);
	     }
	   else if (adpave->AbLeafType == LtPlyLine && adpave->AbVolume > 2)
	     {
		/* La polyline contient au moins un segment */
		ibox = SurLaPolyLine (adpave, Xmax, y, pointselect);
		if (ibox != NULL)
		   return (ibox);
		/* le point n'est pas sur un des segments */
		if ((adpave->AbPolyLineShape == 'p' || adpave->AbPolyLineShape == 's')
		    && DansLaPolyLine (adpave, Xmax, y))
		   return (adpave->AbBox);
		else
		   return (ibox);
	     }
	   else
	      return (ibox);
	else
	   return (NULL);
     }
}				/*DansLaBoite */


/* ---------------------------------------------------------------------- */
/* |    DesPave recherche le pave englobant le point designe' par x,y   | */
/* |            de la fenetre frame.                                    | */
/* |            La fonction rend le pointeur sur le plus elementaire    | */
/* |            des paves qui englobe le point designe ou NULL.         | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
PtrAbstractBox             DesPave (int frame, int x, int y)
#else  /* __STDC__ */
PtrAbstractBox             DesPave (frame, x, y)
int                 frame;
int                 x;
int                 y;

#endif /* __STDC__ */
{
   ViewFrame            *pFe1;
   PtrBox            ibox;
   int                 pointselect;

   pFe1 = &FntrTable[frame - 1];
   ibox = NULL;
   if (pFe1->FrAbstractBox != NULL)
      if (ThotLocalActions[T_desboite] != NULL)
	 (*ThotLocalActions[T_desboite]) (&ibox, pFe1->FrAbstractBox, frame, x + pFe1->FrXOrg,
				       y + pFe1->FrYOrg, &pointselect);
   if (ibox == NULL)
      return (NULL);
   else
      return (ibox->BxAbstractBox);
}


/* ---------------------------------------------------------------------- */
/* |    Pave_Suivant retourne le premier pave fils ou le suivant ou le  | */
/* |            suivant du pere.                                        | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
PtrAbstractBox             Pave_Suivant (PtrAbstractBox adpave)
#else  /* __STDC__ */
PtrAbstractBox             Pave_Suivant (adpave)
PtrAbstractBox             adpave;

#endif /* __STDC__ */
{
   if (adpave->AbFirstEnclosed != NULL)
      return (adpave->AbFirstEnclosed);	/*le premier fils */
   else if (adpave->AbNext != NULL)
      return (adpave->AbNext);	/*le suivant */
   else if (adpave->AbEnclosing != NULL)
     {
	/* Le suivant d'un pere */
	do
	   if (adpave->AbEnclosing != NULL)
	      adpave = adpave->AbEnclosing;
	   else
	      return (NULL);
	while (adpave->AbNext == NULL);
	return (adpave->AbNext);
     }
   else
      return (NULL);
}



/* ---------------------------------------------------------------------- */
/* |  DesBoiteTerm recherche la boite terminale situee au point x,y     | */
/* |    dans l'image concrete.                                          | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
PtrBox            DesBoiteTerm (int frame, int x, int y)
#else  /* __STDC__ */
PtrBox            DesBoiteTerm (frame, x, y)
int                 frame;
int                 x;
int                 y;

#endif /* __STDC__ */
{
   PtrAbstractBox             pav;
   PtrBox            sbox, ibox;
   PtrBox            testbox;
   int                 distmax;
   int                 lepoint;
   int                 d;
   ViewFrame            *pFe1;

   ibox = NULL;
   sbox = NULL;
   distmax = 2000;		/* au-dela, on n'accepte pas la selection */
   pFe1 = &FntrTable[frame - 1];

   if (pFe1->FrAbstractBox != NULL)
      ibox = pFe1->FrAbstractBox->AbBox;
   if (ibox != NULL)
     {
	ibox = ibox->BxNext;
	while (ibox != NULL)
	  {
	     pav = ibox->BxAbstractBox;
	     if (pav->AbVisibility >= pFe1->FrVisibility
		 && (!pav->AbPresentationBox || pav->AbCanBeModified))
	       {
		  if (pav->AbLeafType == LtGraphics || pav->AbLeafType == LtPlyLine)
		    {
		       testbox = DansLaBoite (pav, x, x, y, &lepoint);
		       /*d = DistGraphique(x, y, ibox, (int)pav->AbRealShape); */
		       if (testbox == NULL)
			  d = distmax + 1;
		       else
			  d = 0;
		    }
		  else if (pav->AbLeafType == LtSymbol && pav->AbShape == 'r')
		     /* glitch pour le symbole racine */
		     d = DistGraphique (x, y, ibox, 1);
		  else if (pav->AbLeafType == LtText
			   || pav->AbLeafType == LtSymbol
			   || pav->AbLeafType == LtPicture
		     /* ou une boite composee vide */
		   || (pav->AbLeafType == LtCompound && pav->AbVolume == 0))
		     d = DistBox (x, y, ibox->BxXOrg, ibox->BxYOrg, ibox->BxWidth, ibox->BxHeight);
		  else
		     d = distmax + 1;

		  /* Prend l'element le plus proche */
		  if (d < distmax)
		    {
		       distmax = d;
		       sbox = ibox;
		    }
		  else if (d == distmax)
		    {
		       /* Si c'est la premiere boite trouvee */
		       if (sbox == NULL)
			 {
			    distmax = d;
			    sbox = ibox;
			 }
		       /* Si la boite est sur un plan au dessus de la precedente */
		       else if (sbox->BxAbstractBox->AbDepth > ibox->BxAbstractBox->AbDepth)
			 {
			    distmax = d;
			    sbox = ibox;
			 }
		    }
	       }
	     ibox = ibox->BxNext;
	  }
     }
   return sbox;
}


/* ---------------------------------------------------------------------- */
/* |  DepZone de'termine les limites de de'placement de la boite en X ou| */
/* |            en Y.                                                   | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         DepZone (PtrAbstractBox pave, int frame, boolean EnX, int *min, int *max)
#else  /* __STDC__ */
static void         DepZone (pave, frame, EnX, min, max)
PtrAbstractBox             pave;
int                 frame;
boolean             EnX;
int                *min;
int                *max;

#endif /* __STDC__ */
{
   PtrAbstractBox             englobant;

   /* Valeurs par defaut */
   *min = 0;
   *max = 100000;

   /* C'est la boite racine */
   if (pave == FntrTable[frame - 1].FrAbstractBox)
     {
	if (EnX)
	  {
	     /* Est-ce que la boite depend de la fenetre */
	     if ((pave->AbWidth.DimValue == 0)
		 || (pave->AbBox->BxHorizEdge == Right))
		*max = pave->AbBox->BxWidth;
	  }
	else
	  {
	     if ((pave->AbHeight.DimValue == 0)
		 || (pave->AbBox->BxVertEdge == Bottom))
		*max = pave->AbBox->BxHeight;
	  }
     }
   /* Dans les autres cas */
   else if (EnX)
     {
	/* Si le pave est englobe depend de son englobant direct */
	if (pave->AbHorizEnclosing)
	   englobant = pave->AbEnclosing;
	else
	   englobant = FntrTable[frame - 1].FrAbstractBox;

	/* A priori limite dans l'espace de la boite englobante */
	*min = englobant->AbBox->BxXOrg;
	*max = *min + englobant->AbBox->BxWidth;

	/* Si la boite englobante prend la taille de son contenu */
	/* et que le pave n'est pas attache a l'englobante       */
	if (englobant->AbBox->BxContentWidth)
	   switch (englobant->AbBox->BxHorizEdge)
		 {
		    case Left:
		       *max = 100000;
		       break;
		    case Right:
		       *min = 0;
		       break;
		    default:
		       *min = 0;
		       *max = 100000;
		       break;
		 }
     }
   else
     {
	/* Si le pave est englobe depend de son englobant direct */
	if (pave->AbVertEnclosing)
	   englobant = pave->AbEnclosing;
	else
	   englobant = FntrTable[frame - 1].FrAbstractBox;

	/* A priori limite dans l'espace de la boite englobante */
	*min = englobant->AbBox->BxYOrg;
	*max = *min + englobant->AbBox->BxHeight;

	/* Si la boite englobante prend la taille de son contenu */
	if (englobant->AbBox->BxContentHeight)
	   switch (englobant->AbBox->BxVertEdge)
		 {
		    case Top:
		       *max = 100000;
		       break;
		    case Bottom:
		       *min = 0;
		       break;
		    default:
		       *min = 0;
		       *max = 100000;
		       break;
		 }
     }
}


/* ---------------------------------------------------------------------- */
/* |    PModifiable teste si un pave est modifiable en position         | */
/* |            (X ou Y), et si oui, rend les positions extremes        | */
/* |            de la boite.                                            | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static boolean      PModifiable (PtrAbstractBox pave, int frame, boolean EnX, int *min, int *max)
#else  /* __STDC__ */
static boolean      PModifiable (pave, frame, EnX, min, max)
PtrAbstractBox             pave;
int                 frame;
boolean             EnX;
int                *min;
int                *max;

#endif /* __STDC__ */
{
   PtrAbstractBox             pere;
   PtrBox            box;
   boolean             ok;

   box = pave->AbBox;
   pere = pave->AbEnclosing;
   /* Deplacement nul si ok est faux */
   if (EnX)
      *min = box->BxXOrg;
   else
      *min = box->BxYOrg;
   *max = *min;

#ifndef STRUCT_EDIT
   /* For Amaya only !!!!!!!!!!!! */
   if (!ExceptTypeElem (ExcMoveResize, pave->AbElement->ElTypeNumber, pave->AbElement->ElSructSchema))
      ok = False;
   else
#endif
      /* Box non deplacable */
   if (!PavMovable (pave, EnX))
      ok = False;
   /* Box de presentation */
   else if (pave->AbPresentationBox)
      ok = False;
   /* Box elastique */
   else if (EnX && pave->AbWidth.DimIsPosition)
      ok = False;
   else if (!EnX && pave->AbHeight.DimIsPosition)
      ok = False;
   /* Box flottante */
   else if (EnX && pave->AbHorizPos.PosAbRef == NULL)
      ok = False;
   else if (!EnX && pave->AbVertPos.PosAbRef == NULL)
      ok = False;
   /* Box racine */
   else if (pere == NULL)
      ok = True;
   /* Box mise en lignes */
   else if (pere->AbInLine || pere->AbBox->BxType == BoGhost)
      ok = False;
   else if (EnX
      /* et le pere ne depend pas de son contenu */
	    && (pere->AbBox->BxContentWidth
		|| (!pere->AbWidth.DimIsPosition && pere->AbWidth.DimMinimum))
	    && pave->AbHorizPos.PosAbRef == pere
	    && pave->AbHorizPos.PosRefEdge != Left)
      ok = False;
   else if (!EnX
      /* et le pere ne depend pas de son contenu */
	    && (pere->AbBox->BxContentHeight
		|| (!pere->AbHeight.DimIsPosition && pere->AbHeight.DimMinimum))
	    && pave->AbVertPos.PosAbRef == pere
	    && pave->AbVertPos.PosRefEdge != Top)
      ok = False;
   else
      ok = True;

   if (EnX)
      if (ok)
	{
	   DepZone (pave, frame, EnX, min, max);
	   /* La boite est-elle bloquee dans l'englobante ? */
	   if (*min == box->BxXOrg && *max == *min + box->BxWidth)
	      ok = False;
	}
      else
	{
	   *min = box->BxXOrg;
	   *max = *min + box->BxWidth;
	}
   else if (ok)
     {
	DepZone (pave, frame, EnX, min, max);
	/* La boite est-elle bloquee dans l'englobante ? */
	if (*min == box->BxYOrg && *max == *min + box->BxHeight)
	   ok = False;
     }
   else
     {
	*min = box->BxYOrg;
	*max = *min + box->BxHeight;
     }

   return ok;
}

/* ---------------------------------------------------------------------- */
/* |    APPgraphicModify envoie un message qui notifie qu'un trace' est | */
/* |            modifie'.                                               | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
boolean             APPgraphicModify (PtrElement pEl, int val, int frame, boolean pre)

#else  /* __STDC__ */
boolean             APPgraphicModify (pEl, val, frame, pre)
PtrElement          pEl;
int                 val;
int                 frame;
boolean             pre;

#endif /* __STDC__ */

{
   PtrElement          pAsc;
   boolean             result;
   NotifyOnValue       notifyEl;
   PtrDocument         pDoc;
   int                 vue;
   boolean             assoc;
   boolean             ok;

   DocVueFen (frame, &pDoc, &vue, &assoc);
   result = False;
   pAsc = pEl;
   while (pAsc != NULL)
     {
	notifyEl.event = TteElemGraphModify;
	notifyEl.document = (Document) IdentDocument (pDoc);
	notifyEl.element = (Element) pAsc;
	notifyEl.target = (Element) pEl;
	notifyEl.value = val;
	ok = ThotSendMessage ((NotifyEvent *) & notifyEl, pre);
	result = result || ok;
	pAsc = pAsc->ElParent;
     }
   return result;
}


/* ---------------------------------------------------------------------- */
/* |    DesBPosition recherche la boite selectionnee pour un changement | */
/* |            de position. Si la plus petite boite englobant le point | */
/* |            xm,ym de la fenetre frame ne peut pas etre deplacee, la | */
/* |            procedure prend la boite englobante et ainsi de suite.  | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                DesBPosition (int frame, int xm, int ym)
#else  /* __STDC__ */
void                DesBPosition (frame, xm, ym)
int                 frame;
int                 xm;
int                 ym;

#endif /* __STDC__ */
{
   PtrBox            ibox;
   PtrAbstractBox             adpave;
   boolean             encore, okH, okV;
   int                 x, large;
   int                 y, haut;
   int                 xr, Xmin;
   int                 yr, Xmax;
   int                 ymin;
   int                 ymax;
   ViewFrame            *pFe1;
   int                 pointselect;

   pFe1 = &FntrTable[frame - 1];
   pointselect = 0;		/* pas de point selectionne */
   if (pFe1->FrAbstractBox != NULL)
     {
	/* On note les coordonnees par rapport a l'image concrete */
	xr = xm + pFe1->FrXOrg;
	yr = ym + pFe1->FrYOrg;

	/* On recherche la boite englobant le point designe */

	if (ThotLocalActions[T_desboite] != NULL)
	   (*ThotLocalActions[T_desboite]) (&ibox, pFe1->FrAbstractBox, frame, xr, yr,
					 &pointselect);
	if (ibox == NULL)
	   adpave = NULL;
	else
	   adpave = ibox->BxAbstractBox;

	if (pointselect != 0)
	   encore = False;
	else
	   /* ctrl click */
	   encore = True;

	/* On boucle tant que l'on ne trouve pas une boite deplacable */
	while (encore)
	  {
	     if (adpave == NULL)
		ibox = NULL;
	     else
		ibox = adpave->AbBox;
	     if (ibox == NULL)
		encore = False;	/* Il n'y a pas de boite */
	     /* On regarde si le deplacement est autorise */
	     else
	       {
		  okH = PModifiable (adpave, frame, True, &Xmin, &Xmax);
		  okV = PModifiable (adpave, frame, False, &ymin, &ymax);
		  if (okH || okV)
		     encore = False;
	       }

	     /* Si on n'a pas trouve, il faut remonter */
	     if (encore)
		/* On passe a la boite englobante */
		if (adpave != NULL)
		   adpave = adpave->AbEnclosing;
		else
		  {
		     ibox = NULL;
		     encore = False;
		  }
	  }

	/* Est-ce que l'on a trouve une boite ? */
	if (ibox != NULL)
	  {
	     x = ibox->BxXOrg - pFe1->FrXOrg;
	     y = ibox->BxYOrg - pFe1->FrYOrg;
	     large = ibox->BxWidth;
	     haut = ibox->BxHeight;

	     if (pointselect != 0)
	       {
		  if (!APPgraphicModify (ibox->BxAbstractBox->AbElement, pointselect, frame, True))
		    {
		       /* Deplacement d'un point de la polyline */
		       x = ibox->BxXOrg - pFe1->FrXOrg;
		       y = ibox->BxYOrg - pFe1->FrYOrg;
		       TtaDisplaySimpleMessageString (LIB, INFO, LIB_MOVING_THE_BOX, TypePave (ibox->BxAbstractBox));
		       /* Note si le trace est ouvert ou ferme */
		       encore = (adpave->AbPolyLineShape == 'p' || adpave->AbPolyLineShape == 's');
		       PolyLineModification (frame, x, y, adpave->AbPolyLineBuffer, ibox->BxBuffer, ibox->BxNChars, pointselect, encore);
		       /* Pour les courbes il faut recalculer les points de controle */
		       if (ibox->BxImageDescriptor != NULL)
			 {
			    free ((char *) ibox->BxImageDescriptor);
			    ibox->BxImageDescriptor = (int *) PointsControle (ibox->BxBuffer, ibox->BxNChars);
			 }
		       /* on force le reaffichage de la boite */
		       DefClip (frame, ibox->BxXOrg - EXTRA_GRAPH, ibox->BxYOrg - EXTRA_GRAPH, ibox->BxXOrg + large + EXTRA_GRAPH, ibox->BxYOrg + haut + EXTRA_GRAPH);
		       AfFinFenetre (frame, 0);
		       SetSelect (frame, False);	/* Reaffiche la selection */
		       NouvContenu (adpave);
		       APPgraphicModify (ibox->BxAbstractBox->AbElement, pointselect, frame, False);
		    }
	       }
	     else
	       {

		  /* On note les coordonnees du point de reference */
		  switch (ibox->BxHorizEdge)
			{
			   case Left:
			      xr = x;
			      break;
			   case Right:
			      xr = x + large;
			      break;
			   case VertMiddle:
			      xr = x + large / 2;
			      break;
			   case VertRef:
			      xr = x + ibox->BxVertRef;
			      break;
			   default:
			      xr = x;
			      break;
			}

		  switch (ibox->BxVertEdge)
			{
			   case Top:
			      yr = y;
			      break;
			   case Bottom:
			      yr = y + haut;
			      break;
			   case HorizMiddle:
			      yr = y + haut / 2;
			      break;
			   case HorizRef:
			      yr = y + ibox->BxHorizRef;
			      break;
			   default:
			      yr = y;
			      break;
			}

		  TtaDisplaySimpleMessageString (LIB, INFO, LIB_MOVING_THE_BOX, TypePave (ibox->BxAbstractBox));
		  /* On retablit les positions par rapport a la fenetre */
		  Xmin -= pFe1->FrXOrg;
		  Xmax -= pFe1->FrXOrg;
		  ymin -= pFe1->FrYOrg;
		  ymax -= pFe1->FrYOrg;
		  /* On initialise la boite fantome */
		  ChPosition (frame, &x, &y, large, haut, xr, yr, Xmin, Xmax, ymin, ymax, xm, ym);

		  /* On transmet la modification a l'editeur */
		  x = x + pFe1->FrXOrg - ibox->BxXOrg;
		  y = y + pFe1->FrYOrg - ibox->BxYOrg;
		  NouvPosition (ibox->BxAbstractBox, x, y, frame, True);
	       }
	  }
	else
	   /* On n'a pas trouve de boite modifiable */
	   TtaDisplaySimpleMessage (LIB, INFO, LIB_YOU_CANNOT_MODIFY_A_BOX_HERE);
     }
}


/* ---------------------------------------------------------------------- */
/* |    DModifiable teste si un pave est modifiable en Dimension.       | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static boolean      DModifiable (PtrAbstractBox pave, int frame, boolean EnX, int *min, int *max)
#else  /* __STDC__ */
static boolean      DModifiable (pave, frame, EnX, min, max)
PtrAbstractBox             pave;
int                 frame;
boolean             EnX;
int                *min;
int                *max;

#endif /* __STDC__ */
{
   boolean             ok;
   PtrBox            box;
   PtrAbstractBox             pere;

   box = pave->AbBox;
   pere = pave->AbEnclosing;
   /* Modification nulle si ok est faux */
   if (EnX)
      *min = box->BxXOrg;
   else
      *min = box->BxYOrg;
   *max = *min;


#ifndef STRUCT_EDIT
   /* For Amaya only !!!!!!!!!!!! */
   if (!ExceptTypeElem (ExcMoveResize, pave->AbElement->ElTypeNumber, pave->AbElement->ElSructSchema))
      ok = False;
   else
#endif
      /* Box non retaillable */
   if (!PavResizable (pave, EnX))
      ok = False;
   /* Box de presentation */
   else if (pave->AbPresentationBox)
      ok = False;
   /* Box elastique */
   else if (EnX && pave->AbWidth.DimIsPosition)
      ok = False;
   else if (!EnX && pave->AbHeight.DimIsPosition)
      ok = False;
   /* Box racine */
   else if (pere == NULL)
     {
	if ((EnX && pave->AbWidth.DimValue == 0)
	    || (!EnX && pave->AbHeight.DimValue == 0))
	   ok = False;
	else
	   ok = True;
     }
   /* Texte mise en lignes */
   else if (pave->AbLeafType == LtText
	    && (pere->AbInLine || pere->AbBox->BxType == BoGhost))
      ok = False;
   /* Il est impossible de modifier si la dimension du contenu */
   /* d'une boite construite ou de type texte                  */
   else if (pave->AbLeafType == LtCompound || pave->AbLeafType == LtText)
      if (EnX && (box->BxContentWidth || (!pave->AbWidth.DimIsPosition && pave->AbWidth.DimMinimum)))
	 ok = False;
      else if (!EnX && (box->BxContentHeight || (!pave->AbHeight.DimIsPosition && pave->AbHeight.DimMinimum)))
	 ok = False;
      else
	 ok = True;
   else
      ok = True;

   if (EnX)
      if (ok)
	 DepZone (pave, frame, EnX, min, max);
      else
	{
	   *min = box->BxXOrg;
	   *max = *min + box->BxWidth;
	}
   else if (ok)
      DepZone (pave, frame, EnX, min, max);
   else
     {
	*min = box->BxYOrg;
	*max = *min + box->BxHeight;
     }

   return ok;
}


/* ---------------------------------------------------------------------- */
/* |    DesBDimension recherche la boite selectionnee pour un changement| */
/* |            de dimension. Si la plus petite boite englobant le point| */
/* |            x,y de la fenetre frame ne peut pas etre redimensionnee,        | */
/* |            la procedure prend la boite englobante et ainsi de      | */
/* |            suite.                                                  | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                DesBDimension (int frame, int xm, int ym)
#else  /* __STDC__ */
void                DesBDimension (frame, xm, ym)
int                 frame;
int                 xm;
int                 ym;

#endif /* __STDC__ */
{
   PtrBox            ibox;
   PtrAbstractBox             adpave;
   boolean             encore, okH, okV;
   int                 x, large;
   int                 y, haut;
   int                 xr, Xmin;
   int                 yr, Xmax;
   int                 ymin;
   int                 ymax;
   ViewFrame            *pFe1;

   int                 pointselect;

   okH = False;
   okV = False;
   pFe1 = &FntrTable[frame - 1];
   if (pFe1->FrAbstractBox != NULL)
     {
	/* On note les coordonnees par rapport a l'image concrete */
	xr = xm + pFe1->FrXOrg;
	yr = ym + pFe1->FrYOrg;

	/* On recherche la boite englobant le point designe */
	/* designation style Grenoble */
	if (ThotLocalActions[T_desboite] != NULL)
	   (*ThotLocalActions[T_desboite]) (&ibox, pFe1->FrAbstractBox, frame, xr, yr,
					 &pointselect);
	if (ibox == NULL)
	   adpave = NULL;
	else
	   adpave = ibox->BxAbstractBox;

	/* ctrlClick */
	encore = True;
	/* On boucle tant que l'on ne trouve pas une boite modifiable */
	while (encore)
	  {
	     if (adpave == NULL)
		ibox = NULL;
	     else
		ibox = adpave->AbBox;

	     if (ibox == NULL)
		encore = False;	/* Il n'y a pas de boite */
	     /* On regarde si les modifications sont autorisees */
	     else
	       {
		  okH = DModifiable (adpave, frame, True, &Xmin, &Xmax);
		  okV = DModifiable (adpave, frame, False, &ymin, &ymax);
		  if (okH || okV)
		     encore = False;
	       }

	     /* Si on n'a pas trouve, il faut remonter */
	     if (encore)
		/* On passe a la boite englobante */
		if (adpave != NULL)
		   adpave = adpave->AbEnclosing;
		else
		  {
		     ibox = NULL;
		     encore = False;
		  }
	  }

	/* Est-ce que l'on a trouve une boite ? */
	if (ibox != NULL)
	  {
	     x = ibox->BxXOrg - pFe1->FrXOrg;
	     y = ibox->BxYOrg - pFe1->FrYOrg;
	     large = ibox->BxWidth;
	     haut = ibox->BxHeight;

	     /* On note les coordonnees du point de reference */
	     switch (ibox->BxHorizEdge)
		   {
		      case Left:
			 xr = x;
			 break;
		      case Right:
			 xr = x + large;
			 break;
		      case VertMiddle:
			 xr = x + large / 2;
			 break;
		      case VertRef:
			 xr = x + ibox->BxVertRef;
			 break;
		      default:
			 xr = x + large;
			 break;
		   }

	     switch (ibox->BxVertEdge)
		   {
		      case Top:
			 yr = y;
			 break;
		      case Bottom:
			 yr = y + haut;
			 break;
		      case HorizMiddle:
			 yr = y + haut / 2;
			 break;
		      case HorizRef:
			 yr = y + ibox->BxHorizRef;
			 break;
		      default:
			 yr = y + haut;
			 break;
		   }

	     TtaDisplaySimpleMessageString (LIB, INFO, LIB_CHANGING_THE_BOX, TypePave (ibox->BxAbstractBox));
	     /* On retablit les positions par rapport a la fenetre */
	     Xmin -= pFe1->FrXOrg;
	     if (okH)
		Xmax -= pFe1->FrXOrg;
	     else
		Xmax = Xmin;
	     ymin -= pFe1->FrYOrg;
	     if (okV)
		ymax -= pFe1->FrYOrg;
	     else
		ymax = ymin;
	     /* On initialise la boite fantome */
	     ChDimension (frame, x, y, &large, &haut, xr, yr, Xmin, Xmax, ymin, ymax, xm, ym);

	     /* On transmet la modification a l'editeur */
	     large = large - ibox->BxWidth;
	     haut = haut - ibox->BxHeight;
	     NouvDimension (ibox->BxAbstractBox, large, haut, frame, True);
	  }
	else
	   /* On n'a pas trouve de boite modifiable */
	   TtaDisplaySimpleMessage (LIB, INFO, LIB_YOU_CANNOT_MODIFY_A_BOX_HERE);
     }
}


/* ---------------------------------------------------------------------- */
/* |    ModeCreation re'alise les differents modes de cre'ation         | */
/* |            interactive des boi^tes.                                | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                ModeCreation (PtrBox ibox, int frame)
#else  /* __STDC__ */
void                ModeCreation (ibox, frame)
PtrBox            ibox;
int                 frame;

#endif /* __STDC__ */
{
   int                 x, y;
   int                 large, haut;
   int                 xr, yr;
   int                 Xmin, Xmax;
   int                 Ymin, Ymax;
   ViewFrame            *pFe1;
   PtrAbstractBox             adpave;
   boolean             modPosition, modDimension;

   /* Il ne faut realiser qu'une seule creation interactive a la fois */
   if (EnCreation)
      return;
   else
      EnCreation = True;

   pFe1 = &FntrTable[frame - 1];

   /* Il faut verifier que la boite reste visible dans la fenetre */
   DimFenetre (frame, &large, &haut);
   if (ibox->BxXOrg < pFe1->FrXOrg)
      x = 0;
   else if (ibox->BxXOrg > pFe1->FrXOrg + large)
      x = large;
   else
      x = ibox->BxXOrg - pFe1->FrXOrg;

   if (ibox->BxYOrg < pFe1->FrYOrg)
      y = 0;
   else if (ibox->BxYOrg > pFe1->FrYOrg + haut)
      y = haut;
   else
      y = ibox->BxYOrg - pFe1->FrYOrg;
   large = ibox->BxWidth;
   haut = ibox->BxHeight;
   adpave = ibox->BxAbstractBox;

   /* On note les coordonnees du point de reference */
   switch (ibox->BxHorizEdge)
	 {
	    case Left:
	       xr = x;
	       break;
	    case Right:
	       xr = x + large;
	       break;
	    case VertMiddle:
	       xr = x + large / 2;
	       break;
	    case VertRef:
	       xr = x + ibox->BxVertRef;
	       break;
	    default:
	       xr = x;
	       break;
	 }

   switch (ibox->BxVertEdge)
	 {
	    case Top:
	       yr = y;
	       break;
	    case Bottom:
	       yr = y + haut;
	       break;
	    case HorizMiddle:
	       yr = y + haut / 2;
	       break;
	    case HorizRef:
	       yr = y + ibox->BxHorizRef;
	       break;
	    default:
	       yr = y;
	       break;
	 }

   modPosition = (PModifiable (adpave, frame, True, &Xmin, &Xmax)
		  || PModifiable (adpave, frame, False, &Ymin, &Ymax));
   if (!modPosition)
     {
	adpave->AbHorizPos.PosUserSpecified = False;
	adpave->AbVertPos.PosUserSpecified = False;
     }
   modDimension = (DModifiable (adpave, frame, True, &Xmin, &Xmax)
		   || DModifiable (adpave, frame, False, &Ymin, &Ymax));
   if (!modDimension)
     {
	adpave->AbWidth.DimUserSpecified = False;
	adpave->AbHeight.DimUserSpecified = False;
     }
   if (modPosition || modDimension)
     {
	/* Determine les limites de deplacement de la boite */
	DepZone (adpave, frame, True, &Xmin, &Xmax);
	DepZone (adpave, frame, False, &Ymin, &Ymax);
	TtaDisplaySimpleMessageString (LIB, INFO, LIB_CREATING_THE_BOX, TypePave (adpave));
	/* On retablit les positions par rapport a la fenetre */
	Xmin -= pFe1->FrXOrg;
	Xmax -= pFe1->FrXOrg;
	Ymin -= pFe1->FrYOrg;
	Ymax -= pFe1->FrYOrg;
	GeomCreation (frame, &x, &y, xr, yr, &large, &haut,
		      Xmin, Xmax, Ymin, Ymax,
		      adpave->AbHorizPos.PosUserSpecified,
		      adpave->AbVertPos.PosUserSpecified,
		      adpave->AbWidth.DimUserSpecified,
		      adpave->AbHeight.DimUserSpecified);

	/* Notification de la boite saisie */
	adpave->AbHorizPos.PosUserSpecified = False;
	adpave->AbVertPos.PosUserSpecified = False;
	adpave->AbWidth.DimUserSpecified = False;
	adpave->AbHeight.DimUserSpecified = False;
	x = x + pFe1->FrXOrg - ibox->BxXOrg;
	y = y + pFe1->FrYOrg - ibox->BxYOrg;
	NouvPosition (adpave, x, y, frame, True);
	large = large - ibox->BxWidth;
	haut = haut - ibox->BxHeight;
	NouvDimension (adpave, large, haut, frame, True);
     }

   /* Traitement de la creation interactive termine */
   EnCreation = False;
}


/* ---------------------------------------------------------------------- */
/* |    DesCaractere cherche le caractere affiche dans la boite ibox    | */
/* |            avec un decalage x. Rend le pointeur sur le buffer qui  | */
/* |            contient le caractere designe', l'index du caractere    | */
/* |            dans le buffer, l'index du caractere dans la boite et   | */
/* |            le nombre de blancs qui le precedent dans la boite.     | */
/* |            Met a jour la valeur x.                                 | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                DesCaractere (PtrBox ibox, PtrTextBuffer * adbuff, int *x, int *icar, int *nbcar, int *nbbl)
#else  /* __STDC__ */
void                DesCaractere (ibox, adbuff, x, icar, nbcar, nbbl)
PtrBox            ibox;
PtrTextBuffer     *adbuff;
int                *x;
int                *icar;
int                *nbcar;
int                *nbbl;

#endif /* __STDC__ */
{
   int                 dx;
   int                 reste;
   int                 restbl;
   int                 lgbl;
   int                 lgcar;
   int                 newcar;
   ptrfont             font;
   unsigned char       car;
   boolean             nontrouve;


   /* Nombre de caracteres qui precedent */
   *nbcar = 0;
   *nbbl = 0;
   car = '\0';
   lgcar = 0;
   if (ibox->BxNChars == 0 || *x <= 0)
     {
	*x = 0;
	*adbuff = ibox->BxBuffer;
	*icar = ibox->BxFirstChar;
     }
   else
     {
	font = ibox->BxFont;
	dx = 0;
	newcar = ibox->BxFirstChar;
	*icar = newcar;
	*adbuff = ibox->BxBuffer;
	reste = ibox->BxNChars;
	/* Calcule la largeur des blancs */
	if (ibox->BxSpaceWidth == 0)
	  {
	     lgbl = CarWidth (BLANC, font);
	     restbl = 0;
	  }
	else
	  {
	     lgbl = ibox->BxSpaceWidth;
	     restbl = ibox->BxNPixels;
	  }

	/* Recherche le caractere designe dans la boite */
#ifdef STRUCT_EDIT
	nontrouve = (dx < *x);
#else
	/* largeur du caractere suivant */
	car = (unsigned char) ((*adbuff)->BuContent[newcar - 1]);
	if (car == 0)
	   lgcar = 0;
	else if (car == BLANC)
	   lgcar = lgbl;
	else
	   lgcar = CarWidth (car, font);
	nontrouve = (dx + lgcar / 2 < *x);
#endif
	while (nontrouve && reste > 0)
	  {
#ifdef STRUCT_EDIT
	     /* largeur du caractere courant */
	     car = (unsigned char) ((*adbuff)->BuContent[newcar - 1]);
	     if (car == 0)
		lgcar = 0;
	     else if (car == BLANC)
		lgcar = lgbl;
	     else
		lgcar = CarWidth (car, font);
#endif

	     if (car == BLANC)
	       {
		  (*nbbl)++;
		  if (restbl > 0)
		    {
		       dx++;
		       restbl--;
		    }
	       }

	     dx += lgcar;
	     (*nbcar)++;

	     /* On passe au caractere suivant */
	     *icar = newcar;
	     if (newcar < (*adbuff)->BuLength || reste == 1)
		newcar++;
	     else if ((*adbuff)->BuNext == NULL)
	       {
		  reste = 0;
		  newcar++;
	       }
	     else
	       {
		  *adbuff = (*adbuff)->BuNext;
		  newcar = 1;
	       }
	     reste--;
#ifdef STRUCT_EDIT
	     nontrouve = (dx < *x);
#else
	     /* largeur du caractere suivant */
	     car = (unsigned char) ((*adbuff)->BuContent[newcar - 1]);
	     if (car == 0)
		lgcar = 0;
	     else if (car == BLANC)
		lgcar = lgbl;
	     else
		lgcar = CarWidth (car, font);
	     nontrouve = (dx + lgcar / 2 < *x);
#endif
	  }

	/* On a trouve le caractere : Recadre la position x */
	if (dx == *x)
	   *icar = newcar;	/* BAlignment OK */
	else if (dx > *x)
	  {
#ifdef STRUCT_EDIT
	     *x = dx - lgcar;	/* BAlignment sur le caractere */
	     if (car == BLANC)
	       {
		  if (*nbbl > 0 && ibox->BxNPixels >= *nbbl)
		     (*x)--;
		  (*nbbl)--;
	       }

	     if (newcar == 1)
		*adbuff = (*adbuff)->BuPrevious;
	     (*nbcar)--;
#else
	     *x = dx;
	     *icar = newcar;
#endif
	  }
	else
	   /* BAlignment en fin de boite */
	  {
	     *x = dx;
	     if (newcar == 1)
		*adbuff = (*adbuff)->BuPrevious;
	     (*icar)++;
	  }
     }
}
