
/* -- Copyright (c) 1996 Inria -- All rights reserved -- */

/* ====================================================================== */
/* |                                                                    | */
/* |                           Projet THOT                              | */
/* |                                                                    | */
/* |    Le programme STR compile un schema de structure, contenu dans   | */
/* |    un fichier de type .S                                           | */
/* |    STR est dirige' par la grammaire du langage de structure        | */
/* |    contenue, sous forme codee, dans le fichier STRUCT.GRM.         | */
/* |    Ce fichier .GRM est produit par le programme GRM.               | */
/* |    STR produit un fichier de type .STR qui sera ensuite utilise'   | */
/* |    par l'editeur Thot.						| */
/* |                                                                    | */
/* ====================================================================== */

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

#define MAX_SRULE_RECURS 15	/* nombre maximuimum de niveaux de
				   definition de regles dans une regle */
#define MAX_EXTERNAL_TYPES 20	/* nombre max. de types de documents
				   externes */
typedef enum
  {
     AttrRef, ElemRef, ElContent
  }
ContStrExt;

#define EXPORT
#include "compil.var"
#include "environ.var"

#undef EXPORT
#define EXPORT extern
#include "analsynt.var"

int                 linenb;	/* compteur de lignes du fichier source */

static Name          srceFileName; /* nom du fichier a compiler */
static PtrSSchema pSSchema;	/* pointeur sur le schema de structure en cours
				   de construction */
static int          TextConstPtr;
				/* index courant dans le buffer des const */
static boolean      CompilAttr;	/* on est en train d'analyser les attributs
				   globaux */
static boolean      CompilLocAttr;	/* on est en train d'analyser des attributs

					   locaux */
static boolean      CompilParam;	/* on est en train d'analyser les parametres */
static boolean      CompilAssoc;	/* on analyse les elements associes */
static boolean      CompilUnits;	/* on analyse les unites exportees */
static boolean      RootRule;	/* on attend la regle racine */
static boolean      Rules;	/* on est dans les Rules */
static boolean      CompilExtens;	/* on est dans les regles d'extension */
static boolean      ExceptExternalType;	/* on a rencontre' "EXTERN" avant un nom */

				/* de type dans la section EXCEPT */
static SRule       *CurExtensRule;	/* regle d'extension en cours de traitement */
static boolean      Minimum;	/* nombre minimum d'elements d'une liste */
static boolean      Maximum;	/* nombre maximum d'elements d'une liste */
static boolean      RRight[MAX_SRULE_RECURS];	/* on est dans la partie droite de la regle */

static int          RecursLevel;	/* niveau de recursivite' */
static int  CurRule[MAX_SRULE_RECURS];		/* numero de la regle courante */
static Name          CurName;	/* symbole gauche de la derniere regle

				   rencontree */
static int          CurNum;	/* indice de ce nom dans la table des

				   identificateurs */
static int          CurNLocAttr;	/* nombre d'attributs locaux asocies a CurName */
static int CurLocAttr[MAX_LOCAL_ATTR];	/* les attributs  locaux

							   associes a CurName */
static boolean      CurReqAttr[MAX_LOCAL_ATTR];	/* les booleens 'Required' des

						   attributs locaux associes a CurName */
static boolean      CurParam;	/* la derniere regle rencontree est un param */
static boolean      CurAssoc;	/* la derniere regle rencontree est un associe' */
static boolean      CurUnit;	/* la derniere regle rencontree est une unite

				   exportee */
static boolean      Equal;	/* c'est une regle d'egalite */
static boolean      Option;	/* c'est un composant facultatif  d'agregat */
static boolean      MandatoryAttr;	/* c'est un attribut obligatoire */
static int          Sign;	/* -1 ou 1 selon le signe de la derniere valeur 

				 * d'attribut rencontree */
static PtrSSchema pExternSSchema;	/* pointeur sur le schema de structure externe */
static int  RuleExportWith;	/* element dont on traite le contenu exporte' */
static Name          ReferredTypeName;	/* dernier nom d'un type reference' */
static int          BeginReferredTypeName;	/* position de ce nom dans la ligne */
static ContStrExt   ExternalStructContext;	/* contexte ou est utilise' une structure externe */
static boolean      UnknownContent;	/* indique que le contenu d'un element exporte 

					   n'est pas declare dans le schema */
static Name          PreviousIdent;	/* Name du dernier identificateur de type rencontre' */
static int  PreviousRule;

static int          NExternalTypes;	/* nombre de types declares comme externes */
static Name          ExternalType[MAX_EXTERNAL_TYPES];	/* table des noms de types

							   declares comme externes */
static boolean      IncludedExternalType[MAX_EXTERNAL_TYPES];	/* table des noms de */

				/* types declares comme externes inclus */
static boolean      CompilExcept;	/* on est dans les declarations d'exceptions */
static int  ExceptType;	/* numero du type d'element dont on traite les exceptions */
static int ExceptAttr;	/* Numero de l'attribut dont on traite les exceptions */
static int          CurBasicElem;	/* numero du type de base courant */
static int          NAlias;	/* nombre d'alias definis dans le schema */
static int  Alias[MAX_RULES_SSCHEMA];		/* les regles du schema qui definissent

						   des alias */
static boolean      FirstInPair;	/* on a rencontre' le mot-cle "First" */
static boolean      SecondInPair;	/* on a rencontre' le mot-cle "Second" */
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
/* |    InitBasicType                                                  | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void         InitBasicType (SRule * pRule, char *name, BasicType typ)

#else  /* __STDC__ */
static void         InitBasicType (pRule, name, typ)
SRule              *pRule;
char               *name;
BasicType          typ;

#endif /* __STDC__ */

