
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern int EPSFOpenImageDrvr ( ImagingModel model );
extern void EPSFCloseImageDrvr ( void );
extern void EPSFInitImage ( void );
extern Drawable EPSFCreateImage ( char * fn, PictureScaling pres, int *xif, int *yif, int *wif, int *hif );
extern void EPSFPrintImage ( char * fn, PictureScaling pres, int xif, int yif, int wif, int hif, int xcf, int ycf, int wcf, int hcf, FILE *fd );
extern boolean EPSFIsFormat ( char * fn );

#else /* __STDC__ */

extern int EPSFOpenImageDrvr (/* ImagingModel model */);
extern void EPSFCloseImageDrvr (/* void */);
extern void EPSFInitImage (/* void */);
extern Drawable EPSFCreateImage (/* char * fn, PictureScaling pres, int *xif, int *yif, int *wif, int *hif */);
extern void EPSFPrintImage (/* char * fn, PictureScaling pres, int xif, int yif, int wif, int hif, int xcf, int ycf, int wcf, int hcf, FILE *fd */);
extern boolean EPSFIsFormat (/* char * fn */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
