/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 

/*
 * Constants for documents presentation
 *
 */

#ifndef __CONST_PRS_H__
#define __CONST_PRS_H__

#define MAX_VIEW 5		/* max. number of views for a document */
#define MAX_PRINT_VIEW 10	/* max. number of views to print for a
				   presentation schema */
#define MAX_PRES_COUNTER 60	/* max. number of counters for an presentation
				   schema */
#define MAX_PRES_VARIABLE 180	/* max. number of variables for a presentation
				   schema */
#define MAX_PRES_CONST 1024	/* max number of constant strings for
				   presentation schema */
#define MAX_PRES_CONST_LEN 100	/* max. length of presentation string
				   constants */
#define MAX_PRES_VAR_ITEM 12	/* max. number of items making up a
				   presentation variable */
#define MAX_PRES_COUNT_ITEM 12	/* max. number of operations defined on a
				   counter */	
#define MAX_PRES_COUNT_USER 24	/* max. number of types of presentation boxes
				   using a counter */	
#define MAX_PRES_ATTR_CASE 8	/* max. number of different cases for
				   application of a numerical attribute
				   presentation rules */
#define MAX_PRES_BOX 1024	/* max. number of presentation boxes for a
				   presentation schema */
#define MAX_TRANSM_ATTR 5	/* max. number of attributes to which can be
				   transmitted a counter or an element value */
#define MAX_TRANSM_ELEM 25	/* max. number of elements that can pass on
				   their value to attributes of imported
				   documents */	
#define MAX_COLUMN_PAGE 4	/* max. number of columns for a page */
#define MAX_COUNTER_VAL 32767	/* max. value of a counter 2^15 - 1 */
#define MIN_COUNTER_VAL -32768	/* min. value of a counter -2^15 */

/*
 * Definition of the presentation schema coding produced by
 * the P language compiler
 *
 */

#if defined(_I18N_) || defined(__JIS__)

/* units of distance */
#define C_UNIT_REL            L'R'
#define C_UNIT_XHEIGHT        L'X'
#define C_UNIT_POINT          L'P'
#define C_UNIT_PIXEL          L'p'
#define C_UNIT_PERCENT        L'C'
#define C_UNIT_AUTO           L'A'

/* boxes edges and axis */ 
#define C_AX_TOP              L'T'
#define C_AX_BOTTOM           L'B'
#define C_AX_LEFT             L'G'
#define C_AX_RIGHT            L'D'
#define C_AX_HREF             L'h'
#define C_AX_VREF             L'v'
#define C_AX_HMIDDLE          L'H'
#define C_AX_VMIDDLE          L'V'
#define C_AX_NULL             L'0'

/* relative level */
#define C_PARENT              L'S'
#define C_SAME_LEVEL          L'M'
#define C_CHILD               L'I'
#define C_PREVIOUS            L'p'
#define C_NEXT                L's'
#define C_SELF                L'*'
#define C_CONTAINS_REF        L'R'
#define C_ROOT                L'r'
#define C_REFERRED            L'D'
#define C_CREATOR             L'C'

/* Kind of object used as a reference in a relative position */
#define C_KIND_ELEMENT_TYPE   L'E'
#define C_KIND_PRES_BOX       L'B'
#define C_KIND_ATTRIBUTE      L'A'

/* presentation box contents type */
/* type du contenu d'une boite de presentation */
#define C_CONT_FREE           L'L'
#define C_CONT_VAR            L'V'
#define C_CONT_CONST          L'C'
#define C_CONT_ELEM           L'E'

/* alignment */
/*    Defined in constpiv.h */
/* C_PIV_LEFT	= 'G'; */
/* C_PIV_RIGHT	= 'D'; */
/* C_PIV_CENTERED	= 'C'; */
/* C_PIV_LEFTDOT	= 'P'; */


/* presentation rule type */
#define C_PR_VERTOVERFLOW     L'O'
#define C_PR_VISIBILITY       L'V'
#define C_PR_FUNCTION         L'f'
#define C_PR_HREF             L'h'
#define C_PR_HORIZOVERFLOW    L'o'
#define C_PR_VREF             L'v'
/* other presentation rule types are defined in constpiv.h */

/* properties computing mode */
#define C_IMMEDIATE           L'V'
#define C_INHERIT             L'H'
#define C_PRES_FUNCTION       L'F'

/* inheritance type */
#define C_INH_ASCEND          L'A'
#define C_INH_PREVIOUS        L'P'
#define C_INH_DESC            L'D'
#define C_INH_CREATOR         L'C'
#define C_INH_GRAND_FATHER    L'G'

