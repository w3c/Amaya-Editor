
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

/* ======================================================================= */
/* |                                                                    | */
/* |                           Projet THOT                              | */
/* |                                                                    | */
/* |    Le programme TYP compile un schema de typographie,              | */
/* |    contenu dans un fichier de type .SCH.                           | */
/* |    TYP est dirige' par la grammaire du langage de typographie      | */
/* |    contenue, sous forme codee, dans le fichier TYP.GRM.            | */
/* |    Ce fichier .GRM est produit par le programme GRM.               | */
/* |    TYP produit un fichier de type .TYP qui sera ensuite utilise    | */
/* |    par l'editeur, pour guider la correction typographique.         | */
/* |    Ce programme utilise le module ANALSYNT pour l'analyse          | */
/* |    syntaxique du schema a compiler.                                | */
/* |                                                                    | */
/* ======================================================================= */


#include "thot_sys.h"
#include "TYP.h"
#include "constgrm.h"
#include "constmedia.h"
#include "typemedia.h"
#include "typegrm.h"
#include "typetyp.h"
#include "message.h"
#include "storage.h"
#include "compilmsg.h"
#include "libmsg.h"
#include "typmsg.h"
#include "thotfile.h"
#include "thotdir.h"

/* Variables  pour l'analyseur syntaxique */
#define EXPORT
#include "compil_tv.h"
#include "platform_tv.h"

/* Variables exportees pour l'analyseur syntaxique */
#undef EXPORT
#define EXPORT extern
#include "analsynt_tv.h"


/* Variables locales de TYP */
static Name          pfilename;	/* pointeur sur le nom du fichier a compiler */
int                 LineNum;	/* compteur de lignes */
static indLine        i;		/* position courante dans la ligne en cours */
static indLine        wi;		/* position du debut du mot courant dans la

				 * ligne en cours */
static indLine        wl;		/* longueur du mot courant */
static SyntacticType       wn;		/* SyntacticType du mot courant */
static SyntRuleNum          r;		/* numero de regle */
static SyntRuleNum          pr;		/* numero de la regle precedente */
static SyntacticCode      c;		/* code grammatical du mot trouve */
static int          nb;		/* indice dans Identifier du mot trouve, si identific */
static PtrSSchema pSchStr;	/* pointeur sur le schema de structure */
static PtrSchTypo   pSchTypo;	/* pointeur sur le schema de typographie */
static PtrTypoFunction PremFonction;	/* pointeur sur la premiere fonction de la

					   chaine des fonctions definie dans la section
					   FUNCTION du schema de typographie */
static PtrTypoFunction DerFonction;	/* pointeur sur la derniere fonction de la

					   chaine */
static PtrModeleCompo PremModele;	/* pointeur sur le premier modele de

					   composition du schema de typographie */
static PtrModeleCompo DerModele;	/* pointeur sur le dernier modele de

					   composition du schema de typographie */

static boolean      ChangeBloc;	/* il faut ouvrir un nouveau bloc de regles */

static boolean      DansBloc;	/* on est dans un bloc de regles */
static boolean      DansParam;	/* on est dans un bloc de param */
static boolean      DansCondition;	/* on analyse une condition 'If...TRuleBlock' */

static boolean      ReglesTypes;	/* on est dans les regles des types */
static boolean      ReglesAttr;	/* on est dans les regles des attributs */
static boolean      ReglesMod;	/* on est dans les regles des modeles */

static int /* 0 .. MAX_RULES_SSCHEMA */ CourType;	/* numero du type d'element courant */
static PtrRegleTypo CourBloc;	/* premiere regle du bloc de regles courant */
static PtrRegleTypo CourRegle;	/* regle courante dans le bloc courant */
static TypeFunct    CourFunct;	/* type de la fonction courante */

static TypoCondition TabCondition[MaxTyCond];	/* table des conditions */
static int /* 0 .. MaxTyCond */ NbCourCond;	/* rang de la condition courante */

static int          SigneAttrVal;	/* Signe d'une valeur d'attribut numerique */
static int /* 0 .. MAX_ATTR_SSCHEMA */ CourAttr;	/* numero de l'attribut courant */
static int /* 0 .. MAX_ATTR_VAL */ CourValAttr;	/* numero de la valeur d'attribut */

#include "platform_f.h"
#include "parser_f.h"
#include "readstr_f.h"
#include "compilmsg_f.h"
#include "message_f.h"
#include "writetyp_f.h"
#include "fileaccess_f.h"
int                 TYP;

#ifdef __STDC__
extern void         TtaInitializeAppRegistry (char *);
extern void         TtaSaveAppRegistry (void);

#else
extern void         TtaInitializeAppRegistry ();
extern void         TtaSaveAppRegistry ();

#endif /* __STDC__ */

/* ---------------------------------------------------------------------- */
/* |    prepare la generation : initialise le schema de typographie     | */
/* |    en memoire                                                      | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                initgener ()
#else  /* __STDC__ */

void                initgener ()
#endif				/* __STDC__ */
{
   int                 i;

   /* acquiert la memoire pour un schema de structure */
   if ((pSchStr = (PtrSSchema) malloc (sizeof (StructSchema))) == NULL)
      TtaDisplaySimpleMessage (FATAL, TYP, TYP_NOT_ENOUGH_MEM);
   else
      /* acquiert la memoire pour un schema de typographie */
   if ((pSchTypo = (PtrSchTypo) malloc (sizeof (SchTypo))) == NULL)
      TtaDisplaySimpleMessage (FATAL, TYP, TYP_NOT_ENOUGH_MEM);
   else
     {
	pSchTypo->STySuivant = NULL;
	pSchTypo->STyNomStruct[0] = '\0';
	pSchTypo->STyStructCode = 0;
	for (i = 1; i <= MAX_RULES_SSCHEMA; i++)
	  {
	     pSchTypo->STyRegleElem[i - 1] = NULL;
	     /* pointeurs sur le debut de la chaine de */
	     /* regles de typographie specifiques a chaque type d'element */
	     pSchTypo->STyElemHeritAttr[i - 1] = 0;
	     /* liste des elements de type alinea */
	     pSchTypo->STyElemAlinea[i - 1] = 0;
	  }
	for (i = 1; i <= MAX_ATTR_SSCHEMA; i++)
	   pSchTypo->STyAttribSem[i - 1] = NULL;

	PremFonction = NULL;
	DerFonction = NULL;
	PremModele = NULL;
	DerModele = NULL;
	ReglesTypes = False;
	ReglesAttr = False;
	ReglesMod = False;
	ChangeBloc = False;
	DansBloc = False;
	DansParam = False;
	DansCondition = False;
	CourFunct = (TypeFunct) 0;	/* ??? */
	CourBloc = NULL;
	CourRegle = NULL;
	NbCourCond = 0;
	SigneAttrVal = 1;

	for (i = 1; i <= MaxTyCond; i++)
	   TabCondition[i - 1].CondTypeElem = 0;
     }
}

