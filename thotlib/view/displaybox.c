/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * displaybox.c : all the stuff needed to display boxes in frames.
 *
 * Author: I. Vatton (INRIA)
 *
 */
 
#include "ustring.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "picture.h"
#include "appdialogue.h"

#define THOT_EXPORT extern
#include "boxes_tv.h"
#include "edit_tv.h"
#include "frame_tv.h"
#include "appdialogue_tv.h"
#include "picture_tv.h"

#include "buildboxes_f.h"
#include "displaybox_f.h"
#include "displayselect_f.h"
#include "font_f.h"
#include "picture_f.h"
#include "units_f.h"
#include "xwindowdisplay_f.h"

/*----------------------------------------------------------------------
  GetLineWeight computes the line weight of an abstract box.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static int          GetLineWeight (PtrAbstractBox pAb)
#else  /* __STDC__ */
static int          GetLineWeight (pAb)
PtrAbstractBox      pAb;
#endif /* __STDC__ */
{
  if (pAb == NULL)
    return (0);
  else
    return PixelValue (pAb->AbLineWeight, pAb->AbLineWeightUnit, pAb, 0);
}


/*----------------------------------------------------------------------
  DisplayImage displays a empty box in the frame.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         DisplayImage (PtrBox pBox, int frame, int xmin, int xmax, int ymin, int ymax, ThotBool selected)
#else  /* __STDC__ */
static void         DisplayImage (pBox, frame, xmin, xmax, ymin, ymax, selected)
PtrBox              pBox;
int                 frame;
int                 xmin;
int                 xmax;
int                 ymin;
int                 ymax;
Thotbool            selected;
#endif /* __STDC__ */
{
  ViewFrame          *pFrame;
  int                 xd, yd, x, y;
  int                 width, height;
  int                 op, RO;

  pFrame = &ViewFrameTable[frame - 1];
  if (pBox->BxAbstractBox->AbVisibility >= pFrame->FrVisibility)
    {
      if (pBox->BxAbstractBox->AbSensitive)
	op = 1;
      else
	op = 0;
      if (pBox->BxAbstractBox->AbReadOnly)
	RO = 1;
      else
	RO = 0;

      /* For changing drawing color */
      DrawRectangle (frame, 0, 0, 0, 0, 0, 0, 0, 0, pBox->BxAbstractBox->AbForeground,
		     pBox->BxAbstractBox->AbBackground, 0);
      x = pFrame->FrXOrg;
      y = pFrame->FrYOrg;
      xd = pBox->BxXOrg + pBox->BxLMargin + pBox->BxLBorder + pBox->BxLPadding - x;
      yd = pBox->BxYOrg + pBox->BxTMargin + pBox->BxTBorder + pBox->BxTPadding + FrameTable[frame].FrTopMargin - y;
      width = pBox->BxW;
      height = pBox->BxH;
      DrawPicture (pBox, (PictInfo *) pBox->BxPictInfo, frame, xd, yd, width, height);
      /* Should the end of de line be filled with dots */
      if (pBox->BxEndOfBloc > 0)
	{
	  /* fill the end of the line with dots */
	  xd = pBox->BxXOrg + pBox->BxLMargin + pBox->BxLBorder + pBox->BxLPadding - x;
	  yd = pBox->BxYOrg + pBox->BxHorizRef - y;
	  DrawPoints (frame, xd + width, yd,
		      pBox->BxEndOfBloc, RO, op, pBox->BxAbstractBox->AbForeground);
	}

      /* show the selection on the whole image */
      if (selected)
	if (pFrame->FrSelectOnePosition &&
	    pBox == pFrame->FrSelectionBegin.VsBox)
	  /* show the selection on the beginning or the end of the image */
	  DisplayStringSelection (frame,
				  pFrame->FrSelectionBegin.VsXPos,
				  pFrame->FrSelectionBegin.VsXPos + 2,
				  pFrame->FrSelectionBegin.VsBox);
	else
	  DisplayPointSelection (frame, pBox, 0);
    }
}


