/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2008
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * visualization of Selection.
 *
 * Author: I. Vatton (INRIA)
 *
 */
#include "thot_gui.h"
#include "ustring.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "appdialogue.h"

#define THOT_EXPORT extern
#include "boxes_tv.h"
#include "platform_tv.h"
#include "select_tv.h"
#include "edit_tv.h"
#include "frame_tv.h"

#include "boxmoves_f.h"
#include "boxpositions_f.h"
#include "displayselect_f.h"
#include "exceptions_f.h"
#include "font_f.h"
#include "frame_f.h"
#include "units_f.h"
#include "xwindowdisplay_f.h"


#ifdef _GL
#include "glwindowdisplay.h"
#endif /*_GL*/

/*----------------------------------------------------------------------
  DisplayPointSelection draw control points of the box.
  Display with resize colors when the parameter could_resize is set
  ----------------------------------------------------------------------*/
void DisplayPointSelection (int frame, PtrBox pBox, int pointselect,
                            ThotBool could_resize)
{
  ViewFrame          *pFrame;
  PtrAbstractBox      pAb;
  PtrTextBuffer       pBuffer;
  PtrPathSeg          pPa, pPaStart;
  PtrElement          pEl;
  int                 leftX, middleX, rightX;
  int                 topY, middleY, bottomY;
  int                 t, b, l, r;
  int                 thick, halfThick;
  int                 i, j;
  int                 x, y, bg, fg;
  int                 xstart, ystart, xctrlstart, yctrlstart;
  int                 xend, yend, xctrlend, yctrlend;
  ThotBool            svg_or_img;
  int i_start;

  if (pBox)
    {
      pFrame = &ViewFrameTable[frame - 1];
      pAb = pBox->BxAbstractBox;
      if (pAb->AbPresentationBox)
        // don't display handles of presentation boxes
        return;

      pEl = pAb->AbElement;
      thick = HANDLE_WIDTH;
      svg_or_img = (pAb->AbLeafType == LtPicture ||
                    (pEl && !pAb->AbPresentationBox &&
                     TypeHasException (ExcIsDraw, pEl->ElTypeNumber, pEl->ElStructSchema)));
      if (could_resize && svg_or_img)
          // display larger handles
        thick += 2;
      if (thick > pBox->BxWidth)
        thick = pBox->BxWidth;
      if (thick > pBox->BxHeight)
        thick = pBox->BxHeight;
      halfThick = thick / 2;

      /* selection points */
      GetExtraMargins (pBox, frame, FALSE, &t, &b, &l, &r);
      if (pEl->ElSystemOrigin)
        {
          leftX = - halfThick;
          topY = - halfThick;
        }
      else
        {
          leftX = l + pBox->BxXOrg + pBox->BxLMargin + pBox->BxLBorder
            + pBox->BxLPadding - pFrame->FrXOrg - halfThick;
          topY = t + pBox->BxYOrg + pBox->BxTMargin + pBox->BxTBorder
            + pBox->BxTPadding - pFrame->FrYOrg - halfThick;
        }
      bottomY = topY + pBox->BxH - 1;
      rightX = leftX + pBox->BxW - 1;
      middleX = leftX + (pBox->BxW / 2) - halfThick/2;
      middleY = topY + (pBox->BxH / 2) - halfThick/2;
          // select the handle color
      if (could_resize)
        {
          bg = ResizeBgSelColor;
          fg = ResizeFgSelColor;
        }
      else
        {
          bg = BgSelColor;
          fg = BgSelColor;
        }

      if (svg_or_img)
        {
          /* 8 control points */
          DrawRectangle (frame, 1, 0, leftX+halfThick, topY+halfThick, pBox->BxW, pBox->BxH - 1,
              fg, 0, 0);
          DrawRectangle (frame, 1, 0, leftX, topY, thick, thick,
              fg, bg, 2);
          DrawRectangle (frame, 1, 0, middleX, topY, thick, thick,
              fg, bg, 2);
          DrawRectangle (frame, 1, 0, rightX, topY, thick, thick,
              fg, bg, 2);
          DrawRectangle (frame, 1, 0, leftX, middleY, thick, thick,
              fg, bg, 2);
          DrawRectangle (frame, 1, 0, rightX, middleY, thick, thick,
              fg, bg, 2);
          DrawRectangle (frame, 1, 0, leftX, bottomY, thick, thick,
              fg, bg, 2);
          DrawRectangle (frame, 1, 0, middleX, bottomY, thick, thick,
              fg, bg, 2);
          DrawRectangle (frame, 1, 0, rightX, bottomY, thick, thick,
              fg, bg, 2);
        }
      else if (pAb->AbLeafType == LtPolyLine && pBox->BxNChars > 1)
        {
          /* Draw control points of the polyline */
          /* if there is at least one point in the polyline */
          pBuffer = pBox->BxBuffer;
          leftX = pBox->BxXOrg - pFrame->FrXOrg - halfThick;
          topY = pBox->BxYOrg - pFrame->FrYOrg - halfThick;
          j = 1;
          for (i = 1; pBuffer; i++)
            {
              if (pointselect == 0 || pointselect == i)
                {
                  x = leftX + PixelValue (pBuffer->BuPoints[j].XCoord,
                                          UnPixel, NULL,
                                          ViewFrameTable[frame - 1].FrMagnification);
                  y = topY + PixelValue (pBuffer->BuPoints[j].YCoord,
                                         UnPixel, NULL,
                                         ViewFrameTable[frame - 1].FrMagnification);
                  DrawRectangle (frame, 0, 0, x, y, thick, thick, fg, bg, 2);
                }
	      
              j++;
              if (j == pBuffer->BuLength)
                {
		  pBuffer = pBuffer->BuNext;
		  j = 0;
                }
            }
        }
      else if (pAb->AbLeafType == LtPath)
        {
          /* Draw the points of the path */
          pPa = pAb->AbFirstPathSeg;
          leftX = pBox->BxXOrg - pFrame->FrXOrg;
          topY = pBox->BxYOrg - pFrame->FrYOrg;
	  i = 1;
	  
          while (pPa)
            {
	      xstart = leftX + PixelValue (pPa->XStart, UnPixel, NULL,
					   ViewFrameTable[frame - 1].FrMagnification);
	      ystart = topY + PixelValue (pPa->YStart, UnPixel, NULL,
					   ViewFrameTable[frame - 1].FrMagnification);
	      xctrlstart = leftX + PixelValue (pPa->XCtrlStart, UnPixel, NULL,
					   ViewFrameTable[frame - 1].FrMagnification);
	      yctrlstart = topY + PixelValue (pPa->YCtrlStart, UnPixel, NULL,
					   ViewFrameTable[frame - 1].FrMagnification);
	      xend = leftX + PixelValue (pPa->XEnd, UnPixel, NULL,
					   ViewFrameTable[frame - 1].FrMagnification);
	      yend = topY + PixelValue (pPa->YEnd, UnPixel, NULL,
					   ViewFrameTable[frame - 1].FrMagnification);
	      xctrlend = leftX + PixelValue (pPa->XCtrlEnd, UnPixel, NULL,
					   ViewFrameTable[frame - 1].FrMagnification);
	      yctrlend = topY + PixelValue (pPa->YCtrlEnd, UnPixel, NULL,
					   ViewFrameTable[frame - 1].FrMagnification);


              if ((pPa->PaNewSubpath || !pPa->PaPrevious))
		{
		  /* this path segment starts a new subpath */
		  pPaStart = pPa;
		  i_start = i;

                  if(pointselect == 0 || /* The whole path is selected */
		     pointselect == i || /* Current point selected */

		     (pointselect == i+1 && /* Next control point selected*/
		       (pPa->PaShape == PtCubicBezier ||
			pPa->PaShape == PtQuadraticBezier))

		     )

		    /* draw the start point of this path segment */
		      DrawRectangle (frame, 0, 0, xstart - halfThick,
				     ystart - halfThick,
				     thick, thick, fg, bg, 2);
		  i++;
                }

	      if(pointselect > 0 &&
		 (pPa->PaShape == PtCubicBezier ||
		 pPa->PaShape == PtQuadraticBezier))
		{
		  /* Check if we draw Bezier Control */
		  if(
		     /*
		       i-2     i-1     i   
		       O------x-------0

		      */
		     pointselect == i-1 || /* A point is selected */
		     pointselect == i   || /* Current control point selected */

		     (pointselect == i-2 &&/* Previous control point selected */
		      !(pPa->PaNewSubpath) &&
		       pPa->PaPrevious &&
		       (pPa->PaPrevious->PaShape == PtCubicBezier ||
			pPa->PaPrevious->PaShape == PtQuadraticBezier)
		      )

		     )
		    DrawBezierControl (frame, thick, xstart, ystart,
				       xctrlstart, yctrlstart, bg, fg);
		  
		  i++;


		  /* Check if we draw Bezier Control */
		  if(
		     /*
		       i     i+1     i+2
		       O------x-------0

		      */

		     pointselect == i+1 || /* A point is selected */
		     pointselect == i   || /* Current control point selected */
		     
		     (pointselect == i+2 && /* Next control point selected */
		      pPa->PaNext && !(pPa->PaNext->PaNewSubpath)
		      && (pPa->PaNext->PaShape == PtCubicBezier ||
			  pPa->PaNext->PaShape == PtQuadraticBezier)  )
		     )
		    DrawBezierControl (frame, thick, xend, yend,
				       xctrlend, yctrlend, bg, fg);
		  
		  i++;
		}

	      if(pointselect == 0 || /* The whole path is selected */
		 pointselect == i || /* Current point selected */

		 (pointselect == i-1 && /* Previous control point selected*/
		 (pPa->PaShape == PtCubicBezier ||
		  pPa->PaShape == PtQuadraticBezier)) ||

		 (pointselect == i+1 && /* Next control point selected */
		  pPa->PaNext && !(pPa->PaNext->PaNewSubpath)
		  && (pPa->PaNext->PaShape == PtCubicBezier ||
		      pPa->PaNext->PaShape == PtQuadraticBezier)  )
		 )
		/* Draw the end point of the path segment */
		DrawRectangle (frame, 0, 0, xend - halfThick, yend - halfThick,
			       thick, thick, fg, bg, 2);
	
	      if(!pPa->PaNext || pPa->PaNext->PaNewSubpath)
		{
		  /* Check if the first and last point of the subpath are
		     connected using a Bezier fragment */
		  if((pPaStart->PaShape == PtCubicBezier ||
		      pPaStart->PaShape == PtQuadraticBezier) &&
		    (pPa->PaShape == PtCubicBezier ||
		     pPa->PaShape == PtQuadraticBezier) &&
		     pPa->XEnd == pPaStart->XStart &&
		     pPa->YEnd == pPaStart->YStart)
		    {
		      if(pointselect == i || pointselect == i-1)
			{
			  /* Last point of the subpath is active, draw the
			   Bezier control at the beginning */
			  xstart = leftX + PixelValue (pPaStart->XStart,
						       UnPixel, NULL,
				   ViewFrameTable[frame - 1].FrMagnification);
			  ystart = topY + PixelValue (pPaStart->YStart,
						      UnPixel, NULL,
				   ViewFrameTable[frame - 1].FrMagnification);
			  xctrlstart = leftX + PixelValue (pPaStart->XCtrlStart,
						       UnPixel, NULL,
				   ViewFrameTable[frame - 1].FrMagnification);
			  yctrlstart = topY + PixelValue (pPaStart->YCtrlStart,
						      UnPixel, NULL,
				   ViewFrameTable[frame - 1].FrMagnification);
			  
			  DrawBezierControl (frame, thick, xstart, ystart,
					     xctrlstart, yctrlstart, bg, fg);
			}
		      else if(pointselect == i_start ||
			      pointselect == i_start+1)
			{
			  /* First point of the subpath is active, draw the
			   Bezier control at the end */
			  xend = leftX + PixelValue (pPa->XEnd,
						     UnPixel, NULL,
				   ViewFrameTable[frame - 1].FrMagnification);
			  yend = topY + PixelValue (pPa->YEnd,
						      UnPixel, NULL,
				   ViewFrameTable[frame - 1].FrMagnification);
			  xctrlend = leftX + PixelValue (pPa->XCtrlEnd,
							 UnPixel, NULL,
				   ViewFrameTable[frame - 1].FrMagnification);
			  yctrlend = topY + PixelValue (pPa->YCtrlEnd,
							UnPixel, NULL,
				   ViewFrameTable[frame - 1].FrMagnification);
			  
			  DrawBezierControl (frame, thick, xend, yend,
					     xctrlend, yctrlend, bg, fg);

			}
		    }
		}

              pPa = pPa->PaNext;
	      i++;
            }
        }
      else if (pointselect != 0)
        /* Keep in mind the selected caracteristic point       */
        /*            1-------------2-------------3            */
        /*            |                           |            */
        /*            |                           |            */
        /*            8                           4            */
        /*            |                           |            */
        /*            |                           |            */
        /*            7-------------6-------------5            */
        switch (pointselect)
          {
          case 1:
            DrawRectangle (frame, 0, 0, leftX, topY, thick, thick,
                           fg, bg, 2);
            break;
          case 2:
            DrawRectangle (frame, 0, 0, middleX, topY, thick, thick,
                           fg, bg, 2);
            break;
          case 3:
            DrawRectangle (frame, 0, 0, rightX, topY, thick, thick,
                           fg, bg, 2);
            break;
          case 4:
            DrawRectangle (frame, 0, 0, rightX, middleY, thick, thick,
                           fg, bg, 2);
            break;
          case 5:
            DrawRectangle (frame, 0, 0, rightX, bottomY, thick, thick,
                           fg, bg, 2);
            break;
          case 6:
            DrawRectangle (frame, 0, 0, middleX, bottomY, thick, thick,
                           fg, bg, 2);
            break;
          case 7:
            DrawRectangle (frame, 0, 0, leftX, bottomY, thick, thick,
                           fg, bg, 2);
            break;
          case 8:
            DrawRectangle (frame, 0, 0, leftX, middleY, thick, thick,
                           fg, bg, 2);
            break;
          }
      else if (pAb->AbLeafType == LtGraphics && pAb->AbVolume != 0)
        /* C'est une boite graphique */
        /* On marque les points caracteristiques de la boite */
        switch (pAb->AbRealShape)
          {
          case SPACE:
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
            /* 8 control points */
            DrawRectangle (frame, 0, 0, leftX, topY, thick, thick,
                           fg, bg, 2);
            DrawRectangle (frame, 0, 0, middleX, topY, thick, thick,
                           fg, bg, 2);
            DrawRectangle (frame, 0, 0, rightX, topY, thick, thick,
                           fg, bg, 2);
            DrawRectangle (frame, 0, 0, leftX, middleY, thick, thick,
                           fg, bg, 2);
            DrawRectangle (frame, 0, 0, rightX, middleY, thick, thick,
                           fg, bg, 2);
            DrawRectangle (frame, 0, 0, leftX, bottomY, thick, thick,
                           fg, bg, 2);
            DrawRectangle (frame, 0, 0, middleX, bottomY, thick, thick,
                           fg, bg, 2);
            DrawRectangle (frame, 0, 0, rightX, bottomY, thick, thick,
                           fg, bg, 2);
            break;
          case 'C':
          case 'L':
          case 'a':
          case 'c':
          case 'P':
          case 'Q':
            /* 4 control points */
            DrawRectangle (frame, 0, 0, middleX, topY, thick, thick,
                           fg, bg, 2);
            DrawRectangle (frame, 0, 0, leftX, middleY, thick, thick,
                           fg, bg, 2);
            DrawRectangle (frame, 0, 0, rightX, middleY, thick, thick,
                           fg, bg, 2);
            DrawRectangle (frame, 0, 0, middleX, bottomY, thick, thick,
                           fg, bg, 2);
            if (pAb->AbRealShape == 'C' && pAb->AbRx == 0 && pAb->AbRy == 0)
              /* rounded corners are not round. display a control point
                 for each corner */
              {
                DrawRectangle (frame, 0, 0, leftX, topY, thick, thick,
                               fg, bg, 2);
                DrawRectangle (frame, 0, 0, rightX, topY, thick, thick,
                               fg, bg, 2);
                DrawRectangle (frame, 0, 0, leftX, bottomY, thick, thick,
                               fg, bg, 2);
                DrawRectangle (frame, 0, 0, rightX, bottomY, thick, thick,
                               fg, bg, 2);
              }
            break;
          case 'W':
            /* 3 control points */
            DrawRectangle (frame, 0, 0, leftX, topY, thick, thick,
                           fg, bg, 2);
            DrawRectangle (frame, 0, 0, rightX, topY, thick, thick,
                           fg, bg, 2);
            DrawRectangle (frame, 0, 0, rightX, bottomY, thick, thick,
                           fg, bg, 2);
            break;
          case 'X':
            /* 3 control points */
            DrawRectangle (frame, 0, 0, rightX, topY, thick, thick,
                           fg, bg, 2);
            DrawRectangle (frame, 0, 0, leftX, bottomY, thick, thick,
                           fg, bg, 2);
            DrawRectangle (frame, 0, 0, rightX, bottomY, thick, thick,
                           fg, bg, 2);
            break;
          case 'Y':
            /* 3 control points */
            DrawRectangle (frame, 0, 0, leftX, topY, thick, thick,
                           fg, bg, 2);
            DrawRectangle (frame, 0, 0, leftX, bottomY, thick, thick,
                           fg, bg, 2);
            DrawRectangle (frame, 0, 0, rightX, bottomY, thick, thick,
                           fg, bg, 2);
            break;
          case 'Z':
            /* 3 control points */
            DrawRectangle (frame, 0, 0, leftX, topY, thick, thick,
                           fg, bg, 2);
            DrawRectangle (frame, 0, 0, rightX, topY, thick, thick,
                           fg, bg, 2);
            DrawRectangle (frame, 0, 0, rightX, bottomY, thick, thick,
                           fg, bg, 2);
            break;
	    
          case 'h':
          case '<':
          case '>':
            /* 2 control points */
            DrawRectangle (frame, 0, 0, leftX, middleY, thick, thick,
                           fg, bg, 2);
            DrawRectangle (frame, 0, 0, rightX, middleY, thick, thick,
                           fg, bg, 2);
            break;
          case 't':
            /* 3 control points */
            DrawRectangle (frame, 0, 0, leftX, topY, thick, thick,
                           fg, bg, 2);
            DrawRectangle (frame, 0, 0, middleX, topY, thick, thick,
                           fg, bg, 2);
            DrawRectangle (frame, 0, 0, rightX, topY, thick, thick,
                           fg, bg, 2);
            break;
          case 'b':
            /* 3 control points */
            DrawRectangle (frame, 0, 0, leftX, bottomY, thick, thick,
                           fg, bg, 2);
            DrawRectangle (frame, 0, 0, middleX, bottomY, thick, thick,
                           fg, bg, 2);
            DrawRectangle (frame, 0, 0, rightX, bottomY, thick, thick,
                           fg, bg, 2);
            break;
          case 'v':
          case '^':
          case 'V':
            /* 2 control points */
            DrawRectangle (frame, 0, 0, middleX, topY, thick, thick,
                           fg, bg, 2);
            DrawRectangle (frame, 0, 0, middleX, bottomY, thick, thick,
                           fg, bg, 2);
            break;
          case 'l':
            /* 3 control points */
            DrawRectangle (frame, 0, 0, leftX, topY, thick, thick,
                           fg, bg, 2);
            DrawRectangle (frame, 0, 0, leftX, middleY, thick, thick,
                           fg, bg, 2);
            DrawRectangle (frame, 0, 0, leftX, bottomY, thick, thick,
                           fg, bg, 2);
            break;
          case 'r':
            /* 3 control points */
            DrawRectangle (frame, 0, 0, rightX, topY, thick, thick,
                           fg, bg, 2);
            DrawRectangle (frame, 0, 0, rightX, middleY, thick, thick,
                           fg, bg, 2);
            DrawRectangle (frame, 0, 0, rightX, bottomY, thick, thick,
                           fg, bg, 2);
            break;
          case '\\':
          case 'O':
          case 'e':
            /* 2 control points */
            DrawRectangle (frame, 0, 0, leftX, topY, thick, thick,
                           fg, bg, 2);
            DrawRectangle (frame, 0, 0, rightX, bottomY, thick, thick,
                           fg, bg, 2);
            break;
          case '/':
          case 'o':
          case 'E':
            /* 2 control points */
            DrawRectangle (frame, 0, 0, rightX, topY, thick, thick,
                           fg, bg, 2);
            DrawRectangle (frame, 0, 0, leftX, bottomY, thick, thick,
                           fg, bg, 2);
            break;
          case 'g':
            /* Coords of the line are given by the enclosing box */
            pAb = pAb->AbEnclosing;
            if ((pAb->AbHorizPos.PosEdge == Left &&
                 pAb->AbVertPos.PosEdge == Top) ||
                (pAb->AbHorizPos.PosEdge == Right &&
                 pAb->AbVertPos.PosEdge == Bottom))
              {
                /* draw a \ */
                /* 2 control points */
                DrawRectangle (frame, 0, 0, leftX, topY, thick, thick,
                               fg, bg, 2);
                DrawRectangle (frame, 0, 0, rightX, bottomY, thick, thick,
                               fg, bg, 2);
              }
            else
              {
                /* draw a / */
                /* 2 control points */
                DrawRectangle (frame, 0, 0, rightX, topY, thick, thick,
                               fg, bg, 2);
                DrawRectangle (frame, 0, 0, leftX, bottomY, thick, thick,
                               fg, bg, 2);
              }
            break;
          default:
            break;
          }
    }
}


