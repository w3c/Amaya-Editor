/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * Warning:
 * This module is part of the Thot library, which was originally
 * developed in French. That's why some comments are still in
 * French, but their translation is in progress and the full module
 * will be available in English in the next release.
 * 
 */

/*
 * Authors: N. Layaida (INRIA)
 *          R. Guetari (W3C/INRIA) - Unicode and Windows version
 */

#include "thot_gui.h"
#include "thot_sys.h"
#include "png.h"
#include "constmedia.h"
#include "typemedia.h"
#include "picture.h"
#include "frame.h"
#include "message.h"

#define THOT_EXPORT extern
#include "picture_tv.h"

#include "font_f.h"
#include "units_f.h"
#include "frame_tv.h"
#include "picture_f.h"
#include "gifhandler_f.h"
#include "memory_f.h"

#define MAX(x,y)  (((x) > (y)) ? (x) : (y))
#define min(a, b) (((a) < (b)) ? (a) : (b))

#ifdef _WINDOWS
#include "wininclude.h"

extern void png_read_init (png_structp png_ptr);
#endif /* _WINDOWS */

int Magic256[256] =    /* for halftoning */
{
    0, 223, 48, 207, 14, 237, 62, 221, 3, 226, 51, 210, 13, 236, 61, 220,
    175, 80, 128, 96, 189, 94, 141, 110, 178, 83, 130, 99, 188, 93, 140, 109,
    191, 32, 239, 16, 205, 46, 253, 30, 194, 35, 242, 19, 204, 45, 252, 29,
    112, 143, 64, 159, 126, 157, 78, 173, 115, 146, 67, 162, 125, 156, 77, 172,
    11, 234, 59, 218, 5, 228, 53, 212, 8, 231, 56, 215, 6, 229, 54, 213,
    186, 91, 138, 107, 180, 85, 132, 101, 183, 88, 135, 104, 181, 86, 133, 102,
    202, 43, 250, 27, 196, 37, 244, 21, 199, 40, 247, 24, 197, 38, 245, 22,
    123, 154, 75, 170, 117, 148, 69, 164, 120, 151, 72, 167, 118, 149, 70, 165,
    12, 235, 60, 219, 2, 225, 50, 209, 15, 238, 63, 222, 1, 224, 49, 208,
    187, 92, 139, 108, 177, 82, 129, 98, 190, 95, 142, 111, 176, 81, 128, 97,
    203, 44, 251, 28, 193, 34, 241, 18, 206, 47, 254, 31, 192, 33, 240, 17,
    124, 155, 76, 171, 114, 145, 66, 161, 127, 158, 79, 174, 113, 144, 65, 160,
    7, 230, 55, 214, 9, 232, 57, 216, 4, 227, 52, 211, 10, 233, 58, 217,
    182, 87, 134, 103, 184, 89, 136, 105, 179, 84, 131, 100, 185, 90, 137, 106,
    198, 39, 246, 23, 200, 41, 248, 25, 195, 36, 243, 20, 201, 42, 249, 26,
    119, 150, 71, 166, 121, 152, 73, 168, 116, 147, 68, 163, 122, 153, 74, 169

};

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

char* typecouleur[] = {"grayscale", "undefined type", "RGB",
		       "colormap", "grayscale+alpha",
		       "undefined type", "RGB+alpha"};

#ifdef _WINDOWS     
extern BOOL pic2print;
#endif /* _WINDOWS */

#ifdef __STDC__
static void my_png_error (png_struct* png_ptr, STRING message)
#else  /* !__STDC__ */
static void my_png_error (png_ptr, message)
png_struct* png_ptr; 
STRING       message;
#endif /* __STDC__ */
{
   fprintf(stderr,"libpng error: %s\n", message);
   longjmp(png_ptr->jmpbuf, 1);
}

#ifdef __STDC__
static void my_png_warning (png_struct* png_ptr, STRING message)
#else  /* !__STDC__ */
static void my_png_warning (png_ptr, message)
png_struct* png_ptr; 
STRING       message;
#endif /* __STDC__ */
{
   if (!png_ptr)
     return;
   fprintf(stderr,"libpng warning: %s\n", message);
}
 
#ifdef __STDC__
unsigned char *ReadPng (FILE* infile, int* width, int* height, int* ncolors, int* cpp, ThotColorStruct colrs[256], int* bg)

