/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1999-2008
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
#ifdef _WX
#include "appdialogue_wx.h"
#endif /* _WX */
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

#ifdef BOOKMARKS
#include "f/BMevent_f.h"
#endif /* BOOKMARKS */

/* RDF parser */
#if defined(ANNOTATIONS) && !defined(BOOKMARKS)
#include "raptor.h"
#endif /* BOOKMARKS */

#define DEFAULT_ALGAE_QUERY "w3c_algaeQuery=(ask '((?p ?s ?o)) :collect '(?p ?s ?o))"

/* some state variables */
static char *annotUser; /* user id for saving the annotation */
static char *annotDir;   /* directory where we're storing the annotations */
static List   *annotServers;   /* URL pointing to the annot server script */
static char *annotPostServer; /* URL pointing to the annot server script */
static char *annotMainIndex; /* index file where we give the correspondance
                                between URLs and annotations */
static ThotBool annotLAutoLoad; /* should local annotations be downloaded
                                   automatically? */
static ThotBool annotRAutoLoad; /* should remote annotations be downloaded
                                   automatically? */
static ThotBool annotCustomQuery; /* use an algae custom query if TRUE */
static char *annotAlgaeText;    /* the custom algae query text */
static ThotBool annotPOSTUpdate; /* use the POST method for updating annotations */

/* last selected annotation */
static Element last_selected_annotation[DocumentTableLength];

static ThotBool schema_init = FALSE;

/* the structure used for storing the context of the 
   Annot_Raisesourcedoc_callback function */
typedef struct _RAISESOURCEDOC_context {
  char *url;
  Document doc_annot;
  ThotBool has_thread;
} RAISESOURCEDOC_context;

/* the structure used for storing the context of the 
   RemoteLoad_callback function */
typedef struct _REMOTELOAD_context {
  char *rdf_file;
  char *remoteAnnotIndex;
  char *localfile;
  Document source_doc;
  char *source_doc_url;
  char *source_body_url;
  char *annot_url;
  ThotBool isReplyTo;
} REMOTELOAD_context;

/* the structure used for storing the context of the 
   RemoteLoad_callback function */
