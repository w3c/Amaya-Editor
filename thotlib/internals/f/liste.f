
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__
/**CP*/ /*  WrPav est ajoutee */
extern void WrPav ( PtrAbstractBox pPav, int Indent, FILE *outfile );
extern void ListeBoites ( Name fname, int fen );
extern void StatMedia ( FILE *outfile );

#else /* __STDC__ */

extern void WrPav (/* PtrAbstractBox pPav, int Indent, FILE *outfile */);
extern void ListeBoites (/* Name fname, int fen */);
extern void StatMedia (/* FILE *outfile */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
