/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2005
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * Data structures for the Thot translator.
 *
 * Type declarations for document translation
 *
 * Author: V. Quint (INRIA)
 *
 */

#ifndef _TYPETRA_H
#define _TYPETRA_H
#include "consttra.h"

/*
   DEFINITIONS:

   A counter number is the rank of a counter in the table
	TranslSchema.TsCounter.
   A constant number is the rank of a constant in the table
	TranslSchema.STConst.
   A variable number is the rank of a variable in the table
	TranslSchema.TsVariable.
*/

/* a translation rule application condition type */
typedef enum
{
	TcondFirst,
	TcondLast,
	TcondReferred,
	TcondFirstRef,
	TcondLastRef,
	TcondWithin,
	TcondFirstWithin,
	TcondAttr,
	TcondPresentation,
	TcondPRule,
	TcondAttributes,
	TcondFirstAttr,
	TcondLastAttr,
	TcondScript,
	TcondComputedPage,
	TcondStartPage,
	TcondUserPage,
	TcondReminderPage,
	TcondEmpty,
	TcondRoot,
	TcondExternalRef,
	TcondElementType,
	TcondTransclusion
} TransCondition;

/* a translation rule type */
typedef enum
{
	TCreate,
	TGet,
	TUse,
	TRemove,
	TWrite,
	TRead,
	TInclude,
	TNoTranslation,
	TCopy,
	TChangeMainFile,
	TSetCounter,
	TAddCounter,
	TNoLineBreak,
	TIndent,
	TRemoveFile,
	TIgnore
} TRuleType;

/* the position where the translator-produced strings are created */
typedef enum
{
	TAfter, 
	TBefore
} TOrder;

/* The translation rules relative to an object are grouped by blocks.
   Rules from a same block apply depending on the same condition.
   A string of rule blocks applies to one object. This way the translation
   rule blocks of a type defined in the structure schema are linked,
   as well as the translation rule blocks for an attribute value.
*/

/* a pointer on a translation rule */
typedef struct _TranslRule *PtrTRule;

/* a pointer on a translation rule block */
typedef struct _TRuleBlock *PtrTRuleBlock;

/* to interpret the field TcAscendLevel */
typedef enum
{
	RelGreater, RelLess, RelEquals
}       RelatNAscend;

/* a translation rule block application condition */
typedef struct _TranslCondition
{
	TransCondition  TcCondition;     /* type of the application condition
					    of the block's rules */
	ThotBool        TcNegativeCond;	 /* the condition is negative */
	ThotBool	TcTarget;        /* the condition is on the referenced
					    element */
	int		TcAscendType; 	 /* type of the ascendent element
					    affected by the condition */
	Name		TcAscendNature;  /* name of the schema where this
					    element type name is defined */
	int		TcAscendRelLevel;/* relative level of the ascendent
					    element affected by the condition.
					    0  = the element itself
					    -1 = the element itself or any
						 ascendent */
	union
	  {
	  struct	/* TcCondition = TcondScript */
	    {
	    char         _TcScript_;   /* the alphabet on which the condition
			 		    applies */ 
	    } s0;
	  struct	 /* TcCondition = TcondWithin, TcondFirstWithin */
	    {
	    int		 _TcElemType_; 	 /* type of the ascendent */
	    Name	 _TcElemNature_; /* name of the nature where TcElemType
					    is defined, 0 if same schema */
	    ThotBool	 _TcImmediatelyWithin_;    /* Immediately within */
	    RelatNAscend _TcAscendRel_;
	    int		 _TcAscendLevel_;/* RelLevel */
	    } s1;
	  struct	/* TcCondition = TcondAttr || TcCondition = TcondPRule
			   TcCondition = TcondElementType */
	    {
	    int		_TcAttr_;        /* attribute num, type num or type
				            of the presentation rule */ 
	    union
	      {
	      struct	/* AttribType = AtNumAttr */
	        {
	        int	_TcLowerBound_;	 /* minimum value of the attribute such
					    that the block's rules be applied*/
	        int	_TcUpperBound_;	 /* maximum value of the attribute such
					    that the block's rules be applied*/
	        } s0;
	      struct	/* AttribType = AtTextAttr */
	        {
	        Name	_TcTextValue_;	/* the value triggering the application
					   of the block's rules */
	        } s1;
	      struct	/* AttribType = AtEnumAttr */
	        {
	        int	 _TcAttrValue_;	/* value of the attribute for which the
					   block's rules are applied, 0 if
					   applied for any value */
	        } s2;
	      struct	/* in the case TcondPRule only */
	        {
	        char  _TcPresValue_;    /* value of the presentation for which
					   the block's rules are applied, 0 if
					   applied for any value */
	        } s3;
	      } u;
	    } s2;
	  } u;

} TranslCondition;

