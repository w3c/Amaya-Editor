/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2009
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 
/*
 * inites.c : module handling colors and patterns in the context of
 *            drawing on a computer screen (initpses is for Postcript).
 *
 * Authors: I. Vatton (INRIA)
 *          R. Guetari (W3C/INRIA) - Unicode and Windows version
 */

#ifdef _WX
  #include "wx/wx.h"
#endif /* _WX */

#include "thot_gui.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "constmenu.h"
#include "typemedia.h"
#include "frame.h"
#include "message.h"
#include "pattern.h"

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "frame_tv.h"
#include "thotcolor_tv.h"

#include "context_f.h"
#include "memory_f.h"
#include "inites_f.h"
#include "registry_f.h"

#ifdef _WINGUI
  #include "wininclude.h"
#endif /* _WINGUI*/

#ifdef _WX
  #include "wx/colour.h"
  #include "wx/memory.h"
#endif /* _WX */

#ifndef _WX
static ThotColorStruct def_colrs[256];
static int             allocation_index[256];
static int             have_colors = 0;
#endif /* _WX */


/*----------------------------------------------------------------------
   InstallColor try to install a color in the public colormap.
  ----------------------------------------------------------------------*/
static void InstallColor (int i)
{
#ifdef _WINGUI
   Pix_Color[i] = RGB (RGB_Table[i].red, RGB_Table[i].green, RGB_Table[i].blue);
#endif /* _WINGUI */
#ifdef _WX
   if (Pix_Color[i])
     {
       Pix_Color[i]->Set( RGB_Table[i].red,
			  RGB_Table[i].green,
			  RGB_Table[i].blue );
     }
   else
     {
       Pix_Color[i] = new wxColour( RGB_Table[i].red,
				    RGB_Table[i].green,
				    RGB_Table[i].blue );
     }
#endif /* _WX */
}

/*----------------------------------------------------------------------
   ApproximateColors : this function is called when there is no
   more free slot in the colormap. It adjusts the colors allocated
   upon startup depending on the existing colormap to minimize
   the distances between the requested one and existing one.
   The algorithm is based on the order of the corlor in Thot
   color base and should be far less expensive than an optimal
   (e.g. closest in cube) algorithm.
  ----------------------------------------------------------------------*/
static void ApproximateColors (void)
{
   ThotColor  white = Pix_Color[0];
   ThotColor  col;
   int        line, b;

   for (line = 1; line < (NColors / 8); line++)
     {
	/*
	 * on each line, colors are stored from the brightest to
	 * the darkest. Unallocated cells are white.
	 * the fourth row should be allocated or the whole line is
	 * white.
	 */
#ifndef _WX
        col = Pix_Color[line * 8 + 4];
	for (b = 4; b < 8; b++)
	   if (Pix_Color[line * 8 + b] != white)
	      col = Pix_Color[line * 8 + b];
	   else
	      Pix_Color[line * 8 + b] = col;

	col = Pix_Color[line * 8 + 4];
	for (b = 4; b >= 0; b--)
	   if (Pix_Color[line * 8 + b] != white)
	      col = Pix_Color[line * 8 + b];
	   else
	      Pix_Color[line * 8 + b] = col;
#else /* #ifndef _WX */
        col = Pix_Color[line * 8 + 4];
	for (b = 4; b < 8; b++)
	   if (*Pix_Color[line * 8 + b] != *white)
	      col = Pix_Color[line * 8 + b];
	   else
	   {
	      if (Pix_Color[line * 8 + b])
		*Pix_Color[line * 8 + b] = *col;
	      else
		Pix_Color[line * 8 + b] = new wxColour(*col);
	   }	

	col = Pix_Color[line * 8 + 4];
	for (b = 4; b >= 0; b--)
	   if (*Pix_Color[line * 8 + b] != *white)
	      col = Pix_Color[line * 8 + b];
	   else
	   {
	      if (Pix_Color[line * 8 + b])
		*Pix_Color[line * 8 + b] = *col;
	      else
		Pix_Color[line * 8 + b] = new wxColour(*col);
	   }
#endif /* #ifndef _WX */
     }
}

/*----------------------------------------------------------------------
 *      FreeDocColors frees the Thot predefined X-Window colors.
 ----------------------------------------------------------------------*/
