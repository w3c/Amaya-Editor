/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2001
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
   Chargement et liberation des schemas de structure et de presentation
 */

#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "libmsg.h"
#include "message.h"

#ifndef NODISPLAY
#include "fileaccess.h"
#endif /* NODISPLAY */

typedef struct _AStructure
  {
     PtrSSchema      pStructSchema;    /* pointeur sur le schema */
     int             UsageCount;       /* nombre d'utilisations de ce schema */
     Name            StructSchemaName; /* nom du schema de structure */
  }
AStructure;

#ifndef NODISPLAY
typedef struct _APresentation
  {
     PtrPSchema      pPresSchema;    /* pointeur sur le schema */
     int             UsageCount;     /* nombre d'utilisations de ce schema */
     Name            PresSchemaName; /* nom du schema de presentation */
  }
APresentation;

#include "appstruct.h"
#include "appdialogue.h"
#endif /* NODISPLAY */

#define THOT_EXPORT extern
#include "edit_tv.h"

#ifndef NODISPLAY
#include "appdialogue_tv.h"
#include "modif_tv.h"

/* list of structure schemas used by saved elements (cut/past commands) */
PtrDocSchemasDescr FirstSchemaUsedBySavedElements = NULL;

/* table des schemas de presentation charges */
#define MAX_PSCHEMAS 10		/* max. number of loaded presentation schemas*/
static APresentation LoadedPSchema[MAX_PSCHEMAS];
#endif /* NODISPLAY */

/* table des schemas de structure charge's */
#define MAX_SSCHEMAS 10		/* max. number of loaded structure schemas */
static AStructure LoadedSSchema[MAX_SSCHEMAS];

#include "config_f.h"
#include "memory_f.h"
#include "readstr_f.h"
#include "schemas_f.h"

#ifndef NODISPLAY
#include "readprs_f.h"
#include "tree_f.h"
#endif /* NODISPLAY */

/*----------------------------------------------------------------------
   InitNatures     initialise la table des schemas de presentation 
   charges.                                        
  ----------------------------------------------------------------------*/
void InitNatures ()
{
   int                 i;

   for (i = 0; i < MAX_SSCHEMAS; i++)
      {
      LoadedSSchema[i].pStructSchema = NULL;
      LoadedSSchema[i].UsageCount = 0;
      LoadedSSchema[i].StructSchemaName[0] = EOS;
      }
#ifndef NODISPLAY
   FirstSchemaUsedBySavedElements = NULL;
   for (i = 0; i < MAX_PSCHEMAS; i++)
      {
      LoadedPSchema[i].pPresSchema = NULL;
      LoadedPSchema[i].UsageCount = 0;
      LoadedPSchema[i].PresSchemaName[0] = EOS;
      }
#endif /* NODISPLAY */
}

/*----------------------------------------------------------------------
  GetSSchemaForDoc
  Return the structure schema called name used by document pDoc.
  Return NULL if this document does not use this structure schema.
  ----------------------------------------------------------------------*/
PtrSSchema GetSSchemaForDoc (char *name, PtrDocument pDoc)
{
  PtrSSchema          pSS;
  PtrDocSchemasDescr  pPfS;

  pSS = NULL;
  pPfS = pDoc->DocFirstSchDescr;
  while (pPfS && !pSS)
    {
      if (pPfS->PfSSchema)
	if (strcmp (name, pPfS->PfSSchema->SsName) == 0)
	  pSS = pPfS->PfSSchema;
      pPfS = pPfS->PfNext;
    }
  return pSS;
}

/*----------------------------------------------------------------------
  GetSSchemaByUriForDoc
  Return the structure schema with URI uriName used by document pDoc.
  Return NULL if this document does not use this structure schema.
  ----------------------------------------------------------------------*/
PtrSSchema GetSSchemaByUriForDoc (char *uriName, PtrDocument pDoc)
{
  PtrSSchema          pSS;
  PtrDocSchemasDescr  pPfS;

  pSS = NULL;
  pPfS = pDoc->DocFirstSchDescr;
  while (pPfS && !pSS)
    {
      if (pPfS->PfSSchema && pPfS->PfSSchema->SsUriName)
	if (strcmp (uriName, pPfS->PfSSchema->SsUriName) == 0)
	  pSS = pPfS->PfSSchema;
      pPfS = pPfS->PfNext;
    }
  return pSS;
}

/*----------------------------------------------------------------------
   StructSchemaForDoc
   Return the block describing structure schema pSS used by a document pDoc
  ----------------------------------------------------------------------*/
static PtrDocSchemasDescr StructSchemaForDoc (PtrDocument pDoc,
					      PtrSSchema pSS,
					      PtrDocSchemasDescr *pPrevPfS)
{
  PtrDocSchemasDescr pPfS;

  pPfS = NULL;
  *pPrevPfS = NULL;
  if (pDoc && pSS)
    {
      pPfS = pDoc->DocFirstSchDescr;
      while (pPfS && pPfS->PfSSchema != pSS)
	{
	  *pPrevPfS = pPfS;
	  pPfS = pPfS->PfNext;
	}
    }

  return pPfS;
}

/*----------------------------------------------------------------------
   ResetNatureRules
   Check all Nature rules in all structure schema that are currently
   loaded. If a Nature rule points to the oldSS schema, reset that pointer.
  ----------------------------------------------------------------------*/
static void ResetNatureRules (PtrSSchema oldSS)
{
  PtrSSchema   pSS;
  int          i, rule;
  PtrSRule     pRule;

  /* Look at the table of loaded schemas */
  for (i = 0; i < MAX_SSCHEMAS; i++)
    {
      pSS = LoadedSSchema[i].pStructSchema;
      if (pSS)
	/* check all rules of that schema */
	for (rule = 0; rule < pSS->SsNRules; rule++)
	  {
	    pRule = pSS->SsRule->SrElem[rule];
	    if (pRule->SrConstruct == CsNatureSchema)
	      {
		/* it's a nature rule */
		if (pRule->SrSSchemaNat == oldSS)
		  /* it points to the old schema */
		  pRule->SrSSchemaNat = NULL;
	      }
	  }
    }
}

#ifndef NODISPLAY

/*----------------------------------------------------------------------
   RegisterSSchemaForSavedElements
   Register schema pSSchema in the list of schemas used by the saved elements
  ----------------------------------------------------------------------*/
void RegisterSSchemaForSavedElements (PtrSSchema pSSchema)
{
  PtrDocSchemasDescr  pPfS, pPrevPfS;
  int                 i;

  if (!pSSchema)
    return;
  /* is this schema already in the list? */
  pPfS = FirstSchemaUsedBySavedElements;
  while (pPfS && pPfS->PfSSchema != pSSchema)
    pPfS = pPfS->PfNext;
  if (!pPfS)
    /* this schema is not in the list. Add it */
    {
      /* look first for this schema in the list of loaded schemas */
      for (i = 0; (i < MAX_SSCHEMAS) &&
                  (LoadedSSchema[i].pStructSchema != pSSchema); i++);
      if (i >= MAX_SSCHEMAS)
        /* This schema is not in the table, error */
	fprintf (stderr, "S schema %s not loaded!?!?\n", pSSchema->SsName);
      else
        {
	  /* increment the number of users of this schema */
	  LoadedSSchema[i].UsageCount++;
	  /* add the schema to the list of schemas used by the saved elements*/
	  GetDocSchemasDescr (&pPfS);
	  pPfS->PfNext = NULL;
	  pPfS->PfSSchema = pSSchema;
	  pPfS->PfPSchema = NULL;
	  pPfS->PfFirstPSchemaExtens = NULL;
	  /* append the new schema descriptor */
	  if (FirstSchemaUsedBySavedElements == NULL)
	    FirstSchemaUsedBySavedElements = pPfS;
	  else
	    {
	      pPrevPfS = FirstSchemaUsedBySavedElements;
	      while (pPrevPfS->PfNext)
		pPrevPfS = pPrevPfS->PfNext;
	      pPrevPfS->PfNext = pPfS;
	    }
	}
    }
}

/*----------------------------------------------------------------------
   ReleaseSSchemasForSavedElements
   Cancel the list of schemas used by the saved elements and unload the
   schemas that are not used by any document.
  ----------------------------------------------------------------------*/
void ReleaseSSchemasForSavedElements ()
{
  AStructure          *pStr;
  PtrDocSchemasDescr  pPfS, pNextPfS;
  PtrSSchema          pSS;
  int                 i;

  pPfS = FirstSchemaUsedBySavedElements;
  while (pPfS)
    {
      pSS = pPfS->PfSSchema;
      /* look for this schema in the table */
      for (i = 0; i < MAX_SSCHEMAS && LoadedSSchema[i].pStructSchema != pSS;
	   i++);
      if (i >= MAX_SSCHEMAS)
	/* error. This schema is not in the table */
	fprintf (stderr, "S schema %s released but not loaded!?!?\n",
		 pSS->SsName);
      else
	{
	  pStr = &LoadedSSchema[i];
	  pStr->UsageCount--;
	  if (pStr->UsageCount <= 0)
	    /* This schema is no longer used by any document. Unload it */
	    {
	      pStr->pStructSchema = NULL;
	      pStr->StructSchemaName[0] = EOS;
	      ResetNatureRules (pSS);
	      FreeSchStruc (pSS);
	    }
	}
      pNextPfS = pPfS->PfNext;
      FreeDocSchemasDescr (pPfS);
      pPfS = pNextPfS;
    }
  FirstSchemaUsedBySavedElements = NULL;
}

/*----------------------------------------------------------------------
   FreePRuleList
   libere la liste de regles de presentation dont l'ancre est firstPRule.
  ----------------------------------------------------------------------*/
static void         FreePRuleList (PtrPRule * firstPRule)
{
   PtrPRule            pPRule, pNextPRule;

   pPRule = *firstPRule;
   while (pPRule != NULL)
      {
      pNextPRule = pPRule->PrNextPRule;
      FreePresentRule (pPRule);
      pPRule = pNextPRule;
      }
   *firstPRule = NULL;
}

/*----------------------------------------------------------------------
  ReleasePresentationSchema						
  The presentation schema pPSchema is released.
  Free all the content when the boolean force is TRUE (Extension schema)
  or when the count of usages is null.
  ----------------------------------------------------------------------*/
