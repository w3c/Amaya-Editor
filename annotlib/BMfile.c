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
#include "f/BMfile_f.h"

/* global variables */
static librdf_world *world;
static librdf_storage *storage;
static librdf_model *model;

/*   First step.
** open the bookmark file if it's not open, add something, close it 
*/

/*----------------------------------------------------------------------
  redland_init
  Initializes the redland environment.
  ----------------------------------------------------------------------*/
void redland_init (void)
{
  /* Creates a new Redland execution environment */
  world = librdf_new_world ();

  /* Open an environment */
  librdf_world_open (world);

  /* register the rdfxml serializer */
  librdf_serializer_rdfxml_constructor (world);

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


}

/*----------------------------------------------------------------------
  redland_free
  Frees the redland environment.
  ----------------------------------------------------------------------*/
void redland_free (void)
{
  /*
  ** free the model 
  */
  if (model)
    librdf_free_model(model);
  if (storage)
    librdf_free_storage(storage);
  if (world)
    librdf_free_world(world);
}

/*----------------------------------------------------------------------
  add_statement
  Add a statement to a storage model.
  ----------------------------------------------------------------------*/
static void add_statement (librdf_world  *world, librdf_model *model,
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
static void parse_file (librdf_world *world, char *filename, char *base, int ntriples)
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
      librdf_free_uri(uri);
      /* librdf_free_uri(base_uri); */
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
  /* librdf_free_uri (base_uri); */
  fprintf(stderr, "Added %d statements\n", count);
}

/*----------------------------------------------------------------------
  serialize
  Serializes an RDF model into either triples or rdf/xml.
  ----------------------------------------------------------------------*/
static void serialize (librdf_model *model, char *name, char *filename, char *mime_type)
{
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
    fprintf(stderr, "Failed to create new serializer\n");
  else
    {
      FILE *test;

      test = fopen (filename, "w");
      librdf_serializer_serialize_model (serializer, test, NULL, model);
      fclose (test);
      /* uncomment this to output the model to stdout */
      /* librdf_serializer_serialize_model (serializer, stdout, NULL, model); */
      librdf_free_serializer(serializer);
    }
}

/*----------------------------------------------------------------------
  Model_queryID
  Returns a unique blank node ID that doesn't yet exist in the model.
  ----------------------------------------------------------------------*/
static int Model_queryID (librdf_world *world, librdf_model *model, char * base_url, 
			   char  *template, int start_id)
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
      snprintf (genid + base_url_len, sizeof (genid) - base_url_len, template, igenid);

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
  Writes the bookmark model to filename.
  ----------------------------------------------------------------------*/
void BM_save (char *filename)
{
  if (filename && *filename)
    serialize (model, "rdfxml", filename, "application/rdf+xml");
}

/*----------------------------------------------------------------------
  BM_parse
  Parses a bookmark file into the model.
  ----------------------------------------------------------------------*/
void BM_parse (char *filename, char *base)
{
  if (filename && *filename)
    {
      char *file_uri, *base_uri;

      /* redland expects a file URIs */
      if (!IsFilePath (filename))
	file_uri =  ANNOT_MakeFileURL ((const char *) filename);
      else
	file_uri = filename;
      if (!IsFilePath (base))
	base_uri = ANNOT_MakeFileURL ((const char *) base);
      else
	base_uri = base;
      parse_file (world, file_uri, base_uri, FALSE);
      if (filename != file_uri)
	TtaFreeMemory (file_uri);
      if (base != base_uri)
	TtaFreeMemory (base_uri);
    }
}

/*----------------------------------------------------------------------
  AddBookmarkTopicList
  ----------------------------------------------------------------------*/
static void AddBookmarkTopicList (char *url, List *list)
{
  List  *cur = list;
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
      cur = cur->next;
    }  
}


/*----------------------------------------------------------------------
  BM_addBookmark
  Adds a bookmark to a storage model.
  ----------------------------------------------------------------------*/
