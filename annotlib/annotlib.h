#ifndef ANNOTATIONS_H
#define ANNOTATIONS_H
/* 
 *
 *  (c) COPYRIGHT MIT and INRIA, 1999-2005.
 *  Please first read the full copyright statement in file COPYRIGHT.
 * 
 */

/*
 * Author: J. Kahan (W3C/INRIA)
 *
 * Acknowledgments: inspired from code written by Christophe Marjoline 
 *                   for the byzance collaborative work application
 */

#define THOT_EXPORT extern

/* turn this off to remove the reply to annotations feature */
#ifndef ANNOT_ON_ANNOT
#define ANNOT_ON_ANNOT
#endif

#include "amaya.h"

/* RDF Property names */

#define RDFMS_NS "http://www.w3.org/1999/02/22-rdf-syntax-ns#"
#define RDF_TYPE    RDFMS_NS "type"
#define RDFMS_TYPE  "type"
#define RDF_SEQ     RDFMS_NS "Seq"
#define RDF_NODEID  RDFMS_NS "nodeID"
#define RDF_FIRST   RDFMS_NS "first"
#define RDF_REST    RDFMS_NS "rest"
#define RDF_NIL     RDFMS_NS "nil"
#define RDF_DESCRIPTION RDFMS_NS "Description"

#define RDFS_LABEL "http://www.w3.org/2000/01/rdf-schema#label"
#define RDFS_SUBCLASSOF "http://www.w3.org/2000/01/rdf-schema#subClassOf"
#define RDFS_SEEALSO "http://www.w3.org/2000/01/rdf-schema#seeAlso"

/* DC Property names -- note that we use / rather than # here */
#define DC0_NS "http://purl.org/dc/elements/1.0/"
#define DC1_NS "http://purl.org/dc/elements/1.1/"
#define DC_NS DC1_NS
#define DC_CREATOR "creator"
#define DCNS_CREATOR DC_NS""DC_CREATOR
#define DC0NS_CREATOR DC0_NS""DC_CREATOR
#define DC1NS_CREATOR DC1_NS""DC_CREATOR
#define DC_DATE "date"
#define DCNS_DATE DC_NS""DC_DATE
#define DC0NS_DATE DC0_NS""DC_DATE
#define DC1NS_DATE DC1_NS""DC_DATE
#define DC_TITLE "title"
#define DCNS_TITLE DC_NS""DC_TITLE
#define DC0NS_TITLE DC0_NS""DC_TITLE
#define DC1NS_TITLE DC1_NS""DC_TITLE
#define DC_DESCRIPTION "description"
#define DCNS_DESCRIPTION DC_NS""DC_DESCRIPTION
#define DC0NS_DESCRIPTION DC0_NS""DC_DESCRIPTION
#define DC1NS_DESCRIPTION DC1_NS""DC_DESCRIPTION

/* Annotea NS property names */
#define ANNOT_NS1 "http://www.w3.org/2000/10/annotation-ns"
#define ANNOT_ANNOTATES "annotates"
#define ANNOT_BODY "body"
#define ANNOT_CONTEXT "context"
#define ANNOTNS_CONTEXT ANNOT_NS1"#"ANNOT_CONTEXT
#define ANNOT_CREATED  "created"
#define ANNOTNS_CREATED ANNOT_NS1"#"ANNOT_CREATED
#define ANNOT_ANNOTATION "Annotation"

#ifdef ANNOT_ON_ANNOT
#define THREAD_NS "http://www.w3.org/2001/03/thread#"
#define THREAD_REPLY "Reply"
#define THREAD_ROOT "root"
#define THREAD_INREPLYTO "inReplyTo"
#define THREAD_REPLY_LOCAL_NAME "Reply"
#endif /* ANNOT_ON_ANNOT */


/* HTTP_NS property names */
#define HTTP_NS "http://www.w3.org/1999/xx/http#"
#define HTTP_BODY "Body"
#define HTTP_CONTENT_LENGTH "ContentLength"
#define HTTP_CONTENT_TYPE   "ContentType"

#define ANNOT_LOCAL_NAME "Annotation"
#define FALLBACK_ANNOTATION_NS "http://www.w3.org/1999/xx/annotation-ns#"
#define FALLBACK_ANNOTATION_CLASSNAME  FALLBACK_ANNOTATION_NS ANNOT_LOCAL_NAME 

#define ID_NS "http://www.w3.org/2000/08/palm56/addr#"
#define EMAIL_PROPNAME  ID_NS "E-mail"
#define NAME_PROPNAME  ID_NS "name"
#define FIRSTNAME_PROPNAME  ID_NS "firstName"

#define USESICON_PROPNAME "http://www.w3.org/2001/10/typeIcon#usesIcon"

/* Structures and global variables */

/* basic linked list structure */
typedef struct _List {
  void *object;
  struct _List *next;
} List;

typedef struct _RDFClassExt
{
  List *instances;		/* each item is an RDFResourceP */
  List *subClasses;		/* each item is an RDFResourceP */
} RDFClassExt, *RDFClassExtP;

typedef struct _RDFResource
{
  char *name;
  List *statements;		/* each item is an RDFStatementP */
  RDFClassExtP class_;		/* if type->Class, points to more data */
#if 0
  ThotBool isLiteral;		/* mostly a guess */
#endif
} RDFResource, *RDFResourceP,
  RDFClass, *RDFClassP,
  RDFProperty, *RDFPropertyP;

/* linked list of all annotation threads related to a document */
typedef struct _AnnotThreadList {
  /* the list of all the replies related to a document */
  List *annotations;      /* a list of all the annotations belonging
			     to this thread */
  char *rootOfThread;     /* url of the root of thread */
  int  references;       /* how many times is this thread referenced? */
} AnnotThreadList;

