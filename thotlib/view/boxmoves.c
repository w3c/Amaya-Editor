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
 * This module maintains constraints between boxes
 *
 * Author: I. Vatton (INRIA)
 *
 */

#include "ustring.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "appdialogue.h"

#define THOT_EXPORT extern
#include "boxes_tv.h"
#include "appdialogue_tv.h"

#include "boxmoves_f.h"
#include "boxrelations_f.h"
#include "buildboxes_f.h"
#include "buildlines_f.h"
#include "exceptions_f.h"
#include "font_f.h"
#include "frame_f.h"
#include "memory_f.h"
#include "windowdisplay_f.h"


/*----------------------------------------------------------------------
  IsXYPosComplete returns TRUE indicators when the box position is
  relative to the root box instead of the parent box.
  They could be FALSE when the box building is in progress and any
  enclosing box is stretchable or depends on an out-of-strucure box
  (not included by its ancestor).
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                IsXYPosComplete (PtrBox pBox, ThotBool *horizRef, ThotBool *vertRef)
#else  /* __STDC__ */
void                IsXYPosComplete (pBox, horizRef, vertRef)
PtrBox              pBox;
ThotBool           *horizRef;
ThotBool           *vertRef;

#endif /* __STDC__ */
{
  PtrBox              pParentBox;

  pParentBox = pBox;
  *vertRef = (Propagate != ToSiblings);
  *horizRef = *vertRef;
  while ((!*vertRef || !*horizRef) && pParentBox != NULL)
    {
      if (!*vertRef)
	*vertRef = (pParentBox->BxVertFlex || pParentBox->BxYOutOfStruct);
      if (!*horizRef)
	*horizRef = (pParentBox->BxHorizFlex || pParentBox->BxXOutOfStruct);
      /* Remonte a la boite englobante */
      if (pParentBox->BxAbstractBox->AbEnclosing == NULL)
	pParentBox = NULL;
      else
	pParentBox = pParentBox->BxAbstractBox->AbEnclosing->AbBox;
    }

  if (pBox->BxType == BoBlock && pBox->BxYToCompute)
    /* It's too early to compute the vertical position of lines */
    *vertRef = FALSE;
}


/*----------------------------------------------------------------------
  IsXPosComplete returns TRUE when the horizontal box position is
  relative to the root box instead of the parent box.
  It could be FALSE when the box building is in progress and any
  enclosing box is stretchable or depends on an out-of-strucure box
  (not included by its ancestor).
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool            IsXPosComplete (PtrBox pBox)
#else  /* __STDC__ */
ThotBool            IsXPosComplete (pBox)
PtrBox              pBox;
#endif /* __STDC__ */
{
  PtrBox              pParentBox;
  ThotBool            Ok;

  pParentBox = pBox;
  Ok = (Propagate != ToSiblings);
  while (!Ok && pParentBox != NULL)
    {
      Ok = (pParentBox->BxHorizFlex || pParentBox->BxXOutOfStruct);
      /* Remonte a la boite englobante */
      if (pParentBox->BxAbstractBox->AbEnclosing == NULL)
	pParentBox = NULL;
      else
	pParentBox = pParentBox->BxAbstractBox->AbEnclosing->AbBox;
    }
  return (Ok);
}


/*----------------------------------------------------------------------
  IsYPosComplete returns TRUE when the vertical box position is
  relative to the root box instead of the parent box.
  It could be FALSE when the box building is in progress and any
  enclosing box is stretchable or depends on an out-of-strucure box
  (not included by its ancestor).
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool            IsYPosComplete (PtrBox pBox)
#else  /* __STDC__ */
ThotBool            IsYPosComplete (pBox)
PtrBox              pBox;

#endif /* __STDC__ */
{
  PtrBox              pParentBox;
  ThotBool            Ok;

  pParentBox = pBox;
  Ok = (Propagate != ToSiblings);
  while (!Ok && pParentBox != NULL)
    {
      Ok = (pParentBox->BxVertFlex || pParentBox->BxYOutOfStruct);
      /* Remonte a la boite englobante */
      if (pParentBox->BxAbstractBox->AbEnclosing == NULL)
	pParentBox = NULL;
      else
	pParentBox = pParentBox->BxAbstractBox->AbEnclosing->AbBox;
    }
  return (Ok);
}


/*----------------------------------------------------------------------
   IsParentBox returns TRUE when pAncestor is an ancestor of pChild or
   pChild itself.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool            IsParentBox (PtrBox pAncestor, PtrBox pChild)
#else  /* __STDC__ */
ThotBool            IsParentBox (pAncestor, pChild)
PtrBox              pAncestor;
PtrBox              pChild;
#endif /* __STDC__ */
{
  PtrAbstractBox      pAb;
  ThotBool            equal;

  if (pChild == NULL || pAncestor == NULL)
    return (FALSE);
  else
    {
      /* Look for pChild ancestors including itself */
      pAb = pChild->BxAbstractBox;
      equal = FALSE;
      while (!equal && pAb != NULL)
	{
	  equal = pAb->AbBox == pAncestor;
	  pAb = pAb->AbEnclosing;
	}
      return (equal);
    }
}


/*----------------------------------------------------------------------
  IsSiblingBox returns TRUE if pBox is a sibling (same parent but not
  pRefBox itself) of pRefBox.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static ThotBool     IsSiblingBox (PtrBox pBox, PtrBox pRefBox)
#else  /* __STDC__ */
static ThotBool     IsSiblingBox (pBox, pRefBox)
PtrBox              pBox;
PtrBox              pRefBox;
#endif /* __STDC__ */
{
  if (pRefBox == pBox)
    return (FALSE);
  else if (pRefBox == NULL)
    return (FALSE);
  else if (pBox == NULL)
    return (FALSE);
  else if (pRefBox->BxAbstractBox->AbEnclosing == pBox->BxAbstractBox->AbEnclosing)
    return (TRUE);
  else
    return (FALSE);
}


/*----------------------------------------------------------------------
  MirrorPolyline inverts horizontally and/or vertically points in the polyline.
  The parameter inAbtractBox is TRUE when the invertion concerns the
  abstract box instead of the box.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         MirrorPolyline (PtrAbstractBox pAb, ThotBool horizRef, ThotBool vertRef, ThotBool inAbtractBox)
#else  /* __STDC__ */
static void         MirrorPolyline (pAb, horizRef, vertRef, inAbtractBox)
PtrAbstractBox      pAb;
ThotBool            horizRef;
ThotBool            vertRef;
ThotBool            inAbtractBox;
#endif /* __STDC__ */
{
  PtrTextBuffer       pSourceBuff, pTargetBuff;
  PtrBox              pBox;
  int                 xMax, yMax;
  int                 val, i, j;
  float               xRatio, yRatio;
  int                 pointIndex;

  pBox = pAb->AbBox;
  j = 1;
  if (inAbtractBox)
    {
      /* maj des reperes du pave a partir de la boite */
      pSourceBuff = pBox->BxBuffer;
      pTargetBuff = pAb->AbPolyLineBuffer;
      xRatio = pBox->BxXRatio;
      yRatio = pBox->BxYRatio;
    }
  else
    {
      /* maj des reperes de la boite a partir du pave */
      pSourceBuff = pAb->AbPolyLineBuffer;
      pTargetBuff = pBox->BxBuffer;
      /* annule les deformations precedentes */
      xRatio = 1;
      yRatio = 1;
      pBox->BxXRatio = 1;
      pBox->BxYRatio = 1;
      pTargetBuff->BuPoints[0].XCoord = pSourceBuff->BuPoints[0].XCoord;
      pTargetBuff->BuPoints[0].YCoord = pSourceBuff->BuPoints[0].YCoord;
    }

  /* inversions eventuelles par rapport aux milieux */
  xMax = pSourceBuff->BuPoints[0].XCoord;
  yMax = pSourceBuff->BuPoints[0].YCoord;
  val = pBox->BxNChars;
  for (i = 1; i < val; i++)
    {
      if (j >= pSourceBuff->BuLength)
	{
	  if (pSourceBuff->BuNext != NULL)
	    {
	      /* Changement de buffer */
	      pSourceBuff = pSourceBuff->BuNext;
	      pTargetBuff = pTargetBuff->BuNext;
	      j = 0;
	    }
	}
      
      pointIndex = (int) ((float) pSourceBuff->BuPoints[j].XCoord / xRatio);
      if (horizRef)
	/* inversion des points en x */
	pTargetBuff->BuPoints[j].XCoord = xMax - pointIndex;
      else
	pTargetBuff->BuPoints[j].XCoord = pointIndex;
      
      pointIndex = (int) ((float) pSourceBuff->BuPoints[j].YCoord / yRatio);
      if (vertRef)
	/* inversion des points en y */
	pTargetBuff->BuPoints[j].YCoord = yMax - pointIndex;
      else
	pTargetBuff->BuPoints[j].YCoord = pointIndex;
      
      j++;
     }
  
  if (pBox->BxPictInfo != NULL && !inAbtractBox)
    {
      /* le calcul des points de controle doit etre reexecute */
      free ((STRING) pBox->BxPictInfo);
      pBox->BxPictInfo = NULL;
    }
}


/*----------------------------------------------------------------------
  MirrorShape adapts the drawing to current inversion of box edges.
  If the abstract box has children the request is tranmited to them.
  Parameters horizRef and vertRef are TRUE when respectivelly horizontal
  and vertical edges are inverted.
  The parameter inAbtractBox is TRUE when the invertion concerns the
  abstract box instead of the box.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                MirrorShape (PtrAbstractBox pAb, ThotBool horizRef, ThotBool vertRef, ThotBool inAbtractBox)
#else  /* __STDC__ */
void                MirrorShape (pAb, horizRef, vertRef, inAbtractBox)
PtrAbstractBox      pAb;
ThotBool            horizRef;
ThotBool            vertRef;
ThotBool            inAbtractBox;

#endif /* __STDC__ */
{
  PtrAbstractBox      pChildAb;
  char                shape;

  if (pAb->AbLeafType == LtCompound && !inAbtractBox)
    {
      /* Transmet le traitement aux paves fils */
      pChildAb = pAb->AbFirstEnclosed;
      while (pChildAb != NULL)
	{
	  if (pChildAb->AbLeafType == LtGraphics
	      || pChildAb->AbLeafType == LtPolyLine
	      || pChildAb->AbLeafType == LtCompound)
	    MirrorShape (pChildAb, horizRef, vertRef, inAbtractBox);
	  pChildAb = pChildAb->AbNext;
	}
    }
  else if (pAb->AbLeafType == LtPolyLine)
    MirrorPolyline (pAb, horizRef, vertRef, inAbtractBox);
  else if (pAb->AbLeafType == LtGraphics)
    {
      /* Ajuste le graphique de la boite */
      if (inAbtractBox)
	/* le caractere de reference est le trace reel */
	shape = pAb->AbRealShape;
      else
	/* le caractere de reference est le trace de l'element */
	shape = pAb->AbShape;
      
      if (horizRef)
	switch (shape)
	  {
	  case '\\':
	    shape = '/';
	    break;
	  case '/':
	    shape = '\\';
	    break;
	  case '>':
	    shape = '<';
	    break;
	  case '<':
	    shape = '>';
	    break;
	  case 'l':
	    shape = 'r';
	    break;
	  case 'r':
	    shape = 'l';
	    break;
	  case 'o':
	    shape = 'e';
	    break;
	  case 'e':
	    shape = 'o';
	    break;
	  case 'O':
	    shape = 'E';
	    break;
	  case 'E':
	    shape = 'O';
	    break;
	  case 'W':
	    shape = 'Z';
	    break;
	  case 'Z':
	    shape = 'W';
	    break;
	  case 'X':
	    shape = 'Y';
	    break;
	  case 'Y':
	    shape = 'X';
	    break;
	  default:
	    break;
	  }
      
      if (vertRef)
	switch (shape)
	  {
	  case '\\':
	    shape = '/';
	    break;
	  case '/':
	    shape = '\\';
	    break;
	  case '^':
	    shape = 'V';
	    break;
	  case 'V':
	    shape = '^';
	    break;
	  case 't':
	    shape = 'b';
	    break;
	  case 'b':
	    shape = 't';
	    break;
	  case 'o':
	    shape = 'O';
	    break;
	  case 'O':
	    shape = 'o';
	    break;
	  case 'e':
	    shape = 'E';
	    break;
	  case 'E':
	    shape = 'e';
	    break;
	  case 'W':
	    shape = 'X';
	    break;
	  case 'Z':
	    shape = 'Y';
	    break;
	  case 'X':
	    shape = 'W';
	    break;
	  case 'Y':
	    shape = 'Z';
	    break;
	  default:
	    break;
	  }

      /* Ajuste le graphique de la boite */
      if (inAbtractBox)
	/* le caractere de reference est le trace reel */
	pAb->AbShape = shape;
      else
	/* le caractere de reference est le trace de l'element */
	pAb->AbRealShape = shape;
    }
}


/*----------------------------------------------------------------------
  XEdgesExchange inverts horizontal edges and the width of the box.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                XEdgesExchange (PtrBox pBox, OpRelation op)
#else  /* __STDC__ */
void                XEdgesExchange (pBox, op)
PtrBox              pBox;
OpRelation          op;

#endif /* __STDC__ */
{
   PtrAbstractBox      pAb;
   PtrPosRelations     pPosRel;
   int                 i;
   BoxEdge             oldPosEdge;
   BoxEdge             newPosEdge;
   BoxEdge             oldDimEdge;
   BoxEdge             newDimEdge;
   PtrBox              pRefBox;

   pAb = pBox->BxAbstractBox;

   /* prend le repere symetrique dans le positionnement */
   /* Ancien repere de position */
   oldPosEdge = pAb->AbHorizPos.PosEdge;
   /* Nouveau repere de position */
   newPosEdge = oldPosEdge;
   /* Ancien repere de dimension */
   oldDimEdge = oldPosEdge;
   /* Nouveau repere de dimension */
   newDimEdge = oldPosEdge;
   if (oldPosEdge == Left)
     {
	newPosEdge = Right;
	pAb->AbHorizPos.PosEdge = newPosEdge;
	oldDimEdge = Right;
	newDimEdge = Left;
	pAb->AbWidth.DimPosition.PosEdge = newDimEdge;
     }
   else if (oldPosEdge == Right)
     {
	newPosEdge = Left;
	pAb->AbHorizPos.PosEdge = newPosEdge;
	oldDimEdge = Left;
	newDimEdge = Right;
	pAb->AbWidth.DimPosition.PosEdge = newDimEdge;
     }

   /* Mise a jour du point fixe de la boite */
   if (op == OpWidth && pBox->BxHorizEdge == oldPosEdge)
      pBox->BxHorizEdge = newPosEdge;
   else if (op == OpHorizDep && pBox->BxHorizEdge == oldDimEdge)
      pBox->BxHorizEdge = newDimEdge;

   /* Les reperes sont inverses */
   pBox->BxHorizInverted = !pBox->BxHorizInverted;

   /* Modifie enventuellement le dessin de la boite */
   if (oldPosEdge != newPosEdge)
      MirrorShape (pAb, pBox->BxHorizInverted, pBox->BxVertInverted, FALSE);

   /* Note la boite qui fixe la position de la boite elastique */
   if (pAb->AbHorizPos.PosAbRef == NULL)
      pRefBox = NULL;
   else
      pRefBox = pAb->AbHorizPos.PosAbRef->AbBox;

   /* Met a jour les relations de la boite */
   i = 0;
   pPosRel = pBox->BxPosRelations;
   while (pPosRel != NULL)
     {
	if (pPosRel->PosRTable[i].ReBox == NULL)
	  {
	     pPosRel = pPosRel->PosRNext;
	     i = -1;
	  }
	/* Est-ce une relation avec le repere de position ? */
	else if (pPosRel->PosRTable[i].ReRefEdge == oldPosEdge
		 && pPosRel->PosRTable[i].ReBox == pRefBox
		 && pPosRel->PosRTable[i].ReOp != OpHorizRef)
	   pPosRel->PosRTable[i].ReRefEdge = newPosEdge;
	i++;
     }
}


/*----------------------------------------------------------------------
  YEdgesExchange inverts vertical edges and the height of the box.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                YEdgesExchange (PtrBox pBox, OpRelation op)
#else  /* __STDC__ */
void                YEdgesExchange (pBox, op)
PtrBox              pBox;
OpRelation          op;

