/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2010
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
#include "thot_gui.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "application.h"
#include "document.h"
#include "fileaccess.h"
#include "constpiv.h"
#include "typecorr.h"
#include "appdialogue.h"
#include "zlib.h"
#include "fileaccess.h"

#ifdef NODISPLAY
/*** For ThotKernel, variables FirstSelectedElement and
     LastSelectedElement are defined here, to be used at the end of
     function MergeTextElements  ***/
#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "select_tv.h"
#endif

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "edit_tv.h"
#include "platform_tv.h"
#include "undo_f.h"
#ifndef NODISPLAY
#include "dialogapi.h"
#include "dialogapi_tv.h"
#include "modif_tv.h"
#include "print_tv.h"
#endif
#include "appdialogue_tv.h"

#include "appdialogue_f.h"
#include "applicationapi_f.h"
#include "attributes_f.h"
#include "callback_f.h"
#include "config_f.h"
#include "createabsbox_f.h"
#include "displayview_f.h"
#include "docs_f.h"
#include "documentapi_f.h"
#include "externalref_f.h"
#include "fileaccess_f.h"
#include "memory_f.h"
#include "platform_f.h"
#include "readpivot_f.h"
#include "readstr_f.h"
#include "references_f.h"
#include "schemas_f.h"
#include "structschema_f.h"
#include "structselect_f.h"
#include "thotmsg_f.h"
#include "translation_f.h"
#include "tree_f.h"
#include "undo_f.h"
#include "viewapi_f.h"
#include "views_f.h"
#include "writepivot_f.h"

static char nameBuffer[100];
static char ISObuffer[400];

/*----------------------------------------------------------------------
  TtaGetDocumentProfile
  Gets the document profile
  ----------------------------------------------------------------------*/
int TtaGetDocumentProfile (Document document)
{
  PtrDocument pDoc;

  UserErrorCode = 0;
  /* verifies the parameter document */
  if (document < 1 || document > MAX_DOCUMENTS ||
      LoadedDocument[document - 1] == NULL)
    {
      TtaError (ERR_invalid_document_parameter);
      return (UNDEFINED_CHARSET);
    }
  else
    {
      pDoc = LoadedDocument[document - 1];
      return (pDoc->DocProfile);
    }
}

/*----------------------------------------------------------------------
  TtaGetDocumentExtraProfile
  Gets the document profile
  ----------------------------------------------------------------------*/
int TtaGetDocumentExtraProfile (Document document)
{
  PtrDocument pDoc;

  UserErrorCode = 0;
  /* verifies the parameter document */
  if (document < 1 || document > MAX_DOCUMENTS ||
      LoadedDocument[document - 1] == NULL)
    {
      TtaError (ERR_invalid_document_parameter);
      return (UNDEFINED_CHARSET);
    }
  else
    {
      pDoc = LoadedDocument[document - 1];
      return (pDoc->DocExtraProfile);
    }
}

/*----------------------------------------------------------------------
  TtaSetDocumentProfile
  Sets the document profile
  ----------------------------------------------------------------------*/
