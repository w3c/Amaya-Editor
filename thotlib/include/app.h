/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * Warning:
 * This module is part of the Thot library, which was originally
 * developed in French. That's why some comments are still in
 * French, but their translation is in progress and the full module
 * will be available in English in the next release.
 * 
 */
 
#ifndef _APP_H_
#define _APP_H_
#include "interface.h"
#include "appaction.h"

typedef void        (*Proc) ();
typedef int         (*UserProc) ();
typedef             boolean (*Func) ();

/* Type definition for windows */
typedef enum
{
     MainWindow,
     DocWindow,
     DocTypeWindow
} WindowType;

/* Type definitions for binding actions and events */
typedef struct _APP_action *PtrAction;
typedef struct _APP_action
{
  char               *ActName;	/* Name of the action */
  Proc                ActAction;/* The action to be executed */
  UserProc            ActUser;  /* Any specific action defined by the user */
  void               *ActArg;   /* and the associated argument */
  boolean             ActPre;	/* Previous event implies function */
  APPevent            ActEvent;	/* What NotifyEvent context to generate */
  PtrAction           ActNext;	/* Next action in the list */
} APP_action;


typedef struct _ActionEvent *PtrActionEvent;
typedef struct _ActionEvent
{
  PtrAction           AEvAction;/* A pointer to the action  */
  int                 AEvType;	/* Element or attribute type associated with */
  boolean             AEvPre;	/* Pre or Post event */
  PtrActionEvent      AEvNext;	/* Next set of(event/action) */
} ActionEvent;

typedef struct _EventsSet *PtrEventsSet;
typedef struct _EventsSet
{
  int                 EvSStructId;/* Identifier of SSchema */
  char               *EvSName;	/* Name of the events set */
  PtrActionEvent      EvSList[NUMBER_OF_APP_EVENTS];
  PtrEventsSet        EvSNext;	/* Next EventsSet in the list */
} EventsSet;

extern void         GraphicssLoadResources ();
extern void         KeyboardsLoadResources ();
extern void         StructEditingLoadResources ();
extern void         StructSelectLoadResources ();
extern void         LookupLoadResources ();
extern void         EditingLoadResources ();
extern void         AttributeMenuLoadResources ();
extern void         SelectionMenuLoadResources ();
extern void         DisplayEmptyBoxLoadResources ();
extern void         SpellCheckLoadResources ();
extern void         TableHLoadResources ();
extern void	    BackupOnFatalErrorLoadResources ();


#ifndef __CEXTRACT__
#ifdef __STDC__
extern void         TteConnectAction (int id, Proc procedure);
extern void         TtaSetBackup (Proc procedure);
extern void         TteZeroMenu (WindowType windowtype, char *schemaName);
extern void         TteOpenMainWindow (char *name, Pixmap logo, Pixmap icon);
extern void         TteInitMenus (char *name, int number);
extern void         TteAddMenuAction (char *actionName, Proc procedure);
extern int          TteAddUserMenuAction (char *actionName, UserProc procedure, void *arg);
extern void         TteAddMenu (WindowType windowtype, char *schemaName, int view, int menuID, int itemsNumber, char *menuName);
extern void         TteAddSubMenu (WindowType windowtype, char *schemaName, int menuID, int itemID, int itemsNumber);
extern void         TteAddMenuItem (WindowType windowtype, char *schemaName, int menuID, int subMenuID, int itemID, char *actionName, char itemType);

extern void         TteAddAction (char *actionName, Proc doIt);
extern int          TteAddUserAction (char *actionName, UserProc procedure, void *arg);
extern void         TteAddActionEvent (PtrEventsSet eventsList, int typeId, APPevent event, boolean pre, char *actionName);
extern PtrEventsSet TteGetEventsSet (char *name);
extern PtrEventsSet TteNewEventsSet (int structureId, char *name);
extern void         TtcStandardPresentation (Document document, View view);
extern void         TtcStandardGeometry (Document document, View view);
extern void         TtcChangeType (Document document, View view);

