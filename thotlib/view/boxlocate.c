/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2001
 *  Please first read the full copyright statement in file COPYRIGHT.
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
#include "appaction.h"
#include "appdialogue.h"

#define THOT_EXPORT extern
#include "edit_tv.h"
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
#include "hyphen_f.h"
#include "memory_f.h"
#include "presentationapi_f.h"
#include "structcreation_f.h"
#include "structselect_f.h"
#include "textcommands_f.h"
#include "tree_f.h"
#include "undo_f.h"
#include "units_f.h"
#include "views_f.h"
#include "word_f.h"

#define Y_RATIO 200		/* penalisation en Y */
#define ANCHOR_SIZE 3		/* taille des ancres */
#define	MAX_STACK	50
#define	MIDDLE_OF(v1, v2) (((v1)+(v2))/2.0)
#define SEG_SPLINE      5
#define ALLOC_POINTS    300

typedef struct stack_point
  {
     float               x1, y1, x2, y2, x3, y3, x4, y4;
  }
StackPoint;
static StackPoint   stack[MAX_STACK];
static int          stack_deep;

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
void LocateSelectionInView (int frame, int x, int y, int button)
{
  PtrBox              pBox;
  PtrTextBuffer       pBuffer;
  PtrAbstractBox      pAb;
  NotifyElement       notifyEl;
  PtrElement          el;
  ViewFrame          *pFrame;
  int                 charsNumber;
  int                 spacesNumber;
  int                 index, pos;
  int                 xOrg, yOrg;
  ThotBool            extend;

  if (frame >= 1)
    {
      /* check if a leaf box is selected */
      pFrame = &ViewFrameTable[frame - 1];
      x += pFrame->FrXOrg;
      y += pFrame->FrYOrg;
      pAb = pFrame->FrAbstractBox;
      charsNumber = 0;
      extend = (button == 0 || button == 1);
      /* get the selected box */
      if (ThotLocalActions[T_selecbox] != NULL)
	(*ThotLocalActions[T_selecbox]) (&pBox, pAb, frame, x, y,&charsNumber);
      /* When it's an extended selection, avoid to extend to the
	 enclosing box */
      if (extend)
	{
	  if (pBox != pFrame->FrSelectionBegin.VsBox &&
	      IsParentBox (pBox, pFrame->FrSelectionBegin.VsBox))
	    pBox = GetClickedLeafBox (frame, x, y);
	}
      if (pBox != NULL)
	{
	  pAb = pBox->BxAbstractBox;
	  if (pAb->AbLeafType == LtText &&
	      (!pAb->AbPresentationBox || pAb->AbCanBeModified))
	    {
	      pos = x - pBox->BxXOrg - pBox->BxLMargin - pBox->BxLBorder
		    - pBox->BxLPadding;
	      LocateClickedChar (pBox, extend, &pBuffer, &pos, &index,
				 &charsNumber, &spacesNumber);
	      charsNumber = pBox->BxIndChar + charsNumber + 1;
	    }
	}
      else
	pAb = NULL;

      CloseInsertion ();
      if (pAb != NULL)
	{
	  /* Initialization of the selection */
	  if (button == 3)
	    {
	      if (!ChangeSelection (frame, pAb, charsNumber, FALSE, TRUE,
				    TRUE, FALSE) &&
		  pAb->AbLeafType == LtText &&
		  (!pAb->AbPresentationBox || pAb->AbCanBeModified))
		SelectCurrentWord (frame, pBox, charsNumber, index, pBuffer,
				   TRUE);
	    }
	  else if (button == 2)
	    ChangeSelection (frame, pAb, charsNumber, FALSE, TRUE, FALSE,
			     FALSE);
	  /* Extension of selection */
	  else if (button == 0)
	    ChangeSelection (frame, pAb, charsNumber, TRUE, TRUE, FALSE,FALSE);
	  else if (button == 1)
	    ChangeSelection (frame, pAb, charsNumber, TRUE, TRUE, FALSE, TRUE);
	  else /* button == 4 */
	    {
	      /* check if the curseur is within the box */
	      xOrg =  pBox->BxXOrg + pBox->BxLMargin + pBox->BxLBorder +
		pBox->BxLPadding;
	      yOrg =  pBox->BxYOrg + pBox->BxTMargin + pBox->BxTBorder +
		pBox->BxTPadding;
	      if (x >= xOrg && x <= xOrg + pBox->BxW &&
		  y >= yOrg && y <= yOrg + pBox->BxH)
		{
		  /* send event TteElemActivate.Pre to the application */
		  el = pAb->AbElement;
		  notifyEl.event = TteElemClick;
		  notifyEl.document = FrameTable[frame].FrDoc;
		  notifyEl.element = (Element) el;
		  notifyEl.elementType.ElTypeNum = el->ElTypeNumber;
		  notifyEl.elementType.ElSSchema = (SSchema)(el->ElStructSchema);
		  notifyEl.position = 0;
		  if (CallEventType ((NotifyEvent *) & notifyEl, TRUE))
		    /* the application asks Thot to do nothing */
		    return;
		  /* send event TteElemActivate.Post to the application */
		  CallEventType ((NotifyEvent *) & notifyEl, FALSE);
		}
	    }
	}
    }
}

/*----------------------------------------------------------------------
  GetDistance returns 0 if value is between -delta and +delta.
  In other cases returns the absolute value of value - delta
  ----------------------------------------------------------------------*/
static int          GetDistance (int value, int delta)
{
   if (value > delta)
      return (value - delta);
   else if (value < -delta)
      return (-value - delta);
   else
      return (0);
}

/*----------------------------------------------------------------------
  GetBoxDistance computes the distance of a point xRef, yRef to a box
  We apply a ratio to vertical distances to give a preference to the
  horizontal proximity.
  ----------------------------------------------------------------------*/
int    GetBoxDistance (int xRef, int yRef, int x, int y, int width, int height)
{
   int                 value;

   /* prend le centre de la boite */
   width /= 2;
   x += width;
   height /= 2;
   y += height;
   value = GetDistance (xRef - x, width) +
           Y_RATIO * GetDistance (yRef - y, height);
   return (value);
}

/*----------------------------------------------------------------------
  PolyNewPoint : add a new point to the current polyline.
  ----------------------------------------------------------------------*/
ThotBool    PolyNewPoint (int x, int y, ThotPoint **points, int *npoints,
			  int *maxpoints)
{
   ThotPoint          *tmp;
   int                 size;

   if (*npoints >= *maxpoints)
     {
	size = *maxpoints + ALLOC_POINTS;
	if ((tmp = (ThotPoint*)realloc(*points, size * sizeof(ThotPoint))) ==0)
	   return (FALSE);
	else
	  {
	     /* la reallocation a reussi */
	     *points = tmp;
	     *maxpoints = size;
	  }
     }
   /* ignore identical points */
   if (*npoints > 0 &&
       (*points)[*npoints - 1].x == x && (*points)[*npoints - 1].y == y)
      return (FALSE);

   (*points)[*npoints].x = x;
   (*points)[*npoints].y = y;
   (*npoints)++;
   return (TRUE);
}

/*----------------------------------------------------------------------
  PushStack : push a spline on the stack.
  ----------------------------------------------------------------------*/
static void  PushStack (float x1, float y1, float x2, float y2, float x3,
			float y3, float x4, float y4)
{
   StackPoint         *stack_ptr;

   if (stack_deep == MAX_STACK)
      return;

   stack_ptr = &stack[stack_deep];
   stack_ptr->x1 = x1;
   stack_ptr->y1 = y1;
   stack_ptr->x2 = x2;
   stack_ptr->y2 = y2;
   stack_ptr->x3 = x3;
   stack_ptr->y3 = y3;
   stack_ptr->x4 = x4;
   stack_ptr->y4 = y4;
   stack_deep++;
}

/*----------------------------------------------------------------------
  PopStack : pop a spline from the stack.
  ----------------------------------------------------------------------*/
static ThotBool PopStack (float *x1, float *y1, float *x2, float *y2,
			  float *x3, float *y3, float *x4, float *y4)
{
   StackPoint         *stack_ptr;

   if (stack_deep == 0)
      return (FALSE);

   stack_deep--;
   stack_ptr = &stack[stack_deep];
   *x1 = stack_ptr->x1;
   *y1 = stack_ptr->y1;
   *x2 = stack_ptr->x2;
   *y2 = stack_ptr->y2;
   *x3 = stack_ptr->x3;
   *y3 = stack_ptr->y3;
   *x4 = stack_ptr->x4;
   *y4 = stack_ptr->y4;
   return (TRUE);
}

/*----------------------------------------------------------------------
  PolySplit : split a polyline and push the results on the stack.
  ----------------------------------------------------------------------*/
