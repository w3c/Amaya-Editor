/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 
#ifndef _THOTLIB_TYPEPRS_H_
#define _THOTLIB_TYPEPRS_H_

/*
 * Type declaration for the presentation of documents
 *
 * Author V. Quint (INRIA)
 *
 */


/* DEFINITIONS

	A view number is the rank of a view in the table PresentSchema.PsView.
	A counter number is the rank of a counter in the table PresentSchema.
	PsCounter.
	A constant number is the rank of a constant in the table PresentSchema.
	PsConstant.
	A variable number is the rank of a variable in the table PresentSchema.
	PsVariable.
	A presentation box number is the rank of a box in the table
	PresentSchema.PsPresentBox.
*/

#include "typebase.h"

/* presentation mode of the pictures */
#define UNKNOWN_FORMAT   -1
typedef enum
{
        RealSize,
        ReScale,
        FillFrame,
	XRepeat,
	YRepeat
} PictureScaling;

/* dimensions and axes of the boxes */
typedef enum
{
    Top, Bottom, Left, Right, HorizRef, VertRef,
    HorizMiddle, VertMiddle, NoEdge
} BoxEdge;

typedef struct _PresRule *PtrPRule;

/* relative level */
typedef enum
{
  RlEnclosing, RlSameLevel, RlEnclosed, RlPrevious, RlNext, RlSelf, 
  RlContainsRef, RlRoot, RlReferred, RlCreator
} Level;

/* Kind of object used as a reference in a relative position */
typedef enum
{
  RkElType, RkPresBox, RkAttr
} RefKind;

/* relative positionning rule for the dimensions or axes of two boxes */
typedef struct _PosRule
{
  BoxEdge	PoPosDef;	/* side of the abstract box of which the
				   position is defined */
  BoxEdge	PoPosRef;	/* side of the abstract box from which the
				   position is defined */
  TypeUnit	PoDistUnit;	/* PoDistance is expressed in picas, 1/10 of
				   a character, etc. */
  ThotBool	PoDistAttr;	/* PoDistance is a numerical attribute number
				   or a numerical value */
  int		PoDistance;	/* distance between those two sides */
  Level		PoRelation;	/* relative level of the reference element
				   in the internal representation */
  ThotBool	PoNotRel;	/* if 'false', PoTypeRefElem or PoRefPresBox
				   indicates the type of the reference element,
				   if 'true', the exclusive type. */
  ThotBool	PoUserSpecified;/* the distance may be chosen by the user
				   during the creation */
  RefKind	PoRefKind;	/* the reference is the box of an element,
				   a presentation box or the box of an element
				   with an attribute */
  int		PoRefIdent;	/* depending on PoRefKind, element type number,
				   pres box number or attribute number */
} PosRule;

/* Box dimensionning rule */
typedef struct _DimensionRule
{
  ThotBool	 DrPosition;	/* the dimension is defined as a position
				   (rubber band box) */
  union
  {
    struct			/* DrPosition = True */
    {
      PosRule	_DrPosRule_;    /* the position rule that defines the
				   dimension */
    }   s0;
    struct			/* DrPosition = False */
    {
      ThotBool	_DrAbsolute_;   /* absolute value, relative otherwise */ 
      ThotBool	_DrSameDimens_; /* the dimension is defined in regard to
				   the same dimension of another box */
      TypeUnit	_DrUnit_;       /* DrValue is expressed in picas, 1/10 of a
				   character, etc. */ 
      ThotBool	_DrAttr_;       /* DrValue is a numerical attribute or a
				   numerical value attribute  number */ 
      ThotBool	_DrMin_; 	/* minimum dimension */
      ThotBool	_DrUserSpecified_; /* the distance may be chosen by the user */
      int	_DrValue_;      /* value of the percentage, the increment or
				   the absolute value */
      Level	_DrRelation_;  	/* relative level of the reference element */
      ThotBool	_DrNotRelat_;   /* if false, DrTypeRefElem or DrRefPresBox
				  indicates the type of the reference elt.
				  if false, the exclusive type. */
      RefKind	_DrRefKind_;	/* the reference is the box of an element,
				   a presentation box or the box of an element
				   with an attribute */
      int	_DrRefIdent_;	/* depending on DrRefKind, element type number,
				   pres box number or attribute number */
    } s1;
  } u;
} DimensionRule;

