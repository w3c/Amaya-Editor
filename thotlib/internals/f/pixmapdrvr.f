
#ifndef __CEXTRACT__
#ifdef __STDC__

extern Drawable PixmapCreateImage ( char * fn, PictureScaling pres, int *xif, int *yif, int *wif, int *hif, unsigned long BackGroundPixel, Drawable *mask1 );
extern void PixmapPrintImage ( char * fn, PictureScaling pres, int xif, int yif, int wif, int hif, int PicXArea, int PicYArea, int PicWArea, int PicHArea, int fd, unsigned long BackGroundPixel );
extern boolean PixmapIsFormat ( char * fn );

#else /* __STDC__ */

extern Drawable PixmapCreateImage (/* char * fn, PictureScaling pres, int *xif, int *yif, int *wif, int *hif, unsigned long BackGroundPixel, Drawable *mask1 */);
extern void PixmapPrintImage (/* char * fn, PictureScaling pres, int xif, int yif, int wif, int hif, int PicXArea, int PicYArea, int PicWArea, int PicHArea, int fd, unsigned long BackGroundPixel */);
extern boolean PixmapIsFormat (/* char * fn */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
