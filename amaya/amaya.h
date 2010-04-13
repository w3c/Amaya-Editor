/*
 *
 *  (c) COPYRIGHT INRIA and W3C, 1996-2009
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

#ifndef AMAYA_H 
#define AMAYA_H

/* Thot interface */
#include "thot_gui.h"
#include "thot_sys.h"
#include "appstruct.h"
#include "application.h"
#include "attribute.h"
#include "browser.h"
#include "content.h"
#include "dialog.h"
#include "fileaccess.h"
#include "interface.h"
#include "libmsg.h"
#include "message.h"
#include "presentation.h"
#include "selection.h"
#include "reference.h"
#include "tree.h"
#include "view.h"
#include "uconvert.h"
#include "undo.h"

/* Included headerfiles */
#include "EDITOR.h"
#include "HTML.h"
#include "MathML.h"
#include "SVG.h"
#include "TextFile.h"
#include "XML.h"
#include "amayamsg.h"
#include "parser.h"
#define NAME_LENGTH     32

#define URL_STR       "/"
#define URL_SEP       '/'


/* Number of views used in Amaya */
#define AMAYA_MAX_VIEW_DOC  7

/* the type of character convertion we want to make on local URLs */
typedef enum _ConvertionType
{
  AM_CONV_NONE = 0,
  AM_CONV_URL_SEP = 0x1, /* converts URL_SEPs into DIR_SEPs */
  AM_CONV_PERCENT = 0x2, /* converts %xx into the equiv. char */
  AM_CONV_ALL = 0xFFFF   /* do everything */
}
ConvertionType;

/* the structure used for exchanging HTTP header info between the net library
   and amaya */
typedef struct _AHTHeaders
  {
    char *content_type;
    char *charset;
    char *content_length;
    char *reason;
    char *content_location;
    char *full_content_location;
    int   status;
  }
AHTHeaders;

/* the HTTP header name we want to make visible to the application */
typedef enum _AHTHeaderName
  {
    AM_HTTP_CONTENT_TYPE = 0,
    AM_HTTP_CHARSET = 1,
    AM_HTTP_CONTENT_LENGTH = 2,
    AM_HTTP_REASON = 3,
    AM_HTTP_CONTENT_LOCATION = 4,
    AM_HTTP_FULL_CONTENT_LOCATION = 5
  } 
AHTHeaderName;

/* The structures used for request callbacks */
typedef void   TIcbf (Document doc, int status, char *urlName,
		      char *outputfile, const AHTHeaders *http_headers,
		      const char *data_block, int data_block_size,
		      void *context);

typedef void  TTcbf (Document doc, int status, char *urlName,
                     char *outputfile, char *proxyName,
                     const AHTHeaders *http_headers, void *context);

/* the structure used for storing the context of the 
   GetAmayaDoc_callback function */
typedef struct _AmayaDoc_context
{
  Document   doc;
  Document   baseDoc;
  ThotBool   history;
  ThotBool   local_link;
  char      *target;
  char      *documentname; /* the document name */
  char      *initial_url;  /* initial loaded URL */
  char      *form_data;
  int        method;
  ThotBool   inNewWindow;
  TTcbf     *cbf;
  void      *ctx_cbf;
} AmayaDoc_context;

/* the structure used for storing the context of the 
   Reload_callback function */
typedef struct _RELOAD_context
{
  Document   newdoc;
  char      *documentname;
  char      *form_data;
  int        method;
  int        position;	/* volume preceding the the first element to be shown */
  int        distance; /* distance from the top of the window to the top of this
                   element (% of the window height) */
  int        visibility; /* register the current visibility */
  ThotBool   maparea; /* register the current maparea status */
  ThotBool   withSrc; /* register source open */
  ThotBool   withStruct; /* register structure open */
} RELOAD_context;

/* How are Network accesses provided ? */
#include "libwww.h"

/* The different events to open a new document */
typedef enum _ClickEvent {
  CE_ABSOLUTE, CE_RELATIVE, CE_FORM_POST, CE_FORM_GET,
  CE_HELP, CE_MAKEBOOK, CE_LOG , CE_TEMPLATE, CE_INIT,
  CE_CSS, CE_ANNOT, CE_INSTANCE
} ClickEvent;

