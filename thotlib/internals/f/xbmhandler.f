
#ifndef __CEXTRACT__
#ifdef __STDC__

extern Drawable XbmCreate ( char * fn, PictureScaling pres, int *xif, int *yif, int *wif, int *hif , unsigned long BackGroundPixel, Drawable *mask1);
extern void XbmPrint ( char * fn, PictureScaling pres, int xif, int yif, int wif, int hif, int PicXArea, int PicYArea, int PicWArea, int PicHArea, int fd, unsigned int BackGroundPixel );
extern boolean IsXbmFormat ( char * fn );

#else /* __STDC__ */

extern Drawable XbmCreate (/* char * fn, PictureScaling pres, int *xif, int *yif, int *wif, int *hif */);
extern void XbmPrint (/* char * fn, PictureScaling pres, int xif, int yif, int wif, int hif, int PicXArea, int PicYArea, int PicWArea, int PicHArea, int fd */);
extern boolean IsXbmFormat (/* char * fn */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
