
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

/* ======================================================================= */
/* |                                                                    | */
/* |                           Projet THOT                              | */
/* |                                                                    | */
/* |    Le programme STR compile un schema de structure, contenu dans   | */
/* |    un fichier de type .SCH.                                        | */
/* |    STR est dirige' par la grammaire du langage de structure        | */
/* |    contenue, sous forme codee, dans le fichier STRUCT.GRM.         | */
/* |    Ce fichier .GRM est produit par le programme GRM.               | */
/* |    STR produit un fichier de type .STR qui sera ensuite utilise'   | */
/* |    par l'editeur thot, l'imprimeur print ou le traducteur export.  | */
/* |    Ce programme utilise le module ANALSYNT pour l'analyse          | */
/* |    syntaxique du schema a compiler.                                | */
/* |                                                                    | */
/* |                                                                    | */
/* |                    V. Quint        Juin 1984                       | */
/* |                                                                    | */
/* ======================================================================= */

#include "thot_sys.h"
#include "STRUCT.h"
#include "constmedia.h"
#include "constgrm.h"
#include "compilmsg.h"
#include "message.h"
#include "strmsg.h"
#include "typemedia.h"
#include "typegrm.h"
#include "storage.h"

#define MaxRecurs 15		/* nombre maximuimum de niveaux de
				   definition de regles dans une regle */
#define MaxTypeExterne 20	/* nombre max. de types de documents
				   externes */
typedef enum
  {
     AttrRef, RegleRef, Contenu
  }
ContStrExt;

/* Variables  pour l'analyseur syntaxique */
#define EXPORT
#include "compil.var"
#include "environ.var"
/* Variables importees de l'analyseur syntaxique */
#undef EXPORT
#define EXPORT extern
#include "analsynt.var"

int                 linenb;	/* compteur de lignes du fichier source */

/* Variables locales du compilateur STR */
static Name          pfilename;	/* nom du fichier a compiler */
static PtrSSchema pSchStr;	/* pointeur sur le schema de structure en cours

				   de construction */
static int          PtrConsText;

				/* index courant dans le buffer des const */
static boolean      AnalAttr;	/* on est en train d'analyser les attributs

				   globaux */
static boolean      AnalAttrLoc;	/* on est en train d'analyser des attributs

					   locaux */
static boolean      AnalParam;	/* on est en train d'analyser les parametres */
static boolean      AnalAssoc;	/* on analyse les elements associes */
static boolean      AnalUnits;	/* on analyse les unites exportees */
static boolean      RegleRacine;	/* on attend la regle racine */
static boolean      Regles;	/* on est dans les Regles */
static boolean      AnalExtens;	/* on est dans les regles d'extension */
static boolean      ExceptTypeExtern;	/* on a rencontre' "EXTERN" avant un nom */

				/* de type dans la section EXCEPT */
static SRule       *CurRuleExtens;	/* regle d'extension en cours de traitement */
static boolean      Minimum;	/* nombre minimum d'elements d'une liste */
static boolean      Maximum;	/* nombre maximum d'elements d'une liste */
static boolean      Droite[MaxRecurs];	/* on est dans la partie droite de la regle */

static int          NiveauR;	/* niveau de recursivite' */
static int  CurRule[MaxRecurs];		/* numero de la regle courante */
static Name          CurName;	/* symbole gauche de la derniere regle

				   rencontree */
static int          CurNb;	/* indice de ce nom dans la table des

				   identificateurs */
static int          CurNbAttrLoc;	/* nombre d'attributs locaux asocies a CurName */
static int CurAttrLoc[MAX_LOCAL_ATTR];	/* les attributs  locaux

							   associes a CurName */
static boolean      CurAttrReq[MAX_LOCAL_ATTR];	/* les booleens 'Required' des

						   attributs locaux associes a CurName */
static boolean      CurParam;	/* la derniere regle rencontree est un param */
static boolean      CurAssoc;	/* la derniere regle rencontree est un associe' */
static boolean      CurUnit;	/* la derniere regle rencontree est une unite

				   exportee */
static boolean      Egal;	/* c'est une regle d'egalite */
static boolean      Option;	/* c'est un composant facultatif  d'agregat */
static boolean      AttrObligatoire;	/* c'est un attribut obligatoire */
static int          Signe;	/* -1 ou 1 selon le signe de la derniere valeur 

				 * d'attribut rencontree */
static PtrSSchema pSchStrExt;	/* pointeur sur le schema de structure externe */
static int  RegleContenu;	/* element dont on traite le contenu exporte' */
static Name          NomTypeRef;	/* dernier nom d'un type reference' */
static int          DebutNomTypeRef;	/* position de ce nom dans la ligne */
static ContStrExt   ContexteStrExt;	/* contexte ou est utilise' une structure externe */
static boolean      ContenuInconnu;	/* indique que le contenu d'un element exporte 

					   n'est pas declare dans le schema */
static Name          IdentPreced;	/* Name du dernier identificateur de type rencontre' */
static int  ReglePreced;

static int          NbTypeExternes;	/* nombre de types declares comme externes */
static Name          TypeExterne[MaxTypeExterne];	/* table des noms de types

							   declares comme externes */
static boolean      TypeExterneInclus[MaxTypeExterne];	/* table des noms de */

				/* types declares comme externes inclus */
static boolean      DansExcept;	/* on est dans les declarations d'exceptions */
static int  TypeExcept;	/* numero du type d'element dont on traite les exceptions */
static int AttrExcept;	/* Numero de l'attribut dont on traite les exceptions */
static int          CurBasicElem;	/* numero du type de base courant */
static int          NbAlias;	/* nombre d'alias definis dans le schema */
static int  Alias[MAX_RULES_SSCHEMA];		/* les regles du schema qui definissent

						   des alias */
static boolean      PremierDePaire;	/* on a rencontre' le mot-cle "First" */
static boolean      SecondDePaire;	/* on a rencontre' le mot-cle "Second" */
static boolean      ReferenceAttr;	/* on traite une reference attribut */
static boolean      ImportExcept;	/* on rencontre' une exception ImportLine ou

					   ImportParagraph */

#include "filesystem.f"
#include "analsynt.f"
#include "wrschstr.f"
#include "rdschstr.f"
#include "cdialog.f"
#include "storage.f"
#include "message.f"
#include "compilmsg.f"

#ifdef __STDC__
extern void         TtaInitializeAppRegistry (char *);
extern void         TtaSaveAppRegistry (void);

#else
extern void         TtaInitializeAppRegistry ();
extern void         TtaSaveAppRegistry ();

#endif /* __STDC__ */

/* ---------------------------------------------------------------------- */
/* |    initPredefined                                                  | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void         initPredefined (SRule * pRule, char *name, BasicType type)

#else  /* __STDC__ */
static void         initPredefined (pRule, name, type)
SRule              *pRule;
char               *name;
BasicType          type;

#endif /* __STDC__ */

{
   strncpy (pRule->SrName, name, MAX_NAME_LENGTH);
   pRule->SrConstruct = CsBasicElement;
   pRule->SrBasicType = type;
   pRule->SrAssocElem = False;
   pRule->SrParamElem = False;
   pRule->SrUnitElem = False;
   pRule->SrExportedElem = False;
   pRule->SrFirstExcept = 0;
   pRule->SrLastExcept = 0;
   pRule->SrNDefAttrs = 0;
   pRule->SrRecursive = False;
   pRule->SrNLocalAttrs = 0;
   pRule->SrNInclusions = 0;
   pRule->SrNExclusions = 0;
   pRule->SrRefImportedDoc = False;
}


/* ---------------------------------------------------------------------- */
/* |    initgener       prepare la generation : initialise le schema de | */
/* |    structure en memoire.                                           | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         initgener ()

#else  /* __STDC__ */
static void         initgener ()
#endif				/* __STDC__ */

{
   int                 i;
   TtAttribute           *pAttr;
   SRule              *pRule;

   pSchStr->SsExtension = False;
   pSchStr->SsNExtensRules = 0;
   pSchStr->SsExtensBlock = NULL;
   pSchStr->SsRootElem = 0;
   CurRuleExtens = NULL;
   pSchStr->SsNAttributes = 0;
   for (i = 0; i < MAX_ATTR_SSCHEMA; i++)
     {
	pAttr = &pSchStr->SsAttribute[i];
	pAttr->AttrOrigName[0] = '\0';
	pAttr->AttrGlobal = True;
	/* tous les attributs sont globaux */
	pAttr->AttrFirstExcept = 0;
	/* pas d'exception pour l'attribut */
	pAttr->AttrLastExcept = 0;
	pAttr->AttrType = AtEnumAttr;
	pAttr->AttrNEnumValues = 0;
     }
   /* cree un attribut Langue */
   pAttr = &pSchStr->SsAttribute[0];
   strncpy (pAttr->AttrName, "Langue", MAX_NAME_LENGTH);
   pAttr->AttrOrigName[0] = '\0';
   pAttr->AttrGlobal = True;
   pAttr->AttrType = AtTextAttr;
   pSchStr->SsNAttributes++;

   pSchStr->SsNRules = 0;
   /* les premieres regles de structure du schema sont celles qui */
   /* definissent les types de base */
   pRule = &pSchStr->SsRule[ord (CharString)];
   initPredefined (pRule, "TEXT_UNIT", CharString);

   pRule = &pSchStr->SsRule[ord (GraphicElem)];
   initPredefined (pRule, "GRAPHICS_UNIT", GraphicElem);

   pRule = &pSchStr->SsRule[ord (Symbol)];
   initPredefined (pRule, "SYMBOL_UNIT", Symbol);

   pRule = &pSchStr->SsRule[ord (Picture)];
   initPredefined (pRule, "PICTURE_UNIT", Picture);

   pRule = &pSchStr->SsRule[ord (Refer)];
   initPredefined (pRule, "REFERENCE_UNIT", Refer);

   pRule = &pSchStr->SsRule[ord (PageBreak)];
   initPredefined (pRule, "PAGE_BREAK", PageBreak);

   pRule = &pSchStr->SsRule[ord (UnusedBasicType)];
   initPredefined (pRule, "UNUSED", UnusedBasicType);

   pSchStr->SsNRules = MAX_BASIC_TYPE;
   pSchStr->SsConstBuffer[0] = '\0';
   pSchStr->SsConstBuffer[1] = '\0';
   pSchStr->SsExport = False;
   pSchStr->SsNExceptions = 0;
   PtrConsText = 1;
   AnalAttr = False;
   AnalAttrLoc = False;
   AnalParam = False;
   AnalAssoc = False;
   AnalUnits = False;
   RegleRacine = False;
   Regles = False;
   AnalExtens = False;
   Minimum = False;
   Maximum = False;
   NiveauR = 1;
   Droite[0] = False;

   CurRule[0] = pSchStr->SsNRules;
   CurName[0] = '\0';
   CurNb = 0;
   CurNbAttrLoc = 0;
   CurParam = False;
   CurAssoc = False;
   CurUnit = False;
   Egal = False;
   Signe = 1;
   NbTypeExternes = 0;
   DansExcept = False;
   TypeExcept = 0;
   AttrExcept = 0;
   CurBasicElem = 0;
   NbAlias = 0;
   ReglePreced = 0;
   PremierDePaire = False;
   SecondDePaire = False;
   ImportExcept = False;
}


/* ---------------------------------------------------------------------- */
/* |    NomRegleExiste  verifie si le nom de la derniere regle de la    | */
/* |    table des regles est deja utilise' dans une autre regle de la   | */
/* |    table des regles. Retourne Vrai si oui.                         | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static boolean      NomRegleExiste ()

#else  /* __STDC__ */
static boolean      NomRegleExiste ()
#endif				/* __STDC__ */

{
   int                 r;
   boolean             ret;
   Name                 N;

   /* valeur de retour de la fonction */
   ret = False;
   /* nom de la derniere regle */
   strncpy (N, pSchStr->SsRule[pSchStr->SsNRules - 1].SrName, MAX_NAME_LENGTH);
   if (N[0] != '\0')
     {
	/* numero de la regle a traiter */
	r = 0;
	do
	   ret = strcmp (N, pSchStr->SsRule[r++].SrName) == 0;
	while (!ret && r < pSchStr->SsNRules - 1);
     }
   return ret;
}


/* ---------------------------------------------------------------------- */
/* |    nondefini                                                       | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         nondefini (int n)

#else  /* __STDC__ */
static void         nondefini (n)
int                 n;

#endif /* __STDC__ */
{
   int                 j;
   identelem          *pIdent;
   SRule              *pRule;

   pIdent = &identtable[n - 1];
   TtaDisplaySimpleMessageString (STR, INFO, STR_EXTERNAL_STRUCT, pIdent->identname);
   if (pSchStr->SsNRules >= MAX_RULES_SSCHEMA)
      TtaDisplaySimpleMessage (STR, FATAL, STR_TOO_MAN_RULES);
   /* trop de regles */
   else
      /* ajoute une regle de nature a la fin du schema */
     {
	pSchStr->SsNRules++;
	pIdent->identdef = pSchStr->SsNRules;
	if (pIdent->identlg > MAX_NAME_LENGTH - 1)
	   TtaDisplaySimpleMessage (STR, FATAL, STR_WORD_TOO_LONG);
	else
	  {
	     pRule = &pSchStr->SsRule[pSchStr->SsNRules - 1];
	     for (j = 0; j < pIdent->identlg; j++)
		pRule->SrName[j] = pIdent->identname[j];
	     pRule->SrName[pIdent->identlg] = '\0';
	     pRule->SrNLocalAttrs = 0;
	     pRule->SrNInclusions = 0;
	     pRule->SrNExclusions = 0;
	     pRule->SrRefImportedDoc = False;
	     pRule->SrNDefAttrs = 0;
	     pRule->SrAssocElem = False;
	     pRule->SrParamElem = False;
	     pRule->SrUnitElem = False;
	     pRule->SrConstruct = CsNatureSchema;
	     pRule->SrSSchemaNat = NULL;
	     pRule->SrRecursive = False;
	     pRule->SrExportedElem = False;
	     pRule->SrFirstExcept = 0;
	     pRule->SrLastExcept = 0;
	     if (NomRegleExiste ())
		TtaDisplaySimpleMessage (STR, FATAL, STR_NAME_ALREADY_DECLARED);
	  }
     }
}


