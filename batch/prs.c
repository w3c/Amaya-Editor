/*
 *
 *  (c) COPYRIGHT INRIA, Grif, 1996.
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
 * Ce programme compile un schema de presentation contenu dans un fichier
 * de type .P
 * Il est dirige par la grammaire du langage de presentation
 * contenue, sous forme codee, dans le fichier PRESEN.GRM.
 * Il produit un fichier de type .PRS qui sera ensuite utilise par Thot
 *
 * Authors: V. Quint (INRIA)
 *          C. Roisin (INRIA) - Columns and pages
 *
 */

#include "thot_sys.h"
#include "constgrm.h"
#include "constmedia.h"
#include "compilmsg.h"
#include "prsmsg.h"
#include "message.h"
#include "typemedia.h"
#include "typegrm.h"
#include "fileaccess.h"
#include "prsdef.h"
#include "thotpattern.h"
#include "fileaccess.h"
#include "thotdir.h"

#define THOT_EXPORT
#include "compil_tv.h"
#include "thotcolor.h"
#include "thotcolor_tv.h"
#include "thotpalette_tv.h"

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "analsynt_tv.h"
#include "compil_tv.h"

int                 LineNum;	/* compteur de lignes dans le fichier source */

static PtrPSchema   pPSchema;	/* Schema de presentation genere */
static PtrSSchema   pSSchema;	/* Schema de structure */
static int          CurView;	/* numero de la vue courante */
static PtrPRule     FirstRule;	/* pointeur sur la premiere regle de la suite

				   de regles courante */
static PtrPRule     CurRule;	/* pointeur sur la regle de present. courante */
static PtrPRule     NextRule;	/* pointeur sur la regle de present. suivante */
static int          CurAttrNum;	/* numero de l'attribut en cours */
static int          CurAttrVal;	/* numero de la valeur d'attribut en cours */
static int          CurComparAttr;	/* numero de l'attribut de comparaison */
static int          CurElemHeritAttr;	/* numero de l'element heritant de cet

					   attribut */
static int          CurAttrLowerBound;	/* borne inferieure de comparaison */
static int          CurAttrUpperBound;	/* borne superieure de comparaison */
static Name         CurTextEqual;	/* valeur d'attribut textuel */
static int          CurType;	/* numero de la regle de structure definissant

				   le type dont on analyse les regles de presentation */
static int          CurPresBox;	/* numero de la boite de presentation courante */
static boolean      ViewDef;	/* on est dans la definition des vues du doc. */
static boolean      CounterDef;	/* on est dans la definition des compteurs */
static boolean      ConstantDef;	/* on est dans la definition des constantes */
static boolean      VariableDef;	/* on est dans la definition des variables */
static CounterValue CurMinMax;	/* SyntacticType de la valeur du compteur */
static boolean      PresBoxDef;	/* on est dans la definition des boites */
static boolean      DefaultRuleDef;	/* on est dans la definition des regles par

					   defaut */
static boolean      RuleDef;	/* on est dans la definition des regles */
static boolean      AttributeDef;	/* on est dans la definition des attributs */
static boolean      NewAttributeDef;	/* definition d'un nouveau paquet de regles

					   de presentation d'un attribut */
static int          TransmittedCounter;		/* numero du compteur dont on traite la regle

						   de transmission */
static int          TransmittedElem;	/* type de l'element dont on traite la regle

					   de transmission */
static boolean      InBreakRule;	/* on est dans une regle 'Break' */
static boolean      InPageBreakRule;	/* on est dans une regle 'PageBreak' */
static boolean      InLineBreakRule;	/* on est dans une regle 'LineBreak' */
static boolean      InGatherRule;	/* on est dans une regle 'Gather' */
static boolean      InInLineRule;	/* on est dans une regle 'InLine' */
static boolean      IncludedColumn;
static boolean      InRule;	/* on est dans une regle */
static boolean      InWithinCond;	/* on est dans une condition 'Within' */
static boolean      AxisDef;	/* le prochain repere boite rencontre est une

				   definition de repere */
static boolean      Forward;	/* on est dans une reference en avant pour une

				   boite */
static boolean      CondBlock;	/* on est dans un bloc de condition */
static boolean      ViewBlock;	/* on est dans un bloc de vues */
static boolean      RuleBlock;	/* on est dans un bloc de regles */
static boolean      RulesForView;	/* on est dans les regles d'une vue */
static boolean      NewVariableDef;	/* on definit une nouvelle variable */
static int          LatestNumber;	/* valeur absolue du dernier nombre recontre */
static boolean      LatestNumberAttr;	/* LatestNumber est un numero d'attribut numerique */
static int          PrevSign;	/* signe du dernier nombre rencontre */
static int          AncestorSign;	/* signe du dernier niveau d'ancetre rencontre'

					   dans un compteur */
static enum
  {
     Centimeter, Millimeter, Point, Pica, Inch, ScreenPixel, FontHeight,
     XHeight, Percent
  }
CurUnit;

static int          AttrValSign;	/* signe (+1 ou -1) de la derniere valeur

					   d'attribut rencontree (AttrRelat) */
static PtrCondition Conditions;	/* les conditions du IF */
static boolean      Immediately;	/* Pour les conditions avec Immediately Within */
static boolean      SignGreaterOrLess;	/* il y a un signe > ou < dans la condition

					   Within en cours de traitement */
static int          CurCondCntSign;	/* signe dans une condition */
static Name         CopyType;	/* nom du type qui apparait dans la regle Copy */
static int          BeginCopyType;	/* indice dans la ligne du debut de ce nom */
static PtrSSchema   pExternalSS;	/* pointeur sur le schema de structure externe */
static boolean      InCondPage;	/* on est dans une condition Even, Odd ou One */
static boolean      InclusionRefName;	/* le nom de type qui suit doit etre interprete

					   comme le nom d'une reference d'inclusion
					   sans expansion */
static boolean      VCondLess;	/* on est dans une condition 'Less' */
static boolean      VCondGreater;	/* on est dans une condition 'Greater' */
static boolean      CondEqual;	/* on est dans une condition 'Equal' */

static boolean      FirstInPair;	/* on a rencontre' "First" */
static boolean      SecondInPair;	/* on a rencontre' "Second" */

static boolean      AttrInitCounter;	/* on a rencontre' "Init" dans une definition

					   de compteur */
#include "compilmsg_f.h"
#include "fileaccess_f.h"
#include "parser_f.h"
#include "platform_f.h"
#include "prs_f.h"
#include "memory_f.h"
#include "message_f.h"
#include "readstr_f.h"
#include "registry_f.h"
#include "writeprs_f.h"

/*----------------------------------------------------------------------
   Initialize							
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         Initialize ()
#else  /* __STDC__ */
static void         Initialize ()
#endif	/* __STDC__ */
{
   int                 i;

   /* acquiert un schema de presentation */
   GetSchPres (&pPSchema);
   if (pPSchema == NULL)
     /* memoire insuffisante */
     CompilerMessage (0, PRS, FATAL, NO_MORE_MEM_LEFT, inputLine, LineNum);
   else
     {
       /* acquiert un schema de structure pour les structures externes */
       GetSchStruct (&pExternalSS);
       if (pExternalSS == NULL)
	 TtaDisplaySimpleMessage (FATAL, PRS, NO_MORE_MEM_LEFT);
       else
	 {
	   CurMinMax = CntCurVal;
	   /* initialise les regles de presentation des elements */
	   for (i = 0; i < MAX_RULES_SSCHEMA; i++)
	     {
	       pPSchema->PsAcceptPageBreak[i] = True;
	       pPSchema->PsAcceptLineBreak[i] = True;
	     }

	   /* initialise les boites de presentation */
	   for (i = 0; i < MAX_PRES_BOX; i++)
	     {
	       pPSchema->PsPresentBox[i].PbAcceptPageBreak = True;
	       pPSchema->PsPresentBox[i].PbAcceptLineBreak = True;
	     }
	   
	   /* initialise les indicateurs du compilateur */
	   ViewDef = False;
	   CounterDef = False;
	   ConstantDef = False;
	   VariableDef = False;
	   PresBoxDef = False;
	   DefaultRuleDef = False;
	   RuleDef = False;
	   AttributeDef = False;
	   NewAttributeDef = False;
	   InBreakRule = False;
	   InPageBreakRule = False;
	   InLineBreakRule = False;
	   InGatherRule = False;
	   InInLineRule = False;
	   IncludedColumn = False;
	   InRule = False;
	   InWithinCond = False;
	   AxisDef = False;
	   Forward = False;
	   CondBlock = False;
	   ViewBlock = False;
	   RuleBlock = False;
	   RulesForView = False;
	   NewVariableDef = False;
	   LatestNumber = 0;
	   LatestNumberAttr = False;
	   PrevSign = 1;
	   CurUnit = FontHeight;
	   AttrValSign = 1;
	   CurView = 1;
	   CurRule = NULL;
	   FirstRule = NULL;
	   CurType = 1;
	   GetPresentRule (&NextRule);
	   if (NextRule == NULL)
	     /* memoire insuffisante */
	     CompilerMessage (0, PRS, FATAL, NO_MORE_MEM_LEFT, inputLine, LineNum);
	   Conditions = NULL;
	   InclusionRefName = False;
	   CopyType[0] = '\0';
	   BeginCopyType = 0;
	   FirstInPair = False;
	   SecondInPair = False;
	 }
     }
}

/*----------------------------------------------------------------------
   CopyName        copy the word of length wl startig at position	
   wi in buffer inputLine into string n.		
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static void         CopyName (Name n, indLine wi, indLine wl)

#else  /* __STDC__ */
static void         CopyName (n, wi, wl)
Name                n;
indLine             wi;
indLine             wl;

#endif /* __STDC__ */

{
   if (wl > MAX_NAME_LENGTH - 1)
      CompilerMessage (wi, PRS, FATAL, WORD_SIZE_OVERFLOW, inputLine, LineNum);
   else
     {
	strncpy (n, &inputLine[wi - 1], MAX_NAME_LENGTH);
	n[wl] = '\0';
     }
}

/*----------------------------------------------------------------------
   CheckConditions verifie qu'il n'y a aucune condition courante   
   ou qu'il n'y a que des conditions Within.                       
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         CheckConditions (indLine wi)

#else  /* __STDC__ */
static void         CheckConditions (wi)
indLine             wi;

#endif /* __STDC__ */

{
   PtrCondition        pCond;

   pCond = Conditions;
   while (pCond != NULL)
      if (pCond->CoCondition != PcWithin && pCond->CoCondition != PcDefaultCond
	  && pCond->CoCondition != PcElemType)
	{
	   CompilerMessage (wi, PRS, FATAL, ONLY_CONDITION_WITHIN, inputLine, LineNum);
	   pCond = NULL;
	}
      else
	 pCond = pCond->CoNextCondition;
}

/*----------------------------------------------------------------------
   ConditionEnd                                                    
  ----------------------------------------------------------------------*/
static void         ConditionEnd ()
{
   InWithinCond = False;
   if (CopyType[0] != '\0')
      /* on n'a pas encore traite' le nom de type suppose' externe */
      /* ce nom de type est donc erronne' */
     {
	CompilerMessage (BeginCopyType, PRS, FATAL, UNDECLARED_IDENTIFIER, inputLine, LineNum);
	CopyType[0] = '\0';
	BeginCopyType = 0;
     }
}


/*----------------------------------------------------------------------
   CreatePRule cree une nouvelle regle de type t                   
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         CreatePRule (PRuleType t, indLine wi)

#else  /* __STDC__ */
static void         CreatePRule (t, wi)
PRuleType           t;
indLine             wi;

#endif /* __STDC__ */

{
   PtrPRule            pPRuleV;

   ConditionEnd ();
   if (t != PtFunction)
      /* ce n'est pas une regle fonction. Verifie qu'il n'y a que des
         conditions Within parmi les conditions courantes */
      CheckConditions (wi);
   CurRule = NextRule;
   GetPresentRule (&NextRule);
   if (NextRule == NULL)
      /*memoire insuffisante */
      CompilerMessage (0, PRS, FATAL, NO_MORE_MEM_LEFT, inputLine, LineNum);
   CurRule->PrType = t;
   CurRule->PrCond = Conditions;
   CurRule->PrNextPRule = NextRule;
   CurRule->PrViewNum = CurView;
   CurRule->PrPresMode = PresImmediate;
   switch (t)
	 {
	    case PtVisibility:
	    case PtDepth:
	    case PtFillPattern:
	    case PtBackground:
	    case PtForeground:
	       CurRule->PrAttrValue = False;
	       CurRule->PrIntValue = 0;
	       break;
	    case PtFont:
	       CurRule->PrChrValue = 'T';	/* Times par defaut */
	       break;
	    case PtStyle:
	       CurRule->PrChrValue = 'R';	/* Romain par defaut */
	       break;
	    case PtUnderline:
	       CurRule->PrChrValue = 'N';	/* Pas de souligne par defaut */
	       break;
	    case PtThickness:
	       CurRule->PrChrValue = 'N';	/* souligne mince par defaut */
	       break;
	    case PtLineStyle:
	       CurRule->PrChrValue = 'S';	/* trait continu par defaut */
	       break;
	    case PtBreak1:
	    case PtBreak2:
	    case PtIndent:
	    case PtSize:
	    case PtLineSpacing:
	    case PtLineWeight:
	       CurRule->PrMinUnit = UnRelative;
	       CurRule->PrMinAttr = False;
	       CurRule->PrMinValue = 0;
	       CurRule->PrInhMinOrMax = 0;
	       CurRule->PrInhDelta = 0;
	       CurRule->PrMinMaxAttr = False;
	       break;
	    case PtAdjust:
	       CurRule->PrAdjust = AlignLeft;
	       break;
	    case PtVertRef:
	    case PtHorizRef:
	    case PtVertPos:
	    case PtHorizPos:
	       CurRule->PrPosRule.PoPosDef = NoEdge;
	       CurRule->PrPosRule.PoPosRef = NoEdge;
	       CurRule->PrPosRule.PoDistUnit = UnRelative;
	       CurRule->PrPosRule.PoDistAttr = False;
	       CurRule->PrPosRule.PoDistance = 0;
	       CurRule->PrPosRule.PoRelation = RlSameLevel;
	       CurRule->PrPosRule.PoNotRel = False;
	       CurRule->PrPosRule.PoRefKind = RkPresBox;
	       CurRule->PrPosRule.PoUserSpecified = False;
	       CurRule->PrPosRule.PoRefIdent = 0;
	       break;
	    case PtHeight:
	    case PtWidth:
	       CurRule->PrDimRule.DrPosition = False;
	       CurRule->PrDimRule.DrAbsolute = True;
	       CurRule->PrDimRule.DrUnit = UnRelative;
	       CurRule->PrDimRule.DrAttr = False;
	       CurRule->PrDimRule.DrValue = 0;
	       CurRule->PrDimRule.DrRelation = RlSameLevel;
	       CurRule->PrDimRule.DrNotRelat = False;
	       CurRule->PrDimRule.DrRefKind = RkPresBox;
	       CurRule->PrDimRule.DrMin = False;
	       CurRule->PrDimRule.DrUserSpecified = False;
	       CurRule->PrDimRule.DrRefIdent = 0;
	       break;
	    default:
	       break;
	 }
   /* verifie que cette regle n'est pas deja presente pour cette vue */
   pPRuleV = FirstRule;
   while (pPRuleV != NULL && pPRuleV != CurRule)
     {
	if (pPRuleV->PrViewNum == CurView)
	   if (pPRuleV->PrType == t)
	      if (t != PtFunction)
		 /* seules les fonctions peuvent etre en plusieurs exemplaires */
		 if (pPRuleV->PrCond == NULL)
		    /* les regles sans condition ne peuvent pas figurer en
		       plusieurs exemplaires */
		    CompilerMessage (wi, PRS, FATAL, RULE_ALREADY_DEFINED, inputLine, LineNum);
	pPRuleV = pPRuleV->PrNextPRule;
     }
}

/*----------------------------------------------------------------------
   SetLevel                                                          
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static void         SetLevel (Level lev, indLine wi)

#else  /* __STDC__ */
static void         SetLevel (lev, wi)
Level               lev;
indLine             wi;

#endif /* __STDC__ */

{
   if ((CurRule->PrType == PtVertRef || CurRule->PrType == PtHorizRef) &&
       !(lev == RlEnclosed || lev == RlSelf))
      CompilerMessage (wi, PRS, FATAL, ONLY_ENCLOSED_AND_ARE_ALLOWED, inputLine, LineNum);
   else if (lev == RlEnclosed && (CurRule->PrType == PtVertPos || CurRule->PrType == PtHorizPos))
      /* position par rapport au contenu, erreur */
      CompilerMessage (wi, PRS, FATAL, FORBIDDEN_IN_A_POSITION, inputLine, LineNum);
   else
      switch (CurRule->PrType)
	    {
	       case PtVertRef:
	       case PtHorizRef:
	       case PtVertPos:
	       case PtHorizPos:
		  CurRule->PrPosRule.PoRelation = lev;
		  break;
	       case PtHeight:
	       case PtWidth:
		  if (CurRule->PrDimRule.DrPosition)
		     CurRule->PrDimRule.DrPosRule.PoRelation = lev;
		  else
		     CurRule->PrDimRule.DrRelation = lev;
		  break;
	       default:
		  break;
	    }
}

/*----------------------------------------------------------------------
   CheckBoxEnd                                                      
  ----------------------------------------------------------------------*/
static void         CheckBoxEnd ()
{
   if (PresBoxDef)
      /* fin des regles de presentation d'une boite */
     {
	if (pPSchema->PsPresentBox[CurPresBox - 1].PbFirstPRule == NextRule)
	   /* aucune regle (seulement 'Content') */
	   pPSchema->PsPresentBox[CurPresBox - 1].PbFirstPRule = NULL;
     }
   if (CurRule != NULL)
      CurRule->PrNextPRule = NULL;
}


/*----------------------------------------------------------------------
   EndOfRulesForType   fin des regles de presentation associees	
   a un type d'element structure' ou a une valeur	
   d'attribut. On verifie si au moins une regle a  
   ete definie pour ce type ou cet attribut.	
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         EndOfRulesForType ()

#else  /* __STDC__ */
static void         EndOfRulesForType ()
#endif				/* __STDC__ */

{
   AttributePres      *pPRuleA;
   NumAttrCase        *pAttrCase;
   int                 l;

   if (RuleDef)
      if (pPSchema->PsElemPRule[CurType - 1] == NextRule)
	 /* aucune regle de presentation pour ce type d'element */
	 pPSchema->PsElemPRule[CurType - 1] = NULL;
   if (AttributeDef)
     {
	pPRuleA = pPSchema->PsAttrPRule[CurAttrNum - 1];
	for (l = pPSchema->PsNAttrPRule[CurAttrNum - 1]; --l > 0; pPRuleA = pPRuleA->ApNextAttrPres)
	   if (pPRuleA->ApElemType == CurElemHeritAttr)
	      break;

	if (pPRuleA)
	  {
	     /* selon le type de l'attribut */
	     switch (pSSchema->SsAttribute[CurAttrNum - 1].AttrType)
		   {
		      case AtNumAttr:
			 pAttrCase = &pPRuleA->ApCase[pPRuleA->ApNCases - 1];
			 if (pAttrCase->CaFirstPRule == NextRule)
			    pAttrCase->CaFirstPRule = NULL;
			 break;
		      case AtTextAttr:
			 if (pPRuleA->ApTextFirstPRule == NextRule)
			    pPRuleA->ApTextFirstPRule = NULL;
			 break;
		      case AtReferenceAttr:
			 if (pPRuleA->ApRefFirstPRule == NextRule)
			    pPRuleA->ApRefFirstPRule = NULL;
			 break;
		      case AtEnumAttr:
			 if (pPRuleA->ApEnumFirstPRule[CurAttrVal] == NextRule)
			    pPRuleA->ApEnumFirstPRule[CurAttrVal] = NULL;
			 break;
		      default:
			 break;
		   }
	  }
     }
}

/*----------------------------------------------------------------------
   NewVarListItem alloue une nouvelle entree dans la table des     
   items constituant une variable de presentation.         
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         NewVarListItem (PresVariable * pVar, indLine wi)
#else  /* __STDC__ */
static void         NewVarListItem (pVar, wi)
PresVariable       *pVar;
indLine             wi;

#endif /* __STDC__ */
{
   if (pVar->PvNItems > 0)
      /* ce n'est pas le premier element de la variable */
      /* si le 1er element n'est pas du texte, on n'accepte pas le */
      /* nouvel element */
      if (pVar->PvItem[0].ViType == VarText)
	 if (pPSchema->PsConstant[pVar->PvItem[0].ViConstant - 1].PdType != CharString)
	    CompilerMessage (wi, PRS, FATAL, MAX_FUNCTIONS_IN_A_VARIABLE_OVERFLOW, inputLine, LineNum);
   if (pVar->PvNItems >= MAX_PRES_VAR_ITEM)
      CompilerMessage (wi, PRS, FATAL, MAX_FUNCTIONS_IN_A_VARIABLE_OVERFLOW, inputLine, LineNum);
   else
      pVar->PvNItems++;
}

/*----------------------------------------------------------------------
   NewCondition alloue une nouvelle condition                      
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         NewCondition (indLine wi)
#else  /* __STDC__ */
static void         NewCondition (wi)
indLine             wi;
#endif /* __STDC__ */
{
   PtrCondition        newCond;

   /* acquiert un bloc memoire pour une nouvelle condition */
   GetPresentRuleCond (&newCond);
   if (newCond == NULL)
      /*memoire insuffisante */
      CompilerMessage (0, PRS, FATAL, NO_MORE_MEM_LEFT, inputLine, LineNum);
   else
     {
	Immediately = False;
	/* chaine la nouvelle condition en tete */
	newCond->CoNextCondition = Conditions;
	Conditions = newCond;
	/* initialise la nouvelle condition */
	newCond->CoNotNegative = True;
	newCond->CoTarget = False;
	CurCondCntSign = 1;
	InCondPage = False;
     }
}


/*----------------------------------------------------------------------
   NewConst        alloue une nouvelle entree dans la table des    
   constantes du schema de presentation            
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         NewConst (indLine wi)
#else  /* __STDC__ */
static void         NewConst (wi)
indLine             wi;
#endif /* __STDC__ */

{
   if (pPSchema->PsNConstants >= MAX_PRES_CONST)
      CompilerMessage (wi, PRS, FATAL, MAX_CONSTANTS_OVERFLOW, inputLine,
		       LineNum);
   else
      {
      pPSchema->PsConstant[pPSchema->PsNConstants].PdAlphabet = 'L';
      pPSchema->PsConstant[pPSchema->PsNConstants].PdType = CharString;
      pPSchema->PsNConstants++;
      }
}


/*----------------------------------------------------------------------
   NewVar  alloue une nouvelle entree dans la table des variables  
   variables du schema de presentation                     
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static void         NewVar (indLine wi)

#else  /* __STDC__ */
static void         NewVar (wi)
indLine             wi;

#endif /* __STDC__ */

{
   if (pPSchema->PsNVariables >= MAX_PRES_VARIABLE)
      CompilerMessage (wi, PRS, FATAL, MAX_VARIABLES_OVERFLOW, inputLine, LineNum);
   else
      pPSchema->PsVariable[pPSchema->PsNVariables++].PvNItems = 0;
}


/*----------------------------------------------------------------------
   Round arrondit un float en un int.                              
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static int          Round (float e)
#else  /* __STDC__ */
static int          Round (e)
float               e;

#endif /* __STDC__ */
{
   register int        result;

   if (e < 0.0)
      result = e - 0.5;
   else
      result = e + 0.5;
   return result;
}

/*----------------------------------------------------------------------
   EndOfNumber                                                     
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         EndOfNumber ()
#else  /* __STDC__ */
static void         EndOfNumber ()
#endif				/* __STDC__ */
{
   TypeUnit            unit;

   unit = UnRelative;
   if (LatestNumber != 0)
     {
	if (LatestNumberAttr)
	   /* on ne convertit pas les numeros d'attribut */
	   switch (CurUnit)
		 {
		    case Centimeter:
		    case Millimeter:
		    case Point:
		    case Pica:
		    case Inch:
		       unit = UnPoint;
		       break;
		    case ScreenPixel:
		       unit = UnPixel;
		       break;
		    case FontHeight:
		       unit = UnRelative;
		       break;
		    case XHeight:
		       unit = UnXHeight;
		       break;
		    case Percent:
		       unit = UnPercent;
		       break;
		    default:
		       fprintf (stderr, "Invalid distance unit %X\n", CurUnit);
		       break;
		 }
	else
	   /* convertit les unite's de longueur */
	   switch (CurUnit)
		 {
		    case Centimeter:
		       /* convertit les centimetres en points typo */
		       LatestNumber = Round ((float) (LatestNumber * 72) / 2540);
		       unit = UnPoint;
		       break;
		    case Millimeter:
		       /* convertit les millimetres en points typo */
		       LatestNumber = Round ((float) (LatestNumber * 72) / 25400);
		       unit = UnPoint;
		       break;
		    case Point:
		       /* arrondit en points typo */
		       LatestNumber = Round ((float) LatestNumber / 1000);
		       unit = UnPoint;
		       break;
		    case Pica:
		       /* convertit les picas en points typo */
		       LatestNumber = Round ((float) (LatestNumber * 12) / 1000);
		       unit = UnPoint;
		       break;
		    case Inch:
		       /* convertit les pouces en points typo */
		       LatestNumber = Round ((float) (LatestNumber * 72) / 1000);
		       unit = UnPoint;
		       break;
		    case ScreenPixel:
		       /* arrondit en pixels */
		       LatestNumber = Round ((float) LatestNumber / 1000);
		       unit = UnPixel;
		       break;
		    case FontHeight:
		       /* convertit en 1/10 */
		       LatestNumber = Round ((float) LatestNumber / 100);
		       unit = UnRelative;
		       break;
		    case XHeight:
		       /* convertit en 1/10 */
		       LatestNumber = Round ((float) LatestNumber / 100);
		       unit = UnXHeight;
		       break;
		    case Percent:
		       LatestNumber = Round ((float) LatestNumber / 1000);
		       unit = UnPercent;
		       break;
		    default:
		       fprintf (stderr, "Invalid distance unit %X\n", CurUnit);
		       break;
		 }

	LatestNumber = LatestNumber * PrevSign;
	if (CurRule->PrPresMode == PresInherit)
	  {
	     if (CurRule->PrType == PtIndent || CurRule->PrType == PtLineSpacing
		 || CurRule->PrType == PtLineWeight)
	       {
		  CurRule->PrInhDelta = LatestNumber;
		  CurRule->PrInhAttr = LatestNumberAttr;
		  CurRule->PrInhUnit = unit;
	       }
	  }
	else
	   switch (CurRule->PrType)
		 {
		    case PtBreak1:
		    case PtBreak2:
		    case PtIndent:
		    case PtLineSpacing:
		    case PtLineWeight:
		       CurRule->PrMinUnit = unit;
		       CurRule->PrMinAttr = LatestNumberAttr;
		       CurRule->PrMinValue = LatestNumber;
		       break;
		    case PtVertRef:
		    case PtHorizRef:
		    case PtVertPos:
		    case PtHorizPos:
		       CurRule->PrPosRule.PoDistUnit = unit;
		       CurRule->PrPosRule.PoDistAttr = LatestNumberAttr;
		       CurRule->PrPosRule.PoDistance = LatestNumber;
		       break;
		    case PtHeight:
		    case PtWidth:
		       if (CurRule->PrDimRule.DrPosition)
			 {
			    CurRule->PrDimRule.DrPosRule.PoDistUnit = unit;
			    CurRule->PrDimRule.DrPosRule.PoDistAttr = LatestNumberAttr;
			    CurRule->PrDimRule.DrPosRule.PoDistance = LatestNumber;
			 }
		       else
			 {
			    CurRule->PrDimRule.DrUnit = unit;
			    CurRule->PrDimRule.DrAttr = LatestNumberAttr;
			    CurRule->PrDimRule.DrValue = LatestNumber;
			 }
		       break;
		    default:
		       break;
		 }

     }
   LatestNumber = 0;
   LatestNumberAttr = False;
   PrevSign = 1;
   CurUnit = FontHeight;
}

/*----------------------------------------------------------------------
   NewAttrPRule    allocation et initialisation d'une structure    
   AttributePres en fonction de l'attribut de numero att   
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static AttributePres *NewAttrPRule (int att)
#else  /* __STDC__ */
static AttributePres *NewAttrPRule (att)
int                 att;
#endif /* __STDC__ */
{
   AttributePres      *pPRuleA;
   NumAttrCase        *pAttrCase;
   int                 j;

   GetAttributePres (&pPRuleA, 1);
   if (pPRuleA)
     {
	/* selon le type de l'attribut */
	switch (pSSchema->SsAttribute[att - 1].AttrType)
	      {
		 case AtNumAttr:
		    pPRuleA->ApNCases = 0;
		    for (j = 0; j < MAX_PRES_ATTR_CASE; j++)
		      {
			 pAttrCase = &pPRuleA->ApCase[j];
			 pAttrCase->CaComparType = ComparConstant;
			 pAttrCase->CaLowerBound = -MAX_INT_ATTR_VAL - 1;
			 pAttrCase->CaUpperBound = MAX_INT_ATTR_VAL + 1;
			 pAttrCase->CaFirstPRule = NULL;
		      }
		    break;
		 case AtTextAttr:
		    pPRuleA->ApString[0] = '\0';
		    pPRuleA->ApTextFirstPRule = NULL;
		    break;
		 case AtReferenceAttr:
		    pPRuleA->ApRefFirstPRule = NULL;
		    break;
		 case AtEnumAttr:
		    for (j = 0; j <= MAX_ATTR_VAL; j++)
		       pPRuleA->ApEnumFirstPRule[j] = NULL;
		    break;
		 default:
		    break;
	      }
     }
   else
      /* memoire insuffisante */
      CompilerMessage (0, PRS, FATAL, NO_MORE_MEM_LEFT, inputLine, LineNum);
   pPSchema->PsNAttrPRule[att - 1] += 1;
   return (pPRuleA);
}


/*----------------------------------------------------------------------
   GenerateRPresAttribut	alloue eventuellement une nouvelle	
   structure AttributePres et initialise la regle concernee	
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         GenerateRPresAttribut (indLine wi)

#else  /* __STDC__ */
static void         GenerateRPresAttribut (wi)
indLine             wi;

#endif /* __STDC__ */

