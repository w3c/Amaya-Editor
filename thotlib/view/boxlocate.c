/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * Warning:
 * This module is part of the Thot library, which was originally
 * developed in French. That's why some comments are still in
 * French, but their translation is in progress and the full module
 * will be available in English in the next release.
 * 
 */
 
/* 
 * locate what is designated in Concret Image.
 *
 * Author: I. Vatton (INRIA)
 *
 */

#include "ustring.h"
#include "libmsg.h"
#include "thot_sys.h"

#include "constmedia.h"
#include "typemedia.h"
#include "message.h"
#include "appdialogue.h"

#define THOT_EXPORT extern
#include "boxes_tv.h"
#include "frame_tv.h"
#include "units_tv.h"
#include "platform_tv.h"
#include "appdialogue_tv.h"

#include "absboxes_f.h"
#include "appdialogue_tv.h"
#include "applicationapi_f.h"
#include "appli_f.h"
#include "boxmoves_f.h"
#include "boxlocate_f.h"
#include "boxselection_f.h"
#include "buildboxes_f.h"
#include "buildlines_f.h"
#include "callback_f.h"
#include "changepresent_f.h"
#include "exceptions_f.h"
#include "font_f.h"
#include "frame_f.h"
#include "geom_f.h"
#include "presentationapi_f.h"
#include "structcreation_f.h"
#include "structselect_f.h"
#include "textcommands_f.h"
#include "tree_f.h"
#include "undo_f.h"
#include "units_f.h"
#include "views_f.h"

#define Y_RATIO 200		/* penalisation en Y */
#define ANCHOR_SIZE 3		/* taille des ancres */


/*----------------------------------------------------------------------
  LocateSelectionInView finds out the selected Abstract Box and if it's
  a TEXT element the selected character(s).
  The parameter button says what the editor wants to do with this
  new selection:
    0 -> extend the current selection
    1 -> extend the current selection by draging
    2 -> replace the old selection
    3 -> activate a link
    4 -> click an element
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                LocateSelectionInView (int frame, int x, int y, int button)
#else  /* __STDC__ */
void                LocateSelectionInView (frame, x, y, button)
int                 frame;
int                 x;
int                 y;
int                 button;

#endif /* __STDC__ */
{
   int                 index;
   PtrBox              pBox;
   PtrTextBuffer       pBuffer;
   PtrAbstractBox      pAb;
   NotifyElement       notifyEl;
   PtrElement          el;
   int                 charsNumber;
   int                 spacesNumber;
   ViewFrame          *pFrame;

   if (frame >= 1)
     {
	/* recherche si une boite terminale est designee */
	pFrame = &ViewFrameTable[frame - 1];
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
		pBox = GetClickedLeafBox (frame, x, y);
	  }
	if (pBox != NULL)
	  {
	     pAb = pBox->BxAbstractBox;
	     if (pAb->AbLeafType == LtText &&
		 (!pAb->AbPresentationBox || pAb->AbCanBeModified))
	       {
		  x -= pBox->BxXOrg;
		  LocateClickedChar (pBox, &pBuffer, &x, &index, &charsNumber, &spacesNumber);
		  charsNumber = pBox->BxIndChar + charsNumber + 1;
	       }
	  }
	else
	   pAb = NULL;

	CloseInsertion ();
	if (pAb != NULL)
	   /* Initialisation de la selection */
	   if (button == 3)
	      ChangeSelection (frame, pAb, charsNumber, FALSE, TRUE, TRUE, FALSE);
	   else if (button == 2)
	      ChangeSelection (frame, pAb, charsNumber, FALSE, TRUE, FALSE, FALSE);
	/* Extension de la selection */
	   else if (button == 0)
	      ChangeSelection (frame, pAb, charsNumber, TRUE, TRUE, FALSE, FALSE);
	   else if (button == 1)
	      ChangeSelection (frame, pAb, charsNumber, TRUE, TRUE, FALSE, TRUE);
          else /* button == 4 */
	    {
	      /* send event TteElemActivate.Pre to the application */
	      el = pAb->AbElement;
	      notifyEl.event = TteElemClick;
	      notifyEl.document = FrameTable[frame].FrDoc;
	      notifyEl.element = (Element) el;
	      notifyEl.elementType.ElTypeNum = el->ElTypeNumber;
	      notifyEl.elementType.ElSSchema = (SSchema) (el->ElStructSchema);
	      notifyEl.position = 0;
	      if (CallEventType ((NotifyEvent *) & notifyEl, TRUE))
		/* the application asks Thot to do nothing */
		return;
	      /* send event TteElemActivate.Pre to the application */
	      CallEventType ((NotifyEvent *) & notifyEl, FALSE);
            }
     }
}


/*----------------------------------------------------------------------
   GetDistance rend 0 si value dans l'intervalle de delta		
   sinon la  distance absloue - delta.       	       		
  ----------------------------------------------------------------------*/
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


/*----------------------------------------------------------------------
   GetBoxDistance calcule la distance d'un point xRef, yRef a` une    
   boite. On penalise favorise la distance verticale.      
  ----------------------------------------------------------------------*/
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
   int                 value;

   /* prend le centre de la boite */
   width /= 2;
   x += width;
   height /= 2;
   y += height;
   value = GetDistance (xRef - x, width) + Y_RATIO * GetDistance (yRef - y, height);
   return (value);
}

/*----------------------------------------------------------------------
   IsOnPolyline teste qu'un point x,y est sur un segment P1(x1,y1)  
   P2(x2,y2) avec une certaine precision DELTA_SEL.         
   On teste l'appartenance du point a` un rectangle cree    
   par e'largissement de DELTA_SEL autour des deux points.  
   Le test est fait apres une rotation permettant de se     
   ramener a` un rectangle horizontal.                      
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static ThotBool     IsOnPolyline (int x, int y, int x1, int y1, int x2, int y2)
#else  /* __STDC__ */
static ThotBool     IsOnPolyline (x, y, x1, y1, x2, y2)
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
   nX = (int) (x * cs + y * ss);
   nY = (int) (y * cs - x * ss);
   /* test */
   return (nY <= DELTA_SEL
	   && nY >= -DELTA_SEL
	   && nX >= -DELTA_SEL
	   && nX <= ra + DELTA_SEL);
}


/*----------------------------------------------------------------------
   InPolyline indique si le point x,y se trouve a`                 
   l'inte'rieur de la polyline ou non.                            
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static ThotBool     InPolyline (PtrAbstractBox pAb, int x, int y)
#else  /* __STDC__ */
static ThotBool     InPolyline (pAb, x, y)
PtrAbstractBox      pAb;
int                 x;
int                 y;

#endif /* __STDC__ */
{
   PtrTextBuffer       buff, pLastBuffer;
   int                 cross;
   int                 i, max;
   int                 prevX, prevY;
   int                 nextX, nextY;
   PtrBox              box;
   ThotBool            OK;

   box = pAb->AbBox;
   x -= box->BxXOrg;
   y -= box->BxYOrg;
   max = box->BxNChars;
   if (max < 4)
      /* il n'y a pas de surface a l'interieur de la polyline */
      return (FALSE);

   /* premier et dernier point de la polyline */
   pLastBuffer = buff = box->BxBuffer;
   i = 1;
   while (pLastBuffer->BuNext != NULL)
      pLastBuffer = pLastBuffer->BuNext;
   max = pLastBuffer->BuLength - 1;

   cross = 0;
   nextX = PointToPixel (buff->BuPoints[i].XCoord / 1000);
   nextY = PointToPixel (buff->BuPoints[i].YCoord / 1000);
   prevX = PointToPixel (pLastBuffer->BuPoints[max].XCoord / 1000);
   prevY = PointToPixel (pLastBuffer->BuPoints[max].YCoord / 1000);
   if ((prevY >= y) != (nextY >= y))
     {
	/* y entre nextY et prevY */
	if ((OK = (prevX >= x)) == (nextX >= x))
	  {
	     /* x du meme cote des deux extremites */
	     if (OK)
		/* nextX et prevX >= x */
		cross++;
	  }
	else
	   /* x entre deux extremites */
	   cross += (prevX - (prevY - y) * (nextX - prevX) / (nextY - prevY)) >= x;
     }

   i++;
   while (i <= max || buff != pLastBuffer)
     {
	prevX = nextX;
	prevY = nextY;
	nextX = PointToPixel (buff->BuPoints[i].XCoord / 1000);
	nextY = PointToPixel (buff->BuPoints[i].YCoord / 1000);
	if (prevY >= y)
	  {
	     while ((i <= max || buff != pLastBuffer) && (nextY >= y))
	       {
		  i++;		/* changement de point */
		  if (i >= buff->BuLength && buff != pLastBuffer)
		    {
		       buff = buff->BuNext;	/* passe au buffer suivant */
		       i = 0;
		    }
		  prevY = nextY;
		  prevX = nextX;
		  nextX = PointToPixel (buff->BuPoints[i].XCoord / 1000);
		  nextY = PointToPixel (buff->BuPoints[i].YCoord / 1000);
	       }

	     if (i > max && buff == pLastBuffer)
		break;

	     if ((OK = (prevX >= x)) == (nextX >= x))
	       {
		  if (OK)
		     cross++;
	       }
	     else
		cross += (prevX - (prevY - y) * (nextX - prevX) / (nextY - prevY)) >= x;
	  }
	else
	  {
	     while ((i <= max || buff != pLastBuffer) && (nextY < y))
	       {
		  i++;		/* changement de point */
		  if (i >= buff->BuLength && buff != pLastBuffer)
		    {
		       buff = buff->BuNext;	/* passe au buffer suivant */
		       i = 0;
		    }
		  prevY = nextY;
		  prevX = nextX;
		  nextX = PointToPixel (buff->BuPoints[i].XCoord / 1000);
		  nextY = PointToPixel (buff->BuPoints[i].YCoord / 1000);
	       }

	     if (i > max && buff == pLastBuffer)
		break;

	     if ((OK = (prevX >= x)) == (nextX >= x))
	       {
		  if (OK)
		     cross++;
	       }
	     else
		cross += (prevX - (prevY - y) * (nextX - prevX) / (nextY - prevY)) >= x;
	  }
     }
   OK = (ThotBool) (cross & 0x01);
   return (OK);
}


