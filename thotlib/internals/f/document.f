
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern void TraiteExceptionCreation ( PtrElement pEl, PtrDocument pDoc );
extern Document TtaNewDocument ( char *structureSchema, char *documentName );
extern Document TtaOpenDocument ( char *documentName, int accessMode );
extern void TtaSaveDocument ( Document document, char *documentName );
extern void TtaCloseDocument ( Document document );
extern void TtaRemoveDocument ( Document document );
extern void TtaSetDocumentPath ( char *path );
extern void TtaSetSchemaPath ( char *path );
extern SSchema TtaNewNature ( SSchema schema, char *natureName );
extern SSchema TtaNewSchemaExtension ( Document document, char *extensionName, char *presentationName );
extern void TtaRemoveSchemaExtension ( Document document, SSchema extension, int *removedElements, int *removedAttributes );
extern void TtaSetDocumentAccessMode ( Document document, int accessMode );
extern void TtaSetNotificationMode ( Document document, int notificationMode );
extern void TtaSetDocumentModified ( Document document );
extern void TtaSetDocumentUnmodified ( Document document );
extern SSchema TtaGetDocumentSSchema ( Document document );
extern char *TtaGetSSchemaName ( SSchema schema );
extern SSchema TtaGetSSchema ( char *name, Document document );
extern void TtaGiveSchemasOfDocument ( char *documentName, char *structureName, char *presentationName );
extern void TtaNextSchemaExtension ( Document document, SSchema *extension );
extern void TtaNextNature ( Document document, SSchema *nature );
extern int TtaIsDocumentModified ( Document document );
extern int TtaGetDocumentAccessMode ( Document document );
extern int TtaGetNotificationMode ( Document document );
extern void TtaGetDocumentPath ( char *buffer, int bufferLength );
extern void TtaGetSchemaPath ( char *buffer, int bufferLength );
extern PtrDocument DocToPtr ( Document document );
extern Document TtaGetDocumentOfSavedElements ( void );
extern Document PtrToDoc ( PtrDocument pDoc );

#else /* __STDC__ */

extern void TraiteExceptionCreation (/* PtrElement pEl, PtrDocument pDoc */);
extern Document TtaNewDocument (/* char *structureSchema, char *documentName */);
extern Document TtaOpenDocument (/* char *documentName, int accessMode */);
extern void TtaSaveDocument (/* Document document, char *documentName */);
extern void TtaCloseDocument (/* Document document */);
extern void TtaRemoveDocument (/* Document document */);
extern void TtaSetDocumentPath (/* char *path */);
extern void TtaSetSchemaPath (/* char *path */);
extern SSchema TtaNewNature (/* SSchema schema, char *natureName */);
extern SSchema TtaNewSchemaExtension (/* Document document, char *extensionName, char *presentationName */);
extern void TtaRemoveSchemaExtension (/* Document document, SSchema extension, int *removedElements, int *removedAttributes */);
extern void TtaSetDocumentAccessMode (/* Document document, int accessMode */);
extern void TtaSetNotificationMode (/* Document document, int notificationMode */);
extern void TtaSetDocumentModified (/* Document document */);
extern void TtaSetDocumentUnmodified (/* Document document */);
extern SSchema TtaGetDocumentSSchema (/* Document document */);
extern char *TtaGetSSchemaName (/* SSchema schema */);
extern SSchema TtaGetSSchema (/* char *name, Document document */);
extern void TtaGiveSchemasOfDocument (/* char *documentName, char *structureName, char *presentationName */);
extern void TtaNextSchemaExtension (/* Document document, SSchema *extension */);
extern void TtaNextNature (/* Document document, SSchema *nature */);
extern int TtaIsDocumentModified (/* Document document */);
extern int TtaGetDocumentAccessMode (/* Document document */);
extern int TtaGetNotificationMode (/* Document document */);
extern void TtaGetDocumentPath (/* char *buffer, int bufferLength */);
extern void TtaGetSchemaPath (/* char *buffer, int bufferLength */);
extern PtrDocument DocToPtr (/* Document document */);
extern Document TtaGetDocumentOfSavedElements (/* void */);
extern Document PtrToDoc (/* PtrDocument pDoc */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