#else /* __STDC__ */
unsigned char *ReadPng (infile, width, height, ncolors, cpp, colrs, bg)
FILE *infile;
int *width; 
int *height; 
int *ncolors; 
int *cpp; 
ThotColorStruct colrs[256];
int *bg;
#endif /* __STDC__ */
{
#ifdef IV
  png_struct     png_ptr;
  png_info       info_ptr;
  png_uint_32 width, height;
  int bit_depth, color_type, interlace_type;

   /* Create and initialize the png_struct with the desired error handler
    * functions.  If you want to use the default stderr and longjump method,
    * you can supply NULL for the last three parameters.  We also supply the
    * the compiler header file version, so that we know if the application
    * was compiled with a compatible version of the library.  REQUIRED
    */
   png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,
      (void *)user_error_ptr, my_png_error, my_png_warning);

   if (png_ptr == NULL)
   {
      fclose(infile);
      return (NULL);
   }

   /* Allocate/initialize the memory for image information.  REQUIRED. */
   info_ptr = png_create_info_struct(png_ptr);
   if (info_ptr == NULL)
   {
      fclose(infile);
      png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
      return (NULL);
   }

   /* Set error handling if you are using the setjmp/longjmp method (this is
    * the normal method of doing things with libpng).  REQUIRED unless you
    * set up your own error handlers in the png_create_read_struct() earlier.
    */
   if (setjmp(png_ptr->jmpbuf))
   {
      /* Free all of the memory associated with the png_ptr and info_ptr */
      png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
      fclose(infile);
      /* If we get here, we had a problem reading the file */
      return (NULL);
   }

   /* One of the following I/O initialization methods is REQUIRED */
#ifdef streams /* PNG file I/O method 1 */
   /* Set up the input control if you are using standard C streams */
   png_init_io(png_ptr, infile);

#else /* no_streams */ /* PNG file I/O method 2 */
   /* If you are using replacement read functions, instead of calling
    * png_init_io() here you would call:
    */
   png_set_read_fn(png_ptr, (void *)user_io_ptr, user_read_fn);
   /* where user_io_ptr is a structure you want available to the callbacks */
#endif /* no_streams */ /* Use only one I/O method! */

   /* If we have already read some of the signature */
   png_set_sig_bytes(png_ptr, sig_read);

   /* The call to png_read_info() gives us all of the information from the
    * PNG file before the first IDAT (image data chunk).  REQUIRED
    */
   png_read_info(png_ptr, info_ptr);

   png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type,
       &interlace_type, NULL, NULL);

