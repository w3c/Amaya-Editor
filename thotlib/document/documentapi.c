/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 
#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "application.h"
#include "document.h"
#include "fileaccess.h"
#include "constpiv.h"
#include "typecorr.h"
#include "appdialogue.h"
#include "thotdir.h"
#include "fileaccess.h"

#ifdef NODISPLAY
/*** For the ThotKernel, variables FirstSelectedElement and
     LastSelectedElement, are defined here, to be used at the end of
     function MergeTextElements  ***/
#undef THOT_EXPORT
#define THOT_EXPORT
#include "select_tv.h"
#endif

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "edit_tv.h"
#include "platform_tv.h"
#ifndef NODISPLAY
#include "modif_tv.h"
#endif
#include "appdialogue_tv.h"

#include "tree_f.h"
#include "attributes_f.h"
#include "config_f.h"
#include "views_f.h"
#include "viewapi_f.h"

#include "draw_f.h"
#include "translation_f.h"
#include "memory_f.h"

#include "writepivot_f.h"
#include "readpivot_f.h"
#include "readstr_f.h"
#include "references_f.h"
#include "externalref_f.h"
#include "schemas_f.h"
#include "fileaccess_f.h"
#include "structschema_f.h"
#include "thotmsg_f.h"
#include "docs_f.h"
#include "applicationapi_f.h"
#include "platform_f.h"
#include "appdialogue_f.h"

extern int          UserErrorCode;
static Name         nameBuffer;

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateWithException (PtrElement pEl, PtrDocument pDoc)
#else  /* __STDC__ */
void                CreateWithException (pEl, pDoc)
PtrElement          pEl;
PtrDocument         pDoc;
#endif /* __STDC__ */
{
   /* If table creation */
   if (ThotLocalActions[T_createtable] != NULL)
      (*ThotLocalActions[T_createtable]) (pEl, pDoc);
}


/*----------------------------------------------------------------------
   TtaNewDocument

   Creates the internal representation of a new document according to a given
   structure schema. No file is created immediately, but the backup files
   (.BAK and .SAV) and the document file (.PIV see TtaNewDocument) will be created
   in the first directory of the document path (see TtaSetDocumentPath).

   Parameters:
   structureSchema: name of the structure schema that defines the type of
   document to be created.
   documentName: name of the document to be created (maximum length 19
   characters). The directory name is not part of this parameter
   (see TtaSetDocumentPath).

   Return value:
   the document that has been created or 0 if the document has not
   been created.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
Document            TtaNewDocument (char *structureSchema, char *documentName)
#else  /* __STDC__ */
Document            TtaNewDocument (structureSchema, documentName)
char               *structureSchema;
char               *documentName;
#endif /* __STDC__ */
{
  PtrDocument         pDoc;
  Document            document;
  int                 i;
  
  UserErrorCode = 0;
  document = 0;
  pDoc = NULL;
  if (documentName[0] == EOS)
    /* No name provided by the user */
    TtaError (ERR_document_name);
  else
    {
      /* initializes a document context */
      CreateDocument (&pDoc);
      if (pDoc == NULL)
	/* No free context document */
	TtaError (ERR_too_many_documents);
      else
	{
	  Name sschemaName;
	  
	  strncpy(sschemaName, structureSchema, MAX_NAME_LENGTH);
	  /* charge le schema de structure */
	  GetSchStruct (&pDoc->DocSSchema);
	  pDoc->DocSSchema->SsExtension = FALSE;
	  if (!ReadStructureSchema (sschemaName, pDoc->DocSSchema) ||
	      pDoc->DocSSchema->SsExtension)
	    /* failure while reading the structure schema or while loading
	       a schema extension */
	    {
	      FreeSchStruc (pDoc->DocSSchema);
	      pDoc->DocSSchema = NULL;
	      UnloadDocument (&pDoc);
	      TtaError (ERR_cannot_read_struct_schema);
	    }
	  else
	    {
	      /* The structure schema is loaded */
	      /* The structure schema is translated into the user language */
	      ConfigTranslateSSchema (pDoc->DocSSchema);
#ifndef NODISPLAY
	      InitApplicationSchema (pDoc->DocSSchema);
#endif
	      /* One create the internal representation of an empty document */
	      pDoc->DocRootElement = NewSubtree (pDoc->DocSSchema->SsRootElem,
						 pDoc->DocSSchema, pDoc, 0, TRUE, TRUE, TRUE, TRUE);
	      /* suppress excluded elements */
	      RemoveExcludedElem (&pDoc->DocRootElement);
	      if (pDoc->DocRootElement == NULL)
		{
		  UnloadDocument (&pDoc);
		  TtaError (ERR_empty_document);
		}
	      else
		{
		  pDoc->DocRootElement->ElAccess = AccessReadWrite;
#ifndef NODISPLAY
		  /* Create required attributes by the whole created tree */
		  AttachMandatoryAttributes (pDoc->DocRootElement, pDoc);
#endif
		  /* dealing with exceptions */
		  CreateWithException (pDoc->DocRootElement, pDoc);
		  /* An attribut Language is stored in the root */
		  CheckLanguageAttr (pDoc, pDoc->DocRootElement);
		  /* The document is named */
		  strncpy (pDoc->DocDName, documentName, MAX_NAME_LENGTH);
		  /* one get an identifier to the document */
		  GetDocIdent (&pDoc->DocIdent, documentName);
		  /* keep the actual schema path in the document context */
		  strncpy (pDoc->DocSchemasPath, SchemaPath, MAX_PATH);
		  /* initializes the directory of the document */
		  strncpy (pDoc->DocDirectory, DocumentPath, MAX_PATH);
		  /* if path, keep only the first directory */
		  i = 1;
		  while (pDoc->DocDirectory[i - 1] != EOS &&
			 pDoc->DocDirectory[i - 1] != PATH_SEP && i < MAX_PATH)
		    i++;
		  pDoc->DocDirectory[i - 1] = EOS;
		  /* Read-Write document */
		  pDoc->DocReadOnly = FALSE;
		  document = IdentDocument (pDoc);
		}
	    }
	}
    }
  return document;
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
#ifdef __STDC__
Document            TtaOpenDocument (char *documentName, int accessMode)
#else  /* __STDC__ */
Document            TtaOpenDocument (documentName, accessMode)
char               *documentName;
int                 accessMode;
#endif /* __STDC__ */
{
   PtrDocument         pDoc;
   Document            document;
   int                 lg;
   boolean             ok;

   UserErrorCode = 0;
   document = 0;
   /* initializes the document context */
   CreateDocument (&pDoc);
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
	     /* suppresses the .PIV suffix if found */
	     if (lg > 4)
		if (strcmp (&(pDoc->DocDName[lg - 4]), ".PIV") == 0)
		   pDoc->DocDName[lg - 4] = EOS;
	     GetDocIdent (&pDoc->DocIdent, pDoc->DocDName);
	     strncpy (pDoc->DocDirectory, DocumentPath, MAX_PATH);
	     ok = OpenDocument (pDoc->DocDName, pDoc, TRUE, FALSE, NULL,
				FALSE, TRUE);
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
		  document = IdentDocument (pDoc);
		  if (!pDoc->DocReadOnly)
		     pDoc->DocReadOnly = (accessMode == 0);
	       }
	  }
     }
   return document;
}


