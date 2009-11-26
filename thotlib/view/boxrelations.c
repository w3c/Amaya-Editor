/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2009
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * gestion des Relations entre boites
 *
 * Author: I. Vatton (INRIA)
 *
 */
#include "thot_gui.h"
#include "thot_sys.h"
#include "libmsg.h"
#include "message.h"
#include "constmedia.h"
#include "typemedia.h"
#include "frame.h"

#ifdef _GL
#include "glwindowdisplay.h"
#include "content.h"
#endif /* _GL */

#define THOT_EXPORT extern
#include "boxes_tv.h"
#ifdef _GL
#include "frame_tv.h"
#endif /* _GL */


#include "absboxes_f.h"
#include "appli_f.h"
#include "boxmoves_f.h"
#include "boxpositions_f.h"
#include "boxrelations_f.h"
#include "buildboxes_f.h"
#include "exceptions_f.h"
#include "font_f.h"
#include "memory_f.h"

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
static void ClearBoxMoved (PtrBox pBox)
{
  PtrBox           pNextBox;
  while (pBox != NULL)
    {
      pNextBox = pBox->BxMoved;
      pBox->BxMoved = NULL;
      pBox = pNextBox;
    }
}
 
/*----------------------------------------------------------------------
  GetPosRelativeAb retourne le pointeur sur le pave de reference    
  pour le positionnement implicite, horizontal/vertical,  
  du pave pCurrentAb.                                     
  Si la valeur rendue est NULL, le pave se positionne par 
  rapport au pave englobant.                              
  ----------------------------------------------------------------------*/
static PtrAbstractBox GetPosRelativeAb (PtrAbstractBox pCurrentAb,
                                        ThotBool horizRef)
{
  ThotBool            still;
  PtrAbstractBox      pAb;

  still = TRUE;
  pAb = pCurrentAb->AbPrevious;
  while (still)
    if (pAb == NULL)
      still = FALSE;
    else if (pAb->AbDead)
      pAb = pAb->AbPrevious;
    else if (horizRef)
      if (pAb->AbHorizPos.PosAbRef == NULL)
        still = FALSE;
      else
        pAb = pAb->AbPrevious;
    else if (pAb->AbVertPos.PosAbRef == NULL)
      still = FALSE;
    else
      pAb = pAb->AbPrevious;

  return pAb;
}


/*----------------------------------------------------------------------
  InsertPosRelation stores position relations between box edges.
  - One relation from Enclosing box to child box (OpHorizInc or OpVertInc).
  - Two relations for sibling boxes (OpHorizDep or OpVertDep).
  - One relation for Alignment axes (OpHorizRef or OpVertRef).
  - Reverse relation for strechable dimensions (OpWidth or OpHeight).
  ----------------------------------------------------------------------*/
static void InsertPosRelation (PtrBox pOrginBox, PtrBox pTargetBox,
                               OpRelation op, BoxEdge originEdge,
                               BoxEdge targetEdge)
{
  int                 i;
  ThotBool            loop;
  ThotBool            empty;
  PtrPosRelations     pPreviousPosRel, pNext;
  PtrPosRelations     pPosRel;
  BoxRelation        *pRelation;

  i = 0;
  /* add a relation from origin to target */
  if (op != OpHorizRef && op != OpVertRef && op != OpWidth && op != OpHeight)
    {
      /* look for an empty entry */
      pPosRel = pOrginBox->BxPosRelations;
      pPreviousPosRel = NULL;
      loop = TRUE;
      while (loop && pPosRel)
        {
          i = 0;
          pPreviousPosRel = pPosRel;
          do
            {
              empty = (pPosRel->PosRTable[i].ReBox == NULL);
              i++;
            }
          while (i != MAX_RELAT_POS && !empty);
	  
          if (empty)
            {
              loop = FALSE;
              i--;
            }
          else
            /* next block */
            pPosRel = pPosRel->PosRNext;
        }

      if (loop)
        {
          /* add a new block */
          pNext = pPosRel;
          GetPosBlock (&pPosRel);
          pPosRel->PosRNext = pNext;
          if (pPreviousPosRel == NULL)
            pOrginBox->BxPosRelations = pPosRel;
          else
            pPreviousPosRel->PosRNext = pPosRel;
          i = 0;
        }
      pRelation = &pPosRel->PosRTable[i];
      pRelation->ReRefEdge = originEdge;
      pRelation->ReBox = pTargetBox;
      pRelation->ReOp = op;
    }

  /* add a relation from target to origin */
  if (op != OpHorizInc && op != OpVertInc)
    {
      /* look for an empty entry */
      pPosRel = pTargetBox->BxPosRelations;
      pPreviousPosRel = NULL;
      loop = TRUE;
      while (loop && pPosRel)
        {
          i = 0;
          pPreviousPosRel = pPosRel;
          do
            {
              empty = (pPosRel->PosRTable[i].ReBox == NULL);
              i++;
            }
          while (i != MAX_RELAT_POS && !empty);
	  
          if (empty)
            {
              loop = FALSE;
              i--;
            }
          else
            /* next block */
            pPosRel = pPosRel->PosRNext;
        }

      if (loop)
        {
          /* add a new block */
          pNext = pPosRel;
          GetPosBlock (&pPosRel);
          pPosRel->PosRNext = pNext;
          if (pPreviousPosRel == NULL)
            pTargetBox->BxPosRelations = pPosRel;
          else
            pPreviousPosRel->PosRNext = pPosRel;
          i = 0;
        }
      pRelation = &pPosRel->PosRTable[i];
      pRelation->ReRefEdge = targetEdge;
      pRelation->ReBox = pOrginBox;
      pRelation->ReOp = op;
    }
}


/*----------------------------------------------------------------------
  InsertDimRelation registers vertical or horizontal links between two
  boxes (from pOrginBox to pTargetBox).
  The parameter op gives the relation:
  - OpSame, if the relation concerns the same dimension.
  - OpReverse, if the Width will change the Height or vice versa.
  - OpIgnore, keeps in mind the box that has a relation.
  The parameter inLine is TRUE when the pOrginBox box is displayed within
  a block of lines.
  ----------------------------------------------------------------------*/
static void InsertDimRelation (PtrBox pOrginBox, PtrBox pTargetBox,
                               OpDim op, ThotBool horizRef, ThotBool inLine)
{
  PtrDimRelations     pPreviousDimRel, pNext;
  PtrDimRelations     pDimRel;
  int                 i;
  ThotBool            loop;
  ThotBool            empty;

  if (inLine && IsParentBox (pTargetBox, pOrginBox))
    /* dont register the relation in this case */
    return;
  if (pOrginBox == NULL)
    return;

  if (op != OpIgnore)
    /* register in pTargetBox that pOrginBox has a relation with it */
    InsertDimRelation (pTargetBox, pOrginBox, OpIgnore, horizRef, inLine);

  /* add a relation from origin to target */
  i = 0;
  if (op == OpReverse)
    /* store the back releation in the other dimension list */
    horizRef = !horizRef;
  if (horizRef)
    pDimRel = pOrginBox->BxWidthRelations;
  else
    pDimRel = pOrginBox->BxHeightRelations;
 
  /* look for an empty entry */
  pPreviousDimRel = NULL;
  loop = TRUE;
  while (loop && pDimRel)
    {
      i = 0;
      pPreviousDimRel = pDimRel;
      do
        {
          empty = (pDimRel->DimRTable[i] == NULL);
          i++;
        }
      while (i != MAX_RELAT_DIM && !empty);

      if (empty)
        {
          loop = FALSE;
          i--;
        }
      else
        /* next block */
        pDimRel = pDimRel->DimRNext;
    }

  /* Need a new block? */
  if (loop)
    {
      /* add a new block */
      pNext = pDimRel;
      GetDimBlock (&pDimRel);
      pDimRel->DimRNext = pNext;
      if (pPreviousDimRel == NULL)
        {
          if (horizRef)
            pOrginBox->BxWidthRelations = pDimRel;
          else
            pOrginBox->BxHeightRelations = pDimRel;
        }
      else
        pPreviousDimRel->DimRNext = pDimRel;
      i = 0;
    }
  pDimRel->DimRTable[i] = pTargetBox;
  pDimRel->DimROp[i] = op;
}


/*----------------------------------------------------------------------
  NextAbToCheck recherche le prochain pave du pave pAb en         
  sautant le pave de reference pRefAb. Le parcours de     
  l'arborescence se fait de bas en haut et de gauche a`   
  droite.                                                 
  ----------------------------------------------------------------------*/
static PtrAbstractBox NextAbToCheck (PtrAbstractBox pAb, PtrAbstractBox pRefAb)
{
  PtrAbstractBox      pNextAb;

  /* Il y a un premier fils different du pave reference ? */
  if (pAb->AbFirstEnclosed != NULL && pAb->AbFirstEnclosed != pRefAb)
    pNextAb = pAb->AbFirstEnclosed;
  /* Il y a un frere different du pave reference ? */
  else if (pAb->AbNext != NULL && pAb->AbNext != pRefAb)
    pNextAb = pAb->AbNext;
  /* Sinon on remonte dans la hierarchie */
  else
    {
      pNextAb = NULL;
      while (pAb != NULL)
        {
          /* On saute le pave reference */
          if (pAb->AbFirstEnclosed == pRefAb && pRefAb->AbNext != NULL)
            pAb = pRefAb;
          else if (pAb->AbNext == pRefAb)
            pAb = pRefAb;

          /* On recherche un prochain frere d'un pere */
          if (pAb->AbNext != NULL)
            {
              pNextAb = pAb->AbNext;
              pAb = NULL;
            }
          else
            {
              /* On passe au pere tant que ce n'est pas la racine */
              pAb = pAb->AbEnclosing;
              if (pAb->AbFirstEnclosed == pRefAb)
                pAb = pAb->AbEnclosing;
              if (pAb != NULL)
                if (pAb->AbEnclosing == NULL)
                  pAb = NULL;
            }
        }
    }
  return pNextAb;
}


/*----------------------------------------------------------------------
  PropagateXOutOfStruct propage l'indicateur hors-structure.        
  ----------------------------------------------------------------------*/
static void PropagateXOutOfStruct (PtrAbstractBox pCurrentAb, int frame,
                                   ThotBool status, ThotBool enclosed)
{
  PtrAbstractBox      pAb;
  PtrBox              pBox;

  /* Recherche un pave frere qui depend ce celui-ci */
  if (pCurrentAb->AbEnclosing == NULL)
    return;
  else
    pAb = pCurrentAb->AbEnclosing->AbFirstEnclosed;

  while (pAb)
    {
      if (!HorizExtraAbFlow (pAb, frame))
        {
          pBox = pAb->AbBox;
          if (pAb == pCurrentAb || pBox == NULL || IsDead (pAb))
            ;			/* inutile de traiter ce pave */
          else if (pBox->BxXOutOfStruct == status)
            ;			/* inutile de traiter ce pave */
          else if (pAb->AbHorizPos.PosAbRef == pCurrentAb && !pAb->AbHorizPosChange)
            {
              /* Dependance de position */
              pBox->BxXOutOfStruct = status;
              /* Propagate aussi le non englobement */
              if (pAb->AbEnclosing == pCurrentAb->AbEnclosing)
                /* une boite soeur positionnee par rapport a une boite */
                /* elastique non englobee n'est pas englobee elle-meme */
                pAb->AbHorizEnclosing = enclosed;
              PropagateXOutOfStruct (pAb, frame, status, pAb->AbHorizEnclosing);
            }
          else if (pAb->AbWidth.DimIsPosition &&
                   pAb->AbWidth.DimPosition.PosAbRef == pCurrentAb &&
                   !pAb->AbWidthChange)
            /* Dependance de dimension */
            pBox->BxWOutOfStruct = status;
        }
      /* next abstract box */
      pAb = pAb->AbNext;
    }
}


/*----------------------------------------------------------------------
  PropagateYOutOfStruct propage l'indicateur hors-structure.         
  ----------------------------------------------------------------------*/
static void PropagateYOutOfStruct (PtrAbstractBox pCurrentAb, int frame,
                                   ThotBool status, ThotBool enclosed)
{
  PtrAbstractBox      pAb;
  PtrBox              pBox;

  /* Recherche un pave frere qui depend ce celui-ci */
  if (pCurrentAb->AbEnclosing == NULL)
    return;
  else
    pAb = pCurrentAb->AbEnclosing->AbFirstEnclosed;

  while (pAb)
    {
      if (!VertExtraAbFlow (pAb, frame))
        {
          pBox = pAb->AbBox;
          if (pAb == pCurrentAb || pBox == NULL || IsDead (pAb))
            ;			/* inutile de traiter ce pave */
          else if (pBox->BxYOutOfStruct == status)
            ;			/* inutile de traiter ce pave */
          else if (pAb->AbVertPos.PosAbRef == pCurrentAb && !pAb->AbVertPosChange)
            {
              pBox->BxYOutOfStruct = status;
              /* Propagate aussi le non englobement */
              if (pAb->AbEnclosing == pCurrentAb->AbEnclosing)
                /* une boite soeur positionnee par rapport a une boite */
                /* elastique non englobee n'est pas englobee elle-meme */
                pAb->AbVertEnclosing = enclosed;
              PropagateYOutOfStruct (pAb, frame, status, pAb->AbVertEnclosing);
            }
          else if (pAb->AbHeight.DimIsPosition
                   && pAb->AbHeight.DimPosition.PosAbRef == pCurrentAb && !pAb->AbHeightChange)
            pBox->BxHOutOfStruct = status;
        }
      /* next abstract box */
      pAb = pAb->AbNext;
    }
}


/*----------------------------------------------------------------------
  SetPositionConstraint memorizes the constrainted edge of the box and
  updates the position value according to the current edge used.
  The initial value of val depends on the refered box.
  ----------------------------------------------------------------------*/
void SetPositionConstraint (BoxEdge localEdge, PtrBox pBox, int *val)
{
  AbPosition         *pPosAb;

  switch (localEdge)
    {
    case Left:
      pBox->BxHorizEdge = Left;
      break;
    case Right:
      *val -= pBox->BxWidth;
      pBox->BxHorizEdge = Right;
      break;
    case VertMiddle:
      *val -= pBox->BxWidth / 2;
      pBox->BxHorizEdge = VertMiddle;
      break;
    case VertRef:
#ifdef IV
      if (pBox->BxAbstractBox->AbFloat == 'L')
        {
          // force left alignment
          *val = 0;
          pBox->BxHorizEdge = Left;
        }
      else if (pBox->BxAbstractBox->AbFloat == 'R')
        {
          *val -= pBox->BxWidth;
          pBox->BxHorizEdge = Right;
        }
      else
#endif
        {
          *val -= pBox->BxVertRef;
          pPosAb = &pBox->BxAbstractBox->AbVertRef;
          if (pPosAb->PosAbRef == pBox->BxAbstractBox)
            {
              if (pPosAb->PosRefEdge == VertMiddle)
                pBox->BxHorizEdge = VertMiddle;
              else if (pPosAb->PosRefEdge == Right)
                pBox->BxHorizEdge = Right;
              else
                pBox->BxHorizEdge = Left;
            }
          else
            pBox->BxHorizEdge = VertRef;
        }
      break;
    case Top:
      pBox->BxVertEdge = Top;
      break;
    case Bottom:
      *val -= pBox->BxHeight;
      pBox->BxVertEdge = Bottom;
      break;
    case HorizMiddle:
      *val -= pBox->BxHeight / 2;
      pBox->BxVertEdge = HorizMiddle;
      break;
    case HorizRef:
      *val -= pBox->BxHorizRef;
      pPosAb = &pBox->BxAbstractBox->AbHorizRef;
      if (pPosAb->PosAbRef == pBox->BxAbstractBox)
        {
          if (pPosAb->PosRefEdge == HorizMiddle)
            pBox->BxVertEdge = HorizMiddle;
          else if (pPosAb->PosRefEdge == Bottom)
            pBox->BxVertEdge = Bottom;
          else
            pBox->BxVertEdge = Top;
        }
      else
        pBox->BxVertEdge = HorizRef;
      break;
    default:
      break;
    }
}
/*----------------------------------------------------------------------
  GetPixelValue returns the pixel value according to the unit
  ----------------------------------------------------------------------*/
int GetPixelValue (int val, TypeUnit unit, int size, PtrAbstractBox pAb,
                   int zoom)
{
  if (unit == UnPercent)
    return PixelValue (val, unit, (PtrAbstractBox) size, 0);
  else
    return PixelValue (val, unit, pAb, zoom);
}

/*----------------------------------------------------------------------
  ComputeRadius updates the horizontal or vartical radius of a rectangle
  with rounded corners.
  ----------------------------------------------------------------------*/
void ComputeRadius (PtrAbstractBox pAb, int frame, ThotBool horizRef)
{
  PtrBox              pBox;
  int zoom = ViewFrameTable[frame - 1].FrMagnification;

  pBox = pAb->AbBox;
  if (horizRef)
    {
      if(pAb->AbRx == -1)
	pBox->BxRx = -1;
      else
	pBox->BxRx = GetPixelValue (pAb->AbRx, pAb->AbRxUnit,
				    pBox->BxW, pAb, zoom);
    }
  else
    {
      if(pAb->AbRy == -1)
	pBox->BxRy = -1;
      else
	pBox->BxRy = GetPixelValue (pAb->AbRy, pAb->AbRyUnit,
				    pBox->BxH, pAb, zoom);
    }
}

/*----------------------------------------------------------------------
  CleanAutoMargins cleans up all auto margins.
  ----------------------------------------------------------------------*/
void CleanAutoMargins (PtrAbstractBox pAb)
{
  PtrBox              box;

  if (pAb && pAb->AbBox)
    {
      box = pAb->AbBox;
      if (pAb->AbLeftMarginUnit == UnAuto)
        {
          box->BxWidth -= box->BxLMargin;
          box->BxLMargin = 0;
        }
      if (pAb->AbRightMarginUnit == UnAuto)
        {
          box->BxWidth -= box->BxRMargin;
          box->BxRMargin = 0;
        }
    }
}

/*----------------------------------------------------------------------
  GetEnclosingViewport returns the enclosing abstractbox that defines
  the viewport.
  ----------------------------------------------------------------------*/
PtrAbstractBox GetEnclosingViewport (PtrAbstractBox pAb)
{
  PtrAbstractBox prev;

  if (pAb)
    {
      pAb = pAb->AbEnclosing;
      while (pAb &&
             !TypeHasException (ExcNewRoot, pAb->AbElement->ElTypeNumber,
                                pAb->AbElement->ElStructSchema) &&
             (pAb->AbLeafType != LtCompound ||
              pAb->AbPositioning == NULL ||
              pAb->AbPositioning->PnAlgorithm == PnInherit ||
              pAb->AbPositioning->PnAlgorithm == PnStatic))
        {
          if (pAb->AbPrevious &&
              TypeHasException (ExcSetWindowBackground,
                                pAb->AbElement->ElTypeNumber,
                                pAb->AbElement->ElStructSchema))
            {
              // it could be the xtiger head before the HTML body
              prev = pAb->AbPrevious;
              while (prev && prev->AbElement == pAb->AbElement)
                prev = prev->AbPrevious;
              if (prev)
                return pAb;
            }
          pAb = pAb->AbEnclosing;
        }
    }
  return pAb;
}

/*----------------------------------------------------------------------
  ComputeMBP applies margin, padding, and border rules.
  Relation between values:
  <-LMargin-><-LBorder-><-LPadding-><-W-><-RPadding-><-RBorder-><-LRargin->
  <---------------------------------Width--------------------------------->
  ----------------------------------------------------------------------*/
