/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 
/* 
   Variables for parsing languages S, P, T, and A
*/

THOT_EXPORT int 		NKeywords;		/* length of Keywords */
THOT_EXPORT int 		LastShortKeyword;	/* rank of last short keyword
						   in the Keywords table */
THOT_EXPORT SrcKeywordDesc  	Keywords[MAX_KEYWORDS];	/* table of short keywords */
THOT_EXPORT int 		NIdentifiers;		/* length of the Identifier
						   table */
THOT_EXPORT SrcIdentDesc 	Identifier[MAX_IDENTIFIERS];/* table of identifiers */
THOT_EXPORT SyntRuleNum     	NGramRules;		/* length of GramRule */
THOT_EXPORT SyntacticRule    GramRule[MAX_RULES];	/* table of syntactic rules */