/**** Set up the data transformations you want.  Note that these are all
 **** optional.  Only call them if you want/need them.  Many of the
 **** transformations only work on specific types of images, and many
 **** are mutually exclusive.
 ****/

   /* tell libpng to strip 16 bit/color files down to 8 bits/color */
   png_set_strip_16(png_ptr);

   /* Strip alpha bytes from the input data without combining with th
    * background (not recommended).
    */
   png_set_strip_alpha(png_ptr);

   /* Extract multiple pixels with bit depths of 1, 2, and 4 from a single
    * byte into separate bytes (useful for paletted and grayscale images).
    */
   png_set_packing(png_ptr);

   /* Change the order of packed pixels to least significant bit first
    * (not useful if you are using png_set_packing). */
   png_set_packswap(png_ptr);

   /* Expand paletted colors into true RGB triplets */
   if (color_type == PNG_COLOR_TYPE_PALETTE)
      png_set_expand(png_ptr);

   /* Expand grayscale images to the full 8 bits from 1, 2, or 4 bits/pixel */
   if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
      png_set_expand(png_ptr);

   /* Expand paletted or RGB images with transparency to full alpha channels
    * so the data will be available as RGBA quartets.
    */
   if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
      png_set_expand(png_ptr);

   /* Set the background color to draw transparent and alpha images over.
    * It is possible to set the red, green, and blue components directly
    * for paletted images instead of supplying a palette index.  Note that
    * even if the PNG file supplies a background, you are not required to
    * use it - you should use the (solid) application background if it has one.
    */

   png_color_16 my_background, *image_background;

   if (png_get_bKGD(png_ptr, info_ptr, &image_background))
      png_set_background(png_ptr, image_background,
                         PNG_BACKGROUND_GAMMA_FILE, 1, 1.0);
   else
      png_set_background(png_ptr, &my_background,
                         PNG_BACKGROUND_GAMMA_SCREEN, 0, 1.0);

   /* Some suggestions as to how to get a screen gamma value */

   /* Note that screen gamma is (display_gamma/viewing_gamma) */
   if (/* We have a user-defined screen gamma value */)
   {
      screen_gamma = user-defined screen_gamma;
   }
   /* This is one way that applications share the same screen gamma value */
   else if ((gamma_str = getenv("SCREEN_GAMMA")) != NULL)
   {
      screen_gamma = atof(gamma_str);
   }
   /* If we don't have another value */
   else
   {
      screen_gamma = 2.2;  /* A good guess for a PC monitors in a dimly
                              lit room */
      screen_gamma = 1.7 or 1.0;  /* A good guess for Mac systems */
   }

   /* Tell libpng to handle the gamma conversion for you.  The second call
    * is a good guess for PC generated images, but it should be configurable
    * by the user at run time by the user.  It is strongly suggested that
    * your application support gamma correction.
    */

   int intent;

   if (png_get_sRGB(png_ptr, info_ptr, &intent))
      png_set_sRGB(png_ptr, intent, 0);
   else 
      if (png_get_gAMA(png_ptr, info_ptr, &image_gamma))
         png_set_gamma(png_ptr, screen_gamma, image_gamma);
      else
         png_set_gamma(png_ptr, screen_gamma, 0.50);

   /* Dither RGB files down to 8 bit palette or reduce palettes
    * to the number of colors available on your screen.
    */
   if (color_type & PNG_COLOR_MASK_COLOR)
   {
      png_uint_32 num_palette;
      png_colorp palette;

      /* This reduces the image to the application supplied palette */
      if (/* we have our own palette */)
      {
         /* An array of colors to which the image should be dithered */
         png_color std_color_cube[MAX_SCREEN_COLORS];

         png_set_dither(png_ptr, std_color_cube, MAX_SCREEN_COLORS,
            MAX_SCREEN_COLORS, NULL, 0);
      }
      /* This reduces the image to the palette supplied in the file */
      else if (png_get_PLTE(png_ptr, info_ptr, &palette, &num_palette))
      {
         png_color16p histogram;

         png_get_hIST(png_ptr, info_ptr, &histogram);

         png_set_dither(png_ptr, palette, num_palette,
                        max_screen_colors, histogram, 0);
      }
   }

   /* invert monocrome files to have 0 as white and 1 as black */
   png_set_invert_mono(png_ptr);

   /* If you want to shift the pixel values from the range [0,255] or
    * [0,65535] to the original [0,7] or [0,31], or whatever range the
    * colors were originally in:
    */
   if (png_get_valid(png_ptr, info_ptr, PNG_INFO_sBIT))
   {
      png_color8p sig_bit;

      png_get_sBIT(png_ptr, info_ptr, &sig_bit);
      png_set_shift(png_ptr, sig_bit);
   }

   /* flip the RGB pixels to BGR (or RGBA to BGRA) */
   png_set_bgr(png_ptr);

   /* swap the RGBA or GA data to ARGB or AG (or BGRA to ABGR) */
   png_set_swap_alpha(png_ptr);

   /* swap bytes of 16 bit files to least significant byte first */
   png_set_swap(png_ptr);

   /* Add filler (or alpha) byte (before/after each RGB triplet) */
   png_set_filler(png_ptr, 0xff, PNG_FILLER_AFTER);

   /* Turn on interlace handling.  REQUIRED if you are not using
    * png_read_image().  To see how to handle interlacing passes,
    * see the png_read_row() method below:
    */
   number_passes = png_set_interlace_handling(png_ptr);

   /* Optional call to gamma correct and add the background to the palette
    * and update info structure.  REQUIRED if you are expecting libpng to
    * update the palette for you (ie you selected such a transform above).
    */
   png_read_update_info(png_ptr, info_ptr);

   /* Allocate the memory to hold the image using the fields of info_ptr. */

   /* The easiest way to read the image: */
   png_bytep row_pointers[height];

   for (row = 0; row < height; row++)
   {
      row_pointers[row] = malloc(png_get_rowbytes(png_ptr, info_ptr));
   }

   /* Now it's time to read the image.  One of these methods is REQUIRED */
#ifdef entire /* Read the entire image in one go */
   png_read_image(png_ptr, row_pointers);

#else /* no_entire */ /* Read the image one or more scanlines at a time */
   /* The other way to read images - deal with interlacing: */

   for (pass = 0; pass < number_passes; pass++)
   {
#ifdef single /* Read the image a single row at a time */
      for (y = 0; y < height; y++)
      {
         png_bytep row_pointers = row[y];
         png_read_rows(png_ptr, &row_pointers, NULL, 1);
      }

#else /* no_single */  /* Read the image several rows at a time */
      for (y = 0; y < height; y += number_of_rows)
      {
#ifdef sparkle /* Read the image using the "sparkle" effect. */
         png_read_rows(png_ptr, row_pointers, NULL, number_of_rows);
        
#else /* no_sparkle */ /* Read the image using the "rectangle" effect */
         png_read_rows(png_ptr, NULL, row_pointers, number_of_rows);
#endif /* no_sparkle */ /* use only one of these two methods */
      }
     
      /* if you want to display the image after every pass, do
         so here */
#endif /* no_single */ /* use only one of these two methods */
   }
