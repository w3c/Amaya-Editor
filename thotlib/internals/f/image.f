
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern Pixmap TtaCreateLogoFromPixmap ( unsigned int width, unsigned int height, unsigned int ncolors, char **colors, char **pixels );
extern Pixmap TtaCreateLogoFromBitmap ( int width, int height, char *bits );

#else /* __STDC__ */

extern Pixmap TtaCreateLogoFromPixmap (/* unsigned int width, unsigned int height, unsigned int ncolors, char **colors, char **pixels */);
extern Pixmap TtaCreateLogoFromBitmap (/* int width, int height, char *bits */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
