/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2003
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * displaybox.c : all the stuff needed to display boxes in frames.
 *
 * Authors: I. Vatton (INRIA)
 *          P. Cheyrou-lagreze (INRIA)
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
#include "memory_f.h"
#include "picture_f.h"
#include "units_f.h"
#include "xwindowdisplay_f.h"

#ifdef _GL
#include <GL/gl.h>
#include "glwindowdisplay.h"
#include "openglfont.h"
#include "frame_f.h"
#include "appli_f.h"
#include "glgradient_f.h"
#endif /*_GL*/

#include "stix.h"
#include "fileaccess.h"



/*-------------------------------------------------------------------------
  NextChar returns the previous character of the same buffer according
  to the index given by ind and according to the orientation.it returns 
  space if it pass to another buffer
  -------------------------------------------------------------------------*/
static CHAR_T Previous_Char ( PtrTextBuffer *adbuff, int *ind )
{
  if (*ind < ( (*adbuff)->BuLength-1))
    return ((*adbuff)->BuContent[(*ind)+1]);
  else 
    return (0x0020);/*SPACE*/
}

/*------------------------------------------------------------------------
  PreviousChar do the same as NextChar but in the opposite way.It gives
  the previous Character.
  ------------------------------------------------------------------------*/
static CHAR_T Next_Char ( PtrTextBuffer *adbuff, int *ind )
{
  if (*ind > 0)
    return ((*adbuff)->BuContent[(*ind)-1]);
  else 
    return (0x0020);/*SPACE*/
}
  

/*----------------------------------------------------------------------
  GetLineWeight computes the line weight of an abstract box.
  ----------------------------------------------------------------------*/
static int GetLineWeight (PtrAbstractBox pAb, int frame)
{
  if (pAb == NULL)
    return (0);
  else
    return PixelValue (pAb->AbLineWeight, pAb->AbLineWeightUnit, pAb,
		       ViewFrameTable[frame - 1].FrMagnification);
}


/*----------------------------------------------------------------------
  DisplayImage displays an image in the frame.
  ----------------------------------------------------------------------*/
