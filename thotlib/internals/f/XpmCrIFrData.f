
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern int XpmCreateImageFromData ( Display *display, char **data, XImage **image_return, XImage **shapeimage_return, XpmAttributes *attributes );
extern int XpmCreateXpmImageFromData ( char **data, XpmImage *image, XpmInfo *info );

#else /* __STDC__ */

extern int XpmCreateImageFromData (/* Display *display, char **data, XImage **image_return, XImage **shapeimage_return, XpmAttributes *attributes */);
extern int XpmCreateXpmImageFromData (/* char **data, XpmImage *image, XpmInfo *info */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