typedef struct _DELETE_context {
  Document source_doc;
  Document annot_doc;
  char *annot_url;
  char *output_file;
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
char *GetAnnotAlgaeText (void)
{
  return annotAlgaeText;
}

/*-----------------------------------------------------------------------
  SetAnnotAlgaeText
  -----------------------------------------------------------------------*/
void SetAnnotAlgaeText (char *value)
{
  if (!value && *value == EOS)
    annotAlgaeText = NULL;
  else
    annotAlgaeText = TtaStrdup (value);
}

/*-----------------------------------------------------------------------
  GetAnnotUser
  Returns the directory where the annotations of a given document are stored.
  -----------------------------------------------------------------------*/
char *GetAnnotUser (void)
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
char *GetAnnotPostServer (void)
{
  return annotPostServer;
}

/*-----------------------------------------------------------------------
  Procedure GetAnnotMainIndex
  -----------------------------------------------------------------------
  Returns the directory where the annotations of a given document are stored.
  -----------------------------------------------------------------------*/
char *GetAnnotMainIndex (void)
{
  return annotMainIndex;
}

/*-----------------------------------------------------------------------
  Procedure GetAnnotDir
  -----------------------------------------------------------------------
  Returns the directory where the annotations of a given document are stored.
  -----------------------------------------------------------------------*/
char *GetAnnotDir (void)
{
  return annotDir;
}

/*-----------------------------------------------------------------------
  Procedure CopyAnnotServers
  -----------------------------------------------------------------------
  Returns the directory where the annotations of a given document are stored.
  -----------------------------------------------------------------------*/
List *CopyAnnotServers (char *server_list)
{
  List *me = NULL;
  char *server;
  char *ptr;
  char *scratch;


  if (!server_list || *server_list == EOS)
    return NULL;

  /* make a copy we can modify */
  scratch = TtaStrdup (server_list);
  ptr = scratch;
  while (*ptr != EOS)
    {
      server = ptr;
      while (*ptr != ' ' && *ptr != EOS)
        ptr++;
      if (*ptr == ' ')
        {
          *ptr = EOS;
          ptr++;
        }
      List_add (&me, TtaStrdup (server));
    }
  TtaFreeMemory (scratch);
  return me;
}

/*-----------------------------------------------------------------------
  CopyAlgaeTemplateURL
  Prepares a query URL using the algae text template. Any %u will be
  substituted with the url given in the parameter.
  -----------------------------------------------------------------------*/
static void CopyAlgaeTemplateURL (char **dest, char *url)
{
  char *in;
  char *out;
  char *tmp;
  int url_len;
  int i;

  url_len = (url) ? strlen (url) : 0;

  /* allocate enough memory in the string */
  i = 0;
  in = annotAlgaeText;
  while (in)
    {
      tmp = strstr (in, "%u");
      if (tmp)
        {
          i++;
          in = tmp + 2;
        }
      else
        break;
    }
  *dest = (char *)TtaGetMemory (i * strlen (url) + strlen (annotAlgaeText) + 30);
  in = annotAlgaeText;
  out = *dest;
  while (*in != EOS)
    {
      if (*in == '%' && *(in + 1) == 'u')
        {
          /* copy the proto and the URL */
          sprintf (out, "%s", url);
          /* and update the pointers */
          out = out + url_len;
          in = in + 2;
        }
      else
        {
          *out = *in;
          in++;
          out++;
        }
    }
  *out = EOS;
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
  char *tmp, *ptr;

  /* initialize the annot global variables */
  ptr = TtaGetEnvString ("APP_HOME");
  annotDir = (char *)TtaGetMemory (strlen (ptr) + strlen ("annotations") + 2);
  sprintf (annotDir, "%s%c%s", ptr, DIR_SEP, "annotations");
  annotMainIndex = TtaStrdup (TtaGetEnvString ("ANNOT_MAIN_INDEX"));
  TtaGetEnvBoolean ("ANNOT_LAUTOLOAD", &annotLAutoLoad);
  TtaGetEnvBoolean ("ANNOT_RAUTOLOAD", &annotRAutoLoad);
  TtaGetEnvBoolean ("ANNOT_POSTUPDATE", &annotPOSTUpdate);

  tmp = TtaGetEnvString ("ANNOT_USER");
  if (tmp)
    annotUser = (char *)TtaConvertByteToMbs ((unsigned char *)tmp,
                                             TtaGetLocaleCharset ());
  else
    annotUser = NULL;
  tmp = TtaGetEnvString ("ANNOT_SERVERS");
  if (tmp)
    annotServers = CopyAnnotServers (tmp);
  else
    annotServers = NULL;
  tmp = TtaGetEnvString ("ANNOT_POST_SERVER");
  if (tmp)
    annotPostServer = TtaStrdup (tmp);
  else
    annotPostServer = TtaStrdup ("localhost");

  /* @@@ temporary custom query, as we could use the configuration menu  ***/
  annotCustomQuery = FALSE;
  annotAlgaeText = TtaStrdup (DEFAULT_ALGAE_QUERY);

  /* create the directory where we'll store the annotations if it
     doesn't exist */
  TtaMakeDirectory (annotDir);

  /* initializes raptor if we're using redland and not using bookmarks.
     Redland initializes raptor itself in that case */
#ifndef BOOKMARKS
  /* initializes raptor */
  raptor_init ();
#endif /* ! BOOKMARKS */
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

  /* frees raptor if we're using redland and not using bookmarks.
     Redland frees raptor itself in that case */
#ifndef BOOKMARKS
  raptor_finish ();
#endif /* ! BOOKMARKS */
}

/*-----------------------------------------------------------------------
  ANNOT_FreeAnnotResource
  Frees all the annotation resources that are associated with
  annotation annot (doesn't remove this annotation yet).
  -----------------------------------------------------------------------*/
static void ANNOT_FreeAnnotResource (Document source_doc, Element annotEl, 
                                     AnnotMeta *annot)
{
  XPointerContextPtr ctx;

  /* remove the annotation link in the source document */
  if (annotEl)
    {
      LINK_RemoveLinkFromSource (source_doc, annotEl);
      /* we relesct the region that was annotated */
      if (TtaGetSelectedDocument () == source_doc)
        TtaUnselect (source_doc);
      ctx = XPointer_parse (source_doc, annot->xptr);
      XPointer_select (ctx);
      XPointer_free (ctx);
    }
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
#ifdef ANNOT_ON_ANNOT
  Document doc_select, doc_thread;
#endif /* ANNOT_ON_ANNOT */

  /* reset the last_selected_annotation for this document */
  last_selected_annotation[doc] = NULL;

#ifdef ANNOT_ON_ANNOT
  /* reset the thread entry */
  doc_thread =  AnnotThread_searchThreadDoc (DocumentURLs[doc]);
  if (doc_thread > 0 && doc_thread != doc)
    {
      doc_select = ANNOT_GetThreadDoc (doc_thread);
      if (doc_select != 0)
        {
          ANNOT_ToggleThread (doc_thread, doc_select, FALSE);
        }
    }
#endif /* ANNOT_ON_ANNOT */

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
          AmayaCloseWindow (i, 1);
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
      char *body_url;
#ifdef ANNOT_ON_ANNOT
      ThotBool isThread = FALSE;
#endif /* ANNOT_ON_ANNOT */
      source_doc = DocumentMeta[doc]->source_doc;
      if (!IsW3Path(DocumentURLs[doc]))
        body_url = FixFileURL (DocumentURLs[doc]);
      else
        body_url = DocumentURLs[doc];
      annot = AnnotList_searchAnnot (AnnotMetaData[source_doc].annotations,
                                     body_url,
                                     AM_BODY_URL);

#ifdef ANNOT_ON_ANNOT
      if (!annot && AnnotThread[doc_thread].annotations)
        {
          annot = AnnotList_searchAnnot (AnnotThread[doc_thread].annotations,
                                         body_url,
                                         AM_BODY_URL);
          if (annot)
            isThread = 1;
        }
#endif /* ANNOT_ON_ANNOT */
      if (body_url != DocumentURLs[doc])
        TtaFreeMemory (body_url);

      if (annot)
        {
	  
          annotEl = SearchAnnotation (source_doc, annot->name);
          /* remove the annotation metadata and the annotation icon */
          ANNOT_FreeAnnotResource (source_doc, annotEl, annot);
          /* remove the annotation from the document's annotation list and 
             update it */
#ifdef ANNOT_ON_ANNOT
          if (isThread)
            AnnotList_delAnnot (&(AnnotThread[doc_thread].annotations),
                                annot->body_url, FALSE);
          else
#endif /* ANNOT_ON_ANNOT */
            AnnotList_delAnnot (&(AnnotMetaData[source_doc].annotations),
                                annot->body_url, FALSE);
        }
#ifdef ANNOT_ON_ANNOT
      if (doc_thread > 0 && doc_thread != doc)
        {
          ANNOT_DeleteThread (doc_thread);
          if (AnnotThread[doc_thread].annotations)
            ANNOT_BuildThread (doc_thread);
        }
#endif /* ANNOT_ON_ANNOT */
    }
  
  /* free the memory allocated for annotations */
  LINK_DelMetaFromMemory (doc);
  /* reset the state */
  AnnotMetaData[doc].local_annot_loaded = FALSE;
}


/*-----------------------------------------------------------------------
  RemoteLoad_callback
  -----------------------------------------------------------------------*/
