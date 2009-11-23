/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2009
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * This module maintains constraints between boxes
 *
 * Author: I. Vatton (INRIA)
 *         P. Cheyrou-lagreze (INRIA)
 */
#include "thot_gui.h"
#include "ustring.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "picture.h"
#include "appdialogue.h"
#include "frame.h"
#include "content.h"

#define THOT_EXPORT extern
#include "boxes_tv.h"
#include "appdialogue_tv.h"
#include "frame_tv.h"

#include "appli_f.h"
#include "content_f.h"
#include "contentapi_f.h"
#include "boxmoves_f.h"
#include "boxpositions_f.h"
#include "boxrelations_f.h"
#include "buildboxes_f.h"
#include "buildlines_f.h"
#include "displayview_f.h"
#include "exceptions_f.h"
#include "font_f.h"
#include "frame_f.h"
#ifdef _GL
#include "glwindowdisplay.h"
#endif /*_GL*/
#include "memory_f.h"
#include "picture_f.h"
#include "stix_f.h"
#include "tableH_f.h"
#include "windowdisplay_f.h"

/*----------------------------------------------------------------------
  IsSystemOrigin returns TRUE if the box is a system origin.
  ----------------------------------------------------------------------*/
ThotBool IsSystemOrigin (PtrAbstractBox pAb, int frame)
{
#ifdef _GL
  if (pAb && pAb->AbElement && FrameTable[frame].FrView == 1 &&
      pAb->AbBox &&
      pAb->AbElement->ElSystemOrigin)
     return TRUE;
#endif /* _GL */
  return FALSE;
}

/*----------------------------------------------------------------------
  IsXYPosComplete returns TRUE indicators when the box position is
  relative to the root box instead of the parent box.
  They could be FALSE when the box building is in progress and any
  enclosing box is stretchable or depends on an out-of-strucure box
  (not included by its ancestor).
  ----------------------------------------------------------------------*/
void IsXYPosComplete (PtrBox pBox, ThotBool *horizRef, ThotBool *vertRef)
{
  PtrBox              pParentBox;

  pParentBox = pBox;
  *vertRef = (Propagate != ToSiblings);
  *horizRef = *vertRef;
  /* Work with absolute values when out of a translated SVG */
  while ((!*vertRef || !*horizRef) && pParentBox)
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

  if (pBox->BxYToCompute &&
      (pBox->BxType == BoBlock ||
       pBox->BxType == BoFloatBlock || pBox->BxType == BoCellBlock))
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
ThotBool IsXPosComplete (PtrBox pBox)
{
  PtrBox              pParentBox;
  ThotBool            Ok;

  pParentBox = pBox;
  Ok = (Propagate != ToSiblings);
  /* Work with absolute values when out of a translated SVG */
  while (!Ok && pParentBox)
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
ThotBool IsYPosComplete (PtrBox pBox)
{
  PtrBox              pParentBox;
  ThotBool            Ok;

  pParentBox = pBox;
  Ok = (Propagate != ToSiblings);
  /* Work with absolute values when out of a translated SVG */
  while (!Ok && pParentBox)
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
ThotBool IsParentBox (PtrBox pAncestor, PtrBox pChild)
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
          // prevent a deadlock when pAb == pAb->AbEnclosing
          equal = pAb->AbBox == pAncestor || pAb == pAb->AbEnclosing;
          if (pAb->AbElement == NULL)
            pAb = NULL;
          else
            pAb = pAb->AbEnclosing;
        }
      return (equal);
    }
}


/*----------------------------------------------------------------------
  IsSiblingBox returns TRUE if pBox is a sibling (same parent but not
  pRefBox itself) of pRefBox.
  ----------------------------------------------------------------------*/
static ThotBool IsSiblingBox (PtrBox pBox, PtrBox pRefBox)
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
  MirrorPath inverts horizontally and/or vertically all points of a path.
  The parameter inAbtractBox is TRUE when the invertion concerns the
  abstract box instead of the box.
  ----------------------------------------------------------------------*/
static void MirrorPath (PtrAbstractBox pAb, ThotBool horizRef,
                        ThotBool vertRef, ThotBool inAbtractBox)
{
  /* todo */
}

/*----------------------------------------------------------------------
  MirrorPolyline inverts horizontally and/or vertically points in the polyline.
  The parameter inAbtractBox is TRUE when the invertion concerns the
  abstract box instead of the box.
  ----------------------------------------------------------------------*/