#endif /* __STDC__ */
{
   PtrAbstractBox      pAb;
   PtrPosRelations     pPosRel;
   int                 i;
   BoxEdge             oldPosEdge;
   BoxEdge             newPosEdge;
   BoxEdge             oldDimEdge;
   BoxEdge             newDimEdge;
   PtrBox              pRefBox;

   pAb = pBox->BxAbstractBox;

   /* prend le repere symetrique dans le positionnement */
   oldPosEdge = pAb->AbVertPos.PosEdge;
   /* Nouveau repere de position */
   newPosEdge = oldPosEdge;
   /* Ancien repere de dimension */
   oldDimEdge = oldPosEdge;
   /* Nouveau repere de dimension */
   newDimEdge = oldPosEdge;
   if (oldPosEdge == Top)
     {
	newPosEdge = Bottom;
	pAb->AbVertPos.PosEdge = newPosEdge;
	oldDimEdge = Bottom;
	newDimEdge = Top;
	pAb->AbHeight.DimPosition.PosEdge = newDimEdge;
     }
   else if (oldPosEdge == Bottom)
     {
	newPosEdge = Top;
	pAb->AbVertPos.PosEdge = newPosEdge;
	oldDimEdge = Top;
	newDimEdge = Bottom;
	pAb->AbHeight.DimPosition.PosEdge = newDimEdge;
     }

   /* Mise a jour du point fixe de la boite */
   if (op == OpHeight && pBox->BxVertEdge == oldPosEdge)
      pBox->BxVertEdge = newPosEdge;
   else if (op == OpVertDep && pBox->BxVertEdge == oldDimEdge)
      pBox->BxVertEdge = newDimEdge;

   /* Les reperes sont inverses */
   pBox->BxVertInverted = !pBox->BxVertInverted;

   /* Modifie enventuellement le dessin de la boite */
   if (oldPosEdge != newPosEdge)
      MirrorShape (pAb, pBox->BxHorizInverted, pBox->BxVertInverted, FALSE);

   /* Note la boite qui fixe la position de la boite elastique */
   if (pAb->AbVertPos.PosAbRef == NULL)
      pRefBox = NULL;
   else
      pRefBox = pAb->AbVertPos.PosAbRef->AbBox;

   /* Met a jour les relations de la boite */
   i = 0;
   pPosRel = pBox->BxPosRelations;
   while (pPosRel != NULL)
     {
	if (pPosRel->PosRTable[i].ReBox == NULL)
	  {
	     pPosRel = pPosRel->PosRNext;
	     i = -1;
	  }
	/* Est-ce une relation avec le repere de position ? */
	else if (pPosRel->PosRTable[i].ReRefEdge == oldPosEdge
		 && pPosRel->PosRTable[i].ReBox == pRefBox
		 && pPosRel->PosRTable[i].ReOp != OpVertRef)
	   pPosRel->PosRTable[i].ReRefEdge = newPosEdge;
	i++;
     }
}


/*----------------------------------------------------------------------
  ChangeDefaultHeight updates the contents box height.
  Check if the rule of the mininmum is respected and eventually exchange
  the real and the constrainted height.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ChangeDefaultHeight (PtrBox pBox, PtrBox pSourceBox, int height, int frame)
#else  /* __STDC__ */
void                ChangeDefaultHeight (pBox, pSourceBox, height, frame)
PtrBox              pBox;
PtrBox              pSourceBox;
int                 height;
int                 frame;
#endif /* __STDC__ */
{
   int                 delta;

   if (pBox != NULL)
     {
	/* Regarde si la hauteur reelle actuelle depend du contenu */
	if (pBox->BxContentHeight)
	  {
	     /* La hauteur reelle est egale a la hauteur du contenu */
	     if (height < pBox->BxRuleHeigth)
	       {
		  /* Il faut echanger la hauteur reelle avec l'autre hauteur */
		  delta = pBox->BxRuleHeigth - pBox->BxH;
		  pBox->BxRuleHeigth = height;
		  pBox->BxContentHeight = !pBox->BxContentHeight;
		  ResizeHeight (pBox, pSourceBox, NULL, delta, 0, 0, frame);
	       }
	     else
		/* Mise a jour de la hauteur du contenu */
		ResizeHeight (pBox, pSourceBox, NULL, height - pBox->BxH, 0, 0, frame);
	  }
	else if (!pBox->BxAbstractBox->AbHeight.DimIsPosition && pBox->BxAbstractBox->AbHeight.DimMinimum)
	  {
	     /* La hauteur reelle est egale au minimum */
	     if (height > pBox->BxH)
	       {
		  /* Il faut echanger la hauteur reelle avec l'autre hauteur */
		  pBox->BxRuleHeigth = pBox->BxH;
		  pBox->BxContentHeight = !pBox->BxContentHeight;
		  ResizeHeight (pBox, pSourceBox, NULL, height - pBox->BxH, 0, 0, frame);
	       }
	     else
		/* Mise a jour de la hauteur du contenu */
		pBox->BxRuleHeigth = height;
	  }
     }
}


/*----------------------------------------------------------------------
  ChangeDefaultWidth updates the contents box width.
  Check if the rule of the mininmum is respected and eventually exchange
  the real and the constrainted width.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ChangeDefaultWidth (PtrBox pBox, PtrBox pSourceBox, int width, int spaceDelta, int frame)
#else  /* __STDC__ */
void                ChangeDefaultWidth (pBox, pSourceBox, width, spaceDelta, frame)
PtrBox              pBox;
PtrBox              pSourceBox;
int                 width;
int                 spaceDelta;
int                 frame;

#endif /* __STDC__ */
{
   int                 delta;

   if (pBox != NULL)
     {
	/* Regarde si la largeur reelle actuelle depend du contenu */
	if (pBox->BxContentWidth)
	  {
	     /* La largeur reelle est egale a la largeur du contenu */
	     if (width < pBox->BxRuleWidth)
	       {
		  /* Il faut echanger la largeur reelle avec l'autre largeur */
		  delta = pBox->BxRuleWidth - pBox->BxW;
		  pBox->BxRuleWidth = width;
		  pBox->BxContentWidth = !pBox->BxContentWidth;
		  ResizeWidth (pBox, pSourceBox, NULL, delta, spaceDelta, 0, 0, frame);
	       }
	     else
		/* Mise a jour de la largeur du contenu */
		ResizeWidth (pBox, pSourceBox, NULL, width - pBox->BxW, 0, 0, spaceDelta, frame);
	  }
	else if (!pBox->BxAbstractBox->AbWidth.DimIsPosition && pBox->BxAbstractBox->AbWidth.DimMinimum)
	  {
	     /* La largeur reelle est egale au minimum */
	     if (width > pBox->BxW)
	       {
		  /* Il faut echanger la largeur reelle avec l'autre largeur */
		  pBox->BxRuleWidth = pBox->BxW;
		  pBox->BxContentWidth = !pBox->BxContentWidth;
		  ResizeWidth (pBox, pSourceBox, NULL, width - pBox->BxW, 0, 0, spaceDelta, frame);
	       }
	     else
		/* Mise a jour de la largeur du contenu */
		pBox->BxRuleWidth = width;
	  }
     }
}


/*----------------------------------------------------------------------
   ChangeWidth updates the constrainted box width.
  Check if the rule of the mininmum is respected and eventually exchange
  the real and the constrainted width.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ChangeWidth (PtrBox pBox, PtrBox pSourceBox, PtrBox pFromBox, int delta, int spaceDelta, int frame)
#else  /* __STDC__ */
void                ChangeWidth (pBox, pSourceBox, pFromBox, delta, spaceDelta, frame)
PtrBox              pBox;
PtrBox              pSourceBox;
PtrBox              pFromBox;
int                 delta;
int                 spaceDelta;
int                 frame;
#endif /* __STDC__ */
{
  int                 width;
  ThotBool            minimumRule;


  if (pBox != NULL)
    {
      minimumRule = (!pBox->BxAbstractBox->AbWidth.DimIsPosition
		     && pBox->BxAbstractBox->AbWidth.DimMinimum);

      if (minimumRule && IsParentBox (pFromBox, pBox)
	  && pFromBox->BxContentWidth)
	/* If the minimum size depends on an enclosing box and the current
	 * size of this box depends on the content size,
	 * don't change the value
	 */
	return;

      /* Regarde si la largeur reelle actuelle depend du contenu */
      if (pBox->BxContentWidth)
	{
	  /* Il y a une regle de minimum mais ce n'est la largeur reelle */
	  width = pBox->BxRuleWidth + delta;
	  if (width > pBox->BxW)
	    {
	      /* Il faut echanger la largeur reelle avec l'autre largeur */
	      pBox->BxRuleWidth = pBox->BxW;
	      pBox->BxContentWidth = !pBox->BxContentWidth;
	      ResizeWidth (pBox, pSourceBox, pFromBox, width - pBox->BxW, 0, 0, spaceDelta, frame);
	    }
	  else
	    /* Mise a jour de la largeur minimum */
	    pBox->BxRuleWidth = width;
	}
      else if (minimumRule)
	{
	  /* La largeur reelle est egale au minimum */
	  width = pBox->BxW + delta;
	  if (width < pBox->BxRuleWidth)
	    {
	      /* Il faut echanger la largeur reelle avec l'autre largeur */
	      width = pBox->BxRuleWidth;
	      pBox->BxRuleWidth = pBox->BxW + delta;
	      pBox->BxContentWidth = !pBox->BxContentWidth;
	      ResizeWidth (pBox, pSourceBox, pFromBox, width - pBox->BxW, 0, 0, spaceDelta, frame);
	    }
	  else
	    /* Mise a jour de la largeur minimum */
	    ResizeWidth (pBox, pSourceBox, pFromBox, delta, 0, 0, spaceDelta, frame);
	}
      else
	ResizeWidth (pBox, pSourceBox, pFromBox, delta, 0, 0, spaceDelta, frame);
    }
}


/*----------------------------------------------------------------------
  ChangeHeight updates the constrainted box height.
  Check if the rule of the mininmum is respected and eventually exchange
  the real and the constrainted height.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ChangeHeight (PtrBox pBox, PtrBox pSourceBox, PtrBox pFromBox, int delta, int frame)
#else  /* __STDC__ */
void                ChangeHeight (pBox, pSourceBox, pFromBox, delta, frame)
PtrBox              pBox;
PtrBox              pSourceBox;
PtrBox              pFromBox;
int                 delta;
int                 frame;
#endif /* __STDC__ */
{
  int                 height;
  ThotBool            minimumRule;

  if (pBox != NULL)
    {
      minimumRule = (!pBox->BxAbstractBox->AbHeight.DimIsPosition
		     && pBox->BxAbstractBox->AbHeight.DimMinimum);

      if (minimumRule && IsParentBox (pFromBox, pBox)
	  && pFromBox->BxContentHeight)
	/* If the minimum size depends on an enclosing box and the current
	 * size of this box depends on the content size,
	 * don't change the value
	 */
	return;

      /* Regarde si la hauteur reelle actuelle depend du contenu */
      if (pBox->BxContentHeight)
	{
	  /* Il y a une regle de minimum mais ce n'est la hauteur reelle */
	  height = pBox->BxRuleHeigth + delta;
	  if (height > pBox->BxH)
	    {
	      /* Il faut echanger la hauteur reelle avec l'autre hauteur */
	      pBox->BxRuleHeigth = pBox->BxH;
	      pBox->BxContentHeight = !pBox->BxContentHeight;
	      ResizeHeight (pBox, pSourceBox, pFromBox, height - pBox->BxH, 0, 0, frame);
	    }
	  else
	    /* Mise a jour de la hauteur minimum */
	    pBox->BxRuleHeigth = height;
	}
      else if (minimumRule)
	{
	  /* La hauteur courante est egale au minimum */
	  height = pBox->BxH + delta;
	  if (height < pBox->BxRuleHeigth)
	    {
	      /* Il faut echanger la hauteur reelle avec l'autre hauteur */
	      height = pBox->BxRuleHeigth;
	      pBox->BxRuleHeigth = pBox->BxH + delta;
	      pBox->BxContentHeight = !pBox->BxContentHeight;
	      ResizeHeight (pBox, pSourceBox, pFromBox, height - pBox->BxH, 0, 0, frame);
	    }
	  else
	    ResizeHeight (pBox, pSourceBox, pFromBox, delta, 0, 0, frame);
	}
      else
	ResizeHeight (pBox, pSourceBox, pFromBox, delta, 0, 0, frame);
    }
}


/*----------------------------------------------------------------------
  MoveBoxEdge moves the edge of a stretchable box.
  Invert box edges and the drawing of the box when a dimension becomes
  negative.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                MoveBoxEdge (PtrBox pBox, PtrBox pSourceBox, OpRelation op, int delta, int frame, ThotBool horizRef)
#else  /* __STDC__ */
void                MoveBoxEdge (pBox, pSourceBox, op, delta, frame, horizRef)
PtrBox              pBox;
PtrBox              pSourceBox;
OpRelation          op;
int                 delta;
int                 frame;
ThotBool            horizRef;
#endif /* __STDC__ */
{
  PtrAbstractBox      pAb;
  BoxEdge             oldPosEdge;
  int                 translation;

  pAb = pBox->BxAbstractBox;
  translation = 0;
  /* Avoid to perform two times the same job */
  if (pAb != NULL && delta != 0 && pBox->BxPacking == 0)
    {
      /* register that we're preforming the job */
      pBox->BxPacking = 1;
      if (horizRef)
	{
	  /* Look for the horizontal fixed edge and the horizontal free edge */
	  if (op == OpWidth)
	    oldPosEdge = pAb->AbWidth.DimPosition.PosEdge;
	  else
	    {
	      oldPosEdge = pAb->AbHorizPos.PosEdge;
	      /* Update the box history to avoid two moving of pSourceBox */
	      if (pSourceBox == NULL)
		pBox->BxMoved = pSourceBox;
	      else if (pSourceBox->BxMoved != pBox)
		pBox->BxMoved = pSourceBox;
	    }

	  /* compute changes and temporally change the fixed edge */
	  if (oldPosEdge == Left)
	    {
	      delta = -delta;
	      pBox->BxHorizEdge = Right;
	    }
	  else if (oldPosEdge == Right)
	    pBox->BxHorizEdge = Left;
	  else if (oldPosEdge == VertMiddle)
	    {
	      delta = delta * 2;
	      if (op == OpHorizDep)
		pBox->BxHorizEdge = pAb->AbWidth.DimPosition.PosEdge;
	      else
		pBox->BxHorizEdge = pAb->AbHorizPos.PosEdge;
	    }

	  if (delta < 0 && -delta > pBox->BxWidth)
	    {
	      /* Invert box edges */
	      XEdgesExchange (pBox, op);
	      
	      /* Translate the box origin */
	      delta = -delta - 2 * pBox->BxWidth;
	      if (pBox->BxHorizEdge == Right)
		translation = -pBox->BxWidth;
	      else if (pBox->BxHorizEdge == Left)
		translation = pBox->BxWidth;
	      XMove (pBox, pSourceBox, translation, frame);
	    }
	  /* Resize the box */
	  delta = delta + pBox->BxWidth - pBox->BxW - pBox->BxLMargin - pBox->BxRMargin - pBox->BxLPadding - pBox->BxRPadding - pBox->BxLBorder - pBox->BxRBorder;
	  ResizeWidth (pBox, pSourceBox, NULL, delta, 0, 0, 0, frame);
	  /* restore the fixed edge */
	  pBox->BxHorizEdge = NoEdge;
	}
      else
	{
	  /* Look for the vertical fixed edge and the vertical free edge */
	  if (op == OpHeight)
	    oldPosEdge = pAb->AbHeight.DimPosition.PosEdge;
	  else
	    {
	      oldPosEdge = pAb->AbVertPos.PosEdge;
	      /* Update the box history to avoid two moving of pSourceBox */
	      if (pSourceBox == NULL)
		pBox->BxMoved = pSourceBox;
	      else if (pSourceBox->BxMoved != pBox)
		pBox->BxMoved = pSourceBox;
	    }
	  
	  /* compute changes and temporally change the fixed edge */
	  if (oldPosEdge == Top)
	    {
	      delta = -delta;
	      pBox->BxVertEdge = Bottom;
	    }
	  else if (oldPosEdge == Bottom)
	    pBox->BxVertEdge = Top;
	  else if (oldPosEdge == HorizMiddle)
	    {
	      delta = delta * 2;
	      if (op == OpVertDep)
		pBox->BxVertEdge = pAb->AbHeight.DimPosition.PosEdge;
	      else
		pBox->BxVertEdge = pAb->AbVertPos.PosEdge;
	    }
	  
	  if (delta < 0 && -delta > pBox->BxH)
	    {
	      /* Invert box edges */
	      YEdgesExchange (pBox, op);
	      
	      /* Translate the box origin */
	      delta = -delta - 2 * pBox->BxH;
	      if (pBox->BxVertEdge == Bottom)
		translation = -pBox->BxH;
	      else if (pBox->BxVertEdge == Top)
		translation = pBox->BxH;
	      YMove (pBox, pSourceBox, translation, frame);
	      
	    }
	  /* Resize the box */
	  ResizeHeight (pBox, pSourceBox, NULL, delta, 0, 0, frame);
	  /* restore the fixed edge */
	  pBox->BxVertEdge = NoEdge;
	}

      /* the job is performed now */
      pBox->BxPacking = 0;
      /* restore the box history */
      if (pSourceBox == NULL)
	pBox->BxMoved = pSourceBox;
      else if (pSourceBox->BxMoved != pBox)
	pBox->BxMoved = pSourceBox;
    }
}