{
   strncpy (pRule->SrName, name, MAX_NAME_LENGTH);
   pRule->SrConstruct = CsBasicElement;
   pRule->SrBasicType = typ;
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
/* |    Initialize       prepare la generation : initialise le schema	| */
/* |    de structure en memoire.                                        | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         Initialize ()

#else  /* __STDC__ */
static void         Initialize ()
#endif				/* __STDC__ */

{
   int                 i;
   TtAttribute           *pAttr;
   SRule              *pRule;

   pSSchema->SsExtension = False;
   pSSchema->SsNExtensRules = 0;
   pSSchema->SsExtensBlock = NULL;
   pSSchema->SsRootElem = 0;
   CurExtensRule = NULL;
   pSSchema->SsNAttributes = 0;
   for (i = 0; i < MAX_ATTR_SSCHEMA; i++)
     {
	pAttr = &pSSchema->SsAttribute[i];
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
   pAttr = &pSSchema->SsAttribute[0];
   strncpy (pAttr->AttrName, "Langue", MAX_NAME_LENGTH);
   pAttr->AttrOrigName[0] = '\0';
   pAttr->AttrGlobal = True;
   pAttr->AttrType = AtTextAttr;
   pSSchema->SsNAttributes++;

   pSSchema->SsNRules = 0;
   /* les premieres regles de structure du schema sont celles qui */
   /* definissent les types de base */
   pRule = &pSSchema->SsRule[CharString];
   InitBasicType (pRule, "TEXT_UNIT", CharString);

   pRule = &pSSchema->SsRule[GraphicElem];
   InitBasicType (pRule, "GRAPHICS_UNIT", GraphicElem);

   pRule = &pSSchema->SsRule[Symbol];
   InitBasicType (pRule, "SYMBOL_UNIT", Symbol);

   pRule = &pSSchema->SsRule[Picture];
   InitBasicType (pRule, "PICTURE_UNIT", Picture);

   pRule = &pSSchema->SsRule[Refer];
   InitBasicType (pRule, "REFERENCE_UNIT", Refer);

   pRule = &pSSchema->SsRule[PageBreak];
   InitBasicType (pRule, "PAGE_BREAK", PageBreak);

   pRule = &pSSchema->SsRule[UnusedBasicType];
   InitBasicType (pRule, "UNUSED", UnusedBasicType);

   pSSchema->SsNRules = MAX_BASIC_TYPE;
   pSSchema->SsConstBuffer[0] = '\0';
   pSSchema->SsConstBuffer[1] = '\0';
   pSSchema->SsExport = False;
   pSSchema->SsNExceptions = 0;
   TextConstPtr = 1;
   CompilAttr = False;
   CompilLocAttr = False;
   CompilParam = False;
   CompilAssoc = False;
   CompilUnits = False;
   RootRule = False;
   Rules = False;
   CompilExtens = False;
   Minimum = False;
   Maximum = False;
   RecursLevel = 1;
   RRight[0] = False;

   CurRule[0] = pSSchema->SsNRules;
   CurName[0] = '\0';
   CurNum = 0;
   CurNLocAttr = 0;
   CurParam = False;
   CurAssoc = False;
   CurUnit = False;
   Equal = False;
   Sign = 1;
   NExternalTypes = 0;
   CompilExcept = False;
   ExceptType = 0;
   ExceptAttr = 0;
   CurBasicElem = 0;
   NAlias = 0;
   PreviousRule = 0;
   FirstInPair = False;
   SecondInPair = False;
   ImportExcept = False;
}


/* ---------------------------------------------------------------------- */
/* |    RuleNameExist  verifie si le nom de la derniere regle de la	| */
/* |    table des regles est deja utilise' dans une autre regle de	| */
/* |    la meme table des regles. Retourne Vrai si oui.			| */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static boolean      RuleNameExist ()

#else  /* __STDC__ */
static boolean      RuleNameExist ()
#endif				/* __STDC__ */

{
   int                 r;
   boolean             ret;
   Name                 name;

   /* valeur de retour de la fonction */
   ret = False;
   /* nom de la derniere regle */
   strncpy (name, pSSchema->SsRule[pSSchema->SsNRules - 1].SrName, MAX_NAME_LENGTH);
   if (name[0] != '\0')
     {
	/* numero de la regle a traiter */
	r = 0;
	do
	   ret = strcmp (name, pSSchema->SsRule[r++].SrName) == 0;
	while (!ret && r < pSSchema->SsNRules - 1);
     }
   return ret;
}


/* ---------------------------------------------------------------------- */
/* |    Undefined                                                       | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         Undefined (int n)

#else  /* __STDC__ */
static void         Undefined (n)
int                 n;

#endif /* __STDC__ */
{
   int                 j;
   identelem          *pIdent;
   SRule              *pRule;

   pIdent = &identtable[n - 1];
   TtaDisplayMessage (INFO, TtaGetMessage(STR, STR_EXTERNAL_STRUCT), pIdent->identname);
   if (pSSchema->SsNRules >= MAX_RULES_SSCHEMA)
      TtaDisplaySimpleMessage (FATAL, STR, STR_TOO_MAN_RULES);
   /* trop de regles */
   else
      /* ajoute une regle de nature a la fin du schema */
     {
	pSSchema->SsNRules++;
	pIdent->identdef = pSSchema->SsNRules;
	if (pIdent->identlg > MAX_NAME_LENGTH - 1)
	   TtaDisplaySimpleMessage (FATAL, STR, STR_WORD_TOO_LONG);
	else
	  {
	     pRule = &pSSchema->SsRule[pSSchema->SsNRules - 1];
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
	     if (RuleNameExist ())
		TtaDisplaySimpleMessage (FATAL, STR, STR_NAME_ALREADY_DECLARED);
	  }
     }
}


/* ---------------------------------------------------------------------- */
/* |    ChangeOneRule met dans une regle les vrais numeros de regles a	| */
/* |    la place des numeros d'identificateur. Les elements non definis	| */
/* |    sont considere's comme des structures externes (natures);       | */
/* |    les elements non reference's sont signale's comme des erreurs   | */
/* |    s'ils ne sont pas des elements associes.                        | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         ChangeOneRule (SRule * pRule)

#else  /* __STDC__ */
static void         ChangeOneRule (pRule)
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
		Undefined (pRule->SrInclusion[j] - MAX_BASIC_TYPE);
	     pRule->SrInclusion[j] =
		identtable[pRule->SrInclusion[j] - MAX_BASIC_TYPE - 1].identdef;
	  }
	else if (pRule->SrInclusion[j] < 0)
	   pRule->SrInclusion[j] = -pRule->SrInclusion[j];
	pSSchema->SsRule[pRule->SrInclusion[j] - 1].SrRecursDone = False;
     }
   /* traitement des exclusions */
   for (j = 0; j < pRule->SrNExclusions; j++)
     {
	if (pRule->SrExclusion[j] > MAX_BASIC_TYPE)
	  {
	     if (identtable[pRule->SrExclusion[j] - MAX_BASIC_TYPE - 1].identdef == 0)
		Undefined (pRule->SrExclusion[j] - MAX_BASIC_TYPE);
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
			  Undefined (pRule->SrReferredType - MAX_BASIC_TYPE);
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
		       Undefined (pRule->SrIdentRule - MAX_BASIC_TYPE);
		    pRule->SrIdentRule = identtable[pRule->SrIdentRule - MAX_BASIC_TYPE - 1].
		       identdef;
		 }
	       else if (pRule->SrIdentRule < 0)
		  pRule->SrIdentRule = -pRule->SrIdentRule;
	       pSSchema->SsRule[pRule->SrIdentRule - 1].SrRecursDone = False;
	       break;
	    case CsList:
	       if (pRule->SrListItem > MAX_BASIC_TYPE)
		 {
		    if (identtable[pRule->SrListItem - MAX_BASIC_TYPE - 1].identdef == 0)
		       Undefined (pRule->SrListItem - MAX_BASIC_TYPE);
		    pRule->SrListItem = identtable[pRule->SrListItem - MAX_BASIC_TYPE - 1].
		       identdef;
		 }
	       else if (pRule->SrListItem < 0)
		  pRule->SrListItem = -pRule->SrListItem;
	       pSSchema->SsRule[pRule->SrListItem - 1].SrRecursDone = False;
	       break;
	    case CsChoice:
	       if (pRule->SrNChoices > 0)
		  for (j = 0; j < pRule->SrNChoices; j++)
		    {
		       if (pRule->SrChoice[j] > MAX_BASIC_TYPE)
			 {
			    if (identtable[pRule->SrChoice[j] - MAX_BASIC_TYPE - 1].identdef == 0)
			       Undefined (pRule->SrChoice[j] - MAX_BASIC_TYPE);
			    pRule->SrChoice[j] =
			       identtable[pRule->SrChoice[j] - MAX_BASIC_TYPE - 1].identdef;
			 }
		       else if (pRule->SrChoice[j] < 0)
			  pRule->SrChoice[j] = -pRule->SrChoice[j];
		       pSSchema->SsRule[pRule->SrChoice[j] - 1].SrRecursDone = False;
		    }
	       break;
	    case CsUnorderedAggregate:
	    case CsAggregate:
	       for (j = 0; j < pRule->SrNComponents; j++)
		 {
		    if (pRule->SrComponent[j] > MAX_BASIC_TYPE)
		      {
			 if (identtable[pRule->SrComponent[j] - MAX_BASIC_TYPE - 1].identdef == 0)
			    Undefined (pRule->SrComponent[j] - MAX_BASIC_TYPE);
			 pRule->SrComponent[j] =
			    identtable[pRule->SrComponent[j] - MAX_BASIC_TYPE - 1].identdef;
		      }
		    else if (pRule->SrComponent[j] < 0)
		       pRule->SrComponent[j] = -pRule->SrComponent[j];
		    pSSchema->SsRule[pRule->SrComponent[j] - 1].SrRecursDone = False;
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
/* |    ChangeRules    met dans les regles les vrais numeros de regles  | */
/* |    a la place des numeros d'identificateur. Les elements non       | */
/* |    definis sont considere's comme des structures externes(natures);| */
/* |    les elements non reference's sont signale's comme des erreurs   | */
/* |    s'ils ne sont pas des elements associes.                        | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         ChangeRules ()

#else  /* __STDC__ */
static void         ChangeRules ()
#endif				/* __STDC__ */

{
   int         i;
   TtAttribute           *pAttr;

   /* utilise l'indicateur SrRecursDone, avec le sens `regle non utilisee' */
   for (i = MAX_BASIC_TYPE; i < pSSchema->SsNRules; i++)
      pSSchema->SsRule[i].SrRecursDone = True;
   /* parcourt toutes les regles des elements construits */
   for (i = MAX_BASIC_TYPE; i < pSSchema->SsNRules; i++)
      ChangeOneRule (&pSSchema->SsRule[i]);
   /* parcourt toutes les regles d'extension */
   for (i = 0; i < pSSchema->SsNExtensRules; i++)
      ChangeOneRule (&pSSchema->SsExtensBlock->EbExtensRule[i]);
   /* parcourt tous les attributs definis dans le schema de structure */
   for (i = 0; i < pSSchema->SsNAttributes; i++)
     {
	pAttr = &pSSchema->SsAttribute[i];
	/* on ne traite que les attributs reference */
	if (pAttr->AttrType == AtReferenceAttr)
	   /* on ne traite que les types definis dans le meme schema */
	   if (pAttr->AttrTypeRefNature[0] == '\0')
	      if (pAttr->AttrTypeRef > MAX_BASIC_TYPE)
		{
		   if (identtable[pAttr->AttrTypeRef - MAX_BASIC_TYPE - 1].identdef == 0)
		      Undefined (pAttr->AttrTypeRef - MAX_BASIC_TYPE);
		   pAttr->AttrTypeRef =
		      identtable[pAttr->AttrTypeRef - MAX_BASIC_TYPE - 1].identdef;
		}
	      else if (pAttr->AttrTypeRef < 0)
		 pAttr->AttrTypeRef = -pAttr->AttrTypeRef;
     }
}


/* ---------------------------------------------------------------------- */
/* |    CopyWord    copie dans n le mot traite'.                        | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         CopyWord (Name name, iline wi, iline wl)

#else  /* __STDC__ */
static void         CopyWord (name, wi, wl)
Name                 name;
iline               wi;
iline               wl;

#endif /* __STDC__ */

{
   if (wl > MAX_NAME_LENGTH - 1)
      CompilerError (wi, STR, FATAL, STR_WORD_TOO_LONG, inputLine, linenb);
   else
     {
	strncpy (name, &inputLine[wi - 1], wl);
	name[wl] = '\0';
     }
}


/* ---------------------------------------------------------------------- */
/* |    Push                                                            | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         Push (iline wi)

#else  /* __STDC__ */
static void         Push (wi)
iline               wi;

#endif /* __STDC__ */

{
   if (RecursLevel >= MAX_SRULE_RECURS)
      CompilerError (wi, STR, FATAL, STR_RULE_NESTING_TOO_DEEP, inputLine, linenb);
   else
     {
	RRight[RecursLevel] = False;
	CurRule[RecursLevel] = 0;
	RecursLevel++;
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

   if (PreviousRule > 0)
     {
	n = -PreviousRule;
	PreviousRule = 0;
     }
   pRule = &pSSchema->SsRule[CurRule[RecursLevel - 1] - 1];
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
/* |    NewRule                                                         | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         NewRule (iline wi)

#else  /* __STDC__ */
static void         NewRule (wi)
iline               wi;

#endif /* __STDC__ */

{
   int                 i;
   SRule              *pRule;

   if (pSSchema->SsNRules >= MAX_RULES_SSCHEMA)
      CompilerError (wi, STR, FATAL, STR_TOO_MAN_RULES, inputLine, linenb);
   else
      pSSchema->SsNRules++;
   if (CurNum > 0)		/* il y a un symbole gauche a cette regle */
      if (identtable[CurNum - 1].identdef > 0)
	 /* double definition */
	 CompilerError (wi, STR, FATAL, STR_NAME_ALREADY_DECLARED, inputLine, linenb);
   if (!error)
     {
	CurRule[RecursLevel - 1] = pSSchema->SsNRules;
	if (Rules && RecursLevel > 1)
	   /* la regle courante est referencee par la regle englobante */
	  {
	     RecursLevel--;
	     if (CurNum > 0)
		RightIdentifier (CurNum + MAX_BASIC_TYPE, wi);
	     else
		RightIdentifier (-CurRule[RecursLevel], wi);
	     RecursLevel++;
	  }
	if (CurNum > 0)
	   identtable[CurNum - 1].identdef = pSSchema->SsNRules;
	if (Rules)
	   RRight[RecursLevel - 1] = True;
	pRule = &pSSchema->SsRule[pSSchema->SsNRules - 1];
	strncpy (pRule->SrName, CurName, MAX_NAME_LENGTH);
	pRule->SrNDefAttrs = 0;
	if (pRule->SrNLocalAttrs > 0)
	   /* il y a deja des attributs locaux pour cet element */
	   CompilerError (wi, STR, FATAL, STR_THIS_ELEM_HAS_LOCAL_ATTRS, inputLine, linenb);
	else
	   pRule->SrNLocalAttrs = CurNLocAttr;
	for (i = 0; i < pRule->SrNLocalAttrs; i++)
	  {
	     pRule->SrLocalAttr[i] = CurLocAttr[i];
	     pRule->SrRequiredAttr[i] = CurReqAttr[i];
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
	if (RuleNameExist ())
	   CompilerError (wi, STR, FATAL, STR_NAME_ALREADY_DECLARED, inputLine, linenb);
	if (RootRule)
	   /* compare ce nom avec le nom du schema */
	  {
	     if (strcmp (pSSchema->SsName, CurName) == 0)
		/* c'est la racine du schema */
		pSSchema->SsRootElem = pSSchema->SsNRules;
	     else if (!pSSchema->SsExtension)
		CompilerError (wi, STR, FATAL, STR_FIRST_RULE_MUST_BE_THE_ROOT, inputLine, linenb);
	     RootRule = False;
	  }
	CurName[0] = '\0';
	CurNum = 0;
	CurNLocAttr = 0;
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
iline              wl;
iline              wi;

#endif /* __STDC__ */

{
   int                 RuleNum;
   Name                 N;
   boolean             ok;

   CopyWord (N, wi, wl);
   RuleNum = 0;
   do
      ok = strcmp (N, pSSchema->SsRule[RuleNum++].SrName) == 0;
   while (!ok && RuleNum < pSSchema->SsNRules);
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

   CopyWord (N, wi, wl);
   AttrNum = 0;
   do
      ok = strcmp (N, pSSchema->SsAttribute[AttrNum++].AttrName) == 0;
   while (!ok && AttrNum < pSSchema->SsNAttributes);
   if (!ok)
      AttrNum = 0;
   return AttrNum;
}


/* ---------------------------------------------------------------------- */
/* |    ExceptionNum    traite le numero d'exception Num. Si checkType  | */
/* |    est vrai, verifie que l'exception porte sur un type d'element.  | */
/* |    Si checkAttr est vrai, verifie que l'exception porte sur un     | */
/* |    attribut.                                                       | */
/* |    SiCheckIntAttr est vrai, verifie que l'exception porte sur un   | */
/* |    attribut numerique.                                             | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void         ExceptionNum (int Num, boolean checkType, boolean checkAttr, boolean CheckIntAttr, iline wi)

#else  /* __STDC__ */
static void         ExceptionNum (Num, checkType, checkAttr, CheckIntAttr, wi)
int                 Num;
boolean             checkType;
boolean             checkAttr;
boolean             CheckIntAttr;
iline               wi;

#endif /* __STDC__ */

{
   SRule              *pRule;
   TtAttribute           *pAttr;

   if (checkType && ExceptType == 0)
      CompilerError (wi, STR, FATAL, STR_ONLY_FOR_ELEMS, inputLine, linenb);
   if (checkAttr && ExceptAttr == 0)
      CompilerError (wi, STR, FATAL, STR_ONLY_FOR_ATTRS, inputLine, linenb);
   if (ExceptAttr > 0)
     {
	if (CheckIntAttr && pSSchema->SsAttribute[ExceptAttr - 1].AttrType != AtNumAttr)
	   CompilerError (wi, STR, FATAL, STR_ONLY_FOR_NUMERICAL_ATTRS, inputLine, linenb);
	if (Num == ExcActiveRef && pSSchema->SsAttribute[ExceptAttr - 1].AttrType != AtReferenceAttr)
	   CompilerError (wi, STR, FATAL, STR_ONLY_FOR_REFERENCE_ATTRS, inputLine, linenb);
     }
   if (!error)
     {
	if (pSSchema->SsNExceptions >= MAX_EXCEPT_SSCHEMA)
	   /* la liste des numeros d'exception est pleine */
	   CompilerError (wi, STR, FATAL, STR_TOO_MANY_EXCEPTS, inputLine, linenb);
	else
	   /* range le numero d'exception dans la liste des */
	   /* numeros d'exception du schema de structure */
	  {
	     pSSchema->SsNExceptions++;
	     pSSchema->SsException[pSSchema->SsNExceptions - 1] = Num;
	     if (ExceptType != 0)
	       {
		  if (CurExtensRule != NULL)
		     pRule = CurExtensRule;
		  else
		     pRule = &pSSchema->SsRule[ExceptType - 1];
		  if (pRule->SrFirstExcept == 0)
		     pRule->SrFirstExcept = pSSchema->SsNExceptions;
		  pRule->SrLastExcept = pSSchema->SsNExceptions;
	       }
	     else if (ExceptAttr != 0)
	       {
		  pAttr = &pSSchema->SsAttribute[ExceptAttr - 1];
		  if (pAttr->AttrFirstExcept == 0)
		     pAttr->AttrFirstExcept = pSSchema->SsNExceptions;
		  pAttr->AttrLastExcept = pSSchema->SsNExceptions;
	       }
	  }
     }
}


/* ---------------------------------------------------------------------- */
/* |    BasicEl                                                       | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         BasicEl (int n, iline wi, rnb pr)

#else  /* __STDC__ */
static void         BasicEl (n, wi, pr)
int                 n;
iline               wi;
rnb                 pr;

#endif /* __STDC__ */

{
   SRule              *pRule;

   pRule = NULL;
   if (pr == RULE_InclElem || pr == RULE_ExclElem)
      if (CompilExtens)
	 pRule = CurExtensRule;
      else
	 pRule = &pSSchema->SsRule[CurRule[RecursLevel - 1] - 1];
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
	ExceptType = n;
	if (pSSchema->SsRule[ExceptType - 1].SrFirstExcept != 0)
	   CompilerError (wi, STR, FATAL, STR_THIS_TYPE_ALREADY_HAS_EXCEPTS, inputLine, linenb);
     }
   else
     {
	if (Equal)
	  {
	     NewRule (wi);
	     Equal = False;
	  }
	if (!error)
	  {
	     if (RRight[RecursLevel - 1])
		RightIdentifier (n, wi);
	     else
	       {
		  RecursLevel--;
		  RightIdentifier (n, wi);
		  RecursLevel++;
	       }
	     CurBasicElem = n;
	  }
     }
}


/* ---------------------------------------------------------------------- */
/* |    Stocke le texte d'une constante chaine                          | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static int          StoreConstText (int wi, int wl)

#else  /* __STDC__ */
static int          StoreConstText (wi, wl)
int                 wi;
int                 wl;

#endif /* __STDC__ */

{
   int                 i, pos;

   pos = TextConstPtr;
   if (TextConstPtr + wl >= MAX_LEN_ALL_CONST)
      CompilerError (wi, STR, FATAL, STR_CONSTANT_BUFFER_FULL, inputLine, linenb);
   else
     {
	for (i = 0; i <= wl - 2; i++)
	   pSSchema->SsConstBuffer[TextConstPtr + i - 1] = inputLine[wi + i - 1];
	TextConstPtr += wl;
	/* un nul pour la fin de la constante */
	pSSchema->SsConstBuffer[TextConstPtr - 2] = '\0';
	/* un autre nul pour la fin de toutes les constantes */
	pSSchema->SsConstBuffer[TextConstPtr - 1] = '\0';
     }
   return pos;
}


/* ---------------------------------------------------------------------- */
/* |    InitRule       initialise une regle de structure                | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void         InitRule (SRule * pRule)

#else  /* __STDC__ */
static void         InitRule (pRule)
SRule              *pRule;

#endif /* __STDC__ */

{
   pRule->SrName[0] = '\0';
   pRule->SrNDefAttrs = 0;
   pRule->SrNLocalAttrs = 0;
   pRule->SrAssocElem = False;
   pRule->SrParamElem = False;
   pRule->SrUnitElem = False;
   pRule->SrRecursive = False;
   pRule->SrExportedElem = False;
   pRule->SrFirstExcept = 0;
   pRule->SrLastExcept = 0;
   pRule->SrNInclusions = 0;
   pRule->SrNExclusions = 0;
   pRule->SrRefImportedDoc = False;
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
   SRule              *newRule;
   SRule              *prevRule;

   prevRule = &pSSchema->SsRule[pSSchema->SsNRules - 1];
   if (prevRule->SrConstruct == CsPairedElement && prevRule->SrFirstOfPair)
     {
	/* ajoute une regle CsPairedElement a la fin du schema */
	pSSchema->SsNRules++;
	newRule = &pSSchema->SsRule[pSSchema->SsNRules - 1];
	InitRule (newRule);
	strncpy (newRule->SrName, prevRule->SrName, MAX_NAME_LENGTH);
	newRule->SrConstruct = CsPairedElement;
	newRule->SrFirstOfPair = False;
     }
}


/* ---------------------------------------------------------------------- */
/* |    GetExtensionRule      recherche une regle d'extension ayant le  | */
/* |                   nom (wi, wl)                                     | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static SRule       *GetExtensionRule (iline wi, iline wl)

#else  /* __STDC__ */
static SRule       *GetExtensionRule (wi, wl)
iline               wi;
iline               wl;

#endif /* __STDC__ */

{
   SRule              *pRule;
   Name                 n;
   int                 r;
   boolean             found;

   pRule = NULL;
   if (pSSchema->SsExtensBlock != NULL)
     {
	CopyWord (n, wi, wl);
	found = False;
	r = 0;
	while (!found && r < pSSchema->SsNExtensRules)
	  {
	     pRule = &pSSchema->SsExtensBlock->EbExtensRule[r];
	     if (strncmp (n, pRule->SrName, MAX_NAME_LENGTH) == 0)
		found = True;
	     else
		r++;
	  }
	if (!found)
	   pRule = NULL;
     }
   return pRule;
}


/* ---------------------------------------------------------------------- */
/* |    NewExtensionRule alloue et initialise une nouvelle regle         | */
/* |                    d'extension                                     | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static SRule       *NewExtensionRule (iline wi, iline wl)

#else  /* __STDC__ */
static SRule       *NewExtensionRule (wi, wl)
iline               wi;
iline               wl;

#endif /* __STDC__ */

{
   SRule              *pRule;

   pRule = NULL;
   if (GetExtensionRule (wi, wl) != NULL)
      CompilerError (wi, STR, FATAL, STR_NAME_ALREADY_DECLARED, inputLine, linenb);
   if (pSSchema->SsExtensBlock == NULL)
      if ((pSSchema->SsExtensBlock = (PtrExtensBlock) malloc (sizeof (ExtensBlock))) == NULL)
	 TtaDisplaySimpleMessage (FATAL, STR, STR_NOT_ENOUGH_MEM);
   if (pSSchema->SsExtensBlock != NULL)
      if (pSSchema->SsNExtensRules >= MAX_EXTENS_SSCHEMA)
	 CompilerError (wi, STR, FATAL, STR_TOO_MAN_RULES, inputLine, linenb);
      else
	{
	   pRule = &pSSchema->SsExtensBlock->EbExtensRule[pSSchema->SsNExtensRules];
	   pSSchema->SsNExtensRules++;
	   InitRule (pRule);
	   CopyWord (pRule->SrName, wi, wl);
	   pRule->SrConstruct = CsExtensionRule;
	}
   return pRule;
}


/* ---------------------------------------------------------------------- */
/* |    ProcessToken    traite le mot commencant a la position wi dans  | */
/* |    la ligne courante, de longueur wl et de code grammatical c.     | */
/* |    Si c'est un identificateur, nb contient son rang dans la table  | */
/* |    des identificateurs. r est le numero de la regle dans laquelle  | */
/* |    apparait ce mot.                                                | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         ProcessToken (iline wi, iline wl, grmcode c, grmcode r, int nb, rnb pr)

#else  /* __STDC__ */
static void         ProcessToken (wi, wl, c, r, nb, pr)
iline               wi;
iline               wl;
grmcode             c;
grmcode             r;
int                 nb;
rnb                 pr;

#endif /* __STDC__ */

{
   int                 number, i, j;
   Name                 N;
   TtAttribute           *pAttr;
   SRule              *pRule;
   boolean             ok;
   int                 attrNum;

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
		       if (CompilExtens)
			  /* dans une regle d'extension */
			  pAttr = &pSSchema->SsAttribute[CurExtensRule->SrLocalAttr[CurExtensRule->SrNLocalAttrs - 1] - 1];
		       else
			  /* dans une regle de structure */
			  pAttr = &pSSchema->SsAttribute[CurLocAttr[CurNLocAttr - 1] - 1];
		       if (pAttr->AttrType == AtEnumAttr && pAttr->AttrNEnumValues == 0)
			  /* pas de valeur definie */
			  CompilerError (wi, STR, FATAL, STR_ATTR_WITHOUT_VALUE, inputLine, linenb);
		    }
		  if (!(CompilAttr || CompilLocAttr))
		     if (r == RULE_RuleList || r == RULE_OptDefList || r == RULE_DefList)
			/* fin de regle de structure */
		       {
			  if (CurNum > 0)
			     /* le dernier nom rencontre' n'a pas ete traite' */
			    {
			       RecursLevel--;
			       /* il figure en partie droite de */
			       /* la regle de niveau inferieur */
			       RightIdentifier (CurNum + MAX_BASIC_TYPE, wi);
			       RecursLevel++;
			       CurNum = 0;
			       CurName[0] = '\0';
			       CurNLocAttr = 0;
			       CurParam = False;
			       CurAssoc = False;
			       CurUnit = False;
			    }
			  RRight[RecursLevel - 1] = False;	/* partie droite de regle terminee */

			  CurBasicElem = 0;
			  DupliqueReglePaire ();
		       }
		  if (r == RULE_ExpList)
		    {
		       /* fin d'un element exporte' */
		       if (UnknownContent)
			  CompilerError (BeginReferredTypeName, STR, FATAL, STR_TYPE_UNKNOWN, inputLine, linenb);
		       /* contenu invalide */
		    }

		  if (r == RULE_ExceptList)
		     /* fin des exceptions d'un type ou d'un attribut */
		    {
		       CurExtensRule = NULL;
		       ExceptType = 0;
		       ExceptAttr = 0;
		    }
		  break;
	       case CHR_61:
		  /*  =  */
		  if (r == RULE_Rule || r == RULE_ExtOrDef)
		     Equal = True;
		  else if (r == RULE_LocalAttr)
		     /* on va definir le type d'un attribut local */
		    {
		       pAttr = &pSSchema->SsAttribute[pSSchema->SsNAttributes - 1];
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
		       if (pSSchema->SsRule[CurRule[RecursLevel - 1] - 1].SrConstruct == CsList)
			  /* definition des elements d'une liste */
			  Push (wi);
		    }

		  if (r == RULE_LocAttrList)
		     /* debut des attributs locaux d'un type */
		    {
		       CompilLocAttr = True;
		       CurNLocAttr = 0;
		    }
		  break;
	       case CHR_41:
		  /*  )  */
		  if (r == RULE_LocAttrList)
		     /* fin des attributs locaux d'un type */
		    {
		       CompilLocAttr = False;
		       if (CompilExtens)
			  /* dans une regle d'extension */
			  pAttr = &pSSchema->SsAttribute[CurExtensRule->SrLocalAttr[CurExtensRule->SrNLocalAttrs - 1] - 1];
		       else
			  /* dans une regle de structure */
			  pAttr = &pSSchema->SsAttribute[CurLocAttr[CurNLocAttr - 1] - 1];
		       if (pAttr->AttrType == AtEnumAttr && pAttr->AttrNEnumValues == 0)
			  /* pas de valeur definie */
			  CompilerError (wi, STR, FATAL, STR_ATTR_WITHOUT_VALUE, inputLine, linenb);
		       if (CurBasicElem > 0)
			  /* fin des attributs locaux d'un element de base */
			 {
			    pRule = &pSSchema->SsRule[CurBasicElem - 1];
			    if (pRule->SrNLocalAttrs > 0)
			       /* il y a deja des attributs locaux pour cet element */
			       CompilerError (wi, STR, FATAL, STR_THIS_ELEM_HAS_LOCAL_ATTRS, inputLine, linenb);
			    else
			       pRule->SrNLocalAttrs = CurNLocAttr;
			    for (i = 0; i < pRule->SrNLocalAttrs; i++)
			      {
				 pRule->SrLocalAttr[i] = CurLocAttr[i];
				 pRule->SrRequiredAttr[i] = CurReqAttr[i];
			      }
			    CurBasicElem = 0;
			    CurNLocAttr = 0;
			 }
		    }
		  if (r == RULE_Constr)
		    {
		       CurBasicElem = 0;
		       if (RecursLevel > 1)
			  if (pSSchema->SsRule[CurRule[RecursLevel - 2] - 1].SrConstruct == CsList)
			     /* fin d'une regle LIST */
			     /* partie droite de regle terminee */
			    {
			       RRight[RecursLevel - 1] = False;

			       /* attributs fixes termines */
			       RecursLevel--;
			       if (CurNum > 0)
				 {
				    RightIdentifier (CurNum + MAX_BASIC_TYPE, wi);
				    CurNum = 0;
				    CurName[0] = '\0';
				    CurNLocAttr = 0;
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
		       if (FirstInPair || SecondInPair)
			  /* le nom d'element reference' est precede' de First ou Second */
			 {
			    /* cherche si le type reference' est deja defini */
			    i = 0;
			    do
			      {
				 ok = strcmp (ReferredTypeName, pSSchema->SsRule[i].SrName) == 0;
				 i++;	/* passe a la regle suivante */
			      }
			    while (!ok && i < pSSchema->SsNRules);
			    if (!ok)
			       CompilerError (BeginReferredTypeName, STR, FATAL, STR_TYPE_UNKNOWN, inputLine, linenb);
			    else if (pSSchema->SsRule[i - 1].SrConstruct != CsPairedElement)
			       /* ce n'est pas une paire */
			       CompilerError (BeginReferredTypeName, STR, FATAL, STR_FIRST_SECOND_FORBIDDEN, inputLine, linenb);
			    else if (SecondInPair)
			      {
				 if (r == RULE_AttrType)
				    pSSchema->SsAttribute[pSSchema->SsNAttributes - 1].
				       AttrTypeRef = -(i + 1);
				 else if (pSSchema->SsRule[CurRule[RecursLevel - 1] - 1].SrConstruct
					  == CsReference)
				    pSSchema->SsRule[CurRule[RecursLevel - 1] - 1].SrReferredType
				       = -(i + 1);
			      }
			 }
		       FirstInPair = False;
		       SecondInPair = False;
		    }
		  break;
	       case CHR_44:
		  /*  ,  */
		  if (r == RULE_ExpList)
		     /* fin d'un element exporte' */
		     if (UnknownContent)
			CompilerError (BeginReferredTypeName, STR, FATAL, STR_TYPE_UNKNOWN, inputLine, linenb);
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
		       if (CompilExtens)
			  pRule = CurExtensRule;
		       else
			  pRule = &pSSchema->SsRule[CurRule[RecursLevel - 1] - 1];
		       pRule->SrDefAttrModif[pRule->SrNDefAttrs - 1] = True;
		    }
		  else
		     Option = True;
		  break;
	       case CHR_33:
		  /*  !  */
		  MandatoryAttr = True;
		  break;
	       case CHR_42:
		  /*  *  */
		  if (r == RULE_min)
		     pSSchema->SsRule[CurRule[RecursLevel - 1] - 1].SrMinItems = 0;
		  else
		     pSSchema->SsRule[CurRule[RecursLevel - 1] - 1].SrMaxItems = 32000;
		  break;
	       case CHR_43:
		  /*  +  */
		  if (r == RULE_DefWith)
		     if (!RRight[RecursLevel - 1])
			CompilerError (wi, STR, FATAL, STR_GIVE_A_NAME_TO_THAT_ELEM, inputLine, linenb);
		  break;
	       case CHR_45:
		  /*  -  */
		  if (r == RULE_FixedValue)
		     /* signe negatif pour la valeur d'attribut qui suit */
		     Sign = -1;
		  else if (r == RULE_DefWith)
		     if (!RRight[RecursLevel - 1])
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
		  pSSchema->SsExtension = True;
		  pSSchema->SsNExtensRules = 0;
		  pSSchema->SsExtensBlock = NULL;
		  break;
	       case KWD_DEFPRES:
		  break;
	       case KWD_ATTR /* ATTR */ :
		  if (r == RULE_StructModel)
		     /* debut des attributs globaux */
		     CompilAttr = True;
		  else if (r == RULE_LocAttrList)
		     /* debut des attributs locaux d'un type d'element */
		     /* a priori, le prochain attribut local n'est pas obligatoire */
		     MandatoryAttr = False;
		  break;
	       case KWD_CONST:
		  CompilAttr = False;
		  break;
	       case KWD_PARAM:
		  if (pSSchema->SsExtension)
		     CompilerError (wi, STR, FATAL, STR_NOT_ALLOWED_IN_AN_EXTENSION, inputLine, linenb);
		  else
		    {
		       CompilParam = True;
		       Rules = True;
		       CompilAttr = False;
		    }
		  break;
	       case KWD_STRUCT:
		  /* la premiere regle sera la regle racine */
		  RootRule = True;
		  Rules = True;
		  CompilParam = False;
		  CompilAttr = False;
		  break;
	       case KWD_EXTENS:
		  /* verifie qu'on est bien dans une extension de schema */
		  if (!pSSchema->SsExtension)
		     CompilerError (wi, STR, FATAL, STR_NOT_AN_EXTENSION, inputLine, linenb);
		  else
		    {
		       CompilParam = False;
		       CompilAttr = False;
		       CompilExtens = True;
		    }
		  break;
	       case KWD_ASSOC:
		  if (pSSchema->SsRootElem == 0 && !pSSchema->SsExtension)
		     CompilerError (wi, STR, FATAL, STR_STRUCT_SECTION_MISSING, inputLine, linenb);
		  else
		    {
		       CompilParam = False;
		       CompilAttr = False;
		       CompilAssoc = True;
		       CompilExtens = False;
		    }
		  break;
	       case KWD_UNITS:
		  if (pSSchema->SsRootElem == 0 && !pSSchema->SsExtension)
		     CompilerError (wi, STR, FATAL, STR_STRUCT_SECTION_MISSING, inputLine, linenb);
		  else
		    {
		       CompilUnits = True;
		       CompilAssoc = False;
		       CompilParam = False;
		       CompilAttr = False;
		       CompilExtens = False;
		    }
		  break;
	       case KWD_EXPORT:
		  if (pSSchema->SsRootElem == 0 && !pSSchema->SsExtension)
		     CompilerError (wi, STR, FATAL, STR_STRUCT_SECTION_MISSING, inputLine, linenb);
		  else if (pSSchema->SsExtension)
		     CompilerError (wi, STR, FATAL, STR_NOT_ALLOWED_IN_AN_EXTENSION, inputLine, linenb);
		  else
		    {
		       CompilParam = False;
		       CompilAttr = False;
		       CompilExtens = False;
		       CompilAssoc = False;
		       CompilUnits = False;
		       ChangeRules ();
		       /* met les bons numeros de regle */
		       pSSchema->SsExport = True;
		    }
		  break;
	       case KWD_EXCEPT:
		  if (pSSchema->SsRootElem == 0 && !pSSchema->SsExtension)
		     CompilerError (wi, STR, FATAL, STR_STRUCT_SECTION_MISSING, inputLine, linenb);
		  else
		    {
		       CompilParam = False;
		       CompilAttr = False;
		       CompilExtens = False;
		       CompilAssoc = False;
		       CompilUnits = False;
		       CurExtensRule = NULL;
		       CompilExcept = True;
		       ExceptExternalType = False;
		    }
		  break;
	       case KWD_END:
		  if (r == RULE_Constr)
		    {
		       RecursLevel--;
		       if (CurNum > 0)
			 {
			    RightIdentifier (CurNum + MAX_BASIC_TYPE, wi);
			    CurNum = 0;
			    CurName[0] = '\0';
			    CurNLocAttr = 0;
			    CurParam = False;
			    CurAssoc = False;
			    CurUnit = False;
			 }
		    }
		  else if (r == RULE_StructModel)
		     if (pSSchema->SsRootElem == 0 && !pSSchema->SsExtension)
			CompilerError (wi, STR, FATAL, STR_STRUCT_SECTION_MISSING, inputLine, linenb);
		  break;
	       case KWD_INTEGER:
		  pSSchema->SsAttribute[pSSchema->SsNAttributes - 1].AttrType = AtNumAttr;
		  break;
	       case KWD_TEXT:
		  if (r == RULE_BasicType)
		     /* type d'un element */
		     BasicEl (CharString + 1, wi, pr);
		  else
		     /* type d'un attribut */
		     pSSchema->SsAttribute[pSSchema->SsNAttributes - 1].AttrType = AtTextAttr;
		  break;
	       case KWD_REFERENCE:
		  if (r == RULE_Constr)
		     /* constructeur d'un element */
		    {
		       ReferenceAttr = False;	/* c'est un element reference */
		       Equal = False;
		       NewRule (wi);
		       if (!error)
			 {
			    pRule = &pSSchema->SsRule[CurRule[RecursLevel - 1] - 1];
			    pRule->SrConstruct = CsReference;
			    pRule->SrRefTypeNat[0] = '\0';
			    /* a priori, le type d'element */
			    /* reference' est defini dans le meme schema */
			    pRule->SrRefImportedDoc = False;
			    pRule->SrReferredType = 0;
			    /* c'est une reference ordinaire, */
			    /* pas l'inclusion d'un document externe */
			    ExternalStructContext = ElemRef;
			 }
		    }
		  else
		     /* type d'un attribut */
		    {
		       ReferenceAttr = True;	/* c'est un attribut reference */
		       pAttr = &pSSchema->SsAttribute[pSSchema->SsNAttributes - 1];
		       pAttr->AttrType = AtReferenceAttr;
		       pAttr->AttrTypeRefNature[0] = '\0';
		       pAttr->AttrTypeRef = 0;
		       /* a priori, le type d'elem. */
		       /* reference' est defini dans le meme schema */
		       ExternalStructContext = AttrRef;
		    }
		  break;
	       case KWD_ANY:
		  if (ReferenceAttr)
		     pSSchema->SsAttribute[pSSchema->SsNAttributes - 1].AttrTypeRef = 0;
		  else
		     pSSchema->SsRule[CurRule[RecursLevel - 1] - 1].SrReferredType = 0;
		  break;
	       case KWD_First:
		  FirstInPair = True;
		  break;
	       case KWD_Second:
		  SecondInPair = True;
		  break;
	       case KWD_GRAPHICS:
		  BasicEl (GraphicElem + 1, wi, pr);
		  break;
	       case KWD_SYMBOL:
		  BasicEl (Symbol + 1, wi, pr);
		  break;
	       case KWD_PICTURE:
		  BasicEl (Picture + 1, wi, pr);
		  break;
	       case KWD_UNIT:
	       case KWD_NATURE:
		  if (pr == RULE_ExclElem || pr == RULE_InclElem)
		     CompilerError (wi, STR, FATAL, STR_NOT_ALLOWED_HERE, inputLine, linenb);
		  else
		    {
		       Equal = False;
		       NewRule (wi);
		       if (!error)
			 {
			    pRule = &pSSchema->SsRule[CurRule[RecursLevel - 1] - 1];
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
		     if (!RRight[RecursLevel - 1])
			CompilerError (wi, STR, FATAL, STR_GIVE_A_NAME_TO_THAT_ELEM, inputLine, linenb);

		  break;
	       case KWD_EXTERN:
		  if (r == RULE_ExceptType)
		     ExceptExternalType = True;
		  else if (r == RULE_ExtOrDef)
		     if (NExternalTypes >= MAX_EXTERNAL_TYPES)
			/* table des types externes saturee */
			CompilerError (wi, STR, FATAL, STR_TOO_MANY_EXTERNAL_DOCS, inputLine, linenb);
		     else
			/* met dans la table le dernier nom de type rencontre' */
		       {
			  NExternalTypes++;
			  IncludedExternalType[NExternalTypes - 1] = False;
			  strncpy (ExternalType[NExternalTypes - 1], PreviousIdent, MAX_NAME_LENGTH);
			  if (strcmp (PreviousIdent, pSSchema->
			  SsRule[pSSchema->SsRootElem - 1].SrName) == 0)
			     /* C'est le type de document lui-meme qui est utilise' */
			     /* comme externe */
			    {
			       /* ajoute une regle de nature a la fin du schema */
			       pRule = &pSSchema->SsRule[pSSchema->SsNRules++];
			       strncpy (pRule->SrName, PreviousIdent, MAX_NAME_LENGTH);
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
			       PreviousRule = pSSchema->SsNRules;
			    }
		       }
		  break;
	       case KWD_INCLUDED:
		  /* included */
		  if (NExternalTypes >= MAX_EXTERNAL_TYPES)
		     /* table des types externes saturee */
		     CompilerError (wi, STR, FATAL, STR_TOO_MANY_EXTERNAL_DOCS, inputLine, linenb);
		  else
		     /* met dans la table le dernier nom de type rencontre' */
		    {
		       IncludedExternalType[NExternalTypes] = True;
		       strncpy (ExternalType[NExternalTypes], PreviousIdent, MAX_NAME_LENGTH);
		       NExternalTypes++;
		    }
		  break;
	       case KWD_LIST:
		  Equal = False;
		  NewRule (wi);
		  if (!error)
		    {
		       pRule = &pSSchema->SsRule[CurRule[RecursLevel - 1] - 1];
		       pRule->SrConstruct = CsList;
		       pRule->SrMinItems = 0;
		       pRule->SrMaxItems = 32000;
		    }
		  break;
	       case KWD_OF:
		  break;
	       case KWD_AGGREGATE:
	       case KWD_BEGIN:
		  Equal = False;
		  NewRule (wi);
		  if (!error)
		    {
		       pRule = &pSSchema->SsRule[CurRule[RecursLevel - 1] - 1];
		       if (c == KWD_BEGIN)
			  pRule->SrConstruct = CsAggregate;
		       else if (c == KWD_AGGREGATE)
			  pRule->SrConstruct = CsUnorderedAggregate;
		       pRule->SrNComponents = 0;
		       for (i = 0; i < MAX_COMP_AGG; i++)
			  pRule->SrOptComponent[i] = False;
		       Option = False;
		       Push (wi);
		    }
		  break;
	       case KWD_CASE:
		  Equal = False;
		  NewRule (wi);
		  if (!error)
		    {
		       pRule = &pSSchema->SsRule[CurRule[RecursLevel - 1] - 1];
		       pRule->SrConstruct = CsChoice;
		       pRule->SrNChoices = 0;
		       Push (wi);
		    }
		  break;
	       case KWD_CONSTANT:
		  break;
	       case KWD_PAIR:
		  Equal = False;
		  NewRule (wi);
		  if (!error)
		     if (pSSchema->SsRootElem == CurRule[RecursLevel - 1])
			CompilerError (wi, STR, FATAL, STR_ROOT_CANNOT_BE_A_PAIR, inputLine, linenb);
		     else
		       {
			  pRule = &pSSchema->SsRule[CurRule[RecursLevel - 1] - 1];
			  pRule->SrConstruct = CsPairedElement;
			  pRule->SrFirstOfPair = True;
		       }
		  break;
	       case KWD_Nothing:
		  /* pas de contenu pour l'element exporte' courant */
		  pRule = &pSSchema->SsRule[RuleExportWith - 1];
		  pRule->SrExportContent = 0;
		  if ((pRule->SrConstruct == CsChoice) && (pRule->SrNChoices >= 1))
		     for (j = 0; j < pRule->SrNChoices; j++)
			pSSchema->SsRule[pRule->SrChoice[j] - 1].SrExportContent = 0;
		  break;
	       case KWD_NoCut:
		  ExceptionNum (ExcNoCut, True, False, False, wi);
		  break;
	       case KWD_NoCreate:
		  ExceptionNum (ExcNoCreate, True, False, False, wi);
		  break;
	       case KWD_NoHMove:
		  ExceptionNum (ExcNoHMove, True, False, False, wi);
		  break;
	       case KWD_NoVMove:
		  ExceptionNum (ExcNoVMove, True, False, False, wi);
		  break;
	       case KWD_NoHResize:
		  ExceptionNum (ExcNoHResize, True, False, False, wi);
		  break;
	       case KWD_NoVResize:
		  ExceptionNum (ExcNoVResize, True, False, False, wi);
		  break;
	       case KWD_NewWidth:
		  ExceptionNum (ExcNewWidth, False, True, True, wi);
		  break;
	       case KWD_NewHeight:
		  ExceptionNum (ExcNewHeight, False, True, True, wi);
		  break;
	       case KWD_NewHPos:
		  ExceptionNum (ExcNewHPos, False, True, True, wi);
		  break;
	       case KWD_NewVPos:
		  ExceptionNum (ExcNewVPos, False, True, True, wi);
		  break;
	       case KWD_Invisible:
		  ExceptionNum (ExcInvisible, False, True, False, wi);
		  break;
	       case KWD_NoMove:
		  ExceptionNum (ExcNoMove, True, False, False, wi);
		  break;
	       case KWD_NoResize:
		  ExceptionNum (ExcNoResize, True, False, False, wi);
		  break;
	       case KWD_IsDraw:
		  ExceptionNum (ExcIsDraw, True, False, False, wi);
		  break;
	       case KWD_MoveResize:
		  ExceptionNum (ExcMoveResize, True, False, False, wi);
		  break;
	       case KWD_NoSelect:
		  ExceptionNum (ExcNoSelect, True, False, False, wi);
		  /* end case c */
		  break;
	       case KWD_NoSpellCheck:
		  ExceptionNum (ExcNoSpellCheck, True, False, False, wi);
		  break;
	       case KWD_GraphCreation:
		  /* GraphCreation */
		  ExceptionNum (ExcGraphCreation, True, False, False, wi);
		  break;
	       case KWD_Hidden:
		  /* Hidden */
		  ExceptionNum (ExcHidden, True, False, False, wi);
		  break;
	       case KWD_PageBreak:
		  ExceptionNum (ExcPageBreak, True, False, False, wi);
		  break;
	       case KWD_PageBreakAllowed:
		  ExceptionNum (ExcPageBreakAllowed, True, False, False, wi);
		  break;
	       case KWD_PageBreakPlace:
		  ExceptionNum (ExcPageBreakPlace, True, False, False, wi);
		  break;
	       case KWD_PageBreakRepetition:
		  ExceptionNum (ExcPageBreakRepetition, True, False, False, wi);
		  break;
	       case KWD_PageBreakRepBefore:
		  ExceptionNum (ExcPageBreakRepBefore, True, False, False, wi);
		  break;
	       case KWD_ActiveRef:
		  ExceptionNum (ExcActiveRef, False, True, False, wi);
		  break;
	       case KWD_NoPaginate:
		  ExceptionNum (ExcNoPaginate, True, False, False, wi);
		  break;
	       case KWD_ImportLine:
		  if (ImportExcept)
		     CompilerError (wi, STR, FATAL, STR_ONLY_ONE_IMPORT_EXCEPTION,
				    inputLine, linenb);
		  else
		    {
		       ExceptionNum (ExcImportLine, True, False, False, wi);
		       ImportExcept = True;
		    }
		  break;
	       case KWD_ImportParagraph:
		  if (ImportExcept)
		     CompilerError (wi, STR, FATAL, STR_ONLY_ONE_IMPORT_EXCEPTION,
				    inputLine, linenb);
		  else
		    {
		       ExceptionNum (ExcImportParagraph, True, False, False, wi);
		       ImportExcept = True;
		    }
		  break;
	       case KWD_HighlightChildren:
		  ExceptionNum (ExcHighlightChildren, True, False, False, wi);
		  break;
	       case KWD_ExtendedSelection:
		  ExceptionNum (ExcExtendedSelection, True, False, False, wi);
		  break;
	       case KWD_ParagraphBreak:
		  ExceptionNum (ExcParagraphBreak, True, False, False, wi);
		  break;
	       case KWD_Root:
		  CurExtensRule = NewExtensionRule (wi, 0);
		  CurExtensRule->SrName[0] = '\0';
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
			   case RULE_ElemName /* VarElemName */ :
			      if (pr == RULE_StructModel)
				 /* apres le mot-cle 'STRUCTURE' */
				 /* conserve le nom de la structure */
				{
				   CopyWord (pSSchema->SsName, wi, wl);
				   /* compare ce nom avec le nom du fichier */

				   if (strcmp (pSSchema->SsName, srceFileName) != 0)
				      /* noms differents */
				      CompilerError (wi, STR, FATAL, STR_FILE_NAME_AND_STRUCT_NAME_DIFFERENT, inputLine, linenb);
				}
			      if (pr == RULE_Rule)
				 /* debut d'une regle du premier niveau */
				{
				   CopyWord (CurName, wi, wl);
				   CurNum = nb;
				   if (CompilParam)
				      CurParam = True;
				   else
				      CurParam = False;
				   if (CompilAssoc)
				      CurAssoc = True;
				   else
				      CurAssoc = False;
				   if (CompilUnits)
				      CurUnit = True;
				   else
				      CurUnit = False;
				}
			      if (pr == RULE_RootOrElem)
				 /* debut d'une regle d'extension */
				 CurExtensRule = NewExtensionRule (wi, wl);
			      if ((pr == RULE_TypeRef && !ReferenceAttr) || pr == RULE_Element)
				{
				   if (pr == RULE_Element)
				      CopyWord (PreviousIdent, wi, wl);
				   /* garde le nom du type au cas ou il serait suivi de extern */
				   if (Equal)
				     {
					NewRule (wi);
					Equal = False;
				     }
				   if (!error)
				      if (RRight[RecursLevel - 1])
					 /* identificateur en partie droite de regle */
					{
					   RightIdentifier (nb + MAX_BASIC_TYPE, wi);
					   CopyWord (ReferredTypeName, wi, wl);
					   /* garde le nom du type */
					   /* reference' au cas ou il est defini dans un */
					   /* autre schema de structure. */
					   BeginReferredTypeName = wi;
					}
				      else
					 /* identificateur partie gauche de regle */
					{
					   CopyWord (CurName, wi, wl);
					   CurNum = nb;
					}
				}
			      if (pr == RULE_TypeRef && ReferenceAttr)
				 /* dans un attribut reference */
				{
				   pSSchema->SsAttribute[pSSchema->SsNAttributes - 1].AttrTypeRef =
				      nb + MAX_BASIC_TYPE;
				   CopyWord (ReferredTypeName, wi, wl);
				   /* garde le nom du type reference' au cas ou il est defini */
				   /* dans un autre schema de structure. */
				   BeginReferredTypeName = wi;
				}
			      if (pr == RULE_ExtStruct)
				 /* nom d'une structure externe dans une reference ou */
				 /* dans un contenu d'element exporte' */
				{
				   CopyWord (N, wi, wl);
				   /* recupere le nom du schema externe */
				   /* lit le schema de structure externe */
				   if (!RdSchStruct (N, pExternSSchema))
				      CompilerError (wi, STR, FATAL, STR_CANNOT_READ_STRUCT_SCHEM, inputLine, linenb);
				   /* echec lecture du schema */
				   else
				      /* le schema de structure a ete charge' */
				      /* le type reference' existe-t-il dans ce schema? */
				     {
					i = 0;
					while (strcmp (ReferredTypeName, pExternSSchema->SsRule[i].SrName) != 0
					&& i - 1 < pExternSSchema->SsNRules)
					   i++;
					if (strcmp (ReferredTypeName, pExternSSchema->SsRule[i].SrName) != 0)
					   CompilerError (BeginReferredTypeName, STR, FATAL, STR_TYPE_UNKNOWN, inputLine, linenb);
					/* type inconnu */
					else
					   /* le type reference' existe, il a le numero i+1 */
					   switch (ExternalStructContext)
						 {
						    case AttrRef:
						       /* dans un attribut reference */
						       pAttr = &pSSchema->SsAttribute[pSSchema->SsNAttributes - 1];
						       if (SecondInPair)
							  pAttr->AttrTypeRef = i + 2;
						       else
							  pAttr->AttrTypeRef = i + 1;
						       strncpy (pAttr->AttrTypeRefNature, N, MAX_NAME_LENGTH);
						       FirstInPair = False;
						       SecondInPair = False;
						       break;
						    case ElemRef:
						       /* dans une construction CsReference */
						       pRule = &pSSchema->SsRule[CurRule[RecursLevel - 1] - 1];
						       if (SecondInPair)
							  pRule->SrReferredType = i + 2;
						       else
							  pRule->SrReferredType = i + 1;
						       strncpy (pRule->SrRefTypeNat, N, MAX_NAME_LENGTH);
						       FirstInPair = False;
						       SecondInPair = False;
						       break;
						    case ElContent:
						       /* dans un contenu d'elem exporte' */
						       pRule = &pSSchema->SsRule[RuleExportWith - 1];
						       pRule->SrExportContent = i + 1;
						       strncpy (pRule->SrNatExpContent, N, MAX_NAME_LENGTH);
						       UnknownContent = False;
						       if (pRule->SrConstruct == CsChoice)
							  if (pRule->SrNChoices >= 1)
							     for (j = 0; j < pRule->SrNChoices; j++)
							       {
								  pSSchema->SsRule[pRule->SrChoice[j] - 1].SrExportContent = i + 1;
								  strncpy (pSSchema->SsRule[pRule->
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
					pRule = &pSSchema->SsRule[i - 1];
					/* le type existe */
					pRule->SrExportedElem = True;
					RuleExportWith = i;
					pRule->SrExportContent = i;
					/* par defaut on met tout le contenu */
					pRule->SrNatExpContent[0] = '\0';
					ExternalStructContext = ElContent;
					/* si c'est un choix qui est exporte', toutes */
					/* ses options sont egalement exportees */
					if (pRule->SrConstruct == CsChoice)
					   if (pRule->SrNChoices >= 1)
					      for (j = 0; j < pRule->SrNChoices; j++)
						{
						   pSSchema->SsRule[pRule->SrChoice[j] - 1].SrExportedElem = True;
						   pSSchema->SsRule[pRule->SrChoice[j] - 1].SrExportContent = pRule->SrChoice[j];
						   pSSchema->SsRule[pRule->SrChoice[j] - 1].SrNatExpContent[0] = '\0';
						}
					/* si c'est une marque de debut de paire qui est */
					/* exporte'e, la marque de fin est egalement exportee */
					if (pRule->SrConstruct == CsPairedElement)
					   if (pRule->SrFirstOfPair)
					     {
						pSSchema->SsRule[i].SrExportedElem = True;
						pSSchema->SsRule[i].SrExportContent = i + 1;
						pSSchema->SsRule[i].SrNatExpContent[0] = '\0';
					     }
				     }
				}
			      if (pr == RULE_Content)
				 /* le contenu d'un element exporte' */
				{
				   CopyWord (ReferredTypeName, wi, wl);
				   /* garde le nom du type */
				   /* exporte' au cas ou il est defini dans un */
				   /* autre schema de structure. */
				   BeginReferredTypeName = wi;
				   i = RuleNumber (wl, wi);
				   if (i == 0)
				      /* type inconnu */
				      /* le contenu est peut-etre defini dans un autre */
				      /* schema, attendons de voir si un schema externe */
				      /* est indique' apres */
				      UnknownContent = True;
				   else
				      /* c'est un type defini dans le schema */
				   if (pSSchema->SsRule[i - 1].SrConstruct != CsNatureSchema)
				     {
					pRule = &pSSchema->SsRule[RuleExportWith - 1];
					pRule->SrExportContent = i;
					if (pRule->SrConstruct == CsChoice)
					   if (pRule->SrNChoices >= 1)
					      for (j = 0; j < pRule->SrNChoices; j++)
						 pSSchema->SsRule[pRule->SrChoice[j] - 1].
						    SrExportContent = i;
					/* si c'est une marque de debut de paire qui est */
					/* exporte'e, la marque de fin est egalement traitee */
					if (pRule->SrConstruct == CsPairedElement)
					   if (pRule->SrFirstOfPair)
					      if (i == RuleExportWith)
						 /* le contenu de la marque de fin est la marque */
						 /* de fin elle-meme */
						 pSSchema->SsRule[RuleExportWith].SrExportContent = i + 1;
					      else
						 pSSchema->SsRule[RuleExportWith].SrExportContent = i;
				     }
				   else
				      /* le contenu est un objet construit selon un */
				      /* autre schema de structure, on lit ce schema */
				   if (!RdSchStruct (pSSchema->SsRule[i - 1].SrName, pExternSSchema))
				      CompilerError (wi, STR, FATAL, STR_CANNOT_READ_STRUCT_SCHEM, inputLine, linenb);
				   /* echec lecture du schema */
				   else
				      /* le schema de structure a ete charge', le */
				      /* *contenu est l'element racine de ce schema */
				     {
					pRule = &pSSchema->SsRule[RuleExportWith - 1];
					pRule->SrExportContent = pExternSSchema->SsRootElem;
					strncpy (pRule->SrNatExpContent, pExternSSchema->SsName, MAX_NAME_LENGTH);
					if (pRule->SrConstruct == CsChoice)
					   if (pRule->SrNChoices >= 1)
					      for (j = 0; j < pRule->SrNChoices; j++)
						{
						   pSSchema->SsRule[pRule->SrChoice[j] - 1].
						      SrExportContent = pExternSSchema->SsRootElem;
						   strncpy (pSSchema->
							    SsRule[pRule->SrChoice[j] - 1].SrNatExpContent,
							    pExternSSchema->SsName, MAX_NAME_LENGTH);
						}
				     }
				}
			      if (pr == RULE_ExceptType)
				 /* Un nom de type d'element dans les exceptions */
				 if (ExceptExternalType)
				   {
				      ExceptExternalType = False;
				      /*cherche s'il existe deja une regle d'extension pour ce type */
				      CurExtensRule = GetExtensionRule (wi, wl);
				      if (CurExtensRule == NULL)
					 /* il n'en existe pas, on cree une regle d'extension */
					 CurExtensRule = NewExtensionRule (wi, wl);
				      ExceptType = 1;
				   }
				 else
				   {
				      ExceptType = RuleNumber (wl, wi);
				      if (ExceptType == 0)
					 /* ce n'est pas un type declare' dans le schema */
					 /* c'est peut etre un attribut */
					{
					   ExceptAttr = AttributeNumber (wl, wi);
					   if (ExceptAttr == 0)
					      CompilerError (wi, STR, FATAL, STR_TYPE_OR_ATTR_UNKNOWN, inputLine, linenb);
					   else if (pSSchema->SsAttribute[ExceptAttr - 1].AttrFirstExcept != 0)
					      CompilerError (wi, STR, FATAL, STR_THIS_ATTR_ALREADY_HAS_EXCEPTS, inputLine, linenb);
					}
				      else
					 /* c'est un type declare' */
					{
					   if (FirstInPair || SecondInPair)
					      /* le nom d'element est precede' du mot-cle First ou Second */
					      if (pSSchema->SsRule[ExceptType - 1].SrConstruct != CsPairedElement)
						 /* ce n'est pas une paire */
						 CompilerError (wi, STR, FATAL, STR_FIRST_SECOND_FORBIDDEN, inputLine, linenb);
					      else if (SecondInPair)
						 ExceptType++;
					   if (pSSchema->SsRule[ExceptType - 1].SrFirstExcept != 0)
					      CompilerError (wi, STR, FATAL, STR_THIS_TYPE_ALREADY_HAS_EXCEPTS, inputLine, linenb);
					}
				      FirstInPair = False;
				      SecondInPair = False;
				   }
			      if (pr == RULE_InclElem)
				{
				   if (CompilExtens)
				      pRule = CurExtensRule;
				   else
				      pRule = &pSSchema->SsRule[CurRule[RecursLevel - 1] - 1];
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
				   if (CompilExtens)
				      pRule = CurExtensRule;
				   else
				      pRule = &pSSchema->SsRule[CurRule[RecursLevel - 1] - 1];
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
			      CopyWord (pSSchema->SsDefaultPSchema, wi, wl);
			      break;
			      case RULE_AttrName
			   /* AttrName */ :
			      attrNum = identtable[nb - 1].identdef;
			      if (attrNum == 0)
				 /* ce nom n'a pas encore ete rencontre' dans le schema */
				 if (strncmp (&inputLine[wi - 1], pSSchema->SsAttribute[0].AttrName, wl) == 0)
				    /* c'est l'attribut Langue */
				    attrNum = 1;
			      if (CompilAttr || CompilLocAttr)
				 /* une declaration d'attribut */
				 if (CompilAttr && attrNum > 0)
				    CompilerError (wi, STR, FATAL, STR_ATTR_ALREADY_DECLARED, inputLine, linenb);
			      /* deja defini */
				 else
				   {
				      if (attrNum > 0)
					{
					   /* l'attribut est deja defini */
					   if (pSSchema->SsAttribute[attrNum - 1].AttrGlobal)
					      /* un attribut global ne peut pas etre */
					      /* utilise' comme attribut local */
					      CompilerError (wi, STR, FATAL, STR_GLOBAL_ATTR, inputLine, linenb);
					}
				      else
					 /* nouvel attribut */
				      if (pSSchema->SsNAttributes >= MAX_ATTR_SSCHEMA)
					 CompilerError (wi, STR, FATAL, STR_TOO_MANY_ATTRS, inputLine, linenb);
				      /* table des attributs saturee */
				      else
					{
					   pSSchema->SsNAttributes++;
					   identtable[nb - 1].identdef = pSSchema->SsNAttributes;
					   pAttr = &pSSchema->SsAttribute[pSSchema->SsNAttributes - 1];
					   if (CompilLocAttr)
					      /* attribut local */
					      pAttr->AttrGlobal = False;
					   CopyWord (pAttr->AttrName, wi, wl);
					   pAttr->AttrType = AtEnumAttr;
					   pAttr->AttrNEnumValues = 0;
					}
				      if (CompilLocAttr)	/* un attribut local */
					 if (CurNLocAttr >= MAX_LOCAL_ATTR)
					    /* trop d'attributs locaux pour cet element */
					    CompilerError (wi, STR, FATAL, STR_TOO_MANY_ATTRS, inputLine, linenb);
					 else if (CompilExtens)
					    /* dans une regle d'extension */
					   {
					      CurExtensRule->SrNLocalAttrs++;
					      CurExtensRule->SrLocalAttr[CurExtensRule->SrNLocalAttrs - 1] = pSSchema->SsNAttributes;
					      CurExtensRule->SrRequiredAttr[CurExtensRule->SrNLocalAttrs - 1] = MandatoryAttr;
					      MandatoryAttr = False;
					   }
					 else
					    /* dans une regle de structure */
					   {
					      CurLocAttr[CurNLocAttr] = identtable[nb - 1].
						 identdef;
					      CurReqAttr[CurNLocAttr] = MandatoryAttr;
					      CurNLocAttr++;
					      MandatoryAttr = False;
					   }
				   }
			      else if (CompilExcept)
				 /* un nom d'attribut dans les exceptions */
				{
				   ExceptAttr = AttributeNumber (wl, wi);
				   /* numero de cet attribut */
				   if (ExceptAttr == 0)
				      /* ce n'est pas un attribut declare' dans le */
				      /* schema */
				      /* c'est peut etre un type d'element */
				     {
					ExceptType = RuleNumber (wl, wi);
					if (ExceptType == 0)
					   CompilerError (wi, STR, FATAL, STR_TYPE_OR_ATTR_UNKNOWN, inputLine, linenb);
					else if (pSSchema->SsRule[ExceptType - 1].SrFirstExcept != 0)
					   CompilerError (wi, STR, FATAL, STR_THIS_TYPE_ALREADY_HAS_EXCEPTS, inputLine, linenb);
				     }
				   else if (pSSchema->SsAttribute[ExceptAttr - 1].AttrFirstExcept != 0)
				      CompilerError (wi, STR, FATAL, STR_THIS_ATTR_ALREADY_HAS_EXCEPTS, inputLine, linenb);
				}
			      else
				 /* utilisation d'attribut dans une regle avec WITH */
			      if (attrNum == 0)
				 CompilerError (wi, STR, FATAL, STR_ATTR_NOT_DECLARED, inputLine, linenb);
			      else
				{
				   if (CompilExtens)
				      pRule = CurExtensRule;
				   else
				      pRule = &pSSchema->SsRule[CurRule[RecursLevel - 1] - 1];
				   if (pRule->SrNDefAttrs >= MAX_DEFAULT_ATTR)
				      CompilerError (wi, STR, FATAL, STR_TOO_MANY_ATTRS, inputLine, linenb);
				   else
				     {
					pRule->SrNDefAttrs++;
					pRule->SrDefAttr[pRule->SrNDefAttrs - 1] = attrNum;
					pRule->SrDefAttrValue[pRule->SrNDefAttrs - 1] = 1;
					/* par defaut la valeur initiale n'est pas modifiable */
					pRule->SrDefAttrModif[pRule->SrNDefAttrs - 1] = False;
				     }
				}
			      break;
			   case RULE_AttrValue:
			      /* AttrValue */
			      if (CompilAttr || CompilLocAttr)
				 /* definition d'une valeur d'attribut pour l'attribut */
				 /* courant, qui est pour l'instant le dernier */
				{
				   pAttr = &pSSchema->SsAttribute[pSSchema->SsNAttributes - 1];
				   if (pAttr->AttrNEnumValues >= MAX_ATTR_VAL)
				      CompilerError (wi, STR, FATAL, STR_TOO_MANY_VALUES, inputLine, linenb);
				   /* la liste des valeurs deborde */
				   else
				      /* une valeur de plus pour l'attribut */
				     {
					pAttr->AttrNEnumValues++;
					/* range la valeur dans la liste des valeurs de */
					/* l'attribut */
					CopyWord (pAttr->AttrEnumValue[pAttr->AttrNEnumValues - 1], wi, wl);
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
					     identtable[nb - 1].identref = pSSchema->SsNAttributes;
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
				   if (CompilExtens)
				      pRule = CurExtensRule;
				   else
				      pRule = &pSSchema->SsRule[CurRule[RecursLevel - 1] - 1];
				   pAttr = &pSSchema->SsAttribute[pRule->SrDefAttr[pRule->SrNDefAttrs - 1] - 1];
				   if (pAttr->AttrType != AtEnumAttr)
				      CompilerError (wi, STR, FATAL,
						     STR_INVALID_VALUE_FOR_THAT_ATTR, inputLine, linenb);
				   else
				     {
					/* cette valeur est-elle dans la liste des valeurs de */
					/* l'attribut ? */
					i = 1;
					ok = False;
					CopyWord (N, wi, wl);
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
			      if (!Rules)
				 /* definition de constante */
				{
				   CopyWord (CurName, wi, wl);
				   CurNum = nb;
				   CurParam = False;
				   CurAssoc = False;
				   CurUnit = False;
				   NewRule (wi);
				   /* cree une regle */
				   if (!error)
				     {
					pRule = &pSSchema->SsRule[CurRule[RecursLevel - 1] - 1];
					pRule->SrConstruct = CsConstant;
					pRule->SrIndexConst = TextConstPtr;
				     }
				}
			      else
				 /* utilisation d'une constante dans une regle */
			      if (identtable[nb - 1].identdef == 0)
				 /* non defini, c'est une erreur */
				 CompilerError (wi, STR, FATAL, STR_CONSTANT_NOT_DECLARED, inputLine, linenb);
			      else if (pr == RULE_Constr)
				 /* dans une regle de structuration */
				 BasicEl (nb + MAX_BASIC_TYPE, wi, pr);
			      else if (pr == RULE_ExceptType)
				 /* Un nom de constante dans les exceptions */
				 ExceptType = RuleNumber (wl, wi);
			      if (pSSchema->SsRule[ExceptType - 1].SrFirstExcept != 0)
				 CompilerError (wi, STR, FATAL, STR_THIS_TYPE_ALREADY_HAS_EXCEPTS, inputLine, linenb);
			      break;
			   default:
			      break;
			}
		  break;
	       case 3002:
		  /* un nombre */
		  number = AsciiToInt (wi, wl);
		  /* le nombre lu */
		  if (r == RULE_Integer)
		     /* nombre d'elements min. ou max. d'une liste */
		    {
		       pRule = &pSSchema->SsRule[CurRule[RecursLevel - 1] - 1];
		       if (Minimum)
			  /* min */
			  pRule->SrMinItems = number;
		       else if (Maximum)
			  /* max */
			 {
			    pRule->SrMaxItems = number;
			    if (pRule->SrMaxItems < pRule->SrMinItems)
			       CompilerError (wi, STR, FATAL, STR_MAXIMUM_LOWER_THAN_MINIMUM, inputLine, linenb);
			 }
		    }
		  if (r == RULE_NumValue)
		     /* valeur d'un attribut fixe */
		    {
		       if (CompilExtens)
			  pRule = CurExtensRule;
		       else
			  pRule = &pSSchema->SsRule[CurRule[RecursLevel - 1] - 1];
		       if (pSSchema->SsAttribute[pRule->SrDefAttr[pRule->SrNDefAttrs - 1] - 1].AttrType != AtNumAttr)
			  CompilerError (wi, STR, FATAL, STR_INVALID_VALUE_FOR_THAT_ATTR, inputLine, linenb);
		       else
			 {
			    pRule->SrDefAttrValue[pRule->SrNDefAttrs - 1] = number * Sign;
			    /* a priori la prochaine valeur d'attribut sera positive */
			    Sign = 1;
			 }
		    }
		  if (r == RULE_ExceptNum)
		     /* un numero d'exception associe' a un type d'element */
		     /* ou a un attribut */
		     if (number <= 100)
			/* les valeurs inferieures a 100 sont reservees aux */
			/* exceptions predefinies */
			CompilerError (wi, STR, FATAL, STR_THIS_NUMBER_MUST_BE_GREATER_THAN_100, inputLine, linenb);
		     else
			ExceptionNum (number, False, False, False, wi);
		  break;
	       case 3003:
		  /* une chaine de caracteres */
		  if (r == RULE_ConstValue)
		     /* c'est le texte d'une constante */
		     StoreConstText (wi, wl);
		  else if (r == RULE_StrValue)
		     /* valeur fixe d'un attribut texte */
		    {
		       if (CompilExtens)
			  pRule = CurExtensRule;
		       else
			  pRule = &pSSchema->SsRule[CurRule[RecursLevel - 1] - 1];
		       if (pSSchema->SsAttribute[pRule->SrDefAttr[
			 pRule->SrNDefAttrs - 1] - 1].AttrType != AtTextAttr)
			  CompilerError (wi, STR, FATAL, STR_INVALID_VALUE_FOR_THAT_ATTR, inputLine, linenb);
		       else
			  pRule->SrDefAttrValue[pRule->SrNDefAttrs - 1] =
			     StoreConstText (wi, wl);
		    }
		  break;
	    }
}


/* ---------------------------------------------------------------------- */
/* |    ExternalTypes     marque comme externe dans le schema de        | */
/* |    structure tous les elements qui sont dans la table des types    | */
/* |    externes.                                                       | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         ExternalTypes ()

#else  /* __STDC__ */
static void         ExternalTypes ()
#endif				/* __STDC__ */

{
   int                 i, j;
   SRule              *pRule;

   /* parcourt la table des types externes */
   for (j = 0; j < NExternalTypes; j++)
      /* cherche le type externe dans le schema a partir de la fin */
     {
	i = pSSchema->SsNRules - 1;
	while (strcmp (ExternalType[j], pSSchema->SsRule[i].SrName) != 0)
	   i--;
	/* le type externe est defini par la regle de numero i */
	pRule = &pSSchema->SsRule[i];
	if (pRule->SrConstruct != CsNatureSchema)
	   /* ce n'est pas une nature externe, erreur */
	  {
	     if (IncludedExternalType[j])
		CompilerErrorString (0, STR, INFO, STR_CANNOT_BE_INCLUDED, inputLine, linenb, pRule->SrName);
	     else
		CompilerErrorString (0, STR, INFO, STR_CANNOT_BE_EXTERN, inputLine, linenb, pRule->SrName);
	  }
	else
	   /* transforme la regle CsNatureSchema */
	  {
	     if (!RdSchStruct (pRule->SrName, pExternSSchema))
	       {
		  /* echec lecture du schema externe */
		  CompilerErrorString (0, STR, INFO, STR_EXTERNAL_STRUCT_NOT_FOUND, inputLine, linenb, pRule->SrName);
		  /* meme si le schema externe n'existe pas, on transforme la regle */
		  pRule->SrReferredType = MAX_BASIC_TYPE + 1;
	       }
	     else
		pRule->SrReferredType = pExternSSchema->SsRootElem;
	     /* transforme la regle en regle CsReference */
	     strncpy (pRule->SrRefTypeNat, pRule->SrName, MAX_NAME_LENGTH);
	     pRule->SrRefImportedDoc = True;
	     pRule->SrConstruct = CsReference;
	  }
     }
}

/* ---------------------------------------------------------------------- */
/* |    CheckRecursivity                                                | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         CheckRecursivity (int r, int path[], int level, boolean busy[], boolean done[])

#else  /* __STDC__ */
static void         CheckRecursivity (r, path, level, busy, done)
int         r;
int         path[];
int                 level;
boolean             busy[];
boolean             done[];

#endif /* __STDC__ */

{
   int                 m;
   SRule              *pRule;

   pRule = &pSSchema->SsRule[r - 1];
   /* Si l'element est deja connu, inutile de l'explorer */
   if (done[r])
      return;
   /* si l'element est `busy', c'est qu'il est recursif;
      Il est forcement dans la pile, et de plus tous les elements
      qui ont ete empiles apres lui sont egalement recursifs,
      donc on les marque */
   if (busy[r])
     {
	for (m = level - 1; m >= 0; m--)
	  {
	     pSSchema->SsRule[path[m] - 1].SrRecursive = True;
	     if (path[m] == r)
		break;
	  }
	return;
     }
   busy[r] = True;
   path[level] = r;
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
		  CheckRecursivity (pRule->SrChoice[m],
				    path, level + 1, busy, done);
	       break;
	    case CsIdentity:
	       CheckRecursivity (pRule->SrIdentRule,
				 path, level + 1, busy, done);
	       break;
	    case CsList:
	       CheckRecursivity (pRule->SrListItem,
				 path, level + 1, busy, done);
	       break;
	    case CsUnorderedAggregate:
	    case CsAggregate:
	       for (m = 0; m < pRule->SrNComponents; m++)
		  CheckRecursivity (pRule->SrComponent[m],
				    path, level + 1, busy, done);
	       break;
	    default:
	       break;
	 }
   busy[r] = False;
   /* l'element a ete explore, on ne le fera plus */
   done[r] = True;
}

/* ---------------------------------------------------------------------- */
/* |    ChkRecurs                                                       | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         ChkRecurs ()

#else  /* __STDC__ */
static void         ChkRecurs ()
#endif				/* __STDC__ */

{
   int         i;
   int         path[100];
   boolean             busy[MAX_RULES_SSCHEMA + 1], done[MAX_RULES_SSCHEMA + 1];
   SRule              *pRule;

   for (i = MAX_BASIC_TYPE; i <= pSSchema->SsNRules; i++)
      busy[i] = done[i] = False;

   for (i = MAX_BASIC_TYPE + 1; i <= pSSchema->SsNRules; i++)
      CheckRecursivity (i, path, 0, busy, done);

   for (i = MAX_BASIC_TYPE; i < pSSchema->SsNRules; i++)
     {
	pRule = &pSSchema->SsRule[i];
	if (pRule->SrRecursive)
	   TtaDisplayMessage (INFO, TtaGetMessage(STR, STR_RECURSIVE_ELEM), (char *) pRule->SrName);
     }
}

/* ---------------------------------------------------------------------- */
/* |    ListAssocElem       liste les elements consideres comme		| */
/* |            parametres et elements associes                         | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         ListAssocElem ()

#else  /* __STDC__ */
static void         ListAssocElem ()
#endif				/* __STDC__ */

{
  int	i;
  SRule	*pRule;

  /* parcourt toute la table des regles */
  for (i = MAX_BASIC_TYPE; i < pSSchema->SsNRules; i++)
    {
      if (pSSchema->SsRule[i].SrParamElem)
	/* affiche un message */
	    TtaDisplayMessage (INFO, TtaGetMessage(STR, STR_PARAMETER), (char *)pSSchema->SsRule[i].SrName);
      if (pSSchema->SsRule[i].SrAssocElem)
	if (!pSSchema->SsRule[i].SrRecursDone)
	  /* l'element associe est utilise dans une autre regle, erreur */
	  {
	    CompilerErrorString(0, STR, INFO, STR_THE_ASSOC_ELEM_IS_USED_IN_ANOTHER_RULE, inputLine, linenb, pSSchema->SsRule[i].SrName);
	    error = True;
	  } 
	else	
	  /* element associe correct */
	  /* On cree une nouvelle regle liste a la fin de la table des */
	  /* regles */
	  {
	    if (pSSchema->SsNRules >= MAX_RULES_SSCHEMA)
		TtaDisplaySimpleMessage (FATAL, STR, STR_TOO_MAN_RULES);
	    /* saturation de la table des regles */
	    else
	      pSSchema->SsNRules++;
	    pRule = &pSSchema->SsRule[pSSchema->SsNRules - 1];
	    /* la nouvelle regle liste prend le nom de ses elements... */
	    strncpy(pRule->SrName, pSSchema->SsRule[i].SrName, MAX_NAME_LENGTH-2);
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
	    TtaDisplayMessage (INFO, TtaGetMessage(STR, STR_ASSOC_ELEMS), (char *)pRule->SrName);
	    if (RuleNameExist())
		TtaDisplaySimpleMessage (FATAL, STR, STR_NAME_ALREADY_DECLARED);
	  }
	else
	   /* ce n'est pas un element associe */
	if (pSSchema->SsRule[i].SrRecursDone)
	   if (i + 1 != pSSchema->SsRootElem && !pSSchema->SsRule[i].SrUnitElem)
	     {
		if (pSSchema->SsRule[i].SrConstruct == CsChoice)
		   /* c'est un choix qui definit un alias */
		   /* on met l'element dans la table des alias */
		  {
		     NAlias++;
		     Alias[NAlias - 1] = i + 1;
		     TtaDisplayMessage (INFO, TtaGetMessage(STR, STR_ALIAS), (char *) pSSchema->SsRule[i].SrName);
		  }
		else
		   /* les regles de Fin de CsPairedElement ne sont jamais referencees */
		   if (pSSchema->SsRule[i].SrConstruct != CsPairedElement ||
		       pSSchema->SsRule[i].SrFirstOfPair)
		   /* c'est une definition inutile */
		   TtaDisplayMessage (INFO, TtaGetMessage(STR, STR_UNUSED), (char *) pSSchema->SsRule[i].SrName);
	     }
     }
}


/* ---------------------------------------------------------------------- */
/* |    ListNotCreated       liste les types d'elements qui ne seront	| */
/* |    pas crees lors de l'edition d'un document ou d'un objet de	| */
/* |    ce type.                                                        | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         ListNotCreated ()

#else  /* __STDC__ */
static void         ListNotCreated ()
#endif				/* __STDC__ */

{
   int         r, rr;
   int                 i;
   boolean             temp;
   SRule              *pRule;
   SRule              *pRule2;

   /* remet a zero dans toutes les regles l'indicateur de creation */
   /* (on utilise SrRecursDone comme indicateur de creation) */
   for (r = 0; r < pSSchema->SsNRules; r++)
      pSSchema->SsRule[r].SrRecursDone = False;
   /* on creera la racine */
   if (pSSchema->SsRootElem > 0)
      pSSchema->SsRule[pSSchema->SsRootElem - 1].SrRecursDone = True;
   /* parcourt toute la table des regles */
   for (r = 0; r < pSSchema->SsNRules; r++)
     {
	pRule = &pSSchema->SsRule[r];
	if (pRule->SrParamElem)
	   /* les parametres seront crees */
	   pRule->SrRecursDone = True;
	/* les elements inclus (au sens SGML) seront crees */
	if (pRule->SrNInclusions > 0)
	   for (i = 0; i < pRule->SrNInclusions; i++)
	      pSSchema->SsRule[pRule->SrInclusion[i] - 1].SrRecursDone = True;
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
		    pSSchema->SsRule[pRule->SrListItem - 1].SrRecursDone = True;
		    /* les listes d'elements associes seront crees */
		    if (pSSchema->SsRule[pRule->SrListItem - 1].SrAssocElem)
		       pRule->SrRecursDone = True;
		    break;
		 case CsChoice:
		    /* les elements de choix seront crees */
		    if (pRule->SrNChoices > 0)
		      {
			 for (i = 0; i < pRule->SrNChoices; i++)
			   {
			      pSSchema->SsRule[pRule->SrChoice[i] - 1].SrRecursDone = True;
			      /* si le choix est une unite exportee, ce sont */
			      /* ses options qui seront des unites exportees */
			      if (pRule->SrUnitElem)
				 pSSchema->SsRule[pRule->SrChoice[i] - 1].SrUnitElem = True;
			   }
			 pRule->SrUnitElem = False;
		      }
		    break;
		 case CsUnorderedAggregate:
		 case CsAggregate:
		    /* les composants d'agregats seront crees */
		    for (i = 0; i < pRule->SrNComponents; i++)
		       pSSchema->SsRule[pRule->SrComponent[i] - 1].SrRecursDone = True;
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
   for (r = 0; r < pSSchema->SsNRules; r++)
     {
	pRule = &pSSchema->SsRule[r];
	if (pRule->SrConstruct == CsChoice)
	   /* tous les choix qui ne sont pas la regle racine, qui ne portent pas
	      d'exceptions SGML et qui n'appartiennent pas a un agregat sont crees
	      temporairement, jusqu'a ce qu'ils soient remplaces par une de leurs
	      valeurs possibles. Si le choix est un simple alias, l'element ne
	      sera jamais cree. */
	  {
	     temp = True;
	     if (r + 1 == pSSchema->SsRootElem)
		temp = False;
	     else if (pRule->SrNInclusions > 0 || pRule->SrNExclusions > 0)
		temp = False;
	     else
		for (rr = 0; rr < pSSchema->SsNRules; rr++)
		   if (pSSchema->SsRule[rr].SrConstruct == CsAggregate ||
		       pSSchema->SsRule[rr].SrConstruct == CsUnorderedAggregate)
		      for (i = 0; i < pSSchema->SsRule[rr].SrNComponents; i++)
			 if (pSSchema->SsRule[rr].SrComponent[i] == r + 1)
			    temp = False;
	     if (temp)
		/* est-ce un alias ? On cherche dans la table des alias */
		for (i = 0; i < NAlias; i++)
		   if (Alias[i] == r + 1)
		      /* l'element est dans la table. Il ne sera pas cree */
		      temp = False;
	     if (temp)
		TtaDisplayMessage (INFO, TtaGetMessage(STR, STR_IS_A_TEMPORARY_ELEM), (char *) pRule->SrName);
	  }
	else if (!pRule->SrRecursDone)
	   /* les unites peuvent ne pas etre utilisees dans le schema */
	   if (!pRule->SrUnitElem)
	     {
		TtaDisplayMessage (INFO, TtaGetMessage(STR, STR_WON_T_BE_CREATED), (char *) pRule->SrName);
		/* cherche s'il y a des REFERENCES sur ce type d'element */
		for (rr = 0; rr < pSSchema->SsNRules; rr++)
		  {
		     pRule2 = &pSSchema->SsRule[rr];
		     if (pRule2->SrConstruct == CsReference)
			if (pRule2->SrRefTypeNat[0] == '\0')
			   if (pRule2->SrReferredType == r + 1)
			      TtaDisplayMessage (INFO, TtaGetMessage(STR, STR_WON_T_BE_CREATED_AND_IS_REFD), (char *) pRule->SrName);
		  }
	     }
     }
}


/* ---------------------------------------------------------------------- */
/* |    main                                                            | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
int                 main (int argc, char **argv)

#else  /* __STDC__ */
int                 main (argc, argv)
int                 argc;
char              **argv;

#endif /* __STDC__ */

{
   FILE                *inputFile;
   boolean             fileOK;
   char                buffer[200];
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
   InitParser ();
   InitSyntax ("STRUCT.GRM");
   if (!error)
     {
	/* teste les arguments d'appel du programme */
	if (argc != 2)
	   TtaDisplaySimpleMessage (FATAL, STR, STR_NO_SUCH_FILE);
	else
	  {
	     /* recupere le nom du fichier a compiler */
	     strncpy (srceFileName, argv[1], MAX_NAME_LENGTH - 1);
	     i = strlen (srceFileName);
	     /* ajoute le suffixe .SCH */
	     strcat (srceFileName, ".SCH");
	     /* teste si le fichier a compiler existe */
	     if (FileExist (srceFileName) == 0)
		TtaDisplaySimpleMessage (FATAL, STR, STR_NO_SUCH_FILE);
	     else
		/* le fichier d'entree existe, on l'ouvre */
	       {
		  inputFile = BIOreadOpen (srceFileName);
		  /* supprime le suffixe ".SCH" */
		  srceFileName[i] = '\0';
		  /* acquiert la memoire pour le schema de structure */
		  if ((pSSchema = (PtrSSchema) malloc (sizeof (StructSchema))) == NULL)
		     TtaDisplaySimpleMessage (FATAL, STR, STR_NOT_ENOUGH_MEM);
		  /* memoire pour un schema de structure externe */
		  if ((pExternSSchema = (PtrSSchema) malloc (sizeof (StructSchema))) == NULL)
		     TtaDisplaySimpleMessage (FATAL, STR, STR_NOT_ENOUGH_MEM);

		  lgidenttable = 0;	/* table des identificateurs vide */
		  linenb = 0;
		  Initialize ();	/* prepare la generation */
		  /* lit tout le fichier et fait l'analyse */
		  fileOK = True;
		  while (fileOK && !error)
		     /* lit une ligne */
		    {
		       i = 0;
		       do
			 {
			    fileOK = BIOreadByte (inputFile, &inputLine[i]);
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
			    sscanf (inputLine, "# %d %s", &linenb, buffer);
			    linenb--;
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
				 GetNextToken (i, &wi, &wl, &wn);	/* mot suivant */
				 if (wi > 0)
				    /* on a trouve un mot */
				   {
				      /* on analyse le mot */
				      AnalyzeToken (wi, wl, wn, &c, &r, &nb, &pr);
				      if (!error)
					 /* on le traite */
					 ProcessToken (wi, wl, c, r, nb, pr);
				   }
			      }
			    while (!(wi == 0 || error));
			 }	/* il n'y a plus de mots */
		    }
		  if (!error)
		     ParserEnd ();	/* fin d'analyse */
		  if (!error)
		     /* met les bons numeros de regle si ca n'a pas deja ete fait */
		     /* lorsqu'on a rencontre' le mot-cle' EXPORT */
		     if (!pSSchema->SsExport)
			ChangeRules ();
		  if (!error)
		     ExternalTypes ();
		  /* traite les noms de types declares comme extern */
		  if (!error)
		    {
		       /* cherche les regles recursives */
		       ChkRecurs ();
		       /* liste les elements consideres comme
		          elements associes */
		       ListAssocElem ();
		       /* liste les elements qui ne seront pas
		          crees par l'editeur */
		       ListNotCreated ();

		       /* ecrit le schema compile' dans le fichier de sortie */
		       DirectorySchemas[0] = '\0';	/* utilise le directory courant */
		       if (!error)
			 {
			    strcat (srceFileName, ".STR");
			    fileOK = WrSchStruct (srceFileName, pSSchema, 0);
			    if (!fileOK)
			       TtaDisplayMessage (FATAL, TtaGetMessage(STR, STR_CANNOT_WRITE), srceFileName);
			 }
		    }
		  free (pSSchema);
		  free (pExternSSchema);
		  BIOreadClose (inputFile);
	       }
	  }
     }
   fflush (stdout);
   TtaSaveAppRegistry ();
   exit (0);
}