{
   AttributePres      *pPRuleA;
   NumAttrCase        *pAttrCase;
   int                 l;

   /* s'il n'y a pas de structure AttributePres, on en alloue une */
   if (pPSchema->PsNAttrPRule[CurAttrNum - 1] == 0)
     {
	pPRuleA = pPSchema->PsAttrPRule[CurAttrNum - 1]
	   = NewAttrPRule (CurAttrNum);
	if (CurElemHeritAttr)
	  {
	     pPSchema->PsNInheritedAttrs[CurElemHeritAttr - 1] += 1;
	     pPSchema->PsNHeirElems[CurAttrNum - 1] += 1;
	  }
     }

   else
      /* sinon on cherche s'il y en a une qui traite deja certaines regles
         de l'element CurElemHeritAttr */
     {
	pPRuleA = pPSchema->PsAttrPRule[CurAttrNum - 1];
	for (l = pPSchema->PsNAttrPRule[CurAttrNum - 1]; --l > 0; pPRuleA = pPRuleA->ApNextAttrPres)
	   if (CurTextEqual[0] == '\0')
	      if (pPRuleA->ApElemType == CurElemHeritAttr)
		 break;
	/* si on n'a pas touve, on alloue un paquet suivant */
	if (pPRuleA->ApElemType != CurElemHeritAttr || CurTextEqual[0] != '\0')
	  {
	     pPRuleA->ApNextAttrPres = NewAttrPRule (CurAttrNum);
	     pPRuleA = pPRuleA->ApNextAttrPres;
	     if (CurElemHeritAttr)
	       {
		  pPSchema->PsNInheritedAttrs[CurElemHeritAttr - 1] += 1;
		  pPSchema->PsNHeirElems[CurAttrNum - 1] += 1;
	       }
	  }
     }

   /* maintenant on remplit les champs dans pPRuleA */
   pPRuleA->ApElemType = CurElemHeritAttr;

   switch (pSSchema->SsAttribute[CurAttrNum - 1].AttrType)
	 {
	    case AtNumAttr:
	       /* c'est un attribut a valeur numerique */
	       if (pPRuleA->ApNCases >= MAX_PRES_ATTR_CASE)
		  /* trop de cas pour cet attribut */
		  CompilerMessage (wi, PRS, FATAL, MAX_CASES_IN_ATTR_OVERFLOW, inputLine, LineNum);
	       else
		 {
		    pAttrCase = &pPRuleA->ApCase[pPRuleA->ApNCases++];
		    if (CurComparAttr == 0)
		       /* on utilise des constantes pour comparer */
		      {
			 pAttrCase->CaComparType = ComparConstant;
			 pAttrCase->CaLowerBound = CurAttrLowerBound;
			 pAttrCase->CaUpperBound = CurAttrUpperBound;
		      }
		    else
		       /* on utilise un numero d'attribut */
		      {
			 if (CurElemHeritAttr == 0)
			    pPSchema->PsNComparAttrs[CurComparAttr - 1] += 1;
			 /* ATTENTION
			    ce n'est donc pas le vrai nombre d'attributs se comparant
			    a CurComparAttr puisqu'on fait +1 a chaque fois */
			 pAttrCase->CaComparType = ComparAttr;
			 if (VCondGreater)
			    /* attr GREATER MinValAttrName  */
			   {
			      pAttrCase->CaLowerBound = CurComparAttr;
			      pAttrCase->CaUpperBound = -1;
			   }
			 else if (VCondLess)
			    /* attr LESS MaxValAttrName  */
			   {
			      pAttrCase->CaLowerBound = -1;
			      pAttrCase->CaUpperBound = CurComparAttr;
			   }
			 else
			    /* attr EQUAL EqValAttrName  */
			   {
			      pAttrCase->CaLowerBound = CurComparAttr;
			      pAttrCase->CaUpperBound = CurComparAttr;
			   }
		      }
		    pAttrCase->CaFirstPRule = NextRule;
		    FirstRule = NextRule;
		 }
	       break;

	    case AtTextAttr:
	       strcpy (pPRuleA->ApString, CurTextEqual);
	       CurTextEqual[0] = '\0';
	       pPRuleA->ApTextFirstPRule = NextRule;
	       FirstRule = NextRule;
	       break;

	    case AtReferenceAttr:
	       if (pPRuleA->ApRefFirstPRule != NULL)
		  /* attribut deja rencontre' */
		  CompilerMessage (wi, PRS, FATAL, CANT_REDEFINE, inputLine, LineNum);
	       else
		 {
		    pPRuleA->ApRefFirstPRule = NextRule;
		    FirstRule = NextRule;
		 }
	       break;

	    case AtEnumAttr:
	       if (pPRuleA->ApEnumFirstPRule[CurAttrVal] != NULL)
		  /* attribut deja rencontre' */
		  CompilerMessage (wi, PRS, FATAL, CANT_REDEFINE, inputLine, LineNum);
	       else
		 {
		    pPRuleA->ApEnumFirstPRule[CurAttrVal] = NextRule;
		    FirstRule = NextRule;
		 }
	       break;

	    default:
	       break;
	 }
   NewAttributeDef = False;
}


/*----------------------------------------------------------------------
   PageCounterChangeBox Cherche les compteurs de page pour la vue  
   pageView et indique dans ces compteurs que la boite de numero   
   boxNum peut etre modifiee par ces compteurs.			
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         PageCounterChangeBox (int boxNum, int pageView)

#else  /* __STDC__ */
static void         PageCounterChangeBox (boxNum, pageView)
int                 boxNum;
int                 pageView;

#endif /* __STDC__ */

{
   int                 counter;
   int                 item;
   Counter            *pCntr;

   /* parcourt tous les compteurs du schema de presentation */
   for (counter = 0; counter < pPSchema->PsNCounters; counter++)
     {
	pCntr = &pPSchema->PsCounter[counter];
	for (item = 0; item < pCntr->CnNItems; item++)
	   if (pCntr->CnItem[item].CiElemType == PageBreak + 1)
	      if (pCntr->CnItem[item].CiViewNum == pageView)
		 /* C'est un compteur de pages pour la vue voulue */
		 if (pCntr->CnNPresBoxes < MAX_PRES_COUNT_USER)
		   {
		      pCntr->CnNPresBoxes++;
		      pCntr->CnPresBox[pCntr->CnNPresBoxes - 1] = boxNum;
		   }
     }
}


/*----------------------------------------------------------------------
   ProcessShortKeyWord                                             
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         ProcessShortKeyWord (int x, indLine wi, SyntacticCode gCode)

#else  /* __STDC__ */
static void         ProcessShortKeyWord (x, wi, gCode)
int                 x;
indLine             wi;
SyntacticCode       gCode;

#endif /* __STDC__ */

{
   PresVariable       *pPresVar;

   switch (x)
	 {
	       /* traitement selon le code du mot-cle court */
	    case CHR_59:
	       /*  ;  */
	       if (gCode == RULE_Rule)
		  /* fin d'une regle */
		 {
		    if (CurRule != NULL)
		       if (CurRule->PrType == PtBreak1 || CurRule->PrType == PtBreak2 ||
			   CurRule->PrType == PtIndent || CurRule->PrType == PtVertRef || CurRule->
			   PrType == PtHorizRef || CurRule->PrType == PtVertPos || CurRule->PrType ==
			   PtHorizPos || CurRule->PrType == PtHeight || CurRule->PrType == PtWidth ||
			   CurRule->PrType == PtLineSpacing || CurRule->PrType == PtLineWeight)
			  EndOfNumber ();
		    InBreakRule = False;
		    InPageBreakRule = False;
		    InLineBreakRule = False;
		    InGatherRule = False;
		    InInLineRule = False;
		    /* verifie la validite des regles de dimensionnement relatif */
		    /* au contenu */
		    if (CurRule != NULL)
		       if (CurRule->PrType == PtHeight || CurRule->PrType == PtWidth)
			  if (!CurRule->PrDimRule.DrPosition)
			     if (!CurRule->PrDimRule.DrAbsolute)
				if (CurRule->PrDimRule.DrRelation == RlEnclosed)
				   if (CurRule->PrDimRule.DrRefIdent != 0 || CurRule->PrDimRule.DrValue != 0)
				      CompilerMessage (wi, PRS, FATAL, BAD_DIM_RULE, inputLine, LineNum);
		    InRule = False;
		    if (RulesForView && !RuleBlock && !CondBlock)
		      {
			 RulesForView = False;
			 CurView = 1;
		      }
		    if (!ViewBlock)
		       CheckBoxEnd ();
		    if (!RuleBlock)
		       Conditions = NULL;
		 }
	       else if (gCode == RULE_Transmit)
		  /* fin d'une regle transmit */
		 {
		    TransmittedCounter = 0;
		    TransmittedElem = 0;
		 }
	       break;
	    case CHR_44:
	       /*  ,  */
	       if (gCode == RULE_Function)
		 {
		    pPresVar = &pPSchema->PsVariable[pPSchema->PsNVariables - 1];
		    if (pPresVar->PvItem[pPresVar->PvNItems - 1].ViType == VarPageNumber)
		       /* c'est la fin d'un PageNumber dans une variable. On connait */
		       /* la vue concernee */
		      {
			 if (NewVariableDef)
			    /* definition de variable dans une regle Content */
			    /* Cherche les compteurs de page pour la vue concernee */
			    /* et indique dans ces compteurs que la boite courante */
			    /* peut etre modifiee par ces compteurs */
			    PageCounterChangeBox (CurPresBox, pPresVar->PvItem[pPresVar->PvNItems - 1].ViView);
		      }
		 }
	       break;
	    case CHR_58:
	       /*  :  */
	       if (gCode == RULE_Rule1 || gCode == RULE_Rule2 || gCode == RULE_Rule3 ||
		   gCode == RULE_Rule4)
		  InRule = True;
	       if (gCode == RULE_Attr && NewAttributeDef)
		  GenerateRPresAttribut (wi);
	       break;
	    case CHR_40:
	       /*  (  */
	       if (gCode == RULE_VarConst)
		  /* dans une regle VarConst */
		  if (PresBoxDef)
		    /* dans la regle Content d'une boite de presentation */
		    {
		       NewVar (wi);	/* cree une nouvelle variable */
		       pPSchema->PsPresentBox[CurPresBox - 1].PbContent = ContVariable;
		       pPSchema->PsPresentBox[CurPresBox - 1].PbContVariable = pPSchema->PsNVariables;
		       NewVariableDef = True;
		    }
		  else
		     /* dans une regle Content d'une paire ou d'une reference,
		        on refuse: seules les constantes sont acceptees dans
			cette regle */
		     CompilerMessage (wi, PRS, FATAL, FORBIDDEN_IN_A_REF, inputLine, LineNum);
	       else if (gCode == RULE_Rule3)
		  /* dans Rule3 */
		  InRule = True;
	       else if (gCode == RULE_ElemCondition && !InCondPage)
		  /* on s'occupe de la construction ``if (compteur < cste)'' (resp >, =, IN [*..*]
		     et pas des construction ``if One (compteur)'' (resp Even, Odd) */
		 {
		    Conditions->CoCondition = PcInterval;
		    Conditions->CoCounter = 0;
		    Conditions->CoMinCounter = MIN_COUNTER_VAL;
		    Conditions->CoMaxCounter = MAX_COUNTER_VAL;
		    Conditions->CoValCounter = CntCurVal;
		 }
	       break;
	    case CHR_41:
	       /*  )  */
	       if (gCode == RULE_VarConst)
		  /* dans une regle VarConst */
		  NewVariableDef = False;	/* fin de definition de variable */
	       if (gCode == RULE_Rule3)
		  if (CurRule->PrType == PtFunction && CurRule->PrPresFunction == FnCopy)
		     /* fin d'une regle Copy */
		     if (CopyType[0] != '\0')
			/* on n'a pas encore traite' le nom de type a copier */
			/* ce nom de type est donc erronne' */
		       {
			  CompilerMessage (BeginCopyType, PRS, FATAL, UNDECLARED_IDENTIFIER, inputLine, LineNum);
			  CopyType[0] = '\0';
			  BeginCopyType = 0;
		       }
	       break;
	    case CHR_61:
	       /*  =  */
	       if (gCode == RULE_InheritVal || gCode == RULE_NameInherit ||
		   gCode == RULE_BoolInherit || gCode == RULE_InheritDist ||
		 gCode == RULE_InheritSize || gCode == RULE_AdjustInherit ||
		   gCode == RULE_LineStyleInherit)
		  /* PresInherit */
		 {
		    CurRule->PrInhDelta = 0;
		    CurRule->PrInhUnit = UnRelative;
		    CurRule->PrInhAttr = False;
		 }
	       if (gCode == RULE_InheritDist)
		 {
		    PrevSign = 1;
		    LatestNumberAttr = False;
		    LatestNumber = 0;
		    CurUnit = FontHeight;
		 }
	       break;
	    case CHR_46:
	       /*  .  */
	       if (gCode == RULE_Dimension)	/* dimension relative */
		 {
		    if (!CurRule->PrDimRule.DrPosition)
		       CurRule->PrDimRule.DrAbsolute = False;
		 }
	       else if (gCode == RULE_AbsDist)	/* introduit une partie decimale */
		  if (LatestNumberAttr)		/* interdit apres un attribut */
		     CompilerMessage (wi, PRS, FATAL, NO_DECIMAL_PART_AFTER_AN_ATTR, inputLine, LineNum);
	       break;
	    case CHR_42:
	       /*  *  */
	       if (gCode == RULE_TypeOrPage || gCode == RULE_RefVPosition ||
		   gCode == RULE_RefHPosition || gCode == RULE_Present ||
		   gCode == RULE_BoxType)
		  InclusionRefName = True;
	       else if (gCode == RULE_Reference)
		  SetLevel (RlSelf, wi);
	       break;
	    case CHR_43:
	       /*  +  */
	       if (gCode == RULE_Sign || gCode == RULE_InheritDist)
		  /* dans une distance */
		  PrevSign = 1;
	       break;
	    case CHR_45:
	       /*  -  */
	       if (gCode == RULE_Sign || gCode == RULE_InheritDist)
		  /* dans une distance */
		  PrevSign = -1;
	       else if (gCode == RULE_AttrRelat || gCode == RULE_AttrValue)
		  /* devant une valeur d'attribut numerique, dans une condition */
		  /* d'application de regles associees a un attribut */
		  AttrValSign = -1;
	       else if (gCode == RULE_ElemCondition)
		  CurCondCntSign = -1;
	       else if (gCode == RULE_RelAncestorLevel)
		  AncestorSign = -1;
	       break;
	    case CHR_62:
	       /*  >  */
	       if (gCode == RULE_GreaterLess)
		 {
		    Conditions->CoAncestorRel = CondGreater;
		    SignGreaterOrLess = True;
		 }
	       break;
	    case CHR_60:
	       /*  <  */
	       if (gCode == RULE_GreaterLess)
		 {
		    Conditions->CoAncestorRel = CondLess;
		    SignGreaterOrLess = True;
		 }
	       break;
	    case CHR_37:
	       /*  %  */
	       if (gCode == RULE_Unit)
		  CurUnit = Percent;
	       break;
	    default:
	       break;
	 }

}

/*----------------------------------------------------------------------
   ProcessAxis                                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         ProcessAxis (BoxEdge axis, indLine wi)

#else  /* __STDC__ */
static void         ProcessAxis (axis, wi)
BoxEdge             axis;
indLine             wi;

#endif /* __STDC__ */

{
   switch (CurRule->PrType)
	 {
	    case PtVertRef:
	    case PtHorizRef:
	       CurRule->PrPosRule.PoPosRef = axis;
	       break;
	    case PtVertPos:
	    case PtHorizPos:
	       if (AxisDef)
		 {
		    AxisDef = False;
		    CurRule->PrPosRule.PoPosDef = axis;
		 }
	       else
		  CurRule->PrPosRule.PoPosRef = axis;
	       break;
	    case PtHeight:
	    case PtWidth:
	       if (!CurRule->PrDimRule.DrPosition)
		  if ((CurRule->PrType == PtHeight && !(axis == Top || axis == Bottom))
		      || (CurRule->PrType == PtWidth && !(axis == Left || axis == Right)))
		     CompilerMessage (wi, PRS, FATAL, BROKEN_RULE, inputLine, LineNum);
		  else
		    {
		       CurRule->PrDimRule.DrPosition = True;
		       CurRule->PrDimRule.DrPosRule.PoPosDef = axis;
		       CurRule->PrDimRule.DrPosRule.PoPosRef = NoEdge;
		       CurRule->PrDimRule.DrPosRule.PoDistUnit = UnRelative;
		       CurRule->PrDimRule.DrPosRule.PoDistAttr = False;
		       CurRule->PrDimRule.DrPosRule.PoDistance = 0;
		       CurRule->PrDimRule.DrPosRule.PoRelation = RlSameLevel;
		       CurRule->PrDimRule.DrPosRule.PoNotRel = False;
		       CurRule->PrDimRule.DrPosRule.PoRefKind = RkPresBox;
		       CurRule->PrDimRule.DrPosRule.PoRefIdent = 0;
		       AxisDef = False;
		    }
	       else
		  /* 2eme repere d'un dimensionnement elastique */
		  CurRule->PrDimRule.DrPosRule.PoPosRef = axis;
	       break;
	    default:
	       break;
	 }

}

/*----------------------------------------------------------------------
   CreateConstant                                                  
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         CreateConstant (BasicType constType, indLine wi)

#else  /* __STDC__ */
static void         CreateConstant (constType, wi)
BasicType           constType;
indLine             wi;

#endif /* __STDC__ */

{
   PresVariable       *pPresVar;

   if (!ConstantDef)
     {
	NewConst (wi);
	if (NewVariableDef || VariableDef)
	   /* dans une definition de variable */
	  {
	     pPresVar = &pPSchema->PsVariable[pPSchema->PsNVariables - 1];
	     NewVarListItem (pPresVar, wi);
	     pPresVar->PvItem[pPresVar->PvNItems - 1].ViType = VarText;
	     pPresVar->PvItem[pPresVar->PvNItems - 1].ViConstant =
							pPSchema->PsNConstants;
	  }
	else
	   /* on ne peut etre que dans une regle 'Content' */
	  if (PresBoxDef)
	    /* definition dans une regle Content d'une boite de presentation */
	    {
	     pPSchema->PsPresentBox[CurPresBox - 1].PbContent = ContConst;
	     pPSchema->PsPresentBox[CurPresBox - 1].PbContConstant =
							pPSchema->PsNConstants;
	    }
	  else if (RuleDef && CurRule->PrPresMode == PresFunction
		   && CurRule->PrPresFunction == FnContentRef)
	     /* dans une regle Content d'un element reference ou paire */
	    {
	     CurRule->PrNPresBoxes = 1;
	     CurRule->PrPresBox[0] = pPSchema->PsNConstants;
	    }
     }
   pPSchema->PsConstant[pPSchema->PsNConstants - 1].PdType = constType;
}

/*----------------------------------------------------------------------
   NewCounterOper                                                  
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         NewCounterOper (CounterOp oper, indLine wi)

#else  /* __STDC__ */
static void         NewCounterOper (oper, wi)
CounterOp           oper;
indLine             wi;

#endif /* __STDC__ */

{
   Counter            *pCntr;

   pCntr = &pPSchema->PsCounter[pPSchema->PsNCounters - 1];
   if (pCntr->CnNItems >= MAX_PRES_COUNT_ITEM)
      CompilerMessage (wi, PRS, FATAL, MAX_OPS_ON_A_COUNTER_OVERFLOW, inputLine, LineNum);
   else
     {
	pCntr->CnItem[pCntr->CnNItems].CiCntrOp = oper;
	pCntr->CnItem[pCntr->CnNItems].CiElemType = 0;
	pCntr->CnItem[pCntr->CnNItems].CiAscendLevel = 0;
	pCntr->CnItem[pCntr->CnNItems].CiViewNum = 0;
	pCntr->CnItem[pCntr->CnNItems].CiParamValue = 0;
	pCntr->CnItem[pCntr->CnNItems].CiInitAttr = 0;
	pCntr->CnItem[pCntr->CnNItems].CiReinitAttr = 0;
	pCntr->CnItem[pCntr->CnNItems].CiCondAttr = 0;
	pCntr->CnItem[pCntr->CnNItems].CiCondAttrPresent = FALSE;
	pCntr->CnNItems++;
     }
}

/*----------------------------------------------------------------------
   GetTypedRule   cherche la regle de type ruleType dans la liste de     
   regles qui commence par la regle firstR.                         
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static PtrPRule     GetTypedRule (PRuleType ruleType, PtrPRule firstR)

#else  /* __STDC__ */
static PtrPRule     GetTypedRule (ruleType, firstR)
PRuleType           ruleType;
PtrPRule            firstR;

#endif /* __STDC__ */

{
   PtrPRule            pRule;
   boolean             stop;

   pRule = firstR;
   stop = False;
   do
      if (pRule == NULL)
	 stop = True;
      else if (pRule->PrType == ruleType)
	 stop = True;
      else
	 pRule = pRule->PrNextPRule;
   while (!stop);
   return pRule;
}

/*----------------------------------------------------------------------
   CreateDefaultRule cree une nouvelle regle de presentation par	
   defaut.							
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         CreateDefaultRule ()
#else  /* __STDC__ */
static void         CreateDefaultRule ()
#endif				/* __STDC__ */

{
   if (CurRule != NULL)
      CurRule->PrNextPRule = NextRule;
   CurRule = NextRule;
   GetPresentRule (&NextRule);
   if (NextRule == NULL)
      /* memoire insuffisante */
      CompilerMessage (0, PRS, FATAL, NO_MORE_MEM_LEFT, inputLine, LineNum);

   if (pPSchema->PsFirstDefaultPRule == NULL)
      pPSchema->PsFirstDefaultPRule = CurRule;
   CurRule->PrNextPRule = NULL;
   CurRule->PrCond = NULL;
   CurRule->PrViewNum = 1;
   CurRule->PrPresMode = PresImmediate;
}

/*----------------------------------------------------------------------
   InheritRule transforme la regle de presentation courante en     
   une regle d'heritage de type inheritType.                       
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         InheritRule (InheritMode inheritType)
#else  /* __STDC__ */
static void         InheritRule (inheritType)
InheritMode         inheritType;
#endif /* __STDC__ */
{
   CurRule->PrPresMode = PresInherit;
   CurRule->PrInheritMode = inheritType;
   CurRule->PrInhAttr = False;
   CurRule->PrInhDelta = 0;
   CurRule->PrMinMaxAttr = False;
   CurRule->PrInhMinOrMax = 0;
   CurRule->PrInhUnit = UnRelative;
}


/*----------------------------------------------------------------------
   CheckDefaultRules   verifie que toutes les regles par defaut    
   sont presentes pour la vue 1 et si certaines sont absentes, on  
   les ajoute.                                                     
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         CheckDefaultRules ()

#else  /* __STDC__ */
static void         CheckDefaultRules ()
indLine             wi;

#endif /* __STDC__ */

{
   if (pPSchema->PsFirstDefaultPRule == NextRule)
      pPSchema->PsFirstDefaultPRule = NULL;
   if (GetTypedRule (PtVisibility, pPSchema->PsFirstDefaultPRule) == NULL)
      /* pas de regle Visibility par defaut, on en cree une : */
      /* Visibility: Enclosing =; */
     {
	CreateDefaultRule ();
	CurRule->PrType = PtVisibility;
	InheritRule (InheritParent);
     }
   if (GetTypedRule (PtVertRef, pPSchema->PsFirstDefaultPRule) == NULL)
      /* pas de regle VertRef par defaut, on en cree une : */
      /* VertRef: * . Left; */
     {
	CreateDefaultRule ();
	CurRule->PrType = PtVertRef;
	CurRule->PrPosRule.PoPosDef = VertRef;
	CurRule->PrPosRule.PoPosRef = Left;
	CurRule->PrPosRule.PoDistUnit = UnRelative;
	CurRule->PrPosRule.PoDistAttr = False;
	CurRule->PrPosRule.PoDistance = 0;
	CurRule->PrPosRule.PoRelation = RlSelf;
	CurRule->PrPosRule.PoNotRel = False;
	CurRule->PrPosRule.PoUserSpecified = False;
	CurRule->PrPosRule.PoRefKind = RkPresBox;
	CurRule->PrPosRule.PoRefIdent = 0;
     }
   if (GetTypedRule (PtHorizRef, pPSchema->PsFirstDefaultPRule) == NULL)
      /* pas de regle HorizRef par defaut, on en cree une : */
      /* HorizRef: Enclosed . HRef; */
     {
	CreateDefaultRule ();
	CurRule->PrType = PtHorizRef;
	CurRule->PrPosRule.PoPosDef = HorizRef;
	CurRule->PrPosRule.PoPosRef = HorizRef;
	CurRule->PrPosRule.PoDistUnit = UnRelative;
	CurRule->PrPosRule.PoDistAttr = False;
	CurRule->PrPosRule.PoDistance = 0;
	CurRule->PrPosRule.PoRelation = RlEnclosed;
	CurRule->PrPosRule.PoNotRel = False;
	CurRule->PrPosRule.PoUserSpecified = False;
	CurRule->PrPosRule.PoRefKind = RkPresBox;
	CurRule->PrPosRule.PoRefIdent = 0;
     }
   if (GetTypedRule (PtVertPos, pPSchema->PsFirstDefaultPRule) == NULL)
      /* pas de regle VertPos par defaut, on en cree une : */
      /* VertPos: Top = Previous . Bottom; */
     {
	CreateDefaultRule ();
	CurRule->PrType = PtVertPos;
	CurRule->PrPosRule.PoPosDef = Top;
	CurRule->PrPosRule.PoPosRef = Bottom;
	CurRule->PrPosRule.PoDistUnit = UnRelative;
	CurRule->PrPosRule.PoDistAttr = False;
	CurRule->PrPosRule.PoDistance = 0;
	CurRule->PrPosRule.PoRelation = RlPrevious;
	CurRule->PrPosRule.PoNotRel = False;
	CurRule->PrPosRule.PoUserSpecified = False;
	CurRule->PrPosRule.PoRefKind = RkPresBox;
	CurRule->PrPosRule.PoRefIdent = 0;
     }
   if (GetTypedRule (PtHorizPos, pPSchema->PsFirstDefaultPRule) == NULL)
      /* pas de regle HorizPos par defaut, on en cree une : */
      /* HorizPos: Left = Enclosing . Left; */
     {
	CreateDefaultRule ();
	CurRule->PrType = PtHorizPos;
	CurRule->PrPosRule.PoPosDef = Left;
	CurRule->PrPosRule.PoPosRef = Left;
	CurRule->PrPosRule.PoDistUnit = UnRelative;
	CurRule->PrPosRule.PoDistAttr = False;
	CurRule->PrPosRule.PoDistance = 0;
	CurRule->PrPosRule.PoRelation = RlEnclosing;
	CurRule->PrPosRule.PoNotRel = False;
	CurRule->PrPosRule.PoUserSpecified = False;
	CurRule->PrPosRule.PoRefKind = RkPresBox;
	CurRule->PrPosRule.PoRefIdent = 0;
     }
   if (GetTypedRule (PtHeight, pPSchema->PsFirstDefaultPRule) == NULL)
      /* pas de regle Height par defaut, on en cree une : */
      /* Height: Enclosed . Height; */
     {
	CreateDefaultRule ();
	CurRule->PrType = PtHeight;
	CurRule->PrDimRule.DrPosition = False;
	CurRule->PrDimRule.DrAbsolute = False;
	CurRule->PrDimRule.DrSameDimens = True;
	CurRule->PrDimRule.DrUnit = UnRelative;
	CurRule->PrDimRule.DrAttr = False;
	CurRule->PrDimRule.DrMin = False;
	CurRule->PrDimRule.DrUserSpecified = False;
	CurRule->PrDimRule.DrValue = 0;
	CurRule->PrDimRule.DrRelation = RlEnclosed;
	CurRule->PrDimRule.DrNotRelat = False;
	CurRule->PrDimRule.DrRefKind = RkElType;
	CurRule->PrDimRule.DrRefIdent = 0;
     }
   if (GetTypedRule (PtWidth, pPSchema->PsFirstDefaultPRule) == NULL)
      /* pas de regle Width par defaut, on en cree une : */
      /* Width: Enclosed . Width; */
     {
	CreateDefaultRule ();
	CurRule->PrType = PtWidth;
	CurRule->PrDimRule.DrPosition = False;
	CurRule->PrDimRule.DrAbsolute = False;
	CurRule->PrDimRule.DrSameDimens = True;
	CurRule->PrDimRule.DrUnit = UnRelative;
	CurRule->PrDimRule.DrAttr = False;
	CurRule->PrDimRule.DrMin = False;
	CurRule->PrDimRule.DrUserSpecified = False;
	CurRule->PrDimRule.DrValue = 0;
	CurRule->PrDimRule.DrRelation = RlEnclosed;
	CurRule->PrDimRule.DrNotRelat = False;
	CurRule->PrDimRule.DrRefKind = RkElType;
	CurRule->PrDimRule.DrRefIdent= 0;
     }
   if (GetTypedRule (PtSize, pPSchema->PsFirstDefaultPRule) == NULL)
      /* pas de regle Size par defaut, on en cree une : */
      /* Size: Enclosing =; */
     {
	CreateDefaultRule ();
	CurRule->PrType = PtSize;
	InheritRule (InheritParent);
     }
   if (GetTypedRule (PtStyle, pPSchema->PsFirstDefaultPRule) == NULL)
      /* pas de regle Style par defaut, on en cree une : */
      /* Style: Enclosing =; */
     {
	CreateDefaultRule ();
	CurRule->PrType = PtStyle;
	InheritRule (InheritParent);
     }
   if (GetTypedRule (PtFont, pPSchema->PsFirstDefaultPRule) == NULL)
      /* pas de regle Font par defaut, on en cree une : */
      /* Font: Enclosing =; */
     {
	CreateDefaultRule ();
	CurRule->PrType = PtFont;
	InheritRule (InheritParent);
     }
   if (GetTypedRule (PtUnderline, pPSchema->PsFirstDefaultPRule) == NULL)
      /* pas de regle Underline par defaut, on en cree une : */
      /* Underline: Enclosing =; */
     {
	CreateDefaultRule ();
	CurRule->PrType = PtUnderline;
	InheritRule (InheritParent);
     }
   if (GetTypedRule (PtThickness, pPSchema->PsFirstDefaultPRule) == NULL)
      /* pas de regle Thickness par defaut, on en cree une : */
      /* Thickness: Enclosing =; */
     {
	CreateDefaultRule ();
	CurRule->PrType = PtThickness;
	InheritRule (InheritParent);
     }
   if (GetTypedRule (PtIndent, pPSchema->PsFirstDefaultPRule) == NULL)
      /* pas de regle Indent par defaut, on en cree une : */
      /* Indent: Enclosing =; */
     {
	CreateDefaultRule ();
	CurRule->PrType = PtIndent;
	InheritRule (InheritParent);
     }
   if (GetTypedRule (PtLineSpacing, pPSchema->PsFirstDefaultPRule) == NULL)
      /* pas de regle LineSpacing par defaut, on en cree une : */
      /* LineSpacing: Enclosing =; */
     {
	CreateDefaultRule ();
	CurRule->PrType = PtLineSpacing;
	InheritRule (InheritParent);
     }
   if (GetTypedRule (PtDepth, pPSchema->PsFirstDefaultPRule) == NULL)
      /* pas de regle Depth par defaut, on en cree une : */
      /* Depth: 0; */
     {
	CreateDefaultRule ();
	CurRule->PrType = PtDepth;
	CurRule->PrPresMode = PresImmediate;
	CurRule->PrAttrValue = False;
	CurRule->PrIntValue = 0;
     }
   if (GetTypedRule (PtAdjust, pPSchema->PsFirstDefaultPRule) == NULL)
      /* pas de regle Adjust par defaut, on en cree une : */
      /* Adjust: Enclosing =; */
     {
	CreateDefaultRule ();
	CurRule->PrType = PtAdjust;
	InheritRule (InheritParent);
     }
   if (GetTypedRule (PtJustify, pPSchema->PsFirstDefaultPRule) == NULL)
      /* pas de regle Justify par defaut, on en cree une : */
      /* Justify: Enclosing =; */
     {
	CreateDefaultRule ();
	CurRule->PrType = PtJustify;
	InheritRule (InheritParent);
     }
   if (GetTypedRule (PtLineStyle, pPSchema->PsFirstDefaultPRule) == NULL)
      /* pas de regle LineStyle par defaut, on en cree une : */
      /* LineStyle: Enclosing =; */
     {
	CreateDefaultRule ();
	CurRule->PrType = PtLineStyle;
	InheritRule (InheritParent);
     }
   if (GetTypedRule (PtLineWeight, pPSchema->PsFirstDefaultPRule) == NULL)
      /* pas de regle LineWeight par defaut, on en cree une : */
      /* LineWeight: Enclosing =; */
     {
	CreateDefaultRule ();
	CurRule->PrType = PtLineWeight;
	InheritRule (InheritParent);
     }
   if (GetTypedRule (PtFillPattern, pPSchema->PsFirstDefaultPRule) == NULL)
      /* pas de regle FillPattern par defaut, on en cree une : */
      /* FillPattern: Enclosing =; */
     {
	CreateDefaultRule ();
	CurRule->PrType = PtFillPattern;
	InheritRule (InheritParent);
     }
   if (GetTypedRule (PtBackground, pPSchema->PsFirstDefaultPRule) == NULL)
      /* pas de regle Background par defaut, on en cree une : */
      /* Background: Enclosing =; */
     {
	CreateDefaultRule ();
	CurRule->PrType = PtBackground;
	InheritRule (InheritParent);
     }
   if (GetTypedRule (PtForeground, pPSchema->PsFirstDefaultPRule) == NULL)
      /* pas de regle Foreground par defaut, on en cree une : */
      /* Foreground: Enclosing =; */
     {
	CreateDefaultRule ();
	CurRule->PrType = PtForeground;
	InheritRule (InheritParent);
     }
   if (GetTypedRule (PtHyphenate, pPSchema->PsFirstDefaultPRule) == NULL)
      /* pas de regle Hyphenate par defaut, on en cree une : */
      /* Hyphenate: Enclosing =; */
     {
	CreateDefaultRule ();
	CurRule->PrType = PtHyphenate;
	InheritRule (InheritParent);
     }
   if (GetTypedRule (PtVertOverflow, pPSchema->PsFirstDefaultPRule) == NULL)
      /* pas de regle VertOverflow par defaut, on en cree une : */
      /* VertOverflow: False; */
     {
	CreateDefaultRule ();
	CurRule->PrType = PtVertOverflow;
	CurRule->PrPresMode = PresImmediate;
        CurRule->PrJustify = FALSE;
     }
   if (GetTypedRule (PtHorizOverflow, pPSchema->PsFirstDefaultPRule) == NULL)
      /* pas de regle HorizOverflow par defaut, on en cree une : */
      /* HorizOverflow: False; */
     {
	CreateDefaultRule ();
	CurRule->PrType = PtHorizOverflow;
	CurRule->PrPresMode = PresImmediate;
        CurRule->PrJustify = FALSE;
     }
   CurRule->PrNextPRule = NULL;
}

