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

/* app generated files */
#include "HTML.h"
#include "MathML.h"
#include "GraphML.h"
#include "XLink.h"
#include "Annot.h"

/* Amaya modules */

#define THOT_EXPORT extern
#include "amaya.h"

#ifdef _WINDOWS
#include "wininclude.h"
#endif /* _WINDOWS */

#include "html2thot_f.h"
#include "init_f.h"
#include "query_f.h"
#include "EDITORactions_f.h"
#include "HTMLactions_f.h"
#include "HTMLedit_f.h"
#include "HTMLtable_f.h"
#

/* RDF Property names */
#define DEFAULT_ANNOT_TYPE TEXT("http://www.w3.org/1999/xx/annotation-ns#Comment")
#define RDF_TYPE  TEXT("http://www.w3.org/1999/02/22-rdf-syntax-ns#type")
#define RDFS_LABEL TEXT("http://www.w3.org/2000/01/rdf-schema#label")
#define RDFS_SUBCLASSOF TEXT("http://www.w3.org/2000/01/rdf-schema#subClassOf")
#define ANNOTATION_PROP TEXT("http://www.w3.org/1999/xx/annotation-ns#Annotation")
#define FALLBACK_ANNOTATION_NS "http://www.w3.org/1999/xx/annotation-ns#"
#define FALLBACK_ANNOTATION_PROP TEXT(FALLBACK_ANNOTATION_NS "Annotation")

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
  ThotBool is_visible; /* if set, this annotation is not shown
			 on the filters */
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
  ThotBool local_annot_loaded; /* if set to true, means we have tried to
				  load the local annotations related to this
				  document */
} AnnotMetaDataList;

/* Annotation modules */
#include "ANNOTtools_f.h"
#include "ANNOTlink_f.h"
#include "ANNOTevent_f.h"
#include "ANNOTfiles_f.h"
#include "ANNOTmenu_f.h"
#include "AHTrdf2annot_f.h"

/*************
 ** Annot Filter menu
 *************/
typedef struct _AnnotFilter {
  CHAR_T *object;
  ThotBool show;
  AnnotMeta *annot;
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

typedef struct _RDFStatement
{
  RDFPropertyP predicate;
#if 0
  RDFResourceP subject;		/* subject is given by the relations list */
#endif
  RDFResourceP object;
} RDFStatement, *RDFStatementP;

#include "ANNOTschemas_f.h"

#endif /* ANNOTATIONS_H */




