/* ======================================================================= */
/* |                                                                    | */
/* |                           Projet THOT                              | */
/* |                                                                    | */
/* |               Chargement des schemas de presentation               | */
/* |                                                                    | */
/* |                                                                    | */
/* |                    V. Quint        Juillet 1987                    | */
/* |                                                                    | */
/* |               France Logiciel no de depot 88-39-001-00             | */
/* |                                                                    | */
/* ======================================================================= */


#include "thot_sys.h"
#include "constmedia.h"
#include "constpiv.h"
#include "typemedia.h"
#include "storage.h"
#include "libmsg.h"
#include "message.h"

#define EXPORT extern
#include "platform_tv.h"
static boolean      erreur;

#include "dofile_f.h"
#include "fileaccess_f.h"
#include "memory_f.h"
#include "readstr_f.h"
#include "readprs_f.h"


/* ---------------------------------------------------------------------- */
/* procedures pour la lecture des fichiers schemas */
/* Gestion d'erreur */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         Erreur_Present (int Erreur)
#else  /* __STDC__ */
static void         Erreur_Present (Erreur)
int                 Erreur;

#endif /* __STDC__ */
{
   erreur = TRUE;
}


/* ---------------------------------------------------------------------- */
/* rdTypeRegle  lit un type de regle de presentation dans le fichier */
/* et retourne sa valeur. */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static PRuleType    rdTypeRegle (BinFile file)
#else  /* __STDC__ */
static PRuleType    rdTypeRegle (file)
BinFile             file;

#endif /* __STDC__ */
{
   char                c;

   if (!BIOreadByte (file, &c))
     {
	c = ' ';
	Erreur_Present (1);
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
	       Erreur_Present (1);	/* erreur de codage */
	       return PtVisibility;
	       break;
	 }
}


/* ---------------------------------------------------------------------- */
/* rdModeCalcul lit un mode de calcul dans le fichier et */
/* retourne sa valeur. */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static PresMode   rdModeCalcul (BinFile file)
#else  /* __STDC__ */
static PresMode   rdModeCalcul (file)
BinFile             file;

#endif /* __STDC__ */
{
   char                c;

   if (!BIOreadByte (file, &c))
     {
	c = ' ';
	Erreur_Present (1);
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
	       Erreur_Present (1);	/* erreur de codage */
	       return PresImmediate;
	       break;
	 }
}

/* ---------------------------------------------------------------------- */
/* | rdTypeUnite lit un type d'unite dans le fichier                    | */
/* |            et retourne sa valeur.                                  | */
/* ---------------------------------------------------------------------- */
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
	Erreur_Present (1);
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
	       Erreur_Present (1);	/* erreur de codage */
	       return UnPoint;
	       break;
	 }
}

/* ---------------------------------------------------------------------- */
/* | rdTypeHeritage lit un type d'heritage dans le fichier              | */
/* |            et retourne sa valeur.                                  | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static InheritMode rdTypeHeritage (BinFile file)
#else  /* __STDC__ */
static InheritMode rdTypeHeritage (file)
BinFile             file;

#endif /* __STDC__ */
{
   char                c;

   if (!BIOreadByte (file, &c))
     {
	c = ' ';
	Erreur_Present (1);
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
	       Erreur_Present (1);	/* erreur de codage */
	       return InheritParent;
	       break;
	 }
}


/* ---------------------------------------------------------------------- */
/* rdTypeFonct  lit un type de fonction de presentation dans le fichier */
/* et retourne sa valeur. */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static FunctionType    rdTypeFonct (BinFile file, boolean * rep)
#else  /* __STDC__ */
static FunctionType    rdTypeFonct (file, rep)
BinFile             file;
boolean            *rep;

#endif /* __STDC__ */
{
   char                c;
   FunctionType           tf;

   *rep = FALSE;
   if (!BIOreadByte (file, &c))
     {
	c = ' ';
	Erreur_Present (1);
     }
   switch (c)
	 {
	    case C_PF_LINE:
	       tf = FnLine;
	       break;
	    case C_PF_PAGE:
	       tf = FnPage;
	       break;
	    case C_PF_CR_BEFORE:
	       tf = FnCreateBefore;
	       break;
	    case C_PF_CR_BEFORE_REP:
	       tf = FnCreateBefore;
	       *rep = TRUE;
	       break;
	    case C_PF_CR_WITH:
	       tf = FnCreateWith;
	       break;
	    case C_PF_CR_FIRST:
	       tf = FnCreateFirst;
	       break;
	    case C_PF_CR_FIRST_REP:
	       tf = FnCreateFirst;
	       *rep = TRUE;
	       break;
	    case C_PF_CR_LAST:
	       tf = FnCreateLast;
	       break;
	    case C_PF_CR_LAST_REP:
	       tf = FnCreateLast;
	       *rep = TRUE;
	       break;
	    case C_PF_CR_AFTER:
	       tf = FnCreateAfter;
	       break;
	    case C_PF_CR_AFTER_REP:
	       tf = FnCreateAfter;
	       *rep = TRUE;
	       break;
	    case C_PF_CR_ENCLOSING:
	       tf = FnCreateEnclosing;
	       break;
	    case C_PF_CR_ENCLOSING_REP:
	       tf = FnCreateEnclosing;
	       *rep = TRUE;
	       break;
	    case C_PF_COLUMN:
	       tf = FnColumn;
	       break;
	    case C_PF_SUBCOLUMN:
	       tf = FnSubColumn;
	       break;
	    case C_PF_COPY:
	       tf = FnCopy;
	       break;
	    case C_PF_REF_CONTENT:
	       tf = FnContentRef;
	       break;
	    case C_PF_NOLINE:
	       tf = FnNoLine;
	       break;
	    default:
	       Erreur_Present (1);	/* erreur de codage */
	       tf = FnLine;
	       break;
	 }

   return tf;
}


