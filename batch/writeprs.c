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
 * Ce module sauve dans un fichier un schema de presentation qui a
 * ete compile' en memoire
 *
 * Authors: V. Quint (INRIA)
 *          C. Roisin (INRIA) - Columns and pages
 *
 */

#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "constpiv.h"
#include "fileaccess.h"

static BinFile      outfile;

#include "memory_f.h"
#include "fileaccess_f.h"
#include "writeprs_f.h"

/*----------------------------------------------------------------------
   WriteShort							
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         WriteShort (int n)
#else  /* __STDC__ */
static void         WriteShort (n)
int                 n;

#endif /* __STDC__ */
{
   TtaWriteByte (outfile, (char) (n / 256));
   TtaWriteByte (outfile, (char) (n % 256));
}

/*----------------------------------------------------------------------
   WriteSignedShort						
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         WriteSignedShort (int n)
#else  /* __STDC__ */
static void         WriteSignedShort (n)
int                 n;

#endif /* __STDC__ */
{
   if (n >= 0)
      WriteShort (n);
   else
      WriteShort (n + 65536);
}

/*----------------------------------------------------------------------
   WriteName							
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         WriteName (Name n)
#else  /* __STDC__ */
static void         WriteName (n)
Name                n;

#endif /* __STDC__ */
{
   int                 i;

   i = 0;
   do
      TtaWriteByte (outfile, n[i++]);
   while (n[i - 1] != '\0');
}


/*----------------------------------------------------------------------
   WriteRulePtr							
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         WriteRulePtr (PtrPRule p)
#else  /* __STDC__ */
static void         WriteRulePtr (p)
PtrPRule            p;

#endif /* __STDC__ */
{
   if (p == NULL)
      TtaWriteByte (outfile, '\0');
   else
      TtaWriteByte (outfile, '\1');
}


/*----------------------------------------------------------------------
   WriteBoolean							
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         WriteBoolean (boolean b)
#else  /* __STDC__ */
static void         WriteBoolean (b)
boolean             b;

#endif /* __STDC__ */
{
   if (b)
      TtaWriteByte (outfile, '\1');
   else
      TtaWriteByte (outfile, '\0');
}

/*----------------------------------------------------------------------
   WriteUnit ecrit une unite dans le fichier sur un octet          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         WriteUnit (TypeUnit unit)
#else  /* __STDC__ */
static void         WriteUnit (unit)
BinFile             file;
TypeUnit            unit;

#endif /* __STDC__ */
{
   switch (unit)
	 {
	    case UnRelative:
	       TtaWriteByte (outfile, C_UNIT_REL);
	       break;
	    case UnXHeight:
	       TtaWriteByte (outfile, C_UNIT_XHEIGHT);
	       break;
	    case UnPoint:
	       TtaWriteByte (outfile, C_UNIT_POINT);
	       break;
	    case UnPixel:
	       TtaWriteByte (outfile, C_UNIT_PIXEL);
	       break;
	    case UnPercent:
	       TtaWriteByte (outfile, C_UNIT_PERCENT);
	       break;
	    default:
	       fprintf (stderr, "Invalid unit %X\n", (int)unit);
	       TtaWriteByte (outfile, C_UNIT_REL);
	       break;
	 }
}

/*----------------------------------------------------------------------
   WritePRuleType  ecrit un type de regle de presentation		
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                WritePRuleType (PRuleType ruleType)
#else  /* __STDC__ */
void                WritePRuleType (ruleType)
PRuleType           ruleType;

#endif /* __STDC__ */
{
   switch (ruleType)
	 {
	    case PtVisibility:
	       TtaWriteByte (outfile, C_PR_VISIBILITY);
	       break;
	    case PtFunction:
	       TtaWriteByte (outfile, C_PR_FUNCTION);
	       break;
	    case PtVertRef:
	       TtaWriteByte (outfile, C_PR_VREF);
	       break;
	    case PtHorizRef:
	       TtaWriteByte (outfile, C_PR_HREF);
	       break;
	    case PtHeight:
	       TtaWriteByte (outfile, C_PR_HEIGHT);
	       break;
	    case PtWidth:
	       TtaWriteByte (outfile, C_PR_WIDTH);
	       break;
	    case PtVertPos:
	       TtaWriteByte (outfile, C_PR_VPOS);
	       break;
	    case PtHorizPos:
	       TtaWriteByte (outfile, C_PR_HPOS);
	       break;
	    case PtSize:
	       TtaWriteByte (outfile, C_PR_SIZE);
	       break;
	    case PtUnderline:
	       TtaWriteByte (outfile, C_PR_UNDERLINE);
	       break;
	    case PtThickness:
	       TtaWriteByte (outfile, C_PR_UNDER_THICK);
	       break;
	    case PtStyle:
	       TtaWriteByte (outfile, C_PR_STYLE);
	       break;
	    case PtFont:
	       TtaWriteByte (outfile, C_PR_FONT);
	       break;
	    case PtBreak1:
	       TtaWriteByte (outfile, C_PR_BREAK1);
	       break;
	    case PtBreak2:
	       TtaWriteByte (outfile, C_PR_BREAK2);
	       break;
	    case PtIndent:
	       TtaWriteByte (outfile, C_PR_INDENT);
	       break;
	    case PtLineSpacing:
	       TtaWriteByte (outfile, C_PR_LINESPACING);
	       break;
	    case PtAdjust:
	       TtaWriteByte (outfile, C_PR_ADJUST);
	       break;
	    case PtJustify:
	       TtaWriteByte (outfile, C_PR_JUSTIFY);
	       break;
	    case PtHyphenate:
	       TtaWriteByte (outfile, C_PR_HYPHENATE);
	       break;
	    case PtDepth:
	       TtaWriteByte (outfile, C_PR_DEPTH);
	       break;
	    case PtLineStyle:
	       TtaWriteByte (outfile, C_PR_LINESTYLE);
	       break;
	    case PtLineWeight:
	       TtaWriteByte (outfile, C_PR_LINEWEIGHT);
	       break;
	    case PtFillPattern:
	       TtaWriteByte (outfile, C_PR_FILLPATTERN);
	       break;
	    case PtBackground:
	       TtaWriteByte (outfile, C_PR_BACKGROUND);
	       break;
	    case PtForeground:
	       TtaWriteByte (outfile, C_PR_FOREGROUND);
	       break;
	    case PtHorizOverflow:
	       TtaWriteByte (outfile, C_PR_HORIZOVERFLOW);
	       break;
	    case PtVertOverflow:
	       TtaWriteByte (outfile, C_PR_VERTOVERFLOW);
	       break;
	    default:
	       fprintf (stderr, "Invalid rule type %X\n", (int)ruleType);
	       break;
	 }
}


