/* 
   locate what is designated in Concret Image.
   I. Vatton
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

#include "appli_f.h"
#include "boxmoves_f.h"
#include "boxlocate_f.h"
#include "views_f.h"
#include "callback_f.h"
#include "font_f.h"
#include "geom_f.h"
#include "absboxes_f.h"
#include "buildboxes_f.h"
#include "buildlines_f.h"
#include "changepresent_f.h"
#include "boxselection_f.h"
#include "structcreation_f.h"
#include "structselect_f.h"

#ifdef WWW_MSWINDOWS		/* map to MSVC library system calls */
#include <math.h>
#endif /* WWW_MSWINDOWS */

#define Y_RATIO 200		/* penalisation en Y */
#define ANCHOR_SIZE 3		/* taille des ancres */


/* ---------------------------------------------------------------------- */
/* |  LocateSelectionInView repe`re le pave' et e'ventuellement le      | */
/* |    caracte`re se'lectionne'. La valeur de button, indique s'il     | */
/* |    s'agit d'une marque initiale ou d'une extension de se'lection : | */
/* |    - 0 s'il s'agit d'une extension de se'lection.                  | */
/* |    - 1 s'il s'agit d'un drag.                                      | */
/* |    - 2 s'il s'agit d'une marque initiale.                          | */
/* |    - 3 s'il s'agit d'un double clic.                               | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                PoseMrq (int frame, int x, int y, int button)
#else  /* __STDC__ */
void                PoseMrq (frame, x, y, button)
int                 frame;
int                 x;
int                 y;
int                 button;
#endif /* __STDC__ */
{
   int               index;
   PtrBox            pBox;
   PtrTextBuffer     pBuffer;
   PtrAbstractBox    pAb;
   int               charsNumber;
   int               spacesNumber;
   ViewFrame        *pFrame;

   if (frame >= 1)
     {
	/* recherche si une boite terminale est designee */
	pFrame = &FntrTable[frame - 1];
	x += pFrame->FrXOrg;
	y += pFrame->FrYOrg;
	pAb = pFrame->FrAbstractBox;
	charsNumber = 0;
	/* recupere la boite selectionnee */
	if (ThotLocalActions[T_selecbox] != NULL)
	   (*ThotLocalActions[T_selecbox]) (&pBox, pAb, frame, x, y, &charsNumber);
	/* S'il s'agit d'une extension de la selection */
	/* il faut eviter de selectionner la boite englobante */
	if (button == 0 || button == 1)
	  {
	     if (IsParentBox (pBox, pFrame->FrSelectionBegin.VsBox))
		pBox = DesBoiteTerm (frame, x, y);
	  }
	if (pBox != NULL)
	  {
	     pAb = pBox->BxAbstractBox;
	     if (pAb->AbLeafType == LtText &&
		 (!pAb->AbPresentationBox || pAb->AbCanBeModified))
	       {
		  x -= pBox->BxXOrg;
		  DesCaractere (pBox, &pBuffer, &x, &index, &charsNumber, &spacesNumber);
		  charsNumber = pBox->BxIndChar + charsNumber + 1;
	       }
	  }
	else
	   pAb = NULL;

	EndInsert ();
	if (pAb != NULL)
	   /* Initialisation de la selection */
	   if (button == 3)
	      SelectCour (frame, pAb, charsNumber, FALSE, TRUE, TRUE, FALSE);
	   else if (button == 2)
	      SelectCour (frame, pAb, charsNumber, FALSE, TRUE, FALSE, FALSE);
	/* Extension de la selection */
	   else if (button == 0)
	      SelectCour (frame, pAb, charsNumber, TRUE, TRUE, FALSE, FALSE);
	   else if (button == 1)
	      SelectCour (frame, pAb, charsNumber, TRUE, TRUE, FALSE, TRUE);
     }
}


/* ---------------------------------------------------------------------- */
/* |    GetDistance rend 0 si value dans l'intervalle de delta		| */
/* |         sinon la  distance absloue - delta.       	       		| */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static int          GetDistance (int value, int delta)
#else  /* __STDC__ */
static int          GetDistance (value, delta)
int                 value;
int                 delta;

#endif /* __STDC__ */
{
   if (value > delta)
      return (value - delta);
   else if (value < -delta)
      return (-value - delta);
   else
      return (0);
}


/* ---------------------------------------------------------------------- */
/* | GetBoxDistance calcule la distance d'un point xRef, yRef a` une    | */
/* |            boite. On penalise favorise la distance verticale.      | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
int                 GetBoxDistance (int xRef, int yRef, int x, int y, int width, int height)
#else  /* __STDC__ */
int                 GetBoxDistance (xRef, yRef, x, y, width, height)
int                 xRef;
int                 yRef;
int                 x;
int                 y;
int                 width;
int                 height;

#endif /* __STDC__ */
{
   int              value;

   /* prend le centre de la boite */
   width /= 2;
   x += width;
   height /= 2;
   y += height;
   value = GetDistance (xRef - x, width) + Y_RATIO * GetDistance (yRef - y, height);
   return (value);
}

/* ---------------------------------------------------------------------- */
/* |    IsOnPolyline teste qu'un point x,y est sur un segment P1(x1,y1)  | */
/* |            P2(x2,y2) avec une certaine precision DELTA_SEL.         | */
/* |            On teste l'appartenance du point a` un rectangle cree    | */
/* |            par e'largissement de DELTA_SEL autour des deux points.  | */
/* |            Le test est fait apres une rotation permettant de se     | */
/* |            ramener a` un rectangle horizontal.                      | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static boolean      IsOnPolyline (int x, int y, int x1, int y1, int x2, int y2)
#else  /* __STDC__ */
static boolean      IsOnPolyline (x, y, x1, y1, x2, y2)
int                 x;
int                 y;
int                 x1;
int                 y1;
int                 x2;
int                 y2;