/* rdCadrage    lit un mode de cadrage de lignes dans le fichier */
/* et retourne sa valeur. */
#ifdef __STDC__
static BAlignment      rdCadrage (BinFile file)
#else  /* __STDC__ */
static BAlignment      rdCadrage (file)
BinFile             file;

#endif /* __STDC__ */
{
   char                c;
   BAlignment             align;

   if (!BIOreadByte (file, &c))
     {
	c = ' ';
	Erreur_Present (1);
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
	       Erreur_Present (1);	/* erreur de codage */
	       align = AlignLeft;
	       break;
	 }

   return align;
}


/* rdTypeCondition      lit un type de condition dans le fichier */
/* et retourne sa valeur. */
#ifdef __STDC__
static PresCondition rdTypeCondition (BinFile file)
#else  /* __STDC__ */
static PresCondition rdTypeCondition (file)
BinFile             file;

#endif /* __STDC__ */
{
   char                c;
   PresCondition       condtype;

   if (!BIOreadByte (file, &c))
     {
	c = ' ';
	Erreur_Present (1);
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
	       Erreur_Present (1);	/* erreur de codage */
	       condtype = PcFirst;
	       break;
	 }
   return condtype;
}


/* rdRelationParent     lit la relation d'une condition Within */
#ifdef __STDC__
static ArithRel   rdRelationParent (BinFile file)
#else  /* __STDC__ */
static ArithRel   rdRelationParent (file)
BinFile             file;

#endif /* __STDC__ */
{
   char                c;
   ArithRel          ret;

   if (!BIOreadByte (file, &c))
     {
	c = ' ';
	Erreur_Present (1);
     }
   switch (c)
	 {
	    case C_WITHIN_GT:
	       ret = CondGreater;
	       break;
	    case C_WITHIN_LT:
	       ret = CondLess;
	    case C_WITHIN_EQ:
	       ret = CondEquals;
	 }
   return ret;
}


/* rdRepereBoite                lit un repere de boite dans le fichier */
/* et retourne sa valeur. */
#ifdef __STDC__
static BoxEdge  rdRepereBoite (BinFile file)
#else  /* __STDC__ */
static BoxEdge  rdRepereBoite (file)
BinFile             file;

#endif /* __STDC__ */
{
   char                c;
   BoxEdge         edge;

   if (!BIOreadByte (file, &c))
     {
	c = ' ';
	Erreur_Present (1);
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
	       Erreur_Present (1);	/* erreur de codage */
	       edge = Top;
	       break;
	 }
   return edge;
}


/* rdNiveau     lit un niveau relatif de boite dans le fichier */
/* et retourne sa valeur. */
#ifdef __STDC__
static Level       rdNiveau (BinFile file)

#else  /* __STDC__ */
static Level       rdNiveau (file)
BinFile             file;

#endif /* __STDC__ */
{
   char                c;
   Level              level;

   if (!BIOreadByte (file, &c))
     {
	c = ' ';
	Erreur_Present (1);
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
	       Erreur_Present (1);	/* erreur de codage */
	       level = RlSameLevel;
	       break;
	 }
   return level;
}


/* rdCptTypeOp  lit un type d'operation sur compteur dans le fichier */
/* et retourne sa valeur. */
#ifdef __STDC__
static CounterOp    rdCptTypeOp (BinFile file)
#else  /* __STDC__ */
static CounterOp    rdCptTypeOp (file)
BinFile             file;

#endif /* __STDC__ */
{
   char                c;
   CounterOp           optype;

   if (!BIOreadByte (file, &c))
     {
	c = ' ';
	Erreur_Present (1);
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
	       Erreur_Present (1);	/* erreur de codage */
	       optype = CntrRank;
	       break;
	 }
   return optype;
}


/* rdTypeDeBase lit un type de base dans le fichier et retourne */
/* sa valeur. */
#ifdef __STDC__
static BasicType   rdTypeDeBase (BinFile file)
#else  /* __STDC__ */
static BasicType   rdTypeDeBase (file)
BinFile             file;

