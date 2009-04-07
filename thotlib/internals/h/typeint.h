/*
 *
 *  (c) COPYRIGHT INRIA 1996-2009
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

#include "constmenu.h"
#include "constbutton.h"

/* document external identifier: document name for the user */
typedef char ADocumentName[MAX_NAME_LENGTH];

/* pointers */
typedef struct _ElementDescr *PtrElement;
typedef struct _ReferredElemDescriptor *PtrReferredDescr;
typedef struct _ReferenceDescriptor *PtrReference;
typedef struct _AttributeBlock *PtrAttribute;
/* Pointer on an image abstract box */
typedef struct _AbstractBox *PtrAbstractBox;
typedef struct _CopyDescriptor *PtrCopyDescr;

/* the reference types */
typedef enum
{ 
  RefFollow, 
  RefInclusion
} ReferenceType;


/* Descriptor representing an element copied by a Copy presentation rule
   applied to a referenced element */
typedef struct _CopyDescriptor
{
  PtrAbstractBox  CdCopiedAb;	/* the abstract box that is a copy */
  PtrElement	  CdCopiedElem;	/* the copied element */
  PtrPRule	  CdCopyRule;	/* the Copy presentation rule */
  PtrCopyDescr	  CdPrevious;	/* previous copy descriptor for the same elem*/
  PtrCopyDescr	  CdNext;	/* next copy descriptor for the same element */
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
  LabelString	   ReReferredLabel;	/* label of the referenced element,
					   useful only if ReExternalRef */
  PtrElement       ReReferredElem;	/* the element that is referenced */
} ReferredElemDescriptor;

/* a reference attached to a reference element or a reference attribute */
typedef struct _ReferenceDescriptor
{
  PtrReference	    RdNext;	   /* next reference to the same element */
  PtrReference      RdPrevious;	   /* previous reference to the same element*/ 
  PtrReferredDescr  RdReferred;	   /* descriptor of the referenced element */
  PtrElement	    RdElement;	   /* the referencing element, even if it
				      is a reference by attribute */
  PtrAttribute	    RdAttribute;   /* corresponding attribute or NULL if
				      not a reference by attribute */
  ReferenceType     RdTypeRef;	   /* reference type */
} ReferenceDescriptor;

typedef struct _HandlePSchema *PtrHandlePSchema;

/* a handle for a presentation schema extension (aka style sheet) */
/* this is used to link a sequence of presentation schema extensions
   (aka style sheets) to the main pres. schema they extend */
typedef struct _HandlePSchema
{
  PtrPSchema	       HdPSchema;    /* pointer to the presentation schema
				        extension */
  PtrHandlePSchema   HdNextPSchema;  /* handle of the next presentation
					schema extension */     
  PtrHandlePSchema   HdPrevPSchema;  /* handle of the previous presentation
					schema extension */
} HandlePSchema;

typedef struct _DocSchemasDescr *PtrDocSchemasDescr;

/* a block describing a structure schemas used by a document, as well as the
   main presentation schema to be used for this structure schemas in this
   document. The presentation schema extensions are also linked from there */
typedef struct _DocSchemasDescr
{
  PtrDocSchemasDescr PfNext;          /* next block for the same document */
  PtrSSchema         PfSSchema;       /* the structure schema of interest */
  PtrPSchema         PfPSchema;       /* the presentation schema associated
					 with this structure schema */
  PtrHandlePSchema   PfFirstPSchemaExtens; /* first extension for this
					      presentation schema */
} DocSchemasDescr;

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
        intptr_t        _AeAttrValue_;	   /* attribute value or value number*/
      } s0;
      struct	  /* AeAttrType = AtReferenceAttr */
      {
	PtrReference	_AeAttrReference_; /* reference to the element
					      referenced by the attribute */
      } s1;
      struct	  /* AeAttrType = AtTextAttr */
      {
	PtrTextBuffer	_AeAttrText_;	   /* textual value of the attribute */
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
  LtPolyLine,
  LtPath
} LeafType;

typedef CHAR_T Buffer[THOT_MAX_CHAR];
/* a control point in a polyline (polygon or spline) */
typedef struct _PolyLinePoint
{
  int		XCoord;	    /* coordinates from the box origin */
  int		YCoord;	    /* expressed in millipoint */
}PolyLinePoint;
#define MAX_POINT_POLY	THOT_MAX_CHAR / sizeof(PolyLinePoint)

