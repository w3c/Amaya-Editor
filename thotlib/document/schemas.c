/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
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
#include "fileaccess.h"

typedef struct _APresentation
  {
     PtrPSchema          pPresSchema;	/* pointeur sur le schema */
     int                 UsageCount;	/* nombre d'utilisations de ce schema */
     Name                PresSchemaName;	/* nom du schema de presentation */
  }
APresentation;

#include "appstruct.h"
#include "appdialogue.h"

#define THOT_EXPORT extern
#include "edit_tv.h"
#include "appdialogue_tv.h"
#include "modif_tv.h"

/* table des schemas de presentation charges */
static APresentation LoadedPSchema[MAX_PSCHEMAS];

#include "config_f.h"
#include "memory_f.h"
#include "readstr_f.h"
#include "readprs_f.h"
#include "schemas_f.h"
#include "tree_f.h"

/*----------------------------------------------------------------------
   InitNatures     initialise la table des schemas de presentation 
   charges.                                        
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                InitNatures ()

#else  /* __STDC__ */
void                InitNatures ()
#endif				/* __STDC__ */
{
   int                 i;

   for (i = 0; i < MAX_PSCHEMAS; i++)
     {
	LoadedPSchema[i].pPresSchema = NULL;
	LoadedPSchema[i].UsageCount = 0;
	LoadedPSchema[i].PresSchemaName[0] = EOS;
     }
}


/*----------------------------------------------------------------------
   LoadPresentationSchema charge le schema de presentation de nom	
   schemaName pour le schema de structure pointe' par pSS et		
   retourne un pointeur sur le schema charge' ou NULL si echec.      
  ----------------------------------------------------------------------*/
#ifdef __STDC__
PtrPSchema          LoadPresentationSchema (Name schemaName, PtrSSchema pSS)

#else  /* __STDC__ */
PtrPSchema          LoadPresentationSchema (schemaName, pSS)
Name                schemaName;
PtrSSchema          pSS;

#endif /* __STDC__ */

{
   PtrPSchema          pPSchema;
   int                 i;
   ThotBool            found;

   pPSchema = NULL;
   /* cherche dans la table si le schema est deja charge */
   i = 0;
   found = FALSE;
   do
     {
	if (LoadedPSchema[i].pPresSchema != NULL)
	   /* compare les noms schemaName et PresSchemaName */
	   if (ustrcmp (schemaName, LoadedPSchema[i].PresSchemaName) == 0)
	      found = TRUE;
	if (!found)
	   i++;
     }
   while (!found && i < MAX_PSCHEMAS);
   if (found)
      /* ce schema est dans la table des schemas charges */
     {
	LoadedPSchema[i].UsageCount++;	/* une utilisation de plus */
	pPSchema = LoadedPSchema[i].pPresSchema;
     }
   else
      /* c'est un nouveau schema, il faut le charger */
     {
	Name pschemaName;

	ustrncpy(pschemaName, schemaName, MAX_NAME_LENGTH);
	pPSchema = ReadPresentationSchema (pschemaName, pSS);
	if (pPSchema != NULL)
	   /* met le nouveau schema dans la table des schemas charges */
	   /* cherche une entree libre dans la table */
	  {
	     i = 0;
	     do
		i++;
	     while (LoadedPSchema[i].pPresSchema != NULL &&
		    i < MAX_PSCHEMAS - 1);
	     if (LoadedPSchema[i].pPresSchema == NULL)
		/* on a trouve une entree libre, on l'utilise */
	       {
		  LoadedPSchema[i].UsageCount = 1;
		  LoadedPSchema[i].pPresSchema = pPSchema;
		  ustrncpy (LoadedPSchema[i].PresSchemaName, schemaName,
			   MAX_NAME_LENGTH);
	       }
	  }
     }
   if (pPSchema != NULL)
      ustrncpy (pSS->SsDefaultPSchema, schemaName, MAX_NAME_LENGTH);
   /* rend la valeur de retour */
   return pPSchema;
}