/*----------------------------------------------------------------------
   CheckForwardRef    verifie que toutes les references de boites  
   en avant sont resolues.                                         
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         CheckForwardRef (indLine wi)

#else  /* __STDC__ */
static void         CheckForwardRef (wi)
indLine             wi;

#endif /* __STDC__ */
{
   int                 box;
   int                 i, j;
   boolean             stop;

   for (box = 0; box < pPSchema->PsNPresentBoxes; box++)
      if (pPSchema->PsPresentBox[box].PbName[0] == ' ')
	{
	   i = 0;
	   stop = False;
	   do
	     {
		if (i + 1 > MAX_IDENTIFIERS)
		   stop = True;
		else if (Identifier[i].SrcIdentCode == RULE_ViewName)
		   /* identificateur de boite */
		   if (Identifier[i].SrcIdentDefRule == box + 1)
		     {
			stop = True;
			/* affiche le nom de l'identificateur */
			printf ("\n");
			for (j = 0; j < Identifier[i].SrcIdentLen; j++)
			   putchar (Identifier[i].SrcIdentifier[j]);
			printf ("\n");
		     }
		i++;
	     }
	   while (!stop);
	   if (i > MAX_IDENTIFIERS)
	      /*erreur dans la declaration FORWARD */
	      CompilerMessage (wi, PRS, FATAL, BAD_USE_OF_KEYWORD_FORWARD, inputLine, LineNum);
	}
}

/*----------------------------------------------------------------------
   LayoutRule                                                      
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         LayoutRule (FunctionType layoutFonct, indLine wi)

#else  /* __STDC__ */
static void         LayoutRule (layoutFonct, wi)
FunctionType        layoutFonct;
indLine             wi;

#endif /* __STDC__ */

{
   PtrPRule            pPRule;

   ConditionEnd ();
   if (DefaultRuleDef)
      CompilerMessage (wi, PRS, FATAL, FORBIDDEN_IN_DEFAULT_RULES, inputLine, LineNum);
   else if (RuleDef && CurType <= MAX_BASIC_TYPE)
      CompilerMessage (wi, PRS, FATAL, BAD_RULE_FOR_A_TERMINAL_ELEM, inputLine, LineNum);
   else if (layoutFonct != FnLine &&
	    layoutFonct != FnNoLine &&
	    layoutFonct != FnShowBox &&
	    pSSchema->SsRule[CurType - 1].SrConstruct == CsChoice)
      CompilerMessage (wi, PRS, FATAL, CANT_USE_RULE_FOR_A_CHOICE, inputLine, LineNum);
   else
     {
	/* verifie qu'il n'y a que des conditions Within parmi les
	   conditions courantes */
	CheckConditions (wi);
	CreatePRule (PtFunction, wi);
	CurRule->PrPresMode = PresFunction;
	CurRule->PrPresFunction = layoutFonct;
	CurRule->PrPresBoxRepeat = False;
	CurRule->PrElement = False;
	CurRule->PrExternal = False;
	CurRule->PrNPresBoxes = 0;
	CurRule->PrPresBoxName[0] = '\0';
	/* verifie que cette regle n'est pas deja presente pour cette */
	/* vue  uniquement pour layoutFonct = FnLine ou FnPage ou FnColumn */
	if (layoutFonct == FnLine || layoutFonct == FnPage
	    || layoutFonct == FnColumn
	    || layoutFonct == FnSubColumn
	    || layoutFonct == FnNoLine)
	  {
	     pPRule = FirstRule;
	     while (pPRule != CurRule)
	       {
		  if (pPRule->PrViewNum == CurView)
		     if (pPRule->PrType == PtFunction)
			if (pPRule->PrPresFunction == FnLine
			    || pPRule->PrPresFunction == FnNoLine
			    || pPRule->PrPresFunction == FnPage
			    || pPRule->PrPresFunction == FnColumn
			    || pPRule->PrPresFunction == FnSubColumn)
#ifdef __COLPAGE__
			   /* on autorise quand meme Page suivi de Column */
			   if (!((layoutFonct == FnColumn && pPRule->PrPresFunction == FnPage) ||
				 (layoutFonct == FnSubColumn && pPRule->PrPresFunction == FnColumn)))
#endif /* __COLPAGE__ */
			      CompilerMessage (wi, PRS, FATAL, ONLY_ONE_PAGE_RULE, inputLine, LineNum);
		  pPRule = pPRule->PrNextPRule;
	       }
	  }
     }
}


/*----------------------------------------------------------------------
   BooleanValue affecte la valeur bool au parametre de presentation courant
   (indique' par InBreakRule, InLineBreakRule, InPageBreakRule, InGatherRule
   ou InInLineRule) de la boite courante ou des regles par defaut.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         BooleanValue (boolean bool, indLine wi)

#else  /* __STDC__ */
static void         BooleanValue (bool, wi)
boolean             bool;
indLine		    wi;

#endif /* __STDC__ */

{
   int                 k;

   if (InBreakRule)
      /* on est dans une regle 'Break' */
     {
	if (DefaultRuleDef)
	   /* on est dans les regles par defaut */
	   /* initialise la secabilite dans toutes les boites de */
	   /* presentation et de tous les elements */
	  {
	     for (k = 0; k < MAX_PRES_BOX; k++)
	       {
		  pPSchema->PsPresentBox[k].PbAcceptPageBreak = bool;
		  pPSchema->PsPresentBox[k].PbAcceptLineBreak = bool;
	       }
	     for (k = 0; k < MAX_RULES_SSCHEMA; k++)
	       {
		  pPSchema->PsAcceptPageBreak[k] = bool;
		  pPSchema->PsAcceptLineBreak[k] = bool;
	       }
	  }
	else
	   /* dans une regle specifique */
	if (PresBoxDef)
	   /* pour une boite de presentation */
	  {
	     pPSchema->PsPresentBox[CurPresBox - 1].PbAcceptPageBreak = bool;
	     pPSchema->PsPresentBox[CurPresBox - 1].PbAcceptLineBreak = bool;
	  }
	else
	   /* pour la boite d'un type d'element */
	  {
	     pPSchema->PsAcceptPageBreak[CurType - 1] = bool;
	     pPSchema->PsAcceptLineBreak[CurType - 1] = bool;
	  }
     }
   else if (InPageBreakRule)
      /* on est dans une regle 'PageBreak' */
     {
	if (DefaultRuleDef)	/* on est dans les regles par defaut */
	   /* initialise la secabilite dans toutes les boites de */
	   /* presentation et de tous les elements */
	  {
	     for (k = 0; k < MAX_PRES_BOX; k++)
		pPSchema->PsPresentBox[k].PbAcceptPageBreak = bool;
	     for (k = 0; k < MAX_RULES_SSCHEMA; k++)
		pPSchema->PsAcceptPageBreak[k] = bool;
	  }
	else
	   /* dans une regle specifique */
	if (PresBoxDef)
	   /* pour une boite de presentation */
	   pPSchema->PsPresentBox[CurPresBox - 1].PbAcceptPageBreak = bool;
	else			/* pour la boite d'un type d'element */
	   pPSchema->PsAcceptPageBreak[CurType - 1] = bool;
     }
   else if (InLineBreakRule)
      /* on est dans une regle 'LineBreak' */
     {
	if (DefaultRuleDef)	/* on est dans les regles par defaut */
	   /* initialise la secabilite dans toutes les boites de */
	   /* presentation et de tous les elements */
	  {
	     for (k = 0; k < MAX_PRES_BOX; k++)
		pPSchema->PsPresentBox[k].PbAcceptLineBreak = bool;
	     for (k = 0; k < MAX_RULES_SSCHEMA; k++)
		pPSchema->PsAcceptLineBreak[k] = bool;
	  }
	else
	   /* dans une regle specifique */
	if (PresBoxDef)
	   /* pour une boite de presentation */
	   pPSchema->PsPresentBox[CurPresBox - 1].PbAcceptLineBreak = bool;
	else			/* pour la boite d'un type d'element */
	   pPSchema->PsAcceptLineBreak[CurType - 1] = bool;
     }
   else if (InGatherRule)
      /* on est dans une regle Gather */
      if (PresBoxDef)
	 /* pour une boite de presentation */
	 pPSchema->PsPresentBox[CurPresBox - 1].PbBuildAll = bool;
      else			/* pour la boite d'un type d'element */
	 pPSchema->PsBuildAll[CurType - 1] = bool;
   else if (InInLineRule)
      /* on est dans une regle InLine */
      {
        if (!bool)
	   LayoutRule (FnNotInLine, wi);
      }
   else
      /* on est dans une regle Justify, Hyphenate, VertOverflow ou
	 HorizOverflow */
      {
	CurRule->PrPresMode = PresImmediate;
	CurRule->PrJustify = bool;
      }
}

/*----------------------------------------------------------------------
   CreationRule                                                    
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         CreationRule (FunctionType creatFonct, indLine wi)

#else  /* __STDC__ */
static void         CreationRule (creatFonct, wi)
FunctionType        creatFonct;
indLine             wi;

#endif /* __STDC__ */

{
   if (DefaultRuleDef)
      /* pas de creation dans les regles par defaut */
      CompilerMessage (wi, PRS, FATAL, FORBIDDEN_IN_DEFAULT_RULES, inputLine, LineNum);
   else if (CurView != 1)
      /* regles de creation seulement dans la vue principale */
      CompilerMessage (wi, PRS, FATAL, RULE_FORBIDDEN_IN_A_VIEW, inputLine, LineNum);
   else if ((creatFonct == FnCreateBefore || creatFonct == FnCreateAfter || creatFonct == FnCreateWith ||
	     creatFonct == FnCreateEnclosing) &&
	    CurType == pSSchema->SsRootElem)
      /* pas de creation avant, apres ou au-dessus de la racine */
      CompilerMessage (wi, PRS, FATAL, PRS_INVALID_RULE_FOR_ROOT_ELEM, inputLine, LineNum);
   else
     {
	CreatePRule (PtFunction, wi);
	CurRule->PrPresMode = PresFunction;
	CurRule->PrPresFunction = creatFonct;
	CurRule->PrPresBoxRepeat = False;
	CurRule->PrElement = False;
	CurRule->PrExternal = False;
	CurRule->PrNPresBoxes = 0;
	CurRule->PrPresBoxName[0] = '\0';
     }
}

/*----------------------------------------------------------------------
   GenerateCopyRule                                                
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         GenerateCopyRule (FunctionType fonctType, indLine wi)

#else  /* __STDC__ */
static void         GenerateCopyRule (fonctType, wi)
FunctionType        fonctType;
indLine             wi;

#endif /* __STDC__ */

{
   PtrPRule            R;

   /* verifie qu'il n'y a que des conditions Within parmi les
      conditions courantes */
   CheckConditions (wi);
   CreatePRule (PtFunction, wi);
   CurRule->PrPresMode = PresFunction;
   CurRule->PrPresFunction = fonctType;
   CurRule->PrPresBoxRepeat = False;
   CurRule->PrElement = False;
   CurRule->PrExternal = False;
   CurRule->PrNPresBoxes = 0;
   CurRule->PrPresBoxName[0] = '\0';
   /* S'il n'y a pas de condition courante, verifie qu'il n'y a pas */
   /* deja une regle Copy ou Content sans condition pour cette vue */
   if (Conditions != NULL)
     {
	R = FirstRule;
	while (R != CurRule)
	  {
	     if (R->PrViewNum == CurView)
		if (R->PrType == PtFunction)
		   if (R->PrPresFunction == FnContentRef || R->PrPresFunction == FnCopy)
		      if (R->PrCond == NULL)
			 CompilerMessage (wi, PRS, FATAL, ONLY_ONE_COPY_CONTENT_RULE, inputLine, LineNum);
	     R = R->PrNextPRule;
	  }
     }
}


/*----------------------------------------------------------------------
   SameConditions compare deux chaines de conditions et retourne   
   True si elles sont egales.                                      
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static boolean      SameConditions (PtrCondition pCond1, PtrCondition pCond2)

#else  /* __STDC__ */
static boolean      SameConditions (pCond1, pCond2)
PtrCondition        pCond1;
PtrCondition        pCond2;

#endif /* __STDC__ */

{
   boolean             different, sameRules, oneRule;
   PtrCondition        curCond1;
   PtrCondition        curCond2;

   curCond1 = pCond1;
   curCond2 = pCond2;
   different = False;
   /* les deux chaines de conditions ont-elles la meme longueur ? */
   while (curCond1 != NULL && curCond2 != NULL)
     {
	curCond1 = curCond1->CoNextCondition;
	curCond2 = curCond2->CoNextCondition;
     }
   if (curCond1 != NULL || curCond2 != NULL)
      /* une chaine est plus longue que l'autre */
      different = True;
   else
      /* les deux chaines de conditions ont la meme longueur */
      /* on compare chaque regle d'une chaine avec toutes celles */
      /* de l'autre, jusqu'a trouver une regle de la 1ere chaine qui */
      /* n'aie pas d'egale dans le deuxieme */
     {
	curCond1 = pCond1;
	while (curCond1 != NULL && !different)
	  {
	     curCond2 = pCond2;
	     oneRule = False;
	     while (curCond2 != NULL && !oneRule)
	       {
		  sameRules = True;
		  if (curCond1->CoNotNegative != curCond2->CoNotNegative)
		     sameRules = False;
		  else if (curCond1->CoCondition != curCond2->CoCondition)
		     sameRules = False;
		  else if (curCond1->CoTarget != curCond2->CoTarget)
		     sameRules = False;
		  else if (curCond1->CoCondition == PcInterval ||
			   curCond1->CoCondition == PcEven ||
			   curCond1->CoCondition == PcOdd ||
			   curCond1->CoCondition == PcOne)
		    {
		       if (curCond1->CoCounter != curCond2->CoCounter)
			  sameRules = False;
		       else if (curCond1->CoCondition == PcInterval)
			  if (curCond1->CoMinCounter != curCond2->CoMinCounter)
			     sameRules = False;
			  else if (curCond1->CoMaxCounter != curCond2->CoMaxCounter)
			     sameRules = False;
		    }
		  else if (curCond1->CoCondition == PcElemType)
		    {
		       if (curCond1->CoTypeElAttr != curCond2->CoTypeElAttr)
			  sameRules = False;
		    }
		  else if (curCond1->CoCondition == PcWithin)
		     if (curCond1->CoRelation != curCond2->CoRelation)
			sameRules = False;
		     else if (curCond1->CoTypeAncestor != curCond2->CoTypeAncestor)
			sameRules = False;
		     else if (curCond1->CoImmediate != curCond2->CoImmediate)
			sameRules = False;
		     else if (curCond1->CoAncestorRel != curCond2->CoAncestorRel)
			sameRules = False;
		     else if (strcmp (curCond1->CoAncestorName, curCond2->CoAncestorName) != 0)
			sameRules = False;
		     else if (strcmp (curCond1->CoSSchemaName, curCond2->CoSSchemaName) != 0)
			sameRules = False;

		  if (sameRules)
		     oneRule = True;
		  else
		     curCond2 = curCond2->CoNextCondition;
	       }
	     if (!oneRule)
		different = True;
	     else
		curCond1 = curCond1->CoNextCondition;
	  }
     }
   return !different;
}


/*----------------------------------------------------------------------
   ProcessLongKeyWord                                              
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         ProcessLongKeyWord (int x, SyntacticCode gCode, indLine wi)

#else  /* __STDC__ */
static void         ProcessLongKeyWord (x, gCode, wi)
int                 x;
SyntacticCode       gCode;
indLine             wi;

#endif /* __STDC__ */

