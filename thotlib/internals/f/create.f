
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern int XpmCreateImageFromXpmImage ( Display *display, XpmImage *image, XImage **image_return, XImage **shapeimage_return, XpmAttributes *attributes );
extern void xpm_xynormalizeimagebits ( register unsigned char *bp, register XImage *img );
extern void xpm_znormalizeimagebits ( register unsigned char *bp, register XImage *img );
extern int XpmCreatePixmapFromXpmImage ( Display *display, Drawable d, XpmImage *image, Pixmap *pixmap_return, Pixmap *shapemask_return, XpmAttributes *attributes );

#else /* __STDC__ */

extern int XpmCreateImageFromXpmImage (/* Display *display, XpmImage *image, XImage **image_return, XImage **shapeimage_return, XpmAttributes *attributes */);
extern void xpm_xynormalizeimagebits (/* register unsigned char *bp, register XImage *img */);
extern void xpm_znormalizeimagebits (/* register unsigned char *bp, register XImage *img */);
extern int XpmCreatePixmapFromXpmImage (/* Display *display, Drawable d, XpmImage *image, Pixmap *pixmap_return, Pixmap *shapemask_return, XpmAttributes *attributes */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