/*----------------------------------------------------------------------
   GetPolylinePoint teste qu'un point x,y est sur un segment de la    
   boi^te polyline.                                        
   Si oui, retourne l'adresse de la boi^te correspondante et le    
   point de contro^le se'lectionne' (0 pour toute la boi^te).      
   sinon, la valeur NULL.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static PtrBox       GetPolylinePoint (PtrAbstractBox pAb, int x, int y, int *pointselect)
#else  /* __STDC__ */
static PtrBox       GetPolylinePoint (pAb, x, y, pointselect)
PtrAbstractBox      pAb;
int                 x;
int                 y;
int                *pointselect;

#endif /* __STDC__ */
{
   int                 i, j, nb;
   int                 X1, Y1;
   int                 X2, Y2;
   PtrTextBuffer       adbuff;
   PtrBox              box;
   ThotBool            OK;

   Y1 = 0;
   box = pAb->AbBox;
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
   if (pAb->AbPolyLineShape == 'p' || pAb->AbPolyLineShape == 's')
     {
	X2 = PointToPixel (box->BxBuffer->BuPoints[1].XCoord / 1000);
	Y2 = PointToPixel (box->BxBuffer->BuPoints[1].YCoord / 1000);
	OK = IsOnPolyline (x, y, X1, Y1, X2, Y2);
	if (OK)
	   return (box);
     }
   return (NULL);
}

/*----------------------------------------------------------------------
   InShape indique si le point x,y se trouve a`            
   l'inte'rieur du graphique ou non.                              
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static ThotBool     InShape (PtrAbstractBox pAb, int x, int y)
#else  /* __STDC__ */
static ThotBool     InShape (pAb, x, y)
PtrAbstractBox      pAb;
int                 x;
int                 y;

#endif /* __STDC__ */
{
  int                 point[8][2];
  int                 cross;
  int                 i, max;
  int                 prevX, prevY;
  int                 nextX, nextY;
  int                 arc;
  float               value1, value2;
  PtrBox              box;
  ThotBool            OK;

  box = pAb->AbBox;
  x -= box->BxXOrg;
  y -= box->BxYOrg;
  max = 0;

  /* Est-ce un point caracteristique specifique du graphique ? */
  switch (pAb->AbRealShape)
    {
    case SPACE:
    case TEXT('R'):
    case TEXT('0'):
    case TEXT('1'):
    case TEXT('2'):
    case TEXT('3'):
    case TEXT('4'):
    case TEXT('5'):
    case TEXT('6'):
    case TEXT('7'):
    case TEXT('8'):		/* rectangles */
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
      arc = (int) ((3 * DOT_PER_INCHE) / 25.4 + 0.5);
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
    case 'a':		/* circles */
    case 'c':		/* ovals */
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
   cross = 0;
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
		cross++;
	  }
	else
	   /* x entre deux extremites */
	   cross += (prevX - (prevY - y) * (nextX - prevX) / (nextY - prevY)) >= x;
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
	     while (i <= max && nextY >= y)
	       {
		  i++;		/* changement de point */
		  prevY = nextY;
		  prevX = nextX;
		  if (i > max)
		    {
		      nextX = 0;
		      nextY = 0;
		    }
		  else
		    {
		      nextX = point[i][0];
		      nextY = point[i][1];
		    }
	       }

	     if (i > max)
		break;

	     if ((OK = (prevX >= x)) == (nextX >= x))
	       {
		  if (OK)
		     cross++;
	       }
	     else
		cross += (prevX - (prevY - y) * (nextX - prevX) / (nextY - prevY)) >= x;
	  }
	else
	  {
	     while (i <= max && nextY < y)
	       {
		  i++;		/* changement de point */
		  prevY = nextY;
		  prevX = nextX;
		  if (i > max)
		    {
		      nextX = 0;
		      nextY = 0;
		    }
		  else
		    {
		      nextX = point[i][0];
		      nextY = point[i][1];
		    }
	       }

	     if (i > max)
		break;

	     if ((OK = (prevX >= x)) == (nextX >= x))
	       {
		  if (OK)
		     cross++;
	       }
	     else
		cross += (prevX - (prevY - y) * (nextX - prevX) / (nextY - prevY)) >= x;
	  }
     }
   OK = (ThotBool) (cross & 0x01);
   return (OK);
}


