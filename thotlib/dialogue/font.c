/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2001.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 
/*
 * Module dedicated to font handling.
 *
 * Authors: I. Vatton (INRIA)
 *          R. Guetari (W3C/INRIA) Windows version
 *          D. Veillard (W3C/INRIA): Windows NT/95 routines
 *
 */

#include "thot_sys.h"
#include "libmsg.h"
#include "message.h"
#include "constmedia.h"
#include "typemedia.h"
#include "frame.h"

/*  tolower(c) was a macro defined in ctypes.h that returns
   something wrong if c is not an upper case letter. */
#define TOLOWER(c)	(isupper(c)? tolower(c) : (c))

#define THOT_EXPORT extern
#include "frame_tv.h"
#include "units_tv.h"
#include "edit_tv.h"
#undef THOT_EXPORT
#define THOT_EXPORT
#include "font_tv.h"
/* that table for the charSThotLoacter glyphs */
static int          FirstRemovableFont = 1;
static char         StylesTable[MAX_HIGHLIGHT] = "rbiogq";

/* Maximum number of font size handled */
static int          MaxNumberOfSizes;
static int          LogicalPointsSizes[MAX_LOG_SIZE] =
{6, 8, 10, 12, 14, 16, 20, 24, 30, 40, 60};
static char*        FontFamily;
static ThotBool     UseLucidaFamily;
static ThotBool     UseAdobeFamily;

#ifdef _WINDOWS
typedef struct FontCharacteristics {
        int   highlight; 
        int   size;
        char  alphabet; 
        char  family; 
}FontCharacteristics;

typedef FontCharacteristics* ptrFC;

static ptrFC LastUsedFont = (ptrFC)0;
static HFONT OldFont;
#endif /* _WINDOWS */

#ifdef _I18N_
static SpecFont   FirstFontSel = NULL;
#endif /* _I18N_ */

#include "buildlines_f.h"
#include "registry_f.h"
#include "language_f.h"
#include "font_f.h"
#include "memory_f.h"
#include "registry_f.h"
#include "units_f.h"
#include "windowdisplay_f.h"

#ifdef _WINDOWS
#include "wininclude.h"

/*----------------------------------------------------------------------
  WIN_LoadFont :  load a Windows TrueType with a defined set of
  characteristics.
  ----------------------------------------------------------------------*/
static HFONT WIN_LoadFont (char alphabet, char family, int highlight,
			   int size, TypeUnit unit)
{
   HFONT hFont;
   char  lpszFace[MAX_LENGTH];
   int   nHeight;
   int   nWidth;
   int   fnWeight;
   int   fdwItalic;
   int   fdwUnderline;
   int   fdwStrikeOut;

   nHeight = 0;
   nWidth = 0;
   fnWeight = FW_NORMAL;
   fdwItalic = FALSE;
   fdwUnderline = FALSE;
   fdwStrikeOut = FALSE;

   if (alphabet != 'L' && alphabet != 'G' && alphabet != 'g')
      return NULL;
   if (alphabet == 'g' || alphabet == 'G')
     {
      family    = 's';
      highlight = 0 ;
     }

   switch (family)
     {
     case 'T':
     case 't':
       sprintf (&lpszFace[0], "Times New Roman");
       break;

     case 'H':
     case 'h':
       sprintf (&lpszFace[0], "Arial");
       break;

     case 'C':
     case 'c':
       sprintf (&lpszFace[0], "Courier New");
       break;

     case 'S':
     case 's':
       sprintf (&lpszFace[0], "Symbol");
       break;

     default:
       return NULL;
     }

   switch (StylesTable[highlight])
     {
     case 'r':
       break;

     case 'i':
     case 'o':
       fdwItalic = TRUE;
       break;

     case 'b':
     case 'g':
     case 'q':
       fnWeight = FW_BOLD;
       break;

     default:
       return NULL;
     }

   nHeight = -MulDiv(size, DOT_PER_INCH, 72);
   hFont = CreateFont (nHeight, nWidth, 0, 0, fnWeight,
                       fdwItalic, fdwUnderline, fdwStrikeOut,
                       DEFAULT_CHARSET, OUT_TT_ONLY_PRECIS, CLIP_DEFAULT_PRECIS,
                       PROOF_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
                       lpszFace);
   if (hFont == (HFONT)0)
	WinErrorBox (NULL, "CreateFont");

   return (hFont);
}

/*----------------------------------------------------------------------
  WinLoadFont : Load a Windows font in a Device context.
  ----------------------------------------------------------------------*/