#endif /* __STDC__ */
{
   int                 dX, dY, nX, nY;
   double              ra, cs, ss;

   x -= x1;
   y -= y1;
   dX = x2 - x1;
   dY = y2 - y1;
   /* ramene le 2e point sur l'horizontale */
   ra = sqrt ((double) dX * dX + dY * dY);
   if (ra == 0.0)
      return FALSE;
   cs = dX / ra;
   ss = dY / ra;
   nX = x * cs + y * ss;
   nY = y * cs - x * ss;
   /* test */
   return (nY <= DELTA_SEL
	   && nY >= -DELTA_SEL
	   && nX >= -DELTA_SEL
	   && nX <= ra + DELTA_SEL);
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
      return (FALSE);

   /* premier et dernier point de la polyline */
   maxbuff = buff = box->BxBuffer;
   i = 1;
   while (maxbuff->BuNext != NULL)
      maxbuff = maxbuff->BuNext;
   max = maxbuff->BuLength - 1;

   croise = 0;
   nextX = PointToPixel (buff->BuPoints[i].XCoord / 1000);
   nextY = PointToPixel (buff->BuPoints[i].YCoord / 1000);
   prevX = PointToPixel (maxbuff->BuPoints[max].XCoord / 1000);
   prevY = PointToPixel (maxbuff->BuPoints[max].YCoord / 1000);
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
	nextX = PointToPixel (buff->BuPoints[i].XCoord / 1000);
	nextY = PointToPixel (buff->BuPoints[i].YCoord / 1000);
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
		  nextX = PointToPixel (buff->BuPoints[i].XCoord / 1000);
		  nextY = PointToPixel (buff->BuPoints[i].YCoord / 1000);
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
		  nextX = PointToPixel (buff->BuPoints[i].XCoord / 1000);
		  nextY = PointToPixel (buff->BuPoints[i].YCoord / 1000);
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
	X2 = PointToPixel (adbuff->BuPoints[j].XCoord / 1000);
	Y2 = PointToPixel (adbuff->BuPoints[j].YCoord / 1000);
	if (x >= X2 - DELTA_SEL && x <= X2 + DELTA_SEL && y >= Y2 - DELTA_SEL && y <= Y2 + DELTA_SEL)
	  {
	     /* La selection porte sur un point de controle particulier */
	     *pointselect = i;
	     return (box);
	  }
	else if (X1 == -1)
	   OK = FALSE;
	else
	   OK = IsOnPolyline (x, y, X1, Y1, X2, Y2);

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
	X2 = PointToPixel (box->BxBuffer->BuPoints[1].XCoord / 1000);
	Y2 = PointToPixel (box->BxBuffer->BuPoints[1].YCoord / 1000);
	OK = IsOnPolyline (x, y, X1, Y1, X2, Y2);
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
   float               value1, value2;
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
	       arc = (3 * DOT_PER_INCHE) / 25.4 + 0.5;
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
	       value1 = x - ((float) box->BxWidth / 2);
	       value2 = (y - ((float) box->BxHeight / 2)) * ((float) box->BxWidth / (float) box->BxHeight);
	       value1 = value1 * value1 + value2 * value2;
	       value2 = (float) box->BxWidth / 2;
	       value2 = value2 * value2;
	       if (value1 <= value2)
		  return (TRUE);	/* le point est dans le cercle */
	       else
		  return (FALSE);	/* le point est hors du cercle */
	       break;
	    default:
	       break;
	 }
   if (max < 2)
      /* il n'y a pas de surface */
      return (FALSE);

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
	       if (IsOnPolyline (x, y, 0, 0, box->BxWidth, 0) ||
		   IsOnPolyline (x, y, 0, box->BxHeight, box->BxWidth, box->BxHeight) ||
		   IsOnPolyline (x, y, 0, 0, 0, box->BxHeight) ||
		   IsOnPolyline (x, y, box->BxWidth, 0, box->BxWidth, box->BxHeight))
		  return (box);
	       break;
	    case 'L':
	       if (IsOnPolyline (x, y, 0, box->BxHeight / 2, box->BxWidth / 2, 0) ||
		   IsOnPolyline (x, y, 0, box->BxHeight / 2, box->BxWidth / 2, box->BxHeight) ||
		   IsOnPolyline (x, y, box->BxWidth, box->BxHeight / 2, box->BxWidth / 2, 0) ||
		   IsOnPolyline (x, y, box->BxWidth, box->BxHeight / 2, box->BxWidth / 2, box->BxHeight))
		  return (box);
	       break;
	    case 'C':
	    case 'P':
	       arc = (3 * DOT_PER_INCHE) / 25.4 + 0.5;
	       if (IsOnPolyline (x, y, arc, 0, box->BxWidth - arc, 0) ||
		   IsOnPolyline (x, y, 0, arc, 0, box->BxHeight - arc) ||
		   IsOnPolyline (x, y, arc, box->BxHeight, box->BxWidth - arc, box->BxHeight) ||
		   IsOnPolyline (x, y, box->BxWidth, arc, box->BxWidth, box->BxHeight - arc))
		  return (box);
	       break;
	    case 'c':
	    case 'Q':
	       if (ptc == 2 || ptc == 4 || ptc == 6 || ptc == 8)
		  return (box);
	       break;
	    case 'W':
	       if (ptc == 1 || ptc == 3 || ptc == 5 ||
		   IsOnPolyline (x, y, 0, 0, box->BxWidth, 0) ||
	       IsOnPolyline (x, y, box->BxWidth, 0, box->BxWidth, box->BxHeight))
		  return (box);
	       break;
	    case 'X':
	       if (ptc == 3 || ptc == 5 || ptc == 7 ||
		   IsOnPolyline (x, y, box->BxWidth, 0, box->BxWidth, box->BxHeight) ||
	       IsOnPolyline (x, y, box->BxWidth, box->BxHeight, 0, box->BxHeight))
		  return (box);
	       break;
	    case 'Y':
	       if (ptc == 1 || ptc == 5 || ptc == 7 ||
		   IsOnPolyline (x, y, box->BxWidth, box->BxHeight, 0, box->BxHeight) ||
		   IsOnPolyline (x, y, 0, box->BxHeight, 0, 0))
		  return (box);
	       break;
	    case 'Z':
	       if (ptc == 1 || ptc == 3 || ptc == 7 ||
		   IsOnPolyline (x, y, 0, box->BxHeight, 0, 0) ||
		   IsOnPolyline (x, y, 0, 0, box->BxWidth, 0))
		  return (box);
	       break;
	    case 'h':
	    case '<':
	    case '>':
	       if (ptc == 4 || ptc == 8 ||
		   IsOnPolyline (x, y, 0, box->BxHeight / 2, box->BxWidth, box->BxHeight / 2))
		  return (box);
	       break;
	    case 't':
	       if (ptc == 1 || ptc == 2 || ptc == 3 ||
		   IsOnPolyline (x, y, 0, 0, box->BxWidth, 0))
		  return (box);
	       break;
	    case 'b':
	       if (ptc == 5 || ptc == 6 || ptc == 7 ||
	       IsOnPolyline (x, y, box->BxWidth, box->BxHeight, 0, box->BxHeight))
		  return (box);
	       break;
	    case 'v':
	    case '^':
	    case 'V':
	       if (ptc == 2 || ptc == 6 ||
		   IsOnPolyline (x, y, box->BxWidth / 2, 0, box->BxWidth / 2, box->BxHeight))
		  return (box);
	       break;
	    case 'l':
	       if (ptc == 1 || ptc == 7 || ptc == 8 ||
		   IsOnPolyline (x, y, 0, box->BxHeight, 0, 0))
		  return (box);
	       break;
	    case 'r':
	       if (ptc == 3 || ptc == 4 || ptc == 5 ||
	       IsOnPolyline (x, y, box->BxWidth, 0, box->BxWidth, box->BxHeight))
		  return (box);
	       break;
	    case '\\':
	    case 'O':
	    case 'e':
	       if (ptc == 1 || ptc == 5 ||
		   IsOnPolyline (x, y, 0, 0, box->BxWidth, box->BxHeight))
		  return (box);
	       break;
	    case '/':
	    case 'o':
	    case 'E':
	       if (ptc == 3 || ptc == 7 ||
		   IsOnPolyline (x, y, 0, box->BxHeight, box->BxWidth, 0))
		  return (box);
	       break;
	    default:
	       break;
	 }
   return (NULL);
}

