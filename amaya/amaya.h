#ifndef AMAYA_H
#define AMAYA_H

/* Included headerfiles */

#include "app.h"
#include "HTML.h"

#include "amayamsg.h"

/* libwww interface */

#include "thot_gui.h"
#include "thot_sys.h"

#include "WWWLib.h"
#include "WWWApp.h"
#include "WWWHTTP.h"
#include "WWWInit.h"
#include "HTReqMan.h"
#include "HTAncMan.h"
#include "AHTDialog.h"
#include "HTReqMan.h"
#include "HTAncMan.h"
#include "HTAccess.h"
#include "HTEvntrg.h"
#include "HTAlert.h"
#include "HTNetMan.h"
#include "HTBInit.h"
#include "WWWHTTP.h"		/* HTTP access module */
#include "HTProxy.h"
#include "application.h"

typedef char        PathBuffer[MAX_PATH];

typedef struct _AHTDocIdStatus
  {
     int                 docid;	/* a docid */
     int                 counter;	/* number of open requests associated with dicid */
  }
AHTDocId_Status;


typedef struct __AmayaContext
  {
     HTList             *reqlist;	/* List of current requests */
     HTList             *docid_status;	/* Status for each active docid */
     int                 open_requests;		/* number of open requests */
  }
AmayaContext;


/* The different events for a DoubleClick */

typedef enum _DoubleClickEvent
  {
     DC_FALSE = 0,
     DC_TRUE = 1,
     DC_FORM_POST = 2,
     DC_FORM_GET = 4
  }
DoubleClickEvent;

/* The possible request modes */

#define AMAYA_SYNC	1	/*0x000001 */
#define AMAYA_ISYNC	2	/*0x000010 */
#define AMAYA_ASYNC	4	/*0x000100 */
#define AMAYA_IASYNC	8	/*0x001000 */
#define AMAYA_FORM_POST 16	/*0x010000 */
#define AMAYA_FORM_GET  32	/*0x100000 */

typedef char        AmayaReadChar ();

/*typedef void        *PresentationTarget; */

typedef enum _AHTReqStatus
  {
     HT_NEW = 0,
     HT_NEW_PENDING = 1,
     HT_WAITING = 2,
     HT_BUSY = 4,
     HT_END = 8,
     HT_ABORT = 16,
     HT_ERR = 32
  }
AHTReqStatus;

typedef void        TIcbf (void *request_context, const char *data_block, int data_block_size, int request_status);

typedef void        TTcbf (void *request_context, int request_status);

typedef struct _AHTReqContext
  {
     HTRequest          *request;	/* Pointer to the associated request object     */
     HTParentAnchor     *anchor;
     HTMethod            method;	/* What method are we envoking                  */
     int                 docid;	/* docid to which this request belongs          */
     AHTReqStatus        reqStatus;	/* status of the request                        */
     SockOps             read_ops;	/* The ops operation which must be used during
					   ** an Xt read callback */

     SockOps             write_ops;	/* The ops operation which must be used during
					   ** an Xt write callback */

     SockOps             except_ops;	/* The ops operation which must be used during
					   ** an Xt exception callback */

#ifdef WWW_XWINDOWS
     XtInputId           read_xtinput_id;	/* The read xt event id assocciated with
						   the request */
     XtInputId           write_xtinput_id;	/* The write xt event id assocciated with
						   the request */
     XtInputId           except_xtinput_id;	/* The except xt event id assocciated with
						   the request */
#endif				/* WWW_XWINDOWS */
     char               *outputfile;	/* file to receive incoming data         */
     FILE               *output;	/* file pointer to outputfile            */
     int                 mode;	/* Mode of request: SYNC/ASYNC/IASYNC    */
     char               *urlName;	/* url to retrieve/or that was retrieved */
     TIcbf              *incremental_cbf;	/* For IASYNC mode, @ of callback function */
     /* It'll be called each time a new data package */
     /* is received                                  */
     void               *context_icbf;	/* Context for the above cbf                  */
     TTcbf              *terminate_cbf;		/* optional CBF which will be invoked after  */
     /* a file has been received                  */
     void               *context_tcbf;	/* Context for the above cbf                 */

     /* The following elements are used for the PUT and POST */

     HTParentAnchor     *dest;	/* Destination for PUT etc.              */
     unsigned long       block_size;	/* size in bytes of the file to put      */
     int                 put_counter;	/* number of bytes already put           */
     char               *mem_ptr;	/* ptr to a struct in mem which contains a copy */
     /* of the file to put                           */
/* For debugging */
     int                *s;	/* socket number                                */
     char               *error_stream;
     int                 error_stream_size;
     BOOL                error_html;
  }
