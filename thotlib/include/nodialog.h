#ifndef __NODIALOG_H__
#define __NODIALOG_H__

#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "style.h"
#include "picture.h"
#include "frame.h"
#include "application.h"
#include "memory_f.h"

/* ------------------ AMAYA STRUCT : BEGIN ---------------------- */
#define DocumentTableLength MAX_DOCUMENTS
typedef enum
{
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
  docLibrary,
  docBookmark,
  docJavascript
} DocumentType;

/* The different events to open a new document */
typedef enum _ClickEvent {
  CE_ABSOLUTE, CE_RELATIVE, CE_FORM_POST, CE_FORM_GET,
  CE_HELP, CE_MAKEBOOK, CE_LOG , CE_TEMPLATE, CE_INIT,
  CE_CSS, CE_ANNOT
} ClickEvent;

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
  ClickEvent method;           /* method used to send this data */
  ThotBool   xmlformat;        /* the document should be exported in xml format */
#ifdef ANNOTATIONS
  Document   source_doc;       /* if the document is an annotation,
				  this variable giveso the annoted document
				  docid */
#endif /* ANNOTATIONS */
} DocumentMetaDataElement;
/* ------------------ AMAYA STRUCT : END ---------------------- */

/* ------------------ CSS STRUCT : BEGIN ---------------------- */
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
  ThotBool            import; /* import entry */
} CSSInfo , *CSSInfoPtr;
/* ------------------ CSS STRUCT : END ---------------------- */


/* one callback proto for image handeling */
typedef void (*LoadedImageCallback)(Document doc, Element el, char *file,
				    void *extra, ThotBool isnew);

/* extern declaration : => link with css.c function */
extern void LoadStyleSheet (char *url, Document doc, Element link, CSSInfoPtr css,
			    char *urlRef, CSSmedia media, ThotBool user);


#endif /* __NODIALOG_H__ */
