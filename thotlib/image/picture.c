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
 * Picture Handling
 * Authors: I. Vatton (INRIA)
 *          N. Layaida (INRIA) - New picture formats
 *          R. Guetari (INRIA) - Plugins and Windows
 *
 * Last modification: Jan 09 1997
 */

#include "thot_gui.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "lost.xpm"
#include "picture.h"
#include "frame.h"
#include "epsflogo.h"
#include "interface.h"
#include "fileaccess.h"
#include "thotdir.h"
#include "png.h"
#include "fileaccess.h"
#define THOT_EXPORT extern
#include "boxes_tv.h"
#include "frame_tv.h"
#include "font_tv.h"
#include "platform_tv.h"
#include "picture_tv.h"

#include "appli_f.h"
#include "tree_f.h"
#include "views_f.h"
#include "platform_f.h"
#include "font_f.h"
#include "frame_f.h"
#include "picture_f.h"
#include "presrules_f.h"
#include "inites_f.h"
#include "gifhandler_f.h"
#include "jpeghandler_f.h"
#include "xbmhandler_f.h"
#include "xpmhandler_f.h"
#include "pnghandler_f.h"
#include "epshandler_f.h"
#include "fileaccess_f.h"

PictureHandler  PictureHandlerTable[MAX_PICT_FORMATS];
int             PictureIdType[MAX_PICT_FORMATS];
int             PictureMenuType[MAX_PICT_FORMATS];
int             InlineHandlers;
int             HandlersCounter;
int             currentExtraHandler;
boolean         Printing;
ThotGC          GCpicture;
THOT_VInfo      THOT_vInfo;
Pixmap          EpsfPictureLogo;

static char    *PictureMenu;
static Pixmap   PictureLogo;

#ifndef _WINDOWS
XVisualInfo*    vptr;
Visual*         theVisual;
#endif

char* FileExtension[] = {
      ".xbm", ".eps", ".xpm", ".gif", ".jpg", ".png"
};

static unsigned char MirrorBytes[0x100] = {
   0x00, 0x80, 0x40, 0xc0, 0x20, 0xa0, 0x60, 0xe0,
   0x10, 0x90, 0x50, 0xd0, 0x30, 0xb0, 0x70, 0xf0,
   0x08, 0x88, 0x48, 0xc8, 0x28, 0xa8, 0x68, 0xe8,
   0x18, 0x98, 0x58, 0xd8, 0x38, 0xb8, 0x78, 0xf8,
   0x04, 0x84, 0x44, 0xc4, 0x24, 0xa4, 0x64, 0xe4,
   0x14, 0x94, 0x54, 0xd4, 0x34, 0xb4, 0x74, 0xf4,
   0x0c, 0x8c, 0x4c, 0xcc, 0x2c, 0xac, 0x6c, 0xec,
   0x1c, 0x9c, 0x5c, 0xdc, 0x3c, 0xbc, 0x7c, 0xfc,
   0x02, 0x82, 0x42, 0xc2, 0x22, 0xa2, 0x62, 0xe2,
   0x12, 0x92, 0x52, 0xd2, 0x32, 0xb2, 0x72, 0xf2,
   0x0a, 0x8a, 0x4a, 0xca, 0x2a, 0xaa, 0x6a, 0xea,
   0x1a, 0x9a, 0x5a, 0xda, 0x3a, 0xba, 0x7a, 0xfa,
   0x06, 0x86, 0x46, 0xc6, 0x26, 0xa6, 0x66, 0xe6,
   0x16, 0x96, 0x56, 0xd6, 0x36, 0xb6, 0x76, 0xf6,
   0x0e, 0x8e, 0x4e, 0xce, 0x2e, 0xae, 0x6e, 0xee,
   0x1e, 0x9e, 0x5e, 0xde, 0x3e, 0xbe, 0x7e, 0xfe,
   0x01, 0x81, 0x41, 0xc1, 0x21, 0xa1, 0x61, 0xe1,
   0x11, 0x91, 0x51, 0xd1, 0x31, 0xb1, 0x71, 0xf1,
   0x09, 0x89, 0x49, 0xc9, 0x29, 0xa9, 0x69, 0xe9,
   0x19, 0x99, 0x59, 0xd9, 0x39, 0xb9, 0x79, 0xf9,
   0x05, 0x85, 0x45, 0xc5, 0x25, 0xa5, 0x65, 0xe5,
   0x15, 0x95, 0x55, 0xd5, 0x35, 0xb5, 0x75, 0xf5,
   0x0d, 0x8d, 0x4d, 0xcd, 0x2d, 0xad, 0x6d, 0xed,
   0x1d, 0x9d, 0x5d, 0xdd, 0x3d, 0xbd, 0x7d, 0xfd,
   0x03, 0x83, 0x43, 0xc3, 0x23, 0xa3, 0x63, 0xe3,
   0x13, 0x93, 0x53, 0xd3, 0x33, 0xb3, 0x73, 0xf3,
   0x0b, 0x8b, 0x4b, 0xcb, 0x2b, 0xab, 0x6b, 0xeb,
   0x1b, 0x9b, 0x5b, 0xdb, 0x3b, 0xbb, 0x7b, 0xfb,
   0x07, 0x87, 0x47, 0xc7, 0x27, 0xa7, 0x67, 0xe7,
   0x17, 0x97, 0x57, 0xd7, 0x37, 0xb7, 0x77, 0xf7,
   0x0f, 0x8f, 0x4f, 0xcf, 0x2f, 0xaf, 0x6f, 0xef,
   0x1f, 0x9f, 0x5f, 0xdf, 0x3f, 0xbf, 0x7f, 0xff
};

/*----------------------------------------------------------------------
   Match_Format returns TRUE if the considered header file matches   
   the image file description, FALSE in the the other cases        
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static boolean      Match_Format (int typeImage, char *fileName)
#else  /* __STDC__ */
static boolean      Match_Format (typeImage, fileName)
int                 typeImage;
char               *fileName;

#endif /* __STDC__ */
{
   if (PictureHandlerTable[typeImage].Match_Format != NULL)
      return (*(PictureHandlerTable[typeImage].Match_Format)) (fileName);
   return FALSE;
}