void RemoteLoad_callback (int doc, int status, char *urlName, char *outputfile, 
                          char *proxyName, AHTHeaders *http_headers, void * context)
{
  REMOTELOAD_context *ctx;
  Document            source_doc;
  char               *source_doc_url, *ptr, *s;
  char               *source_body_url, *buffer;
  int                 l;

  /* restore REMOTELOAD contextext's */  
  ctx = (REMOTELOAD_context *) context;
  if (!ctx)
    return;
   
  ResetStop (doc);
  source_doc = ctx->source_doc;
  source_doc_url = ctx->source_doc_url;
  source_body_url = ctx->source_body_url;

  /* only load the annotation if the download was succesful and if
     the source document hasn't disappeared in the meantime */
  if (status == HT_OK &&
      DocumentURLs[source_doc] &&
      (Annot_isSameURL (DocumentURLs[source_doc], source_body_url)
       || (AnnotMetaData[source_doc].annot_url &&
           Annot_isSameURL (AnnotMetaData[source_doc].annot_url, source_doc_url))))
    {
      LINK_LoadAnnotationIndex (doc, ctx->remoteAnnotIndex, TRUE);
      ctx->remoteAnnotIndex = NULL; // the string is already freed
      /* clear the status line if there was no error*/
      TtaSetStatus (doc, 1,  "Done!", NULL);
    }
  else
    {
      ptr = HTTP_headers (http_headers, AM_HTTP_REASON);
      if (ptr)
        {
          s = TtaGetMessage (AMAYA, AM_ANNOT_INDEX_FAILURE);
          l = strlen (s) + strlen (ptr) + 4;
          buffer = (char *)TtaGetMemory (l);
          sprintf (buffer, "%s: %s", s, ptr);
          TtaSetStatus (doc, 1, buffer, NULL);
          TtaFreeMemory (buffer);
        }
      else
        TtaSetStatus (doc, 1,  TtaGetMessage (AMAYA, AM_ANNOT_INDEX_FAILURE), NULL);
    }
   
  TtaFreeMemory (source_body_url);
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
  char               *annotIndex;
  char               *annotURL;
  char               *tmp_url;
  char               *doc_url;
  char               *tmp_doc_url;
  char               *body_url;
  char               *tmp_body_url;
  char               *server;
  REMOTELOAD_context *ctx;
  int                 res;
  List               *ptr;
  ThotBool            is_active = FALSE;


  if (mode == AM_LOAD_NONE)
    return;

  /* the user may have aborted the request */
  if (!DocumentURLs[doc])
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


#ifdef ANNOT_ON_ANNOT
  if (DocumentTypes[doc] == docAnnot)
    doc_url = AnnotMetaData[doc].annot_url;
  else
#endif /* ANNOT_ON_ANNOT */
    doc_url = DocumentURLs[doc];
  tmp_doc_url = LocalToWWW (doc_url);
  if (!tmp_doc_url)
    tmp_doc_url = doc_url;

  body_url = DocumentURLs[doc];
  tmp_body_url = LocalToWWW (body_url);
  if (!tmp_body_url)
    tmp_body_url = body_url;

  /*
   * load the local annotations if there's no annotserver or if
   * annotServers include the localhost
   */
  if ((mode & AM_LOAD_LOCAL)
      && (!annotServers || AnnotList_search (annotServers, "localhost")))
    {
      /* for annotation documents, we store the index using the annot_url, not the
         body_url */
      if (DocumentTypes[doc] == docAnnot)
        tmp_url = doc_url;
      else
        tmp_url = body_url;
      annotIndex = LINK_GetAnnotationIndexFile (tmp_url);
      LINK_LoadAnnotationIndex (doc, annotIndex, TRUE);
      annotIndex = NULL;
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
          server = (char *)ptr->object;
          ptr = ptr->next;
          if (!server || !strcasecmp (server, "localhost")
              || server[0] == '-' || server[0] == EOS)
            continue;
          /* create the context for the callback */
          ctx = (REMOTELOAD_context*)TtaGetMemory (sizeof (REMOTELOAD_context));
          memset (ctx,  0, sizeof (REMOTELOAD_context));
          /* make some space to store the remote file name */
          ctx->remoteAnnotIndex = (char *)TtaGetMemory (MAX_LENGTH);
          ctx->remoteAnnotIndex[0]  = EOS;
          /* store the source document infos */
          ctx->source_doc = doc;
          ctx->source_doc_url = TtaStrdup (tmp_doc_url);
          ctx->source_body_url = TtaStrdup (tmp_body_url);

          if (!annotCustomQuery || !annotAlgaeText || 
              annotAlgaeText[0] == EOS)
            {
#ifdef ANNOT_ON_ANNOT
              if (DocumentTypes[doc] == docAnnot)
                {
                  annotURL = (char *)TtaGetMemory (strlen (tmp_doc_url)
                                                   + strlen (tmp_body_url)
                                                   + sizeof ("w3c_annotates=&w3c_replyTree=")
                                                   + 50);
                  /* we annotate the body, but reply to the annotation */
                  sprintf (annotURL, "w3c_annotates=%s&w3c_replyTree=%s", 
                           tmp_body_url, tmp_doc_url);
                }
              else
#endif /* ANNOT_ON_ANNOT */
                {
                  annotURL = (char *)TtaGetMemory (strlen (tmp_body_url)
                                                   + sizeof ("w3c_annotates=")
                                                   + 50);
                  sprintf (annotURL, "w3c_annotates=%s", tmp_body_url);
                }
            }
          else
            /* substitute the %u for DocumentURLs[doc] and go for it! */
            /* @@ we have a potential mem bug here as I'm not computing
               the exact size */
            /* @@ what shall we do with annotations and replies? */
            CopyAlgaeTemplateURL (&annotURL, tmp_doc_url);
	  
          /* launch the request */
          if (!is_active)
            {
              is_active = TRUE;
              ANNOT_UpdateTransfer (doc);
            }
          else
            UpdateTransfer (doc);
          res = GetObjectWWW (doc, 0, server, annotURL,
                              ctx->remoteAnnotIndex,
                              AMAYA_ASYNC | AMAYA_FLUSH_REQUEST,
                              NULL, NULL, 
                              (void (*)(int, int, char*, char*, char*, const AHTHeaders*, void*)) RemoteLoad_callback,
                              (void *) ctx, NO, "application/xml");
          TtaFreeMemory (annotURL);
        }
    }
  if (tmp_body_url != body_url)
    TtaFreeMemory (tmp_body_url);
  
  if (tmp_doc_url != doc_url)
    TtaFreeMemory (tmp_doc_url);

}

/*-----------------------------------------------------------------------
  ANNOT_AutoLoad
  -----------------------------------------------------------------------
  -----------------------------------------------------------------------*/
void ANNOT_AutoLoad (Document doc, View view)
{
  AnnotLoadMode mode = AM_LOAD_NONE;

  if (annotLAutoLoad)
    mode = (AnnotLoadMode)(mode | AM_LOAD_LOCAL);
  if (annotRAutoLoad)
    mode = (AnnotLoadMode)(mode | AM_LOAD_REMOTE);

#ifdef ANNOT_ON_ANNOT
  /* link the new metadata to the discussion thread */
  AnnotThread_link2ThreadDoc (doc);
#endif /* ANNOT_ON_ANNOT */

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
#ifdef ANNOT_ON_ANNOT
  /* link the new metadata to the discussion thread */
  AnnotThread_link2ThreadDoc (doc);
#endif /* ANNOT_ON_ANNOT */

  ANNOT_Load2 (doc, view, (AnnotLoadMode)(AM_LOAD_LOCAL | AM_LOAD_REMOTE));
}

/*-----------------------------------------------------------------------
  ANNOT_Reload
  Redisplays in the source document an already downloaded annotation index
  -----------------------------------------------------------------------*/
void ANNOT_Reload (Document doc, View view)
{
  LINK_ReloadAnnotationIndex (doc, view);
}

/*-----------------------------------------------------------------------
  ANNOT_Create
  Creates an annotation on the selected text. If there's no selected
  text, it doesn't do anything.

  ** To do: protect for annot/annot agains the user putting an annotation
  on other parts of the annotation than the body
  -----------------------------------------------------------------------*/
