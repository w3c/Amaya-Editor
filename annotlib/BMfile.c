/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 2003-2005.
 *  Please first read the full copyright statement in file COPYRIGHT.
 * 
 */

/*
 * BMfile.c : creates RDF bookmarks, outputs them, parses them ... ?
 *
 * Author: J. Kahan (W3C/ERCIM)
 *
 */


/* general includes */
#include "annotlib.h"
#include "redland.h"
#include "rdf_heuristics.h"

/* bookmarks includes */
#include "bookmarks.h"
#include "f/BMevent_f.h"
#include "f/BMtools_f.h"

/* annotlib includes */
#include "f/ANNOTtools_f.h"

/* amaya includes */
#include "AHTURLTools_f.h"
#include "init_f.h"
#include "f/BMfile_f.h"

/* global variables */

/* A structure to store the redland related info for bookmarks */
typedef struct _BMContext {
  /* the redland parameters */
  librdf_world *world;
  librdf_storage *storage;
  librdf_model *model;
  /* the url where this bookmark is stored */
  char *url;
  /* the tmpfile where the rdf is stored */
  char *tmpfile;
  /* a quick pointer for searching this file */
  Document ref;
  /* stuff I don't know yet if we should use */
  char *is_default_file;
  /* the doc number if this file is being viewed, 0 otherwise */
  Document doc;

  /* pointer to the next bookmark file context */
  struct _BMContext *next;
} BMContext;

static BMContext *BookmarkFiles;
/* a counter used to quickly refer to a bookmark file */
static Document BookmarkRef;

/* counter for the generated IDs of new statements that we will
   add to the model */
static int genid_counter = 1;


/* DEFINE for debugging the parser */
/*
#define BMFILE_DEBUG 1
*/


/*----------------------------------------------------------------------
  BM_Context_url
  Returns the URL associated to a given bookmark file
  ----------------------------------------------------------------------*/
char *BMContext_url (int ref)
{
  BMContext *me;
  char *ptr;

  me = BookmarkFiles;
  while (me && me->ref != ref)
      me = me->next;

  if (me) 
    ptr = me->url;
  else 
    ptr = NULL;

  return ptr;
}

/*----------------------------------------------------------------------
  BM_Context_tmpfile
  Returns the local file we're using to parse a given bookmark.
  ----------------------------------------------------------------------*/
char *BMContext_tmpfile (int ref)
{
  BMContext *me;
  char *ptr;

  me = BookmarkFiles;
  while (me && me->ref != ref)
    me = me->next;

  if (me) 
    ptr = me->tmpfile;
  else 
    ptr = NULL;

  return ptr;
}

/*----------------------------------------------------------------------
  BM_Context_tmpfileSet
  Changes the temporary file name for saving a bookmark
  ----------------------------------------------------------------------*/
ThotBool BMContext_tmpfileSet (int ref, char *tmpfile)
{
  BMContext *me;
  ThotBool res;

  me = BookmarkFiles;
  while (me && me->ref != ref)
    me = me->next;

  if (me) 
    {
      if (me->tmpfile)
	TtaFreeMemory (me->tmpfile);
      if (tmpfile)
	me->tmpfile = TtaStrdup (tmpfile);
      else
	me->tmpfile = NULL;
      res = TRUE;
    }
  else 
    res = FALSE;

  return res;
}
  
/*----------------------------------------------------------------------
  BM_Context_reference
  Returns the internal reference for a given bookmark URL
  ----------------------------------------------------------------------*/
ThotBool BM_Context_reference (char *url, int *ref)
{
  BMContext *me;
  ThotBool result;
  char *normalized_url;

  if (!url || !url[0])
    return FALSE;

  /* check to see if this is an amaya-like file URL and fix it */
  if (!IsHTTPPath (url) && !IsFilePath (url))
    normalized_url = FixFileURL (url);
  else
    normalized_url = url;

  if (!normalized_url)
    return FALSE;

  me = BookmarkFiles;
  while (me && strcmp (normalized_url, me->url))
    me = me->next;

  if (me) 
    {
      *ref = me->ref;
      result = TRUE;
    }
  else
      result = FALSE;

  if (normalized_url != url)
    TtaFreeMemory (normalized_url);

  return (result);
}

/*----------------------------------------------------------------------
  BM_Context_dumpasCharList
  Returns a list with a copy of the URLs.
  Caller must free the returned list (but not its contents)
  ----------------------------------------------------------------------*/
int BM_Context_dumpAsCharList (char **bm_urls[])
{
  char **dump = NULL;
  int i, count = 0;
  BMContext *me;

  me = BookmarkFiles;

  while (me)
    {
      count++;
      me = me->next;
    }
  
  if (count)
    {
      dump = (char **) TtaGetMemory  (sizeof (char *) * count);
      dump[0] = "Default bookmark file";
      me = BookmarkFiles->next;
      for (i = 1; i < count; i++)
	{
	  dump[i] = me->url;
	  me = me->next;
	}
    }
  *bm_urls = dump;
  return (count);
}

/*----------------------------------------------------------------------
  BM_Context_free
  ----------------------------------------------------------------------*/
static ThotBool BM_Context_free (Document ref)
{
  BMContext *me, *prev;

  prev = NULL;
  me = BookmarkFiles;
  while (me && me->ref != ref)
    {
      prev = me;
      me = me->next;
    }
  if (me) 
    {
      if (!prev)
	BookmarkFiles = me->next;
      else
	prev->next = me->next;
      TtaFreeMemory (me->url);
      TtaFreeMemory (me->tmpfile);
      TtaFreeMemory (me);
      return TRUE;
    }
  return FALSE;
}

/*----------------------------------------------------------------------
  BM_Context_get
  ----------------------------------------------------------------------*/
static ThotBool BM_Context_get (int ref, librdf_world **world, 
				librdf_model **model, 
				librdf_storage **storage)
{
  BMContext *me;

  me = BookmarkFiles;

  while (me)
    {
      if (ref == me->ref)
	break;
      else
	me = me->next;
    }

  if (!me)
    return FALSE;

  *world = me->world;
  *model = me->model;
  *storage = me->storage;

  if (!*world || !*model || !*storage)
    return FALSE;
  else
    return TRUE;
}

/*----------------------------------------------------------------------
  BM_Context_set
  ----------------------------------------------------------------------*/
static ThotBool BM_Context_set (int ref, char *url,
				char *tmpfile,
				librdf_world *world, librdf_model *model, 
				librdf_storage *storage)
{
   BMContext *me, *tmp;

  if (!world  || !model || !storage)
    return FALSE;

  me = (BMContext *) TtaGetMemory (sizeof (BMContext));
  memset (me, 0, sizeof (BMContext));

  /* @@ JK considerations for the first case */

  if (!BookmarkFiles)
    {
      BookmarkFiles = me;
    }
  else
    {
      /* insert it at the end */
      tmp = BookmarkFiles;
      while (tmp->next)
	tmp = tmp->next;
      tmp->next = me;
    }
  me->world = world;
  me->model = model;
  me->storage = storage;
  me->ref = ref;

  /* make sure that all the URLs are normalized to simplify things */
  if (!IsHTTPPath (url) && !IsFilePath (url))
    me->url = FixFileURL (url);
  else
    me->url = TtaStrdup (url);

  if (tmpfile)
    me->tmpfile = TtaStrdup (tmpfile);

  return TRUE;
}

/* two global variables to communicate data to error handler */
static ThotBool error;

/*----------------------------------------------------------------------
  error handler
  Called whenever there's an RDF parsing error.
  Sets an error flag, a la errorno. The application must reset it.
  ---------------------------------------------------------------------*/
static void error_handler (void *user_data, const char *message, va_list arguments)
{
  char msg[256];

  /* set the error flag */
  error = TRUE;
  
  /* print the error message that redland gave us */
  fprintf (stderr, "Bookmark parsing error:\n");
  vfprintf (stderr, message, arguments);
  fputc('\n', stderr);

  /* do the same thing, but for putting a popup message */
  vsnprintf (msg, sizeof (msg), message, arguments);
  InitConfirm3L (0, 0, "Bookmark parsing error", msg, NULL, FALSE);
}

/*----------------------------------------------------------------------
  redland_init
  Initializes the redland environment.
  Returns a reference number for this bookmark
  ----------------------------------------------------------------------*/
Document redland_init (char *url, char *tmpfile, 
		       ThotBool init_world)
{
  int ref;
  static librdf_world *world;
  librdf_model *model;
  librdf_storage *storage;

  if (init_world)
    {
      /* Creates a new Redland execution environment */
      world = librdf_new_world ();

      /* Open an environment */
      librdf_world_open (world);
    }

  /* create a new librdf_storage object, the place where statements will be stored? */
  storage = librdf_new_storage (world, 
				/* the storage factory name */
				"hashes", 
				/* the storage factory name (our personal ref. to it) */
				"test",
				/* options to initialise storage */
				"hash-type='memory'"
				);

  /* create a new librdf_storage object. N.B. in 0.9.12 
     the model storage relationship is 1:1 */
  model = librdf_new_model (world, 
			    storage, /* storage to use */
			    NULL);   /* options to intialize model */

  /* set the error handler and reset the error flag */
  librdf_world_set_error (world, NULL, 
			  error_handler);
  error = FALSE;

  ref = BookmarkRef++;
  BM_Context_set (ref, url, tmpfile, world, model, storage);
  return (ref);
}

/*----------------------------------------------------------------------
  redland_free
  Frees the redland environment.
  ----------------------------------------------------------------------*/
void redland_free (int ref, ThotBool free_world)
{
  librdf_world *world;
  librdf_model *model; 
  librdf_storage *storage;

  if (!BM_Context_get (ref, &world, &model, &storage))
    return;

  /*
  ** free the model 
  */

  if (model)
    librdf_free_model(model);
  if (storage)
    librdf_free_storage(storage);
  if (free_world && world)
    librdf_free_world(world);
  
  /* free the context */
  BM_Context_free (ref);
}

/*----------------------------------------------------------------------
  add_statement
  Add a statement to a storage model and then frees the temporary
  statement.
  ----------------------------------------------------------------------*/
static void add_statement (librdf_world *world, librdf_model *model,
			   librdf_node* subject, librdf_node* predicate, 
			   librdf_node* object)
{
  librdf_statement* statement;

  statement = librdf_new_statement_from_nodes (world, subject, predicate, object);
  librdf_model_add_statement(model, statement);
  librdf_free_statement (statement);
}

/*----------------------------------------------------------------------
  parse_file
  Parsers a file into the model.
  ----------------------------------------------------------------------*/
static ThotBool parse_file (librdf_world *world, librdf_model *model,
			char *filename, char *base, int ntriples)
{
  librdf_parser* parser;
  librdf_stream* stream;
  librdf_uri *uri;
  librdf_uri *base_uri;
  librdf_node *node_p;
  int count = 0 ;
  ThotBool res;

  /* redland expects a specific coded file URI */

  if (!IsHTTPPath (filename) && !IsFilePath (filename))
    uri = librdf_new_uri_from_filename (world, (const char *) filename);
  else
    uri = librdf_new_uri (world, (unsigned char *) filename);

  if (!IsHTTPPath (base) && !IsFilePath (base))
    base_uri = librdf_new_uri_from_filename (world, (const char *) base);
  else
    base_uri = librdf_new_uri (world, (unsigned char *) base);

  if (ntriples)
    /* parse the file as triplets */
    parser = librdf_new_parser (world, "ntriples", NULL, NULL);
  else
    /* parse the file as rdf */
    parser = librdf_new_parser (world, "raptor", NULL, NULL);
  
  /* @@ JK trying something to understand how this works */
  node_p = librdf_new_node_from_literal (world, (unsigned char *) "yes", NULL, 0);
  librdf_parser_set_feature(parser, LIBRDF_MS_aboutEach_URI, node_p);
  librdf_parser_set_feature(parser, LIBRDF_MS_aboutEachPrefix_URI, node_p);
  librdf_free_node (node_p);

  error = FALSE;
  stream = librdf_parser_parse_as_stream (parser, uri, base_uri);

  if (!error && stream)
    {
      res = TRUE;

      while(!librdf_stream_end (stream)) {
	librdf_statement *statement=librdf_stream_get_object (stream);
	if(!statement) {
	  fprintf(stderr, "librdf_stream_next returned NULL\n");
	  break;
	}
	
	if (librdf_model_add_statement (model, statement)) {
	  fprintf(stderr, "librdf_model_add_statement returned error\n");
	  break;
	}
	
	/* cannot free them anymore */
	/* librdf_free_statement (statement); */
	librdf_stream_next(stream); /* advance the stream */
	count++;
      }
    }
  else
    res = FALSE;

  if (stream)
    librdf_free_stream (stream);
  else
  librdf_free_parser (parser);
  librdf_free_uri (uri);
  librdf_free_uri (base_uri); 

  if (error)
    {
      error = FALSE;
      fprintf(stderr, "Failed to parse RDF as stream\n");
      /* JK error and we should return 0 and disable this bookmark file, 
	 rather than save it */
    }
  
#ifdef BMFILE_DEBUG
  fprintf(stderr, "Added %d statements\n", count);
#endif /* BMFILE_DEBUG */

  return res;
}

/*----------------------------------------------------------------------
  serialize
  Serializes an RDF model into either triples or rdf/xml.
  Returns TRUE if success, FALSE otherwise.
  ----------------------------------------------------------------------*/
static ThotBool serialize (librdf_world *world, librdf_model *model, 
		       char *name, char *filename, char *mime_type)
{
  ThotBool result = TRUE;

  /* create a new librdf_serializer object */
  librdf_serializer* serializer;
  serializer = librdf_new_serializer (world, 
				      /* the serializer factory name */
				      name,
				      /* the MIME type of the syntax (NULL if not used) */
				      mime_type,
				      /* URI of syntax (NULL if not used) */
				      NULL);
  if (!serializer) 
    {
      fprintf(stderr, "Failed to create new serializer\n");
      result = FALSE;
    }
  else
    {
      FILE *test;

      test = TtaWriteOpen (filename);
      if (test)
	{
	  if (librdf_serializer_serialize_model (serializer, test, NULL, model) != 0)
	    result = FALSE;
	  TtaWriteClose (test);
	  /* uncomment this to output the model to stdout */
	  /* librdf_serializer_serialize_model (serializer, stdout, NULL, model); */
	  librdf_free_serializer(serializer);
	}
      else 
	{
	  InitInfo ("Error", "Couldn't write to the bookmark file.");
	  result = FALSE;
	}
    }
  return result;
}

/*----------------------------------------------------------------------
  Model_queryID
  Returns a unique blank node ID that doesn't yet exist in the model.
  ----------------------------------------------------------------------*/
static int Model_queryID (librdf_world *world, librdf_model *model, char * base_url, 
			   char  *template_, int start_id)
{
#define BM_MAXID 1000
  librdf_stream *stream = NULL;
  librdf_statement *partial_statement = NULL;
  librdf_node *subject = NULL;
  librdf_node *predicate = NULL;
  librdf_node *object = NULL;

  int base_url_len;
  int count = 0;
  int igenid = start_id;
  char genid[MAX_LENGTH];


  if (igenid == 0)
    igenid++;

  base_url_len = strlen (base_url);
  sprintf (genid, "%s", base_url);
  while (count < BM_MAXID)
    {
      snprintf (genid + base_url_len, sizeof (genid) - base_url_len, template_, igenid);

      subject = librdf_new_node_from_uri_string (world, (unsigned char *) genid);

      partial_statement = librdf_new_statement (world);
      librdf_statement_set_subject (partial_statement, subject);
      librdf_statement_set_predicate (partial_statement, predicate);
      librdf_statement_set_object (partial_statement, object);

      stream = librdf_model_find_statements (model, partial_statement);

      /* @@ JK: librdf_stream_end doesn't seem to work as I want */
      if (librdf_stream_end (stream)) {
	/* we found an non-existing genid */
	break;
      }
      else
	{
	  librdf_free_stream (stream);  
	  stream = NULL;
	  librdf_free_statement (partial_statement);
	  partial_statement = NULL;
	  igenid++;
	  count++;
	}
    }

  if (count == BM_MAXID)
    igenid = 0; /* didn't found anything */
  if (stream)
    librdf_free_stream (stream);
  if (partial_statement)
      librdf_free_statement (partial_statement);

  return (igenid);
}

/*----------------------------------------------------------------------
  Model_queryTopic
  Returns true if the model contains the given topicid.
  ----------------------------------------------------------------------*/
static int Model_queryTopic (librdf_world *world, librdf_model *model, char  *topicid)
{
  librdf_stream *stream = NULL;
  librdf_statement *partial_statement = NULL;
  librdf_node *subject = NULL;
  librdf_node *predicate = NULL;
  librdf_node *object = NULL;
  int found = 0;

  if (!topicid && !topicid[0] == EOS)
    return 0;

  subject = librdf_new_node_from_uri_string (world, (unsigned char *) topicid);
  predicate = librdf_new_node_from_uri_string (world, (unsigned char *) RDF_TYPE);
  object =  librdf_new_node_from_uri_string (world,
					     (unsigned char *) BMNS_TOPIC);

  partial_statement=librdf_new_statement (world);
  librdf_statement_set_subject (partial_statement, subject);
  librdf_statement_set_predicate (partial_statement, predicate);
  librdf_statement_set_object (partial_statement, object);
  
  stream = librdf_model_find_statements (model, partial_statement);

  if (librdf_stream_end (stream))
    found = 0;
  else
    found = 1;

  librdf_free_stream (stream);  
  librdf_free_statement (partial_statement);

  return (found);
}

/*----------------------------------------------------------------------
  Model_queryExists
  Returns TRUE if a matching statement exists.
  ----------------------------------------------------------------------*/
