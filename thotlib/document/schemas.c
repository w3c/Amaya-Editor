
/* ======================================================================= */
/* |                                                                    | */
/* |                    Chargement et Liberation                        | */
/* |            des schemas de structure et de presentation             | */
/* |                                                                    | */
/* |                    V. Quint  		                        | */
/* |                                                                    | */
/* ======================================================================= */


#define msglen 50		/* longueur max des messages */
#include "thot_gui.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "libmsg.h"
#include "message.h"
#include "storage.h"

typedef struct _UnePresent
  {
     PtrPSchema          ChPtrSchP;	/* pointeur sur le schema */
     int                 PNbUtil;	/* nombre d'utilisations de ce schema */
     Name                 PrsNomSchema;	/* nom du schema */
  }
UnePresent;

#include "app.h"
#include "appdialogue.h"

/* variables importees */
#define EXPORT extern
#include "edit.var"
#include "appdialogue.var"

extern PtrEventsSet  SchemasEvents;
/* variables locales */
/* table des schemas de presentation charges */
static UnePresent   TabSchPres[MAX_PSCHEMAS];

#include "config_f.h"
#include "memory_f.h"
#include "readstr_f.h"
#include "readprs_f.h"
#include "schemas_f.h"

/* ---------------------------------------------------------------------- */
/* |    InitSchAppli(pSchema)                                            | */
/* |            Initialises the application pointer in the StructSchema    | */
/* |            if there is an application for this schema. If not,     | */
/* |            the pointer is set to NULL.                             | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                InitSchAppli (PtrSSchema pSchema)
#else  /* __STDC__ */
void                InitSchAppli (pSchema)
PtrSSchema        pSchema;
#endif /* __STDC__ */
{
   char          schemaName[MAX_NAME_LENGTH];
   PtrEventsSet  schemaActions;

   strcpy (schemaName, pSchema->SsName);
   pSchema->SsActionList = NULL;
   if (pSchema->SsName[0] != '\0')
     {
	/* Do the specific initialisations and update StructSchema. */
	schemaActions = SchemasEvents;
	while (schemaActions != NULL && strcmp (schemaActions->EvSName, schemaName) != 0)
	   schemaActions = schemaActions->EvSNext;
	if (schemaActions != NULL)
	   /* It contains the complete set of event/action for an application. */
	   pSchema->SsActionList = schemaActions;
     }
}


/* ---------------------------------------------------------------------- */
/* |    InitNatures     initialise la table des schemas de presentation | */
/* |                    charges.                                        | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                InitNatures ()

#else  /* __STDC__ */
void                InitNatures ()
#endif				/* __STDC__ */
{
   int                 i;

   for (i = 0; i < MAX_PSCHEMAS; i++)
     {
	TabSchPres[i].ChPtrSchP = NULL;
	TabSchPres[i].PNbUtil = 0;
	TabSchPres[i].PrsNomSchema[0] = '\0';
     }
}


/* ---------------------------------------------------------------------- */
/* |    LdSchPres       charge le schema de presentation de nom fname   | */
/* |                    pour le schema de structure pointe' par SS et   | */
/* |                    retourne un pointeur sur le schema charge' ou   | */
/* |                    NULL si echec.                                  | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
PtrPSchema          LdSchPres (Name fname, PtrSSchema SS)

#else  /* __STDC__ */
PtrPSchema          LdSchPres (fname, SS)
Name                 fname;
PtrSSchema        SS;

#endif /* __STDC__ */

