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

typedef enum
{
  CSS_ALL,	/* screen and print */
  CSS_SCREEN,	/* screen only */
  CSS_PRINT,	/* print only */
  CSS_OTHER,	/* aural or other */
} CSSmedia;

typedef struct _PISchema
{
  struct _PISchema   *PiSNext;
  SSchema             PiSSchema; /* the Structure Schema */
  PSchema             PiPSchema; /* the Presentation Schema */
} PISchema , *PISchemaPtr;

typedef struct _PInfo
{
  struct _PInfo      *PiNext;
  Document            PiDoc;
  Element             PiLink; /* the element whcih links this CSS */
  PISchemaPtr         PiSchemas; /* list of schemas */
} PInfo , *PInfoPtr;

typedef struct _CSSInfo
{
  struct _CSSInfo    *NextCSS;
  int                 doc;   /* entry in the document table or 0 */
  STRING              url;
  STRING              localName;
  CSSCategory         category;
  PInfoPtr            infos; /* the document Presentation Schemas */
  /* documents using this CSS */
  ThotBool            documents[DocumentTableLength]; /* document concerned */
  ThotBool            enabled[DocumentTableLength];   /* enabled/disabled */
  CSSmedia            media[DocumentTableLength];     /* for what media */
} CSSInfo , *CSSInfoPtr;

THOT_EXPORT CSSInfoPtr   CSSList;
THOT_EXPORT CHAR_T*      UserCSS; /* name of the User style sheet */
THOT_EXPORT CHAR_T       CSSbuffer[MAX_CSS_LENGTH + 1];

#endif /* CSS_INC_H */