static ThotBool Model_queryExists (librdf_world *world, librdf_model *model, 
				   char *subject_str, char *predicate_str, 
				   char *object_str)
{
  librdf_statement *partial_statement = NULL;
  librdf_node *subject = NULL;
  librdf_node *predicate = NULL;
  librdf_node *object = NULL;
  ThotBool found;

  /* we need all three properties */
  if (!subject_str || !*subject_str
      || !predicate_str || !*predicate_str
      || !object_str || !*object_str)
    return FALSE;

  subject = librdf_new_node_from_uri_string (world, (unsigned char *) subject_str);
  predicate = librdf_new_node_from_uri_string (world, (unsigned char *) predicate_str);
  if (object_str)
    {
      if (librdf_heuristic_object_is_literal (object_str))
	object = librdf_new_node_from_literal (world, (unsigned char *) object_str, NULL, 0);
      else
	object = librdf_new_node_from_uri_string (world, (unsigned char *) object_str);
    }

  partial_statement=librdf_new_statement (world);
  librdf_statement_set_subject (partial_statement, subject);
  librdf_statement_set_predicate (partial_statement, predicate);
  librdf_statement_set_object (partial_statement, object);
  
  if (librdf_model_contains_statement (model, partial_statement))
    found = TRUE;
  else
    found = FALSE;

  librdf_free_statement (partial_statement);

  return (found);
} 

/*----------------------------------------------------------------------
  Model_query
  Returns a matching statement. There could be more than one
  statement. I'm only returning the first one.
  Caller has to free the returned statement.
  ----------------------------------------------------------------------*/
static librdf_statement* Model_query (librdf_world *world, librdf_model *model, 
				      char *subject_str, char *predicate_str, 
				      char *object_str)
{
  librdf_stream *stream = NULL;
  librdf_statement *partial_statement = NULL;
  librdf_statement *found = NULL;
  librdf_node *subject = NULL;
  librdf_node *predicate = NULL;
  librdf_node *object = NULL;

  subject = librdf_new_node_from_uri_string (world, (unsigned char *) subject_str);
  predicate = librdf_new_node_from_uri_string (world, (unsigned char *) predicate_str);
  if (object_str)
    {
      if (librdf_heuristic_object_is_literal (object_str))
	object = librdf_new_node_from_literal (world, (unsigned char *) object_str, NULL, 0);
      else
	object = librdf_new_node_from_uri_string (world, (unsigned char *) object_str);
    }

  partial_statement=librdf_new_statement (world);
  librdf_statement_set_subject (partial_statement, subject);
  librdf_statement_set_predicate (partial_statement, predicate);
  librdf_statement_set_object (partial_statement, object);
  
  stream = librdf_model_find_statements (model, partial_statement);

  if (!librdf_stream_end (stream)) 
    {
      librdf_statement *statement=librdf_stream_get_object(stream);
      /* we found something */
      found = librdf_new_statement_from_statement (statement);
    }
  else
    found = NULL;

  librdf_free_stream (stream);  
  librdf_free_statement (partial_statement);

  return (found);
}

/*----------------------------------------------------------------------
  Model_getNodeAsStringControl
  returns the value of a node as a string. It verifies that the value
  is a literal if the user requested it so.
  Caller must free the returned string.
  ----------------------------------------------------------------------*/
static char* Model_getNodeAsStringControl (librdf_node *node, ThotBool isLiteral)
{
  librdf_uri *uri;
  librdf_node_type type;
  char *object_str;

  type = librdf_node_get_type (node);

  /* @@ JK: some control */
  if (type == LIBRDF_NODE_TYPE_LITERAL && isLiteral == FALSE)
      return NULL;
  else if (type != LIBRDF_NODE_TYPE_LITERAL && isLiteral == TRUE)
      return NULL;

  if (type == LIBRDF_NODE_TYPE_LITERAL)
    object_str = TtaStrdup ((char *) librdf_node_get_literal_value (node));
  else
    {
      uri = librdf_node_get_uri (node);
      object_str = (char *) librdf_uri_to_string (uri);
    }
  return (object_str);
}

/*----------------------------------------------------------------------
  Model_getNodeAsString
  returns the value of a node as a string.
  Caller must free the returned string.
  ----------------------------------------------------------------------*/
static char* Model_getNodeAsString (librdf_node *node)
{
  librdf_uri *uri;
  librdf_node_type type;
  char *object_str;

  type = librdf_node_get_type (node);

  if (type == LIBRDF_NODE_TYPE_LITERAL)
    object_str = TtaStrdup ((char *) librdf_node_get_literal_value (node));
  else if (type == LIBRDF_NODE_TYPE_BLANK)
    object_str = TtaStrdup ((char *) librdf_node_get_blank_identifier (node));
  else
    {
      uri = librdf_node_get_uri (node);
      object_str = (char *) librdf_uri_to_string (uri);
    }
  return (object_str);
}

/*----------------------------------------------------------------------
  Model_getBlankNodeAsString
  returns the value of a blank node as a string.
  Caller must free the returned string.
  ----------------------------------------------------------------------*/
static char* Model_getBlankNodeAsString (librdf_node *node)
{
  librdf_node_type type;
  char *object_str;

  type = librdf_node_get_type (node);

  if (type == LIBRDF_NODE_TYPE_BLANK)
    object_str = TtaStrdup ((char *) librdf_node_get_blank_identifier (node));
  else
    object_str = NULL;

  return (object_str);
}

/*----------------------------------------------------------------------
  Model_getObjectAsString
  returns the object as a string, for a given subject and predicate.
  Caller must free the returned string.
  ----------------------------------------------------------------------*/
static char* Model_getObjectAsString (librdf_world *world, librdf_model *model,
				      librdf_node *subject, char *predicate_url)
{
  librdf_node *predicate, *node;
  librdf_iterator *iterator;
  librdf_uri *uri;

  char *object_str;

  predicate = librdf_new_node_from_uri_string (world, (unsigned char *) predicate_url);
  iterator = librdf_model_get_targets (model, subject, predicate);
  librdf_free_node (predicate);
  if (librdf_iterator_have_elements (iterator))
    {
      librdf_node_type type;
      
      node = (librdf_node *) librdf_iterator_get_object (iterator);
      type = librdf_node_get_type (node);
      /* why is this uri being parsed as a literal? */
      if (type == LIBRDF_NODE_TYPE_LITERAL)
	object_str = TtaStrdup ((char *) librdf_node_get_literal_value (node));
      else
	{
	  uri = librdf_node_get_uri (node);
	  object_str = (char *) librdf_uri_to_string (uri);
	}
    }
  else
    object_str = NULL;
  librdf_free_iterator (iterator);

  return (object_str);
}

/*----------------------------------------------------------------------
  Model_getItemType
  Returns the type of a given item
  ----------------------------------------------------------------------*/
static BookmarkElements Model_getItemType (int ref, char *subject_url)
{
  char   *uri_str;
  BookmarkElements bm_type = BME_UNKNOWN;

  librdf_world *world;
  librdf_model *model; 
  librdf_storage *storage;
  librdf_node *subject;
  librdf_node *predicate;
  librdf_node *object;
  librdf_stream* stream;
  librdf_statement *partial_statement;

  if (!BM_Context_get (ref, &world, &model, &storage))
    return BME_UNKNOWN;

  subject = librdf_new_node_from_uri_string (world, (unsigned char *) subject_url);
  predicate = librdf_new_node_from_uri_string (world, (unsigned char *) RDF_TYPE);
  object = librdf_model_get_target (model, subject, predicate);

  if (!object)
    {
      /* maybe we got a blank node, try a different subject */
      librdf_free_node (subject);
      subject = librdf_new_node_from_blank_identifier  (world, (unsigned char *) subject_url);
      object = librdf_model_get_target (model, subject, predicate);
    }

  if (!object)
    {
      librdf_free_node (subject);
      librdf_free_node (predicate);
      return BME_UNKNOWN;
    }
  
  librdf_free_node (object);
  object = NULL;

  partial_statement = librdf_new_statement (world);
  librdf_statement_set_subject (partial_statement, subject);
  librdf_statement_set_predicate (partial_statement, predicate);
  librdf_statement_set_object (partial_statement, object);

  stream = librdf_model_find_statements (model, partial_statement);
  librdf_free_statement (partial_statement);
  
   while (!librdf_stream_end (stream))
    {
      librdf_statement *statement = librdf_stream_get_object (stream);

      if(!statement) 
	{
	  fprintf(stderr, "librdf_stream_next returned NULL\n");
	  break;
	}

      object = librdf_statement_get_object (statement);
      uri_str = Model_getNodeAsString (object);
      
      if (!strcmp (uri_str, BMNS_TOPIC))
	bm_type = BME_TOPIC;
      else if (!strcmp (uri_str, BMNS_BOOKMARK))
	bm_type = BME_BOOKMARK;
      else if (!strcmp (uri_str, BMNS_SEPARATOR))
	bm_type = BME_SEPARATOR;
      TtaFreeMemory (uri_str);

      if (bm_type != BME_UNKNOWN)
	break;
      
      librdf_stream_next (stream);
    }
   
   librdf_free_stream (stream);

  return bm_type;
}


/*----------------------------------------------------------------------
  Model_getItemInfo
  Fills a bookmark item with all info we're interested in from the model.
  ----------------------------------------------------------------------*/
static void Model_getItemInfo (int ref, librdf_node *subject,
			       BookmarkP item)
{
  librdf_world *world;
  librdf_model *model; 
  librdf_storage *storage;

  if (!BM_Context_get (ref, &world, &model, &storage))
    return;

  /* 
  ** query the model to get all the info we need to sort and display the
  ** bookmarks.
  */
  
  if (item->bm_type == BME_SEEALSO)
    {
      /* Get the title and nickname. We don't use subject because we are looking
       for a different property, associated rather to self_url */
      if (item->self_url && item->blank_id)
	{
	  librdf_node *node;

	  node = librdf_new_node_from_blank_identifier (world, (unsigned char *) item->blank_id);
	  item->title = Model_getObjectAsString (world, model, node, 
						 RDFS_LABEL);
	  if (!item->title)
	    item->title = Model_getObjectAsString (world, model, node, 
						   DC1NS_TITLE);
	  item->nickname = Model_getObjectAsString (world, model, node, 
						    BMNS_NICKNAME);
	  librdf_free_node (node);
	}
    }
  else if (item->bm_type == BME_TOPIC || item->bm_type == BME_BOOKMARK)
    {
      if (item->bm_type == BME_TOPIC)
	{
	  item->parent_url = Model_getObjectAsString (world, model, subject,
						      BMNS_SUBTOPICOF);
	  item->collapsed = Model_queryState (ref, item->self_url,
					      BMNS_COLLAPSED);
	}
      else if (item->bm_type == BME_BOOKMARK)
	{
	  List *dump = NULL;
	  
	  item->bm_type = BME_BOOKMARK;
	  /* JK: Get a list of all the topics to which this bookmark belongs */
	  Model_dumpBookmarkTopics (ref, item, &dump);
	  item->parent_url_list = dump;       
	  item->parent_url = NULL;
	  item->bookmarks = Model_getObjectAsString (world, model, subject, 
						     BMNS_BOOKMARKS);
	}
      
      /* modified  */
      item->modified = Model_getObjectAsString (world, model, subject, DC1NS_DATE);
      /* title */
      item->title = Model_getObjectAsString (world, model, subject, DC1NS_TITLE);
    }
}

/*----------------------------------------------------------------------
  Model_addItemToCollection
  Adds the item identified by item_url to the collection associated
  to topic_url.
  Returns TRUE if succesful.
  @@ Add instructions to add it to the end of before a given element
  @@ check what happens when the collection is empty
  @@ Add new function to exchange one item too (for reorganizing)
  ----------------------------------------------------------------------*/
static ThotBool Model_addItemToCollection (librdf_world *world, librdf_model *model, 
					   char *topic_url, char *item_url)
{
  librdf_node* subject;
  librdf_node* predicate;
  librdf_node* object;
  librdf_statement *partial_statement;
  char *tmp_url;
  char *subject_url;
  ThotBool res;

  subject = librdf_new_node_from_uri_string (world, (unsigned char *) topic_url);
  predicate = librdf_new_node_from_uri_string (world, (unsigned char *) BMNS_COLLECTION);
  object = librdf_model_get_target (model, subject, predicate);

  if (!object)
    {
      librdf_free_node (subject);
      librdf_free_node (predicate);
      return FALSE;
    }

  tmp_url = Model_getNodeAsString (object);
  librdf_free_node (object);
  librdf_free_node (predicate);
  predicate = librdf_new_node_from_uri_string (world, (unsigned char *) RDF_REST);
  subject_url = NULL;

  while (tmp_url && strcmp (tmp_url, RDF_NIL))
    {
      librdf_free_node (subject);
      if (subject_url)
	TtaFreeMemory (subject_url);
      subject_url = tmp_url;
      subject = librdf_new_node_from_blank_identifier (world, (unsigned char *) subject_url);
      object = librdf_model_get_target (model, subject, predicate);
      if (object)
	{
	  tmp_url = Model_getNodeAsString (object);
	  librdf_free_node (object);
	}
      else
	tmp_url = NULL;
    }

  /* we found the last one */
  if (subject_url && tmp_url)
    {
      librdf_node * new_bnode;

      res = TRUE;
      /* create the blank node that will be associated with the item */
      new_bnode = librdf_new_node_from_blank_identifier (world, NULL);

      /* delete the previous statement that says NIL */
      object = librdf_new_node_from_uri_string (world, (unsigned char *) RDF_NIL);
      partial_statement = librdf_new_statement (world);
      /* subject and predicate came from the above while */
      librdf_statement_set_subject (partial_statement, subject);
      librdf_statement_set_predicate (partial_statement, predicate);
      librdf_statement_set_object (partial_statement, object);
      if (librdf_model_contains_statement (model, partial_statement))
	librdf_model_remove_statement (model, partial_statement);
      librdf_free_statement (partial_statement);

      /* make the rest point to the new blank node */
      subject = librdf_new_node_from_blank_identifier (world, (unsigned char *) subject_url);
      predicate = librdf_new_node_from_uri_string (world, (unsigned char *) RDF_REST);
      object = librdf_new_node_from_node  (new_bnode);
      add_statement (world, model, subject, predicate, object);

      /* add the collection statements refering to the blank node */
      subject = librdf_new_node_from_node (new_bnode);
      predicate = librdf_new_node_from_uri_string (world, (unsigned char *) RDF_FIRST);
      object = librdf_new_node_from_uri_string (world, (unsigned char *) item_url);
      add_statement (world, model, subject, predicate, object);

      subject = new_bnode;
      predicate = librdf_new_node_from_uri_string (world, (unsigned char *) RDF_REST);
      object = librdf_new_node_from_uri_string (world, (unsigned char *) RDF_NIL);
      add_statement (world, model, subject, predicate, object);
    }
  else
    res = FALSE;

  return res;
}

/*----------------------------------------------------------------------
  BM_deleteItemCollection
  Deletes the item identified by item_url from the collection associated
  to topic_url.
  Returns TRUE if succesful.
  @@ What to do when we have deleted everything in the collection
  ----------------------------------------------------------------------*/