void         FreeDocColors ()
{
#ifndef _WIN_PRINT
#ifdef _WINGUI
  /* free extended colors */
  if (!TtIsTrueColor && TtCmap && !DeleteObject (TtCmap))
    WinErrorBox (WIN_Main_Wd, "FreeDocColors (1)");
  TtCmap = 0;
#endif /* _WINGUI */
#if defined(_WX)
  int        i;

  /* free standard colors */
  for (i = 0; i < NColors; i++)
    if (Pix_Color[i])
      {
        delete ((wxColour *)Pix_Color[i]);
        Pix_Color[i] = NULL;
      }

  /* free extended colors */
  for (i = 0; i < NbExtColors; i++)
    if (ExtColor && ExtColor[i])
      {
        delete ((wxColour *)ExtColor[i]);
        ExtColor[i] = NULL;
      }
#endif /* #if defined(_WX) */
#endif /* _WIN_PRINT */

  TtaFreeMemory (ExtRGB_Table);
  TtaFreeMemory (ExtColor);
  TtaFreeMemory (ExtCount_Table);
  ExtRGB_Table = NULL;
  ExtColor = NULL;
  ExtCount_Table = NULL;
}

/*----------------------------------------------------------------------
   InitDocColors initialize the Thot internal color table.
   If ReduceColor environment setting is set, less color
   are allocated.
  ----------------------------------------------------------------------*/
