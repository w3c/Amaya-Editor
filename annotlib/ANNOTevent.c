/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1999.
 *  Please first read the full copyright statement in file COPYRIGHT.
 * 
 */

/*
 * ANNOTevent.c : interface module between Amaya and the annotation
 * code.
 *
 * Author: J. Kahan (W3C/INRIA)
 *
 * Acknowledgments: inspired from code written by Christophe Marjoline 
 *                  for the byzance collaborative work application
 */

#include "annotlib.h"
#include "AHTURLTools_f.h"
#include "ANNOTtools_f.h"
#include "XPointer.h"
#include "XPointer_f.h"
#include "XPointerparse_f.h"

#define DEFAULT_ALGAE_QUERY "w3c_algaeQuery=(ask '((?p ?s ?o)) :collect '(?p ?s ?o))"

/* some state variables */
static CHAR_T *annotUser; /* user id for saving the annotation */
static CHAR_T *annotDir;   /* directory where we're storing the annotations */
static List   *annotServers;   /* URL pointing to the annot server script */
static CHAR_T *annotPostServer; /* URL pointing to the annot server script */
static CHAR_T *annotMainIndex; /* index file where we give the correspondance
				between URLs and annotations */
static ThotBool annotAutoLoad; /* should annotations be downloaded
				  automatically? */
static ThotBool annotCustomQuery; /* use an algae custom query if TRUE */
static CHAR_T *annotAlgaeText;    /* the custom algae query text */

static Element last_selected_annotation; /* last selected annotation */

/* the structure used for storing the context of the 
   Annot_Raisesourcedoc_callback function */
typedef struct _RAISESOURCEDOC_context {
  CHAR_T *url;
  Document doc_annot;
} RAISESOURCEDOC_context;

/* the structure used for storing the context of the 
   RemoteLoad_callback function */
typedef struct _REMOTELOAD_context {
  CHAR_T *rdf_file;
  char *remoteAnnotIndex;
  char *localfile;
} REMOTELOAD_context;

/* the structure used for storing the context of the 
   RemoteLoad_callback function */
typedef struct _DELETE_context {
  Document source_doc;
  Document annot_doc;
  CHAR_T *annot_url;
  CHAR_T *output_file;
  Element annotEl;
  ThotBool annot_is_remote;
  AnnotMeta *annot;
} DELETE_context;

/*-----------------------------------------------------------------------
   GetAnnotCustomQuery
  -----------------------------------------------------------------------*/

#ifdef __STDC__
ThotBool GetAnnotCustomQuery (void)
#else /* __STDC__*/
ThotBool GetAnnotCustomQuery (void)
#endif /* __STDC__*/
{
  return annotCustomQuery;
}

/*-----------------------------------------------------------------------
   SetAnnotCustomQuery
  -----------------------------------------------------------------------*/
#ifdef __STDC__
void SetAnnotCustomQuery (ThotBool value)
#else /* __STDC__*/
void SetAnnotCustomQuery (value)
ThotBool value
#endif /* __STDC__*/
{
  annotCustomQuery = value;
}

/*-----------------------------------------------------------------------
   GetAnnotAlgaeText
  -----------------------------------------------------------------------*/

#ifdef __STDC__
CHAR_T *GetAnnotAlgaeText (void)
#else /* __STDC__*/
CHAR_T *GetAnnotAlgaeText (void)
#endif /* __STDC__*/
{
  return annotAlgaeText;
}

/*-----------------------------------------------------------------------
   SetAnnotAlgaeText
  -----------------------------------------------------------------------*/
#ifdef __STDC__
void SetAnnotAlgaeText (CHAR_T *value)
#else /* __STDC__*/
void SetAnnotAlgaeText (CHAR_T *value)
ThotBool value
#endif /* __STDC__*/
{
  if (!value && *value == WC_EOS)
    annotAlgaeText = NULL;
  else
    annotAlgaeText = TtaStrdup (value);
}

/*-----------------------------------------------------------------------
   GetAnnotUser
   Returns the directory where the annotations of a given document are stored.
  -----------------------------------------------------------------------*/

#ifdef __STDC__
CHAR_T *GetAnnotUser (void)
#else /* __STDC__*/
CHAR_T *GetAnnotUser (void)
#endif /* __STDC__*/
{
  return annotUser;
}

/*-----------------------------------------------------------------------
   GetAnnotServers
   Returns the URLs of the annotation servers
  -----------------------------------------------------------------------*/

#ifdef __STDC__
List *GetAnnotServers (void)
#else /* __STDC__*/
List *GetAnnotServers (void)
#endif /* __STDC__*/
{
  return annotServers;
}

/*-----------------------------------------------------------------------
   GetAnnotPostServer
   Returns the URLs of the annotation Post server
  -----------------------------------------------------------------------*/

#ifdef __STDC__
CHAR_T *GetAnnotPostServer (void)
#else /* __STDC__*/
CHAR_T *GetAnnotPostServer (void)
#endif /* __STDC__*/
{
  return annotPostServer;
}

/*-----------------------------------------------------------------------
   Procedure GetAnnotMainIndex
  -----------------------------------------------------------------------
   Returns the directory where the annotations of a given document are stored.
  -----------------------------------------------------------------------*/

