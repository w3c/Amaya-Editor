
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

/* some state variables */
static CHAR_T *annotUser; /* user id for saving the annotation */
static CHAR_T *annotDir;   /* directory where we're storing the annotations */
static CHAR_T *annotServer; /* URL pointing to the annot server script */
static CHAR_T *annotMainIndex; /* index file where we give the correspondance
				between URLs and annotations */
static ThotBool annotAutoLoad; /* should annotations be downloaded
				  automatically? */


/* the structure used for storing the context of the 
   RemoteLoad_callback function */
typedef struct _REMOTELOAD_context {
  char *remoteAnnotIndex;
} REMOTELOAD_context;

/* the structure used for storing the context of the 
   RemoteLoad_callback function */
typedef struct _REMOTESAVE_context {
  char *remoteAnnotIndex;
} REMOTESAVE_context;

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
   GetAnnotServer
   Returns the URL of the annotation server
  -----------------------------------------------------------------------*/

#ifdef __STDC__
CHAR_T *GetAnnotServer (void)
#else /* __STDC__*/
CHAR_T *GetAnnotServer (void)
#endif /* __STDC__*/
{
  return annotServer;
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
  tmp = TtaGetEnvString ("ANNOT_SERVER");
  if (tmp)
    annotServer = TtaWCSdup (tmp);
  else
    annotServer = NULL;
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
  if (annotServer)  
    TtaFreeMemory (annotServer);
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

   if (status == 0)
     LINK_LoadAnnotationIndex (doc, ctx->remoteAnnotIndex);
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

  /* only HTML documents can be annotated */
  elType.ElSSchema = TtaGetDocumentSSchema (doc);
  if (ustrcmp(TtaGetSSchemaName (elType.ElSSchema), TEXT("HTML")))
    return;

  /*
   * Parsing test!
  */

  /*
   * load the local annotations 
   */
  annotIndex = LINK_GetAnnotationIndexFile (DocumentURLs[doc]);
#if 1
  LINK_LoadAnnotationIndex (doc, annotIndex);
#else
  LINK_LoadAnnotationIndex (doc, "/tmp/rdfquery.xml");
#endif
  TtaFreeMemory (annotIndex);

  /* 
   * load the remote annotation index
   */
  if (annotServer) 
    {
      /* create the context for the callback */
      ctx = TtaGetMemory (sizeof (REMOTELOAD_context));
      /* make some space to store the remote file name */
      ctx->remoteAnnotIndex = TtaGetMemory (MAX_LENGTH);
      /* "compute" the url we're looking up in the annotation server */
      annotURL = TtaGetMemory (MAX_LENGTH);
      if (!IsW3Path (DocumentURLs[doc]) &&
	  !IsFilePath (DocumentURLs[doc]))
	proto = "file://";
      else
	proto = "";
      sprintf (annotURL, "w3c_annotates=%s%s", proto, DocumentURLs[doc]);
      /* launch the request */
      res = GetObjectWWW (doc,
			  annotServer,
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

      TtaFreeMemory (ctx->remoteAnnotIndex);
      TtaFreeMemory (ctx);
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

#if 0
   if (status == 0)
     LINK_LoadAnnotationIndex (doc, ctx->remoteAnnotIndex);
   /* TtaFileUnlink (remoteAnnotIndex);*/
#endif
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
  char *annotUrl;

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
  /* "compute" the url we're looking up in the annotation server */
  annotUrl = TtaGetMemory (MAX_LENGTH);
  sprintf (annotUrl, "%s/%s", annotServer, "CGI/annotate");
  /* launch the request */
  res = GetObjectWWW (doc,
		      annotUrl,
		      TEXT("/tmp/rdf.tmp"),
		      ctx->remoteAnnotIndex,
		      AMAYA_FILE_POST | AMAYA_ASYNC | AMAYA_FLUSH_REQUEST,
		      NULL,
		      NULL, 
		      (void *)  ANNOT_Post_callback,
		      (void *) ctx,
		      NO,
		      NULL);
  TtaFreeMemory (annotUrl);

  if (res)			/* @@ */
    {
      fprintf (stderr, "Failed to post the annotation!\n");
    }

  TtaFreeMemory (ctx->remoteAnnotIndex);
  TtaFreeMemory (ctx);
}

/***************************************************
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
  CHAR_T *     annotName, fileName;
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
  annotName = SearchAttributeInElt (document, first, HTML_ATTR_NAME);
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

  printf ("(ANNOT_Delete) FIN\n");
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










