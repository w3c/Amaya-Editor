/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2001.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * Definition for picture management
 */

#ifdef _WINDOWS
#include <windows.h>
#endif /* _WINDOWS */

#ifndef _PICTURE_h
#define _PICTURE_h

#define XBM_FORMAT      	0     /* X11 BitmapFile format */
#define EPS_FORMAT        	1     /* Postscript  */
#define XPM_FORMAT      	2     /* Xpm XReadFileToPixmap format */
#define GIF_FORMAT        	3     /* gif */
#define PNG_FORMAT              4     /* Png */
#define JPEG_FORMAT             5     /* Jpeg */
#define SVG_FORMAT              6     /* SVG Images */
#define PLUGIN_FORMAT           7     /* Plugin files format */
#define MAX_PICT_FORMATS        30

/* A few constants */

#define MAX_FORMAT_NAMELENGHT  32       
#define NULLIMAGEDRVR          255
#define NOERROR                1
#define MAX_PICT_SIZE          32768

#define XbmName  "Bitmap (.xbm)"
#define EpsName  "EPSF (.eps)"
#define GifName  "Gif (.gif)"
#define JpegName "Jpeg (.jpg)"
#define XpmName  "Pixmap (.xpm)"
#define PngName  "Ping (.png)"

typedef struct 
{
  int            PicXArea;
  int            PicYArea;
  int            PicWArea;
  int            PicHArea;
  int            PicWidth;
  int            PicHeight;
  PictureScaling PicPresent;
  int            PicType;
  char          *PicFileName; 
#ifndef _GL
#ifdef _GTK
  GdkImlibImage     *im;
  GdkPixmap      *PicPixmap;
  Pixmap         PicAlpha;
#else /* _GTK*/
  Pixmap         PicPixmap;
  Pixmap         PicAlpha;
#endif /*_GTK*/
#ifdef _WINDOWS
  int            PicMask;        /* Mask color */
#else /* _WINDOWS */
  Pixmap         PicMask;        /* Mask bitmap */
#endif /* _WINDOWS */
#else /*_GL*/
  unsigned int         TextureBind;/* integer name of video card memory pointer*/
  unsigned char        *PicPixmap;
  ThotBool             RGBA;
#endif /* _GL */
  ThotBool       mapped;         /* Used for ExtraHandlers */
  ThotBool       created;        /* Used for ExtraHandlers */
  ThotWidget     wid;            /* Used for ExtraHandlers */
  void          *pluginInstance; /* Used for ExtraHandlers */
} PictInfo;

typedef enum
{
  Supported_Format,
  Corrupted_File,
  Unsupported_Format
} Picture_Report;


typedef struct
{
  char     GUI_Name[MAX_FORMAT_NAMELENGHT];
  Drawable (*Produce_Picture) ();
  void     (*Produce_Postscript) ();
  ThotBool (*Match_Format) ();
  void     (*DrawPicture) ();
  void     (*FreePicture)();
} PictureHandler;

#endif /* _PICTURE_h */


