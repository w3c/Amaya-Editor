/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2004
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * Handle specific presentation of pictures
 * Authors: I. Vatton (INRIA)
 *          N. Layaida (INRIA) - New picture formats
 *          R. Guetari (W3C/INRIA) - Windows version
 *
 */
#include "thot_gui.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "picture.h"

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "picture_tv.h"

#include "inites_f.h"
#include "memory_f.h"
#include "picture_f.h"

#ifdef _GL
#include "glwindowdisplay.h"
#endif /*_GL*/


/*----------------------------------------------------------------------
  CleanPictInfo frees the picture information but not the structure itself
  ----------------------------------------------------------------------*/
void CleanPictInfo (PictInfo *imageDesc)
{
  if (imageDesc)
    {
#ifndef _GL
      if (imageDesc->PicPixmap != None)
	{
	  FreePixmap (imageDesc->PicPixmap);
	  imageDesc->PicPixmap = None;
#ifdef _WINGUI
	  FreePixmap (imageDesc->PicMask);
#endif /* _WINGUI */ 
#ifdef _GTK
	  /*Frees imlib struct that contains the real ref to pics */
	  if (imageDesc->im)
	    gdk_imlib_destroy_image (imageDesc->im);
	  imageDesc->im = NULL;
#endif /* _GTK */
	  imageDesc->PicMask = None;
	}
#ifdef _WINGUI
	imageDesc->PicBgMask = -1;
#endif /* _WINGUI */
#else /*_GL*/
	FreeGlTexture (imageDesc);
#endif /*_GL*/
	imageDesc->PicWArea = 0;
	imageDesc->PicHArea = 0;
	imageDesc->PicWidth = 0;
	imageDesc->PicHeight = 0;
    }
}


/*----------------------------------------------------------------------
   NewPictInfo cree un descripteur par element image.       
   Si le pointeur sur le descripteur existe deja (champ    
   ElPictInfo dans l'element), la procedure recopie 
   le champ ElPictInfo dans le pave.                
   Si le pointeur sur le descripteur n'existe pas, la      
   procedure commence par creer le descripteur.            
  ----------------------------------------------------------------------*/
void NewPictInfo (PtrAbstractBox pAb, PathBuffer filename, int imagetype)
{
  PictInfo           *imageDesc = NULL;
  char               *ptr = NULL;
  PictureScaling      picPresent;
  int                 len;

  picPresent = DefaultPres;
  if (!pAb->AbPresentationBox && pAb->AbElement->ElLeafType == LtPicture)
    {
      /* image element -> attach the element descriptor to the abtract box */
      imageDesc = (PictInfo *) pAb->AbElement->ElPictInfo;
      if (imageDesc == NULL)
	{
	  /* Create the element descriptor */
	  imageDesc = (PictInfo *) TtaGetMemory (sizeof (PictInfo));
	  memset (imageDesc, 0, sizeof (PictInfo));
	  pAb->AbElement->ElPictInfo = (int *) imageDesc;
	  pAb->AbPictInfo = (int *) imageDesc;
	}
      else
	{
	  /* don't reset the presentation value */
	  pAb->AbPictInfo = pAb->AbElement->ElPictInfo;
	  return;
	}
      ptr = filename;
    }
  else if (pAb->AbPresentationBox)
    {
      /*  It's a presentation box -> Create the descriptor */
      imageDesc = (PictInfo *) pAb->AbPictInfo;
      if (imageDesc == NULL)
	{
	  imageDesc = (PictInfo *) TtaGetMemory (sizeof (PictInfo));
	  memset (imageDesc, 0, sizeof (PictInfo));
	  pAb->AbPictInfo = (int *) imageDesc;
	}
      else
	/* don't reset the presentation value */
	picPresent = imageDesc->PicPresent;
      ptr = filename;
    }
  else if (pAb->AbLeafType == LtCompound)
    {
      /*  It's a background image -> Create the descriptor */
      imageDesc = (PictInfo *) pAb->AbPictBackground;
      if (imageDesc == NULL)
	{
	  imageDesc = (PictInfo *) TtaGetMemory (sizeof (PictInfo));
	  memset (imageDesc, 0, sizeof (PictInfo));
	  pAb->AbPictBackground = (int *) imageDesc;
	}
      else
	{
	  /* don't reset the presentation value */
	  picPresent = imageDesc->PicPresent;
	  ptr = imageDesc->PicFileName;
	}
 
       /* create the text buffer */
      if (filename == NULL)
	{
	  if (ptr)
	    {
	      TtaFreeMemory (ptr);
	      ptr = NULL;
	    }
	}
      else
	{
	  len = strlen (filename) + 1;
	  if (ptr == NULL || len > (int) strlen (ptr) + 1)
	    {
	      if (imageDesc->PicFileName == ptr)
		imageDesc->PicFileName = NULL;
	      TtaFreeMemory (ptr);
	      ptr = (char *)TtaGetMemory (len);
	    }
	  strcpy (ptr, filename);
	}
    }
  else
    return;

  if (imageDesc && (imageDesc->PicFileName != ptr || ptr == NULL))
    {
      /* reinitialize the image descriptor */
      CleanPictInfo (imageDesc);
      imageDesc->PicFileName = ptr;
      imageDesc->PicType    = imagetype;
      imageDesc->PicPresent = picPresent;
    }
}


/*----------------------------------------------------------------------
   Copie d'un PictInfo                                      
  ----------------------------------------------------------------------*/
void CopyPictInfo (int *Imdcopie, int *Imdsource)
{
   PictInfo           *imagec;
   PictInfo           *images;

   imagec = (PictInfo *) Imdcopie;
   images = (PictInfo *) Imdsource;
   imagec->PicWArea = images->PicWArea;
   imagec->PicHArea = images->PicHArea;
   imagec->PicWidth = images->PicWidth;
   imagec->PicHeight = images->PicHeight;
   imagec->PicPresent = images->PicPresent;
   imagec->PicType = images->PicType;
#ifdef _GL
   imagec->TextureBind = images->TextureBind;
#endif /*_GL*/
}




