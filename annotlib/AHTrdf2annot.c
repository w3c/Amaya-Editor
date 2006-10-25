/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 2000-2005
 *  Please first read the full copyright statement in file COPYRIGHT.
 * 
 */

/*
 * AHTrdf2annot.c : parses an annotation RDF Description and intializes
 *    the corresponding memory elements 
 *
 * NOTE: the code will use the libwww RDF parser by default; if
 *    RAPTOR_RDF_PARSER is defined the Raptor RDF parser:
 *
 *     http://www.redland.opensource.ac.uk/raptor/
 *
 *    will be used.
 *
 * Author: Arthur Barstow (W3C/MIT)
 *
 */

/* system includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* annotlib includes */
#include "annotlib.h"
#include "AHTrdf2annot_f.h"
#include "ANNOTschemas_f.h"
#include "ANNOTtools_f.h"

/* RDF parser */
#include "raptor.h"

/* Amaya includes */
#include "AHTURLTools_f.h"
#include "uconvert.h"

/********************** static variables ***********************/

/* 
 * Namespace and Property names used in an Annotation Description 
 */
#if 0
static const char * DC_CREATOR = "creator";
static const char * DC_DATE    = "date";
static const char * DC_TITLE   = "title";

static const char * ANNOT_ANNOTATES  = "annotates";
static const char * ANNOT_BODY       = "body";
static const char * ANNOT_CONTEXT    = "context";
static const char * ANNOT_CREATED    = "created";
static const char * ANNOT_ANNOTATION = "Annotation";

static const char * HTTP_BODY           = "Body";
static const char * HTTP_CONTENT_LENGTH = "ContentLength";
static const char * HTTP_CONTENT_TYPE   = "ContentType";

#ifdef ANNOT_ON_ANNOT
static const char * THREAD_REPLY   = "Reply";
static const char * THREAD_ROOT   = "root";
static const char * THREAD_INREPLYTO = "inReplyTo";
#endif /* ANNOT_ON_ANNOT */

static const char * RDFMS_TYPE = "type";
#endif

static char* find_last_annotURL = NULL;
static int find_last_length = 0;
static AnnotMeta *find_last_annot = NULL;

/********************** global variables ***********************/

List *annot_list;  /* a list of annotations */

typedef struct _ParseContext
{
  List **annot_list;
  List **rdf_model;
  char *base_uri;   /* base URI for anonymous RDF names */
} ParseContext, *ParseContextP;

#define AnnTriple const raptor_statement

/* ------------------------------------------------------------
   ParseIdFragment
   Extracts the id (or in this case the thotlib labels)
   from a URL and reoves this info from the URL.
   @@@ JK This function can be removed when we decide we don't
   need the Amaya label backward compatibilty
   ------------------------------------------------------------*/
static void ParseIdFragment (AnnotMeta *annot, char *buff)
{
  char *c, *d;
  
  c = strchr (buff, '#');
  if (c)
    {
      *c = EOS;
      c += 4;
      d = c;
      while (*d)
	{
	  switch (*d) 
	    {
	    case '|':
	      *d = ' ';
	      break;
	    case ')':
	      *d = EOS;
	      break;
	    }
	  d++;
	}
      sscanf (c, "%s %d %s %d", annot->labf, (&annot->c1),
	      annot->labl, &(annot->cl));
    }
}

/* ------------------------------------------------------------
   ParseXptrFragment
   Extracts the id (or in this case the thotlib labels)
   from a URL and reoves this info from the URL.
   ------------------------------------------------------------*/
static void ParseXptrFragment (AnnotMeta *annot, char *buff)
{
  char *c;
  
  c = strchr (buff, '#');
  if (c)
    {
      c++;
      annot->xptr = TtaStrdup (c);
    }
}

