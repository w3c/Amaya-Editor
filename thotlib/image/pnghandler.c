/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2009
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * Authors: I. Vatton, N. Layaida (INRIA)
 *          R. Guetari (W3C/INRIA) - Initial Windows version
 */

#include "thot_gui.h"
#include "thot_sys.h"
#include "png.h"
#include "constmedia.h"
#include "typemedia.h"
#include "picture.h"
#include "frame.h"
#include "message.h"

#ifdef _WINGUI
  #include "wininclude.h"
#endif /* _WINGUI */

#define THOT_EXPORT extern
#include "edit_tv.h"
#include "picture_tv.h"

#include "font_f.h"
#include "frame_tv.h"
#include "gifhandler_f.h"
#include "inites_f.h"
#include "memory_f.h"
#include "picture_f.h"
#include "units_f.h"

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
#ifdef _WINGUI     
extern int              PngTransparentColor;
#endif /* _WINGUI */
#ifdef _GTK
extern ThotPixmap MakeMask (Display *dsp, unsigned char *pixels, int w, int h,
		 unsigned int bg, int bperpix);
#endif /* _GTK */

#ifdef _GL
/*----------------------------------------------------------------------
  ReadPng : reads from a file pointer a png file into a RGBA buffer
  All png specifications is supported : http://www.w3.org/Graphics/PNG/
  lib manual http://www.libpng.org/pub/png/libpng-manual.txt
  ----------------------------------------------------------------------*/
static unsigned char *ReadPng (FILE *pfFile, unsigned int *width, unsigned int *height, 
			       int *ncolors, int *cpp, ThotColorStruct **colrs,
			       int *bg, ThotBool *withAlpha, ThotBool *grayScale)
{
  png_structp     png_ptr;
  png_infop       info_ptr;
  png_byte        pbSig[8];
  png_uint_32     ulChannels;
  png_uint_32     ulRowBytes;
  png_byte      **ppbRowPointers;
  unsigned char  *pixels;
  unsigned int    i, j, passes;
  unsigned long   lw, lh;
  int             iBitDepth, iColorType;
  double          dGamma;

    /* Checks the eight byte PNG signature*/
    fread (pbSig, 1, 8, pfFile);
    if (!png_check_sig (pbSig, 8))
        return NULL;	
    /* create the two png(-info) structures*/
    png_ptr = png_create_read_struct (PNG_LIBPNG_VER_STRING, 
				      NULL, 
				      (png_error_ptr) NULL, 
				      (png_error_ptr) NULL);
    if (!png_ptr)
      return NULL;	
    info_ptr = png_create_info_struct (png_ptr);
    if (!info_ptr)
      {
        png_destroy_read_struct (&png_ptr, NULL, NULL);
        return NULL;
      }   
    if (setjmp (png_ptr->jmpbuf))
      {
        /* Free all of the memory associated with the png_ptr and info_ptr */
        png_destroy_read_struct (&png_ptr, &info_ptr, (png_infopp)NULL);
        return NULL;
      }
    png_init_io (png_ptr, pfFile);
    png_set_sig_bytes (png_ptr, 8);
    png_read_info (png_ptr, info_ptr);
    png_get_IHDR(png_ptr, info_ptr, &lw, &lh, 
		 &iBitDepth,
		 &iColorType, NULL, NULL, NULL);	
   /* if less than 8 bits /channels => 8 bits / channels*/
    if (iBitDepth < 8)
      png_set_packing (png_ptr);
    /* 16 bits /channels => 8 bits / channels*/
    if (iBitDepth == 16)
      png_set_strip_16 (png_ptr);
    /* Grayscale =>RGB or RGBA */
    if (iColorType == PNG_COLOR_TYPE_GRAY || iColorType == PNG_COLOR_TYPE_GRAY_ALPHA)
      png_set_gray_to_rgb (png_ptr);
    /* Palette indexed colors to RGB */
    if (iColorType == PNG_COLOR_TYPE_PALETTE)
      png_set_palette_to_rgb (png_ptr);
    /* 8 bits / channel is needed */
    if (iColorType == PNG_COLOR_TYPE_GRAY && iBitDepth < 8) 
      png_set_gray_1_2_4_to_8(png_ptr);
    /* all transparency type : 1 color, indexed => alpha channel*/
    if (png_get_valid (png_ptr, info_ptr,PNG_INFO_tRNS)) 
      png_set_tRNS_to_alpha (png_ptr);
    /* RGB => RGBA*/
    if (iColorType != PNG_COLOR_TYPE_RGBA)
      png_set_filler (png_ptr, 0xff, PNG_FILLER_AFTER);
    /* Gives us the good number of passes to make : 
       if not interlaced -> 1 passes else often 7 passes*/
    passes = png_set_interlace_handling (png_ptr);
    /* Gamma settings 
       2.2 => A good guess for a  PC monitor in a bright office or a dim room 
       2.0 => A good guess for a PC monitor in a dark room 
       1.7 => A good guess for Mac systems    
    */
    if (png_get_gAMA (png_ptr, info_ptr, &dGamma))
      png_set_gamma (png_ptr, (double) 2.2, dGamma);
    /* Update the png in order to reach ou out pixels sprcification*/
    png_read_update_info (png_ptr, info_ptr);
    /* get again width, height and the new bit-depth and color-type*/
    png_get_IHDR (png_ptr, info_ptr, &lw, &lh, 
		  &iBitDepth, 
		  &iColorType, 
		  NULL, NULL, NULL);
    /* row_bytes is the width x number of channels => the length of a line */
    ulRowBytes = png_get_rowbytes (png_ptr, info_ptr);
    ulChannels = png_get_channels (png_ptr, info_ptr);
    pixels = (png_byte *) TtaGetMemory (ulRowBytes * lh * sizeof(png_byte));
    /* Row pointers give a pointer on each line */
    ppbRowPointers = (png_bytepp) TtaGetMemory  (lh * sizeof(png_bytep));
    /* Opengl Texture inversion */   
    for (i = 0; i < lh; i++)
      ppbRowPointers[i] = pixels + ((lh - (i+1)) * ulRowBytes * sizeof(png_byte));    
    png_start_read_image (png_ptr); 
    /* depending on interlacing, reading the data*/
    for (i = 0; i < passes; i++)
      for (j = 0; j < lh; j++)
        png_read_row (png_ptr, ppbRowPointers[j], NULL);
    png_read_end(png_ptr, NULL);
    TtaFreeMemory (ppbRowPointers);
    ppbRowPointers = NULL; 
    /* clean up after the read, and free any memory allocated */
    //png_read_destroy (png_ptr, info_ptr, (png_info*) NULL);
    /* Free all of the memory associated with the png_ptr and info_ptr */
    png_destroy_read_struct (&png_ptr, &info_ptr, (png_infopp)NULL);
    *width = (unsigned int)lw;
    *height = (unsigned int)lh;
    return pixels;
}