static void ReleasePresentationSchema (PtrPSchema pPSchema, PtrSSchema pSS, 
				       PtrDocument pDoc, ThotBool force)
{
  APresentation      *pPres;
  AttributePres      *pAttrPres;
  int                 i, j;
  PtrHostView         pHostView, pNextHostView;

  /* look for this schema in the PSchemas table */
  for (i = 0; i < MAX_PSCHEMAS && LoadedPSchema[i].pPresSchema != pPSchema;
       i++);
  if (i < MAX_PSCHEMAS)
    {
      /* this schema is in the table */
      pPres = &LoadedPSchema[i];
      pPres->UsageCount--;
      if (pPres->UsageCount == 0)
	/* c'etait la derniere utilisation, on le supprime de la table */
	{
	  pPres->pPresSchema = NULL;
	  pPres->PresSchemaName[0] = EOS;
	  force = TRUE;
	}
    }

  if (force)
    {
      /* libere les regles de presentation par defaut */
      FreePRuleList (&pPSchema->PsFirstDefaultPRule);
      /* libere les regles de presentation des boites de presentation */
      for (i = 0; i < pPSchema->PsNPresentBoxes; i++)
	FreePRuleList (&pPSchema->PsPresentBox[i].PbFirstPRule);
      /* libere les regles de presentation des attributs */
      for (i = 0; i < pSS->SsNAttributes; i++)
	{
	  pAttrPres = pPSchema->PsAttrPRule->AttrPres[i];
	  while (pAttrPres != NULL)
	    {
	      switch (pSS->SsAttribute->TtAttr[i]->AttrType)
		{
		case AtNumAttr:
		  for (j = 0; j < pAttrPres->ApNCases; j++)
		    FreePRuleList (&pAttrPres->ApCase[j].CaFirstPRule);
		  break;
		case AtTextAttr:
		  FreePRuleList (&pAttrPres->ApTextFirstPRule);
		  break;
		case AtReferenceAttr:
		  FreePRuleList (&pAttrPres->ApRefFirstPRule);
		  break;
		case AtEnumAttr:
		  for (j = 0;
		       j <= pSS->SsAttribute->TtAttr[i]->AttrNEnumValues; j++)
		    FreePRuleList (&pAttrPres->ApEnumFirstPRule[j]);
		  break;
		default:
		  break;
		}
	      pAttrPres = pAttrPres->ApNextAttrPres;
	    }
	}

      /* libere les regles de presentation des types */
      for (i = 0; i < pSS->SsNRules; i++)
	FreePRuleList (&pPSchema->PsElemPRule->ElemPres[i]);
      /* libere les descripteurs de vues hotes */
      for (i = 0; i < MAX_VIEW; i++)
	{
	  pHostView = pPSchema->PsHostViewList[i];
	  pPSchema->PsHostViewList[i] = 0;
	  while (pHostView)
	    {
	      pNextHostView = pHostView->NextHostView;
	      TtaFreeMemory (pHostView);
	      pHostView = pNextHostView;
	    }
	}
      FreeSchPres (pPSchema, pSS);
    }
}

/*----------------------------------------------------------------------
   LoadPresentationSchema
   If presentation schema called schemaName is already loaded, return a
   pointer to it, otherwise load it from its file, register it in the
   table of loaded schemas, and return a pointer to it.
   Return TRUE if schema has been successfully loaded.
  ----------------------------------------------------------------------*/
ThotBool LoadPresentationSchema (Name schemaName, PtrSSchema pSS,
				 PtrDocument pDoc)
{
   PtrPSchema           pPSchema;
   PtrDocSchemasDescr   pPfS, pPrevPfS;
   int                  i;

   if (pSS == NULL)
     /* invalid parameter */
     return FALSE;
   if (schemaName == NULL)
     /* no presentation schema specified, use the default P schema specified
	in the structure schema */
     schemaName = pSS->SsDefaultPSchema;
   pPfS = StructSchemaForDoc (pDoc, pSS, &pPrevPfS);
   if (pPfS && pPfS->PfPSchema &&
       !strcmp (schemaName, pPfS->PfPSchema->PsPresentName))
     /* this presentation schema is already associated with this structure
	schema for this document */
     return TRUE;
   pPSchema = NULL;
   /* Look at the table of loaded schemas */
   for (i = 0; i < MAX_PSCHEMAS &&
	       strcmp (schemaName, LoadedPSchema[i].PresSchemaName); i++);
   if (i < MAX_PSCHEMAS)
      /* This schema is in the table, no need to load it */
      {
      LoadedPSchema[i].UsageCount++;
      pPSchema = LoadedPSchema[i].pPresSchema;
      }
   else
      /* That's a new schema. Load it */
      {
      pPSchema = ReadPresentationSchema (schemaName, pSS);
      if (pPSchema)
	 /* schema loaded. Register it in the table of loaded schemas */
	 {
	 /* look for an free entry in the table */
	 for (i = 0; i < MAX_PSCHEMAS && LoadedPSchema[i].pPresSchema; i++);
	 if (i < MAX_PSCHEMAS && LoadedPSchema[i].pPresSchema == NULL)
	    /* free entry found */
	    {
	    LoadedPSchema[i].UsageCount = 1;
	    LoadedPSchema[i].pPresSchema = pPSchema;
	    strncpy (LoadedPSchema[i].PresSchemaName, schemaName,
		     MAX_NAME_LENGTH);
	    }
	 }
      }
   if (pPSchema)
     {
     strncpy (pSS->SsDefaultPSchema, schemaName, MAX_NAME_LENGTH);
     /* associate the presentation schema with the structure schema
        for this document */
     if (!pPfS)
       fprintf (stderr, "*** S schema %s missing ***\n",
		pPSchema->PsPresentName);
     else
       {
	 if (pPfS->PfPSchema)
	   /* release the previous presentation schema */
	   {
	     ReleasePresentationSchema (pPfS->PfPSchema, pSS, pDoc, FALSE);
	     pPfS->PfPSchema = NULL;
	   }
	 pPfS->PfPSchema = pPSchema;
       }
     }
   return (pPSchema != NULL);
}

/*----------------------------------------------------------------------
   FreePresentationSchema						
   Libere le schema de presentation pointe par pPSchema,		
   ainsi que toutes les regles de presentation qu'il pointe.	
   pSS pointe le schema de structure auquel le schema de           
   presentation a liberer est associe.
  ----------------------------------------------------------------------*/
void FreePresentationSchema (PtrPSchema pPSchema, PtrSSchema pSS,
			     PtrDocument pDoc)
{
  PtrDocSchemasDescr   pPfS, pPrevPfS;
  PtrHandlePSchema     pHd, pNextHSP;

  ReleasePresentationSchema (pPSchema, pSS, pDoc, FALSE);
  pPfS = StructSchemaForDoc (pDoc, pSS, &pPrevPfS);
  if (pPfS && pPfS->PfPSchema == pPSchema)
    /* it's the main presentation schema. Unlink it */
    {
      pPfS->PfPSchema = NULL;
      pHd = pPfS->PfFirstPSchemaExtens;
      while (pHd)
	{
	  pNextHSP = pHd->HdNextPSchema;
	  /* release the extension schema */
	  ReleasePresentationSchema (pHd->HdPSchema, pSS, pDoc, TRUE);
	  FreeHandleSchPres (pHd);
	  pHd = pNextHSP;
	}
      pPfS->PfFirstPSchemaExtens = NULL;
      /* unlink the block */
      if (pPfS->PfSSchema == NULL)
	{
	  if (pPrevPfS)
	    pPrevPfS = pPfS->PfNext;
	  else
	    pDoc->DocFirstSchDescr = pPfS->PfNext;
	  FreeDocSchemasDescr (pPfS);
	}
    }
}

/*----------------------------------------------------------------------
  FirstPSchemaExtension
  Returns the first extension to the presentation schema associated with
  structure schema pSS in document pDoc.
  ----------------------------------------------------------------------*/
PtrHandlePSchema FirstPSchemaExtension (PtrSSchema pSS, PtrDocument pDoc)
{
  PtrDocSchemasDescr  pPfS, pPrevPfS;

  pPfS = StructSchemaForDoc (pDoc, pSS, &pPrevPfS);
  if (pPfS)
    return (pPfS->PfFirstPSchemaExtens);
  else
    return NULL;
}

/*----------------------------------------------------------------------
  UnlinkPSchemaExtension
  ----------------------------------------------------------------------*/
void UnlinkPSchemaExtension (PtrDocument pDoc, PtrSSchema pSS,
			     PtrPSchema pPS)
{
  PtrDocSchemasDescr  pPfS, pPrevPfS;
  PtrHandlePSchema    pHd;

  if (pDoc && pSS && pPS)
    {
      pPfS = StructSchemaForDoc (pDoc, pSS, &pPrevPfS);
      if (pPfS)
	{
	  pHd = pPfS->PfFirstPSchemaExtens;
	  while (pHd && pHd->HdPSchema != pPS)
	    pHd = pHd->HdNextPSchema;
	  if (pHd)
	    {
	      /* release the extension schema */
	      ReleasePresentationSchema (pHd->HdPSchema, pSS, pDoc, TRUE);
	      /* release the block */
	      if (pHd->HdPrevPSchema == NULL)
		pPfS->PfFirstPSchemaExtens = pHd->HdNextPSchema;
	      else
		pHd->HdPrevPSchema->HdNextPSchema = pHd->HdNextPSchema;
	      if (pHd->HdNextPSchema)
		pHd->HdNextPSchema->HdPrevPSchema = pHd->HdPrevPSchema;
	      FreeHandleSchPres (pHd);
	    }
	}
    }
}

/*----------------------------------------------------------------------
  InsertPSchemaExtension
  ----------------------------------------------------------------------*/
ThotBool InsertPSchemaExtension (PtrDocument pDoc, PtrSSchema pSS,
				 PtrPSchema pPS, PtrPSchema pOldPS,
				 ThotBool before)
{
  PtrDocSchemasDescr  pPfS, pPrevPfS;
  PtrHandlePSchema    oldHd, newHd;
  ThotBool            ok;

  pPfS = StructSchemaForDoc (pDoc, pSS, &pPrevPfS);
  if (!pPfS)
    return FALSE;
  ok = FALSE;
  oldHd = pPfS->PfFirstPSchemaExtens;
  if (!pOldPS)
    ok = TRUE;
  else
    {
      while (oldHd && oldHd->HdPSchema != pOldPS)
	oldHd = oldHd->HdNextPSchema;
      if (oldHd)
	ok = TRUE;
    }

  if (ok)
    {
      GetHandleSchPres (&newHd);
      newHd->HdPSchema = pPS;
      if (oldHd == NULL)
	pPfS->PfFirstPSchemaExtens = newHd;
      else if (before)
	{
	  newHd->HdNextPSchema = oldHd;
	  newHd->HdPrevPSchema = oldHd->HdPrevPSchema;
	  oldHd->HdPrevPSchema = newHd;
	  if (newHd->HdPrevPSchema)
	    newHd->HdPrevPSchema->HdNextPSchema = newHd;
	  else
	    pPfS->PfFirstPSchemaExtens = newHd;
	}
      else
	{
	  newHd->HdNextPSchema = oldHd->HdNextPSchema;
	  newHd->HdPrevPSchema = oldHd;
	  oldHd->HdNextPSchema = newHd;
	  if (newHd->HdNextPSchema)
	    newHd->HdNextPSchema->HdPrevPSchema = newHd;
	}
    }
  return ok;
}
#endif /* NODISPLAY */

/*----------------------------------------------------------------------
  PresentationSchema
  Returns the presentation schema associated with structure schema pSS
  in document pDoc.
  ----------------------------------------------------------------------*/
PtrPSchema PresentationSchema (PtrSSchema pSS, PtrDocument pDoc)
{
#ifndef NODISPLAY
  PtrDocSchemasDescr  pPfS, pPrevPfS;

  pPfS = StructSchemaForDoc (pDoc, pSS, &pPrevPfS);
  if (pPfS)
    return (pPfS->PfPSchema);
  else
#endif /* NODISPLAY */
    return NULL;
}

/*----------------------------------------------------------------------
   LoadStructureSchema
   If structure schema called schemaName is already loaded, return a
   pointer to it, otherwise load it from its file, register it in the
   table of loaded schemas, and return a pointer to it.
   Return NULL if schema can't be loaded.
  ----------------------------------------------------------------------*/