void InitDocColors (const char *name)
{
#ifdef _WINGUI
  PALETTEENTRY        sysPalEntries[256];
  int                 palSize, max;
  int                 best;
  int                 i, j;
  unsigned int        delred, delgreen, delblue;
  unsigned int        dsquare;
  unsigned int        best_dsquare;

  /* Initializes the color table */
  NColors = MAX_COLOR;
  RGB_Table = RGB_colors;
  Color_Table = Name_colors;
  palSize = GetDeviceCaps (TtDisplay, SIZEPALETTE);
  TtIsTrueColor = (palSize == 0);
  Pix_Color[0] = RGB (255, 255, 255);
  Pix_Color[1] = RGB (0, 0, 0);
  /* Create a color palette for the Thot set of colors. */
  /* fill-in the Pix_Color table */
  max = GetSystemPaletteEntries (TtDisplay, 0, palSize, sysPalEntries);

  if (TtIsTrueColor)
    {
      for (i = 0; i < MAX_COLOR; i++) 
	Pix_Color[i] = RGB (RGB_Table[i].red, RGB_Table[i].green, RGB_Table[i].blue);
      /* accept 512 any new colors */
      Max_Extend_Colors = 512;
      ExtRGB_Table = (RGBstruct *) TtaGetMemory (Max_Extend_Colors * sizeof (RGBstruct));
      ExtColor = (ThotColor *) TtaGetMemory (Max_Extend_Colors * sizeof (ThotColor));
      ExtCount_Table = (int *) TtaGetMemory (Max_Extend_Colors * sizeof (int));
      NbExtColors = 0;
    }
  else
    {
      for (i = 2; i < MAX_COLOR; i++)
	{
      best = 0;
	  best_dsquare = (unsigned int) -1;
	  for (j = 0; j < max; j++)
	    {
	      delred   = sysPalEntries[j].peRed - RGB_Table[i].red;
	      delgreen = sysPalEntries[j].peGreen - RGB_Table[i].green;
	      delblue  = sysPalEntries[j].peBlue - RGB_Table[i].blue;
	      dsquare  = delred * delred + delgreen * delgreen + delblue * delblue;
	      if (dsquare < best_dsquare)
		{
		  best = j;
		  best_dsquare = dsquare;
		}
	    }
	  Pix_Color[i] = RGB (sysPalEntries[best].peRed, sysPalEntries[best].peGreen,
			      sysPalEntries[best].peBlue);
	}
      /* accept only 256 system colors */
      Max_Extend_Colors = max;
      NbExtColors = max;
      ExtRGB_Table = (RGBstruct *) TtaGetMemory (Max_Extend_Colors * sizeof (RGBstruct));
      ExtColor = (ThotColor *) TtaGetMemory (Max_Extend_Colors * sizeof (ThotColor));
      ExtCount_Table = (int *) TtaGetMemory (Max_Extend_Colors * sizeof (int));
      for (j = 0; j < max; j++)
	{
	  ExtRGB_Table[j].red = sysPalEntries[j].peRed;
	  ExtRGB_Table[j].green = sysPalEntries[j].peGreen;
	  ExtRGB_Table[j].blue = sysPalEntries[j].peBlue;
	  ExtCount_Table[j] = 1;
	  ExtColor[j] = RGB (sysPalEntries[j].peRed, sysPalEntries[j].peGreen,
			     sysPalEntries[j].peBlue);
	}
    }
#endif /* _WINGUI */
#ifdef _WX
  int                 i, j, k;
  char               *value;
  ThotBool            reducecolor;
  ThotBool            colormap_full;
  
  /* Initializes the color table */
  NColors = MAX_COLOR;
  RGB_Table = RGB_colors;
  Color_Table = Name_colors;
  reducecolor = FALSE;
  colormap_full = FALSE;
  value = TtaGetEnvString ("ReduceColor");
  if (value == NULL)
    reducecolor = FALSE;
  else if (!strcasecmp (value, "yes"))
    reducecolor = TRUE;
  else
    reducecolor = FALSE;

  /* set up black and white Pixels */
  Pix_Color[0] = new wxColour(_T("WHITE"));
  Pix_Color[1] = new wxColour(_T("BLACK"));;
  /* clean up everything with white */
  for (i = 2; i < NColors; i++)
    Pix_Color[i] = new wxColour( *Pix_Color[0] );  
  
  /* setup greyscale colors */
  for (i = 2; i < 8; i++)
    InstallColor (i);
  /* install the first row of primary colors */
  i = 4;
  for (i += 8; i < NColors; i += 8)
    InstallColor (i);

  /*
   * ApproximateColors is also point less but we can show
   * all the colors as issued from the primary, if allocated
   */
  if (colormap_full)
    {
      for (j = 1; j <= (NColors / 8); j++)
	for (i = j * 8, k = 0; (i < NColors) && (k < 8); i++, k++)
	  {         
	    if (Pix_Color[i])
	      *Pix_Color[i] = *Pix_Color[j * 8 + 4];
	    else
	      Pix_Color[i] = new wxColour( *Pix_Color[j * 8 + 4] );  
	  }
      return;
    }
  
  /* install the second row of colors */
  i = 2;
  for (i += 8; i < NColors; i += 8)
    InstallColor (i);
  /* install the the third and fourth rows of colors */
  i = 6;
  for (i += 8; i < NColors; i += 8)
    InstallColor (i);
  i = 0;
  for (i += 8; i < NColors; i += 8)
    InstallColor (i);

  /* here, if the user asked for reduced colormap, approximate colors */
  if (reducecolor)
    ApproximateColors ();
  else
    {
      /* install the last rows of colors */
      i = 3;
      for (i += 8; i < NColors; i += 8)
	InstallColor (i);
      i = 7;
      for (i += 8; i < NColors; i += 8)
	InstallColor (i);
      i = 5;
      for (i += 8; i < NColors; i += 8)
	InstallColor (i);
      i = 1;
      for (i += 8; i < NColors; i += 8)
	InstallColor (i);
      
      if (colormap_full)
	ApproximateColors ();
    }
  NbExtColors = 0;
  if (TtWDepth <= 8)
    Max_Extend_Colors = 256;
  else
    Max_Extend_Colors = 512;

  ExtRGB_Table = (RGBstruct *) TtaGetMemory (Max_Extend_Colors * sizeof (RGBstruct));
  memset( ExtRGB_Table, 0, Max_Extend_Colors * sizeof (RGBstruct) );

  ExtColor = (ThotColor *) TtaGetMemory (Max_Extend_Colors * sizeof (ThotColor));
  memset( ExtColor, 0, Max_Extend_Colors * sizeof (ThotColor) );

  ExtCount_Table = (int *) TtaGetMemory (Max_Extend_Colors * sizeof (int));
  memset( ExtCount_Table, 0, Max_Extend_Colors * sizeof (int) );  
#endif /* _WX */
}

/*----------------------------------------------------------------------
   NumberOfColors  returns the number of colors in Thot color table.
  ----------------------------------------------------------------------*/
int NumberOfColors ()
{
   return NColors;
}


/*----------------------------------------------------------------------
   ColorName       returns the name of a color in Thot color table.
  ----------------------------------------------------------------------*/
const char *ColorName (int num)
{
   if (num < NColors && num >= 0)
      return Color_Table[num];
   else
      return NULL;
}

/*----------------------------------------------------------------------
   ColorPixel      returns the value of a color in Thot color table.
  ----------------------------------------------------------------------*/
