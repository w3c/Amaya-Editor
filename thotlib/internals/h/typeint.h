/*
 *
 *  (c) COPYRIGHT INRIA 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * Type declarations for the internal representation of the documents
 * and their logical structures
 *
 * Author: V Quint (INRIA)
 *
 */

#ifndef __TYPE_INT_H__
#define __TYPE_INT_H__

#include "typestr.h"
#include "typeprs.h"
#include "language.h"

/* document internal identifier: unique identifier for the storage system */
typedef CHAR_T DocumentIdentifier[MAX_DOC_IDENT_LEN];

/* document external identifier: document name for the user */
typedef CHAR_T ADocumentName[MAX_NAME_LENGTH];

/* element access mode */
typedef enum {
	AccessReadOnly,
	AccessReadWrite,
	AccessHidden,
	AccessInherited
} ElemAccessRight;

/* pointers */
typedef struct _ElementDescr *PtrElement;
typedef struct _ReferredElemDescriptor *PtrReferredDescr;
typedef struct _ReferenceDescriptor *PtrReference;
typedef struct _AttributeBlock *PtrAttribute;
typedef struct _ExternalDoc *PtrExternalDoc;
/* Pointer on an image abstract box */
typedef struct _AbstractBox *PtrAbstractBox;
typedef struct _CopyDescriptor *PtrCopyDescr;

/* the reference types */
typedef enum { 
	RefFollow, 
	RefInclusion
} ReferenceType;

/* descriptor of an external document containing one or more references to a
given element */
typedef struct _ExternalDoc
{
	PtrExternalDoc		EdNext;
	DocumentIdentifier	EdDocIdent;
} ExternalDoc;

/* Descriptor representing an element copied by a Copy presentation rule
   applied to a referenced element */
typedef struct _CopyDescriptor
{
	PtrAbstractBox	CdCopiedAb;	/* the abstract box that is a copy */
	PtrElement	CdCopiedElem;	/* the copied element */
	PtrPRule	CdCopyRule;	/* the Copy presentation rule */
	PtrCopyDescr	CdPrevious;	/* previous copy descriptor for the
					   same element */
	PtrCopyDescr	CdNext;		/* next copy descriptor for the same
					   element */
} CopyDescriptor;

/* an element Label: an identifier that is unique in the document to which the
   element belongs */
typedef char	LabelString[MAX_LABEL_LEN];

/* Descriptor of a referenced element and the string of all of its references.
   All the references to a same element constitute a string on the beginning of
   which points ReFirstReference. For each document, all the descriptors
   ReferredElemDescriptor constitute a string linked by means ReNext and
   RePrevious, the anchor being DocReferredEl. */
typedef struct _ReferredElemDescriptor
{
  PtrReferredDescr RePrevious;		/* previous descriptor in the string */
  PtrReferredDescr ReNext;		/* next descriptor in the string */
  PtrReference	   ReFirstReference;	/* The first reference to this elem. */
  PtrExternalDoc   ReExtDocRef;		/* beginning of the string of document
					   descriptors containing references to
					   the element */
  LabelString	   ReReferredLabel;	/* label of the referenced element,
					   useful only if ReExternalRef */
  ThotBool         ReExternalRef;	/* the referenced element is in another
					   document */
  union
  {
    struct	   /* ReExternalRef = False */
    {
      PtrElement   _ReReferredElem_;	/* the element that is referenced */
    } s0;
    struct	   /* ReExternalRef = True */
    {
      DocumentIdentifier  _ReExtDocument_; /* identifier of the document
				containing the referenced element. This element
				is identified within its document by
				ReReferredLabel. */
    } s1;
  } u;
} ReferredElemDescriptor;

#define ReReferredElem u.s0._ReReferredElem_
#define ReExtDocument u.s1._ReExtDocument_

/* a reference attached to a reference element or a reference attribute */
typedef struct _ReferenceDescriptor
{
    PtrReference	RdNext;		/* next reference to the same element*/
    PtrReference    	RdPrevious;	/* previous reference to the same
					   element */ 
    PtrReferredDescr	RdReferred;	/* descriptor of the referenced
					   element */
    PtrElement		RdElement;	/* the referencing element, even if it
					   is a reference by attribute */
    PtrAttribute	RdAttribute;	/* corresponding attribute or NULL if
					   not a reference by attribute */
    ReferenceType       RdTypeRef;	/* reference type */
    ThotBool        	RdInternalRef;	/* the reference and the designated
					   element are in the same document if
					   True, in different documents if
					   false */
    PtrElement		RdPageBreak;	/* the page break element corresponding
				to the page where the element is included,
				NULL if the referenced element is not included
				yet (only when including an associated element
				at the top or the bottom of a page) */
} ReferenceDescriptor;

