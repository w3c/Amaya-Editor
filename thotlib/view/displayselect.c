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
 * visualisation of Selections.
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
#include "windowdisplay_f.h"


/*----------------------------------------------------------------------
  DisplayPointSelection draw characteristics point of the box.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void         DisplayPointSelection (int frame, PtrBox pBox, int pointselect)
#else  /* __STDC__ */
void         DisplayPointSelection (frame, pBox, pointselect)
int                 frame;
PtrBox              pBox;
int                 pointselect;
#endif /* __STDC__ */
{
  ViewFrame          *pFrame;
  PtrAbstractBox      pAb;
  PtrTextBuffer       pBuffer;
  int                 leftX, rightX;
  int                 topY, bottomY;
  int                 minX, thick;
  int                 middleY;
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

      /* selection points */
      leftX = pBox->BxXOrg - pFrame->FrXOrg;
      topY = pBox->BxYOrg - pFrame->FrYOrg;
      bottomY = topY + pBox->BxHeight - thick;
      rightX = leftX + pBox->BxWidth - thick;
      minX = leftX + (pBox->BxWidth - thick) / 2;
      middleY = topY + (pBox->BxHeight - thick) / 2;
      if (pAb->AbLeafType == LtPicture)
	{
	  /* 8 characteristic points */
	  DrawRectangle (frame, 0, 0, leftX, topY, thick, thick,
			 0, 0, 0, InsertColor, 2);
	  DrawRectangle (frame, 0, 0, minX, topY, thick, thick,
			 0, 0, 0, InsertColor, 2);
	  DrawRectangle (frame, 0, 0, rightX, topY, thick, thick,
			 0, 0, 0, InsertColor, 2);
	  DrawRectangle (frame, 0, 0, leftX, middleY, thick, thick,
			 0, 0, 0, InsertColor, 2);
	  DrawRectangle (frame, 0, 0, rightX, middleY, thick, thick,
			 0, 0, 0, InsertColor, 2);
	  DrawRectangle (frame, 0, 0, leftX, bottomY, thick, thick,
			 0, 0, 0, InsertColor, 2);
	  DrawRectangle (frame, 0, 0, minX, bottomY, thick, thick,
			 0, 0, 0, InsertColor, 2);
	  DrawRectangle (frame, 0, 0, rightX, bottomY, thick, thick,
			 0, 0, 0, InsertColor, 2);
	}
      else if (pAb->AbLeafType == LtPolyLine && pBox->BxNChars > 1)
	{
	  /* Draw characteristic points of the polyline */
	  /* if there is almost one point in he polyline */
	  pBuffer = pBox->BxBuffer;
	  leftX = pBox->BxXOrg - pFrame->FrXOrg;
	  topY = pBox->BxYOrg - pFrame->FrYOrg;
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
		  x = leftX + PointToPixel (pBuffer->BuPoints[j].XCoord / 1000);
		  if (x > rightX)
		    x =  rightX;
		  y = topY + PointToPixel (pBuffer->BuPoints[j].YCoord / 1000);
		  if (y > bottomY)
		    y =  bottomY;
		  DrawRectangle (frame, 0, 0,
				 x, y,
				 thick, thick,
				 0, 0, 0, InsertColor, 2);
		}
	      
	      j++;
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
			   0, 0, 0, InsertColor, 2);
	    break;
	  case 2:
	    DrawRectangle (frame, 0, 0, minX, topY, thick, thick,
			   0, 0, 0, InsertColor, 2);
	    break;
	  case 3:
	    DrawRectangle (frame, 0, 0, rightX, topY, thick, thick,
			   0, 0, 0, InsertColor, 2);
	    break;
	  case 4:
	    DrawRectangle (frame, 0, 0, rightX, middleY, thick, thick,
			   0, 0, 0, InsertColor, 2);
	    break;
	  case 5:
	    DrawRectangle (frame, 0, 0, rightX, bottomY, thick, thick,
			   0, 0, 0, InsertColor, 2);
	    break;
	  case 6:
	    DrawRectangle (frame, 0, 0, minX, bottomY, thick, thick,
			   0, 0, 0, InsertColor, 2);
	    break;
	  case 7:
	    DrawRectangle (frame, 0, 0, leftX, bottomY, thick, thick,
			   0, 0, 0, InsertColor, 2);
	    break;
	  case 8:
	    DrawRectangle (frame, 0, 0, leftX, middleY, thick, thick,
			   0, 0, 0, InsertColor, 2);
	    break;
	  }
      else if (pAb->AbLeafType == LtGraphics && pAb->AbVolume != 0)
	/* C'est une boite graphique */
	/* On marque en noir les points caracteristiques de la boite */
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
	    /* 8 characteristic points */
	    DrawRectangle (frame, 0, 0, leftX, topY, thick, thick,
			   0, 0, 0, InsertColor, 2);
	    DrawRectangle (frame, 0, 0, minX, topY, thick, thick,
			   0, 0, 0, InsertColor, 2);
	    DrawRectangle (frame, 0, 0, rightX, topY, thick, thick,
			   0, 0, 0, InsertColor, 2);
	    DrawRectangle (frame, 0, 0, leftX, middleY, thick, thick,
			   0, 0, 0, InsertColor, 2);
	    DrawRectangle (frame, 0, 0, rightX, middleY, thick, thick,
			   0, 0, 0, InsertColor, 2);
	    DrawRectangle (frame, 0, 0, leftX, bottomY, thick, thick,
			   0, 0, 0, InsertColor, 2);
	    DrawRectangle (frame, 0, 0, minX, bottomY, thick, thick,
			   0, 0, 0, InsertColor, 2);
	    DrawRectangle (frame, 0, 0, rightX, bottomY, thick, thick,
			   0, 0, 0, InsertColor, 2);
	    break;
	  case 'C':
	  case 'L':
	  case 'a':
	  case 'c':
	  case 'P':
	  case 'Q':
	    /* 4 characteristic points */
	    DrawRectangle (frame, 0, 0, minX, topY, thick, thick,
			   0, 0, 0, InsertColor, 2);
	    DrawRectangle (frame, 0, 0, leftX, middleY, thick, thick,
			   0, 0, 0, InsertColor, 2);
	    DrawRectangle (frame, 0, 0, rightX, middleY, thick, thick,
			   0, 0, 0, InsertColor, 2);
	    DrawRectangle (frame, 0, 0, minX, bottomY, thick, thick,
			   0, 0, 0, InsertColor, 2);
	    break;
	  case 'W':
	    /* 3 characteristic points */
	    DrawRectangle (frame, 0, 0, leftX, topY, thick, thick,
			   0, 0, 0, InsertColor, 2);
	    DrawRectangle (frame, 0, 0, rightX, topY, thick, thick,
			   0, 0, 0, InsertColor, 2);
	    DrawRectangle (frame, 0, 0, rightX, bottomY, thick, thick,
			   0, 0, 0, InsertColor, 2);
	    break;
	  case 'X':
	    /* 3 characteristic points */
	    DrawRectangle (frame, 0, 0, rightX, topY, thick, thick,
			   0, 0, 0, InsertColor, 2);
	    DrawRectangle (frame, 0, 0, leftX, bottomY, thick, thick,
			   0, 0, 0, InsertColor, 2);
	    DrawRectangle (frame, 0, 0, rightX, bottomY, thick, thick,
			   0, 0, 0, InsertColor, 2);
	    break;
	  case 'Y':
	    /* 3 characteristic points */
	    DrawRectangle (frame, 0, 0, leftX, topY, thick, thick,
			   0, 0, 0, InsertColor, 2);
	    DrawRectangle (frame, 0, 0, leftX, bottomY, thick, thick,
			   0, 0, 0, InsertColor, 2);
	    DrawRectangle (frame, 0, 0, rightX, bottomY, thick, thick,
			   0, 0, 0, InsertColor, 2);
	    break;
	  case 'Z':
	    /* 3 characteristic points */
	    DrawRectangle (frame, 0, 0, leftX, topY, thick, thick,
			   0, 0, 0, InsertColor, 2);
	    DrawRectangle (frame, 0, 0, rightX, topY, thick, thick,
			   0, 0, 0, InsertColor, 2);
	    DrawRectangle (frame, 0, 0, rightX, bottomY, thick, thick,
			   0, 0, 0, InsertColor, 2);
	    break;
	    
	  case 'h':
	  case '<':
	  case '>':
	    /* 2 characteristic points */
	    DrawRectangle (frame, 0, 0, leftX, middleY, thick, thick,
			   0, 0, 0, InsertColor, 2);
	    DrawRectangle (frame, 0, 0, rightX, middleY, thick, thick,
			   0, 0, 0, InsertColor, 2);
	    break;
	  case 't':
	    /* 3 characteristic points */
	    DrawRectangle (frame, 0, 0, leftX, topY, thick, thick,
			   0, 0, 0, InsertColor, 2);
	    DrawRectangle (frame, 0, 0, minX, topY, thick, thick,
			   0, 0, 0, InsertColor, 2);
	    DrawRectangle (frame, 0, 0, rightX, topY, thick, thick,
			   0, 0, 0, InsertColor, 2);
	    break;
	  case 'b':
	    /* 3 characteristic points */
	    DrawRectangle (frame, 0, 0, leftX, bottomY, thick, thick,
			   0, 0, 0, InsertColor, 2);
	    DrawRectangle (frame, 0, 0, minX, bottomY, thick, thick,
			   0, 0, 0, InsertColor, 2);
	    DrawRectangle (frame, 0, 0, rightX, bottomY, thick, thick,
			   0, 0, 0, InsertColor, 2);
	    break;
	  case 'v':
	  case '^':
	  case 'V':
	    /* 2 characteristic points */
	    DrawRectangle (frame, 0, 0, minX, topY, thick, thick,
			   0, 0, 0, InsertColor, 2);
	    DrawRectangle (frame, 0, 0, minX, bottomY, thick, thick,
			   0, 0, 0, InsertColor, 2);
	    break;
	  case 'l':
	    /* 3 characteristic points */
	    DrawRectangle (frame, 0, 0, leftX, topY, thick, thick,
			   0, 0, 0, InsertColor, 2);
	    DrawRectangle (frame, 0, 0, leftX, middleY, thick, thick,
			   0, 0, 0, InsertColor, 2);
	    DrawRectangle (frame, 0, 0, leftX, bottomY, thick, thick,
			   0, 0, 0, InsertColor, 2);
	    break;
	  case 'r':
	    /* 3 characteristic points */
	    DrawRectangle (frame, 0, 0, rightX, topY, thick, thick,
			   0, 0, 0, InsertColor, 2);
	    DrawRectangle (frame, 0, 0, rightX, middleY, thick, thick,
			   0, 0, 0, InsertColor, 2);
	    DrawRectangle (frame, 0, 0, rightX, bottomY, thick, thick,
			   0, 0, 0, InsertColor, 2);
	    break;
	  case '\\':
	  case 'O':
	  case 'e':
	    /* 2 characteristic points */
	    DrawRectangle (frame, 0, 0, leftX, topY, thick, thick,
			   0, 0, 0, InsertColor, 2);
	    DrawRectangle (frame, 0, 0, rightX, bottomY, thick, thick,
			   0, 0, 0, InsertColor, 2);
	    break;
	  case '/':
	  case 'o':
	  case 'E':
	    /* 2 characteristic points */
	    DrawRectangle (frame, 0, 0, rightX, topY, thick, thick,
			   0, 0, 0, InsertColor, 2);
	    DrawRectangle (frame, 0, 0, leftX, bottomY, thick, thick,
			   0, 0, 0, InsertColor, 2);
	    break;
	  case 'g':
	    /* Coords of the line are given by the enclosing box */
	    pAb = pAb->AbEnclosing;
	    if ((pAb->AbHorizPos.PosEdge == Left && pAb->AbVertPos.PosEdge == Top) ||
		(pAb->AbHorizPos.PosEdge == Right && pAb->AbVertPos.PosEdge == Bottom))
	      {
		/* draw a \ */
		/* 2 characteristic points */
		DrawRectangle (frame, 0, 0, leftX, topY, thick, thick,
			       0, 0, 0, InsertColor, 2);
		DrawRectangle (frame, 0, 0, rightX, bottomY, thick, thick,
			       0, 0, 0, InsertColor, 2);
	      }
	    else
	      {
		/* draw a / */
		/* 2 characteristic points */
		DrawRectangle (frame, 0, 0, rightX, topY, thick, thick,
			       0, 0, 0, InsertColor, 2);
		DrawRectangle (frame, 0, 0, leftX, bottomY, thick, thick,
			       0, 0, 0, InsertColor, 2);
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
#ifdef __STDC__
void         DisplayBgBoxSelection (int frame, PtrBox pBox)
#else  /* __STDC__ */
void         DisplayBgBoxSelection (frame, pBox)
int          frame;
PtrBox       pBox;
#endif /* __STDC__ */
{
  PtrBox              pChildBox;
  ViewFrame          *pFrame;
  PtrAbstractBox      pAb;
  int                 leftX, topY;

  if (pBox != NULL)
    {
      pFrame = &ViewFrameTable[frame - 1];
      pAb = pBox->BxAbstractBox;
      
      if (pBox->BxType == BoGhost
	  || (pAb != NULL
	      && TypeHasException (ExcHighlightChildren, pAb->AbElement->ElTypeNumber, pAb->AbElement->ElStructSchema)))
	{
	  /* the box is not displayed, select its children */
	  if (pAb->AbFirstEnclosed != NULL)
	    {
	      pChildBox = pAb->AbFirstEnclosed->AbBox;
	      while (pChildBox != NULL)
		{
		  DisplayBgBoxSelection (frame, pChildBox);
		  pAb = pChildBox->BxAbstractBox;
		  if (pAb->AbNext != NULL)
		    pChildBox = pAb->AbNext->AbBox;
		  else
		    pChildBox = NULL;
		}
	    }
	}
      else if (pBox->BxType == BoSplit)
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
			 0, 0, 0, SelColor, 2);
	}
    }
}

