/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
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
   This module loads a presentation schema from a .PRS file

 */

#include "thot_sys.h"
#include "constmedia.h"
#include "constpiv.h"
#include "typemedia.h"
#include "fileaccess.h"
#include "libmsg.h"
#include "message.h"

#define THOT_EXPORT extern
#include "platform_tv.h"

static ThotBool     error;


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
   CHAR_T                c;

   if (!TtaReadByte (file, &c))
     {
	c = SPACE;
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
	    case C_PR_WEIGHT:
	       return PtWeight;
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
	    case C_PR_VERTOVERFLOW:
	       return PtVertOverflow;
	       break;
	    case C_PR_HORIZOVERFLOW:
	       return PtHorizOverflow;
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
static PresMode     ReadPresMode (BinFile file)
#else  /* __STDC__ */
static PresMode     ReadPresMode (file)
BinFile             file;

#endif /* __STDC__ */
{
   CHAR_T                c;

   if (!TtaReadByte (file, &c))
     {
	c = SPACE;
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
   CHAR_T                c;

   if (!TtaReadByte (file, &c))
     {
	c = SPACE;
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
static InheritMode  ReadInheritMode (BinFile file)
#else  /* __STDC__ */
static InheritMode  ReadInheritMode (file)
BinFile             file;

#endif /* __STDC__ */
{
   CHAR_T                c;

   if (!TtaReadByte (file, &c))
     {
	c = SPACE;
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
static FunctionType ReadFunctionType (BinFile file, ThotBool * repeat)
#else  /* __STDC__ */
static FunctionType ReadFunctionType (file, repeat)
BinFile             file;
ThotBool           *repeat;

#endif /* __STDC__ */
{
   CHAR_T                c;
   FunctionType        functType;

   *repeat = FALSE;
   if (!TtaReadByte (file, &c))
     {
	c = SPACE;
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
	    case C_PF_SHOWBOX:
	       functType = FnShowBox;
	       break;
	    case C_PF_BGPICTURE:
	       functType = FnBackgroundPicture;
	       break;
	    case C_PF_PICTUREMODE:
	       functType = FnPictureMode;
	       break;
	    case C_PF_NOTINLINE:
	       functType = FnNotInLine;
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
static BAlignment   ReadAlignment (BinFile file)
#else  /* __STDC__ */
static BAlignment   ReadAlignment (file)
BinFile             file;

#endif /* __STDC__ */
{
   CHAR_T                c;
   BAlignment          align;

   if (!TtaReadByte (file, &c))
     {
	c = SPACE;
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
   CHAR_T                c;
   PresCondition       condtype;

   if (!TtaReadByte (file, &c))
     {
	c = SPACE;
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
static ArithRel     ReadArithRel (BinFile file)
#else  /* __STDC__ */
static ArithRel     ReadArithRel (file)
BinFile             file;

#endif /* __STDC__ */
{
   CHAR_T                c;
   ArithRel            rel;

   if (!TtaReadByte (file, &c))
     {
	c = SPACE;
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
     default: rel = CondEquals;
     }
   return (rel);
}


/*----------------------------------------------------------------------
   	ReadBoxEdge   lit un repere de boite et retourne sa valeur.	
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static BoxEdge      ReadBoxEdge (BinFile file)
#else  /* __STDC__ */
static BoxEdge      ReadBoxEdge (file)
BinFile             file;

#endif /* __STDC__ */
{
   CHAR_T                c;
   BoxEdge             edge;

   if (!TtaReadByte (file, &c))
     {
	c = SPACE;
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
static Level        ReadLevel (BinFile file)

#else  /* __STDC__ */
static Level        ReadLevel (file)
BinFile             file;

#endif /* __STDC__ */
{
   CHAR_T                c;
   Level               level;

   if (!TtaReadByte (file, &c))
     {
	c = SPACE;
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
   CHAR_T                c;
   CounterOp           optype;

   if (!TtaReadByte (file, &c))
     {
	c = SPACE;
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
static BasicType    ReadBasicType (BinFile file)
#else  /* __STDC__ */
static BasicType    ReadBasicType (file)
BinFile             file;

#endif /* __STDC__ */
{
   CHAR_T                c;
   BasicType           basictyp;

   if (!TtaReadByte (file, &c))
     {
	c = SPACE;
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
static VariableType ReadVariableType (BinFile file)

#else  /* __STDC__ */
static VariableType ReadVariableType (file)
BinFile             file;

#endif /* __STDC__ */
{
   CHAR_T                c;
   VariableType        vartyp;

   if (!TtaReadByte (file, &c))
     {
	c = SPACE;
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
static CounterStyle ReadCounterStyle (BinFile file)
#else  /* __STDC__ */
static CounterStyle ReadCounterStyle (file)
BinFile             file;

#endif /* __STDC__ */
{
   CHAR_T                c;
   CounterStyle        countstyle;

   if (!TtaReadByte (file, &c))
     {
	c = SPACE;
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
   CHAR_T                c;
   CounterValue        value;

   if (!TtaReadByte (file, &c))
     {
	c = SPACE;
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
   CHAR_T                c;
   ContentType         conttype;

   if (!TtaReadByte (file, &c))
     {
	c = SPACE;
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
PtrPRule            ReadPRulePtr (BinFile file, PtrPRule * pNextPRule)

#else  /* __STDC__ */
PtrPRule            ReadPRulePtr (file, pNextPRule)
BinFile             file;
PtrPRule           *pNextPRule;

#endif /* __STDC__ */
{
   CHAR_T                c;

   if (!TtaReadByte (file, &c))
      return NULL;
   else if (c == EOS)
      return NULL;
   else
      return *pNextPRule;
}


/*----------------------------------------------------------------------
   	ReadRefKind
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static RefKind      ReadRefKind (BinFile file)
#else  /* __STDC__ */
static RefKind      ReadRefKind (file)
BinFile             file;

#endif /* __STDC__ */
{
   CHAR_T                c;

   if (!TtaReadByte (file, &c))
      c = SPACE;
   switch (c)
	 {
	    case C_KIND_ELEMENT_TYPE:
	       return (RkElType);
	       break;
	    case C_KIND_PRES_BOX:
	       return (RkPresBox);
	       break;
	    case C_KIND_ATTRIBUTE:
	       return (RkAttr);
	       break;
	    default:
	       error = True;
	       return (RkElType);
	       break;
	 }
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
   TtaReadBool (file, &pPosRule->PoDistAttr);
   TtaReadSignedShort (file, &pPosRule->PoDistance);
   pPosRule->PoRelation = ReadLevel (file);
   TtaReadBool (file, &pPosRule->PoNotRel);
   TtaReadBool (file, &pPosRule->PoUserSpecified);
   pPosRule->PoRefKind = ReadRefKind (file);
   TtaReadShort (file, &pPosRule->PoRefIdent);
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
   CHAR_T                c;

   if (!TtaReadByte (file, &c))
      c = SPACE;
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
PtrPRule           *pPRule;
PtrPRule           *pNextPRule;

#endif /* __STDC__ */
{
   PtrPRule            pPR;
   PtrCondition        pCond;
   PresCondition       typeCond;
   DimensionRule      *pDim;
   int                 i;

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
		  TtaReadBool (file, &pCond->CoNotNegative);
		  TtaReadBool (file, &pCond->CoTarget);
		  switch (typeCond)
			{
			   case PcEven:
			   case PcOdd:
			   case PcOne:
			      TtaReadShort (file, &pCond->CoCounter);
			      break;
			   case PcInterval:
			      TtaReadShort (file, &pCond->CoCounter);
			      TtaReadSignedShort (file, &pCond->CoMinCounter);
			      TtaReadSignedShort (file, &pCond->CoMaxCounter);
			      pCond->CoValCounter = ReadCounterValue (file);
			      break;
			   case PcWithin:
			      TtaReadBool (file, &pCond->CoImmediate);
			      TtaReadShort (file, &pCond->CoRelation);
			      pCond->CoAncestorRel = ReadArithRel (file);
			      TtaReadSignedShort (file, &pCond->CoTypeAncestor);
			      if (pCond->CoTypeAncestor == 0)
				{
				   TtaReadName (file, pCond->CoAncestorName);
				   TtaReadName (file, pCond->CoSSchemaName);
				}
			      else
				{
				   pCond->CoAncestorName[0] = EOS;
				   pCond->CoSSchemaName[0] = EOS;
				}
			      break;
			   case PcElemType:
			   case PcAttribute:
			      TtaReadSignedShort (file, &pCond->CoTypeElAttr);
			      break;
			   default:
			      break;
			}
		  typeCond = ReadPresCondition (file);
	       }
	     TtaReadShort (file, &pPR->PrViewNum);
	     pPR->PrPresMode = ReadPresMode (file);
	     if (!error)
		switch (pPR->PrPresMode)
		      {
			 case PresInherit:
			    pPR->PrInheritMode = ReadInheritMode (file);
			    TtaReadBool (file, &pPR->PrInhPercent);
			    TtaReadBool (file, &pPR->PrInhAttr);
			    TtaReadSignedShort (file, &pPR->PrInhDelta);
			    TtaReadBool (file, &pPR->PrMinMaxAttr);
			    TtaReadSignedShort (file, &pPR->PrInhMinOrMax);
			    pPR->PrInhUnit = rdUnit (file);
			    break;
			 case PresFunction:
			    pPR->PrPresBoxRepeat = FALSE;
			    pPR->PrPresFunction = ReadFunctionType (file, &pPR->PrPresBoxRepeat);
			    if (pPR->PrPresFunction != FnLine
				&& pPR->PrPresFunction != FnNoLine
				&& pPR->PrPresFunction != FnShowBox
				&& pPR->PrPresFunction != FnNotInLine)
			      {
				 TtaReadBool (file, &pPR->PrExternal);
				 TtaReadBool (file, &pPR->PrElement);
				 error = !TtaReadShort (file, &pPR->PrNPresBoxes);
				 if (!error)
				    if (pPR->PrNPresBoxes == 0)
				       TtaReadName (file, pPR->PrPresBoxName);
				    else
				      {
					 for (i = 0; i < pPR->PrNPresBoxes; i++)
					    TtaReadShort (file, &pPR->PrPresBox[i]);
					 pPR->PrPresBoxName[0] = EOS;
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
					TtaReadBool (file, &pPR->PrAttrValue);
					TtaReadSignedShort (file, &pPR->PrIntValue);
					break;
				     case PtFont:
				     case PtStyle:
				     case PtWeight:
				     case PtUnderline:
				     case PtThickness:
				     case PtLineStyle:
					if (!TtaReadByte (file, &pPR->PrChrValue))
					   error = True;
					break;
				     case PtBreak1:
				     case PtBreak2:
				     case PtIndent:
				     case PtSize:
				     case PtLineSpacing:
				     case PtLineWeight:
					pPR->PrMinUnit = rdUnit (file);
					TtaReadBool (file, &pPR->PrMinAttr);
					TtaReadSignedShort (file, &pPR->PrMinValue);
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
					TtaReadBool (file, &pDim->DrPosition);
					if (pDim->DrPosition)
					   ReadPosRule (file, &pDim->DrPosRule);
					else
					  {
					     TtaReadBool (file, &pDim->DrAbsolute);
					     TtaReadBool (file, &pDim->DrSameDimens);
					     pDim->DrUnit = rdUnit (file);
					     TtaReadBool (file, &pDim->DrAttr);
					     TtaReadBool (file, &pDim->DrMin);
					     TtaReadBool (file, &pDim->DrUserSpecified);
					     TtaReadSignedShort (file, &pDim->DrValue);
					     pDim->DrRelation = ReadLevel (file);
					     TtaReadBool (file, &pDim->DrNotRelat);
					     pDim->DrRefKind = ReadRefKind (file);
					     TtaReadShort (file, &pDim->DrRefIdent);
					  }
					break;
				     case PtAdjust:
					pPR->PrAdjust = ReadAlignment (file);
					break;
				     case PtJustify:
				     case PtHyphenate:
				     case PtVertOverflow:
				     case PtHorizOverflow:
					TtaReadBool (file, &pPR->PrJustify);
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
	while (pPR->PrNextPRule != NULL && !error);
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
   PtrPRule            pNextPRule;
   PtrPSchema          pPSch;
   Counter            *pCntr;
   CntrItem           *pCntrItem;
   PresConstant       *pConst;
   PresVariable       *pVar;
   PresVarItem        *pVarItem;
   PresentationBox    *pBox;
   AttributePres      *pAttrP, *pAttrOld;
   NumAttrCase        *pCase;
   PathBuffer          dirBuffer;
   BinFile             file;
   CHAR_T                buf[MAX_TXT_LEN];
   int                 InitialNElems, i, j, l;
   ThotBool            ret;

   error = FALSE;
   pPSch = NULL;
   /* compose le nom du fichier a ouvrir */
   ustrncpy (dirBuffer, SchemaPath, MAX_PATH);
   MakeCompleteName (fileName, PRS_EXT2, dirBuffer, buf, &i);

   /* teste si le fichier existe */
   file = TtaReadOpen (buf);
   if (file == 0)
     {
	/* message 'Fichier inaccessible' */
	ustrncpy (buf, fileName, MAX_NAME_LENGTH);
	ustrcat (buf, PRS_EXT);
	TtaDisplayMessage (INFO, TtaGetMessage (LIB, TMSG_LIB_MISSING_FILE), buf);
     }
   else
     {
	/* supprime le suffixe .PRS a la fin du nom de fichier */
	buf[i - 4] = EOS;
	GetSchPres (&pPSch);
	/* acquiert un bloc pour la prochaine regle lue */
	GetPresentRule (&pNextPRule);
	pNextPRule->PrCond = NULL;
	/* met son nom dans le schema de presentation */
	ustrncpy (pPSch->PsPresentName, fileName, MAX_NAME_LENGTH - 1);
	/* lit la partie fixe du schema de presentation */
	/* lit le nom du schema de structure correspondant */
	TtaReadName (file, pPSch->PsStructName);
	TtaReadShort (file, &pPSch->PsStructCode);
	error = !TtaReadShort (file, &pPSch->PsNViews);
	if (!error)
	   for (i = 0; i < pPSch->PsNViews; i++)
	      TtaReadName (file, pPSch->PsView[i]);
	if (!error)
	   for (i = 0; i < pPSch->PsNViews; i++)
	      TtaReadBool (file, &pPSch->PsPaginatedView[i]);
	/* significatif uniquement dans la V4 */
	if (!error)
	   for (i = 0; i < pPSch->PsNViews; i++)
	      TtaReadBool (file, &pPSch->PsColumnView[i]);
	error = !TtaReadShort (file, &pPSch->PsNPrintedViews);
	if (!error)
	   for (i = 0; i < pPSch->PsNPrintedViews; i++)
	     {
		TtaReadBool (file, &pPSch->PsPrintedView[i].VpAssoc);
		error = !TtaReadShort (file, &pPSch->PsPrintedView[i].VpNumber);
	     }
	if (!error)
	   for (i = 0; i < pPSch->PsNViews; i++)
	      TtaReadBool (file, &pPSch->PsExportView[i]);
	TtaReadShort (file, &pPSch->PsNCounters);
	TtaReadShort (file, &pPSch->PsNConstants);
	TtaReadShort (file, &pPSch->PsNVariables);
	TtaReadShort (file, &pPSch->PsNPresentBoxes);
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
	     TtaDisplayMessage (INFO, TtaGetMessage (LIB, TMSG_OLD_SCH_TO_RECOMP),
				buf);
	  }
	else
	  {
	     /* lit les compteurs */
	     if (!error)
		for (i = 0; i < pPSch->PsNCounters; i++)
		  {
		     pCntr = &pPSch->PsCounter[i];
		     error = !TtaReadShort (file, &pCntr->CnNItems);
		     if (!error)
			for (j = 0; j < pCntr->CnNItems; j++)
			  {
			     pCntrItem = &pCntr->CnItem[j];
			     pCntrItem->CiCntrOp = ReadCounterOp (file);
			     TtaReadShort (file, &pCntrItem->CiElemType);
			     TtaReadSignedShort (file, &pCntrItem->CiAscendLevel);
			     TtaReadShort (file, &pCntrItem->CiViewNum);
			     TtaReadSignedShort (file, &pCntrItem->CiParamValue);
			     TtaReadShort (file, &pCntrItem->CiInitAttr);
			     TtaReadShort (file, &pCntrItem->CiReinitAttr);
			     TtaReadShort (file, &pCntrItem->CiCondAttr);
			     TtaReadBool (file, &pCntrItem->CiCondAttrPresent);
			  }
		     error = !TtaReadShort (file, &pCntr->CnNPresBoxes);
		     if (!error)
			for (j = 0; j < pCntr->CnNPresBoxes; j++)
			  {
			     TtaReadShort (file, &pCntr->CnPresBox[j]);
			     error = !TtaReadBool (file, &pCntr->CnMinMaxPresBox[j]);
			  }
		     error = !TtaReadShort (file, &pCntr->CnNTransmAttrs);
		     if (!error)
			for (j = 0; j < pCntr->CnNTransmAttrs; j++)
			  {
			     TtaReadName (file, pCntr->CnTransmAttr[j]);
			     TtaReadShort (file, &pCntr->CnTransmSSchemaAttr[j]);
			  }
		     error = !TtaReadShort (file, &pCntr->CnNCreators);
		     if (!error)
			for (j = 0; j < pCntr->CnNCreators; j++)
			  {
			     error = !TtaReadShort (file, &pCntr->CnCreator[j]);
			     error = !TtaReadBool (file, &pCntr->CnMinMaxCreator[j]);
			  }
		     if (!error)
			for (j = 0; j < pCntr->CnNCreators; j++)
			   TtaReadBool (file, &pCntr->CnPresBoxCreator[j]);
		     error = !TtaReadShort (file, &pCntr->CnNCreatedBoxes);
		     if (!error)
			for (j = 0; j < pCntr->CnNCreatedBoxes; j++)
			  {
			     TtaReadShort (file, &pCntr->CnCreatedBox[j]);
			     error = !TtaReadBool (file, &pCntr->CnMinMaxCreatedBox[j]);
			  }
		     error = !TtaReadBool (file, &pCntr->CnPageFooter);
		  }
	     /* lit les constantes de presentation */
	     if (!error)
		for (i = 0; i < pPSch->PsNConstants; i++)
		  {
		     pConst = &pPSch->PsConstant[i];
		     pConst->PdType = ReadBasicType (file);
		     if (!TtaReadByte (file, &pConst->PdAlphabet))
			error = True;
		     j = 0;
		     if (!error)
			do
			   if (!TtaReadByte (file, &pConst->PdString[j++]))
			      error = True;
			while (pConst->PdString[j - 1] != EOS && !error) ;
		  }

	     /* lit les variables de presentation */
	     if (!error)
		for (i = 0; i < pPSch->PsNVariables; i++)
		  {
		     pVar = &pPSch->PsVariable[i];
		     error = !TtaReadShort (file, &pVar->PvNItems);
		     if (!error)
			for (j = 0; j < pVar->PvNItems; j++)
			  {
			     pVarItem = &pVar->PvItem[j];
			     pVarItem->ViType = ReadVariableType (file);
			     switch (pVarItem->ViType)
				   {
				      case VarText:
					 TtaReadShort (file, &pVarItem->ViConstant);
					 break;
				      case VarCounter:
					 TtaReadShort (file, &pVarItem->ViCounter);
					 pVarItem->ViStyle = ReadCounterStyle (file);
					 pVarItem->ViCounterVal = ReadCounterValue (file);
					 break;
				      case VarAttrValue:
					 TtaReadShort (file, &pVarItem->ViAttr);
					 pVarItem->ViStyle = ReadCounterStyle (file);

					 break;
				      case VarPageNumber:
					 TtaReadShort (file, &pVarItem->ViView);
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
		     TtaReadName (file, pBox->PbName);
		     pBox->PbFirstPRule = ReadPRulePtr (file, &pNextPRule);
		     TtaReadBool (file, &pBox->PbAcceptPageBreak);
		     TtaReadBool (file, &pBox->PbAcceptLineBreak);
		     TtaReadBool (file, &pBox->PbBuildAll);
		     TtaReadBool (file, &pBox->PbPageFooter);
		     TtaReadBool (file, &pBox->PbPageHeader);
		     TtaReadBool (file, &pBox->PbPageBox);
		     TtaReadShort (file, &pBox->PbFooterHeight);
		     TtaReadShort (file, &pBox->PbHeaderHeight);
		     TtaReadShort (file, &pBox->PbPageCounter);
		     pBox->PbContent = ReadContentType (file);
		     if (!error)
			switch (pBox->PbContent)
			      {
				 case ContVariable:
				    TtaReadShort (file, &pBox->PbContVariable);
				    break;
				 case ContConst:
				    TtaReadShort (file, &pBox->PbContConstant);
				    break;
				 case ContElement:
				    TtaReadShort (file, &pBox->PbContElem);
				    TtaReadShort (file, &pBox->PbContRefElem);
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
			  TtaReadShort (file, &l);
			  pPSch->PsNAttrPRule[i] = l;
			  pPSch->PsAttrPRule[i] = NULL;
			  if (l > 0)
			    {
			      pAttrOld = NULL;
			      while (l > 0)
				{
				  /* allocate attribute blocks */
				  GetAttributePres (&pAttrP);
				  if (pPSch->PsAttrPRule[i] == NULL)
				    pPSch->PsAttrPRule[i] = pAttrP;
				  else if (pAttrOld != NULL)
				    pAttrOld->ApNextAttrPres = pAttrP;
				  else
				    error = TRUE;
				  pAttrOld = pAttrP;
				  l--;
				}
			    }
		       }
		  for (i = 0; i < pSS->SsNAttributes; i++)
		     if (!error)
		       {
			  pAttrP = pPSch->PsAttrPRule[i];
			  for (l = pPSch->PsNAttrPRule[i]; l-- > 0; pAttrP = pAttrP->ApNextAttrPres)
			    {
			       if (!error)
				 {
				    error = !TtaReadShort (file, &pAttrP->ApElemType);
				    switch (pSS->SsAttribute[i].AttrType)
					  {
					     case AtNumAttr:
						for (j = 0; j < MAX_PRES_ATTR_CASE; j++)
						   pAttrP->ApCase[j].CaFirstPRule = NULL;
						error = !TtaReadShort (file, &pAttrP->ApNCases);
						if (!error)
						   for (j = 0; j < pAttrP->ApNCases; j++)
						     {
							pCase = &pAttrP->ApCase[j];
							pCase->CaComparType = ReadAttrCompar (file);
							TtaReadSignedShort (file, &pCase->CaLowerBound);
							TtaReadSignedShort (file, &pCase->CaUpperBound);
							pCase->CaFirstPRule = ReadPRulePtr (file, &pNextPRule);
						     }
						break;
					     case AtReferenceAttr:
						pAttrP->ApRefFirstPRule = ReadPRulePtr (file, &pNextPRule);
						break;
					     case AtTextAttr:
						TtaReadName (file, pAttrP->ApString);
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
		   TtaReadShort (file, &pPSch->PsNHeirElems[i]);

	     if (!error)
		for (i = 0; i < InitialNElems; i++)
		  {
		     TtaReadShort (file, &pPSch->PsNInheritedAttrs[i]);
		     pPSch->PsInheritedAttr[i] = NULL;
		  }

	     if (!error)
		for (i = 0; i < pSS->SsNAttributes; i++)
		  {
		     TtaReadShort (file, &pPSch->PsNComparAttrs[i]);
		     pPSch->PsComparAttr[i] = NULL;
		  }

	     if (!error)
		for (i = 0; i < InitialNElems; i++)
		   TtaReadBool (file, &pPSch->PsAcceptPageBreak[i]);

	     if (!error)
		for (i = 0; i < InitialNElems; i++)
		   TtaReadBool (file, &pPSch->PsAcceptLineBreak[i]);

	     if (!error)
		for (i = 0; i < InitialNElems; i++)
		   TtaReadBool (file, &pPSch->PsBuildAll[i]);

	     if (!error)
		for (i = 0; i < InitialNElems; i++)
		   TtaReadBool (file, &pPSch->PsInPageHeaderOrFooter[i]);

	     if (!error)
		for (i = 0; i < InitialNElems; i++)
		   TtaReadBool (file, &pPSch->PsAssocPaginated[i]);
	     if (!error)
		for (i = 0; i < InitialNElems; i++)
		   TtaReadShort (file, &pPSch->PsElemTransmit[i]);
	     if (!error)
		TtaReadShort (file, &pPSch->PsNTransmElems);
	     if (!error)
		for (i = 0; i < pPSch->PsNTransmElems; i++)
		  {
		     TtaReadShort (file, &pPSch->PsTransmElem[i].TeTargetDoc);
		     TtaReadName (file, pPSch->PsTransmElem[i].TeTargetAttr);
		  }
	     FreePresentRule (pNextPRule);
	  }
	/* ferme le fichier */
	TtaReadClose (file);
     }
   if (error)
     {
	/* message 'Schema incorrect' */
	TtaDisplayMessage (INFO, TtaGetMessage (LIB, TMSG_INCORRECT_PRS_FILE),
			   fileName);
	return NULL;
     }
   else
      return pPSch;
}