/*----------------------------------------------------------------------
  XMoveAllEnclosed moves horizontally all enclosed boxes.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                XMoveAllEnclosed (PtrBox pBox, int delta, int frame)
#else  /* __STDC__ */
void                XMoveAllEnclosed (pBox, delta, frame)
PtrBox              pBox;
int                 delta;
int                 frame;

#endif /* __STDC__ */
{
   PtrAbstractBox      pChildAb;
   PtrBox              pChildBox;
   PtrBox              pParentBox;
   BoxRelation        *pRelation;
   PtrPosRelations     pPosRel;
   int                 i;
   ThotBool            notEmpty;
   ThotBool            toHorizPack;

   if (pBox != NULL && (delta != 0 || pBox->BxXToCompute))
     {
	/* enregistre la hierarchie des boites dont le */
	/* traitement de l'englobement doit etre differe  */
	pParentBox = PackBoxRoot;
	if (pBox->BxAbstractBox->AbEnclosing != NULL)
	   PackBoxRoot = pBox /*->BxAbstractBox->AbEnclosing->AbBox*/ ;

	/* Si PackBoxRoot est une boite mere de la boite   */
	/* pFromBoxedemment designee on garde l'ancienne boite */
	if (IsParentBox (PackBoxRoot, pParentBox))
	   PackBoxRoot = pParentBox;

	/* Si la boite est mise en lignes */
	if (pBox->BxType == BoSplit)
	  {
	     /* decale les boites de coupure */
	     pChildBox = pBox->BxNexChild;
	     while (pChildBox != NULL)
	       {
		  pChildBox->BxXOrg += delta;
		  pChildBox = pChildBox->BxNexChild;
	       }
	  }

	/* analyse les relations pour savoir comment deplacer la boite */
	else if (pBox->BxAbstractBox != NULL)
	  {
	     /* La boite est elastique et n'est pas en cours de traitement */
	     if (pBox->BxHorizFlex && (!pBox->BxAbstractBox->AbLeafType == LtCompound || pBox->BxPacking == 0))
		 MoveBoxEdge (pBox, NULL, OpHorizDep, delta, frame, TRUE);
	     /* Dans les autres cas */
	     else
	       {
		  /* simple translation */
		  pBox->BxXOrg += delta;

		  /* Faut-il mettre a jour le rectangle de reaffichage ? */
		  if (!pBox->BxAbstractBox->AbHorizEnclosing && ReadyToDisplay)
		    {
		       /* Prend en compte une zone de debordement des graphiques */
		       if (pBox->BxAbstractBox->AbLeafType == LtGraphics)
			  i = EXTRA_GRAPH;
		       else
			  i = 0;
		       if (delta > 0)
			  DefClip (frame, pBox->BxXOrg - delta - i, pBox->BxYOrg - i,
				   pBox->BxXOrg + pBox->BxWidth + i, pBox->BxYOrg + pBox->BxHeight + i);
		       else
			  DefClip (frame, pBox->BxXOrg - i, pBox->BxYOrg - i,
				   pBox->BxXOrg + pBox->BxWidth - delta + i, pBox->BxYOrg + pBox->BxHeight + i);
		    }

		  /* decale les boites qui ont des relations hors-structure avec */
		  /* la boite deplacee et on met a jour les dimensions elastiques   */
		  /* des boites liees a la boite deplacee.                          */
		  pPosRel = pBox->BxPosRelations;
		  while (pPosRel != NULL)
		    {
		       i = 0;
		       notEmpty = pPosRel->PosRTable[i].ReBox != NULL;
		       while (i < MAX_RELAT_POS && notEmpty)
			 {
			    pRelation = &pPosRel->PosRTable[i];
			    if (pRelation->ReBox->BxAbstractBox != NULL)
			      {
				 /* Relation hors-struture sur l'origine de la boite */
				 if (pRelation->ReOp == OpHorizDep && pRelation->ReBox->BxXOutOfStruct
				     && pRelation->ReBox->BxAbstractBox->AbHorizPos.PosAbRef == pBox->BxAbstractBox)
				   {

				      /* refuse de deplacer une boite englobante de pBox */
				      if (IsParentBox (pRelation->ReBox, pBox)) ;

				      else if (pRelation->ReBox->BxHorizFlex)
					 MoveBoxEdge (pRelation->ReBox, pBox, pRelation->ReOp, delta, frame, TRUE);
				      else
					 XMove (pRelation->ReBox, pBox, delta, frame);

				   }
				 /* Relation sur la largeur elastique de la boite */
				 else if (pRelation->ReOp == OpWidth)
				    MoveBoxEdge (pRelation->ReBox, pBox, pRelation->ReOp, delta, frame, TRUE);
			      }

			    i++;
			    if (i < MAX_RELAT_POS)
			       notEmpty = (pPosRel->PosRTable[i].ReBox != NULL);
			 }
		       /* next block */
		       pPosRel = pPosRel->PosRNext;
		    }

		  /* Decale des boites englobees dont l'origine depend de l'englobante */
		  pChildAb = pBox->BxAbstractBox->AbFirstEnclosed;
		  /* Traite le niveau inferieur */
		  toHorizPack = FALSE;
		  /* Si la boite est en cours de deplacement -> il faut transmettre */
		  /* la valeur de l'origine plutot que la valeur du decalage        */
		  if (pBox->BxXToCompute)
		     delta = pBox->BxXOrg;

		  while (pChildAb != NULL)
		    {
		       if (pChildAb->AbBox != NULL)
			  if (pChildAb->AbBox->BxXOutOfStruct)
			     toHorizPack = TRUE;
			  else
			    {
			       if (pBox->BxXToCompute && !pChildAb->AbBox->BxHorizFlex)
				  /* Additionne le decalage de la boite */
				  pChildAb->AbBox->BxXToCompute = TRUE;
			       XMoveAllEnclosed (pChildAb->AbBox, delta, frame);
			    }
		       pChildAb = pChildAb->AbNext;
		    }
		  pBox->BxXToCompute = FALSE;	/* le decalage eventuel est pris en compte */

		  /* Si la largeur de la boite depend du contenu et qu'une des     */
		  /* boites filles est positionnee par une relation hors-structure */
		  /* --> il faut reevaluer la largeur correspondante.              */
		  if (toHorizPack &&
		      (pBox->BxContentWidth || (!pBox->BxAbstractBox->AbWidth.DimIsPosition && pBox->BxAbstractBox->AbWidth.DimMinimum)))
		     RecordEnclosing (pBox, TRUE);
	       }
	  }
	/* restaure */
	PackBoxRoot = pParentBox;
     }
}


/*----------------------------------------------------------------------
  YMoveAllEnclosed moves vertically all enclosed boxes.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                YMoveAllEnclosed (PtrBox pBox, int delta, int frame)
#else  /* __STDC__ */
void                YMoveAllEnclosed (pBox, delta, frame)
PtrBox              pBox;
int                 delta;
int                 frame;

#endif /* __STDC__ */
{
   PtrAbstractBox      pChildAb;
   PtrBox              pChildBox;
   PtrBox              pParentBox;
   BoxRelation        *pRelation;
   PtrPosRelations     pPosRel;
   int                 i;
   ThotBool            notEmpty;
   ThotBool            toVertPack;

   if (pBox != NULL && (delta != 0 || pBox->BxYToCompute))
     {
	/* enregistre la hierarchie des boites dont le */
	/* traitement de l'englobement doit etre differe  */
	pParentBox = PackBoxRoot;
	if (pBox->BxAbstractBox->AbEnclosing != NULL)
	   PackBoxRoot = pBox /*->BxAbstractBox->AbEnclosing->AbBox*/ ;

	/* Si PackBoxRoot est une boite mere de la boite   */
	/* pFromBoxedemment designee on garde l'ancienne boite */
	if (IsParentBox (PackBoxRoot, pParentBox))
	   PackBoxRoot = pParentBox;

	/* Si la boite est mise en lignes */
	if (pBox->BxType == BoSplit)
	  {
	     /* decale les boites de coupure */
	     pChildBox = pBox->BxNexChild;
	     while (pChildBox != NULL)
	       {
		  pChildBox->BxYOrg += delta;
		  pChildBox = pChildBox->BxNexChild;
	       }

	  }

	/* analyse les relations pour savoir comment deplacer la boite */
	else if (pBox->BxAbstractBox != NULL)
	  {
	    /*-> La boite est elastique et n'est pas en cours de traitement */
	     if (pBox->BxVertFlex && (!pBox->BxAbstractBox->AbLeafType == LtCompound || pBox->BxPacking == 0))
		 MoveBoxEdge (pBox, NULL, OpVertDep, delta, frame, FALSE);
	     /*-> Dans les autres cas */
	     else
	       {
		  /* simple translation */
		  pBox->BxYOrg += delta;

		  /* Faut-il mettre a jour le rectangle de reaffichage ? */
		  if (!pBox->BxAbstractBox->AbVertEnclosing && ReadyToDisplay)
		    {
		       /* Prend en compte une zone de debordement des graphiques */
		       if (pBox->BxAbstractBox->AbLeafType == LtGraphics)
			  i = EXTRA_GRAPH;
		       else
			  i = 0;
		       if (delta > 0)
			  DefClip (frame, pBox->BxXOrg - i, pBox->BxYOrg - delta - i,
				   pBox->BxXOrg + pBox->BxWidth + i, pBox->BxYOrg + pBox->BxHeight + i);
		       else
			  DefClip (frame, pBox->BxXOrg - i, pBox->BxYOrg - i,
				   pBox->BxXOrg + pBox->BxWidth + i,
				 pBox->BxYOrg + pBox->BxHeight - delta + i);
		    }

		  /* decale les boites qui ont des relations hors-structure avec */
		  /* la boite deplacee et on met a jour les dimensions elastiques   */
		  /* des boites liees a la boite deplacee.                          */
		  pPosRel = pBox->BxPosRelations;
		  while (pPosRel != NULL)
		    {
		       i = 0;
		       notEmpty = pPosRel->PosRTable[i].ReBox != NULL;
		       while (i < MAX_RELAT_POS && notEmpty)
			 {
			    pRelation = &pPosRel->PosRTable[i];
			    if (pRelation->ReBox->BxAbstractBox != NULL)
			      {
				 /* Relation hors-struture sur l'origine de la boite */
				 if (pRelation->ReOp == OpVertDep
				     && pRelation->ReBox->BxYOutOfStruct
				     && pRelation->ReBox->BxAbstractBox->AbVertPos.PosAbRef == pBox->BxAbstractBox)
				   {

				      /* refuse de deplacer une boite englobante de pBox */
				      if (IsParentBox (pRelation->ReBox, pBox))
					 ;
				      else if (pRelation->ReBox->BxVertFlex)
					 MoveBoxEdge (pRelation->ReBox, pBox, pRelation->ReOp, delta, frame, FALSE);
				      else
					 YMove (pRelation->ReBox, pBox, delta, frame);

				   }
				 /* Relation sur la hauteur elastique de la boite */
				 else if (pRelation->ReOp == OpHeight)
				    MoveBoxEdge (pRelation->ReBox, pBox, pRelation->ReOp, delta, frame, FALSE);
			      }
			    i++;
			    if (i < MAX_RELAT_POS)
			       notEmpty = pPosRel->PosRTable[i].ReBox != NULL;
			 }
		       /* next block */
		       pPosRel = pPosRel->PosRNext;
		    }

		  /* traite les fils englobes ou places par rapport a l'englobante */
		  /* Traite le niveau inferieur */
		  pChildAb = pBox->BxAbstractBox->AbFirstEnclosed;
		  toVertPack = FALSE;
		  /* Si la boite est en cours de deplacement -> il faut transmettre */
		  /* la valeur de l'origine plutot que la valeur du decalage        */
		  if (pBox->BxYToCompute)
		     delta = pBox->BxYOrg;
		  while (pChildAb != NULL)
		    {
		       if (pChildAb->AbBox != NULL)
			 {
			    if (pChildAb->AbBox->BxYOutOfStruct)
			       toVertPack = TRUE;
			    else
			      {
				 if (pBox->BxYToCompute && !pChildAb->AbBox->BxVertFlex)
				    /* Additionne le decalage de la boite */
				    pChildAb->AbBox->BxYToCompute = TRUE;
				 YMoveAllEnclosed (pChildAb->AbBox, delta, frame);
			      }
			 }
		       pChildAb = pChildAb->AbNext;
		    }
		  pBox->BxYToCompute = FALSE;	/* le decalage eventuel est pris en compte */

		  /* Si la hauteur de la boite depend du contenu et qu'une des     */
		  /* boites filles est positionnee par une relation hors-structure */
		  /* --> il faut reevaluer la hauteur correspondante.              */
		  if (toVertPack &&
		      (pBox->BxContentHeight || (!pBox->BxAbstractBox->AbHeight.DimIsPosition && pBox->BxAbstractBox->AbHeight.DimMinimum)))
		     RecordEnclosing (pBox, FALSE);
	       }
	  }
	/* restaure */
	PackBoxRoot = pParentBox;
     }
}


/*----------------------------------------------------------------------
  MoveVertRef moves the vertical reference axis of the box.
  All boxes attached to that axis are moved.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                MoveVertRef (PtrBox pBox, PtrBox pFromBox, int delta, int frame)
#else  /* __STDC__ */
void                MoveVertRef (pBox, pFromBox, delta, frame)
PtrBox              pBox;
PtrBox              pFromBox;
int                 delta;
int                 frame;