#ifdef __STDC__
CHAR_T *GetAnnotMainIndex (void)
#else /* __STDC__*/
CHAR_T *GetAnnotationMainIndex(void)
#endif /* __STDC__*/
{
  return annotMainIndex;
}

/*-----------------------------------------------------------------------
   Procedure GetAnnotDir
  -----------------------------------------------------------------------
   Returns the directory where the annotations of a given document are stored.
  -----------------------------------------------------------------------*/

#ifdef __STDC__
CHAR_T *GetAnnotDir (void)
#else /* __STDC__*/
CHAR_T *GetAnnotDir (void)
#endif /* __STDC__*/
{
  return annotDir;
}

/*-----------------------------------------------------------------------
   Procedure GetAnnotDir
  -----------------------------------------------------------------------
   Returns the directory where the annotations of a given document are stored.
  -----------------------------------------------------------------------*/

#ifdef __STDC__
List *CopyAnnotServers (CHAR_T *server_list)
#else /* __STDC__*/
List *CopyAnnotServers (CHAR_T *server_list)
#endif /* __STDC__*/
{
  List *me = NULL;
  CHAR_T *server;
  CHAR_T *ptr;
  CHAR_T *scratch;


  if (!server_list || *server_list == WC_EOS)
    return NULL;

  /* make a copy we can modify */
  scratch = TtaWCSdup (server_list);
  ptr = scratch;
  while (*ptr != WC_EOS)
    {
      server = ptr;
      while (*ptr != TEXT(' ') && *ptr != WC_EOS)
	ptr++;
      if (*ptr == TEXT(' '))
	{
	  *ptr = WC_EOS;
	  ptr++;
	}
      List_add (&me, TtaWCSdup (server));
    }
  TtaFreeMemory (scratch);
  return me;
}
/*-----------------------------------------------------------------------
   CopyAlgaeTemplateURL
   Prepares a query URL using the algae text template. Any %u will be
   substituted with the url given in the parameter.
  -----------------------------------------------------------------------*/

#ifdef __STDC__
static void CopyAlgaeTemplateURL (CHAR_T **dest, CHAR_T *proto, CHAR_T *url)
#else /* __STDC__*/
static void CopyAlgaeTemplateURL (dest, proto, url)
CHAR_T **dest;
CHAR_T *proto;
CHAR_T *url;
#endif /* __STDC__*/
{
  CHAR_T *in;
  CHAR_T *out;
  CHAR_T *tmp;
  int proto_len;
  int url_len;
  int i;

  proto_len = (proto) ? ustrlen (proto) : 0;
  url_len = (url) ? ustrlen (url) : 0;

  /* allocate enough memory in the string */
  i = 0;
  in = annotAlgaeText;
  while (in)
    {
      tmp = ustrstr (in, TEXT("%u"));
      if (tmp)
	{
	  i++;
	  in = tmp + 2;
	}
      else
	break;
    }
  *dest = TtaGetMemory (i * (ustrlen (proto) + ustrlen (url))
			+ ustrlen (annotAlgaeText)
			+ 30);
  in = annotAlgaeText;
  out = *dest;
  while (*in != WC_EOS)
    {
      if (*in == TEXT('%') && *(in + 1) == TEXT('u'))
	{
	  /* copy the proto and the URL */
	  usprintf (out, "%s%s", proto, url);
	  /* and update the pointers */
	  out = out + proto_len + url_len;
	  in = in + 2;
	}
      else
	{
	  *out = *in;
	  in++;
	  out++;
	}
    }
  *out = WC_EOS;
}

/*-----------------------------------------------------------------------
   Procedure ANNOT_Init
  -----------------------------------------------------------------------
  -----------------------------------------------------------------------*/

#ifdef __STDC__
void ANNOT_Init ()
#else /* __STDC__*/
void ANNOT_Init ()
#endif /* __STDC__*/
{
  CHAR_T *tmp;

  /* setup the default registry values */
  tmp = TtaGetEnvString ("APP_HOME");
  annotDir = TtaGetMemory (ustrlen (tmp) + ustrlen (ANNOT_DIR) + 2);
  usprintf (annotDir, TEXT("%s%c%s"), tmp, DIR_SEP, ANNOT_DIR);
  TtaSetEnvString ("ANNOT_DIR", annotDir, FALSE);
  TtaFreeMemory (annotDir);
  TtaSetEnvString ("ANNOT_MAIN_INDEX", ANNOT_MAIN_INDEX, FALSE);
  TtaSetEnvString ("ANNOT_USER", ANNOT_USER, FALSE);
  TtaSetEnvString ("ANNOT_AUTOLOAD", TEXT("yes"), FALSE);

  /* initialize the annot global variables */
  annotDir = TtaWCSdup (TtaGetEnvString ("ANNOT_DIR"));
  annotMainIndex = TtaWCSdup (TtaGetEnvString ("ANNOT_MAIN_INDEX"));
  annotUser = TtaWCSdup (TtaGetEnvString ("ANNOT_USER"));
  annotAutoLoad = !ustrcasecmp (TtaGetEnvString ("ANNOT_AUTOLOAD"), "yes");
  tmp = TtaGetEnvString ("ANNOT_SERVERS");
  if (tmp)
    annotServers = CopyAnnotServers (tmp);
  else
    annotServers = NULL;
  tmp = TtaGetEnvString ("ANNOT_POST_SERVER");
  if (tmp)
    annotPostServer = TtaWCSdup (tmp);
  else
    annotPostServer = TtaWCSdup (TEXT("localhost"));

  /* @@@ temporary custom query, as we could use the configuration menu  ***/
  annotCustomQuery = FALSE;
  annotAlgaeText = TtaStrdup (DEFAULT_ALGAE_QUERY);

  /* create the directory where we'll store the annotations if it
     doesn't exist */
  TtaMakeDirectory (annotDir);
}

