/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2009
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 
/*
 * psdisplay.c : All the Postscript generation routines.
 *
 * Author: I. Vatton (INRIA)
 *         R. Guetari (W3C/INRIA) - Printing routines for Windows.
 *
 */
#ifndef _WIN_PRINT
#ifndef _GL

#include "ustring.h"
#include "math.h"
#include "thot_sys.h"
#include "message.h"
#include "constmedia.h"
#include "typemedia.h"
#include "frame.h"
#include "picture.h"

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "boxes_tv.h"
#include "font_tv.h"
#include "frame_tv.h"

#define VoidPixmap (Pixmap)(-1)
#define EmptyPixmap (Pixmap)(-2)
#define HL 4

static char*       Patterns_PS[] =
{
   "2222222222222222",		/*horiz1 */
   "6666666666666666",
   "7777777777777777",
   "0000ff000000ff00",		/*vert1 */
   "00ffff0000ffff00",
   "ffffff00ffffff00",
   "1122448811224488",		/*left */
   "993366cc993366cc",
   "ddbb77eeddbb77ee",
   "8844221188442211",		/*right1 */
   "cc663399cc663399",
   "ee77bbddee77bbdd",
   "11ff111111ff1111",		/*square1 */
   "33ffff3333ffff33",
   "77ffffff77ffffff",
   "11aa44aa11aa44aa",		/*diamond */
   "2222223e222222e3",		/*brick */
   "4244241c244442c1",		/*tile */
   "6688888884444222",		/*sea */
   "11b87c3a11a3c78b"		/*basket */
};


/* Handling of loaded fonts */
extern ThotFont     PostscriptFont;
extern int          ColorPs;
static char        *Scale = NULL;
int                 X, Y;
static int          SameBox = 0; /* 1 if the text is in the same box */
static int          NbWhiteSp;

#include "buildlines_f.h"
#include "font_f.h"
#include "initpses_f.h"
#include "memory_f.h"
#include "units_f.h"
#include "psdisplay_f.h"


/*----------------------------------------------------------------------
   CurrentColor compares the last RGB Postscript color loaded
   and the one asked, and load it if needed.
   num is an index in the Thot internal color table.
  ----------------------------------------------------------------------*/
static void CurrentColor (FILE *fout, int num)
{
  unsigned short      red;
  unsigned short      green;
  unsigned short      blue;
  float               fact;

  /* Compare the color asked with the current one */
  if (num != ColorPs)
    {
      /* Ask for the RedGreenBlue values */
      TtaGiveThotRGB (num, &red, &green, &blue);
      if (red == 0 && green == 0 && blue == 0)
	fprintf (fout, "0 setgray\n");
      else
	{
	  /* write the Postscript command */
	  fact = 255;
	  fprintf (fout, "%f %f %f setrgbcolor\n", ((float) red) / fact, ((float) green) / fact, ((float) blue) / fact);
	}
      ColorPs = num;
    }
}


/*----------------------------------------------------------------------
   CurrentFont compute the correct PostScript font needed,
   and emit the code to load it, if necessary.
   Returns 0 if it is a Latin font and 1 for a Greek one.
  ----------------------------------------------------------------------*/
static int CurrentFont (FILE *fout, ThotFont font)
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
  if (font != PostscriptFont)
    {
      PostscriptFont = font;
      if (TtPsFontName[i] == 'g')  /* Greek script */
	{
	  c0 = TtPsFontName[i];
	  c1 = TtPsFontName[i];
	  c2 = 'r';	     /* Symbol only has one style available */
	  result = 1;
	}
      else
	{
	  /* Latin Script */
	  c0 = 'l';
	  c1 = TtPsFontName[i + 1]; /* font Helvetica Times Courrier */
	  /* convert lowercase to uppercase */
	  c2 = TtPsFontName[i + 2]; /* Style normal bold italique */
#ifndef _GL
	  if (c1 == 'n')
	    {
	      c0 = c1 = 'g';
	      c2 = 'r';
	    }
#endif /* _GL */
	  result = 0;
	}
      
      /* update the scaling factor */
      Scale = &TtPsFontName[i + 3];
      fprintf (fout, "%c%c%c %s sf\n", c0, c1, c2, Scale);
      return result;
    }
  /* returns the indicator for the family of fonts */
  else if (TtPsFontName[i] == 'g')
    return (1);
  else
    return (0);
}

/*----------------------------------------------------------------------
  DrawChar draw a char at location (x, y) in frame and with font.
  The parameter fg indicates the drawing color
  ----------------------------------------------------------------------*/
void DrawChar (CHAR_T car, int frame, int x, int y, ThotFont font, int fg)
{
#ifndef _WX
   FILE               *fout;
   int                 w;

   fout = (FILE *) FrRef[frame];
   if (y < 0)
      return;
   y += FrameTable[frame].FrTopMargin;
   if (fg >= 0)
     {
       /* Do we need to change the current color ? */
       CurrentColor (fout, fg);
       w = CharacterWidth (car, font);
       /* Do we need to change the current font ? */
       CurrentFont (fout, font);
       fprintf (fout, "(%c) %d %d %d s\n", car, w, x, -y);
     }
#endif /* _WX */
}

/*----------------------------------------------------------------------
   Transcode emit the PostScript code for the given char.
  ----------------------------------------------------------------------*/
static void Transcode (FILE *fout, int encoding, unsigned char car)
{
  switch (car)
    {
    case '(':
      fprintf (fout, "\\(");
      break;
    case ')':
      fprintf (fout, "\\)");
      break;
    case '*':
      if (encoding == 0)
	fprintf (fout, "*");
      else
	fprintf (fout, "\\267");
      break;
    case '\\':
      fprintf (fout, "\\\\");
      break;
    case START_ENTITY:
      fprintf (fout, "&");
      break;
    default:
      if (car >= ' ' && car <= '~')
	fprintf (fout, "%c", car);
      else
	fprintf (fout, "\\%o", car);
    }
}


/*----------------------------------------------------------------------
   FillWithPattern fills in the current stroke with a black and white
   pattern, or the drawing color, or the background color,
   or keep it transparent, depending on pattern value.
  ----------------------------------------------------------------------*/
static void FillWithPattern (FILE *fout, int fg, int bg, int pattern)
{
  unsigned short      red;
  unsigned short      green;
  unsigned short      blue;
  float               fact;

  fact = 255;
  /* Do the current stroke need to be filled ? */
  if (pattern == 0)
    /* no filling */
    fprintf (fout, "0\n");
  else if (pattern == 1)
    {
      /* Ask for the RedGreenBlue values */
      TtaGiveThotRGB (fg, &red, &green, &blue);
      /* Emit the Postscript command */
      fprintf (fout, "%f %f %f -1\n", ((float) red) / fact,
               ((float) green) / fact, ((float) blue) / fact);
    }
  else if (pattern == 2)
    {
      /* Ask for the RedGreenBlue values */
      TtaGiveThotRGB (bg, &red, &green, &blue);
      /* Emit the Postscript command */
      fprintf (fout, "%f %f %f -1\n", ((float) red) / fact,
               ((float) green) / fact, ((float) blue) / fact);
    }
  else if (pattern >= 10)
    {
      /* Use of a fill pattern */
      /*fprintf(fout, "<d1e3c5885c3e1d88> 8 "); */
      fprintf (fout, "<%s> 8\n", Patterns_PS[pattern - 10]);
    }
  else
    /* Shade of grey */
    fprintf (fout, "%d\n", pattern - 2);
}

/*----------------------------------------------------------------------
   DrawString draw a char string of lg chars beginning in buff.
   Drawing starts at (x, y) in frame and using font.
   boxWidth gives the width of the final box or zero,
   this is used only by the thot formmating engine.
   bl indicate that there are one or more spaces before the string
   hyphen indicate whether an hyphen char has to be added.
   StartABlock is not null if the text is at a paragraph beginning
   (no justification of first spaces).
   parameter fg indicate the drawing color
   Returns the lenght of the string drawn.
  ----------------------------------------------------------------------*/
