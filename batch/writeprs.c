
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

/* ======================================================================= */
/* |                                                                    | */
/* |                           Projet THOT                              | */
/* |                                                                    | */
/* |    Ce module sauve un schema de presentation compile'.             | */
/* |                                                                    | */
/* |                                                                    | */
/* |                    V. Quint        Juin 1984                       | */
/* |                                                                    | */
/* ======================================================================= */


#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "constpiv.h"
#include "storage.h"

static BinFile      outfile;

#include "storage.f"
#include "wrschprs.f"

/* ---------------------------------------------------------------------- */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         wrshort (int n)
#else  /* __STDC__ */
static void         wrshort (n)
int                 n;

#endif /* __STDC__ */
{
   BIOwriteByte (outfile, (char) (n / 256));
   BIOwriteByte (outfile, (char) (n % 256));
}

/* ---------------------------------------------------------------------- */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         wrsignshort (int n)
#else  /* __STDC__ */
static void         wrsignshort (n)
int             n;

#endif /* __STDC__ */
{
   if (n >= 0)
      wrshort (n);
   else
      wrshort (n + 65536);
}

/* ---------------------------------------------------------------------- */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         wrnom (Name n)
#else  /* __STDC__ */
static void         wrnom (n)
Name                 n;

#endif /* __STDC__ */
{
   int                 i;

   i = 0;
   do
     {
	i++;
	BIOwriteByte (outfile, n[i - 1]);
     }
   while (!(n[i - 1] == '\0'));
}


/* ---------------------------------------------------------------------- */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         wrptrreg (PtrPRule p)
#else  /* __STDC__ */
static void         wrptrreg (p)
PtrPRule        p;

#endif /* __STDC__ */
{
   if (p == NULL)
      BIOwriteByte (outfile, '\0');
   else
      BIOwriteByte (outfile, '\1');
}


/* ---------------------------------------------------------------------- */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         wrbool (boolean b)
#else  /* __STDC__ */
static void         wrbool (b)
boolean             b;

#endif /* __STDC__ */
{
   if (b)
      BIOwriteByte (outfile, '\1');
   else
      BIOwriteByte (outfile, '\0');
}

/* ---------------------------------------------------------------------- */
/* | wrUnit ecrit une unite dans le fichier sur un octet                | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         wrUnit (TypeUnit unit)
#else  /* __STDC__ */
static void         wrUnit (unit)
BinFile             file;
TypeUnit            unit;

#endif /* __STDC__ */
{
   switch (unit)
	 {
	    case UnRelative:
	       BIOwriteByte (outfile, C_UNIT_REL);
	       break;
	    case UnXHeight:
	       BIOwriteByte (outfile, C_UNIT_XHEIGHT);
	       break;
	    case UnPoint:
	       BIOwriteByte (outfile, C_UNIT_POINT);
	       break;
	    case UnPixel:
	       BIOwriteByte (outfile, C_UNIT_PIXEL);
	       break;
	    case UnPercent:
	       BIOwriteByte (outfile, C_UNIT_PERCENT);
	       break;
	    default:
	       fprintf (stderr, "Invalid unit %X\n", unit);
	       BIOwriteByte (outfile, C_UNIT_REL);
	       break;
	 }
}

/* ---------------------------------------------------------------------- */
/* wrTypeRegle  ecrit un type de regle de presentation dans le fichier */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                wrTypeRegle (PRuleType T)
#else  /* __STDC__ */
void                wrTypeRegle (T)
PRuleType           T;

#endif /* __STDC__ */
{
   switch (T)
	 {
	    case PtVisibility:
	       BIOwriteByte (outfile, C_PR_VISIBILITY);
	       break;
	    case PtFunction:
	       BIOwriteByte (outfile, C_PR_FUNCTION);
	       break;
	    case PtVertRef:
	       BIOwriteByte (outfile, C_PR_VREF);
	       break;
	    case PtHorizRef:
	       BIOwriteByte (outfile, C_PR_HREF);
	       break;
	    case PtHeight:
	       BIOwriteByte (outfile, C_PR_HEIGHT);
	       break;
	    case PtWidth:
	       BIOwriteByte (outfile, C_PR_WIDTH);
	       break;
	    case PtVertPos:
	       BIOwriteByte (outfile, C_PR_VPOS);
	       break;
	    case PtHorizPos:
	       BIOwriteByte (outfile, C_PR_HPOS);
	       break;
	    case PtSize:
	       BIOwriteByte (outfile, C_PR_SIZE);
	       break;
	    case PtUnderline:
	       BIOwriteByte (outfile, C_PR_UNDERLINE);
	       break;
	    case PtThickness:
	       BIOwriteByte (outfile, C_PR_UNDER_THICK);
	       break;
	    case PtStyle:
	       BIOwriteByte (outfile, C_PR_STYLE);
	       break;
	    case PtFont:
	       BIOwriteByte (outfile, C_PR_FONT);
	       break;
	    case PtBreak1:
	       BIOwriteByte (outfile, C_PR_BREAK1);
	       break;
	    case PtBreak2:
	       BIOwriteByte (outfile, C_PR_BREAK2);
	       break;
	    case PtIndent:
	       BIOwriteByte (outfile, C_PR_INDENT);
	       break;
	    case PtLineSpacing:
	       BIOwriteByte (outfile, C_PR_LINESPACING);
	       break;
	    case PtAdjust:
	       BIOwriteByte (outfile, C_PR_ADJUST);
	       break;
	    case PtJustify:
	       BIOwriteByte (outfile, C_PR_JUSTIFY);
	       break;
	    case PtHyphenate:
	       BIOwriteByte (outfile, C_PR_HYPHENATE);
	       break;
	    case PtDepth:
	       BIOwriteByte (outfile, C_PR_DEPTH);
	       break;
	    case PtLineStyle:
	       BIOwriteByte (outfile, C_PR_LINESTYLE);
	       break;
	    case PtLineWeight:
	       BIOwriteByte (outfile, C_PR_LINEWEIGHT);
	       break;
	    case PtFillPattern:
	       BIOwriteByte (outfile, C_PR_FILLPATTERN);
	       break;
	    case PtBackground:
	       BIOwriteByte (outfile, C_PR_BACKGROUND);
	       break;
	    case PtForeground:
	       BIOwriteByte (outfile, C_PR_FOREGROUND);
	       break;
	    default:
	       fprintf (stderr, "Invalid rule type %X\n", T);
	       break;
	 }
}


