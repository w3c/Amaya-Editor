
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern void GetStretchSize ( int orig_w, int orig_h, int dest_w, int dest_h, PictureScaling pres, int *presW, int *presH );
#ifdef WWW_XWINDOWS
extern Pixmap StretchPixmap ( XImage *image, int new_w, int new_h );
#endif /* WWW_XWINDOWS */

#else /* __STDC__ */

extern void GetStretchSize (/* int orig_w, int orig_h, int dest_w, int dest_h, PictureScaling pres, int *presW, int *presH */);
#ifdef WWW_XWINDOWS
extern Pixmap StretchPixmap (/* XImage *image, int new_w, int new_h */);
#endif /* WWW_XWINDOWS */

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
