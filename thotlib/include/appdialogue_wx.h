#ifdef _WX

#ifndef __APPDIALOGUE_WX_H__
#define __APPDIALOGUE_WX_H__

#include "constmenu.h"

extern void          ThotCallback ( int ref, int typedata, char *data );
extern wxString      TtaConvMessageToWX ( const char * p_message );
extern int           TtaGetWindowNumber( );
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

#endif /* #define __APPDIALOGUE_WX_H__ */

#endif /* _WX */
