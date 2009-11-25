/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2009
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 
/*
 * Module dedicated to font handling.
 *
 * Authors: I. Vatton (INRIA)
 *          R. Guetari (W3C/INRIA) Windows version
 *          D. Veillard (W3C/INRIA): Windows NT/95 routines
 *          P. Cheyrou-lagreze (INRIA) - Opengl Version
 *
 */

#include "thot_gui.h"
#include "thot_sys.h"
#include "libmsg.h"
#include "message.h"
#include "constmedia.h"
#include "typemedia.h"
#include "frame.h"
#include "appdialogue.h"
#include "application.h"
/*  tolower(c) was a macro defined in ctypes.h that returns
    something wrong if c is not an upper case letter. */
#define TOLOWER(c)	(isupper(c)? tolower(c) : (c))

#define THOT_EXPORT extern
#include "frame_tv.h"
#include "units_tv.h"
#include "edit_tv.h"
#include "boxes_tv.h"

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "font_tv.h"

/* Store the list of frames that use each font */
static unsigned int TtFontMask[MAX_FONT];
/* that table for the charSThotLoacter glyphs */
static int          FirstFreeFont = 0;
static char         StylesTable[MAX_HIGHLIGHT] = "rbiogq";

/* Maximum number of font size handled */
static int          MaxNumberOfSizes;
static int          LogicalPointsSizes[MAX_LOG_SIZE] =
  {6, 8, 10, 11, 12, 14, 16, 18, 20, 30, 40, 60, 70};
static char        *FontFamily;
static char         GreekFontScript;

#ifdef _WINGUI
static ThotFont   LastUsedFont = NULL;
static HFONT      OldFont;
#include "wininclude.h"  
#endif /* _WINGUI */

static SpecFont   FirstFontSel = NULL;

#include "buildlines_f.h"
#include "dialogapi_f.h"
#include "displaybox_f.h"
#include "font_f.h"
#include "language_f.h"
#include "memory_f.h"
#include "platform_f.h"
#include "registry_f.h"
#include "stix_f.h"
#include "units_f.h"
#include "windowdisplay_f.h"

#ifdef _GTK
#include <gdk/gdkx.h>
#endif /*_GTK*/

#include "fontconfig.h"

#ifdef _GL
  
#ifdef _GTK
#include <gtkgl/gtkglarea.h>
#endif /*_GTK*/
#if defined (_MACOS) && defined (_WX)
#include <glu.h>
#else /* _MACOS */
#include <GL/glu.h>
#endif /* _MACOS */

/* Texture Font */
/* ~/Amaya/thotlib/internals/h */
#include "openglfont.h"
#include "glwindowdisplay.h"


/*----------------------------------------------------------------------
  GL_LoadFont uses point size font
  ----------------------------------------------------------------------*/
static void *GL_LoadFont (char alphabet, int family, int highlight, int size)
{
  char filename[2048];

  if (GetFontFilename (alphabet, family, highlight, size, filename))
    {
      //  printf ("load %s size=%d font=%d\n",filename, size, FirstFreeFont);
      return (gl_font_init (filename, alphabet, size));
    }
  return NULL;
}
#endif /* _GL */

#ifdef _WINGUI
/*----------------------------------------------------------------------
  String_To_Charset Transcode a string charset into a windows define 
  ----------------------------------------------------------------------*/
static DWORD String_To_Charset (char *string)
{
  int int_charset;

  int_charset = atoi (string);
  switch (int_charset)
    {
    case 1250:
      return EASTEUROPE_CHARSET;/*FS_LATIN2*/
    case 1251:
      return RUSSIAN_CHARSET;/*FS_CYRILLIC*/	
    case 1252:
      return ANSI_CHARSET;/*FS_LATIN1*/
    case 1253:
      return GREEK_CHARSET;/*FS_GREEK*/
    case 1254:
      return TURKISH_CHARSET;/*FS_TURKISH*/
    case 1255:
      return HEBREW_CHARSET;/*FS_HEBREW*/
    case 1256:
      return ARABIC_CHARSET;/*FS_ARABIC*/
    case 1257:
      return BALTIC_CHARSET;/*FS_BALTIC*/
    case 1361:
      return JOHAB_CHARSET;/*FS_JOHAB*/
    case 874:
      return THAI_CHARSET;/*FS_THAI*/
    case 932:
      return SHIFTJIS_CHARSET;/*FS_JISJAPAN*/
    case 936:
      return GB2312_CHARSET;/*FS_CHINESESIMP*/
    case 949:
      return HANGEUL_CHARSET;/*FS_WANSUNG*/
    case 950:
      return CHINESEBIG5_CHARSET;/*FS_CHINESETRAD*/
    default:
      return ANSI_CHARSET;/*FS_LATIN1*/
    }
}
/*----------------------------------------------------------------------
  Win_Get_Charset Transcode Thot font attributes into Windows ones
  ----------------------------------------------------------------------*/
static void Win_Get_Charset (char script, DWORD *charset, int *family)
{
  switch (script)
    {
    case 'G':
      *charset = DEFAULT_CHARSET;/*SYMBOL_CHARSET;*/
      *family = 0;
      break;
    case '2':
      *charset = EASTEUROPE_CHARSET;
      break;
    case '3':
      *charset = BALTIC_CHARSET;
      break;
    case '4':
      *charset = DEFAULT_CHARSET;
      break;
    case '5':
      *charset = RUSSIAN_CHARSET;
      break;
    case '6':
      *charset = ARABIC_CHARSET;
      break;
    case '7':
      *charset = GREEK_CHARSET;
      break;
    case '8':
      *charset = HEBREW_CHARSET;
      break;
    case '9':
      *charset = TURKISH_CHARSET;
      break;
    case 'Z': /* fall through */
    default:
      *charset = DEFAULT_CHARSET;
      break;
    }
}
   
/*----------------------------------------------------------------------
  Win_Get_FontFace Transcode Thot font attributes into Windows ones
  ----------------------------------------------------------------------*/
static void Win_Get_FontFace (char *lpszFace, int script, 
                              int family, int *highlight)
{
  switch (family)
    {
    case 0:
      sprintf (lpszFace, "Symbol");
      break;
    case 1:
      sprintf (lpszFace, "Times New Roman");
      break;
    case 2:
      sprintf (lpszFace, "Arial");
      break;
    case 3:
      sprintf (lpszFace, "Courier New");
      break;
    default:
      sprintf (lpszFace, "Arial");
    }
  if (script == 'Z')
    sprintf (lpszFace, "Arial Unicode MS");
  /* sprintf (&lpszFace[0], "Bitstream Cyberbit"); */
  else if (script == 'E')
    {
      switch (family)
        {
        case 6:
          sprintf (lpszFace, "ESSTIXSix");
          break;
        case 7:
          sprintf (lpszFace, "ESSTIXSeven");
          break;     
        case 10:
          sprintf (lpszFace, "ESSTIXTen");
          break;
        default:
          break;
        }
      *highlight = 0;
    }
}
/*----------------------------------------------------------------------
  Win_Get_Highlight Transcode Thot font attributes into Windows ones
  ----------------------------------------------------------------------*/
static void Win_Get_Highlight (int highlight, int *fdwItalic, int *fnWeight)
{  
  switch (highlight)
    {
    case 0:
      break;
    case 2:
    case 3:
      *fdwItalic = TRUE;
      break;
    case 1:
    case 4:
    case 5:
      *fdwItalic = TRUE;
      *fnWeight = FW_BOLD;
      break;
    default:
      break;
    }
}
/*----------------------------------------------------------------------
  GetWinFontConfig Get font description from configuration file (config/fonts.win)
  ----------------------------------------------------------------------*/