#endif /* __STDC__ */
{
   char                c;
   BasicType          basictyp;

   if (!BIOreadByte (file, &c))
     {
	c = ' ';
	Erreur_Present (1);
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
	       Erreur_Present (1);	/* erreur de codage */
	       basictyp = CharString;
	       break;
	 }

   return basictyp;
}


/* rdVarType    lit un type de variable dans le fichier et */
/* retourne sa valeur. */
#ifdef __STDC__
static VariableType      rdVarType (BinFile file)

#else  /* __STDC__ */
static VariableType      rdVarType (file)
BinFile             file;

#endif /* __STDC__ */
{
   char                c;
   VariableType             vartyp;

   if (!BIOreadByte (file, &c))
     {
	c = ' ';
	Erreur_Present (1);
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
	       Erreur_Present (1);	/* erreur de codage */
	       vartyp = VarText;
	       break;
	 }
   return vartyp;
}


/* rdComptStyle lit un style de compteur dans le fichier et */
/* retourne sa valeur. */
#ifdef __STDC__
static CounterStyle   rdComptStyle (BinFile file)
#else  /* __STDC__ */
static CounterStyle   rdComptStyle (file)
BinFile             file;

#endif /* __STDC__ */
{
   char                c;
   CounterStyle          countstyle;

   if (!BIOreadByte (file, &c))
     {
	c = ' ';
	Erreur_Present (1);
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
	       Erreur_Present (1);	/* erreur de codage */
	       countstyle = CntArabic;
	       break;
	 }
   return countstyle;
}


/* rdNatValCmpt         lit la nature du compteur inclus dans une variable */
/* et retourne sa valeur. */
#ifdef __STDC__
static CounterValue rdNatValCmpt (BinFile file)
#else  /* __STDC__ */
static CounterValue rdNatValCmpt (file)
BinFile             file;

#endif /* __STDC__ */
{
   char                c;
   CounterValue      nature;


   if (!BIOreadByte (file, &c))
     {
	c = ' ';
	Erreur_Present (1);
     }
   switch (c)
	 {
	    case C_VAL_MAX:
	       nature = CntMaxVal;
	       break;
	    case C_VAL_MIN:
	       nature = CntMinVal;
	       break;
	    case C_VAL_CUR:
	       nature = CntCurVal;
	       break;
	    default:
	       Erreur_Present (1);	/* erreur de codage */
	       nature = CntCurVal;
	       break;
	 }
   return nature;
}


/* ReadContentType                lit un type de contenu dans le fichier */
/* et retourne sa valeur. */
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
	Erreur_Present (1);
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
	       Erreur_Present (1);	/* erreur de codage */
	       conttype = FreeContent;
	       break;
	 }
   return conttype;
}


/* ReadRulePtr     retourne un pointeur sur la regle suivante ou NULL */
/* s'il n'y a pas de regle suivante. */
#ifdef __STDC__
PtrPRule        ReadRulePtr (BinFile file, PtrPRule * nextr)

#else  /* __STDC__ */
PtrPRule        ReadRulePtr (file, nextr)
BinFile             file;
PtrPRule       *nextr;

#endif /* __STDC__ */
{
   char                c;

   if (!BIOreadByte (file, &c))
      return NULL;
   else if (c == '\0')
      return NULL;
   else
      return *nextr;
}


/* ReadPosition   lit un positionnement relatif */
#ifdef __STDC__
void                ReadPosition (BinFile file, PosRule * RP)
#else  /* __STDC__ */
void                ReadPosition (file, RP)
BinFile             file;
PosRule           *RP;

#endif /* __STDC__ */
{
   PosRule           *pRe1;

   pRe1 = RP;
   pRe1->PoPosDef = rdRepereBoite (file);
   pRe1->PoPosRef = rdRepereBoite (file);
   pRe1->PoDistUnit = rdUnit (file);
   BIOreadBool (file, &pRe1->PoDistAttr);
   BIOreadSignedShort (file, &pRe1->PoDistance);
   pRe1->PoRelation = rdNiveau (file);
   BIOreadBool (file, &pRe1->PoNotRel);
   BIOreadBool (file, &pRe1->PoUserSpecified);
   BIOreadBool (file, &pRe1->PoRefElem);
   if (pRe1->PoRefElem)
      BIOreadShort (file, &pRe1->PoTypeRefElem);
   else
      BIOreadShort (file, &pRe1->PoRefPresBox);

}


/* ReadAttrCompar lit un type de comparaison pour les valeurs d'attributs */
/* retourne sa valeur. */
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
	       Erreur_Present (1);	/* erreur de codage */
	       return (ComparConstant);
	       break;
	 }
}


/* ReadRules    lit une suite de regles chainees et fait pointer le */
/* pointeur pR sur la premiere regle de la suite de regles lues. */
#ifdef __STDC__
void                ReadRules (BinFile file, PtrPRule * pR, PtrPRule * nextr)
#else  /* __STDC__ */
void                ReadRules (file, pR, nextr)
BinFile             file;
PtrPRule       *pR;
PtrPRule       *nextr;