PtrSSchema LoadStructureSchema (Name schemaName, PtrDocument pDoc)
{
   PtrSSchema           pSSchema;
   PtrDocSchemasDescr   pPfS, pPrevPfS;
   int                  i;

   pSSchema = NULL;
   if (schemaName == NULL || schemaName[0] == EOS)
     /* invalid parameter */
     return NULL;

   /* is this schema already used by the document? */
   pPfS = pDoc->DocFirstSchDescr;
   while (pPfS && pPfS->PfSSchema &&
	  strcmp (schemaName, pPfS->PfSSchema->SsName))
     pPfS = pPfS->PfNext;
   if (pPfS)
     /* this schema is already used by the document */
     return (pPfS->PfSSchema);

   /* Look at the table of loaded schemas */
   for (i = 0; i < MAX_SSCHEMAS &&
	       strcmp (schemaName, LoadedSSchema[i].StructSchemaName); i++);
   if (i < MAX_SSCHEMAS)
      /* This schema is in the table, no need to load it */
      {
      LoadedSSchema[i].UsageCount++;
      pSSchema = LoadedSSchema[i].pStructSchema;
      }
   else
      /* That's a new schema. Load it */
      {
      /* get some memory */
      GetSchStruct (&pSSchema);
      /* read the file */
      if (!ReadStructureSchema (schemaName, pSSchema))
	/* failure */
	{
	 FreeSchStruc (pSSchema);
	 pSSchema = NULL;
	}
      else
	 /* schema loaded. Register it in the table of loaded schemas */
	 {
	 /* look for an free entry in the table */
	 for (i = 0; i < MAX_SSCHEMAS && LoadedSSchema[i].pStructSchema; i++);
	 if (i < MAX_SSCHEMAS && LoadedSSchema[i].pStructSchema == NULL)
	    /* free entry found */
	    {
	    LoadedSSchema[i].UsageCount = 1;
	    LoadedSSchema[i].pStructSchema = pSSchema;
	    strncpy (LoadedSSchema[i].StructSchemaName, schemaName,
		      MAX_NAME_LENGTH);
	    }
	 /* translate the structure schema in the user's language */
	 ConfigTranslateSSchema (pSSchema);
	 }
      }
   if (pSSchema)
     /* add a schema descriptor to the document */
     {
       GetDocSchemasDescr (&pPfS);
       pPfS->PfNext = NULL;
       pPfS->PfSSchema = pSSchema;
       pPfS->PfPSchema = NULL;
       pPfS->PfFirstPSchemaExtens = NULL;
       /* append the new schema descriptor */
       if (pDoc->DocFirstSchDescr == NULL)
	 pDoc->DocFirstSchDescr = pPfS;
       else
	 {
	   pPrevPfS = pDoc->DocFirstSchDescr;
	   while (pPrevPfS->PfNext)
	     pPrevPfS = pPrevPfS->PfNext;
	   pPrevPfS->PfNext = pPfS;
	 }
     }
   return pSSchema;
}

/*----------------------------------------------------------------------
   ReleaseStructureSchema
   Structure schema pSS is no longer used by document pDoc.
   If it's not used by any other document, unload it.
   Return TRUE if the schema is no longer used by any document. It has
   been unloaded.
  ----------------------------------------------------------------------*/
ThotBool      ReleaseStructureSchema (PtrSSchema pSS, PtrDocument pDoc)
{
  AStructure          *pStr;
  PtrDocSchemasDescr  pPfS, pPrevPfS;
  int                 i;
  ThotBool            result;

  result = FALSE;
  /* look for this schema in the table */
  for (i = 0; i < MAX_SSCHEMAS && LoadedSSchema[i].pStructSchema != pSS; i++);
  if (i >= MAX_SSCHEMAS)
    /* error. This schema is not in the table */
    return FALSE;
  pStr = &LoadedSSchema[i];
  pStr->UsageCount--;
  if (pStr->UsageCount <= 0)
    /* This schema is no longer used by any document. Unload it */
    {
      pStr->pStructSchema = NULL;
      pStr->StructSchemaName[0] = EOS;
      ResetNatureRules (pSS);
      FreeSchStruc (pSS);
      result = TRUE;
    }
  pPfS = StructSchemaForDoc (pDoc, pSS, &pPrevPfS);
  if (pPfS)
    {
      pPfS->PfSSchema = NULL;
      if (pPfS->PfPSchema == NULL)
	{
	  if (pPrevPfS)
	    pPrevPfS = pPfS->PfNext;
	  else
	    pDoc->DocFirstSchDescr = pPfS->PfNext;
	  FreeDocSchemasDescr (pPfS);
	}
    }
  return result;
}

/*----------------------------------------------------------------------
   LoadNatureSchema
   Charge la nature definie dans la regle rule du schema de structure pSS.
   Si le 1er octet de PSchName est nul on propose a l'utilisateur le schema
   de presentation par defaut defini dans le schema de structure, sinon on
   propose le schema de presentation de nom PSchName.
  ----------------------------------------------------------------------*/
void LoadNatureSchema (PtrSSchema pSS, char *PSchName, int rule,
		       PtrDocument pDoc)
{
   Name          schName;
   PtrSSchema    pNatureSS;
#ifndef NODISPLAY
   ThotBool      loaded;
#endif

   /* utilise le nom de la nature comme nom de fichier. */
   /* copie le nom de nature dans schName */
   strncpy (schName, pSS->SsRule->SrElem[rule-1]->SrOrigNat, MAX_NAME_LENGTH);
   /* cree un schema de structure et le charge depuis le fichier */
   pNatureSS = LoadStructureSchema (schName, pDoc);
   if (!pNatureSS)
      /* echec */
      pSS->SsRule->SrElem[rule - 1]->SrSSchemaNat = NULL;
   else
      /* chargement du schema de structure reussi */
      {
      pSS->SsRule->SrElem[rule - 1]->SrSSchemaNat = pNatureSS;
#ifndef NODISPLAY
      loaded = FALSE;
      if (PSchName != NULL && PSchName[0] != EOS)
	 /* l'appelant indique un schema de presentation, on essaie de le
	    charger */
	 {
	 strncpy (schName, PSchName, MAX_NAME_LENGTH);
	 loaded = LoadPresentationSchema (schName, pNatureSS, pDoc);
	 }
      if (PSchName == NULL || PSchName[0] == EOS || !loaded)
	 /* pas de schema de presentation particulier demande' par l'appelant*/
	 /* ou schema demande' inaccessible */
	 {
	 /* on consulte le fichier .conf */
	 if (!ConfigGetPSchemaNature (pSS, pSS->SsRule->SrElem[rule - 1]->SrOrigNat,
				      schName))
	    /* le fichier .conf ne donne pas de schema de presentation pour */
	    /* cette nature, on le demande a l'utilisateur */
	    strncpy (schName, pNatureSS->SsDefaultPSchema, MAX_NAME_LENGTH);
	 /* cree un nouveau schema de presentation et le charge depuis le
	    fichier */
	 loaded = LoadPresentationSchema (schName, pNatureSS, pDoc);
	 }
      if (!loaded)
	 /* failed loading presentation schema */
	 {
	 TtaDisplayMessage (INFO, TtaGetMessage (LIB, TMSG_INCORRECT_PRS_FILE),
			    schName);
	 ReleaseStructureSchema (pNatureSS, pDoc);
	 pSS->SsRule->SrElem[rule - 1]->SrSSchemaNat = NULL;
	 }
      if (ThotLocalActions[T_initevents] != NULL)
	 (*ThotLocalActions[T_initevents]) (pNatureSS);
#endif /* NODISPLAY */
      }
}

/*----------------------------------------------------------------------
   AppendSRule
   Append a new structure rule to structure schema pSS for document pDoc.
   pPSch is the presentation schema associated with pSS for document pDoc.
  ----------------------------------------------------------------------*/
static void         AppendSRule (int *ret, PtrSSchema pSS, PtrPSchema pPSch,
				 PtrDocument pDoc)
{
  int                  size, i;
  PtrDocSchemasDescr   pPfS;
  PtrPSchema           pPSchExt;
  PtrHandlePSchema     pHSP, pHSPNext;

  *ret = 0;
#ifndef NODISPLAY
  pHSP = NULL;
  /* Search the associated presentation schema (including extension schemas) */
  pPfS = pDoc->DocFirstSchDescr;
  while (pPfS && !pHSP)
    {
      if (pPfS->PfSSchema && (pPfS->PfSSchema == pSS))
	{
	  if (!pPSch)
	    pPSch = pPfS->PfPSchema;
	  pHSP = pPfS->PfFirstPSchemaExtens;
	  pPfS = NULL;
	}
      else
	pPfS = pPfS->PfNext;
    }
#endif
  
  /* reserve 2 additional entries for counter aliases (see function
     MakeAliasTypeCount in presvariables.c) */
  if (pSS->SsNRules >= pSS->SsRuleTableSize - 2)
    {
      /* rule table is full. Add 10 new entries */
      size = pSS->SsNRules + 2 + 10;
      i = size * sizeof (PtrSRule);
      pSS->SsRule = (SrRuleTable*) realloc (pSS->SsRule, i);
#ifndef NODISPLAY
      if (pPSch)
	{
	  i = size * sizeof (PtrPRule);
	  pPSch->PsElemPRule = (PtrPRuleTable*) realloc (pPSch->PsElemPRule,i);
	  i = size * sizeof (int);
	  pPSch->PsNInheritedAttrs = (NumberTable*) realloc (pPSch->PsNInheritedAttrs, i);
	  i = size * sizeof (InheritAttrTable*);
          pPSch->PsInheritedAttr = (InheritAttrTbTb*) realloc (pPSch->PsInheritedAttr, i);
	  i = size * sizeof (int);
          pPSch->PsElemTransmit = (NumberTable*) realloc (pPSch->PsElemTransmit, i);
	}
#endif

      if (!pSS->SsRule
#ifndef NODISPLAY
	  || !pPSch->PsElemPRule
#endif
	  )
	{
	  TtaDisplaySimpleMessage (FATAL, LIB, TMSG_NO_MEMORY);
	  return;
	}
      else
	{
	  pSS->SsRuleTableSize = size;
	  for (i = pSS->SsNRules; i < size; i++)
	    pSS->SsRule->SrElem[i] = NULL;
#ifndef NODISPLAY
	  if (pPSch)
	    {
	      for (i = pSS->SsNRules; i < size; i++)
		{
		  pPSch->PsElemPRule->ElemPres[i] = NULL;
		  pPSch->PsNInheritedAttrs->Num[i] = 0;
                  pPSch->PsInheritedAttr->ElInherit[i] = NULL;
		  pPSch->PsElemTransmit->Num[i] = 0;
	}
	    }
#endif
	}

#ifndef NODISPLAY      
      /* associated extension schemas */
      pHSPNext = pHSP;
      while (pHSPNext)
	{
	  pPSchExt = pHSPNext->HdPSchema;
	  if (pPSchExt)
	    {
	      i = size * sizeof (PtrPRule);
	      pPSchExt->PsElemPRule =
		(PtrPRuleTable*) realloc (pPSchExt->PsElemPRule,i);
	      i = size * sizeof (int);
	      pPSchExt->PsNInheritedAttrs =
		(NumberTable*) realloc (pPSchExt->PsNInheritedAttrs, i);
	      i = size * sizeof (InheritAttrTable*);
	      pPSchExt->PsInheritedAttr =
		(InheritAttrTbTb*) realloc (pPSchExt->PsInheritedAttr, i);
	      i = size * sizeof (int);
	      pPSchExt->PsElemTransmit =
		(NumberTable*) realloc (pPSchExt->PsElemTransmit, i);
	      
	      if (!pPSchExt->PsElemPRule)
		{
		  TtaDisplaySimpleMessage (FATAL, LIB, TMSG_NO_MEMORY);
		  return;
		}
	      else
		{
		  for (i = pSS->SsNRules; i < size; i++)
		    {
		      pPSchExt->PsElemPRule->ElemPres[i] = NULL;
		      pPSchExt->PsNInheritedAttrs->Num[i] = 0;
		      pPSchExt->PsInheritedAttr->ElInherit[i] = NULL;
		      pPSchExt->PsElemTransmit->Num[i] = 0;
		    }
		}
	    }
	  /* next extension schema */
	  pHSPNext = pHSPNext->HdNextPSchema;
	}
#endif
    }

  /* initializes new entry */
  i = pSS->SsNRules;
  pSS->SsRule->SrElem[i] = (PtrSRule) malloc (sizeof (SRule));
  if (!pSS->SsRule->SrElem[i])
    {
      TtaDisplaySimpleMessage (FATAL, LIB, TMSG_NO_MEMORY);
      return;      
    }
  memset (pSS->SsRule->SrElem[i], 0, sizeof (SRule));
#ifndef NODISPLAY
  if (pPSch)
    {
      pPSch->PsElemPRule->ElemPres[i] = NULL;
      pPSch->PsNInheritedAttrs->Num[i] = 0;
      pPSch->PsInheritedAttr->ElInherit[i] = NULL;
      pPSch->PsElemTransmit->Num[i] = 0;
    }

  /* associated extension schemas */
  pHSPNext = pHSP;
  while (pHSPNext)
    {
      pPSchExt = pHSPNext->HdPSchema;
      if (pPSchExt)
	{
	  pPSchExt->PsElemPRule->ElemPres[i] = NULL;
	  pPSchExt->PsNInheritedAttrs->Num[i] = 0;
	  pPSchExt->PsInheritedAttr->ElInherit[i] = NULL;
	  pPSchExt->PsElemTransmit->Num[i] = 0;
	}
      /* next extension schema */
      pHSPNext = pHSPNext->HdNextPSchema;
    }
#endif

  pSS->SsNRules++;
  *ret = pSS->SsNRules;
}

