/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2001.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * Authors: I. Vatton, N. Layaida (INRIA)
 *          R. Guetari (W3C/INRIA) - Windows version
 */

#include "thot_gui.h"
#include "thot_sys.h"
#include "png.h"
#include "constmedia.h"
#include "typemedia.h"
#include "picture.h"
#include "frame.h"
#include "message.h"
#ifdef _WINDOWS
#include "wininclude.h"
#endif /* _WINDOWS */

#define THOT_EXPORT extern
#include "picture_tv.h"

#include "font_f.h"
#include "units_f.h"
#include "frame_tv.h"
#include "picture_f.h"
#include "gifhandler_f.h"
#include "memory_f.h"
#define min(a, b) (((a) < (b)) ? (a) : (b))


int Magic16[256] =    /* for 16 levels of gray */
{
    0, 13, 3, 12, 1, 14, 4, 13, 0, 13, 3, 12, 1, 14, 4, 13,
    10, 5, 8, 6, 11, 6, 8, 6, 10, 5, 8, 6, 11, 5, 8, 6,
    11, 2, 14, 1, 12, 3, 15, 2, 11, 2, 14, 1, 12, 3, 15, 2,
    7, 8, 4, 9, 7, 9, 5, 10, 7, 9, 4, 10, 7, 9, 5, 10,
    1, 14, 3, 13, 0, 13, 3, 12, 0, 14, 3, 13, 0, 13, 3, 13,
    11, 5, 8, 6, 11, 5, 8, 6, 11, 5, 8, 6, 11, 5, 8, 6,
    12, 3, 15, 2, 12, 2, 14, 1, 12, 2, 15, 1, 12, 2, 14, 1,
    7, 9, 4, 10, 7, 9, 4, 10, 7, 9, 4, 10, 7, 9, 4, 10,
    1, 14, 4, 13, 0, 13, 3, 12, 1, 14, 4, 13, 0, 13, 3, 12,
    11, 5, 8, 6, 10, 5, 8, 6, 11, 6, 8, 7, 10, 5, 8, 6,
    12, 3, 15, 2, 11, 2, 14, 1, 12, 3, 15, 2, 11, 2, 14, 1,
    7, 9, 4, 10, 7, 9, 4, 9, 7, 9, 5, 10, 7, 8, 4, 9,
    0, 14, 3, 13, 1, 14, 3, 13, 0, 13, 3, 12, 1, 14, 3, 13,
    11, 5, 8, 6, 11, 5, 8, 6, 11, 5, 8, 6, 11, 5, 8, 6,
    12, 2, 14, 1, 12, 2, 15, 1, 11, 2, 14, 1, 12, 2, 15, 2,
    7, 9, 4, 10, 7, 9, 4, 10, 7, 9, 4, 10, 7, 9, 4, 10

};

int Magic32[256] =    /* for 8 levels of green */
{
    0, 27, 6, 25, 2, 29, 8, 27, 0, 27, 6, 26, 2, 29, 7, 27,
    21, 10, 16, 12, 23, 11, 17, 13, 22, 10, 16, 12, 23, 11, 17, 13,
    23, 4, 29, 2, 25, 6, 31, 4, 24, 4, 29, 2, 25, 5, 31, 4,
    14, 17, 8, 19, 15, 19, 9, 21, 14, 18, 8, 20, 15, 19, 9, 21,
    1, 28, 7, 27, 1, 28, 6, 26, 1, 28, 7, 26, 1, 28, 7, 26,
    23, 11, 17, 13, 22, 10, 16, 12, 22, 11, 16, 13, 22, 10, 16, 12,
    25, 5, 30, 3, 24, 4, 30, 3, 24, 5, 30, 3, 24, 5, 30, 3,
    15, 19, 9, 21, 14, 18, 8, 20, 15, 18, 9, 20, 14, 18, 8, 20,
    1, 29, 7, 27, 0, 27, 6, 25, 2, 29, 8, 27, 0, 27, 6, 25,
    23, 11, 17, 13, 22, 10, 16, 12, 23, 12, 17, 13, 21, 10, 16, 12,
    25, 5, 31, 3, 23, 4, 29, 2, 25, 6, 31, 4, 23, 4, 29, 2,
    15, 19, 9, 21, 14, 18, 8, 20, 15, 19, 10, 21, 14, 18, 8, 19,
    1, 28, 7, 26, 1, 28, 7, 26, 0, 28, 6, 26, 1, 28, 7, 26,
    22, 11, 16, 12, 22, 11, 17, 13, 22, 10, 16, 12, 23, 11, 17, 13,
    24, 5, 30, 3, 24, 5, 30, 3, 24, 4, 30, 2, 24, 5, 30, 3,
    14, 18, 9, 20, 15, 19, 9, 20, 14, 18, 8, 20, 15, 19, 9, 21

};

