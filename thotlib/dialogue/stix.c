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

typedef struct _MapEntry {
  unsigned char MapFont;
  unsigned char MapIndex;
} MapEntry;

/* Mapping of arrows in the stix fonts */
unsigned int Stix_Arrows_Start = 0x2190;
MapEntry     Stix_Arrows [] = {
  {1, 0x21}, {1,0x37}
};
#define Stix_Arrows_length sizeof(Stix_Arrows) / sizeof(MapEntry)

/*----------------------------------------------------------------------
  GetStixFontAndIndex returns the glyph index and the font
  used to display the wide character c
  ----------------------------------------------------------------------*/
int GetStixFontAndIndex (CHAR_T c, SpecFont fontset, ThotFont **font)
{
  MapEntry      entry;
  int           val = c, index = 0, face = 0;

  if (val >= Stix_Arrows_Start && val < Stix_Arrows_Start + Stix_Arrows_length)
    {
      entry = Stix_Arrows[val - Stix_Arrows_Start];
      index = (int) (entry.MapIndex);
      face = (int) (entry.MapFont);
    }
  if (face == 1)
    *font = &(fontset->SFont_1);
  else if (face == 2)
    *font = &(fontset->SFont_2);
  else if (face == 3)
    *font = &(fontset->SFont_3);
  else if (face == 4)
    *font = &(fontset->SFont_4);
  else if (face == 5)
    *font = &(fontset->SFont_5);
  else if (face == 6)
    *font = &(fontset->SFont_6);
  else if (face == 7)
    *font = &(fontset->SFont_7);
  else if (face == 8)
    *font = &(fontset->SFont_8);
  else if (face == 9)
    *font = &(fontset->SFont_9);
  else if (face == 10)
    *font = &(fontset->SFont_10);
  else if (face == 11)
    *font = &(fontset->SFont_11);
  else if (face == 12)
    *font = &(fontset->SFont_12);
  else if (face == 13)
    *font = &(fontset->SFont_13);
  else if (face == 14)
    *font = &(fontset->SFont_14);
  else if (face == 15)
    *font = &(fontset->SFont_15);
  else if (face == 16)
    *font = &(fontset->SFont_16);
  else if (face == 17)
    *font = &(fontset->SFont_17);

  return index;
}

/*----------------------------------------------------------------------
  DrawStixSigma
  ----------------------------------------------------------------------*/
void DrawStixSigma (int frame, int x, int y,  int l, int h, ThotFont font, int fg)
{
  if (fg < 0)
    return;

  /* Integrals using esstix6 charmap
     62 - => 3x text 3 line eq
     45 - => 2x text 2 line eq
     83 - => 1x text for one-line eq */

  if (h < LOW_CHAR)
    /* display a single glyph */
    DrawStixChar (font, 83, x, y, l, h, fg, frame);
  else if (h < MID_CHAR)
    /* display a single glyph */
    DrawStixChar (font, 45, x, y, l, h, fg, frame);
  else
    /* display a single glyph */
    DrawStixChar (font, 62, x, y, l, h, fg, frame);
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
  unsigned char   symb;

  /* Integrals using esstix6 charmap
     52 - => 3x text 3 line eq
     33 - => 2x text 2 line eq
     69 - => 1x text for oneline eq */
  /* display a single glyph */
  if (h < LOW_CHAR)
    symb = 69;
  else if (h < MID_CHAR)
    symb = 33;
  else
    symb = 52;
  font = (ThotFont)LoadStixFont (6, CharRelSize (h, symb, 6));
  DrawStixChar (font, symb, x, y, l, h, fg, frame);

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
  unsigned char   symb;

  /*  Esstix 7 : 
      61 normal
      33 2 line
      48 3 line  */
  if (h < LOW_CHAR )
    {
      if (direction == 0)
	symb = 63;
      else
        symb = 64;
    }
  else if (h < MID_CHAR)
    {
      if (direction == 0)
	symb = 36;
      else
	symb = 37;
    }
  else
    {
      if (direction == 0)
	symb = 50;
      else
	symb = 51;
    }
  font = (ThotFont)LoadStixFont (7, CharRelSize (h, 36, 7));
  DrawStixChar (font, symb, x, y, l, h, fg, frame);
}