/*----------------------------------------------------------------------
  DisplaySymbol displays a mathematical symbols box enclosed in
  a frame. The glyphs are drawn with the Greek font and lines.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         DisplaySymbol (PtrBox pBox, int frame, ThotBool selected)
#else  /* __STDC__ */
static void         DisplaySymbol (pBox, frame selected)
PtrBox              pBox;
int                 frame;
ThotBool            selected;
#endif /* __STDC__ */
{
  ptrfont             font;
  ViewFrame          *pFrame;
  ThotBool            withbackground;
  int                 xd, yd, i;
  int                 fg, bg;
  int                 op, RO;
  int                 width, height;

  fg = pBox->BxAbstractBox->AbForeground;
  bg = pBox->BxAbstractBox->AbBackground;
  withbackground = (pBox->BxAbstractBox->AbFillPattern == 2);
  pFrame = &ViewFrameTable[frame - 1];
  if (pBox->BxAbstractBox->AbVisibility >= pFrame->FrVisibility)
    {
      font = pBox->BxFont;
      if (font != NULL)
	{
	  /* Position in the frame */
	  xd = pBox->BxXOrg + pBox->BxLMargin + pBox->BxLBorder + pBox->BxLPadding - pFrame->FrXOrg;
	  yd = pBox->BxYOrg + pBox->BxTMargin + pBox->BxTBorder + pBox->BxTPadding - pFrame->FrYOrg;
	  
	  if (pBox->BxAbstractBox->AbSensitive)
	    op = 1;
	  else
	    op = 0;
	  if (pBox->BxAbstractBox->AbReadOnly)
	    RO = 1;
	  else
	    RO = 0;

	  /* box sizes have to be positive */
	  width = pBox->BxW;
	  if (width < 0)
	    width = 0;
	  height = pBox->BxH;
	  if (height < 0)
	    height = 0;
	  
	  if (withbackground)
	    DrawRectangle (frame, 0, 0, xd, yd, width, height, 0, 0, 0, bg, 2);
	  
	  /* Line thickness */
	  i = GetLineWeight (pBox->BxAbstractBox);
	  
	  switch (pBox->BxAbstractBox->AbShape)
	    {
	    case 'r':
	      DrawRadical (frame, i, xd, yd, width, height, font, RO, op, fg);
	      break;
	    case 'i':
	      DrawIntegral (frame, i, xd, yd, width, height, 0, font, RO, op, fg);
	      break;
	    case 'c':
	      DrawIntegral (frame, i, xd, yd, width, height, 1, font, RO, op, fg);
	      break;
	    case 'd':
	      DrawIntegral (frame, i, xd, yd, width, height, 2, font, RO, op, fg);
	      break;
	    case 'S':
	      DrawSigma (frame, xd, yd, width, height, font, RO, op, fg);
	      break;
	    case 'P':
	      DrawPi (frame, xd, yd, width, height, font, RO, op, fg);
	      break;
	    case 'I':
	      DrawIntersection (frame, xd, yd, width, height, font, RO, op, fg);
	      break;
	    case 'U':
	      DrawUnion (frame, xd, yd, width, height, font, RO, op, fg);
	      break;
	    case 'h':
	      DrawHorizontalLine (frame, i, 5, xd, yd, width, height, 1, RO, op, fg);
	      break;
	    case 'v':
	      DrawVerticalLine (frame, i, 5, xd, yd, width, height, 1, RO, op, fg);
	      break;
	    case '>':
	      DrawArrow (frame, i, 5, xd, yd, width, height, 0, RO, op, fg);
	      break;
	    case '^':
	      DrawArrow (frame, i, 5, xd, yd, width, height, 90, RO, op, fg);
	      break;
	    case '<':
	      DrawArrow (frame, i, 5, xd, yd, width, height, 180, RO, op, fg);
	      break;
	    case 'V':
	      DrawArrow (frame, i, 5, xd, yd, width, height, 270, RO, op, fg);
	      break;
	    case '(':
	      DrawParenthesis (frame, i, xd, yd, width, height, 0, font, RO, op, fg);
	      break;
	    case ')':
	      DrawParenthesis (frame, i, xd, yd, width, height, 1, font, RO, op, fg);
	      break;
	    case '{':
	      DrawBrace (frame, i, xd, yd, width, height, 0, font, RO, op, fg);
	      break;
	    case '}':
	      DrawBrace (frame, i, xd, yd, width, height, 1, font, RO, op, fg);
	      break;
	    case '[':
	      DrawBracket (frame, i, xd, yd, width, height, 0, font, RO, op, fg);
	      break;
	    case ']':
	      DrawBracket (frame, i, xd, yd, width, height, 1, font, RO, op, fg);
	      break;
	    default:
	      break;
	    }

	  if (pBox->BxEndOfBloc > 0)
	    {
	      /* fill the end of the line with dots */
	      yd = pBox->BxYOrg + pBox->BxHorizRef - pFrame->FrYOrg;
	      DrawPoints (frame, xd + width, yd, pBox->BxEndOfBloc, RO, op, fg);
	    }

	  /* show the selection on the beginning or the end of the image */
	  if (selected)
	    {
	      if (pFrame->FrSelectOnePosition ||
		  pFrame->FrSelectionBegin.VsXPos == pBox->BxW)
		/* display a carret 
		   or the selection starts at the end of the box */
		DisplayStringSelection (frame,
					pFrame->FrSelectionBegin.VsXPos,
					pFrame->FrSelectionBegin.VsXPos + 2,
					pBox);
	      else
		DisplayStringSelection (frame,
					pFrame->FrSelectionBegin.VsXPos,
					pFrame->FrSelectionEnd.VsXPos,
					pBox);
	    }
	}
    }
}

/*----------------------------------------------------------------------
  DisplayEmptyBox shows an empty box but formatted and placed.
  A specific background is drawn in the box area.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                DisplayEmptyBox (PtrBox pBox, int frame, CHAR_T modele, ThotBool selected)
#else  /* __STDC__ */
void                DisplayEmptyBox (pBox, frame, modele, selected)
PtrBox              pBox;
int                 frame;
CHAR_T              modele;
ThotBool            selected;
#endif /* __STDC__ */
{
  ViewFrame          *pFrame;
  PtrAbstractBox      pAb;
  int                 op, RO, bg;
  int                 xd, yd;
  int                 width, height;

  pFrame = &ViewFrameTable[frame - 1];
  pAb = pBox->BxAbstractBox;
  if (pAb->AbVisibility >= pFrame->FrVisibility)
    {
      if (pAb->AbSensitive)
	op = 1;
      else
	op = 0;
      if (pAb->AbReadOnly)
	RO = 1;
      else
	RO = 0;

      xd = pBox->BxXOrg + pBox->BxLMargin + pBox->BxLBorder + pBox->BxLPadding - pFrame->FrXOrg;
      yd = pBox->BxYOrg + pBox->BxTMargin + pBox->BxTBorder + pBox->BxTPadding - pFrame->FrYOrg;
      /* box sizes have to be positive */
      width = pBox->BxW;
      if (width < 0)
	width = 0;
      height = pBox->BxH;
      if (height < 0)
	height = 0;

       /* show the selection on the current symbol */
      if (selected)
	{
	  bg = SelColor;
	  if (pAb->AbLeafType == LtGraphics)
	    DrawRectangle (frame, 2, 0, xd, yd, width,
			   height, RO, op, pAb->AbForeground,
			   bg, 0);
	  else
	    {
	      PaintWithPattern (frame, xd, yd, width, height, 0, RO, op,
				pAb->AbForeground,
				bg, 4);
	      DisplayStringSelection (frame, 0, 2, pBox);
	    }
	}
      else
	{
	  bg = pAb->AbBackground;
	  if (pAb->AbLeafType == LtGraphics)
	    DrawRectangle (frame, 2, 0, xd, yd, width,
			   height, RO, op, pAb->AbForeground,
			   bg, 0);
	  else
	    PaintWithPattern (frame, xd, yd, width, height, 0, RO, op,
			      pAb->AbForeground,
			      bg, 4);
	}
      
    }
}


