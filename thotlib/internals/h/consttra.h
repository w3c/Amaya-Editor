/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 
#ifndef _THOTLIB_CONSTTRA_H_
#define _THOTLIB_CONSTTRA_H_

/*
 * Constants for documents translation
 *
 */

#define MAX_TRANSL_COUNTER 30	/* max. number of counters for a translation
				   schema */
#define MAX_TRANSL_CONST 2048	/* max. number of constants for translation
				   schema */
#define MAX_TRANSL_CONST_LEN 24000	/* max. total length of translation
					   constants */
#define MAX_TRANSL_VARIABLE 50	/* max. number of variables for a translation
				   schema */	
#define MAX_TRANSL_VAR_ITEM 12	/* max. number of items making up a
				   translation variable */
#define MAX_TRANSL_BUFFER 10	/* max. number of buffers for a translation
				   schema */
#define MAX_TRANSL_BUFFER_LEN 80	/* maximum buffer length */
#define MAX_TRANSL_ALPHABET 4	/* max. number of translated alphabets */
#define MAX_TRANSL_CHAR 512	/* max. number of entries in the character
				   translation table */
#define MAX_SRCE_LEN 15		/* max. length of character strings to
				   translate */
#define MAX_TARGET_LEN 128	/* max. length of translated character
				   strings */
#define MAX_TRANSL_ATTR_CASE 5	/* max. number of different cases for the
				   application of a numerical attribute
				   translation rules */
#define MAX_TRANSL_COND 10	/* max. number of conditions controlling the
				   application of a translation rules block */
#define MAX_TRANSL_PRULE 30	/* number of presentation rules types to which
				   translations can be associated */
#define MAX_TRANSL_PRES_CASE 20	/* max number of different cases for the
				   application of translation rules of a
				   numerical valued presentation rule */
#define MAX_TRANSL_PRES_VAL 5	/* maximum number of different values for a
				   non-numerical presentation rule */
#define MAX_EOL_STRING_LEN  16	/* max length of a character string marking the
				   end of line in the translated file */

/* definition for the coding of translation tables produced by the
   T language compiler */ 

#if defined(_I18N_) || defined(__JIS__)

/* a type of translation rule application condition */
#define C_TR_FIRST L'P'
#define C_TR_LAST L'D'
#define C_TR_DEFINED L'd'
#define C_TR_REFERRED L'R'
#define C_TR_FIRSTREF L'F'
#define C_TR_LAST_REF L'L'
#define C_TR_WITHIN L'T'
#define C_TR_FIRST_WITHIN L'f'
#define C_TR_ATTRIBUTE L'A'
#define C_TR_PRESENT L'S'
#define C_TR_PRULE L's'
#define C_TR_COMMENT L'c'
#define C_TR_ALPHABET L'a'
#define C_TR_ATTRIBUTES L'E'
#define C_TR_FIRSTATTR L'p'
#define C_TR_LASTATTR L'l'
#define C_TR_COMPUTEDPAGE L'w'
#define C_TR_STARTPAGE L'x'
#define C_TR_USERPAGE L'y'
#define C_TR_REMINDERPAGE L'z'
#define C_TR_EMPTY L'v'
#define C_TR_EXTERNALREF L'e'
#define C_TR_ELEMENTTYPE L't'

/* a translation rule type */
#define C_TR_CREATE L'C'
#define C_TR_GET L'G'
#define C_TR_USE L'U'
#define C_TR_REMOVE L'R'
#define C_TR_WRITE L'W'
#define C_TR_READ L'r'
#define C_TR_INCLUDE L'I'
#define C_TR_NOTRANSL L'N'
#define C_TR_COPY L'c'
#define C_TR_CHANGEFILE L'F'
#define C_TR_SET_COUNTER L'S'
#define C_TR_ADD_COUNTER L'A'
#define C_TR_NOLINEBREAK L'B'
#define C_TR_INDENT L'i'
#define C_TR_REMOVEFILE L'f'

/* position where the translator-produced chains must be created */
#define C_TR_AFTER L'A'
#define C_TR_BEFORE L'B'

/* type of thing to create in the output */
#define C_OB_CONST L'C'
#define C_OB_BUFFER L'B'
#define C_OB_VAR L'V'
#define C_OB_ATTR L'A'
#define C_OB_CONTENT L'D'
#define C_OB_PRES_VAL L'P'
#define C_OB_PRESENTATION L'p'
#define C_OB_COMMENT L'c'
#define C_OB_ATTRIBUTES L'T'
#define C_OB_REFID L'R'
#define C_OB_PAIRID L'M'
#define C_OB_REFERRED_ELEM L'r'
#define C_OB_FILEDIR L'd'
#define C_OB_FILENAME L'f'
#define C_OB_EXTENSION L'E'
#define C_OB_DOCUMENTNAME L'N'
#define C_OB_DOCUMENTDIR L'I'
#define C_OB_REFERRED_DOCNAME L'n'
#define C_OB_REFERRED_DOCDIR L'i'
#define C_OB_REFERRED_REFID L'F'
#define C_OB_TRANSLATED_ATT L't'

/* relative position in the abstract tree of an element to get */
#define C_TR_SIBLING L'F'
#define C_TR_DESCEND L'D'
#define C_TR_REFERRED_EL L'd'
#define C_TR_ASSOC L'A'

