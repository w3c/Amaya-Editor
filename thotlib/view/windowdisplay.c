/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2001
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 
/*
 * windowdisplay.c : handling of low level drawing routines, both for
 *                   MS-Windows (incomplete).
 *
 * Authors: I. Vatton (INRIA)
 *          R. Guetari (W3C/INRIA) Unicode
 *
 */

#include "ustring.h"
#include "math.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "frame.h"
#include "wininclude.h"

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "font_tv.h"
#include "frame_tv.h"
#include "units_tv.h"
#include "boxes_tv.h"
#include "edit_tv.h"
#include "thotcolor_tv.h"

extern ThotColorStruct cblack;
extern int             ColorPs;

#define	MAX_STACK      50
#define	MIDDLE_OF(v1, v2) (((v1)+(v2))/2.0)
#define SEG_SPLINE     5
#define ALLOC_POINTS   300

typedef struct stack_point
  {
     float               x1, y1, x2, y2, x3, y3, x4, y4;
  }
StackPoint;
static StackPoint   stack[MAX_STACK];
static int          stack_deep;
static DWORD        fontLangInfo = -1;
static int          SameBox = 0; /* 1 if the text is in the same box */
static int          NbWhiteSp;

extern BOOL autoScroll;
extern int          LastPageNumber, LastPageWidth, LastPageHeight;

int                 X, Y;

#include "buildlines_f.h"
#include "context_f.h"
#include "font_f.h"
#include "memory_f.h"
#include "inites_f.h"
#include "units_f.h"
#include "xwindowdisplay_f.h"

#ifndef _WIN_PRINT
/*----------------------------------------------------------------------
   SetMainWindowBackgroundColor :                          
  ----------------------------------------------------------------------*/
void         SetMainWindowBackgroundColor (int frame, int color)
{
  COLORREF    cr;

  WIN_GetDeviceContext (frame);
  cr = ColorPixel (color);
  SetBkColor (TtDisplay, cr); 
  WIN_ReleaseDeviceContext ();
}


/*----------------------------------------------------------------------
  SpaceToChar substitute in text the space chars to their visual
  equivalents.
  ----------------------------------------------------------------------*/
static void         SpaceToChar (USTRING text)
{
  int                 i;

  if (text == NULL)
    return;
  i = 0;
  while (text[i] != 0)
    {
      switch (text[i])
	{
	case BREAK_LINE:
	  text[i] = (UCHAR_T) SHOWN_BREAK_LINE;
	  break;
	case THIN_SPACE:
	  text[i] = (UCHAR_T) SHOWN_THIN_SPACE;
	  break;
	case HALF_EM:
	  text[i] = (UCHAR_T) SHOWN_HALF_EM;
	  break;
	case UNBREAKABLE_SPACE:
	  text[i] = (UCHAR_T) SHOWN_UNBREAKABLE_SPACE;
	  break;
	case SPACE:
	  text[i] = (UCHAR_T) SHOWN_SPACE;
	  break;
	}
      i++;
    }
}
#endif /* _WIN_PRINT */

/*----------------------------------------------------------------------
  DrawArrowHead draw the end of an arrow.
  ----------------------------------------------------------------------*/
static void   DrawArrowHead (int frame, int x1, int y1, int x2, int y2, int thick, int fg)
{
  float               x, y, xb, yb, dx, dy, l, sina, cosa;
  float               width, height;
  HPEN                hPen;
  HPEN                hOldPen;
  int                 xc, yc, xd, yd;
  ThotPoint           point[4];

  if (thick == 0 || fg < 0)
    return;

  width = (float) (5 + thick);
  height = 10;
  dx = (float) (x2 - x1);
  dy = (float) (y1 - y2);
  l = (float) sqrt ((double) (dx * dx + dy * dy));
  if (l == 0)
    return;
  sina = dy / l;
  cosa = dx / l;
  xb = x2 * cosa - y2 * sina;
  yb = x2 * sina + y2 * cosa;
  x = xb - height;
  y = yb - width / 2;
  xc = FloatToInt ((float) (x * cosa + y * sina + .5));
  yc = FloatToInt ((float) (-x * sina + y * cosa + .5));
  y = yb + width / 2;
  xd = FloatToInt ((float) (x * cosa + y * sina + .5));
  yd = FloatToInt ((float) (-x * sina + y * cosa + .5));

  /* draw */
  point[0].x = x2;
  point[0].y = y2;
  point[1].x = xc;
  point[1].y = yc;
  point[2].x = xd;
  point[2].y = yd;
  point[3].x = x2;
  point[3].y = y2;
  hPen = CreatePen (PS_SOLID, thick, ColorPixel (fg));
#ifdef _WIN_PRINT
  hOldPen = SelectObject (TtPrinterDC, hPen);
  Polyline (TtPrinterDC, point, 4);
  SelectObject (TtPrinterDC, hOldPen);
#else /* _WIN_PRINT */
  WIN_GetDeviceContext (frame);
  SelectClipRgn (TtDisplay, clipRgn);
  hOldPen = SelectObject (TtDisplay, hPen);
  Polyline (TtDisplay, point, 4);
  SelectObject (TtDisplay, hOldPen);
  WIN_ReleaseDeviceContext ();
#endif /* _WIN_PRINT */

  DeleteObject (hPen);
}

/*----------------------------------------------------------------------
  DrawOneLine draw one line starting from (x1, y1) to (x2, y2) in frame.
  ----------------------------------------------------------------------*/
static void  DrawOneLine (int frame, int thick, int style, int x1, int y1, int x2, int y2, int fg)
{
  HPEN     hPen;
  HPEN     hOldPen;

  if (thick == 0)
    hPen = CreatePen (PS_NULL, thick, ColorPixel (fg));
  else
    {
      switch (style)
	{
	case 3:
	  hPen = CreatePen (PS_DOT, thick, ColorPixel (fg));
	  break;
	case 4:
	  hPen = CreatePen (PS_DASH, thick, ColorPixel (fg)); 
	  break;
	default:
	  hPen = CreatePen (PS_SOLID, thick, ColorPixel (fg));   
	  break;
	}
    }
#ifdef _WIN_PRINT
  hOldPen = SelectObject (TtPrinterDC, hPen);
  MoveToEx (TtPrinterDC, x1, y1, NULL);
  LineTo (TtPrinterDC, x2, y2);

  SelectObject (TtPrinterDC, hOldPen);
#else /* _WIN_PRINT */
  WIN_GetDeviceContext (frame);
  hOldPen = SelectObject (TtDisplay, hPen);
  SelectClipRgn (TtDisplay, clipRgn);
  MoveToEx (TtDisplay, x1, y1, NULL);
  LineTo (TtDisplay, x2, y2);

  SelectObject (TtDisplay, hOldPen);
  WIN_ReleaseDeviceContext ();
#endif /* _WIN_PRINT */
  DeleteObject (hPen);
}


/*----------------------------------------------------------------------
  DrawChar draw a char at location (x, y) in frame and with font.
  The parameter fg indicates the drawing color
  ----------------------------------------------------------------------*/
void      DrawChar (UCHAR_T car, int frame, int x, int y, ptrfont font, int fg)
{
  CHAR_T              str[2] = {car, 0};
  HFONT               hOldFont;

  if (fg < 0)
    return;
   y += FrameTable[frame].FrTopMargin;

#ifdef _WIN_PRINT
   SetTextColor (TtPrinterDC, ColorPixel (fg));
   SetBkMode (TtPrinterDC, TRANSPARENT);
   SetMapperFlags (TtPrinterDC, 1);
   hOldFont = WinLoadFont (TtPrinterDC, font);
   TextOut (TtPrinterDC, x, y, (USTRING) str, 1);   
   SelectObject (TtPrinterDC, hOldFont);
   DeleteObject (currentActiveFont);
   currentActiveFont = (HFONT)0;
#else /* _WIN_PRINT */
   WIN_GetDeviceContext (frame);
   SetTextColor (TtDisplay, ColorPixel (fg));
   SetBkMode (TtDisplay, TRANSPARENT);
   SetMapperFlags (TtDisplay, 1);
   hOldFont = WinLoadFont (TtDisplay, font);
   SelectClipRgn (TtDisplay, clipRgn);
   TextOut (TtDisplay, x, y, (USTRING) str, 1);
   SelectObject (TtDisplay, hOldFont);
   DeleteObject (currentActiveFont);
   currentActiveFont = (HFONT)0;
   WIN_ReleaseDeviceContext ();

#endif /* _WIN_PRINT */
}

/*----------------------------------------------------------------------
  DrawString draw a char string of lg chars beginning at buff[i].
  Drawing starts at (x, y) in frame and using font.
  lgboite gives the width of the final box or zero,
  this is used only by the thot formmating engine.
  bl indicates taht there is a space before the string
  hyphen indicates whether an hyphen char has to be added.
  debutbloc is 1 if the text is at a paragraph beginning
  (no justification of first spaces).
  parameter fg indicates the drawing color
  Returns the lenght of the string drawn.
  ----------------------------------------------------------------------*/