typedef struct _TextBuffer *PtrTextBuffer;

/* an attribute of an abstract tree element */
typedef struct _AttributeBlock
{
    PtrAttribute  AeNext;	  /* next attribute for the same element */
    PtrSSchema    AeAttrSSchema;  /* structure schema defining the attribute */
    int		  AeAttrNum;	  /* number of the attribute of the element */
    ThotBool      AeDefAttr;	  /* attribute with fixed value or not */
    AttribType    AeAttrType;	  /* type of the attribute */
    union
    {
      struct	  /* AeAttrType = AtNumAttr or AtEnumAttr */
      {
	int		_AeAttrValue_;	/* attribute value or value number */
      } s0;
      struct	  /* AeAttrType = AtReferenceAttr */
      {
	PtrReference	_AeAttrReference_;	/* reference to the element
						referenced by the attribute */
      } s1;
      struct	  /* AeAttrType = AtTextAttr */
      {
	PtrTextBuffer	_AeAttrText_;	/* textual value of the attribute */
      } s2;
    } u;
} AttributeBlock;

#define AeAttrValue u.s0._AeAttrValue_
#define AeAttrReference u.s1._AeAttrReference_
#define AeAttrText u.s2._AeAttrText_

/* origin of a page break */
typedef enum
{
	PgComputed,
	PgBegin,
	PgUser,
	PgRepeat,
	ColBegin,
	ColComputed,
	ColUser,
	ColGroup
} PageType;

/* nature of a leaf element in the internal representation */
typedef enum
{
	LtText,
	LtSymbol,
	LtGraphics,
	LtPicture,
	LtCompound,
	LtPageColBreak,
	LtReference,
	LtPairedElem,
	LtPolyLine
} LeafType;

typedef CHAR_T Buffer[THOT_MAX_CHAR];

/* a control point in a polyline (polygon or spline) */
typedef struct _PolyLinePoint
{
	int		XCoord;	    /* coordinates of from the box origin */
	int		YCoord;	    /* expressed in millipoint */
}PolyLinePoint;

#define MAX_POINT_POLY	THOT_MAX_CHAR / sizeof(PolyLinePoint)

#ifndef AMAYA_H__	
typedef CHAR_T PathBuffer[MAX_PATH];
#endif 

/* Description of a text buffer */
typedef struct _TextBuffer
{
	PtrTextBuffer	BuNext;		/* Next buffer */
	PtrTextBuffer	BuPrevious;	/* Previous buffer */
	int 		BuLength;	/* actual length (number of characters
					   or number of polyline points */
    union
    {
	struct
	{
	    Buffer	_BuContent_;	/* character string */
	} s0;
	struct
	{
	    PolyLinePoint _BuPoints_[MAX_POINT_POLY]; /* control points */
	} s1;
    } u;
} TextBuffer;

#define BuContent u.s0._BuContent_
#define BuPoints u.s1._BuPoints_

/* Descriptor representing an element that is kept after a Cut or Copy
   operation and is to be Pasted */
typedef struct _PasteElemDescr *PtrPasteElem;
#define MAX_PASTE_LEVEL 5

typedef struct _PasteElemDescr
{
	PtrPasteElem	PePrevious;	/* descriptor of the previous element
					   to be pasted */
	PtrPasteElem	PeNext;		/* descriptor of the next element to
					   be pasted */
	PtrElement	PeElement;	/* the element to be paste */
	int	 	PeElemLevel;	/* level in the abstract tree of the
					   original element */
	int		PeAscendTypeNum[MAX_PASTE_LEVEL];  /* type number of
					   the former ancestors of the element
					   to be pasted */
	PtrSSchema	PeAscendSSchema[MAX_PASTE_LEVEL]; /* structure schema
					   of the former ancestors */
	PtrElement	PeAscend[MAX_PASTE_LEVEL]; /* former ancestor element*/
} PasteElemDescr;

/* Document view number */
typedef int		DocViewNumber;