/*----------------------------------------------------------------------
   FreePixmap frees the pixmap allocated in the X server if it is not
   empty and if it is not one of the internal images        
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         FreePixmap (Pixmap pix)
#else  /* __STDC__ */
static void         FreePixmap (pix)
Pixmap              pix;

#endif /* __STDC__ */
{
#ifndef _WINDOWS
   if ((pix != None) && (pix != PictureLogo) && (pix != EpsfPictureLogo))
      XFreePixmap (TtDisplay, pix);
#endif /* _WINDOWS */
}

/*----------------------------------------------------------------------
   UpdatePictInfo updates the picture information structure by      
   setting the picture and the mask                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         UpdatePictInfo (PictInfo * imdesc, Pixmap pix, Pixmap PicMask)
#else  /* __STDC__ */
static void         UpdatePictInfo (imdesc, pix, PicMask)
PictInfo           *imdesc;
Pixmap              pix;
Pixmap              PicMask;

#endif /* __STDC__ */
{
   FreePixmap (imdesc->PicPixmap);
   FreePixmap (imdesc->PicMask);
   imdesc->PicPixmap = pix;
   imdesc->PicMask = PicMask;
}


/*----------------------------------------------------------------------
   Picture_Center updates the parameters xtranslate, ytranslate,     
   pxorig, pyorig depending on the values of PicWArea,     
   PicHArea, wif, hif and pres.                            
   - If we use ReScale, the tranlation is performed        
   in one direction.                                       
   - If we use FillFrame, there's no translation           
   - if we use RealSize we translate to achieve            
   the centering               .                           
   if the picture size is greater than the frame then      
   pxorig or pyorig are positive.                            
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void Picture_Center (int wimage, int himage, int wbox, int hbox, PictureScaling pres, int *xtranslate, int *ytranslate, int *pxorig, int *pyorig)
#else  /* __STDC__ */
static void Picture_Center (wimage, himage, wbox, hbox, pres, xtranslate, ytranslate, pxorig, pyorig)
int                 wimage;
int                 himage;
int                 wbox;
int                 hbox;
PictureScaling      pres;
int                *xtranslate;
int                *ytranslate;
int                *pxorig;
int                *pyorig;

#endif /* __STDC__ */
{
   float               Rapw, Raph;

   /* the box has the wbox, hbox dimensions */
   /* the picture has  wimage, himage size */

   *pxorig = 0;
   *pyorig = 0;
   *xtranslate = 0;
   *ytranslate = 0;

   switch (pres)
	 {
	    case ReScale:
	       Rapw = (float) wbox / (float) wimage;
	       Raph = (float) hbox / (float) himage;
	       if (Rapw <= Raph)
		  *ytranslate = (hbox - (himage * Rapw)) / 2;
	       else
		  *xtranslate = (wbox - (wimage * Raph)) / 2;
	       break;

	    case FillFrame:
	       /* the picture covers all the box space */
	       break;

	    case RealSize:
	       /* we center the picture in the box frame */
	       *xtranslate = (wbox - wimage) / 2;
	       *ytranslate = (hbox - himage) / 2;
	       break;
	 }
   if (wimage > wbox)
     {
	*pxorig = -*xtranslate;
	*xtranslate = 0;
     }
   if (himage > hbox)
     {
	*pyorig = -*ytranslate;
	*ytranslate = 0;
     }
}

/*----------------------------------------------------------------------
   SetPictureClipping clips the picture into boundaries.              
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         SetPictureClipping (int *PicWArea, int *PicHArea, int wif, int hif, PictInfo * imageDesc)
#else  /* __STDC__ */
static void         SetPictureClipping (PicWArea, PicHArea, wif, hif, imageDesc)
int                *PicWArea;
int                *PicHArea;
int                 wif;
int                 hif;
PictInfo           *imageDesc;

#endif /* __STDC__ */
{
   if (((imageDesc->PicWArea == 0) && (imageDesc->PicHArea == 0)) ||
       ((imageDesc->PicWArea > MAX_PICT_SIZE) ||
	(imageDesc->PicHArea > MAX_PICT_SIZE)))
     {
	*PicWArea = wif;
	*PicHArea = hif;
     }
   else
     {
	*PicWArea = imageDesc->PicWArea;
	*PicHArea = imageDesc->PicHArea;
     }
}

/*----------------------------------------------------------------------
   LayoutPicture performs the layout of SrcPix on the screen described 
   by Drawab.                                                          
   if srcorx or srcory are postive, the copy operation is shifted      
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         LayoutPicture (Pixmap SrcPix, Drawable Drawab, int srcorx, int srcory, int w, int h, int desorx, int desory)
#else  /* __STDC__ */
static void         LayoutPicture (SrcPix, Drawab, srcorx, srcory, w, h, desorx, desory)
Pixmap              SrcPix;
Drawable            Drawab;
int                 srcorx;
int                 srcory;
int                 w;
int                 h;
int                 desorx;
int                 desory;