/*----------------------------------------------------------------------
  DisplayBgBoxSelection paints the box background with the selection
  color.
  ----------------------------------------------------------------------*/
void DisplayBgBoxSelection (int frame, PtrBox pBox)
{
  PtrBox              pChildBox;
  ViewFrame          *pFrame;
  PtrAbstractBox      pAb;
  PtrElement          pEl;
  int                 leftX, topY;

  if (pBox && pBox->BxAbstractBox)
    {
      pFrame = &ViewFrameTable[frame - 1];
      pAb = pBox->BxAbstractBox;
      pEl = pAb->AbElement;
      if (pBox->BxType == BoSplit || pBox->BxType == BoMulScript)
        {
          /* display the selection on pieces of the current box */
          pChildBox = pBox->BxNexChild;
          while (pChildBox != NULL)
            {
              DisplayBgBoxSelection (frame, pChildBox);
              pChildBox = pChildBox->BxNexChild;
	    }
        }
      else if (FrameTable[frame].FrView == 1 && // formatted view
               (pAb->AbLeafType == LtPicture ||
                pAb->AbLeafType == LtGraphics ||
                pAb->AbLeafType == LtPath ||
                pAb->AbLeafType == LtPolyLine ||
                TypeHasException (ExcIsImg, pEl->ElTypeNumber, pEl->ElStructSchema)))
        DisplayPointSelection (frame, pBox, 0, FALSE);
      else
        {
          /* the whole box is selected */
          leftX = pBox->BxXOrg - pFrame->FrXOrg;
          if (pBox->BxLMargin < 0)
            leftX += pBox->BxLMargin;
          topY = pBox->BxYOrg - pFrame->FrYOrg;
          if (pBox->BxTMargin < 0)
            topY += pBox->BxTMargin;
          /* draw the background of the selection */
          DrawRectangle (frame, 0, 0, leftX, topY,
                         pBox->BxWidth, pBox->BxHeight,
                         0, BgSelColor, 2);
        }
    }
  
}


