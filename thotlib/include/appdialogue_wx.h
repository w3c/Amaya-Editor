
#ifdef _WX

#include "constmenu.h"

extern void          ThotCallback ( int ref, int typedata, char *data );
extern wxString      TtaConvMessageToWX ( const char * p_message );
extern int           TtaGetWindowNumber( );
extern void          TtaCloseWindow( int window_id );


/* constants used to identify different type of ressources */
typedef enum
  {
    WX_RESOURCES_ICON,
    WX_RESOURCES_XRC
  } wxResourceType;

extern wxString      TtaGetResourcePathWX( wxResourceType type, const char * filename );

#endif /* _WX */
