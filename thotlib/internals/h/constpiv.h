/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 
#ifndef _THOTLIB_CONSTPIV_H_
#define _THOTLIB_CONSTPIV_H_

/*
 * marks structuring the pivot representation
 *
 */

#define C_PIV_ATTR 1		/* attribute mark */
#define C_PIV_ASSOC 2		/* associated element mark */
#define C_PIV_OLD_COMMENT 3	/* old comment mark */	
#define C_PIV_BEGIN 4		/* contents beginning mark */
#define C_PIV_COMMENT 5		/* comment mark */
#define C_PIV_END 6		/* contents ending mark */	
#define C_PIV_PRESENT 7		/* presentation rule mark */
#define C_PIV_LANG 8		/* language mark */
#define C_PIV_INCLUDED 9	/* included element mark */	
#define C_PIV_SSCHEMA_EXT 10	/* structure schema extension mark */
#define	C_PIV_LABEL 11		/* (character string) label mark */
#define C_PIV_SHORT_LABEL 12	/* short (2-byte) numerical label mark */
#define C_PIV_LONG_LABEL 13	/* long (4-byte) numerical label mark */
#define C_PIV_NATURE 14		/* nature mark */
#define C_PIV_DOCNAME 15	/* document name mark */
#define C_PIV_PARAM 16		/* parameter mark */
#define C_PIV_REFERRED 17	/* referenced element mark */
#define C_PIV_REFERENCE 18	/* reference mark */
#define C_PIV_STRUCTURE 19	/* element mark */
#define C_PIV_TYPE 20		/* type mark */
#define C_PIV_VERSION 22	/* version mark */
#define C_PIV_HOLOPHRAST 23	/* holophrasted element mark */
#define C_PIV_POLYLINE 24	/* polyline mark */
#define C_PIV_DOC_END 26	/* document ending mark */

/* specific presentation rule type codes */
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

/* picture presentation codes */
#define C_PIV_REALSIZE 'S'
#define C_PIV_RESCALE 'M'
#define C_PIV_FILLFRAME 'P'
#define C_PIV_XREPEAT 'X'
#define C_PIV_YREPEAT 'Y'

/* line alignment codes */
#define C_PIV_CENTERED 'C'
#define C_PIV_RIGHT 'D'
#define C_PIV_LEFT 'G'
#define C_PIV_LEFTDOT 'P'

/* dimension types codes */
#define C_PIV_ABSOLUTE 'A'
#define C_PIV_RELATIVE 'R'

/* units of measure codes */	
#define C_PIV_EM 'C'
#define C_PIV_PT 'P'
#define C_PIV_PERCENT '%'

/* arithmetic signs codes */
#define C_PIV_PLUS '+'
#define C_PIV_MINUS '-'
	
/* booleans codes */
#define C_PIV_FALSE 'F'
#define C_PIV_TRUE 'V'

/* page types codes */
#define C_PIV_COMPUTED_PAGE 'C'
#define C_PIV_START_PAGE 'D'
#define C_PIV_USER_PAGE 'U'
#define C_PIV_REPEAT_PAGE 'R'
#define C_PIV_COMPUTED_COL 'c'
#define C_PIV_START_COL 'd'
#define C_PIV_USER_COL 'u'
#define C_PIV_COL_GROUP 'g'

/* references types codes */
#define C_PIV_REF_FOLLOW 'r'
#define C_PIV_REF_INCLUSION 'i'
#define C_PIV_REF_INCLUS_EXP 'e'

/* inclusion of an element with on-screen expansion */
#define MOldRefInterne 'x'
#define MOldRefExterne 'y'
#define MOldRefInclusion 'z'

#endif /* _THOTLIB_CONSTPIV_H_ */