#endif /* __STDC__ */
{
  int                 i, j, k;
  PtrAbstractBox      pAb;
  PtrAbstractBox      pCurrentAb;
  PtrBox              pNextBox;
  PtrBox              pRefBox;
  PtrPosRelations     pPosRel;
  BoxRelation        *pRelation;
  ThotBool            toMove;
  ThotBool            notEmpty;

  if (pBox != NULL && delta != 0)
    {
      if (pBox->BxAbstractBox != NULL)
	if (!pBox->BxAbstractBox->AbDead)
	  {
	    /* check if the box is in the history of moved boxes */
	    toMove = TRUE;
	    pNextBox = pFromBox;
	    pRefBox = pBox;
	    while (pNextBox != NULL)
	      if (pNextBox == pBox)
		{
		  pNextBox = NULL;
		  toMove = FALSE;
		}
	      else
		{
		  if (pNextBox->BxMoved == NULL)
		    /* boite ancetre */
		    pRefBox = pNextBox;
		  pNextBox = pNextBox->BxMoved;
		}
	     
	    if (toMove)
	      {
		pBox->BxVertRef += delta;
		pCurrentAb = pBox->BxAbstractBox;
		/* add the box in the history */
		pBox->BxMoved = pFromBox;
		/* regarde si les regles de dependance sont valides */
		if (pCurrentAb->AbEnclosing != NULL)
		  if (pCurrentAb->AbEnclosing->AbBox != NULL)
		    toMove = pCurrentAb->AbEnclosing->AbBox->BxType != BoGhost;
		
		if (toMove)
		  if (pBox->BxHorizEdge == VertRef)
		    {
		      /* the box position depends on its vertical axis */
		      delta = -delta;
		      /* need to check inclusion */
		      toMove = TRUE;
		      /* register the window area to be redisplayed */
		      if (ReadyToDisplay && pBox->BxType != BoSplit)
			{
			  i = pBox->BxXOrg;
			  j = pBox->BxXOrg + pBox->BxWidth;
			  /* add margins for graphics */
			  if (pBox->BxAbstractBox->AbLeafType == LtGraphics)
			    k = EXTRA_GRAPH;
			  else
			    k = 0;
			  if (delta > 0)
			    j += delta;
			  else
			    i += delta;
			  DefClip (frame, i - k, pBox->BxYOrg - k, j + k, pBox->BxYOrg + pBox->BxHeight + k);
			}
		       
		      if (IsXPosComplete (pBox))
			{
			  XMoveAllEnclosed (pBox, delta, frame);
			  /* we could clean up the history -> restore it */
			  pBox->BxMoved = pFromBox;
			}
		      else
			 pBox->BxXOrg += delta;
		       
		      /* Move attached boxes */
		      pPosRel = pBox->BxPosRelations;
		      while (pPosRel != NULL)
			{
			  i = 0;
			  notEmpty = (pPosRel->PosRTable[i].ReBox != NULL);
			  while (i < MAX_RELAT_POS && notEmpty)
			    {
			      pRelation = &pPosRel->PosRTable[i];
			      if (pRelation->ReBox->BxAbstractBox != NULL)
				{
				  if (pRelation->ReBox != pBox
				      && pRelation->ReRefEdge != VertRef)
				    if (pRelation->ReOp == OpHorizRef)
				      MoveVertRef (pRelation->ReBox, pBox, delta, frame);
				    else if ((pRelation->ReOp == OpHorizDep
					      && pRelation->ReBox->BxAbstractBox->AbWidth.DimIsPosition)
					     || pRelation->ReOp == OpWidth)
				      MoveBoxEdge (pRelation->ReBox, pBox, pRelation->ReOp, delta, frame, TRUE);
				    else if (pRelation->ReOp == OpHorizDep)
				      XMove (pRelation->ReBox, pBox, delta, frame);
				}
			       
			      i++;
			      if (i < MAX_RELAT_POS)
				notEmpty = (pPosRel->PosRTable[i].ReBox != NULL);
			    }
			  /* next relation block */
			  pPosRel = pPosRel->PosRNext;
			}
		    }
		  else
		    {
		      /* the box position doesn't depends on its axis */
		      /* move sibing boxes? */
		      toMove = FALSE;
		      pPosRel = pBox->BxPosRelations;
		      while (pPosRel != NULL)
			{
			  i = 0;
			  notEmpty = (pPosRel->PosRTable[i].ReBox != NULL);
			  while (i < MAX_RELAT_POS && notEmpty)
			    {
			      pRelation = &pPosRel->PosRTable[i];
			      if (pRelation->ReBox->BxAbstractBox != NULL)
				{
				  if (pRelation->ReRefEdge == VertRef)
				    if (pRelation->ReOp == OpHorizRef)
				      /* move a vertical axis */
				      MoveVertRef (pRelation->ReBox, pFromBox, delta, frame);
				    else if ((pRelation->ReOp == OpHorizDep
					      && pRelation->ReBox->BxAbstractBox->AbWidth.DimIsPosition)
					     || pRelation->ReOp == OpWidth)
				      /* move a a edge of a stretchable box */
				      MoveBoxEdge (pRelation->ReBox, pBox, pRelation->ReOp, delta, frame, TRUE);
				    else if (pRelation->ReOp == OpHorizDep)
				      {
					/* move a sibling */
					XMove (pRelation->ReBox, pBox, delta, frame);
					/* verify enclosing */
					toMove = TRUE;
				      }
				}
			      
			      i++;
			      if (i < MAX_RELAT_POS)
				notEmpty = (pPosRel->PosRTable[i].ReBox != NULL);
			    }
			  /* next relation block */
			  pPosRel = pPosRel->PosRNext;
			}
		       
		      /* Move inclosing boxes */
		      pNextBox = NULL;
		      if (IsXPosComplete (pBox) && pBox->BxType != BoBlock)
			{
			  pAb = pCurrentAb->AbFirstEnclosed;
			  while (pAb != NULL)
			    {
			      if (!pAb->AbDead
				  && pAb->AbHorizPos.PosAbRef == pCurrentAb
				  && pAb->AbHorizPos.PosRefEdge == VertRef)
				{
				  pNextBox = pAb->AbBox;
				  XMove (pNextBox, pBox, delta, frame);
				}
			      pAb = pAb->AbNext;
			    }
			   
			  if (pNextBox != NULL && Propagate == ToAll)
			    /* check the inclusion */
			    WidthPack (pCurrentAb, pRefBox, frame);
			}
		    }
		 
		/* check the inclusion constraint */
		pAb = pCurrentAb->AbEnclosing;
		if (toMove)
		  if (pAb == NULL)
		    {
		      if (Propagate == ToAll && pBox->BxXOrg < 0)
			XMoveAllEnclosed (pBox, -pBox->BxXOrg, frame);
		    }
		  else if (!pAb->AbInLine
			   && pAb->AbBox->BxType != BoGhost
			   && !IsParentBox (pAb->AbBox, pRefBox)
			   && !IsParentBox (pAb->AbBox, PackBoxRoot))
		    /* check the inclusion of the sibling box */
		    WidthPack (pAb, pRefBox, frame);
	      }
	  }
    }
}

/*----------------------------------------------------------------------
  MoveHorizRef moves the baseline of the box.
  All boxes attached to that axis are moved.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                MoveHorizRef (PtrBox pBox, PtrBox pFromBox, int delta, int frame)
#else  /* __STDC__ */
void                MoveHorizRef (pBox, pFromBox, delta, frame)
PtrBox              pBox;
PtrBox              pFromBox;
int                 delta;
int                 frame;

#endif /* __STDC__ */
{
  int                 i, j, k;
  PtrAbstractBox      pAb;
  PtrAbstractBox      pCurrentAb;
  PtrBox              pNextBox;
  PtrBox              pRefBox;
  PtrPosRelations     pPosRel;
  BoxRelation        *pRelation;
  ThotBool            toMove;
  ThotBool            notEmpty;

  if (pBox != NULL && delta != 0)
    {
      if (pBox->BxAbstractBox != NULL)
	if (!pBox->BxAbstractBox->AbDead)
	  {
	    /* check if the box is in the history of moved boxes */
	    toMove = TRUE;
	    pNextBox = pFromBox;
	    pRefBox = pBox;
	    while (pNextBox != NULL)
	      if (pNextBox == pBox)
		{
		  pNextBox = NULL;
		  toMove = FALSE;
		}
	      else
		{
		  if (pNextBox->BxMoved == NULL)
		    pRefBox = pNextBox;	/* ancestor box */
		  pNextBox = pNextBox->BxMoved;
		}
	    
	    if (toMove)
	      {
		pBox->BxHorizRef += delta;
		pCurrentAb = pBox->BxAbstractBox;
		/* add the box in the history */
		pBox->BxMoved = pFromBox;
		/* check validity of dependencies */
		if (pCurrentAb->AbEnclosing != NULL)
		  if (pCurrentAb->AbEnclosing->AbBox != NULL)
		    toMove = pCurrentAb->AbEnclosing->AbBox->BxType != BoGhost;
		
		if (toMove)
		  if (pBox->BxVertEdge == HorizRef)
		    {
		      /* the box position depends on its baseline */
		      delta = -delta;
		      /* need to check inclusion */
		      toMove = TRUE;
		      /* register the window area to be redisplayed */
		      if (ReadyToDisplay && pBox->BxType != BoSplit)
			{
			  i = pBox->BxYOrg;
			  j = pBox->BxYOrg + pBox->BxHeight;
			  /* add margins for graphics */
			  if (pBox->BxAbstractBox->AbLeafType == LtGraphics)
			    k = EXTRA_GRAPH;
			  else
			    k = 0;
			  if (delta > 0)
			    j += delta;
			  else
			    i += delta;
			  DefClip (frame, pBox->BxXOrg - k, i - k, pBox->BxXOrg + pBox->BxWidth + k, j + k);
			}
		      
		      if (IsYPosComplete (pBox))
			{
			  YMoveAllEnclosed (pBox, delta, frame);
			  /* we could clean up the history -> restore it */
			  pBox->BxMoved = pFromBox;
			}
		      else
			pBox->BxYOrg += delta;
		      
		      /* Move attached boxes */
		      pPosRel = pBox->BxPosRelations;
		      while (pPosRel != NULL)
			{
			  i = 0;
			  notEmpty = pPosRel->PosRTable[i].ReBox != NULL;
			  while (i < MAX_RELAT_POS && notEmpty)
			    {
			      pRelation = &pPosRel->PosRTable[i];
			      if (pRelation->ReBox->BxAbstractBox != NULL)
				{
				  if (pRelation->ReBox != pBox && pRelation->ReRefEdge != HorizRef)
				    if (pRelation->ReOp == OpVertRef)
				      MoveHorizRef (pRelation->ReBox, pBox, delta, frame);
				    else if ((pRelation->ReOp == OpVertDep
					      && pRelation->ReBox->BxAbstractBox->AbHeight.DimIsPosition)
					     || pRelation->ReOp == OpHeight)
				      MoveBoxEdge (pRelation->ReBox, pBox, pRelation->ReOp, delta, frame, FALSE);
				    else if (pRelation->ReOp == OpVertDep)
				      YMove (pRelation->ReBox, pBox, delta, frame);
				}
			      
			      i++;
			      if (i < MAX_RELAT_POS)
				notEmpty = pPosRel->PosRTable[i].ReBox != NULL;
			    }
			  /* next relation block */
			  pPosRel = pPosRel->PosRNext;
			}
		    }
		  else
		    {
		      /* the box position doesn't depends on its baseline */
		      /* move sibing boxes? */
		      toMove = FALSE;
		      pPosRel = pBox->BxPosRelations;
		      while (pPosRel != NULL)
			{
			  i = 0;
			  notEmpty = pPosRel->PosRTable[i].ReBox != NULL;
			  while (i < MAX_RELAT_POS && notEmpty)
			    {
			      pRelation = &pPosRel->PosRTable[i];
			      if (pRelation->ReBox->BxAbstractBox != NULL)
				{
				  if (pRelation->ReRefEdge == HorizRef)
				    if (pRelation->ReOp == OpVertRef)
				      /* move a baseline */
				      MoveHorizRef (pRelation->ReBox, pFromBox, delta, frame);
				    else if ((pRelation->ReOp == OpVertDep
					      && pRelation->ReBox->BxAbstractBox->AbHeight.DimIsPosition)
					     || pRelation->ReOp == OpHeight)
				      /* move a a edge of a stretchable box */
				      MoveBoxEdge (pRelation->ReBox, pBox, pRelation->ReOp, delta, frame, FALSE);
				    else if (pRelation->ReOp == OpVertDep)
				      {
					/* move a sibling */
					YMove (pRelation->ReBox, pBox, delta, frame);
					/* verify enclosing */
					toMove = TRUE;
				      }
				}
			      
			      i++;
			      if (i < MAX_RELAT_POS)
				notEmpty = pPosRel->PosRTable[i].ReBox != NULL;
			    }
			  /* next relation block */
			  pPosRel = pPosRel->PosRNext;
			}
		      
		      /* Move inclosing boxes */
		      pNextBox = NULL;
		      if (IsYPosComplete (pBox) && pBox->BxType != BoBlock)
			{
			  pAb = pCurrentAb->AbFirstEnclosed;
			  while (pAb != NULL)
			    {
			      if (!pAb->AbDead
				  && pAb->AbVertPos.PosAbRef == pCurrentAb
				  && pAb->AbVertPos.PosRefEdge == HorizRef)
				{
				  pNextBox = pAb->AbBox;
				  YMove (pNextBox, pBox, delta, frame);
				}
			      pAb = pAb->AbNext;
			    }
			  
			  if (pNextBox != NULL && Propagate == ToAll)
			    /* check the inclusion */
			    HeightPack (pCurrentAb, pRefBox, frame);
			}
		    }
		
		/* check the inclusion constraint */
		pAb = pCurrentAb->AbEnclosing;
		if (toMove)
		  if (pAb == NULL)
		    {
		      if (Propagate == ToAll && pBox->BxYOrg < 0)
			YMoveAllEnclosed (pBox, -pBox->BxYOrg, frame);
		    }
		   else if (pAb->AbInLine
			    || pAb->AbBox->BxType == BoGhost)
		     {
		       if (Propagate == ToAll)
			 EncloseInLine (pBox, frame, pAb);
		     }
		  else if (!IsParentBox (pAb->AbBox, pRefBox)
			   && !IsParentBox (pAb->AbBox, PackBoxRoot))
		    /* check the inclusion of the sibling box */
		    HeightPack (pAb, pRefBox, frame);
	      }
	  }
    }
}


/*----------------------------------------------------------------------
  ResizeWidth updates the inside box width BxW then the outside one BxWidth.
  The parameter origin of the change is pSourceBox.
  Check positionning and sizing constraints:
  - any box attached to a box edge is moved
  - update the baseline
  - any other box size which depends on it is updated
  The parameters delta, l, r gives changes of the width, left, and right 
  extra (margin, border, padding)
  The parameter spaceDelta gives the number of spaces added (>0) or removed
  only if it's a text box included within a justified line.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ResizeWidth (PtrBox pBox, PtrBox pSourceBox, PtrBox pFromBox, int delta, int l, int r, int spaceDelta, int frame)
#else  /* __STDC__ */
void                ResizeWidth (pBox, pSourceBox, pFromBox, delta, l, r, spaceDelta, frame)
PtrBox              pBox;
PtrBox              pSourceBox;
PtrBox              pFromBox;
int                 delta;
int                 l;
int                 r;
int                 spaceDelta;
int                 frame;

