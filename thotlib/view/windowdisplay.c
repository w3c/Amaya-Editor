/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 
/*
 * windowdisplay.c : handling of low level drawing routines, both for
 *                   X-Window and MS-Windows (incomplete).
 *
 * Authors: I. Vatton (INRIA)
 *          R. Guetari and D. Veillard (W3C/INRIA) - Windows 95/NT routines
 *
 */

#include "thot_sys.h"
#ifdef SYSV
#endif
#include "constmedia.h"
#include "typemedia.h"
#include "frame.h"


#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "font_tv.h"
#include "frame_tv.h"
#include "units_tv.h"
#include "edit_tv.h"
#include "thotcolor_tv.h"

extern ThotColorStruct cblack;

#define	MAX_STACK	50
#define	MIDDLE_OF(v1, v2)	(((v1)+(v2))/2.0)
#define SEG_SPLINE      5
#define ALLOC_POINTS    300
static ThotPoint   *points;	/* control points for curbs */
static int          npoints;
static int          MAX_points;

typedef struct stack_point
  {
     float               x1, y1, x2, y2, x3, y3, x4, y4;
  }
StackPoint;
static StackPoint   stack[MAX_STACK];
static int          stack_deep;

#include "font_f.h"
#include "units_f.h"
#include "context_f.h"
#include "memory_f.h"
#include "inites_f.h"
#include "buildlines_f.h"

/*----------------------------------------------------------------------
  FontOrig update and (x, y) location before DrawString
  accordingly to the ascent of the font used.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                FontOrig (ptrfont font, char firstchar, int *pX, int *pY)
#else  /* __STDC__ */
void                FontOrig (font, firstchar, pX, pY)
ptrfont             font;
char                firstchar;
int                *pX;
int                *pY;

#endif /* __STDC__ */
{
   if (!font)
      return;
#ifndef _WINDOWS
   *pY += ((XFontStruct *) font)->ascent;
#endif /* _WINDOWS */
}


/*----------------------------------------------------------------------
  LoadColor load the given color in the drawing Graphic Context.
  RO indicates whether it's a read-only box active
  indicates if the box is active parameter fg indicates the drawing color
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         LoadColor (int disp, int RO, int active, int fg)
#else  /* __STDC__ */
static void         LoadColor (disp, RO, active, fg)
int                 disp;
int                 RO;
int                 active;
int                 fg;

#endif /* __STDC__ */
{
#  ifdef _WINDOWS
   TtLineGC.capabilities |= THOT_GC_FOREGROUND;
   if (RO && ShowReadOnly () && fg == 1)
      TtLineGC.foreground = RO_Color;
   else
       TtLineGC.foreground = fg;
#  else  /* _WINDOWS */
   if (active && ShowReference ())
     {
	if (TtWDepth == 1)
	   /* Modify the fill style of the characters */
	   XSetFillStyle (TtDisplay, TtLineGC, FillTiled);
	else
	   /* Modify the color of the active boxes */
	   XSetForeground (TtDisplay, TtLineGC, Box_Color);
     }
   else if (RO && ShowReadOnly () && ColorPixel (fg) == cblack.pixel)
      /* Color of ReadOnly parts */
      XSetForeground (TtDisplay, TtLineGC, RO_Color);
   else
      /* Color of the box */
      XSetForeground (TtDisplay, TtLineGC, ColorPixel (fg));
#  endif /* _WINDOWS */
}


/*----------------------------------------------------------------------
  InitDrawing update the Graphic Context accordingly to parameters.
  RO indicates whether it's a read-only box active
  indicates if the box is active parameter fg indicates the drawing color
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         InitDrawing (int disp, int style, int thick, int RO, int active, int fg)
#else  /* __STDC__ */
static void         InitDrawing (disp, style, thick, RO, active, fg)
int                 disp;
int                 style;
int                 thick;
int                 RO;
int                 active;
int                 fg;

#endif /* __STDC__ */
{
#  ifndef _WINDOWS
    char                dash[2];
  if (style == 0)
      XSetLineAttributes (TtDisplay, TtLineGC, thick, LineSolid, CapButt, JoinMiter);
   else
     {
	dash[0] = (char) (style * 4);
	dash[1] = (char) 4;
	XSetDashes (TtDisplay, TtLineGC, 0, dash, 2);
	XSetLineAttributes (TtDisplay, TtLineGC, thick, LineOnOffDash, CapButt, JoinMiter);
     }
#  endif /* _WINDOWS */
   /* Load the correct color */
    LoadColor (disp, RO, active, fg);
}


/*----------------------------------------------------------------------
      FinishDrawing update the Graphic Context accordingly to parameters.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         FinishDrawing (int disp, int RO, int active)
#else  /* __STDC__ */
static void         FinishDrawing (disp, RO, active)
int                 disp;
int                 RO;
int                 active;

#endif /* __STDC__ */
{
#  ifndef _WINDOWS
   if (TtWDepth == 1 && (active || RO))
      XSetFillStyle (TtDisplay, TtLineGC, FillSolid);
#  endif /* _WINDOWS */
}


/*----------------------------------------------------------------------
  DoDrawOneLine draw one line starting from (x1, y1) to (x2, y2) in frame.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         DoDrawOneLine (int frame, int x1, int y1, int x2, int y2)
#else  /* __STDC__ */
static void         DoDrawOneLine (frame, x1, y1, x2, y2)
int                 frame;
int                 x1;
int                 y1;
int                 x2;
int                 y2;

#endif /* __STDC__ */
{
#  ifdef _WINDOWS 
   HPEN pen ;
   HPEN hOldPen;
#  endif /* _WINDOWS  */
   x1 += FrameTable[frame].FrLeftMargin;
   y1 += FrameTable[frame].FrTopMargin;
   x2 += FrameTable[frame].FrLeftMargin;
   y2 += FrameTable[frame].FrTopMargin;
#  ifdef _WINDOWS
   WIN_GetDeviceContext (frame);
   pen = CreatePen (PS_SOLID, 1, RGB (RGB_colors[TtLineGC.foreground].red, RGB_colors[TtLineGC.foreground].green, RGB_colors[TtLineGC.foreground].blue));
   hOldPen = SelectObject (TtDisplay, pen);

   MoveToEx (TtDisplay, x1, y1, NULL);
   LineTo (TtDisplay, x2, y2);
   SelectObject (TtDisplay, hOldPen);
   WIN_ReleaseDeviceContext ();
   if (!DeleteObject (pen))
      WinErrorBox (WIN_Main_Wd);
#  else  /* _WINDOWS */
   XDrawLine (TtDisplay, FrRef[frame], TtLineGC, x1, y1, x2, y2);
#  endif /* _WINDOWS */
}


/*----------------------------------------------------------------------
  SpaceToChar substitute in text the space chars to their visual
  equivalents.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         SpaceToChar (unsigned char *text)
#else  /* __STDC__ */
static void         SpaceToChar (text)
unsigned char      *text;

#endif /* __STDC__ */
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
		    text[i] = (unsigned char) SHOWN_BREAK_LINE;
		    break;
		 case THIN_SPACE:
		    text[i] = (unsigned char) SHOWN_THIN_SPACE;
		    break;
		 case HALF_EM:
		    text[i] = (unsigned char) SHOWN_HALF_EM;
		    break;
		 case UNBREAKABLE_SPACE:
		    text[i] = (unsigned char) SHOWN_UNBREAKABLE_SPACE;
		    break;
		 case _SPACE_:
		    text[i] = (unsigned char) SHOWN_SPACE;
		    break;
	      }
	i++;
     }
}

#ifndef _WIN_PRINT
/*----------------------------------------------------------------------
  DrawChar draw a char at location (x, y) in frame and with font.
  RO indicates whether it's a read-only box active
  indicates if the box is active parameter fg indicates the drawing color
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                DrawChar (unsigned char car, int frame, int x, int y, ptrfont font, int RO, int active, int fg)
#else  /* __STDC__ */
void                DrawChar (car, frame, x, y, font, RO, active, fg)
unsigned char       car;
int                 frame;
int                 x;
int                 y;
ptrfont             font;
int                 RO;
int                 active;
int                 fg;