#endif /* __STDC__ */
{

   if (srcorx < 0)
     {
	desorx = desorx - srcorx;
	srcorx = 0;
     }
   if (srcory < 0)
     {
	desory = desory - srcory;
	srcory = 0;
     }

#ifndef _WINDOWS
   if (SrcPix != None)
      XCopyArea (TtDisplay, SrcPix, Drawab, TtGraphicGC, srcorx, srcory, w, h, desorx, desory);

#ifdef IV
    XGCValues values;
    GC gc_bg;
    unsigned int valuemask;

   if (bg_style->flag & S_BACKGROUND_IMAGE) /* background image */
     {
       if (bg_style->image)
	 {
	   gc_bg = XCreateGC (TtDisplay, Drawab, 0, NULL);
	   XCopyGC (TtDisplay, gc_fill, 0xFFFF, gc_bg);
	   valuemask = GCTile|GCFillStyle|GCTileStipXOrigin|GCTileStipYOrigin;
	   values.fill_style = FillTiled;
	   values.tile = bg_style->image->pixmap;
	   if (bg_style->flag & S_BACKGROUND_ORIGIN)
	     {
	       if (bg_style->flag & S_BACKGROUND_FIXED)
		 {
		   values.ts_x_origin = bg_style->x_pos * (WinWidth-bg_style->image->width)/100;
		   values.ts_y_origin = bg_style->y_pos * (WinHeight-bg_style->image->height)/100 + WinTop;
		 }
	       else
		 {
		   values.ts_x_origin = -PixelIndent;
		   values.ts_y_origin = WinTop - PixelOffset;
		 }
	     }
	   XChangeGC (TtDisplay, gc_bg, valuemask, &values);
	   
	   if (bg_style->flag & S_BACKGROUND_Y_REPEAT)
	     {
	       frameRect.y = WinTop;
	       frameRect.height = WinHeight;
	     }
	   else
	     {
	       frameRect.y = values.ts_y_origin;
	       frameRect.height = bg_style->image->height;
	     }
	   if (bg_style->flag & S_BACKGROUND_X_REPEAT)
	     {
	       frameRect.x = WinLeft;
	       frameRect.width = WinWidth;
	     }
	   else
	     {
	       frameRect.x = values.ts_x_origin;
	       frameRect.width = bg_style->image->width;
	     }
	   XSetClipRectangles (TtDisplay, gc_bg, 0, 0,&frameRect, 1, Unsorted);
	   XFillRectangle (TtDisplay, Drawab, gc_bg, x, y, w, h);
	   XFreeGC (TtDisplay, gc_bg);
	 }
       else
	 if (!(bg_style->flag & (S_BACKGROUND_COLOR)))
	   /* use paper if no pix map and color */
	   XFillRectangle(TtDisplay, Drawab, gc_fill, x, y, w, h);
     }

#endif
#endif /* _WINDOWS */
}


/*----------------------------------------------------------------------
   IsValid retourne FALSE si le pixmap contenu dans imageDesc       
   est vide. On retourne TRUE s'il est egal aux images     
   predefinies BadPixmap.                                  
   - if we use RealSize, we  return TRUE.                  
   - if we use  ReScale, we return TRUE                    
   the box have one of the two  dimensions a least equals  
   to the one of the pixmap.                               
   - if we use  ReScale FillFrame, we return TRUE if the   
   frame box  has the same size than the pixmap in         
   both directions.                                        
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static boolean      IsValid (PtrBox box, PictInfo* imageDesc)
#else  /* __STDC__ */
static boolean      IsValid (box, imageDesc)
PtrBox              box;
PictInfo           *imageDesc;

#endif /* __STDC__ */
{
  boolean             pixmapok;
  int                 xpix, ypix, wpix, hpix, bdw, dep;
  Drawable            root;

  pixmapok = TRUE;
  if ((imageDesc->PicPixmap == PictureLogo)
      || (imageDesc->PicPixmap == EpsfPictureLogo))
    return TRUE;
  
  if (imageDesc->PicPixmap == None)
    return FALSE;
  else
    {
#ifndef _WINDOWS
      XGetGeometry (TtDisplay, imageDesc->PicPixmap, &root, &xpix, &ypix, &wpix, &hpix, &bdw, &dep);
#endif /* _WINDOWS */
      switch (imageDesc->PicPresent)
	{
	case RealSize:
	  pixmapok = TRUE;
	  break;
	case ReScale:
	  /*pixmapok = ((box->BxWidth == wpix) || (box->BxHeight == hpix));*/
	  break;
	case FillFrame:
	  /*pixmapok = ((box->BxWidth == wpix) && (box->BxHeight == hpix));*/
	  break;
	}
      return pixmapok;
    }
}


/*----------------------------------------------------------------------
   GetPictureFormat returns the format of a file picture           
   the file  fileName or UNKNOWN_FORMAT if not recognized          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static int          GetPictureFormat (char *fileName)
#else  /* __STDC__ */
static int          GetPictureFormat (fileName)
char               *fileName;

#endif /* __STDC__ */
{
  int                 i;
  int                 l = 0;

  i = 0 ;
  l = strlen (fileName);
  if (l > 4)
    {
      if (strcmp (fileName + l - 4, ".pic") == 0 || strcmp (fileName + l - 4, ".xbm") == 0)
	return XBM_FORMAT;
      if (strcmp (fileName + l - 4, ".eps") == 0 || strcmp (fileName + l - 3, ".ps") == 0)
	return EPS_FORMAT;
      if (strcmp (fileName + l - 4, ".xpm") == 0)
	return XPM_FORMAT;
      if ((strcmp (fileName + l - 4, ".gif") == 0) || (strcmp (fileName + l - 4, ".GIF") == 0))
	return GIF_FORMAT;
      if ((strcmp (fileName + l - 4, ".jpg") == 0) || (strcmp (fileName + l - 5, ".jpeg") == 0))
	return JPEG_FORMAT;
      if (strcmp (fileName + l - 4, ".png") == 0)
	return PNG_FORMAT;
    }

  i = HandlersCounter - 1;

   while (i > UNKNOWN_FORMAT)
     {
         if (i >= InlineHandlers)
            currentExtraHandler = i - InlineHandlers;
         if (Match_Format (i, fileName))
            return i ;
         --i ;
     }
   return UNKNOWN_FORMAT;
}


/*----------------------------------------------------------------------
   PictureFileOk returns Unsupported_Format if the file does not exist
   - if typeImage is defined it returns Supported_Format if the 
   file is of type typeImage, else Corrupted_File.            
   - if typeImage is not defined, it is updated and we return   
   Supported_Format is of an known type                       
   and Corrupted_File in the other cases                      
  ----------------------------------------------------------------------*/
#ifdef __STDC__
Picture_Report PictureFileOk (char *fileName, int *typeImage)
#else  /* __STDC__ */
Picture_Report PictureFileOk (fileName, typeImage)
char               *fileName;
int                *typeImage;

