/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 
/* 
   Variables for parsing languages S, P, T, and A
*/

EXPORT int 		NKeywords;		/* length of Keywords */
EXPORT int 		LastShortKeyword;	/* rank of last short keyword
						   in the Keywords table */
EXPORT SrcKeywordDesc  	Keywords[MAX_KEYWORDS];	/* table of short keywords */
EXPORT int 		NIdentifiers;		/* length of the Identifier
						   table */
EXPORT SrcIdentDesc 	Identifier[MAX_IDENTIFIERS];/* table of identifiers */
EXPORT SyntRuleNum     	NGramRules;		/* length of GramRule */
EXPORT SyntacticRule    GramRule[MAX_RULES];	/* table of syntactic rules */