int DrawString (unsigned char *buff, int lg, int frame, int x, int y,
		ThotFont font, int boxWidth, int bl, int hyphen,
		int startABlock, int fg)
{
#ifndef _WX
  FILE               *fout;
  int                 j, i, encoding, width;
  int                 noJustifiedWhiteSp;

  fout = (FILE *) FrRef[frame];
  encoding = 0;
  if (y < 0)
    return 0;
  y += FrameTable[frame].FrTopMargin;

  /* Is this a new box ? */
  if (SameBox == 0)
    {
      /* store the start position for the justified box */
      SameBox = 1;
      X = x;
      Y = y;
      NbWhiteSp = 0;
      if (fg >= 0)
	{
	  /* Do we need to change the current color ? */
	  CurrentColor (fout, fg);
	  /* Do we need to change the current font ? */
	  encoding = CurrentFont (fout, font);
	  fprintf (fout, "(");
	}
    }
  
  buff[lg] = EOS;
  /* Add the justified white space */
  if (bl > 0)
    {
      NbWhiteSp += bl;
      if (fg >= 0)
	{
	  for (i = 1; i <= bl; i++)
	    fprintf (fout, "%c", ' ');
	  /* Transcode (fout, encoding, ' '); */
	}
    }

  width = 0;
  if (lg > 0)
    {
      /* noJustifiedWhiteSp is > 0 if writing a fixed lenght is needed */
      /* and equal to 0 if a justified space is to be printed */  
      noJustifiedWhiteSp = startABlock;
      /* Emit the chars */
      for (j = 0; j < lg; j++)
	{
	  /* compute the width of the string */
	  width += CharacterWidth (buff[j], font);
	  /* enumerate the white spaces */
	  if (buff[j] == ' ')
	    {
	      if (noJustifiedWhiteSp == 0)
		{
		  /* write a justified white space */
		  NbWhiteSp++;
		  if (fg >= 0)
		    fputs (" ", fout);
		}
	      else if (fg >= 0)
		/* write a fixed lenght white space */
		fputs ("\\240", fout);
	    }
	  else
	    {
	      noJustifiedWhiteSp = 0;
	      if (fg >= 0)
		Transcode (fout, encoding, buff[j]);
	    }
	}
    }
   
  /* Is an hyphen needed ? */
  if (hyphen && fg >= 0)
    fprintf (fout, "%c", '\255');
  /* is this the end of the box */
  if (boxWidth != 0 && SameBox == 1)
    {
      /* now let Postscript justify the text with the right width */
      if (boxWidth < 0)
	boxWidth = - boxWidth + width;
      else if (boxWidth < width)
	/* not enough space to display the last piece of text */
	boxWidth = width;
      /* Is justification needed ? */
      if (fg >= 0)
	{
	  if (NbWhiteSp == 0)
	    fprintf (fout, ") %d %d %d s\n", boxWidth, X, -Y);
	  else
	    fprintf (fout, ") %d %d %d %d j\n", NbWhiteSp, boxWidth, X, -Y);
	}
      SameBox = 0;
      NbWhiteSp = 0;
    }
  return (width);
#else /* _WX */
   /* TODO : a faire si on porte la version non _GL de wxWidgets */
  return 0;
#endif /* _WX */
}

/*----------------------------------------------------------------------
   WDrawString draw a char string of lg chars beginning in buff.
   Drawing starts at (x, y) in frame and using font.
   boxWidth gives the width of the final box or zero,
   this is used only by the thot formmating engine.
   bl indicate that there are one or more spaces before the string
   hyphen indicate whether an hyphen char has to be added.
   StartABlock is 1 if the text is at a paragraph beginning
   (no justification of first spaces).
   parameter fg indicate the drawing color
   Returns the lenght of the string drawn.
  ----------------------------------------------------------------------*/
int WDrawString (wchar_t *buff, int lg, int frame, int x, int y,
		ThotFont font, int boxWidth, int bl, int hyphen,
		int startABlock, int fg)
{
#ifndef _WX
  FILE               *fout;
  int                 j, i, encoding, width;
  int                 noJustifiedWhiteSp;

  fout = (FILE *) FrRef[frame];
  encoding = 0;
  if (y < 0)
    return 0;
  y += FrameTable[frame].FrTopMargin;

  /* Is this a new box ? */
  if (SameBox == 0)
    {
      /* store the start position for the justified box */
      SameBox = 1;
      X = x;
      Y = y;
      NbWhiteSp = 0;
      if (fg >= 0)
	{
	  /* Do we need to change the current color ? */
	  CurrentColor (fout, fg);
	  /* Do we need to change the current font ? */
	  encoding = CurrentFont (fout, font);
	  fprintf (fout, "(");
	}
    }
  
  buff[lg] = EOS;
  /* Add the justified white space */
  if (bl > 0)
    {
      NbWhiteSp += bl;
      if (fg >= 0)
	{
	  for (i = 1; i <= bl; i++)
	    fprintf (fout, "%c", ' ');
	}
    }

  width = 0;
  if (lg > 0)
    {
      /* noJustifiedWhiteSp is > 0 if writing a fixed lenght is needed */
      /* and equal to 0 if a justified space is to be printed */  
      noJustifiedWhiteSp = startABlock;
      /* Emit the chars */
      for (j = 0; j < lg; j++)
	{
	  /* compute the width of the string */
	  width += CharacterWidth (buff[j], font);
	  /* enumerate the white spaces */
	  if (buff[j] == ' ')
	    {
	      if (noJustifiedWhiteSp == 0)
		{
		  /* write a justified white space */
		  NbWhiteSp++;
		  if (fg >= 0)
		    fputs (" ", fout);
		}
	      else if (fg >= 0)
		/* write a fixed lenght white space */
		fputs ("\\240", fout);
	    }
	  else
	    {
	      noJustifiedWhiteSp = 0;
	      if (fg >= 0)
		fprintf (fout, "%c", (char)buff[j]);
	    }
	}
    }

  /* Is an hyphen needed ? */
  if (hyphen && fg >= 0)
    fprintf (fout, "%c", '\255');
  /* is this the end of the box */
  if (boxWidth != 0 && SameBox == 1)
    {
      /* now let Postscript justify the text with the right width */
      if (boxWidth < 0)
	boxWidth = - boxWidth + width;
      else if (boxWidth < width)
	/* not enough space to display the last piece of text */
	boxWidth = width;
      /* Is justification needed ? */
      if (fg >= 0)
	{
	  if (NbWhiteSp == 0)
	    fprintf (fout, ") %d %d %d s\n", boxWidth, X, -Y);
	  else
	    fprintf (fout, ") %d %d %d %d j\n", NbWhiteSp, boxWidth, X, -Y);
	}
      SameBox = 0;
      NbWhiteSp = 0;
    }
  return (width);
#else /* _WX */
   /* TODO : a faire si on porte la version non _GL de wxWidgets */
  return 0;
#endif /* _WX */
}


/*----------------------------------------------------------------------
   DisplayUnderline draw the underline, overline or cross line
  ----------------------------------------------------------------------*/
void DisplayUnderline (int frame, int x, int y, int h, int type,
		       int lg, int fg)
{
#ifndef _WX
  int                 bottom;	/* underline position    */
  int                 middle;	/* cross-over position   */
  int                 thickness;	/* thickness of drawing */
  int                 l_start;	/* start of the line     */
  int                 l_end;	/* end of the line       */
  FILE               *fout;

  if (y < 0 || fg < 0)
    return;
  y += FrameTable[frame].FrTopMargin;
  
  fout = (FILE *) FrRef[frame];
  /* The last box must be finished */
  if (SameBox == 0)
    {
      thickness = (h / 20) + 1;
      bottom = y + h - thickness;
      middle = y + h / 2;
      l_start = X;		/* get current X value (cf DrawString) */
      l_end = X + lg;	/* compute the end coordinate */
      
      /*
       * for an underline independant of the font add
       * the following lines here :
       *         thickness = 1;
       *         height = y + 2 * thickness;
       *         bottom = y + ascent + 3;
       */
      switch (type)
	{
	case 0:	/* nothing */
	  break;
	  
	case 1:	/* underlined */
	  fprintf (fout, "%d %d %d %d %d %d %d Seg\n",
		   l_end, -(bottom), l_start,
		   -(bottom), 5, thickness, 2);
	  break;
	  
	case 2:	/* overlined */
	  fprintf (fout, "%d %d %d %d %d %d %d Seg\n",
		   l_end, -(y), l_start, -(y),
		   5, thickness, 2);
	  break;
	  
	case 3:	/* cross-over */
	  fprintf (fout, "%d %d %d %d %d %d %d Seg\n",
		   l_end, -(middle), l_start,
		   -(middle), 5, thickness, 2);
	  break;
	}
    }
#else /* _WX */
   /* TODO : a faire si on porte la version non _GL de wxWidgets */
#endif /* _WX */
}


/*----------------------------------------------------------------------
   DrawRadical Draw a radical symbol.
  ----------------------------------------------------------------------*/
void DrawRadical (int frame, int thick, int x, int y, int l, int h,
		  ThotFont font, int fg)
{
#ifndef _WX
   int                 fh;
   int                 ex;
   FILE               *fout;

   if (y < 0)
      return;
   y += FrameTable[frame].FrTopMargin;
   if (thick < 0 || fg < 0)
      return;

   fout = (FILE *) FrRef[frame]; 
   CurrentColor (fout, fg);
   fh = FontHeight (font);
   ex = h / 3;
   if ((ex = h / 3) < 6)
     ex = 6;

   if (h <= (3 * fh))
     fprintf (fout, "%d %d %d %d %d %d %d %d r\n",
	      x + l, -(y), 
	      x + (fh / 2), -(y),
	      x + (fh / 4), -(y + h),
	      x, -(y + (2 * (h / 3))));
   else
     fprintf (fout, "%d %d %d %d %d %d %d %d r\n",
	      x + l, -(y),
	      x + (fh / 2), -(y),
	      x + (fh / 2), -(y + h),
	      x, -(y + (2 * (h / 3))));
#else /* _WX */
   /* TODO : a faire si on porte la version non _GL de wxWidgets */
#endif /* _WX */
}


/*----------------------------------------------------------------------
   DrawIntegral draw an integral. depending on type :
   - simple if type = 0
   - contour if type = 1
   - double if type = 2.
  ----------------------------------------------------------------------*/
