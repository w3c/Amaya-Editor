
/* -- Copyright (c) 1996 Inria All rights reserved. -- */

#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "application.h"
#include "pschema.h"

#undef EXPORT
#define EXPORT
#include "edit.var"

#include "memory_f.h"
#include "schemas_f.h"
#include "compilmsg_f.h"

/* -------------------------------------------------------------------
   HandleOfPSchema      returns the handle that points the additional
   presentation schema "schema" from document doc.
   ---------------------------------------------------------------------- */

#ifdef __STDC__
static PtrHandlePSchema HandleOfPSchema (PSchema schema, Document doc)

#else  /* __STDC__ */
static PtrHandlePSchema HandleOfPSchema (schema, doc)
PSchema             schema;
Document            doc;

#endif /* __STDC__ */

{
   PtrSSchema        pSS;
   PtrHandlePSchema    result, pHd;

   result = NULL;
   /* check parameter doc */
   if (doc < 1 || doc > MAX_DOCUMENTS)
      TtaError (ERR_invalid_document_parameter);
   else if (TabDocuments[doc - 1] == NULL)
      TtaError (ERR_invalid_document_parameter);
   else
      /* parameter doc is OK */
     {
	pSS = TabDocuments[doc - 1]->DocSSchema;
	if (pSS != NULL)
	  {
	     pHd = pSS->SsFirstPSchemaExtens;
	     while (result == NULL && pHd != NULL)
		if (pHd->HdPSchema == (PtrPSchema) schema)
		   result = pHd;
		else
		   pHd = pHd->HdNextPSchema;
	  }
	if (result == NULL)
	   TtaError (ERR_invalid_parameter);
     }
   return result;
}

/* ----------------------------------------------------------------------
   TtaNewPSchema

   Creates a new (empty) presentation schema.

   Parameters:
   No parameter

   Return value:
   the new presentation schema.
   ---------------------------------------------------------------------- */

#ifdef __STDC__
PSchema             TtaNewPSchema ()

#else  /* __STDC__ */
PSchema             TtaNewPSchema ()

#endif				/* __STDC__ */

{
   PtrPSchema          pSchPres;

   pSchPres = NULL;
   GetSchPres (&pSchPres);
   return ((PSchema) pSchPres);
}


/* ----------------------------------------------------------------------
   TtaRemovePSchema

   Removes a presentation schema from a document and destroys that schema
   if it is not used by other documents.

   Parameters:
   schema: the presentation schema to be deleted.
   document: the document to which that presentation schema is related.

   ---------------------------------------------------------------------- */

#ifdef __STDC__
void                TtaRemovePSchema (PSchema schema, Document document)

#else  /* __STDC__ */
void                TtaRemovePSchema (schema, document)
PSchema             schema;
Document            document;

#endif /* __STDC__ */

{
   PtrHandlePSchema    pHd;
   PtrPSchema          pSchP;

   pHd = HandleOfPSchema (schema, document);
   if (pHd != NULL)
     {
	if (pHd->HdPrevPSchema == NULL)
	   TabDocuments[document - 1]->DocSSchema->SsFirstPSchemaExtens = pHd->HdNextPSchema;
	else
	   pHd->HdPrevPSchema->HdNextPSchema = pHd->HdNextPSchema;
	if (pHd->HdNextPSchema != NULL)
	   pHd->HdNextPSchema->HdPrevPSchema = pHd->HdPrevPSchema;
	pSchP = pHd->HdPSchema;
	pSchP->PsStructCode--;	/* number of documents using this schema */
	if (pSchP->PsStructCode == 0)
	   /* this presentation schema is no longer used */
	   SupprSchPrs (pSchP, TabDocuments[document - 1]->DocSSchema);
	FreeHandleSchPres (pHd);
     }
}


/* ----------------------------------------------------------------------
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

   ---------------------------------------------------------------------- */

