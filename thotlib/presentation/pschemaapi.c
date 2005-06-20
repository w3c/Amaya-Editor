/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2005
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 *
 * Author: V. Quint (INRIA)
 *
 */
#include "thot_gui.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "application.h"
#include "pschema.h"

#undef THOT_EXPORT
#define THOT_EXPORT extern /* to avoid redefinition */
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
static PtrHandlePSchema HandleOfPSchema (PSchema schema, Document doc,
					 SSchema nature)
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
	   pHd = FirstPSchemaExtension (pSS, LoadedDocument[doc - 1], NULL);
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

   Creates a new (empty) presentation schema for structure schema nature

   Parameters:
   nature: the structure schema for which a presentation schema is created
   userStyleSheet: indicates whether the presentation schema is created
                   for a user stylesheet (TRUE) of for an author stylesheet
                   (FALSE).

   Return value:
   the new presentation schema.
  ----------------------------------------------------------------------*/
PSchema TtaNewPSchema (SSchema nature, ThotBool userStyleSheet)
{
   PtrPSchema          pSchPres;
   PtrSSchema          pSS;
   int                 size;

   pSchPres = NULL;
   if (nature)
     {
       GetSchPres (&pSchPres);
       if (pSchPres)
	 {
	   if (userStyleSheet)
	     pSchPres->PsOrigin = User;
	   else
	     pSchPres->PsOrigin = Author;
	   pSS = (PtrSSchema) nature;

           pSchPres->PsSSchema = pSS;
	   size = pSS->SsAttrTableSize * sizeof (PtrAttributePres);
	   pSchPres->PsAttrPRule =  (AttrPresTable*) malloc (size);
	   if (pSchPres->PsAttrPRule)
	     memset (pSchPres->PsAttrPRule, 0, size);

	   size = pSS->SsAttrTableSize * sizeof (int);
	   pSchPres->PsNAttrPRule = (NumberTable*) malloc (size);
	   if (pSchPres->PsNAttrPRule)
	     memset (pSchPres->PsNAttrPRule, 0, size);

	   size = pSS->SsRuleTableSize * sizeof (PtrPRule);
	   pSchPres->PsElemPRule = (PtrPRuleTable*) malloc (size);
	   if (pSchPres->PsElemPRule)
	     memset (pSchPres->PsElemPRule, 0 , size);
	   pSchPres->PsNElemPRule = pSS->SsNRules;

	   size = pSS->SsAttrTableSize * sizeof (int);
	   pSchPres->PsNHeirElems = (NumberTable*) malloc (size);
	   if (pSchPres->PsNHeirElems)
	     memset (pSchPres->PsNHeirElems, 0, size);

	   size = pSS->SsRuleTableSize * sizeof (int);
	   pSchPres->PsNInheritedAttrs = (NumberTable*) malloc (size);
	   if (pSchPres->PsNInheritedAttrs)
	     memset (pSchPres->PsNInheritedAttrs, 0, size);

	   size = pSS->SsRuleTableSize * sizeof (InheritAttrTable*);
	   pSchPres->PsInheritedAttr = (InheritAttrTbTb*) malloc (size);
	   if (pSchPres->PsInheritedAttr)
	     memset (pSchPres->PsInheritedAttr, 0, size);

	   size = pSS->SsAttrTableSize * sizeof (int);
	   pSchPres->PsNComparAttrs = (NumberTable*) malloc (size);
	   if (pSchPres->PsNComparAttrs)
	     memset (pSchPres->PsNComparAttrs, 0, size);

	   size = pSS->SsAttrTableSize * sizeof (PtrAttributePres);
	   pSchPres->PsComparAttr = (CompAttrTbTb*) malloc (size);
	   if (pSchPres->PsComparAttr)
	     memset (pSchPres->PsComparAttr, 0, size);

	   size = pSS->SsRuleTableSize * sizeof (int);
	   pSchPres->PsElemTransmit = (NumberTable*) malloc (size);
	   if (pSchPres->PsElemTransmit)
	     memset (pSchPres->PsElemTransmit, 0, size);
	 }
     }
   return ((PSchema) pSchPres);
}

/*----------------------------------------------------------------------
  TtaMoveDocumentExtensionsToElement moves schema extensions of a
  document to a hierarchy of elements.
  This is useful to manage CSS style sheets attached to an external object.
  ----------------------------------------------------------------------*/
