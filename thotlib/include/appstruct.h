/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * This module is part of the Thot library
 */
 
#ifndef _APPSTRUCT_H_
#define _APPSTRUCT_H_
#include "interface.h"
#include "appaction.h"

typedef void        (*Proc) ();
typedef int         (*UserProc) ();
typedef             ThotBool (*Func) ();

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
  STRING              ActName;	/* Name of the action */
  Proc                ActAction;/* The action to be executed */
  UserProc            ActUser;  /* Any specific action defined by the user */
  void               *ActArg;   /* and the associated argument */
  ThotBool            ActPre;	/* Previous event implies function */
  APPevent            ActEvent;	/* What NotifyEvent context to generate */
  PtrAction           ActNext;	/* Next action in the list */
} APP_action;


typedef struct _ActionEvent *PtrActionEvent;
typedef struct _ActionEvent
{
  PtrAction           AEvAction;/* A pointer to the action  */
  int                 AEvType;	/* Element or attribute type associated with */
  ThotBool            AEvPre;	/* Pre or Post event */
  PtrActionEvent      AEvNext;	/* Next set of(event/action) */
} ActionEvent;

typedef struct _EventsSet *PtrEventsSet;
typedef struct _EventsSet
{
  int                 EvSStructId;/* Identifier of SSchema */
  char*               EvSName;	/* Name of the events set */
  PtrActionEvent      EvSList[NUMBER_OF_APP_EVENTS];
  PtrEventsSet        EvSNext;	/* Next EventsSet in the list */
} EventsSet;

extern void         AttributeMenuLoadResources ();
extern void	    BackupOnFatalErrorLoadResources ();
extern void         DisplayEmptyBoxLoadResources ();
extern void         EditingLoadResources ();
extern void         GraphicssLoadResources ();
extern void         KeyboardsLoadResources ();
extern void         LookupLoadResources ();
extern void         NoStructSelectLoadResources ();
extern void	    PivotLoadResources ();
extern void         SelectionMenuLoadResources ();
extern void         SpellCheckLoadResources ();
extern void         StructEditingLoadResources ();
extern void         StructSelectLoadResources ();
extern void         TableHLoadResources ();

#ifndef __CEXTRACT__
#ifdef __STDC__
extern void         TteConnectAction (int id, Proc procedure);
extern void         TtaSetBackup (Proc procedure);
extern void         TtaSetDocStatusUpdate (Proc procedure);
extern void         TteZeroMenu (WindowType windowtype, STRING schemaName);
extern void         TteOpenMainWindow (STRING name, Pixmap logo, Pixmap icon);
extern void         TteInitMenus (char* name, int number);
extern void         TteAddMenuAction (char* actionName, Proc procedure, ThotBool state);
extern int          TteAddUserMenuAction (char* actionName, UserProc procedure, void *arg);
extern void         TteAddMenu (WindowType windowtype, STRING schemaName, int view, int menuID, int itemsNumber, STRING menuName);
extern void         TteAddSubMenu (WindowType windowtype, STRING schemaName, int menuID, int itemID, int itemsNumber);
extern void         TteAddMenuItem (WindowType windowtype, STRING schemaName, int menuID, int subMenuID, int itemID, char* actionName, char itemType);

extern void         TteAddAction (STRING actionName, Proc doIt);
extern int          TteAddUserAction (STRING actionName, UserProc procedure, void *arg);
extern void         TteAddActionEvent (PtrEventsSet eventsList, int typeId, APPevent event, ThotBool pre, STRING actionName);
extern PtrEventsSet TteGetEventsSet (char* name);
extern PtrEventsSet TteNewEventsSet (int structureId, char* name);
extern void         TtcStandardPresentation (Document document, View view);
extern void         TtcStandardGeometry (Document document, View view);
extern void         TtcChangeType (Document document, View view);
extern void         TtaSetTransformCallback (Func function);