int Magic64[256] =    /* for 4 levels of red and blue */
{
    0, 55, 12, 51, 3, 59, 15, 55, 1, 56, 13, 52, 3, 58, 15, 54,
    43, 20, 32, 24, 47, 23, 35, 27, 44, 20, 32, 24, 47, 23, 35, 27,
    47, 8, 59, 4, 51, 11, 63, 7, 48, 9, 60, 5, 50, 11, 62, 7,
    28, 35, 16, 39, 31, 39, 19, 43, 28, 36, 16, 40, 31, 39, 19, 43,
    3, 58, 15, 54, 1, 56, 13, 52, 2, 57, 14, 53, 1, 57, 13, 53,
    46, 22, 34, 26, 45, 21, 33, 25, 45, 22, 33, 26, 45, 21, 33, 25,
    50, 11, 62, 7, 48, 9, 60, 5, 49, 10, 61, 6, 49, 9, 61, 5,
    30, 38, 18, 42, 29, 37, 17, 41, 30, 37, 18, 41, 29, 37, 17, 41,
    3, 58, 15, 54, 0, 56, 12, 52, 4, 59, 16, 55, 0, 55, 12, 51,
    46, 23, 34, 27, 44, 20, 32, 24, 47, 23, 35, 27, 44, 20, 32, 24,
    50, 11, 62, 7, 48, 8, 60, 4, 51, 12, 63, 8, 47, 8, 59, 4,
    31, 38, 19, 42, 28, 36, 16, 40, 31, 39, 19, 43, 28, 36, 16, 40,
    2, 57, 14, 53, 2, 57, 14, 53, 1, 56, 13, 52, 2, 58, 14, 54,
    45, 21, 33, 25, 46, 22, 34, 26, 44, 21, 32, 25, 46, 22, 34, 26,
    49, 10, 61, 6, 49, 10, 61, 6, 48, 9, 60, 5, 50, 10, 62, 6,
    29, 37, 17, 41, 30, 38, 18, 42, 29, 36, 17, 40, 30, 38, 18, 42

};

static png_color        Std_color_cube[128];
#ifdef _WINDOWS     
extern ThotBool         pic2print;
#endif /* _WINDOWS */

