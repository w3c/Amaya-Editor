/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
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
   Chargement et liberation des schemas de structure sans schemas
   de presentation
 */

#include "thot_sys.h"
#include "constmedia.h"
#include "libmsg.h"
#include "message.h"
#include "typemedia.h"

/* variables importees */
#define THOT_EXPORT extern
#include "edit_tv.h"

/* procedures importees de l'editeur */
#include "config_f.h"
#include "memory_f.h"
#include "readstr_f.h"
#include "schemas_f.h"

void                InitNatures ()
{
}

/*----------------------------------------------------------------------
   LoadNatureSchema    Charge la nature definie dans la regle r	
   du schema de structure pointe par pSS.				
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                LoadNatureSchema (PtrSSchema pSS, Name PSchName, int rule)

#else  /* __STDC__ */
void                LoadNatureSchema (pSS, PSchName, rule)
PtrSSchema          pSS;
Name                PSchName;
int                 rule;

#endif /* __STDC__ */

{
   PtrSSchema          pNatureSS;
   Name                schName;

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
   if (pNatureSS->SsExtension)
      /* c'est une extension de schema, on abandonne */
     {
	FreeSchStruc (pNatureSS);
	pNatureSS = NULL;
	pSS->SsRule[rule - 1].SrSSchemaNat = NULL;
     }
   else
     {
	/* traduit le schema de structure dans la langue de l'utilisateur */
	ConfigTranslateSSchema (pNatureSS);
	pSS->SsRule[rule - 1].SrSSchemaNat = pNatureSS;
     }
}


/*----------------------------------------------------------------------
   AppendSRule     ajoute une nouvelle regle a la fin de la table  
   des regles                                      
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
   CreateNature retourne le numero de la regle definissant le type 
   de nom SSchName dans le schema de structure pointe par pSS.     
   S'il n'existe pas de type de ce nom, ajoute une regle de type   
   CsNatureSchema au schema de structure et charge le schema de    
   structure de nom SSchName; retourne le numero de la regle       
   creee ou 0 si echec creation.                                   
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                 CreateNature (Name SSchName, Name PSchName, PtrSSchema pSS)

#else  /* __STDC__ */
int                 CreateNature (SSchName, PSchName, pSS)
Name                SSchName;
Name                PSchName;
PtrSSchema          pSS;

#endif /* __STDC__ */

{
   SRule              *pRule;
   int                 ret;
   boolean             found;

   /* schema de structure du document */
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
	     strncpy (pRule->SrOrigNat, SSchName, MAX_NAME_LENGTH);
	     strncpy (pRule->SrName, SSchName, MAX_NAME_LENGTH);
	     pRule->SrAssocElem = FALSE;
	     pRule->SrNDefAttrs = 0;
	     pRule->SrConstruct = CsNatureSchema;
	     pRule->SrSSchemaNat = NULL;
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
		   ret = 0;
		/* echec chargement */
	     }
     }
   return ret;
}

/*----------------------------------------------------------------------
   LoadSchemas  charge en memoire, pour le document pointe par pDocu, 
   le schema de structure de nom SSchName.                            
   Si pLoadedSS n'est pas NULL, on ne charge rien                     
   Extension indique s'il s'agit d'une extension de schema ou d'un    
   schema de structure complet.                                       
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                LoadSchemas (Name SSchName, Name PSchName, PtrSSchema * pSS, PtrSSchema pLoadedSS, boolean Extension)

#else  /* __STDC__ */
void                LoadSchemas (SSchName, PSchName, pSS, pLoadedSS, Extension)
Name                SSchName;
Name                PSchName;
PtrSSchema         *pSS;
PtrSSchema          pLoadedSS;
boolean             Extension;

#endif /* __STDC__ */

{
   Name                schName;

   strncpy (schName, SSchName, MAX_NAME_LENGTH);
   /* cree le schema de structure et charge le fichier dedans */
   if (pLoadedSS == NULL)
     {
	GetSchStruct (pSS);
	if (!ReadStructureSchema (schName, *pSS))
	  {
	     FreeSchStruc (*pSS);
	     *pSS = NULL;
	  }
	else if ((*pSS)->SsExtension != Extension)
	   /* on voulait un schema d'extension et ce n'en est pas un, */
	   /* ou l'inverse */
	  {
	     FreeSchStruc (*pSS);
	     *pSS = NULL;
	  }
	else
	   /* traduit les noms du schema dans la langue de l'utilisateur */
	   ConfigTranslateSSchema (*pSS);
     }
}


/*----------------------------------------------------------------------
   LoadExtension charge en memoire, pour le document pDoc, le schema  
   d'extension de nom SSchName.                                   
  ----------------------------------------------------------------------*/
#ifdef __STDC__
PtrSSchema          LoadExtension (Name SSchName, Name PSchName, PtrDocument pDoc)

#else  /* __STDC__ */
PtrSSchema          LoadExtension (SSchName, PSchName, pDoc)
Name                SSchName;
Name                PSchName;
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   PtrSSchema          pExtens, pPrevExtens;

   pExtens = NULL;
   if (pDoc->DocSSchema != NULL)
     {
	/* charge le schema d'extension demande' */
	LoadSchemas (SSchName, PSchName, &pExtens, NULL, TRUE);
	if (pExtens != NULL)
	  {
	     if (PSchName[0] != EOS)
		strncpy (pExtens->SsDefaultPSchema, PSchName,
			 MAX_NAME_LENGTH - 1);
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
   return pExtens;
}


/*----------------------------------------------------------------------
   FreeNatureRules cherche dans le schema de structure pointe' par 
   pSS les regles de nature qui font reference au schema pointe'	
   par pNatureSS.                                                  
   S'il y en a, retourne Vrai, annule ces regles et traite de meme 
   les autres natures. S'il n'y en a pas, retourne Faux.           
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static boolean      FreeNatureRules (PtrSSchema pSS, PtrSSchema pNatureSS)
#else  /* __STDC__ */
static boolean      FreeNatureRules (pSS, pNatureSS)
PtrSSchema          pSS;
PtrSSchema          pNatureSS;

#endif /* __STDC__ */
{
   SRule              *pRule;
   int                 rule;
   boolean             ret;

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
		{
		   ret = TRUE;
		   pRule->SrSSchemaNat = NULL;
		}
	}
   return ret;
}


/*----------------------------------------------------------------------
   FreeNature  Si le schema de structure pointe' par pSS contient  
   une regle de nature pour le schema pointe' par pNatureSS,       
   retourne Vrai et libere le schema de structure pointe par	
   	pNatureSS et son schema de presentation.			
   Retourne faux sinon.                                            
  ----------------------------------------------------------------------*/
#ifdef __STDC__
boolean             FreeNature (PtrSSchema pSS, PtrSSchema pNatureSS)

#else  /* __STDC__ */
boolean             FreeNature (pSS, pNatureSS)
PtrSSchema          pSS;
PtrSSchema          pNatureSS;

#endif /* __STDC__ */

{
   boolean             ret;

   ret = FALSE;
   /* Cherche tous les schemas de structure qui utilisaient cette nature */
   if (FreeNatureRules (pSS, pNatureSS))
     {
	ret = TRUE;
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

   pSS = pDoc->DocSSchema;
   /* libere le schema de structure du document et ses extensions */
   while (pSS != NULL)
     {
	pNextSS = pSS->SsNextExtens;
	/* libere le schemas de structure */
	FreeSchStruc (pSS);
	pSS = pNextSS;
     }
   pDoc->DocSSchema = NULL;
}
