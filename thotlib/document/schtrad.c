
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

/*
   schtrad.c :
   Chargement et Liberation
   des schemas de structure et de traduction
   V. Quint     Janvier 1988
   IV : Mai 92   adaptation Tool Kit
 */

#include "thot_sys.h"
#include "constmedia.h"
#include "consttra.h"
#include "message.h"
#include "libmsg.h"
#include "typemedia.h"
#include "typetra.h"

#define EXPORT extern
#include "edit.var"

#include "cdialog.f"
#include "rdschstr.f"
#include "rdschtra.f"
#include "schemas.f"
#include "schtrad.f"

typedef struct _UneTraduc
  {
     PtrSSchema        TradPtrSchS;	/* pointeur sur le schema de struct. */
     PtrTSchema        TradPtrSchT;	/* pointeur sur le schema de trad. */
     Name                 TradNomSchT;	/* nom du schema de traduction */
  }
UneTraduc;

#define NbMaxSchT 16		/* nombre max de schemas de traduction charges
				   en meme temps */

/* table des schemas de traduction charges */
static UneTraduc    TabSchTrad[NbMaxSchT];

/* ---------------------------------------------------------------------- */
/* |    InitTableSchTrad initialise la table des schemas de traduction  | */
/* |            charges.                                                | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                InitTableSchTrad ()

#else  /* __STDC__ */
void                InitTableSchTrad ()
#endif				/* __STDC__ */

{
   int                 i;

   for (i = 0; i < NbMaxSchT; i++)
     {
	TabSchTrad[i].TradPtrSchS = NULL;
	TabSchTrad[i].TradPtrSchT = NULL;
	TabSchTrad[i].TradNomSchT[0] = '\0';
     }
}


/* ---------------------------------------------------------------------- */
/* |    ClearTableSchTrad libere tous les schemas de traduction         | */
/* |            pointe's par la table des schemas de traduction         | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                ClearTableSchTrad ()

#else  /* __STDC__ */
void                ClearTableSchTrad ()
#endif				/* __STDC__ */

{
   int                 i;

   for (i = 0; i < NbMaxSchT; i++)
     {
	if (TabSchTrad[i].TradPtrSchT != NULL)
	  {
	     LibereSchTrad (TabSchTrad[i].TradPtrSchT, TabSchTrad[i].TradPtrSchS);
	     TabSchTrad[i].TradPtrSchT = NULL;
	  }
	TabSchTrad[i].TradPtrSchS = NULL;
	TabSchTrad[i].TradNomSchT[0] = '\0';
     }
}


/* ---------------------------------------------------------------------- */
/* |    LdSchTrad charge un schema de traduction.                       | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
PtrTSchema        LdSchTrad (Name fname, PtrSSchema SS)

#else  /* __STDC__ */
PtrTSchema        LdSchTrad (fname, SS)
Name                 fname;
PtrSSchema        SS;

#endif /* __STDC__ */

{
   int                 i;
   PtrTSchema        pSchT;
   boolean             found;

   pSchT = NULL;
   /* cherche dans la table si le schema est deja charge */
   i = 0;
   found = False;
   do
     {
	if (TabSchTrad[i].TradPtrSchT != NULL)
	   found = (strcmp (fname, TabSchTrad[i].TradNomSchT) == 0);
	if (!found)
	   i++;
     }
   while (!found && i < NbMaxSchT);
   if (found)
      /* ce schema est dans la table des schemas charges */
      pSchT = TabSchTrad[i].TradPtrSchT;
   else
      /* c'est un nouveau schema, il faut le charger */
     {
	/* cherche d'abord une entree libre dans la table */
	i = 0;
	while (TabSchTrad[i].TradPtrSchS != NULL && i < NbMaxSchT)
	   i++;
	if (TabSchTrad[i].TradPtrSchS == NULL && i < NbMaxSchT)
	   /* on a trouve une entree libre */
	  {
	     /* on charge le schema de traduction */
	     pSchT = RdSchTrad (fname, SS);
	     if (pSchT != NULL)
		/* met le nouveau schema dans la table des schemas charges */
	       {
		  TabSchTrad[i].TradPtrSchS = SS;
		  TabSchTrad[i].TradPtrSchT = pSchT;
		  strcpy (TabSchTrad[i].TradNomSchT, fname);
	       }
	  }
     }
   return (pSchT);
}


/* ---------------------------------------------------------------------- */
/* |    ChercheUse cherche une regle USE dans la suite de blocs de      | */
/* |            regles pBloc. Si une regle USE pour la nature de nom    | */
/* |            fname est trouvee dans ce bloc, on met dans fname le    | */
/* |            nom du schema de traduction a` utiliser.                | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static boolean      ChercheUse (PtrTRuleBlock pBloc, Name fname)

#else  /* __STDC__ */
static boolean      ChercheUse (pBloc, fname)
PtrTRuleBlock       pBloc;
Name                 fname;

#endif /* __STDC__ */