HFONT WinLoadFont (HDC hdc, PtrFont font)
{
  if (LastUsedFont == (ptrFC)0)
    {
      LastUsedFont = (ptrFC) TtaGetMemory (sizeof (FontCharacteristics));
      LastUsedFont->highlight = font->highlight; 
      LastUsedFont->size = font->size;
      LastUsedFont->alphabet = font->alphabet; 
      LastUsedFont->family = font->family; 
      
      if (ActiveFont != (HFONT)0)
	{
	  if (!DeleteObject (SelectObject (hdc, GetStockObject (SYSTEM_FONT))))
            WinErrorBox (NULL, "WinLoadFont (1)");
	  ActiveFont = (HFONT)0;
	}
    }
  else if (LastUsedFont->highlight != font->highlight ||
	   LastUsedFont->size != font->size ||
	   LastUsedFont->alphabet != font->alphabet ||
	   LastUsedFont->family != font->family)
    {
    if (ActiveFont != (HFONT)0)
      {
	SelectObject (hdc, GetStockObject (SYSTEM_FONT));
	DeleteObject (ActiveFont);
	ActiveFont = (HFONT)0;
	LastUsedFont->highlight = font->highlight; 
	LastUsedFont->size      = font->size;
	LastUsedFont->alphabet  = font->alphabet; 
	LastUsedFont->family    = font->family; 
      } 
   }

   ActiveFont = WIN_LoadFont (font->alphabet, font->family,
				 font->highlight, font->size, 0);
  return (OldFont = SelectObject (hdc, ActiveFont));
}
#endif /* _WINDOWS */


/*----------------------------------------------------------------------
  NumberOfFonts returns the number of fonts.
  ----------------------------------------------------------------------*/
int NumberOfFonts ()
{
  return MaxNumberOfSizes + 1;
}

/*----------------------------------------------------------------------
  GetCharsCapacity converts from pixel volume to char size
  ----------------------------------------------------------------------*/
int GetCharsCapacity (int volpixel)
{
  return volpixel / 200;
}


/*----------------------------------------------------------------------
  CharacterWidth returns the width of a char in a given font.
  ----------------------------------------------------------------------*/
int CharacterWidth (unsigned char c, PtrFont font)
{
#if !defined(_WINDOWS) && !defined(_GTK)
  XFontStruct        *xf;
  int                 i;
#endif /* !defined(_WINDOWS) && !defined(_GTK) */
  int
                 l;
  if (font == NULL)
    return (0);
  else
    {
      if (c == START_ENTITY)
	c = '&';
      else if (c == NEW_LINE)
	/* characters NEW_LINE and BREAK_LINE are equivalent */
	c = BREAK_LINE;
      else if (c == TAB)
	/* we use the SPACE width for the character TAB */
	c = SPACE;
#ifdef _WINDOWS
      if (c == UNBREAKABLE_SPACE)
	l = font->FiWidths[32];
      else if (c == BREAK_LINE)
	l = 1;
      else if (c == THIN_SPACE)
	l = (font->FiWidths[32] + 3) / 4;
      else if (c == HALF_EM)
	l = (font->FiWidths[32] + 1) / 2;
      else
	l = font->FiWidths[c];
#else  /* _WINDOWS */
#ifdef _GTK
      l = gdk_char_width (font, c);
#else /* _GTK */
      xf = (XFontStruct *) font;
      if (xf->per_char == NULL)
	l = xf->max_bounds.width;
      else if (c < xf->min_char_or_byte2)
	l = 0;
      else
	{
	  if (c == UNBREAKABLE_SPACE)
	    l = xf->per_char[32 - xf->min_char_or_byte2].width;
	  else if (c == BREAK_LINE)
	    l = 1;
	  else if (c == THIN_SPACE)
	    l = (xf->per_char[32 - xf->min_char_or_byte2].width + 3) / 4;
	  else if (c == HALF_EM)
	    l = (xf->per_char[32 - xf->min_char_or_byte2].width + 1) / 2;
	  else if (c == 244)
	    {
	      /* a patch due to errors in standard symbol fonts */
	      i = 0;
	      while (i < MAX_FONT && font != TtFonts[i])
		i++;
	      if (TtPatchedFont[i] == 8 || TtPatchedFont[i] == 10)
		l = 1;
	      else if (TtPatchedFont[i] == 12 || TtPatchedFont[i] == 14)
		l = 2;
	      else if (TtPatchedFont[i] == 24)
		l = 4;
	      else
		l = xf->per_char[c - xf->min_char_or_byte2].width;
	    }
	  else
	    l = xf->per_char[c - xf->min_char_or_byte2].width;
	}
#endif /* _GTK */
#endif /* _WINDOWS */
     }
   return (l);
}


/*----------------------------------------------------------------------
  BoxCharacterWidth returns the width of a char in a given font
  ----------------------------------------------------------------------*/
int BoxCharacterWidth (CHAR_T c, SpecFont specfont)
{
#ifdef _I18N_
  PtrFont         font;
  unsigned char   car;

  car = GetFontAndIndexFromSpec (c, specfont, &font);
  if (font == NULL)
    return 3;
  else
    return CharacterWidth (car, font);
#else /* _I18N_ */
  return CharacterWidth (c, specfont);
#endif /* _I18N_ */
}

/*----------------------------------------------------------------------
  CharacterHeight returns the height of a char in a given font
  ----------------------------------------------------------------------*/
int CharacterHeight (unsigned char c, PtrFont font)
{
#ifdef _GTK
  int              l;
#endif /* _GTK */

  if (font == NULL)
    return (0);
#ifdef _WINDOWS
  else
    return (font->FiHeights[c]);
#else  /* _WINDOWS */
#ifdef _GTK
  else
    l = gdk_char_height (font, c);
    return (l);
#else /* _GTK */
  else if (((XFontStruct *) font)->per_char == NULL)
    return FontHeight (font);
  else
    return ((XFontStruct *) font)->per_char[c - ((XFontStruct *) font)->min_char_or_byte2].ascent
      + ((XFontStruct *) font)->per_char[c - ((XFontStruct *) font)->min_char_or_byte2].descent;
#endif /* _GTK */
#endif /* _WINDOWS */
}

