/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * Warning:
 * This module is part of the Thot library, which was originally
 * developed in French. That's why some comments are still in
 * French, but their translation is in progress and the full module
 * will be available in English in the next release.
 * 
 */
 
#ifndef _DOCUMENT_H_
#define _DOCUMENT_H_

/* document */
typedef int         Document;

/* structure schema */
typedef int        *SSchema;

#include "typebase.h"

#define		CONFIG_UNKNOWN_TYPE		0
#define		CONFIG_DOCUMENT_STRUCT		1
#define		CONFIG_NATURE_STRUCT		2
#define		CONFIG_EXTENSION_STRUCT		3
#define		CONFIG_EXCLUSION		4

#ifndef __CEXTRACT__
#ifdef __STDC__


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
extern Document     TtaNewDocument (STRING structureSchema, STRING documentName);

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
extern Document     TtaOpenDocument (STRING documentName, int accessMode);

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
extern void         TtaSaveDocument (Document document, STRING documentName);

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
   
   Return: TRUE if done correctly
  ----------------------------------------------------------------------*/
extern ThotBool     TtaExportDocument (Document document, STRING fileName, STRING TSchemaName);

/*----------------------------------------------------------------------
   TtaExportDocumentWithNewLineNumbers

   Saves a whole document into a file in a particular format. The output
   format is specified by a translation schema. The document is not closed
   by the function and it can still be accessed by the application program.
   Line numbers recorded in document elements are updated according to the
   generated file.

   Parameters:
   document: the document to be exported.
   fileName: name of the file in which the document must be saved,
   including the directory name.
   TSchemaName: name of the translation schema to be used. The directory
   name must not be specified in parameter TSchemaName. See
   function TtaSetSchemaPath.

   Return: TRUE if done correctly
  ----------------------------------------------------------------------*/
extern ThotBool     TtaExportDocumentWithNewLineNumbers (Document document,
							 STRING fileName,
							 STRING TSchemaName);

/*----------------------------------------------------------------------
   TtaCloseDocument

   Closes a document that is no longer needed and releases all ressources
   allocated to the document. This function does not save the document.

   Parameter:
   document: the document to be closed.

  ----------------------------------------------------------------------*/
extern void         TtaCloseDocument (Document document);

/*----------------------------------------------------------------------
   TtaRemoveDocument

   Closes a document, releases all ressources allocated to that document,
   removes all files related to the document and updates all links connecting
   the removed document with other documents.

   Parameter:
   document: the document to be removed.
  ----------------------------------------------------------------------*/
extern void         TtaRemoveDocument (Document document);

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
extern void         TtaSetDocumentPath (STRING path);

/*----------------------------------------------------------------------
   TtaCheckPath

   Checks if all directories in a path can be accessed.

   Parameter:
   path: the path to be checked

   Return value:
   TRUE if all directories are OK, FALSE if at least one cannot be
   accessed.

  ----------------------------------------------------------------------*/
extern ThotBool     TtaCheckPath (char* path );

/*----------------------------------------------------------------------
   TtaIsInDocumentPath

   returns TRUE if the directory is in the list of document directories.

   Parameter:
   directory: the new directory name.

  ----------------------------------------------------------------------*/
extern ThotBool     TtaIsInDocumentPath (char* aDirectory);

/*----------------------------------------------------------------------
   TtaAppendDocumentPath

   Appends a new directory in the list of document directories if this
   directory is not already in the list and if the directory exists.

   Parameter:
   directory: the new directory name.

  ----------------------------------------------------------------------*/
extern void         TtaAppendDocumentPath (STRING aDirectory);

/*----------------------------------------------------------------------
   TtaSetSchemaPath

   Sets a new list of schema directories. This list replaces the existing one.
   It is used for searching schemas.

   Parameter:
   path: the directory list, where directory names are separated by
   the character PATH_SEP.

  ----------------------------------------------------------------------*/
extern void         TtaSetSchemaPath (STRING path);

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
extern SSchema      TtaNewNature (SSchema schema, STRING natureName, STRING presentationName);

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
extern SSchema      TtaNewSchemaExtension (Document document, STRING extensionName, STRING presentationName);

/*----------------------------------------------------------------------
   TtaGetSchemaExtension

   Returns the structure schema extension that is associated with a given
   document and that has a given name.

   Parameters:
   document:
   NomExtension:

   Return value:
   NULL if the extension is not associated with the document.

  ----------------------------------------------------------------------*/
extern SSchema      TtaGetSchemaExtension (Document document, STRING NomExtension);

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
extern void         TtaRemoveSchemaExtension (Document document, SSchema extension, int *removedElements, int *removedAttributes);

/*----------------------------------------------------------------------
   TtaSetPSchema

   Sets or changes the main presentation schema of a document. The document
   must be open, but no view must be open for that document.

   Parameters:
   document: the document.
   presentationName: Name of the presentation schema to be associated
   with the document.

  ----------------------------------------------------------------------*/
extern void         TtaSetPSchema (Document document, STRING presentationName);

