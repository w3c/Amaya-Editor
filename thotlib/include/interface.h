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

#ifdef __STDC__
typedef void (* ExternalInitMainLoop)(ThotAppContext app_ctxt);
typedef void (* ExternalMainLoop)(void);
typedef int (* ExternalFetchEvent)(ThotEvent *ev);
typedef boolean (* ExternalFetchAvailableEvent)(ThotEvent *ev);
#else
typedef void (* ExternalInitMainLoop)();
typedef void (* ExternalMainLoop)();
typedef int (* ExternalFetchEvent)();
typedef boolean (* ExternalFetchAvailableEvent)();
#endif

#ifndef __CEXTRACT__
#ifdef __STDC__

extern void         TtaSetMainLoop (ExternalInitMainLoop init,
                                    ExternalMainLoop loop,
                                    ExternalFetchEvent fetch,
				    ExternalFetchAvailableEvent fetchavail);


/*----------------------------------------------------------------------
   TtaAddButton

   Adds a new button entry in a document view.
   This function must specify a valid view of a valid document.

   Parameters:
   document: the concerned document.
   view: the concerned view.
   picture: label of the new entry. None creates a space between buttons.
   procedure: procedure to be executed when the new entry is
   selected by the user. Null creates a cascade button.
   info: text to display when the cursor stays on the button.
   Returns index
  ----------------------------------------------------------------------*/
extern int          TtaAddButton (Document document, View view, Pixmap icon, void (*procedure) (), char *info);

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
extern void         TtaSwitchButton (Document document, View view, int index);

/*----------------------------------------------------------------------
   TtaChangeButton

   Change the button entry in a document view.
   This function must specify a valid view of a valid document.

   Parameters:
   document: the concerned document.
   view: the concerned view.
   index: the index.
   picture: the new icon.
  ----------------------------------------------------------------------*/
extern void         TtaChangeButton (Document document, View view, int index, Pixmap picture);

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
  ----------------------------------------------------------------------*/
extern int          TtaAddTextZone (Document document, View view, char *label, boolean editable, void (*procedure) ());

/*----------------------------------------------------------------------
   TtaSetTextZone

   Sets the text in text-zone in a document view.
   This function must specify a valid view of a valid document.

   Parameters:
   document: identifier of the document.
   view: identifier of the view.
   index: 

  ----------------------------------------------------------------------*/
extern void         TtaSetTextZone (Document document, View view, int index, char *text);

/*----------------------------------------------------------------------
   TtaSetMenuOff desactive le menu (1 a n) de la vue du document ou   
   de la fenetre principale (document = 0, view = 0).                 
  ----------------------------------------------------------------------*/
extern void         TtaSetMenuOff (Document document, View view, int menuID);

/*----------------------------------------------------------------------
   TtaSetMenuOn reactive le menu (1 a n) de la vue du document ou     
   de la fenetre principale (document = 0, view = 0).                 
  ----------------------------------------------------------------------*/
extern void         TtaSetMenuOn (Document document, View view, int menuID);

/*----------------------------------------------------------------------
   TtaSetToggleItem positionne l'item du menu de la vue du document   
   ou de la fenetre principale (document = 0, view = 0).   
  ----------------------------------------------------------------------*/
extern void         TtaSetToggleItem (Document document, View view, int menuID, int itemID, boolean on);

/*----------------------------------------------------------------------
   TtaSetItemOff desactive l'item actionName de la vue du document  
   ou de la fenetre principale (document = 0, view = 0).   
  ----------------------------------------------------------------------*/
extern void         TtaSetItemOff (Document document, View view, int menuID, int itemID);

/*----------------------------------------------------------------------
   TtaSetItemOn active l'item actionName de la vue du document      
   ou de la fenetre principale (document = 0, view = 0).   
  ----------------------------------------------------------------------*/
extern void         TtaSetItemOn (Document document, View view, int menuID, int itemID);

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
extern int          TtaSetCallback (void (*callbakProcedure) (), int set);

/*----------------------------------------------------------------------
   TtaSetStatus affiche le status de la vue du document.                      
  ----------------------------------------------------------------------*/
extern void         TtaSetStatus (Document document, View view, CONST char *text, CONST char *name);

/*----------------------------------------------------------------------
   TtaGetViewFrame retourne le widget du frame de la vue document.    
  ----------------------------------------------------------------------*/
extern ThotWidget   TtaGetViewFrame (Document document, View view);

#ifndef _WINDOWS
/*----------------------------------------------------------------------
   TtaFetchOneEvent

   retrieve one X-Windows Event from the queue, this is a blocking call.
  ----------------------------------------------------------------------*/
extern void         TtaFetchOneEvent (ThotEvent *ev);
extern void         TtaHandleOneEvent (ThotEvent * ev);
extern int          TtaXLookupString (ThotKeyEvent * event, char *buffer, int nbytes,
				  KeySym * keysym, ThotComposeStatus * status);

#endif /* !_WINDOWS */
extern void         TtaMainLoop (void);

/*----------------------------------------------------------------------
   * TtaHandlePendingEvents
 *
   * Processes all pending events in an application.
  ----------------------------------------------------------------------*/
extern void         TtaHandlePendingEvents ();

/*----------------------------------------------------------------------
   TtaClickElement

   Returns document and element clicked.

  ----------------------------------------------------------------------*/
