/*
 * Copyright (c) 1996 INRIA, All rights reserved
 */

/*
   This module loads a presentation schema from a .PRS file

 */

#include "thot_sys.h"
#include "constmedia.h"
#include "constpiv.h"
#include "typemedia.h"
#include "storage.h"
#include "libmsg.h"
#include "message.h"

#define EXPORT extern
#include "platform_tv.h"

static boolean      error;


#include "fileaccess_f.h"
#include "memory_f.h"
#include "readstr_f.h"
#include "readprs_f.h"


/*----------------------------------------------------------------------
   	ReadrdTypeRegle  lit un type de regle de presentation et	
   	retourne sa valeur.						
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static PRuleType    ReadrdTypeRegle (BinFile file)
#else  /* __STDC__ */
static PRuleType    ReadrdTypeRegle (file)
BinFile             file;

#endif /* __STDC__ */
{
   char                c;

   if (!BIOreadByte (file, &c))
     {
	c = ' ';
	error = True;
     }
   switch (c)
	 {
	    case C_PR_VISIBILITY:
	       return PtVisibility;
	       break;
	    case C_PR_FUNCTION:
	       return PtFunction;
	       break;
	    case C_PR_VREF:
	       return PtVertRef;
	       break;
	    case C_PR_HREF:
	       return PtHorizRef;
	       break;
	    case C_PR_HEIGHT:
	       return PtHeight;
	       break;
	    case C_PR_WIDTH:
	       return PtWidth;
	       break;
	    case C_PR_VPOS:
	       return PtVertPos;
	       break;
	    case C_PR_HPOS:
	       return PtHorizPos;
	       break;
	    case C_PR_SIZE:
	       return PtSize;
	       break;
	    case C_PR_STYLE:
	       return PtStyle;
	       break;
	    case C_PR_UNDERLINE:
	       return PtUnderline;
	       break;
	    case C_PR_UNDER_THICK:
	       return PtThickness;
	       break;
	    case C_PR_FONT:
	       return PtFont;
	       break;
	    case C_PR_BREAK1:
	       return PtBreak1;
	       break;
	    case C_PR_BREAK2:
	       return PtBreak2;
	       break;
	    case C_PR_INDENT:
	       return PtIndent;
	       break;
	    case C_PR_LINESPACING:
	       return PtLineSpacing;
	       break;
	    case C_PR_ADJUST:
	       return PtAdjust;
	       break;
	    case C_PR_JUSTIFY:
	       return PtJustify;
	       break;
	    case C_PR_HYPHENATE:
	       return PtHyphenate;
	       break;
	    case C_PR_DEPTH:
	       return PtDepth;
	       break;
	    case C_PR_LINESTYLE:
	       return PtLineStyle;
	       break;
	    case C_PR_LINEWEIGHT:
	       return PtLineWeight;
	       break;
	    case C_PR_FILLPATTERN:
	       return PtFillPattern;
	       break;
	    case C_PR_BACKGROUND:
	       return PtBackground;
	       break;
	    case C_PR_FOREGROUND:
	       return PtForeground;
	       break;
	    default:
	       error = True;
	       return PtVisibility;
	       break;
	 }
}

/*----------------------------------------------------------------------
   	ReadPresMode lit un mode de presentation et retourne sa valeur.	
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static PresMode   ReadPresMode (BinFile file)
#else  /* __STDC__ */
static PresMode   ReadPresMode (file)
BinFile             file;

#endif /* __STDC__ */
{
   char                c;

   if (!BIOreadByte (file, &c))
     {
	c = ' ';
	error = True;
     }
   switch (c)
	 {
	    case C_IMMEDIATE:
	       return PresImmediate;
	       break;
	    case C_INHERIT:
	       return PresInherit;
	       break;
	    case C_PRES_FUNCTION:
	       return PresFunction;
	       break;
	    default:
	       error = True;
	       return PresImmediate;
	       break;
	 }
}

/*----------------------------------------------------------------------
   rdTypeUnite lit un type d'unite dans le fichier                    
   et retourne sa valeur.                                  
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static TypeUnit     rdUnit (BinFile file)
#else  /* __STDC__ */
static TypeUnit     rdUnit (file)
BinFile             file;

#endif /* __STDC__ */
{
   char                c;

   if (!BIOreadByte (file, &c))
     {
	c = ' ';
	error = True;
     }
   switch (c)
	 {
	    case C_UNIT_REL:
	       return UnRelative;
	       break;
	    case C_UNIT_XHEIGHT:
	       return UnXHeight;
	       break;
	    case C_UNIT_POINT:
	       return UnPoint;
	       break;
	    case C_UNIT_PIXEL:
	       return UnPixel;
	       break;
	    case C_UNIT_PERCENT:
	       return UnPercent;
	       break;
	    default:
	       error = True;
	       return UnPoint;
	       break;
	 }
}

/*----------------------------------------------------------------------
   ReadInheritMode lit un mode d'heritage et retourne sa valeur.	
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static InheritMode ReadInheritMode (BinFile file)
#else  /* __STDC__ */
static InheritMode ReadInheritMode (file)
BinFile             file;

#endif /* __STDC__ */
{
   char                c;

   if (!BIOreadByte (file, &c))
     {
	c = ' ';
	error = True;
     }
   switch (c)
	 {
	    case C_INH_ASCEND:
	       return InheritParent;
	       break;
	    case C_INH_PREVIOUS:
	       return InheritPrevious;
	       break;
	    case C_INH_DESC:
	       return InheritChild;
	       break;
	    case C_INH_CREATOR:
	       return InheritCreator;
	       break;
	    case C_INH_GRAND_FATHER:
	       return InheritGrandFather;
	       break;
	    default:
	       error = True;
	       return InheritParent;
	       break;
	 }
}


/*----------------------------------------------------------------------
   	ReadFunctionType  lit un type de fonction de presentation et	
   	retourne sa valeur.						
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static FunctionType    ReadFunctionType (BinFile file, boolean * repeat)
#else  /* __STDC__ */
static FunctionType    ReadFunctionType (file, repeat)
BinFile             file;
boolean            *repeat;