/*----------------------------------------------------------------------
   TtaSetDocumentDirectory

   Sets the directory to which the document is supposed to be saved.

   Parameters:
   document: the document whose directory is set.
   directory: new document directory.

  ----------------------------------------------------------------------*/
extern void         TtaSetDocumentDirectory (Document document, STRING directory);

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
extern void         TtaSetDocumentName (Document document, STRING documentName);

/*----------------------------------------------------------------------
   TtaSetDocumentAccessMode

   Sets the access mode for a document.

   Parameters:
   document: the document whose access mode is changed.
   accessMode: 0 = read only, 1 = read-write.

  ----------------------------------------------------------------------*/
extern void         TtaSetDocumentAccessMode (Document document, int accessMode);

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
extern void         TtaSetDocumentBackUpInterval (Document document, int interval);

/*----------------------------------------------------------------------
   TtaSetNotificationMode

   Sets the ECF notification mode for a document.
   Parameters:
   document: the document whose notification mode is changed.
   notificationMode: 0 = only roots of created and deleted subtrees must
   be notified, 1 = all elements of created and deleted subtrees must
   be notified.

  ----------------------------------------------------------------------*/
extern void         TtaSetNotificationMode (Document document, int notificationMode);

/*----------------------------------------------------------------------
   TtaSetDocumentModified

   Notifies the tool kit that a document has been modified by the application.
   As a consequence, the user will be asked to save the document when closing it.

   Parameter:
   document: the document.

  ----------------------------------------------------------------------*/
extern void         TtaSetDocumentModified (Document document);

/*----------------------------------------------------------------------
   TtaSetDocumentUnmodified

   Notifies the tool kit that a document must be considered as not modified
   by the application or by the user. As a consequence, if no further modification
   is made to that document, the user will not be asked to save the document
   when closing it.

   Parameter:
   document: the document.

  ----------------------------------------------------------------------*/
extern void         TtaSetDocumentUnmodified (Document document);

/*----------------------------------------------------------------------
   TtaGetDocumentName

   Returns the name of a document.

   Parameter:
   document: the document whose name is asked.

   Return value:
   name of that document.

  ----------------------------------------------------------------------*/
extern STRING       TtaGetDocumentName (Document document);

/*----------------------------------------------------------------------
   TtaGetDocumentFromName

   Returns the document having a given name.

   Parameter:
   documentName: the document name.

   Return value:
   the document having that name.

  ----------------------------------------------------------------------*/
extern Document     TtaGetDocumentFromName (STRING documentName);

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
extern void         TtaGetDocumentDirectory (Document document, STRING buffer, int bufferLength);

/*----------------------------------------------------------------------
   TtaGetDocumentSSchema

   Returns the main structure schema of a document.

   Parameter:
   document: the document for which the structure schema is asked.

   Return value:
   the structure schema of that document.

  ----------------------------------------------------------------------*/
extern SSchema      TtaGetDocumentSSchema (Document document);

/*----------------------------------------------------------------------
   TtaGetSSchemaName

   Returns the name of a structure schema.

   Parameter:
   schema: the structure schema of interest.

   Return value:
   name of that structure schema.

  ----------------------------------------------------------------------*/
extern STRING        TtaGetSSchemaName (SSchema schema);

/*----------------------------------------------------------------------
   TtaGetPSchemaName

   Returns the name of the presentation schema currently associated
   with a given structure schema.

   Parameter:
   schema: the structure schema of interest.

   Return value:
   name of the associated presentation schema.

  ----------------------------------------------------------------------*/
extern STRING       TtaGetPSchemaName (SSchema schema);

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
extern SSchema      TtaGetSSchema (STRING name, Document document);

/*----------------------------------------------------------------------
   TtaSameSSchemas

   Compares two structure schemas.

   Parameters:
   schema1: first structure schema.
   schema2: second structure schema.

   Return value:
   0 if both schemas are different, 1 if they are identical.

  ----------------------------------------------------------------------*/
extern int          TtaSameSSchemas (SSchema schema1, SSchema schema2);

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
extern void         TtaGiveSchemasOfDocument (STRING documentName, /*OUT*/ STRING structureName, /*OUT*/ STRING presentationName);

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
extern void         TtaNextSchemaExtension (Document document, /*INOUT*/ SSchema * extension);

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
extern void         TtaNextNature (Document document, /*INOUT*/ SSchema * nature);

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
extern int          TtaIsDocumentModified (Document document);

/*----------------------------------------------------------------------
   TtaGetDocumentAccessMode

   Returns the access mode for a document.

   Parameter:
   document: the document whose access mode is asked.
   Return value:
   0 if access mode is read only, 1 if access mode is read-write.

  ----------------------------------------------------------------------*/
extern int          TtaGetDocumentAccessMode (Document document);

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
extern int          TtaGetDocumentBackUpInterval (Document document);

/*----------------------------------------------------------------------
   TtaGetNotificationMode

   Returns the ECF notification mode for a document.

   Parameters:
   document: the document whose notification mode is asked.
   Return value:
   0 = if only roots of created and deleted subtrees must be notified,
   1 = all elements of created and deleted subtrees must be notified.

  ----------------------------------------------------------------------*/
