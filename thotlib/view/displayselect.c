/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2009
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

#include "boxrelations_f.h"
#include "boxpositions_f.h"
#include "displayselect_f.h"
#include "exceptions_f.h"
#include "font_f.h"
#include "frame_f.h"
#include "units_f.h"
#include "windowdisplay_f.h"


#ifdef _GL
#include "glwindowdisplay.h"
#endif /*_GL*/



/* Type of handles */
enum handle_type
  {
    DEFAULT_HANDLE,
    FRAME_HANDLE,
    BEZIER_HANDLE,
    RESIZE_HANDLE,
    ROUNDED_HANDLE
  };

enum direction
  {
    N,
    W,
    S,
    E,
    NW,
    NE,
    SW,
    SE
  };

/*----------------------------------------------------------------------
  DrawHandle
  ----------------------------------------------------------------------*/
void DrawHandle(int handle, int frame, int thick, ...)
{
  int x1, y1, x2, y2;
  int dir;
  int red = TtaGetThotColor(255, 0, 0);
  int blue = TtaGetThotColor(0, 0, 255);

  va_list varpos;
  va_start(varpos, thick);

  switch(handle)
    {
    case RESIZE_HANDLE:
      x1 = va_arg(varpos, int);
      y1 = va_arg(varpos, int);
      dir = va_arg(varpos, int);
      DrawResizeTriangle (frame, thick, x1, y1,
			  blue, ResizeFgSelColor, dir);

      break;

    case ROUNDED_HANDLE:
      x1 = va_arg(varpos, int);
      y1 = va_arg(varpos, int);
      DrawEllips (frame, 1, 0,
		  x1 - thick/2,
		  y1 - thick/2,
		  thick, thick,
		  ResizeFgSelColor,
		  red,
		  2
		  );
      break;

    case FRAME_HANDLE:
      x1 = va_arg(varpos, int);
      y1 = va_arg(varpos, int);
      DrawRectangle (frame, 1, 0,
		     x1 - thick/2,
		     y1 - thick/2,
		     thick, thick,
		     ResizeFgSelColor,
		     ResizeBgSelColor,
		     2
		     );
      break;

    case DEFAULT_HANDLE:
      x1 = va_arg(varpos, int);
      y1 = va_arg(varpos, int);
      DrawRectangle (frame, 1, 0,
		     x1 - thick/2,
		     y1 - thick/2,
		     thick, thick,
		     FgSelColor,
		     BgSelColor,
		     2
		     );
      break;

    case BEZIER_HANDLE:
      x1 = va_arg(varpos, int);
      y1 = va_arg(varpos, int);
      x2 = va_arg(varpos, int);
      y2 = va_arg(varpos, int);
      DrawBezierControl (frame, thick,
			 x1, y1, x2, y2, ResizeBgSelColor, ResizeFgSelColor);
      break;

    }

  va_end(varpos);
}


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
  PtrPathSeg          pPa, pPaStart = NULL;
  PtrElement          pEl;
  int                 leftX, middleX, rightX;
  int                 topY, middleY, bottomY;
  int                 t, b, l, r;
  int                 thick;
  int                 i, j;
  int                 x, y;
  int                 rx, ry;
  int                 xstart, ystart, xctrlstart, yctrlstart;
  int                 xend, yend, xctrlend, yctrlend;
  ThotBool            svg_or_img;
  int h;
  int i_start;

  if (pBox)
    {
      pFrame = &ViewFrameTable[frame - 1];
      pAb = pBox->BxAbstractBox;
      if (pAb->AbPresentationBox)
        /* don't display handles of presentation boxes */
        return;

      pEl = pAb->AbElement;
      thick = HANDLE_WIDTH;
      svg_or_img = (pAb->AbLeafType == LtPicture ||
                    (pEl && !pAb->AbPresentationBox &&
                     TypeHasException (ExcIsDraw, pEl->ElTypeNumber, pEl->ElStructSchema)));

      if (could_resize && svg_or_img)
	/* display larger handles */
        thick += 2;

      /* The box is smaller than the handle */
      if (pAb->AbLeafType != LtGraphics || pAb->AbRealShape != 'g')
        {
          if (thick > pBox->BxW)
            thick = pBox->BxW;
          if (thick > pBox->BxH)
            thick = pBox->BxH;
        }

      /* selection points */
      GetExtraMargins (pBox, frame, TRUE, &t, &b, &l, &r);
      if (pEl->ElSystemOrigin)
        {
          leftX = pBox->BxLMargin + pBox->BxLBorder;
          topY = pBox->BxTMargin + pBox->BxTBorder;
        }
      else
        {
          leftX = l + pBox->BxXOrg + pBox->BxLMargin + pBox->BxLBorder
            + pBox->BxLPadding - pFrame->FrXOrg;
          topY = t + pBox->BxYOrg + pBox->BxTMargin + pBox->BxTBorder
            + pBox->BxTPadding - pFrame->FrYOrg;
        }
      bottomY = topY + pBox->BxH - 1;
      rightX = leftX + pBox->BxW - 1;
      middleX = leftX + (pBox->BxW / 2);
      middleY = topY + (pBox->BxH / 2);

      if(could_resize)
        h = FRAME_HANDLE;
      else
        h = DEFAULT_HANDLE;

      if (svg_or_img)
        {
          /* Frame */
          DrawRectangle (frame, 1, 0, leftX, topY, pBox->BxW, pBox->BxH - 1,
                         ResizeFgSelColor, 0, 0);

          /* 8 control points */
          DrawHandle(h, frame, thick, leftX, topY);
          DrawHandle(h, frame, thick, middleX, topY);
          DrawHandle(h, frame, thick, rightX, topY);
          DrawHandle(h, frame, thick, leftX, middleY);
          DrawHandle(h, frame, thick, rightX, middleY);
          DrawHandle(h, frame, thick, leftX, bottomY);
          DrawHandle(h, frame, thick, middleX, bottomY);
          DrawHandle(h, frame, thick, rightX, bottomY);
        }
      else if (pAb->AbLeafType == LtPolyLine && pBox->BxNChars > 1)
        {
          /* Draw control points of the polyline */
          /* if there is at least one point in the polyline */
          pBuffer = pBox->BxBuffer;
          leftX = pBox->BxXOrg - pFrame->FrXOrg;
          topY = pBox->BxYOrg - pFrame->FrYOrg;
          j = 1;
          for (i = 1; pBuffer; i++)
            {
                  x = leftX + PixelValue (pBuffer->BuPoints[j].XCoord,
                                          UnPixel, NULL,
                                          ViewFrameTable[frame - 1].FrMagnification);
                  y = topY + PixelValue (pBuffer->BuPoints[j].YCoord,
                                         UnPixel, NULL,
                                         ViewFrameTable[frame - 1].FrMagnification);

              if (pointselect == i)
		DrawHandle(FRAME_HANDLE, frame, thick+2, x, y);
	      else
		DrawHandle(FRAME_HANDLE, frame, thick, x, y);
	      
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

                  if(
		     pointselect == i || /* Current point selected */

		     (pointselect == i+1 && /* Next control point selected*/
		       (pPa->PaShape == PtCubicBezier ||
			pPa->PaShape == PtQuadraticBezier))

		     )

		    /* draw the start point of this path segment */
		    DrawHandle(FRAME_HANDLE, frame, thick+2, xstart, ystart);
		  else
		    DrawHandle(FRAME_HANDLE, frame, thick, xstart, ystart);

		  i++;
                }

	      if(pPa->PaShape == PtCubicBezier ||
		 pPa->PaShape == PtQuadraticBezier)
		{
		  /* Check if we draw Bezier Control */
		  if(pointselect > 0 &&
		     /*
		       i-2     i-1     i   
		       O------x-------0

		      */
		     (pointselect == i-1 || /* A point is selected */
		     pointselect == i   || /* Current control point selected */

		     (pointselect == i-2 &&/* Previous control point selected */
		      !(pPa->PaNewSubpath) &&
		       pPa->PaPrevious &&
		       (pPa->PaPrevious->PaShape == PtCubicBezier ||
			pPa->PaPrevious->PaShape == PtQuadraticBezier)
		      ))

		     )
		    DrawHandle(BEZIER_HANDLE, frame, thick, xstart, ystart,
			       xctrlstart, yctrlstart);
		  
		  i++;


		  /* Check if we draw Bezier Control */
		  if(pointselect > 0 &&
		     /*
		       i     i+1     i+2
		       O------x-------0

		      */

		     (pointselect == i+1 || /* A point is selected */
		     pointselect == i   || /* Current control point selected */
		     
		     (pointselect == i+2 && /* Next control point selected */
		      pPa->PaNext && !(pPa->PaNext->PaNewSubpath)
		      && (pPa->PaNext->PaShape == PtCubicBezier ||
			  pPa->PaNext->PaShape == PtQuadraticBezier)  )
		      ))
		    DrawHandle(BEZIER_HANDLE, frame, thick, xend, yend,
				       xctrlend, yctrlend);
		  
		  i++;
		}

	      if(
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
		DrawHandle(FRAME_HANDLE, frame, thick+2, xend, yend);
	      else
		DrawHandle(FRAME_HANDLE, frame, thick, xend, yend);
	
	      if(pointselect > 0 && pPaStart && (!pPa->PaNext || pPa->PaNext->PaNewSubpath))
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

			  DrawHandle(BEZIER_HANDLE, frame, thick,
				     xstart, ystart, xctrlstart, yctrlstart);
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

			  DrawHandle(BEZIER_HANDLE, frame, thick, xend, yend,
				     xctrlend, yctrlend);

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
	    DrawHandle(h, frame, thick, leftX, topY);
            break;
          case 2:
	    DrawHandle(h, frame, thick, middleX, topY);
            break;
          case 3:
	    DrawHandle(h, frame, thick, rightX, topY);
            break;
          case 4:
	    DrawHandle(h, frame, thick, rightX, middleY);
            break;
          case 5:
	    DrawHandle(h, frame, thick, rightX, bottomY);
            break;
          case 6:
	    DrawHandle(h, frame, thick, middleX, bottomY);
            break;
          case 7:
	    DrawHandle(h, frame, thick, leftX, bottomY);
            break;
          case 8:
	    DrawHandle(h, frame, thick, leftX, middleY);
            break;
          }
      else if (pAb->AbLeafType == LtGraphics && pAb->AbVolume != 0)
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
	    DrawHandle(h, frame, thick, leftX, topY);
	    DrawHandle(h, frame, thick, middleX, topY);
	    DrawHandle(h, frame, thick, rightX, topY);
	    DrawHandle(h, frame, thick, leftX, middleY);
	    DrawHandle(h, frame, thick, rightX, middleY);
	    DrawHandle(h, frame, thick, leftX, bottomY);
	    DrawHandle(h, frame, thick, middleX, bottomY);
	    DrawHandle(h, frame, thick, rightX, bottomY);
            break;
          case 'P':
          case 'Q':
            /* 4 control points */
	    DrawHandle(h, frame, thick, middleX, topY);
	    DrawHandle(h, frame, thick, leftX, middleY);
	    DrawHandle(h, frame, thick, rightX, middleY);
	    DrawHandle(h, frame, thick, middleX, bottomY);
	    break;

	  case 2: /* Parallelogram */
	    thick+=2;
	    rx = pBox->BxRx;
	    DrawHandle(ROUNDED_HANDLE, frame, thick, leftX+rx, topY);
	    DrawHandle(h, frame, thick, rightX, topY);
	    DrawHandle(ROUNDED_HANDLE, frame, thick, rightX - rx, bottomY);
	    DrawHandle(h, frame, thick, leftX, bottomY);

	    /* resize handle */
	    if(rx > 0)
	      {
		DrawHandle(RESIZE_HANDLE, frame, thick, leftX, topY, NW);
		DrawHandle(RESIZE_HANDLE, frame, thick, rightX, bottomY, SE);
	      }

	    DrawHandle(RESIZE_HANDLE, frame, thick, middleX, topY, N);
	    DrawHandle(RESIZE_HANDLE, frame, thick, middleX, bottomY, S);
	    DrawHandle(RESIZE_HANDLE, frame, thick, leftX, middleY, W);
	    DrawHandle(RESIZE_HANDLE, frame, thick, rightX, middleY, E);

	    break;

	  case 3: /* Trapezium */
	    thick+=2;
	    rx = pBox->BxRx;
	    ry = pBox->BxRy;

	    DrawHandle(RESIZE_HANDLE, frame, thick, middleX, topY, N);
	    DrawHandle(RESIZE_HANDLE, frame, thick, middleX, bottomY, S);
	    DrawHandle(RESIZE_HANDLE, frame, thick, leftX, middleY, W);
	    DrawHandle(RESIZE_HANDLE, frame, thick, rightX, middleY, E);

	    if(rx < 0)
	      {
		rx=-rx;
		DrawHandle(RESIZE_HANDLE, frame, thick, leftX, bottomY, SW);
		DrawHandle(h, frame, thick, leftX+rx, bottomY);
		DrawHandle(h, frame, thick, leftX, topY);
	      }
	    else
	      {
		DrawHandle(RESIZE_HANDLE, frame, thick, leftX, topY, NW);
		DrawHandle(h, frame, thick, leftX+rx, topY);
		DrawHandle(h, frame, thick, leftX, bottomY);
	      }

	    if(ry < 0)
	      {
		ry=-ry;
		DrawHandle(RESIZE_HANDLE, frame, thick, rightX, topY, NE);
		DrawHandle(h, frame, thick, rightX-ry, topY);
		DrawHandle(h, frame, thick, rightX, bottomY);
	      }
	    else
	      {
		DrawHandle(RESIZE_HANDLE, frame, thick, rightX, bottomY, SE);
		DrawHandle(h, frame, thick, rightX-ry, bottomY);
		DrawHandle(h, frame, thick, rightX, topY);
	      }
	    break;

	  case 4: /* Equilateral triangle */
	  case 5: /* Isosceles triangle */
	    thick+=2;
            /* control points */
	    DrawHandle(h, frame, thick, middleX, topY);
	    DrawHandle(h, frame, thick, leftX, bottomY);
	    DrawHandle(h, frame, thick, rightX, bottomY);
	    
	    /* resize handle */
	    DrawHandle(RESIZE_HANDLE, frame, thick, leftX, topY, NW);
	    DrawHandle(RESIZE_HANDLE, frame, thick, rightX, topY, NE);
	    DrawHandle(RESIZE_HANDLE, frame, thick, middleX, bottomY, S);
	    DrawHandle(RESIZE_HANDLE, frame, thick, leftX, middleY, W);
	    DrawHandle(RESIZE_HANDLE, frame, thick, rightX, middleY, E);
	    break;

	  case 6: /* rectangle triangle */
	    thick+=2;
            /* control points */
	    DrawHandle(h, frame, thick, leftX, topY);
	    DrawHandle(h, frame, thick, leftX, bottomY);
	    DrawHandle(h, frame, thick, rightX, topY);

	    /* resize handle */
	    DrawHandle(RESIZE_HANDLE, frame, thick, middleX, topY, N);
	    DrawHandle(RESIZE_HANDLE, frame, thick, leftX, middleY, W);
	    DrawHandle(RESIZE_HANDLE, frame, thick, middleX, middleY, SE);
	    break;

	  case 7: /* square */
          case 8: /* rectangle */
	    thick+=2;
	    DrawHandle(h, frame, thick, middleX, topY);
	    DrawHandle(h, frame, thick, leftX, middleY);
	    DrawHandle(h, frame, thick, middleX, bottomY);
	    DrawHandle(h, frame, thick, rightX, middleY);
	    DrawHandle(h, frame, thick, leftX, topY);
	    DrawHandle(h, frame, thick, leftX, bottomY);
	    DrawHandle(h, frame, thick, rightX, topY);
	    DrawHandle(h, frame, thick, rightX, bottomY);
	    break;

          case 'L': /* diamond */
	    thick+=2;
	    DrawHandle(h, frame, thick, middleX, topY);
	    DrawHandle(h, frame, thick, leftX, middleY);
	    DrawHandle(h, frame, thick, middleX, bottomY);
	    DrawHandle(h, frame, thick, rightX, middleY);

	    DrawHandle(RESIZE_HANDLE, frame, thick, leftX, topY, NW);
	    DrawHandle(RESIZE_HANDLE, frame, thick, leftX, bottomY, SW);
	    DrawHandle(RESIZE_HANDLE, frame, thick, rightX, topY, NE);
	    DrawHandle(RESIZE_HANDLE, frame, thick, rightX, bottomY, SE);
	    break;

	  case 1: /* square */
          case 'C': /* <rect/> */
	    thick+=2;
	    rx = pBox->BxRx;
	    ry = pBox->BxRy;
	    if(ry == -1)ry = rx;
	    else if(rx == -1)rx = ry; 
    
	    DrawHandle(h, frame, thick, middleX, topY);
	    DrawHandle(h, frame, thick, leftX, middleY);
	    DrawHandle(h, frame, thick, middleX, bottomY);
	    DrawHandle(h, frame, thick, rightX, middleY);

	    if(rx == 0 || ry == 0)
	      {
		DrawHandle(h, frame, thick, leftX, topY);
		DrawHandle(h, frame, thick, leftX, bottomY);
		DrawHandle(h, frame, thick, rightX, topY);
		DrawHandle(h, frame, thick, rightX, bottomY);
	      }
	    else
	      {
		/* Rounded rectangle */
		DrawHandle(RESIZE_HANDLE, frame, thick, leftX, topY, NW);
		DrawHandle(RESIZE_HANDLE, frame, thick, leftX, bottomY, SW);
		DrawHandle(RESIZE_HANDLE, frame, thick, rightX, topY, NE);
		DrawHandle(RESIZE_HANDLE, frame, thick, rightX, bottomY, SE);
	      }

	    /* 2 radius handles */
	    DrawHandle(ROUNDED_HANDLE, frame, thick, rightX, topY + ry);
	    DrawHandle(ROUNDED_HANDLE, frame, thick, rightX - rx, topY);
            break;

          case 'a': /* <circle/> */
          case 'c': /* <ellipse/> */
	    thick+=2;
	    DrawHandle(h, frame, thick, middleX, topY);
	    DrawHandle(h, frame, thick, leftX, middleY);
	    DrawHandle(h, frame, thick, middleX, bottomY);
	    DrawHandle(h, frame, thick, rightX, middleY);
	    DrawHandle(RESIZE_HANDLE, frame, thick, leftX, topY, NW);
	    DrawHandle(RESIZE_HANDLE, frame, thick, leftX, bottomY, SW);
	    DrawHandle(RESIZE_HANDLE, frame, thick, rightX, topY, NE);
	    DrawHandle(RESIZE_HANDLE, frame, thick, rightX, bottomY, SE);

	    /*DrawHandle(ROUNDED_HANDLE, frame, thick, rightX, middleY);
	      DrawHandle(ROUNDED_HANDLE, frame, thick, middleX, topY);*/
	    break;

          case 'W':
            /* 3 control points */
	    DrawHandle(h, frame, thick, leftX, topY);
	    DrawHandle(h, frame, thick, rightX, topY);
	    DrawHandle(h, frame, thick, leftX, bottomY);
            break;
          case 'X':
            /* 3 control points */
            DrawHandle(h, frame, thick, rightX, topY);
            DrawHandle(h, frame, thick, leftX, bottomY);
            DrawHandle(h, frame, thick, rightX, bottomY);
            break;
          case 'Y':
            /* 3 control points */
	    DrawHandle(h, frame, thick, leftX, topY);
            DrawHandle(h, frame, thick, leftX, bottomY);
            DrawHandle(h, frame, thick, rightX, bottomY);
            break;
          case 'Z':
            /* 3 control points */
            DrawHandle(h, frame, thick, leftX, topY);
            DrawHandle(h, frame, thick, rightX, topY);
            DrawHandle(h, frame, thick, rightX, bottomY);
            break;
	    
          case 'h':
          case '<':
          case '>':
            /* 2 control points */
            DrawHandle(h, frame, thick, leftX, middleY);
	    DrawHandle(h, frame, thick, rightX, middleY);
            break;
          case 't':
            /* 3 control points */
            DrawHandle(h, frame, thick, leftX, topY);
            DrawHandle(h, frame, thick, middleX, topY);
            DrawHandle(h, frame, thick, rightX, topY);
            break;
          case 'b':
            /* 3 control points */
            DrawHandle(h, frame, thick, leftX, bottomY);
            DrawHandle(h, frame, thick, middleX, bottomY);
            DrawHandle(h, frame, thick, rightX, bottomY);
            break;
          case 'v':
          case '^':
          case 'V':
            /* 2 control points */
            DrawHandle(h, frame, thick, middleX, topY);
	    DrawHandle(h, frame, thick, middleX, bottomY);
            break;
          case 'l':
            /* 3 control points */
            DrawHandle(h, frame, thick, leftX, topY);
	    DrawHandle(h, frame, thick, leftX, middleY);
            DrawHandle(h, frame, thick, leftX, bottomY);
            break;
          case 'r':
            /* 3 control points */
            DrawHandle(h, frame, thick, rightX, topY);
            DrawHandle(h, frame, thick, rightX, middleY);
            DrawHandle(h, frame, thick, rightX, bottomY);
            break;
          case '\\':
          case 'O':
          case 'e':
            /* 2 control points */
            DrawHandle(h, frame, thick, leftX, topY);
            DrawHandle(h, frame, thick, rightX, bottomY);
            break;
          case '/':
          case 'o':
          case 'E':
            /* 2 control points */
            DrawHandle(h, frame, thick, rightX, topY);
            DrawHandle(h, frame, thick, leftX, bottomY);
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
		DrawHandle(h, frame, thick, leftX, topY);
		DrawHandle(h, frame, thick, rightX, bottomY);
              }
            else
              {
                /* draw a / */
                /* 2 control points */
		DrawHandle(h, frame, thick, rightX, topY);
		DrawHandle(h, frame, thick, leftX, bottomY);
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

  if (pAb && pAb->AbElement && pAb->AbElement->ElStructSchema)
    {
      if (pAb->AbDocView == 1 &&
          TypeHasException (ExcIsMarker, pAb->AbElement->ElTypeNumber,
                            pAb->AbElement->ElStructSchema))
        // don't select markers in the formatted view
        status = FALSE;
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
          if (pAb->AbDocView == 1 && status == TRUE &&
              TypeHasException (ExcIsGroup, pAb->AbElement->ElTypeNumber,
                                pAb->AbElement->ElStructSchema))
            // don't select children of a selected group in the formatted view
            status = FALSE;
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
      topY = pBox->BxYOrg - pFrame->FrYOrg /*+t*/;
      //topY += pBox->BxTMargin + pBox->BxTBorder;
      h = pBox->BxHeight;

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
                 pParentBox->BxType == BoStructGhost ||
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
      l = pBox->BxXOrg;
      if (!pBox->BxAbstractBox->AbPresentationBox)
        l += pBox->BxLMargin + pBox->BxLBorder + pBox->BxLPadding;
      leftX = leftX + l - pFrame->FrXOrg;
      if (leftX > pBox->BxXOrg + width)
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