#define DrPosRule u.s0._DrPosRule_
#define DrAbsolute u.s1._DrAbsolute_
#define DrSameDimens u.s1._DrSameDimens_
#define DrUnit u.s1._DrUnit_
#define DrAttr u.s1._DrAttr_
#define DrMin u.s1._DrMin_
#define DrUserSpecified u.s1._DrUserSpecified_
#define DrValue u.s1._DrValue_
#define DrRelation u.s1._DrRelation_
#define DrNotRelat u.s1._DrNotRelat_
#define DrRefKind u.s1._DrRefKind_
#define DrRefIdent u.s1._DrRefIdent_

/* type of the content of a presentation box */
typedef enum
{
    FreeContent, ContVariable, ContConst, ContElement
}	ContentType;

/* a presentation box */
typedef struct _PresentationBox
{
  Name      PbName;		/* box name */	
  PtrPRule	PbFirstPRule;	/* first presentation rule defining the box */
  ThotBool	PbAcceptPageBreak;	/* indicates whether the box may be
					   broken at the bottom of a page */
  ThotBool	PbAcceptLineBreak;	/* indicates whether the box may be
					   broken at the end of a line */
  ThotBool	PbBuildAll;	/* indicates if the box image must be built
				   in one piece or if it can be partial */
  ThotBool	PbPageFooter;	/* it is a footer box */
  ThotBool	PbPageHeader;	/* it is a header box */
  ThotBool	PbPageBox;	/* it is a page box */
  int	 	PbFooterHeight;	/* if it is a page box, size of the footer
				   in picas */
  int		PbHeaderHeight;	/* if it is a page box, size of the header
				   in picas */
  int		PbPageCounter;	/* number of the page counter, only if
				   PbPageBox is true */
  ContentType	PbContent;	/* compulsory box content */
  union
  {
    struct			/* PbContent = ContVariable */
    {
      int	_PbContVariable_;	/* number of the variable */
    }   s0;
    struct			/* PbContent = ContConst */
    {
      int	_PbContConstant_;	/* number of the constant */
    } s1;
    struct			/* PbContent = ContElement */
    {
      int	_PbContElem_;	/* number of the element type */
      int	_PbContRefElem_;	/* number of the type of reference
					   to this element */
    } s2;
    struct			/* PbContent = FreeContent */
    {
      CHAR_T	_PbContFree_; /* to make the compiler happy */
    } s3;
  } u;
} PresentationBox;

#define PbContVariable u.s0._PbContVariable_
#define PbContConstant u.s1._PbContConstant_
#define PbContElem u.s2._PbContElem_
#define PbContRefElem u.s2._PbContRefElem_
#define PbContFree u.s3._PbContFree_

/* Alignment of the lines in an abstract box */
typedef enum
{
  AlignLeft, AlignRight, AlignCenter, AlignLeftDots
} BAlignment;	/* AlignLeftDots = aligned to the left, the last
		   line is filled with dots */

/* type of a presentation rule */
typedef enum
{
  /* the order determines the order of the rules in the presentation schema */
  /* Don't forget to change constant MAX_TRANSL_PRULE when adding rules */
  PtVisibility, PtFunction, PtVertRef, PtHorizRef, PtHeight, PtWidth, 
  PtVertPos, PtHorizPos,
  PtMarginTop, PtMarginRight, PtMarginBottom, PtMarginLeft,
  PtPaddingTop, PtPaddingRight, PtPaddingBottom, PtPaddingLeft,
  PtBorderTopWidth, PtBorderRightWidth, PtBorderBottomWidth, PtBorderLeftWidth,
  PtBorderTopColor, PtBorderRightColor, PtBorderBottomColor, PtBorderLeftColor,
  PtBorderTopStyle, PtBorderRightStyle, PtBorderBottomStyle, PtBorderLeftStyle,
  PtSize, PtStyle, PtWeight, PtFont, PtUnderline, PtThickness,
  PtIndent, PtLineSpacing, PtDepth, PtAdjust, PtJustify,
  PtLineStyle, PtLineWeight, PtFillPattern, PtBackground, PtForeground,
  PtHyphenate, PtVertOverflow, PtHorizOverflow,
  /* the three following types must be the last ones */
  PtBreak1, PtBreak2, PtPictInfo
} PRuleType;