extern int          TtaGetNotificationMode (Document document);

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
extern void         TtaGetDocumentPath (/*OUT*/ STRING buffer, int bufferLength);

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
extern void         TtaGetSchemaPath (/*OUT*/ STRING buffer, int bufferLength);

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
extern Document     TtaGetDocumentOfSavedElements ();


/*----------------------------------------------------------------------
   TtaConfigReadConfigFiles (re)initialise les tables des schemas de
   structure (documents, natures et extensions) qui ont    
   des fichiers de langue dans les directories de schemas. 
  ----------------------------------------------------------------------*/
extern void TtaConfigReadConfigFiles ( char* aSchemaPath );

/*----------------------------------------------------------------------
   TtaConfigSSchemaExternalName retourne dans nameUser le nom     
   externe, dans la langue de l'utilisateur, du schema de          
   structure dont le nom interne est nameSchema.                    
   Typ indique s'il s'agit d'un schema de document (1), de         
   nature (2) ou d'extension (3).                                  
  ----------------------------------------------------------------------*/
extern void TtaConfigSSchemaExternalName (/*OUT*/ char* nameUser, char* nameSchema, int Typ );

#else  /* __STDC__ */

extern Document     TtaNewDocument ( /* char *structureSchema, char *documentName */ );
extern Document     TtaOpenDocument ( /* char *documentName, int accessMode */ );
extern void         TtaSaveDocument ( /* Document document, char *documentName */ );
extern ThotBool     TtaExportDocument ( /* Document document, char *fileName, char* TSchemaName */ );
extern ThotBool     TtaExportDocumentWithNewLineNumbers ( /* Document document, char *fileName, char* TSchemaName */ );
extern void         TtaCloseDocument ( /* Document document */ );
extern void         TtaRemoveDocument ( /* Document document */ );
extern void         TtaSetDocumentPath ( /* char *path */ );
extern ThotBool     TtaCheckPath (/* char *path */);
extern ThotBool     TtaIsInDocumentPath ( /*char *aDirectory */ );
extern void         TtaAppendDocumentPath ( /*char *aDirectory */ );
extern void         TtaSetSchemaPath ( /* char *path */ );
extern SSchema      TtaNewNature ( /* SSchema schema, char *natureName, char *presentationName */ );
extern SSchema      TtaNewSchemaExtension ( /* Document document, char *extensionName, char *presentationName */ );
extern SSchema      TtaGetSchemaExtension ( /*Document document, char *NomExtension */ );
extern void         TtaRemoveSchemaExtension ( /* Document document, SSchema extension, int *removedElements, int *removedAttributes */ );
extern void         TtaSetPSchema ( /* Document document, char *presentationName */ );
extern void         TtaSetDocumentDirectory ( /* Document document, char *directory */ );
extern void         TtaSetDocumentName ( /* Document document, char *documentName */ );
extern void         TtaSetDocumentAccessMode ( /* Document document, int accessMode */ );
extern void         TtaSetDocumentBackUpInterval ( /* Document document, int interval */ );
extern void         TtaSetNotificationMode ( /* Document document, int notificationMode */ );
extern void         TtaSetDocumentModified ( /* Document document */ );
extern void         TtaSetDocumentUnmodified ( /* Document document */ );
extern STRING       TtaGetDocumentName ( /* Document document */ );
extern Document     TtaGetDocumentFromName ( /* char *documentName */ );
extern void         TtaGetDocumentDirectory ( /* Document document, char *buffer, int bufferLength */ );
extern SSchema      TtaGetDocumentSSchema ( /* Document document */ );
extern STRING       TtaGetSSchemaName ( /* SSchema schema */ );
extern STRING       TtaGetPSchemaName ( /* SSchema schema */ );
extern SSchema      TtaGetSSchema ( /* char *name, Document document */ );
extern int          TtaSameSSchemas ( /* SSchema schema1, SSchema schema2 */ );
extern void         TtaGiveSchemasOfDocument ( /* char *documentName, char *structureName, char *presentationName */ );
extern void         TtaNextSchemaExtension ( /* Document document, SSchema *extension */ );
extern void         TtaNextNature ( /* Document document, SSchema *nature */ );
extern int          TtaIsDocumentModified ( /* Document document */ );
extern int          TtaGetDocumentAccessMode ( /* Document document */ );
extern int          TtaGetDocumentBackUpInterval ( /* Document document */ );
extern int          TtaGetNotificationMode ( /* Document document */ );
extern void         TtaGetDocumentPath ( /* char *buffer, int bufferLength */ );
extern void         TtaGetSchemaPath ( /* char *buffer, int bufferLength */ );
extern Document     TtaGetDocumentOfSavedElements ();

extern void TtaConfigReadConfigFiles (/* char *aSchemaPath */);
extern void TtaConfigSSchemaExternalName (/* char *nameUser,
                                             char *nameSchema,
                                             int Typ */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */

#endif