/*----------------------------------------------------------------------
  DisplayBgSelection goes through the tree for displaying the background
  selection.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                DisplayBgSelection (int frame, PtrAbstractBox pAb)
#else  /* __STDC__ */
void                DisplayBgSelection (frame, pAb)
int                 frame;
PtrAbstractBox      pAb;
#endif /* __STDC__ */
{
   PtrAbstractBox      pChildAb;
   ViewFrame          *pFrame;

   if (pAb != NULL)
     {
	/* The abstract box is selected */
	if (pAb->AbSelected)
	  {
	     pFrame = &ViewFrameTable[frame - 1];
	     if (pFrame->FrSelectionBegin.VsBox == NULL ||
		 pFrame->FrSelectionEnd.VsBox == NULL ||
		 pAb->AbLeafType == LtCompound ||
		 (pAb != pFrame->FrSelectionBegin.VsBox->BxAbstractBox &&
		  pAb != pFrame->FrSelectionEnd.VsBox->BxAbstractBox))
		 /* it's not a terminal extremity of the selection */
	       DisplayBgBoxSelection (frame, pAb->AbBox);
	  }
	else if (pAb->AbLeafType == LtCompound)
	   /* Sinon on parcours le sous-arbre */
	  {
	     pChildAb = pAb->AbFirstEnclosed;
	     while (pChildAb != NULL)
	       {
		  DisplayBgSelection (frame, pChildAb);
		  pChildAb = pChildAb->AbNext;
	       }
	  }
     }
}