ThotBool BM_addBookmark (BookmarkP me)
{
  librdf_node* subject;
  librdf_node* predicate;
  librdf_node* object;

  char bookmarkid[MAX_LENGTH];
  char *template = "#ambookmark%d";
  char *base_uri = GetLocalBookmarksBaseURI ();
  char *tmp;

  static int genid_counter = 1;

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
  
  genid_counter = Model_queryID (world, model, base_uri, template, 
				 genid_counter);
  if (genid_counter == 0) 
    {
      /* couldn't create a genid */
      return FALSE;
    }
  sprintf (bookmarkid, "%s", base_uri);
  sprintf (bookmarkid + strlen (base_uri), template, genid_counter++);

  /* bookmark type */
  subject = librdf_new_node_from_uri_string (world, bookmarkid);
  predicate = librdf_new_node_from_uri_string (world, RDF_TYPE);
  object =  librdf_new_node_from_uri_string (world,
					     BMNS_BOOKMARK);
  add_statement (world, model, subject, predicate, object);

  /* topic folders */
  if (me->parent_url_list)
      AddBookmarkTopicList (bookmarkid, me->parent_url_list);

#if 0 /* deprecated when moving to multiple topics */
  if (me->parent_url && me->parent_url[0] != EOS)
    {
      subject = librdf_new_node_from_uri_string (world, bookmarkid);
      predicate = librdf_new_node_from_uri_string (world, BMNS_HASTOPIC);
      object =  librdf_new_node_from_uri_string (world, me->parent_url);
      add_statement (world, model, subject, predicate, object);
    }
#endif

  /* creator */
  tmp = TtaConvertByteToMbs (me->author, ISO_8859_1);
  subject = librdf_new_node_from_uri_string (world, bookmarkid);
  predicate = librdf_new_node_from_uri_string (world, DCNS_CREATOR);
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
  predicate = librdf_new_node_from_uri_string (world, DCNS_DATE);
  object =  librdf_new_node_from_literal (world, 
					  me->modified,  /* literal string value */
					  NULL,  /* literal XML language */
 					  0);    /* non 0 if literal is XML */
  add_statement (world, model, subject, predicate, object);


  /* title */
  subject = librdf_new_node_from_uri_string (world, bookmarkid);
  predicate = librdf_new_node_from_uri_string (world, DCNS_TITLE);
  object =  librdf_new_node_from_literal (world, 
					  me->title,  /* literal string value */
					  NULL,  /* literal XML language */
 					  0);    /* non 0 if literal is XML */
  add_statement (world, model, subject, predicate, object);

 /* description */
  subject = librdf_new_node_from_uri_string (world, bookmarkid);
  predicate = librdf_new_node_from_uri_string (world, DCNS_DESCRIPTION);
  object =  librdf_new_node_from_literal (world, 
					  me->description,  /* literal string value */
					  NULL,  /* literal XML language */
 					  0);    /* non 0 if literal is XML */
  add_statement (world, model, subject, predicate, object);

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
  BM_save (GetLocalBookmarksFile ());
  return TRUE;
}

/*----------------------------------------------------------------------
  BM_addTopic
  Adds a topic to a storage model.
  ----------------------------------------------------------------------*/