ThotBool BM_deleteItemCollection (int ref, char *topic_url, char *item_url, 
				  ThotBool *isBlankNode, ThotBool cleanAll)
{
  ThotBool res;

  librdf_world *world;
  librdf_model *model;
  librdf_storage *storage;

  librdf_node* subject;
  librdf_node* predicate;
  librdf_node* object;
  librdf_node* predicate_f;
  librdf_node* predicate_r;
  librdf_node* object_r;
  librdf_statement *partial_statement;

  char *cur_blank_node_id;
  char *subject_url;

  if (!BM_Context_get (ref, &world, &model, &storage))
    return FALSE;

  subject = librdf_new_node_from_uri_string (world, (unsigned char *) topic_url);
  predicate = librdf_new_node_from_uri_string (world, (unsigned char *) BMNS_COLLECTION);
  object = librdf_model_get_target (model, subject, predicate);
  librdf_free_node (predicate);

  if (!object)
    {
      librdf_free_node (subject);
      return FALSE;
    }

  cur_blank_node_id = Model_getNodeAsString (object);
  librdf_free_node (object);
  
  if (!strcmp (cur_blank_node_id, RDF_NIL))
    {
      /* the collection is empty */
      librdf_free_node (subject);
      return FALSE;
    }

  predicate_f = librdf_new_node_from_uri_string (world, (unsigned char *) RDF_FIRST);
  predicate_r = librdf_new_node_from_uri_string (world, (unsigned char *) RDF_REST);

  subject_url = NULL;
  object_r = NULL;
  /* find the blank node that points to the item and store it in cur_blank_node_id */
  while (cur_blank_node_id && strcmp (cur_blank_node_id, RDF_NIL))
    {
      librdf_free_node (subject);
      if (subject_url)
	TtaFreeMemory (subject_url);
      subject_url = cur_blank_node_id;
      subject = librdf_new_node_from_blank_identifier (world, (unsigned char *) subject_url);
      object = librdf_model_get_target (model, subject, predicate_f);
      if (object)
	{
	  cur_blank_node_id = Model_getNodeAsString (object);
	  librdf_free_node (object);
	  if (!strcmp (cur_blank_node_id, item_url))
	    {
	      /* found copy the next object */
	      object_r = librdf_model_get_target (model, subject, predicate_r);
	      TtaFreeMemory (cur_blank_node_id);
	      break;
	    }
	  else
	    {
	      TtaFreeMemory (cur_blank_node_id);
	      object = librdf_model_get_target (model, subject, predicate_r);
	      if (object)
		cur_blank_node_id = Model_getNodeAsString (object);
	      else 
		cur_blank_node_id = NULL;
	      librdf_free_node (object);
	    }
	}
      else
	cur_blank_node_id = NULL;
    }

  /* we found the entry, now remove it from the list */
  if (object_r)
    {
      partial_statement = librdf_new_statement (world);

      /* supress both first and rest properties from this statement */

      /* remove rest */
      librdf_statement_set_subject (partial_statement, subject);
      predicate = librdf_new_node_from_node (predicate_r);
      librdf_statement_set_predicate (partial_statement, predicate);
      object = librdf_new_node_from_node (object_r);
      librdf_statement_set_object (partial_statement, object);
      librdf_model_remove_statement (model, partial_statement);
      librdf_statement_clear (partial_statement);

      /* remove first */
      subject = librdf_new_node_from_blank_identifier (world, (unsigned char *) subject_url);
      object = librdf_model_get_target (model, subject, predicate_f);
      if (librdf_node_get_type (object) == LIBRDF_NODE_TYPE_BLANK)
	*isBlankNode = TRUE;
      else
	*isBlankNode = FALSE;
      librdf_statement_set_subject (partial_statement, subject);
      predicate = librdf_new_node_from_node (predicate_f);
      librdf_statement_set_predicate (partial_statement, predicate);
      librdf_statement_set_object (partial_statement, object);
      librdf_model_remove_statement (model, partial_statement);
      librdf_statement_clear (partial_statement);

      /* remove all the statements associated with this node */
      if (cleanAll)
	BM_deleteItem (ref, subject_url, TRUE);

      /* find the previous collection statement */
      subject = NULL;
      object = librdf_new_node_from_blank_identifier (world, (unsigned char *) subject_url);
      subject = librdf_model_get_source (model, predicate_r, object);

      /* remove the previous rest statement and make a new one pointing to
	 the rest statement of the one we just removed */
      if (subject)
	{
	  librdf_node *tmp;
	  tmp = librdf_new_node_from_node (subject);
	  librdf_statement_set_subject (partial_statement, tmp);
	  predicate = librdf_new_node_from_node (predicate_r);
	  librdf_statement_set_predicate (partial_statement, predicate);
	  librdf_statement_set_object (partial_statement, object);
	  librdf_model_remove_statement (model, partial_statement);
	  librdf_statement_clear (partial_statement);

	  predicate = librdf_new_node_from_node (predicate_r);
	  add_statement (world, model, subject, predicate, object_r);
	}
      else
	{
	  /* no subject, check if it's the first member of the collection */
	  predicate = librdf_new_node_from_uri_string (world, (unsigned char *) BMNS_COLLECTION);
	  subject = librdf_model_get_source (model, predicate, object);
	  if (subject)
	    {
	      librdf_node *tmp;
	      tmp = librdf_new_node_from_node (subject);
	      librdf_statement_set_subject (partial_statement, tmp);
	      librdf_statement_set_predicate (partial_statement, predicate);
	      librdf_statement_set_object (partial_statement, object);
	      librdf_model_remove_statement (model, partial_statement);
	      librdf_statement_clear (partial_statement);
	      
	      predicate = librdf_new_node_from_uri_string (world, 
							   (unsigned char *) BMNS_COLLECTION);
	      add_statement (world, model, subject, predicate, object_r);
	    }
	  else 	/* nothing found. It must be an error */
	    {
	    librdf_free_node (object);
	    librdf_free_node (predicate);
	    }
	}
      librdf_free_statement (partial_statement);
    }
  else
    {
      librdf_free_node (subject);
      res = FALSE;
    }

  librdf_free_node (predicate_f);
  librdf_free_node (predicate_r);

  return res;
}

/*----------------------------------------------------------------------
  Model_collectionContainsItem
  Returns TRUE if the collection starting at root contains an item
  with the URL given as a parameter.
  ----------------------------------------------------------------------*/
static ThotBool Model_collectionContainsItem (int ref, librdf_node *root, char *url)
{
  ThotBool res = FALSE;
  librdf_world *world;
  librdf_model *model;
  librdf_storage *storage;

  librdf_node *object;

  librdf_node *predicate_f;
  librdf_node *predicate_r;
  librdf_node *current;
  
  char *tmp_url;

  if (!BM_Context_get (ref, &world, &model, &storage))
    return FALSE;

  if (!root || !url || url[0] == EOS)
    return FALSE;

  tmp_url = Model_getNodeAsString (root);
  if (!strcmp (tmp_url, RDF_NIL))
    {
      /* empty collection */
      TtaFreeMemory (tmp_url);
      return FALSE;
    }
  TtaFreeMemory (tmp_url);

  predicate_f = librdf_new_node_from_uri_string (world, (unsigned char *) RDF_FIRST);
  predicate_r = librdf_new_node_from_uri_string (world, (unsigned char *) RDF_REST);
  
  current = librdf_new_node_from_node (root);
  tmp_url = NULL;
  while (current)
    {
      object = librdf_model_get_target (model, current, predicate_f);
      tmp_url = Model_getNodeAsString (object);
      librdf_free_node (object);
      if (!strcmp (tmp_url, url))
	{
	  librdf_free_node (current);
	  res = TRUE;
	  break;
	}
      TtaFreeMemory (tmp_url);
      tmp_url = NULL;
      object = librdf_model_get_target (model, current, predicate_r);
      librdf_free_node (current);
      if (object)
	{
	  tmp_url = Model_getNodeAsString (object);
	  if (!strcmp (tmp_url, RDF_NIL))
	    {
	      /* end of collection, nothing found */
	      librdf_free_node (object);
	      break;
	    }
	  else
	    {
	      TtaFreeMemory (tmp_url);
	      tmp_url = NULL;
	      current = object;
	    }
	}
      else
	current = NULL;
    }

  if (tmp_url)
    TtaFreeMemory (tmp_url);

  librdf_free_node (predicate_f);
  librdf_free_node (predicate_r);

  return res;
}

/*----------------------------------------------------------------------
  BM_intializeCollection
  Adds the first and rest items to a new collection
  ----------------------------------------------------------------------*/
ThotBool BM_initializeCollection (int ref, BookmarkP new_item)
{
  librdf_node *new_item_node;
  ThotBool res = FALSE;

  librdf_world *world;
  librdf_model *model;
  librdf_storage *storage;

  librdf_node *subject;
  librdf_node *predicate;
  librdf_node *object;
  librdf_statement *partial_statement;

  if (new_item->bm_type != BME_TOPIC)  
    return FALSE;

  if (!BM_Context_get (ref, &world, &model, &storage))
    return FALSE;

  /* add the collection statement and open state */
  new_item_node = librdf_new_node_from_uri_string (world, 
						   (unsigned char *) new_item->self_url);
  
  /* do the collection statements exist already for this item ? */
  predicate = librdf_new_node_from_uri_string (world,  (unsigned char *) BMNS_COLLECTION);
  object = librdf_model_get_target (model, new_item_node, predicate);
  
  if (!object)
    {
      /* the related topic collection statements don't exist yet,
	 create them */
      partial_statement = librdf_new_statement (world);
      
      subject = librdf_new_node_from_node (new_item_node);
      /* predicate was created above */
      object = librdf_new_node_from_uri_string (world, (unsigned char *) RDF_NIL);
      librdf_statement_set_subject (partial_statement, subject);
      librdf_statement_set_predicate (partial_statement, predicate);
      librdf_statement_set_object (partial_statement, object);
      librdf_model_add_statement (model, partial_statement);
      librdf_statement_clear (partial_statement);
      
      subject = librdf_new_node_from_node (new_item_node);
      predicate = librdf_new_node_from_uri_string (world, (unsigned char *) BMNS_STATE);
      object = librdf_new_node_from_uri_string (world, (unsigned char *) BMNS_EXPANDED);
      librdf_statement_set_subject (partial_statement, subject);
      librdf_statement_set_predicate (partial_statement, predicate);
      librdf_statement_set_object (partial_statement, object);
      librdf_model_add_statement (model, partial_statement);
      librdf_free_statement (partial_statement);
      res = TRUE;
    }
  else
    {
      librdf_free_node (predicate);
      librdf_free_node (object);
      res = FALSE;
    }

  librdf_free_node (new_item_node);

  return res;
}

/*----------------------------------------------------------------------
  BM_addItemToCollection
  Adds item_url to the collection associated to topic_url.
  If an previous_item_url parameter is given, the new item is inserted
  at that point in the collection; if this parameter is NULL, the
  new item is added at the bottom of the collection.
  isBlankNode is used to know if we're pointing to a separator.
  Returns TRUE if succesful.
  @@ see if we can use librdf_node_equals (librdf_node* first_node, librdf_node* second_node);
  (non 0 if equal) to simplify
  @@ See how to check if an item existed already in the collection before
  adding it. Probably with another function, getting a stream with "first ="
  ----------------------------------------------------------------------*/
ThotBool BM_addItemToCollection (int ref, char *topic_url, 
				 char *previous_item_url,
				 BookmarkP new_item,
				 ThotBool addFirst)
{
  ThotBool res = FALSE;

  librdf_world *world;
  librdf_model *model;
  librdf_storage *storage;

  librdf_node *subject;
  librdf_node *predicate;
  librdf_node *object;
  librdf_statement *partial_statement;

  librdf_node *predicate_f;
  librdf_node *predicate_r;
  librdf_node *root;
  librdf_node *current;

  char *tmp_url;

  if (!BM_Context_get (ref, &world, &model, &storage))
    return FALSE;

  if (!topic_url || topic_url[0] == EOS)
    return FALSE;

  subject = librdf_new_node_from_uri_string (world, (unsigned char *) topic_url);
  predicate = librdf_new_node_from_uri_string (world, (unsigned char *) BMNS_COLLECTION);
  root = librdf_model_get_target (model, subject, predicate);
  librdf_free_node (subject);
  librdf_free_node (predicate);

  if (!root)
    return FALSE;

  predicate_f = librdf_new_node_from_uri_string (world, (unsigned char *) RDF_FIRST);
  predicate_r = librdf_new_node_from_uri_string (world, (unsigned char *) RDF_REST);

  tmp_url = Model_getNodeAsString (root);
  if (!strcmp (tmp_url, RDF_NIL))
    {
      /* empty collection, add it as the first one */
      addFirst = TRUE;
    }
  TtaFreeMemory (tmp_url);

  /* if the collection already contains this item, skip it */
  if (Model_collectionContainsItem (ref, root, new_item->self_url))
    {
      librdf_free_node (root);
      librdf_free_node (predicate_f);
      librdf_free_node (predicate_r);
      return FALSE;
    }

  if (!addFirst) /* browse the collection to find the previous node */
    {
      current = librdf_new_node_from_node (root);
      while (current)
	{
	  if (previous_item_url) /* find a given node */
	    {
	      object = librdf_model_get_target (model, current, predicate_f);
	      if (object)
		{
		  tmp_url = Model_getNodeAsString (object);
		  librdf_free_node (object);
		  if (!strcmp (tmp_url, previous_item_url))
		    {
		      /* found ! */
		      TtaFreeMemory (tmp_url);
		      break; 
		    }
		  else
		    {
		      /* get the next one */
		      TtaFreeMemory (tmp_url);
		      object = librdf_model_get_target (model, current, predicate_r);
		      librdf_free_node (current);
		      current = object;
		      tmp_url = Model_getNodeAsString (object);
		      if (!strcmp (tmp_url, RDF_NIL))
			{
			  /* this is the last node. we didn't find it */
			  TtaFreeMemory (tmp_url);
			  librdf_free_node (current);
			  current = NULL;
			  break; 
			}
		      else
			TtaFreeMemory (tmp_url);
		    }
		}
	      else /* invalid collection */
		{
		  librdf_free_node (current);
		  current = NULL;
		  break;
		}
	    }
	  else  /* get the last item */
	    {
	      object = librdf_model_get_target (model, current, predicate_r);
	      if (object)
		{
		  tmp_url = Model_getNodeAsString (object);
		  if (!strcmp (tmp_url, RDF_NIL))
		    {
		      /* found the item */
		      librdf_free_node (object);
		      TtaFreeMemory (tmp_url);
		      break;
		    }
		  else
		    {
		      librdf_free_node (current);
		      current = object;
		    }
		}
	      else
		{
		  /* invalid collection */
		  librdf_free_node (current);
		  current = NULL;
		  break;
		}
	    }
	}
    }

  /* at this point current contains the previous node or NULL if nothing
   was found */
  
  if (addFirst)
    {
      librdf_node *new_item_node;
      /* supress the statement pointing to the first item */
      
      /* make the new statement's rest point to the previous one */      
      partial_statement = librdf_new_statement (world);
      new_item_node = librdf_new_node_from_blank_identifier (world, NULL);
      
      /* add the first statement pointing to the new item */
      if (new_item->bm_type == BME_SEPARATOR)
	object = librdf_new_node_from_blank_identifier (world, 
							(unsigned char *) new_item->self_url);
      else
	object = librdf_new_node_from_uri_string (world, 
						  (unsigned char *) new_item->self_url);

      subject = librdf_new_node_from_node (new_item_node);
      predicate = librdf_new_node_from_node (predicate_f);
      librdf_statement_set_subject (partial_statement, subject);
      librdf_statement_set_predicate (partial_statement, predicate);
      librdf_statement_set_object (partial_statement, object);
      librdf_model_add_statement (model, partial_statement);
      librdf_statement_clear (partial_statement);

      /* add the rest statement pointing to what was the root before */
      subject = librdf_new_node_from_node (new_item_node);
      predicate = librdf_new_node_from_node (predicate_r);
      object = librdf_new_node_from_node (root);
      librdf_statement_set_subject (partial_statement, subject);
      librdf_statement_set_predicate (partial_statement, predicate);
      librdf_statement_set_object (partial_statement, object);
      librdf_model_add_statement (model, partial_statement);

      /* delete the previous statement saying that root was the beginning of
	 the collection */
      subject = librdf_new_node_from_uri_string (world, (unsigned char *) topic_url);
      predicate = librdf_new_node_from_uri_string (world, 
						   (unsigned char *) BMNS_COLLECTION);
      object = librdf_new_node_from_node (root);
      librdf_statement_set_subject (partial_statement, subject);
      librdf_statement_set_predicate (partial_statement, predicate);
      librdf_statement_set_object (partial_statement, object);
      librdf_model_remove_statement (model, partial_statement);
      librdf_statement_clear (partial_statement);
      
      /* and make the new node the root of the collection */
      subject = librdf_new_node_from_uri_string (world, (unsigned char *) topic_url);
      predicate = librdf_new_node_from_uri_string (world,
						   (unsigned char *) BMNS_COLLECTION);
      librdf_statement_set_subject (partial_statement, subject);
      librdf_statement_set_predicate (partial_statement, predicate);
      librdf_statement_set_object (partial_statement, new_item_node);
      librdf_model_add_statement (model, partial_statement);

      librdf_free_statement (partial_statement);
      res = TRUE;
    }
  else if (current)
    { /* add it after the previous item or to the bottom */
      librdf_node *new_item_node;

      /* make the new statement's rest point to the previous one */      
      partial_statement = librdf_new_statement (world);
      new_item_node = librdf_new_node_from_blank_identifier (world, NULL);
  
      /* add the first statement pointing to the new item */
      if (new_item->bm_type == BME_SEPARATOR)
	object = librdf_new_node_from_blank_identifier (world, 
							(unsigned char *) new_item->self_url);
      else
	object = librdf_new_node_from_uri_string (world, 
						  (unsigned char *) new_item->self_url);

      subject = librdf_new_node_from_node (new_item_node);
      predicate = librdf_new_node_from_node (predicate_f);
      librdf_statement_set_subject (partial_statement, subject);
      librdf_statement_set_predicate (partial_statement, predicate);
      librdf_statement_set_object (partial_statement, object);
      librdf_model_add_statement (model, partial_statement);
      librdf_statement_clear (partial_statement);

      /* add the rest statement pointing to the previous's rest */
      subject = librdf_new_node_from_node (new_item_node);
      predicate = librdf_new_node_from_node (predicate_r);
      object = librdf_model_get_target (model, current, predicate_r);
      librdf_statement_set_subject (partial_statement, subject);
      librdf_statement_set_predicate (partial_statement, predicate);
      librdf_statement_set_object (partial_statement, object);
      librdf_model_add_statement (model, partial_statement);
      librdf_statement_clear (partial_statement);

      /* delete the previous rest statement */
      subject = librdf_new_node_from_node (current);
      predicate = librdf_new_node_from_node (predicate_r);
      object = librdf_model_get_target (model, current, predicate_r);
      librdf_statement_set_subject (partial_statement, subject);
      librdf_statement_set_predicate (partial_statement, predicate);
      librdf_statement_set_object (partial_statement, object);
      librdf_model_remove_statement (model, partial_statement);
      librdf_statement_clear (partial_statement);

      /* and make the previous node rest point to the new one */
      subject = librdf_new_node_from_node (current);
      predicate = librdf_new_node_from_node (predicate_r);
      librdf_statement_set_subject (partial_statement, subject);
      librdf_statement_set_predicate (partial_statement, predicate);
      librdf_statement_set_object (partial_statement, new_item_node);
      librdf_model_add_statement (model, partial_statement);

      librdf_free_statement (partial_statement);
      librdf_free_node (current);
      res = TRUE;
    }
  
  if (res && new_item->bm_type == BME_TOPIC)
    BM_initializeCollection (ref, new_item);
  
  librdf_free_node (root);
  librdf_free_node (predicate_f);
  librdf_free_node (predicate_r);

  return res;
}

/*----------------------------------------------------------------------
  Model_removeItemIfDisplaced 
  Removes the corresponding collection entries of an item if the item
  has changed topics.
  ----------------------------------------------------------------------*/
