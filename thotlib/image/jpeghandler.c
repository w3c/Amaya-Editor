
/* +-------------------------------------------------------------------+ */
/* |  Driver Jpeg: LAYAIDA Nabil 19-12-95                                                                             | */
/* |               Copyright INRIA Rhone-Alpes                                                                             | */
/* |               Jpeg format.                                                                                                       | */
/* +-------------------------------------------------------------------+ */

#include "thot_gui.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "jpeglib.h"

#define HAVE_BOOLEAN
#include "typemedia.h"
#include "picture.h"
#include "frame.h"
#include "message.h"
#include "application.h"


#define EXPORT extern
#include "picture_tv.h"
#include "frame_tv.h"
#include "picture_f.h"


struct my_error_mgr
  {
     struct jpeg_error_mgr pub;	/* "public" fields */
     jmp_buf             setjmp_buffer;		/* for return to caller */
  };

typedef struct my_error_mgr *my_error_ptr;

METHODDEF void
                    my_error_exit (j_common_ptr cinfo)
{
   my_error_ptr        myerr = (my_error_ptr) cinfo->err;

#if 0
   fprintf (stderr, "Error reading JPEG image: ");
   (*cinfo->err->output_message) (cinfo);
#endif
   longjmp (myerr->setjmp_buffer, 1);
}

struct jpeg_decompress_struct cinfo;
struct my_error_mgr jerr;

#ifdef __STDC__
extern Pixmap       DataToPixmap (char *, int, int, int, ThotColorStruct[]);
extern int          PixelEnPt (int, int);

#else  /* __STDC__ */
extern Pixmap       DataToPixmap ();
extern int          PixelEnPt ();

#endif /* __STDC__ */

/* ---------------------------------------------------------------------- */
/* | lit un fichier Jpeg et le rend en forme normalizee                                                                    | */
/* ---------------------------------------------------------------------- */
unsigned char      *ReadJPEG (FILE * infile, int *width, int *height, ThotColorStruct colrs[256])
{
   unsigned char      *retBuffer = 0;	/* Output image buffer */
   unsigned char      *r;
   JSAMPROW            buffer[1];	/* row pointer array for read_scanlines */
   int                 row_stride;	/* physical row width in output buffer */
   int                 i;

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
	  {
	     TtaFreeMemory (retBuffer);
	  }
	return 0;
     }

   jpeg_create_decompress (&cinfo);

   jpeg_stdio_src (&cinfo, infile);

   (void) jpeg_read_header (&cinfo, TRUE);

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

   if (!(retBuffer = (unsigned char *) TtaGetMemory (cinfo.output_width
			  * cinfo.output_height * cinfo.output_components)))
     {
	jpeg_destroy_decompress (&cinfo);
	fprintf (stderr, "Couldn't create space for JPEG read\n");
	return (0);
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

#ifndef NEW_WILLOWS
   if (cinfo.out_color_components == 3)
     {
	for (i = 0; i < cinfo.actual_number_of_colors; i++)
	  {
	     colrs[i].red = cinfo.colormap[0][i] << 8;
	     colrs[i].green = cinfo.colormap[1][i] << 8;
	     colrs[i].blue = cinfo.colormap[2][i] << 8;
	     colrs[i].pixel = i;
	     colrs[i].flags = DoRed | DoGreen | DoBlue;
	  }
     }
   else
     {
	for (i = 0; i < cinfo.actual_number_of_colors; i++)
	  {
	     colrs[i].red = colrs[i].green =
		colrs[i].blue = cinfo.colormap[0][i] << 8;
	     colrs[i].pixel = i;
	     colrs[i].flags = DoRed | DoGreen | DoBlue;
	  }
     }
#endif /* NEW_WILLOWS */


   (void) jpeg_finish_decompress (&cinfo);
   jpeg_destroy_decompress (&cinfo);

   return retBuffer;
}

