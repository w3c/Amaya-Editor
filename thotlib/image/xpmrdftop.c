
/*
 * Copyright (C) 1989-94 GROUPE BULL
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * GROUPE BULL BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
 * AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Except as contained in this notice, the name of GROUPE BULL shall not be
 * used in advertising or otherwise to promote the sale, use or other dealings
 * in this Software without prior written authorization from GROUPE BULL.
 */

/*
 * Warning:
 * This module is part of the Thot library, which was originally
 * developed in French. That's why some comments are still in
 * French, but their translation is in progress and the full module
 * will be available in English in the next release.
 * 
 */

/*****************************************************************************\
* XpmRdFToP.c:                                                                *
*                                                                             *
*  XPM library                                                                *
*  Parse an XPM file and create the pixmap and possibly its PicMask              *
*                                                                             *
*  Developed by Arnaud Le Hors                                                *
\*****************************************************************************/

#include "thot_gui.h"
#include "thot_sys.h"

#ifdef __STDC__
#include "xpmP.h"

int
                    XpmReadFileToPixmap (Display * display, Drawable d, char *filename, Pixmap * pixmap_return, Pixmap * shapemask_return, XpmAttributes * attributes)

#else  /* __STDC__ */
#include "xpmP.h"

int
                    XpmReadFileToPixmap (display, d, filename, pixmap_return, shapemask_return, attributes)
Display            *display;
Drawable            d;
char               *filename;
Pixmap             *pixmap_return;
Pixmap             *shapemask_return;
XpmAttributes      *attributes;

#endif /* __STDC__ */

{
   XImage             *ximage, *shapeimage;
   int                 ErrorStatus;

   /* initialize return values */
   if (pixmap_return)
      *pixmap_return = 0;
   if (shapemask_return)
      *shapemask_return = 0;

   /* create the images */
   ErrorStatus = XpmReadFileToImage (display, filename,
				     (pixmap_return ? &ximage : NULL),
				     (shapemask_return ? &shapeimage : NULL),
				     attributes);

   if (ErrorStatus < 0)		/* fatal error */
      return (ErrorStatus);

   /* create the pixmaps and destroy images */
   if (pixmap_return && ximage)
     {
	xpmCreatePixmapFromImage (display, d, ximage, pixmap_return);
	XDestroyImage (ximage);
     }
   if (shapemask_return && shapeimage)
     {
	xpmCreatePixmapFromImage (display, d, shapeimage, shapemask_return);
	XDestroyImage (shapeimage);
     }
   return (ErrorStatus);
}