#endif /* __STDC__ */
{
   ThotWindow          w;
#  ifdef _WINDOWS
   char                str[2] = {car, 0};
   HFONT               hOldFont;
#  endif /* _WINDOWS */

   w = FrRef[frame];
   if (w == None)
      return;

   LoadColor (0, RO, active, fg);

#  ifdef _WINDOWS
   WIN_GetDeviceContext (frame);
   WinLoadGC (fg, RO);
   SetMapperFlags (TtDisplay, 1);
   hOldFont = WinLoadFont (TtDisplay, font);
   TextOut (TtDisplay, x + FrameTable[frame].FrLeftMargin, y + FrameTable[frame].FrTopMargin, (unsigned char*) str, 1);   
   SelectObject (TtDisplay, hOldFont);
   WIN_ReleaseDeviceContext ();
#  else  /* _WINDOWS */
   XSetFont (TtDisplay, TtLineGC, ((XFontStruct *) font)->fid);
   XDrawString (TtDisplay, w, TtLineGC, x + FrameTable[frame].FrLeftMargin, y + FrameTable[frame].FrTopMargin + FontBase (font), &car, 1);
#  endif /* _WINDOWS */

   FinishDrawing (0, RO, active);
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
  RO indicates whether it's a read-only box
  active indicates if the box is active
  parameter fg indicates the drawing color
  
  Returns the lenght of the string drawn.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                 DrawString (char *buff, int i, int lg, int frame, int x, int y, ptrfont font, int lgboite, int bl, int hyphen, int debutbloc, int RO, int active, int fg)
#else  /* __STDC__ */
int                 DrawString (buff, i, lg, frame, x, y, font, lgboite, bl, hyphen, debutbloc, RO, active, fg)
char               *buff;
int                 i;
int                 lg;
int                 frame;
int                 x;
int                 y;
ptrfont             font;
int                 lgboite;
int                 bl;
int                 hyphen;
int                 debutbloc;
int                 RO;
int                 active;
int                 fg;

#endif /* __STDC__ */
{
   ThotWindow          w;
   char               *ptcar;
   int                 width;
#  ifndef _WINDOWS
   register int        j;
#  endif /* !_WINDOWS */

#  ifdef _WINDOWS
   SIZE                size;
   RECT                rect;
   HFONT               hOldFont;
#  endif

   w = FrRef[frame];

   if (lg > 0 && w != None) {
      ptcar = &buff[i - 1];
      /* Dealing with BR tag for windows */
#     ifdef _WINDOWS
      WIN_GetDeviceContext (frame);
      SetMapperFlags (TtDisplay, 1);
      hOldFont = WinLoadFont (TtDisplay, font);
      /* GetTextExtentPoint32(TtDisplay, ptcar, lg, &size); */
      GetTextExtentPoint (TtDisplay, ptcar, lg, &size);
      width = size.cx;
#     else  /* _WINDOWS */
      XSetFont (TtDisplay, TtLineGC, ((XFontStruct *) font)->fid);
      /* compute the width of the string */
      width = 0;
      j = 0;
      while (j < lg)
            width += CharacterWidth (ptcar[j++], font);
#     endif /* !_WINDOWS */

#     ifndef _WINDOWS 
      LoadColor (0, RO, active, fg);
#     else /* _WINDOWS */
	  WinLoadGC (fg, RO);
#     endif /* _WINDOWS */

      if (!ShowSpace) {
         /* draw the spaces */
         ptcar = TtaGetMemory (lg + 1);
         strncpy (ptcar, &buff[i - 1], lg);
         ptcar[lg] = EOS;
	     SpaceToChar (ptcar);	/* substitute spaces */
#        ifdef _WINDOWS
         GetClientRect (TtDisplay, &rect);
         TextOut (TtDisplay, x + FrameTable[frame].FrLeftMargin, y + FrameTable[frame].FrTopMargin, (unsigned char*) ptcar, lg);
#        else  /* _WINDOWS */
         XDrawString (TtDisplay, w, TtLineGC, x + FrameTable[frame].FrLeftMargin, y + FrameTable[frame].FrTopMargin + FontBase (font), ptcar, lg);
#        endif /* _WINDOWS */
         TtaFreeMemory (ptcar);
      } else {
           if (ptcar[0] == '\212') {
              /* skip the Control return char */
              ptcar++;
             lg--;
           }
           if (lg != 0) {
#             ifdef _WINDOWS
              TextOut (TtDisplay, x + FrameTable[frame].FrLeftMargin, y + FrameTable[frame].FrTopMargin, (unsigned char*) ptcar, lg);
#             else  /* _WINDOWS */
	          XDrawString (TtDisplay, w, TtLineGC, x + FrameTable[frame].FrLeftMargin, y + FrameTable[frame].FrTopMargin + FontBase (font), ptcar, lg);
#             endif /* _WINDOWS */
           }
      }
      if (hyphen) {
         /* draw the hyphen */
#        ifdef _WINDOWS
         TextOut (TtDisplay, x + width + FrameTable[frame].FrLeftMargin, y + FrameTable[frame].FrTopMargin, "\255", 1);
#        else  /* _WINDOWS */
         XDrawString (TtDisplay, w, TtLineGC, x + width + FrameTable[frame].FrLeftMargin,
         y + FrameTable[frame].FrTopMargin + FontBase (font), "\255", 1);
#        endif /* _WINDOWS */
      }
      FinishDrawing (0, RO, active);
#     ifdef _WINDOWS
	  SelectObject (TtDisplay, hOldFont);
      WIN_ReleaseDeviceContext ();
#     endif /* _WINDOWS */

      return (width);
   } else
         return (0);
}

/*----------------------------------------------------------------------
  DisplayUnderline draw the underline, overline or cross line
  added to some text of lenght lg, using font and located
  at (x, y) in frame. 
  RO indicates whether it's a read-only box
  active indicates if the box is active
  parameter fg indicates the drawing color
  thick indicates thickness : thin (0) thick (1)
  Type indicates the kind of drawing :
  - 0 = none
  - 1 = underlined
  - 2 = overlined
  - 3 = cross-over
  
                  (x,y)
          __________________+_______________________________\_/__ height
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
#ifdef __STDC__
void                DisplayUnderline (int frame, int x, int y, ptrfont font, int type, int thick, int lg, int RO, int active, int fg)
#else  /* __STDC__ */
void                DisplayUnderline (frame, x, y, font, type, thick, lg, RO, active, fg)
int                 frame;
int                 x;
int                 y;
ptrfont             font;
int                 type;
int                 thick;
int                 lg;
int                 RO;
int                 active;
int                 fg;

#endif /* __STDC__ */
{
   ThotWindow          w;
   int                 fheight;	/* font height           */
   int                 ascent;	/* font ascent           */
   int                 bottom;	/* underline position    */
   int                 middle;	/* cross-over position   */
   int                 height;	/* overline position     */
   int                 thickness;	/* thickness of drawing */
   int                 shift;	/* shifting of drawing   */

   if (lg > 0)
     {
	w = FrRef[frame];

	if (w == None)
	   return;
	fheight = FontHeight (font);
	ascent = FontAscent (font);
	thickness = ((fheight / 20) + 1) * (thick + 1);
	shift = thick * thickness;
	height = y + shift;
	bottom = y + ascent + 2 + shift;
	middle = y + fheight / 2 - shift;

	/*
	 * for an underline independant of the font add
	 * the following lines here :
	 *         thickness = 1;
	 *         height = y + 2 * thickness;
	 *         bottom = y + ascent + 3;
	 */

	InitDrawing (0, 0, thickness, RO, active, fg);
	switch (type)
	      {
		 case 1:
		    /* underlined */
		    DoDrawOneLine (frame, x - lg, bottom, x, bottom);
		    break;

		 case 2:
		    /* overlined */
		    DoDrawOneLine (frame, x - lg, height, x, height);
		    break;

		 case 3:
		    /* cross-over */
		    DoDrawOneLine (frame, x - lg, middle, x, middle);
		    break;

		 default:
		    /* not underlined */
		    break;
	      }
	FinishDrawing (0, RO, active);
     }
}

/*----------------------------------------------------------------------
  DrawPoints draw a line of dot.
  RO indicates whether it's a read-only box active indicates if the box
  is active parameter fg indicates the drawing color
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                DrawPoints (int frame, int x, int y, int lgboite, int RO, int active, int fg)
#else  /* __STDC__ */
void                DrawPoints (frame, x, y, lgboite, RO, active, fg)
int                 frame;
int                 x;
int                 y;
int                 lgboite;
int                 RO;
int                 active;
int                 fg;

#endif /* __STDC__ */
{
   ThotWindow          w;
   ptrfont             font;
   int                 xcour, width, nb;
   char               *ptcar;

   font = ThotLoadFont ('L', 't', 0, 6, UnPoint, frame);
   if (lgboite > 0)
     {
	w = FrRef[frame];

	ptcar = " .";

	/* compute lenght of the string " ." */
	width = CharacterWidth (' ', font) + CharacterWidth ('.', font);

	/* compute the number of string to write */
	nb = lgboite / width;
	xcour = x + FrameTable[frame].FrLeftMargin + (lgboite % width);
	y += FrameTable[frame].FrTopMargin - FontBase (font);
#       ifndef _WINDOWS
	XSetFont (TtDisplay, TtLineGC, ((XFontStruct *) font)->fid);
#       endif /* _WINDOWS */
	LoadColor (0, RO, active, fg);

	/* draw the points */
	FontOrig (font, *ptcar, &x, &y);
	while (nb > 0)
	  {
#            ifndef _WINDOWS
	     XDrawString (TtDisplay, w, TtLineGC, xcour, y, ptcar, 2);
#            endif /* _WINDOWS */
	     xcour += width;
	     nb--;
	  }
	FinishDrawing (0, RO, active);
     }
}

/*----------------------------------------------------------------------
  DrawRadical Draw a radical symbol.
  RO indicates whether it's a read-only box active indicates if the box
  is active parameter fg indicates the drawing color
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                DrawRadical (int frame, int thick, int x, int y, int l, int h, ptrfont font, int RO, int active, int fg)

#else  /* __STDC__ */
void                DrawRadical (frame, thick, x, y, l, h, font, RO, active, fg)
int                 frame;
int                 thick;
int                 x;
int                 y;
int                 l;
int                 h;
ptrfont             font;
int                 RO;
int                 active;
int                 fg;

#endif /* __STDC__ */

{
   int                 xm, xp, fh;

   fh = FontHeight (font);
   xm = x + (fh / 2);
   xp = x + (fh / 4);
   InitDrawing (0, 0, 0, RO, active, fg);
   /* vertical part */
   DoDrawOneLine (frame, x, y + (2 * (h / 3)), xp - (thick / 2), y + h);

   InitDrawing (0, 0, thick, RO, active, fg);
   /* Acending part */
   DoDrawOneLine (frame, xp, y + h, xm, y);
   /* Upper part */
   DoDrawOneLine (frame, xm, y, x + l, y);
   FinishDrawing (0, RO, active);
}

/*----------------------------------------------------------------------
  DrawIntegral draw an integral. depending on type :
  - simple if type = 0
  - contour if type = 1
  - double if type = 2.
  RO indicates whether it's a read-only box
  active indicates if the box is active
  parameter fg indicates the drawing color
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                DrawIntegral (int frame, int thick, int x, int y, int l, int h, int type, ptrfont font, int RO, int active, int fg)
#else  /* __STDC__ */
void                DrawIntegral (frame, thick, x, y, l, h, type, font, RO, active, fg)
int                 frame;
int                 thick;
int                 x;
int                 y;
int                 l;
int                 h;
int                 type;
ptrfont             font;
int                 RO;
int                 active;
int                 fg;
#endif /* __STDC__ */
{
   int                 xm, yf, yend, exnum, delta;
   int                 wd, asc, hd;

   exnum = 0;
   if (FontHeight (font) *1.2 >= h)
     /* display a single glyph */
     {
     xm = x + ((l - CharacterWidth ('\362', font)) / 2);
     yf = y + ((h - CharacterHeight ('\362', font)) / 2) - FontAscent (font) +
	  CharacterAscent ('\362', font);
     DrawChar ('\362', frame, xm, yf, font, RO, active, fg);
     }
   else
     {
     /* Need more than one glyph */
     xm = x + ((l - CharacterWidth ('\363', font)) / 2);
     yf = y - FontAscent (font) + CharacterAscent ('\363', font);
     DrawChar ('\363', frame, xm, yf, font, RO, active, fg);
     yend = y + h - CharacterHeight ('\365', font) - FontAscent (font) +
	    CharacterAscent ('\365', font) - 1;
     DrawChar ('\365', frame, xm, yend, font, RO, active, fg);

     yf += CharacterHeight ('\363', font);
     delta = yend - yf;
     asc = CharacterAscent ('\364', font)  - FontAscent (font) - 1;
     hd = CharacterHeight ('\364', font) - 1;
     wd = (CharacterWidth ('\363', font) - CharacterWidth ('\364', font)) / 2;
     if (delta >= 0)
       {
	for (yf += asc,
	     yend -= hd;
	     yf < yend;
	     yf += CharacterHeight ('\364', font), exnum++)
	   DrawChar ('\364', frame, xm+wd, yf, font, RO, active, fg);
	if (exnum)
	   DrawChar ('\364', frame, xm+wd, yend, font, RO, active, fg);
	else
	   DrawChar ('\364', frame, xm+wd, yf + ((delta - hd) / 2), font, RO, active, fg);
       }
     }

   if (type == 2)		/* double integral */
      DrawIntegral (frame, thick, x + (CharacterWidth ('\364', font) / 2),
		    y, l, h, -1, font, RO, active, fg);

   else if (type == 1)		/* contour integral */
      DrawChar ('o', frame, x + ((l - CharacterWidth ('o', font)) / 2),
		y + (h - CharacterHeight ('o', font)) / 2 - FontAscent (font) + CharacterAscent ('o', font),
		font, RO, active, fg);
}
#endif /* _WIN_PRINT */

/*----------------------------------------------------------------------
  DrawMonoSymb draw a one glyph symbol.
  RO indicates whether it's a read-only box
  active indicates if the box is active
  parameter fg indicates the drawing color
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         DrawMonoSymb (char symb, int frame, int x, int y, int l, int h, int RO, int active, ptrfont font, int fg)
#else  /* __STDC__ */
static void         DrawMonoSymb (symb, frame, x, y, l, h, RO, active, font, fg)
char                symb;
int                 frame;
int                 x;
int                 y;
int                 l;
int                 h;
int                 RO;
int                 active;
ptrfont             font;
int                 fg;
#endif /* __STDC__ */
{
   int                 xm, yf;

   xm = x + ((l - CharacterWidth (symb, font)) / 2);
   yf = y + ((h - CharacterHeight (symb, font)) / 2) - FontAscent (font) + CharacterAscent (symb, font);

   DrawChar (symb, frame, xm, yf, font, RO, active, fg);
}

#ifndef _WIN_PRINT
/*----------------------------------------------------------------------
  DrawSigma draw a Sigma symbol.
  active indicates if the box is active
  parameter fg indicates the drawing color
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                DrawSigma (int frame, int x, int y, int l, int h, ptrfont font, int RO, int active, int fg)
#else  /* __STDC__ */
void                DrawSigma (frame, x, y, l, h, font, RO, active, fg)
int                 frame;
int                 x;
int                 y;
int                 l;
int                 h;
ptrfont             font;
int                 RO;
int                 active;
int                 fg;
#endif /* __STDC__ */
{
   int                 xm, ym, fh;

   fh = FontHeight (font);
   if (h < fh * 2 && l <= CharacterWidth ('\345', font))
     {
	/* Only one glyph needed */
	DrawMonoSymb ('\345', frame, x, y, l, h, RO, active, font, fg);
     }
   else
     {
	xm = x + (l / 3);
	ym = y + (h / 2) - 1;
	InitDrawing (0, 0, 0, RO, active, fg);
	/* Center */
	DoDrawOneLine (frame, x, y + 1, xm, ym);
	DoDrawOneLine (frame, x, y + h - 2, xm, ym);

	InitDrawing (0, 0, 2, RO, active, fg);
	/* Borders */
	DoDrawOneLine (frame, x, y, x + l, y);
	DoDrawOneLine (frame, x, y + h - 2, x + l, y + h - 2);
	FinishDrawing (0, RO, active);
     }
}

/*----------------------------------------------------------------------
  DrawPi draw a PI symbol.
  RO indicates whether it's a read-only box
  active indicates if the box is active
  parameter fg indicates the drawing color
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                DrawPi (int frame, int x, int y, int l, int h, ptrfont font, int RO, int active, int fg)
#else  /* __STDC__ */
void                DrawPi (frame, x, y, l, h, font, RO, active, fg)
int                 frame;
int                 x;
int                 y;
int                 l;
int                 h;
ptrfont             font;
int                 RO;
int                 active;
int                 fg;
#endif /* __STDC__ */
{
   int                 fh;

   fh = FontHeight (font);
   if (h < fh * 2 && l <= CharacterWidth ('\325', font))
     {
	/* Only one glyph needed */
	DrawMonoSymb ('\325', frame, x, y, l, h, RO, active, font, fg);
     }
   else
     {
	InitDrawing (0, 0, 0, RO, active, fg);
	/* Vertical part */
	DoDrawOneLine (frame, x + 2, y + 1, x + 2, y + h);
	DoDrawOneLine (frame, x + l - 3, y + 1, x + l - 3, y + h);

	InitDrawing (0, 0, 2, RO, active, fg);
	/* Upper part */
	DoDrawOneLine (frame, x + 1, y + 1, x + l, y);
	FinishDrawing (0, RO, active);
     }
}

/*----------------------------------------------------------------------
  DrawIntersection draw an intersection symbol.
  RO indicates whether it's a read-only box
  active indicates if the box is active
  parameter fg indicates the drawing color
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                DrawIntersection (int frame, int x, int y, int l, int h, ptrfont font, int RO, int active, int fg)
#else  /* __STDC__ */
void                DrawIntersection (frame, x, y, l, h, font, RO, active, fg)
int                 frame;
int                 x;
int                 y;
int                 l;
int                 h;
ptrfont             font;
int                 RO;
int                 active;
int                 fg;
#endif /* __STDC__ */
{
   int                 arc, fh;

   fh = FontHeight (font);
   if (h < fh * 2 && l <= CharacterWidth ('\307', font))
     {
	/* Only one glyph needed */
	DrawMonoSymb ('\307', frame, x, y, l, h, RO, active, font, fg);
     }
   else
     {
	/* radius of arcs is 6mm */
	arc = h / 4;
	InitDrawing (0, 0, 2, RO, active, fg);
	/* vertical part */
	DoDrawOneLine (frame, x + 1, y + arc, x + 1, y + h);
	DoDrawOneLine (frame, x + l - 2, y + arc, x + l - 2, y + h);

	/* Upper part */
#       ifndef _WINDOWS
	XDrawArc (TtDisplay, FrRef[frame], TtLineGC, x + 1, y + 1, l - 3, arc * 2, 0 * 64, 180 * 64);
#       else  /* _WINDOWS */
#       endif /* _WINDOWS */
	FinishDrawing (0, RO, active);
     }
}

/*----------------------------------------------------------------------
  DrawUnion draw an Union symbol.
  RO indicates whether it's a read-only box
  active indicates if the box is active
  parameter fg indicates the drawing color
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                DrawUnion (int frame, int x, int y, int l, int h, ptrfont font, int RO, int active, int fg)
#else  /* __STDC__ */
void                DrawUnion (frame, x, y, l, h, font, RO, active, fg)
int                 frame;
int                 x;
int                 y;
int                 l;
int                 h;
ptrfont             font;
int                 RO;
int                 active;
int                 fg;
#endif /* __STDC__ */
{
   int                 arc, fh;

   fh = FontHeight (font);
   if (h < fh * 2 && l <= CharacterWidth ('\310', font))
     {
	/* Only one glyph needed */
	DrawMonoSymb ('\310', frame, x, y, l, h, RO, active, font, fg);
     }
   else
     {
	/* radius of arcs is 3mm */
	arc = h / 4;
	InitDrawing (0, 0, 2, RO, active, fg);
	/* two vertical lines */
	DoDrawOneLine (frame, x + 1, y, x + 1, y + h - arc);
	DoDrawOneLine (frame, x + l - 2, y, x + l - 2, y + h - arc);

	/* Lower part */
#       ifndef _WINDOWS
	XDrawArc (TtDisplay, FrRef[frame], TtLineGC, x + 1, y + h - arc * 2 - 2, l - 3, arc * 2, -0 * 64, -180 * 64);
	FinishDrawing (0, RO, active);
#       endif /* _WINDOWS */
     }
}
#endif /* _WIN_PRINT */

/*----------------------------------------------------------------------
  TraceFleche draw the end of an arrow.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         TraceFleche (int frame, int x1, int y1, int x2, int y2, int thick, int RO, int active, int fg)
#else  /* __STDC__ */
static void         TraceFleche (frame, x1, y1, x2, y2, thick, RO, active, fg)
int                 frame;
int                 x1, y1, x2, y2;
int                 thick;
int                 RO;
int                 active;
int                 fg;

#endif /* __STDC__ */
{
#  ifndef _WINDOWS
   float               x, y, xb, yb, dx, dy, l, sina, cosa;
   int                 xc, yc, xd, yd;
   float               width, height;
   ThotPoint           point[3];
   Pixmap              pattern;

   width = 5 + thick;
   height = 10;
   dx = (float) (x2 - x1);
   dy = (float) (y1 - y2);
   l = sqrt ((double) (dx * dx + dy * dy));
   if (l == 0)
      return;
   sina = dy / l;
   cosa = dx / l;
   xb = x2 * cosa - y2 * sina;
   yb = x2 * sina + y2 * cosa;
   x = xb - height;
   y = yb - width / 2;
   xc = FloatToInt (x * cosa + y * sina + .5);
   yc = FloatToInt (-x * sina + y * cosa + .5);
   y = yb + width / 2;
   xd = FloatToInt (x * cosa + y * sina + .5);
   yd = FloatToInt (-x * sina + y * cosa + .5);

   /* draw */
   point[0].x = x2;
   point[0].y = y2;
   point[1].x = xc;
   point[1].y = yc;
   point[2].x = xd;
   point[2].y = yd;
   pattern = CreatePattern (0, RO, active, fg, fg, 1);
   if (pattern != 0)
     {
	XSetTile (TtDisplay, TtGreyGC, pattern);
	XFillPolygon (TtDisplay, FrRef[frame], TtGreyGC, point, 3, Convex, CoordModeOrigin);
	XFreePixmap (TtDisplay, pattern);
     }
#  endif /* _WINDOWS */
}

#ifndef _WIN_PRINT
/*----------------------------------------------------------------------
  DrawArrow draw an arrow following the indicated direction in degrees :
  0 (right arrow), 45, 90, 135, 180,
  225, 270 ou 315.
  RO indicates whether it's a read-only box
  active indicates if the box is active
  parameter fg indicates the drawing color
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                DrawArrow (int frame, int thick, int style, int x, int y, int l, int h, int orientation, int RO, int active, int fg)
#else  /* __STDC__ */
void                DrawArrow (frame, thick, style, x, y, l, h, orientation, RO, active, fg)
int                 frame;
int                 thick;
int                 style;
int                 x;
int                 y;
int                 l;
int                 h;
int                 orientation;
int                 RO;
int                 active;
int                 fg;
#endif /* __STDC__ */
{
   int                 xm, ym, xf, yf;

   if (thick <= 0)
      return;
   xm = x + ((l - thick) / 2);
   xf = x + l - 1;
   ym = y + ((h - thick) / 2);
   yf = y + h - 1;

   InitDrawing (0, style, thick, RO, active, fg);
   if (orientation == 0)
     {
	/* draw a right arrow */
	DoDrawOneLine (frame, x, ym, xf, ym);
	TraceFleche (frame, x, ym, xf, ym, thick, RO, active, fg);
     }
   else if (orientation == 45)
     {
	DoDrawOneLine (frame, x, yf, xf - thick + 1, y);
	TraceFleche (frame, x, yf, xf - thick + 1, y, thick, RO, active, fg);
     }
   else if (orientation == 90)
     {
	/* draw a bottom-up arrow */
	DoDrawOneLine (frame, xm, y, xm, yf);
	TraceFleche (frame, xm, yf, xm, y, thick, RO, active, fg);
     }
   else if (orientation == 135)
     {
	DoDrawOneLine (frame, x, y, xf - thick + 1, yf);
	TraceFleche (frame, xf - thick + 1, yf, x, y, thick, RO, active, fg);
     }
   else if (orientation == 180)
     {
	/* draw a left arrow */
	DoDrawOneLine (frame, x, ym, xf, ym);
	TraceFleche (frame, xf, ym, x, ym, thick, RO, active, fg);
     }
   else if (orientation == 225)
     {
	DoDrawOneLine (frame, x, yf, xf - thick + 1, y);
	TraceFleche (frame, xf - thick + 1, y, x, yf, thick, RO, active, fg);
     }
   else if (orientation == 270)
     {
	/* draw a top-down arrow */
	DoDrawOneLine (frame, xm, y, xm, yf);
	TraceFleche (frame, xm, y, xm, yf, thick, RO, active, fg);
     }
   else if (orientation == 315)
     {
	DoDrawOneLine (frame, x, y, xf - thick + 1, yf);
	TraceFleche (frame, x, y, xf - thick + 1, yf, thick, RO, active, fg);
     }
   FinishDrawing (0, RO, active);
}

/*----------------------------------------------------------------------
  DrawBracket draw an opening or closing bracket (depending on direction)
  RO indicates whether it's a read-only box
  active indicates if the box is active
  parameter fg indicates the drawing color
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                DrawBracket (int frame, int thick, int x, int y, int l, int h, int direction, ptrfont font, int RO, int active, int fg)

#else  /* __STDC__ */
void                DrawBracket (frame, thick, x, y, l, h, direction, font, RO, active, fg)
int                 frame;
int                 thick;
int                 x;
int                 y;
int                 l;
int                 h;
int                 direction;
ptrfont             font;
int                 RO;
int                 active;
int                 fg;

#endif /* __STDC__ */

{
   int                 xm, yf, yend;

   if (FontHeight (font) >= h)
     {
	/* With only one glyph */
	if (direction == 0)
	  {
	     /* Draw a opening bracket */
	     xm = x + ((l - CharacterWidth ('[', font)) / 2);
	     yf = y + ((h - CharacterHeight ('[', font)) / 2) -
		FontAscent (font) + CharacterAscent ('[', font);
	     DrawChar ('[', frame, xm, yf, font, RO, active, fg);
	  }
	else
	  {
	     /* Draw a closing bracket */
	     xm = x + ((l - CharacterWidth (']', font)) / 2);
	     yf = y + ((h - CharacterHeight (']', font)) / 2) -
		FontAscent (font) + CharacterAscent (']', font);
	     DrawChar (']', frame, xm, yf, font, RO, active, fg);
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
	     DrawChar ('\351', frame, xm, yf, font, RO, active, fg);
	     yend = y + h - CharacterHeight ('\353', font) -
		FontAscent (font) + CharacterAscent ('\353', font);
	     DrawChar ('\353', frame, xm, yend, font, RO, active, fg);
	     for (yf = yf + CharacterHeight ('\351', font) -
		  FontAscent (font) + CharacterAscent ('\352', font);
		  yf < yend;
		  yf += CharacterHeight ('\352', font))
		DrawChar ('\352', frame, xm, yf, font, RO, active, fg);
	  }
	else
	  {
	     /* Draw a closing bracket */
	     xm = x + ((l - CharacterWidth ('\371', font)) / 2);
	     yf = y - FontAscent (font) + CharacterAscent ('\371', font);
	     DrawChar ('\371', frame, xm, yf, font, RO, active, fg);
	     yend = y + h - CharacterHeight ('\373', font) -
		FontAscent (font) + CharacterAscent ('\373', font);
	     DrawChar ('\373', frame, xm, yend, font, RO, active, fg);
	     for (yf = yf + CharacterHeight ('\371', font) -
		  FontAscent (font) + CharacterAscent ('\372', font);
		  yf < yend;
		  yf += CharacterHeight ('\372', font))
		DrawChar ('\372', frame, xm, yf, font, RO, active, fg);
	  }
     }
}

/*----------------------------------------------------------------------
  DrawParenthesis draw a closing or opening parenthesis (direction).
  RO indicates whether it's a read-only box
  active indicates if the box is active
  parameter fg indicates the drawing color
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                DrawParenthesis (int frame, int thick, int x, int y, int l, int h, int direction, ptrfont font, int RO, int active, int fg)
#else  /* __STDC__ */
void                DrawParenthesis (frame, thick, x, y, l, h, direction, font, RO, active, fg)
int                 frame;
int                 thick;
int                 x;
int                 y;
int                 l;
int                 h;
int                 direction;
ptrfont             font;
int                 RO;
int                 active;
int                 fg;
#endif /* __STDC__ */
{
   int                 xm, yf, yend, exnum, delta;

   exnum = 0;

   if (FontHeight (font) >= h)
     {
	/* With only one glyph */
	if (direction == 0)
	  {
	     /* draw a opening parenthesis */
	     xm = x + ((l - CharacterWidth ('(', font)) / 2);
	     yf = y + ((h - CharacterHeight ('(', font)) / 2) - FontAscent (font) + CharacterAscent ('(', font);
	     DrawChar ('(', frame, xm, yf, font, RO, active, fg);
	  }
	else
	  {
	     /* draw a closing parenthesis */
	     xm = x + ((l - CharacterWidth (')', font)) / 2);
	     yf = y + ((h - CharacterHeight (')', font)) / 2) - FontAscent (font) + CharacterAscent (')', font);
	     DrawChar (')', frame, xm, yf, font, RO, active, fg);
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
	     DrawChar ('\346', frame, xm, yf, font, RO, active, fg);
	     yend = y + h - CharacterHeight ('\350', font) - FontAscent (font) + CharacterAscent ('\350', font) - 1;
	     DrawChar ('\350', frame, xm, yend, font, RO, active, fg);

	     yf += CharacterHeight ('\346', font) - 1;
	     delta = yend - yf;
	     if (delta >= 0)
	       {
		  for (yf += CharacterAscent ('\347', font) - FontAscent (font),
		       yend -= CharacterHeight ('\347', font) - 1;
		       yf < yend;
		       yf += CharacterHeight ('\347', font), exnum++)
		     DrawChar ('\347', frame, xm, yf, font, RO, active, fg);
		  if (exnum)
		     DrawChar ('\347', frame, xm, yend, font, RO, active, fg);
		  else
		     DrawChar ('\347', frame, xm, yf + ((delta - CharacterHeight ('\347', font)) / 2), font, RO, active, fg);
	       }
	  }

	else
	  {
	     /* draw a closing parenthesis */
	     xm = x + ((l - CharacterWidth ('\366', font)) / 2);
	     yf = y - FontAscent (font) + CharacterAscent ('\366', font);
	     DrawChar ('\366', frame, xm, yf, font, RO, active, fg);
	     yend = y + h - CharacterHeight ('\370', font) - FontAscent (font) + CharacterAscent ('\370', font) - 1;
	     DrawChar ('\370', frame, xm, yend, font, RO, active, fg);

	     yf += CharacterHeight ('\366', font) - 1;
	     delta = yend - yf;
	     if (delta >= 0)
	       {
		  for (yf += CharacterAscent ('\367', font) - FontAscent (font),
		       yend -= CharacterHeight ('\367', font) - 1;
		       yf < yend;
		       yf += CharacterHeight ('\367', font), exnum++)
		     DrawChar ('\367', frame, xm, yf, font, RO, active, fg);
		  if (exnum)
		     DrawChar ('\367', frame, xm, yend, font, RO, active, fg);
		  else
		     DrawChar ('\367', frame, xm, yf + ((delta - CharacterHeight ('\367', font)) / 2), font, RO, active, fg);
	       }
	  }
     }
}

/*----------------------------------------------------------------------
  DrawBrace draw an opening of closing brace (depending on direction).
  RO indicates whether it's a read-only box
  active indicates if the box is active
  parameter fg indicates the drawing color
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                DrawBrace (int frame, int thick, int x, int y, int l, int h, int direction, ptrfont font, int RO, int active, int fg)

#else  /* __STDC__ */
void                DrawBrace (frame, thick, x, y, l, h, direction, font, RO, active, fg)
int                 frame;
int                 thick;
int                 x;
int                 y;
int                 l;
int                 h;
int                 direction;
ptrfont             font;
int                 RO;
int                 active;
int                 fg;

#endif /* __STDC__ */

{
   int                 xm, ym, yf, yend, exnum, delta;

   exnum = 0;

   if (FontHeight (font) >= h)
     {
	/* need only one char */
	if (direction == 0)
	  {
	     /* just use the opening brace glyph */
	     xm = x + ((l - CharacterWidth ('{', font)) / 2);
	     yf = y + ((h - CharacterHeight ('{', font)) / 2) - FontAscent (font) + CharacterAscent ('{', font);
	     DrawChar ('{', frame, xm, yf, font, RO, active, fg);
	  }
	else
	  {
	     /* just use the closing brace glyph */
	     xm = x + ((l - CharacterWidth ('}', font)) / 2);
	     yf = y + ((h - CharacterHeight ('}', font)) / 2) - FontAscent (font) + CharacterAscent ('}', font);
	     DrawChar ('}', frame, xm, yf, font, RO, active, fg);
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
	     DrawChar ('\354', frame, xm, yf, font, RO, active, fg);
	     /* vertical line */
	     ym = y + ((h - CharacterHeight ('\355', font)) / 2) - FontAscent (font)
		+ CharacterAscent ('\355', font);
	     DrawChar ('\355', frame, xm, ym, font, RO, active, fg);
	     /* bottom */
	     yend = y + h - CharacterHeight ('\356', font) - FontAscent (font) + CharacterAscent ('\356', font);
	     DrawChar ('\356', frame, xm, yend, font, RO, active, fg);

	     /* finish top */
	     yf += CharacterHeight ('\354', font) - 1;
	     delta = ym - yf;
	     if (delta >= 0)
	       {
		  for (yf += CharacterAscent ('\357', font) - FontAscent (font),
		       ym -= CharacterHeight ('\357', font);
		       yf < ym;
		       yf += CharacterHeight ('\357', font), exnum++)
		     DrawChar ('\357', frame, xm, yf, font, RO, active, fg);
		  if (exnum)
		     DrawChar ('\357', frame, xm, ym, font, RO, active, fg);
		  else
		     DrawChar ('\357', frame, xm, yf + ((delta - CharacterHeight ('\357', font)) / 2), font, RO, active, fg);
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
		     DrawChar ('\357', frame, xm, yf, font, RO, active, fg);
		  if (exnum)
		     DrawChar ('\357', frame, xm, yend, font, RO, active, fg);
		  else
		     DrawChar ('\357', frame, xm, yf + ((delta - CharacterHeight ('\357', font)) / 2), font, RO, active, fg);
	       }
	  }

	else
	  {
	     /* top */
	     xm = x + ((l - CharacterWidth ('\374', font)) / 2);
	     yf = y - FontAscent (font) + CharacterAscent ('\374', font);
	     DrawChar ('\374', frame, xm, yf, font, RO, active, fg);
	     /* center */
	     ym = y + ((h - CharacterHeight ('\375', font)) / 2)
		- FontAscent (font) + CharacterAscent ('\375', font);
	     DrawChar ('\375', frame, xm, ym, font, RO, active, fg);
	     /* bottom */
	     yend = y + h - CharacterHeight ('\376', font)
		- FontAscent (font) + CharacterAscent ('\376', font);
	     DrawChar ('\376', frame, xm, yend, font, RO, active, fg);
	     /* finish top */
	     yf += CharacterHeight ('\374', font) - 1;
	     delta = ym - yf;
	     if (delta >= 0)
	       {
		  for (yf += CharacterAscent ('\357', font) - FontAscent (font),
		       ym -= CharacterHeight ('\357', font);
		       yf < ym;
		       yf += CharacterHeight ('\357', font), exnum++)
		     DrawChar ('\357', frame, xm, yf, font, RO, active, fg);
		  if (exnum)
		     DrawChar ('\357', frame, xm, ym, font, RO, active, fg);
		  else
		     DrawChar ('\357', frame, xm, yf + ((delta - CharacterHeight ('\357', font)) / 2), font, RO, active, fg);
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
		     DrawChar ('\357', frame, xm, yf, font, RO, active, fg);
		  if (exnum)
		     DrawChar ('\357', frame, xm, yend, font, RO, active, fg);
		  else
		     DrawChar ('\357', frame, xm, yf + ((delta - CharacterHeight ('\357', font)) / 2), font, RO, active, fg);
	       }
	  }
     }
}

/*----------------------------------------------------------------------
  DrawRectangle draw a rectangle located at (x, y) in frame,
  of geometry width x height.
  thick indicates the thickness of the lines.
  RO indicates whether it's a read-only box
  active indicates if the box is active
  Parameters fg, bg, and pattern are for drawing
  color, background color and fill pattern.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                DrawRectangle (int frame, int thick, int style, int x, int y, int width, int height, int RO, int active, int fg, int bg, int pattern)

#else  /* __STDC__ */
void                DrawRectangle (frame, thick, style, x, y, width, height, RO, active, fg, bg, pattern)
int                 frame;
int                 thick;
int                 style;
int                 x;
int                 y;
int                 width;
int                 height;
int                 RO;
int                 active;
int                 fg;
int                 bg;
int                 pattern;

#endif /* __STDC__ */

{
   /*int eps2; */
   Pixmap              pat;

#  ifdef _WINDOWS
   HBRUSH              hBrush;
   HBRUSH              hOldBrush;
   HPEN                hPen = 0;
   HPEN                hOldPen;
#  endif

#  ifdef _WINDOWS 
   WIN_GetDeviceContext (frame);
#  endif /* _WINDOWS */
   if (width <= 0 || height <= 0) {
#     ifdef _WINDOWS
      WIN_ReleaseDeviceContext ();
#     endif /* _WINDOWS */
      return;
   }
   width = width - thick - 1;
   height = height - thick - 1;
   x += thick / 2;
   y += thick / 2;
   /*eps2 = thick > 1; */

   /* Fill in the rectangle */
   pat = (Pixmap) CreatePattern (0, RO, active, fg, bg, pattern);
   if (pat != 0) {
#  ifndef _WINDOWS
      XSetTile (TtDisplay, TtGreyGC, pat);
      XFillRectangle (TtDisplay, FrRef[frame], TtGreyGC,
                      x + FrameTable[frame].FrLeftMargin, y + FrameTable[frame].FrTopMargin, width, height);
      XFreePixmap (TtDisplay, pat);
#  else /* _WINDOWS */
      SelectClipRgn(TtDisplay, clipRgn); 
      WinLoadGC (fg, RO);
   
      hBrush = CreateSolidBrush (Pix_Color[bg]);
      hOldBrush = SelectObject (TtDisplay, hBrush);
      PatBlt (TtDisplay, x + FrameTable[frame].FrLeftMargin, y + FrameTable[frame].FrTopMargin, width, height, PATCOPY);
      SelectObject (TtDisplay, hOldBrush);
      if (!DeleteObject (hBrush))
         WinErrorBox (WIN_Main_Wd);
#  endif /* _WINDOWS */
   }

   /* Draw the border */
   if (thick > 0)
     {
	InitDrawing (0, style, thick, RO, active, fg);
#       ifndef _WINDOWS
	XDrawRectangle (TtDisplay, FrRef[frame], TtLineGC, x + FrameTable[frame].FrLeftMargin, y + FrameTable[frame].FrTopMargin, width, height);
#       else  /* _WINDOWS */
        if (!(hPen = CreatePen (PS_SOLID, thick, Pix_Color [fg])))
           WinErrorBox (WIN_Main_Wd);
        hOldPen = SelectObject (TtDisplay, hPen) ;
        SelectObject (TtDisplay, GetStockObject (NULL_BRUSH)) ;
        Rectangle (TtDisplay, x + FrameTable[frame].FrLeftMargin, y + FrameTable[frame].FrTopMargin, x + FrameTable[frame].FrLeftMargin + width, y + FrameTable[frame].FrTopMargin + height);
		SelectObject (TtDisplay, hOldPen);
#       endif /* _WINDOWS */
	FinishDrawing (0, RO, active);
   }
#       ifdef _WINDOWS 
	WIN_ReleaseDeviceContext ();
    if (hPen && !DeleteObject (hPen))
       WinErrorBox (WIN_Main_Wd);
#       endif /* _WINDOWS */
}

/*----------------------------------------------------------------------
  DrawDiamond draw a diamond.
  RO indicates whether it's a read-only box
  active indicates if the box is active
  Parameters fg, bg, and pattern are for drawing
  color, background color and fill pattern.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                DrawDiamond (int frame, int thick, int style, int x, int y, int width, int height, int RO, int active, int fg, int bg, int pattern)

#else  /* __STDC__ */
void                DrawDiamond (frame, thick, style, x, y, width, height, RO, active, fg, bg, pattern)
int                 frame;
int                 thick;
int                 style;
int                 x;
int                 y;
int                 width;
int                 height;
int                 RO;
int                 active;
int                 fg;
int                 bg;
int                 pattern;

#endif /* __STDC__ */

{
#  ifndef _WINDOWS
   ThotPoint           point[5];
   Pixmap              pat;

   width = width - thick - 1;
   height = height - thick - 1;
   x += thick / 2;
   y += thick / 2;

   point[0].x = x + (width / 2) + FrameTable[frame].FrLeftMargin;
   point[0].y = y + FrameTable[frame].FrTopMargin;
   point[4].x = point[0].x;
   point[4].y = point[0].y;
   point[1].x = x + width + FrameTable[frame].FrLeftMargin;
   point[1].y = y + (height / 2) + FrameTable[frame].FrTopMargin;
   point[2].x = point[0].x;
   point[2].y = y + height + FrameTable[frame].FrTopMargin;
   point[3].x = x + FrameTable[frame].FrLeftMargin;
   point[3].y = point[1].y;

   /* Fill in the diamond */
   pat = CreatePattern (0, RO, active, fg, bg, pattern);
   if (pat != 0)
     {
	XSetTile (TtDisplay, TtGreyGC, pat);
	XFillPolygon (TtDisplay, FrRef[frame], TtGreyGC,
		      point, 5, Convex, CoordModeOrigin);
	XFreePixmap (TtDisplay, pat);
     }

   /* Draw the border */
   if (thick > 0)
     {
	InitDrawing (0, style, thick, RO, active, fg);
	XDrawLines (TtDisplay, FrRef[frame], TtLineGC,
		    point, 5, CoordModeOrigin);
	FinishDrawing (0, RO, active);
     }
#  endif /* _WINDOWS */
}

/*----------------------------------------------------------------------
  DrawSegments draw a set of segments.
  Parameter buffer is a pointer to the list of control points.
  nb indicates the number of points.
  The first point is a fake one containing the geometry.
  RO indicates whether it's a read-only box
  active indicates if the box is active
  fg parameter gives the drawing color.
  arrow parameter indicates whether :
  - no arrow have to be drawn (0)
  - a forward arrow has to be drawn (1)
  - a backward arrow has to be drawn (2)
  - both backward and forward arrows have to be drawn (3)
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                DrawSegments (int frame, int thick, int style, int x, int y, PtrTextBuffer buffer, int nb, int RO, int active, int fg, int arrow)

#else  /* __STDC__ */
void                DrawSegments (frame, thick, style, x, y, buffer, nb, RO, active, fg, arrow)
int                 frame;
int                 thick;
int                 style;
int                 x;
int                 y;
PtrTextBuffer       buffer;
int                 nb;
int                 RO;
int                 active;
int                 fg;
int                 arrow;

#endif /* __STDC__ */

{
#  ifndef _WINDOWS
   ThotPoint          *points;
   int                 i, j;
   PtrTextBuffer       adbuff;

   if (thick == 0)
      return;

   /* Allocate a table of points */
   points = (ThotPoint *) TtaGetMemory (sizeof (ThotPoint) * (nb - 1));
   adbuff = buffer;
   j = 1;
   for (i = 1; i < nb; i++)
     {
	if (j >= adbuff->BuLength)
	  {
	     if (adbuff->BuNext != NULL)
	       {
		  /* Next buffer */
		  adbuff = adbuff->BuNext;
		  j = 0;
	       }
	  }
	points[i - 1].x = x + FrameTable[frame].FrLeftMargin + PointToPixel (adbuff->BuPoints[j].XCoord / 1000);
	points[i - 1].y = y + FrameTable[frame].FrTopMargin + PointToPixel (adbuff->BuPoints[j].YCoord / 1000);
	j++;
     }


   /* backward arrow  */
   if (arrow == 2 || arrow == 3)
      TraceFleche (frame, points[1].x, points[1].y, points[0].x, points[0].y, thick, RO, active, fg);

   /* Draw the border */
   InitDrawing (0, style, thick, RO, active, fg);
   XDrawLines (TtDisplay, FrRef[frame], TtLineGC, points, nb - 1, CoordModeOrigin);
   FinishDrawing (0, RO, active);

   /* Forward arrow */
   if (arrow == 1 || arrow == 3)
      TraceFleche (frame, points[nb - 3].x, points[nb - 3].y, points[nb - 2].x, points[nb - 2].y, thick, RO, active, fg);

   /* free the table of points */
   free ((char *) points);
#  endif /* _WINDOWS */
}

/*----------------------------------------------------------------------
  DrawPolygon draw a polygone.
  Parameter buffer is a pointer to the list of control points.
  nb indicates the number of points.
  The first point is a fake one containing the geometry.
  RO indicates whether it's a read-only box
  active indicates if the box is active
  Parameters fg, bg, and pattern are for drawing
  color, background color and fill pattern.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                DrawPolygon (int frame, int thick, int style, int x, int y, PtrTextBuffer buffer, int nb, int RO, int active, int fg, int bg, int pattern)

#else  /* __STDC__ */
void                DrawPolygon (frame, thick, style, x, y, buffer, nb, RO, active, fg, bg, pattern)
int                 frame;
int                 thick;
int                 style;
int                 x;
int                 y;
PtrTextBuffer       buffer;
int                 nb;
int                 RO;
int                 active;
int                 fg;
int                 bg;
int                 pattern;

#endif /* __STDC__ */

{
   ThotPoint          *points;
   int                 i, j;
   PtrTextBuffer       adbuff;
   Pixmap              pat;

#  ifdef _WINDOWS 
   HPEN hPen;
   HPEN hOldPen;
#  endif /* _WINDOWS */

   /* Allocate a table of points */
   points = (ThotPoint *) TtaGetMemory (sizeof (ThotPoint) * nb);
   adbuff = buffer;
   j = 1;
   for (i = 1; i < nb; i++)
     {
	if (j >= adbuff->BuLength)
	  {
	     if (adbuff->BuNext != NULL)
	       {
		  /* Next buffer */
		  adbuff = adbuff->BuNext;
		  j = 0;
	       }
	  }
	points[i - 1].x = x + FrameTable[frame].FrLeftMargin + PointToPixel (adbuff->BuPoints[j].XCoord / 1000);
	points[i - 1].y = y + FrameTable[frame].FrTopMargin + PointToPixel (adbuff->BuPoints[j].YCoord / 1000);
	j++;
     }
   /* Close the polygone */
   points[nb - 1].x = points[0].x;
   points[nb - 1].y = points[0].y;

   /* Fill in the polygone */
#  ifndef _WINDOWS
   pat = CreatePattern (0, RO, active, fg, bg, pattern);
   if (pat != 0) {
      XSetTile (TtDisplay, TtGreyGC, pat);
      XFillPolygon (TtDisplay, FrRef[frame], TtGreyGC, points, nb, Complex, CoordModeOrigin);
      XFreePixmap (TtDisplay, pat);
   }
#  endif /* _WINDOWS */

   /* Draw the border */
   if (thick > 0) {
#     ifdef _WINDOWS
      WIN_GetDeviceContext (frame);
      if (!(hPen = CreatePen (PS_SOLID, thick, Pix_Color [fg])))
         WinErrorBox (WIN_Main_Wd);
      hOldPen = SelectObject (TtDisplay, hPen) ;
      InitDrawing (0, style, thick, RO, active, fg);
      Polyline (TtDisplay, points, nb);
      FinishDrawing (0, RO, active);
	  SelectObject (TtDisplay, hOldPen);
      WIN_ReleaseDeviceContext ();
	  if (!DeleteObject (hPen))
         WinErrorBox (WIN_Main_Wd);
#     else  /* !_WINDOWS */
      InitDrawing (0, style, thick, RO, active, fg);
      XDrawLines (TtDisplay, FrRef[frame], TtLineGC, points, nb, CoordModeOrigin);
      FinishDrawing (0, RO, active);
#     endif /* !_WINDOWS */
   }
   /* free the table of points */
   free ((char *) points);
}
#endif /* _WIN_PRINT */

/*----------------------------------------------------------------------
  PolyNewPoint : add a new point to the current polyline.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static boolean      PolyNewPoint (int x, int y)
#else  /* __STDC__ */
static boolean      PolyNewPoint (x, y)
int                 x, y;
#endif /* __STDC__ */
{
#  ifndef _WINDOWS
   ThotPoint          *tmp;
   int                 taille;

   if (npoints >= MAX_points)
     {
	taille = MAX_points + ALLOC_POINTS;
	if ((tmp = (ThotPoint *) realloc (points, taille * sizeof (ThotPoint))) == 0)
	   return (FALSE);
	else
	  {
	     /* la reallocation a reussi */
	     points = tmp;
	     MAX_points = taille;
	  }
     }

   /* ignore identical points */
   if (npoints > 0 && points[npoints - 1].x == x && points[npoints - 1].y == y)
      return (FALSE);

   points[npoints].x = x;
   points[npoints].y = y;
   npoints++;
#  endif /* _WINDOWS */
   return (TRUE);
}

/*----------------------------------------------------------------------
  PushStack : push a spline on the stack.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         PushStack (float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4)
#else  /* __STDC__ */
static void         PushStack (x1, y1, x2, y2, x3, y3, x4, y4)
float               x1, y1, x2, y2, x3, y3, x4, y4;
#endif /* __STDC__ */
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
#ifdef __STDC__
static boolean      PopStack (float *x1, float *y1, float *x2, float *y2, float *x3, float *y3, float *x4, float *y4)
#else  /* __STDC__ */
static boolean      PopStack (x1, y1, x2, y2, x3, y3, x4, y4)
float              *x1, *y1, *x2, *y2, *x3, *y3, *x4, *y4;

#endif /* __STDC__ */
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
#ifdef __STDC__
static void         PolySplit (float a1, float b1, float a2, float b2, float a3, float b3, float a4, float b4)
#else  /* __STDC__ */
static void         PolySplit (a1, b1, a2, b2, a3, b3, a4, b4)
float               a1, b1, a2, b2, a3, b3, a4, b4;

#endif /* __STDC__ */
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
	   PolyNewPoint (FloatToInt (x1), FloatToInt (y1));
	else
	  {
	     tx   = MIDDLE_OF (x2, x3);
	     ty   = MIDDLE_OF (y2, y3);
	     sx1  = MIDDLE_OF (x1, x2);
	     sy1  = MIDDLE_OF (y1, y2);
	     sx2  = MIDDLE_OF (sx1, tx);
	     sy2  = MIDDLE_OF (sy1, ty);
	     tx2  = MIDDLE_OF (x3, x4);
	     ty2  = MIDDLE_OF (y3, y4);
	     tx1  = MIDDLE_OF (tx2, tx);
	     ty1  = MIDDLE_OF (ty2, ty);
	     xmid = MIDDLE_OF (sx2, tx1);
	     ymid = MIDDLE_OF (sy2, ty1);

	     PushStack (xmid, ymid, tx1, ty1, tx2, ty2, x4, y4);
	     PushStack (x1, y1, sx1, sy1, sx2, sy2, xmid, ymid);
	  }
     }
}

