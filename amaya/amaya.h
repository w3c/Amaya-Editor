/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

#ifndef AMAYA_H
#define AMAYA_H

/* Thot interface */
#include "thot_gui.h"
#include "thot_sys.h"
#include "app.h"
#include "application.h"
#include "attribute.h"
#include "browser.h"
#include "content.h"
#include "dialog.h"
#include "interface.h"
#include "libmsg.h"
#include "message.h"
#include "presentation.h"
#include "selection.h"
#include "reference.h"
#include "tree.h"
#include "view.h"

/* Included headerfiles */
#include "EDITOR.h"
#include "HTML.h"
#include "amayamsg.h"

#define MAX_LENGTH     512
#define NAME_LENGTH     32
#define HTAppName "amaya"
#define HTAppVersion "V1.0 Beta"

/* How are Network accesses provided ? */
#ifdef AMAYA_JAVA
#include "libjava.h"
#else
#include "libwww.h"
#endif

typedef char        PathBuffer[MAX_PATH];

/* The different events for a DoubleClick */

typedef enum _DoubleClickEvent
  {
     DC_FALSE = 0,
     DC_TRUE = 1,
     DC_FORM_POST = 2,
     DC_FORM_GET = 4
  }
DoubleClickEvent;

typedef char        AmayaReadChar ();

/*typedef void        *PresentationTarget; */

#define NO               0
#define YES              1

/* dialogue */
#define URLForm          1
#define OpenForm       2
#define URLName           3
#define LocalName         4
#define DirSelect           5
#define DocSelect           6
#define StopCommand      7
#define SaveForm       8
#define DirSave          9
#define DocSave         10
#define ToggleSave      11
#define NameSave         12
#define Lbl1Save        13
#define Lbl2Save        14
#define ImgDirSave     14
#define Lbl3Save        15
#define Lbl4Save        16
#define Lbl5Save        17
#define ConfirmForm   18
#define ConfirmText  19
#define AttrHREFForm    20
#define AttrHREFText    21
#define FormAnswer      22
#define TextLabel       23
#define AnswerText      24
#define NameText        25
#define PasswordText    26
#define ClassForm       27
#define ClassLabel      28
#define ClassSelect     29
#define AClassForm      30
#define AClassLabel     31
#define AClassSelect    32
#define ConfirmSave     33
#define ConfirmSaveLbl  34
#define ConfirmSaveList 35
#define OptionMenu	36
#define MAX_REF         40

/* The possible GET/POST/PUT request modes */

/*synchronous request*/
#define AMAYA_SYNC	1	/*0x000001 */  
/*synchronous request with incremental callbacks */
#define AMAYA_ISYNC	2	/*0x000010 */  
/*asynchronous request */
#define AMAYA_ASYNC	4	/*0x000100 */
/*asynchronous request with incremental callbacks */
#define AMAYA_IASYNC	8	/*0x001000 */
/* send the form using the POST HTTP method */
#define AMAYA_FORM_POST 16	/*0x010000 */
/* send the form using the GET HTTP method */
#define AMAYA_FORM_GET  32	/*0x100000 */
/* bypass caching */
#define AMAYA_NOCACHE	64
/* don't follow redirections */
#define AMAYA_NOREDIR	128

/*
 * Flags for HTParse, specifying which parts of the URL are needed
 */
#define AMAYA_PARSE_ACCESS      16  /* Access scheme, e.g. "HTTP" */
#define AMAYA_PARSE_HOST        8   /* Host name, e.g. "www.w3.org" */
#define AMAYA_PARSE_PATH        4   /* URL Path, e.g. "pub/WWW/TheProject.html" */
#define AMAYA_PARSE_ANCHOR      2   /* Fragment identifier, e.g. "news" */
#define AMAYA_PARSE_PUNCTUATION 1   /* Include delimiters, e.g, "/" and ":" */
#define AMAYA_PARSE_ALL         31  /* All the parts */