/* An element of an abstract tree */
typedef struct _ElementDescr
{
    PtrElement		ElParent;	/* Parent in the abstract tree */
    PtrElement		ElPrevious;	/* Previous sibling in the abs. tree */
    PtrElement		ElNext;		/* next sibling in the abs. tree */
    PtrReferredDescr	ElReferredDescr;/* Pointer on the referenced element
					   descriptor if there are references
					   to this element */
    PtrAttribute     	ElFirstAttr;	/* pointer on the element first
					   attribute, NULL if no attribute */
    PtrPRule    	ElFirstPRule;	/* pointer on the first rule of the
					   specific presentation rule string
					   to beiedapply to the element */
    PtrTextBuffer  	ElComment;	/* pointer on the buffer containing the
					   beginning of the comment associated
					   with the element */
    PtrAbstractBox	ElAbstractBox[MAX_VIEW_DOC]; /* pointer on the first
					   abstract box corresponding to the
					   element for each view of the doc. */
    PtrSSchema    	ElStructSchema;	/* pointer on the structure schema
					   where te element type is defined */
    int			ElTypeNumber;	/* number of the rule defining the type
					   of the element in the structure
					   schema */
    int		    	ElVolume;    	/* volume (number of characters) of the
					   element subtree */
    PtrCopyDescr    	ElCopyDescr; 	/* beginning of the string of the
					   descriptors of the abstract boxes
					   that copy the element by a Copy
					   presentation rule */
    int			ElAssocNum; 	/* number of the associated tree owning
					   the element, 0 if main tree */
    ThotBool        	ElIsCopy;	/* the element is a copy that cannot
					   be modified  (parameter or copy by
					   inclusion) */
    PtrReference    	ElSource;	/* pointer on the reference block
					   designating the source of which the
					   element is a copy by inclusion,
					   NULL if the element is not a copy
					   by inclusion */
    int			ElLineNb;	/* line number in the source file */
    LabelString		ElLabel;	/* element unique identifier */
    PtrElement		ElCopy;		/* copy of the element made by the
					   latest call to function CopyTree */
    ElemAccessRight	ElAccess;	/* access rights to the element */
    ThotBool		ElHolophrast;	/* holophrasted element */
    ThotBool		ElTransContent;	/* element contents have been alreay
					   translated */
    ThotBool		ElTransAttr;	/* element attributes have been alreay
					   translated */
    ThotBool		ElTransPres;	/* element presentation rules have been
					   alreay translated */
    ThotBool 		ElTerminal;	/* the element is a leaf in the tree */
    union
    {
	struct				/* ElTerminal = False */
	{
	    PtrElement _ElFirstChild_;	/* first child element */
	} s0;
	struct				/* ElTerminal = True */
	{
	    LeafType _ElLeafType_;	/* type of leaf */
	    union
	    {
		struct			/* ElLeafType = LtText */
		{
		    PtrTextBuffer _ElText_;  	/* pointer on the buffer
						   containing the
						   beginning of the text */
		    int           _ElTextLength_;/* text length */
		    Language      _ElLanguage_;	/* text language */
		} s0;
		struct			/*ElLeafType = LtGraphics or LtSymbol*/
		{
		    char          _ElGraph_;	/* code of element */
		} s1;
		struct			/* ElLeafType = LtPageColBreak */
		{
		    ThotBool      _ElAssocHeader_; /* the header associated
						     elements must be created*/
		    ThotBool      _ElPageModified_;/* the page was modified */
		    PageType      _ElPageType_;    /* origin of the page */
		    int           _ElPageNumber_;  /* page number */
		    int           _ElViewPSchema_; /* view number in the
						      presentation schema */
		} s2;
		struct			/* ElLeafType = LtReference */
		{
		    PtrReference  _ElReference_;   /* pointer on the referenced
						      element */
		} s3;
		struct			/* ElLeafType = LtPairedElem */
		{
		    int		  _ElPairIdent_;   /* unique identifier of the
						      pair in the document */
		    PtrElement	  _ElOtherPairedEl_;/* pointer on the other
						     element in the same pair*/
		} s4;
		struct			/* ElLeafType = LtPlyLine */
		{
		    PtrTextBuffer _ElPolyLineBuffer_; /* buffer containing the
						     points defining the line*/
		    int		  _ElNPoints_;	     /* number of points */
		    char 	  _ElPolyLineType_;  /* type of line */
		} s5;
		struct			/* TypeImage = LtPicture */
		{
		    PtrTextBuffer _ElPictureName_;/* pointer on the buffer
						   containing thepicture name*/
		    int           _ElNameLength_; /* picture name length */
		    int           *_ElPictInfo_;  /* info about the picture */
		} s6;
	    } u;
	} s1;
    } u;
} ElementDescr;

