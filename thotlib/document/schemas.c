/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2009
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
  Chargement et liberation des schemas de structure et de presentation
*/
#include "thot_gui.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "libmsg.h"
#include "message.h"
#include "application.h"

#ifndef NODISPLAY
#include "fileaccess.h"
#endif /* NODISPLAY */

typedef struct _AStructure
{
  PtrSSchema      pStructSchema;    /* pointeur sur le schema */
  int             UsageCount;       /* nombre d'utilisations de ce schema */
  char           *StructSchemaName; /* nom du schema de structure */
}
AStructure;

#ifndef NODISPLAY
typedef struct _APresentation
{
  PtrPSchema      pPresSchema;    /* pointeur sur le schema */
  int             UsageCount;     /* nombre d'utilisations de ce schema */
  char           *PresSchemaName; /* nom du schema de presentation */
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

/******* replace the next two tables by unlimited lists ******/
/* table des schemas de presentation charges */
#define MAX_PSCHEMAS 50		/* max. number of loaded presentation schemas*/
static APresentation LoadedPSchema[MAX_PSCHEMAS];
#endif /* NODISPLAY */

/* table des schemas de structure charge's */
#define MAX_SSCHEMAS 50		/* max. number of loaded structure schemas */
static AStructure LoadedSSchema[MAX_SSCHEMAS];

#include "config_f.h"
#include "content_f.h"
#include "memory_f.h"
#include "readstr_f.h"
#include "schemas_f.h"
#include "tree_f.h"

#ifndef NODISPLAY
#include "readprs_f.h"
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
      LoadedSSchema[i].StructSchemaName = NULL;
    }
#ifndef NODISPLAY
  FirstSchemaUsedBySavedElements = NULL;
  for (i = 0; i < MAX_PSCHEMAS; i++)
    {
      LoadedPSchema[i].pPresSchema = NULL;
      LoadedPSchema[i].UsageCount = 0;
      LoadedPSchema[i].PresSchemaName = NULL;
    }
#endif /* NODISPLAY */
}

/*----------------------------------------------------------------------
  GetSSchemaForDoc
  Return the structure schema called name used by document pDoc.
  Return NULL if this document does not use this structure schema.
  ----------------------------------------------------------------------*/