#ifdef __STDC__
void                TtaAddPSchema (PSchema schema, PSchema oldSchema, boolean before, Document document)

#else  /* __STDC__ */
void                TtaAddPSchema (schema, oldSchema, before, document)
PSchema             schema;
PSchema             oldSchema;
boolean             before;
Document            document;

#endif /* __STDC__ */

{
   PtrHandlePSchema    oldHd, newHd;
   boolean             ok;

   ok = FALSE;
   if (oldSchema != NULL)
     {
	oldHd = HandleOfPSchema (oldSchema, document);
	if (oldHd != NULL)
	   ok = TRUE;
     }
   else if (document < 1 || document > MAX_DOCUMENTS)
      TtaError (ERR_invalid_document_parameter);
   else if (TabDocuments[document - 1] == NULL)
      TtaError (ERR_invalid_document_parameter);
   else
      /* parameter doc is OK */
   if (TabDocuments[document - 1]->DocSSchema->SsFirstPSchemaExtens != NULL)
      TtaError (ERR_invalid_parameter);
   else
     {
	ok = TRUE;
	oldHd = NULL;
     }
   if (ok)
     {
	GetHandleSchPres (&newHd);
	newHd->HdPSchema = (PtrPSchema) schema;
	if (oldHd == NULL)
	   TabDocuments[document - 1]->DocSSchema->SsFirstPSchemaExtens = newHd;
	else if (before)
	  {
	     newHd->HdNextPSchema = oldHd;
	     newHd->HdPrevPSchema = oldHd->HdPrevPSchema;
	     oldHd->HdPrevPSchema = newHd;
	     if (newHd->HdPrevPSchema)
		newHd->HdPrevPSchema->HdNextPSchema = newHd;
	     else
		TabDocuments[document - 1]->DocSSchema->SsFirstPSchemaExtens = newHd;
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
     }
}


/* ----------------------------------------------------------------------
   TtaGetFirstPSchema

   Returns the first (i.e. lowest priority) additional presentation schema
   related with a document.
   Returns NULL if no additional presentation schema is related with the document.

   Parameter:
   document: the document of interest.

   ---------------------------------------------------------------------- */

#ifdef __STDC__
PSchema             TtaGetFirstPSchema (Document document)

#else  /* __STDC__ */
PSchema             TtaGetFirstPSchema (document)
Document            document;

#endif /* __STDC__ */

{
   PtrPSchema          pSchPres;

   pSchPres = NULL;
   if (document < 1 || document > MAX_DOCUMENTS)
      TtaError (ERR_invalid_document_parameter);
   else if (TabDocuments[document - 1] == NULL)
      TtaError (ERR_invalid_document_parameter);
   else
      /* parameter doc is OK */
   if (TabDocuments[document - 1]->DocSSchema->SsFirstPSchemaExtens != NULL)
      pSchPres = TabDocuments[document - 1]->DocSSchema->SsFirstPSchemaExtens->HdPSchema;
   return ((PSchema) pSchPres);
}


/* ----------------------------------------------------------------------
   TtaNextPSchema

   Gets the next additional presentation schema, in the increasing order of
   priority.

   Parameter:
   schema: the schema whose successor is asked.
   document: the document of interest.

   Return parameter:
   schema: the next schema, or NULL if there is no next schema.

   ---------------------------------------------------------------------- */

#ifdef __STDC__
void                TtaNextPSchema (PSchema * schema, Document document)

#else  /* __STDC__ */
void                TtaNextPSchema (schema, document)
PSchema            *schema;
Document            document;

#endif /* __STDC__ */

{
   PtrPSchema          pSchPres;
   PtrHandlePSchema    pHd;

   pSchPres = NULL;
   pHd = HandleOfPSchema (*schema, document);
   if (pHd != NULL)
      if (pHd->HdNextPSchema != NULL)
	 pSchPres = pHd->HdNextPSchema->HdPSchema;
   *schema = (PSchema) pSchPres;
}

/* end of module */
