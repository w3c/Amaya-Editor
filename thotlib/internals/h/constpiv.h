
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

/*
 * marques structurant la representation pivot
 *
 */

#define C_PIV_ATTR 1		/* marque d'attribut */
#define C_PIV_ASSOC 2		/* marque d'element associe */
#define C_PIV_OLD_COMMENT 3		/* marque de commentaire ancienne version */
#define C_PIV_BEGIN 4		/* marque de debut de contenu */
#define C_PIV_COMMENT 5		/* marque de commentaire */
#define C_PIV_END 6			/* marque de fin de contenu */
#define C_PIV_PRESENT 7		/* marque de regle de presentation */

#define C_PIV_LANG 8		/* marque de langue */
#define C_PIV_INCLUDED 9		/* marque d'element inclus */

#define C_PIV_SSCHEMA_EXT 10	/* marque d'extension de schema de structure */

#define	C_PIV_LABEL 11		/* marque de label chaine de caracteres */
#define C_PIV_SHORT_LABEL 12		/* marque de label numerique court (2 octets)*/
#define C_PIV_LONG_LABEL 13		/* marque de label numerique long (4 octets) */
#define C_PIV_NATURE 14		/* marque de la classe (ex-nature) */
#define C_PIV_DOCNAME 15		/* marque de nom de document */
#define C_PIV_PARAM 16		/* marque de parametre */

#define C_PIV_REFERRED 17	/* marque d'element reference' */
#define C_PIV_REFERENCE 18			/* marque de reference */
#define C_PIV_STRUCTURE 19		/* marque d'element structure */
#define C_PIV_TYPE 20		/* marque de type */
#define C_PIV_VERSION 22		/* marque de version */

#define C_PIV_HOLOPHRAST 23		/* marque d'element holophraste' */

#define C_PIV_POLYLINE 24		/* marque de polyline */
#define C_PIV_DOC_END 26		/* marque de fin de document */

/* codes des types de regles de presentation specifique */
#define C_PR_ADJUST 'A'
#define C_PR_FONT 'F'
#define C_PR_UNDERLINE 'U'
#define C_PR_UNDER_THICK 'E'
#define C_PR_HEIGHT 'H'
#define C_PR_LINESPACING 'I'
#define C_PR_JUSTIFY 'J'

#define C_PR_HYPHENATE 'C'
#define C_PR_WIDTH 'L'
#define C_PR_BREAK1 '1'
#define C_PR_BREAK2 '2'
#define C_PR_PICTURE 'P'
#define C_PR_INDENT 'R'
#define C_PR_STYLE 'S'
#define C_PR_SIZE 'T'
#define C_PR_HPOS 'X'
#define C_PR_VPOS 'Y'

#define C_PR_LINESTYLE 'l'
#define C_PR_LINEWEIGHT 'W'
#define C_PR_FILLPATTERN 'p'
#define C_PR_BACKGROUND 'b'
#define C_PR_FOREGROUND 't'

/* codes de presentation des images */
#define C_PIV_REALSIZE 'S'
#define C_PIV_RESCALE 'M'
#define C_PIV_FILLFRAME 'P'

/* codes des cadrages des lignes */
#define C_PIV_CENTERED 'C'
#define C_PIV_RIGHT 'D'
#define C_PIV_LEFT 'G'
#define C_PIV_LEFTDOT 'P'

/* code des types de dimension */
#define C_PIV_ABSOLUTE 'A'
#define C_PIV_RELATIVE 'R'
	
/* codes des unites de mesure */
#define C_PIV_EM 'C'
#define C_PIV_PT 'P'
#define C_PIV_PERCENT '%'
	
/* codes des signes arithmetiques */
#define C_PIV_PLUS '+'
#define C_PIV_MINUS '-'
	
/* code des booleens */
#define C_PIV_FALSE 'F'
#define C_PIV_TRUE 'V'
	
/* codes des types de page */
#define C_PIV_COMPUTED_PAGE 'C'
#define C_PIV_START_PAGE 'D'
#define C_PIV_USER_PAGE 'U'
#define C_PIV_REPEAT_PAGE 'R'

#define C_PIV_COMPUTED_COL 'c'
#define C_PIV_START_COL 'd'
#define C_PIV_USER_COL 'u'
#define C_PIV_COL_GROUP 'g'

/* code des types de references */
#define C_PIV_REF_FOLLOW 'r'
#define C_PIV_REF_INCLUSION 'i'
#define C_PIV_REF_INCLUS_EXP 'e'

/* inclusion d'un element avec expansion a l'ecran */
/* a supprimer */
#define MOldRefInterne 'x'
#define MOldRefExterne 'y'
#define MOldRefInclusion 'z'