/* ---------------------------------------------------------------------- */
/* |    DansLaBoite teste si le point x, y appartient au pave'          | */
/* |    pAb.                                                         | */
/* |    Si oui, retourne l'adresse du pave' correspondant               | */
/* |    sinon, la valeur NULL.                                          | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
PtrBox            DansLaBoite (PtrAbstractBox pAb, int Xmin, int Xmax, int y, int *pointselect)
#else  /* __STDC__ */
PtrBox            DansLaBoite (pAb, Xmin, Xmax, y, pointselect)
PtrAbstractBox             pAb;
int                 Xmin;
int                 Xmax;
int                 y;
int                *pointselect;

#endif /* __STDC__ */
{
   PtrBox            pBox;

   *pointselect = 0;

   if (pAb->AbBox == NULL)
      return (NULL);
   else
     {
	pBox = pAb->AbBox;
	/* Est-ce une boite de coupure incluse ? */
	if (pBox->BxType == BoSplit)
	  {
	     for (pBox = pBox->BxNexChild; pBox != NULL; pBox = pBox->BxNexChild)
	       {
		  if (pBox->BxNChars > 0 &&
		      pBox->BxXOrg <= Xmax &&
		      pBox->BxXOrg + pBox->BxWidth >= Xmin &&
		      pBox->BxYOrg <= y &&
		      pBox->BxYOrg + pBox->BxHeight >= y)
		     return (pBox);
	       }
	     return (NULL);
	  }
	/* C'est une boite eclatee ? */
	else if (pBox->BxType == BoGhost)
	   return (NULL);
	/* Si le pave englobe le point designe */
	else if (pBox->BxXOrg <= Xmax
		 && pBox->BxXOrg + pBox->BxWidth >= Xmin
		 && pBox->BxYOrg <= y
		 && pBox->BxYOrg + pBox->BxHeight >= y)
	   /* Si c'est un pave graphique */
	   if (pAb->AbLeafType == LtGraphics && pAb->AbVolume != 0)
	     {
		pBox = SurLeGraphique (pAb, Xmax, y);
		if (pBox != NULL)
		   return (pBox);
		/* le point n'est pas sur un des segments */
		if (DansLeGraphique (pAb, Xmax, y))
		   return (pAb->AbBox);
		else
		   return (pBox);
	     }
	   else if (pAb->AbLeafType == LtPlyLine && pAb->AbVolume > 2)
	     {
		/* La polyline contient au moins un segment */
		pBox = SurLaPolyLine (pAb, Xmax, y, pointselect);
		if (pBox != NULL)
		   return (pBox);
		/* le point n'est pas sur un des segments */
		if ((pAb->AbPolyLineShape == 'p' || pAb->AbPolyLineShape == 's')
		    && DansLaPolyLine (pAb, Xmax, y))
		   return (pAb->AbBox);
		else
		   return (pBox);
	     }
	   else
	      return (pBox);
	else
	   return (NULL);
     }
}


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
   ViewFrame            *pFrame;
   PtrBox            pBox;
   int                 pointselect;

   pFrame = &FntrTable[frame - 1];
   pBox = NULL;
   if (pFrame->FrAbstractBox != NULL)
      if (ThotLocalActions[T_selecbox] != NULL)
	 (*ThotLocalActions[T_selecbox]) (&pBox, pFrame->FrAbstractBox, frame, x + pFrame->FrXOrg,
				       y + pFrame->FrYOrg, &pointselect);
   if (pBox == NULL)
      return (NULL);
   else
      return (pBox->BxAbstractBox);
}


/* ---------------------------------------------------------------------- */
/* |    Pave_Suivant retourne le premier pave fils ou le suivant ou le  | */
/* |            suivant du pere.                                        | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
PtrAbstractBox             Pave_Suivant (PtrAbstractBox pAb)
#else  /* __STDC__ */
PtrAbstractBox             Pave_Suivant (pAb)
PtrAbstractBox             pAb;