/*-----------------------------------------------------------------------
   ANNOT_Quit
  -----------------------------------------------------------------------
  -----------------------------------------------------------------------*/

#ifdef __STDC__
void ANNOT_Quit ()
#else /* __STDC__*/
void ANNOT_Quit ()
#endif /* __STDC__*/
{
  if (annotDir)
    TtaFreeMemory (annotDir);
  if (annotMainIndex)
    TtaFreeMemory (annotMainIndex);
  if (annotUser)
    TtaFreeMemory (annotUser);
  /* remove all the annotServers */
  if (annotServers)  
      List_delAll (&annotServers, List_delCharObj);
  if (annotPostServer)  
    TtaFreeMemory (annotPostServer);
  if (annotAlgaeText)
    TtaFreeMemory (annotAlgaeText);
}

/*-----------------------------------------------------------------------
   ANNOT_FreeDocumentResource
   Frees all the annotation resources that are associated with
   Document doc
  -----------------------------------------------------------------------*/

#ifdef __STDC__
void ANNOT_FreeDocumentResource (Document doc)
#else /* __STDC__*/
void ANNOT_FreeDocumentResource (doc)
Document doc;
#endif /* __STDC__*/
{
  int i;

  /* stop the on-going requests */

  /* close all the open annotation windows, asking the user if he 
   wants to save them */
  for (i = 1; i < DocumentTableLength; i++)
    {
      if (doc == i)
	continue;
      if (DocumentURLs[i] && DocumentTypes[i] == docAnnot
	  && DocumentMeta[i]->source_doc == doc)
	{
	  /* insist, until the user saves the annotation.
	     It'd be nice to keep the annotation open, even if the
	     doc disappears
	  */
	  while (DocumentURLs[i])
	    CloseDocument (i, 1);
	}
    }
  /* free the allocated memory */
  LINK_DelMetaFromMemory (doc);
  /* reset the state */
  AnnotMetaData[doc].local_annot_loaded = FALSE;
}


/*-----------------------------------------------------------------------
   ANNOT_AutoLoad
  -----------------------------------------------------------------------
  -----------------------------------------------------------------------*/

#ifdef __STDC__
void ANNOT_AutoLoad (Document doc, View view)
#else /* __STDC__*/
void ANNOT_AutoLoad (doc, view)
Document doc;
View view;
#endif /* __STDC__*/
{
  if (!annotAutoLoad)
    return;

  ANNOT_Load (doc, view);
}

/*-----------------------------------------------------------------------
  RemoteLoad_callback
  -----------------------------------------------------------------------
  -----------------------------------------------------------------------*/
#ifdef __STDC__
void               RemoteLoad_callback (int doc, int status, 
					 CHAR_T *urlName,
					 CHAR_T *outputfile, 
					 AHTHeaders *http_headers,
					 void * context)
#else  /* __STDC__ */
void               RemoteLoad_callback (doc, status, urlName,
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
   REMOTELOAD_context *ctx;

   /* restore REMOTELOAD contextext's */  
   ctx = (REMOTELOAD_context *) context;

   if (!ctx)
     return;

   if (status == HT_OK)
     LINK_LoadAnnotationIndex (doc, ctx->remoteAnnotIndex, TRUE);

   TtaFreeMemory (ctx->remoteAnnotIndex);
   TtaFreeMemory (ctx);
   /* clear the status line if there was no error*/
   if (!status)
     TtaSetStatus (doc, 1,  TEXT(""), NULL);
}

/*-----------------------------------------------------------------------
   Procedure ANNOT_Load
  -----------------------------------------------------------------------
  -----------------------------------------------------------------------*/

