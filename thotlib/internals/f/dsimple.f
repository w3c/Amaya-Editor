
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern void Fatal_Error ( char *msg, char *arg0, char *arg1, char *arg2, char *arg3, char *arg4, char *arg5, char *arg6 );
extern char *Malloc ( unsigned size );
extern char *Realloc ( char *ptr, int size );
extern Display *Open_Display ( char *display_name );
extern XFontStruct *Open_Font ( char *name );
extern void Beep ( void );
extern Pixmap ReadBitmapFile ( Drawable d, char *filename, int *width, int *height, int *x_hot, int *y_hot );
extern void WriteBitmapFile ( char *filename, Pixmap bitmap, int width, int height, int x_hot, int y_hot );
extern ThotWindow Select_ThotWindow_Args ( int *rargc, char **argv );
extern unsigned long Resolve_Color ( ThotWindow w, char *name );
extern Pixmap Bitmap_To_Pixmap ( Display *dpy, Drawable d, ThotGC gc, Pixmap bitmap, int width, int height );
extern int outl ( char *msg, char *arg0, char *arg1, char *arg2, char *arg3, char *arg4, char *arg5, char *arg6 );
extern void blip ( void );
extern ThotWindow Select_ThotWindow ( Display *dpy );
extern ThotWindow ThotWindow_With_Name ( Display *dpy, ThotWindow top, char *name );

#else /* __STDC__ */

extern void Fatal_Error (/* char *msg, char *arg0, char *arg1, char *arg2, char *arg3, char *arg4, char *arg5, char *arg6 */);
extern char *Malloc (/* unsigned size */);
extern char *Realloc (/* char *ptr, int size */);
extern Display *Open_Display (/* char *display_name */);
extern XFontStruct *Open_Font (/* char *name */);
extern void Beep (/* void */);
extern Pixmap ReadBitmapFile (/* Drawable d, char *filename, int *width, int *height, int *x_hot, int *y_hot */);
extern void WriteBitmapFile (/* char *filename, Pixmap bitmap, int width, int height, int x_hot, int y_hot */);
extern ThotWindow Select_ThotWindow_Args (/* int *rargc, char **argv */);
extern unsigned long Resolve_Color (/* ThotWindow w, char *name */);
extern Pixmap Bitmap_To_Pixmap (/* Display *dpy, Drawable d, ThotGC gc, Pixmap bitmap, int width, int height */);
extern int outl (/* char *msg, char *arg0, char *arg1, char *arg2, char *arg3, char *arg4, char *arg5, char *arg6 */);
extern void blip (/* void */);
extern ThotWindow Select_ThotWindow (/* Display *dpy */);
extern ThotWindow ThotWindow_With_Name (/* Display *dpy, ThotWindow top, char *name */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