/*----------------------------------------------------------------------
   FreePRuleList  libere la liste de regles de presentation dont   
   l'ancre est firstPRule.                         
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         FreePRuleList (PtrPRule * firstPRule)

#else  /* __STDC__ */
static void         FreePRuleList (firstPRule)
PtrPRule           *firstPRule;

#endif /* __STDC__ */

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
   FreePresentationSchema						
   	libere le schema de presentation pointe par pPSchema,		
   ainsi que toutes les regles de presentation qu'il pointe.	
   pSS pointe le schema de structure auquel le schema de           
   presentation a liberer est associe.                             
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                FreePresentationSchema (PtrPSchema pPSchema, PtrSSchema pSS)

#else  /* __STDC__ */
void                FreePresentationSchema (pPSchema, pSS)
PtrPSchema          pPSchema;
PtrSSchema          pSS;

#endif /* __STDC__ */

{
   APresentation      *pPres;
   AttributePres      *pAttrPres;
   int                 i, j;
   ThotBool            delete;

   delete = TRUE;
   /* parcourt la table des schemas de presentation pour trouver ce schema */
   i = 0;
   while (i < MAX_PSCHEMAS - 1 && LoadedPSchema[i].pPresSchema != pPSchema)
      i++;
   pPres = &LoadedPSchema[i];
   if (pPres->pPresSchema == pPSchema)
      /* ce schema est dans la table */
     {
	pPres->UsageCount--;
	/* une utilisation de moins */
	if (pPres->UsageCount > 0)
	   /* il y a d'autres utilisations, on ne le supprime pas */
	   delete = FALSE;
	else
	   /* c'etait la derniere utilisation, on le supprime de la table */
	  {
	     pPres->pPresSchema = NULL;
	     pPres->UsageCount = 0;
	     pPres->PresSchemaName[0] = EOS;
	  }
     }
   if (delete)
     {
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
	FreeSchPres (pPSchema);
     }
}


/*----------------------------------------------------------------------
   LoadNatureSchema charge la nature definie dans la regle rule du	
   schema de structure pSS. Si le 1er octet de PSchName est nul on	
   propose a l'utilisateur le schema de presentation par defaut	
   defini dans le schema de structure, sinon on propose le schema de  
   presentation de nom PSchName.					
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                LoadNatureSchema (PtrSSchema pSS, USTRING PSchName, int rule)

#else  /* __STDC__ */
void                LoadNatureSchema (pSS, PSchName, rule)
PtrSSchema          pSS;
USTRING              PSchName;
int                 rule;

#endif /* __STDC__ */

{
   Name                schName;
   PtrSSchema          pNatureSS;

   /* utilise le nom de la nature comme nom de fichier. */
   /* copie le nom de nature dans schName */
   ustrncpy (schName, pSS->SsRule[rule - 1].SrOrigNat, MAX_NAME_LENGTH);
   /* cree un schema de structure et le charge depuis le fichier */
   GetSchStruct (&pNatureSS);
   if (!ReadStructureSchema (schName, pNatureSS))
      /* echec */
     {
	TtaDisplayMessage (INFO, TtaGetMessage (LIB, TMSG_SCHEMA_NOT_FIND), schName);
	FreeSchStruc (pNatureSS);
	pSS->SsRule[rule - 1].SrSSchemaNat = NULL;
     }
   else
      /* chargement du schema de structure reussi */
     {
	/* traduit le schema de structure dans la langue de l'utilisateur */
	ConfigTranslateSSchema (pNatureSS);
	pSS->SsRule[rule - 1].SrSSchemaNat = pNatureSS;
	if (PSchName != NULL && PSchName[0] != EOS)
	   /* l'appelant indique un schema de presentation, on essaie de le
	      charger */
	  {
	     ustrncpy (schName, PSchName, MAX_NAME_LENGTH);
	     pNatureSS->SsPSchema = LoadPresentationSchema (schName, pNatureSS);
	  }
	if (PSchName == NULL || PSchName[0] == EOS || pNatureSS->SsPSchema == NULL)
	   /* pas de schema de presentation particulier demande' par l'appelant */
	   /* ou schema demande' inaccessible */
	  {
	     /* on consulte le fichier .conf */
	     if (!ConfigGetPSchemaNature (pSS, pSS->SsRule[rule - 1].SrOrigNat,
					  schName))
		/* le fichier .conf ne donne pas de schema de presentation pour */
		/* cette nature, on le demande a l'utilisateur */
	       {
		  ustrncpy (schName, pNatureSS->SsDefaultPSchema, MAX_NAME_LENGTH);
		  if (ThotLocalActions[T_presentchoice] != NULL)
		     (*ThotLocalActions[T_presentchoice]) (pNatureSS, schName);
	       }
	     /* cree un nouveau schema de presentation et le charge depuis le
	        fichier */
	     pNatureSS->SsPSchema = LoadPresentationSchema (schName, pNatureSS);
	  }
	if (pNatureSS->SsPSchema == NULL)
	   /* echec chargement schema */
	  {
	     TtaDisplayMessage (INFO, TtaGetMessage (LIB, TMSG_SCHEMA_NOT_FIND),
				schName);
	     FreeSchStruc (pNatureSS);
	     pSS->SsRule[rule - 1].SrSSchemaNat = NULL;
	  }
	if (ThotLocalActions[T_initevents] != NULL)
	  (*ThotLocalActions[T_initevents]) (pNatureSS);
     }
}


