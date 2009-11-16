/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2009
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * displaybox.c : all the stuff needed to display boxes in frames.
 *
 * Authors: I. Vatton (INRIA)
 *          P. Cheyrou-lagreze (INRIA)
 */

#ifdef _WX
#include "wx/wx.h"
#endif /* _WX */

#include "thot_gui.h"
#include "ustring.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "picture.h"
#include "appdialogue.h"

#define THOT_EXPORT extern
#include "boxes_tv.h"
#include "edit_tv.h"
#include "font_tv.h"
#include "frame_tv.h"
#include "appdialogue_tv.h"
#include "picture_tv.h" 

#include "boxrelations_f.h"
#include "buildboxes_f.h"
#include "displaybox_f.h"
#include "displayselect_f.h"
#include "font_f.h"
#include "exceptions_f.h"
#include "memory_f.h"
#include "picture_f.h"
#include "registry_f.h"
#include "selectionapi_f.h"
#include "stix_f.h"
#include "units_f.h"
#include "windowdisplay_f.h"

#ifdef _GL
#if defined (_MACOS) && defined (_WX)
#include <gl.h>
#else /* _MACOS */
#include <GL/gl.h>
#endif /* _MACOS */
#include "glwindowdisplay.h"
#include "openglfont.h"
#include "frame_f.h"
#include "appli_f.h"
#include "glgradient_f.h"
#include "glprint.h"
#endif /*_GL*/

#include "fileaccess.h"

#ifdef _GL
void DrawRectangle2 (int frame, int thick, int style, int x, int y, int width,
		     int height, int fg, int bg, int pattern);
#endif /*_GL*/

/*-------------------------------------------------------------------------
  NextChar returns the previous character of the same buffer according
  to the index given by ind and according to the orientation.it returns 
  space if it pass to another buffer
  -------------------------------------------------------------------------*/
static CHAR_T Previous_Char (PtrTextBuffer *adbuff, int *ind)
{
  if (*ind < ((*adbuff)->BuLength-1))
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
  t, b, l, and r give top, bottom, left and right extra margins.
  ----------------------------------------------------------------------*/
static void DisplayImage (PtrBox pBox, int frame, int xmin, int xmax,
                          int ymin, int ymax, ThotBool selected,
                          int t, int b, int l, int r)
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
      l += pBox->BxLMargin + pBox->BxLBorder + pBox->BxLPadding;
      t += pBox->BxTMargin + pBox->BxTBorder + pBox->BxTPadding;
      xd = pBox->BxXOrg + l - x;
      yd = pBox->BxYOrg + t + FrameTable[frame].FrTopMargin - y;
      width = pBox->BxW;
      height = pBox->BxH;
      DrawPicture (pBox, (ThotPictInfo *) pBox->BxPictInfo, frame, xd, yd,
                   width, height, t, l, TRUE);
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
                                    t, pFrame->FrSelectionBegin.VsBox);
          else
            DisplayPointSelection (frame, pBox, 0, TRUE);
        }
    }
}

/*----------------------------------------------------------------------
  AnimatedBoxAdd : Add a reference to animated element in the frame
  ----------------------------------------------------------------------*/
void AnimatedBoxAdd (PtrElement element)
{
#ifdef _GL 
  Animated_Cell *current;

  if (FrameTable[ActiveFrame].Animated_Boxes == NULL)
    {
      FrameTable[ActiveFrame].Animated_Boxes = TtaGetMemory (sizeof(Animated_Cell));
      current = (Animated_Cell *) FrameTable[ActiveFrame].Animated_Boxes;
    }
  else
    {
      current = (Animated_Cell *) FrameTable[ActiveFrame].Animated_Boxes;
      while (current->Next)
        {
          if (current->El == element)
            return;
          current = current->Next;
        }
      
      current->Next = (Animated_Cell *)TtaGetMemory (sizeof(Animated_Cell));
      current = current->Next;
    }
  current->Next = NULL;
  current->El = element;
  /* current->El = element->ElParent; */
#endif /* _GL */
}
/*----------------------------------------------------------------------
  AnimatedBoxDel : Delete a reference to an animated element
  ----------------------------------------------------------------------*/
void AnimatedBoxDel (PtrElement element)
{
#ifdef _GL 
  Animated_Cell *current, *previous;
  ThotBool not_found = TRUE;
  
  if (element->ElAnimation)
    {
      if (FrameTable[ActiveFrame].Animated_Boxes != NULL)
        {
          previous = NULL;
          current = (Animated_Cell *) FrameTable[ActiveFrame].Animated_Boxes;
          while (current && not_found)
            {
              if (current->El == element)
                not_found = FALSE;
              else
                {
                  previous = current;	  
                  current = current->Next;
                }	      
            }
	  
          if (not_found)
            return;

          if (previous)
            {	      
              if (previous && current && current->Next)
                previous->Next = current->Next;
              else
                previous->Next = NULL;
            }	  
          else
            FrameTable[ActiveFrame].Animated_Boxes = NULL;
          TtaFreeMemory (current);
        }
    }
#endif /* _GL */
}
/*----------------------------------------------------------------------
  FreeAnimatedBoxes : Free all animation allocated resources
  ----------------------------------------------------------------------*/
void FreeAnimatedBoxes (Animated_Cell *current)
{
#ifdef _GL 
  Animated_Cell *next;

  while (current)
    {
      next = current->Next;
      TtaFreeMemory (current);
      current = next;
    }
#endif /* _GL */
}

#ifdef _WINGUI
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
#endif /*_WINGUI*/

/*----------------------------------------------------------------------
  DisplaySymbol displays a mathematical symbols box enclosed in
  a frame. The glyphs are drawn with the Greek font and lines.
  t, b, l, and r give top, bottom, left and right extra margins.
  ----------------------------------------------------------------------*/