/*----------------------------------------------------------------------
  DrawBoxSelection paints the box with the selection background.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void         DrawBoxSelection (int frame, PtrBox pBox)
#else  /* __STDC__ */
void         DrawBoxSelection (frame, pBox)
int          frame;
PtrBox       pBox;
#endif /* __STDC__ */
{
  PtrBox              pChildBox;
  PtrAbstractBox      pAb;

  if (pBox != NULL)
    {
      pAb = pBox->BxAbstractBox;      
      if (pBox->BxType == BoGhost
	  || (pAb != NULL
	      && TypeHasException (ExcHighlightChildren, pAb->AbElement->ElTypeNumber, pAb->AbElement->ElStructSchema)))
	{
	  /* the box is not displayed, select its children */
	  if (pAb->AbFirstEnclosed != NULL)
	    {
	      pChildBox = pAb->AbFirstEnclosed->AbBox;
	      while (pChildBox != NULL)
		{
		  DrawBoxSelection (frame, pChildBox);
		  pAb = pChildBox->BxAbstractBox;
		  if (pAb->AbNext != NULL)
		    pChildBox = pAb->AbNext->AbBox;
		  else
		    pChildBox = NULL;
		}
	    }
	}
      else if (pBox->BxType == BoSplit)
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
	{
	  /* display other elements */
	  DefClip (frame, pBox->BxXOrg, pBox->BxYOrg, pBox->BxXOrg + pBox->BxWidth, pBox->BxYOrg + pBox->BxHeight);
	  RedrawFrameBottom (frame, 0, pAb);
	}
    }
}