void        PolySplit (float a1, float b1, float a2, float b2,
		       float a3, float b3, float a4, float b4,
		       ThotPoint **points, int *npoints, int *maxpoints)
{
   register float      tx, ty;
   float               x1, y1, x2, y2, x3, y3, x4, y4;
   float               sx1, sy1, sx2, sy2;
   float               tx1, ty1, tx2, ty2, xmid, ymid;

   stack_deep = 0;
   PushStack (a1, b1, a2, b2, a3, b3, a4, b4);

   while (PopStack (&x1, &y1, &x2, &y2, &x3, &y3, &x4, &y4))
     {
	if (fabs (x1 - x4) < SEG_SPLINE && fabs (y1 - y4) < SEG_SPLINE)
	   PolyNewPoint (FloatToInt (x1), FloatToInt (y1), points, npoints,
			 maxpoints);
	else
	  {
	     tx   = (float) MIDDLE_OF (x2, x3);
	     ty   = (float) MIDDLE_OF (y2, y3);
	     sx1  = (float) MIDDLE_OF (x1, x2);
	     sy1  = (float) MIDDLE_OF (y1, y2);
	     sx2  = (float) MIDDLE_OF (sx1, tx);
	     sy2  = (float) MIDDLE_OF (sy1, ty);
	     tx2  = (float) MIDDLE_OF (x3, x4);
	     ty2  = (float) MIDDLE_OF (y3, y4);
	     tx1  = (float) MIDDLE_OF (tx2, tx);
	     ty1  = (float) MIDDLE_OF (ty2, ty);
	     xmid = (float) MIDDLE_OF (sx2, tx1);
	     ymid = (float) MIDDLE_OF (sy2, ty1);

	     PushStack (xmid, ymid, tx1, ty1, tx2, ty2, x4, y4);
	     PushStack (x1, y1, sx1, sy1, sx2, sy2, xmid, ymid);
	  }
     }
}

/*----------------------------------------------------------------------
  QuadraticSplit : split a quadratic Bezier and pushes the result on the stack.
  ----------------------------------------------------------------------*/
void        QuadraticSplit (float a1, float b1, float a2, float b2,
			    float a3, float b3,
			    ThotPoint **points, int *npoints,
			    int *maxpoints)
{
   register float      tx, ty;
   float               x1, y1, x2, y2, x3, y3, i, j;
   float               sx, sy;
   float               xmid, ymid;

   stack_deep = 0;
   PushStack (a1, b1, a2, b2, a3, b3, 0, 0);

   while (PopStack (&x1, &y1, &x2, &y2, &x3, &y3, &i, &j))
     {
	if (fabs (x1 - x3) < SEG_SPLINE && fabs (y1 - y3) < SEG_SPLINE)
	   PolyNewPoint (FloatToInt (x1), FloatToInt (y1), points, npoints,
			 maxpoints);
	else
	  {
	     tx   = (float) MIDDLE_OF (x2, x3);
	     ty   = (float) MIDDLE_OF (y2, y3);
	     sx   = (float) MIDDLE_OF (x1, x2);
	     sy   = (float) MIDDLE_OF (y1, y2);
	     xmid = (float) MIDDLE_OF (sx, tx);
	     ymid = (float) MIDDLE_OF (sy, ty);

	     PushStack (xmid, ymid, tx, ty, x3, y3, 0, 0);
	     PushStack (x1, y1, sx, sy, xmid, ymid, 0, 0);
	  }
     }
}

/*----------------------------------------------------------------------
  IsOnSegment checks if the point x, y is on the segment x1, y1 to
  x2, y2 with DELTA_SEL precision.
  Check if the segment is included by a rectangle of width DELTA_SEL
  around the line.
  Checking is performed after a rotation that provides an horizontal
  rectangle.
  ----------------------------------------------------------------------*/
static ThotBool     IsOnSegment (int x, int y, int x1, int y1, int x2, int y2)
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
  CrossLine returns the next cross value
  ----------------------------------------------------------------------*/
static int     CrossLine (int x, int y, int prevX, int prevY, int nextX, int nextY, int cross)
{
  int          i;
  ThotBool     ok;

  /* y between nextY and prevY */
  i = cross;
  if ((ok = (prevX >= x)) == (nextX >= x))
    {
      /* x on the same side of both extremities */
      if (ok)
	/* nextX et prevX >= x */
	i = i + 1;
    }
  else if ((prevX - (prevY - y) * (nextX - prevX) / (nextY - prevY)) >= x)
    /* x between the extremities */
    i = i + 1;
  return i;
}

/*----------------------------------------------------------------------
  IsWithinPolyline returns TRUE if the point x, y is within the polyline pAb
  ----------------------------------------------------------------------*/
static ThotBool  IsWithinPolyline (PtrAbstractBox pAb, int x, int y, int frame)
{
   PtrTextBuffer       buff, pLastBuffer;
   int                 cross;
   int                 i, max;
   int                 prevX, prevY;
   int                 nextX, nextY;
   PtrBox              box;
   ThotBool            ok;

   box = pAb->AbBox;
   x -= box->BxXOrg;
   y -= box->BxYOrg;
   max = box->BxNChars;
   if (max < 4)
      /* no space within polyline */
      return (FALSE);

   /* first and last points of the polyline */
   pLastBuffer = buff = box->BxBuffer;
   i = 1;
   while (pLastBuffer->BuNext != NULL)
      pLastBuffer = pLastBuffer->BuNext;
   max = pLastBuffer->BuLength - 1;

   cross = 0;
   nextX = PixelValue (buff->BuPoints[i].XCoord,
		       UnPixel, NULL,
		       ViewFrameTable[frame - 1].FrMagnification);
   nextY = PixelValue (buff->BuPoints[i].YCoord,
		       UnPixel, NULL,
		       ViewFrameTable[frame - 1].FrMagnification);
   prevX = PixelValue (pLastBuffer->BuPoints[max].XCoord,
		       UnPixel, NULL,
		       ViewFrameTable[frame - 1].FrMagnification);
   prevY = PixelValue (pLastBuffer->BuPoints[max].YCoord,
		       UnPixel, NULL,
		       ViewFrameTable[frame - 1].FrMagnification);
   if ((prevY >= y) != (nextY >= y))
     /* y between nextY and prevY */
     cross = CrossLine (x, y, prevX, prevY, nextX, nextY, cross);

   i++;
   while (i <= max || buff != pLastBuffer)
     {
       prevX = nextX;
       prevY = nextY;
       nextX = PixelValue (buff->BuPoints[i].XCoord,
			   UnPixel, NULL,
			   ViewFrameTable[frame - 1].FrMagnification);
       nextY = PixelValue (buff->BuPoints[i].YCoord,
			   UnPixel, NULL,
			   ViewFrameTable[frame - 1].FrMagnification);
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
	       nextX = PixelValue (buff->BuPoints[i].XCoord,
				   UnPixel, NULL,
				   ViewFrameTable[frame - 1].FrMagnification);
	       nextY = PixelValue (buff->BuPoints[i].YCoord,
				   UnPixel, NULL,
				   ViewFrameTable[frame - 1].FrMagnification);
	     }
	   
	   if (i > max && buff == pLastBuffer)
	     break;
	   cross = CrossLine (x, y, prevX, prevY, nextX, nextY, cross);
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
	       nextX = PixelValue (buff->BuPoints[i].XCoord,
				   UnPixel, NULL,
				   ViewFrameTable[frame - 1].FrMagnification);
	       nextY = PixelValue (buff->BuPoints[i].YCoord,
				   UnPixel, NULL,
				   ViewFrameTable[frame - 1].FrMagnification);
	     }
	   
	   if (i > max && buff == pLastBuffer)
	     break;
	   cross = CrossLine (x, y, prevX, prevY, nextX, nextY, cross);
	 }
     }
   ok = (ThotBool) (cross & 0x01);
   return (ok);
}

/*----------------------------------------------------------------------
  IsWithinPath returns TRUE if the point x, y is within the path represented
  by the polyline defined by points.
  ----------------------------------------------------------------------*/
