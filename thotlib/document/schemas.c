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
static APresentation LoadedPSchema[MAX_PSCHEMAS];
#endif /* NODISPLAY */

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
#ifndef NODISPLAY
   int                 i;

   for (i = 0; i < MAX_PSCHEMAS; i++)
      {
      LoadedPSchema[i].pPresSchema = NULL;
      LoadedPSchema[i].UsageCount = 0;
      LoadedPSchema[i].PresSchemaName[0] = EOS;
      }
#endif /* NODISPLAY */
}

#ifndef NODISPLAY
/*----------------------------------------------------------------------
   LoadPresentationSchema charge le schema de presentation de nom	
   schemaName pour le schema de structure pointe' par pSS et		
   retourne un pointeur sur le schema charge' ou NULL si echec.      
  ----------------------------------------------------------------------*/
PtrPSchema LoadPresentationSchema (Name schemaName, PtrSSchema pSS)
{
   PtrPSchema          pPSchema;
   int                 i;
   Name                pschemaName;
   ThotBool            found;

   pPSchema = NULL;
   /* cherche dans la table si le schema est deja charge */
   i = 0;
   found = FALSE;
   do
      {
      if (LoadedPSchema[i].pPresSchema != NULL)
	 /* compare les noms schemaName et PresSchemaName */
	 if (strcmp (schemaName, LoadedPSchema[i].PresSchemaName) == 0)
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
      strncpy (pschemaName, schemaName, MAX_NAME_LENGTH);
      pPSchema = ReadPresentationSchema (pschemaName, pSS);
      if (pPSchema != NULL)
	 /* met le nouveau schema dans la table des schemas charges */
	 /* cherche une entree libre dans la table */
	 {
	 i = 0;
	 do
	    i++;
	 while (LoadedPSchema[i].pPresSchema != NULL && i < MAX_PSCHEMAS - 1);
	 if (LoadedPSchema[i].pPresSchema == NULL)
	    /* on a trouve une entree libre, on l'utilise */
	    {
	    LoadedPSchema[i].UsageCount = 1;
	    LoadedPSchema[i].pPresSchema = pPSchema;
	    strncpy (LoadedPSchema[i].PresSchemaName, schemaName,
		      MAX_NAME_LENGTH);
	    }
	 }
      }
   if (pPSchema != NULL)
      strncpy (pSS->SsDefaultPSchema, schemaName, MAX_NAME_LENGTH);
   /* rend la valeur de retour */
   return pPSchema;
}

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
   FreePresentationSchema						
   	libere le schema de presentation pointe par pPSchema,		
   ainsi que toutes les regles de presentation qu'il pointe.	
   pSS pointe le schema de structure auquel le schema de           
   presentation a liberer est associe.                             
  ----------------------------------------------------------------------*/