/*----------------------------------------------------------------------
  CharacterAscent returns the ascent of a char in a given font.
  ----------------------------------------------------------------------*/
int CharacterAscent (unsigned char c, PtrFont font)
{
#ifdef _GTK
  int               lbearing, rbearing, width, ascent, descent;
#else /* _GTK */
#ifndef _WINDOWS
  XFontStruct      *xf;
  int               i;
#endif /* _WINDOWS */
#endif /* _GTK */

  if (font == NULL)
    return (0);
#ifdef _WINDOWS
  else
    return font->FiAscent;
#else  /* _WINDOWS */
#ifdef _GTK
  else
    {
      gdk_string_extents (font, &c, &lbearing, &rbearing, &width, &ascent, &descent);
      return (ascent);
    }
#else /* _GTK */
  else
    {
      xf = (XFontStruct *) font;
      /* a patch due to errors in standard symbol fonts */
      if (xf->per_char == NULL)
	return (xf->max_bounds.ascent);
      else if (c == 244)
	{
	  /* a patch due to errors in standard symbol fonts */
	  i = 0;
	  while (i < MAX_FONT && font != TtFonts[i])
	    i++;
	  if (TtPatchedFont[i])
	    return (xf->per_char[c - xf->min_char_or_byte2].ascent - 2);
	}
      return (xf->per_char[c - xf->min_char_or_byte2].ascent);
    }
#endif /* _GTK */
#endif /* !_WINDOWS */
}

/*----------------------------------------------------------------------
  FontAscent returns a global ascent for a font.
  ----------------------------------------------------------------------*/
int FontAscent (PtrFont font)
{
  if (font == NULL)
    return (0);
#ifdef _WINDOWS
  else
    return (font->FiAscent);
#else  /* _WINDOWS */
  else
#ifdef _GTK
    return (font->ascent);
#else /* _GTK */
    return (((XFontStruct *) font)->ascent);
#endif /* _GTK */
#endif /* _WINDOWS */
}

/*----------------------------------------------------------------------
  XFontAscent returns the x ascent in a font set.
  ----------------------------------------------------------------------*/
static int XFontAscent (SpecFont specfont)
{
#ifdef _I18N_
  PtrFont         font;
  unsigned char   car;

  car = GetFontAndIndexFromSpec (120, specfont, &font);
  if (font == NULL)
    font = FontDialogue;
  return CharacterAscent ('x', font);
#else /* _I18N_ */
  return CharacterAscent ('x', specfont);
#endif /* _I18N_ */
}

/*----------------------------------------------------------------------
  FontHeight returns the height of a given font.
  ----------------------------------------------------------------------*/
int FontHeight (PtrFont font)
{
#ifdef _GTK
  /* int lbearing, rbearing, width, ascent, descent;*/
  char c[]="Xp";
#endif /* _GTK */

  if (font == NULL)
    return (0);
#ifdef _WINDOWS
   else
     return (font->FiHeight);
#else  /* _WINDOWS */
#ifdef _GTK
  else
    return (gdk_string_height (font, c) + 3); /* need some extra space */
#else /* _GTK */
   else
     return ((XFontStruct *) font)->max_bounds.ascent + ((XFontStruct *) font)->max_bounds.descent;
#endif /* _GTK */
#endif /* !_WINDOWS */
}

/*----------------------------------------------------------------------
  BoxFontHeight returns the height of a given font.
  ----------------------------------------------------------------------*/
int BoxFontHeight (SpecFont specfont)
{
#ifdef _I18N_
  PtrFont         font;
  unsigned char   car;

  car = GetFontAndIndexFromSpec (120, specfont, &font);
  return FontHeight (font);
#else /* _I18N_ */
  return FontHeight (specfont);
#endif /* _I18N_ */
}

/*----------------------------------------------------------------------
  PixelValue computes the pixel size for a given logical unit.
  pAb is the current abstract box except for UnPercent unit
  where it holds the comparison value.
  ----------------------------------------------------------------------*/
