/*
 * Definition for picture management
*/


#ifndef _PICTURE_h
#define _PICTURE_h

#define XBM_FORMAT      	0     /* X11 BitmapFile format */
#define EPS_FORMAT        	1     /* Postscript  */
#define XPM_FORMAT      	2     /* Xpm XReadFileToPixmap format */
#define GIF_FORMAT        	3     /* gif */
#define JPEG_FORMAT             4     /* Jpeg */
#define PNG_FORMAT              5     /* Png */
#define MAX_PICT_FORMATS        6

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
  int PicXArea,  PicYArea;
  int PicWArea,  PicHArea;
  PictureScaling PicPresent;
  int            PicType;
  char          *PicFileName; 
  Pixmap         PicPixmap;
  Pixmap         PicMask;
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
} PictureHandler;




#endif


