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

#include "annot.h"

/* some state variables */
static char *annotUser; /* user id for saving the annotation */
static char *annotDir;   /* directory where we're storing the annotations */
static char *annotServer; /* URL pointing to the annot server script */
static char *annotMainIndex; /* index file where we give the correspondance
				between URLs and annotations */
static ThotBool annotAutoLoad; /* should annotations be downloaded
				  automatically? */


/* the structure used for storing the context of the 
   RemoteLoad_callback function */
typedef struct _REMOTELOAD_context {
  char *remoteAnnotIndex;
} REMOTELOAD_context;

/*-----------------------------------------------------------------------
   Procedure GetAnnotUser
  -----------------------------------------------------------------------
   Returns the directory where the annotations of a given document are stored.
  -----------------------------------------------------------------------*/

#ifdef __STDC__
STRING GetAnnotUser (void)
#else /* __STDC__*/
STRING GetAnnotUser (void)
#endif /* __STDC__*/
{
  return annotUser;
}

/*-----------------------------------------------------------------------
   Procedure GetAnnotMainIndex
  -----------------------------------------------------------------------
   Returns the directory where the annotations of a given document are stored.
  -----------------------------------------------------------------------*/

#ifdef __STDC__
STRING GetAnnotMainIndex (void)
#else /* __STDC__*/
STRING GetAnnotationMainIndex(void)
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
STRING GetAnnotDir (void)
#else /* __STDC__*/
STRING GetAnnotDir (void)
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
  STRING tmp_dir;
  STRING tmp;

  /* setup the default registry values */
  tmp_dir = TtaGetEnvString ("APP_TMPDIR");
  annotDir = TtaGetMemory (strlen (tmp_dir) + strlen (ANNOT_DIR) + 2);
  sprintf (annotDir, "%s%c%s", tmp_dir, DIR_SEP, ANNOT_DIR);
  TtaSetEnvString ("ANNOT_DIR", annotDir, FALSE);
  TtaFreeMemory (annotDir);
  TtaSetEnvString ("ANNOT_MAIN_INDEX", ANNOT_MAIN_INDEX, FALSE);
  TtaSetEnvString ("ANNOT_USER", ANNOT_USER, FALSE);
  TtaSetEnvString ("ANNOT_AUTOLOAD", "yes", FALSE);

  /* initialize the annot global variables */
  annotDir = TtaStrdup (TtaGetEnvString ("ANNOT_DIR"));
  annotMainIndex = TtaStrdup (TtaGetEnvString ("ANNOT_MAIN_INDEX"));
  annotUser = TtaStrdup (TtaGetEnvString ("ANNOT_USER"));
  annotAutoLoad = !strcasecmp (TtaGetEnvString ("ANNOT_AUTOLOAD"), "yes");
  tmp = TtaGetEnvString ("ANNOT_SERVER");
  if (tmp)
    annotServer = TtaStrdup (tmp);
  else
    annotServer = NULL;
  /* @@ should be a nice mode! */
  mkdir (annotDir, 0777);
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
   Procedure ANNOT_AutoLoad
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
   Procedure RemoteLoad_callback
  -----------------------------------------------------------------------
  -----------------------------------------------------------------------*/
#ifdef __STDC__
void               RemoteLoad_callback (int doc, int status, 
					 STRING urlName,
					 STRING outputfile, 
					 STRING content_type,
					 void * context)
#else  /* __STDC__ */
void               RemoteLoad_callback (doc, status, urlName,
					outputfile, content_type, 
					context)
int newdoc;
int status;
STRING urlName;
STRING outputfile;
STRING content_type;
void *context;