static ThotBool     IsWithinPath (int x, int y, ThotPoint *points, int npoints)
{
   int                 cross;
   int                 i;
   int                 prevX, prevY;
   int                 nextX, nextY;
   ThotBool            ok;

   ok = FALSE;
   cross = 0;
   nextX = points[0].x;
   nextY = points[0].y;
   prevX = points[npoints - 1].x;
   prevY = points[npoints - 1].y;
   if ((prevY >= y) != (nextY >= y))
     /* y is between nextY and prevY */
     cross = CrossLine (x, y, prevX, prevY, nextX, nextY, cross);
   i = 1;
   while (i < npoints)
     {
       prevX = nextX;
       prevY = nextY;
       nextX = points[i].x;
       nextY = points[i].y;
       if (prevY >= y)
	 {
	   while (i < npoints && nextY >= y)
	     {
	       i++;		/* changement de point */
	       prevY = nextY;
	       prevX = nextX;
	       nextX = points[i].x;
	       nextY = points[i].y;
	     }
	   if (i >= npoints)
	     break;
	   cross = CrossLine (x, y, prevX, prevY, nextX, nextY, cross);
	 }
       else
	 {
	   while (i < npoints && nextY < y)
	     {
	       i++;		/* changement de point */
	       prevY = nextY;
	       prevX = nextX;
	       nextX = points[i].x;
	       nextY = points[i].y;
	     }
	   if (i >= npoints)
	     break;
	   cross = CrossLine (x, y, prevX, prevY, nextX, nextY, cross);
	 }
     }
   ok = (ThotBool) (cross & 0x01);
   return (ok);
}

/*----------------------------------------------------------------------
  GetPolylinePoint teste qu'un point x,y est sur un segment de la    
   boi^te polyline.                                        
   Si oui, retourne l'adresse de la boi^te correspondante et le    
   point de contro^le se'lectionne' (0 pour toute la boi^te).      
   sinon, la valeur NULL.                                          
  ----------------------------------------------------------------------*/
static PtrBox  GetPolylinePoint (PtrAbstractBox pAb, int x, int y, int frame,
				 int *pointselect)
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
      if (j >= adbuff->BuLength &&
	  adbuff->BuNext != NULL)
	{
	  /* Changement de buffer */
	  adbuff = adbuff->BuNext;
	  j = 0;
	}

      /* Teste si le point est sur ce segment */
      X2 = PixelValue (adbuff->BuPoints[j].XCoord, UnPixel, NULL,
		       ViewFrameTable[frame - 1].FrMagnification);
      Y2 = PixelValue (adbuff->BuPoints[j].YCoord, UnPixel, NULL,
		       ViewFrameTable[frame - 1].FrMagnification);
      if (x >= X2 - DELTA_SEL && x <= X2 + DELTA_SEL &&
	  y >= Y2 - DELTA_SEL && y <= Y2 + DELTA_SEL)
	{
	  /* La selection porte sur un point de controle particulier */
	  *pointselect = i;
	  return (box);
	}
      else if (X1 == -1)
	OK = FALSE;
      else
	OK = IsOnSegment (x, y, X1, Y1, X2, Y2);

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
      X2 = PixelValue (box->BxBuffer->BuPoints[1].XCoord, UnPixel, NULL,
		       ViewFrameTable[frame - 1].FrMagnification);
      Y2 = PixelValue (box->BxBuffer->BuPoints[1].YCoord, UnPixel, NULL,
		       ViewFrameTable[frame - 1].FrMagnification);
      OK = IsOnSegment (x, y, X1, Y1, X2, Y2);
      if (OK)
	return (box);
    }
  return (NULL);
}

/*----------------------------------------------------------------------
   BuildPolygonForPath
   Build the polygons that approximate a path
   A different 
   pPa: first segment of the path
   return a list of points and the number of points in this list (npoints).
   if the path is unique, subpathStart is NULL, but if there are several
   subpath, subpathStart is a table of integers: each of them is the rank
   of the first point of each subpath in the list of points returned.
  ----------------------------------------------------------------------*/
static ThotPoint*  BuildPolygonForPath (PtrPathSeg pPa, int frame,
					int* npoints, int **subpathStart)
{
  float               x1, y1, cx1, cy1, x2, y2, cx2, cy2;
  int                 ix1, ix2, iy1, iy2;
  ThotPoint           *points;
  int                 *tmp;
  int                 maxpoints, maxsubpaths, nbsubpaths;

  /* get a buffer to store the points of the polygon */
  maxpoints = ALLOC_POINTS;
  points = (ThotPoint *) TtaGetMemory (maxpoints * sizeof(ThotPoint));
  *npoints = 0;
  /* assume there is a single path */
  *subpathStart = NULL;
  nbsubpaths = 0;
  maxsubpaths = 10;
  /* process all segments of the path */
  while (pPa)
    {
      if (pPa->PaNewSubpath && pPa->PaPrevious)
	/* this path segment starts a new subpath */
	{
	  if (*subpathStart == NULL)
	    /* allocate a table of subpath start points */
	    *subpathStart = (int *) TtaGetMemory (maxsubpaths * sizeof(int));
	  else if (nbsubpaths >= maxsubpaths - 1)
	    /* the current table is full. Extend it */
	    {
	      maxsubpaths += 10;
	      tmp = (int *) realloc(*subpathStart, maxsubpaths * sizeof(int));
	      *subpathStart = tmp;
	    }
	  /* register the rank of the point starting this subpath */
	  (*subpathStart)[nbsubpaths++] = *npoints;
	  (*subpathStart)[nbsubpaths] = 0;   /* indicate end of table */
	}

      switch (pPa->PaShape)
	{
	case PtLine:
	  ix1 = PixelValue (pPa->XStart, UnPixel, NULL,
			    ViewFrameTable[frame - 1].FrMagnification);
	  iy1 = PixelValue (pPa->YStart, UnPixel, NULL,
			    ViewFrameTable[frame - 1].FrMagnification);
	  ix2 = PixelValue (pPa->XEnd, UnPixel, NULL,
			    ViewFrameTable[frame - 1].FrMagnification);
	  iy2 = PixelValue (pPa->YEnd, UnPixel, NULL,
			    ViewFrameTable[frame - 1].FrMagnification);
	  PolyNewPoint (ix1, iy1, &points, npoints, &maxpoints);
	  PolyNewPoint (ix2, iy2, &points, npoints, &maxpoints);
	  break;

	case PtCubicBezier:
	  x1 = (float) (PixelValue (pPa->XStart, UnPixel, NULL,
				   ViewFrameTable[frame - 1].FrMagnification));
	  y1 = (float) (PixelValue (pPa->YStart, UnPixel, NULL,
				   ViewFrameTable[frame - 1].FrMagnification));
	  cx1 = (float) (PixelValue (pPa->XCtrlStart, UnPixel, NULL,
				   ViewFrameTable[frame - 1].FrMagnification));
	  cy1 = (float) (PixelValue (pPa->YCtrlStart, UnPixel, NULL,
				   ViewFrameTable[frame - 1].FrMagnification));
	  x2 = (float) (PixelValue (pPa->XEnd, UnPixel, NULL,
				   ViewFrameTable[frame - 1].FrMagnification));
	  y2 = (float) (PixelValue (pPa->YEnd, UnPixel, NULL,
				   ViewFrameTable[frame - 1].FrMagnification));
	  cx2 = (float) (PixelValue (pPa->XCtrlEnd, UnPixel, NULL,
				   ViewFrameTable[frame - 1].FrMagnification));
	  cy2 = (float) (PixelValue (pPa->YCtrlEnd, UnPixel, NULL,
				   ViewFrameTable[frame - 1].FrMagnification));
	  PolySplit (x1, y1, cx1, cy1, cx2, cy2, x2, y2, &points, npoints,
		     &maxpoints);
	  PolyNewPoint ((int) x2, (int) y2, &points, npoints, &maxpoints);
	  break;

	case PtQuadraticBezier:
	  x1 = (float) (PixelValue (pPa->XStart, UnPixel, NULL,
				   ViewFrameTable[frame - 1].FrMagnification));
	  y1 = (float) (PixelValue (pPa->YStart, UnPixel, NULL,
				   ViewFrameTable[frame - 1].FrMagnification));
	  cx1 = (float) (PixelValue (pPa->XCtrlStart, UnPixel, NULL,
				   ViewFrameTable[frame - 1].FrMagnification));
	  cy1 = (float) (PixelValue (pPa->YCtrlStart, UnPixel, NULL,
				   ViewFrameTable[frame - 1].FrMagnification));
	  x2 = (float) (PixelValue (pPa->XEnd, UnPixel, NULL,
				   ViewFrameTable[frame - 1].FrMagnification));
	  y2 = (float) (PixelValue (pPa->YEnd, UnPixel, NULL,
				   ViewFrameTable[frame - 1].FrMagnification));
	  QuadraticSplit (x1, y1, cx1, cy1, x2, y2, &points, npoints,
			  &maxpoints);
	  PolyNewPoint ((int) x2, (int) y2, &points, npoints, &maxpoints);
	  break;

	case PtEllipticalArc:
	  /**** to do ****/
	  break;
	}
      pPa = pPa->PaNext;
    }
  return (points);
}

/*----------------------------------------------------------------------
  IsInShape returns TRUE if the point x, y is included by the drawing.
  ----------------------------------------------------------------------*/