#endif /* __STDC__ */
{
  PtrBox              box;
  PtrLine             pLine;
  PtrAbstractBox      pAb;
  PtrAbstractBox      pCurrentAb;
  PtrPosRelations     pPosRel;
  PtrDimRelations     pDimRel;
  BoxRelation        *pRelation;
  ViewSelection      *pViewSel;
  int                 i, j, k, diff, val;
  int                 orgTrans, middleTrans, endTrans;
  ThotBool            notEmpty;
  ThotBool            toMove;
  ThotBool            absoluteMove;
  ThotBool            externalRef;

  if (!pBox)
    return;
  /* check if the inside width, margins, borders, and paddings change */
  pCurrentAb = pBox->BxAbstractBox;
  if (!pCurrentAb)
    return;

  if (pCurrentAb->AbMBPChange)
    /* margins borders and are not interpreted yet */
    diff = 0;
  else
    diff = pBox->BxW + pBox->BxLMargin + pBox->BxRMargin + pBox->BxLPadding + pBox->BxRPadding + pBox->BxLBorder + pBox->BxRBorder - pBox->BxWidth;
  if (delta || diff)
    {
      /* Do we have to clean up the history of moved boxes */
      if (pSourceBox == NULL && pFromBox == NULL)
	pBox->BxMoved = NULL;
      
      if (!pCurrentAb->AbDead)
	{
	  /* Area zone before moving */
	  i = pBox->BxXOrg;
	  j = i + pBox->BxWidth;
	  /* It's not a stretchable box: clean up the history */
	  if (!pBox->BxHorizFlex)
	    pBox->BxMoved = NULL;
	  
	  /* Force the reevaluation of polyline control points */
	  if (pCurrentAb->AbLeafType == LtPolyLine && pBox->BxPictInfo != NULL)
	    {
	      /* free control points */
	      free ((STRING) pBox->BxPictInfo);
	      pBox->BxPictInfo = NULL;
	    }
	    
	  /* Check the validity of dependency rules */
	  toMove = TRUE;
	  if (pCurrentAb->AbEnclosing != NULL && pCurrentAb->AbEnclosing->AbBox != NULL)
	    toMove = (pCurrentAb->AbEnclosing->AbBox->BxType != BoGhost &&
		      pCurrentAb->AbEnclosing->AbBox->BxType != BoBlock);
	  
	  /* check positionning constraints */
	  if (!toMove || pBox->BxHorizEdge == Left || pBox->BxHorizEdge == VertRef)
	    {
	      /*====> The left is fixed */
	      /* Move the middle and the right */
	      orgTrans = 0;
	      middleTrans = (pBox->BxWidth + delta + diff) / 2 - pBox->BxWidth / 2;
	      endTrans = delta + diff;
	    }
	  else if (pBox->BxHorizEdge == VertMiddle)
	    {
	      /*====> The middle is fixed */
	      /* Move the left and the right */
	      orgTrans = pBox->BxWidth / 2 - (pBox->BxWidth + delta + diff) / 2;
	      /* adjust */
	      endTrans = delta + diff + orgTrans;
	      middleTrans = 0;
	    }
	  else
	    {
	      /*====> The right is fixed */
	      /* Move the left and the middle */
	      orgTrans = -delta - diff;
	      middleTrans = pBox->BxWidth / 2 - (pBox->BxWidth + delta + diff) / 2;
	      endTrans = 0;
	    }
	  
	  /* inside width */
	  pBox->BxW += delta;
	  /* outside width */
	  pBox->BxWidth = pBox->BxWidth + delta + diff;
	  pBox->BxXOrg += orgTrans;
	  
	  /* register the window area to be redisplayed */
	  if (ReadyToDisplay
	      && pBox->BxType != BoSplit
	      /* don't take care of a box which is not */
	      /* at its right place in the concrete image  */
	      && !pBox->BxXToCompute
	      && !pBox->BxYToCompute
	      && (orgTrans != 0
		  || pCurrentAb->AbFirstEnclosed == NULL
		  /* redisplay filled boxes */
		  || pCurrentAb->AbFillBox
		  || pCurrentAb->AbPictBackground != NULL))
	    {
	      if (pCurrentAb->AbLeafType == LtText)
		{
		  k = 0;
		  if (orgTrans == 0)
		    i = j;
		  else if (orgTrans < 0)
		    i += orgTrans;
		  
		  if (endTrans == 0)
		    j = i;
		  else if (endTrans > 0)
		    j += endTrans;
		}
	      else
		{
		  /* add an extra margin for graphics */
		  if (pCurrentAb->AbLeafType == LtGraphics)
		    k = EXTRA_GRAPH;
		  else
		    k = 0;
		  if (orgTrans < 0)
		    i += orgTrans;
		  if (endTrans > 0)
		    j += endTrans;
		}
	      
	      DefClip (frame, i - k, pBox->BxYOrg - k, j + k, pBox->BxYOrg + pBox->BxHeight + k);
	    }
	  
	  /* Moving sibling boxes and the parent? */
	  pPosRel = pBox->BxPosRelations;
	  while (pPosRel != NULL)
	    {
	      i = 0;
	      notEmpty = pPosRel->PosRTable[i].ReBox != NULL;
	      while (i < MAX_RELAT_POS && notEmpty)
		{
		  pRelation = &pPosRel->PosRTable[i];
		  if (pRelation->ReBox->BxAbstractBox != NULL)
		    /* Ignore the back relation of a stretchable box */
		    if (!pBox->BxHorizFlex
			|| pRelation->ReOp != OpHorizDep
			|| pCurrentAb == pRelation->ReBox->BxAbstractBox->AbHorizPos.PosAbRef)
		      switch (pRelation->ReRefEdge)
			{
			case Left:
			  if (pRelation->ReOp == OpHorizRef)
			    {
			      if (pRelation->ReBox != pBox)
				MoveVertRef (pRelation->ReBox, pSourceBox, orgTrans, frame);
			    }
			  else if (toMove)
			    {
			      if (pRelation->ReOp == OpHorizInc)
				{
				  if (!pBox->BxHorizFlex)
				    XMove (pBox, NULL, -orgTrans, frame);
				}
			      else if ((pRelation->ReOp == OpHorizDep && pRelation->ReBox->BxHorizFlex)
				       || pRelation->ReOp == OpWidth)
				MoveBoxEdge (pRelation->ReBox, pBox, pRelation->ReOp, orgTrans, frame, TRUE);
			      else if (pRelation->ReBox != pSourceBox)
				XMove (pRelation->ReBox, pBox, orgTrans, frame);
			    }
			  break;
			case VertMiddle:
			  if (pRelation->ReOp == OpHorizRef)
			    {
			      if (pRelation->ReBox == pBox)
				{
				  MoveVertRef (pBox, NULL, delta / 2, frame);
				  /* restore the history of moved boxes */
				  pBox->BxMoved = pFromBox;
				}
			      else
				MoveVertRef (pRelation->ReBox, pSourceBox, middleTrans, frame);
			    }
			  else if (toMove)
			    {
			      if (pRelation->ReOp == OpHorizInc)
				{
				  if (!pBox->BxHorizFlex)
				    XMove (pBox, NULL, -middleTrans, frame);
				}
			      else if ((pRelation->ReOp == OpHorizDep && pRelation->ReBox->BxHorizFlex)
				       || pRelation->ReOp == OpWidth)
				MoveBoxEdge (pRelation->ReBox, pBox, pRelation->ReOp, middleTrans, frame, TRUE);
			      else if (pRelation->ReBox != pSourceBox)
				XMove (pRelation->ReBox, pBox, middleTrans, frame);
			    }
			  break;
			case Right:
			  if (pRelation->ReOp == OpHorizRef)
			    {
			      if (pRelation->ReBox == pBox)
				{
				  MoveVertRef (pBox, NULL, delta, frame);
				  /* restore the history of moved boxes */
				  pBox->BxMoved = pFromBox;
				}
			      else
				MoveVertRef (pRelation->ReBox, pSourceBox, endTrans, frame);
			    }
			  else if (toMove)
			    {
			      if (pRelation->ReOp == OpHorizInc)
				{
				  if (!pBox->BxHorizFlex)
				    XMove (pBox, NULL, -endTrans, frame);
				}
			      else if ((pRelation->ReOp == OpHorizDep && pRelation->ReBox->BxHorizFlex)
				       || pRelation->ReOp == OpWidth)
				MoveBoxEdge (pRelation->ReBox, pBox, pRelation->ReOp, endTrans, frame, TRUE);
			      else if (pRelation->ReBox != pSourceBox)
				XMove (pRelation->ReBox, pBox, endTrans, frame);
			    }
			  break;
			default:
			  break;
			}
		    
		  i++;
		  if (i < MAX_RELAT_POS)
		    notEmpty = (pPosRel->PosRTable[i].ReBox != NULL);
		}
	      /* next relations block */
	      pPosRel = pPosRel->PosRNext;
	    }
	  
	  /* Keep in mind if the box positionning is absolute or not */
	  absoluteMove = IsXPosComplete (pBox);
	  /* internal boxes take into account margins borders and paddings */
	  if (l != 0 || r != 0)
	    {
	      orgTrans = l;
	      middleTrans = (l - r)/2;
	      endTrans = - r;
	    }
	  /* Moving included boxes or reevalution of the block of lines? */
	  if (absoluteMove ||
	      pCurrentAb->AbWidth.DimAbRef != NULL ||
	      pCurrentAb->AbWidth.DimValue != 0)
	    /* the box is already built */
	    /* or it's within a stretchable box */
	    /* or it's within a unnested box */
	    /* or it doesn't inherit the size from its contents */
	    if (pBox->BxType == BoBlock)
	      RecomputeLines (pCurrentAb, pBox->BxFirstLine, pSourceBox, frame);
	    else
	      {
		pAb = pCurrentAb->AbFirstEnclosed;
		while (pAb != NULL)
		  {
		    if (!pAb->AbDead && pAb->AbBox != NULL)
		      {
			box = pAb->AbBox;
			/* check if the position box depends on its enclosing */
			pPosRel = box->BxPosRelations;
			while (pPosRel != NULL)
			  {
			    i = 0;
			    notEmpty = (pPosRel->PosRTable[i].ReBox != NULL);
			    while (i < MAX_RELAT_POS && notEmpty)
			      {
				pRelation = &pPosRel->PosRTable[i];
				if (pRelation->ReOp == OpHorizInc
				    && pRelation->ReRefEdge != VertRef)
				  switch (pAb->AbHorizPos.PosRefEdge)
				    {
				    case VertMiddle:
				      if (absoluteMove)
					j = middleTrans;
				      else
					/* recheck the position */
					j = (pBox->BxWidth - box->BxWidth) / 2 - box->BxXOrg;
				      if (box->BxHorizFlex)
					MoveBoxEdge (box, pBox, pRelation->ReOp, j, frame, TRUE);
				      else
					XMove (box, pBox, j, frame);
				      break;
				    case Right:
				      if (absoluteMove)
					j = endTrans;
				      else
					/* recheck the position */
					j = pBox->BxWidth - box->BxWidth - box->BxXOrg;
				      if (box->BxHorizFlex)
					MoveBoxEdge (box, pBox, pRelation->ReOp, j, frame, TRUE);
				      else
					XMove (box, pBox, j, frame);
				      break;
				    default:
				      if (absoluteMove)
					j = orgTrans;
				      else
					/* recheck the position */
					j = 0;
				      if (box->BxHorizFlex)
					MoveBoxEdge (box, pBox, pRelation->ReOp, j, frame, TRUE);
				      else
					XMove (box, pBox, j, frame);
				      break;
				    }
				
				i++;
				if (i < MAX_RELAT_POS)
				  notEmpty = (pPosRel->PosRTable[i].ReBox != NULL);
			      }
			    /* next relationblock */
			    pPosRel = pPosRel->PosRNext;
			  }
		      }
		    /* next child */
		    pAb = pAb->AbNext;
		  }
	      }
	  
	  /* check dimension constraints */
	  pDimRel = pBox->BxWidthRelations;
	  while (pDimRel != NULL)
	    {
	      i = 0;
	      while (i < MAX_RELAT_DIM && pDimRel->DimRTable[i] != NULL)
		{
		  box = pDimRel->DimRTable[i];
		  pAb = box->BxAbstractBox;
		  if (pAb)
		    /* Is it the same dimension? */
		    if (pDimRel->DimRSame[i])
		      {
			/* Changing the width */
			if (pAb->AbWidth.DimUnit == UnPercent)
			  {
			    if (pAb->AbEnclosing == pCurrentAb)
			      /* refer the inside width */
			      val = pBox->BxW;
			    else
			      /* refer the outside width */
			      val = pBox->BxWidth;
			    val = val * pAb->AbWidth.DimValue / 100;
			    val = val - box->BxWidth;
			  }
			else
			  {
			    val = delta;
			    if (pAb->AbEnclosing != pCurrentAb)
			      /* refer the outside width */
			      val = val + l + r;
			  }
			/* avoid cycles on the same box */
			if (box != pBox)
			  ChangeWidth (box, pSourceBox, pBox, val, 0, frame);
		      }
		    else
		      {
			/* Changing the height */
			if (pAb->AbHeight.DimUnit == UnPercent)
			  {
			    if (pAb->AbEnclosing == pCurrentAb)
			      val = pBox->BxW;
			    else
			      /* refer the outside width */
			      val = pBox->BxWidth;
			    val = val * pAb->AbHeight.DimValue / 100;
			    val = val - box->BxHeight;
			  }
			else
			  {
			    val = delta;
			    if (pAb->AbEnclosing != pCurrentAb)
			      /* refer the outside width */
			      val = val + l + r;
			  }
			ChangeHeight (box, pSourceBox, NULL, val, frame);
		      }
		    
		  i++;
		}
	      /* next relation block */
	      pDimRel = pDimRel->DimRNext;
	    }
	  
	  /* Check enclosing constraints */
	  pAb = pCurrentAb->AbEnclosing;
	  if (!toMove)
	    {
	      /* look for the enclosing block of lines  */
	      while (!pAb->AbInLine || pAb->AbBox->BxType == BoGhost)
		pAb = pAb->AbEnclosing;
	    }
	  
	  if (pAb == NULL)
	    {
	      /* It's the root of the concrete image */
	      if (Propagate == ToAll && pBox->BxXOrg < 0)
		XMoveAllEnclosed (pBox, -pBox->BxXOrg, frame);
	    }
	  else if (pCurrentAb->AbHorizEnclosing)
	    {
	      /* keep in mind if the relation concerns parent boxes */
	      externalRef = !IsParentBox (pAb->AbBox, pSourceBox);
	      
	      /*
	       * if pSourceBox is a child and the inclusion is not performed
	       * by another sibling box, we need to propagate the change
	       */
	      if ((Propagate == ToAll || externalRef)
		  && !IsSiblingBox (pBox, pFromBox)
		  && !IsSiblingBox (pBox, pSourceBox))
		{		    
		  /* Within a block of line */
		  if (pAb->AbInLine)
		    {
		      pViewSel = &ViewFrameTable[frame - 1].FrSelectionBegin;
		      if (pBox == pViewSel->VsBox)
			pLine = pViewSel->VsLine;
		      else
			pLine = SearchLine (pBox);
		      UpdateLineBlock (pAb, pLine, pBox, delta, spaceDelta, frame);
		    }
		  /* if the inclusion is not checked at the end */
		  else if (!IsParentBox (pAb->AbBox, PackBoxRoot))
		    {
		      /* Differ the checking of the inclusion */
		      if (Propagate != ToAll)
			RecordEnclosing (pAb->AbBox, TRUE);
		      /* Don't check the inclusion more than 2 times */
		      else if (pAb->AbBox->BxCycles <= 1)
			{
			  if (pAb->AbBox->BxType == BoCell &&
			      ThotLocalActions[T_checkcolumn])
			    (*ThotLocalActions[T_checkcolumn]) (pAb, NULL, frame);
			  else
			    WidthPack (pAb, pSourceBox, frame);
			}
		    }
		}
	      else if (!pCurrentAb->AbNew
		       && Propagate == ToSiblings
		       && pCurrentAb->AbLeafType == LtCompound
		       && pCurrentAb->AbInLine && !pBox->BxYToCompute)
		{
		  /* the width of the block of lines is given by the next box
		     -> check vertical enclosing */
		  if (pAb->AbBox->BxType != BoTable)
		    HeightPack (pAb, pSourceBox, frame);
		  Propagate = ToSiblings;
		}
	    }

	  if (pBox->BxType == BoTable && pBox->BxCycles == 0 &&
	      ThotLocalActions[T_resizetable])
	    (*ThotLocalActions[T_resizetable]) (pCurrentAb, frame);
	}
    }
}

/*----------------------------------------------------------------------
  ResizeHeight updates the inside box width BxH then the outside one BxHeight.
  The parameter origin of the change is pSourceBox.
  Check positionning and sizing constraints:
  - any box attached to a box edge is moved
  - update the vertical reference axis
  - any other box size which depends on it is updated
  The parameters delta, t, b gives changes of the height, top, and bottom
  extra (margin, border, padding)
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ResizeHeight (PtrBox pBox, PtrBox pSourceBox, PtrBox pFromBox, int delta, int t, int b, int frame)
#else  /* __STDC__ */
void                ResizeHeight (pBox, pSourceBox, pFromBox, delta, t, b, frame)
PtrBox              pBox;
PtrBox              pSourceBox;
PtrBox              pFromBox;
int                 delta;
int                 t;
int                 b;
int                 frame;