THOT_EXPORT int          appArgc;
THOT_EXPORT char       **appArgv;
THOT_EXPORT char         TempFileDirectory[MAX_LENGTH];
THOT_EXPORT char         Answer_text[MAX_LENGTH];
THOT_EXPORT char         Answer_name[NAME_LENGTH];
THOT_EXPORT char         Answer_password[NAME_LENGTH];
THOT_EXPORT char         Display_password[NAME_LENGTH];
THOT_EXPORT int          Lg_password;
THOT_EXPORT int          BaseDialog;
THOT_EXPORT char        *LastURLName;	/* last URL requested               */
THOT_EXPORT char        *DirectoryName;	/* local path of the document       */
THOT_EXPORT char        *DocumentName;	/* document name                    */
THOT_EXPORT char        *ObjectName;		/* document name                    */
THOT_EXPORT char        *SaveImgsURL;	/* where to save remote Images      */
THOT_EXPORT char        *TargetName;
THOT_EXPORT boolean	 SaveAsHTML;
THOT_EXPORT boolean	 SaveAsText;
THOT_EXPORT int          CopyImages;		/* should we copy images in Save As */
THOT_EXPORT int          UpdateURLs;		/* should we update URLs in Save As */
THOT_EXPORT boolean      UserAnswer;
THOT_EXPORT int          ReturnOption;
THOT_EXPORT boolean      InNewWindow;
THOT_EXPORT Document     CurrentDocument;
THOT_EXPORT Document     SavingDocument;
THOT_EXPORT Document     SavingObject;
THOT_EXPORT char        *SavingFile;		/* complete path or URL of the document */
THOT_EXPORT Document     AttrHREFdocument;
THOT_EXPORT Element      AttrHREFelement;
THOT_EXPORT char        *AttrHREFvalue;
THOT_EXPORT Document     SelectionDoc;
THOT_EXPORT boolean      SelectionInPRE;
THOT_EXPORT boolean      SelectionInComment;
THOT_EXPORT boolean      SelectionInEM;
THOT_EXPORT boolean      SelectionInSTRONG;
THOT_EXPORT boolean      SelectionInCITE;
THOT_EXPORT boolean      SelectionInDFN;
THOT_EXPORT boolean      SelectionInCODE;
THOT_EXPORT boolean      SelectionInVAR;
THOT_EXPORT boolean      SelectionInSAMP;
THOT_EXPORT boolean      SelectionInKBD;
THOT_EXPORT boolean      SelectionInI;
THOT_EXPORT boolean      SelectionInB;
THOT_EXPORT boolean      SelectionInTT;
THOT_EXPORT boolean      SelectionInU;
THOT_EXPORT boolean      SelectionInSTRIKE;
THOT_EXPORT boolean      SelectionInBIG;
THOT_EXPORT boolean      SelectionInSMALL;

#define DocumentTableLength 10
THOT_EXPORT char        *DocumentURLs[DocumentTableLength];

/* The whole document is loaded when the corresponding value
   in FilesLoading is equal to 0 */
THOT_EXPORT int          FilesLoading[DocumentTableLength];
THOT_EXPORT Document     W3Loading;	/* the document being loaded */
THOT_EXPORT int          IButton;
THOT_EXPORT int          BButton;
THOT_EXPORT int          TTButton;

THOT_EXPORT char AmayaLastHTTPErrorMsg [];

#define IMAGE_NOT_LOADED	0
#define IMAGE_LOCAL		1
#define IMAGE_LOADED		2
#define IMAGE_MODIFIED		3

typedef struct _ElemImage
  {
     Element             currentElement;	/* first element using this image */
     struct _ElemImage  *nextElement;
  }
ElemImage;

typedef struct _LoadedImageDesc
  {
     char               *originalName;	/* complete URL of the image             */
     char               *localName;	/* local name (without path) of the image   */
     struct _LoadedImageDesc *prevImage;
     struct _LoadedImageDesc *nextImage;
     Document            document;	/* document concerned                        */
     struct _ElemImage  *elImage;	/* first element using this image          */
     int                 status;
  }
LoadedImageDesc;

THOT_EXPORT LoadedImageDesc *ImageURLs;

#define EOS     '\0'
#define EOL     '\n'
#define TAB     '\t'
#define SPACE    ' '

#endif /* AMAYA_H */

