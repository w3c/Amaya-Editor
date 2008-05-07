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
 * The schema is parsed into linked data structures that, once,
 * created, are expected to last for the remaining lifetime of
 * the application and must be freed as an entire unit.  This
 * permits the use of simple (memory address) handles rather
 * than strings (URIs) for Resources.

 * NOTE: the code will use the libwww RDF parser by default; if
 *    RAPTOR_RDF_PARSER is defined the Raptor RDF parser:
 *
 *     http://www.redland.opensource.ac.uk/raptor/
 *
 *    will be used.
 *
 * Author: Ralph R. Swick (W3C/MIT)
 * $Revision$
 *
 */

/* system includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* annotlib includes */
#include "annotlib.h"
#include "ANNOTschemas_f.h"
#include "ANNOTtools_f.h"
#include "ANNOTevent_f.h"

/* RDF parser */
#include "raptor.h"

/* amaya includes */
#include "AHTURLTools_f.h"
#include "init_f.h"
#include "query_f.h"

/********************** static variables ***********************/

/* 
 * Namespace and Property names used in an Annotation Description 
 */

static RDFResourceP subclassOfP = NULL;
static RDFResourceP typeP = NULL;
static RDFPropertyP labelP = NULL;

/********************** global variables ***********************/

List *annot_schema_list = NULL;  /* a list of schemas */
char *ANNOT_NS = NULL;	/* the Annotation namespace name to use */
char *ANNOTATION_CLASSNAME = NULL;
RDFPropertyP PROP_name = NULL;
RDFPropertyP PROP_firstName = NULL;
RDFPropertyP PROP_Email = NULL;
RDFPropertyP PROP_usesIcon = NULL;
RDFClassP ANNOTATION_CLASS;
RDFClassP DEFAULT_ANNOTATION_TYPE;

#ifdef ANNOT_ON_ANNOT
RDFClassP THREAD_REPLY_CLASS;
RDFClassP DEFAULT_REPLY_TYPE;
#endif /* ANNOT_ON_ANNOT */


typedef struct _ReadCallbackContext
{
  char filename[MAX_LENGTH];
} ReadCallbackContext;

typedef struct _ParseContext
{
  List **annot_schema_list;
  char *base_uri;   /* base URI for anonymous RDF names */
} ParseContext, *ParseContextP;

/*------------------------------------------------------------
   _ListSearchResource
  ------------------------------------------------------------*/
static RDFResourceP _ListSearchResource( List* list, const char* name)
{
  List *item = list;
  RDFResourceP resource = NULL;
  ThotBool found = FALSE;

  while (item)
    {
      resource = (RDFResourceP) item->object;
      if (resource && resource->name 
	  && !strcmp (resource->name, name))
	{
	  found = TRUE;
	  break;
	}
      item = item->next;
    }

  return found ? resource : NULL;
}

/*------------------------------------------------------------
   SCHEMA_AddStatement
  ------------------------------------------------------------*/
void SCHEMA_AddStatement (RDFResourceP s, RDFPropertyP p, RDFResourceP o)
{
  RDFStatement *statement;

  if (s && p && o)
    {
      statement = (RDFStatement*)TtaGetMemory (sizeof(RDFStatement));

      statement->predicate = p;
      statement->object = o;
      List_add (&s->statements, (void*)statement);
    }
}

/*------------------------------------------------------------
   _AddInstance
  ------------------------------------------------------------*/
static void _AddInstance( RDFClassP c, RDFResourceP instance )
{
  if (!c->class_)
    {
      c->class_ = (RDFClassExtP) TtaGetMemory (sizeof(RDFClassExt));
      memset (c->class_, 0, sizeof (RDFClassExt));
    }

  List_add (&c->class_->instances, (void*)instance);
}

/*------------------------------------------------------------
   _AddSubClass
  ------------------------------------------------------------*/
static void _AddSubClass( RDFClassP c, RDFClassP sub )
{
  if (!c->class_)
    {
      c->class_ = (RDFClassExtP)TtaGetMemory (sizeof(RDFClassExt));
      c->class_->instances = NULL;
      c->class_->subClasses = NULL;
    }

  List_add (&c->class_->subClasses, (void*)sub);
}