#define ElFirstChild u.s0._ElFirstChild_
#define ElLeafType u.s1._ElLeafType_
#define ElText u.s1.u.s0._ElText_
#define ElTextLength u.s1.u.s0._ElTextLength_
#define ElLanguage u.s1.u.s0._ElLanguage_
#define ElGraph u.s1.u.s1._ElGraph_
#define ElAssocHeader u.s1.u.s2._ElAssocHeader_
#define ElPageModified u.s1.u.s2._ElPageModified_
#define ElPageType u.s1.u.s2._ElPageType_
#define ElPageNumber u.s1.u.s2._ElPageNumber_
#define ElViewPSchema u.s1.u.s2._ElViewPSchema_
#define ElReference u.s1.u.s3._ElReference_
#define ElPairIdent u.s1.u.s4._ElPairIdent_
#define ElOtherPairedEl u.s1.u.s4._ElOtherPairedEl_
#define ElPolyLineBuffer u.s1.u.s5._ElPolyLineBuffer_
#define ElNPoints u.s1.u.s5._ElNPoints_
#define ElPolyLineType u.s1.u.s5._ElPolyLineType_
#define ElPictureName u.s1.u.s6._ElPictureName_
#define ElNameLength u.s1.u.s6._ElNameLength_
#define ElPictInfo u.s1.u.s6._ElPictInfo_

/* The pointer ElFirstChild is interpreted according to the constructor of the
   rule defining the element:
     - LIST: first element of the list,
     - AGGREGATE: first component of the aggregate,
     - IDENTITY or SchNATURE: identical element,
     - CONST: constant element,
     - REFERENCE: the element that is referenced.
   The pointers ElPrevious and ElNext are used to link a structured element to
   the preceding and following element in the list or the aggregate it is part
   of, if its parent's constructor is CsList or CsAggregate.
   These pointers are also used to link associated elements of the same type.
   They are left unused if the parent's constructor is CsIdentity or CsConstant. 
   Yet another use is to link the different REFERENCE elements pointing on the
   same element. */

/* a descriptor of a moved or deleted outgoing reference */
typedef struct _OutReference *PtrOutReference;

typedef struct _OutReference
{
  	PtrOutReference	OrNext;	 /* next descriptor */
  	LabelString	OrLabel; /* label of the referenced external element */
	DocumentIdentifier OrDocIdent; /* identity of the document containing
					  this element */
} OutReference;

/* the descriptor of an element referenced from outside and that was deleted
   or moved to another document */
typedef struct _ChangedReferredEl *PtrChangedReferredEl;

typedef struct _ChangedReferredEl
{
	PtrChangedReferredEl	CrNext;	/* next descriptor */
	LabelString	CrOldLabel;	/* element old label */
	LabelString	CrNewLabel;	/* new label, 0 if the element is
					   deleted */
 	DocumentIdentifier	CrOldDocument;	/* document that used to
						   contain the element */
	DocumentIdentifier	CrNewDocument;	/* the document that contains
						   the element now, if the
						   element was moved */
 	PtrExternalDoc	CrReferringDoc;	/* list of the documents referencing
					   the element */
} ChangedReferredEl;

/* descriptor of a .EXT file loaded in memory: list of the elements of a
   document that are referenced by other documents */
typedef struct _EnteringReferences *PtrEnteringReferences;

typedef struct _EnteringReferences
{
	PtrEnteringReferences	ErNext;			/* next descriptor */
	PtrReferredDescr	ErFirstReferredEl;	/* first element
							   referenced by other
						  	   documents */
	DocumentIdentifier	ErDocIdent;		/* the document */
	PathBuffer		ErFileName;		/* complete name of
							   the .EXT file */
} EnteringReferences;

/* descriptor of a .REF file loaded in memory: list of the referenced elements
   that were part of a document and that were deleted or moved to another
   document */
typedef struct _ReferenceChange *PtrReferenceChange;

typedef struct _ReferenceChange
{
	PtrReferenceChange	RcNext;		/* next descriptor */
	PtrChangedReferredEl	RcFirstChange;	/* first changed referenced
						   element */
	DocumentIdentifier	RcDocIdent;	/* the document */
	PathBuffer		RcFileName;	/* complete name of the .REF
						   file */
} ReferenceChange;
	