void ANNOT_Create (Document doc, View view, AnnotMode mode)
{
  Document           doc_annot, doc_thread;
  Element            el = NULL;
  AnnotMeta         *annot;
  XPointerContextPtr ctx;
  char              *xptr = NULL, *tfid = NULL;
  int                firstC, lastC;
  ThotBool           useDocRoot = (mode & ANNOT_useDocRoot) != 0;
  ThotBool           isReplyTo = (mode & ANNOT_isReplyTo) != 0;

  /* we can only annotate some types of documents and saved documents */
  if (!ANNOT_CanAnnotate (doc))
    return;
  
  /* only accept reply tos in annotations */
  if (DocumentTypes[doc] != docAnnot && isReplyTo)
    return;
  if (!annotUser || *annotUser == EOS)
    {
      InitInfo (TtaGetMessage (AMAYA, AM_NEW_ANNOTATION),
                TtaGetMessage (AMAYA, AM_NO_ANNOT_USER));
      return;
    }

  if (!useDocRoot && TtaGetSelectedDocument () != doc)
    return; /* Error: nothing selected in this document */

  /* if not docText file */
  if (DocumentTypes[doc] != docText)
    {
      /* Annotation XPointer */
      xptr = XPointer_build (doc, 1, useDocRoot);
      /* if we can't compute the XPointer, we return (we could make a
         popup message box stating what happened) */
      if (!xptr)
        {
          TtaSetStatus (doc, 1,
                        TtaGetMessage (AMAYA, AM_CANNOT_ANNOTATE),
                        NULL);
          return;
        }
    }
  else
    {
      tfid = TextPlainId_build (doc,view,useDocRoot);
      if (!tfid)
        {
          TtaSetStatus (doc, 1,
                        TtaGetMessage (AMAYA, AM_CANNOT_ANNOTATE),
                        NULL);
          return;
        } 
    }

  
  /* create the document that will store the annotation */
  if ((doc_annot = ANNOT_NewDocument (doc, mode)) == 0)
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
  annot = LINK_CreateMeta (doc, doc_annot, mode);
  /* if not docText */
  if (DocumentTypes[doc] != docText)
    /* update the XPointer */
    annot->xptr = xptr;
  else
    /* update the plain text identifier*/
    annot->xptr = tfid;
  /* initialize everything */
  mode = (AnnotMode)(mode | ANNOT_initATitle | ANNOT_initBody);
  ANNOT_InitDocumentStructure (doc, doc_annot, annot, mode);
#ifdef _WX
  // set the default icon
  TtaSetPageIcon (doc_annot, 1, NULL);
#endif /* _WX */
  /* turn on/off entries in the menu bar */
  //UpdateContextSensitiveMenus (doc, 1);
  /* show the thread in the source document */
#ifdef ANNOT_ON_ANNOT
  if (isReplyTo)
    {
      /* we should add here the current document where the thread is viewed */
      doc_thread =  AnnotThread_searchThreadDoc (DocumentURLs[doc_annot]);
      if (doc_thread > 0)
        {
          DocumentMeta[doc_annot]->source_doc = doc_thread;
          ANNOT_AddThreadItem (doc_thread, annot);
        }
    }
#endif /* ANNOT_ON_ANNOT */

  /* add the annotation icon to the source document */
  TtaGiveFirstSelectedElement (doc_annot, &el, &firstC, &lastC);
  TtaUnselect (doc);
  LINK_AddLinkToSource (doc, annot);
  /* reselect the annotated text starting from the xpointer */
  if (!useDocRoot)
    {
      ctx = XPointer_parse (doc, annot->xptr);
      XPointer_select (ctx);
      XPointer_free (ctx);
    }
  if (el)
    TtaSelectElement (doc_annot, el);
}

/*-----------------------------------------------------------------------
  ANNOT_Post_callback
  -----------------------------------------------------------------------*/