/* wrModeCalcul ecrit un mode de calcul dans le fichier */
#ifdef __STDC__
static void         wrModeCalcul (PresMode M)

#else  /* __STDC__ */
static void         wrModeCalcul (M)
PresMode          M;

#endif /* __STDC__ */
{
   switch (M)
	 {
	    case PresImmediate:
	       BIOwriteByte (outfile, C_IMMEDIATE);
	       break;
	    case PresInherit:
	       BIOwriteByte (outfile, C_INHERIT);
	       break;
	    case PresFunction:
	       BIOwriteByte (outfile, C_PRES_FUNCTION);
	       break;
	    default:
	       fprintf (stderr, "Invalid mode %X\n", M);
	       break;
	 }
}


/* wrTypeHeritage       ecrit un type d'heritage dans le fichier */
#ifdef __STDC__
static void         wrTypeHeritage (InheritMode T)
#else  /* __STDC__ */
static void         wrTypeHeritage (T)
InheritMode        T;

#endif /* __STDC__ */
{
   switch (T)
	 {
	    case InheritParent:
	       BIOwriteByte (outfile, C_INH_ASCEND);
	       break;
	    case InheritPrevious:
	       BIOwriteByte (outfile, C_INH_PREVIOUS);
	       break;
	    case InheritChild:
	       BIOwriteByte (outfile, C_INH_DESC);
	       break;
	    case InheritCreator:
	       BIOwriteByte (outfile, C_INH_CREATOR);
	       break;
	    case InheritGrandFather:
	       BIOwriteByte (outfile, C_INH_GRAND_FATHER);
	       break;
	    default:
	       fprintf (stderr, "Invalid inherit %X\n", T);
	       break;
	 }
}


/* wrTypeFonct  ecrit un type de fonction de presentation dans le */
/* fichier */
#ifdef __STDC__
static void         wrTypeFonct (FunctionType T, boolean rep)

#else  /* __STDC__ */
static void         wrTypeFonct (T, rep)
FunctionType           T;
boolean             rep;

#endif /* __STDC__ */
{
   switch (T)
	 {
	    case FnLine:
	       BIOwriteByte (outfile, C_PF_LINE);
	       break;
	    case FnPage:
	       BIOwriteByte (outfile, C_PF_PAGE);
	       break;
	    case FnCreateBefore:
	       if (rep)
		  BIOwriteByte (outfile, C_PF_CR_BEFORE_REP);
	       else
		  BIOwriteByte (outfile, C_PF_CR_BEFORE);
	       break;
	    case FnCreateWith:
	       BIOwriteByte (outfile, C_PF_CR_WITH);
	       break;
	    case FnCreateFirst:
	       if (rep)
		  BIOwriteByte (outfile, C_PF_CR_FIRST_REP);
	       else
		  BIOwriteByte (outfile, C_PF_CR_FIRST);
	       break;
	    case FnCreateLast:
	       if (rep)
		  BIOwriteByte (outfile, C_PF_CR_LAST_REP);
	       else
		  BIOwriteByte (outfile, C_PF_CR_LAST);
	       break;
	    case FnCreateAfter:
	       if (rep)
		  BIOwriteByte (outfile, C_PF_CR_AFTER_REP);
	       else
		  BIOwriteByte (outfile, C_PF_CR_AFTER);
	       break;
	    case FnCreateEnclosing:
	       if (rep)
		  BIOwriteByte (outfile, C_PF_CR_ENCLOSING_REP);
	       else
		  BIOwriteByte (outfile, C_PF_CR_ENCLOSING);
	       break;
	    case FnColumn:
	       BIOwriteByte (outfile, C_PF_COLUMN);
	       break;
	    case FnSubColumn:
	       BIOwriteByte (outfile, C_PF_SUBCOLUMN);
	       break;
	    case FnCopy:
	       BIOwriteByte (outfile, C_PF_COPY);
	       break;
	    case FnContentRef:
	       BIOwriteByte (outfile, C_PF_REF_CONTENT);
	       break;
	    case FnNoLine:
	       BIOwriteByte (outfile, C_PF_NOLINE);
	       break;
	    default:
	       fprintf (stderr, "Invalid function %X\n", T);
	       break;
	 }
}


/* wrCadrage    ecrit un mode de cadrage de lignes dans le fichier */
#ifdef __STDC__
static void         wrCadrage (BAlignment C)
#else  /* __STDC__ */
static void         wrCadrage (C)
BAlignment             C;