int   DrawString (STRING buff, int i, int lg, int frame, int x, int y, ptrfont font, int lgboite, int bl, int hyphen, int debutbloc, int fg, int shadow)
{
   STRING              ptcar;
   int                 j, width;
   SIZE                size;
   HFONT               hOldFont;
#ifdef _WIN_PRINT
   int                 encoding, NonJustifiedWhiteSp;
#else  /* _WIN_PRINT */
   RECT                rect;
   UINT                outOpt; 
#ifdef _I18N_
   GCP_RESULTS         results;
   USHORT              auGlyphs [2000];
   CHAR_T              szNewText [2000];
   UINT                infoFlag;
   int                 anDX [2000];
#endif /* _I18N_ */
#endif /* _WIN_PRINT */

#ifdef _WIN_PRINT
   encoding = 0;
   if (y < 0)
      return 0;
#endif /* _WIN_PRINT */

#ifdef _WIN_PRINT
   y += FrameTable[frame].FrTopMargin;
   /* NonJustifiedWhiteSp is > 0 if writing a fixed lenght is needed */
   /* and equal to 0 if a justified space is to be printed */

   NonJustifiedWhiteSp = debutbloc;
   /* Is this a new box ? */
   if (SameBox == 0)
     {
      /* Beginning of a new box */
      SameBox = 1;
      X = x;
      Y = y + FontBase (font);
      NbWhiteSp = 0;

      if (fg >= 0 )
	  {
       /* Do we need to change the current color ? */
	   SetTextColor (TtPrinterDC, ColorPixel (fg));
	   SetBkMode (TtPrinterDC, TRANSPARENT);
	   /* Do we need to change the current font ? */
	   hOldFont = WinLoadFont (TtPrinterDC, font);
	  }
     }

   if (shadow)
     {
       /* replace each character by a star */
       j = 0;
       ptcar = TtaAllocString ((size_t) (lg + 1));
       while (j < lg)
	     ptcar[j++] = '*';
       ptcar[lg] = EOS;
       bl = 0;
     }
   else
     ptcar = &buff[i - 1];

     /* Add the justified white space */
     if (ptcar[0] == '\212' || ptcar[0] == '\12')
	 {
         /* skip the Control return char */
         ptcar++;
         lg--;
	 }
     if (fg >= 0)
	 {
          SetMapperFlags (TtPrinterDC, 1);
	  GetTextExtentPoint (TtPrinterDC, ptcar, lg, &size);
	  width = size.cx;
	  if (lg > 0)
	    if (!TextOut (TtPrinterDC, x, y, (USTRING) ptcar, lg))
	      WinErrorBox (NULL, TEXT("DrawString (1)"));
	  
	  if (hyphen) /* draw the hyphen */
	    if (!TextOut (TtPrinterDC, x + width, y, TEXT("\255"), 1))
	      WinErrorBox (NULL, TEXT("DrawString (2)"));
	 }
     if (lgboite != 0)
       SameBox = 0;
   if (lg > 0)
     {
       /* compute the width of the string */
       width = 0;
       j = 0;
       while (j < lg)
	 width += CharacterWidth (ptcar[j++], font);
       return (width);
     } 
   
   if (shadow)
     TtaFreeMemory (ptcar);
   return (0);
   
#else  /* _WIN_PRINT */
   if (lg > 0 && FrRef[frame] != None)
     {
      /* Dealing with BR tag for windows */
      WIN_GetDeviceContext (frame);
      SetMapperFlags (TtDisplay, 1);
      hOldFont = WinLoadFont (TtDisplay, font);
      ptcar = &buff[i - 1];
      GetTextExtentPoint (TtDisplay, ptcar, lg, &size);
      width = size.cx;

      if (fg < 0)
	/* color is transparent. Don't do anything */
        return (width);
      SelectClipRgn (TtDisplay, clipRgn);
      SetTextColor (TtDisplay, ColorPixel (fg));
      SetBkMode (TtDisplay, TRANSPARENT);
      if (!ShowSpace || shadow)
	{
	 /* draw the spaces */
         ptcar = TtaAllocString (lg + 1);
         if (shadow)
	   {
	    /* replace each character by a star */
	    j = 0;
	    while (j < lg)
	      ptcar[j++] = TEXT('*');
	    ptcar[lg] = EOS;
	   }
	 else
	   {
	    ustrncpy (ptcar, &buff[i - 1], lg);
	    ptcar[lg] = EOS;
	    SpaceToChar (ptcar);	/* substitute spaces */
	   } 
         GetClientRect (FrRef [frame], &rect);
         outOpt = 0;

#if 0 /* ifdef _I18N_ */
         fontLangInfo = GetFontLanguageInfo (TtDisplay);

         if (fontLangInfo == GCP_ERROR) /* There is a Problem. */
            WinErrorBox (NULL, TEXT("DrawString (1)"));

         if (fontLangInfo & GCP_DIACRITIC)
            infoFlag |= GCP_DIACRITIC;
         
         if (fontLangInfo & GCP_GLYPHSHAPE)
	   {
            /* The font/language contains multiple glyphs per code point or
	       per code point combination (supports shaping and/or ligation),
	       and the font contains advanced glyph tables to provide extra
	       glyphs for the extra shapes. If this value is given,
	       the lpGlyphs array must be used with the GetCharacterPlacement
	       function and the ETO_GLYPHINDEX value must be passed to the
	       ExtTextOut function when the string is drawn. */
	     infoFlag |= GCP_GLYPHSHAPE;
	   } 
         
         if (fontLangInfo & GCP_USEKERNING)
            /* The font contains a kerning table which can be used to
	       provide better spacing between the characters and glyphs. */
            infoFlag |= GCP_USEKERNING;
         
         if (fontLangInfo & GCP_REORDER)
            /* The language requires reordering for display--for example,
	       Hebrew or Arabic. */
            infoFlag |= GCP_CLASSIN;

         infoFlag |= GCP_DISPLAYZWG;

         results.lStructSize = sizeof (results);
         results.lpOutString = &szNewText[0];
         results.lpOrder     = NULL;
         results.lpDx        = &anDX[0];
         results.lpCaretPos  = NULL;
         results.lpClass     = NULL;
         results.lpGlyphs    = &auGlyphs[0];
         results.nGlyphs     = 2000;
         results.nMaxFit     = 0;

         GetCharacterPlacement (TtDisplay, ptcar, ustrlen (ptcar),
				GCP_MAXEXTENT, &results, infoFlag);

         ExtTextOut (TtDisplay, x, y + FrameTable[frame].FrTopMargin, outOpt,
		     &rect, (USTRING) szNewText, lg, anDX); 
#endif /* else  /* !_I18N_ */
         /* ExtTextOut (TtDisplay, x, y + FrameTable[frame].FrTopMargin, 0,
	                &rect, (USTRING) ptcar, lg, NULL);  */
         TextOut (TtDisplay, x, y + FrameTable[frame].FrTopMargin,
		  (USTRING) ptcar, lg);
/* #        endif /* !_I18N_ */
         TtaFreeMemory (ptcar);
	}
      else
	{
	 if (ptcar[0] == TEXT('\212') || ptcar[0] == TEXT('\12'))
	   {
	    /* skip the Control return char */
	    ptcar++;
	    lg--;
	   }
	 if (lg != 0)
	   {
	    outOpt = 0;

#if 0 /* ifdef _I18N_ */
	    fontLangInfo = GetFontLanguageInfo (TtDisplay);

	    if (fontLangInfo == GCP_ERROR) /* There is a Problem. */
	      WinErrorBox (NULL, TEXT("DrawString (2)"));

	    if (fontLangInfo & GCP_DIACRITIC)
	      infoFlag |= GCP_DIACRITIC;
	    
	    if (fontLangInfo & GCP_GLYPHSHAPE)
	      {
	       /* The font/language contains multiple glyphs per code point
		  or per code point combination (supports shaping and/or
		  ligation), and the font contains advanced glyph tables
		  to provide extra glyphs for the extra shapes. If this
		  value is given, the lpGlyphs array must be used with
		  the GetCharacterPlacement function and the ETO_GLYPHINDEX
		  value must be passed to the ExtTextOut function when
		  the string is drawn. */
		infoFlag |= GCP_GLYPHSHAPE;
	      }  
         
	    if (fontLangInfo & GCP_USEKERNING)
	      /* The font contains a kerning table which can be used to
		 provide better spacing between the characters and glyphs. */
	      infoFlag |= GCP_USEKERNING;
         
	    if (fontLangInfo & GCP_REORDER)
	      /* The language requires reordering for display--for example,
		 Hebrew or Arabic. */
	      infoFlag |= GCP_CLASSIN;

	    infoFlag |= GCP_DISPLAYZWG;

	    results.lStructSize = sizeof (results);
	    results.lpOutString = &szNewText[0];
	    results.lpOrder     = NULL;
	    results.lpDx        = &anDX[0];
	    results.lpCaretPos  = NULL;
	    results.lpClass     = NULL;
	    results.lpGlyphs    = &auGlyphs[0];
	    results.nGlyphs     = 2000;
	    results.nMaxFit     = 0;
	    GetCharacterPlacement (TtDisplay, ptcar, ustrlen (ptcar),
				   GCP_MAXEXTENT, &results, infoFlag);

	    ExtTextOut (TtDisplay, x, y + FrameTable[frame].FrTopMargin,
			outOpt, &rect, (USTRING) szNewText, lg, anDX);
#endif /* 00000 else  /* !_I18N_ */ */
	    TextOut (TtDisplay, x, y + FrameTable[frame].FrTopMargin,
		     (USTRING) ptcar, lg);
/* #               endif /* !_18N_ */
	   } 
	} 
 
      if (hyphen)
	{
         /* draw the hyphen */
         TextOut (TtDisplay, x + width, y + FrameTable[frame].FrTopMargin,
		  TEXT("\255"), 1);
	} 
      SelectObject (TtDisplay, hOldFont);
      DeleteObject (currentActiveFont);
      currentActiveFont = (HFONT)0;
      /* WIN_ReleaseDeviceContext (); */
      return (width);
     }
   else
     return (0);
#endif /* _WIN_PRINT */
}

/*----------------------------------------------------------------------
  DisplayUnderline draw the underline, overline or cross line
  added to some text of lenght lg, using font and located
  at (x, y) in frame. 
  The parameter fg indicates the drawing color and type indicates the
  kind of drawing:
  - 0 = none
  - 1 = underlined
  - 2 = overlined
  - 3 = cross-over
  
                  (x,y)
          _________________________________________________\_/__ top
          /|\    I    I          /|\       /|\   /|\        
           |     I\  /I           |         |     |       
           |  ___I_\/_I_______    |ascent   |     |  
   fheight |     I    I  I  \     |         |     |
           |     I    I  I  |     |         |  __\|/ middle
           |  ___I____I__I__/____\|/        | 
           |             I                  |
	       |             I                  |
	      \|/____________I_________________\|/_ bottom
	      
  ----------------------------------------------------------------------*/
void      DisplayUnderline (int frame, int x, int y, ptrfont font, int type, int lg, int fg)
{
  int                 fheight;	/* font height           */
  int                 bottom;	/* underline position    */
  int                 middle;	/* cross-over position   */
  int                 thickness;	/* thickness of drawing */

  if (fg < 0)
    return;

#ifdef _WIN_PRINT
  if (y < 0)
    return;
#endif /* _WIN_PRINT */

  y += FrameTable[frame].FrTopMargin;
  if (lg > 0)
    {
      fheight = FontHeight (font);
      thickness = (fheight / 20) + 1;
      bottom = fheight - thickness;
      middle = fheight / 2;
      /*
       * for an underline independant of the font add
       * the following lines here :
       *         thickness = 1;
       *         height = y + 2 * thickness;
       *         bottom = y + ascent + 3;
       */
      switch (type)
	{
	case 1: /* underlined */
	  bottom += y;
	  DrawOneLine (frame, thickness, 5, x - lg, bottom, x, bottom, fg);
	  break;
	  
	case 2: /* overlined */
	  DrawOneLine (frame, thickness, 5, x - lg, y, x, y, fg);
	  break;
	  
	case 3: /* cross-over */
	  middle += y;
	  DrawOneLine (frame, thickness, 5, x - lg, middle, x, middle, fg);
	  break;
	  
	default: /* not underlined */
	  break;
	} 
    } 
}

/*----------------------------------------------------------------------
  DrawPoints draw a line of dot.
  The parameter fg indicates the drawing color
  ----------------------------------------------------------------------*/
void      DrawPoints (int frame, int x, int y, int lgboite, int fg)
{
  ptrfont             font;
  int                 xcour;
  int                 width, nb;
  STRING              ptcar;

#ifdef _WIN_PRINT
  if (y < 0)
    return;
#endif /* _WIN_PRINT */

  y += FrameTable[frame].FrTopMargin;
  font = ThotLoadFont ('L', 't', 0, 6, UnPoint, frame);
  if (lgboite > 0)
    {
      y = y - FontAscent (font) + CharacterAscent (' ', font);
      ptcar = TEXT(" .");
      /* compute lenght of the string " ." */
      width = CharacterWidth (SPACE, font) + CharacterWidth (TEXT('.'), font);
      /* compute the number of string to write */
      nb = lgboite / width;
      xcour = x + (lgboite % width);
      y = y + FrameTable[frame].FrTopMargin - FontBase (font);

      /* draw the points */
      while (nb > 0)
	  {
      DrawChar ('\362', frame, xcour, y, font, fg);
	  xcour += width;
	  nb--;
	  }
    }
}

/*----------------------------------------------------------------------
  DrawRadical Draw a radical symbol.
  The parameter fg indicates the drawing color
  ----------------------------------------------------------------------*/
void     DrawRadical (int frame, int thick, int x, int y, int l, int h, ptrfont font, int fg)
{
  int       xm, xp, fh;

  if (fg < 0 || thick <= 0)
    return;
#ifdef _WIN_PRINT
  if (y < 0)
    return;
#endif /* _WIN_PRINT */

  y += FrameTable[frame].FrTopMargin;
  fh = FontHeight (font);
  xm = x + (fh / 2);
  xp = x + (fh / 4);
  /* vertical part */
  DrawOneLine (frame, thick, 5, x, y + (2 * (h / 3)), xp - (thick / 2), y + h, fg);
  /* Acending part */
  DrawOneLine (frame, thick, 5, xp, y + h, xm, y, fg);
  /* Upper part */
  DrawOneLine (frame, thick, 5, xm, y, x + l, y, fg);
}

/*----------------------------------------------------------------------
  DrawIntegral draw an integral. depending on type :
  - simple if type = 0
  - contour if type = 1
  - double if type = 2.
  parameter fg indicates the drawing color
  ----------------------------------------------------------------------*/
void       DrawIntegral (int frame, int thick, int x, int y, int l, int h, int type, ptrfont font, int fg)
{
  int      xm, yf, yend, exnum, delta;
  int      wd, asc, hd;

  if (fg < 0 || thick <= 0)
    return;
#ifdef _WIN_PRINT
  if (y < 0)
    return;
#endif /* _WIN_PRINT */

  y += FrameTable[frame].FrTopMargin;
  exnum = 0;
  if (FontHeight (font) *1.2 >= h)
    {
      /* display a single glyph */
      xm = x + ((l - CharacterWidth ('\362', font)) / 2);
      yf = y + ((h - CharacterHeight ('\362', font)) / 2) - FontAscent (font) +
      CharacterAscent ('\362', font);
      DrawChar ('\362', frame, xm, yf, font, fg);
    }
  else
    {
      /* Need more than one glyph */
      xm = x + ((l - CharacterWidth ('\363', font)) / 2);
      yf = y - FontAscent (font) + CharacterAscent ('\363', font);
      DrawChar ('\363', frame, xm, yf, font, fg);
      yend = y + h - CharacterHeight ('\365', font) - FontAscent (font) +
      CharacterAscent ('\365', font) - 1;
      DrawChar ('\365', frame, xm, yend, font, fg);
	 
      yf += CharacterHeight ('\363', font);
      delta = yend - yf;
      asc = CharacterAscent ('\364', font)  - FontAscent (font) - 1;
      hd = CharacterHeight ('\364', font) - 1;
      wd = (CharacterWidth ('\363', font) - CharacterWidth ('\364', font)) / 2;
      if (delta >= 0)
	  {
	    for (yf += asc, yend -= hd; yf < yend; yf += CharacterHeight ('\364', font), exnum++)
	      DrawChar ('\364', frame, xm+wd, yf, font, fg);
	    if (exnum)
	      DrawChar ('\364', frame, xm+wd, yend, font, fg);
	    else
	      DrawChar ('\364', frame, xm+wd, yf + ((delta - hd) / 2), font, fg);
	  }
  }

  if (type == 2)
    /* double integral */
    DrawIntegral (frame, thick, x + (CharacterWidth ('\364', font) / 2), y, l, h, -1, font, fg);
  else if (type == 1)
    /* contour integral */
    DrawChar ('o', frame, x + ((l - CharacterWidth ('o', font)) / 2),
	      y + (h - CharacterHeight ('o', font)) / 2 - FontAscent (font) + CharacterAscent ('o', font),
	      font, fg);
}