static void Model_removeItemIfDisplaced (int ref, librdf_statement *statement, BookmarkP me)
{
  librdf_world *world;
  librdf_model *model; 
  librdf_storage *storage;

  ThotBool isBlankNode;
  librdf_node *object = NULL;
  librdf_uri *rdf_uri;
  char *uri_str;
  List *cur;

  if (!BM_Context_get (ref, &world, &model, &storage))
    return;

  object = librdf_statement_get_object (statement);
  rdf_uri = librdf_node_get_uri (object);	      
  uri_str = (char *) librdf_uri_to_string (rdf_uri);

  if (me->parent_url)
    {
      if (strcmp (uri_str, me->parent_url))
	BM_deleteItemCollection (ref, uri_str, me->self_url,
				 &isBlankNode, FALSE);
    }
  else
    {
      cur = me->parent_url_list;
      while (cur)
	{
	  char *tmp;

	  tmp = (char *) cur->object;
	  
	  if (!strcmp (uri_str, tmp))
	    break;
	  
	  cur = cur->next;	      
	}
      if (!cur) /* nothing found, delete it */
	BM_deleteItemCollection (ref, uri_str, me->self_url,
				 &isBlankNode, TRUE);
    }
  free (uri_str);
}

/*----------------------------------------------------------------------
  AddBookmarkTopicList
  ----------------------------------------------------------------------*/
static void AddBookmarkTopicList (int ref, BookmarkP me)

{
  librdf_world *world;
  librdf_model *model; 
  librdf_storage *storage;

  List *cur;
  char *url;
  char *parent_url;
  librdf_statement *statement;
  librdf_node *subject, *object, *predicate;

  if (!BM_Context_get (ref, &world, &model, &storage))
    return;

  url = me->self_url;
  cur = me->parent_url_list;
  while (cur) 
    {
      subject = librdf_new_node_from_uri_string (world,  (unsigned char *) url);
      statement = librdf_new_statement (world);
      librdf_statement_set_subject (statement, subject);
      predicate = librdf_new_node_from_uri_string (world,  (unsigned char *) BMNS_HASTOPIC);
      librdf_statement_set_predicate (statement, predicate);
      parent_url = (char *) cur->object;
      object = librdf_new_node_from_uri_string (world, (unsigned char *) parent_url);
      librdf_statement_set_object (statement, object);
      librdf_model_add_statement (model, statement);
      librdf_free_statement (statement);
      /* add the item to the collection */
      BM_addItemToCollection (ref, parent_url, NULL, me, FALSE);
      cur = cur->next;
    }  
}


/*
** Public API
*/
/*----------------------------------------------------------------------
  BM_generateCollection
  Given a list of sorted bookmark items, generates an equivalent
  collection.
  ----------------------------------------------------------------------*/
ThotBool BM_generateCollection (int ref, List *items)
{
  List *cur;
  BookmarkP me;

  cur = items;

  if (!cur)
    return FALSE;

  /* add the root collection */
  me = (BookmarkP) cur->object;
  BM_initializeCollection (ref, me);

  cur = cur->next;
  while (cur)
    {
      me = (BookmarkP) cur->object;
      BM_addItemToCollection (ref, me->parent_url, NULL, me, FALSE);
      cur = cur->next;
    }

  return TRUE;
}

/*----------------------------------------------------------------------
  BM_tmpsave
  Gets the temporary file name of a downloaded bookmark file and
  save the model to it.
  ----------------------------------------------------------------------*/
ThotBool BM_tmpsave (int ref)
{
  char *tmpname;
  ThotBool result;

  tmpname = BMContext_tmpfile (ref);
  if (tmpname)
    /* save the modified model in the temporary file */
    result = BM_save (ref, tmpname);
  else
    result = FALSE;

  return result;
}

/*----------------------------------------------------------------------
  BM_save
  Writes the bookmark model to filename.
  ----------------------------------------------------------------------*/
ThotBool BM_save (int ref, char *filename)
{
  librdf_world *world;
  librdf_model *model; 
  librdf_storage *storage;
  ThotBool result;

  if (!BM_Context_get (ref, &world, &model, &storage))
    result = FALSE;
  else if (filename && *filename)
    result = serialize (world, model, "rdfxml", filename, "application/rdf+xml");
  return result;
}

/*----------------------------------------------------------------------
  BM_saveOrModify
  In function of the type of document, either saves it or
  just updates the modified flag
  ----------------------------------------------------------------------*/
ThotBool BM_saveOrModify (int ref, Document doc)
{
  ThotBool res;

  /* save the model */
  if (ref == 0)
    res = BM_save (ref, GetLocalBookmarksFile ());
  else
    {
      /* save the modified model in the temporary file */
      if (BM_tmpsave (ref))
	TtaSetDocumentModified (doc);
      res = TtaIsDocumentModified (doc);
    }

  return res;
}

/*----------------------------------------------------------------------
  BM_parse
  Parses a bookmark file into the model.
  ----------------------------------------------------------------------*/
ThotBool BM_parse (int ref, char *filename, char *base)
{
  ThotBool res = FALSE;

  if (filename && *filename)
    {
      librdf_world *world;
      librdf_model *model; 
      librdf_storage *storage;
      char *local_filename;
      char *local_base;

      if (!BM_Context_get (ref, &world, &model, &storage))
	return FALSE;

      if (!IsHTTPPath (filename) && IsFilePath (filename))
	{
	  local_filename = TtaStrdup (filename);
	  WWWToLocal (local_filename);
	}
      else
	local_filename = filename;

      if (!IsHTTPPath (base) && IsFilePath (base))
	{
	  local_base =  TtaStrdup (base);
	  WWWToLocal (local_base);
	}
      else 
	local_base = base; 

      res = parse_file (world, model, local_filename, local_base, FALSE);

      if (filename != local_filename)
	TtaFreeMemory (local_filename);

      if (base != local_base)
	TtaFreeMemory (local_base);
    }

  return res;
}


/*----------------------------------------------------------------------
  BM_addBookmark
  Adds a bookmark to a storage model.
  ----------------------------------------------------------------------*/
ThotBool BM_addBookmark (Document doc, int ref, BookmarkP me)
{
  librdf_node* subject;
  librdf_node* predicate;
  librdf_node* object;
  librdf_world *world;
  librdf_model *model; 
  librdf_storage *storage;

  char bookmarkid[MAX_LENGTH];
  char *template_ = "#ambookmark%d";
  /* should be something else? */
  char *base_uri = GetLocalBookmarksBaseURI ();
  char *tmp;

 if (!BM_Context_get (ref, &world, &model, &storage))
   return FALSE;

  /* get new blank node id */
  if (genid_counter == 0) 
    {
      /* couldn't create a genid */
      return FALSE;
    }
  
  /* if the user specified a topic, control that it exists */
  if (me->parent_url && me->parent_url[0] != EOS)
    {
      if (! Model_queryTopic (world, model, me->parent_url))
	return FALSE;
    }
  
  genid_counter = Model_queryID (world, model, base_uri, template_, 
				 genid_counter);
  if (genid_counter == 0) 
    {
      /* couldn't create a genid */
      return FALSE;
    }

#ifdef URN
  /* @@ JK urn test */
  strcpy (bookmarkid, BM_newURN ());
#else
  sprintf (bookmarkid, "%s", base_uri);
  sprintf (bookmarkid + strlen (base_uri), template_, genid_counter++);
#endif /* URN */

  /* complete the bookmark with the value of its url */
  me->self_url = TtaStrdup (bookmarkid);

  /* add the bookmark to the model */
  /* bookmark type */
  subject = librdf_new_node_from_uri_string (world, (unsigned char *) bookmarkid);
  predicate = librdf_new_node_from_uri_string (world, (unsigned char *) RDF_TYPE);
  object =  librdf_new_node_from_uri_string (world,
					     (unsigned char *) BMNS_BOOKMARK);
  add_statement (world, model, subject, predicate, object);

  /* topic folders */
  if (me->parent_url_list)
    AddBookmarkTopicList (ref, me);

  /* creator */
  tmp = (char *)TtaConvertByteToMbs ((unsigned char *)me->author, ISO_8859_1);
  subject = librdf_new_node_from_uri_string (world, (unsigned char *) bookmarkid);
  predicate = librdf_new_node_from_uri_string (world, (unsigned char *) DC1NS_CREATOR);
  object =  librdf_new_node_from_literal (world, 
					  (unsigned char *) tmp,
					  NULL,  /* literal XML language */
 					  0);    /* non 0 if literal is XML */
  add_statement (world, model, subject, predicate, object);
  TtaFreeMemory (tmp);

  /* created */
  subject = librdf_new_node_from_uri_string (world, (unsigned char *) bookmarkid);
  predicate = librdf_new_node_from_uri_string (world, (unsigned char *) ANNOTNS_CREATED);
  object =  librdf_new_node_from_literal (world, 
					  (unsigned char *) me->created, 
					  NULL,  /* literal XML language */
 					  0);    /* non 0 if literal is XML */
  add_statement (world, model, subject, predicate, object);


  /* modified  */
  subject = librdf_new_node_from_uri_string (world, (unsigned char *) bookmarkid);
  predicate = librdf_new_node_from_uri_string (world, (unsigned char *) DC1NS_DATE);
  object =  librdf_new_node_from_literal (world, 
					  (unsigned char *) me->modified, 
					  NULL,  /* literal XML language */
 					  0);    /* non 0 if literal is XML */
  add_statement (world, model, subject, predicate, object);


  /* title */
  tmp = (char *)TtaConvertByteToMbs ((unsigned char *)me->title, ISO_8859_1); 
  subject = librdf_new_node_from_uri_string (world, (unsigned char *) bookmarkid);
  predicate = librdf_new_node_from_uri_string (world, (unsigned char *) DC1NS_TITLE);
  object =  librdf_new_node_from_literal (world, 
					  (unsigned char *) tmp,
					  NULL,  /* literal XML language */
 					  0);    /* non 0 if literal is XML */
  add_statement (world, model, subject, predicate, object);
  TtaFreeMemory (tmp);

 /* description */
  tmp = (char *)TtaConvertByteToMbs ((unsigned char *)me->description, ISO_8859_1); 
  subject = librdf_new_node_from_uri_string (world, (unsigned char *) bookmarkid);
  predicate = librdf_new_node_from_uri_string (world, (unsigned char *) DC1NS_DESCRIPTION);
  object =  librdf_new_node_from_literal (world, 
					  (unsigned char *) tmp,
					  NULL,  /* literal XML language */
 					  0);    /* non 0 if literal is XML */
  add_statement (world, model, subject, predicate, object);
  TtaFreeMemory (tmp);

  /* bookmarks */
  subject = librdf_new_node_from_uri_string (world, (unsigned char *) bookmarkid);
  predicate = librdf_new_node_from_uri_string (world, (unsigned char *) BMNS_BOOKMARKS);
  object =  librdf_new_node_from_uri_string (world, (unsigned char *) me->bookmarks);
  add_statement (world, model, subject, predicate, object);

  /* context */
  if (me->context)
    {
      subject = librdf_new_node_from_uri_string (world, (unsigned char *) bookmarkid);
      predicate = librdf_new_node_from_uri_string (world, (unsigned char *) ANNOTNS_CONTEXT);
      object =  librdf_new_node_from_literal (world, 
					      (unsigned char *) me->context, 
					      NULL,  /* literal XML language */
					      0);    /* non 0 if literal is XML */
      add_statement (world, model, subject, predicate, object);
    }

  /* save the model */
  BM_saveOrModify (ref, doc);

  return TRUE;
}

/*----------------------------------------------------------------------
  BM_addTopic
  Adds a topic to a storage model.
  ----------------------------------------------------------------------*/
ThotBool BM_addTopic (Document doc, int ref, BookmarkP me, ThotBool generateID)
{
  librdf_node* subject;
  librdf_node* predicate;
  librdf_node* object;
  librdf_world *world;
  librdf_model *model; 
  librdf_storage *storage;

  char topicid[MAX_LENGTH];
  char *base_uri = GetLocalBookmarksBaseURI ();
  char *template_ = "#amtopic%d";
  static int genid_counter = 1;
  char *tmp;

  if (!BM_Context_get (ref, &world, &model, &storage))
    return FALSE;

  if (generateID)
    {
      /* get new blank node id */
      if (genid_counter == 0) 
	{
	  /* couldn't create a genid */
	  return FALSE;
	}
      
      genid_counter = Model_queryID (world, model, base_uri,
				     template_, genid_counter);
      if (genid_counter == 0) 
	{
	  /* couldn't create a genid */
	  return FALSE;
	}
      
#ifdef URN
      /* @@ JK urn test */
      strcpy (topicid, BM_newURN ());
#else
      sprintf (topicid, "%s", base_uri);
      sprintf (topicid + strlen (base_uri), template_, genid_counter++);
#endif /* URN */
      me->self_url = TtaStrdup (topicid);
    }
  else
    strcpy (topicid, me->self_url);

#if 0 /* the following block let's the user type his own URL. USeful for
	 debugging. */
  /* @@ JK: For the moment, I'll use the URL I input as the topic id. this
     line should be removed later on */
  if (!me->self_url)
    return FALSE;
  strcpy (topicid, me->self_url);

  /* control that it doesn't exist yet */
  if (Model_queryTopic (world, model, topicid))
    return FALSE;
  /* @@ JK: End of test routines */
#endif

  /* if the user gave a parent topic, check if it exists. */
  if (me->parent_url && me->parent_url[0] != EOS)
    {
      if (! Model_queryTopic (world, model, me->parent_url))
	return FALSE;
    }

  /* topic type */
  subject = librdf_new_node_from_uri_string (world, (unsigned char *) topicid);
  predicate = librdf_new_node_from_uri_string (world, (unsigned char *) RDF_TYPE);
  object =  librdf_new_node_from_uri_string (world,
					     (unsigned char *) BMNS_TOPIC);
  add_statement (world, model, subject, predicate, object);

  /* parent topic */
  if (me->parent_url && me->parent_url[0] != EOS)
    {
      subject = librdf_new_node_from_uri_string (world, (unsigned char *) topicid);
      predicate = librdf_new_node_from_uri_string (world, (unsigned char *) BMNS_SUBTOPICOF);
      object =  librdf_new_node_from_uri_string (world,
						  (unsigned char *) me->parent_url);
      add_statement (world, model, subject, predicate, object);

      /* @@ JK: thesaurus */
    }

  /* creator */
  tmp = (char *)TtaConvertByteToMbs ((unsigned char *) me->author, ISO_8859_1); 
  subject = librdf_new_node_from_uri_string (world, (unsigned char *) topicid);
  predicate = librdf_new_node_from_uri_string (world, (unsigned char *) DC1NS_CREATOR);
  object =  librdf_new_node_from_literal (world, 
					  /* literal string value */
					  (unsigned char *) tmp,
					  NULL,  /* literal XML language */
 					  0);    /* non 0 if literal is XML */
  add_statement (world, model, subject, predicate, object);
  TtaFreeMemory (tmp);

  /* created */
  subject = librdf_new_node_from_uri_string (world, (unsigned char *) topicid);
  predicate = librdf_new_node_from_uri_string (world, (unsigned char *) ANNOTNS_CREATED);
  object =  librdf_new_node_from_literal (world, 
					  /* literal string value */
					  (unsigned char *) me->created,  
					  NULL,  /* literal XML language */
 					  0);    /* non 0 if literal is XML */
  add_statement (world, model, subject, predicate, object);


  /* modified  */
  subject = librdf_new_node_from_uri_string (world, (unsigned char *) topicid);
  predicate = librdf_new_node_from_uri_string (world, (unsigned char *) DC1NS_DATE);
  object =  librdf_new_node_from_literal (world, 
					  /* literal string value */
					  (unsigned char *) me->modified, 
					  NULL,  /* literal XML language */
 					  0);    /* non 0 if literal is XML */
  add_statement (world, model, subject, predicate, object);


  /* title */
  tmp = (char *)TtaConvertByteToMbs ((unsigned char *)me->title, ISO_8859_1); 
  subject = librdf_new_node_from_uri_string (world, (unsigned char *) topicid);
  predicate = librdf_new_node_from_uri_string (world, (unsigned char *) DC1NS_TITLE);
  object =  librdf_new_node_from_literal (world, 
					  (unsigned char *) tmp,
					  NULL,  /* literal XML language */
 					  0);    /* non 0 if literal is XML */
  add_statement (world, model, subject, predicate, object);
  TtaFreeMemory (tmp);

 /* description */
  tmp = (char *)TtaConvertByteToMbs ((unsigned char *)me->description, ISO_8859_1); 
  subject = librdf_new_node_from_uri_string (world, (unsigned char *) topicid);
  predicate = librdf_new_node_from_uri_string (world, (unsigned char *) DC1NS_DESCRIPTION);
  object =  librdf_new_node_from_literal (world, 
					  (unsigned char *) tmp,
					  NULL,  /* literal XML language */
 					  0);    /* non 0 if literal is XML */
  add_statement (world, model, subject, predicate, object);
  TtaFreeMemory (tmp);

  /* add it to the collection */
  /* JK: @@@ have to do something specific for the root parent */
  BM_addItemToCollection (ref, me->parent_url, NULL, me, FALSE);

  /* save the model */
  BM_saveOrModify (ref, doc);

  return (TRUE);
}

/*----------------------------------------------------------------------
  BM_addSeparator
  Adds a separator to a storage model. 
  ----------------------------------------------------------------------*/
