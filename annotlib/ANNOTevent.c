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

/* the structure used for storing the context of the 
   Annot_Raisesourcedoc_callback function */
typedef struct _RAISESOURCEDOC_context {
  CHAR_T *url;
  Document annot_doc;
} RAISESOURCEDOC_context;

/* the structure used for storing the context of the 
   RemoteLoad_callback function */
typedef struct _REMOTELOAD_context {
  char *remoteAnnotIndex;
} REMOTELOAD_context;

/* the structure used for storing the context of the 
   RemoteSave_callback function */
typedef struct _REMOTESAVE_context {
  char *remoteAnnotIndex;
} REMOTESAVE_context;

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
  /* @@ I'm lazy today, so I'll just count how many times we have
     cookies, rather than reallocating the memory if it's not enough */
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
     doesn't exist ** how to check that with the thotlib? */
  /* @@ should be a nice mode! */
  umkdir (annotDir, 0777);
}

/*-----------------------------------------------------------------------
   Procedure ANNOT_Quit
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
      List_delAll (&annotServers);
  if (annotPostServer)  
    TtaFreeMemory (annotPostServer);
  if (annotAlgaeText)
    TtaFreeMemory (annotAlgaeText);
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
     LINK_LoadAnnotationIndex (doc, ctx->remoteAnnotIndex);

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
  ElementType elType;
  char *annotIndex;
  char *annotURL;
  char *proto;
  REMOTELOAD_context *ctx;
  int res;
  List *ptr;
  CHAR_T *server;

  /* only HTML documents can be annotated */
  elType.ElSSchema = TtaGetDocumentSSchema (doc);
  if (ustrcmp(TtaGetSSchemaName (elType.ElSSchema), TEXT("HTML")))
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
      LINK_LoadAnnotationIndex (doc, annotIndex);
      TtaFreeMemory (annotIndex);
    }
  /* 
   * load the remote annotation index
   *  @@ this should be in fact a for each annot in AnnotServers
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
  ElementType elType;
  Element     first, last;
  CHAR_T     *labf, *labl;
  int         c1, cl, i;
  Document    doc_annot;
  AnnotMeta  *annot;

  elType.ElSSchema = TtaGetDocumentSSchema (doc);
  if (ustrcmp(TtaGetSSchemaName (elType.ElSSchema), TEXT("HTML")))
    return;

  TtaGiveFirstSelectedElement (doc, &first, &c1, &i);
  TtaGiveLastSelectedElement (doc, &last, &i, &cl);

  if (first == NULL)
    return;  /* Error: there's no selected zone */

  /* create the document that will store the annotation */
  if ((doc_annot = ANNOT_NewDocument (doc)) == 0)
    return;

  /* Link the source document to the annotation */
  labf = TtaWCSdup (TtaGetElementLabel (first));
  labl = TtaWCSdup (TtaGetElementLabel (last));
  annot = LINK_CreateMeta (doc, doc_annot, labf, c1, labl, cl);
  TtaFreeMemory (labf);
  TtaFreeMemory (labl);

  ANNOT_InitDocumentStructure (doc, doc_annot, annot);

  LINK_AddLinkToSource (doc, annot);
  LINK_SaveLink (doc);

#if 0
  /* @@@ this should be added later on, when saving the annot document */
  /* create the annotation anchor */

  /* Saves the selection data */
  strcpy (tabRefAnnot[doc_annot].labf, TtaGetElementLabel (first));
  tabRefAnnot[doc_annot].c1 = c1;
  strcpy (tabRefAnnot[doc_annot].labl, TtaGetElementLabel (last));
  tabRefAnnot[doc_annot].cN = cN;
  tabRefAnnot[doc_annot].docName = TtaWCSdup (TtaGetDocumentName (doc_annot));
#endif
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
       LINK_LoadAnnotationIndex (doc, ctx->remoteAnnotIndex);
       ANNOT_LoadAnnotation (source_doc, doc);