/* computing mode of the properties */
typedef enum
{
  PresImmediate, PresInherit, PresFunction
} PresMode;

/* inherit mode */
typedef enum
{
  InheritParent, InheritPrevious, InheritChild, InheritCreator, 
  InheritGrandFather
} InheritMode;

/* functions */
/* the order determines the order of the rules in the presentation schema */
typedef enum
{
  FnLine, FnPage, FnCreateBefore, FnCreateWith, FnCreateFirst, FnCreateLast,
  FnCreateAfter, FnColumn, FnCopy, FnContentRef, FnSubColumn, FnNoLine,
  FnCreateEnclosing, FnShowBox, FnBackgroundPicture, FnPictureMode,
  FnNotInLine, FnAny
} FunctionType;

/* counter styles */
typedef enum
{
  CntArabic, CntURoman, CntLRoman, CntUppercase, CntLowercase
} CounterStyle;

/* types of a presentation variable */
typedef enum
{
  VarText, VarCounter, VarDate, VarFDate, VarDirName, VarDocName, VarElemName,
  VarAttrName, VarAttrValue, VarPageNumber
} VariableType;

/* to indicate the nature of the counter value */
typedef enum
{
  CntMaxVal, CntMinVal, CntCurVal
} CounterValue;

/* a variable element */
typedef struct _PresVarItem
{
  VariableType     ViType;
  CounterStyle  ViStyle;	/* digit style for VarCounter, VarAttrValue and
				   VarPageNumber */
  union
  {
    struct
    {
      int	     _ViConstant_;	/* number of the constant */ 
    } s0;
    struct
    {
      int	     _ViCounter_;   	/* number of the counter */
      CounterValue _ViCounterVal_; 	/* indicates if we are interested in
					   the maximum, minimum or current
					   value of the counter */
    } s1;
    struct
    {
      int  _ViAttr_;  	/* the attribute number */
    } s2;
    struct
    {
      int            _ViDate_;      /* for the compiler */
    } s3;
    struct			    /* ViType = VarPageNumber */
    {
      int	     _ViView_;	/* number of the view in which the pages are
				   counted */
    } s4;
  } u;
} PresVarItem;

#define ViConstant u.s0._ViConstant_
#define ViCounter u.s1._ViCounter_
#define ViCounterVal u.s1._ViCounterVal_
#define ViAttr u.s2._ViAttr_
#define ViDate u.s3._ViDate_
#define ViView u.s4._ViView_

/* A box creation condition type */
typedef enum
{
  PcFirst, PcLast, PcReferred, PcFirstRef, PcLastRef, PcExternalRef,
  PcInternalRef, PcCopyRef, PcAnyAttributes, PcFirstAttr, PcLastAttr,
  PcUserPage, PcStartPage, PcComputedPage, PcEmpty, PcEven, PcOdd, PcOne,
  PcInterval, PcWithin, PcElemType, PcAttribute, PcNoCondition, PcDefaultCond
} PresCondition;

/* To interpret the field CoRelation */
typedef enum
{
  CondGreater, CondLess, CondEquals
} ArithRel;

typedef struct _Condition *PtrCondition;