#ifdef __STDC__
void ANNOT_Load (Document doc, View view)
#else /* __STDC__*/
void ANNOT_Load (doc, view)
Document doc;
View view;
#endif /* __STDC__*/
{
  char *annotIndex;
  char *annotURL;
  char *proto;
  REMOTELOAD_context *ctx;
  int res;
  List *ptr;
  CHAR_T *server;
  static ThotBool init = FALSE;

  if (!init)
    {
      /* @@ unfinished; this is temporary while the code is raw @@ */
      ANNOT_ReadSchema (doc, "http://www.w3.org/1999/xx/annotation-ns#");
      init = TRUE;
    }

  /* don't annotate other annotations, text only documents, or
     graphic documents */
  if (!ANNOT_CanAnnotate(doc))
    return;

  /*
   * Parsing test!
  */

  /*
   * load the local annotations if there's no annotserver or if
   * annotServers include the localhost
   */
  if (!annotServers || AnnotList_search (annotServers, TEXT("localhost")))
    {
      annotIndex = LINK_GetAnnotationIndexFile (DocumentURLs[doc]);
      LINK_LoadAnnotationIndex (doc, annotIndex, TRUE);
      TtaFreeMemory (annotIndex);
      AnnotMetaData[doc].local_annot_loaded = TRUE;
    }

  /* 
   * Query each annotation server for annotations related to this 
   * document
   */
  if (annotServers) 
    {
      /* load the annotations, server by server */
      ptr = annotServers;
      while (ptr)
	{
	  server = ptr->object;
	  ptr = ptr->next;
	  if (!ustrcasecmp (server, TEXT("localhost")))
	    continue;
	  /* create the context for the callback */
	  ctx = TtaGetMemory (sizeof (REMOTELOAD_context));
	  /* make some space to store the remote file name */
	  ctx->remoteAnnotIndex = TtaGetMemory (MAX_LENGTH);
	  /* "compute" the url we're looking up in the annotation server */
	  if (!IsW3Path (DocumentURLs[doc]) &&
	      !IsFilePath (DocumentURLs[doc]))
	    proto = TEXT("file://");
	  else
	    proto = TEXT("");
	  if (!annotCustomQuery || !annotAlgaeText || 
	      annotAlgaeText[0] == WC_EOS)
	    {
	      annotURL = TtaGetMemory (ustrlen (DocumentURLs[doc])
				       + ustrlen (proto)
				       + sizeof (TEXT("w3c_annotates="))
				       + 50);
	      sprintf (annotURL, "w3c_annotates=%s%s", proto, 
		       DocumentURLs[doc]);
	    }
	  else
	    /* substitute the %u for DocumentURLs[doc] and go for it! */
	    /* @@ we have a potential mem bug here as I'm not computing
	       the exact size */
	    CopyAlgaeTemplateURL (&annotURL, proto, DocumentURLs[doc]);

	  /* launch the request */
	  res = GetObjectWWW (doc,
			      server,
			      annotURL,
			      ctx->remoteAnnotIndex,
			      AMAYA_ASYNC | AMAYA_FLUSH_REQUEST,
			      NULL,
			      NULL, 
			      (void *)  RemoteLoad_callback,
			      (void *) ctx,
			      NO,
			      TEXT("application/rdf"));
	  TtaFreeMemory (annotURL);
	}
    }
}

/*-----------------------------------------------------------------------
  ANNOT_Create
  -----------------------------------------------------------------------
   Creates an annotation on the selected text. If there's no selected
   text, it doesn't do anything.
  -----------------------------------------------------------------------*/

#ifdef __STDC__
void ANNOT_Create (Document doc, View view)
#else /* __STDC__*/
void ANNOT_Create (doc, view)
     Document doc;
     View view;
#endif /* __STDC__*/
{
  Element     first, last;
  int         c1, cl, i;
  Document    doc_annot;
  AnnotMeta  *annot;

  if (!ANNOT_CanAnnotate (doc))
    return;

  TtaGiveFirstSelectedElement (doc, &first, &c1, &i);
  TtaGiveLastSelectedElement (doc, &last, &i, &cl);

  if (first == NULL)
    return;  /* Error: there's no selected zone */

  /* create the document that will store the annotation */
  if ((doc_annot = ANNOT_NewDocument (doc)) == 0)
    return;

  annot = LINK_CreateMeta (doc, doc_annot);

  ANNOT_InitDocumentStructure (doc, doc_annot, annot);

  LINK_AddLinkToSource (doc, annot);
  LINK_SaveLink (doc);
}

/*-----------------------------------------------------------------------
   Procedure ANNOT_Post_callback
  -----------------------------------------------------------------------
  -----------------------------------------------------------------------*/
#ifdef __STDC__
void               ANNOT_Post_callback (int doc, int status, 
					 CHAR_T *urlName,
					 CHAR_T *outputfile, 
					 AHTHeaders *http_headers,
					 void * context)
