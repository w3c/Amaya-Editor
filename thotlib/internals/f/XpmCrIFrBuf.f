
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern int XpmCreateImageFromBuffer ( Display *display, char *buffer, XImage **image_return, XImage **shapeimage_return, XpmAttributes *attributes );
extern int XpmCreateXpmImageFromBuffer ( char *buffer, XpmImage *image, XpmInfo *info );

#else /* __STDC__ */

extern int XpmCreateImageFromBuffer (/* Display *display, char *buffer, XImage **image_return, XImage **shapeimage_return, XpmAttributes *attributes */);
extern int XpmCreateXpmImageFromBuffer (/* char *buffer, XpmImage *image, XpmInfo *info */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
