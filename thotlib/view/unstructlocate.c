/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2003
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 
/* 
 * locate what is designated in Concrete Image in unstructured mode.
 *
 * Author: I. Vatton (INRIA)
 *
 */

#include "ustring.h"
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
   GetClickedBox look for the abstract box that overlaps the point x,y
   or the nearest abstract box.
   The function checks all boxes in the tree and returns the best choice.
   Between a box and its child the function choses the child.
   The parameter ration fixes penalities of the vertical proximity.
  ----------------------------------------------------------------------*/
void   GetClickedBox (PtrBox *result, PtrAbstractBox pRootAb, int frame,
		      int x, int y, int ratio, int *pointselect)
{
   PtrAbstractBox      pAb;
   PtrBox              pSelBox, pBox;
   PtrBox              graphicBox;
   int                 dist;
   int                 pointIndex;
   ViewFrame          *pFrame;
   int                 d;

   pBox = NULL;
   pSelBox = NULL;
   /* dist gives the previous distance of the selected box
      MAX_DISTANCE when no box is selected */
   dist = MAX_DISTANCE;
   pFrame = &ViewFrameTable[frame - 1];

   if (pFrame->FrAbstractBox != NULL)
     pBox = pFrame->FrAbstractBox->AbBox;

   if (pBox != NULL)
     {
       pBox = pBox->BxNext;
       while (pBox)
	 {
#ifdef _GL
	   if (pBox->BxBoundinBoxComputed || 
	      pBox->BxType == BoBlock || pBox->BxNChars == 0)
	     {
#endif /* _GL */
	   pAb = pBox->BxAbstractBox;
	   if (pAb->AbVisibility >= pFrame->FrVisibility)
	     {
	       pointIndex = 0;
	       graphicBox = NULL;
	       if ((pAb->AbPresentationBox && !pAb->AbCanBeModified) ||
		   pAb->AbLeafType == LtGraphics ||
		   pAb->AbLeafType == LtPolyLine ||
		   pAb->AbLeafType == LtPath)
		 {
#ifdef _GL  
		   if (pBox->BxClipX <= x &&
		       pBox->BxClipX + pBox->BxClipW >= x &&
		       pBox->BxClipY <= y &&
		       pBox->BxClipY + pBox->BxClipH >= y)
#endif  /* _GL */
		     graphicBox = GetEnclosingClickedBox (pAb, x, x, y, frame,
							  &pointIndex);
		   if (graphicBox == NULL)
		     /* eliminate this box */
		     d = dist + 1;
		   else
		     d = 0;
		 }
	       else if (pAb->AbLeafType == LtSymbol && pAb->AbShape == 'r')
		 /* glitch for the root symbol */
		 d = GetShapeDistance (x, y, pBox, 1);
	       else if (pAb->AbLeafType == LtText ||
			pAb->AbLeafType == LtSymbol ||
			pAb->AbLeafType == LtPicture ||
			/* or an empty compound box */
			(pAb->AbLeafType == LtCompound && pAb->AbVolume == 0))
		 {
#ifndef _GL
		   if (pAb->AbLeafType == LtPicture)
		     {
		       /* check if the right side of the picture is selected */
		       d = pBox->BxXOrg + (pBox->BxWidth / 2);
		       if (x > d)
			 pointIndex = 1;
		       d = GetBoxDistance (x, y, ratio, pBox->BxXOrg, pBox->BxYOrg,
					   pBox->BxWidth, pBox->BxHeight);
		     }
		   else
		     d = GetBoxDistance (x, y, ratio, pBox->BxXOrg, pBox->BxYOrg,
					 pBox->BxWidth, pBox->BxHeight);
#else /*_GL */
		   if (pAb->AbLeafType == LtPicture)
		     {
		       /* check if the right side of the picture is selected */
		       d = pBox->BxClipX + (pBox->BxClipW / 2);
		       if (x > d)
			 pointIndex = 1;
		       d = GetBoxDistance (x, y, ratio, pBox->BxClipX, pBox->BxClipY,
					   pBox->BxClipW, pBox->BxClipH);
		     }
		   else
		     d = GetBoxDistance (x, y, ratio, pBox->BxClipX, pBox->BxClipY,
					 pBox->BxClipW, pBox->BxClipH);
#endif /*_GL */
		   if (d > dist && dist == MAX_DISTANCE)
		     /* it's the first box selected */
		     dist = d;
		 }
	       else
		 d = dist + 1;
	       
	       /* select the nearest box */
	       if (d < dist ||
		   (d == dist &&
		    (pSelBox == NULL ||
		     pSelBox->BxAbstractBox->AbDepth >= pBox->BxAbstractBox->AbDepth)))
		 {
		   dist = d;
		   pSelBox = pBox;
		   /* the selected reference point */
		   *pointselect = pointIndex;
		 }
	     }
#ifdef _GL
  }
#endif /* _GL */
	   pBox = pBox->BxNext;
	 }
       /* return the root box if there is no box selected */
       if (pSelBox == NULL)
	 pSelBox = pBox = pFrame->FrAbstractBox->AbBox;
     }
   *result = pSelBox;
}