static void DisplayImage (PtrBox pBox, int frame, int xmin, int xmax,
			  int ymin, int ymax, ThotBool selected)
{
  ViewFrame          *pFrame;
  int                 xd, yd, x, y;
  int                 width, height;

  pFrame = &ViewFrameTable[frame - 1];
  if (pBox->BxAbstractBox->AbVisibility >= pFrame->FrVisibility)
    {
      /* For changing drawing color */
      DrawRectangle (frame, 0, 0, 0, 0, 0, 0,
      		     pBox->BxAbstractBox->AbForeground,
      		     pBox->BxAbstractBox->AbBackground, 0);
      x = pFrame->FrXOrg;
      y = pFrame->FrYOrg;
      xd = pBox->BxXOrg + pBox->BxLMargin + pBox->BxLBorder +
           pBox->BxLPadding - x;
      yd = pBox->BxYOrg + pBox->BxTMargin + pBox->BxTBorder +
           pBox->BxTPadding + FrameTable[frame].FrTopMargin - y;
      width = pBox->BxW;
      height = pBox->BxH;
      DrawPicture (pBox, (PictInfo *) pBox->BxPictInfo, frame, xd, yd, width,
	height);
      /* Should the end of de line be filled with dots */
      if (pBox->BxEndOfBloc > 0)
	{
	  /* fill the end of the line with dots */
	  xd = pBox->BxXOrg + pBox->BxLMargin + pBox->BxLBorder +
               pBox->BxLPadding - x;
	  yd = pBox->BxYOrg + pBox->BxHorizRef - y;
	  DrawPoints (frame, xd + width, yd, pBox->BxEndOfBloc,
		      pBox->BxAbstractBox->AbForeground);
	}

      /* show the selection on the whole image */
      if (selected)
	{
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
}
#ifdef _WINDOWS
/*----------------------------------------------------------------------
  WinFontExist : Test existence of a font based on its filename
  as the Windows API fucntion CreateFont always return a font 
  even it's not the one we want (win2000)
  ----------------------------------------------------------------------*/
ThotBool WinFontExist (char *fontname)
{
	static unsigned char Exists = 'D';
	char filename [MAX_LENGTH];

	if (Exists == 'T')
		return TRUE;
	else if (Exists == 'F')
		return FALSE;
	else
	{
		
	  GetWindowsDirectory (filename , 1024);  
	  strcat (filename, "\\fonts\\"); 
	  strcat (filename, fontname); 
	  if (TtaFileExist (filename))
		{
			Exists = 'T';
			return TRUE;
		}
		else
		{
			Exists = 'F';
			return FALSE;
		}

	}
}
#endif /*_WINDOWS*/

/*----------------------------------------------------------------------
  DisplaySymbol displays a mathematical symbols box enclosed in
  a frame. The glyphs are drawn with the Greek font and lines.
  ----------------------------------------------------------------------*/
static void DisplaySymbol (PtrBox pBox, int frame, ThotBool selected)
{ 
  PtrFont             font;
  ViewFrame          *pFrame;
  ThotBool            withbackground;
  int                 xd, yd, i, w;
  int                 fg, bg;
  int                 width, height;
  ThotBool            StixExist;

 
  fg = pBox->BxAbstractBox->AbForeground;
  bg = pBox->BxAbstractBox->AbBackground;
  withbackground = (pBox->BxFill && pBox->BxDisplay);
  pFrame = &ViewFrameTable[frame - 1];
  if (pBox->BxAbstractBox->AbVisibility >= pFrame->FrVisibility)
    {
      font = NULL;
#ifdef _WINDOWS
	if (WinFontExist ("esstix6_.ttf"))
#endif /*_WINDOWS*/
	  if (pBox->BxH > 0) 
	    {
	      GetMathFontFromChar (pBox->BxAbstractBox->AbShape,
				   pBox->BxFont,
				   (void **) &font,
				   SizetoLogical(pBox->BxH-5));
	    }
      if (font == NULL)
	{
	  GetFontAndIndexFromSpec (32, pBox->BxFont, &font);
	  StixExist = FALSE;
	}
      else
	StixExist = TRUE;

      if (font != NULL)
	{
	  /* Position in the frame */
	  xd = pBox->BxXOrg + pBox->BxLMargin + pBox->BxLBorder +
	    pBox->BxLPadding - pFrame->FrXOrg;
	  yd = pBox->BxYOrg + pBox->BxTMargin + pBox->BxTBorder +
	    pBox->BxTPadding - pFrame->FrYOrg;
	  
	  /* box sizes have to be positive */
	  width = pBox->BxW;
	  if (width < 0)
	    width = 0;
	  height = pBox->BxH;
	  if (height < 0)
	    height = 0;
	  
	  if (withbackground)
	    /* display the background selection */
	    DrawRectangle (frame, 0, 0, xd, yd, width, height, 0, bg, 2);
	  if (selected &&
	      !pFrame->FrSelectOnePosition &&
	      pFrame->FrSelectionBegin.VsXPos != pBox->BxW)
	    DisplayStringSelection (frame, 0, pBox->BxW, pBox);
	  
	  /* Line thickness */
	  i = GetLineWeight (pBox->BxAbstractBox, frame);
	  
#ifdef _GL
  SetTextureScale (IsBoxDeformed(pBox));
#endif /* _GL */

	  switch (pBox->BxAbstractBox->AbShape)
	    {
	    case 'r':
	      DrawRadical (frame, i, xd, yd, width, height, font, fg);
	      break;
	    case 'i':
	      if (StixExist)
		DrawStixIntegral (frame, i, xd, yd, width, height, 0, font, fg);
	      else
		DrawIntegral (frame, i, xd, yd, width, height, 0, font, fg);
	      break;
	    case 'c':
	      if (StixExist)
		DrawStixIntegral (frame, i, xd, yd, width, height, 1, font, fg);
	      else
		DrawIntegral (frame, i, xd, yd, width, height, 1, font, fg);
	      break;
	    case 'd':
	      if (StixExist)
		DrawStixIntegral (frame, i, xd, yd, width, height, 2, font, fg);
	      else
		DrawIntegral (frame, i, xd, yd, width, height, 2, font, fg);
	      break;
	    case 'S':
	      if (StixExist)
		DrawStixSigma (frame, xd, yd, width, height, font, fg);
	      else
		DrawSigma (frame, xd, yd, width, height, font, fg);
	      break;
	    case 'P':
	      DrawPi (frame, xd, yd, width, height, font, fg);
	      break;
	    case 'I':
	      DrawIntersection (frame, xd, yd, width, height, font, fg);
	      break;
	    case 'U':
	      DrawUnion (frame, xd, yd, width, height, font, fg);
	      break;
	    case 'o':
	      DrawHorizontalBrace (frame, i, 5, xd, yd, width, height, 0, fg);
	      break;
	    case 'u':
	      DrawHorizontalBrace (frame, i, 5, xd, yd, width, height, 1, fg);
	      break;
	    case 'h':
	      DrawHorizontalLine (frame, i, 5, xd, yd, width, height, 1, fg);
	      break;
	    case 'v':
	      DrawVerticalLine (frame, i, 5, xd, yd, width, height, 1,
				fg);
	      break;
	    case 'R':
	      DrawArrow (frame, i, 5, xd, yd, width, height, 0, fg);
	      break;
	    case '^':
	      DrawArrow (frame, i, 5, xd, yd, width, height, 90, fg);
	      break;
	    case 'L':
	      DrawArrow (frame, i, 5, xd, yd, width, height, 180, fg);
	      break;
	    case 'V':
	      DrawArrow (frame, i, 5, xd, yd, width, height, 270, fg);
	      break;
	    case '(':
	      if (StixExist)
		DrawStixParenthesis (frame, i, xd, yd, width, height, 0, font, fg);
	      else
		DrawParenthesis (frame, i, xd, yd, width, height, 0, font, fg);
	      break;
	    case ')':
	      if (StixExist)
		DrawStixParenthesis (frame, i, xd, yd, width, height, 1, font, fg);
	      else
		DrawParenthesis (frame, i, xd, yd, width, height, 1, font, fg);
	      break;
	    case '{':
	      if (StixExist)
		DrawStixBrace (frame, i, xd, yd, width, height, 0, font, fg);
	      else
		DrawBrace (frame, i, xd, yd, width, height, 0, font, fg);
	      break;
	    case '}':
	      if (StixExist)
		DrawStixBrace (frame, i, xd, yd, width, height, 1, font, fg);
	      else
		DrawBrace (frame, i, xd, yd, width, height, 1, font, fg);
	      break;
	    case '[':
	      if (StixExist)
		DrawStixBracket (frame, i, xd, yd, width, height, 0, font, fg);
	      else
		DrawBracket (frame, i, xd, yd, width, height, 0, font, fg);
	      break;
	    case ']':
	      if (StixExist)
		DrawStixBracket (frame, i, xd, yd, width, height, 1, font, fg);
	      else
		DrawBracket (frame, i, xd, yd, width, height, 1, font, fg);
	      break;
	    case '<':
	      if (StixExist)
		DrawStixPointyBracket (frame, i, xd, yd, width, height, 0, font, fg);
	      else
		DrawPointyBracket (frame, i, xd, yd, width, height, 0, font, fg);
	      break;
	    case '>':
	      if (StixExist)
		DrawStixPointyBracket (frame, i, xd, yd, width, height, 1, font, fg);
	      else
		DrawPointyBracket (frame, i, xd, yd, width, height, 1, font, fg);
	      break;
	    case '|':
	      DrawVerticalLine (frame, i, 5, xd, yd, width, height, 1, fg);
	      break;
	    case 'D':
	      DrawDoubleVerticalLine (frame, i, 5, xd, yd, width, height, 1, fg);
	      break;
	    case '?':
	    case UNDISPLAYED_UNICODE:
	      /* Thot does not know how to display that symbol or character */
	      /* Draw a box instead and leave some space (1 pixel) to the
		 right and the bottom to avoid collision with the next char */
	      if (width > 3)
		w = width - 1;
	      else
		w = width;
	      /* in SVG foreground and background are inverted in the main
		 view */
	      if (!strcmp(pBox->BxAbstractBox->AbElement->ElStructSchema->SsName, "SVG") &&
		  FrameTable[frame].FrView == 1)
		fg = pBox->BxAbstractBox->AbBackground;
	      DrawRectangle (frame, 1, 5, xd, yd, w, height-1, fg, 0, 0);
	      break;
	    default:
	      DrawChar (pBox->BxAbstractBox->AbShape, frame, xd,
			yd + FontBase (font), font, fg);
	      break;
	    } 
#ifdef _GL
      StopTextureScale ();
#endif /* _GL */

	  if (pBox->BxEndOfBloc > 0)
	    {
	      /* fill the end of the line with dots */
	      yd = pBox->BxYOrg + pBox->BxHorizRef - pFrame->FrYOrg;
	      DrawPoints (frame, xd + width, yd, pBox->BxEndOfBloc, fg);
	    }
	  
	  /* show the selection on the beginning or the end of the image */
	  if (selected &&
	      (pFrame->FrSelectOnePosition ||
	       pFrame->FrSelectionBegin.VsXPos == pBox->BxW))
	    /* display a carret 
	       or the selection starts at the end of the box */
	    DisplayStringSelection (frame,
				    pFrame->FrSelectionBegin.VsXPos,
				    pFrame->FrSelectionBegin.VsXPos + 2,
				    pBox);
	}
    }
}

/*----------------------------------------------------------------------
  DisplayEmptyBox shows an empty box but formatted and placed.
  A specific background is drawn in the box area.
  ----------------------------------------------------------------------*/
void DisplayEmptyBox (PtrBox pBox, int frame, ThotBool selected)
{
  ViewFrame          *pFrame;
  PtrAbstractBox      pAb;
  int                 bg;
  int                 xd, yd;
  int                 width, height;

  pFrame = &ViewFrameTable[frame - 1];
  pAb = pBox->BxAbstractBox;
  if (pAb->AbVisibility >= pFrame->FrVisibility)
    {
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
			   height, pAb->AbForeground,
			   bg, 0);
	  else
	    {
	      PaintWithPattern (frame, xd, yd, width, height, 0,
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
			   height, pAb->AbForeground,
			   bg, 0);
	  else
	    PaintWithPattern (frame, xd, yd, width, height, 0,
			      pAb->AbForeground,
			      bg, 4);
	}
      
    }
}
#ifdef _GL
/*----------------------------------------------------------------------
  DisplayGraph display a graphic.
  The parameter selected is TRUE when the graphic is selected.
  ----------------------------------------------------------------------*/
void  DisplayGraph (PtrBox pBox, int frame, ThotBool selected)
{
  ViewFrame          *pFrame;
  PtrAbstractBox      pAb;
  int                 i;
  int                 fg, bg;
  int                 pat;
  int                 style;
  float               xd, yd;
  float               width, height;

  pAb = pBox->BxAbstractBox;
  pFrame = &ViewFrameTable[frame - 1];
  if (pAb->AbVisibility >= pFrame->FrVisibility)
    {
      bg = pAb->AbBackground;
      pat = pAb->AbFillPattern;
      fg = pAb->AbForeground;
      xd = pBox->BxXOrg + (float) (pBox->BxLMargin + pBox->BxLBorder +
	pBox->BxLPadding - pFrame->FrXOrg);
      yd = pBox->BxYOrg + (float) (pBox->BxTMargin + pBox->BxTBorder +
	pBox->BxTPadding - pFrame->FrYOrg);
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
      i = GetLineWeight (pAb, frame);
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
	  DrawRectangle (frame, 0, 0, xd, yd, width, height, fg,
			 bg, 2);
	  break;
	case '\261':
	  DrawRectangle (frame, 0, 0, xd, yd, width, height, fg,
			 bg, 5);
	  break;
	case '\262':
	  DrawRectangle (frame, 0, 0, xd, yd, width, height, fg,
			 bg, 6);
	  break;
	case '\263':
	  DrawRectangle (frame, 0, 0, xd, yd, width, height, fg,
			 bg, 7);
	  break;
	case '\264':
	  DrawRectangle (frame, 0, 0, xd, yd, width, height, fg,
			 bg, 8);
	  break;
	case '\265':
	  DrawRectangle (frame, 0, 0, xd, yd, width, height, fg,
			 bg, 9);
	  break;
	case '\266':
	  DrawRectangle (frame, 0, 0, xd, yd, width, height, fg,
			 bg, 1);
	  break;
	case '\267':
	  DrawRectangle (frame, 0, 0, xd, yd, width, height, fg,
			 bg, 0);
	  break;
	case '\270':
	  DrawRectangle (frame, 0, 0, xd, yd, width, height, fg,
			 bg, 4);
	  break;
	case '0':
	  FDrawRectangle (frame, 0, 0, xd, yd, width, height, fg,
			 bg, pat);
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
	  FDrawRectangle (frame, i, style, xd, yd, width, height,
			 fg, bg, pat);
	  break;
	case 'g':
	  /* Coords of the line are given by the enclosing box */
	  pAb = pAb->AbEnclosing;
	  if ((pAb->AbHorizPos.PosEdge == Left &&
	       pAb->AbVertPos.PosEdge == Top) ||
	      (pAb->AbHorizPos.PosEdge == Right &&
	       pAb->AbVertPos.PosEdge == Bottom))
	    /* draw a \ */
	    DrawSlash (frame, i, style, xd, yd, width, height, 1, fg);
	  else
	    /* draw a / */
	    DrawSlash (frame, i, style, xd, yd, width, height, 0, fg);
	  break;
	case 'C':
	  if (pBox->BxRx == 0 && pBox->BxRy == 0)	    
	    FDrawRectangle (frame, i, style, xd, yd, width, height, fg, bg, pat);
	  else
	    DrawOval (frame, i, style, xd, yd, width, height, pBox->BxRx,
		      pBox->BxRy, fg, bg, pat);
	  break;
	case 'L':
	  DrawDiamond (frame, i, style, xd, yd, width, height, fg, bg, pat);
	  break;
	case 'a':
	case 'c':
	  DrawEllips (frame, i, style, xd, yd, width, height, fg, bg, pat);
	  break;
	case 'h':
	  DrawHorizontalLine (frame, i, style, xd, yd, width, height, 1, fg);
	  break;
	case 't':
	  DrawHorizontalLine (frame, i, style, xd, yd, width, height, 0, fg);
	  break;
	case 'b':
	  DrawHorizontalLine (frame, i, style, xd, yd, width, height, 2, fg);
	  break;
	case 'v':
	  DrawVerticalLine (frame, i, style, xd, yd, width, height, 1, fg);
	  break;
	case 'l':
	  DrawVerticalLine (frame, i, style, xd, yd, width, height, 0, fg);
	  break;
	case 'r':
	  DrawVerticalLine (frame, i, style, xd, yd, width, height, 2, fg);
	  break;
	case '/':
	  DrawSlash (frame, i, style, xd, yd, width, height, 0, fg);
	  break;
	case '\\':
	  DrawSlash (frame, i, style, xd, yd, width, height, 1, fg);
	  break;
	case '>':
	  DrawArrow (frame, i, style, xd, yd, width, height, 0, fg);
	  break;
	case 'E':
	  DrawArrow (frame, i, style, xd, yd, width, height, 45, fg);
	  break;
	case '^':
	  DrawArrow (frame, i, style, xd, yd, width, height, 90, fg);
	  break;
	case 'O':
	  DrawArrow (frame, i, style, xd, yd, width, height, 135, fg);
	  break;
	case '<':
	  DrawArrow (frame, i, style, xd, yd, width, height, 180, fg);
	  break;
	case 'o':
	  DrawArrow (frame, i, style, xd, yd, width, height, 225, fg);
	  break;
	case 'V':
	  DrawArrow (frame, i, style, xd, yd, width, height, 270, fg);
	  break;
	case 'e':
	  DrawArrow (frame, i, style, xd, yd, width, height, 315, fg);
	  break;
	    
	case 'P':
	  DrawRectangleFrame (frame, i, style, xd, yd, width,
			      height, fg, bg, pat);
	  break;
	case 'Q':
	  DrawEllipsFrame (frame, i, style, xd, yd, width,
			   height, fg, bg, pat);
	  break;
	case 'W':
	  DrawCorner (frame, i, style, xd, yd, width, height, 0, fg);
	  break;
	case 'X':
	  DrawCorner (frame, i, style, xd, yd, width, height, 1, fg);
	  break;
	case 'Y':
	  DrawCorner (frame, i, style, xd, yd, width, height, 2, fg);
	  break;
	case 'Z':
	  DrawCorner (frame, i, style, xd, yd, width, height, 3, fg);
	  break;
	    
	default:
	  break;
	}
	
      if (pBox->BxEndOfBloc > 0)
	{
	  /* fill the end of the line with dots */
	  yd = pBox->BxYOrg + pBox->BxHorizRef - pFrame->FrYOrg;
	  DrawPoints (frame, xd + width, yd, pBox->BxEndOfBloc, fg);
	}

      /* show the selection on the whole image */
      if (selected)
	{
	  if (pFrame->FrSelectOnePosition)
	    DisplayPointSelection (frame, pBox,
				   pFrame->FrSelectionBegin.VsIndBox);
	  else
	    DisplayPointSelection (frame, pBox, 0);
	}
    }
}
#else /* _GL */
/*----------------------------------------------------------------------
  DisplayGraph display a graphic.
  The parameter selected is TRUE when the graphic is selected.
  ----------------------------------------------------------------------*/