#define TcScript u.s0._TcScript_
#define TcImmediatelyWithin u.s1._TcImmediatelyWithin_
#define TcAscendRel u.s1._TcAscendRel_
#define TcAscendLevel u.s1._TcAscendLevel_
#define TcElemType u.s1._TcElemType_
#define TcElemNature u.s1._TcElemNature_
#define TcAttr u.s2._TcAttr_
#define TcLowerBound u.s2.u.s0._TcLowerBound_
#define TcUpperBound u.s2.u.s0._TcUpperBound_
#define TcTextValue u.s2.u.s1._TcTextValue_
#define TcAttrValue u.s2.u.s2._TcAttrValue_
#define TcPresValue u.s2.u.s3._TcPresValue_

/* a translation rules block */
typedef struct _TRuleBlock
{
	PtrTRuleBlock   TbNextBlock; 	/* pointer on the next rules block */
	PtrTRule        TbFirstTRule;  	/* pointer on the first rule of the
					   block */
	int		TbNConditions; 	/* number of conditions in
					   TbCondition */
	TranslCondition	TbCondition[MAX_TRANSL_COND]; /* the conditions that
					   have to be satisfied to apply the
					   rules of the block */
} TRuleBlock;

/* type of object to create in the output */
typedef enum
{
  	ToConst,
	ToBuffer,
	ToVariable,
	ToAttr,
	ToPRuleValue,
	ToContent,
	ToAllAttr,
	ToAllPRules,
	ToRefId,
	ToPairId,
	ToReferredElem,
	ToDocumentName,
	ToDocumentDir,
	ToReferredDocumentName,
	ToReferredDocumentDir,
	ToReferredRefId,
	ToFileDir,
	ToFileName,
	ToExtension,
	ToTranslatedAttr
} CreatedObject;

/* relative position in the abstract tree of the element to take by a Get or
   Copy rule */
typedef enum
{
  	RpSibling,
	RpDescend,
	RpReferred
} TRelatPosition;

/* type of an Indent rule */
typedef enum
{
	ItAbsolute,
        ItRelative,
	ItSuspend,
	ItResume
} TIndentType;