#ifndef _WIN_PRINT
/*----------------------------------------------------------------------
  DrawCurb draw an open curb.
  Parameter buffer is a pointer to the list of control points.
  nb indicates the number of points.
  The first point is a fake one containing the geometry.
  RO indicates whether it's a read-only box
  active indicates if the box is active
  fg indicates the drawing color
  arrow parameter indicates whether :
  - no arrow have to be drawn (0)
  - a forward arrow has to be drawn (1)
  - a backward arrow has to be drawn (2)
  - both backward and forward arrows have to be drawn (3)
  Parameter control indicates the control points.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                DrawCurb (int frame, int thick, int style, int x, int y, PtrTextBuffer buffer, int nb, int RO, int active, int fg, int arrow, C_points * controls)

#else  /* __STDC__ */
void                DrawCurb (frame, thick, style, x, y, buffer, nb, RO, active, fg, arrow, controls)
int                 frame;
int                 thick;
int                 style;
int                 x;
int                 y;
PtrTextBuffer       buffer;
int                 nb;
int                 RO;
int                 active;
int                 fg;
int                 arrow;
C_points           *controls;

#endif /* __STDC__ */
{
   PtrTextBuffer       adbuff;
   int                 i, j;
   float               x1, y1, x2, y2;
   float               cx1, cy1, cx2, cy2;

   if (thick == 0)
      return;

   /* alloue la liste des points */
   npoints = 0;
   MAX_points = ALLOC_POINTS;
#  ifndef _WINDOWS
   points = (ThotPoint *) TtaGetMemory (sizeof (ThotPoint) * MAX_points);
#  endif /* _WINDOWS */

   adbuff = buffer;
   j = 1;
   x1 = (float) (x + FrameTable[frame].FrLeftMargin + PointToPixel (adbuff->BuPoints[j].XCoord / 1000));
   y1 = (float) (y + FrameTable[frame].FrTopMargin + PointToPixel (adbuff->BuPoints[j].YCoord / 1000));
   j++;
   cx1 = (controls[j].lx * 3 + x1 - x - FrameTable[frame].FrLeftMargin) / 4 + x + FrameTable[frame].FrLeftMargin;
   cy1 = (controls[j].ly * 3 + y1 - y - FrameTable[frame].FrTopMargin) / 4 + y + FrameTable[frame].FrTopMargin;
   x2 = (float) (x + FrameTable[frame].FrLeftMargin + PointToPixel (adbuff->BuPoints[j].XCoord / 1000));
   y2 = (float) (y + FrameTable[frame].FrTopMargin + PointToPixel (adbuff->BuPoints[j].YCoord / 1000));
   cx2 = (controls[j].lx * 3 + x2 - x - FrameTable[frame].FrLeftMargin) / 4 + x + FrameTable[frame].FrLeftMargin;
   cy2 = (controls[j].ly * 3 + y2 - y - FrameTable[frame].FrTopMargin) / 4 + y + FrameTable[frame].FrTopMargin;

   /* backward arrow  */
   if (arrow == 2 || arrow == 3)
      TraceFleche (frame, FloatToInt (cx1), FloatToInt (cy1), (int) x1, (int) y1, thick, RO, active, fg);

   for (i = 2; i < nb; i++)
     {
	PolySplit (x1, y1, cx1, cy1, cx2, cy2, x2, y2);

	/* skip to next points */
	x1 = x2;
	y1 = y2;
	cx1 = controls[i].rx + x + FrameTable[frame].FrLeftMargin;
	cy1 = controls[i].ry + y + FrameTable[frame].FrTopMargin;
	if (i < nb - 1)
	  {
	     /* not finished */
	     j++;
	     if (j >= adbuff->BuLength)
	       {
		  if (adbuff->BuNext != NULL)
		    {
		       /* Next buffer */
		       adbuff = adbuff->BuNext;
		       j = 0;
		    }
	       }
	     x2 = (float) (x + FrameTable[frame].FrLeftMargin + PointToPixel (adbuff->BuPoints[j].XCoord / 1000));
	     y2 = (float) (y + FrameTable[frame].FrTopMargin + PointToPixel (adbuff->BuPoints[j].YCoord / 1000));
	     if (i == nb - 2)
	       {
		  cx1 = (controls[i].rx * 3 + x1 - x - FrameTable[frame].FrLeftMargin) / 4 + x + FrameTable[frame].FrLeftMargin;
		  cy1 = (controls[i].ry * 3 + y1 - y - FrameTable[frame].FrTopMargin) / 4 + y + FrameTable[frame].FrTopMargin;
		  cx2 = (controls[i].rx * 3 + x2 - x - FrameTable[frame].FrLeftMargin) / 4 + x + FrameTable[frame].FrLeftMargin;
		  cy2 = (controls[i].ry * 3 + y2 - y - FrameTable[frame].FrTopMargin) / 4 + y + FrameTable[frame].FrTopMargin;
	       }
	     else
	       {
		  cx2 = controls[i + 1].lx + x + FrameTable[frame].FrLeftMargin;
		  cy2 = controls[i + 1].ly + y + FrameTable[frame].FrTopMargin;
	       }
	  }
     }
   PolyNewPoint ((int) x2, (int) y2);

   /* Draw the border */
   InitDrawing (0, style, thick, RO, active, fg);
#  ifndef _WINDOWS
   XDrawLines (TtDisplay, FrRef[frame], TtLineGC, points, npoints, CoordModeOrigin);
#  endif /* _WINDOWS */

   /* Forward arrow */
   if (arrow == 1 || arrow == 3)
      TraceFleche (frame, FloatToInt (cx2), FloatToInt (cy2), (int) x2, (int) y2, thick, RO, active, fg);

   FinishDrawing (0, RO, active);
   /* free the table of points */
   free ((char *) points);
}