#endif /* __STDC__ */
{
   PtrPRule        r;
   int                 i;
   PtrPRule        pRe1;
   PtrCondition        pCond;
   PresCondition       typeCond;
   DimensionRule       *pRelD1;

   if (*pR != NULL && !erreur)
      /* pointeur sur la premiere regle qui va etre lue */
     {
	*pR = *nextr;		/* lecture de la suite de regles */
	do
	  {
	     r = *nextr;	/* acquiert un buffer pour la regle suivante */
	     GetPresentRule (nextr);
	     (*nextr)->PrCond = NULL;
	     /* lit une regle */
	     pRe1 = r;
	     pRe1->PrType = rdTypeRegle (file);
	     pRe1->PrNextPRule = ReadRulePtr (file, nextr);
	     pRe1->PrCond = NULL;
	     typeCond = rdTypeCondition (file);
	     while (typeCond != PcNoCondition && !erreur)
	       {
		  GetPresentRuleCond (&pCond);
		  pCond->CoNextCondition = pRe1->PrCond;
		  pRe1->PrCond = pCond;
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
			      pCond->CoValCounter = rdNatValCmpt (file);
			      break;
			   case PcWithin:
			      BIOreadBool (file, &pCond->CoImmediate);
			      BIOreadShort (file, &pCond->CoRelation);
			      pCond->CoAncestorRel = rdRelationParent (file);
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
		  typeCond = rdTypeCondition (file);
	       }
	     BIOreadShort (file, &pRe1->PrViewNum);
	     pRe1->PrPresMode = rdModeCalcul (file);
	     if (!erreur)
		switch (pRe1->PrPresMode)
		      {
			 case PresInherit:
			    pRe1->PrInheritMode = rdTypeHeritage (file);
			    BIOreadBool (file, &pRe1->PrInhAttr);
			    BIOreadSignedShort (file, &pRe1->PrInhDelta);
			    BIOreadBool (file, &pRe1->PrMinMaxAttr);
			    BIOreadSignedShort (file, &pRe1->PrInhMinOrMax);
			    pRe1->PrInhUnit = rdUnit (file);
			    break;
			 case PresFunction:
			    pRe1->PrPresBoxRepeat = FALSE;
			    pRe1->PrPresFunction = rdTypeFonct (file, &pRe1->PrPresBoxRepeat);
			    if (pRe1->PrPresFunction != FnLine
				&& pRe1->PrPresFunction != FnNoLine)
			      {
				 BIOreadBool (file, &pRe1->PrExternal);
				 BIOreadBool (file, &pRe1->PrElement);
				 erreur = !BIOreadShort (file, &pRe1->PrNPresBoxes);
				 if (!erreur)
				    if (pRe1->PrNPresBoxes == 0)
				       BIOreadName (file, pRe1->PrPresBoxName);
				    else
				      {
					 for (i = 1; i <= pRe1->PrNPresBoxes; i++)
					    BIOreadShort (file, &pRe1->PrPresBox[i - 1]);
					 pRe1->PrPresBoxName[0] = '\0';
				      }
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
					BIOreadBool (file, &pRe1->PrAttrValue);
					BIOreadSignedShort (file, &pRe1->PrIntValue);
					break;
				     case PtFont:
				     case PtStyle:
				     case PtUnderline:
				     case PtThickness:
				     case PtLineStyle:
					if (!BIOreadByte (file, &pRe1->PrChrValue))
					   Erreur_Present (1);
					break;
				     case PtBreak1:
				     case PtBreak2:
				     case PtIndent:
				     case PtSize:
				     case PtLineSpacing:
				     case PtLineWeight:
					pRe1->PrMinUnit = rdUnit (file);
					BIOreadBool (file, &pRe1->PrMinAttr);
					BIOreadSignedShort (file, &pRe1->PrMinValue);
					break;
				     case PtVertRef:
				     case PtHorizRef:
				     case PtVertPos:
				     case PtHorizPos:
					ReadPosition (file, &pRe1->PrPosRule);
					break;
				     case PtHeight:
				     case PtWidth:
					pRelD1 = &pRe1->PrDimRule;
					BIOreadBool (file, &pRelD1->DrPosition);
					if (pRelD1->DrPosition)
					   ReadPosition (file, &pRelD1->DrPosRule);
					else
					  {
					     BIOreadBool (file, &pRelD1->DrAbsolute);
					     BIOreadBool (file, &pRelD1->DrSameDimens);
					     pRelD1->DrUnit = rdUnit (file);
					     BIOreadBool (file, &pRelD1->DrAttr);
					     BIOreadBool (file, &pRelD1->DrMin);
					     BIOreadBool (file, &pRelD1->DrUserSpecified);
					     BIOreadSignedShort (file, &pRelD1->DrValue);
					     pRelD1->DrRelation = rdNiveau (file);
					     BIOreadBool (file, &pRelD1->DrNotRelat);
					     BIOreadBool (file, &pRelD1->DrRefElement);
					     if (pRelD1->DrRefElement)
						BIOreadShort (file, &pRelD1->DrTypeRefElem);
					     else
						BIOreadShort (file, &pRelD1->DrRefPresBox);
					  }
					break;
				     case PtAdjust:
					pRe1->PrAdjust = rdCadrage (file);
					break;
				     case PtJustify:
				     case PtHyphenate:
					BIOreadBool (file, &pRe1->PrJustify);
					break;
				     default:
					break;
				  }
			    /* passe a la regle suivante */
			    break;
		      }

	     if (pRe1->PrNextPRule != NULL)
		pRe1->PrNextPRule = *nextr;
	  }
	while (!(r->PrNextPRule == NULL || erreur));
     }
}