#endif /* __STDC__ */
{
  PtrBox              box;
  PtrLine             pLine;
  PtrAbstractBox      pAb/*, pCell*/;
  PtrAbstractBox      pCurrentAb;
  PtrPosRelations     pPosRel;
  PtrDimRelations     pDimRel;
  BoxRelation        *pRelation;
  ptrfont             font;
  float               value;
  int                 i, j, k, diff, val;
  int                 orgTrans, middleTrans, endTrans;
  ThotBool            notEmpty;
  ThotBool            toMove;
  ThotBool            absoluteMove;
  ThotBool            externalRef;

  if (!pBox)
    return;
  /* check if the inside width, margins, borders, and paddings change */
  pCurrentAb = pBox->BxAbstractBox;
  if (!pCurrentAb)
    return;

  if (pCurrentAb->AbMBPChange)
    /* margins borders and are not interpreted yet */
    diff = 0;
  else
    diff = pBox->BxH + pBox->BxTMargin + pBox->BxBMargin + pBox->BxTPadding + pBox->BxBPadding + pBox->BxTBorder + pBox->BxBBorder - pBox->BxHeight;
  if (delta || diff)
    {
      /* Do we have to clean up the history of moved boxes */
      if (pSourceBox == NULL && pFromBox == NULL)
	pBox->BxMoved = NULL;
      
      if (!pCurrentAb->AbDead)
	{
	  /* Area zone before moving */
	  i = pBox->BxYOrg;
	  j = i + pBox->BxHeight;
	  /* It's not a stretchable box: clean up the history */
	  if (!pBox->BxVertFlex)
	    pBox->BxMoved = NULL;
	  
	  /* Force the reevaluation of polyline control points */
	  if (pCurrentAb->AbLeafType == LtPolyLine && pBox->BxPictInfo != NULL)
	    {
	      /* free control points */
	      free ((STRING) pBox->BxPictInfo);
	      pBox->BxPictInfo = NULL;
	    }
	  
	  /* Check the validity of dependency rules */
	  toMove = TRUE;
	  if (pCurrentAb->AbEnclosing != NULL)
	    if (pCurrentAb->AbEnclosing->AbBox != NULL)
	      toMove = pCurrentAb->AbEnclosing->AbBox->BxType != BoGhost;
	  
	  /* Check the validity of dependency rules */
	  if (!toMove || pBox->BxVertEdge == Top || pBox->BxVertEdge == HorizRef)
	    {
	      /*====> The top is fixed */
	      /* Move the middle and the bottom */
	      orgTrans = 0;
	      middleTrans = (pBox->BxHeight + delta + diff) / 2 - pBox->BxHeight / 2;
	      endTrans = delta + diff;
	    }
	  else if (pBox->BxVertEdge == HorizMiddle)
	    {
	      /*====> The middle is fixed */
	      /* Move the top and the bottom */
	      orgTrans = pBox->BxHeight / 2 - (pBox->BxHeight + delta + diff) / 2;
	      /* adjust */
	      endTrans = delta + diff + orgTrans;
	      middleTrans = 0;
	    }
	  /*=> Point fixe sur le cote inferieur */
	  else
	    {
	      /*====> The bottom is fixed */
	      /* Move the top and the middle */
	      orgTrans = -delta - diff;
	      middleTrans = pBox->BxHeight / 2 - (pBox->BxHeight + delta + diff) / 2;
	      endTrans = 0;
	    }
	  
	  /* inside height */
	  pBox->BxH += delta;
	  /* outside height */
	  pBox->BxHeight = pBox->BxHeight + delta + diff;
	  pBox->BxYOrg += orgTrans;
	  
	  /* register the window area to be redisplayed */
	  if (ReadyToDisplay
	      && pBox->BxType != BoSplit
	      /* don't take care of a box which is not */
	      /* at its right place in the concrete image  */
	      && !pBox->BxXToCompute
	      && !pBox->BxYToCompute
	      && (orgTrans != 0
		  || pCurrentAb->AbFirstEnclosed == NULL
		  /* redisplay filled boxes */
		  || pCurrentAb->AbFillBox
		  || pCurrentAb->AbPictBackground != NULL))
	    {
	      if (pCurrentAb->AbLeafType == LtText)
		{
		  k = 0;
		  if (orgTrans == 0)
		    i = j;
		  else if (orgTrans < 0)
		    i += orgTrans;
		  if (endTrans == 0)
		    j = i;
		  else if (endTrans > 0)
		    j += endTrans;
		}
	      else
		{
		  /* add an extra margin for graphics */
		  if (pCurrentAb->AbLeafType == LtGraphics)
		    k = EXTRA_GRAPH;
		  else
		    k = 0;
		  if (orgTrans < 0)
		    i += orgTrans;
		  if (endTrans > 0)
		    j += endTrans;
		}
	      DefClip (frame, pBox->BxXOrg - k, i - k, pBox->BxXOrg + pBox->BxWidth + k, j + k);
	    }
	  
	  /* Moving sibling boxes and the parent? */
	  pPosRel = pBox->BxPosRelations;
	  while (pPosRel != NULL)
	    {
	      i = 0;
	      notEmpty = (pPosRel->PosRTable[i].ReBox != NULL);
	      while (i < MAX_RELAT_POS && notEmpty)
		{
		  pRelation = &pPosRel->PosRTable[i];
		  if (pRelation->ReBox->BxAbstractBox != NULL)
		    /* Ignore the back relation of a stretchable box */
		    if (!pBox->BxVertFlex
			|| pRelation->ReOp != OpVertDep
			|| pCurrentAb == pRelation->ReBox->BxAbstractBox->AbVertPos.PosAbRef)
		      switch (pRelation->ReRefEdge)
			{
			case Top:
			  if (pRelation->ReOp == OpVertRef)
			    {
			      if (pRelation->ReBox != pBox)
				MoveHorizRef (pRelation->ReBox, pSourceBox, orgTrans, frame);
			    }
			  else if (toMove)
			    {
			      if (pRelation->ReOp == OpVertInc)
				{
				  if (!pBox->BxVertFlex)
				    YMove (pBox, NULL, -orgTrans, frame);
				}
			      else if ((pRelation->ReOp == OpVertDep && pRelation->ReBox->BxVertFlex)
				       || pRelation->ReOp == OpHeight)
				MoveBoxEdge (pRelation->ReBox, pBox, pRelation->ReOp, orgTrans, frame, FALSE);
			      else if (pRelation->ReBox != pSourceBox)
				YMove (pRelation->ReBox, pBox, orgTrans, frame);
			    }
			  break;
			case HorizMiddle:
			  if (pRelation->ReOp == OpVertRef)
			    {
			      if (pRelation->ReBox == pBox)
				{
				  MoveHorizRef (pBox, NULL, delta / 2, frame);
				  /* restore the history of moved boxes */
				  pBox->BxMoved = pFromBox;
				}
			      else
				MoveHorizRef (pRelation->ReBox, pSourceBox, middleTrans, frame);
			    }
			  else if (toMove)
			    {
			      if (pRelation->ReOp == OpVertInc)
				{
				  if (!pBox->BxVertFlex)
				    YMove (pBox, NULL, -middleTrans, frame);
				}
			      else if ((pRelation->ReOp == OpVertDep && pRelation->ReBox->BxVertFlex)
				       || pRelation->ReOp == OpHeight)
				MoveBoxEdge (pRelation->ReBox, pBox, pRelation->ReOp, middleTrans, frame, FALSE);
			      else if (pRelation->ReBox != pSourceBox)
				YMove (pRelation->ReBox, pBox, middleTrans, frame);
			    }
			  break;
			case Bottom:
			  if (pRelation->ReOp == OpVertRef)
			    {
			      if (pRelation->ReBox == pBox)
				{
				  if (pCurrentAb->AbLeafType == LtText
				      && pCurrentAb->AbHorizRef.PosAbRef == NULL)
				    j = FontBase (pBox->BxFont) - pBox->BxHorizRef;
				  else
				    j = delta;
				  MoveHorizRef (pBox, NULL, j, frame);
				  /* restore the history of moved boxes */
				  pBox->BxMoved = pFromBox;
				}
			      else
				MoveHorizRef (pRelation->ReBox, pSourceBox, endTrans, frame);
			    }
			  else if (toMove)
			    {
			      if (pRelation->ReOp == OpVertInc)
				{
				  if (!pBox->BxVertFlex)
				    YMove (pBox, NULL, -endTrans, frame);
				}
			      else if ((pRelation->ReOp == OpVertDep && pRelation->ReBox->BxVertFlex)
				       || pRelation->ReOp == OpHeight)
				MoveBoxEdge (pRelation->ReBox, pBox, pRelation->ReOp, endTrans, frame, FALSE);
			      else if (pRelation->ReBox != pSourceBox)
				YMove (pRelation->ReBox, pBox, endTrans, frame);
			    }
			  break;
			default:
			  break;
			}
		  i++;
		  if (i < MAX_RELAT_POS)
		    notEmpty = pPosRel->PosRTable[i].ReBox != NULL;
		}
	      /* next relations block */
	      pPosRel = pPosRel->PosRNext;
	    }
	     
	  /* Keep in mind if the box positionning is absolute or not */
	  absoluteMove = IsYPosComplete (pBox);
	  /* internal boxes take into account margins borders and paddings */
	  if (t != 0 || b != 0)
	    {
	      orgTrans = t;
	      middleTrans = (t - b)/2;
	      endTrans = -b;
	    }
	  /* Moving included boxes? */
	  if (absoluteMove && pBox->BxType == BoBlock)
	    {
	      /* manage stretched block of lines */
	      /* which are already processed     */
	      if (orgTrans != 0)
		{
		  /* move also included boxes */
		  pAb = pCurrentAb->AbFirstEnclosed;
		  while (pAb != NULL)
		    {
		      if (!pAb->AbDead)
			YMoveAllEnclosed (pAb->AbBox, orgTrans, frame);
		      pAb = pAb->AbNext;
		    }
		}
	    }
	  else if (absoluteMove
		   || pCurrentAb->AbHeight.DimAbRef != NULL
		   || pCurrentAb->AbHeight.DimValue != 0)
	    {
	      /* the box is already built */
	      /* or it's within a stretchable box */
	      /* or it's within a unnested box */
	      /* or it doesn't inherit the size from its contents */
	      pAb = pCurrentAb->AbFirstEnclosed;
	      while (pAb != NULL)
		{
		  if (!pAb->AbDead && pAb->AbBox != NULL)
		    {
		      box = pAb->AbBox;
		      /* check if the position box depends on its enclosing */
		      pPosRel = box->BxPosRelations;
		      while (pPosRel != NULL)
			{
			  i = 0;
			  notEmpty = (pPosRel->PosRTable[i].ReBox != NULL);
			  while (i < MAX_RELAT_POS && notEmpty)
			    {
			      pRelation = &pPosRel->PosRTable[i];
			      if (pRelation->ReOp == OpVertInc
				  && pRelation->ReRefEdge != HorizRef)
				{
				  switch (pAb->AbVertPos.PosRefEdge)
				    {
				    case HorizMiddle:
				      if (absoluteMove)
					j = middleTrans;
				      else
					/* recheck the position */
					j = (pBox->BxHeight - box->BxHeight) / 2 - box->BxYOrg;
				      if (box->BxVertFlex)
					MoveBoxEdge (box, pBox, pRelation->ReOp, j, frame, FALSE);
				      else
					YMove (box, pBox, j, frame);
				      break;
				    case Bottom:
				      if (absoluteMove)
					j = endTrans;
				      else
					/* recheck the position */
					j = pBox->BxHeight - box->BxHeight - box->BxYOrg;
				      if (box->BxVertFlex)
					MoveBoxEdge (box, pBox, pRelation->ReOp, j, frame, FALSE);
				      else
					YMove (box, pBox, j, frame);
				      break;
				    default:
				      if (absoluteMove)
					j = orgTrans;
				      else
					/* recheck the position */
					j = 0;
				      if (box->BxVertFlex)
					MoveBoxEdge (box, pBox, pRelation->ReOp, j, frame, FALSE);
				      else
					YMove (box, pBox, j, frame);
				      break;
				    }
				}
			      i++;
			      if (i < MAX_RELAT_POS)
				notEmpty = pPosRel->PosRTable[i].ReBox != NULL;
			    }
			  /* next relation block */
			  pPosRel = pPosRel->PosRNext;
			}
		    }
		  /* next child */
		  pAb = pAb->AbNext;
		}
	    }
	  
	  /* check dimension constraints */
	  pDimRel = pBox->BxHeightRelations;
	  while (pDimRel != NULL)
	    {
	      i = 0;
	      while (i < MAX_RELAT_DIM && pDimRel->DimRTable[i] != NULL)
		{
		  box = pDimRel->DimRTable[i];
		  pAb = box->BxAbstractBox;		    
		  /* Is it the same dimension? */
		  if (pDimRel->DimRSame[i])
		    {
		      /* Changing the height */
		      if (pAb->AbHeight.DimUnit == UnPercent)
			{
			  if (pAb->AbEnclosing == pCurrentAb)
			    {
			      if (pCurrentAb->AbInLine || pBox->BxType == BoGhost)
				{
				/* inherit from the line height */
				  pLine = SearchLine (box);
				  if (pLine == NULL)
				    /* no line available */
				    val = box->BxHeight;
				  else
				    val = pLine->LiHeight;
				}
			      else
				/* refer the inside height */
				val = pBox->BxH;
			    }
			  else
			    /* refer the outside height */
			    val = pBox->BxHeight;
			  val = val * pAb->AbHeight.DimValue / 100;
			  val = val - box->BxHeight;
			}
		      else
			{
			  val = delta;
			  if (pAb->AbEnclosing != pCurrentAb)
			    /* refer the outside width */
			    val = val + t + b;
			}
		      /* avoid cycles on the same box */
		      if (box != pBox)
			ChangeHeight (box, pSourceBox, pBox, val, frame);
		    }
		  else
		    {
		      /* Changing the width */
		      if (pAb->AbWidth.DimUnit == UnPercent)
			{
			  if (pAb->AbEnclosing == pCurrentAb || pAb == pCurrentAb)
			    /* refer the inside height */
			    val = pBox->BxH;
			  else
			    /* refer the outside height */
			    val = pBox->BxHeight;
			  val = val * pAb->AbWidth.DimValue / 100;
			  val = val - box->BxWidth;
			}
		      else
			{
			  val = delta;
			  if (pAb->AbEnclosing != pCurrentAb)
			    /* refer the outside width */
			    val = val + t + b;
			}
		      ChangeWidth (box, pSourceBox, NULL, val, 0, frame);
		    }
		  i++;
		  if (i < MAX_RELAT_DIM)
		    box = pDimRel->DimRTable[i];
		}
	      /* next relation block */
	      pDimRel = pDimRel->DimRNext;
	    }
	  
	  /* Check enclosing constraints */
	  pAb = pCurrentAb->AbEnclosing;
	  if (!toMove)
	    {
	      /* look for the enclosing block of lines  */
	      while (!pAb->AbInLine || pAb->AbBox->BxType == BoGhost)
		pAb = pAb->AbEnclosing;
	    }
	  
	  if (pAb == NULL)
	    {
	      /* It's the root of the concrete image */
	      if (Propagate == ToAll && pBox->BxYOrg < 0)
		YMoveAllEnclosed (pBox, -pBox->BxYOrg, frame);
	    }
	  else if (pCurrentAb->AbVertEnclosing)
	    {
	      /* keep in mind if the relation concerns parent boxes */
	      externalRef = !IsParentBox (pAb->AbBox, pSourceBox);
	      
	      /*
	       * if pSourceBox is a child and the inclusion is not performed
	       * by another sibling box, we need to propagate the change
	       */
	      if ((Propagate == ToAll || externalRef)
		  && !IsSiblingBox (pBox, pFromBox)
		  && !IsSiblingBox (pBox, pSourceBox))
		{
		  if (pAb->AbInLine)
		    /* Within a block of line */
		    EncloseInLine (pBox, frame, pAb);
		  /* if the inclusion is not checked at the end */
		  else if (!IsParentBox (pAb->AbBox, PackBoxRoot))
		    {
		      /* Differ the checking of the inclusion */
		      if (Propagate != ToAll && pAb->AbBox->BxType != BoCell)
			RecordEnclosing (pAb->AbBox, FALSE);
		      /* Don't check the inclusion more than 2 times */
		      else if (pAb->AbBox->BxPacking <= 1)
			HeightPack (pAb, pSourceBox, frame);
		    }
		}
	    }
	}
    }
  
  /* Manage the specific width of symbols */
  if (pBox != NULL)
    {
      if (pCurrentAb->AbLeafType == LtSymbol)
	{
	  i = 0;
	  font = pBox->BxFont;
	  value = 1 + ((float) (pBox->BxH * 0.6) / (float) FontHeight (font));
	  switch (pCurrentAb->AbShape)
	    {
	    case 'd':	/* double integral */
	      i = CharacterWidth (231, font) + CharacterWidth (231, font) / 2;
	    case 'i':	/* integral */
	    case 'c':	/* circle integral */
	      i = (int) ((float) CharacterWidth (231, font) * value);
	      ResizeWidth (pBox, NULL, NULL, i - pBox->BxW, 0, 0, 0, frame);
	      break;
	    case '(':
	    case ')':
	    case '{':
	    case '}':
	    case '[':
	    case ']':
	      i = (int) ((float) CharacterWidth (230, font) * value);
	      ResizeWidth (pBox, NULL, NULL, i - pBox->BxW, 0, 0, 0, frame);
	      break;
	    default:
	      break;
	    }
	}
    }
}


/*----------------------------------------------------------------------
  XMove moves horizontally the box.
  Check positionning constraints.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                XMove (PtrBox pBox, PtrBox pFromBox, int delta, int frame)
#else  /* __STDC__ */
void                XMove (pBox, pFromBox, delta, frame)
PtrBox              pBox;
PtrBox              pFromBox;
int                 delta;
int                 frame;

