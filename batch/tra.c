/*
 *
 *  (c) COPYRIGHT INRIA 1996-2005
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * Ce programme compile un schema de traduction contenu dans un fichier
 * de type .T
 * Il est dirige' par la grammaire du langage de 
 * contenue, sous forme codee, dans le fichier TRANS.GRM.
 * Il produit un fichier de type .TRA qui sera ensuite utilise'
 * par le traducteur, pour guider sa traduction.
 *
 * Author: V. Quint (INRIA)
 *         R. Guetari (W3C/INRIA): Windows.
 *
 */

#include "thot_sys.h"
#include "tradef.h"
#include "constgrm.h"
#include "constmedia.h"
#include "consttra.h"
#include "typemedia.h"
#include "typegrm.h"
#include "fileaccess.h"
#include "compilmsg.h"
#include "tramsg.h"
#include "message.h"

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "compil_tv.h"

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "platform_tv.h"
#include "thotcolor.h"
#include "thotcolor_tv.h"
#include "thotpattern.h"

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "analsynt_tv.h"


#ifdef _WINGUI
#define FATAL_EXIT_CODE 33
#define COMP_SUCCESS     0
#else  /* !_WINGUI */
#define FATAL_EXIT_CODE -1
#endif /* _WINGUI */

int                LineNum;	/* compteur de lignes */

static PtrSSchema  pSSchema;	/* pointeur sur le schema de structure */
static PtrSSchema  pExtSSchema;	/* pointeur sur le schema de structure
				   externe */
static PtrTSchema  pTSchema;	/* pointeur sur le schema de traduction */

static ThotBool    BuffDef;	/* on est dans les declarations de buffers */
static ThotBool    ConstDef;	/* on est dans les declaration de constantes */
static ThotBool    ComptDef;	/* on est dans les declarations de compteurs */
static ThotBool    VarDef;	/* on est dans les declarations de variables */
static ThotBool    InTypeRules;	/* on est dans les regles des types */
static ThotBool    InAttrRules;	/* on est dans les regles des attributs */
static ThotBool    InPresRules;	/* on est dans les regles de la presentation */
static ThotBool    InCondBlock;	/* on est dans un bloc conditionnel */
static ThotBool    InCondition;	/* on analyse une condition 'If...TRuleBlock'*/
static ThotBool    TextTrans;	/* on est dans les traductions de texte */
static ThotBool    SymbTrans;	/* on est dans les traductions de symbole */
static ThotBool    GraphTrans;	/* on est dans les traductions de graphique */

static int         CurType;	/* numero du type d'element courant */
static int         CurAttr;	/* numero de l'attribut courant */
static int         CurValAttr;	/* numero de la valeur d'attribut */
static int         CurPres;	/* numero de la presentation courante */
static int         CurPresVal;	/* numero de la valeur courante de la
				   presentation courante */
static ThotBool    ChangeRuleBlock;  /* il faut ouvrir un nouveau bloc de
					regles */
static PtrTRuleBlock CurBlock;	/* bloc de regles courant */
static PtrTRule    CurTRule;	/* current translation SyntacticRule */
static int         CurEntry;	/* entree courante de la table de traduction
				   de caracteres */
static int         ConstIndx;	/* indice courant dans le buffer des
				   constantes */
static ThotBool    ExternalSchema; /* TypeIdent est un nom de schema externe */
static Name        TypeInGetRule;  /* TypeIdent dans la regle Get courante */
static int         BeginTypeInGetRule;	/* indice du debut du TypeIdent dans
				   la ligne */
static Name        TypeWithin;	/* TypeIdent dans la condition Within ou
				   First Within en cours d'analyse */
static int         BeginTypeWithin; /* indice du debut de ce TypeIdent dans la
				   ligne en cours de traitement */
static Name        AncestorName;/* 1er TypeIdent dans la regle LevelOrType */
static int         BeginAncestorName; /* indice du debut de ce TypeIdent dans
				   la ligne en cours de traitement */
static ThotBool    Immediately;	/* condition Immediately Within */
static ThotBool    Asterisk;	/* on a rencontre' "" dans une condition */
static ThotBool    GreaterOrLessSign; /* On a rencontre' un signe > ou < apres
				   Within ou FirstWithin dans la condition
				   courante */
static int         AttrValSign;	/* Signe d'une valeur d'attribut numerique */
static int         PresValSign;	/* Signe d'une valeur de presentation
				   numerique */
static int         AncestorSign;/* signe du dernier niveau d'ancetre
				   rencontre' dans un compteur */
static int	   IndentSign;	/* identation sign */
static ThotBool    VarDefinition;
static ThotBool    FirstInPair;	/* on a rencontre' "First" */
static ThotBool    SecondInPair;/* on a rencontre' "Second" */

#include "platform_f.h"
#include "parser_f.h"
#include "readstr_f.h"
#include "writetra_f.h"
#include "memory_f.h"
#include "message_f.h"
#include "compilmsg_f.h"
#include "registry_f.h"

#ifdef _WINGUI
#include "compilers_f.h"
#ifndef DLLEXPORT
#define DLLEXPORT __declspec (dllexport)
#endif /* DLLEXPORT */
#endif /* _WINGUI */

/*----------------------------------------------------------------------
   Initialize	initialise le schema de traduction en memoire   
  ----------------------------------------------------------------------*/
static void         Initialize ()
{
   int                 i;

   BuffDef = False;
   ConstDef = False;
   ComptDef = False;
   VarDef = False;
   InTypeRules = False;
   InAttrRules = False;
   InPresRules = False;
   TextTrans = False;
   SymbTrans = False;
   GraphTrans = False;
   ChangeRuleBlock = False;
   InCondBlock = False;
   ConstIndx = 1;
   CurBlock = NULL;
   CurTRule = NULL;
   ExternalSchema = False;
   InCondition = False;
   AttrValSign = 1;
   PresValSign = 1;
   FirstInPair = False;
   SecondInPair = False;
   Asterisk = False;
   TypeWithin[0] = '\0';
   AncestorName[0] = '\0';

   GetSchStruct (&pExtSSchema);
   GetSchStruct (&pSSchema);
   GetSchTra (&pTSchema);
   if (pTSchema == NULL || pSSchema == NULL || pExtSSchema == NULL)
     TtaDisplaySimpleMessage (FATAL, TRA, OUT_OF_MEMORY);

   /* initialise le schema de traduction */
   pTSchema->TsLineLength = 0;	/* pas de longueur max des lignes traduites */
   strcpy (pTSchema->TsEOL, "\n");	/* caractere fin de ligne par
					   defaut */
   strcpy (pTSchema->TsTranslEOL, "\n");	/* fin de ligne a inserer par
						   defaut */
   pTSchema->TsNConstants = 0;	/* nombre de constantes */
   pTSchema->TsNCounters = 0;	/* nombre de compteurs */
   pTSchema->TsNVariables = 0;	/* nombre de variables de traduction */
   pTSchema->TsNBuffers = 0;	/* nombre de buffers */
   pTSchema->TsPictureBuffer = 0;	/* pas de buffer pour les images */
   pTSchema->TsNVarBuffers = 0; /* nombre de buffer de type variable */
   for (i = 0; i < MAX_TRANSL_PRULE; i++)
      pTSchema->TsPresTRule[i].RtExist = False;
   pTSchema->TsNTranslScripts = 0;	/* pas de traduction de texte */
   pTSchema->TsSymbolFirst = 0;	  /* indice de la 1ere regle de traduction
				     de symboles dans la table TsCharTransl */
   pTSchema->TsSymbolLast = 0;	  /* indice de la derniere regle de traduction
				     de symboles dans la meme table */
   pTSchema->TsGraphicsFirst = 0; /* indice de la 1ere regle de traduction de
				     graphiques dans la table TsCharTransl */
   pTSchema->TsGraphicsLast = 0;  /* indice de la derniere regle de traduction
				     de graphiques dans la meme table */
   pTSchema->TsNCharTransls = 0;  /* nombre total de regles de traduction de
				     caracteres */
}

/*----------------------------------------------------------------------
   initialise les regles de traduction des attributs               
  ----------------------------------------------------------------------*/
static void         InitAttrTransl ()
{
   int                 i, j, size;
   PtrAttributeTransl  pAttTr;
   TranslNumAttrCase  *pCase;

   size = pSSchema->SsNAttributes * sizeof (PtrAttributeTransl);
   pTSchema->TsAttrTRule =  (AttrTransTable*) malloc (size);
   if (pTSchema->TsAttrTRule)
     memset (pTSchema->TsAttrTRule, 0, size);
   for (i = 0; i < pSSchema->SsNAttributes; i++)
     {
       pAttTr = (PtrAttributeTransl) malloc (sizeof (AttributeTransl));
       pTSchema->TsAttrTRule->TsAttrTransl[i] = pAttTr;
       pAttTr->AtrElemType = 0;
       /* selon le type de l'attribut */
       switch (pSSchema->SsAttribute->TtAttr[i]->AttrType)
	 {
	 case AtNumAttr:
	   pAttTr->AtrNCases = 0;
	   for (j = 0; j < MAX_TRANSL_ATTR_CASE; j++)
	     {
	       pCase = &pAttTr->AtrCase[j];
	       pCase->TaLowerBound = -MAX_INT_ATTR_VAL - 1;	/* - infini */
	       pCase->TaUpperBound = MAX_INT_ATTR_VAL + 1;	/* + infini */
	       pCase->TaTRuleBlock = NULL;
	     }
	   break;
	 case AtTextAttr:
	   pAttTr->AtrTextValue[0] = '\0';
	   pAttTr->AtrTxtTRuleBlock = NULL;
	   break;
	 case AtReferenceAttr:
	   pAttTr->AtrRefTRuleBlock = NULL;
	   break;
	 case AtEnumAttr:
	   for (j = 0; j <= MAX_ATTR_VAL; j++)
	     pAttTr->AtrEnuTRuleBlock[j] = NULL;
	   break;
	 default:
	   break;
	 }
     }
}

/*----------------------------------------------------------------------
   initialise les regles de traduction des elements               
  ----------------------------------------------------------------------*/
static void         InitElemTransl ()
{
  int                 i, size;

  size = pSSchema->SsNRules * sizeof (PtrTRuleBlock);
  pTSchema->TsElemTRule = (ElemTransTable*) malloc (size);
  size = pSSchema->SsNRules * sizeof (ThotBool);
  pTSchema->TsInheritAttr = (BlnTable*) malloc (size);
  if (pTSchema->TsElemTRule && pTSchema->TsInheritAttr)
    for (i = 0; i < pSSchema->SsNRules; i++)
      {
      pTSchema->TsElemTRule->TsElemTransl[i] = NULL; /* pointeurs sur le debut
				     de la chaine de regles de traduction
				     propres a chaque type d'element */
      pTSchema->TsInheritAttr->Bln[i] = False;
      }
}

/*----------------------------------------------------------------------
   EndOfCondition
   traite le type d'element indique' dans une condition Within ou  
   FirstWithin Le nom du type se trouve dans la variable TypeWithin 
  ----------------------------------------------------------------------*/
static void         EndOfCondition (PtrSSchema pSS)
{
   int                 i;

   if (TypeWithin[0] != '\0')
     /* verifie si le type existe dans le schema de structure */
     {
       i = 0;
       while (strcmp (TypeWithin, pSS->SsRule->SrElem[i]->SrName) != 0 &&
	      i < pSS->SsNRules - 1)
	 i++;
       if (strcmp (TypeWithin, pSS->SsRule->SrElem[i]->SrName) != 0)
	 /* type inconnu */
	 CompilerMessage (BeginTypeWithin, TRA, FATAL, BAD_TYPE, inputLine,
			  LineNum);
       else
	 /* le type existe, il a le numero i */
	 CurBlock->TbCondition[CurBlock->TbNConditions - 1].TcElemType = i + 1;
       TypeWithin[0] = '\0';	/* le nom de type a ete traite' */
     }
}

/*----------------------------------------------------------------------
   traite le type d'element indique' dans une clause Ancestor      
   Le nom du type se trouve dans la variable AncestorName.         
  ----------------------------------------------------------------------*/
static void ProcessAncestorName (PtrSSchema pSS)
{
   int                 i;

   if (AncestorName[0] != '\0')
     /* verifie si le type existe dans le schema de structure */
     {
       i = 0;
       while (strcmp (AncestorName, pSS->SsRule->SrElem[i]->SrName) != 0 &&
	      i < pSS->SsNRules - 1)
	 i++;
       if (strcmp (AncestorName, pSS->SsRule->SrElem[i]->SrName) != 0)
	 /* type inconnu */
	 CompilerMessage (BeginAncestorName, TRA, FATAL, BAD_TYPE, inputLine,
			  LineNum);
       else
	 /* le type existe, il a le numero i */
	 CurBlock->TbCondition[CurBlock->TbNConditions - 1].TcAscendType = i+1;
       AncestorName[0] = '\0';	/* le nom de type a ete traite' */
     }
}

/*----------------------------------------------------------------------
   traite un nom de regle de presentation passee en parametre      
  ----------------------------------------------------------------------*/
static void PresentationName (PRuleType TypeRPres, SyntRuleNum pr, int wi)
{
   int                 i, j;
   PRuleTransl        *pPresTrans;
   TranslNumAttrCase  *pCase;

   if (pr == RULE_TransPres)
     /* debut des regles de traduction d'une presentation */
     {
       CurPres = TypeRPres + 1;
       CurPresVal = 0;
       pPresTrans = &pTSchema->TsPresTRule[CurPres - 1];
       if (!pPresTrans->RtExist)
	 /* pas encore rencontre' de regles pour cette presentation */
	 {
	   pPresTrans->RtExist = True;
	   if (TypeRPres == PtBreak1 ||
	       TypeRPres == PtBreak2 ||
	       TypeRPres == PtIndent ||
	       TypeRPres == PtSize ||
	       TypeRPres == PtLineSpacing ||
	       TypeRPres == PtLineWeight ||
	       TypeRPres == PtMarginTop ||
	       TypeRPres == PtMarginRight ||
	       TypeRPres == PtMarginBottom ||
	       TypeRPres == PtMarginLeft ||
	       TypeRPres == PtPaddingTop ||
	       TypeRPres == PtPaddingRight ||
	       TypeRPres == PtPaddingBottom ||
	       TypeRPres == PtPaddingLeft ||
	       TypeRPres == PtBorderTopWidth ||
	       TypeRPres == PtBorderRightWidth ||
	       TypeRPres == PtBorderBottomWidth ||
	       TypeRPres == PtBorderLeftWidth ||
	       TypeRPres == PtXRadius ||
	       TypeRPres == PtYRadius ||
	       TypeRPres == PtTop ||
	       TypeRPres == PtRight ||
	       TypeRPres == PtBottom ||
	       TypeRPres == PtLeft ||
	       TypeRPres == PtVisibility ||
	       TypeRPres == PtListStyleImage ||
	       TypeRPres == PtDepth ||
	       TypeRPres == PtFillPattern ||
	       TypeRPres == PtBackground ||
	       TypeRPres == PtForeground ||
	       TypeRPres == PtBorderTopColor ||
	       TypeRPres == PtBorderRightColor ||
	       TypeRPres == PtBorderBottomColor ||
	       TypeRPres == PtBorderLeftColor ||
	       TypeRPres == PtOpacity ||
	       TypeRPres == PtFillOpacity ||
	       TypeRPres == PtStrokeOpacity)
	     /* Presentation a valeur numerique */
	     {
	       pPresTrans->RtNCase = 0;
	       for (j = 0; j < MAX_TRANSL_PRES_CASE; j++)
		 {
		   pCase = &pPresTrans->RtCase[j];
		   pCase->TaLowerBound = -MAX_INT_ATTR_VAL - 1;	/* - infini */
		   pCase->TaUpperBound = MAX_INT_ATTR_VAL + 1;	/* + infini */
		   pCase->TaTRuleBlock = NULL;
		 }
	     }
	   else
	     for (j = 0; j <= MAX_TRANSL_PRES_VAL; j++)
	       {
		 pPresTrans->RtPRuleValue[j] = '\0';
		 pPresTrans->RtPRuleValueBlock[j] = NULL;
	       }
	 }
       CurBlock = NULL;
       CurTRule = NULL;
       if (TypeRPres == PtBreak1 ||
	   TypeRPres == PtBreak2 ||
	   TypeRPres == PtIndent ||
	   TypeRPres == PtSize ||
	   TypeRPres == PtLineSpacing ||
	   TypeRPres == PtLineWeight ||
	   TypeRPres == PtMarginTop ||
	   TypeRPres == PtMarginRight ||
	   TypeRPres == PtMarginBottom ||
	   TypeRPres == PtMarginLeft ||
	   TypeRPres == PtPaddingTop ||
	   TypeRPres == PtPaddingRight ||
	   TypeRPres == PtPaddingBottom ||
	   TypeRPres == PtPaddingLeft ||
	   TypeRPres == PtBorderTopWidth ||
	   TypeRPres == PtBorderRightWidth ||
	   TypeRPres == PtBorderBottomWidth ||
	   TypeRPres == PtBorderLeftWidth ||
	   TypeRPres == PtXRadius ||
	   TypeRPres == PtYRadius ||
	   TypeRPres == PtTop ||
	   TypeRPres == PtRight ||
	   TypeRPres == PtBottom ||
	   TypeRPres == PtLeft ||
	   TypeRPres == PtVisibility ||
	   TypeRPres == PtListStyleImage ||
	   TypeRPres == PtDepth ||
	   TypeRPres == PtFillPattern ||
	   TypeRPres == PtBackground ||
	   TypeRPres == PtForeground ||
	   TypeRPres == PtBorderTopColor ||
	   TypeRPres == PtBorderRightColor ||
	   TypeRPres == PtBorderBottomColor ||
	   TypeRPres == PtBorderLeftColor ||
	   TypeRPres == PtOpacity ||
	   TypeRPres == PtFillOpacity ||
	   TypeRPres == PtStrokeOpacity)
	 /* Presentation a valeur numerique */
	 {
	 if (pPresTrans->RtNCase >= MAX_TRANSL_PRES_CASE)
	   /* trop de cas pour cette presentation */
	   CompilerMessage (wi, TRA, FATAL,
			    MAX_POSSIBLE_CASES_FOR_PRES_OVERFLOW,
			    inputLine, LineNum);
	 else
	   pPresTrans->RtNCase++;	/* un cas de plus */
	 }
     }
   else if (pr == RULE_CondOnAscend)
     /* un nom de presentation dans une condition */
     {
       ProcessAncestorName (pSSchema);
       i = CurBlock->TbNConditions - 1;
       CurBlock->TbCondition[i].TcCondition = TcondPRule;
       CurBlock->TbCondition[i].TcAttr = TypeRPres;
       if (TypeRPres == PtBreak1 ||
	   TypeRPres == PtBreak2 ||
	   TypeRPres == PtIndent ||
	   TypeRPres == PtSize ||
	   TypeRPres == PtLineSpacing ||
	   TypeRPres == PtLineWeight ||
	   TypeRPres == PtMarginTop ||
	   TypeRPres == PtMarginRight ||
	   TypeRPres == PtMarginBottom ||
	   TypeRPres == PtMarginLeft ||
	   TypeRPres == PtPaddingTop ||
	   TypeRPres == PtPaddingRight ||
	   TypeRPres == PtPaddingBottom ||
	   TypeRPres == PtPaddingLeft ||
	   TypeRPres == PtBorderTopWidth ||
	   TypeRPres == PtBorderRightWidth ||
	   TypeRPres == PtBorderBottomWidth ||
	   TypeRPres == PtBorderLeftWidth ||
	   TypeRPres == PtXRadius ||
	   TypeRPres == PtYRadius ||
	   TypeRPres == PtTop ||
	   TypeRPres == PtRight ||
	   TypeRPres == PtBottom ||
	   TypeRPres == PtLeft ||
	   TypeRPres == PtVisibility ||
	   TypeRPres == PtListStyleImage ||
	   TypeRPres == PtDepth ||
	   TypeRPres == PtFillPattern ||
	   TypeRPres == PtBackground ||
	   TypeRPres == PtForeground ||
	   TypeRPres == PtBorderTopColor ||
	   TypeRPres == PtBorderRightColor ||
	   TypeRPres == PtBorderBottomColor ||
	   TypeRPres == PtBorderLeftColor ||
	   TypeRPres == PtOpacity ||
	   TypeRPres == PtFillOpacity ||
	   TypeRPres == PtStrokeOpacity)
	 /* Presentation a valeur numerique */
	 {
	   CurBlock->TbCondition[i].TcLowerBound = -MAX_INT_ATTR_VAL - 1;
	   /* -infini */
	   CurBlock->TbCondition[i].TcUpperBound = MAX_INT_ATTR_VAL + 1;
	   /* infini */
	 }
       else
	 CurBlock->TbCondition[i].TcPresValue = '\0';
     }
}

