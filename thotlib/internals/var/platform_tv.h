/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 

/* current path for documents */
THOT_EXPORT PathBuffer  DocumentPath;	

/* current path for schemas */
THOT_EXPORT PathBuffer  SchemaPath;

/* directory for binaries of current architecture */
THOT_EXPORT PathBuffer  BinariesDirectory;

/* maximum levels for building TextInserting menu */
THOT_EXPORT int     InsertionLevels;

/* abstract image errors are highlighted */
THOT_EXPORT boolean HighlightBoxErrors;

/* the logical structure of a document must strictly conform ist structure
schema */
THOT_EXPORT boolean	FullStructureChecking;

/* number of typed characters that triggers an automatic save */
THOT_EXPORT int     	CurSaveInterval;

/* abstract boxes must be created for newly created elements */
THOT_EXPORT	boolean	FirstCreation;
