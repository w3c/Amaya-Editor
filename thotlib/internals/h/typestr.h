/*
 *
 *  (c) COPYRIGHT INRIA, Grif, 1996.
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
 
   Whatever the category of an element is (composed, associated,
   parameter), its structure is defined by a rule from the table
   StructSchema.SsRule.
   A rule number is the rank of the rule in the table
     StructSchema.SsRule.
   An attribute number is the rank of an attribute in the table
     StructSchema.SsAttribute.
   An attribute value number is the rank of the value in the table
     AttrEnumValue of the attribute.
 
     */
 
#include "app.h"
 
typedef char    Name[MAX_NAME_LENGTH]; /* a name is terminated by a null byte*/

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
        CsExtensionRule
} RConstruct;
#define MAX_CONSTRUCT   11      /* number of values for RConstruct */
 
/* basic types known by Thot (refer to the constant MAX_BASIC_TYPE) */
typedef enum
{
        CharString,
        GraphicElem,
        Symbol,
        Picture,
        Refer,
        PageBreak,
        UnusedBasicType
} BasicType;
#define MAX_BASIC_TYPE 7                /* number of values for BasicType */
 
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
        Name             AttrName;      /* name of the attribute, may be
                                           translated in the user's language */
        Name             AttrOrigName;  /* real name of the attribute */
        boolean         AttrGlobal;     /* the attribute can apply to all
                                           the elements defined in the schema 
*/
        int       AttrFirstExcept;      /* index in SsException of the first
                                           exception number associated with 
this
                                           attribute, 0 if no exception is
                                           associated */
        int       AttrLastExcept;       /* index in SsException of the last
                                           exception number associated with 
this
                                           attribute */
        AttribType      AttrType;       /* attribute type */
        union
        {
          struct                        /* AttribType = AtReferenceAttr */
          {
            /* number of the rule defining the type of element referenced
               by the attribute */
            int     _AttrTypeRef_;
            /* structure schema where the type of referenced element is
               defined, 0 if same schema */
            Name             _AttrTypeRefNature_;
          } s2;
	  struct			/* AttribType = AtEnumAttr */
	  {
	    /* number of possible values (effective size of the table
	       AttrEnumValue) */
	    int   _AttrNEnumValues_;
	    Name             _AttrEnumValue_[MAX_ATTR_VAL]; /* names of those values 
*/
	  } s3;
	} u;
} TtAttribute;

#define AttrTypeRef u.s2._AttrTypeRef_
#define AttrTypeRefNature u.s2._AttrTypeRefNature_
#define AttrNEnumValues u.s3._AttrNEnumValues_
#define AttrEnumValue u.s3._AttrEnumValue_


typedef struct _StructSchema *PtrSSchema;

