
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
#include "xbmhandler_f.h"
#include "views_f.h"
#include "dofile_f.h"
#include "platform_f.h"
#include "font_f.h"
#include "frame_f.h"
#include "gifhandler_f.h"
#include "jpeghandler_f.h"
#include "picture_f.h"
#include "xpmhandler_f.h"
#include "pnghandler_f.h"
#include "presrules_f.h"
#include "epshandler_f.h"
#include "inites_f.h"

#ifndef NEW_WILLOWS
XVisualInfo            *vptr;
Visual                 *theVisual;
#endif
static PictureHandler  PictureHandlerTable[MAX_PICT_FORMATS];
static int             PictureIdType      [MAX_PICT_FORMATS];
static int             PictureMenuType      [MAX_PICT_FORMATS];
static char            *PictureMenu;
static int             HandlersCounter;
static Pixmap          PictureLogo;
Pixmap                 EpsfPictureLogo;
static boolean         Printing;
ThotGC                 GCpicture;	
THOT_VInfo             THOT_vInfo;


/* ---------------------------------------------------------------------- */
/* |    Match_Format returns TRUE if the considered header file matches the                            | */
/* |    image file description, FALSE in the the other cases                                                         | */
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
/* |    SwapAllBits inverse un byte suivant pour permettre la conversion| */
/* |            big endian - little endian.                             | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                SwapAllBits (register unsigned char *b, register long n)
#else  /* __STDC__ */
void                SwapAllBits (b, n)
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


/* ---------------------------------------------------------------------- */
/* |    FreePixmap detruit le pixmap Pix s'il est non vide et different | */
/* |            des images predefinies          | */
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
/* |    SetImageDescPixmap met a jour le pixmap du descripteur d'image  | */
/* |            imdesc avec le pixmap pix.                              | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         SetImageDescPixmap (PictInfo * imdesc, Pixmap pix, Pixmap PicMask)
#else  /* __STDC__ */
static void         SetImageDescPixmap (imdesc, pix, PicMask)
PictInfo    *imdesc;
Pixmap              pix;
Pixmap              PicMask;
#endif /* __STDC__ */
{
   FreePixmap (imdesc->PicPixmap);
   FreePixmap (imdesc->PicMask);
   imdesc->PicPixmap = pix;
   imdesc->PicMask = PicMask;
}