/*----------------------------------------------------------------------
  DrawMonoSymb draw a one glyph symbol.
  parameter fg indicates the drawing color
  ----------------------------------------------------------------------*/
static void     DrawMonoSymb (CHAR_T symb, int frame, int x, int y, int l, int h, ptrfont font, int fg)
{
  int                 xm, yf;

  y += FrameTable[frame].FrTopMargin;
  xm = x + ((l - CharacterWidth (symb, font)) / 2);
  yf = y + ((h - CharacterHeight (symb, font)) / 2) - FontAscent (font) + CharacterAscent (symb, font);
  DrawChar (symb, frame, xm, yf, font, fg);
}

/*----------------------------------------------------------------------
  DrawSigma draw a Sigma symbol.
  parameter fg indicates the drawing color
  ----------------------------------------------------------------------*/
void    DrawSigma (int frame, int x, int y, int l, int h, ptrfont font, int fg)
{
  int               xm, ym, fh;

  if (fg < 0)
    return;
#ifdef _WIN_PRINT
   if (y < 0)
      return;
#endif  /* !_WIN_PRINT */
   y += FrameTable[frame].FrTopMargin;
   fh = FontHeight (font);
   if (h < fh * 2 && l <= CharacterWidth ('\345', font))
     /* Only one glyph needed */
     DrawMonoSymb ('\345', frame, x, y, l, h, font, fg);
   else
     {
       xm = x + (l / 3);
       ym = y + (h / 2) - 1;
       /* Center */
       DrawOneLine (frame, 1, 5, x, y + 1, xm, ym, fg);
       DrawOneLine (frame, 1, 5, x, y + h - 2, xm, ym, fg);
       
       /* Borders */
       DrawOneLine (frame, 1, 5, x, y, x + l, y, fg);
       DrawOneLine (frame, 1, 5, x, y + h - 2, x + l, y + h - 2, fg);
     }
}

/*----------------------------------------------------------------------
  DrawPi draw a PI symbol.
  parameter fg indicates the drawing color
  ----------------------------------------------------------------------*/
void        DrawPi (int frame, int x, int y, int l, int h, ptrfont font, int fg)
{
  int         fh;

  if (fg < 0)
    return;
#ifdef _WIN_PRINT
   if (y < 0)
      return;
#endif  /* !_WIN_PRINT */

   y += FrameTable[frame].FrTopMargin;
   fh = FontHeight (font);
   if (h < fh * 2 && l <= CharacterWidth ('\325', font))
     {
	/* Only one glyph needed */
	DrawMonoSymb ('\325', frame, x, y, l, h, font, fg);
     }
   else
     {
	/* Vertical part */
	DrawOneLine (frame, 1, 5, x + 2, y + 1, x + 2, y + h, fg);
	DrawOneLine (frame, 1, 5, x + l - 3, y + 1, x + l - 3, y + h, fg);
	/* Upper part */
	DrawOneLine (frame, 2, 5, x + 1, y + 1, x + l, y, fg);
     }
}

/*----------------------------------------------------------------------
  DrawIntersection draw an intersection symbol.
  parameter fg indicates the drawing color
  ----------------------------------------------------------------------*/
void        DrawIntersection (int frame, int x, int y, int l, int h, ptrfont font, int fg)
{
  HPEN        hPen;
  HPEN        hOldPen;
  int         arc, fh;

  if (fg < 0)
    return;
#ifdef _WIN_PRINT
  if (y < 0)
    return;
#endif /* _WIN_PRINT */

  y += FrameTable[frame].FrTopMargin;
  fh = FontHeight (font);
  if (h < fh * 2 && l <= CharacterWidth ('\307', font))
    /* Only one glyph needed */
    DrawMonoSymb ('\307', frame, x, y, l, h, font, fg);
  else
    {
      /* radius of arcs is 6mm */
      arc = h / 4;
      /* vertical part */
      DrawOneLine (frame, 2, 5, x + 1, y + arc, x + 1, y + h, fg);
      DrawOneLine (frame, 2, 5, x + l - 2, y + arc, x + l - 2, y + h, fg);

      /* Upper part */
      hPen = CreatePen (PS_SOLID, 1, ColorPixel (fg));
#ifdef _WIN_PRINT
      hOldPen = SelectObject (TtDisplay, hPen);
      Arc (TtDisplay, x + 1, y + arc , x + l - 2, y, x + 1, y + arc, x + l - 2, y - arc);
      SelectObject (TtDisplay, hOldPen);
#else  /* _WIN_PRINT */
      hOldPen = SelectObject (TtPrinterDC, hPen);
      Arc (TtPrinterDC, x + 1, y + arc , x + l - 2, y, x + 1, y + arc, x + l - 2, y - arc);
      SelectObject (TtPrinterDC, hOldPen);
#endif /* _WIN_PRINT */
      DeleteObject (hPen);
     }
}

/*----------------------------------------------------------------------
  DrawUnion draw an Union symbol.
  parameter fg indicates the drawing color
  ----------------------------------------------------------------------*/
void        DrawUnion (int frame, int x, int y, int l, int h, ptrfont font, int fg)
{
  HPEN        hPen;
  HPEN        hOldPen;
  int         arc, fh;

  if (fg < 0)
    return;
#ifdef _WIN_PRINT
  if (y < 0)
    return;
#endif /* _WIN_PRINT */

   y += FrameTable[frame].FrTopMargin;
   fh = FontHeight (font);
   if (h < fh * 2 && l <= CharacterWidth ('\310', font))
     /* Only one glyph needed */
     DrawMonoSymb ('\310', frame, x, y, l, h, font, fg);
   else
     {
       /* radius of arcs is 3mm */
       arc = h / 4;
       /* two vertical lines */
       DrawOneLine (frame, 2, 5, x + 1, y, x + 1, y + h - arc, fg);
       DrawOneLine (frame, 2, 5, x + l - 2, y, x + l - 2, y + h - arc, fg);
       /* Lower part */
       hPen = CreatePen (PS_SOLID, 1, ColorPixel (fg));
       y += h;
#ifdef _WIN_PRINT
       hOldPen = SelectObject (TtPrinterDC, hPen);
       Arc (TtPrinterDC, x + 1, y - arc , x + l - 2, y, x + 1, y - arc, x + l - 2, y - arc);
       SelectObject (TtPrinterDC, hOldPen);
#else /* _WIN_PRINT */
       hOldPen = SelectObject (TtDisplay, hPen);
       Arc (TtDisplay, x + 1, y - arc , x + l - 2, y, x + 1, y - arc, x + l - 2, y - arc);
       SelectObject (TtDisplay, hOldPen);
#endif /* _WIN_PRINT */
       DeleteObject (hPen);
     }
}

/*----------------------------------------------------------------------
  DrawArrow draw an arrow following the indicated direction in degrees :
  0 (right arrow), 45, 90, 135, 180,
  225, 270 ou 315.
  parameter fg indicates the drawing color
  ----------------------------------------------------------------------*/
void        DrawArrow (int frame, int thick, int style, int x, int y, int l, int h, int orientation, int fg)
{
  int         xm, ym, xf, yf;

  if (thick == 0 || fg < 0)
    return;

#ifdef _WIN_PRINT
  if (y < 0)
    return;
#endif /* _WIN_PRINT */

  y += FrameTable[frame].FrTopMargin;
  xm = x + ((l - thick) / 2);
  xf = x + l - 1;
  ym = y + ((h - thick) / 2);
  yf = y + h - 1;

  if (orientation == 0)
    {
      /* draw a right arrow */
      DrawOneLine (frame, thick, style, x, ym, xf, ym, fg);
      DrawArrowHead (frame, x, ym, xf, ym, thick, fg);
    }
  else if (orientation == 45)
    {
      DrawOneLine (frame, thick, style, x, yf, xf - thick + 1, y, fg);
      DrawArrowHead (frame, x, yf, xf - thick + 1, y, thick, fg);
    }
  else if (orientation == 90)
    {
      /* draw a bottom-up arrow */
      DrawOneLine (frame, thick, style, xm, y, xm, yf, fg);
      DrawArrowHead (frame, xm, yf, xm, y, thick, fg);
    }
  else if (orientation == 135)
    {
      DrawOneLine (frame, thick, style, x, y, xf - thick + 1, yf, fg);
      DrawArrowHead (frame, xf - thick + 1, yf, x, y, thick, fg);
    }
  else if (orientation == 180)
    {
      /* draw a left arrow */
      DrawOneLine (frame, thick, style, x, ym, xf, ym, fg);
      DrawArrowHead (frame, xf, ym, x, ym, thick, fg);
    }
  else if (orientation == 225)
    {
      DrawOneLine (frame, thick, style, x, yf, xf - thick + 1, y, fg);
      DrawArrowHead (frame, xf - thick + 1, y, x, yf, thick, fg);
    }
  else if (orientation == 270)
    {
      /* draw a top-down arrow */
      DrawOneLine (frame, thick, style, xm, y, xm, yf, fg);
      DrawArrowHead (frame, xm, y, xm, yf, thick, fg);
    }
  else if (orientation == 315)
    {
      DrawOneLine (frame, thick, style, x, y, xf - thick + 1, yf, fg);
      DrawArrowHead (frame, x, y, xf - thick + 1, yf, thick, fg);
    }
}


/*----------------------------------------------------------------------
  DrawBracket draw an opening or closing bracket (depending on direction)
  parameter fg indicates the drawing color
  ----------------------------------------------------------------------*/
void        DrawBracket (int frame, int thick, int x, int y, int l, int h, int direction, ptrfont font, int fg)
{
   int         xm, yf, yend;

   if (fg < 0)
     return;
   if (FontHeight (font) >= h)
     {
	/* With only one glyph */
	if (direction == 0)
	  {
	     /* Draw a opening bracket */
	     xm = x + ((l - CharacterWidth ('[', font)) / 2);
	     yf = y + ((h - CharacterHeight ('[', font)) / 2) -
		FontAscent (font) + CharacterAscent ('[', font);
	     DrawChar (TEXT('['), frame, xm, yf, font, fg);
	  }
	else
	  {
	     /* Draw a closing bracket */
	     xm = x + ((l - CharacterWidth (']', font)) / 2);
	     yf = y + ((h - CharacterHeight (']', font)) / 2) -
		FontAscent (font) + CharacterAscent (']', font);
	     DrawChar (TEXT(']'), frame, xm, yf, font, fg);
	  }
     }
   else
     {
	/* Need more than one glyph */
	if (direction == 0)
	  {
	     /* Draw a opening bracket */
	     xm = x + ((l - CharacterWidth ('\351', font)) / 2);
	     yf = y - FontAscent (font) + CharacterAscent ('\351', font);
	     DrawChar (TEXT('\351'), frame, xm, yf, font, fg);
	     yend = y + h - CharacterHeight ('\353', font) -
		FontAscent (font) + CharacterAscent ('\353', font);
	     DrawChar (TEXT('\353'), frame, xm, yend, font, fg);
	     for (yf = yf + CharacterHeight ('\351', font) -
		  FontAscent (font) + CharacterAscent ('\352', font);
		  yf < yend;
		  yf += CharacterHeight ('\352', font))
		DrawChar (TEXT('\352'), frame, xm, yf, font, fg);
	  }
	else
	  {
	     /* Draw a closing bracket */
	     xm = x + ((l - CharacterWidth ('\371', font)) / 2);
	     yf = y - FontAscent (font) + CharacterAscent ('\371', font);
	     DrawChar (TEXT('\371'), frame, xm, yf, font, fg);
	     yend = y + h - CharacterHeight ('\373', font) -
		FontAscent (font) + CharacterAscent ('\373', font);
	     DrawChar (TEXT('\373'), frame, xm, yend, font, fg);
	     for (yf = yf + CharacterHeight ('\371', font) -
		  FontAscent (font) + CharacterAscent ('\372', font);
		  yf < yend;
		  yf += CharacterHeight ('\372', font))
		DrawChar (TEXT('\372'), frame, xm, yf, font, fg);
	  }
     }
}

/*----------------------------------------------------------------------
  DrawPointyBracket draw an opening or closing pointy bracket (depending
  on direction)
  parameter fg indicates the drawing color
  ----------------------------------------------------------------------*/
