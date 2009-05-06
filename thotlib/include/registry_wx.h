#ifndef __REGISTRY_WX_H__
#define __REGISTRY_WX_H__

#ifdef _WX
/* constants used to identify different type of ressources */
typedef enum
  {
    WX_RESOURCES_ICON_16X16,
    WX_RESOURCES_ICON_22X22,
    WX_RESOURCES_ICON_MISC,
    WX_RESOURCES_XRC,
    WX_RESOURCES_SVG
  } wxResourceType;

extern wxString TtaGetResourcePathWX( wxResourceType type, const char * filename );
extern wxString TtaGetHomeDir();
extern int TtaGetSystemLanguage( );
extern char *TtaGetDocumentsDir();
#endif /* _WX */

#endif	/* __REGISTRY_WX_H__ */