#endif /* __STDC__ */
{
   char                c;
   FunctionType        functType;

   *repeat = FALSE;
   if (!BIOreadByte (file, &c))
     {
	c = ' ';
	error = True;
     }
   switch (c)
	 {
	    case C_PF_LINE:
	       functType = FnLine;
	       break;
	    case C_PF_PAGE:
	       functType = FnPage;
	       break;
	    case C_PF_CR_BEFORE:
	       functType = FnCreateBefore;
	       break;
	    case C_PF_CR_BEFORE_REP:
	       functType = FnCreateBefore;
	       *repeat = TRUE;
	       break;
	    case C_PF_CR_WITH:
	       functType = FnCreateWith;
	       break;
	    case C_PF_CR_FIRST:
	       functType = FnCreateFirst;
	       break;
	    case C_PF_CR_FIRST_REP:
	       functType = FnCreateFirst;
	       *repeat = TRUE;
	       break;
	    case C_PF_CR_LAST:
	       functType = FnCreateLast;
	       break;
	    case C_PF_CR_LAST_REP:
	       functType = FnCreateLast;
	       *repeat = TRUE;
	       break;
	    case C_PF_CR_AFTER:
	       functType = FnCreateAfter;
	       break;
	    case C_PF_CR_AFTER_REP:
	       functType = FnCreateAfter;
	       *repeat = TRUE;
	       break;
	    case C_PF_CR_ENCLOSING:
	       functType = FnCreateEnclosing;
	       break;
	    case C_PF_CR_ENCLOSING_REP:
	       functType = FnCreateEnclosing;
	       *repeat = TRUE;
	       break;
	    case C_PF_COLUMN:
	       functType = FnColumn;
	       break;
	    case C_PF_SUBCOLUMN:
	       functType = FnSubColumn;
	       break;
	    case C_PF_COPY:
	       functType = FnCopy;
	       break;
	    case C_PF_REF_CONTENT:
	       functType = FnContentRef;
	       break;
	    case C_PF_NOLINE:
	       functType = FnNoLine;
	       break;
	    default:
	       error = True;
	       functType = FnLine;
	       break;
	 }
   return functType;
}


/*----------------------------------------------------------------------
   	ReadAlignment   lit un mode d'alignement des lignes et retourne	
   	sa valeur.							
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static BAlignment      ReadAlignment (BinFile file)
#else  /* __STDC__ */
static BAlignment      ReadAlignment (file)
BinFile             file;

#endif /* __STDC__ */
{
   char                c;
   BAlignment          align;

   if (!BIOreadByte (file, &c))
     {
	c = ' ';
	error = True;
     }
   switch (c)
	 {
	    case C_PIV_LEFT:
	       align = AlignLeft;
	       break;
	    case C_PIV_RIGHT:
	       align = AlignRight;
	       break;
	    case C_PIV_CENTERED:
	       align = AlignCenter;
	       break;
	    case C_PIV_LEFTDOT:
	       align = AlignLeftDots;
	       break;
	    default:
	       error = True;
	       align = AlignLeft;
	       break;
	 }

   return align;
}


/*----------------------------------------------------------------------
   	ReadPresCondition      lit un type de condition et retourne	
   	sa valeur.							
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static PresCondition ReadPresCondition (BinFile file)
#else  /* __STDC__ */
static PresCondition ReadPresCondition (file)
BinFile             file;

#endif /* __STDC__ */
{
   char                c;
   PresCondition       condtype;

   if (!BIOreadByte (file, &c))
     {
	c = ' ';
	error = True;
     }
   switch (c)
	 {
	    case C_COND_FIRST:
	       condtype = PcFirst;
	       break;
	    case C_COND_LAST:
	       condtype = PcLast;
	       break;
	    case C_COND_REFERRED:
	       condtype = PcReferred;
	       break;
	    case C_COND_FIRSTREF:
	       condtype = PcFirstRef;
	       break;
	    case C_COND_LASTREF:
	       condtype = PcLastRef;
	       break;
	    case C_COND_EXTREF:
	       condtype = PcExternalRef;
	       break;
	    case C_COND_INTREF:
	       condtype = PcInternalRef;
	       break;
	    case C_COND_COPYREF:
	       condtype = PcCopyRef;
	       break;
	    case C_COND_ATTR:
	       condtype = PcAnyAttributes;
	       break;
	    case C_COND_FIRST_ATTR:
	       condtype = PcFirstAttr;
	       break;
	    case C_COND_LAST_ATTR:
	       condtype = PcLastAttr;
	       break;
	    case C_COND_USERPAGE:
	       condtype = PcUserPage;
	       break;
	    case C_COND_STARTPAGE:
	       condtype = PcStartPage;
	       break;
	    case C_COND_COMPPAGE:
	       condtype = PcComputedPage;
	       break;
	    case C_COND_EMPTY:
	       condtype = PcEmpty;
	       break;
	    case C_COND_EVEN:
	       condtype = PcEven;
	       break;
	    case C_COND_ODD:
	       condtype = PcOdd;
	       break;
	    case C_COND_ONE:
	       condtype = PcOne;
	       break;
	    case C_COND_INTER:
	       condtype = PcInterval;
	       break;
	    case C_COND_ANCEST:
	       condtype = PcWithin;
	       break;
	    case C_COND_ELEM:
	       condtype = PcElemType;
	       break;
	    case C_COND_HAS_ATTR:
	       condtype = PcAttribute;
	       break;
	    case C_COND_NOCOND:
	       condtype = PcNoCondition;
	       break;
	    case C_COND_DEFAULT:
	       condtype = PcDefaultCond;
	       break;
	    default:
	       error = True;
	       condtype = PcFirst;
	       break;
	 }
   return condtype;
}


/*----------------------------------------------------------------------
   	ReadArithRel     lit la relation d'une condition Within		
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static ArithRel   ReadArithRel (BinFile file)
#else  /* __STDC__ */
static ArithRel   ReadArithRel (file)
BinFile           file;

#endif /* __STDC__ */
{
   char              c;
   ArithRel          rel;

   if (!BIOreadByte (file, &c))
     {
	c = ' ';
	error = True;
     }
   switch (c)
	 {
	    case C_WITHIN_GT:
	       rel = CondGreater;
	       break;
	    case C_WITHIN_LT:
	       rel = CondLess;
	    case C_WITHIN_EQ:
	       rel = CondEquals;
	 }
   return rel;
}


