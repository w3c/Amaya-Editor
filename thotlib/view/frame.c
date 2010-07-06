/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2009
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 
/*
 * frame.c : incremental display in frames.
 *
 * Authors: I. Vatton (INRIA)
 *          R. Guetari (W3C/INRIA) - Unicode, Windows version and Plug-ins
 *
 */
#include "thot_gui.h"
#include "ustring.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "picture.h"
#include "libmsg.h"
#include "message.h"
#include "appdialogue.h"
#ifdef _WINGUI
#include "wininclude.h"
#endif /* _WINGUI */

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "boxes_tv.h"
#include "edit_tv.h"
#include "platform_tv.h"
#include "frame_tv.h"
#include "picture_tv.h"

#include "absboxes_f.h"
#include "appli_f.h"
#include "boxlocate_f.h"
#include "boxmoves_f.h"
#include "boxrelations_f.h"
#include "boxpositions_f.h"
#include "buildboxes_f.h"
#include "displaybox_f.h"
#include "displayselect_f.h"
#include "exceptions_f.h"
#include "font_f.h"
#include "frame_f.h" 
#include "picture_f.h"
#include "windowdisplay_f.h"
#include "buildlines_f.h"
#include "memory_f.h"
#include "selectionapi_f.h"
#include "presentation.h"
#include "registry.h"

#ifdef _GL
#include "openglfont.h"
#if defined (_MACOS) && defined (_WX)
#include <gl.h>
#include <glu.h>
#else /* _MACOS */
#include <GL/gl.h>
#include <GL/glu.h>
#endif /* _MACOS */
#include <math.h>
#include "glwindowdisplay.h"