/*----------------------------------------------------------------------
  IsOnShape teste si le point x,y appartient au pave' graphique pAb.
  Si oui, retourne l'adresse de la boite correspondante sinon, la valeur
  NULL.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static PtrBox       IsOnShape (PtrAbstractBox pAb, int x, int y)
#else  /* __STDC__ */
static PtrBox       IsOnShape (pAb, x, y)
PtrAbstractBox      pAb;
int                 x;
int                 y;
#endif /* __STDC__ */
{
  PtrBox              pBox;
  int                 controlPoint;
  int                 arc;

  /* coordonnees relatives a la boite (calculs plus simples) */
  pBox = pAb->AbBox;
  x -= pBox->BxXOrg;
  y -= pBox->BxYOrg;

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
      controlPoint = 1;
    else if (y > pBox->BxHeight / 2 - DELTA_SEL &&
	     y < pBox->BxHeight / 2 + DELTA_SEL)
      controlPoint = 8;
    else if (y > pBox->BxHeight - 10)
      controlPoint = 7;
    else
      controlPoint = 0;
  else if (x > pBox->BxWidth / 2 - DELTA_SEL &&
	   x < pBox->BxWidth / 2 + DELTA_SEL)
    if (y < DELTA_SEL)
      controlPoint = 2;
    else if (y > pBox->BxHeight - DELTA_SEL)
      controlPoint = 6;
    else
      controlPoint = 0;
  else if (x > pBox->BxWidth - DELTA_SEL)
    if (y < DELTA_SEL)
      controlPoint = 3;
    else if (y > pBox->BxHeight / 2 - DELTA_SEL &&
	     y < pBox->BxHeight / 2 + DELTA_SEL)
      controlPoint = 4;
    else if (y > pBox->BxHeight - 10)
      controlPoint = 5;
    else
      controlPoint = 0;
  else
    controlPoint = 0;

  /* Est-ce un point caracteristique specifique du graphique ? */
  switch (pAb->AbRealShape)
    {
    case SPACE:
    case TEXT('R'):
    case TEXT('0'):
    case TEXT('1'):
    case TEXT('2'):
    case TEXT('3'):
    case TEXT('4'):
    case TEXT('5'):
    case TEXT('6'):
    case TEXT('7'):
    case TEXT('8'):
      if (IsOnPolyline (x, y, 0, 0, pBox->BxWidth, 0) ||
	  IsOnPolyline (x, y, 0, pBox->BxHeight, pBox->BxWidth, pBox->BxHeight) ||
	  IsOnPolyline (x, y, 0, 0, 0, pBox->BxHeight) ||
	  IsOnPolyline (x, y, pBox->BxWidth, 0, pBox->BxWidth, pBox->BxHeight))
	return (pBox);
      break;
    case 'L':
      if (IsOnPolyline (x, y, 0, pBox->BxHeight / 2, pBox->BxWidth / 2, 0) ||
	  IsOnPolyline (x, y, 0, pBox->BxHeight / 2, pBox->BxWidth / 2, pBox->BxHeight) ||
	  IsOnPolyline (x, y, pBox->BxWidth, pBox->BxHeight / 2, pBox->BxWidth / 2, 0) ||
	  IsOnPolyline (x, y, pBox->BxWidth, pBox->BxHeight / 2, pBox->BxWidth / 2, pBox->BxHeight))
	return (pBox);
      break;
    case 'C':
    case 'P':
      arc = (int) ((3 * DOT_PER_INCHE) / 25.4 + 0.5);
      if (IsOnPolyline (x, y, arc, 0, pBox->BxWidth - arc, 0) ||
	  IsOnPolyline (x, y, 0, arc, 0, pBox->BxHeight - arc) ||
	  IsOnPolyline (x, y, arc, pBox->BxHeight, pBox->BxWidth - arc, pBox->BxHeight) ||
	  IsOnPolyline (x, y, pBox->BxWidth, arc, pBox->BxWidth, pBox->BxHeight - arc))
	return (pBox);
      break;
    case 'a':
    case 'c':
    case 'Q':
      if (controlPoint == 2 || controlPoint == 4 || controlPoint == 6 || controlPoint == 8)
	return (pBox);
      break;
    case 'W':
      if (controlPoint == 1 || controlPoint == 3 || controlPoint == 5 ||
	  IsOnPolyline (x, y, 0, 0, pBox->BxWidth, 0) ||
	  IsOnPolyline (x, y, pBox->BxWidth, 0, pBox->BxWidth, pBox->BxHeight))
	return (pBox);
      break;
    case 'X':
      if (controlPoint == 3 || controlPoint == 5 || controlPoint == 7 ||
	  IsOnPolyline (x, y, pBox->BxWidth, 0, pBox->BxWidth, pBox->BxHeight) ||
	  IsOnPolyline (x, y, pBox->BxWidth, pBox->BxHeight, 0, pBox->BxHeight))
	return (pBox);
      break;
    case 'Y':
      if (controlPoint == 1 || controlPoint == 5 || controlPoint == 7 ||
	  IsOnPolyline (x, y, pBox->BxWidth, pBox->BxHeight, 0, pBox->BxHeight) ||
	  IsOnPolyline (x, y, 0, pBox->BxHeight, 0, 0))
	return (pBox);
      break;
    case 'Z':
      if (controlPoint == 1 || controlPoint == 3 || controlPoint == 7 ||
	  IsOnPolyline (x, y, 0, pBox->BxHeight, 0, 0) ||
	  IsOnPolyline (x, y, 0, 0, pBox->BxWidth, 0))
	return (pBox);
      break;
    case 'h':
    case '<':
    case '>':
      if (controlPoint == 4 || controlPoint == 8 ||
	  IsOnPolyline (x, y, 0, pBox->BxHeight / 2, pBox->BxWidth, pBox->BxHeight / 2))
	return (pBox);
      break;
    case 't':
      if (controlPoint == 1 || controlPoint == 2 || controlPoint == 3 ||
	  IsOnPolyline (x, y, 0, 0, pBox->BxWidth, 0))
	return (pBox);
      break;
    case 'b':
      if (controlPoint == 5 || controlPoint == 6 || controlPoint == 7 ||
	  IsOnPolyline (x, y, pBox->BxWidth, pBox->BxHeight, 0, pBox->BxHeight))
	return (pBox);
      break;
    case 'v':
    case '^':
    case 'V':
      if (controlPoint == 2 || controlPoint == 6 ||
	  IsOnPolyline (x, y, pBox->BxWidth / 2, 0, pBox->BxWidth / 2, pBox->BxHeight))
	return (pBox);
      break;
    case 'l':
      if (controlPoint == 1 || controlPoint == 7 || controlPoint == 8 ||
	  IsOnPolyline (x, y, 0, pBox->BxHeight, 0, 0))
	return (pBox);
      break;
    case 'r':
      if (controlPoint == 3 || controlPoint == 4 || controlPoint == 5 ||
	  IsOnPolyline (x, y, pBox->BxWidth, 0, pBox->BxWidth, pBox->BxHeight))
	return (pBox);
      break;
    case '\\':
    case 'O':
    case 'e':
      if (controlPoint == 1 || controlPoint == 5 ||
	  IsOnPolyline (x, y, 0, 0, pBox->BxWidth, pBox->BxHeight))
	return (pBox);
      break;
    case '/':
    case 'o':
    case 'E':
      if (controlPoint == 3 || controlPoint == 7 ||
	  IsOnPolyline (x, y, 0, pBox->BxHeight, pBox->BxWidth, 0))
	return (pBox);
      break;
    default:
      break;
    }
  return (NULL);
}


/*----------------------------------------------------------------------
   GetClickedAbsBox recherche le pave englobant le point designe' par 
   xRef,yRef de la fenetre frame.                          
   La fonction rend le pointeur sur le plus elementaire    
   des paves qui englobe le point designe ou NULL.         
  ----------------------------------------------------------------------*/
#ifdef __STDC__
PtrAbstractBox      GetClickedAbsBox (int frame, int xRef, int yRef)
#else  /* __STDC__ */
PtrAbstractBox      GetClickedAbsBox (frame, xRef, yRef)
int                 frame;
int                 xRef;
int                 yRef;

#endif /* __STDC__ */
{
   ViewFrame          *pFrame;
   PtrBox              pBox;
   int                 pointselect;

   pFrame = &ViewFrameTable[frame - 1];
   pBox = NULL;
   if (pFrame->FrAbstractBox != NULL)
      if (ThotLocalActions[T_selecbox] != NULL)
	 (*ThotLocalActions[T_selecbox]) (&pBox, pFrame->FrAbstractBox, frame, xRef + pFrame->FrXOrg,
				       yRef + pFrame->FrYOrg, &pointselect);
   if (pBox == NULL)
      return (NULL);
   else
      return (pBox->BxAbstractBox);
}


/*----------------------------------------------------------------------
   GetEnclosingClickedBox teste si le point x, y appartient au pave' 
   pAb.                                                            
   Si oui, retourne l'adresse du pave' correspondant               
   sinon, la valeur NULL.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
PtrBox              GetEnclosingClickedBox (PtrAbstractBox pAb, int higherX, int lowerX, int y, int *pointselect)
#else  /* __STDC__ */
PtrBox              GetEnclosingClickedBox (pAb, higherX, lowerX, y, pointselect)
PtrAbstractBox      pAb;
int                 higherX;
int                 lowerX;
int                 y;
int                *pointselect;