/* ---------------------------------------------------------------------- */
/* |    NewFunction     ajoute une nouvelle fonction dans la chaine des | */
/* |                    fonctions typographiques.                       | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                NewFunction (TypeFunct TypeF)

#else  /* __STDC__ */
void                NewFunction (TypeF)
TypeFunct           TypeF;

#endif /* __STDC__ */

{
   PtrTypoFunction     Funct;

   /* acquiert la memoire pour une nouvelle fonction typographique */
   if ((Funct = (PtrTypoFunction) malloc (sizeof (TypoFunction))) == NULL)
      /* memoire insuffisante */
      CompilerError (0, TYP, FATAL, TYP_NOT_ENOUGH_MEM, inputLine, LineNum);
   else
     {
	if (PremFonction == NULL)
	  {
	     /* premiere fonction typographique */
	     PremFonction = Funct;
	     pSchTypo->STyFunction = PremFonction;
	  }
	else
	   /* chaine le nouvelle fonction en fin de chaine */
	   DerFonction->TFSuiv = Funct;
	DerFonction = Funct;
	Funct->TFSuiv = NULL;
	Funct->TFIdent[0] = '\0';
	Funct->TFTypeFunct = TypeF;
     }
}

/* ---------------------------------------------------------------------- */
/* |    ChercheFonction cherche la fonction de nom NomFonct dans la     | */
/* |    chaine des fonctions et retourne un pointeur sur le descripteur | */
/* |    de cette fonction typographique, ou NULL si pas trouve'         | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
PtrTypoFunction     ChercheFonction (Name NomFonct)

#else  /* __STDC__ */
PtrTypoFunction     ChercheFonction (NomFonct)
Name                 NomFonct;

#endif /* __STDC__ */

{
   PtrTypoFunction     pFunct, Retour;

   Retour = NULL;
   pFunct = PremFonction;
   while (Retour == NULL && pFunct != NULL)
      if (strncmp (pFunct->TFIdent, NomFonct, MAX_NAME_LENGTH) == 0)
	 Retour = pFunct;
      else
	 pFunct = pFunct->TFSuiv;
   return Retour;
}

/* ---------------------------------------------------------------------- */
/* |    NewModele       ajoute un nouveau modele dans la chaine des     | */
/* |                    modeles de composition.                         | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                NewModele (Name NomModele)

#else  /* __STDC__ */
void                NewModele (NomModele)
Name                 NomModele;

#endif /* __STDC__ */

{
   PtrModeleCompo      pMod;

   /* acquiert la memoire pour un nouveau modele */
   if ((pMod = (PtrModeleCompo) malloc (sizeof (ModeleCompo))) == NULL)
      /* memoire insuffisante */
      CompilerError (0, TYP, FATAL, TYP_NOT_ENOUGH_MEM, inputLine, LineNum);
   else
     {
	if (PremModele == NULL)
	   /* premier modele de composition */
	   PremModele = pMod;
	else
	   /* chaine le nouveau modele en fin de chaine */
	   DerModele->MCSuiv = pMod;
	DerModele = pMod;
	pMod->MCSuiv = NULL;
	pMod->MCBlocRegles = NULL;
	strncpy (pMod->MCIdent, NomModele, MAX_NAME_LENGTH);
     }
}

/* ---------------------------------------------------------------------- */
/* |    ChercheModele   cherche le modele de composition de nom         | */
/* |    NomModele dans la chaine des modeles et retourne un pointeur    | */
/* |    sur le descripteur de ce modele, ou NULL si pas trouve'         | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
PtrModeleCompo      ChercheModele (Name NomModele)

#else  /* __STDC__ */
PtrModeleCompo      ChercheModele (NomModele)
Name                 NomModele;

#endif /* __STDC__ */

{
   PtrModeleCompo      pMod, Retour;

   Retour = NULL;
   pMod = PremModele;
   while (Retour == NULL && pMod != NULL)
      if (strncmp (pMod->MCIdent, NomModele, MAX_NAME_LENGTH) == 0)
	 Retour = pMod;
      else
	 pMod = pMod->MCSuiv;
   return Retour;
}

/* ---------------------------------------------------------------------- */
/* |    cree une nouvelle regle , la chaine et l'initialise             | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                nouvregle ()

#else  /* __STDC__ */
void                nouvregle ()
#endif				/* __STDC__ */

{
   PtrRegleTypo        r;
   PtrRTypoAttribut    pRTy1;

   if ((r = (PtrRegleTypo) malloc (sizeof (RegleTypo))) == NULL)
      TtaDisplaySimpleMessage (FATAL, TYP, TYP_NOT_ENOUGH_MEM);
   /* chaine la nouvelle regle a la regle courante s'il y en a une */
   if (CourRegle == NULL || ChangeBloc)
     {
	/* premiere regle d'un nouveau bloc de regle */
	/* chainer ce bloc */
	if (ReglesTypes)
	   pSchTypo->STyRegleElem[CourType - 1] = r;
	else if (ReglesMod)
	   /* bloc de regles associe' a un modele de composition */
	   DerModele->MCBlocRegles = r;
	else if (ReglesAttr)
	   /* bloc de regles associe' a un attribut */
	  {
	     pRTy1 = pSchTypo->STyAttribSem[CourAttr - 1];
	     switch (pSchStr->SsAttribute[CourAttr - 1].AttrType)
		   {
		      case AtNumAttr:
			 pRTy1->RTyACas[pRTy1->RTyANbCas - 1].TyANBlocRegles = r;
			 break;
		      case AtTextAttr:
			 pRTy1->RTyATxt = r;
			 break;
		      case AtReferenceAttr:
			 pRTy1->RTyARefPremRegle = r;
			 break;
		      case AtEnumAttr:
			 pRTy1->RTyAValEnum[CourValAttr] = r;
			 break;
		      default:
			 break;
		   }		/* end of switch */
	  }			/* end of if (ReglesAttr) */
	ChangeBloc = False;
	CourBloc = r;
     }
   else
      /* chaine la nouvelle regle a la regle courante */
      CourRegle->RTyRegleSuiv = r;
   /* la nouvelle regle devient la regle courante */
   CourRegle = r;
   /* initialise la regle */
   CourRegle->RTyRegleSuiv = NULL;
}

/* ---------------------------------------------------------------------- */
/* |    initialise la description d'un attribut                         | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                initAttr (PtrRTypoAttribut pRAtt, int att)

#else  /* __STDC__ */
void                initAttr (pRAtt, att)
PtrRTypoAttribut    pRAtt;
int                 att;

#endif /* __STDC__ */