/*----------------------------------------------------------------------
   WritePresMode ecrit un mode de calcul dans le fichier		
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         WritePresMode (PresMode mode)

#else  /* __STDC__ */
static void         WritePresMode (mode)
PresMode            mode;

#endif /* __STDC__ */
{
   switch (mode)
	 {
	    case PresImmediate:
	       TtaWriteByte (outfile, C_IMMEDIATE);
	       break;
	    case PresInherit:
	       TtaWriteByte (outfile, C_INHERIT);
	       break;
	    case PresFunction:
	       TtaWriteByte (outfile, C_PRES_FUNCTION);
	       break;
	    default:
	       fprintf (stderr, "Invalid mode %X\n", mode);
	       break;
	 }
}


/*----------------------------------------------------------------------
   WriteInheritMode       ecrit un type d'heritage dans le fichier	
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         WriteInheritMode (InheritMode mode)
#else  /* __STDC__ */
static void         WriteInheritMode (mode)
InheritMode         mode;

#endif /* __STDC__ */
{
   switch (mode)
	 {
	    case InheritParent:
	       TtaWriteByte (outfile, C_INH_ASCEND);
	       break;
	    case InheritPrevious:
	       TtaWriteByte (outfile, C_INH_PREVIOUS);
	       break;
	    case InheritChild:
	       TtaWriteByte (outfile, C_INH_DESC);
	       break;
	    case InheritCreator:
	       TtaWriteByte (outfile, C_INH_CREATOR);
	       break;
	    case InheritGrandFather:
	       TtaWriteByte (outfile, C_INH_GRAND_FATHER);
	       break;
	    default:
	       fprintf (stderr, "Invalid inherit %X\n", mode);
	       break;
	 }
}


/*----------------------------------------------------------------------
   WriteFunctionType  ecrit un type de fonction de presentation	
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         WriteFunctionType (FunctionType functType, boolean rep)

#else  /* __STDC__ */
static void         WriteFunctionType (functType, rep)
FunctionType        functType;
boolean             rep;

#endif /* __STDC__ */
{
   switch (functType)
	 {
	    case FnLine:
	       TtaWriteByte (outfile, C_PF_LINE);
	       break;
	    case FnPage:
	       TtaWriteByte (outfile, C_PF_PAGE);
	       break;
	    case FnCreateBefore:
	       if (rep)
		  TtaWriteByte (outfile, C_PF_CR_BEFORE_REP);
	       else
		  TtaWriteByte (outfile, C_PF_CR_BEFORE);
	       break;
	    case FnCreateWith:
	       TtaWriteByte (outfile, C_PF_CR_WITH);
	       break;
	    case FnCreateFirst:
	       if (rep)
		  TtaWriteByte (outfile, C_PF_CR_FIRST_REP);
	       else
		  TtaWriteByte (outfile, C_PF_CR_FIRST);
	       break;
	    case FnCreateLast:
	       if (rep)
		  TtaWriteByte (outfile, C_PF_CR_LAST_REP);
	       else
		  TtaWriteByte (outfile, C_PF_CR_LAST);
	       break;
	    case FnCreateAfter:
	       if (rep)
		  TtaWriteByte (outfile, C_PF_CR_AFTER_REP);
	       else
		  TtaWriteByte (outfile, C_PF_CR_AFTER);
	       break;
	    case FnCreateEnclosing:
	       if (rep)
		  TtaWriteByte (outfile, C_PF_CR_ENCLOSING_REP);
	       else
		  TtaWriteByte (outfile, C_PF_CR_ENCLOSING);
	       break;
	    case FnColumn:
	       TtaWriteByte (outfile, C_PF_COLUMN);
	       break;
	    case FnSubColumn:
	       TtaWriteByte (outfile, C_PF_SUBCOLUMN);
	       break;
	    case FnCopy:
	       TtaWriteByte (outfile, C_PF_COPY);
	       break;
	    case FnContentRef:
	       TtaWriteByte (outfile, C_PF_REF_CONTENT);
	       break;
	    case FnNoLine:
	       TtaWriteByte (outfile, C_PF_NOLINE);
	       break;
	    case FnShowBox:
	       TtaWriteByte (outfile, C_PF_SHOWBOX);
	       break;
	    case FnBackgroundPicture:
	       TtaWriteByte (outfile, C_PF_BGPICTURE);
	       break;
	    case FnPictureMode:
	       TtaWriteByte (outfile, C_PF_PICTUREMODE);
	       break;
	    case FnNotInLine:
	       TtaWriteByte (outfile, C_PF_NOTINLINE);
	       break;
	    default:
	       fprintf (stderr, "Invalid function %X\n", functType);
	       break;
	 }
}