ThotColor ColorPixel (int num)
{
#if defined(_WINGUI) || defined(_WX)
   if (num < NColors && num >= 0)
    return (Pix_Color[num]);
  else if (num < NColors + NbExtColors && num >= 0)
    return (ExtColor[num - NColors]);
  else
    return ((ThotColor) 0);
#endif /* #if defined(_WINGUI) || defined(_WX) */
}

/*----------------------------------------------------------------------
  TtaFreeThotColor frees the Thot Color.
 ----------------------------------------------------------------------*/
void TtaFreeThotColor (int num)
{
  if (num < NColors + NbExtColors && num >= NColors)
    {
      num -= NColors;
      if (ExtCount_Table[num] == 1)
	{
#if defined(_WX)
	  if (ExtColor[num])
	    delete ExtColor[num];
	  ExtColor[num] = (ThotColor) 0;
#endif /* defined(_WX) */
	}
      ExtCount_Table[num]--;
    }
}

/*----------------------------------------------------------------------
  TtaGetThotColor returns the Thot Color.
  red, green, blue express the color RGB in 8 bits values
 ----------------------------------------------------------------------*/
int TtaGetThotColor (unsigned short red, unsigned short green,
		     unsigned short blue)
{
  short               delred, delgreen, delblue;
  int                 best;
  int                 i, prev;
  unsigned int        dsquare;
  unsigned int        best_dsquare = (unsigned int) -1;
  ThotBool            found;
   
  /*
   * lookup for the color number among the color set allocated
   * by the application.
   * The lookup is based on a closest in cube algorithm hence
   * we try to get the closest color available for the display.
   */
  best = 0;			/* best color in list not found */
  for (i = 0; i < NColors; i++)
    {
      delred = RGB_Table[i].red - red;
      delgreen = RGB_Table[i].green - green;
      delblue = RGB_Table[i].blue - blue;
      dsquare = delred * delred + delgreen * delgreen + delblue * delblue;
      if (dsquare < best_dsquare)
	{
	  best = i;
	  best_dsquare = dsquare;
	}
    }

  if (best_dsquare != 0)
    {
      prev = Max_Extend_Colors;
      /* look in the extended table */
      for (i = 0; i < NbExtColors; i++)
	{
	  if (ExtCount_Table[i] > 0)
	    {
	      delred = ExtRGB_Table[i].red - red;
	      delgreen = ExtRGB_Table[i].green - green;
	      delblue = ExtRGB_Table[i].blue - blue;
	      dsquare  = delred * delred + delgreen * delgreen + delblue * delblue;
	      if (dsquare < best_dsquare)
		{
		  best = i + NColors;
		  best_dsquare = dsquare;
		}
	    }
	  else if (prev == Max_Extend_Colors)
	    /* get the first empty entry */
	    prev = i;
	}

      if (prev == Max_Extend_Colors)
	/* this is the first empty entry */
	prev = NbExtColors;
	 
      if (best_dsquare != 0 && prev < Max_Extend_Colors)
	{
	  /* try to allocate the right color */
#ifdef _WINGUI
	  ExtColor[prev] = RGB ((BYTE)red, (BYTE)green, (BYTE)blue);
#endif  /* _WINGUI */
#if defined(_WX)
	  if (ExtColor[prev])
	    {
	      ExtColor[prev]->Set( red,
				   green,
				   blue );
	    }
	   else
	     {
	       ExtColor[prev] = new wxColour( red,
					      green,
					      blue );
	     }
#endif /* #if defined(_WX) */
#ifndef _WX	   
	   /* check if this color is already in the table */
	   found = FALSE;
	   for (i = 0; i < NColors && !found; i++)
	     found = (ExtColor[prev] == Pix_Color[i]);
	   for (i = 0; i < NbExtColors && !found; i++)
	     found = (ExtColor[prev] == ExtColor[i]);
#else /* #ifndef _WX */
	   /* check if this color is already in the table */
	   found = FALSE;
	   for (i = 0; i < NColors && !found; i++)
	     found = (*ExtColor[prev] == *Pix_Color[i]);
	   for (i = 0; i < NbExtColors && !found; i++)
	     found = (*ExtColor[prev] == *ExtColor[i]);	   
#endif /* #ifndef _WX */
	   if (!found)
	     {
#ifdef _WINGUI
	       ExtRGB_Table[prev].red = red;
	       ExtRGB_Table[prev].green = green;
	       ExtRGB_Table[prev].blue = blue;
#endif  /* _WINGUI */
#ifdef _WX
	       ExtRGB_Table[prev].red = red;
	       ExtRGB_Table[prev].green = green;
	       ExtRGB_Table[prev].blue = blue;
#endif  /* _WX */
	       best = prev + NColors;
	       ExtCount_Table[prev] = 1;
	       if (prev == NbExtColors)
		 NbExtColors++;
	     }
	   else if (best >= NColors)
	     /* it's an already allocated extended color */
	     ExtCount_Table[best - NColors]++;
	 }
       else if (best >= NColors)
	 ExtCount_Table[best - NColors]++;
     }
   return (best);
}