void ComputeMBP (PtrAbstractBox pAb, int frame, ThotBool horizRef,
                 ThotBool evalAuto)
{
  PtrAbstractBox      pRefAb;
  PtrBox              pBox;
  PtrBox              pParent;
  PtrElement          pEl = pAb->AbElement;
  int                 dim, x1, x2, y1, y2;

  x1 = x2 = y1 = y2 = -1;
  if (pAb->AbEnclosing && pAb->AbEnclosing->AbBox)
    pParent = pAb->AbEnclosing->AbBox;
  else
    pParent = NULL;

  pBox = pAb->AbBox;
  if (horizRef)
    {
      /* reference for percent rules */
      if (pAb->AbLeftMarginUnit == UnPercent ||
          pAb->AbRightMarginUnit == UnPercent ||
          pAb->AbLeftPaddingUnit == UnPercent ||
          pAb->AbRightPaddingUnit == UnPercent ||
          pAb->AbLeftBorderUnit == UnPercent ||
          pAb->AbRightBorderUnit == UnPercent ||
          pAb->AbWidth.DimUnit == UnPercent ||
          /* if the width is fixed */
          (pAb->AbWidth.DimAbRef == NULL &&
           pAb->AbWidth.DimValue != -1))
        {
          /* percent and auto refer the enclosing box */
          if (pAb->AbLeafType == LtCompound &&
              pAb->AbPositioning &&
              (pAb->AbPositioning->PnAlgorithm == PnAbsolute ||
               pAb->AbPositioning->PnAlgorithm == PnFixed))
            {
              pRefAb = GetEnclosingViewport (pAb);
              if (pRefAb == NULL)
                pRefAb = ViewFrameTable[frame -1].FrAbstractBox;
              if (pRefAb && pRefAb->AbBox)
                {
                  pParent = NULL;
                  dim = pRefAb->AbBox->BxW;
                }
              else
                dim = pBox->BxW;
            }
          else if (pParent)
            dim = pParent->BxW;
          else
            dim = pBox->BxW;
        }
      else
        dim = pBox->BxW;

      /* left margin */
      if (pBox->BxType == BoCell || pBox->BxType == BoRow ||
          (pAb->AbLeftMargin >= 0 && ExtraAbFlow (pAb, frame)))
        pBox->BxLMargin = 0;
      else if (pAb->AbLeftMarginUnit != UnAuto)
        pBox->BxLMargin = GetPixelValue (pAb->AbLeftMargin, pAb->AbLeftMarginUnit, dim,
                                         pAb, ViewFrameTable[frame - 1].FrMagnification);
      /* right margin */
      if (pBox->BxType == BoCell || pBox->BxType == BoRow ||
          (pAb->AbRightMargin >= 0 && ExtraAbFlow (pAb, frame)))
        pBox->BxRMargin = 0;
      else if (pAb->AbRightMarginUnit != UnAuto)
        pBox->BxRMargin = GetPixelValue (pAb->AbRightMargin, pAb->AbRightMarginUnit, dim,
                                         pAb, ViewFrameTable[frame - 1].FrMagnification);

      /* left padding */
      pBox->BxLPadding = GetPixelValue (pAb->AbLeftPadding, pAb->AbLeftPaddingUnit, dim,
                                        pAb, ViewFrameTable[frame - 1].FrMagnification);
      /* right padding */
      pBox->BxRPadding = GetPixelValue (pAb->AbRightPadding, pAb->AbRightPaddingUnit, dim,
                                        pAb, ViewFrameTable[frame - 1].FrMagnification);

      /* left border */
      pBox->BxLBorder = GetPixelValue (pAb->AbLeftBorder, pAb->AbLeftBorderUnit, dim,
                                       pAb, ViewFrameTable[frame - 1].FrMagnification);
      /* right border */
      pBox->BxRBorder = GetPixelValue (pAb->AbRightBorder, pAb->AbRightBorderUnit, dim,
                                       pAb, ViewFrameTable[frame - 1].FrMagnification);

      /* Manage auto margins */
      if (pAb->AbLeftMarginUnit == UnAuto || pAb->AbRightMarginUnit == UnAuto)
        {
          if (pBox->BxType != BoCell &&
              pBox->BxType != BoRow &&
              pParent && evalAuto &&
              pParent->BxType != BoBlock &&
              pParent->BxType != BoFloatBlock &&
              pParent->BxType != BoCellBlock &&
              pParent->BxType != BoStructGhost &&
              pParent->BxType != BoGhost &&
              pParent->BxType != BoFloatGhost &&
              pParent->BxW >= dim)
            {
              if (!pAb->AbWidth.DimIsPosition &&
                  (pAb->AbWidth.DimAbRef == pAb->AbEnclosing ||
                   (pAb->AbWidth.DimUnit == UnPercent && pAb->AbWidth.DimValue == 100)))
                /* the box size depends on its parent */
                dim = 0;
              else
                /* box size depends on its contents */
                dim = dim - pBox->BxW - pBox->BxLPadding - pBox->BxRPadding
                  - pBox->BxLBorder - pBox->BxRBorder;
              if (dim < 0)
                dim = 0;
              if (pAb->AbLeftMarginUnit == UnAuto && pAb->AbRightMarginUnit == UnAuto)
                {
                  pBox->BxLMargin = dim / 2;
                  pBox->BxRMargin = pBox->BxLMargin;
                }
              else if (pAb->AbLeftMarginUnit == UnAuto)
                pBox->BxLMargin = dim - pBox->BxRMargin;
              else if (pAb->AbRightMarginUnit == UnAuto)
                pBox->BxRMargin = dim - pBox->BxLMargin;
            }
          else
            {
              if (pAb->AbLeftMarginUnit == UnAuto)
                pBox->BxLMargin = 0;
              if (pAb->AbRightMarginUnit == UnAuto)
                pBox->BxRMargin = 0;
            }
        }
      /* List-item presentation boxes take into account the left margin */
      if (pAb->AbPrevious && pAb->AbPrevious->AbBox &&
          pAb->AbPrevious->AbPresentationBox &&
          pAb->AbPrevious->AbTypeNum == 0)
        {
          if (pAb->AbDirection == 'L')
            pAb->AbPrevious->AbBox->BxXOrg = pBox->BxXOrg + pBox->BxLMargin
              - pAb->AbPrevious->AbBox->BxWidth;
          else
            pAb->AbPrevious->AbBox->BxXOrg = pBox->BxXOrg + pBox->BxWidth
              - pBox->BxRMargin;
        }
    }
  else
    {
      /* reference for percent rules */
      if (pAb->AbTopMarginUnit == UnPercent ||
          pAb->AbBottomMarginUnit == UnPercent ||
          pAb->AbTopPaddingUnit == UnPercent ||
          pAb->AbBottomPaddingUnit == UnPercent ||
          pAb->AbTopBorderUnit == UnPercent ||
          pAb->AbBottomBorderUnit == UnPercent)
        {
          //#ifdef POSITIONING
          if (pAb->AbLeafType == LtCompound &&
              pAb->AbPositioning &&
              (pAb->AbPositioning->PnAlgorithm == PnAbsolute ||
               pAb->AbPositioning->PnAlgorithm == PnFixed))
            {
              pRefAb = GetEnclosingViewport (pAb);
              if (pRefAb == NULL)
                pRefAb = ViewFrameTable[frame -1].FrAbstractBox;
              if (pRefAb && pRefAb->AbBox)
                {
                  pParent = NULL;
                  dim = pRefAb->AbBox->BxH;
                }
              else
                dim = pBox->BxH;
            }
          //#endif /* POSITIONING */
          else if (pParent)
            dim = pParent->BxH;
          else
            dim = pBox->BxH;
        }
      else
        dim = pBox->BxH;

      if (pAb->AbLeafType == LtCompound && pAb->AbTruncatedHead)
        {
          pBox->BxTMargin = 0;
          pBox->BxTPadding = 0;
          pBox->BxTBorder = 0;
        }
      else
        {
          /* top margin */
          if (pBox->BxType == BoCell || pBox->BxType == BoCellBlock ||
              pBox->BxType == BoRow ||
              (pAb->AbTopMargin >= 0 && pAb->AbFloat != 'N'))
            pBox->BxTMargin = 0;
          else if (pAb->AbTopMarginUnit != UnAuto)
            pBox->BxTMargin = GetPixelValue (pAb->AbTopMargin, pAb->AbTopMarginUnit, dim,
                                             pAb, ViewFrameTable[frame - 1].FrMagnification);
          if (pBox->BxType == BoTable && pEl && pEl->ElStructSchema && pEl->ElStructSchema->SsName &&
              !strcmp (pEl->ElStructSchema->SsName, "HTML"))
            {
              pBox->BxTPadding = 0;
              pBox->BxTBorder = 0;
            }
          else
            {
              /* top padding */
              pBox->BxTPadding = GetPixelValue (pAb->AbTopPadding, pAb->AbTopPaddingUnit, dim,
                                                pAb, ViewFrameTable[frame - 1].FrMagnification);
              /* top border */
              pBox->BxTBorder = GetPixelValue (pAb->AbTopBorder, pAb->AbTopBorderUnit, dim,
                                               pAb, ViewFrameTable[frame - 1].FrMagnification);
            }
        }

      if (pAb->AbLeafType == LtCompound && pAb->AbTruncatedTail)
        {
          pBox->BxBMargin = 0;
          pBox->BxBPadding = 0;
          pBox->BxBBorder = 0;
        }
      else
        {
          /* bottom margin */
          if (pBox->BxType == BoCell || pBox->BxType == BoCellBlock ||
              pBox->BxType == BoRow ||
              (pAb->AbBottomMargin >= 0 && pAb->AbFloat != 'N'))
            pBox->BxBMargin = 0;
          else if (pAb->AbBottomMarginUnit != UnAuto)
            pBox->BxBMargin = GetPixelValue (pAb->AbBottomMargin, pAb->AbBottomMarginUnit, dim,
                                             pAb, ViewFrameTable[frame - 1].FrMagnification);
          if (pBox->BxType == BoTable && pEl && pEl->ElStructSchema && pEl->ElStructSchema->SsName &&
              !strcmp (pEl->ElStructSchema->SsName, "HTML"))
            {
              pBox->BxBPadding = 0;
              pBox->BxBBorder = 0;
            }
          else
            {
              /* bottom padding */
              pBox->BxBPadding = GetPixelValue (pAb->AbBottomPadding, pAb->AbBottomPaddingUnit, dim,
                                                pAb, ViewFrameTable[frame - 1].FrMagnification);
              /* bottom border */
              pBox->BxBBorder = GetPixelValue (pAb->AbBottomBorder, pAb->AbBottomBorderUnit, dim,
                                               pAb, ViewFrameTable[frame - 1].FrMagnification);
            }
        }
    }
}

/*----------------------------------------------------------------------
  GetExtraMargins returns the sum of extra margins, borders, and paddings
  generated by enclosing ghosts.
  The parameter pBox points to the non ghost box.
  When the parameter blockMargin is TRUE, ghost blocks are managed
  Returns
  t = the added pixels at the top
  b = the added pixels at the bottom
  l = the added pixels at the left
  r = the added pixels at the right
  ----------------------------------------------------------------------*/
void GetExtraMargins (PtrBox pBox, int frame, ThotBool blockMargin,
                      int *t, int *b, int *l, int *r)
{
  PtrAbstractBox      pAb, pNext, pPrev;
  PtrBox              box;
  ThotBool            first = TRUE, last = TRUE, isExtra;

  *t = *b = *l = *r = 0;
  if (pBox == NULL)
    return;
  pAb = pBox->BxAbstractBox;
  isExtra = ExtraAbFlow (pAb, frame);
  if (pAb && !pAb->AbDead && !isExtra && pAb->AbFloat == 'N')
    {
      /* check if there are enclosing ghost boxes */ 
      if (pAb->AbEnclosing && pAb->AbEnclosing->AbBox &&
          (pAb->AbEnclosing->AbBox->BxType == BoGhost ||
           pAb->AbEnclosing->AbBox->BxType == BoStructGhost))
        {
          if (pBox->BxType != BoGhost || pAb->AbDisplay != 'B')
            {
              /* check if it's the first and/or the last child */
              pPrev = pAb->AbPrevious;
              while (pPrev && pPrev->AbPresentationBox)
                pPrev = pPrev->AbPrevious;
              first = (pPrev == NULL);
              pNext = pAb->AbNext;
              while (pNext && pNext->AbPresentationBox)
                pNext = pNext->AbNext;
              last = (pNext == NULL);
            }
          if (pBox->BxType == BoPiece || pBox->BxType == BoScript)
            {
              /* check if it's the first and/or the last piece of text */
              if (first)
                first = (pBox == pAb->AbBox->BxNexChild);
              if (last)
                last = (pBox->BxNexChild == NULL);
            }

          /* Get extra-margins generated by enclosing ghosts */
          pAb = pAb->AbEnclosing;
          while (pAb && pAb->AbBox &&
                 (pAb->AbBox->BxType == BoGhost ||
                  pAb->AbBox->BxType == BoStructGhost))
            {
              box = pAb->AbBox;
              /* add values if necessary */
              if (box->BxType != BoStructGhost)
                {
                  if (first)
                    *l += box->BxLMargin + box->BxLBorder + box->BxLPadding;
                  if (last)
                    *r += box->BxRMargin + box->BxRBorder + box->BxRPadding;
                  *t += box->BxTMargin + box->BxTBorder + box->BxTPadding;
                  *b += box->BxBMargin + box->BxBBorder + box->BxBPadding;
                }
              else
                {
                  // It's a ghost block
                  if (blockMargin && pAb->AbEnclosing && pAb->AbEnclosing->AbBox &&
                      pAb->AbEnclosing->AbBox->BxType != BoStructGhost)
                    {
                      if (*l > 0 && box->BxLMargin > 0)
                        {
                          if (*l < box->BxLMargin)
                            *l =  box->BxLMargin;
                        }
                      else
                        *l += box->BxLMargin;
                      if (*r > 0 && box->BxRMargin > 0)
                        {
                          if (*r < box->BxRMargin)
                            *r =  box->BxRMargin;
                        }
                      else
                        *r += box->BxRMargin;
                     }
                  if (first)
                    {
                      *t += box->BxTBorder + box->BxTPadding;
                      if (blockMargin)
                        {
                          if (*t > 0 && box->BxTMargin > 0)
                            {
                              if (*t < box->BxTMargin)
                                *t =  box->BxTMargin;
                            }
                          else
                            *t += box->BxTMargin;
                        }
                    }
                  if (last)
                    {
                      *b += box->BxBBorder + box->BxBPadding;
                      if (blockMargin)
                        {
                          if (*b > 0 && box->BxBMargin > 0)
                            {
                              if (*b < box->BxBMargin)
                                *b =  box->BxBMargin;
                            }
                          else
                            *b += box->BxBMargin;
                        }
                    }
                }
              /* search previous and next abstract boxes */
              if (first)
                {
                  pPrev = pAb->AbPrevious;
                  while (pPrev && pPrev->AbPresentationBox)
                    pPrev = pPrev->AbPrevious;
                  first = (pPrev == NULL);
                }
              if (last)
                {
                  pNext = pAb->AbNext;
                  while (pNext && pNext->AbPresentationBox)
                    pNext = pNext->AbNext;
                  last = (pNext == NULL);
                }
              pAb = pAb->AbEnclosing;
            }
        }
    }
}


/*----------------------------------------------------------------------
  GetLeftRightMargins updates if needed left and right margins and
  returns these values
  ----------------------------------------------------------------------*/
void GetLeftRightMargins (PtrBox box, PtrBox pBlock, int frame, int *l, int *r)
{
  PtrAbstractBox      pAb, parent;

  *l = *r = 0;
  if (box && box->BxAbstractBox)
    {
      pAb = box->BxAbstractBox;
      parent = pAb->AbEnclosing;
      if (pAb->AbLeftMarginUnit == UnPercent && pBlock && !pBlock->BxContentWidth)
        {
          if (parent->AbBox && parent->AbBox->BxType == BoStructGhost)
            box->BxLMargin = parent->AbBox->BxW * pAb->AbLeftMargin / 100;
          else
            box->BxLMargin = pBlock->BxW * pAb->AbLeftMargin / 100;
        }
      if (pAb->AbRightMarginUnit == UnPercent && pBlock && !pBlock->BxContentWidth)
        {
          if (parent->AbBox && parent->AbBox->BxType == BoStructGhost)
            box->BxRMargin = parent->AbBox->BxW * pAb->AbRightMargin / 100;
          else
            box->BxRMargin = pBlock->BxW * pAb->AbRightMargin / 100;
        }
        *l = box->BxLMargin;
        *r = box->BxRMargin;
    }
}

/*----------------------------------------------------------------------
  GetLeftRightPaddings updates if needed left and right paddings and
  returns these values
  ----------------------------------------------------------------------*/
void GetLeftRightPaddings (PtrBox box, PtrBox pBlock, int *l, int *r)
{
  PtrAbstractBox      pAb, parent;

  *l = *r = 0;
  if (box && box->BxAbstractBox)
    {
      pAb = box->BxAbstractBox;
      do
        {
          box = pAb->AbBox;
          parent = pAb->AbEnclosing;
          if (parent->AbBox && parent->AbBox->BxType == BoStructGhost)
            {
              if (pAb->AbLeftPaddingUnit == UnPercent &&
                  !parent->AbBox->BxContentWidth)
                box->BxLPadding = parent->AbBox->BxW * pAb->AbLeftPadding / 100;
              if (pAb->AbRightPaddingUnit == UnPercent &&
                  !parent->AbBox->BxContentWidth)
                box->BxRPadding = parent->AbBox->BxW * pAb->AbRightPadding / 100;
              // Include extra margins
              if (box->BxType == BoStructGhost)
                {
                  *l += box->BxLMargin;
                  *r += box->BxRMargin;
                }
              pAb = parent;
           }
          else
            {
              if (pAb->AbLeftPaddingUnit == UnPercent && pBlock && !pBlock->BxContentWidth)
                box->BxLPadding = pBlock->BxW * pAb->AbLeftPadding / 100;
              if (pAb->AbRightPaddingUnit == UnPercent && pBlock && !pBlock->BxContentWidth)
                box->BxRPadding = pBlock->BxW * pAb->AbRightPadding / 100;
              pAb = NULL;
           }

          *l += box->BxLPadding;
          *r += box->BxRPadding;
        }
      while (pAb && pAb->AbBox && pAb->AbBox->BxType == BoStructGhost);
    }
}

/*----------------------------------------------------------------------
  ComputePosRelation applies fixed and absolute positioning rules.
  Return TRUE if the box is the box position is out of the standard flow.
  ----------------------------------------------------------------------*/