/*----------------------------------------------------------------------
  DrawSpline draw a closed curb.
  Parameter buffer is a pointer to the list of control points.
  nb indicates the number of points.
  The first point is a fake one containing the geometry.
  RO indicates whether it's a read-only box
  active indicates if the box is active
  Parameters fg, bg, and pattern are for drawing
  color, background color and fill pattern.
  Parameter controls contains the list of control points.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                DrawSpline (int frame, int thick, int style, int x, int y, PtrTextBuffer buffer, int nb, int RO, int active, int fg, int bg, int pattern, C_points * controls)

#else  /* __STDC__ */
void                DrawSpline (frame, thick, style, x, y, buffer, nb, RO, active, fg, bg, pattern, controls)
int                 frame;
int                 thick;
int                 style;
int                 x;
int                 y;
PtrTextBuffer       buffer;
int                 nb;
int                 RO;
int                 active;
int                 fg;
int                 bg;
int                 pattern;
C_points           *controls;

#endif /* __STDC__ */
{
   PtrTextBuffer       adbuff;
   int                 i, j;
   float               x1, y1, x2, y2;
   float               cx1, cy1, cx2, cy2;
   Pixmap              pat;

   /* allocate the list of points */
   npoints = 0;
   MAX_points = ALLOC_POINTS;
#  ifndef _WINDOWS
   points = (ThotPoint *) TtaGetMemory (sizeof (ThotPoint) * MAX_points);
#  endif /* _WINDOWS */

   adbuff = buffer;
   j = 1;
   x1 = (float) (x + FrameTable[frame].FrLeftMargin + PointToPixel (adbuff->BuPoints[j].XCoord / 1000));
   y1 = (float) (y + FrameTable[frame].FrTopMargin + PointToPixel (adbuff->BuPoints[j].YCoord / 1000));
   cx1 = controls[j].rx + x + FrameTable[frame].FrLeftMargin;
   cy1 = controls[j].ry + y + FrameTable[frame].FrTopMargin;
   j++;
   x2 = (float) (x + FrameTable[frame].FrLeftMargin + PointToPixel (adbuff->BuPoints[j].XCoord / 1000));
   y2 = (float) (y + FrameTable[frame].FrTopMargin + PointToPixel (adbuff->BuPoints[j].YCoord / 1000));
   cx2 = controls[j].lx + x + FrameTable[frame].FrLeftMargin;
   cy2 = controls[j].ly + y + FrameTable[frame].FrTopMargin;

   for (i = 2; i < nb; i++)
     {
	PolySplit (x1, y1, cx1, cy1, cx2, cy2, x2, y2);

	/* next points */
	x1 = x2;
	y1 = y2;
	cx1 = controls[i].rx + x + FrameTable[frame].FrLeftMargin;
	cy1 = controls[i].ry + y + FrameTable[frame].FrTopMargin;
	if (i < nb - 1)
	  {
	     /* not the last loop */
	     j++;
	     if (j >= adbuff->BuLength)
	       {
		  if (adbuff->BuNext != NULL)
		    {
		       /* Next buffer */
		       adbuff = adbuff->BuNext;
		       j = 0;
		    }
	       }
	     x2 = (float) (x + FrameTable[frame].FrLeftMargin + PointToPixel (adbuff->BuPoints[j].XCoord / 1000));
	     y2 = (float) (y + FrameTable[frame].FrTopMargin + PointToPixel (adbuff->BuPoints[j].YCoord / 1000));
	     cx2 = controls[i + 1].lx + x + FrameTable[frame].FrLeftMargin;
	     cy2 = controls[i + 1].ly + y + FrameTable[frame].FrTopMargin;
	  }
	else
	  {
	     /* loop around the origin point */
	     x2 = (float) (x + FrameTable[frame].FrLeftMargin + PointToPixel (buffer->BuPoints[1].XCoord / 1000));
	     y2 = (float) (y + FrameTable[frame].FrTopMargin + PointToPixel (buffer->BuPoints[1].YCoord / 1000));
	     cx2 = controls[1].lx + x + FrameTable[frame].FrLeftMargin;
	     cy2 = controls[1].ly + y + FrameTable[frame].FrTopMargin;
	  }
     }

   /* close the polyline */
   PolySplit (x1, y1, cx1, cy1, cx2, cy2, x2, y2);
   PolyNewPoint ((int) x2, (int) y2);

   /* Fill in the polygone */
   pat = (Pixmap) CreatePattern (0, RO, active, fg, bg, pattern);
#  ifndef _WINDOWS
   if (pat != 0)
     {
	XSetTile (TtDisplay, TtGreyGC, pat);
	XFillPolygon (TtDisplay, FrRef[frame], TtGreyGC, points, npoints, Complex, CoordModeOrigin);
	XFreePixmap (TtDisplay, pat);
     }
#  endif /* _WINDOWS */

   /* Draw the border */
   if (thick > 0)
     {
	InitDrawing (0, style, thick, RO, active, fg);
#       ifndef _WINDOWS
	XDrawLines (TtDisplay, FrRef[frame], TtLineGC, points, npoints, CoordModeOrigin);
#       endif /* _WINDOWS */
	FinishDrawing (0, RO, active);
     }

   /* free the table of points */
   free ((char *) points);
}

