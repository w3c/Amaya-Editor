/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 
/*
 * windowdisplay.c : handling of low level drawing routines, both for
 *                   MS-Windows (incomplete).
 *
 * Authors: I. Vatton (INRIA)
 *          R. Guetari (W3C/INROA) Unicode and Windows version
 *          D. Veillard (W3C/INRIA) - Windows 95/NT routines
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
#include "edit_tv.h"
#include "thotcolor_tv.h"

extern ThotColorStruct cblack;
extern int             ColorPs;

#define	MAX_STACK	50
#define	MIDDLE_OF(v1, v2)	(((v1)+(v2))/2.0)
#define SEG_SPLINE      5
#define ALLOC_POINTS    300
static ThotPoint   *points;	/* control points for curbs */
static int          npoints;
static int          MAX_points;
static int i;

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
#include "windowdisplay_f.h"

#ifdef _WIN_PRINT
/*----------------------------------------------------------------------
   CurrentColor compares the last RGB Postscript color loaded
   and the one asked, and load it if needed.
   num is an index in the Thot internal color table.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void  CurrentColor (FILE * fout, int num)
#else  /* __STDC__ */
static void  CurrentColor (fout, num)
FILE        *fout;
int          num;
#endif /* __STDC__ */
{
  unsigned short      red;
  unsigned short      green;
  unsigned short      blue;

  /* Compare the color asked with the current one */
  if (num != ColorPs)
    {
      /* Ask for the RedGreenBlue values */
      TtaGiveThotRGB (num, &red, &green, &blue);
      /* Emit the Poscript command */
      if (TtPrinterDC)
	SetTextColor (TtPrinterDC, RGB (red, green, blue));
      ColorPs = num;
    }
}

/*----------------------------------------------------------------------
  DoPrintOneLine draw one line starting from (x1, y1) to (x2, y2) in frame.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         DoPrintOneLine (int color, int x1, int y1, int x2, int y2, int thick, int style)
#else  /* __STDC__ */
static void         DoPrintOneLine (color, x1, y1, x2, y2, thick, style)
int                 color;
int                 x1;
int                 y1;
int                 x2;
int                 y2;
int                 thick; 
int                 style;
#endif /* __STDC__ */
{
   HPEN     pen;
   HPEN     hOldPen;
   LOGBRUSH logBrush;
   int      t;

   t = PixelToPoint (thick);
   logBrush.lbStyle = BS_SOLID;
   logBrush.lbColor = RGB (RGB_colors[color].red, RGB_colors[color].green, RGB_colors[color].blue);
   switch (style)
	 {
	 case 3:
	   pen = ExtCreatePen (PS_GEOMETRIC | PS_DOT | PS_ENDCAP_SQUARE, t, &logBrush, 0, NULL);
	   break;
	 case 4:
	   pen = ExtCreatePen (PS_GEOMETRIC | PS_DASH | PS_ENDCAP_SQUARE, t, &logBrush, 0, NULL); 
	   break;
	 default:
	   pen = ExtCreatePen (PS_GEOMETRIC | PS_SOLID | PS_ENDCAP_SQUARE, t, &logBrush, 0, NULL);   
	   break;
	 }
   hOldPen = SelectObject (TtPrinterDC, pen);
   MoveToEx (TtPrinterDC, x1, y1, NULL);
   LineTo (TtPrinterDC, x2, y2);

   SelectObject (TtPrinterDC, hOldPen);
   if (!DeleteObject (pen))
      WinErrorBox (WIN_Main_Wd, TEXT("windowdisplay.c - DoPrintOneLine"));
   pen = (HPEN) 0;
}

/*----------------------------------------------------------------------
  DrawArrowHead draw the end of an arrow.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         DrawArrowHead (int x1, int y1, int x2, int y2, int thick, int RO, int active, int fg)
#else  /* __STDC__ */
static void         DrawArrowHead (x1, y1, x2, y2, thick, RO, active, fg)
int                 x1, y1, x2, y2;
int                 thick;
int                 RO;
int                 active;
int                 fg;

#endif /* __STDC__ */
{
   float               x, y, xb, yb, dx, dy, l, sina, cosa;
   int                 xc, yc, xd, yd;
   float               width, height;
   Pixmap              pattern;
   HPEN                hPen;
   HPEN                hOldPen;
   ThotPoint           point[4];

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

   pattern = (Pixmap) CreatePattern (0, RO, active, fg, fg, 1);
   if (pattern != 0) {
      hPen = CreatePen (PS_SOLID, thick, RGB (RGB_colors[fg].red, RGB_colors[fg].green, RGB_colors[fg].blue));   

      hOldPen = SelectObject (TtPrinterDC, hPen);
      Polyline (TtPrinterDC, point, 4);
      SelectObject (TtPrinterDC, hOldPen);
	  if (!DeleteObject (hPen))
         WinErrorBox (WIN_Main_Wd, TEXT("windowdisplay.c - DrawArrowHead"));
      hPen = (HPEN) 0;
   }
}

#else /* _WIN_PRINT
/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void ClipError (int frame) 
#else  /* !__STDC__ */
void ClipError (frame); 
int  frame;
#endif /* __STDC__ */
{
    HWND parent = NULL;
    if (frame != -1)
       parent = FrMainRef [frame];
    MessageBox (parent, TEXT("Cannot select clipping region"), TEXT("Warning"), MB_OK);
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
   COLORREF cr;
   WIN_GetDeviceContext (frame);
   cr = ColorPixel (color);
   SetBkColor (TtDisplay, (COLORREF)ColorPixel (color)); 
   WIN_ReleaseDeviceContext ();
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
   TtLineGC.capabilities |= THOT_GC_FOREGROUND;
   if (RO && fg == 1)
      TtLineGC.foreground = RO_Color;
   else
       TtLineGC.foreground = fg;
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
    TtLineGC.thick = thick;
    TtLineGC.style = style;
   /* Load the correct color */
    LoadColor (disp, RO, active, fg);
}

/*----------------------------------------------------------------------
  SpaceToChar substitute in text the space chars to their visual
  equivalents.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         SpaceToChar (USTRING text)
#else  /* __STDC__ */
static void         SpaceToChar (text)
USTRING             text;
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
  HPEN     pen;
  HPEN     hOldPen;
  LOGBRUSH logBrush;

  WIN_GetDeviceContext (frame);
      logBrush.lbStyle = BS_SOLID;
      logBrush.lbColor = RGB (RGB_colors[TtLineGC.foreground].red, RGB_colors[TtLineGC.foreground].green, RGB_colors[TtLineGC.foreground].blue);
      
      switch (TtLineGC.style)
	{
	case 3:
	  pen = ExtCreatePen (PS_GEOMETRIC | PS_DOT | PS_ENDCAP_SQUARE, TtLineGC.thick, &logBrush, 0, NULL);
	  break;
	case 4:
	  pen = ExtCreatePen (PS_GEOMETRIC | PS_DASH | PS_ENDCAP_SQUARE, TtLineGC.thick, &logBrush, 0, NULL); 
	  break;
	default:
	  pen = ExtCreatePen (PS_GEOMETRIC | PS_SOLID | PS_ENDCAP_SQUARE, TtLineGC.thick, &logBrush, 0, NULL);   
	  break;
	}
  hOldPen = SelectObject (TtDisplay, pen);
  MoveToEx (TtDisplay, x1, y1, NULL);
  LineTo (TtDisplay, x2, y2);

  SelectObject (TtDisplay, hOldPen);
  WIN_ReleaseDeviceContext ();
  if (!DeleteObject (pen))
    WinErrorBox (WIN_Main_Wd, TEXT("windowdisplay.c - DoDrawOneLine"));
  pen = (HPEN) 0;
}

/*----------------------------------------------------------------------
  ArrowDrawing draw the end of an arrow.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         ArrowDrawing (int frame, int x1, int y1, int x2, int y2, int thick, int RO, int active, int fg)
#else  /* __STDC__ */
static void         ArrowDrawing (frame, x1, y1, x2, y2, thick, RO, active, fg)
int                 frame;
int                 x1, y1, x2, y2;
int                 thick;
int                 RO;
int                 active;
int                 fg;

#endif /* __STDC__ */
{
   float               x, y, xb, yb, dx, dy, l, sina, cosa;
   int                 xc, yc, xd, yd;
   float               width, height;
   Pixmap              pattern;
   HPEN                hPen;
   HPEN                hOldPen;
   ThotPoint           point[4];
   int                 result;

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
   xc = FloatToInt ((float)(x * cosa + y * sina + .5));
   yc = FloatToInt ((float)(-x * sina + y * cosa + .5));
   y = yb + width / 2;
   xd = FloatToInt ((float)(x * cosa + y * sina + .5));
   yd = FloatToInt ((float)(-x * sina + y * cosa + .5));

   /* draw */
   point[0].x = x2;
   point[0].y = y2;
   point[1].x = xc;
   point[1].y = yc;
   point[2].x = xd;
   point[2].y = yd;
   point[3].x = x2;
   point[3].y = y2;

   pattern = (Pixmap) CreatePattern (0, RO, active, fg, fg, 1);
   if (pattern != 0)
     {
      WIN_GetDeviceContext (frame);
      result = SelectClipRgn (TtDisplay, clipRgn);  
      if (result == ERROR)
         ClipError (frame);
      /* if (!GetClipRgn(TtDisplay, clipRgn))
         WinErrorBox (NULL); */
      WinLoadGC (TtDisplay, fg, RO);
      if (!(hPen = CreatePen (PS_SOLID, thick, ColorPixel (fg))))
         WinErrorBox (WIN_Main_Wd, TEXT("windowdisplay.c  - ArrowDrawing (1)"));
      hOldPen = SelectObject (TtDisplay, hPen) ;
      Polyline (TtDisplay, point, 4);
      SelectObject (TtDisplay, hOldPen);
      WIN_ReleaseDeviceContext ();
	  if (!DeleteObject (hPen))
         WinErrorBox (WIN_Main_Wd, TEXT("windowdisplay.c  - ArrowDrawing (2)"));
      hPen = (HPEN) 0;
     }
}
#endif /* _WIN_PRINT */

/*----------------------------------------------------------------------
  DrawChar draw a char at location (x, y) in frame and with font.
  RO indicates whether it's a read-only box active
  indicates if the box is active parameter fg indicates the drawing color
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void        DrawChar (UCHAR_T car, int frame, int x, int y, ptrfont font, int RO, int active, int fg)
#else  /* __STDC__ */
void        DrawChar (car, frame, x, y, font, RO, active, fg)
UCHAR_T     car;
int         frame;
int         x;
int         y;
ptrfont     font;
int         RO;
int         active;
int         fg;

