/*
 *
 *  (c) COPYRIGHT INRIA, Grif, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 

/*
 * Type declarations for the formatter
 *
 * Authors: V. Quint, I. Vatton (INRIA)
 *
 */

#ifndef _TYPEMEDIA_H
#define _TYPEMEDIA_H

#include "typeint.h"
#include "typecorr.h"
#include "typetra.h"

typedef unsigned char RuleSet[32];

/* List of pointers used */
typedef struct _Box *PtrBox;
typedef Buffer *PtrBuffer;
typedef struct _Line *PtrLine;

/* Type of a described box */
typedef enum
{
  BoComplete, 
  BoSplit, 
  BoPiece, 
  BoBlock, 
  BoPicture, 
  BoGhost,
  BoDotted,
  BoTable,
  BoRow,
  BoColumn
} BoxType;

/* Type of relation between boxes:
   Dep(H, V) -> the relation binds two sibling boxes
   Inc(H, V) -> the box is bound to its parent box
   Ref(H, V) -> the box is bound to a reference axis
   Dim(H, V) -> the box modifies the elastic dimension of the other box */
typedef enum
{
  OpHorizDep, 
  OpVertDep,
  OpHorizInc,
  OpVertInc,
  OpHorizRef,
  OpVertRef,
  OpWidth,
  OpHeight
} OpRelation;

/* Domain of propagation of the modifications between boxes */
typedef enum
{
  ToSiblings,
  ToChildren,
  ToAll
} Propagation;

typedef struct _ViewSelection
{
  PtrLine        VsLine;	/* Line containing the selection */
  PtrBox         VsBox;	        /* Number of the selection mark box */
  PtrTextBuffer  VsBuffer;	/* Selection mark buffer address */
  int            VsIndBox;	/* Selection mark character index */
  int            VsNSpaces;	/* Number of leading spaces */
  int            VsXPos;	/* Selection box shift */
  int 		 VsIndBuf;	/* Index of the character in the buffer */
} ViewSelection;

/* Desription of a displayed line */
typedef struct _Line
{
  PtrLine         LiPrevious;	/* Lines linking */
  PtrLine         LiNext;
  int             LiXOrg;	/* X origin from the block */
  int             LiYOrg;	/* Y origin from the block */
  int	          LiXMax;	/* Maximum X in pixels */
  int	          LiRealLength;	/* Current width */
  int	          LiMinLength;	/* Current compressed width */
  int	          LiHeight;	/* Current height */
  int	          LiHorizRef;	/* Current base */
  int	          LiSpaceWidth; /* Size in pixels of the adjusted spaces */
  int	          LiNSpaces;	/* Number of spaces in the line */
  int	          LiNPixels;	/* Remaining pixels after adjustment */
  PtrBox          LiFirstBox;	/* Index of the first contained box */
  PtrBox          LiFirstPiece;	/* Index if split box */
  PtrBox          LiLastBox;	/* Index of last contained box */
  PtrBox          LiLastPiece;	/* Index if split box */
} Line;

typedef struct _BoxRelation
{
  PtrBox         ReBox;	        /* Index of the depending box */
  BoxEdge        ReRefEdge;	/* Reference of the box */
  OpRelation     ReOp;		/* Applied relation */
} BoxRelation;

typedef struct _PosRelations *PtrPosRelations;
typedef struct _PosRelations
{
  PtrPosRelations  PosRNext;	/* Next block */
  BoxRelation      PosRTable[MAX_RELAT_POS];
} PosRelations;

typedef struct _DimRelations *PtrDimRelations;
typedef struct _DimRelations
{
  PtrDimRelations DimRNext;	/* Next block */
  PtrBox          DimRTable[MAX_RELAT_DIM];
  boolean         DimRSame[MAX_RELAT_DIM]; /* Description of a displayed box */
} DimRelations;

typedef struct _TabRelations *PtrTabRelations;
typedef struct _TabRelations
{
  PtrTabRelations  TaRNext;	/* Next block */
  PtrAbstractBox   TaRTable[MAX_RELAT_DIM];
  int              TaRTWidths[MAX_RELAT_DIM];
  int              TaRTPercents[MAX_RELAT_DIM];
} TabRelations;