static ThotBool     IsInShape (PtrAbstractBox pAb, int x, int y)
{
  int                 point[8][2];
  int                 cross;
  int                 i, max;
  int                 prevX, prevY;
  int                 nextX, nextY;
  int                 arc;
  float               value1, value2;
  PtrBox              box;
  ThotBool            ok;

  box = pAb->AbBox;
  x -= box->BxXOrg;
  y -= box->BxYOrg;
  max = 0;

  /* Is there a characteristic point of the drawing? */
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
    case 'P':		/* rectangles with rounded corners */
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
      value2 = (y - ((float) box->BxHeight / 2)) *
                ((float) box->BxWidth / (float) box->BxHeight);
      value1 = value1 * value1 + value2 * value2;
      value2 = (float) box->BxWidth / 2;
      value2 = value2 * value2;
      if (value1 <= value2)
	return (TRUE);	/* within the circle */
      else
	return (FALSE);	/* out of the circle */
      break;
    default:
      break;
    }
  if (max < 2)
    /* only lines */
    return (FALSE);

   /* first and last points of the polyline */
   i = 0;
   cross = 0;
   nextX = point[0][0];
   nextY = point[0][1];
   prevX = point[max][0];
   prevY = point[max][1];
   if ((prevY >= y) != (nextY >= y))
     /* y between nextY and prevY */
     cross = CrossLine (x, y, prevX, prevY, nextX, nextY, cross);

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
	       i++;		/* get next point */
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
	   cross = CrossLine (x, y, prevX, prevY, nextX, nextY, cross);
	 }
       else
	 {
	   while (i <= max && nextY < y)
	     {
	       i++;		/* get next point */
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
	   cross = CrossLine (x, y, prevX, prevY, nextX, nextY, cross);
	 }
     }
   ok = (ThotBool) (cross & 0x01);
   return (ok);
}

/*----------------------------------------------------------------------
  IsOnShape checks if the point x, y is on the drawing of pAb.
  If yes, returns the box address, NULL in other cases.
  Return the control point for lines.
  ----------------------------------------------------------------------*/
static PtrBox       IsOnShape (PtrAbstractBox pAb, int x, int y, int *selpoint)
{
  PtrBox              pBox;
  int                 controlPoint;
  int                 arc;
  float               value1, value2, value3;

  /* relative coords of the box (easy work) */
  pBox = pAb->AbBox;
  x -= pBox->BxXOrg;
  y -= pBox->BxYOrg;
  *selpoint = 0;
  /* Keep in mind the selected caracteristic point       */
  /*            1-------------2-------------3            */
  /*            |                           |            */
  /*            |                           |            */
  /*            8                           4            */
  /*            |                           |            */
  /*            |                           |            */
  /*            7-------------6-------------5            */

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
      /* rectangle */
      if (IsOnSegment (x, y, 0, 0, pBox->BxWidth, 0) ||
	  IsOnSegment (x, y, 0, pBox->BxHeight, pBox->BxWidth,
			pBox->BxHeight) ||
	  IsOnSegment (x, y, 0, 0, 0, pBox->BxHeight) ||
	  IsOnSegment (x, y, pBox->BxWidth, 0, pBox->BxWidth, pBox->BxHeight))
	return (pBox);
      break;
    case 'L':
      if (IsOnSegment (x, y, 0, pBox->BxHeight / 2, pBox->BxWidth / 2, 0) ||
	  IsOnSegment (x, y, 0, pBox->BxHeight / 2, pBox->BxWidth / 2,
			pBox->BxHeight) ||
	  IsOnSegment (x, y, pBox->BxWidth, pBox->BxHeight / 2,
			pBox->BxWidth / 2, 0) ||
	  IsOnSegment (x, y, pBox->BxWidth, pBox->BxHeight / 2,
			pBox->BxWidth / 2, pBox->BxHeight))
	return (pBox);
      break;
    case 'C':
    case 'P':
      /* rectangle with rounded corners */
      arc = (int) ((3 * DOT_PER_INCHE) / 25.4 + 0.5);
      if (IsOnSegment (x, y, arc, 0, pBox->BxWidth - arc, 0) ||
	  IsOnSegment (x, y, 0, arc, 0, pBox->BxHeight - arc) ||
	  IsOnSegment (x, y, arc, pBox->BxHeight, pBox->BxWidth - arc,
			pBox->BxHeight) ||
	  IsOnSegment (x, y, pBox->BxWidth, arc, pBox->BxWidth,
			pBox->BxHeight - arc))
	return (pBox);
      break;
    case 'a': /* circle */
    case 'c': /* ellipse */
    case 'Q': /* ellipse with a bar */
      /* ellipse or circle */
      value1 = x - ((float) pBox->BxWidth / 2);
      value2 = (y - ((float) pBox->BxHeight / 2)) *
                ((float) pBox->BxWidth / (float) pBox->BxHeight);
      /* value1 = square of (distance from center to point) */
      value1 = value1 * value1 + value2 * value2;
      /* value2 = square of (radius - DELTA_SEL) */
      value2 = (float) pBox->BxWidth / 2;
      value3 = value2;
      value2 -= DELTA_SEL;
      value2 = value2 * value2;
      /* value3 = square of (radius + DELTA_SEL) */
      value3 += DELTA_SEL;
      value3 = value3 * value3;

      if (value1 >= value2 && value1 <= value3)
	return (pBox);	/* on the circle */
      break;
    case 'W':
      /* upper right corner of the box */
      if (controlPoint == 1 || controlPoint == 3 || controlPoint == 5 ||
	  IsOnSegment (x, y, 0, 0, pBox->BxWidth, 0) ||
	  IsOnSegment (x, y, pBox->BxWidth, 0, pBox->BxWidth, pBox->BxHeight))
	return (pBox);
      break;
    case 'X':
      /* lower right corner of the box */
      if (controlPoint == 3 || controlPoint == 5 || controlPoint == 7 ||
	  IsOnSegment (x, y, pBox->BxWidth, 0, pBox->BxWidth,
			pBox->BxHeight) ||
	  IsOnSegment (x, y, pBox->BxWidth, pBox->BxHeight, 0,pBox->BxHeight))
	return (pBox);
      break;
    case 'Y':
      /* a segment with an arrow head at the end */
      if (controlPoint == 1 || controlPoint == 5 || controlPoint == 7 ||
	  IsOnSegment (x, y, pBox->BxWidth, pBox->BxHeight, 0,
			pBox->BxHeight) ||
	  IsOnSegment (x, y, 0, pBox->BxHeight, 0, 0))
	return (pBox);
      break;
    case 'Z':
      /* the upper left corner of the box */
      if (controlPoint == 1 || controlPoint == 3 || controlPoint == 7 ||
	  IsOnSegment (x, y, 0, pBox->BxHeight, 0, 0) ||
	  IsOnSegment (x, y, 0, 0, pBox->BxWidth, 0))
	return (pBox);
      break;
    case 'h':
    case '<':
    case '>':
      /* a horizontal line or arrow */
      if (controlPoint == 4 || controlPoint == 8 ||
	  IsOnSegment (x, y, 0, pBox->BxHeight / 2, pBox->BxWidth,
			pBox->BxHeight / 2))
	return (pBox);
      break;
    case 't':
      /* a horizontal line along the upper side of the box */
      if (controlPoint == 1 || controlPoint == 2 || controlPoint == 3 ||
	  IsOnSegment (x, y, 0, 0, pBox->BxWidth, 0))
	return (pBox);
      break;
    case 'b':
      /* a horizontal line along the lower side of the box */
      if (controlPoint == 5 || controlPoint == 6 || controlPoint == 7 ||
	  IsOnSegment (x, y, pBox->BxWidth, pBox->BxHeight, 0,pBox->BxHeight))
	return (pBox);
      break;
    case 'v':
    case '^':
    case 'V':
      /* a vertical line or arrow as tall as the box and placed in its middle*/
      if (controlPoint == 2 || controlPoint == 6 ||
	  IsOnSegment (x, y, pBox->BxWidth / 2, 0, pBox->BxWidth / 2,
			pBox->BxHeight))
	return (pBox);
      break;
    case 'l':
      /* a vertical line on the left side of the box */
      if (controlPoint == 1 || controlPoint == 7 || controlPoint == 8 ||
	  IsOnSegment (x, y, 0, pBox->BxHeight, 0, 0))
	return (pBox);
      break;
    case 'r':
      /* a vertical line on the right side of the box */
      if (controlPoint == 3 || controlPoint == 4 || controlPoint == 5 ||
	  IsOnSegment (x, y, pBox->BxWidth, 0, pBox->BxWidth, pBox->BxHeight))
	return (pBox);
      break;
    case '\\':
    case 'O':
    case 'e':
      /* The northwest/southeast diagonal of the box possibly with an
	 arrowhead */
      if (controlPoint == 1 || controlPoint == 5 ||
	  IsOnSegment (x, y, 0, 0, pBox->BxWidth, pBox->BxHeight))
	return (pBox);
      break;
    case '/':
    case 'o':
    case 'E':
      /* The southwest/northeast diagonal of the box possibly with an
	 arrowhead */
      if (controlPoint == 3 || controlPoint == 7 ||
	  IsOnSegment (x, y, 0, pBox->BxHeight, pBox->BxWidth, 0))
	return (pBox);
      break;
    case 'g':
      /* a line from the origin of the box to its opposite corner */
      pAb = pAb->AbEnclosing;
      if ((pAb->AbHorizPos.PosEdge == Left && pAb->AbVertPos.PosEdge == Top) ||
	  (pAb->AbHorizPos.PosEdge == Right && pAb->AbVertPos.PosEdge == Bottom))
	{
	  /* draw a \ */
	  if (controlPoint == 1 || controlPoint == 5)
	    {
	      *selpoint = controlPoint;
	      return (pBox);
	    }
	  else  if (IsOnSegment (x, y, 0, 0, pBox->BxWidth, pBox->BxHeight))
	    return (pBox);
	}
      else
	{
	  /* draw a / */
	  if (controlPoint == 3 || controlPoint == 7)
	    {
	      *selpoint = controlPoint;
	      return (pBox);
	    }
	  else  if (IsOnSegment (x, y, 0, pBox->BxHeight, pBox->BxWidth, 0))
	    return (pBox);
	}
      break;
    default:
      break;
    }
  return (NULL);
}