{
   int                 j;
   RTypoAttribut      *pRT1;
   TyCasAttrNum       *pTC1;

   pRT1 = pRAtt;
   pRT1->RTyATypeElem = 0;
   /* selon le type de l'attribut */
   switch (pSchStr->SsAttribute[att - 1].AttrType)
	 {
	    case AtNumAttr:
	       pRT1->RTyANbCas = 0;
	       for (j = 1; j <= MaxCasTyAttrNum; j++)
		 {
		    pTC1 = &pRT1->RTyACas[j - 1];
		    pTC1->TyANBorneInf = -MAX_INT_ATTR_VAL - 1;	/* - infini */
		    pTC1->TyANBorneSup = MAX_INT_ATTR_VAL + 1;	/* + infini */
		    pTC1->TyANBlocRegles = NULL;
		 }
	       break;
	    case AtTextAttr:
	       pRT1->RTyATxtVal[0] = '\0';
	       pRT1->RTyATxt = NULL;
	       break;
	    case AtReferenceAttr:
	       pRT1->RTyARefPremRegle = NULL;
	       break;
	    case AtEnumAttr:
	       for (j = 0; j <= MAX_ATTR_VAL; j++)
		  pRT1->RTyAValEnum[j] = NULL;
	       break;
	 }
}


/* ---------------------------------------------------------------------- */
/* |    NewAttr         ajoute un nouvel attribut                       | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                NewAttr (int att)

#else  /* __STDC__ */
void                NewAttr (att)
int                 att;

#endif /* __STDC__ */

{
   PtrRTypoAttribut    TAttr;

   /* acquiert la memoire pour une nouvelle fonction typographique */
   if ((TAttr = (PtrRTypoAttribut) malloc (sizeof (RTypoAttribut))) == NULL)
      /* memoire insuffisante */
      CompilerError (0, TYP, FATAL, TYP_NOT_ENOUGH_MEM, inputLine, LineNum);
   else
     {
	pSchTypo->STyAttribSem[att - 1] = TAttr;
	initAttr (TAttr, att);
     }
}


/* ---------------------------------------------------------------------- */
/* |    rempli la regle courante                                        | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                copierregle (PtrRegleTypo pRegle)

#else  /* __STDC__ */
void                copierregle (pRegle)
PtrRegleTypo        pRegle;

#endif /* __STDC__ */

{
   int                 i, j;

   CourRegle->RTyTypeFunct = pRegle->RTyTypeFunct;
   strcpy (CourRegle->RTyIdentFunct, pRegle->RTyIdentFunct);
   CourRegle->RTyNbCond = pRegle->RTyNbCond;
   j = CourRegle->RTyNbCond;
   for (i = 1; i <= j; i++)
     {
	CourRegle->RTyCondition[i - 1].CondTypeCond =
	   pRegle->RTyCondition[i - 1].CondTypeCond;
	CourRegle->RTyCondition[i - 1].CondNegative =
	   pRegle->RTyCondition[i - 1].CondNegative;
	CourRegle->RTyCondition[i - 1].CondTypeElem =
	   pRegle->RTyCondition[i - 1].CondTypeElem;
	switch (pRegle->RTyCondition[i - 1].CondTypeCond)
	      {
		 case TyLangue:
		 case TyFonction:
		    strcpy (CourRegle->RTyCondition[i - 1].CondNom, pRegle->RTyCondition[i - 1].CondNom);
		    break;
		 case TyAvantType:
		 case TyApresType:
		    CourRegle->RTyCondition[i - 1].CondOpTypeIn = pRegle->RTyCondition[i - 1].CondOpTypeIn;
		    break;
		 default:
		    break;
	      }
     }
}


/* ---------------------------------------------------------------------- */
/* |    ChercheRegle cherche la premiere fonction de type TypeF         | */
/* |    dans le bloc de regle courant a partir de la regle PremRegle    | */
/* |    et retourne un pointeur sur le descripteur de cette             | */
/* |    regle typographique,      ou NULL si pas trouve'                | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
PtrRegleTypo        ChercheRegle (TypeFunct TypeF, PtrRegleTypo PremRegle)

#else  /* __STDC__ */
PtrRegleTypo        ChercheRegle (TypeF, PremRegle)
TypeFunct           TypeF;
PtrRegleTypo        PremRegle;

#endif /* __STDC__ */

{
   PtrRegleTypo        pRe1, Retour;

   Retour = NULL;
   pRe1 = PremRegle;
   while (Retour == NULL && pRe1 != NULL)
      if (pRe1->RTyTypeFunct == TypeF)
	 Retour = pRe1;
      else
	 pRe1 = pRe1->RTyRegleSuiv;
   return Retour;
}

/* ---------------------------------------------------------------------- */
/* |    add_condition   ajoute dans la regle pRegle                     | */
/* |                    la table des conditions courante                | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                add_condition (PtrRegleTypo pRegle)

#else  /* __STDC__ */
void                add_condition (pRegle)
PtrRegleTypo        pRegle;

#endif /* __STDC__ */

{
   int                 i, j, k;

   j = pRegle->RTyNbCond;
   k = 0;
   if (NbCourCond + j >= MaxTyCond)
      /* trop de conditions */
      CompilerError (wi, TYP, FATAL, TYP_TOO_MANY_CONDITIONS,
		     inputLine, LineNum);
   else
      for (i = j; (i < NbCourCond + j); i++)
	{
	   pRegle->RTyCondition[i].CondTypeCond =
	      TabCondition[k].CondTypeCond;
	   pRegle->RTyCondition[i].CondNegative =
	      TabCondition[k].CondNegative;
	   pRegle->RTyCondition[i].CondTypeElem =
	      TabCondition[k].CondTypeElem;
	   switch (TabCondition[k].CondTypeCond)
		 {
		    case TyLangue:
		    case TyFonction:
		       strcpy (pRegle->RTyCondition[i].CondNom, TabCondition[k].CondNom);
		       break;
		    case TyAvantType:
		    case TyApresType:
		       pRegle->RTyCondition[i].CondOpTypeIn = TabCondition[k].CondOpTypeIn;
		       break;
		    default:
		       break;
		 }
	   k++;
	}
   pRegle->RTyNbCond += NbCourCond;
}


/* ---------------------------------------------------------------------- */
/* |    copy_condition  recopie dans la regle courante                  | */
/* |                    la table des conditions courante                | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                copy_condition ()

#else  /* __STDC__ */
void                copy_condition ()
#endif				/* __STDC__ */

{
   int                 i;

   for (i = 1; i <= NbCourCond; i++)
     {
	CourRegle->RTyCondition[i - 1].CondTypeCond =
	   TabCondition[i - 1].CondTypeCond;
	CourRegle->RTyCondition[i - 1].CondNegative =
	   TabCondition[i - 1].CondNegative;
	CourRegle->RTyCondition[i - 1].CondTypeElem =
	   TabCondition[i - 1].CondTypeElem;
	switch (TabCondition[i - 1].CondTypeCond)
	      {
		 case TyLangue:
		 case TyFonction:
		    strcpy (CourRegle->RTyCondition[i - 1].CondNom, TabCondition[i - 1].CondNom);
		    break;
		 case TyAvantType:
		 case TyApresType:
		    CourRegle->RTyCondition[i - 1].CondOpTypeIn = TabCondition[i - 1].CondOpTypeIn;
		    break;
		 default:
		    break;
	      }
	CourRegle->RTyNbCond = NbCourCond;
     }
}