/* A presentation rule application condition */
typedef struct _Condition
{
  PtrCondition    CoNextCondition;	/* Next condition in the list
					   IF cond AND cond AND cond ... */
  PresCondition   CoCondition;	        /* type of the condition */	
  ThotBool        CoNotNegative;	/* the condition is not negative */
  ThotBool	  CoTarget;		/* the condition affects the target
					   (for references only) */
  union
  {
    struct				/* CoCondition = PcInterval, PcEven,
					   PcOdd, PcOne */
    {
      int	  _CoCounter_; 	  /* number of the counter on which the
				     condition applies */
      int	  _CoMinCounter_; /* minimum value of the counter so that
				     the presentation rule may be applied */
      int	  _CoMaxCounter_; /* maximum value of the counter so that
				     the presentation rule may be applied */
      CounterValue _CoValCounter_; 	/* indicates if the minimum, maximum
					   or current value of the counter is
					   used */
    } s0;
    struct				/* CoCondition = PcWithin */
    {
      int       _CoRelation_;		/* RelLevel */
      int       _CoTypeAncestor_;	/* type of the ancestor */	
      ThotBool  _CoImmediate_;	/* Immediately */
      ArithRel  _CoAncestorRel_;
      Name      _CoAncestorName_;	/* Ancestor type name, if defined
					   in another schema */
      Name      _CoSSchemaName_;	/* name of the schema where the
					   ancestor is defined if
					   CoTypeAncestor = 0 */
    } s1;
    struct				/* CoCondition = PcElemType or
					   PcAttribute */
    {
      int	  _CoTypeElAttr_;	/* PcElemType: type of the element
					   to which the attribute is attached.
					   PcAttribute: attribute carried by
					   the element */
    } s2;
  } u;
} Condition;

#define CoCounter u.s0._CoCounter_
#define CoMinCounter u.s0._CoMinCounter_
#define CoMaxCounter u.s0._CoMaxCounter_
#define CoValCounter u.s0._CoValCounter_
#define CoRelation u.s1._CoRelation_
#define CoTypeAncestor u.s1._CoTypeAncestor_
#define CoImmediate u.s1._CoImmediate_
#define CoAncestorRel u.s1._CoAncestorRel_
#define CoAncestorName u.s1._CoAncestorName_
#define CoSSchemaName u.s1._CoSSchemaName_
#define CoTypeElAttr u.s2._CoTypeElAttr_

/* The presentation rules relative to an object are linked by means of
   the pointer PrNextPRule. This way the presentation rules of a type
   defined in the structure schema are linked, as well as the presentation
   rules of a logical attribute value, of a presentation box or of the
   default rules */ 

