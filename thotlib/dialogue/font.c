/*
 *
 *  (c) COPYRIGHT INRIA, Grif, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 
/*
 * Module dedicated to font handling.
 *
 * Author: I. Vatton (INRIA)
 *         R. Guetari & D. Veillard (W3C/INRIA): Windows NT/95 routines
 *
 */

#include "thot_sys.h"
#include "libmsg.h"
#include "message.h"
#include "constmedia.h"
#include "typemedia.h"
#include "frame.h"
#include "xpmP.h"

/*  tolower(c) was a macro defined in ctypes.h that returns
   something wrong if c is not an upper case letter. */
#define TOLOWER(c)	(isupper(c)? tolower(c) : (c))

#define THOT_EXPORT extern
#include "frame_tv.h"
#include "units_tv.h"
#undef THOT_EXPORT
#define THOT_EXPORT
#include "font_tv.h"


/* that table for the character glyphs */
static int          FirstRemovableFont = 1;
static char         StylesTable[MAX_HIGHLIGHT] = "rbiogq";

/* Maximum number of font size handled */
static int          MaxNumberOfSizes;
static int          LogicalPointsSizes[MAX_LOG_SIZE] =
{6, 8, 10, 12, 14, 16, 20, 24, 30, 40, 60};
static char        *FontFamily;
static boolean      UseLucidaFamily;
static boolean      UseBitStreamFamily;


#include "buildlines_f.h"
#include "registry_f.h"
#include "language_f.h"
#include "font_f.h"
#include "memory_f.h"
#include "registry_f.h"
#include "units_f.h"
#include "windowdisplay_f.h"

#ifdef _WINDOWS
static char  WIN_lpszFace [255];
static int   WIN_nHeight;
static int   WIN_nWidth;
static int   WIN_fnWeight;
static int   WIN_fdwItalic;
static int   WIN_fdwUnderline;
static int   WIN_fdwStrikeOut;

#endif /* _WINDOWS */


#ifdef _WINDOWS
#if 0
/*----------------------------------------------------------------------
 *    FontCreated
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static int FontCreated (char alphabet, char family, int highlight, int size, TypeUnit unit)
#else  /* __STDC__ */
static int FontCreated (alphabet, family, highlight, size, unit)
char     alphabet;
char     family;
int      highlight;
int      size;
TypeUnit unit;
#endif /* __STDC__ */
{
    int i = 0;
	BOOL found = FALSE;

	while (i < nbFontsCreated && !found) {
          if (FCTable [i].alphabet       == alphabet         &&
             toupper(FCTable [i].family) == toupper (family) &&
             FCTable [i].highlight       == highlight        &&
             FCTable [i].size            == size             &&
             FCTable [i].unit            == unit)
			 found = TRUE ;
		  else 
			  i++;
    }

	if (found)
       return i;
    return -1;
}
#endif /* 0 */
/*----------------------------------------------------------------------
 *    WIN_LoadFont :  load a Windows TRUEType with a defined set of
 *                    characteristics.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static HFONT WIN_LoadFont (char alphabet, char family, int highlight, int size, TypeUnit unit)
#else  /* __STDC__ */
static HFONT WIN_LoadFont (alphabet, family, highlight, size, unit)
char     alphabet;
char     family;
int      highlight;
int      size;
TypeUnit unit;
#endif /* __STDC__ */
{
   HFONT hFont;

   WIN_nHeight      = 0;
   WIN_nWidth       = 0;
   WIN_fnWeight     = FW_NORMAL;
   WIN_fdwItalic    = FALSE;
   WIN_fdwUnderline = FALSE;
   WIN_fdwStrikeOut = FALSE;

   if (alphabet != 'L' && alphabet != 'G' && alphabet != 'g')
      return NULL;

   if (alphabet == 'g' || alphabet == 'G') {
      family    = 's';
      highlight = 0 ;
   }

   switch (family) {
          case 'T':
          case 't':
               sprintf (&WIN_lpszFace[0], "Times New Roman");
               break;

          case 'H':
          case 'h':
               sprintf (&WIN_lpszFace[0], "Arial");
               break;

          case 'C':
          case 'c':
               sprintf (&WIN_lpszFace[0], "Courier New");
               break;

          case 'S':
          case 's':
               sprintf (&WIN_lpszFace[0], "Symbol");
               break;

          default:
               return NULL;
   }

   switch (StylesTable[highlight]) {
          case 'r':
               break;

          case 'i':
          case 'o':
               WIN_fdwItalic = TRUE;
               break;

          case 'b':
          case 'g':
          case 'q':
               WIN_fnWeight = FW_BOLD;
               break;

          default:
               return NULL;
   }

   WIN_nHeight = -MulDiv (size, DOT_PER_INCHE, 72);

   hFont = CreateFont (WIN_nHeight, WIN_nWidth, 0, 0, WIN_fnWeight,
                       WIN_fdwItalic, WIN_fdwUnderline, WIN_fdwStrikeOut,
                       DEFAULT_CHARSET, OUT_TT_ONLY_PRECIS, CLIP_DEFAULT_PRECIS,
                       PROOF_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
                       WIN_lpszFace);

   if (hFont == NULL) {
      WinErrorBox (WIN_Main_Wd);
   } 

   return (hFont);
}