/* ------------------------------------------------------------
   as_triple_handler

   This callback is invoked when a new RDF triple has been parsed.
  
   As the triples arrive, their predicate is checked and its
   object is cached.
  
   Parameters:
     rdfp - the RDF parser
     triple - an RDF triple
     context - pointer to an RDFResource list
 ------------------------------------------------------------*/
static void as_triple_handler (void * context, const raptor_statement *triple)
{

  ParseContextP parse_ctx = (ParseContextP) context;
  List **listP = parse_ctx->annot_schema_list;

  if (triple) 
    {
      char * predicate = (char *) AM_RAPTOR_URI_AS_STRING(triple->predicate);
      char * subject = NULL;
      char * object = NULL;

      RDFResourceP subjectP;
      RDFPropertyP predicateP = ANNOT_FindRDFResource (listP, predicate, TRUE);

      RDFResourceP objectP;

      /* if it's an anoynmous subject, add the base_uri */
      if (triple->subject_type == RAPTOR_IDENTIFIER_TYPE_ANONYMOUS)
	{
	  char *base_uri = parse_ctx->base_uri;
	  char *ptr = (char *) triple->subject;
	  subject = (char *)TtaGetMemory (strlen (base_uri) + strlen (ptr) + 2);
	  sprintf (subject, "%s#%s", base_uri, ptr);
	}
      else
	subject = (char *) AM_RAPTOR_URI_AS_STRING(triple->subject);

      subjectP = ANNOT_FindRDFResource (listP, subject, TRUE);

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

      objectP = ANNOT_FindRDFResource (listP, object, TRUE);

      SCHEMA_AddStatement (subjectP, predicateP, objectP);

      if (!typeP)
	  typeP = ANNOT_FindRDFResource (listP, RDF_TYPE, TRUE);

      if (!subclassOfP)
	subclassOfP = ANNOT_FindRDFResource (listP, RDFS_SUBCLASSOF, TRUE);

      if (predicateP == typeP)
	_AddInstance( objectP, subjectP );
      else if (predicateP == subclassOfP)
	{
	  _AddSubClass( objectP, subjectP );
	  subjectP->class_ = (RDFClassExtP)TtaGetMemory (sizeof(RDFClassExt));
	  subjectP->class_->instances = NULL;
	  subjectP->class_->subClasses = NULL;
	}

      if (triple->subject_type == RAPTOR_IDENTIFIER_TYPE_ANONYMOUS)
	TtaFreeMemory (subject);
    }
}


/*-----------------------------------------------------------------------
  ReadSchema_callback
  -----------------------------------------------------------------------*/
static void ReadSchema_callback (Document doc, int status, 
				 char *urlName,
				 char *outputfile, 
				 AHTHeaders *http_headers,
				 void * context)
{
  raptor_parser* rdfxml_parser=NULL;
  raptor_uri *uri = NULL;
  char *full_file_name = NULL;
  ParseContextP parse_ctx = NULL;

  ReadCallbackContext *ctx = (ReadCallbackContext*) context;
  BOOL parse = NO;
  
   if (!ctx)
     return;

   ResetStop (doc);

  if (status == HT_OK)
    {
      rdfxml_parser = raptor_new_parser ("rdfxml");

      if (!rdfxml_parser)
	return;

      parse_ctx = (ParseContextP)TtaGetMemory (sizeof (ParseContext));

      /* @@ this is what we should do eventually */
      /* if (!IsW3Path (file_name))
	 full_file_name = LocalToWWW (file_name);
	 else
      */
      /* raptor doesn't grok file URIs under windows. The following is a patch so
	 that we can use it */
      full_file_name = (char *)TtaGetMemory (strlen (ctx->filename) + sizeof ("file:"));
      sprintf (full_file_name, "file:%s", ctx->filename);
      /* remember the base name for anoynmous subjects */
      parse_ctx->base_uri = full_file_name;
      parse_ctx->annot_schema_list = &annot_schema_list;
      raptor_set_statement_handler (rdfxml_parser, (void *) parse_ctx, as_triple_handler);
      
      {
	unsigned char *tmp;
	
	tmp = raptor_uri_filename_to_uri_string ((const char *) ctx->filename);
	/* tmp = raptor_uri_filename_to_uri_string ((const char *) "/home/kahan/Amaya/config/annotschema.rdf"); */
	uri = raptor_new_uri ((const unsigned char *) tmp);
	free (tmp);
      }

      parse = raptor_parse_file (rdfxml_parser, uri, NULL);

      if (!parse)
	TtaSetStatus (doc, 1, "Schema read", NULL); /* @@ */
      else
	TtaSetStatus (doc, 1, "Error during schema read", NULL); /* @@ */
    }  

  raptor_free_uri (uri);
  raptor_free_parser (rdfxml_parser);
  TtaFreeMemory (full_file_name);
  TtaFreeMemory (parse_ctx);
  TtaFreeMemory (ctx);
}
 