/* ---------------------------------------------------------------------- */
/* |    NewRule         ajoute une nouvelle regle dans la chaine des    | */
/* |                    regles typographiques apres verification        | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                NewRule (TypeFunct TypeF, Name NomF)

#else  /* __STDC__ */
void                NewRule (TypeF, NomF)
TypeFunct           TypeF;
Name                 NomF;

#endif /* __STDC__ */

{
   PtrRegleTypo        pRe1 = NULL;

   if (!ChangeBloc && CourBloc != NULL)
     {
	pRe1 = CourBloc;
	/* verifier si une regle de meme type est */
	/* deja definie dans le bloc courant */
	pRe1 = ChercheRegle (TypeF, pRe1);
     }

   if (pRe1 != NULL)
      /* cette regle est deja utilisee dans le bloc courant */
      /* verifier s'il y a des conditions sur les deux regles */
      if (!DansCondition || pRe1->RTyNbCond == 0)
	 /* simple avertissement : deux regles de mm type sur le mm element */
	 CompilerError (wi, TYP, INFO, TYP_RULE_DEFINED_TWICE, inputLine, LineNum);

   /* creer une nouvelle regle dans le bloc courant */
   nouvregle ();
   CourRegle->RTyTypeFunct = TypeF;
   strncpy (CourRegle->RTyIdentFunct, NomF, MAX_NAME_LENGTH);
   if (DansCondition)
      /* recopier le bloc de conditions associe a cette regle */
      copy_condition ();
   else
      CourRegle->RTyNbCond = 0;
}

/* ---------------------------------------------------------------------- */
/* |    InsererRegle    insere la regle pointee par pReg                | */
/* |                    dans la chaine des                              | */
/* |                    regles typographiques du bloc courant           | */
/* |                    apres verification                              | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                InsererRegle (PtrRegleTypo pReg)

#else  /* __STDC__ */
void                InsererRegle (pReg)
PtrRegleTypo        pReg;

#endif /* __STDC__ */

{
   PtrRegleTypo        pRe1;
   TypeFunct           TypeF;
   Name                 NomF;
   boolean             trouve = False;

   pRe1 = CourBloc;
   TypeF = pReg->RTyTypeFunct;
   strcpy (NomF, pReg->RTyIdentFunct);
   while (pRe1 != NULL && !trouve)
     {
	pRe1 = ChercheRegle (TypeF, pRe1);
	if (pRe1 != NULL)
	   if (strcmp (NomF, pRe1->RTyIdentFunct) == 0)
	      trouve = True;
	   else
	      pRe1 = pRe1->RTyRegleSuiv;
     }
   if (trouve == True)
      /* cette regle est deja utilisee dans le bloc courant */
     {
	/* verifier s'il y a deja des conditions  sur cette regle */
	/* et sur la regle a ajouter */
	if (DansCondition && pRe1->RTyNbCond != 0)

/*
   CompilerError(wi, TYP, FATAL, TYP_RULE_DEFINED_TWICE, inputLine, LineNum);
 */
	  {
	     /* plus tard ???  on pourra envisager de verifier ces conditions */
	     /* ajouter le bloc de condition a la regle existante */
	     add_condition (pRe1);
	  }
     }
   else
      /* ajouter la nouvelle regle en fin du bloc */
     {
	nouvregle ();
	copierregle (pReg);
	if (DansCondition)
	   /* recopier le bloc de conditions associe a cette regle  */
	   add_condition (CourRegle);
     }
}


/* ---------------------------------------------------------------------- */
/* |    InsererModele   insere les regles du modele dans la chaine des  | */
/* |                    regles typographiques du bloc courant           | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                InsererModele (PtrModeleCompo pMod)

#else  /* __STDC__ */
void                InsererModele (pMod)
PtrModeleCompo      pMod;

#endif /* __STDC__ */

{
   PtrRegleTypo        pRe1;

   pRe1 = pMod->MCBlocRegles;
   while (pRe1 != NULL)
     {
	InsererRegle (pRe1);
	pRe1 = pRe1->RTyRegleSuiv;
     }
}


/* ---------------------------------------------------------------------- */
/* |    copie le mot traite dans n                                      | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                copy_generate (Name n, indLine * wi, indLine * wl)

#else  /* __STDC__ */
void                copy_generate (n, wi, wl)
Name                 n;
indLine              *wi;
indLine              *wl;

#endif /* __STDC__ */

{
   int                 j;


   if (*wl > MAX_NAME_LENGTH - 1)
      CompilerError (*wi, TYP, FATAL, TYP_NAME_TOO_LONG, inputLine, LineNum);
   else
     {
	for (j = 1; j <= *wl; j++)
	   n[j - 1] = inputLine[*wi + j - 2];
	n[*wl] = '\0';
     }
}

/* ---------------------------------------------------------------------- */
/* |    NumType si le mot de longueyr wl qui commence a l'indice wi du  | */
/* |    buffer de lecture est un nom de type d'element, retourne le     | */
/* |    numero de type correspondant, sinon retourne 0.                 | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static int          NumType (indLine wi, indLine wl)

#else  /* __STDC__ */
static int          NumType (wi, wl)
indLine               wi;
indLine               wl;

#endif /* __STDC__ */

{
   Name                 n;
   int                 i;

   copy_generate (n, &wi, &wl);
   /* verifie si le type existe dans le schema de structure */
   i = 1;
   while (strcmp (n, pSchStr->SsRule[i - 1].SrName) != 0
	  && i < pSchStr->SsNRules)
      i++;
   if (strcmp (n, pSchStr->SsRule[i - 1].SrName) != 0)
      /* type inconnu */
     {
	CompilerError (wi, TYP, FATAL, TYP_UNKNOWN_TYPE, inputLine, LineNum);
	i = 0;
     }
   return i;
}

/* ---------------------------------------------------------------------- */
/* |    traite le mot commencant a la position wi dans la ligne courante| */
/* |    de longueur wl et de code grammatical c. Si c'est un identif, nb| */
/* |    contient son rang dans la table des identificateurs. r est le   | */
/* |    numero de la regle dans laquelle apparait ce mot.               | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                ProcessToken (indLine wi, indLine wl, SyntacticCode c, SyntacticCode r, int nb, SyntRuleNum pr)

#else  /* __STDC__ */
void                ProcessToken (wi, wl, c, r, nb, pr)
indLine               wi;
indLine               wl;
SyntacticCode             c;
SyntacticCode             r;
int                 nb;
SyntRuleNum                 pr;

#endif /* __STDC__ */