#endif /* __STDC__ */
{
   Picture_Report      status;

   /* we consider only the supported image formats */

   if (*typeImage >= MAX_PICT_FORMATS || *typeImage < 0)
      *typeImage = UNKNOWN_FORMAT;

   if (TtaFileExist (fileName))
     {
	if (*typeImage == UNKNOWN_FORMAT)
	  {
	     *typeImage = GetPictureFormat (fileName);
	     if (*typeImage == UNKNOWN_FORMAT)
		status = Corrupted_File;
	     else
		status = Supported_Format;
	  }
	else
	  {
	     if (Match_Format (*typeImage, fileName))
		status = Supported_Format;
	     else
		status = Corrupted_File;
	  }
     }
   else
     {
	status = Unsupported_Format;
     }

   return status;
}

/*----------------------------------------------------------------------
   Private Initializations of picture handlers and the visual type 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                InitPictureHandlers (boolean printing)
#else  /* __STDC__ */
void                InitPictureHandlers (printing)
boolean             printing;

#endif /* __STDC__ */
{

#ifdef _WINDOWS
#if 0
   HDC                 hdc;

   /* more magic needed - @@@ */
   hdc = GetDC (WIN_Main_Wd);
   THOT_vInfo.depth = GetDeviceCaps (hdc, PLANES);
   if (THOT_vInfo.depth == 1)
     {
	THOT_vInfo.depth = GetDeviceCaps (hdc, BITSPIXEL);
     }
   THOT_vInfo.class = THOT_PseudoColor;
   ReleaseDC (WIN_Main_Wd, hdc);

#endif /* 0 */
#else  /* _WINDOWS */
   XVisualInfo         vinfo;

   /* initialize Graphic context to display pictures */
   TtGraphicGC = XCreateGC (TtDisplay, TtRootWindow, 0, NULL);
   XSetForeground (TtDisplay, TtGraphicGC, Black_Color);
   XSetBackground (TtDisplay, TtGraphicGC, White_Color);
   XSetGraphicsExposures (TtDisplay, TtGraphicGC, FALSE);
   GCpicture = XCreateGC (TtDisplay, TtRootWindow, 0, NULL);
   XSetForeground (TtDisplay, GCpicture, Black_Color);
   XSetBackground (TtDisplay, GCpicture, White_Color);
   XSetGraphicsExposures (TtDisplay, GCpicture, FALSE);
   /* create a special logo for lost pictures */
   PictureLogo = TtaCreatePixmapLogo (lost_xpm);
   EpsfPictureLogo = XCreatePixmapFromBitmapData (TtDisplay, TtRootWindow,
						  epsflogo_bits,
						  epsflogo_width,
						  epsflogo_height,
						  Black_Color,
						  White_Color,
						  TtWDepth);
   vinfo.visualid = XVisualIDFromVisual (XDefaultVisual (TtDisplay, TtScreen));
   vptr = XGetVisualInfo (TtDisplay, VisualIDMask, &vinfo, &HandlersCounter);
   THOT_vInfo.class = vptr->class;
   THOT_vInfo.depth = vptr->depth;
   theVisual = DefaultVisual (TtDisplay, TtScreen);

#endif /* !_WINDOWS */

   Printing = printing;
   /* by default no plugins loaded */
   HandlersCounter = 0;
   currentExtraHandler = 0;
   strncpy (PictureHandlerTable[HandlersCounter].GUI_Name, XbmName, MAX_FORMAT_NAMELENGHT);
   PictureHandlerTable[HandlersCounter].Produce_Picture = XbmCreate;
   PictureHandlerTable[HandlersCounter].Produce_Postscript = XbmPrint;
   PictureHandlerTable[HandlersCounter].Match_Format = IsXbmFormat;

   PictureIdType[HandlersCounter] = XBM_FORMAT;
   PictureMenuType[HandlersCounter] = XBM_FORMAT;
   HandlersCounter++;

   strncpy (PictureHandlerTable[HandlersCounter].GUI_Name, EpsName, MAX_FORMAT_NAMELENGHT);
   PictureHandlerTable[HandlersCounter].Produce_Picture = EpsCreate;
   PictureHandlerTable[HandlersCounter].Produce_Postscript = EpsPrint;
   PictureHandlerTable[HandlersCounter].Match_Format = IsEpsFormat;

   PictureIdType[HandlersCounter] = EPS_FORMAT;
   PictureMenuType[HandlersCounter] = EPS_FORMAT;
   HandlersCounter++;

   strncpy (PictureHandlerTable[HandlersCounter].GUI_Name, XpmName, MAX_FORMAT_NAMELENGHT);
   PictureHandlerTable[HandlersCounter].Produce_Picture = XpmCreate;
   PictureHandlerTable[HandlersCounter].Produce_Postscript = XpmPrint;
   PictureHandlerTable[HandlersCounter].Match_Format = IsXpmFormat;

   PictureIdType[HandlersCounter] = XPM_FORMAT;
   PictureMenuType[HandlersCounter] = XPM_FORMAT;
   HandlersCounter++;

   strncpy (PictureHandlerTable[HandlersCounter].GUI_Name, GifName, MAX_FORMAT_NAMELENGHT);
   PictureHandlerTable[HandlersCounter].Produce_Picture = GifCreate;
   PictureHandlerTable[HandlersCounter].Produce_Postscript = GifPrint;
   PictureHandlerTable[HandlersCounter].Match_Format = IsGifFormat;

   PictureIdType[HandlersCounter] = GIF_FORMAT;
   PictureMenuType[HandlersCounter] = GIF_FORMAT;
   HandlersCounter++;

   strncpy (PictureHandlerTable[HandlersCounter].GUI_Name, JpegName, MAX_FORMAT_NAMELENGHT);
   PictureHandlerTable[HandlersCounter].Produce_Picture = JpegCreate;
   PictureHandlerTable[HandlersCounter].Produce_Postscript = JpegPrint;
   PictureHandlerTable[HandlersCounter].Match_Format = IsJpegFormat;

   PictureIdType[HandlersCounter] = JPEG_FORMAT;
   PictureMenuType[HandlersCounter] = JPEG_FORMAT;
   HandlersCounter++;

   strncpy (PictureHandlerTable[HandlersCounter].GUI_Name, PngName, MAX_FORMAT_NAMELENGHT);
   PictureHandlerTable[HandlersCounter].Produce_Picture = PngCreate;
   PictureHandlerTable[HandlersCounter].Produce_Postscript = PngPrint;
   PictureHandlerTable[HandlersCounter].Match_Format = IsPngFormat;

   PictureIdType[HandlersCounter] = PNG_FORMAT;
   PictureMenuType[HandlersCounter] = PNG_FORMAT;
   HandlersCounter++;
   InlineHandlers = HandlersCounter;
}