#ifdef _WINGUI
/*----------------------------------------------------------------------
  GetSystemColorIndex returns the index in the system palette
  ----------------------------------------------------------------------*/
unsigned char GetSystemColorIndex (unsigned short red, unsigned short green,
				   unsigned short blue)
{
  int                 best;
  int                 i;
  unsigned int        dr, dg, db;
  unsigned int        dsquare;
  unsigned int        best_dsquare = (unsigned int) -1;

  best = 0;
  for (i = 0; i < NbExtColors; i++)
    {
      dr = ExtRGB_Table[i].red - red;
      dg = ExtRGB_Table[i].green - green;
      db = ExtRGB_Table[i].blue - blue;
      dsquare  = dr * dr + dg * dg + db * db;
      if (dsquare < best_dsquare)
	{
	  best = i;
	  best_dsquare = dsquare;
	}
    }
   return (best);
}
#endif /* _WINGUI */

/*----------------------------------------------------------------------
   TtaGiveThotRGB returns the Red Green and Blue values corresponding
   to color number num.
   If the color doesn't exist the function returns the values
   for the default color.
  ----------------------------------------------------------------------*/
void TtaGiveThotRGB (int num, unsigned short *red, unsigned short *green,
		     unsigned short *blue)
{
  if (num < NColors && num >= 0)
    {
      *red   = RGB_Table[num].red;
      *green = RGB_Table[num].green;
      *blue  = RGB_Table[num].blue;
    }
  else if (num < NColors + NbExtColors && num >= 0)
    {
      num -= NColors;
      *red   = ExtRGB_Table[num].red;
      *green = ExtRGB_Table[num].green;
      *blue  = ExtRGB_Table[num].blue;
    }
  else
    {
      *red   = RGB_Table[1].red;
      *green = RGB_Table[1].green;
      *blue  = RGB_Table[1].blue;
    }
}

/*----------------------------------------------------------------------
   ColorNumber     lookup in Thot color table for an entry given it's
   name. Returns the index or -1 if not found.
  ----------------------------------------------------------------------*/
int ColorNumber (char *name)
{
   int                 i;
   ThotBool            found;

   found = FALSE;
   i = 0;
   if (Color_Table[i] == NULL)
      return -1;		/* the table is empty */
   do
      if (strcasecmp (Color_Table[i], name) == 0)
	 found = TRUE;
      else
	 i++;
   while (!found && i < NColors);
   if (found)
      return i;
   else
      return -1;
}


/*----------------------------------------------------------------------
   NumberOfPatterns        returns the number of pattern available.
  ----------------------------------------------------------------------*/
int NumberOfPatterns ()
{
   return NbPatterns;
}


/*----------------------------------------------------------------------
   PatternName     returns the name of a pattern available.
  ----------------------------------------------------------------------*/
const char *PatternName (int num)
{
   if (num < NumberOfPatterns () && num >= 0)
      return Patterns[num];
   else
      return NULL;
}


/*----------------------------------------------------------------------
   PatternNumber   lookup fo a pattern given it's name. Returns the
   index or -1 if not found.
  ----------------------------------------------------------------------*/
int PatternNumber (char *name)
{
   int                 i;
   int                 max;
   ThotBool            found;

   found = FALSE;
   i = 0;
   max = NumberOfPatterns ();
   do
      if (strcmp (Patterns[i], name) == 0)
	 found = TRUE;
      else
	 i++;
   while (!found && i < max);
   if (found)
      return i;
   else
      return -1;
}

/*----------------------------------------------------------------------
   CreatePattern loads and return a pixmap pattern.
   active parameter indicate if the box is active.
   parameters fg, bg, and motif indicate respectively
   the drawing color, background color and the pattern.
  ----------------------------------------------------------------------*/
