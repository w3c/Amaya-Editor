/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2009
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * This module loads a presentation schema from a .PRS file
 *
 */
#include "thot_gui.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "constpiv.h"
#include "typemedia.h"
#include "zlib.h"
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
static PRuleType    ReadrdTypeRegle (BinFile file)
{
  char c;

  if (!TtaReadByte (file, (unsigned char *)&c))
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
    case C_PR_VERTOVERFLOW:
      return PtVertOverflow;
      break;
    case C_PR_HORIZOVERFLOW:
      return PtHorizOverflow;
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
    case C_PR_MARGINTOP:
      return PtMarginTop;
      break;
    case C_PR_MARGINRIGHT:
      return PtMarginRight;
      break;
    case C_PR_MARGINBOTTOM:
      return PtMarginBottom;
      break;
    case C_PR_MARGINLEFT:
      return PtMarginLeft;
      break;
    case C_PR_PADDINGTOP:
      return PtPaddingTop;
      break;
    case C_PR_PADDINGRIGHT:
      return PtPaddingRight;
      break;
    case C_PR_PADDINGBOTTOM:
      return PtPaddingBottom;
      break;
    case C_PR_PADDINGLEFT:
      return PtPaddingLeft;
      break;
    case C_PR_BORDERTOPWIDTH:
      return PtBorderTopWidth;
      break;
    case C_PR_BORDERRIGHTWIDTH:
      return PtBorderRightWidth;
      break;
    case C_PR_BORDERBOTTOMWIDTH:
      return PtBorderBottomWidth;
      break;
    case C_PR_BORDERLEFTWIDTH:
      return PtBorderLeftWidth;
      break;
    case C_PR_BORDERTOPCOLOR:
      return PtBorderTopColor;
      break;
    case C_PR_BORDERRIGHTCOLOR:
      return PtBorderRightColor;
      break;
    case C_PR_BORDERBOTTOMCOLOR:
      return PtBorderBottomColor;
      break;
    case C_PR_BORDERLEFTCOLOR:
      return PtBorderLeftColor;
      break;
    case C_PR_BORDERTOPSTYLE:
      return PtBorderTopStyle;
      break;
    case C_PR_BORDERRIGHTSTYLE:
      return PtBorderRightStyle;
      break;
    case C_PR_BORDERBOTTOMSTYLE:
      return PtBorderBottomStyle;
      break;
    case C_PR_BORDERLEFTSTYLE:
      return PtBorderLeftStyle;
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
    case C_PR_VARIANT:
      return PtVariant;
      break;
    case C_PR_FONT:
      return PtFont;
      break;
    case C_PR_UNDERLINE:
      return PtUnderline;
      break;
    case C_PR_UNDER_THICK:
      return PtThickness;
      break;
    case C_PR_INDENT:
      return PtIndent;
      break;
    case C_PR_LINESPACING:
      return PtLineSpacing;
      break;
    case C_PR_DEPTH:
      return PtDepth;
      break;
    case C_PR_ADJUST:
      return PtAdjust;
      break;
    case C_PR_DIRECTION:
      return PtDirection;
      break;
    case C_PR_UNICODEBIDI:
      return PtUnicodeBidi;
      break;
    case C_PR_LINESTYLE:
      return PtLineStyle;
      break;
    case C_PR_FLOAT:
      return PtFloat;
      break;
    case C_PR_CLEAR:
      return PtClear;
      break;
    case C_PR_LINEWEIGHT:
      return PtLineWeight;
      break;
    case C_PR_FILLPATTERN:
      return PtFillPattern;
      break;
    case C_PR_OPACITY:
      return PtOpacity;
      break;
    case C_PR_FILL_OPACITY:
      return PtFillOpacity;
      break;
    case C_PR_STROKE_OPACITY:
      return PtStrokeOpacity;
      break;
    case C_PR_STOPOPACITY:
      return PtStopOpacity;
      break;
    case C_PR_MARKER:
      return PtMarker;
      break;
    case C_PR_MARKERSTART:
      return PtMarkerStart;
      break;
    case C_PR_MARKERMID:
      return PtMarkerMid;
      break;
    case C_PR_MARKEREND:
      return PtMarkerEnd;
      break;
    case C_PR_FILL_RULE:
      return PtFillRule;
      break;
    case C_PR_BACKGROUND:
      return PtBackground;
      break;
    case C_PR_FOREGROUND:
      return PtForeground;
      break;
    case C_PR_COLOR:
      return PtColor;
      break;
    case C_PR_STOPCOLOR:
      return PtStopColor;
      break;
    case C_PR_HYPHENATE:
      return PtHyphenate;
      break;
    case C_PR_PAGEBREAK:
      return PtPageBreak;
      break;
    case C_PR_LINEBREAK:
      return PtLineBreak;
      break;
    case C_PR_GATHER:
      return PtGather;
      break;
    case C_PR_XRADIUS:
      return PtXRadius;
      break;
    case C_PR_YRADIUS:
      return PtYRadius;
      break;
    case C_PR_DISPLAY:
      return PtDisplay;
      break;
    case C_PR_LISTSTYLETYPE:
      return PtListStyleType;
      break;
    case C_PR_LISTSTYLEIMAGE:
      return PtListStyleImage;
      break;
    case C_PR_LISTSTYLEPOSITION:
      return PtListStylePosition;
      break;
    case C_PR_BREAK1:
      return PtBreak1;
      break;
    case C_PR_BREAK2:
      return PtBreak2;
      break;
    default:
      error = True;
      return PtVisibility;
      break;
    }
  return PtVisibility;
}

