
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern void InitDocColors (char *name);
extern int NumberOfColors ();
extern char *ColorName ( int num );
extern unsigned long ColorPixel ( int num );
extern int ColorNumber ( char *name );
extern int NumberOfPatterns ( void );
extern char *PatternName ( int num );
extern int PatternNumber ( char *name );
extern unsigned long CreatePattern ( int disp, int RO, int active, int fg, int bg, int motif );

#else /* __STDC__ */

extern void InitDocColors (/*char *name*/);
extern int NumberOfColors ();
extern char *ColorName (/* int num */);
extern unsigned long ColorPixel (/* int num */);
extern int ColorNumber (/* char *name */);
extern int NumberOfPatterns (/* void */);
extern char *PatternName (/* int num */);
extern int PatternNumber (/* char *name */);
extern unsigned long CreatePattern (/* int disp, int RO, int active, int fg, int bg, int motif */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
