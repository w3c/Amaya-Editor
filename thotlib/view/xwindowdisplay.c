/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2007
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 
/*
 * xwindowdisplay.c : handling of low level drawing routines, both for
 *                    X-Window.
 *
 * Author:  I. Vatton (INRIA)
 *
 */
#ifndef _GL

#include "ustring.h"
#include "math.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "frame.h"

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "boxes_tv.h"
#include "font_tv.h"
#include "frame_tv.h"
#include "units_tv.h"
#include "edit_tv.h"
#include "thotcolor_tv.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#define M_PI_DOUBLE (6.2831853718027492)

/* ((A)*(M_PI/180.0)) */
#define DEG_TO_RAD(A)   ((float)A)/57.29577957795135
#define RAD_TO_DEG(A)   ((float)A)*57.29577957795135

/*If we should use a static table instead for
  performance bottleneck...*/
#define DCOS(A) ((float)cos (A))
#define DSIN(A) ((float)sin (A))
#define DACOS(A) ((float)acos (A))
#define A_DEGREE 0.017453293

#define ALLOC_POINTS    300

#include "boxlocate_f.h"
#include "buildlines_f.h"
#include "context_f.h"
#include "font_f.h"
#include "inites_f.h"
#include "memory_f.h"
#include "spline_f.h"
#include "units_f.h"
#include "xwindowdisplay_f.h"

#ifdef _GTK
#include <gdk/gdkx.h>
#include <gtk/gtkprivate.h>

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
ThotBool is_gtk_font_2byte (ThotFont font)
{
  XFontStruct *xfs;
  
  xfs = (XFontStruct *)GDK_FONT_XFONT(font);
  if (xfs->min_byte1 != 0 || 
      xfs->max_byte1 != 0)
    {
#ifdef _PCLDEBUGFONT
      g_print ("This is a 2-byte font and may not be displayed correctly.");
#endif /*_PCLDEBUGFONT*/
      return TRUE;      
    }
  return FALSE;  
}
#endif /*_GTK*/

/*----------------------------------------------------------------------
  FontOrig update and (x, y) location before DrawString
  accordingly to the ascent of the font used.
  ----------------------------------------------------------------------*/
void FontOrig (ThotFont font, char firstchar, int *pX, int *pY)
{
#ifdef _GTK
  if (!font)
    return;
  *pY += ((XFontStruct *) font)->ascent;
#endif /* _GTK */
}


/*----------------------------------------------------------------------
  LoadColor load the given color in the drawing Graphic Context.
  The parameter fg gives the drawing color
  ----------------------------------------------------------------------*/
static void LoadColor (int fg)
{
#ifdef _GTK
  /* Color of the box */
  gdk_rgb_gc_set_foreground (TtLineGC, ColorPixel (fg));
#endif /* _GTK */
}


/*----------------------------------------------------------------------
  InitDrawing update the Graphic Context accordingly to parameters.
  The parameter fg indicates the drawing color
  ----------------------------------------------------------------------*/
static void InitDrawing (int style, int thick, int fg)
{
  char              dash[2];
  int               i;

  if (style >= 5)
    {
      /* solid */
#ifdef _GTK
      gdk_gc_set_line_attributes (TtLineGC, thick, GDK_LINE_SOLID,
                                  GDK_CAP_BUTT, GDK_JOIN_MITER);
#endif /* _GTK */
    }
  else
    {
      if (style == 3)
        {
          /* dotted */
          if (thick == 1)
            i = 2;
          else
            i = 1;
          dash[0] = i * thick;
          dash[1] = 1 * thick;
        }
      else
        {
        /* dashed */
          dash[0] = 5 * thick;
          dash[1] = 2 * thick;
        }
#ifdef _GTK
      gdk_gc_set_dashes ( TtLineGC, 0, (gint8*)dash, 2); 
      gdk_gc_set_line_attributes (TtLineGC, thick, GDK_LINE_ON_OFF_DASH,
                                  GDK_CAP_BUTT, GDK_JOIN_MITER);

#endif /* _GTK */
    }
  /* Load the correct color */
  LoadColor (fg);
}

/*----------------------------------------------------------------------
  DoDrawOneLine draw one line starting from (x1, y1) to (x2, y2) in frame.
  ----------------------------------------------------------------------*/
static void DoDrawOneLine (int frame, int x1, int y1, int x2, int y2)
{
#ifdef _GTK
  gdk_draw_line (FrRef[frame], TtLineGC, x1, y1, x2, y2);
#endif /* _GTK */
}


/*----------------------------------------------------------------------
  DrawChar draw a char at location (x, y) in frame and with font.
  The parameter fg indicates the drawing color
  ----------------------------------------------------------------------*/
void DrawChar (char car, int frame, int x, int y, ThotFont font, int fg)
{
#ifndef _WX
  ThotWindow          w;
  w = FrRef[frame];
  if (w == None)
    return;

  LoadColor (fg);
  y = y + FrameTable[frame].FrTopMargin;
#ifdef _GTK
  gdk_draw_text (w, font, TtLineGC, x, y, &car, 1);
#endif /* _GTK */
#endif /* _WX */
}

#ifdef XFTGTK
/*----------------------------------------------------------------------
  DrawAAText : Using a Antialiased library if present, 
  named GDKXFT.so, it draws antialiased text.
  ----------------------------------------------------------------------*/
int DrawAAText (int frame, ThotFont font, const gchar *text,
                gint text_length, int x,int y)
{
  GdkWindowPrivate *drawable_private;
  GdkFontPrivate   *font_private;
  GdkGCPrivate      *gc_private;
  XFontStruct       *xfont;

  drawable_private = (GdkWindowPrivate*) FrRef[frame];
  if (drawable_private->destroyed)
    return;

  gc_private = (GdkGCPrivate*) TtLineGC;
  font_private = (GdkFontPrivate*) font;

  if (font->type == GDK_FONT_FONT)
    {
      xfont = (XFontStruct *) font_private->xfont;

      // gdk does this... we don't need it..
      //    XSetFont(drawable_private->xdisplay, gc_private->xgc, xfont->fid);

      if ((xfont->min_byte1 == 0) && (xfont->max_byte1 == 0))
        XDrawString (drawable_private->xdisplay, drawable_private->xwindow,
                     gc_private->xgc, x, y, text, text_length);
      else
        XDrawString16 (drawable_private->xdisplay, drawable_private->xwindow,
                       gc_private->xgc, x, y, (XChar2b *) text, text_length / 2);
    }
  else if (font->type == GDK_FONT_FONTSET)
    {
      XFontSet fontset = (XFontSet) font_private->xfont;
      XmbDrawString (drawable_private->xdisplay, drawable_private->xwindow,
                     fontset, gc_private->xgc, x, y, text, text_length);
    }
  else
    g_error("undefined font type\n");
  /*gdk_draw_text(drawable, font, gc, x, y, text, text_length);*/
}
#endif /*_GTK*/

/*----------------------------------------------------------------------
  DrawString draw a char string of lg chars beginning in buff.
  Drawing starts at (x, y) in frame and using font.
  boxWidth gives the width of the final box or zero,
  this is used only by the thot formmating engine.
  bl indicates that there are one or more spaces before the string
  hyphen indicates whether an hyphen char has to be added.
  startABlock is 1 if the text is at a paragraph beginning
  (no justification of first spaces).
  parameter fg indicates the drawing color
  Returns the lenght of the string drawn.
  ----------------------------------------------------------------------*/
int DrawString (unsigned char *buff, int lg, int frame, int x, int y,
                ThotFont font, int boxWidth, int bl, int hyphen,
                int startABlock, int fg)
{
#ifndef _WX
  ThotWindow          w;
  int                 width;
  register int        j;

  w = FrRef[frame];
  y += FrameTable[frame].FrTopMargin;
  /* compute the width of the string */
  width = 0;
  if (lg > 0 && w)
    {
      /* Dealing with BR tag for windows */
      buff[lg] = EOS;
      j = 0;
      while (j < lg)
        width += CharacterWidth (buff[j++], font);
      if (fg >= 0)
        { 
          LoadColor (fg);
#ifdef _GTK
#ifdef XFTGTK
          DrawAAText(frame, font, buff, lg, x, y);
#else
          if (is_gtk_font_2byte (font))
            gdk_draw_text_wc (w, font,TtLineGC, x, y, 
                              (GdkWChar *)buff, lg * 2);
          else
            gdk_draw_string (w, font,TtLineGC, x, y, (char *)buff);
#endif
          if (hyphen)
            /* draw the hyphen */
            gdk_draw_string (w, font,TtLineGC, x + width, y, "\255");
#endif /* _GTK */
        }
    }
  return (width);
#else /* _WX */
  return 0;
#endif /* _WX */
}


/*----------------------------------------------------------------------
  WDrawString draw a char string of lg chars beginning in buff.
  Drawing starts at (x, y) in frame and using font.
  boxWidth gives the width of the final box or zero,
  this is used only by the thot formmating engine.
  bl indicates that there are one or more spaces before the string
  hyphen indicates whether an hyphen char has to be added.
  startABlock is 1 if the text is at a paragraph beginning
  (no justification of first spaces).
  parameter fg indicates the drawing color
  Returns the lenght of the string drawn.
  ----------------------------------------------------------------------*/
int WDrawString (wchar_t *buff, int lg, int frame, int x, int y,
                 ThotFont font, int boxWidth, int bl, int hyphen,
                 int startABlock, int fg)
{
#ifndef _WX
  ThotWindow          w;
  int                 width, j;
  
  w = FrRef[frame];
  y += FrameTable[frame].FrTopMargin;
  /* compute the width of the string */
  width = 0;
  if (lg > 0 && w)
    {
      /* Dealing with BR tag for windows */
      buff[lg] = EOS;
      j = 0;
      while (j < lg)
        width += CharacterWidth (buff[j++], font);
      if (fg >= 0)
        { 
          LoadColor (fg);
#ifdef _GTK
#ifdef XFTGTK
          DrawAAText(frame, font, buff, lg*2, x, y);
#else
          gdk_draw_text_wc (w, font,TtLineGC, x, y, 
                            (GdkWChar *)buff, lg * 2);
#endif
          if (hyphen)
            /* draw the hyphen */
            gdk_draw_string (w, font,TtLineGC, x + width, y, "\255");
#endif /* _GTK */
        }
    }
  return (width);
#else /* _WX */
  return 0;
#endif /* _WX */
}

