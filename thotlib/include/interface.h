/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996-2008
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

#ifndef _INTERFACE_H_
#define _INTERFACE_H_

#include "thot_gui.h"
#include "tree.h"
#include "view.h"

/** Flags to decorate attributes in the interface.*/
typedef enum _RestrictionFlag
{
  attr_normal    = 0,
  attr_mandatory = 1,
  attr_readonly  = 2,
  attr_enum      = 4, /* The type is restricted to enumerations .*/
  attr_new       = 8, /* the attribute will be added but we
                          dont have any value yet.*/
}RestrictionFlag;

/** Attribute content type. Use it to filter showed content.*/
typedef enum _RestrictionContentType
{
  restr_content_no_restr = 0, /* no restriction. */
  restr_content_number   = 1, /* == Template_ATTR_type_VAL_number. */
  restr_content_string   = 2, /* == Template_ATTR_type_VAL_string. */
  restr_content_list     = 3, /* == Template_ATTR_type_VAL_listVal. */
  restr_content_lang     = 4, /* language */
  restr_content_max
}RestrictionContentType;

/**
 * Descritpion of a restrictied data.
 * Usefull to restrict attribute values with templates (or other).
 */
typedef struct _TypeRestriction
{
  int                    RestrFlags;  /** union of RestrictionFlag. */
  RestrictionContentType RestrType;   /** restricted type. */
  char*                  RestrDefVal; /** default value. */
  char*                  RestrEnumVal;/** enum values. */
}TypeRestriction;

/**
 * Callback prototype for filtering attributes.
 */
typedef void        (*AttributeFilterProc) (Element, Document, SSchema, int, TypeRestriction*);


typedef void        (*Proc) ();
typedef void        (*Proc1) (void * );
typedef void        (*Proc2) (void *,void *);
typedef void        (*Proc3) (void *,void *,void *);
typedef void        (*Proc4) (void *,void *,void *,void *);
typedef void        (*Proc5) (void *,void *,void *,void *,void *);
typedef int         (*UserProc) ();
typedef             ThotBool (*Func) ();
/* for c++ compatibility */
typedef             ThotBool (*Func1) (void *);
typedef             ThotBool (*Func2) (void *, void *);
typedef             ThotBool (*Func3) (void *, void *, void *);

/************************************************************************
 *									*
 * The problem of handling ISO-Latin-1 input with Motif			*
 *   - need to write a wrapper function called XLookupString.		*
 *   - set up the variable TtaUseOwnXLookupString non-zero.		*
 *									*
 ************************************************************************/

//extern int          TtaUseOwnXLookupString;

#define ISO_1_keyboard			0
#define Greek_keyboard			1
#define Symbol_keyboard			2
#define Graphic_keyboard		3

#ifndef __CEXTRACT__
/*----------------------------------------------------------------------
  TtaSetCopyAndCutFunction registers the function to be called when
  a Copy or Cut operation is executed:
  void procedure (Docucment doc)
  ----------------------------------------------------------------------*/
extern void TtaSetCopyAndCutFunction (Proc1 procedure);

/*----------------------------------------------------------------------
  TtaSetCopyCellFunction registers the function to be called when
  a cell of a row or a column is copied:
  void procedure (Element el, Docucment doc, ThotBool inRow)
  ----------------------------------------------------------------------*/
extern void TtaSetCopyCellFunction (Proc3 procedure);

/*----------------------------------------------------------------------
  TtaSetCopyRowFunction registers the function to be called when
  a table row is copied:
  void procedure (Element copy, Element orig, Docucment doc)
  ----------------------------------------------------------------------*/
extern void TtaSetCopyRowFunction (Proc3 procedure);

/*----------------------------------------------------------------------
  TtaSetNextCellInColumnFunction registers the function to be called when
  a table cell row is copied:
  void procedure (Element* cell, Element* row, Element colHead, Document doc,
  ThotBool* fake)
  ----------------------------------------------------------------------*/
extern void TtaSetNextCellInColumnFunction (Proc5 procedure);

/*----------------------------------------------------------------------
  TtaSetFocusChange registers the function to be called when the document
  focus changes:
  void procedure (Docucment doc)
  ----------------------------------------------------------------------*/
extern void TtaSetFocusChange (Proc1 procedure);

/*----------------------------------------------------------------------
  TtaSetAccessKeyFunction registers the access key function.
  ----------------------------------------------------------------------*/
extern void TtaSetAccessKeyFunction (Proc2 procedure);

/*----------------------------------------------------------------------
  TtaSetAttributeChangeFunction registers the attribute creation function
  ----------------------------------------------------------------------*/
extern void TtaSetAttributeChangeFunction (Proc2 procedure);