/*----------------------------------------------------------------------
  DrawOval draw a rectangle with smoothed corners.
  RO indicates whether it's a read-only box
  active indicates if the box is active
  Parameters fg, bg, and pattern are for drawing
  color, background color and fill pattern.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                DrawOval (int frame, int thick, int style, int x, int y, int width, int height, int RO, int active, int fg, int bg, int pattern)

#else  /* __STDC__ */
void                DrawOval (frame, thick, style, x, y, width, height, RO, active, fg, bg, pattern)
int                 frame;
int                 thick;
int                 style;
int                 x;
int                 y;
int                 width;
int                 height;
int                 RO;
int                 active;
int                 fg;
int                 bg;
int                 pattern;

#endif /* __STDC__ */

{
#  ifndef _WINDOWS
   int                 arc, xf, yf;
   XArc                xarc[4];
   XSegment            seg[4];
   Pixmap              pat;
   ThotPoint           point[13];

   width -= thick;
   height -= thick;
   x += thick / 2;
   y += thick / 2;
   /* radius of arcs is 3mm */
   arc = (3 * DOT_PER_INCHE) / 25.4 + 0.5;
   xf = x + width - 1;
   yf = y + height - 1;

   xarc[0].x = x + FrameTable[frame].FrLeftMargin;
   xarc[0].y = y + FrameTable[frame].FrTopMargin;
   xarc[0].width = arc * 2;
   xarc[0].height = xarc[0].width;
   xarc[0].angle1 = 90 * 64;
   xarc[0].angle2 = 90 * 64;

   xarc[1].x = xf - arc * 2 + FrameTable[frame].FrLeftMargin;
   xarc[1].y = xarc[0].y;
   xarc[1].width = xarc[0].width;
   xarc[1].height = xarc[0].width;
   xarc[1].angle1 = 0;
   xarc[1].angle2 = xarc[0].angle2;

   xarc[2].x = xarc[0].x;
   xarc[2].y = yf - arc * 2 + FrameTable[frame].FrTopMargin;
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

   seg[0].x1 = x + arc + FrameTable[frame].FrLeftMargin;
   seg[0].x2 = xf - arc + FrameTable[frame].FrLeftMargin;
   seg[0].y1 = y + FrameTable[frame].FrTopMargin;
   seg[0].y2 = seg[0].y1;

   seg[1].x1 = xf + FrameTable[frame].FrLeftMargin;
   seg[1].x2 = seg[1].x1;
   seg[1].y1 = y + arc + FrameTable[frame].FrTopMargin;
   seg[1].y2 = yf - arc + FrameTable[frame].FrTopMargin;

   seg[2].x1 = seg[0].x1;
   seg[2].x2 = seg[0].x2;
   seg[2].y1 = yf + FrameTable[frame].FrTopMargin;
   seg[2].y2 = seg[2].y1;

   seg[3].x1 = x + FrameTable[frame].FrLeftMargin;
   seg[3].x2 = seg[3].x1;
   seg[3].y1 = seg[1].y1;
   seg[3].y2 = seg[1].y2;

   /* Fill in the figure */
   pat = CreatePattern (0, RO, active, fg, bg, pattern);
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

	XSetTile (TtDisplay, TtGreyGC, pat);
	XFillPolygon (TtDisplay, FrRef[frame], TtGreyGC,
		      point, 13, Convex, CoordModeOrigin);
	/* Trace quatre arcs de cercle */
	XFillArcs (TtDisplay, FrRef[frame], TtGreyGC, xarc, 4);
	XFreePixmap (TtDisplay, pat);
     }

   /* Draw the border */
   if (thick > 0)
     {
	InitDrawing (0, style, thick, RO, active, fg);
	XDrawArcs (TtDisplay, FrRef[frame], TtLineGC, xarc, 4);
	XDrawSegments (TtDisplay, FrRef[frame], TtLineGC, seg, 4);
	FinishDrawing (0, RO, active);
     }