#endif /* __STDC__ */
{
   if (pAb->AbFirstEnclosed != NULL)
      return (pAb->AbFirstEnclosed);	/*le premier fils */
   else if (pAb->AbNext != NULL)
      return (pAb->AbNext);	/*le suivant */
   else if (pAb->AbEnclosing != NULL)
     {
	/* Le suivant d'un pere */
	do
	   if (pAb->AbEnclosing != NULL)
	      pAb = pAb->AbEnclosing;
	   else
	      return (NULL);
	while (pAb->AbNext == NULL);
	return (pAb->AbNext);
     }
   else
      return (NULL);
}

/* ---------------------------------------------------------------------- */
/* |  GetGetMinimumDistance calcule la distance d'une ancre x,y au point| */
/* |            xRef,yRef. La fonction rend la distance minimale        | */
/* |            entre la distance calcule'e et dist.                    | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static int          GetGetMinimumDistance (int xRef, int yRef, int x, int y, int dist)
#else  /* __STDC__ */
static int          GetGetMinimumDistance (xRef, yRef, x, y, dist)
int                 xRef;
int                 yRef;
int                 x;
int                 y;
int                 dist;

#endif /* __STDC__ */
{
   int              value;

   value = GetDistance (x - xRef, ANCHOR_SIZE) + GetDistance (y - yRef, ANCHOR_SIZE) * Y_RATIO;
   if (value < dist)
      return (value);
   else
      return (dist);
}

/* ---------------------------------------------------------------------- */
/* |    DistGraphique calcule la distance d'un point xRef,yRef a`       | */
/* |            un point de la boite graphique (min des distances aux   | */
/* |            ancres).                                                | */
/* |            Cette selection est limitee aux boites terminale.       | */
/* |            Rend la distance de la boite au point.                  | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
int                 DistGraphique (int xRef, int yRef, PtrBox pBox, int value)
#else  /* __STDC__ */
int                 DistGraphique (xRef, yRef, pBox, value)
int                 xRef;
int                 yRef;
PtrBox            pBox;
int                 value;