/*----------------------------------------------------------------------
  TtaAddAccessKey registers a new access key for the document doc
  The parameter param which will be returned when the access key will be
  activated.
  ----------------------------------------------------------------------*/
extern void TtaAddAccessKey (Document doc, unsigned int key, void *param);

/*----------------------------------------------------------------------
  TtaRemoveDocAccessKeys removes all access keys of a document.
  ----------------------------------------------------------------------*/
extern void TtaRemoveDocAccessKeys (Document doc);

/*----------------------------------------------------------------------
  TtaRemoveAccessKey removes an access key of a document.
  ----------------------------------------------------------------------*/
extern void TtaRemoveAccessKey (Document doc, unsigned int key);

/*----------------------------------------------------------------------
  TtaUpdateAttrMenu                                                       
  Updates the Attributes tool.
  ----------------------------------------------------------------------*/
extern void TtaUpdateAttrMenu (Document document);

/*----------------------------------------------------------------------
  TtaIsTextInserting returns the TextInserting status
  ----------------------------------------------------------------------*/
extern ThotBool TtaIsTextInserting ();

/*----------------------------------------------------------------------
  TtaExecuteMenuAction execute the corresponding menu action.
  ----------------------------------------------------------------------*/
extern void TtaExecuteMenuAction (const char *actionName, Document doc, View view,
				  ThotBool force);

/*----------------------------------------------------------------------
  TtaPostMenuAction execute the corresponding menu action when idle.
  ----------------------------------------------------------------------*/
extern void TtaPostMenuAction (const char *actionName, Document doc, View view,
                           ThotBool force);

/*----------------------------------------------------------------------
  TtaPostMenuActionById execute the corresponding menu action when idle.
  ----------------------------------------------------------------------*/
extern void TtaPostMenuActionById (int actionId, Document doc, View view,
                           ThotBool force);



/*----------------------------------------------------------------------
   TtaAddButton
   Adds a new button entry in a document view.
   This function must specify a valid view of a valid document.
   Parameters:
   document: the concerned document.
   view: the concerned view.
   picture: the displayed pixmap. None (0) creates a space between buttons.
   procedure: procedure to be executed when the new entry is
   selected by the user. Null creates a cascade button.
   functionName: internal name of the function used to connect shortcuts.
   info: text to display when the cursor stays on the button.
   type: button type, only used on Windows versions.
   state: TRUE to enable the button, false to disable it.
   Returns index
  ----------------------------------------------------------------------*/
extern int TtaAddButton (Document document, View view, ThotIcon picture,
			 void (*procedure) (), char* functionName, char *info,
			 unsigned char type, ThotBool state);

/*----------------------------------------------------------------------
   TtaGetButtonCallback
   Get the callback of a button in a document view.
   Returns the callback if it exists
           NULL if it doesn't exists
   Parameters:
   document: the concerned document.
   view: the concerned view.
   index: the index.
  ----------------------------------------------------------------------*/
extern void *TtaGetButtonCallback (Document document, View view, int index);

/*----------------------------------------------------------------------
   TtaSwitchButton
   Change the status of the button entry in a document view.
   This function must specify a valid view of a valid document.
   Parameters:
   document: the concerned document.
   view: the concerned view.
   index: the index.
  ----------------------------------------------------------------------*/
extern void TtaSwitchButton (Document document, View view, int index);

/*----------------------------------------------------------------------
   TtaChangeButton
   Change the button entry in a document view.
   This function must specify a valid view of a valid document.
   Parameters:
   document: the concerned document.
   view: the concerned view.
   index: the index.
   picture: the new icon.
   state: TRUE to enable the button, false to disable it.
  ----------------------------------------------------------------------*/
extern void TtaChangeButton (Document document, View view, int index,
			     ThotIcon picture, ThotBool state);

/*----------------------------------------------------------------------
   TtaIsButtonActivated

   Indicates if a callback function has been called from the menu 
   or from a button
  ----------------------------------------------------------------------*/
extern ThotBool TtaIsButtonActivated (Document document, View view);

/*----------------------------------------------------------------------
   TtaAddTextZone
   Adds a new textual command in a document view.
   This function must specify a valid view of a valid document.
   Parameters:
   document: the concerned document.
   view: the concerned view.
   label: label of the new entry.
   procedure: procedure to be executed when the new entry is changed by the
   user.
   listUrl gives URLs that will be displayed in the combobox.
  ----------------------------------------------------------------------*/
extern int TtaAddTextZone (Document document, View view, char *label,
			   ThotBool editable, void (*procedure) (), char *listUrl);