ThotBool ComputePositioning (PtrBox pBox, int frame)
{
  PtrAbstractBox      pAb, pRefAb;
  PtrBox              pRefBox = NULL;
  AbPosition         *pPosAb;
  Positioning        *pos;
  int                 x, y, w, h;
  int                 l, t, r, b;
  int                 lr, tr, rr, br;
  ThotBool            appl,appt, appr, appb;

  if (pBox && pBox->BxAbstractBox &&
      pBox->BxAbstractBox->AbLeafType == LtCompound &&
      pBox->BxAbstractBox->AbPositioning &&
      pBox->BxAbstractBox->AbVisibility >= ViewFrameTable[frame - 1].FrVisibility)
    {
      pAb = pBox->BxAbstractBox;
      pos = pAb->AbPositioning;
      if (pos->PnAlgorithm == PnAbsolute || pos->PnAlgorithm == PnFixed)
        {
          if (pBox->BxType == BoRow || pBox->BxType == BoColumn ||
              pBox->BxType == BoCell)
            {
              //pos->PnAlgorithm = PnStatic;
              return FALSE;
            }

          // get the enclosing viewport
          pRefAb = GetEnclosingViewport (pAb);
          GetSizesFrame (frame, &w, &h);
          if (pos->PnAlgorithm == PnFixed)
            {
              // refer the frame
              x = ViewFrameTable[frame - 1].FrXOrg;
              y = ViewFrameTable[frame - 1].FrYOrg;
            }
          else
            x = y = 0;
          
          lr = tr = rr = br = 0;
          if (pRefAb == NULL)
            {
              pRefAb = ViewFrameTable[frame -1].FrAbstractBox;
              pRefBox = pRefAb->AbBox;
            }
          else if (pRefAb->AbBox)
            {
              // refer another box
              pRefBox = pRefAb->AbBox;
              while (pRefBox &&
                     (pRefBox->BxType == BoGhost ||
                      pRefBox->BxType == BoStructGhost ||
                      pRefBox->BxType == BoFloatGhost))
                {
                  /* ignore ghosts */
                  pRefAb = pRefAb->AbFirstEnclosed;
                  pRefBox = pRefAb->AbBox;
                }
              if (pRefBox &&
                  (pRefBox->BxType == BoSplit ||
                   pRefBox->BxType == BoMulScript))
                pRefBox = pRefBox->BxNexChild;
              if (pRefBox)
                {
                  lr = pRefBox->BxLMargin + pRefBox->BxLBorder;
                  tr =  pRefBox->BxTMargin + pRefBox->BxTBorder;
                  rr = pRefBox->BxRMargin + pRefBox->BxRBorder;
                  br =  pRefBox->BxBMargin + pRefBox->BxBBorder;
                  w = pRefBox->BxLPadding + pRefBox->BxRPadding + pRefBox->BxW;
                  h = pRefBox->BxTPadding + pRefBox->BxBPadding + pRefBox->BxH;
                  x = pRefBox->BxXOrg;
                  y = pRefBox->BxYOrg;
                }
            }
          /* by default don't apply */
          appl = appt = appr = appb = FALSE;
          l = t = r = b = 0;
          if (pos->PnLeftUnit == UnAuto)
            {
              if (pos->PnRightUnit == UnAuto || pos->PnRightUnit == UnUndefined)
                {
                  l = 0;
                  appl = TRUE;
                }
            }
          else if (pos->PnLeftUnit != UnUndefined)
            {
              l = GetPixelValue (pos->PnLeftDistance, pos->PnLeftUnit, w, pAb,
                                 ViewFrameTable[frame - 1].FrMagnification);
              appl = TRUE;
            }
          if (pos->PnRightUnit != UnAuto && pos->PnRightUnit != UnUndefined)
            {
              r = GetPixelValue (pos->PnRightDistance, pos->PnRightUnit, w, pAb,
                                 ViewFrameTable[frame - 1].FrMagnification);
              appr = TRUE;
            }
          if (pos->PnTopUnit == UnAuto)
            {
              if (pos->PnBottomUnit == UnAuto || pos->PnBottomUnit == UnUndefined)
                {
                  t = 0;
                  appt = TRUE;
                }
            }
          else if (pos->PnTopUnit != UnUndefined)
            {
              t = GetPixelValue (pos->PnTopDistance, pos->PnTopUnit, h, pAb,
                              ViewFrameTable[frame - 1].FrMagnification);
              appt = TRUE;
            }
          if (pos->PnBottomUnit != UnAuto && pos->PnBottomUnit != UnUndefined)
            {
              b = GetPixelValue (pos->PnBottomDistance, pos->PnBottomUnit, h, pAb,
                                 ViewFrameTable[frame - 1].FrMagnification);
              appb = TRUE;
            }
          
          /* Move also enclosed boxes */
          if (appl || appr)
            {
              pAb->AbHorizPosChange = FALSE;
              pBox->BxXToCompute = TRUE;
              pAb->AbHorizEnclosing = FALSE;
              pBox->BxXOutOfStruct = TRUE;
            }
          if (appl)
            {
              /* left positioning */
              if (appr &&
                  (pAb->AbWidth.DimUnit == UnAuto ||
                   pAb->AbWidth.DimIsPosition ||
                   (pAb->AbWidth.DimAbRef == NULL && pAb->AbWidth.DimValue == -1)))
                {
                  /* stretchable width */
                  pAb->AbWidthChange = FALSE;
                  pPosAb = &pAb->AbWidth.DimPosition;
                  pAb->AbWidth.DimIsPosition = TRUE;
                  pPosAb->PosAbRef = pRefAb;
                  pPosAb->PosUnit = pos->PnRightUnit;
                  pPosAb->PosDistance =-pos->PnRightDistance;
                  pPosAb->PosRefEdge = Right;
                  pPosAb->PosEdge = Right;
                  pBox->BxWOutOfStruct = TRUE;
                  if (pBox->BxContentWidth)
                    pBox->BxContentWidth = FALSE;
                  InsertPosRelation (pBox, pRefBox, OpWidth, pPosAb->PosEdge, pPosAb->PosRefEdge);
                  /* The box is now set stretchable */
                  pBox->BxHorizFlex = pRefBox;
                  pRefBox->BxMoved = NULL;
                  MoveBoxEdge (pBox, pRefBox, OpWidth, x + lr + w + rr - r, frame, TRUE);
                }
              XMoveAllEnclosed (pBox, x + lr + l - pBox->BxXOrg, frame);
              // register as a pos rule
              pPosAb = &pAb->AbHorizPos;
              pPosAb->PosAbRef = pRefAb;
              pPosAb->PosUnit = pos->PnLeftUnit;
              pPosAb->PosDistance = pos->PnLeftDistance;
              pPosAb->PosRefEdge = Left;
              pPosAb->PosEdge = Left;
              if (pRefBox)
                InsertPosRelation (pBox, pRefBox, OpHorizDep, Left, Left);
              if (pAb->AbWidth.DimUnit == UnAuto && l && !appr)
                ResizeWidth (pBox, pRefBox, NULL, -l, 0, 0, 0, frame, FALSE);
            }
          else if (appr)
            {
              /* right positioning */
              pBox->BxHorizEdge = Right;
              XMoveAllEnclosed (pBox, x + lr + w - r - pBox->BxWidth - pBox->BxXOrg, frame);
              // register as a pos rule
              pPosAb = &pAb->AbHorizPos;
              pPosAb->PosAbRef = pRefAb;
              pPosAb->PosUnit = pos->PnRightUnit;
              pPosAb->PosDistance = -pos->PnRightDistance;
              pPosAb->PosRefEdge = Right;
              pPosAb->PosEdge = Right;
              if (pRefBox)
                InsertPosRelation (pBox, pRefBox, OpHorizDep, Right, Right);
              pBox->BxHorizEdge = Right;
            }
          else
            ComputePosRelation (&pAb->AbHorizPos, pBox, frame, TRUE);
          
          if (appt || appb)
            {
              pAb->AbVertPosChange = FALSE;
              pBox->BxYToCompute = TRUE;
              pAb->AbVertEnclosing = FALSE;
              pBox->BxYOutOfStruct = TRUE;
            }
          if (appt)
            {
              /* top positioning */
              if (appb)
                {
                  /* stretchable height */
                  pAb->AbHeightChange = FALSE;
                  pBox->BxHOutOfStruct = TRUE;
                  if (pBox->BxContentHeight)
                    pBox->BxContentHeight = FALSE;
                  ResizeHeight (pBox, pBox, NULL, tr + h + br - b - pBox->BxH, 0, 0, frame);
                  InsertDimRelation (pRefBox, pBox, OpSame, FALSE, FALSE);
                }
              YMoveAllEnclosed (pBox, y + tr + t - pBox->BxYOrg, frame);
              // register as a pos rule
              pPosAb = &pAb->AbVertPos;
              pPosAb->PosAbRef = pRefAb;
              pPosAb->PosUnit = pos->PnTopUnit;
              pPosAb->PosDistance = pos->PnTopDistance;
              pPosAb->PosRefEdge = Top;
              pPosAb->PosEdge = Top;
              if (pRefBox)
                InsertPosRelation (pBox, pRefBox, OpVertDep, Top, Top);
            }
          else if (appb)
            {
              /* bottom positioning */
              pBox->BxVertEdge = Bottom;
              YMoveAllEnclosed (pBox, y + tr + h - b - pBox->BxHeight - pBox->BxYOrg, frame);
              // register as a pos rule
              pPosAb = &pAb->AbVertPos;
              pPosAb->PosAbRef = pRefAb;
              pPosAb->PosUnit = pos->PnBottomUnit;
              pPosAb->PosDistance = -pos->PnBottomDistance;
              pPosAb->PosRefEdge = Bottom;
              pPosAb->PosEdge = Bottom;
              if (pRefBox)
                InsertPosRelation (pBox, pRefBox, OpVertDep, Bottom, Bottom);
              pBox->BxVertEdge = Bottom;
            }
          else
            ComputePosRelation (&pAb->AbVertPos, pBox, frame, FALSE);
          return TRUE;
        }
      else
        return FALSE;
    }
  else
    return FALSE;
}


/*----------------------------------------------------------------------
  ComputePosRelation applies the vertical/horizontal positioning rule
  according to the parameter horizRef for the box pBox. 
  The box origin BxXOrg or BxYOrg is updated and dependencies between 
  boxes are registered.  
  ^
  BxXOrg
  <-LMargin-><-LBorder-><-LPadding-><-W-><-RPadding-><-RBorder-><-LRargin->
  <---------------------------------Width--------------------------------->
  ----------------------------------------------------------------------*/
void ComputePosRelation (AbPosition *rule, PtrBox pBox, int frame,
                         ThotBool horizRef)
{
  PtrAbstractBox      pRefAb;
  PtrAbstractBox      pAb, pChildAb, pParentAb;
  PtrBox              pRefBox, box;
  BoxEdge             refEdge, localEdge;
  OpRelation          op;
  int                 x, y, dist, dim, d;
  int                 t, l, b, r;
  ThotBool            sibling = FALSE;
  
  /* On calcule la position de reference */
  op = (OpRelation) 0;
  refEdge = (BoxEdge) 0;
  GetSizesFrame (frame, &x, &y);
  pAb = pBox->BxAbstractBox;
  pRefBox = NULL;

  if (pAb->AbFloat != 'N' &&
      (pAb->AbLeafType == LtPicture ||
       (pAb->AbLeafType == LtCompound &&
        !pAb->AbWidth.DimIsPosition &&
        pAb->AbWidth.DimAbRef == NULL)))
    {
      /* the box position is computed by the line formatter */
      pAb->AbVertPosChange = FALSE;
      pAb->AbHorizPosChange = FALSE;
      return;
    }

  pParentAb = pAb->AbEnclosing;
  if (pBox->BxType == BoStructGhost)
    {
      // change the position of structure ghost
      l = pBox->BxLPadding;
      t = pBox->BxTPadding;
      pChildAb = pAb->AbFirstEnclosed;
      while (pChildAb && pChildAb->AbPresentationBox)
        pChildAb = pChildAb->AbNext;
      while (pChildAb && pChildAb->AbBox &&
             pChildAb->AbBox->BxType == BoStructGhost)
        {
          l += pChildAb->AbBox->BxLPadding;
          t += pChildAb->AbBox->BxTPadding;
          pChildAb = pChildAb->AbFirstEnclosed;
          while (pChildAb && pChildAb->AbPresentationBox)
            pChildAb = pChildAb->AbNext;
        }
      if (pChildAb)
        {
          if (horizRef && pChildAb->AbHorizPos.PosAbRef &&
              pChildAb->AbHorizPos.PosAbRef != pChildAb->AbEnclosing &&
              pChildAb->AbVertPos.PosAbRef != pChildAb->AbPrevious)
            {
              // the position depends on another box
              rule->PosAbRef = pChildAb->AbHorizPos.PosAbRef;
              rule->PosRefEdge = pChildAb->AbHorizPos.PosRefEdge;
              rule->PosEdge = pChildAb->AbHorizPos.PosEdge;
              rule->PosDistDelta = -l;
              rule->PosDeltaUnit = UnPixel;
              pBox->BxXOutOfStruct = pChildAb->AbBox->BxXOutOfStruct;
            }
          else if (!horizRef && pChildAb && pChildAb->AbVertPos.PosAbRef &&
                   pChildAb->AbVertPos.PosAbRef != pChildAb->AbEnclosing &&
                   pChildAb->AbVertPos.PosAbRef != pChildAb->AbPrevious)
            {
              // the position depends on another box
              rule->PosAbRef = pChildAb->AbVertPos.PosAbRef;
              rule->PosRefEdge = pChildAb->AbVertPos.PosRefEdge;
              rule->PosEdge = pChildAb->AbVertPos.PosEdge;
              rule->PosDistDelta = -t;
              rule->PosDeltaUnit = UnPixel;
              //pBox->BxYOutOfStruct = pChildAb->AbBox->BxYOutOfStruct;
            }
        }
    }

  pRefAb = rule->PosAbRef;
  if (pRefAb && IsDead (pRefAb))
    {
      fprintf (stderr, "Position refers a dead box");
      pRefAb = NULL;
    }
  else if (pRefAb && !IsParentBox (pRefAb->AbBox, pBox))
    /* ignore previous absolute positioning */
    while (pRefAb &&
           (pRefAb->AbDead ||
            (pRefAb->AbLeafType == LtCompound &&
             pRefAb->AbPositioning &&
             (pRefAb->AbPositioning->PnAlgorithm == PnAbsolute ||
              pRefAb->AbPositioning->PnAlgorithm == PnFixed))))
      {
        if (pRefAb->AbPrevious)
          /* refer the previous box instead */
          pRefAb = pRefAb->AbPrevious;
        else
          {
            /* refer the parent box */
            pRefAb = pRefAb->AbEnclosing;
            if (horizRef)
              rule->PosRefEdge = rule->PosEdge = Left;
            else
              rule->PosRefEdge = rule->PosEdge = Top;
          }
        rule->PosAbRef = pRefAb;
      }
  
  if (pRefAb && pRefAb->AbBox &&
      pRefAb->AbBox->BxType == BoFloatGhost)
    {
      // get a valid child
      pRefAb = pRefAb->AbFirstEnclosed;
      while (pRefAb && pRefAb->AbFloat == 'N')
         {
           if (pRefAb != pAb && pRefAb->AbLeafType == LtCompound)
             pRefAb = pRefAb->AbFirstEnclosed;
           else
             pRefAb = pRefAb->AbNext;
        }
      rule->PosAbRef = pRefAb;
    }

  if (pRefAb && pRefAb->AbBox &&
      (pRefAb->AbBox->BxType == BoStructGhost ||
       pRefAb->AbBox->BxType == BoGhost))
    {
      /* the box position is computed by the line formatter */
      if (horizRef)
        {
          pAb->AbHorizPosChange = FALSE;
          if (pRefAb->AbBox->BxType == BoGhost &&
              pAb->AbElement && pAb->AbElement->ElStructSchema &&
              !strcmp (pAb->AbElement->ElStructSchema->SsName, "Template") &&
              pAb->AbPresentationBox && pAb->AbTypeNum != 0)
            {
              pAb->AbNotInLine = FALSE;
              pAb->AbHorizEnclosing = TRUE;
            }
        }
      else
        {
          pAb->AbVertPosChange = FALSE;
          if (pRefAb->AbBox->BxType == BoGhost &&
              pAb->AbElement && pAb->AbElement->ElStructSchema &&
              !strcmp (pAb->AbElement->ElStructSchema->SsName, "Template") &&
              pAb->AbPresentationBox && pAb->AbTypeNum != 0)
            {
              pAb->AbNotInLine = FALSE;
              pAb->AbVertEnclosing = TRUE;
            }
        }
      return;
    }

  if (horizRef)
    {
      /* Horizontal rule */
      if (pRefAb == pAb)
        {
          /* could not depend on itself */
          pAb->AbHorizPos.PosAbRef = NULL;
          if (pAb->AbWidth.DimIsPosition)
            {
              pAb->AbWidth.DimIsPosition = FALSE;
              pAb->AbWidth.DimUserSpecified = FALSE;
            }
          /* don't process this rule */
          pRefAb = NULL;
        }
      else if (pRefAb && pRefAb == pAb->AbPrevious &&
               pRefAb->AbHorizPos.PosAbRef == pAb)
        {
          /* Bad rule: change the ref to the previous box */
          while (pRefAb && pRefAb->AbHorizPos.PosAbRef == pAb)
            pRefAb = pRefAb->AbPrevious;
          pAb->AbHorizPos.PosAbRef = pRefAb;
        }
      if ((pParentAb && pParentAb->AbDisplay == 'I' ||
           pParentAb && pParentAb->AbDisplay == 'b') &&
          pParentAb->AbFloat == 'N' &&  !ExtraAbFlow (pAb, frame))
        {
          // force inline display
          pRefAb = NULL;
          pAb->AbHorizPos.PosAbRef = NULL;
          pAb->AbHorizPos.PosDistDelta = 0;
        }

      if (pRefAb == NULL)
        {
          /* default rule */
          if (pParentAb == NULL)
            {
              /* Root box */
              refEdge = rule->PosRefEdge;
              localEdge = rule->PosEdge;
              /* Convert the distance value */
              dist = GetPixelValue (rule->PosDistance, rule->PosUnit, x, pAb,
                                    ViewFrameTable[frame - 1].FrMagnification);
              dist += GetPixelValue (rule->PosDistDelta, rule->PosDeltaUnit, x, pAb,
                                     ViewFrameTable[frame - 1].FrMagnification);
            }
          else
            {
              /* there is an enclosing box */
              pRefAb = GetPosRelativeAb (pAb, horizRef);
              /* Si oui -> A droite de sa boite */
              if (pRefAb && rule->PosUnit != UnPercent)
                {
                  /* At the right of the previous */
                  sibling = TRUE;
                  pRefBox = pRefAb->AbBox;
                  dist = 0;
                  refEdge = Right;
                  localEdge = Left;
                  op = OpHorizDep;
                }
              else
                {
                  /* depend on the enclosing box */
                  pRefAb = pParentAb;
                  pRefBox = pRefAb->AbBox;
                  if (rule->PosUnit == UnPercent)
                    /* poucentage de la largeur de l'englobant */
                    dist = PixelValue (rule->PosDistance, UnPercent,
                                       (PtrAbstractBox) pParentAb, 0);
                  else
                    dist = 0;
                  if (rule->PosDeltaUnit == UnPercent)
                    /* poucentage de la largeur de l'englobant */
                    dist += PixelValue (rule->PosDistDelta, UnPercent,
                                        (PtrAbstractBox) pParentAb, 0);
                  refEdge = Left;
                  localEdge = Left;
                  if (pBox->BxHorizFlex)
                    op = OpHorizDep;
                  else
                    op = OpHorizInc;
                }
            }
        }
      else
        {
          /* explicite rule */
          while (pRefAb && pRefAb->AbBox &&
                 (pRefAb->AbBox->BxType == BoGhost ||
                  pRefAb->AbBox->BxType == BoStructGhost ||
                  pRefAb->AbBox->BxType == BoFloatGhost))
            pRefAb = pRefAb->AbEnclosing;
          if (pParentAb == pRefAb && !pBox->BxHorizFlex)
            /* it's not a stretchable box and it depends on its enclosing */
            op = OpHorizInc;
          else
            {
              op = OpHorizDep;
              /* new position */
              pBox->BxXToCompute = FALSE;
              if (pRefAb->AbEnclosing != pParentAb)
                {
                  /* it's a relation out of structure */
                  if (!IsXPosComplete (pBox))
                    pBox->BxXToCompute = TRUE;
                  pBox->BxXOutOfStruct = TRUE;
                  PropagateXOutOfStruct (pAb, frame, TRUE, pAb->AbHorizEnclosing);
                }
              else if (pRefAb->AbBox)
                {
                  /* depend on a sibling box */
                  sibling = TRUE;
                  if (pRefAb->AbBox->BxXOutOfStruct
                      || (pRefAb->AbBox->BxHorizFlex)
                      || (pRefAb->AbBox->BxWOutOfStruct && refEdge != Left))
                    {
                      /* The box inherits out from a structure position */
                      /* or depends on a out of structure dimension */
                      if (!IsXPosComplete (pBox))
                        pBox->BxXToCompute = TRUE;
                      pBox->BxXOutOfStruct = TRUE;
                      PropagateXOutOfStruct (pAb, frame, TRUE, pAb->AbHorizEnclosing);
                    }
                }
            }

          refEdge = rule->PosRefEdge;
          localEdge = rule->PosEdge;
          /* Convert the distance value */
          dim = pParentAb->AbBox->BxW;
          if (rule->PosUnit == UnPercent)
            {
              dist = PixelValue (rule->PosDistance, UnPercent,
                                 (PtrAbstractBox) dim, 0);
              /* Change the rule for further updates */
              pAb->AbHorizPos.PosDistance = dist;
              pAb->AbHorizPos.PosUnit = UnPixel;
            }
          else
            dist = PixelValue (rule->PosDistance, rule->PosUnit, pAb,
                               ViewFrameTable[frame - 1].FrMagnification);
          if (rule->PosDeltaUnit == UnPercent)
            {
              d = PixelValue (rule->PosDistDelta, UnPercent,
                              (PtrAbstractBox) dim, 0);
              /* Change the rule for further updates */
              pAb->AbHorizPos.PosDistDelta = d;
              pAb->AbHorizPos.PosUnit = UnPixel;
            }
          else
            d = PixelValue (rule->PosDistDelta, rule->PosDeltaUnit, pAb,
                            ViewFrameTable[frame - 1].FrMagnification);
          dist += d;
        }
    }
  else
    {
      /* Vertical rule */
      if ((pParentAb && pParentAb->AbDisplay == 'I' ||
           pParentAb && pParentAb->AbDisplay == 'b') &&
          pParentAb->AbFloat == 'N' &&  !ExtraAbFlow (pAb, frame))
        {
          // force inline display
          pRefAb = NULL;
          pAb->AbVertPos.PosAbRef = NULL;
          pAb->AbVertPos.PosDistDelta = 0;
        }

      if (pRefAb == pAb)
        {
          /* could not depend on itself */
          pAb->AbVertPos.PosAbRef = NULL;
          if (pAb->AbHeight.DimIsPosition)
            {
              pAb->AbHeight.DimIsPosition = FALSE;
              pAb->AbHeight.DimUserSpecified = FALSE;
            }
          /* don't process this rule */
          pRefAb = NULL;
        }
      else if (pRefAb && pRefAb == pAb->AbPrevious &&
               pRefAb->AbVertPos.PosAbRef == pAb)
        {
          /* Bad rule: change the ref to the previous box */
          while (pRefAb &&
                 pRefAb->AbVertPos.PosAbRef == pAb)
            pRefAb = pRefAb->AbPrevious;
          pAb->AbVertPos.PosAbRef = pRefAb;
        }

      if (pRefAb == NULL)
        {
          /* default rule */
          if (pParentAb == NULL)
            {
              /* Root box */
              refEdge = rule->PosRefEdge;
              localEdge = rule->PosEdge;
              /* Convert the distance value */
              dist = GetPixelValue (rule->PosDistance, rule->PosUnit, y, pAb,
                                    ViewFrameTable[frame - 1].FrMagnification);
              dist += GetPixelValue (rule->PosDistDelta, rule->PosDeltaUnit, y, pAb,
                                     ViewFrameTable[frame - 1].FrMagnification);
            }
          else
            {
              /* there is an enclosing box */
              pRefAb = GetPosRelativeAb (pAb, horizRef);
              if (pRefAb &&
                  pRefAb->AbVisibility >= ViewFrameTable[frame - 1].FrVisibility)
                {
                  /* Align baselines */
                  sibling = TRUE;
                  pRefBox = pRefAb->AbBox;
                  dist = 0;
                  refEdge = HorizRef;
                  localEdge = HorizRef;
                  op = OpVertDep;
                }
              else
                {
                  /* depend on the enclosing box */
                  pRefAb = pParentAb;
                  pRefBox = pRefAb->AbBox;
                  dist = 0;
                  refEdge = Top;
                  localEdge = Top;
                  if (pBox->BxVertFlex)
                    op = OpVertDep;
                  else
                    op = OpVertInc;
                }
            }
        }
      else
        {
          /* explicite rule */
          if (pParentAb == pRefAb &&
              pBox->BxVertFlex && pBox->BxType == BoCell)
            {
              if (!pBox->BxVertInverted &&
                  (rule->PosRefEdge != Top || rule->PosEdge != Top))
                {
                  /* a specific patch for vertical extended cells */
                  rule->PosRefEdge = Top;
                  rule->PosEdge = Top;
                }
              else if (pBox->BxVertInverted &&
                       (rule->PosRefEdge != Top || rule->PosEdge != Bottom))
                {
                  /* a specific patch for vertical extended cells */
                  pAb->AbVertPos.PosRefEdge = Bottom;
                  pAb->AbVertPos.PosEdge = Bottom;
                  pAb->AbVertPos.PosDistance = 0;
                  pAb->AbVertPos.PosDistDelta = 0;
                  rule->PosRefEdge = Top;
                  rule->PosEdge = Bottom;
                  rule->PosDistance = 0;
                  rule->PosDistDelta = 0;
                }
            }

          if (pParentAb == pRefAb && !pBox->BxVertFlex)
            /* it's not a stretchable box and it depends on its enclosing */
            op = OpVertInc;
          else
            {
              /* new position */
              pBox->BxYToCompute = FALSE;
              op = OpVertDep;
              if (pRefAb->AbEnclosing != pParentAb)
                {
                  /* it's a relation out of structure */
                  if (!IsYPosComplete (pBox))
                    pBox->BxYToCompute = TRUE;
                  pBox->BxYOutOfStruct = TRUE;
                  PropagateYOutOfStruct (pAb, frame, TRUE, pAb->AbVertEnclosing);
                }
              else if (pRefAb->AbBox)
                {
                  /* depend on a sibling box */
                  sibling = TRUE;
                  if (pRefAb->AbBox->BxYOutOfStruct
                      || (pRefAb->AbBox->BxHorizFlex && pRefAb->AbLeafType == LtCompound &&
                          pRefAb->AbInLine && refEdge != Top)
                      || (pRefAb->AbBox->BxHOutOfStruct && refEdge != Top))
                    {
                      /* The box inherits out from a structure position */
                      /* or depends on a out of structure dimension */
                      if (!IsYPosComplete (pBox))
                        pBox->BxYToCompute = TRUE;
                      pBox->BxYOutOfStruct = TRUE;
                      PropagateYOutOfStruct (pAb, frame, TRUE, pAb->AbVertEnclosing);
                    }
                }	
            }
	  
          refEdge = rule->PosRefEdge;
          localEdge = rule->PosEdge;
          /* Convert the distance value */
          dim = pParentAb->AbBox->BxH;
          if (rule->PosUnit == UnPercent)
            {
              dist = PixelValue (rule->PosDistance, UnPercent, (PtrAbstractBox) dim, 0);
              /* Change the rule for further updates */
              pAb->AbVertPos.PosDistance = dist;
              pAb->AbVertPos.PosUnit = UnPixel;
            }
          else
            dist = PixelValue (rule->PosDistance, rule->PosUnit, pAb,
                               ViewFrameTable[frame - 1].FrMagnification);
          if (rule->PosDeltaUnit == UnPercent)
            {
              d = PixelValue (rule->PosDistDelta, UnPercent, (PtrAbstractBox) dim, 0);
              /* Change the rule for further updates */
              pAb->AbVertPos.PosDistDelta = d;
              pAb->AbVertPos.PosDeltaUnit = UnPixel;
            }
          else
            d = PixelValue (rule->PosDistDelta, rule->PosDeltaUnit, pAb,
                            ViewFrameTable[frame - 1].FrMagnification);
          dist += d;
        }
    }

  if (pRefAb)
    {
      /* Depend on another box */
      pRefBox = pRefAb->AbBox;
      if (pRefBox == NULL)
        {
          /* the referred box doesn't already exist */
          pRefBox = GetBox (pRefAb);
          if (pRefBox)
            pRefAb->AbBox = pRefBox;
          else
            /* memory allocation trouble */
            return;
        }
      if (pRefBox->BxType == BoSplit && pRefBox->BxNexChild)
        {
          box = pRefBox->BxNexChild;
          x = box->BxXOrg;
          y = box->BxYOrg;
        }
      else if (pRefBox->BxType == BoGhost ||
               pRefBox->BxType == BoStructGhost ||
               pRefBox->BxType == BoFloatGhost)
        {
          pChildAb = pRefAb;
          while ((pChildAb->AbBox->BxType == BoGhost ||
                  pChildAb->AbBox->BxType == BoStructGhost ||
                  pChildAb->AbBox->BxType == BoFloatGhost) &&
                 pChildAb->AbFirstEnclosed &&
                 pChildAb->AbFirstEnclosed->AbBox)
            pChildAb = pChildAb->AbFirstEnclosed;
          box = pChildAb->AbBox;
          x = box->BxXOrg;
          y = box->BxYOrg;
        }
      else
        {
#ifdef _GL
          if (pRefAb &&
              !pRefAb->AbPresentationBox &&
              pRefAb->AbElement &&
              pRefAb->AbElement->ElSystemOrigin && 
              pRefAb->AbNext != pAb &&
              FrameTable[frame].FrView == 1)
            {	      
              x = 0;
              y = 0;
            }
          else
            {
              x = pRefBox->BxXOrg;
              y = pRefBox->BxYOrg;
            }
#else /* _GL */
          x = pRefBox->BxXOrg;
          y = pRefBox->BxYOrg;
#endif /* _GL */
          /* List-item presentation boxes take into account the left margin */
          if (pAb->AbPresentationBox && pAb->AbTypeNum == 0)
            {
              if (pRefAb->AbDirection == 'L')
                x += pRefBox->BxLMargin;
              else
                x -= pRefBox->BxRMargin;
            }
        }

      if (pRefAb == pParentAb && pAb->AbVertEnclosing)
        {
          GetExtraMargins (pRefBox, frame, FALSE, &t, &b, &l, &r);
          t += pRefBox->BxTMargin + pRefBox->BxTBorder + pRefBox->BxTPadding;
          l += pRefBox->BxLMargin + pRefBox->BxLBorder + pRefBox->BxLPadding;
          b += pRefBox->BxBMargin + pRefBox->BxBBorder + pRefBox->BxBPadding;
          r += pRefBox->BxRMargin + pRefBox->BxRBorder + pRefBox->BxRPadding;
        }
      else
        {
          t = 0;
          l = 0;
          b = 0;
          r = 0;
        }
      switch (refEdge)
        {
        case Top:
          y += t;
          if (!sibling && pBox->BxTMargin > 0 && pRefBox->BxTMargin > 0)
            {
              /* ignore the smaller margin */
              if (pBox->BxTMargin > pRefBox->BxTMargin)
                y -= pRefBox->BxTMargin;
              else
                y -= pBox->BxTMargin;
            }
          else if (sibling && localEdge == Bottom && pBox->BxBMargin > 0)
            {
              /* t = 0, take the larger margin into account */
              if (pBox->BxBMargin > pRefBox->BxTMargin)
                y = y + pBox->BxBMargin - pRefBox->BxTMargin;
              else
                y = y - pBox->BxBMargin + pRefBox->BxTMargin;
            }
          break;
        case Left:
          x += l;
          if (sibling && localEdge == Right && pBox->BxRMargin > 0)
            {
              /* l = 0, take the larger margin into account */
              if (pBox->BxRMargin > pRefBox->BxLMargin)
                x = x - (pBox->BxRMargin - pRefBox->BxLMargin);
              else
                x = x - (pRefBox->BxLMargin - pBox->BxRMargin);
            }
          break;
        case Bottom:
          y = y + pRefBox->BxHeight - b;
          if (!sibling && pBox->BxBMargin > 0 && pRefBox->BxBMargin > 0)
            {
              /* ignore the smaller margin */
              if (pBox->BxBMargin > pRefBox->BxBMargin)
                y -= pRefBox->BxBMargin;
              else
                y -= pBox->BxBMargin;
            }
          else if (sibling && localEdge == Top && pBox->BxTMargin > 0)
            {
              /* b = 0, take the larger margin into account */
              if (pBox->BxTMargin > pRefBox->BxBMargin)
                y = y - pRefBox->BxBMargin;
              else
                y = y - pBox->BxTMargin;
            }
          break;
        case Right:
          x = x + pRefBox->BxWidth - r;
          if (sibling && localEdge == Left && pBox->BxLMargin > 0)
            {
              /* l = 0, take the larger margin into account */
              if (pBox->BxLMargin > pRefBox->BxRMargin)
                x = x - pRefBox->BxRMargin;
              else
                x = x - pBox->BxLMargin;
            }
          break;
        case HorizRef:
          y = y + pRefBox->BxHorizRef;
          break;
        case VertRef:
          x = x + pRefBox->BxVertRef;
          break;
        case HorizMiddle:
          y = y + t + (pRefBox->BxH / 2);
          break;
        case VertMiddle:
          x = x + l + (pRefBox->BxW / 2);
          break;
        default:
          break;
        }
    }
  else
    {
      switch (refEdge)
        {
        case VertRef:
        case VertMiddle:
          x = x / 2;
          break;
        case HorizRef:
        case HorizMiddle:
          y = y / 2;
          break;
        case Right:
        case Bottom:
          break;
        default:
          x = 0;
          y = 0;
          break;
        }
    }
 
  /* Calcule l'origine de la boite et les points fixes */
  if (horizRef)
    {
      /* default value */
      pBox->BxHorizEdge = Left;
      SetPositionConstraint (localEdge, pBox, &x);
    }
  else
    {
      /* default value */
      pBox->BxVertEdge = Top;
      if (pRefAb && pRefAb->AbVisibility >= ViewFrameTable[frame - 1].FrVisibility)
        SetPositionConstraint (localEdge, pBox, &y);
    }

  /* invisible boxes take no space */
  if (pBox->BxAbstractBox->AbVisibility < ViewFrameTable[frame - 1].FrVisibility)
    dist = 0;
  else if (pRefAb &&
           pRefAb->AbVisibility < ViewFrameTable[frame - 1].FrVisibility)
    dist = 0;
   
  /* Met a jour l'origine de la boite suivant la relation indiquee */
  if (horizRef)
    {
      if (!pBox->BxHorizFlex)
        {
          x = x + dist - pBox->BxXOrg;
          ClearBoxMoved (pBox);
          if (pBox->BxXToCompute)
            /* Force le placement des boites filles */
            XMoveAllEnclosed (pBox, x, frame);
          else
            XMove (pBox, NULL, x, frame);
        }
      /* la regle de position est interpretee */
      pAb->AbHorizPosChange = FALSE;
    }
  else
    {
      if (!pBox->BxVertFlex)
        {
          y = y + dist - pBox->BxYOrg;
          if (TypeHasException (ExcIsCaption, pAb->AbElement->ElTypeNumber,
                                pAb->AbElement->ElStructSchema))
            {
              // don't take into account the table border to display the caption
              //pParentAb = pAb->AbEnclosing;
              if (pParentAb && pParentAb->AbBox)
                {
                if (localEdge == Bottom && pParentAb->AbBox->BxTBorder)
                  y -= pParentAb->AbBox->BxTBorder;
                else if (pParentAb->AbBox->BxBBorder)
                  y += pParentAb->AbBox->BxTBorder;
                }
            }
          ClearBoxMoved (pBox);
          if (pBox->BxYToCompute)
            /* Force le placement des boites filles */
            YMoveAllEnclosed (pBox, y, frame);
          else
            YMove (pBox, NULL, y, frame);
        }
      /* la regle de position est interpretee */
      pAb->AbVertPosChange = FALSE;
    }

  /* Il faut mettre a jour les dependances des boites */
  if (pRefAb != NULL && pRefBox != NULL)
    {
      InsertPosRelation (pBox, pRefBox, op, localEdge, refEdge);
      ClearBoxMoved (pRefBox);
       
      if (horizRef && pBox->BxHorizFlex)
        MoveBoxEdge (pBox, pRefBox, op, x + dist - pBox->BxXOrg, frame, TRUE);
      else if (!horizRef && pBox->BxVertFlex)
        MoveBoxEdge (pBox, pRefBox, op, y + dist - pBox->BxYOrg, frame, FALSE);
      ClearBoxMoved (pRefBox);
    }
  /* break down the temporary link of moved boxes */
  ClearBoxMoved (pBox);
}