void               FreePresentationSchema (PtrPSchema pPSchema, PtrSSchema pSS)
{
   APresentation      *pPres;
   AttributePres      *pAttrPres;
   int                 i, j;
   ThotBool            delete;
   PtrHostView         pHostView, pNextHostView;

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
      /* libere les descripteir de vues hotes */
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
#endif /* NODISPLAY */

/*----------------------------------------------------------------------
   LoadNatureSchema charge la nature definie dans la regle rule du	
   schema de structure pSS. Si le 1er octet de PSchName est nul on	
   propose a l'utilisateur le schema de presentation par defaut	
   defini dans le schema de structure, sinon on propose le schema de  
   presentation de nom PSchName.					
  ----------------------------------------------------------------------*/
void LoadNatureSchema (PtrSSchema pSS, char *PSchName, int rule)
{
   Name          schName;
   PtrSSchema    pNatureSS;

   /* utilise le nom de la nature comme nom de fichier. */
   /* copie le nom de nature dans schName */
   strncpy (schName, pSS->SsRule[rule - 1].SrOrigNat, MAX_NAME_LENGTH);
   /* cree un schema de structure et le charge depuis le fichier */
   GetSchStruct (&pNatureSS);
   if (!ReadStructureSchema (schName, pNatureSS))
      /* echec */
      {
      FreeSchStruc (pNatureSS);
      pSS->SsRule[rule - 1].SrSSchemaNat = NULL;
      }
   else
      /* chargement du schema de structure reussi */
      {
      /* traduit le schema de structure dans la langue de l'utilisateur */
      ConfigTranslateSSchema (pNatureSS);
      pSS->SsRule[rule - 1].SrSSchemaNat = pNatureSS;
#ifndef NODISPLAY
      if (PSchName != NULL && PSchName[0] != EOS)
	 /* l'appelant indique un schema de presentation, on essaie de le
	    charger */
	 {
	 strncpy (schName, PSchName, MAX_NAME_LENGTH);
	 pNatureSS->SsPSchema = LoadPresentationSchema (schName, pNatureSS);
	 }
      if (PSchName == NULL || PSchName[0] == EOS ||
	  pNatureSS->SsPSchema == NULL)
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
	 pNatureSS->SsPSchema = LoadPresentationSchema (schName, pNatureSS);
	 }
      if (pNatureSS->SsPSchema == NULL)
	 /* echec chargement schema */
	 {
	 TtaDisplayMessage (INFO, TtaGetMessage (LIB, TMSG_INCORRECT_STR_FILE),
			    schName);
	 FreeSchStruc (pNatureSS);
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
int          CreateNature (char *SSchName, char *PSchName, PtrSSchema pSS)
{
   SRule     *pRule;
   int       ret;
   ThotBool  found;

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
	 if (pSS->SsPSchema != NULL)
	    pSS->SsPSchema->PsElemPRule[ret - 1] = NULL;
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
	    LoadNatureSchema (pSS, PSchName, ret);
	    if (pRule->SrSSchemaNat == NULL)
	       /* echec chargement */
	       ret = 0;
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
void LoadSchemas (char *SSchName, char *PSchName,
		  PtrSSchema *pSS, PtrSSchema pLoadedSS,
		  ThotBool extension)
{
   Name         schName;

   strncpy (schName, SSchName, MAX_NAME_LENGTH);
   /* cree le schema de structure et charge le fichier dedans */
   if (pLoadedSS == NULL)
      {
      GetSchStruct (pSS);
      if (!ReadStructureSchema (SSchName, *pSS))
	 {
#ifndef NODISPLAY
	 TtaDisplayMessage (INFO, TtaGetMessage (LIB, TMSG_INCORRECT_STR_FILE),
			    SSchName);
#endif  /* NODISPLAY */
         FreeSchStruc (*pSS);
         *pSS = NULL;	     
	 }
      else if ((*pSS)->SsExtension != extension)
	 {
	 /* on voulait un schema d'extension et ce n'en est pas un, ou
	    l'inverse */
	 FreeSchStruc (*pSS);
	 *pSS = NULL;
	 }
      else
	 {
	 /* traduit les noms du schema dans la langue de l'utilisateur */
	 ConfigTranslateSSchema (*pSS);
#ifndef NODISPLAY
	 if (ThotLocalActions[T_initevents] != NULL)
	    (*ThotLocalActions[T_initevents]) (*pSS);
#endif  /* NODISPLAY */
	 }
      }
#ifndef NODISPLAY
   else
      *pSS = pLoadedSS;
   if (*pSS)
      {
      if ((*pSS)->SsExtension)
         /* pour eviter que ReadPresentationSchema ne recharge le schema de
	    structure */
	 (*pSS)->SsRootElem = 1;
      if (PSchName != NULL && PSchName[0] != EOS)
	  {
          /* l'appelant specifie le schema de presentation a prendre, on
	     essaie de le charger */
	  strncpy (schName, PSchName, MAX_NAME_LENGTH);
	  (*pSS)->SsPSchema = LoadPresentationSchema (schName, *pSS);
	  }
      if (PSchName == NULL || PSchName[0] == EOS ||
	  (*pSS)->SsPSchema == NULL)
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
	 (*pSS)->SsPSchema = LoadPresentationSchema (schName, *pSS);
	 }
      if ((*pSS)->SsPSchema == NULL)
	 {
         /* echec chargement schema */
         TtaDisplayMessage (INFO, TtaGetMessage (LIB, TMSG_INCORRECT_PRS_FILE),
			    schName);
         FreeSchStruc (*pSS);
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
ThotBool            FreeNature (PtrSSchema pSS, PtrSSchema pNatureSS)
{
   ThotBool            ret;

   ret = FALSE;
   /* Cherche tous les schemas de structure qui utilisaient cette nature */
   if (FreeNatureRules (pSS, pNatureSS))
      {
      ret = TRUE;
#ifndef NODISPLAY
      if (pNatureSS->SsPSchema != NULL)
	 /* libere le schema de presentation associe' */
	 FreePresentationSchema (pNatureSS->SsPSchema, pNatureSS);
#endif  /* NODISPLAY */
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
void                FreeDocumentSchemas (PtrDocument pDoc)
{
   PtrSSchema       pSS, pNextSS;
   SRule            *pRule;
   int              i;

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
#ifndef NODISPLAY
      /* libere le schema de presentation */
      if (pSS->SsPSchema != NULL)
	 FreePresentationSchema (pSS->SsPSchema, pSS);
#endif  /* NODISPLAY */
      /* libere le schema de structure */
      FreeSchStruc (pSS);
      pSS = pNextSS;
      }
   pDoc->DocSSchema = NULL;
}

#ifndef NODISPLAY
/*----------------------------------------------------------------------
   AddGuestViews
   add the guest views defined in presentation schema assosicated with pSS
   to the list of guest views of document view pViewDescr.
  ----------------------------------------------------------------------*/
static void      AddGuestViews (PtrSSchema pSS, DocViewDescr *pViewDescr)
{
  PtrPSchema         pPresSch;
  PtrHostView        pHostView;
  PtrGuestViewDescr  pGuestView;
  int                i;
  ThotBool           found;

  pPresSch = pSS->SsPSchema;
  /* check all views defined in pPresSch */
  for (i = 0; i < pPresSch->PsNViews; i++)
     {
     pHostView = pPresSch->PsHostViewList[i];
     found = FALSE;
     while (pHostView && !found)
        if (strcmp (pViewDescr->DvSSchema->SsPSchema->PsView[pViewDescr->DvPSchemaView - 1],
		     pHostView->HostViewName) == 0)
	   found = TRUE;
        else
	   pHostView = pHostView->NextHostView;
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
            AddGuestViews (pSS, &pDoc->DocView[i]);
}

/*----------------------------------------------------------------------
   AddAllGuestViews
   add the guest views of presentation schema attached to pSS to the list
   of guest views of document view pViewDescr.
   Add also the guest views of presentation schemas attached to all
   natures used in pSS.
  ----------------------------------------------------------------------*/
static void      AddAllGuestViews (PtrSSchema pSS, DocViewDescr *pViewDescr)
{
   SRule              *pRule;
   int                 i;

   while (pSS != NULL)
      {
      /* get guest views in the presentation schema of this structure schema */
      AddGuestViews (pSS, pViewDescr);
      /* look for nature schemas used in this structure schema */
      for (i = 0; i < pSS->SsNRules; i++)
         {
	 pRule = &pSS->SsRule[i];
	 if (pRule->SrConstruct == CsNatureSchema)
	    if (pRule->SrSSchemaNat != NULL)
	       /* the structure schema of this nature is loaded */
	       AddAllGuestViews (pRule->SrSSchemaNat, pViewDescr);
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
   AddAllGuestViews (pDoc->DocSSchema, &pDoc->DocView[view - 1]);
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