#else  /* __STDC__ */
void               ANNOT_Post_callback (doc, status, urlName,
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
  /* get the server's reply */
  /* update the source doc link */
  /* delete the temporary file */
  /* the context gives the link's name and we'll use it to look up the
     document ... */

   REMOTELOAD_context *ctx;

   /* restore REMOTELOAD contextext's */  
   ctx = (REMOTELOAD_context *) context;

   if (!ctx)
     return;

   if (status == HT_OK)
     {
       int source_doc = DocumentMeta[doc]->source_doc;
       AnnotMeta *annot = GetMetaData (source_doc, doc);

#if 0 /* @@ RRS - interim idea for updating the annotation.  Doesn't work
	 unless the POST reply returns all the properties */
       if (annot)
	 {
	   LINK_RemoveLinkFromSource (source_doc, annot);
	   List_rem (AnnotMetaDataList[source_doc], annot);
	   Annot_free (annot);
	 }
       LINK_LoadAnnotationIndex (doc, ctx->remoteAnnotIndex, TRUE);
       ANNOT_LoadAnnotation (source_doc, doc);
#endif /*0*/
       
       /* Using the server's metadata answer to update our local metadata
	  and the reverse links that point to the annotation */
       /* @@ needs more work to handle error return codes */
       if (annot)
	 {
	   List *listP = RDF_parseFile (ctx->remoteAnnotIndex, ANNOT_LIST);
	   if (listP)
	     {
	       AnnotMeta *returned_annot = (AnnotMeta *)listP->object;
	       if (returned_annot->source_url
		   && strcmp(returned_annot->source_url, annot->source_url))
		 fprintf (stderr, "PostCallback: POST returned an annotation for a different source: %s vs %s\n",
			  returned_annot->source_url, annot->source_url);
	       if (returned_annot->annot_url)
		 {
		   TtaFreeMemory(annot->annot_url);
		   annot->annot_url = returned_annot->annot_url;
		   returned_annot->annot_url = NULL;
		 }
	       /* @@ we should unlink the previous file only if we posted a 
		  local file to a server, and if it was a success. Should we
		  close the annotation window and open it up again? */
	       if (returned_annot->body_url) 
		 {
		   /* update the anchor in the source doc */
		   ReplaceLinkToAnnotation (source_doc, annot->name, 
					    returned_annot->body_url);
		   /* unlink the previous body */
		   TtaFileUnlink (annot->body_url);
		   TtaFreeMemory(annot->body_url);
		   /* update the metadata of the annotation */
		   annot->body_url = returned_annot->body_url;
		   returned_annot->body_url = NULL;
		   /* update the Document metadata to point to the new
		      body too */
		   TtaFreeMemory (DocumentURLs[doc]);
		   DocumentURLs[doc] = TtaStrdup (annot->body_url);
		 }
	       if (listP->next)
		 fprintf (stderr, "PostCallback: POST returned more than one annotation\n");
	       AnnotList_free (listP);
	     }
	 }
       /* if we were posting a localfile, remove this file (and update the
	indexes */
       if (ctx->localfile)
	 {
	   TtaFileUnlink (ctx->localfile);
	   TtaFreeMemory (ctx->localfile);
	 }
       TtaFileUnlink (ctx->remoteAnnotIndex);
     }

   /* erase the rdf container */
   TtaFileUnlink (ctx->rdf_file);

   /* free all memory associated with the context */
   TtaFreeMemory (ctx->rdf_file);
   TtaFreeMemory (ctx->remoteAnnotIndex);
   TtaFreeMemory (ctx);
}

/*-----------------------------------------------------------------------
   Procedure ANNOT_Post
  -----------------------------------------------------------------------
  -----------------------------------------------------------------------*/

#ifdef __STDC__
void ANNOT_Post (Document doc, View view)
#else /* __STDC__*/
void ANNOT_Post (doc, view)
Document doc;
View view;
#endif /* __STDC__*/
{
  REMOTELOAD_context *ctx;
  int res;
  CHAR_T *rdf_file;
  CHAR_T *url;
  ThotBool free_url;
  AnnotMeta *annot;
  Document source_doc;

  /* @@ JK: while the post item isn't desactivated on the main window,
     forbid annotations from elsewhere */
  if (ustrcmp (TtaGetSSchemaName (TtaGetDocumentSSchema (doc)), TEXT("Annot")))
    return;

  if (!annotPostServer 
      || *annotPostServer == EOS)
    return;
  
  /* create the RDF container */
  rdf_file = ANNOT_PreparePostBody (doc);
  if (!rdf_file)
    /* there was an error while preparing the tmp.rdf file */
    return;
  /* create the context for the callback */
  ctx = TtaGetMemory (sizeof (REMOTELOAD_context));
  /* make some space to store the remote file name */
  ctx->remoteAnnotIndex = TtaGetMemory (MAX_LENGTH);
  /* store the temporary filename */
  ctx->rdf_file = rdf_file;
  /* copy the name of the localfile if it's a local document */
  if (!IsFilePath (DocumentURLs[doc]))
      ctx->localfile = TtaStrdup (DocumentURLs[doc]);
  else
    ctx->localfile = NULL;

  /* compute the URL */
  if (IsW3Path (DocumentURLs[doc]))
    {
      /* find the annotation metadata that corresponds to this annotation */
      source_doc = DocumentMeta[doc]->source_doc;
      annot = AnnotList_searchAnnot (AnnotMetaData[source_doc].annotations,
				     DocumentURLs[doc], AM_BODY_URL);

      if (!annot)
	/* @@ JK: give some error message */
	return;

      /* we're saving a modification to an existing annotation */
      url = TtaGetMemory (ustrlen (annotPostServer)
			  + sizeof (TEXT("?replace_source="))
			  + ustrlen (annot->annot_url)
			  + sizeof (TEXT("&rdftype="))
			  + sizeof (ANNOTATION_PROP)
			  + 1);
      usprintf (url,"%s?replace_source=%s&rdftype=%s",
		annotPostServer,
		annot->annot_url,
		ANNOTATION_PROP);
      free_url = TRUE;
    }
  else
    {
      /* we're saving a new annotation */
      url = annotPostServer;
      free_url = FALSE;
    }

  /* launch the request */
  res = GetObjectWWW (doc,
		      url,
		      rdf_file,
		      ctx->remoteAnnotIndex,
		      AMAYA_FILE_POST | AMAYA_ASYNC | AMAYA_FLUSH_REQUEST,
		      NULL,
		      NULL, 
		      (void *)  ANNOT_Post_callback,
		      (void *) ctx,
		      NO,
		      NULL);
  if (free_url)
    TtaFreeMemory (url);
  /* @@ here we should delete the context or call the callback in case of
     error */
  if (res)
    fprintf (stderr, "ANNOT_Post: Failed to post the annotation!\n");
}

