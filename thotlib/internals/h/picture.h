/*
 * imagedrvr.h -- Definition  Acces au contenu des images
*/


#ifndef _ImageDrvr_h
#define _ImageDrvr_h

#define XBM_FORMAT      	0     /* Xlib XReadBitmapFile format */
#define EPS_FORMAT        	1     /* Postscript  */
#define XPM_FORMAT      	2     /* Xpm XReadFileToPixmap format */
#define GIF_FORMAT        	3     /* gif */
#define JPEG_FORMAT               4     /* Jpeg */
#define PNG_FORMAT                5     /* Png */
#define MAX_PICT_FORMATS             6

/* A few constants */
#define MAX_FORMAT_NAMELENGHT 32       /* */
/* image driver manager */
#define NULLIMAGEDRVR 255
#define NOERROR 1


/* Penser a synchroniser cette structure avec la regle de presentation */
typedef struct 
{
  	int PicXArea, PicYArea, PicWArea, PicHArea;   /* image cropping frame */
	PictureScaling PicPresent;
	int    PicType;
	char* PicFileName; 
	Pixmap PicPixmap;
	Pixmap PicMask;
} PictInfo;
#endif
