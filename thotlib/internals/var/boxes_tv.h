/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 
/*
 * Global variables for building Concrete Images
 */

EXPORT ViewFrame 	ViewFrameTable[MAX_FRAME];
EXPORT Propagation	Propagate;/* Fix limits of the updates propagation */
EXPORT boolean 	ReadyToDisplay;
EXPORT boolean 	FrameUpdating;	/* The frame is currently updated */
EXPORT boolean 	TextInserting;		/* Text is currently inserted */
EXPORT boolean	BoxCreating;	/* Boxes are currently created */
EXPORT PtrBox 	PackBoxRoot;	/* Root box to repack */
EXPORT PtrDimRelations DifferedPackBlocks;/* Differed packing */
