/*
         Picture Handling
 */

#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "lost.xpm"
#include "picture.h"
#include "frame.h"
#include "epsflogo.h"
#include "interface.h"
#include "thotfile.h"
#include "thotdir.h"
#include "png.h"
#define EXPORT extern
#include "boxes_tv.h"
#include "frame_tv.h"
#include "font_tv.h"
#include "platform_tv.h"
#include "appli_f.h"
#include "tree_f.h"
#include "views_f.h"
#include "dofile_f.h"
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

static PictureHandler  PictureHandlerTable[MAX_PICT_FORMATS];
static int             PictureIdType      [MAX_PICT_FORMATS];
static int             PictureMenuType    [MAX_PICT_FORMATS];
static char           *PictureMenu;
static int             HandlersCounter;
static Pixmap          PictureLogo;
Pixmap                 EpsfPictureLogo;
static boolean         Printing;
ThotGC                 GCpicture;	
THOT_VInfo             THOT_vInfo;
#ifndef NEW_WILLOWS
XVisualInfo            *vptr;
Visual                 *theVisual;
#endif
char *FileExtension[] ={".xbm", ".eps", ".xpm", ".gif", ".jpg", ".png"};


/* ---------------------------------------------------------------------- */
/* |  Match_Format returns TRUE if the considered header file matches   | */
/* |    the image file description, FALSE in the the other cases        | */
/* ---------------------------------------------------------------------- */
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
  else
    return MAX_PICT_FORMATS;
}


/* ---------------------------------------------------------------------- */
/* |  FreePixmap frees the pixmap allocated in the X server if it is not| */
/* |           empty and if it is not one of the internal images        | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         FreePixmap (Pixmap pix)
#else  /* __STDC__ */
static void         FreePixmap (pix)
Pixmap              pix;
#endif /* __STDC__ */
{
#ifndef NEW_WILLOWS
   if ((pix != None)
       && (pix != PictureLogo)
       && (pix != EpsfPictureLogo))
      XFreePixmap (TtDisplay, pix); 
#endif /* NEW_WILLOWS */
}

/* ---------------------------------------------------------------------- */
/* |   UpdatePictInfo updates the picture information structure by      | */
/* |          setting the picture and the mask                          | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         UpdatePictInfo (PictInfo * imdesc, Pixmap pix, Pixmap PicMask)
#else  /* __STDC__ */
static void         UpdatePictInfo (imdesc, pix, PicMask)
PictInfo    *imdesc;
Pixmap       pix;
Pixmap       PicMask;
#endif /* __STDC__ */
{
   FreePixmap (imdesc->PicPixmap);
   FreePixmap (imdesc->PicMask);
   imdesc->PicPixmap = pix;
   imdesc->PicMask = PicMask;
}


/* ---------------------------------------------------------------------- */
/* |  Picture_Center updates the parameters xtranslate, ytranslate,     | */
/* |            pxorig, pyorig depending on the values of PicWArea,     | */
/* |            PicHArea, wif, hif and pres.                            | */
/* |            - If we use ReScale, the tranlation is performed        | */
/* |            in one direction.                                       | */
/* |            - If we use FillFrame, there's no translation           | */
/* |            - if we use RealSize we translate to achieve            | */
/* |            the centering               .                           | */
/* |            if the picture size is greater than the frame then      | */
/* |          pxorig or pyorig are positive.                            | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         Picture_Center (int wimage, int himage, int wbox, int hbox, PictureScaling pres, int *xtranslate, int *ytranslate, int *pxorig, int *pyorig)
#else  /* __STDC__ */
static void         Picture_Center (wimage, himage, wbox, hbox, pres, xtranslate, ytranslate, pxorig, pyorig)
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

/* ---------------------------------------------------------------------- */
/* | SetPictureClipping clips the picture into boundaries.              | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         SetPictureClipping (int *PicWArea, int *PicHArea, int wif, int hif, PictInfo * imageDesc)
#else  /* __STDC__ */
static void         SetPictureClipping (PicWArea, PicHArea, wif, hif, imageDesc)
int                *PicWArea;
int                *PicHArea;
int                 wif;
int                 hif;
PictInfo    *imageDesc;
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

/* ----------------------------------------------------------------------- */
/* | LayoutPicture performs the layout of SrcPix on the screen described | */
/* | by Drawab.                                                          | */
/* | if srcorx or srcory are postive, the copy operation is shifted      | */
/* ----------------------------------------------------------------------- */
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