/* ------------------------------------------------------------
   contains

   Determine if a given string (input) contains the first substring 
   (s1) followed by the second substring (s2).
  
   Parameters:
     input - source string
     s1 - first substring 
     s2 - second substring
  
   Returns: 
     TRUE if input = s1+s2 (+ is string concatenation); FALSE otherwise
  
 ------------------------------------------------------------*/
static ThotBool contains(char *input, const char *s1, const char * s2)
{
  /* Input checks */
  if (input == NULL || s1 == NULL || s2 == NULL)
      return FALSE;
  if (strlen(s1) > strlen(input))
      return FALSE;
  if (strlen(input) == strlen(s1) && strlen(s2) >= 1)
      return FALSE;

  if (!strncmp(input, s1, strlen(s2)))
      /* Now check the last part */
      if (!strcmp(input + strlen(s1), s2))
          return TRUE;

  return FALSE;
}

/* ------------------------------------------------------------
   FillAnnotField

   internal routine to store a known field of an annotation structure.
   ------------------------------------------------------------ */
static ThotBool FillAnnotField( AnnotMeta* annot,
				List **rdf_model,
				char* predicate,
				char* object,
				AnnTriple *triple)
{
  ThotBool found = TRUE;

  if (contains(predicate, ANNOT_NS, ANNOT_ANNOTATES))
    {
      if (!IsW3Path ((char *) object))
	{
	  /* we normalize the URL to take into account
	     the early / and \ problem */
	  annot->source_url = FixFileURL ((char *) object);
	}
      else
	annot->source_url = TtaStrdup ((char *) object);
    }
  else if (contains (predicate, DC0_NS, DC_CREATOR) ||
	   contains (predicate, DC1_NS, DC_CREATOR))
    {

      if (triple && (triple->object_type == RAPTOR_IDENTIFIER_TYPE_RESOURCE 
	  || triple->object_type == RAPTOR_IDENTIFIER_TYPE_ANONYMOUS))
	annot->creator = ANNOT_FindRDFResource (rdf_model, object, TRUE);
      else
	{
	  if (annot->author)
	    TtaFreeMemory (annot->author);
	  annot->author = TtaStrdup ((char *) object);
	}
    }
  else if (contains (predicate, ANNOT_NS, ANNOT_CREATED))
    annot->cdate = TtaStrdup ((char *) object);
  else if (contains (predicate, DC0_NS, DC_DATE) ||
	   contains (predicate, DC1_NS, DC_DATE))
    annot->mdate = TtaStrdup ((char *) object);
  else if (contains (predicate, DC0_NS, DC_TITLE) ||
	   contains (predicate, DC1_NS, DC_TITLE))
    annot->title = TtaStrdup ((char *) object);
  else if (contains (predicate, RDFMS_NS, RDFMS_TYPE)) 
    {
      if (annot->type && (contains (object, ANNOT_NS, ANNOT_ANNOTATION) ||
			 (contains (object, THREAD_NS, THREAD_REPLY))))
	{
	  /* this is the default [annotation type], and
	     we already have a type, so we don't save it */
	} 
	else 
	  annot->type = ANNOT_FindRDFResource (&annot_schema_list,
					       object,
					       TRUE);
    }
  else if (contains (predicate, ANNOT_NS, ANNOT_CONTEXT))
    {
      /* @@@ JK This test can be removed when we decide we don't
	 need the amaya label backward compatibilty. Xptr should
	 be the only mechanism we use */
      if (!strncmp (object, "#id(", 4))
	ParseIdFragment (annot, object);
      else
	ParseXptrFragment (annot, object);
    }
  else if (contains (predicate, HTTP_NS, HTTP_CONTENT_TYPE))
      annot->content_type = TtaStrdup ((char *) object);
  else if (contains (predicate, HTTP_NS, HTTP_CONTENT_LENGTH))
      annot->content_length = TtaStrdup ((char *) object);
  else if (contains (predicate, ANNOT_NS, ANNOT_BODY))
    {
      if (!IsW3Path ((char *) object))
	{
	  /* we normalize the URL to take into account
	     the early / and \ problem */
	  annot->body_url = FixFileURL ((char *) object);
	}
      else
	annot->body_url = TtaStrdup ((char *) object);
    }
  else if (contains (predicate, HTTP_NS, HTTP_BODY))
      annot->body = TtaStrdup ((char *) object);
#ifdef ANNOT_ON_ANNOT
  else if (contains (predicate, THREAD_NS, THREAD_ROOT))
      annot->rootOfThread = TtaStrdup ((char *) object);
  else if (contains (predicate, THREAD_NS, THREAD_INREPLYTO))
    {
      annot->inReplyTo = TtaStrdup ((char *) object);
      /* if it's a local URL, we use the reply to property as
	 the source of the annotation (what was annotated) */
      if (annot->source_url)
	TtaFreeMemory (annot->source_url);
      annot->source_url = TtaStrdup ((char *) object);
    }
#endif /* ANNOT_ON_ANNOT */
  else
    found = FALSE;

  return found;
}