/*----------------------------------------------------------------------
   TtaSaveDocument

   Saves a document into a file in Thot format. The document is not closed
   by the function and can still be accessed by the application program.

   Parameters:
   document: the document to be saved.
   documentName: name of the file in which the document must be saved
   (maximum length 19 characters). The directory name is not part of
   this parameter (see TtaSetDocumentPath).
   If the documentName is not the same as the one used when opening
   (see TtaOpenDocument) or creating (see TtaNewDocument) the document,
   a new file is created and the file with the old name is unchanged,
   i. e. a new version is created. If necessary, the old file can be
   removed by the function TtaRemoveDocument.

  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaSaveDocument (Document document, char *documentName)
#else  /* __STDC__ */
void                TtaSaveDocument (document, documentName)
Document            document;
char               *documentName;
#endif /* __STDC__ */
{
  PtrDocument         pDoc;
  BinFile             pivotFile;
  char                path[250];
  int                 i;

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
      if (pDoc->DocReadOnly)
	TtaError (ERR_read_only_document);
      else
	{
	  /* Arrange the file name */
	  FindCompleteName (documentName, "PIV", pDoc->DocDirectory, path, &i);
	  pivotFile = TtaWriteOpen (path);
	  if (pivotFile == 0)
	    TtaError (ERR_cannot_open_pivot_file);
	  else
	    {
	      /* writing the document in the file in the pivot format */
	      SauveDoc (pivotFile, pDoc);
	      TtaWriteClose (pivotFile);
	      /* modifies files .EXT of new referenced documents or file which
		 are no more referenced bu the document */
	      UpdateExt (pDoc);
	      /* modifies files .REF of documents that reference elements which are
		 no more in the document and updates the .EXT file relating to the document */
	      UpdateRef (pDoc);
	      if (strcmp (documentName, pDoc->DocDName) != 0)
		/* The document is saved under a new name */
		{
		  /* The application wants to create a copy of the document */
		  /* The document copy will be in the .EXT files relating to the 
		     referenced documents */
		  ChangeNomExt (pDoc, documentName, TRUE);
		  /* Puts the new name into the document descriptor */
		  strncpy (pDoc->DocDName, documentName, MAX_NAME_LENGTH);
		  strncpy (pDoc->DocIdent, documentName, MAX_DOC_IDENT_LEN);
#ifndef NODISPLAY
		  /* changes the title of frames */
		  ChangeDocumentName (pDoc, documentName);
#endif
		}
	    }
	}
    }
}