#  endif /* _WINDOWS */
}

/*----------------------------------------------------------------------
  DrawEllips draw an ellips (or a circle).
  RO indicates whether it's a read-only box
  active indicates if the box is active
  Parameters fg, bg, and pattern are for drawing
  color, background color and fill pattern.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                DrawEllips (int frame, int thick, int style, int x, int y, int width, int height, int RO, int active, int fg, int bg, int pattern)

#else  /* __STDC__ */
void                DrawEllips (frame, thick, style, x, y, width, height, RO, active, fg, bg, pattern)
int                 frame;
int                 thick;
int                 style;
int                 x;
int                 y;
int                 width;
int                 height;
int                 RO;
int                 active;
int                 fg;
int                 bg;
int                 pattern;

#endif /* __STDC__ */

{
   Pixmap              pat;

#  ifdef _WINDOWS
   HPEN hPen;
   HPEN hOldPen;
#  endif /* _WINDOWS */

   width -= thick + 1;
   height -= thick + 1;
   x += thick / 2 + FrameTable[frame].FrLeftMargin;
   y += thick / 2 + FrameTable[frame].FrTopMargin;

   /* Fill in the rectangle */
#  ifndef _WINDOWS
   pat = (Pixmap) CreatePattern (0, RO, active, fg, bg, pattern);
   if (pat != 0)
     {
	XSetTile (TtDisplay, TtGreyGC, pat);
	XFillArc (TtDisplay, FrRef[frame], TtGreyGC,
		  x, y, width, height, 0, 360 * 64);
	XFreePixmap (TtDisplay, pat);
     }
#  endif /* _WINDOWS */

   /* Draw the border */
   if (thick > 0) {
      InitDrawing (0, style, thick, RO, active, fg);
#     ifdef _WINDOWS
      WIN_GetDeviceContext (frame);
      if (!(hPen = CreatePen (PS_SOLID, thick, Pix_Color [fg])))
         WinErrorBox (WIN_Main_Wd);
      hOldPen = SelectObject (TtDisplay, hPen) ;
      SelectObject (TtDisplay, GetStockObject (NULL_BRUSH)) ;
	  if (!Ellipse (TtDisplay, x, y, x + width, y + height))
         WinErrorBox (FrRef  [frame]);
	  SelectObject (TtDisplay, hOldPen);
	  WIN_ReleaseDeviceContext ();
	  if (!DeleteObject (hPen))
         WinErrorBox (FrRef [frame]);
#     else  /* !_WINDOWS */
      XDrawArc (TtDisplay, FrRef[frame], TtLineGC, x, y, width, height, 0, 360 * 64);
#     endif /* _WINDOWS */
	FinishDrawing (0, RO, active);
     }
}