void TtaSetDocumentProfile (Document document, int profile,  int extraProfile)
{
  PtrDocument pDoc;

  UserErrorCode = 0;
  /* verifies the parameter document */
  if (document < 1 || document > MAX_DOCUMENTS)
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument[document - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  else
    {
      pDoc = LoadedDocument[document - 1];
      pDoc->DocProfile = profile;
      pDoc->DocExtraProfile = extraProfile;
    }
}


/*----------------------------------------------------------------------
  TtaGetDocumentCharset gets the document charset
  Returns UNDEFINED_CHARSET when the document uses the default charset.
  ----------------------------------------------------------------------*/
CHARSET TtaGetDocumentCharset (Document document)
{
  PtrDocument pDoc;

  UserErrorCode = 0;
  /* verifies the parameter document */
  if (document < 1 || document > MAX_DOCUMENTS || LoadedDocument[document - 1] == NULL)
    {
      TtaError (ERR_invalid_document_parameter);
      return (UNDEFINED_CHARSET);
    }
  else
    {
      pDoc = LoadedDocument[document - 1];
      if (pDoc->DocDefaultCharset)
        return (UNDEFINED_CHARSET);
      else
        return (pDoc->DocCharset);
    }
}


/*------------------------------------------------------------------------------
  TtaSetDocumentCharset sets the document charset
  The boolean 'default' indicates we are setting the default document charset
  -------------------------------------------------------------------------------*/
void TtaSetDocumentCharset (Document document, CHARSET charSet, ThotBool defaultCharset)
{
  PtrDocument pDoc;

  UserErrorCode = 0;
  /* verifies the parameter document */
  if (document < 1 || document > MAX_DOCUMENTS)
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument[document - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  else
    {
      pDoc = LoadedDocument[document - 1];
      pDoc->DocCharset = charSet;
      if (!defaultCharset)
        pDoc->DocDefaultCharset = FALSE;
    }
}

/*----------------------------------------------------------------------
  TtaOpenDocument

  Opens an existing document for subsequent operations.
  Parameters:
  documentName: name of the file containing the document to be open
  (maximum length 19 characters). The directory name is not part of
  this parameter (see TtaSetDocumentPath).
  accessMode: 0 = read only, 1 = read-write.
  Return value:
  the opened document, or 0 if the document cannot be open.
  ----------------------------------------------------------------------*/
Document TtaOpenDocument (char *documentName, int accessMode)
{
  PtrDocument         pDoc;
  Document            document;
  int                 lg;
  ThotBool            ok;

  UserErrorCode = 0;
  document = 0;
  /* initializes the document context */
  CreateDocument (&pDoc, &document);
  if (pDoc == NULL)
    /* too many opened documents */
    TtaError (ERR_too_many_documents);
  else
    {
      lg = strlen (documentName);
      if (lg >= MAX_NAME_LENGTH)
        TtaError (ERR_string_too_long);
      else
        {
          strncpy (pDoc->DocDName, documentName, MAX_NAME_LENGTH);
          pDoc->DocDName[MAX_NAME_LENGTH - 1] = EOS;
          /* suppresses the .PIV suffix if found */
          if (lg > 4)
            if (strcmp (&(pDoc->DocDName[lg - 4]), ".PIV") == 0)
              pDoc->DocDName[lg - 4] = EOS;
          strncpy (pDoc->DocDirectory, DocumentPath, MAX_PATH);
          ok = OpenDocument (pDoc->DocDName, pDoc, TRUE);
          if (!ok)
            /* acces failure to an objectpivot */
            {
              UnloadDocument (&pDoc);
              TtaError (ERR_cannot_open_pivot_file);
            }
          else
            {
              /* keep the actual schema path into the document context */
              strncpy (pDoc->DocSchemasPath, SchemaPath, MAX_PATH);
              if (!pDoc->DocReadOnly)
                pDoc->DocReadOnly = (accessMode == 0);
            }
        }
    }
  return document;
}


/*----------------------------------------------------------------------
  UnloadTree free the document tree of pDoc				
  ----------------------------------------------------------------------*/
void UnloadTree (Document document)
{
  PtrDocument      pDoc;

  pDoc = LoadedDocument[document - 1];
  if (pDoc)
    {
#ifndef NODISPLAY
      /* remove the selection on the document */
      ResetSelection (pDoc);
      if (DocOfSavedElements == pDoc)
        DocOfSavedElements = NULL;
#endif /* NODISPLAY */
       /* free the document tree */
      DeleteAllTrees (pDoc);
    }
}


/*----------------------------------------------------------------------
  UnloadDocument free the document contexts of pDoc				
  ----------------------------------------------------------------------*/
void UnloadDocument (PtrDocument * pDoc)
{
  int                 d;

  if (*pDoc != NULL)
    /* look for the current document in the documents table */
    {
      d = 0;
      while (d < MAX_DOCUMENTS - 1 && LoadedDocument[d] != *pDoc)
        d++;
      if (LoadedDocument[d] == *pDoc)
        {
          /* free document schemas */
          FreeDocumentSchemas (*pDoc);
          FreeDocument (LoadedDocument[d]);
          LoadedDocument[d] = NULL;
#ifndef NODISPLAY
          /* clear the PrintingDoc if it points to this document */
          if (PrintingDoc == d+1)
            PrintingDoc = 0;
#endif /* NODISPLAY */
          /* free the doc type name */
          if ((*pDoc)->DocTypeName)
            TtaFreeMemory((*pDoc)->DocTypeName);
          *pDoc = NULL;
          /* clean up freed contexts */
          FreeAll ();
        }
#ifndef NODISPLAY
      ShowReturn = 0;
#endif /* NODISPLAY */
    }
}


/*----------------------------------------------------------------------
  CloseDocumentAndViews
  Close a document and all its views.
  If notify is TRUE send a notify event.
  ----------------------------------------------------------------------*/
void CloseDocumentAndViews(PtrDocument pDoc, ThotBool notify)
{
  NotifyDialog      notifyDoc;
  Document          doc; 
  if(pDoc)
    {
      doc = (Document) IdentDocument (pDoc);
#ifndef NODISPLAY
      /* Closing all opened views relating to the document */
      /* close the views */
      CloseAllViewsDoc(pDoc);
#endif /* NODISPLAY */

      /* Decrement reference counting.
       * Do not use TtaRemoveDocumentReference here to prevent
       * recursive document closing process.
       */
      pDoc->DocNbRef--;
      if(pDoc->DocNbRef < 0)
        {
#ifndef NODISPLAY
          UnloadTree (doc);
#else /* NODISPLAY */
          DeleteAllTrees (pDoc);
#endif /* NODISPLAY */
          
          if(notify)
            {
              notifyDoc.event = TteDocClose;
              notifyDoc.document = doc;
              notifyDoc.view = 0;
              CallEventType ((NotifyEvent *) & notifyDoc, FALSE);          
            }
          UnloadDocument (&pDoc);
        }
    }
}


/*----------------------------------------------------------------------
  TtaCloseDocument

  Closes a document that is no longer needed and releases all ressources
  allocated to the document. This function does not save the document.
  Parameter:
  document: the document to be closed.
  ----------------------------------------------------------------------*/
void TtaCloseDocument (Document document)
{
  PtrDocument      pDoc;

  UserErrorCode = 0;
  /* verifies the parameter document */
  if (document < 1 || document > MAX_DOCUMENTS)
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument[document - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  else
    /* parameter document is correct */
    {
      pDoc = LoadedDocument[document - 1];
      if (ThotLocalActions[T_clearhistory] != NULL)
        (*(Proc1)ThotLocalActions[T_clearhistory]) (pDoc);

      CloseDocumentAndViews(pDoc, FALSE);
    }
}


/*----------------------------------------------------------------------
  TtaSetDocumentPath

  Sets a new list of document directories. This list replaces the existing one.
  It is used for searching a document when it is open either by the user or
  by the application program (see TtaOpenDocument).
  The first directory in the list is used when a new document is created
  (see TtaNewDocument).
  Parameter:
  path: the directory list, where directory names are separated by
  the character PATH_SEP.
  ----------------------------------------------------------------------*/
void TtaSetDocumentPath (char *path)
{
  UserErrorCode = 0;
  if (strlen (path) >= MAX_PATH)
    TtaError (ERR_string_too_long);
  else
    strcpy (DocumentPath, path);
}

/*----------------------------------------------------------------------
  TtaCheckPath

  Checks if all directories in a path can be accessed.
  Parameter:
  path: the path to be checked
  Return value:
  TRUE if all directories are OK, FALSE if at least one cannot be
  accessed.
  ----------------------------------------------------------------------*/
ThotBool TtaCheckPath (char *path)
{
  int                 i, j;
  PathBuffer          single_directory;
  ThotBool            OK;

  i = 0;
  OK = TRUE;
  while (OK && path[i] != EOS)
    {
      j = 0;
      while (path[i] != PATH_SEP && path[i] != EOS && i <= MAX_PATH)
        {
          /* The list is cutted up into single directories */
          single_directory[j] = path[i];
          i++;
          j++;
        }
      single_directory[j] = EOS;

      OK = TtaCheckDirectory (single_directory);
      /* We try with another directory by ignoring PATH_SEP */
      if (path[i] == PATH_SEP)
        i++;
    }
  return (OK);
}


/*----------------------------------------------------------------------
  TtaIsInDocumentPath

  returns TRUE if the directory is in the list of document directories.
  Parameter:
  directory: the new directory name.
  ----------------------------------------------------------------------*/
ThotBool TtaIsInDocumentPath (char *directory)
{
  int              i;
  char            *ptr;

  /* Verify if this directory is already in the list  */
  ptr = strstr (DocumentPath, directory);
  i = strlen (directory);
  while (ptr != NULL && ptr[i] != PATH_SEP && ptr[i] != EOS)
    {
      ptr = strstr (ptr, PATH_STR);
      if (ptr != NULL)
        ptr = strstr (ptr, directory);
    }
  return (ptr != NULL);
}


/*----------------------------------------------------------------------
  TtaAppendDocumentPath

  Appends a new directory in the list of document directories if this
  directory is not already in the list and if the directory exists.
  Parameter:
  directory: the new directory name.
  ----------------------------------------------------------------------*/
void TtaAppendDocumentPath (char *directory)
{
  int                 i;
  int                 lg;

  UserErrorCode = 0;
  lg = strlen (directory);

  if (lg >= MAX_PATH)
    TtaError (ERR_string_too_long);
  else if (!TtaCheckDirectory (directory))
    TtaError (ERR_invalid_parameter);
  else if (!TtaIsInDocumentPath (directory))
    {
      /* add the directory in the path */
      i = strlen (DocumentPath);
      if (i + lg + 2 >= MAX_PATH)
        TtaError (ERR_string_too_long);
      else
        {
          if (i > 0)
            strcat (DocumentPath, PATH_STR);
          strcat (DocumentPath, directory);
        }
    }
}

/*----------------------------------------------------------------------
  TtaSetSchemaPath

  Sets a new list of schema directories. This list replaces the existing one.
  It is used for searching schemas.
  Parameter:
  path: the directory list, where directory names are separated by
  the character PATH_SEP.
  ----------------------------------------------------------------------*/
void TtaSetSchemaPath (char *path)
{
  UserErrorCode = 0;
  if (strlen (path) >= MAX_PATH)
    TtaError (ERR_string_too_long);
  else
    strcpy (SchemaPath, path);
}

/*----------------------------------------------------------------------
  TtaNewNature

  Adds a new nature in a structure schema and returns the structure schema
  of the new nature. If the nature already exists in that structure schema,
  the function simply returns the structure schema of that nature.
  Parameters:
  document: the document of interest
  schema: the structure schema to which the nature is added.
  natureName: name of the nature to be added in the structure schema.
  presentationName: name of the presentation schema to be associated with
  the extension schema. If presentationName is an empty string, the
  default presentation schema is associated. If the nature already
  exists, presentationName is ignored.
  Return value:
  the structure schema of the new nature; NULL if the structure schema
  has not been loaded.
  ----------------------------------------------------------------------*/
SSchema TtaNewNature (Document document, SSchema schema, const char *natureURI,
                      const char *natureName, const char *presentationName)
{
  int                 natureRule;
  PtrSSchema          natureSchema;

  UserErrorCode = 0;
  natureSchema = NULL;
  if (document < 0 || document > MAX_DOCUMENTS)
    TtaError (ERR_invalid_document_parameter);
  else if (document > 0 && LoadedDocument[document - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  else if (schema == NULL || natureName[0] == EOS)
    TtaError (ERR_invalid_parameter);
  else
    {
      natureRule = CreateNature (natureURI, natureName, presentationName,
                                 (PtrSSchema) schema,
                                 LoadedDocument[document - 1]);
      if (natureRule == 0)
        TtaError (ERR_invalid_parameter);
      else
        {
          natureSchema = ((PtrSSchema) schema)->SsRule->SrElem[natureRule - 1]->SrSSchemaNat;
#ifndef NODISPLAY
          if (document > 0)
            AddSchemaGuestViews (LoadedDocument[document - 1], natureSchema);
#endif
        }
    }
  return ((SSchema) natureSchema);
}


/*----------------------------------------------------------------------
  TtaNewSchemaExtension

  Loads a structure schema extension and associates it with
  a given document.

  document: the document whose structure schema must be extended.
  extensionName: name of the extension schema.
  presentationName: name of the presentation schema to be associated with
  the extension schema. If presentationName is an empty string, the
  default presentation schema is associated.
  Return value:
  the extension schema, NULL if the extension schema has not been loaded.
  ----------------------------------------------------------------------*/
SSchema TtaNewSchemaExtension (Document document, char *extensionName,
                               char *presentationName)
{
  PtrSSchema          extension;

  UserErrorCode = 0;
  /* verifies the parameter document */
  extension = NULL;
  if (document < 1 || document > MAX_DOCUMENTS)
    {
      TtaError (ERR_invalid_document_parameter);
    }
  else if (LoadedDocument[document - 1] == NULL)
    {
      TtaError (ERR_invalid_document_parameter);
    }
  else
    /* parameter document is correct */
    {
      extension = LoadExtension (extensionName, presentationName, LoadedDocument[document - 1]);
      if (extension == NULL)
        {
          TtaError (ERR_cannot_read_struct_schema);
        }
    }
  return ((SSchema) extension);
}


/*----------------------------------------------------------------------
  TtaSetPSchema

  Sets or changes the main presentation schema of a document. The document
  must be open, but no view must be open for that document.
  Parameters:
  document: the document.
  presentationName: Name of the presentation schema to be associated
  with the document.
  ----------------------------------------------------------------------*/
void TtaSetPSchema (Document document, const char *presentationName)
{
  PtrDocument         pDoc;
#ifndef NODISPLAY
  int                 view;
  ThotBool            ok;
#endif

  UserErrorCode = 0;
  /* verifies the parameter document */
  if (document < 1 || document > MAX_DOCUMENTS)
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument[document - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  else
    /* parameter document is correct */
    {
      pDoc = LoadedDocument[document - 1];
#ifdef NODISPLAY
      if (pDoc->DocSSchema != NULL)
        {
          if (pDoc->DocSSchema->SsDefaultPSchema)
            TtaFreeMemory (pDoc->DocSSchema->SsDefaultPSchema);
          pDoc->DocSSchema->SsDefaultPSchema = TtaStrdup (presentationName);
        }
#else
      /* verifies that there is no open views */
      ok = TRUE;
      for (view = 0; view < MAX_VIEW_DOC && ok; view++)
        if (pDoc->DocView[view].DvPSchemaView != 0)
          ok = FALSE;
      if (!ok)
        TtaError (ERR_there_are_open_views);
      else
        /* There is no opened views */
        /* Load the presentation schema */
        LoadPresentationSchema (presentationName, pDoc->DocSSchema, pDoc);
#endif
    }
}


/*----------------------------------------------------------------------
  TtaSetDocumentDirectory

  Sets the directory to which the document is supposed to be saved.
  Parameters:
  document: the document whose directory is set.
  directory: new document directory.
  ----------------------------------------------------------------------*/
void TtaSetDocumentDirectory (Document document, char *directory)
{
  UserErrorCode = 0;
  /* verifies the parameter document */
  if (document < 1 || document > MAX_DOCUMENTS)
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument[document - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  else
    /* parameter document is correct */
    {
      if (strlen (directory) >= MAX_PATH)
        TtaError (ERR_buffer_too_small);
      strcpy (LoadedDocument[document - 1]->DocDirectory, directory);
    }
}


/*----------------------------------------------------------------------
  TtaSetDocumentAccessMode

  Sets the access mode for a document.
  Parameters:
  document: the document whose access mode is changed.
  accessMode: 0 = read only, 1 = read-write.
  ----------------------------------------------------------------------*/
void TtaSetDocumentAccessMode (Document document, int accessMode)
{
  PtrDocument      pDoc;

  UserErrorCode = 0;
  /* verifies the parameter document */
  if (document < 1 || document > MAX_DOCUMENTS)
    TtaError (ERR_invalid_document_parameter);
  else
    {
      pDoc = LoadedDocument[document - 1];
      if (pDoc == NULL)
        TtaError (ERR_invalid_document_parameter);
      else
        /* parameter document is correct */
        {
          if (accessMode == 0)
            {
              pDoc->DocReadOnly = TRUE;
              //if (pDoc->DocDocElement != NULL)
              //  pDoc->DocDocElement->ElAccess = ReadOnly;
            }
          else
            {
              pDoc->DocReadOnly = FALSE;
              //if (pDoc->DocDocElement != NULL)
              //  pDoc->DocDocElement->ElAccess = ReadWrite;
            }
#ifndef NODISPLAY
          /* update the paste entry */
          if (accessMode == 0)
            /* disable the Paste command */
            SwitchPaste (pDoc, FALSE);
#ifdef _WINGUI
          else
            /* enable the Paste command */
            SwitchPaste (pDoc, TRUE);
#else /* _WINGUI */
          else if (FirstSavedElement != NULL || ClipboardThot.BuLength != 0)
            /* enable the Paste command */
            SwitchPaste (pDoc, TRUE);
#endif /* _WINGUI */
          SetAccessMode (pDoc, accessMode);
#endif
        }
    }
}


/*----------------------------------------------------------------------
  TtaSetDocumentBackUpInterval

  Sets the backup interval of documents.
  Parameter:
  interval: 0 if the backup mechanism is disabled, or gives the number
  of characters which triggers an automatic save into a .BAK file.
  ----------------------------------------------------------------------*/
void TtaSetDocumentBackUpInterval (int interval)
{
  UserErrorCode = 0;
  DocBackUpInterval = interval;
}

/*----------------------------------------------------------------------
  TtaSetNotificationMode

  Sets the ECF notification mode for a document.
  Parameters:
  document: the document whose notification mode is changed.
  notificationMode: 0 = only roots of created and deleted subtrees must
  be notified, 1 = all elements of created and deleted subtrees must
  be notified.
  ----------------------------------------------------------------------*/
void  TtaSetNotificationMode (Document document, int notificationMode)
{
  UserErrorCode = 0;
  /* verifies the parameter document */
  if (document < 1 || document > MAX_DOCUMENTS)
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument[document - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  else
    /* parameter document is correct */
    LoadedDocument[document - 1]->DocNotifyAll = (notificationMode != 0);
}

/*----------------------------------------------------------------------
  SetDocumentModified sets the document flags DocUpdated and DocModified 
  to the status value.
  The parameter length gives the number of characters added.
  If the previous status of DocUpdated was FALSE the function notifies
  the application.
  ----------------------------------------------------------------------*/
void  SetDocumentModified (PtrDocument pDoc, ThotBool status, int length)
{
  if (pDoc != NULL)
    {
      if (status)
        {
          /* document modified */
          if ((!pDoc->DocUpdated || !pDoc->DocModified) &&
              ThotLocalActions[T_docmodified])
            (*(Proc2)ThotLocalActions[T_docmodified]) ((void *)IdentDocument (pDoc), (void *)TRUE);
          pDoc->DocModified = TRUE;
          pDoc->DocUpdated = TRUE;
        }
      else
        {
          /* document unmodified */
          if ((pDoc->DocUpdated || pDoc->DocModified) &&
              ThotLocalActions[T_docmodified])
            (*(Proc2)ThotLocalActions[T_docmodified]) ((void *)IdentDocument (pDoc), (void *)FALSE);
          pDoc->DocModified = FALSE;
          pDoc->DocUpdated = FALSE;
        }
    }
}


/*----------------------------------------------------------------------
  TtaSetDocumentModified

  Notifies the tool kit that a document has been modified by the application.
  As a consequence, the user will be asked to save the document when closing it.
  Parameter:
  document: the document.
  ----------------------------------------------------------------------*/
void TtaSetDocumentModified (Document document)
{
  UserErrorCode = 0;
  /* verifies the parameter document */
  if (document < 1 || document > MAX_DOCUMENTS)
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument[document - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  else
    /* parameter document is correct */
    SetDocumentModified (LoadedDocument[document - 1], TRUE, 10);
}

/*----------------------------------------------------------------------
  TtaSetDocumentUnmodified

  Notifies the tool kit that a document must be considered as not modified
  by the application or by the user. As a consequence, if no further modification
  is made to that document, the user will not be asked to save the document
  when closing it.
  Parameter:
  document: the document.
  ----------------------------------------------------------------------*/
void TtaSetDocumentUnmodified (Document document)
{
  UserErrorCode = 0;
  /* verifies the parameter document */
  if (document < 1 || document > MAX_DOCUMENTS)
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument[document - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  else
    /* parameter document is correct */
    SetDocumentModified (LoadedDocument[document - 1], FALSE, 0);
}

/*----------------------------------------------------------------------
  TtaSetDocumentUpdated

  Notifies the tool kit that a document must be considered as updated
  by the application or by the user. That will allow the application to
  detect if any change will be made on the document
  (see TtaIsDocumentUpdated).
  Parameter:
  document: the document.
  ----------------------------------------------------------------------*/
void TtaSetDocumentUpdated (Document document)
{
  UserErrorCode = 0;
  /* verifies the parameter document */
  if (document < 1 || document > MAX_DOCUMENTS)
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument[document - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  else
    /* parameter document is correct */
    LoadedDocument[document - 1]->DocUpdated = TRUE;
}

/*----------------------------------------------------------------------
  TtaSetDocumentUnupdated

  Notifies the tool kit that a document must be considered as not updated
  by the application or by the user. That will allow the application to
  detect if any change will be made on the document
  (see TtaIsDocumentUpdated).
  Parameter:
  document: the document.
  ----------------------------------------------------------------------*/
void TtaSetDocumentUnupdated (Document document)
{
  UserErrorCode = 0;
  /* verifies the parameter document */
  if (document < 1 || document > MAX_DOCUMENTS)
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument[document - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  else
    /* parameter document is correct */
    LoadedDocument[document - 1]->DocUpdated = FALSE;
}

/*----------------------------------------------------------------------
  TtaGetDocumentName

  Returns the name of a document.
  Parameter:
  document: the document whose name is asked.
  Return value:
  name of that document.
  ----------------------------------------------------------------------*/
char *TtaGetDocumentName (Document document)
{
  UserErrorCode = 0;
  nameBuffer[0] = EOS;
  /* verifies the parameter document */
  if (document < 1 || document > MAX_DOCUMENTS)
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument[document - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  else
    /* parameter document is correct */
    strcpy (nameBuffer, LoadedDocument[document - 1]->DocDName);
  return nameBuffer;
}


/*----------------------------------------------------------------------
  TtaGetDocumentFromName

  Returns the document having a given name.
  Parameter:
  documentName: the document name.
  Return value:
  the document having that name.
  ----------------------------------------------------------------------*/
Document TtaGetDocumentFromName (char *documentName)
{
  int                 document;
  ThotBool            found;

  UserErrorCode = 0;
  document = 1;
  found = FALSE;
  while (!found && document < MAX_DOCUMENTS)
    {
      if (LoadedDocument[document - 1] != NULL &&
          (strcmp (documentName, LoadedDocument[document - 1]->DocDName) == 0))
        found = TRUE;
      else
        document++;
    }
  if (!found)
    document = 0;
  return (Document) document;
}


/*----------------------------------------------------------------------
  TtaGetDocumentDirectory

  Returns the directory to which the document is supposed to be saved.
  Parameters:
  document: the document whose directory is asked.
  buffer: a buffer provided by the caller.
  bufferLength: the length of that buffer.
  Return parameter:
  buffer: the document directory.
  ----------------------------------------------------------------------*/
void TtaGetDocumentDirectory (Document document, char *buffer, int bufferLength)
{
  UserErrorCode = 0;
  nameBuffer[0] = EOS;
  /* verifies the parameter document */
  if (document < 1 || document > MAX_DOCUMENTS)
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument[document - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  else
    /* parameter document is correct */
    {
      if (strlen (LoadedDocument[document - 1]->DocDirectory) >= (size_t) bufferLength)
        TtaError (ERR_buffer_too_small);
      strncpy (buffer, LoadedDocument[document - 1]->DocDirectory, bufferLength - 1);
    }
}

/*----------------------------------------------------------------------
  TtaGetDocumentSSchema

  Returns the main structure schema of a document.
  Parameter:
  document: the document for which the structure schema is asked.
  Return value:
  the structure schema of that document.
  ----------------------------------------------------------------------*/
SSchema  TtaGetDocumentSSchema (Document document)
{
  SSchema             schema;

  UserErrorCode = 0;
  /* verifies the parameter document */
  schema = NULL;
  if (document < 1 || document > MAX_DOCUMENTS)
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument[document - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  else
    /* parameter document is correct */
    schema = (SSchema) LoadedDocument[document - 1]->DocSSchema;
  return schema;
}

/*----------------------------------------------------------------------
  TtaGetSSchemaName

  Returns the name of a structure schema.
  Parameter:
  schema: the structure schema of interest.
  Return value:
  name of that structure schema.
  ----------------------------------------------------------------------*/
char *TtaGetSSchemaName (SSchema schema)
{
  UserErrorCode = 0;
  if (schema == NULL || ((PtrSSchema) schema)->SsName == NULL)
    {
      ISObuffer[0] = EOS;
      TtaError (ERR_invalid_parameter);
    }
  else
    strncpy (ISObuffer, ((PtrSSchema) schema)->SsName, 400);
  return ISObuffer;
}

/*----------------------------------------------------------------------
  TtaGetPSchemaName

  Returns the name of the presentation schema currently associated
  with a given structure schema.
  Parameter:
  schema: the structure schema of interest.
  Return value:
  name of the associated presentation schema.
  ----------------------------------------------------------------------*/
char *TtaGetPSchemaName (SSchema schema)
{
  UserErrorCode = 0;
  if (schema == NULL || ((PtrSSchema) schema)->SsName == NULL)
    {
      ISObuffer[0] = EOS;
      TtaError (ERR_invalid_parameter);
    }
  else
    strncpy (ISObuffer, ((PtrSSchema) schema)->SsDefaultPSchema, 400);
  return ISObuffer;
}

/*----------------------------------------------------------------------
  TtaGetSSchema

  Returns a structure schema whose name is known and that is used in a
  given document.
  Parameter:
  name: the name of the structure schema of interest.
  document: the document that uses this structure schema.
  Return value:
  the structure schema having this name, or NULL if this structure
  schema is not loaded or not used by the document.
  ----------------------------------------------------------------------*/
SSchema TtaGetSSchema (const char *name, Document document)
{
  SSchema          schema;

  UserErrorCode = 0;
  schema = NULL;
  if (name == NULL || name[0] == EOS)
    TtaError (ERR_invalid_parameter);
  /* verifies the parameter document */
  else if (document < 1 || document > MAX_DOCUMENTS)
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument[document - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  else
    /* parameter document is correct */
    schema = (SSchema) GetSSchemaForDoc (name, LoadedDocument[document - 1]);
  return schema;
}

/*----------------------------------------------------------------------
  TtaGetSSchemaByUri

  Returns a structure schema whose URI is known and that is used in a
  given document.
  Parameter:
  uriName: the URI of the structure schema of interest.
  document: the document that uses this structure schema.
  Return value:
  the structure schema having this URI, or NULL if this structure
  schema is not loaded or not used by the document.
  ----------------------------------------------------------------------*/
SSchema TtaGetSSchemaByUri (const char *uriName, Document document)
{
  SSchema          schema;

  UserErrorCode = 0;
  schema = NULL;
  if (uriName == NULL || uriName[0] == EOS)
    TtaError (ERR_invalid_parameter);
  else if (document < 1 || document > MAX_DOCUMENTS)
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument[document - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  else
    /* parameter document is correct */
    schema = (SSchema) GetSSchemaByUriForDoc (uriName, LoadedDocument[document - 1]);
  return schema;
}

/*----------------------------------------------------------------------
  TtaSameSSchemas

  Compares two structure schemas.
  Parameters:
  schema1: first structure schema.
  schema2: second structure schema.
  Return value:
  FALSE if schemas are different, TRUE if they are identical.
  ----------------------------------------------------------------------*/
ThotBool TtaSameSSchemas (SSchema schema1, SSchema schema2)
{
  ThotBool result;

  UserErrorCode = 0;
  result = FALSE;
  if (schema1 == NULL || schema2 == NULL)
    TtaError (ERR_invalid_parameter);
  else if (!strcmp (((PtrSSchema) schema1)->SsName, ((PtrSSchema) schema2)->SsName))
    result = TRUE;
  return result;
}

/*----------------------------------------------------------------------
  TtaGiveSchemasOfDocument

  Returns the names of the main structure schema and presentation schema
  associated with a given document. The document does not need to be open
  and the schemas are not loaded by this function.
  Parameters:
  documentName: Name of the document to be checked (maximum length
  19 characters).
  structureName: buffer.
  presentationName: buffer.
  Return value:
  structureName: Name of the document structure schema.
  presentationName: Name of the document presentation schema.
  ----------------------------------------------------------------------*/
void TtaGiveSchemasOfDocument (char *documentName, char *structureName,
                               char *presentationName)
{
  PathBuffer          DirBuffer;
  BinFile             file;
  char              text[MAX_TXT_LEN];
  int                 i;
  ThotBool            error;
  char                charGotten;
  LabelString         lab;
  int                 currentVersion = 0;

  UserErrorCode = 0;
  structureName[0] = EOS;
  presentationName[0] = EOS;
  /* Arrange the name of the file to be opened with the documents directory name */
  strncpy (DirBuffer, DocumentPath, MAX_PATH);
  MakeCompleteName (documentName, "PIV", DirBuffer, text, &i);
  /* Verify if the file exists */
  file = TtaReadOpen (text);
  if (file == 0)
    /* document file inaccessible */
    TtaError (ERR_cannot_open_pivot_file);
  else
    /* Read the begenning of the document file */
    {
      error = FALSE;
      /* Gets the version number if it exists */
      if (!TtaReadByte (file, (unsigned char *)&charGotten))
        error = TRUE;
      if (charGotten == (char) C_PIV_VERSION)
        {
          if (!TtaReadByte (file, (unsigned char *)&charGotten))
            error = TRUE;
          if (!TtaReadByte (file, (unsigned char *)&charGotten))
            error = TRUE;
          else
            currentVersion = (int) charGotten;
          if (!TtaReadByte (file, (unsigned char *)&charGotten))
            error = TRUE;
        }
      /* Gets the label max. of the document if it is present */
      if (!error &&
          ((char) charGotten == (char) C_PIV_SHORT_LABEL ||
           (char) charGotten == (char) C_PIV_LONG_LABEL ||
           charGotten == (char) C_PIV_LABEL))
        {
          ReadLabel (charGotten, lab, file);
          if (!TtaReadByte (file, (unsigned char *)&charGotten))
            error = TRUE;
        }

      if (currentVersion >= 4)
        {
          /* Gets the table of laguages used by the document */
          while (charGotten == (char) C_PIV_LANG && !error)
            {
              do
                if (!TtaReadByte (file, (unsigned char *)&charGotten))
                  error = TRUE;
              while (!(error || charGotten == EOS)) ;
              if (charGotten != EOS)
                error = TRUE;
              else
                /* Gets the byte following the language name */
                if (!TtaReadByte (file, (unsigned char *)&charGotten))
                  error = TRUE;
            }
        }

      /* Gets the name of the schema structure which is at the begenning of the pivot file */
      if (!error && charGotten != (char) C_PIV_NATURE)
        error = TRUE;
      if (!error)
        {
          i = 0;
          do
            if (!TtaReadByte (file, (unsigned char *)&structureName[i++]))
              error = TRUE;
          while (!(error || structureName[i - 1] == EOS || i == MAX_NAME_LENGTH)) ;
          if (structureName[i - 1] != EOS)
            error = TRUE;
          else
            {
              if (currentVersion >= 4)
                /* Gets the code of the structure schema */
                if (!TtaReadShort (file, &i))
                  error = TRUE;
              if (!error)
                {
                  /* Gets the name of the associated presentation schema */
                  i = 0;
                  do
                    if (!TtaReadByte (file, (unsigned char *)&presentationName[i++]))
                      error = TRUE;
                  while (!(error || presentationName[i - 1] == EOS || i == MAX_NAME_LENGTH)) ;
                }
            }
        }
      TtaReadClose (file);
    }
}

/*----------------------------------------------------------------------
  TtaNextSchemaExtension

  Returns a structure schema extension associated with a given document.
  Parameters:
  document: the document of interest.
  extension: a schema extension of that document. NULL for accessing
  the first schema extension.
  Return parameter:
  extension: the schema extension that follows or the first schema
  extension of the document if parameter extension is NULL.
  NULL if there is no more schema extension.
  ----------------------------------------------------------------------*/
void   TtaNextSchemaExtension (Document document, SSchema * extension)
{
  PtrSSchema          nextExtension;

  UserErrorCode = 0;
  /* verifies the parameter document */
  nextExtension = NULL;
  if (document < 1 || document > MAX_DOCUMENTS)
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument[document - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  /* parameter document is correct */
  else if (*extension == NULL)
    nextExtension = LoadedDocument[document - 1]->DocSSchema->SsNextExtens;
  else if (!(((PtrSSchema) (*extension))->SsExtension))
    /* It is not the extension schema */
    TtaError (ERR_invalid_parameter);
  else
    nextExtension = ((PtrSSchema) (*extension))->SsNextExtens;
  *extension = (SSchema) nextExtension;
}

/*----------------------------------------------------------------------
  TtaNextNature

  Returns the structure schema of a nature used in a given document.
  Parameters:
  document: the document of interest.
  nature: the structure schema of a nature for that document. NULL for
  accessing the first nature.
  Return parameter:
  nature: the structure schema of the next nature, or the structure schema
  of the first nature of the document if parameter nature was NULL
  when calling.
  NULL if there is no more nature for the document.
  ----------------------------------------------------------------------*/
void  TtaNextNature (Document document, SSchema * nature)
{
  PtrSSchema          nextNature;
#ifndef NODISPLAY
  int                 n;
  PtrDocument         pDoc;
  ThotBool            found;
#endif

  UserErrorCode = 0;
  /* verifies the parameter document */
  nextNature = NULL;
  if (document < 1 || document > MAX_DOCUMENTS)
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument[document - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  /* parameter document is correct */
#ifndef NODISPLAY
  else
    {
      pDoc = LoadedDocument[document - 1];
      if (*nature == NULL)
        /* First apply, build the table of the natures of the document */
        {
          BuildDocNatureTable (pDoc);
          if (pDoc->DocNNatures > 1)
            /* The first entry of the table will be returned */
            n = 1;
          else
            /* table of natures is empty */
            n = 0;
        }
      else
        /* The table is already builded. One looks for the current entry
           of the table */
        {
          n = 1;
          found = FALSE;
          while ((n < pDoc->DocNNatures) && !found)
            {
              if (pDoc->DocNatureSSchema[n] == (PtrSSchema) (*nature))
                found = TRUE;
              n++;
            }
          if (!found)
            n = 0;
        }

      if (n > 0)
        {
          /* Extension schemas are ignored */
          found = FALSE;
          while ((n < pDoc->DocNNatures) && !found)
            if (!pDoc->DocNatureSSchema[n]->SsExtension)
              found = TRUE;
            else
              n++;
          if (found)
            nextNature = pDoc->DocNatureSSchema[n];
        }
    }
#endif
  *nature = (SSchema) nextNature;
}

/*----------------------------------------------------------------------
  TtaIsDocumentModified

  Indicates whether a document has been modified by the user or not.
  Modifications made by the application program are not considered,
  except when explicitely notified by TtaDocumentModified.
  Parameter:
  document: the document.
  Return value:
  1 if the document has been modified by the user since it has been saved,
  loaded or created, 0 if it has not been modified.
  ----------------------------------------------------------------------*/
ThotBool TtaIsDocumentModified (Document document)
{
  ThotBool modified;

  UserErrorCode = 0;
  /* verifies the parameter document */
  modified = FALSE;
  if (document < 1 || document > MAX_DOCUMENTS)
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument[document - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument[document - 1]->DocModified)
    /* parameter document is correct */
    modified = TRUE;
  return modified;
}

/*----------------------------------------------------------------------
  TtaIsDocumentUpdated

  Indicates whether a document has been modified by the user or not
  since the last TtaSetDocumentUnupdated or TtaSetDocumentUnmodified.
  Modifications made by the application program are not considered,
  except when explicitely notified by TtaDocumentModified.
  Parameter:
  document: the document.
  Return value:
  1 if the document has been modified by the user since it has been saved,
  loaded or created, 0 if it has not been modified.
  ----------------------------------------------------------------------*/
ThotBool TtaIsDocumentUpdated (Document document)
{
  ThotBool updated;

  UserErrorCode = 0;
  /* verifies the parameter document */
  updated = FALSE;
  if (document < 1 || document > MAX_DOCUMENTS)
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument[document - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument[document - 1]->DocUpdated)
    /* parameter document is correct */
    updated = TRUE;
  return updated;
}

/*----------------------------------------------------------------------
  TtaGetDocumentAccessMode

  Returns the access mode for a document.
  Parameter:
  document: the document whose access mode is asked.
  Return value:
  0 if access mode is read only, 1 if access mode is read-write.
  ----------------------------------------------------------------------*/
int TtaGetDocumentAccessMode (Document document)
{
  int                 result;

  UserErrorCode = 0;
  result = 1;
  /* verifies the parameter document */
  if (document < 1 || document > MAX_DOCUMENTS)
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument[document - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument[document - 1]->DocReadOnly)
    result = 0;
  else
    result = 1;
  return result;
}


/*----------------------------------------------------------------------
  TtaGetDocumentBackUpInterval

  Returns backup interval of documents.
  Return value:
  0 if the backup mechanism is disabled or the number of characters
  which trigger an automatic save into a .BAK file.
  ----------------------------------------------------------------------*/
int TtaGetDocumentBackUpInterval ()
{
  UserErrorCode = 0;
  return DocBackUpInterval;
}


/*----------------------------------------------------------------------
  TtaGetNotificationMode

  Returns the ECF notification mode for a document.
  Parameters:
  document: the document whose notification mode is asked.
  Return value:
  0 = if only roots of created and deleted subtrees must be notified,
  1 = all elements of created and deleted subtrees must be notified.
  ----------------------------------------------------------------------*/
int TtaGetNotificationMode (Document document)
{
  int                 result;

  UserErrorCode = 0;
  result = 0;
  /* verifies the parameter document */
  if (document < 1 || document > MAX_DOCUMENTS)
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument[document - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument[document - 1]->DocNotifyAll)
    /* parameter document is correct */
    result = 1;
  else
    result = 0;
  return result;
}


/*----------------------------------------------------------------------
  TtaGetDocumentPath

  Returns the current list of the directories used when a document is open
  (see TtaOpenDocument).
  Parameters:
  buffer: a buffer provided by the caller.
  bufferLength: the length of that buffer.
  Return parameter:
  buffer: the list of directories. Directory names are separated by
  the character PATH_SEP.
  ----------------------------------------------------------------------*/
void TtaGetDocumentPath (char *buffer, int bufferLength)
{

  UserErrorCode = 0;
  if (strlen (DocumentPath) >= (size_t)bufferLength)
    TtaError (ERR_buffer_too_small);
  strncpy (buffer, DocumentPath, bufferLength - 1);
}

/*----------------------------------------------------------------------
  TtaGetSchemaPath

  Returns the current list of directories used for accessing schemas.
  Parameters:
  buffer: a buffer provided by the caller.
  bufferLength: the length of that buffer.
  Return parameter:
  buffer: the list of directories. Directory names are separated by
  the character PATH_SEP.
  ----------------------------------------------------------------------*/
void TtaGetSchemaPath (char *buffer, int bufferLength)
{

  UserErrorCode = 0;
  if (strlen (SchemaPath) >= (size_t)bufferLength)
    TtaError (ERR_buffer_too_small);
  strncpy (buffer, SchemaPath, bufferLength - 1);
}

#ifndef NODISPLAY
/*----------------------------------------------------------------------
  TtaGetDocumentOfSavedElements

  Returns the document for which the last Copy or Cut command has been
  issued.
  Parameters:
  no parameter.
  Return value:
  the document for which the last Copy or Cut command has been issued.
  0 if the clipboard is empty.
  ----------------------------------------------------------------------*/
Document TtaGetDocumentOfSavedElements ()
{
  UserErrorCode = 0;
  if (DocOfSavedElements == NULL)
    return 0;
  else
    return IdentDocument (DocOfSavedElements);
}
#endif

/*----------------------------------------------------------------------
  DocToPtr returns the PtrDocument corresponding to a given document
  ----------------------------------------------------------------------*/
PtrDocument DocToPtr (Document document)
{
  return LoadedDocument[document - 1];
}

/*----------------------------------------------------------------------
  TtaSetNamespaceDeclaration
  Set a namespace declaration for an element 
  ----------------------------------------------------------------------*/
void TtaSetANamespaceDeclaration (Document document, Element element,
                                  const char *nsPrefix, const char *nsUri)
{
  PtrDocument pDoc;

  UserErrorCode = 0;
  /* verifies the parameter document */
  if (document < 1 || document > MAX_DOCUMENTS)
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument[document - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  else
    {
      /* parameter document is correct */
      pDoc = LoadedDocument[document - 1];
      SetNamespaceDeclaration (pDoc, (PtrElement) element, nsPrefix, nsUri);
    }
}

/*----------------------------------------------------------------------
  TtaRemoveANamespaceDeclaration
  Remove a namespace declaration for an element 
  ----------------------------------------------------------------------*/
void TtaRemoveANamespaceDeclaration (Document document, Element element,
                                      const char *nsPrefix, const char *nsUri)
{
  PtrDocument pDoc;

  UserErrorCode = 0;
  /* verifies the parameter document */
  if (document < 1 || document > MAX_DOCUMENTS)
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument[document - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  else
    {
      /* parameter document is correct */
      pDoc = LoadedDocument[document - 1];
      RemoveANamespaceDeclaration (pDoc, (PtrElement) element, nsPrefix, nsUri);
    }
}

/*----------------------------------------------------------------------
  TtaGiveNamespaceURI
  Returns the pointer to the current registerd namesapce URI or NULL
  ----------------------------------------------------------------------*/
char *TtaGiveNamespaceDeclaration (Document document, Element element)
{
  UserErrorCode = 0;
  /* verifies the parameter document */
  if (document < 1 || document > MAX_DOCUMENTS)
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument[document - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  else
    return GiveCurrentNsUri (LoadedDocument[document - 1], (PtrElement)element);
  return NULL;
}

/*----------------------------------------------------------------------
  TtaGiveElemNamespaceDeclarations
  Give all namespace declarations and prefixes defined for a element           
  ----------------------------------------------------------------------*/
void TtaGiveElemNamespaceDeclarations (Document document, Element element,
				       char **declarations, char **prefixes,
				       int max)
{
  UserErrorCode = 0;
  /* verifies the parameter document */
  if (document < 1 || document > MAX_DOCUMENTS)
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument[document - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  else
    GiveElemNamespaceDeclarations (LoadedDocument[document - 1],
				   (PtrElement)element, declarations, prefixes,
				   max);
}

/*----------------------------------------------------------------------
  TtaDocumentUsesNsPrefixes
  Check wether the document uses namespaces with prefix
  ----------------------------------------------------------------------*/
ThotBool TtaDocumentUsesNsPrefixes (Document document)
{
  UserErrorCode = 0;
  /* verifies the parameter document */
  if (document < 1 || document > MAX_DOCUMENTS)
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument[document - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  else
    return DocumentUsesNsPrefixes (LoadedDocument[document - 1]);
  return FALSE;
}

/*----------------------------------------------------------------------
  TtaFreeElemNamespaceDeclarations
  Free the namespaces declarations related to an element
  ----------------------------------------------------------------------*/
void TtaFreeElemNamespaceDeclarations (Document document, Element element)
{
  PtrDocument pDoc;

  UserErrorCode = 0;
  /* verifies the parameter document */
  if (document < 1 || document > MAX_DOCUMENTS)
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument[document - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  else
    {
      /* parameter document is correct */
      pDoc = LoadedDocument[document - 1];
      FreeElemNamespaceDeclarations (pDoc, (PtrElement) element);
    }
}

/*----------------------------------------------------------------------
  TtaFreeNamespaceDeclarations
  Free all the namespaces declarations of a document
  ----------------------------------------------------------------------*/
void TtaFreeNamespaceDeclarations (Document document)
{
  PtrDocument pDoc;

  UserErrorCode = 0;
  /* verifies the parameter document */
  if (document < 1 || document > MAX_DOCUMENTS)
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument[document - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  else
    {
      /* parameter document is correct */
      pDoc = LoadedDocument[document - 1];
      FreeNamespaceDeclarations (pDoc);
    }
}

/*----------------------------------------------------------------------
  TtaAppendXmlAttribute
  Add a new xml global attribute
  ----------------------------------------------------------------------*/
void TtaAppendXmlAttribute (char *xmlName, AttributeType *attrType, Document document)
{
  PtrDocument pDoc;

  UserErrorCode = 0;
  /* verifies the parameter document */
  if (document < 1 || document > MAX_DOCUMENTS)
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument[document - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  else
    {
      /* parameter document is correct */
      pDoc = LoadedDocument[document - 1];
      AppendXmlAttribute (xmlName, attrType, pDoc);
    }
}

/*----------------------------------------------------------------------
  TtaGetXmlAttributeType
  Search in attrType->AttrSSchema if not NULL otherwise,
  search in the different loaded natures.
  ----------------------------------------------------------------------*/
void TtaGetXmlAttributeType (char* xmlName, AttributeType *attrType, Document document)
{
  PtrDocument pDoc;

  UserErrorCode = 0;
  /* verifies the parameter document */
  if (document < 1 || document > MAX_DOCUMENTS)
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument[document - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  else
    {
      /* parameter document is correct */
      pDoc = LoadedDocument[document - 1];
      GetXmlAttributeType (xmlName, attrType, pDoc);
    }
}

#ifndef NODISPLAY
/*-----------------------------------------------------------------------------
  TtaAddEmptyBox
  Add the specific presentation rule Createlast(EmptyBox) to an empty element
  -----------------------------------------------------------------------------*/
void TtaAddEmptyBox (Element element)
{
  AddEmptyBox ((PtrElement) element);
}

/*----------------------------------------------------------------------
  TtaHasXmlInLineRule
  Retuns TRUE if the element type has a 'Line' presentation rule
  ----------------------------------------------------------------------*/
ThotBool TtaHasXmlInLineRule (ElementType elType, Document document)
{
  PtrDocument pDoc;

  UserErrorCode = 0;
  /* verifies the parameter document */
  if (document < 1 || document > MAX_DOCUMENTS)
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument[document - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  else
    {
      /* parameter document is correct */
      pDoc = LoadedDocument[document - 1];
      return HasXmlInLineRule (elType, pDoc);
    }
  return FALSE;
}

/*----------------------------------------------------------------------
  TtaSetXmlInLineRule
  Add an InLine generic rule to an element type
  ----------------------------------------------------------------------*/
void TtaSetXmlInLineRule (ElementType elType, Document document)
{
  PtrDocument pDoc;

  UserErrorCode = 0;
  /* verifies the parameter document */
  if (document < 1 || document > MAX_DOCUMENTS)
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument[document - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  else
    {
      /* parameter document is correct */
      pDoc = LoadedDocument[document - 1];
      SetXmlInLineRule (elType, pDoc);
    }
}
#endif 

/*----------------------------------------------------------------------
  TtaAppendXmlElement
  Add a new element to the schema
  ----------------------------------------------------------------------*/
void TtaAppendXmlElement (const char *xmlName, ElementType *elType,
                          char **mappedName, Document document)
{
  PtrDocument pDoc;

  UserErrorCode = 0;
  /* verifies the parameter document */
  if (document < 1 || document > MAX_DOCUMENTS)
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument[document - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  else
    {
      /* parameter document is correct */
      pDoc = LoadedDocument[document - 1];
      AppendXmlElement (xmlName, &(elType->ElTypeNum),
                        (PtrSSchema)(elType->ElSSchema),
                        mappedName, pDoc);
    }
}

/*----------------------------------------------------------------------
  TtaGetXmlElementType
  Search in elType->ElSSchema if not NULL otherwise,
  search in the different loaded natures.
  ----------------------------------------------------------------------*/
void TtaGetXmlElementType (const char *xmlName, ElementType *elType,
                           char **mappedName, Document document)
{
  PtrDocument pDoc;

  UserErrorCode = 0;
  /* verifies the parameter document */
  if (document < 1 || document > MAX_DOCUMENTS)
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument[document - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  else
    {
      /* parameter document is correct */
      pDoc = LoadedDocument[document - 1];
      GetXmlElementType (xmlName, elType, mappedName, pDoc);
    }
}

/*----------------------------------------------------------------------
  TtaSetUriSSchema
  Set the namespace uri associated with that schema
  ----------------------------------------------------------------------*/
void TtaSetUriSSchema (SSchema sSchema, const char *sSchemaUri)
{
  UserErrorCode = 0;
  if (sSchema == NULL)
    TtaError (ERR_invalid_parameter);
  else
    SetUriSSchema ((PtrSSchema) sSchema, sSchemaUri);
}

/*----------------------------------------------------------------------
  TtaChangeGenericSchemaNames
  Change the name of a generic xml schema
  ----------------------------------------------------------------------*/
void TtaChangeGenericSchemaNames (const char *sSchemaUri, const char *sSchemaName,
                                  Document document)
{
  PtrDocument pDoc;

  UserErrorCode = 0;
  /* verifies the parameter document */
  if (document < 1 || document > MAX_DOCUMENTS)
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument[document - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  else
    {
      /* parameter document is correct */
      pDoc = LoadedDocument[document - 1];
      ChangeGenericSchemaNames (sSchemaUri, sSchemaName, pDoc);
    }
}

/*----------------------------------------------------------------------
  TtaIsXmlSSchema
  Returns yes if schema corresponds to a generic xml structure schema
  Parameter:
  schema: the structure schema of interest.
  ----------------------------------------------------------------------*/
ThotBool TtaIsXmlSSchema (SSchema schema)
{
  UserErrorCode = 0;
  if (!schema)
    {
      TtaError (ERR_invalid_parameter);
      return TRUE;
    }
  return ((PtrSSchema) schema)->SsIsXml;
}

/*----------------------------------------------------------------------
  TtaAddDocumentReference
  Add a reference to the specified document.
  ----------------------------------------------------------------------*/
void TtaAddDocumentReference (Document document)
{
  PtrDocument pDoc;

  UserErrorCode = 0;
  /* verifies the parameter document */
  if (document < 1 || document > MAX_DOCUMENTS)
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument[document - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  else
    {
      /* parameter document is correct */
      pDoc = LoadedDocument[document - 1];
      pDoc->DocNbRef++;
    }
}

/*----------------------------------------------------------------------
  TtaRemoveDocumentReference
  Remove a reference to the specified document.
  If the document has no reference anymore, it is freed.
  ----------------------------------------------------------------------*/
void TtaRemoveDocumentReference (Document document)
{
  PtrDocument pDoc;

  UserErrorCode = 0;
  /* verifies the parameter document */
  if (document < 1 || document > MAX_DOCUMENTS)
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument[document - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  else
    {
      /* parameter document is correct */
      pDoc = LoadedDocument[document - 1];
      pDoc->DocNbRef--;
      if(pDoc->DocNbRef < 0)
        {
#ifndef NODISPLAY          
          TCloseDocument(pDoc);
#else /* NODISPLAY */
          TtaCloseDocument(document);
#endif /* NODISPLAY */
        }
    }
}

/*----------------------------------------------------------------------
  TtaDumpDocumentReference
  Show the number of reference of each opened document.
  ----------------------------------------------------------------------*/
void TtaDumpDocumentReference()
{
  int i;
  for(i=0; i<MAX_DOCUMENTS; i++)
    {
      PtrDocument pDoc = (PtrDocument)LoadedDocument[i];
      if(pDoc)
        {
          printf("[%02d] %s %d\n", i, pDoc->DocDName, pDoc->DocNbRef);
        }
    }
}