/*----------------------------------------------------------------------
   WriteAlignment    ecrit un mode d'alignement de lignes		
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         WriteAlignment (BAlignment align)
#else  /* __STDC__ */
static void         WriteAlignment (align)
BAlignment          align;

#endif /* __STDC__ */
{
   switch (align)
	 {
	    case AlignLeft:
	       TtaWriteByte (outfile, C_PIV_LEFT);
	       break;
	    case AlignRight:
	       TtaWriteByte (outfile, C_PIV_RIGHT);
	       break;
	    case AlignCenter:
	       TtaWriteByte (outfile, C_PIV_CENTERED);
	       break;
	    case AlignLeftDots:
	       TtaWriteByte (outfile, C_PIV_LEFTDOT);
	       break;
	    default:
	       fprintf (stderr, "Invalid alignment %X\n", align);
	       break;
	 }
}


/*----------------------------------------------------------------------
   WritePresCondition      ecrit un type de condition		
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                WritePresCondition (PresCondition cond)
#else  /* __STDC__ */
void                WritePresCondition (cond)
PresCondition       cond;

#endif /* __STDC__ */
{
   switch (cond)
	 {
	    case PcFirst:
	       TtaWriteByte (outfile, C_COND_FIRST);
	       break;
	    case PcLast:
	       TtaWriteByte (outfile, C_COND_LAST);
	       break;
	    case PcReferred:
	       TtaWriteByte (outfile, C_COND_REFERRED);
	       break;
	    case PcFirstRef:
	       TtaWriteByte (outfile, C_COND_FIRSTREF);
	       break;
	    case PcLastRef:
	       TtaWriteByte (outfile, C_COND_LASTREF);
	       break;
	    case PcExternalRef:
	       TtaWriteByte (outfile, C_COND_EXTREF);
	       break;
	    case PcInternalRef:
	       TtaWriteByte (outfile, C_COND_INTREF);
	       break;
	    case PcCopyRef:
	       TtaWriteByte (outfile, C_COND_COPYREF);
	       break;
	    case PcAnyAttributes:
	       TtaWriteByte (outfile, C_COND_ATTR);
	       break;
	    case PcFirstAttr:
	       TtaWriteByte (outfile, C_COND_FIRST_ATTR);
	       break;
	    case PcLastAttr:
	       TtaWriteByte (outfile, C_COND_LAST_ATTR);
	       break;
	    case PcUserPage:
	       TtaWriteByte (outfile, C_COND_USERPAGE);
	       break;
	    case PcStartPage:
	       TtaWriteByte (outfile, C_COND_STARTPAGE);
	       break;
	    case PcComputedPage:
	       TtaWriteByte (outfile, C_COND_COMPPAGE);
	       break;
	    case PcEmpty:
	       TtaWriteByte (outfile, C_COND_EMPTY);
	       break;
	    case PcEven:
	       TtaWriteByte (outfile, C_COND_EVEN);
	       break;
	    case PcOdd:
	       TtaWriteByte (outfile, C_COND_ODD);
	       break;
	    case PcOne:
	       TtaWriteByte (outfile, C_COND_ONE);
	       break;
	    case PcInterval:
	       TtaWriteByte (outfile, C_COND_INTER);
	       break;
	    case PcWithin:
	       TtaWriteByte (outfile, C_COND_ANCEST);
	       break;
	    case PcElemType:
	       TtaWriteByte (outfile, C_COND_ELEM);
	       break;
	    case PcAttribute:
	       TtaWriteByte (outfile, C_COND_HAS_ATTR);
	       break;
	    case PcNoCondition:
	       TtaWriteByte (outfile, C_COND_NOCOND);
	       break;
	    case PcDefaultCond:
	       TtaWriteByte (outfile, C_COND_DEFAULT);
	       break;
	    default:
	       fprintf (stderr, "Invalid condition %X\n", cond);
	       break;
	 }
}


/*----------------------------------------------------------------------
   WriteCounterValue ecrit la nature de la condition sur un compteur 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         WriteCounterValue (CounterValue val)
#else  /* __STDC__ */
static void         WriteCounterValue (val)
CounterValue        val;

#endif /* __STDC__ */
{
   switch (val)
	 {
	    case CntMaxVal:
	       TtaWriteByte (outfile, C_VAL_MAX);
	       break;
	    case CntMinVal:
	       TtaWriteByte (outfile, C_VAL_MIN);
	       break;
	    case CntCurVal:
	       TtaWriteByte (outfile, C_VAL_CUR);
	       break;
	    default:
	       fprintf (stderr, "Invalid counter cond. %X\n", val);
	       break;
	 }
}


/*----------------------------------------------------------------------
   WriteArithRel     ecrit la relation de la condition Within	
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         WriteArithRel (ArithRel rel)
#else  /* __STDC__ */
static void         WriteArithRel (rel)
ArithRel            rel;

#endif /* __STDC__ */
{
   switch (rel)
	 {
	    case CondGreater:
	       TtaWriteByte (outfile, C_WITHIN_GT);
	       break;
	    case CondLess:
	       TtaWriteByte (outfile, C_WITHIN_LT);
	       break;
	    case CondEquals:
	       TtaWriteByte (outfile, C_WITHIN_EQ);
	       break;
	    default:
	       fprintf (stderr, "Invalid relationship %X\n", rel);
	       break;
	 }
}

