/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/* authors (alphabetical order):
 * - Ramzi GUETARI
 * - Nabil LAYAIDA
 * - Loay  SABRY-ISMAIL
 *
 * Last modification: Jan 09 1997
 */

#include "thot_gui.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "picture.h"
#include "frame.h"
#include "interface.h"

#include "npapi.h"
#include "npupp.h"
#include "pluginbrowse.h"

#define EXPORT extern
#include "boxes_tv.h"
#include "appli_f.h"
#include "tree_f.h"
#include "views_f.h"
#include "picture_tv.h"
#include "frame_tv.h"

#include "platform_f.h"
#include "font_f.h"
#include "frame_f.h"
#include "picture_f.h"
#include "inites_f.h"
#include "fileaccess_f.h"
#include "pluginbrowse_f.h"

/* ---------------------------------------------------------------------- */
/* |	BitmapOpenImageDrvr loading the driver.				| */
/* ---------------------------------------------------------------------- */

extern PluginInfo*    pluginTable [100];
extern int            pluginCounter ;
extern PictureHandler PictureHandlerTable[MAX_PICT_FORMATS];
extern int            PictureIdType[MAX_PICT_FORMATS];
extern int            PictureMenuType[MAX_PICT_FORMATS];
extern boolean        Printing;
extern Pixmap         EpsfPictureLogo;
extern char*          pluginPath;

int                   pluginIndex;
extern int            currentPlugin;
static Pixmap         PictureLogo;

#ifdef __STDC__
int Ap_OpenPluginDriver (boolean model, int indexHandler)
#else /* __STDC__ */
int Ap_OpenPluginDriver (model, indexHandler)
boolean model;
int     indexHandler;
#endif /* __STDC__ */
{
  int refNum;

  Ap_InitializePlugin (pluginTable [indexHandler]->pluginDL, indexHandler) ;
#ifdef NEW_WILLOWS
  return(0);
#else  /* NEW_WILLOWS */
  refNum = GetPictTypeIndex (HandlersCounter - 1);
  return refNum;
#endif /* !NEW_WILLOWS */
}/*FigOpenImageDrvr*/

/* ---------------------------------------------------------------------- */
/* |	FigCloseImageDrvr close the driver.				| */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void AP_ClosePluginDriver (void)
#else /* __STDC__ */
void AP_ClosePluginDriver ()
#endif /* __STDC__ */
{
}/*FigCloseImageDrvr*/


/* ---------------------------------------------------------------------- */
/* |	FigInitImage: initializes the driver for an image		| */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void Ap_InitImage (void)
#else /* __STDC__ */
void Ap_InitImage ()
#endif /* __STDC__ */
{
}/*FigInitImage*/

/* ---------------------------------------------------------------------- */
/* |	FigDrawImage draws a fing within a drawable.		        | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void Ap_DrawPicture (PictInfo* imageDesc, int xif, int yif)
#else /* __STDC__ */
void Ap_DrawPicture (imageDesc, xif, yif)
PictInfo* imageDesc;
int       xif;
int       yif;
#endif /* __STDC__ */
{
    int   n;
    Arg   arg[10];
    char* fileName;
    
    fileName = (char*) malloc (sizeof (char) * 1024);
    GetPictureFileName (imageDesc->PicFileName, fileName);

    if ((!imageDesc->created) && (!imageDesc->mapped)) {
       n = 0;
       XtSetArg (arg[n], XmNx, xif ); n++;
       XtSetArg (arg[n], XmNy, yif ); n++;
       XtSetValues ((Widget) (imageDesc->wid), arg, n);
       XtManageChild ((Widget) (imageDesc->wid));
       Ap_CreatePluginInstance (XtWindow ((Widget) (imageDesc->wid)), TtDisplay, fileName);
       imageDesc->mapped  = TRUE;
       imageDesc->created = TRUE;
    } else {
	 XtMoveWidget((Widget) (imageDesc->wid), (Position) xif, (Position) yif);
	 if (!imageDesc->mapped) {
            XtMapWidget ((Widget) (imageDesc->wid));
            imageDesc->mapped = TRUE;
         }
   }
}/*FigDrawImage*/

/* ---------------------------------------------------------------------- */
/* |	FigCreateImage lit et retourne le Fig lu dans le fichier 	| */
/* |		fileName. Met a` jour xif, yif, wif, hif.	       	| */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
Drawable Ap_ProducePicture (int frame, PictInfo* imageDesc, char* fileName)
#else /* __STDC__ */
Drawable Ap_ProducePicture (frame, imageDesc, fileName)
int       frame;
PictInfo* imageDesc;
char*     fileName;
#endif /* __STDC__ */
{
#ifdef NEW_WILLOWS
  return(NULL);
#else  /* NEW_WILLOWS */
  int     status;
  int     w, h; 
  Pixmap  bitmap = None;
  int     xHot, yHot;
  Widget  canvas;
  int     n;
  Arg     arg[10];
    
  n = 0;
  XtSetArg (arg[n], XmNwidth, 400); n++;
  XtSetArg (arg[n], XmNheight, 400); n++;
  XtSetArg (arg[n], XmNborderWidth, 0); n++;
  XtSetArg (arg[n], XmNmarginWidth, 0); n++;
  XtSetArg (arg[n], XmNmarginHeight, 0); n++;
  canvas = (Widget) XmCreateDrawingArea (FrameTable[frame].WdFrame, "Dummy", arg, n);
  /*canvas = (Widget) XmCreateScrolledWindow (FrameTable[frame].WdFrame, "Dummy", arg, n);*/
  /*canvas = (Widget) XmCreateMainWindow (FrameTable[frame].WdFrame, "Dummy", arg, n);*/
  imageDesc->wid = canvas;

  /*CreateInstance(XtWindow((Widget)canvas), TtDisplay, fileName);*/

  bitmap = XCreatePixmap (TtDisplay, TtRootWindow, 400, 400, TtWDepth);
  
  if (bitmap == None) 
     return (Drawable) None;
  else {
       imageDesc->PicPixmap   = bitmap;
       imageDesc->PicMask     = None;
       imageDesc->PicXArea    = 0;
       imageDesc->PicYArea    = 0;
       imageDesc->PicWArea    = 400;
       imageDesc->PicHArea    = 400;
       /* imageDesc->created     = FALSE;*/ /* A VERIFIER */
       imageDesc->mapped      = FALSE;
       return (Drawable) bitmap;
  }
#endif /* !NEW_WILLOWS */
}/*FigCreateImage*/


