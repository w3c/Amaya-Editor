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
static const char * DC_CREATOR = "creator";
static const char * DC_DATE    = "date";

static const char * ANNOT_NS = "http://www.w3.org/1999/xx/annotation-ns#";
static const char * ANNOT_ANNOTATES = "annotates";
static const char * ANNOT_BODY       = "body";
static const char * ANNOT_CONTEXT    = "context";
static const char * ANNOT_CREATED    = "created";
static const char * ANNOT_ANNOTATION = "Annotation";

static const char * HTTP_NS  = "http://www.w3.org/1999/xx/http#";
static const char * HTTP_BODY           = "Body";
static const char * HTTP_CONTENT_LENGTH = "ContentLength";
static const char * HTTP_CONTENT_TYPE   = "ContentType";

static const char * RDFMS_NS = "http://www.w3.org/1999/02/22-rdf-syntax-ns#";
static const char * RDFMS_TYPE = "type";

/********************** global variables ***********************/

List *annot_list;  /* a list of annotations */

/* ------------------------------------------------------------
   ParseIdFragment
   Extracts the id (or in this case the thotlib labels)
   from a URL and reoves this info from the URL.
   @@@ JK This function can be removed when we decide we don't
   need the Amaya label backward compatibilty
   ------------------------------------------------------------*/
static void ParseIdFragment (AnnotMeta *annot, char *buff)
{
  CHAR_T *c, *d;
  
  c = strchr (buff, TEXT('#'));
  if (c)
    {
      *c = WC_EOS;
      c += 4;
      d = c;
      while (*d)
	{
	  switch (*d) 
	    {
	    case TEXT('|'):
	      *d = ' ';
	      break;
	    case TEXT(')'):
	      *d = WC_EOS;
	      break;
	    }
	  d++;
	}
      usscanf (c, TEXT("%s %d %s %d"), annot->labf, (&annot->c1),
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
  CHAR_T *c;
  
  c = strchr (buff, TEXT('#'));
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

static AnnotMeta* FindAnnot( List** listP, char* annot_url, ThotBool create )
{
  static char* last_annotURL = NULL;
  static int last_length = 0;
  static AnnotMeta *annot = NULL;

  /*  uri = HTLocalToWWW (file_name, "file:"); */

  if (!last_annotURL || strcmp(last_annotURL, annot_url)) {
    /* search for annotation in list */
    annot = AnnotList_searchAnnot (*listP, annot_url, AM_ANNOT_URL);
    if (!annot && create)
      {
	annot = AnnotMeta_new ();
	annot->annot_url = TtaStrdup (annot_url);
	List_add (listP, (void*) annot);
      }
    if (last_length < ustrlen(annot_url))
      {
	if (last_annotURL)
	  TtaFreeMemory (last_annotURL);

	last_length = 2*ustrlen(annot_url);
	last_annotURL = TtaAllocString (last_length);
      }
    strcpy(last_annotURL, annot_url);
  }

  return annot;
}

/* ------------------------------------------------------------
   triple_handler

   This callback is invoked when a new RDF triple has been parsed.
  
   As the triples arrive, their predicate is checked and its
   object is cached if it is a predicate that we care about.
  
   Parameters:
     rdfp - the RDF parser
     triple - an RDF triple
     context - pointer to a AnnotMeta structure
 ------------------------------------------------------------*/
static void triple_handler (HTRDF * rdfp, HTTriple * triple, void * context)
{
  List **listP = (List**)context;

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
          annot->source_url = TtaStrdup ((char *) object);
      else if (contains (predicate, DC_NS, DC_CREATOR))
          annot->author = TtaStrdup ((char *) object);
      else if (contains (predicate, ANNOT_NS, ANNOT_CREATED))
          annot->cdate = TtaStrdup ((char *) object);
      else if (contains (predicate, DC_NS, DC_DATE))
          annot->mdate = TtaStrdup ((char *) object);
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
          annot->body_url = TtaStrdup ((char *) object);
      else if (contains (predicate, HTTP_NS, HTTP_BODY))
          annot->body = TtaStrdup ((char *) object);
    }
}

/********************** Public API entry point ********************/

/* ------------------------------------------------------------
   RDF_parseFile

   Parses a file of RDF.
  
   Parameters:
     file_name - the name of the file to parse
     type - the annotation type (see AnnotFileType)

   Returs: a pointer to an annotation's list or NULL if an error
     occurs during the parsing.
 ------------------------------------------------------------*/
List *RDF_parseFile (char *file_name, AnnotFileType type)
{
  annot_list = NULL;

  if (type != ANNOT_LIST)
      return NULL;

  if (HTRDF_parseFile(file_name, triple_handler, &annot_list) != YES)
    {
      AnnotList_free (annot_list);
      annot_list = NULL;
      return NULL;
    }

  /* output whatever we parsed */
  AnnotList_print (annot_list);

  return (annot_list);
}

#ifdef _RDFDEBUG
/* ------------------------------------------------------------
   main 

   function for unit testing 
 ------------------------------------------------------------*/
int main (int argc, char *argv[])
{
  List *l;
  int i;

  if (argc < 2) 
    {
      fprintf(stderr, "Usage: %s file[s]\n", argv[0]); 
      exit(1);
    }

  for (i=1; i < argc; i++) 
    {
      l = RDF_parseFile (argv[i], ANNOT_SINGLE);

      if (!l)
        fprintf(stderr, "ERROR: Parse Failure of %s\n", argv[i]);
    }
}
#endif /* _RDFDEBUG */