{
   int                 i;
   PtrPSchema          pSchP;
   boolean             found;

   pSchP = NULL;
   /* cherche dans la table si le schema est deja charge */
   i = 0;
   found = FALSE;
   do
     {
	i++;
	if (TabSchPres[i - 1].ChPtrSchP != NULL)
	   /* compare les noms fname et PrsNomSchema */
	   if (strcmp (fname, TabSchPres[i - 1].PrsNomSchema) == 0)
	      found = TRUE;
     }
   while (!(found || i >= MAX_PSCHEMAS));
   if (found)
      /* ce schema est dans la table des schemas charges */
     {
	TabSchPres[i - 1].PNbUtil++;	/* une utilisation de plus */
	pSchP = TabSchPres[i - 1].ChPtrSchP;
     }
   else
      /* c'est un nouveau schema, il faut le charger */
     {
	pSchP = RdSchPres (fname, SS);
	if (pSchP != NULL)
	   /* met le nouveau schema dans la table des schemas charges */
	   /* cherche une entree libre dans la table */
	  {
	     i = 0;
	     do
		i++;
	     while (!(TabSchPres[i - 1].ChPtrSchP == NULL || i == MAX_PSCHEMAS));
	     if (TabSchPres[i - 1].ChPtrSchP == NULL)
		/* on a trouve une entree libre, on l'utilise */
	       {
		  TabSchPres[i - 1].PNbUtil = 1;
		  TabSchPres[i - 1].ChPtrSchP = pSchP;
		  strncpy (TabSchPres[i - 1].PrsNomSchema, fname, MAX_NAME_LENGTH);
	       }
	  }
     }
   if (pSchP != NULL)
      strncpy (SS->SsDefaultPSchema, fname, MAX_NAME_LENGTH);
   /* rend la valeur de retour */
   return pSchP;
}


/* ---------------------------------------------------------------------- */
/* |    LibListeRegles  libere la liste de regles de presentation dont  | */
/* |                    l'ancre est arp.                                | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         LibListeRegles (PtrPRule * arp)

#else  /* __STDC__ */
static void         LibListeRegles (arp)
PtrPRule       *arp;

#endif /* __STDC__ */

{
   PtrPRule        p, s;

   p = *arp;
   while (p != NULL)
     {
	s = p->PrNextPRule;
	FreeReglePres (p);
	p = s;
     }
   *arp = NULL;
}


/* ---------------------------------------------------------------------- */
/* |    SupprSchPrs     libere le schema de presentation pointe par     | */
/* |    pSch, ainsi que toutes les regles de presentation qu'il pointe. | */
/* |    pSS pointe le schema de structure auquel le schema de           | */
/* |    presentation a liberer est associe.                             | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                SupprSchPrs (PtrPSchema pSch, PtrSSchema pSS)

#else  /* __STDC__ */
void                SupprSchPrs (pSch, pSS)
PtrPSchema          pSch;
PtrSSchema        pSS;

#endif /* __STDC__ */

{
   int                 i, j;
   boolean             supp;
   UnePresent         *pUn1;
   AttributePres      *pRP1;

   supp = TRUE;
   /* parcourt la table des schemas de presentation pour trouver ce schema */
   i = 1;
   while (i < MAX_PSCHEMAS && TabSchPres[i - 1].ChPtrSchP != pSch)
      i++;
   pUn1 = &TabSchPres[i - 1];
   if (pUn1->ChPtrSchP == pSch)
      /* ce schema est dans la table */
     {
	pUn1->PNbUtil--;
	/* une utilisation de moins */
	if (pUn1->PNbUtil > 0)
	   /* il y a d'autres utilisations, on ne le supprime pas */
	   supp = FALSE;
	else
	   /* c'etait la derniere utilisation, on le supprime de la table */
	  {
	     pUn1->ChPtrSchP = NULL;
	     pUn1->PNbUtil = 0;
	     pUn1->PrsNomSchema[0] = '\0';
	  }
     }
   if (supp)
     {
	/* libere les regles de presentation par defaut */
	LibListeRegles (&pSch->PsFirstDefaultPRule);
	/* libere les regles de presentation des boites de presentation */
	for (i = 0; i < pSch->PsNPresentBoxes; i++)
	   LibListeRegles (&pSch->PsPresentBox[i].PbFirstPRule);
	/* libere les regles de presentation des attributs */
	for (i = 0; i < pSS->SsNAttributes; i++)
	  {
	     pRP1 = pSch->PsAttrPRule[i];
	     while (pRP1 != NULL)
	       {
		  switch (pSS->SsAttribute[i].AttrType)
			{
			   case AtNumAttr:
			      for (j = 0; j < pRP1->ApNCases; j++)
				 LibListeRegles (&pRP1->ApCase[j].CaFirstPRule);
			      break;
			   case AtTextAttr:
			      LibListeRegles (&pRP1->ApTextFirstPRule);
			      break;
			   case AtReferenceAttr:
			      LibListeRegles (&pRP1->ApRefFirstPRule);
			      break;
			   case AtEnumAttr:
			      for (j = 0; j <= pSS->SsAttribute[i].AttrNEnumValues; j++)
				 LibListeRegles (&pRP1->ApEnumFirstPRule[j]);
			      break;
			   default:
			      break;
			}
		  pRP1 = pRP1->ApNextAttrPres;
	       }
	  }

	/* libere les regles de presentation des types */
	for (i = 0; i < pSS->SsNRules; i++)
	   LibListeRegles (&pSch->PsElemPRule[i]);
	FreeSPres (pSch);
     }
}


