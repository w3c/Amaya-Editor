/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2009
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * gldisplay.c : handling of low level drawing routines, 
 *               with Opengl library both for
 *                   MS-Windows (incomplete).
 *                   Unix
 *
 * Author: P. Cheyrou-lagreze (INRIA)
 *
 */
#ifdef _GL
#ifdef _GTK
#include <gtkgl/gtkglarea.h>
#endif /* _GTK */

#ifdef _WINGUI
#include <windows.h>
#endif /* _WINGUI */

#ifdef _WX
#include "wx/wx.h"
#else /* _WX */
#include <GL/gl.h>
#endif /* _WX */

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

#define ALLOC_POINTS    300

#include "boxlocate_f.h"
#include "buildlines_f.h"
#include "context_f.h"
#include "font_f.h"
#include "inites_f.h"
#include "memory_f.h"
#include "units_f.h"
#include "windowdisplay_f.h"
#include "tesse_f.h"
#include "spline_f.h"
#include "stix_f.h"
#include "glwindowdisplay.h"
#include "glprint.h"

/*
 * Math Macros conversion from
 * degrees to radians and so on...
 * All for EllipticSplit and/or GL_DrawArc
 */
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#define M_PI_DOUBLE (6.2831853718027492)

/* ((A)*(M_PI/180.0)) */
#define DEG_TO_RAD(A)   ((double)A)/57.29577957795135
#define RAD_TO_DEG(A)   ((double)A)*57.29577957795135

#define IS_ZERO(arg)                    (fabs(arg)<1.e-20)

/*If we should use a static table instead for
  performance bottleneck...*/
#define DCOS(A) ((double)cos (A))
#define DSIN(A) ((double)sin (A))
#define DACOS(A) ((double)acos (A))
#define A_DEGREE 0.017453293

/* Precision of a degree/1 
   If we need more precision 
   dision is our friend 
   1/2 degree = 0.0087266465
   1/4 degree = 0.0043633233
   or the inverse 
   24 degree = 0.41887903
   5 degree = 0.087266465
   2 degree = 0.034906586
   but best is a degree...
*/
#define TRIGO_PRECISION 1;
#define A_DEGREE_PART A_DEGREE/TRIGO_RECISION

/* Must find better one... 
   Bits methods...*/
/*for double => ( pow (N, 2))*/
/*for int  => (((int)N)<<1)*/
#define P2(N) (N*N)

#define	MIDDLE_OF(v1, v2) (((v1)+(v2))/2.0)
#define ALLOC_POINTS    300

#define MESA


/*----------------------------------------------------------------------
  PixelValueDble : check if we need calculation
  ----------------------------------------------------------------------*/
static double PixelValueDble (int nb, int real_nb, int frame)
{
  if (ViewFrameTable[frame - 1].FrMagnification)
    return (double) (nb + PixelValue (real_nb, UnPixel, NULL,
                                      ViewFrameTable[frame - 1].FrMagnification));
  else
    return (double) (nb + real_nb);
}

/*----------------------------------------------------------------------
  FontOrig update and (x, y) location before DrawString
  accordingly to the ascent of the font used.
  ----------------------------------------------------------------------*/
void FontOrig (ThotFont font, char firstchar, int *pX, int *pY)
{
  if (!font)
    return;
  /**pY += ((XFontStruct *) font)->ascent;*/
}


/*----------------------------------------------------------------------
  LoadColor load the given color in the drawing Graphic Context.
  The parameter fg gives the drawing color
  ----------------------------------------------------------------------*/
static void LoadColor (int fg)
{
  GL_SetForeground (fg, TRUE);
}

/*----------------------------------------------------------------------
  DoDrawOneLine draw one line starting from (x1, y1) to (x2, y2) in frame.
  ----------------------------------------------------------------------*/
static void DoDrawOneLine (int frame, int x1, int y1, int x2, int y2)
{
  GL_DrawLine (x1, y1, x2, y2, FALSE);
}


/*----------------------------------------------------------------------
  DrawChar draw a char at location (x, y) in frame and with font.
  The parameter fg indicates the drawing color
  ----------------------------------------------------------------------*/
void DrawChar (CHAR_T car, int frame, int x, int y, ThotFont font, int fg)
{
  ThotWindow          w = None;

#ifdef _WX
  w = (ThotWindow)FrameTable[frame].WdFrame;
#endif /* _WX */ 
  if (w == None)
    return;
   
  y = y + FrameTable[frame].FrTopMargin;
  GL_DrawUnicodeChar (car, (float) x, (float) y, font, fg);
}

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
  ThotWindow          w = None;
  int                 width;

#ifdef _GTK
  w = FrRef[frame];
#endif /* _GTK */
#ifdef _WX
  w = (ThotWindow)FrameTable[frame].WdFrame;
#endif /* _WX */ 
  y += FrameTable[frame].FrTopMargin; 
  /* compute the width of the string */
  width = 0;
  if (w && fg >= 0)
    {
      /* Dealing with BR tag for windows */
      if (Printing)
        width = GLString (buff, lg, frame, x, y, font, boxWidth, bl, hyphen, 
                          startABlock, fg);
      else if (lg > 0)
        width = GL_DrawString (fg, (CHAR_T *) buff, 
                               (float) x, (float) y, hyphen, font, lg);
    }
  return (width);
}

#ifndef _WIN_PRINT
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
  if (lg < 0)
    return 0;
  
  y += FrameTable[frame].FrTopMargin;
  return (GL_DrawString (fg, buff, (float) x, (float) y, hyphen,
                         (void *) font, lg));
}
#endif /*_WIN_PRINT*/

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
  ThotWindow          w = None;
  int                 bottom;	/* underline position    */
  int                 middle;	/* cross-over position   */
  int                 thickness;	/* thickness of drawing */

  if (fg < 0)
    return;
  if (lg > 0)
    {
#ifdef _GTK
      w = FrRef[frame];
#endif /* _GTK */
#ifdef _WX
      w = (ThotWindow)FrameTable[frame].WdFrame;
#endif /* _WX */ 

      if (w == None)
        return;
      thickness = 1;
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
}

/*----------------------------------------------------------------------
  DrawPoints draw a line of dot.
  The parameter fg indicates the drawing color
  ----------------------------------------------------------------------*/