/*----------------------------------------------------------------------
   CreateNature
   retourne le numero de la regle definissant le type de nom SSchName
   dans le schema de structure pointe par pSS.
   S'il n'existe pas de type de ce nom, ajoute une regle de type   
   CsNatureSchema au schema de structure et charge le schema de	
   structure de nom SSchName; retourne le numero de la regle creee	
   ou 0 si echec creation.						
   En cas de chargement de schema, le nom de schema de             
   presentation PSchName est propose' a l'utilisateur plutot que	
   le schema de presentation par defaut defini dans le schema de	
   structure, sauf si le premier octet de PSchName est nul.	
  ----------------------------------------------------------------------*/
int          CreateNature (char *SSchName, char *PSchName,
			   PtrSSchema pSS, PtrDocument pDoc)
{
#ifndef NODISPLAY
   PtrPSchema  pPS;
#endif
   PtrSRule           pRule;
   PtrSSchema         pSSch;
   int                ret;
   ThotBool           found;

   /* cherche si le type existe deja dans le schema de structure */
   found = FALSE;
   ret = 0;
   do
      {
      pRule = pSS->SsRule->SrElem[ret++];
      if (pRule->SrConstruct == CsNatureSchema)
	 if (strcmp (pRule->SrOrigNat, SSchName) == 0)
	    found = TRUE;
      }
   while (!found && ret < pSS->SsNRules);
   if (!found)
      /* il n'existe pas, cherche une entree libre dans la table des */
      /* regles */
      {
      if (pSS->SsFirstDynNature == 0)
	/* pas encore de nature chargee dynamiquement */
	/* ajoute une regle a la fin de la table */
	{
	  AppendSRule (&ret, pSS, NULL, pDoc);
	  pSS->SsFirstDynNature = pSS->SsNRules;
	}
      else
	/* il y a deja des natures dynamiques */
	/* cherche s'il y en a une libre */
	{
	  ret = pSS->SsFirstDynNature;
	  while (ret <= pSS->SsNRules &&
		 pSS->SsRule->SrElem[ret - 1]->SrSSchemaNat != NULL)
	    ret++;
	  if (ret > pSS->SsNRules)
	    /* pas de regle libre, on ajoute une regle a la fin de la */
	    /* table */
	    AppendSRule (&ret, pSS, NULL, pDoc);
	}
      if (ret > 0)
	 /* il y a une entree libre (celle de rang ret) */
	 /* remplit la regle nature */
	 {
	 pRule = pSS->SsRule->SrElem[ret - 1];
	 strncpy (pRule->SrOrigNat, SSchName, MAX_NAME_LENGTH);
	 strncpy (pRule->SrName, SSchName, MAX_NAME_LENGTH);
	 pRule->SrNDefAttrs = 0;
	 pRule->SrConstruct = CsNatureSchema;
	 pRule->SrSSchemaNat = NULL;
#ifndef NODISPLAY
	 /* initialise le pointeur sur les regles de presentation qui */
	 /* correspondent a cette nouvelle regle de structure */
	 pPS = PresentationSchema (pSS, pDoc);
	 if (pPS != NULL)
	    pPS->PsElemPRule->ElemPres[ret - 1] = NULL;
#endif   /* NODISPLAY */
	 }
      }
   if (ret > 0)
      /* il y a une entree libre (celle de rang ret) */
      {
      pRule = pSS->SsRule->SrElem[ret - 1];
      if (pRule->SrConstruct == CsNatureSchema)
	 if (pRule->SrSSchemaNat == NULL)
	    /* charge les schemas de structure et de presentation */
	    /* de la nouvelle nature */
	    {
	    LoadNatureSchema (pSS, PSchName, ret, pDoc);
	    if (pRule->SrSSchemaNat == NULL)
	       /* echec chargement */
	       ret = 0;
	    }
      if (found)
	/* the nature was already in the structure schema. This may come from
	   another document that uses the same structure schema */
	{
	  /* does the document already use this nature ? */
	  if (!GetSSchemaForDoc (SSchName, pDoc))
	    /* No. Add a descriptor to the document */
	    {
	      pSSch = LoadStructureSchema (SSchName, pDoc);
#ifndef NODISPLAY
	      LoadPresentationSchema (PSchName, pSSch, pDoc);
#endif   /* NODISPLAY */
	    }
	}
      }
   return ret;
}

/*----------------------------------------------------------------------
   LoadSchemas
   Charge en memoire le schema de structure de nom SSchName ainsi que
   son schema de presentation. Si PSchName est une chaine vide, on charge
   le schema de presentation par defaut defini dans le schema de      
   structure, sinon on charge le schema de presentation de nom        
   PSchName.								
   Si pLoadedSS n'est pas NULL, on ne charge que le schema de		
   presentation pour le schema de structure pointe' par pLoadedSS.	
   extension indique s'il s'agit d'une extension de schema ou d'un    
   schema de structure complet.                                       
  ----------------------------------------------------------------------*/
void LoadSchemas (char *SSchName, char *PSchName, PtrSSchema *pSS,
		  PtrDocument pDoc, PtrSSchema pLoadedSS, ThotBool extension)
{
  Name         schName;
#ifndef NODISPLAY
  ThotBool     loaded;
#endif

  strncpy (schName, SSchName, MAX_NAME_LENGTH);
  /* cree le schema de structure et charge le fichier dedans */
  if (pLoadedSS == NULL)
    {
      *pSS = LoadStructureSchema (SSchName, pDoc);
      if (*pSS)
	{
	  if ((*pSS)->SsExtension != extension)
	    {
	      /* on voulait un schema d'extension et ce n'en est pas un, ou
		 l'inverse */
	      ReleaseStructureSchema (*pSS, pDoc);
	      *pSS = NULL;
	    }
#ifndef NODISPLAY
	  else
	    {
	      if (ThotLocalActions[T_initevents] != NULL)
		(*ThotLocalActions[T_initevents]) (*pSS);
	    }
#endif  /* NODISPLAY */
	}
      }
#ifndef NODISPLAY
   else
      *pSS = pLoadedSS;
   if (*pSS)
      {
      loaded = FALSE;
      if (PSchName != NULL && PSchName[0] != EOS)
	  {
          /* l'appelant specifie le schema de presentation a prendre, on
	     essaie de le charger */
	  strncpy (schName, PSchName, MAX_NAME_LENGTH);
	  loaded = LoadPresentationSchema (schName, *pSS, pDoc);
	  }
      if (PSchName == NULL || PSchName[0] == EOS || !loaded)
	 {
         /* pas de presentation specifiee par l'appelant, ou schema specifie'
	    inaccessible */
	 /* on consulte le fichier de configuration */
	 if (!ConfigDefaultPSchema ((*pSS)->SsName, schName))
            /* le fichier de configuration ne dit rien, on demande a
	       l'utilisateur */
            /* propose la presentation par defaut definie dans le schema de
	       structure */
	    strncpy (schName, (*pSS)->SsDefaultPSchema, MAX_NAME_LENGTH);
	 /* charge le schema de presentation depuis le fichier */
	 loaded = LoadPresentationSchema (schName, *pSS, pDoc);
	 }
      if (!loaded)
	 {
         /* failed loading presentation schema */
         TtaDisplayMessage (INFO, TtaGetMessage (LIB, TMSG_INCORRECT_PRS_FILE),
			    schName);
         ReleaseStructureSchema (*pSS, pDoc);
         *pSS = NULL;
	 }
      }
#endif  /* NODISPLAY */
}

/*----------------------------------------------------------------------
   LoadExtension
   Charge en memoire, pour le document pDoc, le schema d'extension de nom
   SSchName et son schema de presentation de nom PSchName.
  ----------------------------------------------------------------------*/
PtrSSchema LoadExtension (char *SSchName, char *PSchName, PtrDocument pDoc)
{
   PtrSSchema    pExtens, pPrevExtens;
   ThotBool      extensionExist;

   pExtens = NULL;
   if (pDoc->DocSSchema != NULL)
      {
      /* cherche si ce schema d'extension existe deja pour le document */
      pExtens = pDoc->DocSSchema->SsNextExtens;
      extensionExist = FALSE;
      while (pExtens != NULL && !extensionExist)
	 if (strcmp (pExtens->SsName, SSchName) == 0)
	    /* le schema d'extension existe deja */
	    extensionExist = TRUE;
	 else
	    pExtens = pExtens->SsNextExtens;
      if (!extensionExist)
	 /* le schema d'extension n'existe pas, on le charge */
	 {
#ifndef NODISPLAY
	 if (PSchName == NULL || PSchName[0] == EOS)
	    /* pas de schema de presentation precise' */
	    /* cherche le schema de presentation de l'extension prevu */
	    /* dans le fichier .conf pour ce type de document */
	    ConfigGetPSchemaNature (pDoc->DocSSchema, SSchName, PSchName);
#endif /* NODISPLAY */
	 /* charge le schema d'extension demande' */
	 LoadSchemas (SSchName, PSchName, &pExtens, pDoc, NULL, TRUE);
	 if (pExtens != NULL)
	    {
	    /* cherche le dernier schema d'extension du document */
	    pPrevExtens = pDoc->DocSSchema;
	    while (pPrevExtens->SsNextExtens != NULL)
	       pPrevExtens = pPrevExtens->SsNextExtens;
	    /* ajoute le nouveau schema d'extension a la fin de la */
	    /* chaine des schemas d'extension du document */
	    pExtens->SsPrevExtens = pPrevExtens;
	    pPrevExtens->SsNextExtens = pExtens;
	    pExtens->SsNextExtens = NULL;
#ifndef NODISPLAY
	    AddSchemaGuestViews (pDoc, pExtens);
#endif /* NODISPLAY */
	    }
	 }
      }
   return pExtens;
}

/*----------------------------------------------------------------------
  FreeNatureRules
  Looks for a rule Nature within the schema pSS that points to the schema
  pNatureSS. If found, cleans up the rule and returns TRUE.
  If not found, checks within other referred Natures except if the current
  Nature points to the initial schema pDocSS.
  Returns FALSE when no Nature was found.
  ----------------------------------------------------------------------*/