/* List of editor dialogue actions */
extern void         TtcChangeCharacters (Document document, View view);
extern void         TtcChangeColors (Document document, View view);
extern void         TtcGetPaletteColors (int *fgcolor, int *bgcolor);
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
extern void         TtcInsertChar (Document document, View view, CHAR_T c);
extern void         TtcInsertGraph (Document document, View view, CHAR_T c);
extern void         TtcInsertPageBreak (Document document, View view);
extern void         TtcLineDown (Document document, View view);
extern void         TtcLineUp (Document document, View view);
extern void         TtcNextChar (Document document, View view);
extern void         TtcNextSelChar (Document document, View view);
extern void         TtcNextElement (Document document, View view);
extern void         TtcNextLine (Document document, View view);
extern void         TtcNextSelLine (Document document, View view);
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
extern void         TtcPreviousSelChar (Document document, View view);
extern void         TtcPreviousElement (Document document, View view);
extern void         TtcPreviousLine (Document document, View view);
extern void         TtcPreviousSelLine (Document document, View view);
extern void         TtcPrint (Document document, View view);
extern void         TtcPrintSetup (Document document, View view);
extern void         TtcSetupAndPrint (Document document, View view);
extern void         TtcSurround (Document document, View view);
extern void         TtcQuit (Document document, View view);
extern void         TtcSaveDocument (Document document, View view);
extern void         TtcSaveDocumentAs (Document document, View view);
extern void         TtcScrollLeft (Document document, View view);
extern void         TtcScrollRight (Document document, View view);
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
extern void         TtcUndo  (Document document, View view);
extern void         TtcRedo  (Document document, View view);

#else  /* __STDC__ */

extern void         TteConnectAction ( /*int id, Proc procedure */ );
extern void         TtaSetBackup (/* Proc procedure */);
extern void         TtaSetDocStatusUpdate (/* Proc procedure */);
extern void         TteZeroMenu ( /*WindowType windowtype, STRING schemaName */ );
extern void         TteOpenMainWindow ( /*STRING name, Pixmap logo, Pixmap icon */ );
extern void         TteInitMenus ( /*STRING name,int number */ );
extern void         TteAddMenuAction ( /*char* actionName, Proc procedure, ThotBool state */ );
extern void         TteAddUserAction ( /*STRING actionName, UserProc procedure, void *arg*/ );
extern void         TteAddMenu ( /*WindowType windowtype, STRING schemaName, int view, int menuID, int itemsNumber, STRING menuName */ );
extern void         TteAddSubMenu ( /*WindowType windowtype, STRING schemaName, int menuID, int itemID, int itemsNumber */ );
extern void         TteAddMenuItem ( /*WindowType windowtype, STRING schemaName, int menuID, int subMenuID, int itemID, STRING actionName, CHAR_T itemType */ );

extern void         TteAddAction ( /*STRING actionName, Proc doIt */ );
extern void         InitEventActions ( /*PtrEventsSet eventsList, int typeId, ECFevent event, ThotBool pre, STRING actionName */ );
extern PtrEventsSet TteGetEventsSet ( /*STRING name*/ );
extern PtrEventsSet TteNewEventsSet ( /*int structureId, STRING name */ );
extern void         TtaSetTransformCallback ( /*Func function*/ );

/* List of editor dialogue actions */
extern void         TtcChangeCharacters ( /*Document document, View view */ );
extern void         TtcChangeColors ( /*Document document, View view */ );
extern void         TtcGetPaletteColors ( /* int *fgcolor, int *bgcolor */ );
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
extern void         TtcInsertChar ( /*Document document, View view, CHAR_T c */ );
extern void         TtcInsertGraph ( /*Document document, View view, CHAR_T c */ );
extern void         TtcInsertPageBreak ( /*Document document, View view */ );
extern void         TtLineDown ( /*Document document, View view */ );
extern void         TtcLineUp ( /*Document document, View view */ );
extern void         TtcNextChar ( /*Document document, View view */ );
extern void         TtcNextSelChar ( /*Document document, View view */ );
extern void         TtcNextElement ( /*Document document, View view */ );
extern void         TtcNextLine ( /*Document document, View view */ );
extern void         TtcNextSelLine ( /*Document document, View view */ );
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
extern void         TtcPreviousSelChar ( /*Document document, View view */ );
extern void         TtcPreviousElement ( /*Document document, View view */ );
extern void         TtcPreviousLine ( /*Document document, View view */ );
extern void         TtcPreviousSelLine ( /*Document document, View view */ );
extern void         TtcPrint ( /*Document document, View view */ );
extern void         TtcPrintSetup ( /*Document document, View view */ );
extern void         TtcSetupAndPrint ( /*Document document, View view */ );
extern void         TtcSurround ( /*Document document, View view */ );
extern void         TtcQuit ( /*Document document, View view */ );
extern void         TtcSaveDocument ( /*Document document, View view */ );
extern void         TtcSaveDocumentAs ( /*Document document, View view */ );
extern void         TtcScrollLeft ( /* Document document, View view */ );
extern void         TtcScrollRight ( /* Document document, View view */ );
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
extern void         TtcUndo ( /*Document document, View view */ );
extern void         TtcRedo ( /*Document document, View view */ );
#endif /* __STDC__ */
#endif /* __CEXTRACT__ */

#endif