static int    Matrix_counter = 0;
/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
static ThotBool IfPushMatrix (PtrAbstractBox pAb)
{
  if (!pAb->AbPresentationBox &&
      (pAb->AbElement->ElSystemOrigin || pAb->AbElement->ElTransform))
    {
      Matrix_counter++;
      glPushMatrix ();
      return TRUE;
    }
  else
    return FALSE;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
static ThotBool IfPopMatrix (PtrAbstractBox pAb)
{
  if (!pAb->AbPresentationBox &&
      (pAb->AbElement->ElSystemOrigin || pAb->AbElement->ElTransform))
    {
      if (Matrix_counter > 0)
        {
          Matrix_counter--;
          glPopMatrix ();
        }
#ifdef _GL_DEBUG
      else
        printf ("Error PopMatrix (%d)\n",Matrix_counter);
#endif /* _GL_DEBUG */
      return TRUE;
    }
  else
    return FALSE;
}
#endif /*_GL*/


/*----------------------------------------------------------------------
  GetXYOrg : do a coordinate shift related to current frame.
  ----------------------------------------------------------------------*/
void GetXYOrg (int frame, int *XOrg, int *YOrg)
{
  ViewFrame          *pFrame;

  pFrame = &ViewFrameTable[frame - 1];
  *XOrg = pFrame->FrXOrg;
  *YOrg = pFrame->FrYOrg;
}

/*----------------------------------------------------------------------
  DefClip defines the area of the frame which need to be redrawn.
  ----------------------------------------------------------------------*/
void DefClip (int frame, int xstart, int ytop, int xstop, int ybottom)
{
  ViewFrame          *pFrame;
  int                 width, height;
  int                 xb, xe, yb, ye;
  int                 scrollx, scrolly;

  /* values can be negative when a scroll back is requested */
  if ((xstart == xstop && xstart == 0 && (ytop != ybottom || ytop != 0)) ||
      (ytop == ybottom && ytop == 0 && (xstart != xstop || xstart != 0)))
    return;
  else if (frame == 0 || frame > MAX_FRAME)
    return;
  else
    {
      /* exchange limits for rigth-to-left string */
      if (xstop < xstart)
        {
          xe = xstart;
          xstart = xstop;
          xstop = xe;
        }
      if (ybottom < ytop)
        {
          ye = ytop;
          ytop = ybottom;
          ybottom = ye;
        }
    }

  pFrame = &ViewFrameTable[frame - 1];
  scrollx = pFrame->FrXOrg;	
  scrolly = pFrame->FrYOrg; 
  xb = pFrame->FrClipXBegin - scrollx;
  xe = pFrame->FrClipXEnd - scrollx - xb; 
  yb = pFrame->FrClipYBegin - scrolly;
  ye = pFrame->FrClipYEnd - scrolly - yb;	
  if (xstart == xstop && xstart == -1)
    {
      /* repaint the whole frame */
      GetSizesFrame (frame, &width, &height);
      pFrame->FrClipXBegin = scrollx;
      pFrame->FrClipXEnd = width + scrollx;
      pFrame->FrClipYBegin = scrolly;
      pFrame->FrClipYEnd = height + scrolly;
    }
  else if (xstart == xstop && xstart == 0 && ytop == ybottom && ytop == 0)
    {
      /* clean up */
      pFrame->FrClipXBegin = 0;
      pFrame->FrClipXEnd = 0;
      pFrame->FrClipYBegin = 0;
      pFrame->FrClipYEnd = 0;
    }
  else if (pFrame->FrClipXBegin == pFrame->FrClipXEnd &&
           pFrame->FrClipXBegin == 0 &&
           pFrame->FrClipYBegin == pFrame->FrClipYEnd &&
           pFrame->FrClipYBegin == 0)
    {
      /* clean up */
      pFrame->FrClipXBegin = xstart;
      pFrame->FrClipXEnd = xstop;
      pFrame->FrClipYBegin = ytop;
      pFrame->FrClipYEnd = ybottom;
    }
  else
    {
      /* Update the coordinates of the area redrawn */
      if (pFrame->FrClipXBegin > xstart)
        pFrame->FrClipXBegin = xstart;
      if (pFrame->FrClipXEnd < xstop)
        pFrame->FrClipXEnd = xstop;
      /* update the coordinates of the area redrawn */
      if (pFrame->FrClipYBegin > ytop)
        pFrame->FrClipYBegin = ytop;
      if (pFrame->FrClipYEnd < ybottom)
        pFrame->FrClipYEnd = ybottom;
    }
}

/*----------------------------------------------------------------------
  DefBoxRegion stores the area of a box which needs to be redrawn.
  When parameters xstart and xstop are equal to -1 the whole width box
  is clipped else only the xstart to xstop region is clipped.
  When parameters ystart and ystop are equal to -1 the whole height box
  is clipped else only the ystart to ystop region is clipped.
  ----------------------------------------------------------------------*/
void DefBoxRegion (int frame, PtrBox pBox, int xstart, int xstop,
                   int ystart, int ystop)
{
  PtrFlow             pFlow;
  PtrAbstractBox      pAb, first, last, pParent;
#ifdef _GL
  PtrAbstractBox      pClipAb;
  ThotBool            formatted;
#endif /* _GL */
  PtrElement          pEl = NULL;
  int                 x1, x2, y1, y2, k;

  k = 0;
  if (pBox &&
      (pBox->BxType == BoGhost ||
       pBox->BxType == BoStructGhost ||
       pBox->BxType == BoFloatGhost))
    {
      pAb = pBox->BxAbstractBox;
      /* get the first and last enclosed boxes */
      first = pAb->AbFirstEnclosed;
      while (first && first->AbBox == NULL)
        first = first->AbNext;
      if (first == NULL)
        return;
      last = first;
      while (last->AbNext && last->AbNext->AbBox)
        last = last->AbNext;
      
      while (first->AbBox &&
             (first->AbBox->BxType == BoGhost || first->AbBox->BxType == BoStructGhost) &&
             first->AbFirstEnclosed)
        first = first->AbFirstEnclosed;
      while (last->AbBox &&
             (last->AbBox->BxType == BoGhost || last->AbBox->BxType == BoStructGhost) &&
             last->AbFirstEnclosed)
        {
          last = last->AbFirstEnclosed;
          while (last->AbNext && last->AbNext->AbBox)
            last = last->AbNext;
        }

      // get horizontal limits
      pParent = pAb->AbEnclosing;
      while (pParent->AbBox &&
             (pParent->AbBox->BxType == BoGhost || pParent->AbBox->BxType == BoStructGhost))
        pParent = pParent->AbEnclosing;
      x1 = pParent->AbBox->BxXOrg + pParent->AbBox->BxLMargin
        + pParent->AbBox->BxLBorder + pParent->AbBox->BxLPadding;
      x2 = x1 + pParent->AbBox->BxW;
      pFlow = GetRelativeFlow (pParent->AbBox, frame);
      if (pFlow)
        {
          x1 += pFlow->FlXStart;
          x2 += pFlow->FlXStart;
        }
      // get the right leaf box
      pBox = first->AbBox;
      if (pBox->BxType == BoSplit || pBox->BxType == BoMulScript)
        pBox = pBox->BxNexChild;
      y1 = pBox->BxYOrg;
      y2 = pBox->BxYOrg + pBox->BxHeight;
      pBox = last->AbBox;
      if (pBox->BxType == BoSplit || pBox->BxType == BoMulScript)
        {
          pBox = pBox->BxNexChild;
          while (pBox->BxNexChild && pBox->BxNexChild->BxW > 0)
            pBox = pBox->BxNexChild;
        }
      if (pBox->BxYOrg + pBox->BxHeight > y2)
        y2 = pBox->BxYOrg + pBox->BxHeight;
      DefClip (frame, x1, y1, x2, y2);
    }
  else
    {
      pAb = pBox->BxAbstractBox;
      if (pBox->BxType == BoCell)
        {
          // look for the cell frame
          while (pAb->AbLeafType != LtGraphics &&
                 pAb->AbPrevious &&
                 pAb->AbPrevious->AbPresentationBox)
            pAb = pAb->AbPrevious;
          if (pAb->AbLeafType == LtGraphics && pAb->AbBox)
            pBox = pAb->AbBox;
        }
#ifdef _GL
      formatted = (FrameTable[frame].FrView == 1 && pAb->AbPSchema &&
                   pAb->AbPSchema->PsStructName &&
                   strcmp (pAb->AbPSchema->PsStructName, "TextFile"));
      if (formatted)
        {
          /* clip on the enclosing box that changes the System origin */
          pClipAb = pAb;
          /* the box could be included within a SVG element */
          while (pAb)
            {
              if (pAb->AbElement &&
                  pAb->AbBox &&
                  pAb->AbElement->ElSystemOrigin)
                pClipAb = pAb;
              pAb = pAb->AbEnclosing;
            }
          if (pClipAb != pBox->BxAbstractBox && pClipAb->AbBox)
            {
              /* clip the enclosing limits */
              xstart = xstop = ystart = ystop = -1;
              pBox = pClipAb->AbBox;
              k = EXTRA_GRAPH;
            }
          else
            pAb = pClipAb;
        }
#endif /* _GL */
      if (pAb)
        pEl = pAb->AbElement;
      /* take into account the positioning */
      pFlow = GetRelativeFlow (pBox, frame);
      if (pEl && pBox->BxAbstractBox &&
          (pBox->BxAbstractBox->AbLeafType == LtGraphics ||
           pBox->BxAbstractBox->AbLeafType == LtPolyLine ||
           pBox->BxAbstractBox->AbLeafType == LtPath ||
           (pEl &&
            (TypeHasException (ExcIsImg, pEl->ElTypeNumber, pEl->ElStructSchema) ||
             TypeHasException (ExcIsTable, pEl->ElTypeNumber, pEl->ElStructSchema) ||
             TypeHasException (ExcIsDraw, pEl->ElTypeNumber, pEl->ElStructSchema)))))
        k = EXTRA_GRAPH;

      x1 = pBox->BxXOrg;
      x2 = x1;
      if ((xstart == -1 && xstop == -1) || pBox->BxAbstractBox->AbPresentationBox)
        {
          /* take into account the negative left margin */
          if (pBox->BxLMargin < 0)
            x1 += pBox->BxLMargin;
          x2 += pBox->BxWidth;
          if (pBox->BxAbstractBox && pBox->BxAbstractBox->AbLeafType == LtText)
            x2 += 2;
        }
      else
        {
          x1 += xstart;
          if (pBox->BxLMargin < 0)
            x1 += pBox->BxLMargin;
          x2 += xstop;
        }
      if (pFlow)
        {
          x1 += pFlow->FlXStart;
          x2 += pFlow->FlXStart;
        }
      y1 = pBox->BxYOrg;
      y2 = y1;
      if ((ystart == -1 && ystop == -1) || pBox->BxAbstractBox->AbPresentationBox)
        {
          if (pBox->BxTMargin < 0)
            y1 += pBox->BxTMargin;
          else
            y1 -= 1; // due to dashed borders
          y2 += pBox->BxHeight;
        }
      else
        {
          y1 += ystart;
          if (pBox->BxTMargin < 0)
            y1 += pBox->BxTMargin;
          y2 += ystop;
        }
      if (pFlow)
        {
          y1 += pFlow->FlYStart;
          y2 += pFlow->FlYStart;
        }
#ifdef CLIP_TRACE
      printf ("ClipBoxRegion x1=%d y1=%d x2=%d y2=%d\n", x1, y1, x2, y2);
#endif /* CLIP_TRACE */
      if (x1 - k != x2 + k && y1 - k != y2 + k)
        {
          /* origins must be positive */
          if (x1 < k)
            x1 = k;
          if (y1 < k)
            y1 = k;
          DefClip (frame, x1 - k, y1 - k, x2 + k*2, y2 + k*2);
        }
    }
}

/*----------------------------------------------------------------------
  UpdateBoxRegion stores the area of a box which needs to be redrawn
  before and after the change.
  The parameters dx gives the horizontal shift (before the change is done).
  The parameters dy gives the vertical shift (before the change is done).
  The parameters dw gives the width delta (before the change is done).
  The parameters dh gives the height delta (before the change is done).
  ----------------------------------------------------------------------*/
void UpdateBoxRegion (int frame, PtrBox pBox, int dx, int dy, int dw, int dh)
{
  ViewFrame          *pFrame;
  PtrFlow             pFlow;
  PtrAbstractBox      pAb;
#ifdef _GL
  PtrAbstractBox      pClipAb;
  ThotBool            formatted;
#endif /* _GL */
  PtrElement          pEl = NULL;
  int                 x1, x2, y1, y2, cpoints, caret;

  cpoints = 0;
  if (pBox && pBox->BxAbstractBox)
    {
      pAb = pBox->BxAbstractBox;
#ifdef _GL
      formatted = (FrameTable[frame].FrView == 1 && pAb->AbPSchema &&
                   pAb->AbPSchema->PsStructName &&
                   strcmp (pAb->AbPSchema->PsStructName, "TextFile"));
      if (formatted)
        {
          /* clip on the enclosing box that changes the System origin */
          pClipAb = pAb;
          while (pAb)
            {
              if (pAb->AbElement &&
                  pAb->AbBox &&
                  pAb->AbElement->ElSystemOrigin)
                pClipAb = pAb;
              pAb = pAb->AbEnclosing;
            }
          if (pClipAb != pBox->BxAbstractBox && pClipAb->AbBox)
            {
              /* clip the enclosing limits */
              dx = dy = dw = dh = 0;
              pBox = pClipAb->AbBox;
              cpoints = EXTRA_GRAPH;
            }
        }
#endif /* _GL */

      if (pAb)
        pEl = pAb->AbElement;
      pFrame = &ViewFrameTable[frame - 1];
      /* take into account the positioning */
      pFlow = GetRelativeFlow (pBox, frame);
      if (pBox->BxAbstractBox &&
          (pBox->BxAbstractBox->AbLeafType == LtGraphics ||
           pBox->BxAbstractBox->AbLeafType == LtPolyLine ||
           pBox->BxAbstractBox->AbLeafType == LtPath ||
           (pEl &&
            (TypeHasException (ExcIsImg, pEl->ElTypeNumber, pEl->ElStructSchema) ||
             TypeHasException (ExcIsTable, pEl->ElTypeNumber, pEl->ElStructSchema)))))
        /* increase the redisplay area due to control points */
        cpoints = EXTRA_GRAPH;

      if (pBox->BxAbstractBox &&
          (pBox->BxAbstractBox->AbLeafType == LtText ||
           pBox->BxAbstractBox->AbLeafType == LtSymbol))
        /* let Amaya displays the caret at the end of the text */
        caret = 2;
      else
        caret = 0;
      x1 = pBox->BxXOrg;
      if (pFlow)
        x1 += pFlow->FlXStart;
      x2 = x1 + pBox->BxWidth;
      /* take into account the negative left margin */
      if (pBox->BxLMargin < 0)
        x1 += pBox->BxLMargin;
      y1 = pBox->BxYOrg;
      if (pFlow)
        y1 += pFlow->FlYStart;
      if (pBox->BxTMargin < 0)
        y1 += pBox->BxTMargin;
      y2 = y1 + pBox->BxHeight;
      if (dx >= 0)
        x2 += dx; /* the box will be moved to the right */
      else
        x1 += dx; /* the box will be moved to the left */
      if (dy >= 0)
        y2 += dy; /* the box will be moved to the bottom */
      else
        y1 += dy; /* the box will be moved to the top */
      if (dw > 0)
        x2 += dw;
      if (dh > 0)
        y2 += dh;
#ifdef CLIP_TRACE
      printf ("UpdateBoxRegion dx=%d dy=%d dw=%d dh=%d\n", dx, dy, dw, dh);
      printf ("                x1=%d y1=%d x2=%d y2=%d\n", x1, y1, x2, y2);
#endif /* CLIP_TRACE */
      /* origins must be positive */
      if (x1 < cpoints)
        x1 = cpoints;
      if (y1 < cpoints)
        y1 = cpoints;
      DefClip (frame, x1 - cpoints, y1 - cpoints,
               x2 + cpoints + caret, y2 + cpoints);
    }
}

/*----------------------------------------------------------------------
  TtaRefresh redraw all the frame of all the loaded documents.
  ----------------------------------------------------------------------*/
void TtaRefresh ()
{
  int        frame;

  for (frame = 1; frame <= MAX_FRAME; frame++)
    {
      if (ViewFrameTable[frame - 1].FrAbstractBox != NULL)
        {
          /* enforce redrawing of the whole frame */
          DefClip (frame, -1, -1, -1, -1);
          (void) RedrawFrameBottom (frame, 0, NULL);
        }
    }
}

/*----------------------------------------------------------------------
  DrawFilledBox draws a box with background or borders.
  The parameter pForm points the box that generates the border or fill.
  Clipping is done by xmin, xmax, ymin, ymax.
  pFlow points to the displayed flow or NULL when it's the main flow.
  Parameters first and last are TRUE when the box pBox is respectively
  at the first position and/or the last position of pFrom (they must be
  TRUE for pFrom itself).
  selected is TRUE when a parent box or the box itself is selected.
  show_bgimage is TRUE if bg_images can be displayed.
  ----------------------------------------------------------------------*/
void DrawFilledBox (PtrBox pBox, PtrAbstractBox pFrom, int frame, PtrFlow pFlow,
                    int xmin, int xmax, int ymin, int ymax, ThotBool selected,
                    ThotBool first, ThotBool last, ThotBool show_bgimage)
{
  PtrBox              from, box;
  PtrAbstractBox      pChild, pAb, pParent, pNext;
  PtrDocument         pDoc;
  PtrElement          pEl = NULL;
  ViewFrame          *pFrame;
  ThotPictInfo       *imageDesc;
  PictureScaling      pres;
  PtrLine             pLine;
  int                 x, y, xd = 0, yd = 0;
  int                 xbg, ybg, shiftx, shifty;
  int                 width = 0, height = 0;
  int                 wbg, hbg;
  int                 w, h, view, delta;
  int                 t, b, l, r, bt, bb, bl, br;
#ifdef _GL
  int                 tex_bg_id = 0;
#endif /* _GL */
  ThotBool            setWindow, isLast;

  if (pBox == NULL || pFrom == NULL || pFrom->AbBox == NULL)
    return;
  else if (pFrom->AbElement == NULL ||
           (FrameTable[frame].FrView == 1 &&
            TypeHasException (ExcNoShowBox,
                              pFrom->AbElement->ElTypeNumber,
                              pFrom->AbElement->ElStructSchema)))
    return;
  
  pFrame = &ViewFrameTable[frame - 1];
  x = pFrame->FrXOrg;
  y = pFrame->FrYOrg;
  GetSizesFrame (frame, &w, &h);
  xd = yd = width = height = t = b = l = r = 0;
  bl = bt = br = bb = 0;
  from = pFrom->AbBox;
  pAb = pBox->BxAbstractBox;
  if (pAb)
    pEl = pAb->AbElement;
  if (pBox->BxType == BoGhost ||
      pBox->BxType == BoStructGhost ||
      pBox->BxType == BoFloatGhost)
    {
      /* check the block type to detect what border to apply */
      pParent = pAb->AbEnclosing;
      while (pParent && pParent->AbBox &&
             (pParent->AbBox->BxType == BoGhost || pParent->AbBox->BxType == BoStructGhost))
        pParent = pParent->AbEnclosing;
      if (pParent == NULL || pParent->AbBox == NULL)
        return;
      /* display all children */
      pChild = pAb->AbFirstEnclosed;
      while (pChild)
        {
          /* skip presentation boxes */
          pNext = pChild->AbNext;
          while (pNext && pNext->AbPresentationBox)
            pNext = pNext->AbNext;
          isLast = (last &&
                    (pNext == NULL || pNext->AbNotInLine || !pNext->AbHorizEnclosing));
          if (pChild->AbBox && !pChild->AbPresentationBox &&
              !TypeHasException (ExcIsBreak,
                                 pChild->AbElement->ElTypeNumber,
                                 pChild->AbElement->ElStructSchema))
            {
              /* draw each child boxes */
              DrawFilledBox (pChild->AbBox, pFrom, frame, pFlow,
                             xmin, xmax, ymin, ymax,
                             selected, first, isLast, show_bgimage);
              first = FALSE;
            }
          pChild = pNext;
        }
      return;
    }
  else if (pBox->BxType == BoSplit || pBox->BxType == BoMulScript)
    {
      pBox = pBox->BxNexChild;
      while (pBox)
        {
          isLast = (last && pBox->BxNexChild == NULL);
          DrawFilledBox (pBox, pFrom, frame, pFlow,
                         xmin, xmax, ymin, ymax,
                         selected, first, isLast, show_bgimage);
          pBox = pBox->BxNexChild;
          first = FALSE;
        }
      return;
    }

  /* add shift due to relative positioning */
  if (pFlow)
    {
      /* apply the box shift */
      shiftx = pFlow->FlXStart;
      shifty = pFlow->FlYStart;
    }
  else
    shiftx = shifty = 0;
#ifdef _GL
  pBox->BxClipX += shiftx;
  pBox->BxClipY += shifty;
#endif /*_GL*/
  pBox->BxXOrg += shiftx;
  pBox->BxYOrg += shifty;
  /* the default area to be painted with the background */
  if (pBox == from)
    {
      /* display borders and fill of the current box */
      l = pBox->BxLMargin;
      b = pBox->BxBMargin;
      t = pBox->BxTMargin;
      r = pBox->BxRMargin;
      bl = pBox->BxLBorder;
      bb = pBox->BxBBorder;
      bt = pBox->BxTBorder;
      br = pBox->BxRBorder;
      xd = pBox->BxXOrg + l;
      width = pBox->BxWidth;
      yd = pBox->BxYOrg + t;
      height = pBox->BxHeight;
    }
  else if ((from->BxType == BoGhost || from->BxType == BoStructGhost) &&
           pFrom->AbDisplay == 'B')
    {
      // check if borders are displayed
      if (first)
        bt = from->BxTBorder;
      else
        bt = 0;
      if (last)
        bb = from->BxBBorder;
      else
        bb = 0;
      bl = br = 0;
      pLine = SearchLine (pBox, frame);
      if (pLine)
        {
          // check the beginning and end of the current line
          if (pBox == pLine->LiFirstBox || pBox == pLine->LiFirstPiece)
            bl = from->BxLBorder;
          if (pBox == pLine->LiLastBox || pBox == pLine->LiLastPiece)
            br = from->BxRBorder;
          pParent = pFrom->AbEnclosing;
          while (pParent->AbBox &&
                 (pParent->AbBox->BxType == BoGhost ||
                  pParent->AbBox->BxType == BoStructGhost))
            pParent = pParent->AbEnclosing;
          xd = pParent->AbBox->BxXOrg + pLine->LiXOrg - bl + shiftx;
          width = pLine->LiXMax + bl;
          yd = pParent->AbBox->BxYOrg + pLine->LiYOrg;
          height = pLine->LiHeight;

          // check if it's the first line
          t = b = l = r = 0;
          if (pLine->LiPrevious)
            box = pLine->LiPrevious->LiLastBox;
          else
            box = 0;
          if (box == NULL || !IsParentBox (from, box))
            {
              yd += from->BxTMargin;
              height -= from->BxTMargin;
            }
          // check if it's the last line
          if (pLine->LiNext)
            box = pLine->LiNext->LiFirstBox;
          else
            box = 0;
          if (box == NULL || !IsParentBox (from, box))
            height -= from->BxBMargin;
        }
      else
        {
          xd = pBox->BxXOrg;
          width = pBox->BxWidth;
          yd = pBox->BxYOrg;
          height = pBox->BxHeight;
        }
    }
  else
    {
      // the box is displayed on a set of lines
      GetExtraMargins (from, frame, FALSE, &t, &b, &l, &r);
      t += from->BxTMargin;
      b += from->BxBMargin;
      bt = from->BxTBorder;
      bb = from->BxBBorder;
      if (first)
        {
          l += from->BxLMargin;
          bl = from->BxLBorder;
        }
      else
        bl = 0;
      if (last)
        {
          r += from->BxRMargin;
          br = from->BxRBorder;
        }
      else
        br = 0;
      xd = pBox->BxXOrg + shiftx;
      if (l > 0)
        xd += l;
      width =  pBox->BxWidth;
      yd = pBox->BxYOrg;
      if (t > 0)
        yd += t;
      height = pBox->BxHeight;
    }
  if (l > 0)
    width -= l;
  if (r > 0)
    width -= r;
  if (t > 0)
    height -= t;
  if (b > 0)
    height -= b;
  
  /* clipping on the origin */
  if (xd < x)
    {
      delta = x - xd;
      width = width - delta;
      xd = x;
    }
  if (yd < y)
    {
      delta = y - yd;
      height = height - delta;
      yd = y;
    }
  /* clipping on the width */
  if (xd + width > xmax)
    width = xmax - xd + 1;
  /* clipping on the height */
  if (yd + height > ymax)
    height = ymax - yd + 1;
  
  setWindow = (pAb == pFrame->FrAbstractBox);
  if (!setWindow &&
      TypeHasException (ExcSetWindowBackground, pAb->AbElement->ElTypeNumber,
                        pAb->AbElement->ElStructSchema))
    {
      /* paint the whole window background when the fill applies to the document */
      pDoc = LoadedDocument[FrameTable[frame].FrDoc - 1];
      view = pDoc->DocView[pAb->AbDocView - 1].DvPSchemaView;
      setWindow = (view == 1 &&
                   (pAb->AbEnclosing == NULL || /* document */
                    pAb->AbEnclosing->AbEnclosing == NULL || /* html */
                    (!pAb->AbEnclosing->AbFillBox &&
                     pAb->AbEnclosing->AbEnclosing->AbEnclosing == NULL)) /* body */);
    }

#ifdef _GL
  if (pFrom && pFrom->AbFillBox)
    tex_bg_id = SetTextureScale (IsBoxDeformed(pBox));
  GL_SetFillOpacity (pFrom->AbFillOpacity);
#endif /* _GL */
  if (setWindow)
    {
      /* get the maximum of the window size and the root box size */
      if (pBox->BxWidth > w)
        w = pBox->BxWidth;
      if (pBox->BxHeight > h)
        h = pBox->BxHeight;
      /* background area */
      wbg = w + 1;
      hbg = h + 1;
      xbg = xmin;
      ybg = ymin;
      if (pFrom->AbFillBox && !selected)
        /* draw the window background */
        DrawRectangle (frame, 0, 0, xbg - x, ybg - y, wbg, hbg,
                       pFrom->AbForeground, pFrom->AbBackground,
                       pFrom->AbFillPattern);
    }
  else
    {
      // prepare the area to display a background image
      xbg = xd + bl;
      ybg = yd + bt;
      wbg = width - bl - br;
      hbg = height - bt - bb;
    }
  if (setWindow ||
      (width && height &&
       yd + height >= ymin && yd <= ymax &&
       xd + width >= xmin && xd <= xmax))
    {
      imageDesc = (ThotPictInfo *) pFrom->AbPictBackground;
      if (pFrom->AbSelected)
        {
          if (FrameTable[frame].FrView == 1 && TtaIsSelectionUnique () &&
              !pAb->AbPresentationBox &&
              TypeHasException (ExcIsImg, pEl->ElTypeNumber, pEl->ElStructSchema))
            // display an IMG as a PICTURE element
            DisplayPointSelection (frame, pBox, 0, FALSE);
          else if (FrameTable[frame].FrView == 1 &&
                   TypeHasException (ExcIsDraw, pEl->ElTypeNumber, pEl->ElStructSchema))
            // display a SVG element
            DisplayPointSelection (frame, pBox, 0, TRUE);
#ifdef _GL
          else if (FrameTable[frame].FrView == 1 && TtaIsSelectionUnique () &&
                   pFrom->AbElement->ElSystemOrigin)
            DrawRectangle (frame, 0, 0, 0, 0, width, height, 0, BgSelColor, 2);
#endif /* _GL */  
          else
            /* draw the box selection */
            DrawRectangle (frame, 0, 0, xd - x, yd - y, width, height, 0, BgSelColor, 2);
        }
      else if (!selected && pFrom->AbElement &&
               pFrom->AbElement->ElStructSchema &&
               (from->BxType != BoTable ||
                strcmp (pFrom->AbElement->ElStructSchema->SsName, "HTML")))
        {
          /* don't fill the background when an enclosing box is selected */
          if (!setWindow && pFrom->AbFillBox && pFrom->AbFillPattern)
            {
              /* draw the box background */
              DrawRectangle (frame, 0, 0, xd - x, yd - y, width, height,
                             pFrom->AbForeground, pFrom->AbBackground,
                             pFrom->AbFillPattern);
            }
          if (imageDesc && show_bgimage && imageDesc->PicFileName &&
              imageDesc->PicFileName[0] != EOS)
            {
              pres = imageDesc->PicPresent;
              // check horizontal and vertical constraints
              if ((pBox->BxType != BoPiece ||
                   (pBox == pAb->AbBox->BxNexChild &&
                    (imageDesc->PicPosX < 10 || imageDesc->PicXUnit != UnPercent)) ||
                   (pBox->BxNexChild == NULL && imageDesc->PicPosX > 70 &&
                    imageDesc->PicXUnit == UnPercent)) &&
                  (pAb->AbLeafType != LtCompound ||
                   ((!pAb->AbTruncatedHead || imageDesc->PicPosY > 50) &&
                    (!pAb->AbTruncatedTail || imageDesc->PicPosY < 50))))
                /* draw the background image the default presentation is repeat */
                DrawPicture (pBox, imageDesc, frame, xbg - x, ybg - y,
                             wbg, hbg, t, l, (ThotBool)(pBox == from));
            }
        }
      if ((bt || bb || bl || br) &&
          pFrom->AbElement &&
          pFrom->AbElement->ElStructSchema &&
          (from->BxType != BoTable ||
           strcmp (pFrom->AbElement->ElStructSchema->SsName, "HTML")))
        {
          DisplayBorders (pBox, pFrom, frame, xd - x, yd - y, width, height,
                          t, b, l, r, bt, bb, bl, br);
        }
    }

  /* remove shift due to relative positioning */
#ifdef _GL
  if (tex_bg_id)
    StopTextureScale (tex_bg_id);
  tex_bg_id = 0;
  GL_SetFillOpacity (1000);
  pBox->BxClipX -= shiftx;
  pBox->BxClipY -= shifty;
#endif /*_GL*/
  pBox->BxXOrg -= shiftx;
  pBox->BxYOrg -= shifty;
}

/*----------------------------------------------------------------------
  OpacityAndTransformNext : Test before going on to a next Box
  ----------------------------------------------------------------------*/
static void OpacityAndTransformNext (PtrAbstractBox pAb, int plane, int frame,
                                     int xmin, int xmax, int ymin, int ymax,
                                     ThotBool activate_opacity)
{
#ifdef _GL
  if (!pAb->AbPresentationBox && pAb->AbElement &&
      pAb->AbDepth == plane && pAb->AbBox)
    {
      if (TypeHasException (ExcIsGroup, pAb->AbElement->ElTypeNumber,
                            pAb->AbElement->ElStructSchema) && 
          pAb->AbOpacity != 1000 && pAb->AbOpacity != 0 &&
          activate_opacity && xmax - xmin > 0 && ymax - ymin > 0)
        {
          if (!pAb->AbBox->VisibleModification && 
              pAb->AbBox->Post_computed_Pic)
            /* display the group image has it is */
            DisplayOpaqueGroup (pAb, frame, xmin, xmax, ymin, ymax, FALSE);
          else if (pAb->AbBox->Pre_computed_Pic)
            {
              OpaqueGroupTexturize (pAb, frame, xmin, xmax, ymin, ymax, FALSE);
              ClearOpaqueGroup (pAb, frame, xmin, xmax, ymin, ymax);
              DisplayOpaqueGroup (pAb, frame, xmin, xmax, ymin, ymax, TRUE);
              /* Unless we can know when a box gets its picture or 
                 when changes are efffective */
              OpaqueGroupTextureFree (pAb, frame);	
            }
        }
    }
#endif /* _GL */
}

#ifdef _GL
/*----------------------------------------------------------------------
  OriginSystemSet : push to current matrix stack
  ----------------------------------------------------------------------*/
static ThotBool OriginSystemSet (PtrAbstractBox pAb, ViewFrame *pFrame,
                                 PtrBox *systemOriginRoot,
                                 int *oldXOrg, int *oldYOrg,
                                 int *clipXOfFirstCoordSys, int *clipYOfFirstCoordSys)
{
  PtrBox              pBox;
  
  pBox = pAb->AbBox;
  if (!pAb->AbPresentationBox && pAb->AbBox &&
      /* skip boxes already managed */
      pAb->AbElement->ElSystemOrigin && !IsParentBox (*systemOriginRoot, pBox))
    {
      *systemOriginRoot = pBox;
      if (pFrame->FrXOrg || pFrame->FrYOrg)
        {
          *oldXOrg = pFrame->FrXOrg;
          *oldYOrg = pFrame->FrYOrg;
          pFrame->FrXOrg = 0;
          pFrame->FrYOrg = 0;
          *clipXOfFirstCoordSys = pBox->BxClipX;
          *clipYOfFirstCoordSys = pBox->BxClipY;
          return TRUE; // changed
        }
    }
  return FALSE; // no change
}

/*----------------------------------------------------------------------
  OriginSystemExit : pop from current matrix stack
  ----------------------------------------------------------------------*/
static ThotBool OriginSystemExit (PtrAbstractBox pAb, ViewFrame  *pFrame, 
				  PtrBox *systemOriginRoot,
                                  int *oldXOrg, int *oldYOrg,
                                  int *clipXOfFirstCoordSys, int *clipYOfFirstCoordSys)
{
  PtrBox              pBox;
  
  pBox = pAb->AbBox;
  if (!pAb->AbPresentationBox && pAb->AbBox && pAb->AbElement->ElSystemOrigin)
    {
      if (*clipXOfFirstCoordSys == pBox->BxClipX &&
          *clipYOfFirstCoordSys == pBox->BxClipY)
        {
          pFrame->FrXOrg = *oldXOrg;
          pFrame->FrYOrg = *oldYOrg;
	  *systemOriginRoot = NULL;
	  *clipXOfFirstCoordSys = 0;
	  *clipYOfFirstCoordSys = 0;
          pFrame->OldFrXOrg = 0;
          pFrame->OldFrYOrg = 0;
          return TRUE; // changed
        }
    }
  return FALSE; // no change
}
#endif /* _GL */

/*----------------------------------------------------------------------
  GetBoxTransformedCoord : Transform windows coordinate x, y
  to the transformed system  of the seeked box
  ----------------------------------------------------------------------*/
void GetBoxTransformedCoord (PtrAbstractBox pAbSeeked, int frame,
                             int *lowerx, int *higherx, int *x, int *y)
{
#ifdef _GL
  PtrAbstractBox      pAb, root, pNext;
  PtrBox              pBox;
  ViewFrame          *pFrame;
  PtrBox              systemOriginRoot = NULL;
  int                 l, h;
  int                 oldXOrg, oldYOrg;
  int                 clipXOfFirstCoordSys, clipYOfFirstCoordSys;
  double              winx, winy, finalx, finaly, finalz;
  int                 viewport[4];
  double              projection_view[16];
  double              model_view[16];
  int                 base_y;
  ThotBool            is_transformed = FALSE;
  ThotBool            updateStatus, formatted;

  updateStatus = FrameUpdating;
  FrameUpdating = TRUE;  
  pFrame = &ViewFrameTable[frame - 1];
  // @@@@ TODO: check positioned boxes
  pAb = pFrame->FrAbstractBox;
  root = pAb;
  GetSizesFrame (frame, &l, &h);
  pBox = pAb->AbBox;
  if (pBox == NULL)
    return;

  pAb = pFrame->FrAbstractBox;
  formatted = (FrameTable[frame].FrView == 1 && pAb->AbPSchema &&
               pAb->AbPSchema->PsStructName &&
               strcmp (pAb->AbPSchema->PsStructName, "TextFile"));
  if (!formatted)
    {
      *lowerx += pFrame->FrXOrg;
      *higherx += pFrame->FrXOrg;
      *y += pFrame->FrYOrg;
      return;
    }

  oldXOrg = 0;
  oldYOrg = 0;
  clipXOfFirstCoordSys = clipYOfFirstCoordSys = 0;
  glGetDoublev (GL_MODELVIEW_MATRIX, model_view);
  glGetDoublev (GL_PROJECTION_MATRIX, projection_view);
  /* Compute transformations */
  pAb = pFrame->FrAbstractBox;
  while (pAb)
    {
      if (pAb != pFrame->FrAbstractBox && pAb->AbElement && pAb->AbBox)
        {
          pBox = pAb->AbBox;
          if (IfPushMatrix (pAb))
            {
              /* If the coord sys origin is translated, 
                 it must be before any other transfromation*/
              if (pAb->AbElement->ElSystemOrigin)
                {
                  is_transformed = TRUE;
                  DisplayBoxTransformation (pAb->AbElement->ElTransform, 
                                            pFrame->FrXOrg, pFrame->FrYOrg);
                }

              /* Normal transformation*/
              if (pAb->AbElement->ElTransform)
                {
                  is_transformed = TRUE;
                  DisplayTransformation (frame, 
                                         pAb->AbElement->ElTransform, 
                                         pBox->BxWidth, 
                                         pBox->BxHeight);
                }
              OriginSystemSet (pAb, pFrame, &systemOriginRoot,
                               &oldXOrg, &oldYOrg,
                               &clipXOfFirstCoordSys, &clipYOfFirstCoordSys);
            }

          if (pAb == pAbSeeked && pAb->AbLeafType != LtCompound &&
              is_transformed) 
            {  
              glGetDoublev (GL_MODELVIEW_MATRIX, model_view);
              glGetDoublev (GL_PROJECTION_MATRIX, projection_view);
            }
        }

      /* get next abstract box */
      if (pAb->AbLeafType == LtCompound &&  pAb->AbFirstEnclosed &&
          (pAb == root || !IsFlow (pAb->AbBox, frame)))
        /* get the first child */
        pAb = pAb->AbFirstEnclosed;
      else
        {
          // go next or up
          pNext = pAb->AbNext;
          // --------------------------------------------------------------
          do
            {
              if (pAb != pFrame->FrAbstractBox && pAb->AbElement && pAb->AbBox &&
                  IfPopMatrix (pAb))
                if (pAb->AbBox == systemOriginRoot)
		  OriginSystemExit (pAb, pFrame, &systemOriginRoot,
				    &oldXOrg, &oldYOrg, 
				    &clipXOfFirstCoordSys, &clipYOfFirstCoordSys);
              if (pAb == root)
                /* all boxes are now managed: stop the loop */
                pAb = pNext = NULL;
              else if (pNext)
                break;
              else
                {
                  /* go up in the tree */
                  pAb = pAb->AbEnclosing;
                  pNext = pAb->AbNext;
                }
            }
          while (pAb);
          pAb = pNext;
          // --------------------------------------------------------------
        }
    }

  FrameUpdating = updateStatus;
  if (is_transformed)
    {
      viewport[0] = 0; viewport[1] = 0; viewport[2] = l; viewport[3] = h;
      winx = (double) *lowerx;
      winy = (double) (h - *y);
      
      if (GL_TRUE == gluUnProject (winx, winy, 0.0, model_view, projection_view,
                                   (GLint*) viewport,
                                   &finalx, &finaly, &finalz))
        {
          base_y = FloatToInt ((float) finaly);
          if (*lowerx != *higherx)
            {
              *lowerx = FloatToInt ((float) finalx);
              winx = (double) *higherx;
              winy = (double) (h - *y);
              gluUnProject (winx, winy, 0.0, model_view, projection_view,
                            (GLint*) viewport,
                            &finalx, &finaly, &finalz);

              *higherx = FloatToInt ((float) finalx);
              base_y = FloatToInt ((float) finaly);
            }
          else
            {
              *higherx = *lowerx = FloatToInt ((float) finalx);
            }
          *y = base_y;
        }
    }
  else
    {
      *lowerx += pFrame->FrXOrg;
      *higherx += pFrame->FrXOrg;
      *y += pFrame->FrYOrg;
    }
  /*Make sure the transformation stack is empty*/
  glLoadIdentity ();
#endif /* _GL */
}

#ifdef _GL
/*----------------------------------------------------------------------
  SyncBoundingboxesReal : sync Bounding box of a group according to openGL 
  Computed values
  ----------------------------------------------------------------------*/
static void SyncBoundingboxesReal (PtrAbstractBox pInitAb, 
                                   int XFrame, int YFrame, int frame)
{
  PtrAbstractBox      pAb;  
  PtrBox              box;
  int                 x, y;

  box = pInitAb->AbBox;
  if (box == NULL)
    return;
  x = box->BxClipX - XFrame;
  y = box->BxClipY - YFrame;
  pAb = pInitAb->AbFirstEnclosed;
  while (pAb != NULL)
    {
      if (pAb->AbLeafType != LtCompound)
        {
          box = pAb->AbBox;
          if (box->BxType == BoPiece ||
              box->BxType == BoScript ||
              box->BxType == BoMulScript ||
              box->BxType == BoSplit)
            {
              while (box != NULL)
                {
                  box->BxClipX -= XFrame;
                  box->BxClipY -= YFrame;
                  box = box->BxNexChild;
                }
            }
        }
      else
        {
          if (pAb->AbElement->ElSystemOrigin && pAb->AbBox)
            {
              CoordinateSystemUpdate (pAb, frame, 
                                      pAb->AbBox->BxXOrg, 
                                      pAb->AbBox->BxYOrg);
              SyncBoundingboxesReal (pAb, XFrame, YFrame, frame);
            }
          else 
            {
              /* compute children of this box*/
              SyncBoundingboxesReal (pAb, XFrame, YFrame, frame);
            }
        }
      pAb = pAb->AbNext;
    }
  box = pInitAb->AbBox;
  box->BxClipX = x;
  box->BxClipY = y;
}

/*----------------------------------------------------------------------
  SyncBoundingboxes: synchronize Bounding box of a group
  ----------------------------------------------------------------------*/
static void SyncBoundingboxes (PtrAbstractBox pInitAb, 
                               int XFrame, int YFrame, int frame, 
                               int FrXOrg, int FrYOrg)
{
  PtrAbstractBox      pAb;  
  PtrBox              box;
  int                 x, y, w, h;
   
  box = pInitAb->AbBox;
  box->BxClipX = box->BxXOrg - FrXOrg;
  box->BxClipY = box->BxYOrg - FrYOrg;
  box->BxClipW = box->BxWidth; 
  box->BxClipH = box->BxHeight; 
  x = box->BxClipX;
  y = box->BxClipY;
  w = box->BxClipW; 
  h = box->BxClipH; 
  pAb = pInitAb->AbFirstEnclosed;
  while (pAb != NULL)
    {
      if (!pAb->AbDead && pAb->AbBox)
        {
          if (pAb->AbLeafType != LtCompound)
            {
              box = pAb->AbBox;
              box->BxClipX = box->BxXOrg - FrXOrg;
              box->BxClipY = box->BxYOrg - FrYOrg;
              box->BxClipW = box->BxWidth; 
              box->BxClipH = box->BxHeight; 
              box->BxBoundinBoxComputed = TRUE;
              if (box->BxType == BoPiece ||
                  box->BxType == BoScript ||
                  box->BxType == BoMulScript ||
                  box->BxType == BoSplit)
                {
                  box = box->BxNexChild;
                  while (box != NULL)
                    {
                      box->BxClipX = box->BxXOrg - FrXOrg;
                      box->BxClipY = box->BxYOrg - FrYOrg;
                      box->BxClipW = box->BxWidth; 
                      box->BxClipH = box->BxHeight; 
                      box->BxBoundinBoxComputed = TRUE;
                      box = box->BxNexChild;
                    }
                }
              box = pAb->AbBox;
            }
          else
            {
              if (pAb->AbElement->ElSystemOrigin && 
                  FrameTable[frame].FrView == 1)
                {
                  CoordinateSystemUpdate (pAb, frame, 
                                          pAb->AbBox->BxXOrg, 
                                          pAb->AbBox->BxYOrg);
                  SyncBoundingboxesReal (pAb, XFrame, YFrame, frame);
                }
              else
                /* compute children of this box */
                SyncBoundingboxes (pAb, XFrame, YFrame, frame, FrXOrg, FrYOrg);
            }
        }
      pAb = pAb->AbNext;
    }
}

/*----------------------------------------------------------------------
  ComputeBoundingBoxes: Compute clipping coordinate for each box.
  The parameter pInitAb gives the current root abstract box.
  ----------------------------------------------------------------------*/
static void ComputeBoundingBoxes (int frame, int xmin, int xmax, int ymin, int ymax,
                                  PtrAbstractBox pInitAb, ThotBool show_bgimage)
{
  PtrAbstractBox      pAb, specAb, child;
  PtrBox              pBox, box;
  PtrBox              topBox;
  ViewFrame          *pFrame;
  int                 winTop, winBottom;
  int                 bt, bb, l, h;
  int                 clipx, clipy, clipw, cliph; 
  ThotBool            userSpec = FALSE;
  ThotBool            formatted;

  pFrame = &ViewFrameTable[frame - 1];
  GetSizesFrame (frame, &l, &h);
  if (xmax == 0 && ymax == 0)
    {
      xmax = l; 
      ymax = h;
    }
  winTop = pFrame->FrYOrg;
  winBottom = winTop + h;
  pBox = pInitAb->AbBox;
  if (pBox == NULL)
    return;

  topBox = NULL;
  clipx = -1;
  clipy = -1;
  clipw = 0;
  cliph = 0;
  formatted = (FrameTable[frame].FrView == 1 &&
               pInitAb &&
               pInitAb->AbPSchema &&
               pInitAb->AbPSchema->PsStructName &&
               (strcmp (pInitAb->AbPSchema->PsStructName, "TextFile") != 0));
 pAb = pInitAb->AbFirstEnclosed;
 while (pAb)
   {
     if (pAb->AbBox && pAb != pInitAb)
       {
         pBox = pAb->AbBox;
         if (pAb->AbElement && formatted && IfPushMatrix (pAb))
           {
             if (pAb->AbElement->ElSystemOrigin)
               {
                 DisplayBoxTransformation (pAb->AbElement->ElTransform, 
                                           pFrame->FrXOrg, 
                                           pFrame->FrYOrg);
               }
             if (pAb->AbElement->ElTransform)
               DisplayTransformation (frame,
                                      pAb->AbElement->ElTransform, 
                                      pBox->BxWidth, 
                                      pBox->BxHeight);
           }

         if (pAb->AbOpacity != 1000
             && !TypeHasException (ExcIsGroup, pAb->AbElement->ElTypeNumber,
                                   pAb->AbElement->ElStructSchema))
           {
             if (pAb->AbFirstEnclosed)
               {
                 child = pAb->AbFirstEnclosed;
                 while (child)
                   {
                     child->AbFillOpacity = pAb->AbOpacity;      
                     child->AbStrokeOpacity = pAb->AbOpacity;
                     child = child->AbNext;
                   } 
               }
             else
               {
                 pAb->AbFillOpacity = pAb->AbOpacity;      
                 pAb->AbStrokeOpacity = pAb->AbOpacity; 
               }
           }
         if (pAb->AbLeafType == LtCompound)
           {
             /* Initialize bounding box */
             if (pAb->AbVisibility >= pFrame->FrVisibility &&
                 (pBox->BxDisplay || pAb->AbSelected))
               ComputeFilledBox (pBox, frame, xmin, xmax, ymin, ymax, show_bgimage);
             if (pBox->BxNew && pAb->AbFirstEnclosed == NULL)
               {
                 /* this is a new box */
                 pBox->BxNew = 0;
                 specAb = pAb;
                 while (!userSpec && specAb)
                   {
                     if (specAb->AbWidth.DimIsPosition ||
                         specAb->AbHeight.DimIsPosition)
                       specAb = NULL;
                     else if (specAb->AbHorizPos.PosUserSpecified ||
                              specAb->AbVertPos.PosUserSpecified ||
                              specAb->AbWidth.DimUserSpecified ||
                              specAb->AbHeight.DimUserSpecified)
                       {
                         /* one paramater is given by the user */
                         userSpec = TRUE;
                       }
                     else
                       specAb = specAb->AbEnclosing;
                   }
               }
             ComputeBoundingBoxes (frame, xmin, xmax, ymin, ymax, pAb, show_bgimage);
           }
         else
           {
             /* look for the box displayed at the top of the window */
             if (pBox->BxType == BoMulScript || pBox->BxType == BoSplit)
               {
                 /* the box itself doen't give right positions */
                 box = pBox->BxNexChild;
                 bt = box->BxClipY;
                 /* don't take into account the last empty box */
                 while (box->BxNexChild &&
                        (box->BxNexChild->BxNChars > 0 ||
                         box->BxNexChild->BxNexChild))
                   box = box->BxNexChild;
                 bb = box->BxClipY + box->BxClipH;
               }
             else
               {
                 bt = pBox->BxClipY;
                 bb = pBox->BxClipY + pBox->BxClipH;
               }
		  
             if (pBox->BxType == BoSplit || pBox->BxType == BoMulScript)
               /* the box itself doen't give right positions */
               box = pBox->BxNexChild;
             else
               box = pBox;
             if (topBox == NULL)
               topBox = box;
             else if (bt >= winTop && topBox->BxClipY < winTop)
               /* the top of the box should be visible */
               topBox = box;
             userSpec = FALSE;
             if (pBox->BxNew)
               {
                 /* this is a new box */
                 pBox->BxNew = 0;
                 specAb = pAb;
                 while (!userSpec && specAb)
                   {
                     if (specAb->AbWidth.DimIsPosition ||
                         specAb->AbHeight.DimIsPosition)
                       specAb = NULL;
                     else if (specAb->AbHorizPos.PosUserSpecified ||
                              specAb->AbVertPos.PosUserSpecified ||
                              specAb->AbWidth.DimUserSpecified ||
                              specAb->AbHeight.DimUserSpecified)
                       {
                         /* one paramater is given by the user */
                         userSpec = TRUE;
                       }
                     else
                       specAb = specAb->AbEnclosing;
                   }
               }
             if (!userSpec)
               {
                 if (pBox->BxType == BoSplit || pBox->BxType == BoMulScript)
                   while (pBox->BxNexChild)
                     {
                       pBox = pBox->BxNexChild;
                       ComputeBoundingBox (pBox, frame, xmin, xmax, ymin, ymax);
                     }
                 else
                   ComputeBoundingBox (pBox, frame, xmin, xmax, ymin, ymax);
               }
           }

         if (pAb->AbElement && formatted)
           IfPopMatrix (pAb);

         /* X and Y is the smallest of all enclosed boxes*/
         if (pBox->BxBoundinBoxComputed)
           {
             if (clipx == -1)
               clipx = pBox->BxClipX;
             else if (pBox->BxClipX < clipx) 
               {
                 clipw += clipx - pBox->BxClipX;
                 clipx = pBox->BxClipX;	  
               }
             if (clipy == -1)
               clipy = pBox->BxClipY;
             else if (pBox->BxClipY < clipy) 
               {
                 cliph += clipy - pBox->BxClipY;
                 clipy = pBox->BxClipY; 
               }
             /* Make sure that Height and Width is correct...*/
             if ((pBox->BxClipW + pBox->BxClipX) > (clipx + clipw))
               clipw = (pBox->BxClipW + pBox->BxClipX) - clipx;		  
             if ((pBox->BxClipY + pBox->BxClipH) > (clipy + cliph))
               cliph = (pBox->BxClipY + pBox->BxClipH) - clipy;		  
           }
       }
     /* get the next sibling */
     pAb = pAb->AbNext;
   }

  pBox = pInitAb->AbBox;
  if (clipx != -1)
    pBox->BxClipX = clipx;	  
  if (clipy != -1)
    pBox->BxClipY = clipy;
  pBox->BxClipW = clipw; 
  pBox->BxClipH = cliph; 
  if (pBox->BxClipW && pBox->BxClipH)
    pBox->BxBoundinBoxComputed = TRUE;
  else
    pBox->BxBoundinBoxComputed = FALSE;
}

/*----------------------------------------------------------------------
  NoBoxModif : True if box or box's son not modified
  ----------------------------------------------------------------------*/
ThotBool NoBoxModif (PtrAbstractBox pinitAb)
{
  PtrAbstractBox      pAb;

  if (pinitAb->AbBox->VisibleModification || pinitAb->AbSelected)
    return FALSE;
  pAb = pinitAb->AbFirstEnclosed;
  while (pAb)
    {
      if (pAb->AbBox->VisibleModification || pAb->AbSelected)
        return FALSE;
      if (pAb->AbFirstEnclosed)
        /* get the first child */
        pAb = pAb->AbFirstEnclosed;
      else if (pAb->AbNext)	    
        {
          /* get the next sibling */
          pAb = pAb->AbNext;
        }
      else
        {
          /* go up in the tree */
          while (pAb->AbEnclosing && pAb->AbEnclosing->AbNext == NULL)
            pAb = pAb->AbEnclosing;
          pAb = pAb->AbEnclosing;
          if (pAb)
            pAb = pAb->AbNext;
        }
    }
  return TRUE;
}
#endif /*_GL*/

/*----------------------------------------------------------------------
  DisplayAllBoxes crosses the Abstract tree from the root top to bottom
  and left to rigth to display all visible boxes.
  pFlow points to the displayed flow or NULL when it's the main flow.
  Parameters xmin, xmax, ymin, ymax give the clipped area.
  tVol and bVol return the volume of not displayed boxes on thetop and
  on the bottom of the window.
  ----------------------------------------------------------------------*/
PtrBox DisplayAllBoxes (int frame, PtrFlow pFlow,
                        int xmin, int xmax, int ymin, int ymax,
                        int *tVol, int *bVol)
{
  PtrAbstractBox      pAb, root, pNext;
  PtrBox              pBox, box, topBox;
  ViewFrame          *pFrame;
  int                 plane, nextplane;
  int                 winTop, winBottom;
  int                 bt, bb;
  int                 l, h;
  int                 x_min, x_max, y_min, y_max;
  ThotBool            selected;
  ThotBool            show_bgimage;
#ifdef _GL
  PtrBox              systemOriginRoot = NULL;
  int                 xOrg, yOrg, clipXOfFirstCoordSys, clipYOfFirstCoordSys;
  ThotBool            updatingStatus, formatted;
  ThotBool            not_g_opacity_displayed, not_in_feedback;

  updatingStatus = FrameUpdating;
  FrameUpdating = TRUE;  
#endif /* _GL */
  pFrame = &ViewFrameTable[frame - 1];
  if (pFlow)
    root = pFlow->FlRootBox;
  else
    root = pFrame->FrAbstractBox;
  pAb = root;
  // Check if background images should be displayed
  TtaGetEnvBoolean ("ENABLE_BG_IMAGES", &show_bgimage);
  /*if (show_bgimage)
    TtaGetEnvBoolean ("LOAD_IMAGES", &show_bgimage);*/
  pBox = pAb->AbBox;
  if (pBox == NULL)
    return NULL;
  /* Display planes in reverse order from biggest to lowest */
  plane = 65536;
  nextplane = plane - 1;
  *tVol = *bVol = 0;
  topBox = NULL;
  GetSizesFrame (frame, &l, &h);

#ifdef _GL
  winTop = 0;
  winBottom = h;
  formatted = (FrameTable[frame].FrView == 1 &&
               pAb->AbPSchema &&
               pAb->AbPSchema->PsStructName &&
               strcmp (pAb->AbPSchema->PsStructName, "TextFile"));
  x_min = xmin - pFrame->FrXOrg;
  x_max = xmax - pFrame->FrXOrg;
  y_min = ymin - pFrame->FrYOrg;
  y_max = ymax - pFrame->FrYOrg;
  xOrg = 0;
  yOrg = 0;
  clipXOfFirstCoordSys = clipYOfFirstCoordSys = 0;
  not_in_feedback =  GL_NotInFeedbackMode ();
#else /* _GL */
  winTop = pFrame->FrYOrg;
  winBottom = winTop + h;
  x_min = xmin;
  x_max = xmax;
  y_min = ymin;
  y_max = ymax;
#endif /* _GL */
  if (pFlow)
    {
      /* take into account relative positioning */
      x_min -= pFlow->FlXStart;
      x_max -= pFlow->FlXStart;
      y_min -= pFlow->FlYStart;
      y_max -= pFlow->FlYStart;
    }

  selected = pAb->AbSelected;
  if (pAb->AbVis != 'H' && (pBox->BxDisplay || selected) && pFlow == NULL)
    DrawFilledBox (pBox, pAb, frame, pFlow, xmin, xmax, ymin, ymax,
                   selected, TRUE, TRUE, show_bgimage);
  while (plane != nextplane)
    /* there is a new plane to display */
    {
      plane = nextplane;
      /* Draw all the boxes not yet displayed */
      pAb = root;
#ifdef _GL
      not_g_opacity_displayed = TRUE;
#endif /* _GL */
      while (pAb)
        {
          if (pAb->AbBox && pAb->AbElement &&
              /* don't display the document element */
              pAb != pFrame->FrAbstractBox &&
              /* skip extra flow child */
              (pAb == root || !IsFlow (pAb->AbBox, frame)))
            {
              pBox = pAb->AbBox;
              if (pAb->AbDepth == plane)
                {
                  if (pAb->AbDocView == 1 &&
                      TypeHasException (ExcIsMarker, pAb->AbElement->ElTypeNumber,
                                        pAb->AbElement->ElStructSchema))
                    selected = pAb->AbSelected = FALSE;
                  else if (!selected && pAb->AbSelected)
                    // open the selected sequence
                    selected = pAb->AbSelected;
                }
#ifdef _GL
              if (not_g_opacity_displayed)
                {
                  if (formatted)
                    {
                      if (pAb->AbSelected && pAb->AbDepth == plane &&
                          TypeHasException (ExcIsGroup,
                                            pAb->AbElement->ElTypeNumber,
                                            pAb->AbElement->ElStructSchema) )
                        {
                          /* Draw the selection on a group */
                          glPushMatrix ();
                          glLoadIdentity();
                          DrawRectangle (frame, 0, 5,
                                         pBox->BxClipX,
                                         pBox->BxClipY,
                                         pBox->BxClipW,
                                         pBox->BxClipH,
                                         TtaGetThotColor(255, 100, 100), 0, 4);
                          glPopMatrix();
                        }

                      if (IfPushMatrix (pAb))
                        {
                          /* If the coord sys origin is translated, 
                             it must be before any other transformation */
                          if (pAb->AbElement->ElSystemOrigin)
                              DisplayBoxTransformation (pAb->AbElement->ElTransform, 
                                                        pFrame->FrXOrg, pFrame->FrYOrg);
                          /* Normal transformation*/
                          if (pAb->AbElement->ElTransform)
                            DisplayTransformation (frame,
                                                   pAb->AbElement->ElTransform, 
                                                   pBox->BxWidth, pBox->BxHeight);
                          if (pAb->AbElement->ElSystemOrigin &&
                              /* skip boxes already managed */
                              !IsParentBox (systemOriginRoot, pBox))
                            {
                              /*Need to Get REAL computed COORD ORIG
                                instead of Computing Bounding Boxes forever...
                                As it's an "optimisation it'll come later :
                                if computed, no more compute, use synbounding, 
                                else compute if near screen"*/
                               if (OriginSystemSet (pAb, pFrame,
                                                    &systemOriginRoot,
                                                    &xOrg, &yOrg,
                                                    &clipXOfFirstCoordSys, &clipYOfFirstCoordSys))
                                 {
                                   ComputeBoundingBoxes (frame, x_min, x_max,
                                                         y_min, y_max, pAb,
                                                         show_bgimage);
                                   clipXOfFirstCoordSys = pBox->BxClipX;
                                   clipYOfFirstCoordSys = pBox->BxClipY;
                                 }
                               else
                                 ComputeBoundingBoxes (frame, x_min, x_max,
                                                       y_min, y_max, pAb,
                                                       show_bgimage);
                                 
                            }

                          if (pAb->AbDepth == plane)
                            {
                              // manage transparent groups
                              if (pAb->AbOpacity != 1000 &&  not_in_feedback)
                                {
                                  if (TypeHasException (ExcIsGroup,
                                                        pAb->AbElement->ElTypeNumber,
                                                        pAb->AbElement->ElStructSchema) )
                                    {
                                      if (pAb->AbOpacity == 0 ||
                                          (NoBoxModif (pAb) &&
                                           pAb->AbBox->Post_computed_Pic != NULL))
                                        {
                                          not_g_opacity_displayed = FALSE;
                                          continue;
                                        }
                                      else
                                        {
                                          if (pAb->AbBox->Post_computed_Pic)
                                            {
                                              FreeGlTextureNoCache (pAb->AbBox->Post_computed_Pic);
                                              TtaFreeMemory (pAb->AbBox->Post_computed_Pic);
                                              pAb->AbBox->Post_computed_Pic = NULL; 
                                            }
                                          OpaqueGroupTexturize (pAb, frame,
                                                                x_min, x_max,
                                                                y_min, y_max, TRUE);
                                          ClearOpaqueGroup (pAb, frame, x_min, x_max,
                                                            y_min, y_max);
                                        }
                                    }
                                  else if (pAb->AbFirstEnclosed)
                                    {
                                      pAb->AbFirstEnclosed->AbFillOpacity = pAb->AbOpacity;
                                      pAb->AbFirstEnclosed->AbStrokeOpacity = pAb->AbOpacity;
                                    }
                                  else 
                                    {
                                      pAb->AbFillOpacity = pAb->AbOpacity;      
                                      pAb->AbStrokeOpacity = pAb->AbOpacity; 
                                    }
                                }
                            }
                        }
                    }
#endif /* _GL */
                  if (pAb->AbDepth == plane)
                    {
                      // draw boxes
                      if (pAb->AbLeafType == LtCompound)
                        {
                          if (pAb->AbVisibility >= pFrame->FrVisibility &&
                              pAb->AbVis != 'H' &&
                              (pBox->BxDisplay || pAb->AbSelected))
                            DrawFilledBox (pBox, pAb, frame, pFlow,
                                           xmin, xmax, ymin, ymax,
                                           selected, TRUE, TRUE, show_bgimage);
                          if (pAb->AbSelected && pAb->AbDocView == 1 &&
                              (TypeHasException (ExcIsGroup, pAb->AbElement->ElTypeNumber,
                                                pAb->AbElement->ElStructSchema) ||
                               TypeHasException (ExcIsDraw, pAb->AbElement->ElTypeNumber,
                                                pAb->AbElement->ElStructSchema)))
                            selected = FALSE;
                          if (pBox->BxNew && pAb->AbFirstEnclosed == NULL)
                            /* this is a new box */
                            pBox->BxNew = 0;
                        }
                      else
                        {
                          /* look for the box displayed at the top of the window */
                          if (pBox->BxType == BoSplit || pBox->BxType == BoMulScript)
                            {
                              /* the box itself doesn't give right positions */
                              box = pBox->BxNexChild;
#ifdef _GL
                              bt = box->BxClipY;
#else /* _GL */
                              bt = box->BxYOrg;
#endif /* _GL */
                              /* don't take into account the last empty box */
                              while (box->BxNexChild &&
                                     (box->BxNexChild->BxNChars > 0 ||
                                      box->BxNexChild->BxNexChild))
                                box = box->BxNexChild;
#ifdef _GL
                              bb = box->BxClipY + box->BxClipH;
#else /* _GL */
                              bb = box->BxYOrg + box->BxHeight;
#endif /* _GL */
                            }
                          else
                            {
#ifdef _GL
                              bt = pBox->BxClipY;
                              bb = pBox->BxClipY + pBox->BxClipH;
#else /* _GL */
                              bt = pBox->BxYOrg;
                              bb = pBox->BxYOrg + pBox->BxHeight;
#endif /* _GL */
                            }
                          if (bb < winTop)
                            /* the box is not visible */
                            *tVol = *tVol + pAb->AbVolume;
                          else if (bt > winBottom)
                            /* the box is not visible */
                            *bVol = *bVol + pAb->AbVolume;
                          else
                            {
                              if (pBox->BxType == BoSplit || pBox->BxType == BoMulScript)
                                /* the box itself doen't give right positions */
                                box = pBox->BxNexChild;
                              else
                                box = pBox;
                              if (topBox == NULL)
                                topBox = box;
#ifdef _GL
                              else if (bt >= winTop && topBox->BxClipY < winTop)
#else /* _GL */
                              else if (bt >= winTop && topBox->BxYOrg < winTop)
#endif /* _GL */
                                /* the top of the box should be visible */
                                topBox = box;
                              if (pAb->AbVis != 'H')
                                {
                                  if (pBox->BxType == BoSplit || pBox->BxType == BoMulScript)
                                    while (pBox->BxNexChild)
                                      {
                                        pBox = pBox->BxNexChild;
#ifdef _GL
                                        if (pBox->BxClipY + pBox->BxClipH >= y_min  &&
                                            pBox->BxClipY <= y_max && 
                                            pBox->BxClipX + pBox->BxClipW  + pBox->BxEndOfBloc>= x_min &&
                                            pBox->BxClipX <= x_max)
#else /* _GL */
                                          if (pBox->BxYOrg + pBox->BxHeight >= y_min  &&
                                              pBox->BxYOrg <= y_max && 
                                              pBox->BxXOrg + pBox->BxWidth + pBox->BxEndOfBloc >= x_min &&
                                              pBox->BxXOrg <= x_max)
#endif /* _GL */
                                            DisplayBox (pBox, frame, xmin, xmax, ymin, ymax, pFlow, selected);
                                      }
#ifdef _GL
                                  else if (bb >= y_min  &&
                                           bt <= y_max && 
                                           pBox->BxClipX + pBox->BxClipW >= x_min &&
                                           pBox->BxClipX <= x_max)
#else /* _GL */
                                  else if (bb >= y_min  &&
                                           bt <= y_max && 
                                           pBox->BxXOrg + pBox->BxWidth + pBox->BxEndOfBloc >= x_min &&
                                           pBox->BxXOrg <= x_max)
#endif /* _GL */
                                    DisplayBox (pBox, frame, xmin, xmax, ymin, ymax, pFlow, selected);
                                }
                            }
                        }
                    }
                }
#ifdef _GL
            }
#endif /* _GL */

          if (pAb->AbDepth < plane &&
              (plane == nextplane || pAb->AbDepth > nextplane))
            /* keep the lowest value for plane depth */
            nextplane = pAb->AbDepth;

          /* Now get the next abstract box */
          if (pAb->AbLeafType == LtCompound && pAb->AbFirstEnclosed &&
#ifdef _GL
              not_g_opacity_displayed &&
#endif /* _GL */
              (pAb == root || !IsFlow (pAb->AbBox, frame)))
            // go down
            pAb = pAb->AbFirstEnclosed;
          else
            {
              // go next or up
              pNext = pAb->AbNext;
              // --------------------------------------------------------------
              do
                {
#ifdef _GL
                  if (formatted && pAb->AbDepth == plane)
                    OpacityAndTransformNext (pAb, plane, frame, x_min, x_max,
                                             y_min, y_max, not_in_feedback);
                  if (IfPopMatrix (pAb))
                    if (pAb->AbBox == systemOriginRoot)
                      OriginSystemExit (pAb, pFrame, &systemOriginRoot,
					&xOrg, &yOrg, 
                                        &clipXOfFirstCoordSys, &clipYOfFirstCoordSys);

                  not_g_opacity_displayed = TRUE;
#endif /* _GL */
                  if (pAb->AbSelected)
                    // close the selected sequence
                    selected = FALSE;
                  if (pAb == root)
                    /* all boxes are now managed: stop the loop */
                    pAb = pNext = NULL;
                  else if (pNext)
                    break;
                  else
                    {
                      /* go up in the tree */
                      pAb = pAb->AbEnclosing;
                      pNext = pAb->AbNext;
                    }
                }
              while (pAb);
              pAb = pNext;
              // --------------------------------------------------------------
            }
        }
    }

#ifdef _GL
  FrameUpdating = updatingStatus;  
#endif /* _GL */
  return topBox;
}

/*----------------------------------------------------------------------
  NeedToComputeABoundingBox:
  Return TRUE if the bounding box need to be recomputed
  ----------------------------------------------------------------------*/
ThotBool NeedToComputeABoundingBox (PtrAbstractBox pAb, int frame)
{
#ifdef _GL
  return (FrameTable[frame].FrView == 1 &&
          pAb->AbPSchema &&
          pAb->AbPSchema->PsStructName &&
          (strcmp (pAb->AbPSchema->PsStructName, "TextFile") != 0) &&
          IsBoxTransformed (pAb->AbBox));
#else /* _GL */
  return FALSE;
#endif /* _GL */
}

/*----------------------------------------------------------------------
  ComputeChangedBoundingBoxes
  Compute bounding boxes after an update of the view.
  ----------------------------------------------------------------------*/
void ComputeChangedBoundingBoxes (int frame)
{
#ifdef _GL
  PtrAbstractBox      pAb, root, pNext;
  PtrBox              pBox;
  ViewFrame          *pFrame;
  PtrBox              systemOriginRoot = NULL;
  int                 plane, nextplane;
  int                 l, h;
  int                 oldXOrg, oldYOrg, clipXOfFirstCoordSys, clipYOfFirstCoordSys;
  ThotBool            updateStatus;
  ThotBool            show_bgimage, formatted;
   

  // Check if background images should be displayed
  TtaGetEnvBoolean ("ENABLE_BG_IMAGES", &show_bgimage);
  /*if (show_bgimage)
    TtaGetEnvBoolean ("LOAD_IMAGES", &show_bgimage);*/
  updateStatus = FrameUpdating;
  FrameUpdating = TRUE;  
  pFrame = &ViewFrameTable[frame - 1];
  // @@@@ TODO: check positioned boxes
  pAb = pFrame->FrAbstractBox;
  formatted =  (FrameTable[frame].FrView == 1 && pAb->AbPSchema &&
                pAb->AbPSchema->PsStructName &&
                strcmp (pAb->AbPSchema->PsStructName, "TextFile"));
  root = pAb;
  if (pAb == NULL)
    return;
  pBox = pAb->AbBox;
  if (pBox == NULL)
    return;
  // to be sure the canvas is ready to draw something
  if (!GL_prepare (frame))
    return;

  GetSizesFrame (frame, &l, &h);
  /* Display planes in reverse order from biggest to lowest */
  plane = 65536;
  nextplane = plane - 1;
  oldXOrg = 0;
  oldYOrg = 0;
  clipXOfFirstCoordSys = clipYOfFirstCoordSys = 0;
  while (plane != nextplane)
    /* there is a new plane to display */
    {
      plane = nextplane;
      /* Draw all the boxes not yet displayed */
      pAb = pFrame->FrAbstractBox;
      while (pAb)
        {
          if (pAb != pFrame->FrAbstractBox && pAb->AbElement && pAb->AbBox)
            {
              pBox = pAb->AbBox;
              /* If the coord sys origin is translated, 
                 it must be before any other transformation*/
              if (formatted && IfPushMatrix (pAb))
                {
                  if (pAb->AbElement->ElSystemOrigin)
                    DisplayBoxTransformation (pAb->AbElement->ElTransform, 
                                              pFrame->FrXOrg, pFrame->FrYOrg);
                  /* Normal transformation*/
                  if (pAb->AbElement->ElTransform)
                    DisplayTransformation (frame,
                                           pAb->AbElement->ElTransform, 
                                           pBox->BxWidth, 
                                           pBox->BxHeight);
                  
                  OriginSystemSet (pAb, pFrame, &systemOriginRoot,
                                   &oldXOrg, &oldYOrg,
                                   &clipXOfFirstCoordSys, &clipYOfFirstCoordSys);

                  if (!pBox->BxBoundinBoxComputed)
                    {
                      if (pAb->AbLeafType == LtCompound)
                        {
                          if (pAb->AbVisibility >= pFrame->FrVisibility &&
                              (pBox->BxDisplay || pAb->AbSelected))
                            ComputeFilledBox (pAb->AbBox, frame, 0, l, 0, h, show_bgimage);
                          ComputeBoundingBoxes (frame, 0, l, 0, h, pAb, show_bgimage);
                        }
                      else if (pBox->BxType == BoSplit || pBox->BxType == BoMulScript)
                        {
                          while (pBox->BxNexChild)
                            {
                              pBox = pBox->BxNexChild;
                              ComputeBoundingBox (pBox, frame, 0, l, 0, h);
                            }
                        }
                      else
                        ComputeBoundingBox (pBox, frame,  0, l, 0, h);
                      if (pAb->AbDepth == plane)
                        /* need to redisplay the whole box */
                        DefBoxRegion (frame, pBox, -1, -1, -1, -1);
                    }
                }
            }
          else if (pAb->AbDepth < plane &&
                   (plane == nextplane || pAb->AbDepth > nextplane))
            /* keep the lowest value for plane depth */
            nextplane = pAb->AbDepth;

          /* get next abstract box */
          if (pAb->AbLeafType == LtCompound && pAb->AbFirstEnclosed&&
              (pAb == root || !IsFlow (pAb->AbBox, frame)))
            /* get the first child */
            pAb = pAb->AbFirstEnclosed;
          else
            {
              pNext = pAb->AbNext;
              // --------------------------------------------------------------
              do
                {
                  if (pAb->AbDepth == plane)
                    OpacityAndTransformNext (pAb, plane, frame, 0, 0, 0, 0, FALSE);
                  if (formatted && IfPopMatrix (pAb))
                    OriginSystemExit (pAb, pFrame, &systemOriginRoot,
				      &oldXOrg, &oldYOrg, 
                                      &clipXOfFirstCoordSys, &clipYOfFirstCoordSys);
              
                  if (pAb == root)
                    /* all boxes are now managed: stop the loop */
                    pAb = pNext = NULL;
                  else if (pNext)
                    break;
                  else
                    {
                      /* go up in the tree */
                      pAb = pAb->AbEnclosing;
                      pNext = pAb->AbNext;
                    }
                }
              while (pAb);
              pAb = pNext;
              // --------------------------------------------------------------
            }
        }
    } 

  FrameUpdating = updateStatus;
  glLoadIdentity ();
#endif /* _GL */
}

/*----------------------------------------------------------------------
 AddingOnTop Adds abstract boxes on top of the frame
 topBox point to the current box displayed on top
 top gives the height of the available space on top
  ----------------------------------------------------------------------*/
static void AddingOnTop (int frame, ViewFrame *pFrame, PtrBox topBox, int top)
{
  PtrElement pEl = NULL;
  int        y, delta, volume, view;

  if (topBox && topBox->BxAbstractBox)
    {
      /* register previous location */
      y = topBox->BxYOrg;
      delta = y + topBox->BxHeight;
      // keep the referred element
      pEl = topBox->BxAbstractBox->AbElement;
      view = topBox->BxAbstractBox->AbDocView;
    }

  /* Adding abstract boxes on top of the frame */
  volume = GetCharsCapacity (top, frame);
  IncreaseVolume (TRUE, volume, frame);

  // Pay attention: the box could be regenerated
  if (pEl && pEl->ElAbstractBox[view-1] &&
      topBox->BxAbstractBox != pEl->ElAbstractBox[view-1])
    topBox = pEl->ElAbstractBox[view-1]->AbBox;
  /* Recompute the location of the frame in the abstract image */
  if (topBox)
    {
      y = -y + topBox->BxYOrg;
      /* y equal the shift of previous first box */
      /* What's already displayed is related to this */
      /* previous first box location */
      pFrame->FrYOrg += y;
      /* delta equal the limit of redrawing after shifting */
      if (y > 0)
        delta = topBox->BxYOrg + topBox->BxHeight;
      /* new limit */
      pFrame->FrClipYEnd = delta;
    }
  RedrawFrameTop (frame, 0);
}

/*----------------------------------------------------------------------
  RedrawFrameTop redraw from bottom to top a frame.
  The scroll parameter indicates the height of a scroll
  back which may take place before recomputing the abstract
  image.
  The area is cleaned before redrawing.
  The origin coordinates of the abstract boxes are expected
  to be already computed.
  Return non zero if new abstract boxes were added in order
  to build the corresponding abstract image.
  ----------------------------------------------------------------------*/
ThotBool RedrawFrameTop (int frame, int scroll)
{
  PtrBox              topBox = NULL;
  PtrBox              pRootBox;
  ViewFrame          *pFrame;
  PtrFlow             pFlow;
  int                 y, tVol, bVol, h, l;
  int                 top, bottom;
  int                 xmin, xmax, ymin, ymax;
  int                 delta, t, b, plane, nextplane;
  ThotBool            toadd;  

  /* are new abstract boxes needed */
  toadd = FALSE;
  pFrame = &ViewFrameTable[frame - 1];
  GetSizesFrame (frame, &l, &h);
  xmin = pFrame->FrClipXBegin;
  xmax = pFrame->FrClipXEnd;
  ymin = pFrame->FrClipYBegin;
  ymax = pFrame->FrClipYEnd;
  if (!pFrame->FrReady || pFrame->FrAbstractBox == NULL)
    return toadd;
  else if (xmin < xmax && ymin < ymax &&
           pFrame->FrXOrg < xmax &&
           pFrame->FrYOrg - scroll < ymax &&
           pFrame->FrXOrg + l > xmin &&
           pFrame->FrYOrg - scroll + h > ymin)
    {
#ifdef _GL      
      if (GL_prepare (frame))
        {
#endif /* _GL */
          pFrame->FrYOrg -= scroll;
#ifdef _GL
          SyncBoundingboxes (pFrame->FrAbstractBox, 0, -scroll, frame,
                             pFrame->FrXOrg, pFrame->FrYOrg);
#endif /* _GL */
          top = pFrame->FrYOrg;
          bottom = top + h;
          tVol = bVol = 0;
#if defined(_WINGUI) && !defined(_WIN_PRINT)
          WIN_GetDeviceContext (frame);
#endif /* __WINGUI && !_WINT_PRINT */
          /* Is there a need to redisplay part of the frame ? */
          if (xmin < xmax && ymin < ymax)
            {
              DefineClipping (frame, pFrame->FrXOrg, pFrame->FrYOrg,
                              &xmin, &ymin, &xmax, &ymax, 1);
	  
              pFlow = pFrame->FrFlow;
              if (pFlow)
                {
                  plane = 65536;
                  nextplane = plane - 1;
                  while (plane != nextplane)
                    {
                      plane = nextplane;
                      if (pFrame->FrAbstractBox->AbDepth == plane)
                        // display the normal flow
                        topBox = DisplayAllBoxes (frame, NULL, xmin, xmax, ymin, ymax,
                                                  &tVol, &bVol);
                      else if (pFrame->FrAbstractBox->AbDepth < plane &&
                               (plane == nextplane ||
                                pFrame->FrAbstractBox->AbDepth > nextplane))
                        /* keep the lowest value for plane depth */
                        nextplane = pFrame->FrAbstractBox->AbDepth;
                      pFlow = pFrame->FrFlow;
                      while (pFlow && pFlow->FlRootBox)
                        {
                          if (pFlow->FlRootBox->AbDepth == plane)
                            DisplayAllBoxes (frame, pFlow, xmin, xmax, ymin, ymax, &t, &b);
                          else if (pFlow->FlRootBox->AbDepth < plane &&
                                   (plane == nextplane ||
                                    pFlow->FlRootBox->AbDepth > nextplane))
                            /* keep the lowest value for plane depth */
                            nextplane = pFlow->FlRootBox->AbDepth;
                          pFlow = pFlow->FlNext;
                        }
                    }
                }
              else
                // display the normal flow
                topBox = DisplayAllBoxes (frame, NULL, xmin, xmax, ymin, ymax,
                                          &tVol, &bVol);
            }
          /* The updated area is redrawn */
          DefClip (frame, 0, 0, 0, 0);
          RemoveClipping (frame);
#if defined(_WINGUI) && !defined(_WIN_PRINT)
          WIN_ReleaseDeviceContext ();
#endif /*  _WINGUI && !WIN_PRINT */
          if (!ShowOnePage)
            {
              /* if needed complete the partial existing image */
              pRootBox = pFrame->FrAbstractBox->AbBox;
              if (!FrameUpdating && !TextInserting)
                {
                  /* The concrete image is being updated */
                  FrameUpdating = TRUE;
                  y = top - pRootBox->BxYOrg;
                  if (pFrame->FrAbstractBox->AbTruncatedHead && y < 0)
                    {
                      /* it lacks some abstract image at the top of the frame */
                      if (bVol > 0 && bVol < pFrame->FrAbstractBox->AbVolume)
                        {
                          /* free abstract boxes at the bottom */
                          DecreaseVolume (FALSE, bVol, frame);
                          DefClip (frame, 0, 0, 0, 0);
                        }
                      if (pFrame->FrAbstractBox == NULL)
                        {
                          printf ("ERR: No more abstract boxes in %d\n", frame);
                          bVol = -pFrame->FrVolume;
                        }
                      else
                        bVol = pFrame->FrVolume - pFrame->FrAbstractBox->AbVolume;

                      /* Volume to add */
                      top = (h / 2 - y) * l;
                      delta = 0;
                      /* Volume of the area to recompute */
                      toadd = TRUE;
                      AddingOnTop (frame, pFrame, topBox, top);
                      if (topBox == NULL)
                        {
                          /* No previous box. The frame is drawn */
                          /* on top of the concrete image */
                          pFrame->FrYOrg = 0;
                          /* Image should be complete */
                          FrameUpdating = FALSE;
                          RedrawFrameTop (frame, 0);
                        }
                    }
                  y = bottom - pRootBox->BxYOrg - pRootBox->BxHeight;
                  if (pFrame->FrAbstractBox->AbTruncatedTail && y > 0)
                    {
                      /* it lacks some abstract image at the bottom of the frame */
                      /* cleanup the bottom of the frame */
                      Clear (frame, l, y, 0, pRootBox->BxYOrg + pRootBox->BxHeight);
                      /* don't loop is volume didn't change */
                      tVol = pFrame->FrAbstractBox->AbVolume;
                      /* Volume to add */
                      bottom = y * l;
                      IncreaseVolume (FALSE, GetCharsCapacity (bottom, frame), frame);
                      tVol -= pFrame->FrAbstractBox->AbVolume;
                      /* Image should be completed */
                      FrameUpdating = FALSE;
                      if (tVol == 0)
                        printf ("ERR: Nothing to add\n");
                      else
                        /* Maybe image is not complete yet */
                        RedrawFrameBottom (frame, 0, NULL);
                    }
                  else
                    /* Volume computed is sufficient */
                    pFrame->FrVolume = pFrame->FrAbstractBox->AbVolume;
                }
              /* update of image is finished */
              FrameUpdating = FALSE;
            }
#ifdef _GL
          GL_realize (frame);
        }
#endif /* _GL */
    }
  else
    {
    /* The modified area is not visible */
    DefClip (frame, 0, 0, 0, 0);
    }
  return toadd;
}

/*----------------------------------------------------------------------
  RedrawFrameBottom redraw from top to bottom a frame.
  The scroll parameter indicates the height of a scroll
  which may take place before recomputing the abstract
  image.
  The area is cleaned before redrawing.
  The origin coordinates of the abstract boxes are expected
  to be already computed.
  The parameter subtree gives the root of the redisplayed subtree. When
  the subtree is NULL the whole tree is taken into account.
  Return non zero if new abstract boxes were added in order
  to build the corresponding abstract image.
  ----------------------------------------------------------------------*/
ThotBool RedrawFrameBottom (int frame, int scroll, PtrAbstractBox subtree)
{
  PtrBox              topBox, pRootBox;
  PtrElement          pEl = NULL;
  ViewFrame          *pFrame;
  PtrFlow             pFlow;
  int                 delta = 0, t, b;
  int                 y, tVol, bVol, h, l;
  int                 top, bottom, org = 0;
  int                 xmin, xmax, view;
  int                 ymin, ymax, plane, nextplane;
  ThotBool            toadd;

  /* are new abstract boxes needed */
  toadd = FALSE;
  pFrame = &ViewFrameTable[frame - 1];
  GetSizesFrame (frame, &l, &h);
  xmin = pFrame->FrClipXBegin;
  xmax = pFrame->FrClipXEnd;
  ymin = pFrame->FrClipYBegin;
  ymax = pFrame->FrClipYEnd;
  if (pFrame->FrAbstractBox && 
      pFrame->FrAbstractBox->AbElement == NULL)
    pFrame->FrAbstractBox = NULL;
  if (!pFrame->FrReady || pFrame->FrAbstractBox == NULL)
    return toadd;
  else if (xmin < xmax &&
           ymin < ymax &&
           pFrame->FrXOrg < xmax &&
           pFrame->FrYOrg + scroll < ymax &&
           pFrame->FrXOrg + l > xmin &&
           pFrame->FrYOrg + scroll + h > ymin)
    {
#ifdef _GL
      if (GL_prepare (frame))
        {
#endif /*_GL*/
          pFrame->FrYOrg += scroll;
#ifdef _GL
          SyncBoundingboxes (pFrame->FrAbstractBox, 0, scroll, frame,
                             pFrame->FrXOrg, pFrame->FrYOrg);
#endif /* _GL */
          top = pFrame->FrYOrg;
          bottom = top + h;
          tVol = bVol = 0;
          delta = top - h / 4;
          pRootBox = topBox = NULL;
          y = 0;
          /* Redraw from top to bottom all filled boxes */
#if defined(_WINGUI) && !defined(_WIN_PRINT)
          WIN_GetDeviceContext (frame);
#endif /* __WINGUI && !_WINT_PRINT */
          /* Is there a need to redisplay part of the frame ? */
          if (xmin < xmax && ymin < ymax)
            { 
              DefineClipping (frame, pFrame->FrXOrg, pFrame->FrYOrg,
                              &xmin, &ymin, &xmax, &ymax, 1);
	      
              pFlow = pFrame->FrFlow;
              if (pFlow)
                {
                  plane = 65536;
                  nextplane = plane - 1;
                  while (plane != nextplane)
                    {
                      plane = nextplane;
                      if (pFrame->FrAbstractBox->AbDepth == plane)
                        // display the normal flow
                        topBox = DisplayAllBoxes (frame, NULL, xmin, xmax, ymin, ymax,
                                                  &tVol, &bVol);
                      else if (pFrame->FrAbstractBox->AbDepth < plane &&
                               (plane == nextplane ||
                                pFrame->FrAbstractBox->AbDepth > nextplane))
                        /* keep the lowest value for plane depth */
                        nextplane = pFrame->FrAbstractBox->AbDepth;
                      pFlow = pFrame->FrFlow;
                      while (pFlow && pFlow->FlRootBox)
                        {
                          if (pFlow->FlRootBox->AbDepth == plane)
                            DisplayAllBoxes (frame, pFlow, xmin, xmax, ymin, ymax, &t, &b);
                          else if (pFlow->FlRootBox->AbDepth < plane &&
                                   (plane == nextplane ||
                                    pFlow->FlRootBox->AbDepth > nextplane))
                            /* keep the lowest value for plane depth */
                            nextplane = pFlow->FlRootBox->AbDepth;
                          pFlow = pFlow->FlNext;
                        }
                    }
                }
              else
                // display the normal flow
                topBox = DisplayAllBoxes (frame, NULL, xmin, xmax, ymin, ymax,
                                          &tVol, &bVol);

              /* The updated area is redrawn */
              DefClip (frame, 0, 0, 0, 0);
              RemoveClipping (frame);
#if defined(_WINGUI) && !defined(_WIN_PRINT)
              WIN_ReleaseDeviceContext ();
#endif /* __WINGUI && !_WINT_PRINT */
              if (!ShowOnePage)
                {
                  /* if needed complete the partial existing image */
                  pRootBox = pFrame->FrAbstractBox->AbBox;
                  if (!FrameUpdating && 
                      (!TextInserting || scroll > 0))
                    {
                      /* The concrete image is being updated */
                      FrameUpdating = TRUE;
                      y = top - pRootBox->BxYOrg;
                      if (pFrame->FrAbstractBox->AbInLine)
                        FrameUpdating = FALSE;
                      else if (pFrame->FrAbstractBox->AbTruncatedHead && 
                               y < 0)
                        {
                          /* it lacks a piece of the concrete image at the frame top */
                          /* filling on top will shift the whole concrete image */
                          top = h / 4 - y;
                          /* Height to add */
                          top = top * l;
                          /* Volume of the area to recompute */
                          toadd = TRUE;
                          AddingOnTop (frame, pFrame, topBox, top);
                        }
                      y = bottom - pRootBox->BxYOrg - pRootBox->BxHeight;
                      if (pFrame->FrAbstractBox->AbTruncatedTail && y >= 0)
                        {
                          /* it lacks some abstract image at the bottom of the frame */
                          /* Volume to add */
                          bottom = (y + h / 4) * l;
                          if (tVol > 0 && tVol < pFrame->FrAbstractBox->AbVolume)
                            {
                              /* free abstract boxes on top of the frame */
                              if (topBox && topBox->BxAbstractBox)
                                {
                                  org = topBox->BxYOrg;
                                  // keep the referred element
                                  pEl = topBox->BxAbstractBox->AbElement;
                                  view = topBox->BxAbstractBox->AbDocView;
                                }
                              DecreaseVolume (TRUE, tVol, frame);

                              // Pay attention: the box could be regenerated
                              if (pEl && pEl->ElAbstractBox[view-1] &&
                                  topBox->BxAbstractBox != pEl->ElAbstractBox[view-1])
                                topBox = pEl->ElAbstractBox[view-1]->AbBox;
                              DefClip (frame, 0, 0, 0, 0);
                              /* check location of frame in concrete image */
                              if (topBox)
                                pFrame->FrYOrg = pFrame->FrYOrg - org + topBox->BxYOrg;
                            }
                          if (pFrame->FrAbstractBox == NULL)
                            {
                              printf ("ERR: No more abstract box in %d\n", frame);
                              tVol = -pFrame->FrVolume;
                            }
                          else
                            tVol = pFrame->FrVolume - pFrame->FrAbstractBox->AbVolume;
			  
                          /* cleanup the bottom of the frame */
                          if (tVol)
                            Clear (frame, l, y, 0, pRootBox->BxYOrg + pRootBox->BxHeight);
                          /* don't loop is volume didn't change */
                          tVol = pFrame->FrAbstractBox->AbVolume;
                          IncreaseVolume (FALSE, GetCharsCapacity (bottom, frame), frame);
                          tVol -= pFrame->FrAbstractBox->AbVolume;
                          /* Image should be completed */
                          FrameUpdating = FALSE;
                          if (tVol != 0)
                            /* Maybe the image is not complete yet */
                            RedrawFrameBottom (frame, 0, NULL);
                        }
                      else
                        {
                          /* Volume computed is sufficient */
                          /* Is a cleanup of the bottom of frame needed ? */
                          if (y > 0)
                            Clear (frame, l, y, 0, 
                                   pRootBox->BxYOrg + pRootBox->BxHeight);
                          pFrame->FrVolume = pFrame->FrAbstractBox->AbVolume;
                        }
                    }
                  /* update of image is finished */
                  FrameUpdating = FALSE;
                }
            }
        }
#ifdef _GL 
      GL_realize (frame);
    }
#endif /* _GL */
  else
    {
      /* Nothing to draw */
#if defined(_WINGUI) && !defined(_WIN_PRINT)
      WIN_GetDeviceContext (frame);
#endif /* __WINGUI && !_WINT_PRINT */
      DefClip (frame, 0, 0, 0, 0);
      RemoveClipping (frame);
#if defined(_WINGUI) && !defined(_WIN_PRINT)
      WIN_ReleaseDeviceContext ();
#endif /* __WINGUI && !_WINT_PRINT */
    }
  return toadd;
}


/*----------------------------------------------------------------------
  DisplayFrame display one view of the document in frame.
  If a part of the abstract image is selected, the
  corresponding concrete image is centered in the frame.
  ----------------------------------------------------------------------*/
void DisplayFrame (int frame)
{
  ViewFrame          *pFrame;
  int                 w, h;

  if (frame > 0 && FrameTable[frame].FrDoc > 0 &&
      documentDisplayMode[FrameTable[frame].FrDoc - 1] == DisplayImmediately)
    {     
      /* Check that the frame exists */
      pFrame = &ViewFrameTable[frame - 1];
      if (pFrame->FrAbstractBox != NULL)
        {
          /* Drawing of the updated area */
          RedrawFrameBottom (frame, 0, NULL);	  
          /* recompute scrolls */
          CheckScrollingWidthHeight (frame);
#ifdef _GL
          if (FrameTable[frame].SwapOK)
            {
              UpdateScrollbars (frame);
              GL_Swap (frame);
            }
#endif /* _GL */
        }
      else
        {
          /* clean the frame */
          GetSizesFrame (frame, &w, &h);
          Clear (frame, w, h, 0, 0);
#ifdef _GL
          GL_Swap (frame);
#endif /* _GL */
        } 
    }
}