void  DisplayGraph (PtrBox pBox, int frame, ThotBool selected)
{
  ViewFrame          *pFrame;
  PtrAbstractBox      pAb;
  int                 i;  
  int                 xd,yd; 
  int                 fg, bg;
  int                 pat;
  int                 style;
  int                 width, height;

  pAb = pBox->BxAbstractBox;
  pFrame = &ViewFrameTable[frame - 1];
  if (pAb->AbVisibility >= pFrame->FrVisibility)
    {
      bg = pAb->AbBackground;
      pat = pAb->AbFillPattern;
      fg = pAb->AbForeground;
      xd = pBox->BxXOrg + pBox->BxLMargin + pBox->BxLBorder +
	pBox->BxLPadding - pFrame->FrXOrg;
      yd = pBox->BxYOrg + pBox->BxTMargin + pBox->BxTBorder +
	pBox->BxTPadding - pFrame->FrYOrg;
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

      i = GetLineWeight (pAb, frame);
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
	  DrawRectangle (frame, 0, 0, xd, yd, width, height, fg,
			 bg, 2);
	  break;
	case '\261':
	  DrawRectangle (frame, 0, 0, xd, yd, width, height, fg,
			 bg, 5);
	  break;
	case '\262':
	  DrawRectangle (frame, 0, 0, xd, yd, width, height, fg,
			 bg, 6);
	  break;
	case '\263':
	  DrawRectangle (frame, 0, 0, xd, yd, width, height, fg,
			 bg, 7);
	  break;
	case '\264':
	  DrawRectangle (frame, 0, 0, xd, yd, width, height, fg,
			 bg, 8);
	  break;
	case '\265':
	  DrawRectangle (frame, 0, 0, xd, yd, width, height, fg,
			 bg, 9);
	  break;
	case '\266':
	  DrawRectangle (frame, 0, 0, xd, yd, width, height, fg,
			 bg, 1);
	  break;
	case '\267':
	  DrawRectangle (frame, 0, 0, xd, yd, width, height, fg,
			 bg, 0);
	  break;
	case '\270':
	  DrawRectangle (frame, 0, 0, xd, yd, width, height, fg,
			 bg, 4);
	  break;
	case '0':
	  DrawRectangle (frame, 0, 0, xd, yd, width, height, fg,
			 bg, pat);
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
	  DrawRectangle (frame, i, style, xd, yd, width, height,
			 fg, bg, pat);
	  break;
	case 'g':
	  /* Coords of the line are given by the enclosing box */
	  pAb = pAb->AbEnclosing;
	  if ((pAb->AbHorizPos.PosEdge == Left &&
	       pAb->AbVertPos.PosEdge == Top) ||
	      (pAb->AbHorizPos.PosEdge == Right &&
	       pAb->AbVertPos.PosEdge == Bottom))
	    /* draw a \ */
	    DrawSlash (frame, i, style, xd, yd, width, height, 1, fg);
	  else
	    /* draw a / */
	    DrawSlash (frame, i, style, xd, yd, width, height, 0, fg);
	  break;
	case 'C':
	  if (pBox->BxRx == 0 && pBox->BxRy == 0)
	    DrawRectangle (frame, i, style, xd, yd, width, height, fg, bg, pat);
	  else
	    DrawOval (frame, i, style, xd, yd, width, height, pBox->BxRx,
		      pBox->BxRy, fg, bg, pat);
	  break;
	case 'L':
	  DrawDiamond (frame, i, style, xd, yd, width, height, fg, bg, pat);
	  break;
	case 'a':
	case 'c':
	  DrawEllips (frame, i, style, xd, yd, width, height, fg, bg, pat);
	  break;
	case 'h':
	  DrawHorizontalLine (frame, i, style, xd, yd, width, height, 1, fg);
	  break;
	case 't':
	  DrawHorizontalLine (frame, i, style, xd, yd, width, height, 0, fg);
	  break;
	case 'b':
	  DrawHorizontalLine (frame, i, style, xd, yd, width, height, 2, fg);
	  break;
	case 'v':
	  DrawVerticalLine (frame, i, style, xd, yd, width, height, 1, fg);
	  break;
	case 'l':
	  DrawVerticalLine (frame, i, style, xd, yd, width, height, 0, fg);
	  break;
	case 'r':
	  DrawVerticalLine (frame, i, style, xd, yd, width, height, 2, fg);
	  break;
	case '/':
	  DrawSlash (frame, i, style, xd, yd, width, height, 0, fg);
	  break;
	case '\\':
	  DrawSlash (frame, i, style, xd, yd, width, height, 1, fg);
	  break;
	case '>':
	  DrawArrow (frame, i, style, xd, yd, width, height, 0, fg);
	  break;
	case 'E':
	  DrawArrow (frame, i, style, xd, yd, width, height, 45, fg);
	  break;
	case '^':
	  DrawArrow (frame, i, style, xd, yd, width, height, 90, fg);
	  break;
	case 'O':
	  DrawArrow (frame, i, style, xd, yd, width, height, 135, fg);
	  break;
	case '<':
	  DrawArrow (frame, i, style, xd, yd, width, height, 180, fg);
	  break;
	case 'o':
	  DrawArrow (frame, i, style, xd, yd, width, height, 225, fg);
	  break;
	case 'V':
	  DrawArrow (frame, i, style, xd, yd, width, height, 270, fg);
	  break;
	case 'e':
	  DrawArrow (frame, i, style, xd, yd, width, height, 315, fg);
	  break;
	    
	case 'P':
	  DrawRectangleFrame (frame, i, style, xd, yd, width,
			      height, fg, bg, pat);
	  break;
	case 'Q':
	  DrawEllipsFrame (frame, i, style, xd, yd, width,
			   height, fg, bg, pat);
	  break;
	case 'W':
	  DrawCorner (frame, i, style, xd, yd, width, height, 0, fg);
	  break;
	case 'X':
	  DrawCorner (frame, i, style, xd, yd, width, height, 1, fg);
	  break;
	case 'Y':
	  DrawCorner (frame, i, style, xd, yd, width, height, 2, fg);
	  break;
	case 'Z':
	  DrawCorner (frame, i, style, xd, yd, width, height, 3, fg);
	  break;
	    
	default:
	  break;
	}
	
      if (pBox->BxEndOfBloc > 0)
	{
	  /* fill the end of the line with dots */
	  yd = pBox->BxYOrg + pBox->BxHorizRef - pFrame->FrYOrg;
	  DrawPoints (frame, xd + width, yd, pBox->BxEndOfBloc, fg);
	}

      /* show the selection on the whole image */
      if (selected)
	{
	  if (pFrame->FrSelectOnePosition)
	    DisplayPointSelection (frame, pBox,
				   pFrame->FrSelectionBegin.VsIndBox);
	  else
	    DisplayPointSelection (frame, pBox, 0);
	}
    }
}
#endif /* _GL */

/*----------------------------------------------------------------------
  PolyTransform checks whether a polyline Box need to be transformed
  to fit in the current area.
  If the CHKR_LIMIT point matches the current geometry, no need
  to change anything, otherwise, all points are moved using
  Box-Width/Lim-X ratio horizontally and Box-Height/Lim-Y ratio vertically.
  ----------------------------------------------------------------------*/