#endif /*0*/

       if (annot)
	 {
	   List* listP =
	     RDF_parseFile (ctx->remoteAnnotIndex, ANNOT_LIST);
	   if (listP)
	     {
	       AnnotMeta* returned_annot = (AnnotMeta *)listP->object;
	       if (returned_annot->source_url
		   && strcmp(returned_annot->source_url, annot->source_url))
		 fprintf (stderr, "?oops - POST returned an annotation for a different source: %s vs %s\n",
			  returned_annot->source_url, annot->source_url);
	       if (returned_annot->annot_url) {
		 TtaFreeMemory(annot->annot_url);
		 annot->annot_url = returned_annot->annot_url;
		 returned_annot->annot_url = NULL;
	       }
	       if (returned_annot->body_url) {
		 TtaFreeMemory(annot->body_url); /* @@ unlink the file */
		 annot->body_url = returned_annot->body_url;
		 /* @@ Update the anchor in the source doc */
		 returned_annot->body_url = NULL;
	       }
	       if (listP->next)
		 fprintf (stderr, "?oops - POST returned more than one annotation\n");
	       AnnotList_free (listP);
	     }
	 }
       TtaFileUnlink (ctx->remoteAnnotIndex);
     }

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
  ElementType elType;

  REMOTELOAD_context *ctx;
  int res;

  elType.ElSSchema = TtaGetDocumentSSchema (doc);
  /*
  if (!annotServer
      || DocumentTypes[doc] != 10
      || DocumentTypes[doc] != 11)
    return;
  */

  /* output the HTML body */
  ANNOT_SaveDocument (doc);

  /* create the RDF container */
  ANNOT_PreparePostBody (doc);

  /* create the context for the callback */
  ctx = TtaGetMemory (sizeof (REMOTELOAD_context));
  /* make some space to store the remote file name */
  ctx->remoteAnnotIndex = TtaGetMemory (MAX_LENGTH);
  /* launch the request */
  res = GetObjectWWW (doc,
		      annotPostServer,
#ifdef _WINDOWS
			  TEXT("rdf.tmp"),
#else
		      TEXT("/tmp/rdf.tmp"),
#endif /* _WINDOWS */
		      ctx->remoteAnnotIndex,
		      AMAYA_FILE_POST | AMAYA_ASYNC | AMAYA_FLUSH_REQUEST,
		      NULL,
		      NULL, 
		      (void *)  ANNOT_Post_callback,
		      (void *) ctx,
		      NO,
		      NULL);
  if (res)
    {
      fprintf (stderr, "Failed to post the annotation!\n"); /* @@ */
    }
}

/*-----------------------------------------------------------------------
  RaiseSourceDoc_callback
  -----------------------------------------------------------------------
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

   TtaFreeMemory (ctx->url);
   /* unselect the selection */

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
  /* don't let Thot perform the normal operation */
  
  /* @@ and now jump to the annotated document and put it on top,
     jump to the anchor... and if the document isn't there, download it? */
  ctx = TtaGetMemory (sizeof (REMOTELOAD_context));
  ctx->url = url;
  ctx->annot_doc = doc_annot;
  targetDocument = GetHTMLDocument (url, NULL,
				    doc_annot, 
				    doc_annot, 
				    CE_ABSOLUTE, FALSE, 
				    (void *) Annot_RaiseSourceDoc_callback,
				    (void *) ctx);
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
  CHAR_T          *new_type;

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

/***************************************2************
 I've not yet used/cleaning the following legacy functions 
***************************************************/

/*----------------------------------------------------------------------
 -----------------------------------------------------------------------*/

#ifdef __STDC__
void ANNOT_Delete (Document document, View view)
#else /* __STDC__*/
void ANNOT_Delete (document, view)
     Document document;
     View view;