/* Description of a text buffer */
typedef struct _TextBuffer
{
  PtrTextBuffer	BuNext;		/* Next buffer */
  PtrTextBuffer	BuPrevious;	/* Previous buffer */
  int 		BuLength;	/* actual length (number of characters or
				   number of polyline points */
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

typedef char            PathBuffer[MAX_PATH];

/* type of a SVG path segment */
typedef enum
{
  PtLine,
  PtCubicBezier,
  PtQuadraticBezier,
  PtEllipticalArc
} PathSegType;

typedef struct _PathSeg *PtrPathSeg;


/* descriptor of the selection to do after redosplaying */
typedef struct _SelectionDescriptor
{
  Element             SDElemSel;
  Element             SDElemExt;
  Attribute           SDAttribute;
  int                 SDFirstChar;
  int                 SDLastChar;
  int                 SDCharExt;
  ThotBool            SDSelActive;
}
SelectionDescriptor;

/* Description of a SVG path segment */
typedef struct _PathSeg
{
  PtrPathSeg      PaNext;	/* Next segment in the same path */
  PtrPathSeg      PaPrevious;   /* Previous segment in the same path */
  PathSegType     PaShape;      /* Shape of that segment */
  ThotBool        PaNewSubpath; /* This segment starts a new subpath */
  /* all coordinates are expressed in millipoint from the box origin */
  int	        XStart;	        /* coordinates of start point */
  int	        YStart;
  int             XEnd;         /* coordinates of end point */
  int             YEnd;
  union
  {
    struct
    {   /* PaShape = PtCubicBezier or PtQuadraticBezier */
      /* a quadratic Bezier curve uses only the first control point */
      int         _XCtrlStart_; /* coordinates of control point at the */
      int         _YCtrlStart_; /* beginning of the curve */
      int         _XCtrlEnd_;   /* coordinates of control point at the */
      int         _YCtrlEnd_;   /* end of the curve */
    } s0;
    struct
    {   /* PaShape = PtEllipticalArc */
      int         _XRadius_;
      int         _YRadius_;
      int         _XAxisRotation_;
      ThotBool    _LargeArc_;
      ThotBool    _Sweep_;      /* draw the arc in a "positive-angle" */
      /* direction */
    } s1;
  } u;
} PathSeg;

#define XCtrlStart u.s0._XCtrlStart_
#define YCtrlStart u.s0._YCtrlStart_
#define XCtrlEnd u.s0._XCtrlEnd_
#define YCtrlEnd u.s0._YCtrlEnd_
#define XRadius u.s1._XRadius_
#define YRadius u.s1._YRadius_
#define XAxisRotation u.s1._XAxisRotation_
#define LargeArc u.s1._LargeArc_
#define Sweep u.s1._Sweep_


#ifdef _GL
/*Structure describing points 
we need double precision here.*/
typedef struct _ThotDblePoint {
  double         x;
  double         y; 
  double         z;
} ThotDblePoint;

typedef struct GL_point 
{
  double x;
  double y;
} ThotPoint;

/*Structure describing points 
resulting of tesselation
(linked list)*/
typedef struct _Mesh {
  double          data[3];
  struct _Mesh   *next;
} Mesh_list;

/*Structure describing a Path*/
typedef struct _ThotPath {
  ThotDblePoint      *npoints;  /* points array*/
  int                *ncontour; /* Countour flag array*/
  Mesh_list          *mesh_list;/*Contains points resulting of tesselation*/
  int                nsize;     /*current number of points*/
  int                maxpoints; /*max size of the npoints array*/
  int                maxcont;   /*max size of the maxcont array*/
  int                cont;      /*current number of countour*/
  int                height;    /*height of path (needed for inversion)*/
} ThotPath;

/* Animation Path defining successive positions */
typedef struct _AnimPath
{
  PtrPathSeg      FirstPathSeg; /*linked list of segment defining the path*/
  float           length;       /* total length */  
  ThotPoint       *Path;        /*The Path*/
  float           *Proportion;  /*per segment % of total length*/
  float           *Tangent_angle;  /*per segment tangent angle for automatic rotating or text path*/
  int             npoints;
  int             maxpoints;
 } AnimPath;

#endif /*_GL */

typedef enum
{
  ArUnknown,
  ArNone,
  ArXMinYMin,
  ArXMidYMin,
  ArXMaxYMin,
  ArXMinYMid,
  ArXMidYMid,
  ArXMaxYMid,
  ArXMinYMax,
  ArXMidYMax,
  ArXMaxYMax
} ViewBoxAspectRatio;

typedef enum
{
  MsUnknown,
  MsMeet,
  MsSlice
} ViewBoxMeetOrSlice;

/* type of a SVG Transform */
typedef enum
{
  PtElScale,
  PtElTranslate,
  PtElAnimTranslate,
  PtElAnimRotate,
  PtElViewBox,
  PtElBoxTranslate,
  PtElRotate,
  PtElMatrix,
  PtElSkewX,
  PtElSkewY
} TransformType;

typedef struct _Transform *PtrTransform;

/* Description of a SVG Transformation */
typedef struct _Transform
{
  PtrTransform      Next;
  TransformType     TransType;
  ThotBool          Enabled;
  union
  {
    struct /* Scale, Translate*/
    {
      float _XS;
      float _YS;
    } s0;
    struct /* SkewX, SkewY */
    {
      float _Factor;
    } s1;
    struct /* Matrix */
    {
      float M[6];
    } s2;
    struct /* Rotate */
    {
      float _Angle;
      float _XA;
      float _YA;
    } s3;
    struct /* Viewbox */
    {
      float VbXTranslate;
      float VbYTranslate;
      float VbWidth;
      float VbHeight;
      ViewBoxAspectRatio VbAspectRatio;
      ViewBoxMeetOrSlice VbMeetOrSlice;
    } s4;
  } u;
} Transform;

#define XScale u.s0._XS
#define YScale u.s0._YS
#define TrFactor u.s1._Factor
#define TrMatrix u.s2.M
#define AMatrix u.s2.M[0]
#define BMatrix u.s2.M[1]
#define CMatrix u.s2.M[2]
#define DMatrix u.s2.M[3]
#define EMatrix u.s2.M[4]
#define FMatrix u.s2.M[5]
#define XRotate u.s3._XA
#define YRotate u.s3._YA
#define TrAngle u.s3._Angle
#define VbXTranslate u.s4.VbXTranslate
#define VbYTranslate u.s4.VbYTranslate
#define VbWidth u.s4.VbWidth
#define VbHeight u.s4.VbHeight
#define VbAspectRatio u.s4.VbAspectRatio
#define VbMeetOrSlice u.s4.VbMeetOrSlice

/* a stop in an SVG gradient */
typedef struct _GradientStop 
{
  unsigned short  r, g, b, a;      /* color and alpha channel */
  float           offset;          /* offset where this color starts */  
  PtrElement      el;              /* reference to the stop element */  
  struct _GradientStop *next;      /* next stop for the same gradient */
} GradientStop;

typedef enum
  {
    Linear,
    Radial
  } GradientType;

/* an SVG gradient */
typedef struct _Gradient
{
  ThotBool             userSpace;     /* units for coordinates are in the user
					 space on use */
  PtrTransform         gradTransform; /* list of transform operations */
  int                  spreadMethod;  /* 1: pad, 2: reflect, 3: repeat */
  PtrElement           el;
  struct _GradientStop *firstStop;
  GradientType         gradType;       /* Linear or Radial */
  union
  {
    struct   /* gradType = Linear */
    {
      float  _gradX1_, _gradX2_, _gradY1_, _gradY2_;
      /* coordinates for the direction of the gradient */
    } s0;
    struct   /* gradType = Radial */
    {
      float  _gradCx_, _gradCy_, _gradFx_, _gradFy_, _gradR_;       
    } s1;
  } u;
} Gradient;

#define gradX1 u.s0._gradX1_
#define gradX2 u.s0._gradX2_
#define gradY1 u.s0._gradY1_
#define gradY2 u.s0._gradY2_
#define gradCx u.s1._gradCx_
#define gradCy u.s1._gradCy_
#define gradFx u.s1._gradFx_
#define gradFy u.s1._gradFy_
#define gradR  u.s1._gradR_

/* Animation structures */
typedef enum
  {
    Set,
    Motion,
    Color,
    Transformation,
    Animate,
    OtherAnim
  } Type_anim;

typedef enum
  {
    Freeze,
    Repeat,
    Otherfill
  } Type_fill;

typedef enum
  {
    Css,
    Xml,
    OtherAttr
  } Type_Attribute;

typedef double AnimTime;

typedef struct _Animated_Element
{
  struct _Animated_Element  *next;      /*to build linked list of animation*/
  AnimTime                  start;      /*...*/
  AnimTime                  duration;   /*...*/
  AnimTime                  action_time;/*Time of last render*/
  void                      *from;      /*Initial Value*/
  void                      *to;        /*final value*/
  Type_anim                 AnimType;   /*Color, Transform....*/
  Type_fill                 Fill;       /*if anim continues forever... or not*/
  Type_Attribute            Attr;       /*class of Attr*/
  char                      *AttrName;  /*permits to select attr ie opacity*/
  ThotBool                  replace;    /*if animation replace precedent animation*/
  void                      *replaced;  /* Initial State before animation*/
  ThotBool                  accum;      /*if animation accumulate precedent animation*/
  int                       repeatCount;
} Animated_Element;

typedef struct _Animated_Cell
{
  PtrElement El;
  struct _Animated_Cell *Next;
} Animated_Cell;

/* Descriptor representing an element that is kept after a Cut or Copy
   operation and is to be Pasted */
typedef struct _PasteElemDescr *PtrPasteElem;
#define MAX_PASTE_LEVEL 16

typedef struct _PasteElemDescr
{
  PtrPasteElem	PePrevious;  /* descriptor of the previous element to be
				pasted */
  PtrPasteElem	PeNext;	     /* descriptor of the next element to be pasted */
  PtrElement	PeElement;   /* the element to be paste */
  int	 	PeElemLevel; /* level in the abstract tree of the original
				element */
  int		PeAscendTypeNum[MAX_PASTE_LEVEL]; /* type number of the former
						     ancestors of the element
						     to be pasted */
  PtrSSchema	PeAscendSSchema[MAX_PASTE_LEVEL]; /* structure schema of the
						     former ancestors */
  PtrElement	PeAscend[MAX_PASTE_LEVEL]; /* former ancestor element*/
} PasteElemDescr;

/* an element of an abstract tree */
typedef struct _ElementDescr
{
  PtrElement		ElParent;	/* Parent in the abstract tree */
  PtrElement		ElPrevious;	/* Previous sibling in the abs. tree */
  PtrElement		ElNext;		/* next sibling in the abs. tree */
  PtrReferredDescr	ElReferredDescr;/* Pointer on the referenced element
					   descriptor if there are references
					   to this element */
  PtrDocSchemasDescr    ElFirstSchDescr;/* first descriptor of all P schema
					   extensions associated with this el*/
  PtrAttribute     	ElFirstAttr;	/* pointer on the element first
					   attribute, NULL if no attribute */
  PtrPRule    	        ElFirstPRule;	/* pointer on the first rule of the
					   specific presentation rule string
					   to be applied to the element */
  PtrAbstractBox	ElAbstractBox[MAX_VIEW_DOC]; /* pointer on the first
					   abstract box corresponding to the
					   element for each view of the doc. */
  PtrSSchema    	ElStructSchema;	/* pointer on the structure schema
					   where the element type is defined */
  int			ElTypeNumber;	/* number of the rule defining the type
					   of the element in the structure
					   schema */
  int		    	ElVolume;    	/* volume (number of characters) of the
					   element subtree */
  PtrCopyDescr    	ElCopyDescr; 	/* beginning of the string of the
					   descriptors of the abstract boxes
					   that copy the element by a Copy
					   presentation rule */
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
  AccessRight	        ElAccess;	/* access rights to the element */
  ThotBool		ElHolophrast;	/* holophrasted element */
  ThotBool		ElTransContent;	/* element contents have been alreay
					   translated */
  ThotBool		ElTransAttr;	/* element attributes have been already
					   translated */
  ThotBool		ElTransPres;	/* element presentation rules have been
					   alreay translated */
  ThotBool 		ElTerminal;	/* the element is a leaf in the
					   tree */
  ThotBool              ElSystemOrigin;/*If the element defines a new
					 coodinates system*/
 
  PtrTransform          ElTransform;    /* the element is transformed */
  void                 *ElAnimation;
  Gradient             *ElGradient;
  PtrElement            ElGradientCopy; /* used when copying trees */
  ThotBool              ElGradientDef;  /* ElGradient is a gradient definition
				  otherwise it is a reference to a definition */
  union
  {
    struct		      /* ElTerminal = False */
    {
      PtrElement _ElFirstChild_;	/* first child element */
    } s0;
    struct		      /* ElTerminal = True */
    {
      LeafType _ElLeafType_;	        /* type of leaf */
      union
      {
	struct		       /* ElLeafType = LtText */
	{
	  PtrTextBuffer _ElText_;  	/* pointer on the buffer
					   containing the
					   beginning of the text */
	  int           _ElTextLength_; /* text length */
	  Language      _ElLanguage_;	/* text language */
	} s0;
	struct		/* ElLeafType = LtGraphics or LtSymbol*/
	{
	  wchar_t       _ElWideChar_;   /* Wide char code if ElGraph = '?' */
	  char          _ElGraph_;	/* code of element */
	} s1;
	struct			/* ElLeafType = LtPageColBreak */
	{
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
	struct			/* ElLeafType = LtPolyLine */
	{
	  PtrTextBuffer _ElPolyLineBuffer_; /* buffer containing the
					       points defining the line*/
	  int		  _ElNPoints_;	     /* number of points */
	  char 	  _ElPolyLineType_;  /* type of line */
	} s5;
	struct                  /* ElLeafType = LtPath */
	{
	  PtrPathSeg    _ElFirstPathSeg_;  /* first segment of the
					      path segments list */
	} s6;
	struct			/* TypeImage = LtPicture */
	{
	  PtrTextBuffer _ElPictureName_;/* pointer on the buffer
					   containing thepicture name*/
	  int           _ElNameLength_; /* picture name length */
	  int           *_ElPictInfo_;  /* info about the picture */
	} s7;
      } u;
    } s1;
  } u;
} ElementDescr;

#define ElFirstChild u.s0._ElFirstChild_
#define ElLeafType u.s1._ElLeafType_
#define ElText u.s1.u.s0._ElText_
#define ElTextLength u.s1.u.s0._ElTextLength_
#define ElLanguage u.s1.u.s0._ElLanguage_
#define ElWideChar u.s1.u.s1._ElWideChar_
#define ElGraph u.s1.u.s1._ElGraph_
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
#define ElFirstPathSeg u.s1.u.s6._ElFirstPathSeg_
#define ElPictureName u.s1.u.s7._ElPictureName_
#define ElNameLength u.s1.u.s7._ElNameLength_
#define ElPictInfo u.s1.u.s7._ElPictInfo_

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
   They are left unused if the parent's constructor is CsIdentity or
   CsConstant. 
   Yet another use is to link the different REFERENCE elements pointing on the
   same element. */

/* identifier of a document view */
typedef int		DocViewNumber;

typedef struct _GuestViewDescr *PtrGuestViewDescr;

/* descriptor of a guest view, i.e. a view (defined in a presentation schema)
   that can be hosted by a document view (see clause MERGE in language P) */
typedef struct _GuestViewDescr
{
  PtrSSchema        GvSSchema;       /* structure schema of the present.
					schema that defines the view */
  int               GvPSchemaView;   /* number of the view in the present
					schema that defines it */
  PtrGuestViewDescr GvNextGuestView; /* next guest view for the same
					document view */
} GuestViewDescr;

/* descriptor of a document view */
typedef struct _DocViewDescr
{
  PtrSSchema	  DvSSchema;    /* structure schema of the presentation
				   schema that defines the view */
  int		  DvPSchemaView;/* number of the view in the
				   presentation schema */
  ThotBool          DvSync;	/* this view must be synchronized with
				   the active view */
  PtrGuestViewDescr DvFirstGuestView; /* list of guest views for that
					 document view */
} DocViewDescr;

/* type of an editing operation recorded in the history */
typedef enum
{
  EtDelimiter,    /* Sequence delimiter */
  EtElement,      /* Operation on elements */
  EtAttribute,    /* Operation on an attribute */
  EtChangeType    /* Change type of an element */
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
					    the last selected element, if it's
					    a character string */
      PtrSSchema    _EoSelectedAttrSch_; /* schema of the selected attribute */
      int           _EoSelectedAttr_;    /* index of the selected attribute */
      ThotBool      _EoColumnSelected_;  /* a table column is selected */
      ThotBool      _EoInitialSequence_; /* the sequence starting with this
                                            delimiter represents the first
                                            editing operation made to the
                                            document since it was loaded */
    } s0;
    struct        /* EoType = EtElement */
    {
      PtrElement    _EoParent_;          /* parent of elements to be inserted
					    to undo the operation */
      PtrElement    _EoPreviousSibling_; /* previous sibling of first element
					    to be inserted to undo the
					    operation */
      PtrElement    _EoCreatedElement_;  /* element to be removed to undo the
					    operation */
      PtrElement    _EoSavedElement_;    /* copy of the element to be inserted
					    to undo the operation */
      int           _EoInfo_;            /* value of the info to be passed to
					    the application with the event
					    ElemPaste.Post when undoing a
					    deletion. Values:
                                            0: not from undo
                                            1: normal undo
                                            2: see BreakElement (structmodif.c)
					    3, 4: undo of command Delete column
					          see CellPasted (HTMLtable.c)
					 */
    } s1;
    struct      /* EoType = EtAttribute */
    {
      PtrElement    _EoElement_;         /* the element to which the attribute
					     belongs */
      PtrAttribute  _EoCreatedAttribute_;/* attribute to be removed to undo
					     the operation */
      PtrAttribute  _EoSavedAttribute_;  /* copy of the attribute to be
					     inserted to undo the operation */
      int           _EoAttrRank_;        /* rank of the attribute in the list
					     of the element's attributes */
    } s2;
    struct        /* EoType = EtChangeType */
    {
      PtrElement    _EoChangedElement_;  /* element to be changed to undo */
      int           _EoElementType_;     /* type to be restored to undo */
    } s3;
  } u;
} EditOperation;

