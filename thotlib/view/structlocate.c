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
 * locate what is designated in Concret Image in structured mode.
 *
 * Authors: I. Vatton (INRIA)
 *          S. Bonhomme (INRIA) - Separation between structured and
 *                                unstructured editing modes
 *
 */

#include "libmsg.h"
#include "thot_sys.h"

#include "constmedia.h"
#include "typemedia.h"
#include "message.h"
#include "appdialogue.h"

#define THOT_EXPORT extern
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


/*----------------------------------------------------------------------
  GetClickedStructBox recherche recursivement le pave qui englobe le 
  point designe' par x,y.                                 
  La fonction regarde toute l'arborescence des paves pour trouver le premier
  pave de plus petite profondeur couvrant le point designe.
  Si un pave et son fils repondent a la condition, c'est le pave fils qui
  l'emporte.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                GetClickedStructBox (PtrBox * result, PtrAbstractBox pRootAb, int frame, int x, int y, int *pointselect)
#else  /* __STDC__ */
void                GetClickedStructBox (result, pRootAb, frame, x, y, pointselect)
PtrBox             *result;
PtrAbstractBox      pRootAb;
int                 frame;
int                 x;
int                 y;
int                *pointselect;
#endif /* __STDC__ */
{
   PtrAbstractBox      pAb;
   PtrBox              pSelBox, pBox;
   PtrBox              pCurrentBox;
   int                 dist;
   int                 pointIndex;
   ViewFrame          *pFrame;

   /* On admet une erreur de precision de DELTA_SEL dans la designation */
   dist = x - DELTA_SEL;
   pFrame = &ViewFrameTable[frame - 1];
   pSelBox = NULL;
   pAb = pRootAb;
   while (pAb != NULL)
     {
       /* Est-ce le pave selectionne ? */
       if (pAb->AbVisibility >= pFrame->FrVisibility)
	 {
	   pBox = GetEnclosingClickedBox (pAb, dist, x, y, &pointIndex);
	   if (pBox != NULL)
	     /* Si c'est le premier pave trouve */
	     if (pSelBox == NULL)
	       {
		 pSelBox = pBox;
		 /* le point selectionne */
		 *pointselect = pointIndex;
	       }
	   /* Si le pave est sur un plan au dessus du precedent */
	   /* ou si le pave est un fils du precedent */
	     else if (pSelBox->BxAbstractBox->AbDepth > pAb->AbDepth
		      || (pSelBox->BxAbstractBox->AbLeafType == LtCompound && pBox->BxAbstractBox->AbLeafType != LtCompound)
		      || IsParentBox (pSelBox, pBox))
	       {
		 pSelBox = pBox;
		 *pointselect = pointIndex;
	       }
	     else
	       {
		 /* Verifie que le point designe est strictement dans la boite */
		 pBox = GetEnclosingClickedBox (pAb, x, x, y, &pointIndex);
		 pCurrentBox = GetEnclosingClickedBox (pSelBox->BxAbstractBox, x, x, y, &pointIndex);
		 if (pCurrentBox == NULL && pBox != NULL)
		   {
		     pSelBox = pBox;
		     /* le point selectionne */
		     *pointselect = pointIndex;
		   }
	       }
	 }
       /* On teste un autre pave de l'arbre */
       pAb = SearchNextAbsBox (pAb, NULL);
     }
   *result = pSelBox;
}