static void PolyTransform (PtrBox pBox, int frame)
{
  float               xRatio, yRatio, pointIndex;
  PtrTextBuffer       adbuff;
  int                 i;
  int                 j, val;
  int                 width, height;
  int                 zoom;

  /* box sizes have to be positive */
  width = pBox->BxW;
  if (width < 0)
    width = 0;
  height = pBox->BxH;
  if (height < 0)
    height = 0;
  zoom = ViewFrameTable[frame - 1].FrMagnification;
  val = PixelValue (pBox->BxBuffer->BuPoints[0].XCoord, UnPixel, NULL, zoom);
  /* Compute ratio for axis X */
  if (val != width && pBox->BxBuffer->BuPoints[0].XCoord > 0)
    {
      val = LogicalValue (width, UnPixel, NULL, zoom);
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
  val = PixelValue (pBox->BxBuffer->BuPoints[0].YCoord, UnPixel, NULL, zoom);
  if (val != height && pBox->BxBuffer->BuPoints[0].YCoord > 0)
    {
      val = LogicalValue (height, UnPixel, NULL, zoom);
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
  DisplayPolyLine displays a polyline.
  The parameter selected is TRUE when the polyline is selected.
  ----------------------------------------------------------------------*/
void DisplayPolyLine (PtrBox pBox, int frame, ThotBool selected)
{
  PtrAbstractBox      pAb;
  ViewFrame          *pFrame;
  int                 i, xd, yd;
  int                 fg, bg;
  int                 pat;
  int                 style, arrow;
  int                 width;

  /* If no point is defined, no need to draw it */
  if (pBox->BxBuffer == NULL || pBox->BxNChars <= 1)
    return;

  /* Transform the polyline if the box size has changed */
  PolyTransform (pBox, frame);
  pAb = pBox->BxAbstractBox;
  pFrame = &ViewFrameTable[frame - 1];
  if (pAb->AbVisibility >= pFrame->FrVisibility)
    {
      bg = pAb->AbBackground;
      pat = pAb->AbFillPattern;
      fg = pAb->AbForeground;
      xd = pBox->BxXOrg + pBox->BxLMargin + pBox->BxLBorder +
	pBox->BxLPadding - pFrame->FrXOrg;
      yd = pBox->BxYOrg + pBox->BxTMargin + pBox->BxTBorder +
	pBox->BxTPadding - pFrame->FrYOrg;
      
      /* box sizes have to be positive */
      width = pBox->BxW;
      if (width < 0)
	width = 0;
      
      /* Style and thickness of the line */
      i = GetLineWeight (pAb, frame);
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
	  DrawSegments (frame, i, style, xd, yd, pBox->BxBuffer,
			pBox->BxNChars, fg, arrow, bg, pat);
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
	    pBox->BxPictInfo = (int *) ComputeControlPoints (pBox->BxBuffer,
							     pBox->BxNChars, ViewFrameTable[frame - 1].FrMagnification);
	  DrawCurve (frame, i, style, xd, yd, pBox->BxBuffer,
		     pBox->BxNChars, fg, arrow,
		     (C_points *) pBox->BxPictInfo);
	  break;
	case 'p':	/* polygon */
	  DrawPolygon (frame, i, style, xd, yd, pBox->BxBuffer,
		       pBox->BxNChars, fg, bg, pat);
	  break;
	case 's':	/* closed spline */
	  /* compute control points */
	  if (pBox->BxPictInfo == NULL)
	    pBox->BxPictInfo = (int *) ComputeControlPoints (pBox->BxBuffer, 
							     pBox->BxNChars, ViewFrameTable[frame - 1].FrMagnification);
	  DrawSpline (frame, i, style, xd, yd, pBox->BxBuffer,
		      pBox->BxNChars, fg, bg, pat,
		      (C_points *) pBox->BxPictInfo);
	  break;
	default:
	  break;
	}
      
      if (pBox->BxEndOfBloc > 0)
	{
	  /* fill the end of the line with dots */
	  yd = pBox->BxYOrg + pBox->BxHorizRef - pFrame->FrYOrg;
	  DrawPoints (frame, xd + width, yd, pBox->BxEndOfBloc, fg);
	}
      
      /* show the selection on the whole image */
      if (selected)
	{
	  if (pFrame->FrSelectOnePosition)
	    DisplayPointSelection (frame, pBox,
				   pFrame->FrSelectionBegin.VsIndBox);
	  else if (pBox->BxNChars > 1)
	    DisplayPointSelection (frame, pBox, 0);
	}
    }
}

/*----------------------------------------------------------------------
  DisplayPath displays a path.
  The parameter selected is TRUE when the polyline is selected.
  ----------------------------------------------------------------------*/
void DisplayPath (PtrBox pBox, int frame, ThotBool selected)
{
  PtrAbstractBox      pAb;
  ViewFrame          *pFrame;
  int                 i, xd, yd;
  int                 fg, bg;
  int                 pat;
  int                 style;
  int                 width;

  /* If the path does not contain any segment, return */
  if (!pBox->BxFirstPathSeg)
    return;

  pAb = pBox->BxAbstractBox;
  pFrame = &ViewFrameTable[frame - 1];
  if (pAb->AbVisibility >= pFrame->FrVisibility)
    {
      bg = pAb->AbBackground;
      pat = pAb->AbFillPattern;
      fg = pAb->AbForeground;
      xd = pBox->BxXOrg + pBox->BxLMargin + pBox->BxLBorder +
	pBox->BxLPadding - pFrame->FrXOrg;
      yd = pBox->BxYOrg + pBox->BxTMargin + pBox->BxTBorder +
	pBox->BxTPadding - pFrame->FrYOrg;
      
      /* Style and thickness of the line */
      i = GetLineWeight (pAb, frame);
      switch (pAb->AbLineStyle)
	{
	case 'S':
	  style = 5; /* solid */
	  break;
	case '-':
	  style = 4; /* dashed */
	  break;
	case '.':    /* dotted */
	  style = 3;
	  break;
	default:
	  style = 5; /* solid */
	}

      DrawPath (frame, i, style, xd, yd, pBox->BxFirstPathSeg, fg,
		bg, pat);

      if (pBox->BxEndOfBloc > 0)
	{
          /* box sizes have to be positive */
	  width = pBox->BxW;
	  if (width < 0)
	    width = 0;
	  /* fill the end of the line with dots */
	  yd = pBox->BxYOrg + pBox->BxHorizRef - pFrame->FrYOrg;
	  DrawPoints (frame, xd + width, yd, pBox->BxEndOfBloc, fg);
	}
      
      /* show the selection on the whole image */
      if (selected)
	{
	  if (pFrame->FrSelectOnePosition)
	    DisplayPointSelection (frame, pBox,
				   pFrame->FrSelectionBegin.VsIndBox);
	  else if (pBox->BxNChars > 1)
	    DisplayPointSelection (frame, pBox, 0);
	}
    }
}

/*----------------------------------------------------------------------
  LocateFirstChar returns the buffer and the index that locates the
  first character of the box according to the writing orientation
  (left-to-right or right-to-left).
  ----------------------------------------------------------------------*/
void LocateFirstChar (PtrBox pBox, ThotBool rtl, PtrTextBuffer *adbuff, int *ind)
{
  int                 buffleft;
  int                 nbcar;
  
  *ind = pBox->BxIndChar;
  *adbuff = pBox->BxBuffer;
  if (rtl && *adbuff)
    {
      /* writing right-to-left */
      nbcar = pBox->BxNChars;
      buffleft = (*adbuff)->BuLength - *ind;
      while ((*adbuff)->BuNext && nbcar > buffleft)
	{
	  nbcar -= buffleft;
	  *adbuff = (*adbuff)->BuNext;
	  buffleft = (*adbuff)->BuLength;
	  *ind = 0;
	}
      if (nbcar <= (*adbuff)->BuLength)
	*ind = *ind + nbcar - 1;
      else
	*ind = (*adbuff)->BuLength - 1;
    }
  else
    while (*adbuff && ((*adbuff)->BuLength == 0) && (*adbuff)->BuNext)
      {
	*adbuff = (*adbuff)->BuNext;
	*ind = 0;
      }
}

/*----------------------------------------------------------------------
  LocateNextChar returns the buffer and the index of the next character
  according to the writing orientation (left-to-right or right-to-left).
  Return TRUE if a new position is found.
  ----------------------------------------------------------------------*/
ThotBool LocateNextChar (PtrTextBuffer *adbuff, int *ind, ThotBool rtl)
{
  if (rtl)
    {
      /* writing right-to-left */
      (*ind)--;
      while (*ind < 0)
	{
	  /* another buffer */
	  if ((*adbuff)->BuPrevious == NULL)
	    return FALSE;
	  *adbuff = (*adbuff)->BuPrevious;
	  *ind = (*adbuff)->BuLength - 1;
	}
    }
  else
    {
      (*ind)++;
      while (*ind >= (*adbuff)->BuLength)
	{
	  /* another buffer */
	  if ((*adbuff)->BuNext == NULL)
	    return FALSE;
	  *adbuff = (*adbuff)->BuNext;
	  *ind = 0;
	}
    }
  return TRUE;
}
/*----------------------------------------------------------------------
  DisplayJustifiedText display the content of a Text box tweaking
  the space sizes to ajust line length to the size of the frame.
  Remaining pixel space (BxNPixels) is equally dispatched 
  on all spaces in the line.
  ----------------------------------------------------------------------*/
static void DisplayJustifiedText (PtrBox pBox, PtrBox mbox, int frame,
				  ThotBool selected)
{
  PtrTextBuffer       adbuff;
  ViewFrame          *pFrame;
  PtrBox              nbox;
  PtrAbstractBox      pAb;
  SpecFont            font;
  PtrFont             prevfont = NULL;
  PtrFont             nextfont = NULL;
  CHAR_T              c;
  wchar_t            *wbuffer = NULL;
  unsigned char      *buffer = NULL;
  char                script;
  int                 restbl, val;
  int                 newbl, lg;
  int                 charleft;
  int                 buffleft;
  int                 indbuff, bl;
  int                 indmax;
  int                 nbcar, x, y, y1;
  int                 lgspace, whitespace;
  int                 fg, bg;
  int                 shadow;
  int                 width, org;
  int                 left, right;
  ThotBool            blockbegin;
  ThotBool            withbackground;
  ThotBool            hyphen, rtl;
  CHAR_T              prevChar, nextChar;

#ifdef _GL
  SetTextureScale (IsBoxDeformed(pBox));
#endif /* _GL */
  indmax = 0;
  buffleft = 0;
  adbuff = NULL;
  indbuff = 0;
  restbl = 0;
  pAb = pBox->BxAbstractBox;
  script = pBox->BxScript;
  /* is it a box with a right-to-left writing? */
  if (pAb->AbUnicodeBidi == 'O')
    rtl = (pAb->AbDirection == 'R');
  else
    rtl = (script == 'A' || script == 'H');
  font = pBox->BxFont;
  /* do we have to display stars instead of characters? */
  if (pAb->AbBox->BxShadow)
    shadow = 1;
  else
    shadow = 0;
  
  /* Is this box the first of a block of text? */
  if (mbox == pBox)
    blockbegin = TRUE;
  else if (mbox->BxFirstLine == NULL ||
	   (mbox->BxType != BoBlock && mbox->BxType != BoFloatBlock))
    blockbegin = TRUE;
  else if (pBox->BxType == BoComplete && mbox->BxFirstLine->LiFirstBox == pBox)
    blockbegin = TRUE;
  else if ((pBox->BxType == BoPiece ||
	    pBox->BxType == BoScript ||
	    pBox->BxType == BoDotted) &&
	   mbox->BxFirstLine->LiFirstPiece == pBox)
    blockbegin = TRUE;
  else
    blockbegin = FALSE;
  
  /* Is an hyphenation mark needed at the end of the box? */
  if (pBox->BxType == BoDotted)
    hyphen = TRUE;
  else
    hyphen = FALSE;
  /* in SVG foreground and background are inverted in the main view */
  if (!strcmp(pAb->AbElement->ElStructSchema->SsName, "SVG") &&
      FrameTable[frame].FrView == 1)
    {
    bg = pAb->AbForeground;
    fg = pAb->AbBackground;
    withbackground = FALSE;
    }
  else
    {
    fg = pAb->AbForeground;
    bg = pAb->AbBackground;
    withbackground = (pAb->AbBox->BxFill && pAb->AbBox->BxDisplay);
    }
  pFrame = &ViewFrameTable[frame - 1];
  left = 0;
  right = 0;
  if (pAb->AbVisibility >= pFrame->FrVisibility)
    {
      /* Initialization */
      x = pBox->BxXOrg + pBox->BxLMargin + pBox->BxLBorder +
	  pBox->BxLPadding - pFrame->FrXOrg;
      y = pBox->BxYOrg + pBox->BxTMargin + pBox->BxTBorder +
	  pBox->BxTPadding - pFrame->FrYOrg;
      y1 = pBox->BxYOrg + pBox->BxHorizRef - pFrame->FrYOrg;
      /* no previous spaces */
      bl = 0;
      charleft = pBox->BxNChars;
      newbl = pBox->BxNPixels;
      lg = 0;
	   
      /* box sizes have to be positive */
      width = pBox->BxW;
      if (width < 0)
	width = 0;
      whitespace = BoxCharacterWidth (SPACE, font);
      lgspace = pBox->BxSpaceWidth;
      if (lgspace == 0)
	lgspace = whitespace;
      
      /* locate the first character */
      LocateFirstChar (pBox, rtl, &adbuff, &indbuff);
      /* Search the first displayable char */
      if (charleft > 0 && adbuff)
	{
	  /* there is almost one character to display */
	  do
	    {
	      /* skip invisible characters */
	      restbl = newbl;
	      x += lg;
	      c = adbuff->BuContent[indbuff];
	      if (c >= 0x0600 && c <= 0x06B0 ) /*arabic character */
		{
		  /* index of the character in arabic font */
		  nextChar = Previous_Char (&adbuff, &indbuff);
		  prevChar = Next_Char (&adbuff, &indbuff);
	       	  if (( nextChar >= 0x064B )&&( nextChar <= 0x0655 ))
		    {
		      if ( indbuff < (adbuff->BuLength - 2) )
			nextChar = adbuff->BuContent[indbuff + 2];
		      else 
			nextChar = 0x0020;
		    }
		  if ((prevChar >= 0x064B )&&( prevChar <= 0x0655 ))
		    {
		      if ( indbuff > 1 )
			prevChar = adbuff->BuContent[indbuff -2];
		      else
			prevChar = 0x0020;
		    }
       		  val = GetArabFontAndIndex (c, prevChar, nextChar, font, &nextfont);
		}
	      else
		val = GetFontAndIndexFromSpec (c, font, &nextfont);
	      if (val == SPACE)
		{
		  lg = lgspace;
		  if (newbl > 0)
		    {
		      newbl--;
		      lg++;
		    } 
		}
	      else
		lg = CharacterWidth (val, nextfont);

	      /* Skip to the next char */
	      if (x + lg <= 0)
		{
		  if (LocateNextChar (&adbuff, &indbuff, rtl))
		    charleft--;
		  else
		    charleft = 0;
		}
	    }
	  while (x + lg <= 0 && charleft > 0);
	   
	  /* Display the list of text buffers pointed by adbuff */
	  /* beginning at indbuff and of lenght charleft.       */
	  /* -------------------------------------------------- */
	  if (adbuff == NULL)
	    charleft = 0;
	  else
	    {
	      /* number of characters to be displayed in the current buffer */
	      if (rtl)
		{
		  buffleft = indbuff + 1;
		  if (charleft < buffleft)
		    {
		      indmax = indbuff - charleft + 1;
		      buffleft = charleft;		      
		    }
		  else
		    indmax = 0;
		}
	      else
		{
		  buffleft = adbuff->BuLength - indbuff;
		  if (charleft < buffleft)
		    {
		      indmax = indbuff + charleft - 1;
		      buffleft = charleft;
		    }
		  else
		    indmax = adbuff->BuLength - 1;
		}
	    } 
	  
	  /* Do we need to draw a background */
	  if (withbackground)
	    DrawRectangle (frame, 0, 0,
			   x - pBox->BxLPadding, y - pBox->BxTPadding,
			   width + pBox->BxLPadding + pBox->BxRPadding,
			   pBox->BxHeight /*BoxFontHeight (font)*/ + pBox->BxTPadding + pBox->BxBPadding,
			   0, bg, 2);
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
		    right = pBox->BxW;
		  DisplayStringSelection (frame, left, right, pBox);
		  /* the selection is done now */
		  left = 0;
		  right = 0;
		}
	    }
	  else if (pBox->BxType == BoPiece ||
		   pBox->BxType == BoScript ||
		   pBox->BxType == BoDotted)
	    {
	      /* check if the box in within the selection */
	      if (pFrame->FrSelectionBegin.VsBox &&
		  pAb == pFrame->FrSelectionBegin.VsBox->BxAbstractBox)
		{
		  nbox = pFrame->FrSelectionBegin.VsBox;
		  while (nbox && nbox != pFrame->FrSelectionEnd.VsBox &&
			 nbox != pBox)
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
	      else if (!pFrame->FrSelectOnePosition)
		DisplayBgBoxSelection (frame, pBox);
	    }
	  else if (!pFrame->FrSelectOnePosition)
	    DisplayBgBoxSelection (frame, pBox);
	}

      /* allocate a buffer to store converted characters */
#ifndef _GL
      if ((script == 'Z')||(script == 'A' ))
	wbuffer = TtaGetMemory ((pBox->BxNChars + 1) * sizeof(wchar_t));
      else
	buffer = TtaGetMemory (pBox->BxNChars + 1);
#else /*_GL*/
      wbuffer = TtaGetMemory ((pBox->BxNChars + 1) * sizeof(wchar_t));
#endif /*_GL*/
      nbcar = 0;
      org = x;
      while (charleft > 0)
	{
	  /* handle each char in the buffer */
	  while ((rtl && indbuff >= indmax) ||
		 (!rtl && indbuff <= indmax))
	    {
	      c = adbuff->BuContent[indbuff];
	      if ( c >= 0x0600 && c <= 0x06B0 ) /* arabic char */
		{
		  nextChar = Previous_Char (&adbuff, &indbuff);
		  prevChar = Next_Char (&adbuff, &indbuff);
	       	  if (( nextChar >= 0x064B )&&( nextChar <= 0x0655 ))
		    {
		      if ( indbuff < (adbuff->BuLength - 2) )
			nextChar = adbuff->BuContent[indbuff + 2];
		      else 
			nextChar = 0x0020;
		    }
		  if ((prevChar >= 0x064B )&&( prevChar <= 0x0655 ))
		    {
		      if ( indbuff > 1 )
			prevChar = adbuff->BuContent[indbuff -2];
		      else
			prevChar = 0x0020;
		    }  
		  val = GetArabFontAndIndex (c ,prevChar, nextChar, font, &nextfont);
		}
	      else
		val = GetFontAndIndexFromSpec (c, font, &nextfont);
	      if (val == INVISIBLE_CHAR || c == ZERO_SPACE ||
		  c == EOL || c == BREAK_LINE)
		/* do nothing */;
	      else if (nextfont == NULL && val == UNDISPLAYED_UNICODE)
		{
		  /* display previous chars handled */
		  if (nbcar > 0)
		    {
		      width = width + org;
		      org -= x;
		      if (org == 0)
			org = -1;
#ifndef _GL
		      if ((script == 'Z')||(script == 'A'))
			x += WDrawString (wbuffer, nbcar, frame, x, y1, prevfont,
					 org, bl, x, blockbegin, fg, shadow);
		      else
			x += DrawString (buffer, nbcar, frame, x, y1, prevfont,
					 org, bl, x, blockbegin, fg, shadow);
#else /*_GL*/
		      x += WDrawString (wbuffer, nbcar, frame, x, y1, prevfont,
					org, bl, x, blockbegin, fg, shadow);
#endif /*_GL*/
		      width = width - x;
		    }
		  nbcar = 0;
		  /* all previous spaces are declared */
		  bl = 0;
		  prevfont = nextfont;
		  DrawRectangle (frame, 1, 5, x, y, 6, pBox->BxH - 1, fg, 0, 0);
		  x += 6;
		  org = x;
		}
	      else
		{
		  if (prevfont != nextfont)
		    {
		      /* display previous chars handled */
#ifdef _WINDOWS
		      if (nbcar > 0)
#else /* WINDOWS */
		      if (nbcar > 0 || Printing)
#endif /* WINDOWS */
			{
			  width = width + org;
			  org -= x;
			  if (org == 0)
			    org = -1;			  
#ifndef _GL
			  if ((script == 'Z')||(script == 'A' ))
			    x += WDrawString (wbuffer, nbcar, frame, x, y1,
					      prevfont, org, bl, 0, blockbegin,
					      fg, shadow);
			  else
			    {
			      if ( prevfont == NULL)
				prevfont = nextfont;
			      x += DrawString (buffer, nbcar, frame, x, y1,
					       prevfont, org, bl, 0, blockbegin,
					       fg, shadow);
			    }
#else /*_GL*/
			  x += WDrawString (wbuffer, nbcar, frame, x, y1,
					    prevfont, org, bl, 0, blockbegin,
					    fg, shadow);
#endif /*_GL*/
			  width = width - x;
			  org = x;
			  /* all previous spaces are declared */
			  bl = 0;
			}
		      nbcar = 0;
		      prevfont = nextfont;
		    }
		  if (c == SPACE || c == TAB ||
		      c == NEW_LINE || c == UNBREAKABLE_SPACE ||
		      c == EN_QUAD || c == EM_QUAD ||
		      c == EN_SPACE || c == EM_SPACE ||
		      c == THICK_SPACE || c == FOUR_PER_EM ||
		      c == SIX_PER_EM || c == FIG_SPACE ||
		      c == PUNC_SPACE || c == THIN_SPACE ||
		      c == HAIR_SPACE || c == MEDIUM_SPACE)
		    {
		      /* display previous chars handled */
		      if (nbcar > 0)
			{
#ifndef _GL
			  if ((script == 'Z')||(script == 'A'))
			    x += WDrawString (wbuffer, nbcar, frame, x, y1, prevfont,  
					      0, bl, 0, blockbegin, fg, shadow);
			  else
			    x += DrawString (buffer, nbcar, frame, x, y1, prevfont,  
					     0, bl, 0, blockbegin, fg, shadow);
#else /*_GL*/
			  x += WDrawString (wbuffer, nbcar, frame, x, y1, prevfont,  
					      0, bl, 0, blockbegin, fg, shadow);

#endif /*_GL*/
			  /* all previous spaces are declared */
			  bl = 0;
			}
		  
		      if (shadow)
			DrawChar ('*', frame, x, y, nextfont, fg);
		      else if (!ShowSpace)
			{
			  /* Show the space chars */
			  if (c == SPACE || c == TAB) 
			    DrawChar ((char) SHOWN_SPACE, frame, x, y, nextfont, fg);
			  else if (c == THIN_SPACE)
			    DrawChar ((char) SHOWN_THIN_SPACE, frame, x, y, nextfont, fg);
			  else if (c == FOUR_PER_EM)
			    DrawChar ((char) SHOWN_HALF_EM, frame, x, y, nextfont, fg);
			  else if (c == UNBREAKABLE_SPACE)
			    DrawChar ((char) SHOWN_UNBREAKABLE_SPACE, frame, x, y,
				      nextfont, fg);
			}
		 
		      nbcar = 0;
		      if (c == SPACE)
			{
			  if (restbl > 0)
			    {
			      /* Pixel space splitting */
			      lg = lgspace + 1;
			      restbl--;
			    }
			  else
			    lg = lgspace;
			}
		      else if (c != EOS)
			lg = CharacterWidth (c, nextfont);
#ifdef _WINDOWS
			x += lg;
#else /* _WINDOWS */
		      if (Printing)
			org -= lg;
		      else
			x += lg;
#endif /* _WINDOWS */
		      /* a new space is handled */
		      bl++;
		    }
#ifndef _GL
		  else if ((script == 'Z')||(script == 'A'))
		    /* add the new char */
		    wbuffer[nbcar++] = val;
		  else
		    /* add the new char */
		    buffer[nbcar++] = val;
#else /*_GL*/
		  else 
		    /* add the new char */
		    wbuffer[nbcar++] = val;
#endif /*_GL*/
		}
	      /* Skip to next char */
	      if (rtl)
		indbuff--;
	      else
		indbuff++;
	    }

	  /* Draw previous chars in the buffer */
	  charleft -= buffleft;
	  if (charleft > 0)
	    {
	      /* number of characters to be displayed in the next buffer */
	      if (rtl)
		{
		  if (adbuff->BuPrevious == NULL)
		    charleft = 0;
		  else
		    {
		      adbuff = adbuff->BuPrevious;
		      indbuff = adbuff->BuLength - 1;
		      buffleft = adbuff->BuLength;
		      if (charleft < buffleft)
			{
			  indmax = indbuff - charleft + 1;
			  buffleft = charleft;		      
			}
		      else
			indmax = 0;
		    }
		}
	      else
		{
		  if (adbuff->BuNext == NULL)
		    charleft = 0;
		  else
		    {
		      adbuff = adbuff->BuNext;
		      indbuff = 0;
		      buffleft = adbuff->BuLength;
		      if (charleft < buffleft)
			{
			  indmax = charleft - 1;
			  buffleft = charleft;		      
			}
		      else
			indmax = adbuff->BuLength - 1;
		    }
		}
	    }

	  if (charleft <= 0)
	    {
	      /*
		Draw the content of the buffer.
		Call the function in any case to let Postscript justify the
		text of the box.
	      */
#ifndef _GL
	      if ((script == 'Z')||(script == 'A'))
		x += WDrawString (wbuffer, nbcar, frame, x, y1, prevfont, width,
				 bl, hyphen, blockbegin, fg, shadow);
	      else
	     
		x += DrawString (buffer, nbcar, frame, x, y1, prevfont, width,
				 bl, hyphen, blockbegin, fg, shadow);
#else /*_GL*/
	      x += WDrawString (wbuffer, nbcar, frame, x, y1, prevfont, width,
				 bl, hyphen, blockbegin, fg, shadow);
#endif /*_GL*/
	      if (pBox->BxUnderline != 0)
		{
#ifdef _GL
		  SetTextureScale (IsBoxDeformed(pBox));
#endif /* _GL */
		  DisplayUnderline (frame, x, y, nextfont,
				    pBox->BxUnderline, width, fg);

#ifdef _GL
		  StopTextureScale ();
#endif /* _GL */
		}
	      nbcar = 0;
	    }
	} 
#ifdef _GL
      StopTextureScale ();
#endif /* _GL */

      /* Should the end of the line be filled with dots */
      if (pBox->BxEndOfBloc > 0)
	{
	  /* fill the end of the line with dots */
	  x = pBox->BxXOrg + pBox->BxLMargin + pBox->BxLBorder +
	    pBox->BxLPadding;
	  y = pBox->BxYOrg + pBox->BxHorizRef - pFrame->FrYOrg;
	  DrawPoints (frame, pBox->BxXOrg + width - pFrame->FrXOrg, y,
		      pBox->BxEndOfBloc, fg);
	}
      /* display a caret if needed */
      if (left != right)
	DisplayStringSelection (frame, left, right, pBox);
      TtaFreeMemory (wbuffer);
      TtaFreeMemory (buffer);
    }
}




/*----------------------------------------------------------------------
  DisplayBorders displays the box borders.
  Parameters x, y, w, h give the clipping region.
  ----------------------------------------------------------------------*/
void DisplayBorders (PtrBox box, int frame, int x, int y, int w, int h) 
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
			      0, color);
	  /* the width of the bottom line depends on the visibility of
	     vertical borders */
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
				2, color);
	  break;
	default:
	  DrawHorizontalLine (frame, t, pAb->AbTopStyle,
			      x, y,
			      w, t,
			      0, color);
	  break;
	}
    }
  if (box->BxLBorder && pAb->AbLeftStyle > 2 &&
      pAb->AbLeftBColor != -2 && l > 0)
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
			    0, color);
	  /* the width of the right line depends on the visibility of
	     horizontal borders */
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
			    2, color);
	  break;
	default:
	  DrawVerticalLine (frame, l, pAb->AbLeftStyle,
			    x, y,
			    l, h,
			    0, color);
	  break;
	}
    }
  if (box->BxBBorder && pAb->AbBottomStyle > 2 &&
      pAb->AbBottomBColor != -2 && b > 0)
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
	  /* the width of the bottom line depends on the visibility of
	     vertical borders */
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
			      pos, yFrame + box->BxHeight - box->BxBMargin -
			                                    box->BxBBorder,
			      dim, 1,
			      0, color);
	  /* bottom line */
	  if (b < h)
	  DrawHorizontalLine (frame, 1, 5,
			      x, yFrame + box->BxHeight - box->BxBMargin,
			      w, 1,
			      2, color);
	  break;
	default:
	  DrawHorizontalLine (frame, b, pAb->AbBottomStyle,
			      x, yFrame + box->BxHeight - box->BxBMargin -
			                                  box->BxBBorder,
			      w, b,
			      2, color);
	  break;
	}
    }
  if (box->BxRBorder && pAb->AbRightStyle > 2 && pAb->AbRightBColor != -2 &&
      r > 0)
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
	  /* the width of the left line depends on the visibility of
	     horizontal borders */
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
			    xFrame + box->BxWidth - box->BxRMargin -
			                            box->BxRBorder, pos,
			    1, dim,
			    0, color);
	  /* rigth line */
	  DrawVerticalLine (frame, 1, 5,
			    xFrame + box->BxWidth - box->BxRMargin, y,
			    1, h,
			    2, color);
	  break;
	default:
	  DrawVerticalLine (frame, r, pAb->AbRightStyle,
			    xFrame + box->BxWidth - box->BxRMargin -
			                            box->BxRBorder, y,
			    r, h,
			    2, color);
	  break;
	}
    }
}
#ifndef _GL
/*----------------------------------------------------------------------
  DisplayViewBox :
  ----------------------------------------------------------------------*/
