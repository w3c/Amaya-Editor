/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2003
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * This module handles line constructions
 *
 * Author: I. Vatton (INRIA)
 *
 */

#include "ustring.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"

#define THOT_EXPORT extern
#include "boxes_tv.h"
#include "select_tv.h"


#include "boxmoves_f.h"
#include "boxlocate_f.h"
#include "buildboxes_f.h"
#include "memory_f.h"
#include "boxselection_f.h"
#include "buildlines_f.h"
#include "font_f.h"
#include "frame_f.h"
#include "hyphen_f.h"

#ifdef _GL
#include <GL/gl.h>
#include "glwindowdisplay.h"
#endif /* _GL */

#define SPACE_VALUE_MIN  3
#define SPACE_VALUE_MAX  6
/* max number of consecutive hyphens */
#define MAX_SIBLING_HYPHENS 2


/*----------------------------------------------------------------------
  GetNextBox returns the box assiated to the next in live abstract box.
  ----------------------------------------------------------------------*/
PtrBox GetNextBox (PtrAbstractBox pAb)
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
	      /* Est-ce la derniere boite fille d'une boite eclatee */
	      if (pAb->AbEnclosing &&
		  pAb->AbEnclosing->AbBox &&
		  (pAb->AbEnclosing->AbBox->BxType == BoGhost ||
		   pAb->AbEnclosing->AbBox->BxType == BoFloatGhost))
		{
		   /* remonte la hierarchie */
		   pAb = pAb->AbEnclosing;
		   pNextAb = pAb->AbNext;
		}
	      else
		 loop = FALSE;
	   else if (pNextAb->AbDead)
	      pNextAb = pNextAb->AbNext;
	   else if (pNextAb->AbBox == NULL)
	      pNextAb = pNextAb->AbNext;
	/* Est-ce un pave compose eclate ? */
	   else if (pNextAb->AbBox->BxType == BoGhost ||
		    pNextAb->AbBox->BxType == BoFloatGhost)
	     {
		/* descend la hierarchie */
		while (loop)
		   if (pNextAb->AbBox == NULL)
		      pNextAb = pNextAb->AbNext;
		   else if (pNextAb->AbBox->BxType == BoGhost ||
			    pNextAb->AbBox->BxType == BoFloatGhost)
		      pNextAb = pNextAb->AbFirstEnclosed;
		   else
		      loop = FALSE;
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
	    if (result->BxType == BoMulScript)
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
static PtrBox GetPreviousBox (PtrAbstractBox pAb)
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
	     pAb->AbEnclosing->AbBox->BxType == BoFloatGhost)
	   {
	      /* remonte la hierarchie */
	      pAb = pAb->AbEnclosing;
	      pNextAb = pAb->AbPrevious;
	   }
	 else
	    loop = FALSE;
      else if (pNextAb->AbDead)
	 pNextAb = pNextAb->AbPrevious;
      else if (pNextAb->AbBox == NULL)
	 pNextAb = pNextAb->AbPrevious;
      /* Est-ce un pave compose eclate ? */
      else if (pNextAb->AbBox->BxType == BoGhost ||
	       pNextAb->AbBox->BxType == BoFloatGhost)
	{
	   /* descend la hierarchie */
	   while (!pNextAb->AbDead &&
		  (pNextAb->AbBox->BxType == BoGhost ||
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
       if (result->BxType == BoMulScript || result->BxType == BoSplit)
	 /* return the last script box */
	 while (result->BxNexChild)
	   result = result->BxNexChild;
     }
   return result;
}


/*----------------------------------------------------------------------
  Adjust computes the space width in the adjusted line.
  Move and update the width of all included boxes.
  Work with absolute positions when xAbs and yAbs are TRUE.
  ----------------------------------------------------------------------*/
static void Adjust (PtrBox pParentBox, PtrLine pLine, int frame,
		    ThotBool xAbs, ThotBool yAbs)
{
  PtrBox              pBox, pBoxInLine;
  PtrBox              boxes[200];
  int                 width, baseline;
  int                 nSpaces, delta;
  int                 x;
  int                 i, j, k, max;
  ThotBool            rtl;

  if (pLine->LiFirstBox == NULL)
    /* no box in the current line */
    return;
  /* take into account the writing direction */
  rtl = pParentBox->BxAbstractBox->AbDirection == 'R';
  x = pLine->LiXOrg;
  if (rtl)
    /* right-to-left wirting */
    x += pLine->LiXMax;
  if (xAbs)
    x += pParentBox->BxXOrg;
  nSpaces = 0;	/* number of spaces */
  width = 0;	/* text width without spaces */
  baseline = pLine->LiYOrg + pLine->LiHorizRef;
  if (yAbs)
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
	if (pBox->BxAbstractBox->AbFloat == 'N')
	  {
	    if (!pBox->BxAbstractBox->AbHorizEnclosing ||
		(pBox->BxAbstractBox->AbNotInLine &&
		 pBox->BxAbstractBox->AbDisplay != 'U'))
	      YMove (pBox, NULL, baseline - pBox->BxHorizRef - pBox->BxYOrg, frame);
	    else if (!pBox->BxAbstractBox->AbNotInLine)
	      {
		boxes[max++] = pBox;
		pBox->BxSpaceWidth = 0;
		/* Compute the line width without spaces */
		if (pBox->BxAbstractBox->AbLeafType == LtText)
		  {
		    delta = pBox->BxNSpaces * BoxCharacterWidth (SPACE, pBox->BxFont);
		    pBox->BxW -= delta;
		    pBox->BxWidth -= delta;
		    nSpaces += pBox->BxNSpaces;
		  }
		width += pBox->BxWidth;
	      }
	  }
	if (pBox->BxAbstractBox->AbLeafType == LtText && pBox->BxNexChild)
	  /* get the next child */
	  pBoxInLine = pBox->BxNexChild;
	else
	  pBoxInLine = GetNextBox (pBox->BxAbstractBox);
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
	      x -= boxes[k]->BxWidth;
	      XMove (boxes[k], NULL, x - boxes[k]->BxXOrg, frame);
	      YMove (boxes[k], NULL, baseline - boxes[k]->BxHorizRef - boxes[k]->BxYOrg, frame);
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
	      XMove (boxes[k], NULL, x - boxes[k]->BxXOrg, frame);
	      YMove (boxes[k], NULL, baseline - boxes[k]->BxHorizRef - boxes[k]->BxYOrg, frame);
	      x += boxes[k]->BxWidth;
	    }

	}
      else
	{
	  if (rtl)
	    /* right-to-left wirting */
	    x -= boxes[i]->BxWidth;
	  XMove (boxes[i], NULL, x - boxes[i]->BxXOrg, frame);
	  YMove (boxes[i], NULL, baseline - boxes[i]->BxHorizRef - boxes[i]->BxYOrg, frame);
	  if (!rtl)
	    /* left-to-right wirting */
	    x += boxes[i]->BxWidth;
	}
    }
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
  Align aligns included boxes in the line.
  Move all included boxes.
  Work with absolute positions when xAbs and yAbs are TRUE.
  ----------------------------------------------------------------------*/
static void Align (PtrBox pParentBox, PtrLine pLine, int frame,
		   ThotBool adjust, ThotBool xAbs, ThotBool yAbs)
{
  PtrBox              pBox, pBoxInLine;
  PtrBox              boxes[200];
  int                 baseline, x, delta = 0;
  int                 i, j, k, max;
  ThotBool            rtl;

  if (pLine->LiFirstBox == NULL)
    /* no box in the current line */
    return;
  /* The baseline of the line */
  baseline = pLine->LiYOrg + pLine->LiHorizRef;
  /* take into account the writing direction */
  rtl = pParentBox->BxAbstractBox->AbDirection == 'R';
  if (rtl)
    {
      /* right-to-left writing */
      if (pLine->LiRealLength > pLine->LiXMax)
	/* this could be the case of a too short table */
	x = pLine->LiXOrg + pLine->LiRealLength;
      else
	{
	  if (adjust)
	    {
	      if (pParentBox->BxAbstractBox->AbAdjust == AlignCenter)
		delta = (pLine->LiXMax - pLine->LiRealLength) / 2;
	      else if (pParentBox->BxAbstractBox->AbAdjust == AlignLeft)
		delta = pLine->LiXMax - pLine->LiRealLength;
	    }
	x = pLine->LiXOrg + pLine->LiXMax - delta;
	}
    }
  else
    {
      if (adjust)
	{
	  if (pParentBox->BxAbstractBox->AbAdjust == AlignCenter)
	    delta = (pLine->LiXMax - pLine->LiRealLength) / 2;
	  else if (pParentBox->BxAbstractBox->AbAdjust == AlignRight)
	    delta = pLine->LiXMax - pLine->LiRealLength;
	}
      x = pLine->LiXOrg + delta;
    }
  if (xAbs)
    x += pParentBox->BxXOrg;
  if (yAbs)
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
	if (pBox->BxAbstractBox->AbFloat == 'N')
	  {
	    if (!pBox->BxAbstractBox->AbHorizEnclosing ||
		(pBox->BxAbstractBox->AbNotInLine &&
		 pBox->BxAbstractBox->AbDisplay != 'U'))
	      YMove (pBox, NULL, baseline - pBox->BxHorizRef - pBox->BxYOrg, frame);
	    else if (!pBox->BxAbstractBox->AbNotInLine)
	      {
		boxes[max++] = pBox;
		pBox->BxSpaceWidth = 0;
	      }
	  }
	if (pBox->BxAbstractBox->AbLeafType == LtText && pBox->BxNexChild)
	  /* get the next child */
	  pBoxInLine = pBox->BxNexChild;
	else
	  pBoxInLine = GetNextBox (pBox->BxAbstractBox);
      }
    while (max < 200 && pBoxInLine && pBox != pLine->LiLastBox &&
	   pBox != pLine->LiLastPiece);

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
	      x -= boxes[k]->BxWidth;
	      XMove (boxes[k], NULL, x - boxes[k]->BxXOrg, frame);
	      YMove (boxes[k], NULL, baseline - boxes[k]->BxHorizRef - boxes[k]->BxYOrg, frame);
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
	      XMove (boxes[k], NULL, x - boxes[k]->BxXOrg, frame);
	      YMove (boxes[k], NULL, baseline - boxes[k]->BxHorizRef - boxes[k]->BxYOrg, frame);
	      x += boxes[k]->BxWidth;
	    }

	}
      else
	{
	  if (rtl)
	    /* right-to-left wirting */
	    x -= boxes[i]->BxWidth;
	  XMove (boxes[i], NULL, x - boxes[i]->BxXOrg, frame);
	  YMove (boxes[i], NULL, baseline - boxes[i]->BxHorizRef - boxes[i]->BxYOrg, frame);
	  if (!rtl)
	    /* left-to-right wirting */
	    x += boxes[i]->BxWidth;
	}
    }
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
  ----------------------------------------------------------------------*/
