/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2005
 *  Please first read the full copyright statement in file COPYRIGHT.
 *exit

 */

/* 
 * Extrapolation of points describing a curve based 
 * on bezier, quadratic or cubic descriptions
 *
 * Author: I. Vatton (INRIA)
 *         P. Cheyrou-lagreze (INRIA)
 *
 */
#include "thot_gui.h"
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
#include "select_tv.h"

#include "absboxes_f.h"
#include "applicationapi_f.h"
#include "appli_f.h"
#include "boxmoves_f.h"
#include "boxlocate_f.h"
#include "boxselection_f.h"
#include "buildboxes_f.h"
#include "buildlines_f.h"
#include "callback_f.h"
#include "changepresent_f.h"
#include "dialogapi_f.h"
#include "displaybox_f.h"
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
#include "unstructlocate_f.h"
#include "views_f.h"
#include "word_f.h"

#ifdef _GL
#include "tesse_f.h"
#endif /* _GL */

/*
 * Math Macros conversion from
 * degrees to radians and so on...
 * All for EllipticSplit and/or GL_DrawArc
 */
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#define M_PI_DOUBLE (6.2831853718027492)

/* ((A)*(M_PI/180.0)) */
#define DEG_TO_RAD(A)   ((double)A)/57.29577957795135
#define RAD_TO_DEG(A)   ((double)A)*57.29577957795135

/*If we should use a static table instead for
  performance bottleneck...*/
#define DCOS(A) ((double)cos (A))
#define DSIN(A) ((double)sin (A))
#define DACOS(A) ((double)acos (A))
#define A_DEGREE 0.017453293

/* Precision of a degree/1 
   If we need more precision 
   dision is our friend 
   1/2 degree = 0.0087266465
   1/4 degree = 0.0043633233
   or the inverse 
   24 degree = 0.41887903
   5 degree = 0.087266465
   2 degree = 0.034906586
   but best is a degree...
*/
#define TRIGO_PRECISION 1;
#define A_DEGREE_PART A_DEGREE/TRIGO_RECISION

/* Must find better one... 
   Bits methods...*/
/*for double => ( pow (N, 2))*/
/*for int  => (((int)N)<<1)*/
#define P2(N) (N*N)

#define ALLOC_POINTS    300

#define	MAX_STACK	50
#define	MIDDLE_OF(v1, v2) (((v1)+(v2))/2.0)
#define SEG_SPLINE      5

typedef struct stack_point
  {
     double         x1, y1, x2, y2, x3, y3, x4, y4;
  }
StackPoint;
static StackPoint   stack[MAX_STACK];
static int          stack_deep;

/*----------------------------------------------------------------------
  PolyNewPoint : add a new point to the current polyline.
  ----------------------------------------------------------------------*/
ThotBool PolyNewPoint (double x, double y, ThotPoint **points, int *npoints,
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
#ifndef _GL
   /* ignore identical points */
   if (*npoints > 0 &&
       (*points)[*npoints - 1].x == x && (*points)[*npoints - 1].y == y)
      return (FALSE);
#endif/*  _GL */

   (*points)[*npoints].x = (long)x;
   (*points)[*npoints].y = (long)y;
   (*npoints)++;
   return (TRUE);
}

/*----------------------------------------------------------------------
  PushStack : push a spline on the stack.
  ----------------------------------------------------------------------*/
static void  PushStack (double x1, double y1, double x2, double y2, double x3,
			double y3, double x4, double y4)
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
static ThotBool PopStack (double *x1, double *y1, double *x2, double *y2,
			  double *x3, double *y3, double *x4, double *y4)
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
  PolySplit2 : split a polyline and push the results on the stack.
  ----------------------------------------------------------------------*/