/* ---------------------------------------------------------------------- */
/* |    change1rule met dans une regle les vrais numeros de regles a la | */
/* |    place des numeros d'identificateur. Les elements non definis    | */
/* |    sont considere's comme des structures externes (natures);       | */
/* |    les elements non reference's sont signale's comme des erreurs   | */
/* |    s'ils ne sont pas des elements associes.                        | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         change1rule (SRule * pRule)

#else  /* __STDC__ */
static void         change1rule (pRule)
SRule              *pRule;

#endif /* __STDC__ */

{
   int                 j;

   /* traitement des inclusions */
   for (j = 0; j < pRule->SrNInclusions; j++)
     {
	if (pRule->SrInclusion[j] > MAX_BASIC_TYPE)
	  {
	     if (identtable[pRule->SrInclusion[j] - MAX_BASIC_TYPE - 1].identdef == 0)
		nondefini (pRule->SrInclusion[j] - MAX_BASIC_TYPE);
	     pRule->SrInclusion[j] =
		identtable[pRule->SrInclusion[j] - MAX_BASIC_TYPE - 1].identdef;
	  }
	else if (pRule->SrInclusion[j] < 0)
	   pRule->SrInclusion[j] = -pRule->SrInclusion[j];
	pSchStr->SsRule[pRule->SrInclusion[j] - 1].SrRecursDone = False;
     }
   /* traitement des exclusions */
   for (j = 0; j < pRule->SrNExclusions; j++)
     {
	if (pRule->SrExclusion[j] > MAX_BASIC_TYPE)
	  {
	     if (identtable[pRule->SrExclusion[j] - MAX_BASIC_TYPE - 1].identdef == 0)
		nondefini (pRule->SrExclusion[j] - MAX_BASIC_TYPE);
	     pRule->SrExclusion[j] =
		identtable[pRule->SrExclusion[j] - MAX_BASIC_TYPE - 1].identdef;
	  }
	else if (pRule->SrExclusion[j] < 0)
	   pRule->SrExclusion[j] = -pRule->SrExclusion[j];
     }
   /* traitement selon le constructeur de la regle */
   switch (pRule->SrConstruct)
	 {
	    case CsReference:
	       /* on ne traite que les types definis dans le meme schema */
	       if (pRule->SrRefTypeNat[0] == '\0')
		  if (pRule->SrReferredType > MAX_BASIC_TYPE)
		    {
		       if (identtable[pRule->SrReferredType - MAX_BASIC_TYPE - 1].identdef == 0)
			  nondefini (pRule->SrReferredType - MAX_BASIC_TYPE);
		       pRule->SrReferredType = identtable[pRule->SrReferredType - MAX_BASIC_TYPE - 1].
			  identdef;
		    }
		  else if (pRule->SrReferredType < 0)
		     pRule->SrReferredType = -pRule->SrReferredType;
	       break;
	    case CsIdentity:
	       if (pRule->SrIdentRule > MAX_BASIC_TYPE)
		 {
		    if (identtable[pRule->SrIdentRule - MAX_BASIC_TYPE - 1].identdef == 0)
		       nondefini (pRule->SrIdentRule - MAX_BASIC_TYPE);
		    pRule->SrIdentRule = identtable[pRule->SrIdentRule - MAX_BASIC_TYPE - 1].
		       identdef;
		 }
	       else if (pRule->SrIdentRule < 0)
		  pRule->SrIdentRule = -pRule->SrIdentRule;
	       pSchStr->SsRule[pRule->SrIdentRule - 1].SrRecursDone = False;
	       break;
	    case CsList:
	       if (pRule->SrListItem > MAX_BASIC_TYPE)
		 {
		    if (identtable[pRule->SrListItem - MAX_BASIC_TYPE - 1].identdef == 0)
		       nondefini (pRule->SrListItem - MAX_BASIC_TYPE);
		    pRule->SrListItem = identtable[pRule->SrListItem - MAX_BASIC_TYPE - 1].
		       identdef;
		 }
	       else if (pRule->SrListItem < 0)
		  pRule->SrListItem = -pRule->SrListItem;
	       pSchStr->SsRule[pRule->SrListItem - 1].SrRecursDone = False;
	       break;
	    case CsChoice:
	       if (pRule->SrNChoices > 0)
		  for (j = 0; j < pRule->SrNChoices; j++)
		    {
		       if (pRule->SrChoice[j] > MAX_BASIC_TYPE)
			 {
			    if (identtable[pRule->SrChoice[j] - MAX_BASIC_TYPE - 1].identdef == 0)
			       nondefini (pRule->SrChoice[j] - MAX_BASIC_TYPE);
			    pRule->SrChoice[j] =
			       identtable[pRule->SrChoice[j] - MAX_BASIC_TYPE - 1].identdef;
			 }
		       else if (pRule->SrChoice[j] < 0)
			  pRule->SrChoice[j] = -pRule->SrChoice[j];
		       pSchStr->SsRule[pRule->SrChoice[j] - 1].SrRecursDone = False;
		    }
	       break;
	    case CsUnorderedAggregate:
	    case CsAggregate:
	       for (j = 0; j < pRule->SrNComponents; j++)
		 {
		    if (pRule->SrComponent[j] > MAX_BASIC_TYPE)
		      {
			 if (identtable[pRule->SrComponent[j] - MAX_BASIC_TYPE - 1].identdef == 0)
			    nondefini (pRule->SrComponent[j] - MAX_BASIC_TYPE);
			 pRule->SrComponent[j] =
			    identtable[pRule->SrComponent[j] - MAX_BASIC_TYPE - 1].identdef;
		      }
		    else if (pRule->SrComponent[j] < 0)
		       pRule->SrComponent[j] = -pRule->SrComponent[j];
		    pSchStr->SsRule[pRule->SrComponent[j] - 1].SrRecursDone = False;
		 }
	       break;
	    case CsConstant:
	       break;
	    case CsBasicElement:
	       break;
	    case CsNatureSchema:
	       break;
	    case CsPairedElement:
	       break;
	    case CsExtensionRule:
	       break;
	 }
}


/* ---------------------------------------------------------------------- */
/* |    Changerulenb    met dans les regles les vrais numeros de regles | */
/* |    a la place des numeros d'identificateur. Les elements non       | */
/* |    definis sont considere's comme des structures externes(natures);| */
/* |    les elements non reference's sont signale's comme des erreurs   | */
/* |    s'ils ne sont pas des elements associes.                        | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         changerulenb ()

#else  /* __STDC__ */
static void         changerulenb ()
#endif				/* __STDC__ */

{
   int         i;
   TtAttribute           *pAttr;

   /* utilise l'indicateur SrRecursDone, avec le sens `regle non utilisee' */
   for (i = MAX_BASIC_TYPE; i < pSchStr->SsNRules; i++)
      pSchStr->SsRule[i].SrRecursDone = True;
   /* parcourt toutes les regles des elements construits */
   for (i = MAX_BASIC_TYPE; i < pSchStr->SsNRules; i++)
      change1rule (&pSchStr->SsRule[i]);
   /* parcourt toutes les regles d'extension */
   for (i = 0; i < pSchStr->SsNExtensRules; i++)
      change1rule (&pSchStr->SsExtensBlock->EbExtensRule[i]);
   /* parcourt tous les attributs definis dans le schema de structure */
   for (i = 0; i < pSchStr->SsNAttributes; i++)
     {
	pAttr = &pSchStr->SsAttribute[i];
	/* on ne traite que les attributs reference */
	if (pAttr->AttrType == AtReferenceAttr)
	   /* on ne traite que les types definis dans le meme schema */
	   if (pAttr->AttrTypeRefNature[0] == '\0')
	      if (pAttr->AttrTypeRef > MAX_BASIC_TYPE)
		{
		   if (identtable[pAttr->AttrTypeRef - MAX_BASIC_TYPE - 1].identdef == 0)
		      nondefini (pAttr->AttrTypeRef - MAX_BASIC_TYPE);
		   pAttr->AttrTypeRef =
		      identtable[pAttr->AttrTypeRef - MAX_BASIC_TYPE - 1].identdef;
		}
	      else if (pAttr->AttrTypeRef < 0)
		 pAttr->AttrTypeRef = -pAttr->AttrTypeRef;
     }
}


/* ---------------------------------------------------------------------- */
/* |    copyword    copie dans n le mot traite'.                        | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         copyword (Name n, iline wi, iline wl)

#else  /* __STDC__ */
static void         copyword (n, wi, wl)
Name                 n;
iline               wi;
iline               wl;

#endif /* __STDC__ */

{
   if (wl > MAX_NAME_LENGTH - 1)
      CompilerError (wi, STR, FATAL, STR_WORD_TOO_LONG, inputLine, linenb);
   else
     {
	strncpy (n, &inputLine[wi - 1], wl);
	n[wl] = '\0';
     }
}

/* ---------------------------------------------------------------------- */
/* |    empile                                                          | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         empile (iline wi)

#else  /* __STDC__ */
static void         empile (wi)
iline               wi;

#endif /* __STDC__ */

{
   if (NiveauR >= MaxRecurs)
      CompilerError (wi, STR, FATAL, STR_RULE_NESTING_TOO_DEEP, inputLine, linenb);
   else
     {
	Droite[NiveauR] = False;
	CurRule[NiveauR] = 0;
	NiveauR++;
     }
}


/* ---------------------------------------------------------------------- */
/* |    RightIdentifier                                                 | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         RightIdentifier (int n, iline wi)

#else  /* __STDC__ */
static void         RightIdentifier (n, wi)
int                 n;
iline               wi;

#endif /* __STDC__ */

{
   SRule              *pRule;

   if (ReglePreced > 0)
     {
	n = -ReglePreced;
	ReglePreced = 0;
     }
   pRule = &pSchStr->SsRule[CurRule[NiveauR - 1] - 1];
   switch (pRule->SrConstruct)
	 {
	    case CsReference:
	       pRule->SrReferredType = n;
	       break;
	    case CsIdentity:
	       pRule->SrIdentRule = n;
	       break;
	    case CsList:
	       pRule->SrListItem = n;
	       break;
	    case CsChoice:
	       if (pRule->SrNChoices >= MAX_OPTION_CASE)
		  CompilerError (wi, STR, FATAL, STR_TOO_MANY_OPTIONS_IN_THE_CASE_STATEMENT, inputLine, linenb);
	       else
		 {
		    pRule->SrNChoices++;
		    pRule->SrChoice[pRule->SrNChoices - 1] = n;
		 }
	       break;
	    case CsUnorderedAggregate:
	    case CsAggregate:
	       if (pRule->SrNComponents >= MAX_COMP_AGG)
		  CompilerError (wi, STR, FATAL, STR_TOO_MANY_COMPONENTS_IN_AGGREGATE, inputLine, linenb);
	       else
		 {
		    pRule->SrNComponents++;
		    pRule->SrComponent[pRule->SrNComponents - 1] = n;
		    if (Option)
		      {
			 pRule->SrOptComponent[pRule->SrNComponents - 1] = True;
			 Option = False;
		      }
		 }
	       break;
	    default:
	       break;
	 }

}

/* ---------------------------------------------------------------------- */
/* |    newrule                                                         | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         newrule (iline wi)

#else  /* __STDC__ */
static void         newrule (wi)
iline               wi;

#endif /* __STDC__ */

{
   int                 i;
   SRule              *pRule;

   if (pSchStr->SsNRules >= MAX_RULES_SSCHEMA)
      CompilerError (wi, STR, FATAL, STR_TOO_MAN_RULES, inputLine, linenb);
   else
      pSchStr->SsNRules++;
   if (CurNb > 0)		/* il y a un symbole gauche a cette regle */
      if (identtable[CurNb - 1].identdef > 0)
	 /* double definition */
	 CompilerError (wi, STR, FATAL, STR_NAME_ALREADY_DECLARED, inputLine, linenb);
   if (!error)
     {
	CurRule[NiveauR - 1] = pSchStr->SsNRules;
	if (Regles && NiveauR > 1)
	   /* la regle courante est referencee par la regle englobante */
	  {
	     NiveauR--;
	     if (CurNb > 0)
		RightIdentifier (CurNb + MAX_BASIC_TYPE, wi);
	     else
		RightIdentifier (-CurRule[NiveauR], wi);
	     NiveauR++;
	  }
	if (CurNb > 0)
	   identtable[CurNb - 1].identdef = pSchStr->SsNRules;
	if (Regles)
	   Droite[NiveauR - 1] = True;
	pRule = &pSchStr->SsRule[pSchStr->SsNRules - 1];
	strncpy (pRule->SrName, CurName, MAX_NAME_LENGTH);
	pRule->SrNDefAttrs = 0;
	if (pRule->SrNLocalAttrs > 0)
	   /* il y a deja des attributs locaux pour cet element */
	   CompilerError (wi, STR, FATAL, STR_THIS_ELEM_HAS_LOCAL_ATTRS, inputLine, linenb);
	else
	   pRule->SrNLocalAttrs = CurNbAttrLoc;
	for (i = 0; i < pRule->SrNLocalAttrs; i++)
	  {
	     pRule->SrLocalAttr[i] = CurAttrLoc[i];
	     pRule->SrRequiredAttr[i] = CurAttrReq[i];
	  }
	pRule->SrParamElem = CurParam;
	pRule->SrAssocElem = CurAssoc;
	pRule->SrUnitElem = CurUnit;
	pRule->SrRecursive = False;
	pRule->SrExportedElem = False;
	pRule->SrFirstExcept = 0;
	/* pas d'exception associee a ce type d'elem. */
	pRule->SrLastExcept = 0;
	pRule->SrNInclusions = 0;
	pRule->SrNExclusions = 0;
	pRule->SrRefImportedDoc = False;
	if (NomRegleExiste ())
	   CompilerError (wi, STR, FATAL, STR_NAME_ALREADY_DECLARED, inputLine, linenb);
	if (RegleRacine)
	   /* compare ce nom avec le nom du schema */
	  {
	     if (strcmp (pSchStr->SsName, CurName) == 0)
		/* c'est la racine du schema */
		pSchStr->SsRootElem = pSchStr->SsNRules;
	     else if (!pSchStr->SsExtension)
		CompilerError (wi, STR, FATAL, STR_FIRST_RULE_MUST_BE_THE_ROOT, inputLine, linenb);
	     RegleRacine = False;
	  }
	CurName[0] = '\0';
	CurNb = 0;
	CurNbAttrLoc = 0;
	CurParam = False;
	CurAssoc = False;
	CurUnit = False;
     }
}