{
   int                 i;
   boolean             assoc;
   Counter            *pCntr;
   PresVariable       *pPresVar;

   switch (x)
	 {
	       /* traitement selon le code du mot-cle */
	    case KWD_PRESENTATION:
	       /* PRESENTATION */
	       break;
	    case KWD_VIEWS:
	       /* VIEWS */
	       ViewDef = True;
	       break;
	    case KWD_PRINT:
	       /* PRINT */
	       ViewDef = False;
	       break;
	    case KWD_COUNTERS:
	       /* COUNTERS */
	       ViewDef = False;
	       CounterDef = True;
	       break;
	    case KWD_CONST:
	       /* CONST */
	       ViewDef = False;
	       CounterDef = False;
	       ConstantDef = True;
	       break;
	    case KWD_VAR:
	       /* VAR */
	       ViewDef = False;
	       CounterDef = False;
	       ConstantDef = False;
	       VariableDef = True;
	       break;
	    case KWD_DEFAULT:
	       /* DEFAULT */
	       ViewDef = False;
	       CounterDef = False;
	       ConstantDef = False;
	       VariableDef = False;
	       DefaultRuleDef = True;
	       pPSchema->PsFirstDefaultPRule = NextRule;
	       FirstRule = NextRule;
	       break;
	    case KWD_BOXES:
	       /* BOXES */
	       /* verifie que toutes les regles par defaut sont presentes pour la */
	       /* vue 1. */
	       CheckDefaultRules ();
	       ViewDef = False;
	       CounterDef = False;
	       ConstantDef = False;
	       VariableDef = False;
	       DefaultRuleDef = False;
	       PresBoxDef = True;
	       break;
	    case KWD_RULES:
	       /* RULES */
	       ViewDef = False;
	       CounterDef = False;
	       ConstantDef = False;
	       VariableDef = False;
	       DefaultRuleDef = False;
	       CheckForwardRef (wi);
	       if (PresBoxDef)
		 {
		    CheckForwardRef (wi);
		    /* verifie les references en avant de boites */
		    PresBoxDef = False;
		 }
	       else
		  /* verifie que toutes les regles par defaut sont presentes pour */
		  /* la vue 1 */
		  CheckDefaultRules ();
	       CurRule->PrNextPRule = NULL;
	       RuleDef = True;
	       break;
	    case KWD_ATTRIBUTES:
	       /* ATTRIBUTES */
	       CheckDefaultRules ();
	       ViewDef = False;
	       CounterDef = False;
	       ConstantDef = False;
	       VariableDef = False;
	       DefaultRuleDef = False;
	       PresBoxDef = False;
	       RuleDef = False;
	       AttributeDef = True;
	       break;
	    case KWD_TRANSMIT:
	       /* TRANSMIT */
	       CheckDefaultRules ();
	       ViewDef = False;
	       CounterDef = False;
	       ConstantDef = False;
	       VariableDef = False;
	       DefaultRuleDef = False;
	       PresBoxDef = False;
	       RuleDef = False;
	       AttributeDef = False;
	       TransmittedCounter = 0;
	       TransmittedElem = 0;
	       EndOfRulesForType ();
	       CurRule->PrNextPRule = NULL;
	       if (NextRule != NULL)
		  free (NextRule);
	       NextRule = NULL;
	       break;
	    case KWD_END:
	       /* END */
	       switch (gCode)
		     {
			   /* r= numero de la regle ou apparait END */
			case RULE_PresentModel:
			   /* fin du progamme */
			   EndOfRulesForType ();
			   CurRule->PrNextPRule = NULL;
			   if (NextRule != NULL)
			      free (NextRule);
			   NextRule = NULL;
			   break;
			case RULE_ViewRuleList:
			   /* fin ViewRuleList */
			   ViewBlock = False;
			   CheckBoxEnd ();
			   EndOfRulesForType ();
			   break;
			case RULE_CondRuleList:
			   /* fin CondRuleList */
			   CurView = 1;		/* la vue par defaut est la premiere */
			   RulesForView = False;
			   CondBlock = False;
			   if (!ViewBlock)
			     {
				CheckBoxEnd ();
				EndOfRulesForType ();
			     }
			   break;
			case RULE_RuleList:
			   RuleBlock = False;
			   Conditions = NULL;
			   if (!ViewBlock && !CondBlock)
			     {
				CheckBoxEnd ();
				EndOfRulesForType ();
			     }
			   break;
		     }

	       break;
	    case KWD_EXPORT /* THOT_EXPORT */ :
	       /* verifie qu'il n'y a pas deja une vue THOT_EXPORT */
	       i = 1;
	       while (i < pPSchema->PsNViews && !pPSchema->PsExportView[i - 1])
		  i++;
	       if (pPSchema->PsExportView[i - 1])
		  CompilerMessage (wi, PRS, FATAL, ONLY_ONE_EXPORT_VIEW_ALLOWED, inputLine, LineNum);	/* deja une vue THOT_EXPORT */
	       else
		  pPSchema->PsExportView[pPSchema->PsNViews - 1] = True;
	       break;
	    case KWD_RANK:
	       /* RANK */
	       NewCounterOper (CntrRank, wi);
	       AncestorSign = 1;
	       AttrInitCounter = False;
	       break;
	    case KWD_RLevel:
	       /* CntrRLevel */
	       NewCounterOper (CntrRLevel, wi);
	       break;
	    case KWD_INIT /* INIT */ :
	       AttrInitCounter = True;
	       break;
	    case KWD_REINIT /* REINIT */ :
	       AttrInitCounter = False;
	       break;
	    case KWD_SET:
	       /* SET */
	       NewCounterOper (CntrSet, wi);
	       break;
	    case KWD_ADD:
	       /* ADD */
	       NewCounterOper (CntrAdd, wi);
	       break;
	    case KWD_Page:
	       if (gCode == RULE_TypeOrPage)	/* dans un compteur */
		 {
		    pCntr = &pPSchema->PsCounter[pPSchema->PsNCounters - 1];
		    pCntr->CnItem[pCntr->CnNItems - 1].CiElemType = PageBreak + 1;
		    pCntr->CnItem[pCntr->CnNItems - 1].CiViewNum = 0;
		 }
	       else
		  /* regle de mise en pages */
		 {
		    if (x == KWD_Page)
		       LayoutRule (FnPage, wi);
		    /* la regle Page s'applique-t-elle a une vue d'elements associes? */
		    assoc = False;
		    if (pSSchema->SsRule[CurType - 1].SrConstruct == CsList)
		       /* la regle s'applique a un element liste */
		      {
			 i = pSSchema->SsRule[CurType - 1].SrListItem;
			 /* les elements de la liste sont-ils des elements associes ? */
			 assoc = pSSchema->SsRule[i - 1].SrAssocElem;
		      }
		    if (assoc)
		       pPSchema->PsAssocPaginated[CurType - 1] = True;
		    else
		       pPSchema->PsPaginatedView[CurView - 1] = True;
		 }
	       break;
	    case KWD_With:
	       /* With */
	       pCntr = &pPSchema->PsCounter[pPSchema->PsNCounters - 1];
	       pCntr->CnItem[pCntr->CnNItems - 1].CiCondAttrPresent = TRUE;
	       break;
	    case KWD_Without:
	       /* Without */
	       pCntr = &pPSchema->PsCounter[pPSchema->PsNCounters - 1];
	       pCntr->CnItem[pCntr->CnNItems - 1].CiCondAttrPresent = FALSE;
	       break;
	    case KWD_TEXT:
	       /* TEXT */
	       CreateConstant (CharString, wi);
	       break;
	    case KWD_SYMBOL:
	       /* SYMBOL */
	       CreateConstant (Symbol, wi);
	       break;
	    case KWD_GRAPHICS:
	       /* GRAPHICS */
	       CreateConstant (GraphicElem, wi);
	       break;
	    case KWD_PICTURE:
	       /* PICTURE */
	       CreateConstant (Picture, wi);
	       break;
	    case KWD_DATE:
	       pPresVar = &pPSchema->PsVariable[pPSchema->PsNVariables - 1];
	       NewVarListItem (pPresVar, wi);
	       pPresVar->PvItem[pPresVar->PvNItems - 1].ViType = VarDate;
	       break;
	    case KWD_FDATE:
	       pPresVar = &pPSchema->PsVariable[pPSchema->PsNVariables - 1];
	       NewVarListItem (pPresVar, wi);
	       pPresVar->PvItem[pPresVar->PvNItems - 1].ViType = VarFDate;
	       break;
	    case KWD_DocName:
	       pPresVar = &pPSchema->PsVariable[pPSchema->PsNVariables - 1];
	       NewVarListItem (pPresVar, wi);
	       pPresVar->PvItem[pPresVar->PvNItems - 1].ViType = VarDocName;
	       break;
	    case KWD_DirName:
	       pPresVar = &pPSchema->PsVariable[pPSchema->PsNVariables - 1];
	       NewVarListItem (pPresVar, wi);
	       pPresVar->PvItem[pPresVar->PvNItems - 1].ViType = VarDirName;
	       break;
	    case KWD_ElemName:
	       pPresVar = &pPSchema->PsVariable[pPSchema->PsNVariables - 1];
	       NewVarListItem (pPresVar, wi);
	       pPresVar->PvItem[pPresVar->PvNItems - 1].ViType = VarElemName;
	       break;
	    case KWD_AttributeName:
	       pPresVar = &pPSchema->PsVariable[pPSchema->PsNVariables - 1];
	       NewVarListItem (pPresVar, wi);
	       pPresVar->PvItem[pPresVar->PvNItems - 1].ViType = VarAttrName;
	       break;
	    case KWD_VALUE:
	       break;
	    case KWD_PageNumber:
	       pPresVar = &pPSchema->PsVariable[pPSchema->PsNVariables - 1];
	       NewVarListItem (pPresVar, wi);
	       pPresVar->PvItem[pPresVar->PvNItems - 1].ViType = VarPageNumber;
	       pPresVar->PvItem[pPresVar->PvNItems - 1].ViStyle = CntArabic;
	       pPresVar->PvItem[pPresVar->PvNItems - 1].ViView = 1;
	       break;
	    case KWD_ARABIC /* ARABIC */ :
	       pPresVar = &pPSchema->PsVariable[pPSchema->PsNVariables - 1];
	       pPresVar->PvItem[pPresVar->PvNItems - 1].ViStyle = CntArabic;
	       break;
	    case KWD_UROMAN /* UROMAN */ :
	       pPresVar = &pPSchema->PsVariable[pPSchema->PsNVariables - 1];
	       pPresVar->PvItem[pPresVar->PvNItems - 1].ViStyle = CntURoman;
	       break;
	    case KWD_LROMAN /* LROMAN */ :
	       pPresVar = &pPSchema->PsVariable[pPSchema->PsNVariables - 1];
	       pPresVar->PvItem[pPresVar->PvNItems - 1].ViStyle = CntLRoman;
	       break;
	    case KWD_UPPERCASE /* UPPERCASE */ :
	       pPresVar = &pPSchema->PsVariable[pPSchema->PsNVariables - 1];
	       pPresVar->PvItem[pPresVar->PvNItems - 1].ViStyle = CntUppercase;
	       break;
	    case KWD_LOWERCASE /* LOWERCASE */ :
	       pPresVar = &pPSchema->PsVariable[pPSchema->PsNVariables - 1];
	       pPresVar->PvItem[pPresVar->PvNItems - 1].ViStyle = CntLowercase;
	       break;
	    case KWD_FORWARD:
	       /* forward */
	       Forward = True;
	       break;
	    case KWD_BEGIN /* BEGIN */ :
	       if (gCode == RULE_ViewRuleList)
		  ViewBlock = True;
	       if (gCode == RULE_RuleList)
		 {
		    RuleBlock = True;
		    ConditionEnd ();
		 }
	       if (gCode == RULE_CondRuleList)
		  CondBlock = True;
	       break;
	    case KWD_Otherwise:
	       /* Otherwise */
	       Conditions = NULL;
	       NewCondition (wi);
	       Conditions->CoCondition = PcDefaultCond;
	       break;
	    case KWD_VertRef:
	       /* VertRef */
	       CreatePRule (PtVertRef, wi);
	       CurRule->PrPosRule.PoPosDef = VertRef;
	       break;
	    case KWD_HorizRef:
	       /* HorizRef */
	       CreatePRule (PtHorizRef, wi);
	       CurRule->PrPosRule.PoPosDef = HorizRef;
	       break;
	    case KWD_Height /* Height */ :
	       if (InRule)
		  if (CurRule->PrType == PtHeight)
		     CurRule->PrDimRule.DrSameDimens = True;
		  else
		     CurRule->PrDimRule.DrSameDimens = False;
	       else
		  CreatePRule (PtHeight, wi);
	       break;
	    case KWD_Width /* Width */ :
	       if (InRule)
		  if (CurRule->PrType == PtWidth)
		     CurRule->PrDimRule.DrSameDimens = True;
		  else
		     CurRule->PrDimRule.DrSameDimens = False;
	       else
		  CreatePRule (PtWidth, wi);
	       break;
	    case KWD_VertPos:
	       /* VertPos */
	       CreatePRule (PtVertPos, wi);
	       AxisDef = True;	/* le prochain repere boite est une definition */
	       break;
	    case KWD_HorizPos:
	       /* HorizPos */
	       CreatePRule (PtHorizPos, wi);
	       AxisDef = True;	/* le prochain repere boite est une definition */
	       break;
	    case KWD_Justify:
	       /* Justify */
	       CreatePRule (PtJustify, wi);
	       break;
	    case KWD_Hyphenate:
	       /* Hyphenate */
	       CreatePRule (PtHyphenate, wi);
	       break;
	    case KWD_VertOverflow:
	       /* VertOverflow */
	       CreatePRule (PtVertOverflow, wi);
	       break;
	    case KWD_HorizOverflow:
	       /* HorizOverflow */
	       CreatePRule (PtHorizOverflow, wi);
	       break;
	    case KWD_LineSpacing:
	       /* LineSpacing */
	       CreatePRule (PtLineSpacing, wi);
	       break;
	    case KWD_Break /* Break */ :
	       CompilerMessage (wi, PRS, INFO, USE_PAGEBREAK, inputLine, LineNum);
	       if (Conditions != NULL)
		  /* un IF precede la regle Break. Erreur */
		  CompilerMessage (wi, PRS, FATAL, FORBIDDEN_CONDITION,
				 inputLine, LineNum);
	       else if (CurView != 1)
		  /* interdit dans une vue non-principale */
		  CompilerMessage (wi, PRS, FATAL, RULE_FORBIDDEN_IN_A_VIEW,
				 inputLine, LineNum);
	       else
		 {
		    InBreakRule = True;
		    ConditionEnd ();
		 }
	       break;
	    case KWD_PageBreak /* PageBreak */ :
	       if (Conditions != NULL)
		  /* un IF precede la regle Break. Erreur */
		  CompilerMessage (wi, PRS, FATAL, FORBIDDEN_CONDITION,
				 inputLine, LineNum);
	       else if (CurView != 1)
		  CompilerMessage (wi, PRS, FATAL, RULE_FORBIDDEN_IN_A_VIEW,
				 inputLine, LineNum);
	       else
		 {
		    InPageBreakRule = True;
		    ConditionEnd ();
		 }
	       break;
	    case KWD_LineBreak /* LineBreak */ :
	       if (Conditions != NULL)
		  /* un IF precede la regle Break. Erreur */
		  CompilerMessage (wi, PRS, FATAL, FORBIDDEN_CONDITION,
				 inputLine, LineNum);
	       else if (CurView != 1)
		  CompilerMessage (wi, PRS, FATAL, RULE_FORBIDDEN_IN_A_VIEW,
				 inputLine, LineNum);
	       else
		 {
		    InLineBreakRule = True;
		    ConditionEnd ();
		 }
	       break;
	    case KWD_InLine:
	       if (DefaultRuleDef)
		  /* pas de regle InLine dans les regles par defaut */
		  CompilerMessage (wi, PRS, FATAL, FORBIDDEN_IN_DEFAULT_RULES,
				   inputLine, LineNum);
	       else
		  InInLineRule = True;
	       break;
	    case KWD_Size:
	       /* Size */
	       CreatePRule (PtSize, wi);
	       break;
	    case KWD_Visibility:
	       /* Visibility */
	       CreatePRule (PtVisibility, wi);
	       break;
	    case KWD_Font:
	       /* Font */
	       CreatePRule (PtFont, wi);
	       break;
	    case KWD_Roman:
	       /* Style roman */
	       CurRule->PrChrValue = 'R';
	       break;
	    case KWD_Bold:
	       /* Style Bold */
	       CurRule->PrChrValue = 'B';
	       break;
	    case KWD_Italics:
	       /* Style Italics */
	       CurRule->PrChrValue = 'I';
	       break;
	    case KWD_BoldItalics:
	       /* Style BoldItalics */
	       CurRule->PrChrValue = 'G';
	       break;
	    case KWD_BoldOblique:
	       /* Style BoldOblique */
	       CurRule->PrChrValue = 'Q';
	       break;
	    case KWD_Oblique:
	       /* Style Oblique */
	       CurRule->PrChrValue = 'O';
	       break;

	    case KWD_Underline:
	       /* Souligne */
	       CreatePRule (PtUnderline, wi);
	       break;
	    case KWD_NoUnderline:
	       /* pas de soulignement */
	       CurRule->PrChrValue = 'N';
	       break;
	    case KWD_Underlined:
	       /* soulignement */
	       CurRule->PrChrValue = 'U';
	       break;
	    case KWD_Overlined:
	       /* surlignement */
	       CurRule->PrChrValue = 'O';
	       break;
	    case KWD_CrossedOut:
	       CurRule->PrChrValue = 'C';
	       break;
	    case KWD_Thickness:
	       /* epaisseur du soulignement */
	       CreatePRule (PtThickness, wi);
	       break;
	    case KWD_Thin:
	       /* soulignement fin */
	       CurRule->PrChrValue = 'N';
	       break;
	    case KWD_Thick:
	       /* soulignement epais */
	       CurRule->PrChrValue = 'T';
	       break;
	    case KWD_Style:
	       /* Style */
	       CreatePRule (PtStyle, wi);
	       break;
	    case KWD_Indent:
	       /* Indent */
	       CreatePRule (PtIndent, wi);
	       break;
	    case KWD_Adjust:
	       /* Adjust */
	       CreatePRule (PtAdjust, wi);
	       break;
	    case KWD_NoBreak1 /* NoBreak1 */ :
	       if (DefaultRuleDef)
		  CompilerMessage (wi, PRS, FATAL, FORBIDDEN_IN_DEFAULT_RULES, inputLine, LineNum);	/* pas de regle NoBreak1
													   * dans les regles par
													   * defaut */
	       else
		  CreatePRule (PtBreak1, wi);
	       break;
	    case KWD_NoBreak2 /* NoBreak2 */ :
	       if (DefaultRuleDef)
		  CompilerMessage (wi, PRS, FATAL, FORBIDDEN_IN_DEFAULT_RULES, inputLine, LineNum);	/* pas de regle NoBreak2
													   * dans les regles par
													   * defaut */
	       else
		  CreatePRule (PtBreak2, wi);
	       break;
	    case KWD_Content /* Content */ :
	       /* autorise' seulement pour les boites de presentation et les */
	       /* elements reference ou paire */
	       if (PresBoxDef)
		 {
		    if (Conditions != NULL)
		       /* un IF precede la regle Content d'une boite de presentation */
		       CompilerMessage (wi, PRS, FATAL, FORBIDDEN_CONDITION,
				      inputLine, LineNum);
		 }
	       else
		  /* on n'est pas dans une boite de presentation */
		  if (!(RuleDef &&
			(pSSchema->SsRule[CurType - 1].SrConstruct == CsReference ||
			 pSSchema->SsRule[CurType - 1].SrConstruct == CsPairedElement)))
		  /* on n'est pas dans les regles d'un element reference */
		  /* ni dans celles d'un element CsPairedElement */
		  CompilerMessage (wi, PRS, FATAL, AUTHORIZED_ONLY_FOR_BOXES_AND_REFS, inputLine, LineNum);
	       else
		  /* cree une regle "Content" pour le type courant */
		 {
		    GenerateCopyRule (FnContentRef, wi);
		    ConditionEnd ();
		 }
	       break;
	    case KWD_Gather /* Gather */ :
	       if (DefaultRuleDef)
		  CompilerMessage (wi, PRS, FATAL, FORBIDDEN_IN_DEFAULT_RULES, inputLine, LineNum);	/* pas de regle Gather
													   * dans les regles par defaut */
	       else if (Conditions != NULL)
		  /* un IF precede la regle Break. Erreur */
		  CompilerMessage (wi, PRS, FATAL, FORBIDDEN_CONDITION,
				 inputLine, LineNum);
	       else if (CurView != 1)
		  /* interdit dans une vue non-principale */
		  CompilerMessage (wi, PRS, FATAL, RULE_FORBIDDEN_IN_A_VIEW, inputLine, LineNum);
	       else
		 {
		    InGatherRule = True;
		    ConditionEnd ();
		 }
	       break;
	    case KWD_Line:
	       /* Line */
	       LayoutRule (FnLine, wi);
	       break;
	    case KWD_NoLine:
	       /* NoLine */
	       LayoutRule (FnNoLine, wi);
	       break;
	    case KWD_Included:
	       IncludedColumn = True;
	       break;
	    case KWD_Column:
	       /* Column */
	       if (IncludedColumn)
		  LayoutRule (FnSubColumn, wi);
	       else
		  LayoutRule (FnColumn, wi);
	       IncludedColumn = False;
#ifdef __COLPAGE__
	       /* la regle Colonne s'applique-t-elle a une vue d'elements associes? */
	       assoc = False;
	       if (pSSchema->SsRule[CurType - 1].SrConstruct == CsList)
		  /* la regle s'applique a un element liste */
		 {
		    i = pSSchema->SsRule[CurType - 1].SrListItem;
		    /* les elements de la liste sont-ils des elements associes ? */
		    assoc = pSSchema->SsRule[i - 1].SrAssocElem;
		 }
	       if (assoc)
		  ;		/*        pPSchema->SPVueAssocAvecCol[CurType - 1] = True; */
	       else
		  pPSchema->PsColumnView[CurView - 1] = True;
#endif /* __COLPAGE__ */
	       break;
	    case KWD_Copy /* Copy */ :
	       if (DefaultRuleDef)
		  CompilerMessage (wi, PRS, FATAL, FORBIDDEN_IN_DEFAULT_RULES, inputLine, LineNum);
	       else if (CurView != 1)
		  /* regle autorisee seulement dans la vue principale */
		  CompilerMessage (wi, PRS, FATAL, RULE_FORBIDDEN_IN_A_VIEW, inputLine, LineNum);
	       else if (RuleDef && pSSchema->SsRule[CurType - 1].SrConstruct != CsReference)
		  /* reserve' aux references */
		  CompilerMessage (wi, PRS, FATAL, VALID_ONLY_FOR_REFS, inputLine, LineNum);
	       else
		 {
		    GenerateCopyRule (FnCopy, wi);
		    ConditionEnd ();
		 }
	       break;
	    case KWD_Depth:
	       /* Depth */
	       CreatePRule (PtDepth, wi);
	       break;
	    case KWD_LineStyle:
	       CreatePRule (PtLineStyle, wi);
	       break;
	    case KWD_LineWeight:
	       CreatePRule (PtLineWeight, wi);
	       break;
	    case KWD_FillPattern:
	       CreatePRule (PtFillPattern, wi);
	       break;
	    case KWD_Background:
	       CreatePRule (PtBackground, wi);
	       break;
	    case KWD_Foreground:
	       CreatePRule (PtForeground, wi);
	       break;
	    case KWD_ShowBox:
	       LayoutRule (FnShowBox, wi);
	       break;
	    case KWD_BackgroundPicture:
	       /* create a new constant */
	       NewConst (wi);
	       LayoutRule (FnBackgroundPicture, wi);
	       CurRule->PrNPresBoxes = 1;
	       CurRule->PrPresBox[0] = pPSchema->PsNConstants;
	       break;
	    case KWD_PictureMode:
	       LayoutRule (FnPictureMode, wi);
	       CurRule->PrNPresBoxes = 1;
	       CurRule->PrPresBox[0] = RealSize;
	       break;
	    case KWD_NormalSize:
	       CurRule->PrPresBox[0] = RealSize;
	       break;
	    case KWD_Scale:
	       CurRule->PrPresBox[0] = ReScale;
	       break;
	    case KWD_RepeatXY:
	       CurRule->PrPresBox[0] = FillFrame;
	       break;
	    case KWD_RepeatX:
	       CurRule->PrPresBox[0] = XRepeat;
	       break;
	    case KWD_RepeatY:
	       CurRule->PrPresBox[0] = YRepeat;
	       break;
	    case KWD_Solid:
	       CurRule->PrChrValue = 'S';
	       break;
	    case KWD_Dashed:
	       CurRule->PrChrValue = '-';
	       break;
	    case KWD_Dotted:
	       CurRule->PrChrValue = '.';
	       break;
	    case KWD_nil /* NULL */ :
	       if (CurRule->PrType == PtHeight || CurRule->PrType == PtWidth)
		  CompilerMessage (wi, PRS, FATAL, FORDBIDDEN_IN_HEIGHT_AND_WIDTH, inputLine, LineNum);
	       else
		 {
		    CurRule->PrPosRule.PoPosDef = NoEdge;
		    AxisDef = True;
		 }
	       break;
	    case KWD_UserSpecified:
	       if (!(RuleDef || AttributeDef))
		  CompilerMessage (wi, PRS, FATAL, FORBIDDEN_IN_A_PRES_BOX, inputLine, LineNum);	/* interdit pour les boites de presentation */
	       else if (CurRule->PrType == PtHeight || CurRule->PrType == PtWidth)
		  CurRule->PrDimRule.DrUserSpecified = True;
	       else if (CurRule->PrType == PtVertPos || CurRule->PrType == PtHorizPos)
		  CurRule->PrPosRule.PoUserSpecified = True;
	       break;
	    case KWD_Refering:
	       /* Refering */
	       SetLevel (RlContainsRef, wi);
	       break;
	    case KWD_Enclosing /* Enclosing */ :
	       if (gCode == RULE_Reference)
		  SetLevel (RlEnclosing, wi);
	       else
		  InheritRule (InheritParent);
	       break;
	    case KWD_Enclosed /* Enclosed */ :
	       if (gCode == RULE_Reference)
		  SetLevel (RlEnclosed, wi);
	       else if (CurRule->PrType == PtVisibility)
		  CompilerMessage (wi, PRS, FATAL, CANT_INHERIT_FROM_ENCLOSED, inputLine, LineNum);
	       else
		  InheritRule (InheritChild);
	       break;
	    case KWD_Creator:
	       if (!PresBoxDef)
		  /* on n'est pas dans une boite de presentation, erreur */
		  CompilerMessage (BeginCopyType, PRS, FATAL, VALID_ONLY_FOR_PRES_BOX, inputLine, LineNum);
	       else if (gCode == RULE_Reference)
		  SetLevel (RlCreator, wi);
	       else
		  InheritRule (InheritCreator);
	       break;
	    case KWD_Previous /* Previous */ :
	       if (gCode == RULE_Reference)
		  SetLevel (RlPrevious, wi);
	       else if (CurRule->PrType == PtVisibility)
		  CompilerMessage (wi, PRS, FATAL, CANT_INHERIT_FROM_PREVIOUS, inputLine, LineNum);
	       else
		  InheritRule (InheritPrevious);
	       break;
	    case KWD_GrandFather:
	       InheritRule (InheritGrandFather);
	       break;
	    case KWD_Next:
	       /* Next */
	       SetLevel (RlNext, wi);
	       break;

	    case KWD_Root:
	       /* Root */
	       SetLevel (RlRoot, wi);
	       break;
	    case KWD_Refered /* Refered */ :
	    case KWD_Referred /* Referred */ :
	       if (gCode == RULE_ElemCondition)
		  /* dans une condition "IF Referred" */
		  Conditions->CoCondition = PcReferred;
	       else if (!AttributeDef && !PresBoxDef)
		  /* autorise' seulement pour les attributs */
		  CompilerMessage (wi, PRS, FATAL, CANT_USE_IF_NOT_A_REF_ATTR, inputLine, LineNum);
	       else if (AttributeDef && pSSchema->SsAttribute[CurAttrNum - 1].AttrType != AtReferenceAttr)
		  /* seulement pour les attributs reference */
		  CompilerMessage (wi, PRS, FATAL, CANT_USE_IF_NOT_A_REF_ATTR, inputLine, LineNum);
	       else
		  SetLevel (RlReferred, wi);
	       break;
	    case KWD_AnyElem:
	       CurRule->PrPosRule.PoRefKind = RkElType;
	       CurRule->PrPosRule.PoRefIdent = MAX_PRES_VARIABLE + 1;
	       break;
	    case KWD_AnyBox:
	       CurRule->PrPosRule.PoRefKind = RkPresBox;
	       CurRule->PrPosRule.PoRefIdent = MAX_PRES_BOX + 1;
	       break;
	    case KWD_ElemWithAttr:
	       break;
	    case KWD_NOT /* NOT */ :
	       if (gCode == RULE_Condition)
		  /* dans une Condition */
		  Conditions->CoNotNegative = False;
	       else
		  /* Not dans  NBoxType */
		 {
		    switch (CurRule->PrType)
			  {
			     case PtVertRef:
			     case PtHorizRef:
			     case PtVertPos:
			     case PtHorizPos:
				CurRule->PrPosRule.PoNotRel = True;
				break;
			     case PtHeight:
			     case PtWidth:
				if (CurRule->PrDimRule.DrPosition)
				   CurRule->PrDimRule.DrPosRule.PoNotRel = True;
				else
				   CurRule->PrDimRule.DrNotRelat = True;
				break;
			     default:
				break;
			  }
		 }
	       break;
	    case KWD_Left /* Left */ :
	       if (gCode == RULE_Adjustment)
		  /* mode d'alignement des lignes */
		  CurRule->PrAdjust = AlignLeft;
	       else
		  ProcessAxis (Left, wi);
	       break;
	    case KWD_VMiddle /* VMiddle */ :
	       if (gCode == RULE_Adjustment)
		  /* mode d'alignement des lignes */
		  CurRule->PrAdjust = AlignCenter;
	       else
		  ProcessAxis (VertMiddle, wi);
	       break;
	    case KWD_VRef:
	       /* VRef */
	       ProcessAxis (VertRef, wi);
	       break;
	    case KWD_Right /* Right */ :
	       if (gCode == RULE_Adjustment)
		  /* mode d'alignement des lignes */
		  CurRule->PrAdjust = AlignRight;
	       else
		  ProcessAxis (Right, wi);
	       break;
	    case KWD_Top:
	       /* Top */
	       ProcessAxis (Top, wi);
	       break;
	    case KWD_HMiddle:
	       /* HMiddle */
	       ProcessAxis (HorizMiddle, wi);
	       break;
	    case KWD_HRef:
	       /* HRef */
	       ProcessAxis (HorizRef, wi);
	       break;
	    case KWD_Bottom:
	       /* Bottom */
	       ProcessAxis (Bottom, wi);
	       break;
	    case KWD_CM /* cm */ :
	       if (LatestNumberAttr)
		  CompilerMessage (wi, PRS, FATAL, BAD_UNIT_FOR_AN_ATTR,
				 inputLine, LineNum);
	       else
		  CurUnit = Centimeter;
	       break;
	    case KWD_MM /* mm */ :
	       if (LatestNumberAttr)
		  CompilerMessage (wi, PRS, FATAL, BAD_UNIT_FOR_AN_ATTR,
				 inputLine, LineNum);
	       else
		  CurUnit = Millimeter;
	       break;
	    case KWD_PT /* pt */ :
	       if (gCode == RULE_Unit)
		  CurUnit = Point;
	       else if (gCode == RULE_SizeInherit)
		  CurRule->PrMinUnit = UnPoint;
	       else if (gCode == RULE_InheritSize)
		  CurRule->PrInhUnit = UnPoint;
	       break;
	    case KWD_PC /* pc */ :
	       if (LatestNumberAttr)
		  CompilerMessage (wi, PRS, FATAL, BAD_UNIT_FOR_AN_ATTR,
				 inputLine, LineNum);
	       else
		  CurUnit = Pica;
	       break;
	    case KWD_IN /* in */ :
	       if (gCode == RULE_Unit)
		  if (LatestNumberAttr)
		     CompilerMessage (wi, PRS, FATAL, BAD_UNIT_FOR_AN_ATTR,
				    inputLine, LineNum);
		  else
		     CurUnit = Inch;
	       break;
	    case KWD_PX /* px */ :
	       CurUnit = ScreenPixel;
	       break;
	    case KWD_EM /* em */ :
	       CurUnit = FontHeight;
	       break;
	    case KWD_EX /* ex */ :
	       CurUnit = XHeight;
	       break;
	    case KWD_Min:
	       if (gCode == RULE_Dimension)
		  CurRule->PrDimRule.DrMin = True;
	       break;
	    case KWD_Yes:
	       /* Yes */
	       BooleanValue (True, wi);
	       break;
	    case KWD_No:
	       /* No */
	       BooleanValue (False, wi);
	       break;
	    case KWD_LeftWithDots:
	       /* LeftWithDots */
	       CurRule->PrAdjust = AlignLeftDots;
	       break;
	    case KWD_IF:
	       /* IF */
	       if (DefaultRuleDef)
		  /* pas de condition dans les regles par defaut */
		  CompilerMessage (wi, PRS, FATAL, FORBIDDEN_IN_DEFAULT_RULES, inputLine, LineNum);
	       else
		 {
		    Conditions = NULL;
		    NewCondition (wi);
		 }
	       break;
	    case KWD_AND /* AND */ :
	       ConditionEnd ();
	       NewCondition (wi);
	       break;
	    case KWD_Target:
	       /* Target */
	       if (RuleDef && pSSchema->SsRule[CurType - 1].SrConstruct != CsReference)
		  /* reserve' aux elements references */
		  CompilerMessage (wi, PRS, FATAL, VALID_ONLY_FOR_REFS, inputLine, LineNum);
	       else if (AttributeDef && pSSchema->SsAttribute[CurAttrNum - 1].AttrType != AtReferenceAttr)
		  /* seulement pour les attributs reference */
		  CompilerMessage (wi, PRS, FATAL, CANT_USE_IF_NOT_A_REF_ATTR, inputLine, LineNum);
	       else
		  Conditions->CoTarget = True;
	       break;
	    case KWD_First:
	       /* First */
	       if (gCode == RULE_FirstSec)
		  FirstInPair = True;
	       else
		  Conditions->CoCondition = PcFirst;
	       break;
	    case KWD_Second:
	       /* Second */
	       SecondInPair = True;
	       break;
	    case KWD_Last:
	       /* Last */
	       Conditions->CoCondition = PcLast;
	       break;

	    case KWD_FirstRef:
	       /* FirstRef */
	       Conditions->CoCondition = PcFirstRef;
	       break;

	    case KWD_LastRef:
	       /* LastRef */
	       Conditions->CoCondition = PcLastRef;
	       break;

	    case KWD_ExternalRef:
	       /* ExternalRef */
	       Conditions->CoCondition = PcExternalRef;
	       break;

	    case KWD_InternalRef:
	       /* InternalRef */
	       Conditions->CoCondition = PcInternalRef;
	       break;

	    case KWD_CopyRef:
	       /* PcCopyRef */
	       Conditions->CoCondition = PcCopyRef;
	       break;

	    case KWD_AnyAttributes:
	       /* AnyAttributs */
	       Conditions->CoCondition = PcAnyAttributes;
	       break;
	    case KWD_FirstAttr:
	       /* FirstAttr */
	       Conditions->CoCondition = PcFirstAttr;
	       break;

	    case KWD_LastAttr:
	       /* LastAttr */
	       Conditions->CoCondition = PcLastAttr;
	       break;

	    case KWD_UserPage:
	       /* UserPage */
	       Conditions->CoCondition = PcUserPage;
	       break;

	    case KWD_StartPage:
	       /* StartPage */
	       Conditions->CoCondition = PcStartPage;
	       break;

	    case KWD_ComputedPage:
	       /* ComputedPage */
	       Conditions->CoCondition = PcComputedPage;
	       break;

	    case KWD_Empty:
	       /* PcEmpty */
	       Conditions->CoCondition = PcEmpty;
	       break;

	    case KWD_MaxRangeVal:
	       /* MaxRangeVal */
	       if (Conditions != NULL)
		  Conditions->CoValCounter = CntMaxVal;
	       CurMinMax = CntMaxVal;
	       break;
	    case KWD_MinRangeVal:
	       /* MinRangeVal */
	       if (Conditions != NULL)
		  Conditions->CoValCounter = CntMinVal;
	       CurMinMax = CntMinVal;
	       break;
	    case KWD_Even:
	       /* Even */
	       Conditions->CoCondition = PcEven;
	       Conditions->CoCounter = 0;
	       InCondPage = True;
	       break;
	    case KWD_Odd:
	       /* Odd */
	       Conditions->CoCondition = PcOdd;
	       Conditions->CoCounter = 0;
	       InCondPage = True;
	       break;
	    case KWD_One:
	       /* One */
	       Conditions->CoCondition = PcOne;
	       Conditions->CoCounter = 0;
	       InCondPage = True;
	       break;
	    case KWD_Immediately:
	       Immediately = True;
	       break;
	    case KWD_Within:
	       Conditions->CoCondition = PcWithin;
	       Conditions->CoImmediate = Immediately;
	       SignGreaterOrLess = False;
	       /* a priori, il n'y a pas de nombre d'ancetres indique' */
	       /* On considere "If Within Type" comme "If Within >0 Type" */
	       Conditions->CoAncestorRel = CondGreater;
	       Conditions->CoRelation = 0;
	       Conditions->CoTypeAncestor = 0;
	       Conditions->CoAncestorName[0] = '\0';
	       Conditions->CoSSchemaName[0] = '\0';
	       InWithinCond = True;
	       break;
	    case KWD_Create:
	       /* Create */
	       CreationRule (FnCreateFirst, wi);
	       CompilerMessage (wi, PRS, INFO, USE_CREATEFIRST, inputLine, LineNum);
	       break;
	    case KWD_CreateBefore:
	       /* CreateBefore */
	       CreationRule (FnCreateBefore, wi);
	       break;
	    case KWD_CreateAfter:
	       /* CreateAfter */
	       CreationRule (FnCreateAfter, wi);
	       break;
	    case KWD_CreateLast:
	       /* CreateLast */
	       CreationRule (FnCreateLast, wi);
	       break;
	    case KWD_CreateFirst:
	       /* CreateFirst */
	       CreationRule (FnCreateFirst, wi);
	       break;
	    case KWD_CreateWith:
	       /* CreateWith */
	       CreationRule (FnCreateWith, wi);
	       break;
	    case KWD_CreateEnclosing:
	       /* CreateEnclosing */
	       CreationRule (FnCreateEnclosing, wi);
	       break;
	    case KWD_Repeated:
	       /* Repeated */
	       CurRule->PrPresBoxRepeat = True;
	       break;
	    case KWD_LESS:
	       VCondLess = True;
	       break;
	    case KWD_GREATER:
	       VCondGreater = True;
	       break;
	    case KWD_EQUAL:
	       CondEqual = True;
	       break;
	    default:
	       break;
	 }
}


/*----------------------------------------------------------------------
   GetTypeNumber        retourne le numero du type d'element dont	
   le nom est typeName.						
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static int          GetTypeNumber (indLine wl, indLine wi, Name typeName)

#else  /* __STDC__ */
static int          GetTypeNumber (wl, wi, typeName)
indLine             wl;
indLine             wi;
Name                typeName;

#endif /* __STDC__ */

{
   int                 i;

   CopyName (typeName, wi, wl);
   /* verifie si le type est declare' dans le schema de structure */
   i = 1;
   while (strcmp (typeName, pSSchema->SsRule[i - 1].SrName)
	  && i < pSSchema->SsNRules)
      i++;
   if (strcmp (typeName, pSSchema->SsRule[i - 1].SrName))
      i = 0;
   else if (InclusionRefName)
      /* on cherche une reference a un document importe' */
     {
	while (i <= pSSchema->SsNRules &&
	       (pSSchema->SsRule[i - 1].SrConstruct != CsReference ||
		!pSSchema->SsRule[i - 1].SrRefImportedDoc))
	   /* ce n'est pas ce que l'on cherche, on continue */
	  {
	     i++;
	     while (strcmp (typeName, pSSchema->SsRule[i - 1].SrName) &&
		    i < pSSchema->SsNRules)
		i++;
	  }
	if (i > pSSchema->SsNRules)
	   i = 0;
     }
   return i;
}


/*----------------------------------------------------------------------
   ProcessTypeName    traite un nom de type d'element		
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         ProcessTypeName (SyntacticCode prevRule, Name typeName, indLine wi, indLine wl)

#else  /* __STDC__ */
static void         ProcessTypeName (prevRule, typeName, wi, wl)
SyntacticCode       prevRule;
Name                typeName;
indLine             wi;
indLine             wl;

#endif /* __STDC__ */