/*----------------------------------------------------------------------
   	ReadBoxEdge   lit un repere de boite et retourne sa valeur.	
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static BoxEdge  ReadBoxEdge (BinFile file)
#else  /* __STDC__ */
static BoxEdge  ReadBoxEdge (file)
BinFile         file;

#endif /* __STDC__ */
{
   char            c;
   BoxEdge         edge;

   if (!BIOreadByte (file, &c))
     {
	c = ' ';
	error = True;
     }
   switch (c)
	 {
	    case C_AX_TOP:
	       edge = Top;
	       break;
	    case C_AX_BOTTOM:
	       edge = Bottom;
	       break;
	    case C_AX_LEFT:
	       edge = Left;
	       break;
	    case C_AX_RIGHT:
	       edge = Right;
	       break;
	    case C_AX_HREF:
	       edge = HorizRef;
	       break;
	    case C_AX_VREF:
	       edge = VertRef;
	       break;
	    case C_AX_HMIDDLE:
	       edge = HorizMiddle;
	       break;
	    case C_AX_VMIDDLE:
	       edge = VertMiddle;
	       break;
	    case C_AX_NULL:
	       edge = NoEdge;
	       break;
	    default:
	       error = True;
	       edge = Top;
	       break;
	 }
   return edge;
}


/*----------------------------------------------------------------------
   	ReadLevel     lit un niveau relatif de boite et retourn		
   	sa valeur.							
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static Level       ReadLevel (BinFile file)

#else  /* __STDC__ */
static Level       ReadLevel (file)
BinFile             file;

#endif /* __STDC__ */
{
   char                c;
   Level              level;

   if (!BIOreadByte (file, &c))
     {
	c = ' ';
	error = True;
     }
   switch (c)
	 {
	    case C_PARENT:
	       level = RlEnclosing;
	       break;
	    case C_SAME_LEVEL:
	       level = RlSameLevel;
	       break;
	    case C_CHILD:
	       level = RlEnclosed;
	       break;
	    case C_PREVIOUS:
	       level = RlPrevious;
	       break;
	    case C_NEXT:
	       level = RlNext;
	       break;
	    case C_SELF:
	       level = RlSelf;
	       break;
	    case C_CONTAINS_REF:
	       level = RlContainsRef;
	       break;
	    case C_ROOT:
	       level = RlRoot;
	       break;
	    case C_REFERRED:
	       level = RlReferred;
	       break;
	    case C_CREATOR:
	       level = RlCreator;
	       break;
	    default:
	       error = True;
	       level = RlSameLevel;
	       break;
	 }
   return level;
}

/*----------------------------------------------------------------------
   	ReadCounterOp  lit un type d'operation sur compteur et retourne	
   	sa valeur.							
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static CounterOp    ReadCounterOp (BinFile file)
#else  /* __STDC__ */
static CounterOp    ReadCounterOp (file)
BinFile             file;

#endif /* __STDC__ */
{
   char                c;
   CounterOp           optype;

   if (!BIOreadByte (file, &c))
     {
	c = ' ';
	error = True;
     }
   switch (c)
	 {
	    case C_CNT_SET:
	       optype = CntrSet;
	       break;
	    case C_CNT_ADD:
	       optype = CntrAdd;
	       break;
	    case C_CNT_RANK:
	       optype = CntrRank;
	       break;
	    case C_CNT_RLEVEL:
	       optype = CntrRLevel;
	       break;
	    default:
	       error = True;
	       optype = CntrRank;
	       break;
	 }
   return optype;
}


/*----------------------------------------------------------------------
   	ReadBasicType lit un type de base dans le fichier et retourne	
   	sa valeur.							
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static BasicType   ReadBasicType (BinFile file)
#else  /* __STDC__ */
static BasicType   ReadBasicType (file)
BinFile             file;

#endif /* __STDC__ */
{
   char                c;
   BasicType          basictyp;

   if (!BIOreadByte (file, &c))
     {
	c = ' ';
	error = True;
     }
   switch (c)
	 {
	    case C_CHAR_STRING:
	       basictyp = CharString;
	       break;
	    case C_GRAPHICS:
	       basictyp = GraphicElem;
	       break;
	    case C_SYMBOL:
	       basictyp = Symbol;
	       break;
	    case C_PICTURE:
	       basictyp = Picture;
	       break;
	    case C_REFER:
	       basictyp = Refer;
	       break;
	    case C_PAGE_BREAK:
	       basictyp = PageBreak;
	       break;
	    default:
	       error = True;
	       basictyp = CharString;
	       break;
	 }
   return basictyp;
}


/*----------------------------------------------------------------------
   	ReadVariableType  lit un type de variable et retourne sa valeur	
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static VariableType      ReadVariableType (BinFile file)

#else  /* __STDC__ */
static VariableType      ReadVariableType (file)
BinFile                  file;

#endif /* __STDC__ */
{
   char                c;
   VariableType        vartyp;

   if (!BIOreadByte (file, &c))
     {
	c = ' ';
	error = True;
     }
   switch (c)
	 {
	    case C_VAR_TEXT:
	       vartyp = VarText;
	       break;
	    case C_VAR_COUNTER:
	       vartyp = VarCounter;

	       break;
	    case C_VAR_ATTR_VAL:
	       vartyp = VarAttrValue;
	       break;
	    case C_VAR_DATE:
	       vartyp = VarDate;
	       break;
	    case C_VAR_FDATE:
	       vartyp = VarFDate;
	       break;
	    case C_VAR_DIRNAME:
	       vartyp = VarDirName;
	       break;
	    case C_VAR_DOCNAME:
	       vartyp = VarDocName;
	       break;
	    case C_VAR_ELEMNAME:
	       vartyp = VarElemName;
	       break;
	    case C_VAR_ATTRNAME:
	       vartyp = VarAttrName;
	       break;
	    case C_VAR_PAGENUMBER:
	       vartyp = VarPageNumber;
	       break;
	    default:
	       error = True;
	       vartyp = VarText;
	       break;
	 }
   return vartyp;
}