#endif /* __STDC__*/
{
  ElementType elType;
  Element     first, last;
  CHAR_T      *annotName, *fileName;
  int         i;

  printf ("(ANNOT_Delete) DEBUT\n");

  /* On verifie si le fragment est en ecriture */
#if 0
  if (!IsDocWritable (document))
    return;
#endif

  /* Récupère le premier et le dernier élément sélectionné */
  TtaGiveFirstSelectedElement (document, &first, &i, &i);
  TtaGiveLastSelectedElement (document, &last, &i, &i);

  /* On verifie si la zone selectionnee est valide */
  if ((first == NULL) || (first != last))
    return;

  /* On verifie que l'on a bien affaire a un lien d'annotation */
  elType = TtaGetElementType (first);
  if ((elType.ElTypeNum != HTML_EL_Anchor) || (!IsAnnotationLink (document, first)))
    return;

  /* Suppression de l'annotation */
  annotName = SearchAttributeInEl (document, first, HTML_ATTR_NAME, 
				   TEXT("HTML"));
  /* @@ BUG annotName is not freed */
  TtaRemoveTree (first, document);
  LINK_RemoveLink (document, annotName);
  fileName = TtaGetMemory (100);
  sprintf (fileName, "%s%c%s.PIV", annotDir, DIR_SEP, annotName);
  TtaFileUnlink (fileName);

#if 0
  /* Notification de la suppression de l'annotation */
  ANNOT_NotifyLocalUsers (document, annotName);
  ANNOT_NotifyToRemoteSites (document, annotName);
#endif

  printf ("(ANNOT_Delete) FINn");
}

/*-----------------------------------------------------------------------
   Procedure ANNOT_Save (docAnnot, viewAnnot)
  -----------------------------------------------------------------------
  -----------------------------------------------------------------------*/

#ifdef __STDC__
void ANNOT_Save (Document docAnnot, View viewAnnot)
#else /* __STDC__*/
void ANNOT_Save (docAnnot, viewAnnot)
     Document docAnnot;
     View viewAnnot;
#endif /* __STDC__*/
{

  Document document;
  CHAR_T *  annotName;

  document = AnnotationTargetDocument (docAnnot);
  annotName = TtaGetDocumentName (docAnnot);

  /* Sauvegarde du document d'annotation */
  TtaSaveDocument (docAnnot, annotName);
  TtaSetDocumentUnmodified (docAnnot);

  /* Si le lien d'annotation n'existe pas encore => affichage + notification */
  if (!SearchAnnotation (document, annotName))
  {
    /* Creation d'un nouveau lien d'annotation */
    printf ("1\n");
    /* Gestion du cas ou une annotation a ete creee sur un document non sauvegarde */
    if (TtaIsDocumentModified (document))
    {
printf ("2\n");
#if 0
      SetCallbackProc (ALLIANCE_BASE + ANNOT_NOTIF_SHEET_REF,
                       ANNOT_SaveProcedure);
#endif

printf ("3\n");
#if 0
      TtaNewSheet (ALLIANCE_BASE + ANNOT_NOTIF_SHEET_REF,
                   TtaGetViewFrame (docAnnot, viewAnnot),
                   NULL,
                   1,
                   TtaGetMessage (AllianceMsgTabId,
                                  ALL_AnnotNotif),
                   TRUE,
                   1,
                   'L',
                   D_CANCEL);
#endif
printf ("4\n");
#if 0
      TtaNewLabel (ALLIANCE_BASE + ANNOT_NOTIF_LABEL_SHEET,
                   ALLIANCE_BASE + ANNOT_NOTIF_SHEET_REF,
                   TtaGetMessage (AllianceMsgTabId,
                                  ALL_AnnotNotifLabel));
#endif
printf ("5\n");
#if 0
      TtaShowDialogue (ALLIANCE_BASE + ANNOT_NOTIF_SHEET_REF,
                       FALSE);
#endif
printf ("6\n");
#if 0
      TtaWaitShowDialogue ();
#endif
printf ("7\n");
    }
printf ("8\n");
  }
}