#ifndef NEW_WILLOWS
   if (SrcPix != None)
      XCopyArea (TtDisplay, SrcPix, Drawab, TtGraphicGC, srcorx, srcory, w, h, desorx, desory);
#endif /* NEW_WILLOWS */

}


/* ---------------------------------------------------------------------- */
/* |   IsValid retourne FALSE si le pixmap contenu dans imageDesc       | */
/* |            est vide. On retourne TRUE s'il est egal aux images     | */
/* |            predefinies BadPixmap.                                  | */
/* |            - if we use RealSize, we  return TRUE.                  | */
/* |            - if we use  ReScale, we return TRUE                    | */
/* |            the box have one of the two  dimensions a least equals  | */
/* |            to the one of the pixmap.                               | */
/* |            - if we use  ReScale FillFrame, we return TRUE if the   | */
/* |            frame box  has the same size than the pixmap in         | */
/* |            both directions.                                        | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static boolean      IsValid (PtrBox box, PictInfo * imageDesc)
#else  /* __STDC__ */
static boolean      IsValid (box, imageDesc)
PtrBox            box;
PictInfo    *imageDesc;
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
     {
	return FALSE;
     }
   else
     {
#ifndef NEW_WILLOWS
	XGetGeometry (TtDisplay, imageDesc->PicPixmap, &root, &xpix, &ypix, &wpix, &hpix, &bdw, &dep);
#endif /* NEW_WILLOWS */
	switch (imageDesc->PicPresent)
	      {
		 case RealSize:
		    pixmapok = TRUE;
		    break;
		 case ReScale:
		    pixmapok = ((box->BxWidth == wpix) || (box->BxHeight == hpix));
		    break;
		 case FillFrame:
		    pixmapok = ((box->BxWidth == wpix) && (box->BxHeight == hpix));
		    break;
	      }
	return pixmapok;
     }
}


/* ---------------------------------------------------------------------- */
/* |    GetPictureFormat returns the format of a file picture           | */
/* |    the file  fileName or UNKNOWN_FORMAT if not recognized          | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static int          GetPictureFormat (char *fileName)
#else  /* __STDC__ */
static int          GetPictureFormat (fileName)
char               *fileName;
#endif /* __STDC__ */
{
   int                 i;
   int                 l = 0;

   i = MAX_PICT_FORMATS - 1;
   l = strlen (fileName);
   if (l > 4)
     {
	if (strcmp (fileName + l - 4, ".pic") == 0 || strcmp (fileName + l - 4, ".xbm") == 0)
	  {
	     return XBM_FORMAT;
	  }
	if (strcmp (fileName + l - 4, ".eps") == 0 || strcmp (fileName + l - 3, ".ps") == 0)
	  {
	     return EPS_FORMAT;
	  }
	if (strcmp (fileName + l - 4, ".xpm") == 0)
	  {
	     return XPM_FORMAT;
	  }
	if ((strcmp (fileName + l - 4, ".gif") == 0) || (strcmp (fileName + l - 4, ".GIF") == 0))
	  {
	     return GIF_FORMAT;
	  }
	if (strcmp (fileName + l - 4, ".jpg") == 0)
	  {
	     return JPEG_FORMAT;
	  }
	if (strcmp (fileName + l - 4, ".png") == 0)
	  {
	     return PNG_FORMAT;
	  }
     }
   while (i > UNKNOWN_FORMAT)
     {
	if (Match_Format (i, fileName))
	  {
	     return i;
	  }
	else
	  {
	     i--;
	  }
     }
   return UNKNOWN_FORMAT;

}	