typedef struct _NsPrefixDescr *PtrNsPrefixDescr; 
/* Description of a namespace prefix declaration */
typedef struct _NsPrefixDescr
{
  PtrNsPrefixDescr  NsNextPrefixDecl;/* next prefix for a namespace */
  char             *NsPrefixName;    /* namespace prefix */
  PtrElement        NsPrefixElem;    /* thot element associated with that prefix */
} NsPrefixDescr;


typedef struct _NsUriDescr *PtrNsUriDescr; 
/* Description of a namespace declaration */
typedef struct _NsUriDescr
{
  PtrNsUriDescr    NsNextUriDecl;   /* next namespace declaration */
  char            *NsUriName;       /* namespace uri */
  PtrSSchema       NsUriSSchema;    /* associated Thot schema */
  PtrNsPrefixDescr NsPtrPrefix;     /* first associated prefix */
} NsUriDescr;


#define EoFirstSelectedEl u.s0._EoFirstSelectedEl_
#define EoFirstSelectedChar u.s0._EoFirstSelectedChar_
#define EoLastSelectedEl u.s0._EoLastSelectedEl_
#define EoLastSelectedChar u.s0._EoLastSelectedChar_
#define EoSelectedAttrSch u.s0._EoSelectedAttrSch_
#define EoColumnSelected u.s0._EoColumnSelected_
#define EoInitialSequence u.s0._EoInitialSequence_
#define EoSelectedAttr u.s0._EoSelectedAttr_
#define EoParent u.s1._EoParent_
#define EoPreviousSibling u.s1._EoPreviousSibling_
#define EoCreatedElement u.s1._EoCreatedElement_
#define EoSavedElement u.s1._EoSavedElement_
#define EoInfo u.s1._EoInfo_
#define EoElement u.s2._EoElement_
#define EoCreatedAttribute u.s2._EoCreatedAttribute_
#define EoSavedAttribute u.s2._EoSavedAttribute_
#define EoAttrRank u.s2._EoAttrRank_
#define EoChangedElement u.s3._EoChangedElement_
#define EoElementType u.s3._EoElementType_

