
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern int BitmapOpenImageDrvr ( ImagingModel model );
extern void BitmapCloseImageDrvr ( void );
extern void BitmapInitImage ( void );
extern Drawable BitmapCreateImage ( char * fn, PictureScaling pres, int *xif, int *yif, int *wif, int *hif , unsigned long BackGroundPixel, Drawable *mask1);
extern void BitmapPrintImage ( char * fn, PictureScaling pres, int xif, int yif, int wif, int hif, int xcf, int ycf, int wcf, int hcf, int fd, unsigned int BackGroundPixel );
extern boolean BitmapIsFormat ( char * fn );

#else /* __STDC__ */

extern int BitmapOpenImageDrvr (/* ImagingModel model */);
extern void BitmapCloseImageDrvr (/* void */);
extern void BitmapInitImage (/* void */);
extern Drawable BitmapCreateImage (/* char * fn, PictureScaling pres, int *xif, int *yif, int *wif, int *hif */);
extern void BitmapPrintImage (/* char * fn, PictureScaling pres, int xif, int yif, int wif, int hif, int xcf, int ycf, int wcf, int hcf, int fd */);
extern boolean BitmapIsFormat (/* char * fn */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