int PixelValue (int val, TypeUnit unit, PtrAbstractBox pAb, int zoom)
{
   int              dist, i;

   dist = 0;
   switch (unit)
     {
     case UnRelative:
       if (pAb == NULL || pAb->AbBox == NULL || pAb->AbBox->BxFont == NULL)
	 dist = 0;
       else
	 dist = (val * BoxFontHeight (pAb->AbBox->BxFont) + 5) / 10;
       break;
     case UnXHeight:
       if (pAb == NULL || pAb->AbBox == NULL || pAb->AbBox->BxFont == NULL)
	 dist = 0;
       else
	 dist = (val * XFontAscent (pAb->AbBox->BxFont)) / 10;
       break;
     case UnPoint:
       /* take zoom into account */
       if (zoom != 0)
	 {
	   dist = val + (val * zoom / 10);
	   if (dist == 0 && val > 0)
	     dist = 1;
	 }
       else
	 dist = val;
#ifndef _WIN_PRINT
       if (!Printing)
	 /* Postscript unit is the point instead of the pixel */
#endif /* _WIN_PRINT */
	 dist = PointToPixel (dist);
       break;
     case UnPixel:
       /* take zoom into account */
       if (zoom != 0)
	 {
	   dist = val + (val * zoom / 10);
	   if (dist == 0 && val > 0)
	     dist = 1;
	 }
       else
	 dist = val;
#ifdef _WIN_PRINT
       if (TtPrinterDC && ScreenDPI)
	 dist = (dist * PrinterDPI + ScreenDPI / 2) / ScreenDPI;
#else /* _WIN_PRINT */
       if (Printing)
	 /* Postscript unit is the point instead of the pixel */
	 dist = PixelToPoint (dist);
#endif /* _WIN_PRINT */
       break;
     case UnPercent:
       i = val * (int) pAb;
       dist = i / 100;
       break;
     case UnAuto:
       /* should not occur: reserved for margins */
       break;
     }
  return (dist);
}

/*----------------------------------------------------------------------
  LogicalValue computes the logical value for a given pixel size.
  pAb is the current abstract box except for UnPercent unit
  where it holds the comparison value.
  ----------------------------------------------------------------------*/
int LogicalValue (int val, TypeUnit unit, PtrAbstractBox pAb, int zoom)
{
   int              dist, i;

   dist = 0;
   switch (unit)
     {
     case UnRelative:
       if (pAb == NULL || pAb->AbBox == NULL || pAb->AbBox->BxFont == NULL)
	 dist = 0;
       else
	 dist = val * 10 / BoxFontHeight (pAb->AbBox->BxFont);
       break;
     case UnXHeight:
       if (pAb == NULL || pAb->AbBox == NULL || pAb->AbBox->BxFont == NULL)
	 dist = 0;
       else
	 dist = val * 10 / XFontAscent (pAb->AbBox->BxFont);
       break;
     case UnPoint:
       /* take zoom into account */
       if (zoom != 0)
	 {
	   dist = val - (val * zoom / 10);
	   if (dist == 0 && val > 0)
	     dist = 1;
	 }
       else
	 dist = val;
#ifndef _WIN_PRINT
       if (!Printing)
	 /* Postscript unit is the point instead of the pixel */
#endif /* _WIN_PRINT */
	 dist = PixelToPoint (dist);
       break;
     case UnPixel:
       /* take zoom into account */
       if (zoom != 0)
	 {
	   dist = val - (val * zoom / 10);
	   if (dist == 0 && val > 0)
	     dist = 1;
	 }
       else
	 dist = val;
#ifdef _WIN_PRINT
       if (TtPrinterDC && PrinterDPI)
	 dist = (dist * ScreenDPI + PrinterDPI / 2) / PrinterDPI;
#else /* _WIN_PRINT */
      if (Printing)
	 /* Postscript unit is the point instead of the pixel */
	 dist = PixelToPoint (dist);
#endif /* _WIN_PRINT */
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
     case UnAuto:
       /* Should not occur. Auto is reserved for margins */
       break;
     }
   return (dist);
}


/*----------------------------------------------------------------------
  FontBase returns the shifting of the base line for a given font.
  ----------------------------------------------------------------------*/
int FontBase (PtrFont font)
{
   if (font == NULL)
      return (0);
   else
      return (FontAscent (font));
}

/*----------------------------------------------------------------------
  BoxFontBase returns the base of a font set.
  ----------------------------------------------------------------------*/
int BoxFontBase (SpecFont specfont)
{
#ifdef _I18N_
  PtrFont         font;
  unsigned char   car;

  car = GetFontAndIndexFromSpec (120, specfont, &font);
  return FontBase (font);
#else /* _I18N_ */
  return FontBase (specfont);
#endif /* _I18N_ */
}

/*----------------------------------------------------------------------
  FontRelSize converts between a size in points and the logical size.
  ----------------------------------------------------------------------*/
int FontRelSize (int size)
{
   int                 j;

   j = 0;
   while ((size > LogicalPointsSizes[j]) && (j < MaxNumberOfSizes))
      j++;

   return (j);
}

/*----------------------------------------------------------------------
  FontPointSize convert a logical size to the point value.
  ----------------------------------------------------------------------*/
int FontPointSize (int size)
{

   if (size > MaxNumberOfSizes)
      size = MaxNumberOfSizes;
   else if (size < 0)
      size = 0;

   return (LogicalPointsSizes[size]);
}

#ifndef _WINDOWS
/*----------------------------------------------------------------------
  LoadFont load a given font designed by its name.
  ----------------------------------------------------------------------*/
PtrFont LoadFont (char *name)
{
#ifdef _GTK
  GdkFont *result;

  result = gdk_font_load ((gchar *)name);
  return (result);
#else /* _GTK */
  return ((PtrFont) XLoadQueryFont (TtDisplay, name));
#endif /* _GTK */
}
#  endif /* !_WINDOWS */

/*----------------------------------------------------------------------
  FontIdentifier computes the name of a Thot font.
  ----------------------------------------------------------------------*/
