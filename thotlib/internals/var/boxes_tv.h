/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 
/*
 * Global variables for building Concrete Images
 */

THOT_EXPORT ViewFrame 	ViewFrameTable[MAX_FRAME];
THOT_EXPORT Propagation	Propagate;/* Fix limits of the updates propagation */
THOT_EXPORT boolean 	ReadyToDisplay;
THOT_EXPORT boolean 	FrameUpdating;	/* The frame is currently updated */
THOT_EXPORT boolean 	TextInserting;		/* Text is currently inserted */
THOT_EXPORT boolean	BoxCreating;	/* Boxes are currently created */
THOT_EXPORT PtrBox 	PackBoxRoot;	/* Root box to repack */
THOT_EXPORT PtrDimRelations DifferedPackBlocks;/* Differed packing */
