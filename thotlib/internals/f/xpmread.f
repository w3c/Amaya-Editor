
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern void xpmFreeColorTable ( char ***colorTable, int ncolors );
extern void xpmInitInternAttrib ( xpmInternAttrib *attrib );
extern void xpmFreeInternAttrib ( xpmInternAttrib *attrib );
extern int xpmReadFile ( char *filename, xpmData *mdata );
extern int xpmDataClose ( xpmData *mdata );
extern int xpmNextString ( xpmData *mdata );
extern unsigned int atoui ( register char *p, unsigned int l, unsigned int *ui_return );
extern unsigned int xpmNextWord ( xpmData *mdata, char *buf );
extern int xpmNextUI ( xpmData *mdata, unsigned int *ui_return );
extern int xpmParseHeader ( xpmData *mdata );
extern int xpmGetCmt ( xpmData *mdata, char **cmt );
extern int xpmParseData ( xpmData *data, xpmInternAttrib *attrib_return, XpmAttributes *attributes );
extern void xpmOpenArray ( char **data, xpmData *mdata );
extern unsigned char *ReadXpm3Pixmap ( FILE *fp, char *datafile, int *w, int *h, ThotColorStruct *colrs, int *bg );
extern unsigned char *ProcessXpm3Data ( ThotWidget wid, char **xpmdata, int *w, int *h, ThotColorStruct *colrs, int *bg );

#else /* __STDC__ */

extern void xpmFreeColorTable (/* char ***colorTable, int ncolors */);
extern void xpmInitInternAttrib (/* xpmInternAttrib *attrib */);
extern void xpmFreeInternAttrib (/* xpmInternAttrib *attrib */);
extern int xpmReadFile (/* char *filename, xpmData *mdata */);
extern int xpmDataClose (/* xpmData *mdata */);
extern int xpmNextString (/* xpmData *mdata */);
extern unsigned int atoui (/* register char *p, unsigned int l, unsigned int *ui_return */);
extern unsigned int xpmNextWord (/* xpmData *mdata, char *buf */);
extern int xpmNextUI (/* xpmData *mdata, unsigned int *ui_return */);
extern int xpmParseHeader (/* xpmData *mdata */);
extern int xpmGetCmt (/* xpmData *mdata, char **cmt */);
extern int xpmParseData (/* xpmData *data, xpmInternAttrib *attrib_return, XpmAttributes *attributes */);
extern void xpmOpenArray (/* char **data, xpmData *mdata */);
extern unsigned char *ReadXpm3Pixmap (/* FILE *fp, char *datafile, int *w, int *h, ThotColorStruct *colrs, int *bg */);
extern unsigned char *ProcessXpm3Data (/* ThotWidget wid, char **xpmdata, int *w, int *h, ThotColorStruct *colrs, int *bg */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