void PolySplit2 (double a1, double b1, double a2, double b2,
		double a3, double b3, double a4, double b4,
		void *mesh)
{
#ifdef _GL
   register double      tx, ty;
   double               x1, y1, x2, y2, x3, y3, x4, y4;
   double               sx1, sy1, sx2, sy2;
   double               tx1, ty1, tx2, ty2, xmid, ymid;
  ThotPath             *path = (ThotPath *) mesh;  

   stack_deep = 0;
   PushStack (a1, b1, a2, b2, a3, b3, a4, b4);

   while (PopStack (&x1, &y1, &x2, &y2, &x3, &y3, &x4, &y4))
     {
	if ((fabs (x1 - x4) < SEG_SPLINE && fabs (y1 - y4) < SEG_SPLINE) ||
      path->nsize >= 800000)
    // no more than 500 000 points in the same polyline
	   MeshNewPoint (x1, y1, mesh);
	else
	  {
	     tx   = (double) MIDDLE_OF (x2, x3);
	     ty   = (double) MIDDLE_OF (y2, y3);
	     sx1  = (double) MIDDLE_OF (x1, x2);
	     sy1  = (double) MIDDLE_OF (y1, y2);
	     sx2  = (double) MIDDLE_OF (sx1, tx);
	     sy2  = (double) MIDDLE_OF (sy1, ty);
	     tx2  = (double) MIDDLE_OF (x3, x4);
	     ty2  = (double) MIDDLE_OF (y3, y4);
	     tx1  = (double) MIDDLE_OF (tx2, tx);
	     ty1  = (double) MIDDLE_OF (ty2, ty);
	     xmid = (double) MIDDLE_OF (sx2, tx1);
	     ymid = (double) MIDDLE_OF (sy2, ty1);

	     PushStack (xmid, ymid, tx1, ty1, tx2, ty2, x4, y4);
	     PushStack (x1, y1, sx1, sy1, sx2, sy2, xmid, ymid);
	  }
     }
#endif/*  _GL */
}

/*----------------------------------------------------------------------
  EllipticSplit2 : creates points on the given elliptic arc 
  (using endpoint parameterization)
  see http://www.w3.org/TR/SVG/implnote.html for implementations notes
  ----------------------------------------------------------------------*/
