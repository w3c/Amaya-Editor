
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef _DOCUMENT_H_
#define _DOCUMENT_H_

/* document */
typedef int         Document;

/* structure schema */
typedef int        *SSchema;

#include "typebase.h"

#ifndef __CEXTRACT__
#ifdef __STDC__

extern Document     TtaNewDocument (char *structureSchema, char *documentName);
extern Document     TtaOpenDocument (char *documentName, int accessMode);
extern void         TtaSaveDocument (Document document, char *documentName);
extern void         TtaExportDocument (Document document, char *fileName, char *TSchemaName);
extern void         TtaCloseDocument (Document document);
extern void         TtaRemoveDocument (Document document);
extern void         TtaSetDocumentPath (char *path);
extern boolean      TtaIsInDocumentPath (char *aDirectory);
extern void         TtaAppendDocumentPath (char *aDirectory);
extern void         TtaSetSchemaPath (char *path);
extern SSchema      TtaNewNature (SSchema schema, char *natureName, char *presentationName);
extern SSchema      TtaNewSchemaExtension (Document document, char *extensionName, char *presentationName);
extern SSchema      TtaGetSchemaExtension (Document document, char *NomExtension);
extern void         TtaRemoveSchemaExtension (Document document, SSchema extension, int *removedElements, int *removedAttributes);
extern void         TtaSetPSchema (Document document, char *presentationName);
extern void         TtaSetDocumentDirectory (Document document, char *directory);
extern void         TtaSetDocumentName (Document document, char *documentName);
extern void         TtaSetDocumentAccessMode (Document document, int accessMode);
extern void         TtaSetDocumentBackUpInterval (Document document, int interval);
extern void         TtaSetNotificationMode (Document document, int notificationMode);
extern void         TtaSetDocumentModified (Document document);
extern void         TtaSetDocumentUnmodified (Document document);
extern char        *TtaGetDocumentName (Document document);
extern Document     TtaGetDocumentFromName (char *documentName);
extern void         TtaGetDocumentDirectory (Document document, char *buffer, int bufferLength);
extern SSchema      TtaGetDocumentSSchema (Document document);
extern char        *TtaGetSSchemaName (SSchema schema);
extern char        *TtaGetPSchemaName (SSchema schema);
extern SSchema      TtaGetSSchema (char *name, Document document);
extern int          TtaSameSSchemas (SSchema schema1, SSchema schema2);
extern void         TtaGiveSchemasOfDocument (char *documentName, char *structureName, char *presentationName);
extern void         TtaNextSchemaExtension (Document document, SSchema * extension);
extern void         TtaNextNature (Document document, SSchema * nature);
extern int          TtaIsDocumentModified (Document document);
extern int          TtaGetDocumentAccessMode (Document document);
extern int          TtaGetDocumentBackUpInterval (Document document);
extern int          TtaGetNotificationMode (Document document);
extern void         TtaGetDocumentPath (char *buffer, int bufferLength);
extern void         TtaGetSchemaPath (char *buffer, int bufferLength);
extern Document     TtaGetDocumentOfSavedElements ();

#else  /* __STDC__ */

extern Document     TtaNewDocument ( /* char *structureSchema, char *documentName */ );
extern Document     TtaOpenDocument ( /* char *documentName, int accessMode */ );
extern void         TtaSaveDocument ( /* Document document, char *documentName */ );
extern void         TtaExportDocument ( /* Document document, char *fileName, char* TSchemaName */ );
extern void         TtaCloseDocument ( /* Document document */ );
extern void         TtaRemoveDocument ( /* Document document */ );
extern void         TtaSetDocumentPath ( /* char *path */ );
extern boolean      TtaIsInDocumentPath ( /*char *aDirectory */ );
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
extern char        *TtaGetDocumentName ( /* Document document */ );
extern Document     TtaGetDocumentFromName ( /* char *documentName */ );
extern void         TtaGetDocumentDirectory ( /* Document document, char *buffer, int bufferLength */ );
extern SSchema      TtaGetDocumentSSchema ( /* Document document */ );
extern char        *TtaGetSSchemaName ( /* SSchema schema */ );
extern char        *TtaGetPSchemaName ( /* SSchema schema */ );
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

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */

#endif