void ANNOT_Post_callback (int doc, int status, char *urlName, char *outputfile,
                          char *proxyName, AHTHeaders *http_headers, void * context)
{
  /* get the server's reply */
  /* update the source doc link */
  /* delete the temporary file */
  /* the context gives the link's name and we'll use it to look up the
     document ... */
  REMOTELOAD_context *ctx;
  Document            source_doc;
  /* For saving threads */
  ThotBool            isReplyTo;
#ifdef ANNOT_ON_ANNOT
  ThotBool            is_root;
#endif
  ThotBool            do_update;
  char               *previous_body_url, *ptr, *s;
  char               *previous_annot_url, *buffer;
  int                 l;

  /* restore REMOTELOAD contextext's */  
  ctx = (REMOTELOAD_context *) context;
  if (!ctx)
    return;

  ResetStop (doc);
  source_doc = ctx->source_doc;
  isReplyTo = ctx->isReplyTo;

  /* only update the metadata if the POST was succesful and if
     the source and annot documents haven't disappeared in the meantime */
  if (status == HT_OK
      && DocumentURLs[source_doc] 
      && !strcmp (DocumentURLs[source_doc], ctx->source_doc_url)
      && DocumentURLs[doc]
      && !strcmp (DocumentURLs[doc], ctx->annot_url))
    {
      AnnotMeta *annot = GetMetaData (source_doc, doc);

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

              /*
              ** Phase 1: update the local annotation metadata and
              ** make a copy of the updated body and annotation URLs
              */

              /* updated the annot_url and  inReplyTo values if they have changed*/
              if (returned_annot->annot_url
                  && (!annot->annot_url || strcasecmp (annot->annot_url, returned_annot->annot_url)))
                {
                  TtaFreeMemory (annot->annot_url);
                  annot->annot_url = returned_annot->annot_url;
                  returned_annot->annot_url = NULL;
                }

              /* update the shortcut to annot_url that we use for the
                 actual operations */
              if (annot->annot_url && strcasecmp (annot->annot_url, AnnotMetaData[doc].annot_url))
                {
                  previous_annot_url = AnnotMetaData[doc].annot_url;
                  AnnotMetaData[doc].annot_url = TtaStrdup (annot->annot_url);
                }
              else
                previous_annot_url = AnnotMetaData[doc].annot_url;

              /* update the annotation body_url */
              if (returned_annot->body_url 
                  && (!annot->body_url
                      || strcasecmp (annot->body_url, returned_annot->body_url)))
                {
                  /* TtaFileUnlink (annot->body_url); */
                  previous_body_url = annot->body_url;
                  /* update the metadata of the annotation */
                  annot->body_url = returned_annot->body_url;
                  returned_annot->body_url = NULL;
                }
              else
                previous_body_url = annot->body_url;

#ifdef ANNOT_ON_ANNOT
              /* see if the inReplyTo URL has changed.  Iif we have an inReplyto, we use it instead of
                 annot_url (annot_url comes from the annotates
                 property */
              if (returned_annot->inReplyTo 
                  && (!annot->inReplyTo || strcasecmp (annot->inReplyTo, returned_annot->inReplyTo)))
                {
                  TtaFreeMemory (annot->inReplyTo);
                  annot->inReplyTo = returned_annot->inReplyTo;
                  returned_annot->inReplyTo = NULL;

                }
#endif /* ANNOT_ON_ANNOT */

              /*
              ** Phase 2: Update the annotation metadata of other annotations that
              ** have a dependency with this one.
              */

              /* update the Document metadata to point to the new
                 body too */
	       
              /* replace the body only if it changed.  This means that a local annot was just 
                 made a shared annot. We update the metadata stored in memory and update the
                 annotations indexes as needed */
              if (previous_body_url != annot->body_url)
                {
                  /* update the anchor in the source doc */
                  ReplaceLinkToAnnotation (source_doc, annot->name, 
                                           annot->body_url);

                  /* update the annotation icon to the new type if it has changed */
                  if (!isReplyTo)
                    LINK_UpdateAnnotIcon (source_doc, annot);
 
#ifdef ANNOT_ON_ANNOT
                  /* update the references to the local annotation reference in the local thread (and
                     root of thread too) */
                  if (isReplyTo && AnnotMetaData[source_doc].thread)
                    {
                      AnnotThread_UpdateReplyTo (AnnotMetaData[source_doc].thread->annotations,
                                                 AnnotMetaData[doc].annot_url,
                                                 previous_annot_url);
                      /* update the item in the thread list to point to the new body */
                      ANNOT_UpdateThreadItem (doc, annot, previous_body_url);
                    }
#endif /* ANNOT_ON_ANNOT */
		   
		   /* save the local annotation index minus this annotation. If there is nothing
		      to save, delete the index entry */
                  do_update = FALSE;
                  if (AnnotList_localCount (AnnotMetaData[source_doc].annotations) > 0)
                    do_update = TRUE;
#ifdef ANNOT_ON_ANNOT		       
                  /* erase the reference to the local annotation reference in the local thread */
                  else if (AnnotMetaData[source_doc].thread
                           && AnnotList_localCount (AnnotMetaData[source_doc].thread->annotations) > 0)
                    do_update = TRUE;
#endif /* ANNOT_ON_ANNOT */
                  if (do_update)
                    LINK_SaveLink (source_doc, isReplyTo);
                  else
                    LINK_DeleteLink (source_doc, isReplyTo);
		   
#ifdef ANNOT_ON_ANNOT		       
                  /*
                  ** we now update the annotations (or replies) related to this annotation 
                  */

                  /* annotations having this annotation as a source */
                  AnnotThread_UpdateAnnotates (AnnotMetaData[doc].annotations,
                                               AnnotMetaData[doc].annot_url,
                                               previous_annot_url);
                  /* This annotation may be the root of a thread, so we update all its replies */
                  if (!isReplyTo 
                      && AnnotThread[doc].annotations
                      && AnnotThread[doc].annotations != AnnotThread[source_doc].annotations
                      && !strcasecmp (previous_annot_url, AnnotThread[doc].rootOfThread))
                    {
                      is_root = TRUE;
                      /* update the reply to and root of Thread references */
                      AnnotThread_UpdateReplyTo (AnnotMetaData[doc].thread->annotations,
                                                 AnnotMetaData[doc].annot_url,
                                                 previous_annot_url);
                    }
                  else
                    is_root = FALSE;

                  /* update the index entry if we posted an annotation that has other annotations
                     glued to it */
                  /* @@ JK: but we have not yet downloaded the annotations */
                  do_update = FALSE;
                  if (AnnotMetaData[doc].local_annot_loaded
                      && AnnotList_localCount (AnnotMetaData[doc].annotations) > 0)
                    do_update = TRUE;
                  else if (is_root
                           && AnnotMetaData[doc].thread != AnnotMetaData[source_doc].thread
                           && AnnotList_localCount (AnnotMetaData[doc].thread->annotations) > 0)
                    do_update = TRUE;
                  else
                    {
                      char *annotIndex;
		       
                      annotIndex = LINK_GetAnnotationIndexFile (previous_annot_url);
                      if (annotIndex && strcasecmp (previous_annot_url, AnnotMetaData[doc].annot_url))
                        do_update = TRUE;
                      if (annotIndex)
                        TtaFreeMemory (annotIndex);
                    }
                  if (do_update)
                    LINK_UpdateAnnotationIndexFile (previous_annot_url, AnnotMetaData[doc].annot_url);

                  /* we now update the index file or delete it if needed */
                  do_update = FALSE;
                  if (AnnotList_localCount (AnnotMetaData[doc].annotations) > 0)
                    do_update = TRUE;
                  /* erase the reference to the local annotation reference in the local thread */
                  else if (is_root &&
                           AnnotMetaData[doc].thread != AnnotMetaData[source_doc].thread &&
                           AnnotList_localCount (AnnotMetaData[doc].thread->annotations) > 0)
                    do_update = TRUE;

                  if (do_update)
                    LINK_SaveLink (doc, isReplyTo);
                  else
                    LINK_DeleteLink (doc, isReplyTo);

		   
                  if (is_root)
                    {
                      /* update the thread entry */
                      TtaFreeMemory (AnnotThread[doc].rootOfThread);
                      AnnotThread[doc].rootOfThread = TtaStrdup (AnnotMetaData[doc].annot_url);
                    }

#endif /* ANNOT_ON_ANNOT */

		   /*
		   ** Last step, update the DocumentURLs entry for this annotation
		   */
                  TtaFreeMemory (DocumentURLs[doc]);
                  DocumentURLs[doc] = TtaStrdup (annot->body_url);
                }

              if (previous_annot_url != AnnotMetaData[doc].annot_url)
                TtaFreeMemory (previous_annot_url);

              if (previous_body_url != annot->body_url)
                TtaFreeMemory (previous_body_url);

              if (listP->next)
                fprintf (stderr, "PostCallback: POST returned more than one annotation\n");
              AnnotList_free (listP);
              /* if we were posting a localfile, remove this file
                 (and update the indexes */
              if (ctx->localfile)
                TtaFileUnlink (ctx->localfile);
            }
          TtaSetDocumentUnmodified (doc); 
          /* switch Amaya buttons and menus */
          DocStatusUpdate (doc, FALSE);
        }
      TtaFileUnlink (ctx->remoteAnnotIndex);
    }
  else /* there was error */
    {
      ptr = HTTP_headers (http_headers, AM_HTTP_REASON);
      if (ptr)
        {
          s = TtaGetMessage (AMAYA, AM_ANNOT_POST_FAILURE);
          l = strlen (s) + strlen (ptr) + 4;
          buffer = (char *)TtaGetMemory (l);
          sprintf (buffer, "%s: %s", s, ptr);
          TtaSetStatus (doc, 1, buffer, NULL);
          TtaFreeMemory (buffer);
        }
      else
        TtaSetStatus (doc, 1,  TtaGetMessage (AMAYA, AM_ANNOT_POST_FAILURE), NULL);
    }

  /* erase the rdf container */
  TtaFileUnlink (ctx->rdf_file);

  /* free all memory associated with the context */
  TtaFreeMemory (ctx->source_doc_url);
  TtaFreeMemory (ctx->annot_url);
  TtaFreeMemory (ctx->rdf_file);
  TtaFreeMemory (ctx->remoteAnnotIndex);
  TtaFreeMemory (ctx->localfile);
  TtaFreeMemory (ctx);
}