/* ---------------------------------------------------------------------- */
/* | LoadNat    Charge la nature definie dans la regle r du schema de   | */
/* | structure pointe par PSchStr. Si le 1er octet de NomSchPrs est nul | */
/* | on propose a l'utilisateur le schema de presentation par defaut    | */
/* | defini dans le schema de structure, sinon on propose le schema de  | */
/* | presentation de nom NomSchPrs.                                     | */
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
	TtaDisplayMessage (INFO, TtaGetMessage(LIB, SCHEMA_NOT_FIND), fname);
	FreeSStruct (PtrStrNat);
	PSchStr->SsRule[r - 1].SrSSchemaNat = NULL;
     }
   else
      /* chargement du schema de structure reussi */
     {
	/* traduit le schema de structure dans la langue de l'utilisateur */
	ConfigTranslateSSchema (PtrStrNat);
	PSchStr->SsRule[r - 1].SrSSchemaNat = PtrStrNat;
	if (NomSchPrs[0] != '\0')
	   /* l'appelant indique un schema de presentation, on essaie de le charger */
	  {
	     strncpy (fname, NomSchPrs, MAX_NAME_LENGTH);
	     PtrStrNat->SsPSchema = LdSchPres (fname, PtrStrNat);
	  }
	if (NomSchPrs[0] == '\0' || PtrStrNat->SsPSchema == NULL)
	   /* pas de schema de presentation particulier demande' par l'appelant */
	   /* ou schema demande' inaccessible */
	  {
	     /* on consulte le fichier .conf */
	     if (!ConfigGetPSchemaNature (PSchStr, PSchStr->SsRule[r - 1].SrOrigNat,
					  fname))
		/* le fichier .conf ne donne pas de schema de presentation pour */
		/* cette nature, on le demande a l'utilisateur */
	       {
		  strncpy (fname, PtrStrNat->SsDefaultPSchema, MAX_NAME_LENGTH);
		  if (ThotLocalActions[T_presentchoice] != NULL)
		     (*ThotLocalActions[T_presentchoice]) (PtrStrNat, fname);
	       }
	     /* cree un nouveau schema de presentation et le charge depuis le fichier */
	     PtrStrNat->SsPSchema = LdSchPres (fname, PtrStrNat);
	  }
	if (PtrStrNat->SsPSchema == NULL)
	   /* echec chargement schema */
	  {
	     TtaDisplayMessage (INFO, TtaGetMessage(LIB, SCHEMA_NOT_FIND), fname);
	     FreeSStruct (PtrStrNat);
	     PSchStr->SsRule[r - 1].SrSSchemaNat = NULL;
	  }
	InitSchAppli (PtrStrNat);
     }
}


