
/* -- Copyright (c) 1990 - 1996 Inria/CNRS  All rights reserved. -- */

/* ======================================================================= */
/* |                                                                    | */
/* |                           Projet THOT                              | */
/* |                                                                    | */
/* |    Le programme TRA compile un schema de traduction,               | */
/* |    contenu dans un fichier de type .SCH.                           | */
/* |    TRA est dirige' par la grammaire du langage de traduction       | */
/* |    contenue, sous forme codee, dans le fichier TRANS.GRM.          | */
/* |    Ce fichier .GRM est produit par le programme GRM.               | */
/* |    TRA produit un fichier de type .TRA qui sera ensuite utilise    | */
/* |    par le traducteur, pour guider sa traduction.                   | */
/* |    Ce programme utilise le module ANALSYNT pour l'analyse          | */
/* |    syntaxique du schema a compiler.                                | */
/* |                                                                    | */
/* |                                                                    | */
/* |                    V. Quint        Janvier 1988                    | */
/* |                                                                    | */
/* ======================================================================= */


#include "thot_sys.h"
#include "TRANS.h"
#include "constgrm.h"
#include "constmedia.h"
#include "consttra.h"
#include "typemedia.h"
#include "typegrm.h"
#include "typetra.h"
#include "storage.h"
#include "compilmsg.h"
#include "tramsg.h"
#include "message.h"
#include "thotpattern.h"

#define EXPORT
#include "compil.var"
#include "environ.var"
#include "thotcolor.h"
#include "thotcolor.var"
#include "thotpalette.var"

#undef EXPORT
#define EXPORT extern
#include "analsynt.var"

/* Variables locales du compilateur TRA */
int                 linenb;	/* compteur de lignes */

static PtrSSchema pSchStr;	/* pointeur sur le schema de structure */
static PtrSSchema pSchStrExt;	/* pointeur sur le schema de structure

				   externe */
static PtrTSchema pSchTra;	/* pointeur sur le schema de traduction */
static boolean      BuffDcl;	/* on est dans les declarations de buffers */
static boolean      ConstDcl;	/* on est dans les declaration de constantes */
static boolean      ComptDcl;	/* on est dans les declarations de compteurs */
static boolean      VarDcl;	/* on est dans les declarations de variables */
static boolean      ReglesTypes;	/* on est dans les regles des types */
static boolean      ReglesAttr;	/* on est dans les regles des attributs */
static boolean      ReglesPres;	/* on est dans les regles de la presentation */
static boolean      ChangeBloc;	/* il faut ouvrir un nouveau bloc de regles */
static boolean      DansBloc;	/* on est dans un bloc conditionnel */
static boolean      TextTrans;	/* on est dans les traductions de texte */
static boolean      SymbTrans;	/* on est dans les traductions de symbole */
static boolean      GraphTrans;	/* on est dans les traductions de graphique */
static int          EntreeCour;	/* entree courante de la table de traduction */

				/* de caracteres */
static int          ConstIndx;	/* indice courant dans le buffer des */

				/* constantes */
static int          CurType;	/* numero du type d'element courant */
static int          CurAttr;	/* numero de l'attribut courant */
static int          CurValAttr;	/* numero de la valeur d'attribut */
static int          CurPres;	/* numero de la presentation courante */
static int          CurValPres;	/* numero de la valeur courante de la */

				/* presentation courante */
static PtrTRuleBlock CurBloc;	/* bloc de regles courant */
static PtrTRuleBlock BlocPrec;	/* bloc de regles precedent */
static PtrTRule CurRegle;	/* SRule courante */
static PtrTRule ReglePrec;	/* SRule precedente */
static boolean      SchemaExt;	/* TypeIdent est un nom de schema externe */
static Name          NomGet;	/* TypeIdent dans la regle Get courante */
static int          DebutNomGet;	/* indice du debut du TypeIdent dans la ligne */
static boolean      Immediat;	/* condition Immediately Within */
static boolean      Asterisk;	/* on a rencontre' "*" dans une condition */
static boolean      SignGreaterOrLess;	/* On a rencontre' un signe > ou < apres */

				/* Within ou FirstWithin dans la condition  */
				/* courante */
static Name          NomWithin;	/* TypeIdent dans la condition Within ou */

				/* First Within en cours d'analyse */
static int          DebutNomWithin;	/* indice du debut de ce TypeIdent dans la */

				/* ligne en cours de traitement */
static Name          NomAscend;	/* 1er TypeIdent dans la regle LevelOrType */
static int          DebutNomAscend;	/* indice du debut de ce TypeIdent dans la */

				/* ligne en cours de traitement */
static boolean      DansCondition;	/* On analyse une condition 'If...TRuleBlock' */
static int          SigneAttrVal;	/* Signe d'une valeur d'attribut numerique */
static int          SignePresVal;	/* Signe d'une valeur de presentation */

				/* numerique */
static int          SigneAncetre;	/* signe du dernier niveau d'ancetre */

				/* rencontre' dans un compteur */
static boolean      DefVar;
static boolean      PairePremier;	/* on a rencontre' "First" */
static boolean      PaireSecond;	/* on a rencontre' "Second" */

#include "filesystem.f"
#include "analsynt.f"
#include "rdschstr.f"
#include "wrschtra.f"
#include "cdialog.f"
#include "storage.f"
#include "message.f"
#include "environ.f"
#include "compilmsg.f"

#ifdef __STDC__
extern void         TtaInitializeAppRegistry (char *);
extern void         TtaSaveAppRegistry (void);

#else
extern void         TtaInitializeAppRegistry ();
extern void         TtaSaveAppRegistry ();

#endif /* __STDC__ */

/* ---------------------------------------------------------------------- */
/* |    prepare la generation : initialise le schema de traduction      | */
/* |    en memoire                                                      | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void         initgener ()

#else  /* __STDC__ */
static void         initgener ()
#endif				/* __STDC__ */

{
   int                 i;

   BuffDcl = False;
   ConstDcl = False;
   ComptDcl = False;
   VarDcl = False;
   ReglesTypes = False;
   ReglesAttr = False;
   ReglesPres = False;
   TextTrans = False;
   SymbTrans = False;
   GraphTrans = False;
   ChangeBloc = False;
   DansBloc = False;
   ConstIndx = 1;
   CurBloc = NULL;
   BlocPrec = NULL;
   CurRegle = NULL;
   ReglePrec = NULL;
   SchemaExt = False;
   DansCondition = False;
   SigneAttrVal = 1;
   SignePresVal = 1;
   PairePremier = False;
   PaireSecond = False;
   Asterisk = False;
   NomWithin[0] = '\0';
   NomAscend[0] = '\0';
   if ((pSchStrExt = (PtrSSchema) malloc (sizeof (StructSchema))) == NULL)
      TtaDisplaySimpleMessage (TRA, FATAL, TRA_NOT_ENOUGH_MEM);
   if ((pSchStr = (PtrSSchema) malloc (sizeof (StructSchema))) == NULL)
      TtaDisplaySimpleMessage (TRA, FATAL, TRA_NOT_ENOUGH_MEM);
   if ((pSchTra = (PtrTSchema) malloc (sizeof (TranslSchema))) == NULL)
      TtaDisplaySimpleMessage (TRA, FATAL, TRA_NOT_ENOUGH_MEM);
   /* initialise le schema de traduction */
   pSchTra->TsLineLength = 0;	/* pas de longueur max des lignes traduites */
   strcpy (pSchTra->TsEOL, "\n");	/* caractere fin de ligne par
						   defaut */
   strcpy (pSchTra->TsTranslEOL, "\n");	/* fin de ligne a inserer par
						   defaut */
   pSchTra->TsNConstants = 0;	/* nombre de constantes */
   pSchTra->TsNCounters = 0;	/* nombre de compteurs */
   pSchTra->TsNVariables = 0;	/* nombre de variables de traduction */
   pSchTra->TsNBuffers = 0;	/* nombre de buffers */
   pSchTra->TsPictureBuffer = 0;	/* pas de buffer pour les images */
   for (i = 0; i < MAX_RULES_SSCHEMA; i++)
     {
	pSchTra->TsElemTRule[i] = NULL;		/* pointeurs sur le debut de la chaine
						   de regles de traduction specifiques a chaque type d'element */
	pSchTra->TsInheritAttr[i] = False;
     }
   for (i = 0; i < MAX_TRANSL_PRULE; i++)
      pSchTra->TsPresTRule[i].RtExist = False;
   pSchTra->TsNTranslAlphabets = 0;	/* pas de traduction de texte */
   pSchTra->TsSymbolFirst = 0;	/* indice de la 1ere regle de traduction
					   de symboles dans la table TsCharTransl */
   pSchTra->TsSymbolLast = 0;	/* indice de la derniere regle de traduction de
				   symboles dans la meme table */
   pSchTra->TsGraphicsFirst = 0;	/* indice de la 1ere regle de traduction de
				   graphiques dans la table TsCharTransl */
   pSchTra->TsGraphicsLast = 0;	/* indice de la derniere regle de traduction
				   de graphiques dans la meme table */
   pSchTra->TsNCharTransls = 0;	/* nombre total de regles de traduction de
				   caracteres */
}

/* ---------------------------------------------------------------------- */
/* |    initialise les regles de traduction des attributs               | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void         initAttr ()

#else  /* __STDC__ */
static void         initAttr ()
#endif				/* __STDC__ */

{
   int                 i, j;
   AttributeTransl      *pRT1;
   TranslNumAttrCase        *pTC1;

   for (i = 0; i < pSchStr->SsNAttributes; i++)
     {
	pRT1 = &pSchTra->TsAttrTRule[i];
	pRT1->AtrElemType = 0;
	/* selon le type de l'attribut */
	switch (pSchStr->SsAttribute[i].AttrType)
	      {
		 case AtNumAttr:
		    pRT1->AtrNCases = 0;
		    for (j = 0; j < MAX_TRANSL_ATTR_CASE; j++)
		      {
			 pTC1 = &pRT1->AtrCase[j];
			 pTC1->TaLowerBound = -MAX_INT_ATTR_VAL - 1;	/* - infini */
			 pTC1->TaUpperBound = MAX_INT_ATTR_VAL + 1;		/* + infini */
			 pTC1->TaTRuleBlock = NULL;
		      }
		    break;
		 case AtTextAttr:
		    pRT1->AtrTextValue[0] = '\0';
		    pRT1->AtrTxtTRuleBlock = NULL;
		    break;
		 case AtReferenceAttr:
		    pRT1->AtrRefTRuleBlock = NULL;
		    break;
		 case AtEnumAttr:
		    for (j = 0; j <= MAX_ATTR_VAL; j++)
		       pRT1->AtrEnuTRuleBlock[j] = NULL;
		    break;
		 default:
		    break;
	      }
     }
}

/* ---------------------------------------------------------------------- */
/* |    traite le type d'element indique' dans une condition Within ou  | */
/* |    FirstWithin Le nom du type se trouve dans la variable NomWithin | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void         traiteFinCondition (PtrSSchema pSS)

#else  /* __STDC__ */
static void         traiteFinCondition (pSS)
PtrSSchema        pSS;

#endif /* __STDC__ */

{
   int                 i;

   if (NomWithin[0] != '\0')
      /* verifie si le type existe dans le schema de structure */
     {
	i = 0;
	while (strcmp (NomWithin, pSS->SsRule[i].SrName) != 0 &&
	       i < pSS->SsNRules - 1)
	   i++;
	if (strcmp (NomWithin, pSS->SsRule[i].SrName) != 0)
	   /* type inconnu */
	   CompilerError (DebutNomWithin, TRA, FATAL, TRA_UNKNOWN_TYPE, inputLine,
			  linenb);
	else
	   /* le type existe, il a le numero i */
	   CurBloc->TbCondition[CurBloc->TbNConditions - 1].TcElemType = i + 1;
	NomWithin[0] = '\0';	/* le nom de type a ete traite' */
     }
}

/* ---------------------------------------------------------------------- */
/* |    traite le type d'element indique' dans une clause Ancestor      | */
/* |    Le nom du type se trouve dans la variable NomAscend.            | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void         traiteFinElemConcerne (PtrSSchema pSS)

#else  /* __STDC__ */
static void         traiteFinElemConcerne (pSS)
PtrSSchema        pSS;

#endif /* __STDC__ */

{
   int                 i;

   if (NomAscend[0] != '\0')
      /* verifie si le type existe dans le schema de structure */
     {
	i = 0;
	while (strcmp (NomAscend, pSS->SsRule[i].SrName) != 0 &&
	       i < pSS->SsNRules - 1)
	   i++;
	if (strcmp (NomAscend, pSS->SsRule[i].SrName) != 0)
	   /* type inconnu */
	   CompilerError (DebutNomAscend, TRA, FATAL, TRA_UNKNOWN_TYPE, inputLine,
			  linenb);
	else
	   /* le type existe, il a le numero i */
	   CurBloc->TbCondition[CurBloc->TbNConditions - 1].TcAscendType = i + 1;
	NomAscend[0] = '\0';	/* le nom de type a ete traite' */
     }
}

/* ---------------------------------------------------------------------- */
/* |    traite un nom de regle de presentation passee en parametre      | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void         nomPres (PRuleType TypeRPres, rnb pr, int wi)

#else  /* __STDC__ */
static void         nomPres (TypeRPres, pr, wi)
PRuleType           TypeRPres;
rnb                 pr;
int                 wi;

#endif /* __STDC__ */

{
   int                 i, j;
   PRuleTransl       *pRT1;
   TranslNumAttrCase        *pTC1;

   if (pr == RULE_TransPres)
      /* debut des regles de traduction d'une presentation */
     {
	CurPres = ord (TypeRPres) + 1;
	CurValPres = 0;
	pRT1 = &pSchTra->TsPresTRule[CurPres - 1];
	if (!pRT1->RtExist)
	   /* pas encore rencontre' de regles pour cette presentation */
	  {
	     pRT1->RtExist = True;
	     if (TypeRPres == PtSize
		 || TypeRPres == PtIndent
		 || TypeRPres == PtLineSpacing
		 || TypeRPres == PtLineWeight
		 || TypeRPres == PtFillPattern
		 || TypeRPres == PtBackground
		 || TypeRPres == PtForeground)
		/* Presentation a valeur numerique */
	       {
		  pRT1->RtNCase = 0;
		  for (j = 0; j < MAX_TRANSL_PRES_CASE; j++)
		    {
		       pTC1 = &pRT1->RtCase[j];
		       pTC1->TaLowerBound = -MAX_INT_ATTR_VAL - 1;	/* - infini */
		       pTC1->TaUpperBound = MAX_INT_ATTR_VAL + 1;	/* + infini */
		       pTC1->TaTRuleBlock = NULL;
		    }
	       }
	     else
		for (j = 0; j <= MAX_TRANSL_PRES_VAL; j++)
		  {
		     pRT1->RtPRuleValue[j] = '\0';
		     pRT1->RtPRuleValueBlock[j] = NULL;
		  }
	  }
	CurBloc = NULL;
	CurRegle = NULL;
	if (TypeRPres == PtSize
	    || TypeRPres == PtIndent
	    || TypeRPres == PtLineSpacing
	    || TypeRPres == PtLineWeight
	    || TypeRPres == PtFillPattern
	    || TypeRPres == PtBackground
	    || TypeRPres == PtForeground)
	   /* Presentation a valeur numerique */
	   if (pRT1->RtNCase >= MAX_TRANSL_PRES_CASE)
	      /* trop de cas pour cette presentation */
	      CompilerError (wi, TRA, FATAL, TRA_TOO_MANY_CASES_FOR_THAT_PRES,
			     inputLine, linenb);
	   else
	      pRT1->RtNCase++;	/* un cas de plus */
     }
   else if (pr == RULE_CondOnAscend)
      /* un nom de presentation dans une condition */
     {
	traiteFinElemConcerne (pSchStr);
	i = CurBloc->TbNConditions - 1;
	CurBloc->TbCondition[i].TcCondition = TcondPRule;
	CurBloc->TbCondition[i].TcAttr = ord (TypeRPres);
	if (TypeRPres == PtSize || TypeRPres == PtIndent ||
	    TypeRPres == PtLineSpacing || TypeRPres == PtLineWeight ||
	    TypeRPres == PtFillPattern || TypeRPres == PtBackground ||
	    TypeRPres == PtForeground)
	   /* Presentation a valeur numerique */
	  {
	     CurBloc->TbCondition[i].TcLowerBound = -MAX_INT_ATTR_VAL - 1;
	     /* -infini */
	     CurBloc->TbCondition[i].TcUpperBound = MAX_INT_ATTR_VAL + 1;
	     /* infini */
	  }
	else
	   CurBloc->TbCondition[i].TcPresValue = '\0';
     }
}