static ThotBool FreeNatureRules (PtrSSchema pSS, PtrSSchema pNatureSS,
				 PtrSSchema pDocSS)
{
   PtrSRule            pRule;
   int                 rule;
   ThotBool            ret;

   ret = FALSE;
   if (pSS != NULL)
      /* parcourt les regles de ce schemas */
      for (rule = 0; rule < pSS->SsNRules; rule++)
	 {
	 pRule = pSS->SsRule->SrElem[rule];
	 if (pRule->SrConstruct == CsNatureSchema)
	   {
	    /* c'est une regle de nature */
	    if (pRule->SrSSchemaNat == pNatureSS)
	       /* elle fait reference a la nature supprimee */
	       /* supprime la reference */
	       {
	       ret = TRUE;
	       pRule->SrSSchemaNat = NULL;
	       }
	    else if (pRule->SrSSchemaNat != pDocSS)
	       /* elle fait reference a une autre nature, on cherche */
	       /* dans cette nature les regles qui font reference a la */
	       /* nature supprimee. */
	       FreeNatureRules (pRule->SrSSchemaNat, pNatureSS, pDocSS);
	   }
	 }
   return ret;
}

/*----------------------------------------------------------------------
   FreeNature
   Si le schema de structure pointe' par pSS contient	
   une regle de nature pour le schema pointe' par pNatureSS,		
   retourne Vrai et libere le schema de structure pointe par		
   pNatureSS et son schema de presentation.            		
   Retourne faux sinon.
  ----------------------------------------------------------------------*/
ThotBool FreeNature (PtrSSchema pSS, PtrSSchema pNatureSS, PtrDocument pDoc)
{
#ifndef NODISPLAY
   PtrPSchema   pPS;
#endif
   ThotBool     ret;

   ret = FALSE;
   /* Cherche tous les schemas de structure qui utilisaient cette nature */
   if (FreeNatureRules (pSS, pNatureSS, pSS))
      {
      ret = TRUE;
#ifndef NODISPLAY
      pPS = PresentationSchema (pNatureSS, pDoc);
      if (pPS != NULL)
	 /* libere le schema de presentation associe' */
	 FreePresentationSchema (pPS, pNatureSS, pDoc);
#endif  /* NODISPLAY */
      ReleaseStructureSchema (pNatureSS, pDoc);
      }
   return ret;
}

/*----------------------------------------------------------------------
   FreeDocumentSchemas
   Libere tous les schemas de structure et de presentation utilises par
   le document dont le descripteur est pointe par pDoc.
   Pour les schemas de presentation, la liberation n'est effective 
   que s'ils ne sont pas utilises par d'autres documents.          
  ----------------------------------------------------------------------*/
void             FreeDocumentSchemas (PtrDocument pDoc)
{
  PtrSSchema          pSS, pDocSS;
  PtrDocSchemasDescr  pPfS;
   int                i;

   while (pDoc->DocFirstSchDescr)
     {
       pPfS = pDoc->DocFirstSchDescr;
       pSS = pPfS->PfSSchema;
       if (pSS)
	 {
#ifndef NODISPLAY
	   FreePresentationSchema (pPfS->PfPSchema, pSS, pDoc);
#endif
	   if (ReleaseStructureSchema (pSS, pDoc))
	     /* this structure schema has been unloaded */
	     /* remove any reference to that schema from all Nature rules
		of other S schemas */
	     for (i = 0; i < MAX_SSCHEMAS; i++)
	       {
		 pDocSS = LoadedSSchema[i].pStructSchema;
		 if (pDocSS)
		   FreeNatureRules (pDocSS, pSS, pDocSS);
	       }
	 }
     }
   pDoc->DocSSchema = NULL;
}

#ifndef NODISPLAY
/*----------------------------------------------------------------------
   AddGuestViews
   Add the guest views defined in presentation schema assosicated with pSS
   to the list of guest views of document view pViewDescr.
  ----------------------------------------------------------------------*/
static void  AddGuestViews (PtrSSchema pSS, DocViewDescr *pViewDescr,
			    PtrDocument pDoc)
{
  PtrPSchema         pPresSch, pPS;
  PtrHostView        pHostView;
  PtrGuestViewDescr  pGuestView;
  int                i;
  ThotBool           found;

  pPresSch = PresentationSchema (pSS, pDoc);
  if (pPresSch)
    /* check all views defined in pPresSch */
    for (i = 0; i < pPresSch->PsNViews; i++)
      {
	pHostView = pPresSch->PsHostViewList[i];
	found = FALSE;
	while (pHostView && !found)
	  {
	    pPS = PresentationSchema (pViewDescr->DvSSchema, pDoc);
	    if (strcmp (pPS->PsView[pViewDescr->DvPSchemaView - 1],
			pHostView->HostViewName) == 0)
	      found = TRUE;
	    else
	      pHostView = pHostView->NextHostView;
	  }
	if (found)
	  {
	    pGuestView = TtaGetMemory (sizeof(GuestViewDescr));
	    pGuestView->GvSSchema = pSS;
	    pGuestView->GvPSchemaView = i+1;
	    /* link the new guest view descriptor */
	    if (pViewDescr->DvFirstGuestView == NULL)
	      pGuestView->GvNextGuestView = NULL;
	    else
	      pGuestView->GvNextGuestView = pViewDescr->DvFirstGuestView;
	    pViewDescr->DvFirstGuestView = pGuestView;
	  }
      }
}

/*----------------------------------------------------------------------
   AddSchemaGuestViews
   add the guest views of presentation schema attached to pSS to the lists
   of guest views of document pDoc.
  ----------------------------------------------------------------------*/
void         AddSchemaGuestViews (PtrDocument pDoc, PtrSSchema pSS)
{
   int                 i;

   if (pSS && pDoc)
      /* check all views of the document */
      for (i = 0; i < MAX_VIEW_DOC; i++)
         if (pDoc->DocView[i].DvSSchema)
	    /* this view is open */
            /* add all its guest views declared in the presentation schema */
            AddGuestViews (pSS, &pDoc->DocView[i], pDoc);
}

/*----------------------------------------------------------------------
   CreateGuestViewList
   Create the guest view list for view view of document pDoc
  ----------------------------------------------------------------------*/
void         CreateGuestViewList (PtrDocument pDoc, int view)
{
  PtrDocSchemasDescr pPfS;
  PtrSSchema         pSS;

  if (pDoc && view > 0)
    {
      /* check all structure schemas used by the document */
      pPfS = pDoc->DocFirstSchDescr;
      while (pPfS)
	{
	  /* for each structure schema, check all its extensions */
	  pSS = pPfS->PfSSchema;
	  while (pSS)
	    {
	      AddGuestViews (pSS, &pDoc->DocView[view - 1], pDoc);
	      pSS = pSS->SsNextExtens;
	    }
	  pPfS = pPfS->PfNext;
	}
    }
}

/*----------------------------------------------------------------------
   BuildDocNatureTable
   Remplit la table des schemas utilises par le document pDoc.
  ----------------------------------------------------------------------*/
void                BuildDocNatureTable (PtrDocument pDoc)
{
  PtrDocSchemasDescr pPfS;
  PtrSSchema         pSS;

  if (pDoc)
    {
      pDoc->DocNNatures = 0;
      pPfS = pDoc->DocFirstSchDescr;
      while (pPfS && pDoc->DocNNatures < MAX_NATURES_DOC)
	{
	  pSS = pPfS->PfSSchema;
	  pDoc->DocNatureSSchema[pDoc->DocNNatures] = pSS;
	  strncpy (pDoc->DocNatureName[pDoc->DocNNatures], pSS->SsName,
		   MAX_NAME_LENGTH);
	  strncpy (pDoc->DocNaturePresName[pDoc->DocNNatures],
		   pSS->SsDefaultPSchema, MAX_NAME_LENGTH);
	  pDoc->DocNNatures++;
	  /* met les extensions du schema dans la table */
	  while (pSS->SsNextExtens && pDoc->DocNNatures < MAX_NATURES_DOC)
	    {
	      pSS = pSS->SsNextExtens;
	      pDoc->DocNatureSSchema[pDoc->DocNNatures] = pSS;
	      strncpy (pDoc->DocNatureName[pDoc->DocNNatures], pSS->SsName,
		       MAX_NAME_LENGTH);
	      strncpy (pDoc->DocNaturePresName[pDoc->DocNNatures],
		       pSS->SsDefaultPSchema, MAX_NAME_LENGTH);
	      pDoc->DocNNatures++;
	    }
	  pPfS = pPfS->PfNext;
	}
    }
}

#define MAX_NAT_TABLE 10
/*----------------------------------------------------------------------
   SearchNatures
   Met dans la table natureTable tous les schemas de structure utilise's
   par le document pDoc et leurs extensions et retourne dans natureTableLen
   le nombre d'entrees de la table.
  ----------------------------------------------------------------------*/
void SearchNatures (PtrDocument pDoc, PtrSSchema natureTable[MAX_NAT_TABLE],
		    int *natureTableLen)
{
  PtrDocSchemasDescr pPfS;
  PtrSSchema         pSS;

  *natureTableLen = 0;
  pPfS = NULL;
  if (pDoc && natureTable)
    {
      pPfS = pDoc->DocFirstSchDescr;
      while (pPfS)
	{
	  /* met le schema dans la table si elle n'est pas pleine */
	  if (*natureTableLen < MAX_NAT_TABLE)
	    {
              pSS = pPfS->PfSSchema;
	      natureTable[(*natureTableLen)++] = pSS;
	      /* met les extensions du schema dans la table */
	      while (pSS->SsNextExtens != NULL)
		{
		  pSS = pSS->SsNextExtens;
		  if (*natureTableLen < MAX_NAT_TABLE)
		    natureTable[(*natureTableLen)++] = pSS;
		}
	    }
	  pPfS = pPfS->PfNext;
	}
    }
}
#endif   /* NODISPLAY */
 
#define FORMATTED_VIEW 1
#define STRUCTURE_VIEW 2
#define ELEMENT_NAME_PBOX 1
#define VERT_LINE_PBOX 2
#define ATTR_NAME_PBOX 3
#define ATTR_VALUE_PBOX 4
#define EMPTY_PBOX 12
#define STRUCT_INDENT 15
#define DIST_BOTTOM 2

/*----------------------------------------------------------------------
   InsertXmlAtRules
   Add the presentation rules associated to the new attribute
  ----------------------------------------------------------------------*/