/*----------------------------------------------------------------------
  GetClickedAbsBox checks if the abstract box includes the reference point
  xRef, yRef.
  Returns the most elementary box (structural level) that includes the
  reference point.
  ----------------------------------------------------------------------*/
PtrAbstractBox      GetClickedAbsBox (int frame, int xRef, int yRef)
{
  ViewFrame          *pFrame;
  PtrBox              pBox;
  int                 pointselect;

  pFrame = &ViewFrameTable[frame - 1];
  pBox = NULL;
  if (pFrame->FrAbstractBox != NULL)
    if (ThotLocalActions[T_selecbox] != NULL)
      (*ThotLocalActions[T_selecbox]) (&pBox, pFrame->FrAbstractBox, frame,
				       xRef + pFrame->FrXOrg,
				       yRef + pFrame->FrYOrg, &pointselect);
  if (pBox == NULL)
    return (NULL);
  else
    return (pBox->BxAbstractBox);
}


/*----------------------------------------------------------------------
  GetEnclosingClickedBox checks if the point x, y belongs to the abstract
  box pAb.
  Returns the box address or NULL.
  ----------------------------------------------------------------------*/
PtrBox          GetEnclosingClickedBox (PtrAbstractBox pAb, int higherX,
					int lowerX, int y, int frame,
					int *pointselect)
{
  PtrBox              pBox;
  PtrElement          pParent;
  int                 i, x;
  ThotPoint           *points = NULL;
  int                 npoints, sub;
  int                 *subpathStart = NULL;
  ThotBool            OK, testSegment;

  *pointselect = 0;
  if (pAb->AbBox)
    {
      pBox = pAb->AbBox;
      /* Is there a piece of split box? */
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
      else if (pAb->AbLeafType == LtPolyLine || pAb->AbLeafType == LtPath ||
	       /* If the box is not a polyline or a path, it must include
		  the point */
	       (pBox->BxXOrg <= lowerX &&
		pBox->BxXOrg + pBox->BxWidth >= higherX &&
		pBox->BxYOrg <= y &&
		pBox->BxYOrg + pBox->BxHeight >= y))
	{
	  pParent = pAb->AbElement->ElParent;
	  if (pAb->AbLeafType == LtGraphics && pAb->AbVolume != 0)
	    /* It's a simple graphic shape */
	    {
	      pBox = IsOnShape (pAb, lowerX, y, pointselect);
	      if (pBox != NULL)
		/* the point is on the outline */
		return (pBox);
	      /* the point is not on the outline */
	      if ((pAb->AbFillPattern > 0 && pAb->AbBackground >= 0) ||
		  TypeHasException (ExcClickableSurface, pParent->ElTypeNumber,
				    pParent->ElStructSchema))
		/* the box is filled. Is the point within the shape? */
		{
		  if (IsInShape (pAb, lowerX, y))
		    return (pAb->AbBox);
		  else
		    return (NULL);
		}
	      else
		return (pBox);
	    }
	  else if (pAb->AbLeafType == LtPolyLine && pAb->AbVolume > 2)
	    {
	      /* the polyline contains at least one segment */
	      pBox = GetPolylinePoint (pAb, lowerX, y, frame, pointselect);
	      if (pBox != NULL)
		/* the point doesn't belong to a segment */
		return (pBox);
	      if ((pAb->AbFillPattern > 0 && pAb->AbBackground >= 0) ||
		  TypeHasException (ExcClickableSurface, pParent->ElTypeNumber,
				    pParent->ElStructSchema))
		/* the shape is filled. Is the point within the shape? */
		{
		  if (IsWithinPolyline (pAb, lowerX, y, frame))
		    return (pAb->AbBox);
		  else
		    return (pBox);
		}
	    }
	  else if (pAb->AbLeafType == LtPath && pAb->AbFirstPathSeg)
	    /* it's a non-empty path */
	    {
	      /* builds the list of points representing the path */
	      points = BuildPolygonForPath (pAb->AbFirstPathSeg, frame,
					    &npoints, &subpathStart);
	      /* is the position of interest on the polyline represented by
		 these points? */
	      x = lowerX - pBox->BxXOrg;
	      y -= pBox->BxYOrg;
	      OK = FALSE;
	      sub = 0;
	      /* test every segment comprised between 2 successive points */
	      for (i = 0; (i < npoints - 1) && !OK; i++)
		{
		  testSegment = TRUE;
		  if (subpathStart)
		    /* there are several subpaths in this path */
		    {
		      if (subpathStart[sub] == i + 1)
			/* this segment corresponds to a moveto. Skip it */
			{
			  testSegment = FALSE;
			  /* get prepared for the next subpath */
			  sub++;
			  if (subpathStart[sub] == 0)
			    /* this is the last subpath. Don't test more
			       subpaths */
			    {
			      free (subpathStart);
			      subpathStart = NULL;
			    }
			}
		    }
		  if (testSegment)
		    OK = IsOnSegment (x, y, points[i].x, points[i].y,
				      points[i + 1].x, points[i + 1].y);
		}
	      if (!OK)
		/* the point is not on the path. Is it within the path ? */
		{
		  /* check only if the path is filled */
		  if ((pAb->AbFillPattern > 0 && pAb->AbBackground >= 0) ||
		      TypeHasException (ExcClickableSurface,
					pParent->ElTypeNumber,
					pParent->ElStructSchema))
		    OK = IsWithinPath (x, y, points, npoints);
		}
	      free (points);
	      if (subpathStart)
		free (subpathStart);
	      if (OK)
		return (pBox);
	      else
		return (NULL);
	    }
	  else
	    return (pBox);
	}
    }
  return (NULL);
}

/*----------------------------------------------------------------------
   GetLeafBox returns the leaf box located at the position x+xDelta
   y+yDelta from pSourceBox box.
  ----------------------------------------------------------------------*/