ThotBool BM_addSeparator (Document doc, int ref, 
			  char *topic_url, 
			  char *previous_item_url)
{
  BookmarkP me;
  librdf_node* subject;
  librdf_node* predicate;
  librdf_node* object;
  librdf_world *world;
  librdf_model *model; 
  librdf_storage *storage;
  char *url;

  if (!BM_Context_get (ref, &world, &model, &storage))
    return FALSE;

  /* add the separator to the model */
  subject = librdf_new_node_from_blank_identifier (world, NULL);
  url = Model_getNodeAsString (subject); /* remember it for adding it to the collection */
  predicate = librdf_new_node_from_uri_string (world, (unsigned char *) RDF_TYPE);
  object =  librdf_new_node_from_uri_string (world, (unsigned char *) BMNS_SEPARATOR);
  add_statement (world, model, subject, predicate, object);
  
  /*
  ** add the separator to the collection 
  */

  me = Bookmark_new ();
  me->self_url = url;
  me->bm_type = BME_SEPARATOR;

  BM_addItemToCollection (ref, topic_url, previous_item_url, me, FALSE);

  /* 
  ** save the model 
  */
  BM_saveOrModify (ref, doc);

  Bookmark_free (me);

  return TRUE;
}

/*----------------------------------------------------------------------
  Model_dumpAsList
  bm_type says what kind of element we want to dump
  dump contains the info list of topics and bookmarks from the model;
  it must be freed by the user.
  Returns the number of matching statements.
  ----------------------------------------------------------------------*/
int Model_dumpAsList (int ref, List **dump, BookmarkElements bm_type)
{
  librdf_stream* stream;
  librdf_statement *partial_statement;
  int count = 0;

  librdf_node *subject = NULL;
  librdf_node *predicate = NULL;
  librdf_node *object = NULL;
  librdf_uri *uri;
  librdf_world *world;
  librdf_model *model; 
  librdf_storage *storage;

  char *uri_str;

  BookmarkP item;

  if (!BM_Context_get (ref, &world, &model, &storage))
    return (0);

  /* dump the topics */
  predicate = librdf_new_node_from_uri_string (world, (unsigned char *) RDF_TYPE);
  if (bm_type == BME_TOPIC)
    object =  librdf_new_node_from_uri_string (world,
					       (unsigned char *) BMNS_TOPIC);
  else
    object =  librdf_new_node_from_uri_string (world,
					       (unsigned char *) BMNS_BOOKMARK);

  partial_statement = librdf_new_statement (world);
  librdf_statement_set_subject (partial_statement, subject);
  librdf_statement_set_predicate (partial_statement, predicate);
  librdf_statement_set_object (partial_statement, object);

  stream = librdf_model_find_statements (model, partial_statement);
  librdf_free_statement (partial_statement);

  while (!librdf_stream_end (stream))
    {
      librdf_statement *statement = librdf_stream_get_object (stream);
      if(!statement) 
	{
	  fprintf(stderr, "librdf_stream_next returned NULL\n");
	  break;
	}

      subject = librdf_statement_get_subject (statement);
      uri = librdf_node_get_uri (subject);

      if (!uri)
	{
	  fprintf(stderr, "no node uri\n");
	  break;
	}

      uri_str = (char *) librdf_uri_to_string (uri);
      if (!uri_str || *uri_str == EOS)
	{
	  fprintf(stderr, "no node uri\n");
	  break;
	}
    
      item = Bookmark_new ();
      item->self_url = uri_str;
      item->bm_type = bm_type;

      /* 
      ** query the model to get all the info we need to sort and display the
      ** bookmarks.
      */

      Model_getItemInfo (ref, subject, item);

      List_add (dump, (void *) item);

#ifdef BMFILE_DEBUG      
      if (item->self_url)
	printf ("found node with url %s\n", item->self_url);

      if (item->parent_url)
	printf ("  with parent topic url %s\n", item->parent_url);

      if (item->bookmarks)
	printf ("  and bookmarks url %s\n", item->bookmarks);
#endif /* BMFILE_DEBUG */

      count++;
      librdf_stream_next (stream);
    }

  librdf_free_stream (stream);  

#ifdef BMFILE_DEBUG  
  fprintf(stderr, "matching statements: %d\n", count);
#endif /* BMFILE_DEBUG */
  return (count);
}

/*----------------------------------------------------------------------
  Model_dumpTopicChild
  dump contains the info list of all child topics under
  the parent topic URL.
  Caller must free the returned list.
  Returns TRUE if at least a matching statement was found.
  ----------------------------------------------------------------------*/
ThotBool Model_dumpTopicChild (int ref, char *parent_topic_url, List **dump)
{
  librdf_stream* stream;
  librdf_statement *partial_statement;

  librdf_node *subject = NULL;
  librdf_node *predicate = NULL;
  librdf_node *object = NULL;
  librdf_world *world;
  librdf_model *model; 
  librdf_storage *storage;

  char *uri_str;
  BookmarkP item;

  if (!BM_Context_get (ref, &world, &model, &storage))
    return FALSE;

  /* get all the topics that are a child of the parent_topic */
  predicate = librdf_new_node_from_uri_string (world, (unsigned char *) BMNS_SUBTOPICOF);
  object =  librdf_new_node_from_uri_string (world, (unsigned char *) parent_topic_url);

  partial_statement = librdf_new_statement (world);
  librdf_statement_set_subject (partial_statement, subject);
  librdf_statement_set_predicate (partial_statement, predicate);
  librdf_statement_set_object (partial_statement, object);

  stream = librdf_model_find_statements (model, partial_statement);
  librdf_free_statement (partial_statement);

  while (!librdf_stream_end (stream))
    {
      librdf_statement *statement = librdf_stream_get_object (stream);
      if(!statement) 
	{
	  fprintf(stderr, "librdf_stream_next returned NULL\n");
	  break;
	}

      subject = librdf_statement_get_subject (statement);
      /* verify if it's a topic */
      uri_str = Model_getNodeAsStringControl (subject, FALSE);
      if (uri_str && Model_queryExists (world, model, uri_str, RDF_TYPE, BMNS_TOPIC))
	Model_dumpTopicChild (ref, uri_str, dump); /* yes, add all its subtopics first */
      if (uri_str)
	free (uri_str);
      librdf_stream_next (stream);
    }
    librdf_free_stream (stream);

    item = Bookmark_new ();
    List_add (dump, (void *) item);
    item->self_url = TtaStrdup (parent_topic_url);
    item->bm_type = BME_TOPIC;

    subject = librdf_new_node_from_uri_string (world, (unsigned char *) parent_topic_url);
    predicate = librdf_new_node_from_uri_string (world, (unsigned char *) RDF_TYPE);
    object =  librdf_new_node_from_uri_string (world,
					       (unsigned char *) BMNS_TOPIC);
    partial_statement = librdf_new_statement (world);
    librdf_statement_set_subject (partial_statement, subject);
    librdf_statement_set_predicate (partial_statement, predicate);
    librdf_statement_set_object (partial_statement, object);
    stream = librdf_model_find_statements (model, partial_statement);
    librdf_free_statement (partial_statement);

    if (!librdf_stream_end (stream))
      {
	librdf_statement *statement = librdf_stream_get_object (stream);
	subject = librdf_statement_get_subject (statement);
	Model_getItemInfo (ref, subject, item);
      }
    librdf_free_stream (stream);  

    return (TRUE);
}

/*----------------------------------------------------------------------
  Model_dumpSeeAlso
  dumps all the SeeAlso properties related to a given resource.
  resource_url gives the URL of the resource.
  dump contains all the SeeAlso properties; caller must free this list.
  Returns the number of found matching statements was found.
  ----------------------------------------------------------------------*/
int Model_dumpSeeAlso (int ref, char *resource_url, char *topic_url, List **dump)
{
  librdf_stream* stream;
  librdf_statement *partial_statement;

  librdf_node *subject = NULL;
  librdf_node *predicate = NULL;
  librdf_node *object = NULL;
  librdf_world *world;
  librdf_model *model; 
  librdf_storage *storage;

  char *uri_str;
  char *blank_id;

  BookmarkP item;

  int count;

  if (!BM_Context_get (ref, &world, &model, &storage))
    return 0;

  if (resource_url == NULL)
    return 0;

  /* get all the topics that are a child of the parent_topic */
  subject = librdf_new_node_from_uri_string (world, (unsigned char *) resource_url);
  predicate = librdf_new_node_from_uri_string (world, (unsigned char *) RDFS_SEEALSO);
  object = NULL;

  partial_statement = librdf_new_statement (world);
  librdf_statement_set_subject (partial_statement, subject);
  librdf_statement_set_predicate (partial_statement, predicate);
  librdf_statement_set_object (partial_statement, object);

  stream = librdf_model_find_statements (model, partial_statement);
  librdf_free_statement (partial_statement);

  count = 0;
  while (!librdf_stream_end (stream))
    {
      librdf_statement *statement = librdf_stream_get_object (stream);
      if(!statement) 
	{
	  fprintf(stderr, "librdf_stream_next returned NULL\n");
	  break;
	}

      object = librdf_statement_get_object (statement);
      /* check if it's a blank node */
      if (librdf_node_get_type (object) == LIBRDF_NODE_TYPE_BLANK)
	{
	  subject = object;
	  predicate = librdf_new_node_from_uri_string (world, (unsigned char *) BMNS_BOOKMARKS);
	  object = librdf_model_get_target (model, subject, predicate);
	  librdf_free_node (predicate);
	  uri_str = Model_getNodeAsStringControl (object, FALSE);
	  blank_id = Model_getBlankNodeAsString (subject);
	  librdf_free_node (object);
	}
      else
	{
	  uri_str = Model_getNodeAsStringControl (object, FALSE);
	  blank_id = NULL;
	}

      if (uri_str)
	{
	  item = Bookmark_new ();
	  List_add (dump, (void *) item);
	  item->parent_url = TtaStrdup (topic_url);
	  item->self_url = TtaStrdup (resource_url);
	  item->blank_id = blank_id;
	  item->bookmarks = TtaStrdup (uri_str);
	  item->bm_type = BME_SEEALSO;
	  Model_getItemInfo (ref, subject, item);
	  free (uri_str);
	  count++;
	}
      librdf_stream_next (stream);
    }
  librdf_free_stream (stream);

  return (count);
}

/*----------------------------------------------------------------------
  Model_dumpSeeAlsoAsList
  bm_list is a list of topics and bookmarks. For each entry in me,
  the function queries the model and dumps all the related seeAlso 
  resources.
  ----------------------------------------------------------------------*/
void Model_dumpSeeAlsoAsList (int ref, List **bm_list)
{

  List       *cur, *next;
  List       *dump;
  BookmarkP   me;

 /* for each entry, find the SeeAlso */
  cur = *bm_list;
  while (cur) 
    {
      next = cur->next;
      me = (BookmarkP) cur->object;
      if (me->bm_type == BME_BOOKMARK
	  || (me->bm_type == BME_TOPIC && !me->collapsed))
	{
	  dump = NULL;
	  if (Model_dumpSeeAlso (ref, me->self_url, me->parent_url, &dump))
	    {
	      /* sort dump and insert it into the list */
	      cur->next = dump;
	      List_merge (dump, next);
	    }
	}
      cur = next;
    }
}

/*----------------------------------------------------------------------
  Model_dumpTopicBookmarks
  dumps all the bookmarks contained in a list of topics.
  topic_list gievs the list of topic.
  dump contains the info list of topics and bookmarks from the model;
  it must be fred by the caller.
  Returns TRUE if at least a matching statement was found.
  ----------------------------------------------------------------------*/
ThotBool Model_dumpTopicBookmarks (int ref, List *topic_list, List **dump)
{
  librdf_stream* stream;
  librdf_statement *partial_statement;

  librdf_node *subject = NULL;
  librdf_node *predicate = NULL;
  librdf_node *object = NULL;
  librdf_world *world;
  librdf_model *model; 
  librdf_storage *storage;

  char *uri_str;
  BookmarkP topic, item;

  List *cur;

  if (!BM_Context_get (ref, &world, &model, &storage))
    return FALSE;

  /* now for each topic in the list, we get all its bookmarks */
  cur = topic_list;

  /* we'll reuse the same statement */
  partial_statement = librdf_new_statement (world);

  while (cur)
    {
      topic = (BookmarkP)cur->object;
      /* get all the bookmarks that are a stored in the topic */
      subject = NULL;
      predicate = librdf_new_node_from_uri_string (world, (unsigned char *) BMNS_HASTOPIC);
      object =  librdf_new_node_from_uri_string (world, (unsigned char *) topic->self_url);
      librdf_statement_set_subject (partial_statement, subject);
      librdf_statement_set_predicate (partial_statement, predicate);
      librdf_statement_set_object (partial_statement, object);

      stream = librdf_model_find_statements (model, partial_statement);

      while (!librdf_stream_end (stream))
	{
	  librdf_statement *statement = librdf_stream_get_object (stream);
	  if (!statement) 
	    {
	      fprintf(stderr, "librdf_stream_next returned NULL\n");
	      continue;
	    }
	  subject = librdf_statement_get_subject (statement);
	  uri_str = Model_getNodeAsStringControl (subject, FALSE);
	  
	  /* verify if it's a bookmark */
	  if (uri_str && Model_queryExists (world, model, uri_str, RDF_TYPE, BMNS_BOOKMARK))
	    {
	      /* if not already stored (multiple topics), do it */
	      if (!BMList_containsURL (*dump, uri_str))
		{
		  /* yes, store it */
		  item = Bookmark_new ();
		  List_add (dump, (void *) item);
		  item->self_url = TtaStrdup (uri_str);
		  item->bm_type = BME_BOOKMARK;
		  Model_getItemInfo (ref, subject, item);
		}
	    }
	  if (uri_str)
	    free (uri_str);
	  librdf_stream_next (stream);
	}
      librdf_free_stream (stream);
      librdf_statement_clear (partial_statement);
      cur = cur->next;  
    }
  librdf_free_statement (partial_statement);

  return (TRUE);
}


/*----------------------------------------------------------------------
  Model_dumpBookmarkTopics
  creates a list (dump) with all the topics to which bookmark belongs to.
  Caller must free dump.
  Returns TRUE if at least a matching statement was found.  
  ----------------------------------------------------------------------*/
ThotBool Model_dumpBookmarkTopics (int ref, BookmarkP me, List **dump)
{
  librdf_stream* stream;
  librdf_statement *partial_statement;

  librdf_node *subject = NULL;
  librdf_node *predicate = NULL;
  librdf_node *object = NULL;
  librdf_world *world;
  librdf_model *model; 
  librdf_storage *storage;

  char *uri_str = NULL;

  if (!me)
    return FALSE;

  if (!BM_Context_get (ref, &world, &model, &storage))
    return FALSE;

  /* we'll reuse the same statement */
  partial_statement = librdf_new_statement (world);

  /* get all the bookmarks that are a stored in the topic */
  subject = librdf_new_node_from_uri_string (world, (unsigned char *) me->self_url);
  predicate = librdf_new_node_from_uri_string (world, (unsigned char *) BMNS_HASTOPIC);
  object =  NULL;
  librdf_statement_set_subject (partial_statement, subject);
  librdf_statement_set_predicate (partial_statement, predicate);
  librdf_statement_set_object (partial_statement, object);
  
  stream = librdf_model_find_statements (model, partial_statement);
  
  while (!librdf_stream_end (stream))
    {
      librdf_statement *statement = librdf_stream_get_object (stream);
      if (!statement) 
	{
	  fprintf(stderr, "librdf_stream_next returned NULL\n");
	  continue;
	}
      object = librdf_statement_get_object (statement);
      uri_str = Model_getNodeAsStringControl (object, FALSE);
      
      if (uri_str) 	  /* store it */
	{
	  char *tmp;
	  /* we copy it to avoid problems between diff. malloc functions */
	  tmp = TtaStrdup (uri_str);
	  List_add (dump, (void *) tmp);
	  free (uri_str);
	}
      librdf_stream_next (stream);
    }
  librdf_free_stream (stream);
  librdf_statement_clear (partial_statement);
  
  librdf_free_statement (partial_statement);

  return (TRUE);
}

/*----------------------------------------------------------------------
  Model_dumpTopicAsList
  Dumps the full contents of a topic into a list called dump.
  Caller must free dump.
  parent_topic_url gives the url of the parent topic.
  sort is TRUE if the caller wants the results to be sorted.
  Returns TRUE if at least a matching statement was found.  
  ----------------------------------------------------------------------*/
ThotBool Model_dumpTopicAsList (int  ref, char *parent_topic_url, 
				ThotBool sort, ThotBool prune, List **dump)
{
  List *topic_list = NULL;
  List *bookmark_list = NULL;
  List *cur;
  BookmarkP bookmark;

  *dump = NULL;

  /* see how it happens */
  if (Model_dumpCollection (ref, parent_topic_url, TRUE, dump) == 0)
    {
      /* first we get all the topics */
      Model_dumpTopicChild  (ref, parent_topic_url, &topic_list);
      if (prune)
	/* remove all non expanded topics */
        topic_list = BM_pruneTopics (&topic_list, parent_topic_url);
      /* then all the bookmarks contained in each topic */
      Model_dumpTopicBookmarks (ref, topic_list, &bookmark_list);
      if (bookmark_list)
	{
	  bookmark_list = BM_expandBookmarks (&bookmark_list, prune, topic_list);
	  cur = bookmark_list;
	  while (cur)
	    {
	      bookmark = (BookmarkP) cur->object;
	      /* erases all the bookmarks that don't belong to this topic */
	      if (!BMList_containsURL (topic_list, bookmark->parent_url))
		{
		  List_delObject (&bookmark_list, cur->object);
		  cur = bookmark_list;
		  continue;
		}
	      cur = cur->next;
	    }
	}
      /*  both lists */
      *dump = List_merge (topic_list, bookmark_list);
      /* sort the result */
      if (sort)
	BM_bookmarksSort (dump);
    }

  /* add the seeAlso's */
  Model_dumpSeeAlsoAsList (ref, dump);

  return (TRUE);
}

