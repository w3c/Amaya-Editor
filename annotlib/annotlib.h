#ifndef ANNOTATIONS_H
#define ANNOTATIONS_H
/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1999.
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
#include "amaya.h"

/* RDF Property names */

#define RDFMS_NS "http://www.w3.org/1999/02/22-rdf-syntax-ns#"
#define RDF_TYPE   RDFMS_NS "type"
#define RDFS_LABEL "http://www.w3.org/2000/01/rdf-schema#label"
#define RDFS_SUBCLASSOF "http://www.w3.org/2000/01/rdf-schema#subClassOf"

#define DC_NS "http://purl.org/dc/elements/1.0/"
#define HTTP_NS "http://www.w3.org/1999/xx/http#"

#define ANNOT_LOCAL_NAME "Annotation"
#define FALLBACK_ANNOTATION_NS "http://www.w3.org/1999/xx/annotation-ns#"
#define FALLBACK_ANNOTATION_CLASSNAME  FALLBACK_ANNOTATION_NS ANNOT_LOCAL_NAME 

#define ID_NS "http://www.w3.org/2000/08/palm56/addr#"
#define EMAIL_PROPNAME  ID_NS "E-mail"
#define NAME_PROPNAME  ID_NS "name"
#define FIRSTNAME_PROPNAME  ID_NS "firstName"

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
  RDFClassExtP class;		/* if type->Class, points to more data */
#if 0
  ThotBool isLiteral;		/* mostly a guess */
#endif
} RDFResource, *RDFResourceP,
  RDFClass, *RDFClassP,
  RDFProperty, *RDFPropertyP;

/* the info we're interested in in an annotation */
typedef struct _AnnotMeta {
  ThotBool is_visible; /* if not set, this annotation is only used when
			  saving the local annotations */
  ThotBool is_orphan; /* set to true if the XPointer for this annotation
			 doesn't resolve anymore */
  ThotBool show;      /* this annotation is visible in the formatted view */

  CHAR_T *annot_url; /* url of the annotation node */
  CHAR_T *source_url; /* document that was annotated */
  /* the internal Amaya Xpath */
  CHAR_T labf[10]; 
  int     c1;
  CHAR_T labl[10];
  int     cl;
  CHAR_T *xptr;
  CHAR_T *cdate; /* creation date of the annotation */
  CHAR_T *mdate; /* last modified date of the annotation */
  CHAR_T *author; /* author of the annotation */
  RDFResourceP creator; /* creator of the annotation */
  RDFResourceP type; /* type of annotation */
  CHAR_T *content_type; /*content type of the body of the annotation,
			  only used while posting */
  CHAR_T *content_length; /* content length of the body, only used while
			     posting */
  CHAR_T *body; /* when reading an annotation with an embedded body,
		   the body is stored in this variable */
  CHAR_T *body_url; /* if the body isn't embedded, this has the URL
		       to the body (only used for local files for the moment */
  CHAR_T *name;  /* the value of the name tag added to the source document 
		  for making a reverse link */
} AnnotMeta;

/* the different kind of annotation searches we can do in an
   an annotation metadata list */
typedef enum _AnnotMetaDataSearch {
  AM_ANNOT_URL = 1, /* compare with the annot_url field */
  AM_BODY_URL = 2,  /* compare with the body_url field */
  AM_BODY_FILE = 4, /* cmopare with the body_url field, but skipping
		       the file:// prefix */
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
  CHAR_T *object;
  ThotBool show;
} AnnotFilterData;

AnnotMetaDataList AnnotMetaData[DocumentTableLength];

/* Definition de constantes pour les annotations */

#define ANNOT_DIR  "annotations"
#define ANNOT_MAIN_INDEX  "annot.index"
#define ANNOT_INDEX_SUFFIX ".index"
#define ANNOT_USER  "amaya"
#define LINK_IMAGE "target.gif"
#define ANNOT_ANAME "Annotation"

/* RDF Schema entry */

extern List *annot_schema_list;  /* a list of schemas */
extern CHAR_T *ANNOT_NS;
extern CHAR_T *ANNOTATION_CLASSNAME;
extern RDFClassP ANNOTATION_CLASS;
extern RDFClassP DEFAULT_ANNOTATION_TYPE;
extern RDFPropertyP PROP_Email;
extern RDFPropertyP PROP_name;
extern RDFPropertyP PROP_firstName;

typedef struct _RDFStatement
{
  RDFPropertyP predicate;
#if 0
  RDFResourceP subject;		/* subject is given by the relations list */
#endif
  RDFResourceP object;
} RDFStatement, *RDFStatementP;

#endif /* ANNOTATIONS_H */
