/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 
/*
 * Module dedicated to font handling.
 *
 * Authors: I. Vatton (INRIA)
 *          R. Guetari (W3C/INRIA) - Unicode and Windows version
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
static ThotBool     UseBitStreamFamily;

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

#include "buildlines_f.h"
#include "registry_f.h"
#include "language_f.h"
#include "font_f.h"
#include "memory_f.h"
#include "registry_f.h"
#include "units_f.h"
#include "ustring_f.h"
#include "windowdisplay_f.h"

#ifdef _WINDOWS
#include "wininclude.h"

static char  WIN_lpszFace [MAX_LENGTH];
static int   WIN_nHeight;
static int   WIN_nWidth;
static int   WIN_fnWeight;
static int   WIN_fdwItalic;
static int   WIN_fdwUnderline;
static int   WIN_fdwStrikeOut;

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
   CHAR_T lpszFace [MAX_LENGTH];

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
               /* sprintf (&WIN_lpszFace[0], "PMT Hoso Gothic"); */
               break;

          case 'H':
          case 'h':
               sprintf (&WIN_lpszFace[0], "Arial");
               /* sprintf (&WIN_lpszFace[0], "PMT Hoso Gothic"); */
               break;

          case 'C':
          case 'c':
               sprintf (&WIN_lpszFace[0], "Courier New");
               /* sprintf (&WIN_lpszFace[0], "PMT Hoso Mincho"); */
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

   iso2wc_strcpy (lpszFace, WIN_lpszFace);
   
   hFont = CreateFont (WIN_nHeight, WIN_nWidth, 0, 0, WIN_fnWeight,
                       WIN_fdwItalic, WIN_fdwUnderline, WIN_fdwStrikeOut,
                       DEFAULT_CHARSET, OUT_TT_ONLY_PRECIS, CLIP_DEFAULT_PRECIS,
                       PROOF_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
                       lpszFace);

   if (hFont == NULL) {
      WinErrorBox (WIN_Main_Wd, TEXT("WIN_LoadFont"));
   } 

   return (hFont);
}

/*----------------------------------------------------------------------
 *      WinLoadFont : Load a Windows font in a Device context.
 *----------------------------------------------------------------------*/