static void DisplaySymbol (PtrBox pBox, int frame, ThotBool selected,
                           int t, int b, int l, int r)
{
  ThotFont            font;
  ViewFrame          *pFrame;
  int                 xd, yd, i, w;
  int                 fg, bg;
  int                 width, height, baseline, size;
  PtrBox              ancestor;
  ThotBool            useStix;
#ifdef _GL
  int                 texture_id = 0;
#endif /* _GL */

  pFrame = &ViewFrameTable[frame - 1];
  if (pBox->BxAbstractBox->AbVisibility >= pFrame->FrVisibility)
    {
      font = NULL;
      baseline = 0;
      useStix = FALSE;
      /* if its a prenthesis, a brace or a bracket, and if this character
         is not high, draw it as an ordinary character */
      if (pBox->BxH <= ((int)1.3 * pBox->BxFont->FontSize) &&
          (pBox->BxAbstractBox->AbShape == '(' ||
           pBox->BxAbstractBox->AbShape == ')' ||
           pBox->BxAbstractBox->AbShape == '[' ||
           pBox->BxAbstractBox->AbShape == ']' ||
           pBox->BxAbstractBox->AbShape == '<' ||
           pBox->BxAbstractBox->AbShape == '>' ||
           pBox->BxAbstractBox->AbShape == '{' ||
           pBox->BxAbstractBox->AbShape == '}'))
        {
          /* get the regular font for that box (not the Symbol font used by
             the SYMBOL element, but the regular font of its parent) */
          if (pBox->BxAbstractBox->AbEnclosing &&
              pBox->BxAbstractBox->AbEnclosing->AbBox)
            GetFontAndIndexFromSpec (32, pBox->BxAbstractBox->AbEnclosing->AbBox->BxFont, 1, &font);
          if (font && pBox->BxH <= (int) (1.3 * FontHeight (font)))
            /* this character is almost the height of an ordinary character;
               use the regular font to display it */
            {
              if (FrameTable[frame].FrView == 1)
                /* it's a symbol in the formatted view */
                /* its baseline is the same as the baseline of its
                   grand parent */
                if (pBox->BxAbstractBox->AbEnclosing &&
                    pBox->BxAbstractBox->AbEnclosing->AbEnclosing &&
                    pBox->BxAbstractBox->AbEnclosing->AbEnclosing->AbBox)
                  {
                    ancestor = pBox->BxAbstractBox->AbEnclosing->AbEnclosing->AbBox;
                    baseline = ancestor->BxYOrg + ancestor->BxHorizRef +
                      ancestor->BxTMargin + ancestor->BxTBorder +
                      ancestor->BxTPadding - pFrame->FrYOrg;
                  }
            }
          else
            /* this is an extended symbol. */
            font = NULL;
        }

        if (StixExist && font == NULL && pBox->BxH > 0)
          {
            size = PixelToPoint (pBox->BxH);
            if (pBox->BxFont && pBox->BxFont->FontSize > size)
              size = pBox->BxFont->FontSize;
            GetMathFontFromChar (pBox->BxAbstractBox->AbShape,
                                 pBox->BxFont,
                                 (void **) &font, size);
            if (font)
              useStix = TRUE;
          }
        else
          size = pBox->BxFont->FontSize;

      if (font == NULL)
        GetFontAndIndexFromSpec (32, pBox->BxFont, 1, &font);
      if (font != NULL)
        {
          /* Position in the frame */
          xd = pBox->BxXOrg + pBox->BxLMargin + pBox->BxLBorder +
            pBox->BxLPadding - pFrame->FrXOrg;
          yd = pBox->BxYOrg + pBox->BxTMargin + pBox->BxTBorder +
            pBox->BxTPadding - pFrame->FrYOrg;
	  
          /* box dimensions have to be positive */
          width = pBox->BxW;
          if (width < 0)
            width = 0;
          height = pBox->BxH;
          if (height < 0)
            height = 0;
	  
          if (selected &&
              !pFrame->FrSelectOnePosition &&
              pFrame->FrSelectionBegin.VsXPos != pBox->BxW)
            DisplayStringSelection (frame, 0, pBox->BxW, t, pBox);
	  
          /* Line thickness */
          i = GetLineWeight (pBox->BxAbstractBox, frame);
#ifdef _GL
          texture_id = SetTextureScale (IsBoxDeformed(pBox));
#endif /* _GL */
          if (selected && !pFrame->FrSelectOnePosition)
            {
              fg = FgSelColor;
              bg = BgSelColor;
            }
          else
            {
              fg = pBox->BxAbstractBox->AbForeground;
              bg = pBox->BxAbstractBox->AbBackground;
            }

          switch (pBox->BxAbstractBox->AbShape)
            {
            case 0:
              break;
            /* Fences */
            case 1: /* LeftDoubleBracket ; U0301A */
              if (useStix)
                DrawStixBracket (frame, xd, yd, width, height, 0, 1, size, fg);
              break;
            case 2: /* RightDoubleBracket ; U0301B */
              if (useStix)
                DrawStixBracket (frame, xd, yd, width, height, 1, 1, size, fg);
              break;
            case 3: /* LeftFloor ; U0230A */
              if (useStix)
                DrawStixBracket (frame, xd, yd, width, height, 0, 3, size, fg);
              break;
            case 4: /* RightFloor ; U0230B */
              if (useStix)
                DrawStixBracket (frame, xd, yd, width, height, 1, 3, size, fg);
              break;
            case 5: /* LeftCeiling ; U02308 */
              if (useStix)
                DrawStixBracket (frame, xd, yd, width, height, 0, 2, size, fg);
              break;
            case 6: /* RightCeiling ; U02309 */
              if (useStix)
                DrawStixBracket (frame, xd, yd, width, height, 1, 2, size, fg);
              break;
            case '(': /* c = 40 */
              if (useStix)
                DrawStixParenthesis (frame, xd, yd, width, height, 0, size,fg);
              else
                DrawParenthesis (frame, i, xd, yd, width, height, 0, font, fg,
                                 baseline);
              break;
            case ')': /* c = 41 */
              if (useStix)
                DrawStixParenthesis (frame, xd, yd, width, height, 1, size,fg);
              else
                DrawParenthesis (frame, i, xd, yd, width, height, 1, font, fg,
                                 baseline);
              break;
            case '{': /* c = 123 */
              if (useStix)
                DrawStixBrace (frame, xd, yd, width, height, 0, size, fg);
              else
                DrawBrace (frame, i, xd, yd, width, height, 0, font, fg,
                           baseline);
              break;
            case '}': /* c = 125 */
              if (useStix)
                DrawStixBrace (frame, xd, yd, width, height, 1, size, fg);
              else
                DrawBrace (frame, i, xd, yd, width, height, 1, font, fg,
                           baseline);
              break;
            case '[': /* c = 91 */
              if (useStix)
                DrawStixBracket (frame, xd, yd, width, height, 0, 0, size, fg);
              else
                DrawBracket (frame, i, xd, yd, width, height, 0, font, fg,
                             baseline);
              break;
            case ']': /* c = 93 */
              if (useStix)
                DrawStixBracket (frame, xd, yd, width, height, 1, 0, size, fg);
              else
                DrawBracket (frame, i, xd, yd, width, height, 1, font, fg,
                             baseline);
              break;
            case '<': /* c = 60 */
              if (useStix)
                DrawStixPointyBracket (frame, xd, yd, width, height, 0, size,
                                       fg);
              else
                DrawPointyBracket (frame, i, xd, yd, width, height, 0, font,
                                   fg);
              break;
            case '>': /* c = 62 */
              if (useStix)
                DrawStixPointyBracket (frame, xd, yd, width, height, 1, size,
                                       fg);
              else
                DrawPointyBracket (frame, i, xd, yd, width, height, 1, font,
                                   fg);
              break;

           /* Lines and bars */
            case '|': /* c= 124 verticalLine */
            case 7 : /* VerticalSeparator */
            case 11 : /* VerticalBar */
              DrawVerticalLine (frame, i, 5, xd, yd, width, height, 1, fg, pBox,
                                0, 0);
              break;
            case 12 : /* DoubleVerticalBar */
              DrawVerticalLine (frame, i, 6, xd, yd, width, height, 1, fg, pBox,
                                0, 0);
              break;
            case 8 : /* HorizontalLine ; U02500 */
            case 9 : /* UnderBar ; U00332 */
            case 10 : /* OverBar ; U000AF */
            case '-': /* c = 45 */
            case '_': /* c = 95 */
            case 'h': /* c = 104 */
              DrawHorizontalLine (frame, i, 5, xd, yd, width, height, 1, fg, pBox,
                                  0, 0);
              break;
            case '/':
            case '\\':
                /* TODO */
              break;

           /* START_ENTITY : c = 26 */

            case '1':/* Clockwise Integral */
              if (useStix)
                DrawStixIntegral (frame, xd, yd, width, height, 6, size, fg);
              else
                DrawIntegral (frame, i, xd, yd, width, height, 6, font, fg);
              break;
            case '2':/* Clockwise Contour Integral */
              if (useStix)
                DrawStixIntegral (frame, xd, yd, width, height, 7, size, fg);
              else
                DrawIntegral (frame, i, xd, yd, width, height, 7, font, fg);
              break;
            case '3':/* Counter Clockwise Contour Integral */
              if (useStix)
                DrawStixIntegral (frame, xd, yd, width, height, 8, size, fg);
              else
                DrawIntegral (frame, i, xd, yd, width, height, 8, font, fg);
              break;
            case 'b':/* horizontal bracket */
                DrawHorizontalBracket (frame, i, 5, xd, yd, width, height, 0,fg);
              break;
            case 'B':/* horizontal bracket */
                DrawHorizontalBracket (frame, i, 5, xd, yd, width, height, 1,fg);
              break;
            case 'c':/* contour integral */
              if (useStix)
                DrawStixIntegral (frame, xd, yd, width, height, 1, size, fg);
              else
                DrawIntegral (frame, i, xd, yd, width, height, 1, font, fg);
              break;
            case 'd':/* double integral */
              if (useStix)
                DrawStixIntegral (frame, xd, yd, width, height, 2, size, fg);
              else
                DrawIntegral (frame, i, xd, yd, width, height, 2, font, fg);
              break;
            case 'e':/* double contour integral */
              if (useStix)
                DrawStixIntegral (frame, xd, yd, width, height, 4, size, fg);
              else
                DrawIntegral (frame, i, xd, yd, width, height, 4, font, fg);
              break;
            case 'f':/* triple contour integral */
              if (useStix)
                DrawStixIntegral (frame, xd, yd, width, height, 5, size, fg);
              else
                DrawIntegral (frame, i, xd, yd, width, height, 5, font, fg);
              break;
            case 'H':/* Hat */
              DrawHat (frame, i, 5, xd, yd, width, height, fg, 1);
              break;
            case 'i':/* integral */ 
              if (useStix)
                DrawStixIntegral (frame, xd, yd, width, height, 0, size, fg);
              else
                DrawIntegral (frame, i, xd, yd, width, height, 0, font, fg);
              break;
            case 'k':/* Hacek */
              DrawHat (frame, i, 5, xd, yd, width, height, fg,-1);
              break;
            case 'o':/* horizontal brace */
              if (useStix)
                DrawStixHorizontalBrace (frame, xd, yd, width, height, 0, size, fg);
              else
                DrawHorizontalBrace (frame, i, 5, xd, yd, width, height, 0,fg);
              break;
            case 'p':
              DrawHorizontalParenthesis (frame, i, 5, xd, yd, width, height, 0, fg);
              break;
            case 'q':
              DrawHorizontalParenthesis (frame, i, 5, xd, yd, width, height, 1, fg);
              break;
            case 'r':/* radical */
              DrawRadical (frame, i, xd, yd, width, height, font, fg);
              break;
            case 't': /* triple integral */
              if (useStix)
                DrawStixIntegral (frame, xd, yd, width, height, 3, size, fg);
              else
                DrawIntegral (frame, i, xd, yd, width, height, 3, font, fg);
              break;
            case 'u':
              if (useStix)
                DrawStixHorizontalBrace (frame, xd, yd, width, height, 1, size, fg);
              else
                DrawHorizontalBrace (frame, i, 5, xd, yd, width, height, 1,fg);
              break;
            case 'v':
              DrawVerticalLine (frame, i, 5, xd, yd, width, height, 1, fg, pBox, 0, 0);
              break;
            case 'D':
              DrawVerticalLine (frame, i, 6, xd, yd, width, height, 1, fg, pBox, 0, 0);
              break;
            case 'I':
              DrawIntersection (frame, xd, yd, width, height, font, fg);
              break;
            case 'P':
              DrawPi (frame, xd, yd, width, height, font, fg);
              break;
            case 'S':
              if (useStix)
                DrawStixSigma (frame, xd, yd, width, height, font, fg);
              else
                DrawSigma (frame, xd, yd, width, height, font, fg);
              break;
            case 'T':/* Diacritical Tilde */
              DrawTilde (frame, i, 5, xd, yd, width, height,fg);
              break;
            case 'U':
              DrawUnion (frame, xd, yd, width, height, font, fg);
              break;

         /* c = 150 to 213 : Display one of the 64 strechable arrows from
            the MathML 2.0 Operator dictionary (appendix F.5)  */
            case 'L': /* (c = 76 [change to 150 ?]) ; LeftArrow ; U02190  */
              if (useStix)
                DrawStixHorizArrow (frame, xd, yd, width, height, 1, size, fg);
              else
                DrawArrow (frame, i, 5, xd, yd, width, height, 180, 0, fg);
              break;
            case '^': /* (c = 94 [change to 151 ?]) ; UpArrow ; U02191 */
              DrawArrow (frame, i, 5, xd, yd, width, height, 90, 0, fg);
              break;
            case 'R': /* (c = 82 [change to 152 ?]) ; RightArrow ; U02192 */
              if (useStix)
                DrawStixHorizArrow (frame, xd, yd, width, height, 0, size, fg);
              else
                DrawArrow (frame, i, 5, xd, yd, width, height, 0, 0, fg);
              break;
            case 'V': /* (c = 86 [change to 153 ?]) ; DownArrow ; U02193 */
              DrawArrow (frame, i, 5, xd, yd, width, height, 270, 0, fg);
              break;
            case 'A': /* (c = 87 [change to 154 ?]) LeftRightArrow ; U02194 */
              DrawArrow (frame, i, 5, xd, yd, width, height, 0, 1, fg);
              break;
            case 155: /* UpDownArrow ; U02195 */
              DrawArrow (frame, i, 5, xd, yd, width, height, 90, 1, fg);
              break;
            case 156: /* UpperLeftArrow ; U02196 */
              DrawArrow (frame, i, 5, xd, yd, width, height, 135, 0, fg);
              break;
            case 157: /* UpperRightArrow ; U02197 */
              DrawArrow (frame, i, 5, xd, yd, width, height, 45, 0, fg);
              break;
            case 158: /* LowerRightArrow ; U02198 */
              DrawArrow (frame, i, 5, xd, yd, width, height, 315, 0, fg);
              break;
            case 159: /* LowerLeftArrow ; U02199 */
              DrawArrow (frame, i, 5, xd, yd, width, height, 225, 0, fg);
              break;
            case 160: /* LeftTeeArrow ; U021A4 */
              DrawArrow (frame, i, 5, xd, yd, width, height, 180, 5, fg);
              break;
            case 161: /* UpTeeArrow ; U021A5 */
              DrawArrow (frame, i, 5, xd, yd, width, height, 90, 5, fg);
              break;
            case 162: /* RightTeeArrow ; U021A6 */
              DrawArrow (frame, i, 5, xd, yd, width, height, 0, 5, fg);
              break;
            case 163: /* DownTeeArrow ; U021A7 */
              DrawArrow (frame, i, 5, xd, yd, width, height, 270, 5, fg);
              break;
            case 164: /* LeftVector ; U021BC */
              DrawArrow (frame, i, 5, xd, yd, width, height, 180, 7, fg);
              break;
            case 165: /* DownLeftVector ; U021BD */
              DrawArrow (frame, i, 5, xd, yd, width, height, 180, 12, fg);
              break;
            case 166: /* RightUpVector ;  U021BE */
              DrawArrow (frame, i, 5, xd, yd, width, height, 90, 12, fg);
              break;
            case 167: /* LeftUpVector ;  U021BF  */
              DrawArrow (frame, i, 5, xd, yd, width, height, 90, 7, fg);
              break;
            case 168: /* RightVector ;  U021C0  */
              DrawArrow (frame, i, 5, xd, yd, width, height, 0, 7, fg);
              break;
            case 169: /* DownRightVector ; U021C1 */
              DrawArrow (frame, i, 5, xd, yd, width, height, 0, 12, fg);
              break;
            case 170: /* RightDownVector ; U021C2 */
              DrawArrow (frame, i, 5, xd, yd, width, height, 270, 12, fg);
              break;
            case 171: /* LeftDownVector ; U021C3 */
              DrawArrow (frame, i, 5, xd, yd, width, height, 270, 7, fg);
              break;
            case 172: /* RightArrowLeftArrow ; U021C4 */
              DrawArrow (frame, i, 5, xd, yd, width, height, 0, 4, fg);
              break;
            case 173: /* UpArrowDownArrow ; U021C5 */
              DrawArrow (frame, i, 5, xd, yd, width, height, 90, 4, fg);
              break;
            case 174: /* LeftArrowRightArrow ; U021C6 */
              DrawArrow (frame, i, 5, xd, yd, width, height, 180, 4, fg);
              break;
            case 175: /* DoubleLeftArrow ; U021D0 */
              DrawArrow (frame, i, 5, xd, yd, width, height, 180, 2, fg);
              break;
            case 176: /* DoubleUpArrow ; U021D1 */
              DrawArrow (frame, i, 5, xd, yd, width, height, 90, 2, fg);
              break;
            case 177: /* DoubleRightArrow ; U021D2 */
              DrawArrow (frame, i, 5, xd, yd, width, height, 0, 2, fg);
              break;
            case 178: /* DoubleDownArrow ; U021D3 */
              DrawArrow (frame, i, 5, xd, yd, width, height, 270, 2, fg);
              break;
            case 179: /* DoubleLeftRightArrow ; U021D4 */
              DrawArrow (frame, i, 5, xd, yd, width, height, 0, 3, fg);
              break;
            case 180: /* DoubleUpDownArrow ; U021D5 */
              DrawArrow (frame, i, 5, xd, yd, width, height, 90, 3, fg);
              break;
            case 181: /* LeftArrowBar ; U021E4 */
              DrawArrow (frame, i, 5, xd, yd, width, height, 180, 6, fg);
              break;
            case 182: /* RightArrowBar ; U021E5 */
              DrawArrow (frame, i, 5, xd, yd, width, height, 0, 6, fg);
              break;
            case 183: /* DownArrowUpArrow ; U021F5 */
              DrawArrow (frame, i, 5, xd, yd, width, height, 270, 4, fg);
              break;
            case 184: /* LongLeftArrow ; U027F5 */
              DrawArrow (frame, i, 5, xd, yd, width, height, 180, 0, fg);
              break;
            case 185: /* LongRightArrow ; U027F6 */
              DrawArrow (frame, i, 5, xd, yd, width, height, 0, 0, fg);
              break;
            case 186: /* LongLeftRightArrow ; U027F7 */
              DrawArrow (frame, i, 5, xd, yd, width, height, 0, 1, fg);
              break;
            case 187: /* DoubleLongLeftArrow ; U027F8 */
              DrawArrow (frame, i, 5, xd, yd, width, height, 180, 2, fg);
              break;
            case 188: /* DoubleLongRightArrow ; U027F9 */
              DrawArrow (frame, i, 5, xd, yd, width, height, 0, 2, fg);
              break;
            case 189: /* DoubleLongLeftRightArrow ; U027FA */
              DrawArrow (frame, i, 5, xd, yd, width, height, 0, 3, fg);
              break;
            case 190: /* UpArrowBar ; U02912 */
              DrawArrow (frame, i, 5, xd, yd, width, height, 90, 6, fg);
              break;
            case 191: /* DownArrowBar ; U02913 */
              DrawArrow (frame, i, 5, xd, yd, width, height, 270, 6, fg);
              break;
            case 192: /* LeftRightVector ; U0294E */
              DrawArrow (frame, i, 5, xd, yd, width, height, 0, 8, fg);
              break;
            case 193: /* RightUpDownVector ; U0294F */
              DrawArrow (frame, i, 5, xd, yd, width, height, 90, 13, fg);
              break;
            case 194: /* DownLeftRightVector ; U02950 */
              DrawArrow (frame, i, 5, xd, yd, width, height, 0, 13, fg);
              break;
            case 195: /* LeftUpDownVector ; U02951 */
              DrawArrow (frame, i, 5, xd, yd, width, height, 90, 8, fg);
              break;
            case 196: /* LeftVectorBar ; U02952 */
              DrawArrow (frame, i, 5, xd, yd, width, height, 180, 10, fg);
              break;
            case 197: /* RightVectorBar ; U02953 */
              DrawArrow (frame, i, 5, xd, yd, width, height, 0, 10, fg);
              break;
            case 198: /* RightUpVectorBar ; U02954 */
              DrawArrow (frame, i, 5, xd, yd, width, height, 90, 15, fg);
              break;
            case 199: /* RightDownVectorBar ; U02955 */
              DrawArrow (frame, i, 5, xd, yd, width, height, 270, 15, fg);
              break;
            case 200: /* DownLeftVectorBar ; U02956 */
              DrawArrow (frame, i, 5, xd, yd, width, height, 180, 15, fg);
              break;
            case 201: /* DownRightVectorBar ; U02957 */
              DrawArrow (frame, i, 5, xd, yd, width, height, 0, 15, fg);
              break;
            case 202: /* LeftUpVectorBar ; U02958 */
              DrawArrow (frame, i, 5, xd, yd, width, height, 90, 10, fg);
              break;
            case 203: /* LeftDownVectorBar ; U02959 */
              DrawArrow (frame, i, 5, xd, yd, width, height, 270, 10, fg);
              break;
            case 204: /* LeftTeeVector ; U0295A */
              DrawArrow (frame, i, 5, xd, yd, width, height, 180, 9, fg);
              break;
            case 205: /* RightTeeVector ; U0295B */
              DrawArrow (frame, i, 5, xd, yd, width, height, 90, 9, fg);
              break;
            case 206: /* RightUpTeeVector ; U0295C */
              DrawArrow (frame, i, 5, xd, yd, width, height, 90, 14, fg);
              break;
            case 207: /* RightDownTeeVector ; U0295D */
              DrawArrow (frame, i, 5, xd, yd, width, height, 270, 14, fg);
              break;
            case 208: /* DownLeftTeeVector ; U0295E */
              DrawArrow (frame, i, 5, xd, yd, width, height, 180, 14, fg);
              break;
            case 209: /* DownRightTeeVector ; U0295F */
              DrawArrow (frame, i, 5, xd, yd, width, height, 0, 14, fg);
              break;
            case 210: /* LeftUpTeeVector ; U02960 */
              DrawArrow (frame, i, 5, xd, yd, width, height, 90, 9, fg);
              break;
            case 211: /* LeftDownTeeVector ; U02961 */
              DrawArrow (frame, i, 5, xd, yd, width, height, 270, 9, fg);
              break;
            case 212: /* UpEquilibrium ; U0296E */
              DrawArrow (frame, i, 5, xd, yd, width, height, 90, 11, fg);
              break;
            case 213: /* ReverseUpEquilibrium ; U0296F */
              DrawArrow (frame, i, 5, xd, yd, width, height, 270, 11, fg);
              break;
            case 214: /* Equilibrium ; U021CC */
              DrawArrow (frame, i, 5, xd, yd, width, height, 0, 11, fg);
              break;
            case 215: /* ReverseEquilibrium ; U021CB */
              DrawArrow (frame, i, 5, xd, yd, width, height, 180, 11, fg);
              break;

            case '?': /* c = 63 */
            case UNDISPLAYED_UNICODE: /* c = 27 */
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
              DrawChar ((CHAR_T)pBox->BxAbstractBox->AbShape, frame, xd,
                        yd + FontBase (font), font, fg);
              break;
            } 
#ifdef _GL
          if (texture_id)
            StopTextureScale (texture_id);
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
                                    t, pBox);
        }
    }
}

