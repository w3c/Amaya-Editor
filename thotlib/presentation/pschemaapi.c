/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 *
 * Author: V. Quint (INRIA)
 *
 */
 
#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "application.h"
#include "pschema.h"

#undef THOT_EXPORT
#define THOT_EXPORT
#include "edit_tv.h"

#include "memory_f.h"
#include "schemas_f.h"
#include "compilmsg_f.h"

/*----------------------------------------------------------------------
   HandleOfPSchema      returns the handle that points the additional
   presentation schema "schema" of the nature "nature" from document doc.
   If nature is NULL, the additional Pschema is associated to the main
   structure schema of the document.
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static PtrHandlePSchema HandleOfPSchema (PSchema schema, Document doc, SSchema nature)

#else  /* __STDC__ */
static PtrHandlePSchema HandleOfPSchema (schema, doc, nature)
PSchema             schema;
Document            doc;
SSchema             nature;
#endif /* __STDC__ */

{
   PtrSSchema          pSS = NULL;
   PtrHandlePSchema    result, pHd;

   result = NULL;
   /* check parameter doc */
   if (doc < 1 || doc > MAX_DOCUMENTS)
      TtaError (ERR_invalid_document_parameter);
   else if (LoadedDocument[doc - 1] == NULL)
      TtaError (ERR_invalid_document_parameter);
   else
      /* parameter doc is OK */
     {
       if (nature == NULL ||
	   nature == (SSchema)(LoadedDocument[doc - 1]->DocSSchema))
	 pSS = LoadedDocument[doc - 1]->DocSSchema;
       else
	 pSS = (PtrSSchema) nature;
       if (pSS != NULL)
	 {
	   pHd = pSS->SsFirstPSchemaExtens;
	   while (result == NULL && pHd != NULL)
	     if (pHd->HdPSchema == (PtrPSchema) schema)
	       result = pHd;
	     else
	       pHd = pHd->HdNextPSchema;
	 }
     }
   return result;
}

/*----------------------------------------------------------------------
   TtaNewPSchema

   Creates a new (empty) presentation schema.

   Parameters:
   No parameter

   Return value:
   the new presentation schema.
  ----------------------------------------------------------------------*/
PSchema             TtaNewPSchema ()
{
   PtrPSchema          pSchPres;

   pSchPres = NULL;
   GetSchPres (&pSchPres);
   return ((PSchema) pSchPres);
}


/*----------------------------------------------------------------------
   TtaUnlinkPSchema

   Unlinks a presentation schema from a document or a nature in a 
   document.

   Parameters:
   schema: the presentation schema to be unlinked.
   document: the document to which that presentation schema is related.
   nature: the structure schema of a nature of the document, NULL if schema
           is relative to the main structure schema of the document.
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                TtaUnlinkPSchema (PSchema schema, Document document, SSchema nature)
#else  /* __STDC__ */
void                TtaUnlinkPSchema (schema, document, nature)
PSchema             schema;
Document            document;
SSchema             nature;
#endif /* __STDC__ */

{
   PtrHandlePSchema    pHd;
   PtrSSchema	       pSchS;

   if (!LoadedDocument[document - 1])
     return;
   pHd = HandleOfPSchema (schema, document, nature);
   if (pHd != NULL)
     {
       if (nature == NULL)
	 pSchS = LoadedDocument[document - 1]->DocSSchema;
       else
	 pSchS = (PtrSSchema) nature;
       /* update links between schema contexts */
       if (pHd->HdPrevPSchema == NULL)
	   pSchS->SsFirstPSchemaExtens = pHd->HdNextPSchema;
	else
	   pHd->HdPrevPSchema->HdNextPSchema = pHd->HdNextPSchema;

	if (pHd->HdNextPSchema != NULL)
	   pHd->HdNextPSchema->HdPrevPSchema = pHd->HdPrevPSchema;
	FreeHandleSchPres (pHd);
     }
}


/*----------------------------------------------------------------------
   TtaRemovePSchema

   Removes a presentation schema from a document or a nature in a 
   document and destroys that schema if it is not used by other documents.

   Parameters:
   schema: the presentation schema to be deleted.
   document: the document to which that presentation schema is related.
   nature: the structure schema of a nature of the document, NULL if schema
           is relative to the main structure schema of the document.
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                TtaRemovePSchema (PSchema schema, Document document, SSchema nature)
#else  /* __STDC__ */
void                TtaRemovePSchema (schema, document, nature)
PSchema             schema;
Document            document;
SSchema             nature;
#endif /* __STDC__ */

{
   PtrHandlePSchema    pHd;
   PtrPSchema          pSchP;
   PtrSSchema	       pSchS;

   if (!LoadedDocument[document - 1])
     return;
   if (nature == NULL)
     pSchS = LoadedDocument[document - 1]->DocSSchema;
   else
     pSchS = (PtrSSchema) nature;
   pSchP = (PtrPSchema) schema;
   pHd = HandleOfPSchema (schema, document, nature);
   if (pHd != NULL)
     {
       if (pHd->HdPrevPSchema == NULL)
	   pSchS->SsFirstPSchemaExtens = pHd->HdNextPSchema;
	else
	   pHd->HdPrevPSchema->HdNextPSchema = pHd->HdNextPSchema;
	if (pHd->HdNextPSchema != NULL)
	   pHd->HdNextPSchema->HdPrevPSchema = pHd->HdPrevPSchema;
	FreeHandleSchPres (pHd);
     }
   /* in any case free the Pschema */
   if (pSchP->PsStructCode > 0)
     {
       pSchP->PsStructCode--;	/* number of documents using this schema */
       if (pSchP->PsStructCode == 0)
	 /* this presentation schema is no longer used */
	 FreePresentationSchema (pSchP, pSchS);
     }
}


