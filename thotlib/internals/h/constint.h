/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 

/*
 * Constant declarations for internal representation of documents
 *
 */

#include "thot_sys.h"

#define MAX_ASSOC_DOC 8  	/* max. number of associated element types for
				   a document */
#define MAX_PARAM_DOC 7		/* max. number of parameters for a document */
#define MAX_NATURES 7		/* max. number of simultaneous natures */
#define MAX_PSCHEMAS 10		/* max. number of loaded presentation schemas*/
#define MAX_DOCUMENTS 10	/* max. number of simultaneous documents */
#define MAX_CHAR 100		/* max. number of character in a text buffer */
#define DEF_SAVE_INTVL 500	/* number of typed characters triggering 
				   automatic saving */
#define MAX_VIEW_DOC 6		/* max. number of views for a document */
#define MAX_LABEL_LEN 12       	/* max. length of labels */
#define MAX_SEL_ELEM 150	/* max. number of elements that can be selected
				   simultaneously */
#define MAX_VIEW_OPEN 50	/* max. number of open views for all documents */
#define MAX_DOC_NAME_LEN 32	/* max. length of a document name */
#define MAX_DOC_IDENT_LEN 32	/* max. length of a document identifier */

#include "conststr.h"
#include "constprs.h"