#endif /* __STDC__ */
{
   PtrBox              pBox;

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
		      pBox->BxXOrg <= lowerX &&
		      pBox->BxXOrg + pBox->BxWidth >= higherX &&
		      pBox->BxYOrg <= y &&
		      pBox->BxYOrg + pBox->BxHeight >= y)
		     return (pBox);
	       }
	     return (NULL);
	  }
	else if (pBox->BxType == BoGhost)
	  /* dummy box */
	   return (NULL);
	else if (pAb->AbLeafType == LtGraphics &&
		 pAb->AbPresentationBox &&
		 pAb->AbShape == '0')
	  /* it's also a dummy box */
	  return (NULL);
	/* Si le pave englobe le point designe */
	else if (pBox->BxXOrg <= lowerX
		 && pBox->BxXOrg + pBox->BxWidth >= higherX
		 && pBox->BxYOrg <= y
		 && pBox->BxYOrg + pBox->BxHeight >= y)
	   /* Si c'est un pave graphique */
	   if (pAb->AbLeafType == LtGraphics && pAb->AbVolume != 0)
	     {
		pBox = IsOnShape (pAb, lowerX, y);
		if (pBox != NULL)
		   return (pBox);
		/* le point n'est pas sur un des segments */
		if (InShape (pAb, lowerX, y))
		   return (pAb->AbBox);
		else
		   return (pBox);
	     }
	   else if (pAb->AbLeafType == LtPolyLine && pAb->AbVolume > 2)
	     {
		/* La polyline contient au moins un segment */
		pBox = GetPolylinePoint (pAb, lowerX, y, pointselect);
		if (pBox != NULL)
		   return (pBox);
		/* le point n'est pas sur un des segments */
		if ((pAb->AbPolyLineShape == 'p' || pAb->AbPolyLineShape == 's')
		    && InPolyline (pAb, lowerX, y))
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


/*----------------------------------------------------------------------
   GetLeafBox returns the leaf box located at the position x+xDelta
   y+yDelta from pSourceBox box.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
PtrBox         GetLeafBox (PtrBox pSourceBox, int frame, int *x, int *y, int xDelta, int yDelta)
#else  /* __STDC__ */
PtrBox         GetLeafBox (pSourceBox, frame, x, y, xDelta, yDelta)
PtrBox              pSourceBox;
int                 frame;
int                 *x;
int                 *y;
int                 xDelta;
int                 yDelta;
#endif /* __STDC__ */
{
  int                 i;
  PtrBox              pBox, pLimitBox, lastBox;
  PtrLine             pLine;
  int                 max;
  int                 h, lastY;
  ThotBool            found;

  found = FALSE;
  lastBox = NULL;
  lastY = *y;
  while (!found)
    {
      pBox = pSourceBox;
      max = ViewFrameTable[frame - 1].FrAbstractBox->AbBox->BxWidth;
      /* locate the last box in the line */
      if (xDelta > 0)
	{
	  pLimitBox = GetClickedLeafBox (frame, max, *y);
	  if (pLimitBox == NULL)
	    pLimitBox = pSourceBox;
	}
      else if (xDelta < 0)
	{
	  pLimitBox = GetClickedLeafBox (frame, 0, *y);
	  if (pLimitBox == NULL)
	    pLimitBox = pSourceBox;
	}
      else
	pLimitBox = NULL;

      i = 0;
      found = TRUE;
      while (pBox == pSourceBox && i < 200 && found)
	{
	  i++;
	  *x += xDelta;
	  *y += yDelta;
	  /* Take the leaf box here */
	  pBox = GetClickedLeafBox (frame, *x, *y);
	  if (pBox == NULL)
	    pBox = pSourceBox;
	  else if (TypeHasException (ExcNoSelect,
				     pBox->BxAbstractBox->AbElement->ElTypeNumber,
				     pBox->BxAbstractBox->AbElement->ElStructSchema))
	    pBox = pSourceBox;
	  if (pBox == pSourceBox || pBox->BxAbstractBox->AbBox == pSourceBox)
	    {
	      /* compute the height to be moved */
	      pLine = SearchLine (pBox);
	      if (pLine != NULL)
		h = pLine->LiHeight / 2;
	      else
		h = 10;
	      if (xDelta > 0 && pLimitBox == pBox)
		{
		  /* move one line down */
		  *x = 0;
		  *y = pBox->BxYOrg + pBox->BxHeight;
		  xDelta = 0;
		  yDelta = h;
		  found = FALSE;
		}
	      else if (xDelta < 0 && pLimitBox == pBox)
		{
		  /* move one line up */
		  *x = max;
		  *y = pBox->BxYOrg;
		  xDelta = 0;
		  yDelta = -h;
		  found = FALSE;
		}
	    }
	  else if (IsParentBox (pSourceBox, pBox))
	    {
	      if (xDelta > 0)
		{
		  /* move to the end of the box */
		  *y = pSourceBox->BxYOrg + pSourceBox->BxHeight;
		  xDelta = 0;
		  yDelta = 0;
		  found = FALSE;
		}
	      else if (xDelta < 0)
		{
		  /* move to the beginning of the box */
		  *y = pSourceBox->BxYOrg;
		  xDelta = 0;
		  yDelta = 0;
		  found = FALSE;
		}
	      else
		{
		  /* avoid to loop on the last box */
		  if (*y != lastY || lastBox != pBox)
		    {
		      lastY = *y;
		      lastBox = pBox;
		      /* continue the search */
		      *y = pBox->BxYOrg + yDelta;
		      found = FALSE;
		    }
		}
	    }
	  else if (pBox->BxAbstractBox->AbLeafType != LtText && pBox->BxNChars != 0)
	    {
	      /* the box doesn't match, skip over */
	      if (xDelta > 0)
		*x = pBox->BxXOrg + pBox->BxWidth;
	      else if (xDelta < 0)
		*x = pBox->BxXOrg;
	      
	      if (yDelta > 0)
		*y = pBox->BxYOrg + pBox->BxHeight;
	      else if (yDelta < 0)
		*y = pBox->BxYOrg;
	      pBox = pSourceBox;
	    }
	}
    }
  return (pBox);
}

/*----------------------------------------------------------------------
   GetGetMinimumDistance calcule la distance d'une ancre x,y au point
   xRef,yRef. La fonction rend la distance minimale        
   entre la distance calcule'e et dist.                    
  ----------------------------------------------------------------------*/
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
   int                 value;

   value = GetDistance (x - xRef, ANCHOR_SIZE) + GetDistance (y - yRef, ANCHOR_SIZE) * Y_RATIO;
   if (value < dist)
      return (value);
   else
      return (dist);
}

/*----------------------------------------------------------------------
   GetShapeDistance calcule la distance d'un point xRef,yRef a`     
   un point de la boite graphique (min des distances aux   
   ancres).                                                
   Cette selection est limitee aux boites terminale.       
   Rend la distance de la boite au point.                  
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                 GetShapeDistance (int xRef, int yRef, PtrBox pBox, int value)
#else  /* __STDC__ */
int                 GetShapeDistance (xRef, yRef, pBox, value)
int                 xRef;
int                 yRef;
PtrBox              pBox;
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



/*----------------------------------------------------------------------
   GetClickedLeafBox recherche la boite terminale situee au point    
   xRef,yRef dans l'image concrete.                                
  ----------------------------------------------------------------------*/
#ifdef __STDC__
PtrBox              GetClickedLeafBox (int frame, int xRef, int yRef)
#else  /* __STDC__ */
PtrBox              GetClickedLeafBox (frame, xRef, yRef)
int                 frame;
int                 xRef;
int                 yRef;

#endif /* __STDC__ */
{
   PtrAbstractBox      pAb;
   PtrBox              pSelBox, pBox;
   PtrBox              pCurrentBox;
   int                 max;
   int                 pointIndex;
   int                 d;
   ViewFrame          *pFrame;

   pBox = NULL;
   pSelBox = NULL;
   /* au-dela de max, on n'accepte pas la selection */
   max = 2000;
   pFrame = &ViewFrameTable[frame - 1];

   if (pFrame->FrAbstractBox != NULL)
      pBox = pFrame->FrAbstractBox->AbBox;
   if (pBox != NULL)
     {
	pBox = pBox->BxNext;
	while (pBox != NULL)
	  {
	     pAb = pBox->BxAbstractBox;
	     if (pAb->AbVisibility >= pFrame->FrVisibility
		 && (!pAb->AbPresentationBox || pAb->AbCanBeModified))
	       {
		  if (pAb->AbLeafType == LtGraphics || pAb->AbLeafType == LtPolyLine)
		    {
		       pCurrentBox = GetEnclosingClickedBox (pAb, xRef, xRef, yRef, &pointIndex);
		       if (pCurrentBox == NULL)
			  d = max + 1;
		       else
			  d = 0;
		    }
		  else if (pAb->AbLeafType == LtSymbol && pAb->AbShape == 'r')
		     /* glitch pour le symbole racine */
		     d = GetShapeDistance (xRef, yRef, pBox, 1);
		  else if (pAb->AbLeafType == LtText
			   || pAb->AbLeafType == LtSymbol
			   || pAb->AbLeafType == LtPicture
		     /* ou une boite composee vide */
		   || (pAb->AbLeafType == LtCompound && pAb->AbVolume == 0))
		     d = GetBoxDistance (xRef, yRef, pBox->BxXOrg, pBox->BxYOrg, pBox->BxWidth, pBox->BxHeight);
		  else
		     d = max + 1;

		  /* Prend l'element le plus proche */
		  if (d < max)
		    {
		       max = d;
		       pSelBox = pBox;
		    }
		  else if (d == max)
		    {
		       /* Si c'est la premiere boite trouvee */
		       if (pSelBox == NULL)
			 {
			    max = d;
			    pSelBox = pBox;
			 }
		       /* Si la boite est sur un plan au dessus de la precedente */
		       else if (pSelBox->BxAbstractBox->AbDepth > pBox->BxAbstractBox->AbDepth)
			 {
			    max = d;
			    pSelBox = pBox;
			 }
		    }
	       }
	     pBox = pBox->BxNext;
	  }
     }
   return pSelBox;
}


/*----------------------------------------------------------------------
   GiveMovingArea de'termine les limites de de'placement de la boite 
   horizontalement/verticalement.                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         GiveMovingArea (PtrAbstractBox pAb, int frame, ThotBool horizRef, int *min, int *max)
#else  /* __STDC__ */
static void         GiveMovingArea (pAb, frame, horizRef, min, max)
PtrAbstractBox      pAb;
int                 frame;
ThotBool            horizRef;
int                *min;
int                *max;