static void      InsertXmlAtRules (PtrPSchema pPS, int nAtRules)
{

  AttributePres *pAtPres;
  PtrPRule       prevPRule, pRule;

  GetAttributePres (&pAtPres);

  /* Create the new AttrPres and attach it to the attribute */
  pAtPres->ApElemType = 0;
  pAtPres->ApNextAttrPres = NULL;
  pAtPres->ApString[0] = EOS;

  pPS->PsAttrPRule->AttrPres[nAtRules] = pAtPres;
  pPS->PsNAttrPRule->Num[nAtRules] +=1;

  /* Create the pRules associated with this attribute */
   pRule = NULL;
   prevPRule = pAtPres->ApTextFirstPRule;

   /* Rule 'CreateBefore (AttrName)' */
   GetPresentRule (&pRule);
   if (pRule != NULL)
     {
       pRule->PrType = PtFunction;
       pRule->PrNextPRule = NULL;
       pRule->PrCond = NULL;
       pRule->PrViewNum = FORMATTED_VIEW;
       pRule->PrSpecifAttr = 0;
       pRule->PrLevel = 0;
       pRule->PrSpecifAttrSSchema = NULL;
       pRule->PrPresMode = PresFunction;
       pRule->PrPresFunction = FnCreateBefore;
       pRule->PrPresBoxRepeat = 0;
       pRule->PrExternal = 0;
       pRule->PrElement = 0;
       pRule->PrNPresBoxes = 1;
       pRule->PrPresBox[0] = ATTR_NAME_PBOX;
       pRule->PrPresBoxName[0] = EOS;
       /* Add the new rule into the chain */
       if (prevPRule == NULL)
	 pAtPres->ApTextFirstPRule = pRule;
       else
	 prevPRule->PrNextPRule = pRule;
       prevPRule = pRule;
     }

   /* Rule 'CreateBefore (AttrValue)' */
   GetPresentRule (&pRule);
   if (pRule != NULL)
     {
       pRule->PrType = PtFunction;
       pRule->PrNextPRule = NULL;
       pRule->PrCond = NULL;
       pRule->PrViewNum = FORMATTED_VIEW;
       pRule->PrSpecifAttr = 0;
       pRule->PrLevel = 0;
       pRule->PrSpecifAttrSSchema = NULL;
       pRule->PrPresMode = PresFunction;
       pRule->PrPresFunction = FnCreateBefore;
       pRule->PrPresBoxRepeat = 0;
       pRule->PrExternal = 0;
       pRule->PrElement = 0;
       pRule->PrNPresBoxes = 1;
       pRule->PrPresBox[0] = ATTR_VALUE_PBOX;
       pRule->PrPresBoxName[0] = EOS;
       /* Add the new rule into the chain */
       prevPRule->PrNextPRule = pRule;
     }
}

/*----------------------------------------------------------------------
   TtaAppendXMLAttribute
   Add a new XML-generic global attribute
  ----------------------------------------------------------------------*/
void    TtaAppendXmlAttribute (char *XMLName, AttributeType *attrType,
			       Document document)
{
  PtrSSchema           pSS;
  PtrPSchema           pPSch, pPSchExt;
  int                  i, size;
  PtrDocument          pDoc;
  PtrDocSchemasDescr   pPfS;
  PtrHandlePSchema     pHSP, pHSPNext;

  attrType->AttrTypeNum = -1;           /* -1 means failure */
  pSS = (PtrSSchema) attrType->AttrSSchema;
  if (pSS == NULL)
    return;

  pDoc = LoadedDocument[document - 1];
  pPfS = pDoc->DocFirstSchDescr;
  /* Search the associated presentation schema */
  pPSch = NULL;
#ifndef NODISPLAY
  while (pPfS && !pPSch)
    {
      if (pPfS->PfSSchema && (pPfS->PfSSchema == pSS))
	{
	  pPSch = pPfS->PfPSchema;
	  pHSP = pPfS->PfFirstPSchemaExtens;
	}
      pPfS = pPfS->PfNext;
    }
  
  if (pPSch == NULL)
    /* no presentation schema, failure */
    return;

  /* free all element and attribute inherit tables */
  for (i = 0; i < pSS->SsNRules; i++)
    {
      pPSch->PsNInheritedAttrs->Num[i] = 0;
      if (pPSch->PsInheritedAttr->ElInherit[i])
	{
	  TtaFreeMemory (pPSch->PsInheritedAttr->ElInherit[i]);
	  pPSch->PsInheritedAttr->ElInherit[i] = NULL;
	}
    }
  for (i = 0; i < pSS->SsNAttributes; i++)
    if (pPSch->PsComparAttr->CATable[i])
      {
	TtaFreeMemory (pPSch->PsComparAttr->CATable[i]);
	pPSch->PsComparAttr->CATable[i] = NULL;
      }
#endif

  /* extend the attribute table if it's full */
  if (pSS->SsNAttributes >= pSS->SsAttrTableSize)
    {
      /* add 10 new entries */
      size = pSS->SsNAttributes + 10;
      i = size * sizeof (PtrTtAttribute);
      pSS->SsAttribute = (TtAttrTable*) realloc (pSS->SsAttribute, i);

#ifndef NODISPLAY      
      /* extend all tables that map attributes */
      /* extend the main presentation schema */
      i = size * sizeof (PtrAttributePres);
      pPSch->PsAttrPRule = (AttrPresTable*) realloc (pPSch->PsAttrPRule, i);
      i = size * sizeof (int);
      pPSch->PsNAttrPRule = (NumberTable*) realloc (pPSch->PsNAttrPRule, i);
      i = size * sizeof (int);
      pPSch->PsNHeirElems = (NumberTable*) realloc (pPSch->PsNHeirElems, i);
      i = size * sizeof (int);
      pPSch->PsNComparAttrs = (NumberTable*) realloc (pPSch->PsNComparAttrs,i);
      i = size * sizeof (ComparAttrTable*);
      pPSch->PsComparAttr = (CompAttrTbTb*) realloc (pPSch->PsComparAttr, i);
#endif
      if (!pSS->SsAttribute
#ifndef NODISPLAY
	  || !pPSch->PsAttrPRule || !pPSch->PsNAttrPRule || !pPSch->PsNHeirElems
#endif
	  )
	{
	  TtaDisplaySimpleMessage (FATAL, LIB, TMSG_NO_MEMORY);
	  return;
	}
      else
	{
	  pSS->SsAttrTableSize = size;
	  for (i = pSS->SsNAttributes; i < size; i++)
	    pSS->SsAttribute->TtAttr[i] = NULL;
	}

#ifndef NODISPLAY      
      /* extend the associated extension schemas */
      pHSPNext = pHSP;
      while (pHSPNext)
	{
	  pPSchExt = pHSPNext->HdPSchema;
	  if (pPSchExt)
	    {
	      i = size * sizeof (PtrAttributePres);
	      pPSchExt->PsAttrPRule =
		(AttrPresTable*) realloc (pPSchExt->PsAttrPRule, i);
	      i = size * sizeof (int);
	      pPSchExt->PsNAttrPRule =
		(NumberTable*) realloc (pPSchExt->PsNAttrPRule, i);
	      i = size * sizeof (int);
	      pPSchExt->PsNHeirElems =
		(NumberTable*) realloc (pPSchExt->PsNHeirElems, i);
	      i = size * sizeof (int);
	      pPSchExt->PsNComparAttrs =
		(NumberTable*) realloc (pPSchExt->PsNComparAttrs,i);
	      i = size * sizeof (ComparAttrTable*);
	      pPSchExt->PsComparAttr =
		(CompAttrTbTb*) realloc (pPSchExt->PsComparAttr, i);
	      if (!pPSchExt->PsAttrPRule || !pPSchExt->PsNAttrPRule ||
		  !pPSchExt->PsNHeirElems)
		{
		  TtaDisplaySimpleMessage (FATAL, LIB, TMSG_NO_MEMORY);
		  return;
		}
	    }
	  /* next extension schema */
	  pHSPNext = pHSPNext->HdNextPSchema;
	}
#endif
    }

  /* Add a new attribute type */
  i = pSS->SsNAttributes;
  pSS->SsAttribute->TtAttr[i] = (PtrTtAttribute) malloc (sizeof (TtAttribute));
  if (pSS->SsAttribute->TtAttr[i] == NULL)
    {
      TtaDisplaySimpleMessage (FATAL, LIB, TMSG_NO_MEMORY);
      return;
    }
  strncpy (pSS->SsAttribute->TtAttr[i]->AttrName, XMLName, MAX_NAME_LENGTH);
  strncpy (pSS->SsAttribute->TtAttr[i]->AttrOrigName, XMLName, MAX_NAME_LENGTH);
  pSS->SsAttribute->TtAttr[i]->AttrGlobal = TRUE;
  pSS->SsAttribute->TtAttr[i]->AttrFirstExcept = 0;
  pSS->SsAttribute->TtAttr[i]->AttrLastExcept = 0;
  pSS->SsAttribute->TtAttr[i]->AttrType = AtTextAttr;

#ifndef NODISPLAY
  /* no presentation rule nor inherit tables for this new attribute */
  pPSch->PsAttrPRule->AttrPres[i] = NULL;
  pPSch->PsNAttrPRule->Num[i] = 0;
  pPSch->PsNHeirElems->Num[i] = 0;
  pPSch->PsNComparAttrs->Num[i] = 0;
  pPSch->PsComparAttr->CATable[i] = NULL;

  /* no presentation rule nor inherit tables in the extension schemas */
  pHSPNext = pHSP;
  while (pHSPNext)
    {
      pPSchExt = pHSPNext->HdPSchema;
      if (pPSchExt)
	{
	  pPSchExt->PsAttrPRule->AttrPres[i] = NULL;
	  pPSchExt->PsNAttrPRule->Num[i] = 0;
	  pPSchExt->PsNHeirElems->Num[i] = 0;
	  pPSchExt->PsNComparAttrs->Num[i] = 0;
	  pPSchExt->PsComparAttr->CATable[i] = NULL;
	}
      /* next extension schema */
      pHSPNext = pHSPNext->HdNextPSchema;
    }

  /* Initialize and insert the presentation rules */
  /* associated with this new attribute */
  InsertXmlAtRules (pPSch, pSS->SsNAttributes);
#endif

  /* Update the type number */
  pSS->SsNAttributes++;
  attrType->AttrTypeNum = pSS->SsNAttributes;
}

/*----------------------------------------------------------------------
  TtaGetXMLAttributeType
  ----------------------------------------------------------------------*/
void    TtaGetXmlAttributeType (char *XMLName, AttributeType *attrType)

{
   PtrSSchema    pSS;
   int           nbattr;
   ThotBool      found;

   found = FALSE;
   pSS = (PtrSSchema) attrType->AttrSSchema;
   for (nbattr = 0;  !found && nbattr < pSS->SsNAttributes; nbattr++)
     {
       if (strcmp (pSS->SsAttribute->TtAttr[nbattr]->AttrName, XMLName) == 0)
	 {
	   attrType->AttrTypeNum = nbattr + 1;
	   found = TRUE;
	 }
     }
}

#ifndef NODISPLAY
/*----------------------------------------------------------------------
   InsertAXmlPRule
   Add a specific presentation rule.
  ----------------------------------------------------------------------*/
static PtrPRule InsertAXmlPRule (PRuleType type,  int view, PresMode mode,
				 PtrPRule prevPRule, PtrPSchema pPSch,
				 int nSRule)
{
   PtrPRule     pRule;

   pRule = NULL;
   GetPresentRule (&pRule);
   if (pRule != NULL)
     {
       pRule->PrType = type;
       pRule->PrNextPRule = NULL;
       pRule->PrCond = NULL;
       pRule->PrViewNum = view;
       pRule->PrSpecifAttr = 0;
       pRule->PrLevel = 0;
       pRule->PrSpecifAttrSSchema = NULL;
       pRule->PrPresMode = mode;
   
       /* Add the new rule into the chain */
       if (prevPRule == NULL)
	 pPSch->PsElemPRule->ElemPres[nSRule] = pRule;
       else
	 prevPRule->PrNextPRule = pRule;
     }
   return (pRule);
}

/*----------------------------------------------------------------------
   InsertXmlPRules
   Add the presentation rules associated to the new element type
  ----------------------------------------------------------------------*/
