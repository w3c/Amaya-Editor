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
 * Handle specific presentation of pictures
 * Authors: I. Vatton (INRIA)
 *          N. Layaida (INRIA) - New picture formats
 *          R. Guetari (W3C/INRIA) - Unicode and Windows version
 *
 */

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


/*----------------------------------------------------------------------
   NewPictInfo cree un descripteur par element image.       
   Si le pointeur sur le descripteur existe deja (champ    
   ElPictInfo dans l'element), la procedure recopie 
   le champ ElPictInfo dans le pave.                
   Si le pointeur sur le descripteur n'existe pas, la      
   procedure commence par creer le descripteur.            
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                NewPictInfo (PtrAbstractBox pAb, char* filename, int imagetype)
#else  /* __STDC__ */
void                NewPictInfo (pAb, filename, imagetype)
PtrAbstractBox      pAb;
char*               filename;
int                 imagetype;
#endif /* __STDC__ */
{
  PtrTextBuffer       pBuffer;
  PictInfo           *image = NULL;
  char*               ptr = NULL;
  int                 picPresent, len;

  if (imagetype == XBM_FORMAT || imagetype == XPM_FORMAT)
    picPresent = RealSize;
  else
    picPresent = ReScale;

  if (!pAb->AbPresentationBox &&
      pAb->AbElement->ElTerminal && pAb->AbElement->ElLeafType == LtPicture)
    {
      /* image element -> attach the element descriptor to the abtract box */
      image = (PictInfo *) pAb->AbElement->ElPictInfo;
      if (image == NULL)
	  {
	    /* Create the element descriptor */
	    image = (PictInfo *) TtaGetMemory (sizeof (PictInfo));
	    image->PicColors = NULL;
	    pAb->AbElement->ElPictInfo = (int *) image;
	  }
      else
	/* don't reset the presentation value */
	picPresent = image->PicPresent;

      pAb->AbPictInfo = pAb->AbElement->ElPictInfo;
      if (filename == NULL)
	{
	  GetTextBuffer (&pBuffer);
	  pAb->AbElement->ElText = pBuffer;
	  ptr = &pBuffer->BuContent[0];
	}
      else
	ptr = filename;
     }
   else if (pAb->AbPresentationBox)
     {
       /*  It's a presentation box -> Create the descriptor */
      image = (PictInfo *) pAb->AbPictInfo;
      if (image == NULL)
	{
	  image = (PictInfo *) TtaGetMemory (sizeof (PictInfo));
	  image->PicColors = NULL;
	  pAb->AbPictInfo = (int *) image;
	}
      else
	/* don't reset the presentation value */
	picPresent = image->PicPresent;

       ptr = filename;
     }
   else if (pAb->AbLeafType == LtCompound)
     {
       /*  It's a background image -> Create the descriptor */
      image = (PictInfo *) pAb->AbPictBackground;
      if (image == NULL)
	{
	  image = (PictInfo *) TtaGetMemory (sizeof (PictInfo));
	  image->PicColors = NULL;
	  pAb->AbPictBackground = (int *) image;
	}
      else
	{
	  /* don't reset the presentation value */
	  picPresent = image->PicPresent;
	  ptr = image->PicFileName;
	}
 
       /* create the text buffer */
      if (filename == NULL)
	{
	  if (ptr != NULL)
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
	      TtaFreeMemory (ptr);
	      ptr = TtaGetMemory (len);
	    }
	   strcpy (ptr, filename);
	}
     }

  if (image)
    {
      /* Initialize image descriptor */
      /* use the buffer allocated by the picture content */
      image->PicFileName = ptr;
      image->PicPixmap = 0;
#     ifndef _WINDOWS 
      image->PicMask = 0;
#     endif /* _WINDOWS */
      image->PicType = imagetype;
      image->PicPresent = (PictureScaling) picPresent;
      image->PicXArea = 0;
      image->PicYArea = 0;
      image->PicWArea = 0;
      image->PicHArea = 0;
      image->PicWidth = 0;
      image->PicHeight = 0;
      image->mapped = FALSE;
      image->created = FALSE;
    }
}


/*----------------------------------------------------------------------
  FreePictInfo  frees the picture information but not the structure itself
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                FreePictInfo (PictInfo * imageDesc)
#else  /* __STDC__ */
void                FreePictInfo (imageDesc)
PictInfo           *imageDesc;
#endif /* __STDC__ */
{
  int        i;

   if (imageDesc)
     {
       if (imageDesc->PicPixmap != None)
	 {
#          ifndef _WINDOWS
	   FreePixmap (imageDesc->PicMask);
	   imageDesc->PicMask = None;
#          endif /* _WINDOWS */
	   FreePixmap (imageDesc->PicPixmap);
	   imageDesc->PicPixmap = None;
	   imageDesc->PicXArea = 0;
	   imageDesc->PicYArea = 0;
	   imageDesc->PicWArea = 0;
	   imageDesc->PicHArea = 0;
	   imageDesc->PicWidth = 0;
	   imageDesc->PicHeight = 0;
	   if (imageDesc->PicColors != NULL)
	     for (i = 0; i < imageDesc->PicNbColors; i++)
	       TtaFreeThotColor (imageDesc->PicColors[i]);
	   TtaFreeMemory (imageDesc->PicColors);
	   imageDesc->PicColors = NULL;
	 }

       if ((imageDesc->PicType >= InlineHandlers) &&
	   (PictureHandlerTable[imageDesc->PicType].FreePicture != NULL))
	 (*(PictureHandlerTable[imageDesc->PicType].FreePicture)) (imageDesc);
     }
}



/*----------------------------------------------------------------------
   Copie d'un PictInfo                                      
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CopyPictInfo (int *Imdcopie, int *Imdsource)
#else  /* __STDC__ */
void                CopyPictInfo (Imdcopie, Imdsource)
int                *Imdcopie;
int                *Imdsource;
#endif /* __STDC__ */
{
   PictInfo           *imagec;
   PictInfo           *images;

   imagec = (PictInfo *) Imdcopie;
   images = (PictInfo *) Imdsource;
   imagec->PicXArea = images->PicXArea;
   imagec->PicYArea = images->PicYArea;
   imagec->PicWArea = images->PicWArea;
   imagec->PicHArea = images->PicHArea;
   imagec->PicWidth = images->PicWidth;
   imagec->PicHeight = images->PicHeight;
   imagec->PicPresent = images->PicPresent;
   imagec->PicType = images->PicType;
   imagec->PicColors = NULL;
}