/*----------------------------------------------------------------------
   WriteBoxEdge      ecrit un repere de boite			
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         WriteBoxEdge (BoxEdge edge)
#else  /* __STDC__ */
static void         WriteBoxEdge (edge)
BoxEdge             edge;

#endif /* __STDC__ */
{
   switch (edge)
	 {
	    case Top:
	       TtaWriteByte (outfile, C_AX_TOP);
	       break;
	    case Bottom:
	       TtaWriteByte (outfile, C_AX_BOTTOM);
	       break;
	    case Left:
	       TtaWriteByte (outfile, C_AX_LEFT);
	       break;
	    case Right:
	       TtaWriteByte (outfile, C_AX_RIGHT);
	       break;
	    case HorizRef:
	       TtaWriteByte (outfile, C_AX_HREF);
	       break;
	    case VertRef:
	       TtaWriteByte (outfile, C_AX_VREF);
	       break;
	    case HorizMiddle:
	       TtaWriteByte (outfile, C_AX_HMIDDLE);
	       break;
	    case VertMiddle:
	       TtaWriteByte (outfile, C_AX_VMIDDLE);
	       break;
	    case NoEdge:
	       TtaWriteByte (outfile, C_AX_NULL);
	       break;
	    default:
	       fprintf (stderr, "Invalid side %X\n", edge);
	       break;
	 }
}


/*----------------------------------------------------------------------
   WriteLevel     ecrit un niveau relatif de boite			
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         WriteLevel (Level level)
#else  /* __STDC__ */
static void         WriteLevel (level)
Level               level;

#endif /* __STDC__ */
{
   switch (level)
	 {
	    case RlEnclosing:
	       TtaWriteByte (outfile, C_PARENT);
	       break;
	    case RlSameLevel:
	       TtaWriteByte (outfile, C_SAME_LEVEL);
	       break;
	    case RlEnclosed:
	       TtaWriteByte (outfile, C_CHILD);
	       break;
	    case RlPrevious:
	       TtaWriteByte (outfile, C_PREVIOUS);
	       break;
	    case RlNext:
	       TtaWriteByte (outfile, C_NEXT);
	       break;
	    case RlSelf:
	       TtaWriteByte (outfile, C_SELF);
	       break;
	    case RlContainsRef:
	       TtaWriteByte (outfile, C_CONTAINS_REF);
	       break;
	    case RlRoot:
	       TtaWriteByte (outfile, C_ROOT);
	       break;
	    case RlReferred:
	       TtaWriteByte (outfile, C_REFERRED);
	       break;
	    case RlCreator:
	       TtaWriteByte (outfile, C_CREATOR);
	       break;
	    default:
	       fprintf (stderr, "Invalid level %X\n", level);
	       break;
	 }
}


/*----------------------------------------------------------------------
   WriteCounterOp  ecrit un type d'operation sur compteur		
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         WriteCounterOp (CounterOp op)
#else  /* __STDC__ */
static void         WriteCounterOp (op)
CounterOp           op;

#endif /* __STDC__ */
{
   switch (op)
	 {
	    case CntrSet:
	       TtaWriteByte (outfile, C_CNT_SET);
	       break;
	    case CntrAdd:
	       TtaWriteByte (outfile, C_CNT_ADD);
	       break;
	    case CntrRank:
	       TtaWriteByte (outfile, C_CNT_RANK);
	       break;
	    case CntrRLevel:
	       TtaWriteByte (outfile, C_CNT_RLEVEL);
	       break;
	    default:
	       fprintf (stderr, "Invalid operator %X\n", op);
	       break;
	 }
}


/*----------------------------------------------------------------------
   WriteBasicType ecrit un type de base				
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         WriteBasicType (BasicType typ)
#else  /* __STDC__ */
static void         WriteBasicType (typ)
BasicType           typ;

#endif /* __STDC__ */
{
   switch (typ)
	 {
	    case CharString:
	       TtaWriteByte (outfile, C_CHAR_STRING);
	       break;
	    case GraphicElem:
	       TtaWriteByte (outfile, C_GRAPHICS);
	       break;
	    case Symbol:
	       TtaWriteByte (outfile, C_SYMBOL);
	       break;
	    case Picture:
	       TtaWriteByte (outfile, C_PICTURE);
	       break;
	    case Refer:
	       TtaWriteByte (outfile, C_REFER);
	       break;
	    case PageBreak:
	       TtaWriteByte (outfile, C_PAGE_BREAK);
	       break;
	    default:
	       fprintf (stderr, "Invalid type %X\n", typ);
	       break;
	 }
}


/*----------------------------------------------------------------------
   WriteVariableType    ecrit un type de variable			
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                WriteVariableType (VariableType typ)
#else  /* __STDC__ */
void                WriteVariableType (typ)
VariableType        typ;

#endif /* __STDC__ */
{
   switch (typ)
	 {
	    case VarText:
	       TtaWriteByte (outfile, C_VAR_TEXT);
	       break;
	    case VarCounter:
	       TtaWriteByte (outfile, C_VAR_COUNTER);
	       break;
	    case VarAttrValue:
	       TtaWriteByte (outfile, C_VAR_ATTR_VAL);
	       break;
	    case VarDate:
	       TtaWriteByte (outfile, C_VAR_DATE);
	       break;
	    case VarFDate:
	       TtaWriteByte (outfile, C_VAR_FDATE);
	       break;
	    case VarDirName:
	       TtaWriteByte (outfile, C_VAR_DIRNAME);
	       break;
	    case VarDocName:
	       TtaWriteByte (outfile, C_VAR_DOCNAME);
	       break;
	    case VarElemName:
	       TtaWriteByte (outfile, C_VAR_ELEMNAME);
	       break;
	    case VarAttrName:
	       TtaWriteByte (outfile, C_VAR_ATTRNAME);
	       break;
	    case VarPageNumber:
	       TtaWriteByte (outfile, C_VAR_PAGENUMBER);
	       break;
	    default:
	       fprintf (stderr, "Invalid variable %X\n", typ);
	       break;
	 }
}


