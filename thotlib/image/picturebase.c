/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 
/*
 * Small image database management
 *
 * Author: D. Veillard (INRIA)
 *
 */

#include "thot_gui.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "frame.h"
#include "picture.h"
#include "fileaccess.h"
#include "content.h"
#ifdef _WINDOWS
#define FOR_MSW
#endif /* _WINDOWS */
#include "xpmP.h"
#include "xpm.h"

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "frame_tv.h"
#include "platform_tv.h"

#include "fileaccess_f.h"
#include "picture_f.h"

typedef struct PixmapBaseEntry {
    char *name;
    Pixmap pix;
} PixmapBaseEntry, *PixmapBaseEntryPtr;

/* defined in picture.c and not exported ... */
extern PictureHandler  PictureHandlerTable[MAX_PICT_FORMATS];
extern int             InlineHandlers;

#define MAX_PIXMAP 100
static PixmapBaseEntry PixmapBase[MAX_PIXMAP];
static int NbPixmapEntry = 0;

/*----------------------------------------------------------------------
   TtaRegisterPixmap

   Register a pre-loaded Pixmap into the database.

   Parameters:
   name : the name of the image.
   pix : pointer to the existing pixmap.

   Return value:
   none

  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaRegisterPixmap (char *name, Pixmap pix)
#else  /* __STDC__ */
void                TtaRegisterPixmap (name, pix)
char               *name;
Pixmap              pix;

#endif /* __STDC__ */

{
   if (name == NULL) return;
   if (pix == 0) return;

   if (name[0] == '\0') return;

    PixmapBase[NbPixmapEntry].name = name;
    PixmapBase[NbPixmapEntry].pix = pix;
    NbPixmapEntry++;
}

/*----------------------------------------------------------------------
   TtaLoadImage

   Load an image from disk into the database.

   Parameters:
   name : the name of the image.
   path : the full pathname referencing the image file.

   Return value:
   The created pixmap

  ----------------------------------------------------------------------*/
#ifdef __STDC__
Pixmap              TtaLoadImage (char *name, char *path)
#else  /* __STDC__ */
Pixmap              TtaLoadImage (name, path)
char               *name;
char               *path;

#endif /* __STDC__ */

{
#ifndef _WINDOWS
    int                 typeImage;
    char                fileName[1023];
    PictureScaling      pres = RealSize;
    int                 xif = 0;
    int                 yif = 0;
    int                 wif, hif;
    Drawable            PicMask = None;
    Drawable            myDrawable = None;
    Picture_Report      status;
    unsigned long       Bgcolor;

    if (name == NULL) return(0);
    if (path == NULL) return(0);

    if (name[0] == '\0') return(0);
    if (path[0] == '\0') return(0);

    GetPictureFileName (path, fileName);
    typeImage = unknown_type;

    status = PictureFileOk (fileName, &typeImage);
    switch (status) {
	case (int)Supported_Format:
	    if ((PictureHandlerTable[typeImage].Produce_Picture != NULL) &&
	        (typeImage < InlineHandlers)) {
		  myDrawable = (*(PictureHandlerTable[typeImage].Produce_Picture)) (fileName, pres, &xif, &yif, &wif, &hif, Bgcolor, &PicMask);
	    }
	    break;
	case (int) Corrupted_File:
	case (int) Unsupported_Format:
	    return(0);
	    break;
    }
    PixmapBase[NbPixmapEntry].name = name;
    PixmapBase[NbPixmapEntry].pix = (Pixmap) myDrawable;
    NbPixmapEntry++;
    return((Pixmap) myDrawable);
#endif /* _WINDOWS */
    return(0);
}

/*----------------------------------------------------------------------
   TtaGetImage

   Get an image from the database.

   Parameters:
   name : the name of the image.

   Return value:
   The pixmap or NULL if not found.

  ----------------------------------------------------------------------*/
#ifdef __STDC__
Pixmap              TtaGetImage (char *name)
#else  /* __STDC__ */
Pixmap              TtaGetImage (name)
char               *name;

#endif /* __STDC__ */

{
    int i;

    for (i = 0; i < NbPixmapEntry;i++) 
        if (!strcmp(name, PixmapBase[i].name))
	    return(PixmapBase[i].pix);
    return(0);
}

