
#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "lost.xpm"
#include "picture.h"
#include "thotconfig.h"
#include "frame.h"

#include "pixmapdrvr.h"
#include "gifdrvr.h"
#include "jpegdrvr.h"
#include "pngdrvr.h"
#include "bitmapdrvr.h"
#include "epsfdrvr.h"
#include "epsflogo.h"
#include "interface.h"

typedef enum
{
  ExisteTypeOK,
  MauvaisType,
  ExistePas
} FileStatus;

typedef struct
{
  char     menuName[MAX_FORMAT_NAMELENGHT];
  Drawable (*CreateImage) ();
  void     (*PrintImage) ();
  boolean  (*IsFormat) ();
} PictureHandler;

#define EXPORT extern
#include "img.var"
#include "frame.var"
#include "font.var"
#include "environ.var"

static boolean      Printing;
ThotGC       GCpicture;	/* for bitmap */
char               *SuffixImage[] =
{".xbm", ".eps", ".xpm", ".gif", ".jpg", ".png"};
THOT_VInfo          THOT_vInfo;
#ifndef NEW_WILLOWS
XVisualInfo        *vptr;
Visual             *theVisual;
#endif

static PictureHandler  PictureHandlerTable[MAX_PICT_FORMATS];
static int          ImageIDType[MAX_PICT_FORMATS];
static int          ImageMenuType[MAX_PICT_FORMATS];
static char        *ImageMenu;
static int          ImageDrvrCount;
static Pixmap       ImageLostPixmapID;
static Pixmap       ImageBadPixmapID;

Pixmap              ImageEPSFPixmapID;

static char         reverseByte[0x100] =
{
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

#include "appli.f"
#include "arbabs.f"
#include "bitmapdrvr.f"
#include "docvues.f"
#include "dofile.f"
#include "epsfdrvr.f"
#include "filesystem.f"
#include "font.f"
#include "fen.f"
#include "gifdrvr.f"
#include "jpegdrvr.f"
#include "imagedrvr.f"
#include "pixmapdrvr.f"
#include "pngdrvr.f"
#include "pres.f"
#include "inites.f"

/* ---------------------------------------------------------------------- */
/* |    IsFormat retourne True si le fichier de nom fileName contient une     | */
/* |            image de type typeImage. False sinon.                   | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static boolean      IsFormat (int typeImage, char *fileName)
#else  /* __STDC__ */
static boolean      IsFormat (typeImage, fileName)
int                 typeImage;
char               *fileName;
#endif /* __STDC__ */
{
  if (PictureHandlerTable[typeImage].IsFormat != NULL)
    return (*(PictureHandlerTable[typeImage].IsFormat)) (fileName);
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
      *b = reverseByte[*b];
      b++;
    }
  while (--n > 0);
}


/* ---------------------------------------------------------------------- */
/* |    FreePixmap detruit le pixmap Pix s'il est non vide et different | */
/* |            des images predefinies LostPixmap et BadPixmap.         | */
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
       && (pix != ImageLostPixmapID)
       && (pix != ImageBadPixmapID)
       && (pix != ImageEPSFPixmapID))
      XFreePixmap (GDp (0), pix);
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
/* |            pxorig, pyorig en fonction des valeur de PicWArea, PicHArea, wif, | */
/* |            hif et pres.                                            | */
/* |            - Si on presente ReScale, la translation se    | */
/* |            fait dans une seule direction.                          | */
/* |            - Si on presente FillFrame, on ne translate pas.       | */
/* |            - Si on presente RealSize on translate pour     | */
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
      /*TODO : valeur 32768 pour essayer de pallier aux imageDesc archifaux */
      /* obtenus avec plusieurs vues */
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
      XCopyArea (GDp (0), SrcPix, Drawab, graphicGC (0), srcorx, srcory, w, h, desorx, desory);
#endif /* NEW_WILLOWS */

}




/* ---------------------------------------------------------------------- */
/* |    ImageLostPixmap retourne le pixmap Picture perdue.                | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static Pixmap       ImageLostPixmap ()
#else  /* __STDC__ */
static Pixmap       ImageLostPixmap ()
#endif				/* __STDC__ */
{
   return ImageLostPixmapID;
}

/* ---------------------------------------------------------------------- */
/* |    ImageBadPixmap retourne le pixmap Picture incorrecte.             | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static Pixmap       ImageBadPixmap ()
#else  /* __STDC__ */
static Pixmap       ImageBadPixmap ()
#endif				/* __STDC__ */
{
   return ImageBadPixmapID;
}