/*----------------------------------------------------------------------
   GetPictHandlersList creates in buffer the list of defined handlers 
   This function is used to create the GUI Menu            
   We return in count the number of handlers               
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                GetPictHandlersList (int *count, char *buffer)
#else  /* __STDC__ */
void                GetPictHandlersList (count, buffer)
int                *count;
char               *buffer;

#endif /* __STDC__ */
{
   int                 i = 0;
   int                 index = 0;
   char               *item;

   *count = HandlersCounter;
   while (i < HandlersCounter)
     {
	item = PictureHandlerTable[i].GUI_Name;
	strcpy (buffer + index, item);
	index += strlen (item) + 1;
	i++;
     }
   buffer = PictureMenu;

}

/*----------------------------------------------------------------------
   SimpleName

   Si filename est un nom de fichier absolu, retourne dans simplename le nom
   simple du fichier.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         SimpleName (char *filename, char *simplename)
#else  /* __STDC__ */
static void         SimpleName (filename, simplename)
char               *filename;
char               *simplename;

#endif /* __STDC__ */
{
   register char      *from, *to;
 
   to = simplename;
   *to = '\0';
   for (from = filename; *from++;) ;
   for (--from; --from > filename;)
     {
        if (*from == DIR_SEP)
          {
             ++from;
             break;
          }
     }
   if (*from == DIR_SEP)
      ++from;
 
   for (; *from;)
      *to++ = *from++;
   *to = '\0';
}


/*----------------------------------------------------------------------
   DrawEpsBox draws the eps logo into the picture box.            
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         DrawEpsBox (PtrBox box, PictInfo * imageDesc, int frame, int wlogo, int hlogo)
#else  /* __STDC__ */
static void         DrawEpsBox (box, imageDesc, frame, wlogo, hlogo)
PtrBox              box;
PictInfo           *imageDesc;
int                 frame;
int                 wlogo;
int                 hlogo;

#endif /* __STDC__ */
{
   int                 x, y, w, h, xif, yif, wif, hif;
   int                 XOrg, YOrg, pxorig, pyorig;
   int                 fileNameWidth;
   int                 fnposx, fnposy;
   char                filename[255];
   Drawable            drawable;
   Pixmap              pix;
   float               Scx, Scy;

   /* Create the temporary picture */
   Scx = 0;
   Scy = 0;
   x = 0;
   y = 0;
   w = 0;
   h = 0;
   switch (imageDesc->PicPresent)
	 {
	    case RealSize:
	       w = imageDesc->PicWArea;
	       h = imageDesc->PicHArea;
	       break;
	    case ReScale:
	       /* what is the smallest scale */
	       Scx = (float) box->BxWidth / (float) imageDesc->PicWArea;
	       Scy = (float) box->BxHeight / (float) imageDesc->PicHArea;
	       if (Scx <= Scy)
		 {
		    w = box->BxWidth;
		    h = (int) ((float) imageDesc->PicHArea * Scx);
		 }
	       else
		 {
		    h = box->BxHeight;
		    w = (int) ((float) imageDesc->PicWArea * Scy);
		 }
	       break;
	    case FillFrame:
	       w = box->BxWidth;
	       h = box->BxHeight;
	       break;
	 }

#ifndef _WINDOWS
   pix = XCreatePixmap (TtDisplay, TtRootWindow, w, h, TtWDepth);
   XFillRectangle (TtDisplay, pix, TtBlackGC, x, y, w, h);

   /* putting the cross edges */

   XDrawRectangle (TtDisplay, pix, TtDialogueGC, x, y, w - 1, h - 1);
   XDrawLine (TtDisplay, pix, TtDialogueGC, x, y, x + w - 1, y + h - 2);
   XDrawLine (TtDisplay, pix, TtDialogueGC, x + w - 1, y, x, y + h - 2);
   XDrawLine (TtDisplay, pix, TtWhiteGC, x, y + 1, x + w - 1, y + h - 1);
   XDrawLine (TtDisplay, pix, TtWhiteGC, x + w - 1, y + 1, x, y + h - 1);

#endif /* _WINDOWS */

   /* copying the logo */
   /* 2 pixels used by the enclosing rectangle */
   if (wlogo > w - 2)
     {
	wif = w - 2;
	xif = x + 1;
	pxorig = wlogo - w + 2;
     }
   else
     {
	wif = wlogo;
	xif = x + w - 1 - wlogo;
	pxorig = 0;
     }
   /* 2 pixels used by the enclosing rectangle */
   if (hlogo > h - 2)
     {
	hif = h - 2;
	yif = y + 1;
	pyorig = hlogo - h + 2;
     }
   else
     {
	hif = hlogo;
	yif = y + 1;
	pyorig = 0;
     }
#ifndef _WINDOWS

   /* Drawing In the Picture Box */
   XCopyArea (TtDisplay, imageDesc->PicPixmap, pix, TtDialogueGC, pxorig, pyorig,
	      wif, hif, xif, yif);
   GetXYOrg (frame, &XOrg, &YOrg);
#endif /* _WINDOWS */
   xif = box->BxXOrg + FrameTable[frame].FrLeftMargin - XOrg;
   yif = box->BxYOrg + FrameTable[frame].FrTopMargin - YOrg;
   wif = box->BxWidth;
   hif = box->BxHeight;
   Picture_Center (w, h, wif, hif, RealSize, &x, &y, &pxorig, &pyorig);
#ifndef _WINDOWS
   drawable = TtaGetThotWindow (frame);
#endif /* _WINDOWS */
   if (w > wif)
      w = wif;
   if (h > hif)
      h = hif;
   x += xif;
   y += yif;
#ifndef _WINDOWS
   LayoutPicture (pix, drawable, pxorig, pyorig, w, h, x, y);
   XFreePixmap (TtDisplay, pix);
   pix = None;
   XSetLineAttributes (TtDisplay, TtLineGC, 1, LineSolid, CapButt, JoinMiter);
   XDrawRectangle (TtDisplay, drawable, TtLineGC, xif, yif, wif - 1, hif - 1);

   /* Display the filename in the bottom of the Picture Box */
   SimpleName (imageDesc->PicFileName, filename);
   fileNameWidth = XTextWidth ((XFontStruct *) FontDialogue, filename, strlen (filename));
   if ((fileNameWidth + wlogo <= wif) && (FontHeight (FontDialogue) + hlogo <= hif))
     {
	fnposx = (wif - fileNameWidth) / 2 + xif;
	fnposy = hif - 5 + yif;
	XSetFont (TtDisplay, TtLineGC, ((XFontStruct *) FontDialogue)->fid);
	XDrawString (TtDisplay, drawable, TtLineGC, fnposx, fnposy, filename, strlen (filename));
     }
#endif /* _WINDOWS */
}