void TtaMoveDocumentExtensionsToElement (Document document, Element element)
{
  PtrDocSchemasDescr  pPfS;

  if (!LoadedDocument[document - 1] || element == NULL)
    return;
  else
    {
      /* link document descriptors to the element hierarchy */
      pPfS = LoadedDocument[document - 1]->DocFirstSchDescr;
#ifdef IV
      // don't release structure schemas as they are used to remove CSS
      first = pPfS;
      while (pPfS)
	{
	  pNextPfS = pPfS->PfNext;
	  if (pPfS->PfSSchema)
	    ReleaseStructureSchema (pPfS->PfSSchema,
				    LoadedDocument[document - 1]);
	  pPfS = pNextPfS;
	}
#endif
      LoadedDocument[document - 1]->DocFirstSchDescr = NULL;
      SetElSchemasExtens ((PtrElement) element, pPfS);
    }

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
void TtaUnlinkPSchema (PSchema schema, Document document, SSchema nature)
{
  PtrSSchema	       pSchS;

  if (!LoadedDocument[document - 1])
    return;
  if (nature == NULL)
    pSchS = LoadedDocument[document - 1]->DocSSchema;
  else
    pSchS = (PtrSSchema) nature;
  UnlinkPSchemaExtension (LoadedDocument[document - 1], pSchS,
			  (PtrPSchema) schema);
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
void TtaRemovePSchema (PSchema schema, Document document, SSchema nature)
{
  PtrPSchema          pSchP;
  PtrSSchema	       pSchS;

  if (!LoadedDocument[document - 1])
    return;
  if (nature == NULL)
    pSchS = LoadedDocument[document - 1]->DocSSchema;
  else
    pSchS = (PtrSSchema) nature;
  /* in any case free the Pschema */
  pSchP = (PtrPSchema) schema;
  if (pSchP->PsStructCode > 0)
    {
      pSchP->PsStructCode--;	/* number of documents using this schema */
      if (pSchP->PsStructCode == 0)
	/* this presentation schema is no longer used */
	FreePresentationSchema (pSchP, pSchS, LoadedDocument[document - 1]);
    }
}

/*----------------------------------------------------------------------
   TtaAddPSchema

   Associates an additional presentation schema with a document.
   All additional presentation schemas for a main presentation schema are
   stored in an ordered list, in increasing priority order.
   TtaAddPSchema inserts the new schema in that list immediately before
   (or after, according to parameter "before") another schema which is
   already part of the list (parameter "oldSchema").
   Parameters:
   schema: the presentation schema to be added
   oldSchema: a presentation schema that is already associated with the
      document. NULL when adding the first additional presentation schema.
   before: if TRUE, the new presentation schema is inserted just before
   oldSchema, else it is inserted just after. Meaningless if oldSchema is NULL.
   document: the document to which the presentation schema is added.
   name: the name of the presentation schema.
  ----------------------------------------------------------------------*/
void TtaAddPSchema (PSchema schema, PSchema oldSchema, ThotBool before,
		    Document document, SSchema nature, char *name)
{
  PtrSSchema	      pSchS;

  if (document < 1 || document > MAX_DOCUMENTS)
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument[document - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  else
    {
      if (nature == NULL)
	pSchS = LoadedDocument[document - 1]->DocSSchema;
      else
	pSchS = (PtrSSchema) nature;
      if (InsertPSchemaExtension (LoadedDocument[document - 1], pSchS,
				  (PtrPSchema) schema, (PtrPSchema) oldSchema,
				  before))
	{
	  /* number of documents using this schema */
	  ((PtrPSchema) schema)->PsStructCode++;
	  /* name of associated structure schema */
	  if (((PtrPSchema) schema)->PsStructName)
	    TtaFreeMemory (((PtrPSchema) schema)->PsStructName);
	  ((PtrPSchema) schema)->PsStructName = TtaStrdup (pSchS->SsName);
	  if (((PtrPSchema) schema)->PsPresentName)
	    TtaFreeMemory (((PtrPSchema) schema)->PsPresentName);
	  ((PtrPSchema) schema)->PsPresentName = TtaStrdup (name);
	}
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
PSchema TtaGetFirstPSchema (Document document, SSchema nature)
{
   PtrPSchema          pSchPres;
   PtrSSchema	       pSchS;
   PtrHandlePSchema    pHPSch;

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
       pHPSch = FirstPSchemaExtension (pSchS, LoadedDocument[document - 1],
				       NULL);
       if (pHPSch)
         pSchPres = pHPSch->HdPSchema;
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
void TtaNextPSchema (PSchema * schema, Document document, SSchema nature)
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