#endif /* __STDC__ */
{
   CHAR_T              str[2] = {car, 0};
   HFONT               hOldFont;
#ifndef _WIN_PRINT
   ThotWindow          w;
   int                 result;

   w = FrRef[frame];
   if (w == None)
      return;

   LoadColor (0, RO, active, fg);
   WIN_GetDeviceContext (frame);
   WinLoadGC (TtDisplay, fg, RO);
   SetMapperFlags (TtDisplay, 1);
   hOldFont = WinLoadFont (TtDisplay, font);
   result = SelectClipRgn (TtDisplay, clipRgn); 
   if (result == ERROR)
      ClipError (frame);
   TextOut (TtDisplay, x, y + FrameTable[frame].FrTopMargin, (USTRING) str, 1);
   SelectObject (TtDisplay, hOldFont);
   DeleteObject (currentActiveFont);
   currentActiveFont = (HFONT)0;
   WIN_ReleaseDeviceContext ();

#else /* _WIN_PRINT */
   CurrentColor (NULL, fg);
   y += FrameTable[frame].FrTopMargin;
   WinLoadGC (TtPrinterDC, fg, RO);
   SetMapperFlags (TtPrinterDC, 1);
   hOldFont = WinLoadFont (TtPrinterDC, font);
   TextOut (TtPrinterDC, x, y, (USTRING) str, 1);   
   SelectObject (TtPrinterDC, hOldFont);
   DeleteObject (currentActiveFont);
   currentActiveFont = (HFONT)0;
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
  RO indicates whether it's a read-only box
  active indicates if the box is active
  parameter fg indicates the drawing color
  
  Returns the lenght of the string drawn.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                 DrawString (STRING buff, int i, int lg, int frame, int x, int y, ptrfont font, int lgboite, int bl, int hyphen, int debutbloc, int RO, int active, int fg, int shadow)
#else  /* __STDC__ */
int                 DrawString (buff, i, lg, frame, x, y, font, lgboite, bl, hyphen, debutbloc, RO, active, fg, shadow)
STRING              buff;
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
int                 shadow;
#endif /* __STDC__ */
{
   STRING              ptcar;
   int                 j, width;
   SIZE                size;
   HFONT               hOldFont;
#ifdef _WIN_PRINT
   int                 encoding, NonJustifiedWhiteSp;
   FILE               *fout;
   unsigned short      red, green, blue;
#else  /* _WIN_PRINT */
   ThotWindow          w;
   RECT                rect;
   int                 result;
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
   if (TtPrinterDC == NULL)
      fout = (FILE *) FrRef[frame];
   encoding = 0;
   if (y < 0)
      return 0;
   y += FrameTable[frame].FrTopMargin;
   /* NonJustifiedWhiteSp is > 0 if writing a fixed lenght is needed */
   /* and equal to 0 if a justified space is to be printed */

   NonJustifiedWhiteSp = debutbloc;
   /* Is this a new box ? */
   if (SameBox == 0)
     {
      /* Beginning of a new box */
      SameBox = 1;
      X = PixelToPoint (x);
      Y = PixelToPoint (y + FontBase (font));
      NbWhiteSp = 0;

      /* Do we need to change the current color ? */
      if (TtPrinterDC)
	{
	  TtaGiveThotRGB (fg, &red, &green, &blue);
	  SetTextColor (TtPrinterDC, RGB (red, green, blue));
	}

      /* Do we need to change the current font ? */
      if (TtPrinterDC)
         hOldFont = WinLoadFont (TtPrinterDC, font);
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
   if (TtPrinterDC) {
      SetMapperFlags (TtPrinterDC, 1);
      GetTextExtentPoint (TtPrinterDC, ptcar, lg, &size);
      width = size.cx;
      if (ptcar[0] == '\212' || ptcar[0] == '\12') {
         /* skip the Control return char */
         ptcar++;
         lg--;
	  }

      if (lg > 0)
         if (!TextOut (TtPrinterDC, x, y, (USTRING) ptcar, lg))
            WinErrorBox (NULL, TEXT("windowdisplay.c - DrawString (1)"));

      if (hyphen) /* draw the hyphen */
         if (!TextOut (TtPrinterDC, x + width, y, TEXT("\255"), 1))
            WinErrorBox (NULL, TEXT("windowdisplay.c - DrawString (2)"));
      if (lgboite != 0)
          SameBox = 0;
   }
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
   w = FrRef[frame];
   if (lg > 0 && w != None) {
      ptcar = &buff[i - 1];
      /* Dealing with BR tag for windows */
      WIN_GetDeviceContext (frame);
      SetMapperFlags (TtDisplay, 1);
      hOldFont = WinLoadFont (TtDisplay, font);
      GetTextExtentPoint (TtDisplay, ptcar, lg, &size);
      width = size.cx;

      result = SelectClipRgn (TtDisplay, clipRgn);  
      if (result == ERROR)
         ClipError (frame);
      /* if (!GetClipRgn(TtDisplay, clipRgn))
         WinErrorBox (NULL); */

      WinLoadGC (TtDisplay, fg, RO);
      if (!ShowSpace || shadow) {
         /* draw the spaces */
         ptcar = TtaAllocString (lg + 1);
         if (shadow) {
            /* replace each character by a star */
	        j = 0;
	        while (j < lg)
	              ptcar[j++] = TEXT('*');
	        ptcar[lg] = EOS;
		 } else {
                ustrncpy (ptcar, &buff[i - 1], lg);
                ptcar[lg] = EOS;
                SpaceToChar (ptcar);	/* substitute spaces */
		 } 
         GetClientRect (FrRef [frame], &rect);
         outOpt = 0;

#        if 0 /* ifdef _I18N_ */
         fontLangInfo = GetFontLanguageInfo (TtDisplay);

         if (fontLangInfo == GCP_ERROR) /* There is a Problem. */
            WinErrorBox (NULL, TEXT("windowdisplay.c - DrawString (1)"));

         if (fontLangInfo & GCP_DIACRITIC)
            infoFlag |= GCP_DIACRITIC;
         
         if (fontLangInfo & GCP_GLYPHSHAPE) {
            /* The font/language contains multiple glyphs per code point or per code point */
            /* combination (supports shaping and/or ligation), and the font contains advanced */
            /* glyph tables to provide extra glyphs for the extra shapes. If this value is given, */
            /* the lpGlyphs array must be used with the GetCharacterPlacement function and the */
            /* ETO_GLYPHINDEX value must be passed to the ExtTextOut function when the string is drawn. */
            infoFlag |= GCP_GLYPHSHAPE;
		 } 
         
         if (fontLangInfo & GCP_USEKERNING)
            /* The font contains a kerning table which can be used to provide better spacing */
            /* between the characters and glyphs. */
            infoFlag |= GCP_USEKERNING;
         
         if (fontLangInfo & GCP_REORDER)
            /* The language requires reordering for display--for example, Hebrew or Arabic. */
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

         GetCharacterPlacement (TtDisplay, ptcar, ustrlen (ptcar), GCP_MAXEXTENT, &results, infoFlag);

         ExtTextOut (TtDisplay, x, y + FrameTable[frame].FrTopMargin, outOpt, &rect, (USTRING) szNewText, lg, anDX); 
#        endif /* else  /* !_I18N_ */
         /* ExtTextOut (TtDisplay, x, y + FrameTable[frame].FrTopMargin, 0, &rect, (USTRING) ptcar, lg, NULL);  */
         TextOut (TtDisplay, x, y + FrameTable[frame].FrTopMargin, (USTRING) ptcar, lg);
/* #        endif /* !_I18N_ */
         TtaFreeMemory (ptcar);
      } else {
             if (ptcar[0] == TEXT('\212') || ptcar[0] == TEXT('\12')) {
                /* skip the Control return char */
                ptcar++;
               lg--;
			 }
             if (lg != 0) {
                /* GetClipRgn(TtDisplay, clipRgn); */
                outOpt = 0;

#               if 0 /* ifdef _I18N_ */
                fontLangInfo = GetFontLanguageInfo (TtDisplay);

                if (fontLangInfo == GCP_ERROR) /* There is a Problem. */
                   WinErrorBox (NULL, TEXT("windowdisplay.c - DrawString (2)"));

                if (fontLangInfo & GCP_DIACRITIC)
                   infoFlag |= GCP_DIACRITIC;
         
                if (fontLangInfo & GCP_GLYPHSHAPE) {
		           /* The font/language contains multiple glyphs per code point or per code point */
		           /* combination (supports shaping and/or ligation), and the font contains advanced */
                   /* glyph tables to provide extra glyphs for the extra shapes. If this value is given, */
                   /* the lpGlyphs array must be used with the GetCharacterPlacement function and the */
                   /* ETO_GLYPHINDEX value must be passed to the ExtTextOut function when the string is drawn. */
                   infoFlag |= GCP_GLYPHSHAPE;
				}  
         
                if (fontLangInfo & GCP_USEKERNING)
                   /* The font contains a kerning table which can be used to provide better spacing */
                   /* between the characters and glyphs. */
                   infoFlag |= GCP_USEKERNING;
         
                if (fontLangInfo & GCP_REORDER)
                   /* The language requires reordering for display--for example, Hebrew or Arabic. */
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
	            GetCharacterPlacement (TtDisplay, ptcar, ustrlen (ptcar), GCP_MAXEXTENT, &results, infoFlag);

                ExtTextOut (TtDisplay, x, y + FrameTable[frame].FrTopMargin, outOpt, &rect, (USTRING) szNewText, lg, anDX);
#               endif /* 00000 else  /* !_I18N_ */ */
                TextOut (TtDisplay, x, y + FrameTable[frame].FrTopMargin, (USTRING) ptcar, lg);
/* #               endif /* !_18N_ */
			 } 
	  } 
 
      if (hyphen) {
         /* draw the hyphen */
         /* GetClipRgn(TtDisplay, clipRgn); */
         TextOut (TtDisplay, x + width, y + FrameTable[frame].FrTopMargin, TEXT("\255"), 1);
	  } 
      SelectObject (TtDisplay, hOldFont);
      DeleteObject (currentActiveFont);
      currentActiveFont = (HFONT)0;
      /* WIN_ReleaseDeviceContext (); */
      return (width);
     } else
     return (0);
#endif /* _WIN_PRINT */
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
   int                 fheight;	/* font height           */
   int                 ascent;	/* font ascent           */
   int                 bottom;	/* underline position    */
   int                 middle;	/* cross-over position   */
   int                 height;	/* overline position     */
   int                 thickness;	/* thickness of drawing */
   int                 shift;	/* shifting of drawing   */
#  ifndef _WIN_PRINT
   ThotWindow          w;
#  endif  /* !_WIN_PRINT */

#ifdef _WIN_PRINT

   if (y < 0)
     return;
   y += FrameTable[frame].FrTopMargin;
   if (TtPrinterDC)
     {
       if (lg > 0)
	 {
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

         switch (type)
	   {
	   case 1: /* underlined */
	     DoPrintOneLine (fg, x - lg, bottom, x, bottom, thick, 5);
	     break;
	     
	   case 2: /* overlined */
	     DoPrintOneLine (fg, x - lg, height, x, height, thick, 5);
	     break;
	     
	   case 3: /* cross-over */
	     DoPrintOneLine (fg, x - lg, middle, x, middle, thick, 5);
	     break;
	     
	   default: /* not underlined */
	     break;
	   } 
	 } 
   }
#else  /* _WIN_PRINT */

   if (lg > 0)
     {
	w = FrRef[frame];

	if (w == None)
	   return;
	fheight = FontHeight (font);
	ascent = FontAscent (font);
	thickness = ((fheight / 20) + 1) * (thick + 1);
	shift = thick * thickness;
	y += FrameTable[frame].FrTopMargin;
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

	InitDrawing (0, 5, thickness, RO, active, fg);
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
     }
#endif /* _WIN_PRINT */
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

   int                 xcour;
#ifdef _WIN_PRINT
   FILE              *fout = NULL;
   HPEN               hPen, hOldPen;
   POINT              ptArray [2];
   unsigned short     r, g, b;
   int                 ycour;
#else /* _WIN_PRINT */
  ThotWindow          w;
  ptrfont             font;
  int                 width, nb;
  STRING              ptcar;
#endif /* _WIN_PRINT */

#ifdef _WIN_PRINT
   if (y < 0)
      return;
   y += FrameTable[frame].FrTopMargin;
   /* Do we need to change the current color ? */
   CurrentColor (fout, fg);

   if (lgboite > 0) {
	  xcour = x;
	  ycour = y;
      if (TtPrinterDC) {
         TtaGiveThotRGB (fg, &r, &g, &b);
         /* hPen = CreatePen (PS_DOT, 1, RGB (r, g, b)); */
         hPen = CreatePen (PS_DOT, 0, RGB ((BYTE) r, (BYTE) g, (BYTE) b)); 
		 hOldPen = SelectObject (TtPrinterDC, hPen);
         ptArray [0].x = xcour;
         ptArray [0].y = ycour;
         ptArray [1].x = xcour + lgboite;
         ptArray [1].y = ycour;
		 Polyline (TtPrinterDC, ptArray, 2);
		 SelectObject (TtPrinterDC, hOldPen);
		 DeleteObject (hPen);
	  }
   }
#else /* _WIN_PRINT */
  font = ThotLoadFont ('L', 't', 0, 6, UnPoint, frame);
  if (lgboite > 0)
    {
      w = FrRef[frame];
      ptcar = TEXT(" .");

      /* compute lenght of the string " ." */
      width = CharacterWidth (SPACE, font) + CharacterWidth (TEXT('.'), font);

      /* compute the number of string to write */
      nb = lgboite / width;
      xcour = x + (lgboite % width);
      y = y + FrameTable[frame].FrTopMargin - FontBase (font);
      LoadColor (0, RO, active, fg);

      /* draw the points */
      while (nb > 0)
	{
	  xcour += width;
	  nb--;
	}
    }
#endif /* _WIN_PRINT */
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

#ifdef _WIN_PRINT
   if (y < 0)
      return;
   y += FrameTable[frame].FrTopMargin;
   if (thick < 0)
      return;

   if (TtPrinterDC) {
      fh = FontHeight (font);
      xm = x + (fh / 2);
      xp = x + (fh / 4);
      /* vertical part */
      DoPrintOneLine (fg, x, y + (2 * (h / 3)), xp - (thick / 2), y + h, thick, 5);

      /* Acending part */
      DoPrintOneLine (fg, xp, y + h, xm, y, thick, 5);
      /* Upper part */
      DoPrintOneLine (fg, xm, y, x + l, y, thick, 5);
   }
#else /* _WIN_PRINT */
   fh = FontHeight (font);
   xm = x + (fh / 2);
   xp = x + (fh / 4);
   y += FrameTable[frame].FrTopMargin;
   InitDrawing (0, 5, 0, RO, active, fg);
   /* vertical part */
   DoDrawOneLine (frame, x, y + (2 * (h / 3)), xp - (thick / 2), y + h);

   InitDrawing (0, 5, thick, RO, active, fg);
   /* Acending part */
   DoDrawOneLine (frame, xp, y + h, xm, y);
   /* Upper part */
   DoDrawOneLine (frame, xm, y, x + l, y);
#endif /* _WIN_PRINT */
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

#ifdef _WIN_PRINT
   if (y < 0)
      return;
   y += FrameTable[frame].FrTopMargin;
   if (thick < 0)
      return;

   if (TtPrinterDC) {
      exnum = 0;
      if (FontHeight (font) *1.2 >= h) { /* display a single glyph */
         xm = x + ((l - CharacterWidth ('\362', font)) / 2);
         yf = y + ((h - CharacterHeight ('\362', font)) / 2) - FontAscent (font) +
         CharacterAscent ('\362', font);
         DrawChar ('\362', frame, xm, yf, font, RO, active, fg);
	  } else { /* Need more than one glyph */
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
           if (delta >= 0) {
              for (yf += asc, yend -= hd; yf < yend; yf += CharacterHeight ('\364', font), exnum++)
                  DrawChar ('\364', frame, xm+wd, yf, font, RO, active, fg);
              if (exnum)
                 DrawChar ('\364', frame, xm+wd, yend, font, RO, active, fg);
              else
                 DrawChar ('\364', frame, xm+wd, yf + ((delta - hd) / 2), font, RO, active, fg);
		   }
	  }

      if (type == 2) /* double integral */
         DrawIntegral (frame, thick, x + (CharacterWidth ('\364', font) / 2), y, l, h, -1, font, RO, active, fg);
       
      else if (type == 1) /* contour integral */
              DrawChar ('o', frame, x + ((l - CharacterWidth ('o', font)) / 2),
                            y + (h - CharacterHeight ('o', font)) / 2 - FontAscent (font) + CharacterAscent ('o', font),
                            font, RO, active, fg);
   }
#else  /* _WIN_PRINT */
   exnum = 0;
   if (FontHeight (font) *1.2 >= h)
     /* display a single glyph */
     {
     xm = x + ((l - CharacterWidth ('\362', font)) / 2);
     yf = y + ((h - CharacterHeight ('\362', font)) / 2) - FontAscent (font) +
	  CharacterAscent ('\362', font);
     DrawChar (TEXT('\362'), frame, xm, yf, font, RO, active, fg);
     }
   else
     {
     /* Need more than one glyph */
     xm = x + ((l - CharacterWidth ('\363', font)) / 2);
     yf = y - FontAscent (font) + CharacterAscent ('\363', font);
     DrawChar (TEXT('\363'), frame, xm, yf, font, RO, active, fg);
     yend = y + h - CharacterHeight ('\365', font) - FontAscent (font) +
	    CharacterAscent ('\365', font) - 1;
     DrawChar (TEXT('\365'), frame, xm, yend, font, RO, active, fg);

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
	   DrawChar (TEXT('\364'), frame, xm+wd, yf, font, RO, active, fg);
	if (exnum)
	   DrawChar (TEXT('\364'), frame, xm+wd, yend, font, RO, active, fg);
	else
	   DrawChar (TEXT('\364'), frame, xm+wd, yf + ((delta - hd) / 2), font, RO, active, fg);
       }
     }

   if (type == 2)		/* double integral */
      DrawIntegral (frame, thick, x + (CharacterWidth (TEXT('\364'), font) / 2),
		    y, l, h, -1, font, RO, active, fg);

   else if (type == 1)		/* contour integral */
      DrawChar (TEXT('o'), frame, x + ((l - CharacterWidth ('o', font)) / 2),
		y + (h - CharacterHeight ('o', font)) / 2 - FontAscent (font) + CharacterAscent ('o', font),
		font, RO, active, fg);
#endif /* _WIN_PRINT */
}

/*----------------------------------------------------------------------
  DrawMonoSymb draw a one glyph symbol.
  RO indicates whether it's a read-only box
  active indicates if the box is active
  parameter fg indicates the drawing color
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         DrawMonoSymb (CHAR_T symb, int frame, int x, int y, int l, int h, int RO, int active, ptrfont font, int fg)
#else  /* __STDC__ */
static void         DrawMonoSymb (symb, frame, x, y, l, h, RO, active, font, fg)
CHAR_T              symb;
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

   y += FrameTable[frame].FrTopMargin;
   xm = x + ((l - CharacterWidth (symb, font)) / 2);
   yf = y + ((h - CharacterHeight (symb, font)) / 2) - FontAscent (font) + CharacterAscent (symb, font);

   DrawChar (symb, frame, xm, yf, font, RO, active, fg);
}

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