/* ---------------------------------------------------------------------- */
/* |    RuleNumber      retourne le numero de la regle definissant le   | */
/* |    mot courant comme un type d'element, ou 0 si le mot n'est pas   | */
/* |    un type d'element deja defini.                                  | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static int          RuleNumber (iline wl, iline wi)

#else  /* __STDC__ */
static int          RuleNumber (wl, wi)
iline              *wl;
iline              *wi;

#endif /* __STDC__ */

{
   int                 RuleNum;
   Name                 N;
   boolean             ok;

   copyword (N, wi, wl);
   RuleNum = 0;
   do
      ok = strcmp (N, pSchStr->SsRule[RuleNum++].SrName) == 0;
   while (!ok && RuleNum < pSchStr->SsNRules);
   if (!ok)
      RuleNum = 0;
   return RuleNum;
}


/* ---------------------------------------------------------------------- */
/* |    AttributeNumber retourne le numero de l'attribut ayant pour     | */
/* |    nom le mot courant, ou 0 si le mot n'est pas un attribut        | */
/* |    deja defini.                                                    | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static int          AttributeNumber (iline wl, iline wi)

#else  /* __STDC__ */
static int          AttributeNumber (wl, wi)
iline               wl;
iline               wi;

#endif /* __STDC__ */

{
   int                 AttrNum;
   Name                 N;
   boolean             ok;

   copyword (N, wi, wl);
   AttrNum = 0;
   do
      ok = strcmp (N, pSchStr->SsAttribute[AttrNum++].AttrName) == 0;
   while (!ok && AttrNum < pSchStr->SsNAttributes);
   if (!ok)
      AttrNum = 0;
   return AttrNum;
}


/* ---------------------------------------------------------------------- */
/* |    NumExcept       traite le numero d'exception Num. Si VerifType  | */
/* |    est vrai, verifie que l'exception porte sur un type d'element.  | */
/* |    Si VerifAttr est vrai, verifie que l'exception porte sur un     | */
/* |    attribut.                                                       | */
/* |    SiVerifNum est vrai, verifie que l'exception porte sur un       | */
/* |    attribut numerique.                                             | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void         NumExcept (int Num, boolean VerifType, boolean VerifAttr, boolean VerifNum, iline wi)

#else  /* __STDC__ */
static void         NumExcept (Num, VerifType, VerifAttr, VerifNum, wi)
int                 Num;
boolean             VerifType;
boolean             VerifAttr;
boolean             VerifNum;
iline               wi;

#endif /* __STDC__ */

{
   SRule              *pRule;
   TtAttribute           *pAttr;

   if (VerifType && TypeExcept == 0)
      CompilerError (wi, STR, FATAL, STR_ONLY_FOR_ELEMS, inputLine, linenb);
   if (VerifAttr && AttrExcept == 0)
      CompilerError (wi, STR, FATAL, STR_ONLY_FOR_ATTRS, inputLine, linenb);
   if (AttrExcept > 0)
     {
	if (VerifNum && pSchStr->SsAttribute[AttrExcept - 1].AttrType != AtNumAttr)
	   CompilerError (wi, STR, FATAL, STR_ONLY_FOR_NUMERICAL_ATTRS, inputLine, linenb);
	if (Num == ExcActiveRef && pSchStr->SsAttribute[AttrExcept - 1].AttrType != AtReferenceAttr)
	   CompilerError (wi, STR, FATAL, STR_ONLY_FOR_REFERENCE_ATTRS, inputLine, linenb);
     }
   if (!error)
     {
	if (pSchStr->SsNExceptions >= MAX_EXCEPT_SSCHEMA)
	   /* la liste des numeros d'exception est pleine */
	   CompilerError (wi, STR, FATAL, STR_TOO_MANY_EXCEPTS, inputLine, linenb);
	else
	   /* range le numero d'exception dans la liste des */
	   /* numeros d'exception du schema de structure */
	  {
	     pSchStr->SsNExceptions++;
	     pSchStr->SsException[pSchStr->SsNExceptions - 1] = Num;
	     if (TypeExcept != 0)
	       {
		  if (CurRuleExtens != NULL)
		     pRule = CurRuleExtens;
		  else
		     pRule = &pSchStr->SsRule[TypeExcept - 1];
		  if (pRule->SrFirstExcept == 0)
		     pRule->SrFirstExcept = pSchStr->SsNExceptions;
		  pRule->SrLastExcept = pSchStr->SsNExceptions;
	       }
	     else if (AttrExcept != 0)
	       {
		  pAttr = &pSchStr->SsAttribute[AttrExcept - 1];
		  if (pAttr->AttrFirstExcept == 0)
		     pAttr->AttrFirstExcept = pSchStr->SsNExceptions;
		  pAttr->AttrLastExcept = pSchStr->SsNExceptions;
	       }
	  }
     }
}


/* ---------------------------------------------------------------------- */
/* |    basicelem                                                       | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         basicelem (int n, iline wi, rnb pr)

#else  /* __STDC__ */
static void         basicelem (n, wi, pr)
int                 n;
iline               wi;
rnb                 pr;

#endif /* __STDC__ */

{
   SRule              *pRule;

   if (pr == RULE_InclElem || pr == RULE_ExclElem)
      if (AnalExtens)
	 pRule = CurRuleExtens;
      else
	 pRule = &pSchStr->SsRule[CurRule[NiveauR - 1] - 1];
   if (pr == RULE_InclElem)
     {
	if (pRule->SrNInclusions >= MAX_INCL_EXCL_SRULE)
	   CompilerError (wi, STR, FATAL, STR_TOO_MANY_INCLS_FOR_THAT_ELEM, inputLine, linenb);
	else
	  {
	     pRule->SrNInclusions++;
	     pRule->SrInclusion[pRule->SrNInclusions - 1] = n;
	  }
     }
   else if (pr == RULE_ExclElem)
     {
	if (pRule->SrNExclusions >= MAX_INCL_EXCL_SRULE)
	   CompilerError (wi, STR, FATAL, STR_TOO_MANY_EXCLUSIONS_FOR_THAT_ELEM, inputLine, linenb);
	else
	  {
	     pRule->SrNExclusions++;
	     pRule->SrExclusion[pRule->SrNExclusions - 1] = n;
	  }
     }
   else if (pr == RULE_ExceptType)
      /* Un nom de type de base dans les exceptions */
     {
	TypeExcept = n;
	if (pSchStr->SsRule[TypeExcept - 1].SrFirstExcept != 0)
	   CompilerError (wi, STR, FATAL, STR_THIS_TYPE_ALREADY_HAS_EXCEPTS, inputLine, linenb);
     }
   else
     {
	if (Egal)
	  {
	     newrule (wi);
	     Egal = False;
	  }
	if (!error)
	  {
	     if (Droite[NiveauR - 1])
		RightIdentifier (n, wi);
	     else
	       {
		  NiveauR--;
		  RightIdentifier (n, wi);
		  NiveauR++;
	       }
	     CurBasicElem = n;
	  }
     }
}


/* ---------------------------------------------------------------------- */
/* |    Stocke le texte d'une constante chaine                          | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static int          storeConstantText (int wi, int wl)

#else  /* __STDC__ */
static int          storeConstantText (wi, wl)
int                 wi;
int                 wl;

#endif /* __STDC__ */

{
   int                 i, pos;

   pos = PtrConsText;
   if (PtrConsText + wl >= MAX_LEN_ALL_CONST)
      CompilerError (wi, STR, FATAL, STR_CONSTANT_BUFFER_FULL, inputLine, linenb);
   else
     {
	for (i = 0; i <= wl - 2; i++)
	   pSchStr->SsConstBuffer[PtrConsText + i - 1] = inputLine[wi + i - 1];
	PtrConsText += wl;
	/* un nul pour la fin de la constante */
	pSchStr->SsConstBuffer[PtrConsText - 2] = '\0';
	/* un autre nul pour la fin de toutes les constantes */
	pSchStr->SsConstBuffer[PtrConsText - 1] = '\0';
     }
   return pos;
}


/* ---------------------------------------------------------------------- */
/* |    InitRegle       initialise une regle de structure               | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void         InitRegle (SRule * NouvRegle)

#else  /* __STDC__ */
static void         InitRegle (NouvRegle)
SRule              *NouvRegle;

#endif /* __STDC__ */

{
   NouvRegle->SrName[0] = '\0';
   NouvRegle->SrNDefAttrs = 0;
   NouvRegle->SrNLocalAttrs = 0;
   NouvRegle->SrAssocElem = False;
   NouvRegle->SrParamElem = False;
   NouvRegle->SrUnitElem = False;
   NouvRegle->SrRecursive = False;
   NouvRegle->SrExportedElem = False;
   NouvRegle->SrFirstExcept = 0;
   NouvRegle->SrLastExcept = 0;
   NouvRegle->SrNInclusions = 0;
   NouvRegle->SrNExclusions = 0;
   NouvRegle->SrRefImportedDoc = False;
}


/* ---------------------------------------------------------------------- */
/* |    DupliqueReglePaire      Si la derniere regle traitee est une    | */
/* |            CsPairedElement, duplique cette regle CsPairedElement.                      | */
/* ---------------------------------------------------------------------- */


#ifdef __STDC__
static void         DupliqueReglePaire ()

#else  /* __STDC__ */
static void         DupliqueReglePaire ()
#endif				/* __STDC__ */

{
   SRule              *NouvRegle;
   SRule              *ReglePrec;

   ReglePrec = &pSchStr->SsRule[pSchStr->SsNRules - 1];
   if (ReglePrec->SrConstruct == CsPairedElement && ReglePrec->SrFirstOfPair)
     {
	/* ajoute une regle CsPairedElement a la fin du schema */
	pSchStr->SsNRules++;
	NouvRegle = &pSchStr->SsRule[pSchStr->SsNRules - 1];
	InitRegle (NouvRegle);
	strncpy (NouvRegle->SrName, ReglePrec->SrName, MAX_NAME_LENGTH);
	NouvRegle->SrConstruct = CsPairedElement;
	NouvRegle->SrFirstOfPair = False;
     }
}


/* ---------------------------------------------------------------------- */
/* |    ChercheRegleExtens      recherche une regle d'extension ayant   | */
/* |                    le nom (wi, wl)                                 | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static SRule       *ChercheRegleExtens (iline wi, iline wl)

#else  /* __STDC__ */
static SRule       *ChercheRegleExtens (wi, wl)
iline               wi;
iline               wl;

#endif /* __STDC__ */

{
   SRule              *pR;
   Name                 n;
   int                 r;
   boolean             trouve;

   pR = NULL;
   if (pSchStr->SsExtensBlock != NULL)
     {
	copyword (n, wi, wl);
	trouve = False;
	r = 0;
	while (!trouve && r < pSchStr->SsNExtensRules)
	  {
	     pR = &pSchStr->SsExtensBlock->EbExtensRule[r];
	     if (strncmp (n, pR->SrName, MAX_NAME_LENGTH) == 0)
		trouve = True;
	     else
		r++;
	  }
	if (!trouve)
	   pR = NULL;
     }
   return pR;
}


/* ---------------------------------------------------------------------- */
/* |    NouvRegleExtens alloue et initialise une nouvelle regle         | */
/* |                    d'extension                                     | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static SRule       *NouvRegleExtens (iline wi, iline wl)

#else  /* __STDC__ */
static SRule       *NouvRegleExtens (wi, wl)
iline               wi;
iline               wl;

#endif /* __STDC__ */

{
   SRule              *pR;

   pR = NULL;
   if (ChercheRegleExtens (wi, wl) != NULL)
      CompilerError (wi, STR, FATAL, STR_NAME_ALREADY_DECLARED, inputLine, linenb);
   if (pSchStr->SsExtensBlock == NULL)
      if ((pSchStr->SsExtensBlock = (PtrExtensBlock) malloc (sizeof (ExtensBlock))) == NULL)
	 TtaDisplaySimpleMessage (STR, FATAL, STR_NOT_ENOUGH_MEM);
   if (pSchStr->SsExtensBlock != NULL)
      if (pSchStr->SsNExtensRules >= MAX_EXTENS_SSCHEMA)
	 CompilerError (wi, STR, FATAL, STR_TOO_MAN_RULES, inputLine, linenb);
      else
	{
	   pR = &pSchStr->SsExtensBlock->EbExtensRule[pSchStr->SsNExtensRules];
	   pSchStr->SsNExtensRules++;
	   InitRegle (pR);
	   copyword (pR->SrName, wi, wl);
	   pR->SrConstruct = CsExtensionRule;
	}
   return pR;
}


