
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
extern char *        ColorName ( int num );

extern int           TtaGetWindowNumber( );
extern AmayaWindow * TtaGetWindowFromId( int window_id );
extern AmayaFrame *  TtaGetFrameFromId( int frame_id );
extern void          TtaCloseWindow( int window_id );
extern ThotBool      TtaClosePage( int window_id, int page_id );
extern void          TtaCleanUpWindow( int window_id );
extern int           TtaGetFrameId( int window_id, int page_id, int position );
extern int           TtaGetFrameDocumentId( int frame_id );
extern ThotBool      TtaRegisterWidgetWX( int ref, void * p_widget );
extern int           TtaGetActiveWindowId();
extern AmayaWindow * TtaGetActiveWindow();
extern wxMenu *      TtaGetContextMenu( int window_id, int page_id, int frame_id );
extern void          TtaRefreshMenuStats( wxMenuBar * p_menu_bar );

extern int TtaMakeWindow(int x, int y, int w, int h, int kind, int parent_window_id );
extern int TtaMakeFrame( const char * schema_name,
			 int schView,
			 Document doc_id,
			 const char * doc_name,
			 int width,
			 int height,
			 int * volume );
extern int TtaGetFreePageId( int window_id );
extern int TtaAddToolBarButton( int window_id,
				ThotIcon picture,
				char * tooltip,
				char * functionName,
				void (*procedure) (),
				ThotBool status );
extern void TtaSetupPanel( int type,
			   int button_id,
			   const char * tooltip,
			   void (*procedure) () );

extern void TtaSwitchPanelButton( Document doc, View view,
				  int panel_type,
				  int button_id,
				  ThotBool value );

extern void TtaRefreshPanelTooltips( int window_id );
extern void TtaToggleOnOffSidePanel( int frame_id );
extern void TtaToggleOnOffSplitView( int frame_id );
extern void TtaToggleOnOffFullScreen( int frame_id );
extern int GetWindowNumber (Document doc, View view);
extern void TtaRegisterOpenURLCallback( void (*callback) (void *) );

#include "AmayaParams.h"
extern void TtaSendDataToPanel( int panel_type, AmayaParams& params );

extern ThotBool TtaHandleUnicodeKey( wxKeyEvent& event );
extern ThotBool TtaHandleShortcutKey( wxKeyEvent& event );
extern ThotBool TtaHandleSpecialKey( wxKeyEvent& event );
extern ThotBool TtaIsSpecialKey( int wx_keycode );

#endif /* #define __APPDIALOGUE_WX_H__ */

#endif /* _WX */