void DisplayViewBox (PtrTransform Trans, int Width, int Height)
{
  return;
}
/*----------------------------------------------------------------------
  DisplayTransformation :
  ----------------------------------------------------------------------*/
void DisplayTransformation (int frame, PtrTransform Trans, int Width, int Height)
{
  while (Trans)
    {
      switch (Trans->TransType)
	{
	case PtElScale:
	  break;
	case PtElTranslate:
	  break;
	case PtElRotate:
	  break;
	case PtElMatrix:
	  break;
	case PtElSkewX:
	  break;
	case PtElSkewY:
	  break;
	default:
	  break;	  
	}
      Trans = Trans->Next;
    }

}

/*----------------------------------------------------------------------
  DisplayTransformationExit :
  ----------------------------------------------------------------------*/
void DisplayTransformationExit ()
{
}

/*----------------------------------------------------------------------
  ComputeBoundingBox :
  ----------------------------------------------------------------------*/
void ComputeBoundingBox (PtrBox box, int frame, int xmin, int xmax,
			 int ymin, int ymax)
{
}

/*----------------------------------------------------------------------
  ClearOpaqueGroup clear before display a translucent Group
  ----------------------------------------------------------------------*/
void ClearOpaqueGroup (PtrAbstractBox pAb, int frame, int xmin, int xmax,
		       int ymin, int ymax)
{
}

