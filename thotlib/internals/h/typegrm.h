/*
 * Declarations pour l'analyse syntaxique et la compilation
 * V. Quint	Juin 1984 
 */

typedef char	gname[11];	/* nom de fichier grammaire */
typedef int	grmcode;	/* 0 .. 3003, valeur d'un code grammatical */
typedef int	errcode;	/* 1 .. 100, code d'erreur */

/* nature d'un mot */
typedef enum
{
	shortkw, 
	name, 
	number, 
	strng, 
	err
} nature;

typedef unsigned char lineBuffer[linelen]; /* une ligne de texte */
typedef int	iline;			/* 0 .. linelen, index dans une ligne*/

typedef struct _kwelem
{
	int 	kwlg;		/* 1 .. kwlen , longueur effective du mot-cle*/
	char 	kwname[kwlen];	/* le mot-cle */
	grmcode	gcode;		/* code grammatical associe */
} kwelem;

typedef struct _identelem
{
	int	        identdef;	/* numero de la regle ou
					   l'identificateur est defini */
	int	        identref;	/* numero de la regle ou
					   l'identificateur est reference */
	grmcode         identtype;	/* type grammatical de l'identific. */
	int 		identlg;	/* 1 .. identlenlongueur effective de
					   l'identific  */
	char            identname[identlen];
} identelem;

typedef grmcode 	rule[maxlgrule + 1]; /* une regle de grammaire codee */
typedef int 		rnb;		     /* 0 .. maxrule, numero de regle
						grammaticale */
typedef char    	msgerr[50];	     /* texte d'un message d'erreur */
