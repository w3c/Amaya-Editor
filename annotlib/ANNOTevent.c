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


/* annotlib includes */
#include "annotlib.h"
#include "ANNOTevent_f.h"
#include "ANNOTtools_f.h"
#include "ANNOTschemas_f.h"
#include "ANNOTlink_f.h"
#include "ANNOTfiles_f.h"
#include "ANNOTmenu_f.h"
#include "AHTrdf2annot_f.h"

/* Amaya includes */
#include "init_f.h"
#include "HTMLactions_f.h"
#include "AHTURLTools_f.h"
#include "query_f.h"
#include "XLinkedit_f.h"
#include "XPointer.h"
#include "XPointer_f.h"
#include "XPointerparse_f.h"

/* schema includes */
#include "Annot.h"
#include "XLink.h"

#define DEFAULT_ALGAE_QUERY "w3c_algaeQuery=(ask '((?p ?s ?o)) :collect '(?p ?s ?o))"

/* some state variables */
static CHAR_T *annotUser; /* user id for saving the annotation */
static CHAR_T *annotDir;   /* directory where we're storing the annotations */
static List   *annotServers;   /* URL pointing to the annot server script */
static CHAR_T *annotPostServer; /* URL pointing to the annot server script */
static CHAR_T *annotMainIndex; /* index file where we give the correspondance
				between URLs and annotations */
static ThotBool annotLAutoLoad; /* should local annotations be downloaded
				   automatically? */
static ThotBool annotRAutoLoad; /* should remote annotations be downloaded
				   automatically? */
static ThotBool annotCustomQuery; /* use an algae custom query if TRUE */
static CHAR_T *annotAlgaeText;    /* the custom algae query text */

/* last selected annotation */
static Element last_selected_annotation[DocumentTableLength];

static ThotBool schema_init = FALSE;

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
  Document source_doc;
  CHAR_T *source_doc_url;
  CHAR_T *annot_url;
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

/* the different annotation download modes */
typedef enum _AnnotLoadMode {
  AM_LOAD_NONE = 0,  /* don't load anything */
  AM_LOAD_LOCAL = 1, /* load local annots */
  AM_LOAD_REMOTE = 2 /* load remote annots */
} AnnotLoadMode;

/*-----------------------------------------------------------------------
   GetAnnotCustomQuery
  -----------------------------------------------------------------------*/
ThotBool GetAnnotCustomQuery (void)
{
  return annotCustomQuery;
}

/*-----------------------------------------------------------------------
   SetAnnotCustomQuery
  -----------------------------------------------------------------------*/
void SetAnnotCustomQuery (ThotBool value)
{
  annotCustomQuery = value;
}

/*-----------------------------------------------------------------------
   GetAnnotAlgaeText
  -----------------------------------------------------------------------*/
CHAR_T *GetAnnotAlgaeText (void)
{
  return annotAlgaeText;
}

/*-----------------------------------------------------------------------
   SetAnnotAlgaeText
  -----------------------------------------------------------------------*/
void SetAnnotAlgaeText (CHAR_T *value)
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
CHAR_T *GetAnnotUser (void)
{
  return annotUser;
}

/*-----------------------------------------------------------------------
   GetAnnotServers
   Returns the URLs of the annotation servers
  -----------------------------------------------------------------------*/
List *GetAnnotServers (void)
{
  return annotServers;
}

/*-----------------------------------------------------------------------
   GetAnnotPostServer
   Returns the URLs of the annotation Post server
  -----------------------------------------------------------------------*/
CHAR_T *GetAnnotPostServer (void)
{
  return annotPostServer;
}

/*-----------------------------------------------------------------------
   Procedure GetAnnotMainIndex
  -----------------------------------------------------------------------
   Returns the directory where the annotations of a given document are stored.
  -----------------------------------------------------------------------*/
CHAR_T *GetAnnotMainIndex (void)
{
  return annotMainIndex;
}

/*-----------------------------------------------------------------------
   Procedure GetAnnotDir
  -----------------------------------------------------------------------
   Returns the directory where the annotations of a given document are stored.
  -----------------------------------------------------------------------*/
CHAR_T *GetAnnotDir (void)
{
  return annotDir;
}

/*-----------------------------------------------------------------------
   Procedure GetAnnotDir
  -----------------------------------------------------------------------
   Returns the directory where the annotations of a given document are stored.
  -----------------------------------------------------------------------*/
List *CopyAnnotServers (CHAR_T *server_list)
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
      while (*ptr != ' ' && *ptr != WC_EOS)
	ptr++;
      if (*ptr == ' ')
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
static void CopyAlgaeTemplateURL (CHAR_T **dest, CHAR_T *proto, CHAR_T *url)
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
      tmp = ustrstr (in, "%u");
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
      if (*in == '%' && *(in + 1) == 'u')
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
   ANNOT_UpdateTransfer
   A frontend to the Amaya UpdateTransfer function, that takes into
   account open transfers.
  -----------------------------------------------------------------------*/
void ANNOT_UpdateTransfer (Document doc)
{
  if (FilesLoading[doc] == 0)
    ActiveTransfer (doc);
  else
    UpdateTransfer (doc);
}

/*-----------------------------------------------------------------------
   ANNOT_Init
   Initializes the annotation library
  -----------------------------------------------------------------------*/