/*----------------------------------------------------------------------
  DisplayOpaqueGroup display a translucent Group
  ----------------------------------------------------------------------*/
void DisplayOpaqueGroup (PtrAbstractBox pAb, int frame, int xmin, int xmax,
			 int ymin, int ymax, 
				   ThotBool do_display_background)
{
}

/*----------------------------------------------------------------------
  OpaqueGroupTextureFree
  ----------------------------------------------------------------------*/
void OpaqueGroupTextureFree (PtrAbstractBox pAb, int frame)
{
}

/*----------------------------------------------------------------------
  DisplayOpaqueGroup display a translucent Group
  ----------------------------------------------------------------------*/
void OpaqueGroupTexturize (PtrAbstractBox pAb, int frame, int xmin,
			   int xmax, int ymin, int ymax,
                           ThotBool Is_Pre)
{
}
#else /*_GL*/

/*----------------------------------------------------------------------
  GetBoundingBox : Get Bounding box of a group
  ----------------------------------------------------------------------*/
static void GetRelativeBoundingBox (PtrAbstractBox pAb, int *x, int *y,
				    int *width, int *height)
{
  PtrBox              box;
  int xprime, yprime, w, h;
  
  while (pAb != NULL)
    { 
      if (pAb->AbLeafType != LtCompound)
	{
	  box = pAb->AbBox;
	  xprime = box->BxXOrg + box->BxLMargin + box->BxLBorder +
	    box->BxLPadding;
	  yprime = box->BxYOrg + box->BxTMargin + box->BxTBorder +
	    box->BxTPadding;
	  if (xprime > 0)
	    {	    
	      if (*x == -1)
		*x = xprime;
	      else
		if (xprime < *x)
		  {
		    *width += *x - xprime;
		    *x = xprime;
		  }
	    }
	  if (yprime > 0)
	    {	   
	      if (*y == -1)
		*y = yprime;
	      else
		if (yprime < *y)
		  {
		    *height += *y - yprime;
		    *y = yprime;
		  }
	    }
	  w = box->BxXOrg + box->BxWidth - box->BxLMargin - box->BxRMargin;
	  h = box->BxYOrg + box->BxHeight - box->BxTMargin - box->BxBMargin;
	  if ((*x + *width) < w)
	    *width = w - *x;
	  if ((*y + *height) < h)
	    *height = h - *y;
	}      
      GetRelativeBoundingBox (pAb->AbFirstEnclosed, x, y, width, height);
      pAb = pAb->AbNext;
    }
}

/*----------------------------------------------------------------------
  LimitBoundingBoxToClip : prevent accessing out of screen memory
  ----------------------------------------------------------------------*/
static ThotBool LimitBoundingBoxToClip (int *x, int *y,
                               int *width, int *height, 
			       int Clipx, int Clipy,
			       int ClipW, int ClipH)
{
  if (*y > (Clipy+ClipH) ||
      *x > (Clipx+ClipW))
    return FALSE;  

  if ((*x + *width) < Clipx ||
      (*y + *height) < Clipy)
    return FALSE;  

  if (*x < Clipx)
    {
      *width += Clipx - *x;
      *x = Clipx; 
    }
  if (*y < Clipy)
    {
      *height += Clipy - *y;
      *y = Clipy;
    }  
  if ((*x + *width) > (Clipx+ClipW))
      *width = (Clipx+ClipW) - *x;
  if ((*y + *height) > (Clipy+ClipH))
      *height = (Clipy+ClipH) - *y;  

  if (*x >= 0 && *y >= 0 && 
      *width > 0 && *height > 0)      
      return TRUE;    
  else
    return FALSE;  
}

/*----------------------------------------------------------------------
  GetBoundingBox : Get Bounding box of a group in absolute coord
  ----------------------------------------------------------------------*/
static ThotBool GetAbsoluteBoundingBox (PtrAbstractBox pAb, 
					int *x, int *y, 
					int *width, int *height, 
					int frame,
					int xmin, int xmax, int ymin, int ymax)
{
  ViewFrame          *pFrame;
PtrBox              box;

  pFrame = &ViewFrameTable[frame - 1];
  box = pAb->AbBox;
  if (box == NULL)
    return FALSE;

  *x = box->BxClipX;
  *y = box->BxClipY;
  *width = box->BxClipW;
  *height = box->BxClipH;

  if (LimitBoundingBoxToClip (x, y,
			      width, height, 
			      0, 0,
			      FrameTable[frame].FrWidth, 
			      FrameTable[frame].FrHeight))
    return LimitBoundingBoxToClip (x, y,
				   width, height, 
				   xmin - pFrame->FrXOrg, ymin - pFrame->FrYOrg,
				   xmax - xmin, ymax - ymin);
  return FALSE;    
}

/*----------------------------------------------------------------------
  DisplayOpaqueGroup display a translucent Group
  ----------------------------------------------------------------------*/
void DisplayOpaqueGroup (PtrAbstractBox pAb, int frame,
			 int xmin, int xmax, int ymin, int ymax, 
				   ThotBool do_display_background)
{
  int x, y, width, height; 
  double *m;
  
  if (GetAbsoluteBoundingBox (pAb, &x, &y, &width, &height, 
			      frame, xmin, xmax, ymin, ymax))  
    {     
      m = TtaGetMemory (16 * sizeof (double));      
      glGetDoublev (GL_MODELVIEW_MATRIX, m);
      glLoadIdentity (); 

      if (do_display_background)
	{
	  GL_SetFillOpacity (1000);     
	  GL_SetOpacity (1000);
	  GL_SetStrokeOpacity (1000);
	  
	  GL_TextureMap (pAb->AbBox->Pre_computed_Pic,  
			 x, y, width, height, frame); 
	  
	}
      GL_SetFillOpacity (pAb->AbOpacity);
      GL_SetOpacity (pAb->AbOpacity);
      GL_SetStrokeOpacity (pAb->AbOpacity);

     
      GL_TextureMap (pAb->AbBox->Post_computed_Pic,
		     x, y, width, height, frame);

      GL_SetFillOpacity (1000);
      GL_SetOpacity (1000);
      GL_SetStrokeOpacity (1000);

      glLoadMatrixd (m);      
      TtaFreeMemory (m);      
    }
}
/*----------------------------------------------------------------------
  OpaqueGroupTextureFree
  ----------------------------------------------------------------------*/
