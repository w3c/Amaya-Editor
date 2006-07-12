/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2005
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * Definition for picture management
 */

#ifdef _WINGUI
#include <windows.h>
#endif /* _WINGUI */
#ifndef _PICTURE_h
#define _PICTURE_h
#include "fileaccess.h"

/* A few constants */
#define MAX_FORMAT_NAMELENGHT  32       
#define NULLIMAGEDRVR          255
#define NOERROR                1
#define MAX_PICT_SIZE          32768
#define MAX_PICTURE_DRIVER     7

#define XbmName  "Bitmap (.xbm)"
#define EpsName  "EPSF (.eps)"
#define GifName  "Gif (.gif)"
#define JpegName "Jpeg (.jpg)"
#define XpmName  "Pixmap (.xpm)"
#define PngName  "Ping (.png)"

typedef struct 
{
  int            PicWArea;
  int            PicHArea;
  int            PicWidth;
  int            PicHeight;
  PictureScaling PicPresent;
  int            PicType;
  char          *PicFileName;
  ThotPixmap     PicPixmap;
  int            PicPosX;
  int            PicPosY;
  TypeUnit       PicXUnit;
  TypeUnit       PicYUnit;
  int            PicShiftX;
  int            PicShiftY;
#ifdef _GTK
  GdkImlibImage *im;
  ThotPixmap     PicMask;        /* Bitmap Mask */
#endif /*_GTK*/
#ifdef _WINGUI
  int            PicBgMask;      /* Color Mask */
  ThotPixmap     PicMask;        /* Bitmap Mask */
#endif /* _WINGUI */
#ifdef _GL
  unsigned int   TextureBind; /* integer name of video card memory pointer*/
  float          TexCoordW;
  float          TexCoordH;  
  ThotBool       RGBA;
#endif /* _GL */
} ThotPictInfo;

typedef enum
{
  Supported_Format,
  Corrupted_File,
  Unsupported_Format
} Picture_Report;

   /* add proto definitions for c++ compatibility */
#define PICHND_PROTO_Produce_Picture 	ThotDrawable (*) (void *,void *,void *,void *,void *,void *,void *,void *,void *,void *)
#define PICHND_PROTO_Produce_Postscript void (*) (void *,void *,void *,void *,void *,void *,void *,void *,void *,void *)
#define PICHND_PROTO_Match_Format 	ThotBool (*) (void *)
#define PICHND_PROTO_DrawPicture 	void (*) (void *,void *,void *,void *,void *,void *,void *)


typedef struct
{
  char     GUI_Name[MAX_FORMAT_NAMELENGHT];
  ThotDrawable (*Produce_Picture) (void *,void *,void *,void *,void *,void *,void *,void *,void *,void *);
  void     (*Produce_Postscript) (void *,void *,void *,void *,void *,void *,void *,void *,void *,void *);
  ThotBool (*Match_Format) (void *);
  void     (*DrawPicture) (void *,void *,void *,void *,void *,void *,void *);
  void     (*FreePicture)();
} PictureHandler;


void FreeAllPicCache ();
void FreeAllPicCacheFromFrame (int frame);

#endif /* _PICTURE_h */