void ANNOT_Init ()
{
  CHAR_T *tmp;

  /* initialize the annot global variables */
  annotDir = TtaWCSdup (TtaGetEnvString ("ANNOT_DIR"));
  annotMainIndex = TtaWCSdup (TtaGetEnvString ("ANNOT_MAIN_INDEX"));
  TtaGetEnvBoolean ("ANNOT_LAUTOLOAD", &annotLAutoLoad);
  TtaGetEnvBoolean ("ANNOT_RAUTOLOAD", &annotRAutoLoad);

  tmp = TtaGetEnvString ("ANNOT_USER");
  if (tmp)
    annotUser = TtaWCSdup (tmp);
  else
    annotUser = NULL;
  tmp = TtaGetEnvString ("ANNOT_SERVERS");
  if (tmp)
    annotServers = CopyAnnotServers (tmp);
  else
    annotServers = NULL;
  tmp = TtaGetEnvString ("ANNOT_POST_SERVER");
  if (tmp)
    annotPostServer = TtaWCSdup (tmp);
  else
    annotPostServer = TtaWCSdup ("localhost");

  /* @@@ temporary custom query, as we could use the configuration menu  ***/
  annotCustomQuery = FALSE;
  annotAlgaeText = TtaStrdup (DEFAULT_ALGAE_QUERY);

  /* create the directory where we'll store the annotations if it
     doesn't exist */
  TtaMakeDirectory (annotDir);
}

/*-----------------------------------------------------------------------
   ANNOT_FreeConf
  -----------------------------------------------------------------------
  -----------------------------------------------------------------------*/
void ANNOT_FreeConf ()
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
   ANNOT_Quit
  -----------------------------------------------------------------------
   Frees Annotation resources and releases all RDF resources.
   Be careful about outstanding references to RDF resources if you
   call this without intending to exit the application.
  -----------------------------------------------------------------------*/
void ANNOT_Quit ()
{
  ANNOT_FreeConf ();
  SCHEMA_FreeAnnotSchema ();
  schema_init = FALSE;
}

/*-----------------------------------------------------------------------
   ANNOT_FreeAnnotResource
   Frees all the annotation resources that are associated with
   annotation annot (doesn't remove this annotation yet).
  -----------------------------------------------------------------------*/
static void ANNOT_FreeAnnotResource (Document source_doc, Element annotEl, 
				AnnotMeta *annot)
{
  /* remove the annotation link in the source document */
  if (annotEl)
    LINK_RemoveLinkFromSource (source_doc, annotEl);
#if 0
  /* remove the annotation from the filters */
  AnnotFilter_delete (&(AnnotMetaData[source_doc].authors), annot, 
		      List_delCharObj);
  AnnotFilter_delete (&(AnnotMetaData[source_doc].types), annot, NULL);
  AnnotFilter_delete (&(AnnotMetaData[source_doc].servers), annot,
		      List_delCharObj);
#endif
}


/*-----------------------------------------------------------------------
   ANNOT_FreeDocumentResource
   Frees all the annotation resources that are associated with
   Document doc
  -----------------------------------------------------------------------*/
void ANNOT_FreeDocumentResource (Document doc)
{
  int i;

  /* reset the last_selected_annotation for this document */
  last_selected_annotation[doc] = NULL;

  /* close all the open annotation windows, asking the user if he 
   wants to save them */
  for (i = 1; i < DocumentTableLength; i++)
    {
      if (doc == i)
	continue;
      if (DocumentURLs[i] && DocumentTypes[i] == docAnnot
	  && DocumentMeta[i]->source_doc == doc)
	{
	  /* stop any active transfer related to this document */
	  StopTransfer (i, 1);	
	  /* insist, until the user saves the annotation.
	     It'd be nice to keep the annotation open, even if the
	     doc disappears */
	  /* while (DocumentURLs[i]) */
	    CloseDocument (i, 1);
	}
    }

  /* if we're deleting an annotation document and this annotations
     hasn't yet been saved (for example, the user changed his mind,
     we delete it */
  if (DocumentTypes[doc] == docAnnot
      && !IsW3Path (DocumentURLs[doc])
      && !TtaFileExist (DocumentURLs[doc]))
    {
      int source_doc;
      AnnotMeta *annot;
      Element annotEl;
      
      source_doc = DocumentMeta[doc]->source_doc;
      annot = AnnotList_searchAnnot (AnnotMetaData[source_doc].annotations,
				     DocumentURLs[doc],
				     AM_BODY_FILE);
      if (annot)
	{
	  annotEl = SearchAnnotation (source_doc, annot->name);
	  /* remove the annotation metadata and the annotation icon */
	  ANNOT_FreeAnnotResource (source_doc, annotEl, annot);
	  /* remove the annotation from the document's annotation list and 
	     update it */
	  AnnotList_delAnnot (&(AnnotMetaData[source_doc].annotations),
			      annot->body_url, FALSE);
	}
    }
  
  /* free the memory allocated for annotations */
  LINK_DelMetaFromMemory (doc);
  /* reset the state */
  AnnotMetaData[doc].local_annot_loaded = FALSE;
}


/*-----------------------------------------------------------------------
  RemoteLoad_callback
  -----------------------------------------------------------------------
  -----------------------------------------------------------------------*/