/********************** Public API entry point ********************/

/*------------------------------------------------------------
   ANNOT_FindRDFResource
  ------------------------------------------------------------*/
RDFResourceP ANNOT_FindRDFResource( List** listP, const char* name, ThotBool create )
{
  RDFResourceP resource;

  if (!name || !name[0])
    return NULL;

  /* search for resource in list */
  resource = _ListSearchResource (*listP, name);
  if ((!resource || !resource->name) && create)
    {
      if (!resource)
        resource = (RDFResourceP) TtaGetMemory (sizeof(RDFResource));

      resource->name = TtaStrdup (name);
      resource->statements = NULL;
      resource->class_ = NULL;
      List_add (listP, (void*) resource);
    }

  return resource;
}


/*------------------------------------------------------------
   ANNOT_FindRDFStatement
  ------------------------------------------------------------*/
RDFStatementP ANNOT_FindRDFStatement( List* list, RDFPropertyP p )
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

   Return a shared string label for the specified RDF resource.
   If not rdfs:label property is found for the resource or (if
   the resource is a class, then) for any of its superclasses,
   then return the name of the resource.
  ------------------------------------------------------------*/
char *ANNOT_GetLabel (List **listP, RDFResourceP r)
{
  RDFStatementP labelS = NULL;

  if (!labelP)
    labelP = ANNOT_FindRDFResource (listP, RDFS_LABEL, FALSE);

  if (!labelP)
    return r->name;

  while (!labelS)
    {
      labelS = ANNOT_FindRDFStatement (r->statements, labelP);

      if (labelS)
	{
	  /* RRS @@ assumes object is a literal, doesn't account for lang */
	  return labelS->object->name;
	}

      if (r->class_)
	{
	  /* search superclass(es) for a label */
	  /* RRS @@ only does one superclass for now */
	  RDFStatementP s;

	  if (!subclassOfP)
	    subclassOfP = ANNOT_FindRDFResource (listP, RDFS_SUBCLASSOF, TRUE);

	  s = ANNOT_FindRDFStatement (r->statements, subclassOfP);

	  if (!s)
	    return r->name;	/* unknown parent class */

	  r = s->object;
	}
      else
	return r->name;
    }
  return r->name;
}



/*------------------------------------------------------------
   SCHEMA_ReadSchema
  ------------------------------------------------------------
   Parses an RDF schema.
  
   Parameters:
     namespace_URI - the name of the Schema to parse
  ------------------------------------------------------------*/
void SCHEMA_ReadSchema (Document doc, char *namespace_URI)
{
  ReadCallbackContext *ctx;
  int res;

  /* make some space to store the remote file name */
  ctx = (ReadCallbackContext*)TtaGetMemory (sizeof(ReadCallbackContext));

  ANNOT_UpdateTransfer (doc);

  if (IsFilePath(namespace_URI))
    {
      NormalizeFile (namespace_URI, ctx->filename, AM_CONV_ALL);
      ReadSchema_callback (doc, HT_OK, namespace_URI, NULL, NULL, (void*)ctx);
      return;
    }
  else
    {
      /* launch the request */
      res = GetObjectWWW (doc,
			  0,
			  namespace_URI,
			  NULL,
			  ctx->filename,
			  /* AMAYA_ASYNC | AMAYA_NOCACHE | */
			  AMAYA_SYNC | AMAYA_FLUSH_REQUEST,
			  NULL,
			  NULL, 
			  (void (*)(int, int, char*, char*, char*, const AHTHeaders*, void*)) ReadSchema_callback,
			  (void *) ctx,
			  NO,
			  "application/xml");
    }

  if (res != HT_OK)
      TtaSetStatus (doc, 1, "Couldn't read schema", NULL); /* @@ */
  else
      TtaSetStatus (doc, 1, "Annotation schema downloaded", NULL); /* @@ */
}