/* presentation function type */
#define C_PF_LINE             L'L'
#define C_PF_PAGE             L'P'
#define C_PF_CR_BEFORE        L'V'
#define C_PF_CR_WITH          L'W'
#define C_PF_CR_FIRST         L'F'
#define C_PF_CR_LAST          L'f'
#define C_PF_CR_AFTER         L'S'
#define C_PF_CR_ENCLOSING     L'E'
#define C_PF_CR_BEFORE_REP    L'a'
#define C_PF_CR_FIRST_REP     L'p'
#define C_PF_CR_LAST_REP      L'd'
#define C_PF_CR_AFTER_REP     L'e'
#define C_PF_CR_ENCLOSING_REP L'g'
#define C_PF_COLUMN           L'c'
#define C_PF_SUBCOLUMN        L's'
#define C_PF_COPY             L'C'
#define C_PF_REF_CONTENT      L'r'
#define C_PF_NOLINE           L'N'
#define C_PF_SHOWBOX          L'B'
#define C_PF_BGPICTURE        L'b'
#define C_PF_PICTUREMODE      L'M'
#define C_PF_NOTINLINE        L'n'

/* counters presentation style */
#define C_NUM_ARABIC          L'A'
#define C_NUM_ROMAN           L'R'
#define C_NUM_LOWER_ROMAN     L'r'
#define C_NUM_UPPERCASE       L'L'
#define C_NUM_LOWERCASE       L'l'

/* presentation variables type */
#define C_VAR_TEXT            L'C'
#define C_VAR_COUNTER         L'V'
#define C_VAR_ATTR_VAL        L'A'
#define C_VAR_DATE            L'D'
#define C_VAR_FDATE           L'F'
#define C_VAR_DOCNAME         L'N'
#define C_VAR_DIRNAME         L'M'
#define C_VAR_ELEMNAME        L'E'
#define C_VAR_ATTRNAME        L'T'
#define C_VAR_PAGENUMBER      L'P'

/* presentation rules application conditions type */
#define C_COND_FIRST          L'P'
#define C_COND_LAST           L'D'
#define C_COND_REFERRED       L'R'
#define C_COND_FIRSTREF       L'A'
#define C_COND_LASTREF        L'B'
#define C_COND_EXTREF         L'C'
#define C_COND_INTREF         L'E'
#define C_COND_COPYREF        L'F'
#define C_COND_ATTR           L'G'
#define C_COND_FIRST_ATTR     L'H'
#define C_COND_LAST_ATTR      L'J'
#define C_COND_USERPAGE       L'K'
#define C_COND_STARTPAGE      L'L'
#define C_COND_COMPPAGE       L'M'
#define C_COND_EMPTY          L'O'
#define C_COND_EVEN           L'p'
#define C_COND_ODD            L'i'
#define C_COND_ONE            L'1'
#define C_COND_INTER          L'I'
#define C_COND_ANCEST         L'a'
#define C_COND_ELEM           L'e'
#define C_COND_HAS_ATTR       L'h'
#define C_COND_NOCOND         L'S'
#define C_COND_DEFAULT        L'd'

/* type of operation on a counter */
#define C_CNT_SET             L'S'
#define C_CNT_ADD             L'A'
#define C_CNT_RANK            L'R'
#define C_CNT_RLEVEL          L'L'

/* type of numerical attribute values comparison */
#define C_COMP_CONST          L'c'
#define C_COMP_ATTR           L'a'

/* type of counters in variables */
#define C_VAL_MAX             L'A'
#define C_VAL_MIN             L'B'
#define C_VAL_CUR             L'C'

/* relation in the Within condition */
#define C_WITHIN_GT           L'S'
#define C_WITHIN_LT           L'I'
#define C_WITHIN_EQ           L'E'

#else /* defined(_I18N_) || defined(__JIS__) */

/* units of distance */
#define C_UNIT_REL            'R'
#define C_UNIT_XHEIGHT        'X'
#define C_UNIT_POINT          'P'
#define C_UNIT_PIXEL          'p'
#define C_UNIT_PERCENT        'C'
#define C_UNIT_AUTO           'A'

/* boxes edges and axis */ 
#define C_AX_TOP              'T'
#define C_AX_BOTTOM           'B'
#define C_AX_LEFT             'G'
#define C_AX_RIGHT            'D'
#define C_AX_HREF             'h'
#define C_AX_VREF             'v'
#define C_AX_HMIDDLE          'H'
#define C_AX_VMIDDLE          'V'
#define C_AX_NULL             '0'

/* relative level */
#define C_PARENT              'S'
#define C_SAME_LEVEL          'M'
#define C_CHILD               'I'
#define C_PREVIOUS            'p'
#define C_NEXT                's'
#define C_SELF                '*'
#define C_CONTAINS_REF        'R'
#define C_ROOT                'r'
#define C_REFERRED            'D'
#define C_CREATOR             'C'

/* Kind of object used as a reference in a relative position */
#define C_KIND_ELEMENT_TYPE   'E'
#define C_KIND_PRES_BOX       'B'
#define C_KIND_ATTRIBUTE      'A'