#endif /* __STDC__ */
{
#ifdef IV
   PtrAbstractBox      pParentAb;
#endif /* IV */
   /* Valeurs par defaut */
   *min = 0;
   *max = 100000;

   /* C'est la boite racine */
   if (pAb == ViewFrameTable[frame - 1].FrAbstractBox)
     {
	if (horizRef)
	  {
	     /* Est-ce que la boite depend de la fenetre */
	     if ((pAb->AbWidth.DimValue == 0)
		 || (pAb->AbBox->BxHorizEdge == Right))
		*max = pAb->AbBox->BxWidth;
	  }
	else
	  {
	     if ((pAb->AbHeight.DimValue == 0)
		 || (pAb->AbBox->BxVertEdge == Bottom))
		*max = pAb->AbBox->BxHeight;
	  }
     }
   /* Dans les autres cas */
   else if (horizRef)
     {
       *min = -50;
       *max = 100000;
#ifdef IV
	/* Si le pave est englobe depend de son englobant direct */
	if (pAb->AbHorizEnclosing)
	   pParentAb = pAb->AbEnclosing;
	else
	   pParentAb = ViewFrameTable[frame - 1].FrAbstractBox;

	/* A priori limite dans l'espace de la boite englobante */
	*min = pParentAb->AbBox->BxXOrg;
	*max = *min + pParentAb->AbBox->BxWidth;

	/* Si la boite englobante prend la taille de son contenu */
	/* et que le pave n'est pas attache a l'englobante       */
	if (pParentAb->AbBox->BxContentWidth)
	   switch (pAb->AbBox->BxHorizEdge)
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
#endif /* IV */
     }
   else
     {
       *min = -50;
       *max = 100000;
#ifdef IV
	/* Si le pave est englobe depend de son englobant direct */
	if (pAb->AbVertEnclosing && pAb->AbEnclosing != NULL)
	  {
	    pParentAb = pAb;
	    do
	      pParentAb = pParentAb->AbEnclosing;
	    while (pParentAb->AbBox->BxType == BoGhost);
	  }
	else
	   pParentAb = ViewFrameTable[frame - 1].FrAbstractBox;

	/* A priori limite dans l'espace de la boite englobante */
	*min = pParentAb->AbBox->BxYOrg;
	*max = *min + pParentAb->AbBox->BxHeight;

	/* Si la boite englobante prend la taille de son contenu */
	if (pParentAb->AbBox->BxContentHeight)
	   switch (pAb->AbBox->BxVertEdge)
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
#endif /* IV */
     }
}


/*----------------------------------------------------------------------
   CanBeTranslated teste si un pave est modifiable en position     
   (X ou Y), et si oui, rend les positions extremes        
   de la boite.                                            
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static ThotBool     CanBeTranslated (PtrAbstractBox pAb, int frame, ThotBool horizRef, int *min, int *max)
#else  /* __STDC__ */
static ThotBool     CanBeTranslated (pAb, frame, horizRef, min, max)
PtrAbstractBox      pAb;
int                 frame;
ThotBool            horizRef;
int                *min;
int                *max;

#endif /* __STDC__ */
{
   PtrAbstractBox      pParentAb;
   PtrBox              pBox;
   PtrElement          pEl;
   PtrDocument         pDoc;
   ThotBool            ok, found;

   pBox = pAb->AbBox;
   pParentAb = pAb->AbEnclosing;
   pEl = pAb->AbElement;
   pDoc = DocumentOfElement (pEl);

   /* Deplacement nul si ok est faux */
   if (horizRef)
      *min = pBox->BxXOrg;
   else
      *min = pBox->BxYOrg;

   *max = *min;
   ok = TRUE;
   if (pDoc->DocReadOnly)
      ok = FALSE;
   else if (pEl->ElIsCopy)
      ok = FALSE;
   else if (ElementIsReadOnly (pEl))
      ok = FALSE;
   else if (pAb->AbPresentationBox)
      /* presentation box */
      ok = FALSE;
   else if (horizRef && pAb->AbWidth.DimIsPosition)
      /* stretchable box */
      ok = FALSE;
   else if (!horizRef && pAb->AbHeight.DimIsPosition)
      /* stretchable box */
      ok = FALSE;
   else if (horizRef && pAb->AbWidth.DimAbRef == pParentAb)
     /* box width linked with its parent */
      ok = FALSE;
   else if (!horizRef && pAb->AbHeight.DimAbRef == pParentAb)
     /* box height linked with its parent */
      ok = FALSE;
   else if (horizRef && pAb->AbHorizPos.PosAbRef == NULL)
      /* no position rule */
      ok = FALSE;
   else if (!horizRef && pAb->AbVertPos.PosAbRef == NULL)
      /* no position rule */
      ok = FALSE;
   else if (pParentAb->AbInLine || pParentAb->AbBox->BxType == BoGhost)
     /* box displayed in block of lines */
      ok = FALSE;
   else
     {
       /* search the first rule Move or NoMove */
       found = FALSE;
       while (!found && ok && pEl != NULL)
	 {
	   if (TypeHasException (ExcNoMove, pEl->ElTypeNumber, pEl->ElStructSchema))
	     ok = FALSE;
	   else if (horizRef && TypeHasException (ExcNoHMove, pEl->ElTypeNumber, pEl->ElStructSchema))
	     ok = FALSE;
	   else if (!horizRef && TypeHasException (ExcNoVMove, pEl->ElTypeNumber, pEl->ElStructSchema))
	     ok = FALSE;
	   else if (TypeHasException (ExcMoveResize, pEl->ElTypeNumber, pEl->ElStructSchema))
	       found = TRUE;
	   /* if no directive is done, see the parent */
	   pEl = pEl->ElParent;
	 }

       if (!found && ok && pParentAb != NULL)
	 {
	   /* it's not the root box */
	   if (horizRef
	       /* et le pParentAb ne depend pas de son contenu */
	       && pParentAb->AbBox->BxContentWidth
	       && !pParentAb->AbWidth.DimIsPosition
	       && !pParentAb->AbWidth.DimMinimum
	       && pAb->AbHorizPos.PosAbRef == pParentAb
	       && pAb->AbHorizPos.PosRefEdge != Left)
	     ok = FALSE;
	   else if (!horizRef
		    /* et le pParentAb ne depend pas de son contenu */
		    && pParentAb->AbBox->BxContentHeight
		    && !pParentAb->AbHeight.DimIsPosition
		    && !pParentAb->AbHeight.DimMinimum
		    && pAb->AbVertPos.PosAbRef == pParentAb
		    && pAb->AbVertPos.PosRefEdge != Top)
	     ok = FALSE;
	 }
     }

   if (horizRef)
      if (ok)
	{
	   GiveMovingArea (pAb, frame, horizRef, min, max);
	   /* La boite est-elle bloquee dans l'englobante ? */
	   if (*min == pBox->BxXOrg && *max == *min + pBox->BxWidth)
	      ok = FALSE;
	}
      else
	{
	   *min = pBox->BxXOrg;
	   *max = *min + pBox->BxWidth;
	}
   else if (ok)
     {
	GiveMovingArea (pAb, frame, horizRef, min, max);
	/* La boite est-elle bloquee dans l'englobante ? */
	if (*min == pBox->BxYOrg && *max == *min + pBox->BxHeight)
	   ok = FALSE;
     }
   else
     {
	*min = pBox->BxYOrg;
	*max = *min + pBox->BxHeight;
     }

   return ok;
}

/*----------------------------------------------------------------------
   APPgraphicModify envoie un message qui notifie qu'un trace' est 
   modifie'.                                               
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool            APPgraphicModify (PtrElement pEl, int value, int frame, ThotBool pre)

#else  /* __STDC__ */
ThotBool            APPgraphicModify (pEl, value, frame, pre)
PtrElement          pEl;
int                 value;
int                 frame;
ThotBool            pre;

#endif /* __STDC__ */

{
   PtrElement          pAsc;
   ThotBool            result;
   NotifyOnValue       notifyEl;
   PtrDocument         pDoc;
   int                 vue;
   ThotBool            assoc;
   ThotBool            ok = FALSE;

   GetDocAndView (frame, &pDoc, &vue, &assoc);
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
   /* if it's before the actual change is made and if the application accepts
      the change, register the operation in the Undo queue (only if it's
      not a creation). */
   if (pre && !ok)
     {
	if (((pEl->ElLeafType == LtGraphics || pEl->ElLeafType == LtSymbol) &&
	     pEl->ElGraph != '\0') ||
	    (pEl->ElLeafType == LtPolyLine && pEl->ElPolyLineType != '\0'))
	   {
	   if (ThotLocalActions[T_openhistory] != NULL)
	     (*ThotLocalActions[T_openhistory]) (pDoc, pEl, pEl, 0, 0);
	   if (ThotLocalActions[T_addhistory] != NULL)
	     (*ThotLocalActions[T_addhistory]) (pEl, pDoc, TRUE, TRUE);
	   if (ThotLocalActions[T_closehistory] != NULL)
	     (*ThotLocalActions[T_closehistory]) (pDoc);
	   }
     }
   return result;
}

/*----------------------------------------------------------------------
   ApplyDirectTranslate recherche la boite selectionnee pour un changement 
   de position. Si la plus petite boite englobant le point 
   xm,ym de la fenetre frame ne peut pas etre deplacee, la 
   procedure prend la boite englobante et ainsi de suite.  
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ApplyDirectTranslate (int frame, int xm, int ym)
#else  /* __STDC__ */
void                ApplyDirectTranslate (frame, xm, ym)
int                 frame;
int                 xm;
int                 ym;