/*----------------------------------------------------------------------
   WriteCounterStyle ecrit un style de compteur			
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         WriteCounterStyle (CounterStyle style)
#else  /* __STDC__ */
static void         WriteCounterStyle (style)
CounterStyle        style;

#endif /* __STDC__ */
{
   switch (style)
	 {
	    case CntArabic:
	       TtaWriteByte (outfile, C_NUM_ARABIC);
	       break;
	    case CntURoman:
	       TtaWriteByte (outfile, C_NUM_ROMAN);
	       break;
	    case CntLRoman:
	       TtaWriteByte (outfile, C_NUM_LOWER_ROMAN);
	       break;
	    case CntUppercase:
	       TtaWriteByte (outfile, C_NUM_UPPERCASE);
	       break;
	    case CntLowercase:
	       TtaWriteByte (outfile, C_NUM_LOWERCASE);
	       break;
	    default:
	       fprintf (stderr, "Invalid style %X\n", style);
	       break;
	 }
}


/*----------------------------------------------------------------------
   WriteContentType   ecrit un type de contenu			
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                WriteContentType (ContentType typ)
#else  /* __STDC__ */
void                WriteContentType (typ)
ContentType         typ;

#endif /* __STDC__ */
{
   switch (typ)
	 {
	    case FreeContent:
	       TtaWriteByte (outfile, C_CONT_FREE);
	       break;
	    case ContVariable:
	       TtaWriteByte (outfile, C_CONT_VAR);
	       break;
	    case ContConst:
	       TtaWriteByte (outfile, C_CONT_CONST);
	       break;
	    case ContElement:
	       TtaWriteByte (outfile, C_CONT_ELEM);
	       break;
	    default:
	       fprintf (stderr, "Invalid content %X\n", typ);
	       break;
	 }
}


/*----------------------------------------------------------------------
   WriteAttrComparType						
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         WriteAttrComparType (AttrComparType typ)
#else  /* __STDC__ */
static void         WriteAttrComparType (typ)
AttrComparType      typ;

#endif /* __STDC__ */
{
   switch (typ)
	 {
	    case ComparConstant:
	       TtaWriteByte (outfile, C_COMP_CONST);
	       break;
	    case ComparAttr:
	       TtaWriteByte (outfile, C_COMP_ATTR);
	       break;
	    default:
	       fprintf (stderr, "Invalid comparison %X\n", typ);
	       break;
	 }
}

/*----------------------------------------------------------------------
   WriteRefKind						
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         WriteRefKind (RefKind kind)
#else  /* __STDC__ */
static void         WriteRefKind (kind)
RefKind      kind;

#endif /* __STDC__ */
{
   switch (kind)
	 {
	    case RkElType:
	       TtaWriteByte (outfile, C_KIND_ELEMENT_TYPE);
	       break;
	    case RkPresBox:
	       TtaWriteByte (outfile, C_KIND_PRES_BOX);
	       break;
	    case RkAttr:
	       TtaWriteByte (outfile, C_KIND_ATTRIBUTE);
	       break;
	    default:
	       fprintf (stderr, "Invalid reference %X\n", kind);
	       break;
	 }
}

/*----------------------------------------------------------------------
   WritePosRule   ecrit un positionnement relatif			
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         WritePosRule (PosRule posRule)
#else  /* __STDC__ */
static void         WritePosRule (posRule)
PosRule             posRule;

#endif /* __STDC__ */
{
   WriteBoxEdge (posRule.PoPosDef);
   WriteBoxEdge (posRule.PoPosRef);
   WriteUnit (posRule.PoDistUnit);
   WriteBoolean (posRule.PoDistAttr);
   WriteSignedShort (posRule.PoDistance);
   WriteLevel (posRule.PoRelation);
   WriteBoolean (posRule.PoNotRel);
   WriteBoolean (posRule.PoUserSpecified);
   WriteRefKind (posRule.PoRefKind);
   WriteShort (posRule.PoRefIdent);
}


/*----------------------------------------------------------------------
   WritePRules   ecrit la chaine de regle de presentation qui	
   commence par la regle pointee par pPRule.			
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                WritePRules (PtrPRule pPRule)
#else  /* __STDC__ */
void                WritePRules (pPRule)
PtrPRule            pPRule;

