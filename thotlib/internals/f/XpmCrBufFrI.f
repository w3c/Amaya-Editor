
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern int XpmCreateBufferFromImage ( Display *display, char **buffer_return, XImage *image, XImage *shapeimage, XpmAttributes *attributes );
extern int XpmCreateBufferFromXpmImage ( char **buffer_return, XpmImage *image, XpmInfo *info );

#else /* __STDC__ */

extern int XpmCreateBufferFromImage (/* Display *display, char **buffer_return, XImage *image, XImage *shapeimage, XpmAttributes *attributes */);
extern int XpmCreateBufferFromXpmImage (/* char **buffer_return, XpmImage *image, XpmInfo *info */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