/*----------------------------------------------------------------------
 *      WinLoadFont : Load a Windows font in a Device context.
 *----------------------------------------------------------------------*/
#ifdef __STDC__
void    WinLoadFont (HDC hdc, ptrfont font)
#else  /* !__STDC__ */
void    WinLoadFont (hdc, font)
HDC     hdc; 
ptrfont font;
#endif /* __STDC__ */
{
  return (SelectObject (hdc, font->FiFont));
}
#endif /* _WINDOWS */

/*----------------------------------------------------------------------
 *      NumberOfFonts returns the number of fonts.
  ----------------------------------------------------------------------*/
int NumberOfFonts ()
{
  return MaxNumberOfSizes + 1;
}

/*----------------------------------------------------------------------
 *      GetCharsCapacity converts from pixel volume to char size
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int GetCharsCapacity (int volpixel)
#else  /* __STDC__ */
int GetCharsCapacity (volpixel)
int volpixel;
#endif /* __STDC__ */
{
  return volpixel / 200;
}


/*----------------------------------------------------------------------
 *      CharacterWidth returns the width of a char in a given font.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                 CharacterWidth (unsigned char c, ptrfont font)
#else  /* __STDC__ */
int                 CharacterWidth (c, font)
unsigned char       c;
ptrfont             font;
#endif /* __STDC__ */
{
  int                 l;

  if (font == NULL)
    return (0);
  else
    {
#ifdef _WINDOWS
      l = font->FiWidths[c];
#else  /* _WINDOWS */
      if (((XFontStruct*) font)->per_char == NULL)
	l = ((XFontStruct*) font)->max_bounds.width;
      else if (c < ((XFontStruct*) font)->min_char_or_byte2)
	l = 0;
      else 
	l = ((XFontStruct *) font)->per_char[c - ((XFontStruct *) font)->min_char_or_byte2].width;
#endif /* !_WINDOWS */
    }
  return (l);
}

/*----------------------------------------------------------------------
 *      CharacterHeight returns the height of a char in a given font.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                 CharacterHeight (unsigned char c, ptrfont font)
#else  /* __STDC__ */
int                 CharacterHeight (c, font)
unsigned char       c;
ptrfont             font;
#endif /* __STDC__ */
{
  if (font == NULL)
    return (0);
#ifdef _WINDOWS
  else
    return (font->FiHeights[c]);
#else  /* _WINDOWS */
   else if (((XFontStruct *) font)->per_char == NULL)
        return FontHeight (font);
   else
       return ((XFontStruct *) font)->per_char[c - ((XFontStruct *) font)->min_char_or_byte2].ascent
               + ((XFontStruct *) font)->per_char[c - ((XFontStruct *) font)->min_char_or_byte2].descent;
#endif /* !_WINDOWS */
}

/*----------------------------------------------------------------------
       CharacterAscent returns the ascent of a char in a given font.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                 CharacterAscent (unsigned char c, ptrfont font)
#else  /* __STDC__ */
int                 CharacterAscent (c, font)
unsigned char       c;
ptrfont             font;
#endif /* __STDC__ */
{
  if (font == NULL)
    return (0);
#ifdef _WINDOWS
  else
    return font->FiAscent;
#else  /* _WINDOWS */
   else if (((XFontStruct *) font)->per_char == NULL)
        return ((XFontStruct *) font)->max_bounds.ascent;
   else
       return ((XFontStruct *) font)->per_char[c - ((XFontStruct *) font)->min_char_or_byte2].ascent;
#endif /* !_WINDOWS */

}