/*----------------------------------------------------------------------
   	ReadCounterStyle lit un style de compteur et retourne sa valeur	
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static CounterStyle   ReadCounterStyle (BinFile file)
#else  /* __STDC__ */
static CounterStyle   ReadCounterStyle (file)
BinFile               file;

#endif /* __STDC__ */
{
   char                c;
   CounterStyle        countstyle;

   if (!BIOreadByte (file, &c))
     {
	c = ' ';
	error = True;
     }
   switch (c)
	 {
	    case C_NUM_ARABIC:
	       countstyle = CntArabic;
	       break;
	    case C_NUM_ROMAN:
	       countstyle = CntURoman;
	       break;
	    case C_NUM_LOWER_ROMAN:
	       countstyle = CntLRoman;
	       break;
	    case C_NUM_UPPERCASE:
	       countstyle = CntUppercase;
	       break;
	    case C_NUM_LOWERCASE:
	       countstyle = CntLowercase;
	       break;
	    default:
	       error = True;
	       countstyle = CntArabic;
	       break;
	 }
   return countstyle;
}


/*----------------------------------------------------------------------
   	ReadCounterValue    lit la nature du compteur inclus dans une	
   	variable et retourne sa valeur.					
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static CounterValue ReadCounterValue (BinFile file)
#else  /* __STDC__ */
static CounterValue ReadCounterValue (file)
BinFile             file;

#endif /* __STDC__ */
{
   char                c;
   CounterValue      value;

   if (!BIOreadByte (file, &c))
     {
	c = ' ';
	error = True;
     }
   switch (c)
	 {
	    case C_VAL_MAX:
	       value = CntMaxVal;
	       break;
	    case C_VAL_MIN:
	       value = CntMinVal;
	       break;
	    case C_VAL_CUR:
	       value = CntCurVal;
	       break;
	    default:
	       error = True;
	       value = CntCurVal;
	       break;
	 }
   return value;
}


/*----------------------------------------------------------------------
   	ReadContentType	lit un type de contenu retourne sa valeur.	
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ContentType         ReadContentType (BinFile file)

#else  /* __STDC__ */
ContentType         ReadContentType (file)
BinFile             file;

#endif /* __STDC__ */
{
   char                c;
   ContentType         conttype;

   if (!BIOreadByte (file, &c))
     {
	c = ' ';
	error = True;
     }
   switch (c)
	 {
	    case C_CONT_FREE:
	       conttype = FreeContent;
	       break;
	    case C_CONT_VAR:
	       conttype = ContVariable;
	       break;
	    case C_CONT_CONST:
	       conttype = ContConst;
	       break;
	    case C_CONT_ELEM:
	       conttype = ContElement;
	       break;
	    default:
	       error = True;
	       conttype = FreeContent;
	       break;
	 }
   return conttype;
}


/*----------------------------------------------------------------------
   	ReadPRulePtr	retourne un pointeur sur la regle suivante ou	
   	s'il n'y a pas de regle suivante.				
  ----------------------------------------------------------------------*/
#ifdef __STDC__
PtrPRule        ReadPRulePtr (BinFile file, PtrPRule * pNextPRule)

#else  /* __STDC__ */
PtrPRule        ReadPRulePtr (file, pNextPRule)
BinFile         file;
PtrPRule       *pNextPRule;

#endif /* __STDC__ */
{
   char                c;

   if (!BIOreadByte (file, &c))
      return NULL;
   else if (c == '\0')
      return NULL;
   else
      return *pNextPRule;
}


/*----------------------------------------------------------------------
   	ReadPosRule   lit un positionnement relatif			
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ReadPosRule (BinFile file, PosRule * pPosRule)
#else  /* __STDC__ */
void                ReadPosRule (file, pPosRule)
BinFile             file;
PosRule            *pPosRule;

#endif /* __STDC__ */
{
   pPosRule->PoPosDef = ReadBoxEdge (file);
   pPosRule->PoPosRef = ReadBoxEdge (file);
   pPosRule->PoDistUnit = rdUnit (file);
   BIOreadBool (file, &pPosRule->PoDistAttr);
   BIOreadSignedShort (file, &pPosRule->PoDistance);
   pPosRule->PoRelation = ReadLevel (file);
   BIOreadBool (file, &pPosRule->PoNotRel);
   BIOreadBool (file, &pPosRule->PoUserSpecified);
   BIOreadBool (file, &pPosRule->PoRefElem);
   if (pPosRule->PoRefElem)
      BIOreadShort (file, &pPosRule->PoTypeRefElem);
   else
      BIOreadShort (file, &pPosRule->PoRefPresBox);

}


/*----------------------------------------------------------------------
   	ReadAttrCompar lit un type de comparaison pour les valeurs	
   	d'attributs retourne sa valeur.					
  ----------------------------------------------------------------------*/
#ifdef __STDC__
AttrComparType      ReadAttrCompar (BinFile file)
#else  /* __STDC__ */
AttrComparType      ReadAttrCompar (file)
BinFile             file;

#endif /* __STDC__ */
{
   char                c;

   if (!BIOreadByte (file, &c))
      c = ' ';
   switch (c)
	 {
	    case C_COMP_CONST:
	       return (ComparConstant);
	       break;
	    case C_COMP_ATTR:
	       return (ComparAttr);
	       break;
	    default:
	       error = True;
	       return (ComparConstant);
	       break;
	 }
}


/*----------------------------------------------------------------------
   	ReadPRules    lit une suite de regles chainees et fait pointer	
   	le pointeur pPRule sur la premiere regle de la suite de regles lues	
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ReadPRules (BinFile file, PtrPRule * pPRule, PtrPRule * pNextPRule)
#else  /* __STDC__ */
void                ReadPRules (file, pPRule, pNextPRule)
BinFile             file;
PtrPRule            *pPRule;
PtrPRule            *pNextPRule;

