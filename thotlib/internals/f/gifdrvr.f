
#ifndef __CEXTRACT__
#ifdef __STDC__

extern unsigned char *ReadGIF ( FILE *fd, int *w, int *h, int *ncolors, int *cpp, ThotColorStruct colrs[256] );
extern int ReadColorMap ( FILE *fd, int number, unsigned char buffer[3][256] );
extern int DoExtension ( FILE *fd, int label );
extern int GetDataBlock ( FILE *fd, unsigned char *buf );
extern int GetCode ( FILE *fd, int code_size, int flag );
extern int LWZReadByte ( FILE *fd, int flag, int input_code_size );
extern unsigned char *ReadGifToData ( char *datafile, int *w, int *h, int *ncolors, int *cpp, ThotColorStruct colrs[256] );
extern void GifPrintErrorMsg ( int ErrorNumber );
extern Drawable GifCreateImage ( char * fn, PictureScaling pres, int *xif, int *yif, int *wif, int *hif, unsigned long BackGroundPixel, Drawable *mask1 );
extern void GifPrintImage ( char * fn, PictureScaling pres, int xif, int yif, int wif, int hif, int PicXArea, int PicYArea, int PicWArea, int PicHArea, int fd, unsigned long BackGroundPixel);
extern boolean GifIsFormat ( char * fn );

#else /* __STDC__ */

extern unsigned char *ReadGIF (/* FILE *fd, int *w, int *h, int *ncolors, int *cpp, ThotColorStruct colrs[256] */);
extern int ReadColorMap (/* FILE *fd, int number, unsigned char buffer[3][256] */);
extern int DoExtension (/* FILE *fd, int label */);
extern int GetDataBlock (/* FILE *fd, unsigned char *buf */);
extern int GetCode (/* FILE *fd, int code_size, int flag */);
extern int LWZReadByte (/* FILE *fd, int flag, int input_code_size */);
extern unsigned char *ReadGifToData (/* char *datafile, int *w, int *h, int *ncolors, int *cpp, ThotColorStruct colrs[256] */);
extern void GifPrintErrorMsg (/* int ErrorNumber */);
extern Drawable GifCreateImage (/* char * fn, PictureScaling pres, int *xif, int *yif, int *wif, int *hif, unsigned long BackGroundPixel, Drawable *mask1 */);
extern void GifPrintImage (/* char * fn, PictureScaling pres, int xif, int yif, int wif, int hif, int PicXArea, int PicYArea, int PicWArea, int PicHArea, int fd, unsigned long BackGroundPixel*/);
extern boolean GifIsFormat (/* char * fn */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