/*----------------------------------------------------------------------
   traite le mot-cle' representant une valeur d'une presentation.  
  ----------------------------------------------------------------------*/
static void PresentValue (char value, int wi)
{
  int                 i;
  PRuleTransl        *pPresTrans;
  
  if (InCondition)
    /* valeur d'une presentation dans une condition */
    CurBlock->TbCondition[CurBlock->TbNConditions - 1].TcPresValue = value;
  else
    {
      /* ajoute une nouvelle valeur pour une presentation */
      /* cherche la premier entree libre dans les valeurs de la */
      /* presentation courante */
      pPresTrans = &pTSchema->TsPresTRule[CurPres - 1];
      i = 0;
      CurPresVal = 0;
      do
	{
	  i++;
	  if (pPresTrans->RtPRuleValue[i] == value)
	    /* deja des regles pour cette valeur */
	    CompilerMessage (wi, TRA, FATAL,RULES_ALREADY_EXIST_FOR_THAT_VALUE,
			     inputLine, LineNum);
	  else if (pPresTrans->RtPRuleValue[i] == '\0')
	    /* une entree libre, on la prend */
	    {
	      CurPresVal = i;
	      pPresTrans->RtPRuleValue[i] = value;
	    }
	}
      while (CurPresVal == 0 && i <= MAX_TRANSL_PRES_VAL + 1);
    }
}

/*----------------------------------------------------------------------
   SrceStringGreater                                               
  ----------------------------------------------------------------------*/
static int SrceStringGreater (int framet, int lent, int rank,
			      ThotBool *equal, SourceString source)
{
   int                 i, k, s;
   ThotBool             eq;

   i = framet;
   eq = False;
   while (pTSchema->TsCharTransl[i - 1].StSource[rank] < source[rank] &&
	  i <= lent)
     i++;
   if (i > lent)
     s = lent + 1;
   else if (pTSchema->TsCharTransl[i - 1].StSource[rank] > source[rank])
     s = i;
   else if (source[rank] == '\0')
     {
       eq = True;
       s = i;
     }
   else
     {
       k = i - 1;
       while (pTSchema->TsCharTransl[k].StSource[rank] == source[rank])
	 k++;
       s = SrceStringGreater (i, k, rank + 1, &eq, source);
     }
   *equal = eq;
   return s;
}

/*----------------------------------------------------------------------
   NewSourceString   traite la chaine source d'une nouvelle regle  
   de traduction de caracteres.                                    
   indx : index dans inputLine du premier caractere de la chaine,  
   len est la longueur de la chaine.                                
  ----------------------------------------------------------------------*/
static void NewSourceString (int indx, int len)
{
   int                 k;
   ThotBool             equal;
   SourceString        source;
   int                 firstEntry, lastEntry;
   ScriptTransl     *pAlphTrans;

   firstEntry = 0;
   lastEntry = 0;
   if (TextTrans)
     /* on est dans les traductions de texte */
     {
       pAlphTrans = &pTSchema->TsTranslScript[pTSchema->
					       TsNTranslScripts - 1];
       firstEntry = pAlphTrans->AlBegin;
       lastEntry = pAlphTrans->AlEnd;
     }
   else if (SymbTrans)
     /* on est dans les traductions de symbole */
     {
       firstEntry = pTSchema->TsSymbolFirst;
       lastEntry = pTSchema->TsSymbolLast;
     }
   else if (GraphTrans)
     /* on est dans les traductions de graphique */
     {
       firstEntry = pTSchema->TsGraphicsFirst;
       lastEntry = pTSchema->TsGraphicsLast;
     }
   if (lastEntry >= MAX_TRANSL_CHAR)
     /* message 'Table saturee' */
     CompilerMessage (1, TRA, FATAL, NO_SPACE_LEFT_IN_TRANSLATION_TABLE,
		      inputLine, LineNum);
   else
     /* recupere le parametre 'source' */
     {
       for (k = 0; k <= len - 2; k++)
	 source[k] = inputLine[indx + k - 1];
       source[len - 1] = '\0';
       CurEntry = SrceStringGreater (firstEntry, lastEntry, 0, &equal, source);
       if (equal)
	 /* la chaine source est deja dans la table */
	 CompilerMessage (indx, TRA, FATAL, CANT_RESPECIFY_TRANSLATION,
			  inputLine, LineNum);
       else
	 /* la chaine source n'est pas dans la table */
	 {
	   /* on l'insere dans la table */
	   for (k = lastEntry; k >= CurEntry; k--)
	     pTSchema->TsCharTransl[k] = pTSchema->TsCharTransl[k - 1];
	   strncpy ((char *)pTSchema->TsCharTransl[CurEntry - 1].StSource, (char *)source,
		     MAX_SRCE_LEN + 1);
	   if (TextTrans)
	     /* on est dans les traductions de texte */
	     {
	       pAlphTrans = &pTSchema->TsTranslScript[pTSchema->
						       TsNTranslScripts - 1];
	       pAlphTrans->AlEnd++;
	     }
	   else if (SymbTrans)
	     /* on est dans les traductions de symbole */
	     pTSchema->TsSymbolLast++;
	   else if (GraphTrans)
	     /* on est dans les traduc. de graphique */
	     pTSchema->TsGraphicsLast++;
	   pTSchema->TsNCharTransls++;
	 }
     }
}

/*----------------------------------------------------------------------
   ProcessTargetString traite la chaine cible de la regle de       
   traduction de caracteres en cours de construction.              
   indx: index dans inputLine du premier caractere de la chaine,   
   len est la longueur de la chaine.                               
  ----------------------------------------------------------------------*/
static void ProcessTargetString (int indx, int len)
{
   int                 k;
   TargetString        target;

   /* recupere le parametre 'target' */
   for (k = 0; k <= len - 2; k++)
     target[k] = inputLine[indx + k - 1];
   target[len - 1] = '\0';
   strncpy ((char *)pTSchema->TsCharTransl[CurEntry - 1].StTarget, (char *)target,
	     MAX_TARGET_LEN + 1);
}

/*----------------------------------------------------------------------
   Cree et initialise un nouveau bloc de regles                    
  ----------------------------------------------------------------------*/
static void NewRuleBlock ()
{
   PtrTRuleBlock       pBlock;
   PtrAttributeTransl  pAttrTrans;
   PRuleTransl        *pPresTrans;

   ChangeRuleBlock = False;
   if ((pBlock = (PtrTRuleBlock) TtaGetMemory (sizeof (TRuleBlock))) == NULL)
     TtaDisplaySimpleMessage (FATAL, TRA, OUT_OF_MEMORY);
   else
     memset (pBlock, 0, sizeof (TRuleBlock));
   /* cree un pBlock */
   if (CurBlock == NULL)
     /* pas de bloc courant, attache ce bloc au schema de traduction */
     if (InTypeRules)
       /* bloc de regles associe' a un type d'element */
       pTSchema->TsElemTRule->TsElemTransl[CurType - 1] = pBlock;
     else if (InAttrRules)
       /* bloc de regles associe' a un attribut */
       {
	 pAttrTrans = pTSchema->TsAttrTRule->TsAttrTransl[CurAttr - 1];
	 switch (pSSchema->SsAttribute->TtAttr[CurAttr - 1]->AttrType)
	   {
	   case AtNumAttr:
	     pAttrTrans->AtrCase[pAttrTrans->AtrNCases - 1].TaTRuleBlock =
	       pBlock;
	     break;
	   case AtTextAttr:
	     pAttrTrans->AtrTxtTRuleBlock = pBlock;
	     break;
	   case AtReferenceAttr:
	     pAttrTrans->AtrRefTRuleBlock = pBlock;
	     break;
	   case AtEnumAttr:
	     pAttrTrans->AtrEnuTRuleBlock[CurValAttr] = pBlock;
	     break;
	   default:
	     break;
	   }
       }
     else
       /* bloc de regles associe' a une presentation */
       {
	 pPresTrans = &pTSchema->TsPresTRule[CurPres - 1];
	 if (CurPres == PtBreak1 + 1 ||
	     CurPres == PtBreak2 + 1 ||
	     CurPres == PtIndent + 1 ||
	     CurPres == PtSize + 1 ||
	     CurPres == PtLineSpacing + 1 ||
	     CurPres == PtLineWeight + 1 ||
	     CurPres == PtMarginTop + 1 ||
	     CurPres == PtMarginRight + 1 ||
	     CurPres == PtMarginBottom + 1 ||
	     CurPres == PtMarginLeft + 1 ||
	     CurPres == PtPaddingTop + 1 ||
	     CurPres == PtPaddingRight + 1 ||
	     CurPres == PtPaddingBottom + 1 ||
	     CurPres == PtPaddingLeft + 1 ||
	     CurPres == PtBorderTopWidth + 1 ||
	     CurPres == PtBorderRightWidth + 1 ||
	     CurPres == PtBorderBottomWidth + 1 ||
	     CurPres == PtBorderLeftWidth + 1 ||
	     CurPres == PtXRadius + 1 ||
	     CurPres == PtYRadius + 1 ||
	     CurPres == PtTop + 1 ||
	     CurPres == PtRight + 1 ||
	     CurPres == PtBottom + 1 ||
	     CurPres == PtLeft + 1 ||
	     CurPres == PtVisibility + 1 ||
	     CurPres == PtListStyleImage + 1 ||
	     CurPres == PtDepth + 1 ||
	     CurPres == PtFillPattern + 1 ||
	     CurPres == PtBackground + 1 ||
	     CurPres == PtForeground + 1 ||
	     CurPres == PtBorderTopColor + 1 ||
	     CurPres == PtBorderRightColor + 1 ||
	     CurPres == PtBorderBottomColor + 1 ||
	     CurPres == PtBorderLeftColor + 1 ||
	     CurPres == PtOpacity + 1 ||
	     CurPres == PtFillOpacity + 1 ||
	     CurPres == PtStrokeOpacity + 1)
	   pPresTrans->RtCase[pPresTrans->RtNCase - 1].TaTRuleBlock = pBlock;
	 else
	   pPresTrans->RtPRuleValueBlock[CurPresVal] = pBlock;
       }
   else
     /* il y a un bloc courant, on chaine le nouveau bloc a celui-la */
     CurBlock->TbNextBlock = pBlock;
   /* le nouveau bloc de regle devient le bloc courant */
   CurBlock = pBlock;
   /* il n'y a plus de regle courante */
   CurTRule = NULL;
   /* initialise le nouveau bloc */
   CurBlock->TbNextBlock = NULL;
   CurBlock->TbFirstTRule = NULL;
   CurBlock->TbNConditions = 0;
}

/*----------------------------------------------------------------------
   cree une nouvelle regle, la chaine et l'initialise              
  ----------------------------------------------------------------------*/
static void         NewTransRule ()
{
   PtrTRule            pTRule;

   EndOfCondition (pSSchema);
   InCondition = False;
   GetTRule (&pTRule);
   if (pTRule == NULL)
     TtaDisplaySimpleMessage (FATAL, TRA, OUT_OF_MEMORY);
   /* cree une nouvelle regle */
   if (CurBlock == NULL || ChangeRuleBlock)
     /* cree un nouveau bloc de regle pour la nouvelle regle */
     NewRuleBlock ();
   /* chaine la nouvelle regle a la regle courante s'il y en a une, ou au */
   /* bloc de regle courant sinon */
   if (CurTRule == NULL)
     CurBlock->TbFirstTRule = pTRule;
   else
     CurTRule->TrNextTRule = pTRule;
   /* la nouvelle regle devient la regle courante */
   CurTRule = pTRule;
   /* initialise la regle */
   CurTRule->TrNextTRule = NULL;
   CurTRule->TrOrder = TBefore;
}

/*----------------------------------------------------------------------
   traite le type d'element indique' dans une regle Get.           
   Le nom du type se trouve dans la variable TypeInGetRule.        
  ----------------------------------------------------------------------*/
static void         ProcessTypeName (PtrSSchema pSS)
{
   int                 i;


   if (TypeInGetRule[0] != '\0')
     /* verifie si le type existe dans le schema de structure */
     {
       i = 1;
       while (strcmp (TypeInGetRule, pSS->SsRule->SrElem[i - 1]->SrName) != 0 &&
	      i < pSS->SsNRules)
	 i++;
       if (strcmp (TypeInGetRule, pSS->SsRule->SrElem[i - 1]->SrName) != 0)
	 /* type inconnu */
	 CompilerMessage (BeginTypeInGetRule, TRA, FATAL, BAD_TYPE, inputLine,
			  LineNum);
       else
	 /* le type existe, il a le numero i */
	 if (CurTRule->TrType == TCreate || CurTRule->TrType == TWrite)
	   {
	     CurTRule->TrObject = ToReferredElem;
	     CurTRule->TrObjectNum = i;
	   }
	 else
	   /* c'est une regle Get ou Copy */
	   CurTRule->TrElemType = i;
       TypeInGetRule[0] = '\0';
       /* le nom de type a ete traite' */
     }
}

/*----------------------------------------------------------------------
   copie le mot traite dans n                                      
  ----------------------------------------------------------------------*/
static void         CopyWord (Name n, indLine wi, indLine wl)
{
  if (wl > MAX_NAME_LENGTH - 1)
    CompilerMessage (wi, TRA, FATAL, MAX_NAME_SIZE_OVERFLOW, inputLine,
		      LineNum);
  else
    {
      strncpy ((char *)n, (char *)&inputLine[wi - 1], wl);
      n[wl] = '\0';
    }
}

/*----------------------------------------------------------------------
   cree une nouvelle constante                                     
  ----------------------------------------------------------------------*/