/* ---------------------------------------------------------------------- */
/* | PictureFileOk returns Unsupported_Format if the file does not exist| */
/* |       - if typeImage is defined it returns Supported_Format if the | */
/* |         file is of type typeImage, else Corrupted_File.            | */
/* |       - if typeImage is not defined, it is updated and we return   | */
/* |         Supported_Format is of an known type                       | */
/* |         and Corrupted_File in the other cases                      | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static Picture_Report   PictureFileOk (char *fileName, int *typeImage)
#else  /* __STDC__ */
static Picture_Report   PictureFileOk (fileName, typeImage)
char               *fileName;
int                *typeImage;
#endif /* __STDC__ */
{
   Picture_Report          status;

   /* we consider only the supported image formats */

   if (*typeImage >= MAX_PICT_FORMATS || *typeImage < 0)
      *typeImage = UNKNOWN_FORMAT;

   if (ThotFile_exist (fileName))
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

/* ------------------------------------------------------------------- */
/* | Private Initializations of picture handlers and the visual type | */
/* ------------------------------------------------------------------- */
#ifdef __STDC__
void       InitPictureHandlers (boolean printing)
#else  /* __STDC__ */
void       InitPictureHandlers (printing)
boolean    printing;
#endif /* __STDC__ */
{
   int                 i;
   XVisualInfo         vinfo, *vptr;
#ifdef NEW_WILLOWS
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
#else  /* NEW_WILLOWS */
   TtGraphicGC = XCreateGC (TtDisplay, TtRootWindow, 0, NULL);
   XSetForeground (TtDisplay, TtGraphicGC, Black_Color);
   XSetBackground (TtDisplay, TtGraphicGC, White_Color);
   XSetGraphicsExposures (TtDisplay, TtGraphicGC, FALSE);

   GCpicture = XCreateGC (TtDisplay, TtRootWindow, 0, NULL);
   XSetForeground (TtDisplay, GCpicture, Black_Color);
   XSetBackground (TtDisplay, GCpicture, White_Color);
   XSetGraphicsExposures (TtDisplay, GCpicture, FALSE);

   PictureLogo = TtaCreatePixmapLogo (lost_xpm);
   EpsfPictureLogo = XCreatePixmapFromBitmapData (TtDisplay, TtRootWindow,
						  epsflogo_bits,
						  epsflogo_width,
						  epsflogo_height, 
						  Black_Color, 
						  White_Color,
						  TtWDepth);
   vinfo.visualid = XVisualIDFromVisual(XDefaultVisual(TtDisplay,TtScreen));
   vptr = XGetVisualInfo (TtDisplay, VisualIDMask, &vinfo, &i);
   THOT_vInfo.class = vptr->class;
   THOT_vInfo.depth = vptr->depth;
   theVisual = DefaultVisual (TtDisplay, TtScreen);

#endif /* !NEW_WILLOWS */

   Printing = printing;
   i = 0;
   strncpy (PictureHandlerTable[i].GUI_Name, XbmName, MAX_FORMAT_NAMELENGHT);
   PictureHandlerTable[i].Produce_Picture = XbmCreate;
   PictureHandlerTable[i].Produce_Postscript = XbmPrint;
   PictureHandlerTable[i].Match_Format = IsXbmFormat;

   PictureIdType[i] = XBM_FORMAT;
   PictureMenuType[i] = XBM_FORMAT;
   i++;

   strncpy (PictureHandlerTable[i].GUI_Name, EpsName, MAX_FORMAT_NAMELENGHT);
   PictureHandlerTable[i].Produce_Picture = EpsCreate;
   PictureHandlerTable[i].Produce_Postscript = EpsPrint;
   PictureHandlerTable[i].Match_Format = IsEpsFormat;

   PictureIdType[i] = EPS_FORMAT;
   PictureMenuType[i] = EPS_FORMAT;
   i++;

   strncpy (PictureHandlerTable[i].GUI_Name, XpmName, MAX_FORMAT_NAMELENGHT);
   PictureHandlerTable[i].Produce_Picture = XpmCreate;
   PictureHandlerTable[i].Produce_Postscript = XpmPrint;
   PictureHandlerTable[i].Match_Format = IsXpmFormat;

   PictureIdType[i] = XPM_FORMAT;
   PictureMenuType[i] = XPM_FORMAT;
   i++;

   strncpy (PictureHandlerTable[i].GUI_Name, GifName, MAX_FORMAT_NAMELENGHT);
   PictureHandlerTable[i].Produce_Picture = GifCreate;
   PictureHandlerTable[i].Produce_Postscript = GifPrint;
   PictureHandlerTable[i].Match_Format = IsGifFormat;

   PictureIdType[i] = GIF_FORMAT;
   PictureMenuType[i] = GIF_FORMAT;
   i++;

   strncpy (PictureHandlerTable[i].GUI_Name, JpegName, MAX_FORMAT_NAMELENGHT);
   PictureHandlerTable[i].Produce_Picture = JpegCreate;
   PictureHandlerTable[i].Produce_Postscript = JpegPrint;
   PictureHandlerTable[i].Match_Format = IsJpegFormat;

   PictureIdType[i] = JPEG_FORMAT;
   PictureMenuType[i] = JPEG_FORMAT;
   i++;

   strncpy (PictureHandlerTable[i].GUI_Name, PngName, MAX_FORMAT_NAMELENGHT);
   PictureHandlerTable[i].Produce_Picture = PngCreate;
   PictureHandlerTable[i].Produce_Postscript = PngPrint;
   PictureHandlerTable[i].Match_Format = IsPngFormat;

   PictureIdType[i] = PNG_FORMAT;
   PictureMenuType[i] = PNG_FORMAT;
   i++;
   HandlersCounter = i;
}


/* ---------------------------------------------------------------------- */
/* | GetPictHandlersList creates in buffer the list of defined handlers | */
/* |            This function is used to create the GUI Menu            | */
/* |            We return in count the number of handlers               | */
/* ---------------------------------------------------------------------- */
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


/* ------------------------------------------------------------------- */
/* |  DrawEpsBox draws the eps logo into the picture box.            | */
/* ------------------------------------------------------------------- */
#ifdef __STDC__
static void       DrawEpsBox (PtrBox box, PictInfo *imageDesc, int frame, int wlogo, int hlogo)
#else  /* __STDC__ */
static void       DrawEpsBox (box, imageDesc, frame, wlogo, hlogo)
PtrBox            box;
PictInfo         *imageDesc;
int               frame;
int               wlogo;
int               hlogo;
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

#ifndef NEW_WILLOWS
   pix = XCreatePixmap (TtDisplay, TtRootWindow, w, h, TtWDepth);
   XFillRectangle (TtDisplay, pix, TtBlackGC, x, y, w, h);

   /* putting the cross edges */

   XDrawRectangle (TtDisplay, pix, TtDialogueGC, x, y, w - 1, h - 1);
   XDrawLine (TtDisplay, pix, TtDialogueGC, x, y, x + w - 1, y + h - 2);
   XDrawLine (TtDisplay, pix, TtDialogueGC, x + w - 1, y, x, y + h - 2);
   XDrawLine (TtDisplay, pix, TtWhiteGC, x, y + 1, x + w - 1, y + h - 1);
   XDrawLine (TtDisplay, pix, TtWhiteGC, x + w - 1, y + 1, x, y + h - 1);

#endif /* NEW_WILLOWS */

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
#ifndef NEW_WILLOWS

   /* Drawing In the Picture Box*/
   XCopyArea (TtDisplay, imageDesc->PicPixmap, pix, TtDialogueGC, pxorig, pyorig,
	      wif, hif, xif, yif);
   GetXYOrg (frame, &XOrg, &YOrg);
#endif /* NEW_WILLOWS */
   xif = box->BxXOrg + FrameTable[frame].FrLeftMargin - XOrg;
   yif = box->BxYOrg + FrameTable[frame].FrTopMargin - YOrg;
   wif = box->BxWidth;
   hif = box->BxHeight;
   Picture_Center (w, h, wif, hif, RealSize, &x, &y, &pxorig, &pyorig);
#ifndef NEW_WILLOWS
   drawable = TtaGetThotWindow (frame);
#endif /* NEW_WILLOWS */
   if (w > wif)
      w = wif;
   if (h > hif)
      h = hif;
   x += xif;
   y += yif;
#ifndef NEW_WILLOWS
   LayoutPicture (pix, drawable, pxorig, pyorig, w, h, x, y);
   XFreePixmap (TtDisplay, pix);
   pix = None;
   XSetLineAttributes (TtDisplay, TtLineGC, 1, LineSolid, CapButt, JoinMiter);
   XDrawRectangle (TtDisplay, drawable, TtLineGC, xif, yif, wif - 1, hif - 1);

   /* Draw the filename in the bottom of the Picture Box */
   BaseName (imageDesc->PicFileName, filename, 0, 0);
   fileNameWidth = XTextWidth ((XFontStruct *) FontDialogue, filename, strlen (filename));
   if ((fileNameWidth + wlogo <= wif) && (FontHeight (FontDialogue) + hlogo <= hif))
     {
	fnposx = (wif - fileNameWidth) / 2 + xif;
	fnposy = hif - 5 + yif;
	XSetFont (TtDisplay, TtLineGC, ((XFontStruct *) FontDialogue)->fid);
	XDrawString (TtDisplay, drawable, TtLineGC, fnposx, fnposy, filename, strlen (filename));
     }
#endif /* NEW_WILLOWS */
}