/*-----------------------------------------------------------------------
  ANNOT_Post
  -----------------------------------------------------------------------*/
void ANNOT_Post (Document doc, View view)
{
  REMOTELOAD_context *ctx;
  int res;
  char *rdf_file;
  char *url;
  ThotBool free_url;
  AnnotMeta *annot = NULL;
  Document source_doc;
  ThotBool isReplyTo;
  ThotBool new_annotation;

  /* @@ JK: while the post item isn't desactivated on the main window,
     forbid annotations from elsewhere */
  if (strcmp (TtaGetSSchemaName (TtaGetDocumentSSchema (doc)), "Annot"))
    return;

  if (!annotPostServer || *annotPostServer == EOS)
    return;

  isReplyTo = Annot_IsReplyTo (doc);

  /* create the RDF container */
  rdf_file = ANNOT_PreparePostBody (doc);
  if (!rdf_file)
    /* there was an error while preparing the tmp.rdf file */
    return;
  /* create the context for the callback */
  ctx = (REMOTELOAD_context*)TtaGetMemory (sizeof (REMOTELOAD_context));
  memset (ctx,  0, sizeof (REMOTELOAD_context));
  /* make some space to store the remote file name */
  ctx->remoteAnnotIndex = (char *)TtaGetMemory (MAX_LENGTH);
  ctx->remoteAnnotIndex[0]  = EOS;
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
  ctx->isReplyTo = isReplyTo;

  /* compute the URL */
  if (IsW3Path (DocumentURLs[doc]))
    {
      /* we're saving a modification to an existing annotation */
      new_annotation = FALSE;
      
      /* find the annotation metadata that corresponds to this annotation */
      annot = AnnotList_searchAnnot (AnnotMetaData[source_doc].annotations,
                                     DocumentURLs[doc], AM_BODY_URL);

#ifdef ANNOT_ON_ANNOT
      if (!annot && AnnotMetaData[source_doc].thread)
        annot = AnnotList_searchAnnot (AnnotMetaData[source_doc].thread->annotations,
                                       DocumentURLs[doc], AM_BODY_URL);
#endif /* ANNOT_ANNOT */
      
      if (!annot)
        /* @@ JK: give some error message, free the ctx */
        return;
    }
  else
    {
      /* we're posting a new annotation */
      new_annotation = TRUE;
    }

  /* launch the request */
  ANNOT_UpdateTransfer (doc);
  if (new_annotation)
    {
      url = annotPostServer;
      free_url = FALSE;

      res = GetObjectWWW (doc, 0, url, rdf_file,
                          ctx->remoteAnnotIndex,
                          AMAYA_FILE_POST | AMAYA_ASYNC | AMAYA_FLUSH_REQUEST,
                          NULL, NULL, 
                          (void (*)(int, int, char*, char*, char*, const AHTHeaders*, void*))ANNOT_Post_callback,
                          (void *) ctx, NO, "application/xml");
    } 
  else
    {
      /* clear the cache entries for the annotation and its body */
      ClearCacheEntry (annot->annot_url);
      ClearCacheEntry (annot->body_url);

      if (annotPOSTUpdate) /* left here to keep a backward compatiblity in case
                              that servers don't support the PUT yet. */
        {
          /* we're saving a modification to an existing annotation */
          url = (char *)TtaGetMemory (strlen (annotPostServer)
                                      + sizeof ("?replace_source=")
                                      + strlen (annot->annot_url)
                                      + sizeof ("&rdftype=")
                                      + strlen (ANNOTATION_CLASSNAME)
                                      + 1);
          sprintf (url,"%s?replace_source=%s&rdftype=%s",
                   annotPostServer,
                   annot->annot_url,
                   ANNOTATION_CLASSNAME);
          free_url = TRUE;
	  
          res = GetObjectWWW (doc, 0, url, rdf_file,
                              ctx->remoteAnnotIndex,
                              AMAYA_FILE_POST | AMAYA_ASYNC | AMAYA_FLUSH_REQUEST,
                              NULL,
                              NULL, 
                              (void (*)(int, int, char*, char*, char*, const AHTHeaders*, void*))  ANNOT_Post_callback,
                              (void *) ctx, NO, "application/xml");
        }
      else /* USE PUT */
        {
          /* we're saving a modification to an existing annotation */
          url = annot->annot_url;
          free_url = FALSE;
	  
          res = PutObjectWWW (doc, rdf_file, url, "application/xml", ctx->remoteAnnotIndex,
                              AMAYA_SIMPLE_PUT | AMAYA_SYNC | AMAYA_NOCACHE |  AMAYA_FLUSH_REQUEST,
                              (void (*)(int, int, char*, char*, char*, const AHTHeaders*, void*))  ANNOT_Post_callback, (void *) ctx);
        }
    }

  if (free_url)
    TtaFreeMemory (url);
  /* @@ JK: here we should delete the context or call the callback in case of
     error */
  if (res)
    TtaSetStatus (doc, 1,  TtaGetMessage (AMAYA, AM_ANNOT_POST_FAILURE), NULL);
}

/*----------------------------------------------------------------------
  ANNOT_SaveDocument
  Frontend function that decides if an annotation should be saved and if
  it should be saved remotely or locally. It then calls the appropriate 
  function to do this operation.
  ----------------------------------------------------------------------*/
void ANNOT_SaveDocument (Document doc_annot, View view)
{
  char *filename;
  ThotBool isReplyTo;

  if (!TtaIsDocumentModified (doc_annot))
    return; /* prevent Thot from performing normal save operation */

  if (IsW3Path (DocumentURLs[doc_annot]))
    {
      /* a remote save */
      ANNOT_Post (doc_annot, view);
    }
  else
    {
      /* a local save */
      /* is this a reply to an annotation ? */
      isReplyTo = Annot_IsReplyTo (doc_annot);
      /* we skip the file: prefix if it's present */
      filename = (char *)TtaGetMemory (strlen (DocumentURLs[doc_annot]) + 1);
      NormalizeFile (DocumentURLs[doc_annot], filename, AM_CONV_ALL);
      if (ANNOT_LocalSave (doc_annot, filename))
        {
          TtaSetDocumentUnmodified (doc_annot);
          /* switch Amaya buttons and menus */
          /* DocStatusUpdate (doc_annot, FALSE); */
          LINK_SaveLink (DocumentMeta[doc_annot]->source_doc, isReplyTo);
        }
      TtaFreeMemory (filename); 
    }
}