/*----------------------------------------------------------------------
   TtaExportDocument

   Saves a whole document into a file in a particular format. The output
   format is specified by a translation schema. The document is not closed
   by the function and it can still be accessed by the application program.

   Parameters:
   document: the document to be exported.
   fileName: name of the file in which the document must be saved,
   including the directory name.
   TSchemaName: name of the translation schema to be used. The directory
   name must not be specified in parameter TSchemaName. See
   function TtaSetSchemaPath.

  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaExportDocument (Document document, char *fileName, char *TSchemaName)
#else  /* __STDC__ */
void                TtaExportDocument (document, fileName, TSchemaName)
Document            document;
char               *fileName;
char               *TSchemaName;
#endif /* __STDC__ */
{
  UserErrorCode = 0;
  /* verifies the parameter document */
  if (document < 1 || document > MAX_DOCUMENTS)
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument[document - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  else
    /* parameter document is correct */
    ExportDocument (LoadedDocument[document - 1], fileName, TSchemaName);
}

/*----------------------------------------------------------------------
   TtaCloseDocument

   Closes a document that is no longer needed and releases all ressources
   allocated to the document. This function does not save the document.

   Parameter:
   document: the document to be closed.

  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaCloseDocument (Document document)
#else  /* __STDC__ */
void                TtaCloseDocument (document)
Document            document;
#endif /* __STDC__ */
{
#ifndef NODISPLAY
  int              nv, numassoc;
#endif
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
#ifndef NODISPLAY
      /* Closing all opened views relating to the document */
      /* First, one close the views of the main tree */
      for (nv = 1; nv <= MAX_VIEW_DOC; nv++)
	if (pDoc->DocView[nv - 1].DvPSchemaView != 0)
	  {
	    DestroyFrame (pDoc->DocViewFrame[nv - 1]);
	    CloseDocumentView (pDoc, nv, FALSE, FALSE);
	  }
      /* Then one close frames of associated elements */
      for (numassoc = 1; numassoc <= MAX_ASSOC_DOC; numassoc++)
	if (pDoc->DocAssocFrame[numassoc - 1] != 0)
	  {
	    DestroyFrame (pDoc->DocAssocFrame[numassoc - 1]);
	    CloseDocumentView (pDoc, numassoc, TRUE, FALSE);
	  }
      UnloadTree (document);
#else
      DeleteAllTrees (pDoc);
#endif
      UnloadDocument (&pDoc);
    }
}