void FontIdentifier (char alphabet, char family, int highlight, int size,
		     TypeUnit unit, char r_name[10], char r_nameX[100])
{
  if (highlight > MAX_HIGHLIGHT)
    highlight = MAX_HIGHLIGHT;
  if (alphabet == 'g' || alphabet == 'G')
    {
      highlight = 0;		/* roman only for symbols */
      family = 's';		/* times only for symbols */
      strcpy (r_nameX, "-");
      strcat (r_nameX, "*");
    }
  else
    strcpy (r_nameX, FontFamily);

  /* apply the current font zoom */
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
      switch (TOLOWER (family))
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
  else if (UseAdobeFamily)
    {
      strcat (r_nameX, "-");
      switch (TOLOWER (family))
	{
	case 't':
	  strcat (r_nameX, "new century schoolbook");
	  break;
	case 'h':
	  strcat (r_nameX, "helvetica");
	  break;
	case 'c':
	  strcat (r_nameX, "courier");
	  break;
	case 's':
	  strcat (r_nameX, "Symbol");
	  break;
	default:
	  strcat (r_nameX, "*");
	  break;
	}
    }
  else
    {
      strcat (r_nameX, "-");
      switch (TOLOWER (family))
	{
	case 't':
	  strcat (r_nameX, "times");
	  break;
	case 'h':
	  strcat (r_nameX, "helvetica");
	  break;
	case 'c':
	  strcat (r_nameX, "courier");
	  break;
	case 's':
	  strcat (r_nameX, "Symbol");
	  break;
	default:
	  strcat (r_nameX, "*");
	}
    }
  
  strcat (r_nameX, "-");
  switch (TOLOWER (StylesTable[highlight]))
    {
    case 'r':
      strcat (r_nameX, "medium-r");
      break;
    case 'i':
    case 'o':
      if (TOLOWER (family) == 'h' || TOLOWER (family) == 'c')
	strcat (r_nameX, "medium-o");
      else
	strcat (r_nameX, "medium-i");
      break;
    case 'b':
      if (UseLucidaFamily && TOLOWER (family) == 't')
	strcat (r_nameX, "demibold-r");
      else
	strcat (r_nameX, "bold-r");
      break;
    case 'g':
    case 'q':
      if (UseLucidaFamily && TOLOWER (family) == 't')
	strcat (r_nameX, "demibold-i");
      else if (TOLOWER (family) == 'h' || TOLOWER (family) == 'c')
	strcat (r_nameX, "bold-o");
      else
	strcat (r_nameX, "bold-i");
      break;
    }

  strcat (r_nameX, "-");
  if (TOLOWER (family) == 'h')
    strcat (r_nameX, "normal");  /* narrow helvetica does not exist */
  else
    strcat (r_nameX, "*");
  if (TOLOWER (family) == 's')
    sprintf (r_nameX, "%s-*-%d-*-75-75-p-*-*-fontspecific", r_nameX, size);
  else
    {
      sprintf (r_nameX, "%s-*-%d-*-75-75", r_nameX, size);
      strcat (r_nameX, "-");
      if (TOLOWER (family) == 'c')
	strcat (r_nameX, "m");
      else
	strcat (r_nameX, "p");
      strcat (r_nameX, "-*-");
      if (TOLOWER (alphabet) == 'l')
	strcat (r_nameX, "iso8859-1");
      else if (TOLOWER (alphabet) == 'e' || alphabet == '2')
	strcat (r_nameX, "iso8859-2");
      else if (TOLOWER (alphabet) == 'g')
	strcat (r_nameX, "*-fontspecific");	/*adobe */
      else
	{
	  strcat (r_nameX, "iso8859-1");
	  /* replace '1' by current alphabet */
	  r_nameX[strlen (r_nameX) -1] = alphabet;
	}
    }

  sprintf (r_name, "%c%c%c%d", TOLOWER (alphabet), TOLOWER (family),
	   StylesTable[highlight], size);
}

/*----------------------------------------------------------------------
  ReadFont do a raw Thot font loading (bypasses the font cache).
  ----------------------------------------------------------------------*/
PtrFont ReadFont (char alphabet, char family, int highlight, int size,
		  TypeUnit unit)
{
  char             name[10], nameX[100];

  FontIdentifier (alphabet, family, highlight, size, unit, name, nameX);
#ifndef _WINDOWS
  return LoadFont (nameX);
#else  /* _WINDOWS */
  return NULL;
#endif /* _WINDOWS */
}

/*----------------------------------------------------------------------
  LoadNearestFont load the nearest possible font given a set of attributes
  like alphabet, family, the size and for a given frame.
  ----------------------------------------------------------------------*/