#endif /* __STDC__ */
{
   PtrBox              pBox;
   PtrAbstractBox      pAb, draw;
   int                 x, width;
   int                 y, height;
   int                 xr, xmin;
   int                 yr, xmax;
   int                 ymin;
   int                 ymax;
   ViewFrame          *pFrame;
   int                 pointselect;
   PtrElement	       pEl;
   ThotBool            still, okH, okV;

   pFrame = &ViewFrameTable[frame - 1];
   /* pas de point selectionne */
   pointselect = 0;
   if (pFrame->FrAbstractBox != NULL)
     {
	/* On note les coordonnees par rapport a l'image concrete */
	xr = xm + pFrame->FrXOrg;
	yr = ym + pFrame->FrYOrg;

	/* On recherche la boite englobant le point designe */

	if (ThotLocalActions[T_selecbox] != NULL)
	   (*ThotLocalActions[T_selecbox]) (&pBox, pFrame->FrAbstractBox,
					    frame, xr, yr, &pointselect);
	if (pBox == NULL)
	   pAb = NULL;
	else
	   pAb = pBox->BxAbstractBox;

	if (pointselect != 0)
	   still = FALSE;
	else
	   /* ctrl click */
	   still = TRUE;

	/* On boucle tant que l'on ne trouve pas une boite deplacable */
	while (still)
	  {
	     if (pAb == NULL)
		pBox = NULL;
	     else
		pBox = pAb->AbBox;
	     if (pBox == NULL)
		still = FALSE;	/* Il n'y a pas de boite */
	     /* On regarde si le deplacement est autorise */
	     else
	       {
		  okH = CanBeTranslated (pAb, frame, TRUE, &xmin, &xmax);
		  okV = CanBeTranslated (pAb, frame, FALSE, &ymin, &ymax);
		  if (okH || okV)
		     still = FALSE;
	       }

	     /* Si on n'a pas trouve, il faut remonter */
	     if (still)
		/* On passe a la boite englobante */
		if (pAb != NULL)
		   pAb = pAb->AbEnclosing;
		else
		  {
		     pBox = NULL;
		     still = FALSE;
		  }
	  }

	/* Est-ce que l'on a trouve une boite ? */
	if (pBox != NULL)
	  {
	     x = pBox->BxXOrg - pFrame->FrXOrg;
	     y = pBox->BxYOrg - pFrame->FrYOrg;
	     width = pBox->BxWidth;
	     height = pBox->BxHeight;

	     if (pointselect != 0 && pBox->BxType != BoPicture)
	       {
		  pEl = pBox->BxAbstractBox->AbElement;
		  if (!APPgraphicModify (pEl, pointselect, frame, TRUE))
		    {
		       /* Deplacement d'un point de la polyline */
		       x = pBox->BxXOrg - pFrame->FrXOrg;
		       y = pBox->BxYOrg - pFrame->FrYOrg;
		       TtaDisplayMessage (INFO,
					  TtaGetMessage (LIB, TMSG_MOVING_BOX),
					  AbsBoxType (pBox->BxAbstractBox,
					  FALSE));
		       /* Note si le trace est ouvert ou ferme */
		       still = (pAb->AbPolyLineShape == 'p' ||
				pAb->AbPolyLineShape == 's');
		       /* Reaffiche la selection */
		       if (ThotLocalActions[T_switchsel])
			 (*ThotLocalActions[T_switchsel]) (frame, FALSE);
		       draw = GetParentDraw (pBox);
		       PolyLineModification (frame, &x, &y, pBox, draw,
					   pBox->BxNChars, pointselect, still);
		       /* on force le reaffichage de la boite */
		       DefClip (frame, pBox->BxXOrg - EXTRA_GRAPH,
				pBox->BxYOrg - EXTRA_GRAPH,
				pBox->BxXOrg + width + EXTRA_GRAPH,
				pBox->BxYOrg + height + EXTRA_GRAPH);
#ifdef IV
		       x += pFrame->FrXOrg;
		       y += pFrame->FrYOrg;
		       if (x != pBox->BxXOrg || y != pBox->BxYOrg)
			 NewPosition (pAb, x, y, frame, TRUE);
		       width = PointToPixel (pBox->BxBuffer->BuPoints[0].XCoord / 1000);
		       height = PointToPixel (pBox->BxBuffer->BuPoints[0].YCoord / 1000);
		       if (width != pBox->BxWidth || height != pBox->BxHeight)
			 NewDimension (pAb, width, height, frame, TRUE);
#endif
		       RedrawFrameBottom (frame, 0);
		       NewContent (pAb);
		       APPgraphicModify (pEl, pointselect, frame, FALSE);
		       /* Reaffiche la selection */
		       if (ThotLocalActions[T_switchsel])
			 (*ThotLocalActions[T_switchsel]) (frame, TRUE);
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
			      xr = x + width;
			      break;
			   case VertMiddle:
			      xr = x + width / 2;
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
			      yr = y + height;
			      break;
			   case HorizMiddle:
			      yr = y + height / 2;
			      break;
			   case HorizRef:
			      yr = y + pBox->BxHorizRef;
			      break;
			   default:
			      yr = y;
			      break;
			}

		  TtaDisplayMessage (INFO, TtaGetMessage (LIB, TMSG_MOVING_BOX), AbsBoxType (pBox->BxAbstractBox, FALSE));
		  /* On retablit les positions par rapport a la fenetre */
		  xmin -= pFrame->FrXOrg;
		  xmax -= pFrame->FrXOrg;
		  ymin -= pFrame->FrYOrg;
		  ymax -= pFrame->FrYOrg;
		  /* On initialise la boite fantome */
		  UserGeometryMove (frame, &x, &y, width, height, xr, yr, xmin, xmax, ymin, ymax, xm, ym);

		  /* On transmet la modification a l'editeur */
		  x = x + pFrame->FrXOrg;
		  y = y + pFrame->FrYOrg;
		  NewPosition (pBox->BxAbstractBox, x, y, frame, TRUE);
	       }
	  }
	else
	   /* On n'a pas trouve de boite modifiable */
	   TtaDisplaySimpleMessage (INFO, LIB, TMSG_MODIFYING_BOX_IMP);
     }
}


/*----------------------------------------------------------------------
   CanBeResized teste si un pave est modifiable en Dimension.       
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static ThotBool     CanBeResized (PtrAbstractBox pAb, int frame, ThotBool horizRef, int *min, int *max)
#else  /* __STDC__ */
static ThotBool     CanBeResized (pAb, frame, horizRef, min, max)
PtrAbstractBox      pAb;
int                 frame;
ThotBool            horizRef;
int                *min;
int                *max;

#endif /* __STDC__ */
{
   PtrBox              pBox;
   PtrAbstractBox      pParentAb;
   PtrElement          pEl;
   PtrDocument         pDoc;
   ThotBool            ok, found;

   pBox = pAb->AbBox;
   pParentAb = pAb->AbEnclosing;
   pEl = pAb->AbElement;
   pDoc = DocumentOfElement (pEl);

   if (horizRef)
      *min = pBox->BxXOrg;
   else
      *min = pBox->BxYOrg;

   *max = *min;
   ok = TRUE;
   if (pDoc->DocReadOnly)
      ok = FALSE;
   else if (pEl->ElIsCopy)
      ok = FALSE;
   else if (ElementIsReadOnly (pEl))
      ok = FALSE;
   else if (pAb->AbPresentationBox)
      /* presentation box */
      ok = FALSE;
   else if (horizRef && pAb->AbWidth.DimIsPosition)
      /* stretchable box */
      ok = FALSE;
   else if (!horizRef && pAb->AbHeight.DimIsPosition)
      /* stretchable box */
      ok = FALSE;
   else if (pAb->AbLeafType == LtText
	    && (pParentAb->AbInLine || pParentAb->AbBox->BxType == BoGhost))
     /* text box displayed in block of lines */
      ok = FALSE;
   /* Il est impossible de modifier la dimension du contenu */
   /* d'une boite construite ou de type texte                  */
   else if (pAb->AbLeafType == LtCompound || pAb->AbLeafType == LtText)
     {
       if (horizRef && pBox->BxContentWidth && !pAb->AbWidth.DimMinimum)
	 ok = FALSE;
       else if (!horizRef && pBox->BxContentHeight && !pAb->AbHeight.DimMinimum)
	 ok = FALSE;
     }
   else if (horizRef && pAb->AbWidth.DimAbRef == pParentAb)
     /* box width linked with its parent */
      ok = FALSE;
   else if (!horizRef && pAb->AbHeight.DimAbRef == pParentAb)
     /* box height linked with its parent */
      ok = FALSE;
   else
     {
       /* search the first rule Move or NoMove */
       found = FALSE;
       while (!found && ok && pEl != NULL)
	 {
	   if (TypeHasException (ExcNoResize, pEl->ElTypeNumber, pEl->ElStructSchema))
	     ok = FALSE;
	   else if (horizRef && TypeHasException (ExcNoHResize, pEl->ElTypeNumber, pEl->ElStructSchema))
	     ok = FALSE;
	   else if (!horizRef && TypeHasException (ExcNoVResize, pEl->ElTypeNumber, pEl->ElStructSchema))
	     ok = FALSE;
	   else if (TypeHasException (ExcMoveResize, pEl->ElTypeNumber, pEl->ElStructSchema))
	     found = TRUE;
	   /* if no directive is done, see the parent */
	   pEl = pEl->ElParent;
	 }

       if (!found && ok && pParentAb != NULL)
	 {
	   /* it's not the root box */
	   if ((horizRef && pAb->AbWidth.DimValue == 0)
	       || (!horizRef && pAb->AbHeight.DimValue == 0))
	     ok = FALSE;
	 }
     }

   if (horizRef)
      if (ok)
	 GiveMovingArea (pAb, frame, horizRef, min, max);
      else
	{
	   *min = pBox->BxXOrg;
	   *max = *min + pBox->BxWidth;
	}
   else if (ok)
      GiveMovingArea (pAb, frame, horizRef, min, max);
   else
     {
	*min = pBox->BxYOrg;
	*max = *min + pBox->BxHeight;
     }

   return ok;
}