/*----------------------------------------------------------------------
  DrawVerticalLine draw a vertical line aligned top center or bottom
  depending on align value.
  RO indicates whether it's a read-only box
  active indicates if the box is active
  parameter fg indicates the drawing color
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                DrawHorizontalLine (int frame, int thick, int style, int x, int y, int l, int h, int align, int RO, int active, int fg)

#else  /* __STDC__ */
void                DrawHorizontalLine (frame, thick, style, x, y, l, h, align, RO, active, fg)
int                 frame;
int                 thick;
int                 style;
int                 x;
int                 y;
int                 l;
int                 h;
int                 align;
int                 RO;
int                 active;
int                 fg;

#endif /* __STDC__ */

{
   register int        Y;

   if (align == 1)
      Y = y + (h - thick) / 2;
   else if (align == 2)
      Y = y + h - thick - 1;
   else
      Y = y;
   if (thick > 0)
     {
	InitDrawing (0, style, thick, RO, active, fg);
	DoDrawOneLine (frame, x, Y, x + l - 1, Y);
	FinishDrawing (0, RO, active);
     }
}

/*----------------------------------------------------------------------
  DrawVerticalLine draw a vertical line aligned left center or right
  depending on align value.
  RO indicates whether it's a read-only box
  active indicates if the box is active
  parameter fg indicates the drawing color
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                DrawVerticalLine (int frame, int thick, int style, int x, int y, int l, int h, int align, int RO, int active, int fg)

#else  /* __STDC__ */
void                DrawVerticalLine (frame, thick, style, x, y, l, h, align, RO, active, fg)
int                 frame;
int                 thick;
int                 style;
int                 x;
int                 y;
int                 l;
int                 h;
int                 align;
int                 RO;
int                 active;
int                 fg;

#endif /* __STDC__ */

{
   register int        X;

   if (align == 1)
      X = x + (l - thick) / 2;
   else if (align == 2)
      X = x + l - thick;
   else
      X = x;

   if (thick > 0)
     {
	InitDrawing (0, style, thick, RO, active, fg);
	DoDrawOneLine (frame, X, y, X, y + h);
	FinishDrawing (0, RO, active);
     }
}

/*----------------------------------------------------------------------
  DrawSlash draw a slash or backslash depending on direction.
  RO indicates whether it's a read-only box
  active indicates if the box is active
  Le parame`tre indique la couleur du trace'.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                DrawSlash (int frame, int thick, int style, int x, int y, int l, int h, int direction, int RO, int active, int fg)

#else  /* __STDC__ */
void                DrawSlash (frame, thick, style, x, y, l, h, direction, RO, active, fg)
int                 frame;
int                 thick;
int                 style;
int                 x;
int                 y;
int                 l;
int                 h;
int                 direction;
int                 RO;
int                 active;
int                 fg;

#endif /* __STDC__ */

{
   int                 xf, yf;

   xf = x + l - 1 - thick;
   yf = y + h - 1 - thick;
   if (thick > 0)
     {
	InitDrawing (0, style, thick, RO, active, fg);
	if (direction == 0)
	   DoDrawOneLine (frame, x, yf, xf, y);
	else
	   DoDrawOneLine (frame, x, y, xf, yf);
	FinishDrawing (0, RO, active);
     }
}

/*----------------------------------------------------------------------
  DrawCorner draw a corner.
  RO indicates whether it's a read-only box
  active indicates if the box is active
  parameter fg indicates the drawing color
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                DrawCorner (int frame, int thick, int style, int x, int y, int l, int h, int corner, int RO, int active, int fg)

#else  /* __STDC__ */
void                DrawCorner (frame, thick, style, x, y, l, h, corner, RO, active, fg)
int                 frame;
int                 thick;
int                 style;
int                 x;
int                 y;
int                 l;
int                 h;
int                 corner;
int                 RO;
int                 active;
int                 fg;

#endif /* __STDC__ */

{
   ThotPoint           point[3];
   int                 xf, yf;

#  ifdef _WINDOWS
   HPEN pen;
   HPEN hOldPen;
#  endif /* _WINDOWS */

   if (thick <= 0)
      return;

   x += FrameTable[frame].FrLeftMargin;
   y += FrameTable[frame].FrTopMargin;
   xf = x + l - thick;
   yf = y + h - thick;

   InitDrawing (0, style, thick, RO, active, fg);
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
#  ifndef _WINDOWS
   XDrawLines (TtDisplay, FrRef[frame], TtLineGC,
	       point, 3, CoordModeOrigin);
   FinishDrawing (0, RO, active);
#  else /* _WINDOWS */
   WIN_GetDeviceContext (frame);
   WinLoadGC (fg, RO);
   pen = CreatePen (PS_SOLID, 1, RGB (RGB_colors[TtLineGC.foreground].red, RGB_colors[TtLineGC.foreground].green, RGB_colors[TtLineGC.foreground].blue));
   hOldPen = SelectObject (TtDisplay, pen);
   Polyline (TtDisplay, point, 3);
   SelectObject (TtDisplay, hOldPen);
   WIN_ReleaseDeviceContext ();
   if (!DeleteObject (pen))
      WinErrorBox (WIN_Main_Wd);
#  endif /* _WINDOWS */
}

/*----------------------------------------------------------------------
  DrawRectangleFrame draw a rectangle with smoothed corners (3mm radius)
  and with an horizontal line at 6mm from top.
  RO indicates whether it's a read-only box
  active indicates if the box is active
  Parameters fg, bg, and pattern are for drawing
  color, background color and fill pattern.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                DrawRectangleFrame (int frame, int thick, int style, int x, int y, int width, int height, int RO, int active, int fg, int bg, int pattern)

#else  /* __STDC__ */
void                DrawRectangleFrame (frame, thick, style, x, y, width, height, RO, active, fg, bg, pattern)
int                 frame;
int                 thick;
int                 style;
int                 x;
int                 y;
int                 width;
int                 height;
int                 RO;
int                 active;
int                 fg;
int                 bg;
int                 pattern;
#endif /* __STDC__ */
{
#  ifndef _WINDOWS
   int                 arc, arc2, xf, yf;
   XArc                xarc[4];
   XSegment            seg[5];
   Pixmap              pat;
   ThotPoint           point[13];

   width -= thick;
   height -= thick;
   x += FrameTable[frame].FrLeftMargin + thick / 2;
   y += FrameTable[frame].FrTopMargin + thick / 2;
   /* radius of arcs is 3mm */
   arc = (3 * DOT_PER_INCHE) / 25.4 + 0.5;
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
   pat = CreatePattern (0, RO, active, fg, bg, pattern);

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

	XSetTile (TtDisplay, TtGreyGC, pat);
	XFillPolygon (TtDisplay, FrRef[frame], TtGreyGC,
		      point, 13, Convex, CoordModeOrigin);
	/* Trace quatre arcs de cercle */
	XFillArcs (TtDisplay, FrRef[frame], TtGreyGC, xarc, 4);
	XFreePixmap (TtDisplay, pat);
     }

   /* Draw the border */

   if (thick > 0)
     {
	InitDrawing (0, style, thick, RO, active, fg);
	XDrawArcs (TtDisplay, FrRef[frame], TtLineGC, xarc, 4);
	if (arc2 < height / 2)
	   XDrawSegments (TtDisplay, FrRef[frame], TtLineGC, seg, 5);
	else
	   XDrawSegments (TtDisplay, FrRef[frame], TtLineGC, seg, 4);
	FinishDrawing (0, RO, active);
     }