/* ---------------------------------------------------------------------- */
/* |	FigPrintImage convertit un Fig en PostScript.		        | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void Ap_ProducePostscript (char* fileName, PictureScaling pres, int xif, int yif, int wif, int hif, int xcf, int ycf, int wcf, int hcf, int fd, unsigned int BackGroundPixel)
#else /* __STDC__ */
void Ap_ProducePostscript (fileName, pres, xif, yif, wif, hif, xcf, ycf, wcf, hcf, fd, BackGroundPixel)
char*        fileName;
PictureScaling pres;
int          xif;
int          yif;
int          wif;
int          hif;
int          xcf;
int          ycf;
int          wcf;
int          hcf;
int          fd;
unsigned int BackGroundPixel;
#endif /* __STDC__ */
{
}/*BitmapPrintImage*/

/* ---------------------------------------------------------------------- */
/* |	BitmapIsFormat teste si un fichier contient un bitmap X11.	| */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
boolean Ap_MatchFormat (char* fileName)
#else /* __STDC__ */
boolean Ap_MatchFormat (fileName)
char* fileName;
#endif /* __STDC__ */
{
#ifdef NEW_WILLOWS
  return FALSE;
#else  /* NEW_WILLOWS */
  int    status;
  int    w, h; 
  Pixmap bitmap = None;
  int    l, xHot, yHot;

  char    suffix [10];
  int     index1 = 0;
  int     index2;
  boolean matched = FALSE;
  /* TODO - TODO - TODO - TODO - TODO - TODO - TODO - TODO - TODO - TODO - TODO - TODO - TODO - TODO */
  /* TODO - TODO - TODO - TODO - TODO - TODO - TODO - TODO - TODO - TODO - TODO - TODO - TODO - TODO */
  /* TODO - TODO - TODO - TODO - TODO - TODO - TODO - TODO - TODO - TODO - TODO - TODO - TODO - TODO */
  /* TODO - TODO - TODO - TODO - TODO - TODO - TODO - TODO - TODO - TODO - TODO - TODO - TODO - TODO */

  l = strlen (fileName);
  if (l > 4 && pluginTable [currentPlugin]->fileExt) {
     while (!matched && pluginTable [currentPlugin]->fileExt [index1] != '\0') {
           index2 = 0;
           suffix [index2++] = '.';
           while ((pluginTable [currentPlugin]->fileExt [index1] != '\0') && 
                  (pluginTable [currentPlugin]->fileExt [index1] != ',')) 
                 suffix [index2++] = pluginTable [currentPlugin]->fileExt [index1++];
           suffix [index2] = '\0';
           if (pluginTable [currentPlugin]->fileExt [index1] == ',')
              index1++;
           if (!strcmp (fileName + l - 4, suffix)) matched = TRUE;
     }
  }
  return matched ;
#endif /* !NEW_WILLOWS */
}/*BitmapIsFormat*/


/*----------------------------------------------------------------------
   Private Initializations of picture handlers and the visual type 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                InitPluginHandlers (boolean printing, int indexHandler)
#else  /* __STDC__ */
void                InitPluginHandlers (printing, indexHandler)
boolean printing;
int     indexHandler;
#endif /* __STDC__ */
{
   Printing = printing;
   /* strncpy (PictureHandlerTable[indexHandler].GUI_Name, TclName, MAX_FORMAT_NAMELENGHT); */
   PictureHandlerTable[HandlersCounter].Produce_Picture    = Ap_ProducePicture;
   PictureHandlerTable[HandlersCounter].Produce_Postscript = Ap_ProducePostscript;
   PictureHandlerTable[HandlersCounter].DrawPicture        = Ap_DrawPicture;
   PictureHandlerTable[HandlersCounter].Match_Format       = Ap_MatchFormat;

   PictureIdType[HandlersCounter]                          = PLUGIN_FORMAT;
   PictureMenuType[HandlersCounter]                        = PLUGIN_FORMAT;

   HandlersCounter++;

   Ap_OpenPluginDriver (printing, indexHandler) ;
}

#ifdef __STDC__
void PluginLoadResources (void)
#else  /* __STDC__ */
void PluginLoadResources ()
#endif
{
   TtaBrowsePluginDirectory ();
}

