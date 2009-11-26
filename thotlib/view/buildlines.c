/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2009
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * This module handles line constructions
 *
 * Author: I. Vatton (INRIA)
 *
 */
#include "thot_gui.h"
#include "ustring.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "frame.h"

#define THOT_EXPORT extern
#include "boxes_tv.h"
#include "select_tv.h"
#include "units_tv.h"
#include "frame_tv.h"

#include "boxmoves_f.h"
#include "boxlocate_f.h"
#include "boxpositions_f.h"
#include "buildboxes_f.h"
#include "exceptions_f.h"
#include "memory_f.h"
#include "boxrelations_f.h"
#include "boxselection_f.h"
#include "buildlines_f.h"
#include "font_f.h"
#include "frame_f.h"
#include "hyphen_f.h"

#ifdef _GL
#if defined (_MACOS) && defined (_WX)
#include <gl.h>
#else /* _MACOS */
#include <GL/gl.h>
#endif /* _MACOS */
#include "glwindowdisplay.h"
#endif /* _GL */

#define SPACE_VALUE_MIN  3
#define SPACE_VALUE_MAX  6
/* max number of consecutive hyphens */
#define MAX_SIBLING_HYPHENS 2


/*----------------------------------------------------------------------
  GetNextBox returns the box assiated to the next in live abstract box.
  ----------------------------------------------------------------------*/
PtrBox GetNextBox (PtrAbstractBox pAb, int frame)
{
  PtrAbstractBox      pNextAb;
  ThotBool            loop;
  PtrBox              result;

  /* verifie que le pave existe toujours */
  if (pAb == NULL)
    result = NULL;
  else
    {
      pNextAb = pAb->AbNext;
      loop = TRUE;
      while (loop)
        if (pNextAb == NULL)
          {
            /* Est-ce la derniere boite fille d'une boite eclatee */
            if (pAb->AbEnclosing &&
                pAb->AbEnclosing->AbBox &&
                (pAb->AbEnclosing->AbBox->BxType == BoGhost ||
                 pAb->AbEnclosing->AbBox->BxType == BoStructGhost ||
                 pAb->AbEnclosing->AbBox->BxType == BoFloatGhost))
              {
                /* remonte la hierarchie */
                pAb = pAb->AbEnclosing;
                pNextAb = pAb->AbNext;
              }
            else
              loop = FALSE;
          }
        else if (pNextAb->AbDead ||
                 pNextAb->AbVisibility < ViewFrameTable[frame - 1].FrVisibility)
          pNextAb = pNextAb->AbNext;
        else if (pNextAb->AbBox == NULL)
          pNextAb = pNextAb->AbNext;
        else if (pNextAb->AbPresentationBox && pNextAb->AbTypeNum == 0 &&
                 pNextAb->AbHorizPos.PosAbRef && pNextAb->AbHorizPos.PosAbRef->AbFloat != 'N')
          // skip not inline bullets
          pNextAb = pNextAb->AbNext;
        else if (pNextAb->AbBox->BxType == BoGhost ||
                 pNextAb->AbBox->BxType == BoStructGhost ||
                 pNextAb->AbBox->BxType == BoFloatGhost)
          {
            /* descend la hierarchie */
            while (loop && pNextAb)
              {
                if (pNextAb->AbBox == NULL)
                  {
                    if (pNextAb->AbNext)
                      pNextAb = pNextAb->AbNext;
                    else
                      loop = FALSE;
                  }
                else if (pNextAb->AbBox->BxType == BoGhost ||
                         pNextAb->AbBox->BxType == BoStructGhost ||
                         pNextAb->AbBox->BxType == BoFloatGhost)
                  pNextAb = pNextAb->AbFirstEnclosed;
                else
                  loop = FALSE;
              }
            loop = TRUE;
            pAb = pNextAb;
          }
        else
          loop = FALSE;
      if (pNextAb == NULL)
        result = NULL;
      else
        {
          result = pNextAb->AbBox;
          if (result && result->BxType == BoMulScript)
            /* return the first script box */
            result = result->BxNexChild;
        }
    }
  return result;
}


/*----------------------------------------------------------------------
  GetPreviousBox rend l'adresse de la boite associee au pave vivant 
  qui precede pAb.                                        
  ----------------------------------------------------------------------*/
static PtrBox GetPreviousBox (PtrAbstractBox pAb, int frame)
{
  PtrAbstractBox      pNextAb;
  ThotBool            loop;
  PtrBox              result;

  pNextAb = pAb->AbPrevious;
  loop = TRUE;
  while (loop)
    {
      if (pNextAb == NULL)
        /* Est-ce la derniere boite fille d'une boite eclatee */
        if (pAb->AbEnclosing->AbBox->BxType == BoGhost ||
            pAb->AbEnclosing->AbBox->BxType == BoStructGhost ||
            pAb->AbEnclosing->AbBox->BxType == BoFloatGhost)
          {
            /* remonte la hierarchie */
            pAb = pAb->AbEnclosing;
            pNextAb = pAb->AbPrevious;
          }
        else
          loop = FALSE;
      else if (pNextAb->AbDead ||
               pNextAb->AbVisibility < ViewFrameTable[frame - 1].FrVisibility)
        pNextAb = pNextAb->AbPrevious;
      else if (pNextAb->AbBox == NULL)
        pNextAb = pNextAb->AbPrevious;
      else if (pNextAb->AbPresentationBox && pNextAb->AbTypeNum == 0 &&
               pNextAb->AbHorizPos.PosAbRef && pNextAb->AbHorizPos.PosAbRef->AbFloat != 'N')
        // skip not inline bullets
        pNextAb = pNextAb->AbPrevious;
      else if (pNextAb->AbBox->BxType == BoGhost ||
               pNextAb->AbBox->BxType == BoStructGhost ||
               pNextAb->AbBox->BxType == BoFloatGhost)
        {
          /* move down hierarchy */
          while (!pNextAb->AbDead && pNextAb->AbBox &&
                 (pNextAb->AbBox->BxType == BoGhost ||
                  pNextAb->AbBox->BxType == BoStructGhost ||
                  pNextAb->AbBox->BxType == BoFloatGhost))
            {
              if (pNextAb->AbFirstEnclosed->AbDead)
                pNextAb->AbBox->BxType = BoComplete;
              else
                pNextAb = pNextAb->AbFirstEnclosed;
              /* recherche le dernier pave fils */
              while (pNextAb->AbNext != NULL)
                pNextAb = pNextAb->AbNext;
            }
          pAb = pNextAb;
        }
      else
        loop = FALSE;
    }
  if (pNextAb == NULL)
    result = NULL;
  else
    {
      result = pNextAb->AbBox;
      if (result &&
          (result->BxType == BoMulScript || result->BxType == BoSplit))
        /* return the last script box */
        while (result->BxNexChild &&
               (result->BxNexChild->BxType == BoScript ||
                result->BxNexChild->BxType == BoDotted ||
                result->BxNexChild->BxType == BoPiece))
          if (result->BxNexChild->BxAbstractBox != pNextAb)
            // should not occur
            result->BxNexChild = NULL;
          else
            result = result->BxNexChild;
    }
  return result;
}

/*----------------------------------------------------------------------
  HasVariableWidth returns TRUE if the width of the box can be adapted
  ----------------------------------------------------------------------*/
