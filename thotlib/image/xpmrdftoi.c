
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
* XpmRdFToI.c:                                                                *
*                                                                             *
*  XPM library                                                                *
*  Parse an XPM file and create the image and possibly its PicMask               *
*                                                                             *
*  Developed by Arnaud Le Hors                                                *
\*****************************************************************************/

#include "thot_gui.h"
#include "thot_sys.h"

#ifdef __STDC__
#include "xpmP.h"

int
                    XpmReadFileToImage (Display * display, char *filename, XImage ** image_return, XImage ** shapeimage_return, XpmAttributes * attributes)

#else  /* __STDC__ */
#include "xpmP.h"

int
                    XpmReadFileToImage (display, filename, image_return, shapeimage_return, attributes)
Display            *display;
char               *filename;
XImage            **image_return;
XImage            **shapeimage_return;
XpmAttributes      *attributes;

#endif /* __STDC__ */

{
   XpmImage            image;
   XpmInfo             info;
   int                 ErrorStatus;

   /* create an XpmImage from the file */
   if (attributes)
     {
	xpmInitAttributes (attributes);
	xpmSetInfoMask (&info, attributes);
	ErrorStatus = XpmReadFileToXpmImage (filename, &image, &info);
     }
   else
      ErrorStatus = XpmReadFileToXpmImage (filename, &image, NULL);

   if (ErrorStatus != XpmSuccess)
      return (ErrorStatus);

   /* create the related ximages */
   ErrorStatus = XpmCreateImageFromXpmImage (display, &image,
					     image_return, shapeimage_return,
					     attributes);
   if (attributes)
     {
	if (ErrorStatus >= 0)	/* no fatal error */
	   xpmSetAttributes (attributes, &image, &info);
	XpmFreeXpmInfo (&info);
     }
   /* free the XpmImage */
   XpmFreeXpmImage (&image);

   return (ErrorStatus);
}

#ifdef __STDC__
int
                    XpmReadFileToXpmImage (char *filename, XpmImage * image, XpmInfo * info)

#else  /* __STDC__ */
int
                    XpmReadFileToXpmImage (filename, image, info)
char               *filename;
XpmImage           *image;
XpmInfo            *info;

#endif /* __STDC__ */

{
   xpmData             mdata;
   int                 ErrorStatus;

   /* init returned values */
   xpmInitXpmImage (image);
   xpmInitXpmInfo (info);

   /* open file to read */
   if ((ErrorStatus = xpmReadFile (filename, &mdata)) != XpmSuccess)
      return (ErrorStatus);

   /* create the XpmImage from the XpmData */
   ErrorStatus = xpmParseData (&mdata, image, info);

   xpmDataClose (&mdata);

   return (ErrorStatus);
}