/*----------------------------------------------------------------------
  @@ JK: To be retired when collection works
  Model_dumpSeq
  dumps all the Seq items related to a given bookmark file.
  dump contains all the SeeAlso properties; caller must free this list.
  Returns the number of entries in the Seq list
  ----------------------------------------------------------------------*/
int Model_dumpSeq (int ref, List **dump)
{
  librdf_stream* stream;
  librdf_statement *partial_statement;

  librdf_node *subject = NULL;
  librdf_node *predicate = NULL;
  librdf_node *object = NULL;
  librdf_world *world;
  librdf_model *model; 
  librdf_storage *storage;

  char *uri_str = NULL;

  int count;

  if (!BM_Context_get (ref, &world, &model, &storage))
    return 0;

  /* get the first Seq in the file (we consider there is only one for the moment */
  subject = NULL;
  predicate = librdf_new_node_from_uri_string (world, (unsigned char *) RDF_TYPE);
  object = librdf_new_node_from_uri_string (world, (unsigned char *) RDF_SEQ);

  partial_statement = librdf_new_statement (world);
  librdf_statement_set_subject (partial_statement, subject);
  librdf_statement_set_predicate (partial_statement, predicate);
  librdf_statement_set_object (partial_statement, object);

  stream = librdf_model_find_statements (model, partial_statement);
  librdf_free_statement (partial_statement);

  while (!librdf_stream_end (stream))
    {
      librdf_statement *statement = librdf_stream_get_object (stream);
      if(!statement) 
	{
	  fprintf(stderr, "librdf_stream_next returned NULL\n");
	  break;
	}

      subject = librdf_statement_get_subject (statement);
      uri_str = Model_getNodeAsString (subject);
      if (uri_str)
	{
	  char *uri_str2;

	  printf("seq subject: %s", (uri_str) ? uri_str : "<none>");
	  uri_str2 = Model_getNodeAsStringControl (object, FALSE);
	  if (uri_str2)
	    printf(" object: %s\n", (uri_str2) ? uri_str2 : "<none>");
	  TtaFreeMemory (uri_str2);
	  break;
	}
      librdf_stream_next (stream);
    }
  librdf_free_stream (stream);

  if (!uri_str)
    return 0;

  subject = librdf_new_node_from_uri_string (world, (unsigned char *) uri_str);
  TtaFreeMemory (uri_str);
  predicate = NULL;
  object = NULL;

  partial_statement = librdf_new_statement (world);
  librdf_statement_set_subject (partial_statement, subject);
  librdf_statement_set_predicate (partial_statement, predicate);
  librdf_statement_set_object (partial_statement, object);

  stream = librdf_model_find_statements (model, partial_statement);
  librdf_free_statement (partial_statement);

  count = 0;
  while (!librdf_stream_end (stream))
    {
      librdf_statement *statement = librdf_stream_get_object (stream);
      if(!statement) 
	{
	  fprintf(stderr, "librdf_stream_next returned NULL\n");
	  break;
	}

      subject = librdf_statement_get_subject (statement);
      uri_str = Model_getNodeAsString (subject);
      if (uri_str)
        printf("suject: %s", (uri_str) ? uri_str : "<none>");
      TtaFreeMemory (uri_str);
      predicate = librdf_statement_get_predicate (statement);
      uri_str = Model_getNodeAsStringControl (predicate, FALSE);
      if (uri_str) 
	printf(" predicate: %s\n", (uri_str) ? uri_str : "<none>");
      TtaFreeMemory (uri_str);
      object = librdf_statement_get_object (statement);
      uri_str = Model_getNodeAsStringControl (object, FALSE);
      if (uri_str)
        printf(" object: %s\n", (uri_str) ? uri_str : "<none>");
      TtaFreeMemory (uri_str);
      librdf_stream_next (stream);
    }
  librdf_free_stream (stream);
  return (count);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
static ThotBool Model_getCollectionItem (int ref, char *identifier,
					   char **first, char **rest)
{
  librdf_stream* stream;
  librdf_statement *partial_statement;

  librdf_node *subject = NULL;
  librdf_node *predicate = NULL;
  librdf_node *object = NULL;
  librdf_world *world;
  librdf_model *model; 
  librdf_storage *storage;

  char *uri_str;

  if (!BM_Context_get (ref, &world, &model, &storage))
    return 0;

  subject = librdf_new_node_from_blank_identifier (world, (unsigned char *) identifier);
  predicate =  librdf_new_node_from_uri_string (world,(unsigned char *)  RDF_FIRST);
  object = NULL;

  partial_statement = librdf_new_statement (world);
  librdf_statement_set_subject (partial_statement, subject);
  librdf_statement_set_predicate (partial_statement, predicate);
  librdf_statement_set_object (partial_statement, object);

  stream = librdf_model_find_statements (model, partial_statement);
  librdf_free_statement (partial_statement);

  uri_str = NULL;
  *first = NULL;
  *rest = NULL;

  while (!librdf_stream_end (stream))
    {
      librdf_statement *statement = librdf_stream_get_object (stream);
      if(!statement) 
	{
	  fprintf(stderr, "librdf_stream_next returned NULL\n");
	  break;
	}
      
      object = librdf_statement_get_object (statement);
      uri_str = Model_getNodeAsString (object);
      if (uri_str)
	{
	  *first = uri_str;
	  break;
	}
      librdf_stream_next (stream);
    }
  librdf_free_stream (stream);

  if (!uri_str)
    return FALSE;

  subject = librdf_new_node_from_blank_identifier (world, (unsigned char *) identifier);
  predicate =  librdf_new_node_from_uri_string (world, (unsigned char *) RDF_REST);
  object = NULL;

  partial_statement = librdf_new_statement (world);
  librdf_statement_set_subject (partial_statement, subject);
  librdf_statement_set_predicate (partial_statement, predicate);
  librdf_statement_set_object (partial_statement, object);

  stream = librdf_model_find_statements (model, partial_statement);
  librdf_free_statement (partial_statement);

  uri_str = NULL;
  *rest = NULL;

  while (!librdf_stream_end (stream))
    {
      librdf_statement *statement = librdf_stream_get_object (stream);
      if(!statement) 
	{
	  fprintf(stderr, "librdf_stream_next returned NULL\n");
	  break;
	}
      
      object = librdf_statement_get_object (statement);
      uri_str = Model_getNodeAsString (object);
      if (uri_str)
	{
	  *rest = uri_str;
	  break;
	}
      librdf_stream_next (stream);
    }
  librdf_free_stream (stream);

  if (!uri_str)
    {
      TtaFreeMemory (*first);
      *first = NULL;
    }

  return (*first != NULL && *rest != NULL);
}

/*----------------------------------------------------------------------
  Model_getCollectionRoot
  dumps all the rdf:collection items related to a given bookmark file.
  dump contains all the SeeAlso properties; caller must free this list.
  Returns the URL of the root topic. User has to free the string.

  As we don't have a way to know what is the manifest or index inside
  the rdf file, we just try to find the root element (the one that
  has no subtopicof property) and start from it.
  ----------------------------------------------------------------------*/
char * Model_getCollectionRoot (int ref, char *topic_url)
{
  librdf_stream* stream;
  librdf_statement *partial_statement;

  librdf_node *subject = NULL;
  librdf_node *predicate = NULL;
  librdf_node *object = NULL;
  librdf_world *world;
  librdf_model *model; 
  librdf_storage *storage;

  char *uri_str = NULL;
  char *res;

  if (!BM_Context_get (ref, &world, &model, &storage))
    return NULL;

  /* get the collection set */
  if (topic_url)
    subject = librdf_new_node_from_uri_string (world, (unsigned char *) topic_url);
  else
    subject = NULL;
  predicate = librdf_new_node_from_uri_string (world, (unsigned char *) BMNS_COLLECTION);
  object = NULL;

  partial_statement = librdf_new_statement (world);
  librdf_statement_set_subject (partial_statement, subject);
  librdf_statement_set_predicate (partial_statement, predicate);
  librdf_statement_set_object (partial_statement, object);

  stream = librdf_model_find_statements (model, partial_statement);
  librdf_free_statement (partial_statement);

  res = NULL;

  predicate = librdf_new_node_from_uri_string (world, (unsigned char *) BMNS_SUBTOPICOF);
  object = NULL;
  
  while (!librdf_stream_end (stream))
    {
      librdf_statement *statement = librdf_stream_get_object (stream);
      if(!statement) 
	{
	  fprintf(stderr, "librdf_stream_next returned NULL\n");
	  break;
	}

      subject = librdf_statement_get_subject (statement);
      object = librdf_model_get_target (model, subject, predicate);
      if (!object) {
	/* we found the root topic */
	uri_str = Model_getNodeAsString (subject);
	/* check if this statement has a subtopicof property */
	if (uri_str)
	  res = uri_str;
	break;
      }
      librdf_free_node (object);
      librdf_stream_next (stream);
    }
  librdf_free_stream (stream);
  librdf_free_node (predicate);

  return (res);
}

/*----------------------------------------------------------------------
  Model_dumpCollection
  dumps all the rdf:collection items related to a given bookmark file.
  dump contains all the SeeAlso properties; caller must free this list.
  Returns the number of entries in the Seq list
  ----------------------------------------------------------------------*/
int Model_dumpCollection (int ref, char *topic_url, ThotBool isRoot, List **dump)
{
  librdf_stream* stream;
  librdf_statement *partial_statement;

  librdf_node *subject = NULL;
  librdf_node *predicate = NULL;
  librdf_node *object = NULL;
  librdf_world *world;
  librdf_model *model; 
  librdf_storage *storage;

  char *uri_str = NULL;
  char *first, *rest;

  int count = 0;

  BookmarkP item;

  if (!BM_Context_get (ref, &world, &model, &storage))
    return 0;

  /* get the collection set */
  if (topic_url)
    {
      if (isRoot)
	{
	  /* add the info related to the root item */
	  item = Bookmark_new ();
	  item->bm_type = Model_getItemType (ref, topic_url);
	  item->self_url = TtaStrdup (topic_url);
	  subject = librdf_new_node_from_uri_string (world, (unsigned char *) topic_url);
	  Model_getItemInfo (ref, subject, item);
	  librdf_free_node (subject);
	  List_add (dump, (void *) item);
	}
      subject = librdf_new_node_from_uri_string (world, (unsigned char *) topic_url);
    }
  else
    subject = NULL;
  predicate = librdf_new_node_from_uri_string (world, (unsigned char *) BMNS_COLLECTION);
  object = NULL;

  partial_statement = librdf_new_statement (world);
  librdf_statement_set_subject (partial_statement, subject);
  librdf_statement_set_predicate (partial_statement, predicate);
  librdf_statement_set_object (partial_statement, object);

  stream = librdf_model_find_statements (model, partial_statement);
  librdf_free_statement (partial_statement);

  while (!librdf_stream_end (stream))
    {
      librdf_statement *statement = librdf_stream_get_object (stream);
      if(!statement) 
	{
	  fprintf(stderr, "librdf_stream_next returned NULL\n");
	  break;
	}

      subject = librdf_statement_get_subject (statement);
      uri_str = Model_getNodeAsString (subject);
      if (uri_str)
	{
	  TtaFreeMemory (uri_str);
	  object = librdf_statement_get_object (statement);
	  uri_str = Model_getNodeAsString (object);
	  break;
	}
      librdf_stream_next (stream);
    }
  librdf_free_stream (stream);

  if (!uri_str)
    return 0;

  /* browse the collection set, by following the rdf:first and rest properties */
  Model_getCollectionItem (ref, uri_str, &first, &rest);
  while (first && rest) 
    {
      count++;

      /* yes, store it */
      item = Bookmark_new ();
      item->bm_type = Model_getItemType (ref, first);
      subject = librdf_new_node_from_uri_string (world, (unsigned char *) first);
      item->self_url = TtaStrdup (first);
      Model_getItemInfo (ref, subject, item);
      if (topic_url)
	item->parent_url = TtaStrdup (topic_url);
      else
	item->parent_url = NULL;
      librdf_free_node (subject);
      /* we should reverse the list */
      List_addEnd (dump, (void *) item);
      if (item->bm_type == BME_TOPIC && !item->collapsed)
	{
	  int count2;
	  List *tmp_dump = NULL;
	  count2 = Model_dumpCollection (ref, item->self_url, FALSE, &tmp_dump);
	  if (count2)
	    {
	      List_merge (*dump, tmp_dump);
	      count += count2;
	    }
	}

      TtaFreeMemory (first);
      TtaFreeMemory (uri_str);
      if (!strcmp (rest, RDF_NIL))
	{
	  TtaFreeMemory (rest);
	  uri_str = NULL;
	  break;
	}
      else
	uri_str = rest;
      Model_getCollectionItem (ref, uri_str, &first, &rest);
    }

  if (uri_str)
    TtaFreeMemory (uri_str);

  return (count);
}

/*----------------------------------------------------------------------
  BM_getItem
  Returns a bookmark structure filled up with data from the model.
  The caller has to free the returned bookmark.
  ----------------------------------------------------------------------*/
BookmarkP BM_getItem (int ref, char *url, ThotBool isTopic)
{
  librdf_stream *stream;
  librdf_statement *partial_statement;

  librdf_node *subject = NULL;
  librdf_node *predicate = NULL;
  librdf_node *object = NULL;
  librdf_uri *rdf_uri;
  librdf_world *world;
  librdf_model *model; 
  librdf_storage *storage;

  char *uri_str;

  BookmarkP me;

  if (!BM_Context_get (ref, &world, &model, &storage))
    return NULL;
 
  /* make a query for all statements related to a given url */
  /* @@ JK: no control yet to see if the RDF type is actually
     bookmark */
  subject = librdf_new_node_from_uri_string (world, (unsigned char *) url);
  partial_statement = librdf_new_statement (world);
  librdf_statement_set_subject (partial_statement, subject);
  librdf_statement_set_predicate (partial_statement, predicate);
  librdf_statement_set_object (partial_statement, object);

  stream = librdf_model_find_statements (model, partial_statement);
  librdf_free_statement (partial_statement);

  me = Bookmark_new ();
  me->bm_type = BME_BOOKMARK;
  me->self_url = TtaStrdup (url);

  while (!librdf_stream_end (stream))
    {
      librdf_statement *statement = librdf_stream_get_object (stream);
      if(!statement) 
	{
	  fprintf(stderr, "librdf_stream_next returned NULL\n");
	  break;
	}

      object = librdf_statement_get_object (statement);
      predicate = librdf_statement_get_predicate (statement);
      rdf_uri = librdf_node_get_uri (predicate);
      uri_str = (char *) librdf_uri_to_string (rdf_uri);

      /* creator */
      if (!strcmp (uri_str, DC1NS_CREATOR))
	me->author =  Model_getNodeAsStringControl (object, TRUE);
      /* created */
      else if (!strcmp (uri_str, ANNOTNS_CREATED))
	me->created = Model_getNodeAsStringControl (object, TRUE);
      /* modified  */
      else if (!strcmp (uri_str, DC1NS_DATE))
	me->modified = Model_getNodeAsStringControl (object, TRUE);
      /* title */
      else if (!strcmp (uri_str, DC1NS_TITLE))
	me->title = Model_getNodeAsStringControl (object, TRUE);
      /* description */
      else if (!strcmp (uri_str, DC1NS_DESCRIPTION))
	me->description = Model_getNodeAsStringControl (object, TRUE);
      /* bookmark / topics distinctions */
      else
	{
	  if (isTopic)
	    {
	      if (!strcmp (uri_str,  BMNS_SUBTOPICOF))
		me->parent_url = Model_getNodeAsStringControl (object, FALSE);
	    }
	  else
	    {
	      /* topic folder */
	      if (!strcmp (uri_str, BMNS_HASTOPIC))
		me->parent_url =  Model_getNodeAsStringControl (object, FALSE);
	      /* bookmarks */
	      else if (!strcmp (uri_str, BMNS_BOOKMARKS))
		me->bookmarks = Model_getNodeAsStringControl (object, FALSE);
	      /* context */
	      else if (!strcmp (uri_str,  ANNOTNS_CONTEXT))
		me->context = Model_getNodeAsStringControl (object, TRUE);
	    }
	}
      free (uri_str);
      librdf_stream_next (stream);
    }

  librdf_free_stream (stream);

  return me;
}

static ThotBool Model_replace_object (librdf_world *world, librdf_model *model,
				      librdf_statement *old_statement, char *new_value,
				      ThotBool isLiteral)
{
  librdf_node *subject;
  librdf_node *predicate;
  librdf_node *tmp_node;

  librdf_statement *new_statement;

  int i;


  if (new_value && *new_value)
    {
      /*
      ** make the new statement
      */
      /* copy the subject and predicate from the old statement */
      tmp_node = librdf_statement_get_subject (old_statement);
      subject = librdf_new_node_from_node (tmp_node);
      tmp_node = librdf_statement_get_predicate (old_statement);
      predicate = librdf_new_node_from_node (tmp_node);
      /* make the new object node */
      if (isLiteral)
	tmp_node =  librdf_new_node_from_literal (world, 
						  (unsigned char *) new_value,
						  NULL,  /* literal XML language */
						  0);    /* non 0 if literal is XML */
      else
	tmp_node = librdf_new_node_from_uri_string (world, (unsigned char *) new_value);
      new_statement = librdf_new_statement (world);
      librdf_statement_set_subject (new_statement, subject);
      librdf_statement_set_predicate (new_statement, predicate);
      librdf_statement_set_object (new_statement, tmp_node);
      
      /* add the new statement */
      librdf_model_add_statement (model, new_statement);
    }
  
  /* remove the old statement */
  i = librdf_model_remove_statement (model, old_statement);  

  return TRUE;
}

/*----------------------------------------------------------------------
  BM_updateItem
  URG! Erases all the statements that we want to update.
  ----------------------------------------------------------------------*/
ThotBool BM_updateItem (Document doc, int ref, BookmarkP me, ThotBool isTopic)
{
  librdf_stream* stream;
  librdf_statement *partial_statement;

  librdf_node *subject = NULL;
  librdf_node *predicate = NULL;
  librdf_node *object = NULL;
  librdf_uri *rdf_uri;
  librdf_world *world;
  librdf_model *model; 
  librdf_storage *storage;

  char *uri_str;
  char *tmp;

  if (!BM_Context_get (ref, &world, &model, &storage))
    return FALSE;

  /* update the modified date parameter */
  if (me->modified)
    TtaFreeMemory (me->modified);
  me->modified = StrdupDate ();

  /* make a query for all statements related to a given url */
  subject = librdf_new_node_from_uri_string (world, (unsigned char *) me->self_url);
  partial_statement = librdf_new_statement (world);
  librdf_statement_set_subject (partial_statement, subject);
  librdf_statement_set_predicate (partial_statement, predicate);
  librdf_statement_set_object (partial_statement, object);

  stream = librdf_model_find_statements (model, partial_statement);
  librdf_free_statement (partial_statement);

  while (!librdf_stream_end (stream))
    {
      librdf_statement *statement = librdf_stream_get_object (stream);
      if(!statement) 
	{
	  fprintf(stderr, "librdf_stream_next returned NULL\n");
	  break;
	}

      predicate = librdf_statement_get_predicate (statement);
      rdf_uri = librdf_node_get_uri (predicate);
      uri_str = (char *) librdf_uri_to_string (rdf_uri);

      /* topic folder */
      /* we remove all the known properties first, then update the model */
      if (!strcmp (uri_str, DC1NS_CREATOR)
	  || !strcmp (uri_str, DC1NS_DATE)
	  || !strcmp (uri_str, DC1NS_TITLE)
	  || !strcmp (uri_str, DC1NS_DESCRIPTION))
	librdf_model_remove_statement (model, statement);
      else
	{
	  /* bookmark / topic items differences */
	  if (isTopic)
	    {
	      /* parent topic */
	      if (!strcmp (uri_str, BMNS_SUBTOPICOF))
		{
		  Model_removeItemIfDisplaced (ref, statement, me);
		  librdf_model_remove_statement (model, statement);
		}
	    }
	  else
	    {
	      /* parent topic */
	      if (!strcmp (uri_str, BMNS_HASTOPIC))
		{
		  Model_removeItemIfDisplaced (ref, statement, me);
		  librdf_model_remove_statement (model, statement);
		}
	      /* bookmarks */
	      else if (!strcmp (uri_str, BMNS_BOOKMARKS))
		librdf_model_remove_statement (model, statement);
	      else if (!strcmp (uri_str,  ANNOTNS_CONTEXT))
		librdf_model_remove_statement (model, statement);
	    }
	}
      free (uri_str);
      librdf_stream_next (stream);
    }
  librdf_free_stream (stream);

  /* add the new info */
  /* creator */
  tmp = (char *)TtaConvertByteToMbs ((unsigned char *)me->author, ISO_8859_1);
  subject = librdf_new_node_from_uri_string (world, (unsigned char *) me->self_url);
  predicate = librdf_new_node_from_uri_string (world, (unsigned char *) DC1NS_CREATOR);
  object =  librdf_new_node_from_literal (world, 
					  (unsigned char *) tmp,
					  NULL,  /* literal XML language */
 					  0);    /* non 0 if literal is XML */
  add_statement (world, model, subject, predicate, object);
  TtaFreeMemory (tmp);

  /* modified  */
  subject = librdf_new_node_from_uri_string (world, (unsigned char *) me->self_url);
  predicate = librdf_new_node_from_uri_string (world, (unsigned char *) DC1NS_DATE);
  object =  librdf_new_node_from_literal (world, 
					  (unsigned char *) me->modified,
					  NULL,  /* literal XML language */
 					  0);    /* non 0 if literal is XML */
  add_statement (world, model, subject, predicate, object);


  /* title */
  tmp = (char *)TtaConvertByteToMbs ((unsigned char *)me->title, ISO_8859_1); 
  subject = librdf_new_node_from_uri_string (world, (unsigned char *) me->self_url);
  predicate = librdf_new_node_from_uri_string (world, (unsigned char *) DC1NS_TITLE);
  object =  librdf_new_node_from_literal (world, 
					  (unsigned char *) tmp,
					  NULL,  /* literal XML language */
 					  0);    /* non 0 if literal is XML */
  add_statement (world, model, subject, predicate, object);
  TtaFreeMemory (tmp);

 /* description */
  tmp = (char *)TtaConvertByteToMbs ((unsigned char *)me->description, ISO_8859_1); 
  subject = librdf_new_node_from_uri_string (world, (unsigned char *) me->self_url);
  predicate = librdf_new_node_from_uri_string (world, (unsigned char *) DC1NS_DESCRIPTION);
  object =  librdf_new_node_from_literal (world, 
					  (unsigned char *) me->description,
					  NULL,  /* literal XML language */
 					  0);    /* non 0 if literal is XML */
  add_statement (world, model, subject, predicate, object);
  TtaFreeMemory (tmp);

  /* bookmark / topic items differences */
  if (isTopic)
    {
      /* parent topic */
      if (me->parent_url && me->parent_url[0] != EOS)
	{
	  subject = librdf_new_node_from_uri_string (world, (unsigned char *) me->self_url);
	  predicate = librdf_new_node_from_uri_string (world, (unsigned char *) BMNS_SUBTOPICOF);
	  object =  librdf_new_node_from_uri_string (world,
						     (unsigned char *) me->parent_url);
	  add_statement (world, model, subject, predicate, object);
	  /* and add it to its new parent collection, reusing the previous
	   collection ID */
	  BM_addItemToCollection (ref, me->parent_url, NULL, me, FALSE);
	}
    }
  else
    {
      /* bookmarks */
      subject = librdf_new_node_from_uri_string (world, (unsigned char *) me->self_url);
      predicate = librdf_new_node_from_uri_string (world, (unsigned char *) BMNS_BOOKMARKS);
      object =  librdf_new_node_from_uri_string (world, (unsigned char *) me->bookmarks);
      add_statement (world, model, subject, predicate, object);
      
      /* parent topics */
      AddBookmarkTopicList (ref, me);
      if (me->context)
	{
	  subject = librdf_new_node_from_uri_string (world, (unsigned char *) me->self_url);
	  predicate = librdf_new_node_from_uri_string (world, (unsigned char *) ANNOTNS_CONTEXT);
	  object =  librdf_new_node_from_literal (world, 
						  (unsigned char *) me->context,
						  NULL,  /* literal XML language */
						  0);    /* non 0 if literal is XML */
	  add_statement (world, model, subject, predicate, object);
	}
    }

  /* save the updated model */
  BM_saveOrModify (ref, doc);

  return TRUE;
}


/*----------------------------------------------------------------------
  BM_deleteItem
  Erases all the statements related to a given URL
  ----------------------------------------------------------------------*/
ThotBool BM_deleteItem (int ref, char *item_url, ThotBool isBlankIdentifier)
{
  librdf_stream* stream;
  librdf_statement *partial_statement;

  librdf_node *subject = NULL;
  librdf_node *predicate = NULL;
  librdf_node *object = NULL;
  librdf_world *world;
  librdf_model *model; 
  librdf_storage *storage;

  if (!BM_Context_get (ref, &world, &model, &storage))
    return FALSE;

  /* make a query for all statements related to a given url */
  if (!item_url || *item_url == EOS)
    return FALSE;

  if (isBlankIdentifier)
    subject = librdf_new_node_from_blank_identifier (world, (unsigned char *) item_url);
  else
    subject = librdf_new_node_from_uri_string (world, (unsigned char *) item_url);
  partial_statement = librdf_new_statement (world);
  librdf_statement_set_subject (partial_statement, subject);
  librdf_statement_set_predicate (partial_statement, predicate);
  librdf_statement_set_object (partial_statement, object);

  stream = librdf_model_find_statements (model, partial_statement);
  librdf_free_statement (partial_statement);

  while (!librdf_stream_end (stream))
    {
      librdf_statement *statement = librdf_stream_get_object (stream);
      if(!statement) 
	{
	  fprintf(stderr, "librdf_stream_next returned NULL\n");
	  break;
	}
      librdf_model_remove_statement (model, statement);
      librdf_stream_next (stream);
    }
  librdf_free_stream (stream);

  return TRUE;
}

/*----------------------------------------------------------------------
  BM_deleteItemList
  Erases all the statements related to a given topic.
  The list is given in reverse order.
  ----------------------------------------------------------------------*/
ThotBool BM_deleteItemList (int ref, char *parent_topic, List *items)
{
  List       *reverse_list = NULL;
  List       *cur;
  BookmarkP   item;
    
  if (!items)
    return FALSE;
  
  /* build a reverse ordered list so that we delete the topic items
     before the topic itself. (this constraint was induced when adding
     support for rdf:collection */

  reverse_list = List_reverse (items);

  cur = reverse_list;
  while (cur)
    {
      item = (BookmarkP) cur->object;
      if (item->bm_type == BME_TOPIC)
	{
	  ThotBool isBlankNode;

	  /* delete all the statements and states in the collection, including
	   separators */
	  /* BM_deleteItemCollectionRec (ref, parent_topic, item->self_url, &isBlankNode); */
	  /* delete the topics entry in its parent collection */
	  BM_deleteItemCollection (ref, item->parent_url, item->self_url, &isBlankNode, TRUE);
	  BM_deleteItem (ref, item->self_url, FALSE);
	}
      else 
	BM_deleteBookmarkItem (ref, item->parent_url, item->self_url, 
			       item->bm_type == BME_SEPARATOR);
      cur = cur->next;
    }

  /* delete our work list */
  List_delAll (&reverse_list, NULL);

  return TRUE;
}

/*----------------------------------------------------------------------
  BM_deleteBookmarkTopic
  Deletes a bookmark from a given topic
  ----------------------------------------------------------------------*/
ThotBool BM_deleteBookmarkTopic (int ref, char *parent_url, char *self_url, 
				 ThotBool isBlankIdentifier)
{
  librdf_statement *partial_statement;

  librdf_node *subject = NULL;
  librdf_node *predicate = NULL;
  librdf_node *object = NULL;
  librdf_world *world;
  librdf_model *model; 
  librdf_storage *storage;

  if (!parent_url || !self_url)
    return FALSE;

  if (!BM_Context_get (ref, &world, &model, &storage))
    return FALSE;

  /* delete the relevant HAS_TOPIC property for this bookmark */
  if (isBlankIdentifier)
    subject = librdf_new_node_from_blank_identifier (world,  (unsigned char *) self_url);
  else
    subject = librdf_new_node_from_uri_string (world, (unsigned char *) self_url);
  predicate = librdf_new_node_from_uri_string (world, (unsigned char *) BMNS_HASTOPIC);
  object = librdf_new_node_from_uri_string (world, (unsigned char *) parent_url);

  partial_statement = librdf_new_statement (world);
  librdf_statement_set_subject (partial_statement, subject);
  librdf_statement_set_predicate (partial_statement, predicate);
  librdf_statement_set_object (partial_statement, object);

  if (librdf_model_contains_statement (model, partial_statement))
    librdf_model_remove_statement (model, partial_statement);

  librdf_free_statement (partial_statement);

  return TRUE;
}

/*----------------------------------------------------------------------
  BM_deleteBookmarkItem
  Erases all the statements related to a given topic
  ----------------------------------------------------------------------*/
ThotBool BM_deleteBookmarkItem (int ref, char *parent_url, char *self_url, 
				ThotBool isBlankIdentifier)
{
  librdf_stream* stream;
  librdf_statement *partial_statement;

  librdf_node *subject = NULL;
  librdf_node *predicate = NULL;
  librdf_node *object = NULL;
  ThotBool can_erase;
  librdf_world *world;
  librdf_model *model; 
  librdf_storage *storage;

  ThotBool isBlankNode;

  if (!parent_url || !self_url)
    return FALSE;

  if (!BM_Context_get (ref, &world, &model, &storage))
    return FALSE;

  /* remove the bookmark from the collection */
  BM_deleteItemCollection (ref, parent_url, self_url, &isBlankNode, TRUE);

  /* delete the relevant HAS_TOPIC property for this bookmark */
  if (isBlankIdentifier)
    subject = librdf_new_node_from_blank_identifier (world,  (unsigned char *) self_url);
  else
    subject = librdf_new_node_from_uri_string (world, (unsigned char *) self_url);
  predicate = librdf_new_node_from_uri_string (world, (unsigned char *) BMNS_HASTOPIC);
  object = librdf_new_node_from_uri_string (world, (unsigned char *) parent_url);

  partial_statement = librdf_new_statement (world);
  librdf_statement_set_subject (partial_statement, subject);
  librdf_statement_set_predicate (partial_statement, predicate);
  librdf_statement_set_object (partial_statement, object);

  if (librdf_model_contains_statement (model, partial_statement))
    librdf_model_remove_statement (model, partial_statement);

  librdf_statement_clear (partial_statement);

  /* check to see if the bookmark item as other HAS_TOPIC 
     properties or if we can delete it now */
  if (isBlankIdentifier)
    subject = librdf_new_node_from_blank_identifier (world,  (unsigned char *) self_url);
  else
    subject = librdf_new_node_from_uri_string (world, (unsigned char *) self_url);
  predicate = librdf_new_node_from_uri_string (world, (unsigned char *) BMNS_HASTOPIC);
  object = NULL;

  librdf_statement_set_subject (partial_statement, subject);
  librdf_statement_set_predicate (partial_statement, predicate);
  librdf_statement_set_object (partial_statement, object);

  stream = librdf_model_find_statements (model, partial_statement);
  
  if (librdf_stream_end (stream))
    can_erase = TRUE;
  else
    can_erase = FALSE;

  librdf_free_stream (stream);
  librdf_free_statement (partial_statement);

  if (can_erase)
    BM_deleteItem (ref, self_url, isBlankIdentifier);

  return TRUE;
}

/*----------------------------------------------------------------------
  BM_containsBookmarks
  Returns true if the RDF file referred to by ref contains bookmarks.
  ----------------------------------------------------------------------*/
ThotBool BM_containsBookmarks (int ref)
{
  librdf_stream* stream;
  librdf_statement *partial_statement;

  librdf_node *subject = NULL;
  librdf_node *predicate = NULL;
  librdf_node *object = NULL;
  librdf_world *world;
  librdf_model *model; 
  librdf_storage *storage;
  
  int count;

  if (!BM_Context_get (ref, &world, &model, &storage))
    return FALSE;

  /* make a query for all statements of rdf type bookmark */

  predicate = librdf_new_node_from_uri_string (world, (unsigned char *) RDF_TYPE);
  object =  librdf_new_node_from_uri_string (world,
					     (unsigned char *) BMNS_TOPIC);
  partial_statement = librdf_new_statement (world);
  librdf_statement_set_subject (partial_statement, subject);
  librdf_statement_set_predicate (partial_statement, predicate);
  librdf_statement_set_object (partial_statement, object);

  stream = librdf_model_find_statements (model, partial_statement);
  librdf_free_statement (partial_statement);
  
  count = 0;
  while (!librdf_stream_end (stream))
    {
      librdf_statement *statement = librdf_stream_get_object (stream);
      if(!statement) 
	{
	  fprintf(stderr, "librdf_stream_next returned NULL\n");
	  break;
	}
      count++;
      librdf_stream_next (stream);
    }
  librdf_free_stream (stream);

  /* fprintf (stderr, "Found %d potential bookmarks\n", count); */
  return (count > 0) ? TRUE : FALSE;
}

/*----------------------------------------------------------------------
  BM_pasteBookmark
  Do we need to check literal for something other than object?
  Returns TRUE if succesful. In this case, new_bookmark_id has a copy
  of the newly assigned identifier and the caller must free it.
  ----------------------------------------------------------------------*/
ThotBool BM_pasteBookmark (int dest_ref, int src_ref, 
			   char *src_bookmark_url, char *dest_parent_url,
			   char **new_bookmark_id)
{

  librdf_world *src_world, *dest_world;
  librdf_model *src_model, *dest_model; 
  librdf_storage *src_storage, *dest_storage;

  librdf_stream *stream = NULL;
  librdf_statement *partial_statement = NULL;
  librdf_node *subject = NULL;
  librdf_node *predicate = NULL;
  librdf_node *object = NULL;

  char bookmarkid[MAX_LENGTH];
  char *btemplate = "#ambookmark%d";
  /* should be something else? */
  char *base_uri = GetLocalBookmarksBaseURI ();

  if (dest_ref == src_ref
      || !src_bookmark_url || src_bookmark_url[0] == EOS
      || !dest_parent_url || dest_parent_url[0] == EOS)
    return FALSE;

  /* get the two contexts */
  if (!BM_Context_get (src_ref, &src_world, &src_model, &src_storage))
    return FALSE;

  if (!BM_Context_get (dest_ref, &dest_world, &dest_model, &dest_storage))
    return FALSE;

  /* get new blank node id */
  if (genid_counter == 0) 
    {
      /* couldn't create a genid */
      return FALSE;
    }
  
  /* @@ don't remember what to do here. I surely need to split the uri we want
     to paste into its components */
  genid_counter = Model_queryID (dest_world, dest_model, base_uri, btemplate, 
				 genid_counter);
  if (genid_counter == 0) 
    {
      /* couldn't create a genid */
      return FALSE;
    }

  sprintf (bookmarkid, "%s", base_uri);
  sprintf (bookmarkid + strlen (base_uri), btemplate, genid_counter++);

  /* get all the statements and copy them */
  subject = librdf_new_node_from_uri_string (src_world, (unsigned char *) src_bookmark_url);
  partial_statement=librdf_new_statement (src_world);
  librdf_statement_set_subject (partial_statement, subject);
  librdf_statement_set_predicate (partial_statement, predicate);
  librdf_statement_set_object (partial_statement, object);
  stream = librdf_model_find_statements (src_model, partial_statement);
  librdf_free_statement (partial_statement);

  /* copy all the statements, except for has parent */
  while (!librdf_stream_end (stream))
    {
      librdf_node *node = NULL;
      char *uri_str;
      
      librdf_statement *statement = librdf_stream_get_object (stream);
      if(!statement) 
	{
	  fprintf(stderr, "librdf_stream_next returned NULL\n");
	  break;
	}
      
      /* get the predicate */
      node = librdf_statement_get_predicate (statement);
      uri_str = Model_getNodeAsString (node);
      /* skip all the hastopic properties. They don't make sense in the target */
      if (!strcmp (uri_str, BMNS_HASTOPIC))
	{
	  TtaFreeMemory (uri_str);
	  librdf_stream_next(stream); /* advance the stream */
	  continue;
	}
      else
	predicate = librdf_new_node_from_uri_string (dest_world, (unsigned char *) uri_str);
      TtaFreeMemory (uri_str);

      /* get the object */
      node = librdf_statement_get_object (statement);
      uri_str = Model_getNodeAsString (node);
      if (librdf_node_get_type (node) == LIBRDF_NODE_TYPE_LITERAL)
	object =  librdf_new_node_from_literal (dest_world,  (unsigned char *) uri_str, NULL, 0);
      else
	object =  librdf_new_node_from_uri_string (dest_world, (unsigned char *) uri_str);
      TtaFreeMemory (uri_str);

      /* set the subject */
      subject = librdf_new_node_from_uri_string (dest_world, (unsigned char *) bookmarkid);

      /* make the statement */
      partial_statement=librdf_new_statement (src_world);
      librdf_statement_set_subject (partial_statement, subject);
      librdf_statement_set_predicate (partial_statement, predicate);
      librdf_statement_set_object (partial_statement, object);

      /* add it */
      add_statement (dest_world, dest_model, subject, predicate, object);
      librdf_stream_next(stream); /* advance the stream */
    }
  librdf_free_stream (stream);

  /* and store it under its new topic */
  subject = librdf_new_node_from_uri_string (dest_world, (unsigned char *) bookmarkid);
  predicate = librdf_new_node_from_uri_string (dest_world, (unsigned char *) BMNS_HASTOPIC);
  object =  librdf_new_node_from_uri_string (dest_world, (unsigned char *) dest_parent_url);

  partial_statement=librdf_new_statement (dest_world);
  librdf_statement_set_subject (partial_statement, subject);
  librdf_statement_set_predicate (partial_statement, predicate);
  librdf_statement_set_object (partial_statement, object);

  add_statement (dest_world, dest_model, subject, predicate, object);

  *new_bookmark_id = TtaStrdup (bookmarkid);

  return TRUE;
}

/*----------------------------------------------------------------------
  BM_pasteBookmarkColl
  Do we need to check literal for something other than object?
  ----------------------------------------------------------------------*/
ThotBool BM_pasteBookmarkColl (int dest_ref, int src_ref, 
			       char *src_bookmark_url, char *dest_parent_url)
{

  librdf_world *src_world, *dest_world;
  librdf_model *src_model, *dest_model; 
  librdf_storage *src_storage, *dest_storage;

  librdf_stream *stream = NULL;
  librdf_statement *partial_statement = NULL;
  librdf_node *subject = NULL;
  librdf_node *predicate = NULL;
  librdf_node *object = NULL;

  char bookmarkid[MAX_LENGTH];
  char *btemplate = "#ambookmark%d";
  /* should be something else? */
  char *base_uri = GetLocalBookmarksBaseURI ();

  if (dest_ref == src_ref
      || !src_bookmark_url || src_bookmark_url[0] == EOS
      || !dest_parent_url || dest_parent_url[0] == EOS)
    return FALSE;

  /* get the two contexts */
  if (!BM_Context_get (src_ref, &src_world, &src_model, &src_storage))
    return FALSE;

  if (!BM_Context_get (dest_ref, &dest_world, &dest_model, &dest_storage))
    return FALSE;

  /* get new blank node id */
  if (genid_counter == 0) 
    {
      /* couldn't create a genid */
      return FALSE;
    }
  
  /* @@ don't remember what to do here. I surely need to split the uri we want
     to paste into its components */
  genid_counter = Model_queryID (dest_world, dest_model, base_uri, btemplate, 
				 genid_counter);
  if (genid_counter == 0) 
    {
      /* couldn't create a genid */
      return FALSE;
    }

  sprintf (bookmarkid, "%s", base_uri);
  sprintf (bookmarkid + strlen (base_uri), btemplate, genid_counter++);

  /* get all the statements and copy them */
  subject = librdf_new_node_from_uri_string (src_world, (unsigned char *) src_bookmark_url);
  partial_statement=librdf_new_statement (src_world);
  librdf_statement_set_subject (partial_statement, subject);
  librdf_statement_set_predicate (partial_statement, predicate);
  librdf_statement_set_object (partial_statement, object);
  stream = librdf_model_find_statements (src_model, partial_statement);
  librdf_free_statement (partial_statement);

  /* copy all the statements, except for has parent */
  while (!librdf_stream_end (stream))
    {
      librdf_node *node = NULL;
      char *uri_str;
      
      librdf_statement *statement = librdf_stream_get_object (stream);
      if(!statement) 
	{
	  fprintf(stderr, "librdf_stream_next returned NULL\n");
	  break;
	}
      
      /* get the predicate */
      node = librdf_statement_get_predicate (statement);
      uri_str = Model_getNodeAsString (node);
      /* skip all the hastopic properties. They don't make sense in the target */
      if (!strcmp (uri_str, BMNS_HASTOPIC))
	{
	  TtaFreeMemory (uri_str);
	  librdf_stream_next(stream); /* advance the stream */
	  continue;
	}
      else
	predicate = librdf_new_node_from_uri_string (dest_world, (unsigned char *) uri_str);
      TtaFreeMemory (uri_str);

      /* get the object */
      node = librdf_statement_get_object (statement);
      uri_str = Model_getNodeAsString (node);
      if (librdf_node_get_type (node) == LIBRDF_NODE_TYPE_LITERAL)
	object =  librdf_new_node_from_literal (dest_world,  (unsigned char *) uri_str, NULL, 0);
      else
	object =  librdf_new_node_from_uri_string (dest_world, (unsigned char *) uri_str);
      TtaFreeMemory (uri_str);

      /* set the subject */
      subject = librdf_new_node_from_uri_string (dest_world, (unsigned char *) bookmarkid);

      /* make the statement */
      partial_statement=librdf_new_statement (src_world);
      librdf_statement_set_subject (partial_statement, subject);
      librdf_statement_set_predicate (partial_statement, predicate);
      librdf_statement_set_object (partial_statement, object);

      /* add it */
      add_statement (dest_world, dest_model, subject, predicate, object);
      librdf_stream_next(stream); /* advance the stream */
    }
  librdf_free_stream (stream);

  /* and store it under its new topic */
  subject = librdf_new_node_from_uri_string (dest_world, (unsigned char *) bookmarkid);
  predicate = librdf_new_node_from_uri_string (dest_world, (unsigned char *) BMNS_HASTOPIC);
  object =  librdf_new_node_from_uri_string (dest_world, (unsigned char *) dest_parent_url);

  partial_statement=librdf_new_statement (dest_world);
  librdf_statement_set_subject (partial_statement, subject);
  librdf_statement_set_predicate (partial_statement, predicate);
  librdf_statement_set_object (partial_statement, object);

  add_statement (dest_world, dest_model, subject, predicate, object);

  /* if item already existed in collection, delete it */
  /* add item to the new collection */
  /* @@ could use value of recall property here */
  Model_addItemToCollection (dest_world, dest_model, dest_parent_url, src_bookmark_url);

  return TRUE;
}

/*----------------------------------------------------------------------
  BM_addTopicToBookmark
  Adds a new hasTopic property to a bookmark
  ----------------------------------------------------------------------*/
ThotBool BM_addTopicToBookmark (int ref, char *bookmark_url, 
				char *new_topic_url)
{
  librdf_world *world;
  librdf_model *model;
  librdf_storage *storage;

  librdf_statement *partial_statement = NULL;
  librdf_node *subject = NULL;
  librdf_node *predicate = NULL;
  librdf_node *object = NULL;

  ThotBool result;

  if (!bookmark_url || bookmark_url[0] == EOS
      || !new_topic_url || new_topic_url[0] == EOS)
    return FALSE;

  if (!BM_Context_get (ref, &world, &model, &storage))
     return FALSE;

  /* does the bookmarkd already belongs to this topic? */
  subject = librdf_new_node_from_uri_string (world, (unsigned char *) bookmark_url);
  predicate = librdf_new_node_from_uri_string (world, (unsigned char *) BMNS_HASTOPIC);
  object = librdf_new_node_from_uri_string (world,
					    (unsigned char *) new_topic_url);
  partial_statement = librdf_new_statement (world);
  librdf_statement_set_subject (partial_statement, subject);
  librdf_statement_set_predicate (partial_statement, predicate);
  librdf_statement_set_object (partial_statement, object);

  if (!librdf_model_contains_statement (model, partial_statement))
    {
      /* add it */
      librdf_model_add_statement (model, partial_statement);
    }
  result = TRUE; /* we always return TRUE, whether it existed before or
		    not */
  librdf_free_statement (partial_statement);

  return result;
}

/*----------------------------------------------------------------------
  BM_replaceTopicParent
  Replaces the subTopicOf property by a new one.
  Only does such if the new topic is not a child of the current topic
  Returns true if the items were exchanged
  ----------------------------------------------------------------------*/
ThotBool BM_replaceTopicParent (int ref, char *topic_url, 
				char *new_topic_parent_url)
{
  librdf_world *world;
  librdf_model *model;
  librdf_storage *storage;

  librdf_statement *partial_statement = NULL;
  librdf_node *subject = NULL;
  librdf_node *predicate = NULL;
  librdf_node *object = NULL;

  char *object_url;

  ThotBool result;
  
  if (!topic_url || topic_url[0] == EOS
      || !new_topic_parent_url || new_topic_parent_url[0] == EOS)
    return FALSE;

  if (!BM_Context_get (ref, &world, &model, &storage))
     return FALSE;

  /* is the new topic a child of the current topic? */
  subject = librdf_new_node_from_uri_string (world, (unsigned char *) new_topic_parent_url);
  predicate = librdf_new_node_from_uri_string (world, (unsigned char *) BMNS_SUBTOPICOF);
  object = librdf_model_get_target (model, subject, predicate);
  result = FALSE;
  while (!result && object)
    {
      object_url = Model_getNodeAsString (object);
      if (!strcmp (topic_url, object_url))
	{
	  TtaFreeMemory (object_url);
	  result = TRUE;
	  break;
	}
      librdf_free_node (subject);
      TtaFreeMemory (object_url);
      subject = object;
      object = librdf_model_get_target (model, subject, predicate);
    }
  librdf_free_node (subject);
  librdf_free_node (predicate);
  if (object)
    librdf_free_node (object);

  if (!result)
    {
      /* replace the subTopicOf property */
      subject = librdf_new_node_from_uri_string (world, (unsigned char *) topic_url);
      predicate = librdf_new_node_from_uri_string (world, (unsigned char *) BMNS_SUBTOPICOF);
      object = librdf_model_get_target (model, subject, predicate);
      partial_statement = librdf_new_statement (world);
      librdf_statement_set_subject (partial_statement, subject);
      librdf_statement_set_predicate (partial_statement, predicate);
      librdf_statement_set_object (partial_statement, object);
      result = Model_replace_object (world, model, partial_statement, 
				     new_topic_parent_url, FALSE);
      /* librdf_free_statement (partial_statement); */
    }
  else
    result = FALSE;
  
  return result;
}


/*-----------------------------------------------------------
  BM_getDocumentFromRef
  ------------------------------------------------------------*/
Document BM_getDocumentFromRef (int ref)
{
  int doc;
  int doc_ref;
  ThotBool found;

  for (doc = 1, found = FALSE; doc <DocumentTableLength; doc ++)
    {
      if (DocumentTypes[doc] == docBookmark)
	{
	  char *normalized_url;
	  if (!IsHTTPPath (DocumentURLs[doc])
	      && !IsW3Path (DocumentURLs[doc]))
	    normalized_url = FixFileURL (DocumentURLs[doc]);
	  else
	    normalized_url = DocumentURLs[doc];

	  if (BM_Context_reference (normalized_url, &doc_ref)
	      && doc_ref == ref)
	    found = TRUE;
	  if (normalized_url != DocumentURLs[doc])
	    TtaFreeMemory (normalized_url);
	  if (found)
	    break;
	}
    }

  return (found) ? doc : 0; 
}

/*-----------------------------------------------------------
  Model_printStatements
  Prints all the statements related to subject_url
  ------------------------------------------------------------*/
Document BM_printStatements (int ref, char *subject_url)
{
  librdf_stream* stream;
  librdf_statement *partial_statement;

  librdf_node *subject = NULL;
  librdf_node *predicate = NULL;
  librdf_node *object = NULL;
  librdf_world *world;
  librdf_model *model; 
  librdf_storage *storage;
  
  int count = 0;

  if (!BM_Context_get (ref, &world, &model, &storage))
    return FALSE;

  /* make a query for all statements of rdf type bookmark */
  subject = librdf_new_node_from_blank_identifier  (world, (unsigned char *) subject_url);
  predicate = librdf_new_node_from_uri_string (world, (unsigned char *) RDF_TYPE);
  object =  NULL;
  partial_statement = librdf_new_statement (world);
  librdf_statement_set_subject (partial_statement, subject);
  librdf_statement_set_predicate (partial_statement, predicate);
  librdf_statement_set_object (partial_statement, object);

  stream = librdf_model_find_statements (model, partial_statement);
  librdf_free_statement (partial_statement);
  
  while (!librdf_stream_end (stream))
    {
      librdf_statement *statement = librdf_stream_get_object (stream);
      if(!statement) 
	{
	  fprintf(stderr, "librdf_stream_next returned NULL\n");
	  break;
	}
      count++;
      librdf_stream_next (stream);
    }
  librdf_free_stream (stream);

  /* fprintf (stderr, "Found %d potential bookmarks\n", count); */
  return (count > 0) ? TRUE : FALSE;
}

/*-----------------------------------------------------------
  Model_queryState
  ------------------------------------------------------------*/
ThotBool Model_queryState (int ref,
			   char *subject_url, char *state_url)
{
  librdf_world *world;
  librdf_model *model; 
  librdf_storage *storage;
  librdf_node *subject = NULL;
  librdf_node *predicate = NULL;
  librdf_node *object = NULL;
  ThotBool res;
  librdf_statement *partial_statement;

  if (!BM_Context_get (ref, &world, &model, &storage)
      || !subject_url || !state_url)
    return FALSE;

  subject = librdf_new_node_from_uri_string (world, (unsigned char *) subject_url);
  predicate = librdf_new_node_from_uri_string (world, (unsigned char *) BMNS_STATE);
  object = librdf_new_node_from_uri_string (world, (unsigned char *) state_url);

  partial_statement = librdf_new_statement (world);

  librdf_statement_set_subject (partial_statement, subject);
  librdf_statement_set_predicate (partial_statement, predicate);
  librdf_statement_set_object (partial_statement, object);

  res = (librdf_model_contains_statement (model, partial_statement) != 0);

  librdf_free_statement (partial_statement);

  return (res);
}

/*-----------------------------------------------------------
  ThotBool Model_changeState
  ------------------------------------------------------------*/
ThotBool Model_changeState (int ref, Document doc,
			    char *subject_url, char *previous_state_url,
			    char *new_state_url)
{
  librdf_world *world;
  librdf_model *model; 
  librdf_storage *storage;
  librdf_node *subject = NULL;
  librdf_node *predicate = NULL;
  librdf_node *object = NULL;
  librdf_statement *partial_statement;

  if (!BM_Context_get (ref, &world, &model, &storage)
      || !subject_url)
    return FALSE;

  subject = librdf_new_node_from_uri_string (world, (unsigned char *) subject_url);
  predicate = librdf_new_node_from_uri_string (world, (unsigned char *) BMNS_STATE);
  object = librdf_new_node_from_uri_string (world, (unsigned char *) previous_state_url);

  partial_statement = librdf_new_statement (world);

  librdf_statement_set_subject (partial_statement, subject);
  librdf_statement_set_predicate (partial_statement, predicate);
  librdf_statement_set_object (partial_statement, object);

  if (librdf_model_contains_statement (model, partial_statement))
    librdf_model_remove_statement (model, partial_statement);

  librdf_statement_clear (partial_statement);

  subject = librdf_new_node_from_uri_string (world, (unsigned char *) subject_url);
  predicate = librdf_new_node_from_uri_string (world, (unsigned char *) BMNS_STATE);
  object = librdf_new_node_from_uri_string (world, (unsigned char *) new_state_url);

  librdf_statement_set_subject (partial_statement, subject);
  librdf_statement_set_predicate (partial_statement, predicate);
  librdf_statement_set_object (partial_statement, object);

  if (!librdf_model_contains_statement (model, partial_statement))
    librdf_model_add_statement (model, partial_statement);

  librdf_free_statement (partial_statement);

  /* save the model */
  BM_saveOrModify (ref, doc);
  
  return TRUE;
}