/*----------------------------------------------------------------------
   TtaSetTextZone
   Sets the text in text-zone in a document view.
   This function must specify a valid view of a valid document.
   Parameters:
   document: identifier of the document.
   view: identifier of the view.
   listUrl gives URLs that will be displayed in the combobox.
  ----------------------------------------------------------------------*/
extern void TtaSetTextZone (Document document, View view, char *listUrl);

/*----------------------------------------------------------------------
  TtaEnableAction enable/disable the given action.
  ----------------------------------------------------------------------*/
extern void TtaEnableAction( Document document, const char * action_name, ThotBool enable );

/*----------------------------------------------------------------------
   TtaSetMenuOff desactive le menu (1 a n) de la vue du document ou   
   de la fenetre principale (document = 0, view = 0).                 
  ----------------------------------------------------------------------*/
extern void TtaSetMenuOff (Document document, View view, int menuID);

/*----------------------------------------------------------------------
   TtaSetMenuOn reactive le menu (1 a n) de la vue du document ou     
   de la fenetre principale (document = 0, view = 0).                 
  ----------------------------------------------------------------------*/
extern void TtaSetMenuOn (Document document, View view, int menuID);

/*----------------------------------------------------------------------
   TtaSetToggleItem positionne l'item du menu de la vue du document   
   ou de la fenetre principale (document = 0, view = 0).   
  ----------------------------------------------------------------------*/
extern void TtaSetToggleItem (Document document, View view, int menuID,
							  int itemID, ThotBool on);

/*----------------------------------------------------------------------
   TtaSetItemOff desactive l'item actionName de la vue du document  
   ou de la fenetre principale (document = 0, view = 0).   
  ----------------------------------------------------------------------*/
extern void TtaSetItemOff (Document document, View view, int menuID, int itemID);

/*----------------------------------------------------------------------
   TtaSetItemOn active l'item actionName de la vue du document      
   ou de la fenetre principale (document = 0, view = 0).   
  ----------------------------------------------------------------------*/
extern void TtaSetItemOn (Document document, View view, int menuID, int itemID);

/*----------------------------------------------------------------------
   TtaSetCallback
   Specifies the procedure that is called when the user activates a set of forms
   and/or menus created by the application.
   The parameter set gives the number of forms and/or menus created by the 
   application managed by this porcedure.
   This function must be called before processing any event, only if the
   application uses the Dialogue tool kit for generating specific forms or menus.
   This function replaces the DefineCallback function in the Dialogue tool kit.
   This procedure is called with three parameters: the menu or form reference,
   data type, and data value.
   void callbakProcedure(reference, datatype, data)
   int reference;
   int datatype;
   char *data; 
   Parameter:
   callbakProcedure: the application callback procedure.
   set: the number of forms and/or menus managed.
   Return:
   reference of the first form or menu.
  ----------------------------------------------------------------------*/
extern int TtaSetCallback (void (*callbakProcedure) (), int set);

/*----------------------------------------------------------------------
   TtaSetStatus affiche le status de la vue du document.                      
  ----------------------------------------------------------------------*/
extern void TtaSetStatus (Document document, View view, const char *text, const char *name);

/*----------------------------------------------------------------------
  TtaSetStatusSelectedElement Set the current selected element in the status bar.
  ----------------------------------------------------------------------*/
extern void TtaSetStatusSelectedElement(Document document, View view, Element elem);

/*----------------------------------------------------------------------
   TtaGetViewFrame retourne le widget du frame de la vue document.    
  ----------------------------------------------------------------------*/
extern ThotWidget TtaGetViewFrame (Document document, View view);


/*----------------------------------------------------------------------
   TtaMainLoop
   The application main event loop
  ----------------------------------------------------------------------*/
extern void TtaMainLoop (void);

/*----------------------------------------------------------------------
    TtaHandlePendingEvents
    Processes all pending events in an application.
  ----------------------------------------------------------------------*/
extern void TtaHandlePendingEvents ();

/*----------------------------------------------------------------------
  TtaClickElement
  Waits for a new click and returns the cliked document and element.
  ----------------------------------------------------------------------*/
extern void TtaClickElement (Document *document, Element *element);

/*----------------------------------------------------------------------
  TtaGetClickedElement
  Returns the last clicked document and element.
  ----------------------------------------------------------------------*/
extern void TtaGetClickedElement (Document *document, Element *element);

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
extern ThotIcon TtaCreatePixmapLogo (char **data);

/*----------------------------------------------------------------------
  TtaCreateBitmapLogo
  Creates a logo pixmap from a bitmap description: width, height and bit array.
  Parameters:
  width: the width value of the bitmap.
  height: the height value of the bitmap.
  bits: the bit array.
  Return value:
  The created pixmap for the logo.
  ----------------------------------------------------------------------*/
