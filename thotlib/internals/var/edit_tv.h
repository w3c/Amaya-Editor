/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2009
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 

THOT_EXPORT int         UserErrorCode;

THOT_EXPORT char       *DocumentURLs[MAX_DOCUMENTS];
THOT_EXPORT PtrDocument LoadedDocument[MAX_DOCUMENTS]; /* loaded documents */
THOT_EXPORT DisplayMode	documentDisplayMode[MAX_DOCUMENTS];
THOT_EXPORT SelectionDescriptor NewDocSelection[MAX_DOCUMENTS];
THOT_EXPORT CHARSET     DialogCharset;/* the charset of dialog string */
THOT_EXPORT PtrDocument	DocAutoSave;  /* document to be saved autoimatically */
THOT_EXPORT int         DocBackUpInterval; /* number of characters which
					      triggers an automatic save */
THOT_EXPORT ThotBool	  ChangeLabel;  /* Labels must be changed */
THOT_EXPORT ThotBool    Printing;     /* TRUE for print, FALSE for amaya */
THOT_EXPORT ThotBool    Dragging; // dragging
THOT_EXPORT ThotBool    ShowOnePage;  /* TRUE when only a page is displayed */
THOT_EXPORT ThotBool    UseDotForFloat; /* TRUE if floats take the form xx.yy */