ThotBool BM_addTopic (BookmarkP me, ThotBool generateID)
{
  librdf_node* subject;
  librdf_node* predicate;
  librdf_node* object;

  char topicid[MAX_LENGTH];
  char *base_uri = GetLocalBookmarksBaseURI ();
  char *template = "#amtopic%d";
  static int genid_counter = 1;
  char *tmp;

  if (generateID)
    {
      /* get new blank node id */
      if (genid_counter == 0) 
	{
	  /* couldn't create a genid */
	  return FALSE;
	}
      
      genid_counter = Model_queryID (world, model, base_uri,
				     template, genid_counter);
      if (genid_counter == 0) 
	{
	  /* couldn't create a genid */
	  return FALSE;
	}
      
      sprintf (topicid, "%s", base_uri);
      sprintf (topicid + strlen (base_uri), template, genid_counter++);
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
  tmp = TtaConvertByteToMbs (me->author, ISO_8859_1); 
  subject = librdf_new_node_from_uri_string (world, topicid);
  predicate = librdf_new_node_from_uri_string (world, DCNS_CREATOR);
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
  predicate = librdf_new_node_from_uri_string (world, DCNS_DATE);
  object =  librdf_new_node_from_literal (world, 
					  me->modified,  /* literal string value */
					  NULL,  /* literal XML language */
 					  0);    /* non 0 if literal is XML */
  add_statement (world, model, subject, predicate, object);


  /* title */
  subject = librdf_new_node_from_uri_string (world, topicid);
  predicate = librdf_new_node_from_uri_string (world, DCNS_TITLE);
  object =  librdf_new_node_from_literal (world, 
					  me->title,
					  NULL,  /* literal XML language */
 					  0);    /* non 0 if literal is XML */
  add_statement (world, model, subject, predicate, object);

 /* description */
  subject = librdf_new_node_from_uri_string (world, topicid);
  predicate = librdf_new_node_from_uri_string (world, DCNS_DESCRIPTION);
  object =  librdf_new_node_from_literal (world, 
					  me->description,
					  NULL,  /* literal XML language */
 					  0);    /* non 0 if literal is XML */
  add_statement (world, model, subject, predicate, object);

  /* save the model */
  BM_save (GetLocalBookmarksFile ());

  return (TRUE);
}

/*----------------------------------------------------------------------
  Model_getNodeAsString
  returns the value of a node as a string.
  Caller must free the returned string.
  ----------------------------------------------------------------------*/
static char* Model_getNodeAsString (librdf_node *node, ThotBool isLiteral)
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
static void Model_getItemInfo (librdf_world *world, librdf_model *model,
			       librdf_node *subject,
			       BookmarkP item, ThotBool isTopic)
{
  /* 
  ** query the model to get all the info we need to sort and display the
  ** bookmarks.
  */
  
  /* @@ JK: not sure if I should copy the subject node here */
  if (isTopic)
    {
      item->isTopic = TRUE;
      item->parent_url = Model_getObjectAsString (world, model, subject, BMNS_SUBTOPICOF);
    }
  else
    {
      List *dump = NULL;

      item->isTopic = FALSE;
      /* JK: Get a list of all the topics to which this bookmark belongs */
      Model_dumpBookmarkTopics (item, &dump);
      item->parent_url_list = dump;       
      item->parent_url = NULL;
      /* item->parent_url = Model_getObjectAsString (world, model, subject, BMNS_HASTOPIC); */
      item->bookmarks = Model_getObjectAsString (world, model, subject, BMNS_BOOKMARKS);
    }
  
  /* modified  */
  item->modified = Model_getObjectAsString (world, model, subject, DCNS_DATE);
  
  /* title */
  item->title = Model_getObjectAsString (world, model, subject, DCNS_TITLE);
}

/*----------------------------------------------------------------------
  Model_dumpAsList
  topics == TRUE means we want to dump the topics.
  topics == FALSE means we want to dump the bookmarks.
  dump contains the info list of topics and bookmarks from the model.
  Returns the number of matching statements.
  ----------------------------------------------------------------------*/
int Model_dumpAsList (List **dump, ThotBool topics)
{
  librdf_stream* stream;
  librdf_statement *partial_statement;
  int count = 0;

  librdf_node *subject = NULL;
  librdf_node *predicate = NULL;
  librdf_node *object = NULL;
  librdf_uri *uri;

  char *uri_str;

  BookmarkP item;

  /* dump the topics */
  predicate = librdf_new_node_from_uri_string (world, RDF_TYPE);
  if (topics)
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

      /* 
      ** query the model to get all the info we need to sort and display the
      ** bookmarks.
      */

      Model_getItemInfo (world, model, subject, item, topics);

      List_add (dump, (void *) item);
      
      if (item->self_url)
	printf ("found node with url %s\n", item->self_url);

      if (item->parent_url)
	printf ("  with parent topic url %s\n", item->parent_url);

      if (item->bookmarks)
	printf ("  and bookmarks url %s\n", item->bookmarks);

      count++;
      librdf_stream_next (stream);
    }

  librdf_free_stream (stream);  
  
  fprintf(stderr, "matching statements: %d\n", count);
  
  return (count);
}

/*----------------------------------------------------------------------
  Model_dumpTopicChild
  dump contains the info list of all child topics under
  the parent topic URL.
  Caller must free the returned list.
  ----------------------------------------------------------------------*/
ThotBool Model_dumpTopicChild (char *parent_topic_url, List **dump)
{
  librdf_stream* stream;
  librdf_statement *partial_statement;

  librdf_node *subject = NULL;
  librdf_node *predicate = NULL;
  librdf_node *object = NULL;

  char *uri_str;
  BookmarkP item;

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
      uri_str = Model_getNodeAsString (subject, FALSE);
      if (uri_str && Model_queryExists (world, model, uri_str, RDF_TYPE, BMNS_TOPIC))
	Model_dumpTopicChild (uri_str, dump); /* yes, add all its subtopics first */
      if (uri_str)
	free (uri_str);
      librdf_stream_next (stream);
    }
    librdf_free_stream (stream);

    item = Bookmark_new ();
    List_add (dump, (void *) item);
    item->self_url = TtaStrdup (parent_topic_url);
    item->isTopic = TRUE;

    subject = librdf_new_node_from_uri_string (world, parent_topic_url);
    predicate = librdf_new_node_from_uri_string (world, RDF_TYPE);
    object =  librdf_new_node_from_uri_string (world,
					     BMNS_TOPIC);
    partial_statement = librdf_new_statement (world);
    librdf_statement_set_subject (partial_statement, subject);
    librdf_statement_set_predicate (partial_statement, predicate);
    librdf_statement_set_object (partial_statement, object);
    stream = librdf_model_find_statements (model, partial_statement);

    if (!librdf_stream_end (stream))
      {
	librdf_statement *statement = librdf_stream_get_object (stream);
	subject = librdf_statement_get_subject (statement);
	Model_getItemInfo (world, model, subject, item, TRUE);
      }
    librdf_free_stream (stream);  

    return (TRUE);
}