#define NO               0
#define YES              1

/* dialogues */
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
#define ConfirmForm     14
#define ConfirmText     15
#define AttrHREFForm    16
#define AttrHREFText    17
#define FormAnswer      18
#define RealmText       19
#define AnswerText      20
#define NameText        21
#define PasswordText    22
#define FilterText      23
#define ClassForm       24
#define ClassSelect     25
#define AClassForm      26
#define AClassSelect    27
#define ConfirmSave     28
#define ConfirmSaveList 29
#define BrowserDirSelect   30
#define BrowserDocSelect   31
#define BrowserFilterText  32
#define HREFLocalName   33
#define MakeIdMenu      34
#define mElemName       35
#define mAddId          36
#define mRemoveId       37
#define mIdUseSelection 38
#define mIdStatus       39
#define FileBrowserForm 40
#define FileBrowserText 41
#define FileBrowserFilter 42
#define FileBrowserLocalName 43
#define MimeTypeForm    44
#define MimeTypeSel     45
#define MimeTypeSave    46
#define CharsetForm     47
#define CharsetSave     48
#define CharsetSel      49
#define RadioSave       50
#define MimeFormStatus  51
#define SaveFormStatus  52
#define OpenLocation    53
/* do not add items before this entry */
#define OptionMenu	54
#define OpenTemplate    55
#define PasswordSave    56
#define ListNSForm      57
#define ListNSAdd       58
#define ListNSSupp      59

/* MAX_SUBMENUS references reserved for submenus of Option menu */
/* Do not insert new entries here */
#define MAX_SUBMENUS    400
#define Label1          54 /* no callback */
#define Label2          55 /* no callback */
#define Label3          56 /* no callback */
#define Label4          57 /* no callback */
#define About1		58 /* no callback */
#define About2		59 /* no callback */
#define About3		60 /* no callback */
#define Version		61 /* no callback */
#define CharsetSaveL    62 /* no callback */
#define MimeTypeSaveL   63 /* no callback */
/* first callback available: 454 */
#define AboutForm      	454
#define TableForm       455
#define TableRows       456
#define TableCols       457
#define TableBorder     458
#define MathEntityForm  459
#define MathEntityText  460
#define TitleForm       461
#define TitleText       462
#define DocInfoForm           463
#define DocInfoMimeTypeTitle  464
#define DocInfoMimeType       465
#define DocInfoCharsetTitle   466
#define DocInfoCharset        467
#define DocInfoContentTitle   468
#define DocInfoContent        469
#define DocInfoURLTitle       470
#define DocInfoURL            471
#define DocInfoTitle1         472
#define DocInfoTitle2         473
#define DocInfoContent1       474
#define DocInfoContent2       475
#define DocInfoLocationTitle  476
#define DocInfoLocation       477
#define DocInfoDocTypeTitle   478
#define DocInfoDocType        479
#define CssDirSave            480
#define JavascriptPromptForm  481
#define JavascriptPromptText  482
#define MAX_REF 483

#define FormPrint        1
#define PrintOptions     2
#define PaperFormat      3
#define PrintSupport     4
#define PPrinterName     5
#define PaperOrientation 6
#define PPagesPerSheet   7
#define PRINT_MAX_REF	 8

#define FormLibrary              1
#define CopySVGLibSelection      2
#define ReferToSVGLibSelection   3
#define AddSVGModel              4
#define NewSVGFileURL            5
#define SVGLibCatalogueTitle     6
#define NewSVGLibrary           13
#define SVGLibraryURL           14
#define SVGLibraryDir           15
#define SVGLibrarySel           16
#define LibraryFilter           17
#define SVGLibFileBrowser       18
#define SVGLibFileBrowserText   19
#define LibraryCopyOption       20
#define LIBRARY_MAX_REF         21

#define ImageURL	1
#define FormImage	      2
#define FormObject      3
#define IMAGE_MAX_REF   4

#define CSSForm         0
#define CSSSelect       1
#define CSSEntry        2
#define CSSValue        3
#define MAX_CSS_REF     4

#define FormMaths 0
#define FormMathOperator 1