/*----------------------------------------------------------------------
  DisplayGraph display a graphic box enclosed in frame.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         DisplayGraph (PtrBox pBox, int frame, ThotBool selected)
#else  /* __STDC__ */
static void         DisplayGraph (pBox, frame, selected)
PtrBox              pBox;
int                 frame;
ThotBool            selected;
#endif /* __STDC__ */
{
  ViewFrame          *pFrame;
  PtrAbstractBox      pAb;
  int                 i, xd, yd;
  int                 op, RO;
  int                 fg, bg;
  int                 pat;
  int                 style;
  int                 width, height;

  pAb = pBox->BxAbstractBox;
  fg = pAb->AbForeground;
  bg = pAb->AbBackground;
  pat = pAb->AbFillPattern;
  pFrame = &ViewFrameTable[frame - 1];
  if (pAb->AbVisibility >= pFrame->FrVisibility)
    {
      xd = pBox->BxXOrg + pBox->BxLMargin + pBox->BxLBorder + pBox->BxLPadding - pFrame->FrXOrg;
      yd = pBox->BxYOrg + pBox->BxTMargin + pBox->BxTBorder + pBox->BxTPadding - pFrame->FrYOrg;
      if (pAb->AbSensitive)
	op = 1;
      else
	op = 0;
      if (pAb->AbReadOnly)
	RO = 1;
      else
	RO = 0;

      width = pBox->BxW;
      height = pBox->BxH;
      if (Printing)
	  {
	    /* clip the origin */
	    if (xd < 0)
	      {
		width += xd;
		xd = 0;
	      }
	    if (yd < 0)
	      {
		height += yd;
		yd = 0;
	      }
	    /* clip the width */
	    if (xd + width > pFrame->FrClipXEnd - pFrame->FrXOrg)
	      width = pFrame->FrClipXEnd - pFrame->FrXOrg - xd;
	    /* limite la hauteur a la valeur du clipping */
	    if (yd + height > pFrame->FrClipYEnd - pFrame->FrYOrg)
	      height = pFrame->FrClipYEnd - pFrame->FrYOrg - yd;
	  }
	/* box sizes have to be positive */
	if (width < 0)
	  width = 0;
	if (height < 0)
	  height = 0;

	/* Style and thickness of drawing */
	i = GetLineWeight (pAb);
	switch (pAb->AbLineStyle)
	  {
	  case 'S':
	    style = 5; /* solid */
	    break;
	  case '-':
	    style = 4; /* dashed */
	    break;
	  case '.':
	    style = 3; /* dotted */
	    break;
	  default:
	    style = 5; /* solid */
	  }

	switch (pAb->AbRealShape)
	  {
	  case '\260':
	    DrawRectangle (frame, 0, 0, xd, yd, width, height, RO, op, fg, bg, 2);
	    break;
	  case '\261':
	    DrawRectangle (frame, 0, 0, xd, yd, width, height, RO, op, fg, bg, 5);
	    break;
	  case '\262':
	    DrawRectangle (frame, 0, 0, xd, yd, width, height, RO, op, fg, bg, 6);
	    break;
	  case '\263':
	    DrawRectangle (frame, 0, 0, xd, yd, width, height, RO, op, fg, bg, 7);
	    break;
	  case '\264':
	    DrawRectangle (frame, 0, 0, xd, yd, width, height, RO, op, fg, bg, 8);
	    break;
	  case '\265':
	    DrawRectangle (frame, 0, 0, xd, yd, width, height, RO, op, fg, bg, 9);
	    break;
	  case '\266':
	    DrawRectangle (frame, 0, 0, xd, yd, width, height, RO, op, fg, bg, 1);
	    break;
	  case '\267':
	    DrawRectangle (frame, 0, 0, xd, yd, width, height, RO, op, fg, bg, 0);
	    break;
	  case '\270':
	    DrawRectangle (frame, 0, 0, xd, yd, width, height, RO, op, fg, bg, 4);
	    break;
	  case '0':
	    DrawRectangle (frame, 0, 0, xd, yd, width, height, RO, op, fg, bg, pat);
	    break;
	  case '1':
	  case '2':
	  case '3':
	  case '4':
	  case '5':
	  case '6':
	  case '7':
	  case '8':
	  case 'R':
	    DrawRectangle (frame, i, style, xd, yd, width, height, RO, op, fg, bg, pat);
	    break;
	  case 'g':
	    /* Coords of the line are given by the enclosing box */
	    pAb = pAb->AbEnclosing;
	    if ((pAb->AbHorizPos.PosEdge == Left && pAb->AbVertPos.PosEdge == Top) ||
		(pAb->AbHorizPos.PosEdge == Right && pAb->AbVertPos.PosEdge == Bottom))
	      /* draw a \ */
	      DrawSlash (frame, i, style, xd, yd, width, height, 1, RO, op, fg);
	    else
	      /* draw a / */
	      DrawSlash (frame, i, style, xd, yd, width, height, 0, RO, op, fg);
	    break;
	  case 'C':
	    if (pBox->BxRx == 0 && pBox->BxRy == 0)
	      DrawRectangle (frame, i, style, xd, yd, width, height, RO, op, fg, bg, pat);
	    else
	      DrawOval (frame, i, style, xd, yd, width, height, pBox->BxRx, pBox->BxRy, RO, op, fg, bg, pat);
	    break;
	  case 'L':
	    DrawDiamond (frame, i, style, xd, yd, width, height, RO, op, fg, bg, pat);
	    break;
	  case 'a':
	  case 'c':
	    DrawEllips (frame, i, style, xd, yd, width, height, RO, op, fg, bg, pat);
	    break;
	  case 'h':
	    DrawHorizontalLine (frame, i, style, xd, yd, width, height, 1, RO, op, fg);
	    break;
	  case 't':
	    DrawHorizontalLine (frame, i, style, xd, yd, width, height, 0, RO, op, fg);
	    break;
	  case 'b':
	    DrawHorizontalLine (frame, i, style, xd, yd, width, height, 2, RO, op, fg);
	    break;
	  case 'v':
	    DrawVerticalLine (frame, i, style, xd, yd, width, height, 1, RO, op, fg);
	    break;
	  case 'l':
	    DrawVerticalLine (frame, i, style, xd, yd, width, height, 0, RO, op, fg);
	    break;
	  case 'r':
	    DrawVerticalLine (frame, i, style, xd, yd, width, height, 2, RO, op, fg);
	    break;
	  case '/':
	    DrawSlash (frame, i, style, xd, yd, width, height, 0, RO, op, fg);
	    break;
	  case '\\':
	    DrawSlash (frame, i, style, xd, yd, width, height, 1, RO, op, fg);
	    break;
	  case '>':
	     DrawArrow (frame, i, style, xd, yd, width, height, 0, RO, op, fg);
	     break;
	  case 'E':
	    DrawArrow (frame, i, style, xd, yd, width, height, 45, RO, op, fg);
	    break;
	  case '^':
	    DrawArrow (frame, i, style, xd, yd, width, height, 90, RO, op, fg);
	    break;
	  case 'O':
	    DrawArrow (frame, i, style, xd, yd, width, height, 135, RO, op, fg);
	    break;
	  case '<':
	    DrawArrow (frame, i, style, xd, yd, width, height, 180, RO, op, fg);
	    break;
	  case 'o':
	    DrawArrow (frame, i, style, xd, yd, width, height, 225, RO, op, fg);
	    break;
	  case 'V':
	    DrawArrow (frame, i, style, xd, yd, width, height, 270, RO, op, fg);
	    break;
	  case 'e':
	    DrawArrow (frame, i, style, xd, yd, width,
		       height, 315, RO, op, fg);
	    break;
	    
	  case 'P':
	    DrawRectangleFrame (frame, i, style, xd, yd, width,
				height, RO, op, fg, bg, pat);
	    break;
	  case 'Q':
	    DrawEllipsFrame (frame, i, style, xd, yd, width,
			     height, RO, op, fg, bg, pat);
	    break;
	  case 'W':
	    DrawCorner (frame, i, style, xd, yd, width, height, 0, RO, op, fg);
	    break;
	  case 'X':
	    DrawCorner (frame, i, style, xd, yd, width, height, 1, RO, op, fg);
	    break;
	  case 'Y':
	    DrawCorner (frame, i, style, xd, yd, width, height, 2, RO, op, fg);
	    break;
	  case 'Z':
	    DrawCorner (frame, i, style, xd, yd, width, height, 3, RO, op, fg);
	    break;
	    
	  default:
	    break;
	  }
	
	if (pBox->BxEndOfBloc > 0)
	  {
	    /* fill the end of the line with dots */
	    yd = pBox->BxYOrg + pBox->BxHorizRef - pFrame->FrYOrg;
	    DrawPoints (frame, xd + width, yd, pBox->BxEndOfBloc, RO, op, fg);
	  }

	/* show the selection on the whole image */
	if (selected)
	  if (pFrame->FrSelectOnePosition)
	    DisplayPointSelection (frame, pBox, pFrame->FrSelectionBegin.VsIndBox);
	  else
	    DisplayPointSelection (frame, pBox, 0);
     }
}


