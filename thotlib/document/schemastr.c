
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

/* ======================================================================= */
/* |                                                                    | */
/* |                           Projet THOT                              | */
/* |                                                                    | */
/* |                    Chargement et Liberation                        | */
/* |    des schemas de structure sans schemas de presentation           | */
/* |                                                                    | */
/* |                    V. Quint        Juin 1984                       | */
/* |                                                                    | */
/* ======================================================================= */

#include "thot_sys.h"
#include "constmedia.h"
#include "libmsg.h"
#include "message.h"
#include "typemedia.h"

/* variables importees */
#define EXPORT extern
#include "edit.var"

/* procedures importees de l'editeur */
#include "config_f.h"
#include "memory_f.h"
#include "readstr_f.h"
#include "schemas_f.h"

void                InitNatures ()
{
}

/* ---------------------------------------------------------------------- */
/* | LoadNat    Charge la nature definie dans la regle r du schema de   | */
/* | structure pointe par PSchStr.                                      | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                LoadNat (PtrSSchema PSchStr, Name NomSchPrs, int r)

#else  /* __STDC__ */
void                LoadNat (PSchStr, NomSchPrs, r)
PtrSSchema        PSchStr;
Name                 NomSchPrs;
int         r;

#endif /* __STDC__ */

{
   Name                 fname;
   PtrSSchema        PtrStrNat;

   /* utilise le nom de la nature comme nom de fichier. */
   /* copie le nom de nature dans fname */
   strncpy (fname, PSchStr->SsRule[r - 1].SrOrigNat, MAX_NAME_LENGTH);
   /* cree un schema de structure et le charge depuis le fichier */
   GetSchStruct (&PtrStrNat);
   if (!RdSchStruct (fname, PtrStrNat))
      /* echec */
     {
	FreeSStruct (PtrStrNat);
	PSchStr->SsRule[r - 1].SrSSchemaNat = NULL;
     }
   else
      /* chargement du schema de structure reussi */
   if (PtrStrNat->SsExtension)
      /* c'est une extension de schema, on abandonne */
     {
	FreeSStruct (PtrStrNat);
	PtrStrNat = NULL;
	PSchStr->SsRule[r - 1].SrSSchemaNat = NULL;
     }
   else
     {
	/* traduit le schema de structure dans la langue de l'utilisateur */
	ConfigTranslateSSchema (PtrStrNat);
	PSchStr->SsRule[r - 1].SrSSchemaNat = PtrStrNat;
     }
}


/* ---------------------------------------------------------------------- */
/* |    ajouteregle     ajoute une nouvelle regle a la fin de la table  | */
/* |                    des regles                                      | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         ajouteregle (int * ret, PtrSSchema pSS)

#else  /* __STDC__ */
static void         ajouteregle (ret, pSS)
int        *ret;
PtrSSchema        pSS;

#endif /* __STDC__ */