/*----------------------------------------------------------------------
  DisplayEmptyBox shows an empty box but formatted and placed.
  A specific background is drawn in the box area.
  t, b, l, and r give top, bottom, left and right extra margins.
  ----------------------------------------------------------------------*/
void DisplayEmptyBox (PtrBox pBox, int frame, ThotBool selected,
                      int t, int b, int l, int r)
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
      xd = pBox->BxXOrg + pBox->BxLMargin + pBox->BxLBorder
        + pBox->BxLPadding - pFrame->FrXOrg;
      yd = pBox->BxYOrg + pBox->BxTMargin + pBox->BxTBorder
        + pBox->BxTPadding - pFrame->FrYOrg;
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
          bg = BgSelColor;
          if (pAb->AbLeafType == LtGraphics)
            DrawRectangle (frame, 2, 0, xd, yd, width,
                           height, pAb->AbForeground, bg, 0);
          else
            {
              PaintWithPattern (frame, xd, yd, width, height, 0,
                                pAb->AbForeground, bg, 4);
              DisplayStringSelection (frame, 0, 2, t, pBox);
            }
        }
      else
        {
          bg = pAb->AbBackground;
          if (pAb->AbLeafType == LtGraphics)
            DrawRectangle (frame, 2, 0, xd, yd, width,
                           height, pAb->AbForeground, bg, 0);
          else
            PaintWithPattern (frame, xd, yd, width, height, 0,
                              pAb->AbForeground, bg, 4);
        }
      
    }
}