/*----------------------------------------------------------------------
  PolyTransform checks whether a polyline Box need to be transformed
  to fit in the current area.
  If the CHKR_LIMIT point matches the current geometry, no need
  to change anything, otherwise, all points are moved using
  Box-Width/Lim-X ratio horizontally and Box-Height/Lim-Y ratio vertically.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                PolyTransform (PtrBox pBox)
#else  /* __STDC__ */
void                PolyTransform (pBox)
PtrBox              pBox;
#endif /* __STDC__ */
{
   float               xRatio, yRatio, pointIndex;
   PtrTextBuffer       adbuff;
   int                 i;
   int                 j, val;
   int                 width, height;

   /* box sizes have to be positive */
   width = pBox->BxW;
   if (width < 0)
     width = 0;
   height = pBox->BxH;
   if (height < 0)
     height = 0;

   /* Compute ratio for axis X */
   val = PixelToPoint (width) * 1000;
   if (val != pBox->BxBuffer->BuPoints[0].XCoord
       && pBox->BxBuffer->BuPoints[0].XCoord > 0)
     {
	pointIndex = (float) pBox->BxBuffer->BuPoints[0].XCoord / pBox->BxXRatio;
	/* save the new distortion ratio between box and abstract box */
	pBox->BxXRatio = (float) val / pointIndex;
	/* ratio applied to the box */
	xRatio = (float) val / (float) pBox->BxBuffer->BuPoints[0].XCoord;
	pBox->BxBuffer->BuPoints[0].XCoord = val;
     }
   else
      xRatio = 1.0;

   /* Compute ratio for axis Y */
   val = PixelToPoint (height) * 1000;
   if (val != pBox->BxBuffer->BuPoints[0].YCoord
       && pBox->BxBuffer->BuPoints[0].YCoord > 0)
     {
	pointIndex = (float) pBox->BxBuffer->BuPoints[0].YCoord / pBox->BxYRatio;
	/* save the new distortion ratio between box and abstract box */
	pBox->BxYRatio = (float) val / pointIndex;
	/* ratio applied to the box */
	yRatio = (float) val / (float) pBox->BxBuffer->BuPoints[0].YCoord;
	pBox->BxBuffer->BuPoints[0].YCoord = val;
     }
   else
      yRatio = 1.0;

   if (xRatio != 1 || yRatio != 1)
     {
	j = 1;
	adbuff = pBox->BxBuffer;
	val = pBox->BxNChars;
	for (i = 1; i < val; i++)
	  {
	     if (j >= adbuff->BuLength)
	       {
		  if (adbuff->BuNext != NULL)
		    {
		       /* Next buffer */
		       adbuff = adbuff->BuNext;
		       j = 0;
		    }
	       }
	     adbuff->BuPoints[j].XCoord = (int) ((float) adbuff->BuPoints[j].XCoord * xRatio);
	     adbuff->BuPoints[j].YCoord = (int) ((float) adbuff->BuPoints[j].YCoord * yRatio);
	     j++;
	  }
     }
}