/* ---------------------------------------------------------------------- */
/* |    generate        traite le mot commencant a la position wi dans  | */
/* |    la ligne courante, de longueur wl et de code grammatical c.     | */
/* |    Si c'est un identificateur, nb contient son rang dans la table  | */
/* |    des identificateurs. r est le numero de la regle dans laquelle  | */
/* |    apparait ce mot.                                                | */
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
   int                 nombre, i, j;
   Name                 N;
   TtAttribute           *pAttr;
   SRule              *pRule;
   boolean             ok;
   int                 numAttr;

   if (c < 1000)
      /* symbole intermediaire de la grammaire, erreur */
      CompilerError (wi, STR, FATAL, STR_INTERMEDIATE_SYMBOL, inputLine, linenb);
   else if (c < 1100)
      /* mot-cle court */
      switch (c)
	    {
	       case CHR_59:
		  /*  ;  */
		  if (r == RULE_LocAttrList)
		     /* fin de la definition d'un attribut local */
		    {
		       if (AnalExtens)
			  /* dans une regle d'extension */
			  pAttr = &pSchStr->SsAttribute[CurRuleExtens->SrLocalAttr[CurRuleExtens->SrNLocalAttrs - 1] - 1];
		       else
			  /* dans une regle de structure */
			  pAttr = &pSchStr->SsAttribute[CurAttrLoc[CurNbAttrLoc - 1] - 1];
		       if (pAttr->AttrType == AtEnumAttr && pAttr->AttrNEnumValues == 0)
			  /* pas de valeur definie */
			  CompilerError (wi, STR, FATAL, STR_ATTR_WITHOUT_VALUE, inputLine, linenb);
		    }
		  if (!(AnalAttr || AnalAttrLoc))
		     if (r == RULE_RuleList || r == RULE_OptDefList || r == RULE_DefList)
			/* fin de regle de structure */
		       {
			  if (CurNb > 0)
			     /* le dernier nom rencontre' n'a pas ete traite' */
			    {
			       NiveauR--;
			       /* il figure en partie droite de */
			       /* la regle de niveau inferieur */
			       RightIdentifier (CurNb + MAX_BASIC_TYPE, wi);
			       NiveauR++;
			       CurNb = 0;
			       CurName[0] = '\0';
			       CurNbAttrLoc = 0;
			       CurParam = False;
			       CurAssoc = False;
			       CurUnit = False;
			    }
			  Droite[NiveauR - 1] = False;	/* partie droite de regle terminee */

			  CurBasicElem = 0;
			  DupliqueReglePaire ();
		       }
		  if (r == RULE_ExpList)
		    {
		       /* fin d'un element exporte' */
		       if (ContenuInconnu)
			  CompilerError (DebutNomTypeRef, STR, FATAL, STR_TYPE_UNKNOWN, inputLine, linenb);
		       /* contenu invalide */
		    }

		  if (r == RULE_ExceptList)
		     /* fin des exceptions d'un type ou d'un attribut */
		    {
		       CurRuleExtens = NULL;
		       TypeExcept = 0;
		       AttrExcept = 0;
		    }
		  break;
	       case CHR_61:
		  /*  =  */
		  if (r == RULE_Rule || r == RULE_ExtOrDef)
		     Egal = True;
		  else if (r == RULE_LocalAttr)
		     /* on va definir le type d'un attribut local */
		    {
		       pAttr = &pSchStr->SsAttribute[pSchStr->SsNAttributes - 1];
		       if (pAttr->AttrType != AtEnumAttr
			   || (pAttr->AttrType == AtEnumAttr && pAttr->AttrNEnumValues > 0))
			  /* attribut deja defini */
			  CompilerError (wi, STR, FATAL, STR_THAT_ATTR_HAS_ALREADY_VALUES, inputLine, linenb);
		    }
		  break;
	       case CHR_40:
		  /*  (  */
		  if (r == RULE_Constr)
		    {
		       if (pSchStr->SsRule[CurRule[NiveauR - 1] - 1].SrConstruct == CsList)
			  /* definition des elements d'une liste */
			  empile (wi);
		    }

		  if (r == RULE_LocAttrList)
		     /* debut des attributs locaux d'un type */
		    {
		       AnalAttrLoc = True;
		       CurNbAttrLoc = 0;
		    }
		  break;
	       case CHR_41:
		  /*  )  */
		  if (r == RULE_LocAttrList)
		     /* fin des attributs locaux d'un type */
		    {
		       AnalAttrLoc = False;
		       if (AnalExtens)
			  /* dans une regle d'extension */
			  pAttr = &pSchStr->SsAttribute[CurRuleExtens->SrLocalAttr[CurRuleExtens->SrNLocalAttrs - 1] - 1];
		       else
			  /* dans une regle de structure */
			  pAttr = &pSchStr->SsAttribute[CurAttrLoc[CurNbAttrLoc - 1] - 1];
		       if (pAttr->AttrType == AtEnumAttr && pAttr->AttrNEnumValues == 0)
			  /* pas de valeur definie */
			  CompilerError (wi, STR, FATAL, STR_ATTR_WITHOUT_VALUE, inputLine, linenb);
		       if (CurBasicElem > 0)
			  /* fin des attributs locaux d'un element de base */
			 {
			    pRule = &pSchStr->SsRule[CurBasicElem - 1];
			    if (pRule->SrNLocalAttrs > 0)
			       /* il y a deja des attributs locaux pour cet element */
			       CompilerError (wi, STR, FATAL, STR_THIS_ELEM_HAS_LOCAL_ATTRS, inputLine, linenb);
			    else
			       pRule->SrNLocalAttrs = CurNbAttrLoc;
			    for (i = 0; i < pRule->SrNLocalAttrs; i++)
			      {
				 pRule->SrLocalAttr[i] = CurAttrLoc[i];
				 pRule->SrRequiredAttr[i] = CurAttrReq[i];
			      }
			    CurBasicElem = 0;
			    CurNbAttrLoc = 0;
			 }
		    }
		  if (r == RULE_Constr)
		    {
		       CurBasicElem = 0;
		       if (NiveauR > 1)
			  if (pSchStr->SsRule[CurRule[NiveauR - 2] - 1].SrConstruct == CsList)
			     /* fin d'une regle LIST */
			     /* partie droite de regle terminee */
			    {
			       Droite[NiveauR - 1] = False;

			       /* attributs fixes termines */
			       NiveauR--;
			       if (CurNb > 0)
				 {
				    RightIdentifier (CurNb + MAX_BASIC_TYPE, wi);
				    CurNb = 0;
				    CurName[0] = '\0';
				    CurNbAttrLoc = 0;
				    CurParam = False;
				    CurAssoc = False;
				    CurUnit = False;
				 }
			       DupliqueReglePaire ();
			    }
		    }
		  if (r == RULE_AttrType || r == RULE_Constr)
		     /* fin de la definition d'une reference (attribut ou element) */
		    {
		       if (PremierDePaire || SecondDePaire)
			  /* le nom d'element reference' est precede' de First ou Second */
			 {
			    /* cherche si le type reference' est deja defini */
			    i = 0;
			    do
			      {
				 ok = strcmp (NomTypeRef, pSchStr->SsRule[i].SrName) == 0;
				 i++;	/* passe a la regle suivante */
			      }
			    while (!ok && i < pSchStr->SsNRules);
			    if (!ok)
			       CompilerError (DebutNomTypeRef, STR, FATAL, STR_TYPE_UNKNOWN, inputLine, linenb);
			    else if (pSchStr->SsRule[i - 1].SrConstruct != CsPairedElement)
			       /* ce n'est pas une paire */
			       CompilerError (DebutNomTypeRef, STR, FATAL, STR_FIRST_SECOND_FORBIDDEN, inputLine, linenb);
			    else if (SecondDePaire)
			      {
				 if (r == RULE_AttrType)
				    pSchStr->SsAttribute[pSchStr->SsNAttributes - 1].
				       AttrTypeRef = -(i + 1);
				 else if (pSchStr->SsRule[CurRule[NiveauR - 1] - 1].SrConstruct
					  == CsReference)
				    pSchStr->SsRule[CurRule[NiveauR - 1] - 1].SrReferredType
				       = -(i + 1);
			      }
			 }
		       PremierDePaire = False;
		       SecondDePaire = False;
		    }
		  break;
	       case CHR_44:
		  /*  ,  */
		  if (r == RULE_ExpList)
		     /* fin d'un element exporte' */
		     if (ContenuInconnu)
			CompilerError (DebutNomTypeRef, STR, FATAL, STR_TYPE_UNKNOWN, inputLine, linenb);
		  /* contenu invalide */
		  break;
	       case CHR_91:
		  /*  [  */
		  Minimum = True;
		  break;
	       case CHR_46:
		  /*  .  */
		  Minimum = False;
		  Maximum = True;
		  break;
	       case CHR_93:
		  /*  ]  */
		  Maximum = False;
		  break;
	       case CHR_63:
		  /*  ?  */
		  if (r == RULE_FixModValue)
		    {
		       /* la valeur de l'attribut fixe sera en fait modifiable */
		       if (AnalExtens)
			  pRule = CurRuleExtens;
		       else
			  pRule = &pSchStr->SsRule[CurRule[NiveauR - 1] - 1];
		       pRule->SrDefAttrModif[pRule->SrNDefAttrs - 1] = True;
		    }
		  else
		     Option = True;
		  break;
	       case CHR_33:
		  /*  !  */
		  AttrObligatoire = True;
		  break;
	       case CHR_42:
		  /*  *  */
		  if (r == RULE_min)
		     pSchStr->SsRule[CurRule[NiveauR - 1] - 1].SrMinItems = 0;
		  else
		     pSchStr->SsRule[CurRule[NiveauR - 1] - 1].SrMaxItems = 32000;
		  break;
	       case CHR_43:
		  /*  +  */
		  if (r == RULE_DefWith)
		     if (!Droite[NiveauR - 1])
			CompilerError (wi, STR, FATAL, STR_GIVE_A_NAME_TO_THAT_ELEM, inputLine, linenb);
		  break;
	       case CHR_45:
		  /*  -  */
		  if (r == RULE_FixedValue)
		     /* signe negatif pour la valeur d'attribut qui suit */
		     Signe = -1;
		  else if (r == RULE_DefWith)
		     if (!Droite[NiveauR - 1])
			CompilerError (wi, STR, FATAL, STR_GIVE_A_NAME_TO_THAT_ELEM, inputLine, linenb);
		  break;
		  /* signe negatif pour la valeur d'attribut qui */
		  /* suit */
	       case CHR_58:
		  /*  :  */
		  break;
	    }

   else if (c < 2000)
      /* mot-cle long */
      switch (c)
	    {
	       case KWD_STRUCTURE:
		  break;
	       case KWD_EXTENSION:
		  pSchStr->SsExtension = True;
		  pSchStr->SsNExtensRules = 0;
		  pSchStr->SsExtensBlock = NULL;
		  break;
	       case KWD_DEFPRES:
		  break;
	       case KWD_ATTR /* ATTR */ :
		  if (r == RULE_StructModel)
		     /* debut des attributs globaux */
		     AnalAttr = True;
		  else if (r == RULE_LocAttrList)
		     /* debut des attributs locaux d'un type d'element */
		     /* a priori, le prochain attribut local n'est pas obligatoire */
		     AttrObligatoire = False;
		  break;
	       case KWD_CONST:
		  AnalAttr = False;
		  break;
	       case KWD_PARAM:
		  if (pSchStr->SsExtension)
		     CompilerError (wi, STR, FATAL, STR_NOT_ALLOWED_IN_AN_EXTENSION, inputLine, linenb);
		  else
		    {
		       AnalParam = True;
		       Regles = True;
		       AnalAttr = False;
		    }
		  break;
	       case KWD_STRUCT:
		  /* la premiere regle sera la regle racine */
		  RegleRacine = True;
		  Regles = True;
		  AnalParam = False;
		  AnalAttr = False;
		  break;
	       case KWD_EXTENS:
		  /* verifie qu'on est bien dans une extension de schema */
		  if (!pSchStr->SsExtension)
		     CompilerError (wi, STR, FATAL, STR_NOT_AN_EXTENSION, inputLine, linenb);
		  else
		    {
		       AnalParam = False;
		       AnalAttr = False;
		       AnalExtens = True;
		    }
		  break;
	       case KWD_ASSOC:
		  if (pSchStr->SsRootElem == 0 && !pSchStr->SsExtension)
		     CompilerError (wi, STR, FATAL, STR_STRUCT_SECTION_MISSING, inputLine, linenb);
		  else
		    {
		       AnalParam = False;
		       AnalAttr = False;
		       AnalAssoc = True;
		       AnalExtens = False;
		    }
		  break;
	       case KWD_UNITS:
		  if (pSchStr->SsRootElem == 0 && !pSchStr->SsExtension)
		     CompilerError (wi, STR, FATAL, STR_STRUCT_SECTION_MISSING, inputLine, linenb);
		  else
		    {
		       AnalUnits = True;
		       AnalAssoc = False;
		       AnalParam = False;
		       AnalAttr = False;
		       AnalExtens = False;
		    }
		  break;
	       case KWD_EXPORT:
		  if (pSchStr->SsRootElem == 0 && !pSchStr->SsExtension)
		     CompilerError (wi, STR, FATAL, STR_STRUCT_SECTION_MISSING, inputLine, linenb);
		  else if (pSchStr->SsExtension)
		     CompilerError (wi, STR, FATAL, STR_NOT_ALLOWED_IN_AN_EXTENSION, inputLine, linenb);
		  else
		    {
		       AnalParam = False;
		       AnalAttr = False;
		       AnalExtens = False;
		       AnalAssoc = False;
		       AnalUnits = False;
		       changerulenb ();
		       /* met les bons numeros de regle */
		       pSchStr->SsExport = True;
		    }
		  break;
	       case KWD_EXCEPT:
		  if (pSchStr->SsRootElem == 0 && !pSchStr->SsExtension)
		     CompilerError (wi, STR, FATAL, STR_STRUCT_SECTION_MISSING, inputLine, linenb);
		  else
		    {
		       AnalParam = False;
		       AnalAttr = False;
		       AnalExtens = False;
		       AnalAssoc = False;
		       AnalUnits = False;
		       CurRuleExtens = NULL;
		       DansExcept = True;
		       ExceptTypeExtern = False;
		    }
		  break;
	       case KWD_END:
		  if (r == RULE_Constr)
		    {
		       NiveauR--;
		       if (CurNb > 0)
			 {
			    RightIdentifier (CurNb + MAX_BASIC_TYPE, wi);
			    CurNb = 0;
			    CurName[0] = '\0';
			    CurNbAttrLoc = 0;
			    CurParam = False;
			    CurAssoc = False;
			    CurUnit = False;
			 }
		    }
		  else if (r == RULE_StructModel)
		     if (pSchStr->SsRootElem == 0 && !pSchStr->SsExtension)
			CompilerError (wi, STR, FATAL, STR_STRUCT_SECTION_MISSING, inputLine, linenb);
		  break;
	       case KWD_INTEGER:
		  pSchStr->SsAttribute[pSchStr->SsNAttributes - 1].AttrType = AtNumAttr;
		  break;
	       case KWD_TEXT:
		  if (r == RULE_BasicType)
		     /* type d'un element */
		     basicelem (ord (CharString) + 1, wi, pr);
		  else
		     /* type d'un attribut */
		     pSchStr->SsAttribute[pSchStr->SsNAttributes - 1].AttrType = AtTextAttr;
		  break;
	       case KWD_REFERENCE:
		  if (r == RULE_Constr)
		     /* constructeur d'un element */
		    {
		       ReferenceAttr = False;	/* c'est un element reference */
		       Egal = False;
		       newrule (wi);
		       if (!error)
			 {
			    pRule = &pSchStr->SsRule[CurRule[NiveauR - 1] - 1];
			    pRule->SrConstruct = CsReference;
			    pRule->SrRefTypeNat[0] = '\0';
			    /* a priori, le type d'element */
			    /* reference' est defini dans le meme schema */
			    pRule->SrRefImportedDoc = False;
			    pRule->SrReferredType = 0;
			    /* c'est une reference ordinaire, */
			    /* pas l'inclusion d'un document externe */
			    ContexteStrExt = RegleRef;
			 }
		    }
		  else
		     /* type d'un attribut */
		    {
		       ReferenceAttr = True;	/* c'est un attribut reference */
		       pAttr = &pSchStr->SsAttribute[pSchStr->SsNAttributes - 1];
		       pAttr->AttrType = AtReferenceAttr;
		       pAttr->AttrTypeRefNature[0] = '\0';
		       pAttr->AttrTypeRef = 0;
		       /* a priori, le type d'elem. */
		       /* reference' est defini dans le meme schema */
		       ContexteStrExt = AttrRef;
		    }
		  break;
	       case KWD_ANY:
		  if (ReferenceAttr)
		     pSchStr->SsAttribute[pSchStr->SsNAttributes - 1].AttrTypeRef = 0;
		  else
		     pSchStr->SsRule[CurRule[NiveauR - 1] - 1].SrReferredType = 0;
		  break;
	       case KWD_First:
		  PremierDePaire = True;
		  break;
	       case KWD_Second:
		  SecondDePaire = True;
		  break;
	       case KWD_GRAPHICS:
		  basicelem (ord (GraphicElem) + 1, wi, pr);
		  break;
	       case KWD_SYMBOL:
		  basicelem (ord (Symbol) + 1, wi, pr);
		  break;
	       case KWD_PICTURE:
		  basicelem (ord (Picture) + 1, wi, pr);
		  break;
	       case KWD_UNIT:
	       case KWD_NATURE:
		  if (pr == RULE_ExclElem || pr == RULE_InclElem)
		     CompilerError (wi, STR, FATAL, STR_NOT_ALLOWED_HERE, inputLine, linenb);
		  else
		    {
		       Egal = False;
		       newrule (wi);
		       if (!error)
			 {
			    pRule = &pSchStr->SsRule[CurRule[NiveauR - 1] - 1];
			    pRule->SrConstruct = CsChoice;
			    if (c == KWD_UNIT)
			       /* 18:     */
			       pRule->SrNChoices = 0;	/* UNIT  */
			    else
			       /* 19:     */
			       pRule->SrNChoices = -1;		/* NATURE  */
			 }
		    }
		  break;
	       case KWD_WITH:
		  /* debut des attributs fixes */
		  if (r == RULE_DefWith)
		     if (!Droite[NiveauR - 1])
			CompilerError (wi, STR, FATAL, STR_GIVE_A_NAME_TO_THAT_ELEM, inputLine, linenb);

		  break;
	       case KWD_EXTERN:
		  if (r == RULE_ExceptType)
		     ExceptTypeExtern = True;
		  else if (r == RULE_ExtOrDef)
		     if (NbTypeExternes >= MaxTypeExterne)
			/* table des types externes saturee */
			CompilerError (wi, STR, FATAL, STR_TOO_MANY_EXTERNAL_DOCS, inputLine, linenb);
		     else
			/* met dans la table le dernier nom de type rencontre' */
		       {
			  NbTypeExternes++;
			  TypeExterneInclus[NbTypeExternes - 1] = False;
			  strncpy (TypeExterne[NbTypeExternes - 1], IdentPreced, MAX_NAME_LENGTH);
			  if (strcmp (IdentPreced, pSchStr->
			  SsRule[pSchStr->SsRootElem - 1].SrName) == 0)
			     /* C'est le type de document lui-meme qui est utilise' */
			     /* comme externe */
			    {
			       /* ajoute une regle de nature a la fin du schema */
			       pRule = &pSchStr->SsRule[pSchStr->SsNRules++];
			       strncpy (pRule->SrName, IdentPreced, MAX_NAME_LENGTH);
			       pRule->SrNLocalAttrs = 0;
			       pRule->SrNDefAttrs = 0;
			       pRule->SrAssocElem = False;
			       pRule->SrParamElem = False;
			       pRule->SrUnitElem = False;
			       pRule->SrConstruct = CsNatureSchema;
			       pRule->SrSSchemaNat = NULL;
			       pRule->SrRecursive = False;
			       pRule->SrExportedElem = False;
			       pRule->SrFirstExcept = 0;
			       pRule->SrLastExcept = 0;
			       ReglePreced = pSchStr->SsNRules;
			    }
		       }
		  break;
	       case KWD_INCLUDED:
		  /* included */
		  if (NbTypeExternes >= MaxTypeExterne)
		     /* table des types externes saturee */
		     CompilerError (wi, STR, FATAL, STR_TOO_MANY_EXTERNAL_DOCS, inputLine, linenb);
		  else
		     /* met dans la table le dernier nom de type rencontre' */
		    {
		       TypeExterneInclus[NbTypeExternes] = True;
		       strncpy (TypeExterne[NbTypeExternes], IdentPreced, MAX_NAME_LENGTH);
		       NbTypeExternes++;
		    }
		  break;
	       case KWD_LIST:
		  Egal = False;
		  newrule (wi);
		  if (!error)
		    {
		       pRule = &pSchStr->SsRule[CurRule[NiveauR - 1] - 1];
		       pRule->SrConstruct = CsList;
		       pRule->SrMinItems = 0;
		       pRule->SrMaxItems = 32000;
		    }
		  break;
	       case KWD_OF:
		  break;
	       case KWD_AGGREGATE:
	       case KWD_BEGIN:
		  Egal = False;
		  newrule (wi);
		  if (!error)
		    {
		       pRule = &pSchStr->SsRule[CurRule[NiveauR - 1] - 1];
		       if (c == KWD_BEGIN)
			  pRule->SrConstruct = CsAggregate;
		       else if (c == KWD_AGGREGATE)
			  pRule->SrConstruct = CsUnorderedAggregate;
		       pRule->SrNComponents = 0;
		       for (i = 0; i < MAX_COMP_AGG; i++)
			  pRule->SrOptComponent[i] = False;
		       Option = False;
		       empile (wi);
		    }
		  break;
	       case KWD_CASE:
		  Egal = False;
		  newrule (wi);
		  if (!error)
		    {
		       pRule = &pSchStr->SsRule[CurRule[NiveauR - 1] - 1];
		       pRule->SrConstruct = CsChoice;
		       pRule->SrNChoices = 0;
		       empile (wi);
		    }
		  break;
	       case KWD_CONSTANT:
		  break;
	       case KWD_PAIR:
		  Egal = False;
		  newrule (wi);
		  if (!error)
		     if (pSchStr->SsRootElem == CurRule[NiveauR - 1])
			CompilerError (wi, STR, FATAL, STR_ROOT_CANNOT_BE_A_PAIR, inputLine, linenb);
		     else
		       {
			  pRule = &pSchStr->SsRule[CurRule[NiveauR - 1] - 1];
			  pRule->SrConstruct = CsPairedElement;
			  pRule->SrFirstOfPair = True;
		       }
		  break;
	       case KWD_Nothing:
		  /* pas de contenu pour l'element exporte' courant */
		  pRule = &pSchStr->SsRule[RegleContenu - 1];
		  pRule->SrExportContent = 0;
		  if ((pRule->SrConstruct == CsChoice) && (pRule->SrNChoices >= 1))
		     for (j = 0; j < pRule->SrNChoices; j++)
			pSchStr->SsRule[pRule->SrChoice[j] - 1].SrExportContent = 0;
		  break;
	       case KWD_NoCut:
		  NumExcept (ExcNoCut, True, False, False, wi);
		  break;
	       case KWD_NoCreate:
		  NumExcept (ExcNoCreate, True, False, False, wi);
		  break;
	       case KWD_NoHMove:
		  NumExcept (ExcNoHMove, True, False, False, wi);
		  break;
	       case KWD_NoVMove:
		  NumExcept (ExcNoVMove, True, False, False, wi);
		  break;
	       case KWD_NoHResize:
		  NumExcept (ExcNoHResize, True, False, False, wi);
		  break;
	       case KWD_NoVResize:
		  NumExcept (ExcNoVResize, True, False, False, wi);
		  break;
	       case KWD_NewWidth:
		  NumExcept (ExcNewWidth, False, True, True, wi);
		  break;
	       case KWD_NewHeight:
		  NumExcept (ExcNewHeight, False, True, True, wi);
		  break;
	       case KWD_NewHPos:
		  NumExcept (ExcNewHPos, False, True, True, wi);
		  break;
	       case KWD_NewVPos:
		  NumExcept (ExcNewVPos, False, True, True, wi);
		  break;
	       case KWD_Invisible:
		  NumExcept (ExcInvisible, False, True, False, wi);
		  break;
	       case KWD_NoMove:
		  NumExcept (ExcNoMove, True, False, False, wi);
		  break;
	       case KWD_NoResize:
		  NumExcept (ExcNoResize, True, False, False, wi);
		  break;
	       case KWD_IsDraw:
		  NumExcept (ExcIsDraw, True, False, False, wi);
		  break;
	       case KWD_MoveResize:
		  NumExcept (ExcMoveResize, True, False, False, wi);
		  break;
	       case KWD_NoSelect:
		  NumExcept (ExcNoSelect, True, False, False, wi);
		  /* end case c */
		  break;
	       case KWD_NoSpellCheck:
		  NumExcept (ExcNoSpellCheck, True, False, False, wi);
		  break;
	       case KWD_GraphCreation:
		  /* GraphCreation */
		  NumExcept (ExcGraphCreation, True, False, False, wi);
		  break;
	       case KWD_Hidden:
		  /* Hidden */
		  NumExcept (ExcHidden, True, False, False, wi);
		  break;
	       case KWD_PageBreak:
		  NumExcept (ExcPageBreak, True, False, False, wi);
		  break;
	       case KWD_PageBreakAllowed:
		  NumExcept (ExcPageBreakAllowed, True, False, False, wi);
		  break;
	       case KWD_PageBreakPlace:
		  NumExcept (ExcPageBreakPlace, True, False, False, wi);
		  break;
	       case KWD_PageBreakRepetition:
		  NumExcept (ExcPageBreakRepetition, True, False, False, wi);
		  break;
	       case KWD_PageBreakRepBefore:
		  NumExcept (ExcPageBreakRepBefore, True, False, False, wi);
		  break;
	       case KWD_ActiveRef:
		  NumExcept (ExcActiveRef, False, True, False, wi);
		  break;
	       case KWD_NoPaginate:
		  NumExcept (ExcNoPaginate, True, False, False, wi);
		  break;
	       case KWD_ImportLine:
		  if (ImportExcept)
		     CompilerError (wi, STR, FATAL, STR_ONLY_ONE_IMPORT_EXCEPTION,
				    inputLine, linenb);
		  else
		    {
		       NumExcept (ExcImportLine, True, False, False, wi);
		       ImportExcept = True;
		    }
		  break;
	       case KWD_ImportParagraph:
		  if (ImportExcept)
		     CompilerError (wi, STR, FATAL, STR_ONLY_ONE_IMPORT_EXCEPTION,
				    inputLine, linenb);
		  else
		    {
		       NumExcept (ExcImportParagraph, True, False, False, wi);
		       ImportExcept = True;
		    }
		  break;
	       case KWD_HighlightChildren:
		  NumExcept (ExcHighlightChildren, True, False, False, wi);
		  break;
	       case KWD_ExtendedSelection:
		  NumExcept (ExcExtendedSelection, True, False, False, wi);
		  break;
	       case KWD_ParagraphBreak:
		  NumExcept (ExcParagraphBreak, True, False, False, wi);
		  break;
	       case KWD_Root:
		  CurRuleExtens = NouvRegleExtens (wi, 0);
		  CurRuleExtens->SrName[0] = '\0';
		  break;
		  /* end case c */
	    }

   else
      /* type de base */
      switch (c)
	    {
	       case 3001:
		  /* un nom */
		  switch (r)
			{
			      /* r = numero de la regle ou apparait le nom */
			   case RULE_ElemName /* ElemName */ :
			      if (pr == RULE_StructModel)
				 /* apres le mot-cle 'STRUCTURE' */
				 /* conserve le nom de la structure */
				{
				   copyword (pSchStr->SsName, wi, wl);
				   /* compare ce nom avec le nom du fichier */

				   if (strcmp (pSchStr->SsName, pfilename) != 0)
				      /* noms differents */
				      CompilerError (wi, STR, FATAL, STR_FILE_NAME_AND_STRUCT_NAME_DIFFERENT, inputLine, linenb);
				}
			      if (pr == RULE_Rule)
				 /* debut d'une regle du premier niveau */
				{
				   copyword (CurName, wi, wl);
				   CurNb = nb;
				   if (AnalParam)
				      CurParam = True;
				   else
				      CurParam = False;
				   if (AnalAssoc)
				      CurAssoc = True;
				   else
				      CurAssoc = False;
				   if (AnalUnits)
				      CurUnit = True;
				   else
				      CurUnit = False;
				}
			      if (pr == RULE_RootOrElem)
				 /* debut d'une regle d'extension */
				 CurRuleExtens = NouvRegleExtens (wi, wl);
			      if ((pr == RULE_TypeRef && !ReferenceAttr) || pr == RULE_Element)
				{
				   if (pr == RULE_Element)
				      copyword (IdentPreced, wi, wl);
				   /* garde le nom du type au cas ou il serait suivi de extern */
				   if (Egal)
				     {
					newrule (wi);
					Egal = False;
				     }
				   if (!error)
				      if (Droite[NiveauR - 1])
					 /* identificateur en partie droite de regle */
					{
					   RightIdentifier (nb + MAX_BASIC_TYPE, wi);
					   copyword (NomTypeRef, wi, wl);
					   /* garde le nom du type */
					   /* reference' au cas ou il est defini dans un */
					   /* autre schema de structure. */
					   DebutNomTypeRef = wi;
					}
				      else
					 /* identificateur partie gauche de regle */
					{
					   copyword (CurName, wi, wl);
					   CurNb = nb;
					}
				}
			      if (pr == RULE_TypeRef && ReferenceAttr)
				 /* dans un attribut reference */
				{
				   pSchStr->SsAttribute[pSchStr->SsNAttributes - 1].AttrTypeRef =
				      nb + MAX_BASIC_TYPE;
				   copyword (NomTypeRef, wi, wl);
				   /* garde le nom du type reference' au cas ou il est defini */
				   /* dans un autre schema de structure. */
				   DebutNomTypeRef = wi;
				}
			      if (pr == RULE_ExtStruct)
				 /* nom d'une structure externe dans une reference ou */
				 /* dans un contenu d'element exporte' */
				{
				   copyword (N, wi, wl);
				   /* recupere le nom du schema externe */
				   /* lit le schema de structure externe */
				   if (!RdSchStruct (N, pSchStrExt))
				      CompilerError (wi, STR, FATAL, STR_CANNOT_READ_STRUCT_SCHEM, inputLine, linenb);
				   /* echec lecture du schema */
				   else
				      /* le schema de structure a ete charge' */
				      /* le type reference' existe-t-il dans ce schema? */
				     {
					i = 0;
					while (strcmp (NomTypeRef, pSchStrExt->SsRule[i].SrName) != 0
					&& i - 1 < pSchStrExt->SsNRules)
					   i++;
					if (strcmp (NomTypeRef, pSchStrExt->SsRule[i].SrName) != 0)
					   CompilerError (DebutNomTypeRef, STR, FATAL, STR_TYPE_UNKNOWN, inputLine, linenb);
					/* type inconnu */
					else
					   /* le type reference' existe, il a le numero i+1 */
					   switch (ContexteStrExt)
						 {
						    case AttrRef:
						       /* dans un attribut reference */
						       pAttr = &pSchStr->SsAttribute[pSchStr->SsNAttributes - 1];
						       if (SecondDePaire)
							  pAttr->AttrTypeRef = i + 2;
						       else
							  pAttr->AttrTypeRef = i + 1;
						       strncpy (pAttr->AttrTypeRefNature, N, MAX_NAME_LENGTH);
						       PremierDePaire = False;
						       SecondDePaire = False;
						       break;
						    case RegleRef:
						       /* dans une construction CsReference */
						       pRule = &pSchStr->SsRule[CurRule[NiveauR - 1] - 1];
						       if (SecondDePaire)
							  pRule->SrReferredType = i + 2;
						       else
							  pRule->SrReferredType = i + 1;
						       strncpy (pRule->SrRefTypeNat, N, MAX_NAME_LENGTH);
						       PremierDePaire = False;
						       SecondDePaire = False;
						       break;
						    case Contenu:
						       /* dans un contenu d'elem exporte' */
						       pRule = &pSchStr->SsRule[RegleContenu - 1];
						       pRule->SrExportContent = i + 1;
						       strncpy (pRule->SrNatExpContent, N, MAX_NAME_LENGTH);
						       ContenuInconnu = False;
						       if (pRule->SrConstruct == CsChoice)
							  if (pRule->SrNChoices >= 1)
							     for (j = 0; j < pRule->SrNChoices; j++)
							       {
								  pSchStr->SsRule[pRule->SrChoice[j] - 1].SrExportContent = i + 1;
								  strncpy (pSchStr->SsRule[pRule->
											       SrChoice[j] - 1].SrNatExpContent, N, MAX_NAME_LENGTH);
							       }
						       break;
						 }

				     }
				}
			      if (pr == RULE_Export)
				 /* un nom d'element exporte' */
				 /* verifie que ce type est deja declare' */
				{
				   i = RuleNumber (wl, wi);
				   if (i == 0)
				      /* type non declare' */
				      CompilerError (wi, STR, FATAL, STR_TYPE_UNKNOWN, inputLine, linenb);
				   else
				     {
					pRule = &pSchStr->SsRule[i - 1];
					/* le type existe */
					pRule->SrExportedElem = True;
					RegleContenu = i;
					pRule->SrExportContent = i;
					/* par defaut on met tout le contenu */
					pRule->SrNatExpContent[0] = '\0';
					ContexteStrExt = Contenu;
					/* si c'est un choix qui est exporte', toutes */
					/* ses options sont egalement exportees */
					if (pRule->SrConstruct == CsChoice)
					   if (pRule->SrNChoices >= 1)
					      for (j = 0; j < pRule->SrNChoices; j++)
						{
						   pSchStr->SsRule[pRule->SrChoice[j] - 1].SrExportedElem = True;
						   pSchStr->SsRule[pRule->SrChoice[j] - 1].SrExportContent = pRule->SrChoice[j];
						   pSchStr->SsRule[pRule->SrChoice[j] - 1].SrNatExpContent[0] = '\0';
						}
					/* si c'est une marque de debut de paire qui est */
					/* exporte'e, la marque de fin est egalement exportee */
					if (pRule->SrConstruct == CsPairedElement)
					   if (pRule->SrFirstOfPair)
					     {
						pSchStr->SsRule[i].SrExportedElem = True;
						pSchStr->SsRule[i].SrExportContent = i + 1;
						pSchStr->SsRule[i].SrNatExpContent[0] = '\0';
					     }
				     }
				}
			      if (pr == RULE_Content)
				 /* le contenu d'un element exporte' */
				{
				   copyword (NomTypeRef, wi, wl);
				   /* garde le nom du type */
				   /* exporte' au cas ou il est defini dans un */
				   /* autre schema de structure. */
				   DebutNomTypeRef = wi;
				   i = RuleNumber (wl, wi);
				   if (i == 0)
				      /* type inconnu */
				      /* le contenu est peut-etre defini dans un autre */
				      /* schema, attendons de voir si un schema externe */
				      /* est indique' apres */
				      ContenuInconnu = True;
				   else
				      /* c'est un type defini dans le schema */
				   if (pSchStr->SsRule[i - 1].SrConstruct != CsNatureSchema)
				     {
					pRule = &pSchStr->SsRule[RegleContenu - 1];
					pRule->SrExportContent = i;
					if (pRule->SrConstruct == CsChoice)
					   if (pRule->SrNChoices >= 1)
					      for (j = 0; j < pRule->SrNChoices; j++)
						 pSchStr->SsRule[pRule->SrChoice[j] - 1].
						    SrExportContent = i;
					/* si c'est une marque de debut de paire qui est */
					/* exporte'e, la marque de fin est egalement traitee */
					if (pRule->SrConstruct == CsPairedElement)
					   if (pRule->SrFirstOfPair)
					      if (i == RegleContenu)
						 /* le contenu de la marque de fin est la marque */
						 /* de fin elle-meme */
						 pSchStr->SsRule[RegleContenu].SrExportContent = i + 1;
					      else
						 pSchStr->SsRule[RegleContenu].SrExportContent = i;
				     }
				   else
				      /* le contenu est un objet construit selon un */
				      /* autre schema de structure, on lit ce schema */
				   if (!RdSchStruct (pSchStr->SsRule[i - 1].SrName, pSchStrExt))
				      CompilerError (wi, STR, FATAL, STR_CANNOT_READ_STRUCT_SCHEM, inputLine, linenb);
				   /* echec lecture du schema */
				   else
				      /* le schema de structure a ete charge', le */
				      /* *contenu est l'element racine de ce schema */
				     {
					pRule = &pSchStr->SsRule[RegleContenu - 1];
					pRule->SrExportContent = pSchStrExt->SsRootElem;
					strncpy (pRule->SrNatExpContent, pSchStrExt->SsName, MAX_NAME_LENGTH);
					if (pRule->SrConstruct == CsChoice)
					   if (pRule->SrNChoices >= 1)
					      for (j = 0; j < pRule->SrNChoices; j++)
						{
						   pSchStr->SsRule[pRule->SrChoice[j] - 1].
						      SrExportContent = pSchStrExt->SsRootElem;
						   strncpy (pSchStr->
							    SsRule[pRule->SrChoice[j] - 1].SrNatExpContent,
							    pSchStrExt->SsName, MAX_NAME_LENGTH);
						}
				     }
				}
			      if (pr == RULE_ExceptType)
				 /* Un nom de type d'element dans les exceptions */
				 if (ExceptTypeExtern)
				   {
				      ExceptTypeExtern = False;
				      /*cherche s'il existe deja une regle d'extension pour ce type */
				      CurRuleExtens = ChercheRegleExtens (wi, wl);
				      if (CurRuleExtens == NULL)
					 /* il n'en existe pas, on cree une regle d'extension */
					 CurRuleExtens = NouvRegleExtens (wi, wl);
				      TypeExcept = 1;
				   }
				 else
				   {
				      TypeExcept = RuleNumber (wl, wi);
				      if (TypeExcept == 0)
					 /* ce n'est pas un type declare' dans le schema */
					 /* c'est peut etre un attribut */
					{
					   AttrExcept = AttributeNumber (wl, wi);
					   if (AttrExcept == 0)
					      CompilerError (wi, STR, FATAL, STR_TYPE_OR_ATTR_UNKNOWN, inputLine, linenb);
					   else if (pSchStr->SsAttribute[AttrExcept - 1].AttrFirstExcept != 0)
					      CompilerError (wi, STR, FATAL, STR_THIS_ATTR_ALREADY_HAS_EXCEPTS, inputLine, linenb);
					}
				      else
					 /* c'est un type declare' */
					{
					   if (PremierDePaire || SecondDePaire)
					      /* le nom d'element est precede' du mot-cle First ou Second */
					      if (pSchStr->SsRule[TypeExcept - 1].SrConstruct != CsPairedElement)
						 /* ce n'est pas une paire */
						 CompilerError (wi, STR, FATAL, STR_FIRST_SECOND_FORBIDDEN, inputLine, linenb);
					      else if (SecondDePaire)
						 TypeExcept++;
					   if (pSchStr->SsRule[TypeExcept - 1].SrFirstExcept != 0)
					      CompilerError (wi, STR, FATAL, STR_THIS_TYPE_ALREADY_HAS_EXCEPTS, inputLine, linenb);
					}
				      PremierDePaire = False;
				      SecondDePaire = False;
				   }
			      if (pr == RULE_InclElem)
				{
				   if (AnalExtens)
				      pRule = CurRuleExtens;
				   else
				      pRule = &pSchStr->SsRule[CurRule[NiveauR - 1] - 1];
				   if (pRule->SrNInclusions >= MAX_INCL_EXCL_SRULE)
				      CompilerError (wi, STR, FATAL, STR_TOO_MANY_INCLS_FOR_THAT_ELEM, inputLine, linenb);
				   else
				     {
					pRule->SrNInclusions++;
					pRule->SrInclusion[pRule->SrNInclusions - 1] = nb + MAX_BASIC_TYPE;
				     }
				}
			      if (pr == RULE_ExclElem)
				{
				   if (AnalExtens)
				      pRule = CurRuleExtens;
				   else
				      pRule = &pSchStr->SsRule[CurRule[NiveauR - 1] - 1];
				   if (pRule->SrNExclusions >= MAX_INCL_EXCL_SRULE)
				      CompilerError (wi, STR, FATAL, STR_TOO_MANY_EXCLUSIONS_FOR_THAT_ELEM, inputLine, linenb);
				   else
				     {
					pRule->SrNExclusions++;
					pRule->SrExclusion[pRule->SrNExclusions - 1] = nb + MAX_BASIC_TYPE;
				     }
				}
			      break;
			   case RULE_PresentName:
			      /* PresentName */
			      copyword (pSchStr->SsDefaultPSchema, wi, wl);
			      break;
			      case RULE_AttrName
			   /* AttrName */ :
			      numAttr = identtable[nb - 1].identdef;
			      if (numAttr == 0)
				 /* ce nom n'a pas encore ete rencontre' dans le schema */
				 if (strncmp (&inputLine[wi - 1], pSchStr->SsAttribute[0].AttrName, wl) == 0)
				    /* c'est l'attribut Langue */
				    numAttr = 1;
			      if (AnalAttr || AnalAttrLoc)
				 /* une declaration d'attribut */
				 if (AnalAttr && numAttr > 0)
				    CompilerError (wi, STR, FATAL, STR_ATTR_ALREADY_DECLARED, inputLine, linenb);
			      /* deja defini */
				 else
				   {
				      if (numAttr > 0)
					{
					   /* l'attribut est deja defini */
					   if (pSchStr->SsAttribute[numAttr - 1].AttrGlobal)
					      /* un attribut global ne peut pas etre */
					      /* utilise' comme attribut local */
					      CompilerError (wi, STR, FATAL, STR_GLOBAL_ATTR, inputLine, linenb);
					}
				      else
					 /* nouvel attribut */
				      if (pSchStr->SsNAttributes >= MAX_ATTR_SSCHEMA)
					 CompilerError (wi, STR, FATAL, STR_TOO_MANY_ATTRS, inputLine, linenb);
				      /* table des attributs saturee */
				      else
					{
					   pSchStr->SsNAttributes++;
					   identtable[nb - 1].identdef = pSchStr->SsNAttributes;
					   pAttr = &pSchStr->SsAttribute[pSchStr->SsNAttributes - 1];
					   if (AnalAttrLoc)
					      /* attribut local */
					      pAttr->AttrGlobal = False;
					   copyword (pAttr->AttrName, wi, wl);
					   pAttr->AttrType = AtEnumAttr;
					   pAttr->AttrNEnumValues = 0;
					}
				      if (AnalAttrLoc)	/* un attribut local */
					 if (CurNbAttrLoc >= MAX_LOCAL_ATTR)
					    /* trop d'attributs locaux pour cet element */
					    CompilerError (wi, STR, FATAL, STR_TOO_MANY_ATTRS, inputLine, linenb);
					 else if (AnalExtens)
					    /* dans une regle d'extension */
					   {
					      CurRuleExtens->SrNLocalAttrs++;
					      CurRuleExtens->SrLocalAttr[CurRuleExtens->SrNLocalAttrs - 1] = pSchStr->SsNAttributes;
					      CurRuleExtens->SrRequiredAttr[CurRuleExtens->SrNLocalAttrs - 1] = AttrObligatoire;
					      AttrObligatoire = False;
					   }
					 else
					    /* dans une regle de structure */
					   {
					      CurNbAttrLoc++;
					      CurAttrLoc[CurNbAttrLoc - 1] = identtable[nb - 1].
						 identdef;
					      CurAttrReq[CurNbAttrLoc - 1] = AttrObligatoire;
					      AttrObligatoire = False;
					   }
				   }
			      else if (DansExcept)
				 /* un nom d'attribut dans les exceptions */
				{
				   AttrExcept = AttributeNumber (wl, wi);
				   /* numero de cet attribut */
				   if (AttrExcept == 0)
				      /* ce n'est pas un attribut declare' dans le */
				      /* schema */
				      /* c'est peut etre un type d'element */
				     {
					TypeExcept = RuleNumber (wl, wi);
					if (TypeExcept == 0)
					   CompilerError (wi, STR, FATAL, STR_TYPE_OR_ATTR_UNKNOWN, inputLine, linenb);
					else if (pSchStr->SsRule[TypeExcept - 1].SrFirstExcept != 0)
					   CompilerError (wi, STR, FATAL, STR_THIS_TYPE_ALREADY_HAS_EXCEPTS, inputLine, linenb);
				     }
				   else if (pSchStr->SsAttribute[AttrExcept - 1].AttrFirstExcept != 0)
				      CompilerError (wi, STR, FATAL, STR_THIS_ATTR_ALREADY_HAS_EXCEPTS, inputLine, linenb);
				}
			      else
				 /* utilisation d'attribut dans une regle avec WITH */
			      if (numAttr == 0)
				 CompilerError (wi, STR, FATAL, STR_ATTR_NOT_DECLARED, inputLine, linenb);
			      else
				{
				   if (AnalExtens)
				      pRule = CurRuleExtens;
				   else
				      pRule = &pSchStr->SsRule[CurRule[NiveauR - 1] - 1];
				   if (pRule->SrNDefAttrs >= MAX_DEFAULT_ATTR)
				      CompilerError (wi, STR, FATAL, STR_TOO_MANY_ATTRS, inputLine, linenb);
				   else
				     {
					pRule->SrNDefAttrs++;
					pRule->SrDefAttr[pRule->SrNDefAttrs - 1] = numAttr;
					pRule->SrDefAttrValue[pRule->SrNDefAttrs - 1] = 1;
					/* par defaut la valeur initiale n'est pas modifiable */
					pRule->SrDefAttrModif[pRule->SrNDefAttrs - 1] = False;
				     }
				}
			      break;
			   case RULE_AttrValue:
			      /* AttrValue */
			      if (AnalAttr || AnalAttrLoc)
				 /* definition d'une valeur d'attribut pour l'attribut */
				 /* courant, qui est pour l'instant le dernier */
				{
				   pAttr = &pSchStr->SsAttribute[pSchStr->SsNAttributes - 1];
				   if (pAttr->AttrNEnumValues >= MAX_ATTR_VAL)
				      CompilerError (wi, STR, FATAL, STR_TOO_MANY_VALUES, inputLine, linenb);
				   /* la liste des valeurs deborde */
				   else
				      /* une valeur de plus pour l'attribut */
				     {
					pAttr->AttrNEnumValues++;
					/* range la valeur dans la liste des valeurs de */
					/* l'attribut */
					copyword (pAttr->AttrEnumValue[pAttr->AttrNEnumValues - 1], wi, wl);
					/* cette valeur est-elle deja dans la liste ? */
					i = 1;
					while (i < pAttr->AttrNEnumValues && !error)
					   if (!strcmp (pAttr->AttrEnumValue[i - 1], pAttr->AttrEnumValue[pAttr->AttrNEnumValues - 1]))
					      CompilerError (wi, STR, FATAL, STR_VALUE_ALREADY_DECLARED, inputLine, linenb);
					/* deux fois la meme valeur */
					   else
					      i++;
					if (!error)
					   /* c'est bien une nouvelle valeur, on l'accepte */
					  {
					     identtable[nb - 1].identdef = pAttr->AttrNEnumValues;
					     identtable[nb - 1].identref = pSchStr->SsNAttributes;
					  }
				     }
				}
			      else
				 /* utilisation dans une regle avec With */
			      if (identtable[nb - 1].identdef == 0)
				 CompilerError (wi, STR, FATAL, STR_ATTR_VALUE_NOT_DECLARED,
						inputLine, linenb);
			      else
				{
				   if (AnalExtens)
				      pRule = CurRuleExtens;
				   else
				      pRule = &pSchStr->SsRule[CurRule[NiveauR - 1] - 1];
				   pAttr = &pSchStr->SsAttribute[pRule->SrDefAttr[pRule->SrNDefAttrs - 1] - 1];
				   if (pAttr->AttrType != AtEnumAttr)
				      CompilerError (wi, STR, FATAL,
						     STR_INVALID_VALUE_FOR_THAT_ATTR, inputLine, linenb);
				   else
				     {
					/* cette valeur est-elle dans la liste des valeurs de */
					/* l'attribut ? */
					i = 1;
					ok = False;
					copyword (N, wi, wl);
					while (i <= pAttr->AttrNEnumValues && !ok)
					   if (strcmp (N, pAttr->AttrEnumValue[i - 1]) == 0)
					      ok = True;
					   else
					      i++;
					if (!ok)
					   CompilerError (wi, STR, FATAL, STR_INVALID_VALUE_FOR_THAT_ATTR, inputLine, linenb);
					else
					   pRule->SrDefAttrValue[pRule->SrNDefAttrs - 1] = i;
				     }
				}
			      break;
			   case RULE_ConstName /* ConstName */ :
			      if (!Regles)
				 /* definition de constante */
				{
				   copyword (CurName, wi, wl);
				   CurNb = nb;
				   CurParam = False;
				   CurAssoc = False;
				   CurUnit = False;
				   newrule (wi);
				   /* cree une regle */
				   if (!error)
				     {
					pRule = &pSchStr->SsRule[CurRule[NiveauR - 1] - 1];
					pRule->SrConstruct = CsConstant;
					pRule->SrIndexConst = PtrConsText;
				     }
				}
			      else
				 /* utilisation d'une constante dans une regle */
			      if (identtable[nb - 1].identdef == 0)
				 /* non defini, c'est une erreur */
				 CompilerError (wi, STR, FATAL, STR_CONSTANT_NOT_DECLARED, inputLine, linenb);
			      else if (pr == RULE_Constr)
				 /* dans une regle de structuration */
				 basicelem (nb + MAX_BASIC_TYPE, wi, pr);
			      else if (pr == RULE_ExceptType)
				 /* Un nom de constante dans les exceptions */
				 TypeExcept = RuleNumber (wl, wi);
			      if (pSchStr->SsRule[TypeExcept - 1].SrFirstExcept != 0)
				 CompilerError (wi, STR, FATAL, STR_THIS_TYPE_ALREADY_HAS_EXCEPTS, inputLine, linenb);
			      break;
			   default:
			      break;
			}
		  break;
	       case 3002:
		  /* un nombre */
		  nombre = trnb (wi, wl);
		  /* le nombre lu */
		  if (r == RULE_Integer)
		     /* nombre d'elements min. ou max. d'une liste */
		    {
		       pRule = &pSchStr->SsRule[CurRule[NiveauR - 1] - 1];
		       if (Minimum)
			  /* min */
			  pRule->SrMinItems = nombre;
		       else if (Maximum)
			  /* max */
			 {
			    pRule->SrMaxItems = nombre;
			    if (pRule->SrMaxItems < pRule->SrMinItems)
			       CompilerError (wi, STR, FATAL, STR_MAXIMUM_LOWER_THAN_MINIMUM, inputLine, linenb);
			 }
		    }
		  if (r == RULE_NumValue)
		     /* valeur d'un attribut fixe */
		    {
		       if (AnalExtens)
			  pRule = CurRuleExtens;
		       else
			  pRule = &pSchStr->SsRule[CurRule[NiveauR - 1] - 1];
		       if (pSchStr->SsAttribute[pRule->SrDefAttr[pRule->SrNDefAttrs - 1] - 1].AttrType != AtNumAttr)
			  CompilerError (wi, STR, FATAL, STR_INVALID_VALUE_FOR_THAT_ATTR, inputLine, linenb);
		       else
			 {
			    pRule->SrDefAttrValue[pRule->SrNDefAttrs - 1] = nombre * Signe;
			    /* a priori la prochaine valeur d'attribut sera positive */
			    Signe = 1;
			 }
		    }
		  if (r == RULE_ExceptNum)
		     /* un numero d'exception associe' a un type d'element */
		     /* ou a un attribut */
		     if (nombre <= 100)
			/* les valeurs inferieures a 100 sont reservees aux */
			/* exceptions predefinies */
			CompilerError (wi, STR, FATAL, STR_THIS_NUMBER_MUST_BE_GREATER_THAN_100, inputLine, linenb);
		     else
			NumExcept (nombre, False, False, False, wi);
		  break;
	       case 3003:
		  /* une chaine de caracteres */
		  if (r == RULE_ConstValue)
		     /* c'est le texte d'une constante */
		     storeConstantText (wi, wl);
		  else if (r == RULE_StrValue)
		     /* valeur fixe d'un attribut texte */
		    {
		       if (AnalExtens)
			  pRule = CurRuleExtens;
		       else
			  pRule = &pSchStr->SsRule[CurRule[NiveauR - 1] - 1];
		       if (pSchStr->SsAttribute[pRule->SrDefAttr[
			 pRule->SrNDefAttrs - 1] - 1].AttrType != AtTextAttr)
			  CompilerError (wi, STR, FATAL, STR_INVALID_VALUE_FOR_THAT_ATTR, inputLine, linenb);
		       else
			  pRule->SrDefAttrValue[pRule->SrNDefAttrs - 1] =
			     storeConstantText (wi, wl);
		    }
		  break;
	    }
}


