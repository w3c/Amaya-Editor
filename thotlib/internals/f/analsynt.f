
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern void InitParser ( void );
extern int AsciiToInt ( indLine index, indLine lg );
extern void OctalToChar ( void );
extern void InitSyntax ( char *fileName );
extern void GetNextToken ( indLine start, indLine *wi, indLine *wl, SyntacticType *wn );
extern void AnalyzeToken ( indLine wi, indLine wl, SyntacticType wn, SyntacticCode *c, SyntRuleNum *r, int *nb, SyntRuleNum *pr );
extern void ParserEnd ( void );

#else /* __STDC__ */

extern void InitParser (/* void */);
extern int AsciiToInt (/* indLine index, indLine lg */);
extern void OctalToChar (/* void */);
extern void InitSyntax (/* char *fileName */);
extern void GetNextToken (/* indLine start, indLine *wi, indLine *wl, SyntacticType *wn */);
extern void AnalyzeToken (/* indLine wi, indLine wl, SyntacticType wn, SyntacticCode *c, SyntRuleNum *r, int *nb, SyntRuleNum *pr */);
extern void ParserEnd (/* void */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