AHTReqContext;

#define NO               0
#define YES              1

/* dialogue */
#define FormURL          1
#define FormOuvrir       2
#define NomURL           3
#define NomLocal         4
#define SelDir           5
#define SelDoc           6
#define StopCommand      7
#define FormSauver       8
#define SauvDir          9
#define SauvDoc         10
#define SauvToggle      11
#define SauvNom         12
#define SauvLbl1        13
#define SauvLbl2        14
#define SauvImgsDir     14
#define SauvLbl3        15
#define SauvLbl4        16
#define SauvLbl5        17
#define FormConfirmer   18
#define TexteConfirmer  19
#define FormAttrHREF    20
#define TextAttrHREF    21
#define FormAnswer      22
#define TextLabel       23
#define TextAnswer      24
#define TextName        25
#define TextPassword    26
#define FormClass       27
#define ClassLabel      28
#define ClassSelect     29
#define FormAClass      30
#define AClassLabel     31
#define AClassSelect    32
#define SauvConfirm     33
#define SauvConfirmLbl  34
#define SauvConfirmList 35
#define MenuOption	36
#define MAX_REF         40


#define MAX_LENGTH     512
#define NAME_LENGTH     32

#define HTAppName "amaya"
#define HTAppVersion "V0.8 Alpha"

#ifndef EXPORT
#define EXPORT extern
#endif /* VSTAUS */

EXPORT int          appArgc;
EXPORT char       **appArgv;
EXPORT char         TempFileDirectory[MAX_LENGTH];
EXPORT char         Answer_text[MAX_LENGTH];
EXPORT char         Answer_name[NAME_LENGTH];
EXPORT char         Answer_password[NAME_LENGTH];
EXPORT char         Display_password[NAME_LENGTH];
EXPORT int          Lg_password;
EXPORT int          BaseDialog;
EXPORT char        *LastURLName;	/* last URL requested               */
EXPORT char        *DirectoryName;	/* local path of the document       */
EXPORT char        *DocumentName;	/* document name                    */
EXPORT char        *ObjectName;	/* document name                    */
EXPORT char        *SaveImgsURL;	/* where to save remote Images      */
EXPORT int          CopyImages;	/* should we copy images in Save As */
EXPORT int          UpdateURLs;	/* should we update URLs in Save As */
EXPORT boolean      UserAnswer;
EXPORT int          ReturnOption;
EXPORT boolean      InNewWindow;
EXPORT Document     CurrentDocument;
EXPORT Document     SavingDocument;
EXPORT Document     SavingObject;
EXPORT char        *SavingFile;	/* complete path or URL of the document */
EXPORT Document     AttrHREFdocument;
EXPORT Element      AttrHREFelement;
EXPORT char        *AttrHREFvalue;
EXPORT Document     SelectionDoc;
EXPORT boolean      SelectionInPRE;
EXPORT boolean      SelectionInComment;
EXPORT boolean      SelectionInEM;
EXPORT boolean      SelectionInSTRONG;
EXPORT boolean      SelectionInCITE;
EXPORT boolean      SelectionInDFN;
EXPORT boolean      SelectionInCODE;
EXPORT boolean      SelectionInVAR;
EXPORT boolean      SelectionInSAMP;
EXPORT boolean      SelectionInKBD;
EXPORT boolean      SelectionInI;
EXPORT boolean      SelectionInB;
EXPORT boolean      SelectionInTT;
EXPORT boolean      SelectionInSTRIKE;
EXPORT boolean      SelectionInBIG;
EXPORT boolean      SelectionInSMALL;

#define DocumentTableLength 10
EXPORT char        *DocumentURLs[DocumentTableLength];

/* The whole document is loaded when the corresponding value
   in FilesLoading is equal to 0 */
EXPORT int          FilesLoading[DocumentTableLength];
EXPORT Document     W3Loading;	/* the document being loaded */
EXPORT int          IButton;
EXPORT int          BButton;
EXPORT int          TTButton;

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

EXPORT LoadedImageDesc *ImageURLs;

extern void         InitMapping ();
extern void         InitImage ();
extern void         InitTransform ();