/*----------------------------------------------------------------------
 *      FontAscent returns a global ascent for a font.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                 FontAscent (ptrfont font)
#else  /* __STDC__ */
int                 FontAscent (font)
ptrfont             font;
#endif /* __STDC__ */
{
  if (font == NULL)
    return (0);
#ifdef _WINDOWS
  else
    return (font->FiAscent);
#else  /* _WINDOWS */
   else
     return ((XFontStruct *) font)->ascent;
#endif /* !_WINDOWS */
}

/*----------------------------------------------------------------------
 *      FontHeight returns the height of a given font.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                 FontHeight (ptrfont font)
#else  /* __STDC__ */
int                 FontHeight (font)
ptrfont             font;
#endif /* __STDC__ */
{
  if (font == NULL)
    return (0);
#ifdef _WINDOWS
   else
     return (font->FiHeight);
#else  /* _WINDOWS */
   else
     return ((XFontStruct *) font)->max_bounds.ascent + ((XFontStruct *) font)->max_bounds.descent;
#endif /* !_WINDOWS */
}

/*----------------------------------------------------------------------
 *  PixelValue computes the pixel size for a given logical unit.
 *		pAb is the current Pave except for UnPercent unit
 *		here it hold the comparison value.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                 PixelValue (int val, TypeUnit unit, PtrAbstractBox pAb)
#else  /* __STDC__ */
int                 PixelValue (val, unit, pAb)
int                 val;
TypeUnit            unit;
PtrAbstractBox      pAb;
#endif /* __STDC__ */
{
   int              dist, i;

   dist = 0;
   switch (unit) {
          case UnRelative:
               if (pAb == NULL || pAb->AbBox == NULL || pAb->AbBox->BxFont == NULL)
                  dist = 0;
               else
                   dist = (val * FontHeight (pAb->AbBox->BxFont) + 5) / 10;
               break;
          case UnXHeight:
               if (pAb == NULL || pAb->AbBox == NULL || pAb->AbBox->BxFont == NULL)
                  dist = 0;
               else
                   dist = (val * CharacterHeight ('X', pAb->AbBox->BxFont)) / 10;
               break;
          case UnPoint:
               dist = PointToPixel (val);
               break;
          case UnPixel:
               dist = val;
               break;
          case UnPercent:
               i = val * (int) pAb;
               dist = i / 100;
               break;
   }
   return (dist);
}

/*----------------------------------------------------------------------
 *  PixelValue computes the logical value for a given pixel size.
 *		pAb is the current Pave except for UnPercent unit
 *		here it hold the comparison value.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                 LogicalValue (int val, TypeUnit unit, PtrAbstractBox pAb)
#else  /* __STDC__ */
int                 LogicalValue (val, unit, pAb)
int                 val;
TypeUnit            unit;
PtrAbstractBox      pAb;
#endif /* __STDC__ */
{
   int              dist, i;

   dist = 0;
   switch (unit)
     {
     case UnRelative:
       if (pAb == NULL || pAb->AbBox == NULL || pAb->AbBox->BxFont == NULL)
	 dist = 0;
       else
	 dist = val * 10 / FontHeight (pAb->AbBox->BxFont);
       break;
     case UnXHeight:
       if (pAb == NULL || pAb->AbBox == NULL || pAb->AbBox->BxFont == NULL)
	 dist = 0;
       else
	 dist = val * 10 / CharacterHeight ('x', pAb->AbBox->BxFont);
       break;
     case UnPoint:
       dist = PixelToPoint (val);
       break;
     case UnPixel:
       dist = val;
       break;
     case UnPercent:
       if (pAb == NULL)
	 dist = 0;
       else
	 {
	   i = val * 100;
	   dist = i / (int) pAb;
	 }
       break;
     }
   return (dist);
}


/*----------------------------------------------------------------------
 *      FontBase returns the shifting of the base line for a given font.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                 FontBase (ptrfont font)
#else  /* __STDC__ */
int                 FontBase (font)
ptrfont             font;
#endif /* __STDC__ */
{
   if (font == NULL)
      return (0);
   else
      return (FontAscent (font));
}

/*----------------------------------------------------------------------
 *   FontRelSize converts between a size in points and the logical size.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                 FontRelSize (int size)
#else  /* __STDC__ */
int                 FontRelSize (size)
int                 size;
#endif /* __STDC__ */
{
   int                 j;

   j = 0;
   while ((size > LogicalPointsSizes[j]) && (j < MaxNumberOfSizes))
      j++;

   return (j);
}