/*----------------------------------------------------------------------
   AppendSRule     ajoute une nouvelle regle a la fin de la table  
   des regles.                                     
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         AppendSRule (int *ret, PtrSSchema pSS)

#else  /* __STDC__ */
static void         AppendSRule (ret, pSS)
int                *ret;
PtrSSchema          pSS;

#endif /* __STDC__ */

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
   CreateNature      retourne le numero de la regle definissant le 
   type de nom SSchName dans le schema de structure pointe par	
   pSS.                                                            
   S'il n'existe pas de type de ce nom, ajoute une regle de type   
   CsNatureSchema au schema de structure et charge le schema de	
   structure de nom SSchName; retourne le numero de la regle creee	
   ou 0 si echec creation.						
   En cas de chargement de schema, le nom de schema de             
   presentation PSchName est propose' a l'utilisateur plutot que	
   le schema de presentation par defaut defini dans le schema de	
   structure, sauf si le premier octet de PSchName est nul.	
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                 CreateNature (USTRING SSchName, USTRING PSchName, PtrSSchema pSS)

#else  /* __STDC__ */
int                 CreateNature (SSchName, PSchName, pSS)
USTRING              SSchName;
USTRING              PSchName;
PtrSSchema          pSS;

#endif /* __STDC__ */

{
   SRule              *pRule;
   int                 ret;
   ThotBool            found;

   /* cherche si le type existe deja dans le schema de structure */
   found = FALSE;
   ret = 0;
   do
     {
	pRule = &pSS->SsRule[ret++];
	if (pRule->SrConstruct == CsNatureSchema)
	   if (ustrcmp (pRule->SrOrigNat, SSchName) == 0)
	      found = TRUE;
     }
   while (!found && ret < pSS->SsNRules);
   if (!found)
      /* il n'existe pas, cherche une entree libre dans la table des */
      /* regles */
     {
	if (pSS->SsFirstDynNature == 0)
	   /* pas encore de nature chargee dynamiquement */
	   AppendSRule (&ret, pSS);
	/* ajoute une regle a la fin de la table */
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
	     ustrncpy (pRule->SrOrigNat, SSchName, MAX_NAME_LENGTH);
	     ustrncpy (pRule->SrName, SSchName, MAX_NAME_LENGTH);
	     pRule->SrAssocElem = FALSE;
	     pRule->SrNDefAttrs = 0;
	     pRule->SrConstruct = CsNatureSchema;
	     pRule->SrSSchemaNat = NULL;
	     /* initialise le pointeur sur les regles de presentation qui */
	     /* correspondent a cette nouvelle regle de structure */
	     if (pSS->SsPSchema != NULL)
		pSS->SsPSchema->PsElemPRule[ret - 1] = NULL;
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
		LoadNatureSchema (pSS, PSchName, ret);
		if (pRule->SrSSchemaNat == NULL)
		   /* echec chargement */
		   ret = 0;
	     }
     }
   return ret;
}