#endif /* __STDC__ */
{
   int                 distance;
   int                 x, y, width, height;

   /* centrer la boite */
   width = pBox->BxWidth / 2;
   x = pBox->BxXOrg + width;
   height = pBox->BxHeight / 2;
   y = pBox->BxYOrg + height;
   distance = 1000;

   switch (value)
	 {
	    case 1:		/* racine .. */
	       distance = GetGetMinimumDistance (xRef, yRef, x - width + height / 3, y + height, distance);
	       distance = GetGetMinimumDistance (xRef, yRef, x - width + (2 * height) / 3, y - height, distance);
	       distance = GetGetMinimumDistance (xRef, yRef, x + width, y - height, distance);
	       break;

	    case 'c':		/* cercle */
	       if (width < height)
		  height = width;
	       else
		  width = height;
	       /* ATTENTION: on continue en sequence, */
	       /* vu qu'un cercle est aussi une ellipse */
	    case 'C':		/* ellipse */
	       distance = GetGetMinimumDistance (xRef, yRef, x - width, y, distance);
	       distance = GetGetMinimumDistance (xRef, yRef, x + width, y, distance);
	       distance = GetGetMinimumDistance (xRef, yRef, x, y - height, distance);
	       distance = GetGetMinimumDistance (xRef, yRef, x, y + height, distance);
	       height = (71 * height) / 100;
	       width = (71 * width) / 100;
	       distance = GetGetMinimumDistance (xRef, yRef, x - width, y + height, distance);
	       distance = GetGetMinimumDistance (xRef, yRef, x - width, y - height, distance);
	       distance = GetGetMinimumDistance (xRef, yRef, x + width, y + height, distance);
	       distance = GetGetMinimumDistance (xRef, yRef, x + width, y - height, distance);
	       break;

	    case 'L':		/* losange */
	       distance = GetGetMinimumDistance (xRef, yRef, x - width, y, distance);
	       distance = GetGetMinimumDistance (xRef, yRef, x + width, y, distance);
	       distance = GetGetMinimumDistance (xRef, yRef, x, y - height, distance);
	       distance = GetGetMinimumDistance (xRef, yRef, x, y + height, distance);
	       break;

	    case 't':		/* trait horiz en haut */
	       distance = GetGetMinimumDistance (xRef, yRef, x - width, y - height, distance);
	       distance = GetGetMinimumDistance (xRef, yRef, x + width, y - height, distance);
	       distance = GetGetMinimumDistance (xRef, yRef, x, y - height, distance);
	       break;

	    case 'b':		/* trait horiz en bas */
	       distance = GetGetMinimumDistance (xRef, yRef, x - width, y + height, distance);
	       distance = GetGetMinimumDistance (xRef, yRef, x + width, y + height, distance);
	       distance = GetGetMinimumDistance (xRef, yRef, x, y + height, distance);
	       break;

	    case 'h':		/* trait horizontal centre ou fleche */
	    case '<':
	    case '>':
	       distance = GetGetMinimumDistance (xRef, yRef, x - width, y, distance);
	       distance = GetGetMinimumDistance (xRef, yRef, x + width, y, distance);
	       distance = GetGetMinimumDistance (xRef, yRef, x, y, distance);
	       break;

	    case 'v':
	    case 'V':		/* trait vertical centre ou fleche ^ */
	    case '^':
	       distance = GetGetMinimumDistance (xRef, yRef, x, y - height, distance);
	       distance = GetGetMinimumDistance (xRef, yRef, x, y + height, distance);
	       distance = GetGetMinimumDistance (xRef, yRef, x, y, distance);
	       break;

	    case 'l':		/* trait vertical gauche */
	       distance = GetGetMinimumDistance (xRef, yRef, x - width, y - height, distance);
	       distance = GetGetMinimumDistance (xRef, yRef, x - width, y + height, distance);
	       distance = GetGetMinimumDistance (xRef, yRef, x - width, y, distance);
	       break;
	    case 'r':		/* trait vertical droit */
	       distance = GetGetMinimumDistance (xRef, yRef, x + width, y - height, distance);
	       distance = GetGetMinimumDistance (xRef, yRef, x + width, y + height, distance);
	       distance = GetGetMinimumDistance (xRef, yRef, x + width, y, distance);
	       break;

	    case '/':		/* diagonale ou fleche vers le haut */
	    case 'E':
	    case 'o':
	       distance = GetGetMinimumDistance (xRef, yRef, x - width, y + height, distance);
	       distance = GetGetMinimumDistance (xRef, yRef, x + width, y - height, distance);
	       distance = GetGetMinimumDistance (xRef, yRef, x, y, distance);
	       break;

	    case '\\':		/* diagonale ou fleche vers le bas */
	    case 'e':
	    case 'O':
	       distance = GetGetMinimumDistance (xRef, yRef, x - width, y - height, distance);
	       distance = GetGetMinimumDistance (xRef, yRef, x + width, y + height, distance);
	       distance = GetGetMinimumDistance (xRef, yRef, x, y, distance);
	       break;
	    default:		/* rectangle ou autre */
	       distance = GetGetMinimumDistance (xRef, yRef, x - width, y + height, distance);
	       distance = GetGetMinimumDistance (xRef, yRef, x - width, y - height, distance);
	       distance = GetGetMinimumDistance (xRef, yRef, x + width, y + height, distance);
	       distance = GetGetMinimumDistance (xRef, yRef, x + width, y - height, distance);
	       break;
	 }
   return (distance);
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
   PtrBox            sbox, pBox;
   PtrBox            testbox;
   int                 distmax;
   int                 pointIndex;
   int                 d;
   ViewFrame            *pFrame;

   pBox = NULL;
   sbox = NULL;
   /* au-dela de distmax, on n'accepte pas la selection */
   distmax = 2000;
   pFrame = &FntrTable[frame - 1];

   if (pFrame->FrAbstractBox != NULL)
      pBox = pFrame->FrAbstractBox->AbBox;
   if (pBox != NULL)
     {
	pBox = pBox->BxNext;
	while (pBox != NULL)
	  {
	     pav = pBox->BxAbstractBox;
	     if (pav->AbVisibility >= pFrame->FrVisibility
		 && (!pav->AbPresentationBox || pav->AbCanBeModified))
	       {
		  if (pav->AbLeafType == LtGraphics || pav->AbLeafType == LtPlyLine)
		    {
		       testbox = DansLaBoite (pav, x, x, y, &pointIndex);
		       if (testbox == NULL)
			  d = distmax + 1;
		       else
			  d = 0;
		    }
		  else if (pav->AbLeafType == LtSymbol && pav->AbShape == 'r')
		     /* glitch pour le symbole racine */
		     d = DistGraphique (x, y, pBox, 1);
		  else if (pav->AbLeafType == LtText
			   || pav->AbLeafType == LtSymbol
			   || pav->AbLeafType == LtPicture
		     /* ou une boite composee vide */
		   || (pav->AbLeafType == LtCompound && pav->AbVolume == 0))
		     d = GetBoxDistance (x, y, pBox->BxXOrg, pBox->BxYOrg, pBox->BxWidth, pBox->BxHeight);
		  else
		     d = distmax + 1;

		  /* Prend l'element le plus proche */
		  if (d < distmax)
		    {
		       distmax = d;
		       sbox = pBox;
		    }
		  else if (d == distmax)
		    {
		       /* Si c'est la premiere boite trouvee */
		       if (sbox == NULL)
			 {
			    distmax = d;
			    sbox = pBox;
			 }
		       /* Si la boite est sur un plan au dessus de la precedente */
		       else if (sbox->BxAbstractBox->AbDepth > pBox->BxAbstractBox->AbDepth)
			 {
			    distmax = d;
			    sbox = pBox;
			 }
		    }
	       }
	     pBox = pBox->BxNext;
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
   if (!TypeHasException (ExcMoveResize, pave->AbElement->ElTypeNumber, pave->AbElement->ElSructSchema))
      ok = FALSE;
   else
#endif
      /* Box non deplacable */
   if (!PavMovable (pave, EnX))
      ok = FALSE;
   /* Box de presentation */
   else if (pave->AbPresentationBox)
      ok = FALSE;
   /* Box elastique */
   else if (EnX && pave->AbWidth.DimIsPosition)
      ok = FALSE;
   else if (!EnX && pave->AbHeight.DimIsPosition)
      ok = FALSE;
   /* Box flottante */
   else if (EnX && pave->AbHorizPos.PosAbRef == NULL)
      ok = FALSE;
   else if (!EnX && pave->AbVertPos.PosAbRef == NULL)
      ok = FALSE;
   /* Box racine */
   else if (pere == NULL)
      ok = TRUE;
   /* Box mise en lignes */
   else if (pere->AbInLine || pere->AbBox->BxType == BoGhost)
      ok = FALSE;
   else if (EnX
      /* et le pere ne depend pas de son contenu */
	    && (pere->AbBox->BxContentWidth
		|| (!pere->AbWidth.DimIsPosition && pere->AbWidth.DimMinimum))
	    && pave->AbHorizPos.PosAbRef == pere
	    && pave->AbHorizPos.PosRefEdge != Left)
      ok = FALSE;
   else if (!EnX
      /* et le pere ne depend pas de son contenu */
	    && (pere->AbBox->BxContentHeight
		|| (!pere->AbHeight.DimIsPosition && pere->AbHeight.DimMinimum))
	    && pave->AbVertPos.PosAbRef == pere
	    && pave->AbVertPos.PosRefEdge != Top)
      ok = FALSE;
   else
      ok = TRUE;

   if (EnX)
      if (ok)
	{
	   DepZone (pave, frame, EnX, min, max);
	   /* La boite est-elle bloquee dans l'englobante ? */
	   if (*min == box->BxXOrg && *max == *min + box->BxWidth)
	      ok = FALSE;
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
	   ok = FALSE;
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
boolean             APPgraphicModify (PtrElement pEl, int value, int frame, boolean pre)

#else  /* __STDC__ */
boolean             APPgraphicModify (pEl, value, frame, pre)
PtrElement          pEl;
int                 value;
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
   result = FALSE;
   pAsc = pEl;
   while (pAsc != NULL)
     {
	notifyEl.event = TteElemGraphModify;
	notifyEl.document = (Document) IdentDocument (pDoc);
	notifyEl.element = (Element) pAsc;
	notifyEl.target = (Element) pEl;
	notifyEl.value = value;
	ok = CallEventType ((NotifyEvent *) & notifyEl, pre);
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
   PtrBox            pBox;
   PtrAbstractBox             pAb;
   boolean             encore, okH, okV;
   int                 x, large;
   int                 y, haut;
   int                 xr, Xmin;
   int                 yr, Xmax;
   int                 ymin;
   int                 ymax;
   ViewFrame            *pFrame;
   int                 pointselect;

   pFrame = &FntrTable[frame - 1];
   pointselect = 0;		/* pas de point selectionne */
   if (pFrame->FrAbstractBox != NULL)
     {
	/* On note les coordonnees par rapport a l'image concrete */
	xr = xm + pFrame->FrXOrg;
	yr = ym + pFrame->FrYOrg;

	/* On recherche la boite englobant le point designe */

	if (ThotLocalActions[T_selecbox] != NULL)
	   (*ThotLocalActions[T_selecbox]) (&pBox, pFrame->FrAbstractBox, frame, xr, yr,
					 &pointselect);
	if (pBox == NULL)
	   pAb = NULL;
	else
	   pAb = pBox->BxAbstractBox;

	if (pointselect != 0)
	   encore = FALSE;
	else
	   /* ctrl click */
	   encore = TRUE;

	/* On boucle tant que l'on ne trouve pas une boite deplacable */
	while (encore)
	  {
	     if (pAb == NULL)
		pBox = NULL;
	     else
		pBox = pAb->AbBox;
	     if (pBox == NULL)
		encore = FALSE;	/* Il n'y a pas de boite */
	     /* On regarde si le deplacement est autorise */
	     else
	       {
		  okH = PModifiable (pAb, frame, TRUE, &Xmin, &Xmax);
		  okV = PModifiable (pAb, frame, FALSE, &ymin, &ymax);
		  if (okH || okV)
		     encore = FALSE;
	       }

	     /* Si on n'a pas trouve, il faut remonter */
	     if (encore)
		/* On passe a la boite englobante */
		if (pAb != NULL)
		   pAb = pAb->AbEnclosing;
		else
		  {
		     pBox = NULL;
		     encore = FALSE;
		  }
	  }

	/* Est-ce que l'on a trouve une boite ? */
	if (pBox != NULL)
	  {
	     x = pBox->BxXOrg - pFrame->FrXOrg;
	     y = pBox->BxYOrg - pFrame->FrYOrg;
	     large = pBox->BxWidth;
	     haut = pBox->BxHeight;

	     if (pointselect != 0)
	       {
		  if (!APPgraphicModify (pBox->BxAbstractBox->AbElement, pointselect, frame, TRUE))
		    {
		       /* Deplacement d'un point de la polyline */
		       x = pBox->BxXOrg - pFrame->FrXOrg;
		       y = pBox->BxYOrg - pFrame->FrYOrg;
		       TtaDisplayMessage (INFO, TtaGetMessage(LIB, MOVING_BOX), AbsBoxType (pBox->BxAbstractBox));
		       /* Note si le trace est ouvert ou ferme */
		       encore = (pAb->AbPolyLineShape == 'p' || pAb->AbPolyLineShape == 's');
		       PolyLineModification (frame, x, y, pAb->AbPolyLineBuffer, pBox->BxBuffer, pBox->BxNChars, pointselect, encore);
		       /* Pour les courbes il faut recalculer les points de controle */
		       if (pBox->BxPictInfo != NULL)
			 {
			    free ((char *) pBox->BxPictInfo);
			    pBox->BxPictInfo = (int *) ComputeControlPoints (pBox->BxBuffer, pBox->BxNChars);
			 }
		       /* on force le reaffichage de la boite */
		       DefClip (frame, pBox->BxXOrg - EXTRA_GRAPH, pBox->BxYOrg - EXTRA_GRAPH, pBox->BxXOrg + large + EXTRA_GRAPH, pBox->BxYOrg + haut + EXTRA_GRAPH);
		       AfFinFenetre (frame, 0);
		       SetSelect (frame, FALSE);	/* Reaffiche la selection */
		       NewContent (pAb);
		       APPgraphicModify (pBox->BxAbstractBox->AbElement, pointselect, frame, FALSE);
		    }
	       }
	     else
	       {

		  /* On note les coordonnees du point de reference */
		  switch (pBox->BxHorizEdge)
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
			      xr = x + pBox->BxVertRef;
			      break;
			   default:
			      xr = x;
			      break;
			}

		  switch (pBox->BxVertEdge)
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
			      yr = y + pBox->BxHorizRef;
			      break;
			   default:
			      yr = y;
			      break;
			}

		  TtaDisplayMessage (INFO, TtaGetMessage(LIB, MOVING_BOX), AbsBoxType (pBox->BxAbstractBox));
		  /* On retablit les positions par rapport a la fenetre */
		  Xmin -= pFrame->FrXOrg;
		  Xmax -= pFrame->FrXOrg;
		  ymin -= pFrame->FrYOrg;
		  ymax -= pFrame->FrYOrg;
		  /* On initialise la boite fantome */
		  UserGeometryMove (frame, &x, &y, large, haut, xr, yr, Xmin, Xmax, ymin, ymax, xm, ym);

		  /* On transmet la modification a l'editeur */
		  x = x + pFrame->FrXOrg - pBox->BxXOrg;
		  y = y + pFrame->FrYOrg - pBox->BxYOrg;
		  NouvPosition (pBox->BxAbstractBox, x, y, frame, TRUE);
	       }
	  }
	else
	   /* On n'a pas trouve de boite modifiable */
	   TtaDisplaySimpleMessage (INFO, LIB, MODIFYING_BOX_IMP);
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
   if (!TypeHasException (ExcMoveResize, pave->AbElement->ElTypeNumber, pave->AbElement->ElSructSchema))
      ok = FALSE;
   else
#endif
      /* Box non retaillable */
   if (!PavResizable (pave, EnX))
      ok = FALSE;
   /* Box de presentation */
   else if (pave->AbPresentationBox)
      ok = FALSE;
   /* Box elastique */
   else if (EnX && pave->AbWidth.DimIsPosition)
      ok = FALSE;
   else if (!EnX && pave->AbHeight.DimIsPosition)
      ok = FALSE;
   /* Box racine */
   else if (pere == NULL)
     {
	if ((EnX && pave->AbWidth.DimValue == 0)
	    || (!EnX && pave->AbHeight.DimValue == 0))
	   ok = FALSE;
	else
	   ok = TRUE;
     }
   /* Texte mise en lignes */
   else if (pave->AbLeafType == LtText
	    && (pere->AbInLine || pere->AbBox->BxType == BoGhost))
      ok = FALSE;
   /* Il est impossible de modifier si la dimension du contenu */
   /* d'une boite construite ou de type texte                  */
   else if (pave->AbLeafType == LtCompound || pave->AbLeafType == LtText)
      if (EnX && (box->BxContentWidth || (!pave->AbWidth.DimIsPosition && pave->AbWidth.DimMinimum)))
	 ok = FALSE;
      else if (!EnX && (box->BxContentHeight || (!pave->AbHeight.DimIsPosition && pave->AbHeight.DimMinimum)))
	 ok = FALSE;
      else
	 ok = TRUE;
   else
      ok = TRUE;

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
   PtrBox            pBox;
   PtrAbstractBox             pAb;
   boolean             encore, okH, okV;
   int                 x, large;
   int                 y, haut;
   int                 xr, Xmin;
   int                 yr, Xmax;
   int                 ymin;
   int                 ymax;
   ViewFrame            *pFrame;

   int                 pointselect;

   okH = FALSE;
   okV = FALSE;
   pFrame = &FntrTable[frame - 1];
   if (pFrame->FrAbstractBox != NULL)
     {
	/* On note les coordonnees par rapport a l'image concrete */
	xr = xm + pFrame->FrXOrg;
	yr = ym + pFrame->FrYOrg;

	/* On recherche la boite englobant le point designe */
	/* designation style Grenoble */
	if (ThotLocalActions[T_selecbox] != NULL)
	   (*ThotLocalActions[T_selecbox]) (&pBox, pFrame->FrAbstractBox, frame, xr, yr,
					 &pointselect);
	if (pBox == NULL)
	   pAb = NULL;
	else
	   pAb = pBox->BxAbstractBox;

	/* ctrlClick */
	encore = TRUE;
	/* On boucle tant que l'on ne trouve pas une boite modifiable */
	while (encore)
	  {
	     if (pAb == NULL)
		pBox = NULL;
	     else
		pBox = pAb->AbBox;

	     if (pBox == NULL)
		encore = FALSE;	/* Il n'y a pas de boite */
	     /* On regarde si les modifications sont autorisees */
	     else
	       {
		  okH = DModifiable (pAb, frame, TRUE, &Xmin, &Xmax);
		  okV = DModifiable (pAb, frame, FALSE, &ymin, &ymax);
		  if (okH || okV)
		     encore = FALSE;
	       }

	     /* Si on n'a pas trouve, il faut remonter */
	     if (encore)
		/* On passe a la boite englobante */
		if (pAb != NULL)
		   pAb = pAb->AbEnclosing;
		else
		  {
		     pBox = NULL;
		     encore = FALSE;
		  }
	  }

	/* Est-ce que l'on a trouve une boite ? */
	if (pBox != NULL)
	  {
	     x = pBox->BxXOrg - pFrame->FrXOrg;
	     y = pBox->BxYOrg - pFrame->FrYOrg;
	     large = pBox->BxWidth;
	     haut = pBox->BxHeight;

	     /* On note les coordonnees du point de reference */
	     switch (pBox->BxHorizEdge)
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
			 xr = x + pBox->BxVertRef;
			 break;
		      default:
			 xr = x + large;
			 break;
		   }

	     switch (pBox->BxVertEdge)
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
			 yr = y + pBox->BxHorizRef;
			 break;
		      default:
			 yr = y + haut;
			 break;
		   }

	     TtaDisplayMessage (INFO, TtaGetMessage(LIB, MODIFYING_BOX), AbsBoxType (pBox->BxAbstractBox));
	     /* On retablit les positions par rapport a la fenetre */
	     Xmin -= pFrame->FrXOrg;
	     if (okH)
		Xmax -= pFrame->FrXOrg;
	     else
		Xmax = Xmin;
	     ymin -= pFrame->FrYOrg;
	     if (okV)
		ymax -= pFrame->FrYOrg;
	     else
		ymax = ymin;
	     /* On initialise la boite fantome */
	     UserGeometryResize (frame, x, y, &large, &haut, xr, yr, Xmin, Xmax, ymin, ymax, xm, ym);

	     /* On transmet la modification a l'editeur */
	     large = large - pBox->BxWidth;
	     haut = haut - pBox->BxHeight;
	     NouvDimension (pBox->BxAbstractBox, large, haut, frame, TRUE);
	  }
	else
	   /* On n'a pas trouve de boite modifiable */
	   TtaDisplaySimpleMessage (INFO, LIB, MODIFYING_BOX_IMP);
     }
}


/* ---------------------------------------------------------------------- */
/* |    ModeCreation re'alise les differents modes de cre'ation         | */
/* |            interactive des boi^tes.                                | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                ModeCreation (PtrBox pBox, int frame)
#else  /* __STDC__ */
void                ModeCreation (pBox, frame)
PtrBox            pBox;
int                 frame;

#endif /* __STDC__ */
{
   int                 x, y;
   int                 large, haut;
   int                 xr, yr;
   int                 Xmin, Xmax;
   int                 Ymin, Ymax;
   ViewFrame            *pFrame;
   PtrAbstractBox             pAb;
   boolean             modPosition, modDimension;

   /* Il ne faut realiser qu'une seule creation interactive a la fois */
   if (EnCreation)
      return;
   else
      EnCreation = TRUE;

   pFrame = &FntrTable[frame - 1];

   /* Il faut verifier que la boite reste visible dans la fenetre */
   DimFenetre (frame, &large, &haut);
   if (pBox->BxXOrg < pFrame->FrXOrg)
      x = 0;
   else if (pBox->BxXOrg > pFrame->FrXOrg + large)
      x = large;
   else
      x = pBox->BxXOrg - pFrame->FrXOrg;

   if (pBox->BxYOrg < pFrame->FrYOrg)
      y = 0;
   else if (pBox->BxYOrg > pFrame->FrYOrg + haut)
      y = haut;
   else
      y = pBox->BxYOrg - pFrame->FrYOrg;
   large = pBox->BxWidth;
   haut = pBox->BxHeight;
   pAb = pBox->BxAbstractBox;

   /* On note les coordonnees du point de reference */
   switch (pBox->BxHorizEdge)
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
	       xr = x + pBox->BxVertRef;
	       break;
	    default:
	       xr = x;
	       break;
	 }

   switch (pBox->BxVertEdge)
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
	       yr = y + pBox->BxHorizRef;
	       break;
	    default:
	       yr = y;
	       break;
	 }

   modPosition = (PModifiable (pAb, frame, TRUE, &Xmin, &Xmax)
		  || PModifiable (pAb, frame, FALSE, &Ymin, &Ymax));
   if (!modPosition)
     {
	pAb->AbHorizPos.PosUserSpecified = FALSE;
	pAb->AbVertPos.PosUserSpecified = FALSE;
     }
   modDimension = (DModifiable (pAb, frame, TRUE, &Xmin, &Xmax)
		   || DModifiable (pAb, frame, FALSE, &Ymin, &Ymax));
   if (!modDimension)
     {
	pAb->AbWidth.DimUserSpecified = FALSE;
	pAb->AbHeight.DimUserSpecified = FALSE;
     }
   if (modPosition || modDimension)
     {
	/* Determine les limites de deplacement de la boite */
	DepZone (pAb, frame, TRUE, &Xmin, &Xmax);
	DepZone (pAb, frame, FALSE, &Ymin, &Ymax);
	TtaDisplayMessage (INFO, TtaGetMessage(LIB, CREATING_BOX), AbsBoxType (pAb));
	/* On retablit les positions par rapport a la fenetre */
	Xmin -= pFrame->FrXOrg;
	Xmax -= pFrame->FrXOrg;
	Ymin -= pFrame->FrYOrg;
	Ymax -= pFrame->FrYOrg;
	UserGeometryCreate (frame, &x, &y, xr, yr, &large, &haut,
		      Xmin, Xmax, Ymin, Ymax,
		      pAb->AbHorizPos.PosUserSpecified,
		      pAb->AbVertPos.PosUserSpecified,
		      pAb->AbWidth.DimUserSpecified,
		      pAb->AbHeight.DimUserSpecified);

	/* Notification de la boite saisie */
	pAb->AbHorizPos.PosUserSpecified = FALSE;
	pAb->AbVertPos.PosUserSpecified = FALSE;
	pAb->AbWidth.DimUserSpecified = FALSE;
	pAb->AbHeight.DimUserSpecified = FALSE;
	x = x + pFrame->FrXOrg - pBox->BxXOrg;
	y = y + pFrame->FrYOrg - pBox->BxYOrg;
	NouvPosition (pAb, x, y, frame, TRUE);
	large = large - pBox->BxWidth;
	haut = haut - pBox->BxHeight;
	NouvDimension (pAb, large, haut, frame, TRUE);
     }

   /* Traitement de la creation interactive termine */
   EnCreation = FALSE;
}