#endif /* __STDC__ */
{
   switch (C)
	 {
	    case AlignLeft:
	       BIOwriteByte (outfile, C_PIV_LEFT);
	       break;
	    case AlignRight:
	       BIOwriteByte (outfile, C_PIV_RIGHT);
	       break;
	    case AlignCenter:
	       BIOwriteByte (outfile, C_PIV_CENTERED);
	       break;
	    case AlignLeftDots:
	       BIOwriteByte (outfile, C_PIV_LEFTDOT);
	       break;
	    default:
	       fprintf (stderr, "Invalid alignment %X\n", C);
	       break;
	 }
}


/* wrTypeCondition      ecrit un type de condition dans le fichier */
#ifdef __STDC__
void                wrTypeCondition (PresCondition T)
#else  /* __STDC__ */
void                wrTypeCondition (T)
PresCondition       T;

#endif /* __STDC__ */
{
   switch (T)
	 {
	    case PcFirst:
	       BIOwriteByte (outfile, C_COND_FIRST);
	       break;
	    case PcLast:
	       BIOwriteByte (outfile, C_COND_LAST);
	       break;
	    case PcReferred:
	       BIOwriteByte (outfile, C_COND_REFERRED);
	       break;
	    case PcFirstRef:
	       BIOwriteByte (outfile, C_COND_FIRSTREF);
	       break;
	    case PcLastRef:
	       BIOwriteByte (outfile, C_COND_LASTREF);
	       break;
	    case PcExternalRef:
	       BIOwriteByte (outfile, C_COND_EXTREF);
	       break;
	    case PcInternalRef:
	       BIOwriteByte (outfile, C_COND_INTREF);
	       break;
	    case PcCopyRef:
	       BIOwriteByte (outfile, C_COND_COPYREF);
	       break;
	    case PcAnyAttributes:
	       BIOwriteByte (outfile, C_COND_ATTR);
	       break;
	    case PcFirstAttr:
	       BIOwriteByte (outfile, C_COND_FIRST_ATTR);
	       break;
	    case PcLastAttr:
	       BIOwriteByte (outfile, C_COND_LAST_ATTR);
	       break;
	    case PcUserPage:
	       BIOwriteByte (outfile, C_COND_USERPAGE);
	       break;
	    case PcStartPage:
	       BIOwriteByte (outfile, C_COND_STARTPAGE);
	       break;
	    case PcComputedPage:
	       BIOwriteByte (outfile, C_COND_COMPPAGE);
	       break;
	    case PcEmpty:
	       BIOwriteByte (outfile, C_COND_EMPTY);
	       break;
	    case PcEven:
	       BIOwriteByte (outfile, C_COND_EVEN);
	       break;
	    case PcOdd:
	       BIOwriteByte (outfile, C_COND_ODD);
	       break;
	    case PcOne:
	       BIOwriteByte (outfile, C_COND_ONE);
	       break;
	    case PcInterval:
	       BIOwriteByte (outfile, C_COND_INTER);
	       break;
	    case PcWithin:
	       BIOwriteByte (outfile, C_COND_ANCEST);
	       break;
	    case PcElemType:
	       BIOwriteByte (outfile, C_COND_ELEM);
	       break;
	    case PcAttribute:
	       BIOwriteByte (outfile, C_COND_HAS_ATTR);
	       break;
	    case PcNoCondition:
	       BIOwriteByte (outfile, C_COND_NOCOND);
	       break;
	    case PcDefaultCond:
	       BIOwriteByte (outfile, C_COND_DEFAULT);
	       break;
	    default:
	       fprintf (stderr, "Invalid condition %X\n", T);
	       break;
	 }
}


/* wrNatValCmpt ecrit la nature de la condition sur un compteur */
#ifdef __STDC__
static void         wrNatValCmpt (CounterValue T)
#else  /* __STDC__ */
static void         wrNatValCmpt (T)
CounterValue      T;

#endif /* __STDC__ */
{
   switch (T)
	 {
	    case CntMaxVal:
	       BIOwriteByte (outfile, C_VAL_MAX);
	       break;
	    case CntMinVal:
	       BIOwriteByte (outfile, C_VAL_MIN);
	       break;
	    case CntCurVal:
	       BIOwriteByte (outfile, C_VAL_CUR);
	       break;
	    default:
	       fprintf (stderr, "Invalid counter cond. %X\n", T);
	       break;
	 }
}


/* wrRelationParent     ecrit la relation de la condition Within */
#ifdef __STDC__
static void         wrRelationParent (ArithRel R)
#else  /* __STDC__ */
static void         wrRelationParent (R)
ArithRel          R;

#endif /* __STDC__ */
{
   switch (R)
	 {
	    case CondGreater:
	       BIOwriteByte (outfile, C_WITHIN_GT);
	       break;
	    case CondLess:
	       BIOwriteByte (outfile, C_WITHIN_LT);
	       break;
	    case CondEquals:
	       BIOwriteByte (outfile, C_WITHIN_EQ);
	       break;
	    default:
	       fprintf (stderr, "Invalid relationship %X\n", R);
	       break;
	 }
}

/* wrRepereBoite                ecrit un repere de boite dans le fichier */
#ifdef __STDC__
static void         wrRepereBoite (BoxEdge R)
#else  /* __STDC__ */
static void         wrRepereBoite (R)
BoxEdge         R;