static PtrFont LoadNearestFont (char alphabet, char family, int highlight,
				int size, TypeUnit unit, int frame,
				ThotBool increase)
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
  PtrFont             ptfont;

  /* use only standard sizes */
  index = 0;
  if (unit == UnRelative)
    index = size;
  else
    {
      /* nearest standard size lookup */
      index = 0;
      while (LogicalPointsSizes[index] < size && index <= MaxNumberOfSizes)
	index++;
    }
   
  FontIdentifier (alphabet, family, highlight, index, FALSE, text, textX);
  /* initialize the PostScript font name */
  strcpy (PsName, text);
   
  /* Font cache lookup */
  j = 0;
  i = 0;
  deb = 0;
  ptfont = NULL;
  while (ptfont == NULL && i < MAX_FONT && TtFonts[i] != NULL)
    {
      if (strcmp (&TtFontName[deb], text) == 0)
	  {
	  /* Font cache lookup succeeded */
	  ptfont = TtFonts[i];
	  }
      else
	  {
      i++;
      deb += MAX_FONTNAME;
	  }
    }
   
  /* Load a new font */
  if (ptfont == NULL)
    {
      /* Check for table font overflow */
      if (i >= MAX_FONT)
	TtaDisplayMessage (INFO, TtaGetMessage (LIB, TMSG_NO_MEMORY),
			   textX);
      else
	{
	  strcpy (&TtFontName[deb], text);
	  strcpy (&TtPsFontName[i * 8], PsName);
	   
#ifdef _WINDOWS
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
	  ptfont->alphabet  = alphabet;
	  ptfont->family    = family;
	  ptfont->highlight = highlight;
	  ptfont->size      = size;
	  ActiveFont = WIN_LoadFont (alphabet, family, highlight,
				     size, unit);
	  if (TtPrinterDC != 0)
	    {
	      hOldFont = SelectObject (TtPrinterDC, ActiveFont);
	      if (GetTextMetrics (TtPrinterDC, &textMetric))
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
		  GetTextExtentPoint (TtPrinterDC, (LPCTSTR) (&c),
				      1, (LPSIZE) (&wsize));
		  ptfont->FiWidths[c] = wsize.cx;
		  ptfont->FiHeights[c] = wsize.cy;
		}
	      SelectObject (TtPrinterDC, hOldFont);
	    }
	  else
	    { 
	      hOldFont = SelectObject (TtDisplay, ActiveFont);
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
		  GetTextExtentPoint (TtDisplay, (LPCTSTR) (&c),
				      1, (LPSIZE) (&wsize));
		  ptfont->FiWidths[c] = wsize.cx;
		  ptfont->FiHeights[c] = wsize.cy;
		}
	      /* SelectObject (TtDisplay, hOldFont); */
	      if (!DeleteObject (SelectObject (TtDisplay, ActiveFont)))
		WinErrorBox (NULL, "LoadNearestFont (1)");
	      ActiveFont = 0;
	    }
#else  /* _WINDOWS */
	  ptfont = LoadFont (textX);
#endif /* !_WINDOWS */
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
		ptfont = LoadNearestFont (alphabet, family,
					  highlight, index,
					  FALSE, frame, increase);
	      else if (index >= MaxNumberOfSizes)
		ptfont = LoadNearestFont (alphabet, family, highlight,
					  MaxNumberOfSizes, FALSE,
					  frame, FALSE);
	      if (ptfont == NULL)
		TtaDisplayMessage (INFO,
				   TtaGetMessage (LIB, TMSG_MISSING_FONT),
				   textX);
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
#if !defined(_WINDOWS) && !defined(_GTK)
	  size = LogicalPointsSizes[index];
	  if (alphabet == 'G' &&
	      (size == 8 || size == 10 || size == 12 ||
	       size == 14 || size == 24))
	    TtPatchedFont[i] = size;
#endif /* !_WINDOWS && !_GTK */
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
  GetFontAndIndexFromSpec return the glyph index and the font
  used to display the wide character c;
  ----------------------------------------------------------------------*/
unsigned char GetFontAndIndexFromSpec (CHAR_T c,
				       SpecFont fontset,
				       PtrFont *font)
{
#ifdef _I18N_
  PtrFont            lfont;
  unsigned char      car;
  int                mask, frame;

  lfont = NULL;
  if (fontset)
    {
      if (c < 255)
	{
	  lfont = fontset->FontIso_1;
	  car = c;
	}
      else if (c >= 880 && c < 1023)
	{
	  if (fontset->FontIso_7 == NULL)
	    {
	      /* load that font */
	      for (frame = 1; frame <= MAX_FRAME; frame++)
		{
		  mask = 1 << (frame - 1);
		  if (fontset->FontMask | mask)
		    lfont = LoadNearestFont ('G', fontset->FontFamily,
					     fontset->FontHighlight,
					     fontset->FontSize, UnPoint,
					     frame, TRUE);
		}
	      fontset->FontIso_7 = lfont;
	    }
	  else
	    lfont = fontset->FontIso_7;
	  car = TtaGetCharFromWC (c, ISO_8859_7);
	}
    }
  /* when any font is available */
  if (lfont == NULL)
    /* generate a square */
    car = UNDISPLAYED_UNICODE;
  *font = lfont;
  return car;
#else /* _I18N_ */
  *font = fontset;
  return c;
#endif /* _I18N_ */
}

#ifdef _I18N_
/*----------------------------------------------------------------------
  RemoveFontInFontSets removes a font from all font sets.
  ----------------------------------------------------------------------*/
