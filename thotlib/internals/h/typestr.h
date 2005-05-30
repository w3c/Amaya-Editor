/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2005
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 
#ifndef _THOTLIB_TYPESTR_H_
#define _THOTLIB_TYPESTR_H_

/*
 * Type declarations for the Editor structure schemas
 *
 * Author: V. Quint (INRIA)
 *
 */
 
/* DEFINITIONS:
 
   The structure of every element is defined by a rule from the table
   StructSchema.SsRule.
   A rule number is the rank of the rule in the table
     StructSchema.SsRule.
   An attribute number is the rank of an attribute in the table
     StructSchema.SsAttribute.
   An attribute value number is the rank of the value in the table
     AttrEnumValue of the attribute.
*/
 
#include "appstruct.h"
 
typedef char    Name[MAX_NAME_LENGTH]; /* a name terminated by a null char */

/* values for using schema or user attribute and element type names */ 
#define USER_NAME 1
#define SCHEMA_NAME 0

/* constructors for structure rules */
typedef enum
{
        CsIdentity,
        CsList,
        CsChoice,
        CsAggregate,
        CsUnorderedAggregate,
        CsConstant,
        CsReference,
        CsBasicElement,
        CsNatureSchema,
        CsPairedElement,
        CsExtensionRule,
        CsDocument,
        CsAny,
        CsEmpty
} RConstruct;
#define MAX_CONSTRUCT   14      /* number of values for RConstruct */

/* basic types known by Thot (refer to the constant MAX_BASIC_TYPE) */
typedef enum
{
        CharString,
        GraphicElem,
        Symbol,
        tt_Picture,
        Refer,
        PageBreak,
        AnyType
} BasicType;
#define MAX_BASIC_TYPE 7        /* number of values for BasicType */
 
typedef enum
{
        AtNumAttr, 		/* integer attribute */
        AtTextAttr,		/* text attribute */
        AtReferenceAttr,	/* reference attribute */
        AtEnumAttr		/* enumerated attribute */
} AttribType;
#define MAX_ATTR_TYPE   4       /* number of values for AttribType */
 
/* A definition of an attribute in a structure schema */
/* Each attribute definition includes the attribute name and, if it is
   an enumerated attribute, the names of its possible values. */
 
typedef struct _TtAttribute
{
        char      *AttrName;          /* name of the attribute, may be
                                         translated in the user's language */
        char      *AttrOrigName;      /* real name of the attribute */
        ThotBool   AttrGlobal;        /* the attribute can apply to all
                                         the elements defined in the schema */
        int        AttrFirstExcept;   /* index in SsException of the first
                                          exception number associated with this
                                          attribute, 0 if no exception is
                                          associated */
        int        AttrLastExcept;    /* index in SsException of the last
                                          exception number associated with this
                                          attribute */
        AttribType AttrType;          /* attribute type */
        union
        {
          struct                      /* AttribType = AtReferenceAttr */
          {
            /* number of the rule defining the type of element referenced by
               the attribute */
            int     _AttrTypeRef_;
            /* structure schema where the type of referenced element is
               defined, 0 if same schema */
            Name             _AttrTypeRefNature_;
          } s2;
	  struct		      /* AttribType = AtEnumAttr */
	  {
	    /* number of possible values (effective size of the table
	       AttrEnumValue) */
	    int   _AttrNEnumValues_;
	    /* translated names of those values */
	    Name           _AttrEnumValue_[MAX_ATTR_VAL];
	    /* original names */
	    Name           _AttrEnumOrigValue_[MAX_ATTR_VAL];
	    
	  } s3;
	} u;
} TtAttribute;

#define AttrTypeRef u.s2._AttrTypeRef_
#define AttrTypeRefNature u.s2._AttrTypeRefNature_
#define AttrNEnumValues u.s3._AttrNEnumValues_
#define AttrEnumValue u.s3._AttrEnumValue_
#define AttrEnumOrigValue u.s3._AttrEnumOrigValue_

typedef TtAttribute *PtrTtAttribute;

typedef struct _TtAttrTable
{
        PtrTtAttribute TtAttr[1];
} TtAttrTable;

typedef struct _NumTable
{
  int           Num[1];
} NumTable;

typedef struct _BoolTable
{
  ThotBool      Bln[1];
} BoolTable;