void RemoteLoad_callback (int doc, int status, 
			  CHAR_T *urlName,
			  CHAR_T *outputfile, 
			  AHTHeaders *http_headers,
			  void * context)
{
   REMOTELOAD_context *ctx;
   Document source_doc;
   CHAR_T *source_doc_url;

   /* restore REMOTELOAD contextext's */  
   ctx = (REMOTELOAD_context *) context;

   if (!ctx)
     return;
   
   ResetStop (doc);

   source_doc = ctx->source_doc;
   source_doc_url = ctx->source_doc_url;

   /* only load the annotation if the download was succesful and if
      the source document hasn't disappeared in the meantime */
   if (status == HT_OK
       && DocumentURLs[source_doc] 
       && !ustrcmp (DocumentURLs[source_doc], source_doc_url))
     {
       LINK_LoadAnnotationIndex (doc, ctx->remoteAnnotIndex, TRUE);
       /* clear the status line if there was no error*/
       TtaSetStatus (doc, 1,  "", NULL);
     }
   else
     {
       CHAR_T *ptr;
       ptr = HTTP_headers (http_headers, AM_HTTP_REASON);
       if (ptr)
	 TtaSetStatus (doc, 1, "Failed to load the annotation index: %s", ptr);
       else
	 TtaSetStatus (doc, 1, "Failed to load the annotation index", NULL);
     }
   
   TtaFreeMemory (source_doc_url);
   TtaFreeMemory (ctx->remoteAnnotIndex);
   TtaFreeMemory (ctx);
}

/*-----------------------------------------------------------------------
  ANNOT_Load2
  Loads the annotations from the local and remote servers. 
  The mode parameter says what kind of servers we want to consult
  -----------------------------------------------------------------------*/
static void ANNOT_Load2 (Document doc, View view, AnnotLoadMode mode)
{
  char *annotIndex;
  char *annotURL;
  char *proto;
  REMOTELOAD_context *ctx;
  int res;
  List *ptr;
  CHAR_T *server;
  ThotBool is_active = FALSE;

  if (mode == AM_LOAD_NONE)
    return;

  /* we can only annotate some types of documents */
  if (!ANNOT_CanAnnotate(doc))
    return;

  if (!schema_init)
    {
      /* @@ RRS unfinished; this is temporary while the code is raw
	 todo: read the schema asynchronously and delay the loading
	 of annotations until after the schema has been processed @@ */

      SCHEMA_InitSchemas (doc);
      schema_init = TRUE;
    }

  /*
   * Parsing test!
  */

  /*
   * load the local annotations if there's no annotserver or if
   * annotServers include the localhost
   */
  if ((mode & AM_LOAD_LOCAL)
      && (!annotServers || AnnotList_search (annotServers, "localhost")))
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
  if ((mode & AM_LOAD_REMOTE) && annotServers) 
    {
      /* load the annotations, server by server */
      ptr = annotServers;
      while (ptr)
	{
	  server = ptr->object;
	  ptr = ptr->next;
	  if (!server || !ustrcasecmp (server, "localhost")
	      || server[0] == '-')
	    continue;
	  /* create the context for the callback */
	  ctx = TtaGetMemory (sizeof (REMOTELOAD_context));
	  /* make some space to store the remote file name */
	  ctx->remoteAnnotIndex = TtaGetMemory (MAX_LENGTH);
	  /* store the source document infos */
	  ctx->source_doc = doc;
	  ctx->source_doc_url = TtaStrdup (DocumentURLs[doc]);
	  /* "compute" the url we're looking up in the annotation server */
	  if (!IsW3Path (DocumentURLs[doc]) &&
	      !IsFilePath (DocumentURLs[doc]))
	    proto = "file://";
	  else
	    proto = "";
	  if (!annotCustomQuery || !annotAlgaeText || 
	      annotAlgaeText[0] == WC_EOS)
	    {
	      annotURL = TtaGetMemory (ustrlen (DocumentURLs[doc])
				       + ustrlen (proto)
				       + sizeof ("w3c_annotates=")
				       + 50);
	      sprintf (annotURL, "w3c_annotates=%s%s", proto, 
		       DocumentURLs[doc]);
	    }
	  else
	    /* substitute the %u for DocumentURLs[doc] and go for it! */
	    /* @@ we have a potential mem bug here as I'm not computing
	       the exact size */
	    CopyAlgaeTemplateURL (&annotURL, proto, DocumentURLs[doc]);

	  if (IsFilePath (annotURL))
	    {
	      /* @@JK: normalize the URL to a local one */
	      
	    }
	  /* launch the request */
	  if (!is_active)
	    {
	      is_active = TRUE;
	      ANNOT_UpdateTransfer (doc);
	    }
	  else
	    UpdateTransfer (doc);
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
			      "application/xml");
	  TtaFreeMemory (annotURL);
	}
    }
}

/*-----------------------------------------------------------------------
   ANNOT_AutoLoad
  -----------------------------------------------------------------------
  -----------------------------------------------------------------------*/
void ANNOT_AutoLoad (Document doc, View view)
{
  AnnotLoadMode mode = AM_LOAD_NONE;

  if (annotLAutoLoad)
    mode |= AM_LOAD_LOCAL;
  if (annotRAutoLoad)
    mode |= AM_LOAD_REMOTE;

  if (mode == 0)
    return;

  ANNOT_Load2 (doc, view, mode);
}