/* ------------------------------------------------------------
   FillInKnownProperties

   internal routine to extract properties from an RDF model and
   store into an annotation structure.
   ------------------------------------------------------------ */
static void FillInKnownProperties( AnnotMeta* annot,
				   List** rdf_model,
				   char* annot_url )
{
   RDFResourceP subjectP = ANNOT_FindRDFResource (rdf_model, annot_url, FALSE);
   if (subjectP)
     {
       List *list;
       for (list = subjectP->statements; list; list = list->next) {
	 RDFStatementP s = (RDFStatementP)list->object;

	 if (s->predicate->name && s->object->name)
	   (void)FillAnnotField (annot,
				 rdf_model,
				 s->predicate->name,
				 s->object->name,
				 NULL);
       }
     }
}


/* ------------------------------------------------------------
   FindAnnot

   internal routine to optimize searching for an annotation
   on the list of annotations.
 ------------------------------------------------------------ */
static AnnotMeta* FindAnnot( List** listP, char* annot_url, ThotBool create )
{
  AnnotMeta *annot = find_last_annot;
  int url_length;
  char *ptr_annot_url;

  if (!IsW3Path (annot_url))
    {
      ptr_annot_url = FixFileURL (annot_url);
      if (!ptr_annot_url)
	/* ignoring weird URLs @@ RRS: this was a temporary bug */
	return NULL;
    }
  else
    ptr_annot_url = annot_url;

  /*  uri = HTLocalToWWW (file_name, "file:"); */

  if (!find_last_annotURL || strcmp(find_last_annotURL, ptr_annot_url)) {
    /* search for annotation in list */
    annot = AnnotList_searchAnnot (*listP, ptr_annot_url, AM_ANNOT_URL);
    if (!annot)
      {
	if (create)
	  {
	    annot = AnnotMeta_new ();
	    annot->annot_url = TtaStrdup (ptr_annot_url);
	    List_add (listP, (void *) annot);
	  }
	else
	  {
	    if (ptr_annot_url != annot_url)
	      TtaFreeMemory (ptr_annot_url);
	    return NULL;		/* if lookup fails, allow create next time */
	  }
      }
    url_length = strlen(ptr_annot_url) + 1;
    if (find_last_length < url_length)
      {
	if (find_last_annotURL)
	  TtaFreeMemory (find_last_annotURL);

	find_last_length = 2*url_length;
	find_last_annotURL = (char *)TtaGetMemory (find_last_length);
      }
    strcpy(find_last_annotURL, ptr_annot_url);
    find_last_annot = annot;
  }

  if (ptr_annot_url != annot_url)
    TtaFreeMemory (ptr_annot_url);

  return annot;
}

/* ------------------------------------------------------------
   Finish_FindAnnot

   Returns dynamic resources allocated in FindAnnot
 ------------------------------------------------------------ */