/*----------------------------------------------------------------------
   DrawPicture draws the picture in the frame window.                   
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                DrawPicture (PtrBox box, PictInfo * imageDesc, int frame)
#else  /* __STDC__ */
void                DrawPicture (box, imageDesc, frame)
PtrBox              box;
PictInfo           *imageDesc;
int                 frame;

#endif /* __STDC__ */
{
#ifndef _WINDOWS
   int                 typeImage;
   char                fileName[1023];
   PictureScaling      pres;
   int                 xif, yif, wif, hif;
   int                 PicXArea, PicYArea, PicWArea, PicHArea;
   int                 xtranslate, ytranslate, pxorig, pyorig;
   Drawable            drawable;
   int                 XOrg, YOrg;
   unsigned long       BackGroundPixel;

   xtranslate = 0;
   ytranslate = 0;
   pxorig = 0;
   pyorig = 0;
   drawable = TtaGetThotWindow (frame);
   GetXYOrg (frame, &XOrg, &YOrg);
   typeImage = imageDesc->PicType;
   GetPictureFileName (imageDesc->PicFileName, fileName);

   pres = imageDesc->PicPresent;
   xif = box->BxXOrg + FrameTable[frame].FrLeftMargin - XOrg;
   yif = box->BxYOrg + FrameTable[frame].FrTopMargin - YOrg;
   wif = box->BxWidth;
   hif = box->BxHeight;
   /* resize plugins if necessary */
   if (typeImage >= InlineHandlers)
     {
       imageDesc->PicWArea = wif;
       imageDesc->PicHArea = hif;
     }
   PicXArea = imageDesc->PicXArea;
   PicYArea = imageDesc->PicYArea;
   PicWArea = imageDesc->PicWArea;
   PicHArea = imageDesc->PicHArea;
   BackGroundPixel = box->BxAbstractBox->AbBackground;

   SetPictureClipping (&PicWArea, &PicHArea, wif, hif, imageDesc);
   if (!Printing)
     {
       SetCursorWatch (frame);
       if (imageDesc->PicPixmap == EpsfPictureLogo)
	 DrawEpsBox (box, imageDesc, frame, epsflogo_width, epsflogo_height);
       else
	 {
	   if (!IsValid (box, imageDesc) || 
	       (( imageDesc->PicWArea!= wif) || (imageDesc->PicHArea!=hif)))
	     {
	       LoadPicture (frame, box, imageDesc);
	       PicWArea = imageDesc->PicWArea;
	       PicHArea = imageDesc->PicHArea;
	       SetPictureClipping (&PicWArea, &PicHArea, wif, hif, imageDesc);
	     }
	   if (imageDesc->PicPresent != FillFrame)
	     Picture_Center (PicWArea, PicHArea, wif, hif, pres, &xtranslate, &ytranslate, &pxorig, &pyorig);
	   
	   if (imageDesc->PicMask)
	     {
	       XSetClipOrigin (TtDisplay, TtGraphicGC, xif - pxorig + xtranslate, yif - pyorig + ytranslate);
	       XSetClipMask (TtDisplay, TtGraphicGC, imageDesc->PicMask);
	     }
	   if (PicWArea < wif)
	     wif = PicWArea;
	   if (PicHArea < hif)
	     hif = PicHArea;
	   if (typeImage >= InlineHandlers)
	     {
	       if (PictureHandlerTable[typeImage].DrawPicture != NULL)
		 (*(PictureHandlerTable[typeImage].DrawPicture)) (imageDesc, xif + xtranslate, yif + ytranslate);
	     }
	   else
	     LayoutPicture (imageDesc->PicPixmap, drawable, pxorig, pyorig,
			    wif, hif, xif + xtranslate, yif + ytranslate);
	   
	   if (imageDesc->PicMask)
	     {
	       XSetClipMask (TtDisplay, TtGraphicGC, None);
	       XSetClipOrigin (TtDisplay, TtGraphicGC, 0, 0);
	     }
	 }
       ResetCursorWatch (frame);
     }
   else if (typeImage < InlineHandlers && typeImage > -1)
     /* for the moment we didn't consider plugin printing */
     (*(PictureHandlerTable[typeImage].Produce_Postscript)) (fileName, pres, xif, yif, wif, hif, PicXArea,
							     PicYArea, PicWArea, PicHArea,
							     (FILE *) drawable, BackGroundPixel);
#endif /* _WINDOWS */
}

/*----------------------------------------------------------------------
  UnmapImage unmaps plug-in widgets   
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void           UnmapImage (PictInfo* imageDesc)
#else /* __STDC__ */
void           UnmapImage (imageDesc)
PictInfo      *imageDesc;
#endif /* __STDC__ */
{
    int typeImage;

    if (imageDesc == NULL)
      return;

    typeImage = imageDesc->PicType;

#ifndef _WINDOWS
    if ((typeImage >= InlineHandlers) && (imageDesc->mapped) && (imageDesc->created))
      {	
	XtUnmapWidget ((Widget) (imageDesc->wid));
 	imageDesc->mapped = FALSE;
      }
#endif /* !_WINDOWS */
}


/*----------------------------------------------------------------------
   Routine handling the zoom-in zoom-out of an image   
  ----------------------------------------------------------------------*/
