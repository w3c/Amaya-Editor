

EXPORT Name     	DefaultDocumentName; /* Name of default document to be loaded*/

/* Documents currently loaded */
EXPORT PtrDocument 	LoadedDocument[MAX_DOCUMENTS];
EXPORT DisplayMode	documentDisplayMode[MAX_DOCUMENTS];

EXPORT PtrDocument	DocAutoSave;	/* document to be saved autoimatically */
EXPORT int		InputSpace;	/* Spaces imput mode (0 or 1) */
EXPORT int		ShowSpace;	/* Spaces display mode (0 or 1) */
EXPORT boolean		ChangeLabel;	/* Labels must be changed */
EXPORT boolean 	StructSelectionMode;	/* Current selection mode */
