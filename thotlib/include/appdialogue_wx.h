#ifdef _WX

#ifndef __APPDIALOGUE_WX_H__
#define __APPDIALOGUE_WX_H__

#include "constmenu.h"

extern void          ThotCallback ( int ref, int typedata, char *data );
extern wxString      TtaConvMessageToWX ( const char * p_message );
extern int           TtaGetWindowNumber( );
extern AmayaWindow * TtaGetWindowFromId( int window_id );
extern void          TtaCloseWindow( int window_id );
extern int           TtaGetFrameId( int window_id, int page_id, int position );
extern int           TtaGetFrameDocumentId( int frame_id );
extern ThotBool      TtaRegisterWidgetWX( int ref, void * p_widget );

/* constants used to identify different type of ressources */
typedef enum
  {
    WX_RESOURCES_ICON,
    WX_RESOURCES_XRC
  } wxResourceType;

extern wxString      TtaGetResourcePathWX( wxResourceType type, const char * filename );

extern int TtaMakeWindow(int x, int y, int w, int h, int kind );
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


#endif /* #define __APPDIALOGUE_WX_H__ */

#endif /* _WX */