/*----------------------------------------------------------------------
   ApplyDirectResize recherche la boite candidate pour un changement 
   de dimension. Si la plus petite boite englobant le point  
   x,y de la fenetre frame ne peut pas etre redimensionnee,  
   la procedure prend la boite englobante et ainsi de        
   suite.                                                    
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ApplyDirectResize (int frame, int xm, int ym)
#else  /* __STDC__ */
void                ApplyDirectResize (frame, xm, ym)
int                 frame;
int                 xm;
int                 ym;
#endif /* __STDC__ */
{
   PtrBox              pBox;
   PtrAbstractBox      pAb;
   ViewFrame          *pFrame;
   int                 x, width;
   int                 y, height;
   int                 xr, xmin;
   int                 yr, xmax;
   int                 ymin;
   int                 ymax;
   int                 percentW, percentH;
   int                 pointselect;
   ThotBool            still, okH, okV;

   okH = FALSE;
   okV = FALSE;
   pFrame = &ViewFrameTable[frame - 1];
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
	still = TRUE;
	/* On boucle tant que l'on ne trouve pas une boite modifiable */
	while (still)
	  {
	     if (pAb == NULL)
		pBox = NULL;
	     else
		pBox = pAb->AbBox;

	     if (pBox == NULL)
		still = FALSE;	/* Il n'y a pas de boite */
	     /* On regarde si les modifications sont autorisees */
	     else
	       {
		  okH = CanBeResized (pAb, frame, TRUE, &xmin, &xmax);
		  okV = CanBeResized (pAb, frame, FALSE, &ymin, &ymax);
		  if (okH || okV)
		     still = FALSE;
	       }

	     /* Si on n'a pas trouve, il faut remonter */
	     if (still)
		/* On passe a la boite englobante */
		if (pAb != NULL)
		   pAb = pAb->AbEnclosing;
		else
		  {
		     pBox = NULL;
		     still = FALSE;
		  }
	  }

	/* Est-ce que l'on a trouve une boite ? */
	if (pBox != NULL)
	  {
	     x = pBox->BxXOrg - pFrame->FrXOrg;
	     y = pBox->BxYOrg - pFrame->FrYOrg;
	     width = pBox->BxWidth;
	     height = pBox->BxHeight;

	     /* On note les coordonnees du point de reference */
	     switch (pBox->BxHorizEdge)
		   {
		      case Left:
			 xr = x;
			 break;
		      case Right:
			 xr = x + width;
			 break;
		      case VertMiddle:
			 xr = x + width / 2;
			 break;
		      case VertRef:
			 xr = x + pBox->BxVertRef;
			 break;
		      default:
			 xr = x + width;
			 break;
		   }

	     switch (pBox->BxVertEdge)
		   {
		      case Top:
			 yr = y;
			 break;
		      case Bottom:
			 yr = y + height;
			 break;
		      case HorizMiddle:
			 yr = y + height / 2;
			 break;
		      case HorizRef:
			 yr = y + pBox->BxHorizRef;
			 break;
		      default:
			 yr = y + height;
			 break;
		   }

	     TtaDisplayMessage (INFO, TtaGetMessage (LIB, TMSG_MODIFYING_BOX), AbsBoxType (pBox->BxAbstractBox, FALSE));
	     /* On retablit les positions par rapport a la fenetre */
	     xmin -= pFrame->FrXOrg;
	     if (okH)
		xmax -= pFrame->FrXOrg;
	     else
		xmax = xmin;
	     ymin -= pFrame->FrYOrg;
	     if (okV)
		ymax -= pFrame->FrYOrg;
	     else
		ymax = ymin;
	     /* On initialise la boite fantome */
	     percentW = 0;
	     percentH = 0;
	     pAb = pBox->BxAbstractBox;
	     if (!pAb->AbWidth.DimUserSpecified &&
		 !pAb->AbWidth.DimIsPosition &&
		 !pAb->AbWidth.DimSameDimension &&
		 pAb->AbWidth.DimAbRef == pAb)
	       {
		 if (pAb->AbWidth.DimUnit == UnPercent)
		   percentW = pAb->AbWidth.DimValue;
		 else if (pAb->AbWidth.DimValue == 0)
		   percentW = 100;
	       }
	     else if (!pAb->AbHeight.DimUserSpecified &&
		      !pAb->AbHeight.DimIsPosition &&
		      !pAb->AbHeight.DimSameDimension &&
		      pAb->AbHeight.DimAbRef == pAb)
	       {
		 if (pAb->AbHeight.DimUnit == UnPercent)
		   percentH = pAb->AbHeight.DimValue;
		 else if (pAb->AbHeight.DimValue == 0)
		   percentH = 100;
	       }
	     UserGeometryResize (frame, x, y, &width, &height, xr, yr,
				 xmin, xmax, ymin, ymax, xm, ym,
				 percentW, percentH);
	     /* On transmet la modification a l'editeur */
	     if (percentW)
	       NewDimension (pAb, 0, height, frame, TRUE);
	     else if (percentH)
	       NewDimension (pAb, width, 0, frame, TRUE);
	     else
	       NewDimension (pAb, width, height, frame, TRUE);
	  }
	else
	   /* On n'a pas trouve de boite modifiable */
	   TtaDisplaySimpleMessage (INFO, LIB, TMSG_MODIFYING_BOX_IMP);
     }
}

/*----------------------------------------------------------------------
   DirectCreation re'alise les differents modes de cre'ation       
   interactive des boi^tes.                                
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                DirectCreation (PtrBox pBox, int frame)
#else  /* __STDC__ */
void                DirectCreation (pBox, frame)
PtrBox              pBox;
int                 frame;