/* ---------------------------------------------------------------------- */
/* |    ajouteregle     ajoute une nouvelle regle a la fin de la table  | */
/* |                    des regles.                                     | */
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
	/* Table de regles saturee */
	TtaDisplaySimpleMessage (FATAL, LIB, LIB_RULES_TABLE_FULL);
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


/* ---------------------------------------------------------------------- */
/* |    CreeNature      retourne le numero de la regle definissant le   | */
/* |    type de nom NomSchStr dans le schema de structure pointe par    | */
/* |    pSS.                                                            | */
/* |    S'il n'existe pas de type de ce nom, ajoute une regle de type   | */
/* |    CsNatureSchema au schema de structure et charge le schema de         | */
/* |    structure de nom NomSchStr; retourne le numero de la regle      | */
/* |    cree, ou 0 si echec creation.                                   | */
/* |    En cas de chargement de schema, le nom de schema de             | */
/* |    presentation NomSchPrs est propose' a l'utilisateur plutot que  | */
/* |    le schema de presentation par defaut defini dans le schema de   | */
/* |    structure, sauf si le premier octet de NomSchPrs est nul.       | */
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

   /* cherche si le type existe deja dans le schema de structure */
   found = FALSE;
   ret = 0;
   do
     {
	pRule = &pSS->SsRule[ret++];
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
/* | le schema de structure de nom NomSchStr ainsi que son schema de    | */
/* | presentation. Si NomSchPrs commence par un octet nul, on charge    | */
/* | le schema de presentation par defaut defini dans le schema de      | */
/* | structure, sinon on charge le schema de presentation de nom        | */
/* | NomSchPrs.                                                         | */
/* | Si pSCharge n'est pas NULL, on ne charge que le schema de          | */
/* | presentation pour le schema de structure pointe' par pSCharge.     | */
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
	     TtaDisplayMessage (INFO, TtaGetMessage(LIB, SCHEMA_NOT_FIND), fname);
	     FreeSStruct (*pSS);
	     *pSS = NULL;
	  }
	else if ((*pSS)->SsExtension != Extension)
	   /* on voulait un schema d'extension et ce n'en est pas un, ou l'inverse */
	  {
	     FreeSStruct (*pSS);
	     *pSS = NULL;
	  }
	else
	  {
	     /* traduit les noms du schema dans la langue de l'utilisateur */
	     ConfigTranslateSSchema (*pSS);
	     InitSchAppli (*pSS);
	  }
     }
   else
      *pSS = pSCharge;
   if (*pSS)
     {
	if ((*pSS)->SsExtension)
	   /* pour eviter que RdSchPres recharge le schema de structure */
	   (*pSS)->SsRootElem = 1;
	if (NomSchPrs[0] != '\0')
	   /* l'appelant specifie le schema de presentation a prendre, on essaie
	      de le charger */
	  {
	     strncpy (fname, NomSchPrs, MAX_NAME_LENGTH);
	     (*pSS)->SsPSchema = LdSchPres (fname, *pSS);
	  }
	if (NomSchPrs[0] == '\0' || (*pSS)->SsPSchema == NULL)
	   /* pas de presentation specifiee par l'appelant, ou schema specifie'
	      inaccessible */
	  {
	     /* on consulte le fichier de configuration */
	     if (!ConfigDefaultPSchema ((*pSS)->SsName, fname))
		/* le fichier de configuration ne dit rien, on demande a l'utilisateur */
	       {
		  /* propose la presentation par defaut definie dans le schema de */
		  /* structure */
		  strncpy (fname, (*pSS)->SsDefaultPSchema, MAX_NAME_LENGTH);
		  if (ThotLocalActions[T_presentchoice] != NULL)
		     (*ThotLocalActions[T_presentchoice]) (*pSS, fname);
	       }
	     /* charge le schema de presentation depuis le fichier */
	     (*pSS)->SsPSchema = LdSchPres (fname, *pSS);
	  }
	if ((*pSS)->SsPSchema == NULL)
	   /* echec chargement schema */
	  {
	     TtaDisplayMessage (INFO, TtaGetMessage(LIB, SCHEMA_NOT_FIND), fname);
	     FreeSStruct (*pSS);
	     *pSS = NULL;
	  }
     }
}


