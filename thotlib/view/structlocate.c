
/* 
   Locate boxes in the Concrete Image
   I. Vatton
 */

#include "libmsg.h"
#include "thot_sys.h"
#include "functions.h"
#include "constmedia.h"
#include "typemedia.h"
#include "message.h"
#include "appdialogue.h"

#define EXPORT extern
#include "boxes_tv.h"
#include "frame_tv.h"
#include "platform_tv.h"
#include "appdialogue_tv.h"

#include "appli_f.h"
#include "structcreation_f.h"
#include "boxmoves_f.h"
#include "boxlocate_f.h"
#include "views_f.h"
#include "callback_f.h"
#include "font_f.h"
#include "geom_f.h"
#include "absboxes_f.h"
#include "buildboxes_f.h"
#include "buildlines_f.h"
#include "changepresent_f.h"
#include "boxselection_f.h"

#ifdef WWW_MSWINDOWS		/* map to MSVC library system calls */
#include <math.h>
#endif /* WWW_MSWINDOWS */


/* ---------------------------------------------------------------------- */
/* |    DesBoite recherche recursivement le pave qui englobe le point   | */
/* |            designe' par x,y.                                       | */
/* |            La fonction regarde toute l'arborescence des paves      | */
/* |            pour trouver le premier pave de plus petite profondeur  | */
/* |            couvrant le point designe.                              | */
/* |            Si un pave et son fils repondent a la condition, c'est  | */
/* |            le pave fils qui l'emporte.                             | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                StrDesBoite (PtrBox * result, PtrAbstractBox pAb, int frame, int x, int y, int *pointselect)
#else  /* __STDC__ */
void                StrDesBoite (result, pAb, frame, x, y, pointselect)
PtrBox           *result;
PtrAbstractBox             pAb;
int                 frame;
int                 x;
int                 y;
int                *pointselect;

#endif /* __STDC__ */
{
   PtrAbstractBox             pav;
   PtrBox            sbox, pBox;
   PtrBox            testbox;
   int                 distmax;
   int                 pointIndex;
   ViewFrame            *pFrame;

   /* On admet une erreur de precision de DELTA_SEL dans la designation */
   distmax = x - DELTA_SEL;
   pFrame = &ViewFrameTable[frame - 1];
   sbox = NULL;
   pav = pAb;
   while (pav != NULL)
     {
	/* Est-ce le pave selectionne ? */
	if (pav->AbVisibility >= pFrame->FrVisibility)
	  {
	     pBox = DansLaBoite (pav, distmax, x, y, &pointIndex);
	     if (pBox != NULL)
		/* Si c'est le premier pave trouve */
		if (sbox == NULL)
		  {
		     sbox = pBox;
		     *pointselect = pointIndex;	/* le point selectionne */
		  }
	     /* Si le pave est sur un plan au dessus du precedent */
	     /* ou si le pave est un fils du precedent */
		else if (sbox->BxAbstractBox->AbDepth > pav->AbDepth
			 || (sbox->BxAbstractBox->AbLeafType == LtCompound && pBox->BxAbstractBox->AbLeafType != LtCompound)
			 || IsParentBox (sbox, pBox))
		  {
		     sbox = pBox;
		     *pointselect = pointIndex;
		  }
		else
		  {
		     /* Verifie que le point designe est strictement dans la boite */
		     pBox = DansLaBoite (pav, x, x, y, &pointIndex);
		     testbox = DansLaBoite (sbox->BxAbstractBox, x, x, y, &pointIndex);
		     if (testbox == NULL && pBox != NULL)
		       {
			  sbox = pBox;
			  *pointselect = pointIndex;	/* le point selectionne */
		       }
		  }
	  }
	/* On teste un autre pave de l'arbre */
	pav = Pave_Suivant (pav);
     }
   *result = sbox;
}
