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
extern BOOL            autoScroll;
extern int             LastPageNumber, LastPageWidth, LastPageHeight;

int                    X, Y;

static DWORD           fontLangInfo = -1;
static int             SameBox = 0; /* 1 if the text is in the same box */
static int             NbWhiteSp;

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

  cr = ColorPixel (color);
  SetBkColor (TtDisplay, cr); 
}
#endif /* _WIN_PRINT */


/*----------------------------------------------------------------------
  TranslateChars substitute in text the space chars to their visual
  equivalents.
  ----------------------------------------------------------------------*/
static void         TranslateChars (USTRING text)
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
      if (!ShowSpace)
	    text[i] = (UCHAR_T) SHOWN_BREAK_LINE;
	  break;
	case THIN_SPACE:
      if (!ShowSpace)
	    text[i] = (UCHAR_T) SHOWN_THIN_SPACE;
	  break;
	case HALF_EM:
      if (!ShowSpace)
	    text[i] = (UCHAR_T) SHOWN_HALF_EM;
	  break;
	case UNBREAKABLE_SPACE:
      if (!ShowSpace)
	    text[i] = (UCHAR_T) SHOWN_UNBREAKABLE_SPACE;
	  break;
	case SPACE:
      if (!ShowSpace)
	    text[i] = (UCHAR_T) SHOWN_SPACE;
	  break;
	case START_ENTITY:
	  text[i] = '&';
	}
      i++;
    }
}

/*----------------------------------------------------------------------
  DrawArrowHead draw the end of an arrow.
  ----------------------------------------------------------------------*/
static void DrawArrowHead (int frame, int x1, int y1, int x2, int y2,
			   int thick, int fg)
{
  float               x, y, xb, yb, dx, dy, l, sina, cosa;
  float               width, height;
  HPEN                hPen;
  HPEN                hOldPen;
  HDC                 display;
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
  display = TtPrinterDC;
#else /* _WIN_PRINT */
  display = TtDisplay;
  SelectClipRgn (display, clipRgn);
#endif /* _WIN_PRINT */
  hOldPen = SelectObject (display, hPen);
  Polyline (display, point, 4);
  SelectObject (display, hOldPen);
  DeleteObject (hPen);
}

/*----------------------------------------------------------------------
  DrawOneLine draw one line starting from (x1, y1) to (x2, y2) in frame.
  ----------------------------------------------------------------------*/
static void  DrawOneLine (int frame, int thick, int style, int x1, int y1,
			  int x2, int y2, int fg)
{
  HPEN     hPen;
  HPEN     hOldPen;
  HDC      display;

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
  display = TtPrinterDC;
#else /* _WIN_PRINT */
  display = TtDisplay;
  SelectClipRgn (display, clipRgn);
#endif /* _WIN_PRINT */
  hOldPen = SelectObject (display, hPen);
  SelectClipRgn (display, clipRgn);
  MoveToEx (display, x1, y1, NULL);
  LineTo (display, x2, y2);
  SelectObject (display, hOldPen);
  DeleteObject (hPen);
}


/*----------------------------------------------------------------------
  DrawChar draw a char at location (x, y) in frame and with font.
  The parameter fg indicates the drawing color
  ----------------------------------------------------------------------*/
void DrawChar (UCHAR_T car, int frame, int x, int y, ptrfont font, int fg)
{
  CHAR_T              str[2] = {car, 0};
  HFONT               hOldFont;
  HDC                 display;

  if (fg < 0)
    return;
   y += FrameTable[frame].FrTopMargin;

#ifdef _WIN_PRINT
  display = TtPrinterDC;
#else /* _WIN_PRINT */
  display = TtDisplay;
  SelectClipRgn (display, clipRgn);
#endif /* _WIN_PRINT */
   SetTextColor (display, ColorPixel (fg));
   SetBkMode (display, TRANSPARENT);
   SetMapperFlags (display, 1);
   hOldFont = WinLoadFont (display, font);
   TextOut (display, x, y, (USTRING) str, 1);
   SelectObject (display, hOldFont);
   DeleteObject (ActiveFont);
   ActiveFont = (HFONT)0;
}