/* a translation rule */
typedef struct _TranslRule
{
  PtrTRule    TrNextTRule;	/* next rule in the same block */
  TOrder      TrOrder;		/* relative position */
  TRuleType   TrType;		/* type of the rule */
  union
    {
      struct	/* TrType = TCreate, TWrite */
	{
	  CreatedObject _TrObject_;	 /* type of the object to create for
					    the output */
	  int	 	_TrObjectNum_;	 /* number of the object to create or
					    write */
	  Name		_TrObjectNature_;/* schema where the element is
					    defined, 0 if same struct. schema*/
	  ThotBool	_TrReferredObj_; /* the variable to be created must be
					    computed for the referenced
					    element, not the element itself */
	  int		_TrFileNameVar_; /* variable defining the name of the
					    output file */
	} s0;
      struct	/* TrType = TGet, TCopy */
	{
	  int		_TrElemType_;	 /* type of the element to take */
	  Name	 	_TrElemNature_;	 /* schema where the element is defined
					    0 if same structure schema */
	  TRelatPosition _TrRelPosition_;/* relative position in the abstract
					    tree of the element to take */
	} s1;
      struct	/* TrType = TUse */
	{
	  Name	_TrNature_;		 /* name of the nature */
	  Name	_TrTranslSchemaName_;	 /* name of the translation schema
					    to apply */
	} s2;
      struct	/* TrType = TRead */
	{
	  int	_TrBuffer_; 		 /* number of the read buffer */
	} s4;
      struct	/* TrType = TInclude */
	{
	  CreatedObject  _TrBufOrConst_; /* the name of the file to include is
					    in a buffer or a constant */
	  int	_TrInclFile_; 	         /* number of the constant or the
					    buffer containing the name of
					    the file to include */
	} s5;
      struct	/* TrType = TChangeMainFile, TRemoveFile */
	{
	  int	_TrNewFileVar_;	         /* variable defining the new filename*/ 
	} s6;
     struct	/* TrType = TSetCounter, TAddCounter */
	{
	  int	_TrCounterNum_;          /* number of the counter to update */ 
	  int	_TrCounterParam_;        /* update value */ 
	} s7;
     struct	/* TrType = TIndent */
	{
	  int	_TrIndentFileNameVar_;   /* variable defining the name of the
					    output file */
	  int   _TrIndentVal_;	         /* indent value */
	  TIndentType _TrIndentType_;	 /* Indent value is relative to parent*/
	} s8;
    } u;
} TranslRule;

#define TrObject u.s0._TrObject_
#define TrObjectNum u.s0._TrObjectNum_
#define TrObjectNature u.s0._TrObjectNature_
#define TrReferredObj u.s0._TrReferredObj_
#define TrFileNameVar u.s0._TrFileNameVar_
#define TrElemType u.s1._TrElemType_
#define TrElemNature u.s1._TrElemNature_
#define TrRelPosition u.s1._TrRelPosition_
#define TrNature u.s2._TrNature_
#define TrTranslSchemaName u.s2._TrTranslSchemaName_
#define TrBuffer u.s4._TrBuffer_
#define TrBufOrConst u.s5._TrBufOrConst_
#define TrInclFile u.s5._TrInclFile_
#define TrNewFileVar u.s6._TrNewFileVar_
#define TrCounterNum u.s7._TrCounterNum_
#define TrCounterParam u.s7._TrCounterParam_
#define TrIndentFileNameVar u.s8._TrIndentFileNameVar_
#define TrIndentVal u.s8._TrIndentVal_
#define TrIndentType u.s8._TrIndentType_

/* counter operation type */
typedef enum
{
	TCntrRank,
	TCntrRLevel,
	TCntrSet,
	TCntrNoOp
} TCounterOp;

/* a counter */
typedef struct _TCounter
{
	TCounterOp TnOperation;    /* type of the counter operation */
	int 	   TnElemType1;    /* first element type on which the
				      counter operation is applied */
	int	   TnAcestorLevel; /* if "CntrRank of", level of the ancestor
				      that is to be counted */
	int 	   TnElemType2;	   /* second element type on which the
				      counter operation is applied */
	int        TnParam1;	   /* first operation parameter */
	int        TnParam2;	   /* second operation parameter */
	int  TnAttrInit;	   /* TtAttribute is the root giving the
				      counter its initial value, 0 if the
				      initial value doesn't depend on any
				      attribute */
} TCounter;

/* types of the elements of the translation variables */
typedef enum
{
	VtText,
	VtCounter,
	VtBuffer,
	VtAttrVal,
	VtFileName,
	VtExtension,
	VtDocumentName,
	VtDocumentDir,
	VtFileDir	
} TranslVarType;

/* a translation variable element */
typedef struct _TranslVarItem
{
	TranslVarType   TvType;	        /* type of the variable element */
	int             TvItem;		/* number of the constant, the counter,
					   the attribute or the buffer making
					   up the variable element */
	int             TvLength;	/* length of the number to generate, in
					   the case of a counter only */
	CounterStyle	TvCounterStyle;	/* style of the number to generate,
					   in the case of a counter only */
} TranslVarItem;

/* a translation variable is the concatenation of the result of
   several elements */