/* ---------------------------------------------------------------------- */
/* |    ReadJpegToData  Just open the file and pass it to the   ReadJpeg Main Routine          | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
unsigned char      *ReadJpegToData (char *datafile, int *w, int *h, ThotColorStruct colrs[256])

#else  /* __STDC__ */
unsigned char      *ReadJpegToData (datafile, w, h, colrs)
char               *datafile;
int                *w;
int                *h;
ThotColorStruct     colrs[256];

#endif /* __STDC__ */
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
   return ((unsigned char *) NULL);
}

/* ---------------------------------------------------------------------- */
/* |    Messages d'erreur : On recupere les erreurs de Xpm et on envoi  | */
/* |            vers le frame Thot Dialogue                             | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                JpegPrintErrorMsg (int ErrorNumber)
#else  /* __STDC__ */
void                JpegPrintErrorMsg (ErrorNumber)
int                 ErrorNumber;
#endif /* __STDC__ */
{
}


/* ---------------------------------------------------------------------- */
/* |    JpegCreate lit et retourne le Jpeg lu dans le fichier      | */
/* |            fn. Met a` jour xif, yif, wif, hif.                     | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
ThotBitmap          JpegCreate (char *fn, PictureScaling pres, int *xif, int *yif, int *wif, int *hif, unsigned long BackGroundPixel, Drawable * mask1)
#else  /* __STDC__ */
ThotBitmap          JpegCreate (fn, pres, xif, yif, wif, hif, BackGroundPixel, mask1)
char               *fn;
PictureScaling           pres;
int                *xif;
int                *yif;
int                *wif;
int                *hif;
unsigned long       BackGroundPixel;
ThotBitmap         *mask1;
#endif /* __STDC__ */
{
   int                 w, h;
   Pixmap              pixmap;
   ThotColorStruct     colrs[256];
   unsigned char      *buffer;

   /* ffective load of the Picture from Jpeg Library */

   buffer = ReadJpegToData (fn, &w, &h, colrs);

#ifndef NEW_WILLOWS
   pixmap = DataToPixmap (buffer, w, h, 100, colrs);
#endif /* NEW_WILLOWS */

   free (buffer);

   if (pixmap == None)
     {
	return ThotBitmapNone;	/* Problems loading the jpeg File */
     }
   else
     {
	*wif = w;
	*hif = h;

	*xif = 0;
	*yif = 0;

	return (ThotBitmap) pixmap;
     }
}