PtrBox GetLeafBox (PtrBox pSourceBox, int frame, int *x, int *y, int xDelta, int yDelta)
{
  int                 i;
  PtrBox              pBox, pLimitBox;
  PtrBox              box, lastBox;
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
	  else if (TypeHasException (ExcSelectParent,
			     pBox->BxAbstractBox->AbElement->ElTypeNumber,
			     pBox->BxAbstractBox->AbElement->ElStructSchema) &&
		   pBox->BxAbstractBox->AbEnclosing)
	    {
	      /* let the algorithm work if we obtain the same box */
	      if (pBox != pSourceBox)
		pBox = pBox->BxAbstractBox->AbEnclosing->AbBox;
	    }
	  if (pBox == pSourceBox || pBox->BxAbstractBox->AbBox == pSourceBox ||
	      IsParentBox (pSourceBox, pBox))
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
		  if (pLine  && pLine->LiPrevious)
		    {
		      box = pBox->BxAbstractBox->AbEnclosing->AbBox;
		      while (box->BxType == BoGhost)
			box = box->BxAbstractBox->AbEnclosing->AbBox;
		      *x = box->BxXOrg + pLine->LiPrevious->LiRealLength;
		      yDelta = -2;
		    }
		  else
		    {
		    *x = max;
		    yDelta = -h;
		    }
		  *y = pBox->BxYOrg;
		  xDelta = 0;
		  found = FALSE;
		}
	    }
	  else if (pBox->BxAbstractBox->AbLeafType != LtText &&
		   pBox->BxNChars != 0)
	    {
	      /* the box doesn't match, skip it */
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
  GetGetMinimumDistance computes the distance between a reference point
  xRef, yRef and an anchor x, y of a box.
  We apply a ratio to vertical distances to give a preference to the
  horizontal proximity.
  ----------------------------------------------------------------------*/
static int          GetGetMinimumDistance (int xRef, int yRef, int x, int y,
					   int dist)
{
  int                 value;

  value = GetDistance (x - xRef, ANCHOR_SIZE) +
          GetDistance (y - yRef, ANCHOR_SIZE) * Y_RATIO;
  if (value < dist)
    return (value);
  else
    return (dist);
}

/*----------------------------------------------------------------------
  GetShapeDistance computes the distance between a reference point
  xRef, yRef and a point of the graphic box (minimum distance to an anchor
  of the box.
  This selection takes only laf boxes into account.
  Returns the distance.
  ----------------------------------------------------------------------*/
int                 GetShapeDistance (int xRef, int yRef, PtrBox pBox,
				      int value)
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
    case 1:		/* root .. */
      distance = GetGetMinimumDistance (xRef, yRef, x - width + height / 3,
					y + height, distance);
      distance = GetGetMinimumDistance (xRef, yRef,
					x - width + (2 * height) / 3,
					y - height, distance);
      distance = GetGetMinimumDistance (xRef, yRef, x + width, y - height,
					distance);
      break;

    case 'c':		/* circle */
      if (width < height)
	height = width;
      else
	width = height;
      /* Warning: continue as a circle is also an ellipse */
    case 'C':		/* ellipse */
      distance = GetGetMinimumDistance (xRef, yRef, x - width, y, distance);
      distance = GetGetMinimumDistance (xRef, yRef, x + width, y, distance);
      distance = GetGetMinimumDistance (xRef, yRef, x, y - height, distance);
      distance = GetGetMinimumDistance (xRef, yRef, x, y + height, distance);
      height = (71 * height) / 100;
      width = (71 * width) / 100;
      distance = GetGetMinimumDistance (xRef, yRef, x - width, y + height,
					distance);
      distance = GetGetMinimumDistance (xRef, yRef, x - width, y - height,
					distance);
      distance = GetGetMinimumDistance (xRef, yRef, x + width, y + height,
					distance);
      distance = GetGetMinimumDistance (xRef, yRef, x + width, y - height,
					distance);
      break;

    case 'L':		/* losange */
      distance = GetGetMinimumDistance (xRef, yRef, x - width, y, distance);
      distance = GetGetMinimumDistance (xRef, yRef, x + width, y, distance);
      distance = GetGetMinimumDistance (xRef, yRef, x, y - height, distance);
      distance = GetGetMinimumDistance (xRef, yRef, x, y + height, distance);
      break;

    case 't':		/* top line */
      distance = GetGetMinimumDistance (xRef, yRef, x - width, y - height,
					distance);
      distance = GetGetMinimumDistance (xRef, yRef, x + width, y - height,
					distance);
      distance = GetGetMinimumDistance (xRef, yRef, x, y - height, distance);
      break;

    case 'b':		/* bottom line */
      distance = GetGetMinimumDistance (xRef, yRef, x - width, y + height,
					distance);
      distance = GetGetMinimumDistance (xRef, yRef, x + width, y + height,
					distance);
      distance = GetGetMinimumDistance (xRef, yRef, x, y + height, distance);
      break;

    case 'h':		/* middle line or arrow */
    case '<':
    case '>':
      distance = GetGetMinimumDistance (xRef, yRef, x - width, y, distance);
      distance = GetGetMinimumDistance (xRef, yRef, x + width, y, distance);
      distance = GetGetMinimumDistance (xRef, yRef, x, y, distance);
      break;

    case 'v':
    case 'V':		/* centerd line or arrow (^) */
    case '^':
      distance = GetGetMinimumDistance (xRef, yRef, x, y - height, distance);
      distance = GetGetMinimumDistance (xRef, yRef, x, y + height, distance);
      distance = GetGetMinimumDistance (xRef, yRef, x, y, distance);
      break;

    case 'l':		/* left line */
      distance = GetGetMinimumDistance (xRef, yRef, x - width, y - height,
					distance);
      distance = GetGetMinimumDistance (xRef, yRef, x - width, y + height,
					distance);
      distance = GetGetMinimumDistance (xRef, yRef, x - width, y, distance);
      break;
    case 'r':		/* right line */
      distance = GetGetMinimumDistance (xRef, yRef, x + width, y - height,
					distance);
      distance = GetGetMinimumDistance (xRef, yRef, x + width, y + height,
					distance);
      distance = GetGetMinimumDistance (xRef, yRef, x + width, y, distance);
      break;

    case '/':		/* diagonal (/) or arrow */
    case 'E':
    case 'o':
      distance = GetGetMinimumDistance (xRef, yRef, x - width, y + height,
					distance);
      distance = GetGetMinimumDistance (xRef, yRef, x + width, y - height,
					distance);
      distance = GetGetMinimumDistance (xRef, yRef, x, y, distance);
      break;

    case '\\':		/* diagonal (\) or arrow */
    case 'e':
    case 'O':
      distance = GetGetMinimumDistance (xRef, yRef, x - width, y - height,
					distance);
      distance = GetGetMinimumDistance (xRef, yRef, x + width, y + height,
					distance);
      distance = GetGetMinimumDistance (xRef, yRef, x, y, distance);
      break;
    default:
      /* rectangle or other cases */
      distance = GetGetMinimumDistance (xRef, yRef, x - width, y + height,
					distance);
      distance = GetGetMinimumDistance (xRef, yRef, x - width, y - height,
					distance);
      distance = GetGetMinimumDistance (xRef, yRef, x + width, y + height,
					distance);
      distance = GetGetMinimumDistance (xRef, yRef, x + width, y - height,
					distance);
      break;
    }
  return (distance);
}

/*----------------------------------------------------------------------
  GetClickedLeafBox looks for a leaf box located at a reference point
   xRef, yRef.
  ----------------------------------------------------------------------*/