/*----------------------------------------------------------------------
  GetHPosRelativeBox search the box which horizontally links   
  pBox to is enclosing:                                
  - If relations were not updated, that box is BxHorizInc
  - If the box has the relation OpHorizInc, it's itself
  - Else get the sibling box which gives the position.     
  Return a box or NULL.
  ----------------------------------------------------------------------*/
PtrBox GetHPosRelativeBox (PtrBox pBox, PtrBox pPreviousBox)
{
  PtrBox              pRelativeBox;
  PtrPosRelations     pPosRel;
  BoxRelation        *pRelation;
  int                 i;
  ThotBool            notEmpty;

  /* On verifie que la boite n'a pas deja ete examinee */
  if (pPreviousBox != NULL)
    {
      pRelativeBox = pPreviousBox->BxMoved;
      while (pRelativeBox != NULL && pRelativeBox != pBox)
        pRelativeBox = pRelativeBox->BxMoved;
    }
  else
    pRelativeBox = NULL;

  if (pRelativeBox == NULL)
    {
      /* On met a jour la pile des boites traitees */
      pBox->BxMoved = pPreviousBox;
      /* regarde si on connait deja la boite qui la relie a l'englobante */
      if (pBox->BxHorizInc != NULL)
        pRelativeBox = pBox->BxHorizInc;
      else
        pRelativeBox = NULL;

      /* Si la position de la boite depend d'une boite externe on prend la */
      /* boite elle meme comme reference.                                  */
      if (pBox->BxXOutOfStruct)
        pRelativeBox = pBox;

      /* regarde si la boite est reliee a son englobante */
      pPosRel = pBox->BxPosRelations;
      while (pRelativeBox == NULL && pPosRel != NULL)
        {
          i = 0;
          notEmpty = (pPosRel->PosRTable[i].ReBox != NULL);
          while (i < MAX_RELAT_POS && notEmpty)
            if (pPosRel->PosRTable[i].ReOp == OpHorizInc)
              {
                /* On a trouve */
                pRelativeBox = pBox;
                i = MAX_RELAT_POS;
              }
            else
              {
                i++;
                if (i < MAX_RELAT_POS)
                  notEmpty = pPosRel->PosRTable[i].ReBox != NULL;
              }
	  
          /* Bloc suivant */
          pPosRel = pPosRel->PosRNext;
        }

      /* Sinon on recherche la boite soeur qui l'est */
      pPosRel = pBox->BxPosRelations;
      while (pRelativeBox == NULL && pPosRel != NULL)
        {
          i = 0;
          notEmpty = pPosRel->PosRTable[i].ReBox != NULL;
          while (i < MAX_RELAT_POS && notEmpty)
            {
              pRelation = &pPosRel->PosRTable[i];
              if (pRelation->ReBox->BxAbstractBox != NULL &&
                  pRelation->ReOp == OpHorizDep &&
                  /* Si c'est la bonne relation de dependance */
                  pRelation->ReBox->BxAbstractBox->AbHorizPos.PosAbRef != pBox->BxAbstractBox)
                {
                  /* la position de la boite depend d'une boite elastique */
                  /* -> on prend la boite elastique comme reference       */
                  if (pRelation->ReBox->BxHorizFlex)
                    pRelativeBox = pRelation->ReBox;
                  else
                    pRelativeBox = GetHPosRelativeBox (pRelation->ReBox, pBox);
		  
                  /* Est-ce que l'on a trouve la boite qui donne la position ? */
                  if (pRelativeBox != NULL)
                    {
                      i = MAX_RELAT_POS;
                      /* La position depend d'une relation hors-structure ? */
                      if (pRelativeBox->BxXOutOfStruct)
                        pBox->BxXOutOfStruct = TRUE;
                    }
                  else
                    {
                      i++;
                      if (i < MAX_RELAT_POS)
                        notEmpty = pPosRel->PosRTable[i].ReBox != NULL;
                    }
                }
              else
                {
                  i++;
                  if (i < MAX_RELAT_POS)
                    notEmpty = pPosRel->PosRTable[i].ReBox != NULL;
                }
            }
          /* Bloc suivant */
          pPosRel = pPosRel->PosRNext;
        }

      pBox->BxHorizInc = pRelativeBox;
      return pRelativeBox;
    }
  else
    return NULL;
}


/*----------------------------------------------------------------------
  GetVPosRelativeBox search the box which vertically links 
  pBox to is enclosing:                                
  - If relations were not updated, that box is BxVertInc
  - If the box has the relation OpVertInc, it's itself
  - Else get the sibling box which gives the position.     
  Return a box or NULL.                        
  ----------------------------------------------------------------------*/
PtrBox GetVPosRelativeBox (PtrBox pBox, PtrBox pPreviousBox)
{
  PtrBox              pRelativeBox;
  PtrPosRelations     pPosRel;
  BoxRelation        *pRelation;
  int                 i;
  ThotBool            notEmpty;

  /* On verifie que la boite n'a pas deja ete examinee */
  if (pPreviousBox != NULL)
    {
      pRelativeBox = pPreviousBox->BxMoved;
      while (pRelativeBox != NULL && pRelativeBox != pBox)
        pRelativeBox = pRelativeBox->BxMoved;
    }
  else
    pRelativeBox = NULL;

  if (pRelativeBox == NULL)
    {
      
      /* On met a jour la pile des boites traitees */
      pBox->BxMoved = pPreviousBox;
      
      /* regarde si on connait deja la boite qui la relie a l'englobante */
      if (pBox->BxVertInc != NULL)
        pRelativeBox = pBox->BxVertInc;
      else
        pRelativeBox = NULL;
      
      /* Si la position de la boite depend d'une boite externe on prend la */
      /* boite elle meme comme reference.                                  */
      if (pBox->BxYOutOfStruct)
        pRelativeBox = pBox;
      
      /* regarde si la boite est reliee a son englobante */
      pPosRel = pBox->BxPosRelations;
      while (pRelativeBox == NULL && pPosRel != NULL)
        {
          i = 0;
          notEmpty = (pPosRel->PosRTable[i].ReBox != NULL);
          while (i < MAX_RELAT_POS && notEmpty)
            if (pPosRel->PosRTable[i].ReOp == OpVertInc)
              {
                /* On a trouve */
                pRelativeBox = pBox;
                i = MAX_RELAT_POS;
              }
            else
              {
                i++;
                if (i < MAX_RELAT_POS)
                  notEmpty = pPosRel->PosRTable[i].ReBox != NULL;
              }
          /* Bloc suivant */
          pPosRel = pPosRel->PosRNext;
        }

      /* Sinon on recherche la boite soeur qui l'est */
      pPosRel = pBox->BxPosRelations;
      while (pRelativeBox == NULL && pPosRel != NULL)
        {
          i = 0;
          notEmpty = pPosRel->PosRTable[i].ReBox != NULL;
          while (i < MAX_RELAT_POS && notEmpty)
            {
              pRelation = &pPosRel->PosRTable[i];
              if (pRelation->ReBox->BxAbstractBox != NULL &&
                  pRelation->ReOp == OpVertDep &&
                  /* Si c'est la bonne relation de dependance */
                  pRelation->ReBox->BxAbstractBox->AbVertPos.PosAbRef != pBox->BxAbstractBox)
                {
                  /* Si la position de la boite depend d'une boite elastique */
                  /* on prend la boite elastique comme reference             */
                  if (pRelation->ReBox->BxVertFlex)
                    pRelativeBox = pRelation->ReBox;
                  else
                    pRelativeBox = GetVPosRelativeBox (pRelation->ReBox, pBox);
                  /* Est-ce que l'on a trouve la boite qui donne la position ? */
                  if (pRelativeBox != NULL)
                    {
                      i = MAX_RELAT_POS;
                      /* La position depend d'une relation hors-structure ? */
                      if (pRelativeBox->BxYOutOfStruct)
                        pBox->BxYOutOfStruct = TRUE;
                    }
                  else
                    {
                      i++;
                      if (i < MAX_RELAT_POS)
                        notEmpty = pPosRel->PosRTable[i].ReBox != NULL;
                    }
                }
              else
                {
                  i++;
                  if (i < MAX_RELAT_POS)
                    notEmpty = pPosRel->PosRTable[i].ReBox != NULL;
                }
            }
          /* Bloc suivant */
          pPosRel = pPosRel->PosRNext;
        }

      pBox->BxVertInc = pRelativeBox;
      return pRelativeBox;
    }
  else
    return (NULL);
}

/*----------------------------------------------------------------------
  GetPercentDim returns the related dimension used for the percentage
  ----------------------------------------------------------------------*/
int GetPercentDim (PtrAbstractBox pAb, PtrAbstractBox pParentAb, ThotBool horizRef)
{
  PtrElement          parent;
  AbDimension        *pDimAb;
  int                 val = 0;

  if (pAb && pAb->AbEnclosing)
    {
      if (horizRef)
        {
          val = pParentAb->AbBox->BxW;
          pDimAb = &pAb->AbWidth;
        }
      else
        {
          val = pParentAb->AbBox->BxH;
          pDimAb = &pAb->AbHeight;
        }
      parent = pParentAb->AbElement;
      // check if the precent concerns a SVG box
      if (pDimAb->DimUnit == UnPercent &&
          parent && parent->ElStructSchema &&
          parent->ElStructSchema->SsName &&
          !strcmp (parent->ElStructSchema->SsName, "SVG"))
        {
          // check if there is an enclosing viewbox
          while (parent &&
                 (parent->ElTransform == NULL ||
                  parent->ElTransform->TransType != PtElViewBox))
            {
              parent = parent->ElParent;
              if (parent &&
                  strcmp (parent->ElStructSchema->SsName, "SVG"))
                parent = NULL;
            }
          if (parent)
            {
              if (horizRef)
                val =  parent->ElTransform->VbWidth;
              else
                val =  parent->ElTransform->VbHeight;
            }
        }
    }
  return val;
}