static void RemoveFontInFontSets (PtrFont font, int mask)
{
  SpecFont            prevset, fontset, nextset;
  ThotBool            used;

  fontset = FirstFontSel;
  prevset = NULL;
  used = FALSE;
  while (fontset)
    {
      if (fontset->FontIso_1 == font)
	fontset->FontIso_1 = NULL;
      else
	used = (used || fontset->FontIso_1);
      if (fontset->FontIso_2 == font)
	fontset->FontIso_2 = NULL;
      else
	used = (used || fontset->FontIso_2);
      if (fontset->FontIso_3 == font)
	fontset->FontIso_3 = NULL;
      else
	used = (used || fontset->FontIso_3);
      if (fontset->FontIso_4 == font)
	fontset->FontIso_4 = NULL;
      else
	used = (used || fontset->FontIso_4);
      if (fontset->FontIso_5 == font)
	fontset->FontIso_5 = NULL;
      else
	used = (used || fontset->FontIso_5);
      if (fontset->FontIso_6 == font)
	fontset->FontIso_6 = NULL;
      else
	used = (used || fontset->FontIso_6);
      if (fontset->FontIso_7 == font)
	fontset->FontIso_7 = NULL;
      else
	used = (used || fontset->FontIso_7);
      if (fontset->FontIso_8 == font)
	fontset->FontIso_8 = NULL;
      else
	used = (used || fontset->FontIso_8);
      if (fontset->FontIso_9 == font)
	fontset->FontIso_9 = NULL;
      else
	used = (used || fontset->FontIso_9);

      /* next set */
      nextset = fontset->NextFontSet;
      /* is it still in use? */
      if (used)
	prevset = fontset;
      else
	{
	  /* free this fontset */
	  TtaFreeMemory (fontset);
	  if (prevset)
	    prevset->NextFontSet = nextset;
	  else
	    FirstFontSel = nextset;
	}
      fontset = nextset;
    }
}
#endif /* _I18N_ */

/*----------------------------------------------------------------------
  LoadFontSet allocate a font set and load the ISO-latin-1 font.
  ----------------------------------------------------------------------*/
static SpecFont LoadFontSet (char alphabet, char family, int highlight,
			     int size, TypeUnit unit, int frame,
			     ThotBool increase)
{
#ifdef _I18N_
  SpecFont            fontset;
  int                 mask;

  /* look for the fontsel */
  fontset = FirstFontSel;
  while (fontset &&
	 (fontset->FontFamily != family &&
	  fontset->FontHighlight != highlight &&
	  fontset->FontSize != size))
    fontset = fontset->NextFontSet;
  if (fontset == NULL)
    {
      /* create a new set */
      fontset = TtaGetMemory (sizeof (FontSet));
      if (fontset)
	{
	  memset (fontset, 0, sizeof (FontSet));
	  fontset->FontFamily = family;
	  fontset->FontHighlight = highlight;
	  fontset->FontSize = size;
	  fontset->FontIso_1 = LoadNearestFont (alphabet, family, highlight,
						size, unit, frame, increase);
	}
      else
	fontset = FirstFontSel;
    }

  /* Compute window frame */
  mask = 1 << (frame - 1);
  /* store window frame number */
  fontset->FontFrames = fontset->FontFrames | mask;
#else /* _I18N_ */
  return LoadNearestFont (alphabet, family, highlight, size, unit,
			  frame, increase);
#endif /* _I18N_ */
}

/*----------------------------------------------------------------------
  ThotLoadFont try to load a font given a set of attributes like alphabet,
  family, the size and for a given frame.
  ----------------------------------------------------------------------*/
SpecFont ThotLoadFont (char alphabet, char family, int highlight, int size,
		       TypeUnit unit, int frame)
{
  if (unit == UnPixel)
    {
     if (Printing)
#ifdef _WIN_PRINT
	  size = (size * 72 + ScreenDPI / 2) / ScreenDPI;
#else /* _WIN_PRINT */
	  /* adjust the font size to the printer definition */
	  size = (size * 72 + DOT_PER_INCH / 2) / DOT_PER_INCH;
#endif /* _WIN_PRINT */
     else
	  size = LogicalValue (size, UnPoint, NULL, 0);
      unit = UnPoint;
    }
  else if (unit == UnXHeight || unit == UnPercent)
    /* what does this mean??? set default size: 12 pt */
    {
      size = 12;
      unit = UnPoint;
    }

  /* take the zoom into account */
   if (frame && FontZoom)
     {
       if (unit == UnPoint)
	 size = size + (2 * FontZoom);
       else
	 size = size + FontZoom;
     }

   /* the minimum size is 6 points */
   if (size < 6 && unit == UnPoint)
      size = 6;
   return LoadFontSet (alphabet, family, highlight, size, unit, frame, TRUE);
}

/*----------------------------------------------------------------------
  TtaSetFontZoom
  Updates the font zoom global varialbe
  ----------------------------------------------------------------------*/
void TtaSetFontZoom (int zoom)
{
  FontZoom = zoom;
}

/*----------------------------------------------------------------------
  InitDialogueFonts initialize the standard fonts used by the Thot Toolkit.
  ----------------------------------------------------------------------*/