#endif /* __STDC__ */
{
   PtrPRule         pPR;
   PtrCondition     pCond;
   PresCondition    typeCond;
   DimensionRule    *pDim;
   int              i;

   if (*pPRule != NULL && !error)
      /* pointeur sur la premiere regle qui va etre lue */
     {
	*pPRule = *pNextPRule;
	/* lecture de la suite de regles */
	do
	  {
	     pPR = *pNextPRule;
	     /* acquiert un buffer pour la regle suivante */
	     GetPresentRule (pNextPRule);
	     (*pNextPRule)->PrCond = NULL;
	     /* lit une regle */
	     pPR->PrType = ReadrdTypeRegle (file);
	     pPR->PrNextPRule = ReadPRulePtr (file, pNextPRule);
	     pPR->PrCond = NULL;
	     typeCond = ReadPresCondition (file);
	     while (typeCond != PcNoCondition && !error)
	       {
		  GetPresentRuleCond (&pCond);
		  pCond->CoNextCondition = pPR->PrCond;
		  pPR->PrCond = pCond;
		  pCond->CoCondition = typeCond;
		  BIOreadBool (file, &pCond->CoNotNegative);
		  BIOreadBool (file, &pCond->CoTarget);
		  switch (typeCond)
			{
			   case PcEven:
			   case PcOdd:
			   case PcOne:
			      BIOreadShort (file, &pCond->CoCounter);
			      break;
			   case PcInterval:
			      BIOreadShort (file, &pCond->CoCounter);
			      BIOreadSignedShort (file, &pCond->CoMinCounter);
			      BIOreadSignedShort (file, &pCond->CoMaxCounter);
			      pCond->CoValCounter = ReadCounterValue (file);
			      break;
			   case PcWithin:
			      BIOreadBool (file, &pCond->CoImmediate);
			      BIOreadShort (file, &pCond->CoRelation);
			      pCond->CoAncestorRel = ReadArithRel (file);
			      BIOreadSignedShort (file, &pCond->CoTypeAncestor);
			      if (pCond->CoTypeAncestor == 0)
				{
				   BIOreadName (file, pCond->CoAncestorName);
				   BIOreadName (file, pCond->CoSSchemaName);
				}
			      else
				{
				   pCond->CoAncestorName[0] = '\0';
				   pCond->CoSSchemaName[0] = '\0';
				}
			      break;
			   case PcElemType:
			   case PcAttribute:
			      BIOreadSignedShort (file, &pCond->CoTypeElAttr);
			      break;
			   default:
			      break;
			}
		  typeCond = ReadPresCondition (file);
	       }
	     BIOreadShort (file, &pPR->PrViewNum);
	     pPR->PrPresMode = ReadPresMode (file);
	     if (!error)
		switch (pPR->PrPresMode)
		      {
			 case PresInherit:
			    pPR->PrInheritMode = ReadInheritMode (file);
			    BIOreadBool (file, &pPR->PrInhAttr);
			    BIOreadSignedShort (file, &pPR->PrInhDelta);
			    BIOreadBool (file, &pPR->PrMinMaxAttr);
			    BIOreadSignedShort (file, &pPR->PrInhMinOrMax);
			    pPR->PrInhUnit = rdUnit (file);
			    break;
			 case PresFunction:
			    pPR->PrPresBoxRepeat = FALSE;
			    pPR->PrPresFunction = ReadFunctionType (file, &pPR->PrPresBoxRepeat);
			    if (pPR->PrPresFunction != FnLine
				&& pPR->PrPresFunction != FnNoLine)
			      {
				 BIOreadBool (file, &pPR->PrExternal);
				 BIOreadBool (file, &pPR->PrElement);
				 error = !BIOreadShort (file, &pPR->PrNPresBoxes);
				 if (!error)
				    if (pPR->PrNPresBoxes == 0)
				       BIOreadName (file, pPR->PrPresBoxName);
				    else
				      {
					 for (i = 0; i < pPR->PrNPresBoxes; i++)
					    BIOreadShort (file, &pPR->PrPresBox[i]);
					 pPR->PrPresBoxName[0] = '\0';
				      }
			      }
			    break;
			 case PresImmediate:
			    switch (pPR->PrType)
				  {
				     case PtVisibility:
				     case PtDepth:
				     case PtFunction:
				     case PtFillPattern:
				     case PtBackground:
				     case PtForeground:
					BIOreadBool (file, &pPR->PrAttrValue);
					BIOreadSignedShort (file, &pPR->PrIntValue);
					break;
				     case PtFont:
				     case PtStyle:
				     case PtUnderline:
				     case PtThickness:
				     case PtLineStyle:
					if (!BIOreadByte (file, &pPR->PrChrValue))
					   error = True;
					break;
				     case PtBreak1:
				     case PtBreak2:
				     case PtIndent:
				     case PtSize:
				     case PtLineSpacing:
				     case PtLineWeight:
					pPR->PrMinUnit = rdUnit (file);
					BIOreadBool (file, &pPR->PrMinAttr);
					BIOreadSignedShort (file, &pPR->PrMinValue);
					break;
				     case PtVertRef:
				     case PtHorizRef:
				     case PtVertPos:
				     case PtHorizPos:
					ReadPosRule (file, &pPR->PrPosRule);
					break;
				     case PtHeight:
				     case PtWidth:
					pDim = &pPR->PrDimRule;
					BIOreadBool (file, &pDim->DrPosition);
					if (pDim->DrPosition)
					   ReadPosRule (file, &pDim->DrPosRule);
					else
					  {
					     BIOreadBool (file, &pDim->DrAbsolute);
					     BIOreadBool (file, &pDim->DrSameDimens);
					     pDim->DrUnit = rdUnit (file);
					     BIOreadBool (file, &pDim->DrAttr);
					     BIOreadBool (file, &pDim->DrMin);
					     BIOreadBool (file, &pDim->DrUserSpecified);
					     BIOreadSignedShort (file, &pDim->DrValue);
					     pDim->DrRelation = ReadLevel (file);
					     BIOreadBool (file, &pDim->DrNotRelat);
					     BIOreadBool (file, &pDim->DrRefElement);
					     if (pDim->DrRefElement)
						BIOreadShort (file, &pDim->DrTypeRefElem);
					     else
						BIOreadShort (file, &pDim->DrRefPresBox);
					  }
					break;
				     case PtAdjust:
					pPR->PrAdjust = ReadAlignment (file);
					break;
				     case PtJustify:
				     case PtHyphenate:
					BIOreadBool (file, &pPR->PrJustify);
					break;
				     default:
					break;
				  }
			    /* passe a la regle suivante */
			    break;
		      }
	     if (pPR->PrNextPRule != NULL)
		pPR->PrNextPRule = *pNextPRule;
	  }
	while (pPR->PrNextPRule != NULL && ! error);
     }
}