/*----------------------------------------------------------------------
  ComputeDimRelation applies the vertical/horizontal sizing rule of pAb
  according to the parameter horizRef. 
  Return TRUE when the dimension depends on the contents.
  There are 4 different cases:
  - if the dim depends on the parent the rule concerns the outside
  dim and is related to the inside dim of the parent.
  - if it's the default dim the rule concerns the inside dim and is 
  related to the outside dim of children.
  - if the dim depends on another box the rule concerns the outside
  dim and is related to the outside dim of that box.
  - if the rule gives an absolute dim the rule concerns the inside
  dim of the box.
  The box size BxW or BxH is updated and dependencies between boxes are
  registered.

  Relation between values:
  ^
  BxXOrg
  <-LMargin-><-LBorder-><-LPadding-><-W-><-RPadding-><-RBorder-><-LRargin->
  <---------------------------------Width--------------------------------->
  ----------------------------------------------------------------------*/
ThotBool  ComputeDimRelation (PtrAbstractBox pAb, int frame, ThotBool horizRef)
{
  PtrBox              pRefBox, pBox;
  PtrAbstractBox      pParentAb, pColumn, pChildAb;
  PtrElement          pEl, parent;
  PtrLine             pLine;
  Positioning        *pos;
  OpRelation          op;
  AbDimension        *pDimAb;
  AbPosition         *pPosAb;
  int                 val, delta, i;
  int                 dx, dy, dim, inx, iny;
  int                 t, b, l, r, zoom;
  ThotBool            inLine, isExtraFlow;
  ThotBool            defaultDim;

  pBox = pAb->AbBox;
  zoom = ViewFrameTable[frame - 1].FrMagnification;
  pParentAb = pAb->AbEnclosing;
  if (pBox->BxType == BoStructGhost)
    {
      pChildAb = pAb->AbFirstEnclosed;
      while (pChildAb && pChildAb->AbPresentationBox)
        pChildAb = pChildAb->AbNext;
      while (pChildAb && pChildAb->AbBox &&
             pChildAb->AbBox->BxType == BoStructGhost)
        {
          pChildAb = pChildAb->AbFirstEnclosed;
          while (pChildAb && pChildAb->AbPresentationBox)
            pChildAb = pChildAb->AbNext;
        }
      if (pChildAb)
        {
          if (horizRef && pChildAb->AbWidth.DimAbRef &&
              pChildAb->AbWidth.DimAbRef != pChildAb->AbEnclosing)
            {
              if (pChildAb->AbWidth.DimAbRef == NULL)
                {
                  /* inherit from contents */
                  pAb->AbWidth.DimAbRef = NULL;
                  pAb->AbWidth.DimValue = -1;
                  pBox->BxContentWidth = TRUE;
                  return TRUE;
                }
              else
                {
                  pAb->AbWidth.DimAbRef = pChildAb->AbWidth.DimAbRef;
                  pAb->AbWidth.DimValue = pChildAb->AbWidth.DimValue;
                  pAb->AbWidth.DimUnit = pChildAb->AbWidth.DimUnit;
                  pAb->AbWidth.DimSameDimension = TRUE;
                }
            }
          else if (!horizRef && pChildAb->AbHeight.DimAbRef &&
                   pChildAb->AbHeight.DimAbRef != pChildAb->AbEnclosing)
            {
              if (pChildAb->AbHeight.DimAbRef == NULL)
                {
                  /* inherit from contents */
                  pAb->AbHeight.DimAbRef = NULL;
                  pAb->AbHeight.DimValue = -1;
                  pBox->BxContentHeight = TRUE;
                  return TRUE;
                }
              else
                {
                  pAb->AbHeight.DimAbRef = pChildAb->AbHeight.DimAbRef;
                  pAb->AbHeight.DimValue = pChildAb->AbHeight.DimValue;
                  pAb->AbHeight.DimUnit = pChildAb->AbHeight.DimUnit;
                  pAb->AbHeight.DimSameDimension = TRUE;
                }
            }
        }
    }

  dx = dy = inx = iny = 0;
  if (pAb->AbLeafType == LtCompound)
    pos = pAb->AbPositioning;
  else
    pos = NULL;
  if (pParentAb)
    parent = pParentAb->AbElement;
  else
    parent = NULL;

  /* Check the box visibility */
  if (pAb->AbVisibility >= ViewFrameTable[frame - 1].FrVisibility)
    {
      /* check if the width is set by positioning rules */
      isExtraFlow = ExtraFlow (pBox, frame);
      if (isExtraFlow)
        {
          if (horizRef)
            {
              if (HorizExtraAbFlow (pAb, frame))
                {
                  pParentAb = GetEnclosingViewport (pAb);
                  if (pParentAb == NULL)
                    pParentAb = ViewFrameTable[frame -1].FrAbstractBox;
                }
              if (pAb->AbWidth.DimUnit != UnAuto &&
                   !pAb->AbWidth.DimIsPosition &&
                  (pAb->AbWidth.DimAbRef || pAb->AbWidth.DimValue != -1))
                ;
              else if (pos->PnLeftUnit != UnAuto &&
                       pos->PnLeftUnit != UnUndefined &&
                       pos->PnRightUnit != UnAuto &&
                       pos->PnRightUnit != UnUndefined)
                {
                  /* width fixed by left and right position */
                  pAb->AbWidth.DimIsPosition = TRUE;
                  pAb->AbWidth.DimPosition.PosAbRef = pParentAb;
                  pAb->AbWidth.DimPosition.PosDistance = pos->PnRightDistance;
                  pAb->AbWidth.DimPosition.PosDistDelta = 0;
                  pAb->AbWidth.DimPosition.PosEdge = Right;
                  pAb->AbWidth.DimPosition.PosRefEdge = Right;
                  pAb->AbWidth.DimPosition.PosUnit = pos->PnRightUnit;
                  return FALSE;
                }
              else if (!pAb->AbWidth.DimIsPosition &&
                       (pAb->AbWidth.DimAbRef == pAb->AbEnclosing ||
                        pAb->AbWidth.DimUnit == UnAuto))
                {
                  pAb->AbWidth.DimAbRef = pParentAb;
                  pAb->AbWidth.DimIsPosition = FALSE;
                  pAb->AbWidth.DimValue = 0;
                  if ((pos->PnAlgorithm == PnAbsolute ||
                      pos->PnAlgorithm == PnFixed) &&
                      pAb->AbWidth.DimUnit == UnAuto)
                    /* shrink does apply */
                    pBox->BxShrink = TRUE;
                }
            }
          if (!horizRef)
            {
              if (VertExtraAbFlow (pAb, frame))
                {
                  pParentAb = GetEnclosingViewport (pAb);
                  if (pParentAb == NULL)
                    pParentAb = ViewFrameTable[frame -1].FrAbstractBox;
                }
              if (pos->PnTopUnit != UnAuto &&
                  pos->PnTopUnit != UnUndefined &&
                  pos->PnBottomUnit != UnAuto &&
                  pos->PnBottomUnit != UnUndefined)
                {
                  /* inherit from an enclosing box */
                  pAb->AbHeight.DimIsPosition = FALSE;
                  pAb->AbHeight.DimAbRef = pParentAb;
                  pAb->AbHeight.DimValue = 0;
                  pAb->AbHeight.DimSameDimension = TRUE;
                  pAb->AbHeight.DimUserSpecified = FALSE;
                  return FALSE;
                }
              else if (!pAb->AbHeight.DimIsPosition &&
                       pAb->AbHeight.DimAbRef == pAb->AbEnclosing)
                {
                  pAb->AbHeight.DimAbRef = pParentAb;
                  pAb->AbHeight.DimIsPosition = FALSE;
                }
            }
        }
      else if (horizRef && pAb->AbWidth.DimUnit == UnAuto &&
               pAb->AbLeafType == LtCompound && pos &&
               (pos->PnAlgorithm == PnAbsolute ||
                pos->PnAlgorithm == PnFixed))
        /* no explicit left or right rule but shrink does apply */
        pBox->BxShrink = TRUE;
      else if (horizRef && pParentAb && pParentAb->AbBox &&
               (pAb->AbWidth.DimUnit == UnAuto || pAb->AbWidth.DimUnit == UnPercent))
        pBox->BxShrink = pParentAb->AbBox->BxShrink;

      /* Check validity of rules */
      if (horizRef && pAb->AbWidth.DimIsPosition &&
          pAb->AbWidth.DimPosition.PosAbRef == NULL)
        pAb->AbWidth.DimIsPosition = FALSE;
      if (!horizRef && pAb->AbHeight.DimIsPosition &&
          pAb->AbHeight.DimPosition.PosAbRef == NULL)
        pAb->AbHeight.DimIsPosition = FALSE;
      if (horizRef && pAb->AbWidth.DimIsPosition)
        {
          if (pAb->AbHorizPos.PosEdge == pAb->AbWidth.DimPosition.PosEdge
              || pAb->AbHorizPos.PosEdge == VertMiddle
              || pAb->AbHorizPos.PosEdge == VertRef)
            {
              /* invalid pos rule */
              if (pAb->AbWidth.DimPosition.PosEdge == Left)
                pAb->AbHorizPos.PosEdge = Right;
              else if (pAb->AbWidth.DimPosition.PosEdge == Right)
                pAb->AbHorizPos.PosEdge = Left;
            }
          else if (pAb->AbHorizPos.PosAbRef == NULL
                   && (pAb->AbWidth.DimPosition.PosAbRef == NULL ||
                       pAb->AbWidth.DimPosition.PosAbRef == pParentAb))
            {
              /* inherit from the enclosing box */
              pAb->AbHorizPos.PosAbRef = pParentAb;
              pAb->AbWidth.DimIsPosition = FALSE;
              pAb->AbWidth.DimAbRef = pParentAb;
              pAb->AbWidth.DimValue = 0;
              pAb->AbWidth.DimSameDimension = TRUE;
              pAb->AbWidth.DimUserSpecified = FALSE;
            }
          else if (pAb->AbHorizPos.PosAbRef == NULL
                   || pAb->AbWidth.DimPosition.PosEdge == VertRef
                   || pAb->AbWidth.DimPosition.PosAbRef == NULL
                   || pAb->AbWidth.DimPosition.PosAbRef == pAb)
            {
              /* invalid Width rule */
              fprintf (stderr, "Bad Width rule on %s\n", AbsBoxType (pAb, TRUE));
              pAb->AbWidth.DimIsPosition = FALSE;
              pAb->AbWidth.DimAbRef = NULL;
              pAb->AbWidth.DimValue = 20;	/* largeur fixe */
              pAb->AbWidth.DimUnit = UnPoint;
              pAb->AbWidth.DimUserSpecified = FALSE;
            }
          /* check if the relative box is not already dead */
          else if (IsDead (pAb->AbHorizPos.PosAbRef))
            {
              fprintf (stderr, "Dimension refers a dead box");
              pAb->AbWidth.DimIsPosition = FALSE;
              pAb->AbWidth.DimAbRef = NULL;
              pAb->AbWidth.DimValue = 20;	/* fixed width */
              pAb->AbWidth.DimUnit = UnPoint;
              pAb->AbWidth.DimUserSpecified = FALSE;
            }
        }
      else if (!horizRef && pAb->AbHeight.DimIsPosition)
        {
          if (pAb->AbVertPos.PosEdge == pAb->AbHeight.DimPosition.PosEdge
              || pAb->AbVertPos.PosEdge == HorizMiddle
              || pAb->AbVertPos.PosEdge == HorizRef)
            {
              /* invalid pos rule */
              if (pAb->AbHeight.DimPosition.PosEdge == Top)
                pAb->AbVertPos.PosEdge = Bottom;
              else if (pAb->AbHeight.DimPosition.PosEdge == Bottom)
                pAb->AbVertPos.PosEdge = Top;
            }
          else if (pAb->AbVertPos.PosAbRef == NULL
                   && (pAb->AbHeight.DimPosition.PosAbRef == NULL ||
                       pAb->AbHeight.DimPosition.PosAbRef == pParentAb))
            {
              /* inherit from the enclosing box */
              pAb->AbVertPos.PosAbRef = pParentAb;
              pAb->AbHeight.DimIsPosition = FALSE;
              pAb->AbHeight.DimAbRef = pParentAb;
              pAb->AbHeight.DimValue = 0;
              pAb->AbHeight.DimSameDimension = TRUE;
              pAb->AbHeight.DimUserSpecified = FALSE;
            }
          else if (pAb->AbVertPos.PosAbRef == NULL
                   || pAb->AbHeight.DimPosition.PosEdge == HorizRef
                   || pAb->AbHeight.DimPosition.PosAbRef == NULL
                   || pAb->AbHeight.DimPosition.PosAbRef == pAb)
            {
              /* invalid Height rule */
              fprintf (stderr, "Bad Height rule on %s\n", AbsBoxType (pAb, TRUE));
              pAb->AbHeight.DimIsPosition = FALSE;
              pAb->AbHeight.DimAbRef = NULL;
              pAb->AbHeight.DimValue = 20;	/* fixed height */
              pAb->AbHeight.DimUnit = UnPoint;
              pAb->AbHeight.DimUserSpecified = FALSE;
            }
          /* check if the relative box is not already dead */
          else if (IsDead (pAb->AbVertPos.PosAbRef))
            {
              fprintf (stderr, "Dimension refers a dead box");
              pAb->AbHeight.DimIsPosition = FALSE;
              pAb->AbHeight.DimAbRef = NULL;
              pAb->AbHeight.DimValue = 0;
              pAb->AbHeight.DimUnit = UnPoint;
              pAb->AbHeight.DimUserSpecified = FALSE;
            }
        }

      pEl = pAb->AbElement;
      if ((horizRef && !pAb->AbWidth.DimIsPosition) ||
          (!horizRef && !pAb->AbHeight.DimIsPosition))
        {
          /* It's not a stretchable box */
          if (horizRef)
            {
              pDimAb = &pAb->AbWidth;
              pBox->BxContentWidth = FALSE;
            }
          else
            {
              pDimAb = &pAb->AbHeight;
              pBox->BxContentHeight = FALSE;
            }

          if (pDimAb->DimAbRef && IsDead (pDimAb->DimAbRef))
            {
              /* the dimension refers a dead box */
              pDimAb->DimAbRef = NULL;
              pDimAb->DimValue = -1;
            }
          else if (pDimAb->DimAbRef == pAb && !pDimAb->DimSameDimension)
            {
              if (horizRef && pAb->AbHeight.DimUnit == UnPoint)
                pDimAb->DimUnit = UnPoint;
              else if (!horizRef && pAb->AbWidth.DimUnit == UnPoint)
                pDimAb->DimUnit = UnPoint;
            }
          else if (!horizRef &&
                   pAb->AbLeafType == LtGraphics &&
                   pAb->AbShape == 'a' &&
                   pDimAb->DimAbRef == NULL)
            {
              /* force the circle height to be equal to its width */
              pDimAb->DimAbRef = pAb;
              pDimAb->DimSameDimension = FALSE;
              pDimAb->DimValue = 0;
              pDimAb->DimUserSpecified = FALSE;
              if (pAb->AbWidth.DimUnit == UnPoint)
                pDimAb->DimUnit = UnPoint;
              else
                pDimAb->DimUnit = UnPixel;
            }
          else if (pAb->AbLeafType == LtPicture &&
                   !pAb->AbPresentationBox &&
                   pParentAb && parent &&
                   TypeHasException (ExcIsImg, parent->ElTypeNumber,
                                     parent->ElStructSchema))
            {
              /* change the rule according to the enclosing IMG */
              if ((horizRef && (!pParentAb->AbWidth.DimIsPosition &&
                                pParentAb->AbWidth.DimValue == -1 &&
                                pParentAb->AbWidth.DimAbRef == NULL)) ||
                  (!horizRef && (!pParentAb->AbHeight.DimIsPosition &&
                                 pParentAb->AbHeight.DimValue == -1 &&
                                 pParentAb->AbHeight.DimAbRef == NULL)))
                {
                  // default dimension
                  pDimAb->DimAbRef = NULL;
                  pDimAb->DimValue = -1;                  
                }
              else
                {
                  // inherit from enclosing
                  pDimAb->DimAbRef = pParentAb;
                  pDimAb->DimValue = 0;
                }
            }
          else if (horizRef && pDimAb->DimAbRef &&
                   pDimAb->DimUnit != UnAuto &&
                   pDimAb->DimAbRef == pParentAb &&
                   (!pParentAb->AbWidthChange ||
                    pParentAb->AbWidth.DimUnit != UnAuto) &&
                   pParentAb->AbWidth.DimAbRef == NULL &&
                   pParentAb->AbWidth.DimValue == -1)
            {
              /* the width depends on the parent width
                 when the parent width depends on its contents */
              pChildAb = pParentAb->AbFirstEnclosed;
              while (pChildAb && pChildAb->AbWidth.DimAbRef == pParentAb)
                pChildAb = pChildAb->AbNext;
              if (pChildAb == NULL)
                {
                  /* all child widths depend on the parent width */
                  pDimAb->DimAbRef = NULL;
                  pDimAb->DimValue = -1;
                }
            }
#ifdef IV
          else if (horizRef && pDimAb->DimUnit == UnAuto &&
                   pAb->AbFloat != 'N')
            {
              /* the width depends on the contents but the contents
               depends on the enclosing */
              pChildAb = pAb->AbFirstEnclosed;
              {
                i = 0;
                while (pChildAb)
                  {
                    if (pChildAb->AbWidth.DimUnit == UnPercent &&
                        pChildAb->AbFloat != 'N')
                      i += pChildAb->AbWidth.DimValue;
                    pChildAb = pChildAb->AbNext;
                  }
                if (i > 0)
                  {
                    pDimAb->DimUnit = UnPercent;
                    pDimAb->DimValue = i;
                  }
              }
            }
#endif
          else if (horizRef && pDimAb->DimUnit == UnPercent && pParentAb &&
                   pParentAb->AbFloat != 'N' &&
                   pParentAb->AbWidth.DimAbRef == NULL &&
                   pParentAb->AbWidth.DimValue == -1)
            {
              // the box width is a percent of the enclosing box
              // but the width of the enclosing box depends on the contents
              while (pParentAb->AbEnclosing &&
                     (pParentAb->AbWidth.DimUnit == UnAuto ||
                      (pParentAb->AbWidth.DimAbRef == NULL &&
                       pParentAb->AbWidth.DimValue == -1)))
                pParentAb = pParentAb->AbEnclosing;
            }
          else if (!horizRef && pDimAb->DimAbRef &&
                   pDimAb->DimUnit != UnAuto &&
                   pDimAb->DimAbRef == pParentAb &&
                   pParentAb->AbHeight.DimAbRef == NULL &&
                   pParentAb->AbHeight.DimValue == -1)
            {
              /* the height depends on the parent height
                 when the parent height depends on its contents */
              if (pAb->AbVertPos.PosAbRef != pParentAb)
                /* this cannnot work */
                pChildAb = NULL;
              else
                pChildAb = pParentAb->AbFirstEnclosed;
              while (pChildAb &&
                     (pChildAb->AbDead ||
                      pChildAb->AbHeight.DimAbRef == pAb->AbEnclosing))
                pChildAb = pChildAb->AbNext;
              if (pChildAb == NULL ||
                  (pParentAb->AbBox &&
                   (pParentAb->AbBox->BxType == BoBlock ||
                    pParentAb->AbBox->BxType == BoFloatBlock ||
                    pParentAb->AbBox->BxType == BoCellBlock ||
                    pParentAb->AbBox->BxType == BoStructGhost ||
                    pParentAb->AbBox->BxType == BoGhost)))
                {
                  /* all child heights depend on the parent height */
                  pDimAb->DimAbRef = NULL;
                  pDimAb->DimValue = -1;
                }
            }
          else if (!horizRef && pDimAb->DimAbRef == NULL &&
                   pDimAb->DimUnit != UnAuto &&
                   pDimAb->DimValue == -1 &&
                   pAb->AbLeafType == LtCompound)
            {
              /* the height depends on its contents, check if it only
                 includes one child which takes the enclosing height */
              pChildAb = pAb->AbFirstEnclosed;
              while (pChildAb &&
                     (pChildAb->AbDead || pChildAb->AbHeight.DimAbRef == pAb ||
                      pChildAb->AbHeight.DimUnit == UnPercent))
                pChildAb = pChildAb->AbNext;
              if (pAb->AbFirstEnclosed && pChildAb == NULL &&
                  (pAb->AbFirstEnclosed->AbLeafType == LtSymbol ||
                   pAb->AbFirstEnclosed->AbLeafType == LtText) &&
                  pAb->AbFirstEnclosed->AbVisibility >= ViewFrameTable[frame - 1].FrVisibility &&
                  pAb->AbFirstEnclosed->AbHeight.DimAbRef == pAb)
                {
                  // get the default height
                  pAb->AbFirstEnclosed->AbHeight.DimAbRef = NULL;
                  pAb->AbFirstEnclosed->AbHeight.DimValue = -1;
                }
            }

          /* Compute the delta width that must be substract to 100% width or enclosing width */
          GetExtraMargins (pBox, frame, FALSE, &t, &b, &l, &r);
          dx += l + r;
          if (pBox->BxLMargin > 0)
            dx += pBox->BxLMargin;
          else
            inx -= pBox->BxLMargin;
          dx += pBox->BxLBorder + pBox->BxLPadding + pBox->BxRBorder + pBox->BxRPadding;
          if (pBox->BxRMargin > 0)
            dx += pBox->BxRMargin;
          else
            inx -= pBox->BxRMargin;
          /* Compute the delta height that must be substract to 100% height or enclosing height */
          dy += t + b;
          if (pBox->BxTMargin > 0)
            dy += pBox->BxTMargin;
          else
            iny -= pBox->BxTMargin;
          dy += pBox->BxTBorder + pBox->BxTPadding + pBox->BxBBorder + pBox->BxBPadding;
          if (pBox->BxBMargin > 0)
            dy += pBox->BxBMargin;
          else
            iny -= pBox->BxBMargin;
          if (pParentAb == NULL)
            {
              /* It's the root box */
              inLine = FALSE;
              if (horizRef)
                {
                  if (pDimAb->DimValue == 0 && pDimAb->DimUnit != UnAuto)
                    /* inherited from the contents */
                    pBox->BxContentWidth = TRUE;
                  else
                    {
                      /* inherited from the window */
                      GetSizesFrame (frame, &val, &i);
                      if (pDimAb->DimValue < 0)
                        val += pDimAb->DimValue;
                      else if (pDimAb->DimUnit == UnPercent)
                        val = PixelValue (pDimAb->DimValue, UnPercent, (PtrAbstractBox) val, 0);
                      else if (pDimAb->DimUnit == UnAuto)
                        val = PixelValue (100, UnPercent, (PtrAbstractBox) val, 0);
                      else
                        val = PixelValue (pDimAb->DimValue, pDimAb->DimUnit, pAb,
                                          zoom);
                      if (pDimAb->DimValue < 0 || pDimAb->DimUnit == UnPercent ||
                          pDimAb->DimUnit == UnAuto)
                        /* the rule gives the outside value */
                        val = val + inx - dx;
                      ResizeWidth (pBox, pBox, NULL, val - pBox->BxW, 0, 0, 0,
                                   frame, FALSE);
                    }
                }
              else
                {
                  if (pDimAb->DimValue == 0 || pDimAb->DimUnit == UnAuto)
                    /* inherited from the contents */
                    pBox->BxContentHeight = TRUE;
                  else
                    {
                      /* inherited from the window */
                      GetSizesFrame (frame, &i, &val);
                      if (pDimAb->DimValue < 0)
                        val += pDimAb->DimValue;
                      else
                        /* explicit value */
                        val = GetPixelValue (pDimAb->DimValue, pDimAb->DimUnit, val, pAb,
                                             zoom);
                      if (pDimAb->DimValue < 0 || pDimAb->DimUnit == UnPercent)
                        /* the rule gives the outside value */
                        val = val + iny - dy;
                      ResizeHeight (pBox, pBox, NULL, val - pBox->BxH, 0, 0, frame);
                    }
                }
            }
          else
            {
              /* it's not the root box */
              inLine = (!pAb->AbNotInLine &&
                        (pAb->AbDisplay == 'I' || pAb->AbDisplay == 'b' ||
                         (pParentAb->AbBox &&
                          (pParentAb->AbBox->BxType == BoBlock ||
                           pParentAb->AbBox->BxType == BoFloatBlock ||
                           pParentAb->AbBox->BxType == BoCellBlock ||
                           pParentAb->AbBox->BxType == BoGhost ||
                           pParentAb->AbBox->BxType == BoStructGhost ||
                           pParentAb->AbBox->BxType == BoFloatGhost))));
              if (horizRef)
                {
                  if (pBox->BxType == BoCell)
                    {                      
                      /* get the related column */
                      if (pDimAb->DimAbRef)
                        pColumn = pDimAb->DimAbRef;
                      else
                        pColumn = pAb->AbHorizPos.PosAbRef;
                      if (pColumn)
                        InsertDimRelation (pColumn->AbBox, pBox, OpSame, TRUE, FALSE);
                      if (pDimAb->DimUnit == UnAuto)
                        {
                          pDimAb->DimAbRef = NULL;
                          pDimAb->DimValue = -1;
                        }
                    }
                  else
                    {
                      pColumn = NULL;
                      /* Detect a block included within another block
                         with auto and takes the width of the content */
                      if (pDimAb->DimUnit != UnAuto && inLine &&
                          pAb->AbFloat == 'N' &&
                          (pBox->BxType == BoGhost ||
                           pBox->BxType == BoStructGhost ||
                           pBox->BxType == BoFloatGhost) &&
                          pParentAb->AbWidth.DimAbRef)
                        pDimAb->DimUnit = UnAuto;
                      else if (pDimAb->DimUnit == UnAuto && pAb->AbFloat != 'N' &&
                               pParentAb->AbWidth.DimUnit != UnAuto &&
                               (pParentAb->AbWidth.DimAbRef != NULL ||
                                pParentAb->AbWidth.DimValue != -1))
                        {
                          // check if there is a previous or next element
                          pChildAb = pAb->AbNext;
                          while (pChildAb && pChildAb->AbPresentationBox)
                            pChildAb = pChildAb->AbNext;
                          if (pChildAb == NULL)
                            {
                              pChildAb = pAb->AbPrevious;
                              while (pChildAb && pChildAb->AbPresentationBox)
                                pChildAb = pChildAb->AbPrevious;
                              if (pChildAb == NULL)
                                {
                                  /* this floated box has no sibling boxes */
                                  pDimAb->DimAbRef = pParentAb;
                                  pDimAb->DimValue = 0;
                                  pDimAb->DimUnit = UnRelative;
                                  pBox->BxContentWidth = FALSE;
                                }
                            }
                        }

                      /* check how to manage auto */
                      if (pDimAb->DimUnit == UnAuto)
                        {
                          if (pAb->AbFloat != 'N'||
                              pBox->BxType == BoFloatGhost ||
                              pAb->AbNotInLine ||
                              pAb->AbDisplay == 'I' || pAb->AbDisplay == 'b')
                            {
                              /* floated box or inline -> content width */
                              pDimAb->DimAbRef = NULL;
                              pDimAb->DimValue = -1;
                              pBox->BxContentWidth = TRUE;
                            }
                          else if (pParentAb->AbFloat != 'N' &&
                                   pParentAb->AbWidth.DimUnit == UnAuto)
                            {
                              /* within a floated box -> content width */
                              pDimAb->DimAbRef = NULL;
                              pDimAb->DimValue = -1;		  
                              pBox->BxContentWidth = TRUE;
                            }
                          else if (dx > pParentAb->AbBox->BxW &&
                                   isExtraFlow)
                            {
                              /* cannot inherit from the enclosing */
                              pDimAb->DimAbRef = NULL;
                              pDimAb->DimValue = -1;		  
                              pBox->BxContentWidth = TRUE;
                            }
                          else if (!pParentAb->AbWidthChange &&
                                   pParentAb->AbBox->BxType != BoCell &&
                                   pParentAb->AbWidth.DimAbRef == NULL &&
                                   pParentAb->AbWidth.DimValue == -1)
                            {
                              /* parent inherits from contents */
                              pDimAb->DimAbRef = NULL;
                              pDimAb->DimValue = -1;		  
                              pBox->BxContentWidth = TRUE;
                            }
                          else if (pParentAb->AbWidthChange &&
                                   pParentAb->AbWidth.DimUnit == UnAuto)
                            {
                              /* parent rule will change check upper level */
                              PtrAbstractBox parent;
                              parent = pParentAb->AbEnclosing;
                              while (parent && parent->AbWidthChange &&
                                     parent->AbWidth.DimUnit == UnAuto &&
                                     parent->AbFloat == 'N')
                                parent = parent->AbEnclosing;
                              if (parent && parent->AbFloat != 'N')
                                {
                                  /* inherit from contents */
                                  pDimAb->DimAbRef = NULL;
                                  pDimAb->DimValue = -1;		  
                                  pBox->BxContentWidth = TRUE;
                               }
                              else if (parent && parent->AbBox &&
                                       parent->AbBox->BxType != BoCell &&
                                       ((parent->AbWidth.DimAbRef == NULL &&
                                         parent->AbWidth.DimValue == -1) ||
                                        (inLine &&
                                         pAb->AbDisplay != 'B' && pAb->AbDisplay != 'L')))
                                {
                                  /* inherit from contents */
                                  pDimAb->DimAbRef = NULL;
                                  pDimAb->DimValue = -1;		  
                                  pBox->BxContentWidth = TRUE;
                                }
                              else
                                {
                                  /* inherit from the parent box */
                                  pDimAb->DimAbRef = pAb->AbEnclosing;
                                  pDimAb->DimValue = 0;		  
                                  pBox->BxContentWidth = FALSE;
                                }
                            }
                          else if (pParentAb->AbInLine &&
                                   pParentAb->AbBox->BxType != BoCell &&
                                   pParentAb->AbWidth.DimAbRef &&
                                   pAb->AbDisplay != 'B' && pAb->AbDisplay != 'L')
                            {
                              /* inherit from contents */
                              pDimAb->DimAbRef = NULL;
                              pDimAb->DimValue = -1;		  
                              pBox->BxContentWidth = TRUE;
                            }
                          else
                            {
                              /* inherit from the parent box */
                              pDimAb->DimAbRef = pParentAb;
                              pDimAb->DimValue = 0;		  
                              pBox->BxContentWidth = FALSE;
                            }
                        }
                    }

                  if ((inLine && pAb->AbLeafType == LtText) ||
                      (pDimAb->DimAbRef == NULL && pDimAb->DimValue < 0))
                    /* inherited from the contents */
                    pBox->BxContentWidth = TRUE;
                  else if (pDimAb->DimAbRef == NULL ||
                           (pDimAb->DimAbRef == pParentAb &&
                            (pDimAb->DimUnit == UnPercent || pDimAb->DimUnit == UnAuto)))
                    {
                      /* percentage or explicit value */
                      if (pDimAb->DimUnit == UnPercent || pDimAb->DimUnit == UnAuto)
                        {
                          if (pBox->BxType != BoCell)
                            {
                              /* inherited from the parent */
                              if (pParentAb)
                                i = GetPercentDim (pAb, pParentAb, horizRef);
                              else
                                GetSizesFrame (frame, &i, &val);
                              if (pDimAb->DimUnit == UnPercent)
                                {
                                  val = PixelValue (pDimAb->DimValue, UnPercent,
                                                    (PtrAbstractBox) i, 0);
                                  if (pDimAb->DimValue == 100)
                                    /* the rule gives the outside value */
                                    val = val + inx - dx;
                                }
                              else /* UnAuto */
                                {
                                  val = PixelValue (100, UnPercent, 
                                                    (PtrAbstractBox) i, 0);
                                  /* the rule gives the outside value */
                                  val = val + inx - dx;
                                }
                              if (pParentAb)
                                {
                                  if (pDimAb->DimSameDimension)
                                    InsertDimRelation (pParentAb->AbBox, pBox,
                                                       OpSame, horizRef, inLine);
                                  else
                                    InsertDimRelation (pParentAb->AbBox, pBox,
                                                       OpReverse, horizRef, inLine);
                                }
                            }
                          else
                            {
                              val = PixelValue (pDimAb->DimValue, UnPercent,
                                                (PtrAbstractBox) pParentAb->AbBox->BxW, 0);
                              val = val + inx - dx;
                            }
                        }
                      else
                        {
                          /* explicit value */
                          val = PixelValue (pDimAb->DimValue, pDimAb->DimUnit, pAb,
                                            zoom);
                        }
                      ResizeWidth (pBox, pBox, NULL, val - pBox->BxW, 0, 0, 0,
                                   frame, FALSE);
                    }
                  else
                    {
                      pPosAb = &pAb->AbHorizPos;
                      if (pDimAb->DimAbRef == pParentAb &&
                          pDimAb->DimUnit != UnAuto &&
                          pParentAb->AbWidth.DimAbRef == NULL &&
                          pParentAb->AbWidth.DimValue <= 0 &&
                          (inLine || pPosAb->PosAbRef != pParentAb ||
                           pPosAb->PosRefEdge != Left ||
                           pPosAb->PosEdge != Left))
                        {
                          while (pParentAb &&
                                 ((pParentAb->AbWidth.DimAbRef == NULL &&
                                   pParentAb->AbWidth.DimValue <= 0) ||
                                  pParentAb->AbInLine ||
                                  pParentAb->AbBox->BxType == BoGhost ||
                                  pParentAb->AbBox->BxType == BoStructGhost ||
                                  pParentAb->AbBox->BxType == BoFloatGhost))
                            /* look for the right ancestor */
                            pParentAb = pParentAb->AbEnclosing;
                          if (pParentAb == NULL)
                            {
                              /* inherited from the contents */
                              pBox->BxContentWidth = TRUE;
                              pDimAb->DimAbRef = NULL;
                              pDimAb->DimValue = -1;
                              pDimAb->DimUnit = UnRelative;
                            }
                          else
                            pDimAb->DimAbRef = pParentAb;
                        }
                      else if (pDimAb->DimAbRef == pAb && pDimAb->DimSameDimension)
                        {
                          /* The dimension cannot depend on itself */
                          /* inherited from the contents */
                          pBox->BxContentWidth = TRUE;
                          pDimAb->DimAbRef = NULL;
                          pDimAb->DimValue = -1;
                          pDimAb->DimUnit = UnRelative;
                        }

                      if (pDimAb->DimAbRef)
                        {
                          /* Inherit from a box */
                          pRefBox = pDimAb->DimAbRef->AbBox;
                          if (pRefBox == NULL)
                            {
                              /* forwards reference */
                              pRefBox = GetBox (pDimAb->DimAbRef);
                              if (pRefBox != NULL)
                                pDimAb->DimAbRef->AbBox = pRefBox;
                            }
			    
                          if (pRefBox)
                            {
                              if (pDimAb->DimAbRef == pParentAb || isExtraFlow)
                                {
                                  /* inherited from the parent */
                                  /* same dimension? */
                                  if (pDimAb->DimSameDimension)
                                    val = pRefBox->BxW;
                                  else
                                    val = pRefBox->BxH;
				  
                                  if (inLine && pDimAb->DimSameDimension)
                                    {
                                      pLine = SearchLine (pBox, frame);
                                      if (pLine)
                                        val = pLine->LiXMax;
                                    }
                                  if (pos)
                                    {
                                      if (pos->PnLeftUnit != UnUndefined &&
                                          pos->PnLeftUnit != UnAuto)
                                        dx += GetPixelValue (pos->PnLeftDistance, pos->PnLeftUnit,
                                                             pRefBox->BxW, pAb, zoom);
                                      else if (pos->PnRightUnit != UnUndefined &&
                                               pos->PnRightUnit != UnAuto)
                                        dx += GetPixelValue (pos->PnRightDistance, pos->PnRightUnit,
                                                             pRefBox->BxW, pAb, zoom);
                                    }
                                }
                              else
                                {
                                  /* same dimension? */
                                  if (pDimAb->DimSameDimension)
                                    val = pRefBox->BxWidth;
                                  else
                                    val = pRefBox->BxHeight;
                                }

                              /* Convert the distance value */
                              if (pDimAb->DimUnit == UnPercent)
                                val = PixelValue (pDimAb->DimValue, UnPercent,
                                                  (PtrAbstractBox) val, 0);
                              else if (pDimAb->DimUnit == UnAuto)
                                val = PixelValue (100, UnPercent,
                                                  (PtrAbstractBox) val, 0);
                              else
                                val += PixelValue (pDimAb->DimValue, pDimAb->DimUnit, pAb,
                                                   zoom);
                              /* the rule gives the outside value */
                              if (val >= dx)
                                val = val - dx;
                              ResizeWidth (pBox, pBox, NULL, val - pBox->BxW, 0,
                                           0, 0, frame, FALSE);
                              /* Marks out of structure relations */
                              if (pDimAb->DimAbRef != pParentAb
                                  && pDimAb->DimAbRef->AbEnclosing != pParentAb)
                                pBox->BxWOutOfStruct = TRUE;
			      
                              /* Store dependencies */
                              if (pBox->BxType != BoCell)
                                {
                                  if (pDimAb->DimSameDimension)
                                    InsertDimRelation (pDimAb->DimAbRef->AbBox, pBox,
                                                       OpSame, horizRef, inLine);
                                  else
                                    InsertDimRelation (pDimAb->DimAbRef->AbBox, pBox,
                                                       OpReverse, horizRef, inLine);
                                }
                            }
                        }
                    }
                }
              else
                {
                  // height rule
                  pDimAb = &pAb->AbHeight;
                  if ((inLine && pAb->AbLeafType == LtText) ||
                      (pDimAb->DimAbRef == NULL && pDimAb->DimValue < 0))
                    /* inherited from the contents */
                    pBox->BxContentHeight = TRUE;
                  else if (pDimAb->DimAbRef == NULL ||
                           (pDimAb->DimAbRef == pParentAb &&
                            pDimAb->DimUnit == UnPercent))
                    {
                      if (pDimAb->DimUnit == UnPercent &&
                          (inLine ||
                           TypeHasException (ExcIsTable, pEl->ElTypeNumber,
                                             pEl->ElStructSchema) ||
                           TypeHasException (ExcIsRow, pEl->ElTypeNumber,
                                             pEl->ElStructSchema) ||
                           TypeHasException (ExcIsCell, pEl->ElTypeNumber,
                                             pEl->ElStructSchema)))
                        {
                          /* mismatch: inherited from the contents */
                          pBox->BxContentHeight = TRUE;
                          pDimAb->DimAbRef = NULL;
                          pDimAb->DimValue = -1;
                          pDimAb->DimUnit = UnRelative;
                          val = pBox->BxH;
                        }
                      else if (pDimAb->DimUnit == UnPercent)
                        {
                          while (pParentAb &&
                                 !pParentAb->AbHeight.DimIsPosition &&
                                 pParentAb->AbHeight.DimValue < 0 &&
                                 pParentAb->AbHeight.DimAbRef == NULL)
                            pParentAb = pParentAb->AbEnclosing;
                          /* inherited from the parent */
                          if (pParentAb)
                            {
                              i = GetPercentDim (pAb, pParentAb, horizRef);
                              val = PixelValue (pDimAb->DimValue, UnPercent, 
                                                (PtrAbstractBox) i, 0);
                            }
                          else
                            {
                              GetSizesFrame (frame, &val, &i);
                              val = PixelValue (pDimAb->DimValue, UnPercent, 
                                                (PtrAbstractBox) i, 0);
                            }
                          /* the rule gives the outside value */
                          //if (pAb->AbVertEnclosing)
                          if (val >= dy)
                            val = val - dy;
                          if (pParentAb)
                            {
                              if (pDimAb->DimSameDimension)
                                InsertDimRelation (pParentAb->AbBox, pBox,
                                                   OpSame, horizRef, inLine);
                              else
                                InsertDimRelation (pParentAb->AbBox, pBox,
                                                   OpReverse, horizRef, inLine);
                            }
                        }
                      else
                        /* explicit value */
                        val = PixelValue (pDimAb->DimValue, pDimAb->DimUnit, pAb, zoom);
                      ResizeHeight (pBox, pBox, NULL, val - pBox->BxH, 0, 0, frame);
                    }
                  else
                    {
                      pPosAb = &pAb->AbVertPos;
                      if (pDimAb->DimAbRef == pParentAb &&
                          pParentAb->AbEnclosing != NULL &&
                          /* parent depends on the content */
                          pParentAb->AbHeight.DimAbRef == NULL &&
                          pParentAb->AbHeight.DimValue <= 0 &&
                          (pPosAb->PosAbRef != pParentAb || pPosAb->PosRefEdge != Top
                           || pPosAb->PosEdge != Top))
                        {
                          /* mismatch: inherited from the contents */
                          pBox->BxContentHeight = TRUE;
                          pDimAb->DimAbRef = NULL;
                          pDimAb->DimValue = -1;
                          pDimAb->DimUnit = UnRelative;
                        }
                      else
                        {
                          if (pDimAb->DimAbRef == pAb && pDimAb->DimSameDimension)
                            {
                              /* The dimension cannot depend on itself */
                              /* inherited from the contents */
                              pBox->BxContentHeight = TRUE;
                              pDimAb->DimAbRef = NULL;
                              pDimAb->DimValue = -1;
                              pDimAb->DimUnit = UnRelative;
                            }
                          else
                            {
                              /* Inherit from a box */
                              pRefBox = pDimAb->DimAbRef->AbBox;
                              if (pRefBox == NULL)
                                {
                                  /* forwards reference */
                                  pRefBox = GetBox (pDimAb->DimAbRef);
                                  if (pRefBox != NULL)
                                    pDimAb->DimAbRef->AbBox = pRefBox;
                                }
			    
                              if (pRefBox != NULL)
                                {
                                  if (pDimAb->DimAbRef == pParentAb || isExtraFlow)
                                    {
                                      /* inherited from the parent */
                                      /* same dimension? */
                                      if (pDimAb->DimSameDimension)
                                        val = pRefBox->BxH;
                                      else
                                        val = pRefBox->BxW;
				
                                      if (inLine && !pDimAb->DimSameDimension)
                                        {
                                          /* remove the indentation value  */
                                          delta = GetPixelValue (pParentAb->AbIndent,
                                                                 pParentAb->AbIndentUnit,
                                                                 val, pParentAb, zoom);
                                          if (pParentAb->AbIndent > 0)
                                            val -= delta;
                                          else if (pParentAb->AbIndent < 0)
                                            val += delta;
                                        }
                                      if (pos)
                                        {
                                          if (pos->PnTopUnit != UnUndefined &&
                                              pos->PnTopUnit != UnAuto)
                                            dy += GetPixelValue (pos->PnTopDistance,
                                                                 pos->PnTopUnit,
                                                                 pRefBox->BxH, pAb, zoom);
                                          else if (pos->PnBottomUnit != UnUndefined &&
                                                   pos->PnBottomUnit != UnAuto)
                                            dy +=GetPixelValue (pos->PnBottomDistance,
                                                                pos->PnBottomUnit,
                                                                pRefBox->BxH, pAb, zoom);
                                        }
                                    }
                                  else
                                    {
                                      /* inherited from another box */
                                      /* same dimension? */
                                      if (pDimAb->DimSameDimension)
                                        val = pRefBox->BxHeight;
                                      else
                                        val = pRefBox->BxWidth;
                                    }
				
                                  val += GetPixelValue (pDimAb->DimValue, pDimAb->DimUnit,
                                                        val, pAb, zoom);
                                  if (pParentAb == NULL || pParentAb->AbBox == NULL ||
                                      pParentAb->AbBox->BxType != BoTable)
                                    /* the rule gives the outside value */
                                    val = val - dy;
                                  ResizeHeight (pBox, pBox, NULL, val - pBox->BxH, 0, 0, frame);
                                  /* Marks out of structure relations */
                                  if (pDimAb->DimAbRef != pParentAb
                                      && pDimAb->DimAbRef->AbEnclosing != pParentAb)
                                    pBox->BxHOutOfStruct = TRUE;
				  
                                  /* Store dependencies */
                                  if (pDimAb->DimAbRef == pAb &&
                                      pDimAb->DimSameDimension)
                                    {
                                      /* Check errors */
                                      if (horizRef)
                                        fprintf (stderr, "Bad HorizPos rule on %s\n", AbsBoxType (pAb, TRUE));
                                      else
                                        fprintf (stderr, "Bad VertPos rule on %s\n", AbsBoxType (pAb, TRUE));
                                    }
                                  if (pDimAb->DimSameDimension)
                                    InsertDimRelation (pDimAb->DimAbRef->AbBox, pBox,
                                                       OpSame, horizRef, inLine);
                                  else
                                    InsertDimRelation (pDimAb->DimAbRef->AbBox, pBox,
                                                       OpReverse, horizRef, inLine);
                                }
                            }
                        }
                    }
                }
            }
        }
      /* C'est une boite elastique */
      else if (horizRef)
        {
          pDimAb = &pAb->AbWidth;
          /* Box elastique en X */
          pPosAb = &pDimAb->DimPosition;
          op = OpWidth;
          /* On teste si la relation est hors structure */
          if (pPosAb->PosAbRef != pParentAb
              && pPosAb->PosAbRef->AbEnclosing != pParentAb)
            pBox->BxWOutOfStruct = TRUE;
          else if (pPosAb->PosAbRef->AbBox != NULL)
            pBox->BxWOutOfStruct = pPosAb->PosAbRef->AbBox->BxXOutOfStruct;
	  
          /* Des boites voisines heritent de la relation hors-structure ? */
          if (pParentAb)
            {
              pChildAb = pParentAb->AbFirstEnclosed;
              while (pChildAb)
                {
                  if (pChildAb != pAb && pChildAb->AbBox != NULL)
                    {
                      /* Si c'est un heritage on note l'indication hors-structure */
                      if (pChildAb->AbHorizPos.PosAbRef == pAb
                          && pChildAb->AbHorizPos.PosRefEdge != Left)
                        {
                          if (!IsXPosComplete (pChildAb->AbBox))
                            /* la boite  est maintenant placee en absolu */
                            pChildAb->AbBox->BxXToCompute = TRUE;
                          pChildAb->AbBox->BxXOutOfStruct = TRUE;
                          if (pChildAb->AbEnclosing == pAb->AbEnclosing)
                            pChildAb->AbHorizEnclosing = pAb->AbHorizEnclosing;
                          PropagateXOutOfStruct (pChildAb, frame, TRUE, pChildAb->AbHorizEnclosing);
                        }
                      if (pChildAb->AbVertPos.PosAbRef == pAb
                          && pChildAb->AbVertPos.PosRefEdge != Top
                          && pAb->AbLeafType == LtCompound
                          && pAb->AbInLine)
                        {
                          if (!IsYPosComplete (pChildAb->AbBox))
                            /* la boite  est maintenant placee en absolu */
                            pChildAb->AbBox->BxYToCompute = TRUE;
                          pChildAb->AbBox->BxYOutOfStruct = TRUE;
                          if (pChildAb->AbEnclosing == pAb->AbEnclosing)
                            pChildAb->AbVertEnclosing = pAb->AbVertEnclosing;
                          PropagateYOutOfStruct (pChildAb, frame, TRUE, pChildAb->AbVertEnclosing);
                        }
                    }
                  pChildAb = pChildAb->AbNext;
                }
            }

          /* Decalage par rapport a la boite distante */
          pRefBox = pPosAb->PosAbRef->AbBox;
          if (pRefBox == NULL)
            {
              /* On doit resoudre une reference en avant */
              if (!IsDead (pPosAb->PosAbRef))
                pRefBox = GetBox (pPosAb->PosAbRef);
              if (pRefBox != NULL)
                pPosAb->PosAbRef->AbBox = pRefBox;
            }

          if (pRefBox)
            {
              /* regarde si la position depend d'une boite invisible */
              if (pPosAb->PosAbRef->AbVisibility < ViewFrameTable[frame - 1].FrVisibility)
                delta = 0;
              else
                {
                  /* Convert the distance value */
                  if (pAb->AbEnclosing == NULL)
                    dim = 0;
                  else
                    dim = pAb->AbEnclosing->AbBox->BxW;
                  delta = GetPixelValue (pPosAb->PosDistance, pPosAb->PosUnit, dim,
                                         pAb, zoom);
                  delta += GetPixelValue (pPosAb->PosDistDelta, pPosAb->PosDeltaUnit, dim,
                                          pAb, zoom);
                }
              val = pRefBox->BxXOrg + delta;
              if (pRefBox->BxAbstractBox == pParentAb)
                val += pRefBox->BxLMargin + pRefBox->BxLBorder + pRefBox->BxLPadding;
              switch (pPosAb->PosRefEdge)
                {
                case Left:
                  break;
                case Right:
                  if (pRefBox->BxAbstractBox == pParentAb)
                    val += pRefBox->BxW;
                  else
                    val += pRefBox->BxWidth;
                  break;
                case VertMiddle:
                  if (pRefBox->BxAbstractBox == pParentAb)
                    val += pRefBox->BxW / 2;
                  else
                    val += pRefBox->BxWidth / 2;
                  break;
                case VertRef:
                  val += pRefBox->BxVertRef;
                  break;
                default:
                  break;
                }

              /* Compute the box width */
              val = val - pBox->BxXOrg - pBox->BxWidth;
              /* La boite n'a pas de point fixe */
              pBox->BxHorizEdge = NoEdge;
              InsertPosRelation (pBox, pRefBox, op, pPosAb->PosEdge, pPosAb->PosRefEdge);
	      
              if (!IsXPosComplete (pBox))
                /* la boite  devient maintenant placee en absolu */
                pBox->BxXToCompute = TRUE;
              /* La boite est marquee elastique */
              pBox->BxHorizFlex = pRefBox;
              pRefBox->BxMoved = NULL;
              MoveBoxEdge (pBox, pRefBox, op, val, frame, TRUE);
            }
        }
      else
        {
          pDimAb = &pAb->AbHeight;
          /* Box elastique en Y */
          pPosAb = &pDimAb->DimPosition;
          op = OpHeight;	  
          /* On teste si la relation est hors structure */
          if (pPosAb->PosAbRef != pParentAb
              && pPosAb->PosAbRef->AbEnclosing != pParentAb)
            pBox->BxHOutOfStruct = TRUE;
          else if (pPosAb->PosAbRef->AbBox != NULL)
            pBox->BxHOutOfStruct = pPosAb->PosAbRef->AbBox->BxYOutOfStruct;
	  
          /* Des boites voisines heritent de la relation hors-structure ? */
          if (pParentAb != NULL /* && pBox->BxHOutOfStruct */ )
            {
              pChildAb = pParentAb->AbFirstEnclosed;
              while (pChildAb != NULL)
                {
                  if (pChildAb != pAb && pChildAb->AbBox != NULL)
                    {
                      /* Si c'est un heritage on note l'indication hors-structure */
                      if (pChildAb->AbVertPos.PosAbRef == pAb
                          && pChildAb->AbVertPos.PosRefEdge != Top)
                        {
                          if (!IsYPosComplete (pChildAb->AbBox))
                            /* la boite  est maintenant placee en absolu */
                            pChildAb->AbBox->BxYToCompute = TRUE;
                          pChildAb->AbBox->BxYOutOfStruct = TRUE;
                          if (pChildAb->AbEnclosing == pAb->AbEnclosing)
                            pChildAb->AbVertEnclosing = pAb->AbVertEnclosing;
                          PropagateYOutOfStruct (pChildAb, frame, TRUE, pChildAb->AbVertEnclosing);
                        }
                    }
                  pChildAb = pChildAb->AbNext;
                }
            }

          /* Decalage par rapport a la boite distante */
          pRefBox = pPosAb->PosAbRef->AbBox;
          if (pRefBox == NULL)
            {
              /* On doit resoudre une reference en avant */
              pRefBox = GetBox (pPosAb->PosAbRef);
              if (pRefBox != NULL)
                pPosAb->PosAbRef->AbBox = pRefBox;
            }

          if (pRefBox != NULL)
            {
              /* regarde si la position depend d'une boite invisible */
              if (pPosAb->PosAbRef->AbVisibility < ViewFrameTable[frame - 1].FrVisibility)
                delta = 0;
              else
                {
                  /* Convert the distance value */
                  if (pAb->AbEnclosing == NULL)
                    dim = 0;
                  else
                    dim = pAb->AbEnclosing->AbBox->BxH;
                  delta = GetPixelValue (pPosAb->PosDistance, pPosAb->PosUnit, dim,
                                         pAb, zoom);
                  delta += GetPixelValue (pPosAb->PosDistDelta, pPosAb->PosDeltaUnit, dim,
                                          pAb, zoom);
                }
              val = pRefBox->BxYOrg + delta;
              if (pRefBox->BxAbstractBox == pParentAb)
                val += pRefBox->BxTMargin + pRefBox->BxTBorder + pRefBox->BxTPadding;
              switch (pPosAb->PosRefEdge)
                {
                case Bottom:
                  if (pRefBox->BxAbstractBox == pParentAb)
                    val += pRefBox->BxH;
                  else
                    val += pRefBox->BxHeight;
                  break;
                case HorizMiddle:
                  if (pRefBox->BxAbstractBox == pParentAb)
                    val += pRefBox->BxH / 2;
                  else
                    val += pRefBox->BxHeight / 2;
                  break;
                case HorizRef:
                  val += pRefBox->BxHorizRef;
                  break;
                default:
                  break;
                }

              /* Compute the boxe height */
              val = val - pBox->BxYOrg - pBox->BxHeight;
              /* La boite n'a pas de point fixe */
              pBox->BxVertEdge = NoEdge;
              InsertPosRelation (pBox, pRefBox, op, pPosAb->PosEdge, pPosAb->PosRefEdge);

              if (!IsYPosComplete (pBox))
                /* la boite  devient maintenant placee en absolu */
                pBox->BxYToCompute = TRUE;
              /* La boite est marquee elastique */
              pBox->BxVertFlex = pRefBox;
              ClearBoxMoved (pBox);
              MoveBoxEdge (pBox, pRefBox, op, val, frame, FALSE);
            }
        }
    }

  /* La regle de dimension est interpretee */
  if (horizRef)
    {
      pAb->AbWidthChange = FALSE;
      /* Marque dans la boite si la dimension depend du contenu ou non */
      defaultDim = pBox->BxContentWidth;
      if (defaultDim)
        {
          pBox->BxRuleWidth = 0;
          pBox->BxShrink = FALSE;
        }
    }
  else
    {
      pAb->AbHeightChange = FALSE;
      /* Marque dans la boite si la dimension depend du contenu ou non */
      pBox->BxRuleHeight = 0;
      defaultDim = pBox->BxContentHeight;
    }

  /* break down the temporary link of moved boxes */
  ClearBoxMoved (pBox);
  return (defaultDim);
}


