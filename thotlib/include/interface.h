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

extern int          TtaAddButton (Document document, View view, Pixmap icon, void (*procedure) (), char *info);
extern void         TtaSwitchButton (Document document, View view, int index);
extern void         TtaChangeButton (Document document, View view, int index, Pixmap picture);
extern int          TtaAddTextZone (Document document, View view, char *label, boolean editable, void (*procedure) ());
extern void         TtaSetTextZone (Document document, View view, int index, char *text);
extern void         TtaSetMenuOff (Document document, View view, int menuID);
extern void         TtaSetMenuOn (Document document, View view, int menuID);
extern void         TtaSetToggleItem (Document document, View view, int menuID, int itemID, boolean on);
extern void         TtaSetActionOff (Document document, View view, int menuID, int itemID);
extern void         TtaSetActionOn (Document document, View view, int menuID, int itemID);
extern int          TtaSetCallback (void (*callbakProcedure) (), int set);
extern void         TtaSetStatus (Document document, View view, char *text, char *name);
extern ThotWidget   TtaGetViewFrame (Document document, View view);

#ifndef _WINDOWS
extern void         TtaHandleOneEvent (ThotEvent * ev);
extern int          TtaXLookupString (ThotKeyEvent * event, char *buffer, int nbytes,
				  KeySym * keysym, ThotComposeStatus * status);

#endif /* !_WINDOWS */
extern void         TtaMainLoop (void);
extern void         TtaHandlePendingEvents ();
extern void         TtaClickElement (Document * document, Element * element);
extern Pixmap       TtaCreatePixmapLogo (char **data);
extern Pixmap       TtaCreateBitmapLogo (int width, int height, char *bits);
extern void         TtaSetCursorWatch (Document document, View view);
extern void         TtaResetCursor (Document document, View view);
extern Pixel        TtaGetMenuColor (void);
extern Pixel        TtaGetButtonColor (void);
extern int          TtaGetScreenDepth (void);

#ifndef _WINDOWS
extern Display     *TtaGetCurrentDisplay (void);

#endif /* !_WINDOWS */
extern void         TtaGiveSelectPosition (Document document, Element element, View view, int *X, int *Y);

#else  /* __STDC__ */

extern void         TtaSetMainLoop ( /* ExternalInitMainLoop init,
                                    ExternalMainLoop loop,
                                    ExternalFetchEvent fetch,
				    ExternalFetchAvailableEvent fetchavail */ );

extern int          TtaAddButton ( /*Document document, View view, Pixmap icon, void (*procedure) (), char *info */ );
extern void         TtaSwitchButton ( /*Document document, View view, int index */ );
extern void         TtaChangeButton ( /*Document document, View view, int index, Pixmap picture */ );
extern int          TtaAddTextZone ( /*Document document, View view, char *label, boolean editable, void (*procedure) () */ );
extern void         TtaSetTextZone ( /*Document document, View view, int index, char *text */ );
extern void         TtaSetMenuOff ( /*Document document, View view, int menuID */ );
extern void         TtaSetMenuOn ( /*Document document, View view, int menuID */ );
extern void         TtaSetToggleItem ( /*Document document, View view, int menuID, int itemID, boolean on */ );
extern void         TtaSetActionOff ( /*Document document, View view, int menuID, int itemID */ );
extern void         TtaSetActionOn ( /*Document document, View view, int menuID, int itemID */ );
extern int          TtaSetCallback ( /*void (*callbakProcedure) (), int set */ );
extern void         TtaSetStatus ( /*Document document, View view, char *text, char *name */ );

#ifndef _WINDOWS
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

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */

#endif
