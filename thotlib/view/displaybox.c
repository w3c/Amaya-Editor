/*
 *
 *  (c) COPYRIGHT INRIA, Grif, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * displaybox.c : all the stuff needed to display boxes in frames.
 *
 * Author: I. Vatton (INRIA)
 *
 */

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

#include "font_f.h"
#include "units_f.h"
#include "displaybox_f.h"
#include "picture_f.h"
#include "buildboxes_f.h"
#include "windowdisplay_f.h"

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
      return PixelValue (pAb->AbLineWeight, pAb->AbLineWeightUnit, pAb);
}


/*----------------------------------------------------------------------
  DisplayImage display a empty box in the frame.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         DisplayImage (PtrBox pBox, int frame)
#else  /* __STDC__ */
static void         DisplayImage (pBox, frame)
PtrBox              pBox;
int                 frame;
#endif /* __STDC__ */
{
   ViewFrame          *pFrame;
   PtrBox              pCurrentBox;
   int                 dx, dy;
   int                 i;
   int                 width, height;
   int                 op, RO;

   if (pBox->BxAbstractBox->AbVisibility >= ViewFrameTable[frame - 1].FrVisibility)
     {
	if (pBox->BxAbstractBox->AbSensitive)
	   op = 1;
	else
	   op = 0;
	if (pBox->BxAbstractBox->AbReadOnly)
	   RO = 1;
	else
	   RO = 0;

	/* compute the location an geometry of the box */
	dx = pBox->BxXOrg;
	dy = pBox->BxYOrg;
	/* box sizes have to be positive */
	width = pBox->BxWidth;
	if (width < 0)
	  width = 0;
	height = pBox->BxHeight;
	if (height < 0)
	  height = 0;

	/* CHKR_LIMIT the area to fill to the englobing box */
	pCurrentBox = pBox->BxAbstractBox->AbEnclosing->AbBox;
	i = pCurrentBox->BxXOrg - dx;
	if (i > 0)
	  {
	     dx = pCurrentBox->BxXOrg;
	     width -= i;
	  }

	i = pCurrentBox->BxXOrg + pCurrentBox->BxWidth - dx;
	if (width > i)
	   width = i;

	i = pCurrentBox->BxYOrg - dy;
	if (i > 0)
	  {
	     dy = pCurrentBox->BxYOrg;
	     height -= i;
	  }
	/* For changing drawing color */
	DrawRectangle (frame, 0, 0, 0, 0, 0, 0, 0, 0, pBox->BxAbstractBox->AbForeground,
		       pBox->BxAbstractBox->AbBackground, 0);

	i = pCurrentBox->BxYOrg + pCurrentBox->BxHeight - dy;
	if (height > i)
	   height = i;

	DrawPicture (pBox, (PictInfo *) pBox->BxPictInfo, frame);

	pFrame = &ViewFrameTable[frame - 1];
	/* Should the end of de line be filled with dots */
	if (pBox->BxEndOfBloc > 0)
	  {
	     /* Compute the origin alignment */
	     dy = pBox->BxYOrg + pBox->BxHorizRef - pFrame->FrYOrg;
	     DrawPoints (frame, pBox->BxXOrg + pBox->BxWidth - pFrame->FrXOrg, dy,
	      pBox->BxEndOfBloc, RO, op, pBox->BxAbstractBox->AbForeground);
	  }
     }
}