/*-----------------------------------------------------------------------
  ANNOT_SelectSourceDoc
  If the user clicked on an annotation link in the source document,
  it highlights the annotated text.
  -----------------------------------------------------------------------*/
void ANNOT_SelectSourceDoc (int doc, Element sel)
{
  Element          el;
  ElementType      elType;
  AttributeType    attrType;
  Attribute	   attr;
  int              length;
  char            *annot_url;

  /* reset the last selected annotation ptr */
  last_selected_annotation[doc] = NULL;

  /* is it an annotation link? */
  elType = TtaGetElementType (sel);
  if (strcmp (TtaGetSSchemaName (elType.ElSSchema), "XLink"))
    return;

  /* if the user selected the annotation icon, jump to the 
     parent */
  if (elType.ElTypeNum == XLink_EL_PICTURE_UNIT)
    {
      el = TtaGetParent (sel);
      elType = TtaGetElementType (el);
    }
  else
    el = sel;

  if (elType.ElTypeNum != XLink_EL_XLink)
    return;

  /* get the URL of the annotation body */
  attrType.AttrSSchema = elType.ElSSchema;
  attrType.AttrTypeNum = XLink_ATTR_href_;
  attr = TtaGetAttribute (el, attrType);
  if (!attr)
    return;
  length = TtaGetTextAttributeLength (attr) + 1;
  annot_url = (char *)TtaGetMemory (length);
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
void Annot_RaiseSourceDoc_callback (int doc, int status, char *urlName,
                                    char *outputfile, char *proxyName,
                                    AHTHeaders *http_headers, void * context)
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
#ifdef ANNOT_ON_ANNOT
      /* if it's an annotation document and it has a thread view, select
         the thread item */
      if (ctx->has_thread)
        ANNOT_ToggleThread (ctx->doc_annot, doc, TRUE);
      
#endif /* ANNOT_ON_ANNOT */
    }
  else
    last_selected_annotation[doc] = NULL;

  if (ctx->url)
    TtaFreeMemory (ctx->url);
  TtaFreeMemory (ctx);
}

/*-----------------------------------------------------------------------
  Annot_RaiseSourceDoc
  The user has double clicked on the annot link to the source document
  -----------------------------------------------------------------------*/
ThotBool Annot_RaiseSourceDoc (NotifyElement *event)
{
  Document         doc_annot;
  Element          el;
  ElementType      elType;
  AttributeType    attrType;
  Attribute	   HrefAttr;
  ThotBool	   docModified;
  ThotBool         has_thread = FALSE;
  int              length;
  Document         targetDocument;
  char            *url = NULL;
  int              method;
  int              rel_doc;
  RAISESOURCEDOC_context *ctx;
#ifdef ANNOT_ON_ANNOT
  Document         tmp_doc;
  char             target[MAX_LENGTH];
#endif /* ANNOT_ON_ANNOT */

  /* initialize from the context */
  el = event->element;
  doc_annot = event->document;
  docModified = TtaIsDocumentModified (doc_annot);

  elType = TtaGetElementType (el);
#ifdef ANNOT_ON_ANNOT
  /* only do this action for thread items and for source document elements */

  if (elType.ElTypeNum != Annot_EL_SourceDoc)
    {
      elType.ElTypeNum = Annot_EL_Thread_item;
      el = TtaSearchTypedElement (elType, SearchBackward, el);
      if (!el)
        return FALSE;  /* let Thot do its usual operations */
      has_thread = TRUE;
    }
  else
#endif /* ANNOT_ON_ANNOT */
    if (elType.ElTypeNum != Annot_EL_SourceDoc)
      return FALSE;

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
      url = (char *)TtaGetMemory (length);
      TtaGiveTextAttributeValue (HrefAttr, url, &length);
      /*** DO WE HAVE TO CONVERT THIS UTF-8 STRING??? */
    }

#ifdef ANNOT_ON_ANNOT
  /* unless we're browsing the document corresponding to this
     thread item, we close it */
  tmp_doc = ANNOT_GetThreadDoc (doc_annot);
  
  if (tmp_doc != 0 && !Annot_isSameURL (DocumentURLs[tmp_doc], url)
      && doc_annot != tmp_doc)
    {
      ANNOT_ToggleThread (doc_annot, tmp_doc, FALSE);
      AmayaCloseWindow (tmp_doc, 1);
    }

  /* @@ JK: a patch so that we can follow the reverse link */
  if (!has_thread)
    {
      tmp_doc = Annot_IsDocumentLoaded (doc_annot, url, NULL);
      if (tmp_doc) 
        {
          char *tmp_ptr;
	  
          ExtractTarget (url, target);
          tmp_ptr = (char *)TtaGetMemory (strlen (DocumentURLs[tmp_doc]) + strlen (target) + 2);  
          sprintf (tmp_ptr, "%s#%s", DocumentURLs[tmp_doc], target);
          TtaFreeMemory (url);
          url = tmp_ptr;
        }
    }