/*----------------------------------------------------------------------
  ReadPresMode lit un mode de presentation et retourne sa valeur.	
  ----------------------------------------------------------------------*/
static PresMode     ReadPresMode (BinFile file)
{
  char c;

  if (!TtaReadByte (file, (unsigned char *)&c))
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
    case C_CURRENT_COLOR:
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
  return PresImmediate;
}

/*----------------------------------------------------------------------
  rdTypeUnite lit un type d'unite dans le fichier                    
  et retourne sa valeur.                                  
  ----------------------------------------------------------------------*/
static TypeUnit     rdUnit (BinFile file)
{
  char c;

  if (!TtaReadByte (file, (unsigned char *)&c))
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
    case C_UNIT_AUTO:
      return UnAuto;
      break;
    default:
      error = True;
      return UnPoint;
      break;
    }
  return UnPoint;
}

/*----------------------------------------------------------------------
  ReadInheritMode lit un mode d'heritage et retourne sa valeur.	
  ----------------------------------------------------------------------*/
static InheritMode  ReadInheritMode (BinFile file)
{
  char c;

  if (!TtaReadByte (file, (unsigned char *)&c))
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
  return InheritParent;
}

/*----------------------------------------------------------------------
  ReadFunctionType  lit un type de fonction de presentation et	
  retourne sa valeur.						
  ----------------------------------------------------------------------*/