/*----------------------------------------------------------------------
   LoadSchemas  charge en memoire, pour le document pointe par pDocu, 
   le schema de structure de nom SSchName ainsi que son schema de	
   presentation. Si PSchName commence par un octet nul, on charge	
   le schema de presentation par defaut defini dans le schema de      
   structure, sinon on charge le schema de presentation de nom        
   PSchName.								
   Si pLoadedSS n'est pas NULL, on ne charge que le schema de		
   presentation pour le schema de structure pointe' par pLoadedSS.	
   extension indique s'il s'agit d'une extension de schema ou d'un    
   schema de structure complet.                                       
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                LoadSchemas (USTRING SSchName, USTRING PSchName, PtrSSchema * pSS, PtrSSchema pLoadedSS, ThotBool extension)

#else  /* __STDC__ */
void                LoadSchemas (SSchName, PSchName, pSS, pLoadedSS, extension)
USTRING              SSchName;
USTRING              PSchName;
PtrSSchema         *pSS;
PtrSSchema          pLoadedSS;
ThotBool            extension;

#endif /* __STDC__ */

{
   Name                schName;

   ustrncpy (schName, SSchName, MAX_NAME_LENGTH);
   /* cree le schema de structure et charge le fichier dedans */
   if (pLoadedSS == NULL)
     {
	GetSchStruct (pSS);
	if (!ReadStructureSchema (schName, *pSS))
	  {
	     TtaDisplayMessage (INFO, TtaGetMessage (LIB, TMSG_SCHEMA_NOT_FIND),
				schName);
	     FreeSchStruc (*pSS);
	     *pSS = NULL;	     
	  }
	else if ((*pSS)->SsExtension != extension)
	   /* on voulait un schema d'extension et ce n'en est pas un,
	      ou l'inverse */
	  {
	     FreeSchStruc (*pSS);
	     *pSS = NULL;
	  }
	else
	  {
	     /* traduit les noms du schema dans la langue de l'utilisateur */
	     ConfigTranslateSSchema (*pSS);
	     if (ThotLocalActions[T_initevents] != NULL)
	       (*ThotLocalActions[T_initevents]) (*pSS);
	  }
     }
   else
      *pSS = pLoadedSS;
   if (*pSS)
     {
	if ((*pSS)->SsExtension)
	   /* pour eviter que ReadPresentationSchema recharge le schema de
	      structure */
	   (*pSS)->SsRootElem = 1;
	if (PSchName != NULL && PSchName[0] != EOS)
	   /* l'appelant specifie le schema de presentation a prendre, on
	      essaie de le charger */
	  {
	     ustrncpy (schName, PSchName, MAX_NAME_LENGTH);
	     (*pSS)->SsPSchema = LoadPresentationSchema (schName, *pSS);
	  }
	if (PSchName == NULL || PSchName[0] == EOS || (*pSS)->SsPSchema == NULL)
	   /* pas de presentation specifiee par l'appelant, ou schema specifie'
	      inaccessible */
	  {
	     /* on consulte le fichier de configuration */
	     if (!ConfigDefaultPSchema ((*pSS)->SsName, schName))
		/* le fichier de configuration ne dit rien, on demande a
		   l'utilisateur */
	       {
		  /* propose la presentation par defaut definie dans le schema
		     de structure */
		  ustrncpy (schName, (*pSS)->SsDefaultPSchema, MAX_NAME_LENGTH);
		  if (ThotLocalActions[T_presentchoice] != NULL)
		     (*ThotLocalActions[T_presentchoice]) (*pSS, schName);
	       }
	     /* charge le schema de presentation depuis le fichier */
	     (*pSS)->SsPSchema = LoadPresentationSchema (schName, *pSS);
	  }
	if ((*pSS)->SsPSchema == NULL)
	   /* echec chargement schema */
	  {
	     TtaDisplayMessage (INFO, TtaGetMessage (LIB, TMSG_SCHEMA_NOT_FIND),
				schName);
	     FreeSchStruc (*pSS);
	     *pSS = NULL;
	  }
     }
}