/* ReadPresentationSchema    lit un fichier contenant un schema de presentation */
/* et le charge en memoire. */
/* fname: nom du fichier a lire, sans le suffixe .PRS */
/* SS: pointeur sur le schema de structure correspondant, */
/* deja rempli si SsRootElem n'est pas nul, */
/* a charger si SsRootElem est nul. */
/* Retourne un pointeur sur le schema de presentation en memoire si */
/* chargement reussi, NULL si echec. */
#ifdef __STDC__
PtrPSchema          ReadPresentationSchema (Name fname, PtrSSchema SS)
#else  /* __STDC__ */
PtrPSchema          ReadPresentationSchema (fname, SS)
Name                 fname;
PtrSSchema        SS;

#endif /* __STDC__ */
{
   int                 i, j, l;
   PtrPRule        nextr;
   PtrPSchema          pSchP;
   boolean             ret;
   PtrPSchema          pSc1;
   Counter           *pCo1;
   CntrItem            *pCp1;
   PresConstant          *pPr1;
   PresVariable            *pPres1;
   PresVarItem            *pVa1;
   PresentationBox             *pBo1;
   AttributePres      *pRP1;
   NumAttrCase         *pCa1;
   PathBuffer          DirBuffer;
   BinFile             file;
   char                texte[MAX_TXT_LEN];
   int                 NbElemStructInitial;

   erreur = FALSE;
   pSchP = NULL;

   /* compose le nom du fichier a ouvrir avec le nom du directory */
   /* des schemas... */
   strncpy (DirBuffer, SchemaPath, MAX_PATH);
   MakeCompleteName (fname, "PRS", DirBuffer, texte, &i);

   /* teste si le fichier existe */

   file = BIOreadOpen (texte);
   if (file == 0)
     {
	/* message 'Fichier inaccessible' */
	strncpy (texte, fname, MAX_NAME_LENGTH);
	strcat (texte, ".PRS");
	TtaDisplayMessage (INFO, TtaGetMessage(LIB, LIB_MISSING_FILE), texte);
     }
   else
     {
	/* supprime le suffixe .PRS a la fin du nom de fichier */

	texte[i - 4] = '\0';
	GetSchPres (&pSchP);
	GetPresentRule (&nextr);	/* ce sera la prochaine regle lue */
	nextr->PrCond = NULL;
	/* met son nom dans le schema de presentation */
	strncpy (pSchP->PsPresentName, fname, MAX_NAME_LENGTH - 1);
	/* lit la partie fixe du schema de presentation */
	pSc1 = pSchP;
	/* lit le nom du schema de structure correspondant */
	BIOreadName (file, pSc1->PsStructName);
	BIOreadShort (file, &pSc1->PsStructCode);
	erreur = !BIOreadShort (file, &pSc1->PsNViews);
	if (!erreur)
	   for (i = 1; i <= pSc1->PsNViews; i++)
	      BIOreadName (file, pSc1->PsView[i - 1]);
	if (!erreur)
	   for (i = 1; i <= pSc1->PsNViews; i++)
	      BIOreadBool (file, &pSc1->PsPaginatedView[i - 1]);
	/* significatif uniquement dans la V4 */
	if (!erreur)
	   for (i = 1; i <= pSc1->PsNViews; i++)
	      BIOreadBool (file, &pSc1->PsColumnView[i - 1]);
	erreur = !BIOreadShort (file, &pSc1->PsNPrintedViews);
	if (!erreur)
	   for (i = 1; i <= pSc1->PsNPrintedViews; i++)
	     {
		BIOreadBool (file, &pSc1->PsPrintedView[i - 1].VpAssoc);
		erreur = !BIOreadShort (file, &pSc1->PsPrintedView[i - 1].VpNumber);
	     }
	if (!erreur)
	   for (i = 1; i <= pSc1->PsNViews; i++)
	      BIOreadBool (file, &pSc1->PsExportView[i - 1]);
	BIOreadShort (file, &pSc1->PsNCounters);
	BIOreadShort (file, &pSc1->PsNConstants);
	BIOreadShort (file, &pSc1->PsNVariables);
	BIOreadShort (file, &pSc1->PsNPresentBoxes);
	pSc1->PsFirstDefaultPRule = ReadRulePtr (file, &nextr);
	ret = !erreur;
	if (SS->SsRootElem == 0)
	   ret = ReadStructureSchema (pSc1->PsStructName, SS);
	if (!ret || pSchP->PsStructCode != SS->SsCode)
	  {
	     FreeSchPres (pSchP);
	     FreePresentRule (nextr);
	     nextr = NULL;
	     pSchP = NULL;
	     /* Schemas incompatibles */
	     TtaDisplayMessage (INFO, TtaGetMessage(LIB, OLD_SCH_TO_RECOMP), texte);
	  }

	else
	  {
	     pSc1 = pSchP;
	     /* lit les compteurs */
	     if (!erreur)
		for (i = 1; i <= pSc1->PsNCounters; i++)
		  {
		     pCo1 = &pSc1->PsCounter[i - 1];
		     erreur = !BIOreadShort (file, &pCo1->CnNItems);
		     if (!erreur)
			for (j = 1; j <= pCo1->CnNItems; j++)
			  {
			     pCp1 = &pCo1->CnItem[j - 1];
			     pCp1->CiCntrOp = rdCptTypeOp (file);
			     BIOreadShort (file, &pCp1->CiElemType);
			     BIOreadSignedShort (file, &pCp1->CiAscendLevel);
			     BIOreadShort (file, &pCp1->CiViewNum);
			     BIOreadSignedShort (file, &pCp1->CiParamValue);
			     BIOreadShort (file, &pCp1->CiInitAttr);
			     BIOreadShort (file, &pCp1->CiReinitAttr);
			  }
		     erreur = !BIOreadShort (file, &pCo1->CnNPresBoxes);
		     if (!erreur)
			for (j = 1; j <= pCo1->CnNPresBoxes; j++)
			  {
			     BIOreadShort (file, &pCo1->CnPresBox[j - 1]);
			     erreur = !BIOreadBool (file, &pCo1->CnMinMaxPresBox[j - 1]);
			  }
		     erreur = !BIOreadShort (file, &pCo1->CnNTransmAttrs);
		     if (!erreur)
			for (j = 1; j <= pCo1->CnNTransmAttrs; j++)
			  {
			     BIOreadName (file, pCo1->CnTransmAttr[j - 1]);
			     BIOreadShort (file, &pCo1->CnTransmSSchemaAttr[j - 1]);
			  }
		     erreur = !BIOreadShort (file, &pCo1->CnNCreators);
		     if (!erreur)
			for (j = 1; j <= pCo1->CnNCreators; j++)
			  {
			     erreur = !BIOreadShort (file, &pCo1->CnCreator[j - 1]);
			     erreur = !BIOreadBool (file, &pCo1->CnMinMaxCreator[j - 1]);
			  }
		     if (!erreur)
			for (j = 1; j <= pCo1->CnNCreators; j++)
			   BIOreadBool (file, &pCo1->CnPresBoxCreator[j - 1]);
		     erreur = !BIOreadShort (file, &pCo1->CnNCreatedBoxes);
		     if (!erreur)
			for (j = 1; j <= pCo1->CnNCreatedBoxes; j++)
			  {
			     BIOreadShort (file, &pCo1->CnCreatedBox[j - 1]);
			     erreur = !BIOreadBool (file, &pCo1->CnMinMaxCreatedBox[j - 1]);
			  }
		     erreur = !BIOreadBool (file, &pCo1->CnPageFooter);
		  }

	     /* lit les constantes de presentation */
	     if (!erreur)
		for (i = 1; i <= pSc1->PsNConstants; i++)
		  {
		     pPr1 = &pSc1->PsConstant[i - 1];
		     pPr1->PdType = rdTypeDeBase (file);
		     if (!BIOreadByte (file, &pPr1->PdAlphabet))
			Erreur_Present (1);
		     j = 0;
		     if (!erreur)
			do
			   if (!BIOreadByte (file, &pPr1->PdString[j++]))
			      Erreur_Present (1);
			while (!(pPr1->PdString[j - 1] == '\0' || erreur)) ;
		  }

	     /* lit les variables de presentation */
	     if (!erreur)
		for (i = 1; i <= pSc1->PsNVariables; i++)
		  {
		     pPres1 = &pSc1->PsVariable[i - 1];
		     erreur = !BIOreadShort (file, &pPres1->PvNItems);
		     if (!erreur)
			for (j = 1; j <= pPres1->PvNItems; j++)
			  {
			     pVa1 = &pPres1->PvItem[j - 1];
			     pVa1->ViType = rdVarType (file);
			     switch (pVa1->ViType)
				   {
				      case VarText:
					 BIOreadShort (file, &pVa1->ViConstant);
					 break;
				      case VarCounter:
					 BIOreadShort (file, &pVa1->ViCounter);
					 pVa1->ViStyle = rdComptStyle (file);
					 pVa1->ViCounterVal = rdNatValCmpt (file);
					 break;
				      case VarAttrValue:
					 BIOreadShort (file, &pVa1->ViAttr);
					 pVa1->ViStyle = rdComptStyle (file);

					 break;
				      case VarPageNumber:
					 BIOreadShort (file, &pVa1->ViView);
					 pVa1->ViStyle = rdComptStyle (file);
					 break;
				      default:
					 break;
				   }
			  }
		  }

	     /* lit les boites de presentation et de mise en page */
	     if (!erreur)
		for (i = 1; i <= pSc1->PsNPresentBoxes; i++)
		  {
		     pBo1 = &pSc1->PsPresentBox[i - 1];
		     BIOreadName (file, pBo1->PbName);
		     pBo1->PbFirstPRule = ReadRulePtr (file, &nextr);
		     BIOreadBool (file, &pBo1->PbAcceptPageBreak);
		     BIOreadBool (file, &pBo1->PbAcceptLineBreak);
		     BIOreadBool (file, &pBo1->PbBuildAll);
		     BIOreadBool (file, &pBo1->PbNotInLine);
		     BIOreadBool (file, &pBo1->PbPageFooter);
		     BIOreadBool (file, &pBo1->PbPageHeader);
		     BIOreadBool (file, &pBo1->PbPageBox);
		     BIOreadShort (file, &pBo1->PbFooterHeight);
		     BIOreadShort (file, &pBo1->PbHeaderHeight);
		     BIOreadShort (file, &pBo1->PbPageCounter);
		     pBo1->PbContent = ReadContentType (file);
		     if (!erreur)
			switch (pBo1->PbContent)
			      {
				 case ContVariable:
				    BIOreadShort (file, &pBo1->PbContVariable);
				    break;
				 case ContConst:
				    BIOreadShort (file, &pBo1->PbContConstant);
				    break;
				 case ContElement:
				    BIOreadShort (file, &pBo1->PbContElem);
				    BIOreadShort (file, &pBo1->PbContRefElem);
				    break;
				 default:
				    break;
			      }
		  }

	     /* lit les presentations des attributs semantiques */
	     if (!erreur)
	       {
		  for (i = 1; i <= SS->SsNAttributes; i++)
		     if (!erreur)
		       {
			  /* lecture du nombre de paquet de regles differentes */
			  BIOreadShort (file, &pSc1->PsNAttrPRule[i - 1]);
			  if (pSc1->PsNAttrPRule[i - 1] > 0)
			    {
			       /* allocation des regles */
			       pSc1->PsAttrPRule[i - 1] =
				  (AttributePres *) TtaGetMemory (pSc1->PsNAttrPRule[i - 1] * sizeof (AttributePres));
			       if ((pRP1 = pSc1->PsAttrPRule[i - 1]) != NULL)
				 {
				    /* chainage des regles */
				    for (l = pSc1->PsNAttrPRule[i - 1]; --l > 0; pRP1 = pRP1->ApNextAttrPres)
				       pRP1->ApNextAttrPres = pRP1 + 1;
				    /* la derniere pointe sur NULL */
				    pRP1->ApNextAttrPres = NULL;
				 }
			       else	/* l'allocation a echouee */
				  erreur = TRUE;
			    }
			  else
			     pSc1->PsAttrPRule[i - 1] = NULL;
		       }

		  for (i = 1; i <= SS->SsNAttributes; i++)
		     if (!erreur)
		       {
			  pRP1 = pSc1->PsAttrPRule[i - 1];
			  for (l = pSc1->PsNAttrPRule[i - 1]; l-- > 0; pRP1 = pRP1->ApNextAttrPres)
			    {
			       if (!erreur)
				 {
				    erreur = !BIOreadShort (file, &pRP1->ApElemType);
				    switch (SS->SsAttribute[i - 1].AttrType)
					  {
					     case AtNumAttr:
						for (j = 0; j < MAX_PRES_ATTR_CASE; j++)
						   pRP1->ApCase[j].CaFirstPRule = NULL;
						erreur = !BIOreadShort (file, &pRP1->ApNCases);
						if (!erreur)
						   for (j = 1; j <= pRP1->ApNCases; j++)
						     {
							pCa1 = &pRP1->ApCase[j - 1];
							pCa1->CaComparType = ReadAttrCompar (file);
							BIOreadSignedShort (file, &pCa1->CaLowerBound);
							BIOreadSignedShort (file, &pCa1->CaUpperBound);
							pCa1->CaFirstPRule = ReadRulePtr (file, &nextr);
						     }
						break;
					     case AtReferenceAttr:
						pRP1->ApRefFirstPRule = ReadRulePtr (file, &nextr);
						break;
					     case AtTextAttr:
						BIOreadName (file, pRP1->ApString);
						pRP1->ApTextFirstPRule = ReadRulePtr (file, &nextr);
						break;
					     case AtEnumAttr:
						for (j = 0; j <= MAX_ATTR_VAL; j++)
						   pRP1->ApEnumFirstPRule[j] = NULL;
						for (j = 0; j <= SS->SsAttribute[i - 1].AttrNEnumValues; j++)
						   pRP1->ApEnumFirstPRule[j] = ReadRulePtr (file, &nextr);
						break;
					  }
				 }
			    }
		       }
	       }

	     if (SS->SsFirstDynNature == 0)
		NbElemStructInitial = SS->SsNRules;
	     else
		NbElemStructInitial = SS->SsFirstDynNature - 1;

	     /* lit la table des pointeurs de regle de chaque type du */
	     /* schema de structure */
	     if (!erreur)
		for (i = 1; i <= NbElemStructInitial; i++)
		   pSc1->PsElemPRule[i - 1] = ReadRulePtr (file, &nextr);

	     /* lit toutes les regles de presentation */
	     /* lit les regles standard */
	     if (!erreur)
		ReadRules (file, &pSc1->PsFirstDefaultPRule, &nextr);

	     /* les regles des boites */
	     if (!erreur)
		for (i = 1; i <= pSc1->PsNPresentBoxes; i++)
		   ReadRules (file, &pSc1->PsPresentBox[i - 1].PbFirstPRule, &nextr);

	     /* lit les regles des attributs */
	     if (!erreur)
		for (i = 1; i <= SS->SsNAttributes; i++)
		   if (!erreur)
		     {
			pRP1 = pSc1->PsAttrPRule[i - 1];
			for (l = pSc1->PsNAttrPRule[i - 1]; l-- > 0; pRP1 = pRP1->ApNextAttrPres)
			  {
			     if (!erreur)
				switch (SS->SsAttribute[i - 1].AttrType)
				      {
					 case AtNumAttr:
					    for (j = 1; j <= pRP1->ApNCases; j++)
					       ReadRules (file, &pRP1->ApCase[j - 1].CaFirstPRule, &nextr);
					    break;
					 case AtReferenceAttr:
					    ReadRules (file, &pRP1->ApRefFirstPRule, &nextr);
					    break;
					 case AtTextAttr:
					    ReadRules (file, &pRP1->ApTextFirstPRule, &nextr);
					    break;
					 case AtEnumAttr:
					    for (j = 0; j <= SS->SsAttribute[i - 1].AttrNEnumValues; j++)
					       ReadRules (file, &pRP1->ApEnumFirstPRule[j], &nextr);
					    /* lit les regles des elements structures */
					    break;
				      }
			  }
		     }

	     if (!erreur)
		for (i = 0; i < NbElemStructInitial; i++)
		   ReadRules (file, &pSc1->PsElemPRule[i], &nextr);

	     if (!erreur)
		for (i = 0; i < SS->SsNAttributes; i++)
		   BIOreadShort (file, &pSc1->PsNHeirElems[i]);

	     if (!erreur)
		for (i = 0; i < NbElemStructInitial; i++)
		  {
		     BIOreadShort (file, &pSc1->PsNInheritedAttrs[i]);
		     pSc1->PsInheritedAttr[i] = NULL;
		  }

	     if (!erreur)
		for (i = 0; i < SS->SsNAttributes; i++)
		  {
		     BIOreadShort (file, &pSc1->PsNComparAttrs[i]);
		     pSc1->PsComparAttr[i] = NULL;
		  }

	     if (!erreur)
		for (i = 0; i < NbElemStructInitial; i++)
		   BIOreadBool (file, &pSc1->PsAcceptPageBreak[i]);

	     if (!erreur)
		for (i = 0; i < NbElemStructInitial; i++)
		   BIOreadBool (file, &pSc1->PsAcceptLineBreak[i]);

	     if (!erreur)
		for (i = 0; i < NbElemStructInitial; i++)
		   BIOreadBool (file, &pSc1->PsBuildAll[i]);

	     if (!erreur)
		for (i = 0; i < NbElemStructInitial; i++)
		   BIOreadBool (file, &pSc1->PsNotInLine[i]);

	     if (!erreur)
		for (i = 0; i < NbElemStructInitial; i++)
		   BIOreadBool (file, &pSc1->PsInPageHeaderOrFooter[i]);

	     if (!erreur)
		for (i = 0; i < NbElemStructInitial; i++)
		   BIOreadBool (file, &pSc1->PsAssocPaginated[i]);
	     if (!erreur)
		for (i = 0; i < NbElemStructInitial; i++)
		   BIOreadShort (file, &pSc1->PsElemTransmit[i]);
	     if (!erreur)
		BIOreadShort (file, &pSc1->PsNTransmElems);
	     if (!erreur)
		for (i = 0; i < pSc1->PsNTransmElems; i++)
		  {
		     BIOreadShort (file, &pSc1->PsTransmElem[i].TeTargetDoc);
		     BIOreadName (file, pSc1->PsTransmElem[i].TeTargetAttr);
		  }
	     FreePresentRule (nextr);
	  }

	/* ferme le fichier */
	BIOreadClose (file);
     }

   if (erreur)
     {
	/* message 'Schema incorrect' */
	TtaDisplayMessage (INFO, TtaGetMessage(LIB, INCORRECT_PRS_FILE), fname);
	return NULL;
     }

   else
      return pSchP;
}
/* End Of Module rdschprs */