char  *typecouleur[] = {"grayscale", "undefined type", "RGB",
			"colormap", "grayscale+alpha",
			"undefined type", "RGB+alpha"};

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
static void PError (png_struct *png_ptr, char *message)
{
   fprintf(stderr,"libpng error: %s\n", message);
   longjmp(png_ptr->jmpbuf, 1);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
static void PWarning (png_struct *png_ptr, char *message)
{
   if (!png_ptr)
     return;
   fprintf(stderr,"libpng warning: %s\n", message);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
static unsigned char *ReadPng (FILE *infile, int *width, int *height, int *ncolors,
			       int *cpp, ThotColorStruct **colrs, int *bg)
{
  png_byte        *pp;
  png_byte         buf[8];
  png_struct      *png_ptr = NULL;
  png_info        *info_ptr = NULL;
  png_byte        *png_pixels = NULL;
  png_byte       **row_pointers = NULL;
  ThotColorStruct *colors = NULL, *ptr;
  unsigned char   *pixels = NULL;
  unsigned short  *spixels = NULL;
  double           gamma_correction;
  unsigned int     bytesPerExpandedLine;
  int              alpha;
  int              color_type;
  int              ret, gr, isgrey;
  int              i, j, passes;
  int              xpos, ypos, ind;
  int              xr, xg, xb;
  int              r, g, b, v, a;
  int              bit_depth, interlace_type;
  int              cr, cg, cb, cgr;
  int              row, col, val;
  ThotBool         found;

  ret = fread (buf, 1, 8, infile);
  if (ret != 8)
    return NULL;
  
  ret = png_check_sig (buf, 8);	
  if (!ret)
    return NULL;
  rewind (infile);
  png_ptr = png_create_read_struct (PNG_LIBPNG_VER_STRING, (png_voidp)PError,
				    (png_voidp)PError, (png_voidp)PWarning);
  if (png_ptr == NULL)
    return NULL;

  /* Allocate/initialize the memory for image information.  REQUIRED. */
  info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == NULL)
    {
      png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
      return NULL;
    }

  if (setjmp (png_ptr->jmpbuf))
    {
      /* Free all of the memory associated with the png_ptr and info_ptr */
      png_destroy_read_struct (&png_ptr, &info_ptr, (png_infopp)NULL);
      /* sends a message to the user throught thot GUI */ 	
      fprintf(stderr, "ping read image error \n");
      if (row_pointers)
	TtaFreeMemory (row_pointers);
      if (pixels)
	TtaFreeMemory (pixels);
      if (png_pixels)
	TtaFreeMemory (png_pixels);
      return NULL;
    }

   /* Set up the input control if you are using standard C streams */
  png_init_io (png_ptr, infile);
    
  /* If we have already read some of the signature */
  /*png_set_sig_bytes(png_ptr, sig_read);*/
  /* read the file information */
  png_read_info (png_ptr, info_ptr);
    
  png_get_IHDR (png_ptr, info_ptr, (png_uint_32 *)width, (png_uint_32 *)height,
		&bit_depth, &color_type, &interlace_type, NULL, NULL);

  /* expand color components to 8 bit rgb */
  png_set_expand (png_ptr);
    
  /* tell libpng to handle the gamma conversion for you */
  if (info_ptr->valid & PNG_INFO_gAMA) 
    gamma_correction = info_ptr->gamma;
  else 
    gamma_correction = (double)0.45;

  if (fabs ((2.2 * gamma_correction) - 1.0) > 0.011)
    png_set_gamma (png_ptr, (double) 2.2, gamma_correction);
    
  /* strip 16-bit channels down to 8 */ 
  if (info_ptr->bit_depth == 16)
    png_set_strip_16 (png_ptr);

  passes = png_set_interlace_handling (png_ptr);
  /*   if (info_ptr->color_type & PNG_COLOR_MASK_COLOR) {*/
  if (info_ptr->valid & PNG_INFO_PLTE)
    png_set_dither (png_ptr, info_ptr->palette, info_ptr->num_palette,
		    256, info_ptr->hist, 1); 
  
  png_read_update_info (png_ptr, info_ptr);
  /* setup other stuff using the fields of png_info. */
  *width  = (int) png_ptr->width;
  *height = (int) png_ptr->height;
  /* calculate the bytes per line (if the pixels were expanded to 1 pixel/byte */ 
  bytesPerExpandedLine = (*width) * info_ptr->channels;
  png_pixels = (png_byte*) TtaGetMemory (bytesPerExpandedLine * (*height) * sizeof (png_byte));
  if (png_pixels == NULL) 
    png_error (png_ptr,"not enough memory ");
    
  row_pointers = (png_bytep*) TtaGetMemory ((*height) * sizeof(png_bytep /**/));
  if (row_pointers == NULL)
    png_error (png_ptr, "not enough memory ");

  for (i = 0; i < *height; i++)
    row_pointers[i] = png_pixels + (bytesPerExpandedLine * i);    
    
  /*   png_read_image(png_ptr, row_pointers);*/
  color_type = info_ptr->color_type;
  alpha = color_type & PNG_COLOR_MASK_ALPHA;
  color_type &= ~PNG_COLOR_MASK_ALPHA;
  if (color_type == PNG_COLOR_TYPE_PALETTE)
    {
      if (info_ptr->num_palette > 256)
	*ncolors = 256;
      else
	*ncolors = info_ptr->num_palette;
      colors = TtaGetMemory ((*ncolors) * sizeof (ThotColorStruct));
      for (i = 0; i < *ncolors; i++)
	{
#ifdef _WINDOWS
	  colors[i].red   = info_ptr->palette[i].red;
	  colors[i].green = info_ptr->palette[i].green;
	  colors[i].blue  = info_ptr->palette[i].blue;
#else /* _WINDOWS */
	  colors[i].red   = info_ptr->palette[i].red << 8;
	  colors[i].green = info_ptr->palette[i].green << 8;
	  colors[i].blue  = info_ptr->palette[i].blue << 8;
	  colors[i].pixel = i;
#ifndef _GTK
	  colors[i].flags = DoRed|DoGreen|DoBlue;
#endif /* _GTK */
#endif /* _WINDOWS */
	}
    }
  else if (color_type == PNG_COLOR_TYPE_RGB)
    {
      if (TtWDepth > 8)
	{
	  /* Generate the image palette */
	  colors = TtaGetMemory (512 * sizeof (ThotColorStruct));
	  *ncolors = 0; 
	}
      else
	{
	  /* Use the standard palette */
	  colors = TtaGetMemory (128 * sizeof (ThotColorStruct));
	  *ncolors = 128;
	  for (i = 0; i < *ncolors ; i++)
	    {
#ifdef _WINDOWS
	      colors[i].red = Std_color_cube[i].red;
	      colors[i].green = Std_color_cube[i].green;
	      colors[i].blue = Std_color_cube[i].blue;
#else /* _WINDOWS */
	      colors[i].red = Std_color_cube[i].red << 8;
	      colors[i].green = Std_color_cube[i].green << 8;
	      colors[i].blue = Std_color_cube[i].blue << 8;
#ifndef _GTK
	      colors[i].flags = DoRed|DoGreen|DoBlue;
#endif /* ! _GTK */
#endif /* _WINDOWS */
	    }
	}
    }
  else if (color_type == PNG_COLOR_TYPE_GRAY)
    {
     if (TtWDepth > 8)
	{
	  /* Generate the image palette */
	  colors = TtaGetMemory (512 * sizeof (ThotColorStruct));
	  *ncolors = 0;
	}
      else
	{
	  /* Use the standard palette */
	  colors = TtaGetMemory (16 * sizeof (ThotColorStruct));
	  *ncolors = 16; 
	  for (i = 0; i < 15; i++)
	    {
#ifdef _WINDOWS
	      colors[i].red = colors[i].green = colors[i].blue = i;
#else /* _WINDOWS */
	      colors[i].red = colors[i].green = colors[i].blue = i << 8;
#ifndef _GTK
	      colors[i].flags = DoRed|DoGreen|DoBlue;
#endif /* _GTK */
#endif /* _WINDOWS */
	    }
	}
    }
  else
    png_error (png_ptr, "Unknown PNG color type ");
  
  /*png_read_update_info (png_ptr, info_ptr);*/
  png_start_read_image (png_ptr);
  for (i = 0; i < passes; i++)
    for (j = 0; j < *height; j++)
      png_read_row (png_ptr, NULL, row_pointers[j]);

  /* Generate the table that'll store an index in the color table for each pixel */
  pixels = (char *) TtaGetMemory ((*width) * (*height) * sizeof (char));
  if (pixels == NULL)
    png_error (png_ptr, "not enough memory ");
  pp = png_pixels;
  color_type = info_ptr->color_type;
  alpha = color_type & PNG_COLOR_MASK_ALPHA;
  color_type &= ~PNG_COLOR_MASK_ALPHA;
  switch (color_type)
    { 
    case  PNG_COLOR_TYPE_PALETTE:
      ind = 0; /* pixel index */
      for (ypos = 0; ypos < *height; ypos++)
	{
	  pp = row_pointers[ypos];
	  for (xpos = 0; xpos < *width; xpos++)
	    {
	      pixels[ind++] = *pp++; 
	      if (alpha)
		/* j'en ai pas vu encore des comme-ca mais on ne sait jamais */
		*pp = *pp + 1;
	    }
	}
      if (info_ptr->valid & PNG_INFO_tRNS)
	*bg = (info_ptr->background.index);
      break;

    case  PNG_COLOR_TYPE_GRAY:
    case PNG_COLOR_TYPE_RGB:
      ind = 0; /* pixel index */
      for (ypos = 0; ypos < *height; ypos ++)
	{
	  col = ypos & 0x0f;
	  pp = row_pointers[ypos];
	  for (xpos = 0; xpos < *width; xpos ++)
	    {
	      cgr = 0;
	      a = 0;
	      row = xpos & 0x0f;
	      if (color_type == PNG_COLOR_TYPE_GRAY)
		{
		  cr = cg = cb = cgr = (*pp++);
		  isgrey = 1;
		}
	      else if (color_type == PNG_COLOR_TYPE_RGB)
		{		
		  cr = (*pp++);
		  cg = (*pp++);
		  cb = (*pp++);
		  isgrey = 0;
		}
	      
	      /* the alpha channel is not yet handled :) */
	      if (alpha)
		{
		  a = (*pp++);
		  cr  = (int) ((a/255.0) * cr  + ((255.0-a)/255.0) * 211.0);
		  cg  = (int) ((a/255.0) * cg  + ((255.0-a)/255.0) * 211.0);
		  cb  = (int) ((a/255.0) * cb  + ((255.0-a)/255.0) * 211.0);
		  cgr = (int) ((a/255.0) * cgr + ((255.0-a)/255.0) * 211.0);
		}
   
	      if (isgrey && TtWDepth <= 8)
		{
		  /* Use the palette of 16 colors */
		  gr = cgr & 0xF0;
		  if (cgr - gr > Magic16[(row << 4) + col])
		    gr += 16;
		  gr = min(gr, 0xF0);
		  pixels[ind++] = gr >> 4;
		}
	      else if (TtWDepth <= 8)
		{
		  /* Use the palette of 128 colors */
		  r = cr & 0xC0;
		  g = cg & 0xE0;
		  b = cb & 0xC0;
		  v = (row << 4) + col;
		  if (cr - r > Magic64[v])
		    r += 64;
		  if (cg - g > Magic32[v])
		    g += 32;
		  if (cb - b > Magic64[v])
		    b += 64;
		  r = min(r, 255) & 0xC0;
		  g = min(g, 255) & 0xE0;
		  b = min(b, 255) & 0xC0;
		  pixels[ind++] = (unsigned char) ((r >> 6) | (g >> 3) | (b >> 1));
		}
	      else
		{    
		  /* Generate a palette for this image */
		  found = FALSE;
#ifndef _WINDOWS
		  cr = cr << 8;
		  cg = cg << 8;
		  cb = cb << 8;
#endif /* _WINDOWS */
		  i = 0;
		  while (i < *ncolors && !found)
		    {
		      if (colors[i].red == cr &&
			  colors[i].green == cg &&
			  colors[i].blue == cb)
			found = TRUE;
		      else
			i++;
		    }
		  if (!found)
		    {
		      if (*ncolors > 256 && spixels == NULL)
			{
			  /* we need two bytes per pixel */
			  spixels = (short *) TtaGetMemory ((*width) * (*height) * sizeof (short));
			  if (spixels)
			    {
			      for (j = 0; j < ind; j++)
				{
				  val = (int) pixels[j];
				  spixels[j] = (unsigned short) val;
				}
			      TtaFreeMemory (pixels);
			      pixels = NULL;
			    }
			}
		      if (*ncolors == 512)
			{
			  ptr = TtaRealloc (colors, sizeof (ThotColorStruct) * 6000);
			  if (ptr)
			    colors = ptr;
			  else
			    i--;
			}
		      if (*ncolors < 6000)
			{
			  /* New entry in the palette */
			  colors[i].red = cr;
			  colors[i].green = cg;
			  colors[i].blue = cb;
#if !defined(_WINDOWS) && !defined(_GTK)
			  colors[i].flags = DoRed|DoGreen|DoBlue;
#endif /* _WINDOWS && GTK */
			  *ncolors = i + 1;
			}
		      else
			i--;
		    }
		  if (spixels)
		    /* use two bytes per pixel */
		    spixels[ind++] = (unsigned short) i;
		  else if (pixels)
		    /* use one byte per pixel */
		    pixels[ind++] = (unsigned char) i;
		}
	    }
	}
      if (info_ptr->valid & PNG_INFO_tRNS)
	{
	  if (isgrey)
	    *bg = ((info_ptr->trans_values.gray) & 0xff) >> 4;
	  else
	    {
	      xr = info_ptr->trans_values.red;
	      xg = info_ptr->trans_values.green;
	      xb = info_ptr->trans_values.blue;
	      xr = xr & 0xff; xg = xg & 0xff; xb = xb & 0xff;
	      xr = min(xr, 255) & 0xC0;
	      xg = min(xg, 255) & 0xE0;
	      xb = min(xb, 255) & 0xC0;
	      *bg = (unsigned char) ((xr >> 6) | (xg >> 3) | (xb >> 1));
	    }
	}
      break;
    default:
      *bg = -1;
    }

  /* clean up after the read, and free any memory allocated */
  png_read_destroy (png_ptr, info_ptr, (png_info*) NULL);
  /* Free all of the memory associated with the png_ptr and info_ptr */
  png_destroy_read_struct (&png_ptr, &info_ptr, (png_infopp)NULL);
  /* free the structures */
  if (row_pointers)
    TtaFreeMemory (row_pointers);
  if (png_pixels)
    TtaFreeMemory (png_pixels);
  *colrs = colors;
  if (spixels)
    return ((unsigned char *)spixels);
  else
    return (pixels);
}

/*----------------------------------------------------------------------
  InitPngColors
  ----------------------------------------------------------------------*/
void InitPngColors ()
{
  int       i;

  /* fills standard color cube */ 
  for (i = 0; i < 127; i++)
    {
      Std_color_cube[i].red   = ((i & 0x3) * 65535/3) >> 8;
      Std_color_cube[i].green = (((i >> 2) & 0x7) * 65535/7) >> 8;
      Std_color_cube[i].blue  = (((i >> 5) & 0x3) * 65535/3) >> 8;
    }
  Std_color_cube[127].red   = 255;
  Std_color_cube[127].green = 255;
  Std_color_cube[127].blue  = 255;
}

/*----------------------------------------------------------------------
  ReadPngToData decompresses and return the main picture info     
  ----------------------------------------------------------------------*/
static unsigned char *ReadPngToData (char *datafile, int *w, int *h,
				      int *ncolors, int *cpp,
				      ThotColorStruct **colrs, int *bg)
{
  unsigned char *bit_data;
  FILE           *fp;
      
#ifndef _WINDOWS  
  fp = ufopen (datafile, "r");
#else  /* _WINDOWS */
  fp = ufopen (datafile, "rb");
#endif /* _WINDOWS */
  if (fp != NULL)
    {
      bit_data = ReadPng (fp, w, h, ncolors, cpp, colrs, bg);
      if (bit_data != NULL)
	{
	  if (fp != stdin) 
	    fclose(fp);
	  return(bit_data);
	}
      if (fp != stdin) 
	fclose(fp);
    }
  return (NULL);
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
Drawable PngCreate (char *fn, PictInfo *imageDesc, int *xif, int *yif,
		    int *wif, int *hif, unsigned long BackGroundPixel,
		    int *width, int *height, int zoom)
{
  Pixmap           pixmap = (Pixmap) 0;
  ThotColorStruct *colrs;
  unsigned char   *buffer = NULL; 
  unsigned char   *buffer2 = NULL;
  int              ncolors, cpp, bg = -1;
  int              w, h, bperpix;

  buffer = ReadPngToData (fn, &w, &h, &ncolors, &cpp, &colrs, &bg);
  if (ncolors > 256)
    bperpix = sizeof (short);
  else
    bperpix = sizeof (char);
  /* return image dimensions */
  *width = w;
  *height = h;
  if (buffer == NULL) 
     return ((Drawable) NULL);

  if (zoom != 0 && *xif == 0 && *yif == 0)
    {
      /* take zoom into account */
      *xif = PixelValue (w, UnPixel, NULL, zoom);
      *yif = PixelValue (h, UnPixel, NULL, zoom);
    }
  else
    {
      if (*xif == 0 && *yif != 0)
	*xif = PixelValue (w, UnPixel, NULL, zoom);
      if (*xif != 0 && *yif == 0)
	*yif = PixelValue (h, UnPixel, NULL, zoom);
    }

#ifndef _WIN_PRINT
  if ((*xif != 0 && *yif != 0) && (w != *xif || h != *yif))
    {
      /* xif and yif contain width and height of the box */
      buffer2 = ZoomPicture (buffer, w , h, *xif, *yif, bperpix);
      TtaFreeMemory (buffer);
      buffer = buffer2;
      buffer2 = NULL;
      w = *xif;
      h = *yif;
    }
#endif /* _WINPRINT */
    
  if (buffer == NULL)
    {
#ifdef _WINDOWS
      WinErrorBox (NULL, "PngCreate: (1)");
#endif /* _WINDOWS */
      return ((Drawable) NULL);
    }

  if (bg >= 0)
    {
#ifdef _WINDOWS
      /* register the transparent color index */
      bg = TtaGetThotColor (colrs[bg].red, colrs[bg].green,
			   colrs[bg].blue);
      imageDesc->PicMask = bg;
#else  /* _WINDOWS */
      /* register the transparent mask */
      imageDesc->PicMask = MakeMask (TtDisplay, buffer, w, h, bg, bperpix);
#endif /* _WINDOWS */
    }

  pixmap = DataToPixmap (buffer, w, h, ncolors, colrs, bperpix);
  TtaFreeMemory (buffer);
  /* free the table of colors */
  TtaFreeMemory (colrs);
  if (pixmap == None)
    {
#ifdef _WINDOWS
      WinErrorBox (NULL, "PngCreate: (2)");
#endif /* _WINDOWS */
      return ((Drawable) NULL); 
    }
  else
    { 
      *wif = w;
      *hif = h;      
      *xif = 0;
      *yif = 0;
      return (Drawable) pixmap;
    }
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void PngPrint (char *fn, PictureScaling pres, int xif, int yif, int wif,
	       int hif, int PicXArea, int PicYArea, int PicWArea,
	       int PicHArea, FILE *fd, unsigned long BackGroundPixel)
{
#ifdef _WINDOWS
  return;
#else  /* _WINDOWS */
  ThotColorStruct *colrs;
  unsigned char   *data;
  unsigned short  *sdata;
  int              delta;
  int              xtmp, ytmp;
  int              x, y, w, h;
  int	           col, ind;
  int              ncolors, cpp, bg = -1;
  
  data = ReadPngToData(fn, &w, &h, &ncolors, &cpp, &colrs, &bg);
  if (!data)
    /* feed the editor with the appropriate message */
       return;
  if (bg != -1 && bg < ncolors)
    {
      colrs[bg].red   = 65535;
      colrs[bg].green = 65535;
      colrs[bg].blue  = 65535;
    }
  
  sdata = (unsigned short  *) data;
  PicWArea = w;
  PicHArea = h;
  xtmp = 0;
  ytmp = 0;
  switch (pres)
    {
    case RealSize:
    case FillFrame:
    case XRepeat:
    case YRepeat:      
      delta = (wif - PicWArea)/2;
      if (delta > 0)
	{
	  xif += delta;
	  wif = PicWArea;
	}
      else
	{
	  xtmp = -delta;
	  PicWArea = wif;
	}     
      delta = (hif - PicHArea)/2;
      if (delta > 0)
	{
	  yif += delta ;
	  hif = PicHArea;
	}
      else
	{

	  ytmp = - delta;
	  PicHArea = hif;
	}   
      fprintf(fd, "gsave %d -%d translate\n", PixelToPoint (xif),
	      PixelToPoint (yif + hif));
      fprintf (fd, "%d %d %d %d DumpImage2\n", PicWArea,
	       PicHArea, PixelToPoint (wif), PixelToPoint (hif));
      break;
    case ReScale:
      fprintf (fd, "gsave %d -%d translate\n", PixelToPoint (xif),
	       PixelToPoint (yif + hif));
      fprintf (fd, "%d %d %d %d DumpImage2\n", PicWArea, PicHArea,
	       PixelToPoint (wif), PixelToPoint (hif));
      wif = PicWArea;
      hif = PicHArea;
      break;
    default:
      break;
    }

  fprintf(fd, "\n");
  for (y = 0 ; y < hif; y++)
    {
      ind = ((ytmp + y) * w) + xtmp;
      for (x = 0 ; x < wif; x++)
	{
	  if (ncolors > 256)
	    /* use two bytes per pixel */
	    col = sdata[ind++];
	  else
	    /* use one byte per pixel */
	    col = data[ind++];
	  fprintf (fd, "%02x%02x%02x",
		   colrs[col].red >> 8,
		   colrs[col].green >> 8,
		   colrs[col].blue >> 8);
	}
      fprintf(fd, "\n");
    }
  fprintf(fd, "\n");
  fprintf(fd, "grestore\n");
  fprintf(fd, "\n");   
  TtaFreeMemory (data);
  /* free the table of colors */
  TtaFreeMemory (colrs);
#endif /* !_WINDOWS */
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
ThotBool IsPngFormat(char *fn)
{
   FILE  *fp;
   char   buf[8];
   int    ret;

   fp = ufopen(fn , "rb");
   if (!fp)
      return 0;
   ret = fread(buf, 1, 8, fp);
   fclose(fp);

   if (ret != 8)
      return FALSE;

   ret = png_check_sig(buf, 8);

   if (ret) return (TRUE);
   return(FALSE);
}
