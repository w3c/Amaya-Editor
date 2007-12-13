#ifdef _WX

#ifndef __WINDOWTYPES_H__
#define __WINDOWTYPES_H__

/* use to differenciate AmayaWindow types */
typedef enum
  {
    WXAMAYAWINDOW_UNKNOWN,
    WXAMAYAWINDOW_NORMAL,
    WXAMAYAWINDOW_SIMPLE,
    WXAMAYAWINDOW_ANNOT,
    WXAMAYAWINDOW_CSS,
    WXAMAYAWINDOW_HELP
  } wxAMAYAWINDOW_KIND;

  
/* use to differenciate AmayaPage types */
typedef enum
{
  WXAMAYAPAGE_SIMPLE,
  WXAMAYAPAGE_SPLITTABLE
} wxAMAYAPAGE_TYPE;
  
#endif /* __WINDOWTYPES_H__ */

#endif /* _WX */
