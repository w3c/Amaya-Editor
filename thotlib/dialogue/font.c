/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2002
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 
/*
 * Module dedicated to font handling.
 *
 * Authors: I. Vatton (INRIA)
 *          R. Guetari (W3C/INRIA) Windows version
 *          D. Veillard (W3C/INRIA): Windows NT/95 routines
 *			P. Cheyrou-lagreze (INRIA) - Opengl Version
 *
 */

#include "thot_sys.h"
#include "libmsg.h"
#include "message.h"
#include "constmedia.h"
#include "typemedia.h"
#include "frame.h"
#include "appdialogue.h"

/*  tolower(c) was a macro defined in ctypes.h that returns
   something wrong if c is not an upper case letter. */
#define TOLOWER(c)	(isupper(c)? tolower(c) : (c))

#define THOT_EXPORT extern
#include "frame_tv.h"
#include "units_tv.h"
#include "edit_tv.h"
#include "boxes_tv.h"
#undef THOT_EXPORT
#define THOT_EXPORT
#include "font_tv.h"

/* Store the list of frames that use each font */
static int          TtFontMask[MAX_FONT];
/* that table for the charSThotLoacter glyphs */
static int          FirstRemovableFont;
static int          FirstFreeFont = 0;
static char         StylesTable[MAX_HIGHLIGHT] = "rbiogq";

