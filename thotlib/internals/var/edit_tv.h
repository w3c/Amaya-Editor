/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 

THOT_EXPORT int         UserErrorCode;

THOT_EXPORT CHAR_T      DefaultDocumentName[MAX_PATH]; /* default document name */
THOT_EXPORT PtrDocument LoadedDocument[MAX_DOCUMENTS]; /* loaded documents */
THOT_EXPORT DisplayMode	documentDisplayMode[MAX_DOCUMENTS];

THOT_EXPORT PtrDocument	DocAutoSave;  /* document to be saved autoimatically */
THOT_EXPORT int         InputSpace;   /* Spaces imput mode (0 or 1) */
THOT_EXPORT int         ShowSpace;    /* Spaces display mode (0 or 1) */
THOT_EXPORT ThotBool	ChangeLabel;  /* Labels must be changed */
THOT_EXPORT ThotBool    Printing;     /* TRUE if it's a printing application */
