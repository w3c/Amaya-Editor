
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern void BuildFileName ( Name fname, char *fext, PathBuffer directory_list, PathBuffer nomabs, int *lg );
extern void GetImageFileName(char *name, char *fn);
extern void DoFileName ( Name fname, char *fext, PathBuffer directory, PathBuffer nomabs, int *lg );

#else /* __STDC__ */

extern void BuildFileName (/* Name fname, char *fext, PathBuffer directory_list, PathBuffer nomabs, int *lg */);
extern void GetImageFileName(/*char *name, char *fn*/);
extern void DoFileName (/* Name fname, char *fext, PathBuffer directory, PathBuffer nomabs, int *lg */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
