
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern unsigned char *ReadJpeg ( FILE *fd, int *w, int *h, ThotColorStruct colrs[256] );
extern unsigned char *ReadJpegToData ( char *datafile, int *w, int *h,  ThotColorStruct colrs[256] );
extern int JpegOpenImageDrvr ( boolean printing);
extern void JpegCloseImageDrvr ( void );
extern void JpegInitImage ( void );
extern Drawable JpegCreateImage ( char * fn, PicPresent pres, int *xif, int *yif, int *wif, int *hif, unsigned long BackGroundPixel, Drawable *mask1 );
extern void JpegPrintImage ( char * fn, ImagePres pres, int xif, int yif, int wif, int hif, int xcf, int ycf, int wcf, int hcf, int fd, unsigned long BackGroundPixel);
extern boolean JpegIsFormat ( char * fn );

#else /* __STDC__ */

extern unsigned char *ReadJpeg (/* FILE *fd, int *w, int *h, int *ncolors, int *cpp, ThotColorStruct colrs[256] */);
extern unsigned char *ReadJpegToData (/* char *datafile, int *w, int *h, ThotColorStruct colrs[256] */);
extern int JpegOpenImageDrvr (/* bbolean printing */);
extern void JpegCloseImageDrvr (/* void */);
extern void JpegInitImage (/* void */);
extern Drawable JpegCreateImage (/* char * fn, PicPresent pres, int *xif, int *yif, int *wif, int *hif, unsigned long BackGroundPixel, Drawable *mask1 */);
extern void JpegPrintImage (/* char * fn, ImagePres pres, int xif, int yif, int wif, int hif, int xcf, int ycf, int wcf, int hcf, int fd, unsigned long BackGroundPixel*/);
extern boolean JpegIsFormat (/* char * fn */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