/*----------------------------------------------------------------------
  DrawString draw a char string of lg chars beginning at buff[i].
  Drawing starts at (x, y) in frame and using font.
  boxWidth gives the width of the final box or zero,
  this is used only by the thot formmating engine.
  bl indicates taht there is a space before the string
  hyphen indicates whether an hyphen char has to be added.
  startABlock is 1 if the text is at a paragraph beginning
  (no justification of first spaces).
  parameter fg indicates the drawing color
  Returns the lenght of the string drawn.
  ----------------------------------------------------------------------*/
int DrawString (STRING buff, int i, int lg, int frame, int x, int y,
		ptrfont font, int boxWidth, int bl, int hyphen, int startABlock,
		int fg, int shadow)
{
  HDC                 display;
  HFONT               hOldFont;
  STRING              ptcar;
  SIZE                size;
  int                 j, width;
#ifdef _I18N_
  GCP_RESULTS         results;
  USHORT              auGlyphs [2000];
  CHAR_T              szNewText [2000];
  UINT                infoFlag;
  int                 anDX [2000];
#endif /* _I18N_ */

  if (lg <= 0)
    return 0;

#ifdef _WIN_PRINT
  if (y < 0)
    return 0;
  display = TtPrinterDC;
#else /* _WIN_PRINT */
  if (FrRef[frame] == NULL)
    return 0;
  display = TtDisplay;
  SelectClipRgn (display, clipRgn);
#endif /* _WIN_PRINT */

  SetMapperFlags (display, 1);
  hOldFont = WinLoadFont (display, font);
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
      if (buff[i - 1] == '\212' || buff[i - 1] == '\12')
	{
	  /* skip the Control return char */
	  i++;
	  lg--;
	}
      ustrncpy (ptcar, &buff[i - 1], lg);
      ptcar[lg] = EOS;
	  TranslateChars (ptcar);	/* substitute spaces */
    }
  /* get the string size */
  GetTextExtentPoint (display, ptcar, lg, &size);
  width = size.cx;
  if (fg >= 0)
    {
      /* not transparent -> draw charaters */
      y += FrameTable[frame].FrTopMargin;
      SetTextColor (display, ColorPixel (fg));
      SetBkMode (display, TRANSPARENT);
      TextOut (display, x, y, (USTRING) ptcar, lg);
      if (hyphen)
	/* draw the hyphen */
	TextOut (display, x + width, y, TEXT("\255"), 1);
    }

  TtaFreeMemory (ptcar);
  SelectObject (display, hOldFont);
  DeleteObject (ActiveFont);
  ActiveFont = (HFONT)0;
  return (width);
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
void DisplayUnderline (int frame, int x, int y, ptrfont font, int type,
		       int lg, int fg)
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
void      DrawPoints (int frame, int x, int y, int boxWidth, int fg)
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
  if (boxWidth > 0)
    {
      y = y - FontAscent (font) + CharacterAscent (' ', font);
      ptcar = TEXT(" .");
      /* compute lenght of the string " ." */
      width = CharacterWidth (SPACE, font) + CharacterWidth (TEXT('.'), font);
      /* compute the number of string to write */
      nb = boxWidth / width;
      xcour = x + (boxWidth % width);
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
void DrawRadical (int frame, int thick, int x, int y, int l, int h,
		  ptrfont font, int fg)
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
void DrawIntegral (int frame, int thick, int x, int y, int l, int h,
		   int type, ptrfont font, int fg)
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
static void DrawMonoSymb (CHAR_T symb, int frame, int x, int y, int l,
			  int h, ptrfont font, int fg)
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
void DrawSigma (int frame, int x, int y, int l, int h, ptrfont font, int fg)
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
void DrawPi (int frame, int x, int y, int l, int h, ptrfont font, int fg)
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
void DrawIntersection (int frame, int x, int y, int l, int h,
		       ptrfont font, int fg)
{
  HPEN        hPen;
  HPEN        hOldPen;
  HDC         display;
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
      display = TtPrinterDC;
#else /* _WIN_PRINT */
      display = TtDisplay;
      SelectClipRgn (display, clipRgn);
#endif /* _WIN_PRINT */
      hOldPen = SelectObject (display, hPen);
      Arc (display, x + 1, y + arc , x + l - 2, y, x + 1, y + arc, x + l - 2, y - arc);
      SelectObject (display, hOldPen);
      DeleteObject (hPen);
     }
}

