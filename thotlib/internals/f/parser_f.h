#ifndef PARSE_F_H
#define PARSE_F_H

#ifdef __STDC__
extern void                InitParser (void) ;
extern int                 AsciiToInt (indLine index, indLine len) ;
extern void                OctalToChar (void) ;
extern void                GetNextToken (indLine start, indLine * wi, indLine * wl, SyntacticType * wn) ;
extern void                AnalyzeToken (indLine wi, indLine wl, SyntacticType wn, SyntacticCode * c, SyntRuleNum * r, int *rank, SyntRuleNum * pr) ;
extern void                ParserEnd (void) ;
extern void                InitSyntax (char *fileName) ;
#else
extern void                InitParser () ;
extern int                 AsciiToInt () ;
extern void                OctalToChar () ;
extern void                GetNextToken () ;
extern void                AnalyzeToken () ;
extern void                ParserEnd () ;
extern void                InitSyntax () ;
#endif /* __STDC__ */

#endif /* PARSE_F_H */
