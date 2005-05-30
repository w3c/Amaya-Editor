/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2005
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 
#ifndef _THOTLIB_CONSTMEDIA_H_
#define _THOTLIB_CONSTMEDIA_H_

/*
 * Constant declaration for document images
 *
 */

#include "constint.h"
#include "thotkey.h"

/* Constants for character strings management */
#define NUL 0
#define MAX_WORD_LEN       30	/* max. length of a word */

/* constant for managing awaiting rules during the abstract
   image construction */ 
#define MAX_QUEUE_LEN 40

/* Constants for documents images management */
#define FULL_BUFFER 99		/* Max. number of characters in buffers */
#define MAX_RELAT_POS 10	/* Number of position dependencies for a block */
#define MAX_RELAT_DIM 5		/* Number of dimension dependencies
				   for a block */
#define	HANDLE_WIDTH 5		/* Width of selection handles */	
#define DELTA_SEL 6             /* Accuracy of the selection */
#define EXTRA_GRAPH 10		/* Graphic boxes overflow */
#define MAX_HIGHLIGHT 8		/* Max. number of highlighting levels */
#define MAX_LOG_SIZE 30		/* Maximum logical size */
#define MAX_WINDOW 40		/* Maximum number of displayed windows */
#define MAX_WINDOW_PAGE 2	/* Maximum number of window pages (a page is a tab) */
#define MAX_FRAME 40		/* Maximum number of displayed views */
#define MAX_PANEL 40		/* Maximum number of displayed panels */
#define MAX_SIZE 65535		/* Maximum size of system structures */
#define MAX_UNDERLINE 4		/* Max. number of styles of underlining in use*/ 
#define MAX_UNDER_THICK 2	/* Maximum thickness of underlining */

/* Constants for catalogs management */
#define MAX_TXT_LEN 1024	/* Max. length of strings */
#define MAX_ENTRY 40		/* Max. number of entries for a catalog */
#define INTEGER 0
#define _STRING_ 1
#define CLICK 2
#define EMPTY 3
#define ENUMERATE 4
#define PRODUCT 5
#define UNION 6
#define OTHER   7
#define SHIFT_16 65536
#define CAT_IMAGE 2000
#define CAT_ZOOM 2001
#define CAT_VISIB 2002
#define CAT_OPTION 2003

/* Constants for commands and buttons management */
#define MAX_NCOMMANDS 15
#define MAX_ITEM_MSG_SEL 16
#define TEXT_INSERT 5
#define TEXT_CUT 6
#define TEXT_PASTE 7
#define TEXT_COPY 8
#define TEXT_DEL 9
#define TEXT_X_PASTE 10
#define TEXT_SUP 11

#endif /* _THOTLIB_CONSTMEDIA_H_ */