/*----------------------------------------------------------------------
  DisplaySymbol displays a mathematical symbols box enclosed in
  a frame. The glyphs are drawn with the Greek font and lines.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         DisplaySymbol (PtrBox pBox, int frame)
#else  /* __STDC__ */
static void         DisplaySymbol (pBox, frame)
PtrBox              pBox;
int                 frame;
#endif /* __STDC__ */
{
   ptrfont             font;
   PtrBox              mbox;
   PtrAbstractBox      pAbbox1;
   ViewFrame          *pFrame;
   boolean             withbackground;
   int                 xd, yd, i;
   int                 fg;
   int                 bg;
   int                 op, RO;
   int                 width, height;

   /* Search for the box defining the enclosing constraints */
   pAbbox1 = pBox->BxAbstractBox->AbEnclosing;
   mbox = pAbbox1->AbBox;

   /* If the englobing abstract box is splitted take the first englobing
      not splitted */
   if (mbox->BxType == BoGhost)
      while (mbox->BxType == BoGhost && mbox->BxAbstractBox->AbEnclosing != NULL)
	 mbox = mbox->BxAbstractBox->AbEnclosing->AbBox;

   fg = pBox->BxAbstractBox->AbForeground;
   bg = pBox->BxAbstractBox->AbBackground;
   withbackground = (pBox->BxAbstractBox->AbFillPattern == 2);
   if (pBox->BxAbstractBox->AbVisibility >= ViewFrameTable[frame - 1].FrVisibility)
      if (mbox->BxXOrg + mbox->BxWidth > pBox->BxXOrg
	  && mbox->BxYOrg + mbox->BxHeight > pBox->BxYOrg)
	{
	   font = pBox->BxFont;
	   if (font != NULL)
	     {
		/* Position in the frame */
		pFrame = &ViewFrameTable[frame - 1];
		xd = pBox->BxXOrg - pFrame->FrXOrg;
		yd = pBox->BxYOrg - pFrame->FrYOrg;
		if (pBox->BxAbstractBox->AbSensitive)
		   op = 1;
		else
		   op = 0;
		if (pBox->BxAbstractBox->AbReadOnly)
		   RO = 1;
		else
		   RO = 0;

		/* box sizes have to be positive */
		width = pBox->BxWidth;
		if (width < 0)
		  width = 0;
		height = pBox->BxHeight;
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
			    DrawHorizontalLine (frame, i, 0, xd, yd, width, height, 1, RO, op, fg);
			    break;
			 case 'v':
			    DrawVerticalLine (frame, i, 0, xd, yd, width, height, 1, RO, op, fg);
			    break;
			 case '>':
			    DrawArrow (frame, i, 0, xd, yd, width, height, 0, RO, op, fg);
			    break;
			 case '^':
			    DrawArrow (frame, i, 0, xd, yd, width, height, 90, RO, op, fg);
			    break;
			 case '<':
			    DrawArrow (frame, i, 0, xd, yd, width, height, 180, RO, op, fg);
			    break;
			 case 'V':
			    DrawArrow (frame, i, 0, xd, yd, width, height, 270, RO, op, fg);
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
		/* Should the end of de line be filled with dots */
		if (pBox->BxEndOfBloc > 0)
		  {
		     pFrame = &ViewFrameTable[frame - 1];
		     /* Compute the origin alignment */
		     yd += pBox->BxHorizRef;
		     DrawPoints (frame, xd + width, yd, pBox->BxEndOfBloc, RO, op, fg);
		  }
	     }
	}
}

/*----------------------------------------------------------------------
  DisplayEmptyBox shows an empty box but formatted and placed.
  A specific background is drawn in the box area.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                DisplayEmptyBox (PtrBox pBox, int frame, char modele)
#else  /* __STDC__ */
void                DisplayEmptyBox (pBox, frame, modele)
PtrBox              pBox;
int                 frame;
char                modele;
#endif /* __STDC__ */
{
   PtrBox              mbox;
   PtrAbstractBox      pAbbox1;
   ViewFrame          *pFrame;
   int                 op, RO;
   int                 xd, yd;
   int                 width, height;

   /* Search for the box defining the enclosing constraints */
   if (pBox->BxAbstractBox->AbEnclosing == NULL)
      mbox = pBox;
   else
     {
	pAbbox1 = pBox->BxAbstractBox->AbEnclosing;
	mbox = pAbbox1->AbBox;
	/* If the englobing abstract box is splitted take the first englobing
	   not splitted */
	if (mbox->BxType == BoGhost)
	   while (mbox->BxType == BoGhost && mbox->BxAbstractBox->AbEnclosing != NULL)
	      mbox = mbox->BxAbstractBox->AbEnclosing->AbBox;
     }

   pFrame = &ViewFrameTable[frame - 1];
   if (pBox->BxAbstractBox->AbVisibility >= pFrame->FrVisibility)
      if (mbox->BxXOrg + mbox->BxWidth > pBox->BxXOrg
	  && mbox->BxYOrg + mbox->BxHeight > pBox->BxYOrg)
	{
	   if (pBox->BxAbstractBox->AbSensitive)
	      op = 1;
	   else
	      op = 0;
	   if (pBox->BxAbstractBox->AbReadOnly)
	      RO = 1;
	   else
	      RO = 0;
	   xd = pBox->BxXOrg - pFrame->FrXOrg;
	   yd = pBox->BxYOrg - pFrame->FrYOrg;
	   /* box sizes have to be positive */
	   width = pBox->BxWidth;
	   if (width < 0)
	     width = 0;
	   height = pBox->BxHeight;
	   if (height < 0)
	     height = 0;

	   if (pBox->BxAbstractBox->AbLeafType == LtGraphics)
	      DrawRectangle (frame, 2, 0, xd, yd, width,
		  height, RO, op, pBox->BxAbstractBox->AbForeground,
			     pBox->BxAbstractBox->AbBackground, 0);
	   else
	      PaintWithPattern (frame, xd, yd, width, height, 0, RO, op,
		     pBox->BxAbstractBox->AbForeground,
		     pBox->BxAbstractBox->AbBackground, 4);
	}
}