/* ---------------------------------------------------------------------- */
/* |    traite le mot-cle' representant une valeur d'une presentation.  | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void         valPres (char CarVal, int wi)

#else  /* __STDC__ */
static void         valPres (CarVal, wi)
char                CarVal;
int                 wi;

#endif /* __STDC__ */

{
   int                 i;
   PRuleTransl       *pRT1;

   if (DansCondition)
      /* valeur d'une presentation dans une condition */
      CurBloc->TbCondition[CurBloc->TbNConditions - 1].TcPresValue = CarVal;
   else
     {
	/* ajoute une nouvelle valeur pour une presentation */
	/* cherche la premier entree libre dans les valeurs de la */
	/* presentation courante */
	pRT1 = &pSchTra->TsPresTRule[CurPres - 1];
	i = 0;
	CurValPres = 0;
	do
	  {
	     i++;
	     if (pRT1->RtPRuleValue[i] == CarVal)
		/* deja des regles pour cette valeur */
		CompilerError (wi, TRA, FATAL, TRA_RULES_ALREADY_EXIST_FOR_THAT_VALUE,
			       inputLine, linenb);
	     else if (pRT1->RtPRuleValue[i] == '\0')
		/* une entree libre, on la prend */
	       {
		  CurValPres = i;
		  pRT1->RtPRuleValue[i] = CarVal;
	       }
	  }
	while (CurValPres == 0 && i <= MAX_TRANSL_PRES_VAL + 1);
     }
}

/* ---------------------------------------------------------------------- */
/* |    superieur                                                       | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static int          superieur (int framet, int lent, int car, boolean * egal,
			       SourceString source)

#else  /* __STDC__ */
static int          superieur (framet, lent, car, egal, source)
int                 framet;
int                 lent;
int                 car;
boolean            *egal;
SourceString            source;

#endif /* __STDC__ */

{
   int                 i, k, s;
   boolean             eq;

   i = framet;
   eq = False;
   while (pSchTra->TsCharTransl[i - 1].StSource[car] < source[car] && i <= lent)
      i++;
   if (i > lent)
      s = lent + 1;
   else if (pSchTra->TsCharTransl[i - 1].StSource[car] > source[car])
      s = i;
   else if (source[car] == '\0')
     {
	eq = True;
	s = i;
     }
   else
     {
	k = i - 1;
	while (pSchTra->TsCharTransl[k].StSource[car] == source[car])
	   k++;
	s = superieur (i, k, car + 1, &eq, source);
     }
   *egal = eq;
   return s;
}

/* ---------------------------------------------------------------------- */
/* |    NouvTradCar     traite la chaine source d'une nouvelle regle de | */
/* |    traduction de caracteres.                                       | */
/* |    indx : index dans inputLine du premier caractere de la chaine,  | */
/* |    lg est la longueur de la chaine.                                | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void         NouvTradCar (int indx, int lg)

#else  /* __STDC__ */
static void         NouvTradCar (indx, lg)
int                 indx;
int                 lg;

#endif /* __STDC__ */

{
   int                 k;
   boolean             egalite;
   SourceString            source;
   int                 PremEntree, DerEntree;
   AlphabetTransl            *pTr1;

   PremEntree = 0;
   DerEntree = 0;
   if (TextTrans)
      /* on est dans les traductions de texte */
     {
	pTr1 = &pSchTra->TsTranslAlphabet[pSchTra->TsNTranslAlphabets - 1];
	PremEntree = pTr1->AlBegin;
	DerEntree = pTr1->AlEnd;
     }
   else if (SymbTrans)
      /* on est dans les traductions de symbole */
     {
	PremEntree = pSchTra->TsSymbolFirst;
	DerEntree = pSchTra->TsSymbolLast;
     }
   else if (GraphTrans)
      /* on est dans les traductions de graphique */
     {
	PremEntree = pSchTra->TsGraphicsFirst;
	DerEntree = pSchTra->TsGraphicsLast;
     }
   if (DerEntree >= MAX_TRANSL_CHAR)
      /* message 'Table saturee' */
      CompilerError (1, TRA, FATAL, TRA_TRANSLATION_TABLE_FULL, inputLine,
		     linenb);
   else
      /* recupere le parametre 'source' */
     {
	for (k = 0; k <= lg - 2; k++)
	   source[k] = inputLine[indx + k - 1];
	source[lg - 1] = '\0';
	EntreeCour = superieur (PremEntree, DerEntree, 0, &egalite, source);
	if (egalite)
	   /* la chaine source est deja dans la table */
	   CompilerError (indx, TRA, FATAL, TRA_TRANSLATION_ALREADY_SPECIFIED,
			  inputLine, linenb);
	else
	   /* la chaine source n'est pas dans la table */
	  {
	     /* on l'insere dans la table */
	     for (k = DerEntree; k >= EntreeCour; k--)
		pSchTra->TsCharTransl[k] = pSchTra->TsCharTransl[k - 1];
	     strncpy (pSchTra->TsCharTransl[EntreeCour - 1].StSource, source,
		      MAX_SRCE_LEN + 1);
	     if (TextTrans)
		/* on est dans les traductions de texte */
	       {
		  pTr1 = &pSchTra->TsTranslAlphabet[pSchTra->TsNTranslAlphabets - 1];
		  pTr1->AlEnd++;
	       }
	     else if (SymbTrans)
		/* on est dans les traductions de symbole */
		pSchTra->TsSymbolLast++;
	     else if (GraphTrans)
		/* on est dans les traduc. de graphique */
		pSchTra->TsGraphicsLast++;
	     pSchTra->TsNCharTransls++;
	  }
     }
}

/* ---------------------------------------------------------------------- */
/* |    TradCible traite la chaine cible de la regle de traduction de   | */
/* |    caracteres en cours de construction.                            | */
/* |    indx : index dans inputLine du premier caractere de la chaine,  | */
/* |    lg est la longueur de la chaine.                                | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void         TradCible (int indx, int lg)

#else  /* __STDC__ */
static void         TradCible (indx, lg)
int                 indx;
int                 lg;

#endif /* __STDC__ */

{
   int                 k;
   TargetString            cible;

   /* recupere le parametre 'cible' */
   for (k = 0; k <= lg - 2; k++)
      cible[k] = inputLine[indx + k - 1];
   cible[lg - 1] = '\0';
   strncpy (pSchTra->TsCharTransl[EntreeCour - 1].StTarget, cible, MAX_TARGET_LEN + 1);
}

/* ---------------------------------------------------------------------- */
/* |    Cree et initialise un nouveau bloc de regles                    | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void         nouvbloc ()

#else  /* __STDC__ */
static void         nouvbloc ()
#endif				/* __STDC__ */

{
   PtrTRuleBlock       bloc;
   AttributeTransl      *pRT1;
   PRuleTransl       *pRTP1;
   PtrTRuleBlock       pBl1;

   ChangeBloc = False;
   if ((bloc = (PtrTRuleBlock) malloc (sizeof (TRuleBlock))) == NULL)
      TtaDisplaySimpleMessage (TRA, FATAL, TRA_NOT_ENOUGH_MEM);
   /* cree un bloc */
   if (CurBloc == NULL)
      /* pas de bloc courant, attache ce bloc au schema de traduction */
      if (ReglesTypes)
	 /* bloc de regles associe' a un type d'element */
	 pSchTra->TsElemTRule[CurType - 1] = bloc;
      else if (ReglesAttr)
	 /* bloc de regles associe' a un attribut */
	{
	   pRT1 = &pSchTra->TsAttrTRule[CurAttr - 1];
	   switch (pSchStr->SsAttribute[CurAttr - 1].AttrType)
		 {
		    case AtNumAttr:
		       pRT1->AtrCase[pRT1->AtrNCases - 1].TaTRuleBlock = bloc;
		       break;
		    case AtTextAttr:
		       pRT1->AtrTxtTRuleBlock = bloc;
		       break;
		    case AtReferenceAttr:
		       pRT1->AtrRefTRuleBlock = bloc;
		       break;
		    case AtEnumAttr:
		       pRT1->AtrEnuTRuleBlock[CurValAttr] = bloc;
		       break;
		    default:
		       break;
		 }
	}
      else
	 /* bloc de regles associe' a une presentation */
	{
	   pRTP1 = &pSchTra->TsPresTRule[CurPres - 1];
	   if (CurPres == ord (PtSize) + 1
	       || CurPres == ord (PtIndent) + 1
	       || CurPres == ord (PtLineSpacing) + 1
	       || CurPres == ord (PtLineWeight) + 1
	       || CurPres == ord (PtFillPattern) + 1
	       || CurPres == ord (PtBackground) + 1
	       || CurPres == ord (PtForeground) + 1)
	      pRTP1->RtCase[pRTP1->RtNCase - 1].TaTRuleBlock = bloc;
	   else
	      pRTP1->RtPRuleValueBlock[CurValPres] = bloc;
	}
   else
      /* il y a un bloc courant, on chaine le nouveau bloc a celui-la */
      CurBloc->TbNextBlock = bloc;
   /* le nouveau bloc de regle devient le bloc courant */
   CurBloc = bloc;
   /* il n'y a plus de regle courante */
   CurRegle = NULL;
   /* initialise le nouveau bloc */
   pBl1 = CurBloc;
   pBl1->TbNextBlock = NULL;
   pBl1->TbFirstTRule = NULL;
   pBl1->TbNConditions = 0;
}

/* ---------------------------------------------------------------------- */
/* |    cree une nouvelle regle, la chaine et l'initialise              | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void         nouvregle ()

#else  /* __STDC__ */
static void         nouvregle ()
#endif				/* __STDC__ */

{
   PtrTRule        r;

   traiteFinCondition (pSchStr);
   DansCondition = False;
   if ((r = (PtrTRule) malloc (sizeof (TranslRule))) == NULL)
      TtaDisplaySimpleMessage (TRA, FATAL, TRA_NOT_ENOUGH_MEM);
   /* cree une nouvelle regle */
   if (CurBloc == NULL || ChangeBloc)
      /* cree un nouveau bloc de regle pour la nouvelle regle */
      nouvbloc ();
   /* chaine la nouvelle regle a la regle courante s'il y en a une, ou au */
   /* bloc de regle courant sinon */
   if (CurRegle == NULL)
      CurBloc->TbFirstTRule = r;
   else
      CurRegle->TrNextTRule = r;
   /* la nouvelle regle devient la regle courante */
   CurRegle = r;
   /* initialise la regle */
   CurRegle->TrNextTRule = NULL;
   CurRegle->TrOrder = TBefore;
}

/* ---------------------------------------------------------------------- */
/* |    traite le type d'element indique' dans une regle Get.           | */
/* |    Le nom du type se trouve dans la variable NomGet.               | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void         traiteTypeIdent (PtrSSchema pSS)

#else  /* __STDC__ */
static void         traiteTypeIdent (pSS)
PtrSSchema        pSS;

#endif /* __STDC__ */

{
   int                 i, j;
   boolean             trouve;
   PtrSSchema        pSc1;
   SRule              *pRe1;


   if (NomGet[0] != '\0')
      /* verifie si le type existe dans le schema de structure */
     {
	i = 1;
	while (strcmp (NomGet, pSS->SsRule[i - 1].SrName) != 0 &&
	       i < pSS->SsNRules)
	   i++;
	if (strcmp (NomGet, pSS->SsRule[i - 1].SrName) != 0)
	   /* type inconnu */
	   CompilerError (DebutNomGet, TRA, FATAL, TRA_UNKNOWN_TYPE, inputLine,
			  linenb);
	else
	   /* le type existe, il a le numero i */ if (CurRegle->TrType == TCreate || CurRegle->TrType == TWrite)
	  {
	     CurRegle->TrObject = ToReferredElem;
	     CurRegle->TrObjectNum = i;
	  }
	else
	   /* c'est une regle Get ou Copy */
	   /* s'agit-il d'un element associe' ? */
	  {
	     if (CurRegle->TrRelPosition == RpSibling)
	       {
		  pSc1 = pSS;
		  j = 0;
		  trouve = False;
		  do
		    {
		       j++;
		       if (j == i)
			  if (pSc1->SsRule[j - 1].SrAssocElem)
			     trouve = True;
			  else
			    {
			       pRe1 = &pSc1->SsRule[j - 1];
			       if (pRe1->SrConstruct == CsList)
				  if (pSc1->SsRule[pRe1->SrListItem - 1].SrAssocElem)
				     trouve = True;
			    }
		    }
		  while (!(j >= pSc1->SsNRules || trouve));
		  if (trouve)
		     CurRegle->TrRelPosition = RpAssoc;
	       }
	     CurRegle->TrElemType = i;
	  }
	NomGet[0] = '\0';
	/* le nom de type a ete traite' */
     }
}

/* ---------------------------------------------------------------------- */
/* |    copie le mot traite dans n                                      | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void         copy (Name n, iline wi, iline wl)

#else  /* __STDC__ */
static void         copy (n, wi, wl)
Name                 n;
iline               wi;
iline               wl;

#endif /* __STDC__ */

{
   if (wl > MAX_NAME_LENGTH - 1)
      CompilerError (wi, TRA, FATAL, TRA_NAME_TOO_LONG, inputLine, linenb);
   else
     {
	strncpy (n, &inputLine[wi - 1], wl);
	n[wl] = '\0';
     }
}

/* ---------------------------------------------------------------------- */
/* |    cree une nouvelle constante                                     | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void         nouvconst (iline wl, iline wi)

#else  /* __STDC__ */
static void         nouvconst (wl, wi)
iline               wl;
iline               wi;

#endif /* __STDC__ */