void        DrawPointyBracket (int frame, int thick, int x, int y, int l, int h, int direction, ptrfont font, int fg)
{
   int         xm, yf;

   if (fg < 0)
     return;
   if (FontHeight (font) >= h)
     {
	/* With only one glyph */
	if (direction == 0)
	  {
	     /* Draw a opening bracket */
	     xm = x + ((l - CharacterWidth ('\341', font)) / 2);
	     yf = y + ((h - CharacterHeight ('\341', font)) / 2) -
		FontAscent (font) + CharacterAscent ('\341', font);
	     DrawChar (TEXT('['), frame, xm, yf, font, fg);
	  }
	else
	  {
	     /* Draw a closing bracket */
	     xm = x + ((l - CharacterWidth ('\361', font)) / 2);
	     yf = y + ((h - CharacterHeight ('\361', font)) / 2) -
		FontAscent (font) + CharacterAscent ('\361', font);
	     DrawChar (TEXT(']'), frame, xm, yf, font, fg);
	  }
     }
   else
     {
       /* Need more than one glyph */
       if (direction == 0)
	 {
	   /* Draw a opening bracket */
	   DrawOneLine (frame, thick, 5, x + l, y, x, y + (h / 2), fg);
	   DrawOneLine (frame, thick, 5, x, y + (h / 2), x + l, y + h, fg);
	 }
       else
	 {
	   /* Draw a closing bracket */
	   DrawOneLine (frame, thick, 5, x, y, x + l, y + (h / 2), fg);
	   DrawOneLine (frame, thick, 5, x + l, y + (h / 2), x, y + h, fg);
	 }
     }
}

/*----------------------------------------------------------------------
  DrawParenthesis draw a closing or opening parenthesis (direction).
  parameter fg indicates the drawing color
  ----------------------------------------------------------------------*/
void        DrawParenthesis (int frame, int thick, int x, int y, int l, int h, int direction, ptrfont font, int fg)
{
  int         xm, yf, yend, exnum, delta;

  if (fg < 0)
    return;
#ifdef _WIN_PRINT
  if (TtPrinterDC)
    {
      exnum = 0;

      if (h <= (int) (1.3 * FontHeight (font)))
	{
	  /* With only one glyph */
	  if (direction == 0)
	    {
	      /* draw an opening parenthesis */
	      xm = x + ((l - CharacterWidth ('(', font)) / 2);
	      yf = y + ((h - CharacterHeight ('(', font)) / 2) - FontAscent (font) + CharacterAscent ('(', font);
	      DrawChar ('(', frame, xm, yf, font, fg);
	    }
	  else
	    { /* draw a closing parenthesis */
	      xm = x + ((l - CharacterWidth (')', font)) / 2);
	      yf = y + ((h - CharacterHeight (')', font)) / 2) - FontAscent (font) + CharacterAscent (')', font);
	      DrawChar (')', frame, xm, yf, font, fg);
	    }
	}
      else
	{ /* Need more than one glyph */
	  if (direction == 0)
	    {
	      /* draw a opening parenthesis */
	      xm = x + ((l - CharacterWidth ('\346', font)) / 2);
	      yf = y - FontAscent (font) + CharacterAscent ('\346', font);
	      DrawChar ('\346', frame, xm, yf, font, fg);
	      yend = y + h - CharacterHeight ('\350', font) - FontAscent (font) + CharacterAscent ('\350', font) - 1;
	      DrawChar ('\350', frame, xm, yend, font, fg);
	      
	      yf += CharacterHeight ('\346', font) - 1;
	      delta = yend - yf;
	      if (delta >= 0)
		{
		  for (yf += CharacterAscent ('\347', font) - FontAscent (font),
			 yend -= CharacterHeight ('\347', font) - 1;
		       yf < yend;
		       yf += CharacterHeight ('\347', font), exnum++)
		    DrawChar ('\347', frame, xm, yf, font, fg);
		  if (exnum)
		    DrawChar ('\347', frame, xm, yend, font, fg);
		  else
		    DrawChar ('\347', frame, xm, yf + ((delta - CharacterHeight ('\347', font)) / 2), font, fg);
		}
	    }
	  else
	    {
	      /* draw a closing parenthesis */
	      xm = x + ((l - CharacterWidth ('\366', font)) / 2);
	      yf = y - FontAscent (font) + CharacterAscent ('\366', font);
	      DrawChar ('\366', frame, xm, yf, font, fg);
	      yend = y + h - CharacterHeight ('\370', font) - FontAscent (font) + CharacterAscent ('\370', font) - 1;
	      DrawChar ('\370', frame, xm, yend, font, fg);

	      yf += CharacterHeight ('\366', font) - 1;
	      delta = yend - yf;
	      if (delta >= 0)
		{
		  for (yf += CharacterAscent ('\367', font) - FontAscent (font),
			 yend -= CharacterHeight ('\367', font) - 1;
		       yf < yend;
		       yf += CharacterHeight ('\367', font), exnum++)
		    DrawChar ('\367', frame, xm, yf, font, fg);
		  if (exnum)
		    DrawChar ('\367', frame, xm, yend, font, fg);
		  else
		    DrawChar ('\367', frame, xm, yf + ((delta - CharacterHeight ('\367', font)) / 2), font, fg);
		}
	    }
	}
   }
#else  /* !_WIN_PRINT */
  exnum = 0;

  if (h <= (int) (1.3 * FontHeight (font)) )
    {
      /* With only one glyph */
      if (direction == 0)
	{
	  /* draw a opening parenthesis */
	  xm = x + ((l - CharacterWidth ('(', font)) / 2);
	  yf = y + ((h - CharacterHeight ('(', font)) / 2) - FontAscent (font) + CharacterAscent ('(', font);
	  DrawChar (TEXT('('), frame, xm, yf, font, fg);
	}
      else
	{
	  /* draw a closing parenthesis */
	  xm = x + ((l - CharacterWidth (')', font)) / 2);
	  yf = y + ((h - CharacterHeight (')', font)) / 2) - FontAscent (font) + CharacterAscent (')', font);
	  DrawChar (TEXT(')'), frame, xm, yf, font, fg);
	}
    }
  else
    {
      /* Need more than one glyph */
      if (direction == 0)
	{
	  /* draw a opening parenthesis */
	  xm = x + ((l - CharacterWidth ('\346', font)) / 2);
	  yf = y - FontAscent (font) + CharacterAscent ('\346', font);
	  DrawChar (TEXT('\346'), frame, xm, yf, font, fg);
	  yend = y + h - CharacterHeight ('\350', font) - FontAscent (font) + CharacterAscent ('\350', font) - 1;
	  DrawChar (TEXT('\350'), frame, xm, yend, font, fg);
	  
	  yf += CharacterHeight ('\346', font) - 1;
	  delta = yend - yf;
	  if (delta >= 0)
	    {
	      for (yf += CharacterAscent ('\347', font) - FontAscent (font),
		     yend -= CharacterHeight ('\347', font) - 1;
		   yf < yend;
		   yf += CharacterHeight ('\347', font), exnum++)
		DrawChar (TEXT('\347'), frame, xm, yf, font, fg);
	      if (exnum)
		DrawChar (TEXT('\347'), frame, xm, yend, font, fg);
	      else
		DrawChar (TEXT('\347'), frame, xm, yf + ((delta - CharacterHeight ('\347', font)) / 2), font, fg);
	    }
	}
      else
	{
	  /* draw a closing parenthesis */
	  xm = x + ((l - CharacterWidth ('\366', font)) / 2);
	  yf = y - FontAscent (font) + CharacterAscent ('\366', font);
	  DrawChar (TEXT('\366'), frame, xm, yf, font, fg);
	  yend = y + h - CharacterHeight ('\370', font) - FontAscent (font) + CharacterAscent ('\370', font) - 1;
	  DrawChar (TEXT('\370'), frame, xm, yend, font, fg);
	  
	  yf += CharacterHeight ('\366', font) - 1;
	  delta = yend - yf;
	  if (delta >= 0)
	    {
	      for (yf += CharacterAscent ('\367', font) - FontAscent (font),
		     yend -= CharacterHeight ('\367', font) - 1;
		   yf < yend;
		   yf += CharacterHeight ('\367', font), exnum++)
		DrawChar (TEXT('\367'), frame, xm, yf, font, fg);
	      if (exnum)
		DrawChar (TEXT('\367'), frame, xm, yend, font, fg);
	      else
		DrawChar (TEXT('\367'), frame, xm, yf + ((delta - CharacterHeight ('\367', font)) / 2), font, fg);
	    }
	}
    }
#endif /* _WIN_PRINT */
}

/*----------------------------------------------------------------------
  DrawBrace draw an opening of closing brace (depending on direction).
  parameter fg indicates the drawing color
  ----------------------------------------------------------------------*/
void        DrawBrace (int frame, int thick, int x, int y, int l, int h, int direction, ptrfont font, int fg)
{
   int         xm, ym, yf, yend, exnum, delta;

   exnum = 0;
   if (fg < 0)
     return;

   if (h <= (int) (1.3 * FontHeight (font)))
     {
	/* need only one char */
	if (direction == 0)
	  {
	     /* just use the opening brace glyph */
	     xm = x + ((l - CharacterWidth ('{', font)) / 2);
	     yf = y + ((h - CharacterHeight ('{', font)) / 2) - FontAscent (font) + CharacterAscent ('{', font);
	     DrawChar (TEXT('{'), frame, xm, yf, font, fg);
	  }
	else
	  {
	     /* just use the closing brace glyph */
	     xm = x + ((l - CharacterWidth ('}', font)) / 2);
	     yf = y + ((h - CharacterHeight ('}', font)) / 2) - FontAscent (font) + CharacterAscent ('}', font);
	     DrawChar (TEXT('}'), frame, xm, yf, font, fg);
	  }
     }

   else
     {
	/* Brace drawn with more than one glyph */
	if (direction == 0)
	  {
	     /* top */
	     xm = x + ((l - CharacterWidth ('\354', font)) / 2);
	     yf = y - FontAscent (font) + CharacterAscent ('\354', font);
	     DrawChar (TEXT('\354'), frame, xm, yf, font, fg);
	     /* vertical line */
	     ym = y + ((h - CharacterHeight ('\355', font)) / 2) - FontAscent (font)
		+ CharacterAscent ('\355', font);
	     DrawChar (TEXT('\355'), frame, xm, ym, font, fg);
	     /* bottom */
	     yend = y + h - CharacterHeight ('\356', font) - FontAscent (font) + CharacterAscent ('\356', font);
	     DrawChar (TEXT('\356'), frame, xm, yend, font, fg);

	     /* finish top */
	     yf += CharacterHeight ('\354', font) - 1;
	     delta = ym - yf;
	     if (delta >= 0)
	       {
		  for (yf += CharacterAscent ('\357', font) - FontAscent (font),
		       ym -= CharacterHeight ('\357', font);
		       yf < ym;
		       yf += CharacterHeight ('\357', font), exnum++)
		     DrawChar (TEXT('\357'), frame, xm, yf, font, fg);
		  if (exnum)
		     DrawChar (TEXT('\357'), frame, xm, ym, font, fg);
		  else
		     DrawChar (TEXT('\357'), frame, xm, yf + ((delta - CharacterHeight ('\357', font)) / 2), font, fg);
	       }
	     /* finish bottom */
	     yf = ym + CharacterHeight ('\355', font) + CharacterHeight ('\357', font);
	     delta = yend - yf;
	     if (delta >= 0)
	       {
		  for (yf += CharacterAscent ('\357', font) - FontAscent (font),
		       yend -= CharacterHeight ('\357', font);
		       yf < yend;
		       yf += CharacterHeight ('\357', font), exnum++)
		     DrawChar (TEXT('\357'), frame, xm, yf, font, fg);
		  if (exnum)
		     DrawChar (TEXT('\357'), frame, xm, yend, font, fg);
		  else
		     DrawChar (TEXT('\357'), frame, xm, yf + ((delta - CharacterHeight ('\357', font)) / 2), font, fg);
	       }
	  }

	else
	  {
	     /* top */
	     xm = x + ((l - CharacterWidth ('\374', font)) / 2);
	     yf = y - FontAscent (font) + CharacterAscent ('\374', font);
	     DrawChar (TEXT('\374'), frame, xm, yf, font, fg);
	     /* center */
	     ym = y + ((h - CharacterHeight ('\375', font)) / 2)
		- FontAscent (font) + CharacterAscent ('\375', font);
	     DrawChar (TEXT('\375'), frame, xm, ym, font, fg);
	     /* bottom */
	     yend = y + h - CharacterHeight ('\376', font)
		- FontAscent (font) + CharacterAscent ('\376', font);
	     DrawChar (TEXT('\376'), frame, xm, yend, font, fg);
	     /* finish top */
	     yf += CharacterHeight ('\374', font) - 1;
	     delta = ym - yf;
	     if (delta >= 0)
	       {
		  for (yf += CharacterAscent ('\357', font) - FontAscent (font),
		       ym -= CharacterHeight ('\357', font);
		       yf < ym;
		       yf += CharacterHeight ('\357', font), exnum++)
		     DrawChar (TEXT('\357'), frame, xm, yf, font, fg);
		  if (exnum)
		     DrawChar (TEXT('\357'), frame, xm, ym, font, fg);
		  else
		     DrawChar (TEXT('\357'), frame, xm, yf + ((delta - CharacterHeight ('\357', font)) / 2), font, fg);
	       }
	     /* finish bottom */
	     yf = ym + CharacterHeight ('\375', font) + CharacterHeight ('\357', font);
	     delta = yend - yf;
	     if (delta >= 0)
	       {
		  for (yf += CharacterAscent ('\357', font) - FontAscent (font),
		       yend -= CharacterHeight ('\357', font);
		       yf < yend;
		       yf += CharacterHeight ('\357', font), exnum++)
		     DrawChar (TEXT('\357'), frame, xm, yf, font, fg);
		  if (exnum)
		     DrawChar (TEXT('\357'), frame, xm, yend, font, fg);
		  else
		     DrawChar (TEXT('\357'), frame, xm, yf + ((delta - CharacterHeight ('\357', font)) / 2), font, fg);
	       }
	  }
     }
}