/*------------------------------------------------------------
   FreeAnnotNS
  ------------------------------------------------------------
   Frees resources associated with the base annotation namespace name
  ------------------------------------------------------------*/
static void FreeAnnotNS (void)
{
  if (ANNOT_NS)
    {
      TtaFreeMemory (ANNOT_NS);
      ANNOT_NS = NULL;
      TtaFreeMemory (ANNOTATION_CLASSNAME);
      ANNOTATION_CLASSNAME = NULL;
    }
}


/*------------------------------------------------------------
   SetAnnotNS
  ------------------------------------------------------------
   Sets the base annotation namespace name
  ------------------------------------------------------------*/
static void SetAnnotNS (const char *ns_name)
{
  ANNOT_NS = TtaStrdup (ns_name);
  ANNOTATION_CLASSNAME = (char *)TtaGetMemory (strlen(ns_name)
				       + strlen(ANNOT_LOCAL_NAME)
				       + 1);
  strcpy (ANNOTATION_CLASSNAME, ns_name);
  strcat (ANNOTATION_CLASSNAME, ANNOT_LOCAL_NAME);

  ANNOTATION_CLASS = ANNOT_FindRDFResource (&annot_schema_list,
					    ANNOTATION_CLASSNAME,
					    FALSE);

  if (!ANNOTATION_CLASS)
    ANNOTATION_CLASS = ANNOT_FindRDFResource (&annot_schema_list,
					      FALLBACK_ANNOTATION_CLASSNAME,
					      TRUE);

  /* Establish an ultimate fallback type for new annotations */
  DEFAULT_ANNOTATION_TYPE = ANNOTATION_CLASS;
}


/*------------------------------------------------------------
   SCHEMA_InitSchemas
  ------------------------------------------------------------
   Initializes the annotation schemas (annot_schema_list) from a config file.
  ------------------------------------------------------------*/