static HFONT GetWinFontConfig (char script, int family, int highlight, int size)
{
  char Result_string[MAX_LENGTH];
  char *parsed, *result;
  char font_family[32];
  char font_charset[1024];
  char font_highlight[1024];
  DWORD charset;
  int        Bold;
  int        Italic;
  int i;

  result = FontLoadFromConfig (script, family, highlight);
  if (result)
    {
      strcpy (Result_string, result);
      result = &Result_string[0];
      parsed = result;
      i = 0;
      while (*parsed++ != '-' && *parsed && i < 1024)
        i++;
      if (i == 1024)
        return NULL;
      *(parsed-1) = '\0';	
      strcpy (font_charset, result);
      result = parsed;
      i = 0;
      while (*parsed++ != '-' && *parsed && i < 32)
        i++;
      if (i == 32)
        return NULL;
      *(parsed-1) = '\0';
      strcpy (font_family, result);
      result = parsed;
      i = 0;
      while (*parsed++ != '-' && *parsed && i < 1024)
        i++;
      if (i == 1024)
        return NULL;
      if (*parsed)
        *(parsed-1) = '\0';
      strcpy (font_highlight, result);
            
      charset = DEFAULT_CHARSET;
      Bold = FW_NORMAL;
      Italic = FALSE;
      
      if (*font_charset == '*')
        Win_Get_Charset (script, &charset, &family);
      else
        {
          i = 0;
          while (!isnum(font_charset[i]) && font_charset[i] != '-')
            i++;
          if (font_charset[i] != '-')
            charset = String_To_Charset (font_charset);
          else 
            Win_Get_Charset (script, &charset, &family);
        }
      
      if (font_family[0] == '*')
        Win_Get_FontFace (font_family, script, family, &highlight);
      if (script != 'E')
        {
          if (font_highlight[0] == '*')
            Win_Get_Highlight (highlight, &Italic, &Bold);
          else
            {
              switch (font_highlight[0])
                {
                case 'r':
                  break;
                case 'i':
                  Italic = TRUE;
                  break;
                case 'b':
                  Bold = FW_BOLD;
                  break;
                default:
                  break;
                }
              if (font_highlight[1] == 'i')
                Italic = TRUE;
            }
        }
      else 
        {
          /*charset = 0;
            Bold = 0;*/
        }
      return CreateFont (size, 0, 0, 0, Bold,
                         Italic, FALSE, FALSE,
                         charset, 
                         OUT_DEFAULT_PRECIS, 
                         CLIP_DEFAULT_PRECIS,
                         DEFAULT_QUALITY, 
                         DEFAULT_PITCH | FF_DONTCARE,
                         font_family);
    }
  return NULL;
}
/*----------------------------------------------------------------------
  WIN_LoadFont loads a Windows TrueType with a defined set of characteristics.
  ----------------------------------------------------------------------*/
static HFONT WIN_LoadFont (char script, int family, int highlight, int size)
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
#ifdef _WIN_PRINT
  nHeight = -MulDiv(size, DOT_PER_INCH, 80);
#else /* _WIN_PRINT */
  /*nHeight = -size;*/
  nHeight = -MulDiv(size, DOT_PER_INCH, 80);
#endif /* _WIN_PRINT */
  hFont = GetWinFontConfig (script, family, highlight, nHeight);
 
  if (hFont)
    return (hFont);

  nWidth = 0;
  fnWeight = FW_NORMAL;
  fdwItalic = FALSE;
  fdwUnderline = FALSE;
  fdwStrikeOut = FALSE;

  charset = DEFAULT_CHARSET;
  Win_Get_Charset (script, &charset, &family);
  Win_Get_FontFace (&lpszFace[0], script, family, &highlight);
  Win_Get_Highlight (highlight, &fdwItalic, &fnWeight);

  hFont = CreateFont (nHeight, nWidth, 0, 0, fnWeight,
                      fdwItalic, fdwUnderline, fdwStrikeOut,
                      charset, OUT_TT_ONLY_PRECIS, CLIP_DEFAULT_PRECIS,
                      PROOF_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
                      lpszFace);
  if (hFont == NULL)
    WinErrorBox (NULL, "CreateFont");
  return (hFont);
}

/*----------------------------------------------------------------------
  WinLoadFont : Load a Windows font in a Device context.
  ----------------------------------------------------------------------*/
HFONT WinLoadFont (HDC hdc, ThotFont font)
{
  /*if (font && LastUsedFont != font)*/
  {
    LastUsedFont = font; 
    if (ActiveFont)
      {
        SelectObject (hdc, GetStockObject (SYSTEM_FONT));
        DeleteObject (ActiveFont);
        ActiveFont = 0;
      } 
	  SetMapperFlags (hdc, 1);
    ActiveFont = WIN_LoadFont (font->FiScript, font->FiFamily,
                               font->FiHighlight, font->FiSize);
  }
  return (OldFont = SelectObject (hdc, ActiveFont));
}
#endif /* _WINGUI */

/*----------------------------------------------------------------------
  NumberOfFonts returns the number of fonts.
  ----------------------------------------------------------------------*/
int NumberOfFonts ()
{
  return MaxNumberOfSizes + 1;
}

/*----------------------------------------------------------------------
  GetCharsCapacity converts a pixel volume into a character volume
  ----------------------------------------------------------------------*/
int GetCharsCapacity (int volpixel, int frame)
{
  int char_size = 200;

  if (frame && frame < MAX_FRAME && ViewFrameTable[frame -1].FrMagnification)
    {
      char_size += ViewFrameTable[frame -1].FrMagnification * 10;
      if (char_size < 100)
        char_size = 100;
    }
  if (volpixel > char_size)
    return volpixel / char_size;
  else
    return volpixel;
}


/*----------------------------------------------------------------------
  CharacterWidth returns the width of a char in a given font.
  ----------------------------------------------------------------------*/
int CharacterWidth (int c, ThotFont font)
{
#ifdef _WINARAB
  SIZE                wsize;
  TEXTMETRIC          textMetric;
  HFONT               hOldFont, ActiveFont;
  HDC                 display;
#endif /*_WINARAB*/
#ifndef _GL
  int                 i = 0;
#endif /* _GL */
  int                 l = 0;
  ThotBool            isTAB = FALSE;
  
  if (font == NULL)
    return 0;
  else if (c == INVISIBLE_CHAR || c == ZERO_SPACE)
    return 1;

  if (c == START_ENTITY)
    c = '&';
  else if (c == TAB)
    {
      /* we use the SPACE width for the character TAB */
      c = SPACE;
      isTAB = TRUE;
    }
  else if (c == UNBREAKABLE_SPACE ||
           c == EN_SPACE || c == EN_QUAD ||
           c == FIG_SPACE)
    /* we use the SPACE width for the character TAB */
    c = SPACE;

  if (c == NEW_LINE || c == BREAK_LINE)
    /* characters NEW_LINE and BREAK_LINE are equivalent */
    l = 1;
  else
    {
#ifndef _GL
#ifdef _WINGUI
      if (font->FiScript == '6')
        {
#ifdef _WINARAB
          ActiveFont =  WIN_LoadFont (font->FiScript,font->FiFamily,
                                      font->FiHighlight, font->FiSize);
          if (TtPrinterDC != NULL)
            {
              display = TtPrinterDC;
              hOldFont = SelectObject (TtPrinterDC, ActiveFont);
            }
          else
            {
              display = GetDC(FrRef[1]);
              hOldFont = SelectObject (display, ActiveFont);
            }  
          GetTextMetrics (display, &textMetric);
          GetTextExtentPoint32 (display, (LPCTSTR) (&c), 1, (LPSIZE) (&wsize));
          return wsize.cx;
#else /*_WINARAB*/
          return 8;
#endif _WINARAB
        }  
      if (c == EM_QUAD || c == EM_SPACE || c == THICK_SPACE ||
          c == FOUR_PER_EM || c == SIX_PER_EM || c == PUNC_SPACE ||
          c == THIN_SPACE || c == HAIR_SPACE || c == MEDIUM_SPACE)
        {
          if (font->FiFirstChar <= 32 && font->FiLastChar >= 32)
            {
              l = font->FiWidths[32 - font->FiFirstChar];
              if (c == EM_QUAD || c == EM_SPACE)
                l = 2 * l;
              else if (c == THICK_SPACE)
                l = (2 * l) / 3;
              else if (c == FOUR_PER_EM || c == PUNC_SPACE || c == MEDIUM_SPACE)
                l = (l + 1) / 2;
              else if (c == SIX_PER_EM || c == THIN_SPACE)
                l = (l + 2) / 3;
              else if (c == HAIR_SPACE)
                l = (l + 3) / 4;
            }
        }
      else if (font->FiFirstChar <= c && 
               font->FiLastChar >= c)
        l = font->FiWidths[c - font->FiFirstChar];
      else if (font->FiScript == 'Z')
        l = font->FiAscent; /* MJD: Simple hack, works only approximately */
#endif  /* _WINGUI */
      
#ifdef _GTK
      if (c == EM_QUAD || c == EM_SPACE || c == THICK_SPACE ||
          c == FOUR_PER_EM || c == SIX_PER_EM || c == PUNC_SPACE ||
          c == THIN_SPACE || c == HAIR_SPACE || c == MEDIUM_SPACE)
        {
          l = gdk_char_width (font, 32);
          if (c == EM_QUAD || c == EM_SPACE)
            l = 2 * l;
          else if (c == THICK_SPACE)
            l = (2 * l) / 3;
          else if (c == FOUR_PER_EM || c == PUNC_SPACE ||
                   c == MEDIUM_SPACE)
            l = (l + 1) / 2;
          else if (c == SIX_PER_EM || c == THIN_SPACE)
            l = (l + 2) / 3;
          else if (c == HAIR_SPACE)
            l = (l + 3) / 4;
        }
      else if (c > 256)
        {
          l = gdk_text_width_wc (font, (GdkWChar *)&c, 2);
          if (l == 0)
            l = gdk_char_width (font, 32);
        }
      else
        l = gdk_char_width (font, c);
#endif /* _GTK */
#ifdef _WX
      /* TODO : a faire si on desir porter la version non opengl de wxwindows */
#endif /* _WX */
#ifndef _WINGUI
      if (c == 244)
        {
          /* a patch due to errors in standard symbol fonts */
          i = 0;
          while (i < MAX_FONT && font != TtFonts[i])
            i++;
          if (TtPatchedFont[i] == 8 || TtPatchedFont[i] == 10)
            l = 1;
          else if (TtPatchedFont[i] == 12 ||
                   TtPatchedFont[i] == 14)
            l = 2;
          else if (TtPatchedFont[i] == 24)
            l = 4;
        }
#endif /* _WINGUI*/
#else /* _GL */
      if (c == EM_QUAD || c == EM_SPACE || c == THICK_SPACE ||
          c == FOUR_PER_EM || c == SIX_PER_EM || c == PUNC_SPACE ||
          c == THIN_SPACE || c == HAIR_SPACE || c == MEDIUM_SPACE)
        {
          l = gl_font_char_width ((void *) font, 32);
          if (c == EM_QUAD || c == EM_SPACE)
            l = 2 * l;
          else if (c == THICK_SPACE)
            l = (2 * l) / 3;
          else if (c == FOUR_PER_EM || c == PUNC_SPACE ||
                   c == MEDIUM_SPACE)
            l = (l + 1) / 2;
          else if (c == SIX_PER_EM || c == THIN_SPACE)
            l = (l + 2) / 3;
          else if (c == HAIR_SPACE)
            l = (l + 3) / 4;
        }
      else if (c > 256)
        {
          l = gl_font_char_width ((void *) font, (CHAR_T) c);
          if (l == 0)
            l = 7/*gl_font_char_width ((void *) font, 32)*/;
        }
      else
        l = gl_font_char_width ((void *) font, (CHAR_T) c);
#endif /* _GL */
      if (isTAB)
        l *= 8;
    }
  return l;
}

