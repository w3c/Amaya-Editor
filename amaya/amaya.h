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
#define HTAppVersion "V1.2a"

#define URL_SEP '/'
#define URL_STR "/"

/* The structures used for request callbacks */

typedef void   TIcbf (Document doc, int status, char *urlName,
		      char *outputfile, const char *content_type,
		      const char *data_block, int data_block_size,
		      void *context);

typedef void  TTcbf (Document doc, int status, char *urlName,
                     char *outputfile, const char *content_type,
                     void *context);

/* How are Network accesses provided ? */
#ifdef AMAYA_JAVA
#include "libjava.h"
#else
#ifdef AMAYA_ILU
#include "libilu.h"
#else
#include "libwww.h"
#endif
#endif

/* The different events for a DoubleClick */
typedef enum _ClickEvent {
  CE_FALSE, CE_TRUE, CE_FORM_POST, CE_FORM_GET, CE_HELP, CE_MAKEBOOK
} ClickEvent;

typedef char        AmayaReadChar ();
#define NO               0
#define YES              1

/* dialogue */
#define URLForm          1
#define OpenForm         2
#define URLName          3
#define LocalName        4
#define DirSelect        5
#define DocSelect        6
#define StopCommand      7
#define SaveForm         8
#define DirSave          9
#define DocSave         10
#define ToggleSave      11
#define NameSave        12
#define ImgDirSave      13
#define Label1          14
#define Label2          15
#define Label3          16
#define Label4          17
#define ConfirmForm     18
#define ConfirmText     19
#define AttrHREFForm    20
#define AttrHREFText    21
#define FormAnswer      22
#define AnswerText      24
#define NameText        25
#define PasswordText    26
#define FilterText      27
#define ClassForm       28
#define ClassSelect     29
#define AClassForm      30
#define AClassSelect    31
#define ConfirmSave     32
#define ConfirmSaveList 33
#define OptionMenu	34
#define About1		35
#define About2		36
#define About3		37
#define Version		38
#define FormAbout      	39    
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
#ifndef AMAYA_JAVA
/* Prevents a stop race condition in ASYNC transfers */
#define AMAYA_ASYNC_SAFE_STOP  256
#endif /* ! AMAYA_JAVA */
/*
 * Flags to indicate the status of the network requests associated
 * to a document.
 */

#define AMAYA_NET_INACTIVE 1
#define AMAYA_NET_ERROR    2
#define AMAYA_NET_ACTIVE   4

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
THOT_EXPORT char         ScanFilter[NAME_LENGTH]; /* to scan directories    */
THOT_EXPORT char        *LastURLName;	/* last URL requested               */
THOT_EXPORT char        *DirectoryName;	/* local path of the document       */
THOT_EXPORT char        *DocumentName;	/* document name                    */
THOT_EXPORT char        *SavePath;	/* saving path                      */
THOT_EXPORT char        *SaveName;	/* saving name of the document      */
THOT_EXPORT char        *ObjectName;	/* document name                    */
THOT_EXPORT char        *SaveImgsURL;	/* where to save remote Images      */
THOT_EXPORT char        *TargetName;
THOT_EXPORT char        *SavingFile;	/* complete path or URL of the document */
THOT_EXPORT int          Lg_password;
THOT_EXPORT int          BaseDialog;
THOT_EXPORT int          ReturnOption;
THOT_EXPORT Document     CurrentDocument;
THOT_EXPORT Document     SavingDocument;
THOT_EXPORT Document     SavingObject;
THOT_EXPORT Document     AttrHREFdocument;
THOT_EXPORT Document     DocBook;
THOT_EXPORT Document     IncludedDocument;
THOT_EXPORT Element      AttrHREFelement;
THOT_EXPORT char        *AttrHREFvalue;
THOT_EXPORT Document     SelectionDoc;
THOT_EXPORT boolean	 SaveAsHTML;
THOT_EXPORT boolean	 SaveAsText;
THOT_EXPORT boolean      CopyImages;	/* should we copy images in Save As */
THOT_EXPORT boolean      UpdateURLs;	/* should we update URLs in Save As */
THOT_EXPORT boolean      UserAnswer;
THOT_EXPORT boolean      InNewWindow;
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
/* TRUE if the document is displayed by help commands */
THOT_EXPORT boolean      HelpDocuments[DocumentTableLength];
/* The whole document is loaded when the corresponding value
   in FilesLoading is equal to 0 */
THOT_EXPORT int          FilesLoading[DocumentTableLength];
/* Gives the status (error, success) of the download of the objects of
   a document */

THOT_EXPORT int          DocNetworkStatus[DocumentTableLength];
THOT_EXPORT Document     W3Loading;	/* the document being loaded */
THOT_EXPORT int          IButton;
THOT_EXPORT int          BButton;
THOT_EXPORT int          TTButton;

#define IMAGE_NOT_LOADED	0
#define IMAGE_LOCAL		1
#define IMAGE_LOADED		2
#define IMAGE_MODIFIED		3

#ifdef __STDC__
typedef void (*LoadedImageCallback)(Document doc, Element el, char *file, void *extra);
#else
typedef void (*LoadedImageCallback)();
#endif
typedef struct _ElemImage
  {
     Element             currentElement;/* first element using this image */
     struct _ElemImage  *nextElement;
     LoadedImageCallback callback;	/* Callback for non-standard handling */
     void		*extra;		/* any extra info for the CallBack */
  }
ElemImage;

typedef struct _LoadedImageDesc
  {
     char               *originalName;	/* complete URL of the image             */
     char               *localName;	/* local name (without path) of the image   */
     struct _LoadedImageDesc *prevImage;/* double linked list */
     struct _LoadedImageDesc *nextImage;/* easier to unchain */
     Document            document;	/* document concerned                        */
     struct _ElemImage  *elImage;	/* first element using this image          */
     int                 status;	/* the status of the Image loading */
  }
LoadedImageDesc;

THOT_EXPORT LoadedImageDesc *ImageURLs;
THOT_EXPORT LoadedImageDesc *ImageLocal;

#define EOS     '\0'
#define EOL     '\n'
#define TAB     '\t'
#define SPACE    ' '

#endif /* AMAYA_H */











