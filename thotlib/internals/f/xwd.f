
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern int main ( int argc, char **argv );
extern int ThotWindow_Dump ( ThotWindow window, int interactiv, int xi, int yi, int wi, int hi, FILE *out );
extern int usage ( void );
extern int Image_Size ( XImage *image );
extern int Get_ThotColorStructs ( XThotWindowAttributes *win_info, ThotColorStruct **colors );
extern int _swapshort ( register char *bp, register unsigned n );
extern int _swaplong ( register char *bp, register unsigned n );
extern ThotWindow Select_Interactiv_ThotWindow ( Display *dpy );
extern ThotWindow DefineArea ( ThotWindow win, int *rX, int *rY, int *rW, int *rH, int clear );

#else /* __STDC__ */

extern int main (/* int argc, char **argv */);
extern int ThotWindow_Dump (/* ThotWindow window, int interactiv, int xi, int yi, int wi, int hi, FILE *out */);
extern int usage (/* void */);
extern int Image_Size (/* XImage *image */);
extern int Get_ThotColorStructs (/* XThotWindowAttributes *win_info, ThotColorStruct **colors */);
extern int _swapshort (/* register char *bp, register unsigned n */);
extern int _swaplong (/* register char *bp, register unsigned n */);
extern ThotWindow Select_Interactiv_ThotWindow (/* Display *dpy */);
extern ThotWindow DefineArea (/* ThotWindow win, int *rX, int *rY, int *rW, int *rH, int clear */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