/*-----------------------------------------------------------------------
  ANNOT_Load
  Front end to the Load Annotations command
  -----------------------------------------------------------------------*/
void ANNOT_Load (Document doc, View view)
{
  ANNOT_Load2 (doc, view, AM_LOAD_LOCAL | AM_LOAD_REMOTE);
}

/*-----------------------------------------------------------------------
  ANNOT_Create
  Creates an annotation on the selected text. If there's no selected
  text, it doesn't do anything.

  ** To do: protect for annot/annot agains the user putting an annotation
  on other parts of the annotation than the body
  -----------------------------------------------------------------------*/
void ANNOT_Create (Document doc, View view, ThotBool useDocRoot)
{
  Document    doc_annot;
  AnnotMeta  *annot;
  XPointerContextPtr ctx;
  CHAR_T     *xptr;

#if 0
  /* not used for the moment... select the annotation doc
     right away */
  Element     el;
  ElementType elType;
#endif

  /* we can only annotate some types of documents and saved documents */
  if (!ANNOT_CanAnnotate (doc))
    return;

  /* It's risky to annotate modified documents as we may end having instant
     orphan or misleading annotations. If it's an annotation document, 
     the document may be marked as not modified even if it has not yet been
     saved, so we verify if the file exists. */
  if (TtaIsDocumentModified (doc)
     || (DocumentTypes[doc] == docAnnot
	 && !IsW3Path (DocumentURLs[doc])
	 && !TtaFileExist (DocumentURLs[doc])))
    {
      InitInfo ("Error",
		"You cannot annotate a modified document. Please save it first.");
      return;
    }

#ifdef ANNOT_ON_ANNOT
  /* @@ JK Exp stuff to add a new thread item */
#if 0
  else if (DocumentTypes[doc] == docAnnot) 
    {
      ANNOT_AddThreadItem (doc, NULL, TRUE);
      return;
    }
  /* @@ */
#endif 
#endif /* ANNOT_ON_ANNOT */

  if (!annotUser || *annotUser == EOS)
    {
      InitInfo ("Make a new annotation", 
		"No annotation user declared. Please open the Annotations/Configure menu.");
      return;
    }

  if (!useDocRoot && TtaGetSelectedDocument () != doc)
    return; /* Error: nothing selected in this document */

 /* Annotation XPointer */
  xptr = XPointer_build (doc, 1, useDocRoot);
  /* if we can't compute the XPointer, we return (we could make a
     popup message box stating what happened) */
  if (!xptr)
    {
      TtaSetStatus (doc, 1,
		    /*  TtaGetMessage (AMAYA, AM_CANNOT_ANNOTATE), */
		    "Unable to build an XPointer for this annotation",
		    NULL);
      return;
    }
  
  /* create the document that will store the annotation */
  if ((doc_annot = ANNOT_NewDocument (doc)) == 0)
    {
      TtaFreeMemory (xptr);
      return;
    }

  /* @@ JK another hack, to solve an immediate problem */
  if (!schema_init)
    {
      /* @@ RRS unfinished; this is temporary while the code is raw
	 todo: read the schema asynchronously and delay the loading
	 of annotations until after the schema has been processed @@ */

      SCHEMA_InitSchemas (doc);
      schema_init = TRUE;
    }

  annot = LINK_CreateMeta (doc, doc_annot, useDocRoot);
  /* update the XPointer */
  annot->xptr = xptr;

  ANNOT_InitDocumentStructure (doc, doc_annot, annot, TRUE);

  /* @@ JK: do I need to do this? */
  UpdateContextSensitiveMenus (doc);

  /* add the annotation icon */
  TtaUnselect (doc);
  LINK_AddLinkToSource (doc, annot);

  /* reselect the annotated text starting from the xpointer */
  if (!useDocRoot)
    {
      ctx = XPointer_parse (doc, annot->xptr);
      XPointer_select (ctx);
      XPointer_free (ctx);
    }
  
#if 0
  /* ready for primetime, but do we want to do it or highlight
     the annotated text? */
  /* select the new annotation document */
  el = TtaGetRootElement (doc_annot);
  elType.ElSSchema =  TtaGetSSchema ("HTML", doc);
  elType.ElTypeNum = HTML_EL_BODY;
  el = TtaSearchTypedElement (elType, SearchInTree, el);
  /* then move the root variable so that it points to the beginning of the
     HTML document */
  el = TtaGetFirstChild (el);
  TtaSelectElement (doc_annot, el);
  TtaSelectView (doc, 1);
#endif
}

/*-----------------------------------------------------------------------
   ANNOT_Post_callback
  -----------------------------------------------------------------------*/