{
   int                 i;
   boolean             found;
   SRule              *pSRule;
   Counter            *pCntr;

   i = GetTypeNumber (wl, wi, typeName);
   if (i == 0)
      CompilerMessage (wi, PRS, FATAL, UNKNOWN_TYPE, inputLine, LineNum);
   /* type inconnu */
   else
     {
	if (pSSchema->SsRule[i - 1].SrConstruct == CsPairedElement)
	   /* c'est un element CsPairedElement */
	  {
	     if (!SecondInPair && !FirstInPair)
		/* le nom du type n'etait pas precede' de First ou Second */
		CompilerMessage (wi, PRS, FATAL, MISSING_FIRST_SECOND,
			       inputLine, LineNum);
	     else if (SecondInPair)
		/* il s'agit du type suivant */
		i++;
	  }
	else
	   /* ce n'est pas un element CsPairedElement */
	if (SecondInPair || FirstInPair)
	   /* le nom du type etait precede' de First ou Second, erreur */
	   CompilerMessage (wi, PRS, FATAL, NOT_A_PAIR,
			  inputLine, LineNum);
	if (prevRule == RULE_FSTypeName)
	   /* un nom de type d'element, avant les regles */
	   /* de presentation d'un attribut */
	   CurElemHeritAttr = i;
	else if (prevRule == RULE_PrintView)
	   /* dans la liste des vues a imprimer */
	  {
	     pSRule = &pSSchema->SsRule[i - 1];
	     /* on n'accepte que les listes d'elements associes */
	     if (pSRule->SrConstruct != CsList)
		CompilerMessage (wi, PRS, FATAL, ASSOC_ELEMS_ONLY, inputLine, LineNum);
	     /* ce n'est pas une liste, erreur */
	     else if (!pSSchema->SsRule[pSRule->SrListItem - 1].SrAssocElem)
		CompilerMessage (wi, PRS, FATAL, ASSOC_ELEMS_ONLY, inputLine, LineNum);
	     /* les constituants de la liste ne sont */
	     /* pas des elements associes */
	     else
	       {
		  if (pPSchema->PsNPrintedViews >= MAX_PRINT_VIEW)
		     CompilerMessage (wi, PRS, FATAL, MAX_VIEWS_TO_BE_PRINTED_OVERFLOW, inputLine, LineNum);
		  /* table des vues a imprimee saturee */
		  else
		    {
		       pPSchema->PsPrintedView[pPSchema->PsNPrintedViews].VpAssoc = True;
		       pPSchema->PsPrintedView[pPSchema->PsNPrintedViews].VpNumber = i;
		       pPSchema->PsNPrintedViews++;
		    }
	       }
	  }
	else if (prevRule == RULE_TypeOrCounter)
	   /* un nom de type au debut d'une regle Transmit */
	  {
	     TransmittedElem = i;
	     if (pPSchema->PsNTransmElems >= MAX_TRANSM_ELEM)
		CompilerMessage (wi, PRS, FATAL, MAX_MANY_TRANSMIT_RULES_FOR_ELEMS_OVERFLOW, inputLine, LineNum);		/* table PsTransmElem saturee */
	     else if (pPSchema->PsElemTransmit[i - 1] > 0)
		CompilerMessage (wi, PRS, FATAL, ELEM_HAS_A_TRANSMIT_RULE, inputLine, LineNum);	/* deja une regle transmit pout l'element */
	     else
	       {
		  pPSchema->PsNTransmElems++;
		  pPSchema->PsElemTransmit[i - 1] = pPSchema->PsNTransmElems;
	       }
	  }
	else if (prevRule == RULE_Transmit)
	  {
	     /* un nom de type a la fin d'une regle Transmit */
	     if (!pSSchema->SsRule[i - 1].SrRefImportedDoc)
		CompilerMessage (wi, PRS, FATAL, NOT_AN_INCLUDED_DOC, inputLine, LineNum);	/* ce n'est pas un document inclus */
	     else if (TransmittedCounter > 0)
		/* c'est une regle Transmit pour un compteur */
	       {
		  pCntr = &pPSchema->PsCounter[TransmittedCounter - 1];
		  pCntr->CnTransmSSchemaAttr[pCntr->CnNTransmAttrs - 1] = i;
	       }
	     else if (TransmittedElem > 0)
		/* un nom de type a la fin d'une regle Transmit pour un */
		/* contenu d'element */
		pPSchema->PsTransmElem[pPSchema->PsNTransmElems - 1].TeTargetDoc = i;
	  }
	else if (prevRule == RULE_VarConst)
	   /* dans une regle "Content" */
	   if (!PresBoxDef)
	      /* on est dans une regle Content d'un element reference ou paire */
	      /* refus: seules les constantes sont acceptees dans cette regle */
	      CompilerMessage (wi, PRS, FATAL, FORBIDDEN_IN_A_REF, inputLine, LineNum);
	   else
	      /* contenu d'une boite de presentation */
	   if (!pSSchema->SsRule[i - 1].SrAssocElem)
	      /* ce n'est pas un element associe, erreur */
	      CompilerMessage (wi, PRS, FATAL, NOT_AN_ASSOC_ELEM, inputLine, LineNum);
	   else
	     {
		pPSchema->PsPresentBox[CurPresBox - 1].PbContent = ContElement;
		pPSchema->PsPresentBox[CurPresBox - 1].PbContElem = i;
		/* ces elements associes sont affiches dans des boites de */
		/* haut ou bas de page */
		pPSchema->PsInPageHeaderOrFooter[i - 1] = True;
		/* cherche dans le schema de structure le type d'element */
		/* qui reference ce type d'element associe */
		i = MAX_BASIC_TYPE;
		found = False;
		do
		  {
		     pSRule = &pSSchema->SsRule[i++];
		     if (pSRule->SrConstruct == CsReference)
			if (pSRule->SrReferredType == pPSchema->PsPresentBox[CurPresBox - 1].PbContElem)
			   found = True;
		  }
		while (!found && i < pSSchema->SsNRules);
		if (found)
		   pPSchema->PsPresentBox[CurPresBox - 1].PbContRefElem = i;
		else
		   CompilerMessage (wi, PRS, FATAL, MISSING_REF_TO_ELEM, inputLine, LineNum);
		/* pas de reference dans le schema, erreur */
	     }
	else if (CounterDef)
	   /* dans une definition de compteur */
	  {
	     pCntr = &pPSchema->PsCounter[pPSchema->PsNCounters - 1];
	     pCntr->CnItem[pCntr->CnNItems - 1].CiElemType = i;
	     pCntr->CnItem[pCntr->CnNItems - 1].CiViewNum = 0;
	  }

	else if (!InRule)
	   /* debut des regles d'un type */
	  {

	     if (pPSchema->PsElemPRule[i - 1] != NULL)
		CompilerMessage (wi, PRS, FATAL, CANT_REDEFINE,
			       inputLine, LineNum);	/* deja traite' */
	     else
	       {
		  pPSchema->PsElemPRule[i - 1] = NextRule;
		  FirstRule = NextRule;
		  CurType = i;
	       }

	  }
	else
	   /* utilisation d'un type dans un positionnement, un */
	   /* dimensionnement, une copie ou un contenu de boite */
	  {
	     if ((CurRule->PrType == PtVertRef
		  || CurRule->PrType == PtHorizRef)
		 && CurRule->PrPosRule.PoRelation != RlEnclosed)
		CompilerMessage (wi, PRS, FATAL, ONLY_ENCLOSED_AND_ARE_ALLOWED, inputLine, LineNum);
	     else
		switch (CurRule->PrType)
		      {
			 case PtVertRef:
			 case PtHorizRef:
			 case PtVertPos:
			 case PtHorizPos:
			    CurRule->PrPosRule.PoRefKind = RkElType;
			    CurRule->PrPosRule.PoRefIdent = i;
			    break;
			 case PtHeight:
			 case PtWidth:
			    if (CurRule->PrDimRule.DrPosition)
			      {
				 CurRule->PrDimRule.DrPosRule.PoRefKind = RkElType;
				 CurRule->PrDimRule.DrPosRule.PoRefIdent = i;
			      }
			    else
			      {
				 CurRule->PrDimRule.DrRefKind = RkElType;
				 CurRule->PrDimRule.DrRefIdent = i;
			      }
			    break;
			 case PtFunction:
			    if (CurRule->PrPresFunction == FnCopy)
			      {
				 CurRule->PrElement = True;
				 CurRule->PrExternal = False;
				 CurRule->PrNPresBoxes = 1;
				 CurRule->PrPresBox[0] = i;
			      }
			    break;
			 default:
			    break;
		      }
	  }
     }
   FirstInPair = False;
   SecondInPair = False;
   InclusionRefName = False;
}


/*----------------------------------------------------------------------
   NewBoxName      traite un nouveau nom de boite de presentation  
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         NewBoxName (indLine wl, indLine wi, int identnum)

#else  /* __STDC__ */
static void         NewBoxName (wl, wi, identnum)
indLine             wl;
indLine             wi;
int                 identnum;

#endif /* __STDC__ */

{
   PresentationBox    *pPresBox;

   pPresBox = &pPSchema->PsPresentBox[pPSchema->PsNPresentBoxes];
   pPSchema->PsNPresentBoxes++;
   Identifier[identnum].SrcIdentDefRule = pPSchema->PsNPresentBoxes;
   if (Forward)
     {
	pPresBox->PbName[0] = ' ';
	Forward = False;
     }
   else
     {
	CopyName (pPresBox->PbName, wi, wl);
	pPresBox->PbFirstPRule = NextRule;
	FirstRule = NextRule;
	CurPresBox = pPSchema->PsNPresentBoxes;
     }
   /* a priori la boite a un contenu libre */
   pPresBox->PbContent = FreeContent;
}

/*----------------------------------------------------------------------
   IntAttribute  traite un nom d'attribut qui apparait a la place d'une 
   valeur numerique entiere.                                       
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         IntAttribute (int attr, SyntacticCode prevRule, indLine wi)

#else  /* __STDC__ */
static void         IntAttribute (attr, prevRule, wi)
int                 attr;
SyntacticCode       prevRule;
indLine             wi;

#endif /* __STDC__ */

{
   if (pSSchema->SsAttribute[attr - 1].AttrType != AtNumAttr)
      /* not an attribute of type Integer, error */
      CompilerMessage (wi, PRS, FATAL, BAD_ATTR_TYPE, inputLine, LineNum);
   else
      switch (prevRule)
	    /* prevRule = number of the rule where the attribute name appears*/
	    {
	       case RULE_NbInherit:
		  /* Integer */
		  CurRule->PrIntValue = attr;
		  CurRule->PrAttrValue = True;
		  break;
	       case RULE_DimRatioA:
		  /* DimRatioA */
		  CurUnit = Percent;
		  LatestNumber = attr;
		  LatestNumberAttr = True;
		  PrevSign = 1;	/* sign =  "+" */
		  CurRule->PrDimRule.DrUnit = UnPercent;
		  break;
	       case RULE_IntPartA:
		  /* IntPartA */
		  LatestNumber = attr;
		  LatestNumberAttr = True;
		  break;
	       case RULE_PosIntegerA:
		  /* PosIntegerA */
		  CurRule->PrInhDelta = attr;
		  CurRule->PrInhAttr = True;
		  CurRule->PrInhUnit = UnRelative;
		  break;
	       case RULE_maximumA:
		  /* maximumA */
		  CurRule->PrInhMinOrMax = attr;
		  CurRule->PrMinMaxAttr = True;
		  break;
	       case RULE_NegIntegerA:
		  /* NegIntegerA */
		  CurRule->PrInhDelta = -attr;
		  CurRule->PrInhAttr = True;
		  CurRule->PrInhUnit = UnRelative;
		  break;
	       case RULE_minimumA:
		  /* minimumA */
		  CurRule->PrInhMinOrMax = attr;
		  CurRule->PrMinMaxAttr = True;
		  break;
	       case RULE_SizeA:
		  /* SizeA */
		  CurRule->PrMinValue = attr;
		  CurRule->PrMinUnit = UnRelative;
		  CurRule->PrMinAttr = True;
		  break;
	       case RULE_PosSizeA:
		  /* PosSizeA */
		  CurRule->PrInhDelta = attr;
		  CurRule->PrInhUnit = UnRelative;
		  CurRule->PrInhAttr = True;
		  break;
	       case RULE_MaxSizeA:
		  /* MaxSizeA */
		  CurRule->PrInhMinOrMax = attr;
		  CurRule->PrMinMaxAttr = True;
		  break;
	       case RULE_NegSizeA:
		  /* NegSizeA */
		  CurRule->PrInhDelta = -attr;
		  CurRule->PrInhUnit = UnRelative;
		  CurRule->PrInhAttr = True;
		  break;
	       case RULE_MinSizeA:
		  /* MinSizeA */
		  CurRule->PrInhMinOrMax = attr;
		  CurRule->PrMinMaxAttr = True;
		  break;
	       default:
		  break;
	    }
}

/*----------------------------------------------------------------------
   ProcessName         traite un nom.                              
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         ProcessName (SyntacticCode gCode, int identnum, SyntacticCode prevRule, indLine wl, indLine wi)
#else  /* __STDC__ */
static void         ProcessName (gCode, identnum, prevRule, wl, wi)
SyntacticCode       gCode;
int                 identnum;
SyntacticCode       prevRule;
indLine             wl;
indLine             wi;
#endif /* __STDC__ */