#define FormMathFenceAttributes 2
#define MathAttributeOpen 3
#define MathAttributeSeparators 4
#define MathAttributeClose 5

#define FormMathIntegral 6
#define MathIntegralNumber 7
#define MathIntegralContour 8
#define MathIntegralType 9
#define MAX_MATHS 10

#define FormGraph 0
#define MenuGraph 1
#define MenuGraph1 2
#define MAX_GRAPH 3

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
#define AMAYA_NOCACHE	64 /* 2^6 */
/* don't follow redirections */
#define AMAYA_NOREDIR	128 /* 2^7 */
/* Prevents a stop race condition in ASYNC transfers */
#define AMAYA_ASYNC_SAFE_STOP  256 /* 2^8 */
#define AMAYA_LOAD_CSS   512 /* 2^9 */
#define AMAYA_FLUSH_REQUEST 1024 /* 2^10 */
#define AMAYA_USE_PRECONDITIONS 2048 /* 2^11 */
#define AMAYA_LOAD_IMAGE 4096 /* 2^12 */
/* post an annonation */
#define AMAYA_FILE_POST 8192 /* 2^13 */
/* delete an annotation */
#define AMAYA_DELETE    16384 /* 2^14 */
#define AMAYA_MBOOK_IMAGE 32768 /* 2^15 */
/* post an annotation using PUT */
#define AMAYA_SIMPLE_PUT 65536 /* 2^16 */

/*
 * Flags to indicate the action to take when the network options
 * are modified
 */
#define AMAYA_CACHE_RESTART 1
#define AMAYA_PROXY_RESTART 2
#define AMAYA_LANNEG_RESTART 4
#define AMAYA_SAFEPUT_RESTART 8

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

#define MAX_AutoSave_list 20

THOT_EXPORT int        AMAYA;     /* Index of amaya message table */
extern      int        appArgc;	  /* defined in EDITORAPP.c */
extern      char     **appArgv;   /* defined in EDITORAPP.c */

THOT_EXPORT char      *AttrHREFvalue;
THOT_EXPORT char      *LastURLName;	/* last URL requested               */
THOT_EXPORT char      *Error_DocURL; /* The parsed file */
THOT_EXPORT char      *DirectoryName;	/* local path of the document       */
THOT_EXPORT char      *DocumentName;	/* document name                    */
THOT_EXPORT char      *SavePath;	/* saving path                      */
THOT_EXPORT char      *SaveName;	/* saving name of the document      */
THOT_EXPORT char       SaveFileName[MAX_LENGTH];	/* concatenation of SavePath+SaveName */
THOT_EXPORT char      *ObjectName;	/* document name                    */
THOT_EXPORT char      *SaveImgsURL;	/* where to save remote Images      */
THOT_EXPORT char      *SaveCssURL;	/* where to save remote Images      */
THOT_EXPORT char      *TargetName;
THOT_EXPORT char      *SavingFile;	/* complete path or URL of the document */
THOT_EXPORT char      *SavedDocumentURL;/* URL of the document that contained the elements
                                           that are now in the Cut and Paste buffer */
THOT_EXPORT char      *AutoSave_list;
THOT_EXPORT char      *Template_list; /* list of templates */
THOT_EXPORT char      *URL_list; /* list of previous open URLs */
THOT_EXPORT char      *RDFa_list; /* list of Namespace declarations for RDFa */
#ifdef _SVG
THOT_EXPORT char      *SVGlib_list;
#endif /* _SVG */
THOT_EXPORT char      *MetaName;
THOT_EXPORT char      *MetaContent;
THOT_EXPORT char      *MetaEquiv;