void  EllipticSplit2 (int frame, int x, int y,
		      double x1, double y1, 
		     double x2, double y2, 
		     double xradius, double yradius, 
		     double Phi, int large, int sweep, void *mesh)
{
  /* TODO: use TtaEndPointToCenterParam

  double   phi, Phicos, Phisin;
  double   cprim, Rxcos, Rysin, cX, cY;
  double   x3, y3, theta, deltatheta, thetabegin;

  if(x1 == x2 && y1 == y2)
    return;

  if (xradius == 0 || yradius == 0)
    {
      MeshNewPoint ((double) x1, (double) y1, mesh); 
      MeshNewPoint ((double) x2, (double) y2, mesh); 
      return;
    }

  if(!TtaEndPointToCenterParam(x1, y1, x2, y2,
			       &xradius, &yradius,
			       &phi,
			       large, sweep,
			       &cX, &cY,
			       &theta, &deltatheta)
     )
    return;

  Phicos = cos(phi);
  Phisin = sin(phi);

  thetabegin = theta;
  theta = 0;
  if (sweep)
    cprim = A_DEGREE;
  else
    cprim = -1 * A_DEGREE;
  deltatheta = fabs (deltatheta);
  while (fabs (theta) < deltatheta)
    {
      Rxcos = xradius * cos (thetabegin + theta);
      Rysin = yradius * sin (thetabegin + theta);
      x3 = Phicos*Rxcos - Phisin*Rysin + cX;
      y3 = Phisin*Rxcos + Phicos*Rysin + cY; 
      MeshNewPoint ((double) x3, (double) y3, mesh); 
      theta += cprim;
    }  
 */

#ifdef _GL
  double   xmid, ymid, Phicos, Phisin; 
  double   rx_p2, ry_p2, translate, xprim, yprim;
  double   cprim, cxprim, cyprim, Rxcos, Rysin, cX, cY;
  double   xtheta, ytheta, xthetaprim, ythetaprim;
  double   x3, y3, theta, deltatheta, inveangle, thetabegin;

  if (xradius == 0 || yradius == 0)
    {
      MeshNewPoint ((double) x1, (double) y1, mesh); 
      MeshNewPoint ((double) x2, (double) y2, mesh); 
      return;
    }

  xradius = (xradius<0)? fabs (xradius):xradius;
  yradius = (yradius<0)? fabs (yradius):yradius;
  
  /*local var init*/
  Phicos = cos (DEG_TO_RAD(Phi));
  Phisin = sin (DEG_TO_RAD(Phi));
  
  /* Math Recall : dot matrix multiplication => 
     V . D = (Vx * Dx) + (Vy * Dy) + (Vz * Dz) 
     and dot product =>
     (a b) (I) = aI + bS;
     (c d) (S) = cI + bS;
     and vector scalar product =>
     A.B = |A||B|cos (theta)
     (where |A| = sqrt (x_p2 + y_p2))
  */
  
  /* Step 1: Compute (x1', y1')*/
  xmid = ((x1 - x2) / 2);
  ymid = ((y1 - y2) / 2);
  xprim = Phicos*xmid + Phisin*ymid;
  yprim = -Phisin*xmid + Phicos*ymid;
  
  /* step 1bis:  verify & correct radius 
   to get at least one solution */
  rx_p2 = (double) P2 (xradius);
  ry_p2 = (double) P2 (yradius);
  translate = (double) P2 (xprim)/rx_p2 + P2 (yprim) / ry_p2;
  if ( translate > 1 )
    {
      translate = (double) sqrt (translate);
      xradius = (double) translate*xradius;
      yradius = (double) translate*yradius; 
      rx_p2 = (double) P2 (xradius);
      ry_p2 = (double) P2 (yradius);
    }

  /* Step 2: Compute (cX ', cY ') */ 
  cprim = (large ==  sweep) ? -1 : 1;
  translate = (double)( rx_p2*P2 (yprim) + ry_p2*P2 (xprim));
  if (translate == 0)
    {
      /*cannot happen... 'a priori' !!
       (according to math demonstration 
       (mainly caus'of the radius correction))*/
      return;
    }
  /*   Original formulae :
       cprim =  (double) cprim * sqrt ((rx_p2*ry_p2 - translate) / translate); 
       But double precision is no sufficent so I've made a math simplification 
       that works well */
  translate = ((rx_p2*ry_p2 / translate) - 1);
  translate = (translate > 0)?translate:-translate;
  cprim = (double) cprim * sqrt (translate);
  cxprim = cprim * ((xradius*yprim)/yradius);
  cyprim = -cprim * ((yradius*xprim)/xradius);
  
  /* Step3: Compute (cX, Cy) from (cX ', cY ') */
  xmid = ((x1 + x2) / 2);
  ymid = ((y1 + y2) / 2);
  cX = Phicos * cxprim - Phisin * cyprim + xmid;
  cY = Phisin * cxprim + Phicos * cyprim + ymid;
  
  /* Step 4: Compute theta and delta_theta */
  xtheta = (xprim - cxprim) / xradius;
  ytheta = (yprim - cyprim) / yradius;
  /*could also use hypot(x,y) = sqrt(x*x+y*Y),
   but further optimisation could be harder..*/
  inveangle = (double) (xtheta) /  (double) sqrt (P2 (xtheta) + P2 (ytheta));
  cprim = 1;
  cprim = ( ytheta < 0) ?-1 : 1;
  theta = cprim * DACOS (inveangle);
  xthetaprim = (double) (-xprim - cxprim) / xradius;
  ythetaprim = (double) (-yprim - cyprim) / yradius;
  inveangle =  (double) (xtheta*xthetaprim + ytheta*ythetaprim) /  
    (double) (sqrt (P2 (xtheta) + P2 (ytheta))* sqrt (P2 (xthetaprim) + P2 (ythetaprim)) );

  /* I add this to be sure that inveangle is in [-1; 1] - F. Wang */
  if(inveangle < -1)inveangle = -1;
  else if(inveangle > 1)inveangle = 1;

  cprim = ( xtheta*ythetaprim - ytheta*xthetaprim < 0) ? -1 : 1;
  deltatheta = fmod (cprim * DACOS (inveangle), M_PI_DOUBLE);
  if (sweep && deltatheta < 0)
    deltatheta += M_PI_DOUBLE;
  else
    if (sweep == 0 && deltatheta > 0)
      deltatheta -= M_PI_DOUBLE;
 /* Step 5: NOW that we have the center and the angles
     we can at least and at last 
     compute the points. */
  thetabegin = theta;
  translate = 0;  
  theta = 0;
  if (sweep)
    cprim = A_DEGREE;
  else
    cprim = -1 * A_DEGREE;
  deltatheta = fabs (deltatheta);
  while (fabs (theta) < deltatheta)
    {
      Rxcos = xradius * cos (thetabegin + theta);
      Rysin = yradius * sin (thetabegin + theta);
      x3 = Phicos*Rxcos - Phisin*Rysin + cX;
      y3 = Phisin*Rxcos + Phicos*Rysin + cY; 
      MeshNewPoint ((double) x3, (double) y3, mesh); 
      theta += cprim;
    }  
#endif /* _GL */
}

