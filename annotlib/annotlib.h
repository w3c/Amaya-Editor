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
  CHAR_T *about;
  CHAR_T *source_url;
  CHAR_T labf[10];
  int     c1;
  CHAR_T labl[10];
  int     cl;
  CHAR_T *date;
  CHAR_T *author;
  CHAR_T *annotFile;
  CHAR_T *type;
  CHAR_T *content_type;
  CHAR_T *content_length;
  CHAR_T *body;
  CHAR_T *body_url;
  struct _AnnotMeta *next;
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
#include "rdf2annot_f.h"

/* linked list of all annotations related to a document */
AnnotMeta *AnnotMetaDataList[DocumentTableLength];

/* Definition de constantes pour les annotations */

#define ANNOT_DIR  "annotations"
#define ANNOT_MAIN_INDEX  "annot.index"
#define ANNOT_INDEX_SUFFIX ".index"
#define ANNOT_USER  "amaya"
#define LINK_IMAGE "target.gif"
#define ANNOT_ANAME "Annotation"

#endif /* ANNOTATIONS_H */