/*----------------------------------------------------------------------
  DrawUnion draw an Union symbol.
  parameter fg indicates the drawing color
  ----------------------------------------------------------------------*/
void DrawUnion (int frame, int x, int y, int l, int h, ptrfont font, int fg)
{
  HPEN        hPen;
  HPEN        hOldPen;
  HDC         display;
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
       display = TtPrinterDC;
#else /* _WIN_PRINT */
       display = TtDisplay;
       SelectClipRgn (display, clipRgn);
#endif /* _WIN_PRINT */
       hOldPen = SelectObject (display, hPen);
       Arc (display, x + 1, y - arc , x + l - 2, y, x + 1, y - arc, x + l - 2, y - arc);
       SelectObject (display, hOldPen);
       DeleteObject (hPen);
     }
}

/*----------------------------------------------------------------------
  DrawArrow draw an arrow following the indicated direction in degrees :
  0 (right arrow), 45, 90, 135, 180,
  225, 270 ou 315.
  parameter fg indicates the drawing color
  ----------------------------------------------------------------------*/
void DrawArrow (int frame, int thick, int style, int x, int y, int l,
		int h, int orientation, int fg)
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
void DrawBracket (int frame, int thick, int x, int y, int l, int h,
		  int direction, ptrfont font, int fg)
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
void DrawPointyBracket (int frame, int thick, int x, int y, int l, int h,
			int direction, ptrfont font, int fg)
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
void DrawParenthesis (int frame, int thick, int x, int y, int l, int h,
		      int direction, ptrfont font, int fg)
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
void DrawBrace (int frame, int thick, int x, int y, int l, int h,
		int direction, ptrfont font, int fg)
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
void DrawRectangle (int frame, int thick, int style, int x, int y, int width,
		    int height, int fg, int bg, int pattern)
{
   LOGBRUSH    logBrush;
   Pixmap      pat = (Pixmap) 0;
   HBRUSH      hBrush;
   HBRUSH      hOldBrush;
   HPEN        hPen;
   HPEN        hOldPen;
   HDC         display;

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
   display = TtPrinterDC;
#else /* _WIN_PRINT */
   display = TtDisplay;
   SelectClipRgn (display, clipRgn);
#endif /* _WIN_PRINT */
   /* fill the polygone */
   hOldPen = SelectObject (display, hPen) ;
   if (hBrush)
     {
       hOldBrush = SelectObject (display, hBrush);
       Rectangle (display, x, y, x + width, y + height);
       SelectObject (display, hOldBrush);
       DeleteObject (hBrush);
     }
   SelectObject (display, hOldPen);
   DeleteObject (hPen);
   if (pat != NULL)
      DeleteObject ((HGDIOBJ)pat);
}

/*----------------------------------------------------------------------
  DrawDiamond draw a diamond.
  Parameters fg, bg, and pattern are for drawing
  color, background color and fill pattern.
  ----------------------------------------------------------------------*/