/*----------------------------------------------------------------------
  QuadraticSplit : split a quadratic Bezier and pushes the result on the stack.
  ----------------------------------------------------------------------*/
void QuadraticSplit2 (double a1, double b1, double a2, double b2,
		     double a3, double b3,
		     void *mesh)
{
#ifdef _GL
   register double      tx, ty;
   double               x1, y1, x2, y2, x3, y3, i, j;
   double               sx, sy;
   double               xmid, ymid;

   stack_deep = 0;
   PushStack (a1, b1, a2, b2, a3, b3, 0, 0);

   while (PopStack (&x1, &y1, &x2, &y2, &x3, &y3, &i, &j))
     {
	if (fabs (x1 - x3) < SEG_SPLINE && fabs (y1 - y3) < SEG_SPLINE)
	   MeshNewPoint (x1, y1, mesh);
	else
	  {
	     tx   = (double) MIDDLE_OF (x2, x3);
	     ty   = (double) MIDDLE_OF (y2, y3);
	     sx   = (double) MIDDLE_OF (x1, x2);
	     sy   = (double) MIDDLE_OF (y1, y2);
	     xmid = (double) MIDDLE_OF (sx, tx);
	     ymid = (double) MIDDLE_OF (sy, ty);

	     PushStack (xmid, ymid, tx, ty, x3, y3, 0, 0);
	     PushStack (x1, y1, sx, sy, xmid, ymid, 0, 0);
	  }
     }
#endif /* _GL */
}


/*----------------------------------------------------------------------
  PolySplit: split a polyline and push the results on the stack.
  ----------------------------------------------------------------------*/
void PolySplit (double a1, double b1, double a2, double b2,
		double a3, double b3, double a4, double b4,
		ThotPoint **points, int *npoints, int *maxpoints)
{
   register double      tx, ty;
   double               x1, y1, x2, y2, x3, y3, x4, y4;
   double               sx1, sy1, sx2, sy2;
   double               tx1, ty1, tx2, ty2, xmid, ymid;

   stack_deep = 0;
   PushStack (a1, b1, a2, b2, a3, b3, a4, b4);

   while (PopStack (&x1, &y1, &x2, &y2, &x3, &y3, &x4, &y4))
     {
	if (fabs (x1 - x4) < SEG_SPLINE && fabs (y1 - y4) < SEG_SPLINE)
	   PolyNewPoint (x1, y1, points, npoints,
			 maxpoints);
	else
	  {
	     tx   = (double) MIDDLE_OF (x2, x3);
	     ty   = (double) MIDDLE_OF (y2, y3);
	     sx1  = (double) MIDDLE_OF (x1, x2);
	     sy1  = (double) MIDDLE_OF (y1, y2);
	     sx2  = (double) MIDDLE_OF (sx1, tx);
	     sy2  = (double) MIDDLE_OF (sy1, ty);
	     tx2  = (double) MIDDLE_OF (x3, x4);
	     ty2  = (double) MIDDLE_OF (y3, y4);
	     tx1  = (double) MIDDLE_OF (tx2, tx);
	     ty1  = (double) MIDDLE_OF (ty2, ty);
	     xmid = (double) MIDDLE_OF (sx2, tx1);
	     ymid = (double) MIDDLE_OF (sy2, ty1);

	     PushStack (xmid, ymid, tx1, ty1, tx2, ty2, x4, y4);
	     PushStack (x1, y1, sx1, sy1, sx2, sy2, xmid, ymid);
	  }
     }
}
 