/*----------------------------------------------------------------------
   TtaAddPSchema

   Associates an additional presentation schema with a document. All additional
   presentation schemas for a given document are stored in an ordered list, in
   increasing priority order. TtaAddPSchema inserts the new schema in that list
   immediately before (or after, according to parameter "before") another schema
   which is already part of the list (parameter "oldSchema").

   Parameters:
   schema: the presentation schema to be added
   oldSchema: a presentation schema that is already associated with the
   document. NULL when adding the first additional presentation schema
   for the document.
   before: if TRUE, the new presentation schema is inserted just before
   oldSchema, else it is inserted just after. Meaningless if
   oldSchema is NULL.
   document: the document to which the presentation schema is added.

  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaAddPSchema (PSchema schema, PSchema oldSchema, ThotBool before, Document document, SSchema nature)
#else  /* __STDC__ */
void                TtaAddPSchema (schema, oldSchema, before, document, nature)
PSchema             schema;
PSchema             oldSchema;
ThotBool            before;
Document            document;
SSchema		    nature;
#endif /* __STDC__ */

{
   PtrHandlePSchema    oldHd, newHd;
   ThotBool            ok;
   PtrSSchema	       pSchS;

   ok = FALSE;
   if (nature == NULL)
     pSchS = LoadedDocument[document - 1]->DocSSchema;
   else
     pSchS = (PtrSSchema) nature;
   oldHd = NULL;
   if (oldSchema != NULL)
     {
	oldHd = HandleOfPSchema (oldSchema, document, nature);
	if (oldHd != NULL)
	   ok = TRUE;
     }
   else if (document < 1 || document > MAX_DOCUMENTS)
      TtaError (ERR_invalid_document_parameter);
   else if (LoadedDocument[document - 1] == NULL)
      TtaError (ERR_invalid_document_parameter);
   else
     {
       ok = TRUE;
       oldHd = pSchS->SsFirstPSchemaExtens;
     }

   if (ok)
     {
	GetHandleSchPres (&newHd);
	newHd->HdPSchema = (PtrPSchema) schema;
	if (oldHd == NULL)
	   pSchS->SsFirstPSchemaExtens = newHd;
	else if (before)
	  {
	     newHd->HdNextPSchema = oldHd;
	     newHd->HdPrevPSchema = oldHd->HdPrevPSchema;
	     oldHd->HdPrevPSchema = newHd;
	     if (newHd->HdPrevPSchema)
		newHd->HdPrevPSchema->HdNextPSchema = newHd;
	     else
		pSchS->SsFirstPSchemaExtens = newHd;
	  }
	else
	  {
	     newHd->HdNextPSchema = oldHd->HdNextPSchema;
	     newHd->HdPrevPSchema = oldHd;
	     oldHd->HdNextPSchema = newHd;
	     if (newHd->HdNextPSchema)
		newHd->HdNextPSchema->HdPrevPSchema = newHd;
	  }
	/* number of documents using this schema */
	((PtrPSchema) schema)->PsStructCode++;
	/* name of associated structure schema */
	ustrncpy (((PtrPSchema) schema)->PsStructName, pSchS->SsName,
		 MAX_NAME_LENGTH);
     }
}


/*----------------------------------------------------------------------
   TtaGetFirstPSchema

   Returns the first (i.e. lowest priority) additional presentation schema
   related with a document.
   Returns NULL if no additional presentation schema is related with the document.

   Parameter:
   document: the document of interest.

  ----------------------------------------------------------------------*/

#ifdef __STDC__
PSchema             TtaGetFirstPSchema (Document document, SSchema nature)

#else  /* __STDC__ */
PSchema             TtaGetFirstPSchema (document, nature)
Document            document;
SSchema		    nature;

#endif /* __STDC__ */

{
   PtrPSchema          pSchPres;
   PtrSSchema	       pSchS;

   pSchPres = NULL;
   if (document < 1 || document > MAX_DOCUMENTS)
      TtaError (ERR_invalid_document_parameter);
   else if (LoadedDocument[document - 1] == NULL)
      TtaError (ERR_invalid_document_parameter);
   else
     {
       /* parameter doc is OK */
       if (nature == NULL)
	 pSchS = LoadedDocument[document - 1]->DocSSchema;
       else
	 pSchS = (PtrSSchema) nature;
       if (pSchS->SsFirstPSchemaExtens != NULL)
       pSchPres = pSchS->SsFirstPSchemaExtens->HdPSchema;
     }
   return ((PSchema) pSchPres);
}


/*----------------------------------------------------------------------
   TtaNextPSchema

   Gets the next additional presentation schema, in the increasing order of
   priority.

   Parameter:
   schema: the schema whose successor is asked.
   document: the document of interest.

   Return parameter:
   schema: the next schema, or NULL if there is no next schema.

  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                TtaNextPSchema (PSchema * schema, Document document, SSchema nature)

#else  /* __STDC__ */
void                TtaNextPSchema (schema, document, nature)
PSchema            *schema;
Document            document;
SSchema		    nature;

#endif /* __STDC__ */

{
   PtrPSchema          pSchPres;
   PtrHandlePSchema    pHd;

   pSchPres = NULL;
   pHd = HandleOfPSchema (*schema, document, nature);
   if (pHd != NULL)
      if (pHd->HdNextPSchema != NULL)
	 pSchPres = pHd->HdNextPSchema->HdPSchema;
   *schema = (PSchema) pSchPres;
}

/* end of module */
