/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

#ifndef CSS_INC_H
#define CSS_INC_H
#include "pschema.h"
#include "genericdriver.h"
#include "specificdriver.h"
 
typedef enum
  {
     UnknownRPI,
     NormalRPI,
     ModifiedRPI,
     RemovedRPI
  }
RPIstate;
 
typedef struct PRuleInfo
  {
     struct PRuleInfo   *NextRPI;
 
     RPIstate            state;
 
     PSchema             pschema;
 
     GenericContext      ctxt;
 
     /* the CSS rule */
     char               *selector;
     char               *css_rule;
  }
PRuleInfo          , *PRuleInfoPtr;


#define SKIP_BLANK(ptr) \
     { while (((*(ptr)) == ' ') || ((*(ptr)) == '\b') || \
              ((*(ptr)) == '\n') || ((*(ptr)) == '\r')) ptr++; }

typedef enum
  {
     CSS_Unknown,		/* for detecting uninitialized fields */
     CSS_USER_STYLE,		/* the CSS associated to the browser */
     CSS_DOCUMENT_STYLE,	/* CSS set in the document header */
     CSS_EXTERNAL_STYLE,	/* external CSS referenced by the document */
     CSS_BROWSED_STYLE		/* an external CSS browsed but not linked */
  }
CSSCategory;

typedef enum
  {
     CSS_STATE_Unknown,		/* for detecting uninitialized fields */
     CSS_STATE_Modified,	/* the CSS associated to the browser */
     CSS_STATE_Unmodified	/* CSS set in the document header */
  }
CSSState;

typedef enum
  {
     CSS_BROWSE_None,		/* No browsing operation current */
     CSS_BROWSE_SaveAll,	/* Saving all modified CSS files */
     CSS_BROWSE_SaveAs,		/* Saving one CSS file to local filesystem */
     CSS_BROWSE_Loading		/* Browsing local filesystem to find CSS files */
  }
CSSBrowseStatus;

typedef struct CSSInfo
  {
     struct CSSInfo     *NextCSS;

     /* the CSS name */
     char               *name;
     char               *url;
     char               *tempfile;
     CSSCategory         category;
     CSSState            state;

     /* the associated pSchema */
     PSchema             pschema;

     /* documents using this CSS */
     boolean                documents[DocumentTableLength + 1];

     /* The original CSS text. Needed for the Dismiss function */
     char               *css_rule;

     /*
      * Extra informations needed to support presentation not
      * currently available at the P level.
      */
     int                 view_background_color;
     int                 magnification;
  }
CSSInfo            , *CSSInfoPtr;


/************************************************************************
 *									*
 *	Constants and variables needed to build the CSS Dialogs		*
 *									*
 ************************************************************************/

#define FormCSS		1
#define CSSSelect	2
#define CSSRName	3
#define CSSLabel	4
#define CSSLName	5
#define RPILList	6
#define RPIActions	7
#define RPIRList	8
#define RPIText		9
#define FormExternalCSS 10
#define ListExternalCSS 11
#define CSSFormSauver	12
#define CSSSauvDir	13
#define CSSSauvDoc	14
#define CSSNomURL	15
#define CSSFormConfirm	16
#define CSSTextConfirm	17
#define FormDeleteCSS	18
#define ListDeleteCSS 19

#define NB_CSS_DIALOGS	20

/*
 * Parameters for the CSS History, size and filename (UNIX) !
 * First string is home directory, second is the application name
 */

#define CSS_HISTORY_FILE "%s/.%s/history.css"
#define CSS_HISTORY_SIZE 50

#define HTML_HISTORY_FILE "%s/.%s/history.html"
#define HTML_HISTORY_SIZE 50

#endif /* CSS_INC_H */