typedef struct _TranslVariable
{
	int 	      TrvNItems;	/* effective number of elements */
	TranslVarItem TrvItem[MAX_TRANSL_VAR_ITEM]; /* list of the elements */ 
} TranslVariable;

/* a read buffer */
typedef char          TranslBuffer[MAX_TRANSL_BUFFER_LEN];

/* an application case of the translation rules for a numerical value
   attribute */
typedef struct _TranslNumAttrCase
{
	int           TaLowerBound; /* minimum value of the attribute such that
				       the translation rules be applied */
	int           TaUpperBound; /* maximum value of the attribute such that
				       the translation rules be applied */
	PtrTRuleBlock TaTRuleBlock; /* rules block to apply when the value of
				       the attribute is in the interval */
} TranslNumAttrCase;

/* translation of a logical attribute */
typedef struct _AttributeTransl
{
  int	AtrElemType;	/* element type to which the translation rules apply,
			   0 if the rules apply whatever the element type is */
  union
    {
      struct	/* AttribType = AtNumAttr */
	{
	  /* number of translation rules application cases */
	  int                _AtrNCases_;
	  /* the translation rules application cases */
	  TranslNumAttrCase  _AtrCase_[MAX_TRANSL_ATTR_CASE];
	} s0;
      struct	/* AttribType = AtReferenceAttr */
	{
	  PtrTRuleBlock _AtrRefTRuleBlock_; /* rules block to apply for the
					       attribute */
	} s1;
      struct	/* AttribType = AtTextAttr */
	{
	  Name          _AtrTextValue_;	    /* the value triggering the
					       application of the translation
					       rules */
	  PtrTRuleBlock _AtrTxtTRuleBlock_; /* rules block to apply for this
					       value */
	} s2;
      struct	/* AttribType = AtEnumAttr */
	{
	  /* address of the first translation rules block associatied with each
	     value of the attribute, in the same order than in the table
	     AttrEnumValue */
	  PtrTRuleBlock _AtrEnuTRuleBlock_[MAX_ATTR_VAL + 1];
	} s3;
    } u;
} AttributeTransl;

#define AtrNCases u.s0._AtrNCases_
#define AtrCase u.s0._AtrCase_
#define AtrRefTRuleBlock u.s1._AtrRefTRuleBlock_
#define AtrTextValue u.s2._AtrTextValue_
#define AtrTxtTRuleBlock u.s2._AtrTxtTRuleBlock_
#define AtrEnuTRuleBlock u.s3._AtrEnuTRuleBlock_

typedef AttributeTransl *PtrAttributeTransl;

typedef struct _AttrTransTable
{
       PtrAttributeTransl TsAttrTransl[1];
} AttrTransTable;

typedef struct _BlnTable
{
       ThotBool       Bln[1];
} BlnTable;

/* translation of a specific presentation */
typedef struct _PRuleTransl
{
  ThotBool  RtExist;    /* there are translation rules for this presentation */
  union
    {
      struct	/* numerical value presentation */
	{
	  int		    _RtNCase_;  /* number of translation rules
					   application cases */
	  TranslNumAttrCase _RtCase_[MAX_TRANSL_PRES_CASE];
					/* the translation rules application
					   cases */
	} s0;
      struct	/* other presentations */
	{
	  char          _RtPRuleValue_[MAX_TRANSL_PRES_VAL + 1]; 
				      /* possible values of the presentation */
	  PtrTRuleBlock	_RtPRuleValueBlock_[MAX_TRANSL_PRES_VAL + 1]; 
				      /* address of the first translation rules
					 block associated with each value of
					 the presentation, in the order of the
					 table RtPRuleValue */
	} s1;
    } u;
} PRuleTransl;

#define RtNCase u.s0._RtNCase_
#define RtCase u.s0._RtCase_
#define RtPRuleValue u.s1._RtPRuleValue_
#define RtPRuleValueBlock u.s1._RtPRuleValueBlock_

/* strings for a characters translation rule */
typedef unsigned char  SourceString[MAX_SRCE_LEN + 1];	 /* source string */
typedef unsigned char  TargetString[MAX_TARGET_LEN + 1]; /* target string */

