/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 


THOT_EXPORT char DefaultDocumentName[MAX_PATH]; /* Name of default document to be loaded*/

/* Documents currently loaded */
THOT_EXPORT PtrDocument 	LoadedDocument[MAX_DOCUMENTS];
THOT_EXPORT DisplayMode	documentDisplayMode[MAX_DOCUMENTS];

THOT_EXPORT PtrDocument	DocAutoSave;	/* document to be saved autoimatically */
THOT_EXPORT int		InputSpace;	/* Spaces imput mode (0 or 1) */
THOT_EXPORT int		ShowSpace;	/* Spaces display mode (0 or 1) */
THOT_EXPORT boolean		ChangeLabel;	/* Labels must be changed */
THOT_EXPORT boolean 	StructSelectionMode;	/* Current selection mode */