PtrBox              GetClickedLeafBox (int frame, int xRef, int yRef)
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
	  if (pAb->AbVisibility >= pFrame->FrVisibility &&
	      (!pAb->AbPresentationBox || pAb->AbCanBeModified))
	    {
	      if (pAb->AbLeafType == LtGraphics ||
		  pAb->AbLeafType == LtPolyLine ||
		  pAb->AbLeafType == LtPath)
		{
		  pCurrentBox = GetEnclosingClickedBox (pAb, xRef, xRef, yRef,
							frame, &pointIndex);
		  if (pCurrentBox == NULL)
		    d = max + 1;
		  else
		    d = 0;
		}
	      else if (pAb->AbLeafType == LtSymbol && pAb->AbShape == 'r')
		/* glitch for the root symbol */
		d = GetShapeDistance (xRef, yRef, pBox, 1);
	      else if (pAb->AbLeafType == LtText ||
		       pAb->AbLeafType == LtSymbol ||
		       pAb->AbLeafType == LtPicture ||
		       /* empty or compound box */
		       (pAb->AbLeafType == LtCompound &&
			pAb->AbVolume == 0))
		d = GetBoxDistance (xRef, yRef, pBox->BxXOrg, pBox->BxYOrg,
				    pBox->BxWidth, pBox->BxHeight);
	      else
		d = max + 1;

	      /* get the closest element */
	      if (d < max)
		{
		  max = d;
		  pSelBox = pBox;
		}
	      else if (d == max)
		{
		  /* If it's the first found box */
		  if (pSelBox == NULL)
		    {
		      max = d;
		      pSelBox = pBox;
		    }
		  /* If the box is over the previous selected box */
		  else if (pSelBox->BxAbstractBox->AbDepth >=
			                         pBox->BxAbstractBox->AbDepth)
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
  GiveMovingArea get limits of the box moving.
  ----------------------------------------------------------------------*/
static void         GiveMovingArea (PtrAbstractBox pAb, int frame,
				    ThotBool horizRef, int *min, int *max)
{
#ifdef IV
   PtrAbstractBox      pParentAb;
#endif /* IV */
   /* default values */
   *min = 0;
   *max = 100000;

   /* It's the root box */
   if (pAb == ViewFrameTable[frame - 1].FrAbstractBox)
     {
	if (horizRef)
	  {
	     /* does the box depend on the window */
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
   /* Other cases */
   else if (horizRef)
     {
       *min = -50;
       *max = 100000;
#ifdef IV
	/* check the enclosing abstract box */
	if (pAb->AbHorizEnclosing)
	   pParentAb = pAb->AbEnclosing;
	else
	   pParentAb = ViewFrameTable[frame - 1].FrAbstractBox;

	/* by default the emclosing box gives limits */
	*min = pParentAb->AbBox->BxXOrg;
	*max = *min + pParentAb->AbBox->BxWidth;

	if (pParentAb->AbBox->BxContentWidth)
	  /* the enclosing box gets the contents size and the box
	     doesn't depend of the enclosing */
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
	/* check the enclosing abstract box */
	if (pAb->AbVertEnclosing && pAb->AbEnclosing != NULL)
	  {
	    pParentAb = pAb;
	    do
	      pParentAb = pParentAb->AbEnclosing;
	    while (pParentAb->AbBox->BxType == BoGhost);
	  }
	else
	   pParentAb = ViewFrameTable[frame - 1].FrAbstractBox;

	/* by default the emclosing box gives limits */
	*min = pParentAb->AbBox->BxYOrg;
	*max = *min + pParentAb->AbBox->BxHeight;

	if (pParentAb->AbBox->BxContentHeight)
	  /* the enclosing box gets the contents size and the box
	     doesn't depend of the enclosing */
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
static ThotBool     CanBeTranslated (PtrAbstractBox pAb, int frame,
				     ThotBool horizRef, int *min, int *max)
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
	  if (TypeHasException (ExcNoMove, pEl->ElTypeNumber,
				pEl->ElStructSchema))
	    ok = FALSE;
	  else if (horizRef && TypeHasException (ExcNoHMove, pEl->ElTypeNumber,
						 pEl->ElStructSchema))
	    ok = FALSE;
	  else if (!horizRef && TypeHasException (ExcNoVMove,pEl->ElTypeNumber,
						  pEl->ElStructSchema))
	    ok = FALSE;
	  else if (TypeHasException (ExcMoveResize, pEl->ElTypeNumber,
				     pEl->ElStructSchema))
	    found = TRUE;
	  /* if no directive is done, see the parent */
	  pEl = pEl->ElParent;
	}

      if (!found && ok && pParentAb != NULL)
	{
	  /* it's not the root box */
	  if (horizRef &&
	      /* et le pParentAb ne depend pas de son contenu */
	      pParentAb->AbBox->BxContentWidth &&
	      !pParentAb->AbWidth.DimIsPosition &&
	      !pParentAb->AbWidth.DimMinimum &&
	      pAb->AbHorizPos.PosAbRef == pParentAb &&
	      pAb->AbHorizPos.PosRefEdge != Left)
	    ok = FALSE;
	  else if (!horizRef &&
		   /* et le pParentAb ne depend pas de son contenu */
		   pParentAb->AbBox->BxContentHeight &&
		   !pParentAb->AbHeight.DimIsPosition &&
		   !pParentAb->AbHeight.DimMinimum &&
		   pAb->AbVertPos.PosAbRef == pParentAb &&
		   pAb->AbVertPos.PosRefEdge != Top)
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
ThotBool          APPgraphicModify (PtrElement pEl, int value, int frame, ThotBool pre)
{
  PtrElement          pAsc;
  ThotBool            result;
  NotifyOnValue       notifyEl;
  PtrDocument         pDoc;
  int                 view;
  ThotBool            assoc;
  ThotBool            ok = FALSE;

  GetDocAndView (frame, &pDoc, &view, &assoc);
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
	  (pEl->ElLeafType == LtPolyLine && pEl->ElPolyLineType != '\0') ||
	  (pEl->ElLeafType == LtPath))
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
  ApplyDirectTranslate looks for the selected box for a move.
  If the smaller box enclosing the point xm, ym of the window cannot
  be moved, the function checks the encolsing box, etc.
  ----------------------------------------------------------------------*/
void              ApplyDirectTranslate (int frame, int xm, int ym)
{
  PtrBox              pBox;
  PtrAbstractBox      pAb;
  PtrElement	      pEl;
  ViewFrame          *pFrame;
  int                 x, width;
  int                 y, height;
  int                 xmin, xmax;
  int                 ymin, ymax;
  int                 xref, yref;
  int                 pointselect;
  ThotBool            still, okH, okV;

  pFrame = &ViewFrameTable[frame - 1];
  /* by default no selected point */
  pointselect = 0;
  if (pFrame->FrAbstractBox != NULL)
    {
      /* Get positions in the window */
      x = xm + pFrame->FrXOrg;
      y = ym + pFrame->FrYOrg;

      /* Look for the box displayed at that point */
      if (ThotLocalActions[T_selecbox] != NULL)
	(*ThotLocalActions[T_selecbox]) (&pBox, pFrame->FrAbstractBox,
					 frame, x, y, &pointselect);
      if (pBox)
	{
	  pAb = pBox->BxAbstractBox;
	  if (pointselect && pBox->BxType != BoPicture)
	    {
	      /* moving a polyline point */
	      still = FALSE;
	      xmin = ymin = 0;
	      xmax = ymax = 9999;
	    }
	  else
	    {
	      /* moving the whole box */
	      still = TRUE;
	      pointselect = 0;
	    }
	  /* Loop as long as a box that can be moved is not found */
	  while (still)
	    {
	      /* check if the moving is allowed */
	      okH = CanBeTranslated (pAb, frame, TRUE, &xmin, &xmax);
	      okV = CanBeTranslated (pAb, frame, FALSE, &ymin, &ymax);
	      if (okH || okV)
		still = FALSE;
	      if (still)
		{
		  /* no box found yet, check the enclosing box */
		  if (pAb != NULL)
		    pAb = pAb->AbEnclosing;
		  if (pAb == NULL)
		    {
		      pBox = NULL;
		      still = FALSE;
		    }
		  else
		    pBox = pAb->AbBox;
		}
	    }

	  if (pBox != NULL)
	    {
	      /* A box is found */
	      x = pBox->BxXOrg - pFrame->FrXOrg;
	      y = pBox->BxYOrg - pFrame->FrYOrg;
	      width = pBox->BxWidth;
	      height = pBox->BxHeight;
	      pEl = pBox->BxAbstractBox->AbElement;
	      if (!APPgraphicModify (pEl, pointselect, frame, TRUE))
		{
		  if (pointselect != 0 && pBox->BxType != BoPicture)
		    {
		      if (pAb->AbLeafType == LtGraphics && pAb->AbShape == 'g')
			{
			  LineModification (frame, pBox, pointselect, &x, &y);
			  
			  /* get back current changes */
			  if (!pAb->AbWidth.DimIsPosition && pAb->AbEnclosing)
			    /* this rule is applied to the parent */
			    pAb = pAb->AbEnclosing;
			  pBox = pAb->AbBox;
			  switch (pointselect)
			    {
			    case 1:
			    case 7:
			      if (pBox->BxHorizInverted)
				NewDimension (pAb, x, y, frame, TRUE);
			      else
				NewPosition (pAb, x, 0, y, 0, frame, TRUE);
			      break;
			    case 3:
			    case 5:
			      if (pBox->BxHorizInverted)
				NewPosition (pAb, x, 0, y, 0, frame, TRUE);
			      else
				NewDimension (pAb, x, y, frame, TRUE);
			      break;
			    default: break;
			    }
			}
		      else
			{
			  /* Moving a point in a polyline */
			  /* check if the polyline is open or closed */
			  still = (pAb->AbPolyLineShape == 'p' ||
				   pAb->AbPolyLineShape == 's');
			  PolyLineModification (frame, &x, &y, pBox,
						pBox->BxNChars, pointselect,
						still);
			  NewContent (pAb);
			}
		      /* redisplay the box */
		      DefClip (frame, pBox->BxXOrg - EXTRA_GRAPH,
			       pBox->BxYOrg - EXTRA_GRAPH,
			       pBox->BxXOrg + width + EXTRA_GRAPH,
			       pBox->BxYOrg + height + EXTRA_GRAPH);
		      RedrawFrameBottom (frame, 0, NULL);
		    }
		  else
		    {
		      /* set positions related to the window */
		      xmin -= pFrame->FrXOrg;
		      xmax -= pFrame->FrXOrg;
		      ymin -= pFrame->FrYOrg;
		      ymax -= pFrame->FrYOrg;
		      /* execute the interaction */
		      GeometryMove (frame, &x, &y, width, height, pBox, xmin,
				    xmax, ymin, ymax, xm, ym);
		      /* get back changes */
		      x += pFrame->FrXOrg;
		      y += pFrame->FrYOrg;
		      /* get the position of reference point */
		      switch (pBox->BxHorizEdge)
			{
			case Right:
			  xref = width;
			  break;
			case VertMiddle:
			  xref = width / 2;
			  break;
			case VertRef:
			  xref = pBox->BxVertRef;
			  break;
			default:
			  xref = 0;
			  break;
			}
		      switch (pBox->BxVertEdge)
			{
			case Bottom:
			  yref = height;
			  break;
			case HorizMiddle:
			  yref = height / 2;
			  break;
			case HorizRef:
			  yref = pBox->BxHorizRef;
			  break;
			default:
			  yref = 0;
			  break;
			}
		      NewPosition (pAb, x, xref, y, yref, frame, TRUE);
		    }
		  APPgraphicModify (pEl, pointselect, frame, FALSE);
		}
	    }
	}
    }
}

/*----------------------------------------------------------------------
   CanBeResized teste si un pave est modifiable en Dimension.       
  ----------------------------------------------------------------------*/
static ThotBool   CanBeResized (PtrAbstractBox pAb, int frame,
				ThotBool horizRef, int *min, int *max)
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
  else if (pAb->AbLeafType == LtText &&
	   (pParentAb->AbInLine || pParentAb->AbBox->BxType == BoGhost))
    /* text box displayed in block of lines */
    ok = FALSE;
  /* Il est impossible de modifier la dimension du contenu */
  /* d'une boite construite ou de type texte                  */
  /*****else if (pAb->AbLeafType == LtCompound || pAb->AbLeafType == LtText)
    {
      if (horizRef && pBox->BxContentWidth && !pAb->AbWidth.DimMinimum)
        ok = FALSE;
      else if (!horizRef && pBox->BxContentHeight && !pAb->AbHeight.DimMinimum)
        ok = FALSE;
    }********/
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
	  if (TypeHasException (ExcNoResize, pEl->ElTypeNumber,
				pEl->ElStructSchema))
	    ok = FALSE;
	  else if (horizRef && TypeHasException (ExcNoHResize,
						 pEl->ElTypeNumber,
						 pEl->ElStructSchema))
	    ok = FALSE;
	  else if (!horizRef && TypeHasException (ExcNoVResize,
						  pEl->ElTypeNumber,
						  pEl->ElStructSchema))
	    ok = FALSE;
	  else if (TypeHasException (ExcMoveResize, pEl->ElTypeNumber,
				     pEl->ElStructSchema))
	    found = TRUE;
	  /* if no directive is done, see the parent */
	  pEl = pEl->ElParent;
	}

      if (!found && ok && pParentAb != NULL)
	{
	  /* it's not the root box */
	  if ((horizRef && pAb->AbWidth.DimValue < 0) ||
	      (!horizRef && pAb->AbHeight.DimValue < 0))
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
   ApplyDirectResize looks for a box that can be resized at the current
   position (xm, ym).
  ----------------------------------------------------------------------*/
void                ApplyDirectResize (int frame, int xm, int ym)
{
  PtrBox              pBox;
  PtrAbstractBox      pAb;
  ViewFrame          *pFrame;
  int                 x, width;
  int                 y, height;
  int                 xmin, xmax;
  int                 ymin, ymax;
  int                 percentW, percentH;
  int                 pointselect;
  ThotBool            still, okH, okV;

  okH = FALSE;
  okV = FALSE;
  pFrame = &ViewFrameTable[frame - 1];
  if (pFrame->FrAbstractBox != NULL)
    {
      /* On note les coordonnees par rapport a l'image concrete */
      x = xm + pFrame->FrXOrg;
      y = ym + pFrame->FrYOrg;

      /* On recherche la boite englobant le point designe */
      /* designation style Grenoble */
      if (ThotLocalActions[T_selecbox] != NULL)
	(*ThotLocalActions[T_selecbox]) (&pBox, pFrame->FrAbstractBox, frame,
					 x, y, &pointselect);
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
	    {
	      /* On passe a la boite englobante */
	      if (pAb != NULL)
		pAb = pAb->AbEnclosing;
	      else
		{
		  pBox = NULL;
		  still = FALSE;
		}
	    }
	}

      /* Est-ce que l'on a trouve une boite ? */
      if (pBox != NULL)
	{
	  x = pBox->BxXOrg - pFrame->FrXOrg;
	  y = pBox->BxYOrg - pFrame->FrYOrg;
	  width = pBox->BxWidth;
	  height = pBox->BxHeight;
	  TtaDisplayMessage (INFO, TtaGetMessage (LIB, TMSG_MODIFYING_BOX),
			     AbsBoxType (pBox->BxAbstractBox, FALSE));
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
	  GeometryResize (frame, x, y, &width, &height, pBox,
			  xmin, xmax, ymin, ymax, xm, ym,
			  percentW, percentH);
	  /* Notify changes */
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
void                DirectCreation (PtrBox pBox, int frame)
{
  ViewFrame          *pFrame;
  PtrAbstractBox      pAb;
  PtrDocument         pDoc;
  int                 x, y, xref, yref;
  int                 width, height;
  int                 xmin, xmax;
  int                 Ymin, Ymax;
  int                 percentW, percentH;
  ThotBool            modPosition, modDimension;
  ThotBool            histOpen;

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
  modPosition = (CanBeTranslated (pAb, frame, TRUE, &xmin, &xmax) ||
		 CanBeTranslated (pAb, frame, FALSE, &Ymin, &Ymax));
  if (!modPosition)
    {
      pAb->AbHorizPos.PosUserSpecified = FALSE;
      pAb->AbVertPos.PosUserSpecified = FALSE;
    }
  modDimension = (CanBeResized (pAb, frame, TRUE, &xmin, &xmax) ||
		  CanBeResized (pAb, frame, FALSE, &Ymin, &Ymax));
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
      TtaDisplayMessage (INFO, TtaGetMessage (LIB, TMSG_CREATING_BOX),
			 AbsBoxType (pAb, FALSE));
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
      GeometryCreate (frame, &x, &y, &width, &height,
		      xmin, xmax, Ymin, Ymax, pBox,
		      pAb->AbHorizPos.PosUserSpecified,
		      pAb->AbVertPos.PosUserSpecified,
		      pAb->AbWidth.DimUserSpecified,
		      pAb->AbHeight.DimUserSpecified,
		      percentW, percentH);

      /* Notification of the new created box */
      x = x + pFrame->FrXOrg;
      y = y + pFrame->FrYOrg;
      /* get the position of reference point */
      switch (pBox->BxHorizEdge)
	{
	case Right:
	  xref = width;
	  break;
	case VertMiddle:
	  xref = width / 2;
	  break;
	case VertRef:
	  xref = pBox->BxVertRef;
	  break;
	default:
	  xref = 0;
	  break;
	}
      switch (pBox->BxVertEdge)
	{
	case Bottom:
	  yref = height;
	  break;
	case HorizMiddle:
	  yref = height / 2;
	  break;
	case HorizRef:
	  yref = pBox->BxHorizRef;
	  break;
	default:
	  yref = 0;
	  break;
	}
      pDoc = DocumentOfElement (pAb->AbElement);
      histOpen = pDoc->DocEditSequence;
      if (!histOpen)
	OpenHistorySequence (pDoc, pAb->AbElement, pAb->AbElement, 0, 0);
      NewPosition (pAb, x, xref, y, yref, frame, TRUE);
      if (percentW)
	NewDimension (pAb, 0, height, frame, TRUE);
      else if (percentH)
	NewDimension (pAb, width, 0, frame, TRUE);
      else
	NewDimension (pAb, width, height, frame, TRUE);
      DefClip (frame, pBox->BxXOrg, pBox->BxYOrg,
	       pBox->BxXOrg + width, pBox->BxYOrg + height);
      pAb->AbHorizPos.PosUserSpecified = FALSE;
      pAb->AbVertPos.PosUserSpecified = FALSE;
      pAb->AbWidth.DimUserSpecified = FALSE;
      pAb->AbHeight.DimUserSpecified = FALSE;
      if (!histOpen)
	CloseHistorySequence (pDoc);	  
    }

  /* Traitement de la creation interactive termine */
  BoxCreating = FALSE;
}

/*----------------------------------------------------------------------
  LocateClickedChar looks for the character of the box displayed at the
  position x.
  The parameter extend is TRUE when the function looks for an extension
  of the current selection.
  Returns:
  - the buffer where the character is located,
  - the index in the buffer,
  - the index in the box,
  - an the number of white spaces before.
  The position x is updated too.
  ----------------------------------------------------------------------*/
void LocateClickedChar (PtrBox pBox, ThotBool extend, PtrTextBuffer *pBuffer, int *x,
			int *index, int *charsNumber, int *spacesNumber)
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
      if (extend)
	notfound = (dx + charWidth < *x);
      else
	notfound = (dx + (charWidth / 2) < *x);
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
	     if (extend)
	       notfound = (dx + charWidth < *x);
	     else
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