/*----------------------------------------------------------------------
   LoadExtension charge en memoire, pour le document pDoc, le schema  
   d'extension de nom SSchName et son schema de presentation de    
   nom PSchName.                                                   
  ----------------------------------------------------------------------*/
#ifdef __STDC__
PtrSSchema          LoadExtension (USTRING SSchName, USTRING PSchName, PtrDocument pDoc)

#else  /* __STDC__ */
PtrSSchema          LoadExtension (SSchName, PSchName, pDoc)
USTRING              SSchName;
USTRING              PSchName;
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   PtrSSchema          pExtens, pPrevExtens;
   ThotBool            extensionExist;

   pExtens = NULL;
   if (pDoc->DocSSchema != NULL)
     {
	/* cherche si ce schema d'extension existe deja pour le document */
	pExtens = pDoc->DocSSchema->SsNextExtens;
	extensionExist = FALSE;
	while (pExtens != NULL && !extensionExist)
	   if (ustrcmp (pExtens->SsName, SSchName) == 0)
	      /* le schema d'extension existe deja */
	      extensionExist = TRUE;
	   else
	      pExtens = pExtens->SsNextExtens;
	if (!extensionExist)
	   /* le schema d'extension n'existe pas, on le charge */
	  {
	     if (PSchName == NULL || PSchName[0] == EOS)
		/* pas de schema de presentation precise' */
		/* cherche le schema de presentation de l'extension prevu */
		/* dans le fichier .conf pour ce type de document */
		ConfigGetPSchemaNature (pDoc->DocSSchema, SSchName, PSchName);
	     /* charge le schema d'extension demande' */
	     LoadSchemas (SSchName, PSchName, &pExtens, NULL, TRUE);
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
#ifdef __STDC__
static ThotBool     FreeNatureRules (PtrSSchema pSS, PtrSSchema pNatureSS)

#else  /* __STDC__ */
static ThotBool     FreeNatureRules (pSS, pNatureSS)
PtrSSchema          pSS;
PtrSSchema          pNatureSS;

#endif /* __STDC__ */

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
   return ret;
}


/*----------------------------------------------------------------------
   FreeNature    Si le schema de structure pointe' par pSS contient	
   une regle de nature pour le schema pointe' par pNatureSS,		
   retourne Vrai et libere le schema de structure pointe par		
   pNatureSS et son schema de presentation.            		
   Retourne faux sinon.                                              
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool            FreeNature (PtrSSchema pSS, PtrSSchema pNatureSS)

#else  /* __STDC__ */
ThotBool            FreeNature (pSS, pNatureSS)
PtrSSchema          pSS;
PtrSSchema          pNatureSS;

#endif /* __STDC__ */

{
   ThotBool            ret;

   ret = FALSE;
   /* Cherche tous les schemas de structure qui utilisaient cette nature */
   if (FreeNatureRules (pSS, pNatureSS))
     {
	ret = TRUE;
	if (pNatureSS->SsPSchema != NULL)
	   /* libere le schema de presentation associe' */
	   FreePresentationSchema (pNatureSS->SsPSchema, pNatureSS);
	/* rend la memoire */
	FreeSchStruc (pNatureSS);
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
#ifdef __STDC__
void                FreeDocumentSchemas (PtrDocument pDoc)

#else  /* __STDC__ */
void                FreeDocumentSchemas (pDoc)
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   PtrSSchema          pSS, pNextSS;
   SRule              *pRule;
   int                 i;

   pSS = pDoc->DocSSchema;
   /* libere le schema de structure du document et ses extensions */
   while (pSS != NULL)
     {
	pNextSS = pSS->SsNextExtens;
	/* libere les schemas de nature pointes par les regles de structure */
	for (i = 0; i < pSS->SsNRules; i++)
	  {
	     pRule = &pSS->SsRule[i];
	     if (pRule->SrConstruct == CsNatureSchema)
		if (pRule->SrSSchemaNat != NULL)
		   FreeNature (pSS, pRule->SrSSchemaNat);
	  }
	/* libere le schema de presentation et de structure */
	if (pSS->SsPSchema != NULL)
	   FreePresentationSchema (pSS->SsPSchema, pSS);
	FreeSchStruc (pSS);
	pSS = pNextSS;
     }
   pDoc->DocSSchema = NULL;
}

/*----------------------------------------------------------------------
   AddNature  met dans la table des natures du document pDoc          
   les schemas references par le schema de structure pSS       
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         AddNature (PtrSSchema pSS, PtrDocument pDoc)
#else  /* __STDC__ */
static void         AddNature (pSS, pDoc)
PtrSSchema          pSS;
PtrDocument         pDoc;

#endif /* __STDC__ */
{
   int                 rule, nat, nObjects;
   ThotBool            present;
   SRule              *pSRule;

#ifndef NODISPLAY
   PtrElement          pSaved;

#endif

   for (rule = 0; rule < pSS->SsNRules; rule++)
     {
	pSRule = &pSS->SsRule[rule];
	if (pSRule->SrConstruct == CsNatureSchema)
	   if (pSRule->SrSSchemaNat != NULL)
	      if (pSRule->SrSSchemaNat->SsNObjects > 0)
		{
		   /* Decompte les objets de cette nature qui sont dans */
		   /* le buffer de Copier-Couper-Coller */
		   nObjects = pSRule->SrSSchemaNat->SsNObjects;
#ifndef NODISPLAY
		   if (FirstSavedElement != NULL)
		     {
			pSaved = FirstSavedElement->PeElement;
			do
			  {
			     if (pSaved->ElStructSchema == pSRule->SrSSchemaNat
				 && pSaved->ElTypeNumber == pSRule->SrSSchemaNat->SsRootElem)
				nObjects--;
			     pSaved = FwdSearchTypedElem (pSaved, pSRule->SrSSchemaNat->SsRootElem,
						      pSRule->SrSSchemaNat);
			  }
			while (pSaved != NULL);
		     }
#endif
		   if (nObjects > 0)
		     {
			/* Si les natures contiennent elles-memes des natures  */
			/* on pourrait ecrire plusieurs fois un nom de nature. */
			/* On verifie que ce nom n'est pas dans la table */
			nat = 0;
			present = FALSE;
			while (nat < pDoc->DocNNatures && !present)
			   if (ustrcmp (pDoc->DocNatureName[nat],
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
				  ustrncpy (pDoc->DocNatureName[pDoc->DocNNatures],
					   pSRule->SrSSchemaNat->SsName, MAX_NAME_LENGTH);
				  ustrncpy (pDoc->DocNaturePresName[pDoc->DocNNatures],
					   pSRule->SrSSchemaNat->SsDefaultPSchema, MAX_NAME_LENGTH);
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

/*----------------------------------------------------------------------
   BuildDocNatureTable remplit la table des schemas utilises          
   par le document pDoc.                                   
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                BuildDocNatureTable (PtrDocument pDoc)
#else  /* __STDC__ */
void                BuildDocNatureTable (pDoc)
PtrDocument         pDoc;

#endif /* __STDC__ */
{
   PtrSSchema          pSSExtens;

   /* met le schema de structure du document en tete de la table des */
   /* natures utilisees */
   pDoc->DocNatureSSchema[0] = pDoc->DocSSchema;
   ustrncpy (pDoc->DocNatureName[0], pDoc->DocSSchema->SsName, MAX_NAME_LENGTH);
   ustrncpy (pDoc->DocNaturePresName[0], pDoc->DocSSchema->SsDefaultPSchema, MAX_NAME_LENGTH);
   pDoc->DocNNatures = 1;
   /* met dans la table des natures du document les */
   /* extensions du schema de structure du document */
   pSSExtens = pDoc->DocSSchema->SsNextExtens;
   while (pSSExtens != NULL)
     {
	/* met ce schema d'extension dans la table des natures */
	if (pDoc->DocNNatures < MAX_NATURES_DOC)
	  {
	     ustrncpy (pDoc->DocNatureName[pDoc->DocNNatures],
		      pSSExtens->SsName, MAX_NAME_LENGTH);
	     ustrncpy (pDoc->DocNaturePresName[pDoc->DocNNatures],
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
