/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 2003.
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
  BM_Context_dumpasList
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

      /* register the rdfxml serializer */
      librdf_serializer_rdfxml_constructor (world);
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
  model = librdf_new_model(world, 
			   storage, /* storage to use */
			   NULL);   /* options to intialize model */

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
static void parse_file (librdf_world *world, librdf_model *model,
			char *filename, char *base, int ntriples)
{
  librdf_parser* parser;
  librdf_stream* stream;
  librdf_uri *uri;
  librdf_uri *base_uri;
  int count;

  uri = librdf_new_uri (world, filename);
  base_uri = librdf_new_uri (world, base);

  if (ntriples)
    /* parse the file as triplets */
    parser = librdf_new_parser (world, "ntriples", NULL, NULL);
  else
    /* parse the file as rdf */
    parser = librdf_new_parser (world, "raptor", NULL, NULL);
  
  librdf_parser_set_feature(parser, LIBRDF_MS_aboutEach_URI, "yes");
  librdf_parser_set_feature(parser, LIBRDF_MS_aboutEachPrefix_URI, "yes");

  stream = librdf_parser_parse_as_stream (parser, uri, base_uri);

  if (!stream)
    {
      fprintf(stderr, "Failed to parse RDF as stream\n");
      librdf_free_parser(parser);
      return;
    }

  count = 0;
  while(!librdf_stream_end (stream)) {
    librdf_statement *statement=librdf_stream_get_object (stream);
    if(!statement) {
      fprintf(stderr, "librdf_stream_next returned NULL\n");
      break;
    }
    
    librdf_model_add_statement (model, statement);
    /* cannot free them anymore */
    /* librdf_free_statement (statement); */
    librdf_stream_next(stream); /* advance the stream */
    count++;
  }
  librdf_free_stream (stream);  
  librdf_free_parser (parser);
  librdf_free_uri (uri);
  librdf_free_uri (base_uri); 
#ifdef BMFILE_DEBUG
  fprintf(stderr, "Added %d statements\n", count);
#endif /* BMFILE_DEBUG */
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

      test = fopen (filename, "w");
      if (test)
	{
	  if (librdf_serializer_serialize_model (serializer, test, NULL, model) != 0)
	    result = FALSE;
	  fclose (test);
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

      subject = librdf_new_node_from_uri_string (world, genid);

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

  subject = librdf_new_node_from_uri_string (world, topicid);
  predicate = librdf_new_node_from_uri_string (world, RDF_TYPE);
  object =  librdf_new_node_from_uri_string (world,
					     BMNS_TOPIC);

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

  subject = librdf_new_node_from_uri_string (world, subject_str);
  predicate = librdf_new_node_from_uri_string (world, predicate_str);
  if (object_str)
    {
      if (librdf_heuristic_object_is_literal (object_str))
	object = librdf_new_node_from_literal (world, object_str, NULL, 0);
      else
	object = librdf_new_node_from_uri_string (world, object_str);
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

  subject = librdf_new_node_from_uri_string (world, subject_str);
  predicate = librdf_new_node_from_uri_string (world, predicate_str);
  if (object_str)
    {
      if (librdf_heuristic_object_is_literal (object_str))
	object = librdf_new_node_from_literal (world, object_str, NULL, 0);
      else
	object = librdf_new_node_from_uri_string (world, object_str);
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


/*
** Public API
*/

/*----------------------------------------------------------------------
  BM_save
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
  BM_parse
  Parses a bookmark file into the model.
  ----------------------------------------------------------------------*/
void BM_parse (int ref, char *filename, char *base)
{
  if (filename && *filename)
    {
      char *file_uri, *base_uri;
      librdf_world *world;
      librdf_model *model; 
      librdf_storage *storage;
      
      if (!BM_Context_get (ref, &world, &model, &storage))
	return;

      /* redland expects a file URI */
      if (!IsFilePath (filename))
	file_uri =  ANNOT_MakeFileURL ((const char *) filename);
      else
	file_uri = filename;
      if (!IsHTTPPath (base) && !IsFilePath (base))
	base_uri = ANNOT_MakeFileURL ((const char *) base);
      else
	base_uri = base;
      parse_file (world, model, file_uri, base_uri, FALSE);
      if (filename != file_uri)
	TtaFreeMemory (file_uri);
      if (base != base_uri)
	TtaFreeMemory (base_uri);
    }
}

/*----------------------------------------------------------------------
  AddBookmarkTopicList
  ----------------------------------------------------------------------*/
static void AddBookmarkTopicList (librdf_world *world, librdf_model *model, 
				  char *url, List *list)
{
  List *cur = list;
  char *parent_url;
  librdf_statement *statement;
  librdf_node *subject, *object, *predicate;

  while (cur) 
    {
      subject = librdf_new_node_from_uri_string (world, url);
      statement = librdf_new_statement (world);
      librdf_statement_set_subject (statement, subject);
      predicate = librdf_new_node_from_uri_string (world, BMNS_HASTOPIC);
      librdf_statement_set_predicate (statement, predicate);
      parent_url = (char *) cur->object;
      object = librdf_new_node_from_uri_string (world, parent_url);
      librdf_statement_set_object (statement, object);
      librdf_model_add_statement (model, statement);
      librdf_free_statement (statement);
      cur = cur->next;
    }  
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
  sprintf (bookmarkid, "%s", base_uri);
  sprintf (bookmarkid + strlen (base_uri), template_, genid_counter++);

  /* bookmark type */
  subject = librdf_new_node_from_uri_string (world, bookmarkid);
  predicate = librdf_new_node_from_uri_string (world, RDF_TYPE);
  object =  librdf_new_node_from_uri_string (world,
					     BMNS_BOOKMARK);
  add_statement (world, model, subject, predicate, object);

  /* topic folders */
  if (me->parent_url_list)
      AddBookmarkTopicList (world, model, bookmarkid, me->parent_url_list);

  /* creator */
  tmp = (char *)TtaConvertByteToMbs ((unsigned char *)me->author, ISO_8859_1);
  subject = librdf_new_node_from_uri_string (world, bookmarkid);
  predicate = librdf_new_node_from_uri_string (world, DC1NS_CREATOR);
  object =  librdf_new_node_from_literal (world, 
					  tmp,  /* literal string value */
					  NULL,  /* literal XML language */
 					  0);    /* non 0 if literal is XML */
  add_statement (world, model, subject, predicate, object);
  TtaFreeMemory (tmp);

  /* created */
  subject = librdf_new_node_from_uri_string (world, bookmarkid);
  predicate = librdf_new_node_from_uri_string (world, ANNOTNS_CREATED);
  object =  librdf_new_node_from_literal (world, 
					  me->created,  /* literal string value */
					  NULL,  /* literal XML language */
 					  0);    /* non 0 if literal is XML */
  add_statement (world, model, subject, predicate, object);


  /* modified  */
  subject = librdf_new_node_from_uri_string (world, bookmarkid);
  predicate = librdf_new_node_from_uri_string (world, DC1NS_DATE);
  object =  librdf_new_node_from_literal (world, 
					  me->modified,  /* literal string value */
					  NULL,  /* literal XML language */
 					  0);    /* non 0 if literal is XML */
  add_statement (world, model, subject, predicate, object);


  /* title */
  tmp = (char *)TtaConvertByteToMbs ((unsigned char *)me->title, ISO_8859_1); 
  subject = librdf_new_node_from_uri_string (world, bookmarkid);
  predicate = librdf_new_node_from_uri_string (world, DC1NS_TITLE);
  object =  librdf_new_node_from_literal (world, 
					  tmp,   /* literal string value */
					  NULL,  /* literal XML language */
 					  0);    /* non 0 if literal is XML */
  add_statement (world, model, subject, predicate, object);
  TtaFreeMemory (tmp);

 /* description */
  tmp = (char *)TtaConvertByteToMbs ((unsigned char *)me->description, ISO_8859_1); 
  subject = librdf_new_node_from_uri_string (world, bookmarkid);
  predicate = librdf_new_node_from_uri_string (world, DC1NS_DESCRIPTION);
  object =  librdf_new_node_from_literal (world, 
					  tmp,   /* literal string value */
					  NULL,  /* literal XML language */
 					  0);    /* non 0 if literal is XML */
  add_statement (world, model, subject, predicate, object);
  TtaFreeMemory (tmp);

  /* bookmarks */
  subject = librdf_new_node_from_uri_string (world, bookmarkid);
  predicate = librdf_new_node_from_uri_string (world, BMNS_BOOKMARKS);
  object =  librdf_new_node_from_uri_string (world, me->bookmarks);
  add_statement (world, model, subject, predicate, object);

  /* context */
  if (me->context)
    {
      subject = librdf_new_node_from_uri_string (world, bookmarkid);
      predicate = librdf_new_node_from_uri_string (world, ANNOTNS_CONTEXT);
      object =  librdf_new_node_from_literal (world, 
					      me->context,  /* literal string value */
					      NULL,  /* literal XML language */
					      0);    /* non 0 if literal is XML */
      add_statement (world, model, subject, predicate, object);
    }
  /* save the model */
  if (ref == 0)
    BM_save (ref, GetLocalBookmarksFile ());
  else
    {
      ThotBool i;
      /* save the modified model in the temporary file */
      if (BM_tmpsave (ref))
	TtaSetDocumentModified (doc);
      i = TtaIsDocumentModified (doc);
    }

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
      
      sprintf (topicid, "%s", base_uri);
      sprintf (topicid + strlen (base_uri), template_, genid_counter++);
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
  subject = librdf_new_node_from_uri_string (world, topicid);
  predicate = librdf_new_node_from_uri_string (world, RDF_TYPE);
  object =  librdf_new_node_from_uri_string (world,
					     BMNS_TOPIC);
  add_statement (world, model, subject, predicate, object);

  /* parent topic */
  if (me->parent_url && me->parent_url[0] != EOS)
    {
      subject = librdf_new_node_from_uri_string (world, topicid);
      predicate = librdf_new_node_from_uri_string (world, BMNS_SUBTOPICOF);
      object =  librdf_new_node_from_uri_string (world,
						 me->parent_url);
      add_statement (world, model, subject, predicate, object);
    }

  /* creator */
  tmp = (char *)TtaConvertByteToMbs ((unsigned char *)me->author, ISO_8859_1); 
  subject = librdf_new_node_from_uri_string (world, topicid);
  predicate = librdf_new_node_from_uri_string (world, DC1NS_CREATOR);
  object =  librdf_new_node_from_literal (world, 
					  tmp,  /* literal string value */
					  NULL,  /* literal XML language */
 					  0);    /* non 0 if literal is XML */
  add_statement (world, model, subject, predicate, object);
  TtaFreeMemory (tmp);

  /* created */
  subject = librdf_new_node_from_uri_string (world, topicid);
  predicate = librdf_new_node_from_uri_string (world, ANNOTNS_CREATED);
  object =  librdf_new_node_from_literal (world, 
					  me->created,  /* literal string value */
					  NULL,  /* literal XML language */
 					  0);    /* non 0 if literal is XML */
  add_statement (world, model, subject, predicate, object);


  /* modified  */
  subject = librdf_new_node_from_uri_string (world, topicid);
  predicate = librdf_new_node_from_uri_string (world, DC1NS_DATE);
  object =  librdf_new_node_from_literal (world, 
					  me->modified,  /* literal string value */
					  NULL,  /* literal XML language */
 					  0);    /* non 0 if literal is XML */
  add_statement (world, model, subject, predicate, object);


  /* title */
  tmp = (char *)TtaConvertByteToMbs ((unsigned char *)me->title, ISO_8859_1); 
  subject = librdf_new_node_from_uri_string (world, topicid);
  predicate = librdf_new_node_from_uri_string (world, DC1NS_TITLE);
  object =  librdf_new_node_from_literal (world, 
					  tmp,
					  NULL,  /* literal XML language */
 					  0);    /* non 0 if literal is XML */
  add_statement (world, model, subject, predicate, object);
  TtaFreeMemory (tmp);

 /* description */
  tmp = (char *)TtaConvertByteToMbs ((unsigned char *)me->description, ISO_8859_1); 
  subject = librdf_new_node_from_uri_string (world, topicid);
  predicate = librdf_new_node_from_uri_string (world, DC1NS_DESCRIPTION);
  object =  librdf_new_node_from_literal (world, 
					  tmp,
					  NULL,  /* literal XML language */
 					  0);    /* non 0 if literal is XML */
  add_statement (world, model, subject, predicate, object);
  TtaFreeMemory (tmp);

  /* save the model */
  if (ref == 0)
    BM_save (ref, GetLocalBookmarksFile ());
  else
    {
      ThotBool i;
      /* save the modified model in the temporary file */
      BM_tmpsave (ref);
      TtaSetDocumentModified (doc);
      i = TtaIsDocumentModified (doc);
    }

  return (TRUE);
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
    object_str = strdup (librdf_node_get_literal_value (node));
  else
    {
      uri = librdf_node_get_uri (node);
      object_str = librdf_uri_to_string (uri);
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
    object_str = strdup (librdf_node_get_literal_value (node));
  else
    {
      uri = librdf_node_get_uri (node);
      object_str = librdf_uri_to_string (uri);
    }
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

  predicate = librdf_new_node_from_uri_string (world, predicate_url);
  iterator = librdf_model_get_targets (model, subject, predicate);
  librdf_free_node (predicate);
  if (librdf_iterator_have_elements (iterator))
    {
      librdf_node_type type;
      
      node = (librdf_node *) librdf_iterator_get_object (iterator);
      type = librdf_node_get_type (node);
      /* why is this uri being parsed as a literal? */
      if (type == LIBRDF_NODE_TYPE_LITERAL)
	object_str = strdup (librdf_node_get_literal_value (node));
      else
	{
	  uri = librdf_node_get_uri (node);
	  object_str = librdf_uri_to_string (uri);
	}
    }
  else
    object_str = NULL;
  librdf_free_iterator (iterator);

  return (object_str);
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
      if (item->self_url)
	{
	  librdf_node *node;

	  node = librdf_new_node_from_uri_string (world, item->bookmarks);
	  item->title = Model_getObjectAsString (world, model, node, 
						 DC1NS_TITLE);
	  item->nickname = Model_getObjectAsString (world, model, node, 
						    BMNS_NICKNAME);
	  librdf_free_node (node);
	}
    }
  else
    {
      if (item->bm_type == BME_TOPIC)
	item->parent_url = Model_getObjectAsString (world, model, subject, 
						    BMNS_SUBTOPICOF);
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
  predicate = librdf_new_node_from_uri_string (world, RDF_TYPE);
  if (bm_type == BME_TOPIC)
    object =  librdf_new_node_from_uri_string (world,
					       BMNS_TOPIC);
  else
    object =  librdf_new_node_from_uri_string (world,
					       BMNS_BOOKMARK);

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

      uri_str = librdf_uri_to_string (uri);
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
  predicate = librdf_new_node_from_uri_string (world, BMNS_SUBTOPICOF);
  object =  librdf_new_node_from_uri_string (world, parent_topic_url);

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

    subject = librdf_new_node_from_uri_string (world, parent_topic_url);
    predicate = librdf_new_node_from_uri_string (world, RDF_TYPE);
    object =  librdf_new_node_from_uri_string (world,
					     BMNS_TOPIC);
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
  Returns TRUE if at least a matching statement was found.
  ----------------------------------------------------------------------*/
ThotBool Model_dumpSeeAlso (int ref, char *resource_url, char *topic_url, List **dump)
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

  int count;

  if (!BM_Context_get (ref, &world, &model, &storage))
    return FALSE;

  if (resource_url == NULL)
    return FALSE;

  /* get all the topics that are a child of the parent_topic */
  subject = librdf_new_node_from_uri_string (world, resource_url);
  /* subject = NULL; */
  predicate = librdf_new_node_from_uri_string (world, RDFS_SEEALSO);
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
      /* verify if it's a topic */
      uri_str = Model_getNodeAsStringControl (object, FALSE);
      if (uri_str)
	{
	  item = Bookmark_new ();
	  List_add (dump, (void *) item);
	  item->parent_url = TtaStrdup (topic_url);
	  item->self_url = TtaStrdup (resource_url);
	  item->bookmarks = TtaStrdup (uri_str);
	  item->bm_type = BME_SEEALSO;
	  Model_getItemInfo (ref, subject, item);
	  free (uri_str);
	  count++;
	}
      librdf_stream_next (stream);
    }
  librdf_free_stream (stream);

  return (count > 0);
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
      dump = NULL;
      if (Model_dumpSeeAlso (ref, me->self_url, me->parent_url, &dump))
	{
	  /* sort dump and insert it into the list */
	  cur->next = dump;
	  List_merge (dump, next);
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
      predicate = librdf_new_node_from_uri_string (world, BMNS_HASTOPIC);
      object =  librdf_new_node_from_uri_string (world, topic->self_url);
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

  char *uri_str;

  if (!me)
    return FALSE;

  if (!BM_Context_get (ref, &world, &model, &storage))
    return FALSE;

  /* we'll reuse the same statement */
  partial_statement = librdf_new_statement (world);

  /* get all the bookmarks that are a stored in the topic */
  subject = librdf_new_node_from_uri_string (world, me->self_url);
  predicate = librdf_new_node_from_uri_string (world, BMNS_HASTOPIC);
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
				ThotBool sort, List **dump)
{
  List *topic_list = NULL;
  List *bookmark_list = NULL;
  List *cur;
  BookmarkP bookmark;

  /* first we get all the topics */
  Model_dumpTopicChild  (ref, parent_topic_url, &topic_list);
  /* then all the bookmarks contained in each topic */
  Model_dumpTopicBookmarks (ref, topic_list, &bookmark_list);
  if (bookmark_list)
    {
      bookmark_list = BM_expandBookmarks (&bookmark_list);
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

  /* add the seeAlso's */
  Model_dumpSeeAlsoAsList (ref, dump);

  return (TRUE);
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
  subject = librdf_new_node_from_uri_string (world, url);
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
      uri_str = librdf_uri_to_string (rdf_uri);

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

static int Model_replace_object (librdf_world *world, librdf_model *model,
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
						  new_value,
						  NULL,  /* literal XML language */
						  0);    /* non 0 if literal is XML */
      else
	tmp_node = librdf_new_node_from_uri_string (world, new_value);
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
  subject = librdf_new_node_from_uri_string (world, me->self_url);
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
      uri_str = librdf_uri_to_string (rdf_uri);

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
		librdf_model_remove_statement (model, statement);
	    }
	  else
	    {
	      /* parent topic */
	      if (!strcmp (uri_str, BMNS_HASTOPIC))
		librdf_model_remove_statement (model, statement);
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
  subject = librdf_new_node_from_uri_string (world, me->self_url);
  predicate = librdf_new_node_from_uri_string (world, DC1NS_CREATOR);
  object =  librdf_new_node_from_literal (world, 
					  tmp,  /* literal string value */
					  NULL,  /* literal XML language */
 					  0);    /* non 0 if literal is XML */
  add_statement (world, model, subject, predicate, object);
  TtaFreeMemory (tmp);

  /* modified  */
  subject = librdf_new_node_from_uri_string (world, me->self_url);
  predicate = librdf_new_node_from_uri_string (world, DC1NS_DATE);
  object =  librdf_new_node_from_literal (world, 
					  me->modified,  /* literal string value */
					  NULL,  /* literal XML language */
 					  0);    /* non 0 if literal is XML */
  add_statement (world, model, subject, predicate, object);


  /* title */
  tmp = (char *)TtaConvertByteToMbs ((unsigned char *)me->title, ISO_8859_1); 
  subject = librdf_new_node_from_uri_string (world, me->self_url);
  predicate = librdf_new_node_from_uri_string (world, DC1NS_TITLE);
  object =  librdf_new_node_from_literal (world, 
					  tmp,  /* literal string value */
					  NULL,  /* literal XML language */
 					  0);    /* non 0 if literal is XML */
  add_statement (world, model, subject, predicate, object);
  TtaFreeMemory (tmp);

 /* description */
  tmp = (char *)TtaConvertByteToMbs ((unsigned char *)me->description, ISO_8859_1); 
  subject = librdf_new_node_from_uri_string (world, me->self_url);
  predicate = librdf_new_node_from_uri_string (world, DC1NS_DESCRIPTION);
  object =  librdf_new_node_from_literal (world, 
					  me->description,  /* literal string value */
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
	  subject = librdf_new_node_from_uri_string (world, me->self_url);
	  predicate = librdf_new_node_from_uri_string (world, BMNS_SUBTOPICOF);
	  object =  librdf_new_node_from_uri_string (world,
						     me->parent_url);
	  add_statement (world, model, subject, predicate, object);
	}
    }
  else
    {
      /* bookmarks */
      subject = librdf_new_node_from_uri_string (world, me->self_url);
      predicate = librdf_new_node_from_uri_string (world, BMNS_BOOKMARKS);
      object =  librdf_new_node_from_uri_string (world, me->bookmarks);
      add_statement (world, model, subject, predicate, object);
      
      /* parent topics */
      AddBookmarkTopicList (world, model, me->self_url, me->parent_url_list);
      if (me->context)
	{
	  subject = librdf_new_node_from_uri_string (world, me->self_url);
	  predicate = librdf_new_node_from_uri_string (world, ANNOTNS_CONTEXT);
	  object =  librdf_new_node_from_literal (world, 
						  me->context,  /* literal string value */
						  NULL,  /* literal XML language */
						  0);    /* non 0 if literal is XML */
	  add_statement (world, model, subject, predicate, object);
	}
    }

  /* save the updated model */
  if (ref == 0)
    BM_save (ref, GetLocalBookmarksFile ());
  else
    {
      ThotBool i;

      /* save the modified model in the temporary file */
      BM_tmpsave (ref);
      TtaSetDocumentModified (doc);
      i = TtaIsDocumentModified (doc);
    }
  return TRUE;
}

/*----------------------------------------------------------------------
  BM_deleteItem
  Erases all the statements related to a given URL
  ----------------------------------------------------------------------*/
ThotBool BM_deleteItem (int ref, char *item_url)
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


  subject = librdf_new_node_from_uri_string (world, item_url);
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
  Erases all the statements related to a given topic
  ----------------------------------------------------------------------*/
ThotBool BM_deleteItemList (Document doc, char *parent_topic, List *items)
{
  List       *cur;
  BookmarkP   item;
    
  if (!items)
    return FALSE;
  
  cur = items;

  while (cur)
    {
      item = (BookmarkP) cur->object;
      if (item->bm_type == BME_TOPIC)
	BM_deleteItem (doc, item->self_url);
      else 
	BM_deleteBookmarkItem (doc, parent_topic, item->self_url);
      cur = cur->next;
    }
  return TRUE;
}


/*----------------------------------------------------------------------
  BM_deleteBookmarkItem
  Erases all the statements related to a given topic
  ----------------------------------------------------------------------*/
ThotBool BM_deleteBookmarkItem (int ref, char *parent_url, char *self_url)
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

  if (!parent_url || !self_url)
    return FALSE;

  if (!BM_Context_get (ref, &world, &model, &storage))
    return FALSE;

  /* delete the relevant HAS_TOPIC property for this bookmark */
  subject = librdf_new_node_from_uri_string (world, self_url);
  predicate = librdf_new_node_from_uri_string (world, BMNS_HASTOPIC);
  object = librdf_new_node_from_uri_string (world, parent_url);

  partial_statement = librdf_new_statement (world);
  librdf_statement_set_subject (partial_statement, subject);
  librdf_statement_set_predicate (partial_statement, predicate);
  librdf_statement_set_object (partial_statement, object);

  if (librdf_model_contains_statement (model, partial_statement))
    librdf_model_remove_statement (model, partial_statement);

  librdf_statement_clear (partial_statement);

  /* check to see if the bookmark item as other HAS_TOPIC 
     properties or if we can delete it now */
  subject = librdf_new_node_from_uri_string (world, self_url);
  predicate = librdf_new_node_from_uri_string (world, BMNS_HASTOPIC);
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
    BM_deleteItem (ref, self_url);

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

  predicate = librdf_new_node_from_uri_string (world, RDF_TYPE);
  object =  librdf_new_node_from_uri_string (world,
					     BMNS_BOOKMARK);
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
  ----------------------------------------------------------------------*/
ThotBool BM_pasteBookmark (int dest_ref, int src_ref, 
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
  subject = librdf_new_node_from_uri_string (src_world, src_bookmark_url);
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
	predicate = librdf_new_node_from_uri_string (dest_world, uri_str);
      TtaFreeMemory (uri_str);

      /* get the object */
      node = librdf_statement_get_object (statement);
      uri_str = Model_getNodeAsString (node);
      if (librdf_node_get_type (node) == LIBRDF_NODE_TYPE_LITERAL)
	object =  librdf_new_node_from_literal (dest_world, uri_str, NULL, 0);
      else
	object =  librdf_new_node_from_uri_string (dest_world, uri_str);
      TtaFreeMemory (uri_str);

      /* set the subject */
      subject = librdf_new_node_from_uri_string (dest_world, bookmarkid);

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
  subject = librdf_new_node_from_uri_string (dest_world, bookmarkid);
  predicate = librdf_new_node_from_uri_string (dest_world, BMNS_HASTOPIC);
  object =  librdf_new_node_from_uri_string (dest_world, dest_parent_url);

  partial_statement=librdf_new_statement (dest_world);
  librdf_statement_set_subject (partial_statement, subject);
  librdf_statement_set_predicate (partial_statement, predicate);
  librdf_statement_set_object (partial_statement, object);

  add_statement (dest_world, dest_model, subject, predicate, object);
  
  return TRUE;
}

/*----------------------------------------------------------------------
  BM_addTopicToBookmark
  AddsDo we need to check literal for something other than object?
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
  subject = librdf_new_node_from_uri_string (world, bookmark_url);
  predicate = librdf_new_node_from_uri_string (world, BMNS_HASTOPIC);
  object = librdf_new_node_from_uri_string (world,
					    new_topic_url);
  partial_statement = librdf_new_statement (world);
  librdf_statement_set_subject (partial_statement, subject);
  librdf_statement_set_predicate (partial_statement, predicate);
  librdf_statement_set_object (partial_statement, object);

  if (!librdf_model_contains_statement (model, partial_statement))
    {
      /* add it */
      librdf_model_add_statement (model, partial_statement);
      result = TRUE;
    }
  else
    result = FALSE;
  librdf_free_statement (partial_statement);

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