#ifdef __STDC__
HFONT    WinLoadFont (HDC hdc, ptrfont font)
#else  /* !__STDC__ */
HFONT    WinLoadFont (hdc, font)
HDC     hdc; 
ptrfont font;
#endif /* __STDC__ */
{
   if (LastUsedFont == (ptrFC)0) {
      LastUsedFont            = (ptrFC) TtaGetMemory (sizeof (FontCharacteristics));
      LastUsedFont->highlight = font->highlight; 
      LastUsedFont->size      = font->size;
      LastUsedFont->alphabet  = font->alphabet; 
      LastUsedFont->family    = font->family; 

      if (currentActiveFont != (HFONT)0) {
         if (!DeleteObject (SelectObject (hdc, GetStockObject (SYSTEM_FONT))))
            WinErrorBox (NULL, TEXT("WinLoadFont (1)"));
         currentActiveFont = (HFONT)0;
	  }
   } else if ((LastUsedFont->highlight != font->highlight) ||
              (LastUsedFont->size      != font->size)      ||
              (LastUsedFont->alphabet  != font->alphabet)  ||
              (LastUsedFont->family    != font->family)) {
          if (currentActiveFont != (HFONT)0) {
             SelectObject (hdc, GetStockObject (SYSTEM_FONT));
             if (!DeleteObject (currentActiveFont))
                WinErrorBox (NULL, TEXT("WinLoadFont (2)"));
             currentActiveFont = (HFONT)0;

             LastUsedFont->highlight = font->highlight; 
             LastUsedFont->size      = font->size;
             LastUsedFont->alphabet  = font->alphabet; 
             LastUsedFont->family    = font->family; 
		  } 
   }

   if (currentActiveFont == (HFONT)0) {
      currentActiveFont = WIN_LoadFont (font->alphabet, font->family, font->highlight, font->size, 0);
      if (currentActiveFont == (HFONT)0)
         WinErrorBox (NULL, TEXT("WinLoadFont (3)"));
      return (OldFont = SelectObject (hdc, currentActiveFont));
   }
   return (OldFont = SelectObject (hdc, currentActiveFont));
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
int                 CharacterWidth (UCHAR_T c, ptrfont font)
#else  /* __STDC__ */
int                 CharacterWidth (c, font)
     UCHAR_T        c;
ptrfont             font;
#endif /* __STDC__ */
{
#ifdef _I18N_
#ifdef _WINDOWS
      SIZE wsize;
      HFONT currentFont, HOldFont; 
      WIN_GetDeviceContext (-1);
      
      currentFont = WinLoadFont (TtDisplay, font);
      GetTextExtentPoint (TtDisplay, (LPCTSTR) (&c), 1, (LPSIZE) (&wsize));
      SelectObject (TtDisplay, currentFont);
#     if 0
      if (currentActiveFont) {
         currentFont = SelectObject (TtDisplay, currentActiveFont);
         GetTextExtentPoint (TtDisplay, (LPCTSTR) (&c), 1, (LPSIZE) (&wsize));
         SelectObject (TtDisplay, currentFont);
	  } else {
             currentFont = WinLoadFont (TtDisplay, font);
             GetTextExtentPoint (TtDisplay, (LPCTSTR) (&c), 1, (LPSIZE) (&wsize));
             SelectObject (TtDisplay, currentFont);
	  }
      WIN_ReleaseDeviceContext ();
#     endif /* 00000000 */
      return wsize.cx;
#else  /* !_WINDOWS */
#endif /* _WINDOWS */
#else /* !_I18N_ */

   int                 l;

   if (font == NULL)
      return (0);
   else
     {
        if (c == NEW_LINE)
	  /* characters NEW_LINE and BREAK_LINE are equivalent */
           c = BREAK_LINE;
	else if (c == TAB)
	  /* we use the SPACE width for the character TAB */
	  c = SPACE;
#ifdef _WINDOWS
        l = font->FiWidths[c];
#else  /* _WINDOWS */
#ifdef _GTK
	l = gdk_char_width (font, c);
#else /* _GTK */
        if (((XFontStruct*) font)->per_char == NULL)
	  l = ((XFontStruct*) font)->max_bounds.width;
        else if (c < ((XFontStruct*) font)->min_char_or_byte2)
	  l = 0;
        else 
	  l = ((XFontStruct *) font)->per_char[c - ((XFontStruct *) font)->min_char_or_byte2].width;
#endif /* _GTK */
#endif /* !_WINDOWS */
     }
   return (l);
#endif /* !_I18N_ */
}

/*----------------------------------------------------------------------
 *      CharacterHeight returns the height of a char in a given font.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                 CharacterHeight (UCHAR_T c, ptrfont font)
#else  /* __STDC__ */
int                 CharacterHeight (c, font)
UCHAR_T       c;
ptrfont             font;
#endif /* __STDC__ */
{
#ifdef _GTK
    int l;
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
#endif /* !_WINDOWS */
}

/*----------------------------------------------------------------------
       CharacterAscent returns the ascent of a char in a given font.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                 CharacterAscent (UCHAR_T c, ptrfont font)
#else  /* __STDC__ */
int                 CharacterAscent (c, font)
UCHAR_T       c;
ptrfont             font;
#endif /* __STDC__ */
{

#ifdef _GTK
   int lbearing, rbearing, width, ascent, descent;
#endif /* _GTK */

  if (font == NULL)
    return (0);
#ifdef _WINDOWS
  else
    return font->FiAscent;
#else  /* _WINDOWS */
#ifdef _GTK
  else {
    gdk_string_extents (font, &c, &lbearing, &rbearing, &width, &ascent, &descent);
    return (ascent);
  }
#else /* _GTK */
  else if (((XFontStruct *) font)->per_char == NULL)
    return ((XFontStruct *) font)->max_bounds.ascent;
  else
    return ((XFontStruct *) font)->per_char[c - ((XFontStruct *) font)->min_char_or_byte2].ascent;
#endif /* _GTK */
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
#ifdef _GTK
    return (font->ascent);
#else /* _GTK */
    return (((XFontStruct *) font)->ascent);
#endif /* _GTK */
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

#ifdef _GTK
   int lbearing, rbearing, width, ascent, descent;
  char c[]="Xp";
  int l;   
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
 *  PixelValue computes the pixel size for a given logical unit.
 *		pAb is the current abstract box except for UnPercent unit
 *		where it holds the comparison value.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                 PixelValue (int val, TypeUnit unit, PtrAbstractBox pAb, int zoom)
#else  /* __STDC__ */
int                 PixelValue (val, unit, pAb, zoom)
int                 val;
TypeUnit            unit;
PtrAbstractBox      pAb;
int                 zoom;
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
	 dist = (val * FontHeight (pAb->AbBox->BxFont) + 5) / 10;
       break;
     case UnXHeight:
       if (pAb == NULL || pAb->AbBox == NULL || pAb->AbBox->BxFont == NULL)
	 dist = 0;
       else
	 dist = (val * CharacterAscent ('x', pAb->AbBox->BxFont)) / 10;
       break;
     case UnPoint:
       dist = PointToPixel (val);
       /* take zoom into account */
       if (zoom != 0)
	 dist += (dist * zoom / 10);
       break;
     case UnPixel:
#ifdef _WINDOWS
       if (TtPrinterDC)
	 {
	   if (PrinterDPI == 0)
             PrinterDPI = GetDeviceCaps (GetDC (NULL), LOGPIXELSY);
	   if (ScreenDPI == 0)
             ScreenDPI = GetDeviceCaps (TtPrinterDC, LOGPIXELSY);
	   dist = (val * PrinterDPI + ScreenDPI / 2) / ScreenDPI;
	 }
       else
#endif /* _WINDOWS */
	 dist = val;
       /* take zoom into account */
       if (zoom != 0)
	 dist += (dist * zoom / 10);
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
 *  LogicalValue computes the logical value for a given pixel size.
 *		pAb is the current abstract box except for UnPercent unit
 *		where it holds the comparison value.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                 LogicalValue (int val, TypeUnit unit, PtrAbstractBox pAb, int zoom)
#else  /* __STDC__ */
int                 LogicalValue (val, unit, pAb, zoom)
int                 val;
TypeUnit            unit;
PtrAbstractBox      pAb;
int                 zoom;
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
	 dist = val * 10 / CharacterAscent ('x', pAb->AbBox->BxFont);
       break;
     case UnPoint:
       /* take zoom into account */
       if (zoom != 0)
	 val -= (val * zoom / 10);
       dist = PixelToPoint (val);
       break;
     case UnPixel:
       /* take zoom into account */
       if (zoom != 0)
	 val -= (val * zoom / 10);
#ifdef _WINDOWS 
       if (TtPrinterDC)
	 dist = (val * ScreenDPI + ScreenDPI / 2) / PrinterDPI;
       else 
#endif /* _WINDOWS */
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
     case UnAuto:
       /* Should not occur. Auto is reserved for margins */
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
#ifdef _GTK
  GdkFont *result;

  result = gdk_font_load ((gchar *)name);
  return (result);
#else /* _GTK */
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
#endif /* _GTK */
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
	       strcat (r_nameX, "medium");
	       strcat (r_nameX, "-");
	       strcat (r_nameX, "r");
	       break;
	    case 'i':
	    case 'o':
	       strcat (r_nameX, "medium");
	       strcat (r_nameX, "-");
	       if (TOLOWER (family) == 'h' || TOLOWER (family) == 'c')
              strcat (r_nameX, "o");
	       else
               strcat (r_nameX, "i");
	       break;
	    case 'b':
	       if (UseLucidaFamily && TOLOWER (family) == 't')
              strcat (r_nameX, "demibold");
	       else
                strcat (r_nameX, "bold");
	       strcat (r_nameX, "-");
	       strcat (r_nameX, "r");
	       break;
	    case 'g':
	    case 'q':
	       if (UseLucidaFamily && TOLOWER (family) == 't')
              strcat (r_nameX, "demibold");
	       else
              strcat (r_nameX, "bold");
              strcat (r_nameX, "-");
	       if (TOLOWER (family) == 'h' || TOLOWER (family) == 'c')
              strcat (r_nameX, "o");
	       else
               strcat (r_nameX, "i");
	       break;
	 }

   strcat (r_nameX, "-");
   if (TOLOWER (family) == 'h')
      strcat (r_nameX, "normal");  /* narrow helvetica does not exist */
   else
      strcat (r_nameX, "*");
   if (TOLOWER (family) == 's')
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
	strcat (r_nameX, "-");
	if (TOLOWER (family) == 'c')
	    strcat (r_nameX, "m");
	else
	    strcat (r_nameX, "p");
	strcat (r_nameX, "-");
	strcat (r_nameX, "*");
	strcat (r_nameX, "-");
	if (TOLOWER (alphabet) == 'l')
	{
	    strcat (r_nameX, "iso8859");
	    strcat (r_nameX, "-");
	    strcat (r_nameX, "1");
	}
	else if (TOLOWER (alphabet) == 'e' || alphabet == '2')
	{
	    strcat (r_nameX, "iso8859");
	    strcat (r_nameX, "-");
	    strcat (r_nameX, "2");
	}	
	else if (TOLOWER (alphabet) == 'g')
	{
	    strcat (r_nameX, "*");		/*adobe */
	    strcat (r_nameX, "-");		/*adobe */
	    strcat (r_nameX, "fontspecific");	/*adobe */
	}
	else
	{
	    strcat (r_nameX, "iso8859");
	    strcat (r_nameX, "-");
	    strcat (r_nameX, "1");
		/* replace '1' by current alphabet */
	    r_nameX[strlen (r_nameX) -1] = alphabet;
	}
     }

   sprintf (r_name, "%c%c%c%d", TOLOWER (alphabet), TOLOWER (family),
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
   char             name[10], nameX[100];

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
static ptrfont      LoadNearestFont (char alphabet, char family, int highlight, int size, TypeUnit unit, int frame, ThotBool increase)
#else  /* __STDC__ */
static ptrfont      LoadNearestFont (alphabet, family, highlight, size, unit, frame, increase)
char                alphabet;
char                family;
int                 highlight;
int                 size;
TypeUnit            unit;
int                 frame;
ThotBool            increase;
#endif /* __STDC__ */
{
  int                 i, j, deb, index;
  int                 mask;
  char                text[10], PsName[10], textX[100];
#ifdef _WINDOWS
  SIZE                wsize;
  TEXTMETRIC          textMetric;
  CHAR_T              fontSize[5];
  char*               pText;
  STRING              pFontSize;
  int                 c;
  HFONT               hOldFont;
#endif /* _WINDOWS */
  ptrfont             ptfont;

  /* use only standard sizes */
# ifdef _WINDOWS
  WIN_GetDeviceContext (frame);
# endif /* _WINDOWS */

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
   
  if (UseBitStreamFamily && size == 11 && unit == UnPoint)
    /* in the case of Bitstream, accept 11 points font size */
    FontIdentifier (alphabet, family, highlight, size, TRUE, text, textX);
  else
    FontIdentifier (alphabet, family, highlight, index, FALSE, text, textX);
   
  /* initialize the PostScript font name */
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
	      size = uatoi (fontSize);
	    }
	  /* Allocate the font structure */
	  ptfont = TtaGetMemory (sizeof (FontInfo));
      ptfont->alphabet  = alphabet;
      ptfont->family    = family;
      ptfont->highlight = highlight;
      ptfont->size      = size;
	  currentActiveFont = WIN_LoadFont (alphabet, family, highlight, size, unit);
      if (TtPrinterDC != 0) {
         hOldFont = SelectObject (TtPrinterDC, currentActiveFont);
         if (GetTextMetrics (TtPrinterDC, &textMetric)) {
            ptfont->FiAscent = textMetric.tmAscent;
            ptfont->FiHeight = textMetric.tmAscent + textMetric.tmDescent;
		 } else {
               ptfont->FiAscent = 0;
               ptfont->FiHeight = 0;
		 }
         for (c = 0; c < 256; c++) {
             GetTextExtentPoint (TtPrinterDC, (LPCTSTR) (&c), 1, (LPSIZE) (&wsize));
             ptfont->FiWidths[c] = wsize.cx;
             ptfont->FiHeights[c] = wsize.cy;
		 }
         SelectObject (TtPrinterDC, hOldFont);
      } else { 
           hOldFont = SelectObject (TtDisplay, currentActiveFont);

           if (GetTextMetrics (TtDisplay, &textMetric)) {
              ptfont->FiAscent = textMetric.tmAscent;
              ptfont->FiHeight = textMetric.tmAscent + textMetric.tmDescent;
		   } else {
                ptfont->FiAscent = 0;
                ptfont->FiHeight = 0;
		   }
           for (c = 0; c < 256; c++) {
               GetTextExtentPoint (TtDisplay, (LPCTSTR) (&c), 1, (LPSIZE) (&wsize));
               ptfont->FiWidths[c] = wsize.cx;
               ptfont->FiHeights[c] = wsize.cy;
		   }
           /* SelectObject (TtDisplay, hOldFont); */
           if (!DeleteObject (SelectObject (TtDisplay, currentActiveFont)))
              WinErrorBox (NULL, TEXT("LoadNearestFont (1)"));
           currentActiveFont = 0;
	  }
#      else  /* _WINDOWS */
	  if (alphabet == 'G' && size > 8 && (size < 16 || size == 24))
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
   return LoadNearestFont (alphabet, family, highlight, size, unit, frame, TRUE);
}

