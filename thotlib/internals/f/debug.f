
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern void CountNodes ( PtrElement pArbre, FILE *outfile, int level );
extern void NumPav ( PtrAbstractBox pP );
extern void AffPaves ( PtrAbstractBox pPav, int Indent, FILE *outfile );

#else /* __STDC__ */

extern void CountNodes (/* PtrElement pArbre, FILE *outfile, int level */);
extern void NumPav (/* PtrAbstractBox pP */);
extern void AffPaves (/* PtrAbstractBox pPav, int Indent, FILE *outfile */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
