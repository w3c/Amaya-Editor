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
static char        *FontFamily;
static char         GreekFontAlphabet;
static ThotBool     UseLucidaFamily;
static ThotBool     UseAdobeFamily;
#ifdef _WINDOWS
typedef struct FontCharacteristics {
        int   highlight; 
        int   size;
        int   family; 
        char  alphabet; 
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
static HFONT WIN_LoadFont (char alphabet, int family, int highlight,
			   int size)
{
   HFONT      hFont;
   DWORD      charset;
   char       lpszFace[MAX_LENGTH];
   int        nHeight;
   int        nWidth;
   int        fnWeight;
   int        fdwItalic;
   int        fdwUnderline;
   int        fdwStrikeOut;

   nHeight = 0;
   nWidth = 0;
   fnWeight = FW_NORMAL;
   fdwItalic = FALSE;
   fdwUnderline = FALSE;
   fdwStrikeOut = FALSE;

   switch (alphabet)
     {
     case 'G':
       charset = DEFAULT_CHARSET;/*SYMBOL_CHARSET;*/
       family = 0;
	 break;
     case '2':
       charset = DEFAULT_CHARSET;
       break;
     case '3':
       charset = DEFAULT_CHARSET;
       break;
     case '4':
       charset = DEFAULT_CHARSET;
       break;
     case '5':
       charset = RUSSIAN_CHARSET;
       break;
     case '6':
       charset = ARABIC_CHARSET;
       break;
     case '7':
       charset = GREEK_CHARSET;
       break;
     case '8':
       charset = HEBREW_CHARSET;
       break;
 	 default:
       charset = DEFAULT_CHARSET;
       break;
     }

   switch (family)
     {
     case 0:
       sprintf (&lpszFace[0], "Symbol");
       break;
     case 1:
       sprintf (&lpszFace[0], "Times New Roman");
       break;
     case 2:
       sprintf (&lpszFace[0], "Arial");
       break;
     case 3:
       sprintf (&lpszFace[0], "Courier New");
       break;
     default:
        sprintf (&lpszFace[0], "Arial");
     }

   switch (highlight)
     {
     case 0:
       break;
     case 2:
     case 3:
       fdwItalic = TRUE;
       break;
     case 1:
     case 4:
     case 5:
       fnWeight = FW_BOLD;
       break;
     default:
       break;
     }

   nHeight = -MulDiv(size, DOT_PER_INCH, 72);
   hFont = CreateFont (nHeight, nWidth, 0, 0, fnWeight,
                       fdwItalic, fdwUnderline, fdwStrikeOut,
                       charset, OUT_TT_ONLY_PRECIS, CLIP_DEFAULT_PRECIS,
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
				 font->highlight, font->size);
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
    return 6;
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
#  endif /* _WINDOWS */

/*----------------------------------------------------------------------
  FontIdentifier computes the name of a Thot font.
  ----------------------------------------------------------------------*/
void FontIdentifier (char alphabet, int family, int highlight, int size,
		     TypeUnit unit, char r_name[10], char r_nameX[100])
{
  char        *cfamily = "sthc";
  char        *wght, *slant, *ffamily;

  /* apply the current font zoom */
  if (unit == UnRelative)
    {
      /* La size est relative */
      if (size > MaxNumberOfSizes)
	size = LogicalPointsSizes[MaxNumberOfSizes];
      else if (size >= 0)
	size = LogicalPointsSizes[size];
    }
  else if (unit == UnPixel)
    size = PixelToPoint (size);

  if (alphabet != 'L' && alphabet != 'G')
    {
      ffamily = "-*-*";
      if (highlight > MAX_HIGHLIGHT)
	wght = "*";
      else if (highlight == 0 || highlight == 2 || highlight == 3)
	wght = "medium";
      else
	wght = "bold";
      if (highlight == 0 || highlight == 1)
	slant = "r";
      else
	slant = "o";
      if (size < 0)
	{
	  sprintf (r_nameX, "%s-%s-%s-*-*-13-*-*-*-*-*-iso8859-%c",
		   ffamily, wght, slant, alphabet);
	  size = 12;
	}
      else
	sprintf (r_nameX, "%s-%s-%s-*-*-%d-*-*-*-*-*-iso8859-%c",
		 ffamily, wght, slant, size, alphabet);
    }
  else if (alphabet == 'G' || family == 0)
    {
      family = 0;
      highlight = 0;
      sprintf (r_nameX, "-*-symbol-medium-r-*-*-%d-*-*-*-*-*-*-fontspecific", size);
    }
  else
    {
      if (UseLucidaFamily)
	{
	  switch (family)
	    {
	    case 1:
	      ffamily = "-*-lucidabright";
	      break;
	    case 3:
	      ffamily = "-*-lucidatypewriter";
	      break;
	    default:
	      ffamily = "-*-lucida";
	      break;
	    }
	}
      else
	{
	  switch (family)
	    {
	    case 1:
	      if (UseAdobeFamily)
		ffamily = "-*-new century schoolbook";
	      else
		ffamily = "-*-times";
	      break;
	    case 2:
	      ffamily = "-*-helvetica";
	      break;
	    case 3:
	      ffamily = "-adobe-courier";
	      break;
	    default:
	      ffamily = "-*-*";
	    }
	}
  
      switch (highlight)
	{
	case 0:
	  wght = "medium";
	  slant = "r";
	  break;
	case 1:
	  if (UseLucidaFamily && family == 1)
	    wght = "demibold";
	  else
	    wght = "bold";
	  slant = "r";
	  break;
	case 2:
	case 3:
	  wght = "medium";
	  if (family == 2 || family == 3)
	    slant = "o";
	  else
	    slant = "i";
	  break;
	case 4:
	case 5:
	  if (UseLucidaFamily && family == 1)
	    {
	    wght = "demibold";
	    slant = "i";
	    }
	  else if (family == 2 || family == 2)
	    {
	    wght = "bold";
	    slant = "o";
	    }
	  else
	    {
	    wght = "bold";
	    slant = "i";
	    }
	  break;
	}
      sprintf (r_nameX, "%s-%s-%s-*-*-%d-*-75-75-*-*-iso8859-1",
	       ffamily, wght, slant, size);
    }
  /* generate the Postscript name */
  if (family > (int) strlen (cfamily))
    family = 1;
  sprintf (r_name, "%c%c%c%d", TOLOWER (alphabet), cfamily[family],
	   StylesTable[highlight], size);
}

/*----------------------------------------------------------------------
  ReadFont do a raw Thot font loading (bypasses the font cache).
  ----------------------------------------------------------------------*/
PtrFont ReadFont (char alphabet, int family, int highlight, int size,
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
  Parameters increase decrease are true when a new test is allowed.
  ----------------------------------------------------------------------*/
static PtrFont LoadNearestFont (char alphabet, int family, int highlight,
				int size, int frame,
				ThotBool increase, ThotBool decrease)
{
  int                 i, j, deb;
  int                 mask;
  char                text[10], PsName[10], textX[100];
#ifdef _WINDOWS
  SIZE                wsize;
  TEXTMETRIC          textMetric;
  int                 c;
  HFONT               hOldFont;
#endif /* _WINDOWS */
  PtrFont             ptfont;

  FontIdentifier (alphabet, family, highlight, size, UnRelative, text, textX);
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
	  /* Allocate the font structure */
	  ptfont = TtaGetMemory (sizeof (FontInfo));
	  ptfont->alphabet  = alphabet;
	  ptfont->family    = family;
	  ptfont->highlight = highlight;
      size = LogicalPointsSizes[size];
	  ptfont->size      = size;
	  ActiveFont = WIN_LoadFont (alphabet, family, highlight, size);
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
	      if (increase)
		{
		  if (size >= MaxNumberOfSizes)
		    increase = FALSE;
		  else
		    {
		      size++;
		      ptfont = LoadNearestFont (alphabet, family, highlight,
						size, frame, increase, FALSE);
		    }
		}
	      if (ptfont == NULL && decrease && !increase)
		{
		  if (size <= 0)
		    decrease = FALSE;
		  else
		    {
		      size--;
		      ptfont = LoadNearestFont (alphabet, family, highlight,
						size, frame, FALSE, decrease);
		    }
		}
	    }
	}

      if (ptfont == NULL)
	{
	  if (alphabet != 'L' && alphabet != 'G' && size != -1)
	    /* try without highlight and no specific size */
	    ptfont = LoadNearestFont (alphabet, family,
				      0, -1, frame, FALSE, FALSE);
	  else
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
	}

      if (i >= MAX_FONT)
	i = j;		/* existing entry in the cache */
      else
	{
	  /* initialize a new cache entry */
	  TtFonts[i] = ptfont;
#if !defined(_WINDOWS) && !defined(_GTK)
          size = LogicalPointsSizes[size];
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
  PtrFont            lfont, *pfont;
  CHARSET            encoding;
  unsigned char      car;
  int                mask, frame;

  lfont = NULL;
  if (fontset)
    {
      if (c < 0xFF)
	{
	  /* 0 -> FF */
	  lfont = fontset->FontIso_1;
	  car = c;
	}
      else if (c >= 0x370 && c < 0x3FF)
	{
	  if (fontset->FontIso_7 == NULL)
	    {
	      /* load that font */
	      for (frame = 1; frame <= MAX_FRAME; frame++)
		{
		  mask = 1 << (frame - 1);
		  if (fontset->FontMask | mask)
		    lfont = LoadNearestFont (GreekFontAlphabet, fontset->FontFamily,
					     fontset->FontHighlight,
					     fontset->FontSize,
					     frame, TRUE, TRUE);
		  if (GreekFontAlphabet == '7' && lfont == NULL)
		    {
		      /* use symbol instead of ISO_8859_7 */
		      GreekFontAlphabet = 'G';
		    lfont = LoadNearestFont (GreekFontAlphabet, fontset->FontFamily,
					     fontset->FontHighlight,
					     fontset->FontSize,
					     frame, TRUE, TRUE);
		    }
		}
	      if (lfont == NULL)
		/* font not found: avoid to retry later */
		lfont = (void *) -1;
	      fontset->FontIso_7 = lfont;
	    }
	  else
	    lfont = fontset->FontIso_7;
	  if (GreekFontAlphabet == '7')
	    car = TtaGetCharFromWC (c, ISO_8859_7);
	  else
	    /* using the font symbol instead of ISO_8859_7 */
	    car = TtaGetCharFromWC (c, ISO_SYMBOL);
	}
      else
	{
	  if (c < 0x17F)
	    {
	      car = '2';
	      pfont = &(fontset->FontIso_2);
	      encoding = ISO_8859_2;
	    }
	  else if (c < 0x24F)
	    {
	      car = '3';
	      pfont = &(fontset->FontIso_3);
	      encoding = ISO_8859_3;
	    }
	  else if (c < 0x2AF)
	    {
	      car = '4';
	      pfont = &(fontset->FontIso_4);
	      encoding = ISO_8859_4;
	    }
	  else if (c < 0x45F)
	    {
	      car = '5'; /* Cyrillic */
	      pfont = &(fontset->FontIso_5);
	      encoding = ISO_8859_5;
	    }
	  else if (c < 0x5FF)
	    {
	      car = '8'; /* Hebrew */
	      pfont = &(fontset->FontIso_8);
	      encoding = ISO_8859_8;
	    }
	  else if (c < 0x5FF)
	    {
	      car = '9';
	      pfont = &(fontset->FontIso_9);
	      encoding = ISO_8859_9;
	    }
	  else if (c < 0x65F)
	    {
	      car = '6'; /* Arabic */
	      pfont = &(fontset->FontIso_6);
	      encoding = ISO_8859_6;
	    }
	  else
	    pfont = NULL;
	  if (pfont)
	  {
	  if (*pfont == NULL)
	    {
	      /* load that font */
	      for (frame = 1; frame <= MAX_FRAME; frame++)
		{
		  mask = 1 << (frame - 1);
		  if (fontset->FontMask | mask)
		    lfont = LoadNearestFont (car, fontset->FontFamily,
					     fontset->FontHighlight,
					     fontset->FontSize,
					     frame, TRUE, TRUE);
		}
	      if (lfont == NULL)
		/* font not found: avoid to retry later */
		lfont = (void *) -1;
	    *pfont = lfont;
	    }
	  else
	    lfont = *pfont;
	  }
	  else
	    lfont = (void *) -1;
	  car = TtaGetCharFromWC (c, encoding);
	}
    }
  /* when any font is available */
  if (lfont == (void *) -1)
    {
      /* generate a square */
      car = UNDISPLAYED_UNICODE;
      *font = NULL;
    }
  else
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
static SpecFont LoadFontSet (char alphabet, int family, int highlight,
			     int size, TypeUnit unit, int frame)
{
  int                 index;
#ifdef _I18N_
  SpecFont            prevfontset, fontset;
  int                 mask;
  ThotBool            specificFont = (alphabet == 'G');
#endif /* _I18N_ */

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

#ifdef _I18N_
  /* look for the fontsel */
  fontset = FirstFontSel;
  prevfontset = NULL;
  while (fontset &&
	 (fontset->specificFont != specificFont ||
	  fontset->FontFamily != family ||
	  fontset->FontHighlight != highlight ||
	  fontset->FontSize != index))
    fontset = fontset->NextFontSet;
  if (fontset == NULL)
    {
      /* create a new set */
      fontset = TtaGetMemory (sizeof (FontSet));
      if (fontset)
	{
	  memset (fontset, 0, sizeof (FontSet));
	  fontset->specificFont = specificFont;
	  fontset->FontFamily = family;
	  fontset->FontHighlight = highlight;
	  fontset->FontSize = size;
	  fontset->FontIso_1 = LoadNearestFont (alphabet, family, highlight,
						index, frame, TRUE, TRUE);
	  /* link this new fontset */
	  if (prevfontset)
	    prevfontset->NextFontSet = fontset;
	  else
	    FirstFontSel = fontset;
	}
      else
	fontset = FirstFontSel;
    }

  /* Compute window frame */
  mask = 1 << (frame - 1);
  /* store window frame number */
  fontset->FontFrames = fontset->FontFrames | mask;
#else /* _I18N_ */
  return LoadNearestFont (alphabet, family, highlight, index,
			  frame, TRUE, TRUE);
#endif /* _I18N_ */
}

/*----------------------------------------------------------------------
  ThotLoadFont try to load a font given a set of attributes like alphabet,
  family, the size and for a given frame.
  ----------------------------------------------------------------------*/
SpecFont ThotLoadFont (char alphabet, int family, int highlight, int size,
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
   return LoadFontSet (alphabet, family, highlight, size, unit, frame);
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
  int              i, index;

  /* is there a predefined font family ? */
  MenuSize = 12;
  alphabet = TtaGetAlphabet (TtaGetDefaultLanguage ());
  /* initialize the font zoom */
  TtaGetEnvInt ("ZOOM", &FontZoom);
  value = TtaGetEnvString ("FontFamily");
  MaxNumberOfSizes = 10;
  GreekFontAlphabet = '7';
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
  GraphicsIcons = NULL;
  SmallFontDialogue = NULL;

  /* Initialize the font table */
  for (i = 0; i < MAX_FONT; i++)
    TtFonts[i] = NULL;

  /* load first five predefined fonts */
  index = 0;
  while (LogicalPointsSizes[index] < MenuSize && index <= MaxNumberOfSizes)
    index++;
  FontDialogue =  LoadNearestFont (alphabet, 1, 0, index, 0, TRUE, TRUE);
  if (FontDialogue == NULL)
    {
      FontDialogue = LoadNearestFont (alphabet, 2, 0, index, 0, TRUE, TRUE);
      if (FontDialogue == NULL)
	TtaDisplaySimpleMessage (FATAL, LIB, TMSG_MISSING_FONT);
    }
  IFontDialogue = LoadNearestFont (alphabet, 1, 2, index, 0, TRUE, TRUE);
  if (IFontDialogue == NULL)
    {
      IFontDialogue = LoadNearestFont (alphabet, 2, 2, index, 0, TRUE, TRUE);
      if (IFontDialogue == NULL)
	IFontDialogue = FontDialogue;
    }

  index = 0;
  while (LogicalPointsSizes[index] < f3 && index <= MaxNumberOfSizes)
    index++;
  LargeFontDialogue = LoadNearestFont (alphabet, 1, 1, index, 0, TRUE, TRUE);
  if (LargeFontDialogue == NULL)
    {
      LargeFontDialogue = LoadNearestFont (alphabet, 2, 1, index, 0, TRUE, TRUE);
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