#endif
{
   char  *remoteAnnotIndex;
   REMOTELOAD_context *ctx;

   /* restore REMOTELOAD contextext's */  
   ctx = (REMOTELOAD_context *) context;

   if (!ctx)
     return;
   remoteAnnotIndex = ctx->remoteAnnotIndex;
   TtaFreeMemory (ctx);
   if (status == 0)
     LINK_LoadAnnotations (doc, remoteAnnotIndex);
   /* TtaFileUnlink (remoteAnnotIndex);*/
   TtaFreeMemory (remoteAnnotIndex);
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
  char *annotUrl;
  REMOTELOAD_context *ctx;
  int res;

  elType.ElSSchema = TtaGetDocumentSSchema (doc);
  if (ustrcmp(TtaGetSSchemaName (elType.ElSSchema), TEXT("HTML")))
    return;

  /*
   * load the local annotations 
   */
  annotIndex = LINK_GetAnnotationIndexFile (DocumentURLs[doc]);
  LINK_LoadAnnotations (doc, annotIndex);
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
      annotUrl = TtaGetMemory (MAX_LENGTH);
      sprintf (annotUrl, "%s%s.index", annotServer, TtaGetDocumentName (doc));
      /* launch the request */
      res = GetObjectWWW (doc,
			  annotUrl,
			  NULL,
			  ctx->remoteAnnotIndex,
			  AMAYA_ASYNC | AMAYA_FLUSH_REQUEST,
			  NULL,
			  NULL, 
			  (void *)  RemoteLoad_callback,
			  (void *) ctx,
			  NO,
			  NULL);
      TtaFreeMemory (annotUrl);

      if (res)
	{
	  /* the document wasn't loaded off the web (because of an error),
	     we clear up the context */
	  TtaFreeMemory (ctx->remoteAnnotIndex);
	  TtaFreeMemory (ctx);
	}
    }
}

/*-----------------------------------------------------------------------
   Procedure ANNOT_Create (document, view)
  -----------------------------------------------------------------------
   Creates an annotation on the selected text. If there's no selected
   text, it doesn't do anything.
  -----------------------------------------------------------------------*/

#ifdef __STDC__
void ANNOT_Create (Document doc, View view)
#else /* __STDC__*/
void ANNOT_Create (document, view)
     Document document;
     View view;
#endif /* __STDC__*/
{
  ElementType elType;
  Element     first, last;
  STRING      labf, labl;
  int         c1, cN, i;
  Document    docAnnot;

  elType.ElSSchema = TtaGetDocumentSSchema (doc);
  if (ustrcmp(TtaGetSSchemaName (elType.ElSSchema), TEXT("HTML")))
    return;

  TtaGiveFirstSelectedElement (doc, &first, &c1, &i);
  TtaGiveLastSelectedElement (doc, &last, &i, &cN);

  if (first == NULL)
    return;  /* Error: there's no selected zone */

  /* create the document that will store the annotation */
  if ((docAnnot = ANNOT_NewDocument (doc)) == 0)
    return;

  /* Link the source document to the annotation */
  labf = TtaStrdup (TtaGetElementLabel (first));
  labl = TtaStrdup (TtaGetElementLabel (last));
  LINK_New (doc, docAnnot, labf, c1, labl, cN);
  TtaFreeMemory (labf);
  TtaFreeMemory (labl);

#if 0
  /* @@@ this should be added later on, when saving the annot document */
  /* create the annotation anchor */

  /* Saves the selection data */
  strcpy (tabRefAnnot[docAnnot].labf, TtaGetElementLabel (first));
  tabRefAnnot[docAnnot].c1 = c1;
  strcpy (tabRefAnnot[docAnnot].labl, TtaGetElementLabel (last));
  tabRefAnnot[docAnnot].cN = cN;
  tabRefAnnot[docAnnot].docName = TtaStrdup (TtaGetDocumentName (docAnnot));
#endif
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
  STRING      annotName, fileName;
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
   Procedure ANNOT_SaveProcedure ()
  -----------------------------------------------------------------------
  -----------------------------------------------------------------------*/
#ifdef __STDC__
static void ANNOT_SaveProcedure ()
#else /* __STDC__*/
static void ANNOT_SaveProcedure ()
#endif /* __STDC__*/
{
  printf ("MEUH\n");
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
  STRING   annotName;

  document = AnnotationTargetDocument (docAnnot);
  annotName = TtaGetDocumentName (docAnnot);

  /* Sauvegarde du document d'annotation */
  TtaSaveDocument (docAnnot, annotName);
  TtaSetDocumentUnmodified (docAnnot);

  /* Si le lien d'annotation n'existe pas encore => affichage + notification */
  if (!SearchAnnotation (document, annotName))
  {
    /* Creation d'un nouveau lien d'annotation */
#if 0
    LINK_New (docAnnot);
#endif
#if 0
    printf ("%s\n", TtaGetMessage (AllianceMsgTabId, ALL_AnnotNotif));
    printf ("%s\n", TtaGetMessage (AllianceMsgTabId, ALL_AnnotNotifLabel));
#endif
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

#if 0
    /* Notification aux autres utilisateurs */
    ANNOT_NotifyLocalUsers (document, annotName);
    ANNOT_NotifyToRemoteSites (document, annotName);
#endif
  }
}

