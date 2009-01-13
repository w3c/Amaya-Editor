
#ifndef __APPDIALOGUE_WX_H__
#define __APPDIALOGUE_WX_H__

#include "constmenu.h"

#ifndef _WINGUI
/* amaya need to know this function in order to launch callbacks */
extern void          ThotCallback ( int ref, int typedata, char *data );
#endif /* _WINGUI */

#ifdef _WX
/* amaya need to know thotlib color list in order to setup the dialogs */
extern int           NumberOfColors ( void );
extern const char *  ColorName ( int num );

extern int           TtaGetFrameWindowParentId( int frame_id );
extern int           TtaGetWindowNumber( );
extern AmayaWindow * TtaGetWindowFromId( int window_id );
extern AmayaFrame *  TtaGetFrameFromId( int frame_id );
extern void          TtaCloseWindow( int window_id );
extern void          TtaEmptyURLBar( int window_id );
extern ThotBool      TtaClosePage( int window_id, int page_id );
extern ThotBool      TtaCloseAllPageButThis( int window_id, int page_id );
extern void          TtaCleanUpWindow( int window_id );
extern int           TtaGetFrameId( int window_id, int page_id, int position );
extern int           TtaGetFrameDocumentId( int frame_id );
extern ThotBool      TtaRegisterWidgetWX( int ref, void * p_widget );
extern int           TtaGetActiveWindowId();
extern AmayaWindow * TtaGetActiveWindow();
extern wxMenu *      TtaGetContextMenu( int window_id );
extern wxMenu *      TtaGetDocContextMenu( int window_id );

extern int TtaMakeWindow(int x, int y, int w, int h, int kind, int parent_window_id );
extern int TtaMakeFrame( const char * schema_name, int schView,
                         Document doc_id, const char * doc_name, int width,
                         int height, int * volume );
extern void TtaSetPageIcon( Document doc, View view, char *iconpath);
extern void TtaShowWindow( int window_id, ThotBool show );
extern int TtaGetFreePageId( int window_id );
extern void TtaSwitchPanelButton( Document doc, View view,
				  int panel_type,
				  int button_id,
				  ThotBool value );

extern void TtaToggleOnOffSidePanel( int frame_id );
extern void TtaToggleToolbar( int frame_id, int toolbar_id );
extern void TtaToggleOnOffFullScreen( int frame_id );
extern ThotBool TtaGetFullScreenState(int frame_id );
extern void TtaSplitViewVertically( int frame_id );
extern void TtaSplitViewHorizontally( int frame_id );
extern int GetWindowNumber (Document doc, View view);
extern void TtaRegisterOpenURLCallback( void (*callback) (void *) );

extern void TtaInitTopMenuStats( int doc_id );
extern void TtaInitMenuItemStats( int doc_id );

#include "AmayaParams.h"
extern void TtaSendDataToPanel( int panel_type, AmayaParams& params );

extern ThotBool TtaFrameIsShown (int frame);
extern ThotBool TtaHandleUnicodeKey( wxKeyEvent& event );
extern ThotBool TtaHandleShortcutKey( wxKeyEvent& event );
extern ThotBool TtaHandleSpecialKey( wxKeyEvent& event );
extern ThotBool TtaIsSpecialKey( int wx_keycode );

extern void TtaSendStatsInfo();


/* Function prototype to show a context menu.*/
typedef int(*PopupDocContextMenuFuction)(int document, int view, int window, void* win, int x, int y);

void TtaSetPopupDocContextMenuFunction(PopupDocContextMenuFuction fn);
void TtaPopupDocContextMenu(int document, int view, int window, void* win, int x, int y);

#endif /* _WX */

extern void TtaCheckLostFocus();
extern void TtaRedirectFocus();

void TtaResetEnumContextMenu();
void TtaSetEnumContextMenu(int res);
int TtaGetEnumContextMenu();


typedef int(*DocumentPageTypeFunction)(Document doc);
void TtaRegisterDocumentPageTypeFunction(DocumentPageTypeFunction fct);
int TtaGetDocumentPageType(Document doc);

void TtaRaiseDoctypePanels(int doctype);

#endif /* #define __APPDIALOGUE_WX_H__ */
