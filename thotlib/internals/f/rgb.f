
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern int xpmReadRgbNames ( char *rgb_fname, xpmRgbName rgbn[] );
extern void xpmFreeRgbNames ( xpmRgbName rgbn[], int rgbn_max );

#else /* __STDC__ */

extern int xpmReadRgbNames (/* char *rgb_fname, xpmRgbName rgbn[] */);
extern void xpmFreeRgbNames (/* xpmRgbName rgbn[], int rgbn_max */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