#endif /* no_entire */ /* use only one of these two methods */

   /* read rest of file, and get additional chunks in info_ptr - REQUIRED */
   png_read_end(png_ptr, info_ptr);

   /* clean up after the read, and free any memory allocated - REQUIRED */
   png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);

   /* close the file */
   fclose(infile);

   /* that's it */
   return;

#else /* IV */
  png_byte      *pp;
  png_byte       buf[8];
  png_byte      *dp;
  png_struct    *png_ptr = NULL;
  png_info      *info_ptr = NULL;
  png_byte      *png_pixels = NULL;
  png_byte     **row_pointers = NULL;
  png_color      std_color_cube[256];
  unsigned char *pixels = NULL;
  double         gamma_correction;
  unsigned int   bytesPerExpandedLine;
  int            alpha;
  int            color_type;
  int            ret;
  int            i, j, passes;
  int            xpos, ypos;
  int            xr, xg, xb;

  ret = fread (buf, 1, 8, infile);
  if (ret != 8)
    return NULL;
  
  ret = png_check_sig (buf, 8);	
  if (!ret)
    return(NULL);
  rewind (infile);

    png_ptr = (png_struct*) TtaGetMemory (sizeof (png_struct));
    if (!png_ptr)
      return NULL;

    png_set_error_fn(png_ptr, NULL, (png_voidp)my_png_error, (png_voidp)my_png_warning);
    
    info_ptr = (png_info*) TtaGetMemory (sizeof (png_info));
    if (!info_ptr)
      {
	TtaFreeMemory (png_ptr);
	return NULL;
      }

    if (setjmp (png_ptr->jmpbuf))
      {
	/* sends a message to the user throught thot GUI */ 	
	fprintf(stderr, "ping read image error \n");
        if (png_ptr != NULL)
	  png_read_destroy (png_ptr, info_ptr, (png_info*)0);
	if (png_ptr != NULL)
	  TtaFreeMemory (png_ptr);
	if (info_ptr != NULL)
	  TtaFreeMemory (info_ptr);
	if (row_pointers != NULL)
	  TtaFreeMemory (row_pointers);
	if (pixels != NULL)
	  TtaFreeMemory (pixels);
	if (png_pixels != NULL)
	  TtaFreeMemory (png_pixels);
	return NULL;
      }

    /* initialize the structures, info first for error handling */
    png_info_init (info_ptr);
    png_read_init (png_ptr);   
    /* set up the input control for C streams*/
    png_init_io (png_ptr, infile);
    
    /* read the file information */
    png_read_info (png_ptr, info_ptr);
    
    /* 
       printf(" Picture information  %ld x %ld, %d-bit, %s, %s\n",
       info_ptr->width, info_ptr->height,
       info_ptr->bit_depth, 
       (info_ptr->color_type>6) ? typecouleur[1] : typecouleur[info_ptr->color_type],
       info_ptr->interlace_type ? "entrelace" : "non-entrelace");
       */
    
    /* expand color components to 8 bit rgb */
    png_set_expand (png_ptr);
    
    /* tell libpng to handle the gamma conversion for you */
    if (info_ptr->valid & PNG_INFO_gAMA) 
      gamma_correction = info_ptr->gamma;
    else 
      gamma_correction = (double)0.45;

    if (fabs ((2.2 * gamma_correction) - 1.0) > 0.011)
      png_set_gamma (png_ptr, (double) 2.2, gamma_correction);
    
    /*
      if (info_ptr->valid & PNG_INFO_bKGD)
      png_set_background (png_ptr, &(info_ptr->background), PNG_BACKGROUND_GAMMA_FILE, 1, gamma_correction);
     else 
     {   
     color_background.index = 0;
     color_background.red = 65535 >> (16 - info_ptr->bit_depth);
     color_background.green = 65535 >> (16 - info_ptr->bit_depth);
     color_background.blue = 65535 >> (16 - info_ptr->bit_depth);
     color_background.gray = 65535 >> (16 - info_ptr->bit_depth);
     png_set_background(png_ptr, &color_background, 
     PNG_BACKGROUND_GAMMA_SCREEN, 0, 1.0);
     }
     */
    
    /* strip 16-bit channels down to 8 */ 
    if (info_ptr->bit_depth == 16)
      png_set_strip_16 (png_ptr);


    passes = png_set_interlace_handling (png_ptr);
    

    /* fills standard color cube */ 
    /* should be done once at the  beginning ! */
    /* clearly writing my thesis makes me a bit lazy :) Nabil */

    
    for (i=0;i<127;i++)
      {
	std_color_cube[i].red   = ((i & 0x3) * 65535/3) >> 8;
	std_color_cube[i].green = (((i >> 2) & 0x7) * 65535/7) >> 8;
	std_color_cube[i].blue  = (((i >> 5) & 0x3) * 65535/3) >> 8;
      }

    std_color_cube[127].red   = 255;
    std_color_cube[127].green = 255;
    std_color_cube[127].blue  = 255;
    
    /*   if (info_ptr->color_type & PNG_COLOR_MASK_COLOR) {*/
    if (info_ptr->valid & PNG_INFO_PLTE)
      {
	png_set_dither (png_ptr, info_ptr->palette, info_ptr->num_palette, 256, info_ptr->hist, 1); 
	/*printf(" Picture Paletted : number of colors %d \n", info_ptr->num_palette);*/
      } 
    
    png_read_update_info (png_ptr, info_ptr);
    /* setup other stuff using the fields of png_info. */
    *width  = (int) png_ptr->width;
    *height = (int) png_ptr->height;

    /*
      printf(" Picture information  %ld x %ld, %d-bit, %s, %s\n",
      info_ptr->width, info_ptr->height,
      info_ptr->bit_depth, 
      (info_ptr->color_type>6) ? typecouleur[1] : typecouleur[info_ptr->color_type],
      info_ptr->interlace_type ? "entrelace" : "non-entrelace");
      */
    
    /* calculate the bytes per line (if the pixels were expanded to 1 pixel/byte */ 
    bytesPerExpandedLine = (*width) * info_ptr->channels;
    png_pixels = (png_byte*) TtaGetMemory (bytesPerExpandedLine * (*height) * sizeof (png_byte));
    if (png_pixels == NULL) 
      png_error (png_ptr,"not enough memory ");
    
    row_pointers = (png_bytep*) TtaGetMemory ((*height) * sizeof(png_bytep /**/));
    
    if (row_pointers == NULL)
      png_error (png_ptr, "not enough memory ");

    for (i=0; i < *height; i++)
      row_pointers[i] = png_pixels + (bytesPerExpandedLine * i);    
    
    /*   png_read_image(png_ptr, row_pointers);*/
    color_type = info_ptr->color_type;
    alpha = color_type & PNG_COLOR_MASK_ALPHA;
    color_type &= ~PNG_COLOR_MASK_ALPHA;
    
    if (color_type == PNG_COLOR_TYPE_PALETTE)
      {
	*ncolors = info_ptr->num_palette; 	
	for (i=0; i < *ncolors; i++)
	  {
#ifndef _WINDOWS
	    colrs[i].red   = info_ptr->palette[i].red << 8;
	    colrs[i].green = info_ptr->palette[i].green << 8;
	    colrs[i].blue  = info_ptr->palette[i].blue << 8;
	    colrs[i].pixel = i;
#ifndef _GTK
	    colrs[i].flags = DoRed|DoGreen|DoBlue;
#endif /* ! _GTK */
#else /* _WINDOWS */
	    colrs[i].red   = info_ptr->palette[i].red;
	    colrs[i].green = info_ptr->palette[i].green;
	    colrs[i].blue  = info_ptr->palette[i].blue;
#endif /* _WINDOWS */
	  }
      }
    else if (color_type == PNG_COLOR_TYPE_RGB )
      {
	*ncolors = 128; 
	for (i=0; i < *ncolors ; i++) {
#          ifdef _WINDOWS 
	  colrs[i].red   = std_color_cube[i].red;
	  colrs[i].green = std_color_cube[i].green;
	  colrs[i].blue  = std_color_cube[i].blue;
#          else  /* !_WINDOWS */
	  colrs[i].red   = std_color_cube[i].red << 8;
	  colrs[i].green = std_color_cube[i].green << 8;
	  colrs[i].blue  = std_color_cube[i].blue << 8;
	  colrs[i].pixel = i;
#ifndef _GTK
	  colrs[i].flags = DoRed|DoGreen|DoBlue;
#endif /* ! _GTK */

#              endif /* _WINDOWS */
	}       
      }
    else if (color_type == PNG_COLOR_TYPE_GRAY)
      {
	/* greymap */
	*ncolors = 16; 
	for (i=0; i < 15; i++ )
	  {
	    colrs[i].red   = colrs[i].green = colrs[i].blue = i * 65535/15;
#ifndef _GTK
#              ifndef _WINDOWS 
	    colrs[i].flags = DoRed|DoGreen|DoBlue;
#              endif /* _WINDOWS */ 
#endif /* ! _GTK */

	  }
#          ifdef _WINDOWS
	colrs[15].red = colrs[15].green = colrs[15].blue = 255;
#          else /* !_WINDOWS */
	colrs[15].red = colrs[15].green = colrs[15].blue = 65535;
#ifndef _GTK
	colrs[15].flags = DoRed|DoGreen|DoBlue;
#endif /* ! _GTK */

#          endif /* _WINDOWS */
      }
    else
      {
	fprintf (stderr, "ping read image error \n");
	png_error (png_ptr, "Unknown PNG color type ");
      }

    png_read_update_info (png_ptr, info_ptr);
    png_start_read_image (png_ptr);
    for (i = 0; i < passes; i++)
      for (j = 0; j < *height; j++)
	png_read_row (png_ptr, NULL, row_pointers[j]);

    pixels = (char*) TtaGetMemory ((*width) * (*height));
    if (pixels == NULL)
      png_error (png_ptr, "not enough memory ");
    
    dp = pixels; 
    pp = png_pixels;
    color_type = info_ptr->color_type;
    alpha      = color_type & PNG_COLOR_MASK_ALPHA;
    color_type &= ~PNG_COLOR_MASK_ALPHA;
    if (info_ptr->valid & PNG_INFO_tRNS)
      {
	switch (color_type)
	  { 
	  case  PNG_COLOR_TYPE_PALETTE:
	    *bg = (info_ptr->background.index);
	    break;
	  case  PNG_COLOR_TYPE_GRAY:
	    *bg = ((info_ptr->trans_values.gray)& 0xff) >> 4;
	    break;
	  case PNG_COLOR_TYPE_RGB:
	    xr = info_ptr->trans_values.red;
	    xg = info_ptr->trans_values.green;
	    xb = info_ptr->trans_values.blue;
	    xr = xr & 0xff; xg = xg & 0xff; xb = xb & 0xff;
	    xr = min(xr, 255) & 0xC0;
	    xg = min(xg, 255) & 0xE0;
	    xb = min(xb, 255) & 0xC0;
	    *bg = (unsigned char) ((xr >> 6) | (xg >> 3) | (xb >> 1));
	    break;
	  default:
	    *bg = -1;
	  }    
      }
    
    if (color_type == PNG_COLOR_TYPE_PALETTE)
      {
	if (alpha)
	  {
	    /* j'en ai pas vu encore des comme-ca mais on ne sait jamais */
	    for (i=0; i<*height; i++)
	      {
		pp = row_pointers[i];
		for (j=0; j<*width; j++)
		  {
		    *dp++ = *pp++; 
		    *pp = *pp + 1 ;
		  }
	      }
	  }
	else
	  {	
	    for (i=0; i<*height; i++)
	      {
		pp = row_pointers[i];
		for (j=0; j<*width; j++) 
		  *dp++ = *pp++; 
	      }
	  }  
      }
    else
      {     	    
	for (ypos = 0; ypos < *height; ypos ++)
	  {
	    int col = ypos & 0x0f;
	    pp = row_pointers[ypos];
	    for (xpos = 0; xpos < *width; xpos ++)
	      {
		int row = xpos & 0x0f;
		int cr=0, cg=0, cb=0, cgr=0, a=0;
		int gr;
		int isgrey = 0;
		    
		if (color_type == PNG_COLOR_TYPE_GRAY)
		  {
		    cr=cg=cb=cgr=(*pp++);
		    isgrey=1;
		  }
		else if (color_type == PNG_COLOR_TYPE_RGB)
		  {		
		    cr= (*pp++);
		    cg= (*pp++);
		    cb= (*pp++);
		  }
		else
		  {
		    fprintf (stderr, "ping read image error \n");
		    png_error (png_ptr, "Unknown PNG color type ");
		  }
			
		/* the alpha channel is not yet handled :) */
		if (alpha)
		  { 	      
		    a=(*pp++);
		    /*if(a!=0xff) {*/
		    cr  = (int) ((a/255.0) * cr  + ((255.0-a)/255.0) * 211.0);
		    cg  = (int) ((a/255.0) * cg  + ((255.0-a)/255.0) * 211.0);
		    cb  = (int) ((a/255.0) * cb  + ((255.0-a)/255.0) * 211.0);
		    cgr = (int) ((a/255.0) * cgr + ((255.0-a)/255.0) * 211.0);
		    /*}*/
		  }
   
		if (isgrey)
		  {
		    gr = cgr & 0xF0;
		    if (cgr - gr > Magic16[(row << 4) + col])
		      gr += 16;
		    gr = min(gr, 0xF0);
		    *dp++ = gr >> 4;
		  }
		else
		  {    
		    int r = cr & 0xC0;
		    int g = cg & 0xE0;
		    int b = cb & 0xC0;
		    int v = (row << 4) + col;
		    if (cr - r > Magic64[v])
		      r += 64;
		    if (cg - g > Magic32[v])
		      g += 32;
		    if (cb - b > Magic64[v])
		      b += 64;
		    r = min(r, 255) & 0xC0;
		    g = min(g, 255) & 0xE0;
		    b = min(b, 255) & 0xC0;
		    *dp++ = (unsigned char) ((r >> 6) | (g >> 3) | (b >> 1));
		  }
	      }
	  }
      }

    /* clean up after the read, and free any memory allocated */
    png_read_destroy (png_ptr, info_ptr, (png_info*)0);
    /* free the structures */
    if (row_pointers != (unsigned char**) NULL)
      TtaFreeMemory ( row_pointers);
    if (png_pixels!= (unsigned char*) NULL)
      TtaFreeMemory ( png_pixels);
    TtaFreeMemory ( png_ptr);
    TtaFreeMemory ( info_ptr);
    return (pixels);
