/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2009
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
#define C_PIV_BEGIN 4		/* contents beginning mark */
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
#define C_PIV_REFERRED 17	/* referenced element mark */
#define C_PIV_REFERENCE 18	/* reference mark */
#define C_PIV_STRUCTURE 19	/* element mark */
#define C_PIV_TYPE 20		/* type mark */
#define C_PIV_VERSION 22	/* version mark */
#define C_PIV_HOLOPHRAST 23	/* holophrasted element mark */
#define C_PIV_POLYLINE 24	/* polyline mark */
#define C_PIV_PATH 25		/* graphics path mark */
#define C_PIV_TRANS_START 26    /* transformation mark*/
#define C_PIV_TRANS_END 27      /* transformation mark*/
#define C_PIV_DOC_END 28	/* document ending mark */

/* specific presentation rule type codes */
#define C_PR_MARKER '&'
#define C_PR_MARKERSTART 39
#define C_PR_MARKERMID '('
#define C_PR_MARKEREND ')'
#define C_PR_VIS '*'
#define C_PR_COLOR '+'
#define C_PR_STOPCOLOR ','
#define C_PR_STOPOPACITY '-'
#define C_PR_POSITION '0'
#define C_PR_BREAK1 '1'
#define C_PR_BREAK2 '2'
/*      C_PR_VERTOVERFLOW '3'        defined in constprs.h */
#define C_PR_LISTSTYLETYPE '4'
#define C_PR_LISTSTYLEIMAGE '5'
#define C_PR_LISTSTYLEPOSITION '6'
#define C_PR_TOP '7'
#define C_PR_RIGHT '8'
#define C_PR_BOTTOM '9'
#define C_PR_LEFT ':'
#define C_PR_VARIANT ';'
#define C_PR_BORDERLEFTSTYLE '<'
#define C_PR_BORDERRIGHTSTYLE '>'
#define C_PR_BACKGROUNDHORIZPOS '?'
#define C_PR_BACKGROUNDVERTPOS '@'
#define C_PR_ADJUST 'A'
/*      C_PR_PAGEBREAK 'B'           defined in constprs.h */
#define C_PR_HYPHENATE 'C'
#define C_PR_DEPTH 'D'
#define C_PR_UNDER_THICK 'E'
#define C_PR_FONT 'F'
/*      C_PR_GATHER 'G'              defined in constprs.h */
#define C_PR_HEIGHT 'H'
#define C_PR_LINESPACING 'I'
#define C_PR_DIRECTION 'J'
#define C_PR_UNICODEBIDI 'K'
#define C_PR_WIDTH 'L'
#define C_PR_XRADIUS 'M'
#define C_PR_YRADIUS 'N'
#define C_PR_WIDTHPOS 'O'
#define C_PR_PICTURE 'P'
#define C_PR_HEIGHTPOS 'Q'
#define C_PR_INDENT 'R'
#define C_PR_STYLE 'S'
#define C_PR_SIZE 'T'
#define C_PR_UNDERLINE 'U'
/*      C_PR_VISIBILITY 'V'           defined in constprs.h */
#define C_PR_LINEWEIGHT 'W'
#define C_PR_HPOS 'X'
#define C_PR_VPOS 'Y'
/*      C_PR_LINEBREAK 'Z'            defined in constprs.h */
#define C_PR_OPACITY '['
#define C_PR_STROKE_OPACITY '\\'
#define C_PR_FILL_OPACITY ']'
#define C_PR_BORDERTOPSTYLE '^'
#define C_PR_BORDERBOTTOMSTYLE '_'
#define C_PR_FILL_RULE '`'
#define C_PR_MARGINTOP 'a'
#define C_PR_BACKGROUND 'b'
#define C_PR_MARGINRIGHT 'c'
#define C_PR_MARGINBOTTOM 'd'
#define C_PR_MARGINLEFT 'e'
/*      C_PR_FUNCTION 'f'             defined in constprs.h */
#define C_PR_PADDINGTOP 'g'
/*      C_PR_HREF 'h'                 defined in constprs.h */
#define C_PR_PADDINGRIGHT 'i'
#define C_PR_PADDINGBOTTOM 'j'
#define C_PR_PADDINGLEFT 'k'
#define C_PR_LINESTYLE 'l'
#define C_PR_BORDERTOPWIDTH 'm'
#define C_PR_BORDERRIGHTWIDTH 'n'
/*      C_PR_HORIZOVERFLOW 'o'        defined in constprs.h */
#define C_PR_FILLPATTERN 'p'
#define C_PR_BORDERBOTTOMWIDTH 'q'
#define C_PR_BORDERLEFTWIDTH 'r'
#define C_PR_BORDERTOPCOLOR 's'
#define C_PR_FOREGROUND 't'
#define C_PR_BORDERRIGHTCOLOR 'u'
/*      C_PR_VREF 'v'                 defined in constprs.h */
#define C_PR_WEIGHT 'w'
#define C_PR_BORDERBOTTOMCOLOR 'x'
#define C_PR_BORDERLEFTCOLOR 'y'
#define C_PR_DISPLAY 'z'
#define C_PR_CLEAR '|'
#define C_PR_FLOAT '~'

/* more presentation rule types are defined in constprs.h */

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
#define C_PIV_JUSTIFY 'J'

/* dimension types codes */
#define C_PIV_ABSOLUTE 'A'
#define C_PIV_RELATIVE 'R'

/* units of measure codes */	
#define C_PIV_EM 'C'
#define C_PIV_PT 'P'
#define C_PIV_PX 'x'
#define C_PIV_PERCENT '%'
#define C_PIV_AUTO 'A'

/* arithmetic signs codes */
#define C_PIV_PLUS '+'
#define C_PIV_MINUS '-'
	
/* ThotBools codes */
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