#  endif /* _WINDOWS */
}

/*----------------------------------------------------------------------
  DrawEllipsFrame draw an ellipse at 7mm under the top of the
  enclosing box.
  RO indicates whether it's a read-only box
  active indicates if the box is active
  Parameters fg, bg, and pattern are for drawing
  color, background color and fill pattern.
  ----------------------------------------------------------------------*/


#ifdef __STDC__
void                DrawEllipsFrame (int frame, int thick, int style, int x, int y, int width, int height, int RO, int active, int fg, int bg, int pattern)

#else  /* __STDC__ */
void                DrawEllipsFrame (frame, thick, style, x, y, width, height, RO, active, fg, bg, pattern)
int                 frame;
int                 thick;
int                 style;
int                 x;
int                 y;
int                 width;
int                 height;
int                 RO;
int                 active;
int                 fg;
int                 bg;
int                 pattern;

#endif /* __STDC__ */

{
#  ifndef _WINDOWS
   int                 px7mm, shiftX;
   double              A;
   Pixmap              pat;

   width -= thick + 1;
   height -= thick + 1;
   x += FrameTable[frame].FrLeftMargin + thick / 2;
   y += FrameTable[frame].FrTopMargin + thick / 2;

   /* Fill in the rectangle */
   pat = CreatePattern (0, RO, active, fg, bg, pattern);
   if (pat != 0)
     {
	XSetTile (TtDisplay, TtGreyGC, pat);
	XFillArc (TtDisplay, FrRef[frame], TtGreyGC,
		  x, y, width, height, 0, 360 * 64);
	XFreePixmap (TtDisplay, pat);
     }

   /* Draw the border */
   if (thick > 0)
     {
	InitDrawing (0, style, thick, RO, active, fg);
	XDrawArc (TtDisplay, FrRef[frame], TtLineGC,
		  x, y, width, height, 0, 360 * 64);

	px7mm = (7 * DOT_PER_INCHE) / 25.4 + 0.5;
	if (height > 2 * px7mm)
	  {
	     A = ((double) height - 2 * px7mm) / height;
	     A = 1.0 - sqrt (1 - A * A);
	     shiftX = width * A * 0.5 + 0.5;
	     XDrawLine (TtDisplay, FrRef[frame], TtLineGC,
		      x + shiftX, y + px7mm, x + width - shiftX, y + px7mm);
	  }
	FinishDrawing (0, RO, active);
     }
#  endif /* _WINDOWS */
}

/*----------------------------------------------------------------------
  StorePageInfo and psBoundingBox are empty, they have no meaning in
  this context and are kept for interface compatibility.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                StorePageInfo (int pagenum, int width, int height)
#else  /* __STDC__ */
void                StorePageInfo (pagenum, width, height)
int                 pagenum;
int                 width;
int                 height;
#endif /* __STDC__ */
{
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                psBoundingBox (int frame, int width, int height)
#else  /* __STDC__ */
void                psBoundingBox (frame, width, height)
int                 frame;
int                 width;
int                 height;
#endif /* __STDC__ */
{
}

/*----------------------------------------------------------------------
   SetMainWindowBackgroundColor :                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void         SetMainWindowBackgroundColor (int frame, int color)
#else  /* __STDC__ */
void         SetMainWindowBackgroundColor (frame, color)
int          frame;
int          color;
#endif /* __STDC__ */
{
#  ifndef _WINDOWS
   XSetWindowBackground (TtDisplay, FrRef[frame], ColorPixel (color));
#  else  /* _WINDOWS */
   WIN_GetDeviceContext (frame) ;
   SetBkColor (TtDisplay, ColorPixel (color)); 
   WIN_ReleaseDeviceContext ();
#  endif /* _WINDOWS */
}

/*----------------------------------------------------------------------
  Clear clear the area of frame located at (x, y) and of size width x height.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                Clear (int frame, int width, int height, int x, int y)
#else  /* __STDC__ */
void                Clear (frame, width, height, x, y)
int                 frame;
int                 width;
int                 height;
int                 x;
int                 y;
#endif /* __STDC__ */
{
   ThotWindow          w;

#  ifdef _WINDOWS
   HBRUSH              hBrush;
   HBRUSH              hOldBrush;
#  endif /* _WINDOWS */

   w = FrRef[frame];

   if (w != None)
     {
#       ifndef _WINDOWS
	XClearArea (TtDisplay, w, x + FrameTable[frame].FrLeftMargin, y + FrameTable[frame].FrTopMargin, width, height, FALSE);
#       else /* _WINDOWS */
	WIN_GetDeviceContext (frame);
	hBrush = CreateSolidBrush (ColorPixel (BackgroundColor[frame]));
	hOldBrush = SelectObject (TtDisplay, hBrush);
	PatBlt (TtDisplay, x + FrameTable[frame].FrLeftMargin, y + FrameTable[frame].FrTopMargin, width, height, PATCOPY);
	SelectObject (TtDisplay, hOldBrush);
    WIN_ReleaseDeviceContext ();
	if (!DeleteObject (hBrush))
       WinErrorBox (WIN_Main_Wd);
#       endif /* _WINDOWS */
     }
}
#endif /* _WIN_PRINT */

/*----------------------------------------------------------------------
  WChaine draw a string in frame, at location (x, y) and using font.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                WChaine (ThotWindow w, char *string, int x, int y, ptrfont font, ThotGC GClocal)
#else  /* __STDC__ */
void                WChaine (w, string, x, y, font, GClocal)
ThotWindow          w;
char               *string;
int                 x;
int                 y;
ptrfont             font;
ThotGC              GClocal;

#endif /* __STDC__ */
{
#  ifndef _WINDOWS
   XSetFont (TtDisplay, GClocal, ((XFontStruct *) font)->fid);
   FontOrig (font, string[0], &x, &y);
   XDrawString (TtDisplay, w, GClocal, x, y, string, strlen (string));
#  else /* _WINDOWS */
   HFONT hOldFont;

   WIN_GetDeviceContext (GetFrameNumber (w));
   SetMapperFlags (TtDisplay, 1);
   hOldFont = WinLoadFont(TtDisplay, font);
   TextOut(TtDisplay, x, y, string, strlen(string));
   SelectObject (TtDisplay, hOldFont);
#  endif /* _WINDOWS */
}


/*----------------------------------------------------------------------
  VideoInvert switch to inverse video the area of frame located at
  (x,y) and of size width x height.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                VideoInvert (int frame, int width, int height, int x, int y)
#else  /* __STDC__ */
void                VideoInvert (frame, width, height, x, y)
int                 frame;
int                 width;
int                 height;
int                 x;
int                 y;

#endif /* __STDC__ */
{
   ThotWindow          w;

   w = FrRef[frame];

   if (w != None)
     {
#       ifndef _WINDOWS
	XFillRectangle (TtDisplay, w, TtInvertGC, x + FrameTable[frame].FrLeftMargin, y + FrameTable[frame].FrTopMargin, width, height);
#       else /* _WINDOWS */
	WIN_GetDeviceContext (frame);
	PatBlt (TtDisplay, x + FrameTable[frame].FrLeftMargin, y + FrameTable[frame].FrTopMargin, width, height, PATINVERT);
    WIN_ReleaseDeviceContext ();
#       endif /* _WINDOWS */
     }
}


/*----------------------------------------------------------------------
  Scroll do a scrolling/Bitblt of frame of a width x height area
  from (xd,yd) to (xf,yf).
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void Scroll (int frame, int width, int height, int xd, int yd, int xf, int yf)
#else  /* __STDC__ */
void Scroll (frame, width, height, xd, yd, xf, yf)
int frame;
int width;
int height;
int xd;
int yd;
int xf;
int yf;
#endif /* __STDC__ */
{
   if (FrRef[frame] != None) {
#     ifndef _WINDOWS
      XCopyArea (TtDisplay, FrRef[frame], FrRef[frame], TtWhiteGC, xd + FrameTable[frame].FrLeftMargin, yd + FrameTable[frame].FrTopMargin, width, height, xf + FrameTable[frame].FrLeftMargin, yf + FrameTable[frame].FrTopMargin);
#     else  /* _WINDOWS */
	  RECT cltRect;
      WIN_GetDeviceContext (frame);

	  GetClientRect (FrRef [frame], &cltRect);
	  /*
      BitBlt (TtDisplay, xf + FrameTable[frame].FrLeftMargin, yf + FrameTable[frame].FrTopMargin, width, height,
	      TtDisplay, xd + FrameTable[frame].FrLeftMargin, yd + FrameTable[frame].FrTopMargin, SRCCOPY);
	  */
	  /* ScrollDC (TtDisplay, xf - xd, yf - yd, NULL, &cltRect, NULL, NULL);  */
	  ScrollWindowEx (FrRef [frame], xf - xd, yf - yd, NULL, &cltRect, NULL, NULL, SW_ERASE | SW_INVALIDATE); 
      WIN_ReleaseDeviceContext ();
      /*InvalidateRect (FrRef[frame], NULL, TRUE);*/
#     endif /* _WINDOWS */
   }
}

#ifndef _WIN_PRINT
/*----------------------------------------------------------------------
  EndOfString check wether string end by suffix.

  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                 EndOfString (char *string, char *suffix)
#else  /* __STDC__ */
int                 EndOfString (string, suffix)
char               *string;
char               *suffix;

#endif /* __STDC__ */
{
   int                 string_lenght, suffix_lenght;

   string_lenght = strlen (string);
   suffix_lenght = strlen (suffix);
   if (string_lenght < suffix_lenght)
      return 0;
   else
      return (strcmp (string + string_lenght - suffix_lenght, suffix) == 0);
}
#endif /* _WIN_PRINT */

/*----------------------------------------------------------------------
  XFlushOutput enforce updating of the calculated image for frame.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                XFlushOutput (int frame)
#else  /* __STDC__ */
void                XFlushOutput (frame)
int                 frame;

#endif /* __STDC__ */
{
#  ifndef _WINDOWS
   XFlush (TtDisplay);
#  endif /* _WINDOWS */
}

#ifndef _WIN_PRINT
/*----------------------------------------------------------------------
  PaintWithPattern fill the rectangle associated to a window w (or frame if w= 0)
  located on (x , y) and geometry width x height, using the
  given pattern.
  RO indicates whether it's a read-only box
  active indicates if the box is active
  Parameters fg, bg, and pattern are for drawing
  color, background color and fill pattern.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                PaintWithPattern (int frame, int x, int y, int width, int height, ThotWindow w, int RO, int active, int fg, int bg, int pattern)
#else  /* __STDC__ */
void                PaintWithPattern (frame, x, y, width, height, w, RO, active, fg, bg, pattern)
int                 frame;
int                 x;
int                 y;
int                 width;
int                 height;
ThotWindow          w;
int                 RO;
int                 active;
int                 fg;
int                 bg;
int                 pattern;

#endif /* __STDC__ */
{
#  ifndef _WINDOWS
   Pixmap              pat;

   /* Fill the rectangle associated to the given frame */
   pat = (Pixmap) CreatePattern (0, RO, active, fg, 0, pattern);
   if (pat != 0)
     {
	XSetTile (TtDisplay, TtGreyGC, pat);
	if (w != 0)
	   XFillRectangle (TtDisplay, w, TtGreyGC, x, y, width, height);
	else
	   XFillRectangle (TtDisplay, FrRef[frame], TtGreyGC, x + FrameTable[frame].FrLeftMargin, y + FrameTable[frame].FrTopMargin, width, height);
	XFreePixmap (TtDisplay, pat);
     }
#  endif /* _WINDOWS */
}
#endif /* _WIN_PRINT */