/*----------------------------------------------------------------------
  DrawBoxSelection paints the box with the selection background.
  ----------------------------------------------------------------------*/
void DrawBoxSelection (int frame, PtrBox pBox)
{
  PtrBox              pChildBox;

  if (pBox)
    {
      if (pBox->BxType == BoSplit || pBox->BxType == BoMulScript)
        {
          /* display the selection on pieces of the current box */
          pChildBox = pBox->BxNexChild;
          while (pChildBox)
            {
              DrawBoxSelection (frame, pChildBox);
              pChildBox = pChildBox->BxNexChild;
            }
        }
      else
        /* display other elements */
        DefBoxRegion (frame, pBox, -1, -1, -1, -1);
    }
}

/*----------------------------------------------------------------------
  SetNewSelectionStatus goes through the tree for switching the selection
  indicator.
  ----------------------------------------------------------------------*/
void SetNewSelectionStatus (int frame, PtrAbstractBox pAb, ThotBool status)
{
  PtrAbstractBox      pChildAb;
  ViewFrame          *pFrame;

  if (pAb != NULL)
    {
      if (pAb->AbSelected)
        {
          /* the abstract box is selected */
          pFrame = &ViewFrameTable[frame - 1];
          pAb->AbSelected = status;
          /* doesn't display selection limits */
          if (pFrame->FrSelectionBegin.VsBox == NULL ||
              pFrame->FrSelectionEnd.VsBox == NULL ||
              pAb->AbLeafType == LtCompound ||
              (pAb != pFrame->FrSelectionBegin.VsBox->BxAbstractBox &&
               pAb != pFrame->FrSelectionEnd.VsBox->BxAbstractBox))
            if (pAb->AbBox)
              DrawBoxSelection (frame, pAb->AbBox);
        }
      else if (pAb->AbLeafType == LtCompound)
        /* check the subtree */
        {
          pChildAb = pAb->AbFirstEnclosed;
          while (pChildAb != NULL)
            {
              SetNewSelectionStatus (frame, pChildAb, status);
              pChildAb = pChildAb->AbNext;
            }
        }
    }
}

