/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2001
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * visualization of Selection.
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
#include "platform_tv.h"
#include "select_tv.h"
#include "edit_tv.h"
#include "frame_tv.h"

#include "displayselect_f.h"
#include "exceptions_f.h"
#include "font_f.h"
#include "frame_f.h"
#include "units_f.h"
#include "xwindowdisplay_f.h"


/*----------------------------------------------------------------------
  DisplayPointSelection draw control points of the box.
  ----------------------------------------------------------------------*/
void         DisplayPointSelection (int frame, PtrBox pBox, int pointselect)
{
  ViewFrame          *pFrame;
  PtrAbstractBox      pAb;
  PtrTextBuffer       pBuffer;
  PtrPathSeg          pPa;
  int                 leftX, middleX, rightX;
  int                 topY, middleY, bottomY;
  int                 thick, halfThick;
  int                 i, j, n;
  int                 x, y;

  if (pBox != NULL)
    {
      pFrame = &ViewFrameTable[frame - 1];
      pAb = pBox->BxAbstractBox;
      thick = HANDLE_WIDTH;
      if (thick > pBox->BxWidth)
	thick = pBox->BxWidth;
      if (thick > pBox->BxHeight)
	thick = pBox->BxHeight;
      halfThick = thick / 2;

      /* selection points */
      leftX = pBox->BxXOrg - pFrame->FrXOrg;
      topY = pBox->BxYOrg - pFrame->FrYOrg;
      bottomY = topY + pBox->BxHeight - thick;
      rightX = leftX + pBox->BxWidth - thick;
      middleX = leftX + (pBox->BxWidth / 2) - halfThick;
      middleY = topY + (pBox->BxHeight / 2) - halfThick;
      if (pAb->AbLeafType == LtPicture)
	{
	  /* 8 control points */
	  DrawRectangle (frame, 0, 0, leftX, topY, thick, thick,
			 0, InsertColor, 2);
	  DrawRectangle (frame, 0, 0, middleX, topY, thick, thick,
			 0, InsertColor, 2);
	  DrawRectangle (frame, 0, 0, rightX, topY, thick, thick,
			 0, InsertColor, 2);
	  DrawRectangle (frame, 0, 0, leftX, middleY, thick, thick,
			 0, InsertColor, 2);
	  DrawRectangle (frame, 0, 0, rightX, middleY, thick, thick,
			 0, InsertColor, 2);
	  DrawRectangle (frame, 0, 0, leftX, bottomY, thick, thick,
			 0, InsertColor, 2);
	  DrawRectangle (frame, 0, 0, middleX, bottomY, thick, thick,
			 0, InsertColor, 2);
	  DrawRectangle (frame, 0, 0, rightX, bottomY, thick, thick,
			 0, InsertColor, 2);
	}
      else if (pAb->AbLeafType == LtPolyLine && pBox->BxNChars > 1)
	{
	  /* Draw control points of the polyline */
	  /* if there is at least one point in the polyline */
	  pBuffer = pBox->BxBuffer;
	  leftX = pBox->BxXOrg - pFrame->FrXOrg - halfThick;
	  topY = pBox->BxYOrg - pFrame->FrYOrg - halfThick;
	  j = 1;
	  n = pBox->BxNChars;
	  for (i = 1; i < n; i++)
	    {
	      if (j >= pBuffer->BuLength)
		{
		  if (pBuffer->BuNext != NULL)
		    {
		      /* Changement de buffer */
		      pBuffer = pBuffer->BuNext;
		      j = 0;
		    }
		}
	      if (pointselect == 0 || pointselect == i)
		{
		  x = leftX + PixelValue (pBuffer->BuPoints[j].XCoord,
				   UnPixel, NULL,
				   ViewFrameTable[frame - 1].FrMagnification);
		  y = topY + PixelValue (pBuffer->BuPoints[j].YCoord,
				   UnPixel, NULL,
				   ViewFrameTable[frame - 1].FrMagnification);
		  DrawRectangle (frame, 0, 0, x, y, thick, thick, 0,
				 InsertColor, 2);
		}
	      
	      j++;
	    }
	}
      else if (pAb->AbLeafType == LtPath)
	{
	  /* Draw control points of the path */
	  pPa = pAb->AbFirstPathSeg;
	  leftX = pBox->BxXOrg - pFrame->FrXOrg - halfThick;
	  topY = pBox->BxYOrg - pFrame->FrYOrg - halfThick;
	  while (pPa)
	    {
	      if (pPa->PaNewSubpath || !pPa->PaPrevious)
		/* this path segment starts a new subpath */
		{
		  x = leftX + PixelValue (pPa->XStart, UnPixel, NULL,
				   ViewFrameTable[frame - 1].FrMagnification);
		  y = topY + PixelValue (pPa->YStart, UnPixel, NULL,
                                   ViewFrameTable[frame - 1].FrMagnification);
		  DrawRectangle (frame, 0, 0, x, y, thick, thick, 0,
				 InsertColor, 2);
		}
	      x = leftX + PixelValue (pPa->XEnd, UnPixel, NULL,
				   ViewFrameTable[frame - 1].FrMagnification);
	      y = topY + PixelValue (pPa->YEnd, UnPixel, NULL,
				   ViewFrameTable[frame - 1].FrMagnification);
	      DrawRectangle (frame, 0, 0, x, y, thick, thick, 0,
			     InsertColor, 2);
	      pPa = pPa->PaNext;
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
			   0, InsertColor, 2);
	    break;
	  case 2:
	    DrawRectangle (frame, 0, 0, middleX, topY, thick, thick,
			   0, InsertColor, 2);
	    break;
	  case 3:
	    DrawRectangle (frame, 0, 0, rightX, topY, thick, thick,
			   0, InsertColor, 2);
	    break;
	  case 4:
	    DrawRectangle (frame, 0, 0, rightX, middleY, thick, thick,
			   0, InsertColor, 2);
	    break;
	  case 5:
	    DrawRectangle (frame, 0, 0, rightX, bottomY, thick, thick,
			   0, InsertColor, 2);
	    break;
	  case 6:
	    DrawRectangle (frame, 0, 0, middleX, bottomY, thick, thick,
			   0, InsertColor, 2);
	    break;
	  case 7:
	    DrawRectangle (frame, 0, 0, leftX, bottomY, thick, thick,
			   0, InsertColor, 2);
	    break;
	  case 8:
	    DrawRectangle (frame, 0, 0, leftX, middleY, thick, thick,
			   0, InsertColor, 2);
	    break;
	  }
      else if (pAb->AbLeafType == LtGraphics && pAb->AbVolume != 0)
	/* C'est une boite graphique */
	/* On marque les points caracteristiques de la boite */
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
	    /* 8 control points */
	    DrawRectangle (frame, 0, 0, leftX, topY, thick, thick,
			   0, InsertColor, 2);
	    DrawRectangle (frame, 0, 0, middleX, topY, thick, thick,
			   0, InsertColor, 2);
	    DrawRectangle (frame, 0, 0, rightX, topY, thick, thick,
			   0, InsertColor, 2);
	    DrawRectangle (frame, 0, 0, leftX, middleY, thick, thick,
			   0, InsertColor, 2);
	    DrawRectangle (frame, 0, 0, rightX, middleY, thick, thick,
			   0, InsertColor, 2);
	    DrawRectangle (frame, 0, 0, leftX, bottomY, thick, thick,
			   0, InsertColor, 2);
	    DrawRectangle (frame, 0, 0, middleX, bottomY, thick, thick,
			   0, InsertColor, 2);
	    DrawRectangle (frame, 0, 0, rightX, bottomY, thick, thick,
			   0, InsertColor, 2);
	    break;
	  case 'C':
	  case 'L':
	  case 'a':
	  case 'c':
	  case 'P':
	  case 'Q':
	    /* 4 control points */
	    DrawRectangle (frame, 0, 0, middleX, topY, thick, thick,
			   0, InsertColor, 2);
	    DrawRectangle (frame, 0, 0, leftX, middleY, thick, thick,
			   0, InsertColor, 2);
	    DrawRectangle (frame, 0, 0, rightX, middleY, thick, thick,
			   0, InsertColor, 2);
	    DrawRectangle (frame, 0, 0, middleX, bottomY, thick, thick,
			   0, InsertColor, 2);
	    if (pAb->AbRealShape == 'C' && pAb->AbRx == 0 && pAb->AbRy == 0)
	      /* rounded corners are not round. display a control point
		 for each corner */
	      {
		DrawRectangle (frame, 0, 0, leftX, topY, thick, thick,
			       0, InsertColor, 2);
		DrawRectangle (frame, 0, 0, rightX, topY, thick, thick,
			       0, InsertColor, 2);
		DrawRectangle (frame, 0, 0, leftX, bottomY, thick, thick,
			       0, InsertColor, 2);
		DrawRectangle (frame, 0, 0, rightX, bottomY, thick, thick,
			       0, InsertColor, 2);
	      }
	    break;
	  case 'W':
	    /* 3 control points */
	    DrawRectangle (frame, 0, 0, leftX, topY, thick, thick,
			   0, InsertColor, 2);
	    DrawRectangle (frame, 0, 0, rightX, topY, thick, thick,
			   0, InsertColor, 2);
	    DrawRectangle (frame, 0, 0, rightX, bottomY, thick, thick,
			   0, InsertColor, 2);
	    break;
	  case 'X':
	    /* 3 control points */
	    DrawRectangle (frame, 0, 0, rightX, topY, thick, thick,
			   0, InsertColor, 2);
	    DrawRectangle (frame, 0, 0, leftX, bottomY, thick, thick,
			   0, InsertColor, 2);
	    DrawRectangle (frame, 0, 0, rightX, bottomY, thick, thick,
			   0, InsertColor, 2);
	    break;
	  case 'Y':
	    /* 3 control points */
	    DrawRectangle (frame, 0, 0, leftX, topY, thick, thick,
			   0, InsertColor, 2);
	    DrawRectangle (frame, 0, 0, leftX, bottomY, thick, thick,
			   0, InsertColor, 2);
	    DrawRectangle (frame, 0, 0, rightX, bottomY, thick, thick,
			   0, InsertColor, 2);
	    break;
	  case 'Z':
	    /* 3 control points */
	    DrawRectangle (frame, 0, 0, leftX, topY, thick, thick,
			   0, InsertColor, 2);
	    DrawRectangle (frame, 0, 0, rightX, topY, thick, thick,
			   0, InsertColor, 2);
	    DrawRectangle (frame, 0, 0, rightX, bottomY, thick, thick,
			   0, InsertColor, 2);
	    break;
	    
	  case 'h':
	  case '<':
	  case '>':
	    /* 2 control points */
	    DrawRectangle (frame, 0, 0, leftX, middleY, thick, thick,
			   0, InsertColor, 2);
	    DrawRectangle (frame, 0, 0, rightX, middleY, thick, thick,
			   0, InsertColor, 2);
	    break;
	  case 't':
	    /* 3 control points */
	    DrawRectangle (frame, 0, 0, leftX, topY, thick, thick,
			   0, InsertColor, 2);
	    DrawRectangle (frame, 0, 0, middleX, topY, thick, thick,
			   0, InsertColor, 2);
	    DrawRectangle (frame, 0, 0, rightX, topY, thick, thick,
			   0, InsertColor, 2);
	    break;
	  case 'b':
	    /* 3 control points */
	    DrawRectangle (frame, 0, 0, leftX, bottomY, thick, thick,
			   0, InsertColor, 2);
	    DrawRectangle (frame, 0, 0, middleX, bottomY, thick, thick,
			   0, InsertColor, 2);
	    DrawRectangle (frame, 0, 0, rightX, bottomY, thick, thick,
			   0, InsertColor, 2);
	    break;
	  case 'v':
	  case '^':
	  case 'V':
	    /* 2 control points */
	    DrawRectangle (frame, 0, 0, middleX, topY, thick, thick,
			   0, InsertColor, 2);
	    DrawRectangle (frame, 0, 0, middleX, bottomY, thick, thick,
			   0, InsertColor, 2);
	    break;
	  case 'l':
	    /* 3 control points */
	    DrawRectangle (frame, 0, 0, leftX, topY, thick, thick,
			   0, InsertColor, 2);
	    DrawRectangle (frame, 0, 0, leftX, middleY, thick, thick,
			   0, InsertColor, 2);
	    DrawRectangle (frame, 0, 0, leftX, bottomY, thick, thick,
			   0, InsertColor, 2);
	    break;
	  case 'r':
	    /* 3 control points */
	    DrawRectangle (frame, 0, 0, rightX, topY, thick, thick,
			   0, InsertColor, 2);
	    DrawRectangle (frame, 0, 0, rightX, middleY, thick, thick,
			   0, InsertColor, 2);
	    DrawRectangle (frame, 0, 0, rightX, bottomY, thick, thick,
			   0, InsertColor, 2);
	    break;
	  case '\\':
	  case 'O':
	  case 'e':
	    /* 2 control points */
	    DrawRectangle (frame, 0, 0, leftX, topY, thick, thick,
			   0, InsertColor, 2);
	    DrawRectangle (frame, 0, 0, rightX, bottomY, thick, thick,
			   0, InsertColor, 2);
	    break;
	  case '/':
	  case 'o':
	  case 'E':
	    /* 2 control points */
	    DrawRectangle (frame, 0, 0, rightX, topY, thick, thick,
			   0, InsertColor, 2);
	    DrawRectangle (frame, 0, 0, leftX, bottomY, thick, thick,
			   0, InsertColor, 2);
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
			       0, InsertColor, 2);
		DrawRectangle (frame, 0, 0, rightX, bottomY, thick, thick,
			       0, InsertColor, 2);
	      }
	    else
	      {
		/* draw a / */
		/* 2 control points */
		DrawRectangle (frame, 0, 0, rightX, topY, thick, thick,
			       0, InsertColor, 2);
		DrawRectangle (frame, 0, 0, leftX, bottomY, thick, thick,
			       0, InsertColor, 2);
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
void         DisplayBgBoxSelection (int frame, PtrBox pBox)
{
  PtrBox              pChildBox;
  ViewFrame          *pFrame;
  PtrAbstractBox      pAb;
  int                 leftX, topY;

  if (pBox != NULL)
    {
      pFrame = &ViewFrameTable[frame - 1];
      pAb = pBox->BxAbstractBox;
      if (pBox->BxType == BoSplit)
	{
	  /* display the selection on pieces of the current box */
	  pChildBox = pBox->BxNexChild;
	  while (pChildBox != NULL)
	    {
	      DisplayBgBoxSelection (frame, pChildBox);
	      pChildBox = pChildBox->BxNexChild;
	    }
	}
      else if (pAb->AbLeafType == LtPicture ||
	       pAb->AbLeafType == LtGraphics ||
	       pAb->AbLeafType == LtPath ||
	       pAb->AbLeafType == LtPolyLine)
	DisplayPointSelection (frame, pBox, 0);
      else
	{
	  /* the whole box is selected */
	  leftX = pBox->BxXOrg - pFrame->FrXOrg;
	  topY = pBox->BxYOrg - pFrame->FrYOrg;
	  /* draw the background of the selection */
	  DrawRectangle (frame, 0, 0, leftX, topY,
			 pBox->BxWidth, pBox->BxHeight,
			 0, SelColor, 2);
	}
    }
}


/*----------------------------------------------------------------------
  DrawBoxSelection paints the box with the selection background.
  ----------------------------------------------------------------------*/
void         DrawBoxSelection (int frame, PtrBox pBox)
{
  PtrBox              pChildBox;
  PtrAbstractBox      pAb;

  if (pBox != NULL)
    {
      pAb = pBox->BxAbstractBox;      
      if (pBox->BxType == BoSplit)
	{
	  /* display the selection on pieces of the current box */
	  pChildBox = pBox->BxNexChild;
	  while (pChildBox != NULL)
	    {
	      DrawBoxSelection (frame, pChildBox);
	      pChildBox = pChildBox->BxNexChild;
	    }
	}
      else
	/* display other elements */
	DefClip (frame, pBox->BxXOrg, pBox->BxYOrg,
		 pBox->BxXOrg + pBox->BxWidth,
		 pBox->BxYOrg + pBox->BxHeight);
    }
}

/*----------------------------------------------------------------------
  SetNewSelectionStatus goes through the tree for switching the selection
  indicator.
  ----------------------------------------------------------------------*/
void    SetNewSelectionStatus (int frame, PtrAbstractBox pAb, ThotBool status)
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
  ----------------------------------------------------------------------*/
void         DisplayStringSelection (int frame, int leftX, int rightX, PtrBox pBox)
{
  PtrBox              pParentBox;
  ViewFrame          *pFrame;
  PtrAbstractBox      pAb;
  int                 width, height;
  int                 topY, h;
  int                 col;

  pFrame = &ViewFrameTable[frame - 1];
  if (pBox->BxAbstractBox != NULL)
    {
      /* For holophrasted documents there is no enclosing */
      if (pBox->BxAbstractBox->AbEnclosing == NULL)
	pParentBox = pBox;
      else
	{
	  pParentBox = pBox->BxAbstractBox->AbEnclosing->AbBox;
	  while (pParentBox->BxType == BoGhost)
	    {
	      pAb = pParentBox->BxAbstractBox;
	      if (pAb->AbEnclosing == NULL)
		pParentBox = pBox;
	      else
		pParentBox = pAb->AbEnclosing->AbBox;
	    }
	}
      /* clipped by the enclosing box */
      height = pParentBox->BxYOrg + pParentBox->BxHeight - pFrame->FrYOrg;
      /* and the scrolling zone */
      width = FrameTable[frame].FrScrollOrg + FrameTable[frame].FrScrollWidth
	      - pFrame->FrXOrg;

      topY = pBox->BxYOrg - pFrame->FrYOrg;
      h = pBox->BxHeight;
      if (topY > height)
	h = 0;
      else if (topY + h > height)
	h = height - topY;
      
      leftX = leftX + pBox->BxXOrg - pFrame->FrXOrg;
      if (leftX > width)
	width = 0;
      else
	{
	  rightX = rightX + pBox->BxXOrg - pFrame->FrXOrg;
	  if (rightX > width)
	    width -= leftX;
	  else
	    width = rightX - leftX;
	}
      if (width <= 2)
	col = InsertColor;
      else
	col = SelColor;
      DrawRectangle (frame, 0, 0, leftX, topY, width, h, 0, col, 2);
    }
}