#endif /* __STDC__ */
{
   PtrBox              box;
   PtrAbstractBox      pAb;
   PtrAbstractBox      pCurrentAb;
   PtrPosRelations     pPosRel;
   BoxRelation        *pRelation;
   int                 i, j, k;
   ThotBool            toComplete;
   ThotBool            notEmpty;
   ThotBool            checkParent;
   ThotBool            absoluteMove;

   if (pBox != NULL && delta != 0)
     {
	pCurrentAb = pBox->BxAbstractBox;
	if (pCurrentAb != NULL)
	   if (!pCurrentAb->AbDead)
	     {
		/* check if the box is in the history of moved boxes */
		if (pFromBox != NULL)
		  {
		     box = pFromBox->BxMoved;
		     while (box != NULL)
		       if (box == pBox)
			 return;
		       else if (box->BxMoved == pFromBox)
			 {
printf ("XMove, box already in the list (label=%s, type=%d)\n", box->BxAbstractBox->AbElement->ElLabel,  box->BxAbstractBox->AbElement->ElTypeNumber);
			 box = NULL;
			 }
		       else
			 box = box->BxMoved;
		  }
		/* add the box in the history */
		pBox->BxMoved = pFromBox;
		/* keep in mind that the box should be placed */
		toComplete = pBox->BxXToCompute;
		/* register the window area to be redisplayed */
		if (ReadyToDisplay)
		  {
		     if (pBox->BxType != BoSplit)
		       {
			  i = pBox->BxXOrg;
			  j = pBox->BxXOrg + pBox->BxWidth;
			  /* add margins for graphics */
			  if (pBox->BxAbstractBox->AbLeafType == LtGraphics)
			     k = EXTRA_GRAPH;
			  else
			     k = 0;
			  if (delta > 0)
			     j += delta;
			  else
			     i += delta;
			  if (pBox->BxHeight > 0)
			     DefClip (frame, i - k, pBox->BxYOrg - k, j + k, pBox->BxYOrg + pBox->BxHeight + k);
		       }
		     /* Is the box not included? */
		     else if (!pCurrentAb->AbVertEnclosing)
			if (delta > 0)
			   DefClip (frame, pBox->BxXOrg - delta, pBox->BxYOrg,
				    pBox->BxXOrg + pBox->BxWidth, pBox->BxYOrg + pBox->BxHeight);
			else
			   DefClip (frame, pBox->BxXOrg, pBox->BxYOrg, pBox->BxXOrg +
				    pBox->BxWidth - delta, pBox->BxYOrg + pBox->BxHeight);
		  }

		/* Keep in mind if the box positionning is absolute or not */
		absoluteMove = IsXPosComplete (pBox);
		/*
		 * Move only the origin if we're building the box
		 * and it's not a stretchable box.
		 * In other cases, move also enclosed boxes.
		 */
		if (absoluteMove)
		  {
		     XMoveAllEnclosed (pBox, delta, frame);
		     /* we could clean up the history -> restore it */
		     pBox->BxMoved = pFromBox;
		     /* avoid cycles on the same boxes */
		     box = pFromBox;
		     while (box != NULL)
			if (box->BxMoved == pFromBox)
			  {
			     box->BxMoved = NULL;
			     box = NULL;
			  }
			else
			   box = box->BxMoved;
		  }
		else
		   pBox->BxXOrg += delta;

		/* Check the validity of dependency rules */
		checkParent = TRUE;
		if (pCurrentAb->AbEnclosing != NULL)
		  if (pCurrentAb->AbEnclosing->AbBox != NULL)
		    checkParent = (pCurrentAb->AbEnclosing->AbBox->BxType != BoGhost);

		/* Move remaining dependent boxes */
		pPosRel = pBox->BxPosRelations;
		while (pPosRel != NULL)
		  {
		    i = 0;
		    notEmpty = (pPosRel->PosRTable[i].ReBox != NULL);
		    while (i < MAX_RELAT_POS && notEmpty)
		      {
			pRelation = &pPosRel->PosRTable[i];
			if (pRelation->ReBox->BxAbstractBox != NULL
			    && pRelation->ReBox->BxType != BoGhost)
			  {
			    /* Left, Right, Middle, and Vertical axis */
			    if (pRelation->ReOp == OpHorizRef)
			      {
				/* except its vertical axis */
				if (pRelation->ReBox != pBox)
				  {
				    pAb = pCurrentAb->AbEnclosing;
				    if (pAb != NULL)
				      box = pAb->AbBox;
				    else
				      box = NULL;
				    if (pRelation->ReBox != box || Propagate == ToAll)
				      MoveVertRef (pRelation->ReBox, pBox, delta, frame);
				  }
			      }
			    /* Ignore the back relation of a stretchable box */
			    else if (pBox->BxHorizFlex
				     && pCurrentAb != pRelation->ReBox->BxAbstractBox->AbHorizPos.PosAbRef)
				;
			    /*
			     * Don't move boxes which have unnested relations
			     * with the moved box.
			     * Don't update dimensions of stretchable boxes
			     * if they are already managed by XMoveAllEnclosed.
			     */
			    else if (absoluteMove)
			      {
				if (!pBox->BxHorizFlex || toComplete)
				  {
				    /* Managed by XMoveAllEnclosed */
				    if (pRelation->ReOp == OpHorizDep && !pRelation->ReBox->BxXOutOfStruct)
				      /* Valid relation with the box origin */
				      if (pRelation->ReBox->BxHorizFlex
					  /* if it's not a child */
					  && pCurrentAb != pRelation->ReBox->BxAbstractBox->AbEnclosing
					  && pCurrentAb == pRelation->ReBox->BxAbstractBox->AbHorizPos.PosAbRef)
					MoveBoxEdge (pRelation->ReBox, pBox, pRelation->ReOp, delta, frame, TRUE);
				      else
					XMove (pRelation->ReBox, pBox, delta, frame);
				  }
			      }
			    else if (pRelation->ReOp == OpHorizDep && !pRelation->ReBox->BxHorizFlex)
			      XMove (pRelation->ReBox, pBox, delta, frame);
			    else if (((pRelation->ReOp == OpHorizDep
				       && pCurrentAb == pRelation->ReBox->BxAbstractBox->AbHorizPos.PosAbRef))
				     || pRelation->ReOp == OpWidth)
			      MoveBoxEdge (pRelation->ReBox, pBox, pRelation->ReOp, delta, frame, TRUE);
			  }
			
			/* we could clean up the history -> restore it */
			pBox->BxMoved = pFromBox;
			i++;
			if (i < MAX_RELAT_POS)
			  notEmpty = pPosRel->PosRTable[i].ReBox != NULL;
		      }
		    /* next relation block */
		    pPosRel = pPosRel->PosRNext;
		  }

		/* Do we have to recompute the width of the enclosing box */
		pAb = pCurrentAb->AbEnclosing;
		if (checkParent && pBox->BxXOutOfStruct && pAb != NULL)
		   /*
		    * cannot compute it if this box is not placed
		    * or if the management is differed.
		    */
		   if (!pAb->AbBox->BxXToCompute
		       && !IsParentBox (pAb->AbBox, PackBoxRoot)
		       && (Propagate == ToAll || !IsParentBox (pAb->AbBox, pFromBox))
		       && !IsSiblingBox (pBox, pFromBox)
		       && pCurrentAb->AbHorizEnclosing)
		     /*
		      * if pSourceBox is a child
		      * and the inclusion is not performed
		      * by another sibling box
		      * and the box is included
		      * we need to propagate the change
		      */
		      WidthPack (pAb, pFromBox, frame);
	     }
     }
}

/*----------------------------------------------------------------------
  YMove moves vertically the box.
  Check positionning constraints.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                YMove (PtrBox pBox, PtrBox pFromBox, int delta, int frame)
#else  /* __STDC__ */
void                YMove (pBox, pFromBox, delta, frame)
PtrBox              pBox;
PtrBox              pFromBox;
int                 delta;
int                 frame;

#endif /* __STDC__ */
{
   PtrBox              box;
   PtrAbstractBox      pAb;
   PtrAbstractBox      pCurrentAb;
   PtrPosRelations     pPosRel;
   BoxRelation        *pRelation;
   int                 i, j, k;
   ThotBool            toComplete;
   ThotBool            notEmpty;
   ThotBool            checkParent;
   ThotBool            absoluteMove;

   if (pBox != NULL && delta != 0)
     {
	pCurrentAb = pBox->BxAbstractBox;
	if (pCurrentAb != NULL)
	   if (!pCurrentAb->AbDead)
	     {
		/* check if the box is in the history of moved boxes */
		if (pFromBox != NULL)
		  {
		    box = pFromBox->BxMoved;
		    while (box != NULL)
		      if (box == pBox)
			return;
		      else if (box->BxMoved == pFromBox)
			{
printf ("YMove, box already in the list (label=%s, type=%d)\n", box->BxAbstractBox->AbElement->ElLabel,  box->BxAbstractBox->AbElement->ElTypeNumber);
			  box = NULL;
			}
		      else
			box = box->BxMoved;
		  }
		/* met a jour la pile des boites traitees */
		pBox->BxMoved = pFromBox;
		/* keep in mind that the box should be placed */
		toComplete = pBox->BxYToCompute;
		/* register the window area to be redisplayed */
		if (ReadyToDisplay)
		  {
		     if (pBox->BxType != BoSplit)
		       {
			  i = pBox->BxYOrg;
			  j = pBox->BxYOrg + pBox->BxHeight;
			  /* add margins for graphics */
			  if (pBox->BxAbstractBox->AbLeafType == LtGraphics)
			     k = EXTRA_GRAPH;
			  else
			     k = 0;
			  if (delta > 0)
			     j += delta;
			  else
			     i += delta;
			  if (pBox->BxWidth > 0 || k > 0)
			     DefClip (frame, pBox->BxXOrg - k, i - k, pBox->BxXOrg + pBox->BxWidth + k, j + k);
		       }
		     /* Is the box not included? */
		     else if (!pCurrentAb->AbHorizEnclosing)
			if (delta > 0)
			   DefClip (frame, pBox->BxXOrg, pBox->BxYOrg - delta,
				    pBox->BxXOrg + pBox->BxWidth, pBox->BxYOrg + pBox->BxHeight);
			else
			   DefClip (frame, pBox->BxXOrg, pBox->BxYOrg, pBox->BxXOrg +
				    pBox->BxWidth, pBox->BxYOrg + pBox->BxHeight - delta);
		  }

		/* Keep in mind if the box positionning is absolute or not */
		absoluteMove = IsYPosComplete (pBox);
		/*
		 * Move only the origin if we're building the box
		 * and it's not a stretchable box.
		 * In other cases, move also enclosed boxes.
		 */
		if (absoluteMove)
		  {
		     YMoveAllEnclosed (pBox, delta, frame);
		     /* we could clean up the history -> restore it */
		     pBox->BxMoved = pFromBox;
		     /* avoid cycles on the same boxes */
		     box = pFromBox;
		     while (box != NULL)
			if (box->BxMoved == pFromBox)
			  {
			     box->BxMoved = NULL;
			     box = NULL;
			  }
			else
			   box = box->BxMoved;
		  }
		else
		   pBox->BxYOrg += delta;

		/* Check the validity of dependency rules */
		checkParent = TRUE;
		if (pCurrentAb->AbEnclosing != NULL)
		  if (pCurrentAb->AbEnclosing->AbBox != NULL)
		    checkParent = (pCurrentAb->AbEnclosing->AbBox->BxType != BoGhost);

		/* Move remaining dependent boxes */
		pPosRel = pBox->BxPosRelations;
		while (pPosRel != NULL)
		  {
		    i = 0;
		    notEmpty = (pPosRel->PosRTable[i].ReBox != NULL);
		    while (i < MAX_RELAT_POS && notEmpty)
		      {
			pRelation = &pPosRel->PosRTable[i];
			if (pRelation->ReBox->BxAbstractBox != NULL
			    && pRelation->ReBox->BxType != BoGhost)
			  {
			    /* Top, Bottom, Middle, and Baseline */
			    if (pRelation->ReOp == OpVertRef)
			      {
				/* except its baseline */
				if (pRelation->ReBox != pBox)
				  {
				    pAb = pCurrentAb->AbEnclosing;
				    if (pAb != NULL)
				      box = pAb->AbBox;
				    else
				      box = NULL;
				    if (pRelation->ReBox != box || Propagate == ToAll)
				      MoveHorizRef (pRelation->ReBox, pBox, delta, frame);
				  }
			      }
			    /* Ignore the back relation of a stretchable box */
			    else if (pBox->BxVertFlex
				     && pCurrentAb != pRelation->ReBox->BxAbstractBox->AbVertPos.PosAbRef)
				;
			    /*
			     * Don't move boxes which have unnested relations
			     * with the moved box.
			     * Don't update dimensions of stretchable boxes
			     * if they are already managed by YMoveAllEnclosed.
			     */
			    else if (absoluteMove)
			      {
				if (!pBox->BxVertFlex || toComplete)
				  {
				    /* Managed by YMoveAllEnclosed */
				    if (pRelation->ReOp == OpVertDep && !pRelation->ReBox->BxYOutOfStruct)
				      /* Valid relation with the box origin */
				      if (pRelation->ReBox->BxVertFlex
					  /* if it's not a child */
					  && pCurrentAb != pRelation->ReBox->BxAbstractBox->AbEnclosing
					  && pCurrentAb == pRelation->ReBox->BxAbstractBox->AbVertPos.PosAbRef)
					MoveBoxEdge (pRelation->ReBox, pBox, pRelation->ReOp, delta, frame, FALSE);
				      else
					YMove (pRelation->ReBox, pBox, delta, frame);
				  }
			      }
			    else if (pRelation->ReOp == OpVertDep && !pRelation->ReBox->BxVertFlex)
			      YMove (pRelation->ReBox, pBox, delta, frame);
			    else if ((pRelation->ReOp == OpVertDep
				      && pCurrentAb == pRelation->ReBox->BxAbstractBox->AbVertPos.PosAbRef)
				     || pRelation->ReOp == OpHeight)
			      MoveBoxEdge (pRelation->ReBox, pBox, pRelation->ReOp, delta, frame, FALSE);
			  }
			
			/* we could clean up the history -> restore it */
			pBox->BxMoved = pFromBox;
			i++;
			if (i < MAX_RELAT_POS)
			  notEmpty = pPosRel->PosRTable[i].ReBox != NULL;
		      }
		    /* next relation block */
		    pPosRel = pPosRel->PosRNext;
		  }

		/* Do we have to recompute the height of the enclosing box */
		pAb = pCurrentAb->AbEnclosing;
		if (checkParent && pBox->BxYOutOfStruct && pAb != NULL)
		   /*
		    * cannot compute it if this box is not placed
		    * or if the management is differed.
		    */
		   if (!pAb->AbBox->BxYToCompute
		       && !IsParentBox (pAb->AbBox, PackBoxRoot)
		       && (Propagate == ToAll || !IsParentBox (pAb->AbBox, pFromBox))
		       && !IsSiblingBox (pBox, pFromBox)
		       && pCurrentAb->AbVertEnclosing)
		     /*
		      * if pSourceBox is a child
		      * and the inclusion is not performed
		      * by another sibling box
		      * and the box is included
		      * we need to propagate the change
		      */
		      HeightPack (pAb, pFromBox, frame);
	     }
     }
}

/*----------------------------------------------------------------------
  WidthPack checks the horizontal inclusion of nested boxes.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                WidthPack (PtrAbstractBox pAb, PtrBox pSourceBox, int frame)
#else  /* __STDC__ */
void                WidthPack (pAb, pSourceBox, frame)
PtrAbstractBox      pAb;
PtrBox              pSourceBox;
int                 frame;