/* a presentation rule */
typedef struct _PresRule
{
  PRuleType     PrType;		/* rule type */	
  PtrPRule	PrNextPRule;	/* next rule for the same object */
  PtrCondition  PrCond;		/* application conditions for the rule */
  int		PrViewNum;	/* number of the view to which the rule
				   applies */	
  int		PrSpecifAttr;	/* only for specifical presentation rules
				   attached to the abstract tree elements:
				   number of the attribute to which the
				   rule corresponds, 0 if the rule is not
				   derived from an attribute rule */
  PtrSSchema    PrSpecifAttrSSchema; /* pointer on the structure schema
                                        defining the attribute PrSpecifAttr */
  PresMode	PrPresMode;	/* computing mode of the value */	
  union
  {
    struct			/* PrPresMode = PresInherit */
    {
      InheritMode  _PrInheritMode_;
      ThotBool	   _PrInhPercent_;/* PrInhDelta is a precentage if true, an
				   increment/decrement if false */
      ThotBool     _PrInhAttr_;	/* PrInhDelta is a numerical attribute
				   number if true, a value if false */  
      int	   _PrInhDelta_;/* positive: increment, zero: equality,
				   negative: decrement */  
      ThotBool	   _PrMinMaxAttr_; /* PrInhMinOrMax is a numerical attribute
				   number or a value */
      int          _PrInhMinOrMax_;/* min or max value of the inheritance */
      TypeUnit     _PrInhUnit_;	/* PrInhDelta and PrInhMinOrMax are
				   expressed in picas, pixels, relative value,
				   etc. */
    } s0;
    struct			  /* PrPresMode = PresFunction */
    {
      FunctionType _PrPresFunction_;
      ThotBool     _PrPresBoxRepeat_;	/* presentation box repeated over all
					   the abstract boxes of the element */
      ThotBool	   _PrExternal_; /* if PrElement is true, PrExternal indicates
				    that the type of which the name is in
				    PrPresBoxName is external */
      ThotBool     _PrElement_;	 /* PrPresBox[1] or PrPresBoxName is an
				    element type number, not a presentation
				    box number */
      int	   _PrNPresBoxes_;	/* number of presentation boxes (of use
					   for the column rule only) */
      int          _PrPresBox_[MAX_COLUMN_PAGE]; /* number of the
					            presentation boxes */
      Name        _PrPresBoxName_;	/* Name of the first (or only) presentation
					   box to which the function applies */
    } s1;
    struct			 /* PrPresMode = PresImmediate */
    {
      union
      {
	struct	/* PRuleType = PtVisibility, PtDepth, PtFillPattern,
	           PtBackground, PtForeground, PtBorderTopColor,
                   PtBorderRightColor, BorderBottomColor, BorderLeftColor */
	{
	  ThotBool _PrAttrValue_; 	/* PrIntValue is a numerical attribute
					   or numerical value number */ 
	  int  _PrIntValue_;   /* Border colors: -2 means Transparent and
				        -1 means "same color as foreground" */
	}  s0;
	struct	/* PRuleType = PtFont, PtStyle, PtWeight, PtUnderline,
	        	       PtThickness, PtLineStyle,
	                       PtBorderTopStyle, PtBorderRightStyle,
                               PtBorderBottomStyle, PtBorderLeftStyle */
	{
	  char     _PrChrValue_;
	}  s1;
	struct	/* PRuleType = PtBreak1, PtBreak2,
	        PtIndent, PtSize, PtLineSpacing, PtLineWeight,
		PtMarginTop, PtMarginRight, PtMarginBottom, PtMarginLeft
	        PtPaddingTop, PtPaddingRight, PtPaddingBottom, PtPaddingLeft,
                PtBorderTopWidth, PtBorderRightWidth, PtBorderBottomWidth,
                PtBorderLeftWidth */
	{
	  TypeUnit _PrMinUnit_;	/* the distance is expressed in picas,
				   1/10 of a character, etc. */
	  ThotBool _PrMinAttr_;	/* the following field is an attribute number
				   or a value */
	  int  _PrMinValue_;    /* value of the distance */
	} s2;
	struct	/* PRuleType = PtVertRef, PtHorizRef, PtVertPos, PtHorizPos*/
	{
	  PosRule _PrPosRule_;
	} s3;
	struct	/* PRuleType = PtHeight, PtWidth */
	{
	  DimensionRule _PrDimRule_;
	} s4;
	struct	/* PRuleType = PtJustify, PtHyphenate, PtVertOverflow,
		   PtHorizOverflow */
	{
	  ThotBool _PrJustify_;
	} s5;
	struct	/* PRuleType = PtAdjust */
	{
	  BAlignment _PrAdjust_;
	} s6;
      } u;
    } s2;
  } u;
} PresRule;

#define PrInheritMode u.s0._PrInheritMode_
#define PrInhPercent u.s0._PrInhPercent_
#define PrInhAttr u.s0._PrInhAttr_
#define PrInhDelta u.s0._PrInhDelta_
#define PrMinMaxAttr u.s0._PrMinMaxAttr_
#define PrInhMinOrMax u.s0._PrInhMinOrMax_
#define PrInhUnit u.s0._PrInhUnit_
#define PrPresFunction u.s1._PrPresFunction_
#define PrPresBoxRepeat u.s1._PrPresBoxRepeat_
#define PrExternal u.s1._PrExternal_
#define PrElement u.s1._PrElement_
#define PrNPresBoxes u.s1._PrNPresBoxes_
#define PrPresBox u.s1._PrPresBox_
#define PrPresBoxName u.s1._PrPresBoxName_
#define PrAttrValue u.s2.u.s0._PrAttrValue_
#define PrIntValue u.s2.u.s0._PrIntValue_
#define PrChrValue u.s2.u.s1._PrChrValue_
#define PrMinUnit u.s2.u.s2._PrMinUnit_
#define PrMinAttr u.s2.u.s2._PrMinAttr_
#define PrMinValue u.s2.u.s2._PrMinValue_
#define PrPosRule u.s2.u.s3._PrPosRule_
#define PrDimRule u.s2.u.s4._PrDimRule_
#define PrJustify u.s2.u.s5._PrJustify_
#define PrAdjust u.s2.u.s6._PrAdjust_

/* operation on a counter */
typedef enum
{
  CntrSet, CntrAdd, CntrRank, CntrRLevel
} CounterOp;