/*----------------------------------------------------------------------
  ComputeAxisRelation applique la regle de positionnement donnee en 
  parametre a` la boite d'indice pBox. L'axe              
  horizontal ou vertical de la boite, selon que horizRef  
  est VRAI ou FAUX est mis a` jour.                       
  ----------------------------------------------------------------------*/
void ComputeAxisRelation (AbPosition rule, PtrBox pBox, int frame, ThotBool horizRef)
{
  PtrBox              pRefBox;
  PtrAbstractBox      pRefAb, pAb;
  BoxEdge             refEdge, localEdge;
  int                 x, y, dist;
  int                 t, b, l, r, dim;

  pRefAb = rule.PosAbRef;
  pAb = pBox->BxAbstractBox;
  /* don't manage invisible boxes */
  if (pAb->AbVisibility < ViewFrameTable[frame - 1].FrVisibility)
    return;
 
  /* Verifie que la position ne depend pas d'un pave mort */
  if (pRefAb &&
      (IsDead (pRefAb) ||
       pRefAb->AbVisibility < ViewFrameTable[frame - 1].FrVisibility))
    pRefAb = NULL;

  if (pRefAb == NULL)
    {
      /* default rule */
      pRefBox = pBox;
      if (horizRef)
        {
          refEdge = Left;
          localEdge = VertRef;
          if (rule.PosUnit == UnPercent)
            dist = PixelValue (rule.PosDistance, UnPercent,
                               (PtrAbstractBox) (pBox->BxW), 0);
          else
            dist = 0;
          if (rule.PosDeltaUnit == UnPercent)
            dist += PixelValue (rule.PosDistDelta, UnPercent,
                                (PtrAbstractBox) (pBox->BxW), 0);
        }
      else
        {
          refEdge = HorizRef;
          localEdge = HorizRef;
         if (rule.PosUnit == UnPercent)
            dist = PixelValue (rule.PosDistance, UnPercent,
                               (PtrAbstractBox) (pBox->BxH), 0);
          else
            dist = BoxFontBase (pBox->BxFont);
          if (rule.PosDeltaUnit == UnPercent)
            dist += PixelValue (rule.PosDistDelta, UnPercent,
                                (PtrAbstractBox) (pBox->BxH), 0);
        }
    }
  else
    {
      /* explicit rule */
      refEdge = rule.PosRefEdge;
      localEdge = rule.PosEdge;
      /* Convert the distance value */
      dim = 0;
      if (pAb->AbEnclosing && pAb->AbEnclosing->AbBox)
        {
          if (horizRef)
            dim = pAb->AbEnclosing->AbBox->BxW;
          else
            dim = pAb->AbEnclosing->AbBox->BxH;
        }
      dist = GetPixelValue (rule.PosDistance, rule.PosUnit, dim, pAb,
                            ViewFrameTable[frame - 1].FrMagnification);
      dist += GetPixelValue (rule.PosDistDelta, rule.PosDeltaUnit, dim, pAb,
                             ViewFrameTable[frame - 1].FrMagnification);

      pRefBox = pRefAb->AbBox;
      if (pRefBox == NULL)
        {
          /* On doit resoudre une reference en avant */
          pRefBox = GetBox (pRefAb);
          if (pRefBox)
            pRefAb->AbBox = pRefBox;
          else
            {
              if (horizRef)
                pAb->AbVertRefChange = FALSE;
              else
                pAb->AbHorizRefChange = FALSE;
              return;	/* plus de boite libre */
            }
        }
    }

  /* Deplacement par rapport a la boite distante */
  /* L'axe est place par rapport a la boite elle-meme */
  if (pRefBox == pBox ||
      pBox->BxType == BoGhost ||
      pBox->BxType == BoStructGhost ||
      pBox->BxType == BoFloatGhost ||
      pBox->BxType == BoBlock ||
      pBox->BxType == BoFloatBlock ||
      pBox->BxType == BoCellBlock)
    {
      GetExtraMargins (pBox, frame, FALSE, &t, &b, &l, &r);
      x = pBox->BxLMargin + pBox->BxLBorder + pBox->BxLPadding + l;
      y = pBox->BxTMargin + pBox->BxTBorder + pBox->BxTPadding + t;
    }
  /* L'axe est place par rapport a une incluse */
  else if (Propagate != ToSiblings)
    {
      /* Il faut peut-etre envisager que pRefBox soit une boite coupee */
      x = pRefBox->BxXOrg - pBox->BxXOrg;
      y = pRefBox->BxYOrg - pBox->BxYOrg;
    }
  else
    {
      x = pRefBox->BxXOrg;
      y = pRefBox->BxYOrg;
    }

  switch (refEdge)
    {
    case Left:
    case Top:
      break;
    case Bottom:
      y += pRefBox->BxH;
      break;
    case Right:
      x += pRefBox->BxW;
      break;
    case HorizRef:
      if (pRefBox != pBox)
        y += pRefBox->BxHorizRef;
      break;
    case VertRef:
      if (pRefBox != pBox)
        x += pRefBox->BxVertRef;
      break;
    case HorizMiddle:
      y += pRefBox->BxHeight / 2;
      break;
    case VertMiddle:
      x += pRefBox->BxWidth / 2;
      break;
    case NoEdge:
      break;
    }

  /* Met a jour l'axe de la boite */
  if (horizRef)
    {
      x = x + dist - pBox->BxVertRef;
      MoveVertRef (pBox, NULL, x, frame);
      /* la regle axe de reference est interpretee */
      pAb->AbVertRefChange = FALSE;
      if (pRefBox != NULL)
        InsertPosRelation (pBox, pRefBox, OpHorizRef, localEdge, refEdge);
    }
  else
    {
      y = y + dist - pBox->BxHorizRef;
      MoveHorizRef (pBox, NULL, y, frame);
      /* la regle axe de reference est interpretee */
      pAb->AbHorizRefChange = FALSE;
      if (pRefBox != NULL)
        InsertPosRelation (pBox, pRefBox, OpVertRef, localEdge, refEdge);
    }
  
  /* break down the temporary link of moved boxes */
  ClearBoxMoved (pBox);
}


