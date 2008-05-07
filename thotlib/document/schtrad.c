/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2008
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
 
/*
  Handler of translation schemas
*/
#include "thot_gui.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "consttra.h"
#include "message.h"
#include "libmsg.h"
#include "typemedia.h"
#include "typetra.h"

#define THOT_EXPORT extern
#include "edit_tv.h"

#include "readstr_f.h"
#include "readtra_f.h"
#include "schemas_f.h"
#include "schtrad_f.h"

typedef struct _ATranslation
{
  PtrSSchema          pStructSchema;		/* pointeur sur le schema de struct. */
  PtrTSchema          pTransSchema;	/* pointeur sur le schema de trad. */
  Name                TransSchemaName;	/* nom du schema de traduction */
}
ATranslation;

#define MAX_TSCHEMAS 16		/* nombre max de schemas de traduction charges
                             en meme temps */

/* table des schemas de traduction charges */
static ATranslation LoadedTSchema[MAX_TSCHEMAS];

/*----------------------------------------------------------------------
  InitTranslationSchemasTable initialise la table des schemas de	
  traduction charges.					
  ----------------------------------------------------------------------*/
void InitTranslationSchemasTable ()
{
  int                 i;

  for (i = 0; i < MAX_TSCHEMAS; i++)
    {
      LoadedTSchema[i].pStructSchema = NULL;
      LoadedTSchema[i].pTransSchema = NULL;
      LoadedTSchema[i].TransSchemaName[0] = EOS;
    }
}


/*----------------------------------------------------------------------
  ClearTranslationSchemasTable libere tous les schemas de         
  traduction pointe's par la table des schemas de traduction      
  ----------------------------------------------------------------------*/
void ClearTranslationSchemasTable ()
{
  int                 i;

  for (i = 0; i < MAX_TSCHEMAS; i++)
    {
      if (LoadedTSchema[i].pTransSchema != NULL)
        {
          FreeTranslationSchema (LoadedTSchema[i].pTransSchema,
                                 LoadedTSchema[i].pStructSchema);
          LoadedTSchema[i].pTransSchema = NULL;
        }
      LoadedTSchema[i].pStructSchema = NULL;
      LoadedTSchema[i].TransSchemaName[0] = EOS;
    }
}


/*----------------------------------------------------------------------
  LoadTranslationSchema charge un schema de traduction.           
  ----------------------------------------------------------------------*/
PtrTSchema LoadTranslationSchema (const char *schName, PtrSSchema pSS)
{
  PtrTSchema          pTSch;
  int                 i;
  ThotBool            found;

  if (pSS == NULL || schName == NULL || schName[0] == EOS)
    return (NULL);
  pTSch = NULL;
  /* cherche dans la table si le schema est deja charge */
  i = 0;
  found = FALSE;
  do
    {
      if (LoadedTSchema[i].pStructSchema &&
          LoadedTSchema[i].pStructSchema->SsCode == pSS->SsCode)
        found = (strcmp (schName, LoadedTSchema[i].TransSchemaName) == 0);
      if (!found)
        i++;
    }
  while (!found && i < MAX_TSCHEMAS);
  if (found)
    /* ce schema est dans la table des schemas charges */
    pTSch = LoadedTSchema[i].pTransSchema;
  else
    /* c'est un nouveau schema, il faut le charger */
    {
      /* cherche d'abord une entree libre dans la table */
      i = 0;
      while (LoadedTSchema[i].pStructSchema != NULL && i < MAX_TSCHEMAS)
        i++;
      if (LoadedTSchema[i].pStructSchema == NULL && i < MAX_TSCHEMAS)
        /* on a trouve une entree libre */
        {
          /* on charge le schema de traduction */
          pTSch = ReadTranslationSchema (schName, pSS);
          if (pTSch != NULL)
            /* met le nouveau schema dans la table des schemas charges */
            {
              LoadedTSchema[i].pStructSchema = pSS;
              LoadedTSchema[i].pTransSchema = pTSch;
              strncpy (LoadedTSchema[i].TransSchemaName, schName, MAX_NAME_LENGTH-1);
              LoadedTSchema[i].TransSchemaName[MAX_NAME_LENGTH-1] = EOS;
            }
        }
    }
  return (pTSch);
}


/*----------------------------------------------------------------------
  GetUSErule cherche une regle USE dans la suite de blocs de      
  regles pBlock. Si une regle USE pour la nature de nom   
  schName est trouvee dans ce bloc, on met dans schName	
  le nom du schema de traduction a` utiliser.		
  ----------------------------------------------------------------------*/
static ThotBool GetUSErule (PtrTRuleBlock pBlock, Name schName)
{
  PtrTRule            pTRule;
  ThotBool            found;

  found = FALSE;
  if (pBlock != NULL)
    do
      {
        pTRule = pBlock->TbFirstTRule;
        if (pTRule != NULL)
          do
            {
              if (pTRule->TrType == TUse)
                /* c'est une regle USE */
                if (strcmp (schName, pTRule->TrNature) == 0)
                  {
                    found = TRUE;
                    strncpy (schName, pTRule->TrTranslSchemaName, MAX_NAME_LENGTH);
                  }
              pTRule = pTRule->TrNextTRule;
            }
          while (pTRule != NULL && !found);
        pBlock = pBlock->TbNextBlock;
      }
    while (pBlock != NULL && !found);
  return found;
}