#endif /* __STDC__ */
{
   switch (R)
	 {
	    case Top:
	       BIOwriteByte (outfile, C_AX_TOP);
	       break;
	    case Bottom:
	       BIOwriteByte (outfile, C_AX_BOTTOM);
	       break;
	    case Left:
	       BIOwriteByte (outfile, C_AX_LEFT);
	       break;
	    case Right:
	       BIOwriteByte (outfile, C_AX_RIGHT);
	       break;
	    case HorizRef:
	       BIOwriteByte (outfile, C_AX_HREF);
	       break;
	    case VertRef:
	       BIOwriteByte (outfile, C_AX_VREF);
	       break;
	    case HorizMiddle:
	       BIOwriteByte (outfile, C_AX_HMIDDLE);
	       break;
	    case VertMiddle:
	       BIOwriteByte (outfile, C_AX_VMIDDLE);
	       break;
	    case NoEdge:
	       BIOwriteByte (outfile, C_AX_NULL);
	       break;
	    default:
	       fprintf (stderr, "Invalid side %X\n", R);
	       break;
	 }
}


/* wrNiveau     ecrit un niveau relatif de boite dans le fichier */
#ifdef __STDC__
static void         wrNiveau (Level N)
#else  /* __STDC__ */
static void         wrNiveau (N)
Level              N;

#endif /* __STDC__ */
{
   switch (N)
	 {
	    case RlEnclosing:
	       BIOwriteByte (outfile, C_PARENT);
	       break;
	    case RlSameLevel:
	       BIOwriteByte (outfile, C_SAME_LEVEL);
	       break;
	    case RlEnclosed:
	       BIOwriteByte (outfile, C_CHILD);
	       break;
	    case RlPrevious:
	       BIOwriteByte (outfile, C_PREVIOUS);
	       break;
	    case RlNext:
	       BIOwriteByte (outfile, C_NEXT);
	       break;
	    case RlSelf:
	       BIOwriteByte (outfile, C_SELF);
	       break;
	    case RlContainsRef:
	       BIOwriteByte (outfile, C_CONTAINS_REF);
	       break;
	    case RlRoot:
	       BIOwriteByte (outfile, C_ROOT);
	       break;
	    case RlReferred:
	       BIOwriteByte (outfile, C_REFERRED);
	       break;
	    case RlCreator:
	       BIOwriteByte (outfile, C_CREATOR);
	       break;
	    default:
	       fprintf (stderr, "Invalid level %X\n", N);
	       break;
	 }
}


/* wrCptTypeOp  ecrit un type d'operation sur compteur dans le fichier */
#ifdef __STDC__
static void         wrCptTypeOp (CounterOp T)
#else  /* __STDC__ */
static void         wrCptTypeOp (T)
CounterOp           T;

#endif /* __STDC__ */
{
   switch (T)
	 {
	    case CntrSet:
	       BIOwriteByte (outfile, C_CNT_SET);
	       break;
	    case CntrAdd:
	       BIOwriteByte (outfile, C_CNT_ADD);
	       break;
	    case CntrRank:
	       BIOwriteByte (outfile, C_CNT_RANK);
	       break;
	    case CntrRLevel:
	       BIOwriteByte (outfile, C_CNT_RLEVEL);
	       break;
	    default:
	       fprintf (stderr, "Invalid operator %X\n", T);
	       break;
	 }
}


/* wrTypeDeBase ecrit un type de base dans le fichier */
#ifdef __STDC__
static void         wrTypeDeBase (BasicType T)
#else  /* __STDC__ */
static void         wrTypeDeBase (T)
BasicType          T;

#endif /* __STDC__ */
{
   switch (T)
	 {
	    case CharString:
	       BIOwriteByte (outfile, C_CHAR_STRING);
	       break;
	    case GraphicElem:
	       BIOwriteByte (outfile, C_GRAPHICS);
	       break;
	    case Symbol:
	       BIOwriteByte (outfile, C_SYMBOL);
	       break;
	    case Picture:
	       BIOwriteByte (outfile, C_PICTURE);
	       break;
	    case Refer:
	       BIOwriteByte (outfile, C_REFER);
	       break;
	    case PageBreak:
	       BIOwriteByte (outfile, C_PAGE_BREAK);
	       break;
	    default:
	       fprintf (stderr, "Invalid type %X\n", T);
	       break;
	 }
}


/* wrVarType    ecrit un type de variable dans le fichier */
#ifdef __STDC__
void                wrVarType (VariableType T)
#else  /* __STDC__ */
void                wrVarType (T)
VariableType             T;

#endif /* __STDC__ */
{
   switch (T)
	 {
	    case VarText:
	       BIOwriteByte (outfile, C_VAR_TEXT);
	       break;
	    case VarCounter:
	       BIOwriteByte (outfile, C_VAR_COUNTER);
	       break;
	    case VarAttrValue:
	       BIOwriteByte (outfile, C_VAR_ATTR_VAL);
	       break;
	    case VarDate:
	       BIOwriteByte (outfile, C_VAR_DATE);
	       break;
	    case VarFDate:
	       BIOwriteByte (outfile, C_VAR_FDATE);
	       break;
	    case VarDirName:
	       BIOwriteByte (outfile, C_VAR_DIRNAME);
	       break;
	    case VarDocName:
	       BIOwriteByte (outfile, C_VAR_DOCNAME);
	       break;
	    case VarElemName:
	       BIOwriteByte (outfile, C_VAR_ELEMNAME);
	       break;
	    case VarAttrName:
	       BIOwriteByte (outfile, C_VAR_ATTRNAME);
	       break;
	    case VarPageNumber:
	       BIOwriteByte (outfile, C_VAR_PAGENUMBER);
	       break;
	    default:
	       fprintf (stderr, "Invalid variable %X\n", T);
	       break;
	 }
}


