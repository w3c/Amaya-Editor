
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern unsigned char *ReadJpeg ( FILE *fd, int *w, int *h, ThotColorStruct colrs[256] );
extern unsigned char *ReadJpegToData ( char *datafile, int *w, int *h,  ThotColorStruct colrs[256] );
extern ThotBitmap JpegCreate (char *fn, PictureScaling pres, int *xif, int *yif, int *wif, int *hif, unsigned long BackGroundPixel, Drawable * mask1);
void JpegPrint (char *fn, PictureScaling pres, int xif, int yif, int wif, int hif, int PicXArea, int PicYArea, int PicWArea, int PicHArea, int fd, unsigned long BackGroundPixel);
extern boolean IsJpegFormat ( char * fn );

#else /* __STDC__ */

extern unsigned char *ReadJpeg (/* FILE *fd, int *w, int *h, int *ncolors, int *cpp, ThotColorStruct colrs[256] */);
extern unsigned char *ReadJpegToData (/* char *datafile, int *w, int *h, ThotColorStruct colrs[256] */);
extern ThotBitmap JpegCreate (/*char *fn, PictureScaling pres, int *xif, int *yif, int *wif, int *hif, unsigned long BackGroundPixel, Drawable * mask1*/);
extern int wif, int hif, int PicXArea, int PicYArea, int PicWArea, int PicHArea, int fd, unsigned long BackGroundPixel);
extern boolean IsJpegFormat (/* char * fn */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