/*----------------------------------------------------------------------
  DisplayGraph display a graphic.
  The parameter selected is TRUE when the graphic is selected.
  t, b, l, and r give top, bottom, left and right extra margins.
  ----------------------------------------------------------------------*/
void  DisplayGraph (PtrBox pBox, int frame, ThotBool selected,
                    int t, int b, int l, int r)
{
  ViewFrame          *pFrame;
  PtrAbstractBox      pAb;
  int                 i;
  int                 fg, bg;
  int                 pat;
  int                 style;
  int                 xd, yd, left, top; 
  int                 width, height;

  pAb = pBox->BxAbstractBox;
  pFrame = &ViewFrameTable[frame - 1];
  if (pAb->AbVisibility >= pFrame->FrVisibility)
    {
      if (selected && pAb->AbPresentationBox &&
          (FrameTable[frame].FrView == 1 || !pAb->AbSelected))
        {
          /* paint with selection colors */
          bg = BgSelColor;
          fg = FgSelColor;
          pat = 2;
        }
      else
        {
	  bg = pAb->AbBackground;
          fg = pAb->AbForeground;
          pat = pAb->AbFillPattern;
        }
      left = pBox->BxLMargin + l + pBox->BxLBorder + pBox->BxLPadding - pFrame->FrXOrg;
      top = pBox->BxTMargin + t + pBox->BxTBorder + pBox->BxTPadding - pFrame->FrYOrg;
      xd = pBox->BxXOrg + left;
      yd = pBox->BxYOrg + top;
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
        case 1: /* Square */
        case 'C': /* Rectangle */
        case 7: /* Square */
        case 8: /* Rectangle */
          if (pBox->BxRx == 0 || pBox->BxRy == 0)
#ifdef _GL
            DrawRectangle2(frame, i, style, xd, yd, width, height, fg, bg, pat);
#else
            DrawRectangle(frame, i, style, xd, yd, width, height, fg, bg, pat);
#endif /*_GL*/
          else
            DrawOval (frame, i, style, xd, yd, width, height,
                      (pBox->BxRx == -1 ? pBox->BxRy : pBox->BxRx),
                      (pBox->BxRy == -1 ? pBox->BxRx : pBox->BxRy),
                      fg, bg, pat);
          break;

        case 'L': /* Diamond */
          DrawDiamond (frame, i, style, xd, yd, width, height, fg, bg, pat);
          break;

        case 2: /* Parallelogram */
          DrawParallelogram (frame, i, style, xd, yd, width, height, pBox->BxRx,
                             fg, bg, pat);
          break;

        case 3: /* Trapezium */
          DrawTrapezium (frame, i, style, xd, yd, width, height,
                         pBox->BxRx, pBox->BxRy,
                         fg, bg, pat);
          break;

        case 4: /* Equilateral triangle */
        case 5: /* Isosceles triangle */
          DrawTriangle (frame, i, style, fg, bg, pat,
                        xd+width/2,yd,
                        xd,yd+height,
                        xd+width,yd+height);	  
          break;

        case 6: /* Rectangled triangle */
          DrawTriangle (frame, i, style, fg, bg, pat,
                        xd,yd,
                        xd,yd+height,
                        xd+width,yd);
          break;

        case 'a': /* Circle */
        case 'c': /* Ellipse */
          DrawEllips (frame, i, style, xd, yd, width, height, fg, bg, pat);
          break;

        case '0':
          DrawRectangle (frame, 0, 0, xd, yd, width, height, fg, bg, pat);
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
        case 'h':
          DrawHorizontalLine (frame, i, style, xd, yd, width, height, 1, fg, pBox,
                              0, 0);
          break;
        case 't':
          DrawHorizontalLine (frame, i, style, xd, yd, width, height, 0, fg, pBox,
                              0, 0);
          break;
        case 'b':
          DrawHorizontalLine (frame, i, style, xd, yd, width, height, 2, fg, pBox,
                              0, 0);
          break;
        case 'v':
          DrawVerticalLine (frame, i, style, xd, yd, width, height, 1, fg, pBox,
                            0, 0);
          break;
        case 'l':
          DrawVerticalLine (frame, i, style, xd, yd, width, height, 0, fg, pBox,
                            0, 0);
          break;
        case 'r':
          DrawVerticalLine (frame, i, style, xd, yd, width, height, 2, fg, pBox,
                            0, 0);
          break;
        case '/':
          DrawSlash (frame, i, style, xd, yd, width, height, 0, fg);
          break;
        case '\\':
          DrawSlash (frame, i, style, xd, yd, width, height, 1, fg);
          break;
        case '>':
          DrawArrow (frame, i, style, xd, yd, width, height, 0, 0, fg);
          break;
        case 'E':
          DrawArrow (frame, i, style, xd, yd, width, height, 45, 0, fg);
          break;
        case '^':
          DrawArrow (frame, i, style, xd, yd, width, height, 90, 0, fg);
          break;
        case 'O':
          DrawArrow (frame, i, style, xd, yd, width, height, 135, 0, fg);
          break;
        case '<':
          DrawArrow (frame, i, style, xd, yd, width, height, 180, 0, fg);
          break;
        case 'o':
          DrawArrow (frame, i, style, xd, yd, width, height, 225, 0, fg);
          break;
        case 'V':
          DrawArrow (frame, i, style, xd, yd, width, height, 270, 0, fg);
          break;
        case 'e':
          DrawArrow (frame, i, style, xd, yd, width, height, 315, 0, fg);
          break;
	    
        case 'P':
          DrawRectangleFrame (frame, i, style, xd, yd, width, height,
                              fg, bg, pat);
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

      /* show the selection on the whole graphics */
      if (selected && !pAb->AbPresentationBox)
	{
	  if (pFrame->FrSelectOnePosition)
	    DisplayPointSelection (frame, pBox,
				   pFrame->FrSelectionBegin.VsIndBox, TRUE);
	  else
	    DisplayPointSelection (frame, pBox, 0,
				   pAb->AbSelected &&
				   TtaIsSelectionUnique ());
	}
    }
}

/*----------------------------------------------------------------------
  PolyTransform checks whether a polyline Box need to be transformed
  to fit in the current area.
  If the CHKR_LIMIT point matches the current geometry, no need
  to change anything, otherwise, all points are moved using
  Box-Width/Lim-X ratio horizontally and Box-Height/Lim-Y ratio vertically.
  ----------------------------------------------------------------------*/
/* static void PolyTransform (PtrBox pBox, int frame) */
/* { */
/*   float               xRatio, yRatio, pointIndex; */
/*   PtrTextBuffer       adbuff; */
/*   int                 i; */
/*   int                 j, val; */
/*   int                 width, height; */
/*   int                 zoom; */
  
/*   /\* box sizes have to be positive *\/ */
/*   width = pBox->BxW; */
/*   if (width < 0) */
/*     width = 0; */
/*   height = pBox->BxH; */
/*   if (height < 0) */
/*     height = 0; */
/*   zoom = ViewFrameTable[frame - 1].FrMagnification; */
/*   val = PixelValue (pBox->BxBuffer->BuPoints[0].XCoord, UnPixel, NULL, zoom); */
/*   /\* Compute ratio for axis X *\/ */
/*   if (val != width && pBox->BxBuffer->BuPoints[0].XCoord > 0) */
/*     { */
/*       val = LogicalValue (width, UnPixel, NULL, zoom); */
/*       pointIndex = (float) pBox->BxBuffer->BuPoints[0].XCoord / pBox->BxXRatio; */
/*       /\* save the new distortion ratio between box and abstract box *\/ */
/*       pBox->BxXRatio = (float) val / pointIndex; */
/*       /\* ratio applied to the box *\/ */
/*       xRatio = (float) val / (float) pBox->BxBuffer->BuPoints[0].XCoord; */
/*       pBox->BxBuffer->BuPoints[0].XCoord = val; */
/*     } */
/*   else */
/*     xRatio = 1.0; */
  