{
   if (pSS->SsNRules >= MAX_RULES_SSCHEMA)
     {
	TtaDisplaySimpleMessage (FATAL, LIB, LIB_RULES_TABLE_FULL);
	*ret = 0;
	/* Table de regles saturee */
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


/* ---------------------------------------------------------------------- */
/* |    CreeNature   retourne le numero de la regle definissant le type | */
/* |    de nom NomSchStr dans le schema de structure pointe par pSS.    | */
/* |    S'il n'existe pas de type de ce nom, ajoute une regle de type   | */
/* |    CsNatureSchema au schema de structure et charge le schema de         | */
/* |    structure de nom NomSchStr; retourne le numero de la regle      | */
/* |    creee ou 0 si echec creation.                                   | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
int         CreeNature (Name NomSchStr, Name NomSchPrs, PtrSSchema pSS)

#else  /* __STDC__ */
int         CreeNature (NomSchStr, NomSchPrs, pSS)
Name                 NomSchStr;
Name                 NomSchPrs;
PtrSSchema        pSS;

#endif /* __STDC__ */

{
   int         ret;
   boolean             found;
   SRule              *pRule;

   /* schema de structure du document */
   /* cherche si le type existe deja dans le schema de structure */
   found = FALSE;
   ret = 0;
   do
     {
	ret++;
	pRule = &pSS->SsRule[ret - 1];
	if (pRule->SrConstruct == CsNatureSchema)
	   if (strcmp (pRule->SrOrigNat, NomSchStr) == 0)
	      found = TRUE;
     }
   while (!(found || ret >= pSS->SsNRules));
   if (!found)
      /* il n'existe pas, cherche une entree libre dans la table des */
      /* regles */
     {
	if (pSS->SsFirstDynNature == 0)
	   /* pas encore de nature chargee dynamiquement */
	   ajouteregle (&ret, pSS);
	/* ajoute une regle a la fin de la table */
	else
	   /* il y a deja des natures dynamiques */
	   /* cherche s'il y en a une libre */
	  {
	     ret = pSS->SsFirstDynNature;
	     while (ret <= pSS->SsNRules && pSS->SsRule[ret - 1].SrSSchemaNat != NULL
		)
		ret++;
	     if (ret > pSS->SsNRules)
		/* pas de regle libre, on ajoute une regle a la fin de la */
		/* table */
		ajouteregle (&ret, pSS);
	  }
	if (ret > 0)
	   /* il y a une entree libre (celle de rang ret) */
	   /* remplit la regle nature */
	  {
	     pRule = &pSS->SsRule[ret - 1];
	     strncpy (pRule->SrOrigNat, NomSchStr, MAX_NAME_LENGTH);
	     strncpy (pRule->SrName, NomSchStr, MAX_NAME_LENGTH);
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
		LoadNat (pSS, NomSchPrs, ret);
		if (pRule->SrSSchemaNat == NULL)
		   ret = 0;
		/* echec chargement */
	     }
     }
   return ret;
}

/* ---------------------------------------------------------------------- */
/* | LoadSchemas  charge en memoire, pour le document pointe par pDocu, | */
/* | le schema de structure de nom NomSchStr.                           | */
/* | Si pSCharge n'est pas NULL, on ne charge rien                      | */
/* | Extension indique s'il s'agit d'une extension de schema ou d'un    | */
/* | schema de structure complet.                                       | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                LoadSchemas (Name NomSchStr, Name NomSchPrs, PtrSSchema * pSS, PtrSSchema pSCharge, boolean Extension)

#else  /* __STDC__ */
void                LoadSchemas (NomSchStr, NomSchPrs, pSS, pSCharge, Extension)
Name                 NomSchStr;
Name                 NomSchPrs;
PtrSSchema       *pSS;
PtrSSchema        pSCharge;
boolean             Extension;

#endif /* __STDC__ */

{
   Name                 fname;

   strncpy (fname, NomSchStr, MAX_NAME_LENGTH);
   /* cree le schema de structure et charge le fichier dedans */
   if (pSCharge == NULL)
     {
	GetSchStruct (pSS);
	if (!RdSchStruct (fname, *pSS))
	  {
	     FreeSStruct (*pSS);
	     *pSS = NULL;
	  }
	else if ((*pSS)->SsExtension != Extension)
	   /* on voulait un schema d'extension et ce n'en est pas un, */
	   /* ou l'inverse */
	  {
	     FreeSStruct (*pSS);
	     *pSS = NULL;
	  }
	else
	   /* traduit les noms du schema dans la langue de l'utilisateur */
	   ConfigTranslateSSchema (*pSS);
     }
}


/* ---------------------------------------------------------------------- */
/* | LoadExtension charge en memoire, pour le document pDoc, le schema  | */
/* |     d'extension de nom NomSchStr.                                  | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
PtrSSchema        LoadExtension (Name NomSchStr, Name NomSchPrs, PtrDocument pDoc)

#else  /* __STDC__ */
PtrSSchema        LoadExtension (NomSchStr, NomSchPrs, pDoc)
Name                 NomSchStr;
Name                 NomSchPrs;
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   PtrSSchema        pExtens, pExtensPrec;

   pExtens = NULL;
   if (pDoc->DocSSchema != NULL)
     {
	/* charge le schema d'extension demande' */
	LoadSchemas (NomSchStr, NomSchPrs, &pExtens, NULL, TRUE);
	if (pExtens != NULL)
	  {
	     if (NomSchPrs[0] != '\0')
		strncpy (pExtens->SsDefaultPSchema, NomSchPrs, MAX_NAME_LENGTH - 1);
	     /* cherche le dernier schema d'extension du document */
	     pExtensPrec = pDoc->DocSSchema;
	     while (pExtensPrec->SsNextExtens != NULL)
		pExtensPrec = pExtensPrec->SsNextExtens;
	     /* ajoute le nouveau schema d'extension a la fin de la */
	     /* chaine des schemas d'extension du document */
	     pExtens->SsPrevExtens = pExtensPrec;
	     pExtensPrec->SsNextExtens = pExtens;
	     pExtens->SsNextExtens = NULL;
	  }
     }
   return pExtens;
}


/* ---------------------------------------------------------------------- */
/* |    LibRegleNat     cherche dans le schema de structure pointe' par | */
/* |    pSchStr les regles de nature qui font reference au schema       | */
/* |    pointe par pSS.                                                 | */
/* |    S'il y en a, retourne Vrai, annule ces regles et traite de meme | */
/* |    les autres natures. S'il n'y en a pas, retourne Faux.           | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static boolean      LibRegleNat (PtrSSchema pSchStr, PtrSSchema pSS)
#else  /* __STDC__ */
static boolean      LibRegleNat (pSchStr, pSS)
PtrSSchema        pSchStr;
PtrSSchema        pSS;

#endif /* __STDC__ */
{
   int                 r;
   SRule              *pRule;
   boolean             ret;

   ret = FALSE;
   if (pSchStr != NULL)
      /* parcourt les regles de ce schemas */
      for (r = 0; r < pSchStr->SsNRules; r++)
	{
	   pRule = &pSchStr->SsRule[r];
	   if (pRule->SrConstruct == CsNatureSchema)
	      /* c'est une regle de nature */
	      if (pRule->SrSSchemaNat == pSS)
		 /* elle fait reference a la nature supprimee */
		{
		   ret = TRUE;
		   pRule->SrSSchemaNat = NULL;
		}
	}
   return ret;
}


/* ---------------------------------------------------------------------- */
/* |    LibNature       Si le schema de structure pointe' par pSPere    | */
/* |    contient une regle de nature pour le schema pointe' par pSS,    | */
/* |    retourne Vrai et libere le schema de structure pointe par pSS   | */
/* |    et son schema de presentation.                                  | */
/* |    Retourne faux sinon.                                            | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
boolean             LibNature (PtrSSchema pSPere, PtrSSchema pSS)

#else  /* __STDC__ */
boolean             LibNature (pSPere, pSS)
PtrSSchema        pSPere;
PtrSSchema        pSS;

#endif /* __STDC__ */

{
   boolean             ret;

   ret = FALSE;
   /* Cherche tous les schemas de structure qui utilisaient cette nature */
   if (LibRegleNat (pSPere, pSS))
     {
	ret = TRUE;
	FreeSStruct (pSS);
     }
   return ret;
}


/* ---------------------------------------------------------------------- */
/* |    LibSchemas      libere tous les schemas de structure et de      | */
/* |    presentation utilises par le document dont le descripteur est   | */
/* |    pointe par pDoc.                                                | */
/* |    Pour les schemas de presentation, la liberation n'est effective | */
/* |    que s'ils ne sont pas utilises par d'autres documents.          | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                LibSchemas (PtrDocument pDoc)
#else  /* __STDC__ */
void                LibSchemas (pDoc)
PtrDocument         pDoc;

#endif /* __STDC__ */
{
   PtrSSchema        pSc1, pSSuiv;

   pSc1 = pDoc->DocSSchema;
   /* libere le schema de structure du document et ses extensions */
   while (pSc1 != NULL)
     {
	pSSuiv = pSc1->SsNextExtens;
	/* libere les schemas de nature pointes par les regles de structure */
      /***for (i = 0; i < pSc1->SsNRules; i++)
	{
	  pRule = &pSc1->SsRule[i];
	}***/
	/* libere le schemas de structure */
	FreeSStruct (pSc1);
	pSc1 = pSSuiv;
     }
   pDoc->DocSSchema = NULL;
}