/* Maximum number of font size handled */
static int          MaxNumberOfSizes;
static int          LogicalPointsSizes[MAX_LOG_SIZE] =
{6, 8, 10, 12, 14, 16, 20, 24, 30, 40, 60};
static char        *FontFamily;
static char         GreekFontScript;
static ThotBool     UseLucidaFamily;
static ThotBool     UseAdobeFamily;
#ifdef _WINDOWS
typedef struct FontCharacteristics {
        int   highlight; 
        int   size;
        int   family; 
        char  script; 
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

#ifdef _GL
#ifdef _GTK
#include <gtkgl/gtkglarea.h>
#endif /*_GTK*/
#include <GL/glu.h>
/* Texture Font */
/* ~/Amaya/thotlib/internals/h */
#include "openglfont.h"
#include "glwindowdisplay.h"


/*----------------------------------------------------------------------
 GL_FontIInit 
 Use Freetype2 and the FTGL lib :
 http://homepages.paradise.net.nz/henryj/code/index.html
  ----------------------------------------------------------------------*/
void *GL_LoadFont (char alphabet, int family, 
		   int highlight, int size,
		   char *xlfd)
{    
  char filename[2048];
  
   if (size > MaxNumberOfSizes)
	size = LogicalPointsSizes[MaxNumberOfSizes];
   else if (size >= 0)
	size = LogicalPointsSizes[size];
   else
	size = 12;
  if (GetFontFilename(alphabet, family, 
		      highlight, size, 
		      UseLucidaFamily, UseAdobeFamily,
		      filename))
      return (gl_font_init (filename, alphabet, size)); 
  return NULL;
}
#endif /* _GL */

#ifdef _WINDOWS
#include "wininclude.h"

/*----------------------------------------------------------------------
  WIN_LoadFont :  load a Windows TrueType with a defined set of
  characteristics.
  ----------------------------------------------------------------------*/
static HFONT WIN_LoadFont (char script, int family, int highlight,
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

   switch (script)
     {
     case 'G':
       charset = DEFAULT_CHARSET;/*SYMBOL_CHARSET;*/
       family = 0;
	 break;
     case '2':
       charset = EASTEUROPE_CHARSET;
       break;
     case '3':
       charset = BALTIC_CHARSET;
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
     case '9':
       charset = TURKISH_CHARSET;
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
      LastUsedFont->script = font->script; 
      LastUsedFont->family = font->family; 
      
      if (ActiveFont)
	{
	  if (!DeleteObject (SelectObject (hdc, GetStockObject (SYSTEM_FONT))))
            WinErrorBox (NULL, "WinLoadFont (1)");
	  ActiveFont = 0;
	}
    }
  else if (LastUsedFont->highlight != font->highlight ||
	   LastUsedFont->size != font->size ||
	   LastUsedFont->script != font->script ||
	   LastUsedFont->family != font->family)
    {
    if (ActiveFont)
      {
	SelectObject (hdc, GetStockObject (SYSTEM_FONT));
	DeleteObject (ActiveFont);
	ActiveFont = 0;
	LastUsedFont->highlight = font->highlight; 
	LastUsedFont->size      = font->size;
	LastUsedFont->script  = font->script; 
	LastUsedFont->family    = font->family; 
      } 
   }

   ActiveFont = WIN_LoadFont (font->script, font->family,
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

#if !defined(_WINDOWS) && !defined(_GTK)
/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
static XCharStruct *CharacterStructure(CHAR_T c, XFontStruct *xf)
{
  unsigned int c1, c2;

  if (xf->per_char == NULL)
    return NULL;
  else if (xf->min_byte1 == 0 && xf->max_byte1 == 0)
    {
      if (c < xf->min_char_or_byte2 || c >= xf->max_char_or_byte2)
	return NULL;
      else
	return &xf->per_char[c - xf->min_char_or_byte2];
    }
  else
    {
      c1 = (c>>8) & 0xff;
      c2 = c & 0xff;
      if (c1 < xf->min_byte1 || c1 >= xf->max_byte1 ||
	  c2 < xf->min_char_or_byte2 || c2 >= xf->max_char_or_byte2)
	return NULL;
      else
	return &xf->per_char[(c1 - xf->min_byte1) * (xf->max_char_or_byte2 - xf->min_char_or_byte2 + 1) + c2 - xf->min_char_or_byte2];
    }
}
#endif /* _WINDOWS && _GTK */

#ifndef _GL
/*----------------------------------------------------------------------
  CharacterWidth returns the width of a char in a given font.
  ----------------------------------------------------------------------*/
int CharacterWidth (int c, PtrFont font)
{
#if !defined(_WINDOWS) && !defined(_GTK)
  XFontStruct        *xf = (XFontStruct *) font;
  XCharStruct        *xc;
#endif /* !defined(_WINDOWS) && !defined(_GTK) */
  int                 i = 0, l;

  if (font == NULL)
    return 0;
  else if (c == INVISIBLE_CHAR)
    return 1;

  if (c == START_ENTITY)
    c = '&';
  else if (c == TAB || c == UNBREAKABLE_SPACE)
    /* we use the SPACE width for the character TAB */
    c = SPACE;

  if (c == NEW_LINE || c == BREAK_LINE)
    /* characters NEW_LINE and BREAK_LINE are equivalent */
    l = 1;
  else
    {
#ifdef _WINDOWS
      if (c == THIN_SPACE)
	l = (font->FiWidths[32] + 3) / 4;
      else if (c == HALF_EM)
	l = (font->FiWidths[32] + 3) / 2;
      else
	l = font->FiWidths[c];
#else  /* _WINDOWS */
#ifdef _GTK
      if (c == THIN_SPACE)
	l = gdk_char_width (font, 32) / 4;
      else if (c == HALF_EM)
	l = gdk_char_width (font, 32) / 2;
      else
	l = gdk_char_width (font, c);
#else /* _GTK */
      if (c == THIN_SPACE || c == HALF_EM)
	xc = CharacterStructure(32, xf);
      else
	xc = CharacterStructure(c, xf);
      if (xc == NULL)
	l = xf->max_bounds.width;
      else
	l = xc->width;
      if (c == THIN_SPACE)
	l = (l + 3)/4;
      else if (c == HALF_EM)
	l = (l + 1)/2;
#endif  /* _GTK */
      if (c == 244)
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
	}
#endif /* _WINDOWS */
    }
  return l;
}
#endif /*_GL*/

/*----------------------------------------------------------------------
  SpecialCharBoxWidth : size of special unicode chars..
  ----------------------------------------------------------------------*/
int SpecialCharBoxWidth (CHAR_T c)
{
  if (c == 0x200C /* zwnj*/ || c == 0x200D /* zwj */ ||
      c == 0x200E /* lrm */ || c == 0x200F /* rlm */ ||
      c == 0x202A /* lre */ || c == 0x202B /* rle */ ||
      c == 0x202D /* lro */ || c == 0x202E /* rlo */ ||
      c == 0x202C /* pdf */ || c == 0x2061 /* ApplyFunction */ ||
      c == 0x2062 /* InvisibleTimes */)
    return 1;
  else
    return 0;
}
/*----------------------------------------------------------------------
  BoxCharacterWidth returns the width of a char in a given font
  ----------------------------------------------------------------------*/
int BoxCharacterWidth (CHAR_T c, SpecFont specfont)
{
#ifndef _GL
#ifdef _I18N_
  PtrFont         font;
  int             car;

  if (SpecialCharBoxWidth (c))
    return 1;
  car = GetFontAndIndexFromSpec (c, specfont, &font);
  if (font == NULL)
    return 6;
  else
    return CharacterWidth (car, font);
#else /* _I18N_ */
  return CharacterWidth (c, specfont);
#endif /* _I18N_ */
#else /*_GL*/
  PtrFont         font;

  /*if (SpecialCharBoxWidth (c))
   return 1;*/
  GetFontAndIndexFromSpec (c, specfont, &font);
  if (font == NULL)
    return 6;
  else 
    return UnicodeCharacterWidth (c, font);
#endif /*_GL*/
}
/*----------------------------------------------------------------------
  CharacterHeight returns the height of a char in a given font
  ----------------------------------------------------------------------*/
int CharacterHeight (int c, PtrFont font)
{
  int              l;
#if !defined (_WINDOWS) && !defined (_GTK)
  XCharStruct     *xc;
#endif /* _WINDOWS && _GTK */

  if (font == NULL)
    return (0);
#ifdef _GL
  else
    l = gl_font_char_height (font, (CHAR_T *) &c);
#else /*_GL*/
#ifdef _WINDOWS
  else
    l = font->FiHeights[c];
#else  /* _WINDOWS */
#ifdef _GTK
  else
    l = gdk_char_height (font, c);
#else /* _GTK */
  xc = CharacterStructure (c, (XFontStruct *)font);
  if (xc == NULL)
    return FontHeight (font);
  else
    return xc->ascent + xc->descent;
#endif /* _GTK */
#endif /* _WINDOWS */
#endif /*_GL*/
  return l;
}

/*----------------------------------------------------------------------
  CharacterAscent returns the ascent of a char in a given font.
  ----------------------------------------------------------------------*/
int CharacterAscent (int c, PtrFont font)
{
#ifndef _WINDOWS
  int		    i;
#endif /*_WINDOWS*/
#if !defined(_WINDOWS)  || defined(_GL)
  int               ascent;
#endif /* _WINDOWS || _GL */
#if !defined(_WINDOWS) && !defined(_GL)
#ifdef _GTK
  char              car;
  int               lbearing, rbearing, width, descent;
#else /* _GTK */
  XFontStruct      *xf = (XFontStruct *) font;
  XCharStruct      *xc;
#endif /* _GTK */
#endif /* _WINDOW && _GL */

  if (font == NULL)
    return (0);
#ifdef _GL
else
   ascent = gl_font_char_ascent (font, (CHAR_T *) &c);
#ifndef _WINDOWS
  if (c == 244)
    {
      i = 0;
      while (i < MAX_FONT && font != TtFonts[i])
	i++;
      if (TtPatchedFont[i])
	ascent -= 2;
    }
#endif /*_WINDOWS*/
  return (ascent);
#else /*_GL*/
#ifdef _WINDOWS
  else
    return font->FiAscent;
#else  /* _WINDOWS */
  else
    {
#ifdef _GTK
      car = (char) c;
      gdk_string_extents (font, &car, &lbearing, &rbearing, &width, &ascent, &descent);
#else /* _GTK */
      xc = CharacterStructure(c, xf);
      if (xc == NULL)
	ascent = xf->max_bounds.ascent;
      else
	ascent = xc->ascent;
#endif /* _GTK */
    }
  if (c == 244)
    {
      /* a patch due to errors in standard symbol fonts */
      i = 0;
      while (i < MAX_FONT && font != TtFonts[i])
	i++;
      if (TtPatchedFont[i])
	ascent -= 2;
    }
  return (ascent);
#endif /* _WINDOWS */
#endif /*_GL*/
}

/*----------------------------------------------------------------------
  FontAscent returns a global ascent for a font.
  ----------------------------------------------------------------------*/
int FontAscent (PtrFont font)
{
  if (font == NULL)
    return (0);
#ifdef _GL
  else
    return (gl_font_ascent(font));
#else /*_GL*/
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
#endif /*_GL*/
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
  if (font == NULL)
    return (0);
  else
#ifdef _GL
    return (gl_font_height (font));
#else /* _GL */
#ifdef _WINDOWS
  return (font->FiHeight);
#else  /* _WINDOWS */
#ifdef _GTK
    /* in the string below, 'r' represents the greek character rho (lowercase)
       and produces a descender in the string when using the Symbol font */
    return (gdk_string_height (font, "AXpr") + 3); /* need some extra space */
#else /* _GTK */
    return ((XFontStruct *) font)->max_bounds.ascent + ((XFontStruct *) font)->max_bounds.descent;
#endif /* _GTK */
#endif /* !_WINDOWS */
#endif /*_GL*/
}

/*----------------------------------------------------------------------
  BoxFontHeight returns the height of a given font.
  ----------------------------------------------------------------------*/
int BoxFontHeight (SpecFont specfont)
{
#ifdef _I18N_
  PtrFont         font;
  int             car;

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
#ifdef _I18N_
  /*printf ("%s\n", name);*/
#endif /* _I18N_ */
#ifdef _GTK
  GdkFont *result;

  result = gdk_font_load ((gchar *)name);
  return (result);
#else /* _GTK */
  return ((PtrFont) XLoadQueryFont (TtDisplay, name));
#endif /* _GTK */
}
#endif /* _WINDOWS */

/*----------------------------------------------------------------------
  FontIdentifier computes the name of a Thot font.
  ----------------------------------------------------------------------*/
void FontIdentifier (char script, int family, int highlight, int size,
		     TypeUnit unit, char r_name[10], char r_nameX[100])
{
  char        *cfamily = "sthc";
  char        *wght, *slant, *ffamily;
  char         encoding[3];

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

  if (script != 'L' && script != 'G' && script != 'Z')
    {
      if (script == 'F')
	strcpy (encoding, "15");
      else if (script == 'D')
	strcpy (encoding, "13");
      else
	sprintf (encoding, "%c", script);
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
	  sprintf (r_nameX, "%s-%s-%s-*-*-13-*-*-*-*-*-iso8859-%s",
		   ffamily, wght, slant, encoding);
	  size = 12;
	}
      else
	sprintf (r_nameX, "%s-%s-%s-*-*-%d-*-*-*-*-*-iso8859-%s",
		 ffamily, wght, slant, size, encoding);
    }
  else if (script == 'G' || family == 0)
    {
      family = 0;
      highlight = 0;
      sprintf (r_nameX, "-*-symbol-medium-r-*-*-%d-*-*-*-*-*-*-fontspecific", size);
    }
  else if (script == 'Z')
    {
      ffamily = "-ms-*";
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
	  sprintf (r_nameX, "%s-%s-%s-*-*-13-*-*-*-*-*-iso10646-1",
		   ffamily, wght, slant);
	  size = 12;
	}
      else
	sprintf (r_nameX, "%s-%s-%s-*-*-%d-*-*-*-*-*-iso10646-1",
		 ffamily, wght, slant, size);
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
	default:
	  wght = "medium";
	  slant = "r";
	  break;
	}
      sprintf (r_nameX, "%s-%s-%s-*-*-%d-*-75-75-*-*-iso8859-1",
	       ffamily, wght, slant, size);
    }
  /* generate the Postscript name */
  if (family > (int) strlen (cfamily))
    family = 1;
  sprintf (r_name, "%c%c%c%d", TOLOWER (script), cfamily[family],
	   StylesTable[highlight], size);
}