void OpaqueGroupTextureFree (PtrAbstractBox     pAb, int frame)
{
PtrBox              box;

 box = pAb->AbBox;
  if (box)
    {
      if (GL_prepare (frame))
	{
	  FreeGlTextureNoCache (pAb->AbBox->Pre_computed_Pic);
	 
	  FreeGlTextureNoCache (pAb->AbBox->Post_computed_Pic);
	  
	}  
      TtaFreeMemory (pAb->AbBox->Pre_computed_Pic);

      TtaFreeMemory (pAb->AbBox->Post_computed_Pic); 
      pAb->AbBox->Pre_computed_Pic = NULL; 

      pAb->AbBox->Post_computed_Pic = NULL;
    }
}

/*----------------------------------------------------------------------
  ClearOpaqueGroup clear an area before displaying a non-opaque Group
  ----------------------------------------------------------------------*/
void ClearOpaqueGroup (PtrAbstractBox pAb, int frame, 
		       int xmin, int xmax, int ymin, int ymax)
{
  int x, y, width, height;
  int xprevclip, yprevclip, heightprevclip, widthprevclip;  
 
  if (GetAbsoluteBoundingBox (pAb, &x, &y, &width, &height, 
			      frame, xmin, xmax, ymin, ymax))
    {
      y = FrameTable[frame].FrHeight
	+ FrameTable[frame].FrTopMargin
	- (y + height);
      GL_GetCurrentClipping (&xprevclip, &yprevclip, 
			     &widthprevclip, &heightprevclip);
      GL_SetClipping (x, y, width, height);  

     /* glClearColor (0, 0, 0, 0);  */

      glClear (GL_COLOR_BUFFER_BIT); 
      GL_UnsetClipping (xprevclip, yprevclip, 
			widthprevclip, heightprevclip);
    }
}

/*----------------------------------------------------------------------
  OpaqueGroupTexturize display an non-opaque Group
  ----------------------------------------------------------------------*/
void OpaqueGroupTexturize (PtrAbstractBox pAb, int frame,
			   int xmin, int xmax, int ymin, int ymax,
                           ThotBool Is_Pre)
{
  int x, y, width, height;
  
  if (GetAbsoluteBoundingBox (pAb, &x, &y, &width, &height, 
			      frame, xmin, xmax, ymin, ymax))
    {
      y = FrameTable[frame].FrHeight
	+ FrameTable[frame].FrTopMargin
	- (y + height);
      if (Is_Pre)
	pAb->AbBox->Pre_computed_Pic = Group_shot (x, y,
						   width,
						   height,
						   frame, 
						   FALSE);
      else 
	pAb->AbBox->Post_computed_Pic = Group_shot (x, y,
						    width,
						    height,
						    frame,
						    TRUE);
    }
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
static ThotBool DisplayGradient (PtrAbstractBox pAb,
				 PtrBox box,
				 int frame, 
				 ThotBool selected)
{
  GradDef            *gradient;
  int                x, y, width, height;
  unsigned char      *pattern;
  /* int                x0,y0;   */

  gradient = pAb->AbElement->ElParent->ElGradient;
  if (gradient->next == NULL)
    return FALSE;
  
  /* orientation*/

  /*
gradient->x2 - gradient->x1;
gradient->y2 - gradient->y1;
hypot ()

  */
  
  x = box->BxXOrg;
  y = box->BxYOrg;
  width = box->BxWidth;
  height = box->BxHeight;
  

  /* if gradient pict not computed*/
  if (box->Pre_computed_Pic == NULL)
  {
    /*create the gradient pattern and put it on a texture*/
    pattern = fill_linear_gradient_image (gradient->next, 
					  width, 
					  height);
    box->Pre_computed_Pic = PutTextureOnImageDesc (pattern, 
						   width, 
						   height);    
  }
    
  /* GL_GetCurrentClipping (&clipx, &clipy, &clipw, &cliph); */
  /* if (box->BxClipW && box->BxClipH) */
  /*     GL_SetClipping (box->BxClipX, box->BxClipY, box->BxClipW, box->BxClipH); */
  /*   else */
  /*     GL_SetClipping (box->BxXOrg, box->BxYOrg, box->BxWidth, box->BxHeight); */
  
  /* Activate stenciling */
  glEnable (GL_STENCIL_TEST);
  glClear(GL_STENCIL_BUFFER_BIT);
  glStencilFunc (GL_ALWAYS, 1, 1);
  glStencilOp (GL_REPLACE, GL_REPLACE, GL_REPLACE);

  /* draw the geometric shape to get boundings in the 
     stencil buffer*/

  if (pAb->AbLeafType == LtGraphics)
    /* Graphics */
    DisplayGraph (box, frame, selected);
  else if (pAb->AbLeafType == LtPolyLine)
    /* Polyline */
    DisplayPolyLine (box, frame, selected);
  else if (pAb->AbLeafType == LtPath)
    /* Path */
    DisplayPath (box, frame, selected);

  /*Activate zone where gradient will be drawn*/
  glClear (GL_DEPTH_BUFFER_BIT);
  glStencilFunc (GL_EQUAL, 1, 1);
  glStencilOp (GL_KEEP, GL_KEEP, GL_KEEP);

  /*then draw the gradient*/
  GL_TextureMap (box->Pre_computed_Pic, 
		 x, y, 
		 width, height, frame);

  /* disable stenciling, */
  glStencilFunc (GL_NOTEQUAL, 1, 1);
  glStencilOp (GL_KEEP, GL_KEEP, GL_KEEP);
  
  /*then draw the shape 
    (again, but really, this time)*/
  if (pAb->AbLeafType == LtGraphics)
    /* Graphics */
    DisplayGraph (box, frame, selected);
  else if (pAb->AbLeafType == LtPolyLine)
    /* Polyline */
    DisplayPolyLine (box, frame, selected);
  else if (pAb->AbLeafType == LtPath)
    /* Path */
    DisplayPath (box, frame, selected);  

  glDisable (GL_STENCIL_TEST);
  /* GL_UnsetClipping (clipx, clipy, clipw, cliph); */
  return TRUE;
}

#endif /*_GL*/

/*----------------------------------------------------------------------
  DisplayBox display a box depending on its content.
  ----------------------------------------------------------------------*/
void DisplayBox (PtrBox box, int frame, int xmin, int xmax, int ymin, int ymax)
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
      if (mbox->BxType == BoGhost || mbox->BxType == BoFloatGhost)
	{
	  selected = selected || mbox->BxAbstractBox->AbSelected;
	  while (mbox->BxAbstractBox->AbEnclosing &&
		 (mbox->BxType == BoGhost ||
		  mbox->BxType == BoFloatGhost))
	    {
	      mbox = mbox->BxAbstractBox->AbEnclosing->AbBox;
	      selected = selected ||
		(mbox->BxAbstractBox->AbSelected &&
		 (mbox->BxType == BoGhost || mbox->BxType == BoFloatGhost));
	    }
	}
    }
#ifdef _GL 
  /*does box need to be recomputed 
    in a new display list*/
  if (FrameTable[frame].FrView == 1)
    {

      /* box->VisibleModification = TRUE; */

      if ((pAb->AbLeafType == LtPolyLine ||
	  /* pAb->AbLeafType == LtGraphics || */
	  pAb->AbLeafType == LtPath) &&
	  !selected)
	{
	  if (!(box->VisibleModification) &&
	      !selected &&
	      box->DisplayList &&
	      glIsList (box->DisplayList))
	    {
	      glCallList (box->DisplayList);
	      return;
	    }
	  else
	    {      
	      if (glIsList (box->DisplayList))
		{
		  glDeleteLists (box->DisplayList, 1);
		}
	      box->DisplayList = glGenLists (1);
	      glNewList (box->DisplayList,
			GL_COMPILE_AND_EXECUTE);
	    }
	}
      GL_SetFillOpacity (pAb->AbFillOpacity);
      GL_SetStrokeOpacity (pAb->AbStrokeOpacity);
      if ((pAb->AbLeafType == LtPolyLine ||
	   pAb->AbLeafType == LtGraphics ||
	   pAb->AbLeafType == LtPath) &&
	  (pAb->AbElement->ElParent) &&
	  (pAb->AbElement->ElParent->ElGradient))
	{
	  if (DisplayGradient (pAb, box, frame, selected))
	    return;          
	}
    }
#endif /*_GL*/
      
  if (pAb->AbVolume == 0 ||
      (pAb->AbLeafType == LtPolyLine && 
       box->BxNChars == 1))
    {
      /* Empty */
      selected = (box == pFrame->FrSelectionBegin.VsBox &&
		  box == pFrame->FrSelectionEnd.VsBox);
      
      if (pAb->AbLeafType == LtSymbol)
	DisplayEmptyBox (box, frame, selected);
      else if (pAb->AbLeafType != LtPolyLine &&
	       pAb->AbLeafType != LtGraphics &&
	       pAb->AbLeafType != LtPath)
	{
	  if (selected)
	    DisplayStringSelection (frame, 0, box->BxW, box);
	  else if (ThotLocalActions[T_emptybox] != NULL)
	    (*ThotLocalActions[T_emptybox]) (box, frame, selected);
	}
    }
  else if (pAb->AbLeafType == LtText)
    /* Display a Text box */
    DisplayJustifiedText (box, mbox, frame, selected);
  else if (box->BxType == BoPicture || pAb->AbLeafType == LtPicture)
    /* Picture */
    DisplayImage (box, frame, xmin, xmax, ymin, ymax, selected);
  else if (pAb->AbLeafType == LtSymbol)
    /* Symbol */
    if (pAb->AbShape == EOS)
      DisplayEmptyBox (box, frame, selected);
    else
      DisplaySymbol (box, frame, selected);
  else if (pAb->AbLeafType == LtGraphics)
    /* Graphics */
    if (pAb->AbShape == EOS)
      DisplayEmptyBox (box, frame, selected);
    else
      DisplayGraph (box, frame, selected);
  else if (pAb->AbLeafType == LtPolyLine)
    /* Polyline */
    DisplayPolyLine (box, frame, selected);
  else if (pAb->AbLeafType == LtPath)
    /* Path */
    DisplayPath (box, frame, selected);

#ifdef _GL
  if (FrameTable[frame].FrView == 1)
    {
      GL_SetFillOpacity (1000);
      GL_SetStrokeOpacity (1000);
      box->VisibleModification = FALSE;  
      if ((pAb->AbLeafType == LtPolyLine ||
/* 	  pAb->AbLeafType == LtGraphics || */
	  pAb->AbLeafType == LtPath) && 
	  !selected)
	glEndList ();
    }
#endif /*_GL*/

  /* then display borders */
  if (yd + height >= ymin
      && yd <= ymax
      && xd + width >= xmin
      && xd <= xmax)
    DisplayBorders (box, frame,
		    xd - x, yd - y,
		    width, height);
}