typedef struct _Box
{
  PtrAbstractBox  BxAbstractBox;/* Pointer on the associated abstract box */
  PtrBox          BxPrevious;	        /* Previous displayable box */
  PtrBox          BxNext;	        /* Next displayable box */
  PtrBox          BxNextBackground;	/* Next background box to display */
  int	          BxNChars;	/* Total number of characters in the box */
  int             BxIndChar;	/* 0 or position of the split box */
  int             BxXOrg;	/* X origin from the root */
  int             BxYOrg;	/* Y origin from the root */
  int             BxHeight;	        /* Current height */
  int             BxWidth;	        /* Width including spaces */
  int             BxHorizRef;	        /* Current base */
  int             BxVertRef;	        /* Current vertical reference */
  int             BxEndOfBloc;	        /* Fill length:
					   >0 if last box of a line block
					   AlignLeftDots. */
  int		  BxUnderline;	        /* Underlining type */
  int             BxThickness;	        /* Line thickness */
  PtrPosRelations BxPosRelations;	/* Dependencies on positions */
  PtrDimRelations BxWidthRelations;	/* Dependencies in width */
  PtrDimRelations BxHeightRelations;	/* Dependencies in height */
  ptrfont         BxFont;	        /* Font bound to the box */
  PtrBox          BxMoved;	        /* Linking of moved boxes */
  PtrBox          BxHorizInc;	        /* Box linking to the enclosing one */
  PtrBox          BxVertInc;	        /* Box linking to the enclosing one */
  BoxEdge         BxHorizEdge;		/* Position point fixed in X */
  BoxEdge         BxVertEdge;		/* Position point fixed in Y */
  boolean         BxXOutOfStruct;	/* Out of structure horizontal pos. */
  boolean         BxYOutOfStruct;	/* Out of structure vertical pos. */
  boolean         BxWOutOfStruct;	/* Out of structure width */
  boolean         BxHOutOfStruct;	/* Out of structure height */
  boolean         BxHorizInverted;	/* Inverted horizontal marks */
  boolean         BxVertInverted;	/* Inverted vertical marks */
  boolean         BxHorizFlex;          /* The box is elastic */
  boolean         BxVertFlex;           /* The box is elastic */
  boolean         BxXToCompute;         /* Box being placed in X */
  boolean         BxYToCompute;         /* Box being placed in Y */
  boolean	  BxNew;	        /* Newly displayed box */
  boolean	  BxContentHeight;	/* Real height is the content height */
  boolean	  BxContentWidth;	/* Real width is the content width */
  int		  BxRuleHeigth;         /* Content height or minimum */
  int		  BxRuleWidth;	        /* Content width or minimum */
  PtrTextBuffer   BxBuffer;	        /* Pointer on the buffer list */
  BoxType         BxType;
  union
  {
    struct /* BoPiece */
    {
      PtrBox     _BxNexChild_;	/* Next split box */
      int	 _BxNSpaces_;	/* Number of spaces in the text */
      int	 _BxNPixels_;	/* Number of pixels to share out */
      int	 _BxSpaceWidth_;/* >0 of the box is justified */
      int	 _BxFirstChar_;	/* First character in buffer */
    } s0;
    struct /* BoPicture */
    {
      int	 *_BxPictInfo_;  /* Image pointer in memory
				    List of control points for a spline
				    + twisting ratio of the control
			            points */
      float 	 _BxXRatio_;
      float 	 _BxYRation_;
    } s1;
    struct /* BoBlock*/
    {
      PtrLine 	 _BxFirstLine_;	/* First line if applicable */
      PtrLine 	 _BxLastLine_;	/* Last line */
      int        _BxMaxWidth_;  /* Width without line wrapping */
      int        _BxMinWidth_;  /* Mininmum width */
      short      _BxCycles_;    /* count reformatting cycles */
      short      _BxPacking_;   /* Packing */
    } s2;
    struct /* BoTable BoColumn BoRow */
    {
      PtrTabRelations 	_BxColumns_;	/* list of columns or table box */
      PtrTabRelations	_BxRows_;	/* list of rows within a table */
      int        	_BxMaxWidth_;   /* Width without line wrapping */
      int        	_BxMinWidth_;   /* Mininmum width */
      short             _BxCycles_;     /* count reformatting cycles */
      short             _BxPacking_;     /* Packing */
    } s3;
  } u;
} Box;