#ifdef __STDC__
char                *ZoomPicture (char *cpic,int cWIDE, int cHIGH , int eWIDE, int eHIGH, int bperpix)
#else  /* __STDC__ */
char                *ZoomPicture (cpic,cWIDE, cHIGH , eWIDE, eHIGH, bperpix)
      char *cpic;
      int cWIDE, cHIGH , eWIDE, eHIGH, bperpix;
#endif /* __STDC__ */
{
    int          cy,ex,ey,*cxarr, *cxarrp;
    char        *clptr,*elptr,*epptr, *epic;
  
    clptr = NULL;  cxarrp = NULL;  cy = 0; 

  /* check for size */
  if ((cWIDE < 0) || (cHIGH < 0) || (eWIDE < 0) || (eHIGH < 0) ||
      (cWIDE > 2000) || (cHIGH > 2000) || (eWIDE > 2000) || (eHIGH > 2000))
    return(NULL);

  /* generate a 'raw' epic, as we'll need it for ColorDither if EM_DITH */

  if (eWIDE==cWIDE && eHIGH==cHIGH) {  
      /* 1:1 expansion.  points destinqtion pic at source pic */
    epic = cpic;
  }
  else {
    /* run the rescaling algorithm */

    /* create a new pic of the appropriate size */

    epic = (char *) malloc((size_t) (eWIDE * eHIGH * bperpix));
    if (!epic) printf(" unable to malloc memory for zoomed image \n");

     cxarr = (int *) malloc(eWIDE * sizeof(int));
    if (!cxarr) printf("unable to allocate cxarr for zoomed image \n");

    for (ex=0; ex<eWIDE; ex++) 
      cxarr[ex] = bperpix * ((cWIDE * ex) / eWIDE);

    elptr = epptr = epic;

    for (ey=0;  ey<eHIGH;  ey++, elptr+=(eWIDE*bperpix)) {
      cy = (cHIGH * ey) / eHIGH;
      epptr = elptr;
      clptr = cpic + (cy * cWIDE * bperpix);

      if (bperpix == 1) {
	for (ex=0, cxarrp = cxarr;  ex<eWIDE;  ex++, epptr++) 
	  *epptr = clptr[*cxarrp++];
      }
      else {
	int j;  char *cp;

	for (ex=0, cxarrp = cxarr; ex<eWIDE; ex++,cxarrp++) {
	  cp = clptr + *cxarrp;
	  for (j=0; j<bperpix; j++) 
	    *epptr++ = *cp++;
	}
      }
    }
    free(cxarr);
  }

  return (char *) epic;

}
/*----------------------------------------------------------------------
   Requests the picture handlers to get the corresponding pixmaps    
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                LoadPicture (int frame, PtrBox box, PictInfo * imageDesc)
#else  /* __STDC__ */
void                LoadPicture (frame, box, imageDesc)
int                 frame;
PtrBox              box;
PictInfo           *imageDesc;
#endif /* __STDC__ */
{
#ifndef _WINDOWS
   int                 typeImage;
   char                fileName[1023];
   PictureScaling      pres;
   int                 xif = 0;
   int                 yif = 0;
   int                 wif, hif, w, h;
   Drawable            PicMask = None;
   Drawable            myDrawable = None;
   Picture_Report      status;
   boolean             noCroppingFrame;
   unsigned long       Bgcolor;

   if (imageDesc->PicFileName == NULL)
      return;
   if (imageDesc->PicFileName[0] == '\0')
      return;
   GetPictureFileName (imageDesc->PicFileName, fileName);
   typeImage = imageDesc->PicType;
   /*typeImage = UNKNOWN_FORMAT;*/

   status = PictureFileOk (fileName, &typeImage);
   w = 0;
   h = 0;
   switch (status) {
	  case (int)Supported_Format:
	       imageDesc->PicType = typeImage;
	       pres = imageDesc->PicPresent;
	       if (box == NULL) {
		    w = 20;
		    h = 20;
	       } else {
		      w = box->BxWidth;
		      h = box->BxHeight;
	       }

	       if (!Printing) {
		  if (box != NULL)
		      /* set the colors of the  graphic context GC */
		      if (TtWDepth == 1) {
			 /* Black and white screen */
			 XSetForeground (TtDisplay, TtGraphicGC, Black_Color);
			 XSetBackground (TtDisplay, TtGraphicGC, ColorPixel (BackgroundColor[frame]));
		      } else if (box->BxAbstractBox->AbSensitive && !box->BxAbstractBox->AbPresentationBox) {
			     /* Set active Box Color */
			     XSetForeground (TtDisplay, TtGraphicGC, Box_Color);
			     XSetForeground (TtDisplay, GCpicture, Box_Color);
			     XSetBackground (TtDisplay, TtGraphicGC, ColorPixel (box->BxAbstractBox->AbBackground));
		      } else {
			    /* Set Box Color */
			    XSetForeground (TtDisplay, TtGraphicGC, ColorPixel (box->BxAbstractBox->AbForeground));
			    XSetForeground (TtDisplay, GCpicture, ColorPixel (box->BxAbstractBox->AbForeground));
			    XSetBackground (TtDisplay, TtGraphicGC, ColorPixel (box->BxAbstractBox->AbBackground));
		      }
	       }

	       Bgcolor = ColorPixel (box->BxAbstractBox->AbBackground);
	       if (PictureHandlerTable[typeImage].Produce_Picture != NULL) {
		  if (typeImage >= InlineHandlers) {
		     imageDesc->PicWArea = wif = box->BxWidth;
		     imageDesc->PicHArea = hif = box->BxHeight;
		     myDrawable = (*(PictureHandlerTable[typeImage].Produce_Picture)) (frame, imageDesc, fileName);
		     xif = imageDesc->PicXArea;
		     yif = imageDesc->PicYArea;
		  } else {
			 if ((box->BxWidth != 0) && (box->BxHeight !=0)) {
			     xif = box->BxWidth;
			     yif = box->BxHeight;
			 }
		     myDrawable = (*(PictureHandlerTable[typeImage].
				     Produce_Picture)) (fileName, pres, &xif, &yif, &wif, &hif, Bgcolor, &PicMask);
		  }
		   noCroppingFrame = ((wif == 0) && (hif == 0));
	       }

	       if (myDrawable == None) {
		   myDrawable = PictureLogo;
		   imageDesc->PicType = -1;
		    w = 40;
		    h = 40;
		    PicMask = None;
	       } else {
		    if (box != NULL) {
			 if (noCroppingFrame)
			    NewDimPicture (box->BxAbstractBox);

			 w = wif;
			 h = hif;
		    }
	       }
	       break;
	    case (int) Corrupted_File:
	    case (int) Unsupported_Format:
	         myDrawable = PictureLogo;
	         imageDesc->PicType = -1;
	         w = 40;
	         h = 40;
	         PicMask = None;
	         break;
   }
   imageDesc->PicXArea = 0; /* xif */
   imageDesc->PicYArea = 0; /* yif */
   imageDesc->PicWArea = w;
   imageDesc->PicHArea = h;
   
   if (!Printing || imageDesc->PicPixmap != EpsfPictureLogo)
     UpdatePictInfo (imageDesc, myDrawable, PicMask);
#endif /* _WINDOWS */
}


