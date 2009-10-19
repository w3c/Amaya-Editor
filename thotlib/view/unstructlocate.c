/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2009
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 
/* 
 * locate what is designated in Concrete Image in unstructured mode.
 *
 * Author: I. Vatton (INRIA)
 *
 */
#include "thot_gui.h"
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
#include "edit_tv.h"
#include "platform_tv.h"
#include "appdialogue_tv.h"

#include "absboxes_f.h"
#include "appli_f.h"
#include "boxmoves_f.h"
#include "boxlocate_f.h"
#include "boxpositions_f.h"
#include "boxselection_f.h"
#include "buildboxes_f.h"
#include "buildlines_f.h"
#include "callback_f.h"
#include "changepresent_f.h"
#include "exceptions_f.h"
#include "font_f.h"
#include "geom_f.h"
#include "structcreation_f.h"
#include "tree_f.h"
#include "views_f.h"


#define MAX_DISTANCE THOT_MAXINT

/*----------------------------------------------------------------------
  GetDistance returns 0 if value is between -delta and +delta.
  In other cases returns the absolute value of value - delta
  ----------------------------------------------------------------------*/
int GetDistance (int value, int delta)
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
int GetBoxDistance (PtrBox pBox, PtrFlow pFlow, int xRef, int yRef,
                    int ratio, int frame, PtrElement *matchCell)
{
  PtrAbstractBox      pCell, row, pAb;
  PtrBox              sibling;
  PtrDocument         pDoc;
  int                 value, x, y, width, height;
  int                 xcell, ycell, wcell, hcell, view;
  int                 s, l, r, t, b;

  *matchCell = NULL;
  b = t = r = l = 0;
  if (pBox == NULL || pBox->BxAbstractBox == NULL ||
      FrameTable[frame].FrDoc == 0)
    return MAX_DISTANCE;
  /* check limits given by an enclosing cell */
  pDoc = LoadedDocument[FrameTable[frame].FrDoc - 1];
  view = pDoc->DocView[pBox->BxAbstractBox->AbDocView - 1].DvPSchemaView;
  if (view == 1)
    pCell = GetParentCell (pBox);
  else
    pCell = NULL;
  pAb = pBox->BxAbstractBox;
  xcell = ycell = 0;
  wcell = hcell = MAX_DISTANCE;
#ifdef _GL
  if (pBox->BxBoundinBoxComputed)
    {
      x = pBox->BxClipX;
      y = pBox->BxClipY;
      width = pBox->BxClipW;
      height = pBox->BxClipH;
    }
  else
    {
      x = pBox->BxXOrg - ViewFrameTable[frame-1].FrXOrg;
      y = pBox->BxYOrg - ViewFrameTable[frame-1].FrYOrg;
      width = pBox->BxW;
      height = pBox->BxH;
    }
  if (pCell && pCell->AbBox)
    {
      // get the current cell spacing
      s = 0;
      row = SearchEnclosingType (pCell, BoRow, BoRow, BoRow);
      if (row)
        {
          l = row->AbBox->BxLMargin + row->AbBox->BxLBorder + row->AbBox->BxLPadding;
          r = row->AbBox->BxRMargin + row->AbBox->BxRBorder + row->AbBox->BxRPadding;
          t = row->AbBox->BxTMargin + row->AbBox->BxTBorder + row->AbBox->BxTPadding;
          b = row->AbBox->BxBMargin + row->AbBox->BxBBorder + row->AbBox->BxBPadding;
          if (row  && row->AbPrevious)
            {
              sibling = row->AbPrevious->AbBox;
              s = row->AbBox->BxYOrg - sibling->BxYOrg - sibling->BxHeight;
            }
          else if (row  && row->AbNext)
            {
              sibling = row->AbNext->AbBox;
              s = sibling->BxYOrg - row->AbBox->BxYOrg - row->AbBox->BxHeight;
            }
        }
      /* use the frame limits */
      if (pCell && pCell->AbPrevious && pCell->AbPrevious->AbPresentationBox)
        pCell = pCell->AbPrevious;
       /* compute cell limits */
      xcell = pCell->AbBox->BxClipX - l - s / 2;
      ycell = pCell->AbBox->BxClipY - t - s / 2;
      wcell = pCell->AbBox->BxClipW + l + r + s;
      hcell = pCell->AbBox->BxClipH + t + b + s;
    }
#else /*_GL */
  x = pBox->BxXOrg;
  y = pBox->BxYOrg;
  width = pBox->BxWidth;
  height = pBox->BxHeight;
  if (pCell && pCell->AbBox)
    {
      /* get cell limits */
      xcell = pCell->AbBox->BxXOrg;
      ycell = pCell->AbBox->BxYOrg;
      wcell = pCell->AbBox->BxWidth;
      hcell = pCell->AbBox->BxHeight;
    }
#endif /*_GL */
  /* take into account the positioning */
  if (pFlow)
    {
      x += pFlow->FlXStart;
      y += pFlow->FlYStart;
    }

  if (pAb && pAb->AbElement &&
      xRef >= x && xRef <= x + width &&
      yRef >= y && yRef <= y + height &&
       TypeHasException (ExcIsDraw, pAb->AbElement->ElTypeNumber, pAb->AbElement->ElStructSchema))
    return 0;
  if (pCell && pCell->AbBox)
    {
      pFlow = GetRelativeFlow (pCell->AbBox, frame);
      if (pFlow)
        {
          xcell += pFlow->FlXStart;
          ycell += pFlow->FlYStart;
        }
    }

  /* get the middle of the current box */
  width /= 2;
  x += width;
  height /= 2;
  y += height;
  if (pCell &&
      (xRef >= xcell && xRef <= xcell + wcell &&
       yRef >= ycell && yRef <= ycell + hcell))
    *matchCell = pCell->AbElement;

  if (*matchCell)
    ratio = 10;
  value = GetDistance (xRef - x, width) + ratio * GetDistance (yRef - y, height);
  return (value);
}