/*----------------------------------------------------------------------
   	ReadPresentationSchema 						
   	lit un fichier contenant un schema de presentation et le charge	
   	en memoire.							
   	fileName: nom du fichier a lire.				
   	pSS: pointeur sur le schema de structure correspondant, deja	
   	rempli si SsRootElem n'est pas nul, a charger si SsRootElem	
   	est nul.							
   	Retourne un pointeur sur le schema de presentation en memoire	
   	chargement reussi, NULL si echec.				
  ----------------------------------------------------------------------*/
#ifdef __STDC__
PtrPSchema          ReadPresentationSchema (Name fileName, PtrSSchema pSS)
#else  /* __STDC__ */
PtrPSchema          ReadPresentationSchema (fileName, pSS)
Name                fileName;
PtrSSchema          pSS;

#endif /* __STDC__ */
{
   PtrPRule         pNextPRule;
   PtrPSchema       pPSch;
   Counter          *pCntr;
   CntrItem         *pCntrItem;
   PresConstant     *pConst;
   PresVariable     *pVar;
   PresVarItem      *pVarItem;
   PresentationBox  *pBox;
   AttributePres    *pAttrP;
   NumAttrCase      *pCase;
   PathBuffer       dirBuffer;
   BinFile          file;
   char             buf[MAX_TXT_LEN];
   int              InitialNElems, i, j, l;
   boolean          ret;

   error = FALSE;
   pPSch = NULL;
   /* compose le nom du fichier a ouvrir */
   strncpy (dirBuffer, SchemaPath, MAX_PATH);
   MakeCompleteName (fileName, "PRS", dirBuffer, buf, &i);

   /* teste si le fichier existe */
   file = BIOreadOpen (buf);
   if (file == 0)
     {
	/* message 'Fichier inaccessible' */
	strncpy (buf, fileName, MAX_NAME_LENGTH);
	strcat (buf, ".PRS");
	TtaDisplayMessage (INFO, TtaGetMessage(LIB, LIB_MISSING_FILE), buf);
     }
   else
     {
	/* supprime le suffixe .PRS a la fin du nom de fichier */
	buf[i - 4] = '\0';
	GetSchPres (&pPSch);
	/* acquiert un bloc pour la prochaine regle lue */
	GetPresentRule (&pNextPRule);
	pNextPRule->PrCond = NULL;
	/* met son nom dans le schema de presentation */
	strncpy (pPSch->PsPresentName, fileName, MAX_NAME_LENGTH - 1);
	/* lit la partie fixe du schema de presentation */
	/* lit le nom du schema de structure correspondant */
	BIOreadName (file, pPSch->PsStructName);
	BIOreadShort (file, &pPSch->PsStructCode);
	error = !BIOreadShort (file, &pPSch->PsNViews);
	if (!error)
	   for (i = 0; i < pPSch->PsNViews; i++)
	      BIOreadName (file, pPSch->PsView[i]);
	if (!error)
	   for (i = 0; i < pPSch->PsNViews; i++)
	      BIOreadBool (file, &pPSch->PsPaginatedView[i]);
	/* significatif uniquement dans la V4 */
	if (!error)
	   for (i = 0; i < pPSch->PsNViews; i++)
	      BIOreadBool (file, &pPSch->PsColumnView[i]);
	error = !BIOreadShort (file, &pPSch->PsNPrintedViews);
	if (!error)
	   for (i = 0; i < pPSch->PsNPrintedViews; i++)
	     {
		BIOreadBool (file, &pPSch->PsPrintedView[i].VpAssoc);
		error = !BIOreadShort (file, &pPSch->PsPrintedView[i].VpNumber);
	     }
	if (!error)
	   for (i = 0; i < pPSch->PsNViews; i++)
	      BIOreadBool (file, &pPSch->PsExportView[i]);
	BIOreadShort (file, &pPSch->PsNCounters);
	BIOreadShort (file, &pPSch->PsNConstants);
	BIOreadShort (file, &pPSch->PsNVariables);
	BIOreadShort (file, &pPSch->PsNPresentBoxes);
	pPSch->PsFirstDefaultPRule = ReadPRulePtr (file, &pNextPRule);
	ret = !error;
	if (pSS->SsRootElem == 0)
	   ret = ReadStructureSchema (pPSch->PsStructName, pSS);
	if (!ret || pPSch->PsStructCode != pSS->SsCode)
	  {
	     FreeSchPres (pPSch);
	     FreePresentRule (pNextPRule);
	     pNextPRule = NULL;
	     pPSch = NULL;
	     /* Schemas incompatibles */
	     TtaDisplayMessage (INFO, TtaGetMessage(LIB, OLD_SCH_TO_RECOMP),
				buf);
	  }
	else
	  {
	     /* lit les compteurs */
	     if (!error)
		for (i = 0; i < pPSch->PsNCounters; i++)
		  {
		     pCntr = &pPSch->PsCounter[i];
		     error = !BIOreadShort (file, &pCntr->CnNItems);
		     if (!error)
			for (j = 0; j < pCntr->CnNItems; j++)
			  {
			     pCntrItem = &pCntr->CnItem[j];
			     pCntrItem->CiCntrOp = ReadCounterOp (file);
			     BIOreadShort (file, &pCntrItem->CiElemType);
			     BIOreadSignedShort (file, &pCntrItem->CiAscendLevel);
			     BIOreadShort (file, &pCntrItem->CiViewNum);
			     BIOreadSignedShort (file, &pCntrItem->CiParamValue);
			     BIOreadShort (file, &pCntrItem->CiInitAttr);
			     BIOreadShort (file, &pCntrItem->CiReinitAttr);
			  }
		     error = !BIOreadShort (file, &pCntr->CnNPresBoxes);
		     if (!error)
			for (j = 0; j < pCntr->CnNPresBoxes; j++)
			  {
			     BIOreadShort (file, &pCntr->CnPresBox[j]);
			     error = !BIOreadBool (file, &pCntr->CnMinMaxPresBox[j]);
			  }
		     error = !BIOreadShort (file, &pCntr->CnNTransmAttrs);
		     if (!error)
			for (j = 0; j < pCntr->CnNTransmAttrs; j++)
			  {
			     BIOreadName (file, pCntr->CnTransmAttr[j]);
			     BIOreadShort (file, &pCntr->CnTransmSSchemaAttr[j]);
			  }
		     error = !BIOreadShort (file, &pCntr->CnNCreators);
		     if (!error)
			for (j = 0; j < pCntr->CnNCreators; j++)
			  {
			     error = !BIOreadShort (file, &pCntr->CnCreator[j]);
			     error = !BIOreadBool (file, &pCntr->CnMinMaxCreator[j]);
			  }
		     if (!error)
			for (j = 0; j < pCntr->CnNCreators; j++)
			   BIOreadBool (file, &pCntr->CnPresBoxCreator[j]);
		     error = !BIOreadShort (file, &pCntr->CnNCreatedBoxes);
		     if (!error)
			for (j = 0; j < pCntr->CnNCreatedBoxes; j++)
			  {
			     BIOreadShort (file, &pCntr->CnCreatedBox[j]);
			     error = !BIOreadBool (file, &pCntr->CnMinMaxCreatedBox[j]);
			  }
		     error = !BIOreadBool (file, &pCntr->CnPageFooter);
		  }
	     /* lit les constantes de presentation */
	     if (!error)
		for (i = 0; i < pPSch->PsNConstants; i++)
		  {
		     pConst = &pPSch->PsConstant[i];
		     pConst->PdType = ReadBasicType (file);
		     if (!BIOreadByte (file, &pConst->PdAlphabet))
			error = True;
		     j = 0;
		     if (!error)
			do
			   if (!BIOreadByte (file, &pConst->PdString[j++]))
			      error = True;
			while (pConst->PdString[j - 1] != '\0' && !error);
		  }

	     /* lit les variables de presentation */
	     if (!error)
		for (i = 0; i < pPSch->PsNVariables; i++)
		  {
		     pVar = &pPSch->PsVariable[i];
		     error = !BIOreadShort (file, &pVar->PvNItems);
		     if (!error)
			for (j = 0; j < pVar->PvNItems; j++)
			  {
			     pVarItem = &pVar->PvItem[j];
			     pVarItem->ViType = ReadVariableType (file);
			     switch (pVarItem->ViType)
				   {
				      case VarText:
					 BIOreadShort (file, &pVarItem->ViConstant);
					 break;
				      case VarCounter:
					 BIOreadShort (file, &pVarItem->ViCounter);
					 pVarItem->ViStyle = ReadCounterStyle (file);
					 pVarItem->ViCounterVal = ReadCounterValue (file);
					 break;
				      case VarAttrValue:
					 BIOreadShort (file, &pVarItem->ViAttr);
					 pVarItem->ViStyle = ReadCounterStyle (file);

					 break;
				      case VarPageNumber:
					 BIOreadShort (file, &pVarItem->ViView);
					 pVarItem->ViStyle = ReadCounterStyle (file);
					 break;
				      default:
					 break;
				   }
			  }
		  }
	     /* lit les boites de presentation et de mise en page */
	     if (!error)
		for (i = 0; i < pPSch->PsNPresentBoxes; i++)
		  {
		     pBox = &pPSch->PsPresentBox[i];
		     BIOreadName (file, pBox->PbName);
		     pBox->PbFirstPRule = ReadPRulePtr (file, &pNextPRule);
		     BIOreadBool (file, &pBox->PbAcceptPageBreak);
		     BIOreadBool (file, &pBox->PbAcceptLineBreak);
		     BIOreadBool (file, &pBox->PbBuildAll);
		     BIOreadBool (file, &pBox->PbNotInLine);
		     BIOreadBool (file, &pBox->PbPageFooter);
		     BIOreadBool (file, &pBox->PbPageHeader);
		     BIOreadBool (file, &pBox->PbPageBox);
		     BIOreadShort (file, &pBox->PbFooterHeight);
		     BIOreadShort (file, &pBox->PbHeaderHeight);
		     BIOreadShort (file, &pBox->PbPageCounter);
		     pBox->PbContent = ReadContentType (file);
		     if (!error)
			switch (pBox->PbContent)
			      {
				 case ContVariable:
				    BIOreadShort (file, &pBox->PbContVariable);
				    break;
				 case ContConst:
				    BIOreadShort (file, &pBox->PbContConstant);
				    break;
				 case ContElement:
				    BIOreadShort (file, &pBox->PbContElem);
				    BIOreadShort (file, &pBox->PbContRefElem);
				    break;
				 default:
				    break;
			      }
		  }
	     /* lit les presentations des attributs */
	     if (!error)
	       {
		  for (i = 0; i < pSS->SsNAttributes; i++)
		     if (!error)
		       {
			  /* lecture du nombre de paquet de regles differentes */
			  BIOreadShort (file, &pPSch->PsNAttrPRule[i]);
			  if (pPSch->PsNAttrPRule[i] > 0)
			    {
			       /* allocation des regles */
			       pPSch->PsAttrPRule[i] =
				  (AttributePres *) TtaGetMemory (pPSch->PsNAttrPRule[i] * sizeof (AttributePres));
			       if ((pAttrP = pPSch->PsAttrPRule[i]) != NULL)
				 {
				    /* chainage des regles */
				    for (l = pPSch->PsNAttrPRule[i]; --l > 0; pAttrP = pAttrP->ApNextAttrPres)
				       pAttrP->ApNextAttrPres = pAttrP + 1;
				    /* la derniere pointe sur NULL */
				    pAttrP->ApNextAttrPres = NULL;
				 }
			       else	/* l'allocation a echouee */
				  error = TRUE;
			    }
			  else
			     pPSch->PsAttrPRule[i] = NULL;
		       }
		  for (i = 0; i < pSS->SsNAttributes; i++)
		     if (!error)
		       {
			  pAttrP = pPSch->PsAttrPRule[i];
			  for (l = pPSch->PsNAttrPRule[i]; l-- > 0; pAttrP = pAttrP->ApNextAttrPres)
			    {
			       if (!error)
				 {
				    error = !BIOreadShort (file, &pAttrP->ApElemType);
				    switch (pSS->SsAttribute[i].AttrType)
					  {
					     case AtNumAttr:
						for (j = 0; j < MAX_PRES_ATTR_CASE; j++)
						   pAttrP->ApCase[j].CaFirstPRule = NULL;
						error = !BIOreadShort (file, &pAttrP->ApNCases);
						if (!error)
						   for (j = 0; j < pAttrP->ApNCases; j++)
						     {
							pCase = &pAttrP->ApCase[j];
							pCase->CaComparType = ReadAttrCompar (file);
							BIOreadSignedShort (file, &pCase->CaLowerBound);
							BIOreadSignedShort (file, &pCase->CaUpperBound);
							pCase->CaFirstPRule = ReadPRulePtr (file, &pNextPRule);
						     }
						break;
					     case AtReferenceAttr:
						pAttrP->ApRefFirstPRule = ReadPRulePtr (file, &pNextPRule);
						break;
					     case AtTextAttr:
						BIOreadName (file, pAttrP->ApString);
						pAttrP->ApTextFirstPRule = ReadPRulePtr (file, &pNextPRule);
						break;
					     case AtEnumAttr:
						for (j = 0; j <= MAX_ATTR_VAL; j++)
						   pAttrP->ApEnumFirstPRule[j] = NULL;
						for (j = 0; j <= pSS->SsAttribute[i].AttrNEnumValues; j++)
						   pAttrP->ApEnumFirstPRule[j] = ReadPRulePtr (file, &pNextPRule);
						break;
					  }
				 }
			    }
		       }
	       }

	     if (pSS->SsFirstDynNature == 0)
		InitialNElems = pSS->SsNRules;
	     else
		InitialNElems = pSS->SsFirstDynNature - 1;

	     /* lit la table des pointeurs de regle de chaque type du */
	     /* schema de structure */
	     if (!error)
		for (i = 0; i < InitialNElems; i++)
		   pPSch->PsElemPRule[i] = ReadPRulePtr (file, &pNextPRule);

	     /* lit toutes les regles de presentation */
	     /* lit les regles standard */
	     if (!error)
		ReadPRules (file, &pPSch->PsFirstDefaultPRule, &pNextPRule);

	     /* les regles des boites */
	     if (!error)
		for (i = 0; i < pPSch->PsNPresentBoxes; i++)
		   ReadPRules (file, &pPSch->PsPresentBox[i].PbFirstPRule, &pNextPRule);

	     /* lit les regles des attributs */
	     if (!error)
		for (i = 0; i < pSS->SsNAttributes; i++)
		   if (!error)
		     {
			pAttrP = pPSch->PsAttrPRule[i];
			for (l = pPSch->PsNAttrPRule[i]; l-- > 0; pAttrP = pAttrP->ApNextAttrPres)
			  {
			     if (!error)
				switch (pSS->SsAttribute[i].AttrType)
				      {
					 case AtNumAttr:
					    for (j = 0; j < pAttrP->ApNCases; j++)
					       ReadPRules (file, &pAttrP->ApCase[j].CaFirstPRule, &pNextPRule);
					    break;
					 case AtReferenceAttr:
					    ReadPRules (file, &pAttrP->ApRefFirstPRule, &pNextPRule);
					    break;
					 case AtTextAttr:
					    ReadPRules (file, &pAttrP->ApTextFirstPRule, &pNextPRule);
					    break;
					 case AtEnumAttr:
					    for (j = 0; j <= pSS->SsAttribute[i].AttrNEnumValues; j++)
					       ReadPRules (file, &pAttrP->ApEnumFirstPRule[j], &pNextPRule);
					    break;
				      }
			  }
		     }

	     /* lit les regles des elements structures */
	     if (!error)
		for (i = 0; i < InitialNElems; i++)
		   ReadPRules (file, &pPSch->PsElemPRule[i], &pNextPRule);

	     if (!error)
		for (i = 0; i < pSS->SsNAttributes; i++)
		   BIOreadShort (file, &pPSch->PsNHeirElems[i]);

	     if (!error)
		for (i = 0; i < InitialNElems; i++)
		  {
		     BIOreadShort (file, &pPSch->PsNInheritedAttrs[i]);
		     pPSch->PsInheritedAttr[i] = NULL;
		  }

	     if (!error)
		for (i = 0; i < pSS->SsNAttributes; i++)
		  {
		     BIOreadShort (file, &pPSch->PsNComparAttrs[i]);
		     pPSch->PsComparAttr[i] = NULL;
		  }

	     if (!error)
		for (i = 0; i < InitialNElems; i++)
		   BIOreadBool (file, &pPSch->PsAcceptPageBreak[i]);

	     if (!error)
		for (i = 0; i < InitialNElems; i++)
		   BIOreadBool (file, &pPSch->PsAcceptLineBreak[i]);

	     if (!error)
		for (i = 0; i < InitialNElems; i++)
		   BIOreadBool (file, &pPSch->PsBuildAll[i]);

	     if (!error)
		for (i = 0; i < InitialNElems; i++)
		   BIOreadBool (file, &pPSch->PsNotInLine[i]);

	     if (!error)
		for (i = 0; i < InitialNElems; i++)
		   BIOreadBool (file, &pPSch->PsInPageHeaderOrFooter[i]);

	     if (!error)
		for (i = 0; i < InitialNElems; i++)
		   BIOreadBool (file, &pPSch->PsAssocPaginated[i]);
	     if (!error)
		for (i = 0; i < InitialNElems; i++)
		   BIOreadShort (file, &pPSch->PsElemTransmit[i]);
	     if (!error)
		BIOreadShort (file, &pPSch->PsNTransmElems);
	     if (!error)
		for (i = 0; i < pPSch->PsNTransmElems; i++)
		  {
		     BIOreadShort (file, &pPSch->PsTransmElem[i].TeTargetDoc);
		     BIOreadName (file, pPSch->PsTransmElem[i].TeTargetAttr);
		  }
	     FreePresentRule (pNextPRule);
	  }
	/* ferme le fichier */
	BIOreadClose (file);
     }
   if (error)
     {
	/* message 'Schema incorrect' */
	TtaDisplayMessage (INFO, TtaGetMessage(LIB, INCORRECT_PRS_FILE),
			   fileName);
	return NULL;
     }
   else
      return pPSch;
}