/*----------------------------------------------------------------------
  SpecialCharBoxWidth : size of special unicode chars..
  ----------------------------------------------------------------------*/
int SpecialCharBoxWidth (CHAR_T c)
{
  if (c == 0x202A /* lre */ || c == 0x200B /* zwsp*/ ||
      c == 0x200C /* zwnj*/ || c == 0x200D /* zwj */ ||
      c == 0x200E /* lrm */ || c == 0x200F /* rlm */ ||
      c == 0x202B /* rle */ || c == 0x202C /* pdf */ ||
      c == 0x202D /* lro */ || c == 0x202E /* rlo */ ||
      c == 0x2061 /* ApplyFunction */ ||
      c == 0x2062 /* InvisibleTimes */ || c == 0x2063 /*InvisibleComma*/)
    return 1;
  else
    return 0;
}
/*----------------------------------------------------------------------
  BoxCharacterWidth returns the width of a char in a given font
  ----------------------------------------------------------------------*/
int BoxCharacterWidth (CHAR_T c, int variant, SpecFont specfont)
{
  ThotFont        font;
  int             car = 6;

  if (SpecialCharBoxWidth (c))
    return 1;
  if (specfont)
    car = GetFontAndIndexFromSpec (c, specfont, variant, &font);
  else
    font = NULL;
  if (font == NULL)
    return 6;
  else
    return CharacterWidth (car, font);
}
/*----------------------------------------------------------------------
  CharacterHeight returns the height of a char in a given font
  ----------------------------------------------------------------------*/
int CharacterHeight (int c, ThotFont font)
{
  int              l;

  if (font == NULL)
    return (0);
#ifdef _GL
  else
    l = gl_font_char_height (font, (CHAR_T *) &c);
#else /*_GL*/
  else if (font->FiFirstChar <= c && font->FiLastChar >= c)
    l = font->FiHeights[c - font->FiFirstChar];
  else
    l = font->FiHeight;
#endif /*_GL*/
  return l;
}

/*----------------------------------------------------------------------
  CharacterAscent returns the ascent of a char in a given font.
  ----------------------------------------------------------------------*/
int CharacterAscent (int c, ThotFont font)
{
#ifdef _WX
  int		    i;
  int       ascent;
#endif /* _WX */

  if (font == NULL)
    return (0);
#ifdef _GL
  else
    ascent = gl_font_char_ascent (font, (CHAR_T *) &c);

#ifdef _WX
  if (c == 244)
    {
      i = 0;
      while (i < MAX_FONT && font != TtFonts[i])
        i++;
      if (TtPatchedFont[i])
        ascent -= 2;
    }
#endif /* _WX */
  return (ascent);
#else /*_GL*/
  else
    return font->FiAscent;
#endif /*_GL*/
}

/*----------------------------------------------------------------------
  FontAscent returns a global ascent for a font.
  ----------------------------------------------------------------------*/
int FontAscent (ThotFont font)
{
  if (font == NULL)
    return (0);
#ifdef _GL
  return (gl_font_ascent(font));
#endif /*_GL*/
#ifdef _WINGUI
  return (font->FiAscent);
#endif /* _WINGUI */
#ifdef _GTK
  return (font->ascent);
#endif /* _GTK */
}

/*----------------------------------------------------------------------
  FontHeight returns the height of a given font.
  ----------------------------------------------------------------------*/
int FontHeight (ThotFont font)
{
  if (font == NULL)
    return (0);
#ifdef _GL
  return (gl_font_height (font));
#endif /*_GL*/
#ifdef _WINGUI
  return (font->FiHeight);
#endif /* _WINGUI */
#ifdef _GTK
  return (font->ascent + font->descent);
#endif /* _GTK */
}

/*----------------------------------------------------------------------
  BoxFontHeight returns the height of a given font.
  ----------------------------------------------------------------------*/
int BoxFontHeight (SpecFont specfont, char code)
{
  ThotFont        font;
  int             car, h = 0;

  if (specfont)
    {
      if (code == 'A' && specfont->Font_6) /* Arabic */
        h = FontHeight (specfont->Font_6);
      else if (code == 'G' && specfont->Font_7) /* Graphic */
        h = FontHeight (specfont->Font_7);
      else if (code == 'H' && specfont->Font_8) /* Hebrew */
        h = FontHeight (specfont->Font_8);
      else if (code == 'Z' && specfont->Font_17) /* Unicode */
        h = FontHeight (specfont->Font_17);
      else if (specfont->Font_1)
        h = FontHeight (specfont->Font_1);
    }
  if (h == 0 && specfont)
    {
      car = GetFontAndIndexFromSpec (120, specfont, 1, &font);
      h = FontHeight (font);
    }
  return h;
}

/*----------------------------------------------------------------------
  PixelValue converts a logical value into a pixel value.
  pAb is the current abstract box except for UnPercent unit
  where it holds the comparison value.
  ----------------------------------------------------------------------*/
int PixelValue (int val, TypeUnit unit, PtrAbstractBox pAb, int zoom)
{
  int              dist, i, h;

  dist = 0;
  switch (unit)
    {
    case UnRelative:
    case UnXHeight:
      if (pAb == NULL || pAb->AbBox == NULL ||
          pAb->AbBox->BxFont == NULL)
        dist = 0;
      else
        {
          if (pAb->AbLeafType != LtText && pAb->AbLeafType != LtSymbol)
            h = GetCurrentFontHeight (pAb->AbSize, pAb->AbSizeUnit, zoom);
          else
            h = BoxFontHeight (pAb->AbBox->BxFont, EOS);
          if (unit == UnRelative)
            dist = (val * h + 5) / 10;
          else
            dist = (val * h + 5) / 20;
        }
      break;
    case UnPoint:
      /* take zoom into account */
      if (zoom != 0)
        {
          if (zoom < -9)
            zoom = -9;
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
          if (zoom < -9)
            zoom = -9;
          dist = val + (val * zoom / 10);
          if (dist == 0 && val > 0)
            dist = 1;
        }
      else
        dist = val;
#ifdef _WIN_PRINT
      if (TtPrinterDC && ScreenDPI)
        dist = (dist * PrinterDPI + ScreenDPI / 2) / ScreenDPI;
#endif /* _WIN_PRINT */
      break;
    case UnPercent:
      i = val * (long int) pAb;
      dist = (i + 50) / 100;
      break;
    default:
      /* should not occur: auto reserved for margins */
      break;
    }
  return (dist);
}