/*----------------------------------------------------------------------
 *   FontPointSize convert a logical size to the point value.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                 FontPointSize (int size)
#else  /* __STDC__ */
int                 FontPointSize (size)
int                 size;
#endif /* __STDC__ */
{

   if (size > MaxNumberOfSizes)
      size = MaxNumberOfSizes;
   else if (size < 0)
      size = 0;

   return (LogicalPointsSizes[size]);
}

#ifndef _WINDOWS
/*----------------------------------------------------------------------
 *      LoadFont load a given font designed by its name.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ptrfont             LoadFont (char name[100], int toPatch)
#else  /* __STDC__ */
ptrfont             LoadFont (name, toPatch)
char                name[100];
int                 toPatch;
#endif /* __STDC__ */
{
   char                tmp[200];
   XFontStruct        *result;
   int                 mincar;
   int                 spacewd;

   strcpy (tmp, name);

   result = XLoadQueryFont (TtDisplay, tmp);
   if (result != NULL)
      if (result->per_char != NULL)
	{
	   mincar = result->min_char_or_byte2;
	   spacewd = result->per_char[32 - mincar].width;
	   if (toPatch != 0)
	     {
	       /* a patch due to errors in standard symbol fonts */
	       if (toPatch == 8 || toPatch == 10)
		 result->per_char[244 - mincar].width = 1;
	       else if (toPatch == 12 || toPatch == 14)
		 result->per_char[244 - mincar].width = 2;
	       else if (toPatch == 24)
		 result->per_char[244 - mincar].width = 4;

	       result->per_char[244 - mincar].ascent -= 2;
	     }
	   if (result->max_char_or_byte2 > UNBREAKABLE_SPACE)
	      /* largeur(Ctrl Space) = largeur(Space) */
	      result->per_char[UNBREAKABLE_SPACE - mincar].width = spacewd;
	   if (result->max_char_or_byte2 > BREAK_LINE)
	      /* largeur(Ctrl Return) = largeur(Space) */
	      result->per_char[BREAK_LINE - mincar].width = 1;
	   if (result->max_char_or_byte2 > THIN_SPACE)
	      /* largeur(Fine) = 1/4largeur(Space) */
	      result->per_char[THIN_SPACE - mincar].width = (spacewd + 3) / 4;
	   if (result->max_char_or_byte2 > HALF_EM)
	      /* largeur(DemiCadratin) = 1/2largeur(Space) */
	      result->per_char[HALF_EM - mincar].width = (spacewd + 1) / 2;
	}
   return ((ptrfont) result);
}
#  endif /* !_WINDOWS */

/*----------------------------------------------------------------------
 *      FontIdentifier computes the name of a Thot font.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                FontIdentifier (char alphabet, char family, int highlight, int size, TypeUnit unit, char r_name[10], char r_nameX[100])
#else  /* __STDC__ */
void                FontIdentifier (alphabet, family, highlight, size, unit, r_name, r_nameX)
char                alphabet;
char                family;
int                 highlight;
int                 size;
TypeUnit            unit;
char                r_name[10];
char                r_nameX[100];