{
   int                 i;

   if (pSchTra->TsNConstants >= MAX_TRANSL_CONST)
      /* table des constantes saturee */
      CompilerError (wi, TRA, FATAL, TRA_TOO_MANY_CONSTANTS, inputLine, linenb);
   else
      /* alloue un nouvelle entree dans la table des const. */
   if (ConstIndx + wl > MAX_TRANSL_CONST_LEN)
      /* plus de place pour les constantes */
      CompilerError (wi, TRA, FATAL, TRA_CONSTANT_BUFFER_OVERFLOW, inputLine,
		     linenb);
   else
      /* FnCopy le texte de la constante */
     {
	pSchTra->TsNConstants++;
	pSchTra->TsConstBegin[pSchTra->TsNConstants - 1] = ConstIndx;
	for (i = 0; i <= wl - 2; i++)
	   pSchTra->TsConstant[ConstIndx + i - 1] = inputLine[wi + i - 1];
	ConstIndx += wl;
	pSchTra->TsConstant[ConstIndx - 2] = '\0';
     }
}

/* ---------------------------------------------------------------------- */
/* |    cree un attribut dans une regle Create ou Write                 | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void         CrWrAttr (int att, rnb pr, iline wi)

#else  /* __STDC__ */
static void         CrWrAttr (att, pr, wi)
int                 att;
rnb                 pr;
iline               wi;

#endif /* __STDC__ */

{
   SRule              *pRe1;
   TranslVariable            *pVa1;
   boolean             ok;
   int                 j;

   /* on refuse les attributs reference */
   if (pSchStr->SsAttribute[att - 1].AttrType == AtReferenceAttr)
      CompilerError (wi, TRA, FATAL, TRA_REF_ATTR_NOT_ALLOWED, inputLine,
		     linenb);
   else if (pr == RULE_Token)
     {
	CurRegle->TrObject = ToAttr;
	CurRegle->TrObjectNum = att;
     }
   else if (pr == RULE_CountFunction)
      /* un nom d'attribut dans une fonction de comptage, apres le mot-cle' */
      /* INIT. Seuls les attributs numeriques portant sur l'element racine  */
      /* du schema de structure sont autorises ici */
      if (pSchStr->SsAttribute[att - 1].AttrType != AtNumAttr)
	 /* ce n'est pas un attribut numerique, erreur */
	 CompilerError (wi, TRA, FATAL, TRA_NOT_A_NUMERICAL_ATTR, inputLine,
			linenb);
      else
	 /* cherche si l'attribut est un attribut local de la racine */
	{
	   ok = False;
	   pRe1 = &pSchStr->SsRule[pSchStr->SsRootElem - 1];
	   j = 1;
	   while (j <= pRe1->SrNLocalAttrs && !ok)
	      if (pRe1->SrLocalAttr[j - 1] == att)
		 ok = True;
	      else
		 j++;
	   if (!ok)
	      /* l'attribut ne porte pas sur la racine */
	      CompilerError (wi, TRA, FATAL, TRA_NOT_AN_ATTR_FOR_THE_ROOT_ELEM,
			     inputLine, linenb);
	   else
	      /* le compteur courant prendra cet attribut comme valeur initiale */
	      pSchTra->TsCounter[pSchTra->TsNCounters - 1].TnAttrInit = att;
	}
   else if (pr == RULE_Function)
      /* un nom d'attribut dans une variable */
     {
	pVa1 = &pSchTra->TsVariable[pSchTra->TsNVariables - 1];
	if (pVa1->TrvNItems >= MAX_TRANSL_VAR_ITEM)
	   /* variable trop longue */
	   CompilerError (wi, TRA, FATAL, TRA_TOO_MANY_ELEMS_IN_THE_VARIABLE,
			  inputLine, linenb);
	else
	  {
	     pVa1->TrvNItems++;
	     pVa1->TrvItem[pVa1->TrvNItems - 1].TvType = VtAttrVal;
	     pVa1->TrvItem[pVa1->TrvNItems - 1].TvItem = att;
	  }
     }
}


/* ---------------------------------------------------------------------- */
/* |    NumeroPattern   retourne le rang du nom n dans le fichier des   | */
/* |            patterns de Thot.                                       | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static int          NumeroPattern (Name n, iline wi)

#else  /* __STDC__ */
static int          NumeroPattern (n, wi)
Name                 n;
iline               wi;

#endif /* __STDC__ */

{
   int                 i;
   boolean             ok;

   /* cherche le nom dans le tableau des trames Thot */
   i = 0;
   ok = False;
   do
      if (strcmp (n, Name_patterns[i]) == 0)
	 ok = True;
      else
	 i++;
   while (!ok && i < sizeof (Name_patterns) / sizeof (char *));

   if (!ok)
     {
	CompilerError (wi, TRA, FATAL, TRA_PATTERN_NOT_FOUND, inputLine,
		       linenb);
	i = 0;
     }
   return i;
}


/* ---------------------------------------------------------------------- */
/* |    NumeroCouleur   retourne le rang du nom n dans le fichier des   | */
/* |            couleurs de Thot.                                       | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static int          NumeroCouleur (Name n, iline wi)

#else  /* __STDC__ */
static int          NumeroCouleur (n, wi)
Name                 n;
iline               wi;

#endif /* __STDC__ */

{
   int                 i;

   i = 0;
   while (i < MAX_COLOR && strcmp (Name_colors[i], n))
      i++;
   if (i == MAX_COLOR)
      CompilerError (wi, TRA, FATAL, TRA_COLOR_NOT_FOUND, inputLine,
		     linenb);
   return i;
}

/* ---------------------------------------------------------------------- */
/* |    NumType si le mot de longueyr wl qui commence a l'indice wi du  | */
/* |    buffer de lecture est un nom de type d'element, retourne le     | */
/* |    numero de type correspondant, sinon retourne 0.                 | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static int          NumType (iline wi, iline wl)

#else  /* __STDC__ */
static int          NumType (wi, wl)
iline               wi;
iline               wl;

#endif /* __STDC__ */

