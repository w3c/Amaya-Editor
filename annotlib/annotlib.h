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
#if defined(AMAYA_JAVA) || defined(AMAYA_ILU)
#else
#include "query_f.h"
#endif /* AMAYA_JAVA */
#include "EDITORactions_f.h"
#include "HTMLactions_f.h"
#include "HTMLedit_f.h"
#if 0
#include "HTMLstyle_f.h"
#endif
#include "HTMLtable_f.h"

/* Annotation modules */
#include "ANNOTtools_f.h"
#include "ANNOTlink_f.h"
#include "ANNOTevent_f.h"
#include "ANNOTfiles_f.h"

/* Structures and global variables */

typedef struct _refAnnot
{
  char *docName;
  char labf[20], labl[20];
  int  c1, cN;
  int annotNum;
} refAnnot;

refAnnot tabRefAnnot[10]; /* Tableau contenant les zones de chaque annotation */

/* Definition de constantes pour les annotations */

#define ANNOT_DIR  "annotations"
#define ANNOT_MAIN_INDEX  "annot.index"
#define ANNOT_INDEX_SUFFIX ".index"
#define ANNOT_USER  "amaya"
#define LINK_IMAGE "target.gif"
#define ANNOT_ANAME "Annotation"

#endif /* ANNOTATIONS_H */
