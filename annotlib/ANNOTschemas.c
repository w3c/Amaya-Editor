/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 2000.
 *  Please first read the full copyright statement in file COPYRIGHT.
 * 
 */

/*
 * ANNOTschemas.c : parses an RDF Schema describing some annotation
 * classes and properties.
 *
 * NOTE: the code assumes libwww's RDF parser.
 *
 * Author: Ralph R. Swick (W3C/MIT)
 * $Revision$
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "xmlparse.h"
#include "annotlib.h"
#include "HTRDF.h"

/********************** static variables ***********************/

/* 
 * Namespace and Property names used in an Annotation Description 
 */
static const char * DC_NS = "http://purl.org/dc/elements/1.0/";
static const char * ANNOT_NS = "http://www.w3.org/1999/xx/annotation-ns#";
static const char * RDFMS_NS = "http://www.w3.org/1999/02/22-rdf-syntax-ns#";

static RDFResourceP subclassOfP = NULL;

/********************** global variables ***********************/

List *annot_schema_list = NULL;  /* a list of schemas */

typedef struct _ReadCallbackContext
{
  char filename[MAX_LENGTH];
} ReadCallbackContext;

/*------------------------------------------------------------
   _ListSearchResource
  ------------------------------------------------------------*/
static RDFResourceP _ListSearchResource( List* list, char* name)
{
  List *item = list;
  RDFResourceP resource;
  ThotBool found = FALSE;

  while (item)
    {
      resource = (RDFResourceP) item->object;
      if (resource && resource->name 
		  && ustrcmp (resource->name, name))
	{
	  found = TRUE;
	  break;
	}
      item = item->next;
    }

  return found ? resource : NULL;
}

/*------------------------------------------------------------
   _AddStatement
  ------------------------------------------------------------*/
static void _AddStatement( RDFResourceP s, RDFPropertyP p, RDFResourceP o )
{
  RDFStatement *statement = (RDFStatement*)TtaGetMemory (sizeof(RDFStatement));

  statement->predicate = p;
  statement->object = o;
  List_add (&s->statements, (void*)statement);
}

/*------------------------------------------------------------
   _AddInstance
  ------------------------------------------------------------*/
static void _AddInstance( RDFClassP class, RDFResourceP instance )
{
  if (!class->class)
    {
      class->class = (RDFClassExtP) TtaGetMemory (sizeof(RDFClassExt));
      memset (class->class, 0, sizeof (RDFClassExt));
    }

  List_add (&class->class->instances, (void*)instance);
}

/*------------------------------------------------------------
   _AddSubClass
  ------------------------------------------------------------*/
static void _AddSubClass( RDFClassP class, RDFClassP sub )
{
  if (!class->class)
    {
      class->class = (RDFClassExtP)TtaGetMemory (sizeof(RDFClassExt));
      class->class->instances = NULL;
      class->class->subClasses = NULL;
    }

  List_add (&class->class->subClasses, (void*)sub);
}

/* ------------------------------------------------------------
   triple_handler

   This callback is invoked when a new RDF triple has been parsed.
  
   As the triples arrive, their predicate is checked and its
   object is cached.
  
   Parameters:
     rdfp - the RDF parser
     triple - an RDF triple
     context - pointer to an RDFResource list
 ------------------------------------------------------------*/
static void triple_handler (HTRDF * rdfp, HTTriple * triple, void * context)
{
  List **listP = (List**)context;
  static RDFResourceP typeP = NULL;

  if (rdfp && triple) 
    {
      char * predicate = HTTriple_predicate(triple);
      char * subject = HTTriple_subject(triple);
      char * object = HTTriple_object(triple);

      RDFResourceP subjectP = ANNOT_FindRDFResource (listP, subject, TRUE);
      RDFPropertyP predicateP = ANNOT_FindRDFResource (listP, predicate, TRUE);
      /* ugly, ugly; libwww discards info -- is the object a Literal? */
      RDFResourceP objectP = ANNOT_FindRDFResource (listP, object, TRUE);

      _AddStatement (subjectP, predicateP, objectP);

      if (!typeP)
	  typeP = ANNOT_FindRDFResource (listP, RDF_TYPE, TRUE);

      if (!subclassOfP)
	subclassOfP = ANNOT_FindRDFResource (listP, RDFS_SUBCLASSOF, TRUE);

      if (predicateP == typeP)
	_AddInstance( objectP, subjectP );
      else if (predicateP == subclassOfP)
	{
	  _AddSubClass( objectP, subjectP );
	  subjectP->class = (RDFClassExtP)TtaGetMemory (sizeof(RDFClassExt));
	  subjectP->class->instances = NULL;
	  subjectP->class->subClasses = NULL;
	}
    }
}


/*-----------------------------------------------------------------------
  ReadSchema_callback
  -----------------------------------------------------------------------*/
#ifdef __STDC__
static void ReadSchema_callback (int doc, int status, 
				 CHAR_T *urlName,
				 CHAR_T *outputfile, 
				 AHTHeaders *http_headers,
				 void * context)
#else  /* __STDC__ */
static void ReadSchema_callback (doc, status, urlName,
				 outputfile, http_headers,
				 context)