#ifdef _WIN_PRINT
   if (y < 0)
      return;
   y += FrameTable[frame].FrTopMargin;

   if (TtPrinterDC)
     {
       fh = FontHeight (font);
       if (h < fh * 2 && l <= CharacterWidth ('\345', font)) /* Only one glyph needed */
         DrawMonoSymb ('\345', frame, x, y, l, h, RO, active, font, fg);
       else {
         xm = x + (l / 3);
         ym = y + (h / 2) - 1;
         /* Center */
         DoPrintOneLine (fg, x, y + 1, xm, ym, 1, 5);
         DoPrintOneLine (fg, x, y + h - 2, xm, ym, 1, 5);
	 
         /* Borders */
         DoPrintOneLine (frame, x, y, x + l, y, 1, 5);
         DoPrintOneLine (frame, x, y + h - 2, x + l, y + h - 2, 1, 5);
       }
     }
#else  /* !_WIN_PRINT */
   fh = FontHeight (font);
   if (h < fh * 2 && l <= CharacterWidth ('\345', font))
     {
	/* Only one glyph needed */
	DrawMonoSymb ('\345', frame, x, y, l, h, RO, active, font, fg);
     }
   else
     {
        y += FrameTable[frame].FrTopMargin;
	xm = x + (l / 3);
	ym = y + (h / 2) - 1;
	InitDrawing (0, 5, 0, RO, active, fg);
	/* Center */
	DoDrawOneLine (frame, x, y + 1, xm, ym);
	DoDrawOneLine (frame, x, y + h - 2, xm, ym);

	InitDrawing (0, 5, 2, RO, active, fg);
	/* Borders */
	DoDrawOneLine (frame, x, y, x + l, y);
	DoDrawOneLine (frame, x, y + h - 2, x + l, y + h - 2);
     }
#endif  /* !_WIN_PRINT */
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

#ifdef _WIN_PRINT
   if (y < 0)
      return;
   y += FrameTable[frame].FrTopMargin;

   if (TtPrinterDC) {
      fh = FontHeight (font);
      if (h < fh * 2 && l <= CharacterWidth ('\325', font)) /* Only one glyph needed */
         DrawMonoSymb ('\325', frame, x, y, l, h, RO, active, font, fg);
      else {
           /* Vertical part */
           DoPrintOneLine (fg, x + 2, y + 1, x + 2, y + h, 1, 5);
           DoPrintOneLine (fg, x + l - 3, y + 1, x + l - 3, y + h, 1, 5);
	 
           /* Upper part */
           DoPrintOneLine (frame, x + 1, y + 1, x + l, y, 1, 5);
	  }
   } 
#else /* _WIN_PRINT */
   fh = FontHeight (font);
   if (h < fh * 2 && l <= CharacterWidth ('\325', font))
     {
	/* Only one glyph needed */
	DrawMonoSymb ('\325', frame, x, y, l, h, RO, active, font, fg);
     }
   else
     {
        y += FrameTable[frame].FrTopMargin;
	InitDrawing (0, 5, 0, RO, active, fg);
	/* Vertical part */
	DoDrawOneLine (frame, x + 2, y + 1, x + 2, y + h);
	DoDrawOneLine (frame, x + l - 3, y + 1, x + l - 3, y + h);

	InitDrawing (0, 5, 2, RO, active, fg);
	/* Upper part */
	DoDrawOneLine (frame, x + 1, y + 1, x + l, y);
     }
#endif /* _WIN_PRINT */
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
   HPEN                pen;
   HPEN                hOldPen;

#ifdef _WIN_PRINT
   if (TtPrinterDC) {
      /* **** A FAIRE **** */
   }
#else  /* _WIN_PRINT */
   fh = FontHeight (font);
   if (h < fh * 2 && l <= CharacterWidth ('\307', font))
     {
	/* Only one glyph needed */
	DrawMonoSymb ('\307', frame, x, y, l, h, RO, active, font, fg);
     }
   else
     {
	y = y + FrameTable[frame].FrTopMargin;
	/* radius of arcs is 6mm */
	arc = h / 4;
	InitDrawing (0, 5, 2, RO, active, fg);
	/* vertical part */
	DoDrawOneLine (frame, x + 1, y + arc, x + 1, y + h);
	DoDrawOneLine (frame, x + l - 2, y + arc, x + l - 2, y + h);

	/* Upper part */
        pen = CreatePen (PS_SOLID, 1, RGB (RGB_colors[fg].red, RGB_colors[fg].green, RGB_colors[fg].blue));
        hOldPen = SelectObject (TtPrinterDC, pen);
        Arc (TtPrinterDC, x + 1, y + arc , x + l - 2, y, x + 1, y + arc, x + l - 2, y - arc);
        SelectObject (TtPrinterDC, hOldPen);
        if (!DeleteObject (pen))
          WinErrorBox (WIN_Main_Wd, TEXT("windowdisplay.c - DrawIntersection"));
        pen = (HPEN) 0;
     }
#endif /* _WIN_PRINT */
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
   HPEN                pen;
   HPEN                hOldPen;

#ifdef _WIN_PRINT 
   if (y < 0)
      return;
   y += FrameTable[frame].FrTopMargin;
   if (TtPrinterDC) {
      fh = FontHeight (font);
      if (h < fh * 2 && l <= CharacterWidth ('\310', font)) {
         /* Only one glyph needed */
         DrawMonoSymb ('\310', frame, x, y, l, h, RO, active, font, fg);
      } else {
             /* radius of arcs is 3mm */
             arc = h / 4;
             /* two vertical lines */
             DoPrintOneLine (fg, x + 1, y, x + 1, y + h - arc, 1, 5);
             DoPrintOneLine (fg, x + l - 2, y, x + l - 2, y + h - arc, 1, 5);
             /* Lower part */
             pen = CreatePen (PS_SOLID, 1, RGB (RGB_colors[fg].red, RGB_colors[fg].green, RGB_colors[fg].blue));
             hOldPen = SelectObject (TtPrinterDC, pen);
             Arc (TtPrinterDC, x + 1, y + h - arc , x + l - 2, y + h, x + 1, y + h - arc, x + l - 2, y + h - arc);
             SelectObject (TtPrinterDC, hOldPen);
             if (!DeleteObject (pen))
                WinErrorBox (WIN_Main_Wd, TEXT("windowdisplay.c - DrawUnion"));
             pen = (HPEN) 0;
	  }
   }
#else /* _WIN_PRINT */
   fh = FontHeight (font);
   if (h < fh * 2 && l <= CharacterWidth ('\310', font))
     {
	/* Only one glyph needed */
	DrawMonoSymb ('\310', frame, x, y, l, h, RO, active, font, fg);
     }
   else
     {
	y = y + FrameTable[frame].FrTopMargin;
	/* radius of arcs is 3mm */
	arc = h / 4;
	InitDrawing (0, 5, 2, RO, active, fg);
	/* two vertical lines */
	DoDrawOneLine (frame, x + 1, y, x + 1, y + h - arc);
	DoDrawOneLine (frame, x + l - 2, y, x + l - 2, y + h - arc);

	/* Lower part */
        pen = CreatePen (PS_SOLID, 1, RGB (RGB_colors[fg].red, RGB_colors[fg].green, RGB_colors[fg].blue));
        hOldPen = SelectObject (TtPrinterDC, pen);
	y += h;
        Arc (TtPrinterDC, x + 1, y - arc , x + l - 2, y, x + 1, y - arc, x + l - 2, y - arc);
        SelectObject (TtPrinterDC, hOldPen);
        if (!DeleteObject (pen))
          WinErrorBox (WIN_Main_Wd, TEXT("windowdisplay.c - DrawUnion"));
        pen = (HPEN) 0;
     }
