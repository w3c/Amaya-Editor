/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

#ifndef CSS_INC_H
#define CSS_INC_H
#include "pschema.h"
#include "style.h"
 
#define MAX_CSS_LENGTH 4000

typedef enum
{
  CSS_Unknown,		/* for detecting uninitialized fields */
  CSS_USER_STYLE,	/* the CSS associated to the browser */
  CSS_DOCUMENT_STYLE,	/* CSS set in the document header */
  CSS_EXTERNAL_STYLE,	/* external CSS */
} CSSCategory;

typedef struct _PInfo
{
  struct _PInfo      *PiNext;
  SSchema             PiSSchema; /* the Structure Schem */
  Document            PiDoc;
  PSchema             PiPSchema; /* the Presentation Schema */
  Element             PiLink; /* the element whcih links this CSS */
} PInfo , *PInfoPtr;

typedef struct _CSSInfo
{
  struct _CSSInfo    *NextCSS;
  int                 doc;   /* entry in the document table or 0 */
  char               *url;
  char               *localName;
  CSSCategory         category;
  PInfoPtr            infos; /* the document Presentation Schemas */
  /* documents using this CSS */
  boolean             documents[DocumentTableLength];
} CSSInfo , *CSSInfoPtr;

THOT_EXPORT CSSInfoPtr   CSSList;
THOT_EXPORT STRING       UserCSS; /* name of the User style sheet */
THOT_EXPORT char         CSSbuffer[MAX_CSS_LENGTH + 1];

#endif /* CSS_INC_H */
