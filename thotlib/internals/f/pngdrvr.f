
/* -- Copyright (c) 1990 - 1994 Inria/Imag  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern unsigned char *ReadPng(FILE *infile,int *width, int *height, int *ncolors, int *cpp, ThotColorStruct colrs[256]);
extern unsigned char *ReadPngToData ( char *datafile, int *w, int *h, int *ncolors, int *cpp, ThotColorStruct colrs[256] );
extern Drawable PngCreateImage ( char * fn, PictureScaling pres, int *xif, int *yif, int *wif, int *hif, unsigned long BackGroundPixel, Drawable *mask1 );
extern void PngPrintImage ( char * fn, PictureScaling pres, int xif, int yif, int wif, int hif, int xcf, int ycf, int wcf, int hcf, int fd, unsigned long BackGroundPixel);
extern boolean PngIsFormat ( char * fn );

#else /* __STDC__ */

extern unsigned char *ReadPng (/* FILE *fd, int *w, int *h, int *ncolors, int *cpp, ThotColorStruct colrs[256] */);
extern unsigned char *ReadPngToData (/* char *datafile, int *w, int *h, ThotColorStruct colrs[256] */);
extern Drawable PngCreateImage (/* char * fn, PictureScaling pres, int *xif, int *yif, int *wif, int *hif, unsigned long BackGroundPixel, Drawable *mask1 */);
extern void PngPrintImage (/* char * fn, PictureScaling pres, int xif, int yif, int wif, int hif, int xcf, int ycf, int wcf, int hcf, int fd, unsigned long BackGroundPixel*/);
extern boolean PngIsFormat (/* char * fn */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