/* ------------------------------------------------------------------------ */
/* | DrawPicture draws the picture in the frame window.                   | */
/* ------------------------------------------------------------------------ */
#ifdef __STDC__
void                DrawPicture (PtrBox box, PictInfo * imageDesc, int frame)
#else  /* __STDC__ */
void                DrawPicture (box, imageDesc, frame)
PtrBox            box;
PictInfo    *imageDesc;
int                 frame;
#endif /* __STDC__ */
{
#ifndef NEW_WILLOWS
  int                 typeImage;
  char                fileName[1023];
  PictureScaling           pres;
  int                 xif, yif, wif, hif;
  int                 PicXArea, PicYArea, PicWArea, PicHArea;
  int                 xtranslate, ytranslate, pxorig, pyorig;
  Drawable            myDrawable;
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
	  if (!IsValid (box, imageDesc))
	    {
	      LoadPicture (frame, box, imageDesc);
	      myDrawable = imageDesc->PicPixmap;
	    }
	  else
	    {
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
	      LayoutPicture (imageDesc->PicPixmap, drawable, pxorig, pyorig,
			    wif, hif, xif + xtranslate, yif + ytranslate);
	      if (imageDesc->PicMask)
		{
		  XSetClipMask (TtDisplay, TtGraphicGC, None);
		  XSetClipOrigin (TtDisplay, TtGraphicGC, 0, 0);
		}
	    }
	}
      ResetCursorWatch (frame);
    }
  else if (typeImage < HandlersCounter && typeImage > -1)
    (*(PictureHandlerTable[typeImage].Produce_Postscript)) (fileName, pres, xif, yif, wif, hif, PicXArea, PicYArea, PicWArea, PicHArea,
						    (FILE *) drawable, BackGroundPixel);
