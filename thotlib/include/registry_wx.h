#ifndef __REGISTRY_WX_H__
#define __REGISTRY_WX_H__

#ifdef _WX
/* constants used to identify different type of ressources */
typedef enum
  {
    WX_RESOURCES_ICON,
    WX_RESOURCES_XRC
  } wxResourceType;

extern wxString TtaGetResourcePathWX( wxResourceType type, const char * filename );
#endif /* _WX */

#endif	/* __REGISTRY_WX_H__ */