static void MirrorPolyline (PtrAbstractBox pAb, ThotBool horizRef,
                            ThotBool vertRef, ThotBool inAbtractBox)
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
      TtaFreeMemory ((STRING) pBox->BxPictInfo);
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
void MirrorShape (PtrAbstractBox pAb, ThotBool horizRef, ThotBool vertRef,
                  ThotBool inAbtractBox)
{
  PtrAbstractBox      pChildAb;
  char                shape;

  if (pAb->AbLeafType == LtCompound && !inAbtractBox)
    {
      /* Transmet le traitement aux paves fils */
      pChildAb = pAb->AbFirstEnclosed;
      while (pChildAb != NULL)
        {
          if (pChildAb->AbLeafType == LtGraphics ||
              pChildAb->AbLeafType == LtPolyLine ||
              pChildAb->AbLeafType == LtPath ||
              pChildAb->AbLeafType == LtCompound)
            MirrorShape (pChildAb, horizRef, vertRef, inAbtractBox);
          pChildAb = pChildAb->AbNext;
        }
    }
  else if (pAb->AbLeafType == LtPolyLine)
    MirrorPolyline (pAb, horizRef, vertRef, inAbtractBox);
  else if (pAb->AbLeafType == LtPath)
    MirrorPath (pAb, horizRef, vertRef, inAbtractBox);
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
void XEdgesExchange (PtrBox pBox, OpRelation op)
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
void YEdgesExchange (PtrBox pBox, OpRelation op)
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
void ChangeDefaultHeight (PtrBox pBox, PtrBox pSourceBox, int height,
                          int frame)
{
  int                 delta;
  ThotBool            minimumRule;

  if (pBox != NULL)
    {
      minimumRule = (!pBox->BxAbstractBox->AbHeight.DimIsPosition
                     && pBox->BxAbstractBox->AbHeight.DimMinimum);
      /* check if the current height depends on the contents */
      if (pBox->BxContentHeight)
        {
          /* the current height equals the contents height */
          if (minimumRule && height < pBox->BxRuleHeight)
            {
             /* apply the minimum rule */
              delta = pBox->BxRuleHeight - pBox->BxH;
              pBox->BxRuleHeight = height;
              pBox->BxContentHeight = !pBox->BxContentHeight;
              ResizeHeight (pBox, pSourceBox, NULL, delta, 0, 0, frame);
            }
          else
            /* update the current content height */
            ResizeHeight (pBox, pSourceBox, NULL, height - pBox->BxH, 0, 0, frame);
        }
      else if (minimumRule)
        {
          /* the current height equals the minimum */
          if (height > pBox->BxH)
            {
              /* apply the content rule */
              pBox->BxRuleHeight = pBox->BxH;
              pBox->BxContentHeight = !pBox->BxContentHeight;
              ResizeHeight (pBox, pSourceBox, NULL, height - pBox->BxH, 0, 0, frame);
            }
          else
            /* update the content height */
            pBox->BxRuleHeight = height;
        }
    }
}


/*----------------------------------------------------------------------
  TransmitRuleWidth transmits the update of rule value to nested boxes.
  ----------------------------------------------------------------------*/
static void TransmitRuleWidth (PtrBox pBox, PtrBox pSourceBox, int frame)
{
  PtrAbstractBox      pAb;
  PtrBox              box;
  PtrDimRelations     pDimRel;
  int                 i, val, width;

  if (pBox && pBox->BxAbstractBox)
    {
      pDimRel = pBox->BxWidthRelations;
      width = pBox->BxRuleWidth;
      while (pDimRel)
        {
          i = 0;
          while (i < MAX_RELAT_DIM && pDimRel->DimRTable[i] != NULL)
            {
              box = pDimRel->DimRTable[i];
              if (box)
                pAb = box->BxAbstractBox;
              else
                pAb = NULL;
              if (IsParentBox (pBox, box) && box->BxShrink &&
                  pAb && !pAb->AbDead && !pAb->AbNew && pAb->AbBox)
                {
                  /* Is it the same dimension? */
                  if (pDimRel->DimROp[i] == OpSame)
                    {
                      /* Changing the width */
                      if (pAb->AbWidth.DimUnit == UnPercent &&
                          pAb->AbWidth.DimValue != 100)
                        val = width * pAb->AbWidth.DimValue / 100;
                      else
                        {
                          // transmit the column width to table cells
                          val = width;
                          if (box->BxLMargin > 0)
                            val -= box->BxLMargin;
                          if (box->BxRMargin > 0)
                            val -= box->BxRMargin;
                          val = val - box->BxLBorder - box->BxLPadding - box->BxRBorder - box->BxRPadding;
                        }

                      /* avoid cycles on the same box */
                      if (box != pBox)
                        ChangeWidth (box, pSourceBox, pBox, val - box->BxRuleWidth, 0, frame);
                    }
                }
              i++;
            }
          /* next relation block */
          pDimRel = pDimRel->DimRNext;
        }
    }
}

/*----------------------------------------------------------------------
  ChangeDefaultWidth updates the contents box width.
  Check if the rule of the mininmum is respected and eventually exchange
  the real and the constrainted width.
  ----------------------------------------------------------------------*/
void ChangeDefaultWidth (PtrBox pBox, PtrBox pSourceBox, int width,
                         int spaceDelta, int frame)
{
  PtrAbstractBox      pAb;
  int                 delta;
  ThotBool            minimumRule;

  if (pBox && pBox->BxAbstractBox)
    {
      pAb = pBox->BxAbstractBox;
      minimumRule = (!pAb->AbWidth.DimIsPosition && pAb->AbWidth.DimMinimum);
      if (minimumRule)
        pBox->BxShrink = FALSE;
      /* check if the current width depends on the contents */
      if (pBox->BxContentWidth)
        {
          /* the current width equals the contents width */
          if (minimumRule && width < pBox->BxRuleWidth)
            {
              /* apply the minimum rule */
              delta = pBox->BxRuleWidth - pBox->BxW;
              pBox->BxRuleWidth = width;
              pBox->BxContentWidth = !pBox->BxContentWidth;
              ResizeWidth (pBox, pSourceBox, NULL, delta, spaceDelta, 0, 0, frame, FALSE);
            }
          else
            /* update the current content width */
            ResizeWidth (pBox, pSourceBox, NULL, width - pBox->BxW, 0, 0,
                         spaceDelta, frame, FALSE);
        }
      else if (minimumRule)
        {
          /* the current width equals the minimum */
          if (width > pBox->BxW)
            {
              /* apply the content rule */
              pBox->BxRuleWidth = pBox->BxW;
              pBox->BxContentWidth = !pBox->BxContentWidth;
              ResizeWidth (pBox, pSourceBox, NULL, width - pBox->BxW, 0, 0,
                           spaceDelta, frame, FALSE);
            }
          else
            /* update the content width */
            pBox->BxRuleWidth = width;
        }
    }
}


/*----------------------------------------------------------------------
  ChangeWidth updates the constrainted box width.
  Check if the rule of the mininmum is respected and eventually exchange
  the real and the constrainted width.
  ----------------------------------------------------------------------*/
void ChangeWidth (PtrBox pBox, PtrBox pSourceBox, PtrBox pFromBox,
                  int delta, int spaceDelta, int frame)
{
  int                 width;
  ThotBool            minimumRule;

  if (pBox)
    {
      minimumRule = (!pBox->BxAbstractBox->AbWidth.DimIsPosition
                     && pBox->BxAbstractBox->AbWidth.DimMinimum);
      if (minimumRule)
        pBox->BxShrink = FALSE;
        
      if (minimumRule && IsParentBox (pFromBox, pBox)
          && pFromBox->BxContentWidth)
        /* If the minimum size depends on an enclosing box and the current
         * size of this box depends on the content size,
         * don't change the value
         */
        return;

      /* check if the current width depends on the content */
      if (pBox->BxContentWidth)
        {
          /* the current width equals the content width */
          width = pBox->BxRuleWidth + delta;
          if (width > pBox->BxW)
            {
              /* apply the minimum rule */
              pBox->BxRuleWidth = pBox->BxW;
              pBox->BxContentWidth = !pBox->BxContentWidth;
              ResizeWidth (pBox, pSourceBox, pFromBox, width - pBox->BxW,
                           0, 0, spaceDelta, frame, FALSE);
            }
          else
            /* update the minimum width */
            pBox->BxRuleWidth = width;
        }
      else if (minimumRule)
        {
          /* the current width equals the minimum */
          width = pBox->BxW + delta;
          if (width < pBox->BxRuleWidth)
            {
              /* apply the content rule */
              width = pBox->BxRuleWidth;
              pBox->BxRuleWidth = pBox->BxW + delta;
              pBox->BxContentWidth = !pBox->BxContentWidth;
              ResizeWidth (pBox, pSourceBox, pFromBox, width - pBox->BxW,
                           0, 0, spaceDelta, frame, FALSE);
            }
          else
            /* update the current minimum */
            ResizeWidth (pBox, pSourceBox, pFromBox, delta, 0, 0,
                         spaceDelta, frame, FALSE);
        }
      else
        {
          if (pBox->BxShrink && pBox->BxRuleWidth && pBox->BxW != pBox->BxRuleWidth)
            {
              pBox->BxRuleWidth += delta;
              if (pBox->BxW < pBox->BxRuleWidth)
                {
                  // keep the shrinked width
                  delta = 0;
                  // but transmit the new rule value to nested boxes
                  TransmitRuleWidth (pBox, pSourceBox, frame);
                }
            }
          else if ((pBox->BxType == BoCell || pBox->BxType == BoCellBlock) &&
                   !pBox->BxAbstractBox->AbWidth.DimIsPosition &&
                   pBox->BxAbstractBox->AbWidth.DimAbRef == NULL &&
                   pBox->BxAbstractBox->AbWidth.DimValue >= 0)
            /* a CSS rule fixes the width of this cell */
            delta = pFromBox->BxWidth - pBox->BxWidth;
          ResizeWidth (pBox, pSourceBox, pFromBox, delta, 0, 0,
                       spaceDelta, frame, FALSE);
        }
    }
}


/*----------------------------------------------------------------------
  ChangeHeight updates the constrainted box height.
  Check if the rule of the mininmum is respected and eventually exchange
  the real and the constrainted height.
  ----------------------------------------------------------------------*/
void ChangeHeight (PtrBox pBox, PtrBox pSourceBox, PtrBox pFromBox,
                   int delta, int frame)
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

      /* check if the current height depends on the contents */
      if (pBox->BxContentHeight)
        {
          /* the current height equals the contents height */
          height = pBox->BxRuleHeight + delta;
          if (minimumRule && height > pBox->BxH)
            {
              /* apply the minimum rule */
              pBox->BxRuleHeight = pBox->BxH;
              pBox->BxContentHeight = !pBox->BxContentHeight;
              ResizeHeight (pBox, pSourceBox, pFromBox, height - pBox->BxH,
                            0, 0, frame);
            }
          else
            /* update the minimum */
            pBox->BxRuleHeight = height;
        }
      else if (minimumRule)
        {
          /* the current height equals the minimum */
          height = pBox->BxH + delta;
          if (height < pBox->BxRuleHeight)
            {
              /* apply the contents rule */
              height = pBox->BxRuleHeight;
              pBox->BxRuleHeight = pBox->BxH + delta;
              pBox->BxContentHeight = !pBox->BxContentHeight;
              ResizeHeight (pBox, pSourceBox, pFromBox, height - pBox->BxH,
                            0, 0, frame);
            }
          else
            /* update the current minimum */
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
void MoveBoxEdge (PtrBox pBox, PtrBox pSourceBox, OpRelation op, int delta,
                  int frame, ThotBool horizRef)
{
  PtrAbstractBox      pAb;
  BoxEdge             oldPosEdge, oldVertEdge, oldHorizEdge;
  int                 t, l, b, r;
  int                 translation;

  pAb = pBox->BxAbstractBox;
  translation = delta;
  /* Avoid to perform two times the same job */
  if (pAb && delta != 0 && pBox->BxPacking <= 1)
    {
      /* register that we're preforming the job */
      pBox->BxPacking += 1;
      if (horizRef)
        {
          /* save the previous fixed edge */
          oldHorizEdge = pBox->BxHorizEdge;
          /* Look for the horizontal fixed edge and the horizontal free edge */
          if ((op == OpWidth/* && !pBox->BxHorizInverted) ||
                               (op != OpWidth && pBox->BxHorizInverted*/))
            oldPosEdge = pAb->AbWidth.DimPosition.PosEdge;
          else
            {
              oldPosEdge = pAb->AbHorizPos.PosEdge;
              /* Update the box history to avoid two moving of pSourceBox */
              if (pSourceBox == NULL)
                pBox->BxMoved = pSourceBox;
              else if (pSourceBox->BxMoved != pBox)
                pBox->BxMoved = pSourceBox;
              else
                {
                  pBox->BxMoved = NULL;
                  pSourceBox->BxMoved = NULL;
                }
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
          GetExtraMargins (pBox, frame, FALSE, &t, &b, &l, &r);
          if (pBox->BxLMargin > 0)
            l += pBox->BxLMargin;
          l +=  pBox->BxLBorder +  pBox->BxLPadding;
          if (pBox->BxRMargin > 0)
            r += pBox->BxRMargin;
          r += pBox->BxRBorder + pBox->BxRPadding;
          delta = delta + pBox->BxWidth - pBox->BxW - l - r;
          // the history starts here
          ResizeWidth (pBox, NULL, NULL, delta, 0, 0, 0, frame, FALSE);
          /* restore the fixed edge */
          pBox->BxHorizEdge = oldHorizEdge;
        }
      else
        {
          /* save the previous fixed edge */
          oldVertEdge = pBox->BxVertEdge;
          /* Look for the vertical fixed edge and the vertical free edge */
          if (op == OpHeight)
            {
              oldPosEdge = pAb->AbHeight.DimPosition.PosEdge;
              pBox->BxMoved = NULL;
            }
          else
            {
              oldPosEdge = pAb->AbVertPos.PosEdge;
              /* Update the box history to avoid two moving of pSourceBox */
              if (pSourceBox == NULL)
                pBox->BxMoved = pSourceBox;
              else if (pSourceBox->BxMoved != pBox)
                pBox->BxMoved = pSourceBox;
              else
                {
                  pBox->BxMoved = NULL;
                  pSourceBox->BxMoved = NULL;
                }
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
	  
          if (delta < 0 && -delta > pBox->BxHeight)
            {
              /* Invert box edges */
              YEdgesExchange (pBox, op);
	      
              /* Translate the box origin */
              delta = -delta - 2 * pBox->BxHeight;
              if (pBox->BxVertEdge == Bottom)
                translation = -pBox->BxHeight;
              else if (pBox->BxVertEdge == Top)
                translation = pBox->BxHeight;
              YMove (pBox, pSourceBox, translation, frame);
            }
          /* Resize the box */
          GetExtraMargins (pBox, frame, FALSE, &t, &b, &l, &r);
          if (pBox->BxTMargin > 0)
            t += pBox->BxTMargin;
          t +=  pBox->BxTBorder +  pBox->BxTPadding;
          if (pBox->BxBMargin > 0)
            b += pBox->BxBMargin;
          b += pBox->BxBBorder + pBox->BxBPadding;
          delta = delta + pBox->BxHeight - pBox->BxH - t - b;
          // the history starts here
          ResizeHeight (pBox, NULL, NULL, delta, 0, 0, frame);
          /* restore the fixed edge */
          pBox->BxVertEdge = oldVertEdge;
        }

      /* the job is performed now */
      pBox->BxPacking -= 1;
      /* restore the box history */
      if (pSourceBox == NULL)
        pBox->BxMoved = pSourceBox;
      else if (pSourceBox->BxMoved != pBox)
        pBox->BxMoved = pSourceBox;
    }
 }


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void CoordinateSystemUpdate (PtrAbstractBox pAb, int frame, int x, int y)
{  
#ifdef _GL 
  int    doc;
      
  /* est-ce un systeme de coordonnee ?*/
  doc = FrameTable[frame].FrDoc;
  TtaReplaceTransform ((Element) pAb->AbElement, 
                       TtaNewBoxTransformTranslate ((float) x, (float) y),
                       doc);
#endif /* _GL */
}


/*----------------------------------------------------------------------
  XMoveAllEnclosed moves horizontally all enclosed boxes.
  ----------------------------------------------------------------------*/
void XMoveAllEnclosed (PtrBox pBox, int delta, int frame)
{
  PtrAbstractBox      pChildAb, pAb;
  PtrBox              pChildBox;
  PtrBox              pParentBox;
  BoxRelation        *pRel;
  PtrPosRelations     pPosRel;
  int                 i;
  ThotBool            notEmpty, isSysOrg;
  ThotBool            toHorizPack;

  if (pBox && (delta != 0 || pBox->BxXToCompute))
    {
      /* register the previous hierarchy of boxes to be packed later */
      pParentBox = PackBoxRoot;
      pAb = pBox->BxAbstractBox;
      if (pAb->AbEnclosing && !IsParentBox (pBox, pParentBox))
        /* change the hierarchy */
        PackBoxRoot = pBox;

      if (pBox->BxType == BoSplit || pBox->BxType == BoMulScript)
        {
          /* the box is split in lines, move all pieces */
          pChildBox = pBox->BxNexChild;
          while (pChildBox)
            {
              pChildBox->BxXOrg += delta;
              pChildBox = pChildBox->BxNexChild;
            }
        }
      else if (pAb)
        {
#ifdef _GL
          pBox->VisibleModification = TRUE;
#endif /* _GL */
          /* stretched box not already handled */
          if (pBox->BxHorizFlex &&
              (pAb->AbLeafType != LtCompound || pBox->BxPacking == 0))
            MoveBoxEdge (pBox, NULL, OpHorizDep, delta, frame, TRUE);
          else
            {
              if (!pAb->AbHorizEnclosing && ReadyToDisplay)
                /* update the clipping region */
                UpdateBoxRegion (frame, pBox, delta, 0, 0, 0);
              /* simple translation */
              pBox->BxXOrg += delta;
	      
              /* Move boxes which are out of structure relations with it
                 and update streched dimensions that depends on it
              */
              pPosRel = pBox->BxPosRelations;
              isSysOrg = IsSystemOrigin (pAb, frame);
              while (pPosRel)
                {
                  i = 0;
                  notEmpty = pPosRel->PosRTable[i].ReBox != NULL;
                  while (i < MAX_RELAT_POS && notEmpty)
                    {
                      pRel = &pPosRel->PosRTable[i];
                      if (pRel->ReBox->BxAbstractBox &&
                          // don't move children of a new system origin
                           !isSysOrg)
                        {
                          /* Relation out of structure */
                          if (pRel->ReOp == OpHorizDep &&
                              pRel->ReBox->BxXOutOfStruct &&
                              pRel->ReBox->BxAbstractBox->AbHorizPos.PosAbRef == pAb)
                            {
                              /* don't move the enclosing box pBox */
                              if (IsParentBox (pRel->ReBox, pBox))
                                ;
                              else if (pRel->ReBox->BxHorizFlex)
                                MoveBoxEdge (pRel->ReBox, pBox, pRel->ReOp, delta, frame, TRUE);
                              else
                                XMove (pRel->ReBox, pBox, delta, frame);
                            }
                          else if (pRel->ReOp == OpWidth)
                            /* stretched dimension */
                            MoveBoxEdge (pRel->ReBox, pBox, pRel->ReOp, delta, frame, TRUE);
                        }
                      i++;
                      if (i < MAX_RELAT_POS)
                        notEmpty = (pPosRel->PosRTable[i].ReBox != NULL);
                    }
                  /* next block */
                  pPosRel = pPosRel->PosRNext;
                }

              if (isSysOrg)
                // don't transmit the moving
                delta = 0;
              else if (pBox->BxXToCompute)
                /* if the box position is not complete
                   transmit the box position instead of the shift except for
                   out of structure relations */
                delta = pBox->BxXOrg;

              /* Move inclused boxes which depend on it */
              pChildAb = pAb->AbFirstEnclosed;
              /* Traite le niveau inferieur */
              toHorizPack = FALSE;
              if (pBox->BxType == BoBlock ||
                  pBox->BxType == BoFloatBlock || pBox->BxType == BoCellBlock)
                // update included floated boxes
                ShiftFloatingBoxes (pBox, delta, frame);
              while (pChildAb)
                {
                  if (pChildAb->AbBox)
                    {
                      if (pChildAb->AbBox->BxXOutOfStruct)
                        toHorizPack = TRUE;
                      else
                        {
                          if (pBox->BxXToCompute && !pChildAb->AbBox->BxXToCompute &&
                              Propagate == ToSiblings &&
                              !pChildAb->AbBox->BxHorizFlex)
                            pChildAb->AbBox->BxXToCompute = TRUE;
                          XMoveAllEnclosed (pChildAb->AbBox, delta, frame);
                        }
                    }
                  pChildAb = pChildAb->AbNext;
                }

              /* the box position is now complete */
              pBox->BxXToCompute = FALSE;

              /* Wen the box width depends on the contents and a child
                 position depends on an external box, it's necessary
                 to check the new width
              */
              if (toHorizPack &&
                  (pBox->BxContentWidth ||
                   (!pAb->AbWidth.DimIsPosition && pAb->AbWidth.DimMinimum)))
                RecordEnclosing (pBox, TRUE);
            }
        }
      /* restore */
      PackBoxRoot = pParentBox;
    }
}


/*----------------------------------------------------------------------
  YMoveAllEnclosed moves vertically all enclosed boxes.
  ----------------------------------------------------------------------*/
void YMoveAllEnclosed (PtrBox pBox, int delta, int frame)
{
  PtrAbstractBox      pChildAb, pAb;
  PtrBox              pChildBox;
  PtrBox              pParentBox;
  BoxRelation        *pRel;
  PtrPosRelations     pPosRel;
  int                 i;
  ThotBool            notEmpty, isSysOrg;
  ThotBool            toVertPack;

  if (pBox && (delta || pBox->BxYToCompute))
    {
      /* register the previous hierarchy of boxes to be packed later */
      pParentBox = PackBoxRoot;
      pAb = pBox->BxAbstractBox;
      if (pAb->AbEnclosing &&
          !IsParentBox (pBox, pParentBox))
        /* change the hierarchy */
        PackBoxRoot = pBox;

      if (pBox->BxType == BoSplit || pBox->BxType == BoMulScript)
        {
          /* the box is split in lines, move all pieces */
          pChildBox = pBox->BxNexChild;
          while (pChildBox)
            {
              pChildBox->BxYOrg += delta;
              pChildBox = pChildBox->BxNexChild;
            }
        }
      else if (pAb)
        {
#ifdef _GL
          pBox->VisibleModification = TRUE;
#endif /* _GL */
          if (pBox->BxVertFlex &&
              /* stretched box not already handled */
              (pAb->AbLeafType != LtCompound ||
               pBox->BxPacking == 0))
            MoveBoxEdge (pBox, NULL, OpVertDep, delta, frame, FALSE);
          else
            {
              if (!pAb->AbVertEnclosing && ReadyToDisplay)
                /* update the clipping region */
                UpdateBoxRegion (frame, pBox, 0, delta, 0, 0);
              /* simple translation */
              pBox->BxYOrg += delta;

              /* Move boxes which are out of structure relations with it
                 and update streched dimensions that depends on it
              */
              isSysOrg = IsSystemOrigin (pAb, frame);
              pPosRel = pBox->BxPosRelations;
              while (pPosRel)
                {
                  i = 0;
                  notEmpty = pPosRel->PosRTable[i].ReBox != NULL;
                  while (i < MAX_RELAT_POS && notEmpty)
                    {
                      pRel = &pPosRel->PosRTable[i];
                      if (pRel->ReBox->BxAbstractBox &&
                          // don't move children of a new system origin
                          !isSysOrg)
                        {
                          /* Relation out of structure */
                          if (pRel->ReOp == OpVertDep &&
                              pRel->ReBox->BxYOutOfStruct &&
                              pRel->ReBox->BxAbstractBox->AbVertPos.PosAbRef == pAb)
                            {
                              /* don't move the enclosing box pBox */
                              if (IsParentBox (pRel->ReBox, pBox))
                                ;
                              else if (pRel->ReBox->BxVertFlex)
                                MoveBoxEdge (pRel->ReBox, pBox, pRel->ReOp, delta, frame, FALSE);
                              else
                                YMove (pRel->ReBox, pBox, delta, frame);
                            }
                          else if (pRel->ReOp == OpHeight)
                            /* stretched dimension */
                            MoveBoxEdge (pRel->ReBox, pBox, pRel->ReOp, delta, frame, FALSE);
                        }
                      i++;
                      if (i < MAX_RELAT_POS)
                        notEmpty = pPosRel->PosRTable[i].ReBox != NULL;
                    }
                  /* next block */
                  pPosRel = pPosRel->PosRNext;
                }

              if (isSysOrg)
                // don't transmit the moving
                delta = 0;
              else if (pBox->BxYToCompute)
                /* if the box position is not complete
                   transmit the box position instead of the shift except for
                   out of structure relations */
                delta = pBox->BxYOrg;

              /* Move inclused boxes which depend on it */
              pChildAb = pAb->AbFirstEnclosed;
              /* Traite le niveau inferieur */
              toVertPack = FALSE;
              while (pChildAb != NULL)
                {
                  if (pChildAb->AbBox)
                    {
                      if (pChildAb->AbBox->BxYOutOfStruct)
                        toVertPack = TRUE;
                      else
                        {
                          if (pBox->BxYToCompute && !pChildAb->AbBox->BxYToCompute &&
                              Propagate == ToSiblings &&
                              !pChildAb->AbBox->BxVertFlex)
                            pChildAb->AbBox->BxYToCompute = TRUE;
                          YMoveAllEnclosed (pChildAb->AbBox, delta, frame);
                        }
                    }
                  pChildAb = pChildAb->AbNext;
                }
              /* the box position is now complete */
              pBox->BxYToCompute = FALSE;

              /* When the box height depends on the contents and a child
                 position depends on an external box, it's necessary
                 to check the new width
              */
              if (toVertPack &&
                  (pBox->BxContentHeight ||
                   (!pAb->AbHeight.DimIsPosition && pAb->AbHeight.DimMinimum)))
                RecordEnclosing (pBox, FALSE);
            }
        }
      /* restore */
      PackBoxRoot = pParentBox;
    }
}


/*----------------------------------------------------------------------
  MoveVertRef moves the vertical reference axis of the box.
  All boxes attached to that axis are moved.
  ----------------------------------------------------------------------*/
void MoveVertRef (PtrBox pBox, PtrBox pFromBox, int delta, int frame)
{
  int                 i;
  PtrAbstractBox      pChild, pAb, pParent;
  PtrBox              pNextBox;
  PtrBox              pRefBox;
  PtrPosRelations     pPosRel;
  BoxRelation        *pRel;
  ThotBool            toMove;
  ThotBool            notEmpty;

  if (pBox != NULL && delta != 0)
    {
      if (pBox->BxAbstractBox && !IsDead (pBox->BxAbstractBox))
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
              pAb = pBox->BxAbstractBox;
              /* add the box in the history */
              pBox->BxMoved = pFromBox;
              /* regarde si les regles de dependance sont valides */
              if (pAb->AbEnclosing && pAb->AbEnclosing->AbBox)
                toMove = (pAb->AbEnclosing->AbBox->BxType != BoGhost &&
                          pAb->AbEnclosing->AbBox->BxType != BoStructGhost &&
                          pAb->AbEnclosing->AbBox->BxType != BoFloatGhost);
	      
              if (toMove)
                {
                  if (pBox->BxHorizEdge == VertRef)
                    {
                      /* the box position depends on its vertical axis */
                      delta = -delta;
                      /* need to check inclusion */
                      toMove = TRUE;
                      /* register the window area to be redisplayed */
                      if (ReadyToDisplay &&
                          pBox->BxType != BoSplit &&
                          pBox->BxType != BoMulScript)
                        /* update the clipping region */
                        UpdateBoxRegion (frame, pBox, delta, 0, 0, 0);
		       
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
                      while (pPosRel)
                        {
                          i = 0;
                          notEmpty = (pPosRel->PosRTable[i].ReBox != NULL);
                          while (i < MAX_RELAT_POS && notEmpty)
                            {
                              pRel = &pPosRel->PosRTable[i];
                              if (pRel->ReBox->BxAbstractBox)
                                {
                                  if (pRel->ReBox != pBox &&
                                      pRel->ReRefEdge != VertRef)
                                    {
                                      if (pRel->ReOp == OpHorizRef)
                                        MoveVertRef (pRel->ReBox, pBox,
                                                     delta, frame);
                                      else if ((pRel->ReOp == OpHorizDep &&
                                                pRel->ReBox->BxAbstractBox->AbWidth.DimIsPosition)
                                               || pRel->ReOp == OpWidth)
                                        MoveBoxEdge (pRel->ReBox, pBox,
                                                     pRel->ReOp, delta,
                                                     frame, TRUE);
                                      else if (pRel->ReOp == OpHorizDep)
                                        XMove (pRel->ReBox, pBox, delta,
                                               frame);
                                    }
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
                      while (pPosRel)
                        {
                          i = 0;
                          notEmpty = (pPosRel->PosRTable[i].ReBox != NULL);
                          while (i < MAX_RELAT_POS && notEmpty)
                            {
                              pRel = &pPosRel->PosRTable[i];
                              if (pRel->ReBox->BxAbstractBox &&
                                  pRel->ReBox->BxAbstractBox->AbFloat == 'N')
                                {
                                  if (pRel->ReRefEdge == VertRef)
                                    {
                                      if (pRel->ReOp == OpHorizRef)
                                        /* move a vertical axis */
                                        MoveVertRef (pRel->ReBox, pFromBox,
                                                     delta, frame);
                                      else if ((pRel->ReOp == OpHorizDep &&
                                                pRel->ReBox->BxAbstractBox->AbWidth.DimIsPosition)
                                               || pRel->ReOp == OpWidth)
                                        /* move a a edge of a stretchable box */
                                        MoveBoxEdge (pRel->ReBox, pBox,
                                                     pRel->ReOp, delta,
                                                     frame, TRUE);
                                      else if (pRel->ReOp == OpHorizDep)
                                        {
                                          /* move a sibling */
                                          XMove (pRel->ReBox, pBox, delta,
                                                 frame);
                                          /* verify enclosing */
                                          toMove = TRUE;
                                        }
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
                      if (IsXPosComplete (pBox) &&
                          pBox->BxType != BoBlock &&
                          pBox->BxType != BoFloatBlock &&
                          pBox->BxType != BoCellBlock)
                        {
                          pChild = pAb->AbFirstEnclosed;
                          while (pChild)
                            {
                              if (!pChild->AbDead
                                  && pChild->AbHorizPos.PosAbRef == pAb
                                  && pChild->AbHorizPos.PosRefEdge == VertRef)
                                {
                                  pNextBox = pChild->AbBox;
                                  XMove (pNextBox, pBox, delta, frame);
                                }
                              pChild = pChild->AbNext;
                            }
			   
                          if (pNextBox && Propagate == ToAll)
                            /* check the inclusion */
                            WidthPack (pAb, pRefBox, frame);
                        }
                    }
                }
              /* check the inclusion constraint */
              pParent = pAb->AbEnclosing;
              if (toMove)
                {
                  if (pParent == NULL)
                    {
                      if (Propagate == ToAll && pBox->BxXOrg < 0)
                        XMoveAllEnclosed (pBox, -pBox->BxXOrg, frame);
                    }
                  else if (pParent->AbBox->BxType != BoBlock &&
                           pParent->AbBox->BxType != BoFloatBlock &&
                           pParent->AbBox->BxType != BoCellBlock &&
                           pParent->AbBox->BxType != BoGhost &&
                           pParent->AbBox->BxType != BoStructGhost &&
                           pParent->AbBox->BxType != BoFloatGhost &&
                           pParent->AbBox->BxType != BoCell &&
                           pParent->AbBox->BxType != BoColumn &&
                           !IsParentBox (pParent->AbBox, pRefBox) &&
                           !IsParentBox (pParent->AbBox, PackBoxRoot))
                    /* check the inclusion of the sibling box */
                    WidthPack (pParent, pRefBox, frame);
                }
            }
        }
    }
}

/*----------------------------------------------------------------------
  MoveHorizRef moves the baseline of the box.
  All boxes attached to that axis are moved.
  ----------------------------------------------------------------------*/
void MoveHorizRef (PtrBox pBox, PtrBox pFromBox, int delta, int frame)
{
  int                 i;
  PtrAbstractBox      pChild, pAb, pParent;
  PtrBox              pNextBox;
  PtrBox              pRefBox;
  PtrPosRelations     pPosRel;
  BoxRelation        *pRel;
  ThotBool            toMove;
  ThotBool            notEmpty;

  if (pBox != NULL && delta != 0)
    {
      if (pBox->BxAbstractBox && !IsDead (pBox->BxAbstractBox))
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
              pAb = pBox->BxAbstractBox;
              /* add the box in the history */
              pBox->BxMoved = pFromBox;
              /* check validity of dependencies */
              if (pAb->AbEnclosing && pAb->AbEnclosing->AbBox)
                toMove = (pAb->AbEnclosing->AbBox->BxType != BoGhost &&
                          pAb->AbEnclosing->AbBox->BxType != BoStructGhost &&
                          pAb->AbEnclosing->AbBox->BxType != BoFloatGhost);
	      
              if (toMove)
                {
                  if (pBox->BxVertEdge == HorizRef)
                    {
                      /* the box position depends on its baseline */
                      delta = -delta;
                      /* need to check inclusion */
                      toMove = TRUE;
                      /* register the window area to be redisplayed */
                      if (ReadyToDisplay &&
                          pBox->BxType != BoSplit &&
                          pBox->BxType != BoMulScript)
                        /* update the clipping region */
                        UpdateBoxRegion (frame, pBox, 0, delta, 0, 0);
		      
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
                      while (pPosRel)
                        {
                          i = 0;
                          notEmpty = pPosRel->PosRTable[i].ReBox != NULL;
                          while (i < MAX_RELAT_POS && notEmpty)
                            {
                              pRel = &pPosRel->PosRTable[i];
                              if (pRel->ReBox->BxAbstractBox)
                                {
                                  if (pRel->ReBox != pBox &&
                                      pRel->ReRefEdge != HorizRef)
                                    {
                                      if (pRel->ReOp == OpVertRef)
                                        MoveHorizRef (pRel->ReBox, pBox,
                                                      delta, frame);
                                      else if ((pRel->ReOp == OpVertDep &&
                                                pRel->ReBox->BxAbstractBox->AbHeight.DimIsPosition)
                                               || pRel->ReOp == OpHeight)
                                        MoveBoxEdge (pRel->ReBox, pBox,
                                                     pRel->ReOp, delta,
                                                     frame, FALSE);
                                      else if (pRel->ReOp == OpVertDep)
                                        YMove (pRel->ReBox, pBox, delta,
                                               frame);
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
                  else
                    {
                      /* the box position doesn't depends on its baseline */
                      /* move sibing boxes? */
                      toMove = FALSE;
                      pPosRel = pBox->BxPosRelations;
                      while (pPosRel)
                        {
                          i = 0;
                          notEmpty = pPosRel->PosRTable[i].ReBox != NULL;
                          while (i < MAX_RELAT_POS && notEmpty)
                            {
                              pRel = &pPosRel->PosRTable[i];
                              if (pRel->ReBox->BxAbstractBox &&
                                  pRel->ReBox->BxAbstractBox->AbFloat == 'N')
                                {
                                  if (pRel->ReRefEdge == HorizRef)
                                    {
                                      if (pRel->ReOp == OpVertRef)
                                        /* move a baseline */
                                        MoveHorizRef (pRel->ReBox, pBox,
                                                      delta, frame);
                                      else if ((pRel->ReOp == OpVertDep &&
                                                pRel->ReBox->BxAbstractBox->AbHeight.DimIsPosition)
                                               || pRel->ReOp == OpHeight)
                                        /* move a a edge of a stretchable box */
                                        MoveBoxEdge (pRel->ReBox, pBox,
                                                     pRel->ReOp, delta,
                                                     frame, FALSE);
                                      else if (pRel->ReOp == OpVertDep)
                                        {
                                          /* move a sibling */
                                          YMove (pRel->ReBox, pBox, delta,
                                                 frame);
                                          /* verify enclosing */
                                          toMove = TRUE;
                                        }
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
                      if (IsYPosComplete (pBox) &&
                          pBox->BxType != BoBlock &&
                          pBox->BxType != BoFloatBlock &&
                          pBox->BxType != BoCellBlock)
                        {
                          pChild = pAb->AbFirstEnclosed;
                          while (pChild)
                            {
                              if (!pChild->AbDead
                                  && pChild->AbVertPos.PosAbRef == pAb
                                  && pChild->AbVertPos.PosRefEdge == HorizRef)
                                {
                                  pNextBox = pChild->AbBox;
                                  YMove (pNextBox, pBox, delta, frame);
                                }
                              pChild = pChild->AbNext;
                            }
			  
                          if (pNextBox && Propagate == ToAll)
                            /* check the inclusion */
                            HeightPack (pAb, pRefBox, frame);
                        }
                    }
                }

              /* check the inclusion constraint */
              pParent = pAb->AbEnclosing;
              if (toMove)
                {
                  if (pParent == NULL)
                    {
                      if (Propagate == ToAll && pBox->BxYOrg < 0)
                        YMoveAllEnclosed (pBox, -pBox->BxYOrg, frame);
                    }
                  else if (pParent->AbBox->BxType == BoBlock ||
                           pParent->AbBox->BxType == BoFloatBlock ||
                           pParent->AbBox->BxType == BoCellBlock ||
                           pParent->AbBox->BxType == BoGhost ||
                           pParent->AbBox->BxType == BoStructGhost ||
                           pParent->AbBox->BxType == BoFloatGhost)
                    {
                      if (Propagate == ToAll &&
                          !IsParentBox (pParent->AbBox, pRefBox))
                        EncloseInLine (pBox, frame, pParent);
                    }
                  else if (!IsParentBox (pParent->AbBox, pRefBox)
                           && !IsParentBox (pParent->AbBox, PackBoxRoot))
                    /* check the inclusion of the sibling box */
                    HeightPack (pParent, pRefBox, frame);
                }
            }
        }
    }
}

/*----------------------------------------------------------------------
  CheckExtensibleBlocks
  Check if extensible boxes must be updated
  ----------------------------------------------------------------------*/
static void CheckExtensibleBlocks (PtrAbstractBox pAb, PtrBox pBox, int frame)
{
  PtrAbstractBox pChild;
  PtrBox         box;

  pChild = pAb->AbFirstEnclosed;
  while (pChild)
    {
      if (pChild->AbLeafType == LtCompound && pChild->AbBox && !pChild->AbDead &&
          pChild->AbBox->BxContentWidth)
        {
          box = pChild->AbBox;
          if (box->BxType == BoBlock || box->BxType == BoFloatBlock)
            {
              //if (box->BxWidth > pBox->BxW)
              RecomputeLines (pChild, NULL, pBox, frame);
            }
          else
            // check enclosed blocks
            CheckExtensibleBlocks (pChild, pBox, frame);
        }
      pChild = pChild->AbNext;
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
  The parameter shrink is TRUE when the resize is generated by a shrink
  contraint. In this case the update is not transmitted to enclosed boxes.
  ----------------------------------------------------------------------*/
void ResizeWidth (PtrBox pBox, PtrBox pSourceBox, PtrBox pFromBox, int delta,
                  int l, int r, int spaceDelta, int frame, ThotBool shrink)
{
  PtrBox              box;
  PtrLine             pLine;
  PtrAbstractBox      pAb, pCell, pParent, pRefAb;
  PtrPosRelations     pPosRel;
  PtrDimRelations     pDimRel;
  BoxRelation        *pRel;
  ViewSelection      *pViewSel;
  Propagation         savpropage;
  int                 i, j, diff, val;
  int                 orgTrans, middleTrans, endTrans;
  int                 extraL = 0, extraR = 0;
  int                 addL = 0, addR = 0;
  ThotBool            notEmpty, toMove, position, isSysOrg;
  ThotBool            absoluteMove, externalRef;

  if (pBox == NULL)
    return;
  /* check if the inside width, margins, borders, and paddings change */
  pAb = pBox->BxAbstractBox;
  if (pAb == NULL)
    return;
  GetExtraMargins (pBox, frame, FALSE, &i, &j, &extraL, &extraR);
  if (!pAb->AbMBPChange && delta)
    {
      if (pBox->BxLMargin > 0)
        extraL += pBox->BxLMargin;
      extraL += pBox->BxLBorder + pBox->BxLPadding;
      if (pBox->BxRMargin > 0)
        extraR += pBox->BxRMargin;
      extraR += pBox->BxRBorder + pBox->BxRPadding;
      diff = pBox->BxW + extraL + extraR - pBox->BxWidth;
    }
  else
    {
      /* margins borders and are not interpreted yet */
      diff = l + extraL + r + extraR;
      /* left and right adds transmitted to enclosing boxes */
      addL = l;
      addR = r;
    }
  if (delta || diff ||
      pAb->AbLeftMarginUnit == UnAuto || pAb->AbRightMarginUnit == UnAuto)
    {
      /* Do we have to clean up the history of moved boxes */
      if (pSourceBox == NULL && pFromBox == NULL)
        pBox->BxMoved = NULL;
      
      if (!IsDead (pAb))
        {
          /* It's not a stretchable box: clean up the history */
          if (!pBox->BxHorizFlex)
            pBox->BxMoved = NULL;
	  
          /* Force the reevaluation of polyline control points */
          if (pAb->AbLeafType == LtPolyLine &&
              pBox->BxPictInfo != NULL)
            {
              /* free control points */
              TtaFreeMemory ((STRING) pBox->BxPictInfo);
              pBox->BxPictInfo = NULL;
            }
          /* Check the validity of dependency rules */
          toMove = TRUE;
          if (pAb->AbFloat == 'N' && !HorizExtraAbFlow (pAb, frame) &&
              pAb->AbEnclosing && pAb->AbEnclosing->AbBox)
            toMove = (pAb->AbEnclosing->AbBox->BxType != BoGhost &&
                      pAb->AbEnclosing->AbBox->BxType != BoStructGhost &&
                      pAb->AbEnclosing->AbBox->BxType != BoFloatGhost &&
                      pAb->AbEnclosing->AbBox->BxType != BoBlock &&
                      pAb->AbEnclosing->AbBox->BxType != BoFloatBlock &&
                      pAb->AbEnclosing->AbBox->BxType != BoCellBlock);

          position = IsFlow (pBox, frame);
          /* check positionning constraints */
          if ((!toMove && !position) ||
              pAb->AbFloat == 'L' ||
              (pAb->AbFloat != 'R' &&
               (pBox->BxHorizEdge == Left || pBox->BxHorizEdge == VertRef)))
            {
              /*====> The left is fixed */
              /* Move the middle and the right */
              orgTrans = 0;
              //middleTrans = (pBox->BxW + delta) / 2 - pBox->BxW / 2;
              middleTrans = (pBox->BxWidth + delta + diff) / 2 - pBox->BxWidth / 2;
              endTrans = delta + diff;
            }
          else if (pAb->AbFloat != 'R' &&
                   pBox->BxHorizEdge == VertMiddle)
            {
              /*====> The middle is fixed */
              /* Move the left and the right */
              //orgTrans = pBox->BxW / 2 - (pBox->BxW + delta) / 2;
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
              //middleTrans = pBox->BxW / 2 - (pBox->BxW + delta) / 2;
              middleTrans = pBox->BxWidth / 2 - (pBox->BxWidth + delta + diff) / 2;
              endTrans = 0;
            }
          // take into account only the left margin and border for SVG content
          isSysOrg = IsSystemOrigin (pAb, frame);
          if (ReadyToDisplay &&
              pBox->BxType != BoSplit &&
              pBox->BxType != BoMulScript &&
              /* don't take care of a box which is not */
              /* at its right place in the concrete image  */
              !pBox->BxXToCompute &&
              !pBox->BxYToCompute &&
              (orgTrans || pAb->AbFirstEnclosed == NULL ||
               /* redisplay filled boxes */
               pAb->AbFillBox || pAb->AbPictBackground ||
               pBox->BxTBorder ||  pBox->BxBBorder ||  pBox->BxLBorder ||  pBox->BxBBorder))
            /* update the clipping region */
            UpdateBoxRegion (frame, pBox, orgTrans, 0, delta, 0);
          /* inside width */
          pBox->BxW += delta;
          if (!shrink && pBox->BxShrink)
            // keep in mind the last inherited width
            pBox->BxRuleWidth = pBox->BxW;
          /* outside width */
          pBox->BxWidth = pBox->BxWidth + delta + diff;
          // change the org if the box width increases on the left side
          pBox->BxXOrg += orgTrans;

          if (pBox->BxType == BoPicture || pAb->AbLeafType == LtGraphics)
            {
              pBox->BxMaxWidth = pBox->BxWidth;
              if (pBox->BxType == BoPicture &&
                  pAb->AbWidth.DimUnit == UnPercent)
                LoadPicture (frame, pBox, (ThotPictInfo *) (pAb->AbPictBackground));
            }

          /* Moving sibling boxes and the parent? */
          pPosRel = pBox->BxPosRelations;
          /* move sibling boxes with their content */
          savpropage = Propagate;
          while (pPosRel)
            {
              i = 0;
              notEmpty = pPosRel->PosRTable[i].ReBox != NULL;
              while (i < MAX_RELAT_POS && notEmpty)
                {
                  pRel = &pPosRel->PosRTable[i];
                  /* move all if the related box position is computed */
                  if (pRel->ReBox->BxXToCompute)
                    Propagate = ToAll;
                  pRefAb = pRel->ReBox->BxAbstractBox;
                  if (pRefAb &&
                      // don't move children of a new system origin
                      !isSysOrg)
                    {
                      /* Ignore the back relation of a stretchable box */
                      if (!pBox->BxHorizFlex ||
                          pRel->ReOp != OpHorizDep ||
                          pAb == pRefAb->AbHorizPos.PosAbRef)
                        switch (pRel->ReRefEdge)
                          {
                          case Left:
                            if (pRel->ReOp == OpHorizRef)
                              {
                                if (pRel->ReBox != pBox)
                                  MoveVertRef (pRel->ReBox, pSourceBox, orgTrans, frame);
                              }
                            else if (toMove)
                              {
                                if (pRefAb->AbHorizPos.PosUnit == UnPercent)
                                  val = delta * pRefAb->AbHorizPos.PosDistance / 100;
                                else
                                  val = 0;
                                if (pRel->ReOp == OpHorizInc)
                                  {
                                    if (!pBox->BxHorizFlex &&
                                        !IsParentBox (pBox, pRel->ReBox))
                                      XMove (pBox, NULL, -orgTrans, frame);
                                  }
                                else if ((pRel->ReOp == OpHorizDep && pRel->ReBox->BxHorizFlex)
                                         || pRel->ReOp == OpWidth)
                                  MoveBoxEdge (pRel->ReBox, pBox, pRel->ReOp, orgTrans, frame, TRUE);
                                else if (pRel->ReBox != pSourceBox &&
                                        !IsParentBox (pRel->ReBox, pBox))
                                  /* don't move the enclosing box */
                                  XMove (pRel->ReBox, pBox, orgTrans + val, frame);
                              }
                            break;
                          case VertMiddle:
                            if (pRel->ReOp == OpHorizRef)
                              {
                                if (pRel->ReBox == pBox)
                                  {
                                    MoveVertRef (pBox, NULL, delta / 2, frame);
                                    /* restore the history of moved boxes */
                                    pBox->BxMoved = pFromBox;
                                  }
                                else
                                  MoveVertRef (pRel->ReBox, pSourceBox, middleTrans, frame);
                              }
                            else if (toMove)
                              {
                                if (pRel->ReOp == OpHorizInc)
                                  {
                                    if (!pBox->BxHorizFlex)
                                      XMove (pBox, NULL, -middleTrans, frame);
                                  }
                                else if ((pRel->ReOp == OpHorizDep && pRel->ReBox->BxHorizFlex)
                                         || pRel->ReOp == OpWidth)
                                  MoveBoxEdge (pRel->ReBox, pBox, pRel->ReOp, middleTrans, frame, TRUE);
                                else if (pRel->ReBox != pSourceBox &&
                                         !IsParentBox (pRel->ReBox, pBox))
                                  /* don't move the enclosing box */
                                  XMove (pRel->ReBox, pBox, middleTrans, frame);
                              }
                            break;
                          case Right:
                            if (pRel->ReOp == OpHorizRef)
                              {
                                if (pRel->ReBox == pBox)
                                  {
                                    MoveVertRef (pBox, NULL, delta, frame);
                                    /* restore the history of moved boxes */
                                    pBox->BxMoved = pFromBox;
                                  }
                                else if (!IsParentBox (pRel->ReBox, pBox))
                                  MoveVertRef (pRel->ReBox, pSourceBox, endTrans, frame);
                              }
                            else if (toMove)
                              {
                                if (pRefAb->AbHorizPos.PosUnit == UnPercent)
                                  val = delta * pRefAb->AbHorizPos.PosDistance / 100;
                                else
                                  val = 0;
                                if (pRel->ReOp == OpHorizInc)
                                  {
                                    if (!pBox->BxHorizFlex)
                                      XMove (pBox, NULL, val - endTrans, frame);
                                  }
                                else if ((pRel->ReOp == OpHorizDep && pRel->ReBox->BxHorizFlex)
                                         || pRel->ReOp == OpWidth)
                                  MoveBoxEdge (pRel->ReBox, pBox, pRel->ReOp, endTrans, frame, TRUE);
                                else if (pRel->ReBox != pSourceBox &&
                                         !IsParentBox (pRel->ReBox, pBox))
                                  /* don't move the enclosing box pBox */
                                  XMove (pRel->ReBox, pBox, endTrans + val, frame);
                              }
                            break;
                          default:
                            break;
                          }
                    }
		    
                  i++;
                  if (i < MAX_RELAT_POS)
                    notEmpty = (pPosRel->PosRTable[i].ReBox != NULL);
                }
              /* next relations block */
              pPosRel = pPosRel->PosRNext;
            }
          /* restore the value */
          Propagate = savpropage;
	  
          /* Keep in mind if the box positionning is absolute or not */
          absoluteMove = IsXPosComplete (pBox);
          /* internal boxes take into account margins borders and paddings */
          if (isSysOrg)
            {
              orgTrans = l;
              middleTrans = 0;
              endTrans = 0;
            }
          else if (l || r)
            {
              orgTrans += l;
              middleTrans += (l - r)/2;
              endTrans += - r;
              if (l)
                /* internal boxes must be translated */
                absoluteMove = TRUE;
            }

          /* Moving included boxes or reevalution of the block of lines? */
          if (pAb->AbLeafType == LtCompound &&
              !pAb->AbNew && /* children are not already created */
              (!pBox->BxContentWidth || /* a fraction by example */
               (pBox->BxType != BoBlock &&
                pBox->BxType != BoFloatBlock && pBox->BxType != BoCellBlock)) &&
              (absoluteMove ||
               pAb->AbWidth.DimAbRef ||
               pAb->AbWidth.DimValue >= 0))

            /* the box is already built */
            /* or it's within a stretchable box */
            /* or it's within a unnested box */
            /* or it doesn't inherit the size from its contents */
            {
              if (pBox->BxType == BoBlock ||
                  pBox->BxType == BoFloatBlock || pBox->BxType == BoCellBlock)
                {
                  if (!shrink)
                    // the block update is not due to a shrink operation
                    if (RecomputeLines (pAb, pBox->BxFirstLine, pSourceBox, frame))
                        // The block width was changed by RecomputeLines
                        // the current update should be stopped
                        return;
                }
              else if (pBox->BxType != BoGhost &&
                       pBox->BxType != BoStructGhost &&
                       pBox->BxType != BoFloatGhost)
                {
                  pRefAb = pAb->AbFirstEnclosed;
                  while (pRefAb)
                    {
                      if (!pRefAb->AbDead && pRefAb->AbBox)
                        {
                          box = pRefAb->AbBox;
                          /* check if the position box depends on its enclosing */
                          pPosRel = box->BxPosRelations;
                          while (pPosRel)
                            {
                              i = 0;
                              notEmpty = (pPosRel->PosRTable[i].ReBox != NULL);
                              while (i < MAX_RELAT_POS && notEmpty)
                                {
                                  pRel = &pPosRel->PosRTable[i];
                                  if (pRel->ReBox->BxAbstractBox)
                                    {
                                      if (pRel->ReOp == OpHorizInc
                                          && pRel->ReRefEdge != VertRef)
                                        switch (pRefAb->AbHorizPos.PosRefEdge)
                                          {
                                          case VertMiddle:
                                            /* recheck the position */
                                            j = pBox->BxLMargin + pBox->BxLBorder + pBox->BxLPadding + (pBox->BxW - box->BxWidth) / 2 - box->BxXOrg;
                                            if (absoluteMove)
                                              j += pBox->BxXOrg;
                                            if (box->BxHorizFlex)
                                              MoveBoxEdge (box, pBox, pRel->ReOp, j, frame, TRUE);
                                            else
                                              XMove (box, pBox, j, frame);
                                            break;
                                          case Right:
                                            /* recheck the position */
                                            j = pBox->BxLMargin + pBox->BxLBorder + pBox->BxLPadding + pBox->BxW - box->BxWidth - box->BxXOrg;
                                            if (absoluteMove)
                                              j += pBox->BxXOrg;
                                            if (box->BxHorizFlex)
                                              MoveBoxEdge (box, pBox, pRel->ReOp, j, frame, TRUE);
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
                                              MoveBoxEdge (box, pBox, pRel->ReOp, j, frame, TRUE);
                                            else
                                              XMove (box, pBox, j, frame);
                                            break;
                                          }
                                    }
                                  i++;
                                  if (i < MAX_RELAT_POS)
                                    notEmpty = (pPosRel->PosRTable[i].ReBox != NULL);
                                }
                              /* next relationblock */
                              pPosRel = pPosRel->PosRNext;
                            }

                          pCell = GetParentCell (box);
                          if ((box->BxType != BoTable || pCell == NULL) &&
                              (pRefAb->AbLeftMarginUnit == UnAuto ||
                               pRefAb->AbRightMarginUnit == UnAuto ||
                               pRefAb->AbLeftMarginUnit == UnPercent ||
                               pRefAb->AbRightMarginUnit == UnPercent))
                            /* update auto and percent margins */
                            CheckMBP (pRefAb, box, frame, TRUE);
                        }
                      /* next child */
                      pRefAb = pRefAb->AbNext;
                    }
                }
            }

          if (pBox->BxType == BoCell)
            CheckExtensibleBlocks (pAb, pBox, frame);
          /* check dimension constraints */
          pDimRel = pBox->BxWidthRelations;
          while (pDimRel)
            {
              i = 0;
              while (i < MAX_RELAT_DIM && pDimRel->DimRTable[i] != NULL)
                {
                  box = pDimRel->DimRTable[i];
                  pRefAb = box->BxAbstractBox;
                  if ((pBox->BxType == BoBlock ||
                       pBox->BxType == BoFloatBlock || pBox->BxType == BoCellBlock ||
                       shrink) &&
                      IsParentBox (pBox, box))
                    /* update managed by ComputeLines or should not occur */
                    pRefAb = NULL;
                  if (pRefAb && !pRefAb->AbDead && !pRefAb->AbNew && pRefAb->AbBox)
                    {
                      /* Is it the same dimension? */
                      if (pDimRel->DimROp[i] == OpSame)
                        {
                          /* Changing the width */
                          if (box->BxType == BoCell || box->BxType == BoCellBlock ||
                              (pRefAb->AbWidth.DimUnit == UnPercent && pRefAb->AbWidth.DimValue == 100))
                            {
                              val = GetPercentDim (pRefAb, pAb, TRUE);
                              // transmit the column width to table cells
                              val -= box->BxW;
                              if (box->BxLMargin > 0)
                                val -= box->BxLMargin;
                              if (box->BxRMargin > 0)
                                val -= box->BxRMargin;
                              val = val - box->BxLBorder - box->BxLPadding - box->BxRBorder - box->BxRPadding;
                            }
                          else if ((pBox->BxType == BoCell || pBox->BxType == BoCellBlock) &&
                                   pRefAb->AbPresentationBox)
                            // transmit the cell width to cellframe
                            val = pBox->BxW + pBox->BxLPadding + pBox->BxRPadding - box->BxW;
                          else if (pRefAb->AbWidth.DimUnit == UnPercent)
                            {
                              if (pRefAb->AbEnclosing == pAb)
                                /* refer the inside width */
                                val = GetPercentDim (pRefAb, pAb, TRUE);
                              else
                                /* refer the outside width */
                                val = pBox->BxWidth;
                              val = val * pRefAb->AbWidth.DimValue / 100;
                              val = val - box->BxW;
                            }
                          else
                            {
                              val = delta;
                              if (pRefAb->AbEnclosing != pAb)
                                /* refer the outside width */
                                val = val + diff + addL + addR;
                            }

                          /* avoid cycles on the same box */
                          if (box != pBox)
                            ChangeWidth (box, pSourceBox, pBox, val, 0, frame);
                        }
                      else if (pDimRel->DimROp[i] == OpReverse)
                        {
                          /* Changing the height */
                          if (pRefAb->AbHeight.DimUnit == UnPercent)
                            {
                              if (pRefAb->AbEnclosing == pAb)
                                val = pBox->BxW;
                              else
                                /* refer the outside width */
                                val = pBox->BxWidth;
                              val = val * pRefAb->AbHeight.DimValue / 100;
                              val = val - box->BxH;
                            }
                          else
                            {
                              val = delta;
                              if (pRefAb->AbEnclosing != pAb)
                                /* refer the outside width */
                                val = val + diff + addL + addR;
                            }
                          ChangeHeight (box, pSourceBox, NULL, val, frame);
                        }
                    }
                  i++;
                }
              /* next relation block */
              pDimRel = pDimRel->DimRNext;
            }
          
          /* Check enclosing constraints */
          pParent = pAb->AbEnclosing;
          if (!toMove || pAb->AbFloat != 'N')
            {
              /* look for the enclosing block of lines  */
              while (pParent &&
                     pParent->AbBox->BxType != BoBlock &&
                     pParent->AbBox->BxType != BoFloatBlock &&
                     pParent->AbBox->BxType != BoCellBlock)
                pParent = pParent->AbEnclosing;
            }
	  
          if (pParent == NULL)
            {
              /* It's the root of the concrete image */
              if (Propagate == ToAll && pBox->BxXOrg < 0)
                XMoveAllEnclosed (pBox, -pBox->BxXOrg, frame);
            }
          else if (pAb->AbHorizEnclosing)
            {
              /* keep in mind if the relation concerns parent boxes */
              externalRef = !IsParentBox (pParent->AbBox, pSourceBox);
	      
              /*
               * if pSourceBox is a child and the inclusion is not performed
               * by another sibling box, we need to propagate the change
               */
              if (ExtraFlow (pBox, frame))
                /* doesn't check enclosing of a positioned box */
                ;
              else if ((Propagate == ToAll || externalRef) &&
                       !IsSiblingBox (pBox, pFromBox) &&
                       !IsSiblingBox (pBox, pSourceBox))
                {
                  /* Within a block of line */
                  if (pParent->AbBox != pSourceBox &&
                      !pParent->AbNew && /* not created yet */
                      (pParent->AbBox->BxType == BoBlock ||
                       pParent->AbBox->BxType == BoFloatBlock ||
                       pParent->AbBox->BxType == BoCellBlock))
                    {
                      pViewSel = &ViewFrameTable[frame - 1].FrSelectionBegin;
                      if (pBox == pViewSel->VsBox)
                        pLine = pViewSel->VsLine;
                      else
                        pLine = SearchLine (pBox, frame);
                      if (!pAb->AbMBPChange && delta)
                        UpdateLineBlock (pParent, pLine, pBox, delta, spaceDelta, frame);
                      else
                        UpdateLineBlock (pParent, pLine, pBox, diff, 0, frame);
                    }
                  /* if the inclusion is not checked at the end */
                  else if (!IsParentBox (pParent->AbBox, PackBoxRoot) &&
                           pParent->AbBox != pFromBox)
                    {
                      /* Differ the checking of the inclusion */
                      if (Propagate != ToAll)
                        RecordEnclosing (pParent->AbBox, TRUE);
                      /* Don't check the inclusion more than 2 times */
                      else if (pParent->AbBox->BxCycles <= 1)
                        {
                          if (pParent->AbBox->BxType == BoCell || pParent->AbBox->BxType == BoCellBlock)
                            UpdateColumnWidth (pParent, NULL, frame);
                          else if (pParent->AbBox != pSourceBox)
                            WidthPack (pParent, pSourceBox, frame);
                        }
                    }
                  else if (pParent->AbBox &&
                           (pParent->AbBox->BxShrink || pParent->AbBox->BxType == BoTable))
                    RecordEnclosing (pParent->AbBox, TRUE);
                }
              else if (!pAb->AbNew &&
                       Propagate == ToSiblings &&
                       pAb->AbLeafType == LtCompound &&
                       !pBox->BxYToCompute &&
                       (pBox->BxType == BoBlock ||
                        pBox->BxType == BoFloatBlock || pBox->BxType == BoCellBlock))
                {
                  /* the width of the block of lines could change its height
                     -> check vertical enclosing */
                  if (pParent->AbBox->BxType != BoTable)
                    {
                      //Propagate = ToChildren;
                      HeightPack (pParent, pSourceBox, frame);
                    }
                  /* restore the value if necessary */
                  Propagate = ToSiblings;
                }
              else if (pAb->AbFloat == 'N' &&
                       pBox->BxType != BoRow &&
                       !IsSiblingBox (pBox, pFromBox) &&
                       !IsSiblingBox (pBox, pSourceBox))
                RecordEnclosing (pParent->AbBox, TRUE);
            }

          if (delta && pBox->BxType == BoTable && pBox->BxCycles == 0)
            ChangeTableWidth (pAb, frame);
        }
      /* check if the root box width changed */
      if (pAb->AbEnclosing == NULL)
        {
          AnyWidthUpdate = TRUE;
          CheckScrollingWidthHeight (frame);
        }
    }
}

/*----------------------------------------------------------------------
  ResizeHeight updates the inside box height BxH then the outside one BxHeight.
  The parameter origin of the change is pSourceBox.
  Check positionning and sizing constraints:
  - any box attached to a box edge is moved
  - update the vertical reference axis
  - any other box size which depends on it is updated
  The parameters delta, t, b gives changes of the height, top, and bottom
  extra (margin, border, padding)
  ----------------------------------------------------------------------*/
void ResizeHeight (PtrBox pBox, PtrBox pSourceBox, PtrBox pFromBox,
                   int delta, int t, int b, int frame)
{
  PtrBox              box;
  PtrLine             pLine;
  PtrAbstractBox      pParent, pAb, pRefAb;
  PtrPosRelations     pPosRel;
  PtrDimRelations     pDimRel;
  BoxRelation        *pRel;
  SpecFont            font;
  Propagation         savpropage;
  int                 i, j, diff, val;
  int                 orgTrans, middleTrans, endTrans;
  int                 extraT = 0, extraB = 0;
  int                 addT = 0, addB = 0;
  ThotBool            notEmpty, toMove, isSysOrg;
  ThotBool            absoluteMove, externalRef;
  
  if (pBox == NULL)
    return;
  /* check if the inside width, margins, borders, and paddings change */
  pAb = pBox->BxAbstractBox;
  if (pAb == NULL)
    return;
  GetExtraMargins (pBox, frame, FALSE, &extraT, &extraB, &i, &j);
  if (!pAb->AbMBPChange && delta)
    {
      if (pBox->BxTMargin > 0)
        extraT += pBox->BxTMargin;
      extraT += pBox->BxTBorder + pBox->BxTPadding;
      if (pBox->BxBMargin > 0)
        extraB += pBox->BxBMargin;
      extraB += pBox->BxBBorder + pBox->BxBPadding;
      diff = pBox->BxH + extraT + extraB - pBox->BxHeight;
    }
  else
    {
      /* margins borders and are not interpreted yet */
      diff = t + extraT + b + extraB;
      /* top and bottom adds transmitted to enclosing boxes */
      addT = t;
      addB = b;
    }

  if (delta || diff)
    {
      /* Do we have to clean up the history of moved boxes */
      if (pSourceBox == NULL && pFromBox == NULL)
        pBox->BxMoved = NULL;
      if (!IsDead (pAb))
        {
          /* Area zone before moving */
          i = pBox->BxYOrg;
          j = i + pBox->BxHeight;
          /* It's not a stretchable box: clean up the history */
          if (!pBox->BxVertFlex)
            pBox->BxMoved = NULL;
	  
          /* Force the reevaluation of polyline control points */
          if (pAb->AbLeafType == LtPolyLine &&
              pBox->BxPictInfo != NULL)
            {
              /* free control points */
              TtaFreeMemory ((STRING) pBox->BxPictInfo);
              pBox->BxPictInfo = NULL;
            }

          /* Check the validity of dependency rules */
          toMove = TRUE;
          if (pAb->AbFloat == 'N' && !VertExtraAbFlow (pAb, frame)  &&
              pAb->AbEnclosing && pAb->AbEnclosing->AbBox)
            toMove = (pAb->AbEnclosing->AbBox->BxType != BoGhost &&
                      pAb->AbEnclosing->AbBox->BxType != BoStructGhost &&
                      pAb->AbEnclosing->AbBox->BxType != BoFloatGhost &&
                      pAb->AbEnclosing->AbBox->BxType != BoBlock &&
                      pAb->AbEnclosing->AbBox->BxType != BoFloatBlock &&
                      pAb->AbEnclosing->AbBox->BxType != BoCellBlock);

          /* check positionning constraints */
          if (!toMove || pBox->BxVertEdge == Top ||
              pBox->BxVertEdge == HorizRef)
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
          // take into account only the left margin and border for SVG content
          isSysOrg = IsSystemOrigin (pAb, frame);
          if (ReadyToDisplay &&
              pBox->BxType != BoSplit &&
              pBox->BxType != BoMulScript &&
              /* don't take care of a box which is not */
              /* at its right place in the concrete image  */
              !pBox->BxXToCompute &&
              !pBox->BxYToCompute &&
              (orgTrans || pAb->AbFirstEnclosed == NULL ||
               /* redisplay filled boxes */
               pAb->AbFillBox || pAb->AbPictBackground ||
               pBox->BxTBorder ||  pBox->BxBBorder ||  pBox->BxLBorder ||  pBox->BxBBorder))
            /* update the clipping region */
            UpdateBoxRegion (frame, pBox, 0, orgTrans, 0, delta + diff);

          /* inside height */
          pBox->BxH += delta;

          /* outside height */
          pBox->BxHeight = pBox->BxHeight + delta + diff;
          pBox->BxYOrg += orgTrans;
	  
          /* Moving sibling boxes and the parent? */
          pPosRel = pBox->BxPosRelations;
          /* move sibling boxes with their content */
          savpropage = Propagate;
          while (pPosRel)
            {
              i = 0;
              notEmpty = (pPosRel->PosRTable[i].ReBox != NULL);
              while (i < MAX_RELAT_POS && notEmpty)
                {
                  pRel = &pPosRel->PosRTable[i];
                  /* move all if the related box position is computed */
                  if (pRel->ReBox->BxYToCompute)
                    Propagate = ToAll;
                  pRefAb = pRel->ReBox->BxAbstractBox;
                  if (pRefAb &&
                      // don't move children of a new system origin
                      !isSysOrg)
                    {
                      /* Ignore the back relation of a stretchable box */
                      if (!pBox->BxVertFlex || pRel->ReOp != OpVertDep ||
                          pAb == pRefAb->AbVertPos.PosAbRef)
                        switch (pRel->ReRefEdge)
                          {
                          case Top:
                            if (pRel->ReOp == OpVertRef)
                              {
                                if (pRel->ReBox != pBox &&
                                    !IsParentBox (pBox, pRel->ReBox))
                                  MoveHorizRef (pRel->ReBox, pSourceBox, orgTrans, frame);
                              }
                            else if (toMove)
                              {
                                if (pRefAb->AbVertPos.PosUnit == UnPercent)
                                  val = delta * pRefAb->AbHorizPos.PosDistance / 100;
                                else
                                  val = 0;
                                if (pRel->ReOp == OpVertInc)
                                  {
                                    if (!pBox->BxVertFlex &&
                                        !IsParentBox (pBox, pRel->ReBox))
                                      YMove (pBox, NULL, -orgTrans, frame);
                                  }
                                else if ((pRel->ReOp == OpVertDep && pRel->ReBox->BxVertFlex)
                                         || pRel->ReOp == OpHeight)
                                  MoveBoxEdge (pRel->ReBox, pBox, pRel->ReOp, orgTrans, frame, FALSE);
                                else if (pRel->ReBox != pSourceBox &&
                                         !IsParentBox (pRel->ReBox, pBox))
                                  /* don't move the enclosing box */
                                  YMove (pRel->ReBox, pBox, orgTrans - val, frame);
                              }
                            break;
                          case HorizMiddle:
                            if (pRel->ReOp == OpVertRef)
                              {
                                if (pRel->ReBox == pBox)
                                  {
                                    MoveHorizRef (pBox, NULL, delta / 2, frame);
                                    /* restore the history of moved boxes */
                                    pBox->BxMoved = pFromBox;
                                  }
                                else if (!IsParentBox (pBox, pRel->ReBox))
                                  MoveHorizRef (pRel->ReBox, pSourceBox, middleTrans, frame);
                              }
                            else if (toMove)
                              {
                                if (pRel->ReOp == OpVertInc)
                                  {
                                    if (!pBox->BxVertFlex)
                                      YMove (pBox, NULL, -middleTrans, frame);
                                  }
                                else if ((pRel->ReOp == OpVertDep && pRel->ReBox->BxVertFlex)
                                         || pRel->ReOp == OpHeight)
                                  MoveBoxEdge (pRel->ReBox, pBox, pRel->ReOp, middleTrans, frame, FALSE);
                                else if (pRel->ReBox != pSourceBox &&
                                         !IsParentBox (pRel->ReBox, pBox))
                                  /* don't move the enclosing box */
                                  YMove (pRel->ReBox, pBox, middleTrans, frame);
                              }
                            break;
                          case Bottom:
                            if (pRel->ReOp == OpVertRef)
                              {
                                if (pRel->ReBox == pBox)
                                  {
                                    if (pAb->AbLeafType == LtText &&
                                        pAb->AbHorizRef.PosAbRef == NULL)
                                      j = BoxFontBase (pBox->BxFont) + pBox->BxTMargin + pBox->BxTBorder + pBox->BxTPadding - pBox->BxHorizRef;
                                    else
                                      j = delta;
                                    MoveHorizRef (pBox, NULL, j, frame);
                                    /* restore the history of moved boxes */
                                    pBox->BxMoved = pFromBox;
                                  }
                                else if (!IsParentBox (pRel->ReBox, pBox))
                                  MoveHorizRef (pRel->ReBox, pSourceBox, endTrans, frame);
                              }
                            else if (toMove)
                              {
                                if (pRefAb->AbHorizPos.PosUnit == UnPercent)
                                  val = delta * pRefAb->AbHorizPos.PosDistance / 100;
                                else
                                  val = 0;
                                if (pRel->ReOp == OpVertInc)
                                  {
                                    if (!pBox->BxVertFlex)
                                      YMove (pBox, NULL, val - endTrans, frame);
                                  }
                                else if ((pRel->ReOp == OpVertDep && pRel->ReBox->BxVertFlex)
                                         || pRel->ReOp == OpHeight)
                                  MoveBoxEdge (pRel->ReBox, pBox, pRel->ReOp, endTrans, frame, FALSE);
                                else if (pRel->ReBox != pSourceBox &&
                                         !IsParentBox (pRel->ReBox, pBox))
                                  /* don't move the enclosing box pBox */
                                  YMove (pRel->ReBox, pBox, endTrans + val, frame);
                              }
                            break;
                          default:
                            break;
                          }
                    }
                  i++;
                  if (i < MAX_RELAT_POS)
                    notEmpty = pPosRel->PosRTable[i].ReBox != NULL;
                }
              /* next relations block */
              pPosRel = pPosRel->PosRNext;
            }
          /* restore the value */
          Propagate = savpropage;
	  
          /* Keep in mind if the box positionning is absolute or not */
          absoluteMove = IsYPosComplete (pBox);
          /* internal boxes take into account margins borders and paddings */
          if (isSysOrg)
            {
              orgTrans = t;
              middleTrans = 0;
              endTrans = 0;
            }
          else if (t || b)
            {
              orgTrans = t;
              middleTrans = (t - b)/2;
              endTrans = -b;
              if (t)
                /* internal boxes must be translated */
                absoluteMove = TRUE;
            }
          /* Moving included boxes? */
          if (absoluteMove &&
              (pBox->BxType == BoBlock ||
               pBox->BxType == BoFloatBlock || pBox->BxType == BoCellBlock))
            {
              /* manage stretched block of lines */
              /* which are already processed     */
              if (orgTrans)
                /* move also included boxes */
                YMoveAllEnclosed (pBox, orgTrans, frame);
            }
          else if (pAb->AbLeafType == LtCompound &&
                   pBox->BxType != BoGhost &&
                   pBox->BxType != BoStructGhost &&
                   pBox->BxType != BoFloatGhost &&
                   (absoluteMove || pAb->AbHeight.DimAbRef ||
                    pAb->AbHeight.DimValue >= 0))
            {
              /* the box is already built */
              /* or it's within a stretchable box */
              /* or it's within a unnested box */
              /* or it doesn't inherit the size from its contents */
              pRefAb = pAb->AbFirstEnclosed;
              while (pRefAb)
                {
                  if (!pRefAb->AbDead && !pRefAb->AbNew && pRefAb->AbBox)
                    {
                      box = pRefAb->AbBox;
                      /* check if the position box depends on its enclosing */
                      pPosRel = box->BxPosRelations;
                      while (pPosRel)
                        {
                          i = 0;
                          notEmpty = (pPosRel->PosRTable[i].ReBox != NULL);
                          while (i < MAX_RELAT_POS && notEmpty)
                            {
                              pRel = &pPosRel->PosRTable[i];
                              if (pRel->ReBox->BxAbstractBox)
                                {
                                  if (pRel->ReOp == OpVertInc
                                      && pRel->ReRefEdge != HorizRef)
                                    {
                                      switch (pRefAb->AbVertPos.PosRefEdge)
                                        {
                                        case HorizMiddle:
                                          /* recheck the position */
                                          j = pBox->BxTMargin + pBox->BxTBorder + pBox->BxTPadding + (pBox->BxH - box->BxHeight) / 2 - box->BxYOrg;
                                          if (absoluteMove)
                                            j += pBox->BxYOrg;
                                          if (box->BxVertFlex)
                                            MoveBoxEdge (box, pBox, pRel->ReOp, j, frame, FALSE);
                                          else
                                            YMove (box, pBox, j, frame);
                                          break;
                                        case Bottom:
                                          /* recheck the position */
                                          j = pBox->BxTMargin + pBox->BxTBorder + pBox->BxTPadding + pBox->BxH - box->BxHeight - box->BxYOrg;
                                          if (absoluteMove)
                                            j += pBox->BxYOrg;
                                          if (box->BxVertFlex)
                                            MoveBoxEdge (box, pBox, pRel->ReOp, j, frame, FALSE);
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
                                            MoveBoxEdge (box, pBox, pRel->ReOp, j, frame, FALSE);
                                          else
                                            YMove (box, pBox, j, frame);
                                          break;
                                        }
                                    }
                                }
                              i++;
                              if (i < MAX_RELAT_POS)
                                notEmpty = pPosRel->PosRTable[i].ReBox != NULL;
                            }
                          /* next relation block */
                          pPosRel = pPosRel->PosRNext;
                        }
                      if (pRefAb->AbTopMarginUnit == UnPercent ||
                          pRefAb->AbBottomMarginUnit == UnPercent)
                        /* update percent margins */
                        CheckMBP (pRefAb, box, frame, FALSE);
                    }
                  /* next child */
                  pRefAb = pRefAb->AbNext;
                }
            }
	  
          /* check dimension constraints */
          pDimRel = pBox->BxHeightRelations;
          while (pDimRel)
            {
              i = 0;
              while (i < MAX_RELAT_DIM && pDimRel->DimRTable[i])
                {
                  box = pDimRel->DimRTable[i];
                  pRefAb = box->BxAbstractBox;		    
                  if (pRefAb && !pRefAb->AbDead)
                    {
                      /* Is it the same dimension? */
                      if (pDimRel->DimROp[i] == OpSame)
                        {
                          /* Changing the height */
                          if (pRefAb->AbHeight.DimUnit == UnPercent)
                            {
                              if (pRefAb->AbEnclosing == pAb)
                                {
                                  if (pBox->BxType == BoBlock ||
                                      pBox->BxType == BoFloatBlock ||
                                      pBox->BxType == BoCellBlock ||
                                      pBox->BxType == BoGhost ||
                                      pBox->BxType == BoStructGhost ||
                                      pBox->BxType == BoFloatGhost)
                                    {
                                      /* inherit from the line height */
                                      pLine = SearchLine (box, frame);
                                      if (pLine == NULL)
                                        /* no line available */
                                        val = box->BxHeight;
                                      else
                                        val = pLine->LiHeight;
                                    }
                                  else
                                    /* refer the inside height */
                                    val = GetPercentDim (pRefAb, pAb, FALSE);
                                }
                              else
                                /* refer the outside height */
                                val = pBox->BxHeight;
                              val = val * pRefAb->AbHeight.DimValue / 100;
                              val = val - box->BxHeight;
                            }
                          else
                            {
                              val = delta;
                              if (pRefAb->AbEnclosing != pAb)
                                /* refer the outside width */
                                val = val + addT + addB;
                            }
                          /* avoid cycles on the same box */
                          if (box != pBox)
                            ChangeHeight (box, pSourceBox, pBox, val, frame);
                        }
                      else if (pDimRel->DimROp[i] == OpReverse)
                        {
                          /* Changing the width */
                          if (pRefAb->AbWidth.DimUnit == UnPercent)
                            {
                              if (pRefAb->AbEnclosing == pAb || pRefAb == pAb)
                                /* refer the inside height */
                                val = pBox->BxH;
                              else
                                /* refer the outside height */
                                val = pBox->BxHeight;
                              val = val * pRefAb->AbWidth.DimValue / 100;
                              val = val - box->BxWidth;
                            }
                          else
                            {
                              val = delta;
                              if (pRefAb->AbEnclosing != pAb)
                                /* refer the outside width */
                                val = val + addT + addB;
                            }
                          ChangeWidth (box, pSourceBox, NULL, val, 0, frame);
                        }
                    }
                  i++;
                }
              /* next relation block */
              pDimRel = pDimRel->DimRNext;
            }

          /* Check enclosing constraints */
          pParent = pAb->AbEnclosing;
          if (!toMove || pAb->AbFloat != 'N')
            {
              /* look for the enclosing block of lines  */
              while (pParent &&
                     pParent->AbBox->BxType != BoBlock &&
                     pParent->AbBox->BxType != BoFloatBlock &&
                     pParent->AbBox->BxType != BoCellBlock)
                pParent = pParent->AbEnclosing;
            }
	  
          if (pParent == NULL)
            {
              /* It's the root of the concrete image */
              if (Propagate == ToAll && pBox->BxYOrg < 0)
                YMoveAllEnclosed (pBox, -pBox->BxYOrg, frame);
            }
          else if (pAb->AbVertEnclosing)
            {
              /* keep in mind if the relation concerns parent boxes */
              externalRef = !IsParentBox (pParent->AbBox, pSourceBox);
              /*
               * if pSourceBox is a child and the inclusion is not performed
               * by another sibling box, we need to propagate the change
               */
              if (ExtraFlow (pBox, frame))
                  /* doesn't check enclosing of a positioned box */
                ;
              else if ((Propagate == ToAll || externalRef) &&
                       !IsSiblingBox (pBox, pFromBox) &&
                       !IsSiblingBox (pBox, pSourceBox))
                {
                  if (pParent->AbBox->BxType == BoBlock ||
                      pParent->AbBox->BxType == BoFloatBlock ||
                      pParent->AbBox->BxType == BoCellBlock)
                    {
                      if (pAb->AbHeight.DimAbRef != pParent)
                        /* Within a block of line */
                        EncloseInLine (pBox, frame, pParent);
                    }
                  /* if the inclusion is not checked at the end */
                  else if (!IsParentBox (pParent->AbBox, PackBoxRoot) &&
                           pParent->AbBox != pFromBox)
                    {
                      /* Differ the checking of the inclusion */
                      if (Propagate != ToAll && pParent->AbBox->BxType != BoCell)
                        RecordEnclosing (pParent->AbBox, FALSE);
                      /* Don't check the inclusion more than 2 times */
                      else if (pParent->AbBox->BxPacking <= 1)
                        HeightPack (pParent, pSourceBox, frame);
                    }
                  else if (pParent->AbBox->BxType == BoTable)
                    RecordEnclosing (pParent->AbBox, FALSE);
                }
              else if ((pBox->BxType == BoBlock ||
                        pBox->BxType == BoFloatBlock ||
                        pBox->BxType == BoCellBlock) &&
                       (pParent->AbBox->BxType == BoBlock ||
                        pParent->AbBox->BxType == BoGhost ||
                        pParent->AbBox->BxType == BoStructGhost ||
                        pParent->AbBox->BxType == BoFloatBlock ||
                        pParent->AbBox->BxType == BoCellBlock))
                /* Within a block of line */
                EncloseInLine (pBox, frame, pParent);
              else if (pAb->AbFloat == 'N' &&
                       !IsSiblingBox (pBox, pFromBox) &&
                       !IsSiblingBox (pBox, pSourceBox))
                RecordEnclosing (pParent->AbBox, FALSE);
            }
          else if (pBox->BxType == BoCell || pBox->BxType == BoCellBlock)
            /* it's a cell with a rowspan attribute */
            UpdateCellHeight (pAb, frame);
        }
    }
  
  /* Manage the specific width of symbols */
  if (pBox)
    {
      if (pAb->AbLeafType == LtSymbol)
        {
          font = pBox->BxFont;
          i = GetMathFontWidth (pAb->AbShape,
                                pBox->BxAbstractBox->AbEnclosing->AbBox->BxFont, pBox->BxH);
          switch (pAb->AbShape)
            {
            case 'i':	/* integral */
            case 'c':	/* circle integral */
              if (i == 0)
                i = BoxCharacterWidth (0xf3, 1, font);
              ResizeWidth (pBox, NULL, NULL, i - pBox->BxW, 0, 0, 0, frame, FALSE);
              break;
            case 'd':	/* double integral */
            case '1':	/* Clockwise Integral */
            case '2':	/* ClockwiseContourIntegral */
            case '3':	/* CounterClockwiseContourIntegral */
              if (i == 0)
                i = BoxCharacterWidth (0xf3, 1, font) * 3 / 2;
              ResizeWidth (pBox, NULL, NULL, i - pBox->BxW, 0, 0, 0, frame, FALSE);
              break;		
            case 'e':	/* double contour integral */
              if (i == 0)
                i = BoxCharacterWidth (0xf3, 1, font) * 2;
              ResizeWidth (pBox, NULL, NULL, i - pBox->BxW, 0, 0, 0, frame, FALSE);
              break;		
            case 't':	/* triple integral */
            case 'f':	/* triple contour integral */
              if (i == 0)
                i = BoxCharacterWidth (0xf3, 1, font) * 5 / 2;
              ResizeWidth (pBox, NULL, NULL, i - pBox->BxW, 0, 0, 0, frame, FALSE);
              break;		
            case '(':
            case ')':
            case '{':
            case '}':
            case '[':
            case ']':
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
            case 6:
              if (i == 0)
                i = BoxCharacterWidth (0xe6, 1, font);
              ResizeWidth (pBox, NULL, NULL, i - pBox->BxW, 0, 0, 0, frame, FALSE);
              break;
            case '<':
            case '>':
              if (i == 0)
                i = BoxCharacterWidth (0xf1, 1, font);
              ResizeWidth (pBox, NULL, NULL, i - pBox->BxW, 0, 0, 0, frame, FALSE);
              break;
            case '|':       /* VerticalLine */
            case 7:       /* VerticalSeparator ; U02758 */
            case 11:       /* VerticalBar ; U02223 */
            case 12:       /* DoubleVerticalBar ; U02956 */
              if (i == 0)
                i = BoxCharacterWidth (0x7c, 1, font);  /* | */
              if (pAb->AbShape == 12)
                i *= 2;
              ResizeWidth (pBox, NULL, NULL, i - pBox->BxW, 0, 0, 0, frame, FALSE);
              break;
            case 'D':       /* double vertical bar */
              if (i == 0)
                i = BoxCharacterWidth (0x7c, 1, font) * 3;  /* | */
              ResizeWidth (pBox, NULL, NULL, i - pBox->BxW, 0, 0, 0, frame, FALSE);
              break;

            case 'v':
            case '^': /* UpArrow */
            case 'V': /* DownArrow */
            case 155: /* UpDownArrow */
              if (i == 0)
                i  = BoxCharacterWidth (0x6c, 1, font);	/* 'm' */
              ResizeWidth (pBox, NULL, NULL, i - pBox->BxW, 0, 0, 0, frame, FALSE);
              break;

            case 161: /* UpTeeArrow */
            case 163: /* DownTeeArrow */
            case 166: /* RightUpVector */
            case 167: /* LeftUpVector */
            case 170: /* RightDownVector */
            case 171: /* LeftDownVector */
            case 173: /* UpArrowDownArrow */
            case 176: /* DoubleUpArrow */
            case 178: /* DoubleDownArrow */
            case 180: /* DoubleUpDownArrow */
            case 183: /* DownArrowUpArrow */
            case 190: /* UpArrowBar */
            case 191: /* DownArrowBar */
            case 193: /* RightUpDownVector */
            case 195: /* LeftUpDownVector */
            case 198: /* RightUpVectorBar */
            case 199: /* RightDownVectorBar */
            case 202: /* LeftUpVectorBar */
            case 203: /* LeftDownVectorBar */
            case 206: /* RightUpTeeVector */
            case 207: /* RightDownTeeVector */
            case 210: /* LeftUpTeeVector */
            case 211: /* LeftDownTeeVector */
            case 212: /* UpEquilibrium */
            case 213: /* ReverseUpEquilibrium */
              if (i == 0)
                i  = BoxCharacterWidth (SPACE, 1, font) * 4;
              ResizeWidth (pBox, NULL, NULL, i - pBox->BxW, 0, 0, 0, frame, FALSE);
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
void XMove (PtrBox pBox, PtrBox pFromBox, int delta, int frame)
{
  PtrBox              box;
  PtrAbstractBox      pParent, pRefAb, pChild, pAb;
  PtrPosRelations     pPosRel;
  BoxRelation        *pRel;
  int                 i;
  ThotBool            toComplete;
  ThotBool            notEmpty;
  ThotBool            checkParent, isSysOrg;
  ThotBool            absoluteMove;

  if (pBox == NULL)
    return;
  else
    pAb = pBox->BxAbstractBox;

 if (delta != 0 && pAb && !IsDead (pAb))
    {
      /* check if the box is in the history of moved boxes */
      if (pFromBox != NULL)
        {
          box = pFromBox->BxMoved;
          while (box != NULL)
            if (box == pBox)
              return;
            else if (box->BxMoved == pFromBox)
              box = NULL;
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
          if (pBox->BxType != BoSplit && pBox->BxType != BoMulScript &&
              pBox->BxHeight > 0)
            /* update the clipping region */
            UpdateBoxRegion (frame, pBox, delta, 0, 0, 0);
          /* Is the box not included? */
          else if (!pAb->AbVertEnclosing)
            /* update the clipping region */
            UpdateBoxRegion (frame, pBox, delta, 0, 0, 0);
        }

      isSysOrg = IsSystemOrigin (pAb, frame);
      /* Keep in mind if the box positionning is absolute or not */
      absoluteMove = IsXPosComplete (pBox);
      /*
       * Move only the origin if we're building the box
       * and it's not a stretchable box.
       * In other cases, move also enclosed boxes.
       */
      if (absoluteMove)
        {
          if (pBox->BxHorizFlex)
            {
              /* force the moving of this box and consider its children */ 
              pBox->BxXOrg += delta;
              absoluteMove = FALSE;
              pChild = pAb->AbFirstEnclosed;
              while (pChild)
                {
                  if (pChild->AbBox)
                    XMoveAllEnclosed (pChild->AbBox, delta, frame);
                  pChild = pChild->AbNext;
                }
            }
          else
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
        {
          pBox->BxXOrg += delta;
          if (pBox->BxType == BoMulScript &&
              (pAb->AbNotInLine || !SearchLine (pBox, frame)))
            {
              /* move script boxes too */
              box = pBox->BxNexChild;
              while (box)
                {
                  box->BxXOrg += delta;
                  box = box->BxNexChild;
                }
            }
          if (pBox->BxType == BoBlock || pBox->BxType == BoFloatBlock)
            // update the osition of floated boxes too
            ShiftFloatingBoxes (pBox, delta, frame);
        }
	  
      /* Check the validity of dependency rules */
      checkParent = TRUE;
      if (pAb->AbEnclosing && pAb->AbEnclosing->AbBox)
        checkParent = (pAb->AbEnclosing->AbBox->BxType != BoGhost &&
                       pAb->AbEnclosing->AbBox->BxType != BoStructGhost &&
                       pAb->AbEnclosing->AbBox->BxType != BoFloatGhost);
      
      if (pAb->AbNotInLine ||
          ((pAb->AbEnclosing &&  pAb->AbEnclosing->AbBox &&
            pAb->AbFloat == 'N' &&
            (pAb->AbEnclosing->AbBox->BxType == BoGhost ||
             pAb->AbEnclosing->AbBox->BxType == BoStructGhost ||
             pAb->AbEnclosing->AbBox->BxType == BoFloatGhost ||
             pAb->AbEnclosing->AbBox->BxType == BoBlock ||
             pAb->AbEnclosing->AbBox->BxType == BoFloatBlock ||
             pAb->AbEnclosing->AbBox->BxType == BoCellBlock))))
        checkParent = FALSE;
      else
        {
          /* Move remaining dependent boxes */
          pPosRel = pBox->BxPosRelations;
          while (pPosRel != NULL)
            {
              i = 0;
              notEmpty = (pPosRel->PosRTable[i].ReBox != NULL);
              while (i < MAX_RELAT_POS && notEmpty)
                {
                  pRel = &pPosRel->PosRTable[i];
                  pRefAb = pRel->ReBox->BxAbstractBox;
                  if (pRefAb &&
                      pRel->ReBox->BxType != BoGhost &&
                      pRel->ReBox->BxType != BoStructGhost &&
                      pRel->ReBox->BxType != BoFloatGhost &&
                      // don't move children of a new system origin
                      (!IsParentBox (pBox, pRel->ReBox) || !isSysOrg))
                    {
                      /* Left, Right, Middle, and Vertical axis */
                      if (pRel->ReOp == OpHorizRef)
                        {
                          /* except its vertical axis */
                          if (pRel->ReBox != pBox)
                            {
                              pParent = pAb->AbEnclosing;
                              if (pParent != NULL)
                                box = pParent->AbBox;
                              else
                                box = NULL;
                              if (pRefAb->AbEnclosing != pAb &&
                                  (pRel->ReBox != box || Propagate == ToAll))
                                MoveVertRef (pRel->ReBox, pBox, delta, frame);
                            }
                        }
                      /* Ignore the back relation of a stretchable box */
                      else if (pBox->BxHorizFlex &&
                               pAb != pRel->ReBox->BxAbstractBox->AbHorizPos.PosAbRef)
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
                              if (pRel->ReOp == OpHorizDep &&
                                  !pRel->ReBox->BxXOutOfStruct)
                                /* Valid relation with the box origin */
                                {
                                  if (pRel->ReBox->BxHorizFlex &&
                                      /* if it's not a child */
                                      pAb != pRel->ReBox->BxAbstractBox->AbEnclosing &&
                                      pAb == pRel->ReBox->BxAbstractBox->AbHorizPos.PosAbRef)
                                    MoveBoxEdge (pRel->ReBox, pBox, pRel->ReOp, delta,
                                                 frame, TRUE);
                                  else
                                    XMove (pRel->ReBox, pBox, delta, frame);
                                }
                            }
                        }
                      else if (pRel->ReOp == OpHorizDep && !pRel->ReBox->BxHorizFlex)
                        XMove (pRel->ReBox, pBox, delta, frame);
                      else if ((pRel->ReOp == OpHorizDep &&
                                pAb == pRel->ReBox->BxAbstractBox->AbHorizPos.PosAbRef) ||
                               pRel->ReOp == OpWidth)
                        {
                          if (!IsParentBox (pBox, pRel->ReBox) || !isSysOrg)
                            MoveBoxEdge (pRel->ReBox, pBox, pRel->ReOp, delta, frame, TRUE);
                        }
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
        }
	  
      /* Do we have to recompute the width of the enclosing box */
      pParent = pAb->AbEnclosing;
      if (checkParent && pBox->BxXOutOfStruct && pParent &&
          /* table evaluation is done by a specific algorithm */
          pBox->BxType != BoCell && pBox->BxType != BoCellBlock && pBox->BxType != BoColumn)
        /*
         * cannot compute it if this box is not placed
         * or if the management is differed.
         */
        if (!pParent->AbBox->BxXToCompute
            && !IsParentBox (pParent->AbBox, PackBoxRoot)
            && (Propagate == ToAll || !IsParentBox (pParent->AbBox, pFromBox))
            && !IsSiblingBox (pBox, pFromBox)
            && pAb->AbHorizEnclosing)
          /*
           * if pSourceBox is a child
           * and the inclusion is not performed
           * by another sibling box
           * and the box is included
           * we need to propagate the change
           */
          WidthPack (pParent, pFromBox, frame);
    }
}

/*----------------------------------------------------------------------
  YMove moves vertically the box.
  Check positionning constraints.
  ----------------------------------------------------------------------*/
void YMove (PtrBox pBox, PtrBox pFromBox, int delta, int frame)
{
  PtrBox              box;
  PtrAbstractBox      pParent, pRefAb, pChild, pAb;
  PtrPosRelations     pPosRel;
  BoxRelation        *pRel;
  int                 i;
  ThotBool            toComplete, notEmpty;
  ThotBool            checkParent, isSysOrg;
  ThotBool            absoluteMove;

  if (pBox == NULL)
    return;
  else
    pAb = pBox->BxAbstractBox;
  if (delta != 0 && pAb && !IsDead (pAb))
    {
      /* check if the box is in the history of moved boxes */
      if (pFromBox != NULL)
        {
          box = pFromBox->BxMoved;
          while (box != NULL)
            if (box == pBox)
              return;
            else if (box->BxMoved == pFromBox)
              box = NULL;
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
          if (pBox->BxType != BoSplit && pBox->BxType != BoMulScript &&
              pBox->BxWidth > 0)
            /* update the clipping region */
            UpdateBoxRegion (frame, pBox, 0, delta, 0, 0);
          /* Is the box not included? */
          else if (!pAb->AbHorizEnclosing)
            /* update the clipping region */
            UpdateBoxRegion (frame, pBox, 0, delta, 0, 0);
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
          if (pBox->BxVertFlex)
            {
              /* force the moving of this box and consider its children */ 
              pBox->BxYOrg += delta;
             absoluteMove = FALSE;
              pChild = pAb->AbFirstEnclosed;
              while (pChild)
                {
                  if (pChild->AbBox)
                    YMoveAllEnclosed (pChild->AbBox, delta, frame);
                  pChild = pChild->AbNext;
                }
            }
          else
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
        {
          pBox->BxYOrg += delta;
          if (pBox->BxType == BoMulScript &&
              (pAb->AbNotInLine || !SearchLine (pBox, frame)))
            {
              /* move script boxes too */
              box = pBox->BxNexChild;
              while (box)
                {
                  box->BxYOrg += delta;
                  box = box->BxNexChild;
                }
            }
        }

      isSysOrg = IsSystemOrigin (pAb, frame);
      /* Check the validity of dependency rules */
      checkParent = TRUE;
      if (pAb->AbEnclosing && pAb->AbEnclosing->AbBox)
        checkParent = (pAb->AbEnclosing->AbBox->BxType != BoGhost &&
                       pAb->AbEnclosing->AbBox->BxType != BoStructGhost &&
                       pAb->AbEnclosing->AbBox->BxType != BoFloatGhost);
      if (pAb->AbNotInLine ||
          ((pAb->AbEnclosing &&  pAb->AbEnclosing->AbBox &&
            pAb->AbFloat == 'N' &&
            (pAb->AbEnclosing->AbBox->BxType == BoGhost ||
             pAb->AbEnclosing->AbBox->BxType == BoStructGhost ||
             pAb->AbEnclosing->AbBox->BxType == BoFloatGhost ||
             pAb->AbEnclosing->AbBox->BxType == BoBlock ||
             pAb->AbEnclosing->AbBox->BxType == BoFloatBlock ||
             pAb->AbEnclosing->AbBox->BxType == BoCellBlock))))
        checkParent = FALSE;
      else
        {
          /* Move remaining dependent boxes */
          pPosRel = pBox->BxPosRelations;
          while (pPosRel != NULL)
            {
              i = 0;
              notEmpty = (pPosRel->PosRTable[i].ReBox != NULL);
              while (i < MAX_RELAT_POS && notEmpty)
                {
                  pRel = &pPosRel->PosRTable[i];
                  pRefAb = pRel->ReBox->BxAbstractBox;
                  if (pRefAb &&
                      pRel->ReBox->BxType != BoGhost &&
                      pRel->ReBox->BxType != BoStructGhost &&
                      pRel->ReBox->BxType != BoFloatGhost &&
                      // don't move children of a new system origin
                      (!IsParentBox (pBox, pRel->ReBox) || !isSysOrg))
                    {
                      /* Top, Bottom, Middle, and Baseline */
                      if (pRel->ReOp == OpVertRef)
                        {
                          /* except its baseline */
                          if (pRel->ReBox != pBox)
                            {
                              pParent = pAb->AbEnclosing;
                              if (pParent)
                                box = pParent->AbBox;
                              else
                                box = NULL;
                              if (pRefAb->AbEnclosing != pAb &&
                                  (pRel->ReBox != box || Propagate == ToAll))
                                /* don't change internal refs */
                                MoveHorizRef (pRel->ReBox, pBox, delta, frame);
                            }
                        }
                      /* Ignore the back relation of a stretchable box */
                      else if (pBox->BxVertFlex &&
                               pAb != pRefAb->AbVertPos.PosAbRef)
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
                              if (pRel->ReOp == OpVertDep &&
                                  !pRel->ReBox->BxYOutOfStruct)
                                {
                                  /* Valid relation with the box origin */
                                  if (pRel->ReBox->BxVertFlex &&
                                      /* if it's not a child */
                                      pAb != pRefAb->AbEnclosing &&
                                      pAb == pRefAb->AbVertPos.PosAbRef)
                                    MoveBoxEdge (pRel->ReBox, pBox,
                                                 pRel->ReOp, delta,
                                                 frame, FALSE);
                                  else
                                    YMove (pRel->ReBox, pBox, delta, frame);
                                }
                            }
                        }
                      else if (pRel->ReOp == OpVertDep &&
                               !pRel->ReBox->BxVertFlex)
                        YMove (pRel->ReBox, pBox, delta, frame);
                      else if ((pRel->ReOp == OpVertDep &&
                                pAb == pRefAb->AbVertPos.PosAbRef) ||
                               pRel->ReOp == OpHeight)
                        {
                          if (!IsParentBox (pBox, pRel->ReBox) ||
                              !isSysOrg)
                            MoveBoxEdge (pRel->ReBox,
                                         pBox, pRel->ReOp, delta,
                                         frame, FALSE);
                        }
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
        }
	  
      /* Do we have to recompute the height of the enclosing box */
      pParent = pAb->AbEnclosing;
      if (checkParent && pBox->BxYOutOfStruct && pParent != NULL)
        /*
         * cannot compute it if this box is not placed
         * or if the management is differed.
         */
        if (!pParent->AbBox->BxYToCompute
            && !IsParentBox (pParent->AbBox, PackBoxRoot)
            && (Propagate == ToAll || !IsParentBox (pParent->AbBox, pFromBox))
            && !IsSiblingBox (pBox, pFromBox)
            && pAb->AbVertEnclosing)
          /*
           * if pSourceBox is a child
           * and the inclusion is not performed
           * by another sibling box
           * and the box is included
           * we need to propagate the change
           */
          HeightPack (pParent, pFromBox, frame);
    }
}

/*----------------------------------------------------------------------
  Shrink shrinks to nested boxes.
  ----------------------------------------------------------------------*/
static void Shrink (PtrAbstractBox pAb, PtrBox pSourceBox, int frame)
{
  PtrAbstractBox      pChildAb;
  PtrBox              pChildBox, pBox;
  int                 x, width;
  ThotBool            absoluteMove;

  pBox = pAb->AbBox;
  if (pBox && !pAb->AbDead)
    {
      absoluteMove = IsXPosComplete (pBox);
      if (absoluteMove)
        x = pBox->BxXOrg + pBox->BxLMargin + pBox->BxLBorder + pBox->BxLPadding;
      else
        x = 0;
      width = x;
      pChildAb = pAb->AbFirstEnclosed;
      while (pChildAb)
        {
          pChildBox = pChildAb->AbBox;
          if (!pChildAb->AbDead && pChildBox &&
              pChildAb->AbHorizEnclosing &&
              pChildAb->AbFloat == 'N' &&
              !ExtraFlow (pChildBox, frame) &&
              pChildAb->AbVisibility >= ViewFrameTable[frame - 1].FrVisibility &&
              pChildBox->BxXOrg + pChildBox->BxWidth > width)
            {
              if (pChildBox->BxXOrg + pChildBox->BxWidth > x + pBox->BxW)
                width = x + pBox->BxW;
              else
                width = pChildBox->BxXOrg + pChildBox->BxWidth;
            }
          pChildAb = pChildAb->AbNext;
        }
      width -= x;
      if (width == pBox->BxW)
        return;
      ResizeWidth (pBox, pSourceBox, NULL, width - pBox->BxW, 0, 0, 0, frame, TRUE);
    }
}

/*----------------------------------------------------------------------
  WidthPack checks the horizontal inclusion of nested boxes.
  ----------------------------------------------------------------------*/
void WidthPack (PtrAbstractBox pAb, PtrBox pSourceBox, int frame)
{
  PtrAbstractBox      pChildAb, pRelativeAb, pRefAb;
  PtrBox              pChildBox, pRelativeBox, pBox;
  AbDimension        *pDimAb;
  AbPosition         *pPosAb;
  int                 val, width;
  int                 left;
  int                 x, i, j;
  ThotBool            movingChild, toMove, isSysOrg;
  ThotBool            absoluteMove, isExtra;

  /*
   * Check if the width depends on the box contents
   * and if we're not already doing the job
   */
  if (pAb == NULL)
    return;
  pBox = pAb->AbBox;
  if (pBox == NULL)
    return;

  pDimAb = &pAb->AbWidth;
  if (pBox->BxType == BoBlock ||
      pBox->BxType == BoFloatBlock || pBox->BxType == BoCellBlock)
    /* don't pack a block or a cell but transmit to enclosing box */
    WidthPack (pAb->AbEnclosing, pSourceBox, frame);
  else if (pBox->BxType == BoCell || pBox->BxType == BoTable ||
           pBox->BxType == BoRow)
    /* width of these elements is computed in tableH.c */
    return;
  else if (pBox->BxType == BoGhost ||
           //pBox->BxType == BoStructGhost ||
           pBox->BxType == BoFloatGhost ||
           pBox->BxType == BoColumn)
    /* don't pack a column head or a ghost element */
    return;
  else if (pBox->BxShrink)
    Shrink (pAb, pSourceBox, frame);
  else if (pBox->BxContentWidth ||
           (!pDimAb->DimIsPosition && pDimAb->DimMinimum))
    {
      /* register that we're preforming the job */
      pBox->BxCycles += 1;
      /* Keep in mind if the box positionning is absolute or not */
      absoluteMove = IsXPosComplete (pBox);
      if (absoluteMove)
        x = pBox->BxXOrg + pBox->BxLMargin + pBox->BxLBorder + pBox->BxLPadding;
      else
        x = 0;
      
      isSysOrg = IsSystemOrigin (pAb, frame);
      /* Initially the inside left and the inside right are the equal */
      width = x;
      /* left gives the current left limit of mobile boxes */
      left = x + pBox->BxW;
      movingChild = FALSE;
      /* nothing is moved */
      toMove = FALSE;
      /* the box itself is positioned */
      isExtra = ExtraFlow (pBox, frame);
      /*
       * The left edge of the lefter enclosed box must be stuck
       * to the inside left edge and the inside width is delimited
       * by the right edge of the righter enclosed box.
       */
      pChildAb = pAb->AbFirstEnclosed;
      while (pChildAb)
        {
          pChildBox = pChildAb->AbBox;
          if (!pChildAb->AbDead && pChildBox &&
              pChildAb->AbHorizEnclosing &&
              !ExtraFlow (pChildBox, frame) &&
              pChildAb->AbVisibility >= ViewFrameTable[frame - 1].FrVisibility &&
              (FrameTable[frame].FrView != 1 ||
               !TypeHasException (ExcIsMap, pChildAb->AbElement->ElTypeNumber,
                                  pChildAb->AbElement->ElStructSchema)) &&
              (pChildAb->AbWidth.DimAbRef != pAb ||
               pChildBox->BxContentWidth ||
               /* Sometimes table width doesn't follow the rule */
               pChildBox->BxType == BoTable))
            {
              /* look for the box which relies the box to its enclosing */
              pRelativeBox = GetHPosRelativeBox (pChildBox, NULL);
              if (pRelativeBox != NULL)
                if (pRelativeBox->BxAbstractBox != NULL)
                  {
                    pRelativeAb = pRelativeBox->BxAbstractBox;
                    /* register the abstract box which gives the width */
                    pRefAb = pChildAb->AbWidth.DimAbRef;
                    if (pRelativeAb->AbHorizPos.PosAbRef == NULL)
                      {
                        /* mobile box */
                        movingChild = TRUE;
                        if (pChildBox->BxXOrg < left)
                          /* minimum value */
                          left = pChildBox->BxXOrg;
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
                            && pRefAb->AbWidth.DimValue < 0
                            && pRefAb->AbBox->BxWidth == pBox->BxWidth)
                          i = x;
                        else if (pChildBox->BxXOrg < x)
                          /* don't take into account negative origins */
                          i = x + pChildBox->BxWidth;
                        else
                          i = pChildBox->BxXOrg + pChildBox->BxWidth;
                      }
                    else if (pChildBox->BxXOrg < x ||
                             (pChildAb->AbHorizPos.PosDistance < 0 &&
                              pChildAb->AbHorizPos.PosRefEdge == Left))
                      /* don't take into account negative origins */
                      i = x + pChildBox->BxWidth;
                    else
                      i = pChildBox->BxXOrg + pChildBox->BxWidth;
                    if (i > width)
                      width = i;
                  }
            }
          else if (pChildBox &&
                   (pChildBox->BxType == BoBlock ||
                    pChildBox->BxType == BoFloatBlock ||
                    pChildBox->BxType == BoCellBlock) &&
                   !ExtraFlow (pChildBox, frame) &&
                   width < pChildBox->BxXOrg + pChildBox->BxMinWidth)
            // apply the rule ob minimum
            width = pChildBox->BxXOrg + pChildBox->BxMinWidth;

          pChildAb = pChildAb->AbNext;
        }
      
      val = x - left; /* Shift of the extra left edge */
      if (movingChild)
        width += val; /* Next position of the extra right edge */
      if (width == x && pAb->AbVolume == 0)
        width = 2;
      else
        width -= x;
      x = width - pBox->BxW; /* widths difference */
      
      /*
       * Now we move misplaced included boxes
       */
      pChildAb = pAb->AbFirstEnclosed;
      if (movingChild && val != 0)
        while (pChildAb)
          {
            pChildBox = pChildAb->AbBox;
            if (!pChildAb->AbDead && pChildBox &&
                pChildAb->AbVisibility >= ViewFrameTable[frame - 1].FrVisibility &&
                (FrameTable[frame].FrView != 1 ||
                 !TypeHasException (ExcIsMap, pChildAb->AbElement->ElTypeNumber,
                                    pChildAb->AbElement->ElStructSchema)))
              {
                /* look for the box which relies the box to its enclosing */
                pRelativeBox = GetHPosRelativeBox (pChildBox, NULL);
                if (pRelativeBox && pRelativeBox->BxAbstractBox &&
                    pRelativeBox->BxAbstractBox->AbHorizPos.PosAbRef == NULL)
                  {
                    /* update the clipping area for the future */
                    if (ReadyToDisplay)
                      /* update the clipping region */
                      UpdateBoxRegion (frame, pChildBox, val, 0, 0, 0);
    
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
                            i = GetPixelValue (pPosAb->PosDistance, pPosAb->PosUnit,
                                               pAb->AbBox->BxW, pAb,
                                               ViewFrameTable[frame - 1].FrMagnification);
                            j = GetPixelValue (pPosAb->PosDistDelta, pPosAb->PosDeltaUnit,
                                               pAb->AbBox->BxW, pAb,
                                               ViewFrameTable[frame - 1].FrMagnification);
                            if (pChildBox->BxType == BoMulScript &&
                                pChildBox->BxNexChild)
                              pChildBox = pChildBox->BxNexChild;
                            i = i + j + pChildBox->BxXOrg + pChildBox->BxVertRef - pBox->BxXOrg;
                            MoveVertRef (pBox, pChildBox, i - pBox->BxVertRef, frame);
                          }
                      }
                  }
              }
            pChildAb = pChildAb->AbNext;
          }
      
      if (x)
        /* update the enclosing box width */
        ChangeDefaultWidth (pBox, pSourceBox, width, 0, frame);
      /* Now check the anscestor box */
      else if (toMove)
        {
          if (pAb->AbEnclosing == NULL)
            {
              if (pBox->BxXOrg < 0)
                XMoveAllEnclosed (pBox, -pBox->BxXOrg, frame);
            }
          else if (pAb->AbEnclosing->AbBox->BxType != BoBlock &&
                   pAb->AbEnclosing->AbBox->BxType != BoFloatBlock &&
                   pAb->AbEnclosing->AbBox->BxType != BoCellBlock &&
                   pAb->AbEnclosing->AbBox->BxType != BoGhost &&
                   //pAb->AbEnclosing->AbBox->BxType != BoStructGhost &&
                   pAb->AbEnclosing->AbBox->BxType != BoFloatGhost &&
                   !isExtra /* doesn't check enclosing of a positioned box */)
            WidthPack (pAb->AbEnclosing, pSourceBox, frame);
        }
      /* the job is performed */
      pBox->BxCycles -= 1;
    }
  else if (!pDimAb->DimIsPosition &&
           pDimAb->DimAbRef == pAb->AbEnclosing &&
           pAb->AbEnclosing &&
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
void HeightPack (PtrAbstractBox pAb, PtrBox pSourceBox, int frame)
{
  PtrAbstractBox      pChildAb, pRelativeAb, pRefAb;
  PtrBox              pChildBox, pRelativeBox, pBox;
  PtrFlow             pFlow;
  AbDimension        *pDimAb;
  AbPosition         *pPosAb;
  int                 val, height;
  int                 y, i, j, top, bottom;
  ThotBool            movingChild;
  ThotBool            toMove, isExtra;
  ThotBool            absoluteMove;

  /*
   * Check if the height depends on the box contents
   * and if we're not already doing the job
   */
  if (pAb == NULL)
    return;
  pBox = pAb->AbBox;
  if (pBox == NULL)
    return;
  pDimAb = &pAb->AbHeight;
  if (pBox->BxType == BoBlock ||
      pBox->BxType == BoFloatBlock || pBox->BxType == BoCellBlock)
    /* don't pack a block or a cell but transmit to enclosing box */
    HeightPack (pAb->AbEnclosing, pSourceBox, frame);
  else if (pBox->BxType == BoGhost ||
           //pBox->BxType == BoStructGhost ||
           pBox->BxType == BoFloatGhost)
    return;
  else if (pBox->BxContentHeight ||
           (!pDimAb->DimIsPosition && pDimAb->DimMinimum))
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
      /* top gives the current top limit of mobile boxes */
      top = y + pBox->BxH;
      movingChild = FALSE;
      /* nothing is moved */
      toMove = FALSE;
      /* the box itself is positioned */
      isExtra = ExtraFlow (pBox, frame);      
      /*
       * The top edge of the upper enclosed box must be stuck
       * to the inside top edge and the inside height is delimited
       * by the bottom edge of the lower enclosed box.
       */
      pChildAb = pAb->AbFirstEnclosed;
      while (pChildAb)
        {
          pChildBox = pChildAb->AbBox;
          if (!pChildAb->AbDead && pChildBox &&
              pChildAb->AbVertEnclosing &&
              pChildAb->AbVisibility >= ViewFrameTable[frame - 1].FrVisibility &&
                (FrameTable[frame].FrView != 1 ||
                 !TypeHasException (ExcIsMap, pChildAb->AbElement->ElTypeNumber,
                                    pChildAb->AbElement->ElStructSchema)) &&
              (pChildAb->AbHeight.DimAbRef != pAb ||
               pChildBox->BxContentHeight))
            {
              /* look for the box which relies the box to its enclosing */
              pRelativeBox = GetVPosRelativeBox (pChildBox, NULL);
              if (pRelativeBox && pRelativeBox->BxAbstractBox)
                {
                  pRelativeAb = pRelativeBox->BxAbstractBox;
                  /* register the abstract box which gives the height */
                  pRefAb = pChildAb->AbHeight.DimAbRef;
                  if (pRelativeAb->AbVertPos.PosAbRef == NULL)
                    {
                      /* mobile box */
                      movingChild = TRUE;
                      /* minimum value */
                      if (pChildBox->BxYOrg < top)
                        top = pChildBox->BxYOrg;
                      i = pChildBox->BxYOrg + pChildBox->BxHeight;
                    }
                  /* the box position depends on the enclosing height? */
                  else if (pRelativeAb->AbVertPos.PosAbRef == pAb &&
                           (pRelativeAb->AbVertPos.PosRefEdge != Top ||
                            (pRelativeAb->AbHeight.DimAbRef == pAb &&
                             !pRelativeAb->AbHeight.DimIsPosition &&
                             pChildAb->AbVertPos.PosRefEdge != Top)))
                    i = y + pChildBox->BxHeight;
                  /* the box height depends on a not included box? */
                  else if (pChildBox->BxHOutOfStruct)
                    {
                      /* which itself inherits form the enclosing? */
                      if (pRefAb && IsParentBox (pRefAb->AbBox, pBox)
                          && pRefAb->AbHeight.DimAbRef == NULL
                          && pRefAb->AbHeight.DimValue < 0
                          && pRefAb->AbBox->BxHeight == pBox->BxHeight)
                        i = y;
                      else if (pChildBox->BxYOrg < y)
                        /* don't take into account negative origins */
                        i = y + pChildBox->BxHeight;
                      else
                        i = pChildBox->BxYOrg + pChildBox->BxHeight;
                    }
                  else if (pChildBox->BxYOrg < y || pChildBox->BxType == BoCell)
                    /* don't take into account negative origins */
                    i = y + pChildBox->BxHeight;
                  else if (pBox->BxType == BoStructGhost &&
                           pAb->AbVertPos.PosAbRef == pChildAb->AbVertPos.PosAbRef)
                    /* inherit from enclosed box */
                    i = pChildBox->BxHeight + pBox->BxTPadding + pBox->BxTBorder + pBox->BxBPadding + pBox->BxBBorder;
                 else
                    i = pChildBox->BxYOrg + pChildBox->BxHeight;
                  if (pChildAb->AbNext == NULL &&
                      pBox->BxBBorder + pBox->BxBPadding == 0)
                    {
                      /* collapsing margins */
                      if (pChildBox->BxBMargin > pBox->BxBMargin)
                        i -= pBox->BxBMargin;
                      else
                        i -= pChildBox->BxBMargin;
                    }
                  if (i > height)
                    height = i;
                }
            }
          pChildAb = pChildAb->AbNext;
        }

      // The body or root element should be extended?
      if (TypeHasException (ExcSetWindowBackground, pAb->AbElement->ElTypeNumber,
                            pAb->AbElement->ElStructSchema))
        {
          pFlow = ViewFrameTable[frame - 1].FrFlow;
          bottom = pBox->BxBMargin;
          while (pFlow && pFlow->FlRootBox && pFlow->FlRootBox->AbBox)
            {
              // take into account the height of the positionned child
              i = pFlow->FlRootBox->AbBox->BxYOrg + pFlow->FlRootBox->AbBox->BxHeight;
              if (i - bottom - pFlow->FlRootBox->AbBox->BxBMargin > height)
                height = i - bottom - pFlow->FlRootBox->AbBox->BxBMargin;
              pFlow = pFlow->FlNext;
            }
        }

      val = y - top; /* Shift of the extra top edge */
      if (movingChild)
        height += val; /* Nex position of the extra bottom edge */
      if (height == y && pAb->AbVolume == 0)
        height = GetCurrentFontHeight (pAb->AbSize, pAb->AbSizeUnit, frame);
      else
        height -= y;
      y = height - pBox->BxH; /* heights difference */
      
      /*
       * Now we move misplaced included boxes
       */
      pChildAb = pAb->AbFirstEnclosed;
      if (movingChild && val != 0)
        while (pChildAb)
          {
            pChildBox = pChildAb->AbBox;
            if (!pChildAb->AbDead && pChildBox &&
                pChildAb->AbVisibility >= ViewFrameTable[frame - 1].FrVisibility &&
                (FrameTable[frame].FrView != 1 ||
                 !TypeHasException (ExcIsMap, pChildAb->AbElement->ElTypeNumber,
                                    pChildAb->AbElement->ElStructSchema)))
              {
                /* look for the box which relies the box to its enclosing */
                pRelativeBox = GetVPosRelativeBox (pChildBox, NULL);
                if (pRelativeBox && pRelativeBox->BxAbstractBox &&
                    pRelativeBox->BxAbstractBox->AbVertPos.PosAbRef == NULL)
                  {
                    /* update the clipping area for the redisplay */
                    if (ReadyToDisplay)
                      /* update the clipping region */
                      UpdateBoxRegion (frame, pChildBox, 0, val, 0, 0);

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
                            i = GetPixelValue (pPosAb->PosDistance, pPosAb->PosUnit,
                                               pAb->AbBox->BxH, pAb,
                                               ViewFrameTable[frame - 1].FrMagnification);
                            j = GetPixelValue (pPosAb->PosDistDelta, pPosAb->PosDeltaUnit,
                                               pAb->AbBox->BxH, pAb,
                                               ViewFrameTable[frame - 1].FrMagnification);
                            if (pChildBox->BxType == BoMulScript &&
                                pChildBox->BxNexChild)
                              pChildBox = pChildBox->BxNexChild;
                            i = i + j + pChildBox->BxYOrg + pChildBox->BxHorizRef - pBox->BxYOrg;
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
        {
          if (pAb->AbEnclosing == NULL || pAb->AbEnclosing->AbBox == NULL)
            {
              if (pBox->BxYOrg < 0)
                YMoveAllEnclosed (pBox, -pBox->BxYOrg, frame);
            }
          else if (pAb->AbEnclosing->AbBox->BxType == BoGhost ||
                   //pAb->AbEnclosing->AbBox->BxType == BoStructGhost ||
                   pAb->AbEnclosing->AbBox->BxType == BoFloatGhost ||
                   pAb->AbEnclosing->AbBox->BxType == BoBlock ||
                   pAb->AbEnclosing->AbBox->BxType == BoFloatBlock ||
                   pAb->AbEnclosing->AbBox->BxType == BoCellBlock)
            {
              /* Il faut remonter au pave de mise en lignes */
              while (pAb->AbEnclosing->AbBox->BxType == BoGhost ||
                     //pAb->AbEnclosing->AbBox->BxType == BoStructGhost ||
                     pAb->AbEnclosing->AbBox->BxType == BoFloatGhost)
                pAb = pAb->AbEnclosing;
              EncloseInLine (pBox, frame, pAb->AbEnclosing);
            }
          else if (!isExtra /* doesn't check enclosing of a positioned box */)
            HeightPack (pAb->AbEnclosing, pSourceBox, frame);
        }
      /* the job is performed */
      pBox->BxPacking -= 1;
    }
}