/*----------------------------------------------------------------------
  ANNOT_Save
  Frontend function that decides if an annotation should be saved and if
  it should be saved remotely or locally. It then calls the appropriate 
  function to do this operation.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ANNOT_SaveDocument (Document doc_annot, View view)
#else  /* __STDC__ */
void                ANNOT_SaveDocument (doc_annot, view)
Document            doc_annot;
View                view;

#endif /* __STDC__ */
{
  CHAR_T *filename;

  if (!TtaIsDocumentModified (doc_annot))
    return; /* prevent Thot from performing normal save operation */

  if (IsW3Path (DocumentURLs[doc_annot]))
    ANNOT_Post (doc_annot, view);
  else
    {
      /* save the file */
      /* we skip the file: prefix if it's present */
      filename = TtaGetMemory (ustrlen (DocumentURLs[doc_annot]) + 1);
      NormalizeFile (DocumentURLs[doc_annot], filename, AM_CONV_ALL);
      if (ANNOT_LocalSave (doc_annot, filename))
	TtaSetDocumentUnmodified (doc_annot);
      TtaFreeMemory (filename); 
    }
}

/*-----------------------------------------------------------------------
  ANNOT_SelectSourceDoc
  If the user clicked on an annotation link in the source document,
  it highlights the annotated text.
  -----------------------------------------------------------------------*/
#ifdef __STDC__
void               ANNOT_SelectSourceDoc (int doc, Element el)
#else  /* __STDC__ */
void               ANNOT_SelectSourceDoc (doc, el)
int doc;
Element el;

#endif
{
  ElementType      elType;
  AttributeType    attrType;
  Attribute	   attr;
  int              length;
  CHAR_T          *annot_url;

  /* reset the last selected annotation ptr */
  last_selected_annotation = NULL;

  /* is it an annotation link? */
  elType = TtaGetElementType (el);
  if (ustrcmp (TtaGetSSchemaName (elType.ElSSchema), TEXT("XLink"))
      || (elType.ElTypeNum != XLink_EL_XLink))
    return;

  /* memorize the last selected annotation */
  last_selected_annotation = el;

  /* get the URL of the annotation body */
  attrType.AttrSSchema = elType.ElSSchema;
  attrType.AttrTypeNum = XLink_ATTR_href_;
  attr = TtaGetAttribute (el, attrType);
  if (!attr)
    return;
  length = TtaGetTextAttributeLength (attr);
  length++;
  annot_url = TtaGetMemory (length);
  TtaGiveTextAttributeValue (attr, annot_url, &length);

  /* select the annotated text */
  LINK_SelectSourceDoc (doc, annot_url);

  /* memorize the last selected annotation */
  last_selected_annotation = el;

  TtaFreeMemory (annot_url);
}

/*-----------------------------------------------------------------------
  RaiseSourceDoc_callback
  -----------------------------------------------------------------------*/
#ifdef __STDC__
void               Annot_RaiseSourceDoc_callback (int doc, int status, 
						 CHAR_T *urlName,
						 CHAR_T *outputfile, 
						 AHTHeaders *http_headers,
						 void * context)
#else  /* __STDC__ */
void               Annot_RaiseSourceDoc_callback (doc, status, urlName,
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
   RAISESOURCEDOC_context *ctx;

   /* restore REMOTELOAD contextext's */  
   ctx = (RAISESOURCEDOC_context *) context;

   if (!ctx)
     return;

   /* select the source of the annotation */
   if (ctx->doc_annot)
     LINK_SelectSourceDoc (doc, DocumentURLs[ctx->doc_annot]);

   TtaFreeMemory (ctx->url);
   TtaFreeMemory (ctx);
}

/*-----------------------------------------------------------------------
  Annot_Raisesourcedoc
  The user has double clicked on the annot link to the source document
  -----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool Annot_RaiseSourceDoc (NotifyElement *event)
#else
ThotBool Annot_RaiseSourceDoc (event)
NotifyElement *event;
#endif /* __STDC__ */
{
  Element          el;
  Document         doc_annot;
  AttributeType    attrType;
  Attribute	   HrefAttr;
  ThotBool	   docModified;
  int              length;
  Document         targetDocument;
  CHAR_T          *url;
  RAISESOURCEDOC_context *ctx;

  /* initialize from the context */
  el = event->element;
  doc_annot = event->document;
  docModified = TtaIsDocumentModified (doc_annot);
  /* remove the selection */
  TtaUnselect (doc_annot);
  /* 
  ** get the source document URL 
  */
  attrType.AttrSSchema = TtaGetDocumentSSchema (doc_annot);
  attrType.AttrTypeNum = Annot_ATTR_HREF_;
  HrefAttr = TtaGetAttribute (el, attrType);
  if (HrefAttr != NULL)
    {
      length = TtaGetTextAttributeLength (HrefAttr);
      length++;
      url = TtaGetMemory (length);
      TtaGiveTextAttributeValue (HrefAttr, url, &length);
    }
  if (!docModified)
    {
      TtaSetDocumentUnmodified (doc_annot);
      DocStatusUpdate (doc_annot, docModified);
    }
  
  /* @@ and now jump to the annotated document and put it on top,
     jump to the anchor... and if the document isn't there, download it? */
  ctx = TtaGetMemory (sizeof (REMOTELOAD_context));
  ctx->url = url;
  ctx->doc_annot = doc_annot;
  targetDocument = GetHTMLDocument (url, NULL,
				    doc_annot, 
				    doc_annot, 
				    CE_ABSOLUTE, FALSE, 
				    (void *) Annot_RaiseSourceDoc_callback,
				    (void *) ctx);

  /* don't let Thot perform the normal operation */
  return TRUE;
}

