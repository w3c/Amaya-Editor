
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern int main ( unsigned int argc, char **argv );
extern void Usage ( void );
extern void ErrorMessage ( int ErrorStatus, char *tag );
extern void Punt ( int i );
extern void kinput ( ThotWidget widget, char *tag, XEvent *xe, booleanean *b );

#else /* __STDC__ */

extern int main (/* unsigned int argc, char **argv */);
extern void Usage (/* void */);
extern void ErrorMessage (/* int ErrorStatus, char *tag */);
extern void Punt (/* int i */);
extern void kinput (/* ThotWidget widget, char *tag, XEvent *xe, booleanean *b */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