THOT_EXPORT char       TempFileDirectory[MAX_LENGTH];
THOT_EXPORT char       Answer_text[MAX_LENGTH];
THOT_EXPORT char       Answer_name[NAME_LENGTH];
THOT_EXPORT char       Answer_password[NAME_LENGTH];
THOT_EXPORT char       Display_password[NAME_LENGTH];
THOT_EXPORT char       CurrentNameSpace[MAX_LENGTH]; // namespace of the sniffed document
THOT_EXPORT char       ScanFilter[NAME_LENGTH]; /* to scan directories    */
THOT_EXPORT char       MathMLEntityName[MAX_LENGTH]; /* entity name typed by the user for a MathML expression */
THOT_EXPORT char       JavascriptPromptValue[MAX_LENGTH]; /* value typed by the user in a prompt form */
THOT_EXPORT char       IdElemName[MAX_LENGTH]; /* element name typed by the user from the MakeID menu */
THOT_EXPORT char       IdStatus[50]; /* element name typed by the user from the MakeID menu */
THOT_EXPORT char       ImgFilter[NAME_LENGTH];
THOT_EXPORT char       DirectoryImage[MAX_LENGTH];
THOT_EXPORT char       LastURLImage[MAX_LENGTH];
THOT_EXPORT char       ImageName[MAX_LENGTH];
THOT_EXPORT char       ImgAlt[MAX_LENGTH];
THOT_EXPORT char       UserMimeType[MAX_LENGTH];
                                        /* Used to pass the user's MIME type 
					   choice when doing a Save As of a
					   local object to a server */
THOT_EXPORT char       UserCharset[MAX_LENGTH];
                                        /* Used to pass the user's charset
					   choice when doing a Save As of a
					   local object to a server */
THOT_EXPORT char       SaveFormTmp[MAX_LENGTH];
                                        /* Used for storing the temporary
					   changes in the ChangeCharset and
					   ChangeMimetype forms */

THOT_EXPORT int        URL_list_len;
THOT_EXPORT int        Template_list_len;
THOT_EXPORT int        RDFa_list_len;
/* list of auto-saved files */
THOT_EXPORT int        AutoSave_list_len;
THOT_EXPORT int        AutoSave_Interval;
THOT_EXPORT int        Lg_password;
THOT_EXPORT int        BaseDialog;
THOT_EXPORT int        BasePrint;
THOT_EXPORT int        BaseImage;
THOT_EXPORT int	       MathsDialogue;
THOT_EXPORT int	       GraphDialogue;
THOT_EXPORT int	       BaseCSS;
THOT_EXPORT int        ReturnOption;
THOT_EXPORT int        NumberRows;
THOT_EXPORT int        NumberCols;
THOT_EXPORT int        TBorder; // border width of created tables
THOT_EXPORT int        TCaption; // value 0=no, 1=yes
THOT_EXPORT int        ImgPosition; // value 0=inline, 1=left, 2=center 3=right
THOT_EXPORT int        ReturnOptionMenu;
THOT_EXPORT int        IdDoc;
THOT_EXPORT int        BaseLibrary;

THOT_EXPORT Document   CurrentDocument;
THOT_EXPORT Document   SavingDocument;
THOT_EXPORT Document   SavingObject;
THOT_EXPORT Document   AttrHREFdocument;
THOT_EXPORT Document   DocBook;
THOT_EXPORT Document   IncludedDocument;
THOT_EXPORT Document   ParsedDoc; /* The document to which CSS are applied */
THOT_EXPORT Document   ParsedCSS; /* The CSS document currently parsed */
THOT_EXPORT Document   SelectionDoc;
THOT_EXPORT Document   W3Loading; /* the document being loaded */
THOT_EXPORT Document   BackupDocument; /* the current backup */
THOT_EXPORT Element    AttrHREFelement;
THOT_EXPORT Element    Right_ClickedElement;
THOT_EXPORT Document	 HighlightDocument;
THOT_EXPORT Element	   HighlightElement;
THOT_EXPORT Attribute  HighLightAttribute;