{
   Name                 n;
   int                 i;

   copy (n, wi, wl);
   /* verifie si le type existe dans le schema de structure */
   i = 1;
   while (strcmp (n, pSchStr->SsRule[i - 1].SrName) != 0
	  && i < pSchStr->SsNRules)
      i++;
   if (strcmp (n, pSchStr->SsRule[i - 1].SrName) != 0)
      /* type inconnu */
     {
	CompilerError (wi, TRA, FATAL, TRA_UNKNOWN_TYPE, inputLine, linenb);
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
void                generate (iline wi, iline wl, grmcode c, grmcode r, int nb, rnb pr)

#else  /* __STDC__ */
void                generate (wi, wl, c, r, nb, pr)
iline               wi;
iline               wl;
grmcode             c;
grmcode             r;
int                 nb;
rnb                 pr;

#endif /* __STDC__ */

{
   int                 i, k;
   Name                 n;
   boolean             DansRegleUse, DansRegleGet, DansRegleCreateWrite;
   AlphabetTransl            *pTr1;
   TranslVariable            *pVa1;
   TCounter         *pTrCo1;
   AttributeTransl      *pRT1;
   TtAttribute           *pAt1;
   TranslNumAttrCase        *pTC1;
   PRuleTransl       *pRTP1;

   i = 1;
   if (c < 1000)
      /* symbole intermediaire de la grammaire, erreur */
      CompilerError (wi, TRA, FATAL, TRA_INTERMEDIATE_SYMBOL, inputLine, linenb);
   else
     {
	if (c < 1100)		/* mot-cle court */
	   switch (c)
		 {
		    case CHR_59:
		       /*  ;  */
		       if (r == RULE_Rule1)	/* fin d'une regle */
			 {
			    if (CurRegle->TrType == TUse)
			       if (CurType == pSchStr->SsRootElem)
				  if (CurRegle->TrNature == pSchStr->SsRule[CurType - 1].SrName)
				     /* une regle Use pour la racine se termine sans 'For' */
				     CompilerError (wi, TRA, FATAL, TRA_FOR_PART_MISSING,
						    inputLine, linenb);
			    if (CurRegle->TrType == TGet ||
				CurRegle->TrType == TCopy ||
				CurRegle->TrType == TCreate ||
				CurRegle->TrType == TWrite)
			       /* fin d'une regle Get, Copy, Create ou Write */
			       traiteTypeIdent (pSchStr);
			    if (!ReglesAttr)
			       if (CurBloc != NULL)
				  if (CurBloc->TbNConditions != 0)
				     if (!DansBloc)
					/* fin d'une regle unique dans bloc conditionnel, il */
					/* faut ouvrir un nouveau bloc pour la regle suivante */
					ChangeBloc = True;
			 }
		       break;

		    case CHR_61:
		       /*  =  */
		       break;

		    case CHR_58:
		       /*  :  */
		       break;

		    case CHR_40:
		       /*  (  */
		       if (r == RULE_Token)
			  if (!DefVar)
			     /* un nom de schema externe va suivre */
			     if (CurRegle->TrObject != ToReferredElem)
				CompilerError (wi, TRA, FATAL, TRA_ONLY_FOR_ELEMS, inputLine,
					       linenb);
			     else
				SchemaExt = True;
			  else
			     /* debut d'une definition de variable */
			  if (pSchTra->TsNVariables >= MAX_TRANSL_VARIABLE)
			     /* table des variables saturee */
			     CompilerError (wi, TRA, FATAL, TRA_TOO_MANY_VARIABLES,
					    inputLine, linenb);
			  else
			    {
			       VarDcl = True;
			       pSchTra->TsNVariables++;
			       pSchTra->TsVariable[pSchTra->TsNVariables - 1].TrvNItems = 0;
			       /* la regle courante utilise cette variable */
			       CurRegle->TrObject = ToVariable;
			       CurRegle->TrObjectNum = pSchTra->TsNVariables;
			    }
		       else if (r == RULE_Rule1)
			  SchemaExt = True;
		       else if (r == RULE_CondOnAscend || r == RULE_LevelOrType)
			  /* On est dans une condition. Un nom de schema externe suit */
			  SchemaExt = True;
		       break;

		    case CHR_41:
		       /*  )  */
		       if (r == RULE_Token)
			  /* fin d'une definition de variable */
			  VarDcl = False;
		       break;

		    case CHR_44:
		       /*  ,  */
		       break;

		    case CHR_62:
		       /*  >  */
		       if (r == RULE_GreaterLess)
			 {
			    CurBloc->TbCondition[CurBloc->TbNConditions - 1].TcAscendRel = RelGreater;
			    SignGreaterOrLess = True;
			 }
		       break;

		    case CHR_45:
		       /*  -  */
		       if (r == RULE_AttrRelat || r == RULE_AttrValue)
			  /* devant une valeur d'attribut numerique */
			  SigneAttrVal = -1;
		       else if (r == RULE_PresRelat || r == RULE_PresValue)
			  /* devant une valeur numerique de presentation */
			  SignePresVal = -1;
		       else if (r == RULE_RelAncestorLevel)
			  /* devant le niveau relatif d'un ancetre dans un compteur CntrRank */
			  SigneAncetre = -1;
		       break;

		    case CHR_60:
		       /*  <  */
		       if (r == RULE_GreaterLess)
			 {
			    CurBloc->TbCondition[CurBloc->TbNConditions - 1].TcAscendRel = RelLess;
			    SignGreaterOrLess = True;
			 }
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

		    case CHR_42:
		       /*  *  */
		       Asterisk = True;
		       break;

		    default:
		       break;
		 }

	else if (c < 2000)	/* mot-cle long */
	   switch (c)
		 {
		    case KWD_Buffers:	/* Buffers */
		       BuffDcl = True;	/* on est dans les declarations de buffers */
		       break;

		    case KWD_Counters:		/* Counters */
		       BuffDcl = False;		/* fin des declarations de buffers */
		       ConstDcl = False;	/* fin des declaration de constantes */
		       ComptDcl = True;		/* on est dans les declarations de compteur */
		       break;

		    case KWD_Const:	/* Const */
		       BuffDcl = False;		/* fin des declarations de buffers */
		       ConstDcl = True;		/* on est dans les declaration de constante */
		       break;

		    case KWD_Var:	/* Var */
		       BuffDcl = False;		/* fin des declarations de buffers */
		       ConstDcl = False;	/* fin des declaration de constantes */
		       ComptDcl = False;	/* fin des declarations de compteurs */
		       VarDcl = True;	/* on est dans les declarations de variables */
		       break;

		    case KWD_Rules:	/* Rules */
		       BuffDcl = False;		/* fin des declarations de buffers */
		       ConstDcl = False;	/* fin des declaration de constantes */
		       ComptDcl = False;	/* fin des declarations de compteurs */
		       VarDcl = False;	/* fin des declarations de variables */
		       ReglesTypes = True;
		       break;

		    case KWD_Attributes:	/* Attributes */
		       if (r == RULE_TransSchema)
			  /* debut des regles de traduction des attributs */
			 {
			    ReglesTypes = False;
			    ReglesAttr = True;
			 }
		       else if (r == RULE_CondOnAscend)		/* dans une condition */
			 {
			    traiteFinElemConcerne (pSchStr);
			    CurBloc->TbCondition[CurBloc->TbNConditions - 1].TcCondition =
			       TcondAttributes;
			 }
		       else if (r == RULE_Token)
			  /* ce qu'il faut generer : les  attributs de l'element */
			  CurRegle->TrObject = ToAllAttr;
		       break;

		    case KWD_TextTranslate:	/* TextTranslate */
		       ReglesTypes = False;
		       ReglesAttr = False;
		       ReglesPres = False;
		       TextTrans = True;
		       /* on est dans les traductions de texte */
		       if (pSchTra->TsNTranslAlphabets >= MAX_TRANSL_ALPHABET)
			  CompilerError (wi, TRA, FATAL, TRA_TOO_MANY_TEXTTRANSLATE,
					 inputLine, linenb);
		       else
			 {
			    pSchTra->TsNTranslAlphabets++;
			    pTr1 = &pSchTra->TsTranslAlphabet[pSchTra->TsNTranslAlphabets - 1];
			    pTr1->AlAlphabet = 'L';	/* alphabet latin par defaut */
			    if (pSchTra->TsNTranslAlphabets == 1)
			      {
				 pTr1->AlBegin = 1;
				 pTr1->AlEnd = 0;
			      }
			    else
			      {
				 pTr1->AlBegin =
				    pSchTra->TsTranslAlphabet[pSchTra->TsNTranslAlphabets - 2].AlEnd + 1;
				 pTr1->AlEnd =
				    pSchTra->TsTranslAlphabet[pSchTra->TsNTranslAlphabets - 2].AlEnd;
			      }
			 }
		       break;

		    case KWD_SymbTranslate:	/* SymbTranslate */
		       ReglesTypes = False;
		       ReglesAttr = False;
		       ReglesPres = False;
		       TextTrans = False;	/* fin des traductions de texte */
		       SymbTrans = True;	/* on est dans les traductions de symbole */
		       if (pSchTra->TsNTranslAlphabets == 0)
			 {
			    pSchTra->TsSymbolFirst = 1;
			    pSchTra->TsSymbolLast = 0;
			 }
		       else
			 {
			    pTr1 = &pSchTra->TsTranslAlphabet[pSchTra->TsNTranslAlphabets - 1];
			    if (pTr1->AlEnd > 0)
			      {
				 pSchTra->TsSymbolFirst = pTr1->AlEnd + 1;
				 pSchTra->TsSymbolLast = pTr1->AlEnd;
			      }
			    else
			      {
				 pSchTra->TsSymbolFirst = 1;
				 pSchTra->TsSymbolLast = 0;
			      }
			 }
		       break;

		    case KWD_GraphTranslate:	/* GraphTranslate */
		       ReglesTypes = False;
		       ReglesAttr = False;
		       ReglesPres = False;
		       TextTrans = False;	/* fin des traductions de texte */
		       SymbTrans = False;	/* fin des traductions de symbole */
		       GraphTrans = True;	/* on est dans les traduc. de graphique */
		       if (pSchTra->TsSymbolLast > 0)
			 {
			    pSchTra->TsGraphicsFirst = pSchTra->TsSymbolLast + 1;
			    pSchTra->TsGraphicsLast = pSchTra->TsSymbolLast;
			 }
		       else if (pSchTra->TsNTranslAlphabets == 0)
			 {
			    pSchTra->TsGraphicsFirst = 1;
			    pSchTra->TsGraphicsLast = 0;
			 }
		       else
			 {
			    pTr1 = &pSchTra->TsTranslAlphabet[pSchTra->TsNTranslAlphabets - 1];
			    if (pTr1->AlEnd > 0)
			      {
				 pSchTra->TsGraphicsFirst = pTr1->AlEnd + 1;
				 pSchTra->TsGraphicsLast = pTr1->AlEnd;
			      }
			    else
			      {
				 pSchTra->TsGraphicsFirst = 1;
				 pSchTra->TsGraphicsLast = 0;
			      }
			 }
		       break;

		    case KWD_End:	/* End */
		       if (r == RULE_SeqOfRules1 && !ReglesAttr)
			 {
			    DansBloc = False;
			    if (CurBloc != NULL)
			       if (CurBloc->TbNConditions != 0)
				  /* fin d'un bloc conditionnel, il faut ouvrir un */
				  /* nouveau bloc pour la regle suivante */
				  ChangeBloc = True;
			 }
		       break;

		    case KWD_Rank:	/* CntrRank */
		       pSchTra->TsCounter[pSchTra->TsNCounters - 1].TnOperation = TCntrRank;
		       pSchTra->TsCounter[pSchTra->TsNCounters - 1].TnAcestorLevel = 0;
		       SigneAncetre = 1;
		       break;

		    case KWD_of:	/* Of */

		       break;	/* rien */

		    case KWD_Rlevel:	/* CntrRLevel */
		       pSchTra->TsCounter[pSchTra->TsNCounters - 1].TnOperation = TCntrRLevel;
		       break;

		    case KWD_Set:	/* Set */
		       if (r == RULE_CountFunction)
			  /* dans une definition de compteur */
			 {
			    pTrCo1 = &pSchTra->TsCounter[pSchTra->TsNCounters - 1];
			    pTrCo1->TnOperation = TCntrSet;
			    pTrCo1->TnElemType1 = 0;
			 }
		       else if (r == RULE_Rule1)
			  /* instruction de traduction Set */
			 {
			    nouvregle ();
			    CurRegle->TrType = TSetCounter;
			 }
		       break;

		    case KWD_On:	/* On */

		       break;	/* rien */

		    case KWD_Add:	/* Add */
		       if (r == RULE_Rule1)
			  /* instruction de traduction Add */
			 {
			    nouvregle ();
			    CurRegle->TrType = TAddCounter;
			 }
		       break;

		    case KWD_Picture:	/* Picture */
		       if (pSchTra->TsPictureBuffer != 0)
			  /* un seul buffer image autorise' */
			  CompilerError (wi, TRA, FATAL, TRA_ONLY_ONE_PICTURE_BUFFER_PLEASE, inputLine, linenb);
		       else	/* c'est le buffer courant qui est le buffer image */
			  pSchTra->TsPictureBuffer = pSchTra->TsNBuffers;
		       break;

		    case KWD_Value:	/* Value */
		       if (r == RULE_Token)
			  if (!ReglesPres)
			     CompilerError (wi, TRA, FATAL, TRA_ONLY_IN_PRES_PART, inputLine, linenb);
		       /* seulement dans les regles de la presentation */
			  else
			     CurRegle->TrObject = ToPRuleValue;
		       else if (r == RULE_Function)
			  /* valeur d'un compteur dans une definition de variable */
			 {
			    pVa1 = &pSchTra->TsVariable[pSchTra->TsNVariables - 1];
			    if (pVa1->TrvNItems >= MAX_TRANSL_VAR_ITEM)
			       /* trop de fonctions */
			       CompilerError (wi, TRA, FATAL, TRA_TOO_MANY_ELEMS_IN_THE_VARIABLE, inputLine, linenb);
			    else
			      {
				 pVa1->TrvNItems++;
				 pVa1->TrvItem[pVa1->TrvNItems - 1].TvType = VtCounter;
			      }
			 }
		       break;

		    case KWD_FileDir:	/* FileDir */
		       if (r == RULE_Function)
			 {
			    pVa1 = &pSchTra->TsVariable[pSchTra->TsNVariables - 1];
			    if (pVa1->TrvNItems >= MAX_TRANSL_VAR_ITEM)
			       /* trop de fonctions */
			       CompilerError (wi, TRA, FATAL, TRA_TOO_MANY_ELEMS_IN_THE_VARIABLE, inputLine, linenb);
			    else
			      {
				 pVa1->TrvNItems++;
				 pVa1->TrvItem[pVa1->TrvNItems - 1].TvType = VtFileDir;
			      }
			 }
		       else if (r == RULE_Token)
			  /* generer le nom du directory du fichier de sortie */
			  CurRegle->TrObject = ToFileDir;
		       break;

		    case KWD_FileName:
		       if (r == RULE_Function)
			 {
			    pVa1 = &pSchTra->TsVariable[pSchTra->TsNVariables - 1];
			    if (pVa1->TrvNItems >= MAX_TRANSL_VAR_ITEM)
			       /* trop de fonctions */
			       CompilerError (wi, TRA, FATAL, TRA_TOO_MANY_ELEMS_IN_THE_VARIABLE, inputLine, linenb);
			    else
			      {
				 pVa1->TrvNItems++;
				 pVa1->TrvItem[pVa1->TrvNItems - 1].TvType = VtFileName;
			      }
			 }
		       else if (r == RULE_Token)
			  /* generer le nom du fichier de sortie */
			  CurRegle->TrObject = ToFileName;
		       break;

		    case KWD_Extension:
		       if (r == RULE_Function)
			 {
			    pVa1 = &pSchTra->TsVariable[pSchTra->TsNVariables - 1];
			    if (pVa1->TrvNItems >= MAX_TRANSL_VAR_ITEM)
			       /* trop de fonctions */
			       CompilerError (wi, TRA, FATAL, TRA_TOO_MANY_ELEMS_IN_THE_VARIABLE, inputLine, linenb);
			    else
			      {
				 pVa1->TrvNItems++;
				 pVa1->TrvItem[pVa1->TrvNItems - 1].TvType = VtExtension;
			      }
			 }
		       else if (r == RULE_Token)
			  /* generer l'extensiion (suffixe) du fichier de sortie */
			  CurRegle->TrObject = ToExtension;
		       break;

		    case KWD_RefId:	/* RefId */
		       if (r == RULE_Token)
			  /* generer le label de l'element traduit */
			  CurRegle->TrObject = ToRefId;
		       else if (r == RULE_VarOrType)
			  /* generer le label de l'element reference' */
			  if ((ReglesTypes && pSchStr->SsRule[CurType - 1].SrConstruct != CsReference) ||
			      (ReglesAttr && pSchStr->SsAttribute[CurAttr - 1].AttrType != AtReferenceAttr))
			     /* l'element ou l'attribut auquel s'applique la regle */
			     /* n'est pas une reference, erreur */
			     CompilerError (wi, TRA, FATAL, TRA_ONLY_FOR_REFS, inputLine, linenb);
			  else
			     CurRegle->TrObject = ToReferredRefId;
		       break;

		    case KWD_PairId:	/* PairId */
		       if (pSchStr->SsRule[CurType - 1].SrConstruct != CsPairedElement)
			  /* l'element auquel s'applique la regle n'est pas une paire */
			  CompilerError (wi, TRA, FATAL, TRA_NOT_A_PAIR,
					 inputLine, linenb);
		       else
			  CurRegle->TrObject = ToPairId;
		       break;

		    case KWD_IN:	/* IN */

		       break;

		    case KWD_Begin:	/* Begin */
		       if (r == RULE_SeqOfRules1)
			 {
			    traiteFinCondition (pSchStr);
			    if (!ReglesAttr)
			       DansBloc = True;
			 }
		       break;

		    case KWD_If:	/* If */
		       DansCondition = True;
		       nouvbloc ();
		       CurBloc->TbNConditions = 1;
		       CurBloc->TbCondition[0].TcNegativeCond = False;
		       CurBloc->TbCondition[0].TcTarget = False;
		       CurBloc->TbCondition[0].TcAscendType = 0;
		       CurBloc->TbCondition[0].TcAscendNature[0] = '\0';
		       CurBloc->TbCondition[0].TcAscendRelLevel = 0;
		       Immediat = False;
		       Asterisk = False;
		       break;

		    case KWD_And:	/* And */
		       if (CurBloc->TbNConditions >= MAX_TRANSL_COND)
			  /* trop de conditions */
			  CompilerError (wi, TRA, FATAL, TRA_TOO_MANY_CONDITIONS, inputLine, linenb);
		       else
			 {
			    traiteFinCondition (pSchStr);
			    i = CurBloc->TbNConditions;
			    CurBloc->TbNConditions++;
			    CurBloc->TbCondition[i].TcNegativeCond = False;
			    CurBloc->TbCondition[i].TcTarget = False;
			    CurBloc->TbCondition[i].TcAscendType = 0;
			    CurBloc->TbCondition[i].TcAscendNature[0] = '\0';
			    CurBloc->TbCondition[i].TcAscendRelLevel = 0;
			    Immediat = False;
			    Asterisk = False;
			 }
		       break;

		    case KWD_Not:	/* Not */
		       CurBloc->TbCondition[CurBloc->TbNConditions - 1].TcNegativeCond = True;
		       break;

		    case KWD_Target:	/* Target */
		       if ((ReglesTypes && pSchStr->SsRule[CurType - 1].SrConstruct != CsReference) ||
			   (ReglesAttr && pSchStr->SsAttribute[CurAttr - 1].AttrType != AtReferenceAttr))
			  /* l'element ou l'attribut auquel s'applique la regle */
			  /* n'est pas une reference, erreur */
			  CompilerError (wi, TRA, FATAL, TRA_ONLY_FOR_REFS, inputLine, linenb);
		       else
			  CurBloc->TbCondition[CurBloc->TbNConditions - 1].TcTarget = True;
		       break;

		    case KWD_First:	/* First */
		       if (r == RULE_FirstSec)
			  PairePremier = True;
		       else
			 {
			    traiteFinElemConcerne (pSchStr);
			    CurBloc->TbCondition[CurBloc->TbNConditions - 1].TcCondition = TcondFirst;
			 }
		       break;

		    case KWD_Second:	/* Second */
		       PaireSecond = True;
		       break;
		    case KWD_Last:	/* Last */
		       traiteFinElemConcerne (pSchStr);
		       CurBloc->TbCondition[CurBloc->TbNConditions - 1].TcCondition = TcondLast;
		       break;
		    case KWD_Defined:	/* Defined */
		       if (pSchStr->SsRule[CurType - 1].SrParamElem)
			  CurBloc->TbCondition[CurBloc->TbNConditions - 1].TcCondition = TcondDefined;
		       else
			  CompilerError (wi, TRA, FATAL, TRA_ONLY_FOR_PARAMETERS, inputLine, linenb);
		       break;

		    case KWD_Refered:	/* Refered */
		    case KWD_Referred:		/* Referred */
		       if (r == RULE_CondOnAscend)	/* dans une condition */
			 {
			    traiteFinElemConcerne (pSchStr);
			    CurBloc->TbCondition[CurBloc->TbNConditions - 1].TcCondition = TcondReferred;
			    if (!Asterisk)
			       CurBloc->TbCondition[CurBloc->TbNConditions - 1].TcAscendRelLevel = -1;
			 }
		       else if ((ReglesTypes && pSchStr->SsRule[CurType - 1].SrConstruct != CsReference) ||
				(ReglesAttr && pSchStr->SsAttribute[CurAttr - 1].AttrType != AtReferenceAttr))
			  /* l'element ou l'attribut auquel s'applique la regle n'est */
			  /* pas une reference, erreur */
			  CompilerError (wi, TRA, FATAL, TRA_ONLY_FOR_REFS, inputLine, linenb);
		       else if (r == RULE_Token)	/* devant un identificateur de variable */
			  CurRegle->TrReferredObj = True;
		       else if (r == RULE_RelPosition)	/* dans une position relative */
			  CurRegle->TrRelPosition = RpReferred;
		       break;

		    case KWD_FirstRef:		/* FirstRef */
		       if ((ReglesTypes && pSchStr->SsRule[CurType - 1].SrConstruct != CsReference) ||
			   (ReglesAttr && pSchStr->SsAttribute[CurAttr - 1].AttrType != AtReferenceAttr))
			  /* l'element ou l'attribut auquel s'applique la regle n'est */
			  /* pas une reference, erreur */
			  CompilerError (wi, TRA, FATAL, TRA_ONLY_FOR_REFS, inputLine, linenb);
		       else
			  CurBloc->TbCondition[CurBloc->TbNConditions - 1].TcCondition = TcondFirstRef;
		       break;

		    case KWD_LastRef:	/* LastRef */
		       if ((ReglesTypes && pSchStr->SsRule[CurType - 1].SrConstruct != CsReference) ||
			   (ReglesAttr && pSchStr->SsAttribute[CurAttr - 1].AttrType != AtReferenceAttr))
			  /* l'element ou l'attribut auquel s'applique la regle n'est */
			  /* pas une reference, erreur */
			  CompilerError (wi, TRA, FATAL, TRA_ONLY_FOR_REFS, inputLine, linenb);
		       else
			  CurBloc->TbCondition[CurBloc->TbNConditions - 1].TcCondition = TcondLastRef;
		       break;

		    case KWD_ExternalRef:	/* ExternalRef */
		       if ((ReglesTypes && pSchStr->SsRule[CurType - 1].SrConstruct != CsReference) ||
			   (ReglesAttr && pSchStr->SsAttribute[CurAttr - 1].AttrType != AtReferenceAttr))
			  /* l'element ou l'attribut auquel s'applique la regle n'est */
			  /* pas une reference, erreur */
			  CompilerError (wi, TRA, FATAL, TRA_ONLY_FOR_REFS, inputLine, linenb);
		       else
			  CurBloc->TbCondition[CurBloc->TbNConditions - 1].TcCondition = TcondExternalRef;
		       break;

		    case KWD_Immediately:
		       Immediat = True;
		       break;

		    case KWD_Within:	/* Within */
		       traiteFinElemConcerne (pSchStr);
		       CurBloc->TbCondition[CurBloc->TbNConditions - 1].TcCondition = TcondWithin;
		       CurBloc->TbCondition[CurBloc->TbNConditions - 1].TcImmediatelyWithin = Immediat;
		       Immediat = False;
		       CurBloc->TbCondition[CurBloc->TbNConditions - 1].TcAscendRel = RelGreater;
		       CurBloc->TbCondition[CurBloc->TbNConditions - 1].TcAscendLevel = 0;
		       SignGreaterOrLess = False;
		       CurBloc->TbCondition[CurBloc->TbNConditions - 1].TcElemNature[0] = '\0';
		       NomWithin[0] = '\0';
		       break;

		    case KWD_FirstWithin:	/* FirstWithin */
		       traiteFinElemConcerne (pSchStr);
		       CurBloc->TbCondition[CurBloc->TbNConditions - 1].TcCondition = TcondFirstWithin;
		       CurBloc->TbCondition[CurBloc->TbNConditions - 1].TcImmediatelyWithin = False;
		       Immediat = False;
		       CurBloc->TbCondition[CurBloc->TbNConditions - 1].TcAscendRel = RelGreater;
		       CurBloc->TbCondition[CurBloc->TbNConditions - 1].TcAscendLevel = 0;
		       SignGreaterOrLess = False;
		       CurBloc->TbCondition[CurBloc->TbNConditions - 1].TcElemNature[0] = '\0';
		       NomWithin[0] = '\0';
		       break;

		    case KWD_Presentation:	/* Presentation */
		       if (ReglesPres)
			  CompilerError (wi, TRA, FATAL, TRA_FORBIDDEN_IN_PRES_PART, inputLine, linenb);
		       else if (r == RULE_TransSchema)
			  /* debut des regles de traduction de la presentation */
			 {
			    ReglesTypes = False;
			    ReglesAttr = False;
			    ReglesPres = True;
			 }
		       else if (r == RULE_CondOnAscend)
			  /* dans une condition */
			 {
			    traiteFinElemConcerne (pSchStr);
			    CurBloc->TbCondition[CurBloc->TbNConditions - 1].TcCondition = TcondPresentation;
			 }
		       else if (r == RULE_Token)
			  /* ce qu'il faut generer */
			  CurRegle->TrObject = ToAllPRules;
		       break;

		    case KWD_Comment:	/* Comment */
		       if (r == RULE_CondOnAscend)
			  /* dans une condition */
			 {
			    traiteFinElemConcerne (pSchStr);

			    CurBloc->TbCondition[CurBloc->TbNConditions - 1].TcCondition = TcondComment;
			 }
		       else if (r == RULE_Token)
			  /* ce qu'il faut generer */
			  CurRegle->TrObject = ToComment;
		       break;

		    case KWD_DocumentName:	/* DocumentName */
		       if (r == RULE_Function)
			 {
			    pVa1 = &pSchTra->TsVariable[pSchTra->TsNVariables - 1];
			    if (pVa1->TrvNItems >= MAX_TRANSL_VAR_ITEM)
			       /* trop de fonctions */
			       CompilerError (wi, TRA, FATAL, TRA_TOO_MANY_ELEMS_IN_THE_VARIABLE, inputLine, linenb);
			    else
			      {
				 pVa1->TrvNItems++;
				 pVa1->TrvItem[pVa1->TrvNItems - 1].TvType = VtDocumentName;
			      }
			 }
		       else if (r == RULE_Token)
			  /* generer le nom du document traduit */
			  CurRegle->TrObject = ToDocumentName;
		       else if (r == RULE_VarOrType)
			  /* generer le nom du document reference' */
			  if ((ReglesTypes && pSchStr->SsRule[CurType - 1].SrConstruct != CsReference) ||
			      (ReglesAttr && pSchStr->SsAttribute[CurAttr - 1].AttrType != AtReferenceAttr))
			     /* l'element ou l'attribut auquel s'applique la regle */
			     /* n'est pas une reference, erreur */
			     CompilerError (wi, TRA, FATAL, TRA_ONLY_FOR_REFS, inputLine, linenb);
			  else
			     CurRegle->TrObject = ToReferredDocumentName;
		       break;

		    case KWD_DocumentDir:	/* DocumentDir */
		       if (r == RULE_Function)
			 {
			    pVa1 = &pSchTra->TsVariable[pSchTra->TsNVariables - 1];
			    if (pVa1->TrvNItems >= MAX_TRANSL_VAR_ITEM)
			       /* trop de fonctions */
			       CompilerError (wi, TRA, FATAL, TRA_TOO_MANY_ELEMS_IN_THE_VARIABLE, inputLine, linenb);
			    else
			      {
				 pVa1->TrvNItems++;
				 pVa1->TrvItem[pVa1->TrvNItems - 1].TvType = VtDocumentDir;
			      }
			 }
		       else if (r == RULE_Token)
			  /* generer le nom du directory du document traduit */
			  CurRegle->TrObject = ToDocumentDir;
		       else if (r == RULE_VarOrType)
			  /* generer le nom du directory du document reference' */
			  if ((ReglesTypes && pSchStr->SsRule[CurType - 1].SrConstruct != CsReference) ||
			      (ReglesAttr && pSchStr->SsAttribute[CurAttr - 1].AttrType != AtReferenceAttr))
			     /* l'element ou l'attribut auquel s'applique la regle */
			     /* n'est pas une reference, erreur */
			     CompilerError (wi, TRA, FATAL, TRA_ONLY_FOR_REFS, inputLine, linenb);
			  else
			     CurRegle->TrObject = ToReferredDocumentDir;
		       break;

		    case KWD_ARABIC:
		       pVa1 = &pSchTra->TsVariable[pSchTra->TsNVariables - 1];
		       pVa1->TrvItem[pVa1->TrvNItems - 1].TvCounterStyle = CntArabic;
		       break;

		    case KWD_UROMAN:
		       pVa1 = &pSchTra->TsVariable[pSchTra->TsNVariables - 1];
		       pVa1->TrvItem[pVa1->TrvNItems - 1].TvCounterStyle = CntURoman;
		       break;

		    case KWD_LROMAN:
		       pVa1 = &pSchTra->TsVariable[pSchTra->TsNVariables - 1];
		       pVa1->TrvItem[pVa1->TrvNItems - 1].TvCounterStyle = CntLRoman;
		       break;

		    case KWD_UPPERCASE:
		       pVa1 = &pSchTra->TsVariable[pSchTra->TsNVariables - 1];
		       pVa1->TrvItem[pVa1->TrvNItems - 1].TvCounterStyle = CntUppercase;
		       break;

		    case KWD_LOWERCASE:
		       pVa1 = &pSchTra->TsVariable[pSchTra->TsNVariables - 1];
		       pVa1->TrvItem[pVa1->TrvNItems - 1].TvCounterStyle = CntLowercase;
		       break;

		    case KWD_Alphabet:		/* Alphabet */
		       if (CurType != ord (CharString) + 1)
			  CompilerError (wi, TRA, FATAL, TRA_ONLY_FOR_TEXT_UNITS, inputLine, linenb);
		       else
			  CurBloc->TbCondition[CurBloc->TbNConditions - 1].TcCondition = TcondAlphabet;
		       break;

		    case KWD_FirstAttr:	/* FirstAttr */
		       CurBloc->TbCondition[CurBloc->TbNConditions - 1].TcCondition = TcondFirstAttr;
		       break;

		    case KWD_LastAttr:		/* LastAttr */
		       CurBloc->TbCondition[CurBloc->TbNConditions - 1].TcCondition = TcondLastAttr;
		       break;

		    case KWD_Create:	/* Create */
		       nouvregle ();
		       CurRegle->TrType = TCreate;
		       CurRegle->TrReferredObj = False;
		       CurRegle->TrObjectNature[0] = '\0';
		       CurRegle->TrFileNameVar = 0;
		       DefVar = True;
		       break;

		    case KWD_Get:	/* Get */
		       nouvregle ();
		       CurRegle->TrType = TGet;
		       CurRegle->TrElemNature[0] = '\0';
		       CurRegle->TrRelPosition = RpSibling;
		       break;

		    case KWD_Copy:	/* Copy */
		       nouvregle ();
		       CurRegle->TrType = TCopy;
		       CurRegle->TrElemNature[0] = '\0';
		       CurRegle->TrRelPosition = RpSibling;
		       break;

		    case KWD_Use:	/* Use */
		       DansCondition = False;
		       if (pSchStr->SsRule[CurType - 1].SrConstruct != CsNatureSchema
			   && CurType != pSchStr->SsRootElem)
			  /* l'element auquel s'applique la regle n'est pas une */
			  /* nature ni la regle racine, erreur */
			  CompilerError (wi, TRA, FATAL, TRA_ONLY_FOR_A_DIFFERENT_STRUCT_SCHEM, inputLine, linenb);
		       else
			 {
			    nouvregle ();
			    if (CurBloc->TbNConditions != 0)
			       CompilerError (wi, TRA, FATAL, TRA_USE_MUST_BE_UNCONDITIONAL, inputLine, linenb);
			    else
			      {
				 CurRegle->TrType = TUse;
				 strncpy (CurRegle->TrNature, pSchStr->SsRule[CurType - 1].SrName, MAX_NAME_LENGTH);
			      }
			 }
		       break;

		    case KWD_For:	/* For */
		       if (CurType != pSchStr->SsRootElem)
			  /* FOR n'est acceptable que pour l'element racine */
			  CompilerError (wi, TRA, FATAL, TRA_ONLY_FOR_THE_ROOT_ELEM, inputLine, linenb);
		       break;

		    case KWD_Remove:	/* Remove */
		       nouvregle ();
		       CurRegle->TrType = TRemove;
		       break;

		    case KWD_Write:	/* Write */
		       nouvregle ();
		       CurRegle->TrType = TWrite;
		       CurRegle->TrReferredObj = False;
		       CurRegle->TrObjectNature[0] = '\0';
		       CurRegle->TrFileNameVar = 0;
		       DefVar = True;
		       break;

		    case KWD_Read:	/* Read */
		       nouvregle ();
		       CurRegle->TrType = TRead;
		       break;

		    case KWD_Include:	/* Include */
		       nouvregle ();
		       CurRegle->TrType = TInclude;
		       break;

		    case KWD_NoTranslation:	/* NoTranslation */
		       nouvregle ();
		       CurRegle->TrType = TNoTranslation;
		       break;

		    case KWD_NoLineBreak:	/* NoLineBreak */
		       nouvregle ();
		       CurRegle->TrType = TNoLineBreak;
		       break;

		    case KWD_ChangeMainFile:	/* ChangeMainFile */
		       nouvregle ();
		       CurRegle->TrType = TChangeMainFile;
		       break;

		    case KWD_Content:	/* Content */
		       CurRegle->TrObject = ToContent;
		       break;

		    case KWD_After:	/* After */
		       CurRegle->TrOrder = TAfter;
		       break;

		    case KWD_Before:	/* Before */
		       CurRegle->TrOrder = TBefore;
		       break;

		    case KWD_Included:		/* Included */
		       CurRegle->TrRelPosition = RpDescend;
		       break;

		    case KWD_Size:
		       nomPres (PtSize, pr, wi);
		       break;

		    case KWD_Indent:
		       nomPres (PtIndent, pr, wi);
		       break;

		    case KWD_LineSpacing:
		       nomPres (PtLineSpacing, pr, wi);
		       break;

		    case KWD_Adjust:
		       nomPres (PtAdjust, pr, wi);
		       break;

		    case KWD_Justify:
		       nomPres (PtJustify, pr, wi);
		       break;

		    case KWD_Hyphenate:
		       nomPres (PtHyphenate, pr, wi);
		       break;

		    case KWD_Style:
		       nomPres (PtStyle, pr, wi);
		       break;

		    case KWD_Font:
		       nomPres (PtFont, pr, wi);
		       break;

		    case KWD_UnderLine:
		       if (r == RULE_PresRule)
			  nomPres (PtUnderline, pr, wi);
		       else if (r == RULE_UnderLineVal)
			  valPres ('U', wi);
		       break;

		    case KWD_Thickness:
		       nomPres (PtThickness, pr, wi);
		       break;

		    case KWD_LineStyle:
		       nomPres (PtLineStyle, pr, wi);
		       break;

		    case KWD_LineWeight:
		       nomPres (PtLineWeight, pr, wi);
		       break;

		    case KWD_FillPattern:
		       nomPres (PtFillPattern, pr, wi);
		       break;

		    case KWD_Background:
		       nomPres (PtBackground, pr, wi);
		       break;

		    case KWD_Foreground:
		       nomPres (PtForeground, pr, wi);
		       break;

		    case KWD_Left:
		       valPres ('L', wi);
		       break;

		    case KWD_Right:
		       valPres ('R', wi);
		       break;

		    case KWD_VMiddle:
		       valPres ('C', wi);
		       break;

		    case KWD_LeftWithDots:
		       valPres ('D', wi);
		       break;

		    case KWD_Yes:
		       valPres ('Y', wi);
		       break;

		    case KWD_No:
		       if (r == RULE_UnderLineVal)
			  valPres ('N', wi);
		       else
			  valPres ('N', wi);
		       break;

		    case KWD_Bold:
		       valPres ('B', wi);
		       break;

		    case KWD_Italics:
		       valPres ('I', wi);
		       break;

		    case KWD_Roman:
		       valPres ('R', wi);
		       break;

		    case KWD_BoldItalics:
		       valPres ('G', wi);
		       break;

		    case KWD_BoldOblique:
		       valPres ('Q', wi);
		       break;

		    case KWD_Oblique:
		       valPres ('O', wi);
		       break;

		    case KWD_Times:
		       valPres ('T', wi);
		       break;

		    case KWD_Helvetica:
		       valPres ('H', wi);
		       break;

		    case KWD_Courier:
		       valPres ('C', wi);
		       break;

		    case KWD_NoUnderline:
		       valPres ('N', wi);
		       break;

		    case KWD_Underlined:
		       valPres ('U', wi);
		       break;

		    case KWD_Overlined:
		       valPres ('O', wi);
		       break;

		    case KWD_CrossedOut:
		       valPres ('C', wi);
		       break;

		    case KWD_OverLine:
		       valPres ('O', wi);
		       break;

		    case KWD_StrikeOut:
		       valPres ('C', wi);
		       break;

		    case KWD_Thick:
		       valPres ('N', wi);
		       break;

		    case KWD_Thin:
		       valPres ('T', wi);
		       break;

		    case KWD_Solid:
		       valPres ('S', wi);
		       break;

		    case KWD_Dashed:
		       valPres ('-', wi);
		       break;

		    case KWD_Dotted:
		       valPres ('.', wi);
		       break;

		    case KWD_ComputedPage:
		       if (CurType != ord (PageBreak) + 1)
			  /* l'element auquel s'applique la regle n'est pas une marque de page */
			  /* erreur */
			  CompilerError (wi, TRA, FATAL, TRA_ONLY_FOR_PAGES, inputLine, linenb);
		       else
			  CurBloc->TbCondition[CurBloc->TbNConditions - 1].TcCondition = TcondComputedPage;
		       break;

		    case KWD_StartPage:
		       if (CurType != ord (PageBreak) + 1)
			  /* l'element auquel s'applique la regle n'est pas une marque de page */
			  /* erreur */
			  CompilerError (wi, TRA, FATAL, TRA_ONLY_FOR_PAGES, inputLine, linenb);
		       else
			  CurBloc->TbCondition[CurBloc->TbNConditions - 1].TcCondition = TcondStartPage;
		       break;

		    case KWD_UserPage:
		       if (CurType != ord (PageBreak) + 1)
			  /* l'element auquel s'applique la regle n'est pas une marque de page */
			  /* erreur */
			  CompilerError (wi, TRA, FATAL, TRA_ONLY_FOR_PAGES, inputLine, linenb);
		       else
			  CurBloc->TbCondition[CurBloc->TbNConditions - 1].TcCondition = TcondUserPage;
		       break;

		    case KWD_ReminderPage:
		       if (CurType != ord (PageBreak) + 1)
			  /* l'element auquel s'applique la regle n'est pas une marque de page */
			  /* erreur */
			  CompilerError (wi, TRA, FATAL, TRA_ONLY_FOR_PAGES, inputLine, linenb);
		       else
			  CurBloc->TbCondition[CurBloc->TbNConditions - 1].TcCondition = TcondReminderPage;
		       break;
		    case KWD_Empty:
		       CurBloc->TbCondition[CurBloc->TbNConditions - 1].TcCondition = TcondEmpty;
		       break;
		    case KWD_Parent:
		       CurBloc->TbCondition[CurBloc->TbNConditions - 1].TcAscendRelLevel = 1;
		       break;
		    case KWD_Ancestor:
		       /* rien a faire */
		       break;
		    default:
		       break;
		 }

	else			/* type de base */
	   switch (c)
		 {
		    case 3001 /* un nom */ :
		       switch (r)
			     {
				   /* r= numero regle */
				case RULE_TypeIdent:	/* TypeIdent */
				   if (pr == RULE_TransSchema)
				      /* nom de la structure generique a laquelle se rapporte */
				      /* le schema de traduction */
				     {
					copy (pSchTra->TsStructName, wi, wl);
					/* lit le schema de structure compile' */
					if (!RdSchStruct (pSchTra->TsStructName, pSchStr))
					   TtaDisplaySimpleMessage (TRA, FATAL, TRA_CANNOT_READ_STRUCT_SCHEM);	/* echec lecture du  schema de structure */
					else if (strcmp (pSchTra->TsStructName, pSchStr->SsName) != 0)
					   CompilerError (wi, TRA, FATAL, TRA_STRUCT_SCHEM_DOES_NOT_MATCH, inputLine, linenb);
					else
					  {
					     pSchTra->TsStructCode = pSchStr->SsCode;
					     initAttr ();
					  }
				     }
				   else if (pr == RULE_CondOnAscend)
				      /* dans une condition Within ou FirstWithin */
				     {
					if (!SchemaExt)
					   /* nom du type d'element dans la condition Within ou */
					   /* FirstWithin */
					  {
					     copy (NomWithin, wi, wl);
					     DebutNomWithin = wi;
					  }
					else
					   /* nom d'un schema de structure externe dans la condition */
					   /* Within ou FirstWithin */
					  {
					     SchemaExt = False;
					     /* recupere dans n le nom du schema externe */
					     copy (n, wi, wl);
					     /* lit le schema de structure externe */
					     if (!RdSchStruct (n, pSchStrExt))
						CompilerError (wi, TRA, FATAL, TRA_CANNOT_READ_STRUCT_SCHEM, inputLine, linenb);
					     /* echec lecture du schema */
					     else
						/* le schema de structure a ete charge' */
					       {
						  strncpy (CurBloc->TbCondition[CurBloc->TbNConditions - 1].TcElemNature, n, MAX_NAME_LENGTH);
						  traiteFinCondition (pSchStrExt);
					       }
					  }
				     }
				   else if (pr == RULE_LevelOrType)
				      /* un type d'element dans une condition portant sur */
				      /* un ascendant */
				     {
					if (!SchemaExt)
					   /* nom du type d'element ascendant dans la condition */
					  {
					     copy (NomAscend, wi, wl);
					     DebutNomAscend = wi;
					  }
					else
					   /* nom d'un schema de structure externe dans Ascend */
					  {
					     SchemaExt = False;
					     /* recupere dans n le nom du schema externe */
					     copy (n, wi, wl);
					     /* lit le schema de structure externe */
					     if (!RdSchStruct (n, pSchStrExt))
						CompilerError (wi, TRA, FATAL, TRA_CANNOT_READ_STRUCT_SCHEM, inputLine, linenb);
					     /* echec lecture du schema */
					     else
						/* le schema de    structure a ete charge' */
					       {
						  strncpy (CurBloc->TbCondition[CurBloc->TbNConditions - 1].TcAscendNature, n, MAX_NAME_LENGTH);
						  traiteFinElemConcerne (pSchStrExt);
					       }
					  }
				     }
				   else
				     {
					DansRegleUse = False;
					DansRegleGet = False;
					DansRegleCreateWrite = (pr == RULE_Token);
					if (pr == RULE_Rule1)
					  {
					     if (CurRegle->TrType == TUse)
						DansRegleUse = True;
					     if (CurRegle->TrType == TGet || CurRegle->TrType == TCopy)
						DansRegleGet = True;
					  }
					if (DansRegleUse)
					   copy (CurRegle->TrNature, wi, wl);
					else if (DansRegleGet || DansRegleCreateWrite)
					   if (SchemaExt)
					      /* nom d'un schema de structure externe */
					     {
						SchemaExt = False;
						copy (n, wi, wl);
						/* recupere dans n le nom du schema externe */
						/* lit le schema de structure externe */
						if (!RdSchStruct (n, pSchStrExt))
						   CompilerError (wi, TRA, FATAL, TRA_CANNOT_READ_STRUCT_SCHEM, inputLine, linenb);
						/* echec lecture du schema */
						else
						   /* le schema de    structure a ete charge' */
						  {
						     if (DansRegleGet)
							strncpy (CurRegle->TrElemNature, n, MAX_NAME_LENGTH);
						     else
							strncpy (CurRegle->TrObjectNature, n, MAX_NAME_LENGTH);
						     traiteTypeIdent (pSchStrExt);
						  }
					     }
					   else
					     {
						copy (NomGet, wi, wl);
						DebutNomGet = wi;
					     }
					else if (pr == RULE_VarOrType)
					   /* dans une regle Create ou Write, apres Refered */
					  {
					     DefVar = False;
					     copy (NomGet, wi, wl);
					     DebutNomGet = wi;
					  }
					else
					  {
					     i = NumType (wi, wl);
					     if (i > 0)
						/* le type existe, il a le numero i */
						if (ComptDcl)	/* dans une declaration de compteur */
						  {
						     pTrCo1 = &pSchTra->TsCounter[pSchTra->TsNCounters - 1];
						     if (pTrCo1->TnOperation == TCntrSet)
							if (pTrCo1->TnElemType1 == 0)
							   pTrCo1->TnElemType1 = i;
							else
							   pTrCo1->TnElemType2 = i;
						     else
							pTrCo1->TnElemType1 = i;
						  }
						else
						   /* on n'est pas dans une declaration de compteur */
						if (pr == RULE_TransType)
						   /* debut des regles associees a un type */
						  {
						     if (pSchStr->SsRule[i - 1].SrConstruct == CsPairedElement)
							/* c'est un element CsPairedElement */
							if (!PaireSecond && !PairePremier)
							   /* le nom du type n'etait pas precede' de First ou Second */
							   CompilerError (wi, TRA, FATAL, TRA_MISSING_FIRST_SECOND,
									  inputLine, linenb);
							else
							  {
							     if (PaireSecond)
								/* il s'agit du type suivant */
								i++;
							  }
						     else
							/* ce n'est pas un element CsPairedElement */
						     if (PaireSecond || PairePremier)
							/* le nom du type etait precede' de First ou Second, erreur */
							CompilerError (wi, TRA, FATAL, TRA_NOT_A_PAIR,
							 inputLine, linenb);
						     if (pSchTra->TsElemTRule[i - 1] != NULL)
							CompilerError (wi, TRA, FATAL, TRA_ALREADY_DEFINED,
							 inputLine, linenb);
						     else
						       {
							  CurType = i;
							  CurBloc = NULL;
							  CurRegle = NULL;
						       }
						     PairePremier = False;
						     PaireSecond = False;
						  }

						else if (pr == RULE_TransAttr)
						   /* apres un nom d'attribut */
						  {
						     pSchTra->TsInheritAttr[i - 1] = True;
						     pSchTra->TsAttrTRule[CurAttr - 1].AtrElemType = i;
						  }
					  }
				     }
				   break;

				case RULE_ConstIdent:	/* ConstIdent */
				   if (ConstDcl)	/* une definition de constante */
				      if (identtable[nb - 1].identdef != 0)
					 /* nom deja declare' */
					 CompilerError (wi, TRA, FATAL, TRA_NAME_ALREADY_DECLARED, inputLine, linenb);
				      else if (pSchTra->TsNConstants >= MAX_TRANSL_CONST)
					 /* table des constantes saturee */
					 CompilerError (wi, TRA, FATAL, TRA_TOO_MANY_CONSTANTS, inputLine, linenb);
				      else
					 /* alloue un nouvelle entree dans la table des constantes */
					{
					   pSchTra->TsNConstants++;
					   identtable[nb - 1].identdef = pSchTra->TsNConstants;
					}
				   else
				      /* utilisation d'une constante */
				   if (identtable[nb - 1].identdef == 0)
				      /* ce nom n'a pas ete declare comme un identificateur */
				      /* de constante */
				      if (pr == RULE_Token || pr == RULE_Function)
					 /* s'il vient de la regle Token ou Function, voyons si */
					 /* ce n'est pas un identificateur d'attribut */
					{
					   /* copie dans n l'identificateur */
					   copy (n, wi, wl);
					   /* verifie si l'attribut existe bien dans le schema */
					   /* de structure */
					   i = 1;
					   while (strcmp (n, pSchStr->SsAttribute[i - 1].AttrName) != 0
						&& i < pSchStr->SsNAttributes)
					      i++;
					   if (strcmp (n, pSchStr->SsAttribute[i - 1].AttrName) != 0)
					      /* attribut inconnu */
					      CompilerError (wi, TRA, FATAL, TRA_UNKNOWN_ATTR, inputLine, linenb);
					   else
					      /* l'attribut existe, il a le numero i */
					     {
						CrWrAttr (i, pr, wi);
						/* ce nom est maintenant un nom d'attribut */
						identtable[nb - 1].identtype = RULE_AttrIdent;
					     }
					}
				      else
					 /* constante non definie */
					 CompilerError (wi, TRA, FATAL, TRA_CONSTANT_NOT_DECLARED, inputLine, linenb);
				   else if (VarDcl)	/* dans une declaration de variable */
				     {
					pVa1 = &pSchTra->TsVariable[pSchTra->TsNVariables - 1];
					if (pVa1->TrvNItems >= MAX_TRANSL_VAR_ITEM)
					   /* trop de fonctions */
					   CompilerError (wi, TRA, FATAL, TRA_TOO_MANY_ELEMS_IN_THE_VARIABLE, inputLine, linenb);
					else
					  {
					     pVa1->TrvNItems++;
					     pVa1->TrvItem[pVa1->TrvNItems - 1].TvType =
						VtText;
					     pVa1->TrvItem[pVa1->TrvNItems - 1].TvItem =
						identtable[nb - 1].identdef;
					  }
				     }
				   else if (pr == RULE_Token)
				      /* dans une regle Create ou Write */
				     {
					CurRegle->TrObject = ToConst;
					CurRegle->TrObjectNum = identtable[nb - 1].identdef;
				     }
				   break;

				case RULE_CounterIdent:	/* CounterIdent */
				   if (ComptDcl)	/* une definition de compteur */
				      if (identtable[nb - 1].identdef != 0)
					 /* nom deja declare' */
					 CompilerError (wi, TRA, FATAL, TRA_NAME_ALREADY_DECLARED, inputLine, linenb);
				      else if (pSchTra->TsNCounters >= MAX_TRANSL_COUNTER)
					 /* table des compteurs saturee */
					 CompilerError (wi, TRA, FATAL, TRA_TOO_MANY_COUNTERS, inputLine, linenb);
				      else
					 /* alloue un nouvelle entree dans la table des compteurs */
					{
					   pSchTra->TsNCounters++;
					   identtable[nb - 1].identdef = pSchTra->TsNCounters;
					   /* a priori ce compteur n'a pas d'operations */
					   pSchTra->TsCounter[pSchTra->TsNCounters - 1].TnOperation = TCntrNoOp;
					   /* a priori, ce compteur ne sera pas */
					   /* intialise' par un attribut */
					   pSchTra->TsCounter[pSchTra->TsNCounters - 1].
					      TnAttrInit = 0;
					}
				   else
				      /* utilisation d'un compteur */
				   if (identtable[nb - 1].identdef == 0)
				      /* compteur non defini */
				      CompilerError (wi, TRA, FATAL, TRA_COUNTER_NOT_DECLARED, inputLine, linenb);
				   else if (pr == RULE_Function)
				      /* dans une declaration de variable */
				     {
					pVa1 = &pSchTra->TsVariable[pSchTra->TsNVariables - 1];
					pVa1->TrvItem[pVa1->TrvNItems - 1].TvItem =
					   identtable[nb - 1].identdef;
					pVa1->TrvItem[pVa1->TrvNItems - 1].TvLength = 0;
					pVa1->TrvItem[pVa1->TrvNItems - 1].TvCounterStyle = CntArabic;
				     }
				   else if (pr == RULE_Rule1)
				      /* un compteur dans une instruction Set ou Add */
				      if (pSchTra->TsCounter[identtable[nb - 1].identdef - 1].TnOperation != TCntrNoOp)
					 CompilerError (wi, TRA, FATAL, TRA_INVALID_COUNTER, inputLine, linenb);
				      else
					 CurRegle->TrCounterNum = identtable[nb - 1].identdef;
				   break;

				case RULE_BufferIdent:		/* BufferIdent */
				   if (BuffDcl)		/* une definition de  buffer */
				      if (identtable[nb - 1].identdef != 0)
					 /* nom deja declare' */
					 CompilerError (wi, TRA, FATAL, TRA_NAME_ALREADY_DECLARED, inputLine, linenb);
				      else if (pSchTra->TsNBuffers >= MAX_TRANSL_BUFFER)
					 /* table des buffers saturee */
					 CompilerError (wi, TRA, FATAL, TRA_TOO_MANY_BUFFERS, inputLine, linenb);
				      else
					{
					   pSchTra->TsNBuffers++;
					   identtable[nb - 1].identdef = pSchTra->TsNBuffers;
					}
				   else
				      /* utilisation d'un buffer */
				   if (identtable[nb - 1].identdef == 0)
				      /* buffer non defini */
				      CompilerError (wi, TRA, FATAL, TRA_BUFFER_NOT_DECLARED, inputLine, linenb);
				   else if (VarDcl)
				      /* dans une declaration de variable */
				     {
					pVa1 = &pSchTra->TsVariable[pSchTra->TsNVariables - 1];
					if (pVa1->TrvNItems >= MAX_TRANSL_VAR_ITEM)
					   /* trop de fonctions */
					   CompilerError (wi, TRA, FATAL, TRA_TOO_MANY_ELEMS_IN_THE_VARIABLE, inputLine, linenb);
					else
					  {
					     pVa1->TrvNItems++;
					     pVa1->TrvItem[pVa1->TrvNItems - 1].TvType = VtBuffer;
					     pVa1->TrvItem[pVa1->TrvNItems - 1].TvItem =
						identtable[nb - 1].identdef;
					  }
				     }
				   else if (pr == RULE_Rule1)
				      /* dans une regle Read */
				      CurRegle->TrBuffer = identtable[nb - 1].identdef;
				   else if (pr == RULE_Token)

				      /* dans une regle Create ou Write */
				     {
					CurRegle->TrObject = ToBuffer;
					CurRegle->TrObjectNum = identtable[nb - 1].identdef;
				     }
				   else if (pr == RULE_File)
				     {
					/* dans une regle Include */
					CurRegle->TrBufOrConst = ToBuffer;
					CurRegle->TrInclFile = identtable[nb - 1].identdef;
				     }
				   break;

				case RULE_VariableIdent:	/* VariableIdent */
				   if (VarDcl)
				      /* une definition de  variable */
				      if (identtable[nb - 1].identdef != 0)
					 /* nom deja declare' */
					 CompilerError (wi, TRA, FATAL, TRA_NAME_ALREADY_DECLARED, inputLine, linenb);
				      else if (pSchTra->TsNVariables >= MAX_TRANSL_VARIABLE)
					 /* table des variables saturee */
					 CompilerError (wi, TRA, FATAL, TRA_TOO_MANY_VARIABLES, inputLine, linenb);
				      else
					{
					   pSchTra->TsNVariables++;
					   identtable[nb - 1].identdef = pSchTra->TsNVariables;
					   pSchTra->TsVariable[pSchTra->TsNVariables - 1].TrvNItems = 0;
					}
				   else if (pr == RULE_Token)
				      /* dans une regle Create  ou Write */
				      if (identtable[nb - 1].identdef == 0)
					 /* variable non definie */
					 CompilerError (wi, TRA, FATAL, TRA_VARIABLE_NOT_DECLARED, inputLine, linenb);
				      else
					{
					   CurRegle->TrObject = ToVariable;
					   CurRegle->TrObjectNum = identtable[nb - 1].identdef;
					}
				   else if (pr == RULE_VarOrType)
				      /* dans une regle Create  ou Write */
				     {
					DefVar = False;
					if (identtable[nb - 1].identdef == 0)
					   /* variable non definie */
					   /* c'est peut-etre un nom de type d'element */
					  {
					     CurRegle->TrObject = ToReferredElem;
					     copy (NomGet, wi, wl);
					     DebutNomGet = wi;
					  }
					else
					   /* la variable est bien definie */
					  {
					     CurRegle->TrObject = ToVariable;
					     CurRegle->TrObjectNum = identtable[nb - 1].identdef;
					  }
				     }
				   else if (pr == RULE_Rule1)
				      if (identtable[nb - 1].identdef == 0)
					 /* variable non definie */
					 CompilerError (wi, TRA, FATAL, TRA_VARIABLE_NOT_DECLARED, inputLine, linenb);
				      else if (CurRegle->TrType == TCreate)
					 /* indication du fichier de sortie dans une regle Create */
					 CurRegle->TrFileNameVar = identtable[nb - 1].identdef;
				      else if (CurRegle->TrType == TChangeMainFile)
					 CurRegle->TrNewFileVar = identtable[nb - 1].identdef;

				   break;

				case RULE_AttrIdent:	/* AttrIdent */
				   /* copie dans n l'identificateur de l'attribut */
				   copy (n, wi, wl);
				   /* verifie si l'attribut existe bien dans le schema de */
				   /* structure */
				   i = 1;
				   while (strcmp (n, pSchStr->SsAttribute[i - 1].AttrName) != 0
					  && i < pSchStr->SsNAttributes)
				      i++;
				   if (strcmp (n, pSchStr->SsAttribute[i - 1].AttrName) != 0)
				      /* attribut inconnu */
				      CompilerError (wi, TRA, FATAL, TRA_UNKNOWN_ATTR, inputLine, linenb);
				   else
				      /* l'attribut existe, il a le numero i */
				   if (pr == RULE_TransAttr)
				      /* c'est un nom d'attribut auquel on va associer des */
				      /* regles de traduction */
				     {
					CurAttr = i;
					CurValAttr = 0;		/* pas encore rencontre' de valeur */
					CurBloc = NULL;
					CurRegle = NULL;
					pRT1 = &pSchTra->TsAttrTRule[CurAttr - 1];
					switch (pSchStr->SsAttribute[CurAttr - 1].AttrType)
					      {
						 case AtNumAttr:	/* attribut a valeur numerique */
						    if (pRT1->AtrNCases >= MAX_TRANSL_ATTR_CASE)
						       /* trop de cas pour cet attribut */
						       CompilerError (wi, TRA, FATAL, TRA_TOO_MANY_CASES_FOR_THAT_ATTR, inputLine, linenb);
						    else
						       pRT1->AtrNCases++;
						    break;

						 case AtTextAttr /* attribut textuel */ :
						    if (pRT1->AtrTxtTRuleBlock != NULL)
						       /* attribut deja rencontre' */
						       CompilerError (wi, TRA, FATAL, TRA_RULES_ALREADY_EXIST_FOR_THAT_ATTR, inputLine, linenb);
						    break;

						 case AtReferenceAttr:
						    if (pRT1->AtrRefTRuleBlock != NULL)
						       /* attribut deja rencontre' */
						       CompilerError (wi, TRA, FATAL, TRA_RULES_ALREADY_EXIST_FOR_THAT_ATTR, inputLine, linenb);
						    break;

						 case AtEnumAttr:

						    break;

						 default:
						    break;
					      }

				     }
				   else if (pr == RULE_CondOnAscend)
				      /* un nom d'attribut dans une condition */
				     {
					traiteFinElemConcerne (pSchStr);
					if (!Asterisk)
					   CurBloc->TbCondition[CurBloc->TbNConditions - 1].TcAscendRelLevel = -1;
					CurBloc->TbCondition[CurBloc->TbNConditions - 1].TcCondition = TcondAttr;
					CurBloc->TbCondition[CurBloc->TbNConditions - 1].TcAttr = i;
					switch (pSchStr->SsAttribute[i - 1].AttrType)
					      {
						 case AtNumAttr:
						    CurBloc->TbCondition[CurBloc->TbNConditions - 1].TcLowerBound = -MAX_INT_ATTR_VAL - 1;
						    /* -infini */
						    CurBloc->TbCondition[CurBloc->TbNConditions - 1].TcUpperBound = MAX_INT_ATTR_VAL + 1;
						    /* +infini */
						    break;
						 case AtTextAttr:
						    CurBloc->TbCondition[CurBloc->TbNConditions - 1].TcTextValue[0] = '\0';
						    break;
						 case AtReferenceAttr:

						    break;	/* rien */
						 case AtEnumAttr:
						    CurBloc->TbCondition[CurBloc->TbNConditions - 1].TcAttrValue = 0;
						    break;
						 default:
						    break;
					      }

				     }
				   else if (pr == RULE_Token ||
					    pr == RULE_CountFunction ||
					    pr == RULE_Function)
				      CrWrAttr (i, pr, wi);
				   break;

				case RULE_AttrValIdent:	/* AttrValIdent */
				   copy (n, wi, wl);
				   /* verifie si cette valeur existe pour l'attribut courant */
				   if (DansCondition)
				      k = CurBloc->TbCondition[CurBloc->TbNConditions - 1].TcAttr;
				   else
				      k = CurAttr;
				   pAt1 = &pSchStr->SsAttribute[k - 1];
				   if (pAt1->AttrType != AtEnumAttr)
				      /* pas un attribut a valeur enumerees */
				      CompilerError (wi, TRA, FATAL, TRA_INCOR_ATTR_VALUE, inputLine, linenb);
				   else
				     {
					i = 1;
					while (strcmp (n, pAt1->AttrEnumValue[i - 1]) != 0
					       && i < pAt1->AttrNEnumValues)
					   i++;
					if (strcmp (n, pAt1->AttrEnumValue[i - 1]) != 0)
					   /* valeur d'attribut  incorrecte */
					   CompilerError (wi, TRA, FATAL, TRA_INCOR_ATTR_VALUE, inputLine, linenb);
					else
					   /* la valeur est correcte, elle a le numero i */
					if (!DansCondition)
					   /* debut des regles de traduction d'un attribut */
					   if (pSchTra->TsAttrTRule[CurAttr - 1].AtrEnuTRuleBlock[i] != NULL)
					      /* deja des regles pour cette valeur */
					      CompilerError (wi, TRA, FATAL, TRA_RULES_ALREADY_EXIST_FOR_THAT_VALUE, inputLine, linenb);
					   else
					      CurValAttr = i;
					else
					   /* un nom d'attribut dans une condition */
					   CurBloc->TbCondition[CurBloc->TbNConditions - 1].TcAttrValue = i;
				     }
				   break;

				case RULE_Alphabet:	/* Alphabet */
				   if (pr == RULE_TSeqOfTransl)
				      /* Deja des regles de traduction pour cet alphabet ? */
				     {
					for (i = 0; i < pSchTra->TsNTranslAlphabets - 1; i++)
					   if (pSchTra->TsTranslAlphabet[i].AlAlphabet == inputLine[wi - 1])
					      CompilerError (wi, TRA, FATAL, TRA_ALPHABET_ALREADY_DEFINED, inputLine, linenb);
					if (!error)
					   pSchTra->TsTranslAlphabet[pSchTra->TsNTranslAlphabets - 1].
					      AlAlphabet = inputLine[wi - 1];
				     }
				   else if (pr == RULE_CondOnSelf)
				      /* dans une condition */
				      CurBloc->TbCondition[CurBloc->TbNConditions - 1].
					 TcAlphabet = inputLine[wi - 1];
				   break;

				case RULE_TrSchema:	/* TrSchema */
				   copy (CurRegle->TrTranslSchemaName, wi, wl);
				   break;
				case RULE_Pattern:	/* Pattern */
				case RULE_Color:	/* Color */
				   copy (n, wi, wl);
				   /* cherche le numero de cette trame ou de cette couleur */
				   if (r == RULE_Pattern)
				      k = NumeroPattern (n, wi);
				   else
				      k = NumeroCouleur (n, wi);
				   if (DansCondition)
				     {
					CurBloc->TbCondition[CurBloc->TbNConditions - 1].TcUpperBound = k;
					CurBloc->TbCondition[CurBloc->TbNConditions - 1].TcLowerBound = k;
				     }
				   else
				     {
					pRTP1 = &pSchTra->TsPresTRule[CurPres - 1];
					pTC1 = &pRTP1->RtCase[pRTP1->RtNCase - 1];
					pTC1->TaUpperBound = k;
					pTC1->TaLowerBound = k;
				     }
				   break;
				default:
				   break;
			     }
		       break;

		    case 3002:	/* un nombre */
		       k = trnb (wi, wl);
		       switch (r)	/* r= numero de regle */
			     {
				case RULE_MaxLineLength:	/* MaxLineLength */
				   pSchTra->TsLineLength = k;
				   break;
				case RULE_AncestorLevel:	/* AncestorLevel */
				   pSchTra->TsCounter[pSchTra->TsNCounters - 1].TnAcestorLevel =
				      k * SigneAncetre;
				   break;
				case RULE_RelLevel:	/* RelLevel */
				   CurBloc->TbCondition[CurBloc->TbNConditions - 1].TcAscendLevel = k;
				   if (!SignGreaterOrLess)
				      CurBloc->TbCondition[CurBloc->TbNConditions - 1].TcAscendRel = RelEquals;
				   break;
				case RULE_CondRelLevel:	/* CondRelLevel */
				   CurBloc->TbCondition[CurBloc->TbNConditions - 1].TcAscendRelLevel = k;
				   break;
				case RULE_InitValue:	/* InitValue */
				   if (pr == RULE_CountFunction)
				      /* dans une definition de compteur */
				      pSchTra->TsCounter[pSchTra->TsNCounters - 1].TnParam1 = k;
				   else if (pr == RULE_Rule1)
				      /* dans une instruction de traduction Set */
				      CurRegle->TrCounterParam = k;
				   break;

				case RULE_IncrValue:	/* IncrValue */
				   if (pr == RULE_CountFunction)
				      /* dans une definition de compteur */
				      pSchTra->TsCounter[pSchTra->TsNCounters - 1].TnParam2 = k;
				   else if (pr == RULE_Rule1)
				      /* dans une instruction de traduction Add */
				      CurRegle->TrCounterParam = k;
				   break;
				case RULE_Length:	/* Length */
				   pVa1 = &pSchTra->TsVariable[pSchTra->TsNVariables - 1];
				   pVa1->TrvItem[pVa1->TrvNItems - 1].TvLength = k;
				   break;

				case RULE_MinVal:	/* MinVal */
				   if (DansCondition)
				      i = CurBloc->TbCondition[CurBloc->TbNConditions - 1].TcAttr;
				   else
				      i = CurAttr;
				   if (pSchStr->SsAttribute[i - 1].AttrType != AtNumAttr
				       || k >= MAX_INT_ATTR_VAL)
				      /* ce n'est pas un attribut numerique */
				      CompilerError (wi, TRA, FATAL, TRA_NOT_A_NUMERICAL_ATTR, inputLine, linenb);
				   else
				     {
					k = k * SigneAttrVal + 1;
					SigneAttrVal = 1;
					/* a priori, la prochaine valeur */
					/* d'attribut numerique sera positive */
					if (DansCondition)
					   CurBloc->TbCondition[CurBloc->TbNConditions - 1].TcLowerBound = k;
					else
					  {
					     pRT1 = &pSchTra->TsAttrTRule[CurAttr - 1];
					     pRT1->AtrCase[pRT1->AtrNCases - 1].TaLowerBound = k;
					  }
				     }
				   break;

				case RULE_MaxVal:	/* MaxVal */
				   if (DansCondition)
				      i = CurBloc->TbCondition[CurBloc->TbNConditions - 1].TcAttr;
				   else
				      i = CurAttr;
				   if (pSchStr->SsAttribute[i - 1].AttrType != AtNumAttr
				       || k >= MAX_INT_ATTR_VAL)
				      /* ce n'est pas un attribut numerique */
				      CompilerError (wi, TRA, FATAL, TRA_NOT_A_NUMERICAL_ATTR, inputLine, linenb);
				   else
				     {
					k = k * SigneAttrVal - 1;
					SigneAttrVal = 1;
					/* a priori, la prochaine valeur */
					/* d'attribut numerique sera positive */
					if (DansCondition)
					   CurBloc->TbCondition[CurBloc->TbNConditions - 1].TcUpperBound = k;
					else
					  {
					     pRT1 = &pSchTra->TsAttrTRule[CurAttr - 1];
					     pRT1->AtrCase[pRT1->AtrNCases - 1].TaUpperBound = k;
					  }
				     }
				   break;

				case RULE_MinInterval:		/* MinInterval */
				   if (DansCondition)
				      i = CurBloc->TbCondition[CurBloc->TbNConditions - 1].TcAttr;
				   else
				      i = CurAttr;
				   if (pSchStr->SsAttribute[i - 1].AttrType != AtNumAttr
				       || k >= MAX_INT_ATTR_VAL)
				      /* ce n'est pas un attribut numerique */
				      CompilerError (wi, TRA, FATAL, TRA_NOT_A_NUMERICAL_ATTR, inputLine, linenb);
				   else
				     {
					k = k * SigneAttrVal;
					SigneAttrVal = 1;
					/* a priori, la prochaine valeur */
					/* d'attribut numerique sera positive */
					if (DansCondition)
					   CurBloc->TbCondition[CurBloc->TbNConditions - 1].TcLowerBound = k;
					else
					  {
					     pRT1 = &pSchTra->TsAttrTRule[CurAttr - 1];
					     pRT1->AtrCase[pRT1->AtrNCases - 1].TaLowerBound = k;
					  }
				     }
				   break;

				case RULE_MaxInterval:		/* MaxInterval */
				   if (DansCondition)
				      i = CurBloc->TbCondition[CurBloc->TbNConditions - 1].TcAttr;
				   else
				      i = CurAttr;
				   if (pSchStr->SsAttribute[i - 1].AttrType != AtNumAttr
				       || k >= MAX_INT_ATTR_VAL)
				      /* ce n'est pas un attribut numerique */
				      CompilerError (wi, TRA, FATAL, TRA_NOT_A_NUMERICAL_ATTR, inputLine, linenb);
				   else
				     {
					k = k * SigneAttrVal;
					SigneAttrVal = 1;
					/* a priori, la prochaine valeur */
					/* d'attribut numerique sera positive */
					if (DansCondition)
					   if (CurBloc->TbCondition[CurBloc->TbNConditions - 1].TcLowerBound > k)
					      CompilerError (wi, TRA, FATAL, TRA_INCONSISTENT_LIMITS, inputLine, linenb);
					   else
					      CurBloc->TbCondition[CurBloc->TbNConditions - 1].TcUpperBound = k;
					else
					  {
					     pRT1 = &pSchTra->TsAttrTRule[CurAttr - 1];
					     if (pRT1->AtrCase[pRT1->AtrNCases - 1].TaLowerBound > k)
						CompilerError (wi, TRA, FATAL, TRA_INCONSISTENT_LIMITS, inputLine, linenb);
					     else
						pRT1->AtrCase[pRT1->AtrNCases - 1].TaUpperBound = k;
					  }
				     }
				   break;

				case RULE_ValEqual:	/* ValEqual */
				   if (DansCondition)
				      i = CurBloc->TbCondition[CurBloc->TbNConditions - 1].TcAttr;
				   else
				      i = CurAttr;
				   if (pSchStr->SsAttribute[i - 1].AttrType != AtNumAttr || k >= MAX_INT_ATTR_VAL)
				      /* ce n'est pas un attribut numerique */
				      CompilerError (wi, TRA, FATAL, TRA_INCONSISTENT_LIMITS, inputLine, linenb);
				   else
				     {
					k = k * SigneAttrVal;
					SigneAttrVal = 1;
					/* a priori, la prochaine valeur */
					/* d'attribut numerique sera positive */
					if (DansCondition)
					  {
					     CurBloc->TbCondition[CurBloc->TbNConditions - 1].TcUpperBound = k;
					     CurBloc->TbCondition[CurBloc->TbNConditions - 1].TcLowerBound = k;
					  }
					else
					  {
					     pRT1 = &pSchTra->TsAttrTRule[CurAttr - 1];
					     pTC1 = &pRT1->AtrCase[pRT1->AtrNCases - 1];
					     pTC1->TaUpperBound = k;
					     pTC1->TaLowerBound = pTC1->TaUpperBound;
					  }
				     }
				   break;

				case RULE_MinimumPres:
				   k = k * SignePresVal + 1;
				   SignePresVal = 1;
				   /* a priori, la prochaine valeur */
				   /* d'attribut numerique sera positive */
				   if (DansCondition)
				      CurBloc->TbCondition[CurBloc->TbNConditions - 1].TcLowerBound = k;
				   else
				     {
					pRTP1 = &pSchTra->TsPresTRule[CurPres - 1];
					pRTP1->RtCase[pRTP1->RtNCase - 1].TaLowerBound = k;
				     }
				   break;

				case RULE_MaximumPres:
				   k = k * SignePresVal - 1;
				   SignePresVal = 1;
				   /* a priori, la prochaine valeur */
				   /* d'attribut numerique sera positive */
				   if (DansCondition)
				      CurBloc->TbCondition[CurBloc->TbNConditions - 1].TcUpperBound = k;
				   else
				     {
					pRTP1 = &pSchTra->TsPresTRule[CurPres - 1];
					pRTP1->RtCase[pRTP1->RtNCase - 1].TaUpperBound = k;
				     }
				   break;

				case RULE_MinIntervalPres:
				   k = k * SignePresVal;
				   SignePresVal = 1;
				   /* a priori, la prochaine valeur */
				   /* d'attribut numerique sera positive */
				   if (DansCondition)
				      CurBloc->TbCondition[CurBloc->TbNConditions - 1].TcLowerBound = k;
				   else
				     {
					pRTP1 = &pSchTra->TsPresTRule[CurPres - 1];
					pRTP1->RtCase[pRTP1->RtNCase - 1].TaLowerBound = k;
				     }
				   break;

				case RULE_MaxIntervalPres:
				   k = k * SignePresVal;
				   SignePresVal = 1;
				   /* a priori, la prochaine valeur */
				   /* d'attribut numerique sera positive */
				   if (DansCondition)
				      if (CurBloc->TbCondition[CurBloc->TbNConditions - 1].TcLowerBound > k)
					 /* BorneInf > BorneSup !! */
					 CompilerError (wi, TRA, FATAL, TRA_INCONSISTENT_LIMITS, inputLine, linenb);
				      else
					 CurBloc->TbCondition[CurBloc->TbNConditions - 1].TcUpperBound = k;
				   else
				     {
					pRTP1 = &pSchTra->TsPresTRule[CurPres - 1];
					if (pRTP1->RtCase[pRTP1->RtNCase - 1].TaLowerBound > k)
					   CompilerError (wi, TRA, FATAL, TRA_INCONSISTENT_LIMITS, inputLine, linenb);
					else
					   pRTP1->RtCase[pRTP1->RtNCase - 1].TaUpperBound = k;
				     }
				   break;

				case RULE_PresVal:
				   k = k * SignePresVal;
				   SignePresVal = 1;
				   /* a priori, la prochaine valeur */
				   /* d'attribut numerique sera positive */
				   if (DansCondition)
				     {
					CurBloc->TbCondition[CurBloc->TbNConditions - 1].TcUpperBound = k;
					CurBloc->TbCondition[CurBloc->TbNConditions - 1].TcLowerBound = k;
				     }
				   else
				     {
					pRTP1 = &pSchTra->TsPresTRule[CurPres - 1];
					pTC1 = &pRTP1->RtCase[pRTP1->RtNCase - 1];
					pTC1->TaUpperBound = k;
					pTC1->TaLowerBound = k;
				     }
				   break;

				default:
				   break;
			     }
		       break;

		    case 3003:	/* une chaine de caracteres */
		       switch (r)	/* r= numero de regle */
			     {
				case RULE_ConstValue:	/* ConstValue */
				   if (ConstIndx + wl > MAX_TRANSL_CONST_LEN)
				      /* plus de place pour les constantes */
				      CompilerError (wi, TRA, FATAL, TRA_CONSTANT_BUFFER_OVERFLOW, inputLine, linenb);
				   else
				      /* FnCopy le texte de la constante */
				     {
					pSchTra->TsConstBegin[pSchTra->TsNConstants - 1] = ConstIndx;
					for (i = 0; i <= wl - 2; i++)
					   pSchTra->TsConstant[ConstIndx + i - 1] = inputLine[wi + i - 1];
					ConstIndx += wl;
					pSchTra->TsConstant[ConstIndx - 2] = '\0';
				     }
				   break;

				case RULE_CharString:	/* CharString */
				   nouvconst (wl, wi);
				   if (!error)
				      if (pr == RULE_Function)	/* dans une definition de variable */
					{
					   pVa1 = &pSchTra->TsVariable[pSchTra->TsNVariables - 1];
					   if (pVa1->TrvNItems >= MAX_TRANSL_VAR_ITEM)
					      /* trop de fonctions */
					      CompilerError (wi, TRA, FATAL, TRA_TOO_MANY_ELEMS_IN_THE_VARIABLE, inputLine, linenb);
					   else
					     {
						pVa1->TrvNItems++;
						pVa1->TrvItem[pVa1->TrvNItems - 1].TvType =
						   VtText;
						pVa1->TrvItem[pVa1->TrvNItems - 1].TvItem =
						   pSchTra->TsNConstants;
					     }
					}
				      else if (pr == RULE_Token)
					 /* dans une regle Create ou Write */
					{
					   CurRegle->TrObject = ToConst;
					   CurRegle->TrObjectNum = pSchTra->TsNConstants;
					}
				   break;

				case RULE_TextEqual:	/* TextEqual */
				   if (pSchStr->SsAttribute[CurAttr - 1].AttrType != AtTextAttr)
				      /* ce n'est pas un attribut textuel */
				      CompilerError (wi, TRA, FATAL, TRA_NOT_A_TEXTUAL_ATTR, inputLine, linenb);
				   else if (wl > MAX_NAME_LENGTH)
				      /* texte trop long */
				      CompilerError (wi, TRA, FATAL, TRA_NAME_TOO_LONG, inputLine, linenb);
				   else if (DansCondition)
				     {
					for (i = 0; i < wl - 1; i++)
					   CurBloc->TbCondition[CurBloc->TbNConditions - 1].TcTextValue[i] =
					      inputLine[wi + i - 1];
					CurBloc->TbCondition[CurBloc->TbNConditions - 1].TcTextValue[wl - 1] = '\0';
				     }
				   else
				     {
					pRT1 = &pSchTra->TsAttrTRule[CurAttr - 1];
					for (i = 0; i < wl - 1; i++)
					   pRT1->AtrTextValue[i] = inputLine[wi + i - 1];
					pRT1->AtrTextValue[wl - 1] = '\0';
				     }
				   break;

				case RULE_FileName:	/* FileName */
				   nouvconst (wl, wi);
				   if (!error)
				     {
					CurRegle->TrBufOrConst = ToConst;
					CurRegle->TrInclFile = pSchTra->TsNConstants;
				     }
				   break;

				case RULE_Source:	/* Source */
				   if (wl > MAX_SRCE_LEN)
				      /* chaine source trop longue */
				      CompilerError (wi, TRA, FATAL, TRA_STRING_TOO_LONG, inputLine, linenb);
				   else if ((SymbTrans || GraphTrans) && wl > 2)
				      /* dans les traductions de symboles et de graphiques */
				      /* la chaine source ne peut contenir qu'un caractere */
				      CompilerError (wi + 1, TRA, FATAL, TRA_ONLY_ONE_CHARACTER, inputLine, linenb);
				   else
				      NouvTradCar (wi, wl);
				   break;

				case RULE_Target:	/* Target */
				   if (wl > MAX_TARGET_LEN)
				      /* chaine cible trop longue */
				      CompilerError (wi, TRA, FATAL, TRA_STRING_TOO_LONG, inputLine, linenb);
				   else
				      TradCible (wi, wl);
				   break;

				case RULE_LineEndString:
				   /* chaine de fin de ligne */
				   pSchTra->TsEOL[0] = inputLine[wi - 1];
				   break;

				case RULE_LineEndInsertString:
				   /* chaine de fin de ligne */
				   if (wl <= MAX_NAME_LENGTH)
				      for (i = 0; i < wl - 1; i++)
					 pSchTra->TsTranslEOL[i] = inputLine[wi + i - 1];
				   pSchTra->TsTranslEOL[i] = '\0';
				   break;

				default:
				   break;
			     }
		       break;
		 }
     }
}


/* ---------------------------------------------------------------------- */
/* |    Main                                                            | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
int                 main (int argc, char **argv)

#else  /* __STDC__ */
int                 main (argc, argv)
int                 argc;
char              **argv;

#endif /* __STDC__ */

{
   Name                 pfilename;	/* nom du fichier a compiler */
   BinFile             infile;
   int                 len, i;
   boolean             fileOK;
   char                cppFileName[200];
   iline               wi;	/* position du debut du mot courant dans la ligne */
   iline               wl;	/* longueur du mot courant */
   nature              wn;	/* nature du mot courant */
   rnb                 r;	/* numero de regle */
   rnb                 pr;	/* numero de la regle precedente */
   grmcode             c;	/* code grammatical du mot trouve */
   int                 nb;	/* indice dans identtable du mot trouve, si c'est un

				   identificateur */

   TtaInitializeAppRegistry (argv[0]);
   i = TtaGetMessageTable ("libdialogue", LIB_MSG_MAX);
   COMPIL = TtaGetMessageTable ("compildialogue", COMPIL_MSG_MAX);
   TRA = TtaGetMessageTable ("tradialogue", TRA_MSG_MAX);

   error = False;
   /* initialise l'analyseur syntaxique */
   initsynt ();
   initgrm ("TRANS.GRM");
   if (!error)
     {
	if (argc != 2)
	   TtaDisplaySimpleMessage (TRA, FATAL, TRA_FILE_NOT_FOUND);
	else
	  {
	     strncpy (pfilename, argv[1], MAX_NAME_LENGTH - 1);
	     len = strlen (pfilename);
	     /* ajoute le suffixe .SCH */
	     strcat (pfilename, ".SCH");

	     if (FileExist (pfilename) == 0)
		TtaDisplaySimpleMessage (TRA, FATAL, TRA_FILE_NOT_FOUND);
	     else
		/* le fichier d'entree existe, on l'ouvre */
	       {
		  infile = BIOreadOpen (pfilename);
		  /* supprime le suffixe ".SCH" */
		  pfilename[len] = '\0';
		  /* acquiert la memoire pour le schema de traduction */
		  if ((pSchTra = (PtrTSchema) malloc (sizeof (TranslSchema))) == NULL)
		     TtaDisplaySimpleMessage (TRA, FATAL, TRA_NOT_ENOUGH_MEM);
		  lgidenttable = 0;	/* table des identificateurs vide */
		  linenb = 0;
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
		       while (i < linelen && inputLine[i - 1] != '\n' && fileOK);
		       /* marque la fin reelle de la ligne */
		       inputLine[i - 1] = '\0';
		       /* incremente le compteur de lignes */
		       linenb++;
		       if (i >= linelen)
			  CompilerError (1, TRA, FATAL, TRA_LINE_TOO_LONG,
					 inputLine, linenb);
		       else if (inputLine[0] == '#')
			  /* cette ligne contient une directive du preprocesseur cpp */
			 {
			    sscanf (inputLine, "# %d %s", &linenb, cppFileName);
			    linenb--;
			 }
		       else
			  /* traduit les caracteres de la ligne */
			 {
			    transchar ();
			    /* analyse la ligne */
			    wi = 1;
			    wl = 0;
			    /* analyse tous les mots de la ligne courante */
			    do
			      {
				 i = wi + wl;
				 getword (i, &wi, &wl, &wn);
				 /* mot suivant */
				 if (wi > 0)
				    /* on a trouve un mot */
				   {
				      analword (wi, wl, wn, &c, &r, &nb, &pr);
				      /* on analyse le mot */
				      if (!error)
					 generate (wi, wl, c, r, nb, pr);
				      /* on le traite */
				   }
			      }
			    while (!(wi == 0 || error));
			 }	/* il n'y a plus de mots dans la ligne */
		    }
		  BIOreadClose (infile);
		  if (!error)
		     termsynt ();	/* fin d'analyse */
		  if (!error)
		    {
		       /* ecrit le schema compile' dans le fichier de sortie */
		       /* le directory des schemas est le directory courant */
		       strcat (pfilename, ".TRA");
		       fileOK = WrSchTrad (pfilename, pSchTra, pSchStr);
		       if (!fileOK)
			  TtaDisplaySimpleMessageString (TRA, FATAL, TRA_CANNOT_WRITE, pfilename);
		    }
		  free (pSchTra);
		  free (pSchStr);
		  free (pSchStrExt);
	       }
	  }
     }
   TtaSaveAppRegistry ();
   exit (0);
}