void DrawDiamond (int frame, int thick, int style, int x, int y, int width,
		  int height, int fg, int bg, int pattern)
{
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
   HDC                 display;
   LOGBRUSH            logBrush;
   HBRUSH              hBrush = NULL;
   HBRUSH              hOldBrush;
   Pixmap              pat;

   if (fg < 0)
     thick = 0;
#ifdef _WIN_PRINT
   display = TtPrinterDC;
#else  /* _WIN_PRINT */
   display = TtDisplay;
   SelectClipRgn (display, clipRgn);
#endif /* _WIN_PRINT */

   /* how to fill the polygon */
   pat = (Pixmap) CreatePattern (0, fg, bg, pattern);
   if (pat != NULL)
     {
       logBrush.lbColor = ColorPixel (bg);
       logBrush.lbStyle = BS_SOLID;
       hBrush = CreateBrushIndirect (&logBrush); 
       /* fill the polygon */
       hPen = CreatePen (PS_NULL, thick, ColorPixel (fg));
       hOldPen = SelectObject (display, hPen);
       hOldBrush = SelectObject (display, hBrush);
       Polygon (display, points, npoints);
       SelectObject (display, hOldPen);
       DeleteObject (hPen);
       SelectObject (display, hOldBrush);
       DeleteObject (hBrush);
       DeleteObject ((HGDIOBJ) pat);
     }

   /* how to stroke the polygon */
   if (thick > 0)
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
     /* draw the border */
     hOldPen = SelectObject (display, hPen);
     Polyline (display, points, npoints);
	 SelectObject (display, hOldPen);
     DeleteObject (hPen);
    }
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
void DrawSegments (int frame, int thick, int style, int x, int y,
		   PtrTextBuffer buffer, int nb, int fg, int arrow,
		   int bg, int pattern)
{
  ThotPoint          *points;
  PtrTextBuffer       adbuff;
  int                 i, j;

  /* Allocate a table of points */
  points = (ThotPoint *) TtaGetMemory (sizeof (ThotPoint) * (nb - 1));
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

   DoDrawPolygon (frame, thick, style, points, nb - 1, fg, bg, pattern);
  /* backward arrow  */
  if (arrow == 2 || arrow == 3)
    DrawArrowHead (frame, points[1].x, points[1].y, points[0].x, points[0].y, thick, fg);

  /* Forward arrow */
  if (arrow == 1 || arrow == 3)
    DrawArrowHead (frame, points[nb - 3].x, points[nb - 3].y, points[nb - 2].x, points[nb - 2].y, thick, fg);

   /* free the table of points */
   free (points);
}

/*----------------------------------------------------------------------
  DrawPolygon draw a polygon.
  Parameter buffer is a pointer to the list of control points.
  nb indicates the number of points.
  The first point is a fake one containing the geometry.
  Parameters fg, bg, and pattern are for drawing
  color, background color and fill pattern.
  ----------------------------------------------------------------------*/