typedef struct _DocumentDescr *PtrDocument;

/* a document under is internal representation */
#define MAX_LANGUAGES_DOC 10
#define MAX_NATURES_DOC 20	/* maximum length of the nature table */
typedef struct _DocumentDescr
{
  PtrDocument        DocNext;	  /* to link the free blocks */
  PtrSSchema	       DocSSchema;	  /* main structure schema of the document */
  PtrDocSchemasDescr DocFirstSchDescr; /* first descriptor of all schemas
				     associated with  this document */
  PtrElement         DocDocElement;  /* pointer to the document element */
  PtrReferredDescr   DocReferredEl; /* pointer on the fake descriptor, beginning
				     of the string of descriptors of elements
				     referenced in the document */
  
  DocViewDescr       DocView[MAX_VIEW_DOC]; /* correspondence between the document
				     views and those defined in the present.
				     schemas used in the document */
  PtrAbstractBox     DocViewRootAb[MAX_VIEW_DOC]; /* pointer on the root abstract
				     box of each view of the main tree, in the
				     same order as in DocView */
  PtrElement	       DocViewSubTree[MAX_VIEW_DOC];	/* root of the subtree of the
				     main tree to display in the view, null
				     by default */
  int		             DocViewFrame[MAX_VIEW_DOC];	/* ident. of the window
				     corresponding to the view */
  int		             DocViewVolume[MAX_VIEW_DOC];   /* volume of the view */
  int                DocViewFreeVolume[MAX_VIEW_DOC]; /* free volume in the view*/
  int                DocViewNPages[MAX_VIEW_DOC];  /* number of pages */
  PtrAbstractBox     DocViewModifiedAb[MAX_VIEW_DOC]; /* pointer on the abstract
				     box to redisplay for the view */
  ADocumentName	     DocDName;	  /* document name for the user */
  PathBuffer         DocDirectory;	  /* directory of the document */
  PathBuffer	       DocSchemasPath; /* path of the document schemas */
  ThotBool	         DocNotifyAll;	  /* Thot must indicate to the application the
				     creation of ALL the subtree elements that
				     were created or pasted */
  ThotBool           DocReadOnly;	  /* document is read only */
  ThotBool           DocExportStructure; /* the logical structure contains only
				     the skeleton elements */
  int                DocLabelExpMax; /* maximum value of element labels */
  int		             DocMaxPairIdent;/* higher document pair id. value */
  ThotBool           DocModified;	  /* the document was modified */
  ThotBool           DocUpdated;	  /* any change occurs since the last reset */
  int                DocNTypedChars; /* number of characters typed since the last
				     time the document was saved */
  int		             DocNLanguages;  /* number of languages actually used */
  Language	         DocLanguages[MAX_LANGUAGES_DOC]; /* the languages used in the
						      document */
  /* table of the structure schemas (natures) used in the document */
  int		             DocNNatures;	  /* number of natures */
  char*		           DocNatureName[MAX_NATURES_DOC]; /* names of natures */
  char*		           DocNaturePresName[MAX_NATURES_DOC]; /* names of the
                                     presentationschemas of these natures */
  PtrSSchema	       DocNatureSSchema[MAX_NATURES_DOC];/* structure schemas
						       of these natures */
  /* information used while reading the pivot file */
  int		             DocPivotVersion;/* pivot version number */
  PtrReferredDescr   DocLabels;	  /* external labels */
  unsigned char      DocCheckingMode;/* check document structure against the
				     structure schemas */
  ThotBool	         DocPivotError;  /* a format error has been detected */
  
  /* history of last changes made in the document */
  PtrEditOperation   DocLastEdit;	  /* latest editing operation */
  int		             DocNbEditsInHistory; /* number of editing commands recorded
				     in the history */
  ThotBool	         DocEditSequence;/* indicate whether a sequence of editing
				     operations is open */
  /* queue of latest undone commands */
  PtrEditOperation   DocLastUndone; /* latest editing operation undone */
  int		             DocNbUndone;	  /* number of undone editing commands */
  CHARSET            DocCharset;     /* charset of the document */
  ThotBool	         DocDefaultCharset;/* TRUE if the charset is set by default */
  int		             DocProfile;	  /* profile of the document */
  int		             DocExtraProfile;/* 'extra' profile of the document */
  PtrNsUriDescr      DocNsUriDecl;   /* first namespace declaration */
  
#ifdef _WX
  ThotBool           EnabledMenus[MAX_MENU]; /* Enabled top menus in the menubar  */
  Proc               Call_Text;  /* This is the callback which is called when a url is activated */
#endif /* _WX */

  char              *DocTypeName; /* this is the document type name given by amaya 
              * see amaya/amaya.h:DocumentTypeNames table for a complet enumeration */
  int                DocNbRef; /* Number of reference to the document.
              * Usefull for templates to reference a document which is not displayed. */ 
} DocumentDescr;

#endif /* __TYPE_INT_H__ */
