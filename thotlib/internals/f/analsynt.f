
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern void InitParser ( void );
extern int AsciiToInt ( iline index, iline lg );
extern void OctalToChar ( void );
extern void InitSyntax ( char *fileName );
extern void GetNextToken ( iline start, iline *wi, iline *wl, nature *wn );
extern void AnalyzeToken ( iline wi, iline wl, nature wn, grmcode *c, rnb *r, int *nb, rnb *pr );
extern void ParserEnd ( void );

#else /* __STDC__ */

extern void InitParser (/* void */);
extern int AsciiToInt (/* iline index, iline lg */);
extern void OctalToChar (/* void */);
extern void InitSyntax (/* char *fileName */);
extern void GetNextToken (/* iline start, iline *wi, iline *wl, nature *wn */);
extern void AnalyzeToken (/* iline wi, iline wl, nature wn, grmcode *c, rnb *r, int *nb, rnb *pr */);
extern void ParserEnd (/* void */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
