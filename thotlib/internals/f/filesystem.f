
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern void *dlopen ( void );
extern void *dlsym ( void );
extern int dlclose ( void );
extern int BaseName ( char *filename, char *basename, char delim, char ext );
extern int DirName ( char *filename, char *dirname, char delim );
extern int FileExist ( char *filename );
extern int RemoveFile ( char *filename );

#else /* __STDC__ */

extern void *dlopen (/* void */);
extern void *dlsym (/* void */);
extern int dlclose (/* void */);
extern int BaseName (/* char *filename, char *basename, char delim, char ext */);
extern int DirName (/* char *filename, char *dirname, char delim */);
extern int FileExist (/* char *filename */);
extern int RemoveFile (/* char *filename */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