/* ---------------------------------------------------------------------- */
/* |    typeexterne     marque comme externe dans le schema de          | */
/* |    structure tous les elements qui sont dans la table des types    | */
/* |    externes.                                                       | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         typeexterne ()

#else  /* __STDC__ */
static void         typeexterne ()
#endif				/* __STDC__ */

{
   int                 i, j;
   SRule              *pRule;

   /* parcourt la table des types externes */
   for (j = 0; j < NbTypeExternes; j++)
      /* cherche le type externe dans le schema a partir de la fin */
     {
	i = pSchStr->SsNRules - 1;
	while (strcmp (TypeExterne[j], pSchStr->SsRule[i].SrName) != 0)
	   i--;
	/* le type externe est defini par la regle de numero i */
	pRule = &pSchStr->SsRule[i];
	if (pRule->SrConstruct != CsNatureSchema)
	   /* ce n'est pas une nature externe, erreur */
	  {
	     if (TypeExterneInclus[j])
		CompilerErrorString (0, STR, INFO, STR_CANNOT_BE_INCLUDED, inputLine, linenb, pRule->SrName);
	     else
		CompilerErrorString (0, STR, INFO, STR_CANNOT_BE_EXTERN, inputLine, linenb, pRule->SrName);
	  }
	else
	   /* transforme la regle CsNatureSchema */
	  {
	     if (!RdSchStruct (pRule->SrName, pSchStrExt))
	       {
		  /* echec lecture du schema externe */
		  CompilerErrorString (0, STR, INFO, STR_EXTERNAL_STRUCT_NOT_FOUND, inputLine, linenb, pRule->SrName);
		  /* meme si le schema externe n'existe pas, on transforme la regle */
		  pRule->SrReferredType = MAX_BASIC_TYPE + 1;
	       }
	     else
		pRule->SrReferredType = pSchStrExt->SsRootElem;
	     /* transforme la regle en regle CsReference */
	     strncpy (pRule->SrRefTypeNat, pRule->SrName, MAX_NAME_LENGTH);
	     pRule->SrRefImportedDoc = True;
	     pRule->SrConstruct = CsReference;
	  }
     }
}