void DrawPoints (int frame, int x, int y, int boxWidth, int fg)
{
  ThotWindow          w = None;
  ThotFont            font;
  SpecFont            spec;
  const char         *ptcar;
  int                 xcour, width, nb;

  spec = ThotLoadFont ('L', 1, 0, 6, UnPoint, frame);
  GetFontAndIndexFromSpec (SPACE, spec, 1, &font);
  if (boxWidth > 0)
    {
#ifdef _GTK
      w = FrRef[frame];
#endif /* _GTK */
#ifdef _WX
      w = (ThotWindow)FrameTable[frame].WdFrame;
#endif /* _WX */ 
      ptcar = " . ";

      /* compute lenght of the string " ." */
      width = CharacterWidth (SPACE, font) + CharacterWidth (46, font);
      /* compute the number of string to write */
      nb = boxWidth / width;
      xcour = x + (boxWidth % width);
      y = y + FrameTable[frame].FrTopMargin;
      /* draw the points */
      FontOrig (font, *ptcar, &x, &y);
      while (nb > 0)
        {
          xcour += GL_DrawString (fg, (CHAR_T *) ptcar, (float) xcour,
                                  (float) y, 0, font, 2);
          nb--;
        }
    }
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

  if (fh > 14)
    InitDrawing (5, 1, fg);
  else
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
  DrawMonoSymb draw a one glyph symbol.
  parameter fg indicates the drawing color
  ----------------------------------------------------------------------*/
static int DrawMonoSymb (CHAR_T symb, int frame, int x, int y, int l,
                         int h, ThotFont font, int fg)
{
  int xm, yf;

  xm = x + ((l - CharacterWidth (symb, font)) / 2);
  yf = y + ((h - CharacterHeight (symb, font)) / 2) + CharacterAscent (symb, font);
  DrawChar (symb, frame, xm, yf, font, fg);
  return 0;
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
  y += FrameTable[frame].FrTopMargin;
  xm = x + (l / 3);
  ym = y + (h / 2) - 1;
   
  InitDrawing (5, 1, fg);
  /* Center */
  DoDrawOneLine (frame, x, y + 1, xm, ym);
  DoDrawOneLine (frame, x, y + h - 2, xm, ym);
   
  InitDrawing (5, 2, fg);
  /* Borders */
  DoDrawOneLine (frame, x, y, x + l, y);
  DoDrawOneLine (frame, x, y + h - 2, x + l, y + h - 2);
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
  if (0 && h < fh * 2 && l <= CharacterWidth (213, font))
    {
      /* Only one glyph needed */
      DrawMonoSymb ((CHAR_T)'\325', frame, x, y, l, h, font, fg);
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
  if (fg < 0)
    return;
   
  DrawCenteredStixChar (font, 88, x, y, l, h, fg, frame);
}

/*----------------------------------------------------------------------
  DrawUnion draw an Union symbol.
  parameter fg indicates the drawing color
  ----------------------------------------------------------------------*/
void DrawUnion (int frame, int x, int y, int l, int h, ThotFont font, int fg)
{
  if (fg < 0)
    return;

  DrawCenteredStixChar (font, 87, x, y,  l, h, fg, frame);
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
  ThotPoint           points[3];

  width = (double) (5 + thick);
  height = 10;
  dx = (double) (x2 - x1);
  dy = (double) (y1 - y2);
  l = (double) sqrt ((double) (dx * dx + dy * dy));
  if (IS_ZERO(l)) 
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
  points[0].x = x2;
  points[0].y = y2;
  points[1].x = xc;
  points[1].y = yc;
  points[2].x = xd;
  points[2].y = yd;

  GL_DrawPolygon (points, 3, 0);
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
void DrawArrow (int frame, int thick, int style, int x, int y, int l,
		       int h, int orientation, int type, int fg)
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
  if(type >= 12 && type <= 15)
    D2 = -D1; else D2 = D1;

  /* Draw a Tee (type = 5, 9, 14) or a Bar (type = 6, 10, 15)  */
  if(type == 5 || type == 6 || type == 9 || type == 10 || type == 14 || type == 15)
    {
    switch(orientation)
      {
      case 0:
        if(type == 5 || type == 9 || type == 14)
          DoDrawOneLine (frame, x, ym - D1, x, ym + D1);
        else
          DoDrawOneLine (frame, xf, ym - D1, xf, ym + D1);
      break;
      case 90:
        if(type == 5 || type == 9 || type == 14)
          DoDrawOneLine (frame, xm - D1, yf, xm + D1, yf);
        else
          DoDrawOneLine (frame, xm - D1, y, xm + D1, y);
      break;
      case 180:
        if(type == 5 || type == 9 || type == 14)
          DoDrawOneLine (frame, xf, ym - D1, xf, ym + D1);
        else
          DoDrawOneLine (frame, x, ym - D1, x, ym + D1);
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
  DrawIntegral draws an integral. depending on type :
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
      DrawChar ((CHAR_T)'\362', frame, x, yf, font, fg);
    }
  else
    {
      /* Need more than one glyph */
      yf = y + CharacterAscent (243, font);
      DrawChar ((CHAR_T)'\363', frame, x, yf, font, fg);
      yend = y + h - CharacterHeight (245, font) 
        + CharacterAscent (245, font) - 1;
      DrawChar ((CHAR_T)'\365', frame, x, yend, font, fg);
      asc = CharacterAscent (244, font);
      hd = CharacterHeight (244, font);
      delta = yend - yf - asc;
      yf += asc;
      wd = (CharacterWidth (243, font) - CharacterWidth (244, font)) / 2;
      if (delta > 0 && hd > 0)
        {
          while (yf < yend)
            {
              DrawChar ((CHAR_T)'\364', frame, x + wd, yf, font, fg);
              yf += hd;
            }
        }
    }
  if (type == 2)		/* double integral */
    DrawIntegral (frame, thick, x + (CharacterWidth (244, font) / 2),
                  y, l, h, -1, font, fg);

  else if (type == 1)		/* contour integral */
    DrawChar ((CHAR_T)'o', frame, x + ((l - CharacterWidth (111, font)) / 2),
              y + (h - CharacterHeight (111, font)) / 2 + CharacterAscent (111, font),
              font, fg);
}

/*----------------------------------------------------------------------
  DrawBracket draw an opening or closing bracket (depending on direction)
  parameter fg indicates the drawing color
  ----------------------------------------------------------------------*/
void DrawBracket (int frame, int thick, int x, int y, int l, int h,
                  int direction, ThotFont font, int fg, int baseline)
{
  int                 xm, yf, yend;

  /* Symbol */
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
          DrawChar ((CHAR_T)'[', frame, xm, yf, font, fg);
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
          DrawChar ((CHAR_T)']', frame, xm, yf, font, fg);
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
          DrawChar ((CHAR_T)'\351', frame, xm, yf, font, fg);
          yend = y + h - CharacterHeight (235, font) + CharacterAscent (235, font);
          DrawChar ((CHAR_T)'\353', frame, xm, yend, font, fg);
          if (CharacterHeight (234, font) > 0)
            for (yf = yf + CharacterHeight (233, font) + CharacterAscent (234, font);
                 yf < yend;
                 yf += CharacterHeight (234, font))
              DrawChar ((CHAR_T)'\352', frame, xm, yf, font, fg);
        }
      else
        {
          /* Draw a closing bracket */
          xm = x + ((l - CharacterWidth (249, font)) / 2);
          yf = y + CharacterAscent (249, font);
          DrawChar ((CHAR_T)'\371', frame, xm, yf, font, fg);
          yend = y + h - CharacterHeight (251, font) + CharacterAscent (251, font);
          DrawChar ((CHAR_T)'\373', frame, xm, yend, font, fg);
          if (CharacterHeight (250, font) > 0)
            for (yf = yf + CharacterHeight (249, font) + CharacterAscent (250, font);
                 yf < yend;
                 yf += CharacterHeight (250, font))
              DrawChar ((CHAR_T)'\372', frame, xm, yf, font, fg);
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

  /*symbol*/
  if (FontHeight (font) >= h)
    {
      /* With only one glyph */
      if (direction == 0)
        {
          /* Draw a opening bracket */
          xm = x + ((l - CharacterWidth (225, font)) / 2);
          yf = y + ((h - CharacterHeight (225, font)) / 2) + CharacterAscent (225, font);

          DrawChar ((CHAR_T)'\341', frame, xm, yf, font, fg);
        }
      else
        {
          /* Draw a closing bracket */
          xm = x + ((l - CharacterWidth (241, font)) / 2);
          yf = y + ((h - CharacterHeight (241, font)) / 2) + CharacterAscent (241, font);
          DrawChar ((CHAR_T)'\361', frame, xm, yf, font, fg);
        }
    }
  else
    {
      /* Need more than one glyph */
      y += FrameTable[frame].FrTopMargin;
      InitDrawing (5, 4, fg);
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


  /* Symbol */
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
          DrawChar ((CHAR_T)'(', frame, xm, yf, font, fg);
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
          DrawChar ((CHAR_T)')', frame, xm, yf, font, fg);
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
          DrawChar ((CHAR_T)'\346', frame, xm, yf, font, fg);
          yend = y + h - CharacterHeight (232, font) + CharacterAscent (232, font) - 1;
          DrawChar ((CHAR_T)'\350', frame, xm, yend, font, fg);
          asc = CharacterAscent (231, font);
          hd = CharacterHeight (231, font);
          delta = yend - yf - hd;
          yf += asc;
          if (delta > 0 && hd > 0)
            {
              while (yf < yend)
                {
                  DrawChar ((CHAR_T)'\347', frame, xm, yf, font, fg);
                  yf += hd;
                }
            }
        }
      else
        {
          /* draw a closing parenthesis */
          xm = x + ((l - CharacterWidth (246, font)) / 2);
          yf = y + CharacterAscent (246, font);
          DrawChar ((CHAR_T)'\366', frame, xm, yf, font, fg);
          yend = y + h - CharacterHeight (248, font) 
            + CharacterAscent (248, font) - 1;
          DrawChar ((CHAR_T)'\370', frame, xm, yend, font, fg);
          asc = CharacterAscent (247, font);
          hd = CharacterHeight (247, font);
          delta = yend - yf - hd;
          yf += asc;
          if (delta > 0 && hd > 0)
            {
              while (yf < yend)
                {
                  DrawChar ((CHAR_T)'\367', frame, xm, yf, font, fg);
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

  /* symbol */
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
          DrawChar ((CHAR_T)'{', frame, xm, yf, font, fg);
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
          DrawChar ((CHAR_T)'}', frame, xm, yf, font, fg);
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
          DrawChar ((CHAR_T)'\354', frame, xm, yf, font, fg);
          /* vertical line */
          ym = y + ((h - CharacterHeight (237, font)) / 2) + CharacterAscent (237, font);
          DrawChar ((CHAR_T)'\355', frame, xm, ym, font, fg);
          /* bottom */
          yend = y + h - CharacterHeight (238, font) + CharacterAscent (238, font);
          DrawChar ((CHAR_T)'\356', frame, xm, yend, font, fg);
          /* finish top */
          asc = CharacterAscent (239, font);
          hd = CharacterHeight (239, font);
          delta = ym - yf - hd;
          yf += asc;
          if (delta > 0 && hd > 0)
            {
              while (yf < yend)
                {
                  DrawChar ((CHAR_T)'\357', frame, xm, yf, font, fg);
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
                  DrawChar ((CHAR_T)'\357', frame, xm, yf, font, fg);
                  yf += hd;
                }
            }
        }
      else
        {
          /* top */
          xm = x + ((l - CharacterWidth (252, font)) / 2);
          yf = y + CharacterAscent (252, font);
          DrawChar ((CHAR_T)'\374', frame, xm, yf, font, fg);
          /* center */
          ym = y + ((h - CharacterHeight (253, font)) / 2) + CharacterAscent (253, font);
          DrawChar ((CHAR_T)'\375', frame, xm, ym, font, fg);
          /* bottom */
          yend = y + h - CharacterHeight (254, font) + CharacterAscent (254, font);
          DrawChar ((CHAR_T)'\376', frame, xm, yend, font, fg);
          /* finish top */
          asc = CharacterAscent (239, font);
          hd = CharacterHeight (239, font);
          delta = ym - yf - hd;
          yf += asc;
          if (delta > 0 && hd > 0)
            {
              while (yf < yend)
                {
                  DrawChar ((CHAR_T)'\357', frame, xm, yf, font, fg);
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
                  DrawChar ((CHAR_T)'\357', frame, xm, yf, font, fg);
                  yf += hd;
                }
            }
        }
    }
}

/*----------------------------------------------------------------------
  DoDrawMesh : Draw Path as lines or polygons
  mode = 0 (GLU_TESS_WINDING_NONZERO), 1 (GLU_TESS_WINDING_ODD)
  ----------------------------------------------------------------------*/
static void DoDrawMesh (int frame, int thick, int style, void *mesh,
                        int fg, int bg, int pattern, int mode)
{
  /* Fill in the polygon */
  if (pattern == 2 && bg >= 0) 
    {
      /*  InitDrawing (style, thick, bg); */
      GL_SetForeground (bg, TRUE);
      // by default mode NONZERO
      MakeMesh (mesh, mode);
    }
  /* Draw the border */
  if (thick > 0 && fg >= 0)
    {
      InitDrawing (style, thick, fg);
      MakeMeshLines (mesh);
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
  float        th;
  const GLubyte gPat1[] = {
    0x88, 0x88, 0x88, 0x88, 0x22, 0x22, 0x22, 0x22,
    0x88, 0x88, 0x88, 0x88, 0x22, 0x22, 0x22, 0x22,
    0x88, 0x88, 0x88, 0x88, 0x22, 0x22, 0x22, 0x22,
    0x88, 0x88, 0x88, 0x88, 0x22, 0x22, 0x22, 0x22,
    0x88, 0x88, 0x88, 0x88, 0x22, 0x22, 0x22, 0x22,
    0x88, 0x88, 0x88, 0x88, 0x22, 0x22, 0x22, 0x22,
    0x88, 0x88, 0x88, 0x88, 0x22, 0x22, 0x22, 0x22,
    0x88, 0x88, 0x88, 0x88, 0x22, 0x22, 0x22, 0x22,
    0x88, 0x88, 0x88, 0x88, 0x22, 0x22, 0x22, 0x22,
    0x88, 0x88, 0x88, 0x88, 0x22, 0x22, 0x22, 0x22,
    0x88, 0x88, 0x88, 0x88, 0x22, 0x22, 0x22, 0x22,
    0x88, 0x88, 0x88, 0x88, 0x22, 0x22, 0x22, 0x22,
    0x88, 0x88, 0x88, 0x88, 0x22, 0x22, 0x22, 0x22,
    0x88, 0x88, 0x88, 0x88, 0x22, 0x22, 0x22, 0x22,
    0x88, 0x88, 0x88, 0x88, 0x22, 0x22, 0x22, 0x22,
    0x88, 0x88, 0x88, 0x88, 0x22, 0x22, 0x22, 0x22 };

  if (width <= 0 || height <= 0)
    return;
  if (thick == 0 && pattern == 0)
    return;

  y += FrameTable[frame].FrTopMargin;
  th = (float)thick /2.;
  /* pattern = 4 => we're drawing a math empty place*/
  if (pattern == 4)
    {
      glEnable (GL_POLYGON_STIPPLE);   
      glPolygonStipple (gPat1);
      GL_DrawRectangle (fg, (float) x, (float) y,
                        (float) width, (float) height);
    }
  else if (pattern == 2)
    GL_DrawRectangle (bg, (float) (x + th), (float)(y + th), 
                      (float)(width - th), (float)(height - th));

  /* Draw the border */
  if (thick > 0 && fg >= 0)
    {
      if (width > thick)
        width = width - thick;
      if (height > thick)
        height = height - thick;
      InitDrawing (style, thick, fg); 
      GL_DrawEmptyRectangle (fg, (float) (x + th), (float) (y + th),
                             (float) width, (float) height,
                             (float) thick);
    }

  if (pattern == 4)
    glDisable (GL_POLYGON_STIPPLE);      
}

/*----------------------------------------------------------------------
  DrawRectangle draw a rectangle located at (x, y) in frame.
  ----------------------------------------------------------------------*/
void DrawRectangle2 (int frame, int thick, int style, int x, int y, int width,
                    int height, int fg, int bg, int pattern)
{
  void        *mesh;  

  /* Check that width and height are positive */
  if (width <= 0 || height <= 0)
    return;

  /* Build the following shape:

    (x,y)
        1-----width-----2
        |               |
    height              |
        |               |
        |               |
        4---------------3  
  */

  mesh = GetNewMesh ();

  /* 1 */
  MeshNewPoint (x, y, mesh);

  /* 2 */
  MeshNewPoint (x + width , y, mesh);

  /* 3 */
  MeshNewPoint (x + width, y + height, mesh);

  /* 4 */
  MeshNewPoint (x, y + height, mesh);

  /* 1 */
  MeshNewPoint (x, y, mesh);

  /* Draw the shape and free the memory used */
  CountourCountAdd (mesh);
  DoDrawMesh (frame, thick, style, mesh, fg, bg, pattern, 0);
  FreeMesh (mesh);

  /* Segments are not joined well when using DoDrawMesh,
     (for instance when thick >= 10) so redraw them using the
     former function, until the problem is fixed. - F. Wang */
  if (thick > 0 && fg >= 0)
    {
      InitDrawing (style, thick, fg); 
      GL_DrawEmptyRectangle (fg, (float) x, (float) y,
                             (float) width, (float) height,
                             (float) thick);
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
  ThotPoint          *points;
  int                 i, j;
  PtrTextBuffer       adbuff;
  
  /* fill the included polygon */
  DrawPolygon (frame, 0, style, x, y, buffer, nb, fg, bg, pattern, 0);
  if (thick == 0 || fg < 0)
    return;
  
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
      points[i - 1].x = PixelValueDble (x, adbuff->BuPoints[j].XCoord, frame);
      points[i - 1].y = PixelValueDble(y, adbuff->BuPoints[j].YCoord, frame);
      j++;
    }
  
  /* backward arrow  */
  if (arrow == 2 || arrow == 3)
    ArrowDrawing (frame,
                  (int)points[1].x, (int)points[1].y,
                  (int)points[0].x, (int)points[0].y,
                  thick, fg);
  
  /* Draw the border */
  InitDrawing (style, thick, fg);
  GL_DrawLines (points, nb - 1);
  /* Forward arrow */
  if (arrow == 1 || arrow == 3)
    ArrowDrawing (frame,
                  (int)points[nb - 3].x, (int)points[nb - 3].y,
                  (int)points[nb - 2].x, (int)points[nb - 2].y,
                  thick, fg);
  
  /* free the table of points */
  TtaFreeMemory (points);
}

/*----------------------------------------------------------------------
  DoDrawLines
  Draw a polygon whose points are stored in buffer points
  Parameters fg, bg, and pattern are for drawing
  color, background color and fill pattern.
  mode = 0 (GLU_TESS_WINDING_NONZERO), 1 (GLU_TESS_WINDING_ODD)
  ----------------------------------------------------------------------*/
static void DoDrawLines (int frame, int thick, int style,
                         ThotPoint *points, int npoints, int fg, int bg,
                         int pattern, int mode)
{

  /* Fill in the polygon */
  if (pattern == 2) 
    {
      /*  InitDrawing (style, thick, bg); */
      GL_SetForeground (bg, TRUE);
      GL_DrawPolygon (points, npoints, mode);
    }

  /* Draw the border */
  if (thick > 0 && fg >= 0)
    {
      InitDrawing (style, thick, fg);
      GL_DrawLines (points, npoints);
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
  ThotPoint           points[5];

  if (width > thick + 1)
    width = width - thick - 1;
  if (height > thick + 1)
    height = height - thick - 1;
  x += thick / 2;
  y = y + thick / 2 + FrameTable[frame].FrTopMargin;

  points[0].x = x + (width / 2);
  points[0].y = y;
  points[4].x = points[0].x;
  points[4].y = points[0].y;
  points[1].x = x + width;
  points[1].y = y + (height / 2);
  points[2].x = points[0].x;
  points[2].y = y + height;
  points[3].x = x;
  points[3].y = points[1].y;

/*   /\* Fill in the diamond *\/ */
/*   if (pattern == 2) */
/*     { */
/*       LoadColor (bg); */
/*       GL_DrawPolygon (points, 5, 0); */
/*     } */

/*   /\* Draw the border *\/ */
/*   if (thick > 0 && fg >= 0) */
/*     { */
/*       InitDrawing (style, thick, fg); */
/*       GL_DrawPolygon (points, 5, 0); */
/*     } */

  DoDrawLines (frame, thick, style, points, 5, fg, bg, pattern, 0);
}

/*----------------------------------------------------------------------
  DrawParallelogram draw a diamond.
  Parameters fg, bg, and pattern are for drawing
  color, background color and fill pattern.
  ----------------------------------------------------------------------*/
void DrawParallelogram (int frame, int thick, int style, int x, int y,
			int width, int height, int e, int fg, int bg,
			int pattern)
{
  ThotPoint           points[5];

  y += FrameTable[frame].FrTopMargin;

  points[0].x = x + e;
  points[0].y = y;
  points[4].x = points[0].x;
  points[4].y = points[0].y;
  points[1].x = x + width;
  points[1].y = y;
  points[2].x = x + width - e;
  points[2].y = y + height;
  points[3].x = x;
  points[3].y = y + height;

  DoDrawLines (frame, thick, style, points, 5, fg, bg, pattern, 0);
}

/*----------------------------------------------------------------------
  DrawTrapezium draw a diamond.
  Parameters fg, bg, and pattern are for drawing
  color, background color and fill pattern.
  ----------------------------------------------------------------------*/
void DrawTrapezium (int frame, int thick, int style, int x, int y,
		    int width, int height, int e, int f,
		    int fg, int bg,
		    int pattern)
{
  ThotPoint           points[5];

  y += FrameTable[frame].FrTopMargin;

  if(e < 0)
    {
      e=-e;
      points[0].x = x;
      points[0].y = y;
      points[3].x = x + e;
      points[3].y = y + height;
    }
  else
    {
      points[0].x = x + e;
      points[0].y = y;
      points[3].x = x;
      points[3].y = y + height;
    }

  if(f < 0)
    {
      f=-f;
      points[1].x = x + width - f;
      points[1].y = y;
      points[2].x = x + width;
      points[2].y = y + height;
    }
  else
    {
      points[1].x = x + width;
      points[1].y = y;
      points[2].x = x + width - f;
      points[2].y = y + height;
    }

  points[4].x = points[0].x;
  points[4].y = points[0].y;

  DoDrawLines (frame, thick, style, points, 5, fg, bg, pattern, 0);
}

/*----------------------------------------------------------------------
  DrawPolygon draw a polygon.
  Parameter buffer is a pointer to the list of control points.
  nb indicates the number of points.
  The first point is a fake one containing the geometry.
  Parameters fg, bg, and pattern are for drawing
  color, background color and fill pattern.
  mode = 0 (GLU_TESS_WINDING_NONZERO), 1 (GLU_TESS_WINDING_ODD)
  ----------------------------------------------------------------------*/
void DrawPolygon (int frame, int thick, int style, int x, int y,
                  PtrTextBuffer buffer, int nb, int fg, int bg, int pattern, int mode)
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
      points[i - 1].x = PixelValueDble (x, adbuff->BuPoints[j].XCoord, frame);
      points[i - 1].y = PixelValueDble(y, adbuff->BuPoints[j].YCoord, frame);
      j++;
    }
  /* Close the polygon */
  points[nb - 1].x = points[0].x;
  points[nb - 1].y = points[0].y;
  DoDrawLines (frame, thick, style, points, nb, fg, bg, pattern, mode);
  /* free the table of points */
  TtaFreeMemory (points);
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
  double              x1, y1, x2, y2;
  double              cx1, cy1, cx2, cy2;

  if (thick == 0 || fg < 0)
    return;

  /* alloue la liste des points */
  npoints = 0;
  maxpoints = ALLOC_POINTS;

  points = (ThotPoint *) TtaGetMemory (sizeof (ThotPoint) * maxpoints);
  adbuff = buffer;
  y += FrameTable[frame].FrTopMargin;
  j = 1;
  x1 = PixelValueDble (x, adbuff->BuPoints[j].XCoord, frame);
  y1 = PixelValueDble(y, adbuff->BuPoints[j].YCoord, frame);
  j++;
  cx1 = (controls[j].lx * 3 + x1 - x) / 4 + x;
  cy1 = (controls[j].ly * 3 + y1 - y) / 4 + y;
  x2 = PixelValueDble (x, adbuff->BuPoints[j].XCoord, frame);
  y2 = PixelValueDble(y, adbuff->BuPoints[j].YCoord, frame);
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
          x2 = PixelValueDble (x, adbuff->BuPoints[j].XCoord, frame);
          y2 = PixelValueDble(y, adbuff->BuPoints[j].YCoord, frame);
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
  GL_DrawLines(points, npoints);
  /* Forward arrow */
  if (arrow == 1 || arrow == 3)
    ArrowDrawing (frame,
                  FloatToInt ((float)cx2), FloatToInt ((float)cy2),
                  (int) x2, (int) y2,
                  thick, fg);

  /* free the table of points */
  TtaFreeMemory (points);
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
  int                  npoints, maxpoints;
  PtrTextBuffer        adbuff;
  int                  i, j;
  double               x1, y1, x2, y2;
  double               cx1, cy1, cx2, cy2;

  /* allocate the list of points */
  npoints = 0;
  maxpoints = ALLOC_POINTS;
  points = (ThotPoint *) TtaGetMemory (sizeof (ThotPoint) * maxpoints);
  adbuff = buffer;
  y += FrameTable[frame].FrTopMargin;
  j = 1;
  x1 = PixelValueDble (x, adbuff->BuPoints[j].XCoord, frame);
  y1 = PixelValueDble(y, adbuff->BuPoints[j].YCoord, frame);
  cx1 = controls[j].rx + x;
  cy1 = controls[j].ry + y;
  j++;
  x2 = PixelValueDble (x, adbuff->BuPoints[j].XCoord, frame);
  y2 = PixelValueDble(y, adbuff->BuPoints[j].YCoord, frame);
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
          x2 = PixelValueDble (x, adbuff->BuPoints[j].XCoord, frame);
          y2 = PixelValueDble(y, adbuff->BuPoints[j].YCoord, frame);
          cx2 = controls[i + 1].lx + x;
          cy2 = controls[i + 1].ly + y;
        }
      else
        {
          /* loop around the origin point */
          x2 = PixelValueDble (x, adbuff->BuPoints[1].XCoord, frame);
          y2 = PixelValueDble(y, adbuff->BuPoints[1].YCoord, frame);
          cx2 = controls[1].lx + x;
          cy2 = controls[1].ly + y;
        }
    }

  /* close the polyline */
  PolySplit (x1, y1, cx1, cy1, cx2, cy2, x2, y2, &points, &npoints, &maxpoints);
  PolyNewPoint ((int) x2, (int) y2, &points, &npoints, &maxpoints);

  /* Fill in the polygone */
  if (pattern == 2)
    {
      GL_SetForeground (bg, TRUE);
      GL_DrawPolygon (points, npoints, 0);
    }

  /* Draw the border */
  if (thick > 0 && fg >= 0)
    {
      InitDrawing (style, thick, fg);
      GL_DrawPolygon (points, npoints, 0);
    }

  /* free the table of points */
  TtaFreeMemory (points);
}

/*----------------------------------------------------------------------
  DrawPath draws a path.
  Parameter path is a pointer to the list of path segments
  fg indicates the drawing color
  mode = 0 (GLU_TESS_WINDING_NONZERO), 1 (GLU_TESS_WINDING_ODD)
  ----------------------------------------------------------------------*/
void DrawPath (int frame, int thick, int style, int x, int y,
               PtrPathSeg path, int fg, int bg, int pattern, int mode)
{
  PtrPathSeg   pPa;
  double       x1, y1, cx1, cy1, x2, y2, cx2, cy2;
  void        *mesh;  

  if (thick > 0 || fg >= 0 || (bg >= 0 && pattern == 2))
    {
      y = 0;
      mesh = GetNewMesh ();
      pPa = path;
      while (pPa)
        {
          if (pPa->PaNewSubpath)
            CountourCountAdd (mesh);
          switch (pPa->PaShape)
            {
            case PtLine:
              x1 = PixelValueDble (x, pPa->XStart, frame);	      
              y1 = PixelValueDble (y, pPa->YStart, frame);
              x2 = PixelValueDble (x, pPa->XEnd, frame);
              y2 = PixelValueDble (y, pPa->YEnd, frame);
              MeshNewPoint (x1, y1, mesh);
              MeshNewPoint (x2, y2, mesh);
              break;

            case PtCubicBezier:
              x1 = PixelValueDble (x, pPa->XStart, frame);
              y1 = PixelValueDble (y, pPa->YStart, frame);
              x2 = PixelValueDble (x, pPa->XEnd, frame);
              y2 = PixelValueDble (y, pPa->YEnd, frame);
              cx1 = PixelValueDble (x, pPa->XCtrlStart, frame);
              cy1 = PixelValueDble (y, pPa->YCtrlStart, frame);
              cx2 = PixelValueDble (x, pPa->XCtrlEnd, frame);
              cy2 = PixelValueDble (y, pPa->YCtrlEnd, frame);
              PolySplit2 (x1, y1, cx1, cy1, cx2, cy2, x2, y2, mesh);
              MeshNewPoint (x2, y2, mesh);
              break;

            case PtQuadraticBezier:
              x1 = PixelValueDble (x, pPa->XStart, frame);
              y1 = PixelValueDble (y, pPa->YStart, frame);
              x2 = PixelValueDble (x, pPa->XEnd, frame);
              y2 = PixelValueDble (y, pPa->YEnd, frame);
              cx1 = PixelValueDble (x, pPa->XCtrlStart, frame);
              cy1 = PixelValueDble (y, pPa->YCtrlStart, frame);
              QuadraticSplit2 (x1, y1, cx1, cy1, x2, y2, mesh);
              MeshNewPoint (x2, y2, mesh);
              break;

            case PtEllipticalArc:
              x1 = PixelValueDble (x, pPa->XStart, frame);
              y1 = PixelValueDble (y, pPa->YStart, frame);
              x2 = PixelValueDble (x, pPa->XEnd, frame);
              y2 = PixelValueDble (y, pPa->YEnd, frame);
              cx1 = (double) PixelValue (pPa->XRadius, UnPixel, NULL,
					 ViewFrameTable[frame - 1].FrMagnification); 
              cy1 = (double) PixelValue (pPa->YRadius, UnPixel, NULL,
					 ViewFrameTable[frame - 1].FrMagnification); 
              EllipticSplit2 (frame, x, y,
                              x1, y1, 
                              x2, y2, 
                              cx1, cy1,
                              fmod ((double)pPa->XAxisRotation, 360), 
                              (int)pPa->LargeArc, pPa->Sweep,
                              mesh);
              MeshNewPoint (x2, y2, mesh);
              break;
            }
          pPa = pPa->PaNext;
        }
      CountourCountAdd (mesh);
      DoDrawMesh (frame, thick, style, mesh, fg, bg, pattern, mode);     
      /* free the table of points */
      FreeMesh (mesh);
    }  
}

/*----------------------------------------------------------------------
  DrawOval draw a rectangle with rounded corners.
  ----------------------------------------------------------------------*/
void DrawOval (int frame, int thick, int style, int x, int y, int width,
               int height, int rx, int ry, int fg, int bg, int pattern)
{
  float rayx, rayy;
  float xmin, xmax, ymin, ymax;
  void *mesh;

  /* Check that width and height are positive */
  if (width <= 0 || height <= 0)
    return;

  /* Compute the actual radius that are going to be used */
  rayx = width / 2.;
  if (rayx > (float)rx)
    rayx = (float)rx;
  rayy = height / 2.;
  if (rayy > (float)ry)
    rayy = (float)ry;

  if (rx == 0 && ry)
    {
      /* radius must be equal */
      rayx = width / 2.;
      if (rayy > rayx)
        rayy = rayx;
      else
        rayx = rayy;
    }
  else if (ry == 0 && rx)
    {
      /* radius must be equal */
      rayy = height / 2.;
      if (rayx > rayy)
        rayx = rayy;
      else
        rayy = rayx;
    }

  /* Compute the coordinates used for the points */
  xmin = (float)x;
  xmax = (float)(x+width);
  ymin = (float)y;
  ymax = (float)(y+height);

  /* Build the following shape:

                        rayx
  (xmin,ymin)           <-->
          2-------------3
         /               \
       1/                 \4
        |                 |
        |                 |
        |                 |
      ^8\                 /5
  rayy|  \               /
      v   7-------------6
                        (xmax,ymax)
  */
 
  mesh = GetNewMesh ();

  /* 1 -> 2 */
  EllipticSplit2 (frame, 0, 0,
		  xmin       , ymin+rayy,
		  xmin + rayx, ymin     ,
		  rayx, rayy, 0, 0, 1, mesh);

  /* 3 -> 4 */
  EllipticSplit2 (frame, 0, 0,
		  xmax - rayx, ymin       ,
		  xmax       , ymin + rayy, 
		  rayx, rayy, 0, 0, 1, mesh);

  /* 5 -> 6 */
  EllipticSplit2 (frame, 0, 0,
		  xmax       , ymax - rayy,
		  xmax - rayx, ymax       ,
		  rayx, rayy, 0, 0, 1, mesh);

  /* 7 -> 8 */
  EllipticSplit2 (frame, 0, 0,
		  xmin + rayx, ymax       ,
		  xmin       , ymax - rayy,
		  rayx, rayy, 0, 0, 1, mesh);

  /* 8 -> 1 */
  MeshNewPoint (xmin, ymin+rayy, mesh);

  /* Draw the shape and free the memory used */
  CountourCountAdd (mesh);
  DoDrawMesh (frame, thick, style, mesh, fg, bg, pattern, 0);
  FreeMesh (mesh);

  /* The arcs are not always drawn well using DoDrawMesh,
     (for instance when thick >= 10) so redraw them using the
     former function, until the problem is fixed. - F. Wang */
#ifndef _MACOS
  GL_DrawArc (xmax-rayx*2, ymin,
	      rayx*2, rayy*2,
	      0, 90,
	      0, FALSE);

  GL_DrawArc (xmax-rayx*2, ymax-rayy*2,
	      rayx*2, rayy*2,
	      270, 90,
	      0, FALSE);

  GL_DrawArc (xmin, ymax-rayy*2,
	      rayx*2, rayy*2,
	      180, 90,
	      0, FALSE);

  GL_DrawArc (xmin, ymin,
	      rayx*2, rayy*2,
	      90, 90,
	      0, FALSE);
#endif /* _MACOS */
/*   float               rayx, rayy, dx, dy; */
/*   float               xf, yf; */
/*   int                 i; */
/*   ThotArc             xarc[4]; */
/*   ThotSegment         seg[4]; */
/*   ThotPoint           point[13]; */

/*   y += FrameTable[frame].FrTopMargin; */
/*   /\* radius of arcs *\/ */
/*   rayx = width / 2.; */
/*   if (rayx > (float)rx) */
/*     rayx = (float)rx; */
/*   rayy = height / 2.; */
/*   if (rayy > (float)ry) */
/*     rayy = (float)ry; */

/*   if (rx == 0 && ry) */
/*     { */
/*       // radius must be equal */
/*       rayx = width / 2.; */
/*       if (rayy > rayx) */
/*         rayy = rayx; */
/*       else */
/*         rayx = rayy; */
/*     } */
/*   else if (ry == 0 && rx) */
/*     { */
/*       // radius must be equal */
/*       rayy = height / 2.; */
/*       if (rayx > rayy) */
/*         rayx = rayy; */
/*       else */
/*         rayy = rayx; */
/*     } */
/*   // arcs diameter */
/*   dx = rayx * 2; */
/*   dy = rayy * 2; */
/*   xf = x + width - 1; */
/*   yf = y + height - 1; */

/*   xarc[0].x = x; */
/*   xarc[0].y = y; */
/*   xarc[0].width = dx; */
/*   xarc[0].height = dy; */
/*   xarc[0].angle1 = 90; */
/*   xarc[0].angle2 = 90; */

/*   xarc[1].x = xf - dx; */
/*   xarc[1].y = xarc[0].y; */
/*   xarc[1].width = dx; */
/*   xarc[1].height = dy; */
/*   xarc[1].angle1 = 0; */
/*   xarc[1].angle2 = xarc[0].angle2; */

/*   xarc[2].x = xarc[0].x; */
/*   xarc[2].y = yf - dy; */
/*   xarc[2].width = dx; */
/*   xarc[2].height = dy; */
/*   xarc[2].angle1 = 180; */
/*   xarc[2].angle2 = xarc[0].angle2; */

/*   xarc[3].x = xarc[1].x; */
/*   xarc[3].y = xarc[2].y; */
/*   xarc[3].width = dx; */
/*   xarc[3].height = dy; */
/*   xarc[3].angle1 = 270; */
/*   xarc[3].angle2 = xarc[0].angle2; */

/*   seg[0].x1 = x + rayx; */
/*   seg[0].x2 = xf - rayx; */
/*   seg[0].y1 = y; */
/*   seg[0].y2 = seg[0].y1; */

/*   seg[1].x1 = xf; */
/*   seg[1].x2 = seg[1].x1; */
/*   seg[1].y1 = y + rayy; */
/*   seg[1].y2 = yf - rayy; */

/*   seg[2].x1 = seg[0].x1; */
/*   seg[2].x2 = seg[0].x2; */
/*   seg[2].y1 = yf; */
/*   seg[2].y2 = seg[2].y1; */

/*   seg[3].x1 = x; */
/*   seg[3].x2 = seg[3].x1; */
/*   seg[3].y1 = seg[1].y1; */
/*   seg[3].y2 = seg[1].y2; */

/*   InitDrawing (style, thick, fg); */
/*   /\* Fill in the figure *\/ */
/*   if (pattern == 2) */
/*     { */
/*       /\* Polygone inscrit: (seg0)       *\/ */
/*       /\*                   0--1         *\/ */
/*       /\*                10-|  |-3       *\/ */
/*       /\*         (seg3) |       |(seg1) *\/ */
/*       /\*                9--|  |-4       *\/ */
/*       /\*                   7--6         *\/ */
/*       /\*                   (seg2)       *\/ */
/*       point[0].x = seg[0].x1; */
/*       point[0].y = seg[0].y1; */

/*       point[1].x = seg[0].x2; */
/*       point[1].y = point[0].y; */
/*       point[2].x = point[1].x; */
/*       point[2].y = seg[1].y1; */

/*       point[3].x = seg[1].x1; */
/*       point[3].y = point[2].y; */
/*       point[4].x = point[3].x; */
/*       point[4].y = seg[1].y2; */

/*       point[5].x = seg[2].x2; */
/*       point[5].y = point[4].y; */
/*       point[6].x = point[5].x; */
/*       point[6].y = seg[2].y2; */

/*       point[7].x = seg[2].x1; */
/*       point[7].y = point[6].y; */
/*       point[8].x = point[7].x; */
/*       point[8].y = seg[3].y2; */

/*       point[9].x = seg[3].x2; */
/*       point[9].y = point[8].y; */
/*       point[10].x = point[9].x; */
/*       point[10].y = seg[3].y1; */

/*       point[11].x = point[0].x; */
/*       point[11].y = point[10].y; */
/*       point[12].x = point[0].x; */
/*       point[12].y = point[0].y; */

/*       GL_SetForeground (bg, TRUE); */
/*       GL_DrawPolygon (point, 13, 0); */

/*       if (rayx || rayy ) */
/*         { */
/*           for (i = 0; i < 4; i++) */
/*             { */
/*               GL_DrawArc (xarc[i].x, xarc[i].y, */
/*                           xarc[i].width, xarc[i].height, */
/*                           xarc[i].angle1, xarc[i].angle2, */
/*                           0, TRUE); */
/*             } */
/*         } */
/*     } */

/*   /\* Draw the border *\/ */
/*   if (thick > 0 && fg >= 0) */
/*     { */
/*       InitDrawing (style, thick, fg); */
/*       for (i = 0; i < 4; i++) */
/*         GL_DrawArc (xarc[i].x, xarc[i].y, */
/*                     xarc[i].width, xarc[i].height, */
/*                     xarc[i].angle1, xarc[i].angle2, */
/*                     0, FALSE); */
/*       GL_DrawSegments (seg, 4); */
/*     } */
}

/*----------------------------------------------------------------------
  DrawEllips draw an ellipse (or a circle).
  Parameters fg, bg, and pattern are for drawing color, background color
  and fill pattern.
  ----------------------------------------------------------------------*/
void DrawEllips (int frame, int thick, int style, int x, int y, int width,
                 int height, int fg, int bg, int pattern)
{
/*   void        *mesh;   */
/*   double x1, y1, x2, y2, rx, ry; */

/*   /\* Check that width and height are positive *\/ */
/*   if (width <= 0 || height <= 0) */
/*     return; */

/*   /\* Compute the coordinates and radius used *\/ */
/*   rx = ((double) width)/2; */
/*   ry = ((double) height)/2; */
/*   x1 = x; */
/*   x2 = x+width; */
/*   y1 = y+ry; */
/*   y2 = y1; */

/*   /\* Build an ellipse */

/*           --------------- */
/*          /       |       \ */
/*         /        ry       \  */
/*         |        |        | */
/*  (x1,y1)|---rx---.        |(x2,y2) */
/*         |                 | */
/*         \                 /  */
/*          \               / */
/*           --------------- */

/*   *\/ */
 
/*   mesh = GetNewMesh (); */

/*   /\* 1 -> 2 *\/ */
/*   EllipticSplit2 (frame, 0, 0, */
/* 		  x1, y1, */
/* 		  x2, y2,  */
/* 		  rx, ry, */
/* 		  0, */
/* 		  0,0, */
/* 		  mesh); */

/*   /\* 2 -> 1 *\/ */
/*   EllipticSplit2 (frame, 0, 0, */
/* 		  x2, y2, */
/* 		  x1, y1,  */
/* 		  rx, ry, */
/* 		  0, */
/* 		  0,0, */
/* 		  mesh); */

/*   /\* 1 *\/ */
/*   MeshNewPoint (x1, y1, mesh); */

/*   CountourCountAdd (mesh); */

/*   /\* Draw the shape and free the memory used *\/ */
/*   CountourCountAdd (mesh); */
/*   DoDrawMesh (frame, thick, style, mesh, fg, bg, pattern); */
/*   FreeMesh (mesh); */

/*   /\* The arcs are not always drawn well using DoDrawMesh, */
/*      (for instance when thick >= 10) so redraw them using the */
/*      former function, until the problem is fixed. - F. Wang *\/ */
/*   if (thick > 0 && fg >= 0) */
/*     { */
/*       InitDrawing (style, (thick/2), fg); */
/*       GL_DrawArc (x, y, width, height, 0, 360, 0, FALSE); */
/*     } */

  /* Fill in the rectangle */
  if (pattern != 2 && thick <= 0 && pattern != fg)
    return;

  if (pattern == fg)
    bg = fg;

  y = y + FrameTable[frame].FrTopMargin;
  if ((pattern == 2 || (bg == fg && bg == pattern)) &&
      thick < width && thick < height)
    {
      GL_SetForeground (bg, TRUE);
      GL_DrawArc (x, y, width, height, 0, 360, 0, TRUE);
    }
  /* Draw the border */
  if (thick > 0 && fg >= 0)
    {
      InitDrawing (style, thick, fg);
      GL_DrawArc (x, y, width, height, 0, 360, 0, FALSE);
    }
}

/*----------------------------------------------------------------------
  DrawHorizontalLine draw a horizontal line
  align gives the current align top (0), middle(1), bottom(2):
  leftslice and rightslice say if the left and right borders are sliced.
  The style parameter is dotted (3), dashed (4), solid (5), etc.
  The parameter fg indicates the drawing color.
  ----------------------------------------------------------------------*/
void DrawHorizontalLine (int frame, int thick, int style, int x, int y,
                         int l, int h, int align, int fg, PtrBox box,
                         int leftslice, int rightslice)
{
  ThotPoint           point[4];
  int                 Y, left, right;
  int                 light = fg, dark = fg;
  unsigned short      red, green, blue, sl = 50, sd = 100;

  if (thick > 0 && fg >= 0)
    {
      if (style > 6 && align != 1)
        {
          /*  */
          TtaGiveThotRGB (fg, &red, &green, &blue);
          if (red && red < sd) sd = red;
          if (green && green < sd) sd = green;
          if (blue && blue < sd) sd = blue;
          dark = TtaGetThotColor (red - sd, green - sd, blue - sd);
          if (red + sl > 254) red = 255 - sl;
          if (green + sl > 254) green = 255 - sl;
          if (blue + sl > 254) blue = 255 - sl;
          light = TtaGetThotColor (red + sl, green + sl, blue + sl);
        }

      y += FrameTable[frame].FrTopMargin;
      if (style < 5 || thick < 2)
        {
          if (align == 1)
            Y = y + (h - thick) / 2;// middle
          else if (align == 2)
            Y = y + h - (thick + 1) / 2;// bottom
          else
            Y = y + thick / 2;// top
          
          InitDrawing (style, thick, fg);
          DoDrawOneLine (frame, x, Y, x + l, Y);
        }
      else
        {
          // check if the top of the box is displayed
          left = leftslice;
          right = rightslice;
          if (style == 7 || style == 8)
            {
              thick = thick / 2; // groove, ridge
              left = left / 2;
              right = right / 2;
            }
          else
            {
              thick--; // solid, outset inset, double
              if (left)
                left--;
              if (right)
                right--;
            }
          if (align == 1)
            {
              // middle
              point[0].x = x;
              point[0].y = y + (h - thick) / 2;
              point[1].x = x + l;
              point[1].y = y + (h - thick) / 2;
              point[2].x = x + l;
              point[2].y = y + (h + thick) / 2;
              point[3].x = x;
              point[3].y = y + (h + thick) / 2;
            }
          else if (align == 2)
            {
              // bottom
              if (style == 7 || style == 9)
                // groove or inset
                fg = light;
              else if (style == 8 || style == 10)
                // ridge or outset
                fg = dark;
              point[0].x = x + left;
              point[0].y = y + h - thick;
              point[1].x = x + l - right;
              point[1].y = y + h - thick;
              point[2].x = x + l;
              point[2].y = y + h;
              point[3].x = x;
              point[3].y = y + h;
            }
          else
            {
              // top
              if (style == 7 || style == 9)
                // groove or inset
                fg = dark;
              else if (style == 8 || style == 10)
                fg = light;
              point[0].x = x;
              point[0].y = y;
              point[1].x = x + l;
              point[1].y = y;
              point[2].x = x + l - right;
              point[2].y = y + thick;
              point[3].x = x + left;
              point[3].y = y + thick;
            }

          if (style == 6)
            {
              // double style
              InitDrawing (5, 1, fg);
              DoDrawOneLine (frame, (int)point[0].x, (int)point[0].y,
                             (int)point[1].x, (int)point[1].y);
              DoDrawOneLine (frame, (int)point[3].x, (int)point[3].y,
                             (int)point[2].x, (int)point[2].y);
            }
          else
            {
              GL_SetForeground (fg, TRUE);
              GL_DrawPolygon (point, 4, 0);
              if (align != 1 && (style == 7 || style == 8))
                {
                  // invert light and dark
                  if (fg == dark)
                    fg = light;
                  else
                    fg = dark;
                  if (align == 0)
                    {
                      // top
                      point[0].x = point[3].x + left;
                      point[0].y = point[3].y + thick;
                      point[1].x = point[2].x - right;
                      point[1].y = point[2].y + thick;
                    }
                  else
                    {
                      // bottom
                      point[3].x = point[0].x + left;
                      point[3].y = point[0].y - thick;
                      point[2].x = point[1].x - right;
                      point[2].y = point[1].y - thick;
                    }
                  GL_SetForeground (fg, TRUE);
                  GL_DrawPolygon (point, 4, 0);
                }
            }
        }
    }
}

/*----------------------------------------------------------------------
  DrawVerticalLine draw a vertical line
  align gives the current align left (0), middle(1), right(2):
  topslice and bottomslice say if the top and bottom borders are sliced.
  The style parameter is dotted (3), dashed (4), solid (5), etc.
  The parameter fg indicates the drawing color
  ----------------------------------------------------------------------*/
void DrawVerticalLine (int frame, int thick, int style, int x, int y,
                       int l, int h, int align, int fg, PtrBox box,
                       int topslice, int bottomslice)
{
  ThotPoint           point[4];
  int                 X, top = y, bottom = y + h;
  int                 light = fg, dark = fg;
  unsigned short      red, green, blue , sl = 50, sd = 100;

  if (thick > 0 && fg >= 0)
    {
      if (style > 6 && align != 1)
        {
          /*  */
          TtaGiveThotRGB (fg, &red, &green, &blue);
          if (red && red < sd) sd = red;
          if (green && green < sd) sd = green;
          if (blue && blue < sd) sd = blue;
          dark = TtaGetThotColor (red - sd, green - sd, blue - sd);
          if (red + sl > 254) red = 255 - sl;
          if (green + sl > 254) green = 255 - sl;
          if (blue + sl > 254) blue = 255 - sl;
          light = TtaGetThotColor (red + sl, green + sl, blue + sl);
        }

      y += FrameTable[frame].FrTopMargin;
      if (style < 5 || thick < 2)
        {
          if (align == 1)
            X = x + (l - thick) / 2;// midle
          else if (align == 2)
            X = x + l - thick / 2;// right
          else
            X = x + thick / 2;// left
            
          InitDrawing (style, thick, fg);
          DoDrawOneLine (frame, X, y, X, y + h);
        }
      else
        {
          // check if the top of the box is displayed
          top = topslice;
          bottom = bottomslice;
          if (style == 7 || style == 8)
            {
              thick = thick / 2; // groove, ridge
              top = top / 2;
              bottom = bottom / 2;
            }
          else
            {
              thick--; // solid, outset, inset style
              if (top)
                top--;
              if (bottom)
                bottom--;
            }
          if (align == 1)
            {
               // midle
              point[0].x = x + (l - thick) / 2;
              point[0].y = y;
              point[1].x = x + (l + thick) / 2;
              point[1].y = y;
              point[2].x = x + (l + thick) / 2;
              point[2].y = y + h;
              point[3].x = x + (l - thick) / 2;
              point[3].y = y + h;
            }
          else if (align == 2)
            {
              // right
              if (style == 7 || style == 9)
                // groove or inset
                fg = light;
              else if (style == 8 || style == 10)
                // ridge or outset
                fg = dark;
              point[0].x = x + l - thick;
              point[0].y = y + top;
              point[1].x = x + l;
              point[1].y = y;
              point[2].x = x + l;
              point[2].y = y + h;
              point[3].x = x + l - thick;
              point[3].y = y + h - bottom;
            }
          else
            {
              // left
              if (style == 7 || style == 9)
                // groove or inset
                fg = dark;
              else if (style == 8 || style == 10)
                // ridge or outset
                fg = light;
              point[0].x = x;
              point[0].y = y;
              point[1].x = x + thick;
              point[1].y = y + top;
              point[2].x = x + thick;
              point[2].y = y + h - bottom;
              point[3].x = x;
              point[3].y = y + h;
            }
          if (style == 6)
            {
              // double style
              InitDrawing (5, 1, fg);
              DoDrawOneLine (frame, (int)point[0].x, (int)point[0].y,
                             (int)point[3].x, (int)point[3].y);
              DoDrawOneLine (frame, (int)point[1].x, (int)point[1].y,
                             (int)point[2].x, (int)point[2].y);
            }
          else
            {
              GL_SetForeground (fg, TRUE);
              GL_DrawPolygon (point, 4, 0);
              if (align != 1 && (style == 7 || style == 8))
                {
                  // invert light and dark
                  if (fg == dark)
                    fg = light;
                  else
                    fg = dark;
                  if (align == 0)
                    {
                      // left
                      point[0].x = point[1].x + thick;
                      point[0].y = point[1].y + top;
                      point[3].x = point[2].x + thick;
                      point[3].y = point[2].y - bottom;
                    }
                  else
                    {
                      // right
                      point[1].x = point[0].x - thick;
                      point[1].y = point[0].y + top;
                      point[2].x = point[3].x - thick;
                      point[2].y = point[3].y - bottom;
                    }
                  GL_SetForeground (fg, TRUE);
                  GL_DrawPolygon (point, 4, 0);
                }
            }
        }
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
  DrawHorizontalParenthesis draw a horizontal parenthesis aligned top or bottom
  depending on align value.
  The parameter fg indicates the drawing color.
  ----------------------------------------------------------------------*/
void DrawHorizontalParenthesis (int frame, int thick, int style, int x, int y,
                          int l, int h, int align, int fg)
{
  h -= thick;
  l -= thick;
  x += thick / 2;
  y +=  FrameTable[frame].FrTopMargin;
  y += thick / 2;
  if (thick > 0 && fg >= 0)
    {
      InitDrawing (style, thick, fg);
      if (align)
        GL_DrawArc(x, y + h, l, -h, 0, 180, 0, FALSE);
      else
        GL_DrawArc(x, y, l, h, 0, 180, 0, FALSE);
    }
}

/*----------------------------------------------------------------------
  DrawHorizontalBrace draw a horizontal brace aligned top or bottom
  depending on align value.
  The parameter fg indicates the drawing color.
  ----------------------------------------------------------------------*/
void DrawHorizontalBrace (int frame, int thick, int style, int x, int y,
                          int l, int h, int align, int fg)
{
  ThotSegment         seg[6];
  int                 Y, X;

  if (thick > 0 && fg >= 0)
    {
      y += FrameTable[frame].FrTopMargin;
      Y = y + (h - thick) / 2;
      X = x + (l / 2);
      if (align == 0)
        /* Over brace */
        {
          seg[0].x1 = x;
          seg[0].y1 = y + h;
          seg[0].x2 = x + thick;
          seg[0].y2 = Y;

          seg[1].x1 = seg[0].x2;
          seg[1].y1 = seg[0].y2;
          seg[1].x2 = X - thick;
          seg[1].y2 = Y;

          seg[2].x1 = seg[1].x2;
          seg[2].y1 = seg[1].y2;
          seg[2].x2 = X;
          seg[2].y2 = y;

          seg[3].x1 = seg[2].x2;
          seg[3].y1 = seg[2].y2;
          seg[3].x2 = X + thick;
          seg[3].y2 = Y;

          seg[4].x1 = seg[3].x2;
          seg[4].y1 = seg[3].y2;
          seg[4].x2 = x + l - thick;
          seg[4].y2 = Y;

          seg[5].x1 = seg[4].x2;
          seg[5].y1 = seg[4].y2;
          seg[5].x2 = x + l;
          seg[5].y2 = y + h;
          InitDrawing (style, thick, fg);
          GL_DrawSegments (seg, 6);
        }
      else
        /* Under brace */
        {
          seg[0].x1 = x;
          seg[0].y1 = y;
          seg[0].x2 = x + thick;
          seg[0].y2 = Y;

          seg[1].x1 = seg[0].x2;
          seg[1].y1 = seg[0].y2;
          seg[1].x2 = X - thick;
          seg[1].y2 = Y;

          seg[2].x1 = seg[1].x2;
          seg[2].y1 = seg[1].y2;
          seg[2].x2 = X;
          seg[2].y2 = y + h;

          seg[3].x1 = seg[2].x2;
          seg[3].y1 = seg[2].y2;
          seg[3].x2 = X + thick;
          seg[3].y2 = Y;

          seg[4].x1 = seg[3].x2;
          seg[4].y1 = seg[3].y2;
          seg[4].x2 = x + l - thick;
          seg[4].y2 = Y;

          seg[5].x1 = seg[4].x2;
          seg[5].y1 = seg[4].y2;
          seg[5].x2 = x + l;
          seg[5].y2 = y;
          InitDrawing (style, thick, fg);
          GL_DrawSegments (seg, 6);
        }
    }
}

/*----------------------------------------------------------------------
  DrawHorizontalBracket draw a horizontal bracket aligned top or bottom
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
  GL_DrawLines (point, 3);
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
  int                 i;
  int                 arc, arc2, xf, yf;
  ThotArc             xarc[4];
  ThotSegment         seg[5];
  ThotPoint           point[13];

  y = y + FrameTable[frame].FrTopMargin;

  width -= thick;
  height -= thick;
  x += thick / 2;
  y = y + thick / 2;

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
  if (pattern == 2)
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

      GL_SetForeground (bg, TRUE);
      GL_DrawPolygon (point, 13, 0);
      for (i = 0; i < 4; i++)
        {  
          GL_DrawArc	(xarc[i].x, xarc[i].y, 
                       xarc[i].width, xarc[i].height, 
                       xarc[i].angle1, xarc[i].angle2,
                       0, TRUE);
        }
    }

  /* Draw the border */
  if (thick > 0 && fg >= 0)
    {
      InitDrawing (style, thick, fg);
      for (i = 0 ; i < 4; i++)
        {  
          GL_DrawArc (xarc[i].x, xarc[i].y, 
                      xarc[i].width, xarc[i].height, 
                      xarc[i].angle1, xarc[i].angle2,
                      0, FALSE); 
        }
      if (arc2 < height / 2)
        GL_DrawSegments(seg, 5);
      else
        GL_DrawSegments(seg, 4);
    }
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
  ThotPoint           point[2];
  int                 px7mm, shiftX;
  double              A;

  width -= thick + 1;
  height -= thick + 1;
  x += thick / 2;
  y += thick / 2;
  y +=  FrameTable[frame].FrTopMargin;

  /* Fill in the rectangle */
  if (pattern == 2)
    {
      GL_SetForeground (bg, TRUE);
      GL_DrawArc (x, y, width, height, 0, 360, 0, TRUE);
    }

  /* Draw the border */
  if (thick > 0 && fg >= 0)
    {
      InitDrawing (style, thick, fg);
      GL_DrawArc(x, y, width, height, 0, 360, 0, FALSE); 
      px7mm = (int)((7 * DOT_PER_INCH) / 25.4 + 0.5);
      if (height > 2 * px7mm)
        {
          A = ((double) height - 2 * px7mm) / height;
          A = 1.0 - sqrt (1 - A * A);
          shiftX = (int)(width * A * 0.5 + 0.5);
          point[0].x = x + shiftX;
          point[0].y = y + px7mm;
          point[1].x = x + width - shiftX;
          point[1].y = y + px7mm;
          GL_DrawLines (point, 1);
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
		   size*2+1, size*2+1, fg, bg, 2);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void DrawResizeTriangle (int frame, int size, int x_point, int y_point,
			 int bg, int fg, int direction)
{
  ThotPoint points[4];
  int size2;

  size2 = (int)(size/sqrt((double)2));
      
  switch(direction)
    {
    case 0: /* N */
      points[0].x = x_point;
      points[0].y = y_point;
      points[1].x = x_point - size2;
      points[1].y = y_point + size;
      points[2].x = x_point + size2;
      points[2].y = y_point + size;
      break;

    case 1: /* W */
      points[0].x = x_point;
      points[0].y = y_point;
      points[1].x = x_point + size;
      points[1].y = y_point - size2;
      points[2].x = x_point + size;
      points[2].y = y_point + size2;
      break;

    case 2: /* S */
      points[0].x = x_point;
      points[0].y = y_point;
      points[1].x = x_point - size2;
      points[1].y = y_point - size;
      points[2].x = x_point + size2;
      points[2].y = y_point - size;
      break;

    case 3: /* E */
      points[0].x = x_point;
      points[0].y = y_point;
      points[1].x = x_point - size;
      points[1].y = y_point - size2;
      points[2].x = x_point - size;
      points[2].y = y_point + size2;
      break;

    case 4: /* NW */
      points[0].x = x_point;
      points[0].y = y_point + size;
      points[1].x = x_point;
      points[1].y = y_point;
      points[2].x = x_point + size;
      points[2].y = y_point;
      break;

    case 5: /* NE */
      points[0].x = x_point;
      points[0].y = y_point + size;
      points[1].x = x_point;
      points[1].y = y_point;
      points[2].x = x_point - size;
      points[2].y = y_point;
      break;

    case 6: /* SW */
      points[0].x = x_point + size;
      points[0].y = y_point;
      points[1].x = x_point;
      points[1].y = y_point;
      points[2].x = x_point;
      points[2].y = y_point - size;
      break;

    case 7: /* SE */
      points[0].x = x_point - size;
      points[0].y = y_point;
      points[1].x = x_point;
      points[1].y = y_point;
      points[2].x = x_point;
      points[2].y = y_point - size;
      break;

    default:
      return;
      break;
    }

  points[3] = points[0];
  DoDrawLines (frame, 1, 5, points, 4, fg, bg, 2, 0);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void DrawTriangle (int frame, int thick, int style, int fg, int bg, int pattern,
		   int x1, int y1,
		   int x2, int y2,
		   int x3, int y3)
{
  ThotPoint points[4];

  y1 += FrameTable[frame].FrTopMargin;
  y2 += FrameTable[frame].FrTopMargin;
  y3 += FrameTable[frame].FrTopMargin;

  points[0].x = x1;
  points[0].y = y1;
  points[1].x = x2;
  points[1].y = y2;
  points[2].x = x3;
  points[2].y = y3;
  points[3] = points[0];

  DoDrawLines (frame, thick, style, points, 4, fg, bg, pattern, 0);
}


/*----------------------------------------------------------------------
  WChaine draw a string in frame, at location (x, y) and using font.
  ----------------------------------------------------------------------*/
void WChaine (ThotWindow w, char *string, int x, int y, ThotFont font,
              ThotGC GClocal)
{

  /* GL_DRAWSTRING*/
}


/*----------------------------------------------------------------------
  VideoInvert switch to inverse video the area of frame located at
  (x,y) and of size width x height.
  ----------------------------------------------------------------------*/
void VideoInvert (int frame, int width, int height, int x, int y)
{
  GL_VideoInvert(width, height, x, y + FrameTable[frame].FrTopMargin);
}


/*----------------------------------------------------------------------
  Scroll do a scrolling/Bitblt of frame of a width x height area
  from (xd,yd) to (xf,yf).
  ----------------------------------------------------------------------*/
void Scroll (int frame, int width, int height, int xd, int yd, int xf, int yf)
{
#ifndef _WX
  if (FrRef[frame] != None)
    GL_window_copy_area (frame,
                         xf, 
                         yf + FrameTable[frame].FrTopMargin,
                         xd, 
                         yd + FrameTable[frame].FrTopMargin,
                         width, 
                         height);
#else /* _WX */
  if (FrameTable[frame].WdFrame != None)
    GL_window_copy_area (frame,
                         xf, 
                         yf + FrameTable[frame].FrTopMargin,
                         xd, 
                         yd + FrameTable[frame].FrTopMargin,
                         width, 
                         height);
#endif /* _WX */
}


/*----------------------------------------------------------------------
  PaintWithPattern fill the rectangle associated to a window w (or frame
  if w = 0) located on (x , y) and geometry width x height, using the
  given pattern.
  Parameters fg, bg, and pattern are for drawing color, background color
  and fill pattern.
  ----------------------------------------------------------------------*/
void PaintWithPattern (int frame, int x, int y, int width, int height,
                       ThotWindow w, int fg, int bg, int pattern)
{
  /* Fill the rectangle associated to the given frame */
  if (pattern == 2)
    {
      if (w == 0)
        y += FrameTable[frame].FrTopMargin;
      GL_DrawRectangle (pattern, (float) x, (float) y,
                        (float) width, (float) height);
    }
}
#endif /* _GL*/