/*----------------------------------------------------------------------
  LogicalValue converts a pixel value into a logical value.
  pAb is the current abstract box except for UnPercent unit
  where it holds the comparison value.
  ----------------------------------------------------------------------*/
int LogicalValue (int val, TypeUnit unit, PtrAbstractBox pAb, int zoom)
{
  int              dist, i, h;

  dist = 0;
  switch (unit)
    {
    case UnRelative:
    case UnXHeight:
      if (pAb == NULL || pAb->AbBox == NULL || 
          pAb->AbBox->BxFont == NULL)
        dist = 0;
      else
        {
          if (pAb->AbLeafType != LtText && pAb->AbLeafType != LtSymbol)
            h = GetCurrentFontHeight (pAb->AbSize, pAb->AbSizeUnit, zoom);
          else
            h = BoxFontHeight (pAb->AbBox->BxFont, EOS);
          if (unit == UnRelative)
            dist = val * 10 / h;
          else
            dist = val * 20 / h;
        }
      break;
    case UnPoint:
      /* take zoom into account */
      if (zoom != 0)
        {
          if (zoom < -9)
            zoom = -9;
          dist = (10 * val) / (10 + zoom);
          if (dist == 0 && val > 0)
            dist = 1;
        }
      else
        dist = val;
#ifndef _WIN_PRINT
      /*if (!Printing)*/
      /* Postscript unit is the point instead of the pixel */
#endif /* _WIN_PRINT */
      dist = PixelToPoint (dist);
      break;
    case UnPixel:
      /* take zoom into account */
      if (zoom != 0)
        {
          if (zoom < -9)
            zoom = -9;
          dist = (10 * val) / (10 + zoom);
          if (dist == 0 && val > 0)
            dist = 1;
        }
      else
        dist = val;
#ifdef _WIN_PRINT
      if (TtPrinterDC && PrinterDPI)
        dist = (dist * ScreenDPI + PrinterDPI / 2) / PrinterDPI;
#endif /* _WIN_PRINT */
      break;
    case UnPercent:
      if (pAb == NULL)
        dist = 0;
      else
        {
          i = val * 100;
          dist = i / (long int) pAb;
        }
      break;
    default:
      /* Should not occur. Auto is reserved for margins */
      break;
    }
  return (dist);
}


/*----------------------------------------------------------------------
  FontBase returns the shifting of the base line for a given font.
  ----------------------------------------------------------------------*/
int FontBase (ThotFont font)
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
  ThotFont        font;
  unsigned char   car;

  car = GetFontAndIndexFromSpec (120, specfont, 1, &font);
  return FontBase (font);
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
  ThotFontPointSize convert a logical size to the point value.
  ----------------------------------------------------------------------*/
int ThotFontPointSize (int size)
{

  if (size > MaxNumberOfSizes)
    size = MaxNumberOfSizes;
  else if (size < 0)
    size = 0;

  return (LogicalPointsSizes[size]);
}

/*----------------------------------------------------------------------
  GeneratePostscriptFont : 
  As Postscript name serves also for the font cache
  ----------------------------------------------------------------------*/
static void GeneratePostscriptFont (char r_name[10], char script, int family,
                                    int highlight, int size)
{
  
  const char        *cfamily = "sthc";
  const char        *efamily = "sthcklmnopqruvwxyzabd";

  if (script != 'E')
    {
      if (family > (int) strlen (cfamily))
      	family = 1;
      sprintf (r_name, "%c%c%c%d", TOLOWER (script), 
               cfamily[family], StylesTable[highlight], size);
    }
  else
    {
      /* As poscript name serves also for the font cache
         we enable it for esstix fonts*/
      /* generate the Postscript name */
      if (family > (int) strlen (efamily))
        family = 1;
      sprintf (r_name, "%c%c%c%i", TOLOWER (script), efamily[family],
               StylesTable[highlight], size);
    }
}

/*----------------------------------------------------------------------
  GetFontIdentifier computes the name of a Thot font.
  ----------------------------------------------------------------------*/
void GetFontIdentifier (char script, int family, int highlight, int size,
                        TypeUnit unit, char r_name[10])
{
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
  if (script == '1')
    script = 'L';
  else if (script != 'A' && script != 'G' && script != 'Z' && script != 'E')
    {
      if (size < 0)
        size = 12;
    }
  else if (script == 'G' || family == 0)
    {
      family = 0;
      highlight = 0;
    }
  GeneratePostscriptFont (r_name, script, family, highlight, size);
}
 
/*----------------------------------------------------------------------
  GetPostscriptNameFromFont : Get Postscript Font name from the font 
  pointer indentification
  ----------------------------------------------------------------------*/
char *GetPostscriptNameFromFont (void * font, char *fontname)
{
  int                 i, result;
  char                c0, c1, c2;
  
  /* browse the table of fonts */
  i = 0;
  result = 0;
  while (TtFonts[i] != font && i < MAX_FONT)
    i++;
  if (i >= MAX_FONT)
    i = 0;
  i = i * 8;

  if (TtPsFontName[i] == 'g')  /* Greek script */
    {
      c0 = TtPsFontName[i];
      c1 = TtPsFontName[i];
      c2 = 'r';	     /* Symbol only has one style available */	
    }
  else
    {
      /* Latin Script */
      c0 = 'l';
      c1 = TtPsFontName[i + 1]; /* font Helvetica Times Courrier */
      /* convert lowercase to uppercase */
      c2 = TtPsFontName[i + 2]; /* Style normal bold italique */	 
    }
  sprintf (fontname, "%c%c%c %s sf\n", c0, c1, c2,  &TtPsFontName[i + 3]);
  return fontname;
}

/*----------------------------------------------------------------------
  LoadNearestFont load the nearest possible font given a set of attributes
  like script, family, the size and for a given frame.
  Parameters increase decrease are true when a new test is allowed.
  The parameter requestedsize gives the initial requested size expressed
  in points.
  ----------------------------------------------------------------------*/