typedef struct _StructSchema *PtrSSchema;

/* A rule defining an element type in a structure schema */
typedef struct _SRule
{
	char      *SrName;	    /* left-hand symbol of the rule =
				       element type defined by the rule */
        char      *SrOrigName;      /* real name of the rule */
	int 	   SrNDefAttrs;     /* 0..MAX_DEFAULT_ATTR, number of
				       attributes with a default value */
	int        SrDefAttr[MAX_DEFAULT_ATTR]; /* default value attributes */
	int        SrDefAttrValue[MAX_DEFAULT_ATTR];/* default values of these
				       attributes, in the same order */
	ThotBool   SrDefAttrModif[MAX_DEFAULT_ATTR];
	int 	   SrNLocalAttrs;   /* number of attributes that can apply to
				       the element type, i.e. size of tables
				       SrLocalAttr and SrRequiredAttr */
	NumTable*  SrLocalAttr;     /* attributes that can apply to the
				       element type */ 
	BoolTable* SrRequiredAttr;  /* indicate whether the local attribute of
				       same rank is mandatory */
	ThotBool   SrUnitElem;      /* it is an exported unit */
	ThotBool   SrRecursive;     /* recursive rule */
	ThotBool   SrRecursDone;    /* already applied recursive rule */
	ThotBool   SrExportedElem;  /* this type of element is exported */
        int        SrExportContent; /* type of element making up the content
				       if the element is exported */
	Name       SrNatExpContent; /* structure schema where SrExportContent
				       is defined, byte 0 if same schema */
	int        SrFirstExcept;   /* index in SsException of the first
				       exception number associated with this
				       element type, 0 if none */
	int        SrLastExcept;    /* index in SsException of the last
				       exception number associated with this
				       element type */
	int	   SrNInclusions;   /* number of SGML-style inclusions ? */
	int	   SrInclusion[MAX_INCL_EXCL_SRULE]; /* the SGML-style included
						     element types */
	int	   SrNExclusions;   /* number of SGML-style exclusions ?*/
        int	   SrExclusion[MAX_INCL_EXCL_SRULE]; /* the SGML-style exluded
						     element types */
	ThotBool   SrRefImportedDoc;/* inclusion link of an external document*/
	RConstruct SrConstruct;
	union
	{
	  struct			/* SrConstruct = CsNatureSchema */
	  {
	    PtrSSchema _SrSSchemaNat_;	/* structure schema of the nature */
	    char      *_SrOrigNat_;	/* (translated) name of the nature */
	  } s0;
	  struct			/* SrConstruct = CsBasicElement */
	  {
	    BasicType  _SrBasicType_;
	  } s1;
	  struct			/* SrConstruct = CsReference */
	  {
	    /* number of the rule defining the referenced element type */ 
	    int        _SrReferredType_;
	    /* structure schema where the referenced element type is defined,
	       null byte if same schema */
	    Name       _SrRefTypeNat_;	
	  } s2;
	  struct			/* SrConstruct = CsIdentity */
	  {
	    int        _SrIdentRule_;   /* number of the rule defining the
					   right-hand symbol */
	  } s3;
	  struct			/* SrConstruct = CsList */
	  {
	    int        _SrListItem_;    /* number of the rule defining the list
					   elements */
	    int        _SrMinItems_;    /* min number of elements */
	    int        _SrMaxItems_;    /* max number of elements */
	  } s4;
	  struct			/* SrConstruct = CsChoice */
	  {
	    int        _SrNChoices_;    /* -1..MAX_OPTION_CASE, number of
					   choice elements, 0 if any unit
					   (UNIT), -1 if any nature (NATURE) */
	    int	       _SrChoice_[MAX_OPTION_CASE];  /* numbers of rules
					   defining each choice element */
	  } s5;
	  struct			/* SrConstruct = CsAggregate |
					   CsUnorderedAggregate */
	  {
	    int	       _SrNComponents_; /* 0..MAX_COMP_AGG, number of
					   components in the aggregate */
	    int	       _SrComponent_[MAX_COMP_AGG]; /* numbers of the rules
					   defining each component */
	    ThotBool   _SrOptComponent_[MAX_COMP_AGG]; /* table of optional
					   components */
	  } s6;
	  struct			/* SrConstruct = CsConstant */
	  {
	    int        _SrIndexConst_;	/* 1..MAX_LEN_ALL_CONST, index in
					   SsConstBuffer of the beginning of
					   the constant string */
	  } s7;
	  struct			/* SrConstruct = CsPairedElement */
	  {
	    ThotBool   _SrFirstOfPair_; /* begin or end mark */
	  } s8;
	} u;
} SRule;