/*----------------------------------------------------------------------
  ReadFont do a raw Thot font loading (bypasses the font cache).
  ----------------------------------------------------------------------*/
PtrFont ReadFont (char script, int family, int highlight, int size,
		  TypeUnit unit)
{
  char             name[10], nameX[100];

  FontIdentifier (script, family, highlight, size, unit, name, nameX);
#ifndef _WINDOWS
  return LoadFont (nameX);
#else  /* _WINDOWS */
  return NULL;
#endif /* _WINDOWS */
}

/*----------------------------------------------------------------------
  LoadNearestFont load the nearest possible font given a set of attributes
  like script, family, the size and for a given frame.
  Parameters increase decrease are true when a new test is allowed.
  The parameter requestedsize gives the initial requested size.
  ----------------------------------------------------------------------*/
static PtrFont LoadNearestFont (char script, int family, int highlight,
				int size, int requestedsize, int frame,
				ThotBool increase, ThotBool decrease)
{
  int                 i, j, deb;
  int                 mask, val;
  char                text[10], PsName[10], textX[100];
#if defined (_WINDOWS) && !defined (_GL)

  SIZE                wsize;
  TEXTMETRIC          textMetric;
  int                 c;
  HFONT               hOldFont;
  HDC                 display;
#endif /* _WINDOWS */
  PtrFont             ptfont;

  FontIdentifier (script, family, highlight, size, UnRelative, text, textX);
  /* initialize the PostScript font name */
  strcpy (PsName, text);   
  /* Font cache lookup */
  i = 0;
  deb = 0;
  ptfont = NULL;
  while (ptfont == NULL && i < FirstFreeFont)
    {
      if (TtFonts[i] == NULL)
	/* check if we forgot to update FirstFreeFont */
	FirstFreeFont = i;
      if (strcmp (&TtFontName[deb], text) == 0)
	/* Font cache lookup succeeded */
	ptfont = TtFonts[i];
      else
	{
	  i++;
	  deb += MAX_FONTNAME;
	}
    }   

  if (ptfont == NULL)
    {
      /* Load a new font */
      if (FirstFreeFont < MAX_FONT)
	{
	  /* No table overflow: load the new font */
#ifdef _GL
#ifdef _PCLDEBUGFONT
  g_print ("\n XLFD selection : %s", textX);
#endif /*_PCLDEBUG*/
	  ptfont = GL_LoadFont (script, family, highlight, size, textX);
#else /*_GL*/
#ifdef _WINDOWS
	  /* Allocate the font structure */
	  ptfont = TtaGetMemory (sizeof (FontInfo));
	  ptfont->script = script;
	  ptfont->family = family;
	  ptfont->highlight = highlight;
	  val = LogicalPointsSizes[size];
	  ptfont->size = val;
	  ActiveFont = WIN_LoadFont (script, family, highlight, val);
	  if (TtPrinterDC != NULL)
	    {
	      display = TtPrinterDC;
	      hOldFont = SelectObject (TtPrinterDC, ActiveFont);
	    }
	  else
	    {
	      display = GetDC(FrRef[frame]);
	      hOldFont = SelectObject (display, ActiveFont);
	    }
	  if (GetTextMetrics (display, &textMetric))
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
		  GetTextExtentPoint (display, (LPCTSTR) (&c),
				      1, (LPSIZE) (&wsize));
		  ptfont->FiWidths[c] = wsize.cx;
		  ptfont->FiHeights[c] = wsize.cy;
		}
	      if (ActiveFont)
	       DeleteObject (ActiveFont);
	      ActiveFont = 0;
	  if (TtPrinterDC == NULL && display)
		ReleaseDC (FrRef[frame], display);
#else  /* _WINDOWS */
	  ptfont = LoadFont (textX);
#endif /* !_WINDOWS */
#endif/*  _GL */
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
		      val = size + 1;
		      ptfont = LoadNearestFont (script, family, highlight,
						val, requestedsize,
						frame, increase, FALSE);
		    }
		}
	      if (ptfont == NULL && decrease && !increase)
		{
		  if (size <= 0)
		    decrease = FALSE;
		  else
		    {
		      val = size - 1;
		      ptfont = LoadNearestFont (script, family, highlight,
						val, requestedsize,
						frame, FALSE, decrease);
		    }
		}
	    }
	}
      if (ptfont == NULL)
	{
	  if (script != 'L' && script != 'G' && size != -1)
	    /* try without highlight and no specific size */
	    ptfont = LoadNearestFont (script, family, 0,
				      -1, requestedsize, frame, FALSE, FALSE);
	  else
	    {
	      /* Try to load another family from the same script */
	      for (j = 0; j < FirstFreeFont; j++)
		{
		  if (TtFonts[j] && TtFontName[j * MAX_FONTNAME] == script)
		    {
		      ptfont = TtFonts[j];
		      j = FirstFreeFont;
		    }
		}
	    }
	  if (ptfont == NULL && script == '7')
	    {
	      /* look for a font Symbol */
	      ptfont = LoadNearestFont (script, family, 0,
					-1, requestedsize, frame, FALSE, FALSE);
	      if (ptfont)
		/* now we'll work with the font Symbol */
		GreekFontScript = 'G';
	    }
	  /* last case the default font */
	  if (ptfont == NULL)
	    ptfont = FontDialogue;
	}
    }

  if (ptfont && size == requestedsize)
    {
      if (i == FirstFreeFont || TtFonts[i] == NULL)
	{
	  /* initialize a new entry */
	  FirstFreeFont = i + 1;
	  strcpy (&TtFontName[deb], text);
	  strcpy (&TtPsFontName[i * 8], PsName);
	  TtFonts[i] = ptfont;
#ifndef _WINDOWS
          val = LogicalPointsSizes[size];
	  if (script == 'G' &&
	      (val == 8 || val == 10 || val == 12 ||
	       val == 14 || val == 24))
	    TtPatchedFont[i] = val;
#endif /* _WINDOWS */
	}
      /* rely to the current frame */
      mask = 1 << (frame - 1);
      TtFontMask[i] = TtFontMask[i] | mask;
    }
  return (ptfont);
}

