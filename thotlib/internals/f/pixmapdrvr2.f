
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern int PixmapOpenImageDrvr ( CNRSingModel model );
extern void PixmapCloseImageDrvr ( void );
extern void PixmapInitImage ( void );
extern void PixmapPrintErrorMsg ( int ErrorNumber );
extern void PixmapDrawImage ( char * fn, PictureScaling pres, int xif, int yif, int wif, int hif, int xcf, int ycf, int wcf, int hcf, Drawable drawable );
extern Drawable PixmapCreateImage ( char * fn, PictureScaling pres, int *xif, int *yif, int *wif, int *hif );
extern Drawable PixmapReadImage ( char * fn, PictureScaling pres, int wif, int hif, int xcf, int ycf, int wcf, int hcf );
extern void PixmapEditImage ( char * fn, int xif, int yif, int wif, int hif, int *xcf, int *ycf, int *wcf, int *hcf );
extern void PixmapPrintImage ( char * fn, PictureScaling pres, int xif, int yif, int wif, int hif, int xcf, int ycf, int wcf, int hcf, int fd );
extern int PixmapIsFormat ( char * fn );
extern ImageInfo *PixmapGetImageInfo ( char * fn );
extern void PixmapEscape ( char * arg );

#else /* __STDC__ */

extern int PixmapOpenImageDrvr (/* CNRSingModel model */);
extern void PixmapCloseImageDrvr (/* void */);
extern void PixmapInitImage (/* void */);
extern void PixmapPrintErrorMsg (/* int ErrorNumber */);
extern void PixmapDrawImage (/* char * fn, PictureScaling pres, int xif, int yif, int wif, int hif, int xcf, int ycf, int wcf, int hcf, Drawable drawable */);
extern Drawable PixmapCreateImage (/* char * fn, PictureScaling pres, int *xif, int *yif, int *wif, int *hif */);
extern Drawable PixmapReadImage (/* char * fn, PictureScaling pres, int wif, int hif, int xcf, int ycf, int wcf, int hcf */);
extern void PixmapEditImage (/* char * fn, int xif, int yif, int wif, int hif, int *xcf, int *ycf, int *wcf, int *hcf */);
extern void PixmapPrintImage (/* char * fn, PictureScaling pres, int xif, int yif, int wif, int hif, int xcf, int ycf, int wcf, int hcf, int fd */);
extern int PixmapIsFormat (/* char * fn */);
extern ImageInfo *PixmapGetImageInfo (/* char * fn */);
extern void PixmapEscape (/* char * arg */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