static void Finish_FindAnnot(void)
{
  if (find_last_annotURL)
    {
      TtaFreeMemory (find_last_annotURL);
      find_last_annotURL = NULL;
      find_last_length = 0;
    }
}

/* ------------------------------------------------------------
   triple_handler

   This callback is invoked when a new RDF triple has been parsed.
  
   As the triples arrive, their predicate is checked and its
   object is cached if it is a predicate that we care about.

   If we don't recognize the predicate then we store the statement
   in a generic "RDF model".  This model is expected to be specific
   to a document and should be freed [only] when all of the annotation
   data for that document are freed.
  
   Parameters:
     rdfp - the RDF parser
     triple - an RDF triple
     context - pointer to a ParseContext structure
 ------------------------------------------------------------*/
static void triple_handler (void * context, const raptor_statement *triple)
{
  List **listP = ((ParseContextP) context)->annot_list;
  List **rdf_model = ((ParseContextP) context)->rdf_model;

  if (triple) 
    {
      char * predicate = (char *) AM_RAPTOR_URI_AS_STRING(triple->predicate);
      char * subject = NULL;
      char * object =  NULL;

      AnnotMeta *annot;

      /* if it's an anoynmous subject, add the base_uri */
      if (triple->subject_type == RAPTOR_IDENTIFIER_TYPE_ANONYMOUS)
	{
	  ParseContext *parseCtx = (ParseContext *) context;
	  char *base_uri = parseCtx->base_uri;
	  char *ptr =  (char *) triple->subject;
	  subject = (char *)TtaGetMemory (strlen (base_uri) + strlen (ptr) + 2);
	  sprintf (subject, "%s#%s", base_uri, ptr);
	}
      else
	subject = (char *) AM_RAPTOR_URI_AS_STRING(triple->subject);

      if (triple->object_type ==  RAPTOR_IDENTIFIER_TYPE_LITERAL)
	object = (char *) triple->object;
      else if (triple->object_type ==  RAPTOR_IDENTIFIER_TYPE_ANONYMOUS)
	{
	  ParseContext *parseCtx = (ParseContext *) context;
	  char *base_uri = parseCtx->base_uri;
	  char *ptr =  (char *) triple->object;
	  object = (char *) TtaGetMemory (strlen (base_uri) + strlen (ptr) + 2);
	  sprintf (object, "%s#%s", base_uri, ptr);
	}
      else
	object = (char *) AM_RAPTOR_URI_AS_STRING(triple->object);

#ifdef _RDFDEBUG
      fprintf (stdout, "PRD = %s\n", predicate);
      fprintf (stdout, "SUB = %s\n", subject);
      fprintf (stdout, "OBJ = %s\n", object);
#endif

      /* Optimize for the case that most Subjects are Annotations */
      annot = FindAnnot (listP, subject, FALSE);

      if (!annot)
	{
	  /* We haven't seen this Annotation yet, or the object
	     might not be an Annotation */
	  
	  if ((contains (predicate, RDFMS_NS, RDFMS_TYPE) &&
	       contains (object, ANNOT_NS, ANNOT_ANNOTATION))
#ifdef ANNOT_ON_ANNOT	      
	      || (contains (predicate, RDFMS_NS, RDFMS_TYPE) &&
		  contains (object, THREAD_NS, THREAD_REPLY))
#endif /* ANNOT_ON_ANNOT */
	      )
	    {
	      /* OK, we're really describing an Annotation now */
	      annot = FindAnnot (listP, subject, TRUE);
	      if (annot)	/* @@ should never be null */
		FillInKnownProperties (annot, rdf_model, subject);
	    }
	}
      if (!annot || !FillAnnotField (annot, rdf_model, predicate, object, triple))
	/* it's some other RDF statement; store it in the
	   document-specific model.  Note that subjects and
	   objects are stored in the document-specific model
	   whereas predicates are stored in the persistent
	   schema model. @@ RRS: this doesn't permit assertions
	   about the predicates to be passed in the annot index. */
	{
	  RDFResourceP subjectP;
	  RDFPropertyP predicateP;
	  RDFResourceP objectP;

	  subjectP = ANNOT_FindRDFResource (rdf_model, subject, TRUE);
	  predicateP = ANNOT_FindRDFResource (&annot_schema_list,
					      predicate,
					      TRUE);
	  objectP = ANNOT_FindRDFResource (rdf_model, object, TRUE);

	  SCHEMA_AddStatement (subjectP, predicateP, objectP);
	}
      if (triple->subject_type == RAPTOR_IDENTIFIER_TYPE_ANONYMOUS)
	TtaFreeMemory (subject);
      if (triple->object_type ==  RAPTOR_IDENTIFIER_TYPE_ANONYMOUS)
	TtaFreeMemory (object);
    }
}