#define BxNexChild u.s0._BxNexChild_
#define BxNSpaces u.s0._BxNSpaces_
#define BxNPixels u.s0._BxNPixels_
#define BxSpaceWidth u.s0._BxSpaceWidth_
#define BxFirstChar u.s0._BxFirstChar_
#define BxPictInfo u.s1._BxPictInfo_
#define BxXRatio u.s1._BxXRatio_
#define BxYRatio u.s1._BxYRation_
#define BxFirstLine u.s2._BxFirstLine_
#define BxLastLine u.s2._BxLastLine_
#define BxMaxWidth u.s2._BxMaxWidth_
#define BxMinWidth u.s2._BxMinWidth_
#define BxCycles u.s2._BxCycles_
#define BxPacking u.s2._BxPacking_
#define BxColumns u.s3._BxColumns_
#define BxTable u.s3._BxColumns_
#define BxRows u.s3._BxRows_

typedef struct C_points_
{
  float lx, ly, rx, ry;
} C_points; /* Storage of the splines's control points */

/* Description of the relative position of two abstract boxes */
typedef struct _AbPosition
{
  PtrAbstractBox         PosAbRef;	/* Pointer of the reference abstract box */
  int             PosDistance;	/* Distance between the two of them */
  BoxEdge         PosEdge;	/* Positioned abstract box mark */
  BoxEdge         PosRefEdge;	/* Reference abstract box mark */
  TypeUnit        PosUnit;	/* Unit used for distance */
  boolean	  PosUserSpecified;	/* User specified distance */
} AbPosition;

typedef struct _AbDimension
{
  boolean	  DimIsPosition;
  union
  {
    struct
    {
      AbPosition  _DimPosition_;
    } s0;
    struct
    {
      PtrAbstractBox _DimAbRef_;	/* Reference abstract box */
      int 	     _DimValue_;	/* Abstract box mandatory dimension
					   -1: content-defined dimension
					    0: null size */
      TypeUnit	    _DimUnit_;		/* Dimension in %, picas, etc. */
      boolean	    _DimSameDimension_;	/* On the same dimension */
      boolean	    _DimUserSpecified_;	/* User-specified dimension */
      boolean	    _DimMinimum_;	/* The given dimension is the minimum 
				   value (only possible if DimAbRf = null and
				   DimValue = -1) */
    } s1;
  } u;
} AbDimension;

#define DimPosition u.s0._DimPosition_
#define DimAbRef u.s1._DimAbRef_
#define DimValue u.s1._DimValue_
#define DimUnit u.s1._DimUnit_
#define DimSameDimension u.s1._DimSameDimension_
#define DimUserSpecified u.s1._DimUserSpecified_
#define DimMinimum u.s1._DimMinimum_

/* Element describing a pending presentation rule */
typedef struct _DelayedPRule *PtrDelayedPRule;
typedef struct _DelayedPRule
{
  PtrPRule        DpPRule;	/* Pointer on the pending rule */
  PtrAbstractBox  DpAbsBox;	/* Pointer on the abstract box on which the
				   rule is to be applied */
  PtrPSchema      DpPSchema;	/* Pointer on the presentation schema owning
				   the rule */
  PtrAttribute    DpAttribute;	/* Pointer on the attribute to which the rule
				   corresponds, nil if not attribute rule */
  PtrDelayedPRule DpNext;	/* Next element */
} DelayedPRule;