/* an elementary operation on a counter */
typedef struct _CntrItem
{
  CounterOp CiCntrOp;	   /* the operation */
  int       CiElemType;	   /* number of the element type (if CECatElem =
			      ElementStruct) triggering the operation */
  int	    CiAscendLevel; /* level of the parent we wish to count for
			      recursive structures */
  int       CiViewNum;	   /* number of the view to which the pages to be
			      counted refer (if CiElemType is a page) */
  int       CiParamValue;  /* value of the operation parameter */
  int	    CiInitAttr;	   /* Attribute of an ascendent that gives the counter
			      its initial value, 0 if the initial value does
			      not depend on any attribute */
  int	    CiReinitAttr;  /* Attribute that gives its value to the counter
			      for the element that carries it and the
			      following elements, 0 if no re-initialization */
  int	    CiCondAttr;	   /* Attribute that qualifies the element to be
			      counted or not, 0 if there is no condition on
			      attributes */
  ThotBool  CiCondAttrPresent; /* if CiCondAttr > 0, indicates whether elements
			      with that attribute are counted of not */
} CntrItem;

/* a counter */
typedef struct _Counter
{
  int		 CnNItems;	/* number of operations on this counter */
  CntrItem        CnItem[MAX_PRES_COUNT_ITEM];	/* the operations */
  int		 CnNPresBoxes;	/* Number of elements in CnPresBox */
  int   CnPresBox[MAX_PRES_COUNT_USER];  /* list of the type numbers of the
					    presentation boxes using the
					    counter in their content */
  ThotBool	 CnMinMaxPresBox[MAX_PRES_COUNT_USER];
  int		 CnNTransmAttrs; /* number of external attributes to which
				    the counter value is transmitted */ 
  Name       CnTransmAttr[MAX_TRANSM_ATTR]; /* names of the attributes
					   to which the counter value is transmitted */
  int		 CnTransmSSchemaAttr[MAX_TRANSM_ATTR]; 	/* type number of
					   the external documents where the
					   CnTransmAttr are defined */
  int		 CnNCreators;	/* Number of elements in CnCreator */
  int   CnCreator[MAX_PRES_COUNT_USER];	/* list of type numbers of the box
				   that create other boxes depending on the
				   counter value */
  ThotBool	 CnMinMaxCreator[MAX_PRES_COUNT_USER];
  ThotBool       CnPresBoxCreator[MAX_PRES_COUNT_USER];	/* indicates whether the
				   box corresponding of CnCreator is a presentation box */
  int		 CnNCreatedBoxes; /* Number of elements in CnCreatedBox */
  int   CnCreatedBox[MAX_PRES_COUNT_USER];	/* list of type numbers of
				   the presentation boxes created in regards to the
				   counter value */
  ThotBool	 CnMinMaxCreatedBox[MAX_PRES_COUNT_USER];
  ThotBool       CnPageFooter;	/* this counter is used in a footer */
} Counter;

/* a presentation constant */
typedef struct _PresConstant
{
  BasicType PdType;                       /* type of the constant                                        */
  char      PdAlphabet;                   /* alphabet of the constant                                    */
  CHAR_T    PdString[MAX_PRES_CONST_LEN]; /* constant presentation string, terminated by a NUL character */
} PresConstant;

/* a presentation variable is the concatenation of the results of various
   elements */
typedef struct _PresVariable
{
    int		 PvNItems;	/* effective number of elements */
    PresVarItem      PvItem[MAX_PRES_VAR_ITEM]; /* list of the elements */  
} PresVariable;

typedef enum 
{
  ComparConstant, ComparAttr
} AttrComparType;


/* a case of presentation rules application for a numerical value
   attribute */
typedef struct _NumAttrCase
{
  AttrComparType  CaComparType;	/* type of the comparison elements */
  int	 	  CaLowerBound; 	/* minimum value of the attribute such that
					   the presentation rules are applied */
  int	 	  CaUpperBound;	/* maximum value of the attribute such that
				   the presentation rules are applied */
  PtrPRule    CaFirstPRule;	/* first rule of the chain of rules to apply
				   when the attribute value is in the interval */
} NumAttrCase;