/*----------------------------------------------------------------------
  DrawPolyLine display a polyline constraint in frame.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         DrawPolyLine (PtrBox pBox, int frame, ThotBool selected)
#else  /* __STDC__ */
static void         DrawPolyLine (pBox, frame, selected)
PtrBox              pBox;
int                 frame;
ThotBool            selected;
#endif /* __STDC__ */
{
  PtrAbstractBox      pAb;
  ViewFrame          *pFrame;
  int                 i, xd, yd;
  int                 op, RO;
  int                 fg, bg;
  int                 pat;
  int                 style, arrow;
  int                 width;

  /* If no point is defined, no need to draw it */
  if (pBox->BxBuffer == NULL || pBox->BxNChars <= 1)
    return;

  /* Transform the polyline if the box size has changed */
  PolyTransform (pBox);
  pAb = pBox->BxAbstractBox;
  fg = pAb->AbForeground;
  bg = pAb->AbBackground;
  pat = pAb->AbFillPattern;
  pFrame = &ViewFrameTable[frame - 1];
  if (pAb->AbVisibility >= pFrame->FrVisibility)
    {
      xd = pBox->BxXOrg + pBox->BxLMargin + pBox->BxLBorder + pBox->BxLPadding - pFrame->FrXOrg;
      yd = pBox->BxYOrg + pBox->BxTMargin + pBox->BxTBorder + pBox->BxTPadding - pFrame->FrYOrg;
      if (pAb->AbSensitive)
	op = 1;
      else
	op = 0;
      
      if (pAb->AbReadOnly)
	RO = 1;
      else
	RO = 0;
      
      /* box sizes have to be positive */
      width = pBox->BxW;
      if (width < 0)
	width = 0;
      
      /* Style and thickness of the line */
      i = GetLineWeight (pAb);
      switch (pAb->AbLineStyle)
	{
	case 'S':
	  style = 5; /* solid */
	  break;
	case '-':
	  style = 4; /* dashed */
	  break;
	case '.': /* dotted */
	  style = 3;
	  break;
	default:
	  style = 5; /* solid */
	}

      switch (pAb->AbPolyLineShape)
	{
	case 'S':	/* Segments */
	case 'U':	/* Segments forward arrow */
	case 'N':	/* Segments backward arrow */
	case 'M':	/* Segments arrows on both directions */
	case 'w':	/* Segments (2 points) */
	case 'x':	/* Segments (2 points) forward arrow */
	case 'y':	/* Segments (2 points) backward arrow */
	case 'z':	/* Segments (2 points) arrows on both directions */
	  if (pAb->AbPolyLineShape == 'S' || pAb->AbPolyLineShape == 'w')
	    arrow = 0;
	  else if (pAb->AbPolyLineShape == 'U' || pAb->AbPolyLineShape == 'x')
	    arrow = 1;
	  else if (pAb->AbPolyLineShape == 'N' || pAb->AbPolyLineShape == 'y')
	    arrow = 2;
	  else
	    arrow = 3;
	  DrawSegments (frame, i, style, xd, yd, pBox->BxBuffer, pBox->BxNChars, RO, op, fg, arrow);
	  break;
	case 'B':	/* Beziers (open) */
	case 'A':	/* Beziers (open) forward arrow */
	case 'F':	/* Beziers (open) backward arrow */
	case 'D':	/* Beziers (open) arrows on both directions */
	  if (pAb->AbPolyLineShape == 'B')
	    arrow = 0;
	  else if (pAb->AbPolyLineShape == 'A')
	    arrow = 1;
	  else if (pAb->AbPolyLineShape == 'F')
	    arrow = 2;
	  else
	    arrow = 3;
	  /* compute control points */
	  if (pBox->BxPictInfo == NULL)
	    pBox->BxPictInfo = (int *) ComputeControlPoints (pBox->BxBuffer, pBox->BxNChars);
	  DrawCurb (frame, i, style, xd, yd, pBox->BxBuffer,
		    pBox->BxNChars, RO, op, fg, arrow, (C_points *) pBox->BxPictInfo);
	  break;
	case 'p':	/* polygon */
	  DrawPolygon (frame, i, style, xd, yd, pBox->BxBuffer,
		       pBox->BxNChars, RO, op, fg, bg, pat);
	  break;
	case 's':	/* closed spline */
	  /* compute control points */
	  if (pBox->BxPictInfo == NULL)
	    pBox->BxPictInfo = (int *) ComputeControlPoints (pBox->BxBuffer, pBox->BxNChars);
	  DrawSpline (frame, i, style, xd, yd, pBox->BxBuffer,
		      pBox->BxNChars, RO, op, fg, bg, pat, (C_points *) pBox->BxPictInfo);
	  break;
	default:
	  break;
	}
      
      if (pBox->BxEndOfBloc > 0)
	{
	  /* fill the end of the line with dots */
	  yd = pBox->BxYOrg + pBox->BxHorizRef - pFrame->FrYOrg;
	  DrawPoints (frame, xd + width, yd, pBox->BxEndOfBloc, RO, op, fg);
	}
      
      /* show the selection on the whole image */
      if (selected)
	if (pFrame->FrSelectOnePosition)
	  DisplayPointSelection (frame, pBox, pFrame->FrSelectionBegin.VsIndBox);
	else if (pBox->BxNChars > 1)
	  DisplayPointSelection (frame, pBox, 0);
    }
}