/*----------------------------------------------------------------------
  DisplayUnderline draw the underline, overline or cross line
  added to some text of lenght lg, using font and located
  at (x, y) in frame. 
  The parameter fg indicates the drawing color ad type indicates the kind
  of drawing:
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
void DisplayUnderline (int frame, int x, int y, int h, int type,
                       int lg, int fg)
{
#ifndef _WX
  ThotWindow          w;
  int                 bottom;	/* underline position    */
  int                 middle;	/* cross-over position   */
  int                 thickness;	/* thickness of drawing */

  if (fg < 0)
    return;
  if (lg > 0)
    {
      w = FrRef[frame];
      if (w == None)
        return;
      thickness = (h / 20) + 1;
      y += FrameTable[frame].FrTopMargin;
      bottom = y + h - thickness;
      middle = y + h / 2;

      /*
       * for an underline independant of the font add
       * the following lines here :
       *         thickness = 1;
       *         top = y + 2 * thickness;
       *         bottom = y + ascent + 3;
       */
      InitDrawing (5, thickness, fg);
      switch (type)
        {
        case 1:
          /* underlined */
          DoDrawOneLine (frame, x - lg, bottom, x, bottom);
          break;
	    
        case 2:
          /* overlined */
          DoDrawOneLine (frame, x - lg, y, x, y);
          break;
	    
        case 3:
          /* cross-over */
          DoDrawOneLine (frame, x - lg, middle, x, middle);
          break;
	    
        default:
          /* not underlined */
          break;
        }
    }
#else /* _WX */
#endif /* _WX */
}

/*----------------------------------------------------------------------
  DrawPoints draw a line of dot.
  The parameter fg indicates the drawing color
  ----------------------------------------------------------------------*/
void DrawPoints (int frame, int x, int y, int boxWidth, int fg)
{
#ifndef _WX
  ThotWindow          w;
  ThotFont            font;
  SpecFont            spec;
  char               *ptcar;
  int                 xcour, width, nb;

  spec = ThotLoadFont ('L', 1, 0, 6, UnPoint, frame);
  GetFontAndIndexFromSpec (SPACE, spec, 1, &font);
  if (boxWidth > 0)
    {
      w = FrRef[frame];
      ptcar = " .";

      /* compute lenght of the string " ." */
      width = CharacterWidth (SPACE, font) + CharacterWidth (46, font);

      /* compute the number of string to write */
      nb = boxWidth / width;
      xcour = x + (boxWidth % width);
      y = y + FrameTable[frame].FrTopMargin;
      LoadColor (fg);
      /* draw the points */
      FontOrig (font, *ptcar, &x, &y);
      while (nb > 0)
        {
#ifdef _GTK
          gdk_draw_string (w,font, TtLineGC, xcour, y, ptcar);
#endif /* _GTK */
          xcour += width;
          nb--;
        }
    }
#else /* _WX */

#endif /* _WX */
}

/*----------------------------------------------------------------------
  DrawRadical Draw a radical symbol.
  The parameter fg indicates the drawing color
  ----------------------------------------------------------------------*/
void DrawRadical (int frame, int thick, int x, int y, int l, int h,
                  ThotFont font, int fg)
{
  int                 xm, xp, fh;

  if (fg < 0)
    return;
  fh = FontHeight (font);
  xm = x + (fh / 2);
  xp = x + (fh / 4);
  y += FrameTable[frame].FrTopMargin;
  InitDrawing (5, 0, fg);
  /* vertical part */
  DoDrawOneLine (frame, x, y + (2 * (h / 3)), xp - (thick / 2), y + h);

  InitDrawing (5, thick, fg);
  /* Acending part */
  DoDrawOneLine (frame, xp, y + h, xm, y);
  /* Upper part */
  DoDrawOneLine (frame, xm, y, x + l, y);
}

/*----------------------------------------------------------------------
  DrawIntegral draw an integral. depending on type :
  - simple if type = 0
  - contour if type = 1
  - double if type = 2.
  parameter fg indicates the drawing color
  ----------------------------------------------------------------------*/
void DrawIntegral (int frame, int thick, int x, int y, int l, int h,
                   int type, ThotFont font, int fg)
{
  int                 yf;
  int                 yend, delta;
  int                 wd, asc, hd;

  if (FontHeight (font) *1.2 >= h)
    /* display a single glyph */
    {
      yf = y + ((h - CharacterHeight (242, font)) / 2) + CharacterAscent (242, font);
      DrawChar ('\362', frame, x, yf, font, fg);
    }
  else
    {
      /* Need more than one glyph */
      yf = y + CharacterAscent (243, font);
      DrawChar ('\363', frame, x, yf, font, fg);
      yend = y + h - CharacterHeight (245, font) 
        + CharacterAscent (245, font) - 1;
      DrawChar ('\365', frame, x, yend, font, fg);
      asc = CharacterAscent (244, font);
      hd = CharacterHeight (244, font);
      delta = yend - yf - asc;
      yf += asc;
      wd = (CharacterWidth (243, font) - CharacterWidth (244, font)) / 2;
      if (delta > 0 && hd > 0)
        {
          while (yf < yend)
            {
              DrawChar ('\364', frame, x + wd, yf, font, fg);
              yf += hd;
            }
        }
    }
  if (type == 2)		/* double integral */
    DrawIntegral (frame, thick, x + (CharacterWidth (243, font) / 2),
                  y, l, h, -1, font, fg);

  else if (type == 1)		/* contour integral */
    DrawChar ('o', frame, x + ((l - CharacterWidth (111, font)) / 2),
              y + (h - CharacterHeight (111, font)) / 2 + CharacterAscent (111, font),
              font, fg);
}

/*----------------------------------------------------------------------
  DrawMonoSymb draw a one glyph symbol.
  parameter fg indicates the drawing color
  ----------------------------------------------------------------------*/
static void DrawMonoSymb (CHAR_T symb, int frame, int x, int y, int l,
                          int h, ThotFont font, int fg)
{
  int                 xm, yf;

  xm = x + ((l - CharacterWidth (symb, font)) / 2);
  yf = y + ((h - CharacterHeight (symb, font)) / 2) + CharacterAscent (symb, font);

  DrawChar ((char)symb, frame, xm, yf, font, fg);
}

/*----------------------------------------------------------------------
  DrawSigma draw a Sigma symbol.
  active indicates if the box is active
  parameter fg indicates the drawing color
  ----------------------------------------------------------------------*/
void DrawSigma (int frame, int x, int y, int l, int h, ThotFont font, int fg)
{
  int                 xm, ym, fh;

  if (fg < 0)
    return;
  fh = FontHeight (font);
  if (h < fh * 2 && l <= CharacterWidth (229, font))
    {
      /* Only one glyph needed */
      DrawMonoSymb ('\345', frame, x, y, l, h, font, fg);
    }
  else
    {
      y += FrameTable[frame].FrTopMargin;
      xm = x + (l / 3);
      ym = y + (h / 2) - 1;
      InitDrawing (5, 0, fg);
      /* Center */
      DoDrawOneLine (frame, x, y + 1, xm, ym);
      DoDrawOneLine (frame, x, y + h - 2, xm, ym);

      InitDrawing (5, 2, fg);
      /* Borders */
      DoDrawOneLine (frame, x, y, x + l, y);
      DoDrawOneLine (frame, x, y + h - 2, x + l, y + h - 2);
    }
}

/*----------------------------------------------------------------------
  DrawPi draw a PI symbol.
  parameter fg indicates the drawing color
  ----------------------------------------------------------------------*/
void DrawPi (int frame, int x, int y, int l, int h, ThotFont font, int fg)
{
  int                 fh;

  if (fg < 0)
    return;
  fh = FontHeight (font);
  if (h < fh * 2 && l <= CharacterWidth (213, font))
    {
      /* Only one glyph needed */
      DrawMonoSymb ('\325', frame, x, y, l, h, font, fg);
    }
  else
    {
      y += FrameTable[frame].FrTopMargin;
      InitDrawing (5, 0, fg);
      /* Vertical part */
      DoDrawOneLine (frame, x + 2, y + 1, x + 2, y + h);
      DoDrawOneLine (frame, x + l - 3, y + 1, x + l - 3, y + h);

      InitDrawing (5, 2, fg);
      /* Upper part */
      DoDrawOneLine (frame, x + 1, y + 1, x + l, y);
    }
}

/*----------------------------------------------------------------------
  DrawIntersection draw an intersection symbol.
  parameter fg indicates the drawing color
  ----------------------------------------------------------------------*/
void DrawIntersection (int frame, int x, int y, int l, int h, ThotFont font,
                       int fg)
{
  int                 arc, fh;

  if (fg < 0)
    return;
  fh = FontHeight (font);
  if (h < fh * 2 && l <= CharacterWidth (199, font))
    {
      /* Only one glyph needed */
      DrawMonoSymb ('\307', frame, x, y, l, h, font, fg);
    }
  else
    {
      y += FrameTable[frame].FrTopMargin;
      /* radius of arcs is 6mm */
      arc = h / 4;
      InitDrawing (5, 2, fg);
      /* vertical part */
      DoDrawOneLine (frame, x + 1, y + arc, x + 1, y + h);
      DoDrawOneLine (frame, x + l - 2, y + arc, x + l - 2, y + h);

      /* Upper part */
#ifdef _GTK
      gdk_draw_arc (FrRef[frame], TtLineGC,FALSE, x + 1, y + 1,
                    l - 3, arc * 2, 0 * 64, 180 * 64);
#endif /* _GTK */
    }
}

/*----------------------------------------------------------------------
  DrawUnion draw an Union symbol.
  parameter fg indicates the drawing color
  ----------------------------------------------------------------------*/
void DrawUnion (int frame, int x, int y, int l, int h, ThotFont font, int fg)
{
  int                 arc, fh;

  if (fg < 0)
    return;
  fh = FontHeight (font);
  if (h < fh * 2 && l <= CharacterWidth (200, font))
    {
      /* Only one glyph needed */
      DrawMonoSymb ('\310', frame, x, y, l, h, font, fg);
    }
  else
    {
      y += FrameTable[frame].FrTopMargin;
      /* radius of arcs is 3mm */
      arc = h / 4;
      InitDrawing (5, 2, fg);
      /* two vertical lines */
      DoDrawOneLine (frame, x + 1, y, x + 1, y + h - arc);
      DoDrawOneLine (frame, x + l - 2, y, x + l - 2, y + h - arc);

      /* Lower part */
#ifdef _GTK
      gdk_draw_arc (FrRef[frame], TtLineGC,FALSE,
                    x + 1, y + h - arc * 2 - 2,
                    l - 3, arc * 2,
                    -0 * 64, -180 * 64);

#endif /* _GTK */
    }
}

/*----------------------------------------------------------------------
  ArrowDrawing draw the end of an arrow.
  ----------------------------------------------------------------------*/
static void ArrowDrawing (int frame, int x1, int y1, int x2, int y2,
                          int thick, int fg)
{
  double              x, y, xb, yb, dx, dy, l, sina, cosa;
  int                 xc, yc, xd, yd;
  double              width, height;
  ThotPixmap          pattern;
  ThotPoint           point[3];

  width = (double) (5 + thick);
  height = 10;
  dx = (double) (x2 - x1);
  dy = (double) (y1 - y2);
  l = (double) sqrt ((double) (dx * dx + dy * dy));
  if (l == 0) 
    return;
  sina = dy / l;
  cosa = dx / l;
  xb = x2 * cosa - y2 * sina;
  yb = x2 * sina + y2 * cosa;
  x = xb - height;
  y = yb - width / 2;
  xc = (int)(x * cosa + y * sina + .5);
  yc = (int)(-x * sina + y * cosa + .5);
  y = yb + width / 2;
  xd = (int)(x * cosa + y * sina + .5);
  yd = (int)(-x * sina + y * cosa + .5);

  /* draw */
  point[0].x = x2;
  point[0].y = y2;
  point[1].x = xc;
  point[1].y = yc;
  point[2].x = xd;
  point[2].y = yd;

  pattern = (ThotPixmap) CreatePattern (0, fg, fg, 1);
  if (pattern != 0)
    {
#ifdef _GTK
      gdk_gc_set_tile (TtGreyGC, (GdkPixmap *)pattern);
      gdk_draw_polygon (FrRef[frame], TtGreyGC, TRUE, point, 3);
      gdk_pixmap_unref ((GdkPixmap *)pattern);
#endif /* _GTK */
    }
}

