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
     Name            StructSchemaName; /* nom du schema de presentation */
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
   PresForStructSchema
   Return the block describing the presentation schemas to be used
   for structure schema pSS in document pDoc.
  ----------------------------------------------------------------------*/
static PtrDocSchemasDescr PresForStructSchema (PtrDocument pDoc,
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

#ifndef NODISPLAY

/*----------------------------------------------------------------------
   FreePRuleList  libere la liste de regles de presentation dont   
   l'ancre est firstPRule.                         
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
   Libere le schema de presentation pointe par pPSchema,		
   ainsi que toutes les regles de presentation qu'il pointe.	
   pSS pointe le schema de structure auquel le schema de           
   presentation a liberer est associe.
  ----------------------------------------------------------------------*/
static void ReleasePresentationSchema (PtrPSchema pPSchema, PtrSSchema pSS, 
				       PtrDocument pDoc)
{
  APresentation      *pPres;
  AttributePres      *pAttrPres;
  int                 i, j;
  PtrHostView         pHostView, pNextHostView;

#ifndef VQ
  fprintf (stderr, "release P schema %s for %s\n", pPSchema->PsPresentName,
	   pSS->SsName);
#endif
  /* look for this schema in the PSchemas table */
  for (i = 0; i < MAX_PSCHEMAS && LoadedPSchema[i].pPresSchema != pPSchema;
       i++);
  if (i >= MAX_PSCHEMAS)
    /* This schema is not in the table. Probably a P Schema extension */
    return;
  /* this schema is in the table */
  pPres = &LoadedPSchema[i];
  pPres->UsageCount--;
  if (pPres->UsageCount == 0)
    /* c'etait la derniere utilisation, on le supprime de la table */
    {
#ifndef VQ
  fprintf (stderr, "   free P schema %s\n", pPSchema->PsPresentName);
#endif
      pPres->pPresSchema = NULL;
      pPres->PresSchemaName[0] = EOS;
      /* libere les regles de presentation par defaut */
      FreePRuleList (&pPSchema->PsFirstDefaultPRule);
      /* libere les regles de presentation des boites de presentation */
      for (i = 0; i < pPSchema->PsNPresentBoxes; i++)
	FreePRuleList (&pPSchema->PsPresentBox[i].PbFirstPRule);
      /* libere les regles de presentation des attributs */
      for (i = 0; i < pSS->SsNAttributes; i++)
	{
	  pAttrPres = pPSchema->PsAttrPRule[i];
	  while (pAttrPres != NULL)
	    {
	      switch (pSS->SsAttribute[i].AttrType)
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
		  for (j = 0; j <= pSS->SsAttribute[i].AttrNEnumValues; j++)
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
	FreePRuleList (&pPSchema->PsElemPRule[i]);
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
      FreeSchPres (pPSchema);
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

   if (schemaName == NULL || schemaName[0] == EOS || pSS == NULL)
     /* invalid parameter */
     return FALSE;
   pPSchema = NULL;
   /* Look at the table of loaded schemas */
   for (i = 0; i < MAX_PSCHEMAS &&
	       ustrcmp (schemaName, LoadedPSchema[i].PresSchemaName); i++);
   if (i < MAX_PSCHEMAS)
      /* This schema is in the table, no need to load it */
      {
      LoadedPSchema[i].UsageCount++;
      pPSchema = LoadedPSchema[i].pPresSchema;
#ifndef VQ
  fprintf (stderr, "get P schema %s\n", pPSchema->PsPresentName);
#endif
      }
   else
      /* That's a new schema. Load it */
      {
      pPSchema = ReadPresentationSchema (schemaName, pSS);
      if (pPSchema)
	 /* schema loaded. Register it in the table of loaded schemas */
	 {
#ifndef VQ
  fprintf (stderr, "load P schema %s\n", pPSchema->PsPresentName);
#endif
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
     pPfS = PresForStructSchema (pDoc, pSS, &pPrevPfS);
     if (!pPfS)
       fprintf (stderr, "*** S schema %s missing ***\n",
		pPSchema->PsPresentName);
     else
       {
	 if (pPfS->PfPSchema)
	   /* release the previous presentation schema */
	   {
	     ReleasePresentationSchema (pPfS->PfPSchema, pSS, pDoc);
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
void               FreePresentationSchema (PtrPSchema pPSchema, PtrSSchema pSS,
					   PtrDocument pDoc)
{
  PtrDocSchemasDescr   pPfS, pPrevPfS;
  PtrHandlePSchema     pHSP, pNextHSP;

  ReleasePresentationSchema (pPSchema, pSS, pDoc);
  pPfS = PresForStructSchema (pDoc, pSS, &pPrevPfS);
  if (pPfS)
    /* check if it's the main presentation schema or an extension */
    if (pPfS->PfPSchema == pPSchema)
      /* it's the main presentation schema. Unlink it */
      {
	pPfS->PfPSchema = NULL;
	pHSP = pPfS->PfFirstPSchemaExtens;
	while (pHSP)
	  {
	    pNextHSP = pHSP->HdNextPSchema;
	    ReleasePresentationSchema (pHSP->HdPSchema, pSS, pDoc);
	    FreeHandleSchPres (pHSP);
	    pHSP = pNextHSP;
	  }
	/* unlink the block */
	if (pPrevPfS)
	  pPrevPfS = pPfS->PfNext;
	else
	  pDoc->DocFirstSchDescr = pPfS->PfNext;
	FreeDocSchemasDescr (pPfS);
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

  pPfS = PresForStructSchema (pDoc, pSS, &pPrevPfS);
  if (pPfS)
    return (pPfS->PfFirstPSchemaExtens);
  else
    return NULL;
}

/*----------------------------------------------------------------------
  UnlinkPSchemaExtension
  ----------------------------------------------------------------------*/
void UnlinkPSchemaExtension (PtrDocument pDoc, PtrSSchema pSS, PtrPSchema pPS)
{
  PtrDocSchemasDescr  pPfS, pPrevPfS;
  PtrHandlePSchema    pHd;

  if (!pDoc || !pSS || !pPS)
    return;
  pPfS = PresForStructSchema (pDoc, pSS, &pPrevPfS);
  if (!pPfS)
    return;
  pHd = pPfS->PfFirstPSchemaExtens;
  while (pHd && pHd->HdPSchema != pPS)
    pHd = pHd->HdNextPSchema;
  if (pHd)
    {
      if (pHd->HdPrevPSchema == NULL)
	pPfS->PfFirstPSchemaExtens = pHd->HdNextPSchema;
      else
	pHd->HdPrevPSchema->HdNextPSchema = pHd->HdNextPSchema;
      if (pHd->HdNextPSchema != NULL)
	pHd->HdNextPSchema->HdPrevPSchema = pHd->HdPrevPSchema;
      FreeHandleSchPres (pHd);
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

  pPfS = PresForStructSchema (pDoc, pSS, &pPrevPfS);
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

  pPfS = PresForStructSchema (pDoc, pSS, &pPrevPfS);
  if (pPfS)
    return (pPfS->PfPSchema);
  else
#endif /* NODISPLAY */
    return NULL;
}

/*----------------------------------------------------------------------
   LoadStructureSchema
   If structure schema called schemaName is already loaded, return a
   pointer ot it, otherwise load it from its file, register it in the
   table of loaded schemas, and return a pointer to it.
   Return NULL if schema can't be loaded.
  ----------------------------------------------------------------------*/
PtrSSchema          LoadStructureSchema (Name schemaName, PtrDocument pDoc)
{
   PtrSSchema           pSSchema;
   PtrDocSchemasDescr   pPfS, pPrevPfS;
   int                  i;

   if (schemaName == NULL || schemaName[0] == EOS)
     /* invalid parameter */
     return NULL;
   pSSchema = NULL;
   /* Look at the table of loaded schemas */
   for (i = 0; i < MAX_SSCHEMAS &&
	       ustrcmp (schemaName, LoadedSSchema[i].StructSchemaName); i++);
   if (i < MAX_SSCHEMAS)
      /* This schema is in the table, no need to load it */
      {
      LoadedSSchema[i].UsageCount++;
      pSSchema = LoadedSSchema[i].pStructSchema;
#ifndef VQ
  fprintf (stderr, "get S schema %s\n", pSSchema->SsName);
#endif
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
#ifndef VQ
  fprintf (stderr, "*** failed loading S schema %s\n", schemaName);
#endif
	 FreeSchStruc (pSSchema);
	 pSSchema = NULL;
	}
      else
	 /* schema loaded. Register it in the table of loaded schemas */
	 {
#ifndef VQ
  fprintf (stderr, "load S schema %s\n", pSSchema->SsName);
#endif
	 /* look for an free entry in the table */
	 for (i = 0; i < MAX_SSCHEMAS && LoadedSSchema[i].pStructSchema; i++);
	 if (i < MAX_SSCHEMAS && LoadedSSchema[i].pStructSchema == NULL)
	    /* free entry found */
	    {
	    LoadedSSchema[i].UsageCount = 1;
	    LoadedSSchema[i].pStructSchema = pSSchema;
	    ustrncpy (LoadedSSchema[i].StructSchemaName, schemaName,
		      MAX_NAME_LENGTH);
	    }
	 /* translate the structure schema in the user's language */
	 ConfigTranslateSSchema (pSSchema);
	 }
      }
   if (pSSchema)
     /* add a schema descriptor to the document */
     {
     pPfS = PresForStructSchema (pDoc, pSSchema, &pPrevPfS);
     if (!pPfS)
       {
	 GetDocSchemasDescr (&pPfS);
	 pPfS->PfNext = pDoc->DocFirstSchDescr;
         pDoc->DocFirstSchDescr = pPfS;
	 pPfS->PfSSchema = pSSchema;
	 pPfS->PfPSchema = NULL;
	 pPfS->PfFirstPSchemaExtens = NULL;
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
  AStructure *pStr;
  int        i;

  /* look for this schema in the table */
  for (i = 0; i < MAX_SSCHEMAS && LoadedSSchema[i].pStructSchema != pSS; i++);
  if (i >= MAX_SSCHEMAS)
    /* error. This schema is not in the table */
    return FALSE;
#ifndef VQ
  fprintf (stderr, "release S schema %s\n", pSS->SsName);
#endif
  pStr = &LoadedSSchema[i];
  pStr->UsageCount--;
  if (pStr->UsageCount > 0)
    return FALSE;
  else
    /* This schema is no longer used by any document. Unload it */
    {
#ifndef VQ
  fprintf (stderr, "   free S schema %s\n", pSS->SsName);
#endif
      pStr->pStructSchema = NULL;
      pStr->StructSchemaName[0] = EOS;
      FreeSchStruc (pSS);
      return TRUE;
    }
}

/*----------------------------------------------------------------------
   LoadNatureSchema charge la nature definie dans la regle rule du	
   schema de structure pSS. Si le 1er octet de PSchName est nul on	
   propose a l'utilisateur le schema de presentation par defaut	
   defini dans le schema de structure, sinon on propose le schema de  
   presentation de nom PSchName.					
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
   strncpy (schName, pSS->SsRule[rule - 1].SrOrigNat, MAX_NAME_LENGTH);
   /* cree un schema de structure et le charge depuis le fichier */
   pNatureSS = LoadStructureSchema (schName, pDoc);
   if (!pNatureSS)
      /* echec */
      pSS->SsRule[rule - 1].SrSSchemaNat = NULL;
   else
      /* chargement du schema de structure reussi */
      {
      pSS->SsRule[rule - 1].SrSSchemaNat = pNatureSS;
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
	 if (!ConfigGetPSchemaNature (pSS, pSS->SsRule[rule - 1].SrOrigNat,
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
	 pSS->SsRule[rule - 1].SrSSchemaNat = NULL;
	 }
      if (ThotLocalActions[T_initevents] != NULL)
	 (*ThotLocalActions[T_initevents]) (pNatureSS);
#endif /* NODISPLAY */
      }
}

/*----------------------------------------------------------------------
   AppendSRule     ajoute une nouvelle regle a la fin de la table  
   des regles.                                     
  ----------------------------------------------------------------------*/
static void         AppendSRule (int *ret, PtrSSchema pSS)
{
   if (pSS->SsNRules >= MAX_RULES_SSCHEMA)
      {
      /* Table de regles saturee */
      TtaDisplaySimpleMessage (FATAL, LIB, TMSG_LIB_RULES_TABLE_FULL);
      *ret = 0;
      }
   else
      /* ajoute une entree a la table */
      {
      pSS->SsNRules++;
      *ret = pSS->SsNRules;
      if (pSS->SsFirstDynNature == 0)
	 pSS->SsFirstDynNature = pSS->SsNRules;
      }
}

/*----------------------------------------------------------------------
   CreateNature
   retourne le numero de la regle definissant le type de nom SSchName
   dans le schema de structure pointe par pSS.                                    S'il n'existe pas de type de ce nom, ajoute une regle de type   
   CsNatureSchema au schema de structure et charge le schema de	
   structure de nom SSchName; retourne le numero de la regle creee	
   ou 0 si echec creation.						
   En cas de chargement de schema, le nom de schema de             
   presentation PSchName est propose' a l'utilisateur plutot que	
   le schema de presentation par defaut defini dans le schema de	
   structure, sauf si le premier octet de PSchName est nul.	
  ----------------------------------------------------------------------*/
int          CreateNature (char *SSchName, char *PSchName, PtrSSchema pSS,
			   PtrDocument pDoc)
{
#ifndef NODISPLAY
   PtrPSchema  pPS;
#endif
   SRule              *pRule;
   PtrSSchema         pSSch;
   int                ret;
   ThotBool           found;

   /* cherche si le type existe deja dans le schema de structure */
   found = FALSE;
   ret = 0;
   do
      {
      pRule = &pSS->SsRule[ret++];
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
	 AppendSRule (&ret, pSS);
      else
	 /* il y a deja des natures dynamiques */
	 /* cherche s'il y en a une libre */
	 {
	 ret = pSS->SsFirstDynNature;
	 while (ret <= pSS->SsNRules &&
		pSS->SsRule[ret - 1].SrSSchemaNat != NULL)
	    ret++;
	 if (ret > pSS->SsNRules)
	    /* pas de regle libre, on ajoute une regle a la fin de la */
	    /* table */
	    AppendSRule (&ret, pSS);
	 }
      if (ret > 0)
	 /* il y a une entree libre (celle de rang ret) */
	 /* remplit la regle nature */
	 {
	 pRule = &pSS->SsRule[ret - 1];
	 strncpy (pRule->SrOrigNat, SSchName, MAX_NAME_LENGTH);
	 strncpy (pRule->SrName, SSchName, MAX_NAME_LENGTH);
	 pRule->SrAssocElem = FALSE;
	 pRule->SrNDefAttrs = 0;
	 pRule->SrConstruct = CsNatureSchema;
	 pRule->SrSSchemaNat = NULL;
#ifndef NODISPLAY
	 /* initialise le pointeur sur les regles de presentation qui */
	 /* correspondent a cette nouvelle regle de structure */
	 pPS = PresentationSchema (pSS, pDoc);
	 if (pPS != NULL)
	    pPS->PsElemPRule[ret - 1] = NULL;
#endif   /* NODISPLAY */
	 }
      }
   if (ret > 0)
      /* il y a une entree libre (celle de rang ret) */
      {
      pRule = &pSS->SsRule[ret - 1];
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
   charge en memoire le schema de structure de nom SSchName ainsi que
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
   LoadExtension charge en memoire, pour le document pDoc, le schema  
   d'extension de nom SSchName et son schema de presentation de    
   nom PSchName.                                                   
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
   FreeNatureRules	cherche dans le schema de structure pointe'	
   par pSS les regles de nature qui font reference au schema	
   pointe par pNatureSS. S'il y en a, retourne Vrai, annule ces	
   regles et traite de meme les autres natures. S'il n'y en a pas,	
   retourne Faux.                                                  
  ----------------------------------------------------------------------*/
static ThotBool     FreeNatureRules (PtrSSchema pSS, PtrSSchema pNatureSS)
{
   SRule              *pRule;
   int                 rule;
   ThotBool            ret;

   ret = FALSE;
   if (pSS != NULL)
      /* parcourt les regles de ce schemas */
      for (rule = 0; rule < pSS->SsNRules; rule++)
	 {
	 pRule = &pSS->SsRule[rule];
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
	    else
	       /* elle fait reference a une autre nature, on cherche */
	       /* dans cette nature les regles qui font reference a la */
	       /* nature supprimee. */
	       FreeNatureRules (pRule->SrSSchemaNat, pNatureSS);
	   }
	 }
   return ret;
}

/*----------------------------------------------------------------------
   FreeNature    Si le schema de structure pointe' par pSS contient	
   une regle de nature pour le schema pointe' par pNatureSS,		
   retourne Vrai et libere le schema de structure pointe par		
   pNatureSS et son schema de presentation.            		
   Retourne faux sinon.                                              
  ----------------------------------------------------------------------*/
ThotBool            FreeNature (PtrSSchema pSS, PtrSSchema pNatureSS,
				PtrDocument pDoc)
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
   FreeDocumentSchemas libere tous les schemas de structure et de	
   presentation utilises par le document dont le descripteur est   
   pointe par pDoc.                                                
   Pour les schemas de presentation, la liberation n'est effective 
   que s'ils ne sont pas utilises par d'autres documents.          
  ----------------------------------------------------------------------*/
void             FreeDocumentSchemas (PtrDocument pDoc)
{
   PtrSSchema    pSS;
   int           i;

   while (pDoc->DocFirstSchDescr)
     {
       pSS = pDoc->DocFirstSchDescr->PfSSchema;
#ifndef NODISPLAY
       FreePresentationSchema (pDoc->DocFirstSchDescr->PfPSchema, pSS, pDoc);
#endif
       if (ReleaseStructureSchema (pSS, pDoc))
	 /* this structure schema has been unloaded */
	 /* remove any reference to that schema from all Nature rules
	    of other S schemas */
	 for (i = 0; i < MAX_SSCHEMAS; i++)
	   if (LoadedSSchema[i].pStructSchema)
	     FreeNatureRules (LoadedSSchema[i].pStructSchema, pSS);
     }
   pDoc->DocSSchema = NULL;
}

#ifndef NODISPLAY
/*----------------------------------------------------------------------
   AddGuestViews
   add the guest views defined in presentation schema assosicated with pSS
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
   AddAllGuestViews
   add the guest views of presentation schema attached to pSS to the list
   of guest views of document view pViewDescr.
   Add also the guest views of presentation schemas attached to all
   natures used in pSS.
  ----------------------------------------------------------------------*/
static void      AddAllGuestViews (PtrSSchema pSS, DocViewDescr *pViewDescr,
				   PtrDocument pDoc)
{
   SRule              *pRule;
   int                 i;

   while (pSS != NULL)
      {
      /* get guest views in the presentation schema of this structure schema */
      AddGuestViews (pSS, pViewDescr, pDoc);
      /* look for nature schemas used in this structure schema */
      for (i = 0; i < pSS->SsNRules; i++)
         {
	 pRule = &pSS->SsRule[i];
	 if (pRule->SrConstruct == CsNatureSchema)
	    if (pRule->SrSSchemaNat != NULL)
	       /* the structure schema of this nature is loaded */
	       AddAllGuestViews (pRule->SrSSchemaNat, pViewDescr, pDoc);
	 }
      pSS = pSS->SsNextExtens;
      }
}

/*----------------------------------------------------------------------
   CreateGuestViewList
   create the guest view list for view view of document pDoc
  ----------------------------------------------------------------------*/
void         CreateGuestViewList (PtrDocument pDoc, int view)
{
   /* look for the presentation schemas of all structure schemas used in that
      document */
   AddAllGuestViews (pDoc->DocSSchema, &pDoc->DocView[view - 1], pDoc);
}

/*----------------------------------------------------------------------
   AddNature  met dans la table des natures du document pDoc          
   les schemas references par le schema de structure pSS       
  ----------------------------------------------------------------------*/
static void         AddNature (PtrSSchema pSS, PtrDocument pDoc)
{
   int                 rule, nat, nObjects;
   ThotBool            present;
   SRule              *pSRule;
   ThotBool            attrSchema;
   PtrElement          pSaved;

   for (rule = 0; rule < pSS->SsNRules; rule++)
      {
      pSRule = &pSS->SsRule[rule];
      if (pSRule->SrConstruct == CsNatureSchema)
	 if (pSRule->SrSSchemaNat != NULL)
	   /* the structure schema for this nature is loaded */
	   {
	    /* number of elements in this document that have been created
	       following this schema */
	    nObjects = pSRule->SrSSchemaNat->SsNObjects;
	    /* if the schema defines only attribute, does not count its
	       elements. Take it into account anyway */
	    attrSchema = (pSRule->SrSSchemaNat->SsRootElem ==
			  pSRule->SrSSchemaNat->SsNRules);
	    /* ignore a structure schema for which no elements have been
               created in the document, except if it's a schema that
               defines no element, only attributes */
	    if (nObjects > 0 || attrSchema)
	       {
	       /* Decompte les objets de cette nature qui sont dans */
	       /* le buffer de Copier-Couper-Coller */
	       if (!attrSchema && FirstSavedElement != NULL)
		  {
		  pSaved = FirstSavedElement->PeElement;
		  do
		     {
		     if (pSaved->ElStructSchema == pSRule->SrSSchemaNat &&
			 pSaved->ElTypeNumber == pSRule->SrSSchemaNat->SsRootElem)
		        nObjects--;
		     pSaved = FwdSearchTypedElem (pSaved,
					     pSRule->SrSSchemaNat->SsRootElem,
					     pSRule->SrSSchemaNat);
		     }
		  while (pSaved != NULL);
		  }
	       if (attrSchema || nObjects > 0)
		  {
		    /* Si les natures contiennent elles-memes des natures  */
		    /* on pourrait ecrire plusieurs fois un nom de nature. */
		    /* On verifie que ce nom n'est pas dans la table */
		    nat = 0;
		    present = FALSE;
		    while (nat < pDoc->DocNNatures && !present)
		      if (strcmp (pDoc->DocNatureName[nat],
				   pSRule->SrSSchemaNat->SsName) == 0)
			present = TRUE;
		      else
			nat++;
		    if (!present)
		      /* il n'est pas dans la table */
		      /* met le schema dans la table */
		      {
			if (pDoc->DocNNatures < MAX_NATURES_DOC)
			  {
			    strncpy (pDoc->DocNatureName[pDoc->DocNNatures],
				      pSRule->SrSSchemaNat->SsName,
				      MAX_NAME_LENGTH);
			    strncpy (pDoc->DocNaturePresName[pDoc->DocNNatures],
				      pSRule->SrSSchemaNat->SsDefaultPSchema,
				      MAX_NAME_LENGTH);
			    pDoc->DocNatureSSchema[pDoc->DocNNatures] =
			      pSRule->SrSSchemaNat;
			    pDoc->DocNNatures++;
			  }
		      }
		    /* cherche les natures utilisees par cette nature */
		    /* meme si elle est deja dans la table : celle qui est */
		    /* dans la table ne reference peut-etre pas des natures */
		    /* qui sont referencees par celle-ci */
		    AddNature (pSRule->SrSSchemaNat, pDoc);
		  }
	       }
	   }
      }
}

/*----------------------------------------------------------------------
   BuildDocNatureTable remplit la table des schemas utilises          
   par le document pDoc.                                   
  ----------------------------------------------------------------------*/
void                BuildDocNatureTable (PtrDocument pDoc)
{
   PtrSSchema          pSSExtens;

   /* met le schema de structure du document en tete de la table des */
   /* natures utilisees */
   pDoc->DocNatureSSchema[0] = pDoc->DocSSchema;
   strncpy (pDoc->DocNatureName[0], pDoc->DocSSchema->SsName,
	     MAX_NAME_LENGTH);
   strncpy (pDoc->DocNaturePresName[0], pDoc->DocSSchema->SsDefaultPSchema,
	     MAX_NAME_LENGTH);
   pDoc->DocNNatures = 1;
   /* met dans la table des natures du document les */
   /* extensions du schema de structure du document */
   pSSExtens = pDoc->DocSSchema->SsNextExtens;
   while (pSSExtens != NULL)
      {
      /* met ce schema d'extension dans la table des natures */
      if (pDoc->DocNNatures < MAX_NATURES_DOC)
	 {
	 strncpy (pDoc->DocNatureName[pDoc->DocNNatures], pSSExtens->SsName,
		   MAX_NAME_LENGTH);
	 strncpy (pDoc->DocNaturePresName[pDoc->DocNNatures],
		   pSSExtens->SsDefaultPSchema, MAX_NAME_LENGTH);
	 pDoc->DocNatureSSchema[pDoc->DocNNatures] = pSSExtens;
	 pDoc->DocNNatures++;
	 }
      /* met dans la table les natures utilises par cette extension */
      AddNature (pSSExtens, pDoc);
      /* passe au schema d'extension suivant */
      pSSExtens = pSSExtens->SsNextExtens;
      }
   /* met dans la table des natures les schemas de structure */
   /* reference's par le schema de structure du document. */
   AddNature (pDoc->DocSSchema, pDoc);
}
#endif   /* NODISPLAY */
