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
#include "AHTURLTools_f.h"
#include "HTRDF.h"

/********************** static variables ***********************/

/* 
 * Namespace and Property names used in an Annotation Description 
 */

static RDFResourceP subclassOfP = NULL;
static RDFResourceP typeP = NULL;
static RDFPropertyP labelP = NULL;

/********************** global variables ***********************/

List *annot_schema_list = NULL;  /* a list of schemas */
CHAR_T *ANNOT_NS = NULL;	/* the Annotation namespace name to use */
CHAR_T *ANNOTATION_CLASSNAME = NULL;
RDFPropertyP PROP_name = NULL;
RDFPropertyP PROP_firstName = NULL;
RDFPropertyP PROP_Email = NULL;
RDFClassP ANNOTATION_CLASS;
RDFClassP DEFAULT_ANNOTATION_TYPE;

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
  RDFResourceP resource = NULL;
  ThotBool found = FALSE;

  while (item)
    {
      resource = (RDFResourceP) item->object;
      if (resource && resource->name 
	  && !ustrcmp (resource->name, name))
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
#ifdef  __STDC__
void SCHEMA_AddStatement (RDFResourceP s, RDFPropertyP p, RDFResourceP o)
#else  /* __STDC__ */
void SCHEMA_AddStatement (s, p, o)
RDFResourceP s;
RDFPropertyP p;
RDFResourceP o;
#endif
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

  if (rdfp && triple) 
    {
      char * predicate = HTTriple_predicate(triple);
      char * subject = HTTriple_subject(triple);
      char * object = HTTriple_object(triple);

      RDFResourceP subjectP = ANNOT_FindRDFResource (listP, subject, TRUE);
      RDFPropertyP predicateP = ANNOT_FindRDFResource (listP, predicate, TRUE);
      /* ugly, ugly; libwww discards info -- is the object a Literal? */
      RDFResourceP objectP = ANNOT_FindRDFResource (listP, object, TRUE);

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
static void ReadSchema_callback (Document doc, int status, 
				 CHAR_T *urlName,
				 CHAR_T *outputfile, 
				 AHTHeaders *http_headers,
				 void * context)
#else  /* __STDC__ */
static void ReadSchema_callback (doc, status, urlName,
				 outputfile, http_headers,
				 context)
Document doc;
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

   ResetStop (doc);

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
  RDFResourceP resource;

  if (!name)
    return NULL;

  /* search for resource in list */
  resource = _ListSearchResource (*listP, name);
  if ((!resource || !resource->name) && create)
    {
      if (!resource)
        resource = TtaGetMemory (sizeof(RDFResource));

      resource->name = TtaStrdup (name);
      resource->statements = NULL;
      resource->class = NULL;
      List_add (listP, (void*) resource);
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

   Return a shared string label for the specified RDF resource.
   If not rdfs:label property is found for the resource or (if
   the resource is a class, then) for any of its superclasses,
   then return the name of the resource.
  ------------------------------------------------------------*/
#ifdef __STDC__
char *ANNOT_GetLabel (List **listP, RDFResourceP r)
#else /*  __STDC__ */
char *ANNOT_GetLabel (listP, r)
     ListP **listP;
     RDFResourceP r;
#endif /*  __STDC__ */
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

      if (r->class)
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
#ifdef __STDC__
void SCHEMA_ReadSchema (Document doc, char *namespace_URI)
#else /* __STDC__ */
void SCHEMA_ReadSchema (doc, namespace_URI)
     Document doc;
     char *namespace_URI;
#endif /* __STDC__ */
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
			  TEXT("application/xml"));
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
#ifdef __STDC__
static void FreeAnnotNS (void)
#else /* __STDC__ */
static void FreeAnnotNS ( /* void */ )
#endif /* __STDC__ */
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
#ifdef __STDC__
static void SetAnnotNS (char *ns_name)
#else /* __STDC__ */
static void SetAnnotNS ()
     char *ns_name;
#endif /* __STDC__ */
{
  ANNOT_NS = TtaStrdup (ns_name);
  ANNOTATION_CLASSNAME = TtaGetMemory (strlen(ns_name)
				       + strlen(ANNOT_LOCAL_NAME)
				       + 1);
  ustrcpy (ANNOTATION_CLASSNAME, ns_name);
  ustrcat (ANNOTATION_CLASSNAME, ANNOT_LOCAL_NAME);

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
#ifdef __STDC__
void SCHEMA_InitSchemas (Document doc)
#else /* __STDC__ */
void SCHEMA_InitSchemas (doc)
     Document doc;
#endif /* __STDC__ */
{
  CHAR_T* thotdir;
  CHAR_T *app_home;
  FILE *fp;
  int len;
  CHAR_T *buffer;

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

  buffer = TtaGetMemory(len);
  usprintf (buffer, TEXT("%s%cannot.schemas"), app_home, DIR_SEP);

  if (!TtaFileExist (buffer))
    {
      usprintf (buffer, TEXT("%s%cconfig%cannot.schemas"),
		thotdir, DIR_SEP, DIR_SEP);

      if (!TtaFileExist (buffer))
	{
	  /* RRS @@ installation error */
	  TtaFreeMemory (buffer);
	  SetAnnotNS (FALLBACK_ANNOTATION_NS);
	  return;
	}
    }

  fp = fopen (buffer, "r");

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
	      char *temp = TtaGetMemory(len);

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
		char *temp = TtaGetMemory(len);

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
	  HTRDF_parseFile (fname, triple_handler, &annot_schema_list);
	  if (!ANNOT_NS)	/* is this the first schema listed? */
	    SetAnnotNS (nsname);
	}

      TtaFreeMemory (nsname);
      /* RRS @@ else config error */
    }

  fclose (fp);
  TtaFreeMemory (buffer);

  /* Establish a default type for new annotations. */

  /* @@ RRS: ANNOT_DEFAULT_TYPE should be accessible from the config menu */
  buffer = TtaGetEnvString ("ANNOT_DEFAULT_TYPE");
  if (!buffer)
    buffer = TEXT("Comment");	/* fallback default type */

  /* two options; user can specify a full property URI or just the localname */
  if (IsW3Path (buffer)) /* full URI */
    DEFAULT_ANNOTATION_TYPE = ANNOT_FindRDFResource (&annot_schema_list,
						     buffer,
						     TRUE);
  else /* localname only */
    { /* Search the subtypes of ANNOTATION_CLASS for one whose name matches */
      RDFClassP annotType = ANNOTATION_CLASS;
      if (annotType && annotType->class)
	{
	  int len = strlen (buffer);
	  List *item = annotType->class->subClasses;
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
  if (r->class)
    {
      List_delAll (&r->class->instances, NULL);
      List_delAll (&r->class->subClasses, NULL);
      TtaFreeMemory ((CHAR_T *)r->class);
    }
  return TRUE;
}


/*------------------------------------------------------------
   SCHEMA_FreeAnnotSchema
  ------------------------------------------------------------
   Frees the dynamic heap for all resources in the RDF Model
   for the loaded schema(s)
  ------------------------------------------------------------*/
#ifdef __STDC__
void SCHEMA_FreeAnnotSchema( void )
#else /* __STDC__ */
void SCHEMA_FreeAnnotSchema()
#endif /* __STDC__ */
{
  SCHEMA_FreeRDFModel (&annot_schema_list);
  subclassOfP = NULL;
  typeP = NULL;
  labelP = NULL;
  PROP_name = NULL;
  PROP_firstName = NULL;
  PROP_Email = NULL;
  ANNOTATION_CLASS = NULL;
  DEFAULT_ANNOTATION_TYPE = NULL;
  FreeAnnotNS();
}


/*------------------------------------------------------------
   SCHEMA_FreeRDFModel()
  ------------------------------------------------------------
   Frees the dynamic heap for all resources in an RDF Model
  ------------------------------------------------------------*/
#ifdef __STDC__
void SCHEMA_FreeRDFModel( List **model )
#else /* __STDC__ */
void SCHEMA_FreeRDFModel()
     List **model;
#endif /* __STDC__ */
{
  List_delAll (model, SCHEMA_FreeRDFResource);
}