/* an entry of the translation table */
typedef struct _StringTransl
{
  SourceString   StSource;	/* source string */
  TargetString   StTarget;	/* traget string */
} StringTransl;

/* indices of the text translation rules for an alphabet */
typedef struct _ScriptTransl
{
  char    AlScript;
  int     AlBegin; 	/* index of the first characters translation
			   rule in the table TsCharTransl */
  int	  AlEnd;	/* index of the last characters translation
			   rule in the same table */
} ScriptTransl;

/* table of translation rules for each element type */
typedef struct _ElemTransTable
{
  PtrTRuleBlock TsElemTransl[1];
} ElemTransTable;

/* descriptor of a buffer of type variable */
typedef struct _TranslVarBuffer
{
  Name    VbIdent;      /* name of the buffer */
  int     VbNum;        /* rank of the buffer in table TsBuffer */
} TranslVarBuffer;

/* pointer on a translation schema */
typedef struct _TranslSchema *PtrTSchema;

/* translation schema associated with a document or object class */
typedef struct _TranslSchema
{
  PtrTSchema     TsNext;       /* to link free blocks */
  Name           TsStructName; /* name of the structure schema */
  int            TsStructCode; /* code identifying the version of this
				  structure schema */
  int            TsLineLength; /* max length of translated lines */
  Name	         TsEOL;        /* end of line character */
  Name           TsTranslEOL;  /* character string to insert at the end
				  of translated lines */
  int 	         TsNConstants; /* number of constants */
  int 	         TsNCounters;  /* number of counters */
  int 	         TsNVariables; /* number of translation variables */
  int 	         TsNBuffers;   /* number of buffers */
  int            TsConstBegin[MAX_TRANSL_CONST]; /* index of the first
				  character of each constant in the buffer
				  TsConstant */
  TCounter       TsCounter[MAX_TRANSL_COUNTER];	/* the counters */
  TranslVariable TsVariable[MAX_TRANSL_VARIABLE];/* the variables */
  int 	         TsPictureBuffer;	        /* number of the image buffer*/
  int            TsNVarBuffers; /* number of buffers of type variable */
  TranslVarBuffer TsVarBuffer[MAX_TRANSL_BUFFER]; /* decriptors of buffers of
						     type variable */
  TranslBuffer   TsBuffer[MAX_TRANSL_BUFFER];  	/* the buffers */
  ElemTransTable *TsElemTRule; /* pointers on the beginning of
				  the string of translation rules associated
				  with each element type, in the same order
				  as in the table StructSchema.SsRule */
  BlnTable       *TsInheritAttr;/* indicates for each
				  element type, in the same order as in the
				  table StructSchema.SsRule, if the element
				  inherits from an attribute on an ancestor */
  AttrTransTable *TsAttrTRule; /* translation rules of the logical attributes,
				  in the same order as in the table
				  StructSchema.SsAttribute */ 
  PRuleTransl    TsPresTRule[MAX_TRANSL_PRULE];	/* translation rules of the
				  specific presentation, in the order of the
				  present. rules types */
  int 	       TsNTranslScripts; /* number of element in the array
				      TsTranslScript */
  ScriptTransl TsTranslScript[MAX_TRANSL_ALPHABET];/* translation of
				  character strings for the alphabets */
  int 	       TsSymbolFirst;  /* index of the first symbols translation
				  rule in the table TsCharTransl */
  int 	       TsSymbolLast;   /* index of the last symbols translation
				  rule in the same table */  
  int 	       TsGraphicsFirst;/* index of the first graphics
				  translation rule in the table
				  TsCharTransl */
  int 	       TsGraphicsLast; /* index of the last graphics translation
				  rule in the same table */
  int 	       TsNCharTransls; /* total number of characters translation
				  rules */
  StringTransl TsCharTransl[MAX_TRANSL_CHAR];	/* the characters
						   translation table */
  unsigned char TsConstant[MAX_TRANSL_CONST_LEN]; /* string of all the
				  translation constants, separated by NULL */
} TranslSchema;
#endif