/*----------------------------------------------------------------------
  Annot_ShowTypes
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool     Annot_Types (NotifyElement *event)
#else
ThotBool     Annot_Types (event)
NotifyElement *event;
#endif /* __STDC__*/
{
  Element          el;
  Document         doc_annot;
  ThotBool         docModified;
  RDFResourceP     new_type;

  /* initialize from the context */
  el = event->element;
  doc_annot = event->document;

  docModified = TtaIsDocumentModified (doc_annot);

  new_type = AnnotTypes2 (doc_annot, 1);
  if (new_type)
    ANNOT_SetType (doc_annot, new_type);

  /* change the annotation type according to what was returned by
     AnnotShowTypes, maybe call it AnnotSelectTypes */
  
  /* remove the selection */
  TtaUnselect (doc_annot);
  if (!docModified)
    {
      if (new_type)
	{
	  TtaSetDocumentModified (doc_annot);
	  DocStatusUpdate (doc_annot, TRUE);
	}
      else
	TtaSetDocumentUnmodified (doc_annot);
    }

  return (TRUE);
}

/*-----------------------------------------------------------------------
  ANNOT_Delete_callback
  -----------------------------------------------------------------------
  -----------------------------------------------------------------------*/
#ifdef __STDC__
void               ANNOT_Delete_callback (int doc, int status, 
					 CHAR_T *urlName,
					 CHAR_T *outputfile, 
					 AHTHeaders *http_headers,
					 void * context)
#else  /* __STDC__ */
void               ANNOT_Delete_callback (doc, status, urlName,
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
   DELETE_context *ctx;
   Document source_doc;
   Document annot_doc;
   CHAR_T  *annot_url;
   CHAR_T  *output_file;
   Element  annotEl;
   AnnotMeta *annot;
   ThotBool annot_is_remote;

   /* restore REMOTELOAD contextext's */  
   ctx = (DELETE_context *) context;

   if (!ctx)
     return;
   
   source_doc = ctx->source_doc;
   annot_doc = ctx->annot_doc;
   annot_url = ctx->annot_url;
   annotEl = ctx->annotEl;
   annot_is_remote = ctx->annot_is_remote;
   output_file = ctx->output_file;
   annot = ctx->annot;

   if (status == HT_OK)
     {
       /* remove the annotation link in the source document */
       LINK_RemoveLinkFromSource (source_doc, annotEl);
       /* remove the annotation from the filters */
       AnnotFilter_delete (&(AnnotMetaData[source_doc].authors), annot);
       AnnotFilter_delete (&(AnnotMetaData[source_doc].types), annot);
       AnnotFilter_delete (&(AnnotMetaData[source_doc].servers), annot);
       
       /* remove the annotation from the document's annotation list and 
	  update it */
       AnnotList_delAnnot (&(AnnotMetaData[source_doc].annotations),
			   annot_url, FALSE);

       /* update the annotation index or delete it if it's empty */
       if (AnnotList_localCount (AnnotMetaData[source_doc].annotations) > 0)
	 LINK_SaveLink (source_doc);
       else
	 LINK_DeleteLink (source_doc);

       /* close the annotation window if it was open */
       /* @@ JK: check if the user didn't close it in the meantime */
       if (annot_doc)
	 {
	   TtaSetDocumentUnmodified (annot_doc);
	   /* we should add all the views */
	   TtaCloseDocument (annot_doc);
	 }
     }
   
   TtaFreeMemory (annot_url);
   if (output_file)
     {
       if (*output_file != EOS)
	 TtaFileUnlink (output_file);
       TtaFreeMemory (output_file);
     }
   TtaFreeMemory (ctx);
   /* clear the status line if there was no error*/
   if (status == HT_OK && doc == source_doc)
     TtaSetStatus (doc, 1,  TEXT("Annotation deleted!"), NULL);
}

/*----------------------------------------------------------------------
  ANNOT_Delete 
  Erases one annotation
 -----------------------------------------------------------------------*/

#ifdef __STDC__
void ANNOT_Delete (Document doc, View view)
#else /* __STDC__*/
void ANNOT_Delete (document, view)
     Document doc;
     View view;