#ifdef __STDC__
extern int          PtEnPixel (int value, int horiz);	/* thot internal function */
extern int          PixelEnPt (int value, int horiz);	/* thot internal function */
extern void         CreateNewElement (int, SSchema, Document);
extern void         SelectInitial (Document);
extern int          TtaFileExist (char *);
extern boolean      OpenMainView (Document, char *, char *);
extern void         SetTarget (Document, View);
extern void         SetTableWidths (Document);
extern void         CheckTable (Element, Document);
extern boolean      DoubleClick (NotifyElement *);
extern void         RemoveAttrInPair (Element, Document, int);
extern void         SelectDestination (Document, Element);
extern void         SetCharAttribute (int, int, int);
extern int          GetObjectWWW (int, char *, char *, char *, int, TIcbf *, void *, TTcbf *, void *, BOOL);
extern int          PutObjectWWW (int, char *, char *, int, TTcbf *, void *);
extern int          UploadMemWWW (int, HTMethod, char *, char *, unsigned long, int, TTcbf *, void *, char *);
extern AHTDocId_Status *GetDocIdStatus (int, HTList *);
extern BOOL         AHTReqContext_delete (AHTReqContext *);
extern void         AHTPrintPendingRequestStatus (Document, boolean);
extern boolean      IsSameHost (char *, char *);
extern void         StopRequest (int);
extern void         AHTError_MemPrint (HTRequest *);
extern HTStream    *AHTFWriter_new (HTRequest *, FILE *, BOOL);
extern int          Add_NewSocket_to_Loop (HTRequest *, HTAlertOpcode, int, const char *, void *, HTAlertPar *);
extern void         CreateAnchor (Document, View, boolean);
extern void         SetCharFontOrPhrase (int, int);
extern void         InsertChar (unsigned char, int);
extern void         ClearAllViewSelection (void);
extern int          TtaFileUnlink (char *);
extern void         QuitEditor (void);
extern void         CloseInsertion (void);
extern char        *ThotRealloc (char *, unsigned int);
extern int          TtaGiveRGB (char *colname, unsigned short *, unsigned short *, unsigned short *);
extern int          GetWindowFrame (ThotWindow);
extern void         GetSizesFrame (int, int *, int *);
extern void         DefClip (int, int, int, int, int);
extern void         FrameResized (int *, int, int *);
extern char        *TtaStrdup (char *str);
extern void         MakeCompleteName (char *, char *, PathBuffer, PathBuffer, int *);
extern void         ColorRGB (int num, unsigned short *red, unsigned short *green, unsigned short *blue);
extern void         TtaSetAttributeReference (Attribute attribute, Element element, Document document, Element target, Document targetDocument);

#else  /* __STDC__ */
extern int          PtEnPixel ();	/* thot internal function */
extern int          PixelEnPt ();	/* thot internal function */
extern void         CreateNewElement ();
extern void         SelectInitial ();
extern int          TtaFileExist ();
extern boolean      OpenMainView ();
extern void         SetTarget ();
extern void         SetTableWidths ();
extern void         CheckTable ();
extern boolean      DoubleClick ();
extern void         RemoveAttrInPair ();
extern void         SelectDestination ();
extern void         SetCharAttribute ();
extern int          GetObjectWWW ();
extern int          PutObjectWWW ();
extern int          UploadMemWWW ();
extern AHTDocId_Status *GetDocIdStatus ();
extern BOOL         AHTReqContext_delete ();
extern void         AHTPrintPendingRequestStatus ();
extern boolean      IsSameHost ();
extern void         StopRequest ();
extern void         AHTError_MemPrint ();
extern HTStream    *AHTFWriter_new ();
extern int          Add_NewSocket_to_Loop ();
extern void         CreateAnchor ();
extern void         SetCharFontOrPhrase ();
extern void         InsertChar ();
extern void         ClearAllViewSelection ();
extern int          TtaFileUnlink ();
extern void         QuitEditor ();
extern void         CloseInsertion ();
extern char        *ThotRealloc ();
extern int          TtaGiveRGB ();
extern int          GetWindowFrame ();
extern void         GetSizesFrame ();
extern void         DefClip ();
extern void         FrameResized ();
extern char        *TtaStrdup ();
extern void         MakeCompleteName ();
extern void         ColorRGB ();
extern void         TtaSetAttributeReference ();

#endif /* __STDC__ */

extern void         QueryInit ();
extern void         QueryClose ();

#ifndef _WINDOWS
#ifdef __STDC__
extern XtInputCallbackProc AHTCallback_bridge (caddr_t, int *, XtInputId *);

#else
extern XtInputCallbackProc AHTCallback_bridge ();

#endif /* __STDC__ */
#else    /* _WINDOWS */
LONG                AHTCallback_bridge (caddr_t, int *);
#endif   /* !_WINDOWS */

extern HTList      *conv;	/* List of global converters */
extern AmayaContext *Amaya;	/* Amaya's global context    */

#define EOS     '\0'
#define EOL     '\n'
#define TAB     '\t'
#define SPACE    ' '
#define boolean unsigned char

#endif /* AMAYA_H */