{
   Name                 n;
   int                 k;
   PtrTypoFunction     pFunct;
   PtrModeleCompo      pMod;
   RTypoAttribut      *pRTy1;
   TyCasAttrNum       *pTC1;
   TtAttribute           *pAt1;


   if (c < 1000)
      /* symbole intermediaire de la grammaire, erreur */
      CompilerError (wi, TYP, FATAL, TYP_INTERMEDIATE_SYMBOL, inputLine, LineNum);
   else
     {
	if (c < 1100)		/* mot-cle court */
	   switch (c)
		 {
		    case CHR_59:
		       /*  ;  */
		       if (pr == RULE_Mod)	/* fin d'un modele */
			 {
			    if (CourBloc != NULL)
			       if (!DansBloc)
				  /* fin d'une regle unique dans bloc */
				  /* il faudra ouvrir un nouveau bloc */
				  ChangeBloc = True;
			 }
		       else
			 {
			    if (pr == RULE_SuiteRegles)
			       /* fin d'une regle conditionnelle */
			       DansCondition = 0;
			 }
		       break;

		    case CHR_58:
		       /*  :  */
		       break;

		    case CHR_40:
		       /*  (  */
		       break;

		    case CHR_41:
		       /*  )  */
		       break;

		    case CHR_61:
		       /*  =  */
		       if (pr == RULE_ConditionElem && DansCondition)
			  TabCondition[NbCourCond - 1].CondOpTypeIn = 0;
		       break;	/* rien */

		    case CHR_62:
		       /*  >  */
		       break;

		    case CHR_45:
		       /*  -  */
		       if (r == RULE_AttrRelat || r == RULE_AttrValue)
			  /* devant une valeur d'attribut numerique */
			  SigneAttrVal = -1;
		       break;

		    case CHR_60:
		       /*  <  */
		       break;

		    case CHR_91:
		       /*  [  */
		       break;

		    case CHR_46:
		       /*  .  */
		       break;

		    case CHR_93:
		       /*  ]  */
		       break;

		    default:
		       break;
		 }

	else if (c < 2000)	/* mot-cle long */
	   switch (c)
		 {
		    case KWD_TYPOGRAPHY:

		       break;

		    case KWD_ALINEA:

		       break;

		    case KWD_FUNCTION:

		       break;

		    case KWD_COMPOSITION:
		       ReglesMod = True;	/* on est dans les regles des modeles */
		       ReglesTypes = False;
		       ReglesAttr = False;
		       break;

		    case KWD_ELEMENT:	/* Element */
		       ReglesMod = False;
		       ReglesTypes = True;	/* on est dans les regles des types */
		       ReglesAttr = False;
		       break;

		    case KWD_ATTR:	/* Attr */
		       /* debut des regles de typographie des attributs */
		       ReglesMod = False;
		       ReglesTypes = False;
		       ReglesAttr = True;	/* on est dans les regles des attributs */
		       break;

		    case KWD_END:	/* End */
		       if (r == RULE_SuiteRegles)
			 {
			    if (ReglesMod)
			       /* fin de la definition d'un modele */
			      {
				 if (CourBloc != NULL)
				    ChangeBloc = True;
			      }
			    else
			       /* fin du bloc de regles defini pour un type */
			      {
				 DansBloc = False;
				 if (CourBloc != NULL)
				    ChangeBloc = True;
			      }
			 }
		       else
			 {
			    if (r == RULE_SuiteParam)
			       /* fin d'un bloc param dependant eventuellement */
			       /* d'une table de condition */
			      {
				 DansCondition = False;
				 DansParam = False;
				 NbCourCond = 0;
			      }
			 }
		       break;

		    case KWD_Capitalization:
		       if (pr == RULE_Funct)
			  NewFunction (TyCapital);
		       if (pr == RULE_ParamTypo)
			  CourFunct = TyCapital;
		       break;

		    case KWD_Word:
		       if (pr == RULE_Funct)
			  NewFunction (TyWord);
		       if (pr == RULE_ParamTypo)
			  CourFunct = TyWord;
		       break;

		    case KWD_SpaceTable:
		       if (pr == RULE_Funct)
			  NewFunction (TySpace);
		       if (pr == RULE_ParamTypo)
			  CourFunct = TySpace;
		       break;

		    case KWD_Punctuation:
		       if (pr == RULE_Funct)
			  NewFunction (TyPunct);
		       if (pr == RULE_ParamTypo)
			  CourFunct = TyPunct;
		       break;

		    case KWD_InsertPair:
		       if (pr == RULE_Funct)
			  NewFunction (TyPair);
		       if (pr == RULE_ParamTypo)
			  CourFunct = TyPair;
		       break;

		    case KWD_Distance:
		       if (pr == RULE_Funct)
			  NewFunction (TyDistance);
		       if (pr == RULE_ParamTypo)
			  CourFunct = TyDistance;
		       break;

		    case KWD_Exponent:
		       if (pr == RULE_Funct)
			  NewFunction (TyExponent);
		       if (pr == RULE_ParamTypo)
			  CourFunct = TyExponent;
		       break;

		    case KWD_Abbreviation:
		       if (pr == RULE_Funct)
			  NewFunction (TyAbbrev);
		       if (pr == RULE_ParamTypo)
			  CourFunct = TyAbbrev;
		       break;

		    case KWD_Attribute:
		       if (pr == RULE_Funct)
			  NewFunction (TyAttribute);
		       if (pr == RULE_ParamTypo)
			  CourFunct = TyAttribute;
		       break;

		    case KWD_IF:	/* If */
		       DansCondition = True;
		       NbCourCond = 1;
		       TabCondition[NbCourCond - 1].CondNegative = False;
		       break;

		    case KWD_BEGIN:	/* Begin */
		       if (pr == RULE_SuiteRegles)
			  /* c'est le debut d'un bloc de regles */
			 {
			    DansBloc = True;
			    DansCondition = False;
			 }
		       else if (pr == RULE_SuiteParam)
			  /* c'est le debut d'un bloc de param */
			  DansParam = True;
		       break;

		    case KWD_FunctNil:
		       /* creer la regle apres s'etre assure' */
		       /* qu'une regle de meme type et de meme */
		       /* identfunct n'existe pas dans le bloc courant */
		       strcpy (n, "FunctNone");		/* ??? */
		       NewRule (CourFunct, n);
		       break;

		    case KWD_AND:	/* And */
		       if (NbCourCond >= MaxTyCond)
			  /* trop de conditions */
			  CompilerError (wi, TYP, FATAL, TYP_TOO_MANY_CONDITIONS,
					 inputLine, LineNum);
		       else
			 {
			    NbCourCond++;
			    TabCondition[NbCourCond - 1].CondNegative = False;
			 }
		       break;

		    case KWD_NOT:	/* Not */
		       TabCondition[NbCourCond - 1].CondNegative = True;
		       break;

		    case KWD_First:	/* First */
		       TabCondition[NbCourCond - 1].CondTypeCond = TyPremier;
		       TabCondition[NbCourCond - 1].CondTypeElem = 0;
		       break;

		    case KWD_Last:	/* Last */
		       TabCondition[NbCourCond - 1].CondTypeCond = TyDernier;
		       TabCondition[NbCourCond - 1].CondTypeElem = 0;
		       break;

		    case KWD_IN:	/* IN */
		       if (pr == RULE_OpType)
			  TabCondition[NbCourCond - 1].CondOpTypeIn = True;
		       else
			  TabCondition[NbCourCond - 1].CondTypeCond = TyDansType;
		       break;

		    case KWD_LANGUAGE:
		       TabCondition[NbCourCond - 1].CondTypeCond = TyLangue;
		       break;

		    case KWD_IS:
		       TabCondition[NbCourCond - 1].CondTypeCond = TyFonction;
		       break;

		    case KWD_Next:
		       TabCondition[NbCourCond - 1].CondTypeCond = TyAvantType;
		       break;

		    case KWD_Previous:
		       TabCondition[NbCourCond - 1].CondTypeCond = TyApresType;
		       break;

		    default:
		       break;
		 }

	else			/* type de base */
	   switch (c)
		 {
		    case 3001 /* un nom */ :
		       copy_generate (n, &wi, &wl);
		       switch (r)
			     {
				   /* r= numero regle */
				case RULE_IdentType:	/* TypeIdent */
				   if (pr == RULE_SchemaTypo)
				      /* nom de la structure generique a laquelle se rapporte */
				      /* le schema de typographie */
				     {
					/* lit le schema de structure compile' */
					if (!ReadStructureSchema (n, pSchStr))
					   TtaDisplaySimpleMessage (FATAL, TYP, TYP_CANNOT_READ_STRUCT_SCHEM);	/* echec lecture du  schema de structure */
					else if (strncmp (pSchStr->SsName, n, MAX_NAME_LENGTH) != 0)
					   CompilerError (wi, TYP, FATAL, TYP_STRUCT_SCHEM_DOES_NOT_MATCH, inputLine, LineNum);
					else
					  {
					     strcpy (pSchTypo->STyNomStruct, n);
					     pSchTypo->STyStructCode = pSchStr->SsCode;
					     pSchTypo->STyFunction = NULL;
					  }
				     }
				   else
				      /* c'est un nom de type d'element */
				     {
					/* retrouver le numero de ce type */
					/* dans le schema de structure compile' */
					i = NumType (wi, wl);
					if (i > 0)
					   /* le type existe, il a le numero i */
					  {
					     if (pr == RULE_SuiteType)
						/* liste des ALINEA */
					       {
						  pSchTypo->STyElemAlinea[i - 1] = True;
					       }
					     else
					       {
						  if (pr == RULE_Typo)
						     /* debut d'un bloc de regles associees a un type */
						    {
						       /* verifie si un bloc existe deja pour ce type */
						       if (pSchTypo->STyRegleElem[i - 1] != NULL)
							  CompilerError (wi, TYP, FATAL,
									 TYP_ALREADY_DEFINED,
							  inputLine, LineNum);
						       else
							 {
							    CourType = i;
							    CourBloc = NULL;
							    CourRegle = NULL;
							 }
						    }
						  else if (pr == RULE_ConditionElem)
						     /* nom d'un type dans une condition */
						     /* IN ou Next ou Previous */
						    {
						       if (DansCondition)
							  TabCondition[NbCourCond - 1].CondTypeElem = i;
						    }
						  else if (pr == RULE_TransAttr)
						     /* apres un nom d'attribut */
						    {
						       pSchTypo->STyElemHeritAttr[i - 1] = True;
						       pRTy1 = pSchTypo->STyAttribSem[CourAttr - 1];
						       pRTy1->RTyATypeElem = i;
						    }
					       }
					  }
				     }
				   break;

				case RULE_AttrIdent:	/* AttrIdent */
				   /* copie dans n l'identificateur de l'attribut */
				   /* verifie si l'attribut existe bien dans le schema de */
				   /* structure */
				   i = 1;
				   while (strncmp (n, pSchStr->SsAttribute[i - 1].AttrName, MAX_NAME_LENGTH) != 0
					  && i < pSchStr->SsNAttributes)
				      i++;
				   if (strncmp (n, pSchStr->SsAttribute[i - 1].AttrName, MAX_NAME_LENGTH) != 0)
				      /* attribut inconnu */
				      CompilerError (wi, TYP, FATAL, TYP_UNKNOWN_ATTR, inputLine, LineNum);
				   else
				      /* l'attribut existe, il a le numero i */
				   if (pr == RULE_TransAttr)
				      /* c'est un nom d'attribut auquel on va associer des */
				      /* regles de typographie */
				     {
					CourAttr = i;
					CourValAttr = 0;	/* pas encore rencontre' de valeur */
					CourBloc = NULL;
					CourRegle = NULL;
					if (pSchTypo->STyAttribSem[CourAttr - 1] == NULL)
					   NewAttr (CourAttr);
					pRTy1 = pSchTypo->STyAttribSem[CourAttr - 1];
					switch (pSchStr->SsAttribute[CourAttr - 1].AttrType)
					      {
						 case AtNumAttr:	/* attribut a valeur numerique */
						    if (pRTy1->RTyANbCas >= MaxCasTyAttrNum)
						       /* trop de cas pour cet attribut */
						       CompilerError (wi, TYP, FATAL, TYP_TOO_MANY_CASES_FOR_THAT_ATTR, inputLine, LineNum);
						    else
						       pRTy1->RTyANbCas++;
						    break;

						 case AtTextAttr /* attribut textuel */ :
						    if (pRTy1->RTyATxt != NULL)
						       /* attribut deja rencontre' */
						       CompilerError (wi, TYP, FATAL, TYP_RULES_ALREADY_EXIST_FOR_THAT_ATTR, inputLine, LineNum);
						    break;

						 case AtReferenceAttr:
						    if (pRTy1->RTyARefPremRegle != NULL)
						       /* attribut deja rencontre' */
						       CompilerError (wi, TYP, FATAL, TYP_RULES_ALREADY_EXIST_FOR_THAT_ATTR, inputLine, LineNum);
						    break;

						 case AtEnumAttr:

						    break;

						 default:
						    break;
					      }
				     }
				   break;

				case RULE_IdentFunct:
				   if (pr == RULE_Funct)
				      /* un IdentFunct dans une regle Funct */
				     {
					/* copie l'IdentFunct dans le descripteur de function
					   typographique courant */
					/* verifie si cet IdentFunct est deja dans la chaine */
					/* des fonctions typographiques */
					pFunct = ChercheFonction (n);
					if (pFunct != NULL)
					   /* cette fonction existe deja */
					   CompilerError (wi, TYP, FATAL, TYP_DUPLICATE_FUNCTION, inputLine, LineNum);
					else
					   strncpy (DerFonction->TFIdent, n, MAX_NAME_LENGTH);
				     }
				   else if (pr == RULE_IdFunct)
				      /* un IdentFunct dans une regle IdFunct */
				     {
					/* verifie que cet IdentFunct est defini */
					/* dans une precedente regle funct */
					pFunct = ChercheFonction (n);
					if (pFunct != NULL)
					   /* cette fonction est definie */
					  {
					     /* creer la regle apres s'etre assure' */
					     /* qu'une regle de meme type et de meme */
					     /* identfunct n'existe pas dans le bloc courant */
					     NewRule (CourFunct, n);
					  }
					else
					  {
					     /* erreur: ce nom de fonction est inconnu */
					     CompilerError (wi, TYP, FATAL, TYP_UNKNOWN_FUNCT, inputLine, LineNum);
					  }
				     }
				   break;

				case RULE_LexicalUnit:
				   if (pr == RULE_ConditionElem
				       && TabCondition[NbCourCond - 1].CondTypeCond == TyFonction)
				      /* une LexicalUnit dans une regle ConditionElem */
				     {
					/* verifier si une fonction portant ce nom est definie */
					pFunct = ChercheFonction (n);
					if (pFunct != NULL)
					   /* cette fonction est definie */
					   strcpy (TabCondition[NbCourCond - 1].CondNom, n);
				     }
				   break;

				case RULE_Language:
				   strcpy (TabCondition[NbCourCond - 1].CondNom, n);
				   break;

				case RULE_IdentMod:
				   /* un IdentMod */
				   /* cherche si ce nom de modele existe deja */
				   pMod = ChercheModele (n);
				   if (pr == RULE_Mod)
				      /* un IdentMod dans une regle Mod */
				      /* definition d'une composition */
				     {
					if (pMod == NULL)
					   /* on cree un nouveau modele de composition */
					  {
					     NewModele (n);
					     ChangeBloc = True;
					  }
					else
					   /* erreur: ce nom de modele exite deja */
					   CompilerError (wi, TYP, FATAL, TYP_DUPLICATE_MODEL, inputLine, LineNum);
				     }
				   else if (pr == RULE_ParamTypo)
				      /* un IdentMod dans une regle ParamTypo */
				     {
					if (pMod == NULL)
					   /* erreur: ce nom de modele est inconnu */
					   CompilerError (wi, TYP, FATAL, TYP_UNKNOWN_MODEL, inputLine, LineNum);
					else
					   /* integrer TOUTES les regles du modele */
					   /* dans le bloc courant */
					   InsererModele (pMod);
				     }
				   break;

				case RULE_AttrValIdent:
				   copy_generate (n, &wi, &wl);
				   /* verifie si cette valeur existe pour l'attribut courant */
				   pAt1 = &pSchStr->SsAttribute[CourAttr - 1];
				   if (pAt1->AttrType != AtEnumAttr)
				      /* pas un attribut a valeur enumerees */
				      CompilerError (wi, TYP, FATAL, TYP_INCOR_ATTR_VALUE, inputLine, LineNum);
				   else
				     {
					i = 1;
					while (strcmp (n, pAt1->AttrEnumValue[i - 1]) != 0
					       && i < pAt1->AttrNEnumValues)
					   i++;
					if (strcmp (n, pAt1->AttrEnumValue[i - 1]) != 0)
					   /* valeur d'attribut  incorrecte */
					   CompilerError (wi, TYP, FATAL, TYP_INCOR_ATTR_VALUE, inputLine, LineNum);
					else
					   /* la valeur est correcte, elle a le numero i */
					   /* debut des regles de typographie d'un attribut */
					   if (pSchTypo->STyAttribSem[CourAttr - 1] != NULL
					       && pSchTypo->STyAttribSem[CourAttr - 1]->RTyAValEnum[i] != NULL)
					   /* deja des regles pour cette valeur */
					   CompilerError (wi, TYP, FATAL, TYP_RULES_ALREADY_EXIST_FOR_THAT_VALUE, inputLine, LineNum);
					else
					   CourValAttr = i;
				     }
				   break;
				default:
				   break;
			     }
		       break;

		    case 3002:	/* un nombre */
		       k = AsciiToInt (wi, wl);
		       switch (r)	/* r= numero de regle */
			     {
				case RULE_MinVal:	/* MinVal */
				   if (pSchStr->SsAttribute[CourAttr - 1].AttrType != AtNumAttr
				       || k >= MAX_INT_ATTR_VAL)
				      /* ce n'est pas un attribut numerique */
				      CompilerError (wi, TYP, FATAL, TYP_NOT_A_NUMERICAL_ATTR, inputLine, LineNum);
				   else
				     {
					k = k * SigneAttrVal + 1;
					SigneAttrVal = 1;
					/* a priori, la prochaine valeur */
					/* d'attribut numerique sera positive */
					pRTy1 = pSchTypo->STyAttribSem[CourAttr - 1];
					pRTy1->RTyACas[pRTy1->RTyANbCas - 1].TyANBorneInf = k;
				     }
				   break;

				case RULE_MaxVal:	/* MaxVal */
				   if (pSchStr->SsAttribute[CourAttr - 1].AttrType != AtNumAttr
				       || k >= MAX_INT_ATTR_VAL)
				      /* ce n'est pas un attribut numerique */
				      CompilerError (wi, TYP, FATAL, TYP_NOT_A_NUMERICAL_ATTR, inputLine, LineNum);
				   else
				     {
					k = k * SigneAttrVal - 1;
					SigneAttrVal = 1;
					/* a priori, la prochaine valeur */
					/* d'attribut numerique sera positive */
					pRTy1 = pSchTypo->STyAttribSem[CourAttr - 1];
					pRTy1->RTyACas[pRTy1->RTyANbCas - 1].TyANBorneSup = k;
				     }
				   break;

				case RULE_MinInterval:		/* MinInterval */
				   if (pSchStr->SsAttribute[CourAttr - 1].AttrType != AtNumAttr
				       || k >= MAX_INT_ATTR_VAL)
				      /* ce n'est pas un attribut numerique */
				      CompilerError (wi, TYP, FATAL, TYP_NOT_A_NUMERICAL_ATTR, inputLine, LineNum);
				   else
				     {
					k = k * SigneAttrVal;
					SigneAttrVal = 1;
					/* a priori, la prochaine valeur */
					/* d'attribut numerique sera positive */
					pRTy1 = pSchTypo->STyAttribSem[CourAttr - 1];
					pRTy1->RTyACas[pRTy1->RTyANbCas - 1].TyANBorneInf = k;
				     }
				   break;

				case RULE_MaxInterval:		/* MaxInterval */
				   if (pSchStr->SsAttribute[CourAttr - 1].AttrType != AtNumAttr
				       || k >= MAX_INT_ATTR_VAL)
				      /* ce n'est pas un attribut numerique */
				      CompilerError (wi, TYP, FATAL, TYP_NOT_A_NUMERICAL_ATTR, inputLine, LineNum);
				   else
				     {
					k = k * SigneAttrVal;
					SigneAttrVal = 1;
					/* a priori, la prochaine valeur */
					/* d'attribut numerique sera positive */
					pRTy1 = pSchTypo->STyAttribSem[CourAttr - 1];
					if (pRTy1->RTyACas[pRTy1->RTyANbCas - 1].TyANBorneInf > k)
					   CompilerError (wi, TYP, FATAL, TYP_INCONSISTENT_LIMITS, inputLine, LineNum);
					else
					   pRTy1->RTyACas[pRTy1->RTyANbCas - 1].TyANBorneSup = k;
				     }
				   break;

				case RULE_ValEqual:	/* ValEqual */
				   if (pSchStr->SsAttribute[CourAttr - 1].AttrType != AtNumAttr || k >= MAX_INT_ATTR_VAL)
				      /* ce n'est pas un attribut numerique */
				      CompilerError (wi, TYP, FATAL, TYP_INCONSISTENT_LIMITS, inputLine, LineNum);
				   else
				     {
					k = k * SigneAttrVal;
					SigneAttrVal = 1;
					/* a priori, la prochaine valeur */
					/* d'attribut numerique sera positive */
					pRTy1 = pSchTypo->STyAttribSem[CourAttr - 1];
					pTC1 = &pRTy1->RTyACas[pRTy1->RTyANbCas - 1];
					pTC1->TyANBorneSup = k;
					pTC1->TyANBorneInf = pTC1->TyANBorneSup;
				     }
				   break;

				default:
				   break;
			     }
		       break;

		    case 3003:	/* une chaine de caracteres */
		       /* RULE_TextEqual */
		       if (pSchStr->SsAttribute[CourAttr - 1].AttrType != AtTextAttr)
			  /* ce n'est pas un attribut textuel */
			  CompilerError (wi, TYP, FATAL, TYP_NOT_A_TEXTUAL_ATTR, inputLine, LineNum);
		       else if (wl > MAX_NAME_LENGTH)
			  /* texte trop long */
			  CompilerError (wi, TYP, FATAL, TYP_NAME_TOO_LONG, inputLine, LineNum);
		       else
			 {
			    pRTy1 = pSchTypo->STyAttribSem[CourAttr - 1];
			    for (i = 1; i <= wl - 1; i++)
			       pRTy1->RTyATxtVal[i - 1] = inputLine[wi + i - 2];
			    pRTy1->RTyATxtVal[wl - 1] = '\0';
			 }
		       break;
		 }
     }
}