/*   /\* Compute ratio for axis Y *\/ */
/*   val = PixelValue (pBox->BxBuffer->BuPoints[0].YCoord, UnPixel, NULL, zoom); */
/*   if (val != height && pBox->BxBuffer->BuPoints[0].YCoord > 0) */
/*     { */
/*       val = LogicalValue (height, UnPixel, NULL, zoom); */
/*       pointIndex = (float) pBox->BxBuffer->BuPoints[0].YCoord / pBox->BxYRatio; */
/*       /\* save the new distortion ratio between box and abstract box *\/ */
/*       pBox->BxYRatio = (float) val / pointIndex; */
/*       /\* ratio applied to the box *\/ */
/*       yRatio = (float) val / (float) pBox->BxBuffer->BuPoints[0].YCoord; */
/*       pBox->BxBuffer->BuPoints[0].YCoord = val; */
/*     } */
/*   else */
/*     yRatio = 1.0; */
  
/*   if (xRatio != 1 || yRatio != 1) */
/*     { */
/*       j = 1; */
/*       adbuff = pBox->BxBuffer; */
/*       val = pBox->BxNChars; */
/*       for (i = 1; i < val; i++) */
/*         { */
/*           if (j >= adbuff->BuLength) */
/*             { */
/*               if (adbuff->BuNext != NULL) */
/*                 { */
/*                   /\* Next buffer *\/ */
/*                   adbuff = adbuff->BuNext; */
/*                   j = 0; */
/*                 } */
/*             } */
/*           adbuff->BuPoints[j].XCoord = (int) ((float) adbuff->BuPoints[j].XCoord * xRatio); */
/*           adbuff->BuPoints[j].YCoord = (int) ((float) adbuff->BuPoints[j].YCoord * yRatio); */
/*           j++; */
/*         } */
/*     } */
/* } */


/*----------------------------------------------------------------------
  DisplayPolyLine displays a polyline.
  The parameter selected is TRUE when the polyline is selected.
  t, b, l, and r give top, bottom, left and right extra margins.
  ----------------------------------------------------------------------*/
void DisplayPolyLine (PtrBox pBox, int frame, ThotBool selected,
                      int t, int b, int l, int r)
{
  PtrAbstractBox      pAb;
  ViewFrame          *pFrame;
  int                 i, xd, yd;
  int                 fg, bg;
  int                 pat;
  int                 style, arrow, mode, width;

  /* If no point is defined, no need to draw it */
  if (pBox->BxBuffer == NULL || pBox->BxNChars <= 1)
    return;
  /* Transform the polyline if the box size has changed */
  //PolyTransform (pBox, frame);
  pAb = pBox->BxAbstractBox;
  pFrame = &ViewFrameTable[frame - 1];
  if (pAb->AbVisibility >= pFrame->FrVisibility)
    {
      bg = pAb->AbBackground;
      pat = pAb->AbFillPattern;
      fg = pAb->AbForeground;
      xd = pBox->BxXOrg + pBox->BxLMargin + l + pBox->BxLBorder +
        pBox->BxLPadding - pFrame->FrXOrg;
      yd = pBox->BxYOrg + pBox->BxTMargin + t + pBox->BxTBorder +
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

      // set the current fill mode
      if (pAb->AbFillRule == 'e')
        mode = 1;
      else
        mode = 0;

      switch (pAb->AbPolyLineShape)
        {
        case 'S':	/* Segments */
        case 'g':	/* Line */
        case 'U':	/* Segments forward arrow */
        case 'N':	/* Segments backward arrow */
        case 'M':	/* Segments arrows on both directions */
        case 'w':	/* Segments (2 points) */
        case 'x':	/* Segments (2 points) forward arrow */
        case 'y':	/* Segments (2 points) backward arrow */
        case 'z':	/* Segments (2 points) arrows on both directions */
          if (pAb->AbPolyLineShape == 'S' || pAb->AbPolyLineShape == 'w' ||
              pAb->AbPolyLineShape == 'g')
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
                                                             pBox->BxNChars);
          DrawCurve (frame, i, style, xd, yd, pBox->BxBuffer,
                     pBox->BxNChars, fg, arrow,
                     (C_points *) pBox->BxPictInfo);
          break;
        case 'p':	/* polygon */
          if (SVGCreating)
            // draw only lines
            DrawSegments (frame, i, style, xd, yd, pBox->BxBuffer,
                          pBox->BxNChars, fg, 0, bg, pat);
          else
            DrawPolygon (frame, i, style, xd, yd, pBox->BxBuffer,
                         pBox->BxNChars, fg, bg, pat, mode);
          break;
        case 's':	/* closed spline */
          /* compute control points */
          if (pBox->BxPictInfo == NULL)
            pBox->BxPictInfo = (int *) ComputeControlPoints (pBox->BxBuffer, 
                                                             pBox->BxNChars);
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
                                   pFrame->FrSelectionBegin.VsIndBox, TRUE);
          else if (pBox->BxNChars > 1)
            DisplayPointSelection (frame, pBox, 0,
                                   pAb->AbSelected && TtaIsSelectionUnique ());
        }
    }
}

/*----------------------------------------------------------------------
  DisplayPath displays a path.
  The parameter selected is TRUE when the polyline is selected.
  t, b, l, and r give top, bottom, left and right extra margins.
  ----------------------------------------------------------------------*/