void ANNOT_Post_callback (int doc, int status, 
			  CHAR_T *urlName,
			  CHAR_T *outputfile, 
			  AHTHeaders *http_headers,
			  void * context)
{
  /* get the server's reply */
  /* update the source doc link */
  /* delete the temporary file */
  /* the context gives the link's name and we'll use it to look up the
     document ... */

   REMOTELOAD_context *ctx;
   int source_doc;
   
   /* restore REMOTELOAD contextext's */  
   ctx = (REMOTELOAD_context *) context;

   if (!ctx)
     return;

   ResetStop (doc);

   source_doc = ctx->source_doc;

   /* only update the metadata if the POST was succesful and if
      the source and annot documents haven't disappeared in the meantime */
   if (status == HT_OK
       && DocumentURLs[source_doc] 
       && !ustrcmp (DocumentURLs[source_doc], ctx->source_doc_url)
       && DocumentURLs[doc]
       && !ustrcmp (DocumentURLs[doc], ctx->annot_url))
     {
       AnnotMeta *annot = GetMetaData (source_doc, doc);

       TtaSetDocumentUnmodified (doc);
       DocStatusUpdate (doc, TRUE);

       /* Use the server's metadata answer to update our local metadata
	  and the reverse links that point to the annotation */
       /* @@ needs more work to handle error return codes */
       if (annot)
	 {
	   List *listP = RDF_parseFile (ctx->remoteAnnotIndex,
					&AnnotMetaData[source_doc].rdf_model);
	   if (listP)
	     {
	       AnnotMeta *returned_annot = (AnnotMeta *) listP->object;
	       if (returned_annot->source_url
		   && strcmp (returned_annot->source_url, annot->source_url))
		 fprintf (stderr, "PostCallback: POST returned an annotation for a different source: %s vs %s\n",
			  returned_annot->source_url, annot->source_url);
	       if (returned_annot->annot_url)
		 {
		   TtaFreeMemory (annot->annot_url);
		   annot->annot_url = returned_annot->annot_url;
		   returned_annot->annot_url = NULL;
		 }

	       /* replace the body only if it changed */
	       if (returned_annot->body_url
		   && (!annot->body_url 
		       || ustrcmp (annot->body_url, returned_annot->body_url)))
		 {
		   /* update the anchor in the source doc */
		   ReplaceLinkToAnnotation (source_doc, annot->name, 
					    returned_annot->body_url);

		   if (IsFilePath (annot->body_url))
		     {
		       /* local annot was just made a shared annot;
			  update the annotation index or delete it
			  if it's now empty */
		       if (AnnotList_localCount (AnnotMetaData[source_doc].annotations) > 0)
			 LINK_SaveLink (source_doc);
		       else
			 LINK_DeleteLink (source_doc);
		     }

		   /* update the annotation body_url */
		   /* TtaFileUnlink (annot->body_url); */
		   TtaFreeMemory (annot->body_url);
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
	       /* if we were posting a localfile, remove this file
		  (and update the indexes */
	       if (ctx->localfile)
		 {
		   TtaFileUnlink (ctx->localfile);
		   TtaFreeMemory (ctx->localfile);
		 }
	     }
	 }
       TtaFileUnlink (ctx->remoteAnnotIndex);
     }
   else /* there was error */
     {
       CHAR_T *ptr;
       ptr = HTTP_headers (http_headers, AM_HTTP_REASON);
       if (ptr)
	 TtaSetStatus (doc, 1, "Failed to post the annotation: %s", ptr);
       else
	 TtaSetStatus (doc, 1, "Failed to post the annotation", NULL);
     }

   /* erase the rdf container */
   TtaFileUnlink (ctx->rdf_file);

   /* free all memory associated with the context */
   TtaFreeMemory (ctx->source_doc_url);
   TtaFreeMemory (ctx->annot_url);
   TtaFreeMemory (ctx->rdf_file);
   TtaFreeMemory (ctx->remoteAnnotIndex);
   TtaFreeMemory (ctx);
}

/*-----------------------------------------------------------------------
   ANNOT_Post
  -----------------------------------------------------------------------*/
void ANNOT_Post (Document doc, View view)
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
  if (ustrcmp (TtaGetSSchemaName (TtaGetDocumentSSchema (doc)), "Annot"))
    return;

  if (!annotPostServer || *annotPostServer == EOS)
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
  /* memorize the source and annotation document info */
  source_doc = DocumentMeta[doc]->source_doc;
  ctx->source_doc = source_doc;
  ctx->source_doc_url = TtaStrdup (DocumentURLs[source_doc]);
  ctx->annot_url = TtaStrdup (DocumentURLs[doc]);

  /* compute the URL */
  if (IsW3Path (DocumentURLs[doc]))
    {
      /* find the annotation metadata that corresponds to this annotation */
      annot = AnnotList_searchAnnot (AnnotMetaData[source_doc].annotations,
				     DocumentURLs[doc], AM_BODY_URL);

      if (!annot)
	/* @@ JK: give some error message, free the ctx */
	return;

      /* we're saving a modification to an existing annotation */
      url = TtaGetMemory (ustrlen (annotPostServer)
			  + sizeof ("?replace_source=")
			  + ustrlen (annot->annot_url)
			  + sizeof ("&rdftype=")
			  + strlen (ANNOTATION_CLASSNAME)
			  + 1);
      usprintf (url,"%s?replace_source=%s&rdftype=%s",
		annotPostServer,
		annot->annot_url,
		ANNOTATION_CLASSNAME);
      free_url = TRUE;
    }
  else
    {
      /* we're saving a new annotation */
      url = annotPostServer;
      free_url = FALSE;
    }

  /* launch the request */
  ANNOT_UpdateTransfer (doc);
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
  /* @@ JK: here we should delete the context or call the callback in case of
     error */
  if (res)
    TtaSetStatus (doc, 1, "Failed to post the annotation", NULL);
}