#endif /* __STDC__ */
{

   if (highlight > MAX_HIGHLIGHT)
      highlight = MAX_HIGHLIGHT;
   if (alphabet == 'g' || alphabet == 'G')
     {
	highlight = 0;		/* roman only for symbols */
	family = 's';		/* times only for symbols */
	strcpy (r_nameX, "-*");
     }
   else
      strcpy (r_nameX, FontFamily);

   if (unit == UnRelative)
     {
	/* La size est relative */
	if (size < 0)
	   size = LogicalPointsSizes[0];
	else if (size > MaxNumberOfSizes)
	   size = LogicalPointsSizes[MaxNumberOfSizes];
	else
	   size = LogicalPointsSizes[size];
     }
   else if (unit == UnPixel)
      size = PixelToPoint (size);

   if (UseLucidaFamily)
     {
	switch ((char) TOLOWER (family))
	      {
		 case 't':
		    strcat (r_nameX, "bright");
		    break;
		 case 'c':
		    strcat (r_nameX, "typewriter");
		    break;
		 default:
		    break;
	      }
     }
   else
     {
	switch ((char) TOLOWER (family))
	      {
		 case 't':
		    strcat (r_nameX, "-times");
		    break;
		 case 'h':
		    strcat (r_nameX, "-helvetica");
		    break;
		 case 'c':
		    strcat (r_nameX, "-courier");
		    break;
		 case 's':
		    strcat (r_nameX, "-symbol");
		    break;
		 default:
		    strcat (r_nameX, "-*");
	      }
     }

   switch ((char) TOLOWER (StylesTable[highlight]))
	 {
	    case 'r':
	       strcat (r_nameX, "-medium-r");
	       break;
	    case 'i':
	    case 'o':
	       if ((char) TOLOWER (family) == 'h' || (char) TOLOWER (family) == 'c')
		  strcat (r_nameX, "-medium-o");
	       else
		  strcat (r_nameX, "-medium-i");
	       break;
	    case 'b':
	    case 'g':
	    case 'q':
	       if (UseLucidaFamily && (char) TOLOWER (family) == 't')
		  strcat (r_nameX, "-demibold-r");
	       else
		  strcat (r_nameX, "-bold-r");
	       break;
	 }

   if ((char) TOLOWER (family) == 'h')
      strcat (r_nameX, "-normal");	/* narrow helvetica does not exist */
   else
      strcat (r_nameX, "-*");

   if ((char) TOLOWER (family) == 's')
     {
	if (UseBitStreamFamily)
	   sprintf (r_nameX, "%s-*-*-%d-83-83-p-*-*-fontspecific", r_nameX, size * 10);
	else
	   sprintf (r_nameX, "%s-*-%d-*-75-75-p-*-*-fontspecific", r_nameX, size);
     }
   else
     {
	if (UseBitStreamFamily)
	   sprintf (r_nameX, "%s-*-*-%d-83-83", r_nameX, size * 10);
	else
	   sprintf (r_nameX, "%s-*-%d-*-75-75", r_nameX, size);
	if ((char) TOLOWER (family) == 'c')
	   strcat (r_nameX, "-m-*");
	else
	   strcat (r_nameX, "-p-*");

	if ((char) TOLOWER (alphabet) == 'l')
	   strcat (r_nameX, "-iso8859-1");
	else if ((char) TOLOWER (alphabet) == 'e')
	   strcat (r_nameX, "-iso8859-2");
	else if ((char) TOLOWER (alphabet) == 'g')
	   strcat (r_nameX, "-*-fontspecific");		/*adobe */
	else
	  {
	   strcat (r_nameX, "-iso8859-1");
	   /* replace '1' by current alphabet */
	   r_nameX[strlen (r_nameX) -1] = alphabet;
	  }
     }

   sprintf (r_name, "%c%c%c%d",
	    TOLOWER (alphabet), TOLOWER (family),
	    StylesTable[highlight], size);
}

/*----------------------------------------------------------------------
 *      ReadFont do a raw Thot font loading (bypasses the font cache).
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ptrfont             ReadFont (char alphabet, char family, int highlight, int size, TypeUnit unit)
#else  /* __STDC__ */
ptrfont             ReadFont (alphabet, family, highlight, size, unit)
char                alphabet;
char                family;
int                 highlight;
int                 size;
TypeUnit            unit;

#endif /* __STDC__ */
{
   char                name[10], nameX[100];

   FontIdentifier (alphabet, family, highlight, size, unit, name, nameX);
#  ifndef _WINDOWS
   return LoadFont (nameX, 0);
#  else  /* _WINDOWS */
   return NULL;
#  endif /* _WINDOWS */
}