/* wrComptStyle ecrit un style de compteur dans le fichier */
#ifdef __STDC__
static void         wrComptStyle (CounterStyle S)
#else  /* __STDC__ */
static void         wrComptStyle (S)
CounterStyle          S;

#endif /* __STDC__ */
{
   switch (S)
	 {
	    case CntArabic:
	       BIOwriteByte (outfile, C_NUM_ARABIC);
	       break;
	    case CntURoman:
	       BIOwriteByte (outfile, C_NUM_ROMAN);
	       break;
	    case CntLRoman:
	       BIOwriteByte (outfile, C_NUM_LOWER_ROMAN);
	       break;
	    case CntUppercase:
	       BIOwriteByte (outfile, C_NUM_UPPERCASE);
	       break;
	    case CntLowercase:
	       BIOwriteByte (outfile, C_NUM_LOWERCASE);
	       break;
	    default:
	       fprintf (stderr, "Invalid style %X\n", S);
	       break;
	 }
}


/* wrTypeContenu                ecrit un type de contenu dans le fichier */
#ifdef __STDC__
void                wrTypeContenu (ContentType T)
#else  /* __STDC__ */
void                wrTypeContenu (T)
ContentType         T;

#endif /* __STDC__ */
{
   switch (T)
	 {
	    case FreeContent:
	       BIOwriteByte (outfile, C_CONT_FREE);
	       break;
	    case ContVariable:
	       BIOwriteByte (outfile, C_CONT_VAR);
	       break;
	    case ContConst:
	       BIOwriteByte (outfile, C_CONT_CONST);
	       break;
	    case ContElement:
	       BIOwriteByte (outfile, C_CONT_ELEM);
	       break;
	    default:
	       fprintf (stderr, "Invalid content %X\n", T);
	       break;
	 }
}


/* wrCompAttr    un type de comparaison de valeur d'attribut
 * dans le fichier */
#ifdef __STDC__
static void         wrComparAttr (AttrComparType C)
#else  /* __STDC__ */
static void         wrComparAttr (C)
AttrComparType      C;

#endif /* __STDC__ */
{
   switch (C)
	 {
	    case ComparConstant:
	       BIOwriteByte (outfile, C_COMP_CONST);
	       break;
	    case ComparAttr:
	       BIOwriteByte (outfile, C_COMP_ATTR);
	       break;
	    default:
	       fprintf (stderr, "Invalid comparison %X\n", C);
	       break;
	 }
}

/* ---------------------------------------------------------------------- */
/* wrPosition   ecrit un positionnement relatif */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         wrPosition_WriteRules (PosRule RP)
#else  /* __STDC__ */
static void         wrPosition_WriteRules (RP)
PosRule            RP;

#endif /* __STDC__ */
{
   PosRule           *pRe1;

   pRe1 = &RP;
   wrRepereBoite (pRe1->PoPosDef);
   wrRepereBoite (pRe1->PoPosRef);
   wrUnit (pRe1->PoDistUnit);
   wrbool (pRe1->PoDistAttr);
   wrsignshort (pRe1->PoDistance);
   wrNiveau (pRe1->PoRelation);
   wrbool (pRe1->PoNotRel);
   wrbool (pRe1->PoUserSpecified);
   wrbool (pRe1->PoRefElem);
   if (pRe1->PoRefElem)
      wrshort (pRe1->PoTypeRefElem);
   else
      wrshort (pRe1->PoRefPresBox);
}


/* WriteRules   ecrit la chaine de regle de presentation qui */
/* commence par la regle pointee par pR. */
#ifdef __STDC__
void                WriteRules (PtrPRule pR)
#else  /* __STDC__ */
void                WriteRules (pR)
PtrPRule        pR;