void DisplayPath (PtrBox pBox, int frame, ThotBool selected,
                  int t, int b, int l, int r)
{
  PtrAbstractBox      pAb;
  ViewFrame          *pFrame;
  int                 i, xd, yd;
  int                 fg, bg, pat;
  int                 style, mode, width;

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
      xd = pBox->BxXOrg + pBox->BxLMargin  + l + pBox->BxLBorder +
        pBox->BxLPadding - pFrame->FrXOrg;
      yd = pBox->BxYOrg + pBox->BxTMargin + t + pBox->BxTBorder +
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

      // set the current fill mode
      if (pAb->AbFillRule == 'e')
        mode = 1;
      else
        mode = 0;
      DrawPath (frame, i, style, xd, yd, pBox->BxFirstPathSeg, fg,
                bg, pat, mode);

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
      
      /* show the selection on the whole path */
      if (selected)
        {
          if (pFrame->FrSelectOnePosition)
            DisplayPointSelection (frame, pBox,
                                   pFrame->FrSelectionBegin.VsIndBox, TRUE);
          else
            DisplayPointSelection (frame, pBox, 0,
                                   pAb->AbSelected && TtaIsSelectionUnique ());
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
  TranslateChars replaces a character by a visual equivalent character.
  ----------------------------------------------------------------------*/
static int TranslateChars (int c, ThotBool showSpecial)
{
  if (c == START_ENTITY)
    /* display '&' */
    return 0x26;
  else if (!Printing && showSpecial)
    /* show special characters */
    switch (c)
      {
      case TAB:
        return SHOWN_TAB;
      case BREAK_LINE:
        return SHOWN_BREAK_LINE;
      case THIN_SPACE:
        return SHOWN_THIN_SPACE;
      case FOUR_PER_EM:
        return SHOWN_HALF_EM;
      case UNBREAKABLE_SPACE:
        return SHOWN_UNBREAKABLE_SPACE;
      default:
        return c;
      }
  else
    return c;
}

/*----------------------------------------------------------------------
  DisplayJustifiedText display the content of a Text box tweaking
  the space sizes to ajust line length to the size of the frame.
  Remaining pixel space (BxNPixels) is equally dispatched 
  on all spaces in the line.
  t, b, l, and r give top, bottom, left and right extra margins.
  ----------------------------------------------------------------------*/
static void DisplayJustifiedText (PtrBox pBox, PtrBox mbox, int frame,
                                  ThotBool selected,
                                  int t, int b, int l, int r)
{
  PtrTextBuffer       adbuff;
  ViewFrame          *pFrame;
  PtrBox              nbox;
  PtrAbstractBox      pAb;
  SpecFont            font;
#ifdef _GL
  PtrLine             pLine;
#endif /* _GL */
  ThotFont            prevfont = NULL;
  ThotFont            nextfont = NULL;
  CHAR_T              c, transc;
  CHAR_T              prevChar, nextChar;
  wchar_t            *wbuffer = NULL;
  unsigned char      *buffer = NULL;
  char                script;
  int                 restbl, val;
  int                 newbl, lg;
  int                 charleft;
  int                 buffleft;
  int                 indbuff, bl;
  int                 indmax, psW, w;
  int                 nbcar, x, y, y1;
  int                 lgspace, whitespace;
  int                 fg, bg, fgbox, bgbox;
  int                 width, xpos, underline_width;
  int                 left, right, variant;
  ThotBool            shadow;
  ThotBool            blockbegin, withinSel = FALSE;
  ThotBool            hyphen, rtl, showSpecial = FALSE;
#ifdef _GL
  int                 texture_id, showtab_id = 0, underline_id = 0;
#endif /* _GL */

  indmax = 0;
  buffleft = 0;
  adbuff = NULL;
  indbuff = 0;
  restbl = 0;
  pAb = pBox->BxAbstractBox;
  if (pAb->AbPrevious &&
      (pAb->AbPrevious->AbFloat != 'N' ||
       (pAb->AbPrevious->AbBox && pAb->AbPrevious->AbBox->BxType == BoFloatGhost)) &&
      pBox->BxIndChar == 0 && pAb->AbText && pAb->AbText->BuContent[0] == SPACE)
    {
      /* ignore the space that follows a floated box */
      indbuff++;
      if (pBox->BxNChars <= 1)
        return;
    }
  if (pBox->BxNChars < 0)
    return;
#ifdef _GL
  texture_id = SetTextureScale (IsBoxDeformed(pBox));
#endif /* _GL */
  if (pAb->AbElement && pAb->AbElement->ElStructSchema &&
      !strcmp (pAb->AbElement->ElStructSchema->SsName, "TextFile"))
    /* only for TextFile documents */
    TtaGetEnvBoolean ("SHOW_SPECIAL_CHARS", &showSpecial);
  variant = pAb->AbFontVariant;
  script = pBox->BxScript;
  /* is it a box with a right-to-left writing? */
  if (pAb->AbUnicodeBidi == 'O')
    rtl = (pAb->AbDirection == 'R');
  else
    rtl = (script == 'A' || script == 'H');
  font = pBox->BxFont;
  /* do we have to display stars instead of characters? */
  shadow = pAb->AbBox->BxShadow;
  /* Is this box the first of a block of text? */
  if (mbox == pBox)
    blockbegin = TRUE;
  else if (mbox->BxFirstLine == NULL ||
           (mbox->BxType != BoBlock &&
            mbox->BxType != BoFloatBlock && mbox->BxType != BoCellBlock))
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
      bgbox = pAb->AbForeground;
      if (pAb->AbGradientBackground)
	/* this chunk of text should be painted with a gradient */
	/* this is not yet implemented. Just paint it in black */
	fgbox = 1;
      else
	fgbox = pAb->AbBackground;
    }
  else
    {
      fgbox = pAb->AbForeground;
      bgbox = pAb->AbBackground;
    }
  bg = bgbox;
  fg = fgbox;
  pFrame = &ViewFrameTable[frame - 1];
  if (pAb->AbVisibility >= pFrame->FrVisibility)
    {
      /* Initialization */
      x = pBox->BxXOrg + l + pBox->BxLMargin + pBox->BxLBorder +
        pBox->BxLPadding - pFrame->FrXOrg;
      y = pBox->BxYOrg + pBox->BxTMargin + pBox->BxTBorder +
        pBox->BxTPadding - pFrame->FrYOrg;
      /* the base line already includes top margins, top borders, etc. */
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
      underline_width = width;
      whitespace = BoxCharacterWidth (SPACE, 1, font);
      lgspace = pBox->BxSpaceWidth;
      if (lgspace == 0)
        lgspace = whitespace;
      
      /* Do we need to draw a background */
      if (pAb->AbPresentationBox && pAb->AbFillPattern && bg >= 0)
        DrawRectangle (frame, 0, 0,
                       x - pBox->BxLPadding, y - pBox->BxTPadding,
                       width + pBox->BxLPadding + pBox->BxRPadding,
                       pBox->BxH + pBox->BxTPadding + pBox->BxBPadding,
                       0, bg, 2);

      /* locate the first character */
      LocateFirstChar (pBox, rtl, &adbuff, &indbuff);
      left = 0; /* start position of the selection */
      right = 0; /* end position of the selection */
      /* check if the box is selected */
      if (pBox == pFrame->FrSelectionBegin.VsBox ||
          pBox == pFrame->FrSelectionEnd.VsBox)
        {
          selected = TRUE;
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
              DisplayStringSelection (frame, left, right, t, pBox);
              /* extra margins are already taken into account */
              left += x - l;
              right += x - l;
            }
        }
      else if (selected &&
               (pFrame->FrSelectionBegin.VsBox == NULL ||
                pAb != pFrame->FrSelectionBegin.VsBox->BxAbstractBox ) &&
               (pFrame->FrSelectionEnd.VsBox == NULL ||
                pAb != pFrame->FrSelectionEnd.VsBox->BxAbstractBox))
        {
          /* the whole box is selected */
          DisplayBgBoxSelection (frame, pBox);
          right = x + pBox->BxWidth;
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
                {
                  /* it's within the current selection */
                  selected = TRUE;
                  DisplayBgBoxSelection (frame, pBox);
                  right = x + pBox->BxWidth;
                }
            }
          else if (pFrame->FrSelectionEnd.VsBox &&
                   pAb == pFrame->FrSelectionEnd.VsBox->BxAbstractBox)
            {
              nbox = pBox->BxNexChild;
              while (nbox && nbox != pFrame->FrSelectionEnd.VsBox)
                nbox = nbox->BxNexChild;
              if (nbox == pFrame->FrSelectionEnd.VsBox)
                {
                  /* it's within the current selection */
                  selected = TRUE;
                  DisplayBgBoxSelection (frame, pBox);
                  right = x + pBox->BxWidth;
                }
            }
        }

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
              if ( c == 0x28 && script == 'A') 
                c = 0x29;
              else if ( c == 0x29 && script == 'A') c = 0x28;
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
                val = GetFontAndIndexFromSpec (c, font, variant, &nextfont);
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
        }

      /* allocate a buffer to store converted characters */
#ifdef _GL
      if (script == 'Z' || script == 'A' ||
          !Printing || GL_TransText ())
#else /*_GL*/
        if (script == 'Z' || script == 'A')
#endif /*_GL*/
          wbuffer = (wchar_t *)TtaGetMemory ((pBox->BxNChars + 1) * sizeof(wchar_t));
        else
          buffer = (unsigned char *)TtaGetMemory (pBox->BxNChars + 1);
      nbcar = 0;
      psW = 0; /* width used to justify a PostScript string */
      xpos = x; /* position of the new displayed character */
      while (charleft > 0 && adbuff)
        {
          /* handle each char in the buffer */
          while ((rtl && indbuff >= indmax) || (!rtl && indbuff <= indmax))
            {
              /* get the current character */
              if (shadow)
                c = '*';
              else
                c = adbuff->BuContent[indbuff];

              /* check if a translation is requested */
              transc = TranslateChars (c, showSpecial);
              /* get the font index into val */
              if (c == 0x28 && script == 'A') 
                c = 0x29;
              else if (c == 0x29 && script == 'A')
                c = 0x28;
              if (c >= 0x0600 && c <= 0x06B0) /* arabic char */
                {
                  nextChar = Previous_Char (&adbuff, &indbuff);
                  prevChar = Next_Char (&adbuff, &indbuff);
                  if (nextChar >= 0x064B && nextChar <= 0x0655)
                    {
                      if (indbuff < adbuff->BuLength - 2)
                        nextChar = adbuff->BuContent[indbuff + 2];
                      else 
                        nextChar = 0x0020;
                    }
                  if (prevChar >= 0x064B && prevChar <= 0x0655)
                    {
                      if (indbuff > 1)
                        prevChar = adbuff->BuContent[indbuff -2];
                      else
                        prevChar = 0x0020;
                    }  
                  val = GetArabFontAndIndex (transc, prevChar, nextChar, font, &nextfont);
                }
              else
                val = GetFontAndIndexFromSpec (transc, font, variant, &nextfont);

              if (val == INVISIBLE_CHAR || c == ZERO_SPACE ||
                  c == EOL || c == BREAK_LINE)
                /* do nothing */;
              else if (!Printing && selected &&
                       (right > left + 2 || pBox->BxW <= 2) &&
                       ((!withinSel && xpos >= left && xpos <= right) ||
                        (withinSel && xpos >= right)))
                {
                  if (nbcar > 0)
                    {
                      /* enter or leave a selected region */
#ifdef _GL
                      if (script == 'Z' || script == 'A' ||
                          !Printing || GL_TransText ())
#else /*_GL*/
                        if (script == 'Z' || script == 'A')
#endif /*_GL*/
                          w = WDrawString (wbuffer, nbcar, frame, x, y1, prevfont,
                                           0, bl, 0, blockbegin, fg);
                        else
                          w = DrawString (buffer, nbcar, frame, x, y1, prevfont,
                                          0, bl, 0, blockbegin, fg);
                      x += w;
                      psW += w;
                      bl = 0; /* all previous spaces are managed */
                    }
                  withinSel = !withinSel;
                  if (withinSel)
                    fg = FgSelColor;
                  else
                    fg = fgbox;
                  nbcar = 0;
                  prevfont = nextfont;
                  xpos = x;
                }

              if (nextfont == NULL && val == UNDISPLAYED_UNICODE)
                {
                  /* display previous chars handled */
                  if (nbcar > 0)
                    {
                      width -= psW;
                      if (psW == 0)
                        /* justify only the current string */
                        psW = 1;
                      else
                        /* there is a previous string */
                        psW = -psW;
#ifdef _GL
                      if (script == 'Z' || script == 'A' ||
                          !Printing || GL_TransText ())
#else /*_GL*/
                        if (script == 'Z' || script == 'A')
#endif /*_GL*/
                          w = WDrawString (wbuffer, nbcar, frame, x, y1, prevfont,
                                           psW, bl, x, blockbegin, fg);
                        else
                          w = DrawString (buffer, nbcar, frame, x, y1, prevfont,
                                          psW, bl, x, blockbegin, fg);
                      bl = 0; /* all previous spaces are managed */
                    }
                  else
                    w = 0;
                  psW = 0; /* the ps justification is done */
                  nbcar = 0;
                  prevfont = nextfont;
                  x += w;
                  width -= w;
                  DrawRectangle (frame, 1, 5, x, y, 6, pBox->BxH - 1, fg, 0, 0);
                  x += 6;
                  if (pBox->BxUnderline)
                    DisplayUnderline (frame, x, y+t, pBox->BxH, pBox->BxUnderline, 7, fg);
                  width -= 6;
                  xpos = x;
                }
              else
                {
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
                      if (prevfont == NULL)
                        prevfont = nextfont;
#ifdef _WINGUI
                      if (nbcar > 0)
#else /* WINDOWS */
			/* PostScript should register the start position */
                        if (nbcar > 0 || Printing)
#endif /* WINDOWS */
                          {
#ifdef _GL
                            if (script == 'Z' || script == 'A' ||
                                !Printing || GL_TransText ())
#else /*_GL*/
                              if (script == 'Z' || script == 'A')
#endif /*_GL*/
                                w = WDrawString (wbuffer, nbcar, frame, x, y1, prevfont,  
                                                 0, bl, 0, blockbegin, fg);
                              else
                                w = DrawString (buffer, nbcar, frame, x, y1, prevfont,  
                                                0, bl, 0, blockbegin, fg);
                            psW += w; /* the ps justification is done */
                            x += w;
                            bl = 0; /* all previous spaces are managed */
                          }
                      if (transc == SPACE)
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
                      if (!Printing && transc == SHOWN_TAB)
                        {
#ifdef _WX
                          if (showtab_id)
                            StopTextureScale (showtab_id);
                          showtab_id = SetTextureScale (IsBoxDeformed(pBox));
                          DrawHorizontalLine (frame, 1, 5, x+2, y, lg-2, pBox->BxH,
                                              2, BgSelColor, pBox, 0, 0);
#else /* _WX */
                        DrawChar ((CHAR_T)(char)val, frame, x, y1, nextfont, BgSelColor);
#endif /* _WX */
                        }
                      else if (transc == SHOWN_SPACE ||
                          transc == SHOWN_UNBREAKABLE_SPACE || transc == SHOWN_HALF_EM)
                        /* a new space is handled */
                        DrawChar ((CHAR_T)(char)val, frame, x, y1, nextfont, BgSelColor);
                      else
                        bl++;
                      nbcar = 0;
                      x += lg;
                      psW +=lg;
                      xpos = x;
                    }
                  else
                    {
                      if (prevfont != nextfont)
                        {
                          /* display previous chars handled */
                          if (nbcar > 0)
                            {
                              width -= psW;
                              if (psW == 0)
                                /* justify only the current string */
                                psW = 1;
                              else
                                /* there is a previous string */
                                psW = -psW;
#ifdef _GL
                              if (script == 'Z' || script == 'A' ||
                                  !Printing || GL_TransText ())
#else /*_GL*/
                                if (script == 'Z' || script == 'A')
#endif /*_GL*/
                                  w = WDrawString (wbuffer, nbcar, frame, x, y1,
                                                   prevfont, psW, bl, 0, blockbegin,
                                                   fg);
                                else
                                  {
                                    if ( prevfont == NULL)
                                      prevfont = nextfont;
                                    w = DrawString (buffer, nbcar, frame, x, y1,
                                                    prevfont, psW, bl, 0, blockbegin,
                                                    fg);
                                  }
                              psW = 0; /* the ps justification is done */
                              width -= w;
                              x += w;
                              bl = 0; /* all previous spaces are managed */
                            }
                          nbcar = 0;
                          prevfont = nextfont;
                          xpos = x;
                        }
#ifdef _GL
                      if (script == 'Z' || script == 'A' ||
                          !Printing || GL_TransText ())
#else /*_GL*/
                        if (script == 'Z' || script == 'A')
#endif /*_GL*/
                          /* add the new char */
                          wbuffer[nbcar++] = val;
                        else
                          /* add the new char */
                          buffer[nbcar++] = val;
                      xpos += CharacterWidth (val, nextfont);
                    }
                }
              /* Skip to next char */
              if (rtl)
                indbuff--;
              else
                indbuff++;
            }

          /* Draw previous chars in the buffer */
          if (buffleft > 0)
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

          if (charleft <= 0 && prevfont)
            {
              /* Draw the end of the box.
                 Call the function in any case to let Postscript justify the
                 text of the box.
              */
#ifdef _WINGUI
              if (nbcar > 0)
#else /* WINDOWS */
                /* PostScript can now justify the displayed string */
                if (nbcar > 0 || Printing)
#endif /* WINDOWS */
                  {
#ifdef _GL
                    if (script == 'Z' || script == 'A' ||
                        !Printing || GL_TransText ())
#else /*_GL*/
                      if (script == 'Z' || script == 'A')
#endif /*_GL*/
                        x += WDrawString (wbuffer, nbcar, frame, x, y1, prevfont, width,
                                          bl, hyphen, blockbegin, fg);
                      else
                        x += DrawString (buffer, nbcar, frame, x, y1, prevfont, width,
                                         bl, hyphen, blockbegin, fg);
                  }
              if (pBox->BxUnderline)
                {
#ifdef _GL
                  if (underline_id)
                    StopTextureScale (underline_id);
                  underline_id = SetTextureScale (IsBoxDeformed(pBox));
                  DisplayUnderline (frame, x, y+t, pBox->BxH, pBox->BxUnderline,
                                    underline_width, fg);
#else /* _GL */
                  DisplayUnderline (frame, x, y+t, pBox->BxH, pBox->BxUnderline,
                                    underline_width, fg);
#endif /* _GL */
                }
              nbcar = 0;
            }
        } 