{
   Name                n;
   int                 i, j;
   boolean             ok, new;
   Counter            *pCntr;
   PresVariable       *pPresVar;
   PresentationBox    *pPresBox;
   PresVarItem        *pVarElem;
   PtrPRule            pPRule;
   PtrCondition        pCond;
   TtAttribute        *pAttr;

   switch (gCode)
	 {
	       /* gCode = numero de la regle ou apparait le nom */
	    case RULE_TypeName /* TypeName */ :
	       if (prevRule == RULE_PresentModel)
		  /* nom de la structure generique */
		 {
		    CopyName (n, wi, wl);
		    /* lit le schema de structure compile' */
		    GetSchStruct (&pSSchema);
		    if (pSSchema == NULL)
		       TtaDisplaySimpleMessage (FATAL, PRS, NO_MORE_MEM_LEFT);	/* memoire insuffisante */
		    if (!ReadStructureSchema (n, pSSchema))
		       TtaDisplaySimpleMessage (FATAL, PRS, MISSING_STRUCT_SCHEM);
		    /* echec lecture du schema de structure */
		    else if (strcmp (n, pSSchema->SsName))
		       CompilerMessage (wi, PRS, FATAL, STRUCT_SCHEM_DOES_NOT_MATCH, inputLine, LineNum);
		    else
		      {
			 Initialize ();
			 strncpy (pPSchema->PsStructName, n, MAX_NAME_LENGTH);
		      }
		 }
	       else if (prevRule == RULE_ExtStruct)
		 {
		    /* verifier que ce schema de structure externe existe et qu'il */
		    /* contient bien le type CopyType */
		    CopyName (n, wi, wl);
		    if (!ReadStructureSchema (n, pExternalSS))
		       /* echec lecture du schema de structure */
		       CompilerMessage (wi, PRS, FATAL, CANT_LOAD_SCHEMA, inputLine, LineNum);
		    else
		       /* le schema de structure externe a ete charge' */
		      {
			 i = 1;
			 while (strcmp (CopyType, pExternalSS->SsRule[i - 1].SrName) != 0
				&& i < pExternalSS->SsNRules)
			    i++;
			 if (strcmp (CopyType, pExternalSS->SsRule[i - 1].SrName) != 0)
			    /* type inconnu */
			    if (PresBoxDef || InWithinCond)
			       /* on est dans une boite de presentation, erreur */
			       CompilerMessage (BeginCopyType, PRS, FATAL, NOT_FOR_PRES_BOX, inputLine, LineNum);
			    else
			      {
				 /* on suppose que c'est un nom de boite de presentation */
				 /* definie dans un autre schema de presentation (pour */
				 /* la presentation des references externes) */
				 if ((RuleDef && pSSchema->SsRule[CurType - 1].SrRefTypeNat[0] == '\0') ||
				     (AttributeDef && pSSchema->SsAttribute[CurAttrNum - 1].AttrTypeRefNature[0] == '\0'))
				    /* la regle ne s'applique pas a` une reference externe */
				    CompilerMessage (BeginCopyType, PRS, FATAL, AUTHORIZED_ONLY_FOR_EXT_REFS, inputLine, LineNum);
				 else
				    /* la regle s'applique a` une reference externe */
				 if (CurRule->PrNPresBoxes != 0)
				    /* deja une boite dans la regle, on refuse */
				    CompilerMessage (wi, PRS, FATAL, REDEFINITION_OF_CONTENT, inputLine, LineNum);
				 else
				   {
				      strncpy (CurRule->PrPresBoxName, CopyType, MAX_NAME_LENGTH);
				      /* indique qu'on copie une boite de presentation et non un element */
				      CurRule->PrElement = False;
				   }
			      }
			 else if (InWithinCond)
			   {
			      strncpy (Conditions->CoAncestorName, CopyType, MAX_NAME_LENGTH);
			      CopyName (Conditions->CoSSchemaName, wi, wl);
			      Conditions->CoTypeAncestor = 0;
			   }
			 else
			    /* le type a copier existe, il a le numero i */
			   {
			      /* indique qu'on copie un element et non une boite de presentation */
			      CurRule->PrElement = True;
			      /* nom du type d'element a copier */
			      strncpy (CurRule->PrPresBoxName, CopyType, MAX_NAME_LENGTH);
			      /* indique que PrPresBoxName est un nom de type defini */
			      /* dans un autre schema */
			      CurRule->PrExternal = True;
			   }
			 /* marque que ce nom externe est traite' */
			 CopyType[0] = '\0';
			 BeginCopyType = 0;
		      }
		 }
	       else if (prevRule == RULE_ElemCondition)
		 {
		    /* On est dans une condition */
		    CopyName (n, wi, wl);
		    /* verifie si le type existe dans le schema de structure */
		    i = 1;
		    while (strcmp (n, pSSchema->SsRule[i - 1].SrName)
			   && i < pSSchema->SsNRules)
		       i++;
		    if (InWithinCond)
		       /* un nom de type d'element dans une condition Within */
		      {
			 if (strcmp (n, pSSchema->SsRule[i - 1].SrName))
			    /* type inconnu */
			   {
			      /* c'est peut-etre un type defini dans un autre schema */
			      CopyName (CopyType, wi, wl);
			      BeginCopyType = wi;
			   }
			 else
			    /* le type existe, il a le numero i */
			    Conditions->CoTypeAncestor = i;
			 /* On remet Immediately a faux seulement ici */
			 Immediately = False;
		      }
		    else
		       /* on est dans une condition, mais pas apres Within */
		      {
			 if (!strcmp (n, pSSchema->SsRule[i - 1].SrName))
			    /* c'est bien un nom de type d'element */
			   {
			      /* la regle est-elle bien pour un attribut ? */
			      if (!AttributeDef)
				 CompilerMessage (wi, PRS, FATAL, VALID_ONLY_FOR_ATTRIBUTES,
						inputLine, LineNum);
			      else
				{
				   Conditions->CoCondition = PcElemType;
				   Conditions->CoTypeElAttr = i;
				}
			   }
			 else
			    /* peut-etre est-ce un nom d'attribut */
			   {
			      /* cherche ce nom parmi les attributs du schema de structure */
			      i = 1;
			      while (strcmp (n, pSSchema->SsAttribute[i - 1].AttrName)
				     && i < pSSchema->SsNAttributes)
				 i++;
			      if (strcmp (n, pSSchema->SsAttribute[i - 1].AttrName))
				 /* on ne l'a pas trouve, erreur */
				 CompilerMessage (wi, PRS, FATAL, UNDECLARED_IDENTIFIER,
						inputLine, LineNum);
			      else
				 /* c'est un nom d'attribut dans une condition */
				{
				   /* change le type de ce nom qui devient un nom d'attribut */
				   Identifier[identnum].SrcIdentCode = RULE_AttrName;
				   /* traite ce nom d'attribut */
				   Conditions->CoCondition = PcAttribute;
				   Conditions->CoTypeElAttr = i;
				}
			   }
		      }
		 }
	       else if (prevRule == RULE_CountFunction)
		 {
		    /* On est dans un CntrRLevel */
		    pCntr = &pPSchema->PsCounter[pPSchema->PsNCounters - 1];
		    CopyName (n, wi, wl);
		    /* verifie si le type existe dans le schema de structure */
		    i = 1;
		    while (strcmp (n, pSSchema->SsRule[i - 1].SrName)
			   && i < pSSchema->SsNRules)
		       i++;
		    if (strcmp (n, pSSchema->SsRule[i - 1].SrName))
		       /* type inconnu */
		       CompilerMessage (wi, PRS, FATAL, UNKNOWN_TYPE, inputLine, LineNum);
		    else
		       /* le type existe, il a le numero i */
		       pCntr->CnItem[0].CiElemType = i;
		 }
	       else
		  ProcessTypeName (prevRule, n, wi, wl);
	       break;

	    case RULE_ViewName:
	       /* ViewName */
	       if (ViewDef)
		  /* declaration de vue */
		 {
		    if (pPSchema->PsNViews >= MAX_VIEW)
		       CompilerMessage (wi, PRS, FATAL, MAX_VIEWS_OVERFLOW, inputLine, LineNum);
		    /* Trop de vues */
		    else if (Identifier[identnum].SrcIdentDefRule != 0)
		       CompilerMessage (wi, PRS, FATAL, CANT_REDECLARE_NAME, inputLine, LineNum);
		    /* Vue deja declaree */
		    else
		       /* ajoute une vue dans la table des vues */
		      {
			 CopyName (pPSchema->PsView[pPSchema->PsNViews], wi, wl);
			 pPSchema->PsNViews++;
			 Identifier[identnum].SrcIdentDefRule = pPSchema->PsNViews;
		      }
		 }
	       else if (prevRule == RULE_PrintView)
		  /* dans la liste des vues a imprimer */
		  if (Identifier[identnum].SrcIdentDefRule == 0)
		     /* ce nom n'a pas ete declare comme nom de vue, voyons si */
		     /* ce n'est pas un identificateur de type */
		    {
		       ProcessTypeName (prevRule, n, wi, wl);
		       Identifier[identnum].SrcIdentCode = RULE_TypeName;
		       /* changement de type, c'est */
		       /* maintenant un identificateur de type structure */
		    }
		  else
		     /* ce nom a ete declare comme nom de vue */
		    {
		       if (pPSchema->PsNPrintedViews >= MAX_PRINT_VIEW)
			  CompilerMessage (wi, PRS, FATAL, MAX_VIEWS_TO_BE_PRINTED_OVERFLOW, inputLine, LineNum);
		       /* table des vues a imprimee saturee */
		       else
			 {
			    pPSchema->PsPrintedView[pPSchema->PsNPrintedViews].VpAssoc = False;
			    pPSchema->PsPrintedView[pPSchema->PsNPrintedViews].VpNumber =
			       Identifier[identnum].SrcIdentDefRule;
			    pPSchema->PsNPrintedViews++;
			 }
		    }
	       else if (CounterDef)
		 {
		    pCntr = &pPSchema->PsCounter[pPSchema->PsNCounters - 1];
		    pCntr->CnItem[pCntr->CnNItems - 1].CiViewNum =
		       Identifier[identnum].SrcIdentDefRule;
		 }
	       else if (prevRule == RULE_CounterAttrPage)
		  if (Identifier[identnum].SrcIdentDefRule == 0)
		     CompilerMessage (wi, PRS, FATAL, UNDECLARED_IDENTIFIER, inputLine, LineNum);		/* Vue non declaree */
		  else
		    {
		       pPresVar = &pPSchema->PsVariable[pPSchema->PsNVariables - 1];
		       pPresVar->PvItem[pPresVar->PvNItems - 1].ViView =
			  Identifier[identnum].SrcIdentDefRule;
		    }
	       else if (prevRule == RULE_ViewRules)
		  /* dans une instruction 'in ViewName ...' */
		  if (Identifier[identnum].SrcIdentDefRule == 0)
		     /* Vue non declaree */
		     CompilerMessage (wi, PRS, FATAL, UNDECLARED_IDENTIFIER, inputLine, LineNum);
		  else if (Identifier[identnum].SrcIdentDefRule == 1)
		     /* C'est la vue* principale, erreur */
		     CompilerMessage (wi, PRS, FATAL, FORBIDDEN_IN_THE_MAIN_VIEW, inputLine, LineNum);
		  else
		    {
		       CurView = Identifier[identnum].SrcIdentDefRule;
		       RulesForView = True;
		    }
	       break;
	    case RULE_CounterName:
	       /* CounterName */
	       if (CounterDef)
		  /* declaration de compteur */
		 {
		    if (pPSchema->PsNCounters >= MAX_PRES_COUNTER)
		       CompilerMessage (wi, PRS, FATAL, MAX_COUNTERS_OVERFLOW, inputLine, LineNum);
		    else if (Identifier[identnum].SrcIdentDefRule != 0)
		       CompilerMessage (wi, PRS, FATAL, CANT_REDECLARE_NAME, inputLine, LineNum);
		    else
		      {
			 pCntr = &pPSchema->PsCounter[pPSchema->PsNCounters];
			 pPSchema->PsNCounters++;
			 pCntr->CnNItems = 0;
			 pCntr->CnNTransmAttrs = 0;
			 pCntr->CnNPresBoxes = 0;
			 pCntr->CnNCreators = 0;
			 pCntr->CnNCreatedBoxes = 0;
			 Identifier[identnum].SrcIdentDefRule = pPSchema->PsNCounters;
		      }
		 }
	       else if (Identifier[identnum].SrcIdentDefRule == 0)
		  /* on n'a pas encore rencontre' ce nom */
		  if (prevRule != RULE_CounterAttrPage)
		     CompilerMessage (wi, PRS, FATAL, UNDECLARED_IDENTIFIER, inputLine, LineNum);		/* compteur non declare' */
		  else
		     /* on vient de la regle CounterAttrPage; peut-etre est-ce un */
		     /* nom d'attribut */
		    {
		       /* cherche ce nom parmi les attributs du schema de structure */
		       CopyName (n, wi, wl);
		       i = 1;
		       while (strcmp (n, pSSchema->SsAttribute[i - 1].AttrName)
			      && i < pSSchema->SsNAttributes)
			  i++;
		       if (strcmp (n, pSSchema->SsAttribute[i - 1].AttrName))
			  CompilerMessage (wi, PRS, FATAL, UNDECLARED_IDENTIFIER, inputLine, LineNum);	/* on ne l'a pas trouve, erreur */
		       else
			  /* c'est un nom d'attribut apres VALUE */
			 {
			    /* change le type de ce nom qui devient un nom d'attribut */
			    Identifier[identnum].SrcIdentCode = RULE_AttrName;
			    /* traite ce nom d'attribut */
			    pPresVar = &pPSchema->PsVariable[pPSchema->PsNVariables - 1];
			    NewVarListItem (pPresVar, wi);
			    pPresVar->PvItem[pPresVar->PvNItems - 1].ViType = VarAttrValue;
			    pPresVar->PvItem[pPresVar->PvNItems - 1].ViStyle = CntArabic;
			    pPresVar->PvItem[pPresVar->PvNItems - 1].ViAttr = i;
			 }
		    }

	       else if (prevRule == RULE_CounterAttrPage)
		  /* un compteur dans une definition de variable */
		 {
		    pPresVar = &pPSchema->PsVariable[pPSchema->PsNVariables - 1];
		    pPresVar->PvItem[pPresVar->PvNItems].ViType = VarCounter;
		    pPresVar->PvItem[pPresVar->PvNItems].ViCounter =
		       Identifier[identnum].SrcIdentDefRule;
		    pPresVar->PvItem[pPresVar->PvNItems].ViCounterVal = CurMinMax;
		    pPresVar->PvNItems++;
		    if (NewVariableDef)
		       /* definition de var. dans une regle Content */
		      {
			 pCntr = &pPSchema->PsCounter[Identifier[identnum].SrcIdentDefRule - 1];
			 /* ce compteur est utilise' dans la boite de presentation */
			 if (pCntr->CnNPresBoxes < MAX_PRES_COUNT_USER)
			   {
			      pCntr->CnPresBox[pCntr->CnNPresBoxes] = CurPresBox;
			      if (CurMinMax == CntMaxVal || CurMinMax == CntMinVal)
				 pCntr->CnMinMaxPresBox[pCntr->CnNPresBoxes] = True;
			      else
				 pCntr->CnMinMaxPresBox[pCntr->CnNPresBoxes] = False;
			      pCntr->CnNPresBoxes++;
			   }
		      }
		    CurMinMax = CntCurVal;
		 }
	       else if (prevRule == RULE_ElemCondition)
		  /* dans une condition */
		 {
		    Conditions->CoCounter = Identifier[identnum].SrcIdentDefRule;
		    pCntr = &pPSchema->PsCounter[Identifier[identnum].SrcIdentDefRule - 1];
		    /* ce compteur est-il deja utilise' par la boite courante */
		    /* comme condition de creation ? */
		    if (PresBoxDef)
		       j = CurPresBox;
		    else
		       j = CurType;
		    new = True;
		    for (i = 0; i < pCntr->CnNCreators; i++)
		       if (pCntr->CnCreator[i] == j
			   && pCntr->CnPresBoxCreator[i] == PresBoxDef)
			  new = False;
		    if (new)
		       /* ce compteur n'est pas encore utilise' par la boite */
		       /* courante dans ses regles de creation, on le marque. */
		      {
			 pCntr->CnCreator[pCntr->CnNCreators] = j;
			 pCntr->CnPresBoxCreator[pCntr->CnNCreators] = PresBoxDef;
			 if (CurMinMax == CntMaxVal || CurMinMax == CntMinVal)
			    pCntr->CnMinMaxCreator[pCntr->CnNCreators] = True;
			 else
			    pCntr->CnMinMaxCreator[pCntr->CnNCreators] = False;
			 CurMinMax = CntCurVal;
			 pCntr->CnNCreators++;
		      }
		 }
	       else if (prevRule == RULE_TypeOrCounter)
		  /* un compteur au debut d'une regle Transmit */
		 {
		    TransmittedCounter = Identifier[identnum].SrcIdentDefRule;
		    pCntr = &pPSchema->PsCounter[TransmittedCounter - 1];
		    if (pCntr->CnNTransmAttrs >= MAX_TRANSM_ATTR)
		      {
			 CompilerMessage (wi, PRS, FATAL, MAX_TRANSMIT_RULES_FOR_COUNTER_OVERFLOW, inputLine, LineNum);	/* trop de regles transmit pour ce compteur */
			 TransmittedCounter = 0;
		      }
		    else
		       pCntr->CnNTransmAttrs++;
		 }
	       break;
	    case RULE_AttrName:
	       /* AttrName */
	       /* cherche ce nom d'attribut dans le schema de structure */
	       CopyName (n, wi, wl);
	       i = 1;
	       while (strcmp (n, pSSchema->SsAttribute[i - 1].AttrName)
		      && i < pSSchema->SsNAttributes)
		  i++;
	       if (strcmp (n, pSSchema->SsAttribute[i - 1].AttrName))
		  CompilerMessage (wi, PRS, FATAL, NO_SUCH__ATTR, inputLine, LineNum);
	       /* on ne l'a pas trouve, erreur */

	       else if (!(prevRule == RULE_Attr
			  || prevRule == RULE_CountFunction
			  || prevRule == RULE_CondAttr
			  || prevRule == RULE_Function
			  || prevRule == RULE_AttrValue
			  || prevRule == RULE_AttrRelat
			  || prevRule == RULE_ElemCondition
			  || prevRule == RULE_CounterAttrPage
			  || prevRule == RULE_BoxType))
		  /* c'est un nom d'attribut a la place d'une valeur numerique */
		  IntAttribute (i, prevRule, wi);

	       else if (prevRule == RULE_Function || prevRule == RULE_CounterAttrPage)
		  /* c'est un nom d'attribut dans une variable */
		 {
		    pPresVar = &pPSchema->PsVariable[pPSchema->PsNVariables - 1];
		    if (pPresVar->PvNItems > 0)
		       /* ce n'est pas le premier element de la variable */
		       /* si le 1er element n'est pas du texte, on n'accepte pas */
		       /* le nouvel element */
		       if (pPresVar->PvItem[0].ViType == VarText)
			  if (pPSchema->PsConstant[pPresVar->PvItem[0].ViConstant - 1].PdType != CharString)
			     CompilerMessage (wi, PRS, FATAL, MAX_FUNCTIONS_IN_A_VARIABLE_OVERFLOW, inputLine, LineNum);

		    if (pPresVar->PvNItems >= MAX_PRES_VAR_ITEM)
		       CompilerMessage (wi, PRS, FATAL, MAX_FUNCTIONS_IN_A_VARIABLE_OVERFLOW, inputLine, LineNum);
		    /* variable trop longue */
		    else
		      {
			 pPresVar->PvItem[pPresVar->PvNItems].ViType = VarAttrValue;
			 pPresVar->PvItem[pPresVar->PvNItems].ViStyle = CntArabic;
			 pPresVar->PvItem[pPresVar->PvNItems].ViAttr = i;
			 pPresVar->PvNItems++;
		      }
		 }

	       else if (prevRule == RULE_Attr)
		  /* c'est un nom d'attribut auquel on va associer des regles de */
		  /* presentation */
		 {
		    CurAttrNum = i;
		    /* on se souvient de son numero */

		    /* on effectue un certain nombre d'initialisations */
		    NewAttributeDef = True;
		    VCondLess = False;
		    VCondGreater = False;
		    CondEqual = False;
		    CurAttrVal = 0;
		    /* on n'a pas encore rencontre' de valeur */
		    CurComparAttr = 0;
		    /* a priori on ne compare pas  avec un attribut */
		    CurElemHeritAttr = 0;
		    /* a priori ce n'est pas de l'heritage */
		    CurAttrLowerBound = -MAX_INT_ATTR_VAL - 1;
		    /* - infini */
		    CurAttrUpperBound = MAX_INT_ATTR_VAL + 1;
		    /* + infini */
		    CurTextEqual[0] = '\0';
		    /* string vide */
		 }
	       else if (prevRule == RULE_CountFunction)
		 {
		    /* c'est un nom d'attribut apres le mot INIT ou REINIT dans */
		    /* une definition de compteur */
		    /* seuls les attributs numeriques sont accepte's */
		    if (pSSchema->SsAttribute[i - 1].AttrType != AtNumAttr)
		       /* ce n'est pas un attribut numerique, erreur */
		       CompilerMessage (wi, PRS, FATAL, BAD_ATTR_TYPE, inputLine,
				      LineNum);
		    else
		      {
			 pCntr = &pPSchema->PsCounter[pPSchema->PsNCounters - 1];
			 if (AttrInitCounter)
			    /* le compteur courant prendra cet attribut comme */
			    /* valeur initiale */
			    pCntr->CnItem[0].CiInitAttr = i;
			 else
			    pCntr->CnItem[0].CiReinitAttr = i;
		      }
		 }
	       else if (prevRule == RULE_CondAttr)
		 {
		    /* c'est un nom d'attribut apres le mot WITH ou WITHOUT
		       dans une definition de compteur */
		    pCntr = &pPSchema->PsCounter[pPSchema->PsNCounters - 1];
		    pCntr->CnItem[pCntr->CnNItems - 1].CiCondAttr = i;
		 }
	       else if (prevRule == RULE_AttrRelat)
		 {
		    /* c'est un nom d'attribut a la place d'une valeur numerique */
		    /* pour comparer la valeur de l'attribut a un attribut pose  */
		    /* sur un element englobant */
		    if (pSSchema->SsAttribute[CurAttrNum - 1].AttrType != AtNumAttr)
		       CompilerMessage (wi, PRS, FATAL, FORBIDDEN_FOR_THIS_TYPE_OF_ATTR, inputLine, LineNum);
		    /* interdit pour un attribut non numerique */
		    else if (pSSchema->SsAttribute[i - 1].AttrType != AtNumAttr)
		       CompilerMessage (wi, PRS, FATAL, FORBIDDEN_FOR_THIS_TYPE_OF_ATTR, inputLine, LineNum);
		    /* l'attribut n'est pas un attribut numerique */
		    else
		       CurComparAttr = i;
		 }
	       else if (prevRule == RULE_ElemCondition)
		  /* c'est un nom d'attribut dans une condition */
		 {
		    Conditions->CoCondition = PcAttribute;
		    Conditions->CoTypeElAttr = i;
		 }
	       else if (prevRule == RULE_BoxType)
		  /* c'est un nom d'attribut dans une regle BoxType */
		 {
		    if ((CurRule->PrType == PtVertRef ||
			 CurRule->PrType == PtHorizRef) &&
			CurRule->PrPosRule.PoRelation != RlEnclosed)
		       CompilerMessage (wi, PRS, FATAL,
				 ONLY_ENCLOSED_AND_ARE_ALLOWED,
				 inputLine, LineNum);
		    else
		      switch (CurRule->PrType)
			{
			case PtVertRef:
			case PtHorizRef:
			case PtVertPos:
			case PtHorizPos:
			   CurRule->PrPosRule.PoRefKind = RkAttr;
			   CurRule->PrPosRule.PoRefIdent = i;
			   break;
			case PtWidth:
			case PtHeight:
			   if (CurRule->PrDimRule.DrPosition)
			     {
			     CurRule->PrDimRule.DrPosRule.PoRefKind = RkAttr;
			     CurRule->PrDimRule.DrPosRule.PoRefIdent = i;
			     }
			   else
			     {
			     CurRule->PrDimRule.DrRefKind = RkAttr;
			     CurRule->PrDimRule.DrRefIdent = i;
			     }
			   break;
			default: break;
			}
		 }
	       break;

	    case RULE_ConstName:
	       /* ConstName */
	       if (ConstantDef)
		  /* definition de constante */
		  if (Identifier[identnum].SrcIdentDefRule != 0)
		     CompilerMessage (wi, PRS, FATAL, CANT_REDECLARE_NAME, inputLine, LineNum);
		  else
		    {
		       NewConst (wi);
		       Identifier[identnum].SrcIdentDefRule = pPSchema->PsNConstants;
		    }
	       else if (Identifier[identnum].SrcIdentDefRule == 0)
		  /* utilisation d'une constante */
		  /* on n'a pas encore rencontre' ce nom */
		  if (prevRule == RULE_Function)
		     /* peut-etre est-ce un nom d'attribut */
		     /* cherche ce nom parmi les attributs du schema de structure */
		    {
		       CopyName (n, wi, wl);
		       i = 1;
		       while (strcmp (n, pSSchema->SsAttribute[i - 1].AttrName)
			      && i < pSSchema->SsNAttributes)
			  i++;
		       if (strcmp (n, pSSchema->SsAttribute[i - 1].AttrName))
			  CompilerMessage (wi, PRS, FATAL, UNDECLARED_IDENTIFIER, inputLine, LineNum);
		       /* on ne l'a pas trouve, erreur */
		       else
			  /* c'est un nom d'attribut dans une variable */
			  /* change le type de ce nom qui devient un nom d'attribut */
			 {
			    Identifier[identnum].SrcIdentCode = RULE_AttrName;
			    /* r=19 */
			    /* traite ce nom d'attribut */
			    pPresVar = &pPSchema->PsVariable[pPSchema->PsNVariables - 1];
			    NewVarListItem (pPresVar, wi);
			    pPresVar->PvItem[pPresVar->PvNItems - 1].ViType = VarAttrValue;
			    pPresVar->PvItem[pPresVar->PvNItems - 1].ViAttr = i;
			 }
		    }
		  else
		     CompilerMessage (wi, PRS, FATAL, UNDECLARED_IDENTIFIER, inputLine, LineNum);
	       /* nom de constante inconnu */
	       else
		 {
		    if (PresBoxDef && !NewVariableDef)
		       /* dans une regle 'Content : Nom_Constante;' d'une boite de presentation */
		      {
			 pPresBox = &pPSchema->PsPresentBox[CurPresBox - 1];
			 pPresBox->PbContent = ContConst;
			 pPresBox->PbContConstant = Identifier[identnum].SrcIdentDefRule;
		      }
		    if (VariableDef || NewVariableDef)
		       /* dans une definition de variable */
		      {
			 pPresVar = &pPSchema->PsVariable[pPSchema->PsNVariables - 1];
			 NewVarListItem (pPresVar, wi);
			 pPresVar->PvItem[pPresVar->PvNItems - 1].ViType = VarText;
			 pPresVar->PvItem[pPresVar->PvNItems - 1].ViConstant =
			    Identifier[identnum].SrcIdentDefRule;
		      }
		    if (RuleDef)
		       if (CurRule->PrPresMode == PresFunction && CurRule->PrPresFunction == FnContentRef)
			  /* un nom de constante dans une regle Content d'un element reference ou paire */
			 {
			    CurRule->PrNPresBoxes = 1;
			    CurRule->PrPresBox[0] = Identifier[identnum].SrcIdentDefRule;
			 }
		 }
	       break;
	    case RULE_AlphabetName:
	       /* AlphabetName */
	       /* c'est l'alphabet d'une constante, on ne garde que le premier */
	       /* caractere */
	       pPSchema->PsConstant[pPSchema->PsNConstants - 1].PdAlphabet =
		  inputLine[wi - 1];
	       break;
	    case RULE_VarName /* VarName */ :
	       if (VariableDef)
		  /* definition de variable */
		  if (Identifier[identnum].SrcIdentDefRule != 0)
		     CompilerMessage (wi, PRS, FATAL, CANT_REDECLARE_NAME, inputLine, LineNum);
		  else
		    {
		       NewVar (wi);
		       Identifier[identnum].SrcIdentDefRule = pPSchema->PsNVariables;
		    }
	       else if (Identifier[identnum].SrcIdentDefRule == 0)
		  /* utilisation d'une variable */
		  /* ce nom n'a pas ete declare comme identificateur de variable */
		  if (prevRule == RULE_VarConst)
		     /* s'il vient de la regle VarConst, voyons */
		     /* si ce n'est pas un identificateur de type */
		    {
		       ProcessTypeName (prevRule, n, wi, wl);
		       Identifier[identnum].SrcIdentCode = RULE_TypeName;
		       /* changement de type, c'est */
		       /* maintenant un identificateur de type structure' */
		    }
		  else
		     CompilerMessage (wi, PRS, FATAL, UNDECLARED_IDENTIFIER, inputLine, LineNum);
	       else
		 {
		    if (PresBoxDef)
		      {
			 pPresBox = &pPSchema->PsPresentBox[CurPresBox - 1];
			 pPresBox->PbContent = ContVariable;
			 pPresBox->PbContVariable = Identifier[identnum].SrcIdentDefRule;
			 /* cherche tous les compteurs reference's par cette variable */
			 /* et marque que la boite de presentation courante les utilise */
			 pPresVar = &pPSchema->PsVariable[Identifier[identnum].SrcIdentDefRule - 1];
			 for (i = 0; i < pPresVar->PvNItems; i++)
			   {
			      pVarElem = &pPresVar->PvItem[i];
			      if (pVarElem->ViType == VarCounter)
				{
				   pCntr = &pPSchema->PsCounter[pVarElem->ViCounter - 1];
				   if (pCntr->CnNPresBoxes < MAX_PRES_COUNT_USER)
				     {
					pCntr->CnPresBox[pCntr->CnNPresBoxes] = CurPresBox;
					if (pPresVar->PvItem[pPresVar->PvNItems - 1].ViCounterVal == CntMinVal ||
					    pPresVar->PvItem[pPresVar->PvNItems - 1].ViCounterVal == CntMaxVal)
					   pCntr->CnMinMaxPresBox[pCntr->CnNPresBoxes] = True;
					pCntr->CnNPresBoxes++;
				     }
				}
			      else if (pVarElem->ViType == VarPageNumber)
				 PageCounterChangeBox (CurPresBox, pVarElem->ViView);
			   }
		      }
		    else
		       /* on est dans une regle Content d'un element reference ou paire, on */
		       /* refuse: seules les constantes sont acceptees dans cette regle */
		       CompilerMessage (wi, PRS, FATAL, FORBIDDEN_IN_A_REF, inputLine, LineNum);
		 }
	       break;
	    case RULE_BoxName:
	       /* BoxName */
	       if (PresBoxDef && !InRule)
		  /* definition de boite */
		 {
		    if (pPSchema->PsNPresentBoxes >= MAX_PRES_BOX)
		       CompilerMessage (wi, PRS, FATAL, MAX_BOXES_OVERFLOW, inputLine, LineNum);
		    else if (GetTypeNumber (wl, wi, n) != 0)
		       /* ce nom de boite de presentation est deja un nom de type */
		       /* d'element, erreur */
		       CompilerMessage (wi, PRS, FATAL, CANT_USE_TYPE_NAME_FOR_A_BOX,
				      inputLine, LineNum);
		    else if (Identifier[identnum].SrcIdentDefRule == 0)
		       NewBoxName (wl, wi, identnum);
		    else if (pPSchema->PsPresentBox[Identifier[identnum].SrcIdentDefRule - 1].PbName[0] != ' ')
		       CompilerMessage (wi, PRS, FATAL, CANT_REDECLARE_NAME, inputLine, LineNum);
		    /* nom deja rencontre' dans une declaration de vue, de */
		    /* compteur ou dans une instruction Forward */
		    else
		      {
			 pPresBox = &pPSchema->PsPresentBox[Identifier[identnum].SrcIdentDefRule - 1];
			 CopyName (pPresBox->PbName, wi, wl);
			 pPresBox->PbFirstPRule = NextRule;
			 FirstRule = NextRule;
			 CurPresBox = Identifier[identnum].SrcIdentDefRule;
		      }
		 }
	       else
		  /* utilisation d'une boite */
		 {
		    if (Identifier[identnum].SrcIdentDefRule == 0)
		       /* ce nom n'a pas ete declare comme identificateur de boite */
		      {
			 i = 1;
			 if (CurRule->PrType == PtFunction && CurRule->PrPresFunction == FnCopy)
			    /* on est dans une regle Copy */
			   {
			      i = GetTypeNumber (wl, wi, n);
			      if (i == 0)
				 /* ce n'est pas un identificateur de type d'element */
				 /* defini dans le schema de structure */
				{
				   /* il s'agit peut-etre d'un type externe, ou d'une */
				   /* boite de presentation externe; on garde ce nom */
				   /* en attendant le nom de la structure externe */
				   CopyName (CopyType, wi, wl);
				   BeginCopyType = wi;
				}
			   }
			 if ((prevRule == RULE_BoxType ||
			      prevRule == RULE_BoxTypeCopied) && i != 0)
			    /* si on vient de la regle BoxType ou BoxTypeCopied, */
			    /* voyons si ce n'est pas un identificateur de type */
			   {
			      ProcessTypeName (prevRule, n, wi, wl);
			      /* changement de type, c'est maintenant un
				 identificateur de type d'element */
			      Identifier[identnum].SrcIdentCode = RULE_TypeName;
			   }
			 else if (i != 0)
			    CompilerMessage (wi, PRS, FATAL, UNDECLARED_IDENTIFIER, inputLine, LineNum);
		      }
		    else
		       /* c'est une boite declaree */
		       if ((CurRule->PrType == PtVertRef
			    || CurRule->PrType == PtHorizRef)
			   && CurRule->PrPosRule.PoRelation != RlEnclosed)
		           CompilerMessage (wi, PRS, FATAL, ONLY_ENCLOSED_AND_ARE_ALLOWED, inputLine, LineNum);
		       else
		          switch (CurRule->PrType)
			     {
				case PtVertRef:
				case PtHorizRef:
				case PtVertPos:
				case PtHorizPos:
				   CurRule->PrPosRule.PoRefKind = RkPresBox;
				   CurRule->PrPosRule.PoRefIdent = Identifier[identnum].SrcIdentDefRule;
				   break;
				case PtWidth:
				case PtHeight:
				   if (CurRule->PrDimRule.DrPosition)
				     {
					CurRule->PrDimRule.DrPosRule.PoRefKind = RkPresBox;
					CurRule->PrDimRule.DrPosRule.PoRefIdent =
					   Identifier[identnum].SrcIdentDefRule;
				     }
				   else
				     {
					CurRule->PrDimRule.DrRefKind = RkPresBox;
					CurRule->PrDimRule.DrRefIdent =
					   Identifier[identnum].SrcIdentDefRule;
				     }
				   break;
				case PtFunction:
				   if (CurRule->PrNPresBoxes >= MAX_COLUMN_PAGE)
				      CompilerMessage (wi, PRS, FATAL, MAX_COLUMNS_OVERFLOW, inputLine, LineNum);
				   else if (CurRule->PrPresFunction == FnCopy
					    && pSSchema->SsRule[CurType - 1].SrConstruct ==
					    CsReference)
				      /* on est dans une regle FnCopy pour une reference */
				      if (CurRule->PrNPresBoxes != 0)
					 /* deja une boite dans la regle, on refuse */
					 CompilerMessage (wi, PRS, FATAL, UNDECLARED_IDENTIFIER, inputLine, LineNum);
				      else
					{
					   CurRule->PrNPresBoxes = 1;
					   CurRule->PrPresBox[0] = Identifier[identnum].SrcIdentDefRule;
					}
				   else
				     {
					CurRule->PrPresBox[CurRule->PrNPresBoxes] =
					   Identifier[identnum].SrcIdentDefRule;
					CurRule->PrNPresBoxes++;
					if (CurRule->PrPresFunction == FnCreateFirst
					    || CurRule->PrPresFunction == FnCreateLast
					    || CurRule->PrPresFunction == FnCreateBefore
					    || CurRule->PrPresFunction == FnCreateWith
					    || CurRule->PrPresFunction == FnCreateAfter)
					  {
					     /* on est dans une regle de creation */
					     /* teste si on a deja cette regle de creation dans */
					     /* la chaine de regles courante */
					     pPRule = FirstRule;
					     while (pPRule != NULL && pPRule != CurRule)
					       {
						  if (pPRule->PrViewNum == CurRule->PrViewNum)
						     /* la regle concerne la meme vue */
						     if (pPRule->PrType == PtFunction)
							if (pPRule->PrPresFunction == CurRule->PrPresFunction)
							   /* meme operation de creation */
							   if (pPRule->PrPresBox[0] == CurRule->PrPresBox[0])
							      /* meme boite creee */
							      if (SameConditions (pPRule->PrCond, CurRule->PrCond))
								 CompilerMessage (wi, PRS, FATAL, RULE_ALREADY_DEFINED, inputLine, LineNum);
						  /* regle suivante dans la chaine */
						  pPRule = pPRule->PrNextPRule;
					       }
					     /* cherche les compteurs qui controlent la */
					     /* creation de cette boite */
					     pCond = CurRule->PrCond;
					     while (pCond != NULL)
						/* teste toutes les conditions de creation */
					       {
						  if (pCond->CoCondition == PcEven
						      || pCond->CoCondition == PcOdd
						      || pCond->CoCondition == PcOne
						      || pCond->CoCondition == PcInterval)
						     /* c'est une condition sur un compteur, */
						     /* indique dans le compteur qu'il controle la */
						     /* creation de ce type de boite, si ce n'est */
						     /* deja fait. */
						    {
						       pCntr = &pPSchema->PsCounter[pCond->CoCounter - 1];
						       new = True;
						       for (j = 0; j < pCntr->CnNCreatedBoxes; j++)
							  if (pCntr->CnCreatedBox[j] ==
							      Identifier[identnum].SrcIdentDefRule)
							     new = False;
						       if (new)
							 {
							    /* Si la boite est creee sur une condition
							       de min ou de max, on le note */
							    if (pCond->CoValCounter == CntMinVal ||
								pCond->CoValCounter == CntMaxVal)
							       pCntr->CnMinMaxCreatedBox[pCntr->CnNCreatedBoxes] = True;
							    else
							       pCntr->CnMinMaxCreatedBox[pCntr->CnNCreatedBoxes] = False;
							    pCntr->CnCreatedBox[pCntr->CnNCreatedBoxes] =
							       Identifier[identnum].SrcIdentDefRule;
							    pCntr->CnNCreatedBoxes++;
							 }
						    }
						  pCond = pCond->CoNextCondition;
					       }
					  }
				     }
				   break;
				default:
				   break;
			     }
		    /* le cas AttrName a ete deplace' plus haut (cas 19:) */
		 }
	       break;
	    case RULE_FontStyleName:
	       if (CurRule->PrType == PtFillPattern)
		 {
		    /* cherche le nom dans le tableau des trames Thot */
		    CopyName (n, wi, wl);
		    i = 0;
		    ok = False;
		    do
		       if (strcmp (n, Name_patterns[i]) == 0)
			  ok = True;
		       else
			  i++;
		    while (!ok && i < sizeof (Name_patterns) / sizeof (char *));

		    if (!ok)
		       CompilerMessage (wi, PRS, FATAL, MISSING_PATTERN, inputLine, LineNum);
		    else
		       /* on met le rang du pattern dans la regle */
		      {
			 CurRule->PrAttrValue = False;
			 CurRule->PrIntValue = i;
		      }
		 }
	       else if (CurRule->PrType == PtBackground || CurRule->PrType == PtForeground)
		 {
		    i = 0;
		    CopyName (n, wi, wl);
		    while (i < MAX_COLOR && strcmp (Name_colors[i], n))
		       i++;
		    if (i == MAX_COLOR)
		       CompilerMessage (wi, PRS, FATAL, MISSING_COLOR, inputLine, LineNum);
		    else
		      {
			 CurRule->PrAttrValue = False;
			 CurRule->PrIntValue = i;
		      }
		 }
	       else
		  /* FontStyleName */
		  CurRule->PrChrValue = inputLine[wi - 1];
	       break;
	    case RULE_AttrVal:
	       /* AttrVal */
	       /* cherche cette valeur parmi celles de l'attribut */
	       /* precedemment trouve */
	       CopyName (n, wi, wl);
	       pAttr = &pSSchema->SsAttribute[CurAttrNum - 1];
	       if (pAttr->AttrType != AtEnumAttr)
		  CompilerMessage (wi, PRS, FATAL, INVALID_ATTR_VALUE, inputLine, LineNum);
	       /* ce n'est pas un attribut a valeur enumerees */
	       else
		 {
		    i = 1;
		    while (strcmp (n, pAttr->AttrEnumValue[i - 1]) && i < pAttr->AttrNEnumValues)
		       i++;
		    if (strcmp (n, pAttr->AttrEnumValue[i - 1]))
		       CompilerMessage (wi, PRS, FATAL, INVALID_ATTR_VALUE, inputLine, LineNum);
		    /* on ne trouve pas cette valeur, erreur */
		    else
		       CurAttrVal = i;	/* on garde le numero de cette valeur */
		 }
	       break;
	    case RULE_ExternalAttr:
	       /* un nom d'attribut externe dans une regle Transmit */
	       if (TransmittedCounter > 0)
		  /* c'est une regle transmit pour un compteur */
		 {
		    pCntr = &pPSchema->PsCounter[TransmittedCounter - 1];
		    CopyName (pCntr->CnTransmAttr[pCntr->CnNTransmAttrs - 1], wi, wl);
		 }
	       else if (TransmittedElem > 0)
		  /* c'est une regle transmit pour un contenu d'element */
		  CopyName (pPSchema->PsTransmElem[pPSchema->PsNTransmElems - 1].TeTargetAttr, wi, wl);
	       break;
	    default:
	       break;
	 }
}

/*----------------------------------------------------------------------
   ProcessInteger	traite un nombre.                               
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         ProcessInteger (SyntacticCode gCode, indLine wl, indLine wi)

#else  /* __STDC__ */
static void         ProcessInteger (gCode, wl, wi)
SyntacticCode       gCode;
indLine             wl;
indLine             wi;

#endif /* __STDC__ */

{
   int                 n;
   Counter            *pCntr;

   n = AsciiToInt (wi, wl);
   switch (gCode)
	 {
	       /* r= numero de la regle ou apparait le nombre */
	    case RULE_RelLevel:
	       Conditions->CoRelation = n;
	       if (!SignGreaterOrLess)
		  /* le nombre d'ancetres n'est pas precede' du signe > ou < */
		  Conditions->CoAncestorRel = CondEquals;
	       break;
	    case RULE_CountValue:
	       /* CountValue */
	       pCntr = &pPSchema->PsCounter[pPSchema->PsNCounters - 1];
	       pCntr->CnItem[pCntr->CnNItems - 1].CiParamValue = n;
	       break;
	    case RULE_AncestorLevel:
	       /* AncestorLevel */
	       pCntr = &pPSchema->PsCounter[pPSchema->PsNCounters - 1];
	       pCntr->CnItem[pCntr->CnNItems - 1].CiAscendLevel = n * AncestorSign;
	       break;
	    case RULE_DimRatio:
	       /* DimRatio */
	       CurUnit = Percent;
	       LatestNumber = n * 1000;
	       LatestNumberAttr = False;
	       PrevSign = 1;	/* signe positif */
	       CurRule->PrDimRule.DrUnit = UnPercent;
	       break;
	    case RULE_IntPart:
	       /* IntPart */
	       LatestNumber = n * 1000;
	       /* LatestNumber est exprime' en 1/1000 */
	       LatestNumberAttr = False;
	       break;
	    case RULE_DecimPart:
	       /* DecimPart */
	       /* on convertit la partie apres la virgule en milliemes */
	       switch (wl)	/* selon le nombre de chiffres apres la virgule */
		     {
			case 1:
			   n = n * 100;
			   break;
			case 2:
			   n = n * 10;
			   break;
			case 3:
			   n = n * 1;
			   break;
			case 4:
			   n = n / 10;
			   break;
			default:
			   /* trop de decimales */
			   CompilerMessage (wi, PRS, FATAL, MAX_AFTER_DIGITS_OVERFLOW, inputLine, LineNum);
			   break;
		     }
	       /* on ajoute cette partie a la partie entiere deja convertie en milliemes */
	       LatestNumber += n;
	       break;
	    case RULE_PosInteger:
	       /* PosInteger */
	       CurRule->PrInhDelta = n;
	       CurRule->PrInhUnit = UnRelative;
	       CurRule->PrInhAttr = False;
	       break;
	    case RULE_NegInteger:
	       /* NegInteger */
	       CurRule->PrInhDelta = -n;
	       CurRule->PrInhUnit = UnRelative;
	       CurRule->PrInhAttr = False;
	       break;
	    case RULE_maximum:
	       /* maximum */
	       CurRule->PrInhMinOrMax = n;
	       CurRule->PrMinMaxAttr = False;
	       break;
	    case RULE_minimum:
	       /* minimum */
	       CurRule->PrInhMinOrMax = n;
	       CurRule->PrMinMaxAttr = False;
	       break;
	    case RULE_Integer:
	       /* Integer */
	       CurRule->PrIntValue = n;
	       CurRule->PrAttrValue = False;
	       break;
	    case RULE_Size:
	       /* Size */
	       CurRule->PrMinValue = n;
	       CurRule->PrMinAttr = False;
	       CurRule->PrMinUnit = UnRelative;
	       break;
	    case RULE_PosSize:
	       /* PosSize */
	       CurRule->PrInhDelta = n;
	       CurRule->PrInhUnit = UnRelative;
	       CurRule->PrInhAttr = False;
	       break;
	    case RULE_NegSize:
	       /* NegSize */
	       CurRule->PrInhDelta = -n;
	       CurRule->PrInhUnit = UnRelative;
	       CurRule->PrInhAttr = False;
	       break;
	    case RULE_MaxSize:
	       /* MaxSize */
	       CurRule->PrInhMinOrMax = n;
	       CurRule->PrMinMaxAttr = False;
	       break;
	    case RULE_MinSize:
	       /* MinSize */
	       CurRule->PrInhMinOrMax = n;
	       CurRule->PrMinMaxAttr = False;
	       break;
	    case RULE_MinVal:
	       /* MinVal */
	       if (pSSchema->SsAttribute[CurAttrNum - 1].AttrType != AtNumAttr
		   || n >= MAX_INT_ATTR_VAL)
		  CompilerMessage (wi, PRS, FATAL, INVALID_ATTR_VALUE, inputLine, LineNum);
	       /* ce n'est pas un attribut a valeur numerique */
	       else
		 {
		    CurAttrLowerBound = n * AttrValSign + 1;
		    AttrValSign = 1;
		 }
	       break;
	       /* a priori, la prochaine valeur */
	       /* d'attribut numerique sera positive */
	    case RULE_MaxVal:
	       /* MaxVal */
	       if (pSSchema->SsAttribute[CurAttrNum - 1].AttrType != AtNumAttr
		   || n >= MAX_INT_ATTR_VAL)
		  CompilerMessage (wi, PRS, FATAL, INVALID_ATTR_VALUE, inputLine, LineNum);
	       /* ce n'est pas un attribut a valeur numerique */
	       else
		 {
		    CurAttrUpperBound = n * AttrValSign - 1;
		    AttrValSign = 1;
		 }
	       break;
	       /* a priori, la prochaine valeur */
	       /* d'attribut numerique sera positive */
	    case RULE_MinInterval:
	       /* MinInterval */
	       if (pSSchema->SsAttribute[CurAttrNum - 1].AttrType != AtNumAttr
		   || n >= MAX_INT_ATTR_VAL)
		  CompilerMessage (wi, PRS, FATAL, INVALID_ATTR_VALUE, inputLine, LineNum);
	       /* ce n'est pas un attribut a valeur numerique */
	       else
		 {
		    CurAttrLowerBound = n * AttrValSign;
		    AttrValSign = 1;
		 }
	       break;
	       /* a priori, la prochaine valeur */
	       /* d'attribut numerique sera positive */
	    case RULE_MaxInterval:
	       /* MaxInterval */
	       if (pSSchema->SsAttribute[CurAttrNum - 1].AttrType != AtNumAttr
		   || n >= MAX_INT_ATTR_VAL)
		  CompilerMessage (wi, PRS, FATAL, INVALID_ATTR_VALUE, inputLine, LineNum);
	       /* ce n'est pas un attribut a valeur numerique */
	       else
		 {
		    if (CurAttrLowerBound > n * AttrValSign)
		       CompilerMessage (wi, PRS, FATAL, INVALID_LOWER_BOUND, inputLine, LineNum);
		    else
		       CurAttrUpperBound = n * AttrValSign;
		    AttrValSign = 1;
		 }
	       break;
	       /* a priori, la prochaine valeur */
	       /* d'attribut numerique sera positive */
	    case RULE_ValEqual:
	       /* ValEqual */
	       if (pSSchema->SsAttribute[CurAttrNum - 1].AttrType != AtNumAttr
		   || n >= MAX_INT_ATTR_VAL)
		  CompilerMessage (wi, PRS, FATAL, INVALID_ATTR_VALUE, inputLine, LineNum);
	       /* ce n'est pas un attribut a valeur numerique */
	       else
		 {
		    CurAttrUpperBound = CurAttrLowerBound = n * AttrValSign;
		    AttrValSign = 1;
		 }
	       break;
	       /* a priori, la prochaine valeur */
	       /* d'attribut numerique sera positive */
	    case RULE_MinCounterVal:
	       if (n >= MAX_COUNTER_VAL)
		  CompilerMessage (wi, PRS, FATAL, BAD_COUNTER_VALUE, inputLine, LineNum);
	       else
		 {
		    Conditions->CoMinCounter = n * CurCondCntSign + 1;
		    /* a priori, la prochaine borne sera positive */
		    CurCondCntSign = 1;
		 }
	       break;
	    case RULE_MaxCounterVal:
	       if (n >= MAX_COUNTER_VAL)
		  CompilerMessage (wi, PRS, FATAL, BAD_COUNTER_VALUE, inputLine, LineNum);
	       else
		 {
		    Conditions->CoMaxCounter = n * CurCondCntSign - 1;
		    /* a priori, la prochaine borne sera positive */
		    CurCondCntSign = 1;
		 }
	       break;
	    case RULE_MinCounterInter:
	       if (n >= MAX_COUNTER_VAL)
		  CompilerMessage (wi, PRS, FATAL, BAD_COUNTER_VALUE, inputLine, LineNum);
	       else
		 {
		    Conditions->CoMinCounter = n * CurCondCntSign;
		    /* a priori, la prochaine borne sera positive */
		    CurCondCntSign = 1;
		 }
	       break;
	    case RULE_MaxCounterInter:
	       if (n >= MAX_COUNTER_VAL)
		  CompilerMessage (wi, PRS, FATAL, BAD_COUNTER_VALUE, inputLine, LineNum);
	       else if (Conditions->CoMinCounter > n * CurCondCntSign)
		  CompilerMessage (wi, PRS, FATAL, INVALID_LOWER_BOUND, inputLine, LineNum);
	       else
		 {
		    Conditions->CoMaxCounter = n * CurCondCntSign;
		    /* a priori, la prochaine borne sera positive */
		    CurCondCntSign = 1;
		 }
	       break;
	    case RULE_CounterValEqual:
	       if (n >= MAX_COUNTER_VAL)
		  CompilerMessage (wi, PRS, FATAL, BAD_COUNTER_VALUE, inputLine, LineNum);
	       else
		 {
		    Conditions->CoMaxCounter = Conditions->CoMinCounter = n * CurCondCntSign;
		    /* a priori, la prochaine borne sera positive */
		    CurCondCntSign = 1;
		 }
	       break;
	    case RULE_ImageType:
	       /* c'est le type d'une image */
	       pPSchema->PsConstant[pPSchema->PsNConstants - 1].PdAlphabet = n;
	       break;
	    default:
	       break;
	 }
}