/*----------------------------------------------------------------------
  GetFontAndIndexFromSpec return the glyph index and the font
  used to display the wide character c;
  ----------------------------------------------------------------------*/
int GetFontAndIndexFromSpec (CHAR_T c, SpecFont fontset, PtrFont *font)
{
#ifdef _I18N_
  PtrFont            lfont, *pfont;
  CHARSET            encoding;
  char               code;
  int                car;
  int                mask, frame;

  lfont = NULL;
  if (fontset)
    {
      if (c <= 0xFF)
	{
	  /* 0 -> FF */
	  lfont = fontset->FontIso_1;
	  car = (int) c;
	}
      else if (c == 0x200C /* zwnj*/ || c == 0x200D /* zwj */ || 
	       c == 0x200E /* lrm */ || c == 0x200F /* rlm */ ||
	       c == 0x202A /* lre */ || c == 0x202B /* rle */ ||
	       c == 0x202D /* lro */ || c == 0x202E /* rlo */ ||
	       c == 0x202C /* pdf */ || c == 0x2061 /* ApplyFunction */ ||
	       c == 0x2062 /* InvisibleTimes */)
	car =  INVISIBLE_CHAR;
      else
	{
	  if (c >= 0x370 && c < 0x3FF)
	    {
	      /* Greek characters */
	      code = GreekFontScript;
	      if (c == 0x3D1 || c == 0x3D2 || c == 0x3D6)
		/* thetasym, upsih, piv */
		{
		  pfont = &(fontset->FontSymbol);
		  encoding = ISO_SYMBOL;
		}
	      else if (GreekFontScript == '7')
		{
		  pfont = &(fontset->FontIso_7);
#ifdef _WINDOWS
		  encoding = WINDOWS_1253;
#else /* _WINDOWS */
		  encoding = ISO_8859_7;
#endif /* _WINDOWS */
		}
	      else
		{
		  pfont = &(fontset->FontSymbol);
		  encoding = ISO_SYMBOL;
		}
	    }
	  else if (c == 0x152  /*oe*/     || c == 0x153  /*OE*/ ||
		   c == 0x178  /*ydiaeresis*/ ||
		   c == 0x2C6  /*circ*/   || c == 0x2DC  /*tilde*/ ||
		   c == 0x2002 /*ensp*/   || c == 0x2003 /*emsp*/ ||
		   c == 0x2009 /*thinsp*/ ||
		   c == 0x2013 /*ndash*/  || c == 0x2014 /*mdash*/ ||
		   (c >= 0x2018 && c <= 0x201E) /*quotes*/ ||
		   c == 0x2026 /*hellip*/    ||
		   c == 0x2039 /*inf*/    || c == 0x203A /*sup*/ ||
		   c == 0x20AC /*euro*/)
	    {
#ifdef _WINDOWS
	      code = '1'; /* West Europe Latin */
	      pfont = &(fontset->FontIso_1);
	      encoding = WINDOWS_1252;
#else /* _WINDOWS */
	      if (c == 0x152 /*oe*/ || c == 0x153  /*OE*/ ||
		  c == 0x178 /*ydiaeresis*/ || c == 0x20AC /*euro*/)
		{
		  if (Printing)
		    {
		      code = '1'; /* Extended Latin */
		      pfont = &(fontset->FontIso_1);
		      encoding = ISO_8859_1;
		      if (c == 0x152)
			c = 75;
		      else if (c == 0x153)
			c = 76;
		      else if (c == 0x178)
			c = 255;
		      else
			c = 128;
		    }
		  else
		    {
		      code = 'F'; /* Extended Latin */
		      pfont = &(fontset->FontIso_15);
		      encoding = ISO_8859_15;
		    }
		}
	      else
		{
		  /* use an approaching character */
		  encoding = ISO_8859_1;
		  code = '1';
		  pfont = &(fontset->FontIso_1);
		  if (c == 0x2C6)       /*circ*/
		    c = 94;
		  else if (c == 0x2DC)  /*tilde*/
		    c = 126;
		  else if (c == 0x2002) /*ensp*/
		    c = 130;
                  else if (c == 0x2003) /*emsp*/
		    c = 160;
		  else if (c == 0x2009) /*thinsp*/
		    c = 129;
		  else if (c == 0x2018 || c == 0x201C)
		    c = 96;
		  else if (c == 0x2019 || c == 0x201D)
		    c = 39;
		  else if (c == 0x201A || c == 0x201E)
		    c = 44;
		  else if (c == 0x2039)
		    c = 60;
		  else if (c == 0x203A)
		    c = 62;
		  else
		    {
		      code = 'G';
		      pfont = &(fontset->FontSymbol);
		      if (c == 0x2013)  /* en dash */
			c = 45;
		      else if (c == 0x2014) /* em dash */
			c = 190;
		      else if (c == 0x2026) /* horizontal ellipsis */
			c = 188;
		    }
		}
#endif /* _WINDOWS */
	    }
	  else if (c < 0x17F)
	    {
	      code = '2'; /* Central Europe */
	      pfont = &(fontset->FontIso_2);
#ifdef _WINDOWS
	      encoding = WINDOWS_1250;
#else /* _WINDOWS */
	      encoding = ISO_8859_2;
#endif /* _WINDOWS */
	    }
	  else if ((c > 0x2000 && c < 0x237F &&
		   (c < 0x2018 || c > 0x201D) && /* Windows quotations */
		   c != 0x20AC) || /* euro */
		   c == 0x25CA ||  /* lozenge */
                   c == 0x2660 ||  /* black spade suit */
                   c == 0x2663 ||  /* black club suit */
                   c == 0x2665 ||  /* black heart suit */
		   c == 0x2666 ||  /* black diamond suit */
		   c == 0x192)     /* latin small letter f with hook */
	    {
	      /* Symbols */
	      code = 'G';
	      pfont = &(fontset->FontSymbol);
	      encoding = ISO_SYMBOL;
	    }
	  else if (c < 0x24F)
	    {
	      code = '3';
	      pfont = &(fontset->FontIso_3);
#ifdef _WINDOWS
	      encoding = WINDOWS_1250;
#else /* _WINDOWS */
	      encoding = ISO_8859_3;
#endif /* _WINDOWS */
	    }
	  else if (c < 0x2AF)
	    {
	      code = '4'; /* Baltic RIM */
	      pfont = &(fontset->FontIso_4);
#ifdef _WINDOWS
	      encoding = WINDOWS_1257;
#else /* _WINDOWS */
	      encoding = ISO_8859_4;
#endif /* _WINDOWS */
	    }
	  else if (c < 0x45F)
	    {
	      code = '5'; /* Cyrillic */
	      pfont = &(fontset->FontIso_5);
#ifdef _WINDOWS
	      encoding = WINDOWS_1251;
#else /* _WINDOWS */
	      encoding = ISO_8859_5;
#endif /* _WINDOWS */
	    }
	  else if (c < 0x5FF)
	    {
	      code = '8'; /* Hebrew */
	      pfont = &(fontset->FontIso_8);
#ifdef _WINDOWS
	      encoding = WINDOWS_1255;
#else /* _WINDOWS */
	      encoding = ISO_8859_8;
#endif /* _WINDOWS */
	    }
	  else if (c < 0x5FF)
	    {
	      code = '9'; /* Turkish */
	      pfont = &(fontset->FontIso_9);
#ifdef _WINDOWS
	      encoding = WINDOWS_1254;
#else /* _WINDOWS */
	      encoding = ISO_8859_9;
#endif /* _WINDOWS */
	    }
	  else if (c < 0x65F)
	    {
	      code = '6'; /* Arabic */
	      pfont = &(fontset->FontIso_6);
#ifdef _WINDOWS
	      encoding = WINDOWS_1256;
#else /* _WINDOWS */
	      encoding = ISO_8859_6;
#endif /* _WINDOWS */
	    }
	  else
	    {
	      code = 'Z'; /* Unicode */
	      pfont = &(fontset->FontUnicode);
	      encoding = UNICODE_1_1;
	    }
	  if (pfont)
	  {
	    if (*pfont == NULL)
	      {
		/* load that font */
		for (frame = 1; frame <= MAX_FRAME; frame++)
		  {
		    mask = 1 << (frame - 1);
		    if (fontset->FontMask & mask)
		      lfont = LoadNearestFont (code, fontset->FontFamily,
					       fontset->FontHighlight,
					       fontset->FontSize, fontset->FontSize,
					       frame, TRUE, TRUE);
		    if (code == '7' && GreekFontScript == 'G')
		      /* use the font Symbol instead of a greek font */
		      encoding = ISO_SYMBOL;
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
	  if (code == 'Z')
	    car = c;
	  car = (int)TtaGetCharFromWC (c, encoding);
	}
    }
  /* when any font is available */
  if (lfont == (void *) -1)
    {
      /* generate a square */
      car = UNDISPLAYED_UNICODE;
      *font = NULL;
    }
  else if (car == EOS)
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

/*----------------------------------------------------------------------
  LoadFontSet allocate a font set and load the ISO-latin-1 font.
  ----------------------------------------------------------------------*/
static SpecFont LoadFontSet (char script, int family, int highlight,
			     int size, TypeUnit unit, int frame)
{
  int                 index;
#ifdef _I18N_
  SpecFont            prevfontset, fontset;
  int                 mask;
  ThotBool            specificFont = (script == 'G');
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
  mask = 1 << (frame - 1);
  prevfontset = NULL;
  while (fontset &&
	 (fontset->specificFont != specificFont ||
	  fontset->FontFamily != family ||
	  fontset->FontHighlight != highlight ||
	  fontset->FontSize != index))
    {
      prevfontset = fontset;
      fontset = fontset->NextFontSet;
    }

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
	  fontset->FontSize = index;
	  fontset->FontMask = mask;
	  fontset->FontIso_1 = LoadNearestFont (script, family, highlight,
						index, index, frame, TRUE, TRUE);
	  /* link this new fontset */
	  if (prevfontset)
	    prevfontset->NextFontSet = fontset;
	  else
	    FirstFontSel = fontset;
	}
      else
	fontset = FirstFontSel;
    }
  else
    /* add the window frame number */
    fontset->FontMask = fontset->FontMask | mask;
  return (fontset);
#else /* _I18N_ */
  return LoadNearestFont (script, family, highlight, index, index,
			  frame, TRUE, TRUE);
#endif /* _I18N_ */
}

/*----------------------------------------------------------------------
  ThotLoadFont try to load a font given a set of attributes like script,
  family, the size and for a given frame.
  ----------------------------------------------------------------------*/
SpecFont ThotLoadFont (char script, int family, int highlight, int size,
		       TypeUnit unit, int frame)
{
  int          zoom;

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
  if (frame)
    {
      if (FontZoom)
	{
	  if (unit == UnRelative)
	    {
	      size = FontPointSize (size);
	      unit = UnPoint;
	    }
	  size = size * FontZoom / 100;
	}
      zoom = ViewFrameTable[frame - 1].FrMagnification;
      if (zoom)
	{
	  if (unit == UnRelative)
	    {
	      size = FontPointSize (size);
	      unit = UnPoint;
	    }
	  size = size + (size * zoom / 10);
	}
    }

   /* the minimum size is 6 points */
  if (size < 6 && unit == UnPoint)
    size = 6;
  return LoadFontSet (script, family, highlight, size, unit, frame);
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
#ifndef _WINDOWS
  int              ndir, ncurrent;
  char             FONT_PATH[128];
  char            *fontpath;
#endif /* _WINDOWS */
  char           **dirlist = NULL;
  char           **currentlist = NULL;
  char            *value;
  char             script;
  int              f3;
  int              i, index;

  /* is there a predefined font family ? */
  MenuSize = 12;
  script = TtaGetScript (TtaGetDefaultLanguage ());
  /* initialize the font zoom */
  TtaGetEnvInt ("FontZoom", &FontZoom);
 if (FontZoom == 0)
    {
      TtaGetEnvInt ("ZOOM", &FontZoom);
      if (FontZoom != 0)
	/* old model */
	FontZoom = (FontZoom * 10) + 100;
    }
   value = TtaGetEnvString ("FontFamily");
  MaxNumberOfSizes = 10;
#ifdef _WINDOWS
  GreekFontScript = '7';
#else /* _WINDOWS */
  if (Printing)
    /* Only the sysmbol font is available in Postscript */
    GreekFontScript = 'G';
  else
    GreekFontScript = '7';
#endif /* _WINDOWS */
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
  if (!Printing)
    {
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
    }
  for (i = 0; i < MAX_FONT; i++)
    TtPatchedFont[i] = 0;
#endif /* _WINDOWS */

  /* Initialize the Thot Lib standards fonts */
  FontDialogue = IFontDialogue = LargeFontDialogue = NULL;
  GraphicsIcons = NULL;
  SmallFontDialogue = NULL;

  /* Initialize the font table */
  for (i = 0; i < MAX_FONT; i++)
    {
      TtFonts[i] = NULL;
      TtFontMask[i] = 0;
    }
  FirstFreeFont = 0;
  /* load first five predefined fonts */
  index = 0;
  while (LogicalPointsSizes[index] < MenuSize && index <= MaxNumberOfSizes)
    index++;
  FontDialogue =  LoadNearestFont (script, 1, 0, index, index, 0, TRUE, TRUE);
  if (FontDialogue == NULL)
    {
      FontDialogue = LoadNearestFont (script, 2, 0, index, index, 0, TRUE, TRUE);
      if (FontDialogue == NULL)
	TtaDisplaySimpleMessage (FATAL, LIB, TMSG_MISSING_FONT);
    }
  IFontDialogue = LoadNearestFont (script, 1, 2, index, index, 0, TRUE, TRUE);
  if (IFontDialogue == NULL)
    {
      IFontDialogue = LoadNearestFont (script, 2, 2, index, index, 0, TRUE, TRUE);
      if (IFontDialogue == NULL)
	IFontDialogue = FontDialogue;
    }

  index = 0;
  while (LogicalPointsSizes[index] < f3 && index <= MaxNumberOfSizes)
    index++;
  LargeFontDialogue = LoadNearestFont (script, 1, 1, index, index, 0, TRUE, TRUE);
  if (LargeFontDialogue == NULL)
    {
      LargeFontDialogue = LoadNearestFont (script, 2, 1, index, index, 0, TRUE, TRUE);
      if (LargeFontDialogue == NULL)
	LargeFontDialogue = IFontDialogue;
    }
  FirstRemovableFont = FirstFreeFont;
}

/*----------------------------------------------------------------------
 FreeAFont frees a specific font
  ----------------------------------------------------------------------*/
static void FreeAFont (int i)
{
  int                 j;
  ThotBool            found;

  if (TtFonts[i])
    {
      /* the font structure could be used by another entry */
      j = 0;
      found = FALSE;
      while (!found && j < MAX_FONT && TtFonts[j])
	{
	  if (j == i)
	    j++;
	  else if (TtFonts[j] == TtFonts[i])
	    found = TRUE;
	  else
	    j++;
	}
      if (!found)
	/* we free this font */
#ifdef _WINDOWS
	TtaFreeMemory (TtFonts[i]);
#else  /* _WINDOWS */
#ifdef _GTK
#ifndef _GL 
	gdk_font_unref (TtFonts[i]);
#else /*_GL */
	gl_font_delete (TtFonts[i]);
#endif /*_GL*/
#else /* _GTK */
	XFreeFont (TtDisplay, (XFontStruct *) TtFonts[i]);
#endif /* _GTK */
      /* unmask patched fonts */
      if (TtPatchedFont[i])
	TtPatchedFont[i] = 0;
#endif /* _WINDOWS */
      TtFontMask[i] = 0;
      /* pack the font table */
      FirstFreeFont--;
      j = FirstFreeFont;
      if (j > i)
	{
	  /* move this entry to the freed position */
	  TtFonts[i] = TtFonts[j];
	  TtFontMask[i] = TtFontMask[j];

#if !defined(_WINDOWS) && !defined(_GL)
	  TtPatchedFont[i] = TtPatchedFont[j];
#endif /* _WINDOWS && _GL*/
	  strncpy (&TtFontName[i * MAX_FONTNAME],
		   &TtFontName[j * MAX_FONTNAME], MAX_FONTNAME);
	  TtFonts[j] = NULL;
	  /* the table is packed now */
	}
      TtFonts[FirstFreeFont] = NULL;
    }
}

/*----------------------------------------------------------------------
 ThotFreeFont frees fonts used by a frame.
  ----------------------------------------------------------------------*/
void ThotFreeFont (int frame)
{
#ifdef _I18N_
  SpecFont            prevset, fontset, nextset;
#endif /* _I18N_ */
  int                 i, mask;

  if (frame > 0)
    {
      /* compute the frame mask */
      mask = 1 << (frame - 1);
#ifdef _I18N_
      /* free all attached fontsets */
      fontset = FirstFontSel;
      prevset = NULL;
      while (fontset)
	{
	  /* next set */
	  nextset = fontset->NextFontSet;
	  /* is it still in use? */
	  if (fontset->FontMask == mask)
	    {
	      /* free this fontset */
	      TtaFreeMemory (fontset);
	      if (prevset)
		prevset->NextFontSet = nextset;
	      else
		FirstFontSel = nextset;
	    }
	  else
	    {
	      fontset->FontMask = fontset->FontMask & (~mask);
	      prevset = fontset;
	    }
	  fontset = nextset;
	}
#endif /* _I18N_ */
      /* keep default fonts */
      i = FirstRemovableFont;
      /* free all attached fonts */
      while (i < FirstFreeFont)
	{
	  if (TtFontMask[i] == mask)
	    /* free the entry */
	    FreeAFont (i);
	  else
	    {
	      /* unlink this frame */
	      TtFontMask[i] = TtFontMask[i] & (~mask);
	      i++;
	    }
	}
    }
}

/*----------------------------------------------------------------------
  ThotFreeAllFonts
  ----------------------------------------------------------------------*/
void ThotFreeAllFonts (void)
{
#ifdef _I18N_
  SpecFont            fontset, nextset;
#endif /* _I18N_ */
  int                 i;

#ifdef _I18N_
  /* free all attached fontsets */
  fontset = FirstFontSel;
  while (fontset)
    {
      nextset = fontset->NextFontSet;
      TtaFreeMemory (fontset);
      fontset = nextset;
    }
  FirstFontSel = NULL;
#endif /* _I18N_ */
  for (i = 0; i < MAX_FONT && TtFonts[i]; i++)
    FreeAFont (i);
  TtaFreeMemory (FontFamily);
}