static FunctionType ReadFunctionType (BinFile file, ThotBool * repeat)
{
  char         c;
  FunctionType functType;

  *repeat = FALSE;
  if (!TtaReadByte (file, (unsigned char *)&c))
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
      functType = FnBackgroundRepeat;
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
static BAlignment   ReadAlignment (BinFile file)
{
  char                c;
  BAlignment          align;

  if (!TtaReadByte (file, (unsigned char *)&c))
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
    case C_PIV_JUSTIFY:
      align = AlignJustify;
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
static PresCondition ReadPresCondition (BinFile file)
{
  char                c;
  PresCondition       condtype;

  if (!TtaReadByte (file, (unsigned char *)&c))
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
    case C_COND_ROOT:
      condtype = PcRoot;
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
    case C_COND_INHERIT_ATTR:
      condtype = PcInheritAttribute;
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
static ArithRel     ReadArithRel (BinFile file)
{
  char                c;
  ArithRel            rel;

  if (!TtaReadByte (file, (unsigned char *)&c))
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
static BoxEdge      ReadBoxEdge (BinFile file)
{
  char                c;
  BoxEdge             edge;

  if (!TtaReadByte (file, (unsigned char *)&c))
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
static Level        ReadLevel (BinFile file)
{
  char                c;
  Level               level;

  if (!TtaReadByte (file, (unsigned char *)&c))
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
    case C_LAST_SIBLING:
      level = RlLastSibling;
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
static CounterOp    ReadCounterOp (BinFile file)
{
  char                c;
  CounterOp           optype;

  if (!TtaReadByte (file, (unsigned char *)&c))
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
static BasicType    ReadBasicType (BinFile file)
{
  char                c;
  BasicType           basictyp;

  if (!TtaReadByte (file, (unsigned char *)&c))
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
      basictyp = tt_Picture;
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
static VariableType ReadVariableType (BinFile file)
{
  char                c;
  VariableType        vartyp;

  if (!TtaReadByte (file, (unsigned char *)&c))
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
static CounterStyle ReadCounterStyle (BinFile file)
{
  char                c;
  CounterStyle        countstyle;

  if (!TtaReadByte (file, (unsigned char *)&c))
    {
      c = SPACE;
      error = True;
    }
  switch (c)
    {
    case C_NUM_DECIMAL:
      countstyle = CntDecimal;
      break;
    case C_NUM_ZLDECIMAL:
      countstyle = CntZLDecimal;
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
    case C_NUM_GREEK:
      countstyle = CntLGreek;
      break;
    case C_NUM_UPPER_GREEK:
      countstyle = CntUGreek;
      break;
    default:
      error = True;
      countstyle = CntDecimal;
      break;
    }
  return countstyle;
}

/*----------------------------------------------------------------------
  ReadCounterValue    lit la nature du compteur inclus dans une	
  variable et retourne sa valeur.					
  ----------------------------------------------------------------------*/
static CounterValue ReadCounterValue (BinFile file)
{
  char                c;
  CounterValue        value;

  if (!TtaReadByte (file, (unsigned char *)&c))
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
ContentType         ReadContentType (BinFile file)
{
  char                c;
  ContentType         conttype;

  if (!TtaReadByte (file, (unsigned char *)&c))
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
PtrPRule     ReadPRulePtr (BinFile file, PtrPRule * pNextPRule)
{
  char                c;

  if (!TtaReadByte (file, (unsigned char *)&c))
    return NULL;
  else if (c == EOS)
    return NULL;
  else
    return *pNextPRule;
}

/*----------------------------------------------------------------------
  ReadRefKind
  ----------------------------------------------------------------------*/
static RefKind      ReadRefKind (BinFile file)
{
  char c;

  if (!TtaReadByte (file, (unsigned char *)&c))
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
    case C_KIND_ANYELEM:
      return (RkAnyElem);
      break;
    case C_KIND_ANYBOX:
      return (RkAnyBox);
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
void       ReadPosRule (BinFile file, PosRule *pPosRule)
{
  pPosRule->PoPosDef = ReadBoxEdge (file);
  pPosRule->PoPosRef = ReadBoxEdge (file);
  pPosRule->PoDistUnit = rdUnit (file);
  pPosRule->PoDeltaUnit = rdUnit (file);
  TtaReadBool (file, &pPosRule->PoDistAttr);
  TtaReadSignedShort (file, &pPosRule->PoDistance);
  TtaReadSignedShort (file, &pPosRule->PoDistDelta);
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
AttrComparType      ReadAttrCompar (BinFile file)
{
  char c;

  if (!TtaReadByte (file, (unsigned char *)&c))
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
static void ReadPRules (BinFile file, PtrPRule *pPRule, PtrPRule *pNextPRule,
                        PtrSSchema pSS)
{
  PtrPRule            pPR;
  PtrCondition        pCond;
  PresCondition       typeCond;
  DimensionRule      *pDim;
  Name                name;
  int                 i;
  unsigned char       c;

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
              pCond->CoChangeElem = FALSE;
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
                      TtaReadName (file, (unsigned char *)name);
                      pCond->CoAncestorName = TtaStrdup (name);
                      TtaReadName (file, (unsigned char *)pCond->CoSSchemaName);
                    }
                  else
                    {
                      pCond->CoAncestorName = NULL;
                      pCond->CoSSchemaName[0] = EOS;
                    }
                  break;
                case PcElemType:
                  TtaReadSignedShort (file, &pCond->CoTypeElem);
                  break;
                case PcAttribute:
                case PcInheritAttribute:
                  TtaReadSignedShort (file, &pCond->CoTypeAttr);
                  TtaReadBool (file, &pCond->CoTestAttrValue);
                  if (pCond->CoTestAttrValue)
                    {
                      if (pSS->SsAttribute->TtAttr[pCond->CoTypeAttr - 1]->AttrType == AtTextAttr)
                        {
                          TtaReadName (file, (unsigned char *)name);
                          pCond->CoAttrTextValue = TtaStrdup (name);
                          pCond->CoTextMatch = CoMatch;
                        }
                      else
                        TtaReadSignedShort (file,
                                            &pCond->CoAttrValue);
                    } 
                  break;
                default:
                  break; 
                }
              typeCond = ReadPresCondition (file);
            }
          TtaReadShort (file, &pPR->PrViewNum);
          TtaReadBool (file, &pPR->PrDuplicate);
          pPR->PrPresMode = ReadPresMode (file);
          if (!error)
            switch (pPR->PrPresMode)
              {
              case PresInherit:
              case PresCurrentColor:
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
                      {
                        if (pPR->PrNPresBoxes == 0)
                          TtaReadName (file, (unsigned char *)pPR->PrPresBoxName);
                        else
                          {
                            for (i = 0; i < pPR->PrNPresBoxes; i++)
                              TtaReadShort (file, &pPR->PrPresBox[i]);
                            pPR->PrPresBoxName[0] = EOS;
                          }
                      }
                  }
                break;
              case PresImmediate:
                switch (pPR->PrType)
                  {
                  case PtFunction:
                  case PtVisibility:
                  case PtDepth:
                  case PtFillPattern:
                  case PtOpacity:
                  case PtFillOpacity:
                  case PtStrokeOpacity:
                  case PtStopOpacity:
                  case PtMarker:
                  case PtMarkerStart:
                  case PtMarkerMid:
                  case PtMarkerEnd:
                  case PtBackground:
                  case PtForeground:
                  case PtColor:
                  case PtStopColor:
                  case PtBorderTopColor:
                  case PtBorderRightColor:
                  case PtBorderBottomColor:
                  case PtBorderLeftColor:
                  case PtListStyleImage:
                    TtaReadByte (file, &c);
		    if (c == 'C')
		      pPR->PrValueType = PrConstStringValue;
		    if (c == 'A')
		      pPR->PrValueType = PrAttrValue;
		    else
		      pPR->PrValueType = PrNumValue;
                    TtaReadSignedShort (file, &pPR->PrIntValue);
                    break;
                  case PtFont:
                  case PtStyle:
                  case PtWeight:
                  case PtVariant:
                  case PtUnderline:
                  case PtThickness:
                  case PtDirection:
                  case PtUnicodeBidi:
                  case PtLineStyle:
                  case PtFloat:
                  case PtClear:
                  case PtDisplay:
                  case PtListStyleType:
                  case PtListStylePosition:
                  case PtBorderTopStyle:
                  case PtBorderRightStyle:
                  case PtBorderBottomStyle:
                  case PtBorderLeftStyle:
                  case PtFillRule:
                    if (!TtaReadByte (file, (unsigned char *)&pPR->PrChrValue))
                      error = True;
                    break;
                  case PtBreak1:
                  case PtBreak2:
                  case PtIndent:
                  case PtSize:
                  case PtLineSpacing:
                  case PtLineWeight:
                  case PtMarginTop:
                  case PtMarginRight:
                  case PtMarginBottom:
                  case PtMarginLeft:
                  case PtPaddingTop:
                  case PtPaddingRight:
                  case PtPaddingBottom:
                  case PtPaddingLeft:
                  case PtBorderTopWidth:
                  case PtBorderRightWidth:
                  case PtBorderBottomWidth:
                  case PtBorderLeftWidth:
                  case PtXRadius:
                  case PtYRadius:
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
                      {
                        ReadPosRule (file, &pDim->DrPosRule);
                        /* this rule cannot be overwritten */
                        pPR->PrImportant = TRUE;
                      }
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
                  case PtHyphenate:
                  case PtVertOverflow:
                  case PtHorizOverflow:
                  case PtGather:
                  case PtPageBreak:
                  case PtLineBreak:
                    TtaReadBool (file, &pPR->PrBoolValue);
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
PtrPSchema      ReadPresentationSchema (const char *fileName, PtrSSchema pSS)
{
  PtrPRule            pNextPRule;
  PtrPSchema          pPSch;
  Counter*            pCntr;
  CntrItem*           pCntrItem;
  PresConstant*       pConst;
  PresVariable*       pVar;
  PresVarItem*        pVarItem;
  PresentationBox*    pBox;
  AttributePres*      pAttrP, *pAttrOld;
  NumAttrCase*        pCase;
  PathBuffer          dirBuffer;
  BinFile             file;
  Name                name;
  char                buf[MAX_TXT_LEN];
  int                 InitialNElems, i, j, k, l;
  PtrHostView         pHostView, prevHostView;
  ThotBool            ret, ComparAttrValue;

  error = FALSE;
  pPSch = NULL;
  /* compose le nom du fichier a ouvrir */
  strncpy (dirBuffer, SchemaPath, MAX_PATH);
  MakeCompleteName (fileName, "PRS", dirBuffer, buf, &i);

  /* teste si le fichier existe */
  file = TtaReadOpen (buf);
  if (file == 0)
    {
      /* message 'Fichier inaccessible' */
      strncpy (buf, fileName, MAX_TXT_LEN - 5);
      buf[MAX_TXT_LEN - 5] = EOS;
      strcat (buf, ".PRS");
      TtaDisplayMessage (INFO, TtaGetMessage (LIB,TMSG_INCORRECT_PRS_FILE ),
                         buf);
    }
  else
    {
      GetSchPres (&pPSch);
      pPSch->PsSSchema = pSS;
      /* this schema is considered as the User Agent default stylesheet */
      pPSch->PsOrigin = Agent;
      /* acquiert un bloc pour la prochaine regle lue */
      GetPresentRule (&pNextPRule);
      pNextPRule->PrCond = NULL;
      /* met son nom dans le schema de presentation */
      if (pPSch->PsPresentName)
        TtaFreeMemory (pPSch->PsPresentName);
      pPSch->PsPresentName = TtaStrdup (fileName);
      /* lit la partie fixe du schema de presentation */
      /* lit le nom du schema de structure correspondant */
      i = 0;
      do
        if (!TtaReadByte (file, (unsigned char*)&buf[i++]))
          error = True;
      while (buf[i - 1] != EOS && i < MAX_TXT_LEN && !error) ;
      buf[MAX_TXT_LEN - 1] = EOS;
      pPSch->PsStructName = TtaStrdup (buf);
      TtaReadShort (file, &pPSch->PsStructCode);
      /* read the name of all declared views */
      error = !TtaReadShort (file, &pPSch->PsNViews);
      if (!error)
        for (i = 0; i < pPSch->PsNViews; i++)
          TtaReadName (file, (unsigned char *)pPSch->PsView[i]);
      /* read the name of all host view for each declared view */
      if (!error)
        for (i = 0; i < pPSch->PsNViews; i++)
          {
            TtaReadShort (file, &j);
            prevHostView = NULL;
            if (!error)
              for (k = 0; k < j; k++)
                {
                  pHostView = (PtrHostView)TtaGetMemory (sizeof(HostView));
                  TtaReadName (file, (unsigned char *)pHostView->HostViewName);
                  pHostView->NextHostView = NULL;
                  if (prevHostView)
                    prevHostView->NextHostView = pHostView;
                  else
                    pPSch->PsHostViewList[i] = pHostView;
                  prevHostView = pHostView;
                }
          }
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
          error = !TtaReadShort (file, &pPSch->PsPrintedView[i].VpNumber);
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
        ret = FALSE;

      if (!ret || pPSch->PsStructCode != pSS->SsCode)
        {
          FreeSchPres (pPSch, pSS);
          FreePresentRule (pNextPRule, pSS);
          pNextPRule = NULL;
          pPSch = NULL;
        }
      else
        {
          /* lit les compteurs */
          if (!error)
            for (i = 0; i < pPSch->PsNCounters; i++)
              {
                pCntr = &pPSch->PsCounter[i];
		pCntr->CnNameIndx = 0;
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
                      pCntrItem->CiCondAttrIntValue = 0;
		      pCntrItem->CiCondAttrTextValue = NULL;
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
                      TtaReadName (file, (unsigned char *)pCntr->CnTransmAttr[j]);
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
                if (!TtaReadByte (file, (unsigned char *)&pConst->PdScript))
                  error = True;
                if (!error)
                  {
                    j = 0;
                    buf[0] = EOS;
                    do
                      if (!TtaReadByte (file, (unsigned char *)&buf[j++]))
                        error = True;
                    while (buf[j - 1] != EOS && j < MAX_TXT_LEN && !error);
                    buf[MAX_TXT_LEN - 1] = EOS;
                    pConst->PdString = TtaStrdup (buf);
                  }
              }

          /* lit les variables de presentation */
          if (!error)
            {
              pPSch->PsVariable = (PresVarTable*) malloc (pPSch->PsNVariables * sizeof (PtrPresVariable));
              memset (pPSch->PsVariable, 0, pPSch->PsNVariables * sizeof (PtrPresVariable));
              pPSch->PsVariableTableSize = pPSch->PsNVariables;
              for (i = 0; i < pPSch->PsNVariables && !error; i++)
                {
                  pVar = (PtrPresVariable) malloc (sizeof (PresVariable));
                  memset (pVar, 0, sizeof (PresVariable));
                  pPSch->PsVariable->PresVar[i] = pVar;
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
            }
          /* lit les boites de presentation et de mise en page */
          if (!error)
            {
              pPSch->PsPresentBox = (PresBoxTable*) malloc (pPSch->PsNPresentBoxes * sizeof (PtrPresentationBox));
              memset (pPSch->PsPresentBox, 0, pPSch->PsNPresentBoxes * sizeof (PtrPresentationBox));
              pPSch->PsPresentBoxTableSize = pPSch->PsNPresentBoxes;
            }
          for (i = 0; i < pPSch->PsNPresentBoxes && !error; i++)
            {
              pBox = (PtrPresentationBox) malloc (sizeof (PresentationBox));
              memset (pBox, 0, sizeof (PresentationBox));
              pPSch->PsPresentBox->PresBox[i] = pBox;
              TtaReadName (file, (unsigned char *)pBox->PbName);
              pBox->PbFirstPRule = ReadPRulePtr (file, &pNextPRule);
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
                  default:
                    break;
                  }
            }
          /* lit les presentations des attributs */
          if (!error)
            {
              pPSch->PsAttrPRule = (AttrPresTable*) malloc (pSS->SsAttrTableSize * sizeof (PtrAttributePres));
              pPSch->PsNAttrPRule = (NumberTable*) malloc (pSS->SsAttrTableSize * sizeof (int));
              memset (pPSch->PsAttrPRule, 0, pSS->SsAttrTableSize * sizeof (PtrAttributePres));
              memset (pPSch->PsAttrPRule, 0, pSS->SsAttrTableSize * sizeof (int));

              for (i = 0; i < pSS->SsNAttributes && !error; i++)
                {
                  /* lecture du nombre de paquet de regles differentes */
                  TtaReadShort (file, &l);
                  pPSch->PsAttrPRule->AttrPres[i] = NULL;
                  pPSch->PsNAttrPRule->Num[i] = l;
                  if (l > 0)
                    {
                      pAttrOld = NULL;
                      while (l > 0)
                        {
                          /* allocate attribute blocks */
                          GetAttributePres (&pAttrP);
                          if (pPSch->PsAttrPRule->AttrPres[i] == NULL)
                            pPSch->PsAttrPRule->AttrPres[i] = pAttrP;
                          else if (pAttrOld != NULL)
                            pAttrOld->ApNextAttrPres = pAttrP;
                          else
                            error = TRUE;
                          pAttrOld = pAttrP;
                          l--;
                        }
                    }
                }
              for (i = 0; i < pSS->SsNAttributes && !error; i++)
                {
                  pAttrP = pPSch->PsAttrPRule->AttrPres[i];
                  for (l = pPSch->PsNAttrPRule->Num[i]; l-- > 0 && !error;
                       pAttrP = pAttrP->ApNextAttrPres)
                    {
                      error = !TtaReadShort (file, &pAttrP->ApElemType);
                      if (pAttrP->ApElemType)
                        TtaReadBool (file, &pAttrP->ApElemInherits);
                      else
                        pAttrP->ApElemInherits = FALSE;
                      switch (pSS->SsAttribute->TtAttr[i]->AttrType)
                        {
                        case AtNumAttr:
                          for (j = 0; j < MAX_PRES_ATTR_CASE; j++)
                            pAttrP->ApCase[j].CaFirstPRule = NULL;
                          error = !TtaReadShort (file,&pAttrP->ApNCases);
                          if (!error)
                            for (j = 0; j < pAttrP->ApNCases; j++)
                              {
                                pCase = &pAttrP->ApCase[j];
                                pCase->CaComparType = ReadAttrCompar (file);
                                TtaReadSignedShort (file, &pCase->CaLowerBound);
                                TtaReadSignedShort (file, &pCase->CaUpperBound);
                                pCase->CaFirstPRule = ReadPRulePtr (file,
                                                                    &pNextPRule);
                              }
                          break;
                        case AtReferenceAttr:
                          pAttrP->ApRefFirstPRule = ReadPRulePtr (file,
                                                                  &pNextPRule);
                          break;
                        case AtTextAttr:
                          TtaReadBool (file, &ComparAttrValue);
                          if (ComparAttrValue)
                            {
                              TtaReadName (file, (unsigned char *)name);
                              pAttrP->ApString = TtaStrdup (name);
                            }
                          else
                            pAttrP->ApString = NULL;
                          pAttrP->ApTextFirstPRule = ReadPRulePtr (file,
                                                                   &pNextPRule);
                          pAttrP->ApMatch = CoMatch;
                          break;
                        case AtEnumAttr:
                          for (j = 0; j <= MAX_ATTR_VAL; j++)
                            pAttrP->ApEnumFirstPRule[j] = NULL;
                          for (j = 0; j <= pSS->SsAttribute->TtAttr[i]->AttrNEnumValues; j++)
                            pAttrP->ApEnumFirstPRule[j] = ReadPRulePtr (file, &pNextPRule);
                          break;
                        }
                    }
                }
            }

          if (pSS->SsFirstDynNature == 0)
            InitialNElems = pSS->SsNRules;
          else
            InitialNElems = pSS->SsFirstDynNature - 1;
          pPSch->PsNElemPRule = InitialNElems;

          /* lit la table des pointeurs de regle de chaque type du */
          /* schema de structure */
          if (!error)
            {
              pPSch->PsElemPRule = (PtrPRuleTable*) malloc (pSS->SsRuleTableSize * sizeof (PtrPRule));
              if (!pPSch->PsElemPRule)
                error = TRUE;
            }
          if (!error)
            for (i = 0; i < InitialNElems; i++)
              pPSch->PsElemPRule->ElemPres[i] = ReadPRulePtr (file, &pNextPRule);
          /* lit toutes les regles de presentation */
          /* lit les regles standard */
          if (!error)
            ReadPRules (file, &pPSch->PsFirstDefaultPRule, &pNextPRule, pSS);

          /* les regles des boites */
          if (!error)
            for (i = 0; i < pPSch->PsNPresentBoxes; i++)
              ReadPRules (file, &pPSch->PsPresentBox->PresBox[i]->PbFirstPRule,
                          &pNextPRule, pSS);

          /* lit les regles des attributs */
          if (!error)
            for (i = 0; i < pSS->SsNAttributes; i++)
              if (!error)
                {
                  pAttrP = pPSch->PsAttrPRule->AttrPres[i];
                  for (l = pPSch->PsNAttrPRule->Num[i]; l-- > 0;
                       pAttrP = pAttrP->ApNextAttrPres)
                    {
                      if (!error)
                        switch (pSS->SsAttribute->TtAttr[i]->AttrType)
                          {
                          case AtNumAttr:
                            for (j = 0; j < pAttrP->ApNCases; j++)
                              ReadPRules (file,&pAttrP->ApCase[j].CaFirstPRule,
                                          &pNextPRule, pSS);
                            break;
                          case AtReferenceAttr:
                            ReadPRules (file, &pAttrP->ApRefFirstPRule,
                                        &pNextPRule, pSS);
                            break;
                          case AtTextAttr:
                            ReadPRules (file, &pAttrP->ApTextFirstPRule,
                                        &pNextPRule, pSS);
                            break;
                          case AtEnumAttr:
                            for (j = 0; j <= pSS->SsAttribute->TtAttr[i]->AttrNEnumValues; j++)
                              ReadPRules (file, &pAttrP->ApEnumFirstPRule[j],
                                          &pNextPRule, pSS);
                            break;
                          }
                    }
                }

          /* lit les regles des elements structures */
          if (!error)
            for (i = 0; i < InitialNElems; i++)
              ReadPRules (file, &pPSch->PsElemPRule->ElemPres[i], &pNextPRule,
                          pSS);

          if (!error)
            {
              pPSch->PsNHeirElems = (NumberTable*) malloc (pSS->SsAttrTableSize * sizeof (int));
              if (!pPSch->PsNHeirElems)
                error = True;
            }
          if (!error)
            {
              for (i = 0; i < pSS->SsNAttributes; i++)
                TtaReadShort (file, &pPSch->PsNHeirElems->Num[i]);
            }

          if (!error)
            {
              pPSch->PsNInheritedAttrs = (NumberTable*) malloc (pSS->SsRuleTableSize * sizeof (int));
              if (!pPSch->PsNInheritedAttrs)
                error = True;
            }

          if (!error)
            {
              pPSch->PsInheritedAttr = (InheritAttrTbTb*) malloc (pSS->SsRuleTableSize * sizeof (InheritAttrTable*));
              if (!pPSch->PsInheritedAttr)
                error = True;
            }

          if (!error)
            for (i = 0; i < InitialNElems; i++)
              {
                TtaReadShort (file, &pPSch->PsNInheritedAttrs->Num[i]);
                pPSch->PsInheritedAttr->ElInherit[i] = NULL;
              }

          if (!error)
            {
              pPSch->PsNComparAttrs = (NumberTable*) malloc (pSS->SsAttrTableSize * sizeof (int));
              if (!pPSch->PsNComparAttrs)
                error = True;
            }
          if (!error)
            {
              pPSch->PsComparAttr = (CompAttrTbTb*) malloc (pSS->SsAttrTableSize * sizeof (ComparAttrTable*));
              if (!pPSch->PsComparAttr)
                error = True;
            }
          if (!error)
            {
              for (i = 0; i < pSS->SsNAttributes && !error; i++)
                {
                  TtaReadShort (file, &pPSch->PsNComparAttrs->Num[i]);
                  pPSch->PsComparAttr->CATable[i] = NULL;
                }
            }

          if (!error)
            {
              pPSch->PsElemTransmit = (NumberTable*) malloc (pSS->SsRuleTableSize * sizeof (int));
              if (!pPSch->PsElemTransmit)
                error = True;
            }
          if (!error)
            for (i = 0; i < InitialNElems; i++)
              TtaReadShort (file, &pPSch->PsElemTransmit->Num[i]);
          if (!error)
            TtaReadShort (file, &pPSch->PsNTransmElems);
          if (!error)
            for (i = 0; i < pPSch->PsNTransmElems; i++)
              {
                TtaReadShort (file, &pPSch->PsTransmElem[i].TeTargetDoc);
                TtaReadName (file, (unsigned char *)pPSch->PsTransmElem[i].TeTargetAttr);
              }
          FreePresentRule (pNextPRule, pSS);
        }
      /* ferme le fichier */
      TtaReadClose (file);
    }
  if (error)
    {
      /* message 'Schema incorrect' */
      TtaDisplayMessage (INFO, TtaGetMessage (LIB, TMSG_INCORRECT_PRS_FILE),
                         fileName);
      FreeSchPres (pPSch, pSS);
      return NULL;
    }
  else
    return pPSch;
}
