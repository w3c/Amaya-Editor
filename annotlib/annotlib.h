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
/* Structures and global variables */

/* the info we're interested in in an annotation */
typedef struct _AnnotMeta {
  int     annotNum;
  CHAR_T *annot_url; /* url of the annotation node */
  CHAR_T *source_url; /* document that was annotated */
  /* the internal Amaya Xpath */
  CHAR_T labf[10]; 
  int     c1;
  CHAR_T labl[10];
  int     cl;
  CHAR_T *cdate; /* creation date of the annotation */
  CHAR_T *mdate; /* last modified date of the annotation */
  CHAR_T *author; /* author of the annotation */
  CHAR_T *type; /* type of annotation */
  CHAR_T *content_type; /*content type of the body of the annotation,
			  only used while posting */
  CHAR_T *content_length; /* content length of the body, only used while
			     posting */
  CHAR_T *body; /* when reading an annotation with an embedded body,
		   the body is stored in this variable */
  CHAR_T *body_url; /* if the body isn't embedded, this has the URL
		       to the body (only used for local files for the moment */
} AnnotMeta;

/* basic linked list structure */
typedef struct _List {
  void *object;
  struct _List *next;
} List;

/* the type of character convertion we want to make on local URLs */
typedef enum _AnnotFileType
{
  ANNOT_SINGLE = 0, /* the file contains a single annotation */
  ANNOT_LIST = 1    /* the file contains a list of annotations */
}
AnnotFileType;

/* Annotation modules */
#include "ANNOTtools_f.h"
#include "ANNOTlink_f.h"
#include "ANNOTevent_f.h"
#include "ANNOTfiles_f.h"
#include "ANNOTmenu_f.h"
#include "rdf2annot_f.h"

/* linked list of all annotations related to a document */
List *AnnotMetaDataList[DocumentTableLength];

/* Definition de constantes pour les annotations */

#define ANNOT_DIR  "annotations"
#define ANNOT_MAIN_INDEX  "annot.index"
#define ANNOT_INDEX_SUFFIX ".index"
#define ANNOT_USER  "amaya"
#define LINK_IMAGE "target.gif"
#define ANNOT_ANAME "Annotation"

#endif /* ANNOTATIONS_H */