static void         NewConstant (indLine wl, indLine wi)
{
   int                 i;

   if (pTSchema->TsNConstants >= MAX_TRANSL_CONST)
     /* table des constantes saturee */
     CompilerMessage (wi, TRA, FATAL, MAX_POSSIBLE_CONSTANTS_OVERFLOW,
		      inputLine, LineNum);
   else
     /* alloue un nouvelle entree dans la table des const. */
     if (ConstIndx + wl > MAX_TRANSL_CONST_LEN)
       /* plus de place pour les constantes */
       CompilerMessage (wi, TRA, FATAL, MAX_CONSTANT_BUFFER_OVERFLOW,
			inputLine, LineNum);
     else
       /* FnCopy le texte de la constante */
       {
	 pTSchema->TsNConstants++;
	 pTSchema->TsConstBegin[pTSchema->TsNConstants - 1] = ConstIndx;
	 for (i = 0; i <= wl - 2; i++)
	   pTSchema->TsConstant[ConstIndx + i - 1] = inputLine[wi + i - 1];
	 ConstIndx += wl;
	 pTSchema->TsConstant[ConstIndx - 2] = '\0';
       }
}

/*----------------------------------------------------------------------
   cree un attribut dans une regle Create ou Write                 
  ----------------------------------------------------------------------*/
static void AttrInCreateOrWrite (int att, SyntRuleNum pr, indLine wi)
{
   PtrSRule            pSRule;
   TranslVariable     *pTransVar;
   ThotBool            ok;
   int                 j;

   /* on refuse les attributs reference */
   if (pSSchema->SsAttribute->TtAttr[att - 1]->AttrType == AtReferenceAttr)
     CompilerMessage (wi, TRA, FATAL, REF_ATTR_NOT_ALLOWED, inputLine,
		      LineNum);
   else if (pr == RULE_Token)
     {
       if (CurTRule->TrObject != ToTranslatedAttr)
	 CurTRule->TrObject = ToAttr;
       CurTRule->TrObjectNum = att;
     }
   else if (pr == RULE_CountFunction)
     /* un nom d'attribut dans une fonction de comptage, apres le mot-cle' */
     /* INIT. Seuls les attributs numeriques portant sur l'element racine  */
     /* du schema de structure sont autorises ici */
     if (pSSchema->SsAttribute->TtAttr[att - 1]->AttrType != AtNumAttr)
       /* ce n'est pas un attribut numerique, erreur */
       CompilerMessage (wi, TRA, FATAL, EXPECTING_A_NUMERICAL_ATTR, inputLine,
			LineNum);
     else
       /* cherche si l'attribut est un attribut local de la racine */
       {
	 ok = False;
	 pSRule = pSSchema->SsRule->SrElem[pSSchema->SsRootElem - 1];
	 j = 1;
	 while (j <= pSRule->SrNLocalAttrs && !ok)
	   if (pSRule->SrLocalAttr->Num[j - 1] == att)
	     ok = True;
	   else
	     j++;
	 if (!ok)
	   /* l'attribut ne porte pas sur la racine */
	   CompilerMessage (wi, TRA, FATAL, NOT_AN_ATTR_FOR_THE_ROOT_ELEM,
			    inputLine, LineNum);
	   else
	     /* le compteur courant prendra cet attribut comme valeur
		initiale */
	     pTSchema->TsCounter[pTSchema->TsNCounters - 1].TnAttrInit = att;
       }
   else if (pr == RULE_Function)
     /* un nom d'attribut dans une variable */
     {
       pTransVar = &pTSchema->TsVariable[pTSchema->TsNVariables - 1];
       if (pTransVar->TrvNItems >= MAX_TRANSL_VAR_ITEM)
	 /* variable trop longue */
	 CompilerMessage (wi, TRA, FATAL,
			  MAX_POSSIBLE_ELEMS_IN_VARIABLE_OVERFLOW,
			  inputLine, LineNum);
       else
	 {
	   pTransVar->TrvNItems++;
	   pTransVar->TrvItem[pTransVar->TrvNItems - 1].TvType = VtAttrVal;
	   pTransVar->TrvItem[pTransVar->TrvNItems - 1].TvItem = att;
	 }
     }
}


/*----------------------------------------------------------------------
   PatternNum   retourne le rang du nom n dans le fichier          
   des patterns de Thot.                                   
  ----------------------------------------------------------------------*/
static int PatternNum (Name n, indLine wi)
{
   unsigned int         i;
   ThotBool             ok;

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
       CompilerMessage (wi, TRA, FATAL, PATTERN_NOT_FOUND, inputLine,
			LineNum);
       i = 0;
     }
   return ((int)i);
}


/*----------------------------------------------------------------------
   ColorNum   retourne le rang du nom n dans le fichier            
   des couleurs de Thot.                                   
  ----------------------------------------------------------------------*/
static int          ColorNum (Name n, indLine wi)
{
   int                 i;

   i = 0;
   while (i < MAX_COLOR && strcmp (Name_colors[i], n))
     i++;
   if (i == MAX_COLOR)
     CompilerMessage (wi, TRA, FATAL, MISSING_COLOR, inputLine,
		      LineNum);
   return i;
}

/*----------------------------------------------------------------------
   ElementTypeNum
   si le mot de longueur wl qui commence a l'indice 
   wi du buffer de lecture est un nom de type d'element, retourne  
   le numero de type correspondant, sinon retourne 0.              
  ----------------------------------------------------------------------*/
static int          ElementTypeNum (indLine wi, indLine wl)
{
   Name                n;
   int                 i;

   CopyWord (n, wi, wl);
   /* verifie si le type existe dans le schema de structure */
   i = 1;
   while (strcmp (n, pSSchema->SsRule->SrElem[i - 1]->SrName) != 0 &&
	  i < pSSchema->SsNRules)
     i++;
   if (strcmp (n, pSSchema->SsRule->SrElem[i - 1]->SrName) != 0)
     /* type inconnu */
     {
       CompilerMessage (wi, TRA, FATAL, BAD_TYPE, inputLine, LineNum);
       i = 0;
     }
   return i;
}

/*----------------------------------------------------------------------
   AttributeNum
   si le mot de longueur wl qui commence a l'indice wi du buffer de
   lecture est un nom d'attribut, retourne le numero de cet attribut,
   sinon retourne 0.              
  ----------------------------------------------------------------------*/
static int          AttributeNum (indLine wi, indLine wl)
{
   Name                n;
   int                 i;

   /* copie dans n l'identificateur */
   CopyWord (n, wi, wl);
   /* verifie si l'attribut existe bien dans le schema de structure */
   i = 1;
   while (strcmp (n, pSSchema->SsAttribute->TtAttr[i - 1]->AttrName) != 0 &&
          i < pSSchema->SsNAttributes)
     i++;
   if (strcmp (n, pSSchema->SsAttribute->TtAttr[i - 1]->AttrName) != 0)
     /* attribut inconnu */
     i = 0;
   return i;
}

/*----------------------------------------------------------------------
   ElementTypeInCond
   si le mot de longueur wl qui commence a l'indice wi du buffer de
   lecture est un type d'element, traite ce type comme apparaissant
   dans une condition "if type"
  ----------------------------------------------------------------------*/
static ThotBool      ElementTypeInCond (indLine wi, indLine wl)
{
   int                 i;

   i = ElementTypeNum (wi, wl);
   if (i > 0)
     /* c'est bien un type d'element */
     {
     /* une condition "if type" ne peut s'appliquer qu'a une regle de */
     /* traduction d'attribnut */
     if (!InAttrRules)
       CompilerMessage (wi, TRA, FATAL, ONLY_FOR_ATTRS, inputLine, LineNum);
     else
       {
         CurBlock->TbCondition[CurBlock->TbNConditions - 1].TcCondition =
	   TcondElementType;
	 /* le type existe,il a le numero i*/
	 CurBlock->TbCondition[CurBlock->TbNConditions - 1].TcAttr = i;
       }
     }
   return (i > 0);
}

/*----------------------------------------------------------------------
   traite le mot commencant a la position wi dans la ligne courante
   de longueur wl et de code grammatical c. Si c'est un identif, nb
   contient son rang dans la table des identificateurs. r est le   
   numero de la regle dans laquelle apparait ce mot.               
  ----------------------------------------------------------------------*/
