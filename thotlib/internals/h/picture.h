/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
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
#define JPEG_FORMAT             4     /* Jpeg */
#define PNG_FORMAT              5     /* Png */
#define PLUGIN_FORMAT           6     /* Plugin files format */
#define MAX_PICT_FORMATS       30

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
  Pixmap         PicPixmap;
# ifndef _WINDOWS
  Pixmap         PicMask;
# else  /* _WINDOWS */
  int            bgRed;
  int            bgGreen;
  int            bgBlue;
# endif /* _WINDOWS */
  boolean        mapped;         /* Used for ExtraHandlers */
  boolean        created;        /* Used for ExtraHandlers */
  ThotWidget     wid;            /* Used for ExtraHandlers */
  void*          pluginInstance; /* Used for ExtraHandlers */
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
  boolean  (*Match_Format) ();
  void     (*DrawPicture) ();
  void     (*FreePicture)();
} PictureHandler;

#endif /* _PICTURE_h */


