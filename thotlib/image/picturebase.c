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

#define THOT_EXPORT extern
#include "frame_tv.h"
#include "platform_tv.h"

#include "fileaccess_f.h"
#include "picture_f.h"
#include "memory_f.h"

/* defined in picture.c and not exported ... */
extern PictureHandler  PictureHandlerTable[MAX_PICT_FORMATS];
extern int             InlineHandlers;

/*
 * All the registred images are stored in a small base
 * made of an hash table.
 */

typedef struct ImageBaseEntry {
    struct ImageBaseEntry *next;
    char *name;
    Pixmap pix;
} ImageBaseEntry, *ImageBaseEntryPtr;

static ImageBaseEntryPtr ImageBaseHash[256];
static int NbImageEntry = 0;
static int ImageBaseHashInitialized = 0;

/*----------------------------------------------------------------------
   GetHash

   Get the hash table entry for an image name.

   Parameters:
   name : the name of the image.

   Return value:
   the index [0..255]

  ----------------------------------------------------------------------*/
#ifdef __STDC__
static int          GetHash (CONST char *name)
#else  /* __STDC__ */
static int          GetHash (name)
CONST char         *name;

#endif /* __STDC__ */
{
    unsigned char idx = 0;

    while (*name != EOS) idx += (unsigned char) *name++;
    return((int) idx);
}

/*----------------------------------------------------------------------
   InitImageBase

   Initialize the image base.

   Parameters:
   none

   Return value:
   none

  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void          InitImageBase (void)
#else  /* __STDC__ */
static void          InitImageBase ()

#endif /* __STDC__ */
{
    int i;

    if (ImageBaseHashInitialized) return;
    for (i = 0;i < 256;i++) ImageBaseHash[i] = NULL;
    NbImageEntry = 0;
    ImageBaseHashInitialized = 1;
}

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
    ImageBaseEntryPtr prev = NULL, cour;
    int hash;
    int res;

    if (name == NULL) return;
    if (pix == 0) return;

    if (name[0] == EOS) return;
    if (!ImageBaseHashInitialized) InitImageBase();

    hash = GetHash(name);
    cour = ImageBaseHash[hash];

    /*
     * go through the list looking for the right
     * place to insert it.
     */
    while (cour != NULL) {
        res = strcmp(cour->name, name);
        if (res <= 0) break;
        prev = cour;
        cour = cour->next;
    }

    /*
     * allocate and fill a new structure.
     */
    NbImageEntry++;
    cour = (ImageBaseEntryPtr) TtaGetMemory (sizeof(struct ImageBaseEntry));
    cour->name = TtaStrdup (name);
    cour->pix = pix;

    /*
     * insert it in the hash table.
     */
    if (prev == NULL) {
        /*
         * add at the head of the hash list.
         */
        cour->next = ImageBaseHash[hash];
        ImageBaseHash[hash] = cour;
    } else {
        /*
         * add if after prev.
         */
        cour->next = prev->next;
        prev->next = cour;
    }
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
    int                 typeImage;
    char                fileName[1023];
    PictureScaling      pres = RealSize;
    int                 xif = 0;
    int                 yif = 0;
    int                 wif, hif;
    Drawable            PicMask = None;
    Drawable            myDrawable = None;
    Picture_Report      status;
    unsigned long       Bgcolor = 0;

    if (name == NULL)
      return(0);
    if (path == NULL)
      return(0);

    if (name[0] == EOS)
      return(0);
    if (path[0] == EOS)
      return(0);

    GetPictureFileName (path, fileName);
    typeImage = UNKNOWN_FORMAT;

    status = PictureFileOk (fileName, &typeImage);
    switch ((int)status)
      {
      case Supported_Format:
	if ((PictureHandlerTable[typeImage].Produce_Picture != NULL) && (typeImage < InlineHandlers))
	  myDrawable = (*(PictureHandlerTable[typeImage].Produce_Picture)) (fileName, pres, &xif, &yif, &wif, &hif, Bgcolor, &PicMask);
	break;
      case Corrupted_File:
      case Unsupported_Format:
	return(0);
	break;
      }
    TtaRegisterPixmap (name, (Pixmap) myDrawable);
    return ((Pixmap) myDrawable);
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
    ImageBaseEntryPtr cour;
    int hash;
    int res;

    if (name == NULL) return(0);
    if (name[0] == EOS) return(0);

    if (!ImageBaseHashInitialized) InitImageBase();

    hash = GetHash(name);
    cour = ImageBaseHash[hash];

    /*
     * go through the list looking for the right
     * place to insert it.
     */
    while (cour != NULL) {
        res = strcmp(cour->name, name);
        if (res == 0) {
	    return(cour->pix);
	}
        if (res <= 0) break;
        cour = cour->next;
    }
    return(0);
}