static void    InsertXmlPRules (PtrPSchema pPSch, int nSRules)
{

  PtrPRule     prevPRule, pRule, nextPRule;

  /* First presention rule associated with this element type */
  prevPRule = pPSch->PsElemPRule->ElemPres[nSRules];

 /* Rule 'NoLine' view 2 */
  pRule = InsertAXmlPRule (PtFunction, STRUCTURE_VIEW, PresFunction,
			   prevPRule, pPSch, nSRules);
  if (pRule != NULL)
    {
      pRule->PrPresFunction = FnNoLine;
      pRule->PrPresBoxRepeat = 0;
      pRule->PrExternal = 0;
      pRule->PrElement = 0;
      prevPRule = pRule;
    }

 /* Rule 'CreateBefore(ElementName)' */
  pRule = InsertAXmlPRule (PtFunction, FORMATTED_VIEW, PresFunction,
			   prevPRule, pPSch, nSRules);
  if (pRule != NULL)
    {
      pRule->PrPresFunction = FnCreateBefore;
      pRule->PrPresBoxRepeat = 0;
      pRule->PrExternal = 0;
      pRule->PrElement = 0;
      pRule->PrNPresBoxes = 1;
      pRule->PrPresBox[0] = ELEMENT_NAME_PBOX;
      pRule->PrPresBoxName[0] = EOS;
      prevPRule = pRule;
    }
  
  /* Rule 'CreateWith(VerticalLine)' */
  pRule = InsertAXmlPRule (PtFunction, FORMATTED_VIEW, PresFunction,
			   prevPRule, pPSch, nSRules);
  if (pRule != NULL)
    {
      pRule->PrPresFunction = FnCreateWith;
      pRule->PrPresBoxRepeat = 0;
      pRule->PrExternal = 0;
      pRule->PrElement = 0;
      pRule->PrNPresBoxes = 1;
      pRule->PrPresBox[0] = VERT_LINE_PBOX;
      pRule->PrPresBoxName[0] = EOS;
      prevPRule = pRule;
    }

  /* Rule 'VertPos' view 2 */
  pRule = InsertAXmlPRule (PtVertPos, STRUCTURE_VIEW, PresImmediate,
			   prevPRule, pPSch, nSRules);
  if (pRule != NULL)
    {
      pRule->PrPosRule.PoPosDef = Top;
      pRule->PrPosRule.PoPosRef = Bottom;
      pRule->PrPosRule.PoDistUnit = UnRelative;
      pRule->PrPosRule.PoDistAttr = FALSE;
      pRule->PrPosRule.PoDistance = 0;
      pRule->PrPosRule.PoRelation = RlPrevious;
      pRule->PrPosRule.PoNotRel = FALSE;
      pRule->PrPosRule.PoUserSpecified = FALSE;
      pRule->PrPosRule.PoRefKind = RkPresBox;
      pRule->PrPosRule.PoRefIdent = 0;
      prevPRule = pRule;
    }

  /* Rule 'HorizPos' view 2 */
  pRule = InsertAXmlPRule (PtHorizPos, STRUCTURE_VIEW, PresImmediate,
			   prevPRule, pPSch, nSRules);
  if (pRule != NULL)
    {
      pRule->PrPosRule.PoPosDef = Left;
      pRule->PrPosRule.PoPosRef = Left;
      pRule->PrPosRule.PoDistUnit = UnRelative;
      pRule->PrPosRule.PoDistAttr = FALSE;
      pRule->PrPosRule.PoDistance = STRUCT_INDENT;
      pRule->PrPosRule.PoRelation = RlEnclosing;
      pRule->PrPosRule.PoNotRel = FALSE;
      pRule->PrPosRule.PoUserSpecified = FALSE;
      pRule->PrPosRule.PoRefKind = RkPresBox;
      pRule->PrPosRule.PoRefIdent = 0;
      prevPRule = pRule;
    }

  /* Rule 'Size' view 2 */
  pRule = InsertAXmlPRule (PtSize, STRUCTURE_VIEW, PresInherit,
			   prevPRule, pPSch, nSRules);
  if (pRule != NULL)
    {
      pRule->PrInheritMode = InheritParent;
      pRule->PrInhPercent = FALSE;
      pRule->PrInhAttr = FALSE;
      pRule->PrInhDelta = 0;
      pRule->PrMinMaxAttr = FALSE;
      pRule->PrInhMinOrMax = 0;
      pRule->PrInhUnit = UnRelative;
      prevPRule = pRule;
    }

  /* Rule 'LineBreak' view 1  */
  pRule = InsertAXmlPRule (PtLineBreak, FORMATTED_VIEW, PresImmediate,
			   prevPRule, pPSch, nSRules);
  if (pRule != NULL)
    {
      pRule->PrBoolValue = TRUE;
      prevPRule = pRule;
    }
}

/*----------------------------------------------------------------------
   TtaAddEmptyBox
   Add a generic rule to an element type
  ----------------------------------------------------------------------*/
void    TtaAddEmptyBox (Element el, Document document)

{
  PtrPRule            prevpRule, pRule;
  PtrElement          pEl;


  pEl = (PtrElement) el;
  prevpRule = pEl->ElFirstPRule;
  pRule = NULL;
  GetPresentRule (&pRule);

  /* Rule 'Createfirst(EmptyBox)' */
  if (pRule != NULL)
    {
      pRule->PrType = PtFunction;
      pRule->PrNextPRule = NULL;
      pRule->PrCond = NULL;
      pRule->PrViewNum = FORMATTED_VIEW;
      pRule->PrSpecifAttr = 0;
      pRule->PrLevel = 0;
      pRule->PrSpecifAttrSSchema = NULL;
      pRule->PrPresMode = PresFunction;
      pRule->PrPresFunction = FnCreateLast;
      pRule->PrPresBoxRepeat = 0;
      pRule->PrExternal = 0;
      pRule->PrElement = 0;
      pRule->PrNPresBoxes = 1;
      pRule->PrPresBox[0] = EMPTY_PBOX;
      pRule->PrPresBoxName[0] = EOS;
    }
  
  if (prevpRule != NULL)
    pRule->PrNextPRule = prevpRule;
  else
    pEl->ElFirstPRule =  pRule;
}

/*----------------------------------------------------------------------
   TtaGetXmlPRule
   Returns a presentation rule of a given type associated
   with a given element type.
   Return value:
   the presentation rule found, or NULL if the element type
   hasn't this type of presentation rule.
  ----------------------------------------------------------------------*/
PtrPRule    TtaGetXmlPRule (PtrPSchema pPSch, int nSRule, PRuleType PrType,
			    FunctionType FcType, int view)

{
  PtrPRule   prevPRule, pRule, nextPRule;
  ThotBool   found;

  /* First presention rule associated with this element type */
  pRule = pPSch->PsElemPRule->ElemPres[nSRule];
  found = FALSE;
  while (pRule && !found)
    {
      if (pRule->PrType == PrType &&
	  (PrType == PtFunction &&
	   pRule->PrPresFunction == FcType) &&
	  pRule->PrViewNum == view)
	found = TRUE;
      if (!found)
	pRule = pRule->PrNextPRule;
    }
  return pRule;
}

/*----------------------------------------------------------------------
   TtaIsXmlTypeInLine
   Retuns TRUE if the element type has a 'Line' presentation rule
  ----------------------------------------------------------------------*/
ThotBool    TtaIsXmlTypeInLine (ElementType elType, Document document)

{
  PtrSSchema          pSS;
  PtrPSchema          pPSch;
  PtrDocument         pDoc;
  PtrDocSchemasDescr  pPfS;
  PtrPRule            prevPRule, pRule;
  int                 nSRule;

  pSS = NULL;
  pPSch = NULL;
  pDoc = LoadedDocument[document - 1];
  pPfS = pDoc->DocFirstSchDescr;
  pSS = (PtrSSchema) elType.ElSSchema;

  /* Search the associated presentation schema */
  while (pSS && pPfS && !pPSch)
    {
      if (pPfS->PfSSchema &&
	  (pPfS->PfSSchema == pSS))
	pPSch = pPfS->PfPSchema;
      pPfS = pPfS->PfNext;
    }

  if (pSS == NULL || pPSch == NULL)
    return FALSE;

  nSRule = elType.ElTypeNum - 1;
  /* Does this rule exist ? */
  pRule = TtaGetXmlPRule (pPSch, nSRule, PtFunction, FnLine, FORMATTED_VIEW);
  if (pRule != NULL)
    return TRUE;
  else
    return FALSE;

}

/*----------------------------------------------------------------------
   TtaSetXmlTypeInLine
   Add a generic rule to an element type
  ----------------------------------------------------------------------*/
void    TtaSetXmlTypeInLine (ElementType elType, Document document)

{
  PtrSSchema          pSS;
  PtrPSchema          pPSch;
  PtrDocument         pDoc;
  PtrDocSchemasDescr  pPfS;
  PtrPRule            prevPRule, pRule;
  int                 nSRule;

  pSS = NULL;
  pPSch = NULL;
  pDoc = LoadedDocument[document - 1];
  pPfS = pDoc->DocFirstSchDescr;
  pSS = (PtrSSchema) elType.ElSSchema;

  /* Search the associated presentation schema */
  while (pSS && pPfS && !pPSch)
    {
      if (pPfS->PfSSchema &&
	  (pPfS->PfSSchema == pSS))
	pPSch = pPfS->PfPSchema;
      pPfS = pPfS->PfNext;
    }

  if (pSS == NULL || pPSch == NULL)
    return;

  nSRule = elType.ElTypeNum - 1;
  /* Does this rule already exist ? */
  pRule = TtaGetXmlPRule (pPSch, nSRule, PtFunction, FnLine, FORMATTED_VIEW);
  if (pRule != NULL)
    return;

  /* First presentation rule associated with this element type */
  prevPRule = pPSch->PsElemPRule->ElemPres[nSRule];

  /* Rule 'Line' view 1 */
   pRule = NULL;
   GetPresentRule (&pRule);
   if (pRule != NULL)
     {
       pRule->PrType = PtFunction;
       pRule->PrNextPRule = NULL;
       pRule->PrCond = NULL;
       pRule->PrViewNum = FORMATTED_VIEW;
       pRule->PrSpecifAttr = 0;
       pRule->PrLevel = 0;
       pRule->PrSpecifAttrSSchema = NULL;
       pRule->PrPresMode = PresFunction;
       pRule->PrPresFunction = FnLine;
       pRule->PrPresBoxRepeat = 0;
       pRule->PrExternal = 0;
       pRule->PrElement = 0;
    
       /* Add the new rule into the chain */
       pPSch->PsElemPRule->ElemPres[nSRule] = pRule;
       pRule->PrNextPRule = prevPRule; 
     }
}
#endif

/*----------------------------------------------------------------------
   TtaAppendXMLElement
   Add a new rule at the end of the rule table
  ----------------------------------------------------------------------*/
void    TtaAppendXmlElement (char *XMLName, ElementType *elType,
			     char **mappedName, Document document)
{
  PtrSSchema          pSS;
  PtrPSchema          pPSch;
  PtrDocument         pDoc;
  PtrDocSchemasDescr  pPfS;
  PtrSRule            pRule;
  int                 rule;

  pSS = NULL;
  pPSch = NULL;
  pDoc = LoadedDocument[document - 1];
  pPfS = pDoc->DocFirstSchDescr;

  pSS = (PtrSSchema) elType->ElSSchema;
  if (pSS == NULL)
    return;

#ifndef NODISPLAY
  /* Search the associated presentation schema */
  while (pSS && pPfS && !pPSch)
    {
      if (pPfS->PfSSchema &&
	  (pPfS->PfSSchema == pSS))
	pPSch = pPfS->PfPSchema;
      pPfS = pPfS->PfNext;
    }
  if (pPSch == NULL)
    return;
#endif
  AppendSRule (&rule, pSS, pPSch, pDoc);

  if (rule == 0)
    *mappedName = NULL;
  else
    {
      /* Initializes a new structure rule */
      pRule = pSS->SsRule->SrElem[rule -1];
      strncpy (pRule->SrName, XMLName, MAX_NAME_LENGTH);
      strncpy (pRule->SrOrigName, XMLName, MAX_NAME_LENGTH);
      pRule->SrNDefAttrs = 0;
      pRule->SrNLocalAttrs = 0;
      pRule->SrLocalAttr = NULL;
      pRule->SrRequiredAttr = NULL;
      pRule->SrUnitElem = FALSE;
      pRule->SrRecursive = FALSE;
      pRule->SrExportedElem = FALSE;
      pRule->SrFirstExcept = 0;
      pRule->SrLastExcept = 0;
      pRule->SrNInclusions = 0;
      pRule->SrNExclusions = 0;
      pRule->SrRefImportedDoc = FALSE;
      pRule->SrSSchemaNat = NULL;
      pRule->SrConstruct = CsAny;

      *mappedName = pRule->SrName;

#ifndef NODISPLAY
      /* Initialize and insert the presentation rules */
      /* associed to this new element type */
      InsertXmlPRules (pPSch, rule - 1);
#endif

      /* Update the type number */
      elType->ElTypeNum = rule;
    }
}