/* presentation box contents type */
/* type du contenu d'une boite de presentation */
#define C_CONT_FREE           'L'
#define C_CONT_VAR            'V'
#define C_CONT_CONST          'C'
#define C_CONT_ELEM           'E'

/* alignment */
/*    Defined in constpiv.h */
/* C_PIV_LEFT	= 'G'; */
/* C_PIV_RIGHT	= 'D'; */
/* C_PIV_CENTERED	= 'C'; */
/* C_PIV_LEFTDOT	= 'P'; */


/* presentation rule type */
#define C_PR_VERTOVERFLOW     'O'
#define C_PR_VISIBILITY       'V'
#define C_PR_FUNCTION         'f'
#define C_PR_HREF             'h'
#define C_PR_HORIZOVERFLOW    'o'
#define C_PR_VREF             'v'
/*  other presentation rule types are defined in constpiv.h */

/* properties computing mode */
#define C_IMMEDIATE           'V'
#define C_INHERIT             'H'
#define C_PRES_FUNCTION       'F'

/* inheritance type */
#define C_INH_ASCEND          'A'
#define C_INH_PREVIOUS        'P'
#define C_INH_DESC            'D'
#define C_INH_CREATOR         'C'
#define C_INH_GRAND_FATHER    'G'

/* presentation function type */
#define C_PF_LINE             'L'
#define C_PF_PAGE             'P'
#define C_PF_CR_BEFORE        'V'
#define C_PF_CR_WITH          'W'
#define C_PF_CR_FIRST         'F'
#define C_PF_CR_LAST          'f'
#define C_PF_CR_AFTER         'S'
#define C_PF_CR_ENCLOSING     'E'
#define C_PF_CR_BEFORE_REP    'a'
#define C_PF_CR_FIRST_REP     'p'
#define C_PF_CR_LAST_REP      'd'
#define C_PF_CR_AFTER_REP     'e'
#define C_PF_CR_ENCLOSING_REP 'g'
#define C_PF_COLUMN           'c'
#define C_PF_SUBCOLUMN        's'
#define C_PF_COPY             'C'
#define C_PF_REF_CONTENT      'r'
#define C_PF_NOLINE           'N'
#define C_PF_SHOWBOX          'B'
#define C_PF_BGPICTURE        'b'
#define C_PF_PICTUREMODE      'M'
#define C_PF_NOTINLINE        'n'

/* counters presentation style */
#define C_NUM_ARABIC          'A'
#define C_NUM_ROMAN           'R'
#define C_NUM_LOWER_ROMAN     'r'
#define C_NUM_UPPERCASE       'L'
#define C_NUM_LOWERCASE       'l'

/* presentation variables type */
#define C_VAR_TEXT            'C'
#define C_VAR_COUNTER         'V'
#define C_VAR_ATTR_VAL        'A'
#define C_VAR_DATE            'D'
#define C_VAR_FDATE           'F'
#define C_VAR_DOCNAME         'N'
#define C_VAR_DIRNAME         'M'
#define C_VAR_ELEMNAME        'E'
#define C_VAR_ATTRNAME        'T'
#define C_VAR_PAGENUMBER      'P'

/* presentation rules application conditions type */
#define C_COND_FIRST          'P'
#define C_COND_LAST           'D'
#define C_COND_REFERRED       'R'
#define C_COND_FIRSTREF       'A'
#define C_COND_LASTREF        'B'
#define C_COND_EXTREF         'C'
#define C_COND_INTREF         'E'
#define C_COND_COPYREF        'F'
#define C_COND_ATTR           'G'
#define C_COND_FIRST_ATTR     'H'
#define C_COND_LAST_ATTR      'J'
#define C_COND_USERPAGE       'K'
#define C_COND_STARTPAGE      'L'
#define C_COND_COMPPAGE       'M'
#define C_COND_EMPTY          'O'
#define C_COND_EVEN           'p'
#define C_COND_ODD            'i'
#define C_COND_ONE            '1'
#define C_COND_INTER          'I'
#define C_COND_ANCEST         'a'
#define C_COND_ELEM           'e'
#define C_COND_HAS_ATTR       'h'
#define C_COND_NOCOND         'S'
#define C_COND_DEFAULT        'd'

/* type of operation on a counter */
#define C_CNT_SET             'S'
#define C_CNT_ADD             'A'
#define C_CNT_RANK            'R'
#define C_CNT_RLEVEL          'L'

/* type of numerical attribute values comparison */
#define C_COMP_CONST          'c'
#define C_COMP_ATTR           'a'

/* type of counters in variables */
#define C_VAL_MAX             'A'
#define C_VAL_MIN             'B'
#define C_VAL_CUR             'C'

/* relation in the Within condition */
#define C_WITHIN_GT           'S'
#define C_WITHIN_LT           'I'
#define C_WITHIN_EQ           'E'

#endif /* defined(_I18N_) || defined(__JIS__) */

#endif /* __CONST_PRS_H__ */