/* ---------------------------------------------------------------------- */
/* |    DesCaractere cherche le caractere affiche dans la boite pBox    | */
/* |            avec un decalage x. Rend le pointeur sur le buffer qui  | */
/* |            contient le caractere designe', l'index du caractere    | */
/* |            dans le buffer, l'index du caractere dans la boite et   | */
/* |            le nombre de blancs qui le precedent dans la boite.     | */
/* |            Met a jour la valeur x.                                 | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                DesCaractere (PtrBox pBox, PtrTextBuffer * adbuff, int *x, int *icar, int *charsNumber, int *spacesNumber)
#else  /* __STDC__ */
void                DesCaractere (pBox, adbuff, x, icar, charsNumber, spacesNumber)
PtrBox            pBox;
PtrTextBuffer     *adbuff;
int                *x;
int                *icar;
int                *charsNumber;
int                *spacesNumber;

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
   *charsNumber = 0;
   *spacesNumber = 0;
   car = '\0';
   lgcar = 0;
   if (pBox->BxNChars == 0 || *x <= 0)
     {
	*x = 0;
	*adbuff = pBox->BxBuffer;
	*icar = pBox->BxFirstChar;
     }
   else
     {
	font = pBox->BxFont;
	dx = 0;
	newcar = pBox->BxFirstChar;
	*icar = newcar;
	*adbuff = pBox->BxBuffer;
	reste = pBox->BxNChars;
	/* Calcule la largeur des blancs */
	if (pBox->BxSpaceWidth == 0)
	  {
	     lgbl = CarWidth (BLANC, font);
	     restbl = 0;
	  }
	else
	  {
	     lgbl = pBox->BxSpaceWidth;
	     restbl = pBox->BxNPixels;
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
		  (*spacesNumber)++;
		  if (restbl > 0)
		    {
		       dx++;
		       restbl--;
		    }
	       }

	     dx += lgcar;
	     (*charsNumber)++;

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
		  if (*spacesNumber > 0 && pBox->BxNPixels >= *spacesNumber)
		     (*x)--;
		  (*spacesNumber)--;
	       }

	     if (newcar == 1)
		*adbuff = (*adbuff)->BuPrevious;
	     (*charsNumber)--;
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