THOT_EXPORT FILE      *ErrFile;
THOT_EXPORT FILE      *CSSErrFile;
THOT_EXPORT ThotBool   IgnoreErrors;
THOT_EXPORT ThotBool   URL_list_keep;
THOT_EXPORT ThotBool   Answer_save_password;
THOT_EXPORT ThotBool   TMAX_Width;
THOT_EXPORT ThotBool   AttrHREFundoable;
THOT_EXPORT ThotBool   IsNewAnchor;
THOT_EXPORT ThotBool   UseLastTarget;
THOT_EXPORT ThotBool   LinkAsCSS;
THOT_EXPORT ThotBool   LinkAsXmlCSS;
THOT_EXPORT ThotBool   LinkAsJavascript;
THOT_EXPORT ThotBool   LinkAsImport;
THOT_EXPORT ThotBool   SaveAsHTML;
THOT_EXPORT ThotBool   SaveAsXML;
THOT_EXPORT ThotBool   SaveAsText;
THOT_EXPORT ThotBool   CopyImages;	    /* should amaya copy images in Save As? */
THOT_EXPORT ThotBool   CopyResources;   /* should amaya copy resource in Save As? */
THOT_EXPORT ThotBool   UpdateURLs;	    /* should amaya update URLs in Save As? */
THOT_EXPORT ThotBool   RemoveTemplate;	/* should amaya remove template in Save As? */
THOT_EXPORT ThotBool   UserAnswer;
THOT_EXPORT ThotBool   ExtraChoice;
THOT_EXPORT ThotBool   TableMenuActive;
THOT_EXPORT ThotBool   MTableMenuActive;
THOT_EXPORT ThotBool   DontReplaceOldDoc;
THOT_EXPORT ThotBool   InNewWindow;
THOT_EXPORT ThotBool   SelectionInPRE;
THOT_EXPORT ThotBool   SelectionInComment;
THOT_EXPORT ThotBool   SelectionInEM;
THOT_EXPORT ThotBool   SelectionInSTRONG;
THOT_EXPORT ThotBool   SelectionInCITE;
THOT_EXPORT ThotBool   SelectionInABBR;
THOT_EXPORT ThotBool   SelectionInACRONYM;
THOT_EXPORT ThotBool   SelectionInINS;
THOT_EXPORT ThotBool   SelectionInDEL;
THOT_EXPORT ThotBool   SelectionInDFN;
THOT_EXPORT ThotBool   SelectionInCODE;
THOT_EXPORT ThotBool   SelectionInVAR;
THOT_EXPORT ThotBool   SelectionInSAMP;
THOT_EXPORT ThotBool   SelectionInKBD;
THOT_EXPORT ThotBool   SelectionInI;
THOT_EXPORT ThotBool   SelectionInB;
THOT_EXPORT ThotBool   SelectionInTT;
THOT_EXPORT ThotBool   SelectionInBIG;
THOT_EXPORT ThotBool   SelectionInSMALL;
THOT_EXPORT ThotBool   SelectionInSpan;
THOT_EXPORT ThotBool   SelectionInSub;
THOT_EXPORT ThotBool   SelectionInSup;
THOT_EXPORT ThotBool   SelectionInQuote;
THOT_EXPORT ThotBool   SelectionInBDO;
THOT_EXPORT ThotBool   HTMLErrorsFound;
THOT_EXPORT ThotBool   XMLErrorsFound;
THOT_EXPORT ThotBool   CSSErrorsFound;
THOT_EXPORT ThotBool   XMLErrorsFoundInProfile;
THOT_EXPORT ThotBool   XMLNotWellFormed;
THOT_EXPORT ThotBool   XMLInvalidToken;
THOT_EXPORT ThotBool   XMLCharacterNotSupported;
THOT_EXPORT ThotBool   XMLUnknownEncoding;
/*  enabling/disabling DAVLib */
THOT_EXPORT ThotBool   DAVLibEnable;
THOT_EXPORT ThotBool   Synchronizing;
THOT_EXPORT ThotBool   AmayaUniqueInstance;
THOT_EXPORT ThotBool   IdApplyToSelection; /* used in the Make ID menu */
THOT_EXPORT ThotBool   Check_read_ids; /* check all parsed IDs */

typedef enum
{
  docFree,
  docHTML,
  docText,
  docImage,
  docCSS,
  docSource,
  docAnnot,
  docLog,
  docSVG,
  docMath,
  docXml,
  docTemplate,
  docLibrary,
  docBookmark,
  docJavascript,
  docTypeMax
} DocumentType;

THOT_EXPORT const char *DocumentTypeNames[docTypeMax]
#ifdef THOT_INITGLOBALVAR
= 
{
  "free",    /* docFree */
  "html",    /* docHTML */
  "text",    /* docText */
  "image",   /* docImage */
  "css",     /* docCSS */
  "source",  /* docSource */
  "annot",   /* docAnnot */
  "log",     /* docLog */
  "svg",     /* docSVG */
  "math",    /* docMath */
  "xml",     /* docXml */
  "library", /* docLibrary */
  "bookmark",/* docBookmark */
}
#endif /* THOT_INITGLOBALVAR */
; /* do not remove this ; */