/*----------------------------------------------------------------------
  RemovePosRelation removes the typed link that exists from the
  pOrginBox to the pTargetBox.
  The type could be horizontal or vertical position or axis.
  The parameter pCurrentAb, when not NULL, points to the abstract box
  of which the position rule is removed and it used to avoid ambiguity
  for bidirectional relations.
  A relation associated to a position rule that refers pCurrentAb must
  not be removed.
  When pCurrentAb is NULL, we're removing a reverse link.
  ----------------------------------------------------------------------*/
static ThotBool RemovePosRelation (PtrBox pOrginBox, PtrBox pTargetBox,
                                   PtrAbstractBox pCurrentAb, ThotBool Pos,
                                   ThotBool Axe, ThotBool horizRef)
{
  PtrPosRelations     pPreviousPosRel;
  PtrPosRelations     precpos;
  PtrPosRelations     pPosRel;
  PtrAbstractBox      pAb;
  BoxRelation        *pRelation;
  int                 i, found;
  ThotBool            loop;
  ThotBool            notEmpty;
  
  /* Look for the removed entry and the last used entry */
  found = 0;
  i = 0;
  pPreviousPosRel = NULL;
  pOrginBox->BxMoved = NULL;
  pPosRel = pOrginBox->BxPosRelations;
  precpos = NULL;
  loop = TRUE;
  if (pPosRel != NULL)
    while (loop)
      {
        i = 1;
        notEmpty = pPosRel->PosRTable[i - 1].ReBox != NULL;
        while (i <= MAX_RELAT_POS && notEmpty)
          {
            pRelation = &pPosRel->PosRTable[i - 1];
            if (horizRef)
              {
                /* horizontal relation */
                if (pRelation->ReBox == pTargetBox &&
                    ((Axe && pRelation->ReOp == OpHorizRef) ||
                     (Pos && pRelation->ReOp == OpHorizInc) ||
                     (Pos && pCurrentAb == NULL && pRelation->ReOp == OpHorizDep) ||
                     /* stretched dimension */
                     (!Pos && !Axe && pRelation->ReOp == OpWidth)))
                  {
                    /* the relation is found */
                    found = i;
                    pPreviousPosRel = pPosRel;
                  }
                else if (found == 0 && Pos && pRelation->ReOp == OpHorizDep &&
                         pCurrentAb != NULL && pRelation->ReBox == pTargetBox)
                  {
		    
                    /* it's the position relation of pCurrentAb */
                    pAb = pRelation->ReBox->BxAbstractBox;
                    pAb = pAb->AbHorizPos.PosAbRef;
		    
                    if (pAb == NULL)
                      /* a rule NIL, get the associated abstract box */
                      pAb = GetPosRelativeAb (pRelation->ReBox->BxAbstractBox, horizRef);
                    if (pAb != pCurrentAb)
                      {
                        found = i;
                        pPreviousPosRel = pPosRel;
                      }
                  }
              }
            else
              {
                /* vertical relation */
                if (pRelation->ReBox == pTargetBox &&
                    ((Axe && pRelation->ReOp == OpVertRef) ||
                     (Pos && pRelation->ReOp == OpVertInc) ||
                     (Pos && pCurrentAb == NULL && pRelation->ReOp == OpVertDep) ||
                     /* stretched dimension */
                     (!Pos && !Axe && pRelation->ReOp == OpHeight)))
                  {
                    /* the relation is found */
                    found = i;
                    pPreviousPosRel = pPosRel;
                  }
                else if (found == 0 && Pos && pRelation->ReOp == OpVertDep &&
                         pCurrentAb != NULL && pRelation->ReBox == pTargetBox)
                  {
                    /* it's the position relation of pCurrentAb */
                    pAb = pRelation->ReBox->BxAbstractBox;
                    pAb = pAb->AbVertPos.PosAbRef;
                    if (pAb == NULL)
                      /* a rule NIL, get the associated abstract box */
                      pAb = GetPosRelativeAb (pRelation->ReBox->BxAbstractBox, horizRef);
                    if (pAb != pCurrentAb)
                      {
                        found = i;
                        pPreviousPosRel = pPosRel;
                      }
                  }
              }
            i++;
            if (i <= MAX_RELAT_POS)
              notEmpty = pPosRel->PosRTable[i - 1].ReBox != NULL;
          }
	
        if (pPosRel->PosRNext == NULL)
          loop = FALSE;
        else
          {
            precpos = pPosRel;
            /* Next block */
            pPosRel = pPosRel->PosRNext;
          }
      }

  if (found > 0)
    {
      /* reorganize the list */
      pRelation = &pPreviousPosRel->PosRTable[found - 1];      
      if (pCurrentAb &&
          (pRelation->ReOp == OpHorizDep || pRelation->ReOp == OpVertDep))
        /* Remove the reverse relation */
        loop = RemovePosRelation (pRelation->ReBox, pOrginBox, NULL, Pos, Axe, horizRef);
      
      if (i > 1)
        {
          i--;
          pRelation->ReBox = pPosRel->PosRTable[i - 1].ReBox;
          pRelation->ReRefEdge = pPosRel->PosRTable[i - 1].ReRefEdge;
          pRelation->ReOp = pPosRel->PosRTable[i - 1].ReOp;
        }
      
      /* free a block if necessary */
      if (i == 1)
        {
          if (precpos == NULL)
            pOrginBox->BxPosRelations = NULL;
          else
            precpos->PosRNext = NULL;
          FreePosBlock (&pPosRel);
        }
      else
        pPosRel->PosRTable[i - 1].ReBox = NULL;
      return TRUE;
    }
  else
    return FALSE;
}