/* ---------------------------------------------------------------------- */
/* |    JpegPrint convertit un Pixmap en PostScript.               | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                JpegPrint (char *fn, PictureScaling pres, int xif, int yif, int wif, int hif, int PicXArea, int PicYArea, int PicWArea, int PicHArea, int fd, unsigned long BackGroundPixel)
#else  /* __STDC__ */
void                JpegPrint (fn, pres, xif, yif, wif, hif, PicXArea, PicYArea, PicWArea, PicHArea, fd, BackGroundPixel)
char               *fn;
PictureScaling           pres;
int                 xif;
int                 yif;
int                 wif;
int                 hif;
int                 PicXArea;
int                 PicYArea;
int                 PicWArea;
int                 PicHArea;
int                 fd;
unsigned long       BackGroundPixel;
#endif /* __STDC__ */
{
   int                 delta;
   int                 xtmp, ytmp;
   float               Scx, Scy;
   unsigned char      *pt;
   int                 x, y, w, h;
   int                 wim;
   unsigned int        NbCharPerLine;

   ThotColorStruct     colrs[256];
   unsigned char      *buffer;

   /* lecture de la pixmap sous forme de donnees et une table de couleurs */
   /* cela nous evite d'alouer les couleurs sur l'ecran mais de les transformer */
   /* directement en RGB pour la generation ps */

   buffer = ReadJpegToData (fn, &w, &h, colrs);


   if (!buffer)
     {
	/* feed the editor with the appropriate message */
	return;
     }

   PicWArea = w;
   PicHArea = h;

   xtmp = 0;
   ytmp = 0;

   switch (pres)
	 {
	    case RealSize:

	       /* on centre l'image en x */
	       /* quelle place a-t-on de chaque cote ? */
	       delta = (wif - PicWArea) / 2;
	       if (delta > 0)
		 {
		    /* on a de la place entre l'if et le cf */
		    /* on a pas besoin de retailler dans le pixmap */
		    /* on va afficher le pixmap dans une boite plus petite */
		    /* decale'e de delta vers le centre */
		    xif += delta;
		    /* la largeur de la boite est celle du cf */
		    wif = PicWArea;
		 }
	       else
		 {
		    /* on a pas de place entre l'if et le cf */
		    /* on va retailler dans le pixmap pour que ca rentre */
		    /* on sauve delta pour savoir ou couper */
		    xtmp = -delta;
		    /* on met a jour PicWArea pour etre coherent */
		    PicWArea = wif;
		 }
	       /* on centre l'image en y */
	       delta = (hif - PicHArea) / 2;
	       if (delta > 0)
		 {
		    /* on a de la place entre l'if et le cf */
		    /* on a pas besoin de retailler dans le pixmap */
		    /* on va afficher le pixmap dans une boite plus petite */
		    /* decale'e de delta vers le centre */
		    yif += delta;
		    /* la hauteur de la boite est celle du cf */
		    hif = PicHArea;
		 }
	       else
		 {
		    /* on a pas de place entre l'if et le cf */
		    /* on va retailler dans le pixmap pour que ca rentre */
		    /* on sauve delta pour savoir ou couper */

		    ytmp = -delta;
		    /* on met a jour PicHArea pour etre coherent */
		    PicHArea = hif;
		 }
	       break;
	    case ReScale:
	       if ((float) PicHArea / (float) PicWArea <= (float) hif / (float) wif)
		 {
		    Scx = (float) wif / (float) PicWArea;
		    yif += (hif - (PicHArea * Scx)) / 2;
		    hif = PicHArea * Scx;
		 }
	       else
		 {
		    Scy = (float) hif / (float) PicHArea;
		    xif += (wif - (PicWArea * Scy)) / 2;
		    wif = PicWArea * Scy;
		 }
	       break;
	    case FillFrame:
	       /* DumpImage fait du plein cadre avec wif et hif */
	       break;
	    default:
	       break;
	 }

   /* NL plus besoin de faire des transformations */
   /* cette fonction est independante de X11                */
   /* si xtmp ou ytmp sont non nuls, ca veut dire qu'on retaille */
   /* dans le pixmap (cas RealSize) */
   wim = w;
   /*him = h; */

   /* generation du poscript , header Dumpimage2 + dimensions + deplacement dans la page */
   /* chaque pt = RRGGBB en hexa */

   fprintf ((FILE *) fd, "gsave %d -%d translate\n", PixelToPoint (xif), PixelToPoint (yif + hif));
   fprintf ((FILE *) fd, "%d %d %d %d DumpImage2\n", PicWArea, PicHArea, PixelToPoint (wif), PixelToPoint (hif));
   fprintf ((FILE *) fd, "\n");
   NbCharPerLine = wim;

   for (y = 0; y < hif; y++)
     {
	pt = (unsigned char *) (buffer + ((ytmp + y) * NbCharPerLine) + xtmp);

	for (x = 0; x < wif; x++)
	  {

	     /* generation des composantes RGB de l'image dans le poscript */

#ifndef NEW_WILLOWS
	     fprintf ((FILE *) fd, "%02x%02x%02x",
		      (colrs[*pt].red) >> 8,
		      (colrs[*pt].green) >> 8,
		      (colrs[*pt].blue) >> 8);
#endif /* NEW_WILLOWS */
	     pt++;
	  }
	fprintf ((FILE *) fd, "\n");
     }

   fprintf ((FILE *) fd, "\n");
   fprintf ((FILE *) fd, "grestore\n");
   fprintf ((FILE *) fd, "\n");
   free (buffer);

}


/* ---------------------------------------------------------------------- */
/* |    IsJpegFormat teste si un fichier contient un Pixmap X11.        | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
boolean             IsJpegFormat (char *fn)
#else  /* __STDC__ */
boolean             IsJpegFormat (fn)
char               *fn;
#endif /* __STDC__ */
{
   /*JSAMPROW buffer[1]; *//* row pointer array for read_scanlines */
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