/*----------------------------------------------------------------------
   FreePicture frees the Picture Info structure from pixmaps        
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                FreePicture (PictInfo * imageDesc)
#else  /* __STDC__ */
void                FreePicture (imageDesc)
PictInfo           *imageDesc;

#endif /* __STDC__ */
{

#ifndef _WINDOWS
   if (imageDesc->PicPixmap != None)
     {

	FreePixmap (imageDesc->PicPixmap);
	FreePixmap (imageDesc->PicMask);
	imageDesc->PicPixmap = None;
	imageDesc->PicMask = None;
     }

     if ((imageDesc->PicType >= InlineHandlers) && (PictureHandlerTable[imageDesc->PicType].FreePicture != NULL))
        (*(PictureHandlerTable[imageDesc->PicType].FreePicture)) (imageDesc);   
#endif /* _WINDOWS */
}				

/*----------------------------------------------------------------------
   GetPictureType returns the type of the image based on the index 
   in the GUI form.                                        
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                 GetPictureType (int GUIIndex)
#else  /* __STDC__ */
int                 GetPictureType (GUIIndex)
int                 menuIndex;

#endif /* __STDC__ */
{
   if (GUIIndex == 0)
      return UNKNOWN_FORMAT;
   else
      /* based on the function GetPictureHandlersList */
      return PictureMenuType[GUIIndex];

}

/*----------------------------------------------------------------------
   GetPictTypeIndex returns the menu type index of the picture.    
   		If the type is unkown we return 0.                      
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                 GetPictTypeIndex (int PicType)
#else  /* __STDC__ */
int                 GetPictTypeIndex (PicType)
int                 PicType;

#endif /* __STDC__ */
{
   int                 i = 0;

   /* based on the function GetPictureHandlersList */
   if (PicType == UNKNOWN_FORMAT)
      return 0;

   while (i <= HandlersCounter)
     {
	if (PictureMenuType[i] == PicType)
	   return i;
	i++;
     }
   return 0;
}
/*----------------------------------------------------------------------
   GetPictPresIndex returns the index of of the presentation.      
   	If the presentation is unknown we return RealSize.      
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                 GetPictPresIndex (PictureScaling PicPresent)
#else  /* __STDC__ */
int                 GetPictPresIndex (PicPresent)
PictureScaling      PicPresent;

#endif /* __STDC__ */
{
   int                 i;

   switch (PicPresent)
	 {
	    case RealSize:
	    case ReScale:
	    case FillFrame:
	       i = (int) PicPresent;
	       break;
	    default:
	       i = (int) RealSize;
	       break;
	 }
   /* based on the function GetPictureHandlersList */

   return i;
}


/*----------------------------------------------------------------------
   GetPictureHandlersList creates the list of installed handlers.  
   This function is used to create the menu picture.       
   It returns the number of handlers in count.             
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                GetPictureHandlersList (int *count, char *buffer)
#else  /* __STDC__ */
void                GetPictureHandlersList (count, buffer)
int                *count;
char               *buffer;

#endif /* __STDC__ */
{
   int                 i = 0;
   int                 index = 0;
   char               *item;

   *count = HandlersCounter;
   while (i < HandlersCounter)
     {
	item = PictureHandlerTable[i].GUI_Name;
	strcpy (buffer + index, item);
	index += strlen (item) + 1;
	i++;
     }
   buffer = PictureMenu;

}


/*----------------------------------------------------------------------
   LittleXBigEndian allows conversion between big and little endian  
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                LittleXBigEndian (register unsigned char *b, register long n)
#else  /* __STDC__ */
void                LittleXBigEndian (b, n)
register unsigned char *b;
register long       n;

#endif /* __STDC__ */
{
   do
     {
	*b = MirrorBytes[*b];
	b++;
     }
   while (--n > 0);
}


/*----------------------------------------------------------------------
   TtaSetMainThotWindowBackgroundImage :                           
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                 TtaSetMainThotWindowBackgroundImage (ThotWindow w, char *imageFile)
#else  /* __STDC__ */
int                 TtaSetMainThotWindowBackgroundImage (w, imageFile)
ThotWindow          w;
char               *imageFile;

#endif /* __STDC__ */

{
#ifndef _WINDOWS
   char            fileName[MAX_PATH];
   int             typeImage;
   Drawable        myDrawable = None;
   Drawable        PicMask;
   int             xif,yif,wif,hif;
   PictureScaling  pres;
   unsigned long   Bgcolor;

   GetPictureFileName (imageFile, fileName);
   typeImage = GetPictureFormat(fileName);
   myDrawable = (*(PictureHandlerTable[typeImage].Produce_Picture))
                  (fileName, pres, &xif, &yif, &wif, &hif, Bgcolor, &PicMask );
   XSetWindowBackgroundPixmap (TtDisplay, w, myDrawable);
   FreePixmap(myDrawable);
   FreePixmap(PicMask);
#endif /* _WINDOWS */
   return (0);
}
