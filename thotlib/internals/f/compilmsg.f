
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern void ThotFin ( int result );
extern void TtaError ( int errorCode );
extern void CompilerError ( int index, int origine, int niveau, int numero, char *inputline, int numline );
extern void CompilerErrorString ( int index, int origine, int niveau, int numero, char *inputline, int numline, char *string );

#else /* __STDC__ */

extern void ThotFin (/* int result */);
extern void TtaError (/* int errorCode */);
extern void CompilerError (/* int index, int origine, int niveau, int numero, char *inputline, int numline */);
extern void CompilerErrorString (/* int index, int origine, int niveau, int numero, char *inputline, int numline, char *string */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
