/*
 * imagedrvr.h -- Definition  Acces au contenu des images
*/


#ifndef _ImageDrvr_h
#define _ImageDrvr_h

#define XBM_FORMAT      	0     /* Xlib XReadBitmapFile format */
#define EPS_FORMAT        	1     /* Postscript  */
#define XPM_FORMAT      	2     /* Xpm XReadFileToPixmap format */
#define GIF_FORMAT        	3     /* gif */
#define JPEG_FORMAT             4     /* Jpeg */
#define PNG_FORMAT              5     /* Png */
#define MAX_PICT_FORMATS        6

/* A few constants */
#define MAX_FORMAT_NAMELENGHT 32       /* */
/* image driver manager */
#define NULLIMAGEDRVR 255
#define NOERROR 1

#define BitmapName "Bitmap (.xbm)" /* ok ? or something else ? We need this
                               to name the format at the UI level... */
#define EPSFName "EPSF (.eps)" /* ok ? or something else ? We need this
                               to name the format at the UI level... */
#define GifName "Gif (.gif)" /* ok ? or something else ? We need this
                               to name the format at the UI level... */
#define JpegName "Jpeg (.jpg)" /* ok ? or something else ? We need this
                               to name the format at the UI level... */
#define PixmapName "Pixmap (.xpm)" /* ok ? or something else ? We need this
                               to name the format at the UI level... */
#define PngName "Ping (.png)" /* ok ? or something else ? We need this */

/* Penser a synchroniser cette structure avec la regle de presentation */
typedef struct 
{
  	int PicXArea, PicYArea, PicWArea, PicHArea; /* image cropping frame */
	PictureScaling PicPresent;
	int    PicType;
	char* PicFileName; 
	Pixmap PicPixmap;
	Pixmap PicMask;
} PictInfo;
#endif