/*----------------------------------------------------------------------
  DrawRectangle draw a rectangle located at (x, y) in frame,
  of geometry width x height.
  thick indicates the thickness of the lines.
  Parameters fg, bg, and pattern are for drawing
  color, background color and fill pattern.
  ----------------------------------------------------------------------*/
void        DrawRectangle (int frame, int thick, int style, int x, int y, int width, int height, int fg, int bg, int pattern)
{
   LOGBRUSH    logBrush;
   Pixmap      pat = (Pixmap) 0;
   HBRUSH      hBrush;
   HBRUSH      hOldBrush;
   HPEN        hPen;
   HPEN        hOldPen;

   if (width <= 0 || height <= 0)
     return;
#ifdef _WIN_PRINT
   if (y < 0)
     return;
#endif /* _WIN_PRINT */

   y += FrameTable[frame].FrTopMargin;
   if (fg < 0)
     thick = 0;

   /* how to stroke the polygone */
   if (thick == 0)
     hPen = CreatePen (PS_NULL, thick, ColorPixel (fg));
   else
     {
       switch (style)
	 {
	 case 3:
	   hPen = CreatePen (PS_DOT, thick, ColorPixel (fg));
	   break;
	 case 4:
	   hPen = CreatePen (PS_DASH, thick, ColorPixel (fg)); 
	   break;
	 default:
	   hPen = CreatePen (PS_SOLID, thick, ColorPixel (fg));   
	   break;
	 }
     }
   /* how to fill the polygone */
   pat = (Pixmap) CreatePattern (0, fg, bg, pattern);
   if (pat == 0)
     logBrush.lbStyle = BS_NULL;
   else
     {
       logBrush.lbColor = ColorPixel (bg);
       logBrush.lbStyle = BS_SOLID;
 
     } 
   hBrush = CreateBrushIndirect (&logBrush);

#ifdef _WIN_PRINT
   /* fill in the rectangle */
   hOldPen = SelectObject (TtPrinterDC, hPen);
   if (hBrush)
     {
       hOldBrush = SelectObject (TtPrinterDC, hBrush);
       Rectangle (TtPrinterDC, x, y, x + width, y + height);
       SelectObject (TtPrinterDC, hOldBrush);
     }

    SelectObject (TtPrinterDC, hOldPen);
#else  /* _WIN_PRINT */

   WIN_GetDeviceContext (frame);
   SelectClipRgn (TtDisplay, clipRgn);

   /* fill the polygone */
   hOldPen = SelectObject (TtDisplay, hPen) ;
   if (hBrush)
     {
       hOldBrush = SelectObject (TtDisplay, hBrush);
       Rectangle (TtDisplay, x, y, x + width, y + height);
       SelectObject (TtDisplay, hOldBrush);
     }
   SelectObject (TtDisplay, hOldPen);
   WIN_ReleaseDeviceContext ();
#endif /* _WIN_PRINT */

   if (hBrush)
     DeleteObject (hBrush);
   DeleteObject (hPen);
   if (pat != NULL)
      if (!DeleteObject ((HGDIOBJ)pat))
         WinErrorBox (NULL, TEXT("DrawRectangle (4)"));
}

/*----------------------------------------------------------------------
  DrawDiamond draw a diamond.
  Parameters fg, bg, and pattern are for drawing
  color, background color and fill pattern.
  ----------------------------------------------------------------------*/

void        DrawDiamond (int frame, int thick, int style, int x, int y, int width, int height, int fg, int bg, int pattern)
{
}

/*----------------------------------------------------------------------
  DrawSegments draw a set of segments.
  Parameter buffer is a pointer to the list of control points.
  nb indicates the number of points.
  The first point is a fake one containing the geometry.
  fg parameter gives the drawing color.
  arrow parameter indicates whether :
  - no arrow have to be drawn (0)
  - a forward arrow has to be drawn (1)
  - a backward arrow has to be drawn (2)
  - both backward and forward arrows have to be drawn (3)
  ----------------------------------------------------------------------*/
void        DrawSegments (int frame, int thick, int style, int x, int y, PtrTextBuffer buffer, int nb, int fg, int arrow, int bg, int pattern)
{
  ThotPoint  *points;
  int         i, j;
  PtrTextBuffer       adbuff;

  /* fill the included polygon */
  DrawPolygon (frame, 0, style, x, y, buffer, nb, fg, bg, pattern);
  if (thick == 0 || fg < 0)
    return;

  y += FrameTable[frame].FrTopMargin;
  /* Allocate a table of points */
  points = (ThotPoint *) TtaGetMemory (sizeof (ThotPoint) * (nb - 1));
  adbuff = buffer;
  j = 1;
  for (i = 1; i < nb; i++)
    {
      if (j >= adbuff->BuLength &&
	  adbuff->BuNext != NULL)
	{
	  /* Next buffer */
	  adbuff = adbuff->BuNext;
	  j = 0;
	}
      points[i - 1].x = x + PixelValue (adbuff->BuPoints[j].XCoord,
					UnPixel, NULL,
					ViewFrameTable[frame - 1].FrMagnification);
      points[i - 1].y = y + PixelValue (adbuff->BuPoints[j].YCoord,
					UnPixel, NULL,
					ViewFrameTable[frame - 1].FrMagnification);
      j++;
    }

  /* backward arrow  */
  if (arrow == 2 || arrow == 3)
    DrawArrowHead (frame, points[1].x, points[1].y, points[0].x, points[0].y, thick, fg);
  
  /* Draw the border */
  for (i = 1; i < nb - 1; i++)
    DrawOneLine (frame, thick, style,
		 points [i-1].x, points[i-1].y, points[i].x, points[i].y, fg);

  /* Forward arrow */
  if (arrow == 1 || arrow == 3)
    DrawArrowHead (frame, points[nb - 3].x, points[nb - 3].y, points[nb - 2].x, points[nb - 2].y, thick, fg);

   /* free the table of points */
   free (points);
}

/*----------------------------------------------------------------------
  DoDrawPolygon
  Draw a polygon whose points are stored in buffer points
  Parameters fg, bg, and pattern are for drawing
  color, background color and fill pattern.
  ----------------------------------------------------------------------*/
static void  DoDrawPolygon (int frame, int thick, int style,
			    ThotPoint *points, int npoints, int fg, int bg,
			    int pattern)
{
   HPEN                hPen;
   HPEN                hOldPen;
   LOGBRUSH            logBrush;
   HBRUSH              hBrush;
   HBRUSH              hOldBrush;
   Pixmap              pat = NULL;

   if (fg < 0)
     thick = 0;

   /* how to stroke the polygon */
   if (thick == 0)
     hPen = CreatePen (PS_NULL, thick, ColorPixel (fg));
   else
     {
       switch (style)
	 {
	 case 3:
	   hPen = CreatePen (PS_DOT, thick, ColorPixel (fg));
	   break;
	 case 4:
	   hPen = CreatePen (PS_DASH, thick, ColorPixel (fg)); 
	   break;
	 default:
	   hPen = CreatePen (PS_SOLID, thick, ColorPixel (fg));   
	   break;
	 }
     }
   /* how to fill the polygon */
   pat = (Pixmap) CreatePattern (0, fg, bg, pattern);
   if (pat == 0)
     logBrush.lbStyle = BS_NULL;
   else
     {
       logBrush.lbColor = ColorPixel (bg);
       logBrush.lbStyle = BS_SOLID;
 
     } 
   hBrush = CreateBrushIndirect (&logBrush);

#ifdef _WIN_PRINT
   /* fill the polygon */
   hOldPen = SelectObject (TtPrinterDC, hPen);
   if (hBrush)
     {
       hOldBrush = SelectObject (TtPrinterDC, hBrush);
       Polygon (TtPrinterDC, points, npoints);
       SelectObject (TtPrinterDC, hOldBrush);
     }

   /* draw the border */
   if (thick > 0)
     Polyline (TtPrinterDC, points, npoints);
   SelectObject (TtPrinterDC, hOldPen);
#else  /* _WIN_PRINT */
   WIN_GetDeviceContext (frame);
   SelectClipRgn (TtDisplay, clipRgn);

   /* fill the polygon */
   hOldPen = SelectObject (TtDisplay, hPen);
   if (hBrush)
     {
       hOldBrush = SelectObject (TtDisplay, hBrush);
       Polygon (TtDisplay, points, npoints);
       SelectObject (TtDisplay, hOldBrush);
     }

   /* draw the border */
    if (thick > 0)
     Polyline (TtDisplay, points, npoints);
   SelectObject (TtDisplay, hOldPen);
   WIN_ReleaseDeviceContext ();
#endif /* _WIN_PRINT */

   if (hBrush)
     DeleteObject (hBrush);
   DeleteObject (hPen);
   if (pat != 0)
     if (!DeleteObject ((HGDIOBJ) pat))
       WinErrorBox (NULL, TEXT("Pattern"));
}

/*----------------------------------------------------------------------
  DrawPolygon draw a polygon.
  Parameter buffer is a pointer to the list of control points.
  nb indicates the number of points.
  The first point is a fake one containing the geometry.
  Parameters fg, bg, and pattern are for drawing
  color, background color and fill pattern.
  ----------------------------------------------------------------------*/
void          DrawPolygon (int frame, int thick, int style, int x, int y,
			   PtrTextBuffer buffer, int nb, int fg, int bg,
			   int pattern)
{
  ThotPoint          *points;
  int                 i, j;
  PtrTextBuffer       adbuff;

   /* Allocate a table of points */
   points = (ThotPoint *) TtaGetMemory (sizeof (ThotPoint) * nb);
   adbuff = buffer;
   y += FrameTable[frame].FrTopMargin;
   j = 1;
   for (i = 1; i < nb; i++)
     {
	if (j >= adbuff->BuLength && adbuff->BuNext != NULL)
	  {
	    /* Next buffer */
	    adbuff = adbuff->BuNext;
	    j = 0;
	  }
	points[i - 1].x = x + PixelValue (adbuff->BuPoints[j].XCoord,
					  UnPixel, NULL,
					  ViewFrameTable[frame - 1].FrMagnification);
	points[i - 1].y = y + PixelValue (adbuff->BuPoints[j].YCoord,
					  UnPixel, NULL,
					  ViewFrameTable[frame - 1].FrMagnification);
	j++;
     }
   /* Close the polygone */
   points[nb - 1].x = points[0].x;
   points[nb - 1].y = points[0].y;
   DoDrawPolygon (frame, thick, style, points, nb, fg, bg, pattern);
   /* free the table of points */
   free (points);
}

/*----------------------------------------------------------------------
  PolyNewPoint : add a new point to the current polyline.
  ----------------------------------------------------------------------*/
static ThotBool     PolyNewPoint (int x, int y,
				  ThotPoint *points, int *npoints,
				  int *maxpoints)
{
   ThotPoint          *tmp;
   int                 size;

   if (*npoints >= *maxpoints)
     {
	size = *maxpoints + ALLOC_POINTS;
	if ((tmp = (ThotPoint *) realloc (points, size * sizeof (ThotPoint))) == 0)
	   return (FALSE);
	else
	  {
	     /* la reallocation a reussi */
	     points = tmp;
	     *maxpoints = size;
	  }
     }

   /* ignore identical points */
   if (*npoints > 0 &&
       points[*npoints - 1].x == x && points[*npoints - 1].y == y)
      return (FALSE);

   points[*npoints].x = x;
   points[*npoints].y = y;
   (*npoints)++;
   return (TRUE);
}

/*----------------------------------------------------------------------
  PushStack : push a spline on the stack.
  ----------------------------------------------------------------------*/
static void   PushStack (float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4)
{
   StackPoint         *stack_ptr;

   if (stack_deep == MAX_STACK)
      return;

   stack_ptr = &stack[stack_deep];
   stack_ptr->x1 = x1;
   stack_ptr->y1 = y1;
   stack_ptr->x2 = x2;
   stack_ptr->y2 = y2;
   stack_ptr->x3 = x3;
   stack_ptr->y3 = y3;
   stack_ptr->x4 = x4;
   stack_ptr->y4 = y4;
   stack_deep++;
}

/*----------------------------------------------------------------------
  PopStack : pop a spline from the stack.
  ----------------------------------------------------------------------*/
