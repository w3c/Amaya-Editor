/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 2000.
 *  Please first read the full copyright statement in file COPYRIGHT.
 * 
 */

/*
 * rdf2annot.c : parses an annotation RDF Description and intializes
 *              the corresponding memory elements 
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
 * Namespace and Predicate names used in an Annotation Description 
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

static const char * FILE_SCHEME = "file://";

/********************** global variables ***********************/

AnnotMeta *annot;  /* the current annotation */

List *annot_list;  /* a list of annotations */

/* ------------------------------------------------------------
   ParseIdFragment
   Extracts the id (or in this case the thotlib labels)
   from a URL and reoves this info from the URL.
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

/*
 * Determine if a given string (input) contains the first substring 
 * (s1) followed by the second substring (s2).
 *
 * RETURN: TRUE if input = s1+s2 (+ is string concatenation); FALSE otherwise
 *
 *@param input source string
 *@param s1 first substring 
 *@param s2 second substring
 */
static ThotBool contains(char *input, const char *s1, const char * s2)
{
  if (!strncmp(input, s1, strlen(s2)))
      /* Now check the last part */
      if (!strcmp(input + strlen(s1), s2))
          return TRUE;

  return FALSE;
}

/*
 * This callback is invoked when a new RDF triple has been parsed.
 *
 * As the triples arrive, their predicate is checked and its
 * object is cached if it is a predicate that we care about.
 *@param rdfp the RDF parser
 *@param t an RDF triple
 *@param context user data - NOT used
 */
static void triple_handler (HTRDF * rdfp, HTTriple * t, void * context)
{
  if (rdfp && t) 
    {
      char * predicate = HTTriple_predicate(t);
#ifdef _DEBUG
      char * subject = HTTriple_subject(t);
#endif
      char * object = HTTriple_object(t);

#ifdef _DEBUG
      fprintf (stdout, "PRD = %s\n", predicate);
      fprintf (stdout, "SUB = %s\n", subject);
      fprintf (stdout, "OBJ = %s\n", object);
#endif

      if (contains(predicate, ANNOT_NS, ANNOT_ANNOTATES))
          annot->source_url = strdup ((char *) object);
      else if (contains (predicate, DC_NS, DC_CREATOR))
          annot->author = strdup ((char *) object);
      else if (contains (predicate, ANNOT_NS, ANNOT_CREATED))
          annot->cdate = strdup ((char *) object);
      else if (contains (predicate, DC_NS, DC_DATE))
          annot->mdate = strdup ((char *) object);
      else if (contains (predicate, RDFMS_NS, RDFMS_TYPE)) 
        {
          if (contains (object, ANNOT_NS, ANNOT_ANNOTATION)) 
            {
              /* this is the default [annotation type], so we don't save it */
            } 
            else 
              {
                int len = sizeof (ANNOT_NS) - 1;
                if (!strncmp (object, ANNOT_NS, len)) 
                  {
                    if (*object + len)
                        annot->type = strdup (object + len);
                    else
                        annot->type = strdup (object);
                  }
	        else 
	          annot->type = strdup (object);
              }
        }
      else if (contains (predicate, ANNOT_NS, ANNOT_CONTEXT))
          ParseIdFragment (annot, object);
      else if (contains (predicate, HTTP_NS, HTTP_CONTENT_TYPE))
          annot->content_type = strdup ((char *) object);
      else if (contains (predicate, HTTP_NS, HTTP_CONTENT_LENGTH))
          annot->content_length = strdup ((char *) object);
      else if (contains (predicate, ANNOT_NS, ANNOT_BODY))
          annot->body_url = strdup ((char *) object);
      else if (contains (predicate, HTTP_NS, HTTP_BODY))
          annot->body = strdup ((char *) object);
    }
}

/********************** Public API entry point ********************/

/* 
 * Parses a file of RDF.
 *
 * RETURN: a pointer to an annotation's list or NULL if an error
 * occurs during the parsing.
 */
List *RDF_parseFile (char *file_name, AnnotFileType type)
{
  char *s;
  char *uri = file_name;

  annot_list = NULL;
  annot = NULL;

  /* Only support a file with a single annotation */
  if (type != ANNOT_LIST)
      return NULL;

  annot =  AnnotMeta_new ();
  if (!annot) return NULL;

  if (strncmp (file_name, FILE_SCHEME, 7)) 
    {
      uri = malloc (strlen(FILE_SCHEME) + strlen(file_name) + 1);
      (void) strcpy (uri, FILE_SCHEME);
      (void) strcat (uri, file_name);
    }

  annot->annot_url = strdup(uri);

  if ((s = HTRDFParseFile(file_name, triple_handler) != NULL)) 
    {
      fprintf(stderr, "%s\n", s);
      AnnotList_free (annot_list);
      annot_list = NULL;
      return NULL;
    }

  List_add (&annot_list, (void *) annot);

  /* output whatever we parsed */
  AnnotList_print (annot_list);

  return (annot_list);
}

#ifdef _DEBUG
/* main function for unit testing */
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
#endif /* _DEBUG */

