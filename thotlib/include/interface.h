/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996-2003
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

#ifndef _INTERFACE_H_
#define _INTERFACE_H_

#include "thot_gui.h"
#include "tree.h"
#include "view.h"

/************************************************************************
 *									*
 * The problem of handling ISO-Latin-1 input with Motif			*
 *   - need to write a wrapper function called XLookupString.		*
 *   - set up the variable TtaUseOwnXLookupString non-zero.		*
 *									*
 ************************************************************************/

extern int          TtaUseOwnXLookupString;

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
extern void TtaSetCopyAndCutFunction (void (*procedure) ());

/*----------------------------------------------------------------------
  TtaSetCopyCellFunction registers the function to be called when
  a cell of a row or a column is copied:
  void procedure (Element el, Docucment doc, ThotBool inRow)
  ----------------------------------------------------------------------*/
extern void TtaSetCopyCellFunction (void (*procedure) (void *,void *,void *));

/*----------------------------------------------------------------------
  TtaSetAccessKeyFunction registers the access key function.
  ----------------------------------------------------------------------*/
extern void TtaSetAccessKeyFunction (void (*procedure) ());

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
  TtaExecuteMenuAction execute the corresponding menu action.
  ----------------------------------------------------------------------*/
extern void TtaExecuteMenuAction (char *actionName, Document doc, View view);

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
extern void TtaSetStatus (Document document, View view, char *text, char *name);

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
   Returns document and element clicked.
  ----------------------------------------------------------------------*/
extern void TtaClickElement (/*OUT*/ Document *document, /*OUT*/ Element *element);
  
/*----------------------------------------------------------------------
   TtaCreateBitmap
   create a bitmap from a file
   const char * filename : the picture filename
   ThotPictFormat type   : the picture type (picture.h)
	XBM_FORMAT =      	0,      X11 BitmapFile format 
	XPM_FORMAT =     	2,      Xpm XReadFileToPixmap format 
	GIF_FORMAT =       	3,      gif 
	PNG_FORMAT =            4,      Png 
	JPEG_FORMAT =           5,      Jpeg 
  ----------------------------------------------------------------------*/
extern ThotPixmap TtaCreateBitmap( const char * filename, int type );

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
   TtaFetchOneEvent
   retrieve one X-Windows Event from the queue, this is a blocking call.
  ----------------------------------------------------------------------*/
extern void TtaFetchOneEvent (ThotEvent *ev);

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

extern int TtaXLookupString (ThotKeyEvent *event, char *buffer, int nbytes,
			     ThotKeySym *keysym, ThotComposeStatus *status);

extern ThotDisplay *TtaGetCurrentDisplay (void);

/*----------------------------------------------------------------------
   TtaGiveSelectPosition: returns the mouse position for the last click 
   with respect to the element (position in pixel)            
  ----------------------------------------------------------------------*/
extern void TtaGiveSelectPosition (Document document, Element element, View view, /*OUT*/ int *X, /*OUT*/ int *Y);

/*----------------------------------------------------------------------
  BuildMenus builds or rebuilds frame menus.
  The parameter RO is TRUE when only ReadOnly functions are accepted
  ----------------------------------------------------------------------*/
extern void TtaUpdateMenus (Document doc, View view, ThotBool RO);

/*----------------------------------------------------------------------
  TtaMakeWindow create a AmayaWindow object and place it
  into WindowsTable array
  returns:
 	+ the window id
        + -1 if too much created windows
  ----------------------------------------------------------------------*/
extern int TtaMakeWindow( );

/*----------------------------------------------------------------------
  TtaMakeFrame create a frame (view container)
  notice : a frame need to be attached to a window
  params:
    + doc : the document id
  returns:
    + the frame id
    + -1 if too much created views
  ----------------------------------------------------------------------*/
extern int TtaMakeFrame( Document doc,
                          int view,
                          int width,
                          int height,
                          int * volume );

/*----------------------------------------------------------------------
   TtaListShortcuts
   Produces in a file a human-readable the list of current shortcuts.  
  ----------------------------------------------------------------------*/
extern void TtaListShortcuts (Document doc, FILE *fileDescriptor);
#endif /* __CEXTRACT__  */

#endif