/*----------------------------------------------------------------------
  ANNOT_SaveDocument
  Frontend function that decides if an annotation should be saved and if
  it should be saved remotely or locally. It then calls the appropriate 
  function to do this operation.
  ----------------------------------------------------------------------*/
void ANNOT_SaveDocument (Document doc_annot, View view)
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
	{
	  TtaSetDocumentUnmodified (doc_annot);
	  LINK_SaveLink (DocumentMeta[doc_annot]->source_doc);
	}
      TtaFreeMemory (filename); 
    }
}

/*-----------------------------------------------------------------------
  ANNOT_SelectSourceDoc
  If the user clicked on an annotation link in the source document,
  it highlights the annotated text.
  -----------------------------------------------------------------------*/
void ANNOT_SelectSourceDoc (int doc, Element el)
{
  ElementType      elType;
  AttributeType    attrType;
  Attribute	   attr;
  int              length;
  CHAR_T          *annot_url;

  /* reset the last selected annotation ptr */
  last_selected_annotation[doc] = NULL;

  /* is it an annotation link? */
  elType = TtaGetElementType (el);
  if (ustrcmp (TtaGetSSchemaName (elType.ElSSchema), "XLink")
      || (elType.ElTypeNum != XLink_EL_XLink))
    return;

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
  LINK_SelectSourceDoc (doc, annot_url, FALSE);
  /* memorize the last selected annotation */
  last_selected_annotation[doc] =  el;

  TtaFreeMemory (annot_url);
}

/*-----------------------------------------------------------------------
  RaiseSourceDoc_callback
  -----------------------------------------------------------------------*/
void Annot_RaiseSourceDoc_callback (int doc, int status, 
				    CHAR_T *urlName,
				    CHAR_T *outputfile, 
				    AHTHeaders *http_headers,
				    void * context)
{
  RAISESOURCEDOC_context *ctx;

  /* restore REMOTELOAD contextext's */  
  ctx = (RAISESOURCEDOC_context *) context;

  if (!ctx)
     return;
 
  /* select the source of the annotation */
  if (status == HT_OK && ctx->doc_annot)
    {
      /* select the annotated text and memorize the last selected 
	 annotation */ 
      last_selected_annotation[doc] =  
	LINK_SelectSourceDoc (doc, DocumentURLs[ctx->doc_annot], TRUE);
    }
  else
    last_selected_annotation[doc] = NULL;

  if (ctx->url)
    TtaFreeMemory (ctx->url);
  TtaFreeMemory (ctx);
}

/*-----------------------------------------------------------------------
  Annot_Raisesourcedoc
  The user has double clicked on the annot link to the source document
  -----------------------------------------------------------------------*/
