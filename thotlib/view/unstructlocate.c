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
 * locate what is designated in Concret Image in unstructured mode.
 *
 * Author: I. Vatton (INRIA)
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


#define MAX_DISTANCE 2000

/*----------------------------------------------------------------------
   GetClickedBox recherche recursivement le pave qui englobe le point 
   designe' par x,y.                                       
   La fonction regarde toute l'arborescence des paves      
   pour trouver le premier pave de plus petite profondeur  
   couvrant le point designe.                              
   Si un pave et son fils repondent a la condition, c'est  
   le pave fils qui l'emporte.                             
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                GetClickedBox (PtrBox * result, PtrAbstractBox pRootAb, int frame, int x, int y, int *pointselect)
#else  /* __STDC__ */
void                GetClickedBox (result, pRootAb, frame, x, y, pointselect)
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
   int                 d;

   pBox = NULL;
   pSelBox = NULL;
   /* au-dela, on n'accepte pas la selection */
   dist = MAX_DISTANCE;
   pFrame = &ViewFrameTable[frame - 1];

   if (pFrame->FrAbstractBox != NULL)
      pBox = pFrame->FrAbstractBox->AbBox;

   if (pBox != NULL)
     {
	pBox = pBox->BxNext;
	while (pBox != NULL)
	  {
	     pAb = pBox->BxAbstractBox;
	     pointIndex = 0;
	     if (pAb->AbVisibility >= pFrame->FrVisibility)
	       {
		  if (pAb->AbPresentationBox || pAb->AbLeafType == LtGraphics || pAb->AbLeafType == LtPolyLine)
		    {
		       pCurrentBox = GetEnclosingClickedBox (pAb, x, x, y, &pointIndex);
		       if (pCurrentBox == NULL)
			  d = dist + 1;
		       else
			  d = 0;
		    }
		  else if (pAb->AbLeafType == LtSymbol && pAb->AbShape == 'r')
		     /* glitch pour le symbole racine */
		     d = GetShapeDistance (x, y, pBox, 1);
		  else if (pAb->AbLeafType == LtText
			   || pAb->AbLeafType == LtSymbol
			   || pAb->AbLeafType == LtPicture
		     /* ou une boite composee vide */
		   || (pAb->AbLeafType == LtCompound && pAb->AbVolume == 0))
		    {
		       if (pAb->AbLeafType == LtPicture)
			 {
			    /* detecte si on selectionne la droite de l'image */
			    d = pBox->BxXOrg + (pBox->BxWidth / 2);
			    if (x > d)
			       pointIndex = 1;
			    d = GetBoxDistance (x, y, pBox->BxXOrg, pBox->BxYOrg,
					     pBox->BxWidth, pBox->BxHeight);
			 }
		       else
			  d = GetBoxDistance (x, y, pBox->BxXOrg, pBox->BxYOrg,
					      pBox->BxWidth, pBox->BxHeight);
		       /* limit the distance to MAX_DISTANCE */
		       if (d > dist && dist == MAX_DISTANCE)
			 dist = d;
		    }
		  else
		     d = dist + 1;

		  /* Prend l'element le plus proche */
		  if (d < dist)
		    {
		       dist = d;
		       pSelBox = pBox;
		       /* le point selectionne */
		       *pointselect = pointIndex;
		    }
		  else if (d == dist)
		    {
		       /* Si c'est la premiere boite trouvee */
		       if (pSelBox == NULL)
			 {
			    dist = d;
			    pSelBox = pBox;
			    /* le point selectionne */
			    *pointselect = pointIndex;
			 }
		       /* Si la boite est sur un plan au dessus de la precedente */
		       else if (pSelBox->BxAbstractBox->AbDepth > pBox->BxAbstractBox->AbDepth)
			 {
			    dist = d;
			    pSelBox = pBox;
			    /* le point selectionne */
			    *pointselect = pointIndex;
			 }
		    }
	       }
	     pBox = pBox->BxNext;
	  }
	/* return the root box if there is no box selected */
	if (pSelBox == NULL)
	  pSelBox = pBox = pFrame->FrAbstractBox->AbBox;
     }
   *result = pSelBox;
}