#endif /* NEW_WILLOWS */
}


/* ---------------------------------------------------------------------- */
/* |  Requests the picture handlers to get the corresponding pixmaps    | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                LoadPicture (int frame, PtrBox box, PictInfo * imageDesc)
#else  /* __STDC__ */
void                LoadPicture (frame, box, imageDesc)
int                 frame;
PtrBox            box;
PictInfo    *imageDesc;
#endif /* __STDC__ */
{
#ifndef NEW_WILLOWS
   int                 typeImage;
   char                fileName[1023];
   PictureScaling           pres;
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
   /* typeImage = imageDesc->PicType;*/
   typeImage = UNKNOWN_FORMAT;

   status = PictureFileOk (fileName, &typeImage);
   w = 0;
   h = 0;
   switch (status)
	 {
	    case (int) Supported_Format:
	       imageDesc->PicType = typeImage;
	       pres = imageDesc->PicPresent;
	       if (box == NULL)
		 {
		    w = 20;
		    h = 20;
		 }
	       else
		 {
		    w = box->BxWidth;	
		    h = box->BxHeight;
		 }

	       if (!Printing)
		 {
		    if (box != NULL)
		       /* set the colors of the  graphic context GC */
		       if (TtWDepth == 1)
			 {
			    /* Black and white screen */
			    XSetForeground (TtDisplay, TtGraphicGC, Black_Color);
			    XSetBackground (TtDisplay, TtGraphicGC,
				       ColorPixel (BackgroundColor[frame]));
			 }
		       else if (box->BxAbstractBox->AbSensitive && 
				!box->BxAbstractBox->AbPresentationBox)
			 {
			    /* Set active Box Color */
			    XSetForeground (TtDisplay, TtGraphicGC, Box_Color);
			    XSetForeground (TtDisplay, GCpicture, Box_Color);
			    XSetBackground (TtDisplay, TtGraphicGC, ColorPixel (box->BxAbstractBox->AbBackground));
			 }
		       else
			 {
			    /* Set Box Color */
			    XSetForeground (TtDisplay, TtGraphicGC, ColorPixel (box->BxAbstractBox->AbForeground));
			    XSetForeground (TtDisplay, GCpicture, ColorPixel (box->BxAbstractBox->AbForeground));
			    XSetBackground (TtDisplay, TtGraphicGC, ColorPixel (box->BxAbstractBox->AbBackground));
			 }
		 }

	       Bgcolor = ColorPixel (box->BxAbstractBox->AbBackground);

	       myDrawable = (*(PictureHandlerTable[typeImage].
			       Produce_Picture)) (fileName, pres, &xif, &yif, &wif, &hif, Bgcolor, &PicMask);

	       noCroppingFrame = ((wif == 0) && (hif == 0));

	       if (myDrawable == None)
		 {
		    myDrawable = PictureLogo;
		    imageDesc->PicType = -1;
		    w = 40;
		    h = 40;
		    PicMask = None;
		 }
	       else
		 {
		    if (box != NULL)
		      {
			 if (noCroppingFrame)
			   {
			      NewDimPicture (box->BxAbstractBox);
			   }
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
   imageDesc->PicXArea = xif;
   imageDesc->PicYArea = yif;
   imageDesc->PicWArea = w;
   imageDesc->PicHArea = h;

   if (!Printing || imageDesc->PicPixmap != EpsfPictureLogo)
      UpdatePictInfo (imageDesc, myDrawable, PicMask);


#endif /* NEW_WILLOWS */
}


/* ---------------------------------------------------------------------- */
/* |   FreePicture frees the Picture Info structure from pixmaps        | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                FreePicture (PictInfo * imageDesc)
#else  /* __STDC__ */
void                FreePicture (imageDesc)
PictInfo    *imageDesc;
#endif /* __STDC__ */
{

#ifndef NEW_WILLOWS
   if (imageDesc->PicPixmap != None)
     {

	FreePixmap (imageDesc->PicPixmap);
	FreePixmap (imageDesc->PicMask);
	imageDesc->PicPixmap = None;
	imageDesc->PicMask = None;
     }
#endif /* NEW_WILLOWS */
}				



/* ---------------------------------------------------------------------- */
/* |    GetPictureType returns the type of the image based on the index | */
/* |            in the GUI form.                                        | */
/* ---------------------------------------------------------------------- */
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

/* ---------------------------------------------------------------------- */
/* |    GetPictTypeIndex returns the menu type index of the picture.    | */
/* |		If the type is unkown we return 0.                      | */
/* ---------------------------------------------------------------------- */
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
	else
	   i++;
     }
   return 0;
}			
/* ---------------------------------------------------------------------- */
/* |    GetPictPresIndex returns the index of of the presentation.      | */
/* |     	If the presentation is unknown we return RealSize.      | */
/* ---------------------------------------------------------------------- */
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


/* ---------------------------------------------------------------------- */
/* |    GetPictureHandlersList creates the list of installed handlers.  | */
/* |            This function is used to create the menu picture.       | */
/* |            It returns the number of handlers in count.             | */
/* ---------------------------------------------------------------------- */
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


/* ---------------------------------------------------------------------- */
/* |  LittleXBigEndian allows conversion between big and little endian  | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                LittleXBigEndian (register unsigned char *b, register long n)
#else  /* __STDC__ */
void                LittleXBigEndian (b, n)
register unsigned char *b;
register long           n;
#endif /* __STDC__ */
{
  do
    {
      *b = MirrorBytes[*b];
      b++;
    }
  while (--n > 0);
}


/* ---------------------------------------------------------------------- */
/* |    TtaSetMainThotWindowBackgroundImage :                           | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
int                 TtaSetMainThotWindowBackgroundImage (ThotWindow w, char *imageFile)
#else  /* __STDC__ */
int                 TtaSetMainThotWindowBackgroundImage (w, imageFile)
ThotWindow          w;
char               *imageFile;

#endif /* __STDC__ */

{
/*****************************************
  char fileName[MAX_PATH];
  int  typeImage;
  Drawable   myDrawable = None;
  Drawable   PicMask;
  int xif,yif,wif,hif;
  PictureScaling  pres;
  unsigned long Bgcolor;
  ThotWindow     frame;
  int        vue;

  GetPictureFileName(imageFile, fileName);
  typeImage = GetPictureFormat(fileName);
  myDrawable = (*(PictureHandlerTable[typeImage].
		  Produce_Picture))(fileName, pres, &xif, &yif, &wif, &hif, Bgcolor,
		  &PicMask );
  XSetWindowBackgroundPixmap(TtDisplay,w,myDrawable);
  FreePixmap(myDrawable);
  FreePixmap(PicMask);
 ****************************************/
   return (0);
}			
