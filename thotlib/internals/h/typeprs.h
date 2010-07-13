/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2010
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
    DefaultPres,
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
    RlContainsRef, RlRoot, RlReferred, RlCreator, RlLastSibling
  } Level;

/* Kind of object used as a reference in a relative position */
typedef enum
  {
    RkElType, RkPresBox, RkAttr, RkAnyElem, RkAnyBox
  } RefKind;

/* relative positionning rule for the dimensions or axes of two boxes */
typedef struct _PosRule
{
  BoxEdge	  PoPosDef;	      /* side of the abstract box of which the
                               position is defined */
  BoxEdge	  PoPosRef;	      /* side of the abstract box from which the
                               position is defined */
  TypeUnit	PoDistUnit;   	/* PoDistance is expressed in picas, 1/10 of
                               a character, etc. */
  TypeUnit  PoDeltaUnit;    /* units for PoDistDelta */
  ThotBool	PoDistAttr;	    /* PoDistance is a numerical attribute number
                               or a numerical value */
  int		    PoDistance;	    /* distance between those two sides */
  int       PoDistDelta;    /* increment to the distance */
  Level		  PoRelation;	    /* relative level of the reference element
                               in the internal representation */
  ThotBool	PoNotRel;	      /* if 'false', PoRefIdent indicates the type of
                               the reference element, if 'true', the
                               excluded type. */
  ThotBool	PoUserSpecified;/* the distance may be chosen by the user
                               during the creation */
  RefKind	  PoRefKind;	    /* the reference is the box of an element,
                               a presentation box or the box of an element
                               with an attribute */
  int		    PoRefIdent;	    /* depending on PoRefKind, element type number,
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
      PosRule	_DrPosRule_;      /* the position rule that defines the
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
      ThotBool	_DrMin_; 	      /* minimum dimension */
      ThotBool	_DrUserSpecified_; /* the distance may be chosen by the user */
      int	_DrValue_;            /* value of the percentage, the increment or
                                   the absolute value */
      Level	_DrRelation_;  	    /* relative level of the reference element */
      ThotBool	_DrNotRelat_;   /* if false, DrTypeRefElem or DrRefPresBox
                                   indicates the type of the reference elt.
                                   if false, the exclusive type. */
      RefKind	_DrRefKind_;	    /* the reference is the box of an element,
                                   a presentation box or the box of an element
                                   with an attribute */
      int	_DrRefIdent_;	        /* depending on DrRefKind, element type number,
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
    FreeContent, ContVariable, ContConst
  }	ContentType;

/* a presentation box */
typedef struct _PresentationBox
{
  Name         PbName;		     /* box name */	
  PtrPRule     PbFirstPRule;	 /* first presentation rule defining the box*/
  ThotBool     PbPageFooter;	 /* it is a page footer box */
  ThotBool     PbPageHeader;	 /* it is a page header box */
  ThotBool     PbPageBox;	     /* it is a page box */
  int	       PbFooterHeight;	 /* if it is a page box, size of the footer
                                  in picas */
  int	       PbHeaderHeight;	 /* if it is a page box, size of the header
				    in picas */
  int	       PbPageCounter;	   /* number of the page counter, only if
                                  PbPageBox is true */
  ContentType  PbContent;	     /* compulsory box content */
  union
  {
    struct                			 /* PbContent = ContVariable */
    {
      int      _PbContVariable_; /* number of the variable */
    }   s0;
    struct			                 /* PbContent = ContConst */
    {
      int      _PbContConstant_; /* number of the constant */
    } s1;
    struct			                 /* PbContent = FreeContent */
    {
      int      _PbContFree_;     /* to make the compiler happy */
    } s2;
  } u;
} PresentationBox;

#define PbContVariable u.s0._PbContVariable_
#define PbContConstant u.s1._PbContConstant_
#define PbContFree u.s2._PbContFree_

typedef struct _PresentationBox *PtrPresentationBox;

typedef struct _PresBoxTable
{
  PtrPresentationBox  PresBox[1];
} PresBoxTable;

/* Alignment of the lines in an abstract box */
typedef enum
  {
    AlignLeft, AlignRight, AlignCenter, AlignJustify, AlignLeftDots
  } BAlignment;	            /* AlignLeftDots = aligned to the left, the last
                               line is filled with dots */

/* Positioning algorithm */
typedef enum
  {
    PnStatic,
    PnRelative,
    PnAbsolute,
    PnFixed,
    PnInherit
  } PosAlgorithm;

/* type of a presentation rule */
typedef enum
{
  /* the order determines the order of the rules in the presentation schema */
  /* Don't forget to change constant MAX_TRANSL_PRULE when adding rules */
  /* Don't forget to update PRxxx values in file presentation.h */
  PtVisibility, PtFunction, PtListStyleType, PtListStyleImage,
  PtListStylePosition, PtVertOverflow, PtHorizOverflow, 
  PtVertRef, PtHorizRef, PtHeight, PtWidth, PtVertPos, PtHorizPos,
  PtMarginTop, PtMarginRight, PtMarginBottom, PtMarginLeft,
  PtPaddingTop, PtPaddingRight, PtPaddingBottom, PtPaddingLeft,
  PtBorderTopWidth, PtBorderRightWidth, PtBorderBottomWidth, PtBorderLeftWidth,
  PtBorderTopColor, PtBorderRightColor, PtBorderBottomColor, PtBorderLeftColor,
  PtBorderTopStyle, PtBorderRightStyle, PtBorderBottomStyle, PtBorderLeftStyle,
  PtSize, PtStyle, PtWeight, PtVariant, PtFont, PtUnderline, PtThickness,
  PtIndent, PtLineSpacing, PtDepth, PtAdjust, PtDirection, PtUnicodeBidi,
  PtLineStyle, PtLineWeight, PtFillPattern, PtBackground, PtForeground, PtColor,
  PtStopColor, PtStopOpacity, PtOpacity, PtFillOpacity, PtStrokeOpacity,
  PtFillRule, PtMarker, PtMarkerEnd, PtMarkerMid, PtMarkerStart,
  PtHyphenate, PtPageBreak, PtLineBreak, PtGather,
  PtXRadius, PtYRadius,
  PtPosition, PtTop, PtRight, PtBottom, PtLeft, PtFloat, PtClear,
  PtDisplay, /* PtDisplay must be at the end. If the value is ListItem, this
                makes sure that the marker box can inherit properties from
                the creator box (see function CreateListItemMarker). */
  PtBackgroundHorizPos, PtBackgroundVertPos,
  PtVis, /* CSS visibility */
  /* the three following types must be the last ones */
  PtBreak1, PtBreak2, PtPictInfo
} PRuleType;

/* computing mode of the properties */
typedef enum
  {
    PresImmediate, PresInherit, PresCurrentColor, PresFunction
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
    FnCreateAfter, FnContent, FnColumn, FnCopy, FnContentRef, FnSubColumn,
    FnNoLine, FnCreateEnclosing, FnShowBox, FnBackgroundPicture,
    FnBackgroundRepeat, FnNotInLine, FnAny
  } FunctionType;

/* counter styles */
typedef enum
  {
    CntDecimal, CntZLDecimal, CntURoman, CntLRoman, CntUppercase, CntLowercase,
    CntLGreek, CntUGreek, CntDisc, CntCircle, CntSquare, CntNone
  } CounterStyle;

/* types of a presentation variable */
typedef enum
  {
    VarText, VarCounter, VarDate, VarFDate, VarDirName, VarDocName,
    VarElemName, VarAttrName, VarAttrValue, VarNamedAttrValue, VarPageNumber
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
  CounterStyle     ViStyle;  /* digit style for VarCounter, VarAttrValue
                                and VarPageNumber */
  union
  {
    struct
    {
      int	   _ViConstant_;   /* number of the constant */ 
    } s0;
    struct
    {
      int	   _ViCounter_;    /* number of the counter */
      CounterValue _ViCounterVal_; /* indicates if we are interested in the
                                      maximum, minimum or current value of
                                      the counter */
    } s1;
    struct
    {
      int    _ViAttr_;       /* the attribute number */
    } s2;
    struct
    {
      int    _ViDate_;       /* for the compiler */
    } s3;
    struct			             /* ViType = VarPageNumber */
    {
      int	   _ViView_;	     /* number of the view in which pages are
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
    PcUserPage, PcStartPage, PcComputedPage, PcEmpty, PcRoot, PcEven,
    PcOdd, PcOne, PcInterval, PcWithin, PcSibling, PcElemType, PcAttribute,
    PcInheritAttribute, PcNoCondition, PcDefaultCond
  } PresCondition;

/* To interpret the field CoRelation */
typedef enum
  {
    CondGreater, CondLess, CondEquals
  } ArithRel;

/* To interpret the field CoMatch */
typedef enum
  {
    CoMatch, CoSubstring, CoWord
  } CondMatch;

typedef struct _Condition *PtrCondition;

/* A presentation rule application condition */
typedef struct _Condition
{
  PtrCondition    CoNextCondition; /* Next condition in the list */
  PresCondition   CoCondition;	   /* type of the condition */
  ThotBool        CoChangeElem;    /* this condition apply to the element
                                      selected by the previous conditions */
  ThotBool        CoNotNegative;   /* the condition is not negative */
  ThotBool	  CoTarget;	   /* the condition affects the target
                                      (for references only) */
  union
  {
    struct			   /* CoCondition = PcInterval, PcEven, PcOdd, PcOne */
    {
      int	   _CoCounter_;    /* number of the counter on which the
                                      condition applies */
      int	   _CoMinCounter_; /* minimum value of the counter so that
                                      the presentation rule may be applied */
      int	   _CoMaxCounter_; /* maximum value of the counter so that
                                      the presentation rule may be applied */
      CounterValue _CoValCounter_; /* indicates if the minimum, maximum or
                                      current value of the counter is used */
    } s0;
    struct			   /* CoCondition = PcWithin */
    {
      int       _CoRelation_;	   /* RelLevel */
      int       _CoTypeAncestor_;  /* type of the ancestor */	
      ThotBool  _CoImmediate_;	   /* Immediately */
      ArithRel  _CoAncestorRel_;
      char     *_CoAncestorName_;  /* Ancestor type name, if defined in
                                      another schema */
      Name      _CoSSchemaName_;   /* name of the schema where the ancestor is
                                      defined if CoTypeAncestor == 0 */
    } s1;
    struct			   /* CoCondition = PcElemType */
    {
      int	_CoTypeElem_;	   /* type of the element to which the
                                      attribute is attached */
    } s2;
    struct			   /* CoCondition = PcAttribute */
    {
      int	_CoTypeAttr_;	   /* attribute carried by the element */
      ThotBool  _CoTestAttrValue_; /* the attribute value must be tested */
      union
      {
        struct        /* numerical or enumerated attribute */
        {
          int	_CoAttrValue_;       /* the value that satisfies the condition */
        } s0;
        struct        /* text attribute */
        {
          char     *_CoAttrTextValue_; /* the value that satisfies the condition */
          CondMatch _CoTextMatch_;     /* matching condition: substring, word, ... */
        } s1;
      } u;
    } s3;
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
#define CoTypeElem u.s2._CoTypeElem_
#define CoTypeAttr u.s3._CoTypeAttr_
#define CoTestAttrValue u.s3._CoTestAttrValue_
#define CoAttrValue u.s3.u.s0._CoAttrValue_
#define CoAttrTextValue u.s3.u.s1._CoAttrTextValue_
#define CoTextMatch u.s3.u.s1._CoTextMatch_

typedef enum
{
  BtElement, BtBefore, BtAfter
} GenBoxType;

typedef enum
{
  PrNumValue, PrAttrValue, PrConstStringValue
} PrValType;

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
  PtrSSchema    PrSpecifAttrSSchema; /* pointer on the structure schema
                                        defining the attribute PrSpecifAttr */
  char         *PrCSSURL;       /* points to the current CSS URL */
  int           PrCSSLine;      /* the line number of the CSS rule */
  int		PrViewNum;	/* the view number to which the rule applies */	
  int		PrSpecifAttr;	/* only for specific presentation rules
				   attached to the abstract tree elements:
				   number of the attribute to which the
				   rule corresponds, 0 if the rule is not
				   derived from an attribute rule */
  int           PrSpecificity;  /* Specificity of the corresponding CSS rule.
				   0 if the rule belongs to the default
				   P Schema */
  GenBoxType    PrBoxType;      /* this rule is for the element itself or one
				   of its pseudo-element (generated content) */
  ThotBool      PrImportant;    /* the corresponding CSS rule has a flag
                                   !important */
  ThotBool      PrDuplicate;    /* duplicate of a conditional rule for an
				   inherited attribute */
  PresMode	PrPresMode;	/* computing mode of the value */
  union
  {
    struct			/* PrPresMode = PresInherit
				   PrPresMode = PresCurrentColor */
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
      ThotBool	   _PrExternal_;  /* if PrElement is true, PrExternal indicates
				     that the type of which the name is in
				     PrPresBoxName is external */
      ThotBool     _PrElement_;	  /* PrPresBox[1] or PrPresBoxName is an
				     element type number, not a presentation
				     box number */
      int	   _PrNPresBoxes_;/* number of presentation boxes (of use for
				     the column rule only) */
      int          _PrPresBox_[MAX_COLUMN_PAGE]; /* number of the
					            presentation boxes */
      Name        _PrPresBoxName_;/* Name of the first (or only) presentation
				     box to which the function applies */
    } s1;
    struct			  /* PrPresMode = PresImmediate */
    {
      union
      {
	struct	/* PRuleType = PtVisibility, PtListStyleImage, PtDepth,
		   PtFillPattern, PtBackground, PtForeground, PtColor,
                   PtStopColor,
		   PtBorderTopColor, PtBorderRightColor,
		   PtBorderBottomColor, PtBorderLeftColor,
		   PtOpacity, PtFillOpacity, PtStrokeOpacity, PtStopOpacity,
		   PtMarker, PtMarkerEnd, PtMarkerMid, PtMarkerStart */
	{
	  PrValType _PrValueType_; 	/* PrIntValue is a numerical attribute
					   or numerical value number */ 
	  int  _PrIntValue_;   /* Border colors: -2 means Transparent and
				  -1 means "same color as foreground" */
	}  s0;
	struct	/* PRuleType = PtListStyleType, PtListStylePosition, PtDisplay,
                               PtFont, PtStyle, PtWeight, PtUnderline,
	        	       PtThickness, PtDirection, PtUnicodeBidi,
                               PtLineStyle, PtFloat, PtClear, PtPosition,
	                       PtBorderTopStyle, PtBorderRightStyle,
                               PtBorderBottomStyle, PtBorderLeftStyle,
                               PtFillRule */
	{
	  char     _PrChrValue_;
	}  s1;
	struct	/* PRuleType = PtBreak1, PtBreak2,
	           PtIndent, PtSize, PtLineSpacing, PtLineWeight,
		   PtMarginTop, PtMarginRight, PtMarginBottom, PtMarginLeft,
	           PtPaddingTop, PtPaddingRight, PtPaddingBottom, PtPaddingLeft,
                   PtBorderTopWidth, PtBorderRightWidth, PtBorderBottomWidth,
                   PtBorderLeftWidth, PtXRadius, PtYRadius,
                   PtTop, PtRight, PtBottom, PtLeft,
                   PtBackgroundHorizPos, PtBackgroundVertPos */
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
	struct	/* PRuleType = PtHyphenate, PtVertOverflow,
		   PtHorizOverflow, PtGather, PtPageBreak, PtLineBreak */
	{
	  ThotBool _PrBoolValue_;
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
#define PrValueType u.s2.u.s0._PrValueType_
#define PrIntValue u.s2.u.s0._PrIntValue_
#define PrChrValue u.s2.u.s1._PrChrValue_
#define PrMinUnit u.s2.u.s2._PrMinUnit_
#define PrMinAttr u.s2.u.s2._PrMinAttr_
#define PrMinValue u.s2.u.s2._PrMinValue_
#define PrPosRule u.s2.u.s3._PrPosRule_
#define PrDimRule u.s2.u.s4._PrDimRule_
#define PrBoolValue u.s2.u.s5._PrBoolValue_
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
  PtrCondition  CiCond;	   /* conditions for applying the operation */
  char      *CiCSSURL;     /* points to the current CSS URL */
  int       CiCSSLine;     /* the line number of the CSS rule */
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
  int       CiCondAttrIntValue; /* if CiCondAttr > 0, integer value of the attribute for
				   the element to be counted */
  char     *CiCondAttrTextValue; /* if CiCondAttr > 0, string value of the attribute for
				    the element to be counted */
  ThotBool  CiCondAttrPresent; /* if CiCondAttr > 0, indicates whether elements
			      with that attribute are counted of not */
} CntrItem;

/* a counter */
typedef struct _Counter
{
  int      CnNameIndx;           /* name of the counter (0 if no name, otherwise
				    index in the constant table) */
  int	   CnNItems;	         /* number of operations on this counter */
  CntrItem CnItem[MAX_PRES_COUNT_ITEM];	   /* the operations */
  int	   CnNPresBoxes;         /* Number of elements in CnPresBox */
  int      CnPresBox[MAX_PRES_COUNT_USER]; /* list of the type numbers of the
				    presentation boxes using the counter in
				    their content */
  ThotBool CnMinMaxPresBox[MAX_PRES_COUNT_USER];
  int	   CnNTransmAttrs;       /* number of external attributes to which the
				counter value is transmitted */ 
  Name     CnTransmAttr[MAX_TRANSM_ATTR];  /* names of the attributes to which
				   the counter value is transmitted */
  int	   CnTransmSSchemaAttr[MAX_TRANSM_ATTR]; 	/* type number of
				   the external documents where the
				   CnTransmAttr are defined */
  int	   CnNCreators;	        /* Number of elements in CnCreator */
  int      CnCreator[MAX_PRES_COUNT_USER];  /* list of type numbers of the box
				   that create other boxes depending on the
				   counter value */
  ThotBool CnMinMaxCreator[MAX_PRES_COUNT_USER];
  ThotBool CnPresBoxCreator[MAX_PRES_COUNT_USER]; /* indicates whether the
				   box corresponding of CnCreator is a presentation box */
  int	   CnNCreatedBoxes;     /* Number of elements in CnCreatedBox */
  int      CnCreatedBox[MAX_PRES_COUNT_USER]; /* list of type numbers of
				   the presentation boxes created in regards
				   to the counter value */
  ThotBool CnMinMaxCreatedBox[MAX_PRES_COUNT_USER];
  ThotBool CnPageFooter;	/* this counter is used in a footer */
} Counter;

/* a presentation constant */
typedef struct _PresConstant
{
  BasicType PdType;             /* type of the constant */
  char      PdScript;           /* script of the constant */
  char     *PdString;           /* constant presentation string,
				   terminated by a NUL character */
} PresConstant;

/* a presentation variable is the concatenation of the results of various
   elements */
typedef struct _PresVariable
{
    int		 PvNItems;	/* effective number of elements */
    PresVarItem  PvItem[MAX_PRES_VAR_ITEM]; /* list of the elements */  
} PresVariable;

typedef struct _PresVariable *PtrPresVariable;

typedef struct _PresVarTable
{
  PtrPresVariable  PresVar[1];
} PresVarTable;

typedef enum 
{
  ComparConstant, ComparAttr
} AttrComparType;


/* a case of presentation rules application for a numerical value
   attribute */
typedef struct _NumAttrCase
{
  AttrComparType CaComparType; /* type of the comparison elements */
  int	 	 CaLowerBound; /* minimum value of the attribute such that
				  the presentation rules are applied */
  int	 	 CaUpperBound; /* maximum value of the attribute such that
				  the presentation rules are applied */
  PtrPRule       CaFirstPRule; /* first rule of the chain of rules to apply
				  when the attribute value is in the interval*/
} NumAttrCase;

/* presentation of a logical attribute, according to its type */
typedef struct _AttributePres
{
  int      ApElemType;	  /* type of element to which the presentation rules
			     apply, 0 if the rules apply whatever the element*/
  ThotBool ApElemInherits; /* the rules apply also to the element that has this
			      attribute. Means nothing if ApElemType is 0 */
  struct  _AttributePres *ApNextAttrPres; /* the packet of presentation rules
					     for the next element */
  union
  {
    struct        /* numerical attribute */
    {
      int	  _ApNCases_; /* number of application cases for the
				 presentation rules */
      NumAttrCase _ApCase_[MAX_PRES_ATTR_CASE]; /* the cases of application
						   of the presentation rules */
    } s0;
    struct        /* reference attribute */
    {
      PtrPRule    _ApRefFirstPRule_; /* first rule in the string of rules
					to apply for the attribute */
    } s1;
    struct        /* text attribute */
    {
      char  	  *_ApString_;	/* the value triggering the application of the
				   presentation rules */
      PtrPRule    _ApTextFirstPRule_; /* first rule in the string of rules
					 to apply for this value */
      CondMatch   _ApMatch_;    /* matching condition: substring, word, ... */
    } s2;
    struct        /* enumerated attribute */
    {
      PtrPRule    _ApEnumFirstPRule_[MAX_ATTR_VAL + 1]; /* for each atribute value,
							   in the order of the table
							   AttrEnumValue, ppoints the
							   first presentation rule
							   associated with this value */
    } s3;
  } u;
} AttributePres;

#define ApNCases u.s0._ApNCases_
#define ApCase u.s0._ApCase_
#define ApRefFirstPRule u.s1._ApRefFirstPRule_
#define ApString u.s2._ApString_
#define ApMatch u.s2._ApMatch_
#define ApTextFirstPRule u.s2._ApTextFirstPRule_
#define ApEnumFirstPRule u.s3._ApEnumFirstPRule_

/* view names table */
typedef Name        ViewTable[MAX_VIEW];

typedef struct _HostView *PtrHostView;

/* a host view, i.e. a view whose name follows "MERGE With" in section VIEWS
   of the presentation schema */
typedef struct _HostView
{
    Name            HostViewName;  /* name of this host view */
    PtrHostView     NextHostView;  /* next host view for the same view */
} HostView;

/* description of a view to print */
typedef struct _PrintedView
{
  int	 	VpNumber;    	/* view number */
} PrintedView;

/* a rule of transmission of a value from an element to an included
   document attribute */
typedef struct _TransmitElem
{
    int		TeTargetDoc;	/* index of the structure rule defining
				   the included document type */
    Name 	TeTargetAttr;   /* name of the included document
				   attribute to which the element value
				   is transmitted */
} TransmitElem;

typedef char InheritAttrTable[1];  /* this table is attached to an element
				   and indicates what are the attributes
				   that this element inherits:
                                     '\0': no inheritance for this attribute,
                                     'S': inherit from ancestors or self,
                                     'H'  inherit from ancestors only */
typedef ThotBool ComparAttrTable[1];   /* this table is attached to an element
					  and indicates what are the attributes
					  comparing themselves to it for
					  presentation */

typedef struct _InheritAttrTbTb
{
  InheritAttrTable  *ElInherit[1];
} InheritAttrTbTb;

typedef struct _CompAttrTbTb
{
  ComparAttrTable *CATable[1];
} CompAttrTbTb;

typedef struct _AttributePres *PtrAttributePres;

typedef struct _AttrPresTable
{
  PtrAttributePres  AttrPres[1];
} AttrPresTable;

typedef struct _PtrPRuleTable
{
  PtrPRule ElemPres[1];
} PtrPRuleTable;

typedef struct _NumberTable
{
  int           Num[1];
} NumberTable;

/* origin of a style sheet */
typedef enum
{
  Agent, User, Author
} StyleSheetOrigin;

typedef struct _PresentSchema *PtrPSchema;

/* a presentation schema loaded in memory */
typedef struct _PresentSchema
{
  PtrPSchema    PsNext;    	        /* for free blocks linking */
  PtrSSchema    PsSSchema;              /* Structure schema */
  char         *PsStructName;    	/* name of the structure schema */
  char	       *PsPresentName;		/* name of this presentation schema */
  int	 	PsStructCode;    	/* code identifying the version of this
					   structure schema */
  StyleSheetOrigin PsOrigin;            /* origin of this structure schema */
  int		PsNViews;	        /* number of views */
  ViewTable     PsView;    		/* definition of the views */
  PtrHostView   PsHostViewList[MAX_VIEW]; /* for each view defined in ViewTable
				             pointer to its first host view */
  ThotBool      PsPaginatedView[MAX_VIEW];/* indicates the paginated views */
  ThotBool      PsColumnView[MAX_VIEW];	  /* indicates the views separated in
					     columns */
  int		PsNPrintedViews;          /* number of views to be printed */
  PrintedView   PsPrintedView[MAX_PRINT_VIEW];/* the views to be printed */
  ThotBool      PsExportView[MAX_VIEW];   /* indicates the views that display
					     only the exported elements */
  int		PsNCounters;	    /* number of counters */
  int		PsNConstants;	    /* number of presentation constants */
  int		PsNVariables;	    /* number of presentation variables */
  int           PsVariableTableSize;/* size of table PsVariable */
  int		PsNPresentBoxes;    /* number of presentation and layout
				       boxes that are actually defined */
  int           PsPresentBoxTableSize; /* size of table PsPresentBox */
  int           PsNElemPRule;       /* number of known structure rules */
  PtrPRule      PsFirstDefaultPRule;/* beginning of the default rules string */
  Counter       PsCounter[MAX_PRES_COUNTER];  /* counters */
  PresConstant  PsConstant[MAX_PRES_CONST];   /* presentation constants */
  PresVarTable  *PsVariable;        /* presentation variables*/
  PresBoxTable  *PsPresentBox;      /* descriptions of the presentation
				       and layout boxes */
        /* For columns layout, box number 0 contains the Column group box,
	         box number 1 contains the left-hand column, etc. */
  AttrPresTable *PsAttrPRule;       /* pointers on the presentation
					                             rules of the logical attributes,
					                                 in the same order as in the table
					                                 StructSchema.SsAttribute */
  NumberTable	*PsNAttrPRule;          /* number of presentation rules
					                               packets for each logical attribute,
					                               i.e. size of the strings of
					                               AttributePres in table PsAttrPRule
					                               in the same order as in table
					                               StructSchema.SsAttribute */
  PtrPRuleTable	*PsElemPRule;    	/* pointers on the beginning of the
					   string of presentation rules related
					   to each type of element, in the same
					   order as in table SsRule */
  NumberTable	*PsNHeirElems;         /* indicates for each attribute,
					  in the same order as in the table
					  StructSchema.SsAttribute, the number
					  of elements that can inherit from the
					  attribute */
  NumberTable   *PsNInheritedAttrs;    /* for each element, in the same order
					  as in table SsRule, indicates the
					  number of attributes inherited by the
					  element */
  InheritAttrTbTb *PsInheritedAttr;    /* points for each type
					  of element, in the same order as in
					  the table StructSchema.SsRule, on the
					  table indicating what are the
					  attributes inherited by the element*/
  NumberTable   *PsNComparAttrs;       /* indicates for each attribute,
					  in the same order as in
					  the table StructSchema.SsAttribute,
					  the number of attributes comparing
					  themselves to the attribute in order
					  to deduce the presentation */
  CompAttrTbTb  *PsComparAttr;         /* points for each attribute
					  in the same order as in the table
					  StructSchema.SsAttribute on the table
					  indicating which attribute compare
					  themselves to the attribute in order
					  to deduce a presentation */
  NumberTable   *PsElemTransmit;       /* for each element type, in the same
					  order as in table SsRule, index in
					  table SPTransmit of the entry giving
					  the transmission of the element
					  values to the attributes of the
					  included documents */
  int	        PsNTransmElems;	       /* number of entries in the table
					  PsTransmElem */
  TransmitElem	PsTransmElem[MAX_TRANSM_ELEM];/* table of the transmissions of
					  the element values to attributes of
					  included documents */
} PresentSchema;

#endif /* _THOTLIB_TYPEPRS_H_ */