/*----------------------------------------------------------------------
  DisplayJustifiedText display the content of a Text box tweaking
  the space sizes to ajust line length to the size of the frame.
  Remaining pixel space (BxNPixels) is equally dispatched 
  on all spaces in the line.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         DisplayJustifiedText (PtrBox pBox, PtrBox mbox, int frame, ThotBool selected)
#else  /* __STDC__ */
static void         DisplayJustifiedText (pBox, mbox, frame, selected)
PtrBox              pBox;
PtrBox              mbox;
int                 frame;
ThotBool            selected;
#endif /* __STDC__ */
{
  PtrTextBuffer       adbuff;
  PtrTextBuffer       newbuff;
  ViewFrame          *pFrame;
  PtrBox              nbox;
  PtrAbstractBox      pAb;
  UCHAR_T             car;
  int                 indbuff;
  int                 restbl;
  int                 newind;
  int                 newbl, lg;
  int                 charleft, dc;
  int                 buffleft;
  int                 indmax, bl;
  int                 nbcar, x, y;
  int                 lgspace;
  int                 fg, bg;
  int                 pat;
  int                 RO, op;
  int                 shadow;
  int                 width;
  int                 left, right;
  ThotBool            blockbegin;
  ThotBool            withbackground;
  ThotBool            withline;

  indmax = 0;
  buffleft = 0;
  adbuff = NULL;
  indbuff = 0;
  restbl = 0;
  nbcar = 0;
  pAb = pBox->BxAbstractBox;
  
  /* do we have to display stars instead of characters? */
  if (pAb->AbBox->BxShadow)
    shadow = 1;
  else
    shadow = 0;
  
  /* Is this box the first of a block of text? */
  if (mbox == pBox)
    blockbegin = TRUE;
  else if (mbox->BxType != BoBlock || mbox->BxFirstLine == NULL)
    blockbegin = TRUE;
  else if (pBox->BxType == BoComplete && mbox->BxFirstLine->LiFirstBox == pBox)
    blockbegin = TRUE;
  else if ((pBox->BxType == BoPiece || pBox->BxType == BoDotted) &&
	   mbox->BxFirstLine->LiFirstPiece == pBox)
    blockbegin = TRUE;
  else
    blockbegin = FALSE;
  
  /* Is an hyphenation mark needed at the end of the box? */
  if (pBox->BxType == BoDotted)
    withline = TRUE;
  else
    withline = FALSE;

  fg = pAb->AbForeground;
  bg = pAb->AbBackground;
  withbackground = (pAb->AbFillPattern == 2);
  pFrame = &ViewFrameTable[frame - 1];
  left = 0;
  right = 0;
  if (pAb->AbVisibility >= pFrame->FrVisibility)
    {
      /* Initialization */
      x = pBox->BxXOrg + pBox->BxLMargin + pBox->BxLBorder + pBox->BxLPadding - pFrame->FrXOrg;
      y = pBox->BxYOrg + pBox->BxTMargin + pBox->BxTBorder + pBox->BxTPadding - pFrame->FrYOrg;
      bl = 0;
      newind = pBox->BxFirstChar;
      newbuff = pBox->BxBuffer;
      charleft = pBox->BxNChars;
      newbl = pBox->BxNPixels;
      lg = 0;
      if (pAb->AbSensitive)
	op = 1;
      else
	op = 0;
      if (pAb->AbReadOnly)
	RO = 1;
      else
	RO = 0;
	   
      /* box sizes have to be positive */
      width = pBox->BxW;
      if (width < 0)
	width = 0;
      lgspace = pBox->BxSpaceWidth;
      if (lgspace == 0)
	lgspace = CharacterWidth (SPACE, pBox->BxFont);
      
      /* Search the first displayable char */
      if (charleft > 0)
	{
	  /* there is almost one character to display */
	  do
	    {
	      adbuff = newbuff;
	      indbuff = newind;
	      restbl = newbl;
	      x += lg;
	      car = (UCHAR_T) (adbuff->BuContent[indbuff - 1]);
	      if (car == SPACE)
		{
		  lg = lgspace;
		  if (newbl > 0)
		    {
		      newbl--;
		      lg++;
		    } 
		}
	      else
		lg = CharacterWidth (car, pBox->BxFont);
	       
	      charleft--;
	      /* Skip to next char */
	      if (indbuff < adbuff->BuLength)
		newind = indbuff + 1;
	      else
		{
		  if (adbuff->BuNext == NULL && charleft > 0)
		    charleft = 0;
		  newind = 1;
		  newbuff = adbuff->BuNext;
		} 
	    }
	  while (!(x + lg > 0 || charleft <= 0));
	   
	  /* Display the list of text buffers pointed by adbuff */
	  /* beginning at indbuff and of lenght charleft.       */
	  /* -------------------------------------------------- */
	  if (x + lg > 0)
	    charleft++;
	  nbcar = 0;
	  if (adbuff == NULL)
	    charleft = 0;
	  else
	    {
	      buffleft = adbuff->BuLength - indbuff + 1;
	      if (charleft > buffleft)
		indmax = adbuff->BuLength;
	      else
		indmax = indbuff - 1 + charleft;
	    } 
	  
	  /* Do we need to draw a background */
	  if (withbackground)
	    DrawRectangle (frame, 0, 0,
			   x - pBox->BxLPadding, y - pBox->BxTPadding,
			   width + pBox->BxLPadding + pBox->BxRPadding,
			   FontHeight (pBox->BxFont) + pBox->BxTPadding + pBox->BxBPadding,
			   0, 0, 0, bg, 2);
	}

      /* check if the box is selected */
      if (selected)
	{
	  if (pBox == pFrame->FrSelectionBegin.VsBox ||
	      pBox == pFrame->FrSelectionEnd.VsBox)
	    {
	      if (pFrame->FrSelectOnePosition)
		{
		  left = pFrame->FrSelectionBegin.VsXPos;
		  right = left + 2;
		}
	      else
		{
		  /* almost one character is selected */
		  if (pBox == pFrame->FrSelectionBegin.VsBox)
		    left = pFrame->FrSelectionBegin.VsXPos;
		  if (pBox == pFrame->FrSelectionEnd.VsBox)
		    right = pFrame->FrSelectionEnd.VsXPos;
		  else
		    right = pBox->BxWidth;
		  DisplayStringSelection (frame, left, right, pBox);
		  /* the selection is done now */
		  left = 0;
		  right = 0;
		}
	    }
	  else if (pBox->BxType == BoPiece)
	    {
	      /* check if the box in within the selection */
	      if (pFrame->FrSelectionBegin.VsBox &&
		  pAb == pFrame->FrSelectionBegin.VsBox->BxAbstractBox)
		{
		  nbox = pFrame->FrSelectionBegin.VsBox;
		  while (nbox && nbox != pFrame->FrSelectionEnd.VsBox && nbox != pBox)
		    nbox = nbox->BxNexChild;
		  if (nbox == pBox)
		    /* it's within the current selection */
		    DisplayBgBoxSelection (frame, pBox);
		}
	      else if (pFrame->FrSelectionEnd.VsBox &&
		       pAb == pFrame->FrSelectionEnd.VsBox->BxAbstractBox)
		{
		  nbox = pBox->BxNexChild;
		  while (nbox && nbox != pFrame->FrSelectionEnd.VsBox)
		    nbox = nbox->BxNexChild;
		  if (nbox == pFrame->FrSelectionEnd.VsBox)
		    /* it's within the current selection */
		    DisplayBgBoxSelection (frame, pBox);
		}
	      else
		DisplayBgBoxSelection (frame, pBox);
	    }
	  else
	    DisplayBgBoxSelection (frame, pBox);
	}

      while (charleft > 0)
	{
	  /* handle each char in the buffer */
	  while (indbuff <= indmax)
	    {
	      car = (UCHAR_T) (adbuff->BuContent[indbuff - 1]);
	      
	      if (car == SPACE || car == THIN_SPACE ||
		  car == HALF_EM || car == UNBREAKABLE_SPACE || car == TAB)
		{
		  /* display the last chars handled */
		  dc = indbuff - nbcar;
		  x += DrawString (adbuff->BuContent, dc, nbcar, frame, x, y, pBox->BxFont, 0, bl, 0, blockbegin, RO, op, fg, shadow);
		  
		  if (shadow && (car == SPACE || car == THIN_SPACE || car == HALF_EM || UNBREAKABLE_SPACE || car == TAB))
		    DrawChar ('*', frame, x, y, pBox->BxFont, RO, op, fg);
		  else if (!ShowSpace)
		    {
		      /* Show the space chars */
		      if (car == SPACE || car == TAB) 
			DrawChar (SHOWN_SPACE, frame, x, y, pBox->BxFont, RO, op, fg);
		      else if (car == THIN_SPACE)
			DrawChar (SHOWN_THIN_SPACE, frame, x, y, pBox->BxFont, RO, op, fg);
		      else if (car == HALF_EM)
			DrawChar (SHOWN_HALF_EM, frame, x, y, pBox->BxFont, RO, op, fg);
		      else if (car == UNBREAKABLE_SPACE)
			DrawChar (SHOWN_UNBREAKABLE_SPACE, frame, x, y, pBox->BxFont, RO, op, fg);
		    }
		 
		  nbcar = 0;
		  if (car == SPACE)
		   if (restbl > 0) {
		     /* Pixel space splitting */
		     x = x + lgspace + 1;
		     restbl--;
		   } else
		     x += lgspace;
		 else
		   x += CharacterWidth (car, pBox->BxFont);
		 
		 bl = 1;
	       } else /* Just add the next char */
		 nbcar++;
	       
	       indbuff++; /* Skip to next char */
	     } 
	     
	     /* Draw the last chars from buffer */
	     dc = indbuff - nbcar;
	     charleft -= buffleft;
	     if (charleft <= 0)
	       {
		 /* Finished */
		 x += DrawString (adbuff->BuContent, dc, nbcar, frame, x, y, pBox->BxFont, width, bl, withline, blockbegin, RO, op, fg, shadow);
		 if (pBox->BxUnderline != 0)
		   DisplayUnderline (frame, x, y, pBox->BxFont, pBox->BxUnderline, pBox->BxThickness, pBox->BxWidth, RO, op, fg);
		 /* Next char lookup */
		 if (((UCHAR_T) adbuff->BuContent[indbuff - 1] == BREAK_LINE ||
		      (UCHAR_T) adbuff->BuContent[indbuff - 1] == NEW_LINE) &&
		     !ShowSpace)
		   DrawChar (SHOWN_BREAK_LINE, frame, x, y, pBox->BxFont, RO, op, fg);
	       }
	     else
	       {
		 x += DrawString (adbuff->BuContent, dc, nbcar, frame, x, y, pBox->BxFont, 0, bl, 0, blockbegin, RO, op, fg, shadow);
		 bl = 0;
		 /* Skip to next buffer */
		 if (adbuff->BuNext == NULL)
		   charleft = 0;
		 else
		   {
		     indbuff = 1;
		     adbuff = adbuff->BuNext;
		     buffleft = adbuff->BuLength;
		     if (charleft < buffleft)
		       indmax = charleft;
		     else
		       indmax = buffleft;
		   }
	       }
	     nbcar = 0;
	}  
      
      /* Should the end of de line be filled with dots */
      if (pBox->BxEndOfBloc > 0)
	{
	  /* fill the end of the line with dots */
	  x = pBox->BxXOrg + pBox->BxLMargin + pBox->BxLBorder + pBox->BxLPadding;
	  y = pBox->BxYOrg + pBox->BxHorizRef - pFrame->FrYOrg;
	  DrawPoints (frame, pBox->BxXOrg + width - pFrame->FrXOrg, y, pBox->BxEndOfBloc, RO, op, fg);
	}
      /* display a caret if needed */
      if (left != right)
	DisplayStringSelection (frame, left, right, pBox);
    }
}