#endif /* __STDC__*/
{
  Document         source_doc, annot_doc;
  ElementType      elType;
  Element          annotEl;
  AnnotMeta       *annot;
  AttributeType    attrType;
  Attribute	   attr;
  CHAR_T          *annot_url;
  CHAR_T          *annot_server;
  CHAR_T          *form_data;
  CHAR_T          *char_ptr;
  List            *list_ptr;
  int              i;
  int              res;
  ThotBool         annot_is_remote;
  DELETE_context *ctx;

  /* maybe detect if the user just clicked on the annotation */
  /* e.g, if the annot_doc is not open */

  /* 
  **  get the annotation URL, source_doc, and annot_doc
  */
  if (DocumentTypes[doc] == docAnnot)
    {
      /* delete from an annotation document */

      /* clear the status */
      last_selected_annotation =  NULL;
      annot_doc = doc;
      source_doc = DocumentMeta[doc]->source_doc;
  
      /* make the body */
      if (IsW3Path (DocumentURLs[doc]))
	{
	  /* it's a remote annotation */
	  annot_is_remote = TRUE;
	  if (DocumentMeta[doc]->form_data)
	    {
	      annot_url = TtaGetMemory (ustrlen (DocumentURLs[doc])
				       + ustrlen (DocumentMeta[doc]->form_data)
					+ sizeof (TEXT("?"))
					+ 1);
	      usprintf (annot_url, "%s?%s", DocumentURLs[doc], 
			DocumentMeta[doc]->form_data);
	    }
	  else
	    annot_url = TtaStrdup (DocumentURLs[doc]);
	}
      else 
	{
	  /* it's a local annotation */
	  annot_is_remote = FALSE;
	  annot_url = ANNOT_MakeFileURL (DocumentURLs[doc]);
	}

      /* find the annotation link in the source document that corresponds
	 to this annotation */
      annot = AnnotList_searchAnnot (AnnotMetaData[source_doc].annotations,
				     annot_url, AM_BODY_URL);
      if (!annot)
	{
	  TtaFreeMemory (annot_url);
	  /* signal some error */
	  return;
	}
      annotEl = SearchAnnotation (source_doc, annot->name);
    }
  else
    {
      /* delete from the source doc */

      source_doc = doc;

      /* verify if the user has selected an annotation link */
      if (!last_selected_annotation)
	return;

      annotEl = last_selected_annotation;

      /* is it an annotation link? */
      elType = TtaGetElementType (annotEl);
      if (ustrcmp (TtaGetSSchemaName (elType.ElSSchema), TEXT("XLink"))
	  || (elType.ElTypeNum != XLink_EL_XLink))
	return;
      
      /* get the annotation URL */
      attrType.AttrSSchema = elType.ElSSchema;
      attrType.AttrTypeNum = XLink_ATTR_href_;
      attr = TtaGetAttribute (annotEl, attrType);
      if (!attr)
	return;
      i = TtaGetTextAttributeLength (attr);
      i++;
      annot_url = TtaGetMemory (i);
      TtaGiveTextAttributeValue (attr, annot_url, &i);

      /* @@ JK: I may need to split the url and get the separate form_data */
      if (IsFilePath (annot_url))
	{
	  annot_is_remote = FALSE;
	  char_ptr = TtaGetMemory (ustrlen (annot_url));
	  NormalizeFile (annot_url, char_ptr, AM_CONV_NONE);
	  annot_doc = IsDocumentLoaded (char_ptr, NULL);
	  TtaFreeMemory (char_ptr);
	}
      else
	{
	  annot_is_remote = TRUE;
	  annot_doc = IsDocumentLoaded (annot_url, NULL);
	}

      /* find the annotation metadata that corresponds to this annotation */
      annot = AnnotList_searchAnnot (AnnotMetaData[source_doc].annotations,
				     annot_url, AM_BODY_URL);
    }

  ctx = (DELETE_context *) TtaGetMemory (sizeof (DELETE_context));
  ctx->source_doc = source_doc;
  ctx->annot_doc = annot_doc;
  ctx->annot_url = annot_url;
  ctx->annotEl = annotEl;
  ctx->annot = annot;
  ctx->annot_is_remote = annot_is_remote;

  if (annot_is_remote)
    /* make some space to store any output file that the server would send */
    ctx->output_file = TtaGetMemory (MAX_LENGTH + 1);
  else
    ctx->output_file = NULL;

  if (annot_is_remote)
    {
      /* do the post call */

      /* find the annotation server by comparition */
      list_ptr = annotServers;
      annot_server = NULL;
      while (list_ptr)
	{
	  annot_server = list_ptr->object;
	  if (!ustrcasecmp (annot_server, TEXT("localhost")))
	    continue;
	  if (!ustrncasecmp (annot_server, annot_url, ustrlen (annot_server)))
	    break;
	  list_ptr = list_ptr->next;
	}

      if (annot_server)
	{
	  /* compute the form_data */
	  form_data = TtaGetMemory  (sizeof (TEXT("delete_source="))
				     + ustrlen (annot->annot_url)
				     + sizeof (TEXT("&rdftype="))
				     + sizeof (ANNOTATION_PROP)
				     + 1);
	  usprintf (form_data,
		    "delete_source=%s&rdftype=%s", annot->annot_url, ANNOTATION_PROP);
	  /* launch the request */
	  res = GetObjectWWW (doc,
			      annot_server,
			      form_data,
			      ctx->output_file,
			      AMAYA_ASYNC | AMAYA_FORM_POST | AMAYA_FLUSH_REQUEST,
			      NULL,
			      NULL, 
			      (void *)  ANNOT_Delete_callback,
			      (void *) ctx,
			      NO,
			      NULL);
	  TtaFreeMemory (form_data);
	  /* do something with res in case of error (invoke the callback? */
	}
      else
	{
	  /* invoke the callback with an error */
	  ANNOT_Delete_callback (doc, HT_ERROR, NULL, NULL, NULL,
				 (void *) ctx);
	}
    }
  else
    {
      /* invoke the callback */
      ANNOT_Delete_callback (doc, HT_OK, NULL, NULL, NULL, (void *) ctx);
    }
  /* @@ JK: Todo rename the function to LINK_SaveIndex, as that's what it's
     doing */
}