#endif /* __STDC__ */
{
   PtrPRule        cR, nR;
   int                 i;
   PtrPRule        pRe1;
   PtrCondition        pCond;
   DimensionRule       *pRelD1;

   /* debut de la procedure WriteRules */
   cR = pR;
   while (cR != NULL)
     {
	pRe1 = cR;
	wrTypeRegle (pRe1->PrType);
	wrptrreg (pRe1->PrNextPRule);
	pCond = pRe1->PrCond;
	while (pCond != NULL)
	  {
	     wrTypeCondition (pCond->CoCondition);
	     wrbool (pCond->CoNotNegative);
	     wrbool (pCond->CoTarget);
	     switch (pCond->CoCondition)
		   {
		      case PcEven:
		      case PcOdd:
		      case PcOne:
			 wrshort (pCond->CoCounter);
			 break;
		      case PcInterval:
			 wrshort (pCond->CoCounter);
			 wrsignshort (pCond->CoMinCounter);
			 wrsignshort (pCond->CoMaxCounter);
			 wrNatValCmpt (pCond->CoValCounter);
			 break;
		      case PcWithin:
			 wrbool (pCond->CoImmediate);
			 wrshort (pCond->CoRelation);
			 wrRelationParent (pCond->CoAncestorRel);
			 wrsignshort (pCond->CoTypeAncestor);
			 if (pCond->CoTypeAncestor == 0)
			   {
			      wrnom (pCond->CoAncestorName);
			      wrnom (pCond->CoSSchemaName);
			   }
			 break;
		      case PcElemType:
		      case PcAttribute:
			 wrsignshort (pCond->CoTypeElAttr);
			 break;
		      default:
			 break;
		   }
	     pCond = pCond->CoNextCondition;
	  }
	wrTypeCondition (PcNoCondition);
	wrshort (pRe1->PrViewNum);
	wrModeCalcul (pRe1->PrPresMode);
	switch (pRe1->PrPresMode)
	      {
		 case PresInherit:
		    wrTypeHeritage (pRe1->PrInheritMode);
		    wrbool (pRe1->PrInhAttr);
		    wrsignshort (pRe1->PrInhDelta);
		    wrbool (pRe1->PrMinMaxAttr);
		    wrsignshort (pRe1->PrInhMinOrMax);
		    wrUnit (pRe1->PrInhUnit);
		    break;
		 case PresFunction:
		    wrTypeFonct (pRe1->PrPresFunction, pRe1->PrPresBoxRepeat);
		    if (pRe1->PrPresFunction != FnLine
			&& pRe1->PrPresFunction != FnNoLine)
		      {
			 wrbool (pRe1->PrExternal);
			 wrbool (pRe1->PrElement);
			 wrshort (pRe1->PrNPresBoxes);
			 if (pRe1->PrNPresBoxes == 0)
			    wrnom (pRe1->PrPresBoxName);
			 else
			    for (i = 1; i <= pRe1->PrNPresBoxes; i++)
			       wrshort (pRe1->PrPresBox[i - 1]);
		      }
		    break;
		 case PresImmediate:
		    switch (pRe1->PrType)
			  {
			     case PtVisibility:
			     case PtDepth:
			     case PtFunction:
			     case PtFillPattern:
			     case PtBackground:
			     case PtForeground:
				wrbool (pRe1->PrAttrValue);
				wrsignshort (pRe1->PrIntValue);
				break;
			     case PtFont:
			     case PtStyle:
			     case PtUnderline:
			     case PtThickness:
			     case PtLineStyle:
				BIOwriteByte (outfile, pRe1->PrChrValue);
				break;
			     case PtBreak1:
			     case PtBreak2:
			     case PtIndent:
			     case PtSize:
			     case PtLineSpacing:
			     case PtLineWeight:
				wrUnit (pRe1->PrMinUnit);
				wrbool (pRe1->PrMinAttr);
				wrsignshort (pRe1->PrMinValue);
				break;
			     case PtVertRef:
			     case PtHorizRef:
			     case PtVertPos:
			     case PtHorizPos:
				wrPosition_WriteRules (pRe1->PrPosRule);
				break;
			     case PtHeight:
			     case PtWidth:
				pRelD1 = &pRe1->PrDimRule;
				wrbool (pRelD1->DrPosition);
				if (pRelD1->DrPosition)
				   wrPosition_WriteRules (pRelD1->DrPosRule);
				else
				  {
				     wrbool (pRelD1->DrAbsolute);
				     wrbool (pRelD1->DrSameDimens);
				     wrUnit (pRelD1->DrUnit);
				     wrbool (pRelD1->DrAttr);
				     wrbool (pRelD1->DrMin);
				     wrbool (pRelD1->DrUserSpecified);
				     wrsignshort (pRelD1->DrValue);
				     wrNiveau (pRelD1->DrRelation);
				     wrbool (pRelD1->DrNotRelat);
				     wrbool (pRelD1->DrRefElement);
				     if (pRelD1->DrRefElement)
					wrshort (pRelD1->DrTypeRefElem);
				     else
					wrshort (pRelD1->DrRefPresBox);
				  }
				break;
			     case PtAdjust:
				wrCadrage (pRe1->PrAdjust);
				break;
			     case PtJustify:
			     case PtHyphenate:
				wrbool (pRe1->PrJustify);
				break;
			     case PtPictInfo:
				break;
			  }

		    break;
	      }

	nR = pRe1->PrNextPRule;
	free (cR);
	cR = nR;
     }
}


/* WrSchPres    cree le fichier de sortie et y ecrit le schema de */
/* presentation */

#ifdef __STDC__
boolean             WrSchPres (Name fname, PtrPSchema pSchPres, PtrSSchema pSchStr)

#else  /* __STDC__ */
boolean             WrSchPres (fname, pSchPres, pSchStr)
Name                 fname;
PtrPSchema          pSchPres;
PtrSSchema        pSchStr;

#endif /* __STDC__ */