/* presentation of a logical attribute, according to its type */
typedef struct _AttributePres
{
  int     ApElemType;	/* type of element to which the presentation rules
			   apply, 0 if the rules apply whatever the element
			   type is */
  struct _AttributePres *ApNextAttrPres; /* the packet of presentation rules
			   for the next element */
  union
  {
    struct
    {
      int	  _ApNCases_; /* number of application cases for the presentation
			   rules */
      NumAttrCase  _ApCase_[MAX_PRES_ATTR_CASE]; /* the cases of application
			   of the presentation rules */
    } s0;
    struct
    {
      PtrPRule _ApRefFirstPRule_; /* first rule in the string of rules
			   to apply for the attribute */
    } s1;
    struct
    {
      Name  	   _ApString_;	/* the value triggering the application of the
			   presentation rules */
      PtrPRule _ApTextFirstPRule_;    /* first rule in the string of rules
			   to apply for this value */
    } s2;
    struct
    {
      PtrPRule _ApEnumFirstPRule_[MAX_ATTR_VAL + 1]; /* for each value of the
			   attribute, in the order of the table AttrEnumValue,
			   address of the first presentation rule associated
			   with this value */
    } s3;
  } u;
} AttributePres;

#define ApNCases u.s0._ApNCases_
#define ApCase u.s0._ApCase_
#define ApRefFirstPRule u.s1._ApRefFirstPRule_
#define ApString u.s2._ApString_
#define ApTextFirstPRule u.s2._ApTextFirstPRule_
#define ApEnumFirstPRule u.s3._ApEnumFirstPRule_

/* view names table */
typedef Name       ViewTable[MAX_VIEW]; 

/* description of a view to print */
typedef struct _PrintedView
{
    ThotBool    VpAssoc;	/* it is a view of associated elements */
    int	 	VpNumber;    	/* number of the view, or of the type of the
				   associated elements list if VpAssoc. */
} PrintedView;

typedef PtrPRule PtrPRuleTable[MAX_RULES_SSCHEMA];  /* Table of the addresses
				  of the presentation rules of the Types */

/* a rule of transmission of a value from an element to an included
   document attribute */
typedef struct _TransmitElem
{
    int		TeTargetDoc;	/* index of the structure rule defining
				   the included document type */
    Name 		TeTargetAttr;  /* name of the included document
				   attribute to which the element value
				   is transmitted */
} TransmitElem;

typedef ThotBool InheritAttrTable[MAX_ATTR_SSCHEMA];	/* this table is
                                          attached to an element and
					  indicates what are the attributes
					  that this element inherits */
typedef ThotBool ComparAttrTable[MAX_ATTR_SSCHEMA]; /* this table is
					  attached to an element and
					  indicates what are the attributes
					  comparing themselves to it for
					  presentation */

