/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2004
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
/*
 * Handles Stix fonts for MathML 
 * 
 * Author: P. Cheyrou-lagreze (INRIA)
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "thot_gui.h"
#include "ustring.h"
#include "math.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "frame.h"
#include "appdialogue.h"
#include "registry.h"

#ifdef _GL
  #include "openglfont.h"
  #include "glwindowdisplay.h"
#endif /*_GL*/

#ifdef _WINGUI
  #include "windowdisplay_f.h"
#endif /*_WINGUI*/

#if defined(_GTK) || defined(_WX)
  #include "xwindowdisplay_f.h"
#endif /* #if defined(_GTK) || defined(_WX) */

#include "font_f.h"
#include "stix.h"

#define LOW_HEIGHT 2
#define MID_HEIGHT 3
#define HIGH_HEIGHT 5


#ifdef _GL
  #define LOW_CHAR 25
  #define MID_CHAR 45
  #define HIGH_CHAR 45
#else /*_GL*/
  #define LOW_CHAR 25
  #define MID_CHAR 45
  #define HIGH_CHAR 45
#endif /*_GL*/


/*----------------------------------------------------------------------
  DrawStixSigma
  ----------------------------------------------------------------------*/
void DrawStixSigma (int frame, int x, int y,  int l, int h, ThotFont font, int fg)
{
   if (fg < 0)
     return;

   /* Integrals using esstix6 charmap
     52 - => 3x text 3 line eq
     33 - => 2x text 2 line eq
     69 - => 1x+2 text or 18 for oneline eq */

   if (h < LOW_CHAR)
     /* display a single glyph */
     {
       DrawStixChar (font, 83, x, y, l, h, fg, frame);
     }
   else if (h < MID_CHAR)
	/* display a single glyph */
     {
       DrawStixChar (font, 45, x, y, l, h, fg, frame);
     }
   else
    /* display a single glyph */
     {
       DrawStixChar (font, 62, x, y, l, h, fg, frame);
     }
}
/*----------------------------------------------------------------------
  DrawStixIntegral draws an integral. depending on type :
  - simple if type = 0
  - contour if type = 1
  - double if type = 2.
  parameter fg indicates the drawing color
  ----------------------------------------------------------------------*/
void DrawStixIntegral (int frame, int thick, int x, int y, int l, int h,
		       int type, ThotFont font, int fg)
{
  /* Integrals using esstix6 charmap
     52 - => 3x text 3 line eq
     33 - => 2x text 2 line eq
     69 - => 1x+2 text or 18 for oneline eq */
  if (h < LOW_CHAR)
     /* display a single glyph */
     {
       font =  (ThotFont)LoadStixFont (6, FontRelSize (h-15));
       DrawStixChar (font, 69, x, y, l, h, fg, frame);
     }
  else if (h < MID_CHAR)
    /* display a single glyph */
    {
      font =  (ThotFont)LoadStixFont (6, FontRelSize (h-5));
      DrawStixChar (font, 33, x, y, l, h, fg, frame);
    }
  else 
    /* display a single glyph */
    {
      font =  (ThotFont)LoadStixFont (6, FontRelSize (h-5));
      DrawStixChar (font, 52, x, y, l, h, fg, frame);
     }
   if (type == 2)		
     /* double integral */
      DrawStixIntegral (frame, thick, x + (CharacterWidth (52, font) / 2),
		    y, l, h, -1, font, fg);
   /*contour integral
     else if (type == 1)	
     DrawChar ('o', frame, x + ((l - CharacterWidth (111, font)) / 2),
     y + (h - CharacterHeight (111, font)) / 2 + CharacterAscent (111, font),
     font, fg);
   */

}
/* ----------------------------------------------------------------------
  StixIntegralWidth
  ----------------------------------------------------------------------*/
static int StixIntegralWidth (int h, ThotFont font)
{
  int i;
  
  if (h < LOW_HEIGHT)
    i = CharacterWidth (69, font);
  else if (h < MID_HEIGHT)
    i = CharacterWidth (33, font);
  else 
    i = CharacterWidth (52, font);
  return i;
}

/*----------------------------------------------------------------------
  DrawStixBracket draw an opening or closing bracket (depending on direction)
  parameter fg indicates the drawing color
  ----------------------------------------------------------------------*/