/* ----------------------------------------------------------------------
  StixBracketWidth
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
  unsigned char   symb;

  if (fg < 0)
    return;

  /*   Esstix 7 : 
       61 normal
       33 2 line
       48 3 line  */
  if (h <  LOW_CHAR)
    {
      if (direction == 0)
        symb = 67;
      else
	symb = 68;
    }
  else if (h < MID_CHAR)
    {
      if (direction == 0)
	symb = 41;
      else
	symb = 42;
    }
  else
    {
      if (direction == 0)
	symb = 54;
      else
	symb = 55;
    }
  font = (ThotFont)LoadStixFont (7, CharRelSize (h, 41, 7));
  DrawStixChar (font, symb, x, y, l, h, fg, frame); 
}

/* ----------------------------------------------------------------------
  StixPointyBracketWidth
  ----------------------------------------------------------------------*/
static int StixPointyBracketWidth (int h, ThotFont font)
{
  int i;
  
  if (h < LOW_HEIGHT)
    i = CharacterWidth (67, font);
  else if (h < MID_HEIGHT)
    i = CharacterWidth (41, font);
  else 
    i = CharacterWidth (54, font);
  return i;
}

/*----------------------------------------------------------------------
  DrawStixParenthesis draw a closing or opening parenthesis (direction).
  parameter fg indicates the drawing color
  ----------------------------------------------------------------------*/
void DrawStixParenthesis (int frame, int thick, int x, int y, int l, int h,
		      int direction, ThotFont font, int fg)
{
  unsigned char   symb;

  /*  Esstix 7 : 
    61 normal
    33 2 line
    48 3 line
  */
  if (h < LOW_CHAR)
    {
      if (direction == 0)
	symb = 61;
      else
	symb = 62;
    }
  else if (h < MID_CHAR)
    {
      if (direction == 0)
	symb = 33;
      else
	symb = 35;
    }
  else
    {
      if (direction == 0)
	symb = 48;
      else
	symb = 49;
    }
  font = (ThotFont)LoadStixFont (7, CharRelSize (h, 33, 7));
  DrawStixChar (font, symb, x, y, l, h, fg, frame);
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
  unsigned char   symb;
  /*
  Esstix 7 : 
  61 normal
  33 2 line
  48 3 line
  */
  if (h < LOW_CHAR)
    {
      if (direction == 0)
	symb = 65;
      else
	symb = 66;
    }
  else if (h < MID_CHAR)
    {
      if (direction == 0)
	symb = 38;
      else
	symb = 40;
    }
  else
    {
      if (direction == 0)
	symb = 52;
      else
	symb = 53;
    }
  font = (ThotFont)LoadStixFont (7, CharRelSize (h, 38, 7));
  DrawStixChar (font, symb, x, y, l, h, fg, frame);
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
	case '<':
	case '>':
	  i = StixPointyBracketWidth (size, pfont);	
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
      *width = *width + *width/2;
      *height *= 4;
      break;      
    case 'i':	/* integral */
    case 'c':	/* circle integral */
      *width = StixIntegralWidth (size, pfont);
      *width = *width + *width;
      *height *= 2;
      break;
    case '(':
    case ')':
      *width = StixParenthesisWidth (size, pfont); 
      *width = *width + *width/2;
      break;
    case '{':
    case '}':
      *width = StixBraceWidth (size, pfont);
      *width = *width + *width/2;
      break;
    case '[':
    case ']':
      *width = StixBracketWidth (size, pfont);	
      *width = *width + *width/2;
      break;
    case '<':
    case '>':
      *width = StixPointyBracketWidth (size, pfont);	
      *width = *width + *width/2;
      break;
#ifdef o
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
void GetMathFontFromChar (char typesymb, SpecFont fontset, void **font,
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
      if (height > 2)
	*font = LoadStixFont (7, height);
      break;
    case '<':
    case '>':
      *font = LoadStixFont (7, height);
      break;
    default:
      *font = NULL;	  
      return;
      break;
    }
}