/*----------------------------------------------------------------------
  TtaGetXMLElementType
  If elType->ElSSchema is not NULL, search in that specific schema,
  otherwise search in the different generic schemas loaded for the document
  ----------------------------------------------------------------------*/
void    TtaGetXmlElementType (char* XMLName, ElementType *elType,
			      char** mappedName, Document doc)
{
  PtrSSchema          pSS;
  int                 rule;
  ThotBool            found;
  PtrDocument         pDoc;
  PtrDocSchemasDescr  pPfS;

   found = FALSE;

   if (elType->ElSSchema)
     {
       pSS = (PtrSSchema) elType->ElSSchema;
       for (rule = 0; !found && rule < pSS->SsNRules; rule++)
	 {
 	   if (strcmp (pSS->SsRule->SrElem[rule]->SrName, XMLName) == 0)
	     {     
	       elType->ElTypeNum = rule + 1;
	       *mappedName = pSS->SsRule->SrElem[rule]->SrName;
	       found = TRUE;
	     }
	 }
     }
   else
     {
       pSS = NULL;
       pDoc = LoadedDocument[doc - 1];
       pPfS = pDoc->DocFirstSchDescr;
       while (pPfS && !found)
	 {
	   if (pPfS->PfSSchema)
	     {
	       pSS = (PtrSSchema) pPfS->PfSSchema;
	       if (strcmp (pSS->SsName, "HTML") &&
		   strcmp (pSS->SsName, "MathML") &&
		   strcmp (pSS->SsName, "SVG") &&
		   strcmp (pSS->SsName, "XLink") &&
		   strcmp (pSS->SsName, "Annot"))
		 {
		   for (rule = 0; !found && rule < pSS->SsNRules; rule++)
		     {
		       if (strcmp (pSS->SsRule->SrElem[rule]->SrName, XMLName) == 0)
			 {
			   elType->ElTypeNum = rule + 1;
			   elType->ElSSchema = (SSchema) pSS;
			   found = TRUE;
			 }
		     }
		 }
	     }
	   pPfS = pPfS->PfNext;
	 }
     }
}

/*----------------------------------------------------------------------
  TtaChangeGenericSchemaNames
  ----------------------------------------------------------------------*/
void TtaChangeGenericSchemaNames (char *sSchemaUri, char *sSchemaName,
				  Document document)

{
  PtrDocument         pDoc;
  PtrSSchema          pSS;
  PtrPSchema          pPSch;
  PtrDocSchemasDescr  pPfS;
  PtrPRule            nextPRule;
  int                 i;

  pDoc = LoadedDocument[document - 1];
  pSS = NULL;
  pPSch = NULL;
  pPfS = pDoc->DocFirstSchDescr;

  /* Search the appropriate schemas */
  while (pPfS && !pSS)
    {
      if (pPfS->PfSSchema &&
	  (strcmp ("XML", pPfS->PfSSchema->SsName) == 0))
	{
	  pSS = pPfS->PfSSchema;
	  pPSch = pPfS->PfPSchema;
	}
      pPfS = pPfS->PfNext;
    }

  if (pSS != NULL)
    {
      /* Modify the structure schema name */
      /* ***
      if (sSchemaUri != NULL && pSS->SsUriName == NULL)
	{
	  pSS->SsUriName = TtaGetMemory (strlen (sSchemaUri) + 1);
	  strcpy (pSS->SsUriName, sSchemaUri);
	}
      if (sSchemaName == NULL)
	{
	  pSS->SsName[0] = EOS;
	  strcpy (pSS->SsDefaultPSchema, "Unknown");
	  strcat (pSS->SsDefaultPSchema, "P");
	}
      else
	{
	  strncpy (pSS->SsName, sSchemaName, MAX_NAME_LENGTH);
	  strcpy (pSS->SsDefaultPSchema, sSchemaName);
	  strcat (pSS->SsDefaultPSchema, "P");
	  for (i = 0; i < pSS->SsNRules; i++)
	    if (strcmp (pSS->SsRule->SrElem[i]->SrName, "XML") == 0)
	      {
		strncpy (pSS->SsRule->SrElem[i]->SrName, sSchemaName, MAX_NAME_LENGTH);
		i = pSS->SsNRules;
	      }
	}
	*** */


      /* Update the LoadedSSchema table */
      for (i = 0; i < MAX_SSCHEMAS &&
	     strcmp ("XML", LoadedSSchema[i].StructSchemaName); i++);
      if (i < MAX_SSCHEMAS)
	{
	  /* The generic schema is found in the table, modify its name */
	  /* **
	     if (sSchemaName != NULL)
	    strncpy (LoadedSSchema[i].StructSchemaName, sSchemaName,
		     MAX_NAME_LENGTH);
	  else
	    LoadedSSchema[i].StructSchemaName[0] = EOS;
	  *** */
	}

      /* Update the LoadedPSchema table */
#ifndef NODISPLAY
      for (i = 0; i < MAX_SSCHEMAS &&
	     strcmp ("XMLP", LoadedPSchema[i].PresSchemaName); i++);
      if (i < MAX_SSCHEMAS)
	{
	  /* The generic schema is found in the table, modify its name */
	  /* ***
	  if (sSchemaName != NULL)
	    strncpy (LoadedPSchema[i].PresSchemaName, sSchemaName,
		     MAX_NAME_LENGTH);
	  else
	    LoadedPSchema[i].PresSchemaName[0] = EOS;
	    *** */
	}
#endif
    }
}

/*----------------------------------------------------------------------
   WrText export in the fileDescriptor file the content of the text list
   buffer, pBT is the first one.  
   length: max length to export.                         
  ----------------------------------------------------------------------*/
static void WrText (PtrTextBuffer pBT, int length, FILE *fileDescriptor)
{
   PtrTextBuffer       b;
   int                 i, l;

   l = 0;
   b = pBT;

   /* export the text buffer content */
   while (b != NULL)
     {
       i = 0;
       while (i < b->BuLength && b->BuContent[i] != EOS)
	 {
	   if (l > length)
	     {
	       /* generate a CR */
	       putc (__CR__, fileDescriptor);
	       l = 0;
	     }
	   putc (b->BuContent[i], fileDescriptor);
	   i++;
	   l++;
	 }
       /* following text buffer buffer for the same element */
       b = b->BuNext;
     }
}

/*----------------------------------------------------------------------
   ExportXmlDoc
  ----------------------------------------------------------------------*/
void ExportXmlDoc (PtrDocument pDoc, PtrElement pNode, int indent,
		   FILE *fileDescriptor, ThotBool premierfils)
{
  PtrElement          f;
  PtrSRule            pRe1;
  PtrAttribute        pAttr;
  PtrTtAttribute      pAttr1;
  ElementType        *elType;
  char                text[100];
  char                startName[MAX_NAME_LENGTH+1];
  char                endName[MAX_NAME_LENGTH+3];
  int                 i;
  ThotBool            specialTag;

  if (pNode != NULL)
    {
      /* Indentation white-spaces */
      for (i = 1; i <= indent; i++)
	fprintf (fileDescriptor, " ");
      i = 1;

      if (!pNode->ElTerminal)
	{
	  /* don't export Document element */
	  if (pNode == pDoc->DocDocElement)
	    {
	      fprintf (fileDescriptor, "<?xml version=\"1.0\" encoding=\"iso-8859-1\"?>");
	    }
	  else
	    {
	      specialTag = FALSE;
	      /* export element name */
	      pRe1 = pNode->ElStructSchema->SsRule->SrElem[pNode->ElTypeNumber - 1];
	      if (strcmp (pRe1->SrOrigName, "XMLcomment") == 0)
		{
		  strcpy (startName, "\n<!--");
		  strcpy (endName, "-->");
		  specialTag = TRUE;
		}
	      else if (strcmp (pRe1->SrOrigName, "XMLPI") == 0)
		{
		  strcpy (startName, "\n<?");
		  strcpy (endName, "?>");
		  specialTag = TRUE;
		}
	      else if ((strcmp (pRe1->SrOrigName, "XMLcomment_line") == 0) ||
		       (strcmp (pRe1->SrOrigName, "XMLPI_line") == 0))
		{
		  startName[0] = EOS;
		  endName[0] = EOS;
		}
	      else
		{
		  strcpy (startName, "\n<");
		  strcat (startName, pRe1->SrOrigName);
		  strcpy (endName, "</");
		  strcat (endName, pRe1->SrOrigName);
		  strcat (endName, ">");
		}
	      fprintf (fileDescriptor, "%s", startName);
	      
	      /* export element attributes */
	      pAttr = pNode->ElFirstAttr;
	      if (pAttr != NULL)
		fprintf (fileDescriptor, " ");
	      while (pAttr != NULL)
		{
		  pAttr1 = pAttr->AeAttrSSchema->SsAttribute->TtAttr[pAttr->AeAttrNum-1];
		  fprintf (fileDescriptor, "%s=", pAttr1->AttrOrigName);
		  switch (pAttr1->AttrType)
		    {
		    case AtNumAttr:
		      fprintf (fileDescriptor, "\"%d\"", pAttr->AeAttrValue);
		      break;
		    case AtTextAttr:
		      if (pAttr->AeAttrText)
			{
			  CopyBuffer2MBs (pAttr->AeAttrText, 0, text, 99);
			  fprintf (fileDescriptor, "\"%s\"", text);
			  if (pAttr->AeAttrText->BuNext)
			    fprintf (fileDescriptor, "...");
			}
		      break;
		    case AtEnumAttr:
		      fprintf (fileDescriptor, "\"%s\"",
			       pAttr1->AttrEnumValue[pAttr->AeAttrValue - 1]);
		      break;
		    default:
		      break;
		    }
		  if (pAttr->AeNext != NULL)
		    fprintf (fileDescriptor, ", ");
		  pAttr = pAttr->AeNext;
		}
	      if ((startName[0] != EOS) && !specialTag)
		fprintf (fileDescriptor, ">");
	    }
	  
	  /* element children */
	  f = pNode->ElFirstChild;
	  while (f != NULL)
	    {
	      ExportXmlDoc (pDoc, f, indent, fileDescriptor, premierfils);
	      /* ExportXmlDoc (f, indent + 2, fileDescriptor, premierfils); */
	      if (!premierfils)
		f = f->ElNext;
	      else
		f = NULL;
	    }
	  if (pNode != pDoc->DocDocElement)
	    fprintf (fileDescriptor, "%s", endName);
	}
      else
	{
	  /* terminal element */
	  for (i = 1; i <= indent; i++)
	    fprintf (fileDescriptor, " ");
	  switch (pNode->ElLeafType)
	    {
	    case LtPicture:
	      WrText (pNode->ElText, 72 - indent, fileDescriptor);
	      break;
	    case LtText:
	      WrText (pNode->ElText, 72 - indent, fileDescriptor);
	      break;
	    default:
	      break;	
	    }
	}
    }
}