/* Description of an abstract images of a document view */
typedef struct _AbstractBox
{
  PtrElement      AbElement;	/* Corresponding element in the I.R. */
  PtrBox          AbBox;	/* Box descriptor index */
  PtrAbstractBox  AbEnclosing;	/* Linking towards the enclosing asbstr. box */
  PtrAbstractBox  AbNext;	/* Linking towards the next sibling abstract box */
  PtrAbstractBox  AbPrevious;	/* Linking towards the previous abstract box */
  PtrAbstractBox  AbFirstEnclosed;    /* Linking towards the first child
					 abstract box */
  PtrAbstractBox  AbNextRepeated;/* Linking towards the next repeated
					   abstract box (laid out??) */
  PtrAbstractBox  AbPreviousRepeated; /* Linking towards the previous
					 repeated abstract box */
  PtrPSchema      AbPSchema;	/* Presentation schema of the abstract box */
  int		  AbDocView;	/* Document view number */
  PtrCopyDescr	  AbCopyDescr;	/* Descriptor of the copied element if the
				   abstract box is produced by a Copy rule */
  PtrAttribute    AbCreatorAttr;/* Pointer on the attribute that created the
				    abstract box, if it is an attribute-created
				    presentation abstract box */
  PtrDelayedPRule AbDelayedPRule;       /* Used by the editor */
  int             AbVolume;	/* Equivalent characters number */
  AbPosition      AbVertRef;	/* Vertical reference mark position */
  AbPosition      AbHorizRef;	/* Horizontal reference mark position */
  AbPosition      AbVertPos;	/* Vertical position in the box */
  AbPosition      AbHorizPos;	/* Horizontal position in the box */
  AbDimension     AbHeight;	/* Box height */
  AbDimension     AbWidth;	/* Box width */
  int		  AbUnderline;	/* Underlining type */
  int		  AbThickness;	/* Underlining thickness */
  int             AbIndent;	/* Indentation for line breaking */
  TypeUnit        AbIndentUnit; /* Indentation unit */
  int             AbDepth;	/* Abstract box display plane */
  int	          AbTypeNum;	/* Type number/presentation box */
  int	          AbNum;	/* Abstract box number for debug */
  int	          AbVisibility; /* Abstract box visibility degree */
  char            AbFont;	/* Characteristics of the font used */
  int		  AbHighlight;	/* Highlighting of the abstract box */
  int		  AbSize;	/* Character logical/real size */
  TypeUnit        AbSizeUnit;   /* Unit for the size */
  char		  AbLineStyle;  /* Line style */
  int             AbLineWeight; /* Line thickness */
  TypeUnit	  AbLineWeightUnit; 	/* Unit of thickness */
  int		  AbFillPattern;/* Fill pattern */	
  int		  AbBackground; /* Background color */
  int		  AbForeground; /* Drawing color */
  int             AbLineSpacing;        /* Interlining */
  TypeUnit        AbLineSpacingUnit;    /* Interlining unit */
  BAlignment      AbAdjust;	/* BAlignment of lines in the abstract box */
  boolean         AbJustify;	/* Lines are justified */
  boolean         AbAcceptLineBreak;    /* Can be split in lines */
  boolean	  AbAcceptPageBreak;    /* Can be split by page breaks */
  boolean	  AbHyphenate;	/* Contents can be hyphenated */
  boolean         AbOnPageBreak;	/* The box crosses the page limit */
  boolean         AbAfterPageBreak;	/* Abstract box beyond the page limit*/
  boolean	  AbNotInLine;	/* The abstract box is not part of the line */
  boolean         AbHorizEnclosing;	/* True: horizontally enclosed box */
  boolean         AbVertEnclosing;	/* True: vertically enclosed box */
  boolean         AbCanBeModified;	/* Modification is allowed */
  boolean         AbSelected;		/* The abstract box is selected */
  boolean         AbPresentationBox;	/* A presentation box */
  boolean	  AbRepeatedPresBox;	/* A repeated presentation box */
  boolean         AbSensitive;	/* True: the abstract box is active */
  boolean	  AbReadOnly;	/* Abstract box is read only */
  /* Indications of modification for the abstract box */
  boolean         AbNew;	/* Newly created abstract box */
  boolean         AbDead;       /* Abstract box to be destroyed */
  boolean         AbWidthChange;	/* Change the horizontal dimension */
  boolean         AbHeightChange;	/* Change the vertical dimension */
  boolean         AbHorizPosChange;	/* Change the horizontal position */
  boolean         AbVertPosChange;	/* Change the vertical axis */
  boolean         AbHorizRefChange;	/* Change the horizontal axis */
  boolean         AbVertRefChange;	/* Change the vertical positioning */
  boolean         AbSizeChange;	        /* Change the character size */
  boolean         AbAspectChange;       /* Change the graphical aspect: plane,
					   color, pattern, line style */
  boolean         AbChange;	/* Change of another type */
  LeafType        AbLeafType;
  union
  {
    struct /* AbLeafType = LtCompound */
    {
      int       *_AbPictBackground_;    /* Picture in background */
      boolean    _AbFillBox_;           /* True: a fill box is displayed */
      boolean	 _AbInLine_;
      /* The following two fields only make sense if AbInLine = False */
      boolean	 _AbTruncatedHead_; /* Beginning of box contents is missing */
      boolean	 _AbTruncatedTail_; /* End of box contents is missing */
    } s0;
    struct /* AbLeafType = LtText */
    {
      PtrTextBuffer  _AbText_;      /* On first text buffer */
      Language  _AbLanguage_;  /* Language used */
    } s1;
    struct /* AbLeafType = LtGraphics or LtSymbol */
    {
      char       _AbShape_;         /* Drawing code */
      char       _AbGraphAlphabet_; /* Alphabet used*/
      char       _AbRealShape_;     /* Effective drawing code */
    } s2;
    struct /* AbLeafType = LtPicture */
    {
      int *_AbPictInfo_;
    } s3;
    struct /* AbLeafType = LtPolyline  */
    {
      PtrTextBuffer _AbPolyLineBuffer_; /* First buffer */
      char          _AbPolyLineShape_;  /* Drawing type */
    } s4;
  } u;
} AbstractBox;