extern void         TtaClickElement (/*OUT*/ Document *document, /*OUT*/ Element *element);
extern Pixmap       TtaCreatePixmapLogo (char **data);

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
extern Pixmap       TtaCreateBitmapLogo (int width, int height, char *bits);
extern void         TtaSetCursorWatch (Document document, View view);
extern void         TtaResetCursor (Document document, View view);

/*----------------------------------------------------------------------
   TtaGetMenuColor

   Returns the color used for the background of dialogue windows.
  ----------------------------------------------------------------------*/
extern Pixel        TtaGetMenuColor (void);

/*----------------------------------------------------------------------
   TtaGetButtonColor

   Returns the color used for displaying active buttons in forms.

  ----------------------------------------------------------------------*/
extern Pixel        TtaGetButtonColor (void);

/*----------------------------------------------------------------------
   TtaGetScreenDepth

   Returns the current screen depth.

  ----------------------------------------------------------------------*/
extern int          TtaGetScreenDepth (void);

#ifndef _WINDOWS
extern Display     *TtaGetCurrentDisplay (void);

#endif /* !_WINDOWS */

/*----------------------------------------------------------------------
   TtaGiveSelectPosition: returns the mouse position for the last click 
   with respect to the element (position in pixel)            
  ----------------------------------------------------------------------*/
extern void         TtaGiveSelectPosition (Document document, Element element, View view, /*OUT*/ int *X, /*OUT*/ int *Y);

/*----------------------------------------------------------------------
   TtaRegisterPixmap

   Register a pre-loaded Pixmap into the database.

   Parameters:
   name : the name of the image.
   pix : pointer to the existing pixmap.

   Return value:
   none

  ----------------------------------------------------------------------*/
extern void         TtaRegisterPixmap ( char *name, Pixmap pix );

/*----------------------------------------------------------------------
   TtaLoadImage

   Load an image from disk into the database.

   Parameters:
   name : the name of the image.
   path : the full pathname referencing the image file.

   Return value:
   The created pixmap

  ----------------------------------------------------------------------*/
extern Pixmap       TtaLoadImage ( char *name, char *path );

/*----------------------------------------------------------------------
   TtaGetImage

   Get an image from the database.

   Parameters:
   name : the name of the image.

   Return value:
   The pixmap or NULL if not found.

  ----------------------------------------------------------------------*/
extern Pixmap       TtaGetImage ( char *name ); 

#else  /* __STDC__ */

extern void         TtaSetMainLoop ( /* ExternalInitMainLoop init,
                                    ExternalMainLoop loop,
                                    ExternalFetchEvent fetch,
				    ExternalFetchAvailableEvent fetchavail */ );

extern int          TtaAddButton ( /*Document document, View view, Pixmap icon, void (*procedure) (), char *info */ );
extern void        *TtaGetButtonCallback ( /*Document document, View view, int index */);
extern void         TtaSwitchButton ( /*Document document, View view, int index */ );
extern void         TtaChangeButton ( /*Document document, View view, int index, Pixmap picture */ );
extern int          TtaAddTextZone ( /*Document document, View view, char *label, boolean editable, void (*procedure) () */ );
extern void         TtaSetTextZone ( /*Document document, View view, int index, char *text */ );
extern void         TtaSetMenuOff ( /*Document document, View view, int menuID */ );
extern void         TtaSetMenuOn ( /*Document document, View view, int menuID */ );
extern void         TtaSetToggleItem ( /*Document document, View view, int menuID, int itemID, boolean on */ );
extern void         TtaSetItemOff ( /*Document document, View view, int menuID, int itemID */ );
extern void         TtaSetItemOn ( /*Document document, View view, int menuID, int itemID */ );
extern int          TtaSetCallback ( /*void (*callbakProcedure) (), int set */ );
extern void         TtaSetStatus ( /*Document document, View view, CONST char *text, CONST char *name */ );

#ifndef _WINDOWS
extern void         TtaFetchOneEvent ( /*ThotEvent *ev */ );
extern void         TtaHandleOneEvent ( /*ThotEvent *ev */ );
extern int          TtaXLookupString (	/* ThotKeyEvent *event, char *buffer, int nbytes,
					   KeySym *keysym, ThotComposeStatus *status */ );

#endif /* !_WINDOWS */
extern void         TtaMainLoop ( /* void */ );
extern void         TtaHandlePendingEvents ();
extern void         TtaClickElement ( /*Document *document, Element *element */ );
extern Pixmap       TtaCreatePixmapLogo ( /*char **data */ );
extern Pixmap       TtaCreateBitmapLogo ( /*int width, int height, char *bits */ );
extern void         TtaSetCursorWatch ( /*Document document, View view */ );
extern void         TtaResetCursor ( /*Document document, View view */ );
extern Pixel        TtaGetMenuColor ( /* void */ );
extern Pixel        TtaGetButtonColor ( /* void */ );
extern int          TtaGetScreenDepth ( /* void */ );
extern Display     *TtaGetCurrentDisplay ( /* void */ );
extern void         TtaGiveSelectPosition ( /* Document document, Element element, View view, int *X, int *Y */ );
extern void         TtaRegisterPixmap (/* char *name, Pixmap pix */);
extern Pixmap       TtaLoadImage (/* char *name, char *path */);
extern Pixmap       TtaGetImage (/* char *name */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */

#endif
