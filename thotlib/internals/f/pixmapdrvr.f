
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern int PixmapOpenImageDrvr ( ImagingModel model );
extern void PixmapCloseImageDrvr ( void );
extern void PixmapInitImage ( void );
extern void PixmapPrintErrorMsg ( int ErrorNumber );
extern Drawable PixmapCreateImage ( char * fn, PictureScaling pres, int *xif, int *yif, int *wif, int *hif, unsigned long BackGroundPixel, Drawable *mask1 );
extern void PixmapPrintImage ( char * fn, PictureScaling pres, int xif, int yif, int wif, int hif, int xcf, int ycf, int wcf, int hcf, int fd, unsigned long BackGroundPixel );
extern boolean PixmapIsFormat ( char * fn );

#else /* __STDC__ */

extern int PixmapOpenImageDrvr (/* ImagingModel model */);
extern void PixmapCloseImageDrvr (/* void */);
extern void PixmapInitImage (/* void */);
extern void PixmapPrintErrorMsg (/* int ErrorNumber */);
extern Drawable PixmapCreateImage (/* char * fn, PictureScaling pres, int *xif, int *yif, int *wif, int *hif, unsigned long BackGroundPixel, Drawable *mask1 */);
extern void PixmapPrintImage (/* char * fn, PictureScaling pres, int xif, int yif, int wif, int hif, int xcf, int ycf, int wcf, int hcf, int fd, unsigned long BackGroundPixel */);
extern boolean PixmapIsFormat (/* char * fn */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