/* ---------------------------------------------------------------------- */
/* |    checkRecursivity                                                | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         checkRecursivity (int r, int chemin[], int niveau, boolean busy[], boolean done[])

#else  /* __STDC__ */
static void         checkRecursivity (r, chemin, niveau, busy, done)
int         r;
int         chemin[];
int                 niveau;
boolean             busy[];
boolean             done[];

#endif /* __STDC__ */

{
   int                 m;
   SRule              *pRule;

   pRule = &pSchStr->SsRule[r - 1];
   /* Si l'element est deja connu, inutile de l'explorer */
   if (done[r])
      return;
   /* si l'element est `busy', c'est qu'il est recursif;
      Il est forcement dans la pile, et de plus tous les elements
      qui ont ete empiles apres lui sont egalement recursifs,
      donc on les marque */
   if (busy[r])
     {
	for (m = niveau - 1; m >= 0; m--)
	  {
	     pSchStr->SsRule[chemin[m] - 1].SrRecursive = True;
	     if (chemin[m] == r)
		break;
	  }
	return;
     }
   busy[r] = True;
   chemin[niveau] = r;
   switch (pRule->SrConstruct)
	 {
	    case CsNatureSchema:
	    case CsBasicElement:
	    case CsReference:
	    case CsConstant:
	    case CsPairedElement:
	       break;
	    case CsChoice:
	       for (m = 0; m < pRule->SrNChoices; m++)
		  checkRecursivity (pRule->SrChoice[m],
				    chemin, niveau + 1, busy, done);
	       break;
	    case CsIdentity:
	       checkRecursivity (pRule->SrIdentRule,
				 chemin, niveau + 1, busy, done);
	       break;
	    case CsList:
	       checkRecursivity (pRule->SrListItem,
				 chemin, niveau + 1, busy, done);
	       break;
	    case CsUnorderedAggregate:
	    case CsAggregate:
	       for (m = 0; m < pRule->SrNComponents; m++)
		  checkRecursivity (pRule->SrComponent[m],
				    chemin, niveau + 1, busy, done);
	       break;
	    default:
	       break;
	 }
   busy[r] = False;
   /* l'element a ete explore, on ne le fera plus */
   done[r] = True;
}