/*----------------------------------------------------------------------
   ProcessString       traitee une chaine de caracteres.           
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         ProcessString (SyntacticCode gCode, indLine wl, indLine wi)

#else  /* __STDC__ */
static void         ProcessString (gCode, wl, wi)
SyntacticCode       gCode;
indLine             wl;
indLine             wi;

#endif /* __STDC__ */

{
   int                 i;
   PresConstant       *pPresConst;

   if (gCode == RULE_ConstValue || gCode == RULE_FileName)
      /* c'est une valeur de constante ou le nom de fichier d'une image de fond */
      if (wl > MAX_PRES_CONST_LEN)
	 CompilerMessage (wi, PRS, FATAL, MAX_CHAR_STRING_SIZE_OVERFLOW,
			  inputLine, LineNum);
      else
	{
	   pPresConst = &pPSchema->PsConstant[pPSchema->PsNConstants - 1];
	   for (i = 0; i < wl - 1; i++)
	      pPresConst->PdString[i] = inputLine[wi + i - 1];
	   pPresConst->PdString[wl - 1] = '\0';
	}
   else if (gCode == RULE_TextEqual)
      /* TextEqual c'est une valeur d'attribut */
      if (pSSchema->SsAttribute[CurAttrNum - 1].AttrType != AtTextAttr)
	 /* ce n'est pas un attribut a valeur textuelle */
	 CompilerMessage (wi, PRS, FATAL, INVALID_ATTR_VALUE, inputLine, LineNum);
      else if (wl > MAX_NAME_LENGTH)
	 CompilerMessage (wi, PRS, FATAL, MAX_CHAR_STRING_SIZE_OVERFLOW,
			  inputLine, LineNum);
      else
	{
	   for (i = 0; i < wl - 1; i++)
	      CurTextEqual[i] = inputLine[wi + i - 1];
	   CurTextEqual[wl - 1] = '\0';
	}
}


/*----------------------------------------------------------------------
   ProcessToken    traite le token commencant a la position wi dans
   la ligne courante, de longueur wl et de code grammatical c.     
   Si c'est un identificateur, nb contient son rang dans la table  
   des identificateurs. r est le numero de la regle dans laquelle  
   apparait ce mot, pr est le numero de la regle precedente, celle 
   qui a appele la regle r.                                        
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         ProcessToken (indLine wi, indLine wl, SyntacticCode c, SyntacticCode r, int identnum, SyntacticCode pr)

#else  /* __STDC__ */
static void         ProcessToken (wi, wl, c, r, identnum, pr)
indLine             wi;
indLine             wl;
SyntacticCode       c;
SyntacticCode       r;
int                 identnum;
SyntacticCode       pr;

#endif /* __STDC__ */

{
   if (c < 1000)
      /* symbole intermediaire de la grammaire, erreur */
      CompilerMessage (wi, PRS, FATAL, INTERMEDIATE_SYMBOL, inputLine, LineNum);
   else if (c < 1100)
      /* mot-cle court */
      ProcessShortKeyWord (c, wi, r);
   else if (c < 2000)
      /* mot-cle long */
      ProcessLongKeyWord (c, r, wi);
   else
      /* type de base */
      switch (c)
	    {
	       case 3001:
		  /* un nom */
		  ProcessName (r, identnum, pr, wl, wi);
		  break;
	       case 3002:
		  /* un nombre */
		  ProcessInteger (r, wl, wi);
		  break;
	       case 3003:
		  /* une chaine de caracteres */
		  ProcessString (r, wl, wi);
		  break;
	    }
}

/*----------------------------------------------------------------------
   RuleBefore                                                      
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static boolean      RuleBefore (PtrPRule pPRule1, PtrPRule pPRule2)

#else  /* __STDC__ */
static boolean      RuleBefore (pPRule1, pPRule2)
PtrPRule            pPRule1;
PtrPRule            pPRule2;

#endif /* __STDC__ */

{
   boolean             ret;

   /* on classe d'abord dans l'ordre des types de regle */
   if (pPRule1->PrType != pPRule2->PrType)
      ret = pPRule1->PrType < pPRule2->PrType;
   else
      /* meme type */
   if (pPRule1->PrType == PtFunction)
     {
	/* c'est une fonction de presentation, on classe */
	/* d'abord dans l'ordre des codes de fonction. */
	if (pPRule1->PrPresFunction != pPRule2->PrPresFunction)
	  {
#ifdef __COLPAGE__
	     if (((pPRule1->PrPresFunction == FnPage) || (pPRule1->PrPresFunction == FnColumn) ||
		  (pPRule1->PrPresFunction == FnSubColumn)) &&
		 ((pPRule2->PrPresFunction == FnPage) || (pPRule2->PrPresFunction == FnColumn) ||
		  (pPRule2->PrPresFunction == FnSubColumn)))
		/* fonctions equivalentes (Page, Column, Included Column), on */
		/* classe dans l'ordre des vues */
		ret = pPRule1->PrViewNum < pPRule2->PrViewNum;
	     else
#endif /* __COLPAGE__ */
		ret = pPRule1->PrPresFunction < pPRule2->PrPresFunction;
	  }
	else
	  {
	     /* meme fonction, on classe dans l'ordre des vues */
	     ret = pPRule1->PrViewNum < pPRule2->PrViewNum;
	  }
     }
   else
     {
	/* ce n'est pas une fonction de presentation, on classe */
	/* dans l'ordre des vues */
	ret = pPRule1->PrViewNum < pPRule2->PrViewNum;
     }
   return ret;
}

/*----------------------------------------------------------------------
   SortPresRules                                                   
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static void         SortPresRules (PtrPRule * firstPRule)

#else  /* __STDC__ */
static void         SortPresRules (firstPRule)
PtrPRule           *firstPRule;

#endif /* __STDC__ */

{
   PtrPRule            oldPRule, curOldPRule, newPRule, curNewPRule, newAnchor;
   boolean             done, stop;

   if (*firstPRule != NULL)
     {
	oldPRule = *firstPRule;
	newAnchor = NULL;
	/* parcourt l'ancienne chaine des regles pointee par firstPRule */
	while (oldPRule != NULL)
	  {
	     curOldPRule = oldPRule;
	     /* regle courante de l'ancienne chaine */
	     oldPRule = oldPRule->PrNextPRule;
	     if (newAnchor == NULL)
	       {
		  newAnchor = curOldPRule;
		  curOldPRule->PrNextPRule = NULL;
	       }
	     else
	       {
		  newPRule = NULL;
		  curNewPRule = newAnchor;
		  done = False;
		  stop = False;
		  /* parcourt la nouvelle chaine */
		  while (!stop)
		     if (RuleBefore (curOldPRule, curNewPRule))
		       {
			  if (newPRule == NULL)
			     /* insere curOldPRule en tete */
			    {
			       curOldPRule->PrNextPRule = newAnchor;
			       newAnchor = curOldPRule;
			    }
			  else
			     /* insere curOldPRule apres newPRule */
			    {
			       curOldPRule->PrNextPRule = newPRule->PrNextPRule;
			       newPRule->PrNextPRule = curOldPRule;
			    }
			  done = True;
			  stop = True;
		       }
		     else
		       {
			  newPRule = curNewPRule;
			  curNewPRule = curNewPRule->PrNextPRule;
			  if (curNewPRule == NULL)
			     stop = True;
		       }
		  if (!done)
		     /* ajoute curOldPRule a la fin de la nouvelle chaine, apres newPRule */
		    {
		       if (newPRule != NULL)
			  newPRule->PrNextPRule = curOldPRule;
		       else
			  newAnchor = oldPRule;
		       curOldPRule->PrNextPRule = NULL;
		    }
	       }
	  }
	*firstPRule = newAnchor;
     }
}

/*----------------------------------------------------------------------
   SortAllPRules  met les regles de presentation dans le bon ordre 
   pour l'application des regles lors de la production des images  
   abstraites.                                                     
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                SortAllPRules ()

#else  /* __STDC__ */
void                SortAllPRules ()
#endif				/* __STDC__ */

{
   int                 j, l;
   int                 k;
   AttributePres      *pPRuleA;

   /* ordonne les regles par defaut */
   SortPresRules (&pPSchema->PsFirstDefaultPRule);

   /* ordonne les regles des boites de presentation */
   for (j = 0; j < pPSchema->PsNPresentBoxes; j++)
      SortPresRules (&pPSchema->PsPresentBox[j].PbFirstPRule);

   /* ordonne les regles des valeurs d'attribut */
   for (j = 0; j < pSSchema->SsNAttributes; j++)
     {
	/* pour chaque paquet de regles */
	pPRuleA = pPSchema->PsAttrPRule[j];
	for (l = pPSchema->PsNAttrPRule[j]; l-- > 0; pPRuleA = pPRuleA->ApNextAttrPres)
	  {
	     /* selon le type de l'attribut */
	     switch (pSSchema->SsAttribute[j].AttrType)
		   {
		      case AtNumAttr:
			 for (k = 0; k < pPRuleA->ApNCases; k++)
			    SortPresRules (&pPRuleA->ApCase[k].CaFirstPRule);
			 break;
		      case AtTextAttr:
			 SortPresRules (&pPRuleA->ApTextFirstPRule);
			 break;
		      case AtReferenceAttr:
			 SortPresRules (&pPRuleA->ApRefFirstPRule);
			 break;
		      case AtEnumAttr:
			 for (k = 0; k <= pSSchema->SsAttribute[j].AttrNEnumValues; k++)
			    SortPresRules (&pPRuleA->ApEnumFirstPRule[k]);
			 break;
		      default:
			 break;
		   }
	  }
     }

   /* ordonne les regles des elements structures */
   for (j = 0; j < MAX_RULES_SSCHEMA; j++)
      SortPresRules (&pPSchema->PsElemPRule[j]);
}

/*----------------------------------------------------------------------
   PageCounter    determine si le compteur de numero counter est   
   un compteur de page. Retourne Vrai si c'est un compteur de	
   pages, Faux sinon.                                              
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static boolean      PageCounter (int counter)

#else  /* __STDC__ */
static boolean      PageCounter (counter)
int                 counter;

#endif /* __STDC__ */

{
   Counter            *pCntr;
   boolean             ret;

   pCntr = &pPSchema->PsCounter[counter - 1];
   ret = ((pCntr->CnItem[0].CiCntrOp == CntrRank
	   && pCntr->CnItem[0].CiElemType == PageBreak + 1)
	  || (pCntr->CnItem[0].CiCntrOp == CntrSet
	      && pCntr->CnItem[1].CiElemType == PageBreak + 1));
   return ret;
}

/*----------------------------------------------------------------------
   SearchPRule  cherche pour la vue view une regle de type ruleType
   dans la chaine de regles dont l'ancre est firstRule. Retourne   
   un pointeur sur cette regle si elle existe ou insere une regle  
   de ce type dans la chaine et retourne un pointeur sur la        
   regle creee.                                                    
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static PtrPRule     SearchPRule (PtrPRule * firstRule, PRuleType ruleType, int view)
#else  /* __STDC__ */
static PtrPRule     SearchPRule (firstRule, ruleType, view)
PtrPRule           *firstRule;
PRuleType           ruleType;
int                 view;
#endif /* __STDC__ */
{
   PtrPRule            pR, pPRule;
   boolean             stop, cree;

   pR = *firstRule;
   pPRule = NULL;
   stop = False;
   do
     {
	if (pR == NULL)
	   stop = True;
	else if (pR->PrType >= ruleType)
	   stop = True;
	if (!stop)
	  {
	     pPRule = pR;
	     pR = pR->PrNextPRule;
	  }
     }
   while (!stop);
   stop = False;
   do
     {
	if (pR == NULL)
	   stop = True;
	else if (pR->PrType != ruleType || pR->PrViewNum >= view)
	   stop = True;
	if (!stop)
	  {
	     pPRule = pR;
	     pR = pR->PrNextPRule;
	  }
     }
   while (!stop);
   if (pR == NULL)
      cree = True;
   else if (pR->PrType > ruleType || pR->PrViewNum > view)
      cree = True;
   else
      cree = False;
   if (cree)
      /* il n'y a pas de regle de ce type, on en cree une */
     {
        GetPresentRule (&pR);
	if (pR == NULL)
	   TtaDisplaySimpleMessage (FATAL, PRS, NO_MORE_MEM_LEFT);
	pR->PrType = ruleType;
	/* on insere la regle cree */
	if (pPRule == NULL)
	  {
	     pR->PrNextPRule = *firstRule;
	     *firstRule = pR;
	  }
	else
	  {
	     pR->PrNextPRule = pPRule->PrNextPRule;
	     pPRule->PrNextPRule = pR;
	  }
	pR->PrCond = NULL;
	pR->PrViewNum = view;
	pR->PrSpecifAttr = 0;
	pR->PrSpecifAttrSSchema = NULL;
     }
   return pR;
}