void DrawIntegral (int frame, int thick, int x, int y, int l, int h,
		   int type, ThotFont font, int fg)
{
#ifndef _WX
   int                 yf;
   int                 ey, ym;
   FILE               *fout;

   if (y < 0)
     return;

   y += FrameTable[frame].FrTopMargin;
   fout = (FILE *) FrRef[frame];

   /* Do we need to change the current color ? */
   CurrentColor (fout, fg);
   /* Do we need to change the current font ? */
   CurrentFont (fout, font);

   l--;
   h--;
   ey = FontHeight (font);
   h -= ey;
   y += FontBase (font);
   x = x + (l / 2);
   yf = y + h;
   ym = y + (h / 2);
   y = y + 1;
   if (h < ey / 4)
     {
       /* Made of only one glyph */
       if (type == 2)
	 {
	   /* double integral */
	   fprintf (fout, "%d %d (\\362) c\n", -ym,
		    x - CharacterWidth (242, font) / 4);
	   fprintf (fout, "%d %d (\\362) c\n", -ym,
		    x + CharacterWidth (242, font) / 4);
	 }
       else
	 {
	   fprintf (fout, "%d %d (\\362) c\n", -ym, x);
	   if (type == 1)	/* contour integral */
	     fprintf (fout, "%d %d (o) c\n", -ym, x);
	 }
     }
   else
     {
       /* Drawn with more than one glyph */
       if (type == 2)
	 {
	   /* double integral */
	   fprintf (fout, "%d %d %d %s (\\363) (\\364) (\\365) s3\n",
		    x - CharacterWidth (244, font) / 4,
		    -yf, -y, Scale);
	   fprintf (fout, "%d %d %d %s (\\363) (\\364) (\\365) s3\n",
		    x + CharacterWidth (244, font) / 4,
		    -yf, -y, Scale);
	 }
       else
	 {
	   fprintf (fout, "%d %d %d %s (\\363) (\\364) (\\365) s3\n",
		    x, -yf, -y, Scale);
	   if (type == 1)	/* contour integral */
	     fprintf (fout, "%d %d (o) c\n", -ym, x);
	 }
     }
#else /* _WX */
   /* TODO : a faire si on porte la version non _GL de wxWidgets */
#endif /* _WX */
}


/*----------------------------------------------------------------------
   DrawSigma draw a Sigma symbol.
  ----------------------------------------------------------------------*/
void DrawSigma (int frame, int x, int y, int l, int h, ThotFont font, int fg)
{
#ifndef _WX
   FILE               *fout;

   if (y < 0)
      return;
   y += FrameTable[frame].FrTopMargin;

   fout = (FILE *) FrRef[frame];
   /* Do we need to change the current color ? */
   CurrentColor (fout, fg);

   /* Change the current font */
   PostscriptFont = NULL;
   fprintf (fout, "(Symbol) %.0f sf\n", FontHeight (font) * 0.9);
   x = x + (l / 2);
   y = y + h - FontHeight (font + FontBase (font));
   fprintf (fout, "%d %d (\\345) c\n", -y, x);
#else /* _WX */
   /* TODO : a faire si on porte la version non _GL de wxWidgets */
#endif /* _WX */
}


/*----------------------------------------------------------------------
   DrawPi draw a PI symbol.
  ----------------------------------------------------------------------*/
void DrawPi (int frame, int x, int y, int l, int h, ThotFont font, int fg)
{
#ifndef _WX
   FILE             *fout;

   if (y < 0)
      return;
   y += FrameTable[frame].FrTopMargin;
   fout = (FILE *) FrRef[frame];

   /* Do we need to change the current color ? */
   CurrentColor (fout, fg);

   /* Change the current font */
   PostscriptFont = NULL;
   fprintf (fout, "(Symbol) %.0f sf\n", FontHeight (font) * 0.9);
   x = x + (l / 2);
   y = y + h - FontHeight (font + FontBase (font));
   fprintf (fout, "%d %d (\\325) c\n", -y, x);
#else /* _WX */
   /* TODO : a faire si on porte la version non _GL de wxWidgets */
#endif /* _WX */
}


/*----------------------------------------------------------------------
   DrawUnion draw an Union symbol.
  ----------------------------------------------------------------------*/
void DrawUnion (int frame, int x, int y, int l, int h, ThotFont font, int fg)
{
#ifndef _WX
   FILE               *fout;

   if (y < 0)
      return;
   y += FrameTable[frame].FrTopMargin;
   fout = (FILE *) FrRef[frame];

   /* Do we need to change the current color ? */
   CurrentColor (fout, fg);

   /* Change the current font */
   PostscriptFont = NULL;
   fprintf (fout, "(Symbol) %.0f sf\n", FontHeight (font) * 0.9);
   x = x + (l / 2);
   y = y + h - FontHeight (font + FontBase (font));
   fprintf (fout, "%d %d (\\310) c\n", -y, x);
#else /* _WX */
   /* TODO : a faire si on porte la version non _GL de wxWidgets */
#endif /* _WX */
}


/*----------------------------------------------------------------------
   DrawIntersection draw an intersection symbol.
  ----------------------------------------------------------------------*/