/* a record for data associated with a request */
typedef struct _DocumentMetaDataElement
{
  char      *initial_url;      /* if the server returns a different URL name
				  after a redirection, we store here the one
				  that the browser first asked */
  char      *form_data;        /* form data associated with a URL */
  char      *content_type;     /* content type returned by the server */
  char      *charset;          /* charset returned by the server */
  char      *content_length;   /* content length returned by the server */
  char      *content_location; /* simplified content location returned by the server */
  char      *full_content_location; /* full content location returned by the server */
  char      *reason;           /* http_headers reason*/
  int        method;           /* method used to send this data */
  Element    link_icon;        /* there is a link to an icon */
  int        lockState;        /* 0=not webDAV, 1=unlocked, 2=locked */
  ThotBool   xmlformat;        /* the document should be exported in xml format */
  ThotBool   compound;         /* the document is a compound document */
#ifdef ANNOTATIONS
  Document   source_doc;       /* if the document is an annotation,
				  this variable giveso the annoted document docid */
#endif /* ANNOTATIONS */
  
//#ifdef TEMPLATES
//  ThotBool  isTemplate;	 /* True if the document is a XTiger template or library,
//  			    False if template instance or not using XTiger.*/
//  ThotBool  isInternalTemplate;  /* True if the template is loaded internaly, not used ad edited document.*/ 
//  char      *template_url;       /* the url of the original template (or null if not an instance) */
//  char      *template_version;   /* if this document is derived from a template,
//				     The name and the version of the template used to create the document 
//				     is stored in this variable */
//#endif /* TEMPLATES */
  
} DocumentMetaDataElement;


#define DocumentTableLength MAX_DOCUMENTS
/* URL of each loaded document */
extern char                         *DocumentURLs[MAX_DOCUMENTS];
/* Any formdata associated with a URL */
THOT_EXPORT DocumentMetaDataElement *DocumentMeta[DocumentTableLength];
/* Type of document */
THOT_EXPORT DocumentType             DocumentTypes[DocumentTableLength];
/* Document that shows map areas */
THOT_EXPORT ThotBool                 MapAreas[DocumentTableLength];
/* Document splitted horizontally */
THOT_EXPORT ThotBool                 HSplit[DocumentTableLength];
/* Document splitted vertically */
THOT_EXPORT ThotBool                 VSplit[DocumentTableLength];
/* identifier of the document displaying the source code */
THOT_EXPORT Document                 DocumentSource[DocumentTableLength];
/* The whole document is loaded when the corresponding entry in FilesLoading is 0 */
THOT_EXPORT int                      FilesLoading[DocumentTableLength];
/* Status (error, success) of the download of the objects of a document */
THOT_EXPORT int                      DocNetworkStatus[DocumentTableLength];


#define RESOURCE_NOT_LOADED       0
#define RESOURCE_LOADED		        1
#define RESOURCE_MODIFIED		      2
typedef void (*LoadedImageCallback)(Document doc, Element el, char *file,
				    void *extra, ThotBool isnew);
typedef struct _ElemImage
  {
     Element              currentElement;/* first element using this image */
     struct _ElemImage   *nextElement;
     LoadedImageCallback  callback;	/* Callback for non-standard handling */
     void		             *extra;		/* any extra info for the CallBack */
  }
ElemImage;

typedef struct _LoadedImageDesc
  {
     char               *originalName;  /* complete URL of the image                */
     char               *localName;     /* relative name (without path) of the image*/
     char               *tempfile;      /* name of the file that stores the image   */ 
     char               *content_type;  /* the MIME type as sent by the server      */
     struct _LoadedImageDesc *prevImage;/* double linked list                       */
     struct _LoadedImageDesc *nextImage;/* easier to unchain                        */
     Document            document;	    /* document concerned                       */
     struct _ElemImage  *elImage;       /* first element using this image           */
     int                 imageType;     /* the type of the image                    */
     int                 status;        /* the status of the image loading          */
  }
LoadedImageDesc;