/* ---------------------------------------------------------------------- */
/* |    Main                                                            | */
/* ---------------------------------------------------------------------- */
/* Main Program */

#ifdef __STDC__
int                 main (int argc, char **argv)

#else  /* __STDC__ */
int                 main (argc, argv)
int                 argc;
char              **argv;

#endif /* __STDC__ */

{
   BinFile             infile;
   boolean             fileOK;
   char                cppFileName[200];
   int                 i;

   TtaInitializeAppRegistry (argv[0]);
   i = TtaGetMessageTable ("libdialogue", LIB_MSG_MAX);
   COMPIL = TtaGetMessageTable ("compildialogue", COMP_MSG_MAX);
   TYP = TtaGetMessageTable ("typdialogue", TYP_MSG_MAX);
   error = False;
   /* initialise l'analyseur syntaxique */
   InitParser ();
   InitSyntax ("TYP.GRM");
   if (!error)
     {
	/* teste les arguments d'appel du programme */
	if (argc != 2)
	   TtaDisplaySimpleMessage (FATAL, TYP, TYP_FILE_NOT_FOUND);
	else
	  {
	     /* recupere le nom du fichier a compiler */
	     strncpy (pfilename, argv[1], MAX_NAME_LENGTH - 1);
	     i = strlen (pfilename);
	     /* ajoute le suffixe .SCH */
	     strcat (pfilename, ".SCH");
	     /* teste si le fichier a compiler existe */
	     if (ThotFile_exist (pfilename) == 0)
		TtaDisplaySimpleMessage (FATAL, TYP, TYP_FILE_NOT_FOUND);
	     else
		/* le fichier d'entree existe, on l'ouvre */
	       {
		  infile = BIOreadOpen (pfilename);
		  /* supprime le suffixe ".SCH" */
		  pfilename[i] = '\0';
		  /* acquiert la memoire pour le schema de typographie */
		  NIdentifiers = 0;	/* table des identificateurs vide */
		  LineNum = 0;
		  initgener ();	/* prepare la generation */

		  /* lit tout le fichier et fait l'analyse */
		  fileOK = True;
		  while (fileOK && !error)
		     /* lit une ligne */
		    {
		       i = 0;
		       do
			 {
			    fileOK = BIOreadByte (infile, &inputLine[i]);
			    i++;
			 }
		       while (i < LINE_LENGTH && inputLine[i - 1] != '\n' && fileOK);
		       /* marque la fin reelle de la ligne */
		       inputLine[i - 1] = '\0'
		       /* incremente le compteur de lignes */ ;
		       LineNum++;
		       if (i >= LINE_LENGTH)
			  CompilerError (1, TYP, FATAL, TYP_LINE_TOO_LONG, inputLine,
					 LineNum);
		       else if (inputLine[0] == '#')
			  /* cette ligne contient une directive du preprocesseur cpp */
			 {
			    sscanf (inputLine, "# %d %s", &LineNum, cppFileName);
			    LineNum--;
			 }
		       else
			  /* traduit les caracteres de la ligne */
			 {
			    OctalToChar ();
			    /* analyse la ligne */
			    wi = 1;
			    wl = 0;
			    /* analyse tous les mots de la ligne courante */
			    do
			      {
				 i = wi + wl;
				 GetNextToken (i, &wi, &wl, &wn);
				 /* mot suivant */
				 if (wi > 0)
				    /* on a trouve un mot */
				   {
				      AnalyzeToken(wi, wl, wn, &c, &r, &nb, &pr);
				      /* on analyse le mot */
				      if (!error)
					 ProcessToken (wi, wl, c, r, nb, pr);
				      /* on le traite */
				   }
			      }
			    while (wi != 0 && !error);
			 }	/* il n'y a plus de mots dans la ligne */
		    }
		  BIOreadClose (infile);
		  if (!error)
		     ParserEnd ();	/* fin d'analyse */
		  if (!error)
		    {
		       /* ecrit le schema compile' dans le fichier de sortie */
		       /* le directory des schemas est le directory courant */
		       SchemaPath[0] = '\0';
		       WrSchTyp (pfilename, pSchTypo, pSchStr);
		    }
		  free (pSchTypo);
		  free (pSchStr);
	       }
	  }
     }
   TtaSaveAppRegistry ();
   exit (0);
}