/*----------------------------------------------------------------------
  SetNewSelectionStatus goes through the tree for switching the selection
  indicator.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                SetNewSelectionStatus (int frame, PtrAbstractBox pAb, ThotBool status)
#else  /* __STDC__ */
void                SetNewSelectionStatus (frame, pAb, status)
int                 frame;
PtrAbstractBox      pAb;
ThotBool            status;

#endif /* __STDC__ */
{
   PtrAbstractBox      pChildAb;
   ViewFrame          *pFrame;

   if (pAb != NULL)
     {
	/* Le pave est selectionne */
	if (pAb->AbSelected)
	  {
	     pFrame = &ViewFrameTable[frame - 1];
	     pAb->AbSelected = status;
	     if ( pFrame->FrClipXBegin == 0 && pFrame->FrClipXEnd == 0 &&
		  pAb->AbVolume != 0)
	       /* ready to un/display the current selection */
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
	   /* Sinon on parcours le sous-arbre */
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
#ifdef __STDC__
void         DisplayStringSelection (int frame, int leftX, int rightX, PtrBox pBox)
#else  /* __STDC__ */
void         DisplayStringSelection (frame, leftX, rightX, pBox)
int                 frame;
int                 leftX;
int                 rightX;
PtrBox              pBox;
#endif /* __STDC__ */
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
      width = FrameTable[frame].FrScrollOrg + FrameTable[frame].FrScrollWidth - pFrame->FrXOrg;

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
      DrawRectangle (frame, 0, 0, leftX, topY, width, h,
		     0, 0, 0, col, 2);
    }
}