void DrawStixBracket (int frame, int thick, int x, int y, int l, int h,
		  int direction, ThotFont font, int fg)
{
  /*  Esstix 7 : 
      61 normal
      33 2 line
      48 3 line  */
  if (h < LOW_CHAR )
    {
      if (direction == 0)
	DrawStixChar (font, 63, x, y, l, h, fg, frame);
      
      else
	DrawStixChar (font, 64, x, y, l, h, fg, frame);
      return;
    }
  else if (h < MID_CHAR)
    {
      if (direction == 0)
	DrawStixChar (font, 36, x, y, l, h, fg, frame);
      else
	DrawStixChar (font, 37, x, y, l, h, fg, frame);
      return;
    }
  else
    {
      if (direction == 0)
	DrawStixChar (font, 50, x, y, l, h, fg, frame);
      else
	DrawStixChar (font, 51, x, y, l, h, fg, frame);
      return;
    }
}
/* ----------------------------------------------------------------------
  StixParenthesisWidth
  ----------------------------------------------------------------------*/
static int StixBracketWidth (int h, ThotFont font)
{
  int i;
  
  if (h < LOW_HEIGHT)
    i = CharacterWidth (63, font);
  else if (h < MID_HEIGHT)
    i = CharacterWidth (36, font);
  else 
    i = CharacterWidth (50, font);
  return i;
}

/*----------------------------------------------------------------------
  DrawStixPointyBracket draw an opening or closing pointy bracket (depending
  on direction)
  parameter fg indicates the drawing color
  ----------------------------------------------------------------------*/
void DrawStixPointyBracket (int frame, int thick,
			    int x, int y,
			    int l, int h,
			    int direction, ThotFont font, int fg)
{
  if (fg < 0)
    return;

  /*   Esstix 7 : 
       61 normal
       33 2 line
       48 3 line  */
  if (h <  LOW_CHAR)
    {
      if (direction == 0)
	DrawStixChar (font, 67, x, y, l, h, fg, frame);
      
      else
	DrawStixChar (font, 68, x, y, l, h, fg, frame);
      return;
    }
  else if (h < MID_CHAR)
    {
      if (direction == 0)
	DrawStixChar (font, 41, x, y, l, h, fg, frame);
      else
	DrawStixChar (font, 42, x, y, l, h, fg, frame);
      return;
    }
  else
    {
      if (direction == 0)
	DrawStixChar (font, 54, x, y, l, h, fg, frame);
      else
	DrawStixChar (font, 55, x, y, l, h, fg, frame);
      return;
    }
 
}

/*----------------------------------------------------------------------
  DrawStixParenthesis draw a closing or opening parenthesis (direction).
  parameter fg indicates the drawing color
  ----------------------------------------------------------------------*/
void DrawStixParenthesis (int frame, int thick, int x, int y, int l, int h,
		      int direction, ThotFont font, int fg)
{
  /*  Esstix 7 : 
    61 normal
    33 2 line
    48 3 line
  */
  
  if (h < LOW_CHAR)
    {
      if (direction == 0)
	  DrawStixChar (font, 61, x, y, l, h, fg, frame);
      else
	  DrawStixChar (font, 62, x, y, l, h, fg, frame);
      return;
    }
  else if (h < MID_CHAR)
  {
      if (direction == 0)
	  DrawStixChar (font, 33, x, y, l, h, fg, frame);
      else
	  DrawStixChar (font, 35, x, y, l, h, fg, frame);
      return;
  }
  else
  {
      if (direction == 0)
	  DrawStixChar (font, 48, x, y, l, h, fg, frame);
      else
	  DrawStixChar (font, 49, x, y, l, h, fg, frame);
      return;
  }
}
/* ----------------------------------------------------------------------
  StixParenthesisWidth
  ----------------------------------------------------------------------*/
static int StixParenthesisWidth (int h, ThotFont font)
{
  int i;
  
  if (h < LOW_HEIGHT)
    i = CharacterWidth (61, font);
  else if (h < MID_HEIGHT)
    i = CharacterWidth (33, font);
  else 
    i = CharacterWidth (48, font);
  return i;
}

/*----------------------------------------------------------------------
  DrawStixBrace draw an opening of closing brace (depending on direction).
  parameter fg indicates the drawing color
  ----------------------------------------------------------------------*/
void DrawStixBrace (int frame, int thick, int x, int y, int l, int h,
		    int direction, ThotFont font, int fg)
{
/*
  Esstix 7 : 
  61 normal
  33 2 line
  48 3 line
  */
  if (h < LOW_CHAR)
    {
      if (direction == 0)
	  DrawStixChar (font, 65, x, y, l, h, fg, frame);
      else
	  DrawStixChar (font, 66, x, y, l, h, fg, frame);
      return;
    }
  else if (h < MID_CHAR)
  {
      if (direction == 0)
	  DrawStixChar (font, 38, x, y, l, h, fg, frame);
      else
	  DrawStixChar (font, 40, x, y, l, h, fg, frame);
      return;
  }
  else
  {
      if (direction == 0)
	  DrawStixChar (font, 52, x, y, l, h, fg, frame);
      else
	  DrawStixChar (font, 53, x, y, l, h, fg, frame);
      return;
  } 
}