void InitDialogueFonts (char *name)
{
#ifndef _GTK
#ifndef _WINDOWS
  int              ndir, ncurrent;
  char             FONT_PATH[128];
  char            *fontpath;
#endif /* _WINDOWS */
  char           **dirlist = NULL;
  char           **currentlist = NULL;
#endif /* !_GTK */
  char            *value;
  char             alphabet;
  int              f3;
  int              i;

  /* is there a predefined font family ? */
  MenuSize = 12;
  alphabet = TtaGetAlphabet (TtaGetDefaultLanguage ());
  /* initialize the font zoom */
  TtaGetEnvInt ("ZOOM", &FontZoom);
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
      strcpy (FontFamily, "-");
      strcat (FontFamily, value);
      if (!Printing && !strcmp (FontFamily, "-b&h-lucida"))
	UseLucidaFamily = TRUE;
      else
	{
	  UseLucidaFamily = FALSE;
	  if (!Printing && !strcmp (FontFamily, "-adobe"))
	    UseAdobeFamily = TRUE;
	  else
	    UseAdobeFamily = FALSE;
	}
    }

  /* Is there any predefined size for menu fonts ? */
  value = TtaGetEnvString ("FontMenuSize");
  if (value != NULL)
    sscanf (value, "%d", &MenuSize);
  f3 = MenuSize + 2;
  
#ifndef _WINDOWS
#ifndef _GTK
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
	  dirlist = (char**) TtaGetMemory (ndir * sizeof(char*));
	  
	  if (currentlist != NULL)
#ifdef SYSV
	    memcpy (dirlist, currentlist, ncurrent * sizeof (char*));
#else /* SYSV */
	    bcopy (currentlist, dirlist, ncurrent * sizeof (char*));
#endif /* SYSV */
	  dirlist[ncurrent] = FONT_PATH;
	  XSetFontPath (TtDisplay, dirlist, ndir);
	  TtaFreeMemory ( dirlist);
	}
      TtaFreeMemory ( currentlist);
    }
#ifndef _GTK
  for (i = 0; i < MAX_FONT; i++)
    TtPatchedFont[i] = 0;
#endif /* _GTK */
#endif /* !_GTK */
#endif /* _WINDOWS */

  /* Initialize the Thot Lib standards fonts */
  FontDialogue = IFontDialogue = LargeFontDialogue = NULL;
  SymbolIcons = NULL;
  GraphicsIcons = NULL;
  SmallFontDialogue = NULL;

  /* Initialize the font table */
  for (i = 0; i < MAX_FONT; i++)
    TtFonts[i] = NULL;
  /* load first five predefined fonts */
  FontDialogue =  LoadNearestFont (alphabet, 't', 0, MenuSize, UnPoint,
				   0, TRUE);
  if (FontDialogue == NULL)
    {
      FontDialogue = LoadNearestFont (alphabet, 'l', 0, MenuSize, UnPoint,
				      0, TRUE);
      if (FontDialogue == NULL)
	TtaDisplaySimpleMessage (FATAL, LIB, TMSG_MISSING_FONT);
    }

  IFontDialogue = LoadNearestFont (alphabet, 't', 2, MenuSize, UnPoint,
				   0, TRUE);
  if (IFontDialogue == NULL)
    {
      IFontDialogue = LoadNearestFont (alphabet, 'l', 2, MenuSize, UnPoint,
				       0, TRUE);
      if (IFontDialogue == NULL)
	IFontDialogue = FontDialogue;
    }

  LargeFontDialogue = LoadNearestFont (alphabet, 't', 1, f3, UnPoint,
				       0, TRUE);
  if (LargeFontDialogue == NULL)
    {
      LargeFontDialogue = LoadNearestFont (alphabet, 't', 1, f3, UnPoint,
					   0, TRUE);
      if (LargeFontDialogue == NULL)
	LargeFontDialogue = IFontDialogue;
    }
  FirstRemovableFont = 3;
}

/*----------------------------------------------------------------------
 *      ThotFreeFont free the font familly loaded by a frame.
  ----------------------------------------------------------------------*/
void ThotFreeFont (int frame)
{
  /* TODO : Free the gtk fonts */
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
#ifdef _WINDOWS
	      if (j == MAX_FONT)
		{
		  DeleteObject (SelectObject (TtDisplay, ActiveFont));
		  TtaFreeMemory (TtFonts[i]);
		}
#else  /* _WINDOWS */
#ifdef _GTK
	      if (j == MAX_FONT)
	        gdk_font_unref (TtFonts[i]);
#else /* _GTK */
	      if (j == MAX_FONT)
		{
		  /* remove the indicator */
		  if (TtPatchedFont[i])
		    TtPatchedFont[i] = 0;
		  XFreeFont (TtDisplay, (XFontStruct *) TtFonts[i]);
		}
#endif /* _GTK */
#endif /* _WINDOWS */
#ifdef _I18N_
	      /* remove this font in fontsets */
	      RemoveFontInFontSets (TtFonts[i], mask);
#endif /* _I18N_ */
	      TtFonts[i] = NULL;
	      TtFontFrames[i] = 0;
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

/*----------------------------------------------------------------------
  ThotFreeAllFonts
  ----------------------------------------------------------------------*/
void ThotFreeAllFonts (void)
{
   TtaFreeMemory (FontFamily);
   TtaFreeMemory (FontDialogue);
   TtaFreeMemory (IFontDialogue); 
   TtaFreeMemory (LargeFontDialogue);
}