/* the info we're interested in in an annotation */
typedef struct _AnnotMeta {
  ThotBool is_visible; /* if not set, this annotation is only used when
			  saving the local annotations */
  ThotBool is_orphan; /* set to true if the XPointer for this annotation
			 doesn't resolve anymore */
  ThotBool is_orphan_item; /* set to true if the thread item has lost its parent */
  ThotBool show;      /* this annotation is visible in the formatted view */

  char *title;     /* title of the annotation */
  char *annot_url; /* url of the annotation node */
  char *source_url; /* document that was annotated */
  /* the internal Amaya Xpath */
  char labf[10]; 
  int     c1;
  char labl[10];
  int     cl;
  char *xptr;
  char *cdate; /* creation date of the annotation */
  char *mdate; /* last modified date of the annotation */
  char *author; /* author of the annotation */
  RDFResourceP creator; /* creator of the annotation */
  RDFResourceP type; /* type of annotation */
  DocumentType bodyType;  /* the documen type (as Amaya understands it) of the body */
  char *content_type; /*content type of the body of the annotation,
			  only used while posting */
  char *content_length; /* content length of the body, only used while
			     posting */
  char *body; /* when reading an annotation with an embedded body,
		   the body is stored in this variable */
  char *body_url; /* if the body isn't embedded, this has the URL
		       to the body (only used for local files for the moment */
  char *name;  /* the value of the name tag added to the source document 
		  for making a reverse link */
#ifdef ANNOT_ON_ANNOT
  char *rootOfThread;   /* the URL of the root of the thread */
  char *inReplyTo;   /* if the annotation is a reply, this field gets
			  the URL of the annotation we're replying to */
  /* a pointer to the thread to which this annotation belongs */
  AnnotThreadList *thread;
#endif /* ANNOT_ON_ANNOT */
} AnnotMeta;

/* the different kind of annotation searches we can do in an
   an annotation metadata list */
typedef enum _AnnotMetaDataSearch {
  AM_ANNOT_URL = 1, /* compare with the annot_url field */
  AM_BODY_URL = 2,  /* compare with the body_url field */
#if 0  /* JK: not used anymore */
  AM_BODY_FILE = 4, /* cmopare with the body_url field, but skipping
		       the file:// prefix */
#endif
  AM_ANAME = 8      /* compare with the name field */
} AnnotMetaDataSearch;

/* the type of character convertion we want to make on local URLs */
typedef enum _AnnotFileType
{
  ANNOT_SINGLE = 0, /* the file contains a single annotation */
  ANNOT_LIST = 1    /* the file contains a list of annotations */
}
AnnotFileType;

/* the selector type used to show/hide annotations */
typedef enum _SelType {
  BY_AUTHOR = 0,
  BY_TYPE,
  BY_SERVER
} SelType;

/* linked list of all annotations related to a document */
typedef struct _AnnotMetaDataList {
  /* the list of all the annotations related to a document */
  List *annotations;
#ifdef ANNOT_ON_ANNOT
  /* the threads */
  AnnotThreadList *thread;
#endif /*ANNOT_ON_ANNOT */
  /* the URL of the annotation metadata */
  char *annot_url; 
  /* filter information */
  List *authors;
  List *types;
  List *servers;
  List *rdf_model;
  ThotBool local_annot_loaded; /* if set to true, means we have tried to
				  load the local annotations related to this
				  document */
} AnnotMetaDataList;

/*************
 ** Annot Filter menu
 *************/
typedef struct _AnnotFilter {
  char *object;
  ThotBool show;
} AnnotFilterData;

extern AnnotMetaDataList AnnotMetaData[DocumentTableLength];
#ifdef ANNOT_ON_ANNOT
extern AnnotThreadList   AnnotThread[DocumentTableLength];
#endif /* ANNOT_ON_ANNOT */

/***************
 ** Annotation creation/browsing modes
 **************/
typedef enum _AnnotMode {
  ANNOT_useSelection = 0,
  ANNOT_useDocRoot = 1,
  ANNOT_isReplyTo = 2,
  ANNOT_initNone = 4,
  ANNOT_initATitle = 8,
  ANNOT_initBody = 16
} AnnotMode;

/* Annotation constants */

#define ANNOT_DIR  "annotations"
#define ANNOT_MAIN_INDEX  "annot.index"
#define ANNOT_INDEX_SUFFIX ".index"
#define ANNOT_USER  "amaya"
#define ANNOT_ANAME "Annotation"

/* RDF Schema entry */

extern List *annot_schema_list;  /* a list of schemas */
extern char *ANNOT_NS;
extern char *ANNOTATION_CLASSNAME;
extern RDFClassP ANNOTATION_CLASS;
extern RDFClassP DEFAULT_ANNOTATION_TYPE;
extern RDFPropertyP PROP_Email;
extern RDFPropertyP PROP_name;
extern RDFPropertyP PROP_firstName;
extern RDFPropertyP PROP_usesIcon;

#ifdef ANNOT_ON_ANNOT
extern RDFClassP THREAD_REPLY_CLASS;
extern RDFClassP DEFAULT_REPLY_TYPE;
#endif /* ANNOT_ON_ANNOT */

typedef struct _RDFStatement
{
  RDFPropertyP predicate;
#if 0
  RDFResourceP subject;		/* subject is given by the relations list */
#endif
  RDFResourceP object;
} RDFStatement, *RDFStatementP;


#define AM_RAPTOR_URI_AS_STRING(uri) raptor_uri_as_string ((raptor_uri *) uri)

#endif /* ANNOTATIONS_H */