/* A rule defining a type in a structure schema */
typedef struct _SRule
{
	Name             SrName;	/* left-hand symbol of the rule =
					   type defined by the rule */
        Name            SrOrigName;      /* real name of the rule */
	int 		SrNDefAttrs; 	/* 0..MAX_DEFAULT_ATTR, number of
					   attributes with a default value */
        /* numbers of default value attributes */
	int  SrDefAttr[MAX_DEFAULT_ATTR];  
        /* default values of these attributes, in the same order */ 
	int             SrDefAttrValue[MAX_DEFAULT_ATTR];  
	boolean		SrDefAttrModif[MAX_DEFAULT_ATTR];
        /* 0..MAX_LOCAL_ATTR, number of attributes that can apply to the type 
*/
	int 		SrNLocalAttrs;
        /* numbers of the attributes that can apply to the type */ 
	int  SrLocalAttr[MAX_LOCAL_ATTR];  
        /* the local attribute of same rank is mandatory */
	boolean		SrRequiredAttr[MAX_LOCAL_ATTR];
	boolean         SrAssocElem;	/* it is an associated element */
	boolean         SrParamElem;	/* it is a parameter */
	boolean         SrUnitElem;	/* it is an exported unit */
	boolean         SrRecursive;	/* recursive rule */
	boolean         SrRecursDone;	/* already applied recursive rule */
	boolean         SrExportedElem;	/* this type of element is exported */
	int     SrExportContent;	/* type of element making up the
					   content if the element is exported */
	Name             SrNatExpContent;	/* structure schema where
					   SrExportContent is defined, byte 0 if
					   same schema */
	int       SrFirstExcept;	/* index in SsException of the first
					   exception number associated with this
					   element type, 0 if none */
	int       SrLastExcept;	        /* index in SsException of the last
					   exception number associated with this
					   element type */
	int		SrNInclusions;	/* number of SGML-style inclusions ? */
	int	SrInclusion[MAX_INCL_EXCL_SRULE]; /* the SGML-style included
						     element types */
	int		SrNExclusions;	/* number of SGML-style exclusions ?*/
        int	SrExclusion[MAX_INCL_EXCL_SRULE]; /* the SGML-style exluded
						     element types */
	boolean         SrRefImportedDoc;	  /* inclusion link of an
						     external document */
	RConstruct    SrConstruct;
	union
	{
	  struct			/* SrConstruct = CsNatureSchema */
	  {
	    	PtrSSchema    _SrSSchemaNat_;	/* structure schema of the
						   nature */
		Name             _SrOrigNat_;	/* (translated) name of the
						   nature */
	  } s0;
	  struct			/* SrConstruct = CsBasicElement */
	  {
		BasicType      _SrBasicType_;
	  } s1;
	  struct			/* SrConstruct = CsReference */
	  {
	        /* number of the rule defining the referenced element type */ 
	    	int     _SrReferredType_;
		/* structure schema where the referenced element type is
		   defined, null byte if same schema */
		Name             _SrRefTypeNat_;	
	  } s2;
	  struct			/* SrConstruct = CsIdentity */
	  {
		int     _SrIdentRule_;  /* number of the rule defining
					   the right-hand symbol */
	  } s3;
	  struct			/* SrConstruct = CsList */
	  {
		int     _SrListItem_;   /* number of the rule defining
					   the list elements */
		int             _SrMinItems_;   /* min number of elements */
		int             _SrMaxItems_;   /* max number of elements */
	  } s4;
	  struct			/* SrConstruct = CsChoice */
	  {
	    	int 		_SrNChoices_;   /* -1..MAX_OPTION_CASE, number
						   of choice elements, 0 if any
						   unit (UNIT) or -1 if any nature
						   (NATURE) */
		int	_SrChoice_[MAX_OPTION_CASE];	/* numbers of rules
							   defining each choice
							   element */
	  } s5;
	  struct			/* SrConstruct = CsAggregate | CsUnorderedAggregate */
	  {
	    	int		_SrNComponents_; /* 0..MAX_COMP_AGG, number of
						    components in the
						    aggregate */
		int	_SrComponent_[MAX_COMP_AGG]; /* numbers of the rules
							defining each
							component */
		boolean         _SrOptComponent_[MAX_COMP_AGG]; /* table of
							optional components */
	  } s6;
	  struct			/* SrConstruct = CsConstant */
	  {
	    	int 		_SrIndexConst_;	/* 1..MAX_LEN_ALL_CONST, index
						   in SsConstBuffer of the
						   beginning of the constant
						   string */
	  } s7;
	  struct			/* SrConstruct = CsPairedElement */
	  {
		boolean		_SrFirstOfPair_; /* begin or end mark */
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

typedef struct _PresentSchema *PtrPSchema;
typedef struct _HandlePSchema *PtrHandlePSchema;

/* a presentation schema chaining element */
typedef struct _HandlePSchema
{
    PtrPSchema		HdPSchema;   /* pointer on the presentation schema */
    PtrHandlePSchema	HdNextPSchema;	/* handle of the next presentation
					   schema */     
    PtrHandlePSchema	HdPrevPSchema;  /* handle of the previous presentation
					   schema */
}       HandlePSchema;

typedef struct _ExtensBlock *PtrExtensBlock;

/* block containing all the extension rules (EXTENS section) of an
   extension schema */
typedef struct _ExtensBlock
{
    PtrExtensBlock	EbNextBlock;	
    SRule		EbExtensRule[MAX_EXTENS_SSCHEMA];
} ExtensBlock;

/*
  A structure schema (for a document or a nature) in memory.

  All the elements, whatever their category is (composed,
  associated, parameters) are defined in the rule table SsRule. In this
  table rules are grouped by category. The first rule defines the root
  element of the structure, then all the rules defining the composed
  elements. Then come the rules defining the associative elements, as well
  as those defining the parameters. The number of the first and last rule
  is available for each of these groups of rules. The schema attributes
  are stored in the array SsAttribute.
 */

typedef struct _StructSchema
{
	PtrSSchema    SsNextExtens;	/* Pointer on the next schema
					   extension */
	PtrSSchema    SsPrevExtens;	/* Pointer on the previous schema
					   extension or nil if none */
	Name             SsName;	/* generic structure name*/
        int             SsCode;	        /* code to identify the version */
	Name             SsDefaultPSchema;	/* name of the default
					   presentation schema associated with
					   this structure */
	PtrPSchema      SsPSchema;	/* pointer on the actual associated
					   presentation schema */
	PtrHandlePSchema SsFirstPSchemaExtens;	/* first addtional presentation
						   schema */
	PtrEventsSet    SsActionList;	/* Pointer to the list of actions */
					/* that can be applied in documents */
					/* with this schema. */
	boolean		SsExtension;	/* False if document or nature schema,
					   True if schema extension */
	int		SsNExtensRules; /* number of extension rules if it is
					   a schema extension */
	PtrExtensBlock	SsExtensBlock;  /* Extension rules block if it is
					   a schema extension */
        int     SsRootElem;	        /* number of the root rule */
	int             SsNObjects;	/* number of existing elements with the
					   root rule type */
	int  SsNAttributes;	        /* number of attributes in the schema */
	int     SsNRules;	        /* current number of rules defining the
					   elements, including the nature rules
					   added dynamically */
	boolean         SsExport;	/* this schema exports elements */
	int             SsNExceptions;	/* number of entries in SsException */
        /* All the exception numbers associated with the element types and
	   the attributes */
	int             SsException[MAX_EXCEPT_SSCHEMA];
        /* buffer for the text of the constants */
	char            SsConstBuffer[MAX_LEN_ALL_CONST];
        /* number of the rule defining the first dynamically loaded nature */
	int     SsFirstDynNature;
        /* attributes for this schema */
	TtAttribute        SsAttribute[MAX_ATTR_SSCHEMA];
        /* structure rules defining the elements */
	SRule           SsRule[MAX_RULES_SSCHEMA + 2]; /* +2 to be sure to have
			      two free rules at the end of the arrays */
} StructSchema;


#endif /* _THOTLIB_TYPESTR_H_ */
