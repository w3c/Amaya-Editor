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
 *
 */
#include "thot_sys.h"


#include "constmedia.h"
#include "typemedia.h"
#include "picture.h"

#include "picture_f.h"
#include "memory_f.h"


/*----------------------------------------------------------------------
   NewPictInfo cree un descripteur par element image.       
   Si le pointeur sur le descripteur existe deja (champ    
   ElPictInfo dans l'element), la procedure recopie 
   le champ ElPictInfo dans le pave.                
   Si le pointeur sur le descripteur n'existe pas, la      
   procedure commence par creer le descripteur.            
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                NewPictInfo (PtrAbstractBox ppav, char *filename, int imagetype)
#else  /* __STDC__ */
void                NewPictInfo (ppav, filename, imagetype)
PtrAbstractBox      ppav;
char               *filename;
int                 imagetype;
#endif /* __STDC__ */
{
  PtrTextBuffer       pBuffer;
  PictInfo           *image = NULL;
  char               *ptr = NULL;
  int                 picPresent;

  if (imagetype == XBM_FORMAT || imagetype == XPM_FORMAT)
    picPresent = RealSize;
  else
    picPresent = ReScale;

  if (ppav->AbElement->ElTerminal && ppav->AbElement->ElLeafType == LtPicture)
    {
      /* image element -> attach the element descriptor to abtract box */
      image = (PictInfo *) ppav->AbElement->ElPictInfo;
      if (image == NULL)
	  {
	    /* Create the element descriptor */
	    image = (PictInfo *) TtaGetMemory (sizeof (PictInfo));
	    ppav->AbElement->ElPictInfo = (int *) image;
	  }
      else
	/* don't reset the presentation value */
	picPresent = image->PicPresent;

      ppav->AbPictInfo = ppav->AbElement->ElPictInfo;
      if (filename == NULL)
	{
	  GetTextBuffer (&pBuffer);
	  ppav->AbElement->ElText = pBuffer;
	  ptr = &pBuffer->BuContent[0];
	}
      else
	ptr = filename;
     }
   else if (ppav->AbPresentationBox)
     {
       /*  It's not an image element -> Create the descriptor */
      image = (PictInfo *) ppav->AbPictInfo;
      if (image == NULL)
	{
	  image = (PictInfo *) TtaGetMemory (sizeof (PictInfo));
	  ppav->AbPictInfo = (int *) image;
	}
      else
	/* don't reset the presentation value */
	picPresent = image->PicPresent;

       ptr = filename;
     }
   else if (ppav->AbLeafType == LtCompound)
     {
       /*  It's not an image element -> Create the descriptor */
      image = (PictInfo *) ppav->AbPictBackground;
      if (image == NULL)
	{
	  image = (PictInfo *) TtaGetMemory (sizeof (PictInfo));
	  ppav->AbPictBackground = (int *) image;
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
	  if (ptr == NULL)
	    ptr = TtaGetMemory (strlen (filename) + 1);
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
   FreePictInfo libere le descriteur d'image.               
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                FreePictInfo (int *desc)
#else  /* __STDC__ */
void                FreePictInfo (desc)
int                *desc;
#endif /* __STDC__ */
{
   PictInfo        *image;

   if (desc != NULL)
     {
	image = (PictInfo *) desc;
	FreePicture (image);
	TtaFreeMemory (image);
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
}