/*----------------------------------------------------------------------
  Model_dumpTopicBookmarks
  topics == TRUE means we want to dump the topics.
  topics == TRUE means we want to dump the bookmarks.
  dump contains the info list of topics and bookmarks from the model.
  Returns the number of matching statements.
  @@ Get all the topics and children that have this topic_url as a
  parent. Then call the function recursively to 
  ----------------------------------------------------------------------*/
ThotBool Model_dumpTopicBookmarks (List *topic_list, List **dump)
{
  librdf_stream* stream;
  librdf_statement *partial_statement;

  librdf_node *subject = NULL;
  librdf_node *predicate = NULL;
  librdf_node *object = NULL;

  char *uri_str;
  BookmarkP topic, item;

  List *cur;

  /* now for each topic in the list, we get all its bookmarks */
  cur = topic_list;

  /* we'll reuse the same statement */
  partial_statement = librdf_new_statement (world);

  while (cur)
    {
      topic = cur->object;
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
	  uri_str = Model_getNodeAsString (subject, FALSE);
	  
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
		  Model_getItemInfo (world, model, subject, item, FALSE);
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
  Model_dumpBookmarkTopics  creates a list with all the topics to which bookmark belongs to.
  ----------------------------------------------------------------------*/
ThotBool Model_dumpBookmarkTopics (BookmarkP me, List **dump)
{
  librdf_stream* stream;
  librdf_statement *partial_statement;

  librdf_node *subject = NULL;
  librdf_node *predicate = NULL;
  librdf_node *object = NULL;

  char *uri_str;

  if (!me)
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
      uri_str = Model_getNodeAsString (object, FALSE);
      
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
  topics == TRUE means we want to dump the topics.
  topics == TRUE means we want to dump the bookmarks.
  dump contains the info list of topics and bookmarks from the model.
  Returns the number of matching statements.
  @@ Get all the topics and children that have this topic_url as a
  parent. Then call the function recursively to 
  ----------------------------------------------------------------------*/
ThotBool Model_dumpTopicAsList (List **dump, char *parent_topic_url, ThotBool sort)
{
  List *topic_list = NULL;
  List *bookmark_list = NULL;
  List *cur;
  BookmarkP bookmark;

  /* first we get all the topics */
  Model_dumpTopicChild  (parent_topic_url, &topic_list);
  /* then all the bookmarks contained in each topic */
  Model_dumpTopicBookmarks (topic_list, &bookmark_list);
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

  return (TRUE);
}


/*----------------------------------------------------------------------
  BM_getItem
  Returns a bookmark structure filled up with data from the model.
  The caller has to free the returned bookmark.
  ----------------------------------------------------------------------*/
BookmarkP BM_getItem (char *url, ThotBool isTopic)
{
  librdf_stream *stream;
  librdf_statement *partial_statement;

  librdf_node *subject = NULL;
  librdf_node *predicate = NULL;
  librdf_node *object = NULL;
  librdf_uri *rdf_uri;
  char *uri_str;

  BookmarkP me;

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
  me->isTopic = FALSE;
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
      if (!strcmp (uri_str, DCNS_CREATOR))
	me->author =  Model_getNodeAsString (object, TRUE);
      /* created */
      else if (!strcmp (uri_str, ANNOTNS_CREATED))
	me->created = Model_getNodeAsString (object, TRUE);
      /* modified  */
      else if (!strcmp (uri_str, DCNS_DATE))
	me->modified = Model_getNodeAsString (object, TRUE);
      /* title */
      else if (!strcmp (uri_str, DCNS_TITLE))
	me->title = Model_getNodeAsString (object, TRUE);
      /* description */
      else if (!strcmp (uri_str, DCNS_DESCRIPTION))
	me->description = Model_getNodeAsString (object, TRUE);
      /* bookmark / topics distinctions */
      else
	{
	  if (isTopic)
	    {
	      if (!strcmp (uri_str,  BMNS_SUBTOPICOF))
		me->parent_url = Model_getNodeAsString (object, FALSE);
	    }
	  else
	    {
	      /* topic folder */
	      if (!strcmp (uri_str, BMNS_HASTOPIC))
		me->parent_url =  Model_getNodeAsString (object, FALSE);
	      /* bookmarks */
	      else if (!strcmp (uri_str, BMNS_BOOKMARKS))
		me->bookmarks = Model_getNodeAsString (object, FALSE);
	      /* context */
	      else if (!strcmp (uri_str,  ANNOTNS_CONTEXT))
		me->context = Model_getNodeAsString (object, TRUE);
	    }
	}
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
ThotBool BM_updateItem (BookmarkP me, ThotBool isTopic)
{
  librdf_stream* stream;
  librdf_statement *partial_statement;

  librdf_node *subject = NULL;
  librdf_node *predicate = NULL;
  librdf_node *object = NULL;
  librdf_uri *rdf_uri;
  char *uri_str;
  char *tmp;

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
      /* @@ JK shouldn't be added by user? creator */
      if (!strcmp (uri_str, DCNS_CREATOR))
	{
	  tmp = TtaConvertByteToMbs (me->author, ISO_8859_1);
	  Model_replace_object (world, model, statement, tmp, TRUE);
	  TtaFreeMemory (tmp);
	}
      /* @@ JK created */
      else if (!strcmp (uri_str, ANNOTNS_CREATED))
	Model_replace_object (world, model, statement, me->created, TRUE);
      /* @@ JK modified  */
      else if (!strcmp (uri_str, DCNS_DATE))
	Model_replace_object (world, model, statement, me->modified, TRUE);
      /* title */
      else if (!strcmp (uri_str, DCNS_TITLE))
	  Model_replace_object (world, model, statement, me->title, TRUE);
      /* description */
      else if (!strcmp (uri_str, DCNS_DESCRIPTION))
	  Model_replace_object (world, model, statement, me->description, TRUE);
      else
	{
	  /* bookmark / topic items differences */
	  if (isTopic)
	    {
	      /* parent topic */
	      if (!strcmp (uri_str,  BMNS_SUBTOPICOF))
		Model_replace_object (world, model, statement, me->parent_url, FALSE);
	    }
	  else
	    {
	      /* parent topic */
	      if (!strcmp (uri_str, BMNS_HASTOPIC))
		librdf_model_remove_statement (model, statement);
	      /* bookmarks */
	      else if (!strcmp (uri_str, BMNS_BOOKMARKS))
		Model_replace_object (world, model, statement, me->bookmarks, FALSE);
	      else if (!strcmp (uri_str,  ANNOTNS_CONTEXT))
		Model_replace_object (world, model, statement, me->context, TRUE);
	    }
	}
      librdf_stream_next (stream);
    }

  librdf_free_stream (stream);
  
  if (!isTopic)
      AddBookmarkTopicList (me->self_url, me->parent_url_list);

  return TRUE;
}

/*----------------------------------------------------------------------
  BM_deleteItem
  Erases all the statements related to a given URL
  ----------------------------------------------------------------------*/
ThotBool BM_deleteItem (char *item_url)
{
  librdf_stream* stream;
  librdf_statement *partial_statement;

  librdf_node *subject = NULL;
  librdf_node *predicate = NULL;
  librdf_node *object = NULL;

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
ThotBool BM_deleteItemList (char *parent_topic, List *items)
{
  List       *cur;
  BookmarkP   item;
    
  if (!items)
    return FALSE;
  
  cur = items;

  while (cur)
    {
      item = (BookmarkP) cur->object;
      if (item->isTopic)
	BM_deleteItem (item->self_url);
      else 
	BM_deleteBookmarkItem (parent_topic, item->self_url);
      cur = cur->next;
    }
  return TRUE;
}


/*----------------------------------------------------------------------
  BM_deleteItemList
  Erases all the statements related to a given topic
  ----------------------------------------------------------------------*/
ThotBool BM_deleteBookmarkItem (char *parent_url, char *self_url)
{
  librdf_stream* stream;
  librdf_statement *partial_statement;

  librdf_node *subject = NULL;
  librdf_node *predicate = NULL;
  librdf_node *object = NULL;
  ThotBool can_erase;

  if (!parent_url || !self_url)
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
    BM_deleteItem (self_url);

  return TRUE;
}