/*----------------------------------------------------------------------
  DisplayBorders displays the box borders.
  Parameters x, y, w, h give the clipping region.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                  DisplayBorders (PtrBox box, int frame, int x, int y, int w, int h) 
#else  /* __STDC__ */
void                DisplayBorders (box, frame, x, y, w, h)
PtrBox              box;
int                 frame;
int                 x;
int                 y;
int                 w;
int                 h;
#endif /* __STDC__ */
{
  PtrAbstractBox      pAb;
  int                 color;
  int                 t, b, l, r, pos, dim;
  int                 xFrame, yFrame;

  pAb = box->BxAbstractBox;
  /* position in the frame */
  xFrame = box->BxXOrg - ViewFrameTable[frame - 1].FrXOrg;
  yFrame = box->BxYOrg - ViewFrameTable[frame - 1].FrYOrg;
  /* part of the top, left, bottom and right border which are visible */
  t = yFrame + box->BxTMargin + box->BxTBorder - y;
  l = xFrame + box->BxLMargin + box->BxLBorder - x;
  b = y + h - yFrame  - box->BxHeight + box->BxBMargin + box->BxBBorder;
  r = x + w - xFrame  - box->BxWidth + box->BxRMargin + box->BxRBorder;
  if (box->BxTBorder && pAb->AbTopStyle > 2 && pAb->AbTopBColor != -2 && t > 0)
    {
      /* the top border is visible */
      if (pAb->AbTopBColor == -1)
	color = pAb->AbForeground;
      else
	color = pAb->AbTopBColor;
      /* the top border is visible */
      switch (pAb->AbTopStyle)
	{
	case 10: /* outset */
	  break;
	case 9: /* inset */
	  break;
	case 8: /* ridge */
	  break;
	case 7: /* groove */
	  break;
	case 6: /* double */
	  /* top line */
	  DrawHorizontalLine (frame, 1, 5,
			      x, yFrame + box->BxTMargin,
			      w, 1,
			      0, 0, 0, color);
	  /* the width of the bottom line depends on the visibility of vertical borders */
	  dim = w;
	  pos = x;
	  if (l > 0)
	    {
	      pos += l;
	      dim -= l;
	    }
	  if (r > 0)
	    dim -= r;
	  /* bottom line */
	  if (t < h)
	    DrawHorizontalLine (frame, 1, 5,
				pos, yFrame + box->BxTMargin + box->BxTBorder,
				dim, 1,
				2, 0, 0, color);
	  break;
	default:
	  DrawHorizontalLine (frame, t, pAb->AbTopStyle,
			      x, y,
			      w, t,
			      0, 0, 0, color);
	  break;
	}
    }
  if (box->BxLBorder && pAb->AbLeftStyle > 2 && pAb->AbLeftBColor != -2 && l > 0)
    {
      if (pAb->AbLeftBColor == -1)
	color = pAb->AbForeground;
      else
	color = pAb->AbLeftBColor;
      /* the left border is visible */
      switch (pAb->AbTopStyle)
	{
	case 10: /* outset */
	  break;
	case 9: /* inset */
	  break;
	case 8: /* ridge */
	  break;
	case 7: /* groove */
	  break;
	case 6: /* double */
	  /* left line */
	  DrawVerticalLine (frame, 1, 5,
			    xFrame + box->BxLMargin, y,
			    1, h,
			    0, 0, 0, color);
	  /* the width of the right line depends on the visibility of horizontal borders */
	  dim = h;
	  pos = y;
	  if (t > 0)
	    {
	      pos += t;
	      dim -= t;
	    }
	  if (b > 0)
	    dim -= b;
	  /* rigth line */
	  DrawVerticalLine (frame, 1, 5,
			    xFrame + box->BxLMargin + box->BxLBorder, pos,
			    1, dim,
			    2, 0, 0, color);
	  break;
	default:
	  DrawVerticalLine (frame, l, pAb->AbLeftStyle,
			    x, y,
			    l, h,
			    0, 0, 0, color);
	  break;
	}
    }
  if (box->BxBBorder && pAb->AbBottomStyle > 2 && pAb->AbBottomBColor != -2 && b > 0)
    {
      if (pAb->AbBottomBColor == -1)
	color = pAb->AbForeground;
      else
	color = pAb->AbBottomBColor;
      /* the bottom border is visible */
      switch (pAb->AbBottomStyle)
	{
	case 10: /* outset */
	  break;
	case 9: /* inset */
	  break;
	case 8: /* ridge */
	  break;
	case 7: /* groove */
	  break;
	case 6: /* double */
	  /* top line */
	  /* the width of the bottom line depends on the visibility of vertical borders */
	  dim = w;
	  pos = x;
	  if (l > 0)
	    {
	      pos += l;
	      dim -= l;
	    }
	  if (r > 0)
	    dim -= r;
	  /* bottom line */
	  DrawHorizontalLine (frame, 1, 5,
			      pos, yFrame + box->BxHeight - box->BxBMargin - box->BxBBorder,
			      dim, 1,
			      0, 0, 0, color);
	  /* bottom line */
	  if (b < h)
	  DrawHorizontalLine (frame, 1, 5,
			      x, yFrame + box->BxHeight - box->BxBMargin,
			      w, 1,
			      2, 0, 0, color);
	  break;
	default:
	  DrawHorizontalLine (frame, b, pAb->AbBottomStyle,
			      x, yFrame + box->BxHeight - box->BxBMargin - box->BxBBorder,
			      w, b,
			      2, 0, 0, color);
	  break;
	}
    }
  if (box->BxRBorder && pAb->AbRightStyle > 2 && pAb->AbRightBColor != -2 && r > 0)
    {
      if (pAb->AbRightBColor == -1)
	color = pAb->AbForeground;
      else
	color = pAb->AbRightBColor;
      /* the right border is visible */
      switch (pAb->AbRightStyle)
	{
	case 10: /* outset */
	  break;
	case 9: /* inset */
	  break;
	case 8: /* ridge */
	  break;
	case 7: /* groove */
	  break;
	case 6: /* double */
	  /* the width of the left line depends on the visibility of horizontal borders */
	  dim = h;
	  pos = y;
	  if (t > 0)
	    {
	      pos += t;
	      dim -= t;
	    }
	  if (b > 0)
	    dim -= b;
	  /* left line */
	  DrawVerticalLine (frame, 1, 5,
			    xFrame + box->BxWidth - box->BxRMargin - box->BxRBorder, pos,
			    1, dim,
			    0, 0, 0, color);
	  /* rigth line */
	  DrawVerticalLine (frame, 1, 5,
			    xFrame + box->BxWidth - box->BxRMargin, y,
			    1, h,
			    2, 0, 0, color);
	  break;
	default:
	  DrawVerticalLine (frame, r, pAb->AbRightStyle,
			    xFrame + box->BxWidth - box->BxRMargin - box->BxRBorder, y,
			    r, h,
			    2, 0, 0, color);
	  break;
	}
    }
}