/*----------------------------------------------------------------------
  DisplayGraph display a graphic box enclosed in frame.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         DisplayGraph (PtrBox pBox, int frame)
#else  /* __STDC__ */
static void         DisplayGraph (pBox, frame)
PtrBox              pBox;
int                 frame;
#endif /* __STDC__ */
{
   int                 i, xd, yd;
   PtrBox              mbox;
   int                 op, RO;
   int                 fg;
   int                 bg;
   int                 style;
   int                 width, height;
   PtrAbstractBox      pv;
   ViewFrame          *pFrame;

   /* Search for the box defining the enclosing constraints */
   if (pBox->BxAbstractBox->AbEnclosing == NULL)
      mbox = pBox;
   else
     {
	pv = pBox->BxAbstractBox->AbEnclosing;
	mbox = pv->AbBox;
	/* If the englobing abstract box is splitted take the first englobing
	   not splitted */
	if (mbox->BxType == BoGhost)
	   while (mbox->BxType == BoGhost && mbox->BxAbstractBox->AbEnclosing != NULL)
	      mbox = mbox->BxAbstractBox->AbEnclosing->AbBox;
     }

   fg = pBox->BxAbstractBox->AbForeground;
   bg = pBox->BxAbstractBox->AbBackground;
   if (pBox->BxAbstractBox->AbVisibility >= ViewFrameTable[frame - 1].FrVisibility)
      if (mbox->BxXOrg + mbox->BxWidth >= pBox->BxXOrg
	  && mbox->BxYOrg + mbox->BxHeight >= pBox->BxYOrg)
	{
	   pFrame = &ViewFrameTable[frame - 1];
	   xd = pBox->BxXOrg - pFrame->FrXOrg;
	   yd = pBox->BxYOrg - pFrame->FrYOrg;
	   if (pBox->BxAbstractBox->AbSensitive)
	      op = 1;
	   else
	      op = 0;
	   if (pBox->BxAbstractBox->AbReadOnly)
	      RO = 1;
	   else
	      RO = 0;

	   /* box sizes have to be positive */
	   width = pBox->BxWidth;
	   if (width < 0)
	     width = 0;
	   height = pBox->BxHeight;
	   if (height < 0)
	     height = 0;

	   pv = pBox->BxAbstractBox;
	   /* Style and thickness of drawing */
	   i = GetLineWeight (pv);
	   switch (pv->AbLineStyle)
		 {
		    case 'S':
		       style = 0;
		       break;
		    case '-':
		       style = 3;
		       break;
		    case '.':
		       style = 1;
		       break;
		    default:
		       style = 0;
		 }

	   switch (pv->AbRealShape)
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
		    case '1':
		    case '2':
		    case '3':
		    case '4':
		    case '5':
		    case '6':
		    case '7':
		    case '8':
		    case 'R':
		       DrawRectangle (frame, i, style, xd, yd, width,
			 height, RO, op, fg, bg, pv->AbFillPattern);
		       break;
		    case 'C':
		       DrawOval (frame, i, style, xd, yd, width,
			 height, RO, op, fg, bg, pv->AbFillPattern);
		       break;
		    case 'L':
		       DrawDiamond (frame, i, style, xd, yd, width,
			 height, RO, op, fg, bg, pv->AbFillPattern);
		       break;
		    case 'c':
		       DrawEllips (frame, i, style, xd, yd, width,
			 height, RO, op, fg, bg, pv->AbFillPattern);
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
		       DrawSlash (frame, i, style, xd, yd, width,
				  height, 0, RO, op, fg);
		       break;
		    case '\\':
		       DrawSlash (frame, i, style, xd, yd, width,
				  height, 1, RO, op, fg);
		       break;
		    case '>':
		       DrawArrow (frame, i, style, xd, yd, width, height, 0, RO, op, fg);
		       break;
		    case 'E':
		       DrawArrow (frame, i, style, xd, yd, width,
				  height, 45, RO, op, fg);
		       break;
		    case '^':
		       DrawArrow (frame, i, style, xd, yd, width,
				  height, 90, RO, op, fg);
		       break;
		    case 'O':
		       DrawArrow (frame, i, style, xd, yd, width,
				  height, 135, RO, op, fg);
		       break;
		    case '<':
		       DrawArrow (frame, i, style, xd, yd, width,
				  height, 180, RO, op, fg);
		       break;
		    case 'o':
		       DrawArrow (frame, i, style, xd, yd, width,
				  height, 225, RO, op, fg);
		       break;
		    case 'V':
		       DrawArrow (frame, i, style, xd, yd, width,
				  height, 270, RO, op, fg);
		       break;
		    case 'e':
		       DrawArrow (frame, i, style, xd, yd, width,
				  height, 315, RO, op, fg);
		       break;

		    case 'P':
		       DrawRectangleFrame (frame, i, style, xd, yd, width,
			 height, RO, op, fg, bg, pv->AbFillPattern);
		       break;
		    case 'Q':
		       DrawEllipsFrame (frame, i, style, xd, yd, width,
			 height, RO, op, fg, bg, pv->AbFillPattern);
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
		/* Compute the origin alignment */
		yd += pBox->BxHorizRef;
		DrawPoints (frame, xd + width, yd, pBox->BxEndOfBloc, RO, op, fg);
	     }
	}
}