/*----------------------------------------------------------------------
  DrawArrow draw an arrow.
  orientation in degrees : 0 (right arrow), 45, 90, 135, 180, 225, 270 ou 315.
  type : 0 = Arrow, 1 = Arrow with opposite directions, 2 = DoubleArrow,
         3 = DoubleArrow with opposite directions
         4 = two arrows with opposite directions, 5 = TeeArrow, 6 = ArrowBar
         7 = Vector,  8 = Vector with opposite directions, 9 = TeeVector,
         10 = VectorBar, 
         11 = two vectors with opposite directions = Equilibrium
         12 = ReverseVector, 13 = ReverseVector with opposite directions,
         14 = TeeReverseVector, 15 = ReverseVectorBar
         
  fg : drawing color
  ----------------------------------------------------------------------*/
void DrawArrow (int frame, int thick, int style, int x, int y, int l, int h,
                int orientation, int type, int fg)
{
  int                 xm, ym, xf, yf, D1, D2;

  if (fg < 0 || thick <= 0)return;
  InitDrawing (style, thick, fg);

  y += FrameTable[frame].FrTopMargin;
  xm = x + ((l - thick) / 2);
  xf = x + l - 1;
  ym = y + ((h - thick) / 2);
  yf = y + h - 1;

  D1 = thick + 5;

  /* Vector or ReverseVector */
  if(type >= 12 && type <= 15)D2 = -D1; else D2 = D1;

  /* Draw a Tee (type = 5, 9, 14) or a Bar (type = 6, 10, 15)  */
  if(type == 5 || type == 6 || type == 9 || type == 10 || type == 14 || type == 15)
    {
    switch(orientation)
      {
      case 0:
        if(type == 5 || type == 9 || type == 14)DoDrawOneLine (frame, x, ym - D1, x, ym + D1);
        else DoDrawOneLine (frame, xf, ym - D1, xf, ym + D1);
      break;
      case 90:
        if(type == 5 || type == 9 || type == 14)DoDrawOneLine (frame, xm - D1, yf, xm + D1, yf);
        else DoDrawOneLine (frame, xm - D1, y, xm + D1, y);
      break;
      case 180:
        if(type == 5 || type == 9 || type == 14)DoDrawOneLine (frame, xf, ym - D1, xf, ym + D1);
        else DoDrawOneLine (frame, x, ym - D1, x, ym + D1);
      break;
      case 270:
        if(type == 5 || type == 9 || type == 14)DoDrawOneLine (frame, xm - D1, y, xm + D1, y);
        else DoDrawOneLine (frame, xm - D1, yf, xm + D1, yf);
      break;
      default:
      break;
      }
    }

  switch(type)
    {
    case 0: /* Arrow */
    case 5: /* TeeArrow */
    case 6: /* ArrowBar */
      switch(orientation)
        {
        case 0:
          /* draw a right arrow */
          DoDrawOneLine (frame, x, ym, xf, ym);
          ArrowDrawing (frame, x, ym, xf, ym, thick, fg);
        break;
        case 45:
          DoDrawOneLine (frame, x, yf, xf - thick + 1, y);
          ArrowDrawing (frame, x, yf, xf - thick + 1, y, thick, fg);
        break;
        case 90:
          /* draw a bottom-up arrow */
          DoDrawOneLine (frame, xm, y, xm, yf);
          ArrowDrawing (frame, xm, yf, xm, y, thick, fg);
        break;
        case  135:
          DoDrawOneLine (frame, x, y, xf - thick + 1, yf);
          ArrowDrawing (frame, xf - thick + 1, yf, x, y, thick, fg);
        break;
        case 180:
          /* draw a left arrow */
          DoDrawOneLine (frame, x, ym, xf, ym);
          ArrowDrawing (frame, xf, ym, x, ym, thick, fg);
        break;
        case 225:
          DoDrawOneLine (frame, x, yf, xf - thick + 1, y);
          ArrowDrawing (frame, xf - thick + 1, y, x, yf, thick, fg);
        break;
        case 270:
          /* draw a top-down arrow */
          DoDrawOneLine (frame, xm, y, xm, yf);
          ArrowDrawing (frame, xm, y, xm, yf, thick, fg);
        break;
        case 315:
          DoDrawOneLine (frame, x, y, xf - thick + 1, yf);
          ArrowDrawing (frame, x, y, xf - thick + 1, yf, thick, fg);
        break;
        default:
        break;
        }
    break;

    case 1: /* Arrow with opposite directions */
      switch(orientation)
        {
        case 0:
          DoDrawOneLine (frame, x, ym, xf, ym);
          ArrowDrawing (frame, x, ym, xf, ym, thick, fg);
          ArrowDrawing (frame, xf, ym, x, ym, thick, fg);
        break;

        case 90:
          DoDrawOneLine (frame, xm, y, xm, yf);
          ArrowDrawing (frame, xm, yf, xm, y, thick, fg);
          ArrowDrawing (frame, xm, y, xm, yf, thick, fg);
        break;

        default:
        break;
        }
    break;

    case 2: /* DoubleArrow */
      switch(orientation)
        {
        case 0:
          DoDrawOneLine (frame, x, ym - D1/2, xf - D1/2, ym - D1/2);
          DoDrawOneLine (frame, x, ym + D1/2, xf - D1/2, ym + D1/2);
          DoDrawOneLine (frame, xf - D1, ym - D1, xf, ym);
          DoDrawOneLine (frame, xf - D1, ym + D1, xf, ym);
        break;
        case 90:
          DoDrawOneLine (frame, xm + D1/2, y + D1/2, xm + D1/2, yf);
          DoDrawOneLine (frame, xm - D1/2, y + D1/2, xm - D1/2, yf);
          DoDrawOneLine (frame, xm, y, xm - D1, y + D1);
          DoDrawOneLine (frame, xm, y, xm + D1, y + D1);
        break;
        case 180:
          DoDrawOneLine (frame, x + D1/2, ym + D1/2, xf, ym + D1/2);
          DoDrawOneLine (frame, x + D1/2, ym - D1/2, xf, ym - D1/2);
          DoDrawOneLine (frame, x, ym, x + D1, ym - D1);
          DoDrawOneLine (frame, x, ym, x + D1, ym + D1);
        break;
        case 270:
          DoDrawOneLine (frame, xm + D1/2, y, xm + D1/2, yf - D1/2);
          DoDrawOneLine (frame, xm - D1/2, y, xm - D1/2, yf - D1/2);
          DoDrawOneLine (frame, xm, yf, xm - D1, yf - D1);
          DoDrawOneLine (frame, xm, yf, xm + D1, yf - D1);
        break;

        default:
        break;
        }
    break;

    case 3: /* DoubleArrow with opposite directions */
      switch(orientation)
        {
        case 0:
          DoDrawOneLine (frame, x + D1/2, ym - D1/2, xf - D1/2, ym - D1/2);
          DoDrawOneLine (frame, x + D1/2, ym + D1/2, xf - D1/2, ym + D1/2);
          DoDrawOneLine (frame, xf - D1, ym - D1, xf, ym);
          DoDrawOneLine (frame, xf - D1, ym + D1, xf, ym);
          DoDrawOneLine (frame, x, ym, x + D1, ym - D1);
          DoDrawOneLine (frame, x, ym, x + D1, ym + D1);
        break;
        case 90:
          DoDrawOneLine (frame, xm + D1/2, y + D1/2, xm + D1/2, yf - D1/2);
          DoDrawOneLine (frame, xm - D1/2, y + D1/2, xm - D1/2, yf - D1/2);
          DoDrawOneLine (frame, xm, y, xm - D1, y + D1);
          DoDrawOneLine (frame, xm, y, xm + D1, y + D1);
          DoDrawOneLine (frame, xm, yf, xm - D1, yf - D1);
          DoDrawOneLine (frame, xm, yf, xm + D1, yf - D1);
        break;

        default:
        break;
        }
    break;

    case 4: /* two arrows with opposite directions */
      switch(orientation)
        {
        case 0:
          DoDrawOneLine (frame, x, ym - D1/2, xf, ym - D1/2);
          ArrowDrawing (frame, x, ym - D1/2, xf, ym - D1/2, thick, fg);
          DoDrawOneLine (frame, x, ym + D1/2, xf, ym + D1/2);
          ArrowDrawing (frame, xf, ym + D1/2, x, ym + D1/2, thick, fg);
        break;
        case 90:
          DoDrawOneLine (frame, xm - D1/2, y, xm - D1/2, yf);
          ArrowDrawing (frame, xm - D1/2, yf, xm - D1/2, y, thick, fg);
          DoDrawOneLine (frame, xm + D1/2, y, xm + D1/2, yf);
          ArrowDrawing (frame, xm + D1/2, y, xm + D1/2, yf, thick, fg);
        break;
        case 180:
          DoDrawOneLine (frame, x, ym + D1/2, xf, ym + D1/2);
          ArrowDrawing (frame, x, ym + D1/2, xf, ym + D1/2, thick, fg);
          DoDrawOneLine (frame, x, ym - D1/2, xf, ym - D1/2);
          ArrowDrawing (frame, xf, ym - D1/2, x, ym - D1/2, thick, fg);
        break;
        case 270:
          DoDrawOneLine (frame, xm + D1/2, y, xm + D1/2, yf);
          ArrowDrawing (frame, xm + D1/2, yf, xm + D1/2, y, thick, fg);
          DoDrawOneLine (frame, xm - D1/2, y, xm - D1/2, yf);
          ArrowDrawing (frame, xm - D1/2, y, xm - D1/2, yf, thick, fg);
        break;
        default:
        break;
        }
    break;


    case 7: /* Vector */
    case 9: /* TeeVector */
    case 10: /* VectorBar */
    case 12: /* ReverseVector */
    case 14: /* TeeReverseVector*/ 
    case 15: /* ReverseVectorBar */
      switch(orientation)
        {
        case 0:
          DoDrawOneLine (frame, x, ym, xf, ym);
          DoDrawOneLine (frame, xf - D1, ym - D2, xf, ym);
        break;
        case 90:
          DoDrawOneLine (frame, xm, y, xm, yf);
          DoDrawOneLine (frame, xm, y, xm - D2, y + D1);
        break;
        case 180:
          DoDrawOneLine (frame, x, ym, xf, ym);
          DoDrawOneLine (frame, x, ym, x + D1, ym - D2);
        break;
        case 270:
          DoDrawOneLine (frame, xm, y, xm, yf);
          DoDrawOneLine (frame, xm, yf, xm - D2, yf - D1);
        break;
        default:
        break;
        }
    break;

    case 8: /* Vector with opposite directions */
    case 13: /* ReverseVector with opposite directions */
      switch(orientation)
        {
        case 0:
          DoDrawOneLine (frame, x, ym, xf, ym);
          DoDrawOneLine (frame, x, ym, xf, ym);
          DoDrawOneLine (frame, x + D1, ym - D2, x, ym);
          DoDrawOneLine (frame, xf - D1, ym - D2, xf, ym);
        break;
        case 90:
          DoDrawOneLine (frame, xm, y, xm, yf);
          DoDrawOneLine (frame, xm, y, xm, yf);
          DoDrawOneLine (frame, xm, y, xm - D2, y + D1);
          DoDrawOneLine (frame, xm, yf, xm - D2, yf - D1);
        break;

        default:
        break;
        }
    break;

    case 11: /* two vectors with opposite directions */
      switch(orientation)
        {
        case 0:
          DoDrawOneLine (frame, x, ym - D1/2, xf, ym - D1/2);
          DoDrawOneLine (frame, xf, ym - D1/2, xf - D1/2 - D1, ym - D1/2 - D1);
          DoDrawOneLine (frame, x, ym + D1/2, xf, ym + D1/2);
          DoDrawOneLine (frame, x, ym + D1/2, x + D1/2 + D1, ym + D1/2 + D1);
        break;
        case 90:
          DoDrawOneLine (frame, xm - D1/2, y, xm - D1/2, yf);
          DoDrawOneLine (frame, xm - D1/2, y, xm - D1/2 - D2, y + D1);
          DoDrawOneLine (frame, xm + D1/2, y, xm + D1/2, yf);
          DoDrawOneLine (frame, xm + D1/2, yf, xm + D1/2 + D1, yf - D1);
        break;
        case 180:
          DoDrawOneLine (frame, x, ym + D1/2, xf, ym + D1/2);
          DoDrawOneLine (frame, xf, ym + D1/2, xf - D1/2 - D1, ym + D1/2 + D1);
          DoDrawOneLine (frame, x, ym - D1/2, xf, ym - D1/2);
          DoDrawOneLine (frame, x, ym - D1/2, x + D1/2 + D1, ym - D1/2 - D1);
        break;
        case 270:
          DoDrawOneLine (frame, xm + D1/2, y, xm + D1/2, yf);
          DoDrawOneLine (frame, xm + D1/2, y, xm + D1/2 + D1, y + D1);
          DoDrawOneLine (frame, xm - D1/2, y, xm - D1/2, yf);
          DoDrawOneLine (frame, xm - D1/2, yf, xm - D1/2 - D1, yf - D1);
        break;
        default:
        break;
        }
    break;

    default:
    break;
    }
}