#endif /* ANNOT_ON_ANNOT */

  if (!docModified)
    {
      TtaSetDocumentUnmodified (doc_annot);
      DocStatusUpdate (doc_annot, docModified);
    }
  
  /* @@ and now jump to the annotated document and put it on top,
     jump to the anchor... and if the document isn't there, download it? */
  ctx = (RAISESOURCEDOC_context*)TtaGetMemory (sizeof (RAISESOURCEDOC_context));
  ctx->url = url;
  ctx->doc_annot = doc_annot;
  ctx->has_thread = has_thread;

  if (has_thread)
    {
      rel_doc = 0;
      method = CE_ANNOT;
    }
  else
    {
      rel_doc = doc_annot;
      method = CE_ABSOLUTE;
    }
  targetDocument = GetAmayaDoc (url, NULL, rel_doc,
                                doc_annot, (ClickEvent)method, FALSE, 
                                (void (*)(int, int, char*, char*, char*, const AHTHeaders*, void*)) Annot_RaiseSourceDoc_callback,
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
void ANNOT_Delete_callback (int doc, int status, char *urlName, char *outputfile, 
                            char *proxyName, AHTHeaders *http_headers, void * context)
{
  DELETE_context *ctx;
  Document        source_doc;
  Document        annot_doc;
  char           *annot_url;
  char           *output_file, *ptr, *s, *buffer;
  int            l;
  Element         annotEl;
  AnnotMeta      *annot;
  ThotBool        annot_is_remote;
  ThotBool        delete_annot = TRUE;
  /* for deleting threads */
  ThotBool        isReplyTo;

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
#ifdef ANNOT_ON_ANNOT
  isReplyTo = annot->inReplyTo != NULL;
#else
  isReplyTo = FALSE;
#endif /* ANNOT_ON_ANNOT */

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
          AmayaCloseWindow (annot_doc, 1);
        }

      if (delete_annot)
        {
#ifdef ANNOT_ON_ANNOT
          if (annot->inReplyTo)
            AnnotList_delAnnot (&(AnnotMetaData[source_doc].thread->annotations),
                                annot_url, annot_is_remote);      
          else
#endif /* ANNOT_ON_ANNOT */
            {
              /* remove the annotation metadata and the annotation icon */
              ANNOT_FreeAnnotResource (source_doc, annotEl, annot);
              /* remove the annotation from the document's annotation list and 
                 update it */
              AnnotList_delAnnot (&(AnnotMetaData[source_doc].annotations),
                                  annot_url, annot_is_remote);
            }
        }

      /* update the annotation index or delete it if it's empty */
      if (
#ifdef ANNOT_ON_ANNOT
          (AnnotMetaData[source_doc].thread
           && AnnotList_localCount (AnnotMetaData[source_doc].thread->annotations) > 0) ||
#endif /* ANNOT_ON_ANNOT */
          AnnotList_localCount (AnnotMetaData[source_doc].annotations) > 0)
        LINK_SaveLink (source_doc, isReplyTo);
      else
        LINK_DeleteLink (source_doc, isReplyTo);
      
#ifdef ANNOT_ON_ANNOT
      /* erase and redisplay the thread items */
      ANNOT_DeleteThread (source_doc);
      if (AnnotThread[source_doc].annotations)
        ANNOT_BuildThread (source_doc);
#endif /* ANNOT_ON_ANNOT */
    }
  else 
    {
      ptr = HTTP_headers (http_headers, AM_HTTP_REASON);
      if (ptr)
        {
          s = TtaGetMessage (AMAYA, AM_ANNOT_DELETE_FAILURE);
          l = strlen (s) + strlen (ptr) + 4;
          buffer = (char *)TtaGetMemory (l);
          sprintf (buffer, "%s: %s", s, ptr);
          TtaSetStatus (doc, 1, buffer, NULL);
          TtaFreeMemory (buffer);
        }
      else
        TtaSetStatus (doc, 1, TtaGetMessage (AMAYA, AM_ANNOT_DELETE_FAILURE), NULL);
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
    TtaSetStatus (doc, 1,  TtaGetMessage (AMAYA, AM_ANNOT_DELETED), NULL);
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
  char          *annot_url;
  char          *annot_server;
  char          *char_ptr;
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
              annot_url = (char *)TtaGetMemory (strlen (DocumentURLs[doc])
                                                + strlen (DocumentMeta[doc]->form_data)
                                                + sizeof ("?")
                                                + 1);
              sprintf (annot_url, "%s?%s", DocumentURLs[doc],
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
#ifdef ANNOT_ON_ANNOT
      if (!annot && AnnotMetaData[source_doc].thread)
        annot = AnnotList_searchAnnot (AnnotMetaData[source_doc].thread->annotations,
                                       annot_url, AM_BODY_URL);
#endif /* ANNOT_ON_ANNOT */

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
      if (strcmp (TtaGetSSchemaName (elType.ElSSchema), "XLink")
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
      annot_url = (char *)TtaGetMemory (i);
      TtaGiveTextAttributeValue (attr, annot_url, &i);

      /* @@ JK: I may need to split the url and get the separate form_data */
      if (IsFilePath (annot_url))
        {
          annot_is_remote = FALSE;
          char_ptr = (char *)TtaGetMemory (strlen (annot_url));
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
    ctx->output_file = (char *)TtaGetMemory (MAX_LENGTH + 1);
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
          annot_server = (char *)list_ptr->object;
          if (strcasecmp (annot_server, "localhost")
              && !strncasecmp (annot_server, annot->annot_url, 
                               strlen (annot_server)))
            break;
          list_ptr = list_ptr->next;
        }

      if (annot_server)
        {
          /* launch the request */
          ANNOT_UpdateTransfer (doc);
          res = GetObjectWWW (doc, 0, annot->annot_url, NULL,
                              ctx->output_file,
                              AMAYA_ASYNC | AMAYA_DELETE | AMAYA_FLUSH_REQUEST,
                              NULL, NULL, 
                              (void (*)(int, int, char*, char*, char*, const AHTHeaders*, void*))  ANNOT_Delete_callback,
                              (void *) ctx,
                              NO, NULL);
          /* do something with res in case of error (invoke the callback? */
        }
      else
        {
          /* invoke the callback with an error */
          ANNOT_Delete_callback (doc, HT_ERROR, NULL, NULL, NULL, NULL, (void *) ctx);
        }
    }
  else
    {
      /* invoke the callback */
      ANNOT_Delete_callback (doc, HT_OK, NULL, NULL, NULL, NULL, (void *) ctx);
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
  char          *annot_url;
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
              annot_url = (char *)TtaGetMemory (strlen (DocumentURLs[doc])
                                                + strlen (DocumentMeta[doc]->form_data)
                                                + sizeof ("?")
                                                + 1);
              sprintf (annot_url, "%s?%s", DocumentURLs[doc], 
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
  ANNOT_UpdateTitle
  Updates the title of an annotation
  -----------------------------------------------------------------------*/
void Annot_UpdateTitle (NotifyOnTarget *event)
     //ThotBool  Annot_UpdateTitle (NotifyElement *event)
{
#ifdef ANNOT_ON_ANNOT
  AnnotMeta *annot;
  Document   doc;
  Element    el;
  int        len;
  Language   lang;


  el = event->element;
  doc = event->document;
  
  /* search the annotation metadata */
  annot = GetMetaData (DocumentMeta[doc]->source_doc, doc);
  
  if (!annot)
    return;/* FALSE; */ /* let Thot perform normal operation */

  /* update the metadata title field */
  if (annot->title)
    TtaFreeMemory (annot->title);
  el = TtaGetFirstChild (el);
  len = TtaGetTextLength (el) + 1;
  if (len == 1)
    annot->title = NULL;
  else
    {
      annot->title = (char *)TtaGetMemory (len);
      TtaGiveTextContent (el, (unsigned char *)annot->title, &len, &lang);
    }
  /* update the title of the window */
  ANNOT_UpdateThreadItem (doc, annot, annot->body_url);
  /*  return TRUE; */ /* don't let Thot perform normal operation */
#endif /* ANNOT_ON_ANNOT */
  /*  return FALSE; */ /* let Thot perform normal operation */
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






