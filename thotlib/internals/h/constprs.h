
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */
 

/*
 * Constantes pour la presentation des documents
 * V. Quint	Juin 1984
 *
 */



#define MAX_VIEW 5		/* nombre max. de vues pour un document */
#define MAX_PRINT_VIEW 10	/* nombre max de vues a imprimer pour un
				   schema de presentation */
#define MAX_PRES_COUNTER 60		/* nombre max. de compteurs pour une repres.
				   intermediaire */
#define MAX_PRES_VARIABLE 120		/* nombre max. de variables pour une repres.
				   intermediaire */
#define MAX_PRES_CONST 1024	/* nombre max. de chaines constantes pour
				   presentation */
#define MAX_PRES_CONST_LEN 100		/* longueur max. des chaines constantes de
				   presentation */
#define MAX_PRES_VAR_ITEM 12	/* nombre max d'elements constituant une var.
				   de presentation */
#define MAX_PRES_COUNT_ITEM 12		/* nombre max. d'operations definies sur un
				   compteur */
#define MAX_PRES_COUNT_USER 24		/* nombre max. de types de boites de
				   presentation utilisant un compteur */
#define MAX_PRES_ATTR_CASE 8		/* nombre max de cas differents pour
				   l'application des regles de presentation
				   d'un attribut numerique */
#define MAX_PRES_BOX 1024		/* nombre max de boites de presentation et
				   numerotation par schema de presentation */

#define MAX_TRANSM_ATTR 5		/* nombre max d'attributs auxquels on peut */
				/* transmettre la valeur d'un compteur ou */
				/* d'un element */
#define MAX_TRANSM_ELEM 25		/* nombre max d'elements qui peuvent */
				/* transmettre leur valeur a des attributs */
				/* de documents importes */
#define MAX_COLUMN_PAGE 4		/* nombre max de colonne par page */
#define MAX_COUNTER_VAL 32767	/* valeur maximale d'un compteur 2^15 - 1*/
#define MIN_COUNTER_VAL -32768	/* valeur minimale d'un compteur -2^15 */

/* 
  Definition du codage des tables de presentation produites par
  le compilateur du langage P.
 */

/* unites de distance */
#define C_UNIT_REL 'R'
#define C_UNIT_XHEIGHT 'X'
#define C_UNIT_POINT 'P'
#define C_UNIT_PIXEL 'p'
#define C_UNIT_PERCENT 'C'

/* cotes et axes des boites */
#define C_AX_TOP 'T'
#define C_AX_BOTTOM 'B'
#define C_AX_LEFT 'G'
#define C_AX_RIGHT 'D'
#define C_AX_HREF 'h'
#define C_AX_VREF 'v'
#define C_AX_HMIDDLE 'H'
#define C_AX_VMIDDLE 'V'
#define C_AX_NULL '0'

/* niveau relatif */
#define C_PARENT 'S'
#define C_SAME_LEVEL 'M'
#define C_CHILD 'I'
#define C_PREVIOUS 'p'
#define C_NEXT 's'
#define C_SELF '*'
#define C_CONTAINS_REF 'R'
#define C_ROOT 'r'
#define C_REFERRED 'D'

#define C_CREATOR 'C'

/* type du contenu d'une boite de presentation */
#define C_CONT_FREE 'L'
#define C_CONT_VAR 'V'
#define C_CONT_CONST 'C'
#define C_CONT_ELEM 'E'

/* cadrage des lignes dans un pave mis en ligne */
/***	Definis dans constpiv.h ****/
/* C_PIV_LEFT	= 'G'; */
/* C_PIV_RIGHT	= 'D'; */
/* C_PIV_CENTERED	= 'C'; */
/* C_PIV_LEFTDOT	= 'P'; */


/* type de regle de presentation */
#define C_PR_VISIBILITY 'V'
#define C_PR_FUNCTION 'f'
#define C_PR_VREF 'v'
#define C_PR_HREF 'h'
#define C_PR_DEPTH 'P'
/***	Definis dans constpiv.h  ****/
/* C_PR_HEIGHT	= 'H'; */
/* C_PR_WIDTH	= 'L'; */
/* C_PR_VPOS	= 'Y'; */
/* C_PR_HPOS	= 'X'; */
/* C_PR_SIZE	= 'T'; */
/* C_PR_STYLE	= 'S'; */
/* C_PR_FONT	= 'F'; */
/* C_PR_BREAK1	= '1'; */
/* C_PR_BREAK@	= '2'; */
/* C_PR_INDENT	= 'R'; */
/* C_PR_LINESPACING	= 'I'; */
/* C_PR_JUSTIFY	= 'J'; */
/* C_PR_HYPHENATE= 'C'; */