ThotFont LoadNearestFont (char script, int family, int highlight,
                          int size, int requestedsize, int frame,
                          ThotBool increase, ThotBool decrease)
{
  int                 i, j, deb, free_entry;
  int                 val;
  unsigned int		    mask;
  char                text[MAX_FONTNAME];
#ifdef _WINGUI
  SIZE                wsize;
  TEXTMETRIC          textMetric;
  int                 c, ind, space = 32;
  HFONT               hOldFont;
  HDC                 display;
#endif /* _WINGUI */
  ThotFont            ptfont;
  
  GetFontIdentifier (script, family, highlight, size, UnPoint, text);
  /* Font cache lookup */
  i = 0;
  deb = 0;
  ptfont = NULL;
  free_entry = FirstFreeFont;
  while (ptfont == NULL && i < FirstFreeFont)
    {
      if (TtFonts[i] == NULL)
        /* a free entry is found */
        free_entry = i;
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
      if (free_entry < MAX_FONT)
        {
          /* No table overflow: load the new font */
#ifdef _GL
          //printf("tex[%d]=%s \n", free_entry, text);
          ptfont = (ThotFont)GL_LoadFont (script, family, highlight, size);
#else /*_GL*/
          /* Allocate the font structure */
          ActiveFont = WIN_LoadFont (script, family, highlight, size);
          if (ActiveFont)
            {
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
              ptfont = TtaGetMemory (sizeof (FontInfo));
              ptfont->FiScript = script;
              ptfont->FiFamily = family;
              ptfont->FiHighlight = highlight;
              ptfont->FiSize = size;
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
              ptfont->FiFirstChar = textMetric.tmFirstChar;
              ptfont->FiLastChar = textMetric.tmLastChar;
              val = textMetric.tmLastChar - textMetric.tmFirstChar + 1;
              ptfont->FiWidths = (int *) TtaGetMemory (val * sizeof (int));
              ptfont->FiHeights = (int *) TtaGetMemory (val * sizeof (int));
              c = textMetric.tmFirstChar;
              for (ind = 0; ind < val; ind ++)
                {
                  GetTextExtentPoint (display, (LPCTSTR) (&c),
                                      1, (LPSIZE) (&wsize));
                  if (wsize.cx == 0)
                    GetTextExtentPoint (display, (LPCTSTR) (&space),
                                        1, (LPSIZE) (&wsize));
                  ptfont->FiWidths[ind] = wsize.cx;
                  ptfont->FiHeights[ind] = wsize.cy;
                  c++;
                }
              DeleteObject (ActiveFont);
              ActiveFont = 0;
              if (TtPrinterDC == NULL && display)
                ReleaseDC (FrRef[frame], display);
            }
          else
            ptfont = NULL;
          /* Loading failed try to find another size */
          if (ptfont == NULL)
            {
              /* Change size */
              if (increase)
                {
                  if (size >= requestedsize + 10)
                    {
                      /* check a smaller size */
                      increase = FALSE;
                      decrease = TRUE;
                      size = requestedsize;
                    }
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
                  if (size < 3)
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
#endif/*  _GL */
        }
      
      if (ptfont == NULL)
        {
          if (free_entry < MAX_FONT &&
              script != '1' && script != 'L' && script != 'G' && size != -1)
            /* try without highlight and no specific size */
            ptfont = LoadNearestFont (script, family, 0,
                                      -1, requestedsize, frame, FALSE, FALSE);
          else
            {
              /* Try to load another family from the same script and family */
              for (j = 0; j < FirstFreeFont; j++)
                {
                  if (TtFonts[j] &&
                      TtFontName[j * MAX_FONTNAME] == text[0] &&
                      TtFontName[j * MAX_FONTNAME + 1] == text[1])
                    {
                      i = j;
                      ptfont = TtFonts[i];
                      j = FirstFreeFont;  // stop now
                    }
                }
              if (ptfont == NULL && script != 'E')
                {
                  for (j = 0; j < FirstFreeFont; j++)
                    {
                      if (TtFonts[j] &&
                          TtFontName[j * MAX_FONTNAME] == text[0])
                        {
                          i = j;
                          ptfont = TtFonts[i];
                          j = FirstFreeFont;  // stop now
                        }
                    }
                }
            }
          
          if (ptfont == NULL && script == '7' && free_entry < MAX_FONT)
            {
              /* look for a font Symbol */
              ptfont = LoadNearestFont ('G', family, 0, -1, requestedsize,
                                        frame, FALSE, FALSE);
              if (ptfont)
                /* now we'll work with the Greek font */
                GreekFontScript = 'G';
              else
                {
                  ptfont = (ThotFont)LoadStixFont ('E', 10);
                  if (ptfont)
                    /* now we'll work with the Stix font */
                    GreekFontScript = 'E';
                }
            }
          
          /* last case use the default font */
          if (ptfont == NULL)
            {
#ifdef _GL
              ptfont = DefaultGLFont;
#else /* _GL */
              ptfont = DialogFont;
#endif /* _GL */
              /* no entry in the list of fonts */
              return ptfont;
            }
        }
    }
  
  if (ptfont && size == requestedsize)
    {
      if (i == FirstFreeFont && FirstFreeFont >= MAX_FONT)
        printf ("Too many fonts\n");
      if ((i == FirstFreeFont && FirstFreeFont < MAX_FONT) ||
          TtFonts[i] == NULL)
          FirstFreeFont = i + 1;
      if (i < MAX_FONT && TtFonts[i] == NULL)
        {
          /* initialize a new entry */
          strcpy (&TtFontName[deb], text);
          strcpy (&TtPsFontName[i * 8], text);
          TtFonts[i] = ptfont;
          TtFontMask[i] = 0;       
#ifdef _WX
          val = size;
          if (script == 'G' &&
              (val == 8 || val == 10 || val == 12 ||
               val == 14 || val == 24))
            TtPatchedFont[i] = val;
#endif /* _WX */
        }
      /* rely to the current frame */
      mask = 1 << (frame - 1);
      TtFontMask[i] = TtFontMask[i] | mask;
    }
  return (ptfont);
}

/*----------------------------------------------------------------------
  LoadStixFont : Load a stix font
  ----------------------------------------------------------------------*/
void *LoadStixFont (int family, int size)
{
#if !defined(_GL) && !defined(_WINGUI)
  if (Printing)
    return NULL;
#endif /* !defined(_GL) && !defined(_WINGUI */
  return ((void *) LoadNearestFont ('E', family, 0, size, size, ActiveFrame,
                                    FALSE, FALSE));
}


/*----------------------------------------------------------------------
  ChangeFontsetSize : Change fontset size
  ----------------------------------------------------------------------*/
void ChangeFontsetSize (int size, PtrBox box, int frame)
{
  SpecFont fontset, fontsetbase = box->BxFont;
  char code = box->BxScript;

  fontset = fontsetbase;
  if (fontset)
    {
      while (fontset)
        {
          if (fontset->FontSize == size)
            break;
          fontset = fontset->NextFontSet;
        }
      if (fontset)
        box->BxFont = fontset;
      else
        fontset = fontsetbase;
      fontset->FontSize = size;
      fontset->Font_1 = LoadNearestFont (code,
                                         fontset->FontFamily,
                                         fontset->FontHighlight,
                                         size, size,
                                         frame, TRUE, TRUE);
    }
}

/*----------------------------------------------------------------------
  GetCapital converts lower case into upper case characters
  ----------------------------------------------------------------------*/
static CHAR_T GetCapital (CHAR_T c)
{
  if ((c >= 0x61 /* a */ && c <= 0x7A /* z */) ||
      (c >= 0xE0 /* a` */ && c <= 0xFD /* y' */ && c != 0xF7))
    return c - 32;
  else
    return c;
}

/*----------------------------------------------------------------------
  GetFontAndIndexFromSpec returns the glyph index and the font
  used to display the wide character c.
  Generate smallcaps if variant is 2
  ----------------------------------------------------------------------*/
int GetFontAndIndexFromSpec (CHAR_T c, SpecFont fontset, int variant,
                             ThotFont *font)
{
  ThotFont           lfont, *pfont;
  CHARSET            encoding;
  CHAR_T             ref_c = c;
  char               code = ' ';
  int                car;
  int                frame, size, highlight;
  unsigned int       mask, fontmask;
  
  *font = NULL;
  if (variant == 2)
    c = GetCapital (c);
  if (c == EOS)
    c = ref_c;
  car = EOS;
  if (fontset)
    {
      highlight = fontset->FontHighlight;
      size = fontset->FontSize;
      if (c == EOL || c == BREAK_LINE ||
          c == SPACE || c == TAB ||
          c == NEW_LINE || c == UNBREAKABLE_SPACE ||
          c == EN_QUAD || c == EM_QUAD ||
          c == EN_SPACE || c == EM_SPACE ||
          c == THICK_SPACE || c == FOUR_PER_EM ||
          c == SIX_PER_EM || c == FIG_SPACE ||
          c == PUNC_SPACE || c == THIN_SPACE ||
          c == HAIR_SPACE || c == MEDIUM_SPACE)
        {
          /* various spaces */
          *font = fontset->Font_1;
          car = (int) c;
        }
      else if ((c <= 0xFF && c == ref_c) || /* 0 -> FF */
               c == 0x152  /*oe*/     || c == 0x153  /*OE*/ ||
               c == 0x178  /*ydiaeresis*/ ||
               c == 0x2C6  /*circ*/   || c == 0x2DC  /*tilde*/ ||
               c == 0x2013 /*ndash*/  || c == 0x2014 /*mdash*/ ||
               (c >= 0x2018 && c <= 0x201E) /*quotes*/ ||
               c == 0x2026 /*hellip*/ ||
               c == 0x2039 /*inf*/    || c == 0x203A /*sup*/ ||
               c == 0x20AC /*euro*/)
        {
          *font = fontset->Font_1;
          car = (int) c;
          code = 1;
        }
      else if (c == 0x202A /* lre */ || c == 0x200B /* zwsp*/ ||
               c == 0x200C /* zwnj*/ || c == 0x200D /* zwj */ ||
               c == 0x200E /* lrm */ || c == 0x200F /* rlm */ ||
               c == 0x202B /* rle */ || c == 0x202C /* pdf */ || 
               c == 0x202D /* lro */ || c == 0x202E /* rlo */ ||
               c == 0x2061 /*ApplyFunction*/ ||
               c == 0x2062 /*InvisibleTimes*/ || c == 0x2063 /*InvisibleComma*/)
        {
          c =  INVISIBLE_CHAR;
          code = '1';
        }
      else
        {
          if (c != ref_c)
            {
              // generate small-caps
              car = (int)c;
              pfont = &(fontset->Font_19);
              encoding = UNICODE_1_1;
              code = '1';
              size = (int) (size * .8);
            }
          else if ((c >= 0x370 && c < 0x3FF) ||
                   (c >= 0x1F00 && c < 0x1FFF) ||
                   c == 0x20d2 || c == 0x2758 /* Vertical Bars */ ||
                   c == 0x2A2F /* vector or cross product */ ||
                   c == 0x2970 /* roundimplies */ ||
                   c == 0x220F || c == 0x2211 /* summation signs */ )
            {
              /* Greek characters */
              code = '7';
              pfont = &(fontset->Font_7);
#ifndef _GL
              encoding = WINDOWS_1253;
              if (c == 0x3C2 || c == 0x3D1 ||
                  c == 0x3D2 || c == 0x3D5 ||
                  c == 0x3D6)
                /* final sigma, thetasym, upsih, phi, piv */
                /* use the Symbol font */
                {
                  code = 'G';
                  pfont = &(fontset->Font_16);
                  encoding = ISO_SYMBOL;
                }
#endif /* _GL */
            }
#ifdef _GL
          else if ((c >= 0x2100 && c <= 0x22FF) /* math symbols */ ||
                   c == 0x20DB || c == 0x20DC /* dots */ ||
                   c == 0x2036 /* reversed double prime */ ||
                   c == 0x2037 /* reversed double prime */ ||
                   c == 0x2044 /* fraction slash */ ||
                   (c >= 0xfe35 && c <= 0xfe38) /* over braces / parenthesis */ ||
                   (c >= 0x1D49C && c <= 0x1D7E1) /* Alphanumeric symbols */ ||
                   c == 0x2720 /* maltese cross */ ||
                   c == 0x260E /* phone */ )
            {
              /* use STIX fonts here */
              code = 'E';
              car = GetStixFontAndIndex (c, fontset, &pfont);
              if (pfont == NULL )
                {
                  code = '7'; /* West Europe Latin */
                  pfont = &(fontset->Font_7);
                  if (c == 0x20d2 || c == 0x2758 /* Vertical Bars */)
                    c = '|';
                  else if(c == 0x2215 /* division slash */ || c == 0x2044)
                    c = '/';
                  else if(c == 0x2216 /* set minus */)
                    c = '\\';
                  else if(c == 0x02A2F /* vector or cross product */)
                    c = 215;
                  else if (c == 0x2148 /* ImaginaryI */)
                    c = 105;
                  else if (c == 0x2145 /* CapitalDifferentialD */)
                    c = 68;
                  else if (c == 0x2146 /* DifferentialD */)
                    c = 100;
                  else if (c == 0x210E /* planckh */)
                    c = 104;
                  else  if (c == 0x2147 /* ExponentialE */)
                    c = 101;
                  else  if (c == 0x220A /* small element of */)
                    c = 0x3F5;
                  else  if (c == 0x220D /* small contains as member */)
                    c = 0x3F6;
                }
            }
#else /* _GL */
          else if (c == 0x210E /* planckh */ ||
                   c == 0x2145 /* CapitalDifferentialD */ ||
                   c == 0x2146 /* DifferentialD */ ||
                   c == 0x2147 /* ExponentialE */ ||
                   c == 0x2148 /* ImaginaryI */ ||
                   c == 0x20d2 || c == 0x2758 /* Vertical Bars */ ||
                   c == 0x2216 /* set minus */ ||
                   c == 0x02A2F /* vector or cross product */)
            {
              code = '7'; /* West Europe Latin */
              pfont = &(fontset->Font_7);
              encoding = WINDOWS_1252;
              if (c == 0x20d2 || c == 0x2758 /* Vertical Bars */)
                c = '|';
              else if(c == 0x2216) /* set minus */
                c = '\\';
              else if(c == 0x02A2F /* vector or cross product */)
                c = 215;
              else if (c == 0x2148 /* ImaginaryI */)
                c = 105;
              else if (c == 0x2146 /* CapitalDifferentialD */)
                c = 68;
              else if (c == 0x2146 /* DifferentialD */)
                c = 100;
              else if (c == 0x210E /* planckh */)
                c = 104;
              else /* ExponentialE */
                c = 101;
            }
#endif /* _GL */
          else if ((c > 0x2000 && c < 0x237F &&  /* mathematical characters */
                    (c < 0x2018 || c > 0x201D)) || /* Windows quotations */
                   c == 0x192  ||  /* latin small letter f with hook */
                   c == 0x25CA ||  /* lozenge */
                   c == 0x260E ||  /* black telephone */
                   c == 0x2660 ||  /* black spade suit */
                   c == 0x2663 ||  /* black club suit */
                   c == 0x2665 ||  /* black heart suit */
                   c == 0x2666 ||  /* black diamond suit */
                   c == 0x2720 ||  /* maltese cross */
                   c == 0x27FA ||  /* Long left right double arrow */
                   c == 0x2970     /* roundimplies */
                                  )
            {
#ifdef _GL
             if (c == 0x220F || c == 0x2211)
                /* an oversized product or summation sign. Use the Symbol
                   font: these characters are ill-aligned in Esstix */
                {
                  code = '7';
                  pfont = &(fontset->Font_7);
                }
              else
                {
                  /* use Esstix fonts */
                  code = 'E';
                  car = GetStixFontAndIndex (c, fontset, &pfont);
                  if (pfont == NULL)
                    {
                      code = '1';
                      pfont = &(fontset->Font_1);
                    }
                }
#else /* _GL */
		          /* Symbols */
              code = 'G';
              pfont = &(fontset->Font_16);
              encoding = ISO_SYMBOL;
#endif /* _GL */
            }
          else if (c == 0x11F || c == 0x130 || c == 0x131 || c == 0x15F)
            {
              code = '9'; /* Turkish */
              pfont = &(fontset->Font_9);
#ifndef _GL
              encoding = WINDOWS_1254;
#endif /* _GL */
            }
          else if (c < 0x17F)
            {
              code = '2'; /* Central Europe */
              pfont = &(fontset->Font_2);
#ifndef _GL
              encoding = WINDOWS_1250;
#endif /* _GL */
            }
          else if (c < 0x24F)
            {
              code = '3';
              pfont = &(fontset->Font_3);
#ifndef _GL
              encoding = WINDOWS_1250;
#endif /* _GL */
            }
          else if (c < 0x2AF)
            {
              code = '4'; /* Baltic RIM */
              pfont = &(fontset->Font_4);
#ifndef _GL
              encoding = WINDOWS_1257;
#endif /* _GL */
            }
          else if (c < 0x45F)
            {
              code = '5'; /* Cyrillic */
              pfont = &(fontset->Font_5);
#ifndef _GL
              encoding = WINDOWS_1251;
#endif /* _GL */
            }
          else if (c < 0x5FF)
            {
              code = '8'; /* Hebrew */
              pfont = &(fontset->Font_8);
#ifndef _GL
              encoding = WINDOWS_1255;
#endif /* _GL */
            }
          else if (c < 0x5FF)
            {
              code = '9'; /* Turkish */
              pfont = &(fontset->Font_9);
#ifndef _GL
              encoding = WINDOWS_1254;        
#endif /* _GL */
            }
          else if (c < 0x65F)
            {
              code = '6'; /* Arabic */
              pfont = &(fontset->Font_6);
#ifndef _GL
              encoding = WINDOWS_1256;
#endif /* _GL */
            }
          else if (c >= 0x25A0 && c <= 0x25F7)
            /* geometric shapes */
            {
              code = '1';
              pfont = &(fontset->Font_1);
              encoding = UNICODE_1_1;
            }
          else if (c <= 0x10FF && c >= 0x10A0)
            {
              // Georgian
              code = 'X'; /* Unicode */
              pfont = &(fontset->Font_18);
              encoding = UNICODE_1_1;
            }
          else
            {
              code = 'Z'; /* Unicode */
              pfont = &(fontset->Font_17);
              encoding = UNICODE_1_1;
            }
          
          if (pfont)
            {
              /* attach that font to the current frame */
              lfont = *pfont;
              if (code != 'E')
                {
                  for (frame = 1; frame <= MAX_FRAME; frame++)
                    {
                      mask = 1 << (frame - 1);
                      fontmask = fontset->FontMask;
                      if (fontmask & mask)
                        {
                          lfont = LoadNearestFont (code, fontset->FontFamily,
                                                   highlight,
                                                   size, size,
                                                   frame, TRUE, TRUE);
                          if (code == '7' && GreekFontScript == 'G')
                            /* use the font Symbol instead of a greek font */
                            encoding = ISO_SYMBOL;
                        }
                    }
                  /* even if the font is not found avoid to retry later */
                  *pfont = lfont;
                }
              *font = lfont;
            }
          else
            *font = NULL;
          
          if (*font == NULL ||
              (*font == DialogFont && code != '1'))
            {
              c = UNDISPLAYED_UNICODE;
              *font = NULL;
            }
          else if (code == 'Z' || code == '6')
            car = c;
          else if (code != 'E')
#ifdef _GL
            car = c;
#else /* _GL */
          car = (int)TtaGetCharFromWC (c, encoding);
#endif /* _GL */
        }
    }   
  if (car == EOS && (code == 'E' || code == 'G'))
    {
      /* generate a square */
      car = UNDISPLAYED_UNICODE;
      *font = NULL;
    }
  
  
#ifdef _GL
  if (code == 'E' || code == 'G')
    return car;
  else
    return c;
#else /*_GL*/
  return car;
#endif /*_GL*/
}

/*----------------------------------------------------------------------
  LoadFontSet allocate a font set and load the ISO-latin-1 font.
  ----------------------------------------------------------------------*/
static SpecFont LoadFontSet (char script, int family, int highlight,
                             int size, TypeUnit unit, int frame)
{
  int                 index, i;
  SpecFont            prevfontset, fontset;
  unsigned int        mask, fontmask;
  ThotBool            specificFont = (script == 'G');

  index = 0;
  /* work with point sizes */
  if (unit == UnRelative)
    index = LogicalPointsSizes[size];
  else
    index = size;

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
      fontset = (FontSet*)TtaGetMemory (sizeof (FontSet));
      if (fontset)
        {
          memset (fontset, 0, sizeof (FontSet));
          fontset->specificFont = specificFont;
          fontset->FontFamily = family;
          fontset->FontHighlight = highlight;
          fontset->FontSize = index;
          fontmask = fontset->FontMask;
          fontset->FontMask = fontmask | mask;
          fontset->Font_1 = LoadNearestFont (script, family, highlight,
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
  else if ((fontset->FontMask & mask) == 0)
    {
      /* add the window frame number */
      fontmask = fontset->FontMask;
      fontset->FontMask = fontmask | mask;
      /* attach that font to the frame */
	  if (fontset->Font_1)
	  {
        for (i = 0; i < FirstFreeFont; i++)
          if (TtFonts[i] == fontset->Font_1)
		  {
 		    TtFontMask[i] = TtFontMask[i] | mask;
			i = FirstFreeFont;
		  }
	  }
	  else
        fontset->Font_1 = LoadNearestFont (script, family, highlight,
                                         index, index, frame, TRUE, TRUE);
    }
  return (fontset);
}

/*----------------------------------------------------------------------
  ThotLoadFont try to load a font given a set of attributes like script,
  family, the size and variant for a given frame.
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
        size = PixelToPoint (size);
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
              size = ThotFontPointSize (size);
              unit = UnPoint;
            }
          size = size * FontZoom / 100;
        }
      zoom = ViewFrameTable[frame - 1].FrMagnification;
      if (zoom)
        {
          if (unit == UnRelative)
            {
              size = ThotFontPointSize (size);
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
  GetCurrentFontHeight returns the font height without loading the font
  ----------------------------------------------------------------------*/
int GetCurrentFontHeight (int size, TypeUnit unit, int zoom)
{
  if (unit == UnRelative)
    {
      size = ThotFontPointSize (size);
      unit = UnPoint;
    }
  if (zoom)
    size = size + (size * zoom / 10);
  /* the minimum size is 6 points */
  if (size < 6 && unit == UnPoint)
    size = 6;
  return size;
}

/*----------------------------------------------------------------------
  InitDialogueFonts initialize the standard fonts used by the Thot Toolkit.
  ----------------------------------------------------------------------*/
void InitDialogueFonts (const char *name)
{
#ifdef _GTK
  int              ndir, ncurrent;
  char            FONT_PATH[128];
  char            *fontpath;
  char           **dirlist = NULL;
  char           **currentlist = NULL;
#endif /* _GTK */
  char            *value;
  char             script;
  int              f_size;
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
  MaxNumberOfSizes = 10;
#ifdef _WINGUI
  GreekFontScript = '7';
#endif /* _WINGUI */
#if defined(_GTK) || defined(_WX)
  if (Printing)
    /* Only the sysmbol font is available in Postscript */
    GreekFontScript = 'G';
  else
    GreekFontScript = '7';
#endif /* #if defined(_GTK) || defined(_WX) */
  
  FontFamily = (char *)TtaGetMemory (8);
  strcpy (FontFamily, "-*");
  /* Is there any predefined size for menu fonts ? */
  value = TtaGetEnvString ("FontMenuSize");
  if (value != NULL)
    sscanf (value, "%d", &MenuSize);
  f_size = MenuSize + 2;

#if defined(_GTK)
  if (!Printing)
    {
      fontpath = TtaGetEnvString ("THOTFONT");
      if (fontpath && TtaDirExists (fontpath))
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
              TtaFreeMemory (dirlist);
            }
          TtaFreeMemory (currentlist);
        }
    }
  for (i = 0; i < MAX_FONT; i++)
    TtPatchedFont[i] = 0;
#endif /* _GTK */

  /* Initialize the Thot Lib standards fonts */
  DialogFont = IDialogFont = LargeDialogFont = NULL;
  GraphicsIcons = NULL;
  SmallDialogFont = NULL;

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

  DialogFont = NULL;
#ifdef _WX
  IDialogFont = NULL;
  LargeDialogFont = NULL;
#endif /* _WX */
#ifdef _GL
  /* Need a default GL font because the format is different */
  DefaultGLFont = NULL;
  i = 1;
  while (DefaultGLFont == NULL && i < 3)
    {
      DefaultGLFont = (ThotFont)GL_LoadFont ('L', i, 1, LogicalPointsSizes[3]);
      if (!DefaultGLFont)
        DefaultGLFont = (ThotFont)GL_LoadFont ('L', i, 1, LogicalPointsSizes[4]);
      if (!DefaultGLFont)
        DefaultGLFont = (ThotFont)GL_LoadFont ('L', i, 1, LogicalPointsSizes[2]);
      if (!DefaultGLFont)
        DefaultGLFont = (ThotFont)GL_LoadFont ('L', i, 1, LogicalPointsSizes[1]);
      i++;
    }
  if (DefaultGLFont == NULL)
    printf ("Cannot load any GL fonts\n");
#endif /* _GL */
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
#ifdef _WINGUI
#ifndef _GL
        TtaFreeMemory (TtFonts[i]->FiWidths);
      TtaFreeMemory (TtFonts[i]->FiHeights);
      TtaFreeMemory (TtFonts[i]);
#else /*_GL */
      gl_font_delete (TtFonts[i]);
#endif /*_GL*/	
#endif  /* _WINGUI */
#ifdef _GTK
#ifndef _GL 
      if (TtFonts[i] != DefaultFont)
        gdk_font_unref (TtFonts[i]);
#else /*_GL */
      if (TtFonts[i] != (ThotFont) DefaultGLFont)	
        gl_font_delete (TtFonts[i]);
#endif /*_GL*/
#endif /* _GTK */
#ifdef _WX
      if (TtFonts[i] != (ThotFont) DefaultGLFont)
        gl_font_delete (TtFonts[i]);
#endif /* _WX */
#if defined(_GTK) || defined(_WX)
      /* unmask patched fonts */
      if (TtPatchedFont[i])
        TtPatchedFont[i] = 0;
#endif /* #if defined(_GTK) || defined(_WX) */
      
      TtFontMask[i] = 0;
      /* pack the font table */
      FirstFreeFont--;
      j = FirstFreeFont;
      if (j > i)
        {
          /* move this entry to the freed position */
          TtFonts[i] = TtFonts[j];
          TtFontMask[i] = TtFontMask[j];

#if (defined(_GTK) || defined(_WX)) && !defined(_GL)
          TtPatchedFont[i] = TtPatchedFont[j];
#endif /* #if (defined(_GTK) || defined(_WX)) && !defined(_GL) */
          strncpy (&TtFontName[i * MAX_FONTNAME],
                   &TtFontName[j * MAX_FONTNAME], MAX_FONTNAME);
          TtFonts[j] = NULL;
          TtFontMask[j] = 0;
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
  SpecFont            prevset, fontset, nextset;
  int                 i;
  unsigned int        mask, fontmask;
  ThotBool            doIt;

  if (frame > 0)
    {
      /* compute the frame mask */
      mask = 1 << (frame - 1);
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
              fontmask = fontset->FontMask;
              fontset->FontMask = fontmask & (~mask);
              prevset = fontset;
            }
          fontset = nextset;
        }

      /* keep default fonts */
      i = FirstFreeFont - 1;
      /* free all attached fonts */
      doIt = TRUE;
      while (i >= 0)
        {
#ifdef _GL
          if (TtFontMask[i] == mask || TtFontMask[i] == 0)
#else /* _GL */
          if (TtFontMask[i] == mask)
#endif /* _GL */
            {
              /* free the entry */
              //@@@@@@@@@@@@@@@@@@@@
#ifdef _GL
              fontset = FirstFontSel;
              while (fontset)
                {
                  if (fontset->Font_1 == TtFonts[i])
                    {
                      if (TtFontMask[i] == mask)
                          fontset->Font_1 = NULL;
                      else
                        doIt = FALSE;
                    }
                  else if (fontset->SFont_1 == TtFonts[i])
                    fontset->SFont_1 = NULL;
                  else if (fontset->SFont_2 == TtFonts[i])
                    fontset->SFont_2 = NULL;
                  else if (fontset->SFont_3 == TtFonts[i])
                    fontset->SFont_3 = NULL;
                  else if (fontset->SFont_4 == TtFonts[i])
                    fontset->SFont_4 = NULL;
                  else if (fontset->SFont_5 == TtFonts[i])
                    fontset->SFont_5 = NULL;
                  else if (fontset->SFont_6 == TtFonts[i])
                    fontset->SFont_6 = NULL;
                  else if (fontset->SFont_7 == TtFonts[i])
                    fontset->SFont_7 = NULL;
                  else if (fontset->SFont_8 == TtFonts[i])
                    fontset->SFont_8 = NULL;
                  else if (fontset->SFont_9 == TtFonts[i])
                    fontset->SFont_9 = NULL;
                  else if (fontset->SFont_10 == TtFonts[i])
                    fontset->SFont_10 = NULL;
                  else if (fontset->SFont_11 == TtFonts[i])
                    fontset->SFont_11 = NULL;
                  else if (fontset->SFont_12 == TtFonts[i])
                    fontset->SFont_12 = NULL;
                  else if (fontset->SFont_13 == TtFonts[i])
                    fontset->SFont_13 = NULL;
                  else if (fontset->SFont_14 == TtFonts[i])
                    fontset->SFont_14 = NULL;
                  else if (fontset->SFont_15 == TtFonts[i])
                    fontset->SFont_15 = NULL;
                  else if (fontset->SFont_16 == TtFonts[i])
                    fontset->SFont_16 = NULL;
                  else if (fontset->SFont_17 == TtFonts[i])
                    fontset->SFont_17 = NULL;
                  else if (fontset->Font_19 == TtFonts[i])
                    fontset->Font_19 = NULL;
                  fontset = fontset->NextFontSet;
                }
#endif /* _GL */
              if (doIt)
                {
                  //printf ("ThotFreeFont frame=%d font=%d\n", frame, i);
                  FreeAFont (i);
                }
            }
          else
            /* unlink this frame */
            TtFontMask[i] = TtFontMask[i] & (~mask);
          i--;
        }
    }
}

/*----------------------------------------------------------------------
  ThotFreeAllFonts
  ----------------------------------------------------------------------*/
void ThotFreeAllFonts (void)
{
  SpecFont            fontset, nextset;
  int                 i;

  /* free all attached fontsets */
  fontset = FirstFontSel;
  while (fontset)
    {
      nextset = fontset->NextFontSet;
      TtaFreeMemory (fontset);
      fontset = nextset;
    }
  FirstFontSel = NULL;

  for (i = 0; i < MAX_FONT && TtFonts[i]; i++)
    FreeAFont (i);
  TtaFreeMemory (FontFamily);
  FontFamily = NULL;

#ifdef _GTK
  if (DefaultFont)
    {
      gdk_font_unref (DefaultFont);
      DefaultFont = NULL;
    }
  system ("xset fp");
#endif /* _GTK */
#ifdef _GL
  if (DefaultGLFont)
    {
      gl_font_delete (DefaultGLFont);
      DefaultGLFont = NULL;
    }
  FTLibraryFree ();
#endif /*_GL*/
  /*Free the font config structure
    build upon the config file*/
  FreeFontConfig ();
}


/*---------------------------------------------------------------------------
  LoadingArabicFont is a special function which load only arabic font
  "arabweb.ttf"
  ----------------------------------------------------------------------------*/
void LoadingArabicFont (SpecFont fontset ,ThotFont *font)
{
  ThotFont     lfont;
  int          encoding;
  int          frame;
  unsigned int mask;

  *font = NULL;

  encoding = UNICODE_1_1;
  lfont = fontset->Font_17;
  for (frame = 1 ; frame <= MAX_FRAME ; frame++)
    {
      mask = 1 << (frame - 1);
      if (fontset->FontMask & mask)
        lfont = LoadNearestFont ('6', fontset->FontFamily,
                                 fontset->FontHighlight,
                                 fontset->FontSize,
                                 fontset->FontSize, frame, TRUE, TRUE);

    }
  /* even if the font is not found avoid to retry later */
  fontset->Font_17 = lfont;
  *font = lfont;
}



#define MAX_TABLE 250

int Width[MAX_TABLE][2];

/*---------------------------------------------------------------------------
  Char_Width returns the width of the index giving x.
  The values are stored in a table
  ----------------------------------------------------------------------------*/
int Char_Width (int x )
{
	int i;
	for (i = 0 ; i < MAX_TABLE ; i++)
		if (Width[i][0] == x)
			return Width[i][1];
	return -1;
}

/*---------------------------------------------------------------------------
  put the char index and his width in a table 'Width'
  ----------------------------------------------------------------------------*/
void Put_Char_Width (int car, int l )
{
	int i;
	for (i=0 ; i < MAX_TABLE ; i++)
    {
      if (Width[i][0] == 0)
        break;
    }
	if ( i < MAX_TABLE ) 
    {
      Width[i][0] = car;
      Width[i][1] = l;
    }
}


/*---------------------------------------------------------------------------
  BoxArabicCharacterWidth returns the width of an arabic char in a given font
  ----------------------------------------------------------------------------*/
int BoxArabicCharacterWidth (CHAR_T c, PtrTextBuffer *adbuff, int *ind,
                             SpecFont specfont)
{
  ThotFont     font; 
  int          car, l;
  CHAR_T       prevChar, nextChar; 

  if (c != (*adbuff)->BuContent[*ind]) 
    return 6;
  if ( (*ind) > 0)
    prevChar = (*adbuff)->BuContent[(*ind) - 1];
  else
    prevChar = 0x0020;
  if ( (*ind) < (*adbuff)->BuLength - 1)
    nextChar = (*adbuff)->BuContent[(*ind) + 1];
  else
    nextChar = 0x0020;
  if (( nextChar >= 0x064B )&&( nextChar <= 0x0655 ))
    {
      if ((*ind) < (*adbuff)->BuLength - 2)
        nextChar = (*adbuff)->BuContent[(*ind) + 2];
      else
        nextChar = 0x0020;
    }
  if ((prevChar >= 0x064B )&&( prevChar <= 0x0655 ))
    {
      if ((*ind) > 1 )
        prevChar = (*adbuff)->BuContent[(*ind) - 2];
      else
        prevChar = 0x0020;
    }

  car = GetArabFontAndIndex(c, prevChar, nextChar, specfont, &font);
  if ( Char_Width(car) != -1 ) 
    return Char_Width(car);
  if (font == NULL)
    return 6;
  else
    {
#ifdef _GTK
      if (font == specfont->Font_1)
        /* the arabic font was not found:
           avoid to select an invalid position in this font */
        l = 6;
      else
#endif /* _GTK */
        l = CharacterWidth (car, font);
      Put_Char_Width (car , l);
      return l;
    }
}