#ifdef _GL

      // generate the break-line character
      if (showSpecial && mbox && mbox->BxType == BoBlock)
        {
          pLine = mbox->BxFirstLine;
          while (pLine && pLine != mbox->BxLastLine)
            {
              if (pBox == pLine->LiLastBox || pBox == pLine->LiLastPiece)
                {
                  y = pBox->BxYOrg + pBox->BxHorizRef - pFrame->FrYOrg;
                  nextfont = (ThotFont)LoadStixFont (1, 12);
                  DrawChar ((CHAR_T)0x40, frame, x, y, nextfont, 1);
                  pLine = NULL;
                }
              else
                pLine = pLine->LiNext;
            }
        }
      if (showtab_id)
        StopTextureScale (showtab_id);
      if (underline_id)
        StopTextureScale (underline_id);
      StopTextureScale (texture_id);
#endif /* _GL */

      /* Should the end of the line be filled with dots */
      if (pBox->BxEndOfBloc > 0)
        {
          /* fill the end of the line with dots */
          x = pBox->BxXOrg + pBox->BxLMargin + l + pBox->BxLBorder +
            pBox->BxLPadding;
          y = pBox->BxYOrg + pBox->BxHorizRef - pFrame->FrYOrg;
          DrawPoints (frame, pBox->BxXOrg + width - pFrame->FrXOrg, y,
                      pBox->BxEndOfBloc, fg);
        }
      /* display a caret if needed */
      if (selected && right == left + 2 && pFrame->FrSelectOnePosition)
        DisplayStringSelection (frame, left, right, t, pBox);
      TtaFreeMemory (wbuffer);
      TtaFreeMemory (buffer);
    }
}


/*----------------------------------------------------------------------
  DisplayBorders displays the box borders.
  The parameter pForm points the box that generates the border or fill.
  Parameters x, y, w, h give the clipping region.
  et, eb, el, and er give top, bottom, left and right extra margins.
  Parameters bt, bb, bl, br give the top, bottom, left, and right width
  of borders.
  ----------------------------------------------------------------------*/
void DisplayBorders (PtrBox box, PtrAbstractBox pFrom, int frame,
                     int x, int y, int w, int h,
                     int et, int eb, int el, int er,
                     int bt, int bb, int bl, int br) 
{
  PtrBox              from;
  int                 color;
  int                 t, b, l, r, d;
  int                 xFrame, yFrame, height, width;

  if (pFrom == NULL || pFrom->AbBox == NULL ||
      pFrom->AbBox->BxType == BoCell)
    /* cell borders are displayed by a presentation box */
    return;
  from = pFrom->AbBox;
  if (from->BxType == BoTable && pFrom->AbElement &&
      pFrom->AbElement->ElStructSchema &&
      !strcmp (pFrom->AbElement->ElStructSchema->SsName, "HTML"))
    return;
  /* position in the frame */
#ifdef _GL
  if (box->BxBoundinBoxComputed)
    {
      xFrame = box->BxClipX;
      yFrame = box->BxClipY;
      width = box->BxClipW;
      height = box->BxClipH;
    }
  else
#endif /* _GL */
    {
      xFrame = box->BxXOrg - ViewFrameTable[frame - 1].FrXOrg;
      yFrame = box->BxYOrg - ViewFrameTable[frame - 1].FrYOrg;
      width = box->BxWidth;
      height = box->BxHeight;
    }
  if (box->BxLMargin < 0)
    {
      el -= box->BxLMargin;
      xFrame += box->BxLMargin;
    }
  if (box->BxTMargin < 0)
    {
      et -= box->BxTMargin;
      yFrame += box->BxTMargin;
    }
  if (x < xFrame)
    {
      width = width + xFrame - x;
      xFrame = x;
    }
  if (x + w > xFrame + width)
    width = x + w - xFrame;

  /* part of the top, left, bottom and right border which are visible */
  t = yFrame + et + from->BxTBorder - y;
  if (t > from->BxTBorder)
    t = from->BxTBorder;
  else if (t < 0)
    t = 0;

  l = xFrame + el + from->BxLBorder - x;
  if (l > from->BxLBorder)
    l = from->BxLBorder;
  else if (l < 0)
    l = 0;

  if (et < 0)
    yFrame += et;
  if (y + h < yFrame + height)
    b =  eb + from->BxBBorder;
  else
    b = y + h - yFrame - height + eb + from->BxBBorder;
  if (b > from->BxBBorder)
    b = from->BxBBorder;
  else if (b < 0)
    b = 0;

  if (er < 0)
    er = 0;
  if (el < 0)
    xFrame += el;
  if ( x + w < xFrame + width)
    r = er + from->BxRBorder;
  else
    r = x + w + 1 - xFrame - width + er + from->BxRBorder;
  if (r > from->BxRBorder)
    r = from->BxRBorder;
  else if (r < 0)
    r = 0;

  if (from->BxTBorder && pFrom->AbTopStyle > 2 &&
      pFrom->AbTopBColor != -2 && t > 0 && bt)
    {
      /* the top border is visible */
      if (pFrom->AbTopBColor == -1)
        color = pFrom->AbForeground;
      else
        color = pFrom->AbTopBColor;
      if (from->BxTBorder)
        d = 0;
      else
        d = t/from->BxTBorder;
      DrawHorizontalLine (frame, t, pFrom->AbTopStyle, x, y,
                          w, t, 0, color, box,
                          l*d, r*d);
    }
  if (from->BxLBorder && pFrom->AbLeftStyle > 2 &&
      pFrom->AbLeftBColor != -2 && l > 0 && bl)
    {
      /* the left border is visible */
      if (pFrom->AbLeftBColor == -1)
        color = pFrom->AbForeground;
      else
        color = pFrom->AbLeftBColor;
      if (from->BxLBorder)
        d = 0;
      else
        d = l/from->BxLBorder;
      DrawVerticalLine (frame, l, pFrom->AbLeftStyle, x, y,
                        l, h, 0, color, box,
                        t*d, b*d);
    }
  if (from->BxBBorder && pFrom->AbBottomStyle > 2 &&
      pFrom->AbBottomBColor != -2 && b > 0 && bb)
    {
      /* the bottom border is visible */
      if (pFrom->AbBottomBColor == -1)
        color = pFrom->AbForeground;
      else
        color = pFrom->AbBottomBColor;
      if (from->BxBBorder)
        d = 0;
      else
        d = b/from->BxBBorder;
      DrawHorizontalLine (frame, b, pFrom->AbBottomStyle,
                          x, yFrame + height - eb - from->BxBBorder,
                          w, b, 2, color, box,
                          l*d, r*d);
    }
  if (from->BxRBorder && pFrom->AbRightStyle > 2 &&
      pFrom->AbRightBColor != -2 && r > 0 && br)
    {
      /* the right border is visible */
      if (pFrom->AbRightBColor == -1)
        color = pFrom->AbForeground;
      else
        color = pFrom->AbRightBColor;
      if (from->BxRBorder)
        d = 0;
      else
        d = t/from->BxRBorder;
      DrawVerticalLine (frame, r, pFrom->AbRightStyle,
                        xFrame + width - er - from->BxRBorder, y,
                        r, h, 2, color, box,
                        t*d, b*d);
    }
}