/*----------------------------------------------------------------------
  RemoveDimRelation removes if it exists the horizontal or vertical link
  from pOrginBox to pTargetBox and compact the list.
  Return TRUE if the operation is done.
  ----------------------------------------------------------------------*/
static ThotBool RemoveDimRelation (PtrBox pOrginBox, PtrBox pTargetBox,
                                   ThotBool horizRef)
{
  PtrDimRelations     pFoundDimRel;
  PtrDimRelations     pPreviousDimRel;
  PtrDimRelations     pDimRel;
  int                 i, found;
  ThotBool            loop;
  ThotBool            notEmpty;

  i = 0;
  /* look for the removed entry and the last entry */
  found = 0;
  pFoundDimRel = NULL;
  if (horizRef)
    pDimRel = pOrginBox->BxWidthRelations;
  else
    pDimRel = pOrginBox->BxHeightRelations;
  pPreviousDimRel = NULL;
  loop = TRUE;
  if (pDimRel != NULL)
    while (loop)
      {
        i = 1;
        notEmpty = pDimRel->DimRTable[i - 1] != NULL;
        while (i <= MAX_RELAT_DIM && notEmpty)
          {
            /* Is it the right entry? */
            if (pDimRel->DimRTable[i - 1] == pTargetBox)
              {
                found = i;
                pFoundDimRel = pDimRel;
              }
            i++;
            if (i <= MAX_RELAT_DIM)
              notEmpty = pDimRel->DimRTable[i - 1] != NULL;
          }
	
        if (pDimRel->DimRNext == NULL || found)
          loop = FALSE;
        else
          {
            pPreviousDimRel = pDimRel;
            /* next block */
            pDimRel = pDimRel->DimRNext;
          }
      }
  
  /* Relation found, compact the list */
  if (found > 0)
    {
      i--;
      pFoundDimRel->DimRTable[found - 1] = pDimRel->DimRTable[i - 1];
      pFoundDimRel->DimROp[found - 1] = pDimRel->DimROp[i - 1];
      
      /* Is the block now empty? */
      if (i == 1)
        {
          if (pPreviousDimRel == NULL)
            if (horizRef)
              pOrginBox->BxWidthRelations = pDimRel->DimRNext;
            else
              pOrginBox->BxHeightRelations = pDimRel->DimRNext;
          else
            pPreviousDimRel->DimRNext = pDimRel->DimRNext;
          FreeDimBlock (&pDimRel);
        }
      else
        pDimRel->DimRTable[i - 1] = NULL;
      return TRUE;
    }
  else
    /* Not found */
    return FALSE;
}


/*----------------------------------------------------------------------
  CheckDimRelation look for the box that contraints its dimension
  and if found removes it.
  The parameter horizRef is TRUE when it concerns the width, FALSE
  when it concerns the height.
  Return TRUE if there is a registered relation.
  ----------------------------------------------------------------------*/
static ThotBool CheckDimRelation (PtrBox pBox, ThotBool horizRef)
{
  PtrDimRelations     pFoundDimRel;
  PtrDimRelations     pPreviousDimRel;
  PtrDimRelations     pDimRel;
  int                 i, found;
  ThotBool            loop;
  ThotBool            notEmpty;

  i = 0;
  /* look for the removed entry and the last entry */
  found = 0;
  pFoundDimRel = NULL;
  if (horizRef)
    pDimRel = pBox->BxWidthRelations;
  else
    pDimRel = pBox->BxHeightRelations;
  pPreviousDimRel = NULL;
  loop = TRUE;
  if (pDimRel)
    while (loop)
      {
        i = 1;
        notEmpty = pDimRel->DimRTable[i - 1] != NULL;
        while (i <= MAX_RELAT_DIM && notEmpty)
          {
            /* Is it the right entry? */
            if (pDimRel->DimROp[i - 1] == OpIgnore)
              {
                found = i;
                pFoundDimRel = pDimRel;
              }
            i++;
            if (i <= MAX_RELAT_DIM)
              notEmpty = pDimRel->DimRTable[i - 1] != NULL;
          }
	
        if (pDimRel->DimRNext == NULL || found)
          loop = FALSE;
        else
          {
            pPreviousDimRel = pDimRel;
            /* next block */
            pDimRel = pDimRel->DimRNext;
          }
      }
  
  /* Relation found, compact the list */
  if (found > 0)
    {
      if (horizRef)
        pBox->BxWOutOfStruct = FALSE;
      else
        pBox->BxHOutOfStruct = FALSE;
      /* remove the relation */
      if (pFoundDimRel->DimRTable[found - 1] == pBox)
        /* change the horiz ref */
        RemoveDimRelation (pFoundDimRel->DimRTable[found - 1],
                           pBox, !horizRef);
      else
        RemoveDimRelation (pFoundDimRel->DimRTable[found - 1],
                           pBox, horizRef);
      /* clean up the reverse info */
      i--;
      pFoundDimRel->DimRTable[found - 1] = pDimRel->DimRTable[i - 1];
      pFoundDimRel->DimROp[found - 1] = pDimRel->DimROp[i - 1];
      /* Is the block now empty? */
      if (i == 1)
        {
          if (pPreviousDimRel == NULL)
            if (horizRef)
              pBox->BxWidthRelations = pDimRel->DimRNext;
            else
              pBox->BxHeightRelations = pDimRel->DimRNext;
          else
            pPreviousDimRel->DimRNext = pDimRel->DimRNext;
          FreeDimBlock (&pDimRel);
        }
      else
        pDimRel->DimRTable[i - 1] = NULL;
      return TRUE;
    }
  else
    return FALSE;
}


/*----------------------------------------------------------------------
  ClearOutOfStructRelation removes out of structure relations of the box
  pBox.
  ----------------------------------------------------------------------*/
void ClearOutOfStructRelation (PtrBox pBox, int frame)
{
  PtrBox              pOrginBox;
  PtrAbstractBox      pAb;

  pAb = pBox->BxAbstractBox;
  /* Remove horizontal out of structure relations */
  if (pBox->BxXOutOfStruct)
    {
      if (pAb->AbHorizPos.PosAbRef == NULL)
        pOrginBox = NULL;
      else
        pOrginBox = pAb->AbHorizPos.PosAbRef->AbBox;
      if (pOrginBox)
        RemovePosRelation (pOrginBox, pBox, NULL, TRUE, FALSE, TRUE);
      /* Clean up out of structure info */
      PropagateXOutOfStruct (pAb, frame, FALSE, pAb->AbHorizEnclosing);
    }
  
  /* Remove vertical out of structure relations */
  if (pBox->BxYOutOfStruct)
    {
      if (pAb->AbVertPos.PosAbRef == NULL)
        pOrginBox = NULL;
      else
        pOrginBox = pAb->AbVertPos.PosAbRef->AbBox;
      if (pOrginBox)
        RemovePosRelation (pOrginBox, pBox, NULL, TRUE, FALSE, FALSE);
      /* Clean up out of structure info */
      PropagateYOutOfStruct (pAb, frame, FALSE, pAb->AbVertEnclosing);
    }

  /* Remove out of structure width constraints */
  pOrginBox = NULL;
  if (pBox->BxWOutOfStruct)
    {
      /* Streched box? */
      if (pBox->BxHorizFlex)
        {
          pOrginBox = pBox->BxHorizFlex;
          RemovePosRelation (pOrginBox, pBox, NULL, FALSE, FALSE, TRUE);
          // reset flags
          pBox->BxHorizEdge = pAb->AbHorizPos.PosEdge;
          pBox->BxHorizInverted = FALSE;
          pBox->BxHorizFlex = NULL;
        }
      else
        CheckDimRelation (pBox, TRUE);
      pBox->BxWOutOfStruct = FALSE;
    }

  /* Remove out of structure height constraints */
  pOrginBox = NULL;
  if (pBox->BxHOutOfStruct)
    {
      /* Streched box? */
      if (pBox->BxVertFlex)
        {
          pOrginBox = pBox->BxVertFlex;
          RemovePosRelation (pOrginBox, pBox, NULL, FALSE, FALSE, FALSE);
          // reset flags
          pBox->BxVertEdge = pAb->AbVertPos.PosEdge;
          pBox->BxVertInverted = FALSE;
          pBox->BxVertFlex = NULL;
        }
      else
        CheckDimRelation (pBox, FALSE);
      pBox->BxHOutOfStruct = FALSE;
    }
}


/*----------------------------------------------------------------------
  ClearPosRelation looks for horizontal or vertical position relations
  of the box.
  - It could depend on the enclosing box (OpHorizInc or OpVertInc).
  - It could depend on a sibling box (2 OpHorizDep or OpVertDep).
  - It could depend on another box (2 OpHorizDep + BtXOutOfStruct
  or OpVertDep + BtXOutOfStruct).
  ----------------------------------------------------------------------*/
void ClearPosRelation (PtrBox pBox, ThotBool horizRef)
{
  PtrAbstractBox      pRefAb;
  PtrAbstractBox      pCurrentAb;
  ThotBool            loop;

  pRefAb = pBox->BxAbstractBox;
  loop = TRUE;
  /* Process differently when the box is linked with a OutOfStruct box? */
  if ((horizRef && pBox->BxXOutOfStruct) || (!horizRef && pBox->BxYOutOfStruct))
    {
      /* ClearOutOfStructRelation removes out of structure relations */
      if (!IsDead (pRefAb))
        {
          /* go up to the root abstract box */
          pCurrentAb = pRefAb;
          /* La relation hors-structure peut etre heritee d'une boite voisine */
          while (pCurrentAb->AbEnclosing)
            pCurrentAb = pCurrentAb->AbEnclosing;
	  
          /* look for a relation with this out of structure relation */
          while (loop && pCurrentAb)
            {
              if (pCurrentAb->AbBox)
                loop = !RemovePosRelation (pBox, pCurrentAb->AbBox, pRefAb, TRUE, FALSE, horizRef);
              pCurrentAb = NextAbToCheck (pCurrentAb, pRefAb);
            }
	  
          /* this out of structure relation is removed */
          if (horizRef)
            {
              pBox->BxXOutOfStruct = FALSE;
              /* sibling boxes inherited this out of structure relation? */
              pCurrentAb = pRefAb->AbEnclosing;
              if (pCurrentAb)
                {
                  /* check the parent and siblings */
                  pCurrentAb = pCurrentAb->AbFirstEnclosed;
                  while (pCurrentAb)
                    {
                      if (pCurrentAb != pRefAb && pCurrentAb->AbBox)
                        {
                          /* Si c'est un heritage on retire l'indication
                             hors-structure */
                          if (pCurrentAb->AbBox->BxXOutOfStruct
                              && pCurrentAb->AbHorizPos.PosAbRef == pRefAb)
                            pCurrentAb->AbBox->BxXOutOfStruct = FALSE;
                          else if (pCurrentAb->AbBox->BxWOutOfStruct &&
                                   pCurrentAb->AbWidth.DimIsPosition &&
                                   pCurrentAb->AbWidth.DimPosition.PosAbRef == pRefAb)
                            pCurrentAb->AbBox->BxWOutOfStruct = FALSE;
                        }
                      pCurrentAb = pCurrentAb->AbNext;
                    }
                }
            }
          else
            {
              pBox->BxYOutOfStruct = FALSE;
              /* Des boites voisines ont herite de la relation
                 hors-structure ? */
              pCurrentAb = pRefAb->AbEnclosing;
              if (pCurrentAb != NULL)
                {
                  pCurrentAb = pCurrentAb->AbFirstEnclosed;
                  while (pCurrentAb)
                    {
                      if (pCurrentAb != pRefAb && pCurrentAb->AbBox)
                        {
                          /* Si c'est un heritage on retire l'indication
                             hors-structure */
                          if (pCurrentAb->AbBox->BxYOutOfStruct
                              && pCurrentAb->AbVertPos.PosAbRef == pRefAb)
                            pCurrentAb->AbBox->BxYOutOfStruct = FALSE;
                          else if (pCurrentAb->AbBox->BxHOutOfStruct &&
                                   pCurrentAb->AbHeight.DimIsPosition &&
                                   pCurrentAb->AbHeight.DimPosition.PosAbRef == pRefAb)
                            pCurrentAb->AbBox->BxHOutOfStruct = FALSE;
                        }
                      pCurrentAb = pCurrentAb->AbNext;
                    }
                }
            }
        }
    }

  if (loop)
    {
      /* check local relations? */
      pCurrentAb = pRefAb->AbEnclosing;
      /* check a relation with the parent box or a sibling box */
      while (loop && pCurrentAb)
        {
          if (pCurrentAb->AbBox)
            loop = !RemovePosRelation (pBox, pCurrentAb->AbBox, pRefAb, TRUE, FALSE, horizRef);
          if (pCurrentAb == pRefAb->AbEnclosing)
            pCurrentAb = pCurrentAb->AbFirstEnclosed;
          else
            pCurrentAb = pCurrentAb->AbNext;
        }
    }
}

/*----------------------------------------------------------------------
  ClearAxisRelation recherche la boite dont depend l'axe de reference
  horizontal (si horizRef est Vrai) sinon vertical de     
  pBox:					
  - Il peut dependre d'elle meme (une relation chez elle).
  - Il peut dependre d'une englobee (une relation chez    
  l'englobee).                                            
  - Il peut dependre d'une voisine (une relation chez la  
  voisine).                                               
  Si cette dependance existe encore, on detruit la        
  relation.                                               
  ----------------------------------------------------------------------*/
void ClearAxisRelation (PtrBox pBox, ThotBool horizRef)
{
  PtrAbstractBox      pAb;
  PtrAbstractBox      pRefAb;
  ThotBool            loop;
  
  pRefAb = pBox->BxAbstractBox;
  loop = TRUE;
  /* On recherche dans la descendance la dependance de l'axe de reference */
  pAb = pRefAb;
  while (loop && pAb)
    {
      if (pAb->AbBox)
        loop = !RemovePosRelation (pAb->AbBox, pBox, NULL, FALSE, TRUE, horizRef);
      if (pAb == pRefAb)
        pAb = pAb->AbFirstEnclosed;
      else
        pAb = pAb->AbNext;
    }
  
  /* On recherche chez les voisines la dependance de l'axe de reference */
  pAb = pRefAb->AbEnclosing;
  if (pAb)
    {
      pAb = pAb->AbFirstEnclosed;
      while (loop && pAb)
        {
          if (pAb != pRefAb && pAb->AbBox)
            loop = !RemovePosRelation (pAb->AbBox, pBox, NULL, FALSE, TRUE, horizRef);
          pAb = pAb->AbNext;
        }
    }
}


/*----------------------------------------------------------------------
  ClearAllRelations removes all relations of the removed box pBox.
  Remove all local relations and their reverse relations
  Remove remaining relation in the parent box.
  ----------------------------------------------------------------------*/
void ClearAllRelations (PtrBox pBox, int frame)
{

  PtrAbstractBox      pAb;
  PtrAbstractBox      pRefAb;
  PtrPosRelations     pPosRel;
  BoxRelation        *pRelation;
  int                 i;
  ThotBool            loop;
  ThotBool            notEmpty;

  pRefAb = pBox->BxAbstractBox;
  pPosRel = pBox->BxPosRelations;
  pBox->BxPosRelations = NULL;
  loop = TRUE;
  if (pPosRel != NULL)
    while (loop)
      {
        i = 1;
        notEmpty = pPosRel->PosRTable[i - 1].ReBox != NULL;
        while (i <= MAX_RELAT_POS && notEmpty)
          {
            pRelation = &pPosRel->PosRTable[i - 1];
            /* Remove the reverse relation */
            if (pRelation->ReOp == OpHorizDep)
              RemovePosRelation (pRelation->ReBox, pBox, NULL, TRUE, TRUE, TRUE);
            else if (pRelation->ReOp == OpVertDep)
              RemovePosRelation (pRelation->ReBox, pBox, NULL, TRUE, TRUE, FALSE);
            i++;
            if (i <= MAX_RELAT_POS)
              notEmpty = pPosRel->PosRTable[i - 1].ReBox != NULL;
          }
	
        if (pPosRel->PosRNext == NULL)
          loop = FALSE;
        /* Next block */
        FreePosBlock (&pPosRel);
      }

  /* Remove remaining relations in the enclosing box */
  pAb = pRefAb->AbEnclosing;
  if (pAb != NULL)
    {
      /* vertical */
      RemovePosRelation (pAb->AbBox, pBox, NULL, FALSE, TRUE, FALSE);
      /*horizontal */
      RemovePosRelation (pAb->AbBox, pBox, NULL, FALSE, TRUE, TRUE);
    }

  /* Remove dimension relations */
  ClearDimRelation (pBox, TRUE, frame);
  ClearDimRelation (pBox, FALSE, frame);
}


/*----------------------------------------------------------------------
  ClearDimRelation looks for the box that gives the current box dimension
  It could be  the enclosing, a sibling, or a relation out of structure
  (the relation is stored in the other box).
  Si cette dependance existe encore, on detruit la        
  relation. L'indicateur BtLg(Ht)HorsStruct indique que   
  la relation est hors-structure.                         
  ----------------------------------------------------------------------*/
void ClearDimRelation (PtrBox pBox, ThotBool horizRef, int frame)
{
  PtrBox              pOrginBox;
  PtrAbstractBox      pAb;

  if (pBox == NULL || pBox->BxAbstractBox == NULL)
    return;

  pOrginBox = NULL;
  pAb = pBox->BxAbstractBox;
  if (horizRef && pBox->BxHorizFlex)
    {
      /* Remove out of structure width constraints */
      pOrginBox = pBox->BxHorizFlex;
      RemovePosRelation (pOrginBox, pBox, NULL, FALSE, FALSE, TRUE);
      /* Restore the fixed edge */
      pBox->BxHorizEdge = pAb->AbHorizPos.PosEdge;
      pBox->BxHorizInverted = FALSE;
      pBox->BxHorizFlex = NULL;
      ResizeWidth (pBox, NULL, NULL, -pBox->BxW, 0, 0, 0, frame, FALSE);
    }
  else if (!horizRef && pBox->BxVertFlex)
    {
      /* Remove out of structure width constraints */
      pOrginBox = pBox->BxVertFlex;
      RemovePosRelation (pOrginBox, pBox, NULL, FALSE, FALSE, FALSE);
      /* Restore the fixed edge */
      pBox->BxVertEdge = pAb->AbVertPos.PosEdge;
      pBox->BxVertInverted = FALSE;
      pBox->BxVertFlex = NULL;
      ResizeHeight (pBox, NULL, NULL, -pBox->BxH, 0, 0, frame);
    }
  else
    CheckDimRelation (pBox, horizRef);
  // reset flags
  if (horizRef && pBox->BxWOutOfStruct)
    pBox->BxWOutOfStruct = FALSE;
  else if (!horizRef && pBox->BxHOutOfStruct)
    pBox->BxHOutOfStruct = FALSE;
}