/*----------------------------------------------------------------------
  TtaSetFontZoom
  Updates the font zoom global varialbe
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void TtaSetFontZoom (int zoom)
#else
void TtaSetFontZoom (zoom)
int zoom;
#endif /* __STDC__ */
{
  FontZoom = zoom;
}

/*----------------------------------------------------------------------
  InitDialogueFonts initialize the standard fonts used by the Thot Toolkit.
  ----------------------------------------------------------------------*/
#ifdef _WINDOWS
#ifdef __STDC__
void                WIN_InitDialogueFonts (HDC hDC, CHAR_T* name)
#else  /* __STDC__ */
void                WIN_InitDialogueFonts (hDC, name)
HDC                 hDC;
CHAR_T*             name;
#endif /* __STDC__ */
#else  /* !_WINDOWS */
#ifdef __STDC__
void                InitDialogueFonts (CHAR_T* name)
#else  /* __STDC__ */
void                InitDialogueFonts (name)
CHAR_T*             name;
#endif /* __STDC__ */
#endif /* _WINDOWS */
{
#  ifndef _WINDOWS
   int              ndir, ncurrent;
   char             FONT_PATH[128];
   STRING           fontpath;
#  endif /* _WINDOWS */
   char**           dirlist = NULL;
   char**           currentlist = NULL;
   CHAR_T*          value;
   char             alphabet;
   int              f3;
   int              i;

   /* is there a predefined font family ? */
   MenuSize = 12;
   alphabet = TtaGetAlphabet (TtaGetDefaultLanguage ());
   /* initialize the font zoom */
   TtaGetEnvInt ("ZOOM",&FontZoom);
   value = TtaGetEnvString ("FontFamily");
   MaxNumberOfSizes = 10;
   if (value == NULL)
     {
	FontFamily = TtaGetMemory (8);
	strcpy (FontFamily, "-");
	strcat (FontFamily, "*");
     }
   else
     {
	FontFamily = TtaGetMemory (ustrlen (value) + 1);
	wc2iso_strcpy (FontFamily, value);
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
#  ifdef _WINDOWS
   DOT_PER_INCHE = GetDeviceCaps(hDC, LOGPIXELSY);
#  else  /* !_WINDOWS */
   DOT_PER_INCHE = 72;
#  endif /* _WINDOWS */


   /* Is there any predefined size for menu fonts ? */
   value = TtaGetEnvString ("FontMenuSize");
   if (value != NULL)
      usscanf (value, TEXT("%d"), &MenuSize);
   f3 = MenuSize + 2;

#  ifndef _WINDOWS
   fontpath = TtaGetEnvString ("THOTFONT");
   if (fontpath)
     {
	wc2iso_strcpy (FONT_PATH, fontpath);
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
		  if (j == MAX_FONT)
#                   ifdef _WINDOWS
            /* if (TtDisplay)
               SelectObject (TtDisplay, (HFONT)0);
		    if (!DeleteObject (TtFonts[i]->FiFont))
		      WinErrorBox (WIN_Main_Wd);
            TtFonts[i]->FiFont = (HFONT)0; */
            DeleteObject (SelectObject (TtDisplay, currentActiveFont));
		    TtaFreeMemory (TtFonts[i]);
#                   else  /* _WINDOWS */
#ifdef _GTK
 		    gdk_font_unref (TtFonts[i]);
#else /* _GTK */
		    XFreeFont (TtDisplay, (XFontStruct *) TtFonts[i]);
#endif /* _GTK */
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

/*----------------------------------------------------------------------
 *      ThotFreeAllFonts
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ThotFreeAllFonts (void)
#else  /* __STDC__ */
void                ThotFreeAllFonts (void)

#endif /* __STDC__ */
{
   TtaFreeMemory (FontFamily);
   FontFamily = NULL;
   TtaFreeMemory (FontDialogue);
   FontDialogue = NULL;
   TtaFreeMemory (IFontDialogue); 
   IFontDialogue =  NULL;
   TtaFreeMemory (LargeFontDialogue);
   LargeFontDialogue = NULL;
}
