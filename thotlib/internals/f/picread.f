
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern unsigned char *ReadXpmPixmap ( FILE *fp, char *datafile, int *w, int *h, ThotColorStruct *colrs, int Colors, int CharsPP );
extern unsigned char *ReadXbmBitmap ( FILE *fp, char *datafile, int *w, int *h, ThotColorStruct *colrs );
extern unsigned char *ReadBitmap ( char *datafile, int *w, int *h, ThotColorStruct *colrs, int *bg );

#else /* __STDC__ */

extern unsigned char *ReadXpmPixmap (/* FILE *fp, char *datafile, int *w, int *h, ThotColorStruct *colrs, int Colors, int CharsPP */);
extern unsigned char *ReadXbmBitmap (/* FILE *fp, char *datafile, int *w, int *h, ThotColorStruct *colrs */);
extern unsigned char *ReadBitmap (/* char *datafile, int *w, int *h, ThotColorStruct *colrs, int *bg */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
