
#ifndef __CEXTRACT__
#ifdef __STDC__

extern Drawable EpsCreate ( char * fn, PictureScaling pres, int *xif, int *yif, int *wif, int *hif );
extern void EpsPrint ( char * fn, PictureScaling pres, int xif, int yif, int wif, int hif, int PicXArea, int PicYArea, int PicWArea, int PicHArea, FILE *fd );
extern boolean IsEpsFormat ( char * fn );

#else /* __STDC__ */

extern Drawable EpsCreate (/* char * fn, PictureScaling pres, int *xif, int *yif, int *wif, int *hif */);
extern void EpsPrint (/* char * fn, PictureScaling pres, int xif, int yif, int wif, int hif, int PicXArea, int PicYArea, int PicWArea, int PicHArea, FILE *fd */);
extern boolean IsEpsFormat (/* char * fn */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