void DrawIntersection (int frame, int x, int y, int l, int h, ThotFont font,
		       int fg)
{
#ifndef _WX
   FILE               *fout;

   if (y < 0)
      return;
   y += FrameTable[frame].FrTopMargin;
   fout = (FILE *) FrRef[frame];
    /* Do we need to change the current color ? */
   CurrentColor (fout, fg);

   /* Change the current font */
   PostscriptFont = NULL;
   fprintf (fout, "(Symbol) %.0f sf\n", FontHeight (font) * 0.9);
   x = x + (l / 2);
   y = y + h - FontHeight (font + FontBase (font));
   fprintf (fout, "%d %d (\\307) c\n", -y, x);
#else /* _WX */
   /* TODO : a faire si on porte la version non _GL de wxWidgets */
#endif /* _WX */
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

  parameter type is not taken into account here, always draw a arrow.
         
  fg : drawing color
  ----------------------------------------------------------------------*/
void DrawArrow (int frame, int thick, int style, int x, int y, int l,
		int h, int direction, int type, int fg)
{
#ifndef _WX
   int                 xm, ym, xf, yf, lg;
   FILE               *fout;

   if (l == 0 && h == 0)
      return;

   fout = (FILE *) FrRef[frame];
  if (y < 0)
    return;
   y += FrameTable[frame].FrTopMargin;

   if (thick <= 0 || fg < 0)
      return;

   /* Do we need to change the current color ? */
   CurrentColor (fout, fg);

   xm = x + l / 2;
   xf = x + l;
   ym = y + h / 2;
   yf = y + h;
   lg = HL + thick;		/* lenght of the arrow head */

   if (direction == 0)
     {
	/* draw a right arrow */
	fprintf (fout, "%d %d %d %d %d %d %d Seg\n",
		 x, -ym, xf, -ym, style, thick, 2);
	fprintf (fout, "%d %d %d %d %d %d %d %d arr\n",
		 style, x, -ym, xf, -ym, thick, lg, lg);
     }
   else if (direction == 45)
     {
	fprintf (fout, "%d %d %d %d %d %d %d Seg\n",
		 x, -yf, xf, -y, style, thick, 2);
	fprintf (fout, "%d %d %d %d %d %d %d %d arr\n",
		 style, x, -yf, xf, -y, thick, lg, lg);
     }
   else if (direction == 90)
     {
	/* draw a bottom-up arrow */
	fprintf (fout, "%d %d %d %d %d %d %d Seg\n",
		 xm, -yf, xm, -y, style, thick, 2);
	fprintf (fout, "%d %d %d %d %d %d %d %d arr\n",
		 style, xm, -yf, xm, -y, thick, lg, lg);
     }
   else if (direction == 135)
     {
	fprintf (fout, "%d %d %d %d %d %d %d Seg\n",
		 xf, -yf, x, -y, style, thick, 2);
	fprintf (fout, "%d %d %d %d %d %d %d %d arr\n",
		 style, xf, -yf, x, -y, thick, lg, lg);
     }
   else if (direction == 180)
     {
	/* draw a left arrow */
	fprintf (fout, "%d %d %d %d %d %d %d Seg\n",
		 xf, -ym, x, -ym, style, thick, 2);
	fprintf (fout, "%d %d %d %d %d %d %d %d arr\n",
		 style, xf, -ym, x, -ym, thick, lg, lg);
     }
   else if (direction == 225)
     {
	fprintf (fout, "%d %d %d %d %d %d %d Seg\n",
		 xf, -y, x, -yf, style, thick, 2);
	fprintf (fout, "%d %d %d %d %d %d %d %d arr\n",
		 style, xf, -y, x, -yf, thick, lg, lg);
     }
   else if (direction == 270)
     {
	/* draw a top-down arrow */
	fprintf (fout, "%d %d %d %d %d %d %d Seg\n",
		 xm, -y, xm, -yf, style, thick, 2);
	fprintf (fout, "%d %d %d %d %d %d %d %d arr\n",
		 style, xm, -y, xm, -yf, thick, lg, lg);
     }
   else if (direction == 315)
     {
	fprintf (fout, "%d %d %d %d %d %d %d Seg\n",
		 x, -y, xf, -yf, style, thick, 2);
	fprintf (fout, "%d %d %d %d %d %d %d %d arr\n",
		 style, x, -y, xf, -yf, thick, lg, lg);
     }
#else /* _WX */
   /* TODO : a faire si on porte la version non _GL de wxWidgets */
#endif /* _WX */
}

/*----------------------------------------------------------------------
   DrawBracket draw an opening or closing bracket (depending on direction)
  ----------------------------------------------------------------------*/
void DrawBracket (int frame, int thick, int x, int y, int l, int h,
		  int direction, ThotFont font, int fg, int baseline)
{
#ifndef _WX
   int                 ey, yf;
   FILE               *fout;

   if (y < 0)
     return;
   if (thick < 0 || fg < 0)
      return;
   fout = (FILE *) FrRef[frame];
   /* Do we need to change the current color ? */
   CurrentColor (fout, fg);
   /* Do we need to change the current font ? */
   CurrentFont (fout, font);

   y += FrameTable[frame].FrTopMargin;
   l--;
   h--;
   ey = FontHeight (font);
   h -= ey;
   y += FontBase (font);
   x = x + (l / 2);
   yf = y + h;
   y = y + 1;

   if (h < ey / 4)
     {
       yf = yf - (h / 2);
       /* Made of only one glyph */
       if (direction == 0)
	 fprintf (fout, "%d %d ([) c\n", -yf, x);
       else
	 fprintf (fout, "%d %d (])c\n", -yf, x);
     }
   else
     {
       /* Drawn with more than one glyph */
       if (direction == 0)	/* Trace un crochet ouvrant */
	 fprintf (fout, "%d %d %d %s (\\351) (\\352) (\\353) s3\n",
		  x + 1, -yf, -y, Scale);
       else
	 fprintf (fout, "%d %d %d %s (\\371) (\\372) (\\373) s3\n",
		  x, -yf, -y, Scale);
     }
#else /* _WX */
   /* TODO : a faire si on porte la version non _GL de wxWidgets */
#endif /* _WX */
}


/*----------------------------------------------------------------------
   DrawPointyBracket draw an opening or closing pointy bracket (depending
   on direction)
  ----------------------------------------------------------------------*/
void DrawPointyBracket (int frame, int thick, int x, int y, int l, int h,
			int direction, ThotFont font, int fg)
{
#ifndef _WX
   int                 ey, yf;
   FILE               *fout;

   if (y < 0)
     return;
   if (thick < 0 || fg < 0)
      return;
   fout = (FILE *) FrRef[frame];
   /* Do we need to change the current color ? */
   CurrentColor (fout, fg);
   /* Do we need to change the current font ? */
   CurrentFont (fout, font);

   y += FrameTable[frame].FrTopMargin;
   l--;
   h--;
   ey = FontHeight (font);

   if (h - ey < ey / 4)
     {
       /* Made of only one glyph */
       h -= ey;
       y += FontBase (font);
       yf = y + h;
       y = y + 1;
       x = x + (l / 2);
       yf = yf - (h / 2);
       if (direction == 0)
	 fprintf (fout, "%d %d (\341) c\n", -yf, x);
       else
	 fprintf (fout, "%d %d (\361)c\n", -yf, x);
     }
   else
     {
       /* Drawn with more than one glyph */
       if (direction == 0)
	 fprintf (fout, "%d %d %d %d %d %d %d %d %d Seg\n",
		  x + l, -(y),
		  x, -(y + (h / 2)),
		  x + l, -(y + h),
		  5, 1, 3);
       else
	 fprintf (fout, "%d %d %d %d %d %d %d %d %d Seg\n",
		  x, -(y),
		  x + l, -(y + (h / 2)),
		  x, -(y + h),
		  5, 1, 3);
     }
#else /* _WX */
   /* TODO : a faire si on porte la version non _GL de wxWidgets */
#endif /* _WX */
}


/*----------------------------------------------------------------------
   DrawParenthesis draw a closing or opening parenthesis (direction).
  ----------------------------------------------------------------------*/
void DrawParenthesis (int frame, int thick, int x, int y, int l, int h,
		      int direction, ThotFont font, int fg, int baseline)
{
#ifndef _WX
   int                 ey, yf;
   FILE               *fout;

   if (y < 0)
      return;
   if (thick < 0 || fg < 0)
      return;
   fout = (FILE *) FrRef[frame];
   /* Do we need to change the current color ? */
   CurrentColor (fout, fg);
   /* Do we need to change the current font ? */
   CurrentFont (fout, font);

   y += FrameTable[frame].FrTopMargin;
   l--;
   h--;
   ey = FontHeight (font);
   h -= ey;
   y += FontBase (font);
   yf = y + h;
   y = y + 1;
   x = x + (l / 2);

   if (h < ey / 3)
     {
       /* Made of only one glyph */
       yf = yf - (h / 2);
       if (direction == 0)
	 /* draw an opening parenthesis */
	 fprintf (fout, "%d %d (\\() c\n", -yf, x);
       else
	 fprintf (fout, "%d %d (\\)) c\n", -yf, x);
     }
   else
     {
       /* Drawn with more than one glyph */
       if (direction == 0)
	 fprintf (fout, "%d %d %d %s (\\346) (\\347) (\\350) s3\n",
		  x+1, -yf, -y, Scale);
       else
	 fprintf (fout, "%d %d %d %s (\\366) (\\367) (\\370) s3\n",
		  x, -yf, -y, Scale);
     }
#else /* _WX */
   /* TODO : a faire si on porte la version non _GL de wxWidgets */
#endif /* _WX */
}


/*----------------------------------------------------------------------
   DrawBrace draw an opening of closing brace (depending on direction).
  ----------------------------------------------------------------------*/
void DrawBrace (int frame, int thick, int x, int y, int l, int h,
		int direction, ThotFont font, int fg, int baseline)
{
#ifndef _WX
   int                 ey, yf;
   FILE               *fout;

   if (y < 0)
     return;
   if (thick < 0 || fg < 0)
      return;
   fout = (FILE *) FrRef[frame];
   /* Do we need to change the current color ? */
   CurrentColor (fout, fg);
   /* Do we need to change the current font ? */
   CurrentFont (fout, font);

   y += FrameTable[frame].FrTopMargin;
   l--;
   h--;
   ey = FontHeight (font);
   h -= ey;
   y += FontBase (font);
   yf = y + h;
   y = y + 1;
   x = x + (l / 2);

   if (h < ey - 1)
     {
       /* Made of only one glyph */
       yf = yf - (h / 2);
       if (direction == 0)
	 fprintf (fout, "%d %d ({) c\n", -yf, x);
       else
	 fprintf (fout, "%d %d (}) c\n", -yf, x);
     }
   else
     {
       /* Drawn with more than one glyph */
       if (direction == 0)
	 fprintf (fout, "%d %d %d %s (\\354) (\\355) (\\356) (\\357) s4\n",
		  x, -yf, -y, Scale);
       else
	 fprintf (fout, "%d %d %d %s (\\374) (\\375) (\\376) (\\357) s4\n",
		  x, -yf, -y, Scale);
     }
#else /* _WX */
   /* TODO : a faire si on porte la version non _GL de wxWidgets */
#endif /* _WX */
}


/*----------------------------------------------------------------------
   DrawRectangle draw a rectangle located at (x, y) in frame,
   of geometry width x height.
   thick indicate the thickness of the lines.
   Parameters fg, bg, and pattern are for drawing
   color, background color and fill pattern.
  ----------------------------------------------------------------------*/
void DrawRectangle (int frame, int thick, int style, int x, int y,
		    int width, int height, int fg, int bg, int pattern)
{
#ifndef _WX
   int                 xf, yf;
   FILE               *fout;

   if (y < 0)
      return;
   y += FrameTable[frame].FrTopMargin;
   fout = (FILE *) FrRef[frame];

   xf = x + width;
   yf = y + height;

   /* Do we need to change the current color ? */
   if (fg < 0)
     thick = 0;
   if (thick > 0)
      CurrentColor (fout, fg);

   FillWithPattern (fout, fg, bg, pattern);
   fprintf (fout, "%d %d %d %d %d %d  %d %d %d %d %d Poly\n",
	    x, -y, x, -yf, xf, -yf, xf, -y, style, thick, 4);
#else /* _WX */
   /* TODO : a faire si on porte la version non _GL de wxWidgets */
#endif /* _WX */
}

/*----------------------------------------------------------------------
   DrawSegments draw a set of segments.
   Parameter buffer is a pointer to the list of control points.
   nb indicate the number of points.
   The first point is a fake one containing the geometry.
   fg parameter gives the drawing color.
   arrow parameter indicate whether :
   - no arrow have to be drawn (0)
   - a forward arrow has to be drawn (1)
   - a backward arrow has to be drawn (2)
   - both backward and forward arrows have to be drawn (3)
  ----------------------------------------------------------------------*/
void  DrawSegments (int frame, int thick, int style, int x, int y,
		    PtrTextBuffer buffer, int nb, int fg, int arrow, int bg,
		    int pattern)
{
#ifndef _WX
  PtrTextBuffer       adbuff;
  FILE               *fout;
  float               xp, yp;
  int                 i, j;
  int                 prevx, prevy;
  int                 lg;

  if (y < 0)   
    return;
  fout = (FILE *) FrRef[frame];
  if (fg < 0)
    thick = 0;
  /* fill the included polygon */
  DrawPolygon (frame, 0, style, x, y, buffer, nb, fg, bg, pattern, 0);
  y += FrameTable[frame].FrTopMargin;
  xp = yp = 0;
  prevx = prevy = 0;
  lg = HL + thick;
  
  /* Do we need to change the current color ? */
  CurrentColor (fout, fg);
  adbuff = buffer;
  /* backward arrow  */
  if (arrow == 2 || arrow == 3)
    fprintf (fout, "%d %d %d %d %d %d %d %d arr\n",
	     style,
	     PixelValue (adbuff->BuPoints[2].XCoord, UnPixel, NULL, 0) + x,
	     -(PixelValue (adbuff->BuPoints[2].YCoord, UnPixel, NULL, 0) + y),
	     PixelValue (adbuff->BuPoints[1].XCoord, UnPixel, NULL, 0) + x,
	     -(PixelValue (adbuff->BuPoints[1].YCoord, UnPixel, NULL, 0) + y),
	     thick, lg, lg);
  
  j = 1;
  for (i = 1; i < nb; i++)
    {
      if (j >= adbuff->BuLength && adbuff->BuNext != NULL)
	{
	  /* Next buffer */
	  adbuff = adbuff->BuNext;
	  j = 0;
	}
      if (i == nb - 1)
	{
	  /* keep last coordinates for drawing the arrows */
	  prevx = FloatToInt (xp);
	  prevy = FloatToInt (yp);
	}
      /* Coordinate for next point */
      xp = (float) (PixelValue (adbuff->BuPoints[j].XCoord, UnPixel, NULL, 0) + x);
      yp = (float) (PixelValue (adbuff->BuPoints[j].YCoord, UnPixel, NULL, 0) + y);
      fprintf (fout, "%f %f\n", xp, -yp);
      j++;
    }
  /* Extra characteristics for drawing */
  fprintf (fout, " %d %d %d Seg\n", style, thick, nb - 1);
  
  /* forward arrow  */
  j--;
  if (arrow == 1 || arrow == 3)
    fprintf (fout, "%d %d %d %d %d %d %d %d arr\n",
	     style, prevx, -prevy,
	     FloatToInt (xp), -(FloatToInt (yp)), thick, lg, lg);
#else /* _WX */
   /* TODO : a faire si on porte la version non _GL de wxWidgets */
#endif /* _WX */
}


/*----------------------------------------------------------------------
   DrawPolygon draw a polygone.
   Parameter buffer is a pointer to the list of control points.
   nb indicate the number of points.
   The first point is a fake one containing the geometry.
   Parameters fg, bg, and pattern are for drawing
   color, background color and fill pattern.
  mode = 0 (GLU_TESS_WINDING_NONZERO), 1 (GLU_TESS_WINDING_ODD)
  ----------------------------------------------------------------------*/
void DrawPolygon (int frame, int thick, int style, int x, int y,
                  PtrTextBuffer buffer, int nb, int fg, int bg, int pattern, int mode)
{
#ifndef _WX
  float               xp, yp;
  PtrTextBuffer       adbuff;
  FILE               *fout;
  int                 i, j;

  fout = (FILE *) FrRef[frame];
  if (y < 0)
    return;
  y += FrameTable[frame].FrTopMargin;
  if (fg < 0)
    thick = 0;
  /* Do we need to change the current color ? */
  CurrentColor (fout, fg);
  FillWithPattern (fout, fg, bg, pattern);
  adbuff = buffer;
  j = 1;
  for (i = 1; i < nb; i++)
    {
      if (j >= adbuff->BuLength && adbuff->BuNext != NULL)
        {
          /* Next buffer */
          adbuff = adbuff->BuNext;
          j = 0;
        }
      /* Coordinate for next point */
      xp = (float) (PixelValue (adbuff->BuPoints[j].XCoord, UnPixel, NULL, 0) + x);
      yp = (float) (PixelValue (adbuff->BuPoints[j].YCoord, UnPixel, NULL, 0) + y);
      fprintf (fout, "%f %f\n", xp, -yp);
      j++;
    }
  /* Extra characteristics for drawing */
  fprintf (fout, "%d %d %d  Poly\n", style, thick, nb - 1);
#else /* _WX */
  /* TODO : a faire si on porte la version non _GL de wxWidgets */
#endif /* _WX */
}


/*----------------------------------------------------------------------
   DrawCurve draw an open curve.
   Parameter buffer is a pointer to the list of control points.
   nb indicate the number of points.
   The first point is a fake one containing the geometry.
   fg indicate the drawing color
   arrow parameter indicate whether :
   - no arrow have to be drawn (0)
   - a forward arrow has to be drawn (1)
   - a backward arrow has to be drawn (2)
   - both backward and forward arrows have to be drawn (3)
   Parameter control indicate the control points.
  ----------------------------------------------------------------------*/
void DrawCurve (int frame, int thick, int style, int x, int y,
		PtrTextBuffer buffer, int nb, int fg, int arrow,
		C_points *controls)
{
#ifndef _WX
   PtrTextBuffer       adbuff;
   int                 i, j;
   int                 lg;
   int                 lastx, lasty, newx, newy;
   float               x1, y1, x2, y2, x3, y3;
   FILE               *fout;

   fout = (FILE *) FrRef[frame];
  if (y < 0)
    return;
   y += FrameTable[frame].FrTopMargin;
   if (thick == 0 || fg < 0)
      return;

   /* Do we need to change the current color ? */
   CurrentColor (fout, fg);

   lg = HL + thick;
   j = 1;
   i = 2;
   adbuff = buffer;
   lastx = PixelValue (adbuff->BuPoints[j].XCoord, UnPixel, NULL, 0);
   lasty = PixelValue (adbuff->BuPoints[j].YCoord, UnPixel, NULL, 0);
   j++;
   newx = PixelValue (adbuff->BuPoints[j].XCoord, UnPixel, NULL, 0);
   newy = PixelValue (adbuff->BuPoints[j].YCoord, UnPixel, NULL, 0);
   /* control points for first arc */
   x1 = (float) (lastx + x);
   y1 = (float) (lasty + y);
   x2 = (float) ((int) controls[i].lx * 3 + lastx / 4 + x);
   y2 = (float) ((int) controls[i].ly * 3 + lasty / 4 + y);
   x3 = (float) ((int) controls[i].lx * 3 + newx / 4 + x);
   y3 = (float) ((int) controls[i].ly * 3 + newy / 4 + y);

   /* backward arrow  */
   if (arrow == 2 || arrow == 3)
      fprintf (fout, "%d %d %d %d %d %d %d %d arr\n",
	       style, FloatToInt (x2), -(FloatToInt (y2)), FloatToInt (x1),
	       -(FloatToInt (y1)), thick, lg, lg);

   for (i = 2; i < nb; i++)
     {
	/* 3 points needed to define the arc */
	fprintf (fout, "%f %f %f %f %f %f\n", x3, -y3, x2, -y2, x1, -y1);
	/* skip to next arc */
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
	lastx = newx;
	lasty = newy;
	newx = PixelValue (adbuff->BuPoints[j].XCoord, UnPixel, NULL, 0);
	newy = PixelValue (adbuff->BuPoints[j].YCoord, UnPixel, NULL, 0);
	x1 = (float) lastx + x;
	y1 = (float) lasty + y;
	if (i < nb - 2)
	  {
	     x2 = controls[i].rx + x;
	     y2 = controls[i].ry + y;
	     x3 = controls[i + 1].lx + x;
	     y3 = controls[i + 1].ly + y;
	  }
	else if (i == nb - 2)
	  {
	     x2 = (controls[i].rx * 3. + lastx) / (4. + x);
	     y2 = (controls[i].ry * 3. + lasty) / (4. + y);
	     x3 = (controls[i].rx * 3. + newx) / (4. + x);
	     y3 = (controls[i].ry * 3.+ newy) / (4. + y);
	  }
     }
   fprintf (fout, "%f %f %d %d %d Curv\n", x1, -y1, style, thick, nb - 1);

   /* forward arrow */
   if (arrow == 1 || arrow == 3)
      fprintf (fout, "%d %d %d %d %d %d %d %d arr\n",
	       style, FloatToInt (x3), -(FloatToInt (y3)), FloatToInt (x1),
	       -(FloatToInt (y1)), thick, lg, lg);
#else /* _WX */
   /* TODO : a faire si on porte la version non _GL de wxWidgets */
#endif /* _WX */
}


/*----------------------------------------------------------------------
   DrawSpline draw a closed curve.
   Parameter buffer is a pointer to the list of control points.
   nb indicate the number of points.
   The first point is a fake one containing the geometry.
   Parameters fg, bg, and pattern are for drawing
   color, background color and fill pattern.
   Parameter controls contains the list of control points.
  ----------------------------------------------------------------------*/
void DrawSpline (int frame, int thick, int style, int x, int y,
		 PtrTextBuffer buffer, int nb, int fg, int bg, int pattern,
		 C_points *controls)
{
#ifndef _WX
   PtrTextBuffer       adbuff;
   int                 i, j;
   float               x0, y0, x1, y1, x2, y2, x3, y3;
   FILE               *fout;

   fout = (FILE *) FrRef[frame];
   if (y < 0)
     return;
   y += FrameTable[frame].FrTopMargin;

   /* Do we need to change the current color ? */
   CurrentColor (fout, fg);
   FillWithPattern (fout, fg, bg, pattern);
   j = 1;
   i = 1;
   adbuff = buffer;
   x0 = x1 = (float) PixelValue (adbuff->BuPoints[j].XCoord + x, UnPixel, NULL, 0);
   y0 = y1 = (float) PixelValue (adbuff->BuPoints[j].YCoord + y, UnPixel, NULL, 0);
   x2 = controls[i].rx + x;
   y2 = controls[i].ry + y;

   for (i = 2; i < nb; i++)
     {
	x3 = controls[i].lx + x;
	y3 = controls[i].ly + y;
	fprintf (fout, "%f %f %f %f %f %f\n", x3, -y3, x2, -y2, x1, -y1);
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
	x1 = (float) PixelValue (adbuff->BuPoints[j].XCoord + x, UnPixel, NULL, 0);
	y1 = (float) PixelValue (adbuff->BuPoints[j].YCoord + y, UnPixel, NULL, 0);
	x2 = controls[i].rx + x;
	y2 = controls[i].ry + y;
     }

   /* Close the stroke */
   x3 = controls[1].lx + x;
   y3 = controls[1].ly + y;
   fprintf (fout, "%f %f %f %f %f %f\n", x3, -y3, x2, -y2, x1, -y1);
   fprintf (fout, "%f %f %d %d %d Splin\n", x0, -y0, style, thick, nb);
#else /* _WX */
   /* TODO : a faire si on porte la version non _GL de wxWidgets */
#endif /* _WX */
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
#ifndef _WX
  PtrPathSeg          pPa;
  float               x1, y1, cx1, cy1, x2, y2, cx2, cy2;
  FILE               *fout;

  if (thick > 0 || fg >= 0)
    {
      y += FrameTable[frame].FrTopMargin;
      fout = (FILE *) FrRef[frame];
      fprintf (fout, "newpath ");
      pPa = path;
      while (pPa)
	{
	  if (pPa->PaNewSubpath || !pPa->PaPrevious)
	    /* this path segment starts a new subpath */
	    {
	      /* generate a moveto */
	      x1 = (float) x + PixelValue (pPa->XStart, UnPixel, NULL, 0);
	      y1 = (float) y + PixelValue (pPa->YStart, UnPixel, NULL, 0);
	      fprintf (fout, "%f %f moveto ", x1, -y1);
	    }

	  switch (pPa->PaShape)
	    {
	    case PtLine:
	      x2 = (float) x + PixelValue (pPa->XEnd, UnPixel, NULL, 0);
	      y2 = (float) y + PixelValue (pPa->YEnd, UnPixel, NULL, 0);
	      fprintf (fout, "%f %f lineto ", x2, -y2);
	      break;

	    case PtCubicBezier:
	      cx1 = (float) x + PixelValue (pPa->XCtrlStart, UnPixel, NULL, 0);
	      cy1 = (float) y + PixelValue (pPa->YCtrlStart, UnPixel, NULL, 0);
	      cx2 = (float) x + PixelValue (pPa->XCtrlEnd, UnPixel, NULL, 0);
	      cy2 = (float) y + PixelValue (pPa->YCtrlEnd, UnPixel, NULL, 0);
	      x2 = (float) x + PixelValue (pPa->XEnd, UnPixel, NULL, 0);
	      y2 = (float) y + PixelValue (pPa->YEnd, UnPixel, NULL, 0);
	      fprintf (fout, "%f %f %f %f %f %f curveto ",
		       cx1, -cy1, cx2, -cy2, x2, -y2);
	      break;

	    case PtQuadraticBezier:
	      x1 = (float) x + PixelValue (pPa->XStart, UnPixel, NULL, 0);
	      y1 = (float) y + PixelValue (pPa->YStart, UnPixel, NULL, 0);
	      cx1 = (float) x + PixelValue (pPa->XCtrlStart, UnPixel, NULL, 0);
	      cy1 = (float) y + PixelValue (pPa->YCtrlStart, UnPixel, NULL, 0);
	      x2 = (float) x + PixelValue (pPa->XEnd, UnPixel, NULL, 0);
	      y2 = (float) y + PixelValue (pPa->YEnd, UnPixel, NULL, 0);
	      fprintf (fout, "%f %f %f %f %f %f curveto ",
		       x1+((2*(cx1-x1))/3), -y1-((2*(cy1-y1))/3),
                       x2+((2*(cx1-x2))/3), -y2-((2*(cy1-y2))/3), x2, -y2);
	      break;

	    case PtEllipticalArc:
	      /**** to do ****/
	      /* draws a Bezier if it's a half or quarter of a circle */
	      if (pPa->XRadius == pPa->YRadius)
		/* it's an arc of a circle */
		{
		  x1 = (float) (x + PixelValue (pPa->XStart, UnPixel, NULL, 0));
		  y1 = (float) (y + PixelValue (pPa->YStart, UnPixel, NULL, 0));
		  x2 = (float) (x + PixelValue (pPa->XEnd, UnPixel, NULL, 0));
		  y2 = (float) (y + PixelValue (pPa->YEnd, UnPixel, NULL, 0));
		  if (pPa->XStart == pPa->XEnd &&
		      abs (pPa->YEnd - pPa->YStart) == 2 * pPa->XRadius)
		    /* half circle (vertical) */
		    {
		    if ((pPa->Sweep  && pPa->YEnd > pPa->YStart) ||
			(!pPa->Sweep && pPa->YEnd < pPa->YStart))
		      cx1 = (float) (x + PixelValue ((int)(pPa->XStart + 1.36 * pPa->XRadius)
						     , UnPixel, NULL, 0));
		    else
		      cx1 = (float) (x + PixelValue ((int)(pPa->XStart - 1.36 * pPa->XRadius),
						     UnPixel, NULL, 0));
		    cy1 = y1;
		    cx2 = cx1;
		    cy2 = y2;
		    fprintf (fout, "%f %f %f %f %f %f curveto ",
		       cx1, -cy1, cx2, -cy2, x2, -y2);
		    }
		  else if (pPa->YStart == pPa->YEnd &&
		      abs (pPa->XEnd - pPa->XStart) == 2 * pPa->XRadius)
		    /* half circle (horizontal) */
		    {
		    if ((pPa->Sweep  && pPa->XEnd < pPa->XStart) ||
			(!pPa->Sweep && pPa->XEnd > pPa->XStart))
		      cy1 = (float) (y + PixelValue ((int)(pPa->YStart + 1.36 * pPa->YRadius)
						     , UnPixel, NULL, 0));
		    else
		      cy1 = (float) (y + PixelValue ((int)(pPa->YStart - 1.36 * pPa->YRadius)
						     , UnPixel, NULL, 0));
		    cy2 = cy1;
		    cx1 = x1;
		    cx2 = x2;
		    fprintf (fout, "%f %f %f %f %f %f curveto ",
		       cx1, -cy1, cx2, -cy2, x2, -y2);
		    }
		  else if (abs (pPa->YEnd - pPa->YStart) == pPa->YRadius &&
			   abs (pPa->XEnd - pPa->XStart) == pPa->XRadius)
		    /* a quarter or 3/4 of a circle */
		    {
		      if (!pPa->LargeArc)
			/* a quarter of a circle */
			{
			  if (pPa->XStart < pPa->XEnd)
			    if (pPa->YStart < pPa->YEnd)
			      if (pPa->Sweep)
			        {
			          cx1 = x2;
			          cy1 = y1;
			        }
			      else
			        {
			          cx1 = x1;
			          cy1 = y2;
			        }
			    else
			      if (pPa->Sweep)
			        {
			          cx1 = x1;
			          cy1 = y2;
			        }
			      else
			        {
			          cx1 = x2;
			          cy1 = y1;
			        }
			  else
			    if (pPa->YStart < pPa->YEnd)
			      if (pPa->Sweep)
			        {
			          cx1 = x1;
			          cy1 = y2;
			        }
			      else
			        {
			          cx1 = x2;
			          cy1 = y1;
			        }
			    else
			      if (pPa->Sweep)
			        {
			          cx1 = x2;
			          cy1 = y1;
			        }
			      else
			        {
			          cx1 = x1;
			          cy1 = y2;
			        }
			  fprintf (fout, "%f %f %f %f %f %f curveto ",
				   x1+((2*(cx1-x1))/3), -y1-((2*(cy1-y1))/3),
				   x2+((2*(cx1-x2))/3), -y2-((2*(cy1-y2))/3), x2, -y2);
			}
		    }
		}
	      break;
	    }
	  pPa = pPa->PaNext;
	}
      if (bg >= 0 && pattern > 0)
	{
	  if (fg >= 0 && thick > 0)
	     fprintf (fout, "gsave ");
	  /* set background color */
	  CurrentColor (fout, bg);
	  fprintf (fout, "fill ");
	  if (fg >= 0 && thick > 0)
	     fprintf (fout, "grestore ");
	  else
	     fprintf (fout, "\n");
	}
      if (fg >= 0 && thick > 0)
	{
	  /* set stroke color */
	  CurrentColor (fout, fg);
          fprintf (fout, "%d setlinewidth stroke\n", thick);
	}
    }
  return;
#else /* _WX */
   /* TODO : a faire si on porte la version non _GL de wxWidgets */
#endif /* _WX */
}

/*----------------------------------------------------------------------
   DrawDiamond draw a diamond.
   Parameters fg, bg, and pattern are for drawing
   color, background color and fill pattern.
  ----------------------------------------------------------------------*/
void DrawDiamond (int frame, int thick, int style, int x, int y,
		  int width, int height, int fg, int bg, int pattern)
{
#ifndef _WX
   int                 xm, xf, ym, yf;
   FILE               *fout;

   fout = (FILE *) FrRef[frame];
  if (y < 0)
    return;
   y += FrameTable[frame].FrTopMargin;

   /* Do we need to change the current color ? */
   CurrentColor (fout, fg);
   xm = x + width / 2;
   ym = y + height / 2;
   xf = x + width;
   yf = y + height;
   x = x;
   y = y;

   FillWithPattern (fout, fg, bg, pattern);
   fprintf (fout, "%d %d %d %d %d %d %d %d %d %d %d Poly\n",
	    xm, -y, x, -ym, xm, -yf, xf, -ym, style, thick, 4);
#else /* _WX */
   /* TODO : a faire si on porte la version non _GL de wxWidgets */
#endif /* _WX */
}


/*----------------------------------------------------------------------
   DrawOval draw a rectangle with smoothed corners.
   Parameters fg, bg, and pattern are for drawing
   color, background color and fill pattern.
  ----------------------------------------------------------------------*/
void DrawOval (int frame, int thick, int style, int x, int y, int width,
	       int height, int rx, int ry, int fg, int bg, int pattern)
{
#ifndef _WX
  int                 arc, xf, yf;
  FILE               *fout;

  fout = (FILE *) FrRef[frame];
  if (y < 0)
    return;

   y += FrameTable[frame].FrTopMargin;
   /* Do we need to change the current color ? */
   if (fg < 0)
     thick = 0;
   if (thick > 0)
     CurrentColor (fout, fg);

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
 
   xf = x + width - 1;
   yf = y + height - 1;
   x = x;
   y = y;
   FillWithPattern (fout, fg, bg, pattern);
   fprintf (fout, "%d %d %d %d %d %d %d %d oval\n", style, thick,
	    x, -y, xf, -yf, rx, ry);
#else /* _WX */
   /* TODO : a faire si on porte la version non _GL de wxWidgets */
#endif /* _WX */
}


/*----------------------------------------------------------------------
   DrawEllips draw an ellips (or a circle).
   Parameters fg, bg, and pattern are for drawing
   color, background color and fill pattern.
  ----------------------------------------------------------------------*/
void DrawEllips (int frame, int thick, int style, int x, int y, int width,
		 int height, int fg, int bg, int pattern)
{
#ifndef _WX
   int                 xm, ym;
   FILE               *fout;

   if (y < 0)   
      return;
   y += FrameTable[frame].FrTopMargin;
   fout = (FILE *) FrRef[frame];
   if (y < 0)
      return;
   /* Do we need to change the current color ? */
   if (fg < 0)
     thick = 0;
   if (thick > 0)
      CurrentColor (fout, fg);
   width = width / 2;
   height = height / 2;
   xm = x + width;
   ym = y + height;
   width = width;
   height = height;

   FillWithPattern (fout, fg, bg, pattern);
   if (width == height)
     {
       /* Draw a circle */
       fprintf (fout, "%d %d %d %d %d cer\n", style, thick, xm, -ym, width);
     }
   else if (height != 0)
     {
       /* Draw an ellips */
       fprintf (fout, "%d %d %d %d %d %d ellipse\n", style, thick,
		xm, -ym, width, height);
     }
#else /* _WX */
   /* TODO : a faire si on porte la version non _GL de wxWidgets */
#endif /* _WX */
}

/*----------------------------------------------------------------------
   DrawCorner draw two contiguous edges of a rectangle.
  ----------------------------------------------------------------------*/
void DrawCorner (int frame, int thick, int style, int x, int y, int l,
		 int h, int corner, int fg)
{
#ifndef _WX
   int                 xf, yf;
   FILE               *fout;

   if (y < 0)
      return;
   y += FrameTable[frame].FrTopMargin;
   if (thick <= 0 || fg < 0)
      return;
   fout = (FILE *) FrRef[frame];

   /* Do we need to change the current color ? */
   CurrentColor (fout, fg);
   xf = x + l;
   yf = y + h;
   x = x;
   y = y;

   switch (corner)
	 {
	    case 0:		/* Top Right */
	       fprintf (fout, "%d %d %d %d %d %d %d %d %d Seg\n",
			x, -y, xf, -y, xf, -yf, style, thick, 3);
	       break;
	    case 1:		/* Right Bottom */
	       fprintf (fout, "%d %d %d %d %d %d %d %d %d Seg\n",
			xf, -y, xf, -yf, x, -yf, style, thick, 3);
	       break;
	    case 2:		/* Bottom Left */
	       fprintf (fout, "%d %d %d %d %d %d %d %d %d Seg\n",
			xf, -yf, x, -yf, x, -y, style, thick, 3);
	       break;
	    case 3:		/* Left Top */
	       fprintf (fout, "%d %d %d %d %d %d %d %d %d Seg\n",
			x, -yf, x, -y, xf, -y, style, thick, 3);
	       break;
	 }
#else /* _WX */
   /* TODO : a faire si on porte la version non _GL de wxWidgets */
#endif /* _WX */
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
#ifndef _WX
   int                 arc, xf, yf;
   FILE               *fout;

   fout = (FILE *) FrRef[frame];
  if (y < 0)
    return;
   y += FrameTable[frame].FrTopMargin;
   /* Do we need to change the current color ? */
   if (fg < 0)
     thick = 0;
   if (thick > 0)
      CurrentColor (fout, fg);

   arc = (int)(3 * 72 / 25.4);
   xf = x + width - 1;
   yf = y + height - 1;
   x = x;
   y = y;

   FillWithPattern (fout, fg, bg, pattern);
   fprintf (fout, "%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d ov\n",
	    style, thick, x, -y, x, -yf, xf, -yf, xf, -y, x, -y, x,
	    -(yf - arc), arc);

   y += 2 * arc;
   fprintf (fout, "%d %d %d %d %d %d %d Seg\n",
	    x, -y, xf, -y, style, thick, 2);
#else /* _WX */
   /* TODO : a faire si on porte la version non _GL de wxWidgets */
#endif /* _WX */
}


/*----------------------------------------------------------------------
   DrawEllips draw an ellips (or a circle).
   Parameters fg, bg, and pattern are for drawing
   color, background color and fill pattern.
  ----------------------------------------------------------------------*/
void DrawEllipsFrame (int frame, int thick, int style, int x, int y,
		      int width, int height, int fg, int bg, int pattern)
{
#ifndef _WX
   int                 px7mm, shiftX;
   int                 xm, ym;
   FILE               *fout;
   double              A;

   fout = (FILE *) FrRef[frame];
  if (y < 0)
    return;
   y += FrameTable[frame].FrTopMargin;
   /* Do we need to change the current color ? */
   if (fg < 0)
     thick = 0;
   if (thick > 0)
      CurrentColor (fout, fg);

   width = width / 2;
   height = height / 2;
   xm = x + width;
   ym = y + height;
   width = width;
   height = height;

   FillWithPattern (fout, fg, bg, pattern);
   if (width == height)
     {
	/* draw a circle */
	fprintf (fout, "%d %d %d %d %d cer\n", style, thick, xm, -ym, width);
     }
   else
     {
	/* draw an ellipse */
	fprintf (fout, "%d %d %d %d %d %d ellipse\n",
		 style, thick, xm, -ym, width, height);
     }
   px7mm = (int)(7 * 72 / 25.4 + 0.5);
   if (height > px7mm)
     {
	y = (ym - height + px7mm);
	A = ((double) height - px7mm) / height;
	shiftX = (int)(width * sqrt (1 - A * A) + 0.5);
	fprintf (fout, "%d %d  %d %d %d %d %d Seg\n",
		 xm - shiftX, -y, xm + shiftX, -y, style, thick, 2);
     }
#else /* _WX */
   /* TODO : a faire si on porte la version non _GL de wxWidgets */
#endif /* _WX */
}


/*----------------------------------------------------------------------
  DrawHorizontalLine draw a horizontal line aligned top center or bottom
  depending on align value.
  leftslice and rightslice say if the left and right borders are sliced.
  ----------------------------------------------------------------------*/
void DrawHorizontalLine (int frame, int thick, int style, int x, int y,
                         int l, int h, int align, int fg, PtrBox box,
                         int leftslice, int rightslice)
{
#ifndef _WX
  ThotPoint           point[4];
  FILE               *fout;
  int                 xf, Y, left, right;
  int                 light = fg, dark = fg;
  unsigned short      red, green, blue, sl = 50, sd = 100;

  if (y < 0)
    return;
  y += FrameTable[frame].FrTopMargin;
  if (thick <= 0 || fg < 0)
    return;

  if (style > 6 && align != 1)
    {
      /*  */
      TtaGiveThotRGB (fg, &red, &green, &blue);
      if (red < sd) sd = red;
      if (green < sd) sd = green;
      if (blue < sd) sd = blue;
      dark = TtaGetThotColor (red - sd, green - sd, blue - sd);
      if (red + sl > 254) red = 255 - sl;
      if (green + sl > 254) green = 255 - sl;
      if (blue + sl > 254) blue = 255 - sl;
      light = TtaGetThotColor (red + sl, green + sl, blue + sl);
    }

  fout = (FILE *) FrRef[frame];
  /* Do we need to change the current color ? */
  CurrentColor (fout, fg);
  if (style < 5 || thick < 2)
    {
      if (align == 1)
        Y = y + (h - thick) / 2;
      else if (align == 2)
        Y = y + h - thick / 2;
      else
        Y = y + thick / 2;
      xf = x + l;
      fprintf (fout, "%d %d %d %d %d %d %d Seg\n",
               xf, -Y, x, -Y, style, thick, 2);
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
          fprintf (fout, "%d %d %d %d %d %d %d Seg\n",
                   point[0].x, -point[0].y, point[1].x, -point[1].y, style, thick, 2);
          fprintf (fout, "%d %d %d %d %d %d %d Seg\n",
                   point[3].x, -point[3].y, point[2].x, -point[2].y, style, thick, 2);
        }
      else
        {
          FillWithPattern (fout, fg, fg, 2);
          // draw the polygon
          fprintf (fout, "%f %f\n", (float)point[0].x, (float)-point[0].y);
          fprintf (fout, "%f %f\n", (float)point[1].x, (float)-point[1].y);
          fprintf (fout, "%f %f\n", (float)point[2].x, (float)-point[2].y);
          fprintf (fout, "%f %f\n", (float)point[3].x, (float)-point[3].y);
          fprintf (fout, "%d %d %d  Poly\n", style, thick, 4);
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
                  point[2].x = point[1].x + left;
                  point[2].y = point[1].y - thick;
                  point[3].x = point[0].x - right;
                  point[3].y = point[0].y - thick;
                }
              FillWithPattern (fout, fg, fg, 2);
              // draw the polygon
              fprintf (fout, "%f %f\n", (float)point[0].x, (float)-point[0].y);
              fprintf (fout, "%f %f\n", (float)point[1].x, (float)-point[1].y);
              fprintf (fout, "%f %f\n", (float)point[2].x, (float)-point[2].y);
              fprintf (fout, "%f %f\n", (float)point[3].x, (float)-point[3].y);
              fprintf (fout, "%d %d %d  Poly\n", style, thick, 4);
            }
        }
    }