void DrawPolygon (int frame, int thick, int style, int x, int y,
			   PtrTextBuffer buffer, int nb, int fg, int bg,
			   int pattern)
{
  ThotPoint          *points;
  PtrTextBuffer       adbuff;
  int                 i, j;

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
   /* Close the polygon */
   points[nb - 1].x = points[0].x;
   points[nb - 1].y = points[0].y;
   DoDrawPolygon (frame, thick, style, points, nb, fg, bg, pattern);
   /* free the table of points */
   free (points);
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
void DrawCurve (int frame, int thick, int style, int x, int y,
		PtrTextBuffer buffer, int nb, int fg, int arrow,
		C_points *controls)
{
  PtrTextBuffer       adbuff;
  int                 i, j;
  int                 x1, y1, x2, y2;
  int                 cx1, cy1, cx2, cy2;
  POINT               ptCurve[3];
  HDC                 display;
  HPEN                hPen;
  HPEN                hOldPen;

  if (fg < 0)
    thick = 0;
  if (thick <= 0)
    return;

  adbuff = buffer;
  y += FrameTable[frame].FrTopMargin;
  j = 1;
  x1 = x + PixelValue (adbuff->BuPoints[j].XCoord,
				UnPixel, NULL,
				ViewFrameTable[frame - 1].FrMagnification);
  y1 = y + PixelValue (adbuff->BuPoints[j].YCoord,
				UnPixel, NULL,
				ViewFrameTable[frame - 1].FrMagnification);
  j++;
  cx1 = (controls[j].lx * 3 + x1 - x) / 4 + x;
  cy1 = (controls[j].ly * 3 + y1 - y) / 4 + y;
  x2 = x + PixelValue (adbuff->BuPoints[j].XCoord,
				UnPixel, NULL,
				ViewFrameTable[frame - 1].FrMagnification);
  y2 = y + PixelValue (adbuff->BuPoints[j].YCoord,
				UnPixel, NULL,
				ViewFrameTable[frame - 1].FrMagnification);
  cx2 = (controls[j].lx * 3 + x2 - x) / 4 + x;
  cy2 = (controls[j].ly * 3 + y2 - y) / 4 + y;

  /* backward arrow  */
  if (arrow == 2 || arrow == 3)
      DrawArrowHead (frame, cx1, cy1, x1,
		     y1, thick, fg);

#ifdef _WIN_PRINT
  display = TtPrinterDC;
#else  /* _WIN_PRINT */
  display = TtDisplay;
  SelectClipRgn (display, clipRgn);
#endif /* _WIN_PRINT */

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
  hOldPen = SelectObject (display, hPen);

  MoveToEx (display, x1, y1, NULL);
  for (i = 2; i < nb; i++)
    {
      ptCurve[0].x = cx1;
      ptCurve[0].y = cy1;
      ptCurve[1].x = cx2;
      ptCurve[1].y = cy2;
      ptCurve[2].x = x2;
      ptCurve[2].y = y2;
      PolyBezierTo (display, &ptCurve[0], 3);

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
	  x2 = (float) (x + PixelValue (adbuff->BuPoints[j].XCoord, UnPixel,
			     NULL, ViewFrameTable[frame - 1].FrMagnification));
	  y2 = (float) (y + PixelValue (adbuff->BuPoints[j].YCoord, UnPixel,
                             NULL, ViewFrameTable[frame - 1].FrMagnification));
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

  SelectObject (display, hOldPen);
  DeleteObject (hPen);
  /* Forward arrow */
  if (arrow == 1 || arrow == 3)
    DrawArrowHead (frame, cx2, cy2, x2, y2, thick, fg);
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
void DrawSpline (int frame, int thick, int style, int x, int y,
		 PtrTextBuffer buffer, int nb, int fg, int bg,
		 int pattern, C_points *controls)
{
  PtrTextBuffer adbuff;
  int           i, j;
  float         x1, y1, x2, y2;
  float         cx1, cy1, cx2, cy2;
  Pixmap        pat = (Pixmap) 0;
  HDC           display;
  HPEN          hPen;
  HPEN          hOldPen;
  LOGBRUSH      logBrush;
  HBRUSH        hBrush;
  HBRUSH        hOldBrush;
  POINT         ptCurve[3];

  if (fg < 0)
    /* the outline is transparent. Don't draw it */
    thick = 0;
  if (thick <= 0 && bg < 0)
    return;

#ifdef _WIN_PRINT
   display = TtPrinterDC;
#else  /* _WIN_PRINT */
   display = TtDisplay;
   SelectClipRgn (display, clipRgn);
#endif /* _WIN_PRINT */
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
  if (hBrush)
    hOldBrush = SelectObject (display, hBrush);
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
  hOldPen = SelectObject (display, hPen);

  y += FrameTable[frame].FrTopMargin;
  adbuff = buffer;
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

  BeginPath (display);
  MoveToEx (display, x1, y1, NULL);

  for (i = 2; i < nb; i++)
    {
      ptCurve[0].x = cx1;
      ptCurve[0].y = cy1;
      ptCurve[1].x = cx2;
      ptCurve[1].y = cy2;
      ptCurve[2].x = x2;
      ptCurve[2].y = y2;
      PolyBezierTo (display, &ptCurve[0], 3);

      /* next points */
      cx1 = controls[i].rx + x;
      cy1 = controls[i].ry + y;
      if (i < nb - 1)
	{
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
	  /* last point. The next one is the first */
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
  ptCurve[0].x = cx1;
  ptCurve[0].y = cy1;
  ptCurve[1].x = cx2;
  ptCurve[1].y = cy2;
  ptCurve[2].x = x2;
  ptCurve[2].y = y2;
  PolyBezierTo (display, &ptCurve[0], 3);
  EndPath (display);
  /* draw the curve */
  StrokeAndFillPath (display);

  if (hBrush)
    {
      SelectObject (display, hOldBrush);
      DeleteObject (hBrush);
    }
  if (pat != 0)
    if (!DeleteObject ((HGDIOBJ) pat))
      WinErrorBox (NULL, TEXT("Pattern"));
  SelectObject (display, hOldPen);
  DeleteObject (hPen);
}

/*----------------------------------------------------------------------
  SetPath draws a path.
  Parameter path is a pointer to the list of path segments
  fg indicates the drawing color
  ----------------------------------------------------------------------*/
static void SetPath (int frame, HDC display, int x, int y, PtrPathSeg path)
{
  PtrPathSeg          pPa;
  int                 x1, y1, cx1, cy1, x2, y2;
  POINT               ptCurve[3];

  y += FrameTable[frame].FrTopMargin;
  pPa = path;
  while (pPa)
    {
      if (pPa->PaNewSubpath || !pPa->PaPrevious)
	/* this path segment starts a new subpath */
	{
	  x1 = x + PixelValue (pPa->XStart, UnPixel, NULL,
			       ViewFrameTable[frame - 1].FrMagnification);
	  y1 = y + PixelValue (pPa->YStart, UnPixel, NULL,
			       ViewFrameTable[frame - 1].FrMagnification);
	  MoveToEx (display, x1, y1, NULL);
	}

      switch (pPa->PaShape)
	{
	case PtLine:
	  x2 = x + PixelValue (pPa->XEnd, UnPixel, NULL,
			       ViewFrameTable[frame - 1].FrMagnification);
	  y2 = y + PixelValue (pPa->YEnd, UnPixel, NULL,
			       ViewFrameTable[frame - 1].FrMagnification);
	  LineTo (display, x2, y2);
	  break;

	case PtCubicBezier:
	  ptCurve[0].x = x + PixelValue (pPa->XCtrlStart, UnPixel, NULL,
				    ViewFrameTable[frame - 1].FrMagnification);
	  ptCurve[0].y = y + PixelValue (pPa->YCtrlStart, UnPixel, NULL,
				    ViewFrameTable[frame - 1].FrMagnification);
	  ptCurve[1].x = x + PixelValue (pPa->XCtrlEnd, UnPixel, NULL,
			            ViewFrameTable[frame - 1].FrMagnification);
	  ptCurve[1].y = y + PixelValue (pPa->YCtrlEnd, UnPixel, NULL,
                                    ViewFrameTable[frame - 1].FrMagnification);
	  ptCurve[2].x = x + PixelValue (pPa->XEnd, UnPixel, NULL,
				    ViewFrameTable[frame - 1].FrMagnification);
	  ptCurve[2].y = y + PixelValue (pPa->YEnd, UnPixel, NULL,
				    ViewFrameTable[frame - 1].FrMagnification);
	  x2 = ptCurve[2].x;
	  y2 = ptCurve[2].y;
	  PolyBezierTo (display, &ptCurve[0], 3);
	  break;

	case PtQuadraticBezier:
	  x1 = x + PixelValue (pPa->XStart, UnPixel, NULL,
			       ViewFrameTable[frame - 1].FrMagnification);
	  y1 = y + PixelValue (pPa->YStart, UnPixel, NULL,
			       ViewFrameTable[frame - 1].FrMagnification);
	  cx1 = x + PixelValue (pPa->XCtrlStart, UnPixel, NULL,
				ViewFrameTable[frame - 1].FrMagnification);
	  cy1 = y + PixelValue (pPa->YCtrlStart, UnPixel, NULL,
				ViewFrameTable[frame - 1].FrMagnification);
	  x2 = x + PixelValue (pPa->XEnd, UnPixel, NULL,
			       ViewFrameTable[frame - 1].FrMagnification);
	  y2 = y + PixelValue (pPa->YEnd, UnPixel, NULL,
			       ViewFrameTable[frame - 1].FrMagnification);
	  ptCurve[0].x = x1+((2*(cx1-x1))/3);
	  ptCurve[0].y = y1+((2*(cy1-y1))/3);
	  ptCurve[1].x = x2+((2*(cx1-x2))/3);
	  ptCurve[1].y = y2+((2*(cy1-y2))/3);
	  ptCurve[2].x = x2;
	  ptCurve[2].y = y2;
	  PolyBezierTo (display, &ptCurve[0], 3);
	  break;

	case PtEllipticalArc:
	  x2 = x + PixelValue (pPa->XEnd, UnPixel, NULL,
			       ViewFrameTable[frame - 1].FrMagnification);
	  y2 = y + PixelValue (pPa->YEnd, UnPixel, NULL,
			       ViewFrameTable[frame - 1].FrMagnification);
	  LineTo (display, x2, y2); 	  /**** to do ****/
	  break;
	}
      pPa = pPa->PaNext;
    }
}

/*----------------------------------------------------------------------
  DrawPath draws a path.
  Parameter path is a pointer to the list of path segments
  fg indicates the drawing color
  ----------------------------------------------------------------------*/
void DrawPath (int frame, int thick, int style, int x, int y,
	       PtrPathSeg path, int fg, int bg, int pattern)
{
  HDC                 display;
  Pixmap              pat;
  HPEN                hPen;
  HPEN                hOldPen;
  LOGBRUSH            logBrush;
  HBRUSH              hBrush = NULL;
  HBRUSH              hOldBrush;

  if (fg < 0)
    thick = 0;
  if (thick > 0 || bg >= 0)
    {
#ifdef _WIN_PRINT
      display = TtPrinterDC;
#else  /* _WIN_PRINT */
      display = TtDisplay;
      SelectClipRgn (display, clipRgn);
#endif /* _WIN_PRINT */
      /* first, fill the path */
      pat = (Pixmap) CreatePattern (0, fg, bg, pattern);
      if (pat)
	{
	  logBrush.lbColor = ColorPixel (bg);
	  logBrush.lbStyle = BS_SOLID;
	  hBrush = CreateBrushIndirect (&logBrush); 
	  hOldBrush = SelectObject (display, hBrush);
	  BeginPath (display);
	  SetPath (frame, display, x, y, path);
	  EndPath (display);
	  FillPath (display);
	  SelectObject (display, hOldBrush);
	  DeleteObject (hBrush);
	  DeleteObject ((HGDIOBJ) pat);
	}

      if (thick > 0)
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
	  hOldPen = SelectObject (display, hPen);
	  SetPath (frame, display, x, y, path);
	  SelectObject (display, hOldPen);
	  DeleteObject (hPen);
	}
    }
}

/*----------------------------------------------------------------------
  DrawOval draw a rectangle with rounded corners.
  Parameters fg, bg, and pattern are for drawing
  color, background color and fill pattern.
  ----------------------------------------------------------------------*/
void DrawOval (int frame, int thick, int style, int x, int y,
	       int width, int height, int rx, int ry,
	       int fg, int bg, int pattern)
{
  Pixmap        pat = (Pixmap) 0;
  HPEN          hPen;
  HPEN          hOldPen;
  LOGBRUSH      logBrush;
  HBRUSH        hBrush;
  HBRUSH        hOldBrush;
  HDC           display;
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
   display = TtPrinterDC;
#else /* _WIN_PRINT */
   display = TtDisplay;
   SelectClipRgn (display, clipRgn);
#endif /* _WIN_PRINT */
   /* fill the polygone */
   hOldPen = SelectObject (display, hPen);
   hOldBrush = SelectObject (display, hBrush);
   RoundRect (display, x, y, x + width, y + height, rx * 2, ry * 2);
   SelectObject (display, hOldBrush);
   DeleteObject (hBrush);
   SelectObject (display, hOldPen);
   DeleteObject (hPen);
   if (pat != 0)
     DeleteObject ((HGDIOBJ) pat);
}

/*----------------------------------------------------------------------
  DrawEllips draw an ellips (or a circle).
  Parameters fg, bg, and pattern are for drawing color, background color
  and fill pattern.
  ----------------------------------------------------------------------*/
void DrawEllips (int frame, int thick, int style, int x, int y, int width,
		 int height, int fg, int bg, int pattern)
{
  Pixmap   pat = (Pixmap)0;
  HDC      display;
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
  display = TtPrinterDC;
#else /* _WIN_PRINT */
  display = TtDisplay;
  SelectClipRgn (display, clipRgn);
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

   /* fill the polygone */
   hOldPen = SelectObject (display, hPen);
   hOldBrush = SelectObject (display, hBrush);
   Ellipse (display, x, y, x + width, y + height);
   SelectObject (display, hOldBrush);
   DeleteObject (hBrush);
   SelectObject (display, hOldPen);
   DeleteObject (hPen);
   if (pat != 0)
     DeleteObject ((HGDIOBJ) pat);
}

/*----------------------------------------------------------------------
  DrawHorizontalLine draw a vertical line aligned top center or bottom
  depending on align value.
  parameter fg indicates the drawing color
  ----------------------------------------------------------------------*/
void DrawHorizontalLine (int frame, int thick, int style, int x, int y,
			 int l, int h, int align, int fg)
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
void DrawHorizontalBrace (int frame, int thick, int style, int x, int y,
			  int l, int h, int align, int fg)
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
void DrawVerticalLine (int frame, int thick, int style, int x, int y, int l,
		       int h, int align, int fg)
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
void DrawDoubleVerticalLine (int frame, int thick, int style, int x, int y,
			     int l, int h, int align, int fg)
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
void DrawSlash (int frame, int thick, int style, int x, int y, int l, int h,
		int direction, int fg)
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
void DrawCorner (int frame, int thick, int style, int x, int y, int l,
		 int h, int corner, int fg)
{
  ThotPoint   point[3];
  int         xf, yf;
  HPEN        hPen;
  HPEN        hOldPen;
  HDC         display;

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
   display = TtPrinterDC;
#else /* _WIN_PRINT */
   display = TtDisplay;
#endif /* _WIN_PRINT */
   hOldPen = SelectObject (display, hPen);
   Polyline (display, point, 3);
   SelectObject (display, hOldPen);
   DeleteObject (hPen);
}

/*----------------------------------------------------------------------
  DrawRectangleFrame draw a rectangle with smoothed corners (3mm radius)
  and with an horizontal line at 6mm from top.
  Parameters fg, bg, and pattern are for drawing
  color, background color and fill pattern.
  ----------------------------------------------------------------------*/
void DrawRectangleFrame (int frame, int thick, int style, int x, int y,
			 int width, int height, int fg, int bg, int pattern)
{
}

/*----------------------------------------------------------------------
  DrawEllipsFrame draw an ellipse at 7mm under the top of the
  enclosing box.
  Parameters fg, bg, and pattern are for drawing
  color, background color and fill pattern.
  ----------------------------------------------------------------------*/
void DrawEllipsFrame (int frame, int thick, int style, int x, int y,
		      int width, int height, int fg, int bg, int pattern)
{
}

/*----------------------------------------------------------------------
  StorePageInfo and psBoundingBox are empty, they have no meaning in
  this context and are kept for interface compatibility.
  ----------------------------------------------------------------------*/
void StorePageInfo (int pagenum, int width, int height)
{
#ifdef _WIN_PRINT
   LastPageNumber = pagenum;
   LastPageWidth = width;
   LastPageHeight = height;
#endif /* WIN_PRINT */
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void psBoundingBox (int frame, int width, int height)
{
}

/*----------------------------------------------------------------------
  PaintWithPattern fill the rectangle associated to a window w (or frame if w= 0)
  located on (x , y) and geometry width x height, using the
  given pattern.
  Parameters fg, bg, and pattern are for drawing
  color, background color and fill pattern.
  ----------------------------------------------------------------------*/
void PaintWithPattern (int frame, int x, int y, int width, int height,
		       ThotWindow w, int fg, int bg, int pattern)
{
}
