/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2001
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 *
 * Authors: I. Vatton, N. Layaida (INRIA)
 *          R. Guetari (W3C/INRIA) - Unicode and Windows version
 */

#include "thot_gui.h"
#include "thot_sys.h"
#include <setjmp.h>
#include "constmedia.h"
#include "typemedia.h"
#include "picture.h"
#include "frame.h"
#include "message.h"
#include "application.h"
#include "jconfig.h"

/*#include "jinclude.h"*/
#include "jpeglib.h"

#ifdef _WINDOWS
#include "wininclude.h"
#endif /* _WINDOWS */

#define THOT_EXPORT extern
#include "picture_tv.h"
#include "frame_tv.h"
#include "picture_f.h"
#include "font_f.h"
#include "units_f.h"
#include "gifhandler_f.h"

struct my_error_mgr
{
  struct jpeg_error_mgr pub;	           /* "public" fields */
  jmp_buf               setjmp_buffer;  /* for return to caller */
};
typedef struct my_error_mgr *my_error_ptr;

struct jpeg_decompress_struct cinfo;
struct my_error_mgr           jerr;


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
static void my_error_exit (j_common_ptr cinfo)
{
    my_error_ptr        myerr = (my_error_ptr) cinfo->err;

    longjmp (myerr->setjmp_buffer, 1);
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
static unsigned char *ReadJPEG (FILE* infile, int* width, int* height,
				ThotColorStruct colrs[256])
{
  unsigned char      *retBuffer = NULL;/* Output image buffer */
  unsigned char      *r;
  JSAMPROW            buffer[1];    /* row pointer array for read_scanlines */
  int                 row_stride;   /* physical row width in output buffer */
  int                 i, ret;

  /* We set up the normal JPEG error routines, 
     then override error_exit.     */
  cinfo.err = jpeg_std_error (&jerr.pub);
  jerr.pub.error_exit = my_error_exit;

  /* Establish the setjmp return context for my_error_exit to use. */
  if (setjmp (jerr.setjmp_buffer))
    {
      /* If we get here, the JPEG code has signaled an error. */
      jpeg_destroy_decompress (&cinfo);
      fclose (infile);
      if (retBuffer)
	TtaFreeMemory (retBuffer);
      return NULL;
    }

  jpeg_create_decompress (&cinfo);
  jpeg_stdio_src (&cinfo, infile);
  ret = jpeg_read_header (&cinfo, TRUE);
  /* We can ignore the return value from jpeg_read_header since
   *   (a) suspension is not possible with the stdio data source, and
   *   (b) we passed TRUE to reject a tables-only JPEG file as an error.
   * See libjpeg.doc for more info.
   */
  cinfo.quantize_colors = TRUE;
  /*cinfo.desired_number_of_colors = value defined by thot */
  /* Waiting for a good policy *** to be discussed with Vincent  */
  cinfo.desired_number_of_colors = 100;
  cinfo.two_pass_quantize = TRUE;
  jpeg_start_decompress (&cinfo);
  i = cinfo.output_width * cinfo.output_height * cinfo.output_components;
  if (!(retBuffer = (unsigned char*) TtaGetMemory (i)))
    {
      jpeg_destroy_decompress (&cinfo);
      fprintf (stderr, "Couldn't create space for JPEG read\n");
      return (NULL);
    }

  r = retBuffer;
  row_stride = cinfo.output_width * cinfo.output_components;
  while (cinfo.output_scanline < cinfo.output_height)
    {
      buffer[0] = r;
      (void) jpeg_read_scanlines (&cinfo, buffer, 1);
      r += row_stride;
    }

  *width = cinfo.output_width;
  *height = cinfo.output_height;
  /* Initialize our colormap until a clear policy for the 32-bit screen */
  if (ret != JPEG_HEADER_TABLES_ONLY)
    {
      if (cinfo.out_color_components == 3)
	{
	  for (i = 0; i < cinfo.actual_number_of_colors; i++)
	    {
#ifndef _WINDOWS
	      colrs[i].red = cinfo.colormap[0][i] << 8;
	      colrs[i].green = cinfo.colormap[1][i] << 8;
	      colrs[i].blue = cinfo.colormap[2][i] << 8;
	      colrs[i].pixel = i;
#ifndef _GTK
	      colrs[i].flags = DoRed | DoGreen | DoBlue;
#endif /* ! _GTK */
#else /* _WINDOWS */
	      colrs[i].red = cinfo.colormap[0][i];
	      colrs[i].green = cinfo.colormap[1][i];
	      colrs[i].blue = cinfo.colormap[2][i];
#endif /* _WINDOWS */
	    }
	}
      else
	{
	  for (i = 0; i < cinfo.actual_number_of_colors; i++)
	    {
#ifndef _WINDOWS
	      colrs[i].red = colrs[i].green = colrs[i].blue = cinfo.colormap[0][i] << 8;
	      colrs[i].pixel = i;
#ifndef _GTK
	      colrs[i].flags = DoRed | DoGreen | DoBlue;
#endif /* ! _GTK */
#else /* _WINDOWS */
	      colrs[i].red = colrs[i].green = colrs[i].blue = cinfo.colormap[0][i];
#endif /* _WINDOWS */
	    }
	}
    }

  (void) jpeg_finish_decompress (&cinfo);
  jpeg_destroy_decompress (&cinfo);
  return retBuffer;
}

/*----------------------------------------------------------------------
   ReadJpegToData  Just open the file and pass it to the ReadJpeg     
  ----------------------------------------------------------------------*/
static unsigned char *ReadJpegToData (char *datafile, int * w, int* h,
				      ThotColorStruct colrs[256])
{
  unsigned char      *bit_data;
  FILE               *fp;
  
  fp = fopen (datafile, "rb");
  if (fp != NULL)
    {
      bit_data = (unsigned char *) ReadJPEG (fp, w, h, colrs);
      if (bit_data != NULL)
	{
	  if (fp != stdin)
	    fclose (fp);
	  return (bit_data);
	}
      if (fp != stdin)
	fclose (fp);
    }
  return ( NULL);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void JpegPrintErrorMsg (int ErrorNumber)
{
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
Drawable JpegCreate (char *fn, PictInfo *imageDesc, int *xif, int *yif,
		     int *wif, int *hif, int bgColor, int *width,
		     int *height, int zoom)
{
  int                 w, h;
  Pixmap              pixmap = (Pixmap) NULL;
  ThotColorStruct     colrs[256];
  unsigned char      *data = NULL, *data2 = NULL;

  /* effective load of the Picture from Jpeg Library */
  data = ReadJpegToData (fn, &w, &h, colrs);
  /* return image dimensions */
  *width = w;
  *height = h;
  if (data == NULL)
    {
#ifdef _WINDOWS 
      WinErrorBox (NULL, "JpegCreate(1)");
#endif /* _WINDOWS */
      return ((Drawable) NULL);
    }

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
      data2 = ZoomPicture (data, w , h, *xif, *yif, 1);
      TtaFreeMemory (data);
      data = data2;
      data2 = NULL;
      w = *xif;
      h = *yif;
    }
#endif /* _WIN_PRINT */

  if (data == NULL)
    return ((Drawable) NULL);

  pixmap = DataToPixmap (data, w, h, 100, colrs, -1);
  TtaFreeMemory (data);  
  if (pixmap == None)
    {
#ifdef _WINDOWS
      WinErrorBox (NULL, "JpegCreate(2)");
#endif /* _WINDOWS */
      return ((Drawable) NULL);
    }
  else
    {
      *wif = w;
      *hif = h;
      *xif = 0;
      *yif = 0;
      return ((Drawable) pixmap);
    }
}


/*----------------------------------------------------------------------
   JpegPrint produces postscript from a jpeg picture file
  ----------------------------------------------------------------------*/
void JpegPrint (char *fn, PictureScaling pres, int xif, int yif, int wif,
		int hif, FILE *fd, int bgColor)
{
#ifndef _WINDOWS
  ThotColorStruct     colrs[256];
  unsigned char      *data;
  int                 picW, picH;

  data = ReadJpegToData (fn, &picW, &picH, colrs);
  if (!data)
    DataToPrint (data, pres, xif, yif, wif, hif, picW, picH, fd, 100, -1,
		 bgColor, colrs);
  TtaFreeMemory (data);
#endif /* _WINDOWS */
}


/*----------------------------------------------------------------------
   IsJpegFormat checks if the file header conforms the jpeg one    
  ----------------------------------------------------------------------*/
ThotBool IsJpegFormat (char *fn)
{ 
   FILE               *fd;

   if ((fd = fopen (fn, "rb")) == NULL)
     {
	fprintf (stderr, "can't open %s\n", fn);
	return FALSE;
     }

   /* We set up the normal JPEG error routines, 
      then override error_exit.     */
   cinfo.err = jpeg_std_error (&jerr.pub);
   jerr.pub.error_exit = my_error_exit;

   /* Establish the setjmp return context for my_error_exit to use. */
   if (setjmp (jerr.setjmp_buffer))
     {
	/* If we get here, the JPEG code has signaled an error. */
	jpeg_destroy_decompress (&cinfo);
	fclose (fd);
	return FALSE;
     }
   jpeg_create_decompress (&cinfo);
   jpeg_stdio_src (&cinfo, fd);
   (void) jpeg_read_header (&cinfo, TRUE);
   jpeg_destroy_decompress (&cinfo);
   fclose (fd);

   return TRUE;
}
