/*
   Type definitions for syntactic analysis and compilation.
 */

/* a syntactic code (0 - 3003) */
typedef int	SyntacticCode;

/* syntactic type of a token */
typedef enum
{
	SynShortKeyword, 
	SynIdentifier, 
	SynInteger, 
	SynString, 
	SynError
} SyntacticType;

/* an input line to be parsed */
typedef unsigned char lineBuffer[linelen];

/* current position in the input line */
typedef int	indLine;

/* a key-word in the source language */
typedef struct _SrcKeywordDesc
{
	char 	SrcKeyword[kwlen];	/* the key word */
	int 	SrcKeywordLen;		/* its length */
	SyntacticCode	SrcKeywordCode;	/* its syntactic type */
} SrcKeywordDesc;

/* an identifier in the source language */
typedef struct _SrcIdentDesc
{
	char            SrcIdentifier[identlen]; /* the identifier */
	int 		SrcIdentLen;		/* length of the identifier */
	SyntacticCode         SrcIdentCode;	/* syntactic type of the identifier */
	int	        SrcIdentDefRule;	/* number of the rule which
						defines the identifier */
	int	        SrcIdentRefRule;	/* number of the rule where the
						identifier is referred to */
} SrcIdentDesc;

/* a coded syntactical rule */
typedef SyntacticCode 	SyntacticRule[maxlgrule + 1];

/* the number of a syntactical rule */
typedef int 		SyntRuleNum;
