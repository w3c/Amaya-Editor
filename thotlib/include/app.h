
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

/*  Thot Toolkit Events defined by Irene Vatton */

#ifndef _APP_H_
#define _APP_H_
#include "interface.h"
#include "appaction.h"

typedef void        (*Proc) ();
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
  char               *ActionName;	/* Name of the action */
  Proc                doAction;	/* The action to be executed */
  boolean             actPre;	/* Previous event implies function */
  APPevent            actEvent;	/* What NotifyEvent context to generate */
  PtrAction           next;	/* Next action in the list */
} APP_action;


typedef struct _actionbindinglist *PtrActionBindingList;
typedef struct _actionbindinglist
{
  PtrAction           action;	/* A pointer to the action  */
  int                 type;	/* element or attribute type associated with */
  boolean             pre;	/* Pre or Post event */
  PtrActionBindingList next;	/* Next set of binding (event, action) */
} actionbindinglist;

typedef struct _externapplilist *PtrExternAppliList;
typedef struct _externapplilist
{
  int                 strId;	/* Contains an identifier of what document-type
				   (S-file) the "action-table" is for */
  char               *appId;	/* Contains an identifier of what "action-table"
				   to use. */
  /* The two identifiers above determine the Message/actions to apply.
     This meens that you can have more I-files for the same document-type
     (S-file). */
  PtrActionBindingList eventsList[NUMBER_OF_APP_EVENTS];
  /* Each entry above points to a list of bindings concerning this specific
     event. Default binding will be the last entry of this list.
     Entries have to be in the same order than in the enum ECFevent */
  PtrExternAppliList  next;	/* Contains a pointer to the next 
				   "External application". */
} externapplilist;

extern void         KeyboardsLoadResources ();
extern void         StructEditingLoadResources ();
extern void         StructSelectLoadResources ();
extern void         LookupLoadResources ();
extern void         EditingLoadResources ();
extern void         AttributeMenuLoadResources ();
extern void         SelectionMenuLoadResources ();
extern void         DisplayEmptyBoxLoadResources ();
extern void         SpellCheckLoadResources ();


#ifndef __CEXTRACT__
#ifdef __STDC__
extern void         TteConnectAction (int id, Proc procedure);
extern void         TteZeroMenu (WindowType windowtype, char *schemaName);
extern void         TteOpenMainWindow (char *name, Pixmap logo, Pixmap icon);
extern void         TteInitMenus (char *name, int number);
extern void         TteAddMenuAction (char *actionName, Proc procedure);
extern void         TteAddMenu (WindowType windowtype, char *schemaName, int view, int menuID, int itemsNumber, char *menuName);
extern void         TteAddSubMenu (WindowType windowtype, char *schemaName, int menuID, int itemID, int itemsNumber);
extern void         TteAddMenuItem (WindowType windowtype, char *schemaName, int menuID, int subMenuID, int itemID, char *actionName, char itemType);

extern void         InsertAction (char *actionName, Proc doIt);
extern void         InitEventActions (PtrExternAppliList ptrAction, int typeId, APPevent event, boolean pre, char *actionName);
extern PtrExternAppliList MakeNewApplicationStruct (int structureId, char *applicationName);
extern void         TtcStandardPresentation (Document document, View view);
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
extern void         TteZeroMenu ( /*WindowType windowtype, char *schemaName */ );
extern void         TteOpenMainWindow ( /*char *name, Pixmap logo, Pixmap icon */ );
extern void         TteInitMenus ( /*char *name,int number */ );
extern void         TteAddMenuAction ( /*char *actionName, Proc procedure */ );
extern void         TteAddMenu ( /*WindowType windowtype, char *schemaName, int view, int menuID, int itemsNumber, char *menuName */ );
extern void         TteAddSubMenu ( /*WindowType windowtype, char *schemaName, int menuID, int itemID, int itemsNumber */ );
extern void         TteAddMenuItem ( /*WindowType windowtype, char *schemaName, int menuID, int subMenuID, int itemID, char *actionName, char itemType */ );

extern void         InsertAction ( /*char *actionName, Proc doIt */ );
extern void         InitEventActions ( /*PtrExternAppliList ptrAction, int typeId, ECFevent event, boolean pre, char *actionName */ );
extern PtrExternAppliList MakeNewApplicationStruct ( /*int structureId, char *applicationName */ );

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