static ThotBool HasVariableWidth (PtrBox pBox, PtrBox pBlock)
{
  PtrAbstractBox pAb, pAbRef;

  if (pBox == NULL || pBox->BxAbstractBox == NULL || pBox->BxType == BoFloatGhost)
    return FALSE;
  pAb = pBox->BxAbstractBox;
  if (pAb->AbLeafType == LtText)
    return TRUE;
  pAbRef = pAb->AbWidth.DimAbRef;
  if (pAbRef == NULL)
    pAbRef = pAb->AbEnclosing;
  return (pAb->AbLeafType == LtCompound &&
          (pAb->AbWidth.DimAbRef ||
           pAb->AbWidth.DimUnit == UnPercent ||
           pAb->AbWidth.DimUnit == UnAuto) &&
          pAbRef &&  pAbRef->AbBox &&
          (pAbRef == pBlock->BxAbstractBox ||
           pAbRef->AbBox->BxType == BoFloatGhost ||
           pAbRef->AbBox->BxType == BoStructGhost));
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
static void HandleNotInline (PtrBox pBlock, PtrLine pLine, int frame)
{
  PtrBox              pBox, pBoxInLine, refBox;
  PtrAbstractBox      pRefAb, pNext;
  int                 dx, dy;

  if (pLine->LiFirstPiece)
    pBoxInLine = pLine->LiFirstPiece;
  else
    pBoxInLine = pLine->LiFirstBox;
  if (pBoxInLine)
    {
      pBox = pBoxInLine->BxPrevious;
      if (pBox && pBox->BxAbstractBox &&
          (!pBox->BxAbstractBox->AbHorizEnclosing || pBox->BxAbstractBox->AbNotInLine))
        // manage this previous not in line box
        pBoxInLine = pBox;
      do
        {
          if (pBoxInLine->BxType == BoSplit || pBoxInLine->BxType == BoMulScript)
            pBox = pBoxInLine->BxNexChild;
          else
            pBox = pBoxInLine;
          if (pBox->BxAbstractBox->AbFloat == 'N' && !ExtraFlow (pBox, frame))
            {
              pRefAb = pBox->BxAbstractBox->AbHorizPos.PosAbRef;
              if (pBox->BxAbstractBox->AbNotInLine &&
                  // don't apply that rule in the refered element is the root
                  pRefAb && pRefAb->AbEnclosing)
                {
                  // look at the position of the referred box
                  pNext = pRefAb;
                  while (pRefAb && pRefAb->AbBox && pNext &&
                         (pRefAb->AbBox->BxType == BoGhost ||
                          pRefAb->AbBox->BxType == BoStructGhost ||
                          pRefAb->AbNotInLine ||
                          pRefAb->AbDead ||
                          pRefAb->AbVisibility < ViewFrameTable[frame - 1].FrVisibility ||
                          pRefAb->AbBox == pBox))
                    {
                      if (pRefAb->AbBox->BxType == BoGhost ||
                          pRefAb->AbBox->BxType == BoStructGhost)
                        pNext = pRefAb->AbFirstEnclosed;
                      else
                        pNext = pRefAb->AbNext;
                      if (pNext &&
                          !TypeHasException (ExcIsBreak,
                                             pNext->AbElement->ElTypeNumber,
                                             pNext->AbElement->ElStructSchema))
                        pRefAb = pNext;
                      else 
                        pNext = NULL;

                      // look for the bottom position
                      if (pBox->BxAbstractBox->AbVertPos.PosRefEdge == Bottom)
                        {
                          while (pRefAb && pRefAb->AbNext)
                            pRefAb = pRefAb->AbNext;
                        }
                    }
                  if (pRefAb && pRefAb->AbBox)
                    {
                      refBox = pRefAb->AbBox;
                      if (refBox->BxType == BoSplit || refBox->BxType == BoMulScript)
                        {
                          refBox = refBox->BxNexChild;
                          if (pBox->BxAbstractBox->AbVertPos.PosRefEdge == Bottom)
                            {
                              // move to the last of enclosed boxes
                              while (refBox->BxNexChild)
                                refBox = refBox->BxNexChild;
                            }
                        }
                      dx = refBox->BxXOrg + pBox->BxAbstractBox->AbHorizPos.PosDistance - pBox->BxXOrg;
                      dy = refBox->BxYOrg + pBox->BxAbstractBox->AbVertPos.PosDistance - pBox->BxYOrg;
                      if (pBox->BxAbstractBox->AbVertPos.PosRefEdge == Bottom)
                        dy = dy + refBox->BxHeight;
                    }
                  else
                    {
                      dx = 0;
                      dy = pBlock->BxYOrg + pLine->LiYOrg - pBox->BxYOrg;
                    }
                  XMove (pBox, NULL, dx, frame);
                  YMove (pBox, NULL, dy, frame);
                }
              else if (!pBox->BxAbstractBox->AbHorizEnclosing)
                {
                  dy = pBlock->BxYOrg + pLine->LiYOrg + pLine->LiHorizRef - pBox->BxHorizRef - pBox->BxYOrg;
                  YMove (pBox, NULL, dy, frame);
                }
            }
          if (pBox->BxAbstractBox->AbLeafType == LtText && pBox->BxNexChild)
            /* get the next child */
            pBoxInLine = pBox->BxNexChild;
          else
            pBoxInLine = GetNextBox (pBox->BxAbstractBox, frame);
        }
      while (pBoxInLine && pBox != pLine->LiLastBox && pBox != pLine->LiLastPiece);
    }
}

/*----------------------------------------------------------------------
  Adjust computes the space width in the adjusted line of pParentBox.
  pBlock gives block information.
  Move and update the width of all included boxes.
  Work with absolute positions when xAbs and yAbs are TRUE.
  ----------------------------------------------------------------------*/
static void Adjust (PtrBox pParentBox, PtrBox pBlock, PtrLine pLine, int frame,
                    ThotBool xAbs, ThotBool yAbs)
{
  PtrBox              pBox, pBoxInLine;
  PtrBox              boxes[200];
  int                 width, baseline;
  int                 nSpaces, delta;
  int                 x, dx, dy;
  int                 i, j, k, max;
  int                 l = 0, r = 0;
  ThotBool            rtl;

  if (pLine->LiFirstBox == NULL)
    /* no box in the current line */
    return;
  /* take into account the writing direction */
  rtl = pBlock->BxAbstractBox->AbDirection == 'R';
  x = pLine->LiXOrg;
  if (rtl)
    /* right-to-left wirting */
    x += pLine->LiXMax;
  if (xAbs || !pParentBox->BxXToCompute)
    // the ligne position is absolute
    x += pParentBox->BxXOrg;
  nSpaces = 0;	/* number of spaces */
  width = 0;	/* text width without spaces */
  baseline = pLine->LiYOrg + pLine->LiHorizRef;
  if (yAbs || !pParentBox->BxYToCompute)
    // the ligne position is absolute
    baseline += pParentBox->BxYOrg;
  
  /* get the list of boxes displayed in the line */
  if (pLine->LiFirstPiece)
    pBoxInLine = pLine->LiFirstPiece;
  else
    pBoxInLine = pLine->LiFirstBox;
  max = 0;
  if (pBoxInLine)
    do
      {
        if (pBoxInLine->BxType == BoSplit || pBoxInLine->BxType == BoMulScript)
          pBox = pBoxInLine->BxNexChild;
        else
          pBox = pBoxInLine;
        if (pBox->BxAbstractBox->AbFloat == 'N' && !ExtraFlow (pBox, frame))
          {
            if (pBox->BxAbstractBox->AbHorizEnclosing &&
                !pBox->BxAbstractBox->AbNotInLine)
              {
                boxes[max++] = pBox;
                /* Compute the line width without spaces */
                if (pBox->BxAbstractBox->AbLeafType == LtText)
                  {
                    pBox->BxSpaceWidth = 0;
                    delta = pBox->BxNSpaces * BoxCharacterWidth (SPACE, 1, pBox->BxFont);
                    pBox->BxW -= delta;
                    pBox->BxWidth -= delta;
                    nSpaces += pBox->BxNSpaces;
                    //GetExtraMargins (pBox, frame, TRUE, &t, &b, &l, &r);
                    width += pBox->BxWidth + l + r;
                  }
                else
                  width += pBox->BxWidth;
              }
          }
        if (pBox->BxAbstractBox->AbLeafType == LtText && pBox->BxNexChild)
          /* get the next child */
          pBoxInLine = pBox->BxNexChild;
        else
          pBoxInLine = GetNextBox (pBox->BxAbstractBox, frame);
      }
    while (max < 200 && pBoxInLine && pBox != pLine->LiLastBox &&
           pBox != pLine->LiLastPiece);

  /* Compute the space value in pixels */
  if (pLine->LiXMax > width)
    {
      pLine->LiNPixels = pLine->LiXMax - width;
      if (nSpaces == 0)
        pLine->LiSpaceWidth = 0;
      else
        pLine->LiSpaceWidth = pLine->LiNPixels / nSpaces;
      pLine->LiNSpaces = nSpaces;
      /* Mininmum lenght of the line */
      pLine->LiMinLength = width + nSpaces * SPACE_VALUE_MIN;
      /* Extra pixels */
      pLine->LiNPixels -= pLine->LiSpaceWidth * nSpaces;
    }
  else
    {
      pLine->LiNPixels = 0;
      pLine->LiSpaceWidth = 0;
      pLine->LiNSpaces = 0;
      pLine->LiMinLength = width;
    }

  /* Update the position and the width of each included box */
  nSpaces = pLine->LiNPixels;
  for (i = 0; i < max; i++)
    {
      pBox = boxes[i];
      /* Add an extra pixel */
      if (pBox->BxAbstractBox->AbLeafType == LtText)
        {
          if (nSpaces > pBox->BxNSpaces)
            pBox->BxNPixels = pBox->BxNSpaces;
          else
            pBox->BxNPixels = nSpaces;
          nSpaces -= pBox->BxNPixels;
          pBox->BxSpaceWidth = pLine->LiSpaceWidth;
          delta = pBox->BxNSpaces * pLine->LiSpaceWidth + pBox->BxNPixels;
          pBox->BxW += delta;
          pBox->BxWidth += delta;
        }
    }

  /* Now handle included boxes */
  for (i = 0; i < max; i++)
    {
      if (rtl && boxes[i]->BxScript != 'A' && boxes[i]->BxScript != 'H' &&
          boxes[i]->BxAbstractBox->AbDirection == 'L')
        {
          /* write following latin boxes left-to-right */
          j = i;
          while (i < max && boxes[i]->BxScript != 'A' && boxes[i]->BxScript != 'H' &&
                 boxes[i]->BxAbstractBox->AbDirection == 'L')
            i++;
          i--;
          for (k = i; k >= j; k--)
            {
              /* right-to-left wirting */
              x -= boxes[k]->BxWidth - l - r;
              dx = x - boxes[k]->BxXOrg;
              dy = baseline - boxes[k]->BxHorizRef - boxes[k]->BxYOrg;
              XMove (boxes[k], NULL, dx, frame);
              YMove (boxes[k], NULL, dy, frame);
            }
        }
      else if (!rtl && (boxes[i]->BxScript == 'A' || boxes[i]->BxScript == 'H' ||
                        boxes[i]->BxAbstractBox->AbDirection == 'R'))
        {
          /* write following arabic or hebrew boxes right-to-left */
          j = i;
          while (i < max && (boxes[i]->BxScript == 'A' || boxes[i]->BxScript == 'H' ||
                             boxes[i]->BxAbstractBox->AbDirection == 'R'))
            i++;
          i--;
          for (k = i; k >= j; k--)
            {
              /* left-to-right wirting */
              dx = x + l - boxes[k]->BxXOrg;
              dy = baseline - boxes[k]->BxHorizRef - boxes[k]->BxYOrg;
              XMove (boxes[k], NULL, dx, frame);
              YMove (boxes[k], NULL, dy, frame);
              x += boxes[k]->BxWidth + l + r;
            }

        }
      else
        {
          if (rtl)
            /* right-to-left wirting */
            x -= boxes[i]->BxWidth - l - r;
          else
            x += l;
          dx = x - boxes[i]->BxXOrg;
          dy = baseline - boxes[i]->BxHorizRef - boxes[i]->BxYOrg;
          XMove (boxes[i], NULL, dx, frame);
          YMove (boxes[i], NULL, dy, frame);
          if (!rtl)
            /* left-to-right wirting */
            x += boxes[i]->BxWidth + r;
        }
    }

  /* Now handle NotInline boxes */
  HandleNotInline (pParentBox, pLine, frame);
}


/*----------------------------------------------------------------------
  FloatToInt fait un arrondi float -> int.
  ----------------------------------------------------------------------*/
int FloatToInt (float e)
{
  int                 value;

  if (e < 0.0)
    value = (int) (e - 0.5);
  else
    value = (int) (e + 0.5);
  return value;
}


/*----------------------------------------------------------------------
  Align aligns included boxes in the line of pParentBox.
  pBlock gives block information.
  Move all included boxes.
  Work with absolute positions when xAbs and yAbs are TRUE.
  ----------------------------------------------------------------------*/
static void Align (PtrBox pParentBox, PtrBox pBlock, PtrLine pLine, int frame,
                   ThotBool xAbs, ThotBool yAbs)
{
  PtrBox              pBox, pBoxInLine;
  PtrBox              boxes[200];
  BAlignment          adjust;
  int                 baseline, x, delta = 0;
  int                 i, j, k, max, dx, dy;
  int                 l = 0, r = 0, shift;
  ThotBool            rtl;

  if (pLine->LiFirstBox == NULL)
    /* no box in the current line */
    return;
  /* The baseline of the line */
  baseline = pLine->LiYOrg + pLine->LiHorizRef;
  adjust = pBlock->BxAbstractBox->AbAdjust;
  /* take into account the writing direction */
  rtl = pBlock->BxAbstractBox->AbDirection == 'R';
  pBox = pLine->LiFirstBox;
  if (pBlock->BxContentWidth || pBlock->BxShrink)
    x = pLine->LiXOrg;
  else if (rtl)
    {
      /* right-to-left writing */
      if (pLine->LiRealLength > pLine->LiXMax)
        /* this could be the case of a too short table */
        x = pLine->LiXOrg + pLine->LiRealLength;
      else
        {
          if (pBox == pLine->LiLastBox && pBox->BxAbstractBox &&
              pBox->BxAbstractBox->AbLeafType == LtCompound &&
              pBox->BxAbstractBox->AbFloat == 'N' &&
              !ExtraFlow (pBox, frame) &&
              (pBox->BxAbstractBox->AbDisplay == 'B' ||
               pBox->BxType == BoTable))
            {
              if (pBox->BxAbstractBox->AbHorizPos.PosEdge == VertMiddle ||
                  (pBox->BxAbstractBox->AbLeftMarginUnit == UnAuto &&
                   pBox->BxAbstractBox->AbRightMarginUnit == UnAuto))
                {
                  // center only if there is enough space
                  if (pLine->LiXMax > pLine->LiRealLength)
                    delta = (pLine->LiXMax - pLine->LiRealLength) / 2;
                }
              else if (pBox->BxAbstractBox->AbHorizPos.PosEdge == Left)
                delta = pLine->LiXMax - pLine->LiRealLength;
            }
          else if (adjust == AlignCenter)
            delta = (pLine->LiXMax - pLine->LiRealLength) / 2;
          else if (adjust == AlignLeft)
            delta = pLine->LiXMax - pLine->LiRealLength;
          x = pLine->LiXOrg + pLine->LiXMax - delta;
        }
    }
  else
    {
      if (pBox == pLine->LiLastBox && pBox->BxAbstractBox &&
          pBox->BxAbstractBox->AbLeafType == LtCompound &&
          pBox->BxAbstractBox->AbFloat == 'N' &&
          !ExtraFlow (pBox, frame) &&
          (pBox->BxAbstractBox->AbDisplay == 'B' ||
           pBox->BxType == BoTable))
        {
          if (pBox->BxAbstractBox->AbHorizPos.PosEdge == VertMiddle ||
              (pBox->BxAbstractBox->AbLeftMarginUnit == UnAuto &&
               pBox->BxAbstractBox->AbRightMarginUnit == UnAuto))
            {
              // center only if there is enough space
              if (pLine->LiXMax > pLine->LiRealLength)
                delta = (pLine->LiXMax - pLine->LiRealLength) / 2;
            }
          else if (pBox->BxAbstractBox->AbHorizPos.PosEdge == Right)
            delta = pLine->LiXMax - pLine->LiRealLength;
        }
      else if (adjust == AlignCenter)
        delta = (pLine->LiXMax - pLine->LiRealLength) / 2;
      else if (adjust == AlignRight)
        delta = pLine->LiXMax - pLine->LiRealLength;
      x = pLine->LiXOrg + delta;
    }
  if (xAbs || !pBox->BxXToCompute)
    // the ligne position is absolute
    x += pParentBox->BxXOrg;
  if (yAbs || !pBox->BxYToCompute)
    // the ligne position is absolute value
    baseline += pParentBox->BxYOrg;

  /* get the list of boxes displayed in the line */
  if (pLine->LiFirstPiece)
    pBoxInLine = pLine->LiFirstPiece;
  else
    pBoxInLine = pLine->LiFirstBox;

  if (pLine->LiFirstBox == pLine->LiLastBox &&
      // there is a special management of floated boxes
      pBoxInLine->BxLMargin > 0 &&
      (pBoxInLine->BxType == BoTable ||
       pBoxInLine->BxAbstractBox->AbDisplay == 'B'))
    {
      if (pBoxInLine->BxLMargin > pLine->LiXOrg)
        x -= pLine->LiXOrg;
      else
        x -= pBoxInLine->BxLMargin;
    }
  max = 0;
  if (pBoxInLine)
    do
      {
        if (pBoxInLine->BxType == BoSplit || pBoxInLine->BxType == BoMulScript)
          pBox = pBoxInLine->BxNexChild;
        else
          pBox = pBoxInLine;
        if (pBox->BxAbstractBox->AbFloat == 'N' && !ExtraFlow (pBox, frame))
          {
            if (pBox->BxAbstractBox->AbHorizEnclosing &&
                !pBox->BxAbstractBox->AbNotInLine)
              {
                boxes[max++] = pBox;
                if (pBox->BxAbstractBox->AbLeafType == LtText)
                  pBox->BxSpaceWidth = 0;
              }
          }
        if (pBox->BxAbstractBox->AbLeafType == LtText && pBox->BxNexChild)
          /* get the next child */
          pBoxInLine = pBox->BxNexChild;
        else
          pBoxInLine = GetNextBox (pBox->BxAbstractBox, frame);
      }
    while (max < 200 && pBoxInLine && pBox != pLine->LiLastBox &&
           pBox != pLine->LiLastPiece);

  /* Now handle included boxes */
  shift = pBlock->BxW - pLine->LiXOrg - pLine->LiXMax;
  for (i = 0; i < max; i++)
    {
      if (rtl && boxes[i]->BxScript != 'A' && boxes[i]->BxScript != 'H' &&
          boxes[i]->BxAbstractBox->AbDirection == 'L')
        {
          /* write following latin boxes left-to-right */
          j = i;
          while (i < max && boxes[i]->BxScript != 'A' && boxes[i]->BxScript != 'H' &&
                 boxes[i]->BxAbstractBox->AbDirection == 'L')
            i++;
          i--;
          for (k = i; k >= j; k--)
            {
              /* right-to-left wirting */
              x -= boxes[k]->BxWidth - l - r;
              dx = x - boxes[k]->BxXOrg;
              dy = baseline - boxes[k]->BxHorizRef - boxes[k]->BxYOrg;
              XMove (boxes[k], NULL, dx, frame);
              YMove (boxes[k], NULL, dy, frame);
            }
        }
      else if (!rtl && (boxes[i]->BxScript == 'A' || boxes[i]->BxScript == 'H' ||
                        boxes[i]->BxAbstractBox->AbDirection == 'R'))
        {
          /* write following arabic or hebrew boxes right-to-left */
          j = i;
          while (i < max && (boxes[i]->BxScript == 'A' || boxes[i]->BxScript == 'H' ||
                             boxes[i]->BxAbstractBox->AbDirection == 'R'))
            i++;
          i--;
          for (k = i; k >= j; k--)
            {
              /* left-to-right writing */
              x += l;
              dx = x - boxes[k]->BxXOrg;
              dy = baseline - boxes[k]->BxHorizRef - boxes[k]->BxYOrg;
              XMove (boxes[k], NULL, dx, frame);
              YMove (boxes[k], NULL, dy, frame);
              x += boxes[k]->BxWidth + r;
            }

        }
      else
        {
          if (rtl)
            /* right-to-left wirting */
            x -= boxes[i]->BxWidth - l - r;
          else
            x += l;
          if ((boxes[i]->BxType == BoBlock || boxes[i]->BxType == BoFloatBlock) &&
              boxes[i]->BxLMargin > 0 && pLine->LiXOrg > 0)
            {
              x -= boxes[i]->BxLMargin;
            }
          dx = x - boxes[i]->BxXOrg;
          dy = baseline - boxes[i]->BxHorizRef - boxes[i]->BxYOrg;
          XMove (boxes[i], NULL, dx, frame);
          YMove (boxes[i], NULL, dy, frame);
          if (!rtl)
            /* left-to-right wirting */
            x += boxes[i]->BxWidth + r;
        }
    }

  /* Now handle NotInline boxes */
  HandleNotInline (pParentBox, pLine, frame);
}


/*----------------------------------------------------------------------
  ManageBreakLine splits the text box at the position of the break element.
  width = inside width of the first piece.
  breakWidth = the width of the break element.
  boxLength = the number of chars (including spaces) that precede the break.
  nSpaces the number of included spaces in that string.
  pNewBuff = the buffer that contains the next character after the break.
  newIndex = the index the next character after the break in that buffer.
  pRootAb = the root abstract box for updating the chain of leaf boxes.
  l and r give extra left and right margins generated by enclosing ghosts.
  ----------------------------------------------------------------------*/
static void ManageBreakLine (PtrBox pBox, int width, int breakWidth,
                             int boxLength, int nSpaces, int newIndex,
                             int l, int r,
                             PtrTextBuffer pNewBuff, PtrAbstractBox pRootAb)
{
  PtrBox              ibox1, ibox2;
  PtrBox              pPreviousBox, pNextBox;
  PtrAbstractBox      pAb;
  SpecFont            font;
  int                 baseline;
  int                 height;

  if (boxLength < 0)
    return;
  /* Initialisation */
  ibox1 = NULL;
  pAb = pBox->BxAbstractBox;
  height = pBox->BxHeight;
  baseline = pBox->BxHorizRef;
  pPreviousBox = pBox->BxPrevious;
  pNextBox = pBox->BxNext;
  font = pBox->BxFont;

  if (pNewBuff != NULL)
    {
      if (pBox->BxType == BoComplete)
        {
          /* generate the first piece */
          ibox1 = GetBox (pAb);
          if (ibox1 == NULL)
            /* plus de boite */
            return;
        }
      else if (pBox->BxType == BoSplit || pBox->BxType == BoMulScript)
        {
          /* get the last created piece of that box */
          while (pBox->BxNexChild != NULL)
            pBox = pBox->BxNexChild;
        }
      /* generate a new piece */
      ibox2 = GetBox (pAb);
    }
  else
    ibox2 = NULL;

  /* Initialize the new last piece */
  if (ibox2)
    {
      ibox2->BxScript = pBox->BxScript;
      ibox2->BxContentWidth = TRUE;
      ibox2->BxContentHeight = TRUE;
      ibox2->BxH = pBox->BxH;
      ibox2->BxHeight = height;
      /* default position */
      ibox2->BxXOrg = pBox->BxXOrg + width;
      ibox2->BxYOrg = pBox->BxYOrg;
      ibox2->BxFont = font;
      ibox2->BxUnderline = pBox->BxUnderline;
      ibox2->BxThickness = pBox->BxThickness;
      ibox2->BxHorizRef = baseline;
      ibox2->BxType = BoPiece;
      ibox2->BxBuffer = pNewBuff;
      ibox2->BxNexChild = NULL;
      ibox2->BxFirstChar = pBox->BxFirstChar + boxLength + 1;
      ibox2->BxIndChar = newIndex;
      ibox2->BxNChars = pBox->BxNChars - boxLength - 1;
      ibox2->BxNSpaces = pBox->BxNSpaces - nSpaces;

      /* transmit width, margins, borders, and paddings */
      ibox2->BxW = pBox->BxW - width - breakWidth;
      ibox2->BxWidth = ibox2->BxW + r + pBox->BxRBorder + pBox->BxRPadding;
      if (pBox->BxRMargin > 0)
        ibox2->BxWidth += pBox->BxRMargin;
      ibox2->BxTMargin = pBox->BxTMargin;
      ibox2->BxTBorder = pBox->BxTBorder;
      ibox2->BxTPadding = pBox->BxTPadding;
      ibox2->BxBMargin = pBox->BxBMargin;
      ibox2->BxBBorder = pBox->BxBBorder;
      ibox2->BxBPadding = pBox->BxBPadding;
      ibox2->BxRMargin = pBox->BxRMargin;
      ibox2->BxRBorder = pBox->BxRBorder;
      ibox2->BxRPadding = pBox->BxRPadding;
      
      if (pBox->BxType == BoComplete)
        {
          /* Initialize the new first piece */
          ibox1->BxScript = pBox->BxScript;
          ibox1->BxContentWidth = TRUE;
          ibox1->BxContentHeight = TRUE;
          ibox1->BxIndChar = 0;
          ibox1->BxUnderline = pBox->BxUnderline;
          ibox1->BxThickness = pBox->BxThickness;
          ibox1->BxFont = font;
          ibox1->BxH = pBox->BxH;
          ibox1->BxHeight = height;
          /* default position */
          ibox1->BxXOrg = pBox->BxXOrg;
          ibox1->BxYOrg = pBox->BxYOrg;
          ibox1->BxHorizRef = baseline;
          ibox1->BxType = BoPiece;
          ibox1->BxBuffer = pBox->BxBuffer;
          ibox1->BxFirstChar = pBox->BxFirstChar;
          ibox1->BxNSpaces = nSpaces;
          ibox1->BxNChars = boxLength;

          /* transmit widths, margins, borders, and paddings */
          ibox1->BxW = width;
          ibox1->BxWidth = width + l + pBox->BxLBorder + pBox->BxLPadding;
          if (pBox->BxLMargin > 0)
            ibox1->BxWidth += pBox->BxLMargin;
          ibox1->BxTMargin = pBox->BxTMargin;
          ibox1->BxTBorder = pBox->BxTBorder;
          ibox1->BxTPadding = pBox->BxTPadding;
          ibox1->BxBMargin = pBox->BxBMargin;
          ibox1->BxBBorder = pBox->BxBBorder;
          ibox1->BxBPadding = pBox->BxBPadding;
          ibox1->BxLMargin = pBox->BxLMargin;
          ibox1->BxLBorder = pBox->BxLBorder;
          ibox1->BxLPadding = pBox->BxLPadding;

          /* update the chain of leaf boxes */
          ibox1->BxNexChild = ibox2;
          ibox1->BxPrevious = pPreviousBox;
          if (pPreviousBox != NULL)
            pPreviousBox->BxNext = ibox1;
          else
            pRootAb->AbBox->BxNext = ibox1;
          ibox1->BxNext = ibox2;
          ibox2->BxPrevious = ibox1;
          ibox2->BxNext = pNextBox;
          if (pNextBox != NULL)
            pNextBox->BxPrevious = ibox2;
          else
            pRootAb->AbBox->BxPrevious = ibox2;
	  
          /* Update the main box */
          pBox->BxType = BoSplit;
          pBox->BxNexChild = ibox1;
        }
      else
        {
          /* update the existing box */
          pBox->BxNSpaces = nSpaces;
          pBox->BxNChars = boxLength;

          /* remove right margin, border, and padding */
          pBox->BxW = width;
          pBox->BxWidth = width - pBox->BxRMargin - r - pBox->BxRBorder - pBox->BxRPadding;
          pBox->BxRMargin = 0;
          pBox->BxRBorder = 0;
          pBox->BxRPadding = 0;
	  
          /* update the chain of leaf boxes */
          ibox2->BxNexChild = pBox->BxNexChild;
          ibox2->BxPrevious = pBox;
          ibox2->BxNext = pNextBox;
          pBox->BxNexChild = ibox2;
          pBox->BxNext = ibox2;
          if (pNextBox != NULL)
            pNextBox->BxPrevious = ibox2;
          else
            pRootAb->AbBox->BxPrevious = ibox2;
        }
    }
}


/*----------------------------------------------------------------------
  FindBreakLine returns TRUE if there is a break element in the box.
  In that case returns:
  - boxLength = the number of chars (including spaces) that precede the break.
  - boxWidth = the inside width of that string (including spaces).
  - breakWidth = the width of the break;            
  - nSpaces = the number of included spaces in the previous string.
  - pNewBuff = the buffer that contains the next character after the break.
  - newIndex = the index the next character after the break in that buffer.
  - wordWidth = the width of the larger word in the string.
  ----------------------------------------------------------------------*/
static ThotBool FindBreakLine (PtrBox pBox, int *boxWidth, int *breakWidth,
                               int *boxLength, int *nSpaces, int *newIndex,
                               PtrTextBuffer *pNewBuff, int *wordWidth)
{
  PtrTextBuffer       pBuffer;
  SpecFont            font;
  CHAR_T              character;
  int                 i, j, l;
  int                 nChars;
  int                 wWidth, variant;
  ThotBool            found, checkEOL;

  found = FALSE;
  *boxWidth = 0;
  *boxLength = 0;
  *nSpaces = 0;
  *wordWidth = 0;
  wWidth = 0;
  j = 0;
  pBuffer = pBox->BxBuffer;
  nChars = pBox->BxNChars;
  font = pBox->BxFont;
  i = pBox->BxIndChar;
  *pNewBuff = pBuffer;
  *newIndex = 0;
  if (pBox->BxAbstractBox)
    variant = pBox->BxAbstractBox->AbFontVariant;
  else
    variant = 1;
  if (pBuffer && i >= pBuffer->BuLength)
    {
      /* start at the end of a buffer, get the next one */
      pBuffer = pBuffer->BuNext;
      i = 0;
    }

  // ignore NEW_LINE within generic XML document
  checkEOL = pBox && pBox->BxAbstractBox && pBox->BxAbstractBox->AbElement &&
    pBox->BxAbstractBox->AbElement->ElStructSchema &&
    !pBox->BxAbstractBox->AbElement->ElStructSchema->SsIsXml;

  while (j < nChars && !found && pBuffer)
    {
      character = pBuffer->BuContent[i];
      if (character == BREAK_LINE || (checkEOL && character ==  NEW_LINE))
        {
          /* It's a break element */
          found = TRUE;
          *breakWidth = BoxCharacterWidth (BREAK_LINE, 1, font);
          *boxLength = j;
          i++;
          if (i >= pBuffer->BuLength)
            {
              /* get the next buffer */
              if (pBuffer->BuNext)
                {
                  pBuffer = pBuffer->BuNext;
                  i = 0;
                }
              else
                /* end of the box */
                j = nChars;
            }
	  
          *pNewBuff = pBuffer;
          *newIndex = i;
        }
      else
        {
          /* No break element found, continue */
          if (character == SPACE)
            {
              (*nSpaces)++;
              *boxWidth += BoxCharacterWidth (SPACE, 1, font);
              /* compare word widths */
              if (*wordWidth < wWidth)
                *wordWidth = wWidth;
              wWidth = 0;
            }
          else
            {
              if (character >= 0x060C && character <= 0x06B0) /* arabic char */
                l = BoxArabicCharacterWidth (character, &pBuffer, &i, font);
              else
                l = BoxCharacterWidth (character, variant, font);
              *boxWidth += l;
              wWidth += l;
            }
        }
      
      if (!found)
        {
          /* next buffer ? */
          if (i >= pBuffer->BuLength - 1)
            {
              pBuffer = pBuffer->BuNext;
              if (pBuffer == NULL)
                j = nChars;
              i = 0;
            }
          else
            i++;
          j++;
        }
    }
  
  if (*wordWidth < wWidth)
    /* compare word widths */
    *wordWidth = wWidth;
  return (found);
}


/*----------------------------------------------------------------------
  SearchBreak looks for the break point to respect the maximum line width
  (max) given in pixel.
  l and r give extra left and right margins generated by enclosing ghosts.
  line_spaces gives the current number of spaces in the current line
  hyphenate is TRUE when the word hyphenation is allowed
  Return:
  - the number of characters to skip:
  * 1 or more spaces,
  * -1 if an hyphen should be generated,
  * -2 if the last word should be split without hyphen
  * 0 when no break point is found
  - boxLength = the number of chars (including spaces) that precede the break.
  - boxWidth = the inside width of that string (including spaces).
  - nSpaces = the number of included spaces in that string.
  - pNewBuff = the buffer that contains the next character after the break.
  - newIndex = the index the next character after the break in that buffer.
  ----------------------------------------------------------------------*/
static int SearchBreak (PtrLine pLine, PtrBox pBox, int max, SpecFont font,
                        int l, int r, int line_spaces, ThotBool hyphenate,
                        int *boxLength, int *boxWidth, int *nSpaces,
                        int *newIndex, PtrTextBuffer *pNewBuff)
{
  PtrLine             pPreviousLine;
  PtrTextBuffer       pBuffer;
  PtrBox              pParentBox;
  Language            language;
  CHAR_T              character;
  int                 i, count;
  int                 carWidth, newWidth;
  int                 width, variant;
  int                 wordLength, charIndex;
  int                 dummySpaces, spaceWidth;
  int                 spaceAdjust;
  int                 spaceCount;
  ThotBool            still;

  dummySpaces = 0;
  character = 0;
  *pNewBuff = NULL;
  *newIndex = 0;
  *boxLength = pBox->BxNChars;
  *boxWidth = pBox->BxW;
  *nSpaces = pBox->BxNSpaces;
  if (pBox->BxScript == 'Z')
    // Japanese can be split everywhere
    hyphenate = TRUE;
  charIndex = pBox->BxIndChar; /* buffer index */
  pBuffer = pBox->BxBuffer;
  /* newWidth is the width used to build lines */
  /* width is the real width of the text       */
  newWidth = l + pBox->BxLBorder + pBox->BxLPadding;
  if (pBox->BxLMargin > 0)
    newWidth += pBox->BxLMargin;
  width = 0;
  carWidth = 0;
  wordLength = 0;
  spaceCount = 0;
  still = TRUE;
  spaceWidth = BoxCharacterWidth (SPACE, 1, font);
  spaceAdjust = spaceWidth;
  if (pBox->BxAbstractBox)
    variant = pBox->BxAbstractBox->AbFontVariant;
  else
    variant = 1;
  if (pBox->BxScript == 'L')
    language = pBox->BxAbstractBox->AbLang;
  else
    language = TtaGetLanguageIdFromScript (pBox->BxScript);
  if (max < pBox->BxWidth)
    {
      /* we are not just removing extra spaces at the end of the line */
      pParentBox = pBox->BxAbstractBox->AbEnclosing->AbBox;
      /* get the block of lines */
      while (pParentBox->BxType == BoGhost)
        pParentBox = pParentBox->BxAbstractBox->AbEnclosing->AbBox;
      if (pParentBox->BxAbstractBox->AbAdjust == AlignJustify)
        /* for a justified line take the minimum space width */
        spaceAdjust = FloatToInt ((float) (spaceAdjust * 7) / (float) (10));
    }

  i = 0;
  count = pBox->BxNChars;
  /* Look for a break point */
  while (still)
    {
      /* width of the next character */
      character = pBuffer->BuContent[charIndex];
      if (character == EOS)
        carWidth = 0;
      else if (character == SPACE || character == NEW_LINE)
        carWidth = spaceAdjust;
      else
        if (character >= 0x060C && character <= 0x06B0) /* arabic char */
          carWidth = BoxArabicCharacterWidth (character, &pBuffer, &charIndex, font);
        else
          carWidth = BoxCharacterWidth (character, variant, font);

      if ((newWidth + carWidth > max || i >= count) && i != 0 &&
          // don't split a word
          (character == SPACE || character == NEW_LINE ||
           spaceCount + line_spaces> 0 || hyphenate))
        {
          /* The character cannot be inserted in the line */
          still = FALSE;
          if (max >= pBox->BxWidth)
            {
              /* we are removing extra spaces at the end of the line */
              /* go to the box end */
              *newIndex = 0;
              *pNewBuff = NULL;
              *boxWidth = pBox->BxW;
              *boxLength = pBox->BxNChars;
              *nSpaces = pBox->BxNSpaces;
            }
          else if (character == SPACE || character == NEW_LINE)
            {
              /* it is a space, skip all following spaces */
              dummySpaces = 1;
              *boxLength = i;
              *boxWidth = width;
              *nSpaces = spaceCount;
              /* Select the first character after the break */
              if (charIndex >= pBuffer->BuLength - 1 && pBuffer->BuNext)
                {
                  /* the next character is in another buffer */
                  *pNewBuff = pBuffer->BuNext;
                  *newIndex = 0;
                }
              else
                {
                  /* the next character is in the same buffer */
                  *pNewBuff = pBuffer;
                  *newIndex = charIndex + 1;
                }
            }
          else if (spaceCount == 0)
            {
              /* no space found */
              (*boxLength) = i;
              *boxWidth = width;
              *nSpaces = spaceCount;
              /* Select the first character after the break */
              *pNewBuff = pBuffer;
              *newIndex = charIndex;
            }
          else
            {
              /* go to the previous space */
              dummySpaces = 1;
              *nSpaces = spaceCount - 1;
              pBuffer = *pNewBuff;
              charIndex = *newIndex;
              /* Select the first character after the break */
              if (*newIndex >= (*pNewBuff)->BuLength - 1)
                {
                  if ((*pNewBuff)->BuNext)
                    {
                      /* the next character is in another buffer */
                      *pNewBuff = pBuffer->BuNext;
                      *newIndex = 0;
                    }
                }
              else
                /* the next character is in the same buffer */
                (*newIndex)++;
            }
        }
      else if (character == SPACE || character == NEW_LINE)
        {
          /* register the current space for the future */
          *pNewBuff = pBuffer;
          *newIndex = charIndex;
          /* text length */
          *boxLength = i;
          spaceCount++;
          *boxWidth = width;
          wordLength = 1;/* next word */
          i++;		/* number of managed characters */
          newWidth += carWidth;
          width += spaceWidth;
          if (charIndex == pBuffer->BuLength - 1 && pBuffer->BuNext == NULL)
            {
              /* last character of the box */
              still = FALSE;
              dummySpaces = 1;
              (*newIndex)++;
              dummySpaces = 1;
              *nSpaces = spaceCount - 1;
            }
        }
      else
        {
          /* a simple character */
          wordLength++;
          i++;		/* number of managed characters */
          newWidth += carWidth;
          width += carWidth;
        }

      /* next character */
      if (still)
        {
          charIndex++;	/* index of the next character */	  
          if (charIndex >= pBuffer->BuLength)
            {
              if (pBuffer->BuNext == NULL)
                {
                  /* end of the box */
                  still = FALSE;
                  *pNewBuff = NULL;
                  *newIndex = charIndex;
                  *boxWidth = pBox->BxW;
                  *boxLength = pBox->BxNChars;
                  *nSpaces = pBox->BxNSpaces;
                }
              else
                {
                  /* next buffer */
                  pBuffer = pBuffer->BuNext;
                  charIndex = 0;
                }
            }
        }
    }

  /* Point the last character before the break */
  if (pBuffer && charIndex == 0 && pBuffer->BuPrevious)
    {
      /* the previous character is in another buffer */
      pBuffer = pBuffer->BuPrevious;
      charIndex = pBuffer->BuLength - 1;
    }
  else if (*pNewBuff)
    /* the previous character is in the same buffer */
    charIndex--;
  /* Remove extra spaces just before the break */
  still = (pBuffer && (dummySpaces || max >= pBox->BxWidth));
  while (still && *boxLength > 0)
    {
      character = pBuffer->BuContent[charIndex];
      if (character == SPACE || character == NEW_LINE)
        {
          if (*pNewBuff == NULL)
            {
              /* points the last character of the box */
              *pNewBuff = pBuffer;
              *newIndex = charIndex + 1;
            }
          /* previous char */
          if (charIndex == 0)
            {
              if (pBuffer->BuPrevious)
                {
                  pBuffer = pBuffer->BuPrevious;
                  charIndex = pBuffer->BuLength - 1;
                }
              else
                still = FALSE;
            }
          else
            charIndex--;
	  
          *boxWidth -= spaceWidth;
          dummySpaces++;
          (*boxLength)--;
          (*nSpaces)--;
        }
      else
        still = FALSE;
    }

  if (*pNewBuff == NULL && (dummySpaces || spaceCount))
    {
      /* the end of the box was reached */
      *pNewBuff = pBuffer;
      *newIndex = charIndex + 1;
    }
  else if (max < pBox->BxWidth && CanHyphen (pBox) &&
           wordLength > 1 && !TextInserting)
    {
      /* Avoid more than MAX_SIBLING_HYPHENS consecutive hyphens */
      still = TRUE;
      if (pLine && pBox->BxScript != 'Z')
        {
          /* don't really generate an hyphen in asian scripts */
          pPreviousLine = pLine->LiPrevious;
          count = 0;
          while (count < MAX_SIBLING_HYPHENS && pPreviousLine)
            if (pPreviousLine->LiLastPiece)
              {
                if (pPreviousLine->LiLastPiece->BxType == BoDotted)
                  {
                    /* the previous line has an hyphen */
                    /* continue */
                    pPreviousLine = pPreviousLine->LiPrevious;
                    count++;
                  }
                else
                  pPreviousLine = NULL;
              }
            else
              pPreviousLine = NULL;
          if (count == MAX_SIBLING_HYPHENS)
            /* don't break the last word */
            still = FALSE;
        }
      
      if (still)
        {
          /* Try to break the last word */
          if (dummySpaces == 0)
            {
              /* there is only one word in the line */
              width = max;
              pBuffer = pBox->BxBuffer;
              charIndex = pBox->BxIndChar;
              wordLength = HyphenLastWord (font, variant, language, &pBuffer,
                                           &charIndex, &width, &still);
              if (wordLength > 0)
                {
                  /* we can insert an hyphen, we update the break point */
                  *pNewBuff = pBuffer;
                  *newIndex = charIndex;
                  *boxWidth = 0;
                  *boxLength = 0;
                }
            }
          else
            {
              /* try to hyphen the last word */
              width = max - *boxWidth - dummySpaces * spaceAdjust;
              wordLength = HyphenLastWord (font, variant, language, pNewBuff,
                                           newIndex, &width, &still);
            }
	  
          if (wordLength > 0)
            {
              /* update the break point */
              *boxLength += dummySpaces + wordLength;
              *nSpaces += dummySpaces;
              *boxWidth += width + dummySpaces * spaceWidth;
              if (still)
                /* generate the hyphen */
                dummySpaces = -1;
              else
                /* don't generate the hyphen */
                dummySpaces = -2;
            }
        }
  
      /*
       * If we cannot break the word and the word cannot be rejected
       * to the next line (only one word in that line), we force
       * a break after almost one character.
       */
      if (dummySpaces == 0 && *pNewBuff &&
          (pBox == pLine->LiFirstBox || pBox == pLine->LiFirstPiece))
        {
          /* generate an hyphen */
          dummySpaces = -1;
          *boxWidth += BoxCharacterWidth (173, 1, font);
          /* remove one or more characters corresponding to the hyphen width */
          while (*boxWidth > max && *boxLength > 1)
            {
              /* previous char */
              if (*newIndex == 1)
                if ((*pNewBuff)->BuPrevious != NULL)
                  {
                    *pNewBuff = (*pNewBuff)->BuPrevious;
                    *newIndex = (*pNewBuff)->BuLength;
                  }
                else
                  return dummySpaces;
              else
                (*newIndex)--;
              if (character >= 0x060C && character <= 0x06B0) /* arabic char */
                *boxWidth -= BoxArabicCharacterWidth (character, &pBuffer, &charIndex, font);
              else      
                *boxWidth -= BoxCharacterWidth (character, variant, font);
              (*boxLength)--;
            }
        }
    }
  
  /* Remove extra spaces just after the break */
  still = (*pNewBuff && dummySpaces > 0);
  while (still)
    {
      character = (*pNewBuff)->BuContent[*newIndex];
      if (character == SPACE || character == NEW_LINE)
        {
          /* next char */
          if (*newIndex >= (*pNewBuff)->BuLength)
            if ((*pNewBuff)->BuNext)
              {
                *pNewBuff = (*pNewBuff)->BuNext;
                *newIndex = 1;
              }
            else
              {
                still = FALSE;
                (*newIndex)++;
              }
          else
            (*newIndex)++;
	  
          dummySpaces++;
        }
      else
        still = FALSE;
    }
  return dummySpaces;
}


/*----------------------------------------------------------------------
  BreakPieceOfBox splits the piece into two pieces even if there is no
  space.
  l and r give extra left and right margins generated by enclosing ghosts.
  line_spaces gives the current number of spaces in the current line
  max = the maximum width of the first piece.
  pRootAb = the root abstract box for updating the chain of leaf boxes.
  hyphenate is TRUE when the word hyphenation is allowed
  Return TRUE if the whole box can be inserted in the line with compression.
  ----------------------------------------------------------------------*/
static ThotBool BreakPieceOfBox (PtrLine pLine, PtrBox pBox, int max,
                                 int l, int r, int line_spaces, ThotBool hyphenate,
                                 PtrAbstractBox pRootAb)
{
  PtrBox              ibox2, pNextBox;
  PtrTextBuffer       pNewBuff;
  PtrAbstractBox      pAb;
  SpecFont            font;
  int                 oldWidth, oldnSpaces;
  int                 oldlg, spaceWidth;
  int                 length;
  int                 width, lostPixels;
  int                 newIndex, nSpaces;
  int                 height, baseline;

  if (pBox->BxNChars <= 1)
    return TRUE;
  pAb = pBox->BxAbstractBox;
  height = pBox->BxHeight;
  baseline = pBox->BxHorizRef;
  pNextBox = pBox->BxNext;
  oldlg = pBox->BxNChars;
  font = pBox->BxFont;
  oldWidth = pBox->BxW;
  oldnSpaces = pBox->BxNSpaces;

  /* search a break */
  lostPixels = SearchBreak (pLine, pBox, max, font, l, r, line_spaces, hyphenate,
                            &length, &width, &nSpaces, &newIndex, &pNewBuff);
  if (lostPixels <= 0)
    /* don't break on a space */
    spaceWidth = 0;
  else
    /* break on a space */
    spaceWidth = BoxCharacterWidth (SPACE, 1, font);

  /*
   * Generate a new piece:
   * - if SearchBreak found a break point on a space (lostPixels > 0)
   *   or on a character with generation of an hyphen (lostPixels = -1)
   *   or without (lostPixels = -2);
   * - if we want to remove extra spaces at the end of the box.
   */
  if (pNewBuff &&
      (lostPixels != 0 || nSpaces != oldnSpaces || oldnSpaces == 0) &&
      (pBox->BxW != width /*|| length < pBox->BxNChars*/))
    ibox2 = GetBox (pAb);
  else
    {
      ibox2 = NULL;
      /* TRUE if the whole box can is inserted in the line */
      return (length == pBox->BxNChars);
    }
  if (lostPixels == -2)
    /* don't generate the hyphen */
    lostPixels = 0;
  
  if (ibox2)
    {
      /* update the existing box */
      pBox->BxNSpaces = nSpaces;
      pBox->BxNChars = length;

      /* transmit widths, margins, borders, and paddings */
      pBox->BxW = width;
      pBox->BxWidth = width - r - pBox->BxRMargin - pBox->BxRBorder - pBox->BxRPadding;
      pBox->BxRMargin = 0;
      pBox->BxRBorder = 0;
      pBox->BxRPadding = 0;

      if (lostPixels == -1)
        {
          /* add the hyphen at the end */
          pBox->BxType = BoDotted;
          width -= BoxCharacterWidth (173, 1, font);
          lostPixels = 0;
        }
      else if (pBox->BxType != BoScript)
        pBox->BxType = BoPiece;

      /* Initialize the new piece */
      ibox2->BxScript =  pBox->BxScript;
      ibox2->BxContentWidth = TRUE;
      ibox2->BxContentHeight = TRUE;
      ibox2->BxH = pBox->BxH;
      ibox2->BxHeight = height;
      /* default position */
      ibox2->BxXOrg = pBox->BxXOrg + width;
      ibox2->BxYOrg = pBox->BxYOrg;
      ibox2->BxFont = font;
      ibox2->BxUnderline = pBox->BxUnderline;
      ibox2->BxThickness = pBox->BxThickness;
      ibox2->BxHorizRef = baseline;
      ibox2->BxType = BoPiece;
      ibox2->BxBuffer = pNewBuff;
      ibox2->BxNexChild = NULL;
      /* Si lostPixels > 0 -> Il faut retirer les caracteres blanc de la boite */
      ibox2->BxFirstChar = pBox->BxFirstChar + lostPixels + length;
      ibox2->BxIndChar = newIndex;
      ibox2->BxNChars = oldlg - lostPixels - length;
      ibox2->BxNSpaces = oldnSpaces - lostPixels - nSpaces;

      /* transmit widths, margins, borders, and paddings */
      if (ibox2->BxNChars == 0)
        {
          ibox2->BxW = 0;
          ibox2->BxNSpaces = 0;
        }
      else
        ibox2->BxW = oldWidth - width - lostPixels * spaceWidth;
      ibox2->BxWidth = ibox2->BxW + r + pBox->BxRMargin + pBox->BxRBorder + pBox->BxRPadding;
      ibox2->BxTMargin = pBox->BxTMargin;
      ibox2->BxTBorder = pBox->BxTBorder;
      ibox2->BxTPadding = pBox->BxTPadding;
      ibox2->BxBMargin = pBox->BxBMargin;
      ibox2->BxBBorder = pBox->BxBBorder;
      ibox2->BxBPadding = pBox->BxBPadding;
      ibox2->BxRMargin = pBox->BxRMargin;
      ibox2->BxRBorder = pBox->BxRBorder;
      ibox2->BxRPadding = pBox->BxRPadding;

      /* update the chain of leaf boxes */
      ibox2->BxPrevious = pBox;
      ibox2->BxNext = pNextBox;
      if (pNextBox)
        pNextBox->BxPrevious = ibox2;
      else
        pRootAb->AbBox->BxPrevious = ibox2;
      ibox2->BxNexChild = pBox->BxNexChild;
      pBox->BxNexChild = ibox2;
      pBox->BxNext = ibox2;
    }
  return TRUE;
}


/*----------------------------------------------------------------------
  BreakMainBox splits a main box into two pieces.
  When force is TRUE splits on any character, else only splits on a space.
  max = the maximum width of the first piece.
  l and r give extra left and right margins generated by enclosing ghosts.
  line_spaces gives the current number of spaces in the current line
  pRootAb = the root abstract box for updating the chain of leaf boxes.
  hyphenate is TRUE when the word hyphenation is allowed
  Return TRUE if the whole box can be inserted in the line with compression.
  ----------------------------------------------------------------------*/
static ThotBool BreakMainBox (PtrLine pLine, PtrBox pBox, int max, int l,
                              int r, int  line_spaces, ThotBool hyphenate,
                              PtrAbstractBox pRootAb, ThotBool force)
{
  PtrBox              ibox1, ibox2;
  PtrBox              pPreviousBox, pNextBox;
  PtrTextBuffer       pNewBuff;
  PtrAbstractBox      pAb;
  SpecFont            font;
  int                 baseline, width;
  int                 newIndex, height;
  int                 spaceWidth, lostPixels;
  int                 nSpaces, length;
  
  if (pBox->BxType == BoScript)
    return BreakPieceOfBox (pLine, pBox, max, l, r, line_spaces, hyphenate, pRootAb);
  pAb = pBox->BxAbstractBox;
  height = pBox->BxHeight;
  baseline = pBox->BxHorizRef;
  pPreviousBox = pBox->BxPrevious;
  pNextBox = pBox->BxNext;
  font = pBox->BxFont;

  /* search a break */
  lostPixels = SearchBreak (pLine, pBox, max, font, l, r, line_spaces,hyphenate,
                            &length, &width, &nSpaces, &newIndex, &pNewBuff);

  if (lostPixels <= 0)
    /* don't break on a space */
    spaceWidth = 0;
  else
    /* breakmak on a space */
    spaceWidth = BoxCharacterWidth (SPACE, 1, font);

  /*
   * Generate two pieces:
   * - if SearchBreak found a break point on a space (lostPixels > 0)
   *   or on a character with generation of an hyphen (lostPixels = -1)
   *   or without (lostPixels = -2);
   * - if the boolean force is TRUE;
   * - if we want to remove extra spaces at the end of the box.
   */
  if (pNewBuff && length < pBox->BxNChars &&
      (length > 0 || pLine == NULL || pBox != pLine->LiFirstBox) &&
      (lostPixels != 0 || force) &&
      (pBox->BxWidth != max || lostPixels != pBox->BxNSpaces))
    {
      ibox1 = GetBox (pAb);
      ibox2 = GetBox (pAb);
    }
  else
    {
      ibox1 = NULL;
      ibox2 = NULL;
      /* TRUE if the whole box can is inserted in the line */
      return (length == pBox->BxNChars);
    }
  
  if (lostPixels == -2)
    /* don't generate the hyphen */
    lostPixels = 0;
  
  if (ibox1 && ibox2)
    {
      /* Initialize the first piece */
      ibox1->BxScript =  pBox->BxScript;
      ibox1->BxIndChar = 0;
      ibox1->BxContentWidth = TRUE;
      ibox1->BxContentHeight = TRUE;
      ibox1->BxFont = font;
      ibox1->BxUnderline = pBox->BxUnderline;
      ibox1->BxThickness = pBox->BxThickness;
      ibox1->BxH = pBox->BxH;
      ibox1->BxHeight = height;
      /* default position */
      ibox1->BxXOrg = pBox->BxXOrg;
      ibox1->BxYOrg = pBox->BxYOrg;
      ibox1->BxHorizRef = baseline;

      /* transmit widths, margins, borders, and paddings */
      ibox1->BxW = width;
      ibox1->BxWidth = width + l + pBox->BxLBorder + pBox->BxLPadding;
      if (pBox->BxLMargin > 0)
        ibox1->BxWidth += pBox->BxLMargin;
      ibox1->BxTMargin = pBox->BxTMargin;
      ibox1->BxTBorder = pBox->BxTBorder;
      ibox1->BxTPadding = pBox->BxTPadding;
      ibox1->BxBMargin = pBox->BxBMargin;
      ibox1->BxBBorder = pBox->BxBBorder;
      ibox1->BxBPadding = pBox->BxBPadding;
      ibox1->BxLMargin = pBox->BxLMargin;
      ibox1->BxLBorder = pBox->BxLBorder;
      ibox1->BxLPadding = pBox->BxLPadding;
      ibox1->BxBuffer = pBox->BxBuffer;
      ibox1->BxNChars = length;
      ibox1->BxNSpaces = nSpaces;
      ibox1->BxFirstChar = pBox->BxFirstChar;

      if (lostPixels == -1)
        {
          /* add the hyphen at the end */
          ibox1->BxType = BoDotted;
          width -= BoxCharacterWidth (173, 1, font);
          lostPixels = 0;
        }
      else
        ibox1->BxType = BoPiece;
      
      /* Initialize the second piece */
      ibox2->BxScript =  pBox->BxScript;
      ibox2->BxContentWidth = TRUE;
      ibox2->BxContentHeight = TRUE;
      ibox2->BxH = pBox->BxH;
      ibox2->BxHeight = height;
      /* default position */
      ibox2->BxXOrg = pBox->BxXOrg + width;
      ibox2->BxYOrg = pBox->BxYOrg;
      ibox2->BxFont = font;
      ibox2->BxUnderline = pBox->BxUnderline;
      ibox2->BxThickness = pBox->BxThickness;
      ibox2->BxHorizRef = baseline;
      ibox2->BxType = BoPiece;
      ibox2->BxBuffer = pNewBuff;
      ibox2->BxFirstChar = pBox->BxFirstChar + length + lostPixels;
      ibox2->BxIndChar = newIndex;
      ibox2->BxNChars = pBox->BxNChars - lostPixels - length;
      ibox2->BxNSpaces = pBox->BxNSpaces - lostPixels - nSpaces;

      /* transmit widths, margins, borders, and paddings */
      if (ibox2->BxNChars == 0)
        {
          /* that box is empty */
          ibox2->BxW = 0;
          ibox2->BxNSpaces = 0;
        }
      else
        ibox2->BxW = pBox->BxW - width - lostPixels * spaceWidth;      
      ibox2->BxWidth = ibox2->BxW + r + pBox->BxRBorder + pBox->BxRPadding;
      if (pBox->BxRMargin > 0)
        ibox2->BxWidth += pBox->BxRMargin;
      ibox2->BxTMargin = pBox->BxTMargin;
      ibox2->BxTBorder = pBox->BxTBorder;
      ibox2->BxTPadding = pBox->BxTPadding;
      ibox2->BxBMargin = pBox->BxBMargin;
      ibox2->BxBBorder = pBox->BxBBorder;
      ibox2->BxBPadding = pBox->BxBPadding;
      ibox2->BxRMargin = pBox->BxRMargin;
      ibox2->BxRBorder = pBox->BxRBorder;
      ibox2->BxRPadding = pBox->BxRPadding;

      
      /* update the chain of leaf boxes */
      ibox1->BxPrevious = pPreviousBox;
      if (pPreviousBox != NULL)
        pPreviousBox->BxNext = ibox1;
      else
        pRootAb->AbBox->BxNext = ibox1;
      ibox1->BxNext = ibox2;
      ibox2->BxPrevious = ibox1;
      ibox1->BxNexChild = ibox2;
      ibox2->BxNexChild = NULL;
      ibox2->BxNext = pNextBox;
      if (pNextBox != NULL)
        pNextBox->BxPrevious = ibox2;
      else
        pRootAb->AbBox->BxPrevious = ibox2;
      
      /* Update the main box */
      pBox->BxType = BoSplit;
      pBox->BxNexChild = ibox1;
    }
  return TRUE;
}


/*----------------------------------------------------------------------
  AddBoxInLine adds a box in a line.
  ----------------------------------------------------------------------*/
static void AddBoxInLine (PtrBox pBox, int frame, PtrBox pBlock, PtrLine pLine,
                          int *descent, int *ascent)
{
  int t, b, l, r;

  l = b = r = t = 0;
  GetExtraMargins (pBox, frame, TRUE, &t, &b, &l, &r);
  pLine->LiRealLength += pBox->BxWidth;
  /* check if the line includes a compound box or an image */
  if (pBox->BxAbstractBox &&
      (pBox->BxAbstractBox->AbLeafType == LtCompound ||
       pBox->BxAbstractBox->AbEnclosing->AbBox != pBlock ||
       pBox->BxAbstractBox->AbLeafType == LtPicture))
    pLine->LiNoOverlap = TRUE;
  /* Compute the current line height */
  if (*ascent < pBox->BxHorizRef + t)
    *ascent = pBox->BxHorizRef + t;
  if (*descent < pBox->BxHeight - pBox->BxHorizRef + b)
    *descent = pBox->BxHeight - pBox->BxHorizRef + b;
}


/*----------------------------------------------------------------------
  AddAllBoxesInLine links all boxes with the line.
  ----------------------------------------------------------------------*/
static void AddAllBoxesInLine (PtrBox pBox, int frame, PtrBox pBlock,
                               PtrAbstractBox pRootAb, PtrLine pLine,
                               ThotBool notComplete, ThotBool *full)
{
  PtrBox              pNextBox;
  int                 ascent, descent, maxLength;
  int                 t, b, l, r, line_spaces;
  ThotBool            still, hyphenate;

  /* add boxes into the line */
  if (pLine->LiFirstPiece)
    pNextBox = pLine->LiFirstPiece;
  else
    pNextBox = pLine->LiFirstBox;

  ascent = 0;
  descent = 0;
  line_spaces = 0;
  hyphenate = pBlock->BxAbstractBox->AbHyphenate;
  if (pBox)
    {
      /* there is almost a box set in line */
      if (pBox->BxType == BoSplit || pBox->BxType == BoMulScript)
        pBox = pBox->BxNexChild;
      still = TRUE;
      while (pNextBox && still)
        {
          if (pNextBox->BxType == BoSplit || pNextBox->BxType == BoMulScript)
            pNextBox = pNextBox->BxNexChild;
          if (pNextBox->BxAbstractBox->AbFloat == 'N' &&
              !ExtraFlow (pNextBox, frame) &&
              (!pNextBox->BxAbstractBox->AbNotInLine ||
               pNextBox->BxAbstractBox->AbDisplay != 'U'))
            {
              AddBoxInLine (pNextBox, frame, pBlock, pLine, &descent, &ascent);
              if (pNextBox->BxAbstractBox->AbLeafType == LtText)
                line_spaces += pNextBox->BxNSpaces;
            }
          if (pNextBox == pBox)
            still = FALSE;
          else
            {
              if (pNextBox->BxAbstractBox->AbLeafType == LtText &&
                  pNextBox->BxNexChild)
                /* get the next child */
                pNextBox = pNextBox->BxNexChild;
              else
                {
                  pNextBox = GetNextBox (pNextBox->BxAbstractBox, frame);
                  if (pNextBox == NULL)
                    still = FALSE;
                }
            }
        }

      /* complete the chain */
      if (pBox->BxType == BoPiece ||
          pBox->BxType == BoScript ||
          pBox->BxType == BoDotted)
        {
          pLine->LiLastPiece = pBox;
          pLine->LiLastBox = pBox->BxAbstractBox->AbBox;
        }
      else
        pLine->LiLastBox = pBox;
  
      /* teste s'il reste des boites a mettre en ligne */
      if ((pBox->BxAbstractBox->AbLeafType != LtText || pBox->BxNexChild == NULL) &&
          GetNextBox (pBox->BxAbstractBox, frame) == NULL)
        *full = FALSE;

      /* ignore spaces at the end of the line */
      if ((notComplete || *full) &&
          pBox->BxAbstractBox->AbLeafType == LtText &&
          pBox->BxNSpaces != 0)
        {
          GetExtraMargins (pBox, frame, FALSE, &t, &b, &l, &r);
          if (pLine->LiLastPiece == NULL)
            {
              maxLength = pBox->BxWidth;
              /*coupure sur un caractere refusee */
              BreakMainBox (pLine, pBox, maxLength, l, r, line_spaces,
                            hyphenate, pRootAb, FALSE);
              if (pBox->BxNexChild)
                {
                  if (pBox->BxType != BoScript)
                    /* take the first child of a main box */
                    pBox = pBox->BxNexChild;
                  /* remplace la boite entiere par la boite de coupure */
                  pLine->LiRealLength = pLine->LiRealLength - maxLength + pBox->BxWidth;
                  pLine->LiLastPiece = pBox;
                }
            }
          else if (pLine->LiLastPiece->BxNexChild == NULL)
            {
              pBox = pLine->LiLastPiece;
              maxLength = pBox->BxWidth;
              BreakPieceOfBox (pLine, pBox, maxLength, l, r, line_spaces,
                               hyphenate, pRootAb);
              /* met a jour la largeur de la ligne */
              pLine->LiRealLength = pLine->LiRealLength - maxLength + pBox->BxWidth;
            }
        }
    }
  else
    {
      pLine->LiLastPiece = pLine->LiFirstPiece;
      pLine->LiLastBox = pLine->LiFirstBox;
    }
  /* Calcule la hauteur et la base de la ligne */
  pLine->LiHeight = descent + ascent;
  pLine->LiHorizRef = ascent;
}

/*----------------------------------------------------------------------
  ClearFloats
  ----------------------------------------------------------------------*/
void ClearFloats (PtrBox pBox)
{
  PtrFloat            pfloat;

  if (pBox &&
      (pBox->BxType == BoBlock ||
       pBox->BxType == BoFloatBlock || pBox->BxType == BoCellBlock))
    {
      /* free floating contexts */
      while (pBox->BxLeftFloat)
        {
          pfloat = pBox->BxLeftFloat;
          pBox->BxLeftFloat = pfloat->FlNext;
          TtaFreeMemory (pfloat);
        }
      while (pBox->BxRightFloat)
        {
          pfloat = pBox->BxRightFloat;
          pBox->BxRightFloat = pfloat->FlNext;
          TtaFreeMemory (pfloat);
        }
    }
}


/*----------------------------------------------------------------------
  ClearAFloat
  Remove the left or a right floated box
  ----------------------------------------------------------------------*/
void ClearAFloat (PtrAbstractBox pAb)
{
  PtrFloat            pfloat, pPrev;
  PtrBox              box;
  ThotBool            left;

  if (pAb && pAb->AbBox)
    {
      box = pAb->AbBox;
      left = pAb->AbFloat == 'L';
      /* look for the floated context in enclosing block */
      pAb = pAb->AbEnclosing;
      while (pAb && pAb->AbBox &&
             pAb->AbBox->BxType != BoBlock &&
             pAb->AbBox->BxType != BoFloatBlock &&
             pAb->AbBox->BxType != BoCellBlock)
        pAb = pAb->AbEnclosing;
      if (pAb && pAb->AbBox)
        {
          pPrev = NULL;
          if (left)
            pfloat = pAb->AbBox->BxLeftFloat;
          else
            pfloat = pAb->AbBox->BxRightFloat;
          while (pfloat)
            {
              if (pfloat->FlBox == box)
                {
                  /* the box is found */
                  if (pPrev)
                    pPrev->FlNext = pfloat->FlNext;
                  else if (left)
                    pAb->AbBox->BxLeftFloat = pfloat->FlNext;
                  else
                    pAb->AbBox->BxRightFloat = pfloat->FlNext;
                  if (pfloat->FlNext)
                    pfloat->FlNext->FlPrevious = pPrev;
                  TtaFreeMemory (pfloat);
                  return;
                }
              else
                {
                  pPrev = pfloat;
                  pfloat = pfloat->FlNext;
                }
            }
        }
    }
}


/*----------------------------------------------------------------------
  SetClear returns clearL and clearR of the box.
  ----------------------------------------------------------------------*/
static void SetClear (PtrBox box, ThotBool *clearL, ThotBool *clearR)
{
  PtrAbstractBox      pAb, pParent, pChild;

  *clearL = FALSE;
  *clearR = FALSE;
  if (box && box->BxAbstractBox)
    {
      pAb = box->BxAbstractBox;
      *clearL = (pAb->AbClear == 'L' || pAb->AbClear == 'B');
      *clearR =(pAb->AbClear == 'R' || pAb->AbClear == 'B');
      // check also the first child
      if (pAb->AbLeafType == LtCompound && pAb->AbFirstEnclosed &&
          // don't check children of a floated box
          pAb->AbFloat == 'N')
        {
          pChild = pAb->AbFirstEnclosed;
          while (pChild->AbPresentationBox && pChild->AbNext)
            pChild = pChild->AbNext;
          if (pChild->AbClear == 'L' || pChild->AbClear == 'B')
            *clearL = TRUE;
          if (pChild->AbClear == 'R' || pChild->AbClear == 'B')
            *clearR = TRUE;          
        }
      /* check if an enclosing ghost box generates a clear */
      pParent = pAb->AbEnclosing;
      while (pParent && pParent->AbBox &&
             (pParent->AbBox->BxType == BoGhost ||
              pParent->AbBox->BxType == BoStructGhost ||
              pParent->AbBox->BxType == BoFloatGhost) &&
             pParent->AbElement &&
             pParent->AbElement->ElFirstChild == pAb->AbElement)
        {
          if (pParent->AbClear == 'L' || pParent->AbClear == 'B')
            *clearL = TRUE;
          if (pParent->AbClear == 'R' || pParent->AbClear == 'B')
            *clearR = TRUE;
          pAb = pParent;
          pParent = pAb->AbEnclosing;	
        }
    }
}

/*----------------------------------------------------------------------
  InitLine computes the x,y position and the max width of the line pLine
  in the block of lines pBlock.
  Parameters top, bottom, left, and right give the space reserved by
  margins/borders/paddings of the block.
  Parameters floatL and floatR point to last left an right floating
  boxes in the current block.
  The parameter pBox points the next box to be inserted.
  Work with absolute positions when xAbs and yAbs are TRUE.
  ----------------------------------------------------------------------*/
static void InitLine (PtrLine pLine, PtrBox pBlock, int frame, int indent,
                      PtrBox floatL, PtrBox floatR, PtrBox pBox,
                      int top, int bottom, int left, int right,
                      ThotBool xAbs, ThotBool yAbs)
{
  PtrFloat            pfloat = NULL;
  PtrAbstractBox      pAb;
  PtrBox              box, orgBox = pBox;
  int                 bottomL = 0, bottomR = 0, ml = 0, mr = 0;
  int                 orgX, orgY, width, by = 0, bh = 0, bw;
  int                 newOrg, newMax;
  ThotBool            clearL, clearR;
  ThotBool            clearl, clearr;
  ThotBool            variable, newFloat, still;
#define MIN_SPACE 20

  if (pLine == NULL)
    return;
  /* clear values */
  newFloat = FALSE;
  SetClear (pBox, &clearL, &clearR);
  /* relative line positions */
  orgX = 0;
  orgY = 0;
  if (xAbs)
    orgX += pBlock->BxXOrg;
  if (yAbs)
    orgY += pBlock->BxYOrg;
  if (pBox)
    {
      box = pBox;
      /* ignore invisible or floated boxes */
      while (box)
        {
          pAb = box->BxAbstractBox;
          still = (pAb &&
                   (pAb->AbVisibility < ViewFrameTable[frame - 1].FrVisibility ||
                    pAb->AbNotInLine || pAb->AbBox == NULL ||
                    (pAb->AbPresentationBox && pAb->AbTypeNum == 0 &&
                     pAb->AbHorizPos.PosAbRef && pAb->AbHorizPos.PosAbRef->AbFloat != 'N')));
          // skip not inline bullets
          if (still)
            box = GetNextBox (pAb, frame);
          else if (pAb &&
                   (pAb->AbFloat == 'L' || pAb->AbFloat == 'R' ||
                    ExtraFlow (box, frame)))
            pBox = box = NULL;
          else
            {
              if (box && pLine->LiFirstBox == pBox)
                /* update the line */
                pLine->LiFirstBox = pBox;
              pBox = box;
              box = NULL;
            }
        }

      if (pBox == NULL && orgBox)
        // only floated boxes are displayed in the block
        pBox = orgBox;
      if (pBox)
        {
          pAb = pBox->BxAbstractBox;
          if (pAb)
            newFloat = pAb->AbFloat != 'N';
        }
      else
        pAb = NULL;
    } 
  else
    pAb = NULL;
  /* check if a clear is requested */
  SetClear (pBox, &clearl, &clearr);
  /* the clear could be generated by a floated box */
  clearL = clearL | clearl;
  clearR = clearR | clearr;
  variable = HasVariableWidth (pBox, pBlock);
  /* minimum width needed to format the line */
  if (pBox && pAb && pAb->AbFloat == 'N')
    {
      if (pBox->BxLMargin > 0)
        ml = pBox->BxLMargin;
      if (pBox->BxRMargin > 0)
        mr = pBox->BxRMargin;
    }
  if (!pBox)
    width = 0;
  else if (variable)
    {
      width = pBox->BxMinWidth - ml - mr;
      if (width == 0)
        {
        if (pBox->BxW > MIN_SPACE)
          width = MIN_SPACE;
        else
          width = pBox->BxW;
        }
    }
  else if (pBox->BxType == BoTable)
    // min width includes margins
    width = pBox->BxMinWidth - ml - mr;
  else if (!variable || pBox->BxW < MIN_SPACE)
    width = pBox->BxW;
  else if (pBox->BxMinWidth)
    // min width includes margins
    width = pBox->BxMinWidth - ml - mr;
  else
    width = MIN_SPACE;

  /* by default */
  pLine->LiXOrg = left + indent;
  pLine->LiXMax = pBlock->BxW - pLine->LiXOrg + left;
  newOrg = left + indent;
  newMax = pBlock->BxW - pLine->LiXOrg + left;
  bottomL = bottomR = pLine->LiYOrg;
  if (floatL)
    {
      // look for the last left float
      pfloat = pBlock->BxLeftFloat;
      while (pfloat && pfloat->FlBox != floatL)
        pfloat = pfloat->FlNext;
      // check all previous left float to get the max width and bottom
      while (pfloat && pfloat->FlBox)
        {
          floatL = pfloat->FlBox;
          by = floatL->BxYOrg;
          bh = 0;
          if (floatL->BxTMargin < 0)
            by += floatL->BxTMargin;
          else
            bh += floatL->BxTMargin;
          // get extra margins of floated box (XTiger boxes)
          bh += floatL->BxTBorder + floatL->BxTPadding + floatL->BxH + floatL->BxBBorder + floatL->BxBPadding;
          
          if ((pLine->LiYOrg + orgY >= by &&
               pLine->LiYOrg + orgY < by + bh) ||
              (pLine->LiYOrg + pLine->LiHeight + orgY > by &&
               pLine->LiYOrg + pLine->LiHeight + orgY <= by + bh))
            {
              /* line at the right of the current left float */
              bw = floatL->BxWidth;
              // a negative right margin increases the line width
              if (floatL->BxRMargin < 0)
                bw += floatL->BxRMargin;
              else
                bw -= floatL->BxRMargin;
              if (floatL->BxLMargin > 0)
                bw -= floatL->BxLMargin;
              bw = bw + floatL->BxXOrg + indent - orgX;
              if (newOrg < bw)
                newOrg = bw;
              if (bottomL < by + bh - orgY)
                bottomL = by + bh - orgY;
            }
          else if (bottomL < floatL->BxYOrg + floatL->BxHeight - orgY)
            bottomL = floatL->BxYOrg + floatL->BxHeight - orgY;
          pfloat = pfloat->FlPrevious;
        }
    }

  if (floatR)
    {
      pfloat = pBlock->BxRightFloat;
      while (pfloat && pfloat->FlBox != floatR)
        pfloat = pfloat->FlNext;
      // check all previous left float to get the max width and bottom
      while (pfloat && pfloat->FlBox)
        {
          floatR = pfloat->FlBox;
          by = floatR->BxYOrg;
          bh = 0;
          if (floatR->BxTMargin < 0)
            by += floatR->BxTMargin;
          else
            bh += floatR->BxTMargin;
          // get extra margins of floated box (XTiger boxes)
          bh += floatR->BxTBorder + floatR->BxTPadding + floatR->BxH + floatR->BxBBorder + floatR->BxBPadding;

          if ((pLine->LiYOrg + orgY >= by &&
               pLine->LiYOrg + orgY < by + bh) ||
              (pLine->LiYOrg + pLine->LiHeight + orgY > by &&
               pLine->LiYOrg + pLine->LiHeight + orgY <= by + bh))
            {
              /* line extended to the left edge of the current right float */
              bw = floatR->BxXOrg - newOrg - orgX;
              // a negative right margin doesn't increases the line width
              if (floatR->BxLMargin  < 0)
                bw -= floatR->BxLMargin;
              else
                // ignore the margin
                bw += floatR->BxLMargin;
              if (newMax > bw)
                newMax = bw;
              if (bottomR < by + bh - orgY)
                bottomR = by + bh - orgY;
            }
          else if (bottomR < floatR->BxYOrg + floatR->BxHeight - orgY)
            bottomR = floatR->BxYOrg + floatR->BxHeight - orgY;
          pfloat = pfloat->FlPrevious;
        }
    }

  /* compute the line position and width */
  if (pLine->LiXOrg < newOrg)
    pLine->LiXOrg = newOrg;
  /* keep the CSS2 minimun of margins and the current shift */
  if (pLine->LiXOrg < left)
    pLine->LiXOrg = left;
  if (pLine->LiXMax > newMax)
    pLine->LiXMax = newMax;
  /* keep the CSS2 minimun of margins and the current shift */
  if (pLine->LiXOrg + pLine->LiXMax > left + pBlock->BxW)
    pLine->LiXMax = pBlock->BxW - pLine->LiXOrg + left;
  
  if (newFloat)
    /* let SetFloat find the right position of this new floating box */
    return;
  /* check if there is enough space between left and right floating boxes */
  if ((floatL || floatR) &&
      ((width > 0 && pLine->LiXMax < width) ||
       (floatL && floatL != pBox && clearL) ||
       (floatR && floatR != pBox && clearR)))
    {
      /* update line information */
      int delta = pLine->LiXOrg - left - indent;
      if (clearL || clearR)
        {
          if (clearL && pLine->LiYOrg < bottomL)
            {
              pLine->LiYOrg = bottomL;
              pLine->LiXOrg = left + indent;
              pLine->LiXMax += delta;
            }
          if (clearR && pLine->LiYOrg < bottomR)
            {
              pLine->LiYOrg = bottomR;
              pLine->LiXMax = pBlock->BxW - pLine->LiXOrg;
            }
        }
      else
        {
          /* not enough space: move to the first bottom */
          if (pLine->LiYOrg < bottomL && bottomL <= bottomR)
            {
              pLine->LiYOrg = bottomL;
              pLine->LiXOrg = left + indent;
              pLine->LiXMax += delta;
            }
          else if (pLine->LiYOrg < bottomR)
            {
              pLine->LiYOrg = bottomR;
              pLine->LiXMax = pBlock->BxW - pLine->LiXOrg;
            }
          if (pLine->LiXMax < width)
            {
              // still not enough space
              if (pLine->LiYOrg < bottomL)
                {
                  pLine->LiYOrg = bottomL;
                  pLine->LiXOrg = left + indent;
                  pLine->LiXMax += delta;
                }
              else if (pLine->LiYOrg < bottomR)
                {
                  pLine->LiYOrg = bottomR;
                  pLine->LiXMax = pBlock->BxW - pLine->LiXOrg;
                }
            }
        }
      
    }
}


/*----------------------------------------------------------------------
  GetEnclosingBlock returns the enclosing block of the abstract box pAb
  ----------------------------------------------------------------------*/
static PtrAbstractBox GetEnclosingBlock (PtrAbstractBox pAb, PtrAbstractBox block)
{
  PtrAbstractBox pRefBlock;

  if (pAb == NULL)
    return block;

  pRefBlock = pAb->AbEnclosing;
  //if (skipGhost)
  //  while (pRefBlock != block && pRefBlock->AbBox->BxType == BoGhost)
  //    pRefBlock = pRefBlock->AbEnclosing;
  while (pRefBlock != block && pRefBlock->AbDisplay != 'B')
    pRefBlock = pRefBlock->AbEnclosing;
  return pRefBlock;
}


/*----------------------------------------------------------------------
  IsFloatSet returns TRUE if the floating box of the block of lines
  pBlock is already managed.
  ----------------------------------------------------------------------*/
static ThotBool IsFloatSet (PtrBox box, PtrBox floatBox, PtrBox pBlock)
{
  PtrFloat            pfloat;

  if (floatBox == NULL)
    return FALSE;
  else if (box == floatBox)
    return TRUE;
  if (box->BxAbstractBox->AbFloat == 'L')
    pfloat = pBlock->BxLeftFloat;
  else
    pfloat = pBlock->BxRightFloat;
  while (pfloat && pfloat->FlBox != box && pfloat->FlBox != floatBox)
    pfloat = pfloat->FlNext;
  return (pfloat && pfloat->FlBox == box);
}


/*----------------------------------------------------------------------
  Checknewblock returns TRUE when pBox and pNextBox are in different blocks
 ----------------------------------------------------------------------*/
static ThotBool Checknewblock (PtrBox pBox, PtrBox pNextBox, PtrBox pBlock, int frame)
{
  PtrAbstractBox      currentBlock, nextBlock;

  if (pBox == NULL || pBox->BxAbstractBox == NULL ||
      pNextBox == NULL || pNextBox->BxAbstractBox == NULL)
  return FALSE;

  if ((pBox->BxAbstractBox->AbLeafType == LtCompound &&
       (pBox->BxAbstractBox->AbDisplay == 'B' ||
        //pBox->BxAbstractBox->AbInLine ||
        pBox->BxType == BoTable)) ||
      (pNextBox->BxAbstractBox->AbLeafType == LtCompound &&
       (pNextBox->BxAbstractBox->AbDisplay == 'B' ||
        //pNextBox->BxAbstractBox->AbInLine ||
        pNextBox->BxType == BoTable)))
    /* only one compound block by line */
    return TRUE;
/*   if (pBox->BxAbstractBox->AbFloat == 'N' && */
/*       !ExtraFlow (pBox, frame) && */
/*       pNextBox->BxAbstractBox->AbFloat == 'N' && */
/*       !ExtraFlow (pNextBox, frame)) */
    {
      // get current and next block
      nextBlock = pBlock->BxAbstractBox;
      currentBlock = GetEnclosingBlock (pBox->BxAbstractBox, nextBlock);
      nextBlock = GetEnclosingBlock (pNextBox->BxAbstractBox, nextBlock);
      if (currentBlock != nextBlock &&
          (currentBlock->AbDisplay == 'B' || nextBlock->AbDisplay == 'B'))
        /* only one compound block by line */
        return TRUE;
    }
  return FALSE;
} 

/*----------------------------------------------------------------------
  FillLine fills a line pLine of a block of lines pBlock with enclosed boxes.
  Fields LiFirstBox and LiFirstPiece must be set by the caller.
  Fields LiLastBox, LiLastPiece will be set by the function.
  Parameters floatL and floatR point to last left an right floating
  boxes managed in the current block.
  Parameter maxLineWidth gives the max width of the line.
  Parameters top, bottom, left, and right give the summ of
  margin/border/padding of the block.
  Work with absolute positions when xAbs and yAbs are TRUE.
  When the parameter extensibleBlock is TRUE, it doesn't take into account
  the current max with.
  The parameter onlySpace is TRUE when the cut is possible only on a space.
  Returns:
  - the minimum width of the line (the larger word).
  - full = TRUE if the line is full.
  - adjust = TRUE if the line could be justified.
  - notComplete = TRUE if all enclosed boxes are not managed yet.
  - breakLine = TRUE if the end of the line correspond to a break element.
  - newblock = TRUE if a beginning or the end of a block is detected
  ----------------------------------------------------------------------*/
static int FillLine (PtrLine pLine, PtrBox first, PtrBox pBlock,
                     PtrAbstractBox pRootAb, int maxWidth,
                     ThotBool extensibleBlock, ThotBool xAbs, ThotBool yAbs,
                     ThotBool notComplete, ThotBool onlySpace, ThotBool *full,
                     ThotBool *adjust, ThotBool *breakLine, ThotBool *newblock,
                     int frame, int indent, int top, int bottom, int left, int right,
                     PtrBox *floatL, PtrBox *floatR)
{
  PtrTextBuffer       pNewBuff;
  PtrAbstractBox      pAbRef, pRefBlock;
  PtrBox              pBox, pNextBox, lastbox;
  int                 line_spaces;
  int                 width, breakWidth, w;
  int                 boxLength, nSpaces;
  int                 newIndex, wordWidth;
  int                 xi, val, cutwidth, shift;
  int                 maxLength, minWidth, ml, mr;
  int                 t = 0, b = 0, l = 0, r = 0;
  ThotBool            still, toCut, found, hyphenate;
  ThotBool            clearL, clearR, setinline, variable;

  *adjust = TRUE;
  *breakLine = FALSE;
  *newblock = FALSE;
  *full = FALSE;
  toCut = FALSE;
  found = FALSE;
  still = FALSE;
  minWidth = 0;
  wordWidth = 0;
  xi = pLine->LiRealLength;
  line_spaces = 0;
  hyphenate = pBlock->BxAbstractBox->AbHyphenate;
  /* the first managed box */
  pNextBox = first;
  /* evaluate the temporary height of the line */
  if (pNextBox && pNextBox->BxAbstractBox && pNextBox->BxWidth &&
      pNextBox->BxAbstractBox->AbFloat == 'N' && !ExtraFlow (pNextBox, frame) &&
      pNextBox->BxType != BoFloatGhost)
    pLine->LiHeight = pNextBox->BxHeight;
  InitLine (pLine, pBlock, frame, indent, *floatL, *floatR, pNextBox,
            top, bottom, left, right, xAbs, yAbs);
  pLine->LiLastPiece = NULL;
  if (pLine->LiFirstPiece)
    pBox = pLine->LiFirstPiece;
  else
    pBox = pLine->LiFirstBox;
  if (pNextBox)
    {
      /* pNextBox is the current box we're managing */
      /* pBox is the last box added to the line */
      *full = TRUE;
      still = TRUE;
      if (pLine->LiFirstPiece == NULL &&
          (pNextBox->BxType == BoSplit || pNextBox->BxType == BoMulScript))
        pLine->LiFirstPiece = pNextBox->BxNexChild;
      if (pLine->LiFirstPiece &&
          /* and belong to the current box */
          pLine->LiFirstPiece->BxAbstractBox == pNextBox->BxAbstractBox)
        {
          /* the first piece must be inserted in the line */
          pBox = pLine->LiFirstPiece;
          GetExtraMargins (pBox, frame, FALSE, &t, &b, &l, &r);
          /* look for a break element */
          found = FindBreakLine (pBox, &width, &breakWidth, &boxLength,
                                 &nSpaces, &newIndex, &pNewBuff, &wordWidth);
          if (found && width + xi <= pLine->LiXMax)
            {
              /* the break occurs before the end of the line */
              *adjust = FALSE;
              still = FALSE;
              *breakLine = TRUE;
              ManageBreakLine (pBox, width, breakWidth, boxLength,
                               nSpaces, newIndex, l, r, pNewBuff, pRootAb);
            }
          else if (pBox->BxWidth + xi <= pLine->LiXMax)
            {
              if (pBox->BxNexChild)
                {
                  /* get the next child */
                  pNextBox = pBox->BxNexChild;
                  if (pNextBox && pNextBox->BxType == BoPiece)
                    {
                      *full = TRUE;
                      still = FALSE;
                    }
                }
              else
                {
                  /* the whole box can be inserted in the line */
                  pNextBox = GetNextBox (pBox->BxAbstractBox,frame);
                  if (pNextBox == NULL)
                    {
                      *full = FALSE;
                      still = FALSE;
                    }
                  else
                    {
                      *newblock = Checknewblock (pBox, pNextBox, pBlock, frame);
                      if (*newblock)
                        {
                          /* only one compound block by line */
                          *full = TRUE;
                          still = FALSE;
                          *adjust = FALSE;
                        }
                    }
                }
            }
          else
            {
              /* We need to split again that box */
              still = FALSE;
              BreakPieceOfBox (pLine, pBox, pLine->LiXMax - xi, l, r, line_spaces,
                               hyphenate, pRootAb);
            }
          pBox = pLine->LiFirstPiece;
          xi += pBox->BxWidth;
          line_spaces += pBox->BxNSpaces;
          /* take into account the minimum width */
          if (minWidth < wordWidth)
            minWidth = wordWidth;
          /* evaluate the temporary height of the line */
          if (pLine->LiHeight < pBox->BxHeight && pBox->BxWidth)
            pLine->LiHeight = pBox->BxHeight + t + b;
        }
    }

  /* look for a box to split */
  while (still)
    {
      val = 0;
      setinline = (pNextBox->BxAbstractBox->AbFloat == 'N' &&
                   !ExtraFlow (pNextBox, frame));
      // check if the width of the box can be adapted
      variable = HasVariableWidth (pNextBox, pBlock);
      //GetExtraMargins (pNextBox, frame, TRUE, &t, &b, &ml, &mr);
      GetLeftRightMargins (pNextBox, pBlock, frame, &ml, &mr);
      GetLeftRightPaddings (pNextBox, pBlock, &l, &r);
      if (setinline)
        {
          if (ml > 0 && pLine->LiXOrg < ml)
            {
              // shift the box position
              l = l + ml - pLine->LiXOrg;
              ml = 0;
            }
          l += pNextBox->BxLBorder;
          r += pNextBox->BxRBorder;
          shift = pBlock->BxW - pLine->LiXOrg - pLine->LiXMax;
          if (mr > 0 && shift < mr)
            {
              // reduce the size of the box
              r = r + mr - shift;
              mr = 0;
            }
          pRefBlock = GetEnclosingBlock (pNextBox->BxAbstractBox, pBlock->BxAbstractBox);
          if (pRefBlock != pBlock->BxAbstractBox && ml > 0)
            {
              // compute the virtual position of the box
              shift = ml + left - pLine->LiXOrg;
              pLine->LiXOrg += shift;
              pLine->LiXMax -= shift;
            }
        }
      if (pNextBox->BxAbstractBox->AbLeafType == LtCompound)
        {
          /* check if the width depends on the block width */
          pAbRef = pNextBox->BxAbstractBox->AbWidth.DimAbRef;
          if (variable)
            {
              /* just to be sure the line structure is coherent */
              pLine->LiLastBox = pLine->LiFirstBox;
              if (!extensibleBlock)
                {
                  // update parent ghost blocks if needed
                  if (setinline && pNextBox->BxAbstractBox->AbClear != 'B')
                    val = pLine->LiXMax;
                  else if (pNextBox->BxAbstractBox->AbEnclosing &&
                           pNextBox->BxAbstractBox->AbEnclosing->AbBox &&
                           pNextBox->BxAbstractBox->AbEnclosing->AbBox->BxType == BoStructGhost)
                    val = pNextBox->BxAbstractBox->AbEnclosing->AbBox->BxW;
                  else
                    val = pBlock->BxW;
                  if (pNextBox->BxAbstractBox->AbWidth.DimUnit == UnPercent)
                    {
                      // compute the external width
                      val = val * pNextBox->BxAbstractBox->AbWidth.DimValue / 100;
                      val = val + l + r;
                      if (setinline && val > pLine->LiXMax)
                        /* reduce the box width to the current line width */
                        val = pLine->LiXMax;
                      else if (pNextBox->BxAbstractBox->AbFloat != 'N' &&
                               pNextBox->BxAbstractBox->AbWidth.DimValue == 100 &&
                               pBlock->BxAbstractBox->AbFloat == 'N' &&
                               val > pNextBox->BxMaxWidth)
                        // sometimes the 100% is equivalent to auto
                        val = pNextBox->BxMaxWidth;
                    }
                  else if ((pNextBox->BxType == BoBlock ||
                            pNextBox->BxType == BoFloatBlock ||
                            pNextBox->BxType == BoCellBlock ||
                            pNextBox->BxType == BoTable) &&
                           (pNextBox->BxAbstractBox->AbDisplay == 'I' ||
                            pNextBox->BxAbstractBox->AbDisplay == 'b') &&
                           maxWidth < val)
                    {
                      /* use the max between the enclosed and the enclosing widths */
                      pNextBox->BxContentWidth = TRUE;
                      val =  maxWidth;
                    }

                  val = val - l - r;
                  if (pNextBox->BxShrink)
                    {
                      // new rule width
                      w = pNextBox->BxMaxWidth - l - r;
                      if (w > 0 && w < val)
                        {
                          // the box must be shrinked
                          pNextBox->BxContentWidth = FALSE;
                          ResizeWidth (pNextBox, pBlock, pBlock,
                                       w - pNextBox->BxW, 0, 0, 0, frame, FALSE);
                          pNextBox->BxRuleWidth = val;
                        }
                      else
                        ResizeWidth (pNextBox, pBlock, pBlock,
                                     val - pNextBox->BxW, 0, 0, 0, frame, FALSE);
                    }
                  else if (pNextBox->BxAbstractBox->AbWidth.DimAbRef ||
                           pNextBox->BxAbstractBox->AbWidth.DimUnit == UnPercent)
                    ResizeWidth (pNextBox, pBlock, pBlock,
                                 val - pNextBox->BxW, 0, 0, 0, frame, FALSE);
                  else if (pNextBox->BxAbstractBox->AbFloat == 'N' &&
                           val < pNextBox->BxW &&
                           pNextBox->BxAbstractBox->AbWidth.DimUnit == UnAuto)
                    ResizeWidth (pNextBox, pBlock, pBlock,
                                 val - pNextBox->BxW, 0, 0, 0, frame, FALSE);
                }
            }
          else if (pAbRef == NULL &&
                    pNextBox->BxAbstractBox->AbWidth.DimValue == -1 &&
                   (pNextBox->BxType == BoBlock ||
                    pNextBox->BxType == BoFloatBlock ||
                    pNextBox->BxType == BoCellBlock) &&
                   pNextBox->BxCycles == 0)
            // recheck as the max could changed
            RecomputeLines (pNextBox->BxAbstractBox, NULL, pBlock, frame);
        }

      /* check if a clear is requested */
      SetClear (pNextBox, &clearL, &clearR);
      if ((clearL && pBlock->BxLeftFloat && pLine->LiFirstBox != pNextBox) ||
          (clearR && pBlock->BxRightFloat && pLine->LiFirstBox != pNextBox))
        {
          /* report the floating box to the next line */
          *full = TRUE;
          still = FALSE;
        }
      else if (pNextBox->BxAbstractBox->AbFloat != 'N' ||
               ExtraFlow (pNextBox, frame) ||
               !pNextBox->BxAbstractBox->AbHorizEnclosing ||
               pNextBox->BxAbstractBox->AbNotInLine)
        {
          /* that box is not inline */
          if (pNextBox->BxAbstractBox->AbFloat != 'N' ||
              ExtraFlow (pNextBox, frame))
            {
              lastbox = pNextBox;
              /* dont register this floating box within the line */
              pNextBox = GetNextBox (lastbox->BxAbstractBox, frame);
              if (pNextBox &&
                  (pNextBox->BxType == BoSplit ||
                   pNextBox->BxType == BoMulScript))
                pNextBox = pNextBox->BxNexChild;
              if (lastbox == pLine->LiFirstBox)
                pLine->LiFirstBox = pNextBox;

              if ((lastbox->BxAbstractBox->AbFloat == 'L' &&
                   !IsFloatSet (lastbox, *floatL, pBlock)) ||
                  (lastbox->BxAbstractBox->AbFloat == 'R' &&
                   !IsFloatSet (lastbox, *floatR, pBlock)))
                {
                  pLine->LiRealLength = xi;
                  pLine->LiMinLength = minWidth;
                  /* handle a new floating box and rebuild the line */
                  return SetFloat (lastbox, pBlock, pLine, pRootAb,
                                   maxWidth, extensibleBlock, xAbs, yAbs,
                                   notComplete, onlySpace, full, adjust, breakLine,
                                   newblock, frame, indent, top, bottom, left, right,
                                   floatL, floatR);
                }
            }
          else if (pNextBox->BxAbstractBox->AbElement->ElTypeNumber == PageBreak + 1 ||
                   pNextBox->BxAbstractBox->AbDisplay != 'U')
            {
              /* allow a line to present this box */
              *full = TRUE;
              still = FALSE;
              wordWidth = 0;
              if (pBox == NULL)
                {
                  /* it's the first box in the line */
                  pLine->LiLastPiece = pLine->LiFirstPiece;
                  pBox = pNextBox;
                }
              else if (pBox->BxType == BoPiece ||
                       pBox->BxType == BoScript ||
                       pBox->BxType == BoDotted)
                /* break the last word of the previous box */
                pLine->LiLastPiece = pLine->LiFirstPiece;
            }
          else
            {
              /* skip over the box */
              pNextBox = GetNextBox (pNextBox->BxAbstractBox, frame);
              *newblock = Checknewblock (pBox, pNextBox, pBlock, frame);
              if (*newblock)
                {
                  /* only one compound block by line */
                  *full = TRUE;
                  still = FALSE;
                  *adjust = FALSE;
                }
            }
        }
      else
        {
          /* look for a break element */
          if (pNextBox->BxAbstractBox->AbLeafType == LtText &&
              pNextBox->BxAbstractBox->AbAcceptLineBreak)
            found = FindBreakLine (pNextBox, &width, &breakWidth, &boxLength,
                                   &nSpaces, &newIndex, &pNewBuff, &wordWidth);
          else
            found = FALSE;
	   
          if (found && width + xi <= pLine->LiXMax)
            {
              /* the break occurs before the end of the line */
              still = FALSE;
              *adjust = FALSE;
              *breakLine = TRUE;
              if (pNextBox->BxNChars == 1)
                /* only one character in the box: don't cut it */
                pBox = pNextBox;
              else
                {
                  GetExtraMargins (pNextBox, frame, FALSE, &t, &b, &l, &r);
                  ManageBreakLine (pNextBox, width, breakWidth, boxLength,
                                   nSpaces, newIndex, l, r, pNewBuff, pRootAb);
                  if (pNextBox->BxNexChild)
                    {
                      if (pNextBox == pLine->LiFirstBox)
                        {
                          /* a complete box is split */
                          pBox = pNextBox->BxNexChild;
                          pLine->LiFirstPiece = pBox;
                        }
                      else
                        pBox = pNextBox;
                    }
                  else
                    pBox = pNextBox;
                }
            }
          else if (pNextBox->BxWidth + xi <= pLine->LiXMax ||
                   // cannot split that unique compound box
                   (pNextBox->BxAbstractBox->AbLeafType == LtCompound &&
                    pNextBox->BxType == BoComplete &&
                    pNextBox == pLine->LiFirstBox) ||
                   (onlySpace && line_spaces == 0 &&
                   (pNextBox->BxAbstractBox->AbLeafType != LtText ||
                    pNextBox->BxNSpaces == 0)))
             {
               /* the whole box can or must be inserted in the line */
              if (pNextBox->BxType == BoBlock ||
                  pNextBox->BxType == BoFloatBlock ||
                  pNextBox->BxType == BoCellBlock||
                  pNextBox->BxType == BoTable)
                {
                  //if (!variable)
                  wordWidth = pNextBox->BxMinWidth;
                  // don't justify this line
                  //*adjust = TRUE;
                }
              else if (!pNextBox->BxAbstractBox->AbWidth.DimIsPosition &&
                       pNextBox->BxAbstractBox->AbHorizEnclosing &&
                       !variable)
                wordWidth = pNextBox->BxWidth;

              pBox = pNextBox;
              /* evaluate the temporary height of the line */
              if (pLine->LiHeight < pBox->BxHeight && pBox->BxWidth)
                pLine->LiHeight = pBox->BxHeight;
              if (pBlock->BxMaxWidth < pBox->BxMaxWidth)
                /* transmit the max width of the box */
                pBlock->BxMaxWidth = pBox->BxMaxWidth;
              /* dont'take into account the width of the bullet */
              if (!pBox->BxAbstractBox->AbPresentationBox ||
                  pBox->BxAbstractBox->AbTypeNum != 0)
                {
                  xi += pNextBox->BxWidth;
                  if (pNextBox->BxAbstractBox->AbLeafType == LtText)
                    line_spaces += pNextBox->BxNSpaces;
                }
              // check minwidth and maxwidth values
              if (onlySpace && line_spaces == 0)
                {
                  if (pLine->LiXMax < xi)
                    pLine->LiXMax = xi;
                  minWidth = xi;
                }

              if (pNextBox->BxAbstractBox->AbLeafType == LtText &&
                  pNextBox->BxNexChild)
                /* get the next child */
                pNextBox = pNextBox->BxNexChild;
              else
                pNextBox = GetNextBox (pNextBox->BxAbstractBox, frame);

              if (pBox->BxAbstractBox->AbPresentationBox &&
                  pBox->BxAbstractBox->AbTypeNum == 0)
                /* dont' cut just after a bullet */
                still = TRUE;
              else
                {
                  *newblock = Checknewblock (pBox, pNextBox, pBlock, frame);
                  if (*newblock)
                    {
                      if (pNextBox && pNextBox->BxAbstractBox->AbLeafType == LtText &&
                          pNextBox->BxNChars == pNextBox->BxNSpaces)
                        {
                          // include the next empty box in the line
                          pBox = pNextBox;
                          toCut = FALSE;
                          *adjust = TRUE;
                        }
                      else
                        {
                          /* only one compound block by line */
                          *full = TRUE;
                          still = FALSE;
                          *adjust = TRUE;
                        }
                    }
                  else if (!pBox->BxAbstractBox->AbElement->ElTerminal &&
                           (pBox->BxAbstractBox->AbElement->ElTypeNumber == 0 ||
                            pNextBox == NULL ||
                            (pNextBox->BxAbstractBox->AbLeafType == LtText &&
                             xi == pLine->LiXMax) ||
                            (pNextBox->BxAbstractBox->AbLeafType != LtText &&
                             pNextBox->BxWidth + xi > pLine->LiXMax)))
                    /* accept to cut the line here */ 
                    still = FALSE;
                }
            }
          else if ((pNextBox->BxType == BoBlock ||
                    pNextBox->BxType == BoFloatBlock))
            {
              /* don't try to split this box */
              pBox = pNextBox;
              minWidth = pBox->BxMinWidth + l + r;
              pNextBox = GetNextBox (pNextBox->BxAbstractBox, frame);
              if (pNextBox && pNextBox->BxAbstractBox->AbLeafType == LtText &&
                  pNextBox->BxNChars == pNextBox->BxNSpaces)
                {
                  // include the next empty box in the line
                  pBox = pNextBox;
                  toCut = FALSE;
                }
              else
                {
                  pNextBox = pBox;
                  toCut = TRUE;
                }
              still = FALSE;
            }
          else
            {
              /* We need to split that box or a previous one */
              toCut = TRUE;
              still = FALSE;
            }
        }

      if (pNextBox == NULL)
        {
          *full = FALSE;
          still = FALSE;
        }
      /* compare different word widths */
      if (minWidth < wordWidth)
        minWidth = wordWidth;
      if (!toCut)
        wordWidth = 0;
    }

  if (minWidth > pLine->LiXMax)
    pLine->LiXMax = minWidth;
  if (toCut)
    {
      /* Try to break the box pNextBox or a previous one */
      maxLength = pLine->LiXMax - xi;
      GetExtraMargins (pNextBox, frame, FALSE, &t, &b, &l, &r);
      if (pNextBox == pLine->LiFirstBox || pNextBox == pLine->LiFirstPiece)
        {
          /* There is only one box in the line */
          if (!pNextBox->BxAbstractBox->AbAcceptLineBreak ||
              pNextBox->BxAbstractBox->AbLeafType != LtText)
            {
              /* cannot break the box */
              pBox = pNextBox;	/* it's also the last box */
              if (pBox->BxType == BoBlock ||
                  pBox->BxType == BoFloatBlock ||
                  pBox->BxType == BoCellBlock)
                wordWidth = pBox->BxMinWidth + l + r;
              else if (!pBox->BxAbstractBox->AbWidth.DimIsPosition &&
                       pBox->BxAbstractBox->AbHorizEnclosing &&
                       !HasVariableWidth (pBox, pBlock))
                wordWidth = pBox->BxWidth;
              /* check if next boxes are set in lines */
              lastbox = pBox;
              still = (lastbox != NULL);
              while (still)
                {
                  if (lastbox->BxAbstractBox->AbLeafType == LtText &&
                      lastbox->BxNexChild)
                    /* get the next child */
                    pBox = lastbox->BxNexChild;
                  else
                    {
                      lastbox = GetNextBox (lastbox->BxAbstractBox, frame);
                      if (lastbox == NULL)
                        still = FALSE;
                      else if (!lastbox->BxAbstractBox->AbNotInLine &&
                               lastbox->BxAbstractBox->AbHorizEnclosing)
                        still = FALSE;
                      else
                        pBox = lastbox;
                    }
                }
            }
          else
            {
              /* Break the box anywhere */
              pBox = pNextBox;
              BreakMainBox (pLine, pNextBox, maxLength, l, r, line_spaces,
                            hyphenate, pRootAb, TRUE);
              if (pBox->BxAbstractBox->AbLeafType == LtText && pBox->BxNexChild)
                {
                  /* we have a new box */
                  if (pBox->BxType != BoScript && pBox->BxNexChild)
                    /* take the first child of a main box */
                    pBox = pBox->BxNexChild;
                  pLine->LiFirstPiece = pBox;
                }
              else
                pLine->LiFirstPiece = pBox;
            }
        }
      else
        {
          /* There is a previous box in the line */
          if (!pNextBox->BxAbstractBox->AbElement->ElTerminal &&
              pNextBox->BxWidth == pLine->LiXMax && !onlySpace)
            {
              /* cut before that box */
              toCut = FALSE;
              pBox = GetPreviousBox (pNextBox->BxAbstractBox, frame);
            }
          else if (pNextBox->BxAbstractBox->AbLeafType == LtText &&
                   pNextBox->BxAbstractBox->AbAcceptLineBreak &&
                   (CanHyphen (pNextBox) || pNextBox->BxNSpaces != 0))
            {
              /* Break that box */
              pBox = pNextBox;
              if (BreakMainBox (pLine, pNextBox, maxLength, l, r, line_spaces,
                                hyphenate, pRootAb, FALSE))
                {
                  if (pNextBox->BxAbstractBox->AbLeafType == LtText &&
                      pNextBox->BxNexChild)
                    {
                      /* we have a new box */
                      if (pNextBox->BxType != BoScript && pNextBox->BxNexChild)
                        /* take the first child of a main box */
                        pBox = pNextBox->BxNexChild;
                      else
                        pBox = pNextBox;
                    }
                  toCut = FALSE;
                }
              else
                {
                  if (pBox->BxType == BoScript &&
                      pBox != pBox->BxAbstractBox->AbBox->BxNexChild &&
                      pBox->BxPrevious)
                    pBox = pBox->BxPrevious;
                  else
                    pBox = GetPreviousBox (pNextBox->BxAbstractBox, frame);
                  toCut = TRUE;
                }
            }

          lastbox = pNextBox;
          /* If we didn't find a break, try on a previous box in the line */
          while (toCut)
            {
              if (pNextBox)
                {
                  if (pNextBox->BxType == BoScript &&
                      pNextBox != pNextBox->BxAbstractBox->AbBox->BxNexChild &&
                      pNextBox->BxPrevious)
                    pNextBox = pNextBox->BxPrevious;
                  else
                    pNextBox = GetPreviousBox (pNextBox->BxAbstractBox, frame);
                }
	      
              /* if we are working on the first box, we won't try again */
              if (pLine->LiFirstPiece && pNextBox == pLine->LiFirstPiece)
                toCut = FALSE;
              else if (pNextBox == pLine->LiFirstBox)
                {
                  toCut = FALSE;
                  if (pLine->LiFirstPiece)
                    pNextBox = pLine->LiFirstPiece;
                }

              if (pNextBox && 
                  pNextBox->BxAbstractBox->AbLeafType == LtText &&
                  !pNextBox->BxAbstractBox->AbNotInLine &&
                  pNextBox->BxAbstractBox->AbHorizEnclosing &&
                  pNextBox->BxW != 0 &&
                  pNextBox->BxAbstractBox->AbAcceptLineBreak &&
                  pNextBox->BxNSpaces != 0)
                {
                  if (onlySpace)
                    cutwidth = pNextBox->BxW - 3;
                  else
                    cutwidth = pNextBox->BxW - 1;
                  /* break on the last space of the box*/
                  GetExtraMargins (pNextBox, frame, FALSE, &t, &b, &l, &r);
                  if (pNextBox->BxType == BoPiece ||
                      pNextBox->BxType == BoDotted ||
                      pNextBox->BxType == BoScript)
                    {
                       BreakPieceOfBox (pLine, pNextBox, cutwidth, l, r, line_spaces,
                                       hyphenate, pRootAb);
                      pBox = pNextBox;
                      toCut = FALSE;
                    }
                  else
                    {
                      BreakMainBox (pLine, pNextBox, cutwidth, l, r, line_spaces,
                                    hyphenate, pRootAb, FALSE);
                      /* take the first child of a main box */
                      pBox = pNextBox->BxNexChild;
                      /* the first box of the line is a split box now */
                      if (pNextBox == pLine->LiFirstBox &&
                          pLine->LiFirstPiece == NULL)
                        pLine->LiFirstPiece = pBox;
                      if (pBox == NULL)
                        pBox = pNextBox;
                      toCut = FALSE;
                    }
                }
              else if (!toCut)
                {
                  /* no break found,  split the endding box */
                  /* Si la derniere boite est secable, force sa coupure */
                  if (lastbox->BxAbstractBox->AbAcceptLineBreak
                      && lastbox->BxAbstractBox->AbLeafType == LtText)
                    {
                      /* mandatory break */
                      GetExtraMargins (lastbox, frame, FALSE, &t, &b, &l, &r);
                      BreakMainBox (pLine, lastbox, maxLength, l, r, line_spaces,
                                    hyphenate, pRootAb, TRUE);
                      if (lastbox->BxType != BoScript && lastbox->BxNexChild)
                        /* take the first child of a main box */
                        pBox = lastbox->BxNexChild;
                      else
                        pBox = lastbox;
                    }
                  else if (lastbox == pLine->LiFirstBox)
                    /* there is only one box in the line */
                    pBox = lastbox;
                  else
                    {
                      /* break before the last box */
                      pBox = GetPreviousBox (lastbox->BxAbstractBox, frame);
                      /*pBox = lastbox->BxPrevious;*/
                      if (pBox == NULL)
                        pBox = lastbox;
                    }
                }
              else
                {
                  wordWidth = 0;
                  if (pNextBox == NULL)
                    {
                      /* stop */
                      toCut = FALSE;
                      pBox = lastbox;
                    }
                  else
                    /* continue the search */
                    pBox = pNextBox;
                }
            }
        }
    }

  /* check the width of the last box in the line */
  if (minWidth < wordWidth)
    minWidth = wordWidth;

  /* add boxes into the line */
  AddAllBoxesInLine (pBox, frame, pBlock, pRootAb, pLine, notComplete, full);
  return minWidth;
}


/*----------------------------------------------------------------------
  RemoveAdjustement recalcule la largueur de boite apres suppression 
  de la justification. Met a jour les marques de selection
  que la boite soit justifiee ou non.                     
  ----------------------------------------------------------------------*/
static void RemoveAdjustement (PtrBox pBox, int spaceWidth)
{
  int                 x;
  int                 l;

  /* Box justifiee -> met a jour sa largeur et les marques */
  if (!pBox->BxAbstractBox->AbDead &&
      pBox->BxAbstractBox->AbLeafType == LtText &&
      pBox->BxSpaceWidth)
    {
      /* blanc justifie - blanc de la police */
      x = pBox->BxSpaceWidth - spaceWidth;
      l = x * pBox->BxNSpaces + pBox->BxNPixels;
      pBox->BxW -= l;
      pBox->BxWidth -= l;
      pBox->BxSpaceWidth = 0;
      pBox->BxNPixels = 0;
    }
}

/*----------------------------------------------------------------------
  InitFloats looks for previous floating boxes.
  ----------------------------------------------------------------------*/
static void InitFloats (PtrBox pBlock, PtrLine pLine, PtrBox *floatL,
                        PtrBox *floatR, int top, ThotBool yAbs)
{
  PtrFloat            pfloat;
  int                 y;

  *floatL = NULL;
  *floatR = NULL;
  if (pLine)
    y = pLine->LiYOrg + pLine->LiHeight + top;
  else
    y = top;
  if (yAbs)
    y += pBlock->BxYOrg;

  pfloat = pBlock->BxLeftFloat;
  while (pfloat && pfloat->FlBox && pfloat->FlBox->BxYOrg < y)
    {
      *floatL = pfloat->FlBox;
      pfloat = pfloat->FlNext;
    }

  pfloat = pBlock->BxRightFloat;
  while (pfloat && pfloat->FlBox && pfloat->FlBox->BxYOrg < y)
    {
      *floatR = pfloat->FlBox;
      pfloat = pfloat->FlNext;
    }
}

/*----------------------------------------------------------------------
  UpdateBlockWithFloat updates the block of lines pBlock to take into
  account floating boxes.
  Updates the minimum and maximum widths of the block when the parameter
  updateWidth is TRUE.
  Returns the updated height.
  ----------------------------------------------------------------------*/
static void UpdateBlockWithFloat (int frame, PtrBox pBlock,
                                  ThotBool xAbs, ThotBool yAbs,
                                  ThotBool updateWidth, int *height)
{
  Propagation         propagateStatus;
  PtrFloat            pfloat;
  PtrBox              box;
  PtrAbstractBox      pAb;
  int                 y, x, x1, x2, w, minx1, minx2;
  int                 ml, mr, pl, pr, diff;
  int                 t = 0, b = 0, l = 0, r = 0;
  ThotBool            extensibleblock;

  extensibleblock =  pBlock->BxContentWidth;
  if (!extensibleblock && !updateWidth)
    // nothing to do
    return;

  if (xAbs || !pBlock->BxXToCompute)
    x = pBlock->BxXOrg;
  else
    x = 0;
  GetExtraMargins (pBlock, frame, FALSE, &t, &b, &l, &r);
  x += l + pBlock->BxLBorder + pBlock->BxLPadding;
  if (pBlock->BxLMargin > 0)
    x += pBlock->BxLMargin;
  x1 = x2 = minx1 = minx2 = 0;
  if (yAbs || !pBlock->BxYToCompute)
    y = pBlock->BxYOrg;
  else
    y = 0;
  y += t + pBlock->BxTBorder + pBlock->BxTPadding;
  if (pBlock->BxTMargin > 0)
    y += pBlock->BxTMargin;

  pfloat = pBlock->BxLeftFloat;
  while (pfloat && pfloat->FlBox)
    {
      box = pfloat->FlBox;
      GetLeftRightMargins (box, pBlock, frame, &ml, &mr);
      GetLeftRightPaddings (box, pBlock, &pl, &pr);
      if ((box->BxType == BoFloatBlock || box->BxType == BoBlock) && box->BxContentWidth)
        {
          w = box->BxMaxWidth + box->BxLBorder + box->BxRBorder + pl + pr;
          if (box->BxType != BoPicture && minx1 < box->BxMinWidth)
            minx1 = box->BxMinWidth;
          else if (box->BxType == BoPicture && minx1 < box->BxWidth)
            minx1 = box->BxWidth;
        }
      else if (box->BxAbstractBox->AbWidth.DimUnit == UnPercent && extensibleblock)
        {
          // need to update the floated width
          propagateStatus = Propagate;
          if (box->BxAbstractBox->AbEnclosing &&
              box->BxAbstractBox->AbEnclosing->AbBox &&
              box->BxAbstractBox->AbEnclosing->AbBox->BxType == BoStructGhost)
            w = box->BxAbstractBox->AbEnclosing->AbBox->BxW;
          else
            w = pBlock->BxW;
          w = w * box->BxAbstractBox->AbWidth.DimValue / 100;
          diff = w - box->BxW;
          w += box->BxLBorder + box->BxRBorder + pl + pr;
          ResizeWidth (box, pBlock, pBlock, diff, 0, 0, 0, frame, FALSE);
          Propagate = propagateStatus;
        }
      else
        {
          w = box->BxWidth - mr;
          if (ml >= 0)
            w += ml;
          else
            w -= ml;
          if (minx1 < box->BxMinWidth)
            minx1 = box->BxMinWidth;
        }

      if (box->BxXOrg + w - x > x1)
        /* float change the minimum width of the block */
        x1 = box->BxXOrg + w - x;
      if (box->BxYOrg + box->BxHeight - y > *height)
        /* float change the height of the block */
        *height = box->BxYOrg + box->BxHeight - y;
      pfloat = pfloat->FlNext;
    }

  pfloat = pBlock->BxRightFloat;
  while (pfloat && pfloat->FlBox)
    {
      box = pfloat->FlBox;
      GetLeftRightMargins (box, pBlock, frame, &ml, &mr);
      GetLeftRightPaddings (box, pBlock, &pl, &pr);
      if ((box->BxType == BoFloatBlock || box->BxType == BoBlock) && box->BxContentWidth)
        {
          w = box->BxMaxWidth + box->BxLBorder + box->BxRBorder + pl + pr;
          if (minx2 < box->BxMinWidth)
            minx2 = box->BxMinWidth;
        }
       else if (box->BxAbstractBox->AbWidth.DimUnit == UnPercent)
        {
          // need to update the floated width
          propagateStatus = Propagate;
          if (box->BxAbstractBox->AbEnclosing &&
              box->BxAbstractBox->AbEnclosing->AbBox &&
              box->BxAbstractBox->AbEnclosing->AbBox->BxType == BoStructGhost)
            w = box->BxAbstractBox->AbEnclosing->AbBox->BxW;
          else
            w = pBlock->BxW;
          w = w * box->BxAbstractBox->AbWidth.DimValue / 100;
          diff = w - box->BxW;
          w += box->BxLBorder + box->BxRBorder + pl + pr;
          ResizeWidth (box, pBlock, pBlock, diff, 0, 0, 0, frame, FALSE);
          Propagate = propagateStatus;
        }
      else
        {
          w = box->BxWidth;
          if (ml <= 0)
            w += ml;
          else
            w -= ml;
           if (box->BxType != BoPicture && minx2 < box->BxMinWidth)
            minx2 = box->BxMinWidth;
           else if (box->BxType == BoPicture && minx2 < box->BxWidth)
            minx2 = box->BxWidth;
        }
      if (box->BxXOrg + w > x + pBlock->BxMaxWidth + x2)
        /* float change the minimum width of the block */
        x2 = box->BxXOrg + w - x - pBlock->BxMaxWidth;
      if (box->BxYOrg + box->BxHeight - y > *height)
        /* float change the height of the block */
        *height = box->BxYOrg + box->BxHeight - y;
      pfloat = pfloat->FlNext;
    }

  if (updateWidth)
    {
      /* update min and max widths */
      if (pBlock->BxMinWidth < minx1)
        pBlock->BxMinWidth = minx1;
      if (pBlock->BxMinWidth < minx2)
        pBlock->BxMinWidth = minx2;
      pBlock->BxMaxWidth += x1+ x2;
    }

  if (extensibleblock)
    {
      // move right floated boxes according to the right constraint
      pfloat = pBlock->BxRightFloat;
      x = pBlock->BxMaxWidth;
      while (pfloat && pfloat->FlBox)
        {
          XMove (pfloat->FlBox, NULL,
                 x - pfloat->FlBox->BxWidth - pfloat->FlBox->BxXOrg, frame);
          x -= pfloat->FlBox->BxWidth;
          pfloat = pfloat->FlNext;
        }
    }
  if (updateWidth)
    {
      // transmit min and max to enclosing not block of line boxes
      pAb = pBlock->BxAbstractBox;
      while (pAb && pAb->AbEnclosing && pAb->AbEnclosing->AbBox)
        {
          pAb = pAb->AbEnclosing;
          box = pAb->AbBox;
          if (box->BxType == BoFloatGhost ||
              box->BxType == BoBlock ||
              box->BxType == BoFloatBlock ||
              box->BxType == BoCellBlock ||
              box->BxType == BoTable || box->BxType == BoCell ||
              (pAb->AbWidth.DimAbRef == NULL && pAb->AbWidth.DimUnit != UnPercent))
            // don't change the min and max width of that box
            pAb = NULL;
          else
            {
              if (box->BxMinWidth < pBlock->BxMinWidth)
                box->BxMinWidth = pBlock->BxMinWidth;
              if (box->BxMaxWidth < pBlock->BxMaxWidth)
                box->BxMaxWidth = pBlock->BxMaxWidth;
            }
        }
    }
}

/*----------------------------------------------------------------------
  ShiftFloatingBoxes updates the X position of floating boxes
  ----------------------------------------------------------------------*/
void ShiftFloatingBoxes (PtrBox pBlock, int delta, int frame)
{
  PtrFloat            pfloat;

  if (delta == 0)
    return;
  pfloat = pBlock->BxLeftFloat;
  while (pfloat && pfloat->FlBox)
    {
      if (pfloat->FlBox->BxAbstractBox &&
          pfloat->FlBox->BxAbstractBox->AbFloat == 'L')
        /* change the position */
        XMoveAllEnclosed (pfloat->FlBox, delta, frame);
      pfloat = pfloat->FlNext;
    }

  pfloat = pBlock->BxRightFloat;
  while (pfloat && pfloat->FlBox)
    {
      if (pfloat->FlBox->BxAbstractBox &&
          pfloat->FlBox->BxAbstractBox->AbFloat == 'R')
        /* change the position */
        XMoveAllEnclosed (pfloat->FlBox, delta, frame);
      pfloat = pfloat->FlNext;
    }
}

/*----------------------------------------------------------------------
  MoveFloatingBoxes updates the Y position of floating boxes below the
  position y
  ----------------------------------------------------------------------*/
static void MoveFloatingBoxes (PtrBox pBlock, int y, int delta, int frame)
{
  PtrFloat            pfloat;

  pfloat = pBlock->BxLeftFloat;
  while (pfloat && pfloat->FlBox)
    {
      if (pfloat->FlBox->BxAbstractBox &&
          pfloat->FlBox->BxAbstractBox->AbFloat == 'L' &&
          pfloat->FlBox->BxYOrg >= y)
        /* change the position */
        YMove (pfloat->FlBox, NULL, delta, frame);
      pfloat = pfloat->FlNext;
    }

  pfloat = pBlock->BxRightFloat;
  while (pfloat && pfloat->FlBox)
    {
      if (pfloat->FlBox->BxAbstractBox &&
          pfloat->FlBox->BxAbstractBox->AbFloat == 'R' &&
          pfloat->FlBox->BxYOrg >= y)
        /* change the position */
        YMove (pfloat->FlBox, NULL, delta, frame);
      pfloat = pfloat->FlNext;
    }
}


/*----------------------------------------------------------------------
  SetFloat computes the position of the floating box box in block of lines
  pBlock.
  Parameters top, bottom, left, and right give the space reserved by
  margins/borders/paddings of the block.
  Work with absolute positions when xAbs and yAbs are TRUE.
  Parameters floatL and floatR point to last left an right floating
  boxes in the current block.
  ----------------------------------------------------------------------*/
int SetFloat (PtrBox box, PtrBox pBlock, PtrLine pLine, PtrAbstractBox pRootAb,
              int maxWidth, ThotBool extensibleBlock, ThotBool xAbs, ThotBool yAbs,
              ThotBool notComplete, ThotBool onlySpace, ThotBool *full, ThotBool *adjust,
              ThotBool *breakLine,  ThotBool *newblock, int frame, int indent,
              int top, int bottom, int left, int right,
              PtrBox *floatL, PtrBox *floatR)
{
  PtrFloat            pfloat = NULL;
  PtrBox              boxPrevL, boxPrevR;
  PtrBox              pNextBox, prevBox;
  int                 x, y, w, minWidth, ret, h, bottomL, bottomR;
  int                 orgX, orgY, bw, ml, mr, shiftl = 0, shiftr = 0;
  ThotBool            clearl, clearr;

  boxPrevL = *floatL;
  boxPrevR = *floatR;
  minWidth = pLine->LiMinLength;
  h = pLine->LiHeight;
  orgX = 0;
  orgY = 0;
  if (xAbs)
    orgX += pBlock->BxXOrg;
  if (yAbs)
    orgY += pBlock->BxYOrg;
  ml = box->BxLMargin;
  mr = box->BxRMargin;
  bw = box->BxWidth;
  if (box->BxAbstractBox->AbFloat == 'L')
    {
      bw -= mr; // ignore the right margin
      if (ml < 0)
        bw -= ml; // ignore the left margin
      /* initial position */
      x = left + orgX;
    }
  else
    {
      if (ml > 0)
        bw -= ml; // ignore the left margin
      if (mr > 0)
        bw -= mr; // ignore the positive right margin
      /* initial position */
      x = pBlock->BxWidth - right - bw + orgX;
    }
  if (pLine)
    {
      y = orgY + pLine->LiYOrg;
      if (pLine->LiRealLength > 0)
        {
          /* it must be displayed below the current line */
          prevBox = GetPreviousBox (box->BxAbstractBox, frame);
          AddAllBoxesInLine (prevBox, frame, pBlock, pRootAb, pLine, notComplete, full);
          if (pLine->LiRealLength + bw > pLine->LiXMax)
            return minWidth;
        }
      w = pBlock->BxW;
    }
  else
    {
      y = top;
      w = 0;
    }
  if (box->BxAbstractBox->AbFloat == 'L' && boxPrevL &&
      y < boxPrevL->BxYOrg)
    y = boxPrevL->BxYOrg;
  if (box->BxAbstractBox->AbFloat == 'R' && boxPrevR &&
      y < boxPrevR->BxYOrg)
    y = boxPrevR->BxYOrg;
  bottomL = y;
  bottomR = y;
  if  (boxPrevR)
    {
      shiftr = boxPrevR->BxXOrg;
      // calculate the current bottom of the right float
      bottomR = boxPrevR->BxYOrg + boxPrevR->BxHeight;
      pfloat = pBlock->BxRightFloat;
      while (pfloat && pfloat->FlBox != boxPrevR)
        {
          if (pfloat->FlBox->BxYOrg + pfloat->FlBox->BxHeight > bottomR)
            bottomR = pfloat->FlBox->BxYOrg + pfloat->FlBox->BxHeight;
          pfloat = pfloat->FlNext;
        }
      if (boxPrevR->BxXOrg + boxPrevR->BxWidth >= orgX + w - right)
        {
          // a negative left margin increases the line width
          if (boxPrevR->BxLMargin < 0)
            shiftr -= boxPrevR->BxLMargin;
        }
      /* can be inserted next to this previous float ? */
      if (y < bottomR && y + box->BxHeight >= boxPrevR->BxYOrg)
        w = shiftr - left - orgX;
      else
        shiftr = 0;
    }
  if  (boxPrevL)
    {
      shiftl = boxPrevL->BxXOrg + boxPrevL->BxWidth;
      // calculate the current bottom of the left float
      bottomL = boxPrevL->BxYOrg + boxPrevL->BxHeight;
      pfloat = pBlock->BxLeftFloat;
      while (pfloat && pfloat->FlBox != boxPrevL)
        {
          if (pfloat->FlBox->BxYOrg + pfloat->FlBox->BxHeight > bottomL)
            bottomL = pfloat->FlBox->BxYOrg + pfloat->FlBox->BxHeight;
          pfloat = pfloat->FlNext;
        }
     if (boxPrevL->BxXOrg <= orgX + left)
        {
          // a negative right margin increases the line width
          if (boxPrevL->BxRMargin < 0)
            shiftl += boxPrevL->BxRMargin;
          else
            shiftl -= boxPrevL->BxRMargin;
          if (boxPrevL->BxLMargin > 0)
          shiftl -= boxPrevL->BxLMargin;
        }
      /* can be inserted next to this previous float ? */
      if (y < bottomL && y + box->BxHeight > boxPrevL->BxYOrg)
        w = w - shiftl + left + orgX;
      else
        shiftl = 0;
    }

  /* check if a clear is requested */
  SetClear (box, &clearl, &clearr);
  if (boxPrevL && clearl && y < bottomL)
    // make sure the float is displayed under previous left floats
    y = bottomL;
  if (boxPrevR && clearr && y < bottomR)
    // make sure the float is displayed under previous right floats
    y = bottomR;

  if ((boxPrevL && y < bottomL) || (boxPrevR && y < bottomR))
    {
      if (bottomL < bottomR)
        {
          if (y < bottomL &&
              (bw > w + 1 || (boxPrevL && y > boxPrevL->BxYOrg)))
            {
              // at the bottom of the left float
              w = shiftr - left - orgX;
              y = bottomL;
            }
        }
      else
        {
          if (y < bottomR &&
              (bw > w + 1 || (boxPrevR && y > boxPrevR->BxYOrg)))
            {
              w = pBlock->BxW + left + orgX - shiftl;
              y = bottomR;
            }
        }

       if (bw > w + 1 && HasVariableWidth (box, pBlock) &&
           w > 0 && w >= box->BxMinWidth - ml - mr)
         {
           ResizeWidth (box, pBlock, pBlock, 
                        w - box->BxW, 0, 0, 0, frame, FALSE);
           bw = w;
         }

      if (bw <= w + 1)
        {
          /* it's possible to display the floating box at the current position */
          if (boxPrevL && y < bottomL &&
              box->BxAbstractBox->AbFloat == 'L')
             {
              x = shiftl;
              //y = boxPrevL->BxYOrg;
            }
          else if (y < bottomR && boxPrevR &&
                   box->BxAbstractBox->AbFloat == 'R')
            {
              x = shiftr - bw;
              y = boxPrevR->BxYOrg;
            }
        }
      else if (boxPrevL && boxPrevR && y < bottomL  && y < bottomR)
        {
          /* display under the previous floating box of current side */
          if (box->BxAbstractBox->AbFloat == 'L')
            {
              if (bottomL >= bottomR || x + bw <= shiftr)
                /* display under the previous left floating box */
                y = bottomL;
              else
                /* display under the previous right floating box */
                y = bottomR;
            }
          else
            {
              if (bottomR >= bottomL || x <= shiftl)
                /* display under the previous right floating box */
                y = bottomR;
              else
                /* display under the previous left floating box */
                y = bottomL;
             }
        }
      else if (boxPrevL && y < bottomL)
        y = bottomL;
      else if (y < bottomR)
        y = bottomR;
      /* following lines cannot be displayed above this box */
      //pLine->LiYOrg = y - orgY;
    }

  if (box->BxAbstractBox->AbFloat == 'L' && ml < 0)
    x += ml;
  else if (box->BxAbstractBox->AbFloat == 'R')
    {
      //if (ml > 0)
      x -= ml;
      x -= mr;
    }
  XMove (box, NULL, x - box->BxXOrg, frame);
  YMove (box, NULL, y - box->BxYOrg, frame);
  if (box->BxAbstractBox->AbFloat == 'L')
    *floatL = box;
  else
    *floatR = box;

  pNextBox = GetNextBox (box->BxAbstractBox, frame);
  ret=  FillLine (pLine, pNextBox, pBlock, pRootAb, maxWidth,
                  extensibleBlock, xAbs, yAbs,
                  notComplete, onlySpace, full, adjust, breakLine, newblock,
                  frame, indent, top, bottom, left, right, floatL, floatR);
  /* integrate information about previous inserted boxes */
  if (h > pLine->LiHeight)
    pLine->LiHeight = h;
  if (minWidth > ret)
    return minWidth;
  else
    return ret;
}


/*----------------------------------------------------------------------
  RemoveBreaks removes all pieces of the main box pBox.
  The parameter removed is TRUE when the enclosing box will be freed.
  Return:
  Parameters changeSelectBegin and changeSelectEnd are TRUE when the box
  is concerned by the box selection.
  ----------------------------------------------------------------------*/
static void RemoveBreaks (PtrBox pBox, int frame, ThotBool removed,
                          ThotBool *changeSelectBegin, ThotBool *changeSelectEnd)
{
  PtrBox              ibox1;
  PtrBox              ibox2;
  PtrBox              pNextBox;
  PtrAbstractBox      pAb;
  ViewFrame          *pFrame;
  ViewSelection      *pViewSel, *pViewSelEnd;
  int                 x, width = 0;
  int                 nspace = 0;
  int                 lost = 0;
  int                 diff, nchar = 0;
  int                 t, b, l, r, c;

  pFrame = &ViewFrameTable[frame - 1];
  pViewSel = &pFrame->FrSelectionBegin;
  pViewSelEnd = &pFrame->FrSelectionEnd;
  if (pBox && pBox->BxAbstractBox)
    {
      pAb = pBox->BxAbstractBox;
      if (pAb && pAb->AbNew)
        return;
      /* check if the abstract box will be deleted */
      if (!removed)
        removed = pAb->AbDead;
      if (pAb && pAb->AbLeafType == LtText)
        {
          x = BoxCharacterWidth (SPACE, 1, pBox->BxFont);
          if (pViewSel->VsBox == pBox)
            {
              /* need to update the current selection */
              pViewSel->VsBox = pAb->AbBox;
              pViewSel->VsIndBox += pBox->BxFirstChar;
              *changeSelectBegin = TRUE;
            }
          if (pViewSelEnd->VsBox == pBox)
            {
              /* need to update the current selection */
              pViewSelEnd->VsBox = pAb->AbBox;
              pViewSelEnd->VsIndBox += pBox->BxFirstChar;
              *changeSelectEnd = TRUE;
            }

          if (pBox->BxType == BoComplete)
            /* not split */
            RemoveAdjustement (pBox, x);
          else
            {
              if (pBox->BxType == BoMulScript)
                {
                  ibox1 = pBox->BxNexChild;
                  while (ibox1)
                    {
                      /* remove piece boxes but keep all script boxes */
                      RemoveBreaks (ibox1, frame, removed, changeSelectBegin,
                                    changeSelectEnd);
                      ibox1 = ibox1->BxNexChild;
                    }
                }
              else if (pBox->BxType == BoSplit)
                {
                  /* get the first child */
                  ibox1 = pBox->BxNexChild;
                  /* update the main box */
                  pBox->BxNexChild = NULL;
                  pBox->BxType = BoComplete;
                  pBox->BxPrevious = ibox1->BxPrevious;
                  /* transmit the current position */
                  pBox->BxXOrg = ibox1->BxXOrg;
                  pBox->BxYOrg = ibox1->BxYOrg;
                  if (!removed)
                    {
                      /* update box links */
                      if (pBox->BxPrevious)
                        /* not already freed box */
                        pBox->BxPrevious->BxNext = pBox;
                      else
                        pFrame->FrAbstractBox->AbBox->BxNext = pBox;
                    }
                  width = 0;
                  nspace = 0;
                  lost = 1;
                  nchar = 0;
                }
              else
                {
                  /* get the first child */
                  ibox1 = pBox->BxNexChild;
                  if (ibox1 && ibox1->BxType != BoScript)
                    pBox->BxNexChild = NULL;
                  /* merge one or more pieces */
                  RemoveAdjustement (pBox, x);
                  width = pBox->BxW;
                  if (pBox->BxType == BoDotted && !removed)
                    {
                      /* remove the hyphen width */
                      width -= BoxCharacterWidth (173, 1, pBox->BxFont);
                      /* check if it's a script */
                      if (pBox->BxAbstractBox->AbBox &&
                          pBox->BxAbstractBox->AbBox->BxType == BoMulScript &&
                          (pBox == pBox->BxAbstractBox->AbBox->BxNexChild ||
                           pBox->BxPrevious->BxScript != pBox->BxScript))
                        pBox->BxType = BoScript;
                      else
                        pBox->BxType = BoPiece;
                    }
                  nspace = 0;
                  lost = pBox->BxFirstChar + pBox->BxNChars;
                  nchar = 0;
                }
	      
              /* Now free all following pieces */
              if (ibox1 && ibox1->BxType != BoScript)
                {
                  do
                    {
                      pNextBox = ibox1->BxNext;
                      if (!removed)
                        {
                          RemoveAdjustement (ibox1, x);
                          /* lost pixels */
                          diff = ibox1->BxFirstChar - lost;
                          if (diff > 0)
                            {
                              nchar += diff;
                              /* add skipped spaces */
                              if (ibox1->BxBuffer == NULL)
                                c = 0;
                              else if (ibox1->BxIndChar > 0)
                                c = ibox1->BxBuffer->BuContent[ibox1->BxIndChar - 1];
                              else if (ibox1->BxBuffer->BuPrevious)
                                c = ibox1->BxBuffer->BuPrevious->BuContent[ibox1->BxBuffer->BuPrevious->BuLength - 1];
                              else
                                c = 0;
                              if (c == 0xA /* LINEFEED */)
                                {
                                  diff -= 1;
                                  width += BoxCharacterWidth ((CHAR_T)c, 1, ibox1->BxFont);
                                }
                              width = width + (diff * x);
                              nspace += diff;
                            }
                          else if (ibox1->BxType == BoDotted)
                            /* remove the hyphen width */
                            width -= BoxCharacterWidth (173, 1, ibox1->BxFont);
                          if (pNextBox && pNextBox->BxScript == ibox1->BxScript)
                            /* add skipped spaces at the end of the box */
                            lost = ibox1->BxFirstChar + ibox1->BxNChars;
                          /* if the next box is not empty */
                          if (ibox1->BxNChars > 0)
                            {
                              nspace += ibox1->BxNSpaces;
                              width += ibox1->BxW;
                              nchar += ibox1->BxNChars;
                            }
                          /* transmit the rigth margin, border, and padding of
                             the last box */
                          if (ibox1->BxNexChild == NULL && pBox->BxType != BoComplete)
                            {
                              pBox->BxRMargin = ibox1->BxRMargin;
                              pBox->BxRBorder = ibox1->BxRBorder;
                              pBox->BxRPadding = ibox1->BxRPadding;
                            }
			  
                          /* Prepare the new selection */
                          if (pViewSel->VsBox == ibox1)
                            {
                              pViewSel->VsBox = pAb->AbBox;
                              pViewSel->VsIndBox += ibox1->BxFirstChar;
                              *changeSelectBegin = TRUE;
                            }
                          if (pViewSelEnd->VsBox == ibox1)
                            {
                              pViewSelEnd->VsBox = pAb->AbBox;
                              pViewSelEnd->VsIndBox += ibox1->BxFirstChar;
                              *changeSelectEnd = TRUE;
                            }
                        }
#ifdef _GL
#ifdef DEBUG_MAC
if (ibox1->DisplayList)
 printf ( "GLBUG - RemoveBreaks : glIsList=%s (pose prb sur certaines machines)\n", glIsList (ibox1->DisplayList) ? "yes" : "no" );
#endif /* DEBUG_MAC */
                      if (glIsList (ibox1->DisplayList))
                        {
                          glDeleteLists (ibox1->DisplayList, 1);
                          ibox1->DisplayList = 0;
                        }
#endif /* _GL */
                      ibox2 = FreeBox (ibox1);
                      ibox1 = ibox2;
                    }
                  while (ibox1 && ibox1->BxType != BoScript);
		  
                  /* Update the first piece of box */
                  if (pBox->BxType != BoComplete)
                    {
                      pBox->BxNChars += nchar;
                      pBox->BxW = width;
                      GetExtraMargins (pBox, frame, FALSE, &t, &b, &l, &r);
                      pBox->BxWidth = width + pBox->BxLBorder + pBox->BxLPadding
                                      + pBox->BxRBorder + pBox->BxRPadding + l + r;
                      if (pBox->BxLMargin > 0)
                        pBox->BxWidth += pBox->BxLMargin;
                      if (pBox->BxRMargin > 0)
                        pBox->BxWidth += pBox->BxRMargin;
                      pBox->BxNSpaces += nspace;
                    }
                  /* Update the chain of leaf boxes */
                  if (pBox->BxType == BoScript)
                    {
                      if (pNextBox && pNextBox->BxType == BoScript &&
                          pNextBox->BxAbstractBox == pBox->BxAbstractBox)
                        pBox->BxNexChild = pNextBox;
                      else
                        pBox->BxNexChild = NULL;
                    }
                  pBox->BxNext = pNextBox;
                  if (pNextBox)
                    pNextBox->BxPrevious = pBox;
                  else
                    pFrame->FrAbstractBox->AbBox->BxPrevious = pBox;
                }
            }
        }
      /* Pour les autres natures */
      else
        {
          if (pViewSel->VsBox == pBox)
            *changeSelectBegin = TRUE;
          if (pViewSelEnd->VsBox == pBox)
            *changeSelectEnd = TRUE;
        }
    }
}


/*----------------------------------------------------------------------
  ComputeLines generates lines and put enclosed boxes into them.
  Enclosed boxes are placed either relativelly to the pBox (first
  building) or relativelly to the root box.
  Returns the box height.
  ----------------------------------------------------------------------*/
void ComputeLines (PtrBox pBox, int frame, int *height)
{
  PtrLine             prevLine, pLine;
  PtrAbstractBox      pChildAb, pCell;
  PtrAbstractBox      pAb, pRootAb, pParent, pRefBlock, prevBlock;
  PtrBox              pBoxToBreak, pNextBox;
  PtrBox              floatL, floatR, box;
  AbPosition         *pPosAb;
  int                 x, lineSpacing, indent, maxWidth;
  int                 org, width, noWrappedWidth;
  int                 lostPixels, minWidth, y, lspacing;
  int                 top, left, right, bottom, spacing;
  int                 l, r, zoom;
  ThotBool            toAdjust, breakLine, isExtraFlow;
  ThotBool            xAbs, yAbs, extensibleBox, onlySpace = TRUE;
  ThotBool            full, still, standard, isFloat, newblock;

  /* avoid any cycle */
  if (pBox->BxCycles > 0)
    {
      *height = pBox->BxH;
      return;
    }
  pBox->BxCycles++;
  /* Fill the block box */
  noWrappedWidth = 0;
  pAb = pBox->BxAbstractBox;
  pRefBlock = pAb;
  pRootAb = ViewFrameTable[frame - 1].FrAbstractBox;
  /* save current width */
  width = pBox->BxW;
  GetExtraMargins (pBox, frame, FALSE, &top, &bottom, &left, &right);
  top += pBox->BxTMargin + pBox->BxTBorder + pBox->BxTPadding;
  bottom += pBox->BxBMargin + pBox->BxBBorder + pBox->BxBPadding;
  left += pBox->BxLMargin + pBox->BxLBorder + pBox->BxLPadding;
  right += pBox->BxRMargin + pBox->BxRBorder + pBox->BxRPadding;
  l = r = 0;
  extensibleBox = (pBox->BxContentWidth ||
                   (!pAb->AbWidth.DimIsPosition && pAb->AbWidth.DimMinimum));
  /* what is the maximum width allowed */
  pParent = pAb->AbEnclosing;
  isFloat = pAb->AbFloat != 'N';
  isExtraFlow = ExtraFlow (pBox, frame);
  if (pBox->BxShrink && pBox->BxRuleWidth)
    maxWidth = pBox->BxRuleWidth;
  else if ((pAb->AbWidth.DimUnit == UnAuto ||
            pBox->BxType == BoFloatBlock || pBox->BxType == BoCellBlock) &&
      pParent)
    {
      /* limit to the enclosing box */
      if (pAb->AbWidth.DimAbRef == NULL && pAb->AbWidth.DimValue == -1)
        {
          if (isExtraFlow)
            pParent = GetEnclosingViewport (pAb);
          while (pParent && pParent->AbBox &&
                 ((pParent->AbWidth.DimAbRef == NULL &&
                   pParent->AbWidth.DimValue == -1) ||
                  pParent->AbBox->BxType == BoGhost ||
                  //pParent->AbBox->BxType == BoStructGhost ||
                  pParent->AbBox->BxType == BoFloatGhost))
            {
              isExtraFlow = ExtraFlow (pParent->AbBox, frame);
              if (isExtraFlow)
                pParent = GetEnclosingViewport (pParent);
              else
                {
                  if (pParent->AbFloat != 'N')
                    isFloat = TRUE;
                  pParent = pParent->AbEnclosing;
                }
            }
          if (pParent && pParent->AbBox)
          {
            l += pParent->AbBox->BxLMargin + pParent->AbBox->BxLBorder + pParent->AbBox->BxLPadding;
            r += pParent->AbBox->BxRMargin + pParent->AbBox->BxRBorder + pParent->AbBox->BxRPadding;
          }
        }
      // the specific management of cells concerns only formatted views
      if (FrameTable[frame].FrView == 1)
        pCell = GetParentCell (pBox);
      else
        pCell = NULL;
      if (pAb->AbWidth.DimUnit == UnAuto && (isFloat || isExtraFlow))
        {
          if (pParent && pParent->AbBox &&
              pParent->AbWidth.DimUnit != UnAuto)
            maxWidth = pParent->AbBox->BxW - left - right - l - r;
          else
            {
              /* manage this box as an extensible box */
              maxWidth = 30 * DOT_PER_INCH;
              extensibleBox = TRUE;
            }
        }
      else if (pParent && pParent->AbBox && pParent->AbBox->BxType != BoCell)
        {
          if (pParent->AbBox->BxW <= 0)
             /* manage this box as an extensible box but it's not */
             maxWidth = 30 * DOT_PER_INCH;
          else
            {
              /* keep the box width */
              maxWidth = pParent->AbBox->BxW - left - right - l - r;
              onlySpace = (pCell != NULL);
            }
        }
      else
        {
          if (pCell && pCell->AbBox)
            {
              /* keep the box width */
              maxWidth = pCell->AbBox->BxW - left - right;
              onlySpace = TRUE;
            }
          else
            /* manage this box as an extensible box but it's not*/
            maxWidth = 30 * DOT_PER_INCH;
        }
      pBox->BxRuleWidth = maxWidth;
    }
  else
    maxWidth = 30 * DOT_PER_INCH;

  if (extensibleBox || pBox->BxShrink)
    pBox->BxW = maxWidth;
  /* compute the line spacing */
  zoom = ViewFrameTable[frame - 1].FrMagnification;
  if (pBox->BxType == BoBlock)
    lineSpacing = PixelValue (pAb->AbLineSpacing, pAb->AbLineSpacingUnit,
                              pAb, zoom);
  else
    lineSpacing = 0;
  /* space added at the top and bottom of the paragraph */
  spacing = lineSpacing - GetCurrentFontHeight (pAb->AbSize, pAb->AbSizeUnit, zoom);
  standard = (spacing >= 0);
  spacing = 0;
  /* compute the indent */
  if (extensibleBox)
    indent = 0;
  else if (pAb->AbIndentUnit == UnPercent)
    indent = PixelValue (pAb->AbIndent, UnPercent,
                         (PtrAbstractBox) width, 0);
  else
    indent = PixelValue (pAb->AbIndent, pAb->AbIndentUnit, pAb,
                         zoom);

  pNextBox = NULL;
  full = TRUE;
  x = 0;
  floatL = NULL;
  floatR = NULL;
  /* check if the X, Y position is relative or absolute */
  IsXYPosComplete (pBox, &xAbs, &yAbs);
  if (pBox->BxFirstLine == NULL)
    {
      /* Build all the block of lines */
      /* reset the value of the width without wrapping */
      if (extensibleBox || pBox->BxShrink ||
          (pAb->AbWidth.DimAbRef == NULL && pAb->AbWidth.DimUnit != UnPercent))
        pBox->BxMaxWidth = 0;
      pBox->BxMinWidth = 0;
      /* look for the first included box */
      pChildAb = pAb->AbFirstEnclosed;
      still = (pChildAb != NULL);
      pNextBox = NULL;
      while (still)
        if (pChildAb == NULL)
          still = FALSE;
      /* Is the abstract box dead? */
        else if (pChildAb->AbDead || pChildAb->AbNew || pChildAb->AbBox == NULL)
          pChildAb = pChildAb->AbNext;
        else if  (pChildAb->AbNotInLine && pChildAb->AbDisplay == 'U')
          pChildAb = pChildAb->AbNext;
        else if (pChildAb->AbBox->BxType == BoGhost ||
                 pChildAb->AbBox->BxType == BoStructGhost ||
                 pChildAb->AbBox->BxType == BoFloatGhost)
          /* go down into the hierarchy */
          pChildAb = pChildAb->AbFirstEnclosed;
        else if (pChildAb->AbPresentationBox && pChildAb->AbTypeNum == 0 &&
                 pChildAb->AbHorizPos.PosAbRef && pChildAb->AbHorizPos.PosAbRef->AbFloat != 'N')
          // skip not inline bullets
          pChildAb = pChildAb->AbNext;
        else
          {
            /* keep the current box */
            pNextBox = pChildAb->AbBox;
            still = FALSE;
          }
    }

  if (pBox->BxFirstLine == NULL)
    {
      if (pNextBox == NULL)
        /* Nothing to add in the line */
        full = FALSE;
      else
        {
          GetLine (&pLine);
          pBox->BxFirstLine = pLine;
        }
      
      pBoxToBreak = NULL;
      prevLine = NULL;
      /* height of the block box */
      *height = spacing;
      pBox->BxMinWidth = 0;
    }
  else
    {
      /* Partial building of the block */
      prevLine = pBox->BxLastLine;
      pChildAb = prevLine->LiLastBox->BxAbstractBox;
      /* height of the block box */
      *height = prevLine->LiYOrg + prevLine->LiHeight - top;
      pBoxToBreak = prevLine->LiLastPiece;
      pNextBox = prevLine->LiLastBox;
      pBox->BxMinWidth = pBox->BxMinWidth - left - right;
      if (pBoxToBreak && pBoxToBreak->BxNexChild)
        pBoxToBreak = pBoxToBreak->BxNexChild;
      else
        {
          pNextBox = GetNextBox (pChildAb, frame);
          pBoxToBreak = NULL;
        }
      
      if (pNextBox == NULL)
        /* nothing else */
        full = FALSE;
      else
        {
          /* prepare the next line */
          GetLine (&pLine);
          prevLine->LiNext = pLine;
          /* look for previous floating boxes */
          InitFloats (pBox, prevLine, &floatL, &floatR, top, yAbs);
        }
    }

  /* Insert new lines as long as they are full */
  /* ----------------------------------------- */
  while (full)
    {
      /* Initialize a new line */
      pLine->LiPrevious = prevLine;
      pLine->LiYOrg = *height + top;
      pChildAb = pNextBox->BxAbstractBox;
      if (pChildAb->AbFloat == 'N' &&
          !ExtraFlow (pNextBox, frame) &&
          (!pChildAb->AbHorizEnclosing ||
           (pChildAb->AbNotInLine &&
            pChildAb->AbDisplay != 'U')))
        {
          /* The current box escape the line rule */
          pLine->LiFirstBox = pNextBox;
          pLine->LiLastBox = pNextBox;
          pLine->LiFirstPiece = NULL;
          pLine->LiLastPiece = NULL;
          /* It's placed under the previous line */
          InitLine (pLine, pBox, frame, indent, floatL, floatR, pNextBox,
                    top, bottom, left, right, xAbs, yAbs);
          if (extensibleBox)
            /* no limit for an extensible line */
            pLine->LiXMax = maxWidth;
          pLine->LiHeight = pNextBox->BxHeight;
          pLine->LiRealLength = pNextBox->BxWidth;
          /* the paragraph should be large enough
             ( for math with display:block by example) */
          if (pChildAb->AbNotInLine &&
              pChildAb->AbDisplay != 'U')
            {
              if (pNextBox->BxWidth > pBox->BxMaxWidth)
                pBox->BxMaxWidth = pNextBox->BxWidth;
              if (pBox->BxMinWidth < pNextBox->BxWidth)
                pBox->BxMinWidth = pNextBox->BxWidth;
              pLine->LiXOrg = left;
              x = left;
              if (pChildAb->AbHorizPos.PosEdge == VertMiddle &&
                  pChildAb->AbHorizPos.PosRefEdge == VertMiddle &&
                  pBox->BxWidth > pNextBox->BxWidth)
                x += (pBox->BxWidth - pNextBox->BxWidth) / 2;
              if (Propagate != ToSiblings || pBox->BxVertFlex)
                x += pBox->BxXOrg;
              XMove (pNextBox, pBox, x - pNextBox->BxXOrg, frame);
            }
          else
            pLine->LiXOrg = pBox->BxXOrg + left;
	  
          if (Propagate != ToSiblings || pBox->BxVertFlex)
            org = pBox->BxYOrg + *height + top;
          else
            org = *height + top;
          YMove (pNextBox, pBox, org - pNextBox->BxYOrg, frame);
          *height += pLine->LiHeight;
          pBoxToBreak = NULL;
        }
      else if (!pChildAb->AbNotInLine)
        {
          /* line indent */
          pLine->LiXOrg = left;
          if (prevLine || pAb->AbTruncatedHead || indent >= width)
            indent = 0;
          if (pNextBox && (pNextBox->BxType == BoScript ||
                           pNextBox->BxType == BoPiece ||
                           pNextBox->BxType == BoDotted))
            {
              pLine->LiFirstPiece = pNextBox;
              pLine->LiFirstBox = pChildAb->AbBox;
            }
          else
            {
              pLine->LiFirstBox = pNextBox;
              if (pBoxToBreak && pBoxToBreak->BxAbstractBox &&
                  pBoxToBreak->BxAbstractBox->AbBox == pNextBox)
                pLine->LiFirstPiece = pBoxToBreak;
              else
                pLine->LiFirstPiece = NULL;
            }

          /* Fill the line */
          minWidth = FillLine (pLine, pNextBox, pBox, pRootAb, maxWidth,
                               extensibleBox,
                               xAbs, yAbs, pAb->AbTruncatedTail, onlySpace,
                               &full, &toAdjust, &breakLine, &newblock, frame,
                               indent, top, bottom, left, right,
                               &floatL, &floatR);
          // update the min width of the block
          if (pBox->BxMinWidth < minWidth)
            pBox->BxMinWidth = minWidth;
          // check if there is an enclosing ghost block
          pRefBlock = GetEnclosingBlock (pNextBox->BxAbstractBox, pAb);
          if (prevLine)
            {
              /* initial position */
              org = prevLine->LiYOrg + prevLine->LiHeight;
              if (pLine->LiYOrg == org)
                {
                  if (!newblock && pRefBlock->AbBox->BxType == BoBlock)
                    {
                      /* line position not updated by floated boxes */
                      if (pRefBlock != pAb && pRefBlock->AbInLine)
                        lspacing = PixelValue (pAb->AbLineSpacing, pAb->AbLineSpacingUnit,
                                               pAb, zoom);
                      else
                        lspacing = lineSpacing;
                      /* position when line spacing applies */
                      org = prevLine->LiYOrg + prevLine->LiHorizRef + lspacing - pLine->LiHorizRef;
                      if (org > pLine->LiYOrg ||
                          (!prevLine->LiNoOverlap && !pLine->LiNoOverlap && !standard))
                        /* apply the rule of line spacing */
                        pLine->LiYOrg = org;
                    }
                  else if (newblock && prevLine->LiLastBox && pLine->LiFirstBox)
                    {
                      prevBlock = GetEnclosingBlock (prevLine->LiLastBox->BxAbstractBox, pAb);
                      lspacing = 0;
                      // keep the max of top and bottom margins
                      if (pRefBlock != pAb && prevBlock != pAb &&
                          pRefBlock != prevBlock &&
                          pRefBlock->AbBox->BxTMargin > 0 &&
                          prevBlock->AbBox->BxBMargin > 0)
                        {
                          if (pRefBlock->AbBox->BxTMargin > prevBlock->AbBox->BxBMargin)
                            lspacing = prevBlock->AbBox->BxBMargin;
                          else
                            lspacing = pRefBlock->AbBox->BxBMargin;
                        }
                      pLine->LiYOrg -= lspacing;
                    }
                }
            }
          /* prepare information for the next line */
          *height = pLine->LiYOrg + pLine->LiHeight - top;
	  
          /* is there a broken box? */
          if (pLine->LiLastPiece)
            pBoxToBreak = pLine->LiLastPiece->BxNexChild;
          else
            pBoxToBreak = NULL;
          /* Update the no wrapped width of the block */
          noWrappedWidth += pLine->LiRealLength;
          if (!breakLine && pBoxToBreak)
            {
              /* take undisplayed spaces into account */
              lostPixels = pBoxToBreak->BxFirstChar - pLine->LiLastPiece->BxNChars - pLine->LiLastPiece->BxFirstChar;
              if (lostPixels != 0)
                {
                  lostPixels = lostPixels * BoxCharacterWidth (SPACE, 1, pBoxToBreak->BxFont);
                  noWrappedWidth += lostPixels;
                }
              if (pLine->LiLastPiece->BxType == BoDotted)
                /* remove the dash width */
                noWrappedWidth -= BoxCharacterWidth (173, 1, pBoxToBreak->BxFont);
            }
          if (breakLine || !full)
            {
              if (noWrappedWidth > pBox->BxMaxWidth || pLine == pBox->BxFirstLine)
                pBox->BxMaxWidth = noWrappedWidth;
              noWrappedWidth = 0;
            }
	  
          /* Take into account the text-align */
          if (toAdjust && pRefBlock->AbInLine && !pBox->BxShrink &&
              pRefBlock->AbAdjust == AlignJustify && !newblock &&
              (full ||
               pRefBlock->AbTruncatedTail || pLine->LiRealLength > pLine->LiXMax))
            {
              box = pLine->LiFirstBox;
              if (box && box == pLine->LiLastBox &&
                  box->BxAbstractBox &&
                  box->BxAbstractBox->AbLeafType == LtCompound &&
                  box->BxAbstractBox->AbFloat == 'N' &&
                  !ExtraFlow (box, frame) &&
                  (box->BxAbstractBox->AbDisplay == 'B' ||
                   box->BxType == BoTable))
                Align (pBox, pRefBlock->AbBox, pLine, frame, xAbs, yAbs);
              else
                Adjust (pBox, pRefBlock->AbBox, pLine, frame, xAbs, yAbs);
            }
          else
            {
              if (!pRefBlock->AbWidth.DimIsPosition && pRefBlock->AbWidth.DimMinimum &&
                  pLine->LiRealLength > pBox->BxW)
                {
                  pBox->BxContentWidth = TRUE;
                  pLine->LiXMax = pLine->LiRealLength;
                }
              //if (!extensibleBox)
              Align (pBox, pRefBlock->AbBox, pLine, frame, xAbs, yAbs);
            }
        }

      /* is there a breaked box */
      pNextBox = pLine->LiLastBox;
      if (pNextBox &&
          (pBoxToBreak == NULL ||
           (pBoxToBreak->BxNChars == 0 && !breakLine)))
        {
          /* skip to the next box */
          pBoxToBreak = NULL;
          /* skip not in line boxes */
          do
            pNextBox = GetNextBox (pNextBox->BxAbstractBox, frame);
          while (pNextBox &&
                 pNextBox->BxAbstractBox->AbFloat == 'N' &&
                 !ExtraFlow (pNextBox, frame) &&
                 pNextBox->BxAbstractBox->AbNotInLine &&
                 pNextBox->BxAbstractBox->AbDisplay == 'U');
        }
      
      if (full)
        {
          /* It's the last line */
          if (pNextBox == NULL)
            {
              /* Il n'y a plus de boite a traiter */
              full = FALSE;
              pBox->BxLastLine = pLine;
            }
          else
            {
              /* prepare la ligne suivante */
              prevLine = pLine;
              GetLine (&pLine);
              prevLine->LiNext = pLine;
            }
        }
      else
        {
          pBox->BxLastLine = pLine;
          /* Note la largeur de la fin de bloc si le remplissage est demande */
          if (pAb->AbAdjust == AlignLeftDots)
            {
              if (pLine->LiLastPiece != NULL)
                pNextBox = pLine->LiLastPiece;
              else
                {
                  pNextBox = pLine->LiLastBox;
                  /* regarde si la derniere boite est invisible */
                  while (pNextBox->BxWidth == 0 &&
                         pNextBox != pLine->LiFirstBox &&
                         pNextBox != pLine->LiFirstPiece)
                    pNextBox = pNextBox->BxPrevious;
                }
              pNextBox->BxEndOfBloc = pBox->BxXOrg + width - pNextBox->BxXOrg - pNextBox->BxW;
            }
        }
    }
  
  if (noWrappedWidth > pBox->BxMaxWidth)
    pBox->BxMaxWidth = noWrappedWidth;
  noWrappedWidth = 0;

  /* update the block baseline */
  if (pAb->AbHorizRef.PosAbRef == pAb->AbFirstEnclosed && pAb->AbFirstEnclosed &&
      pAb->AbFirstEnclosed->AbFloat == 'N' && pBox->BxFirstLine)
    {
      pPosAb = &pAb->AbHorizRef;
      if (pPosAb->PosUnit == UnPercent)
        y = PixelValue (pPosAb->PosDistance, UnPercent,
                        (PtrAbstractBox) (pAb->AbBox->BxW), 0);
      else
        y = PixelValue (pPosAb->PosDistance, pPosAb->PosUnit, pAb, zoom);
      if (pPosAb->PosDeltaUnit == UnPercent)
        y += PixelValue (pPosAb->PosDistDelta, UnPercent,
                         (PtrAbstractBox) (pAb->AbBox->BxW), 0);
      else
        y += PixelValue (pPosAb->PosDistDelta, pPosAb->PosDeltaUnit, pAb, zoom);
      y += top;
      MoveHorizRef (pBox, NULL,
                    pBox->BxFirstLine->LiHorizRef + y - pBox->BxHorizRef, frame);
    }

  /* now add margins, borders and paddings to min and max widths */
  if (pBox->BxLMargin < 0)
    left -= pBox->BxLMargin;
  if (pBox->BxRMargin < 0)
    right -= pBox->BxRMargin;
  
  // first compute min and max width
  if (extensibleBox || pBox->BxShrink)
    pBox->BxW = pBox->BxMaxWidth;
  pBox->BxMinWidth += left + right;
  // check if the block incluses only one line
  pBox->BxMaxWidth += left + right;
  UpdateBlockWithFloat (frame, pBox, xAbs, yAbs, TRUE, height);

  // if needed restore the previous width to apply right update
  if (extensibleBox)
    {
      // Restore the previous width
      pBox->BxW = width;
      // align all lines content
      pLine = pBox->BxFirstLine;
      while (pLine)
        {
          pLine->LiXMax = pBox->BxMaxWidth;
          Align (pBox, pBox, pLine, frame, xAbs, yAbs);
          pLine = pLine->LiNext;
        }
    }
  else if (pBox->BxShrink)
    {
      // Restore the previous width
      pBox->BxW = width;
    }
  // return the full height
  *height = *height + top + spacing + bottom;
  /* restore the value */
  pBox->BxCycles--;
}


/*----------------------------------------------------------------------
  ShiftLine moves the line contents of x pixels.
  ----------------------------------------------------------------------*/
static void ShiftLine (PtrLine pLine, PtrAbstractBox pAb, PtrBox pBox,
                       int x, int frame)
{
  PtrBox              box;
  PtrBox              pLastBox, pBlock;
  ViewFrame          *pFrame;
  int                 i;
  ThotBool            status;

  pLastBox = NULL;
  pLine->LiRealLength += x;
  box = pLine->LiFirstBox;
  /* prepare the redisplay of the line */
  status = ReadyToDisplay;
  ReadyToDisplay = FALSE;
  /* displayed area */
  pFrame = &ViewFrameTable[frame - 1];
  pBlock = pAb->AbBox;
  /* default clipping on the whole box */
  DefBoxRegion (frame, pBox, -1, -1, -1, -1);

  if (pAb->AbAdjust == AlignCenter ||
      (pAb->AbHorizPos.PosEdge == VertMiddle &&
       pAb->AbHorizPos.PosEdge == VertRef &&
       box == pLine->LiLastBox && box->BxAbstractBox &&
       box->BxAbstractBox->AbLeafType == LtCompound &&
       box->BxAbstractBox->AbFloat == 'N' &&
       !ExtraFlow (box, frame) &&
       (box->BxAbstractBox->AbDisplay == 'B' ||
        box->BxType == BoTable)))
    {
      /* Redisplay the whole line */
      i = x;
      x = (pLine->LiXMax - pLine->LiRealLength) / 2 - (pLine->LiXMax + i - pLine->LiRealLength) / 2;
      if (pLine->LiFirstPiece == NULL)
        box = pLine->LiFirstBox;
      else
        box = pLine->LiFirstPiece;
      if (pLine->LiLastPiece == NULL)
        pLastBox = pLine->LiLastBox;
      else
        pLastBox = pLine->LiLastPiece;

      /* extend the clipping to the whole line */
      DefBoxRegion (frame, pBlock, -1, -1,
                    pLine->LiYOrg, pLine->LiYOrg + pLine->LiHeight);
      /* move previous boxes */
      XMove (box, NULL, x, frame);
      while (box != pBox)
        {
          if (box->BxType == BoScript && box->BxNexChild)
            /* get the next child */
            box = box->BxNexChild;
          else
            box = GetNextBox (box->BxAbstractBox, frame);
          if (box->BxType == BoSplit || box->BxType == BoMulScript)
            box = box->BxNexChild;
          if (box &&
              !box->BxAbstractBox->AbNotInLine &&
              box->BxAbstractBox->AbFloat == 'N' &&
              !ExtraFlow (box, frame) &&
              box->BxAbstractBox->AbHorizEnclosing)
            XMove (box, NULL, x, frame);
        }
      /* move next boxes */
      x = -x;
    }
  else if (pAb->AbAdjust == AlignLeft || pAb->AbAdjust == AlignJustify ||
      pAb->AbAdjust == AlignLeftDots ||
      /* extended blocks are left aligned */
      pBlock->BxContentWidth)
    {
      /* Redisplay the end of the line*/
      if (pLine->LiLastPiece == NULL)
        pLastBox = pLine->LiLastBox;
      else
        pLastBox = pLine->LiLastPiece;
      if (pAb->AbAdjust == AlignLeftDots && pLine->LiNext == NULL)
        /* dotted area */
        pLastBox->BxEndOfBloc -= x;

      /* extend the clipping to the last box */
      UpdateBoxRegion (frame, pLastBox, x, 0, 0, 0);
    }
  else if (pAb->AbAdjust == AlignRight)
    {
      /* Redisplay the beginning of the line */
      x = -x;
      pLastBox = pBox;
      if (pLine->LiFirstPiece == NULL)
        pBox = pLine->LiFirstBox;
      else
        pBox = pLine->LiFirstPiece;

      /* extend the clipping to the last box */
      UpdateBoxRegion (frame, pBox, x, 0, 0, 0);
      XMove (pBox, NULL, x, frame);
    }

  /* decale les boites suivantes */
  while (pBox != pLastBox && pBox)
    {
      if (pBox->BxType == BoScript && pBox->BxNexChild)
        /* get the next child */
        pBox = pBox->BxNexChild;
      else
        pBox = GetNextBox (pBox->BxAbstractBox, frame);
      if (pBox && pBox->BxNexChild &&
          (pBox->BxType == BoSplit || pBox->BxType == BoMulScript))
        pBox = pBox->BxNexChild;
      if (pBox &&
         !pBox->BxAbstractBox->AbNotInLine &&
          pBox->BxAbstractBox->AbFloat == 'N' &&
          !ExtraFlow (pBox, frame) &&
          pBox->BxAbstractBox->AbHorizEnclosing)
        XMove (pBox, NULL, x, frame);
    }
  ReadyToDisplay = status;
}


/*----------------------------------------------------------------------
  CompressLine compresse ou e'tend la ligne justifiee suite a`    
  l'ajout d'espaces et un ecart de xDelta pixels.         
  ----------------------------------------------------------------------*/
static void CompressLine (PtrLine pLine, PtrAbstractBox pAb, int xDelta,
                          int frame, int spaceDelta)
{
  PtrBox              box, pBox, pBlock;
  ViewSelection      *pViewSel;
  int                 nSpaces, diff;
  int                 limit, opixel;
  int                 spaceValue, remainder;
  ThotBool            status;

  remainder = pLine->LiNPixels - xDelta;
  spaceValue = 0;
  nSpaces = pLine->LiNSpaces;
  status = ReadyToDisplay;
  ReadyToDisplay = FALSE;
  pBlock = pAb->AbBox;

  /* Faut-il changer la largeur des blancs ? */
  if (xDelta > 0)
    while (remainder < 0)
      {
        remainder += nSpaces;
        spaceValue++;
      }
  else
    while (nSpaces && remainder >= nSpaces)
      {
        remainder -= nSpaces;
        spaceValue--;
      }

  /* met a jour la ligne */
  pLine->LiNPixels = remainder;
  /* pour chaque blanc insere ou retire on compte la largeur minimale */
  xDelta -= spaceDelta * (pLine->LiSpaceWidth - SPACE_VALUE_MIN);
  pLine->LiMinLength += xDelta;
  pLine->LiSpaceWidth -= spaceValue;

  /* prepare le reaffichage de toute la ligne */
  if (pLine->LiFirstPiece != NULL)
    pBox = pLine->LiFirstPiece;
  else
    pBox = pLine->LiFirstBox;
   
  limit = pBox->BxXOrg;
  /* extend the clipping to the whole line */
  DefBoxRegion (frame, pBlock, -1, -1,
                pLine->LiYOrg, pLine->LiYOrg + pLine->LiHeight);
  /* met a jour chaque boite */
  do
    {
      if (pBox->BxType == BoSplit || pBox->BxType == BoMulScript)
        box = pBox->BxNexChild;
      else
        box = pBox;

      if (!pBox->BxAbstractBox->AbNotInLine)
        {
          XMove (box, NULL, limit - box->BxXOrg, frame);
          if (box->BxAbstractBox->AbLeafType == LtText &&
              box->BxNChars != 0)
            {
              diff = box->BxNSpaces * spaceValue;
              box->BxW -= diff;
              box->BxWidth -= diff;
              box->BxSpaceWidth = pLine->LiSpaceWidth;
		
              /* Repartition des pixels */
              opixel = box->BxNPixels;
              if (remainder > box->BxNSpaces)
                box->BxNPixels = box->BxNSpaces;
              else
                box->BxNPixels = remainder;
              box->BxW = box->BxW - opixel + box->BxNPixels;
              box->BxWidth = box->BxWidth - opixel + box->BxNPixels;
              remainder -= box->BxNPixels;
		
              /* Faut-il mettre a jour les marques de selection ? */
              pViewSel = &ViewFrameTable[frame - 1].FrSelectionBegin;
              if (pViewSel->VsBox == box)
                {
                  pViewSel->VsXPos -= pViewSel->VsNSpaces * spaceValue;
                  if (opixel < pViewSel->VsNSpaces)
                    pViewSel->VsXPos -= opixel;
                  else
                    pViewSel->VsXPos -= pViewSel->VsNSpaces;
                  if (box->BxNPixels < pViewSel->VsNSpaces)
                    pViewSel->VsXPos += box->BxNPixels;
                  else
                    pViewSel->VsXPos += pViewSel->VsNSpaces;
                }

              pViewSel = &ViewFrameTable[frame - 1].FrSelectionEnd;
              if (pViewSel->VsBox == box)
                {
                  pViewSel->VsXPos -= pViewSel->VsNSpaces * spaceValue;
                  if (opixel < pViewSel->VsNSpaces)
                    pViewSel->VsXPos -= opixel;
                  else
                    pViewSel->VsXPos -= pViewSel->VsNSpaces;
                  if (box->BxNPixels < pViewSel->VsNSpaces)
                    pViewSel->VsXPos += box->BxNPixels;
                  else
                    pViewSel->VsXPos += pViewSel->VsNSpaces;
                  if (pViewSel->VsIndBox < box->BxNChars &&
                      pViewSel->VsBuffer->BuContent[pViewSel->VsIndBuf] == SPACE)
                    pViewSel->VsXPos -= spaceValue;
                }
            }
          limit += box->BxWidth;
        }
      if (box->BxType == BoScript && box->BxNexChild)
        /* get the next child */
        pBox = box->BxNexChild;
      else
        pBox = GetNextBox (box->BxAbstractBox, frame);
    }
  while (pBox && box != pLine->LiLastBox && box != pLine->LiLastPiece);
  ReadyToDisplay = status;
}


/*----------------------------------------------------------------------
  RemoveLines frees lines generated within a block of line starting from
  (and including) the first line pFirstLine.
  Free all piece boxes.
  The parameter removed is TRUE when the enclosing box will be removed.
  Parameters changeSelectBegin and changeSelectEnd are TRUE when the
  beginning and the end of the selection must be updated.
  ----------------------------------------------------------------------*/
void RemoveLines (PtrBox pBox, int frame, PtrLine pFirstLine,
                  ThotBool removed, ThotBool *changeSelectBegin,
                  ThotBool *changeSelectEnd)
{
  PtrBox              box = NULL;
  PtrLine             pNextLine;
  PtrLine             pLine;

  *changeSelectBegin = FALSE;
  *changeSelectEnd = FALSE;

  pLine = pFirstLine;
  if (pLine &&
      (pBox->BxType == BoBlock ||
       pBox->BxType == BoFloatBlock || pBox->BxType == BoCellBlock))
    {
      if (pLine->LiFirstPiece &&
          (pLine->LiFirstBox->BxType == BoScript ||
           pLine->LiFirstBox->BxNexChild != pLine->LiFirstPiece))
        /* start with a piece of box */
        box = pLine->LiFirstPiece;
      else
        {
          box = pLine->LiFirstBox;
          pLine->LiFirstPiece = NULL;
        }
      RemoveBreaks (box, frame, removed, changeSelectBegin, changeSelectEnd);
      if (pLine->LiPrevious)
        pBox->BxLastLine = pLine->LiPrevious;
      else
        {
          pBox->BxFirstLine = NULL;
          pBox->BxLastLine = NULL;
        }

      /* free all lines */
      while (pLine)
        {
          pNextLine = pLine->LiNext;
          pLine->LiNext = NULL;
          FreeLine (pLine);
          pLine = pNextLine;
        }
      /* Liberation des boites de coupure suivantes */
      if (box)
        {
          if (box->BxType == BoScript && box->BxNexChild)
            /* get the next child */
            box = box->BxNexChild;
          else
            box = GetNextBox (box->BxAbstractBox, frame);
        }      /* update the lines chaining */
    }
  else if (pBox->BxAbstractBox && pBox->BxAbstractBox->AbFirstEnclosed)
    // it could be a new block element
    box = pBox->BxAbstractBox->AbFirstEnclosed->AbBox;

  while (box)
    {
      RemoveBreaks (box, frame, removed,
                    changeSelectBegin, changeSelectEnd);
      if (box->BxType == BoScript && box->BxNexChild)
        /* get the next child */
        box = box->BxNexChild;
      else
        box = GetNextBox (box->BxAbstractBox, frame);
    }
}

/*----------------------------------------------------------------------
  RecomputeLines recomputes a part or the whole block of lines after
  a change in the box ibox.
  Return TRUE when the block width was shrunk.
  ----------------------------------------------------------------------*/
ThotBool RecomputeLines (PtrAbstractBox pAb, PtrLine pFirstLine, PtrBox ibox,
                     int frame)
{
  Propagation         propagateStatus;
  CHAR_T              c;
  PtrLine             pLine;
  PtrBox              pBox, pSelBox;
  PtrAbstractBox      pParent;
  ViewFrame          *pFrame;
  ViewSelection      *pSelBegin;
  ViewSelection      *pSelEnd;
  int                 w, h, height, width, l, variant;
  ThotBool            changeSelectBegin, changeSelectEnd;
  ThotBool            status, extensibleBox, shrunk = FALSE;


  /* Si la boite est eclatee, on remonte jusqu'a la boite bloc de lignes */
  while (pAb->AbBox->BxType == BoGhost ||
         pAb->AbBox->BxType == BoStructGhost ||
         pAb->AbBox->BxType == BoFloatGhost)
    pAb = pAb->AbEnclosing;
  variant = pAb->AbFontVariant;
  pBox = pAb->AbBox;
  if (pBox)
    {
      if (pBox->BxCycles > 0)
        /* the block of lines is currently computed */
        return shrunk;
      if (pBox->BxFirstLine == NULL)
        pLine = NULL;
      else
        {
          if (pFirstLine == NULL)
            pLine = pBox->BxFirstLine;
          else
            {
              /* regarde si cette ligne fait bien parti de ce bloc de lignes */
              pLine = pBox->BxFirstLine;
              while (pLine != pFirstLine && pLine)
                pLine = pLine->LiNext;
              if (pLine == NULL)
                /* cette ligne ne fait plus parti du bloc de lignes */
                return shrunk;
            }
        }

      /* Zone affichee avant modification */
      if (pLine == NULL)
        {
          w = 0;
          h = 0;
        }
      else
        {
          w = pLine->LiXOrg + pLine->LiXMax;
          h = pLine->LiYOrg;
        }

      /* Si l'origne de la reevaluation du bloc de ligne vient d'une boite */
      /* de coupure, il faut deplacer cette origne sur la boite coupee     */
      /* parce que RemoveLines va liberer toutes les boites de coupure.    */
      if (ibox &&
          (ibox->BxType == BoPiece || ibox->BxType == BoDotted ||
           ibox->BxType == BoScript))
        ibox = ibox->BxAbstractBox->AbBox;

      status = ReadyToDisplay;
      ReadyToDisplay = FALSE;
      /* update the clipping region */
      if (pLine)
        DefBoxRegion (frame, pBox, -1, -1, pLine->LiYOrg, pBox->BxHeight);
      else
        DefBoxRegion (frame, pBox, -1, -1, -1, -1);

      /* free floating contexts */
      RemoveLines (pBox, frame, pLine, FALSE, &changeSelectBegin,
                   &changeSelectEnd);
      ComputeLines (pBox, frame, &height);
      extensibleBox = (pBox->BxContentWidth ||
                       (!pAb->AbWidth.DimIsPosition && pAb->AbWidth.DimMinimum));
      if (extensibleBox)
        /* it's an extensible block of lines */
        width = pBox->BxMaxWidth;
      else if (pBox->BxMinWidth > pBox->BxW)
        {
          width = pBox->BxMinWidth;
          pParent = pAb->AbEnclosing;
#ifdef IV
          // skip struct ghosts
          while (pParent && pParent->AbBox &&
                 pParent->AbBox->BxType == BoStructGhost)
            {
              pParent->AbBox->BxMinWidth = pBox->BxMinWidth;
              pParent = pParent->AbEnclosing;
            }
#endif
          if (pParent && pParent->AbBox &&
              pBox->BxMinWidth > pParent->AbBox->BxMinWidth &&
              pParent->AbWidth.DimAbRef &&
              (pParent->AbBox->BxType == BoBlock ||
               pParent->AbBox->BxType == BoFloatBlock ||
               pParent->AbBox->BxType == BoCellBlock))
            pParent->AbBox->BxMinWidth = pBox->BxMinWidth;
        }
      else
        /* no update */
        width = 0;
      ReadyToDisplay = status;

      /* Zone affichee apres modification */
      /* Il ne faut pas tenir compte de la boite si elle */
      /* n'est pas encore placee dans l'image concrete   */
      if (ReadyToDisplay && !pBox->BxXToCompute && !pBox->BxYToCompute)
        {
          if (extensibleBox)
            w = width;
          if (pBox->BxWidth > w)
            w = pBox->BxWidth;
          if (height > pBox->BxHeight)
            DefBoxRegion (frame, pBox, 0, w, h, height);
          else
            DefBoxRegion (frame, pBox, 0, w, h, pBox->BxHeight);
        }

      /* Faut-il reevaluer les marques de selection ? */
      pFrame = &ViewFrameTable[frame - 1];
      pSelBegin = &pFrame->FrSelectionBegin;
      if (changeSelectBegin && pSelBegin->VsBox)
        {
          /* Si la selection a encore un sens */
          if (pSelBegin->VsBox->BxAbstractBox)
            ComputeViewSelMarks (pSelBegin, frame);

          pSelEnd = &pFrame->FrSelectionEnd;
          if (changeSelectEnd && pSelEnd->VsBox)
            {
              /* Si la selection a encore un sens */
              if (pSelEnd->VsBox->BxAbstractBox)
                {
                  if (pSelEnd->VsBuffer == pSelBegin->VsBuffer &&
                      pSelEnd->VsIndBuf == pSelBegin->VsIndBuf)
                    {
                      pSelEnd->VsBox = pSelBegin->VsBox;
                      pSelEnd->VsIndBox = pSelBegin->VsIndBox;
                      pSelEnd->VsLine = pSelBegin->VsLine;
                      pSelEnd->VsXPos = pSelBegin->VsXPos;
                      pSelEnd->VsNSpaces = pSelBegin->VsNSpaces;
                    }
                  else
                    ComputeViewSelMarks (pSelEnd, frame);
		     
                  /* Recherche la position limite du caractere */
                  pSelBox = pSelEnd->VsBox;
                  if ((pSelBox->BxNChars == 0 ||  pSelEnd->VsIndBox == 0) &&
                      pSelBox->BxType == BoComplete)
                    pSelEnd->VsXPos += pSelBox->BxW;
                  else if (pSelBegin->VsIndBox >= pSelBox->BxNChars)
                    /* select the end of the box */
                    pSelEnd->VsXPos += 2;
                  else if (SelPosition)
                    pSelEnd->VsXPos += 2;
                  else if (pSelEnd->VsIndBox < pSelBox->BxNChars)
                    {
                      c = (pSelEnd->VsBuffer->BuContent[pSelEnd->VsIndBuf]);
                      if (c == SPACE && pSelBox->BxSpaceWidth != 0)
                        pSelEnd->VsXPos += pSelBox->BxSpaceWidth;
                      else
			   
                        if (c >= 0x060C && c <= 0x06B0) /* arabic char */
                          pSelEnd->VsXPos += BoxArabicCharacterWidth (c, &(pSelEnd->VsBuffer), &(pSelEnd->VsIndBuf), pSelBox->BxFont);
                        else
                          pSelEnd->VsXPos += BoxCharacterWidth (c, variant, pSelBox->BxFont);
                    }
                }
            }
        }

      l = pBox->BxLBorder + pBox->BxLPadding + pBox->BxRBorder + pBox->BxRPadding;
      if (pBox->BxLMargin > 0)
        l += pBox->BxLMargin;
      if (pBox->BxRMargin > 0)
        l += pBox->BxRMargin;
      if (pBox->BxShrink && pBox->BxMaxWidth - l > 0)
        {
          if (pBox->BxMaxWidth - l < pBox->BxRuleWidth)
            w = pBox->BxMaxWidth - l;
          else
            w = pBox->BxRuleWidth;
          // the box is shrunk only if there is only one line
          shrunk = (w != pBox->BxW && pBox->BxFirstLine == pBox->BxLastLine);
          if (shrunk)
            {
              int x = pBox->BxXOrg;
              ResizeWidth (pBox, ibox, NULL, w - pBox->BxW, 0, 0, 0, frame, TRUE);
              ShiftFloatingBoxes (pBox, pBox->BxXOrg - x, frame);
            }
        }
      else if (width != 0 && width != pBox->BxWidth)
        {
          pBox->BxCycles = 1;
          ChangeDefaultWidth (pBox, ibox, width - l, 0, frame);
          pBox->BxCycles = 0;
        }

      /* Faut-il conserver la hauteur ? */
      if (height != 0 && height != pBox->BxHeight)
        {
          /* Il faut propager la modification de hauteur */
          propagateStatus = Propagate;
          height -= pBox->BxHeight;
          /* We certainly need to re-check the height of enclosing elements */
          ChangeDefaultHeight (pBox, ibox, pBox->BxH + height, frame);
          Propagate = propagateStatus;
        }
    }
  return shrunk;
}


/*----------------------------------------------------------------------
  UpdateLineBlock updates the block of lines pAb after the width change of
  the included box pBox.
  The parameter xDelta gives the value of the width change.
  ----------------------------------------------------------------------*/
void UpdateLineBlock (PtrAbstractBox pAb, PtrLine pLine, PtrBox pBox,
                      int xDelta, int spaceDelta, int frame)
{
  PtrTextBuffer       pNewBuff;
  PtrBox              box;
  PtrBox              pParentBox;
  AbDimension        *pDimAb;
  PtrLine             pLi2;
  int                 lostPixels;
  int                 maxlostPixels;
  int                 length;
  int                 realLength = 0;
  int                 width, maxLength;
  int                 newIndex;
  int                 nSpaces;
  int                 l, r, t, b;
  ThotBool            hyphenate;

  /* For ghost boxes go up to the block of lines */
  while (pAb->AbBox->BxType == BoGhost ||
         pAb->AbBox->BxType == BoStructGhost ||
         pAb->AbBox->BxType == BoFloatGhost)
    pAb = pAb->AbEnclosing;
  pParentBox = pAb->AbBox;
  hyphenate = pAb->AbHyphenate;
  if (pLine)
    {
      pDimAb = &pAb->AbWidth;
      if (pParentBox->BxShrink &&
          (pParentBox->BxW != pParentBox->BxRuleWidth ||
           (xDelta < 0 && pParentBox->BxW + xDelta < pParentBox->BxRuleWidth)))
        {
          pLine = NULL;
          RecomputeLines (pAb, NULL, NULL, frame);
        }
      else if (pParentBox->BxContentWidth)
        {
          /* the line can be extended */
          if (!pDimAb->DimIsPosition && pDimAb->DimMinimum &&
              pParentBox->BxW + xDelta < pParentBox->BxRuleWidth)
            {
              /* The min width is larger than its inside width */
              /* use the min width */
              pParentBox->BxContentWidth = FALSE;
              pLine = NULL;
              RecomputeLines (pAb, NULL, NULL, frame);
            }
          else if ((pDimAb->DimUnit == UnAuto ||
                    pParentBox->BxType == BoFloatBlock ||
                    pParentBox->BxType == BoCellBlock) &&
                   pParentBox->BxW + xDelta < pParentBox->BxRuleWidth)
            {
              /* The block  width must be recomputed */
              pLine = NULL;
              RecomputeLines (pAb, NULL, NULL, frame);
            }
          else
            {
              ShiftLine (pLine, pAb, pBox, xDelta, frame);
              pLine->LiXMax = pLine->LiRealLength;
              ResizeWidth (pParentBox, pParentBox, NULL, xDelta, 0, 0, 0, frame, FALSE);
            }
        }
      else if (!pDimAb->DimIsPosition && pDimAb->DimMinimum
               && pLine->LiRealLength + xDelta > pLine->LiXMax)
        {
          /* The block inside width is larger than its min width */
          /* use the inside width */
          pParentBox->BxContentWidth = TRUE;
          RecomputeLines (pAb, NULL, NULL, frame);
        }
      else
        {
          /* cannot extend the line width */
          /* check the room is available in the current line */
          pLine->LiNSpaces += spaceDelta;
          maxlostPixels = pLine->LiNSpaces * SPACE_VALUE_MAX + xDelta;
          if (pLine->LiSpaceWidth > 0)
            {
              /* this line is already justified */
              lostPixels = BoxCharacterWidth (SPACE, 1, pBox->BxFont);
              realLength = pLine->LiRealLength + xDelta - spaceDelta * (pLine->LiSpaceWidth - lostPixels);
              lostPixels = pLine->LiXMax - pLine->LiMinLength;
            }
          else
            lostPixels = pLine->LiXMax - pLine->LiRealLength;
	  
          if (pBox->BxWidth - xDelta > pLine->LiXMax)
            {
              /* The box is too large */
              RecomputeLines (pAb, pLine, NULL, frame);
            }
          else if ((xDelta > 0 && xDelta <= lostPixels) ||
                   (xDelta < 0 &&
                    (lostPixels < maxlostPixels ||
                     pLine->LiPrevious == NULL) &&
                    pLine->LiNext == NULL))
            {
              /* compress or complete the current line */
              if (pLine->LiSpaceWidth == 0)
                {
                  if (pBox == pLine->LiFirstBox && pBox == pLine->LiLastBox &&
                      (pBox->BxAbstractBox->AbHorizPos.PosEdge == VertMiddle ||
                       (pBox->BxAbstractBox->AbLeftMarginUnit == UnAuto &&
                        pBox->BxAbstractBox->AbRightMarginUnit == UnAuto)))
                    XMove (pBox, NULL, -xDelta/2, frame);
                  else
                    ShiftLine (pLine, pAb, pBox, xDelta, frame);
                }
              else
                {
                  CompressLine (pLine, pAb, xDelta, frame, spaceDelta);
                  pLine->LiRealLength = realLength;
                }
            }
          else if (xDelta < 0)
            {
              /* Avoid to recompute the whole block of lines */
              /* try to move the first word into the previous line */
              if (pLine->LiPrevious)
                {
                  /* available width */
                  maxLength = pLine->LiPrevious->LiXMax - pLine->LiPrevious->LiRealLength - SPACE_VALUE_MAX;
                  if (pLine->LiFirstPiece)
                    box = pLine->LiFirstPiece;
                  else
                    box = pLine->LiFirstBox;
                  if (box->BxWidth > maxLength)
                    {
                      if (box->BxAbstractBox->AbLeafType == LtText &&
                          maxLength > 0)
                        {
                          if (pLine->LiNSpaces == 0)
                            /* No space in the line -> recompute lines */
                            maxLength = 1;
                          else
                            {
                              length = box->BxNChars;
                              /* look for a break */
                              GetExtraMargins (box, frame, FALSE, &t, &b, &l, &r);
                              maxLength = SearchBreak (pLine, box,
                                                       maxLength, box->BxFont,
                                                       l, r, 0,
                                                       hyphenate,
                                                       &length, &width, &nSpaces,
                                                       &newIndex, &pNewBuff);
                            }
			  
                          if (maxLength > 0)
                            RecomputeLines (pAb, pLine->LiPrevious, NULL, frame);
                        }
                      else
                        maxLength = 0;
                    }
                }
              else
                maxLength = 0;
	      
              /* try to move the last word into the next line */
              if (maxLength == 0)
                {
                  if (pLine->LiNext)
                    {
                      maxLength = pLine->LiXMax - pLine->LiRealLength - xDelta;
                      pLi2 = pLine->LiNext;
                      if (pLi2->LiFirstPiece != NULL)
                        box = pLi2->LiFirstPiece;
                      else
                        box = pLi2->LiFirstBox;
		      
                      if (box && box->BxWidth > maxLength)
                        {
                          if (box->BxAbstractBox->AbLeafType == LtText &&
                              maxLength > 0)
                            {
                              if (pLi2->LiNSpaces == 0)
                                /* No space in the line -> recompute lines */
                                maxLength = 1;
                              else
                                {
                                  length = box->BxNChars;
                                  /* look for a break */
                                  GetExtraMargins (box, frame, FALSE, &t, &b, &l, &r);
                                  maxLength = SearchBreak (pLi2, box,
                                                           maxLength, box->BxFont,
                                                           l, r, 0,
                                                           hyphenate,
                                                           &length, &width, &nSpaces,
                                                           &newIndex, &pNewBuff);
                                }
                            }
                          else
                            maxLength = 0;
                        }
                    }
		  
                  if (maxLength > 0)
                    RecomputeLines (pAb, pLine, NULL, frame);
                  else if (pLine->LiSpaceWidth == 0)
                    ShiftLine (pLine, pAb, pBox, xDelta, frame);
                  else
                    {
                      CompressLine (pLine, pAb, xDelta, frame, spaceDelta);
                      pLine->LiRealLength = realLength;
                    }
                }
              else
                RecomputeLines (pAb, pLine, NULL, frame);
            }
          else
            RecomputeLines (pAb, pLine, NULL, frame);
        }
    }
  else if (pParentBox &&
           (pParentBox->BxType == BoBlock ||
            pParentBox->BxType == BoFloatBlock ||
            pParentBox->BxType == BoCellBlock))
    /* it could be an empty block of lines */
    RecomputeLines (pAb, pLine, NULL, frame);   
}


/*----------------------------------------------------------------------
  EncloseInLine checks that the box pBox is still included within its
  current block of lines of pAb.
  Update the block of lines if necessary.
  ----------------------------------------------------------------------*/
void EncloseInLine (PtrBox pBox, int frame, PtrAbstractBox pAb)
{
  PtrBox              box;
  PtrBox              pPieceBox;
  PtrBox              pBlock;
  int                 ascent, descent, y, shift;
  int                 i, h, top, bottom, left, right;
  int                 pos, linespacing, spacing, zoom;
  PtrLine             pLine, prevLine;
  PtrLine             pNextLine;

  pBlock = pAb->AbBox;
  GetExtraMargins (pBlock, frame, FALSE, &top, &bottom, &left, &right);
  top += pBlock->BxTMargin + pBlock->BxTBorder + pBlock->BxTPadding;
  left += pBlock->BxLMargin + pBlock->BxLBorder + pBlock->BxLPadding;
  zoom = ViewFrameTable[frame - 1].FrMagnification;
  if (pBlock->BxType == BoBlock)
    linespacing = PixelValue (pAb->AbLineSpacing, pAb->AbLineSpacingUnit,
                              pAb, zoom);
  else
    linespacing = 0;
  pNextLine = NULL;
  h = 0;
  if (Propagate != ToSiblings || pBlock->BxVertFlex)
    {
      if (pBox->BxAbstractBox->AbFloat != 'N' ||
          pBlock->BxLeftFloat || pBlock->BxRightFloat)
        {
          RecomputeLines (pAb, NULL, NULL, frame);
          /* get the current block heigh */
          h = pBlock->BxH;
        }
      else
        {
          pLine = SearchLine (pBox, frame);
          if (pLine && (pBlock->BxLeftFloat || pBlock->BxRightFloat))
            {
              RecomputeLines (pBlock->BxAbstractBox, pLine, NULL, frame);
              h = pBlock->BxH;
            }
          else if (pLine)
            {
              pNextLine = pLine->LiNext;
              /* current limit of the line */
              y = pBlock->BxYOrg + pLine->LiYOrg + pLine->LiHeight;
              shift = 0;
              ascent = 0;
              descent = 0;
              if (!pBox->BxAbstractBox->AbHorizEnclosing)
                {
                  /* The box is out of lines (like page breaks) */
                  pLine->LiHorizRef = pBox->BxHorizRef;
                  pLine->LiHeight = pBox->BxHeight;
                  descent = pLine->LiHeight - pLine->LiHorizRef;
                  /* move the box */
                  i = pBlock->BxYOrg + pLine->LiYOrg - pBox->BxYOrg;
                  YMove (pBox, NULL, i, frame);
                  /* delta of the block height if it's the last line */
                  h = pLine->LiYOrg + pLine->LiHeight - pBlock->BxH;
                }
              else
                {
                  /* The box is split in lines */
                  /* compute the line ascent and the line descent */
                  pPieceBox = pLine->LiFirstBox;
                  if ((pPieceBox->BxType == BoSplit ||
                       pPieceBox->BxType == BoMulScript) &&
                      pLine->LiFirstPiece)
                    pPieceBox = pLine->LiFirstPiece;
                  /* loop on included boxes */
                  do
                    {
                      if (pPieceBox->BxType == BoSplit ||
                          pPieceBox->BxType == BoMulScript)
                        box = pPieceBox->BxNexChild;
                      else
                        box = pPieceBox;

                      //skip floated boxes
                      if (box->BxAbstractBox)
                        {
                          if (ascent < box->BxHorizRef)
                            ascent = box->BxHorizRef;
                          i = box->BxHeight - box->BxHorizRef;
                          if (descent < i)
                            descent = i;
                        }
                      /* next box */
                      pPieceBox = GetNextBox (box->BxAbstractBox, frame);
                    }
                  while (pPieceBox && box != pLine->LiLastBox &&
                         box != pLine->LiLastPiece);
                  if (pLine->LiPrevious)
                    {
                      /* new position of the current line */
                      prevLine = pLine->LiPrevious;
                      pos = prevLine->LiYOrg + prevLine->LiHeight;
                      i = prevLine->LiYOrg + prevLine->LiHorizRef + linespacing - pLine->LiHorizRef;
                      if (i > pos || (!prevLine->LiNoOverlap && !pLine->LiNoOverlap))
                        /* apply the rule of line spacing */
                        pos =  i;
                      /* vertical shifting of the current line baseline */
                      i = pos - pLine->LiYOrg + ascent - pLine->LiHorizRef;
                    }
                  else
                    {
                      /* new position of the current line */
                      pos = top;
                      /* vertical shifting of the current line baseline */
                      i = pos - pLine->LiYOrg + ascent - pLine->LiHorizRef;
                    }
	      
                  /* move the line contents */
                  pLine->LiYOrg = pos;
                  if (i)
                    {
                      /* align boxes of the current line */
                      pPieceBox = pLine->LiFirstBox;
                      if ((pPieceBox->BxType == BoSplit ||
                           pPieceBox->BxType == BoMulScript) &&
                          pLine->LiFirstPiece)
                        pPieceBox = pLine->LiFirstPiece;
		      
                      do
                        {
                          if (pPieceBox->BxType == BoSplit ||
                              pPieceBox->BxType == BoMulScript)
                            box = pPieceBox->BxNexChild;
                          else
                            box = pPieceBox;
			  
                          if (box != pBox)
                            YMove (box, NULL, i, frame);
                          pPieceBox = GetNextBox (box->BxAbstractBox, frame);
                        }
                      while (pPieceBox && box != pLine->LiLastBox &&
                             box != pLine->LiLastPiece);
                    }
		  
                  /* change the baseline of the current line */
                  i = ascent - pLine->LiHorizRef;
                  pLine->LiHorizRef = ascent;
                  if (i)
                    {
                      /* move the block baseline if it's inherited from the first line */
                      if (pAb->AbHorizRef.PosAbRef == pAb->AbFirstEnclosed &&
                          !pLine->LiPrevious)
                        MoveHorizRef (pBlock, NULL, i, frame);
                    }
		  
                  /* vertical shift of the box */
                  i = pBlock->BxYOrg + pLine->LiYOrg + pLine->LiHorizRef - pBox->BxHorizRef - pBox->BxYOrg;
                  if (i)
                    YMove (pBox, NULL, i, frame);		  
                  /* update the rest of the block */
                  pLine->LiHeight = descent + ascent;
                  /* delta of the block height if it's the last line */
                  h = pLine->LiYOrg + pLine->LiHeight - pBlock->BxH;
                  shift = pBlock->BxYOrg + pLine->LiYOrg + pLine->LiHeight - y;
                }

              /* Move next floated boxes */
              MoveFloatingBoxes (pBlock, y, shift, frame);

              /* Move next lines */
              if (pNextLine)
                {
                  /* new position of the next line */
                  if (!pBox->BxAbstractBox->AbHorizEnclosing ||
                      linespacing < descent + pNextLine->LiHorizRef)
                    /* we set the next line under the previous one */
                    i = descent + pNextLine->LiHorizRef;
                  else
                    i = linespacing;

                  /* new possible position of the next line */
                  pos = pLine->LiYOrg + pLine->LiHorizRef + i - pNextLine->LiHorizRef;
                  /* vertical shifting of the next lines */
                  h = pos - pNextLine->LiYOrg;
                  if (h < 0)
                    {
                      /* check if the line can be moved up */
                      box = pNextLine->LiFirstBox;
                      while (box)
                        {
                          if (box->BxAbstractBox->AbClear != 'N')
                            {
                              /* a clear forces the position of that line */
                              h = 0;
                              box = NULL;
                            }
                          else if (box->BxAbstractBox->AbFloat == 'N' &&
                                   !ExtraFlow (box, frame))
                            /* move the line */
                            box = NULL;
                          else
                            /* skip floating boxes */
                            box = GetNextBox (box->BxAbstractBox, frame);
                        }
                    }
                  if (h)
                    {
                      while (pNextLine)
                        {
                          if (pNextLine->LiFirstBox)
                            {
                              pNextLine->LiYOrg += h;
                              pPieceBox = pNextLine->LiFirstBox;
                              if ((pPieceBox->BxType == BoSplit ||
                                   pPieceBox->BxType == BoSplit) &&
                                  pNextLine->LiFirstPiece)
                                pPieceBox = pNextLine->LiFirstPiece;
                              do
                                {
                                  if (pPieceBox->BxType == BoSplit)
                                    box = pPieceBox->BxNexChild;
                                  else
                                    box = pPieceBox;
                                  YMove (box, NULL, h, frame);
                                  pPieceBox = GetNextBox (box->BxAbstractBox, frame);
                                }
                              while (pPieceBox && box != pNextLine->LiLastBox &&
                                     box != pNextLine->LiLastPiece);
                            }
                          pNextLine = pNextLine->LiNext;
                        }
                    }
                }
              /* new height */
              h += pBlock->BxH;
            }
        }
      UpdateBlockWithFloat (frame, pBlock, TRUE, TRUE, FALSE, &h);
      /* compute the line spacing */
      /* space added at the top and bottom of the paragraph */
      spacing = linespacing - GetCurrentFontHeight (pAb->AbSize, pAb->AbSizeUnit, zoom);
      if (spacing > 0)
        spacing /= 2;
      else
        spacing = 0;
      h += spacing;
      /* update the block height */
      if (pBlock->BxContentHeight)
        ChangeDefaultHeight (pBlock, pBlock, h, frame);
    }
}