/*----------------------------------------------------------------------
  EllipticSplit : creates points on the given elliptic arc 
  (using endpoint parameterization)
  see http://www.w3.org/TR/SVG/implnote.html for implementations notes
  ----------------------------------------------------------------------*/
void  EllipticSplit (int frame, int x, int y,
		     double x1, double y1, 
		     double x2, double y2, 
		     double xradius, double yradius, 
		     int Phi, int large, int sweep, 
		     ThotPoint **points, int *npoints, int *maxpoints)
{
  /* TODO: use TtaEndPointToCenterParam */

  double xmid, ymid, 
    Phicos, Phisin, 
    rx_p2, ry_p2, 
    translate, xprim, yprim,
    cprim, cxprim, cyprim,
    Rxcos, Rysin, cX, cY,
    xtheta, ytheta, xthetaprim, ythetaprim,
    x3, y3, theta, deltatheta, inveangle,
    thetabegin;

  if (xradius == 0 || yradius == 0)
    {
      PolyNewPoint (x1, y1, points, npoints, maxpoints); 
      PolyNewPoint (x2, y2, points, npoints, maxpoints); 
      return;
    }
  xradius = (xradius<0)? fabs (xradius):xradius;
  yradius = (yradius<0)? fabs (yradius):yradius;
  
  /*local var init*/
 
  Phicos = cos (DEG_TO_RAD(Phi));
  Phisin = sin (DEG_TO_RAD(Phi));
  
  /* Math Recall : dot matrix multiplication => 
     V . D = (Vx * Dx) + (Vy * Dy) + (Vz * Dz) 
     and dot product =>
     (a b) (I) = aI + bS;
     (c d) (S) = cI + bS;
     and vector scalar product =>
     A.B = |A||B|cos (theta)
     (where |A| = sqrt (x_p2 + y_p2))
  */
  
  /* Step 1: Compute (x1', y1')*/
  xmid = ((x1 - x2) / 2);
  ymid = ((y1 - y2) / 2);
  xprim = Phicos*xmid + Phisin*ymid;
  yprim = -Phisin*xmid + Phicos*ymid;
  
  /* step 1bis:  verify & correct radius 
   to get at least one solution */
  rx_p2 = (double) P2 (xradius);
  ry_p2 = (double) P2 (yradius);
  translate = (double) P2 (xprim)/rx_p2 + P2 (yprim) / ry_p2;
  if ( translate > 1 )
    {
      translate = (double) sqrt (translate);
      xradius = (double) translate*xradius;
      yradius = (double) translate*yradius; 
      rx_p2 = (double) P2 (xradius);
      ry_p2 = (double) P2 (yradius);
    }

  /* Step 2: Compute (cX ', cY ') */ 
  cprim = (large ==  sweep) ? -1 : 1;
  translate = (double)( rx_p2*P2 (yprim) + ry_p2*P2 (xprim));
  if (translate == 0)
    {
      /*cannot happen... 'a priori' !!
       (according to math demonstration 
       (mainly caus'of the radius correction))*/
      return;
    }
  /*   Original formulae :
       cprim =  (double) cprim * sqrt ((rx_p2*ry_p2 - translate) / translate); 
       But double precision is no sufficent so I've made a math simplification 
       that works well */
  translate = ((rx_p2*ry_p2 / translate) - 1);
  translate = (translate > 0)?translate:-translate;
  cprim = (double) cprim * sqrt (translate);
  cxprim = cprim * ((xradius*yprim)/yradius);
  cyprim = -cprim * ((yradius*xprim)/xradius);
  
  /* Step3: Compute (cX, Cy) from (cX ', cY ') */
  xmid = ((x1 + x2) / 2);
  ymid = ((y1 + y2) / 2);
  cX = Phicos * cxprim - Phisin * cyprim + xmid;
  cY = Phisin * cxprim + Phicos * cyprim + ymid;
  
  /* Step 4: Compute theta and delta_theta */
  xtheta = (xprim - cxprim) / xradius;
  ytheta = (yprim - cyprim) / yradius;
  /*could also use hypot(x,y) = sqrt(x*x+y*Y),
   but further optimisation could be harder..*/
  inveangle = (double) (xtheta) /  (double) sqrt (P2 (xtheta) + P2 (ytheta));
  cprim = 1;
  cprim = ( ytheta < 0) ?-1 : 1;
  theta = cprim * DACOS (inveangle);
  xthetaprim = (double) (-xprim - cxprim) / xradius;
  ythetaprim = (double) (-yprim - cyprim) / yradius;
  inveangle =  (double) (xtheta*xthetaprim + ytheta*ythetaprim) /  
    (double) (sqrt (P2 (xtheta) + P2 (ytheta))* sqrt (P2 (xthetaprim) + P2 (ythetaprim)) );

  /* I add this to be sure that inveangle is in [-1; 1] - F. Wang */
  if(inveangle < -1)inveangle = -1;
  else if(inveangle > 1)inveangle = 1;

  cprim = ( xtheta*ythetaprim - ytheta*xthetaprim < 0) ? -1 : 1;
  deltatheta = fmod (cprim * DACOS (inveangle), M_PI_DOUBLE);
  if (sweep && deltatheta < 0)
    deltatheta += M_PI_DOUBLE;
  else
    if (sweep == 0 && deltatheta > 0)
      deltatheta -= M_PI_DOUBLE;
 /* Step 5: NOW that we have the center and the angles
     we can at least and at last 
     compute the points. */
  thetabegin = theta;
  translate = 0;  
  theta = 0;
  if (sweep)
    cprim = A_DEGREE;
  else
    cprim = -1 * A_DEGREE;
  deltatheta = fabs (deltatheta);
  while (fabs (theta) < deltatheta)
    {
      Rxcos = xradius * cos (thetabegin + theta);
      Rysin = yradius * sin (thetabegin + theta);
      x3 = Phicos*Rxcos - Phisin*Rysin + cX;
      y3 = Phisin*Rxcos + Phicos*Rysin + cY;

      if(frame > 0)
	{
	  /* Update the values according to zoom, if the frame is mentionned */
	  x3 = (double) (x + PixelValue ((int) x3, UnPixel, NULL,
			    ViewFrameTable[frame - 1].FrMagnification));
	  y3 = (double) (y + PixelValue ((int) y3, UnPixel, NULL,
			    ViewFrameTable[frame - 1].FrMagnification));
	}
      PolyNewPoint (x3, y3, points, npoints, maxpoints); 
      theta += cprim;
    }  
}
/*----------------------------------------------------------------------
  QuadraticSplit: split a quadratic Bezier and pushes the result on
  the stack.
  ----------------------------------------------------------------------*/