/*----------------------------------------------------------------------
   TtaRemoveDocument

   Closes a document, releases all ressources allocated to that document,
   removes all files related to the document and updates all links connecting
   the removed document with other documents.

   Parameter:
   document: the document to be removed.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaRemoveDocument (Document document)
#else  /* __STDC__ */
void                TtaRemoveDocument (document)
Document            document;
#endif /* __STDC__ */
{
  PtrDocument         pDoc;
  int                 i;
  PathBuffer          DirectoryOrig;
  char                text[MAX_TXT_LEN];

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
      /* Keep all external referenced links into the document context */
      /* dealing with the main tree of the document */
      RegisterExternalRef (pDoc->DocRootElement, pDoc, FALSE);
      RegisterDeletedReferredElem (pDoc->DocRootElement, pDoc);
      /* dealing with the trees os associated elements */
      for (i = 1; i <= MAX_ASSOC_DOC; i++)
	if (pDoc->DocAssocRoot[i - 1] != NULL)
	  {
	    RegisterExternalRef (pDoc->DocAssocRoot[i - 1], pDoc, FALSE);
	    RegisterDeletedReferredElem (pDoc->DocAssocRoot[i - 1], pDoc);
	  }
      /* treats the parameters */
      for (i = 1; i <= MAX_PARAM_DOC; i++)
	if (pDoc->DocParameters[i - 1] != NULL)
	  {
	    RegisterExternalRef (pDoc->DocParameters[i - 1], pDoc, FALSE);
	    RegisterDeletedReferredElem (pDoc->DocParameters[i - 1], pDoc);
	  }
      /* modifies files .EXT of documents referenced by destroyed documents */
      UpdateExt (pDoc);
      /* modifies files .REF of documents referencing inexisting documents */
      UpdateRef (pDoc);
      /* destroys files .PIV, .EXT, .REF et .BAK of the document */
      strncpy (DirectoryOrig, pDoc->DocDirectory, MAX_PATH);
      FindCompleteName (pDoc->DocDName, "PIV", DirectoryOrig, text, &i);
      TtaFileUnlink (text);
      strncpy (DirectoryOrig, pDoc->DocDirectory, MAX_PATH);
      FindCompleteName (pDoc->DocDName, "EXT", DirectoryOrig, text, &i);
      TtaFileUnlink (text);
      strncpy (DirectoryOrig, pDoc->DocDirectory, MAX_PATH);
      FindCompleteName (pDoc->DocDName, "REF", DirectoryOrig, text, &i);
      TtaFileUnlink (text);
      strncpy (DirectoryOrig, pDoc->DocDirectory, MAX_PATH);
      FindCompleteName (pDoc->DocDName, "BAK", DirectoryOrig, text, &i);
      /* now close the document */
      TtaCloseDocument (document);
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
#ifdef __STDC__
void                TtaSetDocumentPath (char *path)
#else  /* __STDC__ */
void                TtaSetDocumentPath (path)
char               *path;
#endif /* __STDC__ */
{
   UserErrorCode = 0;
   if (strlen (path) >= MAX_PATH)
      TtaError (ERR_string_too_long);
   else
     {
	strcpy (DocumentPath, path);
     }
}


/*----------------------------------------------------------------------
   TtaCheckDirectory

   Ckecks that a directory exists and can be accessed.

   Parameter:
   directory: the directory name.
   Return value:
   TRUE if the directory is OK, FALSE if not.
	
  ----------------------------------------------------------------------*/
#ifdef __STDC__
boolean             TtaCheckDirectory (char *directory)

#else  /* __STDC__ */
boolean             TtaCheckDirectory (directory)
char               *directory;

#endif /* __STDC__ */

{
#ifdef WWW_MSWINDOWS
   DWORD               attribs;

   /* NEW_WINDOWS - mark for furthur security stuff - EGP
      SECURITY_INFORMATION secInfo;
      SECURITY_DESCRIPTOR secDesc; */
   attribs = GetFileAttributes (directory);
   if (attribs == 0xFFFFFFFF)
      return FALSE;
   else if (!(attribs & FILE_ATTRIBUTE_DIRECTORY))
      return FALSE;
   return TRUE;
#else  /* WWW_MSWINDOWS */
   struct stat         fileStat;

   /* does the directory exist ? */
   if (strlen (directory) < 1)
      return (FALSE);
   else if (stat (directory, &fileStat) != 0)
      return (FALSE);
   else if (S_ISDIR (fileStat.st_mode))
      return (TRUE);
   else
      return (FALSE);
#endif /* !WWW_MSWINDOWS */
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
#ifdef __STDC__
boolean             TtaCheckPath (PathBuffer path)

#else  /* __STDC__ */
boolean             TtaCheckPath (path)
PathBuffer          path;

#endif /* __STDC__ */

{
   int                 i, j;
   PathBuffer          single_directory;
   boolean             OK;

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
#ifdef __STDC__
boolean             TtaIsInDocumentPath (char *directory)

#else  /* __STDC__ */
boolean             TtaIsInDocumentPath (directory)
char               *directory;

#endif /* __STDC__ */

{
   int                 i;
   char               *ptr;

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

#ifdef __STDC__
void                TtaAppendDocumentPath (char *directory)

#else  /* __STDC__ */
void                TtaAppendDocumentPath (directory)
char               *directory;

#endif /* __STDC__ */

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

#ifdef __STDC__
void                TtaSetSchemaPath (char *path)

#else  /* __STDC__ */
void                TtaSetSchemaPath (path)
char               *path;

#endif /* __STDC__ */

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

#ifdef __STDC__
SSchema             TtaNewNature (SSchema schema, char *natureName, char *presentationName)

#else  /* __STDC__ */
SSchema             TtaNewNature (schema, natureName, presentationName)
SSchema             schema;
char               *natureName;
char               *presentationName;

#endif /* __STDC__ */

{
   int                 natureRule;
   PtrSSchema          natureSchema;

   UserErrorCode = 0;
   natureSchema = NULL;
   if (schema == NULL || natureName[0] == EOS)
     {
	TtaError (ERR_invalid_parameter);
     }
   else
     {
	natureRule = CreateNature (natureName, presentationName,
				   (PtrSSchema) schema);
	if (natureRule == 0)
	  {
	     TtaError (ERR_invalid_parameter);
	  }
	else
	   natureSchema = ((PtrSSchema) schema)->SsRule[natureRule - 1].SrSSchemaNat;
     }
   return ((SSchema) natureSchema);
}


/*----------------------------------------------------------------------
   TtaNewSchemaExtension

   Loads a structure schema extension and associates it with
   a given document.

   Parameters:
   document: the document whose structure schema must be extended.
   extensionName: name of the extension schema.
   presentationName: name of the presentation schema to be associated with
   the extension schema. If presentationName is an empty string, the
   default presentation schema is associated.

   Return value:
   the extension schema, NULL if the extension schema has not been loaded.

  ----------------------------------------------------------------------*/

#ifdef __STDC__
SSchema             TtaNewSchemaExtension (Document document, char *extensionName, char *presentationName)

#else  /* __STDC__ */
SSchema             TtaNewSchemaExtension (document, extensionName, presentationName)
Document            document;
char               *extensionName;
char               *presentationName;

#endif /* __STDC__ */

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
	extension = LoadExtension (extensionName, presentationName,
				   LoadedDocument[document - 1]);
	if (extension == NULL)
	  {
	     TtaError (ERR_cannot_read_struct_schema);
	  }
     }
   return ((SSchema) extension);
}

#ifdef __STDC__
static void         RemoveExtensionFromTree (PtrElement * pEl, Document document, PtrSSchema pSSExt, int *removedElements, int *removedAttributes)

#else  /* __STDC__ */
static void         RemoveExtensionFromTree (pEl, document, pSSExt, removedElements, removedAttributes)
PtrElement         *pEl;
Document            document;
PtrSSchema          pSSExt;
int                *removedElements;
int                *removedAttributes;

#endif /* __STDC__ */

{
   PtrDocument         pDoc;
   PtrElement          child, nextChild;
   PtrAttribute        attribute, nextAttribute;

   if (*pEl != NULL)
     {
	pDoc = LoadedDocument[document - 1];
	if ((*pEl)->ElStructSchema == pSSExt)
	   /* this element belongs to the extension schema to be removed */
	  {
	     RegisterExternalRef (*pEl, pDoc, FALSE);
	     RegisterDeletedReferredElem (*pEl, pDoc);
#ifndef NODISPLAY
	     UndisplayElement (*pEl, document);
#endif
	     DeleteElement (pEl);
	     *pEl = NULL;
	     (*removedElements)++;
	  }
	else
	  {
	     /* looks for all attributes associated with that element */
	     attribute = (*pEl)->ElFirstAttr;
	     while (attribute != NULL)
	       {
		  nextAttribute = attribute->AeNext;
		  if (attribute->AeAttrSSchema == pSSExt)
		    {
		       TtaRemoveAttribute ((Element) (*pEl), (Attribute) attribute, document);
#ifndef NODISPLAY
		       UndisplayInheritedAttributes (*pEl, attribute, document, TRUE);
#endif
#ifndef NODISPLAY
		       UndisplayAttribute (*pEl, attribute, document);
#endif
		       (*removedAttributes)++;
		    }
		  attribute = nextAttribute;
	       }

	     if ((*pEl)->ElTerminal)
		child = NULL;
	     else
		child = (*pEl)->ElFirstChild;
	     while (child != NULL)
	       {
		  nextChild = child->ElNext;
		  RemoveExtensionFromTree (&child, document, pSSExt, removedElements,
					   removedAttributes);
		  child = nextChild;
	       }
	  }
     }
}


/*----------------------------------------------------------------------
   TtaRemoveSchemaExtension

   Removes a structure schema extension from a given document. Removes also from
   the document all attributes and elements defined in that structure schema
   extension.

   Parameters:
   document: the document.
   extension: the structure schema extension to be removed.

   Return parameters:
   removedElements: number of elements actually removed.
   removedAttributes: number of attributes actually removed.

  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                TtaRemoveSchemaExtension (Document document, SSchema extension, int *removedElements, int *removedAttributes)

#else  /* __STDC__ */
void                TtaRemoveSchemaExtension (document, extension, removedElements, removedAttributes)
Document            document;
SSchema             extension;
int                *removedElements;
int                *removedAttributes;

#endif /* __STDC__ */

{
   PtrSSchema          curExtension, previousSSchema;
   PtrElement          root;
   PtrDocument         pDoc;
   boolean             found;
   int                 assoc;

   UserErrorCode = 0;
   /* verifies the parameter document */
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
	pDoc = LoadedDocument[document - 1];
	/* Looks for the extension to suppress */
	previousSSchema = pDoc->DocSSchema;
	curExtension = previousSSchema->SsNextExtens;
	found = FALSE;
	while (!found && curExtension != NULL)
	   if (((PtrSSchema) extension)->SsCode == curExtension->SsCode)
	      found = TRUE;
	   else
	     {
		previousSSchema = curExtension;
		curExtension = curExtension->SsNextExtens;
	     }
	if (!found)
	  {
	     TtaError (ERR_invalid_document_parameter);
	  }
	else
	  {
	     root = pDoc->DocRootElement;
	     if (root != NULL)
		RemoveExtensionFromTree (&root, document, (PtrSSchema) extension,
					 removedElements, removedAttributes);
	     for (assoc = 0; assoc < MAX_ASSOC_DOC; assoc++)
	       {
		  root = pDoc->DocAssocRoot[assoc];
		  RemoveExtensionFromTree (&root, document, (PtrSSchema) extension,
					removedElements, removedAttributes);
		  if (root == NULL)
		     pDoc->DocAssocRoot[assoc] = NULL;
	       }
	     previousSSchema->SsNextExtens = curExtension->SsNextExtens;
	     if (curExtension->SsNextExtens != NULL)
		curExtension->SsNextExtens->SsPrevExtens = previousSSchema;
#ifndef NODISPLAY
	     FreePresentationSchema (curExtension->SsPSchema, curExtension);
#endif
	     FreeSchStruc (curExtension);
	  }
     }
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
#ifdef __STDC__
void                TtaSetPSchema (Document document, char *presentationName)
#else  /* __STDC__ */
void                TtaSetPSchema (document, presentationName)
Document            document;
char               *presentationName;
#endif /* __STDC__ */

{
   PtrDocument         pDoc;
#ifndef NODISPLAY
   int                 view;
   int                 Assoc;
   boolean             ok;

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
	   strncpy (pDoc->DocSSchema->SsDefaultPSchema, presentationName,
		    MAX_NAME_LENGTH - 1);
#else
	/* verifies that there is no opened views */
	ok = TRUE;
	for (view = 1; view <= MAX_VIEW_DOC && ok; view++)
	   if (pDoc->DocView[view - 1].DvPSchemaView != 0)
	      ok = FALSE;
	if (ok)
	   for (Assoc = 1; Assoc <= MAX_ASSOC_DOC && ok; Assoc++)
	      if (pDoc->DocAssocFrame[Assoc - 1] != 0)
		 ok = FALSE;
	if (!ok)
	     TtaError (ERR_there_are_open_views);
	else
	   /* There is no opened views */
	  {
	     Name pschemaName;

	     strncpy(pschemaName, presentationName, MAX_NAME_LENGTH);
	     if (pDoc->DocSSchema->SsPSchema != NULL)
		/* a presentation schema already exist. One release it */
	       {
		  FreePresentationSchema (pDoc->DocSSchema->SsPSchema, pDoc->DocSSchema);
		  pDoc->DocSSchema->SsPSchema = NULL;
	       }
	     /* Load the presentation schema */
	     if (pDoc->DocSSchema->SsExtension)
		/* to avoid that ReadPresentationSchema reloades the structure schema */
		pDoc->DocSSchema->SsRootElem = 1;
	     pDoc->DocSSchema->SsPSchema = LoadPresentationSchema (pschemaName,
							  pDoc->DocSSchema);
	     if (pDoc->DocSSchema->SsPSchema == NULL)
		/* Failure while loading schema */
		  TtaError (ERR_cannot_load_pschema);
	  }
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
#ifdef __STDC__
void                TtaSetDocumentDirectory (Document document, char *directory)
#else  /* __STDC__ */
void                TtaSetDocumentDirectory (document, directory)
Document            document;
char               *directory;
#endif /* __STDC__ */

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
   TtaSetDocumentName

   Sets or changes the name of a document. The document must be loaded.

   Parameters:
   document: the document whose name is set.
   documentName: new document name. This is only the name, without any
   suffix, without directory name. See function TtaSetDocumentDirectory
   for changing the directory of a document.
   The name must not exceed 31 characters.

  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaSetDocumentName (Document document, char *documentName)
#else  /* __STDC__ */
void                TtaSetDocumentName (document, documentName)
Document            document;
char               *documentName;
#endif /* __STDC__ */

{
   UserErrorCode = 0;
   /* verifies the parameter document */
   if (document < 1 || document > MAX_DOCUMENTS)
     TtaError (ERR_invalid_document_parameter);
   else if (LoadedDocument[document - 1] == NULL)
     TtaError (ERR_invalid_document_parameter);
   /* parameter document is correct */
   else if (strlen (documentName) >= MAX_NAME_LENGTH)
     TtaError (ERR_buffer_too_small);
   else
     {
#ifndef NODISPLAY
       ChangeDocumentName (LoadedDocument[document - 1], documentName);
#else
       strncpy (LoadedDocument[document - 1]->DocDName, documentName, MAX_NAME_LENGTH);
       strncpy (LoadedDocument[document - 1]->DocIdent, documentName, MAX_DOC_IDENT_LEN);
#endif
     }
}


/*----------------------------------------------------------------------
   TtaSetDocumentAccessMode

   Sets the access mode for a document.

   Parameters:
   document: the document whose access mode is changed.
   accessMode: 0 = read only, 1 = read-write.

  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaSetDocumentAccessMode (Document document, int accessMode)
#else  /* __STDC__ */
void                TtaSetDocumentAccessMode (document, accessMode)
Document            document;
int                 accessMode;
#endif /* __STDC__ */

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
	LoadedDocument[document - 1]->DocReadOnly = (accessMode == 0);
#ifndef NODISPLAY
	SetAccessMode (LoadedDocument[document - 1], accessMode);
#endif
     }
}


/*----------------------------------------------------------------------
   TtaSetDocumentBackUpInterval

   Sets the backup interval for a document.

   Parameters:
   document: the document whose backup interval is changed.
   interval:
   0 : the backup mechanism must be disabled
   positive integer : number of characters typed which triggers
   automatic save of the document into a .BAK file.

  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                TtaSetDocumentBackUpInterval (Document document, int interval)

#else  /* __STDC__ */
void                TtaSetDocumentBackUpInterval (document, interval)
Document            document;
int                 interval;

#endif /* __STDC__ */

{
   UserErrorCode = 0;
   /* verifies the parameter document */
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
   if (interval < 0)
     {
	TtaError (ERR_invalid_parameter);
     }
   else
      LoadedDocument[document - 1]->DocBackUpInterval = interval;
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

#ifdef __STDC__
void                TtaSetNotificationMode (Document document, int notificationMode)

#else  /* __STDC__ */
void                TtaSetNotificationMode (document, notificationMode)
Document            document;
int                 notificationMode;

#endif /* __STDC__ */

{
   UserErrorCode = 0;
   /* verifies the parameter document */
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
      LoadedDocument[document - 1]->DocNotifyAll = (notificationMode != 0);
}


/*----------------------------------------------------------------------
   TtaSetDocumentModified

   Notifies the tool kit that a document has been modified by the application.
   As a consequence, the user will be asked to save the document when closing it.

   Parameter:
   document: the document.

  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                TtaSetDocumentModified (Document document)

#else  /* __STDC__ */
void                TtaSetDocumentModified (document)
Document            document;

#endif /* __STDC__ */

{
   UserErrorCode = 0;
   /* verifies the parameter document */
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
      LoadedDocument[document - 1]->DocModified = TRUE;
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

#ifdef __STDC__
void                TtaSetDocumentUnmodified (Document document)

#else  /* __STDC__ */
void                TtaSetDocumentUnmodified (document)
Document            document;

#endif /* __STDC__ */

{
   UserErrorCode = 0;
   /* verifies the parameter document */
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
      LoadedDocument[document - 1]->DocModified = FALSE;
}

/*----------------------------------------------------------------------
   TtaGetDocumentName

   Returns the name of a document.

   Parameter:
   document: the document whose name is asked.

   Return value:
   name of that document.

  ----------------------------------------------------------------------*/

#ifdef __STDC__
char               *TtaGetDocumentName (Document document)

#else  /* __STDC__ */
char               *TtaGetDocumentName (document)
Document            document;

#endif /* __STDC__ */

{
   UserErrorCode = 0;
   nameBuffer[0] = EOS;
   /* verifies the parameter document */
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
	strcpy (nameBuffer, LoadedDocument[document - 1]->DocDName);
     }
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

#ifdef __STDC__
Document            TtaGetDocumentFromName (char *documentName)

#else  /* __STDC__ */
Document            TtaGetDocumentFromName (documentName)
char               *documentName;

#endif /* __STDC__ */

{
   int                 document;
   boolean             found;

   UserErrorCode = 0;
   document = 1;
   found = FALSE;
   while (!found && document < MAX_DOCUMENTS)
     {
	if (LoadedDocument[document - 1] != NULL)
	   if (strcmp (documentName, LoadedDocument[document - 1]->DocDName) == 0)
	      found = TRUE;
	if (!found)
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

#ifdef __STDC__
void                TtaGetDocumentDirectory (Document document, char *buffer, int bufferLength)

#else  /* __STDC__ */
void                TtaGetDocumentDirectory (document, buffer, bufferLength)
Document            document;
char               *buffer;
int                 bufferLength;

#endif /* __STDC__ */

{
   UserErrorCode = 0;
   nameBuffer[0] = EOS;
   /* verifies the parameter document */
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
	if (strlen (LoadedDocument[document - 1]->DocDirectory) >= bufferLength)
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

#ifdef __STDC__
SSchema             TtaGetDocumentSSchema (Document document)

#else  /* __STDC__ */
SSchema             TtaGetDocumentSSchema (document)
Document            document;

#endif /* __STDC__ */

{
   SSchema             schema;

   UserErrorCode = 0;
   /* verifies the parameter document */
   schema = NULL;
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
	schema = (SSchema) LoadedDocument[document - 1]->DocSSchema;
     }
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

#ifdef __STDC__
char               *TtaGetSSchemaName (SSchema schema)

#else  /* __STDC__ */
char               *TtaGetSSchemaName (schema)
SSchema             schema;

#endif /* __STDC__ */

{
   UserErrorCode = 0;
   if (schema == NULL)
     {
	nameBuffer[0] = EOS;
	TtaError (ERR_invalid_parameter);
     }
   else
     {
	strcpy (nameBuffer, ((PtrSSchema) schema)->SsName);
     }
   return nameBuffer;
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

#ifdef __STDC__
char               *TtaGetPSchemaName (SSchema schema)

#else  /* __STDC__ */
char               *TtaGetPSchemaName (schema)
SSchema             schema;

#endif /* __STDC__ */

{
   UserErrorCode = 0;
   if (schema == NULL)
     {
	nameBuffer[0] = EOS;
	TtaError (ERR_invalid_parameter);
     }
   else
     {
	strcpy (nameBuffer, ((PtrSSchema) schema)->SsDefaultPSchema);
     }
   return nameBuffer;
}

/* ChSchStruct recursively searches the schema which name is "name" within
   nature schema and extension schema used by pSS. It returns a pointer
   which references this schema or NULL if not found. */
#ifdef __STDC__
static SSchema      ChSchStruct (PtrSSchema pSS, char *name)

#else  /* __STDC__ */
static SSchema      ChSchStruct (pSS, name)
PtrSSchema          pSS;
char               *name;

#endif /* __STDC__ */

{
   int                 nRegle;
   SSchema             retour;

   retour = NULL;
   if (pSS != NULL)
      if (strcmp (name, pSS->SsName) == 0)
	 /* The schema itself */
	 retour = (SSchema) pSS;
      else
	{
	   /* Looks for the nature rule of the schema */
	   for (nRegle = MAX_BASIC_TYPE - 1; retour == NULL && nRegle < pSS->SsNRules; nRegle++)
	      if (pSS->SsRule[nRegle].SrConstruct == CsNatureSchema)
		 retour = ChSchStruct (pSS->SsRule[nRegle].SrSSchemaNat, name);
	   /* If not found, one search into the extension schema */
	   if (retour == NULL)
	      retour = ChSchStruct (pSS->SsNextExtens, name);
	}
   return retour;
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
#ifdef __STDC__
SSchema             TtaGetSSchema (char *name, Document document)
#else  /* __STDC__ */
SSchema             TtaGetSSchema (name, document)
char               *name;
Document            document;
#endif /* __STDC__ */

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
     /* One search from the main schema of the document o */
     schema = ChSchStruct (LoadedDocument[document - 1]->DocSSchema, name);
   return schema;
}


/*----------------------------------------------------------------------
   TtaSameSSchemas

   Compares two structure schemas.

   Parameters:
   schema1: first structure schema.
   schema2: second structure schema.

   Return value:
   0 if both schemas are different, 1 if they are identical.

  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                 TtaSameSSchemas (SSchema schema1, SSchema schema2)
#else  /* __STDC__ */
int                 TtaSameSSchemas (schema1, schema2)
SSchema             schema1;
SSchema             schema2;
#endif /* __STDC__ */

{
   int                 result;

   UserErrorCode = 0;
   result = 0;
   if (schema1 == NULL || schema2 == NULL)
	TtaError (ERR_invalid_parameter);
   else if (((PtrSSchema) schema1)->SsCode == ((PtrSSchema) schema2)->SsCode)
      result = 1;
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
#ifdef __STDC__
void                TtaGiveSchemasOfDocument (char *documentName, char *structureName, char *presentationName)
#else  /* __STDC__ */
void                TtaGiveSchemasOfDocument (documentName, structureName, presentationName)
char               *documentName;
char               *structureName;
char               *presentationName;
#endif /* __STDC__ */

{
   PathBuffer          DirBuffer;
   BinFile             file;
   char                text[MAX_TXT_LEN];
   int                 i;
   boolean             error;
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
	if (!TtaReadByte (file, &charGotten))
	   error = TRUE;
	if (charGotten == (char) C_PIV_VERSION)
	  {
	     if (!TtaReadByte (file, &charGotten))
		error = TRUE;
	     if (!TtaReadByte (file, &charGotten))
		error = TRUE;
	     else
		currentVersion = (int) charGotten;
	     if (!TtaReadByte (file, &charGotten))
		error = TRUE;
	  }
	/* Gets the label max. of the document if it is present */
	if (!error && (charGotten == (char) C_PIV_SHORT_LABEL || charGotten == (char) C_PIV_LONG_LABEL ||
		       charGotten == (char) C_PIV_LABEL))
	  {
	     ReadLabel (charGotten, lab, file);
	     if (!TtaReadByte (file, &charGotten))
		error = TRUE;
	  }

	if (currentVersion >= 4)
	  {
	     /* Gets the table of laguages used by the document */
	     while (charGotten == (char) C_PIV_LANG && !error)
	       {
		  do
		     if (!TtaReadByte (file, &charGotten))
			error = TRUE;
		  while (!(error || charGotten == EOS)) ;
		  if (charGotten != EOS)
		     error = TRUE;
		  else
		     /* Gets the byte following the language name */
		  if (!TtaReadByte (file, &charGotten))
		     error = TRUE;
	       }
	  }

	/* Gets the comment of the document if it exists */
	if (!error && (charGotten == (char) C_PIV_COMMENT || charGotten == (char) C_PIV_OLD_COMMENT))
	  {
	     /* Get the byte following the comment */
	     if (!TtaReadByte (file, &charGotten))
		error = TRUE;
	  }
	/* Gets the name of the schema structure which is at the begenning of the pivot file */
	if (!error && charGotten != (char) C_PIV_NATURE)
	   error = TRUE;
	if (!error)
	  {
	     i = 0;
	     do
		if (!TtaReadByte (file, &structureName[i++]))
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
			  if (!TtaReadByte (file, &presentationName[i++]))
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
#ifdef __STDC__
void                TtaNextSchemaExtension (Document document, SSchema * extension)
#else  /* __STDC__ */
void                TtaNextSchemaExtension (document, extension)
Document            document;
SSchema            *extension;
#endif /* __STDC__ */

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
#ifdef __STDC__
void                TtaNextNature (Document document, SSchema * nature)
#else  /* __STDC__ */
void                TtaNextNature (document, nature)
Document            document;
SSchema            *nature;
#endif /* __STDC__ */

{
   PtrSSchema          nextNature;
   int                 n;
   PtrDocument         pDoc;
   boolean             found;

   UserErrorCode = 0;
   /* verifies the parameter document */
   nextNature = NULL;
   if (document < 1 || document > MAX_DOCUMENTS)
	TtaError (ERR_invalid_document_parameter);
   else if (LoadedDocument[document - 1] == NULL)
	TtaError (ERR_invalid_document_parameter);
   else
      /* parameter document is correct */
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
	   /* The table is already builded. One looks for the current entry of the table */
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

#ifdef __STDC__
int                 TtaIsDocumentModified (Document document)

#else  /* __STDC__ */
int                 TtaIsDocumentModified (document)
Document            document;

#endif /* __STDC__ */

{
   int                 modified;

   UserErrorCode = 0;
   /* verifies the parameter document */
   modified = 0;
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
   if (LoadedDocument[document - 1]->DocModified)
      modified = 1;
   return modified;
}

/*----------------------------------------------------------------------
   TtaGetDocumentAccessMode

   Returns the access mode for a document.

   Parameter:
   document: the document whose access mode is asked.
   Return value:
   0 if access mode is read only, 1 if access mode is read-write.

  ----------------------------------------------------------------------*/

#ifdef __STDC__
int                 TtaGetDocumentAccessMode (Document document)

#else  /* __STDC__ */
int                 TtaGetDocumentAccessMode (document)
Document            document;

#endif /* __STDC__ */

{
   int                 result;

   UserErrorCode = 0;
   result = 1;
   /* verifies the parameter document */
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
   if (LoadedDocument[document - 1]->DocReadOnly)
      result = 0;
   else
      result = 1;
   return result;
}


/*----------------------------------------------------------------------
   TtaGetDocumentBackUpInterval

   Returns backup interval for a document.

   Parameters:
   document: the document whose backup interval is asked.
   Return value:
   0 : the backup mechanism is disabled
   positive integer : number of typed characters which trigger an autamatic
   save of the document into a .BAK file.

  ----------------------------------------------------------------------*/

#ifdef __STDC__
int                 TtaGetDocumentBackUpInterval (Document document)

#else  /* __STDC__ */
int                 TtaGetDocumentBackUpInterval (document)
Document            document;

#endif /* __STDC__ */

{
   int                 result;

   UserErrorCode = 0;
   result = 0;
   /* verifies the parameter document */
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
      result = LoadedDocument[document - 1]->DocBackUpInterval;
   return result;
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

#ifdef __STDC__
int                 TtaGetNotificationMode (Document document)

#else  /* __STDC__ */
int                 TtaGetNotificationMode (document)
Document            document;

#endif /* __STDC__ */

{
   int                 result;

   UserErrorCode = 0;
   result = 0;
   /* verifies the parameter document */
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
   if (LoadedDocument[document - 1]->DocNotifyAll)
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

#ifdef __STDC__
void                TtaGetDocumentPath (char *buffer, int bufferLength)

#else  /* __STDC__ */
void                TtaGetDocumentPath (buffer, bufferLength)
char               *buffer;
int                 bufferLength;

#endif /* __STDC__ */

{

   UserErrorCode = 0;
   if (strlen (DocumentPath) >= bufferLength)
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

#ifdef __STDC__
void                TtaGetSchemaPath (char *buffer, int bufferLength)

#else  /* __STDC__ */
void                TtaGetSchemaPath (buffer, bufferLength)
char               *buffer;
int                 bufferLength;

#endif /* __STDC__ */

{

   UserErrorCode = 0;
   if (strlen (SchemaPath) >= bufferLength)
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

#ifdef __STDC__
Document            TtaGetDocumentOfSavedElements ()

#else  /* __STDC__ */
Document            TtaGetDocumentOfSavedElements ()

#endif				/* __STDC__ */

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

#ifdef __STDC__
PtrDocument         DocToPtr (Document document)

#else  /* __STDC__ */
PtrDocument         DocToPtr (document)
Document            document;

#endif /* __STDC__ */

{
   return LoadedDocument[document - 1];
}

/* end of module */
