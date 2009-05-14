/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2009
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 
/*
 * Global variables for building Concrete Images
 */

THOT_EXPORT ViewFrame 	ViewFrameTable[MAX_FRAME];
THOT_EXPORT Propagation	Propagate;/* Fix limits of the updates propagation */
THOT_EXPORT ThotBool 	ReadyToDisplay;
THOT_EXPORT ThotBool 	FrameUpdating;	/* The frame is currently updated */
THOT_EXPORT ThotBool 	TextInserting;	/* Text is currently inserted */
THOT_EXPORT ThotBool	BoxCreating;	/* Boxes are currently created */
THOT_EXPORT PtrBox 	PackBoxRoot;	/* Root box to repack */
THOT_EXPORT PtrDimRelations DifferedPackBlocks;/* Differed packing */
THOT_EXPORT ThotBool    PackRows;       /* Pack ROW elements when it's TRUE */
THOT_EXPORT ThotBool    AnyWidthUpdate;
THOT_EXPORT ThotBool    SVGCreating; /* TRUE during the creation of a SVG shape */


/* paragraphe to be reformatted after insertion */
THOT_EXPORT PtrAbstractBox LastInsertParagraph;
THOT_EXPORT PtrAbstractBox LastInsertCell;

/* text element where the last insertion is done */
THOT_EXPORT PtrElement   LastInsertElText;
THOT_EXPORT PtrElement   LastInsertElement;
THOT_EXPORT int          LastInsertThotWindow;

/* attribute for which a presentation abstract box has been modified */
THOT_EXPORT PtrAttribute LastInsertAttr;
THOT_EXPORT PtrElement   LastInsertAttrElem;
