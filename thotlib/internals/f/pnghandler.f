
/* -- Copyright (c) 1990 - 1994 Inria/Imag  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern unsigned char *ReadPng(FILE *infile,int *width, int *height, int *ncolors, int *cpp, ThotColorStruct colrs[256]);
extern unsigned char *ReadPngToData ( char *datafile, int *w, int *h, int *ncolors, int *cpp, ThotColorStruct colrs[256] );
extern Drawable PngCreate ( char * fn, PictureScaling pres, int *xif, int *yif, int *wif, int *hif, unsigned long BackGroundPixel, Drawable *mask1 );
extern void PngPrint ( char * fn, PictureScaling pres, int xif, int yif, int wif, int hif, int PicXArea, int PicYArea, int PicWArea, int PicHArea, int fd, unsigned long BackGroundPixel);
extern boolean IsPngFormat ( char * fn );

#else /* __STDC__ */

extern unsigned char *ReadPng (/* FILE *fd, int *w, int *h, int *ncolors, int *cpp, ThotColorStruct colrs[256] */);
extern unsigned char *ReadPngToData (/* char *datafile, int *w, int *h, ThotColorStruct colrs[256] */);
extern Drawable PngCreate (/* char * fn, PictureScaling pres, int *xif, int *yif, int *wif, int *hif, unsigned long BackGroundPixel, Drawable *mask1 */);
extern void PngPrint (/* char * fn, PictureScaling pres, int xif, int yif, int wif, int hif, int PicXArea, int PicYArea, int PicWArea, int PicHArea, int fd, unsigned long BackGroundPixel*/);
extern boolean IsPngFormat (/* char * fn */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