/********************** Public API entry point ********************/

/* ------------------------------------------------------------
   RDF_parseFile

   Parses a file of RDF.
  
   Parameters:
     file_name - the name of the file to parse
     rdf_model - a list of RDF resources to update

   Returs: a pointer to an annotation's list or NULL if an error
     occurs during the parsing.
 ------------------------------------------------------------*/
List *RDF_parseFile (char *file_name, List **rdf_model)
{
  ParseContext   ctx;
  raptor_parser *rdfxml_parser=NULL;
  raptor_uri    *uri = NULL;
  char          *full_file_name;
  char          *tmp, *path;

  ctx.annot_list = &annot_list;
  ctx.rdf_model = rdf_model;

  annot_list = NULL;

  rdfxml_parser = raptor_new_parser ("rdfxml");

  if (!rdfxml_parser) {
     AnnotList_free (annot_list);
     /* do not free rdf_model here; it may not have been empty to start */
     annot_list = NULL;
     return NULL;
  }  

   /* @@ this is what we should do eventually */
 /* if (!IsW3Path (file_name))
     full_file_name = LocalToWWW (file_name);
  else
  */
  /* raptor doesn't grok file URIs under windows. The following is a patch so
   that we can use it */
  full_file_name = (char *)TtaGetMemory (strlen (file_name) + sizeof ("file:"));
  sprintf (full_file_name, "file:%s", file_name);
#ifdef _WX
  path = (char *)TtaConvertMbsToByte ((unsigned char *)file_name,
	                                 TtaGetLocaleCharset ());
   /* remember the base name for anoynmous subjects */
  tmp = (char *)TtaConvertMbsToByte ((unsigned char *)full_file_name,
	                                 TtaGetLocaleCharset ());
   /* remember the base name for anoynmous subjects */
  TtaFreeMemory (full_file_name);
  full_file_name = tmp;
#else /* _WX */
  path = file_name;
#endif /* _WX */
   /* remember the base name for anoynmous subjects */
  ctx.base_uri = full_file_name;

  raptor_set_statement_handler(rdfxml_parser, (void *) &ctx, triple_handler); 
  tmp = (char *) raptor_uri_filename_to_uri_string ((const char *) path);
  uri = raptor_new_uri ((const unsigned char *) tmp);
#ifdef _WX
  TtaFreeMemory (path);
#endif /* _WX */
  TtaFreeMemory (tmp);

  if (raptor_parse_file (rdfxml_parser, uri, NULL))
    {
      AnnotList_free (annot_list);
      /* do not free rdf_model here; it may not have been empty to start */
      annot_list = NULL;
      TtaFreeMemory (full_file_name);
      raptor_free_parser (rdfxml_parser);
      raptor_free_uri (uri);
      return NULL;
    }

  TtaFreeMemory (full_file_name);
  raptor_free_parser (rdfxml_parser);
  raptor_free_uri (uri);

  Finish_FindAnnot();

  /* output whatever we parsed */
#ifdef _RDFDEBUG
  AnnotList_print (annot_list);
#endif /* _RDFDEBUG */

  return (annot_list);
}