/*----------------------------------------------------------------------
   CheckPageBoxes    verifie que toutes les boites de presentation 
   declarees pour les pages sont bien utilisees et adapte les      
   regles des boites pages.                                        
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         CheckPageBoxes ()
#else  /* __STDC__ */
static void         CheckPageBoxes ()
#endif /* __STDC__ */
{
   PtrPRule            pR, pHeadR, pPRule, pRule;
   int                 b, hfB, el, view, footHeight, headHeight, h, i,
                       counter;
   boolean             stop, stop1, exist;
   PresentationBox    *pPresBox;
   PresVariable       *pPresVar;
   PtrCondition        pCond;
   int                 viewOfBox[MAX_PRES_BOX];

#ifdef __COLPAGE__
   int                 bc;
   PtrPRule            pRC;

#else  /* __COLPAGE__ */
   PtrPRule            pRLarg;

#endif /* __COLPAGE__ */

   /* a priori les boites de presentation ne sont pas des boites pages */
   for (b = 0; b < pPSchema->PsNPresentBoxes; b++)
     {
	viewOfBox[b] = 0;
	pPresBox = &pPSchema->PsPresentBox[b];
	pPresBox->PbPageBox = False;
	pPresBox->PbPageFooter = False;
	pPresBox->PbPageHeader = False;
     }
   /* a priori les compteurs ne sont pas utilise's en bas de page */
   for (b = 0; b < pPSchema->PsNCounters; b++)
      pPSchema->PsCounter[b].CnPageFooter = False;
   /* recherche toutes les regles PAGE dans le schema de presentation */
   /* et marque les boites pages utilisees par ces regles. */
   for (el = 0; el < pSSchema->SsNRules; el++)
      /* cherche dans les regles de */
      /* presentation de chaque type d'element */
     {
	pR = pPSchema->PsElemPRule[el];
	/* 1ere regle de pres. du type */
	stop = False;
	do
	  {
	     if (pR == NULL)
		stop = True;
	     else if (pR->PrType > PtFunction)
		stop = True;
	     if (!stop)
	       {
		  if (pR->PrType == PtFunction
		      && pR->PrPresFunction == FnPage)
		     /* c'est une regle Page */
		     /* marque la boite page */
		    {
		       b = pR->PrPresBox[0];
		       /* b: numero de la boite page */
		       /* note la vue concernee par cette boite page, si */
		       /* elle n'est pas deja notee */
		       if (viewOfBox[b - 1] == 0)
			  viewOfBox[b - 1] = pR->PrViewNum;
		       pPSchema->PsPresentBox[b - 1].PbPageBox = True;
		       pPSchema->PsPresentBox[b - 1].PbPageCounter = 0;
		       /* verifie que la boite page a une largeur et une hauteur */
		       /* absolues */
		       /* cherche la regle de hauteur d'abord */
		       pRule = pPSchema->PsPresentBox[b - 1].PbFirstPRule;
		       stop1 = False;
		       exist = False;
		       do
			 {
			    if (pRule == NULL)
			       stop1 = True;
			    else if (pRule->PrType > PtHeight)
			       stop1 = True;
			    else if (pRule->PrType == PtHeight)
			      {
				 stop1 = True;
				 exist = True;
			      }
			    if (!stop1)
			       pRule = pRule->PrNextPRule;
			 }
		       while (!stop1);
		       if (!exist)
			  /* il n'y a pas de regle de hauteur, erreur */
			  TtaDisplayMessage (FATAL, TtaGetMessage (PRS, BAD_HEIGHT_RULE), pPSchema->PsPresentBox[b - 1].PbName);
		       else if (!pRule->PrDimRule.DrAbsolute
				|| pRule->PrDimRule.DrUnit == UnRelative
				|| pRule->PrDimRule.DrPosition)
			  /* ce n'est pas une hauteur absolue fixe, erreur */
			  TtaDisplayMessage (FATAL, TtaGetMessage (PRS, BAD_HEIGHT_RULE), pPSchema->PsPresentBox[b - 1].PbName);
		       /* cherche la regle de largeur */
		       pRule = pPSchema->PsPresentBox[b - 1].PbFirstPRule;
		       stop1 = False;
		       exist = False;
		       do
			 {
			    if (pRule == NULL)
			       stop1 = True;
			    else if (pRule->PrType > PtWidth)
			       stop1 = True;
			    else if (pRule->PrType == PtWidth)
			      {
				 stop1 = True;
				 exist = True;
			      }
			    if (!stop1)
			       pRule = pRule->PrNextPRule;
			 }
		       while (!stop1);
		       if (!exist)
			  /* il n'y a pas de regle de largeur, erreur */
			  TtaDisplayMessage (FATAL, TtaGetMessage (PRS, BAD_WIDTH_RULE), pPSchema->PsPresentBox[b - 1].PbName);
		       else if (!pRule->PrDimRule.DrAbsolute
				|| pRule->PrDimRule.DrUnit == UnRelative
				|| pRule->PrDimRule.DrPosition)
			  /* ce n'est pas une largeur absolue fixe, erreur */
			  TtaDisplayMessage (FATAL, TtaGetMessage (PRS, BAD_WIDTH_RULE), pPSchema->PsPresentBox[b - 1].PbName);
		       /* cherche la regle de largeur de la boite qui porte la */
		       /* regle Page, ou cree une regle s'il n'y a pas de regle de */
		       /* largeur */
#ifndef __COLPAGE__
		       /* supprime pour les colonnes */
		       pRLarg = SearchPRule (&pPSchema->PsElemPRule[el], PtWidth,
					     pR->PrViewNum);
		       /* modifie la regle de largeur: meme regle que la boite page */
		       pRLarg->PrPresMode = PresImmediate;
		       pRLarg->PrDimRule = pRule->PrDimRule;
#endif /* __COLPAGE__ */
		       /* cherche la regle de position verticale */
		       /* de la boite page */
		       pRule = pPSchema->PsPresentBox[b - 1].PbFirstPRule;
		       stop1 = False;
		       exist = False;
		       do
			 {
			    if (pRule == NULL)
			       stop1 = True;
			    else if (pRule->PrType > PtVertPos)
			       stop1 = True;
			    else if (pRule->PrType == PtVertPos)
			      {
				 stop1 = True;
				 exist = True;
			      }
			    if (!stop1)
			       pRule = pRule->PrNextPRule;
			 }
		       while (!stop1);
		       if (!exist)
			  /* il n'y a pas de regle de position verticale, erreur */
			  TtaDisplayMessage (FATAL, TtaGetMessage (PRS, BAD_VERTPOS_RULE), pPSchema->PsPresentBox[b - 1].PbName);
		       else
			 {
#ifdef __COLPAGE__
			    if (pRule->PrPosRule.PoPosDef != Top
			    /* position du cote superieur */
				|| pRule->PrPosRule.PoPosRef != Bottom
			    /* par rapport au cote inf. */
				|| pRule->PrPosRule.PoRelation != RlPrevious
			    /* du precedent */
				|| (pRule->PrPosRule.PoDistUnit == UnRelative && pRule->PrPosRule.PoDistance != 0))
#else  /* __COLPAGE__ */
			    if (pRule->PrPosRule.PoPosDef != Top
			    /* position du cote superieur */
				|| pRule->PrPosRule.PoPosRef != Top
			    /* par rapport au cote sup. */
				|| pRule->PrPosRule.PoRelation != RlEnclosing
			    /* de l'englobant */
				|| (pRule->PrPosRule.PoDistUnit == UnRelative && pRule->PrPosRule.PoDistance != 0))
#endif /* __COLPAGE__ */
			       /* et distance absolue */
			       /* ce n'est pas un positionnement correct, erreur */
			       TtaDisplayMessage (FATAL, TtaGetMessage (PRS, BAD_VERTPOS_RULE), pPSchema->PsPresentBox[b - 1].PbName);
			 }
		       /* cherche la regle de position horizontale */
		       pRule = pPSchema->PsPresentBox[b - 1].PbFirstPRule;
		       stop1 = False;
		       exist = False;
		       do
			 {
			    if (pRule == NULL)
			       stop1 = True;
			    else if (pRule->PrType > PtHorizPos)
			       stop1 = True;
			    else if (pRule->PrType == PtHorizPos)
			      {
				 stop1 = True;
				 exist = True;
			      }
			    if (!stop1)
			       pRule = pRule->PrNextPRule;
			 }
		       while (!stop1);
		       if (!exist)
			  /* il n'y a pas de regle de position horiz., erreur */
			  TtaDisplayMessage (FATAL, TtaGetMessage (PRS, BAD_HORIZPOS_RULE), pPSchema->PsPresentBox[b - 1].PbName);
		       else
			 {
			    if (pRule->PrPosRule.PoPosDef != Left
			    /* position du cote gauche */
				|| pRule->PrPosRule.PoPosRef != Left
			    /* par rapport au cote gauche */
				|| pRule->PrPosRule.PoRelation != RlEnclosing
			    /* de l'englobant */
				|| (pRule->PrPosRule.PoDistUnit == UnRelative && pRule->PrPosRule.PoDistance != 0))
			       /* et distance absolue */
			       /* ce n'est pas un positionnement correct, erreur */
			       TtaDisplayMessage (FATAL, TtaGetMessage (PRS, BAD_HORIZPOS_RULE), pPSchema->PsPresentBox[b - 1].PbName);
			 }
		    }
		  pR = pR->PrNextPRule;
		  /* passe a la regle suivante */
	       }
	  }
	while (!stop);
     }
   /* Toutes les boites pages sont maintenant marquees */
   /* Cherche les boites creees par les boites pages (ce sont les */
   /* boites de haut et de bas de page) et marque ces boites. Calcule la */
   /* hauteur minimum des bas de page. */
   for (b = 0; b < pPSchema->PsNPresentBoxes; b++)
      /* examine chaque boite */
      if (pPSchema->PsPresentBox[b].PbPageBox)
	 /* c'est une boite page */
	{
	   footHeight = 0;
	   headHeight = 0;
	   /* cherche les regles de creation de la boite page */
	   pR = pPSchema->PsPresentBox[b].PbFirstPRule;
	   /* 1ere regle de presentation */
	   /* parcourt les premieres regles de la boite page */
	   stop = False;
	   do
	     {
		if (pR == NULL)
		   stop = True;
		else if (pR->PrType > PtFunction)
		   stop = True;
		if (!stop)
		  {
		     if (pR->PrType == PtFunction
			 && (pR->PrPresFunction == FnCreateBefore
			     || pR->PrPresFunction == FnCreateWith
			     || pR->PrPresFunction == FnCreateFirst
			     || pR->PrPresFunction == FnCreateLast
			     || pR->PrPresFunction == FnCreateAfter))
			/* c'est une regle de creation */
			if (!(pR->PrPresFunction == FnCreateBefore || pR->PrPresFunction == FnCreateAfter))
			   /* ce n'est pas une regle de creation autorisee, erreur */
			   TtaDisplayMessage (FATAL, TtaGetMessage (PRS, FORBIDDEN_CREA_RULE), pPSchema->PsPresentBox[b].PbName);
			else
			   /* la regle pR est une regle CreateAfter ou */
			   /* a CreateBefore */
			  {
			     hfB = pR->PrPresBox[0] - 1;
			     /* numero de la boite de haut ou de bas de page */
			     /* la boite de haut ou de bas de page appartient */
			     /* la meme vue que la boite page qui la cree */
			     viewOfBox[hfB] = viewOfBox[b];
#ifdef __COLPAGE__
			     /* type de boite : haut (CreateBefore) ou bas */
			     /*(CreateAfter) */
			     if (pR->PrPresFunction == FnCreateBefore)
				pPSchema->PsPresentBox[hfB].PbPageHeader = True;
			     else
				pPSchema->PsPresentBox[hfB].PbPageFooter = True;
#endif /* __COLPAGE__ */
			     /* cherche la regle de positionnement vertical de */
			     /* la boite creee par la boite page */
			     pPRule = pPSchema->PsPresentBox[hfB].PbFirstPRule;
			     stop1 = False;
			     exist = False;
			     do
			       {
				  if (pPRule == NULL)
				     stop1 = True;
				  else if (pPRule->PrType > PtVertPos)
				     stop1 = True;
				  else if (pPRule->PrType == PtVertPos)
				    {
				       stop1 = True;
				       exist = True;
				    }
				  if (!stop1)
				     pPRule = pPRule->PrNextPRule;
			       }
			     while (!stop1);
			     if (!exist)
				/* pas de regle de positionnement vertical, erreur */
				TtaDisplayMessage (FATAL, TtaGetMessage (PRS, MISSING_VERTIC_POS_IN_THE_PAGE), pPSchema->PsPresentBox[hfB].PbName);
			     else
			       {
#ifdef __COLPAGE__
				  if (!(pPRule->PrPosRule.PoPosDef == Top
				     || pPRule->PrPosRule.PoPosDef == Bottom
					|| pPRule->PrPosRule.PoPosDef == HorizMiddle))
				     /* on ne teste plus PoPosDef,PoNotRel et PoRefKind */
				     /* suppression test elt ref. != marque de page */
#else  /* __COLPAGE__ */
				  if (!(pPRule->PrPosRule.PoPosDef == Top
				     || pPRule->PrPosRule.PoPosDef == Bottom
					|| pPRule->PrPosRule.PoPosDef == HorizMiddle)
				      || pPRule->PrPosRule.PoNotRel
				      || pPRule->PrPosRule.PoRefKind != RkElType
				      || pPRule->PrPosRule.PoRefIdent != PageBreak + 1)
#endif /* __COLPAGE__ */
				     TtaDisplayMessage (FATAL, TtaGetMessage (PRS, INVALID_VERTIC_POS_IN_PAGE), pPSchema->PsPresentBox[hfB].PbName);
				  if (pPRule->PrPosRule.PoDistance != 0)
				     if (pPRule->PrPosRule.PoDistUnit == UnRelative)
					TtaDisplayMessage (FATAL, TtaGetMessage (PRS, VERTIC_DIST_ISNT_CONSTANT), pPSchema->PsPresentBox[hfB].PbName);
#ifdef __COLPAGE__
				  if (pPRule->PrPosRule.PoRelation != RlPrevious)
				     TtaDisplayMessage (FATAL, TtaGetMessage (PRS, INVALID_VERTIC_POS_IN_PAGE), pPSchema->PsPresentBox[hfB].PbName);
				  else
				     /* la boite creee est positionnee par rapport */
				     /* a la boite precedente : seul cas autorise */
				    {
#else  /* __COLPAGE__ */
				  if (pPRule->PrPosRule.PoRelation == RlPrevious)
				     /* la boite creee est positionnee par rapport */
				     /* au haut de la page */
				    {
				       pPSchema->PsPresentBox[hfB].PbPageHeader = True;
				       /* cherche la regle de hauteur de la boite de */
				       /* haut de page */
				       pHeadR = pPSchema->PsPresentBox[hfB].PbFirstPRule;
				       stop1 = False;
				       exist = False;
				       do
					 {
					    if (pHeadR == NULL)
					       stop1 = True;
					    else if (pHeadR->PrType > PtHeight)
					       stop1 = True;
					    else if (pHeadR->PrType == PtHeight)
					      {
						 stop1 = True;
						 exist = True;
					      }
					    if (!stop1)
					       pHeadR = pHeadR->PrNextPRule;
					 }
				       while (!stop1);
				       /* si pas de regle de hauteur, pas d'erreur */
				       if (exist)
					  if (pHeadR->PrDimRule.DrPosition)
					     /* c'est une hauteur elastique, erreur */
					     TtaDisplayMessage (FATAL, TtaGetMessage (PRS, BAD_HEIGHT_RULE), pPSchema->PsPresentBox[hfB].PbName);
					  else if (!pHeadR->PrDimRule.DrAbsolute)
					     /* c'est une hauteur relative, on n'accepte */
					     /* que la hauteur du contenu */
					    {
					       if (pHeadR->PrDimRule.DrRelation != RlEnclosed)
						  TtaDisplayMessage (FATAL, TtaGetMessage (PRS, BAD_HEIGHT_RULE), pPSchema->PsPresentBox[hfB].PbName);
					    }
					  else
					     /* regle de hauteur absolue */
					  if (pHeadR->PrDimRule.DrUnit == UnRelative)
					     /* la hauteur n'est pas en unites fixes */
					     TtaDisplayMessage (FATAL, TtaGetMessage (PRS, BAD_HEIGHT_RULE), pPSchema->PsPresentBox[hfB].PbName);
					  else
					     /* calcule la distance entre le bas de la */
					     /* boite de haut de page et le haut de la page */
					     /* calcule la distance entre le bas de la */
					     /* boite de precedente et le bas de la */
					     /* boite creee (haut ou bas) */
					     /* ce code n'a plus vraiment de sens car */
					     /* le plus souvent, la hauteur des hauts et */
					     /* et bas de page est donnee par leur contenu */
					     /* (plusieurs boites filles) */
					     /* donc, c'est le mediateur qui peut calculer */
					     /* ces valeurs (cf. code de page.c) */
					    {
					       switch (pPRule->PrPosRule.PoPosDef)
						     {
							case Bottom:
							   h = pPRule->PrPosRule.PoDistance;
							   break;
							case Top:
							   h = pPRule->PrPosRule.PoDistance + pHeadR->PrDimRule.DrValue;
							   break;
							case HorizMiddle:
							   h = pPRule->PrPosRule.PoDistance + pHeadR->PrDimRule.DrValue / 2;
							   break;
							default:
							   h = 0;
							   break;
						     }

					       if (h > headHeight)
						  headHeight = h;
					    }
				    }	/* fin boite haut de page */
				  else if (pPRule->PrPosRule.PoRelation == RlNext)
				     /* la boite creee est positionnee par rapport */
				     /* au bas de la page, c'est une boite de bas de page */
				    {
				       pPSchema->PsPresentBox[hfB].PbPageFooter = True;
#endif /* __COLPAGE__ */
				       /* cherche la regle de hauteur de la boite de */
				       /* bas de page */
				       pHeadR = pPSchema->PsPresentBox[hfB].PbFirstPRule;
				       stop1 = False;
				       exist = False;
				       do
					 {
					    if (pHeadR == NULL)
					       stop1 = True;
					    else if (pHeadR->PrType > PtHeight)
					       stop1 = True;
					    else if (pHeadR->PrType == PtHeight)
					      {
						 stop1 = True;
						 exist = True;
					      }
					    if (!stop1)
					       pHeadR = pHeadR->PrNextPRule;
					 }
				       while (!stop1);
#ifdef __COLPAGE__
				       /* si pas de regle de hauteur, pas d'erreur */
				       if (exist)
#else  /* __COLPAGE__ */
				       if (!exist)
					  /* il n'y a pas de regle de hauteur, erreur */
					  TtaDisplayMessage (FATAL, TtaGetMessage (PRS, BAD_HEIGHT_RULE), pPSchema->PsPresentBox[hfB].PbName);
				       else
#endif /* __COLPAGE__ */
				       if (pHeadR->PrDimRule.DrPosition)
					  /* c'est une hauteur elastique, erreur */
					  TtaDisplayMessage (FATAL, TtaGetMessage (PRS, BAD_HEIGHT_RULE), pPSchema->PsPresentBox[hfB].PbName);
				       else if (!pHeadR->PrDimRule.DrAbsolute)
					  /* c'est une hauteur relative, on n'accepte */
					  /* que la hauteur du contenu */
					 {
					    if (pHeadR->PrDimRule.DrRelation != RlEnclosed)
					       TtaDisplayMessage (FATAL, TtaGetMessage (PRS, BAD_HEIGHT_RULE), pPSchema->PsPresentBox[hfB].PbName);
					 }
				       else
					  /* regle de hauteur absolue */
				       if (pHeadR->PrDimRule.DrUnit == UnRelative)
					  /* la hauteur n'est pas en unites fixes */
					  TtaDisplayMessage (FATAL, TtaGetMessage (PRS, BAD_HEIGHT_RULE), pPSchema->PsPresentBox[hfB].PbName);
				       else
					  /* calcule la distance entre le haut de la */
					  /* boite de bas de page et le bas de la page */
					  /* calcule la distance entre le bas de la */
					  /* boite de precedente et le bas de la */
					  /* boite creee (haut ou bas) */
					  /* ce code n'a plus vraiment de sens car */
					  /* le plus souvent, la hauteur des hauts et */
					  /* et bas de page est donnee par leur contenu */
					  /* (plusieurs boites filles) */
					  /* donc, c'est le mediateur qui peut calculer */
					  /* ces valeurs (cf. code de page.c) */
					 {
					    switch (pPRule->PrPosRule.PoPosDef)
						  {
						     case Top:
							h = pPRule->PrPosRule.PoDistance;
							break;
						     case Bottom:
							h = pPRule->PrPosRule.PoDistance + pHeadR->PrDimRule.DrValue;
							break;
						     case HorizMiddle:
							h = pPRule->PrPosRule.PoDistance + pHeadR->PrDimRule.DrValue / 2;
							break;
						     default:
							h = 0;
							break;
						  }

					    if (h > footHeight)
					       footHeight = h;
					 }
				    }
#ifndef __COLPAGE__
				  else
				     /* la boite creee n'est pas positionnee par */
				     /* rapport a la page, erreur */
				     TtaDisplayMessage (FATAL, TtaGetMessage (PRS, INVALID_VERTIC_POS_IN_PAGE), pPSchema->PsPresentBox[hfB].PbName);
#endif /* __COLPAGE__ */
				  /* examine les conditions de creation de la boite de */
				  /* haut ou de bas de page, a la recherche d'un */
				  /* compteur de page qui determine la creation de la */
				  /* boite de haut ou de bas de page. */
				  pCond = pR->PrCond;
				  while (pCond != NULL)
				    {
				       if (pCond->CoCondition == PcInterval ||
					   pCond->CoCondition == PcEven ||
					   pCond->CoCondition == PcOdd ||
					   pCond->CoCondition == PcOne)
					 {
					    counter = pCond->CoCounter;
					    if (PageCounter (counter))
					       /* c'est un compteur de pages */
					       if (pPSchema->PsPresentBox[b].PbPageCounter > 0)
						  /* il y a deja un compteur de page */
						 {
						    if (pPSchema->PsPresentBox[b].PbPageCounter != counter)
						       /* ce n'est pas le meme compteur */
						       TtaDisplayMessage (FATAL, TtaGetMessage (PRS, USES_DIFFERENT_PAGE_COUNTERS), pPSchema->PsPresentBox[b].PbName);
						 }
					       else
						  /* on a trouve' le compteur de pages associe' */
						  /* a ce type de page */
						  pPSchema->PsPresentBox[b].PbPageCounter = counter;
					 }
				       pCond = pCond->CoNextCondition;
				    }
				  /* la boite de haut ou de bas de page contient-elle */
				  /* le numero de page ? */
				  if (pPSchema->PsPresentBox[hfB].PbContent == ContVariable)
				     /* la boite contient une variable */
				    {
				       pPresVar = &pPSchema->PsVariable[pPSchema->PsPresentBox[hfB].PbContVariable - 1];
				       for (i = 0; i < pPresVar->PvNItems; i++)
					  if (pPresVar->PvItem[i].ViType == VarCounter)
					     /* la variable contient une valeur de */
					     /* compteur, on examine ce compteur */
					    {
					       counter = pPresVar->PvItem[i].ViCounter;
					       if (PageCounter (counter))
						  /* c'est un compteur de pages */
						 {
						    if (pPSchema->PsPresentBox[b].PbPageCounter > 0)
						       /* il y a deja un compteur de page */
						      {
							 if (pPSchema->PsPresentBox[b].PbPageCounter != counter)
							    /* ce n'est pas le meme compteur */
							    TtaDisplayMessage (FATAL, TtaGetMessage (PRS, TWO_DIFFERENT_PAGE_NUMBERS), pPSchema->PsPresentBox[hfB].PbName);
						      }
						    else
						       /* on trouve' le compteur de pages */
						       pPSchema->PsPresentBox[b].PbPageCounter = counter;
						    pPSchema->PsCounter[counter - 1].CnPageFooter =
						       pPSchema->PsPresentBox[hfB].PbPageFooter;
						 }
					    }
				    }
#ifdef __COLPAGE__
				  /* le numero de page peut etre fils de la boite */
				  /* haut ou bas de page, il faut donc chercher */
				  /* a un niveau plus bas (boites creees par hfB */
				  else
				    {
				       pRC = pPSchema->PsPresentBox[hfB].PbFirstPRule;
				       /* 1ere regle de presentation */
				       /* parcourt les premieres regles de la boite hfB */
				       stop1 = False;
				       do
					 {
					    if (pRC == NULL)
					       stop1 = True;
					    else if (pRC->PrType > PtFunction)
					       stop1 = True;
					    if (!stop1)
					      {
						 if (pRC->PrType == PtFunction
						     && (pRC->PrPresFunction == FnCreateBefore
							 || pRC->PrPresFunction == FnCreateFirst
							 || pRC->PrPresFunction == FnCreateLast
							 || pRC->PrPresFunction == FnCreateAfter))
						    /* c'est une regle de creation */
						    if (pRC->PrPresFunction == FnCreateBefore || pRC->PrPresFunction == FnCreateAfter)
						       /* ce n'est pas une regle de creation autorisee, erreur */
						       TtaDisplayMessage (FATAL, TtaGetMessage (PRS, FORBIDDEN_CREA_RULE), pPSchema->PsPresentBox[hfB].PbName);
						    else
						       /* la regle pRC est une regle CreatePremFils ou */
						       /* CreateDerFils */
						      {
							 bc = pRC->PrPresBox[0];
							 /* numero de la boite fille. Contient-elle un compteur ? */
							 /* cette boite appartient a la meme vue que la boite */
							 /* qui la cree */
							 viewOfBox[bc - 1] = viewOfBox[hfB];
							 /* contient-elle le numero de page ? */
							 if (pPSchema->PsPresentBox[bc - 1].PbContent == ContVariable)
							    /* la boite contient une variable */
							   {
							      pPresVar = &pPSchema->PsVariable[pPSchema->PsPresentBox[bc - 1].PbContVariable - 1];
							      for (i = 0; i < pPresVar->PvNItems; i++)
								 if (pPresVar->PvItem[i].ViType == VarCounter)
								    /* la variable contient une valeur de */
								    /* compteur, on examine ce compteur */
								   {
								      counter = pPresVar->PvItem[i].ViCounter;
								      if (PageCounter (counter))
									 /* c'est un compteur de pages */
									{
									   if (pPSchema->PsPresentBox[b].PbPageCounter > 0)
									      /* il y a deja un compteur de page */
									     {
										if (pPSchema->PsPresentBox[b].PbPageCounter != counter)
										   /* ce n'est pas le meme compteur */
										   TtaDisplayMessage (FATAL, TtaGetMessage (PRS, TWO_DIFFERENT_PAGE_NUMBERS), pPSchema->PsPresentBox[hfB].PbName);
									     }
									   else
									      /* on a trouve' le compteur de pages */
									      pPSchema->PsPresentBox[b].PbPageCounter = counter;
									   pPSchema->PsCounter[counter - 1].CnPageFooter =
									      pPSchema->PsPresentBox[hfB].PbPageFooter;
									}
								   }
							   }
						      }		/* fin regle creation fille */
					      }		/* fin parcours regle creation de hfB */
					    /* passe a la regle suivante de hfB */
					    pRC = pRC->PrNextPRule;
					 }	/* fin parcours regle creation de hfB */
				       while (!stop1);
				    }	/* fin recherche compteur chez les filles */
#endif /* __COLPAGE__ */
			       }	/* fin regle de position verticale OK  */
			  }
		     pR = pR->PrNextPRule;
		     /* passe a la regle suivante de b */
		  }
	     }
	   while (!stop);
	   /* on a traite' toutes les boites de haut et de bas de page de */
	   /* cette boite page */
	   pPSchema->PsPresentBox[b].PbFooterHeight = footHeight;
	   pPSchema->PsPresentBox[b].PbHeaderHeight = headHeight;
	   /* prevoir d'ajouter le positionnement de RPDimMin du corps */
	   /* de page */
	}
   /* les boites de haut et de bas de page sont maintenant marquees */
   /* verifie que seules les boites pages et les boites de haut et de */
   /* bas de page creent d'autres boites */
   /* verifie egalement les regles Content de toutes les boites */
   for (b = 0; b < pPSchema->PsNPresentBoxes; b++)
     {
	/* examine chaque boite */
	pPresBox = &pPSchema->PsPresentBox[b];
	if (!(pPresBox->PbPageHeader || pPresBox->PbPageFooter))
	   /* ce n'est pas une boite de haut ou bas de page, */
	   /* verifie sa regle Content */
	   if (pPresBox->PbContent == ContElement)
	      /* le contenu est un type d'element, erreur */
	      TtaDisplayMessage (FATAL, TtaGetMessage (PRS, BAD_CONTENT_RULE), pPSchema->PsPresentBox[b].PbName);
	if (pPresBox->PbPageHeader || pPresBox->PbPageFooter)
	   /* c'est une boite de haut ou bas de page, */
	   if (pPresBox->PbContent == ContElement)
	      /* son contenu est un type d'element */
	      if (viewOfBox[b] != 1)
		 /* elle n'est pas creee par une page de la vue 1 */
		 TtaDisplayMessage (FATAL, TtaGetMessage (PRS, FORBIDDEN_OUTSIDE_OF_MAIN_VIEW), pPSchema->PsPresentBox[b].PbName);
	/* cherche les regles de creation de la boite */
	pR = pPresBox->PbFirstPRule;
	/* 1ere regle de presentation */
	/* parcourt les premieres regles de la boite */
	stop = False;
	do
	  {
	     if (pR == NULL)
		stop = True;
	     else if (pR->PrType > PtFunction)
		stop = True;
	     if (!stop)
	       {
		  if (pR->PrType == PtFunction
		      && (pR->PrPresFunction == FnCreateBefore
			  || pR->PrPresFunction == FnCreateAfter))
		     /* ce n'est pas une regle de creation autorisee */
		     if (!pPresBox->PbPageBox)
			TtaDisplayMessage (FATAL, TtaGetMessage (PRS, FORBIDDEN_CREA_RULE), pPSchema->PsPresentBox[b].PbName);
		  pR = pR->PrNextPRule;
		  /* passe a la regle suivante */
	       }
	  }
	while (!stop);
     }
   /* verifie les regles de l'element PAGE_UNIT */
   for (view = 1; view <= pPSchema->PsNViews; view++)
      /* cherche la regle de positionnement vertical */
     {
	pR = SearchPRule (&pPSchema->PsElemPRule[PageBreak], PtVertPos, view);
	/* modifie la regle: positionnement au-dessous de l'element */
	/* precedent */
	pR->PrPresMode = PresImmediate;
	pR->PrPosRule.PoPosDef = Top;
	pR->PrPosRule.PoPosRef = Bottom;
	pR->PrPosRule.PoDistUnit = UnPoint;
	pR->PrPosRule.PoDistance = 0;
	pR->PrPosRule.PoRelation = RlPrevious;
	pR->PrPosRule.PoNotRel = False;
	pR->PrPosRule.PoRefKind = RkElType;
	pR->PrPosRule.PoUserSpecified = False;
	pR->PrPosRule.PoRefIdent = 0;
	/* cherche la regle de positionnement horizontal */
	pR = SearchPRule (&pPSchema->PsElemPRule[PageBreak], PtHorizPos, view);
	/* modifie la regle: positionnement sur le bord gauche de la */
	/* boite racine */
	pR->PrPresMode = PresImmediate;
	pR->PrPosRule.PoPosDef = Left;
	pR->PrPosRule.PoPosRef = Left;
	pR->PrPosRule.PoDistUnit = UnPoint;
	pR->PrPosRule.PoDistance = 0;
	pR->PrPosRule.PoRelation = RlRoot;
	pR->PrPosRule.PoNotRel = False;
	pR->PrPosRule.PoRefKind = RkElType;
	pR->PrPosRule.PoUserSpecified = False;
	pR->PrPosRule.PoRefIdent = 0;
	/* cherche la regle de largeur */
	pR = SearchPRule (&pPSchema->PsElemPRule[PageBreak], PtWidth, view);
	/* modifie la regle: largeur du contenu */
	pR->PrPresMode = PresImmediate;
	pR->PrDimRule.DrPosition = False;
	pR->PrDimRule.DrAbsolute = False;
	pR->PrDimRule.DrSameDimens = True;
	pR->PrDimRule.DrUnit = UnRelative;
	pR->PrDimRule.DrValue = 0;
	pR->PrDimRule.DrRelation = RlEnclosed;
	pR->PrDimRule.DrNotRelat = False;
	pR->PrDimRule.DrRefKind = RkElType;
	pR->PrDimRule.DrUserSpecified = False;
	pR->PrDimRule.DrRefIdent = 0;
	/* cherche la regle de hauteur */
	pR = SearchPRule (&pPSchema->PsElemPRule[PageBreak], PtHeight, view);
	/* modifie la regle: hauteur du contenu */
	pR->PrPresMode = PresImmediate;
	pR->PrDimRule.DrPosition = False;
	pR->PrDimRule.DrAbsolute = False;
	pR->PrDimRule.DrSameDimens = True;
	pR->PrDimRule.DrUnit = UnRelative;
	pR->PrDimRule.DrValue = 0;
	pR->PrDimRule.DrRelation = RlEnclosed;
	pR->PrDimRule.DrNotRelat = False;
	pR->PrDimRule.DrRefKind = RkElType;
	pR->PrDimRule.DrUserSpecified = False;
	pR->PrDimRule.DrRefIdent = 0;
       /* modifie la regle: HorizOverflow: True; */
	pR = SearchPRule (&pPSchema->PsElemPRule[PageBreak], PtHorizOverflow, view);
	pR->PrType = PtHorizOverflow;
	pR->PrPresMode = PresImmediate;
        pR->PrJustify = True;

     }
}

/*----------------------------------------------------------------------
   CheckUsedBoxes    parcourt la chaine de regles de presentation  
   commencant par la regle pointee par pRule et cherche toutes les 
   regles de creation ou de mise en page. Marque utilisee la       
   boite de presentation utilisee dans ces regles                  
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         CheckUsedBoxes (PtrPRule pRule, boolean usedBox[MAX_PRES_BOX])

#else  /* __STDC__ */
static void         CheckUsedBoxes (pRule, usedBox)
PtrPRule            pRule;
boolean             usedBox[MAX_PRES_BOX];

#endif /* __STDC__ */

{
   boolean             stop;
   int                 i;

   stop = False;
   do
     {
	if (pRule == NULL)
	   stop = True;
	/* derniere regle de la chaine */
	else if (pRule->PrType > PtFunction)
	   stop = True;
	/* on a vu la partie interessante de la chaine */
	if (!stop)
	  {
	     if (pRule->PrType == PtFunction
		 && (pRule->PrPresFunction == FnPage
		     || pRule->PrPresFunction == FnCreateEnclosing
		     || pRule->PrPresFunction == FnCreateBefore
		     || pRule->PrPresFunction == FnCreateWith
		     || pRule->PrPresFunction == FnCreateFirst
		     || pRule->PrPresFunction == FnCreateLast
		     || pRule->PrPresFunction == FnCreateAfter))
		/* c'est une regle de creation ou une regle Page */
		/* marque la boite utilisee */
		usedBox[pRule->PrPresBox[0] - 1] = True;
	     else if (pRule->PrType == PtFunction
		      && (pRule->PrPresFunction == FnColumn || pRule->PrPresFunction == FnSubColumn))
		/* c'est une regle de colonnage */
		for (i = 0; i < pRule->PrNPresBoxes; i++)
		   usedBox[pRule->PrPresBox[i] - 1] = True;
	     pRule = pRule->PrNextPRule;
	     /* passe a la regle suivante */
	  }
     }
   while (!stop);
}

/*----------------------------------------------------------------------
   CheckAllBoxesUsed verifie que toutes les boites de presentation 
   declarees dans le schema sont bien utilisees dans une regle de  
   creation ou une regle Page                                      
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static void         CheckAllBoxesUsed ()

#else  /* __STDC__ */
static void         CheckAllBoxesUsed ()
#endif				/* __STDC__ */

{
   int                 b, el, att, k, l;
   boolean             usedBox[MAX_PRES_BOX];
   AttributePres      *pPRuleA;

   /* marque d'abord qu'aucune boite de presentation n'est utilisee */
   for (b = 0; b < pPSchema->PsNPresentBoxes; b++)
      usedBox[b] = False;

   /* recherche toutes les regles de creation associees aux boites de */
   /* presentation et marque les boites utilisees par ces regles. */
   for (b = 0; b < pPSchema->PsNPresentBoxes; b++)
      CheckUsedBoxes (pPSchema->PsPresentBox[b].PbFirstPRule, usedBox);

   /* recherche toutes les regles de creation et les regles PAGE associees aux */
   /* types d'elements et marque les boites utilisees par ces regles. */
   for (el = 0; el < pSSchema->SsNRules; el++)
      /* cherche dans les regles de */
      /* presentation de chaque type d'element */
      CheckUsedBoxes (pPSchema->PsElemPRule[el], usedBox);

   /* recherche toutes les regles de creation associees aux attributs et */
   /* marque les boites utilisees par ces regles. */
   for (att = 0; att < pSSchema->SsNAttributes; att++)
      /* cherche dans les regles de presentation des attributs */
     {
	pPRuleA = pPSchema->PsAttrPRule[att];
	for (l = pPSchema->PsNAttrPRule[att]; l-- > 0; pPRuleA = pPRuleA->ApNextAttrPres)
	  {
	     /* selon le type de l'attribut */
	     switch (pSSchema->SsAttribute[att].AttrType)
		   {
		      case AtNumAttr:
			 for (k = 0; k < pPRuleA->ApNCases; k++)
			    CheckUsedBoxes (pPRuleA->ApCase[k].CaFirstPRule, usedBox);
			 break;
		      case AtTextAttr:
			 CheckUsedBoxes (pPRuleA->ApTextFirstPRule, usedBox);
			 break;
		      case AtReferenceAttr:
			 CheckUsedBoxes (pPRuleA->ApRefFirstPRule, usedBox);
			 break;
		      case AtEnumAttr:
			 for (k = 0; k <= pSSchema->SsAttribute[att].AttrNEnumValues; k++)
			    CheckUsedBoxes (pPRuleA->ApEnumFirstPRule[k], usedBox);
			 break;
		      default:
			 break;
		   }
	  }
     }

   /* sort un message pour chaque boite inutilisee */
   for (b = 0; b < pPSchema->PsNPresentBoxes; b++)
      if (!usedBox[b])
	 TtaDisplayMessage (INFO, TtaGetMessage (PRS, UNUSED_BOX), pPSchema->PsPresentBox[b].PbName);
}


/*----------------------------------------------------------------------
   main                                                            
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                main (int argc, char **argv)
#else  /* __STDC__ */
void                main (argc, argv)
int                 argc;
char              **argv;
#endif /* __STDC__ */
{
   FILE               *infile;
   boolean             fileOK;
   char                fname[200], buffer[200], cmd[800];
   char               *pwd, *ptr;
   Name                srceFileName;	/* nom du fichier a compiler */
   indLine             wi;	/* position du debut du mot courant dans la
				   ligne en cours */
   indLine             wl;	/* longueur du mot courant */
   SyntacticType       wn;	/* SyntacticType grammaticale du mot courant */
   SyntRuleNum         r;	/* numero de regle de grammaire */
   SyntRuleNum         pr;	/* numero de la regle de grammaire precedente */
   SyntacticCode       c;	/* code grammatical du mot trouve */
   int                 nb;	/* indice dans Identifier du mot trouve', si
				   identificateur */
   int                 i;
   int                 param;

   TtaInitializeAppRegistry (argv[0]);
   i = TtaGetMessageTable ("libdialogue", TMSG_LIB_MSG_MAX);
   COMPIL = TtaGetMessageTable ("compildialogue", COMP_MSG_MAX);
   PRS = TtaGetMessageTable ("prsdialogue", PRS_MSG_MAX);
   error = False;
   /* initialize the parser */
   InitParser ();
   /* load the compiler grammar */
   InitSyntax ("PRESEN.GRM");
   if (!error)
     {
        /* prepare the cpp command */
	strcpy (cmd, "cpp ");
        param = 1;
	while (param < argc && argv[param][0] == '-')
	  {
	    /* keep cpp params */
	    strcat (cmd, argv[param]);
	    strcat (cmd, " ");
	    param++;
	  }

	/* recupere d'abord le nom du schema a compiler */
	if (param >= argc)
	  {
	     TtaDisplaySimpleMessage (FATAL, PRS, UNKNOWN_FILE);
	     exit (1);
	  }
	strncpy (srceFileName, argv[param], MAX_NAME_LENGTH - 1);
	srceFileName[MAX_NAME_LENGTH - 1] = '\0';
	param++;
	strcpy (fname, srceFileName);
	/* check if the name contains a suffix */
	ptr = strrchr(fname, '.');
	nb = strlen (srceFileName);
	if (!ptr)
	  /* there is no suffix */
	  strcat (srceFileName, ".P");
	else if (strcmp(ptr, ".P"))
	  {
	    /* it's not the valid suffix */
	    TtaDisplayMessage (FATAL, TtaGetMessage (PRS, INVALID_FILE), srceFileName);
	    exit (1);
	  }
	else
	  {
	    /* it's the valid suffix, cut the srcFileName here */
	    ptr[0] = '\0';
	    nb -= 2; /* length without the suffix */
	  }
	/* add the suffix .SCH in srceFileName */
	strcat (fname, ".SCH");
	
	/* does the file to compile exist */
	if (TtaFileExist (srceFileName) == 0)
	  TtaDisplaySimpleMessage (FATAL, PRS, UNKNOWN_FILE);
	else
	  {
	    /* provide the real source file */
	    TtaFileUnlink (fname);
	    pwd = TtaGetEnvString ("PWD");
	    i = strlen (cmd);
	    if (pwd != NULL)
	      sprintf (&cmd[i], "-I%s -C %s > %s", pwd, srceFileName, fname);
	    else
	      sprintf (&cmd[i], "-C %s > %s", srceFileName, fname);
	    i = system (cmd);
	    if (i == -1)
	      {
		/* cpp is not available, copy directely the file */
		TtaDisplaySimpleMessage (INFO, PRS, CPP_NOT_FOUND);
		TtaFileCopy (srceFileName, fname);
	      }

	    infile = TtaReadOpen (fname);
	    if (param == argc)
	      /* the output name is equal to the input name */
	      /*suppress the suffix ".SCH" */
	      srceFileName[nb] = '\0';
	    else
	      /* read the output name */
	      strncpy (srceFileName, argv[param], MAX_NAME_LENGTH - 1);
	     /* le fichier a compiler est ouvert */
	     NIdentifiers = 0;	/* table des identificateurs vide */
	     LineNum = 0;	/* encore aucune ligne lue */
	     pSSchema = NULL;	/* pas (encore) de schema de structure */
	     /* lit tout le fichier et fait l'analyse */
	     fileOK = True;
	     while (fileOK && !error)
	       {
		  /* lit une ligne */
		  LineNum++;	/* incremente le compteur de lignes lues */
		  i = 0;
		  do
		    {
		       fileOK = TtaReadByte (infile, &inputLine[i]);
		       i++;
		    }
		  while (i < LINE_LENGTH && inputLine[i - 1] != '\n' && fileOK);
		  /* marque la fin reelle de la ligne */
		  inputLine[i - 1] = '\0';
		  if (i >= LINE_LENGTH)
		     /* ligne trop longue */
		     CompilerMessage (1, PRS, FATAL, MAX_LINE_SIZE_OVERFLOW, inputLine,
				    LineNum);
		  else if (inputLine[0] == '#')
		     /* cette ligne contient une directive du preprocesseur cpp */
		    {
		       sscanf (inputLine, "# %d %s", &LineNum, buffer);
		       LineNum--;
		    }
		  else
		     /* traduit tous les caracteres de la ligne */
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
			       /* word found */
			      {
				 AnalyzeToken (wi, wl, wn, &c, &r, &nb, &pr);
				 /* analyze the word */
				 if (!error)
				    ProcessToken (wi, wl, c, r, nb - 1, pr);	/* on le traite */
			      }
			 }
		       while (wi != 0 && !error);
		       /* il n'y a plus de mots a analyser dans la ligne */
		    }
	       }

	     /* end of file */
	     TtaReadClose (infile);
	     if (!error)
		ParserEnd ();
	     /* fin d'analyse */
	     if (!error)
	       {
		  SortAllPRules ();
		  /* met les regles de presentation dans le bon ordre. */
		  /* si aucune vue n'est definie, cree la vue par defaut avec un nom */
		  /* standard */
		  if (pPSchema->PsNViews == 0)
		    {
		       pPSchema->PsNViews = 1;
		       strcpy (pPSchema->PsView[0], TtaGetMessage (PRS, SINGLE_VIEW));
		    }
		  /* verifie que toutes les boites de presentation declarees pour les */
		  /* pages sont bien utilisees et adapte les regles. */
		  /* cela ne peut se faire qu'apres avoir ajoute' la vue par defaut */
		  CheckPageBoxes ();
		  /* verifie que toutes les boites de presentation sont bien utilisees */
		  CheckAllBoxesUsed ();
		  /* write the compiled schema into the output file */
		  /* remove temporary file */
		  TtaFileUnlink (fname);
		  strcat (srceFileName, ".PRS");
		  fileOK = WritePresentationSchema (srceFileName, pPSchema, pSSchema);
		  if (!fileOK)
		     TtaDisplayMessage (FATAL, TtaGetMessage (PRS, WRITE_ERROR), srceFileName);
	       }
	  }
     }
   fflush (stdout);
   TtaSaveAppRegistry ();
   exit (0);
}