/* type of an output indentation rule */
#define C_TR_ABSOLUTE L'A'
#define C_TR_RELATIVE L'R'
#define C_TR_SUSPEND L'S'
#define C_TR_RESUME L'r'

/* type of a counter operation */
#define C_TR_RANK L'R'
#define C_TR_RLEVEL L'L'
#define C_TR_SET L'S'
#define C_TR_NOOP L'N'

/* types of translation variables item */
#define C_TR_CONST L'T'
#define C_TR_COUNTER L'C'
#define C_TR_BUFFER L'B'
#define C_TR_ATTR L'A'
#define C_TR_FILEDIR L'd'
#define C_TR_FILENAME L'F'
#define C_TR_EXTENSION L'E'
#define C_TR_DOCUMENTNAME L'N'
#define C_TR_DOCUMENTDIR L'D'

/* to interpret the field TcAscendLevel */
#define C_WITHIN_GT L'S'
#define C_WITHIN_LT L'I'
#define C_WITHIN_EQ L'E'

#else /* defined(_I18N_) || defined(__JIS__) */

/* a type of translation rule application condition */
#define C_TR_FIRST 'P'
#define C_TR_LAST 'D'
#define C_TR_DEFINED 'd'
#define C_TR_REFERRED 'R'
#define C_TR_FIRSTREF 'F'
#define C_TR_LAST_REF 'L'
#define C_TR_WITHIN 'T'
#define C_TR_FIRST_WITHIN 'f'
#define C_TR_ATTRIBUTE 'A'
#define C_TR_PRESENT 'S'
#define C_TR_PRULE 's'
#define C_TR_COMMENT 'c'
#define C_TR_ALPHABET 'a'
#define C_TR_ATTRIBUTES 'E'
#define C_TR_FIRSTATTR 'p'
#define C_TR_LASTATTR 'l'
#define C_TR_COMPUTEDPAGE 'w'
#define C_TR_STARTPAGE 'x'
#define C_TR_USERPAGE 'y'
#define C_TR_REMINDERPAGE 'z'
#define C_TR_EMPTY 'v'
#define C_TR_EXTERNALREF 'e'
#define C_TR_ELEMENTTYPE 't'

/* a translation rule type */
#define C_TR_CREATE 'C'
#define C_TR_GET 'G'
#define C_TR_USE 'U'
#define C_TR_REMOVE 'R'
#define C_TR_WRITE 'W'
#define C_TR_READ 'r'
#define C_TR_INCLUDE 'I'
#define C_TR_NOTRANSL 'N'
#define C_TR_COPY 'c'
#define C_TR_CHANGEFILE 'F'
#define C_TR_SET_COUNTER 'S'
#define C_TR_ADD_COUNTER 'A'
#define C_TR_NOLINEBREAK 'B'
#define C_TR_INDENT 'i'
#define C_TR_REMOVEFILE 'f'

/* position where the translator-produced chains must be created */
#define C_TR_AFTER 'A'
#define C_TR_BEFORE 'B'

/* type of thing to create in the output */
#define C_OB_CONST 'C'
#define C_OB_BUFFER 'B'
#define C_OB_VAR 'V'
#define C_OB_ATTR 'A'
#define C_OB_CONTENT 'D'
#define C_OB_PRES_VAL 'P'
#define C_OB_PRESENTATION 'p'
#define C_OB_COMMENT 'c'
#define C_OB_ATTRIBUTES 'T'
#define C_OB_REFID 'R'
#define C_OB_PAIRID 'M'
#define C_OB_REFERRED_ELEM 'r'
#define C_OB_FILEDIR 'd'
#define C_OB_FILENAME 'f'
#define C_OB_EXTENSION 'E'
#define C_OB_DOCUMENTNAME 'N'
#define C_OB_DOCUMENTDIR 'I'
#define C_OB_REFERRED_DOCNAME 'n'
#define C_OB_REFERRED_DOCDIR 'i'
#define C_OB_REFERRED_REFID 'F'
#define C_OB_TRANSLATED_ATT 't'

/* relative position in the abstract tree of an element to get */
#define C_TR_SIBLING 'F'
#define C_TR_DESCEND 'D'
#define C_TR_REFERRED_EL 'd'
#define C_TR_ASSOC 'A'

/* type of an output indentation rule */
#define C_TR_ABSOLUTE 'A'
#define C_TR_RELATIVE 'R'
#define C_TR_SUSPEND 'S'
#define C_TR_RESUME 'r'

/* type of a counter operation */
#define C_TR_RANK 'R'
#define C_TR_RLEVEL 'L'
#define C_TR_SET 'S'
#define C_TR_NOOP 'N'

/* types of translation variables item */
#define C_TR_CONST 'T'
#define C_TR_COUNTER 'C'
#define C_TR_BUFFER 'B'
#define C_TR_ATTR 'A'
#define C_TR_FILEDIR 'd'
#define C_TR_FILENAME 'F'
#define C_TR_EXTENSION 'E'
#define C_TR_DOCUMENTNAME 'N'
#define C_TR_DOCUMENTDIR 'D'

/* to interpret the field TcAscendLevel */
#define C_WITHIN_GT 'S'
#define C_WITHIN_LT 'I'
#define C_WITHIN_EQ 'E'

#endif /* defined(_I18N_) || defined(__JIS__) */
#endif /* _THOTLIB_CONSTTRA_H_ */