void QuadraticSplit (double a1, double b1, double a2, double b2,
		     double a3, double b3,
		     ThotPoint **points, int *npoints,
		     int *maxpoints)
{
   register double      tx, ty;
   double               x1, y1, x2, y2, x3, y3, i, j;
   double               sx, sy;
   double               xmid, ymid;

   stack_deep = 0;
   PushStack (a1, b1, a2, b2, a3, b3, 0, 0);

   while (PopStack (&x1, &y1, &x2, &y2, &x3, &y3, &i, &j))
     {
	if (fabs (x1 - x3) < SEG_SPLINE && fabs (y1 - y3) < SEG_SPLINE)
	   PolyNewPoint (x1, y1, points, npoints, maxpoints);
	else
	  {
	     tx   = (double) MIDDLE_OF (x2, x3);
	     ty   = (double) MIDDLE_OF (y2, y3);
	     sx   = (double) MIDDLE_OF (x1, x2);
	     sy   = (double) MIDDLE_OF (y1, y2);
	     xmid = (double) MIDDLE_OF (sx, tx);
	     ymid = (double) MIDDLE_OF (sy, ty);

	     PushStack (xmid, ymid, tx, ty, x3, y3, 0, 0);
	     PushStack (x1, y1, sx, sy, xmid, ymid, 0, 0);
	  }
     }
}
 