/* a presentation schema loaded in memory */
typedef struct _PresentSchema
{
  PtrPSchema    PsNext;    		/* for free blocks linking */
  Name          PsStructName;    	/* name of the structure schema */
  Name		PsPresentName;		/* name of this presentation schema */
  int	 	PsStructCode;    	/* code identifying the version of this
					   structure schema */
  int		PsNViews;	    	/* number of views */
  ViewTable       PsView;    		/* definition of the views */
  ThotBool      PsPaginatedView[MAX_VIEW]; /* indicates the paginated views */
  ThotBool      PsColumnView[MAX_VIEW];	/* indicates the views separated in
					   columns */
  int		PsNPrintedViews;		/* number of views to print */
  PrintedView    PsPrintedView[MAX_PRINT_VIEW];/* the views to print */
  ThotBool      PsExportView[MAX_VIEW];    /* indicates the views that display
					      only the exported elements */
  int		PsNCounters;		/* number of counters */
  int		PsNConstants;		/* number of presentation constants */
  int		PsNVariables;		/* number of presentation variables */
  int		PsNPresentBoxes;	/* number of presentation and layout
					   boxes */
  PtrPRule  PsFirstDefaultPRule;	/* beginning of the default rules
					   string */
  Counter      PsCounter[MAX_PRES_COUNTER]; /* counters */
  PresConstant     PsConstant[MAX_PRES_CONST];	/* presentation constants */
  PresVariable       PsVariable[MAX_PRES_VARIABLE]; /* presentation variables*/
  PresentationBox        PsPresentBox[MAX_PRES_BOX]; /* descriptions of the
							presentation and layout
							boxes */
        /* For the columns layout, box number 0 contains the Column group box
	   box number 1 contains the left-hand column, etc. */
  AttributePres *PsAttrPRule[MAX_ATTR_SSCHEMA];/* pointers on the presentation
					   rules of the logical attributes,
					   in the same order as in the table
					   StructSchema.SsAttribute */
  int		PsNAttrPRule[MAX_ATTR_SSCHEMA]; /* number of presentation rules
					   packets for each logical attribute,
					   i.e. size of the strings of
					   AttributePres in the table PsAttrPRule
					   in the same order as in the table
					   StructSchema.SsAttribute */
  PtrPRuleTable	PsElemPRule;    	/* pointers on the beginning of the
					   string of presentation rules relating
					   to each type of element, in the same
					   order as in tha table
					   StructSchema.SsRule */
  int    	PsNHeirElems[MAX_ATTR_SSCHEMA]; /* indicates for each attribute,
					  in the same order as in the table
					  StructSchema.AsAttribute, the number
					  of elements that can inherit from the
					  attribute */
  int    	PsNInheritedAttrs[MAX_RULES_SSCHEMA]; /* for each element, in
					  the same order as in the table
					  StructSchema.SsRule, indicates the
					  number of attributes inherited by the
					  element */
  InheritAttrTable *PsInheritedAttr[MAX_RULES_SSCHEMA]; /* points for each type
					  of element, in the same order as in
					  the table StructSchema.SsRule, on the
					  table indicating what are the attributes
					  inherited by the element */
  int    	PsNComparAttrs[MAX_ATTR_SSCHEMA]; /* indicates for each attribute,
					  in the same order as in the table
					  StructSchema.SsRule, the number of
					  attributes comparing themselves to
					  the attribute in order to deduce the
					  presentation */
  ComparAttrTable *PsComparAttr[MAX_ATTR_SSCHEMA]; /* points for each attribute,
					  in the same order as in the table
					  StructSchema.SsRule, on the table
					  indicating which attribute compare
					  themselves to the attribute in order to
					  deduce a presentation */
  ThotBool    PsAcceptPageBreak[MAX_RULES_SSCHEMA]; /* indicates for each 
element
					  type, in the same order as in the
					  table StructSchema.SsRule, if the
					  element can be broken at the bottom
					  of a page */
  ThotBool    PsAcceptLineBreak[MAX_RULES_SSCHEMA];	/* indicates for each 
element
					  type, in the same order as in the
					  table StructSchema.SsRule, if the
					  element can be broken at the end of a
					  line */
  ThotBool    PsBuildAll[MAX_RULES_SSCHEMA];    /* indicates for each element
					  type, in the same order as in the
					  table StructSchema.SsRule, if the
					  image of the box must be built in one
					  piece or if it can be divided */
  ThotBool    PsInPageHeaderOrFooter[MAX_RULES_SSCHEMA]; /* indicates for each
					  element type, in the same order as
					  in the table StructSchema.SsRule, if
					  the element is displayed in the body
					  of the pages (false) or in a footer
					  or header box (true) */
  ThotBool    PsAssocPaginated[MAX_RULES_SSCHEMA];/* indicates for each element
					  type, in the same order as in the
					  table StructSchema.SsRule, if the
					  element is paginated (meaningful only
					  for the associated elements lists) */
  int	      PsElemTransmit[MAX_RULES_SSCHEMA];/* for each element
					  type, in the same order as in the
					  table StructSchema.SsRule, index in the
					  table SPTransmit of the entry giving
					  the transmission of the element values
					  to the attributes of the included
					  documents */
  int	      PsNTransmElems;		  /* number of entries in the table
					  PsTransmElem */
  TransmitElem	PsTransmElem[MAX_TRANSM_ELEM];/* table of the transmissions of
					  the element values to attributes of
					  included documents */
} PresentSchema;

#endif /* _THOTLIB_TYPEPRS_H_ */