#define Unicode_length sizeof(Unicode_Map) / sizeof(wchar_t)

CHAR_T Unicode_Map[]={  
  /* unicode values of the arabic characters*/
    0x0621 , 0x0623 , 0x0624 , 0x0625 , 0x0626 , 0x0627 , 
    0x0628 , 0x0629 , 0x062A , 0x062B , 0x062C , 0x062D ,
    0x062E , 0x062F , 0x0630 , 0x0631 , 0x0632 , 0x0633 ,
    0x0634 , 0x0635 , 0x0636 , 0x0637 , 0x0638 , 0x0639 ,
    0x063A , 0x0640 , 0x0641 , 0x0642 , 0x0643 , 0x0644 , 
    0x0645 , 0x0646 , 0x0647 , 0x0648 , 0x0649 , 0x064A , 
    0x064B , 0x064C , 0x064D , 0x064E , 0x064F , 0x0650 , 
    0x0651 , 0x0652 , 0x0653 , 0x0654 , 0x0655 , 0x06A4 ,
    0x06AF };


#define Arab_length 52
#define fields_nbre 5
CHAR_T Arab_Map[Arab_length][fields_nbre]={
  /* arabweb positions for arabic characters with the possibility 
     to be joint with the previous and next char*/ 

{0xFE80 , 0xFE80 , 0xFE80 , 0xFE80 , 1 },   /*hamza */
{0xFE83 , 0xFE83 , 0xFE84 , 0xFE84 , 1 },   /*hamza on alif in top*/ 
{0xFE85 , 0xFE85 , 0xFE86 , 0xFE86 , 1 },   /*hamza on waw*/
{0xFE87 , 0xFE87 , 0xFE88 , 0xFE88 , 1 },   /*hamza on alif below*/
{0xFE89 , 0xFE8B , 0xFE8C , 0xFE8A , 0 },   /*hamza on ya*/
{0xFE8D , 0xFE8D , 0xFE8E , 0xFE8E , 1 },   /*alif*/
{0xFE8F , 0xFE91 , 0xFE92 , 0xFE90 , 0 },   /*Ba*/    
{0xFE93 , 0xFE93 , 0xFE94 , 0xFE94 , 1 },   /*ta marbouta*/
{0xFE95 , 0xFE97 , 0xFE98 , 0xFE96 , 0 },   /*Ta*/
{0xFE99 , 0xFE9B , 0xFE9C , 0xFE9A , 0 },   /*THa*/
{0xFE9D , 0xFE9F , 0xFEA0 , 0xFE9E , 0 },   /*jim*/ 
{0xFEA1 , 0xFEA3 , 0xFEA4 , 0xFEA2 , 0 },   /*ha*/
{0xFEA5 , 0xFEA7 , 0xFEA8 , 0xFEA6 , 0 },   /*kha*/
{0xFEA9 , 0xFEA9 , 0xFEAA , 0xFEAA , 1 },   /*dal*/
{0xFEAB , 0xFEAB , 0xFEAC , 0xFEAC , 1 },   /*dhal*/
{0xFEAD , 0xFEAD , 0xFEAE , 0xFEAE , 1 },   /*ra*/
{0xFEAF , 0xFEAF , 0xFEAF , 0xFEAF , 1 },   /*zay*/
{0xFEB1 , 0xFEB3 , 0xFEB4 , 0xFEB2 , 0 },   /*sin*/
{0xFEB5 , 0xFEB7 , 0xFEB8 , 0xFEB6 , 0 },   /*chin*/
{0xFEB9 , 0xFEBB , 0xFEBC , 0xFEBA , 0 },   /*sad*/
{0xFEBD , 0xFEBF , 0xFEC0 , 0xFEBE , 0 },   /*dad*/
{0xFEC1 , 0xFEC3 , 0xFEC4 , 0xFEC2 , 0 },   /*ta :forced*/
{0xFEC5 , 0xFEC7 , 0xFEC8 , 0xFEC6 , 0 },   /*zha*/
{0xFEC9 , 0xFECB , 0xFECC , 0xFECA , 0 },   /*ain*/
{0xFECD , 0xFECF , 0xFED0 , 0xFECE , 0 },   /*rhain*/   
{0x0640 , 0x0640 , 0x0640 , 0x0640 , 0 },   
{0xFED1 , 0xFED3 , 0xFED4 , 0xFED2 , 0 },   /*faa*/
{0xFED5 , 0xFED7 , 0xFED8 , 0xFED6 , 0 },   /*kaf:forced*/
{0xFED9 , 0xFEDB , 0xFEDC , 0xFEDA , 0 },   /*kaf*/
{0xFEDD , 0xFEDF , 0xFEE0 , 0xFEDE , 0 },   /*lam*/
{0xFEE1 , 0xFEE3 , 0xFEE4 , 0xFEE2 , 0 },   /*mim*/
{0xFEE5 , 0xFEE7 , 0xFEE8 , 0xFEE6 , 0 },   /*noun*/
{0xFEE9 , 0xFEEB , 0xFEEC , 0xFEEA , 0 },   /*ha*/
{0xFEED , 0xFEED , 0xFEEE , 0xFEEE , 1 },   /*waw*/
{0xFEEF , 0xFEEF , 0xFEF0 , 0xFEF0 , 1 },   /*alif maksoura*/
{0xFEF1 , 0xFEF3 , 0xFEF4 , 0xFEF2 , 0 },   /*ya*/
{0x064B , 0x064B , 0x064B , 0x064B , 0 },   /*fathatan*/
{0x064C , 0x064C , 0x064C , 0x064C , 0 },   /*dammatan*/
{0x064D , 0x064D , 0x064D , 0x064D , 0 },   /*kasratan*/
{0x064E , 0x064E , 0x064E , 0x064E , 0 },   /*fatha*/
{0x064F , 0x064F , 0x064F , 0x064F , 0 },   /*damma*/
{0x0650 , 0x0650 , 0x0650 , 0x0650 , 0 },   /*kasra*/
{0x0651 , 0x0651 , 0x0651 , 0x0651 , 0 },   /*chadda*/
{0x0652 , 0x0652 , 0x0652 , 0x0652 , 0 },   /*soukoun*/
{0xFE76 , 0xFE76 , 0xFE76 , 0xFE76 , 0 },   /*madda*/
{0xFE80 , 0xFE80 , 0xFE80 , 0xFE80 , 0 },   /*hamza above*/
{0xFE80 , 0xFE80 , 0xFE80 , 0xFE80 , 0 },   /*hamza below*/
{0xFB6A , 0xFB6C , 0xFB6D , 0xFB6B , 0 },    /*va */
{0xFB92 , 0xFB94 , 0xFB95 , 0xFB93 , 0 },    /*ga */
{0xFEFB , 0xFEFB , 0xFEFC , 0xFEFC , 1 },   /*lam+alif*/
{0xFEF7 , 0xFEF7 , 0xFEF8 , 0xFEF8 , 1 },   /*lam+hamza on alif in top*/
{0xFEF9 , 0xFEF9 , 0xFEFA , 0xFEFA , 1 }    /*lam+hamza on alif below*/
};


/*---------------------------------------------------------------
  FindIndex give the position of a character in Unicode_Map table
  ---------------------------------------------------------------*/
static int FindIndex (CHAR_T c, int p, int q)
{
  int q1 = (int)(p+q)/2;
  wchar_t res = Unicode_Map[q1];

  if (p >= q1) 
    return (-1);
  if (res == c) 
    return q1;
  else if (res< c) 
    return (FindIndex(c, q1, q));
  else 
    return (FindIndex(c, p, q1));
}


/*---------------------------------------------------------------------
  GetArabFontAndIndex returns the glyph corresponding to the character
  given c and it's load the arab font.                       
  ---------------------------------------------------------------------*/
int GetArabFontAndIndex (CHAR_T c, CHAR_T prev, CHAR_T next, 
			 SpecFont fontset, PtrFont *font)
{
  int    i, j, k;
 
  *font=NULL;
 
  LoadingArabicFont (fontset, font);
  if ( c == 0x061F ) 
    return 0x061F ;
  if ( c == 0x060C ) 
    return 0x060C ;
  if ( c == 0x061B ) 
    return 0x061B ;
  i = FindIndex (c, 0, Unicode_length - 1); 
  if (i == -1)
    return (c);
  else
    {
      k = FindIndex (prev, 0, Unicode_length-1);
      j = FindIndex (next, 0, Unicode_length-1);
      
      if ( c == 0x0621 ) return 0x0621;
      if (j == -1 && k == -1) 
 	return (Arab_Map[i][0]);  /* isolated char */
      else if (k == -1)
	{   /* previous char not arabic char*/
	  if (j == 0) /*hamza*/ 
	    return (Arab_Map[i][0]);
	  else 
	    {
	      if (i == 29) /*the current char is 'l'*/
		{
		  switch (j)
		    {
		    case 5: return (Arab_Map[Arab_length -3][0]);
		    case 1: return (Arab_Map[Arab_length -2][0]);
		    case 3: return (Arab_Map[Arab_length -1][0]);
		    default: return (Arab_Map[i][1]);
		    }
		}
	      else if ((i == 1 || i == 3 || i == 5) && k == 29) /*previous char is 'l'*/
		return (0x0020);
	      return (Arab_Map[i][1]);
	    }
	}
      else if (j == -1)
	{ /*the next char is not arabic char */
	  if (Arab_Map[k][4])
	    return (Arab_Map[i][0]);
	  else if ((i == 1 || i == 3 || i == 5) && k == 29)
	    return (0x0020);
	  return (Arab_Map[i][3]);
	}
      else
	{
	  if (!Arab_Map[k][4]) 
	    {
	      if (k == 29 && (i == 1 || i == 3 || i == 5))
		return(0x0020);
	      else if (i == 29)
		{
		  switch (j) 
		    {
		    case 1: return (Arab_Map[Arab_length -2][2]);
		    case 3: return (Arab_Map[Arab_length -1][2]);
		    case 5: return (Arab_Map[Arab_length -3][2]);
		    default: return (Arab_Map[i][2]);
		    }
		}
	      return (Arab_Map[i][2]);
	    }
	  else 
	    {
	      if (Arab_Map[k][4])
		{
		  if (i == 29)
		    {
		      switch (j) 
			{
			case 1: return (Arab_Map[Arab_length -2][0]);
			case 3: return (Arab_Map[Arab_length -1][0]);
			case 5: return (Arab_Map[Arab_length -3][0]);
			default: return (Arab_Map[i][1]);
			}
		    }
		  return (Arab_Map[i][1]);
		}
	      else  
		{
		  if (i == 29)
		    {
		      switch (j) 
			{
			case 1: return (Arab_Map[Arab_length -2][2]);
			case 3: return (Arab_Map[Arab_length -1][2]);
			case 5: return (Arab_Map[Arab_length -3][2]);
			default: return (Arab_Map[i][3]);
			}
		    }
		  return(Arab_Map[i][3]);
		}
	    }
	}
    }
 
}