ThotPixmap CreatePattern (int disp, int fg, int bg, int motif)
{
   ThotColor           FgPixel;
   ThotColor           BgPixel;
   ThotPixmap          pixmap;
#ifdef _WINGUI
   BITMAP              bitmap = {0, 0, 0, 1, 1, 0};
   HBITMAP             hBitmap;
#endif /* _WINGUI */

   FgPixel = ColorPixel (fg);
   BgPixel = ColorPixel (bg);
#ifdef _WINGUI
   if (WIN_LastBitmap != 0)
     {
	if (!DeleteObject (WIN_LastBitmap))
#ifndef _WIN_PRINT
	  WinErrorBox (WIN_Main_Wd, "CreatePattern (1)");
#endif /* _WIN_PRINT */
	;
	WIN_LastBitmap = 0;
     }
   switch (motif)
     {
     case 1:
       bitmap.bmType       = 0;
       bitmap.bmPlanes     = 1;
       bitmap.bmBitsPixel  = 1;
       bitmap.bmWidth      = gray8_width;
       bitmap.bmHeight     = gray8_height;
       bitmap.bmWidthBytes = 2;
       bitmap.bmBits       = gray8_bits;
       hBitmap = CreateBitmapIndirect (&bitmap);
       break;
     case 2:
       bitmap.bmType       = 0;
       bitmap.bmPlanes     = 1;
       bitmap.bmBitsPixel  = 1;
       bitmap.bmWidth      = gray0_width;
       bitmap.bmHeight     = gray0_height;
       /* bitmap.bmWidthBytes = gray0_width / 4; */
       bitmap.bmWidthBytes = 2;
       bitmap.bmBits       = gray0_bits;
       hBitmap = CreateBitmapIndirect (&bitmap);
       break;
     case 3:
       bitmap.bmType       = 0;
       bitmap.bmPlanes     = 1;
       bitmap.bmBitsPixel  = 1;
       bitmap.bmWidth      = gray1_width;
       bitmap.bmHeight     = gray1_height;
       bitmap.bmWidthBytes = 2;
       bitmap.bmBits       = gray1_bits;
       hBitmap = CreateBitmapIndirect (&bitmap);
       break;
     case 4:
       bitmap.bmType       = 0;
       bitmap.bmPlanes     = 1;
       bitmap.bmBitsPixel  = 1;
       bitmap.bmWidth      = gray2_width;
       bitmap.bmHeight     = gray2_height;
       bitmap.bmWidthBytes = 2;
       bitmap.bmBits       = gray2_bits;
       hBitmap = CreateBitmapIndirect (&bitmap);
       break;
     case 5:
       bitmap.bmType       = 0;
       bitmap.bmPlanes     = 1;
       bitmap.bmBitsPixel  = 1;
       bitmap.bmWidth      = gray3_width;
       bitmap.bmHeight     = gray3_height;
       bitmap.bmWidthBytes = 2;
       bitmap.bmBits       = gray3_bits;
       hBitmap = CreateBitmapIndirect (&bitmap);
       break;
     case 6:
       bitmap.bmType       = 0;
       bitmap.bmPlanes     = 1;
       bitmap.bmBitsPixel  = 1;
       bitmap.bmWidth      = gray4_width;
       bitmap.bmHeight     = gray4_height;
       bitmap.bmWidthBytes = 2;
       bitmap.bmBits       = gray4_bits;
       hBitmap = CreateBitmapIndirect (&bitmap);
       break;
     case 7:
       bitmap.bmType       = 0;
       bitmap.bmPlanes     = 1;
       bitmap.bmBitsPixel  = 1;
       bitmap.bmWidth      = gray5_width;
       bitmap.bmHeight     = gray5_height;
       bitmap.bmWidthBytes = 2;
       bitmap.bmBits       = gray5_bits;
       hBitmap = CreateBitmapIndirect (&bitmap);
       break;
     case 8:
       bitmap.bmType       = 0;
       bitmap.bmPlanes     = 1;
       bitmap.bmBitsPixel  = 1;
       bitmap.bmWidth      = gray6_width;
       bitmap.bmHeight     = gray6_height;
       bitmap.bmWidthBytes = 2;
       bitmap.bmBits       = gray6_bits;
       hBitmap = CreateBitmapIndirect (&bitmap);
       break;
     case 9:
       bitmap.bmType       = 0;
       bitmap.bmPlanes     = 1;
       bitmap.bmBitsPixel  = 1;
       bitmap.bmWidth      = gray7_width;
       bitmap.bmHeight     = gray7_height;
       bitmap.bmWidthBytes = 2;
       bitmap.bmBits       = gray7_bits;
       hBitmap = CreateBitmapIndirect (&bitmap);
       break;
     case 10:
       bitmap.bmType       = 0;
       bitmap.bmPlanes     = 1;
       bitmap.bmBitsPixel  = 1;
       bitmap.bmWidth      = horiz1_width;
       bitmap.bmHeight     = horiz1_height;
       bitmap.bmWidthBytes = 2;
       bitmap.bmBits       = horiz1_bits;
       hBitmap = CreateBitmapIndirect (&bitmap);
       break;
     case 11:
       bitmap.bmType       = 0;
       bitmap.bmPlanes     = 1;
       bitmap.bmBitsPixel  = 1;
       bitmap.bmWidth      = horiz2_width;
       bitmap.bmHeight     = horiz2_height;
       bitmap.bmWidthBytes = 2;
       bitmap.bmBits       = horiz2_bits;
       hBitmap = CreateBitmapIndirect (&bitmap);
       break;
     case 12:
       bitmap.bmType       = 0;
       bitmap.bmPlanes     = 1;
       bitmap.bmBitsPixel  = 1;
       bitmap.bmWidth      = horiz3_width;
       bitmap.bmHeight     = horiz3_height;
       bitmap.bmWidthBytes = 2;
       bitmap.bmBits       = horiz3_bits;
       hBitmap = CreateBitmapIndirect (&bitmap);
       break;
     case 13:
       bitmap.bmType       = 0;
       bitmap.bmPlanes     = 1;
       bitmap.bmBitsPixel  = 1;
       bitmap.bmWidth      = vert1_width;
       bitmap.bmHeight     = vert1_height;
       bitmap.bmWidthBytes = 2;
       bitmap.bmBits       = vert1_bits;
       hBitmap = CreateBitmapIndirect (&bitmap);
       break;
     case 14:
       bitmap.bmType       = 0;
       bitmap.bmPlanes     = 1;
       bitmap.bmBitsPixel  = 1;
       bitmap.bmWidth      = vert2_width;
       bitmap.bmHeight     = vert2_height;
       bitmap.bmWidthBytes = 2;
       bitmap.bmBits       = vert2_bits;
       hBitmap = CreateBitmapIndirect (&bitmap);
       break;
     case 15:
       bitmap.bmType       = 0;
       bitmap.bmPlanes     = 1;
       bitmap.bmBitsPixel  = 1;
       bitmap.bmWidth      = vert3_width;
       bitmap.bmHeight     = vert3_height;
       bitmap.bmWidthBytes = 2;
       bitmap.bmBits       = vert3_bits;
       hBitmap = CreateBitmapIndirect (&bitmap);
       break;
     case 16:
       bitmap.bmType       = 0;
       bitmap.bmPlanes     = 1;
       bitmap.bmBitsPixel  = 1;
       bitmap.bmWidth      = left1_width;
       bitmap.bmHeight     = left1_height;
       bitmap.bmWidthBytes = 2;
       bitmap.bmBits       = left1_bits;
       hBitmap = CreateBitmapIndirect (&bitmap);
       break;
     case 17:
       bitmap.bmType       = 0;
       bitmap.bmPlanes     = 1;
       bitmap.bmBitsPixel  = 1;
       bitmap.bmWidth      = left2_width;
       bitmap.bmHeight     = left2_height;
       bitmap.bmWidthBytes = 2;
       bitmap.bmBits       = left2_bits;
       hBitmap = CreateBitmapIndirect (&bitmap);
       break;
     case 18:
       bitmap.bmType       = 0;
       bitmap.bmPlanes     = 1;
       bitmap.bmBitsPixel  = 1;
       bitmap.bmWidth      = left3_width;
       bitmap.bmHeight     = left3_height;
       bitmap.bmWidthBytes = 2;
       bitmap.bmBits       = left3_bits;
       hBitmap = CreateBitmapIndirect (&bitmap);
       break;
     case 19:
       bitmap.bmType       = 0;
       bitmap.bmPlanes     = 1;
       bitmap.bmBitsPixel  = 1;
       bitmap.bmWidth      = right1_width;
       bitmap.bmHeight     = right1_height;
       bitmap.bmWidthBytes = 2;
       bitmap.bmBits       = right1_bits;
       hBitmap = CreateBitmapIndirect (&bitmap);
       break;
     case 20:
       bitmap.bmType       = 0;
       bitmap.bmPlanes     = 1;
       bitmap.bmBitsPixel  = 1;
       bitmap.bmWidth      = right2_width;
       bitmap.bmHeight     = right2_height;
       bitmap.bmWidthBytes = 2;
       bitmap.bmBits       = right2_bits;
       hBitmap = CreateBitmapIndirect (&bitmap);
       break;
     case 21:
       bitmap.bmType       = 0;
       bitmap.bmPlanes     = 1;
       bitmap.bmBitsPixel  = 1;
       bitmap.bmWidth      = right3_width;
       bitmap.bmHeight     = right3_height;
       bitmap.bmWidthBytes = 2;
       bitmap.bmBits       = right3_bits;
       hBitmap = CreateBitmapIndirect (&bitmap);
       break;
     case 22:
       bitmap.bmType       = 0;
       bitmap.bmPlanes     = 1;
       bitmap.bmBitsPixel  = 1;
       bitmap.bmWidth      = square1_width;
       bitmap.bmHeight     = square1_height;
       bitmap.bmWidthBytes = 2;
       bitmap.bmBits       = square1_bits;
       hBitmap = CreateBitmapIndirect (&bitmap);
       break;
     case 23:
       bitmap.bmType       = 0;
       bitmap.bmPlanes     = 1;
       bitmap.bmBitsPixel  = 1;
       bitmap.bmWidth      = square2_width;
       bitmap.bmHeight     = square2_height;
       bitmap.bmWidthBytes = 2;
       bitmap.bmBits       = square2_bits;
       hBitmap = CreateBitmapIndirect (&bitmap);
       break;
     case 24:
       bitmap.bmType       = 0;
       bitmap.bmPlanes     = 1;
       bitmap.bmBitsPixel  = 1;
       bitmap.bmWidth      = square3_width;
       bitmap.bmHeight     = square3_height;
       bitmap.bmWidthBytes = 24;
       bitmap.bmBits       = square3_bits;
       hBitmap = CreateBitmapIndirect (&bitmap);
       break;
     case 25:
       bitmap.bmType       = 0;
       bitmap.bmPlanes     = 1;
       bitmap.bmBitsPixel  = 1;
       bitmap.bmWidth      = lozenge_width;
       bitmap.bmHeight     = lozenge_height;
       bitmap.bmWidthBytes = 2;
       bitmap.bmBits       = lozenge_bits;
       hBitmap = CreateBitmapIndirect (&bitmap);
       break;
     case 26:
       bitmap.bmType       = 0;
       bitmap.bmPlanes     = 1;
       bitmap.bmBitsPixel  = 1;
       bitmap.bmWidth      = brick_width;
       bitmap.bmHeight     = brick_height;
       bitmap.bmWidthBytes = 2;
       bitmap.bmBits       = brick_bits;
       hBitmap = CreateBitmapIndirect (&bitmap);
       break;
     case 27:
       bitmap.bmType       = 0;
       bitmap.bmPlanes     = 1;
       bitmap.bmBitsPixel  = 1;
       bitmap.bmWidth      = tile_width;
       bitmap.bmHeight     = tile_height;
       bitmap.bmWidthBytes = 2;
       bitmap.bmBits       = tile_bits;
       hBitmap = CreateBitmapIndirect (&bitmap);
       break;
     case 28:
       bitmap.bmType       = 0;
       bitmap.bmPlanes     = 1;
       bitmap.bmBitsPixel  = 1;
       bitmap.bmWidth      = sea_width;
       bitmap.bmHeight     = sea_height;
       bitmap.bmWidthBytes = 2;
       bitmap.bmBits       = sea_bits;
       hBitmap = CreateBitmapIndirect (&bitmap);
       break;
     case 29:
       bitmap.bmType       = 0;
       bitmap.bmPlanes     = 1;
       bitmap.bmBitsPixel  = 1;
       bitmap.bmWidth      = basket_width;
       bitmap.bmHeight     = basket_height;
       bitmap.bmWidthBytes = 2;
       bitmap.bmBits       = basket_bits;
       hBitmap = CreateBitmapIndirect (&bitmap);
       break;
     default:
       hBitmap = 0;
       break;
     }
   /* WIN_LastBitmap = hBitmap; */
   pixmap = hBitmap;
#endif /* _WINGUI */
#if defined(_WX)
   /* TODO */
   pixmap = NULL;
#endif /* defined(_WX) */
   return (pixmap);
}