int doc;
int status;
CHAR_T *urlName;
CHAR_T *outputfile;
AHTHeaders *http_headers;
void *context;

#endif
{
   ReadCallbackContext *ctx = (ReadCallbackContext*) context;
   BOOL parse = NO;

   if (!ctx)
     return;

   if (status == HT_OK)
     parse = HTRDF_parseFile (ctx->filename,
			      triple_handler,
			      &annot_schema_list);

   if (parse)
     TtaSetStatus (doc, 1, "Schema read", NULL); /* @@ */
   else
     TtaSetStatus (doc, 1, "Error during schema read", NULL); /* @@ */

   TtaFreeMemory (ctx);
}

/********************** Public API entry point ********************/

/*------------------------------------------------------------
   ANNOT_FindRDFResource
  ------------------------------------------------------------*/
#ifdef __STDC__
RDFResourceP ANNOT_FindRDFResource( List** listP, char* name, ThotBool create )
#else /* __STDC__ */
RDFResourceP ANNOT_FindRDFResource( listP, name, create )
     List** listP;
     char* name;
     ThotBool create;
#endif /* __STDC__ */
{
  static char* last_name = NULL;
  static unsigned int last_length = 0;
  static RDFResourceP resource = NULL;

  if (!last_name || strcmp(last_name, name)) {
    /* search for resource in list */
    resource = _ListSearchResource (*listP, name);
    if ((!resource || !resource->name) && create)
      {
	resource = TtaGetMemory (sizeof(RDFResource));

	resource->name = TtaStrdup (name);
	resource->statements = NULL;
	resource->class = NULL;
	List_add (listP, (void*) resource);
      }
    if (last_length < ustrlen(name))
      {
	if (last_name)
	  TtaFreeMemory (last_name);

	last_length = 2*ustrlen(name);
	last_name = TtaAllocString (last_length);
      }
    strcpy(last_name, name);
  }

  return resource;
}


/*------------------------------------------------------------
   ANNOT_FindRDFStatement
  ------------------------------------------------------------*/
#ifdef __STDC__
RDFStatementP ANNOT_FindRDFStatement( List* list, RDFPropertyP p )
#else /* __STDC__ */
RDFStatementP ANNOT_FindRDFStatement( listP, p )
     List** listP;
     RDFPredicateP p;
#endif /* __STDC__ */
{
  if (!p)
    return NULL;

  while (list)
    {
      RDFStatementP s = (RDFStatementP)list->object;
      if (s->predicate == p)
	return s;
      list = list->next;
    }

  return NULL;
}


/*------------------------------------------------------------
   ANNOT_GetLabel
  ------------------------------------------------------------*/
#ifdef __STDC__
char *ANNOT_GetLabel (List **listP, RDFResourceP r)
#else /*  __STDC__ */
char *ANNOT_GetLabel (listP, r)
     ListP **listP;
     RDFResourceP r;
#endif /*  __STDC__ */
{
  static RDFPropertyP labelP = NULL;
  RDFStatementP labelS = NULL;

  if (!labelP)
    labelP = ANNOT_FindRDFResource (listP, RDFS_LABEL, FALSE);

  if (!labelP)
    return NULL;

  while (!labelS)
    {
      labelS = ANNOT_FindRDFStatement (r->statements, labelP);

      if (labelS)
	{
	  /* RRS @@ assume object is a literal */
	  return labelS->object->name;
	}

      if (r->class)
	{
	  /* search superclass(es) for a label */
	  /* RRS @@ only does one superclass for now */
	  RDFStatementP s;

	  if (!subclassOfP)
	    subclassOfP = ANNOT_FindRDFResource (listP, RDFS_SUBCLASSOF, TRUE);

	  s = ANNOT_FindRDFStatement (r->statements, subclassOfP);

	  if (!s)
	    return NULL;	/* unknown parent class */

	  r = s->object;
	}
      else
	return NULL;
    }
  return NULL;
}



/*------------------------------------------------------------
   ANNOT_ReadSchema
  ------------------------------------------------------------
   Parses an RDF schema.
  
   Parameters:
     namespace_URI - the name of the Schema to parse
  ------------------------------------------------------------*/
#ifdef __STDC__
void ANNOT_ReadSchema (Document doc, char *namespace_URI)
#else /* __STDC__ */
void ANNOT_ReadSchema (doc, namespace_URI)
     Document doc;
     char *namespace_URI;
#endif /* __STDC__ */
{
  ReadCallbackContext *ctx;
  int res;

  /* make some space to store the remote file name */
  ctx = (ReadCallbackContext*)TtaGetMemory (sizeof(ReadCallbackContext));

  /* launch the request */
  res = GetObjectWWW (doc,
		      namespace_URI,
		      NULL,
		      ctx->filename,
		      /* AMAYA_ASYNC | AMAYA_NOCACHE | */
		      AMAYA_SYNC | AMAYA_FLUSH_REQUEST,
		      NULL,
		      NULL, 
		      (void *) ReadSchema_callback,
		      (void *) ctx,
		      NO,
		      TEXT("application/rdf"));

  if (res != HT_OK)
      TtaSetStatus (doc, 1, "Couldn't read schema", NULL); /* @@ */
  else
      TtaSetStatus (doc, 1, "Reading schema", NULL); /* @@ */
}