#endif /* __STDC__ */
{
   PtrPRule            currentRule;
   PtrCondition        pCond;
   DimensionRule      *pDim;
   int                 i;

   currentRule = pPRule;
   while (currentRule != NULL)
     {
	WritePRuleType (currentRule->PrType);
	WriteRulePtr (currentRule->PrNextPRule);
	pCond = currentRule->PrCond;
	while (pCond != NULL)
	  {
	     WritePresCondition (pCond->CoCondition);
	     WriteBoolean (pCond->CoNotNegative);
	     WriteBoolean (pCond->CoTarget);
	     switch (pCond->CoCondition)
		   {
		      case PcEven:
		      case PcOdd:
		      case PcOne:
			 WriteShort (pCond->CoCounter);
			 break;
		      case PcInterval:
			 WriteShort (pCond->CoCounter);
			 WriteSignedShort (pCond->CoMinCounter);
			 WriteSignedShort (pCond->CoMaxCounter);
			 WriteCounterValue (pCond->CoValCounter);
			 break;
		      case PcWithin:
			 WriteBoolean (pCond->CoImmediate);
			 WriteShort (pCond->CoRelation);
			 WriteArithRel (pCond->CoAncestorRel);
			 WriteSignedShort (pCond->CoTypeAncestor);
			 if (pCond->CoTypeAncestor == 0)
			   {
			      WriteName (pCond->CoAncestorName);
			      WriteName (pCond->CoSSchemaName);
			   }
			 break;
		      case PcElemType:
		      case PcAttribute:
			 WriteSignedShort (pCond->CoTypeElAttr);
			 break;
		      default:
			 break;
		   }
	     pCond = pCond->CoNextCondition;
	  }
	WritePresCondition (PcNoCondition);
	WriteShort (currentRule->PrViewNum);
	WritePresMode (currentRule->PrPresMode);
	switch (currentRule->PrPresMode)
	      {
		 case PresInherit:
		    WriteInheritMode (currentRule->PrInheritMode);
		    WriteBoolean (currentRule->PrInhAttr);
		    WriteSignedShort (currentRule->PrInhDelta);
		    WriteBoolean (currentRule->PrMinMaxAttr);
		    WriteSignedShort (currentRule->PrInhMinOrMax);
		    WriteUnit (currentRule->PrInhUnit);
		    break;
		 case PresFunction:
		    WriteFunctionType (currentRule->PrPresFunction,
				       currentRule->PrPresBoxRepeat);
		    if (currentRule->PrPresFunction != FnLine
			&& currentRule->PrPresFunction != FnNoLine
			&& currentRule->PrPresFunction != FnShowBox
			&& currentRule->PrPresFunction != FnNotInLine)
		      {
			 WriteBoolean (currentRule->PrExternal);
			 WriteBoolean (currentRule->PrElement);
			 WriteShort (currentRule->PrNPresBoxes);
			 if (currentRule->PrNPresBoxes == 0)
			    WriteName (currentRule->PrPresBoxName);
			 else
			    for (i = 0; i < currentRule->PrNPresBoxes; i++)
			       WriteShort (currentRule->PrPresBox[i]);
		      }
		    break;
		 case PresImmediate:
		    switch (currentRule->PrType)
			  {
			     case PtVisibility:
			     case PtDepth:
			     case PtFunction:
			     case PtFillPattern:
			     case PtBackground:
			     case PtForeground:
				WriteBoolean (currentRule->PrAttrValue);
				WriteSignedShort (currentRule->PrIntValue);
				break;
			     case PtFont:
			     case PtStyle:
			     case PtUnderline:
			     case PtThickness:
			     case PtLineStyle:
				TtaWriteByte (outfile, currentRule->PrChrValue);
				break;
			     case PtBreak1:
			     case PtBreak2:
			     case PtIndent:
			     case PtSize:
			     case PtLineSpacing:
			     case PtLineWeight:
				WriteUnit (currentRule->PrMinUnit);
				WriteBoolean (currentRule->PrMinAttr);
				WriteSignedShort (currentRule->PrMinValue);
				break;
			     case PtVertRef:
			     case PtHorizRef:
			     case PtVertPos:
			     case PtHorizPos:
				WritePosRule (currentRule->PrPosRule);
				break;
			     case PtHeight:
			     case PtWidth:
				pDim = &currentRule->PrDimRule;
				WriteBoolean (pDim->DrPosition);
				if (pDim->DrPosition)
				   WritePosRule (pDim->DrPosRule);
				else
				  {
				     WriteBoolean (pDim->DrAbsolute);
				     WriteBoolean (pDim->DrSameDimens);
				     WriteUnit (pDim->DrUnit);
				     WriteBoolean (pDim->DrAttr);
				     WriteBoolean (pDim->DrMin);
				     WriteBoolean (pDim->DrUserSpecified);
				     WriteSignedShort (pDim->DrValue);
				     WriteLevel (pDim->DrRelation);
				     WriteBoolean (pDim->DrNotRelat);
				     WriteRefKind (pDim->DrRefKind);
				     WriteShort (pDim->DrRefIdent);
				  }
				break;
			     case PtAdjust:
				WriteAlignment (currentRule->PrAdjust);
				break;
			     case PtJustify:
			     case PtHyphenate:
			     case PtVertOverflow:
			     case PtHorizOverflow:
				WriteBoolean (currentRule->PrJustify);
				break;
			     case PtPictInfo:
				break;
			  }

		    break;
	      }
        /* DO NOT free the poresentation rule that has just been written! */
        /* It may point at condition that is shared with other rules */
	/* This condition would be freed several times!!! */

        /* get next presentation rule */
	currentRule = currentRule->PrNextPRule;
     }
}


/*----------------------------------------------------------------------
   WritePresentationSchema    cree le fichier de sortie et y ecrit	
   le schema de presentation					
  ----------------------------------------------------------------------*/

#ifdef __STDC__
boolean             WritePresentationSchema (Name fileName, PtrPSchema pPSch, PtrSSchema pSS)

#else  /* __STDC__ */
boolean             WritePresentationSchema (fileName, pPSch, pSS)
Name                fileName;
PtrPSchema          pPSch;
PtrSSchema          pSS;

#endif /* __STDC__ */

