/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2009
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
#define MAX_INT_ATTR_VAL 32000	/* maximum value of a numerical attribute */
#define MAX_OPTION_CASE 40	/* maximum number of options in a choice */
#define MAX_COMP_AGG 28		/* maximum number of components in an
				   aggregate */
#define MAX_DEFAULT_ATTR 10	/* maximum number of default attributes in a
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
#define ExcIsBreak 37
#define ExcReturnCreateNL 38
#define ExcReturnCreateWithin 39
#define ExcNoBreakByReturn 40
#define ExcNoReplicate 41
#define ExcIsPlaceholder 42
#define ExcStartCounter 43
#define ExcSetCounter 44
#define ExcCanCut 45
#define ExcMarkupPreserve 46
#define ExcNotAnElementNode 47
#define ExcCheckAllChars 48
#define ExcListItemBreak 49

/* SVG exceptions */
#define ExcIsMarker 309
#define ExcSelectParent 310
#define ExcHighlightChildren 311
#define ExcExtendedSelection 312
#define ExcClickableSurface 313
#define ExcIsDraw 314
#define ExcMoveResize 315
#define ExcNoShowBox 316
#define ExcEmptyGraphic 317
#define ExcIsImg 318
#define ExcIsMap 319

/* Table exceptions */
#define ExcIsTable 320
#define ExcIsRow 321
#define ExcIsColHead 322
#define ExcIsCell 323
#define ExcNewPercentWidth 324
#define ExcColRef 325
#define ExcColSpan 326
#define ExcRowSpan 327
#define ExcShadow 328
#define ExcEventAttr 329
#define ExcIsCaption 330
#define ExcPseudoParag 331
#define ExcColColRef 332

/* CSS exceptions */
#define ExcCssBackground 340
#define ExcCssClass 341
#define ExcDuplicateAttr 342
#define ExcCssId 343
#define ExcCssPseudoClass 344
#define ExcSetWindowBackground 345
#define ExcIsGroup 346
#define ExcSpacePreserve 347
#define ExcNewRoot 348
#define ExcUsePaintServer 349
#define ExcUseMarkers 350

/* Template exceptions */
#define ExcGiveName  370
#define ExcGiveTypes 371
#define ExcIsGhost 372

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
#define C_DOCUMENT_CONSTR 'D'
#define C_ANY_CONSTR 'a'
#define C_EMPTY_CONSTR 'e'

/* base type coding */
#define C_CHAR_STRING 'C'
#define C_GRAPHICS 'G'
#define C_SYMBOL 'S'
#define C_PICTURE 'I'
#define C_REFER 'R'
#define C_PAGE_BREAK 'P'
#define C_ANY_TYPE 'A'

#endif /* __CONST_STR_H__ */