#endif /* _WIN_PRINT */
}

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

#ifdef _WIN_PRINT
   if (thick <= 0)
      return;

   y += FrameTable[frame].FrTopMargin;
   xm = x + ((l - thick) / 2);
   xf = x + l - 1;
   ym = y + ((h - thick) / 2);
   yf = y + h - 1;

   if (orientation == 0) {
      /* draw a right arrow */
      DoPrintOneLine (fg, x, ym, xf, ym, thick, style);
      DrawArrowHead (x, ym, xf, ym, thick, RO, active, fg);
   } else if (orientation == 45) {
          DoPrintOneLine (fg, x, yf, xf - thick + 1, y, thick, style);
          DrawArrowHead (x, yf, xf - thick + 1, y, thick, RO, active, fg);
   } else if (orientation == 90) {
          /* draw a bottom-up arrow */
          DoPrintOneLine (fg, xm, y, xm, yf, thick, style);
          DrawArrowHead (xm, yf, xm, y, thick, RO, active, fg);
   } else if (orientation == 135) {
          DoPrintOneLine (fg, x, y, xf - thick + 1, yf, thick, style);
          DrawArrowHead (xf - thick + 1, yf, x, y, thick, RO, active, fg);
   } else if (orientation == 180) {
          /* draw a left arrow */
          DoPrintOneLine (fg, x, ym, xf, ym, thick, style);
          DrawArrowHead (xf, ym, x, ym, thick, RO, active, fg);
   } else if (orientation == 225) {
          DoPrintOneLine (frame, x, yf, xf - thick + 1, y, thick, style);
          DrawArrowHead (xf - thick + 1, y, x, yf, thick, RO, active, fg);
   } else if (orientation == 270) {
          /* draw a top-down arrow */
          DoPrintOneLine (fg, xm, y, xm, yf, thick, style);
          DrawArrowHead (xm, y, xm, yf, thick, RO, active, fg);
   } else if (orientation == 315) {
          DoPrintOneLine (frame, x, y, xf - thick + 1, yf, thick, style);
          DrawArrowHead (x, y, xf - thick + 1, yf, thick, RO, active, fg);
   } 
#else  /* _WIN_PRINT */
   if (thick <= 0)
      return;
   y += FrameTable[frame].FrTopMargin;
   xm = x + ((l - thick) / 2);
   xf = x + l - 1;
   ym = y + ((h - thick) / 2);
   yf = y + h - 1;

   InitDrawing (0, style, thick, RO, active, fg);
   if (orientation == 0)
     {
	/* draw a right arrow */
	DoDrawOneLine (frame, x, ym, xf, ym);
	ArrowDrawing (frame, x, ym, xf, ym, thick, RO, active, fg);
     }
   else if (orientation == 45)
     {
	DoDrawOneLine (frame, x, yf, xf - thick + 1, y);
	ArrowDrawing (frame, x, yf, xf - thick + 1, y, thick, RO, active, fg);
     }
   else if (orientation == 90)
     {
	/* draw a bottom-up arrow */
	DoDrawOneLine (frame, xm, y, xm, yf);
	ArrowDrawing (frame, xm, yf, xm, y, thick, RO, active, fg);
     }
   else if (orientation == 135)
     {
	DoDrawOneLine (frame, x, y, xf - thick + 1, yf);
	ArrowDrawing (frame, xf - thick + 1, yf, x, y, thick, RO, active, fg);
     }
   else if (orientation == 180)
     {
	/* draw a left arrow */
	DoDrawOneLine (frame, x, ym, xf, ym);
	ArrowDrawing (frame, xf, ym, x, ym, thick, RO, active, fg);
     }
   else if (orientation == 225)
     {
	DoDrawOneLine (frame, x, yf, xf - thick + 1, y);
	ArrowDrawing (frame, xf - thick + 1, y, x, yf, thick, RO, active, fg);
     }
   else if (orientation == 270)
     {
	/* draw a top-down arrow */
	DoDrawOneLine (frame, xm, y, xm, yf);
	ArrowDrawing (frame, xm, y, xm, yf, thick, RO, active, fg);
     }
   else if (orientation == 315)
     {
	DoDrawOneLine (frame, x, y, xf - thick + 1, yf);
	ArrowDrawing (frame, x, y, xf - thick + 1, yf, thick, RO, active, fg);
     }