#define SrSSchemaNat u.s0._SrSSchemaNat_
#define SrOrigNat u.s0._SrOrigNat_
#define SrBasicType u.s1._SrBasicType_
#define SrReferredType u.s2._SrReferredType_
#define SrRefTypeNat u.s2._SrRefTypeNat_
#define SrIdentRule u.s3._SrIdentRule_
#define SrListItem u.s4._SrListItem_
#define SrMinItems u.s4._SrMinItems_
#define SrMaxItems u.s4._SrMaxItems_
#define SrNChoices u.s5._SrNChoices_
#define SrChoice u.s5._SrChoice_
#define SrNComponents u.s6._SrNComponents_
#define SrComponent u.s6._SrComponent_
#define SrOptComponent u.s6._SrOptComponent_
#define SrIndexConst u.s7._SrIndexConst_
#define SrFirstOfPair u.s8._SrFirstOfPair_

typedef SRule *PtrSRule;

typedef struct _SrRuleTable
{
        PtrSRule SrElem[1];
} SrRuleTable;

typedef struct _ExtensBlock *PtrExtensBlock;

/* block containing all the extension rules (EXTENS section) of an
   extension schema */
typedef struct _ExtensBlock
{
    PtrExtensBlock     EbNextBlock;	
    SRule	       EbExtensRule[MAX_EXTENS_SSCHEMA];
} ExtensBlock;

/*
  A structure schema (for a document or a nature) in core memory.

  All the elements are defined in the rule table SsRule. In this
  table rules are grouped by category. The first rules define the
  basic types, then the root rule defines the root
  element of the structure, then all the rules defining the composed
  elements.
  Attributes are stored in the array SsAttribute.
 */

typedef struct _StructSchema
{
   PtrSSchema       SsNextExtens;        /* Pointer to the next schema
					    extension */
   PtrSSchema       SsPrevExtens;        /* Pointer to the previous schema
					    extension or NULL if no extens. */
   char            *SsName;              /* generic structure name */
   char            *SsUriName;           /* Pointer to the URI associated with
					    this schema */
   ThotBool         SsIsXml;             /* Indicates a generix XML schema */
   int              SsCode;              /* code to identify the version */
   char            *SsDefaultPSchema;    /* name of the default presentation 
					    schema associated with this
					    structure */
   PtrEventsSet     SsActionList;        /* Pointer to the list of actions that
                                            can be applied in documents with
                                            this schema. */
   ThotBool         SsExtension;         /* False if document or nature schema,
					    True if schema extension */
   int              SsNExtensRules;      /* number of extension rules if it is
					    a schema extension */
   PtrExtensBlock   SsExtensBlock;       /* Extension rules block if it is a
					    schema extension */
   int              SsDocument;          /* number of the document rule */
   int              SsRootElem;          /* number of the root rule */
   int              SsNAttributes;       /* number of attributes in the
					    schema */
   int              SsAttrTableSize;     /* actual size of table SsAttribute */
   int              SsNRules;            /* current number of rules defining
					    the elements, including the nature
					    rules added dynamically */
   int              SsRuleTableSize;     /* actual size of table SsRule */
   ThotBool         SsExport;            /* this schema exports elements */
   int              SsNExceptions;       /* number of entries in SsException */
                                         /* All the exception numbers
					    associated with the element types
					    and the attributes */
   int              SsException[MAX_EXCEPT_SSCHEMA]; /* buffer for the text of
							the constants */
   char             SsConstBuffer[MAX_LEN_ALL_CONST];/* number of the rule
					    defining the first dynamically
					    loaded nature */
   int              SsFirstDynNature;

   TtAttrTable      *SsAttribute;        /* attributes for this schema */

   SrRuleTable      *SsRule;             /* structure rules defining the
					    elements for this schema */
} StructSchema;

#endif /* _THOTLIB_TYPESTR_H_ */