/* the structure used for storing the context of the 
   FetchAndDisplayImages_callback function */
typedef struct _FetchImage_context {
  char               *base_url;
  LoadedImageDesc    *desc;
} FetchImage_context;


THOT_EXPORT LoadedImageDesc *ImageURLs;
THOT_EXPORT LoadedImageDesc *LoadedResources;

/* The default Amaya HOME pages (page shown at boot time */
#ifdef _WX
#define AMAYA_PAGE  "AmayaPage_WX.html"
#else /* _WX */
#define AMAYA_PAGE  "AmayaPage.html"
#endif /* _WX */
#define AMAYA_PAGE_DOC  "http://www.w3.org/Amaya/User/"

#ifndef MAX_TXT_LEN
#define MAX_TXT_LEN 1024	/* Max. length of strings */
#endif  /* MAX_TXT_LEN */

/* a collection of the XML MIME types that keep changing and that we use in Amaya */
#define AM_XHTML_MIME_TYPE        "application/xhtml+xml"
#define AM_SVG_MIME_TYPE          "image/svg+xml"
#define AM_MATHML_MIME_TYPE       "application/mathml+xml"
#define AM_GENERIC_XML_MIME_TYPE  "text/xml"


/* Doctype declarations used in Amaya */
/* first line of doctype declarations */
#define DOCTYPE1_HTML_STRICT          "<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.01//EN\""
#define DOCTYPE1_HTML_TRANSITIONAL    "<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\""
#define DOCTYPE1_XHTML10_BASIC        "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML Basic 1.0//EN\""
#define DOCTYPE1_XHTML10_STRICT       "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\""
#define DOCTYPE1_XHTML11              "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.1//EN\""
#define DOCTYPE1_XHTML11_PLUS_MATHML  "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.1 plus MathML 2.0//EN\""
#define DOCTYPE1_XHTML11_PLUS_MATHML_PLUS_SVG  "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.1 plus MathML 2.0 plus SVG 1.1//EN\""
#define DOCTYPE1_XHTML10_TRANSITIONAL "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\""
#define DOCTYPE1_XHTML_PLUS_RDFa      "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML+RDFa 1.0//EN\""
#define DOCTYPE1_MATHML20             "<!DOCTYPE math PUBLIC \"-//W3C//DTD MathML 2.0//EN\""
#define DOCTYPE1_SVG10                "<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.0//EN\""

/* second line of doctype declarations */
#define DOCTYPE2_HTML_STRICT          "      \"http://www.w3.org/TR/html4/strict.dtd\">"
#define DOCTYPE2_HTML_TRANSITIONAL    "      \"http://www.w3.org/TR/html4/loose.dtd\">"
#define DOCTYPE2_XHTML10_BASIC        "      \"http://www.w3.org/TR/xhtml-basic/xhtml-basic10.dtd\">"
#define DOCTYPE2_XHTML10_STRICT       "      \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">"
#define DOCTYPE2_XHTML11              "      \"http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd\">"
#define DOCTYPE2_XHTML11_PLUS_MATHML  "      \"http://www.w3.org/Math/DTD/mathml2/xhtml-math11-f.dtd\">"
#define DOCTYPE2_XHTML11_PLUS_MATHML_PLUS_SVG  "      \"http://www.w3.org/2002/04/xhtml-math-svg/xhtml-math-svg.dtd\">"
#define DOCTYPE2_XHTML10_TRANSITIONAL "      \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">"
#define DOCTYPE2_XHTML_PLUS_RDFa      "      \"http://www.w3.org/MarkUp/DTD/xhtml-rdfa-1.dtd\">"
#define DOCTYPE2_MATHML20             "      \"http://www.w3.org/TR/MathML2/dtd/mathml2.dtd\">"
#define DOCTYPE2_SVG10                "      \"http://www.w3.org/TR/2001/REC-SVG-20010904/DTD/svg10.dtd\">"

#define MATHML_XSLT_NAME     "pmathml.xsl"
#define MATHML_XSLT_URI      "xml-stylesheet type=\"text/xsl\" href=\""

#define SVG_VERSION "1.0" /* version of the SVG specificatrion implemented
			     by the Amaya SVG editor */

#endif /* AMAYA_H */
