/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/* authors:
 * - Ramzi GUETARI (W3C/INRIA)
 * - Nabil LAYAIDA
 * - Loay  SABRY-ISMAIL
 *
 * Last modification: March 26 1997
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

#define THOT_EXPORT extern
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
#include "pluginapi_f.h"

extern PluginInfo*    pluginTable [100];                     /* Loaded plug-ins info are stored in pluginTable */
extern int            pluginCounter ;                        /* Counter of loaded plug-ins                     */
extern PictureHandler PictureHandlerTable[MAX_PICT_FORMATS]; /* Image and plug-ins drivers                     */
extern int            PictureIdType[MAX_PICT_FORMATS];
extern int            PictureMenuType[MAX_PICT_FORMATS];
extern char*          pluginPath;
extern int            currentExtraHandler;                   /* Index of urrent running plug-in                */

int                   pluginIndex;
char*                 amayaPluginDir;                        /* Value of environment variable AMAYA_PLUGIN_DIR */

#ifdef __STDC__
static int Ap_OpenPluginDriver (boolean model, int indexHandler)
#else /* __STDC__ */
static int Ap_OpenPluginDriver (model, indexHandler)
boolean model;
int     indexHandler;
#endif /* __STDC__ */
{
  int res = Ap_InitializePlugin (pluginTable [indexHandler]->pluginDL, indexHandler);

  return res ;
}

/* ----------------------------------------------------------------------
   AP_ClosePluginDriver
   ---------------------------------------------------------------------- */

#ifdef __STDC__
void AP_ClosePluginDriver (void)
#else /* __STDC__ */
void AP_ClosePluginDriver ()
#endif /* __STDC__ */
{
}


/* ---------------------------------------------------------------------- 
   Ap_InitImage
   ---------------------------------------------------------------------- */

#ifdef __STDC__
void Ap_InitImage (void)
#else /* __STDC__ */
void Ap_InitImage ()
#endif /* __STDC__ */
{
}

/* ----------------------------------------------------------------------
   Ap_DrawPicture
   ---------------------------------------------------------------------- */

#ifdef __STDC__
void Ap_DrawPicture (PtrBox box, PictInfo* imageDesc, int xif, int yif)
#else /* __STDC__ */
void Ap_DrawPicture (imageDesc, xif, yif)
PictInfo* imageDesc;
int       xif;
int       yif;
#endif /* __STDC__ */
{
#ifndef _WINDOWS
    int   n;
    Arg   arg[10];
    
    if ((!imageDesc->created) && (!imageDesc->mapped)) {
       n = 0;
       XtSetArg (arg[n], XmNx, xif ); n++;
       XtSetArg (arg[n], XmNy, yif ); n++;
       XtSetValues ((Widget) (imageDesc->wid), arg, n);
       XtManageChild ((Widget) (imageDesc->wid));
       /* IMPORTANT: We have to add a support for NP_FULL plug-ins */
       /* Ap_CreatePluginInstance (imageDesc, TtDisplay, NP_FULL); */
       Ap_CreatePluginInstance (box, imageDesc, TtDisplay, NP_EMBED);
       imageDesc->mapped  = TRUE;
       imageDesc->created = TRUE;
    } else {
	 XtMoveWidget((Widget) (imageDesc->wid), (Position) xif, (Position) yif);
	 if (!imageDesc->mapped) {
            XtMapWidget ((Widget) (imageDesc->wid));
            imageDesc->mapped = TRUE;
         }
   }
#endif /* !_WINDOWS */
}

/* ----------------------------------------------------------------------
   Ap_ProducePicture
   ---------------------------------------------------------------------- */
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
  Pixmap  bitmap = None;
  ThotWidget  canvas;
  int     n;
  Arg     arg[10];
    
#ifndef _WINDOWS
  n = 0;
  XtSetArg (arg[n], XmNwidth, imageDesc->PicWArea); n++;
  XtSetArg (arg[n], XmNheight, imageDesc->PicHArea); n++;
  XtSetArg (arg[n], XmNborderWidth, 0); n++;
  XtSetArg (arg[n], XmNmarginWidth, 0); n++;
  XtSetArg (arg[n], XmNmarginHeight, 0); n++;
  canvas = (Widget) XmCreateDrawingArea (FrameTable[frame].WdFrame, "Dummy", arg, n);
  XtRealizeWidget (canvas);
  imageDesc->wid = canvas;

  bitmap = XCreatePixmap (TtDisplay, TtRootWindow, imageDesc->PicWArea, imageDesc->PicHArea, TtWDepth);
  
  if (bitmap == None) 
     return (Drawable) None;

  imageDesc->PicPixmap   = bitmap;
  imageDesc->PicMask     = None;
  imageDesc->PicXArea    = 0;
  imageDesc->PicYArea    = 0;
  /* imageDesc->created     = FALSE;*/ /* A VERIFIER */
  imageDesc->mapped      = FALSE;
  return (Drawable) bitmap;
#endif /* _WINDOWS */
#endif /* !NEW_WILLOWS */
}


/* ----------------------------------------------------------------------
   Ap_ProducePostscript
   ---------------------------------------------------------------------- */

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
}

/*----------------------------------------------------------------------
  Ap_MatchFormat
  ----------------------------------------------------------------------*/

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
  int    l; 

  char    suffix [10];
  int     index1 = 0;
  int     index2;
  boolean matched = FALSE;

  l = strlen (fileName);
  if (l > 4 && pluginTable [currentExtraHandler]->fileExt) {
     while (!matched && pluginTable [currentExtraHandler]->fileExt [index1] != '\0') {
           index2 = 0;
           suffix [index2++] = '.';
           while ((pluginTable [currentExtraHandler]->fileExt [index1] != '\0') && 
                  (pluginTable [currentExtraHandler]->fileExt [index1] != ',')) 
                 suffix [index2++] = pluginTable [currentExtraHandler]->fileExt [index1++];
           suffix [index2] = '\0';
           if (pluginTable [currentExtraHandler]->fileExt [index1] == ',')
              index1++;
           if (!strcmp (fileName + l - 4, suffix)) matched = TRUE;
     }
  }
  return matched ;
#endif /* !NEW_WILLOWS */
}


/*----------------------------------------------------------------------
   Private Initializations of picture handlers and the visual type 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int InitPluginHandlers (boolean printing, int indexHandler)
#else  /* __STDC__ */
int InitPluginHandlers (printing, indexHandler)
boolean printing;
int     indexHandler;
#endif /* __STDC__ */
{
   int res;

   PictureHandlerTable[HandlersCounter].Produce_Picture    = Ap_ProducePicture;
   PictureHandlerTable[HandlersCounter].Produce_Postscript = Ap_ProducePostscript;
   PictureHandlerTable[HandlersCounter].DrawPicture        = Ap_DrawPicture;
   PictureHandlerTable[HandlersCounter].Match_Format       = Ap_MatchFormat;
   PictureHandlerTable[HandlersCounter].FreePicture        = Ap_FreePicture;

   PictureIdType[HandlersCounter]                          = PLUGIN_FORMAT;
   PictureMenuType[HandlersCounter]                        = PLUGIN_FORMAT;

   res = Ap_OpenPluginDriver (printing, indexHandler) ;
   if (res != -1)
       HandlersCounter++;
   /* printf ("HandlersCounter = %d\n", HandlersCounter) ; */
   return res;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void PluginLoadResources (void)
#else  /* __STDC__ */
void PluginLoadResources ()
#endif
{
   amayaPluginDir = TtaGetEnvString ("AMAYA_PLUGIN_DIR");

   TtaBrowsePluginDirectory ();
}