void SCHEMA_InitSchemas (Document doc)
{
  char* thotdir;
  char *app_home;
  FILE *fp;
  int len;
  char *buffer;

  thotdir = TtaGetEnvString ("THOTDIR");
  app_home = TtaGetEnvString ("APP_HOME");

  FreeAnnotNS();

  PROP_Email = ANNOT_FindRDFResource (&annot_schema_list,
				      EMAIL_PROPNAME,
				      TRUE);

  PROP_firstName = ANNOT_FindRDFResource (&annot_schema_list,
					  FIRSTNAME_PROPNAME,
					  TRUE);

  PROP_name = ANNOT_FindRDFResource (&annot_schema_list,
				     NAME_PROPNAME,
				     TRUE);

  len = strlen(thotdir) + strlen(app_home) + MAX_LENGTH + 32;

  buffer = (char *)TtaGetMemory(len);
  sprintf (buffer, "%s%cannot.schemas", app_home, DIR_SEP);

  if (!TtaFileExist (buffer))
    {
      sprintf (buffer, "%s%cconfig%cannot.schemas",
		thotdir, DIR_SEP, DIR_SEP);

      if (!TtaFileExist (buffer))
	{
	  /* RRS @@ installation error */
	  TtaFreeMemory (buffer);
	  SetAnnotNS (FALLBACK_ANNOTATION_NS);
	  return;
	}
    }

  fp = TtaReadOpen (buffer);

  if (!fp)
    {
      /* RRS @@ installation error */
      SetAnnotNS (FALLBACK_ANNOTATION_NS);
      TtaFreeMemory (buffer);
      return;
    }

  while (fgets (buffer, len, fp))
    {
      char *nsname, *fname, *cp;
      int l;

      if (buffer[0] == '#')	/* comment lines start with '#' */
	continue;

      nsname = buffer;

      l = strlen(buffer);
      while (l-- && (buffer[l] == '\n' || buffer[l] == '\r'))
	buffer[l] = '\0';

      cp = strchr (buffer, ' ');
      if (cp)
	*cp++ = '\0';

      if (!*nsname)		/* line was blank; skip it */
	continue;

      if (cp)
	  while (*cp && isspace (*cp)) cp++;

      if (!cp || !*cp)
	/* only a namespace name was specified; try to fetch a schema
	   from the Web using the namespace name */
	{
	  SCHEMA_ReadSchema (doc, nsname);
	  if (!ANNOT_NS)	/* is this the first schema listed? */
	    SetAnnotNS (nsname);
	  continue;
	}

      /* both namespace name and [local] filename were specified;
	 read the schema from the specified file */
      nsname = TtaStrdup (nsname); /* we might overwrite buffer */
      fname = cp;
      while (*cp && !isspace (*cp)) cp++;
      *cp = '\0';		/* terminate filename */

      /* expand $THOTDIR and $APP_HOME */
      if (strncmp (fname, "$THOTDIR", 8) == 0)
	{
	  if (strlen (thotdir) > (unsigned) (fname - buffer + 7))
	    /* no room at beginning of buffer to expand THOTDIR,
	       so copy the filename elsewhere before concatenating */
	    {
	      char *temp = (char *)TtaGetMemory(len);

	      strcpy (temp, fname); /* copy */
	      strcpy (buffer, thotdir);
	      strcat (buffer, temp+8);
	      TtaFreeMemory (temp);
	    }
	  else
	    {
	      strcpy (buffer, thotdir);
	      strcat (buffer, fname+8);
	    }
	  fname = buffer;
	}
      else
	if (strncmp (fname, "$APP_HOME", 9) == 0)
	  {
	    if (strlen (app_home) > (unsigned) (fname - buffer + 8))
	      /* no room at beginning of buffer to expand APP_HOME,
		 so copy the filename elsewhere before concatenating */
	      {
		char *temp = (char *)TtaGetMemory(len);

		strcpy (temp, fname); /* copy */
		strcpy (buffer, app_home);
		strcat (buffer, temp+9);
		TtaFreeMemory (temp);
	      }
	    else
	      {
		strcpy (buffer, app_home);
		strcat (buffer, fname+9);
	      }
	    fname = buffer;
	  }

      if (TtaFileExist (fname))
	{
	  char *ptr;
	  if (!IsW3Path (fname) && !IsFilePath (fname))
	    ptr = ANNOT_MakeFileURL (fname);
	  else
	    ptr = fname;
	  SCHEMA_ReadSchema (doc, ptr);
	  if (ptr != fname)
	    TtaFreeMemory (ptr);

	  if (!ANNOT_NS)	/* is this the first schema listed? */
	    SetAnnotNS (nsname);
	}

      TtaFreeMemory (nsname);
      /* RRS @@ else config error */
    }

  TtaReadClose (fp);
  TtaFreeMemory (buffer);

  /* Establish a default type for new annotations. */

  /* @@ RRS: ANNOT_DEFAULT_TYPE should be accessible from the config menu */
  buffer = TtaGetEnvString ("ANNOT_DEFAULT_TYPE");
  if (!buffer)
    buffer = TtaStrdup("Comment");	/* fallback default type */

  /* two options; user can specify a full property URI or just the localname */
  if (IsW3Path (buffer)) /* full URI */
    DEFAULT_ANNOTATION_TYPE = ANNOT_FindRDFResource (&annot_schema_list,
						     buffer,
						     TRUE);
  else /* localname only */
    { /* Search the subtypes of ANNOTATION_CLASS for one whose name matches */
      RDFClassP annotType = ANNOTATION_CLASS;
      if (annotType && annotType->class_)
	{
	  int len = strlen (buffer);
	  List *item = annotType->class_->subClasses;
	  for (; item; item=item->next)
	    {
	      RDFClassP annotType = (RDFClassP)item->object;
	      int p = strlen (annotType->name) - len;

	      /* @@ RRS: should check the entire localname, not just the tail.
		 Use rdfs:isDefinedBy to split out the namespace name. */
	      if (!strncmp (buffer, &annotType->name[p], len))
		{
		  DEFAULT_ANNOTATION_TYPE = annotType;
		  break;
		}
	    }
	}
    }
#ifdef ANNOT_ON_ANNOT
  THREAD_REPLY_CLASS = ANNOT_FindRDFResource (&annot_schema_list,
					      THREAD_NS THREAD_REPLY_LOCAL_NAME,
					      TRUE);
  DEFAULT_REPLY_TYPE = THREAD_REPLY_CLASS;
  /* @@ RRS: REPLY_DEFAULT_TYPE should be accessible from the config menu */
  buffer = TtaGetEnvString ("REPLY_DEFAULT_TYPE");
  if (!buffer)
    buffer = TtaStrdup("Comment");	/* fallback default type */

  /* two options; user can specify a full property URI or just the localname */
  if (IsW3Path (buffer)) /* full URI */
    DEFAULT_REPLY_TYPE = ANNOT_FindRDFResource (&annot_schema_list,
						buffer,
						TRUE);
  else /* localname only */
    { /* Search the subtypes of THREAD_REPLY_CLASS for one whose name matches */
      RDFClassP annotType = THREAD_REPLY_CLASS;
      if (annotType && annotType->class_)
	{
	  int len = strlen (buffer);
	  List *item = annotType->class_->subClasses;
	  for (; item; item=item->next)
	    {
	      RDFClassP annotType = (RDFClassP)item->object;
	      int p = strlen (annotType->name) - len;

	      /* @@ RRS: should check the entire localname, not just the tail.
		 Use rdfs:isDefinedBy to split out the namespace name. */
	      if (!strncmp (buffer, &annotType->name[p], len))
		{
		  DEFAULT_REPLY_TYPE = annotType;
		  break;
		}
	    }
	}
    }
#endif /* ANNOT_ON_ANNOT */
}


