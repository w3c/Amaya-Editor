/*
 * imagedrvr.h -- Definition  Acces au contenu des images
*/


#ifndef _ImageDrvr_h
#define _ImageDrvr_h

#define Bitmap_drvr      	0     /* Xlib XReadBitmapFile format */
#define EPSF_drvr        	1     /* Postscript  */
#define Pixmap_drvr      	2     /* Xpm XReadFileToPixmap format */
#define Gif_drvr        	3     /* gif */
#define Jpeg_drvr               4     /* Jpeg */
#define Png_drvr                5     /* Png */
#define MAXNBDRIVER             6

/* A few constants */
#define MAXFORMATNAMELENGHT 32       /* */
/* image driver manager */
#define NULLIMAGEDRVR 255
#define NOERROR 1


/* Penser a synchroniser cette structure avec la regle de presentation */
typedef struct 
{
  	int xcf, ycf, wcf, hcf;   /* image cropping frame */
	PictureScaling imagePres;
	int    imageType;
	char* imageFileName; 
	Pixmap imagePixmap;
	Pixmap mask;
} ImageDescriptor;
#endif