/*----------------------------------------------------------------------
 *      LoadNearestFont load the nearest possible font given a set
 *		of attributes like alphabet, family, the size and for
 *		a given frame.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static ptrfont      LoadNearestFont (char alphabet, char family, int highlight, int size, TypeUnit unit, int frame, boolean increase)
#else  /* __STDC__ */
static ptrfont      LoadNearestFont (alphabet, family, highlight, size, unit, frame, increase)
char                alphabet;
char                family;
int                 highlight;
int                 size;
TypeUnit            unit;
int                 frame;
boolean             increase;
#endif /* __STDC__ */
{
  int                 i, j, deb, index;
  int                 mask;
  char                text[10], PsName[10], textX[100];
#ifdef _WINDOWS
  SIZE                wsize;
  TEXTMETRIC          textMetric;
  char                fontSize[5];
  char               *pText;
  char               *pFontSize;
  int                 c;
  HFONT               hOldFont;
#endif /* _WINDOWS */
  ptrfont             ptfont;

  /* use only standard sizes */
  index = 0;
  if (unit == UnRelative)
    index = size;
  else
    {
      if (unit == UnPixel)
	{
	  size = PixelToPoint (size);
	  unit = UnPoint;
	}
      else if (unit == UnXHeight || unit == UnPercent)
	/* what does this mean??? set default size: 12 pt */
	{
	  size = 12;
	  unit = UnPoint;
	}

      if (unit == UnPoint)
	{
	  /* nearest standard size lookup */
	  index = 0;
	  while (LogicalPointsSizes[index] < size && index <= MaxNumberOfSizes)
	    index++;
	}
    }
   
  if (UseBitStreamFamily && size == 11 && unit == UnPoint)
    /* in the case of Bitstream, accept 11 points font size */
    FontIdentifier (alphabet, family, highlight, size, TRUE, text, textX);
  else
    FontIdentifier (alphabet, family, highlight, index, FALSE, text, textX);
   
  /* initialize the Proscript font name */
  strcpy (PsName, text);
   
  /* Font cache lookup */
  j = 0;
  i = 0;
  deb = 0;
  ptfont = NULL;
  while ((ptfont == NULL) && (i < MAX_FONT) && (TtFonts[i] != NULL))
    {
      j = strcmp (&TtFontName[deb], text);
      if (j == 0)
	{
	  /* Font cache lookup succeeded */
	  ptfont = TtFonts[i];
	}
      else
	i++;
      deb += MAX_FONTNAME;
    }
   
  /* Load a new font */
  if (ptfont == NULL)
    {
      /* Check for table font overflow */
      if (i >= MAX_FONT)
	TtaDisplayMessage (INFO, TtaGetMessage (LIB, TMSG_NO_PLACE_FOR_FONT), textX);
      else
	{
	  strcpy (&TtFontName[i * MAX_FONTNAME], text);
	  strcpy (&TtPsFontName[i * 8], PsName);
	   
#         ifdef _WINDOWS
	  if (unit == UnRelative)
	    {
	      pText = text;
	      pFontSize = &fontSize[0];
	      while (!isdigit (*pText))
		pText++;
	      if (isdigit (*pText))
		while (isdigit (*pText))
		  *pFontSize++ = *pText++;
	      *pFontSize = 0;
	      size = atoi (fontSize);
	    }
	  /* Allocate the font structure */
	  ptfont = TtaGetMemory (sizeof (FontInfo));
	  ptfont->FiFont = WIN_LoadFont (alphabet, family, highlight, size, unit);
	  WIN_GetDeviceContext (-1);
	  hOldFont = SelectObject (TtDisplay, ptfont->FiFont);
	  if (GetTextMetrics (TtDisplay, &textMetric))
	    {
	      ptfont->FiAscent = textMetric.tmAscent;
	      ptfont->FiHeight = textMetric.tmAscent + textMetric.tmDescent;
	    }
	  else
	    {
	      ptfont->FiAscent = 0;
	      ptfont->FiHeight = 0;
	    }
	  for (c = 0; c < 256; c++)
	    {
	      GetTextExtentPoint (TtDisplay, &c, 1, &wsize);
	      ptfont->FiWidths[c] = wsize.cx;
	      ptfont->FiHeights[c] = wsize.cy;
	    }
	  SelectObject (TtDisplay, hOldFont);
	  WIN_ReleaseDeviceContext ();
#      else  /* _WINDOWS */
	  if (alphabet == 'G' && (size > 8 && size < 16 || size == 24))
	    ptfont = LoadFont (textX, size);
	  else
	    ptfont = LoadFont (textX, 0);
#      endif /* !_WINDOWS */
	  /* Loading failed try to find a neighbour */
	  if (ptfont == NULL)
	    {
	      /* Change size */
	      if (index == MaxNumberOfSizes)
		{
		  /* size cannot increase */
		  increase = FALSE;
		  index--;
		}
	      else if (increase)
		index++;
	      else
		index--;
	       
	      if (index < MaxNumberOfSizes && index >= 0)
		ptfont = LoadNearestFont (alphabet, family, highlight, index, FALSE, frame, increase);
	      else if (index >= MaxNumberOfSizes)
		ptfont = LoadNearestFont (alphabet, family, highlight, MaxNumberOfSizes, FALSE, frame, FALSE);
	      if (ptfont == NULL)
		TtaDisplayMessage (INFO, TtaGetMessage (LIB, TMSG_LIB_MISSING_FILE), textX);
	    }
	}

      if (ptfont == NULL)
	{
	  /* Try to load another family from the same alphabet */
	  j = 0;
	  while (j < MAX_FONT)
	    {
	      if (TtFonts[j] == NULL)
		j = MAX_FONT;
	      else if (TtFontName[j * MAX_FONTNAME] == alphabet)
		{
		  ptfont = TtFonts[j];
		  j = MAX_FONT;
		}
	      else
		j++;
	    }

	  /* last case the default font */
	  if (ptfont == NULL)
	    {
	      ptfont = FontDialogue;
	      j = 0;
	    }
	}

      if (i >= MAX_FONT)
	i = j;		/* existing entry in the cache */
      else
	{
	  /* initialize a new cache entry */
	  TtFonts[i] = ptfont;
	  TtFontFrames[i] = 0;
	}
    }

  /* Compute window frame */
  mask = 1 << (frame - 1);
  /* store window frame number */
  TtFontFrames[i] = TtFontFrames[i] | mask;
  return (ptfont);
}

