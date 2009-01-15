/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996-2009
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

/* Type definitions for binding actions and events */
typedef struct _APP_action *PtrAction;
typedef struct _APP_action
{
    const char*ActName;   /* Name of the action                      */
    Proc       ActAction; /* The action to be executed               */
    ThotBool   ActPre;    /* Previous event implies function         */
    APPevent   ActEvent;  /* What NotifyEvent context to generate    */
    PtrAction  ActNext;   /* Next action in the list                 */
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
  const char         *EvSName;	/* Name of the events set */
  PtrActionEvent      EvSList[NUMBER_OF_APP_EVENTS];
  PtrEventsSet        EvSNext;	/* Next EventsSet in the list */
} EventsSet;

extern void         AttributeMenuLoadResources ();
extern void         BackupOnFatalErrorLoadResources ();
extern void         EditingLoadResources ();
extern void         GraphicssLoadResources ();
extern void         LookupLoadResources ();
extern void         NoStructSelectLoadResources ();
extern void         PivotLoadResources ();
extern void         SpellCheckLoadResources ();
extern void         StructEditingLoadResources ();
extern void         StructSelectLoadResources ();

#ifndef __CEXTRACT__
extern void         TteConnectAction (int id, Proc procedure);
extern void         TtaSetBackup (Proc procedure);
extern void         TtaSetAutoSave (Proc1 procedure);
extern void         TtaSetDocStatusUpdate (Proc procedure);
extern ThotBool     TtaNotifySubTree (APPevent appEvent, Document doc, Element elem,
                                      ThotBool pre);
extern void         TteZeroMenu ();
extern void         TteOpenMainWindow (char *name);
extern void         TteInitMenus (char *name, int number);
extern void         TteAddMenuAction (const char* actionName, Proc procedure, ThotBool state);
extern void         TteAddMenu (int view, int menuID, int itemsNumber, const char* menuName);
extern void         TteAddSubMenu (int menuID, int itemID, int itemsNumber);
extern void         TteAddMenuItem (int menuID, int subMenuID, int itemID, const char* actionName, char itemType, const char * iconName);

extern void         TteAddAction (const char* actionName, Proc doIt);
extern void         TteAddActionEvent (PtrEventsSet eventsList, int typeId, APPevent event, ThotBool pre, const char* actionName);
extern PtrEventsSet TteGetEventsSet (const char *name);
extern PtrEventsSet TteNewEventsSet (int structureId, const char *name);
extern void         TtcStandardPresentation (Document document, View view);
extern void         TtcStandardGeometry (Document document, View view);
extern void         TtaSetTransformCallback (Func2 function);

/* List of editor dialogue actions */
extern void         TtcGetPaletteColors (int *fgcolor, int *bgcolor, ThotBool palType);
extern void         TtcChangeGraphics (Document document, View view);
extern void         TtcChildElement (Document document, View view);
extern void         TtcCloseDocument (Document document, View view);
extern void         TtcCloseView (Document document, View view);
extern void         TtcCopySelection (Document document, View view);
extern void         TtcCopyToClipboard (Document document, View view);
extern void         TtcCreateElement (Document doc, View view);
extern void         TtcCutSelection (Document document, View view);
extern void         TtcDeletePreviousChar (Document document, View view);
extern void         TtcDeleteSelection (Document document, View view);
extern void         TtcEndOfLine (Document document, View view);
extern void         TtcGotoPage (Document document, View view);
extern void         TtcHolophrast (Document document, View view);
extern void         TtcInclude (Document document, View view);
extern void         TtcIndex (Document document, View view);
extern void         TtcInsert (Document document, View view);
extern void         TtcInsertChar (Document document, View view, CHAR_T c);
extern void         TtcInsertGraph (Document document, View view, unsigned char c);
extern void         TtcInsertLineBreak (Document document, View view);
extern void         TtcInsertPageBreak (Document document, View view);
extern void         TtcLineDown (Document document, View view);
extern void         TtcLineUp (Document document, View view);
extern void         TtcNextChar (Document document, View view);
extern void         TtcNextSelChar (Document document, View view);
extern void         TtcNextElement (Document document, View view);
extern void         TtcNextLine (Document document, View view);
extern void         TtcNextSelLine (Document document, View view);
extern void         TtcNextSelPage (Document document, View view);
extern void         TtcNextWord (Document document, View view);
extern void         TtcNextSelWord (Document document, View view);
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
extern void         TtcPreviousSelPage (Document document, View view);
extern void         TtcPreviousWord (Document document, View view);
extern void         TtcPreviousSelWord (Document document, View view);
extern void         TtcQuit (Document document, View view);
extern void         TtcScrollLeft (Document document, View view);
extern void         TtcScrollRight (Document document, View view);
extern void         TtcSearchText (Document document, View view);
extern void         TtcSelStartOfLine (Document document, View view);
extern void         TtcSelEndOfLine (Document document, View view);
extern void         TtcSetBackupInterval (Document document, View view);
extern void         TtcSetDocumentPath (Document document, View view);
extern void         TtcSetSchemaPath (Document document, View view);
extern void         TtcSpellCheck (Document document, View view);
extern void         TtcStartOfLine (Document document, View view);
extern void         TtcSwitchButtonBar (Document document, View view);
extern void         TtcSwitchCommands (Document document, View view);
extern void         TtcUndo  (Document document, View view);
extern void         TtcRedo  (Document document, View view);
extern void         TtcNextSearchReplace (Document document, View view);
extern void         TtcCloseSearchReplace (Document document, View view);

#endif /* __CEXTRACT__ */

#endif