extern ThotPixmap TtaCreateBitmapLogo (int width, int height, char* bits);

extern void TtaSetCursorWatch (Document document, View view);
extern void TtaResetCursor (Document document, View view);

/*----------------------------------------------------------------------
   TtaGetMenuColor
   Returns the color used for the background of dialogue windows.
  ----------------------------------------------------------------------*/
extern ThotColor TtaGetMenuColor (void);

/*----------------------------------------------------------------------
  TtaUpdateEditorColors
  Updates the current foreground and background (text and menus) from
  the values stored in the registry.
  ----------------------------------------------------------------------*/
extern void TtaUpdateEditorColors (void);

/*----------------------------------------------------------------------
   TtaGetScreenDepth
   Returns the current screen depth.
  ----------------------------------------------------------------------*/
extern int TtaGetScreenDepth (void);

/*----------------------------------------------------------------------
   TtaFetchOneAvailableEvent
   retrieve one X-Windows Event from the queue if one is immediately
   available.
  ----------------------------------------------------------------------*/
ThotBool TtaFetchOneAvailableEvent (ThotEvent *ev);

/*----------------------------------------------------------------------
   TtaHandleOneEvent
   process an X-Windows Event.
  ----------------------------------------------------------------------*/
extern void TtaHandleOneEvent (ThotEvent *ev);

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
extern int TtaXLookupString (ThotKeyEvent *event, char *buffer, int nbytes,
			     ThotKeySym *keysym, ThotComposeStatus *status);

extern ThotDisplay *TtaGetCurrentDisplay (void);

/*----------------------------------------------------------------------
   TtaGiveSelectPosition: returns the mouse position for the last click 
   with respect to the element (position in pixel)            
  ----------------------------------------------------------------------*/
extern void TtaGiveSelectPosition (Document document, Element element,
				   View view, /*OUT*/ int *X, /*OUT*/ int *Y);

/*----------------------------------------------------------------------
  BuildMenus builds or rebuilds frame menus.
  The parameter RO is TRUE when only ReadOnly functions are accepted
  ----------------------------------------------------------------------*/
extern void TtaUpdateMenus (Document doc, View view, ThotBool RO);


/*----------------------------------------------------------------------
  TtaRefreshMenuStats enable/disable a top menu for the given doc
  or all menus (menu_id = -1)
  ----------------------------------------------------------------------*/
extern void TtaRefreshTopMenuStats(Document doc, int menu_id );


/*----------------------------------------------------------------------
  TtaToggleLogError enables/disables the logerror button
  ----------------------------------------------------------------------*/
extern void TtaToggleLogError (Document doc_id, ThotBool enable);

/*----------------------------------------------------------------------
  TtaSetLockButton button
  ----------------------------------------------------------------------*/
extern void TtaSetLockButton (Document doc_id, int status);

/*----------------------------------------------------------------------
  TtaRefreshMenuItemStats enable/disable, toggle/untoggle menu items
  widgets for the given doc or all items of all menus (menu_id = -1)
  ----------------------------------------------------------------------*/
extern void TtaRefreshMenuItemStats (Document doc, void *ptrmenu, int menu_item_id );

/*----------------------------------------------------------------------
   TtaListShortcuts
   Produces in a file a human-readable the list of current shortcuts.  
  ----------------------------------------------------------------------*/
extern void TtaListShortcuts (Document doc, FILE *fileDescriptor);

/*----------------------------------------------------------------------
  TtaShowElementMenu
  ----------------------------------------------------------------------*/
extern void TtaShowElementMenu (Document doc, View view);

/*----------------------------------------------------------------------
  TtaRefreshElementMenu
  Just refresh the XML element list.
  ----------------------------------------------------------------------*/
extern void TtaRefreshElementMenu (Document doc, View view);

/*----------------------------------------------------------------------
  TtaRefreshActiveFrame force a refresh of window widgets
  ----------------------------------------------------------------------*/
extern void TtaRefreshActiveFrame ();

/*----------------------------------------------------------------------
  TtaStringToClipboard
  ----------------------------------------------------------------------*/
extern void TtaStringToClipboard (unsigned char *s, CHARSET encoding);


/*----------------------------------------------------------------------
  TtaUpdateToolPanelLayout
  ----------------------------------------------------------------------*/
extern void TtaUpdateToolPanelLayout ();


/*----------------------------------------------------------------------
  TtaCloseAllHelpWindows Closes all opened help windows.
  ----------------------------------------------------------------------*/
extern void TtaCloseAllHelpWindows ();

#endif /* __CEXTRACT__  */

#endif
