/*
 * Copyright (c) 1996 INRIA, All rights reserved
 */

/*
 * Gestion de la presentation specifique des images
 */
#include "thot_sys.h"


#include "constmedia.h"
#include "typemedia.h"
#include "picture.h"

#include "picture_f.h"
#include "memory_f.h"

/*----------------------------------------------------------------------
   Retourne un pointeur sur la regle de pres specifique de l'elt   
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static PtrPRule     FindPictInfo (PtrElement pEl)

#else  /* __STDC__ */
static PtrPRule     FindPictInfo (pEl)
PtrElement          pEl;

#endif /* __STDC__ */

{
   PtrPRule            pRegle = NULL;
   PtrPRule            pR = NULL;
   boolean             new = FALSE;

   if (pEl->ElFirstPRule == NULL)
     {
	/* cet element n'a aucune regle de presentation specifique, on en */
	/* cree une et on la chaine a l'element */
	GetPresentRule (&pRegle);
	new = (pRegle != NULL);
	pEl->ElFirstPRule = pRegle;
     }
   else
      /* cherche parmi les regles de presentation specifiques de
         l'element si ce type de regle existe pour la vue
         a laquelle appartient le pave. */
     {
	pR = pEl->ElFirstPRule;	/* premiere regle specifique de
				 * l'element */
	while (pRegle == NULL)
	  {
	     if (pR->PrType == PtPictInfo)
		/* la regle existe deja */
		pRegle = pR;
	     else if (pR->PrNextPRule != NULL)
		/* passe a la regle specifique suivante de l'element */
		pR = pR->PrNextPRule;
	     else
	       {
		  /* On a examine' toutes les regles specifiques de */
		  /* l'element, ajoute une nouvelle regle en fin de chaine */
		  GetPresentRule (&pRegle);
		  new = (pRegle != NULL);
		  pR->PrNextPRule = pRegle;
	       }
	  }
     }
   if (new)
     {
	pRegle->PrType = PtPictInfo;
	pRegle->PrNextPRule = NULL;
	pRegle->PrViewNum = 0;
	pRegle->PrSpecifAttr = 0;
	pRegle->PrSpecifAttrSSchema = NULL;
	pRegle->PrPresMode = PresImmediate;
	pRegle->PrPictInfo.PicXArea = 0;
	pRegle->PrPictInfo.PicYArea = 0;
	pRegle->PrPictInfo.PicWArea = 0;
	pRegle->PrPictInfo.PicHArea = 0;
	pRegle->PrPictInfo.PicPresent = RealSize;
	pRegle->PrPictInfo.PicType = XBM_FORMAT;
     }
   return pRegle;

}				/*FindPictInfo */


/*----------------------------------------------------------------------
   
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                SetImageRule (PtrElement pEl, int x, int y, int w, int h, int typeimage, PictureScaling presimage)

#else  /* __STDC__ */
void                SetImageRule (pEl, x, y, w, h, typeimage, presimage)
PtrElement          pEl;
int                 x;
int                 y;
int                 w;
int                 h;
PictureScaling      presimage;
int                 typeimage;

#endif /* __STDC__ */

{
   PtrPRule            pRegle;

   pRegle = FindPictInfo (pEl);
   if (pRegle != NULL)
     {
	pRegle->PrPictInfo.PicXArea = x;
	pRegle->PrPictInfo.PicYArea = y;
	pRegle->PrPictInfo.PicWArea = w;
	pRegle->PrPictInfo.PicHArea = h;
	pRegle->PrPictInfo.PicPresent = presimage;
	pRegle->PrPictInfo.PicType = typeimage;
     }

}				/*SetImageRule */


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
   PictInfo           *image;

   if (ppav->AbElement->ElTerminal && ppav->AbElement->ElLeafType == LtPicture)
     {
	/* C'est un element image -. accroche le descripteur a l'element */
	if (ppav->AbElement->ElPictInfo == NULL)
	  {
	     /* Creation du descripteur */
	     image = (PictInfo *) TtaGetMemory (sizeof (PictInfo));
	     if (filename == NULL)
	       {
		  GetTextBuffer (&pBuffer);
		  ppav->AbElement->ElText = pBuffer;
		  filename = &pBuffer->BuContent[0];
	       }
	     image->PicFileName = filename;
	     image->PicPixmap = 0;
	     image->PicMask = 0;
	     image->PicPresent = RealSize;
	     image->PicType = imagetype;
	     image->PicXArea = 0;
	     image->PicYArea = 0;
	     image->PicWArea = 0;
	     image->PicHArea = 0;
	     ppav->AbElement->ElPictInfo = (int *) image;
	  }
	ppav->AbPictInfo = ppav->AbElement->ElPictInfo;
     }
   else
     {
	/*  Ce n'est pas un element image -> Creation du descripteur */
	image = (PictInfo *) TtaGetMemory (sizeof (PictInfo));
	if (filename == NULL)
	  {
	     GetTextBuffer (&pBuffer);
	     ppav->AbElement->ElText = pBuffer;
	     filename = &pBuffer->BuContent[0];
	  }
	image->PicFileName = filename;
	image->PicPixmap = 0;
	image->PicMask = 0;
	image->PicPresent = RealSize;
	image->PicType = imagetype;
	image->PicXArea = 0;
	image->PicYArea = 0;
	image->PicWArea = 0;
	image->PicHArea = 0;
	ppav->AbPictInfo = (int *) image;
     }

}				/*NewPictInfo */


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
   PictInfo           *image;

   if (desc != NULL)
     {
	image = (PictInfo *) desc;
	FreePicture (image);
	TtaFreeMemory ((char *) image);
     }

}				/*FreePictInfo */



/*----------------------------------------------------------------------
   FnCopy d'un PictInfo                                      
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
   imagec->PicPresent = images->PicPresent;
   imagec->PicType = images->PicType;
}