/*----------------------------------------------------------------------
  GetTransSchName       trouve le nom du schema de traduction a`	
  utiliser pour le schema de structure de nom schName.    
  Au retour, rend dans schName le nom du schema de        
  traduction.                                             
  ----------------------------------------------------------------------*/
static void GetTransSchName (Name schName)
{
  int                 i;
  ThotBool            found, natureOK;
  PtrSSchema          pSS;
  PtrSRule            pSRule;

  found = FALSE;
  /* cherche d'abord si le schema de traduction du document contient */
  /* une regle USE pour cette nature */
  /* le schema de traduction du document est le premier de la table */
  if (LoadedTSchema[0].pTransSchema != NULL)
    /* cherche la nature dans le schema de structure du document */
    {
      natureOK = FALSE;
      pSS = LoadedTSchema[0].pStructSchema;
      i = 0;
      do
        {
          pSRule = pSS->SsRule->SrElem[i++];
          if (pSRule->SrConstruct == CsNatureSchema)
            natureOK = strcmp (schName, pSRule->SrName) == 0;
        }
      while (i < pSS->SsNRules && !natureOK);
      if (natureOK)
        /* on a trouve la nature, on cherche une regle USE parmi les */
        /* regles de traduction de l'element nature */
        found = GetUSErule (LoadedTSchema[0].pTransSchema->TsElemTRule->TsElemTransl[i - 1], schName);
      if (!found)
        /* on cherche une regle USE parmi les regles de traduction de */
        /* l'element racine du document */
        found = GetUSErule (LoadedTSchema[0].pTransSchema->
                            TsElemTRule->TsElemTransl[LoadedTSchema[0].pStructSchema->SsRootElem - 1], schName);
    }
  if (!found)
    schName[0] = EOS;
}

/*----------------------------------------------------------------------
  GetTranslationSchema retourne le schema de traduction a`	
  appliquer aux elements appartenant au schema de structure pSS.	
  ----------------------------------------------------------------------*/
PtrTSchema          GetTranslationSchema (PtrSSchema pSS)
{
  PtrTSchema          pTSchema;
  Name                schemaName;
  int                 i;
  ThotBool            found;

  if (pSS == NULL)
    return NULL;
  pTSchema = NULL;
  found = FALSE;
  i = 0;
  do
    {
      if (LoadedTSchema[i].pStructSchema)
        found = (!strcmp (pSS->SsName, LoadedTSchema[i].pStructSchema->SsName));
      if (!found)
        i++;
    }
  while (!found && i < MAX_TSCHEMAS);
  if (found)
    pTSchema = LoadedTSchema[i].pTransSchema;
  else
    {
      strcpy (schemaName, pSS->SsName);
      GetTransSchName (schemaName);
      if (schemaName[0] != EOS)
        /* cree un nouveau schema de traduction et le charge */
        pTSchema = LoadTranslationSchema (schemaName, pSS);
      else
        /* indique dans la table qu'il n'y a pas de schema de traduction */
        /* pour ce schema de structure */
        {
          /* cherche d'abord une entree libre dans la table */
          i = 0;
          while (LoadedTSchema[i].pStructSchema != NULL && i < MAX_TSCHEMAS)
            i++;
          if (LoadedTSchema[i].pStructSchema == NULL && i < MAX_TSCHEMAS)
            /* on a trouve une entree libre */
            {
              LoadedTSchema[i].pStructSchema = pSS;
              LoadedTSchema[i].pTransSchema = NULL;
              LoadedTSchema[i].TransSchemaName[0] = EOS;
            }
        }
    }
  return pTSchema;
}

/*----------------------------------------------------------------------
  IsTranslateTag
  Indicates if the translation schema exports the textual format
  of the document (in this case, don't export namespace declarations).
  Currently used for HTMLTT schema only.
  (In the next release, this function will be replaced by a new field
  in the translation schema structure)
  ----------------------------------------------------------------------*/
ThotBool IsTranslateTag (PtrTSchema pTSch, PtrSSchema pSSch)
{
  int                 i;
  ThotBool            found, translate;

  found = FALSE;
  translate = FALSE;
  i = 0;

  if ((pSSch->SsName != NULL) &&
      (strcmp (pSSch->SsName, "HTML") == 0))
    {
      do
        {
          if (LoadedTSchema[i].pTransSchema == pTSch)
            {
              found = TRUE;
              if (strcmp (LoadedTSchema[i].TransSchemaName, "HTMLTT"))
                translate = TRUE;
            }
          if (!found)
            i++;
        }
      while (!found && i < MAX_TSCHEMAS);
    }
  else
    translate = TRUE;

  return translate;
}