static void ProcessToken (indLine wi, indLine wl, SyntacticCode c, SyntacticCode r, int nb, SyntRuleNum pr)
{
   int                 i, k;
   Name                n;
   ThotBool            InUseRule, InGetRule, InCreateWriteRule;
   ScriptTransl     *pAlphTrans;
   TranslVariable     *pTransVar;
   TCounter           *pCntr;
   PtrAttributeTransl  pAttrTrans;
   PtrTtAttribute      pAttr;
   TranslNumAttrCase  *pCase;
   PRuleTransl        *pPresTrans;

   i = 1;
   if (c < 1000)
     /* symbole intermediaire de la grammaire, erreur */
     CompilerMessage (wi, TRA, FATAL, INTERMEDIATE_SYMBOL, inputLine, LineNum);
   else
     {
       if (c < 1100)		/* mot-cle court */
	 switch (c)
	   {
	   case CHR_59:
	     /*  ;  */
	     if (r == RULE_RuleA || r == RULE_RuleB)	/* fin d'une regle */
	       {
		 if (CurTRule->TrType == TUse)
		   if (CurType == pSSchema->SsRootElem)
		     if (CurTRule->TrNature ==
			 pSSchema->SsRule->SrElem[CurType - 1]->SrName)
		       /* une regle Use pour la racine se termine sans 'For' */
		       CompilerMessage (wi, TRA, FATAL, FOR_PART_MISSING,
					inputLine, LineNum);
		 if (CurTRule->TrType == TGet ||
		     CurTRule->TrType == TCopy ||
		     CurTRule->TrType == TCreate ||
		     CurTRule->TrType == TWrite)
		   /* fin d'une regle Get, Copy, Create ou Write */
		   ProcessTypeName (pSSchema);
		 if (!InAttrRules)
		   if (CurBlock != NULL)
		     if (CurBlock->TbNConditions != 0)
		       if (!InCondBlock)
			 /* fin d'une regle unique dans bloc conditionnel, il/
			 faut ouvrir un nouveau bloc pour la regle suivante */
			 ChangeRuleBlock = True;
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
	       if (!VarDefinition)
		 /* un nom de schema externe va suivre */
		 if (CurTRule->TrObject != ToReferredElem)
		   CompilerMessage (wi, TRA, FATAL, ONLY_FOR_ELEMS, inputLine,
				    LineNum);
		 else
		   ExternalSchema = True;
	       else
		 /* debut d'une definition de variable */
		 if (pTSchema->TsNVariables >= MAX_TRANSL_VARIABLE)
		   /* table des variables saturee */
		   CompilerMessage (wi, TRA, FATAL,
				    MAX_POSSIBLE_VARIABLES_OVERFLOW,
				    inputLine, LineNum);
		 else
		   {
		     VarDef = True;
		     pTSchema->TsVariable[pTSchema->TsNVariables++].TrvNItems =
		       0;
		     /* la regle courante utilise cette variable */
		     CurTRule->TrObject = ToVariable;
		     CurTRule->TrObjectNum = pTSchema->TsNVariables;
		   }
	     else if (r == RULE_RuleA)
	       ExternalSchema = True;
	     else if (r == RULE_CondOnAscend || r == RULE_LevelOrType)
	       /* On est dans une condition. Un nom de schema externe suit */
	       ExternalSchema = True;
	     break;
	     
	   case CHR_41:
	     /*  )  */
	     if (r == RULE_Token)
	       /* fin d'une definition de variable */
	       VarDef = False;
	     break;
	     
	   case CHR_44:
	     /*  ,  */
	     break;

	   case CHR_62:
	     /*  >  */
	     if (r == RULE_GreaterLess)
	       {
		 CurBlock->TbCondition[CurBlock->TbNConditions - 1].
		   TcAscendRel = RelGreater;
		 GreaterOrLessSign = True;
	       }
	     break;
	     
	   case CHR_43:
	     /*  +  */
	     if (r == RULE_IndentSign)
	       /* devant la valeur de l'indentation relative */
	       IndentSign = 1;
	     break;
	     
	   case CHR_45:
	     /*  -  */
	     if (r == RULE_AttrRelat || r == RULE_AttrValue)
	       /* devant une valeur d'attribut numerique */
	       AttrValSign = -1;
	     else if (r == RULE_PresRelat || r == RULE_PresValue)
	       /* devant une valeur numerique de presentation */
	       PresValSign = -1;
	     else if (r == RULE_RelAncestorLevel)
	       /* devant le niveau relatif d'un ancetre dans un
		  compteur CntrRank */
	       AncestorSign = -1;
	     else if (r == RULE_IndentSign)
	       /* devant la valeur de l'indentation relative */
	       IndentSign = -1;
	     break;
	     
	   case CHR_60:
	     /*  <  */
	     if (r == RULE_GreaterLess)
	       {
		 CurBlock->TbCondition[CurBlock->TbNConditions - 1].
		   TcAscendRel = RelLess;
		 GreaterOrLessSign = True;
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
	     BuffDef = True;	/* on est dans les declarations de buffers */
	     break;
	     
	   case KWD_Counters:	/* Counters */
	     BuffDef = False;	/* fin des declarations de buffers */
	     ConstDef = False;	/* fin des declaration de constantes */
	     ComptDef = True;	/* on est dans les declarations de compteur */
	     break;
	     
	   case KWD_Const:	/* Const */
	     BuffDef = False;	/* fin des declarations de buffers */
	     ConstDef = True;	/* on est dans les declaration de constante */
	     break;
	     
	   case KWD_Var:	/* Var */
	     BuffDef = False;	/* fin des declarations de buffers */
	     ConstDef = False;	/* fin des declaration de constantes */
	     ComptDef = False;	/* fin des declarations de compteurs */
	     VarDef = True;	/* on est dans les declarations de variables */
	     break;

	   case KWD_Rules:	/* Rules */
	     BuffDef = False;	/* fin des declarations de buffers */
	     ConstDef = False;	/* fin des declaration de constantes */
	     ComptDef = False;	/* fin des declarations de compteurs */
	     VarDef = False;	/* fin des declarations de variables */
	     InTypeRules = True;
	     break;
	     
	   case KWD_Attributes:	/* Attributes */
	     if (r == RULE_TransSchema)
	       /* debut des regles de traduction des attributs */
	       {
		 BuffDef = False;	/* fin des declarations de buffers */
		 ConstDef = False;	/* fin des declaration de constantes */
		 ComptDef = False;	/* fin des declarations de compteurs */
		 VarDef = False;	/* fin des declarations de variables */
		 InTypeRules = False;
		 InAttrRules = True;
	       }
	     else if (r == RULE_CondOnAscend)
	       /* dans une condition */
	       {
		 ProcessAncestorName (pSSchema);
		 CurBlock->TbCondition[CurBlock->TbNConditions - 1].TcCondition
		   = TcondAttributes;
	       }
	     else if (r == RULE_Token)
	       /* ce qu'il faut generer : les  attributs de l'element */
	       CurTRule->TrObject = ToAllAttr;
	     break;
	     
	   case KWD_TextTranslate:	/* TextTranslate */
	     BuffDef = False;	/* fin des declarations de buffers */
	     ConstDef = False;	/* fin des declaration de constantes */
	     ComptDef = False;	/* fin des declarations de compteurs */
	     VarDef = False;	/* fin des declarations de variables */
	     InTypeRules = False;
	     InAttrRules = False;
	     InPresRules = False;
	     TextTrans = True;
	     /* on est dans les traductions de texte */
	     if (pTSchema->TsNTranslScripts >= MAX_TRANSL_ALPHABET)
	       CompilerMessage (wi, TRA, FATAL,
				MAX_POSSIBLE_TEXTTRANSLATE_OVERFLOW,
				inputLine, LineNum);
	     else
	       {
		 pAlphTrans = &pTSchema->TsTranslScript[pTSchema->
							 TsNTranslScripts++];
		 pAlphTrans->AlScript = 'L';	/* script latin par defaut */
		 if (pTSchema->TsNTranslScripts == 1)
		   {
		     pAlphTrans->AlBegin = 1;
		     pAlphTrans->AlEnd = 0;
		   }
		 else
		   {
		     pAlphTrans->AlBegin =
		       pTSchema->TsTranslScript[pTSchema->TsNTranslScripts - 2].AlEnd + 1;
		     pAlphTrans->AlEnd =
		       pTSchema->TsTranslScript[pTSchema->TsNTranslScripts - 2].AlEnd;
		   }
	       }
	     break;
	     
	   case KWD_SymbTranslate:	/* SymbTranslate */
	     BuffDef = False;	/* fin des declarations de buffers */
	     ConstDef = False;	/* fin des declaration de constantes */
	     ComptDef = False;	/* fin des declarations de compteurs */
	     VarDef = False;	/* fin des declarations de variables */
	     InTypeRules = False;
	     InAttrRules = False;
	     InPresRules = False;
	     TextTrans = False;	/* fin des traductions de texte */
	     SymbTrans = True;	/* on est dans les traductions de symbole */
	     if (pTSchema->TsNTranslScripts == 0)
	       {
		 pTSchema->TsSymbolFirst = 1;
		 pTSchema->TsSymbolLast = 0;
	       }
	     else
	       {
		 pAlphTrans = &pTSchema->TsTranslScript[pTSchema->
						      TsNTranslScripts - 1];
		 if (pAlphTrans->AlEnd > 0)
		   {
		     pTSchema->TsSymbolFirst = pAlphTrans->AlEnd + 1;
		     pTSchema->TsSymbolLast = pAlphTrans->AlEnd;
		   }
		 else
		   {
		     pTSchema->TsSymbolFirst = 1;
		     pTSchema->TsSymbolLast = 0;
		   }
	       }
	     break;
	     
	   case KWD_GraphTranslate:	/* GraphTranslate */
	     BuffDef = False;	/* fin des declarations de buffers */
	     ConstDef = False;	/* fin des declaration de constantes */
	     ComptDef = False;	/* fin des declarations de compteurs */
	     VarDef = False;	/* fin des declarations de variables */
	     InTypeRules = False;
	     InAttrRules = False;
	     InPresRules = False;
	     TextTrans = False;	/* fin des traductions de texte */
	     SymbTrans = False;	/* fin des traductions de symbole */
	     GraphTrans = True;	/* on est dans les traduc. de graphique */
	     if (pTSchema->TsSymbolLast > 0)
	       {
		 pTSchema->TsGraphicsFirst = pTSchema->TsSymbolLast + 1;
		 pTSchema->TsGraphicsLast = pTSchema->TsSymbolLast;
	       }
	     else if (pTSchema->TsNTranslScripts == 0)
	       {
		 pTSchema->TsGraphicsFirst = 1;
		 pTSchema->TsGraphicsLast = 0;
	       }
	     else
	       {
		 pAlphTrans = &pTSchema->TsTranslScript[pTSchema->
						      TsNTranslScripts - 1];
		 if (pAlphTrans->AlEnd > 0)
		   {
		     pTSchema->TsGraphicsFirst = pAlphTrans->AlEnd + 1;
		     pTSchema->TsGraphicsLast = pAlphTrans->AlEnd;
		   }
		 else
		   {
		     pTSchema->TsGraphicsFirst = 1;
		     pTSchema->TsGraphicsLast = 0;
		   }
	       }
	     break;
	     
	   case KWD_End:	/* End */
	     if (r == RULE_SeqOfRules1 && !InAttrRules)
	       {
		 InCondBlock = False;
		 if (CurBlock != NULL)
		   if (CurBlock->TbNConditions != 0)
		     /* fin d'un bloc conditionnel, il faut ouvrir un */
		     /* nouveau bloc pour la regle suivante */
		     ChangeRuleBlock = True;
	       }
	     break;
	     
	   case KWD_Rank:	/* CntrRank */
	     pTSchema->TsCounter[pTSchema->TsNCounters - 1].TnOperation =
	       TCntrRank;
	     pTSchema->TsCounter[pTSchema->TsNCounters - 1].TnAcestorLevel = 0;
	     AncestorSign = 1;
	     break;
	     
	   case KWD_of:	/* Of */
	     break;	/* rien */
	     
	   case KWD_Rlevel:	/* CntrRLevel */
	     pTSchema->TsCounter[pTSchema->TsNCounters - 1].TnOperation =
	       TCntrRLevel;
	     break;

	   case KWD_Set:	/* Set */
	     if (r == RULE_CountFunction)
	       /* dans une definition de compteur */
	       {
		 pCntr = &pTSchema->TsCounter[pTSchema->TsNCounters - 1];
		 pCntr->TnOperation = TCntrSet;
		 pCntr->TnElemType1 = 0;
	       }
	     else if (r == RULE_RuleB)
	       /* instruction de traduction Set */
	       {
		 NewTransRule ();
		 CurTRule->TrType = TSetCounter;
	       }
	     break;
	     
	   case KWD_On:	/* On */
	     break;	/* rien */
		       
	   case KWD_Add:	/* Add */
	     if (r == RULE_RuleB)
	       /* instruction de traduction Add */
	       {
		 NewTransRule ();
		 CurTRule->TrType = TAddCounter;
	       }
	     break;
	     
	   case KWD_Picture:	/* Picture */
	     if (pTSchema->TsPictureBuffer != 0)
	       /* un seul buffer image autorise' */
	       CompilerMessage (wi, TRA, FATAL,
				CANT_USE_MORE_THAN_ONE_PICTURE_BUFFER,
				inputLine, LineNum);
	     else	/* c'est le buffer courant qui est le buffer image */
	       pTSchema->TsPictureBuffer = pTSchema->TsNBuffers;
	     break;

	   case KWD_Variable:  /* Variable */
	     pTSchema->TsVarBuffer[pTSchema->TsNVarBuffers].VbNum =
	                                                 pTSchema->TsNBuffers;
	     pTSchema->TsNVarBuffers++;
	     break;
	     
	   case KWD_Value:	/* Value */
	     if (r == RULE_Token)
	       if (!InPresRules)
		 CompilerMessage (wi, TRA, FATAL, ONLY_IN_PRES_PART,
				  inputLine, LineNum);
	     /* seulement dans les regles de la presentation */
	       else
		 CurTRule->TrObject = ToPRuleValue;
	     else if (r == RULE_Function)
	       /* valeur d'un compteur dans une definition de variable */
	       {
		 pTransVar = &pTSchema->TsVariable[pTSchema->TsNVariables - 1];
		 if (pTransVar->TrvNItems >= MAX_TRANSL_VAR_ITEM)
		   /* trop de fonctions */
		   CompilerMessage (wi, TRA, FATAL,
				    MAX_POSSIBLE_ELEMS_IN_VARIABLE_OVERFLOW,
				    inputLine, LineNum);
		 else
		   pTransVar->TrvItem[pTransVar->TrvNItems++].TvType =
		     VtCounter;
	       }
	     break;
	     
	   case KWD_FileDir:	/* FileDir */
	     if (r == RULE_Function)
	       {
		 pTransVar = &pTSchema->TsVariable[pTSchema->TsNVariables - 1];
		 if (pTransVar->TrvNItems >= MAX_TRANSL_VAR_ITEM)
		   /* trop de fonctions */
		   CompilerMessage (wi, TRA, FATAL,
				    MAX_POSSIBLE_ELEMS_IN_VARIABLE_OVERFLOW,
				    inputLine, LineNum);
		 else
		   pTransVar->TrvItem[pTransVar->TrvNItems++].TvType =
		     VtFileDir;
	       }
	     else if (r == RULE_Token)
	       /* generer le nom du directory du fichier de sortie */
	       CurTRule->TrObject = ToFileDir;
	     break;
	     
	   case KWD_FileName:
	     if (r == RULE_Function)
	       {
		 pTransVar = &pTSchema->TsVariable[pTSchema->TsNVariables - 1];
		 if (pTransVar->TrvNItems >= MAX_TRANSL_VAR_ITEM)
		   /* trop de fonctions */
		   CompilerMessage (wi, TRA, FATAL,
				    MAX_POSSIBLE_ELEMS_IN_VARIABLE_OVERFLOW,
				    inputLine, LineNum);
		 else
		   pTransVar->TrvItem[pTransVar->TrvNItems++].TvType =
		     VtFileName;
	       }
	     else if (r == RULE_Token)
	       /* generer le nom du fichier de sortie */
	       CurTRule->TrObject = ToFileName;
	     break;
	     
	   case KWD_Extension:
	     if (r == RULE_Function)
	       {
		 pTransVar = &pTSchema->TsVariable[pTSchema->TsNVariables - 1];
		 if (pTransVar->TrvNItems >= MAX_TRANSL_VAR_ITEM)
		   /* trop de fonctions */
		   CompilerMessage (wi, TRA, FATAL,
				    MAX_POSSIBLE_ELEMS_IN_VARIABLE_OVERFLOW,
				    inputLine, LineNum);
		 else
		   pTransVar->TrvItem[pTransVar->TrvNItems++].TvType =
		     VtExtension;
	       }
	     else if (r == RULE_Token)
	       /* generer l'extensiion (suffixe) du fichier de sortie */
	       CurTRule->TrObject = ToExtension;
	     break;
	     
	   case KWD_RefId:	/* RefId */
	     if (r == RULE_Token)
	       /* generer le label de l'element traduit */
	       CurTRule->TrObject = ToRefId;
	     else if (r == RULE_VarOrType)
	       /* generer le label de l'element reference' */
	       {
	       if ((InTypeRules &&
		    pSSchema->SsRule->SrElem[CurType - 1]->SrConstruct != CsReference)||
		   (InAttrRules &&
		    pSSchema->SsAttribute->TtAttr[CurAttr - 1]->AttrType !=
		    AtReferenceAttr))
		 /* l'element ou l'attribut auquel s'applique la regle */
		 /* n'est pas une reference, erreur */
		 CompilerMessage (wi, TRA, FATAL, VALID_ONLY_FOR_REFS,
				  inputLine, LineNum);
	       else
		 CurTRule->TrObject = ToReferredRefId;
	       }
	     break;

	   case KWD_PairId:	/* PairId */
	     if (pSSchema->SsRule->SrElem[CurType - 1]->SrConstruct != CsPairedElement)
	       /* l'element auquel s'applique la regle n'est pas une paire */
	       CompilerMessage (wi, TRA, FATAL, NOT_A_PAIR,inputLine, LineNum);
	     else
	       CurTRule->TrObject = ToPairId;
	     break;
	     
	   case KWD_IN:	/* IN */
	     break;

	   case KWD_Begin:	/* Begin */
	     if (r == RULE_SeqOfRules1)
	       {
		 EndOfCondition (pSSchema);
		 if (!InAttrRules)
		   InCondBlock = True;
	       }
	     break;
	     
	   case KWD_If:	/* If */
	     InCondition = True;
	     NewRuleBlock ();
	     CurBlock->TbNConditions = 1;
	     CurBlock->TbCondition[0].TcNegativeCond = False;
	     CurBlock->TbCondition[0].TcTarget = False;
	     CurBlock->TbCondition[0].TcAscendType = 0;
	     CurBlock->TbCondition[0].TcAscendNature[0] = '\0';
	     CurBlock->TbCondition[0].TcAscendRelLevel = 0;
	     CurBlock->TbCondition[0].TcCondition = TcondElementType;
	     Immediately = False;
	     Asterisk = False;
	     break;
	     
	   case KWD_And:	/* And */
	     if (CurBlock->TbNConditions >= MAX_TRANSL_COND)
	       /* trop de conditions */
	       CompilerMessage (wi, TRA, FATAL, TOO_MANY_CONDITIONS,
				inputLine, LineNum);
	     else
	       {
		 EndOfCondition (pSSchema);
		 i = CurBlock->TbNConditions;
		 CurBlock->TbNConditions++;
		 CurBlock->TbCondition[i].TcNegativeCond = False;
		 CurBlock->TbCondition[i].TcTarget = False;
		 CurBlock->TbCondition[i].TcAscendType = 0;
		 CurBlock->TbCondition[i].TcAscendNature[0] = '\0';
		 CurBlock->TbCondition[i].TcAscendRelLevel = 0;
		 CurBlock->TbCondition[i].TcCondition = TcondElementType;
		 Immediately = False;
		 Asterisk = False;
	       }
	     break;
	     
	   case KWD_Not:	/* Not */
	     CurBlock->TbCondition[CurBlock->TbNConditions - 1].
	       TcNegativeCond = True;
	     break;

	   case KWD_Target:	/* Target */
	     if ((InTypeRules &&
		  pSSchema->SsRule->SrElem[CurType - 1]->SrConstruct != CsReference) ||
		 (InAttrRules &&
		  pSSchema->SsAttribute->TtAttr[CurAttr - 1]->AttrType !=
		  AtReferenceAttr))
	       /* l'element ou l'attribut auquel s'applique la regle */
	       /* n'est pas une reference, erreur */
	       CompilerMessage (wi, TRA, FATAL, VALID_ONLY_FOR_REFS,
				inputLine, LineNum);
	     else
	       CurBlock->TbCondition[CurBlock->TbNConditions - 1].TcTarget =
		 True;
	     break;

	   case KWD_First:	/* First */
	     if (r == RULE_FirstSec)
	       FirstInPair = True;
	     else
	       {
		 ProcessAncestorName (pSSchema);
		 CurBlock->TbCondition[CurBlock->TbNConditions - 1].
		   TcCondition = TcondFirst;
	       }
	     break;
	     
	   case KWD_Second:	/* Second */
	     SecondInPair = True;
	     break;
	   case KWD_Last:	/* Last */
	     ProcessAncestorName (pSSchema);
	     CurBlock->TbCondition[CurBlock->TbNConditions - 1].TcCondition =
	       TcondLast;
	     break;
	   case KWD_Refered:	/* Refered */
	   case KWD_Referred:		/* Referred */
	     if (r == RULE_CondOnAscend)	/* dans une condition */
	       {
		 ProcessAncestorName (pSSchema);
		 CurBlock->TbCondition[CurBlock->TbNConditions - 1].
		   TcCondition = TcondReferred;
		 if (!Asterisk)
		   CurBlock->TbCondition[CurBlock->TbNConditions - 1].
		     TcAscendRelLevel = -1;
	       }
	     else if ((InTypeRules &&
		       pSSchema->SsRule->SrElem[CurType - 1]->SrConstruct !=
		       CsReference) ||
		      (InAttrRules &&
		       pSSchema->SsAttribute->TtAttr[CurAttr - 1]->AttrType !=
		       AtReferenceAttr))
	       /* l'element ou l'attribut auquel s'applique la regle n'est */
	       /* pas une reference, erreur */
	       CompilerMessage (wi, TRA, FATAL, VALID_ONLY_FOR_REFS,
				inputLine, LineNum);
	     else if (r == RULE_Token)
	       /* devant un identificateur de variable */
	       CurTRule->TrReferredObj = True;
	     else if (r == RULE_RelPosition)
	       /* dans une position relative */
	       CurTRule->TrRelPosition = RpReferred;
	     break;
	     
	   case KWD_FirstRef:		/* FirstRef */
	     if ((InTypeRules &&
		  pSSchema->SsRule->SrElem[CurType - 1]->SrConstruct != CsReference) ||
		 (InAttrRules &&
		  pSSchema->SsAttribute->TtAttr[CurAttr - 1]->AttrType !=
		  AtReferenceAttr))
	       /* l'element ou l'attribut auquel s'applique la regle n'est */
	       /* pas une reference, erreur */
	       CompilerMessage (wi, TRA, FATAL, VALID_ONLY_FOR_REFS,
				inputLine, LineNum);
	     else
	       CurBlock->TbCondition[CurBlock->TbNConditions - 1].TcCondition =
		 TcondFirstRef;
	     break;

	   case KWD_LastRef:	/* LastRef */
	     if ((InTypeRules &&
		  pSSchema->SsRule->SrElem[CurType - 1]->SrConstruct != CsReference) ||
		 (InAttrRules &&
		  pSSchema->SsAttribute->TtAttr[CurAttr - 1]->AttrType !=
		  AtReferenceAttr))
	       /* l'element ou l'attribut auquel s'applique la regle n'est */
	       /* pas une reference, erreur */
	       CompilerMessage (wi, TRA, FATAL, VALID_ONLY_FOR_REFS,
				inputLine, LineNum);
	     else
	       CurBlock->TbCondition[CurBlock->TbNConditions - 1].TcCondition =
		 TcondLastRef;
	     break;
	     
	   case KWD_ExternalRef:	/* ExternalRef */
	     if ((InTypeRules &&
		  pSSchema->SsRule->SrElem[CurType - 1]->SrConstruct != CsReference) ||
		 (InAttrRules &&
		  pSSchema->SsAttribute->TtAttr[CurAttr - 1]->AttrType !=
		  AtReferenceAttr))
	       /* l'element ou l'attribut auquel s'applique la regle n'est */
	       /* pas une reference, erreur */
	       CompilerMessage (wi, TRA, FATAL, VALID_ONLY_FOR_REFS,
				inputLine, LineNum);
	     else
	       CurBlock->TbCondition[CurBlock->TbNConditions - 1].TcCondition =
		 TcondExternalRef;
	     break;

	   case KWD_Immediately:
	     Immediately = True;
	     break;
	     
	   case KWD_Within:	/* Within */
	     ProcessAncestorName (pSSchema);
	     CurBlock->TbCondition[CurBlock->TbNConditions - 1].TcCondition =
	       TcondWithin;
	     CurBlock->TbCondition[CurBlock->TbNConditions - 1].
	       TcImmediatelyWithin = Immediately;
	     Immediately = False;
	     CurBlock->TbCondition[CurBlock->TbNConditions - 1].TcAscendRel =
	       RelGreater;
	     CurBlock->TbCondition[CurBlock->TbNConditions - 1].TcAscendLevel =
	       0;
	     GreaterOrLessSign = False;
	     CurBlock->TbCondition[CurBlock->TbNConditions - 1].
	       TcElemNature[0] = '\0';
	     TypeWithin[0] = '\0';
	     break;

	   case KWD_FirstWithin:	/* FirstWithin */
	     ProcessAncestorName (pSSchema);
	     CurBlock->TbCondition[CurBlock->TbNConditions - 1].TcCondition =
	       TcondFirstWithin;
	     CurBlock->TbCondition[CurBlock->TbNConditions - 1].
	       TcImmediatelyWithin = False;
	     Immediately = False;
	     CurBlock->TbCondition[CurBlock->TbNConditions - 1].TcAscendRel =
	       RelGreater;
	     CurBlock->TbCondition[CurBlock->TbNConditions - 1].TcAscendLevel =
	       0;
	     GreaterOrLessSign = False;
	     CurBlock->TbCondition[CurBlock->TbNConditions - 1].
	       TcElemNature[0] = '\0';
	     TypeWithin[0] = '\0';
	     break;

	   case KWD_Presentation:	/* Presentation */
	     if (InPresRules)
	       CompilerMessage (wi, TRA, FATAL, FORBIDDEN_IN_PRES_PART,
				inputLine, LineNum);
	     else if (r == RULE_TransSchema)
	       /* debut des regles de traduction de la presentation */
	       {
		 BuffDef = False;	/* fin des declarations de buffers */
		 ConstDef = False;	/* fin des declaration de constantes */
		 ComptDef = False;	/* fin des declarations de compteurs */
		 VarDef = False;	/* fin des declarations de variables */
		 InTypeRules = False;
		 InAttrRules = False;
		 InPresRules = True;
	       }
	     else if (r == RULE_CondOnAscend)
	       /* dans une condition */
	       {
		 ProcessAncestorName (pSSchema);
		 CurBlock->TbCondition[CurBlock->TbNConditions - 1].
		   TcCondition = TcondPresentation;
	       }
	     else if (r == RULE_Token)
	       /* ce qu'il faut generer */
	       CurTRule->TrObject = ToAllPRules;
	     break;
	     
	   case KWD_DocumentName:	/* DocumentName */
	     if (r == RULE_Function)
	       {
		 pTransVar = &pTSchema->TsVariable[pTSchema->TsNVariables - 1];
		 if (pTransVar->TrvNItems >= MAX_TRANSL_VAR_ITEM)
		   /* trop de fonctions */
		   CompilerMessage (wi, TRA, FATAL,
				    MAX_POSSIBLE_ELEMS_IN_VARIABLE_OVERFLOW,
				    inputLine, LineNum);
		 else
		   pTransVar->TrvItem[pTransVar->TrvNItems++].TvType =
		     VtDocumentName;
	       }
	     else if (r == RULE_Token)
	       /* generer le nom du document traduit */
	       CurTRule->TrObject = ToDocumentName;
	     else if (r == RULE_VarOrType)
	       /* generer le nom du document reference' */
	       {
	       if ((InTypeRules &&
		    pSSchema->SsRule->SrElem[CurType - 1]->SrConstruct != CsReference)||
		   (InAttrRules &&
		    pSSchema->SsAttribute->TtAttr[CurAttr - 1]->AttrType !=
		    AtReferenceAttr))
		 /* l'element ou l'attribut auquel s'applique la regle */
		 /* n'est pas une reference, erreur */
		 CompilerMessage (wi, TRA, FATAL, VALID_ONLY_FOR_REFS,
				  inputLine, LineNum);
	       else
		 CurTRule->TrObject = ToReferredDocumentName;
	       }
	     break;
	     
	   case KWD_DocumentDir:	/* DocumentDir */
	     if (r == RULE_Function)
	       {
		 pTransVar = &pTSchema->TsVariable[pTSchema->TsNVariables - 1];
		 if (pTransVar->TrvNItems >= MAX_TRANSL_VAR_ITEM)
		   /* trop de fonctions */
		   CompilerMessage (wi, TRA, FATAL, 
				    MAX_POSSIBLE_ELEMS_IN_VARIABLE_OVERFLOW,
				    inputLine, LineNum);
		 else
		   pTransVar->TrvItem[pTransVar->TrvNItems++].TvType =
		     VtDocumentDir;
	       }
	     else if (r == RULE_Token)
	       /* generer le nom du directory du document traduit */
	       CurTRule->TrObject = ToDocumentDir;
	     else if (r == RULE_VarOrType)
	       /* generer le nom du directory du document reference' */
	       {
	       if ((InTypeRules &&
		    pSSchema->SsRule->SrElem[CurType - 1]->SrConstruct != CsReference)||
		   (InAttrRules &&
		    pSSchema->SsAttribute->TtAttr[CurAttr - 1]->AttrType !=
		    AtReferenceAttr))
		 /* l'element ou l'attribut auquel s'applique la regle */
		 /* n'est pas une reference, erreur */
		 CompilerMessage (wi, TRA, FATAL, VALID_ONLY_FOR_REFS,
				  inputLine, LineNum);
	       else
		 CurTRule->TrObject = ToReferredDocumentDir;
	       }
	     break;

	   case KWD_ARABIC:
	     pTransVar = &pTSchema->TsVariable[pTSchema->TsNVariables - 1];
	     pTransVar->TrvItem[pTransVar->TrvNItems - 1].TvCounterStyle =
	       CntDecimal;
	     break;
	     
	   case KWD_UROMAN:
	     pTransVar = &pTSchema->TsVariable[pTSchema->TsNVariables - 1];
	     pTransVar->TrvItem[pTransVar->TrvNItems - 1].TvCounterStyle =
	       CntURoman;
	     break;

	   case KWD_LROMAN:
	     pTransVar = &pTSchema->TsVariable[pTSchema->TsNVariables - 1];
	     pTransVar->TrvItem[pTransVar->TrvNItems - 1].TvCounterStyle =
	       CntLRoman;
	     break;

	   case KWD_UPPERCASE:
	     pTransVar = &pTSchema->TsVariable[pTSchema->TsNVariables - 1];
	     pTransVar->TrvItem[pTransVar->TrvNItems - 1].TvCounterStyle =
	       CntUppercase;
	     break;

	   case KWD_LOWERCASE:
	     pTransVar = &pTSchema->TsVariable[pTSchema->TsNVariables - 1];
	     pTransVar->TrvItem[pTransVar->TrvNItems - 1].TvCounterStyle =
	       CntLowercase;
	     break;

	   case KWD_Script:		/* Script */
	     if (CurType != CharString + 1)
	       CompilerMessage (wi, TRA, FATAL, VALID_ONLY_FOR_TEXT_UNITS,
				inputLine, LineNum);
	     else
	       CurBlock->TbCondition[CurBlock->TbNConditions - 1].TcCondition =
		 TcondScript;
	     break;

	   case KWD_FirstAttr:	/* FirstAttr */
	     CurBlock->TbCondition[CurBlock->TbNConditions - 1].TcCondition =
	       TcondFirstAttr;
	     break;

	   case KWD_LastAttr:		/* LastAttr */
	     CurBlock->TbCondition[CurBlock->TbNConditions - 1].TcCondition =
	       TcondLastAttr;
	     break;

	   case KWD_Create:	/* Create */
	     NewTransRule ();
	     CurTRule->TrType = TCreate;
	     CurTRule->TrReferredObj = False;
	     CurTRule->TrObject = ToConst;
	     CurTRule->TrObjectNature[0] = '\0';
	     CurTRule->TrFileNameVar = 0;
	     VarDefinition = True;
	     break;

	   case KWD_Get:	/* Get */
	     NewTransRule ();
	     CurTRule->TrType = TGet;
	     CurTRule->TrElemNature[0] = '\0';
	     CurTRule->TrRelPosition = RpSibling;
	     break;
	     
	   case KWD_Copy:	/* Copy */
	     NewTransRule ();
	     CurTRule->TrType = TCopy;
	     CurTRule->TrElemNature[0] = '\0';
	     CurTRule->TrRelPosition = RpSibling;
	     break;

	   case KWD_Use:	/* Use */
	     InCondition = False;
	     if (pSSchema->SsRule->SrElem[CurType - 1]->SrConstruct != CsNatureSchema
		 && CurType != pSSchema->SsRootElem)
	       /* l'element auquel s'applique la regle n'est pas une */
	       /* SyntacticType ni la regle racine, erreur */
	       CompilerMessage (wi, TRA, FATAL,
				VALID_ONLY_FOR_A_DIFFERENT_STRUCT_SCHEM,
				inputLine, LineNum);
	     else
	       {
		 NewTransRule ();
		 if (CurBlock->TbNConditions != 0)
		   CompilerMessage (wi, TRA, FATAL, MUST_BE_UNCONDITIONAL,
				    inputLine, LineNum);
		 else
		   {
		     CurTRule->TrType = TUse;
		     strncpy (CurTRule->TrNature,
			       pSSchema->SsRule->SrElem[CurType - 1]->SrName,
			       MAX_NAME_LENGTH);
		   }
	       }
	     break;

	   case KWD_For:	/* For */
	     if (CurType != pSSchema->SsRootElem)
	       /* FOR n'est acceptable que pour l'element racine */
	       CompilerMessage (wi, TRA, FATAL, VALID_ONLY_FOR_THE_ROOT_ELEM,
				inputLine, LineNum);
	     break;

	   case KWD_Remove:	/* Remove */
	     NewTransRule ();
	     CurTRule->TrType = TRemove;
	     break;

	   case KWD_Ignore:	/* Ignore */
	     NewTransRule ();
	     CurTRule->TrType = TIgnore;
	     break;

	   case KWD_Write:	/* Write */
	     NewTransRule ();
	     CurTRule->TrType = TWrite;
	     CurTRule->TrReferredObj = False;
	     CurTRule->TrObject = ToConst;
	     CurTRule->TrObjectNature[0] = '\0';
	     CurTRule->TrFileNameVar = 0;
	     VarDefinition = True;
	     break;

	   case KWD_Read:	/* Read */
	     NewTransRule ();
	     CurTRule->TrType = TRead;
	     break;

	   case KWD_Include:	/* Include */
	     NewTransRule ();
	     CurTRule->TrType = TInclude;
	     break;
	     
	   case KWD_NoTranslation:	/* NoTranslation */
	     NewTransRule ();
	     CurTRule->TrType = TNoTranslation;
	     break;

	   case KWD_NoLineBreak:	/* NoLineBreak */
	     NewTransRule ();
	     CurTRule->TrType = TNoLineBreak;
	     break;

	   case KWD_ChangeMainFile:	/* ChangeMainFile */
	     NewTransRule ();
	     CurTRule->TrType = TChangeMainFile;
	     break;

	   case KWD_Translated:	/* Translated */
	     CurTRule->TrObject = ToTranslatedAttr;
	     break;

	   case KWD_Content:	/* Content */
	     CurTRule->TrObject = ToContent;
	     break;

	   case KWD_After:	/* After */
	     CurTRule->TrOrder = TAfter;
	     break;

	   case KWD_Before:	/* Before */
	     CurTRule->TrOrder = TBefore;
	     break;

	   case KWD_Included:	/* Included */
	     CurTRule->TrRelPosition = RpDescend;
	     break;

	   case KWD_Size:
	     PresentationName (PtSize, pr, wi);
	     break;

	   case KWD_Indent:
	     if (r == RULE_PresRule)
	       PresentationName (PtIndent, pr, wi);
	     else if (r == RULE_RuleB)
	       /* instruction de traduction Indent */
	       {
		 NewTransRule ();
		 CurTRule->TrType = TIndent;
		 CurTRule->TrIndentFileNameVar = 0;
		 CurTRule->TrIndentVal = 0;
		 CurTRule->TrIndentType = ItAbsolute;
		 IndentSign = 0;
	       }
	     break;

	   case KWD_RemoveFile:	/* RemoveFile */
	     NewTransRule ();
	     CurTRule->TrType = TRemoveFile;
	     break;

	   case KWD_Suspend:		/* Suspend */
	     CurTRule->TrIndentType = ItSuspend;
	     break;
	     
	   case KWD_Resume:		/* Resume */
	     CurTRule->TrIndentType = ItResume;
	     break;

	   case KWD_LineSpacing:
	     PresentationName (PtLineSpacing, pr, wi);
	     break;

	   case KWD_Adjust:
	     PresentationName (PtAdjust, pr, wi);
	     break;

	   case KWD_Hyphenate:
	     PresentationName (PtHyphenate, pr, wi);
	     break;

	   case KWD_Style:
	     PresentationName (PtStyle, pr, wi);
	     break;
	     
	   case KWD_Weight:
	     PresentationName (PtWeight, pr, wi);
	     break;

	   case KWD_Font:
	     PresentationName (PtFont, pr, wi);
	     break;

	   case KWD_UnderLine:
	     if (r == RULE_PresRule)
	       PresentationName (PtUnderline, pr, wi);
	     else if (r == RULE_UnderLineVal)
	       PresentValue ('U', wi);
	     break;

	   case KWD_Thickness:
	     PresentationName (PtThickness, pr, wi);
	     break;
	     
	   case KWD_LineStyle:
	     PresentationName (PtLineStyle, pr, wi);
	     break;

	   case KWD_LineWeight:
	     PresentationName (PtLineWeight, pr, wi);
	     break;

	   case KWD_FillPattern:
	     PresentationName (PtFillPattern, pr, wi);
	     break;

	   case KWD_Background:
	     PresentationName (PtBackground, pr, wi);
	     break;

	   case KWD_Foreground:
	     PresentationName (PtForeground, pr, wi);
	     break;

	   case KWD_Left:
	     PresentValue ('L', wi);
	     break;
	     
	   case KWD_Right:
	     PresentValue ('R', wi);
	     break;
	     
	   case KWD_VMiddle:
	     PresentValue ('C', wi);
	     break;

	   case KWD_LeftWithDots:
	     PresentValue ('D', wi);
	     break;

	   case KWD_Justify:
	     PresentValue ('J', wi);
	     break;

	   case KWD_Yes:
	     PresentValue ('Y', wi);
	     break;

	   case KWD_No:
	     if (r == RULE_UnderLineVal)
	       PresentValue ('N', wi);
	     else
	       PresentValue ('N', wi);
	     break;

	   case KWD_Roman:
	     PresentValue ('R', wi);
	     break;

	   case KWD_Italics:
	     PresentValue ('I', wi);
	     break;

	   case KWD_Oblique:
	     PresentValue ('O', wi);
	     break;

	   case KWD_Normal:
	     PresentValue ('N', wi);
	     break;

	   case KWD_Bold:
	     PresentValue ('B', wi);
	     break;

	   case KWD_Times:
	     PresentValue ('T', wi);
	     break;

	   case KWD_Helvetica:
	     PresentValue ('H', wi);
	     break;

	   case KWD_Courier:
	     PresentValue ('C', wi);
	     break;

	   case KWD_NoUnderline:
	     PresentValue ('N', wi);
	     break;

	   case KWD_Underlined:
	     PresentValue ('U', wi);
	     break;

	   case KWD_Overlined:
	     PresentValue ('O', wi);
	     break;

	   case KWD_CrossedOut:
	     PresentValue ('C', wi);
	     break;

	   case KWD_OverLine:
	     PresentValue ('O', wi);
	     break;

	   case KWD_StrikeOut:
	     PresentValue ('C', wi);
	     break;

	   case KWD_Thick:
	     PresentValue ('N', wi);
	     break;

	   case KWD_Thin:
	     PresentValue ('T', wi);
	     break;

	   case KWD_Solid:
	     PresentValue ('S', wi);
	     break;

	   case KWD_Dashed:
	     PresentValue ('-', wi);
	     break;

	   case KWD_Dotted:
	     PresentValue ('.', wi);
	     break;

	   case KWD_ComputedPage:
	     if (CurType != PageBreak + 1)
	       /* l'element auquel s'applique la regle n'est pas une marque
		  de page */
	       /* erreur */
	       CompilerMessage (wi, TRA, FATAL, VALID_ONLY_FOR_PAGES,
				inputLine, LineNum);
	     else
	       CurBlock->TbCondition[CurBlock->TbNConditions - 1].TcCondition =
		 TcondComputedPage;
	     break;
	     
	   case KWD_StartPage:
	     if (CurType != PageBreak + 1)
	       /* l'element auquel s'applique la regle n'est pas une marque
		  de page */
	       /* erreur */
	       CompilerMessage (wi, TRA, FATAL, VALID_ONLY_FOR_PAGES,
				inputLine, LineNum);
	     else
	       CurBlock->TbCondition[CurBlock->TbNConditions - 1].TcCondition =
		 TcondStartPage;
	     break;

	   case KWD_UserPage:
	     if (CurType != PageBreak + 1)
	       /* l'element auquel s'applique la regle n'est pas une marque
		  de page */
	       /* erreur */
	       CompilerMessage (wi, TRA, FATAL, VALID_ONLY_FOR_PAGES,
				inputLine, LineNum);
	     else
	       CurBlock->TbCondition[CurBlock->TbNConditions - 1].TcCondition =
		 TcondUserPage;
	     break;

	   case KWD_ReminderPage:
	     if (CurType != PageBreak + 1)
	       /* l'element auquel s'applique la regle n'est pas une marque
		  de page */
	       /* erreur */
	       CompilerMessage (wi, TRA, FATAL, VALID_ONLY_FOR_PAGES,
				inputLine, LineNum);
	     else
	       CurBlock->TbCondition[CurBlock->TbNConditions - 1].TcCondition =
		 TcondReminderPage;
	     break;

	   case KWD_Empty:
	     CurBlock->TbCondition[CurBlock->TbNConditions - 1].TcCondition =
	       TcondEmpty;
	     break;

	   case KWD_Root:
	     CurBlock->TbCondition[CurBlock->TbNConditions - 1].TcCondition =
	       TcondRoot;
	     break;

	   case KWD_Transclusion:
	     CurBlock->TbCondition[CurBlock->TbNConditions - 1].TcCondition =
	       TcondTransclusion;
	     break;

	   case KWD_Parent:
	     CurBlock->TbCondition[CurBlock->TbNConditions - 1].
	       TcAscendRelLevel = 1;
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
	       /* r = rule number  */
	       case RULE_TypeIdent:	/* TypeIdent */
		 if (pr == RULE_TransSchema)
		   /* nom de la structure generique a laquelle se rapporte */
		   /* le schema de traduction */
		   {
		     CopyWord (pTSchema->TsStructName, wi, wl);
		     /* lit le schema de structure compile' */
		     if (!ReadStructureSchema (pTSchema->TsStructName, pSSchema))
		       /* echec lecture du  schema de structure */
		       TtaDisplaySimpleMessage (FATAL, TRA,
						CANT_READ_STRUCT_SCHEM);
		     else
		       if (strcmp (pTSchema->TsStructName, pSSchema->SsName))
			 CompilerMessage (wi, TRA, FATAL,
					  STRUCT_SCHEM_DOES_NOT_MATCH,
					  inputLine, LineNum);
		       else
			 {
			   pTSchema->TsStructCode = pSSchema->SsCode;
			   InitAttrTransl ();
			   InitElemTransl ();
			 }
		   }
		 else if (pr == RULE_CondOnAscend)
		   {
		     if (CurBlock->TbCondition[CurBlock->TbNConditions - 1].
			 TcCondition == TcondFirstWithin ||
			 CurBlock->TbCondition[CurBlock->TbNConditions - 1].
			 TcCondition == TcondWithin)
		       /* dans une condition Within ou FirstWithin */
		       if (!ExternalSchema)
			 /* nom du type d'element */
			 {
			   CopyWord (TypeWithin, wi, wl);
			   BeginTypeWithin = wi;
			 }
		       else
			 /* nom d'un schema de structure externe dans une
			    condition Within ou FirstWithin */
			 {
			   ExternalSchema = False;
			   /* recupere dans n le nom du schema externe */
			   CopyWord (n, wi, wl);
			   /* lit le schema de structure externe */
			   if (!ReadStructureSchema (n, pExtSSchema))
			     /* echec lecture du schema */
			     CompilerMessage (wi, TRA, FATAL,
					      CANT_READ_STRUCT_SCHEM,
					      inputLine, LineNum);
			   else
			     /* le schema de structure a ete charge' */
			     {
			       strncpy (CurBlock->TbCondition[CurBlock->
					   TbNConditions - 1].TcElemNature, n,
					 MAX_NAME_LENGTH);
			       EndOfCondition (pExtSSchema);
			     }
			 }
		     else
		       /* dans une condition "if TYPE" */
		       ElementTypeInCond (wi, wl);
		   }
		 else if (pr == RULE_LevelOrType)
		   /* un type d'element dans une condition portant sur */
		   /* un ascendant */
		   {
		     if (!ExternalSchema)
		       /* nom du type d'element ascendant dans la condition */
		       {
			 CopyWord (AncestorName, wi, wl);
			 BeginAncestorName = wi;
		       }
		     else
		       /* nom d'un schema de structure externe dans Ascend */
		       {
			 ExternalSchema = False;
			 /* recupere dans n le nom du schema externe */
			 CopyWord (n, wi, wl);
			 /* lit le schema de structure externe */
			 if (!ReadStructureSchema (n, pExtSSchema))
			   CompilerMessage (wi, TRA, FATAL,
					    CANT_READ_STRUCT_SCHEM,
					    inputLine, LineNum);
			 /* echec lecture du schema */
			 else
			   /* le schema de    structure a ete charge' */
			   {
			     strncpy (CurBlock->TbCondition[CurBlock->
					  TbNConditions - 1].TcAscendNature,
				       n, MAX_NAME_LENGTH);
			     ProcessAncestorName (pExtSSchema);
			   }
		       }
		   }
		 else
		   {
		     InUseRule = False;
		     InGetRule = False;
		     InCreateWriteRule = (pr == RULE_Token);
		     if (pr == RULE_RuleA)
		       {
			 if (CurTRule->TrType == TUse)
			   InUseRule = True;
			 if (CurTRule->TrType == TGet ||
			     CurTRule->TrType == TCopy)
			   InGetRule = True;
		       }
		     if (InUseRule)
		       CopyWord (CurTRule->TrNature, wi, wl);
		     else if (InGetRule || InCreateWriteRule)
		       if (ExternalSchema)
			 /* nom d'un schema de structure externe */
			 {
			   ExternalSchema = False;
			   CopyWord (n, wi, wl);
			   /* recupere dans n le nom du schema externe */
			   /* lit le schema de structure externe */
			   if (!ReadStructureSchema (n, pExtSSchema))
			     /* echec lecture du schema */
			     CompilerMessage (wi, TRA, FATAL,
					      CANT_READ_STRUCT_SCHEM,
					      inputLine, LineNum);
			   else
			     /* le schema de    structure a ete charge' */
			     {
			       if (InGetRule)
				 strncpy (CurTRule->TrElemNature, n,
					   MAX_NAME_LENGTH);
			       else
				 strncpy (CurTRule->TrObjectNature, n,
					   MAX_NAME_LENGTH);
			       ProcessTypeName (pExtSSchema);
			     }
			 }
		       else
			 {
			   CopyWord (TypeInGetRule, wi, wl);
			   BeginTypeInGetRule = wi;
			 }
		     else if (pr == RULE_VarOrType)
		       /* dans une regle Create ou Write, apres Refered */
		       {
			 VarDefinition = False;
			 CopyWord (TypeInGetRule, wi, wl);
			 BeginTypeInGetRule = wi;
		       }
		     else
		       {
			 i = ElementTypeNum (wi, wl);
			 if (i > 0)
			   /* le type existe, il a le numero i */
			   {
			   if (ComptDef)
			     /* dans une declaration de compteur */
			     {
			       pCntr = &pTSchema->TsCounter[pTSchema->
							   TsNCounters - 1];
			       if (pCntr->TnOperation == TCntrSet)
				 if (pCntr->TnElemType1 == 0)
				   pCntr->TnElemType1 = i;
				 else
				   pCntr->TnElemType2 = i;
			       else
				 pCntr->TnElemType1 = i;
			     }
			   else
			     /* on n'est pas dans une declaration de compteur*/
			     if (pr == RULE_TransType)
			       /* debut des regles associees a un type */
			       {
				 if (pSSchema->SsRule->SrElem[i - 1]->SrConstruct ==
				     CsPairedElement)
				   /* c'est un element CsPairedElement */
				   if (!SecondInPair && !FirstInPair)
				     /* le nom du type n'etait pas precede' de
					First ou Second */
				     CompilerMessage (wi, TRA, FATAL,
						      MISSING_FIRST_SECOND,
						      inputLine, LineNum);
				   else
				     {
				       if (SecondInPair)
					 /* il s'agit du type suivant */
					 i++;
				     }
				 else
				   /* ce n'est pas un element CsPairedElement*/
				   if (SecondInPair || FirstInPair)
				     /* le nom du type etait precede' de First
					ou Second, erreur */
				     CompilerMessage (wi, TRA, FATAL,
						      NOT_A_PAIR,
						      inputLine, LineNum);
				 if (pTSchema->TsElemTRule->TsElemTransl[i-1])
				   CompilerMessage (wi, TRA, FATAL,
						    CANT_REDEFINE,
						    inputLine, LineNum);
				 else
				   {
				     CurType = i;
				     CurBlock = NULL;
				     CurTRule = NULL;
				   }
				 FirstInPair = False;
				 SecondInPair = False;
			       }
			 
			     else if (pr == RULE_TransAttr)
			       /* apres un nom d'attribut */
			       {
				 pTSchema->TsInheritAttr->Bln[i - 1] = True;
				 pTSchema->TsAttrTRule->TsAttrTransl[CurAttr - 1]->AtrElemType = i;
			       }
			   }
		       }
		   }
		 break;
		 
	       case RULE_ConstIdent:	/* ConstIdent */
		 if (ConstDef)	/* une definition de constante */
		   if (Identifier[nb - 1].SrcIdentDefRule != 0)
		     /* nom deja declare' */
		     CompilerMessage (wi, TRA, FATAL,
				      INVALID_NAME_REDECLARATION, inputLine,
				      LineNum);
		   else if (pTSchema->TsNConstants >= MAX_TRANSL_CONST)
		     /* table des constantes saturee */
		     CompilerMessage (wi, TRA, FATAL,
				      MAX_POSSIBLE_CONSTANTS_OVERFLOW,
				      inputLine, LineNum);
		   else
		     /*alloue un nouvelle entree dans la table des constantes*/
		     {
		       pTSchema->TsNConstants++;
		       Identifier[nb - 1].SrcIdentDefRule =
			 pTSchema->TsNConstants;
		     }
		 else
		   /* utilisation d'une constante */
		   if (Identifier[nb - 1].SrcIdentDefRule == 0)
		     /* ce nom n'a pas ete declare comme un identificateur */
		     /* de constante */
		     if (pr == RULE_Token || pr == RULE_Function)
		       /* s'il vient de la regle Token ou Function, voyons si/
			  ce n'est pas un identificateur d'attribut */
		       {
			 i = AttributeNum (wi, wl);
			 if (i == 0)
			   CompilerMessage (wi, TRA, FATAL, BAD_ATTR,
					    inputLine, LineNum);
			 else
			   /* l'attribut existe, il a le numero i */
			   {
			     AttrInCreateOrWrite (i, pr, wi);
			     /* ce nom est maintenant un nom d'attribut */
			     Identifier[nb - 1].SrcIdentCode = RULE_AttrIdent;
			   }
		       }
		     else
		       /* constante non definie */
		       CompilerMessage (wi, TRA, FATAL, UNDECLARED_CONSTANT,
					inputLine, LineNum);
		   else if (VarDef)	/* dans une declaration de variable */
		     {
		       pTransVar = &pTSchema->TsVariable[pTSchema->
							TsNVariables - 1];
		       if (pTransVar->TrvNItems >= MAX_TRANSL_VAR_ITEM)
			 /* trop de fonctions */
			 CompilerMessage (wi, TRA, FATAL,
				      MAX_POSSIBLE_ELEMS_IN_VARIABLE_OVERFLOW,
				      inputLine, LineNum);
		       else
			 {
			   pTransVar->TrvItem[pTransVar->TrvNItems].TvType =
			     VtText;
			   pTransVar->TrvItem[pTransVar->TrvNItems].TvItem =
			     Identifier[nb - 1].SrcIdentDefRule;
			   pTransVar->TrvNItems++;
			 }
		     }
		   else if (pr == RULE_Token)
		     /* dans une regle Create ou Write */
		     {
		       CurTRule->TrObject = ToConst;
		       CurTRule->TrObjectNum =
			 Identifier[nb - 1].SrcIdentDefRule;
		     }
		 break;

	       case RULE_CounterIdent:	/* CounterIdent */
		 if (ComptDef)	/* une definition de compteur */
		   if (Identifier[nb - 1].SrcIdentDefRule != 0)
		     /* nom deja declare' */
		     CompilerMessage (wi, TRA, FATAL,
				      INVALID_NAME_REDECLARATION, inputLine,
				      LineNum);
		   else if (pTSchema->TsNCounters >= MAX_TRANSL_COUNTER)
		     /* table des compteurs saturee */
		     CompilerMessage (wi, TRA, FATAL,
				      MAX_POSSIBLE_COUNTERS_OVERFLOW,
				      inputLine, LineNum);
		   else
		     /* alloue un nouvelle entree dans la table des compteurs*/
		     {
		       /* a priori ce compteur n'a pas d'operations */
		       pTSchema->TsCounter[pTSchema->TsNCounters].TnOperation =
			 TCntrNoOp;
		       /* a priori, ce compteur ne sera pas */
		       /* intialise' par un attribut */
		       pTSchema->TsCounter[pTSchema->TsNCounters].TnAttrInit =
			 0;
		       pTSchema->TsNCounters++;
		       Identifier[nb - 1].SrcIdentDefRule =
			 pTSchema->TsNCounters;
		     }
		 else
		   /* utilisation d'un compteur */
		   if (Identifier[nb - 1].SrcIdentDefRule == 0)
		     /* compteur non defini */
		     CompilerMessage (wi, TRA, FATAL, UNDECLARED_COUNTER,
				      inputLine, LineNum);
		   else if (pr == RULE_Function)
		     /* dans une declaration de variable */
		     {
		       pTransVar = &pTSchema->TsVariable[pTSchema->
							TsNVariables - 1];
		       pTransVar->TrvItem[pTransVar->TrvNItems - 1].TvItem =
			 Identifier[nb - 1].SrcIdentDefRule;
		       pTransVar->TrvItem[pTransVar->TrvNItems - 1].TvLength =
			 0;
		       pTransVar->TrvItem[pTransVar->TrvNItems - 1].
			 TvCounterStyle = CntDecimal;
		     }
		   else if (pr == RULE_RuleB)
		     /* un compteur dans une instruction Set ou Add */
		     {
		     if (pTSchema->TsCounter[Identifier[nb - 1].
				 SrcIdentDefRule - 1].TnOperation != TCntrNoOp)
		       CompilerMessage (wi, TRA, FATAL, BAD_COUNTER,
					inputLine, LineNum);
		     else
		       CurTRule->TrCounterNum =
			 Identifier[nb - 1].SrcIdentDefRule;
		     }
		 break;

	       case RULE_BufferIdent:		/* BufferIdent */
		 if (BuffDef)		/* une definition de  buffer */
		   if (Identifier[nb - 1].SrcIdentDefRule != 0)
		     /* nom deja declare' */
		     CompilerMessage (wi, TRA, FATAL,
				      INVALID_NAME_REDECLARATION, inputLine,
				      LineNum);
		   else if (pTSchema->TsNBuffers >= MAX_TRANSL_BUFFER)
		     /* table des buffers saturee */
		     CompilerMessage (wi, TRA, FATAL,
				      MAX_POSSIBLE_BUFFERS_OVERFLOW,
				      inputLine, LineNum);
		   else
		     {
		       pTSchema->TsNBuffers++;
		       Identifier[nb - 1].SrcIdentDefRule =
			 pTSchema->TsNBuffers;
		       /* save the name of that buffer in case it is
			  followed by "(Variable)" */
		       CopyWord (pTSchema->TsVarBuffer[pTSchema->TsNVarBuffers].VbIdent, wi, wl);
		     }
		 else
		   /* utilisation d'un buffer */
		   if (Identifier[nb - 1].SrcIdentDefRule == 0)
		     /* buffer non defini */
		     CompilerMessage (wi, TRA, FATAL, UNDECLARED_BUFFER,
				      inputLine, LineNum);
		   else if (VarDef)
		     /* dans une declaration de variable */
		     {
		       pTransVar =
			 &pTSchema->TsVariable[pTSchema->TsNVariables - 1];
		       if (pTransVar->TrvNItems >= MAX_TRANSL_VAR_ITEM)
			 /* trop de fonctions */
			 CompilerMessage (wi, TRA, FATAL,
				       MAX_POSSIBLE_ELEMS_IN_VARIABLE_OVERFLOW,
				       inputLine, LineNum);
		       else
			 {
			   pTransVar->TrvItem[pTransVar->TrvNItems].TvType =
			     VtBuffer;
			   pTransVar->TrvItem[pTransVar->TrvNItems].TvItem =
			     Identifier[nb - 1].SrcIdentDefRule;
			   pTransVar->TrvNItems++;
			 }
		     }
		   else if (pr == RULE_RuleA)
		     /* dans une regle Read */
		     CurTRule->TrBuffer = Identifier[nb - 1].SrcIdentDefRule;
		   else if (pr == RULE_Token)
		     /* dans une regle Create ou Write */
		     {
		       CurTRule->TrObject = ToBuffer;
		       CurTRule->TrObjectNum =
			 Identifier[nb - 1].SrcIdentDefRule;
		     }
		   else if (pr == RULE_File)
		     {
		       /* dans une regle Include */
		       CurTRule->TrBufOrConst = ToBuffer;
		       CurTRule->TrInclFile =
			 Identifier[nb - 1].SrcIdentDefRule;
		     }
		 break;

	       case RULE_VariableIdent:	/* VariableIdent */
		 if (VarDef)
		   /* une definition de  variable */
		   if (Identifier[nb - 1].SrcIdentDefRule != 0)
		     /* nom deja declare' */
		     CompilerMessage (wi, TRA, FATAL,
				      INVALID_NAME_REDECLARATION, inputLine,
				      LineNum);
		   else if (pTSchema->TsNVariables >= MAX_TRANSL_VARIABLE)
		     /* table des variables saturee */
		     CompilerMessage (wi, TRA, FATAL,
				      MAX_POSSIBLE_VARIABLES_OVERFLOW,
				      inputLine, LineNum);
		   else
		     {
		       pTSchema->TsVariable[pTSchema->TsNVariables++].
			 TrvNItems = 0;
		       Identifier[nb - 1].SrcIdentDefRule =
			 pTSchema->TsNVariables;
		     }
		 else if (pr == RULE_Token)
		   /* dans une regle Create  ou Write */
		   if (Identifier[nb - 1].SrcIdentDefRule == 0)
		     /* variable non definie */
		     CompilerMessage (wi, TRA, FATAL, UNKNOWN_VARIABLE,
				      inputLine, LineNum);
		   else
		     {
		       CurTRule->TrObject = ToVariable;
		       CurTRule->TrObjectNum =
			 Identifier[nb - 1].SrcIdentDefRule;
		     }
		 else if (pr == RULE_VarOrType)
		   /* dans une regle Create  ou Write */
		   {
		     VarDefinition = False;
		     if (Identifier[nb - 1].SrcIdentDefRule == 0)
		       /* variable non definie */
		       /* c'est peut-etre un nom de type d'element */
		       {
			 CurTRule->TrObject = ToReferredElem;
			 CopyWord (TypeInGetRule, wi, wl);
			 BeginTypeInGetRule = wi;
		       }
		     else
		       /* la variable est bien definie */
		       {
			 CurTRule->TrObject = ToVariable;
			 CurTRule->TrObjectNum =
			   Identifier[nb - 1].SrcIdentDefRule;
		       }
		   }
		 else if (pr == RULE_RuleA || pr == RULE_RuleB)
		   {
		   if (Identifier[nb - 1].SrcIdentDefRule == 0)
		     /* variable non definie */
		     CompilerMessage (wi, TRA, FATAL, UNKNOWN_VARIABLE,
				      inputLine, LineNum);
		   else if (CurTRule->TrType == TCreate)
		     /* indication du fichier de sortie dans une regle Create*/
		     CurTRule->TrFileNameVar =
		       Identifier[nb - 1].SrcIdentDefRule;
		   else if (CurTRule->TrType == TChangeMainFile)
		     CurTRule->TrNewFileVar =
		       Identifier[nb - 1].SrcIdentDefRule;
		   else if (CurTRule->TrType == TIndent)
		     CurTRule->TrIndentFileNameVar =
		       Identifier[nb - 1].SrcIdentDefRule;
		   else if (CurTRule->TrType == TRemoveFile)
		     CurTRule->TrNewFileVar =
		       Identifier[nb - 1].SrcIdentDefRule;
		   }
		 break;

	       case RULE_AttrIdent:	/* AttrIdent */
		 i = AttributeNum (wi, wl);
		 if (pr == RULE_TransAttr)
		   if (i == 0)
		     CompilerMessage (wi, TRA, FATAL, BAD_ATTR, inputLine,
				      LineNum);
		   else
		     /* l'attribut existe, il a le numero i*/
		     /* c'est un nom d'attribut auquel on va associer des */
		     /* regles de traduction */
		     {
		       CurAttr = i;
		       CurValAttr = 0;	 /* pas encore rencontre' de valeur */
		       CurBlock = NULL;
		       CurTRule = NULL;
		       pAttrTrans = pTSchema->TsAttrTRule->TsAttrTransl[CurAttr - 1];
		       switch (pSSchema->SsAttribute->TtAttr[CurAttr - 1]->AttrType)
			 {
			 case AtNumAttr:     /* attribut a valeur numerique */
			   if (pAttrTrans->AtrNCases >= MAX_TRANSL_ATTR_CASE)
			     /* trop de cas pour cet attribut */
			     CompilerMessage (wi, TRA, FATAL,
					  MAX_POSSIBLE_CASES_FOR_ATTR_OVERFLOW,
					  inputLine, LineNum);
			   else
			     pAttrTrans->AtrNCases++;
			   break;

			 case AtTextAttr /* attribut textuel */ :
			   if (pAttrTrans->AtrTxtTRuleBlock != NULL)
			     /* attribut deja rencontre' */
			     CompilerMessage (wi, TRA, FATAL,
					     RULES_ALREADY_EXIST_FOR_THAT_ATTR,
					     inputLine, LineNum);
			   break;

			 case AtReferenceAttr:
			   if (pAttrTrans->AtrRefTRuleBlock != NULL)
			     /* attribut deja rencontre' */
			     CompilerMessage (wi, TRA, FATAL,
					     RULES_ALREADY_EXIST_FOR_THAT_ATTR,
					     inputLine, LineNum);
			   break;

			 case AtEnumAttr:
			   break;

			 default:
			   break;
			 }
		       
		     }
		 else if (pr == RULE_CondOnAscend)
		   /* un nom d'attribut dans une condition */
		   if (i == 0)
		     /* ce n'est pas un attribut connu */
		     /* c'est peut etre un type d'element*/
		     {
		       if (ElementTypeInCond (wi, wl))
			 /* ce nom est maintenant un type */
			 Identifier[nb - 1].SrcIdentCode = RULE_TypeIdent;
		     }
		   else
		     /* c'est un attribut du schema */
		     {
		       ProcessAncestorName (pSSchema);
		       if (!Asterisk)
			 CurBlock->TbCondition[CurBlock->TbNConditions - 1].
			   TcAscendRelLevel = -1;
		       CurBlock->TbCondition[CurBlock->TbNConditions - 1].
			 TcCondition = TcondAttr;
		       CurBlock->TbCondition[CurBlock->TbNConditions - 1].
			 TcAttr = i;
		       switch (pSSchema->SsAttribute->TtAttr[i - 1]->AttrType)
			 {
			 case AtNumAttr:
			   CurBlock->TbCondition[CurBlock->TbNConditions - 1].
			     TcLowerBound = -MAX_INT_ATTR_VAL - 1; /* -infini*/
			   CurBlock->TbCondition[CurBlock->TbNConditions - 1].
			     TcUpperBound = MAX_INT_ATTR_VAL + 1; /* +infini */
			   break;
			 case AtTextAttr:
			   CurBlock->TbCondition[CurBlock->TbNConditions - 1].
			     TcTextValue[0] = '\0';
			   break;
			 case AtReferenceAttr:
			   break;	/* rien */
			 case AtEnumAttr:
			   CurBlock->TbCondition[CurBlock->TbNConditions - 1].
			     TcAttrValue = 0;
			   break;
			 default:
			   break;
			 }

		     }
		 else if (pr == RULE_Token ||
			  pr == RULE_CountFunction ||
			  pr == RULE_Function)
		   AttrInCreateOrWrite (i, pr, wi);
		 break;

	       case RULE_AttrValIdent:	/* AttrValIdent */
		 CopyWord (n, wi, wl);
		 /* verifie si cette valeur existe pour l'attribut courant */
		 if (InCondition)
		   k = CurBlock->TbCondition[CurBlock->TbNConditions - 1].
		     TcAttr;
		 else
		   k = CurAttr;
		 pAttr = pSSchema->SsAttribute->TtAttr[k - 1];
		 if (pAttr->AttrType != AtEnumAttr)
		   /* pas un attribut a valeur enumerees */
		   CompilerMessage (wi, TRA, FATAL, BAD_ATTR_VALUE, inputLine,
				    LineNum);
		 else
		   {
		     i = 1;
		     while (strcmp (n, pAttr->AttrEnumValue[i - 1]) != 0 &&
			    i < pAttr->AttrNEnumValues)
		       i++;
		     if (strcmp (n, pAttr->AttrEnumValue[i - 1]) != 0)
		       /* valeur d'attribut  incorrecte */
		       CompilerMessage (wi, TRA, FATAL, BAD_ATTR_VALUE,
					inputLine, LineNum);
		     else
		       /* la valeur est correcte, elle a le numero i */
		       if (!InCondition)
			 /* debut des regles de traduction d'un attribut */
			 if (pTSchema->TsAttrTRule->TsAttrTransl[CurAttr - 1]->
			     AtrEnuTRuleBlock[i] != NULL)
			   /* deja des regles pour cette valeur */
			   CompilerMessage (wi, TRA, FATAL,
					    RULES_ALREADY_EXIST_FOR_THAT_VALUE,
					    inputLine, LineNum);
			 else
			   CurValAttr = i;
		       else
			 /* un nom d'attribut dans une condition */
			 CurBlock->TbCondition[CurBlock->TbNConditions - 1].
			   TcAttrValue = i;
		   }
		 break;

	       case RULE_Script:	/* Script */
		 if (pr == RULE_TSeqOfTransl)
		   /* Deja des regles de traduction pour cet script ? */
		   {
		     for (i = 0; i < pTSchema->TsNTranslScripts - 1; i++)
		       if (pTSchema->TsTranslScript[i].AlScript ==
			   inputLine[wi - 1])
			 CompilerMessage (wi, TRA, FATAL,
					  CANT_REDEFINE_ALPHABET, inputLine,
					  LineNum);
		     if (!error)
		       pTSchema->TsTranslScript[pTSchema->
						 TsNTranslScripts - 1].
			 AlScript = (char)inputLine[wi - 1];
		   }
		 else if (pr == RULE_CondOnSelf)
		   /* dans une condition */
		   CurBlock->TbCondition[CurBlock->TbNConditions - 1].
		     TcScript = (char)inputLine[wi - 1];
		 break;
		 
	       case RULE_TrSchema:	/* TrSchema */
		 CopyWord (CurTRule->TrTranslSchemaName, wi, wl);
		 break;

	       case RULE_Pattern:	/* Pattern */
	       case RULE_Color:	/* Color */
		 CopyWord (n, wi, wl);
		 /* cherche le numero de cette trame ou de cette couleur */
		 if (r == RULE_Pattern)
		   k = PatternNum (n, wi);
		 else
		   k = ColorNum (n, wi);
		 if (InCondition)
		   {
		     CurBlock->TbCondition[CurBlock->TbNConditions - 1].
		       TcUpperBound = k;
		     CurBlock->TbCondition[CurBlock->TbNConditions - 1].
		       TcLowerBound = k;
		   }
		 else
		   {
		     pPresTrans = &pTSchema->TsPresTRule[CurPres - 1];
		     pCase = &pPresTrans->RtCase[pPresTrans->RtNCase - 1];
		     pCase->TaUpperBound = k;
		     pCase->TaLowerBound = k;
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

	       case RULE_MaxLineLength:	/* MaxLineLength */
		 pTSchema->TsLineLength = k;
		 break;

	       case RULE_AncestorLevel:	/* AncestorLevel */
		 pTSchema->TsCounter[pTSchema->TsNCounters - 1].
		   TnAcestorLevel = k * AncestorSign;
		 break;

	       case RULE_IndentVal:	/* IndentVal */
		 if (IndentSign != 0)
		   /* indent value is relative */
		   {
		     CurTRule->TrIndentType = ItRelative;
		     k  = k * IndentSign;
		   }
		 CurTRule->TrIndentVal = k;
		 break;

	       case RULE_RelLevel:	/* RelLevel */
		 CurBlock->TbCondition[CurBlock->TbNConditions - 1].
		   TcAscendLevel = k;
		 if (!GreaterOrLessSign)
		   CurBlock->TbCondition[CurBlock->TbNConditions - 1].
		     TcAscendRel = RelEquals;
		 break;

	       case RULE_CondRelLevel:	/* CondRelLevel */
		 CurBlock->TbCondition[CurBlock->TbNConditions - 1].
		   TcAscendRelLevel = k;
		 break;

	       case RULE_InitValue:	/* InitValue */
		 if (pr == RULE_CountFunction)
		   /* dans une definition de compteur */
		   pTSchema->TsCounter[pTSchema->TsNCounters - 1].TnParam1 = k;
		 else if (pr == RULE_RuleB)
		   /* dans une instruction de traduction Set */
		   CurTRule->TrCounterParam = k;
		 break;

	       case RULE_IncrValue:	/* IncrValue */
		 if (pr == RULE_CountFunction)
		   /* dans une definition de compteur */
		   pTSchema->TsCounter[pTSchema->TsNCounters - 1].TnParam2 = k;
		 else if (pr == RULE_RuleB)
		   /* dans une instruction de traduction Add */
		   CurTRule->TrCounterParam = k;
		 break;

	       case RULE_Length:	/* Length */
		 pTransVar = &pTSchema->TsVariable[pTSchema->TsNVariables - 1];
		 pTransVar->TrvItem[pTransVar->TrvNItems - 1].TvLength = k;
		 break;

	       case RULE_MinVal:	/* MinVal */
		 if (InCondition)
		   i = CurBlock->TbCondition[CurBlock->TbNConditions - 1].
		     TcAttr;
		 else
		   i = CurAttr;
		 if (pSSchema->SsAttribute->TtAttr[i - 1]->AttrType != AtNumAttr ||
		     k >= MAX_INT_ATTR_VAL)
		   /* ce n'est pas un attribut numerique */
		   CompilerMessage (wi, TRA, FATAL, EXPECTING_A_NUMERICAL_ATTR,
				    inputLine, LineNum);
		 else
		   {
		     k = k * AttrValSign + 1;
		     AttrValSign = 1;
		     /* a priori, la prochaine valeur */
		     /* d'attribut numerique sera positive */
		     if (InCondition)
		       CurBlock->TbCondition[CurBlock->TbNConditions - 1].
			 TcLowerBound = k;
		     else
		       {
			 pAttrTrans = pTSchema->TsAttrTRule->TsAttrTransl[CurAttr - 1];
			 pAttrTrans->AtrCase[pAttrTrans->AtrNCases - 1].
			   TaLowerBound = k;
		       }
		   }
		 break;
		 
	       case RULE_MaxVal:	/* MaxVal */
		 if (InCondition)
		   i = CurBlock->TbCondition[CurBlock->TbNConditions - 1].
		     TcAttr;
		 else
		   i = CurAttr;
		 if (pSSchema->SsAttribute->TtAttr[i - 1]->AttrType != AtNumAttr ||
		     k >= MAX_INT_ATTR_VAL)
		   /* ce n'est pas un attribut numerique */
		   CompilerMessage (wi, TRA, FATAL, EXPECTING_A_NUMERICAL_ATTR,
				    inputLine, LineNum);
		 else
		   {
		     k = k * AttrValSign - 1;
		     AttrValSign = 1;
		     /* a priori, la prochaine valeur */
		     /* d'attribut numerique sera positive */
		     if (InCondition)
		       CurBlock->TbCondition[CurBlock->TbNConditions - 1].
			 TcUpperBound = k;
		     else
		       {
			 pAttrTrans = pTSchema->TsAttrTRule->TsAttrTransl[CurAttr - 1];
			 pAttrTrans->AtrCase[pAttrTrans->AtrNCases - 1].
			   TaUpperBound = k;
		       }
		   }
		 break;

	       case RULE_MinInterval:		/* MinInterval */
		 if (InCondition)
		   i = CurBlock->TbCondition[CurBlock->TbNConditions - 1].
		     TcAttr;
		 else
		   i = CurAttr;
		 if (pSSchema->SsAttribute->TtAttr[i - 1]->AttrType != AtNumAttr ||
		     k >= MAX_INT_ATTR_VAL)
		   /* ce n'est pas un attribut numerique */
		   CompilerMessage (wi, TRA, FATAL, EXPECTING_A_NUMERICAL_ATTR,
				    inputLine, LineNum);
		 else
		   {
		     k = k * AttrValSign;
		     AttrValSign = 1;
		     /* a priori, la prochaine valeur */
		     /* d'attribut numerique sera positive */
		     if (InCondition)
		       CurBlock->TbCondition[CurBlock->TbNConditions - 1].
			 TcLowerBound = k;
		     else
		       {
			 pAttrTrans = pTSchema->TsAttrTRule->TsAttrTransl[CurAttr - 1];
			 pAttrTrans->AtrCase[pAttrTrans->AtrNCases - 1].
			   TaLowerBound = k;
		       }
		   }
		 break;

	       case RULE_MaxInterval:		/* MaxInterval */
		 if (InCondition)
		   i = CurBlock->TbCondition[CurBlock->TbNConditions - 1].
		     TcAttr;
		 else
		   i = CurAttr;
		 if (pSSchema->SsAttribute->TtAttr[i - 1]->AttrType != AtNumAttr ||
		     k >= MAX_INT_ATTR_VAL)
		   /* ce n'est pas un attribut numerique */
		   CompilerMessage (wi, TRA, FATAL, EXPECTING_A_NUMERICAL_ATTR,
				    inputLine, LineNum);
		 else
		   {
		     k = k * AttrValSign;
		     AttrValSign = 1;
		     /* a priori, la prochaine valeur */
		     /* d'attribut numerique sera positive */
		     if (InCondition)
		       if (CurBlock->TbCondition[CurBlock->TbNConditions - 1].
			   TcLowerBound > k)
			 CompilerMessage (wi, TRA, FATAL, BAD_LIMITS,
					  inputLine, LineNum);
		       else
			 CurBlock->TbCondition[CurBlock->TbNConditions - 1].
			   TcUpperBound = k;
		     else
		       {
			 pAttrTrans = pTSchema->TsAttrTRule->TsAttrTransl[CurAttr - 1];
			 if (pAttrTrans->AtrCase[pAttrTrans->AtrNCases - 1].
			     TaLowerBound > k)
			   CompilerMessage (wi, TRA, FATAL, BAD_LIMITS,
					    inputLine, LineNum);
			 else
			   pAttrTrans->AtrCase[pAttrTrans->AtrNCases - 1].
			     TaUpperBound = k;
		       }
		   }
		 break;

	       case RULE_ValEqual:	/* ValEqual */
		 if (InCondition)
		   i = CurBlock->TbCondition[CurBlock->TbNConditions - 1].
		     TcAttr;
		 else
		   i = CurAttr;
		 if (pSSchema->SsAttribute->TtAttr[i - 1]->AttrType != AtNumAttr ||
		     k >= MAX_INT_ATTR_VAL)
		   /* ce n'est pas un attribut numerique */
		   CompilerMessage (wi, TRA, FATAL, BAD_LIMITS, inputLine,
				    LineNum);
		 else
		   {
		     k = k * AttrValSign;
		     AttrValSign = 1;
		     /* a priori, la prochaine valeur */
		     /* d'attribut numerique sera positive */
		     if (InCondition)
		       {
			 CurBlock->TbCondition[CurBlock->TbNConditions - 1].
			   TcUpperBound = k;
			 CurBlock->TbCondition[CurBlock->TbNConditions - 1].
			   TcLowerBound = k;
		       }
		     else
		       {
			 pAttrTrans = pTSchema->TsAttrTRule->TsAttrTransl[CurAttr - 1];
			 pCase = &pAttrTrans->AtrCase[pAttrTrans->AtrNCases-1];
			 pCase->TaUpperBound = k;
			 pCase->TaLowerBound = pCase->TaUpperBound;
		       }
		   }
		 break;

	       case RULE_MinimumPres:
		 k = k * PresValSign + 1;
		 PresValSign = 1;
		 /* a priori, la prochaine valeur */
		 /* d'attribut numerique sera positive */
		 if (InCondition)
		   CurBlock->TbCondition[CurBlock->TbNConditions - 1].
		     TcLowerBound = k;
		 else
		   {
		     pPresTrans = &pTSchema->TsPresTRule[CurPres - 1];
		     pPresTrans->RtCase[pPresTrans->RtNCase - 1].TaLowerBound =
		       k;
		   }
		 break;

	       case RULE_MaximumPres:
		 k = k * PresValSign - 1;
		 PresValSign = 1;
		 /* a priori, la prochaine valeur */
		 /* d'attribut numerique sera positive */
		 if (InCondition)
		   CurBlock->TbCondition[CurBlock->TbNConditions - 1].
		     TcUpperBound = k;
		 else
		   {
		     pPresTrans = &pTSchema->TsPresTRule[CurPres - 1];
		     pPresTrans->RtCase[pPresTrans->RtNCase - 1].TaUpperBound =
		       k;
		   }
		 break;

	       case RULE_MinIntervalPres:
		 k = k * PresValSign;
		 PresValSign = 1;
		 /* a priori, la prochaine valeur */
		 /* d'attribut numerique sera positive */
		 if (InCondition)
		   CurBlock->TbCondition[CurBlock->TbNConditions - 1].
		     TcLowerBound = k;
		 else
		   {
		     pPresTrans = &pTSchema->TsPresTRule[CurPres - 1];
		     pPresTrans->RtCase[pPresTrans->RtNCase - 1].TaLowerBound =
		       k;
		   }
		 break;

	       case RULE_MaxIntervalPres:
		 k = k * PresValSign;
		 PresValSign = 1;
		 /* a priori, la prochaine valeur */
		 /* d'attribut numerique sera positive */
		 if (InCondition)
		   if (CurBlock->TbCondition[CurBlock->TbNConditions - 1].
		       TcLowerBound > k)
		     /* BorneInf > BorneSup !! */
		     CompilerMessage (wi, TRA, FATAL, BAD_LIMITS, inputLine,
				      LineNum);
		   else
		     CurBlock->TbCondition[CurBlock->TbNConditions - 1].
		       TcUpperBound = k;
		 else
		   {
		     pPresTrans = &pTSchema->TsPresTRule[CurPres - 1];
		     if (pPresTrans->RtCase[pPresTrans->RtNCase - 1].
			 TaLowerBound > k)
		       CompilerMessage (wi, TRA, FATAL, BAD_LIMITS,
					inputLine, LineNum);
		     else
		       pPresTrans->RtCase[pPresTrans->RtNCase - 1].
			 TaUpperBound = k;
		   }
		 break;

	       case RULE_PresVal:
		 k = k * PresValSign;
		 PresValSign = 1;
		 /* a priori, la prochaine valeur */
		 /* d'attribut numerique sera positive */
		 if (InCondition)
		   {
		     CurBlock->TbCondition[CurBlock->TbNConditions - 1].
		       TcUpperBound = k;
		     CurBlock->TbCondition[CurBlock->TbNConditions - 1].
		       TcLowerBound = k;
		   }
		 else
		   {
		     pPresTrans = &pTSchema->TsPresTRule[CurPres - 1];
		     pCase = &pPresTrans->RtCase[pPresTrans->RtNCase - 1];
		     pCase->TaUpperBound = k;
		     pCase->TaLowerBound = k;
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
		   CompilerMessage (wi, TRA, FATAL,
				    MAX_CONSTANT_BUFFER_OVERFLOW, inputLine,
				    LineNum);
		 else
		   /* FnCopy le texte de la constante */
		   {
		     pTSchema->TsConstBegin[pTSchema->TsNConstants - 1] =
		       ConstIndx;
		     for (i = 0; i <= wl - 2; i++)
		       pTSchema->TsConstant[ConstIndx + i - 1] =
			 inputLine[wi + i - 1];
		     ConstIndx += wl;
		     pTSchema->TsConstant[ConstIndx - 2] = '\0';
		   }
		 break;

	       case RULE_CharString:	/* CharString */
		 NewConstant (wl, wi);
		 if (!error)
		   {
		   if (pr == RULE_Function)
		     /* dans une definition de variable */
		     {
		       pTransVar = &pTSchema->TsVariable[pTSchema->
							TsNVariables - 1];
		       if (pTransVar->TrvNItems >= MAX_TRANSL_VAR_ITEM)
			 /* trop de fonctions */
			 CompilerMessage (wi, TRA, FATAL,
				       MAX_POSSIBLE_ELEMS_IN_VARIABLE_OVERFLOW,
				       inputLine, LineNum);
		       else
			 {
			   pTransVar->TrvItem[pTransVar->TrvNItems].TvType =
			     VtText;
			   pTransVar->TrvItem[pTransVar->TrvNItems].TvItem =
			     pTSchema->TsNConstants;
			   pTransVar->TrvNItems++;
			 }
		     }
		   else if (pr == RULE_Token)
		     /* dans une regle Create ou Write */
		     {
		       CurTRule->TrObject = ToConst;
		       CurTRule->TrObjectNum = pTSchema->TsNConstants;
		     }
		   }
		 break;

	       case RULE_TextEqual:	/* TextEqual */
		 if (InCondition)
		   i = CurBlock->TbCondition[CurBlock->TbNConditions - 1].
		     TcAttr;
		 else
		   i = CurAttr;
		 if (pSSchema->SsAttribute->TtAttr[i - 1]->AttrType != AtTextAttr)
		   /* ce n'est pas un attribut textuel */
		   CompilerMessage (wi, TRA, FATAL, NOT_A_TEXTUAL_ATTR,
				    inputLine, LineNum);
		 else if (wl > MAX_NAME_LENGTH)
		   /* texte trop long */
		   CompilerMessage (wi, TRA, FATAL, MAX_NAME_SIZE_OVERFLOW,
				    inputLine, LineNum);
		 else if (InCondition)
		   {
		     for (i = 0; i < wl - 1; i++)
		       CurBlock->TbCondition[CurBlock->TbNConditions - 1].
			 TcTextValue[i] = inputLine[wi + i - 1];
		     CurBlock->TbCondition[CurBlock->TbNConditions - 1].
		       TcTextValue[wl - 1] = '\0';
		   }
		 else
		   {
		     pAttrTrans = pTSchema->TsAttrTRule->TsAttrTransl[CurAttr - 1];
		     for (i = 0; i < wl - 1; i++)
		       pAttrTrans->AtrTextValue[i] = inputLine[wi + i - 1];
		     pAttrTrans->AtrTextValue[wl - 1] = '\0';
		   }
		 break;

	       case RULE_FileName:	/* FileName */
		 NewConstant (wl, wi);
		 if (!error)
		   {
		     CurTRule->TrBufOrConst = ToConst;
		     CurTRule->TrInclFile = pTSchema->TsNConstants;
		   }
		 break;
		 
	       case RULE_Source:	/* Source */
		 if (wl > MAX_SRCE_LEN)
		   /* chaine source trop longue */
		   CompilerMessage (wi, TRA, FATAL, MAX_STRING_SIZE_OVERFLOW,
				    inputLine, LineNum);
		 else if ((SymbTrans || GraphTrans) && wl > 2)
		   /* dans les traductions de symboles et de graphiques */
		   /* la chaine source ne peut contenir qu'un caractere */
		   CompilerMessage (wi + 1, TRA, FATAL, ONLY_ONE_CHARACTER,
				    inputLine, LineNum);
		 else
		   NewSourceString (wi, wl);
		 break;

	       case RULE_Target:	/* Target */
		 if (wl > MAX_TARGET_LEN)
		   /* chaine cible trop longue */
		   CompilerMessage (wi, TRA, FATAL, MAX_STRING_SIZE_OVERFLOW,
				    inputLine, LineNum);
		 else
		   ProcessTargetString (wi, wl);
		 break;

	       case RULE_LineEndString:
		 /* chaine de fin de ligne */
		 pTSchema->TsEOL[0] = inputLine[wi - 1];
		 break;
		 
	       case RULE_LineEndInsertString:
		 /* chaine de fin de ligne */
		 if (wl <= MAX_NAME_LENGTH)
		   for (i = 0; i < wl - 1; i++)
		     pTSchema->TsTranslEOL[i] = inputLine[wi + i - 1];
		 pTSchema->TsTranslEOL[i] = '\0';
		 break;
		 
	       default:
		 break;
	       }
	     break;
	   }
     }
}


/*----------------------------------------------------------------------
   main                                                            
  ----------------------------------------------------------------------*/
#ifdef _WINGUI
int TRAmain (HWND hwnd, HWND statusBar, int argc, char **argv, int *Y)
#else  /* _WINGUI */
int main (int argc, char **argv)
#endif /* _WINGUI */
{
   FILE               *infile;
   Name                srceFileName;	/* nom du fichier a compiler */
   indLine             wi; /* position du debut du mot courant dans la ligne */
   indLine             wl; /* longueur du mot courant */
   SyntacticType       wn; /* SyntacticType du mot courant */
   SyntRuleNum         r;  /* numero de regle */
   SyntRuleNum         pr; /* numero de la regle precedente */
   SyntacticCode       c;  /* code grammatical du mot trouve */
   char                fname[200], buffer[200];
   char               *pwd, *ptr;
   int                 nb; /* index within the identifier */
   int                 i;
   int                 param;
#ifdef _WINGUI
   char               *CMD;
   char               *cmd[100];
   int                 ndx, pIndex = 0;
   char                msg[800];
   HANDLE              cppLib;
   /* FARPROC             ptrMainProc; */
   typedef int (*MYPROC) (HWND, int, char **, int *);
   MYPROC              ptrMainProc; 
#else  /* !_WINGUI */
   char                cmd[800];
#endif /* _WINGUI */
   ThotBool            fileOK;

#ifdef _WINGUI 
   COMPWnd = hwnd;
   compilersDC = GetDC (hwnd);
   _CY_ = *Y;
   strcpy (msg, "Executing tra ");
   for (ndx = 1; ndx < argc; ndx++)
     {
       strcat (msg, argv [ndx]);
       strcat (msg, " ");
     }

   TtaDisplayMessage (INFO, msg);

   SendMessage (statusBar, SB_SETTEXT, (WPARAM) 0, (LPARAM) &msg[0]);
   SendMessage (statusBar, WM_PAINT, (WPARAM) 0, (LPARAM) 0);
#endif /* _WINGUI */

   TtaInitializeAppRegistry (argv[0]);
   i = TtaGetMessageTable ("libdialogue", TMSG_LIB_MSG_MAX);
   COMPIL = TtaGetMessageTable ("compildialogue", COMP_MSG_MAX);
   TRA = TtaGetMessageTable ("tradialogue", TRA_MSG_MAX);

   error = False;
   /* initialise l'analyseur syntaxique */
   InitParser ();
   InitSyntax ("TRANS.GRM");
   if (!error)
     {
       /* prepare the cpp command */
#ifdef _WINGUI
       cmd [pIndex] = (char *) TtaGetMemory (4);
       strcpy (cmd [pIndex++], "cpp");
#else  /* !_WINGUI */
       strcpy (cmd, CPP " ");
#endif /* _WINGUI */
       param = 1;
       while (param < argc && argv[param][0] == '-')
	 {
	   /* keep cpp params */
#ifdef _WINGUI
	   cmd [pIndex] = (char *) TtaGetMemory (strlen (argv[param]) + 1);
	   strcpy (cmd [pIndex++], argv[param]);
#else  /* !_WINGUI */
	   strcat (cmd, argv[param]);
	   strcat (cmd, " ");
#endif /* _WINGUI */
	   param++;
	 }

       /* recupere d'abord le nom du schema a compiler */
       if (param >= argc)
	 {
	   TtaDisplaySimpleMessage (FATAL, TRA, MISSING_FILE);
#ifdef _WINGUI 
	   ReleaseDC (hwnd, compilersDC);
	   return FATAL_EXIT_CODE;
#else  /* _WINGUI */
	   exit (1);
#endif /* _WINGUI */
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
	 strcat (srceFileName, ".T");
       else if (strcmp(ptr, ".T"))
	 {
	   /* it's not the valid suffix */
	   TtaDisplayMessage (FATAL, TtaGetMessage (TRA, INVALID_FILE),
			      srceFileName);
#ifdef _WINGUI 
	   ReleaseDC (hwnd, compilersDC);
	   return FATAL_EXIT_CODE;
#else  /* _WINGUI */
	   exit (1);
#endif /* _WINGUI */
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
       if (!TtaFileExist(srceFileName))
	 TtaDisplaySimpleMessage (FATAL, TRA, MISSING_FILE);
       else
	 {
	   /* provide the real source file */
	   TtaFileUnlink (fname);
	   pwd = TtaGetEnvString ("PWD");
#ifndef _WINGUI 
	   i = strlen (cmd);
#endif /* _WINGUI */
	   if (pwd != NULL)
	     {
#ifdef _WINGUI
	       CMD = (char *) TtaGetMemory (3 + strlen (pwd));
	       sprintf (CMD, "-I%s", pwd);
	       cmd [pIndex] = (char *) TtaGetMemory (3 + strlen (pwd));
	       strcpy (cmd [pIndex++], CMD);
	       cmd [pIndex] = (char *) TtaGetMemory (3);
	       strcpy (cmd [pIndex++], "-C");
	       cmd [pIndex] = (char *) TtaGetMemory (strlen (srceFileName) + 1);
	       strcpy (cmd [pIndex++], srceFileName);
	       cmd [pIndex] = (char *) TtaGetMemory (strlen (fname) + 1);
	       strcpy (cmd [pIndex++], fname);
#else  /* !_WINGUI */
	       sprintf (&cmd[i], "-I%s -C %s > %s", pwd, srceFileName, fname);
#endif /* _WINGUI */
	     }
	   else
	     {
#ifdef _WINGUI
               cmd [pIndex] = (char *) TtaGetMemory (3);
               strcpy (cmd [pIndex++], "-C");
               cmd [pIndex] = (char *) TtaGetMemory (strlen (srceFileName) + 1);
               strcpy (cmd [pIndex++], srceFileName);
               cmd [pIndex] = (char *) TtaGetMemory (strlen (fname) + 2);
               strcpy (cmd [pIndex++], fname);
#else  /* !_WINGUI */
               sprintf (&cmd[i], "-C %s > %s", srceFileName, fname);
#endif /* _WINGUI */
	     } 
#ifdef _WINGUI
	   cppLib = LoadLibrary ("cpp");
	   ptrMainProc = (MYPROC) GetProcAddress ((HMODULE)cppLib, "CPPmain");
	   i = ptrMainProc (hwnd, pIndex, cmd, &_CY_);
	   FreeLibrary ((HMODULE)cppLib);
	   for (ndx = 0; ndx < pIndex; ndx++) {
	     free (cmd [ndx]);
	     cmd [ndx] = (char*) 0;
	   }
#else  /* !_WINGUI */
	   i = system (cmd);
#endif /* _WINGUI */
	   if (i == FATAL_EXIT_CODE)
	     {
	       /* cpp is not available, copy directely the file */
	       TtaDisplaySimpleMessage (INFO, TRA, CPP_NOT_FOUND);
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
	   LineNum = 0;
	   Initialize ();	/* prepare la generation */
	    
	   /* lit tout le fichier et fait l'analyse */
	   fileOK = True;
	   while (fileOK && !error)
	     /* lit une ligne */
	     {
	       i = 0;
	       do
		 {
		   fileOK = TtaReadByte (infile, &inputLine[i]);
		   i++;
		 }
	       while (i < LINE_LENGTH && inputLine[i - 1] != '\n' && fileOK);
	       /* marque la fin reelle de la ligne */
	       inputLine[i - 1] = '\0';
	       /* incremente le compteur de lignes */
	       LineNum++;
	       if (i >= LINE_LENGTH)
		  CompilerMessage (1, TRA, FATAL, MAX_LINE_SIZE_OVERFLOW,
				   inputLine, LineNum);
	       else if (inputLine[0] == '#')
		 /* cette ligne contient une directive du preprocesseur cpp */
		 {
		   sscanf ((char *)inputLine, "# %d %s", &LineNum, buffer);
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
			    AnalyzeToken (wi, wl, wn, &c, &r, &nb, &pr);
			    /* on analyse le mot */
			    if (!error)
			      ProcessToken (wi, wl, c, r, nb, pr);
			    /* on le traite */
			 }
		     }
		   while (wi != 0 && !error);
		 }	/* il n'y a plus de mots dans la ligne */
	     }
	   TtaReadClose (infile);
	    if (!error)
	      ParserEnd ();	/* fin d'analyse */
	    if (!error)
	      {
		/* remove temporary file */
		TtaFileUnlink (fname);
		/* ecrit le schema compile' dans le fichier de sortie */
		/* le directory des schemas est le directory courant */
		strcat (srceFileName, ".TRA");
		fileOK = WriteTranslationSchema (srceFileName, pTSchema,
						 pSSchema);
		if (!fileOK)
		  TtaDisplayMessage (FATAL, TtaGetMessage (TRA, CANT_WRITE),
				     srceFileName);
	      }
	    for (i = 0; i < pSSchema->SsNAttributes; i++)
	      free (pTSchema->TsAttrTRule->TsAttrTransl[i]);
	    FreeSchTra (pTSchema, pSSchema);
	    FreeSchStruc (pSSchema);
	    FreeSchStruc (pExtSSchema);
	 }
     }
   TtaSaveAppRegistry ();
#ifdef _WINGUI 
   *Y = _CY_;
   ReleaseDC (hwnd, compilersDC);
   return COMP_SUCCESS;
#else  /* _WINGUI */
   exit (0);
#endif /* _WINGUI */
}