/*----------------------------------------------------------------------
  GetParentWithException returns the abstract box which has the exceptNum
  exception or NULL..
  ----------------------------------------------------------------------*/
PtrAbstractBox GetParentWithException (int exceptNum, PtrAbstractBox pAb)
{
  while (pAb && pAb->AbElement)
    {
      if (TypeHasException (exceptNum,
                            pAb->AbElement->ElTypeNumber,
                            pAb->AbElement->ElStructSchema))
        return pAb;
      else
        pAb = pAb->AbEnclosing;
    }
  return NULL;
}

/*----------------------------------------------------------------------
  GetClickedBox look for the abstract box that overlaps the point x,y
  or the nearest abstract box.
  The function checks all boxes in the tree and returns the best choice.
  Between a box and its child the function choses the child.
  The parameter ration fixes penalities of the vertical proximity.
  ----------------------------------------------------------------------*/
void GetClickedBox (PtrBox *result, PtrFlow *pFlow, PtrAbstractBox pRootAb,
                    int frame, int x, int y, int ratio, int *pointselect)
{
  PtrElement          pEl;
  PtrAbstractBox      pAb, active, sel_active, marker, group;
  PtrBox              pSelBox, pBox;
  PtrBox              graphicBox;
  PtrElement          matchCell = NULL, prevMatch = NULL;
  ViewFrame          *pFrame;
  int                 dist;
  int                 pointIndex;
  int                 d, bx, by, bw;
  ThotBool            selshape = FALSE;

  pBox = NULL;
  pSelBox = NULL;
  *pFlow = NULL;
  sel_active = NULL;
  /* dist gives the previous distance of the selected box
     MAX_DISTANCE when no box is selected */
  dist = MAX_DISTANCE;
  pFrame = &ViewFrameTable[frame - 1];

  if (pFrame->FrAbstractBox != NULL)
    pBox = pRootAb->AbBox;
  
  if (pBox != NULL)
    {
      pBox = pBox->BxNext;
      while (pBox && pBox->BxAbstractBox && pBox->BxAbstractBox->AbElement)
        {
          pAb = pBox->BxAbstractBox;
          pEl = pAb->AbElement;
          marker = group = NULL;
          if (matchCell)
            // keep in memory the previous found cell
            prevMatch = matchCell;
          if (pBox->BxBoundinBoxComputed ||
              pBox->BxType == BoBlock || pBox->BxNChars == 0)
            {
              bx = pBox->BxClipX;
              by = pBox->BxClipY;
              bw = pBox->BxClipW;
              *pFlow = GetRelativeFlow (pBox, frame);
              if (*pFlow)
                {
                  /* apply the box shift */
                  bx += (*pFlow)->FlXStart;
                  by += (*pFlow)->FlYStart;
                }


              if (matchCell && !ElemIsAnAncestor (matchCell, pAb->AbElement))
                // the element is not within that cell
                ;
              else if (pAb->AbVisibility >= pFrame->FrVisibility)
                {
                  pointIndex = 0;
                  graphicBox = NULL;
                  if ((pAb->AbPresentationBox && !pAb->AbCanBeModified) ||
                      pAb->AbLeafType == LtGraphics ||
                      pAb->AbLeafType == LtPolyLine ||
                      pAb->AbLeafType == LtPath ||
                      // skip column heads
                      (pAb->AbEnclosing && pAb->AbEnclosing->AbBox &&
                       pAb->AbEnclosing->AbBox->BxType == BoColumn))
                    {
                      marker = GetParentMarker (pBox);
                      group = GetParentGroup (pBox);
                      if (pAb->AbLeafType == LtPath ||
                         (bx <= x && bx + pBox->BxClipW >= x &&
                          by <= y && by + pBox->BxClipH >= y))
                        {
                          if (pAb->AbLeafType == LtPicture &&
                              pEl && pEl->ElStructSchema &&
                              !strcmp (pEl->ElStructSchema->SsName, "Template"))
                            graphicBox = pBox;
                          else
                            graphicBox = GetEnclosingClickedBox (pAb, x, x, y, frame,
                                                                 &pointIndex, &selshape, pFlow);
                        }
                      if (graphicBox)
                        d = 0;
                      else if (pAb->AbLeafType == LtGraphics && pEl &&
                               (FrameTable[frame].FrView != 1 ||
                                !TypeHasException (ExcIsCell, pEl->ElTypeNumber, pEl->ElStructSchema) ))
                        d = GetBoxDistance (pBox, *pFlow, x, y, ratio, frame, &matchCell) + 2;
                      else
                        /* eliminate this box */
                        d = dist + 1;
                    }
                  else if (pAb->AbLeafType == LtSymbol && pAb->AbShape == 'r')
                    /* glitch for the root symbol */
                    d = GetShapeDistance (x, y, pBox, 1, frame);
                  else if (pAb->AbLeafType == LtText ||
                           pAb->AbLeafType == LtSymbol ||
                           pAb->AbLeafType == LtPicture ||
                           /* or an empty compound box */
                           (pAb->AbLeafType == LtCompound && pAb->AbVolume == 0))
                    {
                      if (pAb->AbLeafType == LtPicture)
                        {
                          /* check if the right side of the picture is selected */
                          d = bx + (bw / 2);
                          if (x > d)
                            pointIndex = 1;
                        }
                      d = GetBoxDistance (pBox, *pFlow, x, y, ratio, frame, &matchCell);
                      if (d > dist && dist == MAX_DISTANCE)
                        /* it's the first box selected */
                        dist = d;
                    }
                  else
                    d = dist + 1;
                  /* select the nearest box */
                  active = GetParentWithException (ExcClickableSurface, pAb);
                  if (active)
                    {
                    if (active->AbBox == NULL)
                      active = NULL;
                    else if ((active->AbBox->BxType == BoGhost ||
                              active->AbBox->BxType == BoStructGhost) &&
                             d != 0)
                      active = NULL;	    
                    else if (active->AbBox->BxType != BoGhost &&
                             active->AbBox->BxType != BoStructGhost &&
                             GetBoxDistance (active->AbBox, *pFlow, x, y, ratio, frame, &matchCell) != 0)
                      active = NULL;
                    }
                  if (active && sel_active == NULL && dist != 0)
                    dist = d + 1;

                  if (prevMatch != matchCell && matchCell)
                    // ignore previous boxes out of the current cell
                    dist = MAX_DISTANCE;
                  if (d < dist ||
                      (d == dist &&
                       (pSelBox == NULL ||
                        pSelBox->BxAbstractBox->AbDepth >= pAb->AbDepth)))
                    {
                      dist = d;
                      if (marker && marker->AbEnclosing && marker->AbEnclosing->AbBox)
                        pSelBox = marker->AbEnclosing->AbBox;
                      else
                        pSelBox = pBox;
                      sel_active = active;
                      /* the selected reference point */
                      *pointselect = pointIndex;
                    }
                }
            }
          pBox = pBox->BxNext;
        }
      /* return the root box if there is no box selected */
      if (pSelBox == NULL)
        pSelBox = pBox = pFrame->FrAbstractBox->AbBox;
    }
  *result = pSelBox;
}