/* ---------------------------------------------------------------------- */
/* |    chkrecurs                                                       | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         chkrecurs ()

#else  /* __STDC__ */
static void         chkrecurs ()
#endif				/* __STDC__ */

{
   int         i;
   int         path[100];
   boolean             busy[MAX_RULES_SSCHEMA + 1], done[MAX_RULES_SSCHEMA + 1];
   SRule              *pRule;

   for (i = MAX_BASIC_TYPE; i <= pSchStr->SsNRules; i++)
      busy[i] = done[i] = False;

   for (i = MAX_BASIC_TYPE + 1; i <= pSchStr->SsNRules; i++)
      checkRecursivity (i, path, 0, busy, done);

   for (i = MAX_BASIC_TYPE; i < pSchStr->SsNRules; i++)
     {
	pRule = &pSchStr->SsRule[i];
	if (pRule->SrRecursive)
	   TtaDisplaySimpleMessageString (STR, INFO, STR_RECURSIVE_ELEM, (char *) pRule->SrName);
     }
}

/* ---------------------------------------------------------------------- */
/* |    listassoc       liste les elements consideres comme parametres  | */
/* |            et elements associes                                    | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         listassoc ()

#else  /* __STDC__ */
static void         listassoc ()
#endif				/* __STDC__ */