/*----------------------------------------------------------------------
 *      ThotLoadFont try to load a font given a set
 *		of attributes like alphabet, family, the size and for
 *		a given frame.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ptrfont             ThotLoadFont (char alphabet, char family, int highlight, int size, TypeUnit unit, int frame)
#else  /* __STDC__ */
ptrfont             ThotLoadFont (alphabet, family, highlight, size, unit, frame)
char                alphabet;
char                family;
int                 highlight;
int                 size;
TypeUnit            unit;
int                 frame;
#endif /* __STDC__ */
{
   /* pas de family inferieure a 6 points */
   if (size < 6 && unit == UnPoint)
      size = 6;
   return LoadNearestFont (alphabet, family, highlight, size, unit, frame, TRUE);
}


/*----------------------------------------------------------------------
 *      InitDialogueFonts initialize the standard fonts used by the Thot Toolkit.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                InitDialogueFonts (char *name)
#else  /* __STDC__ */
void                InitDialogueFonts (name)
char               *name;
#endif /* __STDC__ */
{
#  ifndef _WINDOWS
   int                 ndir, ncurrent;
   char                FONT_PATH[128];
   char               *fontpath;
#  endif /* _WINDOWS */
   char              **dirlist = NULL;
   char              **currentlist = NULL;
   char               *value;
   char                alphabet;
   int                 f3;
   int                 i;

   /* is there a predefined font family ? */
   MenuSize = 12;
   alphabet = TtaGetAlphabet (TtaGetDefaultLanguage ());
   
   value = TtaGetEnvString ("FontFamily");
   MaxNumberOfSizes = 10;
   if (value == NULL)
     {
	FontFamily = TtaGetMemory (8);
	strcpy (FontFamily, "-*");
     }
   else
     {
	FontFamily = TtaGetMemory (strlen (value) + 1);
	strcpy (FontFamily, value);
	if (!strcmp (FontFamily, "-b&h-lucida"))
	   UseLucidaFamily = TRUE;
	else
	  {
	     UseLucidaFamily = FALSE;
	     if (!strcmp (FontFamily, "gipsi-bitstream"))
	       {
		  UseBitStreamFamily = TRUE;
		  /* Changes size 30, 40 and 60 to resp. 36, 48 et 72 */
		  LogicalPointsSizes[MaxNumberOfSizes] = 72;
		  LogicalPointsSizes[MaxNumberOfSizes - 1] = 48;
		  LogicalPointsSizes[MaxNumberOfSizes - 2] = 36;
		  MenuSize = 11;
	       }
	     else
		UseBitStreamFamily = FALSE;
	  }
     }
   DOT_PER_INCHE = 72;


   /* Is there any predefined size for menu fonts ? */
   value = TtaGetEnvString ("FontMenuSize");
   if (value != NULL)
      sscanf (value, "%d", &MenuSize);
   f3 = MenuSize + 2;

#  ifndef _WINDOWS
   fontpath = TtaGetEnvString ("THOTFONT");
   if (fontpath)
     {
	strcpy (FONT_PATH, fontpath);
	strcat (FONT_PATH, "/");

	/* Add the directory to the X server font path */
	currentlist = XGetFontPath (TtDisplay, &ncurrent);
	ndir = 1;
	/* check that the directory is not already in the list */
	i = 0;
	while ((ndir == 1) && (i < ncurrent))
	  {
	     if (strncmp (currentlist[i], FONT_PATH, strlen (currentlist[i]) - 1) == 0)
		ndir = 0;
	     else
		i++;
	  }

	/* Should we write down the new value ? */
	if (ndir > 0)
	  {
	     ndir += ncurrent;
	     dirlist = (char **) TtaGetMemory (ndir * sizeof (char *));

	     if (currentlist != NULL)
#ifdef SYSV
		memcpy (dirlist, currentlist, ncurrent * sizeof (char *));

#else
		bcopy (currentlist, dirlist, ncurrent * sizeof (char*));

#endif
	     dirlist[ncurrent] = FONT_PATH;
	     XSetFontPath (TtDisplay, dirlist, ndir);
	     TtaFreeMemory ( dirlist);
	  }
	TtaFreeMemory ( currentlist);
     }
#  endif /* _WINDOWS */

   /* Initialize the Thot Lib standards fonts */
   FontDialogue = IFontDialogue = LargeFontDialogue = NULL;
   SymbolIcons = NULL;
   GraphicsIcons = NULL;
   SmallFontDialogue = NULL;

   /* Initialize the font table */
   for (i = 0; i < MAX_FONT; i++)
      TtFonts[i] = NULL;

   /* load first five predefined fonts */
   FontDialogue = ThotLoadFont (alphabet, 't', 0, MenuSize, UnPoint, 0);
   if (FontDialogue == NULL)
     {
	FontDialogue = ThotLoadFont (alphabet, 'l', 0, MenuSize, UnPoint, 0);
	if (FontDialogue == NULL)
	   TtaDisplaySimpleMessage (FATAL, LIB, TMSG_MISSING_FONT);
     }

   IFontDialogue = ThotLoadFont (alphabet, 't', 2, MenuSize, UnPoint, 0);
   if (IFontDialogue == NULL)
     {
	IFontDialogue = ThotLoadFont (alphabet, 'l', 2, MenuSize, UnPoint, 0);
	if (IFontDialogue == NULL)
	   IFontDialogue = FontDialogue;
     }

   LargeFontDialogue = ThotLoadFont (alphabet, 't', 1, f3, UnPoint, 0);
   if (LargeFontDialogue == NULL)
     {
	LargeFontDialogue = ThotLoadFont (alphabet, 't', 1, f3, UnPoint, 0);
	if (LargeFontDialogue == NULL)
	   LargeFontDialogue = IFontDialogue;
     }
   FirstRemovableFont = 3;
}