/*----------------------------------------------------------------------
  DisplayBox display a box depending on its content.
  pFlow points to the displayed flow or NULL when it's the main flow.
  selected is TRUE when an eclosing box or the box itself is selected.
  ----------------------------------------------------------------------*/
void DisplayBox (PtrBox box, int frame, int xmin, int xmax, int ymin,
                 int ymax, PtrFlow pFlow, ThotBool selected)
{
  ViewFrame         *pFrame;
  PtrBox             mbox;
  PtrAbstractBox     pAb;
  int                x, y, shiftx, shifty;
  int                xd, yd, width, height;
  int                t, b, l, r;
  ThotBool           selfsel;
#ifdef _GL
  ThotBool           isOpenList = FALSE;
#endif  /* _GL */

  if (pFlow)
    {
      /* shift due to relative positioning */
      shiftx = pFlow->FlXStart;
      shifty = pFlow->FlYStart;
    }
  else
    shiftx = shifty = 0;
  pFrame = &ViewFrameTable[frame - 1];
  pAb = box->BxAbstractBox;
  GetExtraMargins (box, frame, FALSE, &t, &b, &l, &r);
  x = ViewFrameTable[frame - 1].FrXOrg;
  y = ViewFrameTable[frame - 1].FrYOrg;
  xd = box->BxXOrg + box->BxLMargin + l + shiftx;
  yd = box->BxYOrg + box->BxTMargin + t + shifty;
  width = box->BxWidth - l - r;
  if (box->BxLMargin > 0)
    width -= box->BxLMargin;
  if (box->BxRMargin > 0)
    width -= box->BxRMargin;
  height = box->BxHeight - t - b;
  if (box->BxTMargin > 0)
    height -= box->BxTMargin;
  if (box->BxBMargin > 0)
    height -= box->BxBMargin;
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
#ifdef _GL
      InitPrintBox ();
#endif /* _GL */
    }
  /* is the box selected? */
  selfsel = pAb->AbSelected;
  /* Search for the enclosing box */
  if (pAb->AbEnclosing == NULL)
    mbox = box;
  else
    {
      /* get the visible enclosing box */
      mbox = pAb->AbEnclosing->AbBox;
      if (mbox->BxType == BoGhost ||
          mbox->BxType == BoStructGhost ||
          mbox->BxType == BoFloatGhost)
        {
          selfsel = selfsel || mbox->BxAbstractBox->AbSelected;
          while (mbox->BxAbstractBox->AbEnclosing &&
                 (mbox->BxType == BoGhost ||
                  mbox->BxType == BoStructGhost ||
                  mbox->BxType == BoFloatGhost))
            {
              mbox = mbox->BxAbstractBox->AbEnclosing->AbBox;
              selfsel = selfsel ||
                (mbox->BxAbstractBox->AbSelected &&
                 (mbox->BxType == BoGhost ||
                  mbox->BxType == BoStructGhost ||
                  mbox->BxType == BoFloatGhost));
            }
        }
      else if (pAb->AbLeafType == LtPicture)
        // display the selection at the image level
        selfsel = ((pAb->AbSelected || pAb->AbEnclosing->AbSelected) &&
                   !pAb->AbPresentationBox &&
                   (pFrame->FrSelectOnePosition || TtaIsSelectionUnique ()));
    }
  
#ifdef _GL 
  /*does box need to be recomputed in a new display list*/
  if (FrameTable[frame].FrView == 1 && !Printing)
    {
      /* box->VisibleModification = TRUE; */
      if ((pAb->AbLeafType == LtPolyLine || pAb->AbLeafType == LtPath) &&
          !selected)
        {
#ifdef _WX
          wxASSERT_MSG( !box->DisplayList ||
                        glIsList(box->DisplayList),
                        _T("GLBUG - DisplayBox : glIsList returns false"));
#endif /* _WX */
          if (!box->VisibleModification &&
              box->DisplayList && glIsList (box->DisplayList))
            {
              glCallList (box->DisplayList);
              return;
            }
          else if (box->VisibleModification || GL_NotInFeedbackMode ())
            {
              if (glIsList (box->DisplayList))
                glDeleteLists (box->DisplayList, 1);
              box->DisplayList = glGenLists (1);
              glNewList (box->DisplayList, GL_COMPILE_AND_EXECUTE);
              isOpenList = TRUE;
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
          if (DisplayGradient (pAb, box, frame, selfsel, t, b, l, r))
            return;
        }
    }

  /* add shift due to relative positioning */
  box->BxClipX += shiftx;
  box->BxClipY += shifty;
#endif /*_GL*/
  box->BxXOrg += shiftx;
  box->BxYOrg += shifty;
  
  if (pAb->AbVolume == 0 ||
      (pAb->AbLeafType == LtPolyLine && box->BxNChars == 1))
    {
      /* Empty */
      selfsel = (box == pFrame->FrSelectionBegin.VsBox &&
                 box == pFrame->FrSelectionEnd.VsBox);
      
      if (pAb->AbLeafType == LtSymbol)
        DisplayEmptyBox (box, frame, selfsel, t, b, l, r);
      else if (pAb->AbLeafType != LtPolyLine &&
               pAb->AbLeafType != LtGraphics &&
               pAb->AbLeafType != LtPath)
        {
          if (selfsel)
            DisplayStringSelection (frame, 0, box->BxW, t, box);
        }
    }
  else if (pAb->AbLeafType == LtText)
    /* Display a Text box */
    DisplayJustifiedText (box, mbox, frame, selected, t, b, l, r);
  else if (box->BxType == BoPicture || pAb->AbLeafType == LtPicture)
    /* Picture */
    DisplayImage (box, frame, xmin, xmax, ymin, ymax, selfsel,
                  t, b, l, r);
  else if (pAb->AbLeafType == LtSymbol)
    /* Symbol */
    if (pAb->AbShape == EOS)
      DisplayEmptyBox (box, frame, selected, t, b, l, r);
    else
      DisplaySymbol (box, frame, selected, t, b, l, r);
  else if (pAb->AbLeafType == LtGraphics)
    /* Graphics */
    if (pAb->AbShape == EOS)
      DisplayEmptyBox (box, frame, selfsel, t, b, l, r);
    else
      DisplayGraph (box, frame, selfsel, t, b, l, r);
  else if (pAb->AbLeafType == LtPolyLine)
    /* Polyline */
    DisplayPolyLine (box, frame, selfsel, t, b, l, r);
  else if (pAb->AbLeafType == LtPath)
    /* Path */
    DisplayPath (box, frame, selfsel, t, b, l, r);
  
  
  /* then display borders */
  if (box->BxDisplay &&
      yd + height >= ymin && yd <= ymax &&
      xd + width >= xmin && xd <= xmax)
    {
      l += box->BxLMargin;
      b += box->BxBMargin;
      t += box->BxTMargin;
      r += box->BxRMargin;
      DisplayBorders (box, pAb, frame, xd - x, yd - y, width, height, t, b, l, r,
                      box->BxTBorder, box->BxBBorder, box->BxLBorder, box->BxRBorder);
    }
#ifdef _GL
  if (Printing)
    FinishPrintBox ();
  else if (FrameTable[frame].FrView == 1)
    {
      GL_SetFillOpacity (1000);
      GL_SetStrokeOpacity (1000);
      box->VisibleModification = FALSE;
      if (isOpenList)
        glEndList ();
    }

  /* remove shift due to relative positioning */
  box->BxClipX -= shiftx;
  box->BxClipY -= shifty;
#endif /*_GL*/
  box->BxXOrg -= shiftx;
  box->BxYOrg -= shifty;
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
                         SpecFont fontset, ThotFont *font)
{
  int    i, j, k;
 
  *font = NULL;
  LoadingArabicFont (fontset, font);
#ifdef _GTK
  if (*font == fontset->Font_1)
    /* the arabic font was not found:
       avoid to select an invalid position in this font */
    return UNDISPLAYED_UNICODE;
#endif /* _GTK */
  if (c == 0x061F) 
    return 0x061F ;
  if (c == 0x060C) 
    return 0x060C ;
  if (c == 0x061B) 
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
