/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 2000.
 *  Please first read the full copyright statement in file COPYRIGHT.
 * 
 */

/*
 * AHTrdf2annot.c : parses an annotation RDF Description and intializes
 *    the corresponding memory elements 
 *
 * NOTE: the code assumes libwww's RDF parser.
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

/* libwww  includes */
#include "xmlparse.h"
#include "HTRDF.h"

/* Amaya includes */
#include "AHTURLTools_f.h"

/********************** static variables ***********************/

/* 
 * Namespace and Property names used in an Annotation Description 
 */
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
static const char * THREAD_ROOT   = "root";
static const char * THREAD_INREPLYTO = "inReplyTo";
#endif /* ANNOT_ON_ANNOT */

static const char * RDFMS_TYPE = "type";

static char* find_last_annotURL = NULL;
static int find_last_length = 0;
static AnnotMeta *find_last_annot = NULL;

/********************** global variables ***********************/

List *annot_list;  /* a list of annotations */

typedef struct _ParseContext
{
  List **annot_list;
  List **rdf_model;
} ParseContext, *ParseContextP;

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
    ptr_annot_url = FixFileURL (annot_url);
  else
    ptr_annot_url = annot_url;

  /*  uri = HTLocalToWWW (file_name, "file:"); */

  if (!find_last_annotURL || strcmp(find_last_annotURL, ptr_annot_url)) {
    /* search for annotation in list */
    annot = AnnotList_searchAnnot (*listP, ptr_annot_url, AM_ANNOT_URL);
    if (!annot && create)
      {
	annot = AnnotMeta_new ();
	annot->annot_url = TtaStrdup (ptr_annot_url);
	List_add (listP, (void*) annot);
      }
    url_length = strlen(ptr_annot_url) + 1;
    if (find_last_length < url_length)
      {
	if (find_last_annotURL)
	  TtaFreeMemory (find_last_annotURL);

	find_last_length = 2*url_length;
	find_last_annotURL = TtaGetMemory (find_last_length);
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
static void triple_handler (HTRDF * rdfp, HTTriple * triple, void * context)
{
  List **listP = ((ParseContextP) context)->annot_list;
  List **rdf_model = ((ParseContextP) context)->rdf_model;

  if (rdfp && triple) 
    {
      char * predicate = HTTriple_predicate(triple);
      char * subject = HTTriple_subject(triple);
      char * object = HTTriple_object(triple);

      AnnotMeta *annot;

#ifdef _RDFDEBUG
      fprintf (stdout, "PRD = %s\n", predicate);
      fprintf (stdout, "SUB = %s\n", subject);
      fprintf (stdout, "OBJ = %s\n", object);
#endif

      annot = FindAnnot (listP, subject, TRUE);

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
      else if (contains (predicate, DC_NS, DC_CREATOR))
	{
	  /* @@ RRS: hack, hack; we should _not_ be inferring URIs.
	     Libwww should distinguish between literal and resource. */
	  if (IsW3Path (object) || IsFilePath (object))
	      annot->creator = ANNOT_FindRDFResource (rdf_model, object, TRUE);
	  else
	    annot->author = TtaStrdup ((char *) object);
	}
      else if (contains (predicate, ANNOT_NS, ANNOT_CREATED))
          annot->cdate = TtaStrdup ((char *) object);
      else if (contains (predicate, DC_NS, DC_DATE))
          annot->mdate = TtaStrdup ((char *) object);
      else if (contains (predicate, DC_NS, DC_TITLE))
          annot->title = TtaStrdup ((char *) object);
      else if (contains (predicate, RDFMS_NS, RDFMS_TYPE)) 
        {
          if (contains (object, ANNOT_NS, ANNOT_ANNOTATION) && annot->type)
	    {
	      /* this is the default [annotation type], and
		 we already have a type, so we don't save it */
            } 
            else 
              {
		annot->type = ANNOT_FindRDFResource (&annot_schema_list,
						     object,
						     TRUE);
              }
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
	{
          annot->rootOfThread = TtaStrdup ((char *) object);
	  annot->isReplyTo = TRUE;
	}
      else if (contains (predicate, THREAD_NS, THREAD_INREPLYTO))
	{
          annot->inReplyTo = TtaStrdup ((char *) object);
	  annot->isReplyTo = TRUE;
	}
#endif /* ANNOT_ON_ANNOT */
      else
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
	  /* ugly, ugly; libwww discards info -- is the object a Literal? */
	  objectP = ANNOT_FindRDFResource (rdf_model, object, TRUE);

	  SCHEMA_AddStatement (subjectP, predicateP, objectP);
	}
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
  ParseContext ctx;

  ctx.annot_list = &annot_list;
  ctx.rdf_model = rdf_model;

  annot_list = NULL;

  if (HTRDF_parseFile(file_name, triple_handler, &ctx) != YES)
    {
      AnnotList_free (annot_list);
      /* do not free rdf_model here; it may not have been empty to start */
      annot_list = NULL;
      return NULL;
    }

  Finish_FindAnnot();

  /* output whatever we parsed */
#ifdef _RDFDEBUG
  AnnotList_print (annot_list);
#endif /* _RDFDEBUG */

  return (annot_list);
}




