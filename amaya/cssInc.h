#ifndef CSS_INC_H
#define CSS_INC_H

#include "thot_gui.h"
#include "thot_sys.h"
#include "message.h"
#include "content.h"
#include "pschema.h"
#include "application.h"
#include "dialog.h"
#include "browser.h"
#include "interface.h"

#include "amaya.h"  
#include "p2css.h"
#include "genericdriver.h"
#include "HTML.h"
#include "HTMLstyle.h"
/*#include "css.h"*/


#ifdef __STDC__
void ParseHTMLStyleDeclaration(Element elem, char *attrstr, Document doc, PSchema gPres);
void ParseHTMLStyleSheet(char *fragment, Document doc, PSchema gPres);
int DebugPresent(Document doc,PSchema gPres,char *output_file);
void LoadUserStyleSheet(Document doc);
#else /* __STDC__*/
void ParseHTMLStyleDeclaration(/* elem, attrstr, doc, gPres */);
void ParseHTMLStyleSheet(/*char *fragment, Document doc, PSchema gPres*/);
int DebugPresent(/*Document doc,PSchema gPres,char *output_file*/);
void LoadUserStyleSheet(/* doc */);
#endif /* __STDC__*/

#define SKIP_BLANK(ptr) \
     { while (((*(ptr)) == ' ') || ((*(ptr)) == '\b') || \
              ((*(ptr)) == '\n') || ((*(ptr)) == '\r')) ptr++; }
	      
typedef enum {
  CSS_Unknown,		/* for detecting uninitialized fields */
  CSS_USER_STYLE,		/* the CSS associated to the browser */
  CSS_DOCUMENT_STYLE,	/* CSS set in the document header */
  CSS_EXTERNAL_STYLE,	/* external CSS referenced by the document */
  CSS_BROWSED_STYLE	/* an external CSS browsed but not linked */
} CSSCategory;

typedef enum {
  CSS_STATE_Unknown,	/* for detecting uninitialized fields */
  CSS_STATE_Modified,	/* the CSS associated to the browser */
  CSS_STATE_Unmodified	/* CSS set in the document header */
} CSSState;

typedef enum {
  CSS_BROWSE_None,	/* No browsing operation current */
  CSS_BROWSE_SaveAll,	/* Saving all modified CSS files */
  CSS_BROWSE_SaveAs,	/* Saving one CSS file to local filesystem */
  CSS_BROWSE_Loading 	/* Browsing local filesystem to find CSS files */
} CSSBrowseStatus;

typedef struct CSSInfo {
    struct CSSInfo *NextCSS;

    /* the CSS name */
    char *name;
    char *url;
    char *tempfile;
    CSSCategory category;
    CSSState state;

    /* the associated pSchema */
    PSchema pschema;
   
    /* documents using this CSS */
    Bool documents[DocumentTableLength + 1];

    /* The original CSS text. Needed for the Dismiss function */
    char *css_rule;

    /*
     * Extra informations needed to support presentation not
     * currently available at the P level.
     */
    int view_background_color;
    int magnification;
} CSSInfo, *CSSInfoPtr;


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