/* ----------------------------------------------------------------------
  StixBraceWidth
  ----------------------------------------------------------------------*/
static int StixBraceWidth (int h, ThotFont font)
{
  int i;
  
  if (h < LOW_HEIGHT)
    i = CharacterWidth (65, font);
  else if (h < MID_HEIGHT)
    i = CharacterWidth (38, font);
  else 
    i = CharacterWidth (52, font);
  return i;
}


/*----------------------------------------------------------------------
  DrawStixChar draw a one glyph symbol.
  parameter fg indicates the drawing color
  ----------------------------------------------------------------------*/
void DrawStixChar (ThotFont font, unsigned char symb, int x, int y, 
		   int l, int h, int fg, int frame)
{
  x = x + ((l - CharacterWidth ((char) symb, font)) / 2);
  y = y + ((h - CharacterHeight ((char)symb, font)) / 2) 
    + CharacterAscent ((char) symb, font);
  DrawChar ((char) symb, frame, x, y, font, fg);
}

/*----------------------------------------------------------------------
  GetMathFontWidth : Calculates the width of the stix char
----------------------------------------------------------------------*/
int GetMathFontWidth (SpecFont fontset, char shape, int size, int height)
{
  ThotFont      pfont = NULL;
  int           i;

  i = 0;
  if (height > 0) 
      GetMathFontFromChar (shape, fontset, (void **) &pfont,
			   FontRelSize (height-5));
  else
    return 0;
  
  if (pfont != NULL)
    {
      switch (shape)
	{
	case 'd':	/* double integral */
	  i = StixIntegralWidth (size, pfont)/2;
	case 'i':	/* integral */
	case 'c':	/* circle integral */
	  i += StixIntegralWidth (size, pfont);
	  break;
	case '(':
	case ')':
	  i = StixParenthesisWidth (size, pfont); 
	  break;
	case '{':
	case '}':
	  i = StixBraceWidth (size, pfont);
	  break;
	case '[':
	case ']':
	  i = StixBracketWidth (size, pfont);	
	  break;
	default:
	  break;
	}
    }
  return i;
}

/*----------------------------------------------------------------------
   GiveStixSize gives the internal size of a symbol box.
  ----------------------------------------------------------------------*/
void GiveStixSize (ThotFont pfont, PtrAbstractBox pAb, int *width,
		   int *height, int size)
{
  int                 hfont;

  hfont = FontHeight (pfont);
  *height = hfont * 2;
  switch (pAb->AbShape)
    {
      
    case 'd':	/* double integral */
      *width = StixIntegralWidth (size, pfont) + 
	StixIntegralWidth (size, pfont)/2;
      *height *= 4;
      break;      
    case 'i':	/* integral */
    case 'c':	/* circle integral */
      *width = StixIntegralWidth (size, pfont);
      *height *= 2;
      break;
    case '(':
    case ')':
      *width = StixParenthesisWidth (size, pfont); 
      break;
    case '{':
    case '}':
      *width = StixBraceWidth (size, pfont);
      break;
    case '[':
    case ']':
      *width = StixBracketWidth (size, pfont);	
      break;
#ifdef o
    case '<':
    case '>':
      *width = BoxCharacterWidth (241, pfont);
      *height = hfont;
      break;
    case 'o':       /* overbrace */
    case 'u':       /* underbrace */
      *width = *height;
      *height = hfont / 2;
      break;
    case 'I':	/*intersection */
    case 'U':	/*union */
      *width = BoxCharacterWidth (229, pfont);
      *height = hfont;
      break;
#endif /**/
    }
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void GetMathFontFromChar (char typesymb,
    			  SpecFont fontset,
			  void **font,
			  int height)
{
  switch (typesymb)
    {
      /*integral, union...*/
    case 'i':
    case 'c':
    case 'd':	  
    case 'I':
    case 'U':
    case 'o':
    case 'u':
      *font =  LoadStixFont (6, height);
      break;
    case '(':
    case ')':
    case '{':
    case '}':
    case '[':
    case ']':
    case '<':
    case '>':
      if (height > 2)
	*font = LoadStixFont (7, height);
      break;
    default:
      *font = NULL;	  
      return;
      break;
    }
}