#define AbPictBackground u.s0._AbPictBackground_
#define AbFillBox u.s0._AbFillBox_
#define AbInLine u.s0._AbInLine_
#define AbTruncatedHead u.s0._AbTruncatedHead_
#define AbTruncatedTail u.s0._AbTruncatedTail_
#define AbText u.s1._AbText_
#define AbLanguage u.s1._AbLanguage_
#define AbShape u.s2._AbShape_
#define AbGraphAlphabet u.s2._AbGraphAlphabet_
#define AbRealShape u.s2._AbRealShape_
#define AbPictInfo u.s3._AbPictInfo_
#define AbPolyLineBuffer u.s4._AbPolyLineBuffer_
#define AbPolyLineShape u.s4._AbPolyLineShape_

typedef struct _ViewFrame
{
  PtrAbstractBox         FrAbstractBox;	/* Pointer on the root abstract box of the View */
  int             FrXOrg;	/* X origin from root */
  int             FrYOrg;       /* Y origin from root */
  int             FrVolume;	/* Character volume in the window */
  int             FrClipXBegin;	/* X beginning of the display rectangle */
  int             FrClipXEnd;	/* X ending of the display rectangle */
  int             FrClipYBegin;	/* Y beginning of the display rectangle */
  int             FrClipYEnd;	/* Y ending of the display rectangle */
  boolean         FrReady;	/* The window can be displayed */
  boolean         FrSelectOneBox;	/* Only one box is selected */
  boolean         FrSelectShown;	/* The selection is shown */
  boolean	  FrSelectOnePosition;  /* The selection is limited */
  ViewSelection   FrSelectionBegin;	/* Beginning mark of the selection */
  ViewSelection   FrSelectionEnd;	/* End mark of the selection */
  int             FrVisibility;		/* Window visibility threshold */
  int             FrMagnification;	/* Window zoom factor */
} ViewFrame;

typedef struct _VueDeDoc
{
  int             VdView;
  int             VdAssocNum;
  Name            VdViewName;
  PtrSSchema      VdSSchema;
  boolean         VdOpen;
  boolean         VdAssoc;
  boolean	  VdExist;
  boolean         VdNature;
  boolean         VdPaginated;
} DocumentView;

typedef DocumentView AvailableView[MAX_VIEW_OPEN];

/* Declaration of a research domain context */
typedef struct _SearchContext
{
  PtrDocument	SDocument;	/* Document where the search is on */
  int		STree;		/* Number of the current tree for a
				   document-wide search */
  PtrElement	SStartElement;	/* Research starting element */
  int		SStartChar;	/* Research starting character */
  PtrElement	SEndElement;	/* Research ending element */
  int           SEndChar;	/* Research ending character */
  boolean	SStartToEnd;	/* Forward search */
  boolean	SWholeDocument;	/* Document-wide search */
} SearchContext;
typedef SearchContext *PtrSearchContext;

/* Declaration for dialogue */

/* List of the pointers used */
typedef char    FontName[30];
typedef char    ButtonLabel[10];
typedef int     CatIndex[MAX_ENTRY];
typedef char    CatalogText[MAX_TXT_LEN];

#endif