#endif /* IV */
}


/*----------------------------------------------------------------------
   	ReadPngToData decompresses and return the main picture info     
  ----------------------------------------------------------------------*/
#ifdef __STDC__
unsigned char* ReadPngToData (STRING datafile, int* w, int* h, int* ncolors, int* cpp, ThotColorStruct colrs[256], int* bg)
#else /* __STDC__ */
unsigned char* ReadPngToData (datafile, w, h, ncolors, cpp, colrs, bg)
STRING           datafile;
int*            w;
int*            h;
int*            ncolors;
int*            cpp;
ThotColorStruct colrs[256];
int*            bg;
#endif /* __STDC__ */
{
     unsigned char* bit_data;
     FILE*          fp;
      
#    ifndef _WINDOWS  
     fp = ufopen (datafile, "r");
#    else  /* _WINDOWS */
     fp = ufopen (datafile, _RBinaryMODE_);
#    endif /* _WINDOWS */
     if (fp != NULL) {
	bit_data = ReadPng (fp, w, h, ncolors, cpp, colrs, bg);
	if (bit_data != NULL) {
	   if (fp != stdin) 
	      fclose(fp);
	   return(bit_data);
	}
	
	if (fp != stdin) 
	   fclose(fp);
     }
     return ((unsigned char*) NULL);

}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBitmap PngCreate (STRING fn, PictInfo *imageDesc, int* xif, int* yif, int* wif, int* hif, unsigned long BackGroundPixel, ThotBitmap *mask1, int *width, int *height, int zoom)
#else /* __STDC__ */
ThotBitmap PngCreate (fn, imageDesc, xif, yif, wif, hif, BackGroundPixel, mask1, width, height, zoom)
STRING         fn;
PictInfo      *imageDesc;
int*           xif;
int*           yif;
int*           wif;
int*           hif;
unsigned long  BackGroundPixel;
ThotBitmap    *mask1;
int           *width;
int           *height;
int            zoom;
#endif /* __STDC__ */
{
  int             w, h;
  Pixmap          pixmap = (Pixmap) 0;
  ThotColorStruct colrs[256];
  unsigned char   *buffer = (unsigned char*)0; 
# ifndef _WIN_PRINT
  unsigned char* buffer2;
# endif /* _WIN_PRINT */
  int             ncolors, cpp, bg = -1;

# ifdef _WINDOWS
  bgRed   = -1;
  bgGreen = -1;
  bgBlue  = -1;
# endif /* _WINDOWS */

  buffer = ReadPngToData (fn, &w, &h, &ncolors, &cpp, colrs, &bg);

  /* return image dimensions */
  *width = w;
  *height = h;
  if (buffer == NULL) 
     return (ThotBitmapNone);

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
  if ((*xif != 0 && *yif != 0) && (w != *xif || h != *yif)) {
    /* xif and yif contain width and height of the box */
    buffer2 = ZoomPicture (buffer, w , h, *xif, *yif, 1);
    TtaFreeMemory (buffer);
    buffer = buffer2;
    buffer2 = NULL;
    w = *xif;
    h = *yif;
  }
#endif /* _WINPRINT */
    
  if (buffer == NULL) {
#     ifdef _WINDOWS
      WinErrorBox (NULL, "PngCreate: (1)");
#     endif /* _WINDOWS */
    return (ThotBitmapNone);
  }

  if (bg >= 0) {
#   ifndef _WINDOWS
    *mask1 = MakeMask (TtDisplay, buffer, w, h, bg);
#   else  /* _WINDOWS */
    bgRed   = colrs[bg].red;
    bgGreen = colrs[bg].green;
    bgBlue  = colrs[bg].blue;
#   endif /* _WINDOWS */
  }

  pixmap = DataToPixmap (buffer, w, h, ncolors, colrs, &(imageDesc->PicColors));
  if (imageDesc->PicColors != NULL)
    imageDesc->PicNbColors = ncolors;
  TtaFreeMemory (buffer);
  if (pixmap == None) {
#    ifdef _WINDOWS
     WinErrorBox (NULL, "PngCreate: (2)");
#    endif /* _WINDOWS */
    return (ThotBitmapNone); 
  } else
    { 
      *wif = w;
      *hif = h;      
      *xif = 0;
      *yif = 0;
      return (ThotBitmap) pixmap;
    }
}





