
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern int XpmCreateXpmImageFromImage ( Display *display, XImage *image, XImage *shapeimage, XpmImage *xpmimage, XpmAttributes *attributes );
extern int XpmCreateXpmImageFromPixmap ( Display *display, Pixmap pixmap, Pixmap shapemask, XpmImage *xpmimage, XpmAttributes *attributes );

#else /* __STDC__ */

extern int XpmCreateXpmImageFromImage (/* Display *display, XImage *image, XImage *shapeimage, XpmImage *xpmimage, XpmAttributes *attributes */);
extern int XpmCreateXpmImageFromPixmap (/* Display *display, Pixmap pixmap, Pixmap shapemask, XpmImage *xpmimage, XpmAttributes *attributes */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