#ifdef RDFSCHEMA_DEBUG
/*------------------------------------------------------------
   SCHEMA_DumpRDFResources
  ------------------------------------------------------------
   Dumps the schema model to stderr for debugging
  ------------------------------------------------------------*/

int SCHEMA_DumpRDFResources()
{
  List* list = annot_schema_list;
  int entries = 0;

  while (list)
    {
      RDFResourceP r = list->object;
      fprintf (stderr, "%d: 0x%x \"%s\" statements: 0x%x\n",
	       ++entries,
	       r,
	       r->name,
	       r->statements);
      list = list->next;
    }

  return entries;
}
#endif /* RDFSCHEMA_DEBUG */


/*------------------------------------------------------------
   SCHEMA_FreeRDFResource
  ------------------------------------------------------------
   Frees the dynamic heap resources for a singe RDF Resource
  ------------------------------------------------------------*/
static ThotBool SCHEMA_FreeRDFResource( void *item )
{
  RDFResourceP r = (RDFResourceP)item;

  TtaFreeMemory (r->name);
  List_delAll (&r->statements, List_delCharObj);
  if (r->class_)
    {
      List_delAll (&r->class_->instances, NULL);
      List_delAll (&r->class_->subClasses, NULL);
      TtaFreeMemory ((char *)r->class_);
    }
  TtaFreeMemory (r);
  return TRUE;
}


/*------------------------------------------------------------
   SCHEMA_FreeAnnotSchema
  ------------------------------------------------------------
   Frees the dynamic heap for all resources in the RDF Model
   for the loaded schema(s)
  ------------------------------------------------------------*/
void SCHEMA_FreeAnnotSchema( void )
{
  SCHEMA_FreeRDFModel (&annot_schema_list);
  subclassOfP = NULL;
  typeP = NULL;
  labelP = NULL;
  PROP_name = NULL;
  PROP_firstName = NULL;
  PROP_Email = NULL;
  PROP_usesIcon = NULL;
  ANNOTATION_CLASS = NULL;
  DEFAULT_ANNOTATION_TYPE = NULL;
#ifdef ANNOT_ON_ANNOT
  THREAD_REPLY_CLASS = NULL;
  DEFAULT_REPLY_TYPE = NULL;
#endif /* ANNOT_ON_ANNOT */
  FreeAnnotNS();
}


/*------------------------------------------------------------
   SCHEMA_FreeRDFModel()
  ------------------------------------------------------------
   Frees the dynamic heap for all resources in an RDF Model
  ------------------------------------------------------------*/
void SCHEMA_FreeRDFModel( List **model )
{
  List_delAll (model, SCHEMA_FreeRDFResource);
}