/* ---------------------------------------------------------------------- */
/* |    PixmapIsOk retourne False si le pixmap contenu dans imageDesc   | */
/* |            est vide. On retourne True s'il est egal aux images     | */
/* |            predefinies LostPixmap et BadPixmap.                    | */
/* |            - Si on presente RealSize, on retourne True.    | */
/* |            - Si on presente ReScale, on retourne True si  | */
/* |            la boite box possede au moins une dimension egale a`    | */
/* |            celle du pixmap.                                        | */
/* |            - Si on presente FillFrame, on retourne True si la     | */
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

   pixmapok = True;
   if ((imageDesc->PicPixmap == ImageLostPixmapID)
       || (imageDesc->PicPixmap == ImageBadPixmapID)
       || (imageDesc->PicPixmap == ImageEPSFPixmapID))
      return True;

   if (imageDesc->PicPixmap == None)
     {
	return False;
     }
   else
     {
#ifndef NEW_WILLOWS
	XGetGeometry (GDp (0), imageDesc->PicPixmap, &root, &xpix, &ypix, &wpix, &hpix, &bdw, &dep);
#endif /* NEW_WILLOWS */
	switch (imageDesc->PicPresent)
	      {
		 case RealSize:
		    pixmapok = True;	/* tout au plus un centrage */
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
	if (IsFormat (i, fileName))
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
/* |    FileIsOk retourne ExistePas si le fichier fileName n'existe pas.      | */
/* |            - Si typeImage est defini, on retourne ExisteTypeOK si  | */
/* |            le fichier est du type typeImage, MauvaisType sinon.    | */
/* |            - Si typeImage est non defini, on le met a jour et on   | */
/* |            retourne ExisteTypeOK si le fichier est d'un type connu | */
/* |            et MauvaisType sinon.                                   | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static FileStatus   FileIsOk (char *fileName, int *typeImage)
#else  /* __STDC__ */
static FileStatus   FileIsOk (fileName, typeImage)
char               *fileName;
int                *typeImage;
#endif /* __STDC__ */

{
   FileStatus          status;

   /* on ne prend que des types connus */
   if (*typeImage >= MAX_PICT_FORMATS || *typeImage < 0)
      *typeImage = UNKNOWN_FORMAT;

   if (FileExist (fileName))
     {
	if (*typeImage == UNKNOWN_FORMAT)
	  {
	     *typeImage = GetImageFileFormat (fileName);
	     if (*typeImage == UNKNOWN_FORMAT)
		status = MauvaisType;
	     else
		status = ExisteTypeOK;
	  }
	else
	  {
	     if (IsFormat (*typeImage, fileName))
		status = ExisteTypeOK;
	     else
		status = MauvaisType;
	  }
     }
   else
     {
	status = ExistePas;
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
   graphicGC (0) = XCreateGC (GDp (0), GRootW (0), 0, NULL);
   XSetForeground (GDp (0), graphicGC (0), Black_Color);
   XSetBackground (GDp (0), graphicGC (0), White_Color);
   XSetGraphicsExposures (GDp (0), graphicGC (0), False);

   GCpicture = XCreateGC (GDp (0), GRootW (0), 0, NULL);
   XSetForeground (GDp (0), GCpicture, Black_Color);
   XSetBackground (GDp (0), GCpicture, White_Color);
   XSetGraphicsExposures (GDp (0), GCpicture, False);

   ImageLostPixmapID = TtaCreatePixmapLogo (lost_xpm);
   ImageBadPixmapID = TtaCreatePixmapLogo (lost_xpm);
   ImageEPSFPixmapID = XCreatePixmapFromBitmapData (GDp (0), GRootW (0),
						    epsflogo_bits,
						    epsflogo_width,
				  epsflogo_height, Black_Color, White_Color,
						    Gdepth (0));
   vinfo.visualid = XVisualIDFromVisual (XDefaultVisual (GDp (0),
							 ThotScreen (0)));
   vptr = XGetVisualInfo (GDp (0), VisualIDMask, &vinfo, &i);
   THOT_vInfo.class = vptr->class;
   THOT_vInfo.depth = vptr->depth;
   theVisual = DefaultVisual (GDp (0), ThotScreen (0));
#endif /* !NEW_WILLOWS */

   Printing = printing;
   i = 0;
   strncpy (PictureHandlerTable[i].menuName, BitmapName, MAX_FORMAT_NAMELENGHT);
   PictureHandlerTable[i].CreateImage = BitmapCreateImage;
   PictureHandlerTable[i].PrintImage = BitmapPrintImage;
   PictureHandlerTable[i].IsFormat = BitmapIsFormat;

   ImageIDType[i] = XBM_FORMAT;
   ImageMenuType[i] = XBM_FORMAT;
   i++;

   strncpy (PictureHandlerTable[i].menuName, EPSFName, MAX_FORMAT_NAMELENGHT);
   PictureHandlerTable[i].CreateImage = EPSFCreateImage;
   PictureHandlerTable[i].PrintImage = EPSFPrintImage;
   PictureHandlerTable[i].IsFormat = EPSFIsFormat;

   ImageIDType[i] = EPS_FORMAT;
   ImageMenuType[i] = EPS_FORMAT;
   i++;

   strncpy (PictureHandlerTable[i].menuName, PixmapName, MAX_FORMAT_NAMELENGHT);
   PictureHandlerTable[i].CreateImage = PixmapCreateImage;
   PictureHandlerTable[i].PrintImage = PixmapPrintImage;
   PictureHandlerTable[i].IsFormat = PixmapIsFormat;

   ImageIDType[i] = XPM_FORMAT;
   ImageMenuType[i] = XPM_FORMAT;
   i++;

   strncpy (PictureHandlerTable[i].menuName, GifName, MAX_FORMAT_NAMELENGHT);
   PictureHandlerTable[i].CreateImage = GifCreateImage;
   PictureHandlerTable[i].PrintImage = GifPrintImage;
   PictureHandlerTable[i].IsFormat = GifIsFormat;

   ImageIDType[i] = GIF_FORMAT;
   ImageMenuType[i] = GIF_FORMAT;
   i++;

   strncpy (PictureHandlerTable[i].menuName, JpegName, MAX_FORMAT_NAMELENGHT);
   PictureHandlerTable[i].CreateImage = JpegCreateImage;
   PictureHandlerTable[i].PrintImage = JpegPrintImage;
   PictureHandlerTable[i].IsFormat = JpegIsFormat;

   ImageIDType[i] = JPEG_FORMAT;
   ImageMenuType[i] = JPEG_FORMAT;
   i++;

   strncpy (PictureHandlerTable[i].menuName, PngName, MAX_FORMAT_NAMELENGHT);
   PictureHandlerTable[i].CreateImage = PngCreateImage;
   PictureHandlerTable[i].PrintImage = PngPrintImage;
   PictureHandlerTable[i].IsFormat = PngIsFormat;

   ImageIDType[i] = PNG_FORMAT;
   ImageMenuType[i] = PNG_FORMAT;
   i++;
   ImageDrvrCount = i;
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
 
   *count = ImageDrvrCount;
   while (i < ImageDrvrCount)
     {
        item = PictureHandlerTable[i].menuName;
        strcpy (buffer + index, item);
        index += strlen (item) + 1;
        i++;
     }
   buffer = ImageMenu;
 
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
   pix = XCreatePixmap (GDp (0), GRootW (0), w, h, Gdepth (0));
   XFillRectangle (GDp (0), pix, GCblack (0), x, y, w, h);

   /* putting the cross */
   XDrawRectangle (GDp (0), pix, GCdialogue (0), x, y, w - 1, h - 1);
   XDrawLine (GDp (0), pix, GCdialogue (0), x, y, x + w - 1, y + h - 2);
   XDrawLine (GDp (0), pix, GCdialogue (0), x + w - 1, y, x, y + h - 2);
   XDrawLine (GDp (0), pix, GCwhite (0), x, y + 1, x + w - 1, y + h - 1);
   XDrawLine (GDp (0), pix, GCwhite (0), x + w - 1, y + 1, x, y + h - 1);
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
   XCopyArea (GDp (0), imageDesc->PicPixmap, pix, GCdialogue (0), pxorig, pyorig,
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
   XFreePixmap (GDp (0), pix);
   pix = None;
   XSetLineAttributes (GDp (0), GCtrait (0), 1, LineSolid, CapButt, JoinMiter);
   XDrawRectangle (GDp (0), drawable, GCtrait (0), xif, yif, wif - 1, hif - 1);
   /* writing the filename */
   BaseName (imageDesc->PicFileName, filename, 0, 0);
   fileNameWidth = XTextWidth ((XFontStruct *) FontMenu, filename, strlen (filename));
   if ((fileNameWidth + wlogo <= wif) && (FontHeight (FontMenu) + hlogo <= hif))
     {
	fnposx = (wif - fileNameWidth) / 2 + xif;
	fnposy = hif - 5 + yif;
	XSetFont (GDp (0), GCtrait (0), ((XFontStruct *) FontMenu)->fid);
	XDrawString (GDp (0), drawable, GCtrait (0), fnposx, fnposy, filename, strlen (filename));
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
      if (imageDesc->PicPixmap == ImageEPSFPixmapID)
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
		  XSetClipOrigin (GDp (0), graphicGC (0), xif - pxorig + xtranslate, yif - pyorig + ytranslate);
		  XSetClipMask (GDp (0), graphicGC (0), imageDesc->PicMask);
		}
	      if (PicWArea < wif)
		wif = PicWArea;
	      if (PicHArea < hif)
		hif = PicHArea;
	      CopyOnScreen (imageDesc->PicPixmap, drawable, pxorig, pyorig,
			    wif, hif, xif + xtranslate, yif + ytranslate);
	      if (imageDesc->PicMask)
		{
		  XSetClipMask (GDp (0), graphicGC (0), None);
		  XSetClipOrigin (GDp (0), graphicGC (0), 0, 0);
		}
	    }
	}
      ResetCursorWatch (frame);
    }
  else if (typeImage < ImageDrvrCount && typeImage > -1)
    (*(PictureHandlerTable[typeImage].PrintImage)) (fileName, pres, xif, yif, wif, hif, PicXArea, PicYArea, PicWArea, PicHArea,
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
   FileStatus          status;
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
	    case (int) ExisteTypeOK:
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
		       if (Gdepth (0) == 1)
			 {
			    /* Ecran Noir et Blanc */
			    XSetForeground (GDp (0), graphicGC (0), Black_Color);
			    XSetBackground (GDp (0), graphicGC (0),
				       ColorPixel (BackgroundColor[frame]));
			 }
		       else if (box->BxAbstractBox->AbSensitive && !box->BxAbstractBox->AbPresentationBox)
			 {
			    /* Couleur des boites actives */
			    XSetForeground (GDp (0), graphicGC (0), Box_Color);
			    XSetForeground (GDp (0), GCpicture, Box_Color);
			    XSetBackground (GDp (0), graphicGC (0), ColorPixel (box->BxAbstractBox->AbBackground));
			 }
		       else
			 {
			    /* Couleur de la boite */
			    XSetForeground (GDp (0), graphicGC (0), ColorPixel (box->BxAbstractBox->AbForeground));
			    XSetForeground (GDp (0), GCpicture, ColorPixel (box->BxAbstractBox->AbForeground));
			    XSetBackground (GDp (0), graphicGC (0), ColorPixel (box->BxAbstractBox->AbBackground));
			 }
		 }

	       Bgcolor = ColorPixel (box->BxAbstractBox->AbBackground);

	       myDrawable = (*(PictureHandlerTable[typeImage].
			       CreateImage)) (fileName, pres, &xif, &yif, &wif, &hif, Bgcolor, &PicMask);

	       noCroppingFrame = ((wif == 0) && (hif == 0));
	       /* utilise' pour le cgm */


	       if (myDrawable == None)
		 {
		    myDrawable = ImageBadPixmap ();
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
			      /*large = PixelEnPt(box->BxWidth, 1); */
			      /*haut = PixelEnPt(box->BxHeight, 0); */
			      NouvDimImage (box->BxAbstractBox);
			   }
			 w = wif;
			 h = hif;
		      }
		 }
	       break;
	    case (int) MauvaisType:
	    case (int) ExistePas:
	       myDrawable = ImageLostPixmap ();
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

   if (!Printing || imageDesc->PicPixmap != ImageEPSFPixmapID)
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
   (*(PictureHandlerTable[typeImage].PrintImage)) (fileName, pres, xif, yif, wif, hif, PicXArea, PicYArea, PicWArea, PicHArea, fd, BackGroundPixel);

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
int                 GetImageType (int menuIndex)
#else  /* __STDC__ */
int                 GetImageType (menuIndex)
int                 menuIndex;
#endif /* __STDC__ */
{
   if (menuIndex == 0)
      return UNKNOWN_FORMAT;
   else
      /* repose sur l'implementation de GetPictureHandlersList */
      return ImageMenuType[menuIndex];

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

   while (i <= ImageDrvrCount)
     {
	if (ImageMenuType[i] == PicType)
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

   *count = ImageDrvrCount;
   while (i < ImageDrvrCount)
     {
	item = PictureHandlerTable[i].menuName;
	strcpy (buffer + index, item);
	index += strlen (item) + 1;
	i++;
     }
   buffer = ImageMenu;

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
		  CreateImage))(fileName, pres, &xif, &yif, &wif, &hif, Bgcolor,
		                &PicMask );
  XSetWindowBackgroundPixmap(GDp(0),w,myDrawable);
  FreePixmap(myDrawable);
  FreePixmap(PicMask);
 ****************************************/
   return (0);
}				/*TtaSetMainThotWindowBackgroundImage */