#endif /* _WX */
}

/*----------------------------------------------------------------------
  DrawVerticalLine draw a vertical line aligned left center or right
  depending on align value.
  topslice and bottomslice say if the top and bottom borders are sliced.
  ----------------------------------------------------------------------*/
void DrawVerticalLine (int frame, int thick, int style, int x, int y,
                       int l, int h, int align, int fg, PtrBox box,
                       int topslice, int bottomslice)
{
#ifndef _WX
  ThotPoint           point[4];
  FILE               *fout;
  int                 X, yf, top, bottom;
  int                 light = fg, dark = fg;
  unsigned short      red, green, blue, sl = 50, sd = 100;

  if (y < 0)
    return;
  y += FrameTable[frame].FrTopMargin;
  if (thick <= 0 || fg < 0)
    return;

  if (style > 6 && align != 1)
    {
      /*  */
      TtaGiveThotRGB (fg, &red, &green, &blue);
      if (red < sd) sd = red;
      if (green < sd) sd = green;
      if (blue < sd) sd = blue;
      dark = TtaGetThotColor (red - sd, green - sd, blue - sd);
      if (red + sl > 254) red = 255 - sl;
      if (green + sl > 254) green = 255 - sl;
      if (blue + sl > 254) blue = 255 - sl;
      light = TtaGetThotColor (red + sl, green + sl, blue + sl);
    }

  fout = (FILE *) FrRef[frame];
  /* Do we need to change the current color ? */
  CurrentColor (fout, fg);
  if (style < 5 || thick < 2)
    {
      if (align == 1)
        X = x + (l - thick) / 2;
      else if (align == 2)
        X = x + l - thick / 2;
      else
        X = x + thick / 2;
      yf = y + h;
      fprintf (fout, "%d %d %d %d %d %d %d Seg\n",
               X, -yf, X, -y, style, thick, 2);
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
          fprintf (fout, "%d %d %d %d %d %d %d Seg\n",
                   point[0].x, -point[0].y, point[1].x, -point[1].y, style, thick, 2);
          fprintf (fout, "%d %d %d %d %d %d %d Seg\n",
                   point[3].x, -point[3].y, point[2].x, -point[2].y, style, thick, 2);
        }
      else
        {
          FillWithPattern (fout, fg, fg, 2);
          // draw the polygon
          fprintf (fout, "%f %f\n", (float)point[0].x, (float)-point[0].y);
          fprintf (fout, "%f %f\n", (float)point[1].x, (float)-point[1].y);
          fprintf (fout, "%f %f\n", (float)point[2].x, (float)-point[2].y);
          fprintf (fout, "%f %f\n", (float)point[3].x, (float)-point[3].y);
          fprintf (fout, "%d %d %d  Poly\n", style, thick, 4);
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
              FillWithPattern (fout, fg, fg, 2);
              // draw the polygon
              fprintf (fout, "%f %f\n", (float)point[0].x, (float)-point[0].y);
              fprintf (fout, "%f %f\n", (float)point[1].x, (float)-point[1].y);
              fprintf (fout, "%f %f\n", (float)point[2].x, (float)-point[2].y);
              fprintf (fout, "%f %f\n", (float)point[3].x, (float)-point[3].y);
              fprintf (fout, "%d %d %d  Poly\n", style, thick, 4);
            }
        }
    }
