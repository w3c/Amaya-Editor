/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 

/*
 * Constant declarations for structure schemas
 *
 */ 

#ifndef __CONST_STR_H__
#define __CONST_STR_H__

#define MAX_NAME_LENGTH 32	/* maximum name length */	
#define MAX_PARAM_SSCHEMA 10	/* maximum number of parameter definitions in
				   a structure schema */
#define MAX_INT_ATTR_VAL 32000	/* maximum value of a numerical attribute */
#define MAX_OPTION_CASE 32	/* maximum number of options in a choice */
#define MAX_COMP_AGG 28		/* maximum number of components in an
				   aggregate */
#define MAX_DEFAULT_ATTR 10	/* maximum number of default attributes in a
				   structure rule */	
#define MAX_LOCAL_ATTR 25	/* maximum number of local attributes in a
				   structure rule */
#define MAX_MAND_ATTR 10	/* maximum number of mandatory attributes in a
				   structure rule */
#define MAX_ATTR_VAL 20		/* maximum number of values for an enumerated
				   attribute */
#define MAX_LEN_ALL_CONST 512	/* maximum total length of constant strings */
#define MAX_EXCEPT_SSCHEMA 350	/* maximum number of exceptions in a
				   structure schema */	
#define MAX_INCL_EXCL_SRULE 10	/* max number of inclusions/exclusions in a
				   structure rule */	
#define MAX_EXTENS_SSCHEMA 20	/* maximum number of extension rules in a */
				/* structure schema */

#define MAX_RULES_SSCHEMA 200	/* maximum number of rules in a structure
				   schema */	
#define MAX_ATTR_SSCHEMA 200	/* maximum number of attribute definitions in
				   a structure schema */

/* Constants defining exceptions */
#define ExcNoCut 10
#define ExcNoCreate 11
#define ExcNoHMove 12
#define ExcNoVMove 13
#define ExcNoHResize 14
#define ExcNoVResize 15
#define ExcNewWidth 16
#define ExcNewHeight 17
#define ExcNewHPos 18
#define ExcNewVPos 19
#define ExcInvisible 20
#define ExcNoMove 21
#define ExcNoResize 22
#define ExcNoSelect 23
#define ExcGraphCreation 24
#define ExcHidden 25
#define ExcPageBreak 26
#define ExcPageBreakAllowed 27
#define ExcPageBreakPlace 28
#define ExcPageBreakRepetition 29
#define ExcPageBreakRepBefore 30
#define ExcActiveRef 31
#define ExcCreateHolophrasted 32
#define ExcNoPaginate 33
#define ExcImportLine 34
#define ExcImportParagraph 35
#define ExcParagraphBreak 36
#define ExcReturnCreateNL 37

/* Draw exceptions */
#define ExcHighlightChildren 311
#define ExcExtendedSelection 312
#define ExcIsDraw 313
#define ExcMoveResize 314
#define ExcNoShowBox 315

/* Table exceptions */
#define ExcIsTable 316
#define ExcIsRow 317
#define ExcIsColHead 318
#define ExcIsCell 319
#define ExcNewPercentWidth 320
#define ExcColRef 321
#define ExcColSpan 322
#define ExcRowSpan 323
#define ExcShadow 324
#define ExcEventAttr 325

/* CSS exceptions */
#define ExcCssBackground 340
#define ExcCssClass 341
#define ExcCssId 342
#define ExcCssPseudoClass 343
/* SpellCheck exception */
#define ExcNoSpellCheck 400

/*
 * Marks for the coding of structure schemas produced by the S language compiler
 *
 */

/* attribute type coding */
#define C_INT_ATTR 'N'
#define C_TEXT_ATTR 'T'
#define C_REF_ATTR 'R'
#define C_ENUM_ATTR 'E'

/* constructor coding */
#define C_IDENTITY_CONSTR 'I'
#define C_LIST_CONSTR 'L'
#define C_CHOICE_CONSTR 'C'
#define C_AGG_CONSTR 'A'
#define C_UNORD_AGG_CONSTR 'S'
#define C_CONST_CONSTR 'O'
#define C_REF_CONSTR 'R'
#define C_BASIC_TYPE_CONSTR 'B'
#define C_NATURE_CONSTR 'X'
#define C_PAIR_CONSTR 'P'
#define C_EXTENS_CONSTR 'E'

/* base type coding */
#define C_CHAR_STRING 'C'
#define C_GRAPHICS 'G'
#define C_SYMBOL 'S'
#define C_PICTURE 'I'
#define C_REFER 'R'
#define C_PAGE_BREAK 'P'
#define C_UNUSED 'U'

#endif /* __CONST_STR_H__ */