/* descriptor of a document view */
typedef struct _DocViewDescr
{
	PtrSSchema	DvSSchema;	/* structure schema of the presentation
					   schema that defines the view */
	int		DvPSchemaView;	/* Number of the view in the
					   presentation schema */
	ThotBool        DvSync;		/* this view must be synchronized with
					   the active view */
} DocViewDescr;

/* type of an editing operation recorded in the history */
typedef enum
{
        EtDelimiter,    /* Sequence delimiter */
        EtElement,      /* Operation on elements */
        EtAttribute     /* operation on an attribute */
} EditOpType;
 
typedef struct _EditOperation *PtrEditOperation;
 
/* Description of an editing operation in the history of editing commands */
typedef struct _EditOperation
{
  PtrEditOperation EoNextOp;          /* next operation in the editing
                                         history */
  PtrEditOperation EoPreviousOp;      /* previous operation in the editing
                                         history */
  EditOpType       EoType;            /* type of operation */
union
  {
  struct        /* EoType = EtDelimiter */
     {
     PtrElement    _EoFirstSelectedEl_;  /* first selected element */
     int           _EoFirstSelectedChar_;/* index of first selected character
                                            in the first selected element,
                                            if it's acharacter string */
     PtrElement    _EoLastSelectedEl_;  /* last selected element */
     int           _EoLastSelectedChar_;/* index of last selected character in
                                           the last selected element, if it's a
                                           character string */
     } s0;
  struct        /* EoType = EtElement */
     {
     PtrElement    _EoParent_;          /* parent of elements to be inserted to
                                           undo the operation */
     PtrElement    _EoPreviousSibling_; /* previous sibling of first element to
                                           be inserted to undo the operation */
     PtrElement    _EoCreatedElement_;  /* element to be removed to undo the
                                           operation */
     PtrElement    _EoSavedElement_;    /* copy of the element to be inserted
                                           to undo the operation */
     } s1;
  struct      /* EoType = EtAttribute */
    {
    PtrElement     _EoElement_;         /* the element to which the attribute
                                           belongs */
    PtrAttribute   _EoCreatedAttribute_;/* attribute to be removed to undo the
                                           operation */
    PtrAttribute   _EoSavedAttribute_;  /* copy of the attribute to be inserted
                                           to undo the operation */
    } s2;
  } u;
} EditOperation;

#define EoFirstSelectedEl u.s0._EoFirstSelectedEl_
#define EoFirstSelectedChar u.s0._EoFirstSelectedChar_
#define EoLastSelectedEl u.s0._EoLastSelectedEl_
#define EoLastSelectedChar u.s0._EoLastSelectedChar_
#define EoParent u.s1._EoParent_
#define EoPreviousSibling u.s1._EoPreviousSibling_
#define EoCreatedElement u.s1._EoCreatedElement_
#define EoSavedElement u.s1._EoSavedElement_
#define EoElement u.s2._EoElement_
#define EoCreatedAttribute u.s2._EoCreatedAttribute_
#define EoSavedAttribute u.s2._EoSavedAttribute_

typedef struct _DocumentDescr *PtrDocument;