{
  int	i;
  SRule	*pRule;
  
  /* parcourt toute la table des regles */
  for (i = MAX_BASIC_TYPE; i < pSchStr->SsNRules; i++)
    {
      if (pSchStr->SsRule[i].SrParamElem)
	/* affiche un message */
	    TtaDisplaySimpleMessageString(STR, INFO, STR_PARAMETER, (char *)pSchStr->SsRule[i].SrName);
      if (pSchStr->SsRule[i].SrAssocElem)
	if (!pSchStr->SsRule[i].SrRecursDone)
	  /* l'element associe est utilise dans une autre regle, erreur */
	  {
	    CompilerErrorString(0, STR, INFO, STR_THE_ASSOC_ELEM_IS_USED_IN_ANOTHER_RULE, inputLine, linenb, pSchStr->SsRule[i].SrName);
	    error = True;
	  } 
	else	
	  /* element associe correct */
	  /* On cree une nouvelle regle liste a la fin de la table des */
	  /* regles */
	  {
	    if (pSchStr->SsNRules >= MAX_RULES_SSCHEMA)
		TtaDisplaySimpleMessage(STR, FATAL, STR_TOO_MAN_RULES);
	    /* saturation de la table des regles */
	    else
	      pSchStr->SsNRules++;
	    pRule = &pSchStr->SsRule[pSchStr->SsNRules - 1];
	    /* la nouvelle regle liste prend le nom de ses elements... */
	    strncpy(pRule->SrName, pSchStr->SsRule[i].SrName, MAX_NAME_LENGTH-2);
	    /* ... suivi de 's' */
	    pRule->SrName[MAX_NAME_LENGTH-2] = '\0';
	    strcat(pRule->SrName, "s");
	    pRule->SrNDefAttrs = 0;
	    pRule->SrNLocalAttrs = 0;
	    pRule->SrNInclusions = 0;
	    pRule->SrNExclusions = 0;
	    pRule->SrRefImportedDoc = False;
	    pRule->SrAssocElem = False;
	    pRule->SrParamElem = False;
	    pRule->SrUnitElem = False;
	    pRule->SrRecursive = False;
	    pRule->SrExportedElem = False;
	    pRule->SrFirstExcept = 0;
	    pRule->SrLastExcept = 0;
	    pRule->SrConstruct = CsList;
	    pRule->SrListItem = i+1;
	    pRule->SrMinItems = 0;
	    pRule->SrMaxItems = 32000;
	    /* ecrit un message au terminal */
	    TtaDisplaySimpleMessageString(STR, INFO, STR_ASSOC_ELEMS, (char *)pRule->SrName);
	    if (NomRegleExiste())
		TtaDisplaySimpleMessage(STR, FATAL, STR_NAME_ALREADY_DECLARED);
	  }
	else
	   /* ce n'est pas un element associe */
	if (pSchStr->SsRule[i].SrRecursDone)
	   if (i + 1 != pSchStr->SsRootElem && !pSchStr->SsRule[i].SrUnitElem)
	     {
		if (pSchStr->SsRule[i].SrConstruct == CsChoice)
		   /* c'est un choix qui definit un alias */
		   /* on met l'element dans la table des alias */
		  {
		     NbAlias++;
		     Alias[NbAlias - 1] = i + 1;
		     TtaDisplaySimpleMessageString (STR, INFO, STR_ALIAS, (char *) pSchStr->SsRule[i].SrName);
		  }
		else
		   /* les regles de Fin de CsPairedElement ne sont jamais referencees */
		   if (pSchStr->SsRule[i].SrConstruct != CsPairedElement ||
		       pSchStr->SsRule[i].SrFirstOfPair)
		   /* c'est une definition inutile */
		   TtaDisplaySimpleMessageString (STR, INFO, STR_UNUSED, (char *) pSchStr->SsRule[i].SrName);
	     }
     }
}


/* ---------------------------------------------------------------------- */
/* |    listcrees       liste les types d'elements qui ne seront pas    | */
/* |    crees lors de l'edition d'un document ou d'un objet de cette    | */
/* |    classe.                                                         | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         listcrees ()

#else  /* __STDC__ */
static void         listcrees ()
#endif				/* __STDC__ */

{
   int         r, rr;
   int                 i;
   boolean             temp;
   SRule              *pRule;
   SRule              *pRule2;

   /* remet a zero dans toutes les regles l'indicateur de creation */
   /* (on utilise SrRecursDone comme indicateur de creation) */
   for (r = 0; r < pSchStr->SsNRules; r++)
      pSchStr->SsRule[r].SrRecursDone = False;
   /* on creera la racine */
   if (pSchStr->SsRootElem > 0)
      pSchStr->SsRule[pSchStr->SsRootElem - 1].SrRecursDone = True;
   /* parcourt toute la table des regles */
   for (r = 0; r < pSchStr->SsNRules; r++)
     {
	pRule = &pSchStr->SsRule[r];
	if (pRule->SrParamElem)
	   /* les parametres seront crees */
	   pRule->SrRecursDone = True;
	/* les elements inclus (au sens SGML) seront crees */
	if (pRule->SrNInclusions > 0)
	   for (i = 0; i < pRule->SrNInclusions; i++)
	      pSchStr->SsRule[pRule->SrInclusion[i] - 1].SrRecursDone = True;
	if (r < MAX_BASIC_TYPE)
	   pRule->SrRecursDone = True;
	switch (pRule->SrConstruct)
	      {
		 case CsNatureSchema:
		    /* les elements introduisant une nouvelle structure */
		    /* seront crees */
		    pRule->SrRecursDone = True;
		    break;
		 case CsBasicElement:
		    /* tous les elements de base seront crees */
		    pRule->SrRecursDone = True;
		    break;
		 case CsPairedElement:
		    /* toutes les marques par paires seront crees */
		    pRule->SrRecursDone = True;
		    break;
		 case CsReference:
		    /* les references seront crees */
		    pRule->SrRecursDone = True;
		    break;
		 case CsIdentity:
		    /* l'identite ne provoque pas de creation */
		    break;
		 case CsList:
		    /* les elements de liste seront crees */
		    pSchStr->SsRule[pRule->SrListItem - 1].SrRecursDone = True;
		    /* les listes d'elements associes seront crees */
		    if (pSchStr->SsRule[pRule->SrListItem - 1].SrAssocElem)
		       pRule->SrRecursDone = True;
		    break;
		 case CsChoice:
		    /* les elements de choix seront crees */
		    if (pRule->SrNChoices > 0)
		      {
			 for (i = 0; i < pRule->SrNChoices; i++)
			   {
			      pSchStr->SsRule[pRule->SrChoice[i] - 1].SrRecursDone = True;
			      /* si le choix est une unite exportee, ce sont */
			      /* ses options qui seront des unites exportees */
			      if (pRule->SrUnitElem)
				 pSchStr->SsRule[pRule->SrChoice[i] - 1].SrUnitElem = True;
			   }
			 pRule->SrUnitElem = False;
		      }
		    break;
		 case CsUnorderedAggregate:
		 case CsAggregate:
		    /* les composants d'agregats seront crees */
		    for (i = 0; i < pRule->SrNComponents; i++)
		       pSchStr->SsRule[pRule->SrComponent[i] - 1].SrRecursDone = True;
		    break;
		 case CsConstant:
		    /* les constantes seront crees */
		    pRule->SrRecursDone = True;
		    break;
		 default:
		    break;
	      }
     }
   /* ecrit le resultat */
   for (r = 0; r < pSchStr->SsNRules; r++)
     {
	pRule = &pSchStr->SsRule[r];
	if (pRule->SrConstruct == CsChoice)
	   /* tous les choix qui ne sont pas la regle racine, qui ne portent pas
	      d'exceptions SGML et qui n'appartiennent pas a un agregat sont crees
	      temporairement, jusqu'a ce qu'ils soient remplaces par une de leurs
	      valeurs possibles. Si le choix est un simple alias, l'element ne
	      sera jamais cree. */
	  {
	     temp = True;
	     if (r + 1 == pSchStr->SsRootElem)
		temp = False;
	     else if (pRule->SrNInclusions > 0 || pRule->SrNExclusions > 0)
		temp = False;
	     else
		for (rr = 0; rr < pSchStr->SsNRules; rr++)
		   if (pSchStr->SsRule[rr].SrConstruct == CsAggregate ||
		       pSchStr->SsRule[rr].SrConstruct == CsUnorderedAggregate)
		      for (i = 0; i < pSchStr->SsRule[rr].SrNComponents; i++)
			 if (pSchStr->SsRule[rr].SrComponent[i] == r + 1)
			    temp = False;
	     if (temp)
		/* est-ce un alias ? On cherche dans la table des alias */
		for (i = 0; i < NbAlias; i++)
		   if (Alias[i] == r + 1)
		      /* l'element est dans la table. Il ne sera pas cree */
		      temp = False;
	     if (temp)
		TtaDisplaySimpleMessageString (STR, INFO, STR_IS_A_TEMPORARY_ELEM, (char *) pRule->SrName);
	  }
	else if (!pRule->SrRecursDone)
	   /* les unites peuvent ne pas etre utilisees dans le schema */
	   if (!pRule->SrUnitElem)
	     {
		TtaDisplaySimpleMessageString (STR, INFO, STR_WON_T_BE_CREATED, (char *) pRule->SrName);
		/* cherche s'il y a des REFERENCES sur ce type d'element */
		for (rr = 0; rr < pSchStr->SsNRules; rr++)
		  {
		     pRule2 = &pSchStr->SsRule[rr];
		     if (pRule2->SrConstruct == CsReference)
			if (pRule2->SrRefTypeNat[0] == '\0')
			   if (pRule2->SrReferredType == r + 1)
			      TtaDisplaySimpleMessageString (STR, INFO, STR_WON_T_BE_CREATED_AND_IS_REFD, (char *) pRule->SrName);
		  }
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
   BinFile             infile;
   boolean             fileOK;
   char                cppFileName[200];
   iline               i;	/* position courante dans la ligne en cours */
   iline               wi;	/* position du debut du mot courant dans la

				   ligne en cours */
   iline               wl;	/* longueur du mot courant */
   nature              wn;	/* nature du mot courant */
   rnb                 r;	/* numero de regle */
   rnb                 pr;	/* numero de la regle precedente */
   grmcode             c;	/* code grammatical du mot trouve */
   int                 nb;	/* indice dans identtable du mot trouve, si

				   c'est un identificateur */

   TtaInitializeAppRegistry (argv[0]);
   COMPIL = TtaGetMessageTable ("compildialogue", COMPIL_MSG_MAX);
   STR = TtaGetMessageTable ("strdialogue", STR_MSG_MAX);
   error = False;
   /* initialise l'analyseur syntaxique */
   initsynt ();
   initgrm ("STRUCT.GRM");
   if (!error)
     {
	/* teste les arguments d'appel du programme */
	if (argc != 2)
	   TtaDisplaySimpleMessage (STR, FATAL, STR_NO_SUCH_FILE);
	else
	  {
	     /* recupere le nom du fichier a compiler */
	     strncpy (pfilename, argv[1], MAX_NAME_LENGTH - 1);
	     i = strlen (pfilename);
	     /* ajoute le suffixe .SCH */
	     strcat (pfilename, ".SCH");
	     /* teste si le fichier a compiler existe */
	     if (FileExist (pfilename) == 0)
		TtaDisplaySimpleMessage (STR, FATAL, STR_NO_SUCH_FILE);
	     else
		/* le fichier d'entree existe, on l'ouvre */
	       {
		  infile = BIOreadOpen (pfilename);
		  /* supprime le suffixe ".SCH" */
		  pfilename[i] = '\0';
		  /* acquiert la memoire pour le schema de structure */
		  if ((pSchStr = (PtrSSchema) malloc (sizeof (StructSchema))) == NULL)
		     TtaDisplaySimpleMessage (STR, FATAL, STR_NOT_ENOUGH_MEM);
		  /* memoire pour un schema de structure externe */
		  if ((pSchStrExt = (PtrSSchema) malloc (sizeof (StructSchema))) == NULL)
		     TtaDisplaySimpleMessage (STR, FATAL, STR_NOT_ENOUGH_MEM);

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
			  CompilerError (1, STR, FATAL, STR_LINE_TOO_LONG,
					 inputLine, linenb);
		       else if (inputLine[0] == '#')
			  /* cette ligne contient une directive du preprocesseur */
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
				 getword (i, &wi, &wl, &wn);	/* mot suivant */
				 if (wi > 0)
				    /* on a trouve un mot */
				   {
				      /* on analyse le mot */
				      analword (wi, wl, wn, &c, &r, &nb, &pr);
				      if (!error)
					 /* on le traite */
					 generate (wi, wl, c, r, nb, pr);
				   }
			      }
			    while (!(wi == 0 || error));
			 }	/* il n'y a plus de mots */
		    }
		  if (!error)
		     termsynt ();	/* fin d'analyse */
		  if (!error)
		     /* met les bons numeros de regle si ca n'a pas deja ete fait */
		     /* lorsqu'on a rencontre' le mot-cle' EXPORT */
		     if (!pSchStr->SsExport)
			changerulenb ();
		  if (!error)
		     typeexterne ();
		  /* traite les noms de types declares comme extern */
		  if (!error)
		    {
		       /* cherche les regles recursives */
		       chkrecurs ();
		       /* liste les elements consideres comme
		          elements associes */
		       listassoc ();
		       /* liste les elements qui ne seront pas
		          crees par l'editeur */
		       listcrees ();

		       /* ecrit le schema compile' dans le fichier de sortie */
		       DirectorySchemas[0] = '\0';	/* utilise le directory courant */
		       if (!error)
			 {
			    strcat (pfilename, ".STR");
			    fileOK = WrSchStruct (pfilename, pSchStr, 0);
			    if (!fileOK)
			       TtaDisplaySimpleMessageString (STR, FATAL, STR_CANNOT_WRITE, pfilename);
			 }
		    }
		  free (pSchStr);
		  free (pSchStrExt);
		  BIOreadClose (infile);
	       }
	  }
     }
   fflush (stdout);
   TtaSaveAppRegistry ();
   exit (0);
}
