/*
   Type definitions for syntactic analysis and compilation.
 */

/* a syntactic code (0 - 3003) */
typedef int	grmcode;

/* syntactic type of a token */
typedef enum
{
	shortkw, 
	name, 
	number, 
	strng, 
	err
} nature;

/* an input line to be parsed */
typedef unsigned char lineBuffer[linelen];

/* current position in the input line */
typedef int	iline;

/* a key-word in the source language */
typedef struct _kwelem
{
	char 	kwname[kwlen];	/* the key word */
	int 	kwlg;		/* its length */
	grmcode	gcode;		/* its syntactic type */
} kwelem;

/* an identifier in the source language */
typedef struct _identelem
{
	char            identname[identlen];	/* the identifier */
	int 		identlg;	/* actual length of the identifier */
	grmcode         identtype;	/* syntactic type of the identifier */
	int	        identdef;	/* number of the rule which defines
					   the identifier */
	int	        identref;	/* number of the rule where the
					   identifier is referred to */
} identelem;

/* a coded syntactical rule */
typedef grmcode 	rule[maxlgrule + 1];

/* the number of a syntactical rule */
typedef int 		rnb;