/*----------------------------------------------------------------------
  DisplayStringSelection the selection on a substring of text
  between leftX and rightX.
  The parameter t gives the top extra margin of the box.
  ----------------------------------------------------------------------*/
void DisplayStringSelection (int frame, int leftX, int rightX, int t,
                             PtrBox pBox)
{
  PtrBox              pParentBox;
  ViewFrame          *pFrame;
  PtrAbstractBox      pAb;
  int                 width, height;
  int                 topY, h, col, l;

  pFrame = &ViewFrameTable[frame - 1];
  if (leftX > rightX)
    {
      /* echange limits */
      l = leftX;
      leftX = rightX;
      rightX = l;
    }
  if (pBox->BxAbstractBox != NULL)
    {
      topY = pBox->BxYOrg + t + pBox->BxTMargin + pBox->BxTBorder +
        pBox->BxTPadding - pFrame->FrYOrg;
      h = pBox->BxH;

      /* limit to the scrolling zone */
      width = FrameTable[frame].FrScrollOrg + FrameTable[frame].FrScrollWidth
	      - pFrame->FrXOrg;
      /* and clipped by the enclosing box */
      pAb = pBox->BxAbstractBox->AbEnclosing;
      if (pAb && pAb->AbElement &&
          !TypeHasException (ExcNoShowBox, pAb->AbElement->ElTypeNumber,
                             pAb->AbElement->ElStructSchema))
        {
          /* holophrasted elements have no enclosing */
          pParentBox = pBox->BxAbstractBox->AbEnclosing->AbBox;
          while (pParentBox->BxType == BoGhost ||
                 pParentBox->BxType == BoFloatGhost)
            {
              pAb = pParentBox->BxAbstractBox;
              if (pAb->AbEnclosing == NULL)
                pParentBox = pBox;
              else
                pParentBox = pAb->AbEnclosing->AbBox;
            }
          height = pParentBox->BxYOrg + pParentBox->BxHeight - pFrame->FrYOrg;
          /* don't take into account margins and borders */
          if (topY > height)
            h = 0;
          else if (topY + h > height)
            h = height - topY;
        }

      /* don't take into account margins and borders */
      l = pBox->BxXOrg + pBox->BxLMargin + pBox->BxLBorder + pBox->BxLPadding;
      leftX = leftX + l - pFrame->FrXOrg;
      if (leftX > width)
        width = 0;
      else
        {
          rightX = rightX + l - pFrame->FrXOrg;
          if (rightX > width)
            width -= leftX;
          else
            width = rightX - leftX;
        }
      col = BgSelColor;
      DrawRectangle (frame, 0, 0, leftX, topY, width, h, 0, col, 2);
    }
}