/*----------------------------------------------------------------------
  PolyTransform check whether a polyline Box need to be transformed
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
   width = pBox->BxWidth;
   if (width < 0)
     width = 0;
   height = pBox->BxHeight;
   if (height < 0)
     height = 0;

   /* Compute ratio for axis X */
   val = PixelToPoint (width * 1000);
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
   val = PixelToPoint (height * 1000);
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
static void         DrawPolyLine (PtrBox pBox, int frame)
#else  /* __STDC__ */
static void         DrawPolyLine (pBox, frame)
PtrBox              pBox;
int                 frame;

#endif /* __STDC__ */
{
   PtrBox              mbox;
   PtrAbstractBox      pAb;
   ViewFrame          *pFrame;
   int                 i, xd, yd;
   int                 op, RO;
   int                 fg;
   int                 bg;
   int                 style, arrow;
   int                 width;

   /* If no point is defined, no need to draw it */
   if (pBox->BxBuffer == NULL || pBox->BxNChars <= 1)
      return;

   /* Transform the polyline if the box size has changed */
   PolyTransform (pBox);

   /* Search for the box defining the enclosing constraints */
   if (pBox->BxAbstractBox->AbEnclosing == NULL)
      mbox = pBox;
   else
     {
	pAb = pBox->BxAbstractBox->AbEnclosing;
	mbox = pAb->AbBox;
	/* If the englobing abstract box is splitted take the first englobing
	   not splitted */
	if (mbox->BxType == BoGhost)
	   while (mbox->BxType == BoGhost && mbox->BxAbstractBox->AbEnclosing != NULL)
	      mbox = mbox->BxAbstractBox->AbEnclosing->AbBox;
     }

   fg = pBox->BxAbstractBox->AbForeground;
   bg = pBox->BxAbstractBox->AbBackground;
   if (pBox->BxAbstractBox->AbVisibility >= ViewFrameTable[frame - 1].FrVisibility)
      if (mbox->BxXOrg + mbox->BxWidth >= pBox->BxXOrg
	  && mbox->BxYOrg + mbox->BxHeight >= pBox->BxYOrg)
	{
	   pFrame = &ViewFrameTable[frame - 1];
	   xd = pBox->BxXOrg - pFrame->FrXOrg;
	   yd = pBox->BxYOrg - pFrame->FrYOrg;
	   if (pBox->BxAbstractBox->AbSensitive)
	      op = 1;
	   else
	      op = 0;

	   if (pBox->BxAbstractBox->AbReadOnly)
	      RO = 1;
	   else
	      RO = 0;

	   /* box sizes have to be positive */
	   width = pBox->BxWidth;
	   if (width < 0)
	     width = 0;

	   pAb = pBox->BxAbstractBox;
	   /* Style and thickness of the line */
	   i = GetLineWeight (pAb);
	   switch (pAb->AbLineStyle)
		 {
		    case 'S':
		       style = 0;
		       break;
		    case '-':
		       style = 3;
		       break;
		    case '.':
		       style = 1;
		       break;
		    default:
		       style = 0;
		 }

	   switch (pAb->AbPolyLineShape)
		 {
		    case 'S':	/* Segments */
		    case 'U':	/* Segments forward arrow */
		    case 'N':	/* Segments backward arrow */
		    case 'M':	/* Segments arrows on both directions */
		       if (pAb->AbPolyLineShape == 'S')
			  arrow = 0;
		       else if (pAb->AbPolyLineShape == 'U')
			  arrow = 1;
		       else if (pAb->AbPolyLineShape == 'N')
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
			 pBox->BxNChars, RO, op, fg, bg, pAb->AbFillPattern);
		       break;
		    case 's':	/* closed spline */
		       /* compute control points */
		       if (pBox->BxPictInfo == NULL)
			  pBox->BxPictInfo = (int *) ComputeControlPoints (pBox->BxBuffer, pBox->BxNChars);
		       DrawSpline (frame, i, style, xd, yd, pBox->BxBuffer,
				   pBox->BxNChars, RO, op, fg, bg, pAb->AbFillPattern, (C_points *) pBox->BxPictInfo);
		       break;
		    default:
		       break;
		 }

	   if (pBox->BxEndOfBloc > 0)
	     {
		/* Compute the origin alignment */
		yd += pBox->BxHorizRef;
		DrawPoints (frame, xd + width, yd, pBox->BxEndOfBloc, RO, op, fg);
	     }
	}
}


