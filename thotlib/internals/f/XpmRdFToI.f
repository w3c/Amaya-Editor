
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern int XpmReadFileToImage ( Display *display, char *filename, XImage **image_return, XImage **shapeimage_return, XpmAttributes *attributes );
extern int XpmReadFileToXpmImage ( char *filename, XpmImage *image, XpmInfo *info );

#else /* __STDC__ */

extern int XpmReadFileToImage (/* Display *display, char *filename, XImage **image_return, XImage **shapeimage_return, XpmAttributes *attributes */);
extern int XpmReadFileToXpmImage (/* char *filename, XpmImage *image, XpmInfo *info */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
