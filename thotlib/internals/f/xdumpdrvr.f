
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern int XDumpOpenImageDrvr ( ImagingModel model );
extern void XDumpCloseImageDrvr ( void );
extern void XDumpInitImage ( void );
extern Drawable XDumpReadImage ( char * fn, PictureScaling pres, int wif, int hif, int xcf, int ycf, int wcf, int hcf );
extern void XDumpDrawImage ( char * fn, PictureScaling pres, int xif, int yif, int wif, int hif, int xcf, int ycf, int wcf, int hcf, Drawable drawable );
extern Drawable XDumpCreateImage ( char * fn, PictureScaling pres, int *xif, int *yif, int *wif, int *hif );
extern void XDumpEditImage ( char * fn, int xif, int yif, int wif, int hif, int *xcf, int *ycf, int *wcf, int *hcf );
extern void XDumpPrintImage ( char * fn, PictureScaling pres, int xif, int yif, int wif, int hif, int xcf, int ycf, int wcf, int hcf, int fd );
extern int XDumpIsFormat ( char * fn );
extern ImageInfo *XDumpGetImageInfo ( char * fn );
extern void XDumpEscape ( char * arg );

#else /* __STDC__ */

extern int XDumpOpenImageDrvr (/* ImagingModel model */);
extern void XDumpCloseImageDrvr (/* void */);
extern void XDumpInitImage (/* void */);
extern Drawable XDumpReadImage (/* char * fn, PictureScaling pres, int wif, int hif, int xcf, int ycf, int wcf, int hcf */);
extern void XDumpDrawImage (/* char * fn, PictureScaling pres, int xif, int yif, int wif, int hif, int xcf, int ycf, int wcf, int hcf, Drawable drawable */);
extern Drawable XDumpCreateImage (/* char * fn, PictureScaling pres, int *xif, int *yif, int *wif, int *hif */);
extern void XDumpEditImage (/* char * fn, int xif, int yif, int wif, int hif, int *xcf, int *ycf, int *wcf, int *hcf */);
extern void XDumpPrintImage (/* char * fn, PictureScaling pres, int xif, int yif, int wif, int hif, int xcf, int ycf, int wcf, int hcf, int fd */);
extern int XDumpIsFormat (/* char * fn */);
extern ImageInfo *XDumpGetImageInfo (/* char * fn */);
extern void XDumpEscape (/* char * arg */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
