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
 * Author: N. Layaida (INRIA)
 *         R. Guetari (W3C/INRIA) Windows 95/NT routines
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

#define MAX(x,y)  (((x) > (y)) ? (x) : (y))
#define min(a, b) (((a) < (b)) ? (a) : (b))

#ifdef _WINDOWS
extern int bgRed ;
extern int bgGreen;
extern int bgBlue ;
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

char *typecouleur[] = {"grayscale", "undefined type", "RGB",
		       "colormap", "grayscale+alpha",
		       "undefined type", "RGB+alpha"};
    

#ifdef __STDC__
void png_error (png_struct* png_ptr, char* message)
#else  /* !__STDC__ */
void png_error (png_ptr, message)
png_struct* png_ptr; 
char*       message;
#endif /* __STDC__ */
{
   fprintf(stderr,"libpng error: %s\n", message);
   longjmp(png_ptr->jmpbuf, 1);
}

#ifdef __STDC__
void png_warning (png_struct* png_ptr, char* message)
#else  /* !__STDC__ */
void png_warning (png_ptr, message)
png_struct* png_ptr; 
char*       message;
#endif /* __STDC__ */
{
   if (!png_ptr) return;
   fprintf(stderr,"libpng warning: %s\n", message);
}
 
#ifdef __STDC__
unsigned char *ReadPng (FILE* infile, int* width, int* height, int* ncolors, int* cpp, ThotColorStruct colrs[256], int* bg)

