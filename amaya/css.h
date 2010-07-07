/*
 *
 *  (c) COPYRIGHT INRIA and W3C, 1996-2010
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
  CSS_USER_STYLE,	/* the CSS associated to the browser  */
  CSS_DOCUMENT_STYLE,	/* a <style> element in the document  */
  CSS_EXTERNAL_STYLE,	/* an external CSS                    */
  CSS_IMPORT,           /* an imported CSS                    */
  CSS_EMBED             /* CSS of an object element           */
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
  struct _PInfo      *PiNext;     /* next link in the same document */
  Element             PiLink;     /* the element which links this CSS */
  CSSCategory         PiCategory; /* the category of this CSS link */
  ThotBool            PiEnabled;  /* enabled/disabled */
  CSSmedia            PiMedia;    /* for what media */
  PISchemaPtr         PiSchemas;  /* list of schemas */
} PInfo , *PInfoPtr;

typedef struct _CSSInfo
{
  struct _CSSInfo    *NextCSS;
  int                 doc;   /* document which displays the CSS file */
  char               *url;
  char               *localName;
  PInfoPtr            infos[DocumentTableLength]; /* documents using this CSS */
  char               *class_list; /* store the list of defined classes */
  int                 lg_class_list;
  ThotBool            import; /* import entry */
} CSSInfo , *CSSInfoPtr;

THOT_EXPORT CSSInfoPtr   CSSList;
THOT_EXPORT char        *UserCSS; /* name of the User style sheet */
THOT_EXPORT ThotBool     DoDialog; /* TRUE when CSS rules are parsed to update the CSS dialog */

#endif /* CSS_INC_H */
