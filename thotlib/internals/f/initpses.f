
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern void InitDocColors (char *name);
extern unsigned long ColorPixel ( int num );
extern void ColorRGB ( int num, unsigned short *red, unsigned short *green, unsigned short *blue );
#ifdef WWW_XWINDOWS
extern Pixmap CreatePattern ( int disp, int RO, int active, int fg, int bg, int motif );
#endif /* WWW_XWINDOWS */

#else /* __STDC__ */

extern void InitDocColors (/*char * name*/);
extern unsigned long ColorPixel (/* int num */);
extern void ColorRGB (/* int num, unsigned short *red, unsigned short *green, unsigned short *blue */);
#ifdef WWW_XWINDOWS
extern Pixmap CreatePattern (/* int disp, int RO, int active, int fg, int bg, int motif */);
#endif /* WWW_XWINDOWS */

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