{
   int                 i, j, l;
   PtrPSchema          pSc1;
   Counter           *pCo1;
   CntrItem            *pCp1;
   PresConstant          *pPr1;
   PresVariable            *pPres1;
   PresVarItem            *pVa1;
   PresentationBox             *pBo1;
   AttributePres      *pRP1;
   NumAttrCase         *pCa1;

   /* cree le fichier */
   outfile = BIOwriteOpen (fname);
   if (outfile == 0)
      /* echec */
      return False;
   /* recopie le code d'identification du schema de structure */
   pSchPres->PsStructCode = pSchStr->SsCode;

   /* ecrit la partie fixe */
   pSc1 = pSchPres;
   wrnom (pSc1->PsStructName);
   wrshort (pSc1->PsStructCode);
   wrshort (pSc1->PsNViews);
   for (i = 1; i <= pSc1->PsNViews; i++)
      wrnom (pSc1->PsView[i - 1]);
   for (i = 1; i <= pSc1->PsNViews; i++)
      wrbool (pSc1->PsPaginatedView[i - 1]);
   /* significatif uniquement dans la V4 */
   for (i = 1; i <= pSc1->PsNViews; i++)
      wrbool (pSc1->PsColumnView[i - 1]);
   wrshort (pSc1->PsNPrintedViews);
   for (i = 1; i <= pSc1->PsNPrintedViews; i++)
     {
	wrbool (pSc1->PsPrintedView[i - 1].VpAssoc);
	wrshort (pSc1->PsPrintedView[i - 1].VpNumber);
     }
   for (i = 1; i <= pSc1->PsNViews; i++)
      wrbool (pSc1->PsExportView[i - 1]);
   wrshort (pSc1->PsNCounters);
   wrshort (pSc1->PsNConstants);
   wrshort (pSc1->PsNVariables);
   wrshort (pSc1->PsNPresentBoxes);
   wrptrreg (pSc1->PsFirstDefaultPRule);

   /* ecrit les compteurs */
   for (i = 1; i <= pSc1->PsNCounters; i++)
     {
	pCo1 = &pSc1->PsCounter[i - 1];
	wrshort (pCo1->CnNItems);
	for (j = 1; j <= pCo1->CnNItems; j++)
	  {
	     pCp1 = &pCo1->CnItem[j - 1];
	     wrCptTypeOp (pCp1->CiCntrOp);
	     wrshort (pCp1->CiElemType);
	     wrsignshort (pCp1->CiAscendLevel);
	     wrshort (pCp1->CiViewNum);
	     wrsignshort (pCp1->CiParamValue);
	     wrshort (pCp1->CiInitAttr);
	     wrshort (pCp1->CiReinitAttr);
	  }
	wrshort (pCo1->CnNPresBoxes);
	for (j = 1; j <= pCo1->CnNPresBoxes; j++)
	  {
	     wrshort (pCo1->CnPresBox[j - 1]);
	     wrbool (pCo1->CnMinMaxPresBox[j - 1]);
	  }
	wrshort (pCo1->CnNTransmAttrs);
	for (j = 1; j <= pCo1->CnNTransmAttrs; j++)
	  {
	     wrnom (pCo1->CnTransmAttr[j - 1]);
	     wrshort (pCo1->CnTransmSSchemaAttr[j - 1]);
	  }
	wrshort (pCo1->CnNCreators);
	for (j = 1; j <= pCo1->CnNCreators; j++)
	  {
	     wrshort (pCo1->CnCreator[j - 1]);
	     wrbool (pCo1->CnMinMaxCreator[j - 1]);
	  }
	for (j = 1; j <= pCo1->CnNCreators; j++)
	   wrbool (pCo1->CnPresBoxCreator[j - 1]);
	wrshort (pCo1->CnNCreatedBoxes);
	for (j = 1; j <= pCo1->CnNCreatedBoxes; j++)
	  {
	     wrshort (pCo1->CnCreatedBox[j - 1]);
	     wrbool (pCo1->CnMinMaxCreatedBox[j - 1]);
	  }
	wrbool (pCo1->CnPageFooter);
     }

   /* ecrit les constantes de presentation */
   for (i = 1; i <= pSc1->PsNConstants; i++)
     {
	pPr1 = &pSc1->PsConstant[i - 1];
	wrTypeDeBase (pPr1->PdType);
	BIOwriteByte (outfile, pPr1->PdAlphabet);
	j = 0;
	do
	  {
	     j++;
	     BIOwriteByte (outfile, pPr1->PdString[j - 1]);
	  }
	while (!(pPr1->PdString[j - 1] == '\0'));
     }

   /* ecrit les variables de presentation */
   for (i = 1; i <= pSc1->PsNVariables; i++)
     {
	pPres1 = &pSc1->PsVariable[i - 1];
	wrshort (pPres1->PvNItems);
	for (j = 1; j <= pPres1->PvNItems; j++)
	  {
	     pVa1 = &pPres1->PvItem[j - 1];
	     wrVarType (pVa1->ViType);
	     switch (pVa1->ViType)
		   {
		      case VarText:
			 wrshort (pVa1->ViConstant);
			 break;
		      case VarCounter:
			 wrshort (pVa1->ViCounter);
			 wrComptStyle (pVa1->ViStyle);
			 wrNatValCmpt (pVa1->ViCounterVal);
			 break;
		      case VarAttrValue:
			 wrshort (pVa1->ViAttr);
			 wrComptStyle (pVa1->ViStyle);

			 break;
		      case VarPageNumber:
			 wrshort (pVa1->ViView);
			 wrComptStyle (pVa1->ViStyle);
			 break;
		      default:
			 break;
		   }
	  }
     }

   /* ecrit les boites de presentation et numerotation */
   for (i = 1; i <= pSc1->PsNPresentBoxes; i++)
     {
	pBo1 = &pSc1->PsPresentBox[i - 1];
	wrnom (pBo1->PbName);
	wrptrreg (pBo1->PbFirstPRule);
	wrbool (pBo1->PbAcceptPageBreak);
	wrbool (pBo1->PbAcceptLineBreak);
	wrbool (pBo1->PbBuildAll);
	wrbool (pBo1->PbNotInLine);
	wrbool (pBo1->PbPageFooter);
	wrbool (pBo1->PbPageHeader);
	wrbool (pBo1->PbPageBox);
	wrshort (pBo1->PbFooterHeight);
	wrshort (pBo1->PbHeaderHeight);
	wrshort (pBo1->PbPageCounter);
	wrTypeContenu (pBo1->PbContent);
	switch (pBo1->PbContent)
	      {
		 case ContVariable:
		    wrshort (pBo1->PbContVariable);
		    break;
		 case ContConst:
		    wrshort (pBo1->PbContConstant);
		    break;
		 case ContElement:
		    wrshort (pBo1->PbContElem);
		    wrshort (pBo1->PbContRefElem);
		    break;
		 default:
		    break;
	      }
     }

   /* ecrit les presentations des attributs semantiques */
   for (i = 1; i <= pSchStr->SsNAttributes; i++)
      wrshort (pSc1->PsNAttrPRule[i - 1]);

   for (i = 1; i <= pSchStr->SsNAttributes; i++)
     {
	pRP1 = pSc1->PsAttrPRule[i - 1];
	for (l = pSc1->PsNAttrPRule[i - 1]; l-- > 0; pRP1 = pRP1->ApNextAttrPres)
	  {
	     wrshort (pRP1->ApElemType);
	     switch (pSchStr->SsAttribute[i - 1].AttrType)
		   {
		      case AtNumAttr:
			 wrshort (pRP1->ApNCases);
			 for (j = 1; j <= pRP1->ApNCases; j++)
			   {
			      pCa1 = &pRP1->ApCase[j - 1];
			      wrComparAttr (pCa1->CaComparType);
			      wrsignshort (pCa1->CaLowerBound);
			      wrsignshort (pCa1->CaUpperBound);
			      wrptrreg (pCa1->CaFirstPRule);
			   }
			 break;
		      case AtReferenceAttr:
			 wrptrreg (pRP1->ApRefFirstPRule);
			 break;
		      case AtTextAttr:
			 wrnom (pRP1->ApString);
			 wrptrreg (pRP1->ApTextFirstPRule);
			 break;
		      case AtEnumAttr:
			 for (j = 0; j <= pSchStr->SsAttribute[i - 1].AttrNEnumValues; j++)
			    wrptrreg (pRP1->ApEnumFirstPRule[j]);
			 break;
		   }
	  }
     }

   /* ecrit la table des pointeurs de regle de chaque type du schema de */
   /* structure */
   for (i = 1; i <= pSchStr->SsNRules; i++)
      wrptrreg (pSc1->PsElemPRule[i - 1]);

   /* ecrit toutes les regles de presentation */
   /* ecrit les regles standard */
   WriteRules (pSc1->PsFirstDefaultPRule);

   /* ecrit les regles des boites */
   for (i = 1; i <= pSc1->PsNPresentBoxes; i++)
      WriteRules (pSc1->PsPresentBox[i - 1].PbFirstPRule);

   /* ecrit les regles des attributs */
   for (i = 1; i <= pSchStr->SsNAttributes; i++)
     {
	pRP1 = pSc1->PsAttrPRule[i - 1];
	for (l = pSc1->PsNAttrPRule[i - 1]; l-- > 0; pRP1 = pRP1->ApNextAttrPres)
	  {
	     switch (pSchStr->SsAttribute[i - 1].AttrType)
		   {
		      case AtNumAttr:
			 for (j = 1; j <= pRP1->ApNCases; j++)
			    WriteRules (pRP1->ApCase[j - 1].CaFirstPRule);
			 break;
		      case AtReferenceAttr:
			 WriteRules (pRP1->ApRefFirstPRule);
			 break;
		      case AtTextAttr:
			 WriteRules (pRP1->ApTextFirstPRule);
			 break;
		      case AtEnumAttr:
			 for (j = 0; j <= pSchStr->SsAttribute[i - 1].AttrNEnumValues; j++)
			    WriteRules (pRP1->ApEnumFirstPRule[j]);
			 break;
		   }
	  }
     }

   /* ecrit les regles des elements structures */
   for (i = 1; i <= pSchStr->SsNRules; i++)
      WriteRules (pSc1->PsElemPRule[i - 1]);

   for (i = 1; i <= pSchStr->SsNAttributes; i++)
      wrshort (pSc1->PsNHeirElems[i - 1]);

   for (i = 1; i <= pSchStr->SsNRules; i++)
      wrshort (pSc1->PsNInheritedAttrs[i - 1]);

   for (i = 1; i <= pSchStr->SsNAttributes; i++)
      wrshort (pSc1->PsNComparAttrs[i - 1]);

   for (i = 1; i <= pSchStr->SsNRules; i++)
      wrbool (pSc1->PsAcceptPageBreak[i - 1]);

   for (i = 1; i <= pSchStr->SsNRules; i++)
      wrbool (pSc1->PsAcceptLineBreak[i - 1]);

   for (i = 1; i <= pSchStr->SsNRules; i++)
      wrbool (pSc1->PsBuildAll[i - 1]);

   for (i = 1; i <= pSchStr->SsNRules; i++)
      wrbool (pSc1->PsNotInLine[i - 1]);

   for (i = 1; i <= pSchStr->SsNRules; i++)
      wrbool (pSc1->PsInPageHeaderOrFooter[i - 1]);

   for (i = 1; i <= pSchStr->SsNRules; i++)
      wrbool (pSc1->PsAssocPaginated[i - 1]);
   /* for (i = 1; i <= pSchStr->SsNRules; i++) *//* TODO */
   /* wrbool(pSc1->SPVueAssocAvecCol[i - 1]); *//* TODO */
   for (i = 1; i <= pSchStr->SsNRules; i++)
      wrshort (pSc1->PsElemTransmit[i - 1]);
   wrshort (pSc1->PsNTransmElems);
   for (i = 1; i <= pSc1->PsNTransmElems; i++)
     {
	wrshort (pSc1->PsTransmElem[i - 1].TeTargetDoc);
	wrnom (pSc1->PsTransmElem[i - 1].TeTargetAttr);
     }
   BIOwriteClose (outfile);
   return True;
}
/* End Of Module wrschprs */