#else /* _GL */

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
static unsigned char *ReadPng (FILE *infile, int *width, int *height,
			       int *ncolors, int *cpp, ThotColorStruct **colrs,
			       int *bg, ThotBool *withAlpha, ThotBool *grayScale)
{
  png_byte        *pp;
  png_byte         buf[8];
  png_struct      *png_ptr = NULL;
  png_info        *info_ptr = NULL;
  png_byte        *png_pixels;
  png_byte       **row_pointers;
  ThotColorStruct *colors;
  unsigned char   *pixels;
  unsigned short  *spixels = NULL;
  double           gamma_correction;
  unsigned int     bytesPerExpandedLine;
  int              alpha;
  int              color_type;
  int              ret, gr;
  int              i, j, passes;
  int              xpos, ypos, ind;
  int              xr, xg, xb;
  int              r, g, b, v, a;
  int              bit_depth, interlace_type;
  int              cr, cg, cb, cgr;
  int              row, col;
  ThotBool         isgrey;

  *withAlpha = FALSE;
  *grayScale = FALSE;
  *colrs = NULL;
  *ncolors = 0;

  /* Checks the eight byte PNG signature*/
  ret = fread (buf, 1, 8, infile);
  if (ret != 8)
    return NULL;
  ret = png_check_sig (buf, 8);	
  if (!ret)
    return NULL;
  rewind (infile);

  /* create the two png(-info) structures*/
  png_ptr = png_create_read_struct (PNG_LIBPNG_VER_STRING, (png_voidp)PError,
				    (png_error_ptr)PError, (png_error_ptr)PWarning);
  if (png_ptr == NULL)
    return NULL;

  /* Allocate/initialize the memory for image information.  REQUIRED. */
  info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == NULL)
    {
      png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
      return NULL;
    }
  png_pixels = NULL;
  row_pointers = NULL;
  colors = NULL;
  pixels = NULL;
  isgrey = FALSE;
  cr = cg = cb = 0;
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
    


  color_type = info_ptr->color_type;
  alpha = color_type & PNG_COLOR_MASK_ALPHA;
  color_type &= ~PNG_COLOR_MASK_ALPHA;
  if (color_type == PNG_COLOR_TYPE_PALETTE)
    {
      if (info_ptr->num_palette > 256)
	*ncolors = 256;
      else
	*ncolors = info_ptr->num_palette;
      colors = (ThotColorStruct *)TtaGetMemory ((*ncolors) * sizeof (ThotColorStruct));
      for (i = 0; i < *ncolors; i++)
	{
#ifdef _WINGUI
	  colors[i].red   = info_ptr->palette[i].red;
	  colors[i].green = info_ptr->palette[i].green;
	  colors[i].blue  = info_ptr->palette[i].blue;
#endif /* _WINGUI */
#if defined(_GTK) || defined(_WX)
	  colors[i].red   = info_ptr->palette[i].red << 8;
	  colors[i].green = info_ptr->palette[i].green << 8;
	  colors[i].blue  = info_ptr->palette[i].blue << 8;
	  colors[i].pixel = i;
#endif /* #if defined(_GTK) || defined(_WX) */
	}
    }
  else if (color_type == PNG_COLOR_TYPE_RGB)
    {
      if (TtWDepth > 8)
	{
	  /* Generate the image palette */
	  colors = (ThotColorStruct *)TtaGetMemory (512 * sizeof (ThotColorStruct));
	  *ncolors = 0; 
	}
      else
	{
	  /* Use the standard palette */
	  colors = (ThotColorStruct *)TtaGetMemory (128 * sizeof (ThotColorStruct));
	  *ncolors = 128;
	  for (i = 0; i < *ncolors ; i++)
	    {
#ifdef _WINGUI
	      colors[i].red = Std_color_cube[i].red;
	      colors[i].green = Std_color_cube[i].green;
	      colors[i].blue = Std_color_cube[i].blue;
#endif /* _WINGUI */
        
#if defined(_GTK) || defined(_WX)    
	      colors[i].red = Std_color_cube[i].red << 8;
	      colors[i].green = Std_color_cube[i].green << 8;
	      colors[i].blue = Std_color_cube[i].blue << 8;
#endif /* #if defined(_GTK) || defined(_WX) */
	    }
	}
    }
  else if (color_type == PNG_COLOR_TYPE_GRAY)
    {
      isgrey = TRUE;
     if (TtWDepth > 8)
	{
	  /* Generate the image palette */
	  colors = (ThotColorStruct *)TtaGetMemory (512 * sizeof (ThotColorStruct));
	  *grayScale = TRUE;
	  *ncolors = 0;
	}
      else
	{
	  /* Use the standard palette */
	  colors = (ThotColorStruct *)TtaGetMemory (16 * sizeof (ThotColorStruct));
	  *grayScale = TRUE;
	  *ncolors = 16; 
	  for (i = 0; i <= 15; i++)
	    {
#if defined(_WINGUI)
	      colors[i].red = colors[i].green = colors[i].blue = i;
#endif /* _WINGUI */
#if defined(_GTK) || defined(_WX)
	      colors[i].red = colors[i].green = colors[i].blue = i << 4; 
              /* this is 4 and not 8 because the palette should be between 0 and 255 */
#endif /* #if defined(_GTK) || defined(_WX) */
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
  pixels = (unsigned char *) TtaGetMemory ((*width) * (*height) * sizeof (char));
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
      if (info_ptr->valid & PNG_INFO_tRNS &&
	  /* degree of transparency */
	  info_ptr->trans[info_ptr->background.index] < 127)
	*bg = (info_ptr->background.index);
      break;

    case PNG_COLOR_TYPE_RGB:
    case  PNG_COLOR_TYPE_GRAY:
      if (TtWDepth > 8)
	{
	  /* True color -> Keep the image descriptor as it is */
	  TtaFreeMemory (pixels);
	  pixels = png_pixels;
	  png_pixels = NULL;
	  TtaFreeMemory (colors);
	  colors = NULL;
	  *withAlpha = (alpha != 0);
	}
      else
	{
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
		  if (isgrey)
		    cr = cg = cb = cgr = (*pp++);
		  else if (color_type == PNG_COLOR_TYPE_RGB)
		    {		
		      cr = (*pp++);
		      cg = (*pp++);
		      cb = (*pp++);
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
   
		  if (isgrey)
		    {
		      /* Use the palette of 16 colors */
		      gr = cgr & 0xF0;
		      if (cgr - gr > Magic16[(row << 4) + col])
			gr += 16;
		      gr = min(gr, 0xF0);
		      pixels[ind++] = gr >> 4; /* this is the color index in the palette (16 colors) */
		    }
		  else
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
#endif /* _GL */
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
				     ThotColorStruct **colrs, int *bg,
				     ThotBool *withAlpha, ThotBool *grayScale)
{
  unsigned char *bit_data;
  FILE           *fp;
      
  fp = TtaReadOpen (datafile);
  if (fp != NULL)
    {
#ifdef _GL
      bit_data = ReadPng (fp, (unsigned int *)w, (unsigned int *)h, ncolors,
			  cpp, colrs, bg, withAlpha,
			  grayScale);
#else /* #ifdef _GL */
      bit_data = ReadPng (fp, w, h, ncolors, cpp, colrs, bg, withAlpha,
			  grayScale);      
#endif /* #ifdef _GL */      
      if (bit_data != NULL)
	{
	  if (fp != stdin) 
	    TtaReadClose (fp);
	  return(bit_data);
	}
      if (fp != stdin) 
	TtaReadClose (fp);
    }
  return (NULL);
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
ThotDrawable PngCreate (char *fn, ThotPictInfo *imageDesc, int *b_w, int *b_h,
		    int *wif, int *hif, int bgColor, int *width,
		    int *height, int zoom)
{
#ifndef _GL
	ThotPixmap pixmap = (ThotPixmap) NULL;
#endif

	ThotColorStruct *colrs = NULL;
#if defined (_WINGUI) && !defined (_GL)
  unsigned short   red, green, blue;
#endif /* _WINGUI */
  unsigned char   *buffer = NULL; 
#ifndef _GL
  unsigned char   *buffer2 = NULL;
#endif /*_GL*/
  int              ncolors = 1, cpp, bg = -1;
  int              w, h, bperpix;
  ThotBool         withAlpha, grayScale;

  buffer = ReadPngToData (fn, &w, &h, &ncolors, &cpp, &colrs, &bg,
			  &withAlpha, &grayScale);
  if (ncolors == 0)
    {
      /* one byte per component RGB */
      bperpix = 3;
      if (withAlpha)
      /* one more byte */
      bperpix += 1;
    }
  else
    bperpix = 1;
  /* return image dimensions */
  *width = w;
  *height = h;
  if (buffer == NULL) 
     return ((ThotDrawable) NULL);

  if (zoom != 0 && *b_w == 0 && *b_h == 0)
    {
      /* take zoom into account */
      *b_w = PixelValue (w, UnPixel, NULL, zoom);
      *b_h = PixelValue (h, UnPixel, NULL, zoom);
    }
  else
    {
      if (*b_w == 0 && *b_h != 0)
	*b_w = PixelValue (w, UnPixel, NULL, zoom);
      if (*b_w != 0 && *b_h == 0)
	*b_h = PixelValue (h, UnPixel, NULL, zoom);
    }

#ifndef _WIN_PRINT
#ifndef _GL
  if ((*b_w != 0 && *b_h != 0) && (w != *b_w || h != *b_h))
    {
      /* b_w and b_h contain width and height of the box */
      buffer2 = ZoomPicture (buffer, w , h, *b_w, *b_h, bperpix);
      TtaFreeMemory (buffer);
      buffer = buffer2;
      buffer2 = NULL;
      w = *b_w;
      h = *b_h;
    }
#endif /*_GL*/
#endif /* _WIN_PRINT */
    
  if (buffer == NULL)
    {
#ifdef _WINGUI
      WinErrorBox (NULL, "PngCreate: (1)");
#endif /* _WINGUI */
      return ((ThotDrawable) NULL);
    }

#ifdef _GL
  /* GL buffer are display independant, 
  and already in the good format RGB, or RGBA*/
  /* free the table of colors */
  TtaFreeMemory (colrs);
  *wif = w;
  *hif = h;
  *b_w = 0;
  *b_h = 0;
  return (ThotDrawable) buffer;
#else /* _GL */
  if (bg >= 0 && colrs)
    {
#ifdef _WINGUI
      if (Printing)
        {
          TtaGiveThotRGB (bgColor, &red, &green, &blue);
          colrs[bg].red   = (unsigned char) red;
          colrs[bg].green = (unsigned char) green;
          colrs[bg].blue  = (unsigned char) blue;
          colrs[bg].pixel = ColorPixel (bgColor);
          bg = bgColor;
        }
      else
        /* register the transparent color index */
        bg = TtaGetThotColor (colrs[bg].red, colrs[bg].green, colrs[bg].blue);
      imageDesc->PicBgMask = bg;
#endif /* _WINGUI */
    }
  pixmap = DataToPixmap (buffer, w, h, ncolors, colrs, withAlpha, grayScale);
  TtaFreeMemory (buffer);
#ifdef _WINGUI
  if (withAlpha && bg == -1 && PngTransparentColor != -1)
    imageDesc->PicBgMask = PngTransparentColor;
#endif /* _WINGUI */
  /* free the table of colors */
  TtaFreeMemory (colrs);
  if (pixmap != None)
    { 
      *wif = w;
      *hif = h;      
      *b_w = 0;
      *b_h = 0;
    }
  return (ThotDrawable) pixmap;
#endif /*_GL*/
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void PngPrint (char *fn, PictureScaling pres, int b_w, int b_h, int wif,
	       int hif, FILE *fd, int bgColor)
{
#ifdef _GTK
  ThotColorStruct *colrs;
  unsigned char   *data;
  int              picW, picH;
  int              ncolors, cpp, transparent;
  ThotBool         withAlpha, grayScale;

  transparent = -1;
  data = ReadPngToData (fn, &picW, &picH, &ncolors, &cpp, &colrs, &transparent,
			&withAlpha, &grayScale);
  if (data)
    DataToPrint (data, pres, b_w, b_h, wif, hif, picW, picH, fd, ncolors,
		 transparent, bgColor, colrs, withAlpha, grayScale);
  TtaFreeMemory (data);
  /* free the table of colors */
  TtaFreeMemory (colrs);
#endif /* _GTK */
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
ThotBool IsPngFormat(char *fn)
{
   FILE  *fp;
   char   buf[8];
   int    ret;

   fp = TtaReadOpen (fn);
   if (!fp)
      return 0;
   ret = fread (buf, 1, 8, fp);
   TtaReadClose (fp);
   if (ret != 8)
      return FALSE;
   ret = png_check_sig ((png_byte*)buf, 8);
   if (ret) return (TRUE);
   return(FALSE);
}

/*----------------------------------------------------------------------
  SavePng : Wth Alpha, compression in 32 bit
----------------------------------------------------------------------*/
ThotBool SavePng (const char *filename, 
		 unsigned char *m_pData,
		 unsigned int m_width,
		 unsigned int m_height)
{
  png_structp                png = 0;
  png_infop                  pngInfo = 0;
  FILE                       *pngFile;
  unsigned char              **rowPtrs;
  register int      rowcount, linesize, imagesize;

  if (!m_pData)
    return FALSE; 
  png = png_create_write_struct (PNG_LIBPNG_VER_STRING, 0, 0, 0);
  if (!png)
    return FALSE;
  pngInfo = png_create_info_struct (png);
  if (!pngInfo)
    {
      png_destroy_write_struct(&png, (png_infopp) NULL);
      return FALSE;
    }
  pngFile = TtaWriteOpen (filename);
  if (!(pngFile))
    {
      png_destroy_write_struct(&png, (png_infopp) NULL);
      return FALSE;
    }
  if (setjmp(png->jmpbuf)) 
    {
        png_destroy_write_struct(&png, &pngInfo);
        TtaWriteClose (pngFile);
        return FALSE;
    }
  png_init_io (png, pngFile);
  /* set the zlib compression level */
  png_set_compression_level(png, Z_BEST_COMPRESSION);
  /* set other zlib parameters */
  png_set_compression_mem_level(png, 8);
  png_set_compression_strategy(png, Z_DEFAULT_STRATEGY);
  png_set_compression_window_bits(png, 15);
  png_set_compression_method(png, 8);

  png_set_IHDR (png, pngInfo, 
		(int) m_width, (int) m_height, 8, 
		PNG_COLOR_TYPE_RGB_ALPHA, 
		PNG_INTERLACE_NONE, 
		PNG_COMPRESSION_TYPE_DEFAULT,
		PNG_FILTER_TYPE_DEFAULT);
  /* png_set_invert_alpha(png_ptr);*/
  png_write_info (png, pngInfo);  
  rowPtrs = (unsigned char**)malloc (sizeof(unsigned char *) * m_height);
  if (!rowPtrs)
    {
      TtaWriteClose (pngFile);
      png_destroy_read_struct (&png, &pngInfo, 0);
      return FALSE;
    }
  rowcount = ((int) m_height) - 1;
  linesize = ((int) m_width) * 4;
  imagesize = 0;
  /* invert image order*/
  while (rowcount >= 0)
    {
      rowPtrs[rowcount] = (unsigned char *) m_pData + imagesize;
      imagesize += linesize;
      rowcount--;
    }  
  png_write_image (png, rowPtrs);
  png_write_end (png, pngInfo);
  png_write_flush (png);  
  free (rowPtrs);  
  png_destroy_read_struct (&png, &pngInfo, 0);  
  TtaWriteClose (pngFile);
  return TRUE;
}