static ThotBool     PopStack (float *x1, float *y1, float *x2, float *y2, float *x3, float *y3, float *x4, float *y4)
{
   StackPoint         *stack_ptr;

   if (stack_deep == 0)
      return (FALSE);

   stack_deep--;
   stack_ptr = &stack[stack_deep];
   *x1 = stack_ptr->x1;
   *y1 = stack_ptr->y1;
   *x2 = stack_ptr->x2;
   *y2 = stack_ptr->y2;
   *x3 = stack_ptr->x3;
   *y3 = stack_ptr->y3;
   *x4 = stack_ptr->x4;
   *y4 = stack_ptr->y4;
   return (TRUE);
}


/*----------------------------------------------------------------------
  PolySplit : split a poly line and push the results on the stack.
  ----------------------------------------------------------------------*/
static void         PolySplit (float a1, float b1, float a2, float b2,
			       float a3, float b3, float a4, float b4,
			       ThotPoint *points, int *npoints, int *maxpoints)
{
   register float      tx, ty;
   float               x1, y1, x2, y2, x3, y3, x4, y4;
   float               sx1, sy1, sx2, sy2;
   float               tx1, ty1, tx2, ty2, xmid, ymid;

   stack_deep = 0;
   PushStack (a1, b1, a2, b2, a3, b3, a4, b4);

   while (PopStack (&x1, &y1, &x2, &y2, &x3, &y3, &x4, &y4))
     {
	if (fabs (x1 - x4) < SEG_SPLINE && fabs (y1 - y4) < SEG_SPLINE)
	   PolyNewPoint (FloatToInt (x1), FloatToInt (y1), points, npoints,
			 maxpoints);
	else
	  {
	     tx   = (float) MIDDLE_OF (x2, x3);
	     ty   = (float) MIDDLE_OF (y2, y3);
	     sx1  = (float) MIDDLE_OF (x1, x2);
	     sy1  = (float) MIDDLE_OF (y1, y2);
	     sx2  = (float) MIDDLE_OF (sx1, tx);
	     sy2  = (float) MIDDLE_OF (sy1, ty);
	     tx2  = (float) MIDDLE_OF (x3, x4);
	     ty2  = (float) MIDDLE_OF (y3, y4);
	     tx1  = (float) MIDDLE_OF (tx2, tx);
	     ty1  = (float) MIDDLE_OF (ty2, ty);
	     xmid = (float) MIDDLE_OF (sx2, tx1);
	     ymid = (float) MIDDLE_OF (sy2, ty1);

	     PushStack (xmid, ymid, tx1, ty1, tx2, ty2, x4, y4);
	     PushStack (x1, y1, sx1, sy1, sx2, sy2, xmid, ymid);
	  }
     }
}

/*----------------------------------------------------------------------
  QuadraticSplit : split a quadratic Bezier and pushes the result on the stack.
  ----------------------------------------------------------------------*/
static void         QuadraticSplit (float a1, float b1, float a2, float b2,
				    float a3, float b3,
				    ThotPoint *points, int *npoints,
				    int *maxpoints)
{
   register float      tx, ty;
   float               x1, y1, x2, y2, x3, y3, i, j;
   float               sx, sy;
   float               xmid, ymid;

   stack_deep = 0;
   PushStack (a1, b1, a2, b2, a3, b3, 0, 0);

   while (PopStack (&x1, &y1, &x2, &y2, &x3, &y3, &i, &j))
     {
	if (fabs (x1 - x3) < SEG_SPLINE && fabs (y1 - y3) < SEG_SPLINE)
	   PolyNewPoint (FloatToInt (x1), FloatToInt (y1), points, npoints,
			 maxpoints);
	else
	  {
	     tx   = (float) MIDDLE_OF (x2, x3);
	     ty   = (float) MIDDLE_OF (y2, y3);
	     sx   = (float) MIDDLE_OF (x1, x2);
	     sy   = (float) MIDDLE_OF (y1, y2);
	     xmid = (float) MIDDLE_OF (sx, tx);
	     ymid = (float) MIDDLE_OF (sy, ty);

	     PushStack (xmid, ymid, tx, ty, x3, y3, 0, 0);
	     PushStack (x1, y1, sx, sy, xmid, ymid, 0, 0);
	  }
     }
}

/*----------------------------------------------------------------------
  DrawCurve draw an open curve.
  Parameter buffer is a pointer to the list of control points.
  nb indicates the number of points.
  The first point is a fake one containing the geometry.
  fg indicates the drawing color
  arrow parameter indicates whether :
  - no arrow have to be drawn (0)
  - a forward arrow has to be drawn (1)
  - a backward arrow has to be drawn (2)
  - both backward and forward arrows have to be drawn (3)
  Parameter control indicates the control points.
  ----------------------------------------------------------------------*/
void          DrawCurve (int frame, int thick, int style, int x, int y, PtrTextBuffer buffer, int nb, int fg, int arrow, C_points * controls)
{
  ThotPoint           *points;
  int                 npoints, maxpoints;
  PtrTextBuffer       adbuff;
  int                 i, j;
  float               x1, y1, x2, y2;
  float               cx1, cy1, cx2, cy2;
  HPEN                hPen;
  HPEN                hOldPen;

  /* alloue la liste des points */
  npoints = 0;
  maxpoints = ALLOC_POINTS;
  points = (ThotPoint *) TtaGetMemory (sizeof (ThotPoint) * maxpoints);
  adbuff = buffer;
  y += FrameTable[frame].FrTopMargin;
  j = 1;
  x1 = (float) (x + PixelValue (adbuff->BuPoints[j].XCoord,
				UnPixel, NULL,
				ViewFrameTable[frame - 1].FrMagnification));
  y1 = (float) (y + PixelValue (adbuff->BuPoints[j].YCoord,
				UnPixel, NULL,
				ViewFrameTable[frame - 1].FrMagnification));
  j++;
  cx1 = (controls[j].lx * 3 + x1 - x) / 4 + x;
  cy1 = (controls[j].ly * 3 + y1 - y) / 4 + y;
  x2 = (float) (x + PixelValue (adbuff->BuPoints[j].XCoord,
				UnPixel, NULL,
				ViewFrameTable[frame - 1].FrMagnification));
  y2 = (float) (y + PixelValue (adbuff->BuPoints[j].YCoord,
				UnPixel, NULL,
				ViewFrameTable[frame - 1].FrMagnification));
  cx2 = (controls[j].lx * 3 + x2 - x) / 4 + x;
  cy2 = (controls[j].ly * 3 + y2 - y) / 4 + y;

  /* backward arrow  */
  if (arrow == 2 || arrow == 3)
      DrawArrowHead (frame, FloatToInt (cx1), FloatToInt (cy1), (int) x1, (int) y1, thick, fg);

  for (i = 2; i < nb; i++)
    {
      PolySplit (x1, y1, cx1, cy1, cx2, cy2, x2, y2, points, &npoints,
		 &maxpoints);
      /* skip to next points */
      x1 = x2;
      y1 = y2;
      cx1 = controls[i].rx + x;
      cy1 = controls[i].ry + y;
      if (i < nb - 1)
	{
	  /* not finished */
	  j++;
	  if (j >= adbuff->BuLength &&
	      adbuff->BuNext != NULL)
	    {
	      /* Next buffer */
	      adbuff = adbuff->BuNext;
	      j = 0;
	    }
	  x2 = (float) (x + PixelValue (adbuff->BuPoints[j].XCoord,
					UnPixel, NULL,
					ViewFrameTable[frame - 1].FrMagnification));
	  y2 = (float) (y + PixelValue (adbuff->BuPoints[j].YCoord,
					UnPixel, NULL,
					ViewFrameTable[frame - 1].FrMagnification));
	  if (i == nb - 2)
	    {
	      cx1 = (controls[i].rx * 3 + x1 - x) / 4 + x;
	      cy1 = (controls[i].ry * 3 + y1 - y) / 4 + y;
	      cx2 = (controls[i].rx * 3 + x2 - x) / 4 + x;
	      cy2 = (controls[i].ry * 3 + y2 - y) / 4 + y;
	    }
	  else
	    {
	      cx2 = controls[i + 1].lx + x;
	      cy2 = controls[i + 1].ly + y;
	    }
	}
    }
  PolyNewPoint ((int) x2, (int) y2, points, &npoints, &maxpoints);

   if (fg < 0)
     thick = 0;
   /* how to stroke the polygone */
   if (thick == 0)
     hPen = CreatePen (PS_NULL, thick, ColorPixel (fg));
   else
     {
       switch (style)
	 {
	 case 3:
	   hPen = CreatePen (PS_DOT, thick, ColorPixel (fg));
	   break;
	 case 4:
	   hPen = CreatePen (PS_DASH, thick, ColorPixel (fg)); 
	   break;
	 default:
	   hPen = CreatePen (PS_SOLID, thick, ColorPixel (fg));   
	   break;
	 }
     }
#ifdef _WIN_PRINT
   /* fill the polygone */
   hOldPen = SelectObject (TtPrinterDC, hPen);

   /* draw the border */
   if (thick > 0)
     Polyline (TtPrinterDC, points, nb);
   SelectObject (TtPrinterDC, hOldPen);
#else  /* _WIN_PRINT */
   WIN_GetDeviceContext (frame);
   SelectClipRgn (TtDisplay, clipRgn);

   /* fill the polygone */
   hOldPen = SelectObject (TtDisplay, hPen);
   /* draw the border */
    if (thick > 0)
     Polyline (TtDisplay, points, nb);
   SelectObject (TtDisplay, hOldPen);
   WIN_ReleaseDeviceContext ();
#endif /* _WIN_PRINT */

   DeleteObject (hPen);
  /* Forward arrow */
  if (arrow == 1 || arrow == 3)
    DrawArrowHead (frame, FloatToInt (cx2), FloatToInt (cy2), (int) x2, (int) y2, thick, fg);

   /* free the table of points */
   free (points);
}

/*----------------------------------------------------------------------
  DrawSpline draw a closed curve.
  Parameter buffer is a pointer to the list of control points.
  nb indicates the number of points.
  The first point is a fake one containing the geometry.
  Parameters fg, bg, and pattern are for drawing
  color, background color and fill pattern.
  Parameter controls contains the list of control points.
  ----------------------------------------------------------------------*/