/* a document under is internal representation */
typedef struct _DocumentDescr
{
	PtrDocument     DocNext;	/* to link the free blocks */
	PtrTextBuffer	DocComment;	/* comment attached to the document */
	PtrSSchema	DocSSchema;	/* structure schema of the document */
	PtrElement      DocRootElement; /* pointer on the root element of the
					   main abstract tree */
	PtrElement      DocAssocRoot[MAX_ASSOC_DOC];	/* pointers on the
					   roots of each associated tree */
	PtrElement	DocAssocSubTree[MAX_ASSOC_DOC]; /* root of the subtree
					   to be displayed in the associated
					   elements view, NULL if the whole
					   tree is displayed */
	int             DocAssocFrame[MAX_ASSOC_DOC];  /* number of the windows
					   of the associated trees */
	int             DocAssocVolume[MAX_ASSOC_DOC]; /* total volume of the
					   windows of the associated trees */
	int             DocAssocFreeVolume[MAX_ASSOC_DOC]; /* free space
					   remaining in the windows of the
					   the associated trees */
	int             DocAssocNPages[MAX_ASSOC_DOC];   /* number of pages in
					   each associated tree */
	PtrAbstractBox  DocAssocModifiedAb[MAX_ASSOC_DOC]; /* pointers on the
					   abstract box to be redisplayed for
					   the associated trees */
	PtrElement      DocParameters[MAX_PARAM_DOC]; /* pointers on the
					   parameters of each type */
	/* In the field DocParameters, the pointers are in the same order as
	   the rules that define the structure of the pointed elements in the
	   array StructSchema.SsRule. */
	PtrReferredDescr DocReferredEl;	/* pointer on the fake descriptor,
					   beginning of the string of
					   descriptors of elements referenced
					   in the document */
	DocViewDescr    DocView[MAX_VIEW_DOC]; /* correspondence between the
					   document views and those defined in
					   the presentation schemas used in
					   the document */
	PtrAbstractBox  DocViewRootAb[MAX_VIEW_DOC]; /* pointer on the root
					   abstract box of each view of the
					   main tree, in the same order as
					   in DocView */
	PtrElement	DocViewSubTree[MAX_VIEW_DOC];	/* root of the subtree
					   of the main tree to display in the
					   view, null by default */
	int		DocViewFrame[MAX_VIEW_DOC];	/* number of the window
					   corresponding to the view */
	int		DocViewVolume[MAX_VIEW_DOC];   /* volume of the view */
	int             DocViewFreeVolume[MAX_VIEW_DOC]; /* free volume of the
							    view */
	int             DocViewNPages[MAX_VIEW_DOC];  /* number of pages */
	PtrAbstractBox  DocViewModifiedAb[MAX_VIEW_DOC]; /* pointer on the
					   abstract box to redisplay for the
					   view */
	ADocumentName	DocDName;	/* document name for the user */
	DocumentIdentifier DocIdent;	/* unique document id for the storage
					   system */
	PathBuffer      DocDirectory;	/* directory of the document */
	PathBuffer	DocSchemasPath;	/* path of the document schemas */
	ThotBool		DocNotifyAll;	/* Thot must indicate to the application the creation
					   of ALL the subtree elements that were created or
					   pasted */
	ThotBool        DocReadOnly;	/* document is read only */
	ThotBool        DocExportStructure; /* the logical structure contains
					   only the skeleton elements */
	int             DocLabelExpMax;	/* maximum value of element labels */
	int		DocMaxPairIdent;/* higher document pair id. value */
	ThotBool        DocModified;	/* the document was modified */
	int		DocBackUpInterval; /* interval between automatic backup */
	int             DocNTypedChars;	/* number of characters typed since the
					   last time the document was saved */
	PtrOutReference	DocNewOutRef;	/* beginning of the list of outgoing
					   references created since the last
					   time the document was saved */
	PtrOutReference	DocDeadOutRef;	/* beginning of the list of outgoing
					   references deleteted since the last
					   time the document was saved */
	PtrChangedReferredEl	DocChangedReferredEl;	/* beginning of the
					   list of elements referenced from
					   outside that were deleted or moved
					   since the last time the document
					   was saved */
#define MAX_LANGUAGES_DOC 10
	int		DocNLanguages;	/* number of languages actually used */
	Language	DocLanguages[MAX_LANGUAGES_DOC]; /* the languages used
					   in the document */
	/* table of the structure schemas (natures) used in the document */
#define MAX_NATURES_DOC 20	/* maximum length of the nature table */
	int		DocNNatures;	/* number of natures */
	Name		DocNatureName[MAX_NATURES_DOC]; /* names of natures */
	Name		DocNaturePresName[MAX_NATURES_DOC]; /* names of the
				presentation schemas of these natures */
	PtrSSchema	DocNatureSSchema[MAX_NATURES_DOC];/* structure schemas
					   of these natures */
	/* information used while reading the pivot file */
	int		DocPivotVersion;/* pivot version number */
        PtrReferredDescr DocLabels;	/* external labels */
	UCHAR_T	DocCheckingMode; /* check document structure against
					    the structure schemas */
	ThotBool		DocPivotError;  /* an format error has been detected */

	/* history of last changes made in the document */
	PtrEditOperation DocLastEdit;	/* latest editing operation */
	int		DocNbEditsInHistory; /* number of editing commands
					     recorded in the history */
	ThotBool		DocEditSequence;/* indicate whether a sequence of
					   editing operations is open */
	/* queue of latest undone commands */
	PtrEditOperation DocLastUndone;	/* latest editing operation undone */
	int		DocNbUndone;	/* number of undone editing commands */

} DocumentDescr;

#endif /* __TYPE_INT_H__ */