/*----------------------------------------------------------------------
  DisplayJustifiedText display the content of a Text box tweaking
  the space sizes to ajust line length to the size of the frame.
  Remaining pixel space (BxNPixels) is equally dispatched 
  on all spaces in the line.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         DisplayJustifiedText (PtrBox pBox, int frame)
#else  /* __STDC__ */
static void         DisplayJustifiedText (pBox, frame)
PtrBox              pBox;
int                 frame;

#endif /* __STDC__ */
 {
   PtrTextBuffer       adbuff;
   PtrTextBuffer       newbuff;
   PtrAbstractBox      pAbbox1;
   ViewFrame          *pFrame;
   unsigned char       car;
   PtrBox              mbox;
   int                 indbuff;
   int                 restbl;
   int                 newind;
   int                 newbl, lg;
   int                 charleft, dc;
   int                 buffleft;
   int                 indmax, bl;
   int                 nbcar, x, y;
   int                 lgspace;
   int                 fg;
   int                 bg;
   int                 RO;
   int                 op;
   int                 width;
   boolean             blockbegin;
   boolean             withbackground;
   boolean             withline;

   indmax = 0;
   buffleft = 0;
   /* Search for the enclosing box defining the size constraints */
   if (pBox->BxAbstractBox->AbEnclosing == NULL)
      mbox = pBox;
   else
     {
	pAbbox1 = pBox->BxAbstractBox->AbEnclosing;
	mbox = pAbbox1->AbBox;

	/* Si le pave englobant est eclate on saute au pave englobant entier */
	if (mbox->BxType == BoGhost)
	   while (mbox->BxType == BoGhost && mbox->BxAbstractBox->AbEnclosing != NULL)
	      mbox = mbox->BxAbstractBox->AbEnclosing->AbBox;
     }

   /* Is this bos the first of a lines block of isolated text */
   if (mbox == pBox)
      blockbegin = TRUE;
   else if (mbox->BxType != BoBlock || mbox->BxFirstLine == NULL)
      blockbegin = TRUE;
   else if (pBox->BxType == BoComplete && mbox->BxFirstLine->LiFirstBox == pBox)
      blockbegin = TRUE;
   else if ((pBox->BxType == BoPiece || pBox->BxType == BoDotted)
	    && mbox->BxFirstLine->LiFirstPiece == pBox)
      blockbegin = TRUE;
   else
      blockbegin = FALSE;

   /* Is an hyphenation mark needed at the end of the box ? */
   if (pBox->BxType == BoDotted)
      withline = TRUE;
   else
      withline = FALSE;

   fg = pBox->BxAbstractBox->AbForeground;
   bg = pBox->BxAbstractBox->AbBackground;
   withbackground = (pBox->BxAbstractBox->AbFillPattern == 2);

   if (pBox->BxNChars > 0)
      if (pBox->BxAbstractBox->AbVisibility >= ViewFrameTable[frame - 1].FrVisibility)
	 if (mbox->BxXOrg + mbox->BxWidth > pBox->BxXOrg
	     && mbox->BxYOrg + mbox->BxHeight > pBox->BxYOrg)
	   {
	      /* Initialization */
	      /* -------------- */
	      pFrame = &ViewFrameTable[frame - 1];
	      x = pBox->BxXOrg - pFrame->FrXOrg;
	      y = pBox->BxYOrg - pFrame->FrYOrg;
	      bl = 0;
	      newind = pBox->BxFirstChar;
	      newbuff = pBox->BxBuffer;
	      charleft = pBox->BxNChars;
	      newbl = pBox->BxNPixels;
	      lg = 0;
	      if (pBox->BxAbstractBox->AbSensitive)
		 op = 1;
	      else
		 op = 0;
	      if (pBox->BxAbstractBox->AbReadOnly)
		 RO = 1;
	      else
		 RO = 0;

	      /* box sizes have to be positive */
	      width = pBox->BxWidth;
	      if (width < 0)
		width = 0;

	      lgspace = pBox->BxSpaceWidth;
	      if (lgspace == 0)
		 lgspace = CharacterWidth (_SPACE_, pBox->BxFont);

	      /* Search the first displayable char */
	      /* --------------------------------- */
	      if (charleft > 0)
		 do
		   {
		      adbuff = newbuff;
		      indbuff = newind;
		      restbl = newbl;
		      x += lg;
		      car = (unsigned char) (adbuff->BuContent[indbuff - 1]);
		      if (car == _SPACE_)
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
		 DrawRectangle (frame, 0, 0, x, y,
			  width + pBox->BxXOrg - pFrame->FrXOrg - x,
				FontHeight (pBox->BxFont), 0, 0, 0, bg, 2);
	      while (charleft > 0)
		{
		   /* handle each char in the buffer */
		   while (indbuff <= indmax)
		     {
			car = (unsigned char) (adbuff->BuContent[indbuff - 1]);

			if (car == _SPACE_ || car == THIN_SPACE
			    || car == HALF_EM || car == UNBREAKABLE_SPACE)
			  {
			     /* display the last chars handled */
			     dc = indbuff - nbcar;
			     x += DrawString (adbuff->BuContent, dc, nbcar, frame, x, y,
			     pBox->BxFont, 0, bl, 0, blockbegin, RO, op, fg);

			     if (!ShowSpace)
			       {
				  /* Show the space chars */
				  if (car == _SPACE_)
				     DrawChar (SHOWN_SPACE, frame, x, y, pBox->BxFont, RO, op, fg);
				  else if (car == THIN_SPACE)
				     DrawChar (SHOWN_THIN_SPACE, frame, x, y, pBox->BxFont, RO, op, fg);
				  else if (car == HALF_EM)
				     DrawChar (SHOWN_HALF_EM, frame, x, y, pBox->BxFont, RO, op, fg);
				  else if (car == UNBREAKABLE_SPACE)
				     DrawChar (SHOWN_UNBREAKABLE_SPACE, frame, x, y, pBox->BxFont, RO, op, fg);
			       }

			     nbcar = 0;
			     if (car == _SPACE_)
				if (restbl > 0)
				  {
				     /* Pixel space splitting */
				     x = x + lgspace + 1;
				     restbl--;
				  }
				else
				   x += lgspace;
			     else
				x += CharacterWidth (car, pBox->BxFont);

			     bl = 1;
			  }

			else
			   /* Just add the next char */
			   nbcar++;

			indbuff++;	/* Skip to next char */
		     }

		   /* Draw the last chars from buffer */
		   dc = indbuff - nbcar;
		   charleft -= buffleft;
		   if (charleft <= 0)
		     {
			/* Finished */
			x += DrawString (adbuff->BuContent, dc, nbcar, frame, x, y, pBox->BxFont,
			width, bl, withline, blockbegin, RO, op, fg);
			if (pBox->BxUnderline != 0)
			   DisplayUnderline (frame, x, y, pBox->BxFont, pBox->BxUnderline,
			        pBox->BxThickness, pBox->BxWidth, RO, op, fg);
			/* Next char lookup */
			if ((unsigned char) adbuff->BuContent[indbuff - 1] == BREAK_LINE && !ShowSpace)
			   DrawChar (SHOWN_BREAK_LINE, frame, x, y, pBox->BxFont, RO, op, fg);
		     }
		   else
		     {
			x += DrawString (adbuff->BuContent, dc, nbcar, frame, x, y,
			    pBox->BxFont, 0, bl, 0, blockbegin, RO, op, fg);
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
		   pFrame = &ViewFrameTable[frame - 1];
		   /* Compute the origin alignment */
		   y = pBox->BxYOrg + pBox->BxHorizRef - pFrame->FrYOrg;
		   DrawPoints (frame, pBox->BxXOrg + width - pFrame->FrXOrg, y,
			       pBox->BxEndOfBloc, RO, op, fg);
		}
	   }
}


/*----------------------------------------------------------------------
  DisplayBox display a box depending on it's content.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                DisplayBox (PtrBox pBox, int frame)
#else  /* __STDC__ */
void                DisplayBox (pBox, frame)
PtrBox              pBox;
int                 frame;
#endif /* __STDC__ */
{
   /* Empty */
   if (pBox->BxAbstractBox->AbVolume == 0) {
      if (pBox->BxAbstractBox->AbLeafType == LtSymbol)
         DisplayEmptyBox (pBox, frame, '2');
      else if (ThotLocalActions[T_emptybox] != NULL)
           (*ThotLocalActions[T_emptybox]) (pBox, frame, '2');
   } else if (pBox->BxAbstractBox->AbLeafType == LtText) /* Text */
          /* Display a Text box */
          DisplayJustifiedText (pBox, frame);
   else if (pBox->BxType == BoPicture) /* Picture */
        DisplayImage (pBox, frame);
   else if (pBox->BxAbstractBox->AbLeafType == LtSymbol) /* Symbol */
        if (pBox->BxAbstractBox->AbShape == EOS)
           DisplayEmptyBox (pBox, frame, '2');
        else
           DisplaySymbol (pBox, frame);
   else if (pBox->BxAbstractBox->AbLeafType == LtGraphics) /* Graphics */
        if (pBox->BxAbstractBox->AbShape == EOS)
           DisplayEmptyBox (pBox, frame, '2');
        else
           DisplayGraph (pBox, frame);
   else if (pBox->BxAbstractBox->AbLeafType == LtPolyLine) /* Polyline */
        if (pBox->BxNChars == 1)
           DisplayEmptyBox (pBox, frame, '2');
        else
            DrawPolyLine (pBox, frame);
}
