
#ifndef __CEXTRACT__
#ifdef __STDC__

extern Drawable EPSFCreateImage ( char * fn, PictureScaling pres, int *xif, int *yif, int *wif, int *hif );
extern void EPSFPrintImage ( char * fn, PictureScaling pres, int xif, int yif, int wif, int hif, int PicXArea, int PicYArea, int PicWArea, int PicHArea, FILE *fd );
extern boolean EPSFIsFormat ( char * fn );

#else /* __STDC__ */

extern Drawable EPSFCreateImage (/* char * fn, PictureScaling pres, int *xif, int *yif, int *wif, int *hif */);
extern void EPSFPrintImage (/* char * fn, PictureScaling pres, int xif, int yif, int wif, int hif, int PicXArea, int PicYArea, int PicWArea, int PicHArea, FILE *fd */);
extern boolean EPSFIsFormat (/* char * fn */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
