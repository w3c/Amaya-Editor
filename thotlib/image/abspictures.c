
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

/*
 * Gestion de la presentation specifique des images
 * IV : Refonte du code des images Janv 1995
 *
 */
#include "thot_sys.h"

#include "functions.h"
#include "constmedia.h"
#include "typemedia.h"
#include "picture.h"

#include "imagedrvr.f"
#include "memory.f"

/* ---------------------------------------------------------------------- */
/* |    Retourne un pointeur sur la regle de pres specifique de l'elt   | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static PtrPRule FindImageDescriptor (PtrElement pEl)

#else  /* __STDC__ */
static PtrPRule FindImageDescriptor (pEl)
PtrElement          pEl;

#endif /* __STDC__ */

{
   PtrPRule        pRegle = NULL;
   PtrPRule        pR = NULL;
   Bool                new = False;

   if (pEl->ElFirstPRule == NULL)
     {
	/* cet element n'a aucune regle de presentation specifique, on en */
	/* cree une et on la chaine a l'element */
	GetReglePres (&pRegle);
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
	     if (pR->PrType == PtImDescr)
		/* la regle existe deja */
		pRegle = pR;
	     else if (pR->PrNextPRule != NULL)
		/* passe a la regle specifique suivante de l'element */
		pR = pR->PrNextPRule;
	     else
	       {
		  /* On a examine' toutes les regles specifiques de */
		  /* l'element, ajoute une nouvelle regle en fin de chaine */
		  GetReglePres (&pRegle);
		  new = (pRegle != NULL);
		  pR->PrNextPRule = pRegle;
	       }
	  }
     }
   if (new)
     {
	pRegle->PrType = PtImDescr;
	pRegle->PrNextPRule = NULL;
	pRegle->PrViewNum = 0;
	pRegle->PrSpecifAttr = 0;
	pRegle->PrSpecifAttrSSchema = NULL;
	pRegle->PrPresMode = PresImmediate;
	pRegle->PrImageDescr.xcf = 0;
	pRegle->PrImageDescr.ycf = 0;
	pRegle->PrImageDescr.wcf = 0;
	pRegle->PrImageDescr.hcf = 0;
	pRegle->PrImageDescr.imagePres = RealSize;
	pRegle->PrImageDescr.imageType = Bitmap_drvr;
     }
   return pRegle;

}				/*FindImageDescriptor */


/* ---------------------------------------------------------------------- */
/* |                                                                    | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                SetImageRule (PtrElement pEl, int x, int y, int w, int h, int typeimage, PictureScaling presimage)

#else  /* __STDC__ */
void                SetImageRule (pEl, x, y, w, h, typeimage, presimage)
PtrElement          pEl;
int                 x;
int                 y;
int                 w;
int                 h;
PictureScaling           presimage;
int                 typeimage;

#endif /* __STDC__ */

{
   PtrPRule        pRegle;

   pRegle = FindImageDescriptor (pEl);
   if (pRegle != NULL)
     {
	pRegle->PrImageDescr.xcf = x;
	pRegle->PrImageDescr.ycf = y;
	pRegle->PrImageDescr.wcf = w;
	pRegle->PrImageDescr.hcf = h;
	pRegle->PrImageDescr.imagePres = presimage;
	pRegle->PrImageDescr.imageType = typeimage;
     }

}				/*SetImageRule */


/* ---------------------------------------------------------------------- */
/* |    NewImageDescriptor cree un descripteur par element image.       | */
/* |            Si le pointeur sur le descripteur existe deja (champ    | */
/* |            ElImageDescriptor dans l'element), la procedure recopie | */
/* |            le champ ElImageDescriptor dans le pave.                | */
/* |            Si le pointeur sur le descripteur n'existe pas, la      | */
/* |            procedure commence par creer le descripteur.            | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                NewImageDescriptor (PtrAbstractBox ppav, char *filename, int imagetype)

#else  /* __STDC__ */
void                NewImageDescriptor (ppav, filename, imagetype)
PtrAbstractBox             ppav;
char               *filename;
int                 imagetype;

#endif /* __STDC__ */

{
   PtrTextBuffer      pBuffer;
   ImageDescriptor    *image;

   if (ppav->AbElement->ElTerminal && ppav->AbElement->ElLeafType == LtPicture)
     {
	/* C'est un element image -. accroche le descripteur a l'element */
	if (ppav->AbElement->ElImageDescriptor == NULL)
	  {
	     /* Creation du descripteur */
	     image = (ImageDescriptor *) TtaGetMemory (sizeof (ImageDescriptor));
	     if (filename == NULL)
	       {
		  GetBufTexte (&pBuffer);
		  ppav->AbElement->ElText = pBuffer;
		  filename = &pBuffer->BuContent[0];
	       }
	     image->imageFileName = filename;
	     image->imagePixmap = 0;
	     image->mask = 0;
	     image->imagePres = RealSize;
	     image->imageType = imagetype;
	     image->xcf = 0;
	     image->ycf = 0;
	     image->wcf = 0;
	     image->hcf = 0;
	     ppav->AbElement->ElImageDescriptor = (int *) image;
	  }
	ppav->AbImageDescriptor = ppav->AbElement->ElImageDescriptor;
     }
   else
     {
	/*  Ce n'est pas un element image -> Creation du descripteur */
	image = (ImageDescriptor *) TtaGetMemory (sizeof (ImageDescriptor));
	if (filename == NULL)
	  {
	     GetBufTexte (&pBuffer);
	     ppav->AbElement->ElText = pBuffer;
	     filename = &pBuffer->BuContent[0];
	  }
	image->imageFileName = filename;
	image->imagePixmap = 0;
	image->mask = 0;
	image->imagePres = RealSize;
	image->imageType = imagetype;
	image->xcf = 0;
	image->ycf = 0;
	image->wcf = 0;
	image->hcf = 0;
	ppav->AbImageDescriptor = (int *) image;
     }

}				/*NewImageDescriptor */


/* ---------------------------------------------------------------------- */
/* |    FreeImageDescriptor libere le descriteur d'image.               | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                FreeImageDescriptor (int *desc)

#else  /* __STDC__ */
void                FreeImageDescriptor (desc)
int                *desc;

#endif /* __STDC__ */

{
   ImageDescriptor    *image;

   if (desc != NULL)
     {
	image = (ImageDescriptor *) desc;
	FreeImage (image);
	TtaFreeMemory ((char *) image);
     }

}				/*FreeImageDescriptor */



/* ---------------------------------------------------------------------- */
/* |    FnCopy d'un ImageDescriptor                                      | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                UpdateImageDescriptor (int *Imdcopie, int *Imdsource)

#else  /* __STDC__ */
void                UpdateImageDescriptor (Imdcopie, Imdsource)
int                *Imdcopie;
int                *Imdsource;

#endif /* __STDC__ */

{
   ImageDescriptor    *imagec;
   ImageDescriptor    *images;

   imagec = (ImageDescriptor *) Imdcopie;
   images = (ImageDescriptor *) Imdsource;
   imagec->xcf = images->xcf;
   imagec->ycf = images->ycf;
   imagec->wcf = images->wcf;
   imagec->hcf = images->hcf;
   imagec->imagePres = images->imagePres;
   imagec->imageType = images->imageType;
}