#endif /* _WIN_PRINT */
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
	     DrawChar (TEXT('['), frame, xm, yf, font, RO, active, fg);
	  }
	else
	  {
	     /* Draw a closing bracket */
	     xm = x + ((l - CharacterWidth (']', font)) / 2);
	     yf = y + ((h - CharacterHeight (']', font)) / 2) -
		FontAscent (font) + CharacterAscent (']', font);
	     DrawChar (TEXT(']'), frame, xm, yf, font, RO, active, fg);
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
	     DrawChar (TEXT('\351'), frame, xm, yf, font, RO, active, fg);
	     yend = y + h - CharacterHeight ('\353', font) -
		FontAscent (font) + CharacterAscent ('\353', font);
	     DrawChar (TEXT('\353'), frame, xm, yend, font, RO, active, fg);
	     for (yf = yf + CharacterHeight ('\351', font) -
		  FontAscent (font) + CharacterAscent ('\352', font);
		  yf < yend;
		  yf += CharacterHeight ('\352', font))
		DrawChar (TEXT('\352'), frame, xm, yf, font, RO, active, fg);
	  }
	else
	  {
	     /* Draw a closing bracket */
	     xm = x + ((l - CharacterWidth ('\371', font)) / 2);
	     yf = y - FontAscent (font) + CharacterAscent ('\371', font);
	     DrawChar (TEXT('\371'), frame, xm, yf, font, RO, active, fg);
	     yend = y + h - CharacterHeight ('\373', font) -
		FontAscent (font) + CharacterAscent ('\373', font);
	     DrawChar (TEXT('\373'), frame, xm, yend, font, RO, active, fg);
	     for (yf = yf + CharacterHeight ('\371', font) -
		  FontAscent (font) + CharacterAscent ('\372', font);
		  yf < yend;
		  yf += CharacterHeight ('\372', font))
		DrawChar (TEXT('\372'), frame, xm, yf, font, RO, active, fg);
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

#ifdef _WIN_PRINT
   if (TtPrinterDC) {
      exnum = 0;

      if (h <= (int) (1.3 * FontHeight (font))) { /* With only one glyph */
		 if (direction == 0) { /* draw an opening parenthesis */
            xm = x + ((l - CharacterWidth ('(', font)) / 2);
            yf = y + ((h - CharacterHeight ('(', font)) / 2) - FontAscent (font) + CharacterAscent ('(', font);
            DrawChar ('(', frame, xm, yf, font, RO, active, fg);
		 } else { /* draw a closing parenthesis */
               xm = x + ((l - CharacterWidth (')', font)) / 2);
               yf = y + ((h - CharacterHeight (')', font)) / 2) - FontAscent (font) + CharacterAscent (')', font);
               DrawChar (')', frame, xm, yf, font, RO, active, fg);
		 }
	  } else { /* Need more than one glyph */
             if (direction == 0) {
                /* draw a opening parenthesis */
                xm = x + ((l - CharacterWidth ('\346', font)) / 2);
                yf = y - FontAscent (font) + CharacterAscent ('\346', font);
                DrawChar ('\346', frame, xm, yf, font, RO, active, fg);
                yend = y + h - CharacterHeight ('\350', font) - FontAscent (font) + CharacterAscent ('\350', font) - 1;
                DrawChar ('\350', frame, xm, yend, font, RO, active, fg);

                yf += CharacterHeight ('\346', font) - 1;
                delta = yend - yf;
                if (delta >= 0) {
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
			 } else {
                    /* draw a closing parenthesis */
                    xm = x + ((l - CharacterWidth ('\366', font)) / 2);
                    yf = y - FontAscent (font) + CharacterAscent ('\366', font);
                    DrawChar ('\366', frame, xm, yf, font, RO, active, fg);
                    yend = y + h - CharacterHeight ('\370', font) - FontAscent (font) + CharacterAscent ('\370', font) - 1;
                    DrawChar ('\370', frame, xm, yend, font, RO, active, fg);

                    yf += CharacterHeight ('\366', font) - 1;
                    delta = yend - yf;
                    if (delta >= 0) {
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
	     DrawChar (TEXT('('), frame, xm, yf, font, RO, active, fg);
	  }
	else
	  {
	     /* draw a closing parenthesis */
	     xm = x + ((l - CharacterWidth (')', font)) / 2);
	     yf = y + ((h - CharacterHeight (')', font)) / 2) - FontAscent (font) + CharacterAscent (')', font);
	     DrawChar (TEXT(')'), frame, xm, yf, font, RO, active, fg);
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
	     DrawChar (TEXT('\346'), frame, xm, yf, font, RO, active, fg);
	     yend = y + h - CharacterHeight ('\350', font) - FontAscent (font) + CharacterAscent ('\350', font) - 1;
	     DrawChar (TEXT('\350'), frame, xm, yend, font, RO, active, fg);

	     yf += CharacterHeight ('\346', font) - 1;
	     delta = yend - yf;
	     if (delta >= 0)
	       {
		  for (yf += CharacterAscent ('\347', font) - FontAscent (font),
		       yend -= CharacterHeight ('\347', font) - 1;
		       yf < yend;
		       yf += CharacterHeight ('\347', font), exnum++)
		     DrawChar (TEXT('\347'), frame, xm, yf, font, RO, active, fg);
		  if (exnum)
		     DrawChar (TEXT('\347'), frame, xm, yend, font, RO, active, fg);
		  else
		     DrawChar (TEXT('\347'), frame, xm, yf + ((delta - CharacterHeight ('\347', font)) / 2), font, RO, active, fg);
	       }
	  }

	else
	  {
	     /* draw a closing parenthesis */
	     xm = x + ((l - CharacterWidth ('\366', font)) / 2);
	     yf = y - FontAscent (font) + CharacterAscent ('\366', font);
	     DrawChar (TEXT('\366'), frame, xm, yf, font, RO, active, fg);
	     yend = y + h - CharacterHeight ('\370', font) - FontAscent (font) + CharacterAscent ('\370', font) - 1;
	     DrawChar (TEXT('\370'), frame, xm, yend, font, RO, active, fg);

	     yf += CharacterHeight ('\366', font) - 1;
	     delta = yend - yf;
	     if (delta >= 0)
	       {
		  for (yf += CharacterAscent ('\367', font) - FontAscent (font),
		       yend -= CharacterHeight ('\367', font) - 1;
		       yf < yend;
		       yf += CharacterHeight ('\367', font), exnum++)
		     DrawChar (TEXT('\367'), frame, xm, yf, font, RO, active, fg);
		  if (exnum)
		     DrawChar (TEXT('\367'), frame, xm, yend, font, RO, active, fg);
		  else
		     DrawChar (TEXT('\367'), frame, xm, yf + ((delta - CharacterHeight ('\367', font)) / 2), font, RO, active, fg);
	       }
	  }
     }
#endif /* _WIN_PRINT */
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

   if (h <= (int) (1.3 * FontHeight (font)) )
     {
	/* need only one char */
	if (direction == 0)
	  {
	     /* just use the opening brace glyph */
	     xm = x + ((l - CharacterWidth ('{', font)) / 2);
	     yf = y + ((h - CharacterHeight ('{', font)) / 2) - FontAscent (font) + CharacterAscent ('{', font);
	     DrawChar (TEXT('{'), frame, xm, yf, font, RO, active, fg);
	  }
	else
	  {
	     /* just use the closing brace glyph */
	     xm = x + ((l - CharacterWidth ('}', font)) / 2);
	     yf = y + ((h - CharacterHeight ('}', font)) / 2) - FontAscent (font) + CharacterAscent ('}', font);
	     DrawChar (TEXT('}'), frame, xm, yf, font, RO, active, fg);
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
	     DrawChar (TEXT('\354'), frame, xm, yf, font, RO, active, fg);
	     /* vertical line */
	     ym = y + ((h - CharacterHeight ('\355', font)) / 2) - FontAscent (font)
		+ CharacterAscent ('\355', font);
	     DrawChar (TEXT('\355'), frame, xm, ym, font, RO, active, fg);
	     /* bottom */
	     yend = y + h - CharacterHeight ('\356', font) - FontAscent (font) + CharacterAscent ('\356', font);
	     DrawChar (TEXT('\356'), frame, xm, yend, font, RO, active, fg);

	     /* finish top */
	     yf += CharacterHeight ('\354', font) - 1;
	     delta = ym - yf;
	     if (delta >= 0)
	       {
		  for (yf += CharacterAscent ('\357', font) - FontAscent (font),
		       ym -= CharacterHeight ('\357', font);
		       yf < ym;
		       yf += CharacterHeight ('\357', font), exnum++)
		     DrawChar (TEXT('\357'), frame, xm, yf, font, RO, active, fg);
		  if (exnum)
		     DrawChar (TEXT('\357'), frame, xm, ym, font, RO, active, fg);
		  else
		     DrawChar (TEXT('\357'), frame, xm, yf + ((delta - CharacterHeight ('\357', font)) / 2), font, RO, active, fg);
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
		     DrawChar (TEXT('\357'), frame, xm, yf, font, RO, active, fg);
		  if (exnum)
		     DrawChar (TEXT('\357'), frame, xm, yend, font, RO, active, fg);
		  else
		     DrawChar (TEXT('\357'), frame, xm, yf + ((delta - CharacterHeight ('\357', font)) / 2), font, RO, active, fg);
	       }
	  }

	else
	  {
	     /* top */
	     xm = x + ((l - CharacterWidth ('\374', font)) / 2);
	     yf = y - FontAscent (font) + CharacterAscent ('\374', font);
	     DrawChar (TEXT('\374'), frame, xm, yf, font, RO, active, fg);
	     /* center */
	     ym = y + ((h - CharacterHeight ('\375', font)) / 2)
		- FontAscent (font) + CharacterAscent ('\375', font);
	     DrawChar (TEXT('\375'), frame, xm, ym, font, RO, active, fg);
	     /* bottom */
	     yend = y + h - CharacterHeight ('\376', font)
		- FontAscent (font) + CharacterAscent ('\376', font);
	     DrawChar (TEXT('\376'), frame, xm, yend, font, RO, active, fg);
	     /* finish top */
	     yf += CharacterHeight ('\374', font) - 1;
	     delta = ym - yf;
	     if (delta >= 0)
	       {
		  for (yf += CharacterAscent ('\357', font) - FontAscent (font),
		       ym -= CharacterHeight ('\357', font);
		       yf < ym;
		       yf += CharacterHeight ('\357', font), exnum++)
		     DrawChar (TEXT('\357'), frame, xm, yf, font, RO, active, fg);
		  if (exnum)
		     DrawChar (TEXT('\357'), frame, xm, ym, font, RO, active, fg);
		  else
		     DrawChar (TEXT('\357'), frame, xm, yf + ((delta - CharacterHeight ('\357', font)) / 2), font, RO, active, fg);
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
		     DrawChar (TEXT('\357'), frame, xm, yf, font, RO, active, fg);
		  if (exnum)
		     DrawChar (TEXT('\357'), frame, xm, yend, font, RO, active, fg);
		  else
		     DrawChar (TEXT('\357'), frame, xm, yf + ((delta - CharacterHeight ('\357', font)) / 2), font, RO, active, fg);
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
#ifdef _WIN_PRINT
   int                 xf, yf;
#else  /* _WIN_PRINT */
   LOGBRUSH            logBrush;
   int                 result;
#endif /* _WIN_PRINT */
   Pixmap              pat = (Pixmap) 0;
   HBRUSH              hBrush;
   HBRUSH              hOldBrush;
   HPEN                hPen = 0;
   HPEN                hOldPen;

#ifdef _WIN_PRINT

   if (y < 0)   
      return;
   y += FrameTable[frame].FrTopMargin;

   /* Fill in the rectangle */
   if (TtPrinterDC) {
      xf = x + width;
      yf = y + height;
      if (width > thick + 1)
         width = width - thick - 1;
      if (height > thick + 1)
         height = height - thick - 1;
      x += thick / 2;
      y += thick / 2;

      pat = (Pixmap) CreatePattern (0, RO, active, fg, bg, pattern);
      if (pat != 0) {
         WinLoadGC (TtPrinterDC, fg, RO);
   
         hBrush = CreateSolidBrush (ColorPixel (bg));
         hOldBrush = SelectObject (TtPrinterDC, hBrush);
         PatBlt (TtPrinterDC, x, y, width, height, PATCOPY);
         SelectObject (TtPrinterDC, hOldBrush);
         if (!DeleteObject (hBrush))
            WinErrorBox (WIN_Main_Wd, TEXT("windowdisplay.c - DrawRectangle (1)"));
         hBrush = (HBRUSH) 0;
	  }

      if (thick > 0) {
         if (!(hPen = CreatePen (PS_SOLID, thick, ColorPixel (fg))))
            WinErrorBox (WIN_Main_Wd, TEXT("windowdisplay.c - DrawRectangle (2)"));
         hOldPen = SelectObject (TtPrinterDC, hPen) ;
         SelectObject (TtPrinterDC, GetStockObject (NULL_BRUSH)) ;
         Rectangle (TtPrinterDC, x, y, xf, yf);
         SelectObject (TtPrinterDC, hOldPen);
         DeleteObject (hPen);
	  }
   }
#else  /* _WIN_PRINT */
   if (width <= 0 || height <= 0)
      return;

   WIN_GetDeviceContext (frame);
   pat = (Pixmap) CreatePattern (0, RO, active, fg, bg, pattern);

   /* SelectClipRgn(TtDisplay, clipRgn); */
   if (pat == 0 && thick <= 0)
      return;

   if (width > thick + 1)
     width = width - thick - 1;
   if (height > thick + 1)
     height = height - thick - 1;
   x = x + (thick+1) / 2;
   y = y + (thick+1) / 2 + FrameTable[frame].FrTopMargin;

   WinLoadGC (TtDisplay, fg, RO);
   if (pat != 0) {
      if (pattern != 2) {
         hBrush = CreatePatternBrush (pat);
         hOldBrush = SelectObject (TtDisplay, hBrush);
	  } else {
             hBrush = CreateSolidBrush (ColorPixel (bg));
             hOldBrush = SelectObject (TtDisplay, hBrush);
	  }
   } else {
         SelectObject (TtDisplay, GetStockObject (NULL_BRUSH));
		 hBrush = (HBRUSH) 0;
   }

   if (thick > 0)
     {
       if (thick <= 1)
	 {
	   switch (style)
	     {
	     case 3:
	       hPen = CreatePen (PS_DOT, 1, RGB (RGB_colors[fg].red, RGB_colors[fg].green, RGB_colors[fg].blue));
	       break;
	     case 4:
	       hPen = CreatePen (PS_DASH, 1, RGB (RGB_colors[fg].red, RGB_colors[fg].green, RGB_colors[fg].blue)); 
	       break;
	     default:
	       hPen = CreatePen (PS_SOLID, 1, RGB (RGB_colors[fg].red, RGB_colors[fg].green, RGB_colors[fg].blue));   
	       break;
	     } 
	 }
       else
	 {
	   logBrush.lbStyle = BS_SOLID;
	   logBrush.lbColor = RGB (RGB_colors[fg].red, RGB_colors[fg].green, RGB_colors[fg].blue);

	   switch (style)
	     {
	     case 3:
	       hPen = ExtCreatePen (PS_GEOMETRIC | PS_DOT | PS_ENDCAP_SQUARE, thick, &logBrush, 0, NULL);
	       break;
	     case 4:
	       hPen = ExtCreatePen (PS_GEOMETRIC | PS_DASH | PS_ENDCAP_SQUARE, thick, &logBrush, 0, NULL); 
	       break;
	     default:
	       hPen = ExtCreatePen (PS_GEOMETRIC | PS_SOLID | PS_ENDCAP_SQUARE, thick, &logBrush, 0, NULL);   
	       break;
	     } 
	 }  
     }
   else if (!(hPen = CreatePen (PS_SOLID, thick, ColorPixel (bg))))
     WinErrorBox (WIN_Main_Wd, TEXT("windowdisplay.c - DrawRectangle (1)"));

   hOldPen = SelectObject (TtDisplay, hPen) ;
   result = SelectClipRgn (TtDisplay, clipRgn); 
   if (result == ERROR)
      ClipError (frame);
   if (!Rectangle (TtDisplay, x, y, x + width, y + FrameTable[frame].FrTopMargin + height))
      WinErrorBox (FrRef  [frame], TEXT("windowdisplay.c - DrawRectangle (2)"));
   SelectObject (TtDisplay, hOldPen);
   if (!DeleteObject (hPen))
      WinErrorBox (FrRef [frame], TEXT("windowdisplay.c - DrawRectangle (3)"));
   hPen = (HPEN) 0;
   if (hBrush) {
      SelectObject (TtDisplay, hOldBrush);
      if (!DeleteObject (hBrush))
         WinErrorBox (WIN_Main_Wd, TEXT("windowdisplay.c - DrawRectangle (4)"));
      hBrush = (HBRUSH)0;
   }
   WIN_ReleaseDeviceContext ();
   if (pat != (Pixmap)0)
      if (!DeleteObject ((HGDIOBJ)pat))
         WinErrorBox (NULL, TEXT("windowdisplay.c - DrawRectangle (5)"));
#endif /* _WIN_PRINT */
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
   ThotPoint          *points;
   int                 i, j;
   PtrTextBuffer       adbuff;

#ifdef _WIN_PRINT
   if (thick == 0)
      return;

   y += FrameTable[frame].FrTopMargin;
   /* Allocate a table of points */
   points = (ThotPoint *) TtaGetMemory (sizeof (ThotPoint) * (nb - 1));
   adbuff = buffer;
   j = 1;
   for (i = 1; i < nb; i++) {
       if (j >= adbuff->BuLength) {
	     if (adbuff->BuNext != NULL) {
            /* Next buffer */
            adbuff = adbuff->BuNext;
            j = 0;
		 }
	   }
       points[i - 1].x = x + PointToPixel (adbuff->BuPoints[j].XCoord / 1000);
       points[i - 1].y = y + PointToPixel (adbuff->BuPoints[j].YCoord / 1000);
       j++;
   }

   /* backward arrow  */
   if (arrow == 2 || arrow == 3)
      DrawArrowHead (points[1].x, points[1].y, points[0].x, points[0].y, thick, RO, active, fg);

   /* Draw the border */
   for (i = 1; i < nb - 1; i++) 
       DoPrintOneLine (fg, points [i-1].x, points[i-1].y, points[i].x, points[i].y, thick, style);

   /* Forward arrow */
   if (arrow == 1 || arrow == 3)
      DrawArrowHead (points[nb - 3].x, points[nb - 3].y, points[nb - 2].x, points[nb - 2].y, thick, RO, active, fg);

   /* free the table of points */
   free (points);
#else  /* _WIN_PRINT */
   if (thick == 0)
      return;

   /* Allocate a table of points */
   points = (ThotPoint *) TtaGetMemory (sizeof (ThotPoint) * (nb - 1));
   adbuff = buffer;
   y += FrameTable[frame].FrTopMargin;
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
	points[i - 1].x = x + PointToPixel (adbuff->BuPoints[j].XCoord / 1000);
	points[i - 1].y = y + PointToPixel (adbuff->BuPoints[j].YCoord / 1000);
	j++;
     }


   /* backward arrow  */
   if (arrow == 2 || arrow == 3)
      ArrowDrawing (frame, points[1].x, points[1].y, points[0].x, points[0].y, thick, RO, active, fg);

   /* Draw the border */
   InitDrawing (0, style, thick, RO, active, fg);
   for (i = 1; i < nb - 1; i++) {
       DoDrawOneLine (frame, points [i-1].x, points[i-1].y, points[i].x, points[i].y);
   }

   /* Forward arrow */
   if (arrow == 1 || arrow == 3)
      ArrowDrawing (frame, points[nb - 3].x, points[nb - 3].y, points[nb - 2].x, points[nb - 2].y, thick, RO, active, fg);

   /* free the table of points */
   free (points);
#endif /* _WIN_PRINT */
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
   HPEN                hPen;
   HPEN                hOldPen;
   HBRUSH              hBrush;
   HBRUSH              hOldBrush;
#  ifdef _WIN_PRINT
   LOGBRUSH            logBrush;
   int                 t;
#  else /* _WIN_PRINT */
   Pixmap              pat = (Pixmap) 0;
   int                 result;
#  endif /* _WIN_PRINT */

#ifdef _WIN_PRINT
   /* Allocate a table of points */
   points = (ThotPoint *) TtaGetMemory (sizeof (ThotPoint) * nb);
   adbuff = buffer;
   y += FrameTable[frame].FrTopMargin;
   j = 1;
   for (i = 1; i < nb; i++) {
       if (j >= adbuff->BuLength) {
          if (adbuff->BuNext != NULL) {
             /* Next buffer */
             adbuff = adbuff->BuNext;
             j = 0;
		  }
	   }
       points[i - 1].x = x + PointToPixel (adbuff->BuPoints[j].XCoord / 1000);
       points[i - 1].y = y + PointToPixel (adbuff->BuPoints[j].YCoord / 1000);
       j++;
   }
   /* Close the polygone */
   points[nb - 1].x = points[0].x;
   points[nb - 1].y = points[0].y;

   /* Fill in the polygone */

   /* Draw the border */
   if (thick > 0) {
     t = PixelToPoint (thick);

     if (t <= 1)
       {
	 switch (style)
	   {
	   case 3:
	     hPen = CreatePen (PS_DOT, 1, RGB (RGB_colors[fg].red, RGB_colors[fg].green, RGB_colors[fg].blue));
	     break;
	   case 4:
	     hPen = CreatePen (PS_DASH, 1, RGB (RGB_colors[fg].red, RGB_colors[fg].green, RGB_colors[fg].blue)); 
	     break;
	   default:
	     hPen = CreatePen (PS_SOLID, 1, RGB (RGB_colors[fg].red, RGB_colors[fg].green, RGB_colors[fg].blue));   
	     break;
	   }
       }
     else
       {
	 logBrush.lbStyle = BS_SOLID;
	 logBrush.lbColor = RGB (RGB_colors[fg].red, RGB_colors[fg].green, RGB_colors[fg].blue);

	 switch (style)
	   {
	   case 4:
	     hPen = ExtCreatePen (PS_GEOMETRIC | PS_DOT | PS_ENDCAP_SQUARE, thick, &logBrush, 0, NULL);
	     break;
	   case 5:
	     hPen = ExtCreatePen (PS_GEOMETRIC | PS_DASH | PS_ENDCAP_SQUARE, thick, &logBrush, 0, NULL); 
	     break;
	   default:
	     hPen = ExtCreatePen (PS_GEOMETRIC | PS_SOLID | PS_ENDCAP_SQUARE, thick, &logBrush, 0, NULL);   
	     break;
	   }
       } 
     hOldPen = SelectObject (TtPrinterDC, hPen) ;
     Polyline (TtPrinterDC, points, nb);
     SelectObject (TtPrinterDC, hOldPen);
     if (!DeleteObject (hPen))
       WinErrorBox (WIN_Main_Wd, TEXT("windowdisplay.c - DrawPolygon"));
     hPen = (HPEN) 0;
   }

   /* free the table of points */
   free (points);
#else  /* _WIN_PRINT */
   /* Allocate a table of points */
   points = (ThotPoint *) TtaGetMemory (sizeof (ThotPoint) * nb);
   adbuff = buffer;
   y += FrameTable[frame].FrTopMargin;
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
	points[i - 1].x = x + PointToPixel (adbuff->BuPoints[j].XCoord / 1000);
	points[i - 1].y = y + PointToPixel (adbuff->BuPoints[j].YCoord / 1000);
	j++;
     }
   /* Close the polygone */
   points[nb - 1].x = points[0].x;
   points[nb - 1].y = points[0].y;

   /* Fill in the polygone */
   pat = (Pixmap) CreatePattern (0, RO, active, fg, bg, pattern);


   /* Draw the border */
   if (thick > 0) {
      WIN_GetDeviceContext (frame);
      result = SelectClipRgn (TtDisplay, clipRgn);  
      if (result == ERROR)
         ClipError (frame);
      /* if (!GetClipRgn(TtDisplay, clipRgn))
         WinErrorBox (NULL); */
      WinLoadGC (TtDisplay, fg, RO);
      if (!(hPen = CreatePen (PS_SOLID, thick, ColorPixel (fg))))
         WinErrorBox (WIN_Main_Wd, TEXT("windowdisplay.c - DrawPolygon (1)"));
      hOldPen = SelectObject (TtDisplay, hPen) ;
      InitDrawing (0, style, thick, RO, active, fg);
      if (pat != (Pixmap)0) {
         hBrush = CreateSolidBrush (ColorPixel (bg));
         hOldBrush = SelectObject (TtDisplay, hBrush);
         Polygon (TtDisplay, points, nb);
	  }
      Polyline (TtDisplay, points, nb);
      SelectObject (TtDisplay, hOldPen);
      WIN_ReleaseDeviceContext ();
	  if (!DeleteObject (hPen))
         WinErrorBox (WIN_Main_Wd, TEXT("windowdisplay.c - DrawPolygon (2)"));
      hPen = (HPEN) 0;
   }

   if (hBrush) {
      SelectObject (TtDisplay, hOldBrush);
      if (!DeleteObject (hBrush))
         WinErrorBox (NULL, TEXT("windowdisplay.c - DrawSpline (3)"));
   }

   if (pat != (Pixmap)0)
      if (!DeleteObject ((HGDIOBJ) pat))
         WinErrorBox (NULL, TEXT("windowdisplay.c - DrawSpline (3)"));

   /* free the table of points */
   free (points);
#endif /* _WIN_PRINT */
}


/*----------------------------------------------------------------------
  PolyNewPoint : add a new point to the current polyline.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static ThotBool     PolyNewPoint (int x, int y)
#else  /* __STDC__ */
static ThotBool     PolyNewPoint (x, y)
int                 x, y;
#endif /* __STDC__ */
{
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
static ThotBool     PopStack (float *x1, float *y1, float *x2, float *y2, float *x3, float *y3, float *x4, float *y4)
#else  /* __STDC__ */
static ThotBool     PopStack (x1, y1, x2, y2, x3, y3, x4, y4)
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
#ifdef _WIN_PRINT
#else /* _WIN_PRINT */
   PtrTextBuffer       adbuff;
   int                 i, j;
   float               x1, y1, x2, y2;
   float               cx1, cy1, cx2, cy2;

   if (thick == 0)
      return;

   /* alloue la liste des points */
   npoints = 0;
   MAX_points = ALLOC_POINTS;

   points = (ThotPoint *) TtaGetMemory (sizeof (ThotPoint) * MAX_points);

   adbuff = buffer;
   y += FrameTable[frame].FrTopMargin;
   j = 1;
   x1 = (float) (x + PointToPixel (adbuff->BuPoints[j].XCoord / 1000));
   y1 = (float) (y + PointToPixel (adbuff->BuPoints[j].YCoord / 1000));
   j++;
   cx1 = (controls[j].lx * 3 + x1 - x) / 4 + x;
   cy1 = (controls[j].ly * 3 + y1 - y) / 4 + y;
   x2 = (float) (x + PointToPixel (adbuff->BuPoints[j].XCoord / 1000));
   y2 = (float) (y + PointToPixel (adbuff->BuPoints[j].YCoord / 1000));
   cx2 = (controls[j].lx * 3 + x2 - x) / 4 + x;
   cy2 = (controls[j].ly * 3 + y2 - y) / 4 + y;

   /* backward arrow  */
   if (arrow == 2 || arrow == 3)
      ArrowDrawing (frame, FloatToInt (cx1), FloatToInt (cy1), (int) x1, (int) y1, thick, RO, active, fg);

   for (i = 2; i < nb; i++)
     {
	PolySplit (x1, y1, cx1, cy1, cx2, cy2, x2, y2);

	/* skip to next points */
	x1 = x2;
	y1 = y2;
	cx1 = controls[i].rx + x;
	cy1 = controls[i].ry + y;
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
	     x2 = (float) (x + PointToPixel (adbuff->BuPoints[j].XCoord / 1000));
	     y2 = (float) (y + PointToPixel (adbuff->BuPoints[j].YCoord / 1000));
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
   PolyNewPoint ((int) x2, (int) y2);

   /* Draw the border */
   InitDrawing (0, style, thick, RO, active, fg);
   WIN_GetDeviceContext (frame);
   Polyline (TtDisplay, points, npoints) ;

   /* Forward arrow */
   if (arrow == 1 || arrow == 3)
      ArrowDrawing (frame, FloatToInt (cx2), FloatToInt (cy2), (int) x2, (int) y2, thick, RO, active, fg);

   /* free the table of points */
   WIN_ReleaseDeviceContext ();
   free (points);
#endif /* _WIN_PRINT */
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
#ifdef _WIN_PRINT
#else /* _WIN_PRINT */
   PtrTextBuffer       adbuff;
   int                 i, j;
   float               x1, y1, x2, y2;
   float               cx1, cy1, cx2, cy2;
   Pixmap              pat = (Pixmap) 0;
   HPEN                hPen;
   HPEN                hOldPen;
   HBRUSH              hBrush;
   HBRUSH              hOldBrush;
   int                 result;

   /* allocate the list of points */
   npoints = 0;
   MAX_points = ALLOC_POINTS;
   points = (ThotPoint *) TtaGetMemory (sizeof (ThotPoint) * MAX_points);

   adbuff = buffer;
   y += FrameTable[frame].FrTopMargin;
   j = 1;
   x1 = (float) (x + PointToPixel (adbuff->BuPoints[j].XCoord / 1000));
   y1 = (float) (y + PointToPixel (adbuff->BuPoints[j].YCoord / 1000));
   cx1 = controls[j].rx + x;
   cy1 = controls[j].ry + y;
   j++;
   x2 = (float) (x + PointToPixel (adbuff->BuPoints[j].XCoord / 1000));
   y2 = (float) (y + PointToPixel (adbuff->BuPoints[j].YCoord / 1000));
   cx2 = controls[j].lx + x;
   cy2 = controls[j].ly + y;

   for (i = 2; i < nb; i++)
     {
	PolySplit (x1, y1, cx1, cy1, cx2, cy2, x2, y2);

	/* next points */
	x1 = x2;
	y1 = y2;
	cx1 = controls[i].rx + x;
	cy1 = controls[i].ry + y;
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
	     x2 = (float) (x + PointToPixel (adbuff->BuPoints[j].XCoord / 1000));
	     y2 = (float) (y + PointToPixel (adbuff->BuPoints[j].YCoord / 1000));
	     cx2 = controls[i + 1].lx + x;
	     cy2 = controls[i + 1].ly + y;
	  }
	else
	  {
	     /* loop around the origin point */
	     x2 = (float) (x + PointToPixel (buffer->BuPoints[1].XCoord / 1000));
	     y2 = (float) (y + PointToPixel (buffer->BuPoints[1].YCoord / 1000));
	     cx2 = controls[1].lx + x;
	     cy2 = controls[1].ly + y;
	  }
     }

   /* close the polyline */
   PolySplit (x1, y1, cx1, cy1, cx2, cy2, x2, y2);
   PolyNewPoint ((int) x2, (int) y2);

   /* Fill in the polygone */
   pat = (Pixmap) CreatePattern (0, RO, active, fg, bg, pattern);

   /* Draw the border */
   if (thick > 0) {
      InitDrawing (0, style, thick, RO, active, fg);
      WIN_GetDeviceContext (frame);
      result = SelectClipRgn (TtDisplay, clipRgn);  
      if (result == ERROR)
         ClipError (frame);
      /* if (!GetClipRgn(TtDisplay, clipRgn))
         WinErrorBox (NULL); */
      WinLoadGC (TtDisplay, fg, RO);
      if (!(hPen = CreatePen (PS_SOLID, thick, ColorPixel (fg))))
         WinErrorBox (WIN_Main_Wd, TEXT("windowdisplay.c - DrawSpline (1)"));
      hOldPen = SelectObject (TtDisplay, hPen) ;
      InitDrawing (0, style, thick, RO, active, fg);
      if (pat != (Pixmap)0) {
         hBrush = CreateSolidBrush (ColorPixel (bg));
         hOldBrush = SelectObject (TtDisplay, hBrush);
         Polygon (TtDisplay, points, npoints);
	  }
      Polyline (TtDisplay, points, npoints);
	  SelectObject (TtDisplay, hOldPen);
      WIN_ReleaseDeviceContext ();
	  if (!DeleteObject (hPen))
         WinErrorBox (WIN_Main_Wd, TEXT("windowdisplay.c - DrawSpline (2)"));
      hPen = (HPEN) 0;
   }

   if (hBrush) {
      SelectObject (TtDisplay, hOldBrush);
      if (!DeleteObject (hBrush))
         WinErrorBox (NULL, TEXT("windowdisplay.c - DrawSpline (3)"));
   }

   if (pat != (Pixmap)0)
      if (!DeleteObject ((HGDIOBJ) pat))
         WinErrorBox (NULL, TEXT("windowdisplay.c - DrawSpline (4)"));

   /* free the table of points */
   free (points);
#endif /* _WIN_PRINT */
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
   Pixmap              pat = (Pixmap) 0;
   int                 arc;
   HBRUSH              hBrush = (HBRUSH)0;
   HBRUSH              hOldBrush;
   LOGBRUSH            logBrush;
   HPEN                hPen = 0;
   HPEN                hOldPen;
#ifdef _WIN_PRINT 
   int                 t;
#else /* _WIN_PRINT */
   int                 result;
#endif /* _WIN_PRINT */

#  ifdef _WIN_PRINT 
   if (width <= 0 || height <= 0) 
      return;

   pat = (Pixmap) CreatePattern (0, RO, active, fg, bg, pattern);

   if (thick == 0 && pat == 0)
      return;

   y += FrameTable[frame].FrTopMargin;

   arc = (int) ((3 * DOT_PER_INCHE) / 25.4 + 0.5);

   if (width > thick + 1)
      width = width - thick - 1;
   if (height > thick + 1)
      height = height - thick - 1;
   x += thick / 2;
   y += thick / 2;

   /* Fill in the rectangle */
   if (pat != 0) {
      hBrush = CreateSolidBrush (ColorPixel (bg));
      hOldBrush = SelectObject (TtPrinterDC, hBrush);
   } else {
         SelectObject (TtPrinterDC, GetStockObject (NULL_BRUSH));
		 hBrush = (HBRUSH) 0;
   }

   if (thick > 0)
     {
       t = PixelToPoint (thick);

       if (t <= 1)
	 {
	   switch (style)
	     {
	     case 3:
	       hPen = CreatePen (PS_DOT, 1, RGB (RGB_colors[fg].red, RGB_colors[fg].green, RGB_colors[fg].blue));
	       break;
	     case 4:
	       hPen = CreatePen (PS_DASH, 1, RGB (RGB_colors[fg].red, RGB_colors[fg].green, RGB_colors[fg].blue)); 
	       break;
	     default:
	       hPen = CreatePen (PS_SOLID, 1, RGB (RGB_colors[fg].red, RGB_colors[fg].green, RGB_colors[fg].blue));   
	       break;
	     } 
	 }
       else
	 {
	   logBrush.lbStyle = BS_SOLID;
	   logBrush.lbColor = RGB (RGB_colors[fg].red, RGB_colors[fg].green, RGB_colors[fg].blue);

	   switch (style)
	     {
	     case 3:
	       hPen = ExtCreatePen (PS_GEOMETRIC | PS_DOT | PS_ENDCAP_SQUARE, thick, &logBrush, 0, NULL);
	       break;
	     case 4:
	       hPen = ExtCreatePen (PS_GEOMETRIC | PS_DASH | PS_ENDCAP_SQUARE, thick, &logBrush, 0, NULL); 
	       break;
	     default:
	       hPen = ExtCreatePen (PS_GEOMETRIC | PS_SOLID | PS_ENDCAP_SQUARE, thick, &logBrush, 0, NULL);   
	       break;
	     } 
	 }  
     }
   else
     {
       if (!(hPen = CreatePen (PS_SOLID, thick, ColorPixel (bg))))
	 WinErrorBox (WIN_Main_Wd, TEXT("windowdisplay.c - DrawOval (1)"));
     }

   hOldPen = SelectObject (TtPrinterDC, hPen) ;

   if (!RoundRect (TtPrinterDC, x, y, x + width, y + height, arc * 2, arc * 2))
      WinErrorBox (FrRef  [frame], TEXT("windowdisplay.c - DrawOval (2)"));
   SelectObject (TtPrinterDC, hOldPen);
   if (!DeleteObject (hPen))
      WinErrorBox (FrRef [frame], TEXT("windowdisplay.c - DrawOval (3)"));
   hPen = (HPEN) 0;
   if (hBrush) {
      SelectObject (TtPrinterDC, hOldBrush);
      if (!DeleteObject (hBrush))
         WinErrorBox (WIN_Main_Wd, TEXT("windowdisplay.c - DrawOval (4)"));
      hBrush = (HBRUSH)0;
   }
#else  /* _WIN_PRINT */
   if (width <= 0 || height <= 0) 
      return;

   pat = (Pixmap) CreatePattern (0, RO, active, fg, bg, pattern);

   if (thick == 0 && pat == 0)
      return;

   WIN_GetDeviceContext (frame);

   arc = (int) ((3 * DOT_PER_INCHE) / 25.4 + 0.5);

   if (width > thick + 1)
     width = width - thick - 1;
   if (height > thick + 1)
     height = height - thick - 1;
   x += thick / 2;
   y = y + thick / 2 + FrameTable[frame].FrTopMargin;

   /* Fill in the rectangle */
   WinLoadGC (TtDisplay, fg, RO);
   if (pat != 0) {
      hBrush = CreateSolidBrush (ColorPixel (bg));
      hOldBrush = SelectObject (TtDisplay, hBrush);
   } else {
         SelectObject (TtDisplay, GetStockObject (NULL_BRUSH));
		 hBrush = (HBRUSH) 0;
   }

   if (thick > 0)
     {
       if (thick <= 1)
	 {
	   switch (style)
	     {
	     case 3:
	       hPen = CreatePen (PS_DOT, 1, RGB (RGB_colors[fg].red, RGB_colors[fg].green, RGB_colors[fg].blue));
	       break;
	     case 4:
	       hPen = CreatePen (PS_DASH, 1, RGB (RGB_colors[fg].red, RGB_colors[fg].green, RGB_colors[fg].blue)); 
	       break;
	     default:
	       hPen = CreatePen (PS_SOLID, 1, RGB (RGB_colors[fg].red, RGB_colors[fg].green, RGB_colors[fg].blue));   
	       break;
	     } 
	 }
       else
	 {
	   logBrush.lbStyle = BS_SOLID;
	   logBrush.lbColor = RGB (RGB_colors[fg].red, RGB_colors[fg].green, RGB_colors[fg].blue);

	   switch (style)
	     {
	     case 3:
	       hPen = ExtCreatePen (PS_GEOMETRIC | PS_DOT | PS_ENDCAP_SQUARE, thick, &logBrush, 0, NULL);
	       break;
	     case 4:
	       hPen = ExtCreatePen (PS_GEOMETRIC | PS_DASH | PS_ENDCAP_SQUARE, thick, &logBrush, 0, NULL); 
	       break;
	     default:
	       hPen = ExtCreatePen (PS_GEOMETRIC | PS_SOLID | PS_ENDCAP_SQUARE, thick, &logBrush, 0, NULL);   
	       break;
	     } 
	 }  
     }
   else if (!(hPen = CreatePen (PS_SOLID, thick, ColorPixel (bg))))
     WinErrorBox (WIN_Main_Wd, TEXT("windowdisplay.c - DrawOval (1)"));

   hOldPen = SelectObject (TtDisplay, hPen) ;
   result = SelectClipRgn (TtDisplay, clipRgn); 
   if (result == ERROR)
      ClipError (frame);
   if (!RoundRect (TtDisplay, x, y, x + width, y + height, arc * 2, arc * 2))
      WinErrorBox (FrRef  [frame], TEXT("windowdisplay.c - DrawOval (2)"));
   SelectObject (TtDisplay, hOldPen);
   if (!DeleteObject (hPen))
      WinErrorBox (FrRef [frame], TEXT("windowdisplay.c - DrawOval (3)"));
   hPen = (HPEN) 0;
   if (hBrush) {
      SelectObject (TtDisplay, hOldBrush);
      if (!DeleteObject (hBrush))
         WinErrorBox (WIN_Main_Wd, TEXT("windowdisplay.c - DrawOval (4)"));
      hBrush = (HBRUSH)0;
   }
   WIN_ReleaseDeviceContext ();
   
   if (pat != (Pixmap) 0)
      if (!DeleteObject ((HGDIOBJ) pat))
         WinErrorBox (NULL, TEXT("windowdisplay.c - DrawOval (5)"));
#endif /* _WIN_PRINT */
}

/*----------------------------------------------------------------------
  DrawEllips draw an ellips (or a circle).
  RO indicates whether it's a read-only box active indicates if the box
  is active.
  Parameters fg, bg, and pattern are for drawing color, background color
  and fill pattern.
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
   Pixmap   pat = (Pixmap)0;
   HPEN     hPen;
   HPEN     hOldPen;
   HBRUSH   hBrush;
   HBRUSH   hOldBrush;
   LOGBRUSH logBrush;
#ifdef _WIN_PRINT 
   int      t;
   int      xm, ym;    
#else  /* _WIN_PRINT */
   int      result;
#endif  /* _WIN_PRINT */

#ifdef _WIN_PRINT 
   if (y < 0)   
      return;
   y += FrameTable[frame].FrTopMargin;

   if (TtPrinterDC) {
      if (pattern > 2)
         pat = (Pixmap) CreatePattern (0, RO, active, fg, bg, pattern);

      if (pattern > 2 && pat == 0 && thick <= 0)
         return;

      if (pattern <= 2) {
         switch (pattern) {
                case 0:  SelectObject (TtPrinterDC, GetStockObject (NULL_BRUSH));
                         hBrush = (HBRUSH) 0;
                         break;

                case 1:  hBrush = CreateSolidBrush (ColorPixel (fg));
                         hOldBrush = SelectObject (TtPrinterDC, hBrush);
                         break;

                case 2:  hBrush = CreateSolidBrush (ColorPixel (bg));
                         hOldBrush = SelectObject (TtPrinterDC, hBrush);
                         break;

                default: SelectObject (TtPrinterDC, GetStockObject (NULL_BRUSH));
                         hBrush = (HBRUSH) 0;
                         break;
		 }
	  } else if (pat != 0) {
             hBrush = CreatePatternBrush (pat); 
             hOldBrush = SelectObject (TtPrinterDC, hBrush);
	  } else {
             SelectObject (TtPrinterDC, GetStockObject (NULL_BRUSH));
		     hBrush = (HBRUSH) 0;
	  }

      xm = x + width;
      ym = y + height;

      if (thick > 0)
	{
	  t = PixelToPoint (thick);
	  
	  if (t <= 1)
	    {
	      switch (style)
		{
		case 3:
		  hPen = CreatePen (PS_DOT, 1, RGB (RGB_colors[fg].red, RGB_colors[fg].green, RGB_colors[fg].blue));
		  break;
		case 4:
		  hPen = CreatePen (PS_DASH, 1, RGB (RGB_colors[fg].red, RGB_colors[fg].green, RGB_colors[fg].blue)); 
		  break;
		default:
		  hPen = CreatePen (PS_SOLID, 1, RGB (RGB_colors[fg].red, RGB_colors[fg].green, RGB_colors[fg].blue));   
		  break;
		}  
	    }
	  else
	    {
	      logBrush.lbStyle = BS_SOLID;
	      logBrush.lbColor = RGB (RGB_colors[fg].red, RGB_colors[fg].green, RGB_colors[fg].blue);
	      
	      switch (style)
		{
		case 3:
		  hPen = ExtCreatePen (PS_GEOMETRIC | PS_DOT | PS_ENDCAP_SQUARE, thick, &logBrush, 0, NULL);
		 break;
		case 4:
		  hPen = ExtCreatePen (PS_GEOMETRIC | PS_DASH | PS_ENDCAP_SQUARE, thick, &logBrush, 0, NULL); 
		  break;
		default:
		  hPen = ExtCreatePen (PS_GEOMETRIC | PS_SOLID | PS_ENDCAP_SQUARE, thick, &logBrush, 0, NULL);   
		  break;
		} 
	    }  
	}
      else
	{
	  if (!(hPen = CreatePen (PS_SOLID, thick, ColorPixel (bg))))
	    WinErrorBox (WIN_Main_Wd, TEXT("windowdisplay.c - DrawEllips (1)"));
	}

      hOldPen = SelectObject (TtPrinterDC, hPen) ;

      if (!Ellipse (TtPrinterDC, x, y, x + width, y + height))
         WinErrorBox (FrRef  [frame], TEXT("windowdisplay.c - DrawEllips (2)"));
      SelectObject (TtPrinterDC, hOldPen);
      if (!DeleteObject (hPen))
         WinErrorBox (FrRef [frame], TEXT("windowdisplay.c - DrawEllipse (3)"));
      hPen = (HPEN) 0;
      if (hBrush) {
         SelectObject (TtPrinterDC, hOldBrush);
         if (!DeleteObject (hBrush))
            WinErrorBox (WIN_Main_Wd, TEXT("windowdisplay.c - DrawEllips (4)"));
         hBrush = (HBRUSH)0;
	  }
   }
#else /* _WIN_PRINT */
   width -= thick + 1;
   height -= thick + 1;
   x += thick / 2;
   y = y + thick / 2 + FrameTable[frame].FrTopMargin;

   /* Fill in the rectangle */

   if (pattern > 2)
      pat = (Pixmap) CreatePattern (0, RO, active, fg, bg, pattern);

   if (pattern > 2 && pat == 0 && thick <= 0)
      return;

   WIN_GetDeviceContext (frame);

   WinLoadGC (TtDisplay, fg, RO);

   if (pattern <= 2) {
      switch (pattern) {
             case 0:  SelectObject (TtDisplay, GetStockObject (NULL_BRUSH));
                      hBrush = (HBRUSH) 0;
                      break;

             case 1:  hBrush = CreateSolidBrush (ColorPixel (fg));
                      hOldBrush = SelectObject (TtDisplay, hBrush);
                      break;

             case 2:  hBrush = CreateSolidBrush (ColorPixel (bg));
                      hOldBrush = SelectObject (TtDisplay, hBrush);
                      break;

             default: SelectObject (TtDisplay, GetStockObject (NULL_BRUSH));
                      hBrush = (HBRUSH) 0;
                      break;
	  }
   } else if (pat != 0) {
      hBrush = CreatePatternBrush (pat); 
      /* hBrush = CreateSolidBrush (ColorPixel (pattern)); */
      hOldBrush = SelectObject (TtDisplay, hBrush);
   } else {
         SelectObject (TtDisplay, GetStockObject (NULL_BRUSH));
		 hBrush = (HBRUSH) 0;
   }

   if (thick > 0)
     {
       if (thick <= 1)
	 {
	   switch (style)
	     {
	     case 0:
	       hPen = CreatePen (PS_SOLID, 1, RGB (RGB_colors[fg].red, RGB_colors[fg].green, RGB_colors[fg].blue));   
	       break;
	     case 1:
	       hPen = CreatePen (PS_DASH, 1, RGB (RGB_colors[fg].red, RGB_colors[fg].green, RGB_colors[fg].blue)); 
	       break;
	     default:
	       hPen = CreatePen (PS_DOT, 1, RGB (RGB_colors[fg].red, RGB_colors[fg].green, RGB_colors[fg].blue));
	       break;
	     } 
	 }
       else
	 {
	   logBrush.lbStyle = BS_SOLID;
	   logBrush.lbColor = RGB (RGB_colors[fg].red, RGB_colors[fg].green, RGB_colors[fg].blue);

	   switch (style)
	     {
	     case 0:
	       hPen = ExtCreatePen (PS_GEOMETRIC | PS_SOLID | PS_ENDCAP_SQUARE, thick, &logBrush, 0, NULL);   
	       break;
	     case 1:
	       hPen = ExtCreatePen (PS_GEOMETRIC | PS_DASH | PS_ENDCAP_SQUARE, thick, &logBrush, 0, NULL); 
	       break;
	     default:
	       hPen = ExtCreatePen (PS_GEOMETRIC | PS_DOT | PS_ENDCAP_SQUARE, thick, &logBrush, 0, NULL);
	       break;
	     } 
	 }  
     }
   else if (!(hPen = CreatePen (PS_SOLID, thick, ColorPixel (bg))))
     WinErrorBox (WIN_Main_Wd, TEXT("windowdisplay.c - DrawEllips (1)"));

   hOldPen = SelectObject (TtDisplay, hPen) ;
   result = SelectClipRgn (TtDisplay, clipRgn); 
   if (result == ERROR)
      ClipError (frame);
   if (!Ellipse (TtDisplay, x, y, x + width, y + height))
      WinErrorBox (FrRef  [frame], TEXT("windowdisplay.c - DrawEllips (2)"));
   SelectObject (TtDisplay, hOldPen);
   if (!DeleteObject (hPen))
      WinErrorBox (FrRef [frame], TEXT("windowdisplay.c - DrawEllips (3)"));
   hPen = (HPEN) 0;
   if (hBrush) {
      SelectObject (TtDisplay, hOldBrush);
      if (!DeleteObject (hBrush))
         WinErrorBox (WIN_Main_Wd, TEXT("windowdisplay.c - DrawEllips (4)"));
      hBrush = (HBRUSH)0;
   }
   WIN_ReleaseDeviceContext ();
   if (pat != (Pixmap)0)
      if (!DeleteObject ((HGDIOBJ) pat))
         WinErrorBox (NULL, TEXT("windowdisplay.c - DrawEllips (5)"));
   pat = (Pixmap) 0;
#endif /* _WIN_PRINT */
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
   int        Y;

   y += FrameTable[frame].FrTopMargin;
   if (thick <= 0)
     return;
   if (align == 1)
      Y = y + (h - thick) / 2;
   else if (align == 2)
      Y = y + h - thick / 2;
   else
      Y = y + thick / 2;
   x = x + thick / 2;
   l = l - thick;
#ifdef _WIN_PRINT 
   if (y < 0)
     return;
   if (TtPrinterDC)
      DoPrintOneLine (fg, x, Y, x + l, Y, thick, style);
#else /* _WIN_PRINT */
   InitDrawing (0, style, thick, RO, active, fg);
   DoDrawOneLine (frame, x, Y, x + l, Y);
#endif /* _WIN_PRINT */
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
   int        X;

   y += FrameTable[frame].FrTopMargin;
   if (thick <= 0)
      return;
   if (align == 1)
      X = x + (l - thick) / 2;
   else if (align == 2)
      X = x + l - thick / 2;
   else
      X = x + thick / 2;
   y = y + thick / 2;
   h = h - thick;
#ifdef _WIN_PRINT
   if (y < 0)
      return;
   if (TtPrinterDC)
     DoPrintOneLine (fg, X, y, X, y + h, thick, style);
#else  /* _WIN_PRINT */
   InitDrawing (0, style, thick, RO, active, fg);
   DoDrawOneLine (frame, X, y, X, y + h);
#endif /* _WIN_PRINT */
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

#ifdef _WIN_PRINT 
   y += FrameTable[frame].FrTopMargin;
   xf = x + l - 1 - thick;
   yf = y + h - 1 - thick;
   if (thick > 0) {
      if (direction == 0)
         DoPrintOneLine (fg, x, yf, xf, y, thick, style);
      else
           DoPrintOneLine (fg, x, y, xf, yf, thick, style);
   }
#else  /* _WIN_PRINT */
   y += FrameTable[frame].FrTopMargin;
   xf = x + l - 1 - thick;
   yf = y + h - 1 - thick;
   if (thick > 0)
     {
	InitDrawing (0, style, thick, RO, active, fg);
	if (direction == 0)
	   DoDrawOneLine (frame, x, yf, xf, y);
	else
	   DoDrawOneLine (frame, x, y, xf, yf);
     }
#endif /* _WIN_PRINT */
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
   HPEN                hPen;
   HPEN                hOldPen;
#  ifdef _WIN_PRINT
   LOGBRUSH            logBrush;
   int                 t;
#  endif /* _WIN_PRINT */

#ifdef _WIN_PRINT
   if (y < 0)
      return;
   y += FrameTable[frame].FrTopMargin;
   if (thick <= 0)
      return;

   if (TtPrinterDC) {
      xf = PixelToPoint (x + l);
      yf = PixelToPoint (y + h);
      x  = PixelToPoint (x);
      y  = PixelToPoint (y);

      switch (corner) {
             case 0: point[0].x = x;
                     point[0].y = y;
                     point[1].x = xf;
                     point[1].y = y;
                     point[2].x = xf;
                     point[2].y = yf;
                     break;

             case 1: point[0].x = xf;
                     point[0].y = y;
                     point[1].x = xf;
                     point[1].y = yf;
                     point[2].x = x;
                     point[2].y = yf;
                     break;

             case 2: point[0].x = xf;
                     point[0].y = yf;
                     point[1].x = x;
                     point[1].y = yf;
                     point[2].x = x;
                     point[2].y = y;
                     break;

             case 3: point[0].x = x;
                     point[0].y = yf;
                     point[1].x = x;
                     point[1].y = y;
                     point[2].x = xf;
                     point[2].y = y;
                    break;
	  }

      t = PixelToPoint (thick);

      if (t > 0)
	{
	  if (thick <= 1)
	    {
	      switch (style)
		{
		case 3:
		  hPen = CreatePen (PS_DOT, 1, RGB (RGB_colors[fg].red, RGB_colors[fg].green, RGB_colors[fg].blue));
		  break;
		case 4:
		  hPen = CreatePen (PS_DASH, 1, RGB (RGB_colors[fg].red, RGB_colors[fg].green, RGB_colors[fg].blue)); 
		  break;
		default:
		  hPen = CreatePen (PS_SOLID, 1, RGB (RGB_colors[fg].red, RGB_colors[fg].green, RGB_colors[fg].blue));   
		  break;
		} 
	    }
	  else
	    {
	      logBrush.lbStyle = BS_SOLID;
	      logBrush.lbColor = RGB (RGB_colors[fg].red, RGB_colors[fg].green, RGB_colors[fg].blue);
	      
	      switch (style)
		{
		case 3:
		  hPen = ExtCreatePen (PS_GEOMETRIC | PS_DOT | PS_ENDCAP_SQUARE, thick, &logBrush, 0, NULL);
		  break;
		case 4:
		  hPen = ExtCreatePen (PS_GEOMETRIC | PS_DASH | PS_ENDCAP_SQUARE, thick, &logBrush, 0, NULL); 
		  break;
		default:
		  hPen = ExtCreatePen (PS_GEOMETRIC | PS_SOLID | PS_ENDCAP_SQUARE, thick, &logBrush, 0, NULL);   
		  break;
		} 
	    }  
	}

      hOldPen = SelectObject (TtPrinterDC, hPen);
      Polyline (TtPrinterDC, point, 3);
      SelectObject (TtPrinterDC, hOldPen);
   }
#else  /* _WIN_PRINT */
   if (thick <= 0)
      return;

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
   WIN_GetDeviceContext (frame);
   WinLoadGC (TtDisplay, fg, RO);
   hPen = CreatePen (PS_SOLID, thick, RGB (RGB_colors[TtLineGC.foreground].red, RGB_colors[TtLineGC.foreground].green, RGB_colors[TtLineGC.foreground].blue));
   hOldPen = SelectObject (TtDisplay, hPen);
   Polyline (TtDisplay, point, 3);
   SelectObject (TtDisplay, hOldPen);
   WIN_ReleaseDeviceContext ();
   if (!DeleteObject (hPen))
      WinErrorBox (WIN_Main_Wd, TEXT("windowdisplay.c - DrawCorner (1)"));
   hPen = (HPEN) 0;
#endif /* _WIN_PRINT */
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
#  ifdef _WIN_PRINT
   LastPageNumber = pagenum;
   LastPageWidth = width;
   LastPageHeight = height;
#  endif /* WIN_PRINT */
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

#ifndef _WIN_PRINT
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
	if (!DeleteObject (hBrush))
       WinErrorBox (WIN_Main_Wd, TEXT("windowdisplay.c - Clear"));
    hBrush = (HBRUSH) 0;
     }
}

/*----------------------------------------------------------------------
  WChaine draw a string in frame, at location (x, y) and using font.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                WChaine (ThotWindow w, STRING string, int x, int y, ptrfont font, ThotGC GClocal)
#else  /* __STDC__ */
void                WChaine (w, string, x, y, font, GClocal)
ThotWindow          w;
STRING              string;
int                 x;
int                 y;
ptrfont             font;
ThotGC              GClocal;

#endif /* __STDC__ */
{
   HFONT hOldFont;
   int   result;

   WIN_GetDeviceContext (GetFrameNumber (w));
   SetMapperFlags (TtDisplay, 1);
   hOldFont = WinLoadFont(TtDisplay, font);
   result = SelectClipRgn (TtDisplay, clipRgn);  
   if (result == ERROR)
      ClipError (-1);
   /* if (!GetClipRgn(TtDisplay, clipRgn))
      WinErrorBox (NULL); */
   TextOut(TtDisplay, x, y, string, ustrlen(string));
   SelectObject (TtDisplay, hOldFont);
   DeleteObject (currentActiveFont);
   currentActiveFont = (HFONT)0;
   WIN_ReleaseDeviceContext();
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
  RECT cltRect;
  if (FrRef[frame] != None) {
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
   if (w != None) {
      if (TtDisplay)
         WIN_ReleaseDeviceContext ();
       
      WIN_GetDeviceContext (frame); 
      PatBlt (TtDisplay, x, y + FrameTable[frame].FrTopMargin, width, height, PATINVERT);
      WIN_ReleaseDeviceContext ();
   }
}

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
}