void          DrawSpline (int frame, int thick, int style, int x, int y, PtrTextBuffer buffer, int nb, int fg, int bg, int pattern, C_points * controls)
{
  ThotPoint     *points;
  int           npoints, maxpoints;
  PtrTextBuffer adbuff;
  int           i, j;
  float         x1, y1, x2, y2;
  float         cx1, cy1, cx2, cy2;
  Pixmap        pat = (Pixmap) 0;
  HPEN          hPen;
  HPEN          hOldPen;
  LOGBRUSH      logBrush;
  HBRUSH        hBrush;
  HBRUSH        hOldBrush;

  /* allocate the list of points */
  npoints = 0;
  maxpoints = ALLOC_POINTS;
  points = (ThotPoint *) TtaGetMemory (sizeof (ThotPoint) * maxpoints);
  adbuff = buffer;
  y += FrameTable[frame].FrTopMargin;
  j = 1;
  x1 = (float) (x + PixelValue (adbuff->BuPoints[j].XCoord,
				UnPixel, NULL,
				ViewFrameTable[frame - 1].FrMagnification));
  y1 = (float) (y + PixelValue (adbuff->BuPoints[j].YCoord,
				UnPixel, NULL,
				ViewFrameTable[frame - 1].FrMagnification));
  cx1 = controls[j].rx + x;
  cy1 = controls[j].ry + y;
  j++;
  x2 = (float) (x + PixelValue (adbuff->BuPoints[j].XCoord,
				UnPixel, NULL,
				ViewFrameTable[frame - 1].FrMagnification));
  y2 = (float) (y + PixelValue (adbuff->BuPoints[j].YCoord,
				UnPixel, NULL,
				ViewFrameTable[frame - 1].FrMagnification));
  cx2 = controls[j].lx + x;
  cy2 = controls[j].ly + y;

  for (i = 2; i < nb; i++)
    {
      PolySplit (x1, y1, cx1, cy1, cx2, cy2, x2, y2, points, &npoints,
		 &maxpoints);
      /* next points */
      x1 = x2;
      y1 = y2;
      cx1 = controls[i].rx + x;
      cy1 = controls[i].ry + y;
      if (i < nb - 1)
	{
	  /* not the last loop */
	  j++;
	  if (j >= adbuff->BuLength &&
	      adbuff->BuNext != NULL)
	    {
	      /* Next buffer */
	      adbuff = adbuff->BuNext;
	      j = 0;
	    }
	  x2 = (float) (x + PixelValue (adbuff->BuPoints[j].XCoord,
					UnPixel, NULL,
					ViewFrameTable[frame - 1].FrMagnification));
	  y2 = (float) (y + PixelValue (adbuff->BuPoints[j].YCoord,
					UnPixel, NULL,
					ViewFrameTable[frame - 1].FrMagnification));
	  cx2 = controls[i + 1].lx + x;
	  cy2 = controls[i + 1].ly + y;
	}
      else
	{
	  /* loop around the origin point */
	  x2 = (float) (x + PixelValue (buffer->BuPoints[1].XCoord,
					UnPixel, NULL,
					ViewFrameTable[frame - 1].FrMagnification));
	  y2 = (float) (y + PixelValue (buffer->BuPoints[1].YCoord,
					UnPixel, NULL,
					ViewFrameTable[frame - 1].FrMagnification));
	  cx2 = controls[1].lx + x;
	  cy2 = controls[1].ly + y;
	}
    }

  /* close the polyline */
  PolySplit (x1, y1, cx1, cy1, cx2, cy2, x2, y2, points, &npoints, &maxpoints);
  PolyNewPoint ((int) x2, (int) y2, points, &npoints, &maxpoints);

   if (fg < 0)
     thick = 0;
   /* how to stroke the polygone */
   if (thick == 0)
     hPen = CreatePen (PS_NULL, thick, ColorPixel (fg));
   else
     {
       switch (style)
	 {
	 case 3:
	   hPen = CreatePen (PS_DOT, thick, ColorPixel (fg));
	   break;
	 case 4:
	   hPen = CreatePen (PS_DASH, thick, ColorPixel (fg)); 
	   break;
	 default:
	   hPen = CreatePen (PS_SOLID, thick, ColorPixel (fg));   
	   break;
	 }
     }
   /* how to fill the polygone */
   pat = (Pixmap) CreatePattern (0, fg, bg, pattern);
   if (pat == 0)
     logBrush.lbStyle = BS_NULL;
   else
     {
       logBrush.lbColor = ColorPixel (bg);
       logBrush.lbStyle = BS_SOLID;
 
     } 
   hBrush = CreateBrushIndirect (&logBrush);

#ifdef _WIN_PRINT
   /* fill the polygone */
   hOldPen = SelectObject (TtPrinterDC, hPen);
   if (hBrush)
     {
       hOldBrush = SelectObject (TtPrinterDC, hBrush);
       Polygon (TtPrinterDC, points, nb);
       SelectObject (TtPrinterDC, hOldBrush);
     }

   /* draw the border */
   if (thick > 0)
     Polyline (TtPrinterDC, points, nb);
   SelectObject (TtPrinterDC, hOldPen);
#else  /* _WIN_PRINT */
   WIN_GetDeviceContext (frame);
   SelectClipRgn (TtDisplay, clipRgn);

   /* fill the polygone */
   hOldPen = SelectObject (TtDisplay, hPen);
   if (hBrush)
     {
       hOldBrush = SelectObject (TtDisplay, hBrush);
       Polygon (TtDisplay, points, nb);
       SelectObject (TtDisplay, hOldBrush);
     }

   /* draw the border */
    if (thick > 0)
     Polyline (TtDisplay, points, nb);
   SelectObject (TtDisplay, hOldPen);
   WIN_ReleaseDeviceContext ();
#endif /* _WIN_PRINT */

   if (hBrush)
     DeleteObject (hBrush);
   DeleteObject (hPen);
   if (pat != 0)
     if (!DeleteObject ((HGDIOBJ) pat))
       WinErrorBox (NULL, TEXT("Pattern"));

   /* free the table of points */
   free (points);
}

/*----------------------------------------------------------------------
  DrawCurrent
  Draws the polyline or polygon corresponding to the list of points
  contained in buffer points.
  Parameter path is a pointer to the list of path segments
  fg indicates the drawing color
  ----------------------------------------------------------------------*/
static void  DrawCurrent (int frame, int thick, int style,
			  ThotPoint *points, int npoints,
			  int fg, int bg, int pattern)
{
  if (npoints > 1)
    {
      if (npoints == 2)
	/* only two points, that's a single segment */
	DrawOneLine (frame, thick, style, points[0].x, points[0].y,
		     points[1].x, points[1].y, fg);
      else
	/* draw a polyline or a ploygon */
	DoDrawPolygon (frame, thick, style, points, npoints, fg, bg, pattern);
    }
}

/*----------------------------------------------------------------------
  DrawPath draws a path.
  Parameter path is a pointer to the list of path segments
  fg indicates the drawing color
  ----------------------------------------------------------------------*/
void            DrawPath (int frame, int thick, int style, int x, int y,
			  PtrPathSeg path, int fg, int bg, int pattern)
{
  ThotPoint           *points;
  int                 npoints, maxpoints;
  PtrPathSeg          pPa;
  float               x1, y1, cx1, cy1, x2, y2, cx2, cy2;

  if (thick > 0 || fg >= 0)
    {
      y += FrameTable[frame].FrTopMargin;
      /* alloue la liste des points */
      maxpoints = ALLOC_POINTS;
      points = (ThotPoint *) TtaGetMemory (sizeof (ThotPoint) * maxpoints);
      npoints = 0;
      pPa = path;
      while (pPa)
	{
	  if (pPa->PaNewSubpath)
	    /* this path segment starts a new subpath */
	    /* if some points are already stored, display the line
	       they represent */
	    if (npoints > 1)
	      {
		DrawCurrent (frame, thick, style, points, npoints, fg, bg,
			     pattern);
		npoints = 0;
	      }

	  switch (pPa->PaShape)
	    {
	    case PtLine:
	      x1 = (float) (x + PixelValue (pPa->XStart, UnPixel, NULL,
				   ViewFrameTable[frame - 1].FrMagnification));
	      y1 = (float) (y + PixelValue (pPa->YStart, UnPixel, NULL,
				   ViewFrameTable[frame - 1].FrMagnification));
	      x2 = (float) (x + PixelValue (pPa->XEnd, UnPixel, NULL,
				   ViewFrameTable[frame - 1].FrMagnification));
	      y2 = (float) (y + PixelValue (pPa->YEnd, UnPixel, NULL,
				   ViewFrameTable[frame - 1].FrMagnification));
	      PolyNewPoint ((int) x1, (int) y1, points, &npoints, &maxpoints);
	      PolyNewPoint ((int) x2, (int) y2, points, &npoints, &maxpoints);
	      break;

	    case PtCubicBezier:
	      x1 = (float) (x + PixelValue (pPa->XStart, UnPixel, NULL,
				   ViewFrameTable[frame - 1].FrMagnification));
	      y1 = (float) (y + PixelValue (pPa->YStart, UnPixel, NULL,
				   ViewFrameTable[frame - 1].FrMagnification));
	      cx1 = (float) (x + PixelValue (pPa->XCtrlStart, UnPixel, NULL,
				   ViewFrameTable[frame - 1].FrMagnification));
	      cy1 = (float) (y + PixelValue (pPa->YCtrlStart, UnPixel, NULL,
				   ViewFrameTable[frame - 1].FrMagnification));
	      x2 = (float) (x + PixelValue (pPa->XEnd, UnPixel, NULL,
				   ViewFrameTable[frame - 1].FrMagnification));
	      y2 = (float) (y + PixelValue (pPa->YEnd, UnPixel, NULL,
				   ViewFrameTable[frame - 1].FrMagnification));
	      cx2 = (float) (x + PixelValue (pPa->XCtrlEnd, UnPixel, NULL,
				   ViewFrameTable[frame - 1].FrMagnification));
	      cy2 = (float) (y + PixelValue (pPa->YCtrlEnd, UnPixel, NULL,
				   ViewFrameTable[frame - 1].FrMagnification));
	      PolySplit (x1, y1, cx1, cy1, cx2, cy2, x2, y2, points, &npoints,
			 &maxpoints);
	      PolyNewPoint ((int) x2, (int) y2, points, &npoints, &maxpoints);
	      break;

	    case PtQuadraticBezier:
	      x1 = (float) (x + PixelValue (pPa->XStart, UnPixel, NULL,
				   ViewFrameTable[frame - 1].FrMagnification));
	      y1 = (float) (y + PixelValue (pPa->YStart, UnPixel, NULL,
				   ViewFrameTable[frame - 1].FrMagnification));
	      cx1 = (float) (x + PixelValue (pPa->XCtrlStart, UnPixel, NULL,
				   ViewFrameTable[frame - 1].FrMagnification));
	      cy1 = (float) (y + PixelValue (pPa->YCtrlStart, UnPixel, NULL,
				   ViewFrameTable[frame - 1].FrMagnification));
	      x2 = (float) (x + PixelValue (pPa->XEnd, UnPixel, NULL,
				   ViewFrameTable[frame - 1].FrMagnification));
	      y2 = (float) (y + PixelValue (pPa->YEnd, UnPixel, NULL,
				   ViewFrameTable[frame - 1].FrMagnification));
	      QuadraticSplit (x1, y1, cx1, cy1, x2, y2, points, &npoints,
			      &maxpoints);
	      PolyNewPoint ((int) x2, (int) y2, points, &npoints, &maxpoints);
	      break;

	    case PtEllipticalArc:
	      /**** to do ****/
	      break;
	    }
	  pPa = pPa->PaNext;
	}
      /* if some points are left in the buffer, display the line they
	 represent */
      if (npoints > 1)
	DrawCurrent (frame, thick, style, points, npoints, fg, bg, pattern);
      /* free the table of points */
      free (points);
    }
}

/*----------------------------------------------------------------------
  DrawOval draw a rectangle with rounded corners.
  Parameters fg, bg, and pattern are for drawing
  color, background color and fill pattern.
  ----------------------------------------------------------------------*/
void            DrawOval (int frame, int thick, int style, int x, int y,
			  int width, int height, int rx, int ry,
			  int fg, int bg, int pattern)
{
  Pixmap        pat = (Pixmap) 0;
  HPEN          hPen;
  HPEN          hOldPen;
  LOGBRUSH      logBrush;
  HBRUSH        hBrush;
  HBRUSH        hOldBrush;
  int	        arc;

  if (width <= 0 || height <= 0) 
    return;

#ifdef _WIN_PRINT
   if (y < 0)
	 return;
#endif /* _WIN_PRINT */

   /* radius of arcs */
   if (rx == 0 && ry != 0)
     rx = ry;
   else if (ry == 0 && rx != 0)
     ry = rx;
   arc = width / 2;
   if (rx > arc)
     rx = arc;
   arc = height / 2;
   if (ry > arc)
     ry = arc;
   /* check width and height */
   if (width > thick + 1)
     width = width - thick - 1;
   if (height > thick + 1)
     height = height - thick - 1;
   x += thick / 2;
   y = y + thick / 2 + FrameTable[frame].FrTopMargin;

   if (fg < 0)
     thick = 0;
   /* how to stroke the polygone */
   if (thick == 0)
     hPen = CreatePen (PS_NULL, thick, ColorPixel (fg));
   else
     {
       switch (style)
	 {
	 case 3:
	   hPen = CreatePen (PS_DOT, thick, ColorPixel (fg));
	   break;
	 case 4:
	   hPen = CreatePen (PS_DASH, thick, ColorPixel (fg)); 
	   break;
	 default:
	   hPen = CreatePen (PS_SOLID, thick, ColorPixel (fg));   
	   break;
	 }
     }
   /* how to fill the polygone */
   pat = (Pixmap) CreatePattern (0, fg, bg, pattern);
   if (pat == 0)
     logBrush.lbStyle = BS_NULL;
   else
     {
       logBrush.lbColor = ColorPixel (bg);
       logBrush.lbStyle = BS_SOLID;
 
     } 
   hBrush = CreateBrushIndirect (&logBrush);

#ifdef _WIN_PRINT
   hOldPen = SelectObject (TtPrinterDC, hPen);
   hOldBrush = SelectObject (TtPrinterDC, hBrush);
   RoundRect (TtPrinterDC, x, y, x + width, y + height, rx * 2, ry * 2);
   SelectObject (TtPrinterDC, hOldBrush);
   SelectObject (TtPrinterDC, hOldPen);
#else  /* _WIN_PRINT */
   WIN_GetDeviceContext (frame);
   SelectClipRgn (TtDisplay, clipRgn);
   /* fill the polygone */
   hOldPen = SelectObject (TtDisplay, hPen);
   hOldBrush = SelectObject (TtDisplay, hBrush);
   RoundRect (TtDisplay, x, y, x + width, y + height, rx * 2, ry * 2);
   SelectObject (TtDisplay, hOldBrush);
   SelectObject (TtDisplay, hOldPen);
   WIN_ReleaseDeviceContext ();
#endif /* _WIN_PRINT */

   DeleteObject (hBrush);
   DeleteObject (hPen);
   if (pat != 0)
     if (!DeleteObject ((HGDIOBJ) pat))
       WinErrorBox (NULL, TEXT("Pattern"));
}

/*----------------------------------------------------------------------
  DrawEllips draw an ellips (or a circle).
  Parameters fg, bg, and pattern are for drawing color, background color
  and fill pattern.
  ----------------------------------------------------------------------*/