/* List of editor dialogue actions */
extern void         TtcChangeCharacters (Document document, View view);
extern void         TtcChangeColors (Document document, View view);
extern void         TtcChangeFormat (Document document, View view);
extern void         TtcChangeGraphics (Document document, View view);
extern void         TtcChangePresentation (Document document, View view);
extern void         TtcChildElement (Document document, View view);
extern void         TtcCloseDocument (Document document, View view);
extern void         TtcCloseView (Document document, View view);
extern void         TtcCopySelection (Document document, View view);
extern void         TtcCopyToClipboard (Document document, View view);
extern void         TtcCreateDocument (Document document, View view);
extern void         TtcCutSelection (Document document, View view);
extern void         TtcDeletePreviousChar (Document document, View view);
extern void         TtcDeleteSelection (Document document, View view);
extern void         TtcDisplayGraphicsKeyboard (Document document, View view);
extern void         TtcDisplayGreekKeyboard (Document document, View view);
extern void         TtcDisplayLatinKeyboard (Document document, View view);
extern void         TtcDisplayMathKeyboard (Document document, View view);
extern void         TtcEndOfLine (Document document, View view);
extern void         TtcGotoPage (Document document, View view);
extern void         TtcHolophrast (Document document, View view);
extern void         TtcInclude (Document document, View view);
extern void         TtcIndex (Document document, View view);
extern void         TtcInsert (Document document, View view);
extern void         TtcInsertChar (Document document, View view, char c);
extern void         TtcInsertGraph (Document document, View view, char c);
extern void         TtcInsertPageBreak (Document document, View view);
extern void         TtcNextChar (Document document, View view);
extern void         TtcNextElement (Document document, View view);
extern void         TtcNextLine (Document document, View view);
extern void         TtcOpenDocument (Document document, View view);
extern void         TtcOpenView (Document document, View view);
extern void         TtcPageDown (Document document, View view);
extern void         TtcPageEnd (Document document, View view);
extern void         TtcPageTop (Document document, View view);
extern void         TtcPageUp (Document document, View view);
extern void         TtcPaginateDocument (Document document, View view);
extern void         TtcPaginateView (Document document, View view);
extern void         TtcParentElement (Document document, View view);
extern void         TtcPaste (Document document, View view);
extern void         TtcPasteFromClipboard (Document document, View view);
extern void         TtcPreviousChar (Document document, View view);
extern void         TtcPreviousElement (Document document, View view);
extern void         TtcPreviousLine (Document document, View view);
extern void         TtcPrint (Document document, View view);
extern void         TtcPrintSetup (Document document, View view);
extern void         TtcSetupAndPrint (Document document, View view);
extern void         TtcSurround (Document document, View view);
extern void         TtcQuit (Document document, View view);
extern void         TtcSaveDocument (Document document, View view);
extern void         TtcSaveDocumentAs (Document document, View view);
extern void         TtcSearchEmptyElement (Document document, View view);
extern void         TtcSearchEmptyReference (Document document, View view);
extern void         TtcSearchReference (Document document, View view);
extern void         TtcSearchText (Document document, View view);
extern void         TtcSetBackupInterval (Document document, View view);
extern void         TtcSetDocumentPath (Document document, View view);
extern void         TtcSetSchemaPath (Document document, View view);
extern void         TtcSetVisibilityView (Document document, View view);
extern void         TtcSetZoomView (Document document, View view);
extern void         TtcSpellCheck (Document document, View view);
extern void         TtcStartOfLine (Document document, View view);
extern void         TtcSwitchButtonBar (Document document, View view);
extern void         TtcSwitchCommands (Document document, View view);

#else  /* __STDC__ */

extern void         TteConnectAction ( /*int id, Proc procedure */ );
extern void         TtaSetBackup (/* Proc procedure */);
extern void         TteZeroMenu ( /*WindowType windowtype, char *schemaName */ );
extern void         TteOpenMainWindow ( /*char *name, Pixmap logo, Pixmap icon */ );
extern void         TteInitMenus ( /*char *name,int number */ );
extern void         TteAddMenuAction ( /*char *actionName, Proc procedure */ );
extern void         TteAddUserAction ( /*char *actionName, UserProc procedure, void *arg*/ );
extern void         TteAddMenu ( /*WindowType windowtype, char *schemaName, int view, int menuID, int itemsNumber, char *menuName */ );
extern void         TteAddSubMenu ( /*WindowType windowtype, char *schemaName, int menuID, int itemID, int itemsNumber */ );
extern void         TteAddMenuItem ( /*WindowType windowtype, char *schemaName, int menuID, int subMenuID, int itemID, char *actionName, char itemType */ );

extern void         TteAddAction ( /*char *actionName, Proc doIt */ );
extern void         InitEventActions ( /*PtrEventsSet eventsList, int typeId, ECFevent event, boolean pre, char *actionName */ );
extern PtrEventsSet TteGetEventsSet ( /*char *name*/ );
extern PtrEventsSet TteNewEventsSet ( /*int structureId, char *name */ );