/* mode de calcul des proprietes */
#define C_IMMEDIATE 'V'
#define C_INHERIT 'H'
#define C_PRES_FUNCTION 'F'

/* type d'heritage */
#define C_INH_ASCEND 'A'
#define C_INH_PREVIOUS 'P'
#define C_INH_DESC 'D'
#define C_INH_CREATOR 'C'
#define C_INH_GRAND_FATHER 'G'

/* type de fonction de presentation */
#define C_PF_LINE 'L'
#define C_PF_PAGE 'P'
#define C_PF_CR_BEFORE 'V'
#define C_PF_CR_WITH 'W'
#define C_PF_CR_FIRST 'F'
#define C_PF_CR_LAST 'f'
#define C_PF_CR_AFTER 'S'
#define C_PF_CR_ENCLOSING 'E'
#define C_PF_CR_BEFORE_REP 'a'
#define C_PF_CR_FIRST_REP 'p'
#define C_PF_CR_LAST_REP 'd'
#define C_PF_CR_AFTER_REP 'e'
#define C_PF_CR_ENCLOSING_REP 'g'
#define C_PF_COLUMN 'c'
#define C_PF_SUBCOLUMN 's'
#define C_PF_COPY 'C'
#define C_PF_REF_CONTENT 'r'
#define C_PF_NOLINE 'N'

/* style de presentation pour les chiffres des compteurs */
#define C_NUM_ARABIC 'A'
#define C_NUM_ROMAN 'R'
#define C_NUM_LOWER_ROMAN 'r'
#define C_NUM_UPPERCASE 'L'
#define C_NUM_LOWERCASE 'l'

/* type des variables de presentation */
#define C_VAR_TEXT 'C'
#define C_VAR_COUNTER 'V'
#define C_VAR_ATTR_VAL 'A'
#define C_VAR_DATE 'D'
#define C_VAR_FDATE 'F'
#define C_VAR_DOCNAME 'N'
#define C_VAR_DIRNAME 'M'
#define C_VAR_ELEMNAME 'E'
#define C_VAR_ATTRNAME 'T'
#define C_VAR_PAGENUMBER 'P'

/* type de condition d'application de regles de presentation */
#define C_COND_FIRST 'P'
#define C_COND_LAST 'D'
#define C_COND_REFERRED 'R'
#define C_COND_FIRSTREF 'A'
#define C_COND_LASTREF 'B'
#define C_COND_EXTREF 'C'
#define C_COND_INTREF 'E'
#define C_COND_COPYREF 'F'
#define C_COND_ATTR 'G'
#define C_COND_FIRST_ATTR 'H'
#define C_COND_LAST_ATTR 'J'
#define C_COND_USERPAGE 'K'
#define C_COND_STARTPAGE 'L'
#define C_COND_COMPPAGE 'M'
#define C_COND_EMPTY 'O'
#define C_COND_EVEN 'p'
#define C_COND_ODD 'i'
#define C_COND_ONE '1'
#define C_COND_INTER 'I'
#define C_COND_ANCEST 'a'
#define C_COND_ELEM 'e'
#define C_COND_HAS_ATTR 'h'
#define C_COND_NOCOND 'S'
#define C_COND_DEFAULT 'd'

/* type d'operation sur un compteur */
#define C_CNT_SET 'S'
#define C_CNT_ADD 'A'
#define C_CNT_RANK 'R'
#define C_CNT_RLEVEL 'L'

/* type de comparaison pour les valeurs
 * d'un attribut numerique */
#define C_COMP_CONST 'c'
#define C_COMP_ATTR 'a'

/* nature des compteurs dans les variables */
#define C_VAL_MAX 'A'
#define C_VAL_MIN 'B'
#define C_VAL_CUR 'C'

/* relation dans la condition Within */
#define C_WITHIN_GT 'S'
#define C_WITHIN_LT 'I'
#define C_WITHIN_EQ 'E'