#endif /* __STDC__ */
{
  PtrAbstractBox      pChildAb;
  PtrAbstractBox      pRelativeAb;
  PtrAbstractBox      pRefAb;
  PtrBox              pChildBox;
  PtrBox              pRelativeBox;
  PtrBox              pBox;
  AbDimension        *pDimAb;
  AbPosition         *pPosAb;
  int                 val, width;
  int                 x, i, j, k;
  ThotBool            notEmpty;
  ThotBool            toMove;
  ThotBool            absoluteMove;

  /*
   * Check if the width depends on the box contents
   * and if we're not already doing the job
   */
  pBox = pAb->AbBox;
  pDimAb = &pAb->AbWidth;
  if (pBox->BxType == BoBlock || pBox->BxType == BoCell)
    /* don't pack a block or a cell but transmit to enclosing box */
    WidthPack (pAb->AbEnclosing, pSourceBox, frame);
  else if (pBox->BxType == BoGhost)
    return;
  else if (pBox->BxContentWidth || (!pDimAb->DimIsPosition && pDimAb->DimMinimum))
    {
      /* register that we're preforming the job */
      pBox->BxCycles += 1;
      
      /* Keep in mind if the box positionning is absolute or not */
      absoluteMove = IsXPosComplete (pBox);
      if (absoluteMove)
	x = pBox->BxXOrg + pBox->BxLMargin + pBox->BxLBorder + pBox->BxLPadding;
      else
	x = 0;
      
      /* Initially the inside left and the inside right are the equal */
      width = x;
      /* val registers the current right limit */
      val = x + pBox->BxW;
      notEmpty = FALSE;
      /* nothing is moved */
      toMove = FALSE;
      
      /*
       * The left edge of the extra left enclosed box must be stuck
       * to the inside left edge and the inside width is delimited
       * by the right edge of the extra right enclosed box.
       */
      pChildAb = pAb->AbFirstEnclosed;
      while (pChildAb != NULL)
	{
	  pChildBox = pChildAb->AbBox;
	  if (!pChildAb->AbDead && pChildBox != NULL
	      && pChildAb->AbHorizEnclosing
	      && (pChildAb->AbWidth.DimAbRef != pAb || pChildBox->BxContentWidth))
	    {
	      /* look for the box which relies the box to its enclosing */
	      pRelativeBox = GetHPosRelativePos (pChildBox, NULL);
	      if (pRelativeBox != NULL)
		if (pRelativeBox->BxAbstractBox != NULL)
		  {
		    pRelativeAb = pRelativeBox->BxAbstractBox;
		    /* register the abstract box which gives the width */
		    pRefAb = pChildAb->AbWidth.DimAbRef;
		    if (pRelativeAb->AbHorizPos.PosAbRef == NULL)
		      {
			/* mobile box */
			notEmpty = TRUE;
			if (pChildBox->BxXOrg < val)
			  /* minimum value */
			  val = pChildBox->BxXOrg;
			i = pChildBox->BxXOrg + pChildBox->BxWidth;
		      }
		    /* the box position depends on the enclosing width? */
		    else if (pRelativeAb->AbHorizPos.PosAbRef == pAb
			     && (pRelativeAb->AbHorizPos.PosRefEdge != Left
				 || (pRelativeAb->AbWidth.DimAbRef == pAb
				     && !pRelativeAb->AbWidth.DimIsPosition
				     && pChildAb->AbHorizPos.PosRefEdge != Left)))
		      i = x + pChildBox->BxWidth;
		    /* the box width depends on a not included box? */
		    else if (pChildBox->BxWOutOfStruct)
		      {
			/* which itself inherits form the enclosing? */
			if (IsParentBox (pRefAb->AbBox, pBox)
			    && pRefAb->AbWidth.DimAbRef == NULL
			    && pRefAb->AbWidth.DimValue == 0
			    && pRefAb->AbBox->BxWidth == pBox->BxWidth)
			  i = x;
			else if (pChildBox->BxXOrg < x)
			  /* don't take into account negative origins */
			  i = x + pChildBox->BxWidth;
			else
			  i = pChildBox->BxXOrg + pChildBox->BxWidth;
		      }
		    else if (pChildBox->BxXOrg < x)
		      /* don't take into account negative origins */
		      i = x + pChildBox->BxWidth;
		    else
		      i = pChildBox->BxXOrg + pChildBox->BxWidth;
		    if (i > width)
		      width = i;
		  }
	    }
	  pChildAb = pChildAb->AbNext;
	}
      
      val = -val + x; /* Shift of the extra left edge */
      if (notEmpty)
	width += val; /* Nex position of the extra right edge */
      if (width == x && pAb->AbVolume == 0)
	GiveTextSize (pAb, &width, &x, &i);
      else
	width -= x;
      x = width - pBox->BxW; /* widths difference */
      
      /*
       * Now we move misplaced included boxes
       */
      pChildAb = pAb->AbFirstEnclosed;
      if (notEmpty && val != 0)
	while (pChildAb != NULL)
	  {
	    pChildBox = pChildAb->AbBox;
	    if (!pChildAb->AbDead && pChildBox != NULL && pChildAb->AbHorizEnclosing)
	      {
		
		/* look for the box which relies the box to its enclosing */
		pRelativeBox = GetHPosRelativePos (pChildBox, NULL);
		if (pRelativeBox != NULL)
		  if (pRelativeBox->BxAbstractBox != NULL)
		    if (pRelativeBox->BxAbstractBox->AbHorizPos.PosAbRef == NULL)
		      {
			/* update the clipping area for the future */
			if (ReadyToDisplay)
			  {
			    /* Add extra margins for graphics */
			    if (pChildBox->BxAbstractBox->AbLeafType == LtGraphics)
			      k = EXTRA_GRAPH;
			    else
			      k = 0;
			    i = pChildBox->BxXOrg;
			    j = pChildBox->BxXOrg + pChildBox->BxWidth;
			    if (val > 0)
			      j += val;
			    else
			      i += val;
			    DefClip (frame, i - k, pChildBox->BxYOrg - k, j + k, pChildBox->BxYOrg + pChildBox->BxHeight + k);
			  }
			
			if (IsXPosComplete (pChildBox))
			  /* move all included boxes */
			  XMoveAllEnclosed (pChildBox, val, frame);
			else
			  pChildBox->BxXOrg += val;
			
		        /* Does it move the enclosing box? */
			pPosAb = &pAb->AbVertRef;
			if (pPosAb->PosAbRef == pChildAb)
			  {
			    toMove = TRUE;
			    pChildBox->BxMoved = NULL;
			    if (pPosAb->PosRefEdge != VertRef)
			      MoveVertRef (pBox, pChildBox, val, frame);
			    else
			      {
				/* change the baseline */
				if (pPosAb->PosUnit == UnPercent)
				  i = PixelValue (pPosAb->PosDistance, UnPercent, (PtrAbstractBox) pAb->AbBox->BxW, 0);
				else
				  i = PixelValue (pPosAb->PosDistance, pPosAb->PosUnit, pAb, ViewFrameTable[frame - 1].FrMagnification);
				i = i + pChildBox->BxXOrg + pChildBox->BxVertRef - pBox->BxXOrg;
				MoveVertRef (pBox, pChildBox, i - pBox->BxVertRef, frame);
			      }
			  }
		      }
	      }
	    pChildAb = pChildAb->AbNext;
	  }
      
      if (x != 0)
	/* update the enclosing box width */
	ChangeDefaultWidth (pBox, pSourceBox, width, 0, frame);
      /* Now check the ascestor box */
      else if (toMove)
	if (pAb->AbEnclosing == NULL)
	  {
	    if (pBox->BxXOrg < 0)
	      XMoveAllEnclosed (pBox, -pBox->BxXOrg, frame);
	  }
	else if (!pAb->AbEnclosing->AbInLine
		 && pAb->AbEnclosing->AbBox->BxType != BoGhost)
	  WidthPack (pAb->AbEnclosing, pSourceBox, frame);
      
      /* the job is performed */
      pBox->BxCycles -= 1;
    }
  else if (!pDimAb->DimIsPosition && pDimAb->DimMinimum)
    {
      /*
       * the minimum rule is applied, we need to check whether
       * that minimum is still true.
       */
      GiveEnclosureSize (pAb, frame, &width, &val);
      ChangeDefaultWidth (pBox, pSourceBox, width, 0, frame);
    }
  else if (!pDimAb->DimIsPosition && pDimAb->DimAbRef == pAb->AbEnclosing &&
	   pAb->AbEnclosing != NULL &&
	   !pAb->AbEnclosing->AbWidth.DimIsPosition &&
	   pAb->AbEnclosing->AbWidth.DimMinimum)
    {
      /*
       * the box width depends on the parent box for which the minimum rule
       * is applied, we need to check whether that minimum is still true.
       */
      GiveEnclosureSize (pAb, frame, &width, &val);
      ChangeDefaultWidth (pAb->AbEnclosing->AbBox, pSourceBox, width, 0, frame);
    }
}

/*----------------------------------------------------------------------
  HeightPack checks the vertical inclusion of nested boxes.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                HeightPack (PtrAbstractBox pAb, PtrBox pSourceBox, int frame)
#else  /* __STDC__ */
void                HeightPack (pAb, pSourceBox, frame)
PtrAbstractBox      pAb;
PtrBox              pSourceBox;
int                 frame;

#endif /* __STDC__ */
{
  int                 val, height;
  int                 y, i, j, k;
  PtrAbstractBox      pChildAb;
  PtrAbstractBox      pRelativeAb;
  PtrAbstractBox      pRefAb;
  PtrBox              pChildBox;
  PtrBox              pRelativeBox;
  PtrBox              pBox;
  AbDimension        *pDimAb;
  AbPosition         *pPosAb;
  ThotBool            notEmpty;
  ThotBool            toMove;
  ThotBool            absoluteMove;

  /*
   * Check if the height depends on the box contents
   * and if we're not already doing the job
   */
  pBox = pAb->AbBox;
  pDimAb = &pAb->AbHeight;
  if (pBox->BxType == BoBlock)
    /* don't pack a block or a cell but transmit to enclosing box */
    HeightPack (pAb->AbEnclosing, pSourceBox, frame);
  else if (pBox->BxType == BoGhost)
    return;
  else if (pBox->BxContentHeight || (!pDimAb->DimIsPosition && pDimAb->DimMinimum))
    {
      
      /* register that we're preforming the job */
      pBox->BxPacking += 1;
      
      /* Keep in mind if the box positionning is absolute or not */
      absoluteMove = IsYPosComplete (pBox);
      if (absoluteMove)
	y = pBox->BxYOrg + pBox->BxTMargin + pBox->BxTBorder + pBox->BxTPadding;
      else
	y = 0;
      
      /* Initially the inside top and the inside bottom are the equal */
      height = y;
      /* val registers the current bottom limit */
      val = y + pBox->BxH;
      notEmpty = FALSE;
      /* nothing is moved */
      toMove = FALSE;
      
      /*
       * The top edge of the extra upper enclosed box must be stuck
       * to the inside top edge and the inside height is delimited
       * by the bottom edge of the extra lower enclosed box.
       */
      pChildAb = pAb->AbFirstEnclosed;
      while (pChildAb != NULL)
	{
	  pChildBox = pChildAb->AbBox;
	  if (!pChildAb->AbDead
	      && pChildBox != NULL
	      && pChildAb->AbVertEnclosing
	      && (pChildAb->AbHeight.DimAbRef != pAb || pChildBox->BxContentHeight))
	    {
	      /* look for the box which relies the box to its enclosing */
	      pRelativeBox = GetVPosRelativeBox (pChildBox, NULL);
	      if (pRelativeBox != NULL)
		if (pRelativeBox->BxAbstractBox != NULL)
		  {
		    pRelativeAb = pRelativeBox->BxAbstractBox;
		    /* register the abstract box which gives the height */
		    pRefAb = pChildAb->AbHeight.DimAbRef;
		    if (pRelativeAb->AbVertPos.PosAbRef == NULL)
		      {
			/* mobile box */
			notEmpty = TRUE;
			/* minimum value */
			if (pChildBox->BxYOrg < val)
			  val = pChildBox->BxYOrg;
			i = pChildBox->BxYOrg + pChildBox->BxHeight;
		      }
		    /* the box position depends on the enclosing height? */
		    else if (pRelativeAb->AbVertPos.PosAbRef == pAb
			     && (pRelativeAb->AbVertPos.PosRefEdge != Top
				 || (pRelativeAb->AbHeight.DimAbRef == pAb
				     && !pRelativeAb->AbHeight.DimIsPosition
				     && pChildAb->AbVertPos.PosRefEdge != Top)))
		      i = y + pChildBox->BxHeight;
		    /* the box height depends on a not included box? */
		    else if (pChildBox->BxHOutOfStruct)
		      {
			/* which itself inherits form the enclosing? */
			if (IsParentBox (pRefAb->AbBox, pBox)
			    && pRefAb->AbHeight.DimAbRef == NULL
			    && pRefAb->AbHeight.DimValue == 0
			    && pRefAb->AbBox->BxHeight == pBox->BxHeight)
			  i = y;
			else if (pChildBox->BxYOrg < y)
			  /* don't take into account negative origins */
			  i = y + pChildBox->BxHeight;
			else
			  i = pChildBox->BxYOrg + pChildBox->BxHeight;
		      }
		    else if (pChildBox->BxYOrg < y)
		      /* don't take into account negative origins */
		      i = y + pChildBox->BxHeight;
		    else
		      i = pChildBox->BxYOrg + pChildBox->BxHeight;
		    if (i > height)
		      height = i;
		  }
	    }
	  pChildAb = pChildAb->AbNext;
	}
      
      val = -val + y; /* Shift of the extra top edge */
      if (notEmpty)
	height += val; /* Nex position of the extra bottom edge */
      if (height == y && pAb->AbVolume == 0)
	GiveTextSize (pAb, &y, &height, &i);
      else
	height -= y;
      y = height - pBox->BxH; /* heights difference */
      
      /*
       * Now we move misplaced included boxes
       */
      pChildAb = pAb->AbFirstEnclosed;
      if (notEmpty && val != 0)
	while (pChildAb != NULL)
	  {
	    pChildBox = pChildAb->AbBox;
	    if (!pChildAb->AbDead && pChildBox != NULL && pChildAb->AbVertEnclosing)
	      {
		/* look for the box which relies the box to its enclosing */
		pRelativeBox = GetVPosRelativeBox (pChildBox, NULL);
		if (pRelativeBox != NULL)
		  if (pRelativeBox->BxAbstractBox != NULL)
		    if (pRelativeBox->BxAbstractBox->AbVertPos.PosAbRef == NULL)
		      {
			/* update the clipping area for the redisplay */
			if (ReadyToDisplay)
			  {
			    i = pChildBox->BxYOrg;
			    j = pChildBox->BxYOrg + pChildBox->BxHeight;
			    /* Add extra margins for graphics */
			    if (pChildBox->BxAbstractBox->AbLeafType == LtGraphics)
			      k = EXTRA_GRAPH;
			    else
			      k = 0;
			    if (val > 0)
			      j += val;
			    else
			      i += val;
			    DefClip (frame, pChildBox->BxXOrg - k, i - k, pChildBox->BxXOrg
				     + pChildBox->BxWidth + k, j + k);
			  }
			
			if (IsYPosComplete (pChildBox))
			  /* move all included boxes */
			  YMoveAllEnclosed (pChildBox, val, frame);
			else
			  pChildBox->BxYOrg += val;
			
		        /* Does it move the enclosing box? */
			pPosAb = &pAb->AbHorizRef;
			if (pPosAb->PosAbRef == pChildAb)
			  {
			    toMove = TRUE;
			    pChildBox->BxMoved = NULL;
			    if (pPosAb->PosRefEdge != HorizRef)
			      MoveHorizRef (pBox, pChildBox, val, frame);
			    else
			      {
				/* change the vertical axis */
				if (pPosAb->PosUnit == UnPercent)
				  i = PixelValue (pPosAb->PosDistance, UnPercent, (PtrAbstractBox) pAb->AbBox->BxH, 0);
				else
				  i = PixelValue (pPosAb->PosDistance, pPosAb->PosUnit, pAb, ViewFrameTable[frame - 1].FrMagnification);
				i = i + pChildBox->BxYOrg + pChildBox->BxHorizRef - pBox->BxYOrg;
				MoveHorizRef (pBox, pChildBox, i - pBox->BxHorizRef, frame);
			      }
			  }
		      }
	      }
	    pChildAb = pChildAb->AbNext;
	  }
      
      if (y != 0)
	/* update the enclosing box height */
	ChangeDefaultHeight (pBox, pSourceBox, height, frame);
      /* Now check the ascestor box */
      else if (toMove)
	if (pAb->AbEnclosing == NULL)
	  {
	    if (pBox->BxYOrg < 0)
	      YMoveAllEnclosed (pBox, -pBox->BxYOrg, frame);
	  }
	else if (pAb->AbEnclosing->AbInLine)
	  EncloseInLine (pBox, frame, pAb->AbEnclosing);
	else if (pAb->AbEnclosing->AbBox->BxType == BoGhost ||
		 pAb->AbEnclosing->AbBox->BxType == BoBlock)
	  {
	    /* Il faut remonter au pave de mise en lignes */
	    while (pAb->AbEnclosing->AbBox->BxType == BoGhost)
	      pAb = pAb->AbEnclosing;
	    EncloseInLine (pBox, frame, pAb->AbEnclosing);
	  }
	else
	  HeightPack (pAb->AbEnclosing, pSourceBox, frame);

      /* the job is performed */
      pBox->BxPacking -= 1;
    }
  else if (!pDimAb->DimIsPosition && pDimAb->DimMinimum)
    {
      /*
       * the minimum rule is applied, we need to check whether
       * that minimum is still true.
       */
      GiveEnclosureSize (pAb, frame, &val, &height);
      ChangeDefaultHeight (pBox, pSourceBox, height, frame);
    }
  else if (!pDimAb->DimIsPosition && pDimAb->DimAbRef == pAb->AbEnclosing &&
	   pAb->AbEnclosing != NULL &&
	   !pAb->AbEnclosing->AbHeight.DimIsPosition &&
	   pAb->AbEnclosing->AbHeight.DimMinimum)
    {
      /*
       * the box height depends on the parent box for which the minimum rule
       * is applied, we need to check whether that minimum is still true.
       */
      GiveEnclosureSize (pAb, frame, &val, &height);
      ChangeDefaultHeight (pAb->AbEnclosing->AbBox, pSourceBox, height, frame);
    }
}