{
   Counter            *pCntr;
   CntrItem           *pCItem;
   PresConstant       *pConst;
   PresVariable       *pVar;
   PresVarItem        *pVarItem;
   PresentationBox    *pBox;
   AttributePres      *pAttPres;
   NumAttrCase        *pCase;
   int                 i, j, k;

   /* cree le fichier */
   outfile = TtaWriteOpen (fileName);
   if (outfile == 0)
      /* echec */
      return False;
   /* copie le code d'identification du schema de structure */
   pPSch->PsStructCode = pSS->SsCode;

   /* ecrit la partie fixe */
   WriteName (pPSch->PsStructName);
   WriteShort (pPSch->PsStructCode);
   WriteShort (pPSch->PsNViews);
   for (i = 0; i < pPSch->PsNViews; i++)
      WriteName (pPSch->PsView[i]);
   for (i = 0; i < pPSch->PsNViews; i++)
      WriteBoolean (pPSch->PsPaginatedView[i]);
   /* significatif uniquement dans la V4 */
   for (i = 0; i < pPSch->PsNViews; i++)
      WriteBoolean (pPSch->PsColumnView[i]);
   WriteShort (pPSch->PsNPrintedViews);
   for (i = 0; i < pPSch->PsNPrintedViews; i++)
     {
	WriteBoolean (pPSch->PsPrintedView[i].VpAssoc);
	WriteShort (pPSch->PsPrintedView[i].VpNumber);
     }
   for (i = 0; i < pPSch->PsNViews; i++)
      WriteBoolean (pPSch->PsExportView[i]);
   WriteShort (pPSch->PsNCounters);
   WriteShort (pPSch->PsNConstants);
   WriteShort (pPSch->PsNVariables);
   WriteShort (pPSch->PsNPresentBoxes);
   WriteRulePtr (pPSch->PsFirstDefaultPRule);

   /* ecrit les compteurs */
   for (i = 0; i < pPSch->PsNCounters; i++)
     {
	pCntr = &pPSch->PsCounter[i];
	WriteShort (pCntr->CnNItems);
	for (j = 0; j < pCntr->CnNItems; j++)
	  {
	     pCItem = &pCntr->CnItem[j];
	     WriteCounterOp (pCItem->CiCntrOp);
	     WriteShort (pCItem->CiElemType);
	     WriteSignedShort (pCItem->CiAscendLevel);
	     WriteShort (pCItem->CiViewNum);
	     WriteSignedShort (pCItem->CiParamValue);
	     WriteShort (pCItem->CiInitAttr);
	     WriteShort (pCItem->CiReinitAttr);
	     WriteShort (pCItem->CiCondAttr);
	     WriteBoolean (pCItem->CiCondAttrPresent);
	  }
	WriteShort (pCntr->CnNPresBoxes);
	for (j = 0; j < pCntr->CnNPresBoxes; j++)
	  {
	     WriteShort (pCntr->CnPresBox[j]);
	     WriteBoolean (pCntr->CnMinMaxPresBox[j]);
	  }
	WriteShort (pCntr->CnNTransmAttrs);
	for (j = 0; j < pCntr->CnNTransmAttrs; j++)
	  {
	     WriteName (pCntr->CnTransmAttr[j]);
	     WriteShort (pCntr->CnTransmSSchemaAttr[j]);
	  }
	WriteShort (pCntr->CnNCreators);
	for (j = 0; j < pCntr->CnNCreators; j++)
	  {
	     WriteShort (pCntr->CnCreator[j]);
	     WriteBoolean (pCntr->CnMinMaxCreator[j]);
	  }
	for (j = 0; j < pCntr->CnNCreators; j++)
	   WriteBoolean (pCntr->CnPresBoxCreator[j]);
	WriteShort (pCntr->CnNCreatedBoxes);
	for (j = 0; j < pCntr->CnNCreatedBoxes; j++)
	  {
	     WriteShort (pCntr->CnCreatedBox[j]);
	     WriteBoolean (pCntr->CnMinMaxCreatedBox[j]);
	  }
	WriteBoolean (pCntr->CnPageFooter);
     }

   /* ecrit les constantes de presentation */
   for (i = 0; i < pPSch->PsNConstants; i++)
     {
	pConst = &pPSch->PsConstant[i];
	WriteBasicType (pConst->PdType);
	TtaWriteByte (outfile, pConst->PdAlphabet);
	j = 0;
	do
	   TtaWriteByte (outfile, pConst->PdString[j++]);
	while (pConst->PdString[j - 1] != '\0');
     }

   /* ecrit les variables de presentation */
   for (i = 0; i < pPSch->PsNVariables; i++)
     {
	pVar = &pPSch->PsVariable[i];
	WriteShort (pVar->PvNItems);
	for (j = 0; j < pVar->PvNItems; j++)
	  {
	     pVarItem = &pVar->PvItem[j];
	     WriteVariableType (pVarItem->ViType);
	     switch (pVarItem->ViType)
		   {
		      case VarText:
			 WriteShort (pVarItem->ViConstant);
			 break;
		      case VarCounter:
			 WriteShort (pVarItem->ViCounter);
			 WriteCounterStyle (pVarItem->ViStyle);
			 WriteCounterValue (pVarItem->ViCounterVal);
			 break;
		      case VarAttrValue:
			 WriteShort (pVarItem->ViAttr);
			 WriteCounterStyle (pVarItem->ViStyle);

			 break;
		      case VarPageNumber:
			 WriteShort (pVarItem->ViView);
			 WriteCounterStyle (pVarItem->ViStyle);
			 break;
		      default:
			 break;
		   }
	  }
     }

   /* ecrit les boites de presentation */
   for (i = 0; i < pPSch->PsNPresentBoxes; i++)
     {
	pBox = &pPSch->PsPresentBox[i];
	WriteName (pBox->PbName);
	WriteRulePtr (pBox->PbFirstPRule);
	WriteBoolean (pBox->PbAcceptPageBreak);
	WriteBoolean (pBox->PbAcceptLineBreak);
	WriteBoolean (pBox->PbBuildAll);
	WriteBoolean (pBox->PbPageFooter);
	WriteBoolean (pBox->PbPageHeader);
	WriteBoolean (pBox->PbPageBox);
	WriteShort (pBox->PbFooterHeight);
	WriteShort (pBox->PbHeaderHeight);
	WriteShort (pBox->PbPageCounter);
	WriteContentType (pBox->PbContent);
	switch (pBox->PbContent)
	      {
		 case ContVariable:
		    WriteShort (pBox->PbContVariable);
		    break;
		 case ContConst:
		    WriteShort (pBox->PbContConstant);
		    break;
		 case ContElement:
		    WriteShort (pBox->PbContElem);
		    WriteShort (pBox->PbContRefElem);
		    break;
		 default:
		    break;
	      }
     }

   /* ecrit les presentations des attributs */
   for (i = 0; i < pSS->SsNAttributes; i++)
      WriteShort (pPSch->PsNAttrPRule[i]);

   for (i = 0; i < pSS->SsNAttributes; i++)
     {
	pAttPres = pPSch->PsAttrPRule[i];
	for (k = pPSch->PsNAttrPRule[i]; k-- > 0;
	     pAttPres = pAttPres->ApNextAttrPres)
	  {
	     WriteShort (pAttPres->ApElemType);
	     switch (pSS->SsAttribute[i].AttrType)
		   {
		      case AtNumAttr:
			 WriteShort (pAttPres->ApNCases);
			 for (j = 0; j < pAttPres->ApNCases; j++)
			   {
			      pCase = &pAttPres->ApCase[j];
			      WriteAttrComparType (pCase->CaComparType);
			      WriteSignedShort (pCase->CaLowerBound);
			      WriteSignedShort (pCase->CaUpperBound);
			      WriteRulePtr (pCase->CaFirstPRule);
			   }
			 break;
		      case AtReferenceAttr:
			 WriteRulePtr (pAttPres->ApRefFirstPRule);
			 break;
		      case AtTextAttr:
			 WriteName (pAttPres->ApString);
			 WriteRulePtr (pAttPres->ApTextFirstPRule);
			 break;
		      case AtEnumAttr:
			 for (j = 0; j <= pSS->SsAttribute[i].AttrNEnumValues; j++)
			    WriteRulePtr (pAttPres->ApEnumFirstPRule[j]);
			 break;
		   }
	  }
     }

   /* ecrit la table des pointeurs de regle de chaque type du schema de */
   /* structure */
   for (i = 0; i < pSS->SsNRules; i++)
      WriteRulePtr (pPSch->PsElemPRule[i]);

   /* ecrit toutes les regles de presentation */
   /* ecrit les regles standard */
   WritePRules (pPSch->PsFirstDefaultPRule);

   /* ecrit les regles des boites */
   for (i = 0; i < pPSch->PsNPresentBoxes; i++)
      WritePRules (pPSch->PsPresentBox[i].PbFirstPRule);

   /* ecrit les regles des attributs */
   for (i = 0; i < pSS->SsNAttributes; i++)
     {
	pAttPres = pPSch->PsAttrPRule[i];
	for (k = pPSch->PsNAttrPRule[i]; k-- > 0;
	     pAttPres = pAttPres->ApNextAttrPres)
	  {
	     switch (pSS->SsAttribute[i].AttrType)
		   {
		      case AtNumAttr:
			 for (j = 0; j < pAttPres->ApNCases; j++)
			    WritePRules (pAttPres->ApCase[j].CaFirstPRule);
			 break;
		      case AtReferenceAttr:
			 WritePRules (pAttPres->ApRefFirstPRule);
			 break;
		      case AtTextAttr:
			 WritePRules (pAttPres->ApTextFirstPRule);
			 break;
		      case AtEnumAttr:
			 for (j = 0; j <= pSS->SsAttribute[i].AttrNEnumValues; j++)
			    WritePRules (pAttPres->ApEnumFirstPRule[j]);
			 break;
		   }
	  }
     }

   /* ecrit les regles des elements */
   for (i = 0; i < pSS->SsNRules; i++)
      WritePRules (pPSch->PsElemPRule[i]);

   for (i = 0; i < pSS->SsNAttributes; i++)
      WriteShort (pPSch->PsNHeirElems[i]);

   for (i = 0; i < pSS->SsNRules; i++)
      WriteShort (pPSch->PsNInheritedAttrs[i]);

   for (i = 0; i < pSS->SsNAttributes; i++)
      WriteShort (pPSch->PsNComparAttrs[i]);

   for (i = 0; i < pSS->SsNRules; i++)
      WriteBoolean (pPSch->PsAcceptPageBreak[i]);

   for (i = 0; i < pSS->SsNRules; i++)
      WriteBoolean (pPSch->PsAcceptLineBreak[i]);

   for (i = 0; i < pSS->SsNRules; i++)
      WriteBoolean (pPSch->PsBuildAll[i]);

   for (i = 0; i < pSS->SsNRules; i++)
      WriteBoolean (pPSch->PsInPageHeaderOrFooter[i]);

   for (i = 0; i < pSS->SsNRules; i++)
      WriteBoolean (pPSch->PsAssocPaginated[i]);
   for (i = 0; i < pSS->SsNRules; i++)
      WriteShort (pPSch->PsElemTransmit[i]);
   WriteShort (pPSch->PsNTransmElems);
   for (i = 0; i < pPSch->PsNTransmElems; i++)
     {
	WriteShort (pPSch->PsTransmElem[i].TeTargetDoc);
	WriteName (pPSch->PsTransmElem[i].TeTargetAttr);
     }
   TtaWriteClose (outfile);
   return True;
}