#else /* __STDC__ */
unsigned char *ReadPng (infile,width,height, ncolors, cpp, colrs, bg)
FILE *infile;
int *width; 
int *height; 
int *ncolors; 
int *cpp; 
ThotColorStruct colrs[256];
int *bg;
#endif /* __STDC__ */
{

    png_byte  *pp;
    int alpha;
    int color_type;
    int ret;
    png_byte buf[8];
    unsigned char *pixels=NULL;
    png_byte *dp;
    png_struct *png_ptr=NULL;
    png_info *info_ptr=NULL;
    png_byte *png_pixels=NULL;
    png_byte **row_pointers=NULL;
    double gamma_correction;
    int i, j, passes;
    int xpos, ypos;
    unsigned int bytesPerExpandedLine;
    png_color std_color_cube[256];
    int xr, xg, xb;


    ret = fread (buf, 1, 8, infile);
	
    if (ret != 8) return NULL;
	
    ret = png_check_sig (buf, 8);
	
    if (!ret) return(NULL);

    rewind (infile);

    png_ptr = (png_struct*) TtaGetMemory (sizeof (png_struct));
    if (!png_ptr)
       return NULL;

    info_ptr = (png_info*) TtaGetMemory (sizeof (png_info));
    if (!info_ptr) {
       TtaFreeMemory (png_ptr);
       return NULL;
    }

    if (setjmp (png_ptr->jmpbuf)) {
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

    /*    printf(" Picture information  %ld x %ld, %d-bit, %s, %s\n",
	    info_ptr->width, info_ptr->height,
	    info_ptr->bit_depth, 
	    (info_ptr->color_type>6) ? typecouleur[1] : typecouleur[info_ptr->color_type],
	    info_ptr->interlace_type ? "entrelace" : "non-entrelace"); */
    

       /* expand color components to 8 bit rgb */
    png_set_expand (png_ptr);

    /* tell libpng to handle the gamma conversion for you */
    
    if (info_ptr->valid & PNG_INFO_gAMA) 
       gamma_correction = info_ptr->gamma;
    else 
	gamma_correction = (double)0.45;

    if (abs ((2.2 * gamma_correction) - 1) > 0.011)
       png_set_gamma (png_ptr, (double) 2.2, gamma_correction);
    
    /*    if (info_ptr->valid & PNG_INFO_bKGD)
       png_set_background (png_ptr, &(info_ptr->background), PNG_BACKGROUND_GAMMA_FILE, 1, gamma_correction);*/
     /* else 
	     {   
	         color_background.index = 0;
		 color_background.red = 65535 >> (16 - info_ptr->bit_depth);
		 color_background.green = 65535 >> (16 - info_ptr->bit_depth);
		 color_background.blue = 65535 >> (16 - info_ptr->bit_depth);
		 color_background.gray = 65535 >> (16 - info_ptr->bit_depth);

		 png_set_background(png_ptr, &color_background, 
				    PNG_BACKGROUND_GAMMA_SCREEN, 0, 1.0);
	     }*/
    
    
    /* strip 16-bit channels down to 8 */ 
    
    if (info_ptr->bit_depth == 16)
	png_set_strip_16 (png_ptr);


    passes = png_set_interlace_handling (png_ptr);
    

    /* fills standard color cube */ 
    /* should be done once at the  beginning ! */
    /* clearly writing my thesis makes me a bit lazy :) Nabil */

    
    for (i=0;i<127;i++) {
	std_color_cube[i].red   = ((i & 0x3) * 65535/3) >> 8;
	std_color_cube[i].green = (((i >> 2) & 0x7) * 65535/7) >> 8;
	std_color_cube[i].blue  = (((i >> 5) & 0x3) * 65535/3) >> 8;
    }

    std_color_cube[127].red   = 255;
    std_color_cube[127].green = 255;
    std_color_cube[127].blue  = 255;

    

    
    /*   if (info_ptr->color_type & PNG_COLOR_MASK_COLOR) {*/
	
    if (info_ptr->valid & PNG_INFO_PLTE) {
       png_set_dither (png_ptr, info_ptr->palette, info_ptr->num_palette, 256, info_ptr->hist, 1); 
	/*printf(" Picture Paletted : number of colors %d \n", info_ptr->num_palette);*/
    } 
    
    png_read_update_info (png_ptr, info_ptr);
    
    /* setup other stuff using the fields of png_info. */
    
    *width  = (int) png_ptr->width;
    *height = (int) png_ptr->height;

    /* printf(" Picture information  %ld x %ld, %d-bit, %s, %s\n",
	    info_ptr->width, info_ptr->height,
	    info_ptr->bit_depth, 
	    (info_ptr->color_type>6) ? typecouleur[1] : typecouleur[info_ptr->color_type],
	    info_ptr->interlace_type ? "entrelace" : "non-entrelace");*/

    /* calculate the bytes per line (if the pixels were expanded to 1 pixel/byte */ 
        
    bytesPerExpandedLine = (*width) * info_ptr->channels;
    
    png_pixels = (png_byte*) TtaGetMemory (bytesPerExpandedLine * (*height) * sizeof (png_byte));

    if (png_pixels == NULL) 
       png_error (png_ptr,"not enough memory ");
    
    row_pointers = (png_bytep*) TtaGetMemory ((*height) * sizeof(png_bytep /**/));
    
    if (row_pointers == NULL) png_error (png_ptr, "not enough memory ");

    for (i=0; i < *height; i++)
	row_pointers[i] = png_pixels + (bytesPerExpandedLine * i);    
    
    /*   png_read_image(png_ptr, row_pointers);*/

    color_type = info_ptr->color_type;
    alpha = color_type & PNG_COLOR_MASK_ALPHA;
    color_type &= ~PNG_COLOR_MASK_ALPHA;
    
    if (color_type == PNG_COLOR_TYPE_PALETTE) {
       *ncolors = info_ptr->num_palette; 	
       for (i=0; i < *ncolors; i++) {
#          ifndef _WINDOWS
           colrs[i].red   = info_ptr->palette[i].red << 8;
           colrs[i].green = info_ptr->palette[i].green << 8;
           colrs[i].blue  = info_ptr->palette[i].blue << 8;
           colrs[i].pixel = i;
           colrs[i].flags = DoRed|DoGreen|DoBlue;
#          else /* _WINDOWS */
           colrs[i].red   = info_ptr->palette[i].red;
           colrs[i].green = info_ptr->palette[i].green;
           colrs[i].blue  = info_ptr->palette[i].blue;
#          endif /* _WINDOWS */
	}
    } else if (color_type == PNG_COLOR_TYPE_RGB ) {
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
	       colrs[i].flags = DoRed|DoGreen|DoBlue;
#              endif /* _WINDOWS */
	}       
    } else if (color_type == PNG_COLOR_TYPE_GRAY) {
	   /* greymap */
	   *ncolors = 16; 
           for (i=0; i < 15; i++ ) {
	       colrs[i].red   = colrs[i].green = colrs[i].blue = i * 65535/15;
#              ifndef _WINDOWS
	       colrs[i].flags = DoRed|DoGreen|DoBlue;
#              endif /* _WINDOWS */
	   }
#          ifdef _WINDOWS
	   colrs[15].red = colrs[15].green = colrs[15].blue = 255;
#          else /* !_WINDOWS */
	   colrs[15].red = colrs[15].green = colrs[15].blue = 65535;
	   colrs[15].flags = DoRed|DoGreen|DoBlue;
#          endif /* _WINDOWS */
    } else {
	   fprintf (stderr, "ping read image error \n");
	   png_error (png_ptr, "Unknown PNG color type ");
    }

    png_read_update_info (png_ptr, info_ptr);
    png_start_read_image (png_ptr);
    
    for (i = 0; i < passes; i++)
	for (j = 0; j < *height; j++)
	    png_read_row (png_ptr, NULL, row_pointers[j]);

    pixels = (char*) TtaGetMemory ((*width) * (*height));

    if (pixels == NULL) png_error (png_ptr, "not enough memory ");
    
    dp = pixels; 
    pp = png_pixels;

    color_type = info_ptr->color_type;
    alpha      = color_type & PNG_COLOR_MASK_ALPHA;
    color_type &= ~PNG_COLOR_MASK_ALPHA;

    if (info_ptr->valid & PNG_INFO_tRNS) {
       switch (color_type) { 
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
    
    if (color_type == PNG_COLOR_TYPE_PALETTE) {
       if (alpha) {
	  /* j'en ai pas vu encore des comme-ca mais on ne sait jamais */
	  for (i=0; i<*height; i++) {
	      pp = row_pointers[i];
	      for (j=0; j<*width; j++) {
		  *dp++ = *pp++; 
		  *pp = *pp + 1 ;
	      }
	  }
       } else {	
	      for (i=0; i<*height; i++) {
		  pp = row_pointers[i];
		  for (j=0; j<*width; j++) 
		      *dp++ = *pp++; 
	      }
       }  
    } else {     	    
	    for (ypos = 0; ypos < *height; ypos ++) {
		int col = ypos & 0x0f;
		pp = row_pointers[ypos];
		for (xpos = 0; xpos < *width; xpos ++) {
		    int row = xpos & 0x0f;
		    int cr=0, cg=0, cb=0, cgr=0, a=0;
		    int gr;
		    int isgrey = 0;
		    
		    if (color_type == PNG_COLOR_TYPE_GRAY) {
		       cr=cg=cb=cgr=(*pp++);
		       isgrey=1;

		    } else if (color_type == PNG_COLOR_TYPE_RGB) {		
			   cr= (*pp++);
			   cg= (*pp++);
			   cb= (*pp++);
		    } else {
			   fprintf (stderr, "ping read image error \n");
			   png_error (png_ptr, "Unknown PNG color type ");
		    }
			
		    /* the alpha channel is not yet handled :) */
			
		    if (alpha) { 	      
		       a=(*pp++);
		       /*if(a!=0xff) {*/
		       cr  = (a/255.0) * cr  + ((255.0-a)/255.0) * 211;
		       cg  = (a/255.0) * cg  + ((255.0-a)/255.0) * 211;
		       cb  = (a/255.0) * cb  + ((255.0-a)/255.0) * 211;
		       cgr = (a/255.0) * cgr + ((255.0-a)/255.0) * 211;
		       /*}*/
		    }
   
		    if (isgrey) {
		       gr = cgr & 0xF0;
		       if (cgr - gr > Magic16[(row << 4) + col])
			  gr += 16;
		       gr = min(gr, 0xF0);
		       *dp++ = gr >> 4;
		       
		    } else {    
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
    
    return pixels;
}


/*----------------------------------------------------------------------
   	ReadPngToData decompresses and return the main picture info     
  ----------------------------------------------------------------------*/
#ifdef __STDC__
unsigned char* ReadPngToData (char* datafile, int* w, int* h, int* ncolors, int* cpp, ThotColorStruct colrs[256], int* bg)
#else /* __STDC__ */
unsigned char* ReadPngToData (datafile, w, h, ncolors, cpp, colrs, bg)
char*           datafile;
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
     fp = fopen (datafile, "r");
#    else  /* _WINDOWS */
     fp = fopen (datafile, "rb");
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
ThotBitmap PngCreate (char* fn, PictureScaling pres, int* xif, int* yif, int* wif, int* hif, unsigned long BackGroundPixel, ThotBitmap *mask1, int *width, int *height)
#else /* __STDC__ */
ThotBitmap PngCreate (fn, pres, xif, yif, wif, hif, BackGroundPixel, mask1, width, height)
char*          fn;
PictureScaling pres;
int*           xif;
int*           yif;
int*           wif;
int*           hif;
unsigned long  BackGroundPixel;
ThotBitmap    *mask1;
int                *width;
int                *height;
#endif /* __STDC__ */
{
  int             w, h;
  Pixmap          pixmap;
  ThotColorStruct colrs[256];
  unsigned char   *buffer, *buffer2;
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

  if (*xif == 0 && *yif != 0)
    *xif = w;
  if (*xif != 0 && *yif == 0)
    *yif = h;
  if ((*xif != 0 && *yif != 0) && (w != *xif || h != *yif)) {
    /* xif and yif contain width and height of the box */
    buffer2 = ZoomPicture (buffer, w , h, *xif, *yif, 1);
    TtaFreeMemory (buffer);
    buffer = buffer2;
    buffer2 = NULL;
    w = *xif;
    h = *yif;
  }
    
  if (buffer == NULL)
    return (ThotBitmapNone);
  if (bg >= 0) {
#   ifndef _WINDOWS
    *mask1 = MakeMask (TtDisplay, buffer, w, h, bg);
#   else  /* _WINDOWS */
    bgRed   = colrs[bg].red;
    bgGreen = colrs[bg].green;
    bgBlue  = colrs[bg].blue;
#   endif /* _WINDOWS */
  }

  pixmap = DataToPixmap (buffer, w, h, ncolors,  colrs);
  TtaFreeMemory (buffer);
  if (pixmap == None)
    return (ThotBitmapNone); 
  else
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
void PngPrint (char* fn, PictureScaling pres, int xif, int yif, int wif, int hif, int PicXArea, int PicYArea, int PicWArea, int PicHArea, int fd, unsigned long BackGroundPixel)
#else /* __STDC__ */
void PngPrint (fn, pres, xif, yif, wif, hif, PicXArea, PicYArea, PicWArea, PicHArea, fd, BackGroundPixel)
char*          fn;
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
boolean IsPngFormat(char * fn)
#else /* __STDC__ */
boolean IsPngFormat(fn)
	char * fn;
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

