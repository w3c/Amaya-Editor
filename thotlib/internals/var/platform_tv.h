
/* current path for documents */
EXPORT PathBuffer  DocumentPath;	

/* current path for schemas */
EXPORT PathBuffer  SchemaPath;

/* directory for binaries of current architecture */
EXPORT PathBuffer  BinariesDirectory;

/* maximum levels for building TextInserting menu */
EXPORT int     InsertionLevels;

/* abstract image errors are highlighted */
EXPORT boolean HighlightBoxErrors;

/* the logical structure of a document must strictly conform ist structure
schema */
EXPORT boolean	FullStructureChecking;

/* number of typed characters that triggers an automatic save */
EXPORT int     	CurSaveInterval;

/* abstract boxes must be created for newly created elements */
EXPORT	boolean	FirstCreation;