/* ---------------------------------------------------------------------- */
/* | LoadExtension charge en memoire, pour le document pDoc, le schema  | */
/* |    d'extension de nom NomSchStr et son schema de presentation de   | */
/* |    nom NomSchPrs.                                                  | */
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
   boolean             extensionExiste;

   pExtens = NULL;
   if (pDoc->DocSSchema != NULL)
     {
	/* cherche si ce schema d'extension existe deja pour le document */
	pExtens = pDoc->DocSSchema->SsNextExtens;
	extensionExiste = FALSE;
	while (pExtens != NULL && !extensionExiste)
	   if (strcmp (pExtens->SsName, NomSchStr) == 0)
	      /* le schema d'extension existe deja */
	      extensionExiste = TRUE;
	   else
	      pExtens = pExtens->SsNextExtens;
	if (!extensionExiste)
	   /* le schema d'extension n'existe pas, on le charge */
	  {
	     if (NomSchPrs[0] == '\0')
		/* pas de schema de presentation precise' */
		/* cherche le schema de presentation de l'extension prevu dans */
		/* le fichier .conf pour ce type de document */
		ConfigGetPSchemaNature (pDoc->DocSSchema, NomSchStr, NomSchPrs);
	     /* charge le schema d'extension demande' */
	     LoadSchemas (NomSchStr, NomSchPrs, &pExtens, NULL, TRUE);
	     if (pExtens != NULL)
	       {
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
     }
   return pExtens;
}

/* ---------------------------------------------------------------------- */
/* |    LibRegleNat     cherche dans le schema de structure pointe' par | */
/* |    pSchStr les regles de nature qui font reference au schema       | */
/* |    pointe par pSS. S'il y en a, retourne Vrai, annule ces regles   | */
/* |    et traite de meme les autres natures. S'il n'y en a pas,        | */
/* |    retourne Faux.                                                  | */
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
		 /* supprime la reference */
		{
		   ret = TRUE;
		   pRule->SrSSchemaNat = NULL;
		}
	      else
		 /* elle fait reference a une autre nature, on cherche */
		 /* dans cette nature les regles qui font reference a la */
		 /* nature supprimee. */
		 LibRegleNat (pRule->SrSSchemaNat, pSS);
	}
   return ret;
}


/* ---------------------------------------------------------------------- */
/* |    LibNature    Si le schema de structure pointe' par pSPere       | */
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
	if (pSS->SsPSchema != NULL)
	   /* libere le schema de presentation associe' */
	   SupprSchPrs (pSS->SsPSchema, pSS);
	/* rend la memoire */
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
   int                 i;
   PtrSSchema        pSc1, pSSuiv;
   SRule              *pRule;

   pSc1 = pDoc->DocSSchema;
   /* libere le schema de structure du document et ses extensions */
   while (pSc1 != NULL)
     {
	pSSuiv = pSc1->SsNextExtens;
	/* libere les schemas de nature pointes par les regles de structure */
	for (i = 0; i < pSc1->SsNRules; i++)
	  {
	     pRule = &pSc1->SsRule[i];
	     if (pRule->SrConstruct == CsNatureSchema)
		if (pRule->SrSSchemaNat != NULL)
		   LibNature (pSc1, pRule->SrSSchemaNat);
	  }
	/* libere le schema de presentation et de structure */
	if (pSc1->SsPSchema != NULL)
	   SupprSchPrs (pSc1->SsPSchema, pSc1);
	FreeSStruct (pSc1);
	pSc1 = pSSuiv;
     }
   pDoc->DocSSchema = NULL;
}