void          DrawEllips (int frame, int thick, int style, int x, int y, int width, int height, int fg, int bg, int pattern)
{
  Pixmap   pat = (Pixmap)0;
  HPEN     hPen;
  HPEN     hOldPen;
  HBRUSH   hBrush;
  HBRUSH   hOldBrush;
  LOGBRUSH logBrush;

  if (width <= 0 || height <= 0) 
    return;

#ifdef _WIN_PRINT
  if (y < 0)
	return;
#endif /* _WIN_PRINT */

  if (fg < 0)
    thick = 0;
  /* how to stroke the polygone */
  if (thick == 0)
    hPen = CreatePen (PS_NULL, thick, ColorPixel (fg));
  else
    {
     switch (style)
	 {
	 case 3:
	   hPen = CreatePen (PS_DOT, thick, ColorPixel (fg));
	   break;
	 case 4:
	   hPen = CreatePen (PS_DASH, thick, ColorPixel (fg)); 
	   break;
	 default:
	   hPen = CreatePen (PS_SOLID, thick, ColorPixel (fg));   
	   break;
	 }
    }
  /* how to fill the polygone */
  pat = (Pixmap) CreatePattern (0, fg, bg, pattern);
  if (pat == 0)
    logBrush.lbStyle = BS_NULL;
  else
     {
       logBrush.lbColor = ColorPixel (bg);
       logBrush.lbStyle = BS_SOLID;
 
     } 
  hBrush = CreateBrushIndirect (&logBrush);
  y += FrameTable[frame].FrTopMargin;

#ifdef _WIN_PRINT
   hOldPen = SelectObject (TtPrinterDC, hPen);
   hOldBrush = SelectObject (TtPrinterDC, hBrush);
   Ellipse (TtPrinterDC, x, y, x + width, y + height);
   SelectObject (TtPrinterDC, hOldBrush);
   SelectObject (TtPrinterDC, hOldPen);
#else  /* _WIN_PRINT */
   WIN_GetDeviceContext (frame);
   SelectClipRgn (TtDisplay, clipRgn);
   /* fill the polygone */
   hOldPen = SelectObject (TtDisplay, hPen);
   hOldBrush = SelectObject (TtDisplay, hBrush);
   Ellipse (TtDisplay, x, y, x + width, y + height);
   SelectObject (TtDisplay, hOldBrush);
   SelectObject (TtDisplay, hOldPen);
   WIN_ReleaseDeviceContext ();
#endif /* _WIN_PRINT */

   DeleteObject (hBrush);
   DeleteObject (hPen);
   if (pat != 0)
     if (!DeleteObject ((HGDIOBJ) pat))
       WinErrorBox (NULL, TEXT("Pattern"));
}

/*----------------------------------------------------------------------
  DrawHorizontalLine draw a vertical line aligned top center or bottom
  depending on align value.
  parameter fg indicates the drawing color
  ----------------------------------------------------------------------*/
void          DrawHorizontalLine (int frame, int thick, int style, int x, int y, int l, int h, int align, int fg)
{
   int        Y;

   if (thick <= 0 || fg < 0)
     return;
#ifdef _WIN_PRINT 
   if (y < 0)
     return;
#endif /* _WIN_PRINT */
   y += FrameTable[frame].FrTopMargin;
   if (align == 1)
      Y = y + h / 2;
   else if (align == 2)
      Y = y + h - (thick + 1) / 2;
   else
      Y = y + thick / 2;
   DrawOneLine (frame, thick, style, x + thick / 2, Y, x + l - (thick + 1) / 2, Y, fg);
}

/*----------------------------------------------------------------------
  DrawHorizontalBrace draw a horizontal brace aligned top
  or bottom depending on align value.
  parameter fg indicates the drawing color
  ----------------------------------------------------------------------*/
void          DrawHorizontalBrace (int frame, int thick, int style, int x, int y, int l, int h, int align, int fg)
{
  int        Y;

  if (thick <= 0 || fg < 0)
    return;
#ifdef _WIN_PRINT 
  if (y < 0)
    return;
#endif /* _WIN_PRINT */
  y += FrameTable[frame].FrTopMargin;
  Y = y + (h - thick) / 2;
  DrawOneLine (frame, thick, style, x, Y, x + l, Y, fg);
  if (align == 0)
	/* Over brace */
	{
	  DrawOneLine (frame, thick, style, x, Y, x, y + h, fg);
	  DrawOneLine (frame, thick, style, x + (l / 2), Y, x + (l / 2), y, fg);
	  DrawOneLine (frame, thick, style, x + l - thick, Y, x + l - thick, y + h, fg);
	}
  else
	/* Underbrace */
	{
	  DrawOneLine (frame, thick, style, x, Y, x, y, fg);
	  DrawOneLine (frame, thick, style, x + (l / 2), Y, x + (l / 2), y + h, fg);
	  DrawOneLine (frame, thick, style, x + l - thick, Y, x + l - thick, y, fg);
	}
}

/*----------------------------------------------------------------------
  DrawVerticalLine draw a vertical line aligned left center or right
  depending on align value.
  parameter fg indicates the drawing color
  ----------------------------------------------------------------------*/
void          DrawVerticalLine (int frame, int thick, int style, int x, int y, int l, int h, int align, int fg)
{
  int        X;

  if (thick <= 0 || fg < 0)
    return;
#ifdef _WIN_PRINT
  if (y < 0)
    return;
#endif /* _WIN_PRINT */

  y += FrameTable[frame].FrTopMargin;
  if (align == 1)
    X = x + thick / 2;
  else if (align == 2)
    X = x + l - (thick + 1) / 2;
  else
    X = x + thick / 2;
  DrawOneLine (frame, thick, style, X, y + thick / 2, X, y + h - (thick + 1) / 2, fg);
}

/*----------------------------------------------------------------------
  DrawDoubleVerticalLine draw a double vertical line aligned left center or
  right depending on align value.
  parameter fg indicates the drawing color
  ----------------------------------------------------------------------*/
void          DrawDoubleVerticalLine (int frame, int thick, int style, int x, int y, int l, int h, int align, int fg)
{
  int        X;

  if (thick == 0 || fg < 0)
      return;
#ifdef _WIN_PRINT
   if (y < 0)
      return;
#endif /* _WIN_PRINT */

  y += FrameTable[frame].FrTopMargin;
   if (align == 1)
      X = x + l / 2;
   else if (align == 2)
      X = x + l;
   else
      X = x;
   DrawOneLine (frame, thick, style, X, y, X, y + h, fg);
   X = X + (3 * thick);
   DrawOneLine (frame, thick, style, X, y, X, y + h, fg);
}

/*----------------------------------------------------------------------
  DrawSlash draw a slash or backslash depending on direction.
  Le parame`tre indique la couleur du trace'.
  ----------------------------------------------------------------------*/
void          DrawSlash (int frame, int thick, int style, int x, int y, int l, int h, int direction, int fg)
{
  int              xf, yf;

  if (thick == 0 || fg < 0)
    return;

#ifdef _WIN_PRINT
  if (y < 0)
    return;
#endif /* _WIN_PRINT */

  y += FrameTable[frame].FrTopMargin;
  xf = x + l;
  yf = y + h;
  if (direction == 0)
    DrawOneLine (frame, thick, style, x, yf, xf, y, fg);
  else
    DrawOneLine (frame, thick, style, x, y, xf, yf, fg);
}

/*----------------------------------------------------------------------
  DrawCorner draw a corner.
  parameter fg indicates the drawing color
  ----------------------------------------------------------------------*/
void        DrawCorner (int frame, int thick, int style, int x, int y, int l, int h, int corner, int fg)
{
  ThotPoint   point[3];
  int         xf, yf;
  HPEN        hPen;
  HPEN        hOldPen;

  if (thick == 0 || fg < 0)
    return;
#ifdef _WIN_PRINT
  if (y < 0)
    return;
#endif  /* _WIN_PRINT */

   y += FrameTable[frame].FrTopMargin;
   xf = x + l;
   yf = y + h;
   switch (corner)
     {
     case 0:
       point[0].x = x;
       point[0].y = y;
       point[1].x = xf;
       point[1].y = y;
       point[2].x = xf;
       point[2].y = yf;
       break;
     case 1:
       point[0].x = xf;
       point[0].y = y;
       point[1].x = xf;
       point[1].y = yf;
       point[2].x = x;
       point[2].y = yf;
       break;
     case 2:
       point[0].x = xf;
       point[0].y = yf;
       point[1].x = x;
       point[1].y = yf;
       point[2].x = x;
       point[2].y = y;
       break;
     case 3:
       point[0].x = x;
       point[0].y = yf;
       point[1].x = x;
       point[1].y = y;
       point[2].x = xf;
       point[2].y = y;
       break;
     }

  switch (style)
  {
    case 3:
	  hPen = CreatePen (PS_DOT, thick, ColorPixel (fg));
	  break;
    case 4:
	  hPen = CreatePen (PS_DASH, thick, ColorPixel (fg)); 
	  break;
    default:
	  hPen = CreatePen (PS_SOLID, thick, ColorPixel (fg));   
	  break;
  }

#ifdef _WIN_PRINT
   hOldPen = SelectObject (TtDisplay, hPen);
   Polyline (TtDisplay, point, 3);
   SelectObject (TtDisplay, hOldPen);
 #else /* _WIN_PRINT */
   WIN_GetDeviceContext (frame);
   hOldPen = SelectObject (TtDisplay, hPen);
   Polyline (TtDisplay, point, 3);
   SelectObject (TtDisplay, hOldPen);
   WIN_ReleaseDeviceContext ();
#endif /* _WIN_PRINT */

   DeleteObject (hPen);
}

/*----------------------------------------------------------------------
  DrawRectangleFrame draw a rectangle with smoothed corners (3mm radius)
  and with an horizontal line at 6mm from top.
  Parameters fg, bg, and pattern are for drawing
  color, background color and fill pattern.
  ----------------------------------------------------------------------*/
void        DrawRectangleFrame (int frame, int thick, int style, int x, int y, int width, int height, int fg, int bg, int pattern)
{
}

/*----------------------------------------------------------------------
  DrawEllipsFrame draw an ellipse at 7mm under the top of the
  enclosing box.
  Parameters fg, bg, and pattern are for drawing
  color, background color and fill pattern.
  ----------------------------------------------------------------------*/
void        DrawEllipsFrame (int frame, int thick, int style, int x, int y, int width, int height, int fg, int bg, int pattern)
{
}

/*----------------------------------------------------------------------
  StorePageInfo and psBoundingBox are empty, they have no meaning in
  this context and are kept for interface compatibility.
  ----------------------------------------------------------------------*/
void        StorePageInfo (int pagenum, int width, int height)
{
#ifdef _WIN_PRINT
   LastPageNumber = pagenum;
   LastPageWidth = width;
   LastPageHeight = height;
#endif /* WIN_PRINT */
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void        psBoundingBox (int frame, int width, int height)
{
}

#ifndef _WIN_PRINT
/*----------------------------------------------------------------------
  Clear clear the area of frame located at (x, y) and of size width x height.
  ----------------------------------------------------------------------*/
void        Clear (int frame, int width, int height, int x, int y)
{
   ThotWindow          w;
   HBRUSH              hBrush;
   HBRUSH              hOldBrush;

   w = FrRef[frame];
   if (w != None)
     {
	WIN_GetDeviceContext (frame);
	hBrush = CreateSolidBrush (ColorPixel (BackgroundColor[frame]));
	hOldBrush = SelectObject (TtDisplay, hBrush);
	PatBlt (TtDisplay, x, y + FrameTable[frame].FrTopMargin, width, height, PATCOPY);
	SelectObject (TtDisplay, hOldBrush);
        WIN_ReleaseDeviceContext ();
	DeleteObject (hBrush);
     }
}

/*----------------------------------------------------------------------
  Scroll do a scrolling/Bitblt of frame of a width x height area
  from (xd,yd) to (xf,yf).
  ----------------------------------------------------------------------*/
void Scroll (int frame, int width, int height, int xd, int yd, int xf, int yf)
{
  RECT cltRect;

  if (FrRef[frame] != None)
  {
    WIN_GetDeviceContext (frame);
    GetClientRect (FrRef [frame], &cltRect);
    if (autoScroll)
      ScrollDC (TtDisplay, xf - xd, yf - yd, NULL, &cltRect, NULL, NULL);
    else 
	  /* UpdateWindow (FrRef [frame]); */
	  ScrollWindowEx (FrRef [frame], xf - xd, yf - yd, NULL, &cltRect, NULL, NULL, SW_INVALIDATE);
    WIN_ReleaseDeviceContext ();
  }
}
#endif /* _WIN_PRINT */

/*----------------------------------------------------------------------
  PaintWithPattern fill the rectangle associated to a window w (or frame if w= 0)
  located on (x , y) and geometry width x height, using the
  given pattern.
  Parameters fg, bg, and pattern are for drawing
  color, background color and fill pattern.
  ----------------------------------------------------------------------*/
void        PaintWithPattern (int frame, int x, int y, int width, int height, ThotWindow w, int fg, int bg, int pattern)
{
}