/*----------------------------------------------------------------------
  DisplayBox display a box depending on it's content.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                DisplayBox (PtrBox box, int frame, int xmin, int xmax, int ymin, int ymax)
#else  /* __STDC__ */
void                DisplayBox (box, frame, xmin, xmax, ymin, ymax)
PtrBox              box;
int                 frame;
int                 xmin;
int                 xmax;
int                 ymin;
int                 ymax;
#endif /* __STDC__ */
{
  ViewFrame         *pFrame;
  PtrBox             mbox;
  PtrAbstractBox     pAb;
  int                x, y;
  int                xd, yd, width, height;
  ThotBool           selected;

  pFrame = &ViewFrameTable[frame - 1];
  pAb = box->BxAbstractBox;
  x = ViewFrameTable[frame - 1].FrXOrg;
  y = ViewFrameTable[frame - 1].FrYOrg;
  xd = box->BxXOrg + box->BxLMargin;
  yd = box->BxYOrg + box->BxTMargin;
  width = box->BxWidth - box->BxLMargin - box->BxRMargin;
  height = box->BxHeight - box->BxTMargin - box->BxBMargin;
  if (Printing)
    {
      /* clipping on the origin */
      if (xd < x)
	{
	  width = width - x + xd;
	  xd = x;
	}
      if (yd < y)
	{
	  height = height - y + yd;
	  yd = y;
	}
      /* clipping on the width */
      if (xd + width > xmax)
	width = xmax - xd;
      /* clipping on the height */
      if (yd + height > ymax)
	height = ymax - yd;
    }

  /* is the box selected? */
  selected = (pAb->AbSelected || box == pFrame->FrSelectionBegin.VsBox);
  /* Search for the enclosing box */
  if (pAb->AbEnclosing == NULL)
    mbox = box;
  else
    {
      mbox = pAb->AbEnclosing->AbBox;
      if (mbox->BxType == BoGhost)
	{
	  selected = selected || mbox->BxAbstractBox->AbSelected;
	  while (mbox->BxType == BoGhost &&
		 mbox->BxAbstractBox->AbEnclosing != NULL)
	    {
	      mbox = mbox->BxAbstractBox->AbEnclosing->AbBox;
	      selected = selected || mbox->BxAbstractBox->AbSelected;
	    }
	}
    } 

  if (pAb->AbVolume == 0 ||
      (pAb->AbLeafType == LtPolyLine && box->BxNChars == 1))
    {
      /* Empty */
      if (pAb->AbLeafType == LtSymbol)
	DisplayEmptyBox (box, frame, '2', selected);
      else if (pAb->AbLeafType != LtPolyLine && pAb->AbLeafType != LtGraphics)
	{
	   if (selected)
	     DisplayStringSelection (frame, 0, box->BxWidth, box);
	   else if (ThotLocalActions[T_emptybox] != NULL)
	     (*ThotLocalActions[T_emptybox]) (box, frame, '2', selected);
	}
    }
  else if (pAb->AbLeafType == LtText)
    /* Display a Text box */
    DisplayJustifiedText (box, mbox, frame, selected);
  else if (box->BxType == BoPicture)
    /* Picture */
    DisplayImage (box, frame, xmin, xmax, ymin, ymax, selected);
  else if (pAb->AbLeafType == LtSymbol)
    /* Symbol */
    if (pAb->AbShape == EOS)
      DisplayEmptyBox (box, frame, '2', selected);
    else
      DisplaySymbol (box, frame, selected);
  else if (pAb->AbLeafType == LtGraphics)
    /* Graphics */
    if (pAb->AbShape == EOS)
      DisplayEmptyBox (box, frame, '2', selected);
    else
      DisplayGraph (box, frame, selected);
  else if (pAb->AbLeafType == LtPolyLine)
    /* Polyline */
    DrawPolyLine (box, frame, selected);

  /* then display borders */
  if (yd + height >= ymin
      && yd <= ymax
      && xd + width >= xmin
      && xd <= xmax)
    DisplayBorders (box, frame, xd - x, yd - y, width, height);
}