/* ---------------------------------------------------------------------- */
/* |    CentreImage met a jour les parametres xtranslate, ytranslate,   | */
/* |            pxorig, pyorig en fonction des valeur de PicWArea,      | */
/* |            PicHArea, wif, hif et pres.                             | */
/* |            - Si on presente ReScale, la translation se             | */
/* |            fait dans une seule direction.                          | */
/* |            - Si on presente FillFrame, on ne translate pas.        | */
/* |            - Si on presente RealSize on translate pour             | */
/* |            faire le centrage.                                      | */
/* |            Si l'image est plus grande que la boite, on clippe et   | */
/* |            donc pxorig ou pyorig sont non nuls.                    | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                CentreImage (int wimage, int himage, int wbox, int hbox, PictureScaling pres, int *xtranslate, int *ytranslate, int *pxorig, int *pyorig)
#else  /* __STDC__ */
void                CentreImage (wimage, himage, wbox, hbox, pres, xtranslate, ytranslate, pxorig, pyorig)
int                 wimage;
int                 himage;
int                 wbox;
int                 hbox;
PictureScaling           pres;
int                *xtranslate;
int                *ytranslate;
int                *pxorig;
int                *pyorig;
#endif /* __STDC__ */
{
   float               Rapw, Raph;

   /* la boite a les dimensions wbox, hbox */
   /* l'image a les dimensions wimage, himage */
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
	       /* l'image occupe toute la boite */
	       break;

	    case RealSize:
	       /* on deplace l'image au milieu */
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

/* ------------------------------------------------------------------- */
/* | On teste si on a recupere une Cropping frame                    | */
/* ------------------------------------------------------------------- */
#ifdef __STDC__
static void         IsCropOk (int *PicWArea, int *PicHArea, int wif, int hif, PictInfo * imageDesc)
#else  /* __STDC__ */
static void         IsCropOk (PicWArea, PicHArea, wif, hif, imageDesc)
int                *PicWArea;
int                *PicHArea;
int                 wif;
int                 hif;
PictInfo    *imageDesc;
#endif /* __STDC__ */
{
   if (((imageDesc->PicWArea == 0) && (imageDesc->PicHArea == 0)) ||
       ((imageDesc->PicWArea > 32768) || (imageDesc->PicHArea > 32768)))
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

/* ---------------------------------------------------------------------- */
/* |    CopyOnScreen effectue une copie du pixmap SrcPix sur l'ecran    | */
/* |            Drawab.                                                 | */
/* |            Si srcorx ou srcory sont negatif, on decale la copie.   | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         CopyOnScreen (Pixmap SrcPix, Drawable Drawab, int srcorx, int srcory, int w, int h, int desorx, int desory)
#else  /* __STDC__ */
static void         CopyOnScreen (SrcPix, Drawab, srcorx, srcory, w, h, desorx, desory)
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
/* |    PixmapIsOk retourne FALSE si le pixmap contenu dans imageDesc   | */
/* |            est vide. On retourne TRUE s'il est egal aux images     | */
/* |            predefinies BadPixmap.                    | */
/* |            - Si on presente RealSize, on retourne TRUE.    | */
/* |            - Si on presente ReScale, on retourne TRUE si  | */
/* |            la boite box possede au moins une dimension egale a`    | */
/* |            celle du pixmap.                                        | */
/* |            - Si on presente FillFrame, on retourne TRUE si la     | */
/* |            boite box possede la meme taille que le pixmap dans     | */
/* |            les 2 directions.                                       | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static boolean      PixmapIsOk (PtrBox box, PictInfo * imageDesc)
#else  /* __STDC__ */
static boolean      PixmapIsOk (box, imageDesc)
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
		    pixmapok = TRUE;	/* tout au plus un centrage */
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
/* |    GetImageFileFormat retourne le format d'une image contenue dans | */
/* |            le fichier fileName ou UNKNOWN_FORMAT si on ne trouve pas.     | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static int          GetImageFileFormat (char *fileName)
#else  /* __STDC__ */
static int          GetImageFileFormat (fileName)
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

}				/*GetImageFileFormat */


/* ---------------------------------------------------------------------- */
/* |    FileIsOk retourne Unsupported_Format si le fichier fileName n'existe pas.      | */
/* |            - Si typeImage est defini, on retourne Supported_Format si  | */
/* |            le fichier est du type typeImage, Corrupted_File sinon.    | */
/* |            - Si typeImage est non defini, on le met a jour et on   | */
/* |            retourne Supported_Format si le fichier est d'un type connu | */
/* |            et Corrupted_File sinon.                                   | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static Picture_Report   FileIsOk (char *fileName, int *typeImage)
#else  /* __STDC__ */
static Picture_Report   FileIsOk (fileName, typeImage)
char               *fileName;
int                *typeImage;
#endif /* __STDC__ */

{
   Picture_Report          status;

   /* on ne prend que des types connus */
   if (*typeImage >= MAX_PICT_FORMATS || *typeImage < 0)
      *typeImage = UNKNOWN_FORMAT;

   if (FileExist (fileName))
     {
	if (*typeImage == UNKNOWN_FORMAT)
	  {
	     *typeImage = GetImageFileFormat (fileName);
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
/* | On initialise suivant le modele de sortie, la table des drivers | */
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
				  epsflogo_height, Black_Color, White_Color,
						    TtWDepth);
   vinfo.visualid = XVisualIDFromVisual (XDefaultVisual (TtDisplay,
							 TtScreen));
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
/* |    GetImageDriversList cree dans buffer une liste des drivers      | */
/* |            utilisables. Cette fonction sert a` creer le menu       | */
/* |            image. On retourne dans count le nombre de drivers.     | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                GetImageDriversList (int *count, char *buffer)
#else  /* __STDC__ */
void                GetImageDriversList (count, buffer)
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
/* | On dessine la boite correspondant a l'image + logo              | */
/* ------------------------------------------------------------------- */
#ifdef __STDC__
void                DrawImageBox (PtrBox box, PictInfo * imageDesc, int frame, int wlogo, int hlogo)
#else  /* __STDC__ */
void                DrawImageBox (box, imageDesc, frame, wlogo, hlogo)
PtrBox            box;
PictInfo    *imageDesc;
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

   /* Create the temporary image */
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

   /* putting the cross */
   XDrawRectangle (TtDisplay, pix, TtDialogueGC, x, y, w - 1, h - 1);
   XDrawLine (TtDisplay, pix, TtDialogueGC, x, y, x + w - 1, y + h - 2);
   XDrawLine (TtDisplay, pix, TtDialogueGC, x + w - 1, y, x, y + h - 2);
   XDrawLine (TtDisplay, pix, TtWhiteGC, x, y + 1, x + w - 1, y + h - 1);
   XDrawLine (TtDisplay, pix, TtWhiteGC, x + w - 1, y + 1, x, y + h - 1);
#endif /* NEW_WILLOWS */

   /* copying the logo */
   if (wlogo > w - 2)		/* 2 pixels used by the enclosing rectangle */
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
   if (hlogo > h - 2)		/* 2 pixels used by the enclosing rectangle */
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
   XCopyArea (TtDisplay, imageDesc->PicPixmap, pix, TtDialogueGC, pxorig, pyorig,
	      wif, hif, xif, yif);

   /* Affichage dans la boite */
   GetXYOrg (frame, &XOrg, &YOrg);
#endif /* NEW_WILLOWS */
   xif = box->BxXOrg + FrameTable[frame].FrLeftMargin - XOrg;
   yif = box->BxYOrg + FrameTable[frame].FrTopMargin - YOrg;
   wif = box->BxWidth;
   hif = box->BxHeight;
   CentreImage (w, h, wif, hif, RealSize, &x, &y, &pxorig, &pyorig);
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
   CopyOnScreen (pix, drawable, pxorig, pyorig, w, h, x, y);
   XFreePixmap (TtDisplay, pix);
   pix = None;
   XSetLineAttributes (TtDisplay, TtLineGC, 1, LineSolid, CapButt, JoinMiter);
   XDrawRectangle (TtDisplay, drawable, TtLineGC, xif, yif, wif - 1, hif - 1);
   /* writing the filename */
   BaseName (imageDesc->PicFileName, filename, 0, 0);
   fileNameWidth = XTextWidth ((XFontStruct *) FontMenu, filename, strlen (filename));
   if ((fileNameWidth + wlogo <= wif) && (FontHeight (FontMenu) + hlogo <= hif))
     {
	fnposx = (wif - fileNameWidth) / 2 + xif;
	fnposy = hif - 5 + yif;
	XSetFont (TtDisplay, TtLineGC, ((XFontStruct *) FontMenu)->fid);
	XDrawString (TtDisplay, drawable, TtLineGC, fnposx, fnposy, filename, strlen (filename));
     }
#endif /* NEW_WILLOWS */
}


/* ------------------------------------------------------------------------ */
/* | On dessine une image dans une fenetre frame                          | */
/* ------------------------------------------------------------------------ */
#ifdef __STDC__
void                DrawImage (PtrBox box, PictInfo * imageDesc, int frame)
#else  /* __STDC__ */
void                DrawImage (box, imageDesc, frame)
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
  GetImageFileName (imageDesc->PicFileName, fileName);

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

  IsCropOk (&PicWArea, &PicHArea, wif, hif, imageDesc);
  if (!Printing)
    {
      SetCursorWatch (frame);
      if (imageDesc->PicPixmap == EpsfPictureLogo)
	DrawImageBox (box, imageDesc, frame, epsflogo_width, epsflogo_height);
      else
	{
	  if (!PixmapIsOk (box, imageDesc))
	    {
	      ReadImage (frame, box, imageDesc);
	      myDrawable = imageDesc->PicPixmap;
	    }
	  else
	    {
	      /* affichage de l'image dans la boite */
	      if (imageDesc->PicPresent != FillFrame)
		CentreImage (PicWArea, PicHArea, wif, hif, pres, &xtranslate, &ytranslate, &pxorig, &pyorig);

	      if (imageDesc->PicMask)
		{
		  XSetClipOrigin (TtDisplay, TtGraphicGC, xif - pxorig + xtranslate, yif - pyorig + ytranslate);
		  XSetClipMask (TtDisplay, TtGraphicGC, imageDesc->PicMask);
		}
	      if (PicWArea < wif)
		wif = PicWArea;
	      if (PicHArea < hif)
		hif = PicHArea;
	      CopyOnScreen (imageDesc->PicPixmap, drawable, pxorig, pyorig,
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
/* | Demande au driver une image au format specifie dans imageDesc      | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                ReadImage (int frame, PtrBox box, PictInfo * imageDesc)
#else  /* __STDC__ */
void                ReadImage (frame, box, imageDesc)
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
   GetImageFileName (imageDesc->PicFileName, fileName);
   typeImage = imageDesc->PicType;

   status = FileIsOk (fileName, &typeImage);
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
		    w = box->BxWidth;	/* utilise' par cgm */
		    h = box->BxHeight;
		 }

	       if (!Printing)
		 {
		    if (box != NULL)
		       /* Positionne les couleurs du graphicGC */
		       if (TtWDepth == 1)
			 {
			    /* Ecran Noir et Blanc */
			    XSetForeground (TtDisplay, TtGraphicGC, Black_Color);
			    XSetBackground (TtDisplay, TtGraphicGC,
				       ColorPixel (BackgroundColor[frame]));
			 }
		       else if (box->BxAbstractBox->AbSensitive && !box->BxAbstractBox->AbPresentationBox)
			 {
			    /* Couleur des boites actives */
			    XSetForeground (TtDisplay, TtGraphicGC, Box_Color);
			    XSetForeground (TtDisplay, GCpicture, Box_Color);
			    XSetBackground (TtDisplay, TtGraphicGC, ColorPixel (box->BxAbstractBox->AbBackground));
			 }
		       else
			 {
			    /* Couleur de la boite */
			    XSetForeground (TtDisplay, TtGraphicGC, ColorPixel (box->BxAbstractBox->AbForeground));
			    XSetForeground (TtDisplay, GCpicture, ColorPixel (box->BxAbstractBox->AbForeground));
			    XSetBackground (TtDisplay, TtGraphicGC, ColorPixel (box->BxAbstractBox->AbBackground));
			 }
		 }

	       Bgcolor = ColorPixel (box->BxAbstractBox->AbBackground);

	       myDrawable = (*(PictureHandlerTable[typeImage].
			       Produce_Picture)) (fileName, pres, &xif, &yif, &wif, &hif, Bgcolor, &PicMask);

	       noCroppingFrame = ((wif == 0) && (hif == 0));
	       /* utilise' pour le cgm */


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
			      /*large = PixelToPoint (box->BxWidth); */
			      /*haut = PixelToPoint (box->BxHeight); */
			      NouvDimImage (box->BxAbstractBox);
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
      SetImageDescPixmap (imageDesc, myDrawable, PicMask);


#endif /* NEW_WILLOWS */
}				/*ReadImage */


/* ------------------------------------------------------------------- */
/* | On traduit l'image dans un format de sortie                     | */
/* ------------------------------------------------------------------- */
#ifdef __STDC__
void                PrintImage (int typeImage, char *name, PictureScaling pres, int xif, int yif, int wif, int hif, int PicXArea, int PicYArea, int PicWArea, int PicHArea, FILE * fd, unsigned long BackGroundPixel)

#else  /* __STDC__ */
void                PrintImage (typeImage, name, pres, xif, yif, wif, hif, PicXArea, PicYArea, PicWArea, PicHArea, fd, BackGroundPixel)
int                 typeImage;
char               *name;
PictureScaling           pres;
int                 xif;
int                 yif;
int                 wif;
int                 hif;
int                 PicXArea;
int                 PicYArea;
FILE               *fd;
unsigned long       BackGroundPixel;
#endif /* __STDC__ */
{
   char                fileName[1023];

   GetImageFileName (name, fileName);
   (*(PictureHandlerTable[typeImage].Produce_Postscript)) (fileName, pres, xif, yif, wif, hif, PicXArea, PicYArea, PicWArea, PicHArea, fd, BackGroundPixel);

}				/*PrintImage */

/* ---------------------------------------------------------------------- */
/* |    FreeImage libere l'image du descripteur.                        | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                FreeImage (PictInfo * imageDesc)
#else  /* __STDC__ */
void                FreeImage (imageDesc)
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
}				/*FreeImage */



/* ---------------------------------------------------------------------- */
/* |    GetImageType retourne le type d'une image en fonction de        | */
/* |            l'index dans le menu type d'image.                      | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
int                 GetImageType (int GUIIndex)
#else  /* __STDC__ */
int                 GetImageType (GUIIndex)
int                 menuIndex;
#endif /* __STDC__ */
{
   if (GUIIndex == 0)
      return UNKNOWN_FORMAT;
   else
      /* repose sur l'implementation de GetPictureHandlersList */
      return PictureMenuType[GUIIndex];

}				/*GetImageType */

/* ---------------------------------------------------------------------- */
/* |    GetImTypeIndex retourne l'index du menu type d'image d'apres    | */
/* |            le type d'une image. Si le type est inconnu, on         | */
/* |            retourne 0.                                             | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
int                 GetImTypeIndex (int PicType)
#else  /* __STDC__ */
int                 GetImTypeIndex (PicType)
int                 PicType;
#endif /* __STDC__ */
{
   int                 i = 0;

   if (PicType == UNKNOWN_FORMAT)
      return 0;

   while (i <= HandlersCounter)
     {
	if (PictureMenuType[i] == PicType)
	   return i;
	else
	   i++;
	/* repose sur l'implementation de GetPictureHandlersList */
     }
   return 0;
}				/*GetImTypeIndex */

/* ---------------------------------------------------------------------- */
/* |    GetImPresIndex retourne l'index du menu presentation d'apres    | */
/* |            la presentation d'une image. Si la presentation est     | */
/* |            inconnue, on retourne RealSize.                 | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
int                 GetImPresIndex (PictureScaling PicPresent)
#else  /* __STDC__ */
int                 GetImPresIndex (PicPresent)
PictureScaling           PicPresent;
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
   /* repose sur l'implementation de GetPictureHandlersList */
   /* le define des presentations possibles est dans le meme ordre que */
   /* le menu image */

   return i;
}				/*GetImPresIndex */

/* ---------------------------------------------------------------------- */
/* |    GetImagePresentation retourne la presentation d'une image en    | */
/* |            fonction de l'index dans le menu presentation.          | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
PictureScaling           GetImagePresentation (int menuIndex)
#else  /* __STDC__ */
PictureScaling           GetImagePresentation (menuIndex)
int                 menuIndex;
#endif /* __STDC__ */
{
   return (PictureScaling) menuIndex;

}				/*GetImagePresentation */

/* ---------------------------------------------------------------------- */
/* |    GetPictureHandlersList cree dans buffer une liste des drivers      | */
/* |            utilisables. Cette fonction sert a` creer le menu       | */
/* |            image. On retourne dans count le nombre de drivers.     | */
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

  GetImageFileName(imageFile, fileName);
  typeImage = GetImageFileFormat(fileName);
  myDrawable = (*(PictureHandlerTable[typeImage].
		  Produce_Picture))(fileName, pres, &xif, &yif, &wif, &hif, Bgcolor,
		                &PicMask );
  XSetWindowBackgroundPixmap(TtDisplay,w,myDrawable);
  FreePixmap(myDrawable);
  FreePixmap(PicMask);
 ****************************************/
   return (0);
}				/*TtaSetMainThotWindowBackgroundImage */