/*----------------------------------------------------------------------
  DrawBracket draw an opening or closing bracket (depending on direction)
  parameter fg indicates the drawing color
  ----------------------------------------------------------------------*/
void DrawBracket (int frame, int thick, int x, int y, int l, int h,
                  int direction, ThotFont font, int fg, int baseline)
{
  int                 xm, yf, yend;

  if (h <= (int) (1.3 * FontHeight (font)) )
    {
      /* With only one glyph */
      if (direction == 0)
        {
          /* Draw a opening bracket */
          xm = x + ((l - CharacterWidth (91, font)) / 2);
          if (baseline)
            yf = baseline;
          else
            yf = y + ((h - CharacterHeight (91, font)) / 2) +
              CharacterAscent (91, font);
          DrawChar ('[', frame, xm, yf, font, fg);
        }
      else
        {
          /* Draw a closing bracket */
          xm = x + ((l - CharacterWidth (93, font)) / 2);
          if (baseline)
            yf = baseline;
          else
            yf = y + ((h - CharacterHeight (93, font)) / 2) +
              CharacterAscent (93, font);
          DrawChar (']', frame, xm, yf, font, fg);
        }
    }
  else
    {
      /* Need more than one glyph */
      if (direction == 0)
        {
          /* Draw a opening bracket */
          xm = x + ((l - CharacterWidth (233, font)) / 2);
          yf = y + CharacterAscent (233, font);
          DrawChar ('\351', frame, xm, yf, font, fg);
          yend = y + h - CharacterHeight (235, font) + CharacterAscent (235, font);
          DrawChar ('\353', frame, xm, yend, font, fg);
          for (yf = yf + CharacterHeight (233, font);
               yf < yend;
               yf += CharacterHeight (234, font))
            DrawChar ('\352', frame, xm, yf, font, fg);
        }
      else
        {
          /* Draw a closing bracket */
          xm = x + ((l - CharacterWidth (249, font)) / 2);
          yf = y + CharacterAscent (249, font);
          DrawChar ('\371', frame, xm, yf, font, fg);
          yend = y + h - CharacterHeight (251, font) + CharacterAscent (251, font);
          DrawChar ('\373', frame, xm, yend, font, fg);
          for (yf = yf + CharacterHeight (249, font);
               yf < yend;
               yf += CharacterHeight (250, font))
            DrawChar ('\372', frame, xm, yf, font, fg);
        }
    }
}

/*----------------------------------------------------------------------
  DrawPointyBracket draw an opening or closing pointy bracket (depending
  on direction)
  parameter fg indicates the drawing color
  ----------------------------------------------------------------------*/
void DrawPointyBracket (int frame, int thick, int x, int y, int l, int h,
                        int direction, ThotFont font, int fg)
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
          xm = x + ((l - CharacterWidth (225, font)) / 2);
          yf = y + ((h - CharacterHeight (225, font)) / 2) + CharacterAscent (225, font);
          DrawChar ('\341', frame, xm, yf, font, fg);
        }
      else
        {
          /* Draw a closing bracket */
          xm = x + ((l - CharacterWidth (241, font)) / 2);
          yf = y + ((h - CharacterHeight (241, font)) / 2) + CharacterAscent (241, font);
          DrawChar ('\361', frame, xm, yf, font, fg);
        }
    }
  else
    {
      /* Need more than one glyph */
      y += FrameTable[frame].FrTopMargin;
      InitDrawing (5, 0, fg);
      if (direction == 0)
        {
          /* Draw a opening bracket */
          DoDrawOneLine (frame, x + l, y, x, y + (h / 2));
          DoDrawOneLine (frame, x, y + (h / 2), x + l, y + h);
        }
      else
        {
          /* Draw a closing bracket */
          DoDrawOneLine (frame, x, y, x + l, y + (h / 2));
          DoDrawOneLine (frame, x + l, y + (h / 2), x, y + h);
        }
    }
}

/*----------------------------------------------------------------------
  DrawParenthesis draw a closing or opening parenthesis (direction).
  parameter fg indicates the drawing color
  ----------------------------------------------------------------------*/
void DrawParenthesis (int frame, int thick, int x, int y, int l, int h,
                      int direction, ThotFont font, int fg, int baseline)
{
  int                 xm, yf, yend, delta, asc, hd;

  if (h <= (int) (1.3 * FontHeight (font)) )
    {
      /* With only one glyph */
      if (direction == 0)
        {
          /* draw a opening parenthesis */
          xm = x + ((l - CharacterWidth (40, font)) / 2);
          if (baseline)
            yf = baseline;
          else
            yf = y + ((h - CharacterHeight (40, font)) / 2) +
              CharacterAscent (40, font);
          DrawChar ('(', frame, xm, yf, font, fg);
        }
      else
        {
          /* draw a closing parenthesis */
          xm = x + ((l - CharacterWidth (41, font)) / 2);
          if (baseline)
            yf = baseline;
          else
            yf = y + ((h - CharacterHeight (41, font)) / 2) +
              CharacterAscent (41, font);
          DrawChar (')', frame, xm, yf, font, fg);
        }
    }
  else
    {
      /* Need more than one glyph */
      if (direction == 0)
        {
          /* draw a opening parenthesis */
          xm = x + ((l - CharacterWidth (230, font)) / 2);
          yf = y + CharacterAscent (230, font);
          DrawChar ('\346', frame, xm, yf, font, fg);
          yend = y + h - CharacterHeight (232, font) + CharacterAscent (232, font) - 1;
          DrawChar ('\350', frame, xm, yend, font, fg);
          asc = CharacterAscent (231, font);
          hd = CharacterHeight (231, font);
          delta = yend - yf- hd;
          yf += asc;
          if (delta > 0 && hd > 0)
            {
              while (yf < yend)
                {
                  DrawChar ('\347', frame, xm, yf, font, fg);
                  yf += hd;
                }
            }
        }
      
      else
        {
          /* draw a closing parenthesis */
          xm = x + ((l - CharacterWidth (246, font)) / 2);
          yf = y + CharacterAscent (246, font);
          DrawChar ('\366', frame, xm, yf, font, fg);
          yend = y + h - CharacterHeight (248, font) 
            + CharacterAscent (248, font) - 1;
          DrawChar ('\370', frame, xm, yend, font, fg);
          asc = CharacterAscent (247, font);
          hd = CharacterHeight (247, font);
          delta = yend - yf - hd;
          yf += asc;
          if (delta > 0 && hd > 0)
            {
              while (yf < yend)
                {
                  DrawChar ('\367', frame, xm, yf, font, fg);
                  yf += hd;
                }
            }
        }
    }
}

/*----------------------------------------------------------------------
  DrawBrace draw an opening of closing brace (depending on direction).
  parameter fg indicates the drawing color
  ----------------------------------------------------------------------*/