ThotBool Annot_RaiseSourceDoc (NotifyElement *event)
{
  Element          el;
  Document         doc_annot;
  AttributeType    attrType;
  Attribute	   HrefAttr;
  ThotBool	   docModified;
  int              length;
  Document         targetDocument;
  CHAR_T          *url = NULL;
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
ThotBool Annot_Types (NotifyElement *event)
{
  Element          el;
  Document         doc_annot;
  ThotBool         docModified;
  RDFResourceP     new_type;

  /* initialize from the context */
  el = event->element;
  doc_annot = event->document;

  docModified = TtaIsDocumentModified (doc_annot);

  new_type = AnnotTypes (doc_annot, 1);
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
  Todo: rename the LINK_SaveLink to LINK_SaveIndex, as that's what 
  it's really doing.
  -----------------------------------------------------------------------*/
void ANNOT_Delete_callback (int doc, int status, 
			    CHAR_T *urlName,
			    CHAR_T *outputfile, 
			    AHTHeaders *http_headers,
			    void * context)
{
  DELETE_context *ctx;
  Document source_doc;
  Document annot_doc;
  CHAR_T  *annot_url;
  CHAR_T  *output_file;
  Element  annotEl;
  AnnotMeta *annot;
  ThotBool annot_is_remote;
  ThotBool delete_annot = TRUE;

  /* restore REMOTELOAD contextext's */  
  ctx = (DELETE_context *) context;

  if (!ctx)
    return;
 
  ResetStop (doc);
 
  source_doc = ctx->source_doc;
  annot_doc = ctx->annot_doc;
  annot_url = ctx->annot_url;
  annotEl = ctx->annotEl;
  annot_is_remote = ctx->annot_is_remote;
  output_file = ctx->output_file;
  annot = ctx->annot;
  
  if (status == HT_OK)
    {
      /* @@ JK: check if the user didn't close it in the meantime */
      if (annot_doc) /* close the annotation window and free its resources */
	{
	  /* we don't want to save any last modifications, we're deleting
	     it */
	  TtaSetDocumentUnmodified (annot_doc);
	  /* if it was a new annotation that was never saved,
	     it'll be automatically deleted when closing the
	     annot document */
	  if (!IsW3Path (DocumentURLs[annot_doc])
	      && !TtaFileExist (DocumentURLs[annot_doc]))
	    delete_annot = FALSE;

	  /* closes the document window and frees all metada
	   associated with the document */
	  CloseDocument (annot_doc, 1);
	}

      if (delete_annot)
	{
	  /* remove the annotation metadata and the annotation icon */
	  ANNOT_FreeAnnotResource (source_doc, annotEl, annot);
	  /* remove the annotation from the document's annotation list and 
	     update it */
	  AnnotList_delAnnot (&(AnnotMetaData[source_doc].annotations),
			      annot_url, FALSE);
	}

      /* update the annotation index or delete it if it's empty */
      if (AnnotList_localCount (AnnotMetaData[source_doc].annotations) > 0)
	LINK_SaveLink (source_doc);
      else
	LINK_DeleteLink (source_doc);
    }
  else 
    {
      CHAR_T *ptr;
      ptr = HTTP_headers (http_headers, AM_HTTP_REASON);
      if (ptr)
	TtaSetStatus (doc, 1, "Failed to delete the annotation: %s", ptr);
      else
	TtaSetStatus (doc, 1, "Failed to delete the annotation", NULL);
    }

  if (output_file)
    {
      if (*output_file != EOS)
	TtaFileUnlink (output_file);
      TtaFreeMemory (output_file);
    }

  TtaFreeMemory (ctx);
  /* clear the status line if there was no error*/
  if (status == HT_OK && doc == source_doc)
    TtaSetStatus (doc, 1,  "Annotation deleted!", NULL);
}

/*----------------------------------------------------------------------
  ANNOT_Delete 
  Erases one annotation
 -----------------------------------------------------------------------*/
void ANNOT_Delete (Document doc, View view)
{
  Document         source_doc, annot_doc;
  ElementType      elType;
  Element          annotEl;
  AnnotMeta       *annot;
  AttributeType    attrType;
  Attribute	   attr;
  CHAR_T          *annot_url;
  CHAR_T          *annot_server;
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
      annot_doc = doc;
      source_doc = DocumentMeta[doc]->source_doc;
      last_selected_annotation[source_doc] =  NULL;
  
      /* make the body */
      if (IsW3Path (DocumentURLs[doc]))
	{
	  /* it's a remote annotation */
	  annot_is_remote = TRUE;
	  if (DocumentMeta[doc]->form_data)
	    {
	      annot_url = TtaGetMemory (ustrlen (DocumentURLs[doc])
				       + ustrlen (DocumentMeta[doc]->form_data)
					+ sizeof ("?")
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
      if (!last_selected_annotation[source_doc])
	return;

      annotEl = last_selected_annotation[source_doc];
      last_selected_annotation[source_doc] = NULL;

      /* is it an annotation link? */
      elType = TtaGetElementType (annotEl);
      if (ustrcmp (TtaGetSSchemaName (elType.ElSSchema), "XLink")
	  || (elType.ElTypeNum != XLink_EL_XLink))
	return;
      
      /* get the annotation URL */
      attrType.AttrSSchema = elType.ElSSchema;
      attrType.AttrTypeNum = XLink_ATTR_href_;
      attr = TtaGetAttribute (annotEl, attrType);
      if (!attr)
	return;
      i = TtaGetTextAttributeLength (attr);
      if (i < 1)
	/* @@ RRS The service apparently didn't return a URI.  This
	   isn't a bug in the longer-term, but we don't handle this
	   nicely right now.  It would also be more convenient to
	   store the local annot handle in the Thot tree explicitly
	   so we don't have to string compare to find it. */
	return;

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

  TtaFreeMemory (annot_url);
  /* @@ JK: this is a dumb convertion. Why local annotations don't
     have an annotation URL field? Why am I not using the libwww's
     URL to local converters, rather than using my own hack with file://? */
  if (annot_is_remote)
    annot_url = annot->annot_url;
  else
    annot_url = annot->body_url;

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
      /* RRS @@ services should become RDF resources and then the
	 annot structure can simply refer to them, avoiding this loop */
      list_ptr = annotServers;
      annot_server = NULL;
      while (list_ptr)
	{
	  annot_server = list_ptr->object;
	  if (ustrcasecmp (annot_server, "localhost")
	      && !ustrncasecmp (annot_server, annot->annot_url, 
				ustrlen (annot_server)))
	    break;
	  list_ptr = list_ptr->next;
	}

      if (annot_server)
	{
	  /* launch the request */
	  ANNOT_UpdateTransfer (doc);
	  res = GetObjectWWW (doc,
			      annot->annot_url,
			      NULL,
			      ctx->output_file,
			      AMAYA_ASYNC | AMAYA_DELETE | AMAYA_FLUSH_REQUEST,
			      NULL,
			      NULL, 
			      (void *)  ANNOT_Delete_callback,
			      (void *) ctx,
			      NO,
			      NULL);
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
}

/*----------------------------------------------------------------------
  ANNOT_Move
  Changes the context (XPointer) associated with an annotation
  If useSel is true, we'll use the current selection. Otherwise,
  we'll use the memorized XPointer.
 -----------------------------------------------------------------------*/
void ANNOT_Move (Document doc, View view, ThotBool useSel)
{
  Document         source_doc, annot_doc;
  Element          annotEl;
  AnnotMeta       *annot;
  CHAR_T          *annot_url;
  int              i;
  DisplayMode      dispMode;
  char            *xptr;
  XPointerContextPtr xptr_ctx;

  /* 
  **  get the annotation URL, source_doc, and annot_doc
  */
  if (DocumentTypes[doc] == docAnnot)
    {
      annot_doc = doc;
      source_doc = DocumentMeta[doc]->source_doc;

      /* don't move the XPointer unless we know the document's URL */
      if (!DocumentURLs[source_doc] || *DocumentURLs[source_doc] == EOS)
	return;

      if (useSel)
	xptr = XPointer_build (source_doc, view, FALSE);
      else
	xptr = XPointer_buffer ();

      if (!xptr || *xptr == EOS)
	{
	  TtaSetStatus (doc, 1, TtaGetMessage (AMAYA, AM_NO_ANNOT_MOVE),
			NULL);
	  return;
	}

      /* control the stored XPointer */
      if (!useSel)
	{
	  /* don't move the XPointer to an stored XPointer unless this pointer points to the same
	     source document as the previous one */
	  i = strlen (DocumentURLs[source_doc]);
	  if (strncmp (DocumentURLs[source_doc], xptr, i) || xptr[i] != '#')
	    {
	      TtaSetStatus (doc, 1, 
			    TtaGetMessage (AMAYA, AM_NO_ANNOT_MOVE_OTHER),
			    NULL);
	      return;
	    }
	  else
	    {
	      /* point to the first char after the hash */
	      xptr = xptr + i + 1;
	      if (*xptr == EOS)
		return;
	    }
	}

      /* clear the status */
      last_selected_annotation[source_doc] =  NULL;

      /* @@ JK The following code is also shared by ANNOT_delete. Move
	 it to a function */
      /* make the body */
      if (IsW3Path (DocumentURLs[doc]))
	{
	  /* it's a remote annotation */
	  if (DocumentMeta[doc]->form_data)
	    {
	      annot_url = TtaGetMemory (ustrlen (DocumentURLs[doc])
				       + ustrlen (DocumentMeta[doc]->form_data)
					+ sizeof ("?")
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
	  annot_url = ANNOT_MakeFileURL (DocumentURLs[doc]);
	}
  
      /* find the annotation link in the source document that corresponds
	 to this annotation */
      annot = AnnotList_searchAnnot (AnnotMetaData[source_doc].annotations,
				     annot_url, AM_BODY_URL);
      TtaFreeMemory (annot_url);

      if (!annot)
	{
	  if (useSel)
	    TtaFreeMemory (xptr);
	  /* signal some error */
	  return;
	}

      annotEl = SearchAnnotation (source_doc, annot->name);
      if (!annotEl)
	{
	  if (useSel)
	    TtaFreeMemory (xptr);
	  /* signal some error */
	  return;
	}
      
      /* remove the selection from the annotated doc */
      if (TtaGetSelectedDocument () == source_doc)
	TtaUnselect (source_doc);

      /* avoid refreshing the document while moving the  annotation link */
      dispMode = TtaGetDisplayMode (doc);
      if (dispMode == DisplayImmediately)
	TtaSetDisplayMode (doc, DeferredDisplay);

      /* delete the previous annotation element */
      LINK_RemoveLinkFromSource (source_doc, annotEl);

      /* delete the previous xpointer, copy the new one */
      TtaFreeMemory (annot->xptr);      
      annot->xptr = TtaStrdup (xptr);

      /* add the new El */
      if ( LINK_AddLinkToSource (source_doc, annot))
	{
	  annot->is_visible = TRUE;
	  annot->show = TRUE;
	  /* and select it */
	  xptr_ctx = XPointer_parse (source_doc, xptr);
	  XPointer_select (xptr_ctx);
	  XPointer_free (xptr_ctx);
	}
      else
	{
	  annot->is_visible = FALSE;
	  annot->show = FALSE;
	}

      TtaSetDocumentModified (doc);
      /* show the document */
      if (dispMode == DisplayImmediately)
	TtaSetDisplayMode (doc, dispMode);

      if (useSel)
	TtaFreeMemory (xptr);
    }
}

/*----------------------------------------------------------------------
  ANNOT_AddLink
  adds a new link to an annotation
 -----------------------------------------------------------------------*/
void ANNOT_AddLink (Document doc, View view)
{
#if 0
  ElementType elType;
  Element root, el, newEl;
  Attribute attr;
  AttributeType       attrType;

  root = TtaGetRootElement (doc);
  elType.ElSSchema =  TtaGetDocumentSSchema (doc);
  elType.ElTypeNum = Annot_EL_Description;
  root = TtaSearchTypedElement (elType, SearchInTree, root);
  if (!root)
    return;

  /* create the new element */
  elType.ElTypeNum = Annot_EL_SourceDoc;
  newEl = TtaNewElement (doc, elType);
  /* attach it to the metadata */
  el = TtaGetLastChild (root);
  TtaInsertSibling (newEl, el, FALSE, doc);
#if 0
  attrType.AttrSSchema = elType.ElSSchema;
  attrType.AttrTypeNum = Annot_ATTR_HREF_;
  attr = TtaNewAttribute (attrType);
  /* copy the URL + text of the original document */
  TtaAttachAttribute (newEl, attr, doc);
  TtaSetAttributeText (attr, "jose", newEl, doc);
#endif
  TtaSetTextContent (newEl, "jose", TtaGetDefaultLanguage (), doc);
#endif
}