/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void PngPrint (STRING fn, PictureScaling pres, int xif, int yif, int wif, int hif, int PicXArea, int PicYArea, int PicWArea, int PicHArea, int fd, unsigned long BackGroundPixel)
#else /* __STDC__ */
void PngPrint (fn, pres, xif, yif, wif, hif, PicXArea, PicYArea, PicWArea, PicHArea, fd, BackGroundPixel)
STRING          fn;
PictureScaling pres;
int            xif;
int            yif;
int            wif;
int            hif;
int            PicXArea;
int            PicYArea;
int            PicWArea;
int            PicHArea;
int            fd;
unsigned long  BackGroundPixel;
#endif /* __STDC__ */
{
#ifdef _WINDOWS
  return;
#else  /* _WINDOWS */

  int      delta;
  int      xtmp, ytmp;
  unsigned char *pt;
  int       x, y, w, h;
  int	   wim /*, him*/ ;
  int      ncolors, cpp, bg;
  unsigned int NbCharPerLine;
 
  ThotColorStruct colrs[256];
  unsigned char *buffer;

  
  buffer = ReadPngToData(fn, &w, &h, &ncolors, &cpp, colrs, &bg );

   
  if (!buffer)
    {
    /* feed the editor with the appropriate message */
       return ;
    }
  
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
      fprintf((FILE *)fd, "gsave %d -%d translate\n", PixelToPoint (xif), PixelToPoint (yif + hif));
      fprintf ((FILE *)fd, "%d %d %d %d DumpImage2\n", PicWArea, PicHArea, PixelToPoint (wif), PixelToPoint (hif));
      break;
    case ReScale:
      fprintf ((FILE *)fd, "gsave %d -%d translate\n", PixelToPoint (xif), PixelToPoint (yif + hif));
      fprintf ((FILE *)fd, "%d %d %d %d DumpImage2\n", PicWArea, PicHArea, PixelToPoint (wif), PixelToPoint (hif));
      wif = PicWArea;
      hif = PicHArea;
      break;
    default:
      break;
    }

  wim = w;
  fprintf((FILE *)fd, "\n"); 
  NbCharPerLine = wim ;
  
  for (y = 0 ; y < hif; y++)
    {
      pt = (unsigned char *) (buffer + ( (ytmp + y ) * NbCharPerLine ) + xtmp) ;
  
      for (x = 0 ; x < wif; x++)
	{
	  fprintf((FILE *)fd, "%02x%02x%02x",
		  (colrs[*pt].red)>>8,
		  (colrs[*pt].green)>>8,
		  (colrs[*pt].blue)>>8 );       

	  pt++;
	}
      fprintf((FILE *)fd, "\n");
    }
	    
  fprintf((FILE *)fd, "\n");
  fprintf((FILE *)fd, "grestore\n");
  fprintf((FILE *)fd, "\n");   
  TtaFreeMemory (buffer);
#endif /* !_WINDOWS */
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool IsPngFormat(char* fn)
#else /* __STDC__ */
ThotBool IsPngFormat(fn)
char*   fn;
#endif /* __STDC__ */
{
  
   FILE *fp;
   char buf[8];
   int ret;

   fp = fopen(fn , "rb");
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

