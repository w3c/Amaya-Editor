
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern void write_JPEG_file ( char * filename );
extern int read_JPEG_file ( char * filename );

#else /* __STDC__ */

extern void write_JPEG_file (/* char * filename */);
extern int read_JPEG_file (/* char * filename */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