#endif /* __STDC__ */
{
   ViewFrame          *pFrame;
   PtrAbstractBox      pAb;
   int                 x, y;
   int                 width, height;
   int                 xr, yr;
   int                 xmin, xmax;
   int                 Ymin, Ymax;
   int                 percentW, percentH;
   ThotBool            modPosition, modDimension;

   /* Il ne faut realiser qu'une seule creation interactive a la fois */
   if (BoxCreating)
      return;
   else
      BoxCreating = TRUE;

   pFrame = &ViewFrameTable[frame - 1];

   /* Il faut verifier que la boite reste visible dans la fenetre */
   GetSizesFrame (frame, &width, &height);
   if (pBox->BxXOrg < pFrame->FrXOrg)
      x = 0;
   else if (pBox->BxXOrg > pFrame->FrXOrg + width)
      x = width;
   else
      x = pBox->BxXOrg - pFrame->FrXOrg;

   if (pBox->BxYOrg < pFrame->FrYOrg)
      y = 0;
   else if (pBox->BxYOrg > pFrame->FrYOrg + height)
      y = height;
   else
      y = pBox->BxYOrg - pFrame->FrYOrg;
   width = pBox->BxWidth;
   height = pBox->BxHeight;
   pAb = pBox->BxAbstractBox;

   /* On note les coordonnees du point de reference */
   switch (pBox->BxHorizEdge)
	 {
	    case Left:
	       xr = x;
	       break;
	    case Right:
	       xr = x + width;
	       break;
	    case VertMiddle:
	       xr = x + width / 2;
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
	       yr = y + height;
	       break;
	    case HorizMiddle:
	       yr = y + height / 2;
	       break;
	    case HorizRef:
	       yr = y + pBox->BxHorizRef;
	       break;
	    default:
	       yr = y;
	       break;
	 }

   modPosition = (CanBeTranslated (pAb, frame, TRUE, &xmin, &xmax)
		  || CanBeTranslated (pAb, frame, FALSE, &Ymin, &Ymax));
   if (!modPosition)
     {
	pAb->AbHorizPos.PosUserSpecified = FALSE;
	pAb->AbVertPos.PosUserSpecified = FALSE;
     }
   modDimension = (CanBeResized (pAb, frame, TRUE, &xmin, &xmax)
		   || CanBeResized (pAb, frame, FALSE, &Ymin, &Ymax));
   if (!modDimension)
     {
	pAb->AbWidth.DimUserSpecified = FALSE;
	pAb->AbHeight.DimUserSpecified = FALSE;
     }
   if (modPosition || modDimension)
     {
	/* Determine les limites de deplacement de la boite */
	GiveMovingArea (pAb, frame, TRUE, &xmin, &xmax);
	GiveMovingArea (pAb, frame, FALSE, &Ymin, &Ymax);
	TtaDisplayMessage (INFO, TtaGetMessage (LIB, TMSG_CREATING_BOX), AbsBoxType (pAb, FALSE));
	/* On retablit les positions par rapport a la fenetre */
	xmin -= pFrame->FrXOrg;
	xmax -= pFrame->FrXOrg;
	Ymin -= pFrame->FrYOrg;
	Ymax -= pFrame->FrYOrg;
	/*
	  calcule les rapports largeur sur hauteur et hauteur sur
	  largeur si une des deux dimensions depend de l'autre
	  percentW = 0 si la largeur ne depend pas de la hauteur
	  percentH = 0 si la hauteur ne depend pas de la largeur
	  */
	percentW = 0;
	percentH = 0;
	if (!pAb->AbWidth.DimUserSpecified &&
	    !pAb->AbWidth.DimIsPosition &&
	    !pAb->AbWidth.DimSameDimension &&
	    pAb->AbWidth.DimAbRef == pAb)
	  {
	    if (pAb->AbWidth.DimUnit == UnPercent)
	      percentW = pAb->AbWidth.DimValue;
	    else if (pAb->AbWidth.DimValue == 0)
	      percentW = 100;
	  }
	else if (!pAb->AbHeight.DimUserSpecified &&
	    !pAb->AbHeight.DimIsPosition &&
	    !pAb->AbHeight.DimSameDimension &&
	    pAb->AbHeight.DimAbRef == pAb)
	  {
	    if (pAb->AbHeight.DimUnit == UnPercent)
	      percentH = pAb->AbHeight.DimValue;
	    else if (pAb->AbHeight.DimValue == 0)
	      percentH = 100;
	  }
	UserGeometryCreate (frame, &x, &y, xr, yr, &width, &height,
			    xmin, xmax, Ymin, Ymax,
			    pAb->AbHorizPos.PosUserSpecified,
			    pAb->AbVertPos.PosUserSpecified,
			    pAb->AbWidth.DimUserSpecified,
			    pAb->AbHeight.DimUserSpecified,
			    percentW, percentH);

	/* Notification de la boite saisie */
	pAb->AbHorizPos.PosUserSpecified = FALSE;
	pAb->AbVertPos.PosUserSpecified = FALSE;
	pAb->AbWidth.DimUserSpecified = FALSE;
	pAb->AbHeight.DimUserSpecified = FALSE;
	x += pFrame->FrXOrg;
	y += pFrame->FrYOrg;
	NewPosition (pAb, x, y, frame, TRUE);
	if (percentW)
	  NewDimension (pAb, 0, height, frame, TRUE);
	else if (percentH)
	  NewDimension (pAb, width, 0, frame, TRUE);
	else
	  NewDimension (pAb, width, height, frame, TRUE);
     }

   /* Traitement de la creation interactive termine */
   BoxCreating = FALSE;
}

/*----------------------------------------------------------------------
   LocateClickedChar cherche le caractere affiche dans la boite pBox 
   avec un decalage x. Rend le pointeur sur le buffer qui  
   contient le caractere designe', l'index du caractere    
   dans le buffer, l'index du caractere dans la boite et   
   le nombre de blancs qui le precedent dans la boite.     
   Met a jour la valeur x.                                 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                LocateClickedChar (PtrBox pBox, PtrTextBuffer * pBuffer, int *x, int *index, int *charsNumber, int *spacesNumber)
#else  /* __STDC__ */
void                LocateClickedChar (pBox, pBuffer, x, index, charsNumber, spacesNumber)
PtrBox              pBox;
PtrTextBuffer      *pBuffer;
int                *x;
int                *index;
int                *charsNumber;
int                *spacesNumber;

#endif /* __STDC__ */
{
   int                 dx;
   int                 length;
   int                 extraSpace;
   int                 spaceWidth;
   int                 charWidth;
   int                 newIndex;
   ptrfont             font;
   UCHAR_T       c;
   ThotBool            notfound;


   /* Nombre de caracteres qui precedent */
   *charsNumber = 0;
   *spacesNumber = 0;
   c = EOS;
   charWidth = 0;
   if (pBox->BxNChars == 0 || *x <= 0)
     {
	*x = 0;
	*pBuffer = pBox->BxBuffer;
	*index = pBox->BxFirstChar;
     }
   else
     {
	font = pBox->BxFont;
	dx = 0;
	newIndex = pBox->BxFirstChar;
	*index = newIndex;
	*pBuffer = pBox->BxBuffer;
	length = pBox->BxNChars;
	/* Calcule la largeur des blancs */
	if (pBox->BxSpaceWidth == 0)
	  {
	     spaceWidth = CharacterWidth (SPACE, font);
	     extraSpace = 0;
	  }
	else
	  {
	     spaceWidth = pBox->BxSpaceWidth;
	     extraSpace = pBox->BxNPixels;
	  }

	/* Recherche le caractere designe dans la boite */
#ifdef STRUCT_EDIT
	notfound = (dx < *x);
#else
	/* largeur du caractere suivant */
	c = (UCHAR_T) ((*pBuffer)->BuContent[newIndex - 1]);
	if (c == 0)
	   charWidth = 0;
	else if (c == SPACE)
	   charWidth = spaceWidth;
	else
	   charWidth = CharacterWidth (c, font);
	notfound = (dx + charWidth / 2 < *x);
#endif
	while (notfound && length > 0)
	  {
#ifdef STRUCT_EDIT
	     /* largeur du caractere courant */
	     c = (UCHAR_T) ((*pBuffer)->BuContent[newIndex - 1]);
	     if (c == 0)
		charWidth = 0;
	     else if (c == SPACE)
		charWidth = spaceWidth;
	     else
		charWidth = CharacterWidth (c, font);
#endif

	     if (c == SPACE)
	       {
		  (*spacesNumber)++;
		  if (extraSpace > 0)
		    {
		       dx++;
		       extraSpace--;
		    }
	       }

	     dx += charWidth;
	     (*charsNumber)++;

	     /* On passe au caractere suivant */
	     *index = newIndex;
	     if (newIndex < (*pBuffer)->BuLength || length == 1)
		newIndex++;
	     else if ((*pBuffer)->BuNext == NULL)
	       {
		  length = 0;
		  newIndex++;
	       }
	     else
	       {
		  *pBuffer = (*pBuffer)->BuNext;
		  newIndex = 1;
	       }
	     length--;
#ifdef STRUCT_EDIT
	     notfound = (dx < *x);
#else
	     /* largeur du caractere suivant */
	     c = (UCHAR_T) ((*pBuffer)->BuContent[newIndex - 1]);
	     if (c == 0)
		charWidth = 0;
	     else if (c == SPACE)
		charWidth = spaceWidth;
	     else
		charWidth = CharacterWidth (c, font);
	     notfound = (dx + charWidth / 2 < *x);
#endif
	  }

	/* On a trouve le caractere : Recadre la position x */
	if (dx == *x)
	   /* BAlignment OK */
	   *index = newIndex;
	else if (dx > *x)
	  {
#ifdef STRUCT_EDIT
	     /* BAlignment sur le caractere */
	     *x = dx - charWidth;
	     if (c == SPACE)
	       {
		  if (*spacesNumber > 0 && pBox->BxNPixels >= *spacesNumber)
		     (*x)--;
		  (*spacesNumber)--;
	       }

	     if (newIndex == 1)
		*pBuffer = (*pBuffer)->BuPrevious;
	     (*charsNumber)--;
#else
	     *x = dx;
	     *index = newIndex;
#endif
	  }
	else
	   /* BAlignment en fin de boite */
	  {
	     *x = dx;
	     if (newIndex == 1 && (*pBuffer)->BuPrevious != NULL)
		*pBuffer = (*pBuffer)->BuPrevious;
	     (*index)++;
	  }
     }
}
