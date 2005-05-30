/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2005
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * Constant declarations for internal representation of documents
 *
 */

#ifndef __CONST_INT_H__
#define __CONST_INT_H__

#include "thot_sys.h"

#ifdef _WINGUI

#define MAX_NATURES 7		/* max. number of simultaneous natures */
#define MAX_VIEW_DOC 6		/* max. number of views for a document */
#define MAX_SEL_ELEM 150	/* max. number of elements that can be selected
				   simultaneously */
#define MAX_VIEW_OPEN 50	/* max. number of open views for all documents */
#else /* _WINGUI ---------------------------------- */

#define MAX_NATURES 7		/* max. number of simultaneous natures */
#define MAX_VIEW_DOC 6		/* max. number of views for a document */
#define MAX_SEL_ELEM 150	/* max. number of elements that can be selected
				   simultaneously */
#define MAX_VIEW_OPEN 50	/* max. number of open views for all documents */
#endif /* _WINGUI */

#define THOT_MAX_CHAR 100	/* max. number of character in a text buffer */
#define MAX_LABEL_LEN 12       	/* max. length of labels */
#define MAX_DOC_IDENT_LEN 32	/* max. length of a document identifier */

/* document structure checking modes */
#define DEFAULT_CHECK_MASK '\xE0'   /* default : check struct, mandatory el and attr */
#define COMPLETE_CHECK_MASK '\xE0'  /* complete: idem, for compatibility */
#define STR_CHECK_MASK '\x80'       /* check structural relations */
#define EL_MANDATORY_MASK '\x40'    /* check mandatory elements */
#define ATTR_MANDATORY_MASK '\x20'  /* check mandatory attributes */
#define PIV_CHECK_MASK '\x01'       /* check document against SSchema (pivot reading) */

#include "conststr.h"
#include "constprs.h"

#endif /* __CONST_INT_H__ */