void DrawBrace (int frame, int thick, int x, int y, int l, int h,
                int direction, ThotFont font, int fg, int baseline)
{
  int                 xm, ym, yf, yend, delta, hd, asc;

  if (h <= (int) (1.3 * FontHeight (font)) )
    {
      /* need only one char */
      if (direction == 0)
        {
          /* just use the opening brace glyph */
          xm = x + ((l - CharacterWidth (123, font)) / 2);
          if (baseline)
            yf = baseline;
          else
            yf = y + ((h - CharacterHeight (123, font)) / 2) +
              CharacterAscent (123, font);
          DrawChar ('{', frame, xm, yf, font, fg);
        }
      else
        {
          /* just use the closing brace glyph */
          xm = x + ((l - CharacterWidth (125, font)) / 2);
          if (baseline)
            yf = baseline;
          else
            yf = y + ((h - CharacterHeight (125, font)) / 2) +
              CharacterAscent (125, font);
          DrawChar ('}', frame, xm, yf, font, fg);
        }
    }
  else
    {
      /* Brace drawn with more than one glyph */
      if (direction == 0)
        {
          /* top */
          xm = x + ((l - CharacterWidth (236, font)) / 2);
          yf = y + CharacterAscent (236, font);
          DrawChar ('\354', frame, xm, yf, font, fg);
          /* vertical line */
          ym = y + ((h - CharacterHeight (237, font)) / 2) + CharacterAscent (237, font);
          DrawChar ('\355', frame, xm, ym, font, fg);
          /* bottom */
          yend = y + h - CharacterHeight (238, font) + CharacterAscent (238, font);
          DrawChar ('\356', frame, xm, yend, font, fg);
          /* finish top */
          asc = CharacterAscent (239, font);
          hd = CharacterHeight (239, font);
          delta = ym - yf - hd;
          yf += asc;
          if (delta > 0 && hd > 0)
            {
              while (yf < yend)
                {
                  DrawChar ('\357', frame, xm, yf, font, fg);
                  yf += hd;
                }
            }
          /* finish bottom */
          yf = ym + CharacterHeight ('\355', font);
          delta = yend - yf - hd;
          yf += asc;
          if (delta > 0 && hd > 0)
            {
              while (yf < yend)
                {
                  DrawChar ('\357', frame, xm, yf, font, fg);
                  yf += hd;
                }
            }
        }
      else
        {
          /* top */
          xm = x + ((l - CharacterWidth (252, font)) / 2);
          yf = y + CharacterAscent (252, font);
          DrawChar ('\374', frame, xm, yf, font, fg);
          /* center */
          ym = y + ((h - CharacterHeight (253, font)) / 2) + CharacterAscent (253, font);
          DrawChar ('\375', frame, xm, ym, font, fg);
          /* bottom */
          yend = y + h - CharacterHeight (254, font) + CharacterAscent (254, font);
          DrawChar ('\376', frame, xm, yend, font, fg);
          /* finish top */
          asc = CharacterAscent (239, font);
          hd = CharacterHeight (239, font);
          delta = ym - yf - hd;
          yf += asc;
          if (delta > 0 && hd > 0)
            {
              while (yf < yend)
                {
                  DrawChar ('\357', frame, xm, yf, font, fg);
                  yf += hd;
                }
            }
          /* finish bottom */
          yf = ym + CharacterHeight ('\375', font);
          delta = yend - yf - hd;
          yf += asc;
          if (delta > 0 && hd > 0)
            {
              while (yf < yend)
                {
                  DrawChar ('\357', frame, xm, yf, font, fg);
                  yf += hd;
                }
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
  ThotPixmap              pat;

  if (width <= 0 || height <= 0)
    return;
  if (thick == 0 && pattern == 0)
    return;

  y += FrameTable[frame].FrTopMargin;
  pat = (ThotPixmap) CreatePattern (0, fg, bg, pattern);
  if (pat != 0)
    {
#ifdef _GTK
      gdk_gc_set_tile (TtGreyGC, (GdkPixmap *)pat);    
      gdk_draw_rectangle (FrRef[frame], TtGreyGC, TRUE, x, y, width, height);
      gdk_pixmap_unref ((GdkPixmap *)pat);
#endif /* _GTK */
    }

  /* Draw the border */
  if (thick > 0 && fg >= 0)
    {
      if (width > thick)
        width = width - thick;
      if (height > thick)
        height = height - thick;
      x = x + thick / 2;
      y = y + thick / 2;

      InitDrawing (style, thick, fg); 
#ifdef _GTK
      gdk_draw_rectangle ( FrRef[frame],TtLineGC,FALSE, x, y, width, height);
#endif /* _GTK */
    }
}

/*----------------------------------------------------------------------
  DrawDiamond draw a diamond.
  Parameters fg, bg, and pattern are for drawing
  color, background color and fill pattern.
  ----------------------------------------------------------------------*/
void DrawDiamond (int frame, int thick, int style, int x, int y, int width,
                  int height, int fg, int bg, int pattern)
{
  ThotPoint           point[5];
  ThotPixmap              pat;

  if (width > thick + 1)
    width = width - thick - 1;
  if (height > thick + 1)
    height = height - thick - 1;
  x += thick / 2;
  y = y + thick / 2 + FrameTable[frame].FrTopMargin;

  point[0].x = x + (width / 2);
  point[0].y = y;
  point[4].x = point[0].x;
  point[4].y = point[0].y;
  point[1].x = x + width;
  point[1].y = y + (height / 2);
  point[2].x = point[0].x;
  point[2].y = y + height;
  point[3].x = x;
  point[3].y = point[1].y;

  /* Fill in the diamond */
  pat = CreatePattern (0, fg, bg, pattern);
  if (pat != 0)
    {
#ifdef _GTK
      gdk_gc_set_tile (TtGreyGC, (GdkPixmap *)pat);
      gdk_draw_polygon (FrRef[frame], TtGreyGC, TRUE, point, 5);
      gdk_pixmap_unref ((GdkPixmap *)pat); 
#endif /* _GTK */
    }

  /* Draw the border */
  if (thick > 0 && fg >= 0)
    {
      InitDrawing (style, thick, fg);
#ifdef _GTK
      gdk_draw_polygon (FrRef[frame], TtLineGC, FALSE, point, 5);
#endif /* _GTK */
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
                   PtrTextBuffer buffer, int nb, int fg, int arrow, int bg,
                   int pattern)
{
#ifdef _GTK
  int                 k;
#endif /* _GTK */
  ThotPoint          *points;
  int                 i, j;
  PtrTextBuffer       adbuff;

  /* fill the included polygon */
  DrawPolygon (frame, 0, style, x, y, buffer, nb, fg, bg, pattern);
  if (thick == 0 || fg < 0)
    return;

  /* Allocate a table of points */
  points = (ThotPoint *) TtaGetMemory (sizeof (ThotPoint) * (nb - 1));
  adbuff = buffer;
  y += FrameTable[frame].FrTopMargin;
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
    ArrowDrawing (frame,
                  points[1].x, points[1].y,
                  points[0].x, points[0].y,
                  thick, fg);
  
  /* Draw the border */
  InitDrawing (style, thick, fg);

#ifdef _GTK
  for (k=0; k< nb-2; k++)
    gdk_draw_line (FrRef[frame], TtLineGC,
                   points[k].x, points[k].y,
                   points[k+1].x, points[k+1].y);
#endif /* _GTK */

  /* Forward arrow */
  if (arrow == 1 || arrow == 3)
    ArrowDrawing (frame,
                  points[nb - 3].x, points[nb - 3].y,
                  points[nb - 2].x, points[nb - 2].y,
                  thick, fg);

  /* free the table of points */
  free (points);
}

/*----------------------------------------------------------------------
  DoDrawLines
  Draw a polygon whose points are stored in buffer points
  Parameters fg, bg, and pattern are for drawing
  color, background color and fill pattern.
  ----------------------------------------------------------------------*/
static void DoDrawLines (int frame, int thick, int style,
                         ThotPoint *points, int npoints, int fg, int bg,
                         int pattern)
{
  ThotPixmap              pat;

  /* Fill in the polygon */
  pat = CreatePattern (0, fg, bg, pattern);
  if (pat != 0) 
    {
#ifdef _GTK
      gdk_gc_set_tile (TtGreyGC, (GdkPixmap *)pat);
      gdk_draw_polygon (FrRef[frame], TtGreyGC, TRUE, points, npoints); 
      gdk_pixmap_unref ((GdkPixmap *)pat);
#endif /* _GTK */
    }

  /* Draw the border */
  if (thick > 0 && fg >= 0)
    {
      InitDrawing (style, thick, fg);
#ifdef _GTK
      gdk_draw_lines (FrRef[frame], TtLineGC, points, npoints); 
#endif /* _GTK */
    }
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
                  PtrTextBuffer buffer, int nb, int fg, int bg, int pattern)
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
  /* Close the polygon */
  points[nb - 1].x = points[0].x;
  points[nb - 1].y = points[0].y;
  DoDrawLines (frame, thick, style, points, nb, fg, bg, pattern);
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
  ThotPoint           *points;
  int                 npoints, maxpoints;
  PtrTextBuffer       adbuff;
  int                 i, j;
  double               x1, y1, x2, y2;
  double               cx1, cy1, cx2, cy2;

  if (thick == 0 || fg < 0)
    return;

  /* alloue la liste des points */
  npoints = 0;
  maxpoints = ALLOC_POINTS;

  points = (ThotPoint *) TtaGetMemory (sizeof (ThotPoint) * maxpoints);
  adbuff = buffer;
  y += FrameTable[frame].FrTopMargin;
  j = 1;
  x1 = (double) (x + PixelValue (adbuff->BuPoints[j].XCoord,
                                 UnPixel, NULL,
                                 ViewFrameTable[frame - 1].FrMagnification));
  y1 = (double) (y + PixelValue (adbuff->BuPoints[j].YCoord,
                                 UnPixel, NULL,
                                 ViewFrameTable[frame - 1].FrMagnification));
  j++;
  cx1 = (controls[j].lx * 3 + x1 - x) / 4 + x;
  cy1 = (controls[j].ly * 3 + y1 - y) / 4 + y;
  x2 = (double) (x + PixelValue (adbuff->BuPoints[j].XCoord,
                                 UnPixel, NULL,
                                 ViewFrameTable[frame - 1].FrMagnification));
  y2 = (double) (y + PixelValue (adbuff->BuPoints[j].YCoord,
                                 UnPixel, NULL,
                                 ViewFrameTable[frame - 1].FrMagnification));
  cx2 = (controls[j].lx * 3 + x2 - x) / 4 + x;
  cy2 = (controls[j].ly * 3 + y2 - y) / 4 + y;

  /* backward arrow  */
  if (arrow == 2 || arrow == 3)
    ArrowDrawing (frame,
                  FloatToInt ((float)cx1), FloatToInt ((float)cy1),
                  (int) x1, (int) y1,
                  thick, fg);
  
  for (i = 2; i < nb; i++)
    {
      PolySplit (x1, y1, cx1, cy1, cx2, cy2, x2, y2, &points, &npoints,
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
          x2 = (double) (x + PixelValue (adbuff->BuPoints[j].XCoord,
                                         UnPixel, NULL,
                                         ViewFrameTable[frame - 1].FrMagnification));
          y2 = (double) (y + PixelValue (adbuff->BuPoints[j].YCoord,
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
  PolyNewPoint (x2, y2, &points, &npoints, &maxpoints);

  /* Draw the border */
  InitDrawing (style, thick, fg);
#ifdef _GTK
  gdk_draw_lines (FrRef[frame], TtLineGC, points, npoints);
#endif /* _GTK */
  
  /* Forward arrow */
  if (arrow == 1 || arrow == 3)
    ArrowDrawing (frame,
                  FloatToInt ((float)cx2), FloatToInt ((float)cy2),
                  (int) x2, (int) y2,
                  thick, fg);

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
void DrawSpline (int frame, int thick, int style, int x, int y,
                 PtrTextBuffer buffer, int nb, int fg, int bg, int pattern,
                 C_points *controls)
{
  ThotPoint           *points;
  int                 npoints, maxpoints;
  PtrTextBuffer       adbuff;
  int                 i, j;
  double               x1, y1, x2, y2;
  double               cx1, cy1, cx2, cy2;
  ThotPixmap              pat;

  /* allocate the list of points */
  npoints = 0;
  maxpoints = ALLOC_POINTS;
  points = (ThotPoint *) TtaGetMemory (sizeof (ThotPoint) * maxpoints);
  adbuff = buffer;
  y += FrameTable[frame].FrTopMargin;
  j = 1;
  x1 = (double) (x + PixelValue (adbuff->BuPoints[j].XCoord,
                                 UnPixel, NULL,
                                 ViewFrameTable[frame - 1].FrMagnification));
  y1 = (double) (y + PixelValue (adbuff->BuPoints[j].YCoord,
                                 UnPixel, NULL,
                                 ViewFrameTable[frame - 1].FrMagnification));
  cx1 = controls[j].rx + x;
  cy1 = controls[j].ry + y;
  j++;
  x2 = (double) (x + PixelValue (adbuff->BuPoints[j].XCoord,
                                 UnPixel, NULL,
                                 ViewFrameTable[frame - 1].FrMagnification));
  y2 = (double) (y + PixelValue (adbuff->BuPoints[j].YCoord,
                                 UnPixel, NULL,
                                 ViewFrameTable[frame - 1].FrMagnification));
  cx2 = controls[j].lx + x;
  cy2 = controls[j].ly + y;

  for (i = 2; i < nb; i++)
    {
      PolySplit (x1, y1, cx1, cy1, cx2, cy2, x2, y2, &points, &npoints,
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
          x2 = (double) (x + PixelValue (adbuff->BuPoints[j].XCoord,
                                         UnPixel, NULL,
                                         ViewFrameTable[frame - 1].FrMagnification));
          y2 = (double) (y + PixelValue (adbuff->BuPoints[j].YCoord,
                                         UnPixel, NULL,
                                         ViewFrameTable[frame - 1].FrMagnification));
          cx2 = controls[i + 1].lx + x;
          cy2 = controls[i + 1].ly + y;
        }
      else
        {
          /* loop around the origin point */
          x2 = (double) (x + PixelValue (buffer->BuPoints[1].XCoord,
                                         UnPixel, NULL,
                                         ViewFrameTable[frame - 1].FrMagnification));
          y2 = (double) (y + PixelValue (buffer->BuPoints[1].YCoord,
                                         UnPixel, NULL,
                                         ViewFrameTable[frame - 1].FrMagnification));
          cx2 = controls[1].lx + x;
          cy2 = controls[1].ly + y;
        }
    }

  /* close the polyline */
  PolySplit (x1, y1, cx1, cy1, cx2, cy2, x2, y2, &points, &npoints, &maxpoints);
  PolyNewPoint (x2, y2, &points, &npoints, &maxpoints);

  /* Fill in the polygone */
  pat = (ThotPixmap) CreatePattern (0, fg, bg, pattern);
  if (pat != 0)
    {
#ifdef _GTK
      gdk_gc_set_tile ( TtGreyGC, (GdkPixmap *)pat);
      gdk_draw_polygon (FrRef[frame], TtGreyGC, TRUE , points, npoints); 
      gdk_pixmap_unref ((GdkPixmap *)pat);
#endif /* _GTK */
    }

  /* Draw the border */
  if (thick > 0 && fg >= 0)
    {
      InitDrawing (style, thick, fg);
#ifdef _GTK
      gdk_draw_polygon (FrRef[frame], TtLineGC, FALSE, points, npoints);
#endif /* _GTK */
    }

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
        {
          InitDrawing (style, thick, fg);
          DoDrawOneLine (frame, points[0].x, points[0].y,
                         points[1].x, points[1].y);
        }
      else
        /* draw a polyline or a ploygon */
        DoDrawLines (frame, thick, style, points, npoints, fg, bg, pattern);
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
  ThotPoint           *points;
  int                 npoints, maxpoints;
  PtrPathSeg          pPa;
  double               x1, y1, cx1, cy1, x2, y2, cx2, cy2;

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
            /* this path segment starts a new subpath 
               (M or m attribute parameter)
               if some points are already stored, display the line
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
              x1 = (double) (x + PixelValue (pPa->XStart, UnPixel, NULL,
                                             ViewFrameTable[frame - 1].FrMagnification));
              y1 = (double) (y + PixelValue (pPa->YStart, UnPixel, NULL,
                                             ViewFrameTable[frame - 1].FrMagnification));
              x2 = (double) (x + PixelValue (pPa->XEnd, UnPixel, NULL,
                                             ViewFrameTable[frame - 1].FrMagnification));
              y2 = (double) (y + PixelValue (pPa->YEnd, UnPixel, NULL,
                                             ViewFrameTable[frame - 1].FrMagnification));
              PolyNewPoint (x1, y1, &points, &npoints, &maxpoints);
              PolyNewPoint (x2, y2, &points, &npoints, &maxpoints);
              break;

            case PtCubicBezier:
              x1 = (double) (x + PixelValue (pPa->XStart, UnPixel, NULL,
                                             ViewFrameTable[frame - 1].FrMagnification));
              y1 = (double) (y + PixelValue (pPa->YStart, UnPixel, NULL,
                                             ViewFrameTable[frame - 1].FrMagnification));
              cx1 = (double) (x + PixelValue (pPa->XCtrlStart, UnPixel, NULL,
                                              ViewFrameTable[frame - 1].FrMagnification));
              cy1 = (double) (y + PixelValue (pPa->YCtrlStart, UnPixel, NULL,
                                              ViewFrameTable[frame - 1].FrMagnification));
              x2 = (double) (x + PixelValue (pPa->XEnd, UnPixel, NULL,
                                             ViewFrameTable[frame - 1].FrMagnification));
              y2 = (double) (y + PixelValue (pPa->YEnd, UnPixel, NULL,
                                             ViewFrameTable[frame - 1].FrMagnification));
              cx2 = (double) (x + PixelValue (pPa->XCtrlEnd, UnPixel, NULL,
                                              ViewFrameTable[frame - 1].FrMagnification));
              cy2 = (double) (y + PixelValue (pPa->YCtrlEnd, UnPixel, NULL,
                                              ViewFrameTable[frame - 1].FrMagnification));
              PolySplit (x1, y1, cx1, cy1, cx2, cy2, x2, y2, &points, &npoints,
                         &maxpoints);
              PolyNewPoint (x2, y2, &points, &npoints, &maxpoints);
              break;

            case PtQuadraticBezier:
              x1 = (double) (x + PixelValue (pPa->XStart, UnPixel, NULL,
                                             ViewFrameTable[frame - 1].FrMagnification));
              y1 = (double) (y + PixelValue (pPa->YStart, UnPixel, NULL,
                                             ViewFrameTable[frame - 1].FrMagnification));
              cx1 = (double) (x + PixelValue (pPa->XCtrlStart, UnPixel, NULL,
                                              ViewFrameTable[frame - 1].FrMagnification));
              cy1 = (double) (y + PixelValue (pPa->YCtrlStart, UnPixel, NULL,
                                              ViewFrameTable[frame - 1].FrMagnification));
              x2 = (double) (x + PixelValue (pPa->XEnd, UnPixel, NULL,
                                             ViewFrameTable[frame - 1].FrMagnification));
              y2 = (double) (y + PixelValue (pPa->YEnd, UnPixel, NULL,
                                             ViewFrameTable[frame - 1].FrMagnification));
              QuadraticSplit (x1, y1, cx1, cy1, x2, y2, &points, &npoints,
                              &maxpoints);
              PolyNewPoint (x2, y2, &points, &npoints, &maxpoints);
              break;

            case PtEllipticalArc:
              x1 = pPa->XStart;
              y1 = pPa->YStart;
              x2 = pPa->XEnd;
              y2 = pPa->YEnd;		
              cx1 = pPa->XRadius; 
              cy1 = pPa->YRadius; 
              EllipticSplit ( frame, x, y,
                              (double) x1, (double) y1, 
                              (double) x2, (double) y2, 
                              (double) cx1, (double) cy1,
                              (int)fmod (pPa->XAxisRotation, 360), 
                              pPa->LargeArc, pPa->Sweep,
                              &points, &npoints, &maxpoints);
              x2 = (double) (x + PixelValue (pPa->XEnd, UnPixel, NULL,
                                             ViewFrameTable[frame - 1].FrMagnification));
              y2 = (double) (y + PixelValue (pPa->YEnd, UnPixel, NULL,
                                             ViewFrameTable[frame - 1].FrMagnification));
              PolyNewPoint (x2, y2, &points, &npoints, &maxpoints);
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
void DrawOval (int frame, int thick, int style, int x, int y, int width,
               int height, int rx, int ry, int fg, int bg, int pattern)
{
#ifdef _GTK
  int                 i;
  ThotPixmap              pat;
  int                 arc, dx, dy;
  int                 xf, yf;
  ThotArc             xarc[4];
  ThotSegment         seg[4];
  ThotPoint           point[13];

  width -= thick;
  height -= thick;
  x += thick / 2;
  y = y + thick / 2 + FrameTable[frame].FrTopMargin;

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
  dx = rx;
  dy = ry;
  rx = rx * 2;
  ry = ry * 2;
  xf = x + width - 1;
  yf = y + height - 1;

  xarc[0].x = x;
  xarc[0].y = y;
  xarc[0].width = rx;
  xarc[0].height = ry;
  xarc[0].angle1 = 90 * 64;
  xarc[0].angle2 = 90 * 64;

  xarc[1].x = xf - rx;
  xarc[1].y = xarc[0].y;
  xarc[1].width = rx;
  xarc[1].height = ry;
  xarc[1].angle1 = 0;
  xarc[1].angle2 = xarc[0].angle2;

  xarc[2].x = xarc[0].x;
  xarc[2].y = yf - ry;
  xarc[2].width = rx;
  xarc[2].height = ry;
  xarc[2].angle1 = 180 * 64;
  xarc[2].angle2 = xarc[0].angle2;

  xarc[3].x = xarc[1].x;
  xarc[3].y = xarc[2].y;
  xarc[3].width = rx;
  xarc[3].height = ry;
  xarc[3].angle1 = 270 * 64;
  xarc[3].angle2 = xarc[0].angle2;

  seg[0].x1 = x + dx;
  seg[0].x2 = xf - dx;
  seg[0].y1 = y;
  seg[0].y2 = seg[0].y1;

  seg[1].x1 = xf;
  seg[1].x2 = seg[1].x1;
  seg[1].y1 = y + dy;
  seg[1].y2 = yf - dy;

  seg[2].x1 = seg[0].x1;
  seg[2].x2 = seg[0].x2;
  seg[2].y1 = yf;
  seg[2].y2 = seg[2].y1;

  seg[3].x1 = x;
  seg[3].x2 = seg[3].x1;
  seg[3].y1 = seg[1].y1;
  seg[3].y2 = seg[1].y2;

  /* Fill in the figure */
  pat = CreatePattern (0, fg, bg, pattern);
  if (pat != 0)
    {
      /* Polygone inscrit: (seg0)       */
      /*                   0--1         */
      /*                10-|  |-3       */
      /*         (seg3) |       |(seg1) */
      /*                9--|  |-4       */
      /*                   7--6         */
      /*                   (seg2)       */
      point[0].x = seg[0].x1;
      point[0].y = seg[0].y1;

      point[1].x = seg[0].x2;
      point[1].y = point[0].y;
      point[2].x = point[1].x;
      point[2].y = seg[1].y1;

      point[3].x = seg[1].x1;
      point[3].y = point[2].y;
      point[4].x = point[3].x;
      point[4].y = seg[1].y2;

      point[5].x = seg[2].x2;
      point[5].y = point[4].y;
      point[6].x = point[5].x;
      point[6].y = seg[2].y2;

      point[7].x = seg[2].x1;
      point[7].y = point[6].y;
      point[8].x = point[7].x;
      point[8].y = seg[3].y2;

      point[9].x = seg[3].x2;
      point[9].y = point[8].y;
      point[10].x = point[9].x;
      point[10].y = seg[3].y1;

      point[11].x = point[0].x;
      point[11].y = point[10].y;
      point[12].x = point[0].x;
      point[12].y = point[0].y;

      gdk_gc_set_tile (TtGreyGC, (GdkPixmap *)pat);
      gdk_draw_polygon (FrRef[frame], TtGreyGC, TRUE, point, 13);
      /* Trace quatre arcs de cercle */
      for (i=0;i<4;i++)
        {
          gdk_draw_arc (FrRef[frame], TtGreyGC, TRUE,
                        xarc[i].x, xarc[i].y, 
                        xarc[i].width, xarc[i].height, 
                        xarc[i].angle1,xarc[i].angle2); 
        }
      gdk_pixmap_unref ((GdkPixmap *)pat);
    }

  /* Draw the border */
  if (thick > 0 && fg >= 0)
    {
      InitDrawing (style, thick, fg);
      for (i=0;i<4;i++)
        {
          gdk_draw_arc (FrRef[frame], TtLineGC, FALSE, 
                        xarc[i].x, xarc[i].y, 
                        xarc[i].width, xarc[i].height, 
                        xarc[i].angle1,xarc[i].angle2);
        }
      gdk_draw_segments (FrRef[frame], TtLineGC, (GdkSegment *)seg, 4);
    }
#endif /* _GTK */
}

/*----------------------------------------------------------------------
  DrawEllips draw an ellips (or a circle).
  Parameters fg, bg, and pattern are for drawing color, background color
  and fill pattern.
  ----------------------------------------------------------------------*/
void DrawEllips (int frame, int thick, int style, int x, int y, int width,
                 int height, int fg, int bg, int pattern)
{
  ThotPixmap              pat;

  width -= thick + 1;
  height -= thick + 1;
  x += thick / 2;
  y = y + thick / 2 + FrameTable[frame].FrTopMargin;

  /* Fill in the rectangle */
  pat = (ThotPixmap) CreatePattern (0, fg, bg, pattern);
  if (pat == 0 && thick <= 0)
    return;


  if (pat != 0)
    {
#ifdef _GTK 
      gdk_gc_set_tile ( TtGreyGC, (GdkPixmap *)pat);
      gdk_draw_arc (FrRef[frame], TtGreyGC, TRUE, x, y, width, height, 0, 360 * 64);
      gdk_pixmap_unref ((GdkPixmap *)pat);
#endif /* _GTK */
    }

  /* Draw the border */
  if (thick > 0 && fg >= 0)
    {
      InitDrawing (style, thick, fg);
#ifdef _GTK
      gdk_draw_arc (FrRef[frame], TtLineGC, FALSE,  x, y, width, height, 0, 360 * 64);
#endif /* _GTK */
    }
}

/*----------------------------------------------------------------------
  DrawHorizontalLine draw a horizontal line aligned top center or bottom
  depending on align value.
  The parameter fg indicates the drawing color.
  leftslice and rightslice say if the left and right borders are sliced.
  ----------------------------------------------------------------------*/
void DrawHorizontalLine (int frame, int thick, int style, int x, int y,
                         int l, int h, int align, int fg, PtrBox box,
                         int leftslice, int rightslice)
{
  int        Y;

  if (thick <= 0 || fg < 0)
    return;
  y += FrameTable[frame].FrTopMargin;
  if (thick > 1 && style > 5)
    {
      if (align == 1)
        DrawRectangle (frame, 1, style, x, y + (h - thick) / 2, l, thick, fg, fg, 2);
      else if (align == 2)
        DrawRectangle (frame, 1, style, x, y + h - thick, l, thick, fg, fg, 2);
      else
        DrawRectangle (frame, 1, style, x, y, l, thick, fg, fg, 2);
    }
  else
    {
      if (align == 1)
        Y = y + (h - thick) / 2;
      else if (align == 2)
        Y = y + h - (thick + 1) / 2;
      else
        Y = y + thick / 2;
      InitDrawing (style, thick, fg);
      DoDrawOneLine (frame, x, Y, x + l, Y);
    }
}

/*----------------------------------------------------------------------
  DrawHat draw a hat aligned top
  The parameter fg indicates the drawing color.
  ----------------------------------------------------------------------*/
void DrawHat (int frame, int thick, int style, int x, int y, int l, int h,
              int fg, int direction)
{
int Y;

h -= thick;
l -= thick;

  if (thick > 0 && fg >= 0)
    {
      y += FrameTable[frame].FrTopMargin + h / 2;
      Y = y + direction * h / 2;
      InitDrawing (style, thick, fg);
      DoDrawOneLine (frame, x, Y, x + l/2, y);
      DoDrawOneLine (frame, x + l/2, y, x + l, Y);
    }
}

/*----------------------------------------------------------------------
  DrawTilde draw a hat aligned top
  The parameter fg indicates the drawing color.
  ----------------------------------------------------------------------*/
void DrawTilde (int frame, int thick, int style, int x, int y, int l, int h, int fg)
{
  int X, Y1, Y2, Xmax, Ymax;

  h -= thick;
  l -= thick;

  if (thick > 0 && fg >= 0)
    {
      Xmax = 10;
      Ymax = h / 3;
      y += FrameTable[frame].FrTopMargin + h / 2;
      InitDrawing (style, thick, fg);

      for(X = 1, Y1 = 0; X <= Xmax; X++)
        {     
        Y2 = (int)(((float) Ymax) * DSIN (X*M_PI_DOUBLE/Xmax));
        DoDrawOneLine (frame, x + (X-1)*l/Xmax, y + Y1, x + X*l/Xmax, y + Y2);
        Y1 = Y2;
        }
    }
}

/*----------------------------------------------------------------------
  DrawHorizontalBrace draw a horizontal brace aligned top or bottom
  depending on align value.
  The parameter fg indicates the drawing color.
  ----------------------------------------------------------------------*/
void DrawHorizontalParenthesis (int frame, int thick, int style, int x, int y,
                          int l, int h, int align, int fg)
{
}

/*----------------------------------------------------------------------
  DrawHorizontalBrace draw a horizontal brace aligned top or bottom
  depending on align value.
  The parameter fg indicates the drawing color.
  ----------------------------------------------------------------------*/
void DrawHorizontalBrace (int frame, int thick, int style, int x, int y,
                          int l, int h, int align, int fg)
{
  int        Y;

  if (thick > 0 && fg >= 0)
    {
      y += FrameTable[frame].FrTopMargin;
      Y = y + (h - thick) / 2;
      InitDrawing (style, thick, fg);
      DoDrawOneLine (frame, x, Y, x + l, Y);
      if (align == 0)
        /* Over brace */
        {
          DoDrawOneLine (frame, x, Y, x, y + h);
          DoDrawOneLine (frame, x + (l / 2), Y, x + (l / 2), y);
          DoDrawOneLine (frame, x + l - thick, Y, x + l - thick, y + h);
        }
      else
        /* Underbrace */
        {
          DoDrawOneLine (frame, x, Y, x, y);
          DoDrawOneLine (frame, x + (l / 2), Y, x + (l / 2), y + h);
          DoDrawOneLine (frame, x + l - thick, Y, x + l - thick, y);
        }
    }
}

/*----------------------------------------------------------------------
  DrawHorizontalBracket draw a horizontal brace aligned top or bottom
  depending on align value.
  The parameter fg indicates the drawing color.
  ----------------------------------------------------------------------*/
void DrawHorizontalBracket (int frame, int thick, int style, int x, int y,
                          int l, int h, int align, int fg)
{
  int        Y;

  if (thick > 0 && fg >= 0)
    {
      y += FrameTable[frame].FrTopMargin;
      Y = y + (h - thick) / 2;
      InitDrawing (style, thick, fg);
      DoDrawOneLine (frame, x, Y, x + l, Y);
      if (align == 0)
        /* Over bracket */
        {
          DoDrawOneLine (frame, x, Y, x, y + h);
          DoDrawOneLine (frame, x + l - thick, Y, x + l - thick, y + h);
        }
      else
        /* Under bracket */
        {
          DoDrawOneLine (frame, x, Y, x, y);
          DoDrawOneLine (frame, x + l - thick, Y, x + l - thick, y);
        }
    }
}

/*----------------------------------------------------------------------
  DrawVerticalLine draw a vertical line aligned left center or right
  depending on align value.
  parameter fg indicates the drawing color
  topslice and bottomslice say if the top and bottom borders are sliced.
  ----------------------------------------------------------------------*/
void DrawVerticalLine (int frame, int thick, int style, int x, int y,
                       int l, int h, int align, int fg, PtrBox box,
                       int topslice, int bottomslice)
{
  int        X;

  if (thick <= 0 || fg < 0)
    return;
  y += FrameTable[frame].FrTopMargin;
  if (thick > 1 && style > 5)
    {
      if (align == 1)
        DrawRectangle (frame, 1, style, x + (l - thick) / 2, y, thick, h, fg, fg, 2);
      else if (align == 2)
        DrawRectangle (frame, 1, style, x + l - thick, y, thick, h, fg, fg, 2);
      else
        DrawRectangle (frame, 1, style, x, y, thick, h, fg, fg, 2);
    }
  else
    {
      if (align == 1)
        X = x + (l - thick) / 2;
      else if (align == 2)
        X = x + l - (thick + 1) / 2;
      else
        X = x + thick / 2;
      InitDrawing (style, thick, fg);
      DoDrawOneLine (frame, X, y, X, y + h);
    }
}

/*----------------------------------------------------------------------
  DrawSlash draw a slash or backslash depending on direction.
  Le parame`tre indique la couleur du trace'.
  ----------------------------------------------------------------------*/
void DrawSlash (int frame, int thick, int style, int x, int y, int l, int h,
                int direction, int fg)
{
  int                 xf, yf;

  y += FrameTable[frame].FrTopMargin;
  xf = x + l;
  yf = y + h;
  if (thick > 0 && fg >= 0)
    {
      InitDrawing (style, thick, fg);
      if (direction == 0)
        DoDrawOneLine (frame, x, yf, xf, y);
      else
        DoDrawOneLine (frame, x, y, xf, yf);
    }
}

/*----------------------------------------------------------------------
  DrawCorner draw a corner.
  parameter fg indicates the drawing color
  ----------------------------------------------------------------------*/
void DrawCorner (int frame, int thick, int style, int x, int y, int l,
                 int h, int corner, int fg)
{
  ThotPoint           point[3];
  int                 xf, yf;

  if (thick <= 0)
    return;

  y += FrameTable[frame].FrTopMargin;
  xf = x + l - thick;
  yf = y + h - thick;
  InitDrawing (style, thick, fg);
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
   
#ifdef _GTK
  gdk_draw_lines (FrRef[frame], TtLineGC, point, 3);
#endif /* _GTK */
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
#ifdef _GTK
  int                 i;
  int                 arc, arc2, xf, yf;
  ThotArc             xarc[4];
  ThotSegment         seg[5];
  ThotPixmap          pat;
  ThotPoint           point[13];

  width -= thick;
  height -= thick;
  x += thick / 2;
  y = y + FrameTable[frame].FrTopMargin + thick / 2;
  /* radius of arcs is 3mm */
  arc = (int)((3 * DOT_PER_INCH) / 25.4 + 0.5);
  arc2 = 2 * arc;

  xf = x + width;
  yf = y + height;

  xarc[0].x = x;
  xarc[0].y = y;
  xarc[0].width = arc2;
  xarc[0].height = arc2;
  xarc[0].angle1 = 90 * 64;
  xarc[0].angle2 = 90 * 64;

  xarc[1].x = xf - arc2;
  xarc[1].y = y;
  xarc[1].width = xarc[0].width;
  xarc[1].height = xarc[0].width;
  xarc[1].angle1 = 0;
  xarc[1].angle2 = xarc[0].angle2;

  xarc[2].x = x;
  xarc[2].y = yf - arc2;
  xarc[2].width = xarc[0].width;
  xarc[2].height = xarc[0].width;
  xarc[2].angle1 = 180 * 64;
  xarc[2].angle2 = xarc[0].angle2;

  xarc[3].x = xarc[1].x;
  xarc[3].y = xarc[2].y;
  xarc[3].width = xarc[0].width;
  xarc[3].height = xarc[0].width;
  xarc[3].angle1 = 270 * 64;
  xarc[3].angle2 = xarc[0].angle2;

  seg[0].x1 = x + arc;
  seg[0].y1 = y;
  seg[0].x2 = xf - arc;
  seg[0].y2 = y;

  seg[1].x1 = xf;
  seg[1].y1 = y + arc;
  seg[1].x2 = xf;
  seg[1].y2 = yf - arc;

  seg[2].x1 = seg[0].x1;
  seg[2].y1 = yf;
  seg[2].x2 = seg[0].x2;
  seg[2].y2 = yf;

  seg[3].x1 = x;
  seg[3].y1 = seg[1].y1;
  seg[3].x2 = x;
  seg[3].y2 = seg[1].y2;

  /* horizontal line at 6mm from top */
  if (arc2 < height / 2)
    {
      /* not under half-height */
      seg[4].x1 = x;
      seg[4].y1 = y + arc2;
      seg[4].x2 = xf;
      seg[4].y2 = y + arc2;
    }

  /* Fill in the figure */
  pat = CreatePattern (0, fg, bg, pattern);

  if (pat != 0)
    {
      /* Polygone:         (seg0)       */
      /*                   0--1         */
      /*                10-|  |-3       */
      /*         (seg3) |       |(seg1) */
      /*                9--|  |-4       */
      /*                   7--6         */
      /*                   (seg2)       */
      point[0].x = seg[0].x1;
      point[0].y = seg[0].y1;

      point[1].x = seg[0].x2;
      point[1].y = point[0].y;
      point[2].x = point[1].x;
      point[2].y = seg[1].y1;

      point[3].x = seg[1].x1;
      point[3].y = point[2].y;
      point[4].x = point[3].x;
      point[4].y = seg[1].y2;

      point[5].x = seg[2].x2;
      point[5].y = point[4].y;
      point[6].x = point[5].x;
      point[6].y = seg[2].y2;

      point[7].x = seg[2].x1;
      point[7].y = point[6].y;
      point[8].x = point[7].x;
      point[8].y = seg[3].y2;

      point[9].x = seg[3].x2;
      point[9].y = point[8].y;
      point[10].x = point[9].x;
      point[10].y = seg[3].y1;

      point[11].x = point[0].x;
      point[11].y = point[10].y;
      point[12].x = point[0].x;
      point[12].y = point[0].y;
      gdk_gc_set_tile (TtGreyGC, (GdkPixmap *)pat);
      gdk_draw_polygon (FrRef[frame], TtGreyGC, TRUE,  point, 13);
      /* Trace quatre arcs de cercle */
      for (i = 0; i < 4; i++)
        {  
          gdk_draw_arc (FrRef[frame], TtGreyGC,TRUE,  
                        xarc[i].x, xarc[i].y, 
                        xarc[i].width, xarc[i].height, 
                        xarc[i].angle1, xarc[i].angle2);
        }
      gdk_pixmap_unref ((GdkPixmap *)pat);
    }

  /* Draw the border */
  if (thick > 0 && fg >= 0)
    {
      InitDrawing (style, thick, fg);
      for (i = 0 ; i < 4; i++)
        { 
          gdk_draw_arc (FrRef[frame], TtLineGC, FALSE, 
                        xarc[i].x, xarc[i].y, 
                        xarc[i].width, xarc[i].height, 
                        xarc[i].angle1, xarc[i].angle2);
        }
  
      if (arc2 < height / 2)
        gdk_draw_segments (FrRef[frame], TtLineGC, (GdkSegment *)seg, 5);
      else
        gdk_draw_segments (FrRef[frame], TtLineGC, (GdkSegment *)seg, 4);
    }
#endif /* _GTK */   
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
  int                 px7mm, shiftX;
  double              A;
  ThotPixmap              pat;

  width -= thick + 1;
  height -= thick + 1;
  x += thick / 2;
  y = y + FrameTable[frame].FrTopMargin + thick / 2;

  /* Fill in the rectangle */
  pat = CreatePattern (0, fg, bg, pattern);
  if (pat != 0)
    {
#ifdef _GTK
      gdk_gc_set_tile (TtGreyGC, (GdkPixmap *)pat);
      gdk_draw_arc (FrRef[frame], TtGreyGC, TRUE,
                    x, y, width, height, 0, 360 * 64);
      gdk_pixmap_unref ((GdkPixmap *)pat);
#endif /* _GTK */
    }

  /* Draw the border */
  if (thick > 0 && fg >= 0)
    {
      InitDrawing (style, thick, fg);
#ifdef _GTK
      gdk_draw_arc (FrRef[frame], TtLineGC, FALSE,
                    x, y, width, height, 0, 360 * 64); 
#endif /* _GTK */
      px7mm = (int)((7 * DOT_PER_INCH) / 25.4 + 0.5);
      if (height > 2 * px7mm)
        {
          A = ((double) height - 2 * px7mm) / height;
          A = 1.0 - sqrt (1 - A * A);
          shiftX = (int)(width * A * 0.5 + 0.5);
#ifdef _GTK
          gdk_draw_line (FrRef[frame], TtLineGC,
                         x + shiftX, y + px7mm, x + width - shiftX, y + px7mm);
#endif /* _GTK */	    
        }
    }
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void DrawBezierControl (int frame, int size, int x_point, int y_point,
			int x_ctrl, int y_ctrl, int bg, int fg)
{
  InitDrawing (5, 1, bg);
  DoDrawOneLine (frame, x_ctrl, y_ctrl, x_point, y_point);
  DrawEllipsFrame (frame, 1, 5, x_ctrl - size, y_ctrl - size,
		   size*2, size*2, fg, bg, 2);
}

/*----------------------------------------------------------------------
  SetMainWindowBackgroundColor :                          
  ----------------------------------------------------------------------*/
void SetMainWindowBackgroundColor (int frame, int color)
{
#ifdef _GTK
  GdkColor gdkcolor;

  gdkcolor.pixel = gdk_rgb_xpixel_from_rgb (ColorPixel (color));
  gdk_window_set_background (FrRef[frame], &gdkcolor);
#endif /* _GTK */
}

/*----------------------------------------------------------------------
  Clear clear the area of frame located at (x, y) and of size width x height.
  ----------------------------------------------------------------------*/
void Clear (int frame, int width, int height, int x, int y)
{
#ifndef _WX
  ThotWindow          w;

  w = FrRef[frame];

  if (w != None)
    {
#ifdef _GTK
      gdk_window_clear_area (w, x, y + FrameTable[frame].FrTopMargin, width, height);
#endif /* _GTK */
    }
#else /* _WX */

#endif /* _WX */
}

/*----------------------------------------------------------------------
  WChaine draw a string in frame, at location (x, y) and using font.
  ----------------------------------------------------------------------*/
void WChaine (ThotWindow w, char *string, int x, int y, ThotFont font,
              ThotGC GClocal)
{
#ifdef _GTK
  gdk_draw_string (w, font, GClocal, x, y+gdk_string_height (font, string), string); 
#endif /* _GTK */
}


/*----------------------------------------------------------------------
  VideoInvert switch to inverse video the area of frame located at
  (x,y) and of size width x height.
  ----------------------------------------------------------------------*/
void VideoInvert (int frame, int width, int height, int x, int y)
{
#ifndef _WX
  ThotWindow          w;

  w = FrRef[frame];
#ifdef _GTK  
  if (w != None)
    gdk_draw_rectangle (w, TtInvertGC, TRUE, x, 
                        y + FrameTable[frame].FrTopMargin, width, height);
#endif /* _GTK */

#else /* _WX */
#endif /* _WX */
}


/*----------------------------------------------------------------------
  Scroll do a scrolling/Bitblt of frame of a width x height area
  from (xd,yd) to (xf,yf).
  ----------------------------------------------------------------------*/
void Scroll (int frame, int width, int height, int xd, int yd, int xf, int yf)
{
#ifndef _WX
  if (FrRef[frame] != None)
    {
#ifdef _GTK 
      gdk_window_copy_area (FrRef[frame], TtWhiteGC,
                            xf,
                            yf + FrameTable[frame].FrTopMargin,
                            FrRef[frame],
                            xd,
                            yd + FrameTable[frame].FrTopMargin,
                            width,
                            height);
#endif /* _GTK */
    }
#else /* _WX */
#endif /* _WX */
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
  ThotPixmap              pat;

  /* Fill the rectangle associated to the given frame */
  pat = (ThotPixmap) CreatePattern (0, fg, 0, pattern);
  if (pat != 0)
    {
#ifdef _GTK
      gdk_gc_set_tile (TtGreyGC, (GdkPixmap *)pat);
      if (w != 0)
        gdk_draw_rectangle (w, TtGreyGC, TRUE, x, y, width, height);     
      else
        gdk_draw_rectangle (FrRef[frame], TtGreyGC, TRUE,
                            x, y + FrameTable[frame].FrTopMargin, 
                            width, height);
      gdk_pixmap_unref ((GdkPixmap *)pat);
#endif /* _GTK */
    }    
}
#endif /*_GL*/