/*----------------------------------------------------------------------
 *      ThotFreeFont free the font familly loaded by a frame.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ThotFreeFont (int frame)
#else  /* __STDC__ */
void                ThotFreeFont (frame)
int                 frame;

#endif /* __STDC__ */
{
   int                 i, j, mask;
   int                 flag;

   if (frame > 0)
     {
	/* compute the frame mask */
	mask = 1 << (frame - 1);

	i = FirstRemovableFont;
	/* keep the first fonts */
	while (i < MAX_FONT && TtFonts[i] != NULL)
	  {
	     /* if this font family is only used by this frame */
	     if (TtFontFrames[i] == mask)
	       {
		  j = 0;
		  flag = 0;
		  while (flag == 0)
		    {
		       if (j == MAX_FONT)
			  flag = 1;
		       else if (j == i)
			  j++;
		       else if (TtFonts[j] == TtFonts[i])
			  flag = 1;
		       else
			  j++;
		    }

		  /* Shall we free this family ? */
		  if (j == MAX_FONT)
#                   ifdef _WINDOWS
            if (TtDisplay)
               SelectObject (TtDisplay, (HFONT)0);
		    if (!DeleteObject (TtFonts[i]->FiFont))
		      WinErrorBox (WIN_Main_Wd);
            TtFonts[i]->FiFont = (HFONT)0;
		    TtaFreeMemory (TtFonts[i]);
#                   else  /* _WINDOWS */
		    XFreeFont (TtDisplay, (XFontStruct *) TtFonts[i]);
#                   endif /* _WINDOWS */
		    TtFonts[i] = NULL;
	       }
	     else
		TtFontFrames[i] = TtFontFrames[i] & (~mask);
	     i++;
	  }

	/* pack the font table */
	j = FirstRemovableFont;
	i--;
	while (j < i)
	  {
	     while (TtFonts[j] != NULL)
	       {
		  j++;
		  /* skip the used entries */
	       }
	     while (TtFonts[i] == NULL)
	       {
		  i--;
		  /* skip the empty entries */
	       }
	     if (j < i)
	       {
		  TtFonts[j] = TtFonts[i];
		  TtFonts[i] = NULL;
		  TtFontFrames[j] = TtFontFrames[i];
		  strcpy (&TtFontName[j * MAX_FONTNAME], &TtFontName[i * MAX_FONTNAME]);
		  i--;
		  j++;
	       }
	  }
     }
}