{
   PtrTRule        pRegle;
   boolean             UseTrouve;
   PtrTRule        pRe1;

   UseTrouve = False;
   if (pBloc != NULL)
      do
	{
	   pRegle = pBloc->TbFirstTRule;
	   if (pRegle != NULL)
	      do
		{
		   pRe1 = pRegle;
		   if (pRe1->TrType == TUse)
		      /* c'est une regle USE */
		      if (strcmp (fname, pRe1->TrNature) == 0)
			{
			   UseTrouve = True;
			   strncpy (fname, pRe1->TrTranslSchemaName, MAX_NAME_LENGTH);
			}
		   pRegle = pRe1->TrNextTRule;
		}
	      while (!(pRegle == NULL || UseTrouve));
	   pBloc = pBloc->TbNextBlock;
	}
      while (!(pBloc == NULL || UseTrouve));

   return UseTrouve;
}


/* ---------------------------------------------------------------------- */
/* |    rdNomTrad       trouve le nom du schema de traduction a`        | */
/* |            utiliser pour le schema de structure de nom fname.      | */
/* |            Au retour, rend dans fname le nom du schema de          | */
/* |            traduction.                                             | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void         rdNomTrad (Name fname)

#else  /* __STDC__ */
static void         rdNomTrad (fname)
Name                 fname;

#endif /* __STDC__ */

{
   int                 i;
   boolean             trouve, natureok;
   PtrSSchema        pSc1;
   SRule              *pRe1;

   trouve = False;
   /* cherche d'abord si le schema de traduction du document contient */
   /* une regle USE pour cette nature */
   /* le schema de traduction du document est le premier de la table */
   if (TabSchTrad[0].TradPtrSchT != NULL)
      /* cherche la nature dans le schema de structure du document */
     {
	i = 0;
	natureok = False;
	pSc1 = TabSchTrad[0].TradPtrSchS;
	do
	  {
	     i++;
	     pRe1 = &pSc1->SsRule[i - 1];
	     if (pRe1->SrConstruct == CsNatureSchema)
		natureok = strcmp (fname, pRe1->SrName) == 0;
	  }
	while (!(i >= pSc1->SsNRules || natureok));
	if (natureok)
	   /* on a trouve la nature, on cherche une regle USE parmi les */
	   /* regles de traduction de l'element nature */
	   trouve = ChercheUse (TabSchTrad[0].TradPtrSchT->TsElemTRule[i - 1],
				fname);
	if (!trouve)
	   /* on cherche une regle USE parmi les regles de traduction de */
	   /* l'element racine du document */
	   trouve = ChercheUse (TabSchTrad[0].TradPtrSchT->
	     TsElemTRule[TabSchTrad[0].TradPtrSchS->SsRootElem - 1], fname);
     }
   if (!trouve)
     {
	/* on n'a pas trouve' de regle USE pour cette nature */
	TtaDisplaySimpleMessageString (LIB, INFO, LIB_ELEMENTS_NOT_TRANSLATED, fname);
	fname[0] = '\0';
     }
}


/* ---------------------------------------------------------------------- */
/* |    SchemaTrad retourne le schema de traduction a` appliquer aux    | */
/* |            elements appartenant au schema de structure pSS.        | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
PtrTSchema        SchemaTrad (PtrSSchema pSS)

#else  /* __STDC__ */
PtrTSchema        SchemaTrad (pSS)
PtrSSchema        pSS;

#endif /* __STDC__ */

{
   int                 i;
   PtrTSchema        schema;
   Name                 nomSch;
   boolean             found;

   schema = NULL;
   found = False;
   i = 0;
   do
     {
	if (TabSchTrad[i].TradPtrSchS != NULL)
	   found = (TabSchTrad[i].TradPtrSchS->SsCode == pSS->SsCode);
	if (!found)
	   i++;
     }
   while (!found && i < NbMaxSchT);
   if (found)
      schema = TabSchTrad[i].TradPtrSchT;
   else
     {
	strcpy (nomSch, pSS->SsName);
	rdNomTrad (nomSch);
	if (nomSch[0] != '\0')
	   /* cree un nouveau schema de traduction et le */
	   /* charge depuis le fichier */
	   schema = LdSchTrad (nomSch, pSS);
	else
	   /* indique dans la table qu'il n'y a pas de schema de traduction pour */
	   /* ce schema de structure */
	  {
	     /* cherche d'abord une entree libre dans la table */
	     i = 0;
	     while (TabSchTrad[i].TradPtrSchS != NULL && i < NbMaxSchT)
		i++;
	     if (TabSchTrad[i].TradPtrSchS == NULL && i < NbMaxSchT)
		/* on a trouve une entree libre */
	       {
		  TabSchTrad[i].TradPtrSchS = pSS;
		  TabSchTrad[i].TradPtrSchT = NULL;
		  TabSchTrad[i].TradNomSchT[0] = '\0';
	       }
	  }
     }
   return schema;
}

/* End Of Module schtrad */