#endif /* _WX */
}

/*----------------------------------------------------------------------
  DrawHat draw a hat aligned top
  The parameter fg indicates the drawing color.
  ----------------------------------------------------------------------*/
void DrawHat (int frame, int thick, int style, int x, int y, int l, int h,
              int fg, int direction)
{
  int                 xf, xm, Y;
  FILE               *fout;

  if (thick > 0 && fg >= 0)
    {
      y += FrameTable[frame].FrTopMargin;
      y += (h - thick) / 2;
      Y = y + (h - thick) / 2;
      xf = x + l;
      xm = x + (l / 2);
      fout = (FILE *) FrRef[frame];
      /* Do we need to change the current color ? */
      CurrentColor (fout, fg);
      fprintf (fout, "%d %d %d %d %d %d %d Seg\n",
	       xm, -(y), xm, -Y, style, thick, 2);
    }
}

/*----------------------------------------------------------------------
  DrawTilde draw a hat aligned top
  The parameter fg indicates the drawing color.
  ----------------------------------------------------------------------*/
void DrawTilde (int frame, int thick, int style, int x, int y, int l, int h, int fg)
{
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
  ----------------------------------------------------------------------*/
void DrawHorizontalBrace (int frame, int thick, int style, int x, int y,
			  int l, int h, int align, int fg)
{
#ifndef _WX
  int                 xf, xm, Y;
  FILE               *fout;

  if (y < 0)
    return;
  y += FrameTable[frame].FrTopMargin;
  if (thick <= 0 || fg < 0)
    return;

  fout = (FILE *) FrRef[frame];
  /* Do we need to change the current color ? */
  CurrentColor (fout, fg);
  Y = y + (h - thick / 2);
  xf = x + l;
  xm = x + (l / 2);
  if (align == 0)
    /* Over brace */
    {
      fprintf (fout, "%d %d %d %d %d %d %d %d %d %d %d Seg\n",
	       x, -(y + h),
	       x, -Y,
	       xf, -Y,
	       xf, -(y + h), style, thick, 4);
      fprintf (fout, "%d %d %d %d %d %d %d Seg\n",
	       xm, -(y), xm, -Y, style, thick, 2);
    }
  else
    /* Underbrace */
    {
      fprintf (fout, "%d %d %d %d %d %d %d %d %d %d %d Seg\n",
	       x, -(y),
	       x, -Y,
	       xf, -Y,
	       xf, -(y), style, thick, 4);
      fprintf (fout, "%d %d %d %d %d %d %d Seg\n",
	       xm, -(y + h), xm, -Y, style, thick, 2);
    }
#else /* _WX */
   /* TODO : a faire si on porte la version non _GL de wxWidgets */
#endif /* _WX */
}

/*----------------------------------------------------------------------
  DrawHorizontalBracket draw a horizontal brace aligned top or bottom
  depending on align value.
  The parameter fg indicates the drawing color.
  ----------------------------------------------------------------------*/
void DrawHorizontalBracket (int frame, int thick, int style, int x, int y,
                          int l, int h, int align, int fg)
{
}


/*----------------------------------------------------------------------
   DrawPoints draw a line of dot.
  ----------------------------------------------------------------------*/
void DrawPoints (int frame, int x, int y, int boxWidth, int fg)
{
#ifndef _WX
  int                 xcour, ycour;
  FILE               *fout = NULL;

  if (y < 0)
    return;
  y += FrameTable[frame].FrTopMargin;
  /* Do we need to change the current color ? */
  CurrentColor (fout, fg);

  if (boxWidth > 0)
    {
      fout = (FILE *) FrRef[frame];
      xcour = x;
      ycour = y;
      fprintf (fout, "%d %d %d Pes\n", xcour, -ycour, boxWidth);
    }
#else /* _WX */
   /* TODO : a faire si on porte la version non _GL de wxWidgets */
#endif /* _WX */
}


/*----------------------------------------------------------------------
   DrawSlash draw a slash or backslash depending on direction.
  ----------------------------------------------------------------------*/
void DrawSlash (int frame, int thick, int style, int x, int y, int l,
		int h, int direction, int fg)
{
#ifndef _WX
   int                 xf, yf;
   FILE               *fout;

   fout = (FILE *) FrRef[frame];
  if (y < 0)
    return;
   y += FrameTable[frame].FrTopMargin;
   if (fg < 0)
     thick = 0;
   if (thick <= 0)
      return;

   /* Do we need to change the current color ? */
   CurrentColor (fout, fg);

   xf = x + l;
   yf = y + h;
   x = x;
   y = y;

   if (direction == 0)
      fprintf (fout, "%d %d %d %d %d %d %d Seg\n",
	       x, -yf, xf, -y, style, thick, 2);
   else
      fprintf (fout, "%d %d %d %d %d %d %d Seg\n",
	       x, -y, xf, -yf, style, thick, 2);
#else /* _WX */
   /* TODO : a faire si on porte la version non _GL de wxWidgets */
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
#ifndef _WX
   int                 xf, yf;
   FILE               *fout;

   fout = (FILE *) FrRef[frame];
  if (y < 0)
    return;
   y += FrameTable[frame].FrTopMargin;
   /* Do we need to change the current color ? */
   CurrentColor (fout, fg);

   if (pattern >= 0)
     {
	xf = x + width - 1;
	yf = y + height - 1;
	x = x;
	y = y;
	fprintf (fout, "%d %d %d %d %d %d %d %d %d trm\n",
		 pattern, x, -yf, xf, -yf, xf, -y, x, -y);
     }
#else /* _WX */
   /* TODO : a faire si on porte la version non _GL de wxWidgets */
#endif /* _WX */
}
#endif /* _WIN_PRINT */
#endif /* _GL */
