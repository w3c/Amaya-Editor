/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2001.
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
void NewPictInfo (PtrAbstractBox pAb, PathBuffer filename, int imagetype)
{
  PtrTextBuffer       pBuffer;
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

  if (imageDesc && imageDesc->PicFileName != ptr)
    {
      /* Initialize image descriptor */
      /* use the buffer allocated by the picture content */
#ifndef _GTK
#ifdef _WINDOWS
      imageDesc->PicMask = -1;
#else /* _WINDOWS */
      FreePixmap (imageDesc->PicMask);
      imageDesc->PicMask = None;
#endif /* _WINDOWS */
      FreePixmap (imageDesc->PicPixmap);
#endif /* !_GTK */
      imageDesc->PicFileName = ptr;
      imageDesc->PicType    = imagetype;
      imageDesc->PicPresent = picPresent;
    }
}


/*----------------------------------------------------------------------
  CleanPictInfo  frees the picture information but not the structure itself
  ----------------------------------------------------------------------*/
void CleanPictInfo (PictInfo *imageDesc)
{
   if (imageDesc)
     {
#ifndef _GTK
       if (imageDesc->PicPixmap != None)
#endif /* !_GTK */
	 {
#ifndef _GTK
#ifdef _WINDOWS
	   imageDesc->PicMask = -1;
#else /* _WINDOWS */
	   FreePixmap (imageDesc->PicMask);
	   imageDesc->PicMask = None;
#endif /* _WINDOWS */
	   FreePixmap (imageDesc->PicPixmap);
	   imageDesc->PicPixmap = None;
#endif /* !_GTK */
	   imageDesc->PicXArea = 0;
	   imageDesc->PicYArea = 0;
	   imageDesc->PicWArea = 0;
	   imageDesc->PicHArea = 0;
	   imageDesc->PicWidth = 0;
	   imageDesc->PicHeight = 0;
	 }
       if (imageDesc->PicType >= InlineHandlers &&
	   PictureHandlerTable[imageDesc->PicType].FreePicture)
	 (*(PictureHandlerTable[imageDesc->PicType].FreePicture)) (imageDesc);
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
   imagec->PicXArea = images->PicXArea;
   imagec->PicYArea = images->PicYArea;
   imagec->PicWArea = images->PicWArea;
   imagec->PicHArea = images->PicHArea;
   imagec->PicWidth = images->PicWidth;
   imagec->PicHeight = images->PicHeight;
   imagec->PicPresent = images->PicPresent;
   imagec->PicType = images->PicType;
}