PtrSSchema GetSSchemaForDoc (const char *name, PtrDocument pDoc)
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
PtrSSchema GetSSchemaByUriForDoc (const char *uriName, PtrDocument pDoc)
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
  Return the block describing structure schema pSS used by document pDoc
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
              TtaFreeMemory (pStr->StructSchemaName);
              pStr->StructSchemaName = NULL;
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
static void FreePRuleList (PtrPRule *firstPRule, PtrSSchema pSS)
{
  PtrPRule            pPRule, pNextPRule;

  pPRule = *firstPRule;
  while (pPRule != NULL)
    {
      pNextPRule = pPRule->PrNextPRule;
      FreePresentRule (pPRule, pSS);
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
                                       ThotBool force)
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
      if (pPres->UsageCount <= 0)
        /* This schema is no longer used by any document. Remove it from
           the table */
        {
          pPres->pPresSchema = NULL;
          TtaFreeMemory (pPres->PresSchemaName);
          pPres->PresSchemaName = NULL;
          force = TRUE;
        }
    }

  if (force)
    {
      /* libere les regles de presentation par defaut */
      FreePRuleList (&pPSchema->PsFirstDefaultPRule, pSS);
      /* libere les regles de presentation des boites de presentation */
      for (i = 0; i < pPSchema->PsNPresentBoxes; i++)
        FreePRuleList (&pPSchema->PsPresentBox->PresBox[i]->PbFirstPRule, pSS);
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
                    FreePRuleList (&pAttrPres->ApCase[j].CaFirstPRule, pSS);
                  break;
                case AtTextAttr:
                  FreePRuleList (&pAttrPres->ApTextFirstPRule, pSS);
                  break;
                case AtReferenceAttr:
                  FreePRuleList (&pAttrPres->ApRefFirstPRule, pSS);
                  break;
                case AtEnumAttr:
                  for (j = 0;
                       j <= pSS->SsAttribute->TtAttr[i]->AttrNEnumValues; j++)
                    FreePRuleList (&pAttrPres->ApEnumFirstPRule[j], pSS);
                  break;
                default:
                  break;
                }
              pAttrPres = pAttrPres->ApNextAttrPres;
            }
        }

      /* libere les regles de presentation des types */
      for (i = 0; i < pPSchema->PsNElemPRule; i++)
        FreePRuleList (&pPSchema->PsElemPRule->ElemPres[i], pSS);
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
ThotBool LoadPresentationSchema (const char *schemaName, PtrSSchema pSS,
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
  else if (!strcmp (schemaName, "XMLP"))
    // the name of the schema could be already changed
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
	       (LoadedPSchema[i].PresSchemaName == NULL ||
          strcmp (schemaName, LoadedPSchema[i].PresSchemaName)); i++);
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
              if (LoadedPSchema[i].PresSchemaName)
                TtaFreeMemory (LoadedPSchema[i].PresSchemaName);
              LoadedPSchema[i].PresSchemaName = (char *)TtaGetMemory (strlen (schemaName) + 1);
              strcpy (LoadedPSchema[i].PresSchemaName, schemaName);
            }
        }
    }
  if (pPSchema)
    {
      if (pSS->SsDefaultPSchema == NULL)
        pSS->SsDefaultPSchema = TtaStrdup (schemaName);
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
              ReleasePresentationSchema (pPfS->PfPSchema, pSS, FALSE);
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
  PtrHandlePSchema     pHd, pNextHd;

  ReleasePresentationSchema (pPSchema, pSS, FALSE);
  pPfS = StructSchemaForDoc (pDoc, pSS, &pPrevPfS);
  if (pPfS && pPfS->PfPSchema == pPSchema)
    /* it's the main presentation schema. Unlink it */
    {
      pPfS->PfPSchema = NULL;
      pHd = pPfS->PfFirstPSchemaExtens;
      while (pHd)
        {
          pNextHd = pHd->HdNextPSchema;
          /* release the extension schema */
          ReleasePresentationSchema (pHd->HdPSchema, pSS, TRUE);
          FreeHandleSchPres (pHd);
          pHd = pNextHd;
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
  structure schema pSS in document pDoc for element pEl.
  ----------------------------------------------------------------------*/
PtrHandlePSchema FirstPSchemaExtension (PtrSSchema pSS, PtrDocument pDoc,
                                        PtrElement pEl)
{
  PtrDocSchemasDescr  pPfS, pPrevPfS;

  if (pEl && pEl->ElFirstSchDescr)
    {
      // get the schema list attached to the element
      pPfS = pEl->ElFirstSchDescr;
      while (pPfS && pPfS->PfSSchema != pSS)
        pPfS = pPfS->PfNext;
    }
  else
    // get the schema list attached to the document
    pPfS = StructSchemaForDoc (pDoc, pSS, &pPrevPfS);
  if (pPfS)
    return (pPfS->PfFirstPSchemaExtens);
  else
    return NULL;
}

/*----------------------------------------------------------------------
  UnlinkPSchemaExtension unlinks a presentation schema pPs from the list
  of extensions associated to the document pDoc not NULL.
  ----------------------------------------------------------------------*/
void UnlinkPSchemaExtension (PtrDocument pDoc, PtrSSchema pSS, PtrPSchema pPS)
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
              ReleasePresentationSchema (pHd->HdPSchema, pSS, TRUE);
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
  SetElSchemasExtens associates or clears (NULL) the list of extension
  schemas used by the current element and all its children.
  ----------------------------------------------------------------------*/
void SetElSchemasExtens (PtrElement pEl, PtrDocSchemasDescr pPfS)
{
  PtrElement          pChild;

  if (pEl && pEl->ElStructSchema)
    {
      if (!pEl->ElTerminal)
        {
          pEl->ElFirstSchDescr = pPfS;
          pChild = pEl->ElFirstChild;
          while (pChild)
            {
              SetElSchemasExtens (pChild, pPfS);
              pChild = pChild->ElNext;
            }
        }
      else if (pEl->ElLeafType == LtPicture)
        pEl->ElFirstSchDescr = pPfS;
    }
}

/*----------------------------------------------------------------------
  InsertPSchemaExtension inserts the new presentation schema pPs after
  or before the old presentation schema pOldPs in the list of extensions.
  ----------------------------------------------------------------------*/
ThotBool InsertPSchemaExtension (PtrDocument pDoc, PtrSSchema pSS,
                                 PtrPSchema pPS, PtrPSchema pOldPS,
                                 ThotBool before)
{
  PtrDocSchemasDescr  pPfS, pPrevPfS;
  PtrHandlePSchema    oldHd, newHd;
  ThotBool            ok;

  /* get the schemas descriptor of the structure schema */
  pPfS = StructSchemaForDoc (pDoc, pSS, &pPrevPfS);
  if (!pPfS)
    return FALSE;
  ok = FALSE;
  /* look for the old presentation schema */
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
      /* the handle of the old presentation schema is found */
      GetHandleSchPres (&newHd);
      newHd->HdPSchema = pPS;
      if (oldHd == NULL)
        pPfS->PfFirstPSchemaExtens = newHd;
      else if (before)
        {
          /* add the new handle before */
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
          /* add the new handle after */
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
  UnlinkAllSchemasExtens unlinks the presentation schema used by
  element pEl.
  ----------------------------------------------------------------------*/
void UnlinkAllSchemasExtens (PtrElement pEl)
{
#ifndef NODISPLAY
  PtrDocSchemasDescr  pPfS, pNextPfS;
  PtrHandlePSchema    pHd, pNextHd;

  if (pEl->ElFirstSchDescr)
    {
      pPfS = pEl->ElFirstSchDescr;
      if (pEl->ElParent == NULL  || pEl->ElParent->ElFirstSchDescr == pPfS)
        /* don't remove schema extensions linked by enclosing elements */
        pPfS = NULL;
      /* remove the link in the whole hierarchy */
      SetElSchemasExtens (pEl, NULL);
      /* remove all descriptors */
      while (pPfS)
        {
          pNextPfS = pPfS->PfNext;
          /* free all handles */
          pHd = pPfS->PfFirstPSchemaExtens;
          while (pHd)
            {
              pNextHd = pHd->HdNextPSchema;
              /* release the extension schema */
              ReleasePresentationSchema (pHd->HdPSchema,
                                         pPfS->PfSSchema, TRUE);
              /* release the block */
              FreeHandleSchPres (pHd);
              pHd = pNextHd;
            }
          /* free the descriptor itself */
          FreeDocSchemasDescr (pPfS);
          pPfS = pNextPfS;
        }
    }
#endif /* NODISPLAY */
}

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
  If parameter schemaURI is not null, it is used as the namespace URI
  for the schema, instead of the schemaName to check whether the schema
  is already loaded.
  Return NULL if schema can't be loaded.
  ----------------------------------------------------------------------*/
PtrSSchema LoadStructureSchema (const char *schemaURI, const char * schemaName,
                                PtrDocument pDoc)
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
  if (schemaURI)
    /* check with the namespace URI */
    while (pPfS && pPfS->PfSSchema &&
           (pPfS->PfSSchema->SsUriName == NULL ||
            strcmp (schemaURI, pPfS->PfSSchema->SsUriName)))
      pPfS = pPfS->PfNext;
  else
    /* check with the schema name */
    while (pPfS && pPfS->PfSSchema &&
           strcmp (schemaName, pPfS->PfSSchema->SsName))
      pPfS = pPfS->PfNext;
  if (pPfS)
    /* this schema is already used by the document */
    return (pPfS->PfSSchema);

  /* Look at the table of all loaded schemas */
  if (schemaURI)
    /* check with the namespace URI */
    for (i = 0; i < MAX_SSCHEMAS &&
	         (LoadedSSchema[i].pStructSchema == NULL ||
            LoadedSSchema[i].pStructSchema->SsUriName == NULL ||
            strcmp (schemaURI, LoadedSSchema[i].pStructSchema->SsUriName)); i++);
  else
    /* check with the schema name */
    for (i = 0; i < MAX_SSCHEMAS &&
	         (LoadedSSchema[i].StructSchemaName == NULL ||
            strcmp (schemaName, LoadedSSchema[i].StructSchemaName)); i++);
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
              if (LoadedSSchema[i].StructSchemaName)
                TtaFreeMemory (LoadedSSchema[i].StructSchemaName);
              LoadedSSchema[i].StructSchemaName = (char *)TtaGetMemory (strlen (schemaName) + 1);
              strcpy (LoadedSSchema[i].StructSchemaName, schemaName);
            }
          else
            /* The schema table is full !!!! */
            fprintf (stderr, "Schema table full. %s not registered\n",
                     schemaName);
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
  ----------------------------------------------------------------------*/
void ReleaseStructureSchema (PtrSSchema pSS, PtrDocument pDoc)
{
  AStructure          *pStr;
  PtrDocSchemasDescr  pPfS, pPrevPfS;
  int                 i;

  /* look for this schema in the table */
  for (i = 0; i < MAX_SSCHEMAS && LoadedSSchema[i].pStructSchema != pSS; i++);
  if (i >= MAX_SSCHEMAS)
    /* error. This schema is not in the table */
    return;
  pStr = &LoadedSSchema[i];
  pStr->UsageCount--;
  if (pStr->UsageCount <= 0)
    /* This schema is no longer used by any document. Unload it */
    {
      pStr->pStructSchema = NULL;
      TtaFreeMemory (pStr->StructSchemaName);
      pStr->StructSchemaName = NULL;
      ResetNatureRules (pSS);
      FreeSchStruc (pSS);
    }

  pPfS = StructSchemaForDoc (pDoc, pSS, &pPrevPfS);
  if (pPfS)
    {
      pPfS->PfSSchema = NULL;
      if (pPfS->PfPSchema == NULL)
        {
          if (pPrevPfS)
            pPrevPfS->PfNext = pPfS->PfNext;
          else
            pDoc->DocFirstSchDescr = pPfS->PfNext;
          FreeDocSchemasDescr (pPfS);
        }
    }
}

/*----------------------------------------------------------------------
  LoadNatureSchema
  Charge la nature definie dans la regle rule du schema de structure pSS.
  Si le 1er octet de PSchName est nul on propose a l'utilisateur le schema
  de presentation par defaut defini dans le schema de structure, sinon on
  propose le schema de presentation de nom PSchName.
  ----------------------------------------------------------------------*/
void LoadNatureSchema (PtrSSchema pSS, const char *PSchName, int rule,
                        const char *schURI, PtrDocument pDoc)
{
  char          *schName;
  PtrSSchema     pNatureSS;
#ifndef NODISPLAY
  ThotBool       loaded;
  char          *pUriName = NULL;
  int            l;
#endif

  schName = NULL;
  /* utilise le nom de la nature comme nom de fichier. */
  /* cree un schema de structure et le charge depuis le fichier */
  pNatureSS = LoadStructureSchema (schURI, pSS->SsRule->SrElem[rule-1]->SrOrigNat, pDoc);
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
          if (schURI != NULL && !strcmp (PSchName, "XMLP"))
            {
              /* load the presentation schema with the right name */
              l = strlen (pNatureSS->SsName);
              pUriName = (char *) TtaGetMemory (l + 2);
              strcpy (pUriName, pNatureSS->SsName);
              strcat (pUriName, "P");
              loaded = LoadPresentationSchema (pUriName, pNatureSS, pDoc);
              TtaFreeMemory (pUriName);
            }
          else
            {
              schName = TtaStrdup (PSchName);
              loaded = LoadPresentationSchema (PSchName, pNatureSS, pDoc);
            }
        }
      if (PSchName == NULL || PSchName[0] == EOS || !loaded)
        /* pas de schema de presentation particulier demande' par l'appelant*/
        /* ou schema demande' inaccessible */
        {
          /* on consulte le fichier .conf */
          if (!ConfigGetPSchemaNature (pSS, pSS->SsRule->SrElem[rule - 1]->SrOrigNat,
                                       &schName))
            /* le fichier .conf ne donne pas de schema de presentation pour */
            /* cette nature, on prend le schema par defaut */
            schName = TtaStrdup (pNatureSS->SsDefaultPSchema);
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
      else if (ThotLocalActions[T_initevents] != NULL)
        (*(Proc1)ThotLocalActions[T_initevents]) (pNatureSS);
#endif /* NODISPLAY */
    }
  if (schName)
    TtaFreeMemory (schName);
}

/*----------------------------------------------------------------------
  AppendSRule
  Append a new structure rule to structure schema pSS for document pDoc.
  pPSch is the presentation schema associated with pSS for document pDoc.
  ----------------------------------------------------------------------*/
static void AppendSRule (int *ret, PtrSSchema pSS, PtrPSchema pPSch,
                         PtrDocument pDoc)
{
  int                  size, i;
#ifndef NODISPLAY
  PtrDocSchemasDescr   pPfS;
  PtrPSchema           pPSchExt;
  PtrHandlePSchema     pHSP, pHSPNext;
#endif

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
          || (pPSch && !pPSch->PsElemPRule)
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
  pSS->SsRule->SrElem[i]->SrName = NULL;
  pSS->SsRule->SrElem[i]->SrOrigName = NULL;
  memset (pSS->SsRule->SrElem[i], 0, sizeof (SRule));
#ifndef NODISPLAY
  if (pPSch)
    {
      pPSch->PsElemPRule->ElemPres[i] = NULL;
      pPSch->PsNInheritedAttrs->Num[i] = 0;
      pPSch->PsInheritedAttr->ElInherit[i] = NULL;
      pPSch->PsElemTransmit->Num[i] = 0;
      pPSch->PsNElemPRule = i+1;
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
          pPSchExt->PsNElemPRule = i+1;
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
int CreateNature (const char *SSchURI, const char *SSchName, const char *PSchName,
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
                 ((pSS->SsRule->SrElem[ret - 1]->SrName != NULL) ||
                  (pSS->SsRule->SrElem[ret - 1]->SrSSchemaNat != NULL)))
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
          pRule->SrOrigNat = TtaStrdup (SSchName);
          pRule->SrName = TtaStrdup (SSchName);
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
            LoadNatureSchema (pSS, PSchName, ret, SSchURI, pDoc);
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
              pSSch = LoadStructureSchema (SSchURI, SSchName, pDoc);
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
  char         *schName;
#ifndef NODISPLAY
  ThotBool     loaded;
#endif

  schName = NULL;
  /* cree le schema de structure et charge le fichier dedans */
  if (pLoadedSS == NULL)
    {
      *pSS = LoadStructureSchema (NULL, SSchName, pDoc);
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
                (*(Proc1)ThotLocalActions[T_initevents]) (*pSS);
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
          schName = TtaStrdup (PSchName);
          loaded = LoadPresentationSchema (schName, *pSS, pDoc);
        }
      if (PSchName == NULL || PSchName[0] == EOS || !loaded)
        {
          /* pas de presentation specifiee par l'appelant, ou schema specifie'
             inaccessible */
          /* on consulte le fichier de configuration */
          if (!ConfigDefaultPSchema ((*pSS)->SsName, &schName))
            /* le fichier de configuration ne dit rien, on demande a
               l'utilisateur */
            /* propose la presentation par defaut definie dans le schema de
               structure */
            schName = TtaStrdup ((*pSS)->SsDefaultPSchema);
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
  if (schName)
    TtaFreeMemory (schName);
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
            ConfigGetPSchemaNature (pDoc->DocSSchema, SSchName, &PSchName);
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
  Returns FALSE when no Nature was found.
  ----------------------------------------------------------------------*/
static ThotBool FreeNatureRules (PtrSSchema pSS, PtrSSchema pNatureSS)
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
  if (FreeNatureRules (pSS, pNatureSS))
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
void FreeDocumentSchemas (PtrDocument pDoc)
{
  PtrSSchema          pSS;
  PtrDocSchemasDescr  pPfS;

  if (pDoc == NULL)
    return;
  while (pDoc->DocFirstSchDescr)
    {
      pPfS = pDoc->DocFirstSchDescr;
      pSS = pPfS->PfSSchema;
      if (pSS)
        {
#ifndef NODISPLAY
          if (pPfS->PfPSchema)
            FreePresentationSchema (pPfS->PfPSchema, pSS, pDoc);
#endif
          ReleaseStructureSchema (pSS, pDoc);
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
            pGuestView = (PtrGuestViewDescr)TtaGetMemory (sizeof(GuestViewDescr));
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
void CreateGuestViewList (PtrDocument pDoc, int view)
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
void BuildDocNatureTable (PtrDocument pDoc)
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
          if (pSS)
            {
              pDoc->DocNatureSSchema[pDoc->DocNNatures] = pSS;
              if (pDoc->DocNatureName[pDoc->DocNNatures])
                TtaFreeMemory (pDoc->DocNatureName[pDoc->DocNNatures]);
              pDoc->DocNatureName[pDoc->DocNNatures] = TtaStrdup (pSS->SsName);
              if (pDoc->DocNaturePresName[pDoc->DocNNatures])
                TtaFreeMemory (pDoc->DocNaturePresName[pDoc->DocNNatures]);
              pDoc->DocNaturePresName[pDoc->DocNNatures] = TtaStrdup (pSS->SsDefaultPSchema);
              pDoc->DocNNatures++;
              /* met les extensions du schema dans la table */
              while (pSS->SsNextExtens && pDoc->DocNNatures < MAX_NATURES_DOC)
                {
                  pSS = pSS->SsNextExtens;
                  pDoc->DocNatureSSchema[pDoc->DocNNatures] = pSS;
                  if (pDoc->DocNatureName[pDoc->DocNNatures])
                    TtaFreeMemory (pDoc->DocNatureName[pDoc->DocNNatures]);
                  pDoc->DocNatureName[pDoc->DocNNatures] = TtaStrdup (pSS->SsName);
                  if (pDoc->DocNaturePresName[pDoc->DocNNatures])
                    TtaFreeMemory (pDoc->DocNaturePresName[pDoc->DocNNatures]);
                  pDoc->DocNaturePresName[pDoc->DocNNatures] =
                    TtaStrdup (pSS->SsDefaultPSchema);
                  pDoc->DocNNatures++;
                }
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

#ifndef NODISPLAY
/*----------------------------------------------------------------------
  RuleBefore
  Return TRUE if pPRule1 must occur before pPRule2 in a chain of
  presentation rules.
  ----------------------------------------------------------------------*/
static ThotBool RuleBefore (PtrPRule pPRule1, PtrPRule pPRule2)
{
  ThotBool             ret;

  /* first criterion is the rule type */
  if (pPRule1->PrType != pPRule2->PrType)
    ret = pPRule1->PrType < pPRule2->PrType;
  else
    /* same type */
    if (pPRule1->PrType == PtFunction)
      {
        /* both rules are presentation functions, next criterion is */
        /* the function code. */
        if (pPRule1->PrPresFunction != pPRule2->PrPresFunction)
          /* same presentation function */
          ret = pPRule1->PrPresFunction < pPRule2->PrPresFunction;
        else
          /* same presentation function, the next criterion is the view
             number */
          ret = pPRule1->PrViewNum < pPRule2->PrViewNum;
      }
    else
      {
        /* It's not a presentation function, the next criterion is the
           view number */
        if (pPRule1->PrViewNum != pPRule2->PrViewNum)
          ret = pPRule1->PrViewNum < pPRule2->PrViewNum;
        else
          /* same type and same view: rule with no condition or with
             the default condition ("otherwise") must occur first */
          ret = (!pPRule1->PrCond ||
                 pPRule1->PrCond->CoCondition == PcDefaultCond);
      }
  return ret;
}

/*----------------------------------------------------------------------
  LinkNewPRule
  Insert presentation rule pRule at the right position in the chain
  of rules anchored at anchor.
  ----------------------------------------------------------------------*/
static void LinkNewPRule (PtrPRule pRule, PtrPRule *anchor)
{
  PtrPRule       prevPRule, curPRule;

  if (*anchor == NULL)
    /* the chain is currently empty. First rule */
    {
      *anchor = pRule;
      pRule->PrNextPRule = NULL;
    }
  else
    {
      prevPRule = NULL;
      curPRule = *anchor;
      while (curPRule && !RuleBefore (pRule, curPRule))
        {
          prevPRule = curPRule;
          curPRule = curPRule->PrNextPRule;
        }
      if (prevPRule == NULL)
        {
          pRule->PrNextPRule = *anchor;
          *anchor = pRule;
        }
      else
        {
          pRule->PrNextPRule = prevPRule->PrNextPRule;
          prevPRule->PrNextPRule = pRule;
        }
    }
}

/*----------------------------------------------------------------------
  InsertXmlAttrRules
  Add the presentation rules associated to the new attribute
  ----------------------------------------------------------------------*/
static void InsertXmlAttrRules (PtrPSchema pPS, int nAtRules)
{
  AttributePres *pAtPres;
  PtrPRule       pRule;

  GetAttributePres (&pAtPres);

  /* Create the new AttrPres and attach it to the attribute */
  pAtPres->ApElemType = 0;
  pAtPres->ApElemInherits = FALSE;
  pAtPres->ApNextAttrPres = NULL;
  pAtPres->ApString = NULL;

  pPS->PsAttrPRule->AttrPres[nAtRules] = pAtPres;
  pPS->PsNAttrPRule->Num[nAtRules] +=1;

  /* Create the pRules associated with this attribute */

  /* Rule 'CreateBefore (AttrName)' */
  GetPresentRule (&pRule);
  if (pRule != NULL)
    {
      pRule->PrType = PtFunction;
      pRule->PrNextPRule = NULL;
      pRule->PrCond = NULL;
      pRule->PrViewNum = FORMATTED_VIEW;
      pRule->PrSpecifAttr = 0;
      pRule->PrSpecificity = 0;
      pRule->PrImportant = FALSE;
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
      LinkNewPRule (pRule, &pAtPres->ApTextFirstPRule);
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
      pRule->PrSpecificity = 0;
      pRule->PrImportant = FALSE;
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
      LinkNewPRule (pRule, &pAtPres->ApTextFirstPRule);
    }
}
#endif

/*----------------------------------------------------------------------
  AppendXmlAttribute
  Add a new xml global attribute
  ----------------------------------------------------------------------*/
void AppendXmlAttribute (char *xmlName, AttributeType *attrType, PtrDocument pDoc)
{
  PtrDocSchemasDescr   pPfS;
  PtrSSchema           pSS;
  PtrPSchema           pPSch;
#ifndef NODISPLAY
  PtrPSchema           pPSchExt;
  PtrHandlePSchema     pHSP, pHSPNext;
#endif
  int                  i, size;

  attrType->AttrTypeNum = -1;           /* -1 means failure */
  pSS = (PtrSSchema) attrType->AttrSSchema;
  if (pSS == NULL)
    return;

  pPfS = pDoc->DocFirstSchDescr;
  /* Search the associated presentation schema */
  pPSch = NULL;
#ifndef NODISPLAY
  pHSP = NULL;
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
  for (i = 0; i < pPSch->PsNElemPRule; i++)
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
  pSS->SsAttribute->TtAttr[i]->AttrName = TtaStrdup (xmlName);
  pSS->SsAttribute->TtAttr[i]->AttrOrigName = TtaStrdup (xmlName);
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
  InsertXmlAttrRules (pPSch, pSS->SsNAttributes);
#endif

  /* Update the type number */
  pSS->SsNAttributes++;
  attrType->AttrTypeNum = pSS->SsNAttributes;
}

#ifndef NODISPLAY
/*----------------------------------------------------------------------
  InsertAXmlPRule
  Add a presentation rule for element type nSRule to Pres schema pPSch
  ----------------------------------------------------------------------*/
static PtrPRule InsertAXmlPRule (PRuleType type, int view, PresMode mode,
                                 PtrPSchema pPSch, int nSRule)
{
  PtrPRule     pRule;

  GetPresentRule (&pRule);
  if (pRule != NULL)
    {
      pRule->PrType = type;
      pRule->PrNextPRule = NULL;
      pRule->PrCond = NULL;
      pRule->PrViewNum = view;
      pRule->PrSpecifAttr = 0;
      pRule->PrSpecificity = 0;
      pRule->PrImportant = FALSE;
      pRule->PrSpecifAttrSSchema = NULL;
      pRule->PrPresMode = mode;
      /* Add the new rule into the chain */
      LinkNewPRule (pRule, &pPSch->PsElemPRule->ElemPres[nSRule]);
    }
  return (pRule);
}

/*----------------------------------------------------------------------
  InsertXmlPRules
  Add the presentation rules associated to the new element type
  ----------------------------------------------------------------------*/
static void InsertXmlPRules (PtrPSchema pPSch, int nSRules)
{

  PtrPRule     pRule;

  /* Rule 'NoLine' view 2 */
  pRule = InsertAXmlPRule (PtFunction, STRUCTURE_VIEW, PresFunction, pPSch,
                           nSRules);
  if (pRule != NULL)
    {
      pRule->PrPresFunction = FnNoLine;
      pRule->PrPresBoxRepeat = 0;
      pRule->PrExternal = 0;
      pRule->PrElement = 0;
    }

  /* Rule 'CreateBefore(ElementName)' */
  pRule = InsertAXmlPRule (PtFunction, FORMATTED_VIEW, PresFunction, pPSch,
                           nSRules);
  if (pRule != NULL)
    {
      pRule->PrPresFunction = FnCreateBefore;
      pRule->PrPresBoxRepeat = 0;
      pRule->PrExternal = 0;
      pRule->PrElement = 0;
      pRule->PrNPresBoxes = 1;
      pRule->PrPresBox[0] = ELEMENT_NAME_PBOX;
      pRule->PrPresBoxName[0] = EOS;
    }
  
  /* Rule 'CreateWith(VerticalLine)' */
  pRule = InsertAXmlPRule (PtFunction, FORMATTED_VIEW, PresFunction, pPSch,
                           nSRules);
  if (pRule != NULL)
    {
      pRule->PrPresFunction = FnCreateWith;
      pRule->PrPresBoxRepeat = 0;
      pRule->PrExternal = 0;
      pRule->PrElement = 0;
      pRule->PrNPresBoxes = 1;
      pRule->PrPresBox[0] = VERT_LINE_PBOX;
      pRule->PrPresBoxName[0] = EOS;
    }

  /* Rule 'VertPos' view 2 */
  pRule = InsertAXmlPRule (PtVertPos, STRUCTURE_VIEW, PresImmediate, pPSch,
                           nSRules);
  if (pRule != NULL)
    {
      pRule->PrPosRule.PoPosDef = Top;
      pRule->PrPosRule.PoPosRef = Bottom;
      pRule->PrPosRule.PoDistUnit = UnRelative;
      pRule->PrPosRule.PoDeltaUnit = UnRelative;
      pRule->PrPosRule.PoDistAttr = FALSE;
      pRule->PrPosRule.PoDistance = 0;
      pRule->PrPosRule.PoDistDelta = 0;
      pRule->PrPosRule.PoRelation = RlPrevious;
      pRule->PrPosRule.PoNotRel = FALSE;
      pRule->PrPosRule.PoUserSpecified = FALSE;
      pRule->PrPosRule.PoRefKind = RkPresBox;
      pRule->PrPosRule.PoRefIdent = 0;
    }

  /* Rule 'HorizPos' view 2 */
  pRule = InsertAXmlPRule (PtHorizPos, STRUCTURE_VIEW, PresImmediate, pPSch,
                           nSRules);
  if (pRule != NULL)
    {
      pRule->PrPosRule.PoPosDef = Left;
      pRule->PrPosRule.PoPosRef = Left;
      pRule->PrPosRule.PoDistUnit = UnRelative;
      pRule->PrPosRule.PoDeltaUnit = UnRelative;
      pRule->PrPosRule.PoDistAttr = FALSE;
      pRule->PrPosRule.PoDistance = STRUCT_INDENT;
      pRule->PrPosRule.PoDistDelta = 0;
      pRule->PrPosRule.PoRelation = RlEnclosing;
      pRule->PrPosRule.PoNotRel = FALSE;
      pRule->PrPosRule.PoUserSpecified = FALSE;
      pRule->PrPosRule.PoRefKind = RkPresBox;
      pRule->PrPosRule.PoRefIdent = 0;
    }

  /* Rule 'Size' view 2 */
  pRule = InsertAXmlPRule (PtSize, STRUCTURE_VIEW, PresInherit, pPSch,
                           nSRules);
  if (pRule != NULL)
    {
      pRule->PrInheritMode = InheritParent;
      pRule->PrInhPercent = FALSE;
      pRule->PrInhAttr = FALSE;
      pRule->PrInhDelta = 0;
      pRule->PrMinMaxAttr = FALSE;
      pRule->PrInhMinOrMax = 0;
      pRule->PrInhUnit = UnRelative;
    }

  /* Rule 'LineBreak' view 1  */
  pRule = InsertAXmlPRule (PtLineBreak, FORMATTED_VIEW, PresImmediate, pPSch,
                           nSRules);
  if (pRule != NULL)
    pRule->PrBoolValue = TRUE;
}

/*-----------------------------------------------------------------------------
  AddEmptyBox
  Add the specific presentation rule Createlast(EmptyBox) to an empty element
  -----------------------------------------------------------------------------*/
void AddEmptyBox (PtrElement element)

{
  PtrPRule            pRule;

  /* Rule 'Createlast(EmptyBox)' */
  GetPresentRule (&pRule);
  if (pRule != NULL)
    {
      pRule->PrType = PtFunction;
      pRule->PrNextPRule = NULL;
      pRule->PrCond = NULL;
      pRule->PrViewNum = FORMATTED_VIEW;
      pRule->PrSpecifAttr = 0;
      pRule->PrSpecificity = 100;
      pRule->PrImportant = FALSE;
      pRule->PrSpecifAttrSSchema = NULL;
      pRule->PrPresMode = PresFunction;
      pRule->PrPresFunction = FnCreateLast;
      pRule->PrPresBoxRepeat = 0;
      pRule->PrExternal = 0;
      pRule->PrElement = 0;
      pRule->PrNPresBoxes = 1;
      pRule->PrPresBox[0] = EMPTY_PBOX;
      pRule->PrPresBoxName[0] = EOS;
      LinkNewPRule (pRule, &(element->ElFirstPRule));
    }
}

/*----------------------------------------------------------------------
  GetXmlPRule
  Returns a presentation rule of a given type associated
  with a given element type.
  Return value:
  the presentation rule found, or NULL if the element type
  hasn't this type of presentation rule.
  ----------------------------------------------------------------------*/
PtrPRule GetXmlPRule (PtrPSchema pPSch, int nSRule, PRuleType PrType,
                      FunctionType FcType, int view)

{
  PtrPRule   pRule;
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
  HasXmlInLineRule
  Retuns TRUE if the element type has a 'Line' presentation rule
  ----------------------------------------------------------------------*/
ThotBool HasXmlInLineRule (ElementType elType, PtrDocument pDoc)

{
  PtrDocSchemasDescr  pPfS;
  PtrSSchema          pSS;
  PtrPSchema          pPSch;
  PtrPRule            pRule;
  int                 nSRule;

  pSS = NULL;
  pPSch = NULL;
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
  pRule = GetXmlPRule (pPSch, nSRule, PtFunction, FnLine, FORMATTED_VIEW);
  if (pRule != NULL)
    return TRUE;
  else
    return FALSE;

}

/*----------------------------------------------------------------------
  SetXmlInLineRule
  Add an InLine generic rule to an element type
  ----------------------------------------------------------------------*/
void SetXmlInLineRule (ElementType elType, PtrDocument pDoc)
{
  PtrDocSchemasDescr  pPfS;
  PtrSSchema          pSS;
  PtrPSchema          pPSch;
  PtrPRule            pRule;
  int                 nSRule;

  pSS = NULL;
  pPSch = NULL;
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
  pRule = GetXmlPRule (pPSch, nSRule, PtFunction, FnLine, FORMATTED_VIEW);
  if (pRule != NULL)
    return;

  /* Rule 'Line' view 1 */
  GetPresentRule (&pRule);
  if (pRule != NULL)
    {
      pRule->PrType = PtFunction;
      pRule->PrNextPRule = NULL;
      pRule->PrCond = NULL;
      pRule->PrViewNum = FORMATTED_VIEW;
      pRule->PrSpecifAttr = 0;
      pRule->PrSpecificity = 0;
      pRule->PrImportant = FALSE;
      pRule->PrSpecifAttrSSchema = NULL;
      pRule->PrPresMode = PresFunction;
      pRule->PrPresFunction = FnLine;
      pRule->PrPresBoxRepeat = 0;
      pRule->PrExternal = 0;
      pRule->PrElement = 0;
      /* Add the new rule into the chain */
      LinkNewPRule (pRule, &pPSch->PsElemPRule->ElemPres[nSRule]);
    }
}
#endif

/*----------------------------------------------------------------------
  AppendXmlElement
  Add a new element type to the schema
  ----------------------------------------------------------------------*/
void AppendXmlElement (const char *xmlName, int *typeNum, PtrSSchema pSS,
                       char **mappedName, PtrDocument pDoc)
{
  PtrDocSchemasDescr  pPfS;
  PtrPSchema          pPSch;
  PtrSRule            pRule;
  int                 rule;
#ifdef test
  int                 i;
  ThotBool            found;
#endif

  if (pSS == NULL)
    return;
  pPSch = NULL;
  pPfS = pDoc->DocFirstSchDescr;

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
#endif /* NODISPLAY */
  AppendSRule (&rule, pSS, pPSch, pDoc);

  if (rule == 0)
    *mappedName = NULL;
  else
    {
      /* Initializes a new rule structure */
      pRule = pSS->SsRule->SrElem[rule - 1];
      pRule->SrName = TtaStrdup (xmlName);
      pRule->SrOrigName = TtaStrdup (xmlName);
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

      *mappedName = pRule->SrOrigName;

#ifndef NODISPLAY
      /* Initialize and insert the presentation rules */
      /* associed to this new element type */
      InsertXmlPRules (pPSch, rule - 1);
#endif

      /* Update the type number */
      *typeNum = rule;
    }
}

/*----------------------------------------------------------------------
  GetXmlAttributeType
  Search in attrType->AttrSSchema if not NULL otherwise,
  search in the different loaded natures.
  ----------------------------------------------------------------------*/
void GetXmlAttributeType (char* xmlName, AttributeType *attrType, PtrDocument pDoc)
{
  PtrDocSchemasDescr  pPfS;
  PtrSSchema          pSS;
  ThotBool            found;
  int                 att;

  found = FALSE;
  attrType->AttrTypeNum = 0;
  if (attrType->AttrSSchema)
    {
      /* search in that schema */
      pSS = (PtrSSchema) attrType->AttrSSchema;
      for (att = 0; !found && att < pSS->SsNAttributes; att++)
        {
          if (strcmp (xmlName, "lang") == 0)
            {
              attrType->AttrTypeNum = 1;
              found = TRUE;
            }
          else if (pSS->SsAttribute->TtAttr[att]->AttrName != NULL &&
                   !strcmp (pSS->SsAttribute->TtAttr[att]->AttrName, xmlName))
            {
              attrType->AttrTypeNum = att + 1;
              found = TRUE;
            }
        }
    }
  else
    {
      pSS = NULL;
      /* search into the loaded natures */
      pPfS = pDoc->DocFirstSchDescr;
      while (pPfS && !found)
        {
          if (pPfS->PfSSchema)
            {
              pSS = (PtrSSchema) pPfS->PfSSchema;
              for (att = 0; !found && att < pSS->SsNAttributes; att++)
                {
                  if (strcmp (xmlName, "lang") == 0)
                    {
                      attrType->AttrTypeNum = 1;
                      found = TRUE;
                    }
                  else if (pSS->SsAttribute->TtAttr[att]->AttrName != NULL &&
                           !strcmp (pSS->SsAttribute->TtAttr[att]->AttrName, xmlName))
                    {
                      attrType->AttrTypeNum = att + 1;
                      attrType->AttrSSchema = (SSchema) pSS;
                      found = TRUE;
                    }
                }
            }
          pPfS = pPfS->PfNext;
        }
    }
}

/*----------------------------------------------------------------------
  GetXmlElementType
  Search in elType->ElSSchema if not NULL otherwise,
  search in the different loaded natures.
  ----------------------------------------------------------------------*/
void GetXmlElementType (const char *xmlName, ElementType *elType,
                        char **mappedName, PtrDocument pDoc)
{
  PtrDocSchemasDescr  pPfS;
  PtrSSchema          pSS;
  ThotBool            found;
  int                 rule;

  found = FALSE;
  elType->ElTypeNum = 0;
  if (elType->ElSSchema)
    {
      /* search within this schema */
      pSS = (PtrSSchema) elType->ElSSchema;
      for (rule = 0; !found && rule < pSS->SsNRules; rule++)
        {
          if (pSS->SsRule->SrElem[rule]->SrConstruct != CsNatureSchema &&
              ((pSS->SsRule->SrElem[rule]->SrName &&
                strcmp (pSS->SsRule->SrElem[rule]->SrName, xmlName) == 0) ||
               (pSS->SsRule->SrElem[rule]->SrOrigName &&
                strcmp (pSS->SsRule->SrElem[rule]->SrOrigName, xmlName) == 0)))
            {
              elType->ElTypeNum = rule + 1;
              if (mappedName)
                *mappedName = pSS->SsRule->SrElem[rule]->SrOrigName;
              found = TRUE;
            }
        }
    }
  else
    {
      pSS = NULL;
      /* search into the loaded natures */
      pPfS = pDoc->DocFirstSchDescr;
      while (pPfS && !found)
        {
          if (pPfS->PfSSchema)
            {
              pSS = (PtrSSchema) pPfS->PfSSchema;
              for (rule = 0; !found && rule < pSS->SsNRules; rule++)
                {
                  if (pSS->SsRule->SrElem[rule]->SrConstruct != CsNatureSchema &&
                      ((pSS->SsRule->SrElem[rule]->SrName &&
                        strcmp (pSS->SsRule->SrElem[rule]->SrName, xmlName) == 0) ||
                       (pSS->SsRule->SrElem[rule]->SrOrigName &&
                        strcmp (pSS->SsRule->SrElem[rule]->SrOrigName, xmlName) == 0)))
                    {
                      elType->ElTypeNum = rule + 1;
                      if (mappedName)
                        *mappedName = pSS->SsRule->SrElem[rule]->SrOrigName;
                      elType->ElSSchema = (SSchema) pSS;
                      found = TRUE;
                    }
                }
            }
          pPfS = pPfS->PfNext;
        }
    }
}

/*----------------------------------------------------------------------
  SetUriSSchema
  Set the namespace uri associated with that schema
  ----------------------------------------------------------------------*/
void SetUriSSchema (PtrSSchema pSSchema, const char *sSchemaUri)
{
  if (sSchemaUri != NULL && pSSchema->SsUriName == NULL)
    {
      pSSchema->SsUriName = (char *)TtaGetMemory (strlen (sSchemaUri) + 1);
      strcpy (pSSchema->SsUriName, sSchemaUri);
    }
}

/*----------------------------------------------------------------------
  ChangeGenericSchemaNames
  Change the name of a generic xml schema
  ----------------------------------------------------------------------*/
void ChangeGenericSchemaNames (const char *sSchemaUri, const char *sSchemaName,
                               PtrDocument pDoc)

{
  PtrSSchema          pSS, docSS;
  PtrPSchema          pPSch;
  PtrDocSchemasDescr  pPfS;
  PtrSRule            pRule;
  ThotBool            found;
  int                 i;
  char                *sSName;
  char                num[10];

  pSS = NULL;
  pPSch = NULL;
  /* copy the new schema name, as it may be changed to make it unique */
  if (sSchemaName)
    sSName = TtaStrdup (sSchemaName);
  else
    sSName = NULL;

  /* Search the appropriate schemas */
  pPfS = pDoc->DocFirstSchDescr;
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
      /* It's a generic xml schema */
      pSS->SsIsXml = TRUE;
      /* Modify the name of the structure schema */
      if (sSchemaUri && pSS->SsUriName == NULL)
        pSS->SsUriName = TtaStrdup (sSchemaUri);
      if (pSS->SsName)
        TtaFreeMemory (pSS->SsName);
      pSS->SsName = NULL;
      if (sSName == NULL)
        {
          if (pSS->SsDefaultPSchema)
            TtaFreeMemory (pSS->SsDefaultPSchema);
          pSS->SsDefaultPSchema = TtaStrdup ("UnknownP");
        }
      else
        {
          /* make sure the new name is not already used in this document */
          i = 1;
          do
            {
              found = FALSE;
              pPfS = pDoc->DocFirstSchDescr;
              while (pPfS && !found)
                {
                  if (pPfS->PfSSchema && pPfS->PfSSchema->SsName &&
                      (strcmp (sSName, pPfS->PfSSchema->SsName) == 0))
                    found = TRUE;
                  else
                    pPfS = pPfS->PfNext;
                }
              if (found)
                /* this name is already used. Change it by appending a number*/
                {
                  sprintf (num, "%d", i++);
                  TtaFreeMemory (sSName);
                  sSName = (char *)TtaGetMemory (strlen (sSchemaName) +
                                                 strlen (num) + 1);
                  strcpy (sSName, sSchemaName);
                  strcat (sSName, num);
                  /* and check again */
                }
            }
          while (found);

          pSS->SsName = (char *)TtaGetMemory (strlen (sSName) + 1);
          strcpy (pSS->SsName, sSName);
          if (pSS->SsDefaultPSchema)
            TtaFreeMemory (pSS->SsDefaultPSchema);
          pSS->SsDefaultPSchema = (char*)TtaGetMemory (strlen (sSName) +2);
          strcpy (pSS->SsDefaultPSchema, sSName);
          strcat (pSS->SsDefaultPSchema, "P");
          /* Modify the rule element name */
          for (i = 0; i < pSS->SsNRules; i++)
            if (pSS->SsRule->SrElem[i]->SrName != NULL &&
                strcmp (pSS->SsRule->SrElem[i]->SrName, "XML") == 0)
              {
                TtaFreeMemory (pSS->SsRule->SrElem[i]->SrName);
                pSS->SsRule->SrElem[i]->SrName = TtaStrdup (sSName);
                TtaFreeMemory (pSS->SsRule->SrElem[i]->SrOrigName);
                pSS->SsRule->SrElem[i]->SrOrigName = TtaStrdup (sSName);
                i = pSS->SsNRules;
              }
        }
    }
  
  if (pPSch != NULL)
    {
      /* Modify the name of the presentation schema */
      if (pPSch->PsPresentName)
        TtaFreeMemory (pPSch->PsPresentName);
      if (sSName == NULL)
        pPSch->PsPresentName = TtaStrdup ("UnknownP");
      else
        {
          pPSch->PsPresentName = (char*)TtaGetMemory (strlen (sSName) +2);
          strcpy (pPSch->PsPresentName, sSName);
          strcat (pPSch->PsPresentName, "P");
          if (pPSch->PsStructName)
            TtaFreeMemory (pPSch->PsStructName);
          pPSch->PsStructName = TtaStrdup (sSName);
        }
    }

  /* Update the LoadedSSchema table */
  for (i = 0; i < MAX_SSCHEMAS &&
         (LoadedSSchema[i].StructSchemaName == NULL ||
          strcmp ("XML", LoadedSSchema[i].StructSchemaName)); i++);
  if (i < MAX_SSCHEMAS)
    {
      /* The generic schema is found in the table, modify its name */
      TtaFreeMemory (LoadedSSchema[i].StructSchemaName);
      if (sSName != NULL)
        {
          LoadedSSchema[i].StructSchemaName = (char *)TtaGetMemory (strlen (sSName) + 1);
          strcpy (LoadedSSchema[i].StructSchemaName, sSName);
        }
      else
        LoadedSSchema[i].StructSchemaName = NULL;
    }

  /* Update the LoadedPSchema table */
#ifndef NODISPLAY
  for (i = 0; i < MAX_SSCHEMAS &&
         (LoadedPSchema[i].PresSchemaName == NULL ||
          strcmp ("XMLP", LoadedPSchema[i].PresSchemaName)); i++);
  if (i < MAX_SSCHEMAS)
    {
      /* The generic schema is found in the table, modify its name */
      TtaFreeMemory (LoadedPSchema[i].PresSchemaName);
      if (sSName != NULL)
        {
          LoadedPSchema[i].PresSchemaName = (char *)TtaGetMemory (strlen (sSName) + 2);
          strcpy (LoadedPSchema[i].PresSchemaName, sSName);
          strcat (LoadedPSchema[i].PresSchemaName, "P");
        }
      else
        LoadedPSchema[i].PresSchemaName = NULL;
    }
#endif

  /* Update the XML Loaded nature */
  docSS= pDoc->DocSSchema;
  found = FALSE;
  i = 0;
  do
    {
      pRule = docSS->SsRule->SrElem[i++];
      if (pRule->SrConstruct == CsNatureSchema)
        if (pRule->SrOrigNat && strcmp (pRule->SrOrigNat, "XML") == 0)
          found = TRUE;
    }
  while (!found && i < docSS->SsNRules);
  if (found)
    {
      TtaFreeMemory (pRule->SrOrigNat);
      pRule->SrOrigNat = TtaStrdup (sSName);
      if (pRule->SrName)
        TtaFreeMemory (pRule->SrName);
      pRule->SrName = TtaStrdup (sSName);
      if (pRule->SrOrigName)
        TtaFreeMemory (pRule->SrOrigName);
      pRule->SrOrigName = TtaStrdup (sSName);
    }
  if (sSName)
    TtaFreeMemory (sSName);
}

/*----------------------------------------------------------------------
  AddANewNamespacePrefix
  ----------------------------------------------------------------------*/
static void AddANewNamespacePrefix (PtrDocument pDoc, PtrElement element,
                                    const char *nsPrefix, PtrNsUriDescr uriDecl)
{
  PtrNsPrefixDescr   newDecl, lastDecl, prevDecl;

  lastDecl = uriDecl->NsPtrPrefix;
  prevDecl = NULL;
  while (lastDecl)
    {
      /* avoid to duplicate a declaration for the same element */
      /* we now accept it */
      /*
      if (lastDecl->NsPrefixElem == element)
        return;
      */
      prevDecl = lastDecl;
      lastDecl = lastDecl->NsNextPrefixDecl;   
    }

  newDecl = (PtrNsPrefixDescr) TtaGetMemory (sizeof (NsPrefixDescr));
  if (newDecl == NULL)
    return;
  memset (newDecl, 0, sizeof (NsPrefixDescr));
  if (nsPrefix)
    newDecl->NsPrefixName = (char *)TtaStrdup (nsPrefix);
  newDecl->NsPrefixElem = element;

  if (prevDecl == NULL)
    uriDecl->NsPtrPrefix = newDecl;
  else
    prevDecl->NsNextPrefixDecl = newDecl;
  
  return;
}

/*----------------------------------------------------------------------
  AddANewNamespaceUri
  ----------------------------------------------------------------------*/
static void AddANewNamespaceUri (PtrDocument pDoc, PtrElement element,
                                 const char *nsPrefix, const char *nsUri)
{
  PtrNsUriDescr  newUriDecl, uriDecl, prevUriDecl;

  newUriDecl = (PtrNsUriDescr) TtaGetMemory (sizeof (NsUriDescr));
  if (newUriDecl == NULL)
    return;
  memset (newUriDecl, 0, sizeof (NsUriDescr));
  if (nsUri != NULL)
    newUriDecl->NsUriName = (char *)TtaStrdup (nsUri);
  else
    newUriDecl->NsUriName = NULL;
  newUriDecl->NsUriSSchema = element->ElStructSchema;

  if (pDoc->DocNsUriDecl == NULL)
    pDoc->DocNsUriDecl = newUriDecl;
  else
    { 
      prevUriDecl = pDoc->DocNsUriDecl;
      uriDecl = prevUriDecl->NsNextUriDecl;
      while (uriDecl != NULL)
        {
          prevUriDecl = uriDecl;
          uriDecl = uriDecl->NsNextUriDecl;
        }
      prevUriDecl->NsNextUriDecl = newUriDecl;
    }

  /* Add a new prefix/element declaration */
  AddANewNamespacePrefix (pDoc, element, nsPrefix, newUriDecl);
}

/*----------------------------------------------------------------------
  SetNamespaceDeclaration
  Add a namespace declaration to the document
  ----------------------------------------------------------------------*/
void SetNamespaceDeclaration (PtrDocument pDoc, PtrElement element,
                              const char *nsPrefix, const char *nsUri)
{
  PtrNsUriDescr    uriDecl;
  PtrNsPrefixDescr prefixDecl;
  ThotBool         found;

  if (element == NULL || element->ElTerminal || ElementIsHidden (element))
    // don't set a namespace declaration for terminal and hidden elements
    return;
  uriDecl = NULL;
  found = FALSE;
  // a patch to replace the old xtiger path by the new one
  if (nsUri && !strcmp (nsUri, Template_URI_o))
    nsUri = Template_URI;
  if (pDoc->DocNsUriDecl)
    {
      /* Search if this uri has been already declared */
      uriDecl = pDoc->DocNsUriDecl;
      if (nsUri)
        {
          while (!found && uriDecl)
            {
              if (uriDecl->NsUriName)
                {
                  if (strcmp (uriDecl->NsUriName, nsUri) == 0)
                    {
                      found = TRUE;
                      prefixDecl = uriDecl->NsPtrPrefix;
                      while (prefixDecl)
                        {
                          if (prefixDecl->NsPrefixElem == element)
                            {
                              if (nsPrefix &&
                                  (prefixDecl->NsPrefixName == NULL ||
                                   strcmp (nsPrefix, prefixDecl->NsPrefixName)))
                                {
                                  TtaFreeMemory (prefixDecl->NsPrefixName);
                                  prefixDecl->NsPrefixName = TtaStrdup (nsPrefix);
                                }
                              // already registered
                              return;
                            }
                          prefixDecl = prefixDecl->NsNextPrefixDecl;
                        }
                    }
                  else
                    uriDecl = uriDecl->NsNextUriDecl;
                }
              else
                uriDecl = uriDecl->NsNextUriDecl;
            }      
        }
      else
        {
          while (!found && uriDecl)
            {
              if (uriDecl->NsUriName == NULL)
                found = TRUE;
              else
                uriDecl = uriDecl->NsNextUriDecl;
            }      
        }
    }

  if (!found)
    /* Add a new uri declaration */
    AddANewNamespaceUri (pDoc, element, nsPrefix, nsUri);
  else
    /* Add a new prefix/element declaration */
    AddANewNamespacePrefix (pDoc, element, nsPrefix, uriDecl);
}

/*----------------------------------------------------------------------
  RemoveANamespaceDeclaration removes a namespace declaration
  nsPrefix could be NULL if the prefix is unknown
  ----------------------------------------------------------------------*/
void RemoveANamespaceDeclaration (PtrDocument pDoc, PtrElement element,
                                  const char *nsPrefix, const char *nsUri)
{
  PtrNsUriDescr     uriDecl, prevUri;
  PtrNsPrefixDescr  prefixDecl, prevDecl;

  if (pDoc == NULL || element == NULL ||
      element->ElTerminal || ElementIsHidden (element))
    // don't set a namespace for terminal and hidden elements
    return;
  if (nsUri == NULL)
    return;

  prevUri = NULL;
  uriDecl = pDoc->DocNsUriDecl;
  if (uriDecl)
    {
      /* Search if this uri has been already declared */
      while (uriDecl)
        {
          // html element could declare several namespaces
          // It's the uri of the element, check the list of elements
          prefixDecl = uriDecl->NsPtrPrefix;
          prevDecl = NULL;
          while (prefixDecl)
            {
              if ((prefixDecl->NsPrefixElem == element &&
                   !strcmp (uriDecl->NsUriName, nsUri) &&
                   nsPrefix == NULL) ||
                  (prefixDecl->NsPrefixElem == element &&
                   !strcmp (uriDecl->NsUriName, nsUri) &&
                   nsPrefix && !strcmp (prefixDecl->NsPrefixName, nsPrefix)))
                {
                  if (prevDecl)
                    prevDecl->NsNextPrefixDecl = prefixDecl->NsNextPrefixDecl;
                  else
                    {
                      // it was the first entry for this uri
                      uriDecl->NsPtrPrefix = prefixDecl->NsNextPrefixDecl;
                      if (uriDecl->NsPtrPrefix == NULL)
                        {
                          // no more entry for this uri, remove this declaration
                          if (prevUri)
                            prevUri->NsNextUriDecl = uriDecl->NsNextUriDecl;
                          else
                            pDoc->DocNsUriDecl = uriDecl->NsNextUriDecl;
                          TtaFreeMemory (uriDecl->NsUriName);
                          TtaFreeMemory (prefixDecl->NsPrefixName);
                          TtaFreeMemory (uriDecl);
                        }
                    }
                  TtaFreeMemory (prefixDecl);
                  return;
                }
              prevDecl = prefixDecl;
              prefixDecl = prefixDecl->NsNextPrefixDecl;
            }
          // next uri declaration
          prevUri = uriDecl;
          uriDecl = prevUri->NsNextUriDecl;
        }
    }
}

/*----------------------------------------------------------------------
  RemoveNamespaceDeclaration removes a namespace declaration
  ----------------------------------------------------------------------*/
void RemoveNamespaceDeclaration (PtrDocument pDoc, PtrElement element)
{
  PtrNsUriDescr     uriDecl, prevUri;
  PtrNsPrefixDescr  prefixDecl, prevDecl;

  if (pDoc == NULL || element == NULL ||
      element->ElTerminal || ElementIsHidden (element))
    // don't set a namespace for terminal and hidden elements
    return;
  prevUri = NULL;
  uriDecl = pDoc->DocNsUriDecl;
  if (uriDecl)
    {
      /* Search if this uri has been already declared */
      while (uriDecl)
        {
          // html element could declare several namespaces
          // It's the uri of the element, check the list of elements
          prefixDecl = uriDecl->NsPtrPrefix;
          prevDecl = NULL;
          while (prefixDecl)
            {
              if (prefixDecl->NsPrefixElem == element)
                {
                  if (prevDecl)
                    prevDecl->NsNextPrefixDecl = prefixDecl->NsNextPrefixDecl;
                  else
                    {
                      // it was the first entry for this uri
                      uriDecl->NsPtrPrefix = prefixDecl->NsNextPrefixDecl;
                      if (uriDecl->NsPtrPrefix == NULL)
                        {
                          // no more entry for this uri, remove this declaration
                          if (prevUri)
                            prevUri->NsNextUriDecl = uriDecl->NsNextUriDecl;
                          else
                            pDoc->DocNsUriDecl = uriDecl->NsNextUriDecl;
                          TtaFreeMemory (uriDecl->NsUriName);
                          TtaFreeMemory (uriDecl);
                        }
                    }
                  TtaFreeMemory (prefixDecl->NsPrefixName);
                  TtaFreeMemory (prefixDecl);
                  return;
                }
              prevDecl = prefixDecl;
              prefixDecl = prefixDecl->NsNextPrefixDecl;
            }
          // next uri declaration
          prevUri = uriDecl;
          uriDecl = prevUri->NsNextUriDecl;
        }
    }

}

/*----------------------------------------------------------------------
  ReplaceNamespaceDeclaration replaces an element in a namespace
  declaration
  ----------------------------------------------------------------------*/
void ReplaceNamespaceDeclaration (PtrDocument pDoc, PtrElement oldEl,
                                 PtrElement newEl)
{
  PtrNsUriDescr     uriDecl;
  PtrNsPrefixDescr  prefixDecl;

  if (pDoc == NULL || oldEl == NULL ||
      oldEl->ElTerminal || ElementIsHidden (oldEl))
    // don't set a namespace for terminal and hidden elements
    return;
  uriDecl = pDoc->DocNsUriDecl;
  if (uriDecl)
    {
      /* Search if this uri has been already declared */
      while (uriDecl)
        {
          if (uriDecl->NsUriSSchema == oldEl->ElStructSchema)
            {
              // It's the uri of the element, check the list of elements
              prefixDecl = uriDecl->NsPtrPrefix;
              while (prefixDecl)
                {
                  if (prefixDecl->NsPrefixElem == oldEl)
                    {
                      prefixDecl->NsPrefixElem = newEl;
                      return;
                    }
                  prefixDecl = prefixDecl->NsNextPrefixDecl;
                }
              return;
            }
          // next uri declaration
          uriDecl = uriDecl->NsNextUriDecl;
        }
    }

}

/*----------------------------------------------------------------------
  CopyNamespaceDeclarations
  Transmit namespace declarations from the source document to the 
  target document.
  ----------------------------------------------------------------------*/
void CopyNamespaceDeclarations (PtrDocument docSource, PtrElement elSource,
                                PtrDocument docTarget, PtrElement elTarget)
{
  PtrNsUriDescr    uriDecl;
  PtrNsPrefixDescr prefixDecl;
  int              i;

  if (docSource && docSource->DocNsUriDecl &&
      docTarget && elSource && elTarget &&
      elSource->ElStructSchema->SsUriName)
    {
      i = 0;
      /* Give the current namespace declarations for this element */
      uriDecl = docSource->DocNsUriDecl;
      while (uriDecl)
        {
          if (uriDecl->NsUriName)
            {
              if (uriDecl->NsPtrPrefix)
                {
                  prefixDecl = uriDecl->NsPtrPrefix;
                  while (prefixDecl)
                    {
                      if (prefixDecl->NsPrefixElem == elSource)
                        /* duplicate the URI declaration attached to the element */
                        SetNamespaceDeclaration (docTarget, elTarget,
                                                 prefixDecl->NsPrefixName,
                                                 uriDecl->NsUriName);
                      prefixDecl = prefixDecl->NsNextPrefixDecl;   
                    }
                }
            }
          uriDecl = uriDecl->NsNextUriDecl;
        }
    }
}

/*----------------------------------------------------------------------
  GiveCurrentNsUri
  Give the current namespace declarations for the element pEl
  ----------------------------------------------------------------------*/
char *GiveCurrentNsUri (PtrDocument pDoc, PtrElement pEl)
{
  PtrNsUriDescr    uriDecl;
  ThotBool         found;
  char            *ns_uri = NULL;


  if (pDoc == NULL || pEl == NULL)
    return (ns_uri);

  if (pEl->ElStructSchema->SsUriName == NULL)
    /* No URI refefence fot this schema */
    return (ns_uri);

  if (pDoc->DocNsUriDecl == NULL)
    /* There is no namespace declaration for this document */
    return (ns_uri);

  found = FALSE;
  /* Give the current namespace declarations for this element */
  uriDecl = pDoc->DocNsUriDecl;
  while (uriDecl && !found)
    {
      if (uriDecl->NsUriName != NULL &&
          pEl->ElStructSchema->SsUriName != NULL &&
          strcmp (uriDecl->NsUriName, pEl->ElStructSchema->SsUriName) == 0)
        {
          /* The URI corresponding to the element schema has been found */
          /* Search the first associated prefix */
          found = TRUE;
          ns_uri = uriDecl->NsUriName;
        }
      else
        uriDecl = uriDecl->NsNextUriDecl;
    }
  return (ns_uri);
}

/*----------------------------------------------------------------------
  GiveElemNamespaceDeclarations
  Give the current namespace declarations / prefixes  for the element pEl
  ----------------------------------------------------------------------*/
void GiveElemNamespaceDeclarations (PtrDocument pDoc, PtrElement pEl,
				    char **declarations, char **prefixes,
				    int max)
{

  PtrNsUriDescr    uriDecl;
  PtrNsPrefixDescr prefixDecl;
  int              i;

  for (i = 0; i < max; i++)
    {
      *&declarations[i] = NULL;
      *&prefixes[i] = NULL;
    }
  /* Search all the namespace declarations declared for this element */
  uriDecl = pDoc->DocNsUriDecl;
  i = 0;
  while (uriDecl && i < max)
    {
      prefixDecl = uriDecl->NsPtrPrefix;
      while (prefixDecl != NULL)
        {
          if (prefixDecl->NsPrefixElem == pEl &&
              (uriDecl->NsUriName || prefixDecl->NsPrefixName))
            {
              /* A Namespace declaration has been found for this element */
              *&declarations[i] = uriDecl->NsUriName;
              *&prefixes[i] = prefixDecl->NsPrefixName;
              i++;
            }
          prefixDecl = prefixDecl->NsNextPrefixDecl;
        }
      uriDecl = uriDecl->NsNextUriDecl;
    }
  return;
}

/*----------------------------------------------------------------------
  DocumentUsesNsPrefixes
  Check wether the document uses namespaces with prefix
  ----------------------------------------------------------------------*/
ThotBool DocumentUsesNsPrefixes (PtrDocument pDoc)
{
  PtrNsUriDescr    uriDecl;
  ThotBool         usesPrefix;
  char            *ns_uri = NULL;

  usesPrefix = FALSE;
  if (pDoc == NULL)
    return (usesPrefix);

  /* check all namespace declarations of the document */
  uriDecl = pDoc->DocNsUriDecl;
  while (uriDecl && !usesPrefix)
    {
      if (uriDecl->NsUriName && uriDecl->NsPtrPrefix &&
	  uriDecl->NsPtrPrefix->NsPrefixName)
	/* This declaration has both a name and a prefix */
	usesPrefix = TRUE;
      else
        uriDecl = uriDecl->NsNextUriDecl;
    }
  return (usesPrefix);
}