static void ManageBreakLine (PtrBox pBox, int width, int breakWidth,
			     int boxLength, int nSpaces, int newIndex,
			     PtrTextBuffer pNewBuff, PtrAbstractBox pRootAb)
{
  PtrBox              ibox1, ibox2;
  PtrBox              pPreviousBox, pNextBox;
  PtrAbstractBox      pAb;
  SpecFont            font;
  int                 baseline;
  int                 height;

  if (boxLength < 0)
    printf ("Error in ManageBreakLine\n");
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
      ibox2->BxWidth = ibox2->BxW + pBox->BxRMargin + pBox->BxRBorder + pBox->BxRPadding;
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
	  ibox1->BxHorizRef = baseline;
	  ibox1->BxType = BoPiece;
	  ibox1->BxBuffer = pBox->BxBuffer;
	  ibox1->BxFirstChar = pBox->BxFirstChar;
	  ibox1->BxNSpaces = nSpaces;
	  ibox1->BxNChars = boxLength;

	  /* transmit widths, margins, borders, and paddings */
	  ibox1->BxW = width;
	  ibox1->BxWidth = width + pBox->BxLMargin + pBox->BxLBorder + pBox->BxLPadding;
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
	  pBox->BxWidth = width - pBox->BxRMargin - pBox->BxRBorder - pBox->BxRPadding;
	  pBox->BxRMargin = 0;
	  pBox->BxRBorder = 0;
	  pBox->BxRPadding = 0;
	  
	  /* update the chain of leaf boxes */
	  pBox->BxNexChild = ibox2;
	  pBox->BxNext = ibox2;
	  ibox2->BxPrevious = pBox;
	  ibox2->BxNext = pNextBox;
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
  int                 wWidth;
  ThotBool            found;

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

  if (pBuffer && i >= pBuffer->BuLength)
      {
	/* start at the end of a buffer, get the next one */
	pBuffer = pBuffer->BuNext;
	i = 0;
      }

  while (j < nChars && !found && pBuffer)
    {
      character = pBuffer->BuContent[i];
      if (character == BREAK_LINE || character ==  NEW_LINE)
	{
	  /* It's a break element */
	  found = TRUE;
	  *breakWidth = BoxCharacterWidth (BREAK_LINE, font);
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
	      *boxWidth += BoxCharacterWidth (SPACE, font);
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
		l = BoxCharacterWidth (character, font);
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
  int                 width;
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

  charIndex = pBox->BxIndChar; /* buffer index */
  pBuffer = pBox->BxBuffer;
  /* newWidth is the width used to build lines */
  /* width is the real width of the text       */
  newWidth = pBox->BxLMargin + pBox->BxLBorder + pBox->BxLPadding;
  width = 0;
  carWidth = 0;
  wordLength = 0;
  spaceCount = 0;
  still = TRUE;
  spaceWidth = BoxCharacterWidth (SPACE, font);
  spaceAdjust = spaceWidth;
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
	  carWidth = BoxCharacterWidth (character, font);

      if ((newWidth + carWidth > max || i >= count) && i != 0)
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
	  else if ( pBox->BxScript == 'Z' || spaceCount == 0)
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
  else
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
	      wordLength = HyphenLastWord (font, language, &pBuffer,
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
	      wordLength = HyphenLastWord (font, language, pNewBuff,
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
	  *boxWidth += BoxCharacterWidth (173, font);
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
	  *boxWidth -= BoxCharacterWidth (character, font);
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
  max = the maximum width of the first piece.
  pRootAb = the root abstract box for updating the chain of leaf boxes.   
  ----------------------------------------------------------------------*/
static void BreakPieceOfBox (PtrLine pLine, PtrBox pBox, int max,
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
    return;
  pAb = pBox->BxAbstractBox;
  height = pBox->BxHeight;
  baseline = pBox->BxHorizRef;
  pNextBox = pBox->BxNext;
  oldlg = pBox->BxNChars;
  font = pBox->BxFont;
  oldWidth = pBox->BxWidth;
  oldnSpaces = pBox->BxNSpaces;

  /* search a break */
  lostPixels = SearchBreak (pLine, pBox, max, font, &length, &width,
			    &nSpaces, &newIndex, &pNewBuff);
  if (lostPixels <= 0)
    /* don't break on a space */
    spaceWidth = 0;
  else
    /* break on a space */
    spaceWidth = BoxCharacterWidth (SPACE, font);

  /*
   * Generate a new piece:
   * - if SearchBreak found a break point on a space (lostPixels > 0)
   *   or on a character with generation of an hyphen (lostPixels = -1)
   *   or without (lostPixels = -2);
   * - if we want to remove extra spaces at the end of the box.
   */
  if (pNewBuff &&
      (lostPixels != 0 || nSpaces != 0 || oldnSpaces == 0) &&
      (pBox->BxWidth != max || lostPixels != pBox->BxNSpaces))
    ibox2 = GetBox (pAb);
  else
    ibox2 = NULL;
  
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
      pBox->BxWidth = width - pBox->BxRMargin - pBox->BxRBorder - pBox->BxRPadding;
      pBox->BxRMargin = 0;
      pBox->BxRBorder = 0;
      pBox->BxRPadding = 0;

      if (lostPixels == -1)
	{
	  /* add the hyphen at the end */
	  pBox->BxType = BoDotted;
	  width -= BoxCharacterWidth (173, font);
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
      ibox2->BxWidth = ibox2->BxW + pBox->BxRMargin + pBox->BxRBorder + pBox->BxRPadding;
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
}


/*----------------------------------------------------------------------
  BreakMainBox splits a main box into two pieces.
  When force is TRUE splits on any character, else only splits on a space.
  max = the maximum width of the first piece.
  pRootAb = the root abstract box for updating the chain of leaf boxes.   
  ----------------------------------------------------------------------*/
static void BreakMainBox (PtrLine pLine, PtrBox pBox, int max,
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
    {
      BreakPieceOfBox (pLine, pBox, max, pRootAb);
      return;
    }
  pAb = pBox->BxAbstractBox;
  height = pBox->BxHeight;
  baseline = pBox->BxHorizRef;
  pPreviousBox = pBox->BxPrevious;
  pNextBox = pBox->BxNext;
  font = pBox->BxFont;

  /* search a break */
  lostPixels = SearchBreak (pLine, pBox, max, font, &length,
			    &width, &nSpaces, &newIndex, &pNewBuff);

  if (lostPixels <= 0)
    /* don't break on a space */
    spaceWidth = 0;
  else
    /* breakmak on a space */
    spaceWidth = BoxCharacterWidth (SPACE, font);

  /*
   * Generate two pieces:
   * - if SearchBreak found a break point on a space (lostPixels > 0)
   *   or on a character with generation of an hyphen (lostPixels = -1)
   *   or without (lostPixels = -2);
   * - if the boolean force is TRUE;
   * - if we want to remove extra spaces at the end of the box.
   */
  if (pNewBuff
      && (lostPixels != 0 || nSpaces != 0 || force)
      && (pBox->BxWidth != max || lostPixels != pBox->BxNSpaces))
    {
      ibox1 = GetBox (pAb);
      ibox2 = GetBox (pAb);
    }
  else
    {
      ibox1 = NULL;
      ibox2 = NULL;
    }
  
  if (lostPixels == -2)
    /* don't generate the hyphen */
    lostPixels = 0;
  
  if (ibox1 != NULL && ibox2 != NULL)
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
      ibox1->BxHorizRef = baseline;

      /* transmit widths, margins, borders, and paddings */
      ibox1->BxW = width;
      ibox1->BxWidth = width + pBox->BxLMargin + pBox->BxLBorder + pBox->BxLPadding;
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
	  width -= BoxCharacterWidth (173, font);
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
	ibox2->BxW = pBox->BxWidth - width - lostPixels * spaceWidth;      
      ibox2->BxWidth = ibox2->BxW + pBox->BxRMargin + pBox->BxRBorder + pBox->BxRPadding;
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
}


/*----------------------------------------------------------------------
   AddBoxInLine adds a box in a line.
  ----------------------------------------------------------------------*/
static void AddBoxInLine (PtrBox pBox, int *descent, int *ascent, PtrLine pLine)
{
  pLine->LiRealLength += pBox->BxWidth;
  /* Compute the current line height */
  if (*ascent < pBox->BxHorizRef)
    *ascent = pBox->BxHorizRef;
  if (*descent < pBox->BxHeight - pBox->BxHorizRef)
    *descent = pBox->BxHeight - pBox->BxHorizRef;
  /* check if the line includes a compound box or an image */
  if (pBox->BxAbstractBox &&
      (pBox->BxAbstractBox->AbLeafType == LtCompound ||
       pBox->BxAbstractBox->AbLeafType == LtPicture))
    pLine->LiNoOverlap = TRUE;
}

/*----------------------------------------------------------------------
   ClearFloats
  ----------------------------------------------------------------------*/
void ClearFloats (PtrBox pBox)
{
  PtrFloat            pfloat;

  if (pBox &&
      (pBox->BxType == BoBlock || pBox->BxType == BoFloatBlock))
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
  InitLine computes the x,y position and the max width of the line pLine
  in the block of lines pBlock.
  Parameters floatL and floatR point to last left an right floating
  boxes in the current block.
  The parameter pBox points the next box to be inserted.
  Work with absolute positions when xAbs and yAbs are TRUE.
  ----------------------------------------------------------------------*/
static void InitLine (PtrLine pLine, PtrBox pBlock, int indent,
		      PtrBox floatL, PtrBox floatR, PtrBox pBox,
		      ThotBool xAbs, ThotBool yAbs)
{
  PtrFloat            pfloatL = NULL, pfloatR = NULL;
  PtrAbstractBox      pAb, pAbRef;
  char                clearL, clearR;
  int                 bottomL = 0, bottomR = 0, left, top, y;
  int                 orgX, orgY, width;
  ThotBool            variable, newFloat;

  if (pLine == NULL)
    return;

  /* relative line positions */
  left = pBlock->BxLMargin + pBlock->BxLBorder + pBlock->BxLPadding;
  top = pBlock->BxTMargin + pBlock->BxTBorder + pBlock->BxTPadding;
  orgX = 0;
  orgY = 0;
  if (xAbs)
    orgX += pBlock->BxXOrg;
  if (yAbs)
    orgY += pBlock->BxYOrg;
  if (pBox)
    pAb = pBox->BxAbstractBox;
  else
    pAb = NULL;
  /* clear values */
  clearL = 'N';
  clearR = 'N';
  newFloat = FALSE;
  if (pAb)
    {
      newFloat = pAb->AbFloat != 'N';
      if (pAb->AbClear == 'L' || pAb->AbClear == 'B')
	clearL = 'L';
      if (pAb->AbClear == 'R' || pAb->AbClear == 'B')
	clearR = 'R';
    }
  if (pAb)
    pAbRef = pAb->AbWidth.DimAbRef;
  variable = (pBox &&
	      pAb &&
	      pBox->BxType != BoFloatGhost &&
	      (pAb->AbLeafType == LtText ||
	       (/*pAb->AbFloat == 'N' &&*/
		pAb->AbLeafType == LtCompound &&
		((pAbRef == NULL &&
		  pAb->AbWidth.DimUnit == UnPercent) ||
		(pAbRef &&
	      (pAbRef == pBlock->BxAbstractBox ||
	       pAbRef->AbBox->BxType == BoGhost ||
	       pAbRef->AbBox->BxType == BoFloatGhost))))));
  /* minimum width needed to format the line */
  if (variable && (pBox->BxType == BoBlock || pBox->BxType == BoFloatBlock))
    {
      width = pBox->BxMinWidth;
      if (width == 0)
	width = 20 + pBox->BxLMargin + pBox->BxLBorder + pBox->BxLPadding + pBox->BxRMargin + pBox->BxRBorder + pBox->BxRPadding;
    }
  else if (!variable && pBox)
    width = pBox->BxWidth;
  else if (pBox)
    width = 20 + pBox->BxLMargin + pBox->BxLBorder + pBox->BxLPadding + pBox->BxRMargin + pBox->BxRBorder + pBox->BxRPadding;
  else
    width = 20;

  if (floatL)
    {
      pfloatL = pBlock->BxLeftFloat;
      while (pfloatL && pfloatL->FlBox != floatL)
	pfloatL = pfloatL->FlNext;
    }
  if (floatR)
    {
      pfloatR = pBlock->BxRightFloat;
      while (pfloatR && pfloatR->FlBox != floatR)
	pfloatR = pfloatR->FlNext;
    }

  do
    {
      /* compute the line position and width */
      if (floatL &&
	  pLine->LiYOrg + orgY >= floatL->BxYOrg &&
	  pLine->LiYOrg + orgY < floatL->BxYOrg + floatL->BxHeight)
	{
	  /* line at the right of the current left float */
	  pLine->LiXOrg = floatL->BxXOrg + floatL->BxWidth + indent - orgX;
	  bottomL = floatL->BxYOrg + floatL->BxHeight - orgY;
	}
      else if (floatL &&
	       (pLine->LiYOrg + orgY < floatL->BxYOrg ||
		pLine->LiYOrg + orgY >= floatL->BxYOrg + floatL->BxHeight))
	{
	  /* look at all previous floating boxes */
	  while (pfloatL && pfloatL->FlBox &&
		 (pLine->LiYOrg + orgY < pfloatL->FlBox->BxYOrg ||
		  pLine->LiYOrg + orgY >= pfloatL->FlBox->BxYOrg + pfloatL->FlBox->BxHeight))
	    pfloatL = pfloatL->FlPrevious;
	  if (pfloatL)
	    {
	      /* line at the right of a previous left float */
	      floatL = pfloatL->FlBox;
	      pLine->LiXOrg = floatL->BxXOrg + floatL->BxWidth + indent - orgX;
	      bottomL = floatL->BxYOrg + floatL->BxHeight - orgY;
	    }
	  else
	    floatL = NULL;
	}
      if (floatL == NULL)
	{
	  /* line at left of the block */
	  pLine->LiXOrg = left + indent;
	  bottomL = pLine->LiYOrg;
	}

      if (floatR &&
	  pLine->LiYOrg + orgY >= floatR->BxYOrg &&
	   pLine->LiYOrg + orgY < floatR->BxYOrg + floatR->BxHeight)
	{
	  /* line extended to the left edge of the current right float */
	  pLine->LiXMax = floatR->BxXOrg - pLine->LiXOrg - orgX;
	  bottomR = floatR->BxYOrg + floatR->BxHeight - orgY;
	}
      else if (floatR &&
	       (pLine->LiYOrg + orgY < floatR->BxYOrg ||
		pLine->LiYOrg + orgY >= floatR->BxYOrg + floatR->BxHeight))
	{
	  /* look at all previous floating boxes */
	  while (pfloatR && pfloatR->FlBox &&
		 (pLine->LiYOrg + orgY < pfloatR->FlBox->BxYOrg ||
		  pLine->LiYOrg + orgY >= pfloatR->FlBox->BxYOrg + pfloatR->FlBox->BxHeight))
	    pfloatR = pfloatR->FlPrevious;
	  if (pfloatR)
	    {
	      /* line extended to the left edge of a previous right float */
	      floatR = pfloatR->FlBox;
	      pLine->LiXMax = floatR->BxXOrg - pLine->LiXOrg - orgX;
	      bottomR = floatR->BxYOrg + floatR->BxHeight - orgY;
	    }
	  else
	    floatR = NULL;
	}
      if (floatR == NULL)
	{
	  /* line extended to the right edge of the block */
	  pLine->LiXMax = pBlock->BxW + left - pLine->LiXOrg;
	  bottomR = pLine->LiYOrg;
	}
  
      /* check if there is enough space between left and right floating boxes */
      if ((floatL || floatR) &&
	  (pLine->LiXMax < width ||
	   (floatL && clearL == 'L') || (floatR && clearR == 'R')))
	{
	  /* update line information */
	  if ((newFloat || clearL == 'L') && pLine->LiYOrg < bottomL)
	    pLine->LiYOrg = bottomL;
	  else if (bottomR < bottomL && pLine->LiXMax < width)
	    pLine->LiYOrg = bottomL;
	  if ((newFloat || clearR == 'R') && pLine->LiYOrg < bottomR)
	    pLine->LiYOrg = bottomR;
	  else if (bottomL < bottomR && pLine->LiXMax < width)
	    pLine->LiYOrg = bottomR;
	  if (pBlock->BxType == BoFloatBlock)
	    {
	      /* it's not an inline block,
		 then the content is moved to the bottom of the block */
	      clearL = 'L';
	      clearR = 'R';
	    }
	  pLine->LiXOrg = left + indent;
	  pLine->LiXMax = pBlock->BxW - indent;
	  /* check previous floating box to set the x position */
	  if (floatL)
	    {
	      /* look for another floating box that will define the line position */
	      y = floatL->BxYOrg + floatL->BxHeight;
	      while (pfloatL && pfloatL->FlBox &&
		     pfloatL->FlBox->BxYOrg + pfloatL->FlBox->BxHeight <= y)
		    pfloatL = pfloatL->FlPrevious;
	      if (pfloatL)
		floatL = pfloatL->FlBox;
	      else
		floatL = NULL;
	    }

	  /* check previous floating box to set the width */
	  if (floatR)
	    {
	      /* look for another floating box that will define the line position */
	      y = floatR->BxYOrg + floatR->BxHeight;
	      while (pfloatR && pfloatR->FlBox &&
		     pfloatR->FlBox->BxYOrg + pfloatR->FlBox->BxHeight <= y)
		    pfloatR = pfloatR->FlPrevious;
	      if (pfloatR)
		floatR = pfloatR->FlBox;
	      else
		floatR = NULL;
	    }
	  pLine->LiXMax -= pLine->LiXOrg;
	}
      else
	{
	  /* line position and width are correct know */
	  floatL = NULL;
	  floatR = NULL;
	}
    }
  while (floatL || floatR);
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
  FillLine fills a line pLine of a block of lines pBlock with enclosed boxes.
  Fields LiFirstBox and LiFirstPiece must be set by the caller.
  Fields LiLastBox, LiLastPiece will be set by the function.
  Parameters floatL and floatR point to last left an right floating
  boxes managed in the current block.
  Work with absolute positions when xAbs and yAbs are TRUE.
  Returns:
  - the minimum width of the line (the larger word).
  - full = TRUE if the line is full.
  - adjust = TRUE if the line must be justified.
  - notComplete = TRUE if all enclosed boxes are not managed yet.
  - breakLine = TRUE if the end of the line correspond to a break element.
  ----------------------------------------------------------------------*/
static int FillLine (PtrLine pLine, PtrBox pBlock, PtrAbstractBox pRootAb,
		     ThotBool xAbs, ThotBool yAbs,
		     ThotBool notComplete, ThotBool *full, ThotBool *adjust,
		     ThotBool *breakLine, int frame, int indent,
		     PtrBox *floatL, PtrBox *floatR)
{
  PtrTextBuffer       pNewBuff;
  PtrAbstractBox      pAbRef;
  PtrBox              pBox;
  PtrBox              pNextBox;
  PtrBox              lastbox;
  int                 ascent, descent;
  int                 width, breakWidth;
  int                 boxLength, nSpaces;
  int                 newIndex, wordWidth;
  int                 xi, val;
  int                 maxLength, minWidth;
  ThotBool            still;
  ThotBool            toCut;
  ThotBool            found;

  *adjust = TRUE;
  *breakLine = FALSE;
  *full = FALSE;
  toCut = FALSE;
  found = FALSE;
  still = FALSE;
  minWidth = 0;
  wordWidth = 0;
  xi = 0;
  ascent = 0;
  descent = 0;
  /* the first managed box */
  pNextBox = pLine->LiFirstBox;
  InitLine (pLine, pBlock, indent, *floatL, *floatR, pNextBox, xAbs, yAbs);
  pLine->LiLastPiece = NULL;
  pBox = NULL;
  if (pNextBox)
    {
      /* pNextBox is the current box we're managing */
      /* pBox is the last box added to the line */
      *full = TRUE;
      still = TRUE;
      if (pLine->LiFirstPiece == NULL &&
	  (pNextBox->BxType == BoSplit || pNextBox->BxType == BoMulScript))
	pLine->LiFirstPiece = pNextBox->BxNexChild;
      if (pLine->LiFirstPiece)
	{
	  /* the first piece must be inserted in the line */
	  pBox = pLine->LiFirstPiece;
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
			       nSpaces, newIndex, pNewBuff, pRootAb);
	    }
	  else if (pBox->BxWidth + xi <= pLine->LiXMax)
	    {
	      if (pBox->BxNexChild)
		/* get the next child */
		pNextBox = pBox->BxNexChild;
	      else
		{
		  /* the whole box can be inserted in the line */
		  pNextBox = GetNextBox (pBox->BxAbstractBox);
		  if (pNextBox == NULL)
		    {
		      *full = FALSE;
		      still = FALSE;
		    }
		}
	    }
	  else
	    {
	      /* We need to split again that box */
	      still = FALSE;
	      BreakPieceOfBox (pLine, pBox, pLine->LiXMax - xi, pRootAb);
	    }
	  pBox = pLine->LiFirstPiece;
	  xi += pBox->BxWidth;
	  /* take into account the minimum width */
	  if (!toCut && minWidth < wordWidth)
	    minWidth = wordWidth;
	}
    }

  /* look for a box to split */
  while (still)
    {
      val = 0;
      if (pNextBox->BxAbstractBox->AbLeafType == LtCompound)
	{
	  /* check if the width depends on the block width */
	  pAbRef = pNextBox->BxAbstractBox->AbWidth.DimAbRef;
	  if ((pAbRef == NULL &&
	       pNextBox->BxAbstractBox->AbWidth.DimUnit == UnPercent) ||
	      (pAbRef &&
	      (pAbRef == pBlock->BxAbstractBox ||
	       pAbRef->AbBox->BxType == BoGhost ||
	       pAbRef->AbBox->BxType == BoFloatGhost)))
	    {
	      /* just to be sure the line structure is coherent */
	      pLine->LiLastBox = pLine->LiFirstBox;
	      if (pNextBox->BxAbstractBox->AbFloat == 'N')
		val = pLine->LiXMax;
	      else
		val = pBlock->BxW;
	      if (pNextBox->BxAbstractBox->AbWidth.DimUnit == UnPercent)
		val = val * pNextBox->BxAbstractBox->AbWidth.DimValue / 100;
	      ResizeWidth (pNextBox, pBlock, NULL,
			   val - pNextBox->BxWidth, 0, 0, 0, frame);
	      /* recheck if the line could be moved under floating boxes */
	      InitLine (pLine, pBlock, indent, *floatL, *floatR, pNextBox, xAbs, yAbs);
	    }
	}

      if (pNextBox->BxAbstractBox->AbFloat != 'N' ||
	  !pNextBox->BxAbstractBox->AbHorizEnclosing ||
	  pNextBox->BxAbstractBox->AbNotInLine)
	{
	  /* that box is not inline */
	  if (pNextBox->BxAbstractBox->AbFloat != 'N')
	    {
	      /* skip over the box */
	      lastbox = pNextBox;
	      pNextBox = GetNextBox (lastbox->BxAbstractBox);
	      if (lastbox == pLine->LiFirstBox)
		pLine->LiFirstBox = pNextBox;
	      if ((lastbox->BxAbstractBox->AbFloat == 'L' &&
		   !IsFloatSet (lastbox, *floatL, pBlock)) ||
		  (lastbox->BxAbstractBox->AbFloat == 'R' &&
		   !IsFloatSet (lastbox, *floatR, pBlock)))
		{
		  if ((pBlock->BxType == BoFloatBlock ||
		       lastbox->BxAbstractBox->AbClear != 'N') &&
		      pLine->LiFirstBox != pNextBox)
		    {
		      /* report the floating box to the next line */
		      *full = TRUE;
		      still = FALSE;
		    }
		  else
		    /* handle a new floating box and rebuild the line */
		    return SetFloat (lastbox, pBlock, pLine, pRootAb, xAbs, yAbs,
				     notComplete, full, adjust, breakLine,
				     frame, indent, floatL, floatR);
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
	    /* skip over the box */
	    pNextBox = GetNextBox (pNextBox->BxAbstractBox);
	}
      else
	{
	  /* look for a break element */
	  if (pNextBox->BxAbstractBox->AbAcceptLineBreak)
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
		/* just the Break character in the box */
		/* it's not necessary to cut it */
		pBox = pNextBox;
	      else
		{
		  ManageBreakLine (pNextBox, width, breakWidth, boxLength,
				   nSpaces, newIndex, pNewBuff, pRootAb);
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
	  else if (pNextBox->BxWidth + xi <= pLine->LiXMax)
	    {
	      /* the whole box can be inserted in the line */
	      if (pNextBox->BxType == BoBlock ||
		  pNextBox->BxType == BoFloatBlock ||
		  pNextBox->BxType == BoTable)
		wordWidth = pNextBox->BxMinWidth;
	      else if (pNextBox->BxAbstractBox->AbLeafType != LtText &&
		       !pNextBox->BxAbstractBox->AbWidth.DimIsPosition &&
		       pNextBox->BxAbstractBox->AbHorizEnclosing &&
		       (pNextBox->BxAbstractBox->AbWidth.DimAbRef == NULL ||
			!IsParentBox (pNextBox->BxAbstractBox->AbWidth.DimAbRef->AbBox, pNextBox)))
		wordWidth = pNextBox->BxWidth;
	      pBox = pNextBox;
	      xi += pNextBox->BxWidth;
	      if (pNextBox->BxAbstractBox->AbLeafType == LtText &&
		  pNextBox->BxNexChild)
		/* get the next child */
		pNextBox = pNextBox->BxNexChild;
	      else
		pNextBox = GetNextBox (pNextBox->BxAbstractBox);
	      if (!pBox->BxAbstractBox->AbElement->ElTerminal &&
		  (pNextBox == NULL ||
		   (pNextBox->BxAbstractBox->AbLeafType == LtText &&
		    xi == pLine->LiXMax) ||
		   (pNextBox->BxAbstractBox->AbLeafType != LtText &&
		    pNextBox->BxWidth + xi > pLine->LiXMax)))
		/* accept to cut the line here */ 
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
      wordWidth = 0;
    }

  if (toCut)
    {
      /* Try to break the box pNextBox or a previous one */
      maxLength = pLine->LiXMax - xi;
      if (pNextBox == pLine->LiFirstBox)
	{
	  /* There is only one box in the line */
	  if (!pNextBox->BxAbstractBox->AbAcceptLineBreak ||
	      pNextBox->BxAbstractBox->AbLeafType != LtText)
	    {
	      /* cannot break the box */
	      pBox = pNextBox;	/* it's also the last box */
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
		      lastbox = GetNextBox (lastbox->BxAbstractBox);
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
	      BreakMainBox (pLine, pNextBox, maxLength, pRootAb, TRUE);
	      pBox = pNextBox;
	      if (pBox->BxAbstractBox->AbLeafType == LtText && pBox->BxNexChild)
		{
		  /* we have a new box */
		  if (pBox->BxType != BoScript && pBox->BxNexChild)
		    /* take the first child of a main box */
		    pBox = pBox->BxNexChild;
		  pLine->LiFirstPiece = pBox;
		}
	    }
	}
      else
	{
	  /* There is a previous box in the line */
	  if (!pNextBox->BxAbstractBox->AbElement->ElTerminal &&
	      pNextBox->BxWidth == pLine->LiXMax)
	    {
	      /* cut before that box */
	      toCut = FALSE;
	      pBox = GetPreviousBox (pNextBox->BxAbstractBox);
	    }
	  else if (pNextBox->BxAbstractBox->AbLeafType == LtText &&
	      pNextBox->BxAbstractBox->AbAcceptLineBreak &&
	      (CanHyphen (pNextBox) || pNextBox->BxNSpaces != 0))
	    {
	      /* Break that box */
	      BreakMainBox (pLine, pNextBox, maxLength, pRootAb, FALSE);
	      pBox = pNextBox;
	      if (pNextBox->BxAbstractBox->AbLeafType == LtText &&
		  pNextBox->BxNexChild)
		{
		  /* we have a new box */
		  if (pNextBox->BxType != BoScript && pNextBox->BxNexChild)
		    /* take the first child of a main box */
		    pBox = pNextBox->BxNexChild;
		  else
		    pBox = pNextBox;
		  toCut = FALSE;
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
		    pNextBox = GetPreviousBox (pNextBox->BxAbstractBox);
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
		  /* break on the last space of the box*/
		  if (pNextBox->BxType == BoPiece ||
		      pNextBox->BxType == BoScript)
		    {
		      BreakPieceOfBox (pLine, pNextBox, pNextBox->BxW - 1, pRootAb);
		      pBox = pNextBox;
		      toCut = FALSE;
		    }
		  else
		    {
		      BreakMainBox (pLine, pNextBox, pNextBox->BxW - 1, pRootAb, FALSE);
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
		      BreakMainBox (pLine, lastbox, maxLength, pRootAb, TRUE);
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
		      pBox = GetPreviousBox (lastbox->BxAbstractBox);
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
  if (pLine->LiFirstPiece)
    pNextBox = pLine->LiFirstPiece;
  else
    pNextBox = pLine->LiFirstBox;

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
	      (!pNextBox->BxAbstractBox->AbNotInLine ||
	       pNextBox->BxAbstractBox->AbDisplay != 'U'))
	    AddBoxInLine (pNextBox, &descent, &ascent, pLine);
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
		  pNextBox = GetNextBox (pNextBox->BxAbstractBox);
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
	  GetNextBox (pBox->BxAbstractBox) == NULL)
	*full = FALSE;

      /* coupe les blancs en fin de ligne pleine */
      if ((notComplete || *full) &&
	  pBox->BxAbstractBox->AbLeafType == LtText &&
	  pBox->BxNSpaces != 0)
	{
	  if (pLine->LiLastPiece == NULL)
	    {
	      maxLength = pBox->BxWidth;
	      /*coupure sur un caractere refusee */
	      BreakMainBox (pLine, pBox, maxLength, pRootAb, FALSE);
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
	      BreakPieceOfBox (pLine, pBox, maxLength, pRootAb);
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
   if (pBox->BxSpaceWidth != 0)
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
void InitFloats (PtrBox pBlock, PtrLine pLine, PtrBox *floatL, PtrBox *floatR)
{
  PtrFloat            pfloat;
  int                 y;

  if (pLine)
    y = pLine->LiYOrg;
  else
    y = 0;
  pfloat = pBlock->BxLeftFloat;
  while (pfloat && pfloat->FlBox && pfloat->FlBox->BxYOrg < y)
    pfloat = pfloat->FlNext;
  if (pfloat)
    *floatL = pfloat->FlBox;
  else
    *floatL = NULL;
  pfloat = pBlock->BxRightFloat;
  while (pfloat && pfloat->FlBox && pfloat->FlBox->BxYOrg < y)
    pfloat = pfloat->FlNext;
  if (pfloat)
    *floatR = pfloat->FlBox;
  else
    *floatR = NULL;
}

/*----------------------------------------------------------------------
  UpdateBlockWithFloat updates the block of lines pBlock to take into
  account floating boxes.
  Updates the minimum and maximum widths of the block when the parameter
  updateWidth is TRUE.
  Returns the updated height.
  ----------------------------------------------------------------------*/
static void UpdateBlockWithFloat (PtrBox pBlock, ThotBool xAbs, ThotBool yAbs,
				  ThotBool updateWidth, int *height)
{
  PtrFloat            pfloat;
  int                 y, x, x1, x2;

  if (xAbs)
    x = pBlock->BxXOrg;
  else
    x = 0;
  x += pBlock->BxLMargin + pBlock->BxLBorder + pBlock->BxLPadding;
  x1 = x2 = 0;
  if (yAbs)
    y = pBlock->BxYOrg;
  else
    y = 0;
  y += pBlock->BxTMargin + pBlock->BxTBorder + pBlock->BxTPadding;
  pfloat = pBlock->BxLeftFloat;
  while (pfloat && pfloat->FlBox)
    {
      if (pfloat->FlBox->BxXOrg + pfloat->FlBox->BxWidth - x > x1)
	/* float change the minimum width of the block */
	x1 = pfloat->FlBox->BxXOrg + pfloat->FlBox->BxWidth - x;
      if (pfloat->FlBox->BxYOrg + pfloat->FlBox->BxHeight - y > *height)
	/* float change the height of the block */
	*height = pfloat->FlBox->BxYOrg + pfloat->FlBox->BxHeight - y;
      pfloat = pfloat->FlNext;
    }
  pfloat = pBlock->BxRightFloat;
  while (pfloat && pfloat->FlBox)
    {
      if (pBlock->BxW - pfloat->FlBox->BxXOrg - x > x2)
	/* float change the minimum width of the block */
	x2 = pBlock->BxW - pfloat->FlBox->BxXOrg - x;
      if (pfloat->FlBox->BxYOrg + pfloat->FlBox->BxHeight - y > *height)
	/* float change the height of the block */
	*height = pfloat->FlBox->BxYOrg + pfloat->FlBox->BxHeight - y;
      pfloat = pfloat->FlNext;
    }
  if (updateWidth)
    {
      /* update min and max widths */
      x1 += x2;
      pBlock->BxMinWidth += x1;
      pBlock->BxMaxWidth += x1;
    }
}


/*----------------------------------------------------------------------
  SetFloat computes the position of the floating box box in block of lines
  pBlock.
  Work with absolute positions when xAbs and yAbs are TRUE.
  Parameters floatL and floatR point to last left an right floating
  boxes in the current block.
  ----------------------------------------------------------------------*/
int SetFloat (PtrBox box, PtrBox pBlock, PtrLine pLine, PtrAbstractBox pRootAb,
	      ThotBool xAbs, ThotBool yAbs,
	      ThotBool notComplete, ThotBool *full, ThotBool *adjust,
	      ThotBool *breakLine, int frame, int indent, PtrBox *floatL,
	      PtrBox *floatR)
{
  PtrBox              boxPrev = NULL;
  int                 x, y;

  /* look for the box in the right float list */
  if (box->BxAbstractBox->AbFloat == 'L')
    {
      /* left float */
      if (box->BxAbstractBox->AbClear != 'L' &&
	  box->BxAbstractBox->AbClear != 'B')
	boxPrev = *floatL;
      if (boxPrev &&
	  boxPrev->BxYOrg + boxPrev->BxHeight >= pLine->LiYOrg &&
	  box->BxWidth <= pLine->LiXMax)
	{
	  /* the box is set at the right of the previous box */
	  x = boxPrev->BxXOrg + boxPrev->BxWidth;
	  y = boxPrev->BxYOrg;
	}
      else
	{
	  /* the box is set at the left of the parent box */
	  x = pBlock->BxLMargin + pBlock->BxLBorder + pBlock->BxLPadding;
	  if (boxPrev)
	    /* the box is set at the bottom of the previous box */
	    y = boxPrev->BxYOrg + boxPrev->BxHeight;
	  else if (pLine /*&& pLine->LiPrevious*/)
	    /* the box is set at the top of the parent box */
	    /*y = pLine->LiPrevious->LiYOrg + pLine->LiPrevious->LiHeight;*/
	    y = pLine->LiYOrg;
	  else
	    /* the box is set at the top of the parent box */
	    y = pBlock->BxTMargin + pBlock->BxTBorder + pBlock->BxTPadding;
	  if (xAbs)
	    x += pBlock->BxXOrg;
	  if (yAbs && !boxPrev)
	    y += pBlock->BxYOrg;
	  boxPrev = NULL;
	}
      XMove (box, NULL, x - box->BxXOrg, frame);
      YMove (box, NULL, y - box->BxYOrg, frame);
      *floatL = box;
    }
  else
    {
      /* right float */
      if (box->BxAbstractBox->AbClear != 'R' &&
	  box->BxAbstractBox->AbClear != 'B')
	boxPrev = *floatR;
     if (boxPrev &&
	  boxPrev->BxYOrg + boxPrev->BxHeight >= pLine->LiYOrg &&
	  box->BxWidth <= pLine->LiXMax - pLine->LiXOrg)
	{
	  /* the box is set at the left of the previous box */
	  x = boxPrev->BxXOrg - box->BxWidth;
	  y = boxPrev->BxYOrg;
	}
      else
	{
	  /* the box is set at the right of the parent box */
	  x = pBlock->BxW - box->BxWidth;
	  if (boxPrev)
	    /* the box is set at the bottom of the previous box */
	    y = boxPrev->BxYOrg + boxPrev->BxHeight;
	  else if (pLine /*&& pLine->LiPrevious*/)
	    /* the box is set at the top of the parent box */
	    /*y = pLine->LiPrevious->LiYOrg + pLine->LiPrevious->LiHeight;*/
	    y = pLine->LiYOrg;
	  else
	    /* the box is set at the top of the parent box */
	    y = pBlock->BxTMargin + pBlock->BxTBorder + pBlock->BxTPadding;
	  boxPrev = NULL;
	  if (xAbs)
	    x += pBlock->BxXOrg;
	  if (yAbs && !boxPrev)
	    y += pBlock->BxYOrg;
	}
      XMove (box, NULL, x - box->BxXOrg, frame);
      YMove (box, NULL, y - box->BxYOrg, frame);
      *floatR = box;
    }
  return FillLine (pLine, pBlock, pRootAb, xAbs, yAbs, notComplete, full,
		   adjust, breakLine, frame, indent, floatL, floatR);
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

  pFrame = &ViewFrameTable[frame - 1];
  pViewSel = &pFrame->FrSelectionBegin;
  pViewSelEnd = &pFrame->FrSelectionEnd;
  if (pBox && pBox->BxAbstractBox)
    {
      pAb = pBox->BxAbstractBox;
      if (pAb && pAb->AbLeafType == LtText)
	{
	  x = BoxCharacterWidth (SPACE, pBox->BxFont);
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
		  if (pBox->BxType == BoDotted)
		    {
		      /* remove the hyphen width */
		      width -= BoxCharacterWidth (173, pBox->BxFont);
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
		      RemoveAdjustement (ibox1, x);
		      /* lost pixels */
		      diff = ibox1->BxFirstChar - lost;
		      if (diff > 0)
			{
			  nchar += diff;
			  /* add skipped spaces */
			  width += diff * x;
			  nspace += diff;
			}
		      else if (ibox1->BxType == BoDotted)
			/* remove the hyphen width */
			width -= BoxCharacterWidth (173, ibox1->BxFont);

		      pNextBox = ibox1->BxNext;
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
			  pBox->BxRMargin = pBox->BxRMargin;
			  pBox->BxRBorder = pBox->BxRBorder;
			  pBox->BxRPadding = pBox->BxRPadding;
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
#ifdef _GL
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
		      pBox->BxWidth = width + pBox->BxLMargin + pBox->BxLBorder + pBox->BxLPadding + pBox->BxRMargin + pBox->BxRBorder + pBox->BxRPadding;
		      pBox->BxNSpaces += nspace;
		    }
		  /* Update the chain of leaf boxes */
		  if (pBox->BxType == BoScript)
		    {
		      if (pNextBox && pNextBox->BxType == BoScript)
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
  PtrAbstractBox      pChildAb;
  PtrAbstractBox      pAb, pRootAb;
  PtrBox              pBoxToBreak, pNextBox;
  PtrBox              floatL, floatR;
  AbPosition         *pPosAb;
  int                 x, lineSpacing, indent;
  int                 org, width, noWrappedWidth;
  int                 lostPixels, minWidth;
  int                 top, left, right, spacing;
  ThotBool            toAdjust;
  ThotBool            breakLine;
  ThotBool            xAbs, yAbs;
  ThotBool            extensibleBox;
  ThotBool            full;
  ThotBool            still;
  ThotBool            standard;

  /* avoid any cycle */
  if (pBox->BxCycles > 0)
    {
      *height = pBox->BxHeight;
      return;
    }
  pBox->BxCycles++;

  /* Fill the block box */
  noWrappedWidth = 0;
  pAb = pBox->BxAbstractBox;
  extensibleBox = (pBox->BxContentWidth ||
		   (!pAb->AbWidth.DimIsPosition && pAb->AbWidth.DimMinimum));
  pNextBox = NULL;
  full = TRUE;
  top = pBox->BxTMargin + pBox->BxTBorder + pBox->BxTPadding;
  left = pBox->BxLMargin + pBox->BxLBorder + pBox->BxLPadding;
  right = pBox->BxRMargin + pBox->BxRBorder + pBox->BxRPadding;
  x = 0;
  floatL = NULL;
  floatR = NULL;
  pRootAb = ViewFrameTable[frame - 1].FrAbstractBox;
  /* check if the X, Y position is relative or absolute */
  IsXYPosComplete (pBox, &xAbs, &yAbs);
  if (pBox->BxFirstLine == NULL)
    {
      /* Build all the block of lines */
      /* reset the value of the width without wrapping */
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
	else if (pChildAb->AbDead || pChildAb->AbNew)
	  pChildAb = pChildAb->AbNext;
        else if  (pChildAb->AbNotInLine &&
		  pChildAb->AbDisplay == 'U'/* &&
		  pChildAb->AbFloat == 'N'*/)
		  pChildAb = pChildAb->AbNext;
	else if (pChildAb->AbBox->BxType == BoGhost ||
		 pChildAb->AbBox->BxType == BoFloatGhost)
	  /* go down into the hierarchy */
	  pChildAb = pChildAb->AbFirstEnclosed;
	else
	  {
	    /* keep the current box */
	    pNextBox = pChildAb->AbBox;
	    still = FALSE;
	  }
    }

  /* compute the line spacing */
  lineSpacing = PixelValue (pAb->AbLineSpacing, pAb->AbLineSpacingUnit,
			    pAb, ViewFrameTable[frame - 1].FrMagnification);
  /* space added at the top and bottom of the paragraph */
  spacing = lineSpacing - BoxFontHeight (pBox->BxFont);
  standard = (spacing >= 0);
  if (spacing > 0)
    spacing /= 2;
  else
    spacing = 0;
  width = pBox->BxW;
  if (width > BoxCharacterWidth (119, pBox->BxFont)/*'w'*/ || extensibleBox)
    {
      /* compute the indent */
      if (extensibleBox)
	{
	  indent = 0;
	  /* the real width will be know later */
	  width = 3000;
	  pBox->BxW = width;
	}
      else if (pAb->AbIndentUnit == UnPercent)
	indent = PixelValue (pAb->AbIndent, UnPercent,
				 (PtrAbstractBox) width, 0);
      else
	indent = PixelValue (pAb->AbIndent, pAb->AbIndentUnit, pAb,
				 ViewFrameTable[frame - 1].FrMagnification);
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
	  if (pBoxToBreak && pBoxToBreak->BxNexChild)
	    pBoxToBreak = pBoxToBreak->BxNexChild;
	  else
	    pNextBox = GetNextBox (pChildAb);
	  
	  if (pNextBox == NULL)
	    /* nothing else */
	    full = FALSE;
	  else
	    {
	      /* prepare the next line */
	      GetLine (&pLine);
	      prevLine->LiNext = pLine;
	      /* look for previous floating boxes */
	      InitFloats (pBox, pLine, &floatL, &floatR);
	    }
	}

      /* Insert new lines as long as they are full */
      /* ----------------------------------------- */
      while (full)
	{
	  /* Initialize a new line */
	  pLine->LiPrevious = prevLine;
	  pLine->LiYOrg = *height + top;
	  if (pNextBox->BxAbstractBox->AbFloat == 'N' &&
	      (!pNextBox->BxAbstractBox->AbHorizEnclosing ||
	      (pNextBox->BxAbstractBox->AbNotInLine &&
	       pNextBox->BxAbstractBox->AbDisplay != 'U')))
	    {
	      /* The current box escape the rule line */
	      pLine->LiFirstBox = pNextBox;
	      pLine->LiLastBox = pNextBox;
	      pLine->LiFirstPiece = NULL;
	      pLine->LiLastPiece = NULL;
	      /* It's placed under the previous line */
	      InitLine (pLine, pBox, indent, floatL, floatR, pNextBox, xAbs, yAbs);
	      if (extensibleBox)
		/* no limit for an extensible line */
		pLine->LiXMax = 3000;
	      pLine->LiHeight = pNextBox->BxHeight;
	      pLine->LiRealLength = pNextBox->BxWidth;
	      /* the paragraph should be large enough
		 ( for math with display:block by example) */
	      if (pNextBox->BxAbstractBox->AbNotInLine &&
		   pNextBox->BxAbstractBox->AbDisplay != 'U')
		{
		  if (pNextBox->BxWidth > pBox->BxMaxWidth)
		    pBox->BxMaxWidth = pNextBox->BxWidth;
		  if (pBox->BxMinWidth < pNextBox->BxWidth)
		    pBox->BxMinWidth = pNextBox->BxWidth;
		  pLine->LiXOrg = left;
		  x = left;
		  if (pNextBox->BxAbstractBox->AbHorizPos.PosEdge == VertMiddle &&
		      pNextBox->BxAbstractBox->AbHorizPos.PosRefEdge == VertMiddle &&
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
	  else if (!pNextBox->BxAbstractBox->AbNotInLine)
	    {
	      /* line indent */
	      pLine->LiXOrg = left;
	      if (prevLine || pAb->AbTruncatedHead || indent >= width)
		indent = 0;
	      pLine->LiFirstBox = pNextBox;
	      pLine->LiFirstPiece = pBoxToBreak;
	      /* Fill the line */
	      minWidth = FillLine (pLine, pBox, pRootAb, xAbs, yAbs,
				   pAb->AbTruncatedTail,
				   &full, &toAdjust, &breakLine, frame,
				   indent, &floatL, &floatR);
	      if (pBox->BxMinWidth < minWidth)
		pBox->BxMinWidth = minWidth;
	      if (prevLine)
		{
		  /* initial position */
		  org = prevLine->LiYOrg + prevLine->LiHeight;
		  if (pLine->LiYOrg == org)
		    {
		      /* line position not updated by floating boxes */
		      /* position when line spacing applies */
		      org = prevLine->LiYOrg + prevLine->LiHorizRef + lineSpacing - pLine->LiHorizRef;
		      if (org > pLine->LiYOrg ||
			  (!prevLine->LiNoOverlap && !standard))
			/* apply the rule of line spacing */
			pLine->LiYOrg = org;
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
		      lostPixels = lostPixels * BoxCharacterWidth (SPACE, pBoxToBreak->BxFont);
		      noWrappedWidth += lostPixels;
		    }
		  if (pLine->LiLastPiece->BxType == BoDotted)
		    /* remove the dash width */
		    noWrappedWidth -= BoxCharacterWidth (173, pBoxToBreak->BxFont);
		}
	      if (breakLine || !full)
		{
		  if (noWrappedWidth > pBox->BxMaxWidth)
		    pBox->BxMaxWidth = noWrappedWidth;
		  noWrappedWidth = 0;
		}

	      /* Take into account the text-align */
	      if (toAdjust &&
		  (full || pAb->AbTruncatedTail || pLine->LiRealLength > pLine->LiXMax) &&
		  pAb->AbAdjust == AlignJustify)
		Adjust (pBox, pLine, frame, xAbs, yAbs);
	      else
		{
		  if (!pAb->AbWidth.DimIsPosition && pAb->AbWidth.DimMinimum &&
		      pLine->LiRealLength > pBox->BxW)
		    {
		      pBox->BxContentWidth = TRUE;
		      pLine->LiXMax = pLine->LiRealLength;
		    }
		  if (!pBox->BxContentWidth && !extensibleBox)
		    Align (pBox, pLine, frame, TRUE, xAbs, yAbs);
		  else
		    Align (pBox, pLine, frame, FALSE, xAbs, yAbs);
		}
	    }

	  if (pLine->LiLastBox)
	    {
	      pNextBox = pLine->LiLastBox;
	      do
		if (pNextBox->BxType == BoScript && pNextBox->BxNexChild)
		  /* get the next child */
		  pNextBox = pNextBox->BxNexChild;
		else
		  pNextBox = GetNextBox (pNextBox->BxAbstractBox);
	      while (pNextBox &&
		     pNextBox->BxAbstractBox->AbFloat == 'N' &&
		     pNextBox->BxAbstractBox->AbNotInLine &&
		     pNextBox->BxAbstractBox->AbDisplay == 'U');
	    }
	  else
	    pNextBox = NULL;
	  
	  /* is there a breaked box */
	  if (pBoxToBreak != NULL)
	    {
	    /* is it empty ? */
	    if (pBoxToBreak->BxNChars > 0)
	      pNextBox = pLine->LiLastBox;
	    else if (pNextBox == NULL)
	      pNextBox = pLine->LiLastBox;
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
      /* met a jour la base du bloc de lignes   */
      /* s'il depend de la premiere boite englobee */
      if (pAb->AbHorizRef.PosAbRef == pAb->AbFirstEnclosed && pBox->BxFirstLine)
	{
	  pPosAb = &pAb->AbHorizRef;
	  x = PixelValue (pPosAb->PosDistance, pPosAb->PosUnit, pAb,
			  ViewFrameTable[frame - 1].FrMagnification);
	  x += top;
	  MoveHorizRef (pBox, NULL,
			pBox->BxFirstLine->LiHorizRef + x - pBox->BxHorizRef, frame);
	}
    }
  else
    {
      *height = BoxFontHeight (pBox->BxFont) + spacing;
      /* compute minimum and maximum width of the paragraph
	 with no limit as an extensible line*/
      pBox->BxW = 3000;
      if (pNextBox && !pNextBox->BxAbstractBox->AbHorizEnclosing)
	do
	  if (pNextBox->BxType == BoScript && pNextBox->BxNexChild)
	    /* get the next child */
	    pNextBox = pNextBox->BxNexChild;
	  else
	    pNextBox = GetNextBox (pNextBox->BxAbstractBox);
	while (pNextBox && pNextBox->BxAbstractBox->AbNotInLine);
      if (pNextBox)
	{
	  GetLine (&pLine);
	  do
	    {	      
	      /* no limit as an extensible line */
	      pLine->LiFirstBox = pNextBox;
	      pLine->LiFirstPiece = NULL;
	      pLine->LiLastBox = NULL;
	      pLine->LiLastPiece = NULL;
	      minWidth = FillLine (pLine, pBox, pRootAb, xAbs, yAbs,
				   pAb->AbTruncatedTail,
				   &full, &toAdjust, &breakLine, frame,
				   0, &floatL, &floatR);
	      if (pBox->BxMinWidth < minWidth)
		pBox->BxMinWidth = minWidth;
	      if (breakLine)
		{
		  if (pLine->LiLastBox->BxType == BoScript &&
		      pLine->LiLastBox->BxNexChild)
		    /* get the next child */
		    pNextBox = pLine->LiLastBox->BxNexChild;
		  else
		    pNextBox = GetNextBox (pLine->LiLastBox->BxAbstractBox);
		}
	      if (full)
		{
		  /* the last box has been broken */
		  full = FALSE;
		  still = FALSE;
		  RemoveBreaks (pLine->LiLastBox, frame, FALSE, &full, &still);
		}
	      if (pBox->BxMaxWidth < pLine->LiRealLength)
		pBox->BxMaxWidth = pLine->LiRealLength;
	      if (pLine->LiHeight > *height)
		*height = pLine->LiHeight;
	    }
	  while (breakLine && pNextBox);

	  FreeLine (pLine);
	}
      /* restore the right width */
      pBox->BxW = width;
    }
  /* now add margins, borders and paddings to min and max widths */
  pBox->BxMinWidth += left + right;
  pBox->BxMaxWidth += left + right;
  UpdateBlockWithFloat (pBox, xAbs, yAbs, TRUE, height);
  *height = *height + spacing;
  /* restore the value */
  pBox->BxCycles--;
}


/*----------------------------------------------------------------------
  ShiftLine moves the line contents of x pixels.
  ----------------------------------------------------------------------*/
static void ShiftLine (PtrLine pLine, PtrAbstractBox pAb, PtrBox pBox,
		       int x, int frame)
{
   PtrBox              ibox1, pSaveBox;
   PtrBox              pLastBox;
   ViewFrame          *pFrame;
   int                 xd, xf;
   int                 yd, yf;
   int                 i;
   ThotBool            status;

   pLastBox = NULL;
   pSaveBox = pBox;
   pLine->LiRealLength += x;
   /* prepare le reaffichage de la ligne */
   status = ReadyToDisplay;
   ReadyToDisplay = FALSE;
   /* Bornes de reaffichage a priori */
   pFrame = &ViewFrameTable[frame - 1];
   xd = pFrame->FrClipXBegin;
   xf = pFrame->FrClipXEnd;
   yd = pSaveBox->BxYOrg + pSaveBox->BxHorizRef - pLine->LiHorizRef;
   yf = yd + pLine->LiHeight;

   /* reaffiche la fin de la ligne */
   if (pAb->AbAdjust == AlignLeft || pAb->AbAdjust == AlignJustify ||
       pAb->AbAdjust == AlignLeftDots ||
       /* on force le cadrage a gauche si le bloc de lignes prend la largeur */
       /* du contenu */
       pAb->AbBox->BxContentWidth)
     {
	if (pLine->LiLastPiece == NULL)
	   pLastBox = pLine->LiLastBox;
	else
	   pLastBox = pLine->LiLastPiece;
	if (pLastBox && pLastBox != pBox)
	  {
	     xf = pLastBox->BxXOrg + pLastBox->BxWidth;
	     if (x > 0)
		xf += x;
	  }

	/* Note la largeur de la fin de bloc si le remplissage est demande! */

	if (pAb->AbAdjust == AlignLeftDots && pLine->LiNext == NULL)
	   pLastBox->BxEndOfBloc -= x;
     }
   /* reaffiche toute la ligne */
   else if (pAb->AbAdjust == AlignCenter)
     {
	i = x;
	x = (pLine->LiXMax - pLine->LiRealLength) / 2 - (pLine->LiXMax + i - pLine->LiRealLength) / 2;
	if (pLine->LiFirstPiece == NULL)
	   ibox1 = pLine->LiFirstBox;
	else
	   ibox1 = pLine->LiFirstPiece;
	xd = ibox1->BxXOrg;
	if (x < 0)
	   xd += x;
	if (pLine->LiLastPiece == NULL)
	   pLastBox = pLine->LiLastBox;
	else
	   pLastBox = pLine->LiLastPiece;
	if (pLastBox != pBox)
	     xf = pLastBox->BxXOrg + pLastBox->BxWidth;
	if (x < 0)
	   xf -= x;

	/* decale les boites precedentes */
	XMove (ibox1, NULL, x, frame);
	while (ibox1 != pBox)
	  {
	    if (ibox1->BxType == BoScript && ibox1->BxNexChild)
	      /* get the next child */
	      ibox1 = ibox1->BxNexChild;
	    else
	      ibox1 = GetNextBox (ibox1->BxAbstractBox);
	    if (ibox1->BxType == BoSplit || ibox1->BxType == BoMulScript)
	      ibox1 = ibox1->BxNexChild;
	    XMove (ibox1, NULL, x, frame);
	  }
	/* decale les boites suivantes */
	x = -x;
     }
   /* reaffiche le debut de la ligne */
   else if (pAb->AbAdjust == AlignRight)
     {
	x = -x;
	pLastBox = pBox;
	if (pLine->LiFirstPiece == NULL)
	   pBox = pLine->LiFirstBox;
	else
	   pBox = pLine->LiFirstPiece;
	xd = pBox->BxXOrg;
	if (x < 0)
	   xd += x;
	XMove (pBox, NULL, x, frame);
     }

   /* decale les boites suivantes */
   while (pBox != pLastBox && pBox)
     {
       if (pBox->BxType == BoScript && pBox->BxNexChild)
	 /* get the next child */
	 pBox = pBox->BxNexChild;
       else
	 pBox = GetNextBox (pBox->BxAbstractBox);
	if (pBox && pBox->BxNexChild &&
	    (pBox->BxType == BoSplit || pBox->BxType == BoMulScript))
	  pBox = pBox->BxNexChild;
	if (pBox && !pBox->BxAbstractBox->AbNotInLine)
	  XMove (pBox, NULL, x, frame);
     }
   DefClip (frame, xd, yd, xf, yf);
   ReadyToDisplay = status;
}


/*----------------------------------------------------------------------
   CompressLine compresse ou e'tend la ligne justifiee suite a`    
   l'ajout d'espaces et un ecart de xDelta pixels.         
  ----------------------------------------------------------------------*/
static void CompressLine (PtrLine pLine, int xDelta, int frame, int spaceDelta)
{
   PtrBox              ibox1, pBox;
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

   /* Faut-il changer la largeur des blancs ? */
   if (xDelta > 0)
      while (remainder < 0)
	{
	   remainder += nSpaces;
	   spaceValue++;
	}
   else
      while (remainder >= nSpaces)
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
   DefClip (frame, limit, pBox->BxYOrg + pBox->BxHorizRef - pLine->LiHorizRef,
	    limit + pLine->LiXMax,
     pBox->BxYOrg + pBox->BxHorizRef - pLine->LiHorizRef + pLine->LiHeight);

   /* met a jour chaque boite */
   do
     {
	if (pBox->BxType == BoSplit || pBox->BxType == BoMulScript)
	   ibox1 = pBox->BxNexChild;
	else
	   ibox1 = pBox;

	if (!pBox->BxAbstractBox->AbNotInLine)
	  {
	    XMove (ibox1, NULL, limit - ibox1->BxXOrg, frame);
	    if (ibox1->BxAbstractBox->AbLeafType == LtText && ibox1->BxNChars != 0)
	      {
		diff = ibox1->BxNSpaces * spaceValue;
		ibox1->BxW -= diff;
		ibox1->BxWidth -= diff;
		ibox1->BxSpaceWidth = pLine->LiSpaceWidth;
		
		/* Repartition des pixels */
		opixel = ibox1->BxNPixels;
		if (remainder > ibox1->BxNSpaces)
		  ibox1->BxNPixels = ibox1->BxNSpaces;
		else
		  ibox1->BxNPixels = remainder;
		ibox1->BxW = ibox1->BxW - opixel + ibox1->BxNPixels;
		ibox1->BxWidth = ibox1->BxWidth - opixel + ibox1->BxNPixels;
		remainder -= ibox1->BxNPixels;
		
		/* Faut-il mettre a jour les marques de selection ? */
		pViewSel = &ViewFrameTable[frame - 1].FrSelectionBegin;
		if (pViewSel->VsBox == ibox1)
		  {
		    pViewSel->VsXPos -= pViewSel->VsNSpaces * spaceValue;
		    if (opixel < pViewSel->VsNSpaces)
		      pViewSel->VsXPos -= opixel;
		    else
		      pViewSel->VsXPos -= pViewSel->VsNSpaces;
		    if (ibox1->BxNPixels < pViewSel->VsNSpaces)
		      pViewSel->VsXPos += ibox1->BxNPixels;
		    else
		      pViewSel->VsXPos += pViewSel->VsNSpaces;
		  }

		pViewSel = &ViewFrameTable[frame - 1].FrSelectionEnd;
		if (pViewSel->VsBox == ibox1)
		  {
		    pViewSel->VsXPos -= pViewSel->VsNSpaces * spaceValue;
		    if (opixel < pViewSel->VsNSpaces)
		      pViewSel->VsXPos -= opixel;
		    else
		      pViewSel->VsXPos -= pViewSel->VsNSpaces;
		    if (ibox1->BxNPixels < pViewSel->VsNSpaces)
		      pViewSel->VsXPos += ibox1->BxNPixels;
		    else
		      pViewSel->VsXPos += pViewSel->VsNSpaces;
		    if (pViewSel->VsIndBox < ibox1->BxNChars &&
			pViewSel->VsBuffer->BuContent[pViewSel->VsIndBuf] == SPACE)
		      pViewSel->VsXPos -= spaceValue;
		  }
	      }
	    limit += ibox1->BxWidth;
	  }
       if (ibox1->BxType == BoScript && ibox1->BxNexChild)
	 /* get the next child */
	 pBox = ibox1->BxNexChild;
       else
	 pBox = GetNextBox (ibox1->BxAbstractBox);
     }
   while (pBox && ibox1 != pLine->LiLastBox && ibox1 != pLine->LiLastPiece);
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
  PtrBox              ibox1;
  PtrLine             pNextLine;
  PtrLine             pLine;

  *changeSelectBegin = FALSE;
  *changeSelectEnd = FALSE;

  pLine = pFirstLine;
  if (pLine &&
      (pBox->BxType == BoBlock || pBox->BxType == BoFloatBlock))
    {
      if (pLine->LiFirstPiece &&
	  pLine->LiFirstBox->BxNexChild != pLine->LiFirstPiece)
	/* start with a piece of box */
	ibox1 = pLine->LiFirstPiece;
      else
	ibox1 = pLine->LiFirstBox;
      RemoveBreaks (ibox1, frame, removed, changeSelectBegin, changeSelectEnd);
      /* update the lines chaining */
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
	  FreeLine (pLine);
	  pLine = pNextLine;
	}
      
      /* Liberation des boites de coupure suivantes */
      if (ibox1)
	{
	  if (ibox1->BxType == BoScript && ibox1->BxNexChild)
	    /* get the next child */
	    ibox1 = ibox1->BxNexChild;
	  else
	    ibox1 = GetNextBox (ibox1->BxAbstractBox);
	}
      while (ibox1)
	{
	  RemoveBreaks (ibox1, frame, removed,
			changeSelectBegin, changeSelectEnd);
	  if (ibox1->BxType == BoScript && ibox1->BxNexChild)
	    /* get the next child */
	    ibox1 = ibox1->BxNexChild;
	  else
	    ibox1 = GetNextBox (ibox1->BxAbstractBox);
	}
    }
}

/*----------------------------------------------------------------------
  RecomputeLines recomputes a part or the whole block of lines after
  a change in the box ibox.
  ----------------------------------------------------------------------*/
void RecomputeLines (PtrAbstractBox pAb, PtrLine pFirstLine, PtrBox ibox,
		     int frame)
{
   Propagation         propagateStatus;
   CHAR_T              c;
   PtrLine             pLine;
   PtrBox              pBox;
   PtrBox              pSelBox;
   ViewFrame          *pFrame;
   ViewSelection      *pSelBegin;
   ViewSelection      *pSelEnd;
   int                 l, h, height;
   int                 width;
   ThotBool            changeSelectBegin;
   ThotBool            changeSelectEnd;
   ThotBool            status;


   /* Si la boite est eclatee, on remonte jusqu'a la boite bloc de lignes */
   while (pAb->AbBox->BxType == BoGhost || pAb->AbBox->BxType == BoFloatGhost)
      pAb = pAb->AbEnclosing;

   pBox = pAb->AbBox;
   if (pBox)
     {
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
	       while (pLine != pFirstLine && pLine != NULL)
		 pLine = pLine->LiNext;
	       if (pLine == NULL)
		 /* cette ligne ne fait plus parti du bloc de lignes */
		 return;
	     }
	 }


	/* Zone affichee avant modification */
	if (pLine == NULL)
	  {
	     l = 0;
	     h = pBox->BxYOrg;
	  }
	else
	  {
	     l = pLine->LiXOrg + pLine->LiXMax;
	     h = pBox->BxYOrg + pLine->LiYOrg;
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
	RemoveLines (pBox, frame, pLine, FALSE, &changeSelectBegin, &changeSelectEnd);
	if (pBox->BxFirstLine == NULL)
	  {
	  /* fait reevaluer la mise en lignes et on recupere */
	  /* la hauteur et la largeur du contenu de la boite */
	  GiveEnclosureSize (pAb, frame, &width, &height);
	  }
	else
	  {
	    ComputeLines (pBox, frame, &height);
	    if (pBox->BxContentWidth)
	      {
		/* it's an extensible bloc of lines */
		width = pBox->BxMaxWidth;
		h = pBox->BxYOrg;
		pBox->BxW = pBox->BxMaxWidth;
		pBox->BxWidth = pBox->BxW + pBox->BxLMargin + pBox->BxLBorder + pBox->BxLPadding + pBox->BxRMargin + pBox->BxRBorder + pBox->BxRPadding;
	      }
	    width = 0;
	  }
	ReadyToDisplay = status;

	/* Zone affichee apres modification */
	/* Il ne faut pas tenir compte de la boite si elle */
	/* n'est pas encore placee dans l'image concrete   */
	if (ReadyToDisplay && !pBox->BxXToCompute && !pBox->BxYToCompute)
	  {
	     if (pBox->BxWidth > l)
		l = pBox->BxWidth;
	     l += pBox->BxXOrg;
	     if (height > pBox->BxHeight)
	       DefClip (frame, pBox->BxXOrg, h, l, pBox->BxYOrg + height);
	     else
	       DefClip (frame, pBox->BxXOrg, h, l, pBox->BxYOrg + pBox->BxHeight);
	  }

	/* Faut-il reevaluer les marques de selection ? */
	pFrame = &ViewFrameTable[frame - 1];
	pSelBegin = &pFrame->FrSelectionBegin;
	if (changeSelectBegin && pSelBegin->VsBox)
	  {
	     /* Si la selection a encore un sens */
	     if (pSelBegin->VsBox->BxAbstractBox)
		ComputeViewSelMarks (pSelBegin);

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
		       ComputeViewSelMarks (pSelEnd);
		     
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
			     pSelEnd->VsXPos += BoxCharacterWidth (c, pSelBox->BxFont);
		       }
		   }
	       }
	  }

	if (width != 0 && width != pBox->BxW)
	   ChangeDefaultWidth (pBox, ibox, width, 0, frame);
	/* Faut-il conserver la hauteur ? */
	if (height != 0)
	  {
	     /* Il faut propager la modification de hauteur */
	     propagateStatus = Propagate;
	     /* We certainly need to re-check the height of enclosing elements */
	     /*if (propagateStatus == ToChildren)
	       RecordEnclosing (pBox, FALSE);*/
	     ChangeDefaultHeight (pBox, ibox, height, frame);
	     Propagate = propagateStatus;
	  }
     }
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
  PtrBox              ibox1;
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

  /* For ghost boxes go up to the block of lines */
  while (pAb->AbBox->BxType == BoGhost || pAb->AbBox->BxType == BoFloatGhost)
    pAb = pAb->AbEnclosing;
  pParentBox = pAb->AbBox;
  if (pLine)
    {
      pDimAb = &pAb->AbWidth;
      if (pParentBox->BxContentWidth)
	{
	  /* the line can be extended */
	  if (!pDimAb->DimIsPosition && pDimAb->DimMinimum
	      && pParentBox->BxW + xDelta < pParentBox->BxRuleWidth)
	    {
	      /* The block  min width is larger than its inside width */
	      /* use the min width */
	      pParentBox->BxContentWidth = FALSE;
	      RecomputeLines (pAb, NULL, NULL, frame);
	    }
	  else
	    {
	      ShiftLine (pLine, pAb, pBox, xDelta, frame);
	      pLine->LiXMax = pLine->LiRealLength;
	      ResizeWidth (pParentBox, pParentBox, NULL, xDelta, 0, 0, 0, frame);
	    }
	}
      else if (!pDimAb->DimIsPosition && pDimAb->DimMinimum
	       && pLine->LiRealLength + xDelta > pParentBox->BxW)
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
	      /* justified line */
	      lostPixels = BoxCharacterWidth (SPACE, pBox->BxFont);
	      realLength = pLine->LiRealLength + xDelta - spaceDelta * (pLine->LiSpaceWidth - lostPixels);
	      lostPixels = pLine->LiXMax - pLine->LiMinLength;
	    }
	  else
	    lostPixels = pLine->LiXMax - pLine->LiRealLength;
	  
	  if (pBox->BxW - xDelta > pLine->LiXMax)
	    {
	      /* The box too large */
	      /* freed pixels in the line */
	      lostPixels = pLine->LiXMax - pBox->BxW;
	      if (lostPixels > 0)
		RecomputeLines (pAb, pLine, NULL, frame);
	    }
	  else if ((xDelta > 0 && xDelta <= lostPixels) ||
		   (xDelta < 0 && (lostPixels < maxlostPixels
				   || (pLine->LiPrevious == NULL && pLine->LiNext == NULL))))
	    /* compress or complete the current line */
	    if (pLine->LiSpaceWidth == 0)
	      ShiftLine (pLine, pAb, pBox, xDelta, frame);
	    else
	      {
		CompressLine (pLine, xDelta, frame, spaceDelta);
		pLine->LiRealLength = realLength;
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
		    ibox1 = pLine->LiFirstPiece;
		  else
		    ibox1 = pLine->LiFirstBox;
		  if (ibox1->BxWidth > maxLength)
		    {
		      if (ibox1->BxAbstractBox->AbLeafType == LtText &&
			  maxLength > 0)
			{
			  if (pLine->LiNSpaces == 0)
			    /* No space in the line -> recompute lines */
			    maxLength = 1;
			  else
			    {
			      length = ibox1->BxNChars;
			      /* look for a break */
			      maxLength = SearchBreak (pLine, ibox1,
						       maxLength, ibox1->BxFont,
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
			ibox1 = pLi2->LiFirstPiece;
		      else
			ibox1 = pLi2->LiFirstBox;
		      
		      if (ibox1 && ibox1->BxWidth > maxLength)
			{
			  if (ibox1->BxAbstractBox->AbLeafType == LtText &&
			      maxLength > 0)
			    {
			      if (pLi2->LiNSpaces == 0)
				/* No space in the line -> recompute lines */
				maxLength = 1;
			      else
				{
				  length = ibox1->BxNChars;
				  /* look for a break */
				  maxLength = SearchBreak (pLi2, ibox1,
							   maxLength, ibox1->BxFont,
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
		      CompressLine (pLine, xDelta, frame, spaceDelta);
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
}


/*----------------------------------------------------------------------
  EncloseInLine checks that the box pBox is still included within its
  current block of lines of pAb.
  Update the block of lines if necessary.
  ----------------------------------------------------------------------*/
void EncloseInLine (PtrBox pBox, int frame, PtrAbstractBox pAb)
{
  PtrBox              ibox1;
  PtrBox              pPieceBox;
  PtrBox              pParentBox;
  int                 ascent, descent;
  int                 i, h, top, left;
  int                 pos, linespacing;
  PtrLine             pLine, prevLine;
  PtrLine             pNextLine;

  pParentBox = pAb->AbBox;
  top = pParentBox->BxTMargin + pParentBox->BxTBorder + pParentBox->BxTPadding;
  left = pParentBox->BxLMargin + pParentBox->BxLBorder + pParentBox->BxLPadding;
  linespacing = PixelValue (pAb->AbLineSpacing, pAb->AbLineSpacingUnit,
			    pAb, ViewFrameTable[frame - 1].FrMagnification);
  if (Propagate != ToSiblings || pParentBox->BxVertFlex)
    {
      if (pBox->BxAbstractBox->AbFloat != 'N')
	{
	  pLine = pParentBox->BxFirstLine;
	  while (pLine &&
		 pParentBox->BxYOrg + top + pLine->LiYOrg + pLine->LiHeight < pBox->BxYOrg)
	    pLine = pLine->LiNext;
	  while (pLine &&
		 pParentBox->BxYOrg + top + pLine->LiYOrg + pLine->LiHeight < pBox->BxYOrg + pBox->BxHeight &&
		 pParentBox->BxXOrg + left + pLine->LiXOrg >= pBox->BxXOrg + pBox->BxWidth)
	    pLine = pLine->LiNext;
	  if (pLine)
	    {
	      /* rebuild adjacent lines of that floating box */
	      RecomputeLines (pAb, pLine, NULL, frame);
	      pNextLine = NULL;
	      h = 0;
	    }
	  else
	    h = pParentBox->BxH;
	}
      else
	{
	  pLine = SearchLine (pBox);
	  if (pLine)
	    {
	      pNextLine = pLine->LiNext;
	      ascent = 0;
	      descent = 0;
	      if (!pBox->BxAbstractBox->AbHorizEnclosing)
		{
		  /* The box is out of lines (like page breaks) */
		  pLine->LiHorizRef = pBox->BxHorizRef;
		  pLine->LiHeight = pBox->BxHeight;
		  descent = pLine->LiHeight - pLine->LiHorizRef;
		  /* move the box */
		  i = pParentBox->BxYOrg + pLine->LiYOrg - pBox->BxYOrg;
		  YMove (pBox, NULL, i, frame);
		  /* delta of the block height if it's the last line */
		  h = pLine->LiYOrg + pLine->LiHeight - pParentBox->BxH;
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
			ibox1 = pPieceBox->BxNexChild;
		      else
			ibox1 = pPieceBox;
		      
		      if (ascent < ibox1->BxHorizRef)
			ascent = ibox1->BxHorizRef;
		      i = ibox1->BxHeight - ibox1->BxHorizRef;
		      if (descent < i)
			descent = i;
		      /* next box */
		      pPieceBox = GetNextBox (ibox1->BxAbstractBox);
		    }
		  while (pPieceBox && ibox1 != pLine->LiLastBox &&
			 ibox1 != pLine->LiLastPiece);
		  if (pLine->LiPrevious)
		    {
		      /* new position of the current line */
		      prevLine = pLine->LiPrevious;
		      pos = prevLine->LiYOrg + prevLine->LiHeight;
		      i = prevLine->LiYOrg + prevLine->LiHorizRef + linespacing - pLine->LiHorizRef;
		      if (i > pos || !prevLine->LiNoOverlap)
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
			    ibox1 = pPieceBox->BxNexChild;
			  else
			    ibox1 = pPieceBox;
			  
			  if (ibox1 != pBox)
			    YMove (ibox1, NULL, i, frame);
			  pPieceBox = GetNextBox (ibox1->BxAbstractBox);
			}
		      while (pPieceBox && ibox1 != pLine->LiLastBox &&
			     ibox1 != pLine->LiLastPiece);
		    }
		  
		  /* change the baseline of the current line */
		  i = ascent - pLine->LiHorizRef;
		  pLine->LiHorizRef = ascent;
		  if (i)
		    {
		      /* move the block baseline if it's inherited from the first line */
		      if (pAb->AbHorizRef.PosAbRef == pAb->AbFirstEnclosed &&
			  !pLine->LiPrevious)
			MoveHorizRef (pParentBox, NULL, i, frame);
		    }
		  
		  /* move the box */
		  /* vertical shifting of the box */
		  i = pParentBox->BxYOrg + pLine->LiYOrg + pLine->LiHorizRef - pBox->BxHorizRef - pBox->BxYOrg;
		  if (i)
		    YMove (pBox, NULL, i, frame);
		  
		  /* update the rest of the block */
		  pLine->LiHeight = descent + ascent;
		  /* delta of the block height if it's the last line */
		  h = pLine->LiYOrg + pLine->LiHeight - pParentBox->BxH;
		}

	      /* move next lines */
	      if (pNextLine)
		{
		  /* new position of the next line */
		  if (!pBox->BxAbstractBox->AbHorizEnclosing ||
		      linespacing < descent + pNextLine->LiHorizRef)
		    /* we set the next line under the previous one */
		    i = descent + pNextLine->LiHorizRef;
		  else
		    i = linespacing;
		  pos = pLine->LiYOrg + pLine->LiHorizRef + i - pNextLine->LiHorizRef;
		  /* vertical shifting of the next lines */
		  h = pos - pNextLine->LiYOrg;
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
				    ibox1 = pPieceBox->BxNexChild;
				  else
				    ibox1 = pPieceBox;
				  YMove (ibox1, NULL, h, frame);
				  pPieceBox = GetNextBox (ibox1->BxAbstractBox);
				}
			      while (pPieceBox && ibox1 != pNextLine->LiLastBox &&
				     ibox1 != pNextLine->LiLastPiece);
			    }
			  pNextLine = pNextLine->LiNext;
			}
		    }
		}
	      /* new height */
	      h += pParentBox->BxH;
	    }
	}
      UpdateBlockWithFloat (pParentBox, TRUE, TRUE, FALSE, &h);
      /* compute the line spacing */
      /* space added at the top and bottom of the paragraph */
      linespacing = linespacing - BoxFontHeight (pParentBox->BxFont);
      if (linespacing > 0)
	linespacing /= 2;
      else
	linespacing = 0;
      h += linespacing;
      /* update the block height */
      if (pParentBox->BxContentHeight)
	ChangeDefaultHeight (pParentBox, pParentBox, h, frame);
    }
}