/* List of editor dialogue actions */
extern void         TtcChangeCharacters ( /*Document document, View view */ );
extern void         TtcChangeColors ( /*Document document, View view */ );
extern void         TtcChangeFormat ( /*Document document, View view */ );
extern void         TtcChangeGraphics ( /*Document document, View view */ );
extern void         TtcChangePresentation ( /*Document document, View view */ );
extern void         TtcChildElement ( /*Document document, View view */ );
extern void         TtcCloseDocument ( /*Document document, View view */ );
extern void         TtcCloseView ( /*Document document, View view */ );
extern void         TtcCopySelection ( /*Document document, View view */ );
extern void         TtcCopyToClipboard ( /*Document document, View view */ );
extern void         TtcCreateDocument ( /*Document document, View view */ );
extern void         TtcCutSelection ( /*Document document, View view */ );
extern void         TtcDeletePreviousChar ( /*Document document, View view */ );
extern void         TtcDeleteSelection ( /*Document document, View view */ );
extern void         TtcDisplayGraphicsKeyboard ( /*Document document, View view */ );
extern void         TtcDisplayGreekKeyboard ( /*Document document, View view */ );
extern void         TtcDisplayLatinKeyboard ( /*Document document, View view */ );
extern void         TtcDisplayMathKeyboard ( /*Document document, View view */ );
extern void         TtcEndOfLine ( /*Document document, View view */ );
extern void         TtcGotoPage ( /*Document document, View view */ );
extern void         TtcHolophrast ( /*Document document, View view */ );
extern void         TtcInclude ( /*Document document, View view */ );
extern void         TtcInsert ( /*Document document, View view */ );
extern void         TtcInsertChar ( /*Document document, View view, char c */ );
extern void         TtcInsertGraph ( /*Document document, View view, char c */ );
extern void         TtcInsertPageBreak ( /*Document document, View view */ );
extern void         TtcNextChar ( /*Document document, View view */ );
extern void         TtcNextElement ( /*Document document, View view */ );
extern void         TtcNextLine ( /*Document document, View view */ );
extern void         TtcOpenDocument ( /*Document document, View view */ );
extern void         TtcOpenView ( /*Document document, View view */ );
extern void         TtcPageDown ( /*Document document, View view */ );
extern void         TtcPageEnd ( /*Document document, View view */ );
extern void         TtcPageTop ( /*Document document, View view */ );
extern void         TtcPageUp ( /*Document document, View view */ );
extern void         TtcPaginateDocument ( /*Document document, View view */ );
extern void         TtcPaginateView ( /*Document document, View view */ );
extern void         TtcParentElement ( /*Document document, View view */ );
extern void         TtcPaste ( /*Document document, View view */ );
extern void         TtcPasteFromClipboard ( /*Document document, View view */ );
extern void         TtcPreviousChar ( /*Document document, View view */ );
extern void         TtcPreviousElement ( /*Document document, View view */ );
extern void         TtcPreviousLine ( /*Document document, View view */ );
extern void         TtcPrint ( /*Document document, View view */ );
extern void         TtcPrintSetup ( /*Document document, View view */ );
extern void         TtcSetupAndPrint ( /*Document document, View view */ );
extern void         TtcSurround ( /*Document document, View view */ );
extern void         TtcQuit ( /*Document document, View view */ );
extern void         TtcSaveDocument ( /*Document document, View view */ );
extern void         TtcSaveDocumentAs ( /*Document document, View view */ );
extern void         TtcSearchEmptyElement ( /*Document document, View view */ );
extern void         TtcSearchEmptyReference ( /*Document document, View view */ );
extern void         TtcSearchReference ( /*Document document, View view */ );
extern void         TtcSearchText ( /*Document document, View view */ );
extern void         TtcSetBackupInterval ( /*Document document, View view */ );
extern void         TtcSetDocumentPath ( /*Document document, View view */ );
extern void         TtcSetSchemaPath ( /*Document document, View view */ );
extern void         TtcSetVisibilityView ( /*Document document, View view */ );
extern void         TtcSetZoomView ( /*Document document, View view */ );
extern void         TtcSpellCheck ( /*Document document, View view */ );
extern void         TtcStartOfLine ( /*Document document, View view */ );
extern void         TtcSwitchButtonBar ( /*Document document, View view */ );
extern void         TtcSwitchCommands ( /*Document document, View view */ );

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */

#endif
