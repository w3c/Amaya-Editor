/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2004
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * gestion des Relations entre boites
 *
 * Author: I. Vatton (INRIA)
 *
 */
#include "thot_gui.h"
#include "thot_sys.h"
#include "libmsg.h"
#include "message.h"
#include "constmedia.h"
#include "typemedia.h"
#include "frame.h"

#ifdef _GL
#include "glwindowdisplay.h"
#include "content.h"
#endif /* _GL */

#define THOT_EXPORT extern
#include "boxes_tv.h"
#ifdef _GL
#include "frame_tv.h"
#endif /* _GL */


#include "absboxes_f.h"
#include "appli_f.h"
#include "boxmoves_f.h"
#include "boxrelations_f.h"
#include "buildboxes_f.h"
#include "exceptions_f.h"
#include "font_f.h"
#include "memory_f.h"

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
static void ClearBoxMoved (PtrBox pBox)
{
  PtrBox           pNextBox;
  while (pBox != NULL)
    {
      pNextBox = pBox->BxMoved;
      pBox->BxMoved = NULL;
      pBox = pNextBox;
    }
}
 
/*----------------------------------------------------------------------
   GetPosRelativeAb retourne le pointeur sur le pave de reference    
   pour le positionnement implicite, horizontal/vertical,  
   du pave pCurrentAb.                                     
   Si la valeur rendue est NULL, le pave se positionne par 
   rapport au pave englobant.                              
  ----------------------------------------------------------------------*/
static PtrAbstractBox GetPosRelativeAb (PtrAbstractBox pCurrentAb,
					ThotBool horizRef)
{
   ThotBool            still;
   PtrAbstractBox      pAb;

   still = TRUE;
   pAb = pCurrentAb->AbPrevious;
   while (still)
      if (pAb == NULL)
	 still = FALSE;
      else if (pAb->AbDead)
	 pAb = pAb->AbPrevious;
      else if (horizRef)
	 if (pAb->AbHorizPos.PosAbRef == NULL)
	    still = FALSE;
	 else
	    pAb = pAb->AbPrevious;
      else if (pAb->AbVertPos.PosAbRef == NULL)
	 still = FALSE;
      else
	 pAb = pAb->AbPrevious;

   return pAb;
}


/*----------------------------------------------------------------------
   InsertPosRelation etablit les liens de dependance entre les deux  
   repers des boites :                                     
   - double sens dans le cas du positionnement entre deux  
   soeurs.                                                 
   - sens unique dans le cas du positionnement entre fille 
   et mere.                                                
   - sens repere vers axe pour la definition des axes de   
   reference (un deplacement du repere de pTargetBox       
   modifie l'axe de reference de pOrginBox).               
   - sens inverse dans le cas d'une dimension elastique    
   (un deplacement du repere de pTargetBox modifie la      
   dimension de pOrginBox).                                
  ----------------------------------------------------------------------*/
static void InsertPosRelation (PtrBox pOrginBox, PtrBox pTargetBox,
			       OpRelation op, BoxEdge originEdge,
			       BoxEdge targetEdge)
{
  int                 i;
  ThotBool            loop;
  ThotBool            empty;
  PtrPosRelations     pPreviousPosRel, pNext;
  PtrPosRelations     pPosRel;
  BoxRelation        *pRelation;

  i = 0;
  /* add a relation from origin to target */
  if (op != OpHorizRef && op != OpVertRef && op != OpWidth && op != OpHeight)
    {
      /* look for an empty entry */
      pPosRel = pOrginBox->BxPosRelations;
      pPreviousPosRel = NULL;
      loop = TRUE;
      while (loop && pPosRel)
	{
	  i = 0;
	  pPreviousPosRel = pPosRel;
	  do
	    {
	      empty = (pPosRel->PosRTable[i].ReBox == NULL);
	      i++;
	    }
	  while (i != MAX_RELAT_POS && !empty);
	  
	  if (empty)
	    {
	      loop = FALSE;
	      i--;
	    }
	  else
	    /* next block */
	    pPosRel = pPosRel->PosRNext;
	}

      if (loop)
	{
	  /* add a new block */
	  pNext = pPosRel;
	  GetPosBlock (&pPosRel);
	  pPosRel->PosRNext = pNext;
	  if (pPreviousPosRel == NULL)
	    pOrginBox->BxPosRelations = pPosRel;
	  else
	    pPreviousPosRel->PosRNext = pPosRel;
	  i = 0;
	}
      pRelation = &pPosRel->PosRTable[i];
      pRelation->ReRefEdge = originEdge;
      pRelation->ReBox = pTargetBox;
      pRelation->ReOp = op;
    }

  /* add a relation from target to origin */
  if (op != OpHorizInc && op != OpVertInc)
    {
      /* look for an empty entry */
      pPosRel = pTargetBox->BxPosRelations;
      pPreviousPosRel = NULL;
      loop = TRUE;
      while (loop && pPosRel)
	{
	  i = 0;
	  pPreviousPosRel = pPosRel;
	  do
	    {
	      empty = (pPosRel->PosRTable[i].ReBox == NULL);
	      i++;
	    }
	  while (i != MAX_RELAT_POS && !empty);
	  
	  if (empty)
	    {
	      loop = FALSE;
	      i--;
	    }
	  else
	    /* next block */
	    pPosRel = pPosRel->PosRNext;
	}

      if (loop)
	{
	  /* add a new block */
	  pNext = pPosRel;
	  GetPosBlock (&pPosRel);
	  pPosRel->PosRNext = pNext;
	  if (pPreviousPosRel == NULL)
	    pTargetBox->BxPosRelations = pPosRel;
	  else
	    pPreviousPosRel->PosRNext = pPosRel;
	  i = 0;
	}
      pRelation = &pPosRel->PosRTable[i];
      pRelation->ReRefEdge = targetEdge;
      pRelation->ReBox = pOrginBox;
      pRelation->ReOp = op;
    }
}


/*----------------------------------------------------------------------
  InsertDimRelation registers vertical or horizontal links between two
  boxes (from pOrginBox to pTargetBox).
  The parameter sameDimension is FALSE when the height depends on a width
  or vise versa.
  The parameter inLine is TRUE when the pOrginBox box is displayed within
  a block of lines.
  ----------------------------------------------------------------------*/
static void InsertDimRelation (PtrBox pOrginBox, PtrBox pTargetBox,
			       ThotBool sameDimension, ThotBool horizRef,
			       ThotBool inLine)
{
  PtrDimRelations     pPreviousDimRel, pNext;
  PtrDimRelations     pDimRel;
  int                 i;
  ThotBool            loop;
  ThotBool            empty;

  if (!sameDimension)
    horizRef = !horizRef;
  if (inLine && IsParentBox (pTargetBox, pOrginBox))
    /* dont register the relation in this case */
    return;
  i = 0;
  /* add a relation from origin to target */
  if (horizRef)
    {
      if (sameDimension)
	pDimRel = pOrginBox->BxWidthRelations;
      else
	pDimRel = pOrginBox->BxHeightRelations;
    }
  else if (sameDimension)
    pDimRel = pOrginBox->BxHeightRelations;
  else
    pDimRel = pOrginBox->BxWidthRelations;

  /* look for an empty entry */
  pPreviousDimRel = NULL;
  loop = TRUE;
  while (loop && pDimRel)
    {
      i = 0;
      pPreviousDimRel = pDimRel;
      do
	{
	  empty = (pDimRel->DimRTable[i] == NULL);
	  i++;
	}
      while (i != MAX_RELAT_DIM && !empty);

      if (empty)
	{
	  loop = FALSE;
	  i--;
	}
      else
	/* next block */
	pDimRel = pDimRel->DimRNext;
    }

  /* Need a new block? */
  if (loop)
    {
      /* add a new block */
      pNext = pDimRel;
      GetDimBlock (&pDimRel);
      pDimRel->DimRNext = pNext;
      if (pPreviousDimRel == NULL)
	{
	  if (horizRef)
	    pOrginBox->BxWidthRelations = pDimRel;
	  else
	    pOrginBox->BxHeightRelations = pDimRel;
	}
      else
	pPreviousDimRel->DimRNext = pDimRel;
      i = 0;
     }

   pDimRel->DimRTable[i] = pTargetBox;
   pDimRel->DimRSame[i] = sameDimension;
}


/*----------------------------------------------------------------------
   NextAbToCheck recherche le prochain pave du pave pAb en         
   sautant le pave de reference pRefAb. Le parcours de     
   l'arborescence se fait de bas en haut et de gauche a`   
   droite.                                                 
  ----------------------------------------------------------------------*/
static PtrAbstractBox NextAbToCheck (PtrAbstractBox pAb, PtrAbstractBox pRefAb)
{
   PtrAbstractBox      pNextAb;

   /* Il y a un premier fils different du pave reference ? */
   if (pAb->AbFirstEnclosed != NULL && pAb->AbFirstEnclosed != pRefAb)
      pNextAb = pAb->AbFirstEnclosed;
   /* Il y a un frere different du pave reference ? */
   else if (pAb->AbNext != NULL && pAb->AbNext != pRefAb)
      pNextAb = pAb->AbNext;
   /* Sinon on remonte dans la hierarchie */
   else
     {
	pNextAb = NULL;
	while (pAb != NULL)
	  {
	     /* On saute le pave reference */
	     if (pAb->AbFirstEnclosed == pRefAb && pRefAb->AbNext != NULL)
		pAb = pRefAb;
	     else if (pAb->AbNext == pRefAb)
		pAb = pRefAb;

	     /* On recherche un prochain frere d'un pere */
	     if (pAb->AbNext != NULL)
	       {
		  pNextAb = pAb->AbNext;
		  pAb = NULL;
	       }
	     else
	       {
		  /* On passe au pere tant que ce n'est pas la racine */
		  pAb = pAb->AbEnclosing;
		  if (pAb->AbFirstEnclosed == pRefAb)
		     pAb = pAb->AbEnclosing;
		  if (pAb != NULL)
		     if (pAb->AbEnclosing == NULL)
			pAb = NULL;
	       }
	  }
     }
   return pNextAb;
}


/*----------------------------------------------------------------------
   PropagateXOutOfStruct propage l'indicateur hors-structure.        
  ----------------------------------------------------------------------*/
static void PropagateXOutOfStruct (PtrAbstractBox pCurrentAb,
				   ThotBool status, ThotBool enclosed)
{
   PtrAbstractBox      pAb;
   PtrBox              pBox;

   /* Recherche un pave frere qui depend ce celui-ci */
   if (pCurrentAb->AbEnclosing == NULL)
      return;
   else
      pAb = pCurrentAb->AbEnclosing->AbFirstEnclosed;

   while (pAb != NULL)
     {
	pBox = pAb->AbBox;
	if (pAb == pCurrentAb || pBox == NULL || IsDead (pAb))
	   ;			/* inutile de traiter ce pave */
	else if (pBox->BxXOutOfStruct == status)
	   ;			/* inutile de traiter ce pave */
	else if (pAb->AbHorizPos.PosAbRef == pCurrentAb && !pAb->AbHorizPosChange)
	  {
	     /* Dependance de position */
	     pBox->BxXOutOfStruct = status;
	     /* Propagate aussi le non englobement */
	     if (pAb->AbEnclosing == pCurrentAb->AbEnclosing)
		/* une boite soeur positionnee par rapport a une boite */
		/* elastique non englobee n'est pas englobee elle-meme */
		pAb->AbHorizEnclosing = enclosed;
	     PropagateXOutOfStruct (pAb, status, pAb->AbHorizEnclosing);
	  }
	else if (pAb->AbWidth.DimIsPosition &&
		 pAb->AbWidth.DimPosition.PosAbRef == pCurrentAb &&
		 !pAb->AbWidthChange)
	   /* Dependance de dimension */
	   pBox->BxWOutOfStruct = status;

	/* passe au pave suivant */
	pAb = pAb->AbNext;
     }
}


/*----------------------------------------------------------------------
   PropagateYOutOfStruct propage l'indicateur hors-structure.         
  ----------------------------------------------------------------------*/
static void PropagateYOutOfStruct (PtrAbstractBox pCurrentAb,
				   ThotBool status, ThotBool enclosed)
{
   PtrAbstractBox      pAb;
   PtrBox              pBox;

   /* Recherche un pave frere qui depend ce celui-ci */
   if (pCurrentAb->AbEnclosing == NULL)
      return;
   else
      pAb = pCurrentAb->AbEnclosing->AbFirstEnclosed;

   while (pAb != NULL)
     {
	pBox = pAb->AbBox;
	if (pAb == pCurrentAb || pBox == NULL || IsDead (pAb))
	   ;			/* inutile de traiter ce pave */
	else if (pBox->BxYOutOfStruct == status)
	   ;			/* inutile de traiter ce pave */
	else if (pAb->AbVertPos.PosAbRef == pCurrentAb && !pAb->AbVertPosChange)
	  {
	     pBox->BxYOutOfStruct = status;
	     /* Propagate aussi le non englobement */
	     if (pAb->AbEnclosing == pCurrentAb->AbEnclosing)
		/* une boite soeur positionnee par rapport a une boite */
		/* elastique non englobee n'est pas englobee elle-meme */
		pAb->AbVertEnclosing = enclosed;
	     PropagateYOutOfStruct (pAb, status, pAb->AbVertEnclosing);
	  }
	else if (pAb->AbHeight.DimIsPosition
		 && pAb->AbHeight.DimPosition.PosAbRef == pCurrentAb && !pAb->AbHeightChange)
	   pBox->BxHOutOfStruct = status;

	/* passe au pave suivant */
	pAb = pAb->AbNext;
     }
}


/*----------------------------------------------------------------------
  SetPositionConstraint memorizes the constrainted edge of the box and
  updates the position value according to the current edge used.
  The initial value of val depends on the refered box.
  ----------------------------------------------------------------------*/
void SetPositionConstraint (BoxEdge localEdge, PtrBox pBox, int *val)
{
  AbPosition         *pPosAb;

   switch (localEdge)
     {
     case Left:
       pBox->BxHorizEdge = Left;
       break;
     case Right:
       *val -= pBox->BxWidth;
       pBox->BxHorizEdge = Right;
       break;
     case VertMiddle:
       *val -= pBox->BxWidth / 2;
       pBox->BxHorizEdge = VertMiddle;
       break;
     case VertRef:
       *val -= pBox->BxVertRef;
       pPosAb = &pBox->BxAbstractBox->AbVertRef;
       if (pPosAb->PosAbRef == pBox->BxAbstractBox)
	 {
	   if (pPosAb->PosRefEdge == VertMiddle)
	     pBox->BxHorizEdge = VertMiddle;
	   else if (pPosAb->PosRefEdge == Right)
	     pBox->BxHorizEdge = Right;
	   else
	     pBox->BxHorizEdge = Left;
	 }
       else
	 pBox->BxHorizEdge = VertRef;
       break;
     case Top:
       pBox->BxVertEdge = Top;
       break;
     case Bottom:
       *val -= pBox->BxHeight;
       pBox->BxVertEdge = Bottom;
       break;
     case HorizMiddle:
       *val -= pBox->BxHeight / 2;
       pBox->BxVertEdge = HorizMiddle;
       break;
     case HorizRef:
       *val -= pBox->BxHorizRef;
       pPosAb = &pBox->BxAbstractBox->AbHorizRef;
       if (pPosAb->PosAbRef == pBox->BxAbstractBox)
	 {
	   if (pPosAb->PosRefEdge == HorizMiddle)
	     pBox->BxVertEdge = HorizMiddle;
	   else if (pPosAb->PosRefEdge == Bottom)
	     pBox->BxVertEdge = Bottom;
	   else
	     pBox->BxVertEdge = Top;
	 }
       else
	 pBox->BxVertEdge = HorizRef;
       break;
     default:
       break;
     }
}

/*----------------------------------------------------------------------
  ComputeRadius updates the horizontal or vartical radius of a rectangle
  with rounded corners.
  ----------------------------------------------------------------------*/
void     ComputeRadius (PtrAbstractBox pAb, int frame, ThotBool horizRef)
{
  PtrBox              pBox;

  pBox = pAb->AbBox;
  if (horizRef)
    {
      if (pAb->AbRxUnit == UnPercent)
	pBox->BxRx = PixelValue (pAb->AbRx, UnPercent, (PtrAbstractBox) (pBox->BxW), 0);
      else
	pBox->BxRx = PixelValue (pAb->AbRx, pAb->AbRxUnit, pAb,
				 ViewFrameTable[frame - 1].FrMagnification);
    }
  else
    {
      if (pAb->AbRyUnit == UnPercent)
	pBox->BxRy = PixelValue (pAb->AbRy, UnPercent, (PtrAbstractBox) (pBox->BxH), 0);
      else
	pBox->BxRy = PixelValue (pAb->AbRy, pAb->AbRyUnit, pAb,
				 ViewFrameTable[frame - 1].FrMagnification);
    }
}


/*----------------------------------------------------------------------
  GetGhostSize returns the width (horizontal)  or the height of the ghost
  The parameter pBlock points to the block element.
  ----------------------------------------------------------------------*/
static int GetGhostSize (PtrBox pBox, ThotBool horizontal, PtrBox pBlock)
{
  PtrAbstractBox  pAb, pChild, pNext, pParent;
  int             dim;

  if (pBox == NULL || pBlock == NULL)
    return 0;
  else if (pBox->BxType == BoGhost || pBox->BxType == BoFloatGhost)
    {
      pAb = pBox->BxAbstractBox;
      pParent = pAb->AbEnclosing;
      if ((horizontal &&
	   ((pAb->AbWidth.DimValue < 0 &&
	    pAb->AbWidth.DimAbRef == NULL) ||
	   (pAb->AbWidth.DimUnit == UnAuto && pBlock->BxType == BoBlock))) ||
	  (!horizontal &&
	   ((pAb->AbHeight.DimValue < 0 &&
	     pAb->AbHeight.DimAbRef == NULL) ||
	    pAb->AbHeight.DimUnit == UnAuto)))
	{
	  /* depend on the contents */
	  pChild = pAb->AbFirstEnclosed;
	  dim = 0;
	  while (pChild)
	    {
	      /* skip presentation boxes */
	      pNext = pChild->AbNext;
	      while (pNext && pNext->AbPresentationBox)
		pNext = pNext->AbNext;
	      if (pChild->AbBox && !pChild->AbPresentationBox)
		{
		  if ((pChild->AbBox->BxType == BoGhost ||
		      pChild->AbBox->BxType == BoFloatGhost) &&
		      /* skip enclosed boxes that inherit from the parent */ 
		      ((horizontal &&
			((pChild->AbWidth.DimValue < 0 &&
			 pChild->AbWidth.DimAbRef  == NULL) ||
			(pChild->AbWidth.DimUnit == UnAuto &&
			 pBlock->BxType == BoBlock))) ||
		       (!horizontal &&
			((pChild->AbHeight.DimValue < 0 &&
			  pChild->AbHeight.DimAbRef == NULL) ||
			 pChild->AbHeight.DimUnit == UnAuto))))
		    dim += GetGhostSize (pChild->AbBox, horizontal, pBlock);
		  else if (!pChild->AbPresentationBox)
		    {
		      if (horizontal)
			dim += pChild->AbBox->BxWidth;
		      else
			dim += pChild->AbBox->BxHeight;
		    }
		  if (!pChild->AbPresentationBox &&
		      ((horizontal && pParent->AbBox->BxType == BoFloatBlock) ||
		       (!horizontal && pParent->AbBox->BxType == BoBlock)))
		    /* the first child gives the size */
		    return dim;
		}
	      pChild = pNext;
	    }
	}
      else
	{
	  /* fixed width or inherited from the enclosing box */
	  pBox = pParent->AbBox;
	  if (pBox &&
	      (pBox->BxType == BoGhost || pBox->BxType == BoFloatGhost))
	    dim = GetGhostSize (pBox, horizontal, pBlock);
	  else if (horizontal)
	    dim = pBox->BxW;
	  else
	    dim = pBox->BxH;
	  if (horizontal && pBox->BxAbstractBox->AbWidth.DimUnit == UnPercent)
	    dim = dim * pBox->BxAbstractBox->AbWidth.DimValue / 100;
	}
      return dim;
    }
  else if (horizontal)
    return pBox->BxW;
  else
    return pBox->BxH;
}


/*----------------------------------------------------------------------
  CleanAutoMargins cleans up all auto margins.
  ----------------------------------------------------------------------*/
void CleanAutoMargins (PtrAbstractBox pAb)
{
  PtrBox              box;

  if (pAb && pAb->AbBox)
    {
      box = pAb->AbBox;
      if (pAb->AbLeftMarginUnit == UnAuto)
	{
	  box->BxWidth -= box->BxLMargin;
	  box->BxLMargin = 0;
	}
      if (pAb->AbRightMarginUnit == UnAuto)
	{
	  box->BxWidth -= box->BxRMargin;
	  box->BxRMargin = 0;
	}
    }
}

/*----------------------------------------------------------------------
  ComputeMBP applies margin, padding, and border rules.
  Relation between values:
  <-LMargin-><-LBorder-><-LPadding-><-W-><-RPadding-><-RBorder-><-LRargin->
  <---------------------------------Width--------------------------------->
  ----------------------------------------------------------------------*/
void ComputeMBP (PtrAbstractBox pAb, int frame, ThotBool horizRef,
		 ThotBool evalAuto)
{
  PtrBox              pBox;
  PtrBox              pParent, pBlock;
  int                 dim, x1, x2, y1, y2;

  x1 = x2 = y1 = y2 = -1;
  if (pAb->AbEnclosing && pAb->AbEnclosing->AbBox)
    pParent = pAb->AbEnclosing->AbBox;
  else
    pParent = NULL;

  pBox = pAb->AbBox;
  if (pBox->BxType == BoGhost || pBox->BxType == BoFloatGhost)
    {
      /* get the block element */
      pBlock = pParent;
      while (pBlock &&
	     (pBlock->BxType == BoGhost || pBlock->BxType == BoFloatGhost))
	{
	  if (pBlock->BxAbstractBox && pBlock->BxAbstractBox->AbEnclosing)
	    pBlock = pBlock->BxAbstractBox->AbEnclosing->AbBox;
	  else
	    pBlock = NULL;
	}
    }
  else
    pBlock = NULL;

  if (horizRef)
    {
      /* reference for percent rules */
      if ((pBox->BxType == BoGhost || pBox->BxType == BoFloatGhost) &&
	  (pAb->AbLeftMarginUnit == UnPercent ||
	   pAb->AbRightMarginUnit == UnPercent ||
	   pAb->AbLeftPaddingUnit == UnPercent ||
	   pAb->AbRightPaddingUnit == UnPercent ||
	   pAb->AbLeftBorderUnit == UnPercent ||
	   pAb->AbRightBorderUnit == UnPercent))
	dim = GetGhostSize (pBox, horizRef, pBlock);
      else
	dim = pBox->BxW;
      /* left margin */
      if (pAb->AbLeftMarginUnit == UnPercent)
	pBox->BxLMargin = PixelValue (pAb->AbLeftMargin, UnPercent,
				      (PtrAbstractBox) dim, 0);
      else if (pAb->AbLeftMarginUnit != UnAuto)
	pBox->BxLMargin = PixelValue (pAb->AbLeftMargin, pAb->AbLeftMarginUnit,
				      pAb, ViewFrameTable[frame - 1].FrMagnification);
      /* right margin */
      if (pAb->AbRightMarginUnit == UnPercent)
	pBox->BxRMargin = PixelValue (pAb->AbRightMargin, UnPercent,
				      (PtrAbstractBox) dim, 0);
      else if (pAb->AbRightMarginUnit != UnAuto)
	pBox->BxRMargin = PixelValue (pAb->AbRightMargin, pAb->AbRightMarginUnit,
				      pAb, ViewFrameTable[frame - 1].FrMagnification);

      /* left padding */
      if (pAb->AbLeftPaddingUnit == UnPercent)
	pBox->BxLPadding = PixelValue (pAb->AbLeftPadding, UnPercent,
				       (PtrAbstractBox) dim, 0);
      else
	pBox->BxLPadding = PixelValue (pAb->AbLeftPadding, pAb->AbLeftPaddingUnit,
				       pAb, ViewFrameTable[frame - 1].FrMagnification);
      /* right padding */
      if (pAb->AbRightPaddingUnit == UnPercent)
	pBox->BxRPadding = PixelValue (pAb->AbRightPadding, UnPercent,
				       (PtrAbstractBox) dim, 0);
      else
	pBox->BxRPadding = PixelValue (pAb->AbRightPadding, pAb->AbRightPaddingUnit,
				       pAb, ViewFrameTable[frame - 1].FrMagnification);

      /* left border */
      if (pAb->AbLeftBorderUnit == UnPercent)
	pBox->BxLBorder = PixelValue (pAb->AbLeftBorder, UnPercent,
				      (PtrAbstractBox) dim, 0);
      else
	pBox->BxLBorder = PixelValue (pAb->AbLeftBorder, pAb->AbLeftBorderUnit,
				      pAb, ViewFrameTable[frame - 1].FrMagnification);
      /* right border */
      if (pAb->AbRightBorderUnit == UnPercent)
	pBox->BxRBorder = PixelValue (pAb->AbRightBorder, UnPercent,
				      (PtrAbstractBox) dim, 0);
      else
	pBox->BxRBorder = PixelValue (pAb->AbRightBorder, pAb->AbRightBorderUnit,
				      pAb, ViewFrameTable[frame - 1].FrMagnification);

      /* Manage auto margins */
      if (pAb->AbLeftMarginUnit == UnAuto || pAb->AbRightMarginUnit == UnAuto)
	{
	  if (pBox->BxType != BoCell &&
	      pBox->BxType != BoRow &&
	      pParent && evalAuto &&
	      pParent->BxType != BoBlock &&
	      pParent->BxType != BoFloatBlock &&
	      pParent->BxType != BoGhost &&
	      pParent->BxType != BoFloatGhost &&
	      pParent->BxW >= dim)
	    {
	      if ( !pAb->AbWidth.DimIsPosition &&
		   pAb->AbWidth.DimAbRef == pAb->AbEnclosing &&
		   pAb->AbWidth.DimValue == 0)
		/* the box has the size of its parent */
		dim = 0;
	      else
	        dim = - dim + pParent->BxW - pBox->BxLPadding - pBox->BxRPadding
		      - pBox->BxLBorder - pBox->BxRBorder;
	      if (pAb->AbLeftMarginUnit == UnAuto && pAb->AbRightMarginUnit == UnAuto)
		{
		  pBox->BxLMargin = dim / 2;
		  pBox->BxRMargin = pBox->BxLMargin;
		}
	      else if (pAb->AbLeftMarginUnit == UnAuto)
		pBox->BxLMargin = dim - pBox->BxRMargin;
	      else if (pAb->AbRightMarginUnit == UnAuto)
		pBox->BxRMargin = dim - pBox->BxLMargin;
	    }
	  else
	    {
	      if (pAb->AbLeftMarginUnit == UnAuto)
		pBox->BxLMargin = 0;
	      if (pAb->AbRightMarginUnit == UnAuto)
		pBox->BxRMargin = 0;
	    }
	}
      /* List-item presentation boxes take into account the left margin */
      if (pAb->AbPrevious && pAb->AbPrevious->AbBox &&
	  pAb->AbPrevious->AbPresentationBox &&
	  pAb->AbPrevious->AbTypeNum == 0)
	{
	  if (pAb->AbDirection == 'L')
	    pAb->AbPrevious->AbBox->BxXOrg = pBox->BxXOrg + pBox->BxLMargin
	                             - pAb->AbPrevious->AbBox->BxWidth;
	  else
	    pAb->AbPrevious->AbBox->BxXOrg = pBox->BxXOrg + pBox->BxWidth
	                  - pBox->BxRMargin;
	}
    }
  else
    {
      /* reference for percent rules */
      if ((pBox->BxType == BoGhost || pBox->BxType == BoFloatGhost) &&
	  (pAb->AbTopMarginUnit == UnPercent ||
	   pAb->AbBottomMarginUnit == UnPercent ||
	   pAb->AbTopPaddingUnit == UnPercent ||
	   pAb->AbBottomPaddingUnit == UnPercent ||
	   pAb->AbTopBorderUnit == UnPercent ||
	   pAb->AbBottomBorderUnit == UnPercent))
	dim = GetGhostSize (pBox, horizRef, pBlock);
      else
	dim = pBox->BxH;
      /* top margin */
      if (pAb->AbTopMarginUnit == UnPercent)
	pBox->BxTMargin = PixelValue (pAb->AbTopMargin, UnPercent,
				      (PtrAbstractBox) dim, 0);
      else if (pAb->AbTopMarginUnit != UnAuto)
	pBox->BxTMargin = PixelValue (pAb->AbTopMargin, pAb->AbTopMarginUnit,
				      pAb, ViewFrameTable[frame - 1].FrMagnification);

      /* bottom margin */
      if (pAb->AbBottomMarginUnit == UnPercent)
	pBox->BxBMargin = PixelValue (pAb->AbBottomMargin, UnPercent,
				      (PtrAbstractBox) dim, 0);
      else if (pAb->AbBottomMarginUnit != UnAuto)
	pBox->BxBMargin = PixelValue (pAb->AbBottomMargin, pAb->AbBottomMarginUnit,
				      pAb, ViewFrameTable[frame - 1].FrMagnification);

      /* top padding */
      if (pAb->AbTopPaddingUnit == UnPercent)
	pBox->BxTPadding = PixelValue (pAb->AbTopPadding, UnPercent,
				       (PtrAbstractBox) dim, 0);
      else
	pBox->BxTPadding = PixelValue (pAb->AbTopPadding, pAb->AbTopPaddingUnit,
				       pAb, ViewFrameTable[frame - 1].FrMagnification);
      /* bottom padding */
      if (pAb->AbBottomPaddingUnit == UnPercent)
	pBox->BxBPadding = PixelValue (pAb->AbBottomPadding, UnPercent,
				       (PtrAbstractBox) dim, 0);
      else
	pBox->BxBPadding = PixelValue (pAb->AbBottomPadding, pAb->AbBottomPaddingUnit,
				       pAb, ViewFrameTable[frame - 1].FrMagnification);

      /* top border */
      if (pAb->AbTopBorderUnit == UnPercent)
	pBox->BxTBorder = PixelValue (pAb->AbTopBorder, UnPercent,
				      (PtrAbstractBox) dim, 0);
      else
	pBox->BxTBorder = PixelValue (pAb->AbTopBorder, pAb->AbTopBorderUnit, pAb, ViewFrameTable[frame - 1].FrMagnification);
      /* bottom border */
      if (pAb->AbBottomBorderUnit == UnPercent)
	pBox->BxBBorder = PixelValue (pAb->AbBottomBorder, UnPercent,
				      (PtrAbstractBox) dim, 0);
      else
	pBox->BxBBorder = PixelValue (pAb->AbBottomBorder, pAb->AbBottomBorderUnit,
				      pAb, ViewFrameTable[frame - 1].FrMagnification);
     }
}

/*----------------------------------------------------------------------
  ComputePosRelation applies the vertical/horizontal positionning rule
  according to the parameter horizRef for the box pBox. 
  The box origin BxXOrg or BxYOrg is updated and dependencies between 
  boxes are registered.  
  Relation between values:
  ^
  BxXOrg
  <-LMargin-><-LBorder-><-LPadding-><-W-><-RPadding-><-RBorder-><-LRargin->
  <---------------------------------Width--------------------------------->
  ----------------------------------------------------------------------*/
void ComputePosRelation (AbPosition rule, PtrBox pBox, int frame, ThotBool horizRef)
{
  PtrAbstractBox      pRefAb;
  PtrAbstractBox      pAb, child;
  PtrBox              pRefBox, box;
  BoxEdge             refEdge, localEdge;
  OpRelation          op;
  int                 x, y, dist, dim, d;
  int                 t, l, b, r;
  ThotBool            sibling = FALSE;
  
  /* On calcule la position de reference */
  op = (OpRelation) 0;
  refEdge = (BoxEdge) 0;
  GetSizesFrame (frame, &x, &y);
  pRefBox = NULL;
  pRefAb = rule.PosAbRef;
  pAb = pBox->BxAbstractBox;
  if (pRefAb && IsDead (pRefAb))
    {
      fprintf (stderr, "Position refers a dead box");
      pRefAb = NULL;
    }
  if (pAb->AbFloat != 'N' &&
      (pAb->AbLeafType == LtPicture ||
       (pAb->AbLeafType == LtCompound &&
	!pAb->AbWidth.DimIsPosition &&
	pAb->AbWidth.DimAbRef == NULL)))
    {
      /* the box position is computed by the line formatter */
      pAb->AbVertPosChange = FALSE;
      pAb->AbHorizPosChange = FALSE;
      return;
    }
  else if (pRefAb && pRefAb->AbBox &&
	   (pRefAb->AbBox->BxType == BoGhost ||
	    pRefAb->AbBox->BxType == BoFloatGhost))
    {
      /* the box position is computed by the line formatter */
      if (horizRef)
	pAb->AbHorizPosChange = FALSE;
      else
	pAb->AbVertPosChange = FALSE;
      return;
    }

  if (horizRef)
    {
      /* Horizontal rule */
      if (pRefAb == pAb)
	{
	  /* could not depend on itself */
	  pAb->AbHorizPos.PosAbRef = NULL;
	  if (pAb->AbWidth.DimIsPosition)
	    {
	      pAb->AbWidth.DimIsPosition = FALSE;
	      pAb->AbWidth.DimUserSpecified = FALSE;
	    }
	  /* don't process this rule */
	  pRefAb = NULL;
	}
      else if (pRefAb && pRefAb == pAb->AbPrevious &&
	       pRefAb->AbHorizPos.PosAbRef == pAb)
	{
	  /* Bad rule: change the ref to the previous box */
	  while (pRefAb &&
		 pRefAb->AbHorizPos.PosAbRef == pAb)
	    pRefAb = pRefAb->AbPrevious;
	  pAb->AbHorizPos.PosAbRef = pRefAb;
	}
      
      if (pRefAb == NULL)
	{
	  /* default rule */
	  if (pAb->AbEnclosing == NULL)
	    {
	      /* Root box */
	      refEdge = rule.PosRefEdge;
	      localEdge = rule.PosEdge;
	      /* Convert the distance value */
	      if (rule.PosUnit == UnPercent)
		dist = PixelValue (rule.PosDistance, UnPercent, (PtrAbstractBox) x, 0);
	      else
		dist = PixelValue (rule.PosDistance, rule.PosUnit, pAb,
				   ViewFrameTable[frame - 1].FrMagnification);
	      if (rule.PosDeltaUnit == UnPercent)
		dist += PixelValue (rule.PosDistDelta, UnPercent, (PtrAbstractBox) x, 0);
	      else
		dist += PixelValue (rule.PosDistDelta, rule.PosDeltaUnit, pAb,
				    ViewFrameTable[frame - 1].FrMagnification);
	    }
	  else
	    {
	      /* there is an enclosing box */
	      pRefAb = GetPosRelativeAb (pAb, horizRef);
	      /* Si oui -> A droite de sa boite */
	      if (pRefAb != NULL && rule.PosUnit != UnPercent)
		{
		  /* At the right of the previous */
		  sibling = TRUE;
		  pRefBox = pRefAb->AbBox;
		  dist = 0;
		  refEdge = Right;
		  localEdge = Left;
		  op = OpHorizDep;
		}
	      else
		{
		  /* depend on the enclosing box */
		  pRefAb = pAb->AbEnclosing;
		  pRefBox = pRefAb->AbBox;
		  if (rule.PosUnit == UnPercent)
		    /* poucentage de la largeur de l'englobant */
		    dist = PixelValue (rule.PosDistance, UnPercent,
				       (PtrAbstractBox) pAb->AbEnclosing, 0);
		  else
		    dist = 0;
		  if (rule.PosDeltaUnit == UnPercent)
		    /* poucentage de la largeur de l'englobant */
		    dist += PixelValue (rule.PosDistDelta, UnPercent,
					(PtrAbstractBox) pAb->AbEnclosing, 0);
		  refEdge = Left;
		  localEdge = Left;
		  if (pBox->BxHorizFlex)
		    op = OpHorizDep;
		  else
		    op = OpHorizInc;
		}
	    }
	}
      else
	{
	  /* explicite rule */
	  while (pRefAb && pRefAb->AbBox &&
		 (pRefAb->AbBox->BxType == BoGhost ||
		  pRefAb->AbBox->BxType == BoFloatGhost))
	    pRefAb = pRefAb->AbEnclosing;
	  if (pAb->AbEnclosing == pRefAb && !pBox->BxHorizFlex)
	    /* it's not a stretchable box and it depends on its enclosing */
	    op = OpHorizInc;
	  else
	    {
	      op = OpHorizDep;
	      /* new position */
	      pBox->BxXToCompute = FALSE;
	      if (pRefAb->AbEnclosing != pAb->AbEnclosing)
		{
		  /* it's a relation out of structure */
		  if (!IsXPosComplete (pBox))
		    pBox->BxXToCompute = TRUE;
		  pBox->BxXOutOfStruct = TRUE;
		  PropagateXOutOfStruct (pAb, TRUE, pAb->AbHorizEnclosing);
		}
	      else if (pRefAb->AbBox)
		{
		  /* depend on a sibling box */
		  sibling = TRUE;
		  if (pRefAb->AbBox->BxXOutOfStruct
		      || (pRefAb->AbBox->BxHorizFlex)
		      || (pRefAb->AbBox->BxWOutOfStruct && refEdge != Left))
		    {
		      /* The box inherits out from a structure position */
		      /* or depends on a out of structure dimension */
		      if (!IsXPosComplete (pBox))
			pBox->BxXToCompute = TRUE;
		      pBox->BxXOutOfStruct = TRUE;
		      PropagateXOutOfStruct (pAb, TRUE, pAb->AbHorizEnclosing);
		    }
		}
	    }

	  refEdge = rule.PosRefEdge;
	  localEdge = rule.PosEdge;
	  /* Convert the distance value */
	  dim = pAb->AbEnclosing->AbBox->BxW;
	  if (rule.PosUnit == UnPercent)
	    {
	      dist = PixelValue (rule.PosDistance, UnPercent,
				 (PtrAbstractBox) dim, 0);
	      /* Change the rule for further updates */
	      pAb->AbHorizPos.PosDistance = dist;
	      pAb->AbHorizPos.PosUnit = UnPixel;
	    }
	  else
	    dist = PixelValue (rule.PosDistance, rule.PosUnit, pAb,
			       ViewFrameTable[frame - 1].FrMagnification);
	  if (rule.PosDeltaUnit == UnPercent)
	    {
	      d = PixelValue (rule.PosDistDelta, UnPercent,
			      (PtrAbstractBox) dim, 0);
	      /* Change the rule for further updates */
	      pAb->AbHorizPos.PosDistDelta = d;
	      pAb->AbHorizPos.PosUnit = UnPixel;
	    }
	  else
	    d = PixelValue (rule.PosDistDelta, rule.PosDeltaUnit, pAb,
			    ViewFrameTable[frame - 1].FrMagnification);
	  dist += d;
	}
    }
  else
    {
      /* Vertical rule */
      if (pRefAb == pAb)
	{
	  /* could not depend on itself */
	  pAb->AbVertPos.PosAbRef = NULL;
	  if (pAb->AbHeight.DimIsPosition)
	    {
	      pAb->AbHeight.DimIsPosition = FALSE;
	      pAb->AbHeight.DimUserSpecified = FALSE;
	    }
	  /* don't process this rule */
	  pRefAb = NULL;
	}
      else if (pRefAb && pRefAb == pAb->AbPrevious &&
	       pRefAb->AbVertPos.PosAbRef == pAb)
	{
	  /* Bad rule: change the ref to the previous box */
	  while (pRefAb &&
		 pRefAb->AbVertPos.PosAbRef == pAb)
	    pRefAb = pRefAb->AbPrevious;
	  pAb->AbVertPos.PosAbRef = pRefAb;
	}

      if (pRefAb == NULL)
	{
	  /* default rule */
	  if (pAb->AbEnclosing == NULL)
	    {
	      /* Root box */
	      refEdge = rule.PosRefEdge;
	      localEdge = rule.PosEdge;
	      /* Convert the distance value */
	      if (rule.PosUnit == UnPercent)
		dist = PixelValue (rule.PosDistance, UnPercent, (PtrAbstractBox) y, 0);
	      else
		dist = PixelValue (rule.PosDistance, rule.PosUnit, pAb,
				   ViewFrameTable[frame - 1].FrMagnification);
	      if (rule.PosDeltaUnit == UnPercent)
		dist += PixelValue (rule.PosDistDelta, UnPercent, (PtrAbstractBox) y, 0);
	      else
		dist += PixelValue (rule.PosDistDelta, rule.PosDeltaUnit, pAb,
				   ViewFrameTable[frame - 1].FrMagnification);
	    }
	  else
	    {
	      /* there is an enclosing box */
	      pRefAb = GetPosRelativeAb (pAb, horizRef);
	      if (pRefAb &&
		  pRefAb->AbVisibility >= ViewFrameTable[frame - 1].FrVisibility)
		{
		  /* Align baselines */
		  sibling = TRUE;
		  pRefBox = pRefAb->AbBox;
		  dist = 0;
		  refEdge = HorizRef;
		  localEdge = HorizRef;
		  op = OpVertDep;
		}
	      else
		{
		  /* depend on the enclosing box */
		  pRefAb = pAb->AbEnclosing;
		  pRefBox = pRefAb->AbBox;
		  dist = 0;
		  refEdge = Top;
		  localEdge = Top;
		  if (pBox->BxVertFlex)
		    op = OpVertDep;
		  else
		    op = OpVertInc;
		}
	    }
	}
      else
	{
	  /* explicite rule */
	  if (pAb->AbEnclosing == pRefAb &&
	      pBox->BxVertFlex && pBox->BxType == BoCell)
	    {
	      if (!pBox->BxVertInverted &&
		  (rule.PosRefEdge != Top || rule.PosEdge != Top))
		{
		  /* a specific patch for vertical extended cells */
		  pAb->AbVertPos.PosRefEdge = Top;
		  pAb->AbVertPos.PosEdge = Top;
		  rule.PosRefEdge = Top;
		  rule.PosEdge = Top;
		}
	      else if (pBox->BxVertInverted &&
		       (rule.PosRefEdge != Top || rule.PosEdge != Bottom))
		{
		  /* a specific patch for vertical extended cells */
		  pAb->AbVertPos.PosRefEdge = Bottom;
		  pAb->AbVertPos.PosEdge = Bottom;
		  pAb->AbVertPos.PosDistance = 0;
		  pAb->AbVertPos.PosDistDelta = 0;
		  rule.PosRefEdge = Top;
		  rule.PosEdge = Bottom;
		  rule.PosDistance = 0;
		  rule.PosDistDelta = 0;
		}
	    }

	  if (pAb->AbEnclosing == pRefAb && !pBox->BxVertFlex)
	    /* it's not a stretchable box and it depends on its enclosing */
	    op = OpVertInc;
	  else
	    {
	      op = OpVertDep;
	      /* new position */
	      pBox->BxYToCompute = FALSE;
	      if (pRefAb->AbEnclosing != pAb->AbEnclosing)
		{
		  /* it's a relation out of structure */
		  if (!IsYPosComplete (pBox))
		    pBox->BxYToCompute = TRUE;
		  pBox->BxYOutOfStruct = TRUE;
		  PropagateYOutOfStruct (pAb, TRUE, pAb->AbVertEnclosing);
		}
	      else if (pRefAb->AbBox)
		{
		  /* depend on a sibling box */
		  sibling = TRUE;
		  if (pRefAb->AbBox->BxYOutOfStruct
		      || (pRefAb->AbBox->BxHorizFlex && pRefAb->AbLeafType == LtCompound &&
			  pRefAb->AbInLine && refEdge != Top)
		      || (pRefAb->AbBox->BxHOutOfStruct && refEdge != Top))
		    {
		      /* The box inherits out from a structure position */
		      /* or depends on a out of structure dimension */
		      if (!IsYPosComplete (pBox))
			pBox->BxYToCompute = TRUE;
		      pBox->BxYOutOfStruct = TRUE;
		      PropagateYOutOfStruct (pAb, TRUE, pAb->AbVertEnclosing);
		    }
		}	
	    }
	  
	  refEdge = rule.PosRefEdge;
	  localEdge = rule.PosEdge;
	  /* Convert the distance value */
	  dim = pAb->AbEnclosing->AbBox->BxH;
	  if (rule.PosUnit == UnPercent)
	    {
	      dist = PixelValue (rule.PosDistance, UnPercent, (PtrAbstractBox) dim, 0);
	      /* Change the rule for further updates */
	      pAb->AbVertPos.PosDistance = dist;
	      pAb->AbVertPos.PosUnit = UnPixel;
	    }
	  else
	    dist = PixelValue (rule.PosDistance, rule.PosUnit, pAb,
			       ViewFrameTable[frame - 1].FrMagnification);
	  if (rule.PosDeltaUnit == UnPercent)
	    {
	      d = PixelValue (rule.PosDistDelta, UnPercent, (PtrAbstractBox) dim, 0);
	      /* Change the rule for further updates */
	      pAb->AbVertPos.PosDistDelta = d;
	      pAb->AbVertPos.PosDeltaUnit = UnPixel;
	    }
	  else
	    d = PixelValue (rule.PosDistDelta, rule.PosDeltaUnit, pAb,
			       ViewFrameTable[frame - 1].FrMagnification);
	  dist += d;
	}
    }

  if (pRefAb)
    {
      /* Depend on another box */
      pRefBox = pRefAb->AbBox;
      if (pRefBox == NULL)
	{
	  /* the referred box doesn't already exist */
	  pRefBox = GetBox (pRefAb);
	  if (pRefBox)
	    pRefAb->AbBox = pRefBox;
	  else
	    /* memory allocation trouble */
	    return;
	}
      if (pRefBox->BxType == BoSplit && pRefBox->BxNexChild)
	{
	  box = pRefBox->BxNexChild;
	  x = box->BxXOrg;
	  y = box->BxYOrg;
	}
      else if (pRefBox->BxType == BoGhost || pRefBox->BxType == BoFloatGhost)
	{
	  child = pRefAb;
	  while ((child->AbBox->BxType == BoGhost ||
		  child->AbBox->BxType == BoFloatGhost) &&
		 child->AbFirstEnclosed &&
		 child->AbFirstEnclosed->AbBox)
	    child = child->AbFirstEnclosed;
	  box = child->AbBox;
	  x = box->BxXOrg;
	  y = box->BxYOrg;
	}
      else
	{
#ifdef _GL
	  if (pRefAb &&
	      pRefAb->AbElement &&
	      pRefAb->AbElement->ElSystemOrigin && 
	      pRefAb->AbNext != pAb &&
	      FrameTable[frame].FrView == 1)
	    {	      
	      x = 0;
	      y = 0;
	    }
	  else
	    {
	      /*GetSystemOrigins (pRefBox->BxAbstractBox, &sgx, &sgy);*/
	      x = pRefBox->BxXOrg/* - sgx*/;
	      y = pRefBox->BxYOrg/* - sgy*/;
	      /*GetSystemOrigins (pBox->BxAbstractBox, &sgx, &sgy);
	      x += sgx;
	      y += sgy;*/
	    }
#else /* _GL */
	  x = pRefBox->BxXOrg;
	  y = pRefBox->BxYOrg;
#endif /* _GL */
	  /* List-item presentation boxes take into account the left margin */
	  if (pAb->AbPresentationBox && pAb->AbTypeNum == 0)
	    {
	      if (pRefAb->AbDirection == 'L')
		  x += pRefBox->BxLMargin;
	      else
		  x -= pRefBox->BxRMargin;
	    }
	}

      if (pRefAb == pAb->AbEnclosing)
	{
	  GetExtraMargins (pRefBox, NULL, &t, &b, &l, &r);
	  t += pRefBox->BxTMargin + pRefBox->BxTBorder + pRefBox->BxTPadding;
	  l += pRefBox->BxLMargin + pRefBox->BxLBorder + pRefBox->BxLPadding;
	  b += pRefBox->BxBMargin + pRefBox->BxBBorder + pRefBox->BxBPadding;
	  r += pRefBox->BxRMargin + pRefBox->BxRBorder + pRefBox->BxRPadding;
	}
      else
	{
	  t = 0;
	  l = 0;
	  b = 0;
	  r = 0;
	}
      switch (refEdge)
	{
	case Top:
	  if (t)
	    {
	      if (pRefBox->BxTBorder + pRefBox->BxTPadding > 0)
		y += t;
	      else if (t > pBox->BxTMargin)
		/* collapsing margins */
		y = y + t - pBox->BxTMargin;
	    }
	  break;
	case Left:
	  x += l;
	  if (sibling && localEdge == Right)
	    {
	      /* only take the larger margin into account */
	      if (pBox->BxRMargin < pRefBox->BxLMargin)
		x -= pBox->BxRMargin;
	      else
		x -= pRefBox->BxLMargin;
	    }
	  break;
	case Bottom:
	  y = y + pRefBox->BxHeight - b;
	  if (sibling && localEdge == Top)
	    {
	      /* normally b = 0 */
	      /* only take the larger margin into account */
	      if (pBox->BxTMargin > 0 && pRefBox->BxBMargin > 0)
		{
		  if (pBox->BxTMargin < pRefBox->BxBMargin)
		    y = y - pBox->BxTMargin;
		  else
		    y = y - pRefBox->BxBMargin;
		}
	      else if (pBox->BxTMargin < 0 && pRefBox->BxBMargin < 0)
		{
		  if (pBox->BxTMargin < pRefBox->BxBMargin)
		    y = y - pRefBox->BxBMargin;
		  else
		    y = y - pBox->BxTMargin;
		}
	    }
	  break;
	case Right:
	  x = x + pRefBox->BxWidth - r;
	  if (sibling && localEdge == Left)
	    {
	      /* only take the larger margin into account */
	      if (pBox->BxLMargin < pRefBox->BxRMargin)
		x -= pBox->BxLMargin;
	      else
		x -= pRefBox->BxRMargin;
	    }
	  break;
	case HorizRef:
	  y = y + pRefBox->BxHorizRef;
	  break;
	case VertRef:
	  x = x + pRefBox->BxVertRef;
	  break;
	case HorizMiddle:
	  y = y + (pRefBox->BxHeight / 2);
	  break;
	case VertMiddle:
	  x = x + (pRefBox->BxWidth / 2);
	  break;
	default:
	  break;
	}
    }
  else
    {
      switch (refEdge)
	{
	case VertRef:
	case VertMiddle:
	  x = x / 2;
	  break;
	case HorizRef:
	case HorizMiddle:
	  y = y / 2;
	  break;
	case Right:
	case Bottom:
	  break;
	default:
	  x = 0;
	  y = 0;
	  break;
	}
    }
 
  /* Calcule l'origine de la boite et les points fixes */
  if (horizRef)
    {
      /* default value */
      pBox->BxHorizEdge = Left;
      SetPositionConstraint (localEdge, pBox, &x);
    }
  else
    {
      /* default value */
      pBox->BxVertEdge = Top;
      if (pRefAb && pRefAb->AbVisibility >= ViewFrameTable[frame - 1].FrVisibility)
	SetPositionConstraint (localEdge, pBox, &y);
    }

   /* invisible boxes take no space */
   if (pBox->BxAbstractBox->AbVisibility < ViewFrameTable[frame - 1].FrVisibility)
     dist = 0;
   else if (pRefAb &&
	    pRefAb->AbVisibility < ViewFrameTable[frame - 1].FrVisibility)
     dist = 0;
   
   /* Met a jour l'origine de la boite suivant la relation indiquee */
   if (horizRef)
     {
       if (!pBox->BxHorizFlex)
	 {
	   x = x + dist - pBox->BxXOrg;
	   ClearBoxMoved (pBox);
	   if (x == 0 && pBox->BxXToCompute)
	     /* Force le placement des boites filles */
	     XMoveAllEnclosed (pBox, x, frame);
	   else
	     XMove (pBox, NULL, x, frame);
	 }
       /* la regle de position est interpretee */
       pAb->AbHorizPosChange = FALSE;
     }
   else
     {
       if (!pBox->BxVertFlex)
	 {
	   y = y + dist - pBox->BxYOrg;
	   ClearBoxMoved (pBox);
	   if (y == 0 && pBox->BxYToCompute)
	     /* Force le placement des boites filles */
	     YMoveAllEnclosed (pBox, y, frame);
	   else
	     YMove (pBox, NULL, y, frame);
	 }
       /* la regle de position est interpretee */
       pAb->AbVertPosChange = FALSE;
     }

   /* Il faut mettre a jour les dependances des boites */
   if (pRefAb != NULL && pRefBox != NULL)
     {
       InsertPosRelation (pBox, pRefBox, op, localEdge, refEdge);
       ClearBoxMoved (pRefBox);
       
       if (horizRef && pBox->BxHorizFlex)
	 MoveBoxEdge (pBox, pRefBox, op, x + dist - pBox->BxXOrg, frame, TRUE);
       else if (!horizRef && pBox->BxVertFlex)
	 MoveBoxEdge (pBox, pRefBox, op, y + dist - pBox->BxYOrg, frame, FALSE);
       ClearBoxMoved (pRefBox);
     }
   /* break down the temporary link of moved boxes */
   ClearBoxMoved (pBox);
}



/*----------------------------------------------------------------------
   GetHPosRelativeBox search the box which horizontally links   
   pBox to is enclosing:                                
   - If relations were not updated, that box is BxHorizInc
   - If the box has the relation OpHorizInc, it's itself
   - Else get the sibling box which gives the position.     
   Return a box or NULL.
  ----------------------------------------------------------------------*/
PtrBox GetHPosRelativeBox (PtrBox pBox, PtrBox pPreviousBox)
{
  PtrBox              pRelativeBox;
  PtrPosRelations     pPosRel;
  BoxRelation        *pRelation;
  int                 i;
  ThotBool            notEmpty;

  /* On verifie que la boite n'a pas deja ete examinee */
  if (pPreviousBox != NULL)
    {
      pRelativeBox = pPreviousBox->BxMoved;
      while (pRelativeBox != NULL && pRelativeBox != pBox)
	pRelativeBox = pRelativeBox->BxMoved;
    }
  else
    pRelativeBox = NULL;

  if (pRelativeBox == NULL)
    {
      /* On met a jour la pile des boites traitees */
      pBox->BxMoved = pPreviousBox;
      /* regarde si on connait deja la boite qui la relie a l'englobante */
      if (pBox->BxHorizInc != NULL)
	pRelativeBox = pBox->BxHorizInc;
      else
	pRelativeBox = NULL;

      /* Si la position de la boite depend d'une boite externe on prend la */
      /* boite elle meme comme reference.                                  */
      if (pBox->BxXOutOfStruct)
	pRelativeBox = pBox;

      /* regarde si la boite est reliee a son englobante */
      pPosRel = pBox->BxPosRelations;
      while (pRelativeBox == NULL && pPosRel != NULL)
	{
	  i = 0;
	  notEmpty = (pPosRel->PosRTable[i].ReBox != NULL);
	  while (i < MAX_RELAT_POS && notEmpty)
	    if (pPosRel->PosRTable[i].ReOp == OpHorizInc)
	      {
		/* On a trouve */
		pRelativeBox = pBox;
		i = MAX_RELAT_POS;
	      }
	    else
	      {
		i++;
		if (i < MAX_RELAT_POS)
		  notEmpty = pPosRel->PosRTable[i].ReBox != NULL;
	      }
	  
	  /* Bloc suivant */
	  pPosRel = pPosRel->PosRNext;
	}

      /* Sinon on recherche la boite soeur qui l'est */
      pPosRel = pBox->BxPosRelations;
      while (pRelativeBox == NULL && pPosRel != NULL)
	{
	  i = 0;
	  notEmpty = pPosRel->PosRTable[i].ReBox != NULL;
	  while (i < MAX_RELAT_POS && notEmpty)
	    {
	      pRelation = &pPosRel->PosRTable[i];
	      if (pRelation->ReBox->BxAbstractBox != NULL &&
		  pRelation->ReOp == OpHorizDep &&
		  /* Si c'est la bonne relation de dependance */
		  pRelation->ReBox->BxAbstractBox->AbHorizPos.PosAbRef != pBox->BxAbstractBox)
		{
		  /* la position de la boite depend d'une boite elastique */
		  /* -> on prend la boite elastique comme reference       */
		  if (pRelation->ReBox->BxHorizFlex)
		    pRelativeBox = pRelation->ReBox;
		  else
		    pRelativeBox = GetHPosRelativeBox (pRelation->ReBox, pBox);
		  
		  /* Est-ce que l'on a trouve la boite qui donne la position ? */
		  if (pRelativeBox != NULL)
		    {
		      i = MAX_RELAT_POS;
		      /* La position depend d'une relation hors-structure ? */
		      if (pRelativeBox->BxXOutOfStruct)
			pBox->BxXOutOfStruct = TRUE;
		    }
		  else
		    {
		      i++;
		      if (i < MAX_RELAT_POS)
			notEmpty = pPosRel->PosRTable[i].ReBox != NULL;
		    }
		}
	      else
		{
		  i++;
		  if (i < MAX_RELAT_POS)
		    notEmpty = pPosRel->PosRTable[i].ReBox != NULL;
		}
	    }
	  /* Bloc suivant */
	  pPosRel = pPosRel->PosRNext;
	}

      pBox->BxHorizInc = pRelativeBox;
      return pRelativeBox;
    }
  else
    return NULL;
}


/*----------------------------------------------------------------------
   GetVPosRelativeBox search the box which vertically links 
   pBox to is enclosing:                                
   - If relations were not updated, that box is BxVertInc
   - If the box has the relation OpVertInc, it's itself
   - Else get the sibling box which gives the position.     
   Return a box or NULL.                        
  ----------------------------------------------------------------------*/
PtrBox GetVPosRelativeBox (PtrBox pBox, PtrBox pPreviousBox)
{
  PtrBox              pRelativeBox;
  PtrPosRelations     pPosRel;
  BoxRelation        *pRelation;
  int                 i;
  ThotBool            notEmpty;

  /* On verifie que la boite n'a pas deja ete examinee */
  if (pPreviousBox != NULL)
    {
      pRelativeBox = pPreviousBox->BxMoved;
      while (pRelativeBox != NULL && pRelativeBox != pBox)
	pRelativeBox = pRelativeBox->BxMoved;
    }
  else
    pRelativeBox = NULL;

  if (pRelativeBox == NULL)
    {
      
      /* On met a jour la pile des boites traitees */
      pBox->BxMoved = pPreviousBox;
      
      /* regarde si on connait deja la boite qui la relie a l'englobante */
      if (pBox->BxVertInc != NULL)
	pRelativeBox = pBox->BxVertInc;
      else
	pRelativeBox = NULL;
      
      /* Si la position de la boite depend d'une boite externe on prend la */
      /* boite elle meme comme reference.                                  */
      if (pBox->BxYOutOfStruct)
	pRelativeBox = pBox;
      
      /* regarde si la boite est reliee a son englobante */
      pPosRel = pBox->BxPosRelations;
      while (pRelativeBox == NULL && pPosRel != NULL)
	{
	  i = 0;
	  notEmpty = (pPosRel->PosRTable[i].ReBox != NULL);
	  while (i < MAX_RELAT_POS && notEmpty)
	    if (pPosRel->PosRTable[i].ReOp == OpVertInc)
	      {
		/* On a trouve */
		pRelativeBox = pBox;
		i = MAX_RELAT_POS;
	      }
	    else
	      {
		i++;
		if (i < MAX_RELAT_POS)
		  notEmpty = pPosRel->PosRTable[i].ReBox != NULL;
	      }
	  /* Bloc suivant */
	  pPosRel = pPosRel->PosRNext;
	}

      /* Sinon on recherche la boite soeur qui l'est */
      pPosRel = pBox->BxPosRelations;
      while (pRelativeBox == NULL && pPosRel != NULL)
	{
	  i = 0;
	  notEmpty = pPosRel->PosRTable[i].ReBox != NULL;
	  while (i < MAX_RELAT_POS && notEmpty)
	    {
	      pRelation = &pPosRel->PosRTable[i];
	      if (pRelation->ReBox->BxAbstractBox != NULL &&
		  pRelation->ReOp == OpVertDep &&
		  /* Si c'est la bonne relation de dependance */
		  pRelation->ReBox->BxAbstractBox->AbVertPos.PosAbRef != pBox->BxAbstractBox)
		{
		  /* Si la position de la boite depend d'une boite elastique */
		  /* on prend la boite elastique comme reference             */
		  if (pRelation->ReBox->BxVertFlex)
		    pRelativeBox = pRelation->ReBox;
		  else
		    pRelativeBox = GetVPosRelativeBox (pRelation->ReBox, pBox);
		  /* Est-ce que l'on a trouve la boite qui donne la position ? */
		  if (pRelativeBox != NULL)
		    {
		      i = MAX_RELAT_POS;
		      /* La position depend d'une relation hors-structure ? */
		      if (pRelativeBox->BxYOutOfStruct)
			pBox->BxYOutOfStruct = TRUE;
		    }
		  else
		    {
		      i++;
		      if (i < MAX_RELAT_POS)
			notEmpty = pPosRel->PosRTable[i].ReBox != NULL;
		    }
		}
	      else
		{
		  i++;
		  if (i < MAX_RELAT_POS)
		    notEmpty = pPosRel->PosRTable[i].ReBox != NULL;
		}
	    }
	  /* Bloc suivant */
	  pPosRel = pPosRel->PosRNext;
	}

      pBox->BxVertInc = pRelativeBox;
      return pRelativeBox;
    }
  else
    return (NULL);
}


/*----------------------------------------------------------------------
  ComputeDimRelation applies the vertical/horizontal sizing rule of pAb
  according to the parameter horizRef. 
  Return TRUE when the dimension depends on the contents.
  There are 4 different cases:
  - if the dim depends on the parent the rule concerns the outside
    dim and is related to the inside dim of the parent.
  - if it's the default dim the rule concerns the inside dim and is 
    related to the outside dim of children.
  - if the dim depends on another box the rule concerns the outside
    dim and is related to the outside dim of that box.
  - if the rule gives an absolute dim the rule concerns the inside
    dim of the box.
  The box size BxW or BxH is updated and dependencies between boxes are
  registered.

  Relation between values:
  ^
  BxXOrg
  <-LMargin-><-LBorder-><-LPadding-><-W-><-RPadding-><-RBorder-><-LRargin->
  <---------------------------------Width--------------------------------->
  ----------------------------------------------------------------------*/
ThotBool  ComputeDimRelation (PtrAbstractBox pAb, int frame, ThotBool horizRef)
{
  PtrBox              pRefBox;
  PtrBox              pBox;
  PtrAbstractBox      pParentAb, pColumn;
  PtrAbstractBox      pChildAb;
  PtrElement          pEl;
  PtrLine             pLine;
  OpRelation          op;
  AbDimension        *pDimAb;
  AbPosition         *pPosAb;
  int                 val, delta, i;
  int                 dx, dy, dim;
  int                 t, b, l, r;
  ThotBool            inLine;
  ThotBool            defaultDim;

  pBox = pAb->AbBox;
  /* Check the box visibility */
  if (pAb->AbVisibility >= ViewFrameTable[frame - 1].FrVisibility)
    {
      pParentAb = pAb->AbEnclosing;
      /* Check validity of rules */
      if (horizRef && pAb->AbWidth.DimIsPosition)
	{
	  if (pAb->AbHorizPos.PosEdge == pAb->AbWidth.DimPosition.PosEdge
	      || pAb->AbHorizPos.PosEdge == VertMiddle
	      || pAb->AbHorizPos.PosEdge == VertRef)
	    {
	      /* invalid pos rule */
	      if (pAb->AbWidth.DimPosition.PosEdge == Left)
		pAb->AbHorizPos.PosEdge = Right;
	      else if (pAb->AbWidth.DimPosition.PosEdge == Right)
		pAb->AbHorizPos.PosEdge = Left;
	    }
	  else if (pAb->AbHorizPos.PosAbRef == NULL
		   && (pAb->AbWidth.DimPosition.PosAbRef == NULL ||
		       pAb->AbWidth.DimPosition.PosAbRef == pParentAb))
	    {
	      /* inherit from the enclosing box */
	      pAb->AbHorizPos.PosAbRef = pParentAb;
	      pAb->AbWidth.DimIsPosition = FALSE;
	      pAb->AbWidth.DimAbRef = pParentAb;
	      pAb->AbWidth.DimValue = 0;
	      pAb->AbWidth.DimSameDimension = TRUE;
	      pAb->AbWidth.DimUserSpecified = FALSE;
	    }
	  else if (pAb->AbHorizPos.PosAbRef == NULL
		   || pAb->AbWidth.DimPosition.PosEdge == VertRef
		   || pAb->AbWidth.DimPosition.PosAbRef == NULL
		   || pAb->AbWidth.DimPosition.PosAbRef == pAb)
	    {
	      /* invalid Width rule */
	      fprintf (stderr, "Bad Width rule on %s\n", AbsBoxType (pAb, TRUE));
	      pAb->AbWidth.DimIsPosition = FALSE;
	      pAb->AbWidth.DimAbRef = NULL;
	      pAb->AbWidth.DimValue = 20;	/* largeur fixe */
	      pAb->AbWidth.DimUnit = UnPoint;
	      pAb->AbWidth.DimUserSpecified = FALSE;
	    }
	  /* check if the relative box is not already dead */
	  else if (IsDead (pAb->AbHorizPos.PosAbRef))
	    {
	      fprintf (stderr, "Dimension refers a dead box");
	      pAb->AbWidth.DimIsPosition = FALSE;
	      pAb->AbWidth.DimAbRef = NULL;
	      pAb->AbWidth.DimValue = 20;	/* fixed width */
	      pAb->AbWidth.DimUnit = UnPoint;
	      pAb->AbWidth.DimUserSpecified = FALSE;
	    }
	}
      else if (!horizRef && pAb->AbHeight.DimIsPosition)
	{
	  if (pAb->AbVertPos.PosEdge == pAb->AbHeight.DimPosition.PosEdge
	      || pAb->AbVertPos.PosEdge == HorizMiddle
	      || pAb->AbVertPos.PosEdge == HorizRef)
	    {
	      /* invalid pos rule */
	      if (pAb->AbHeight.DimPosition.PosEdge == Top)
		pAb->AbVertPos.PosEdge = Bottom;
	      else if (pAb->AbHeight.DimPosition.PosEdge == Bottom)
		pAb->AbVertPos.PosEdge = Top;
	    }
	  else if (pAb->AbVertPos.PosAbRef == NULL
		   && (pAb->AbHeight.DimPosition.PosAbRef == NULL ||
		       pAb->AbHeight.DimPosition.PosAbRef == pParentAb))
	    {
	      /* inherit from the enclosing box */
	      pAb->AbVertPos.PosAbRef = pParentAb;
	      pAb->AbHeight.DimIsPosition = FALSE;
	      pAb->AbHeight.DimAbRef = pParentAb;
	      pAb->AbHeight.DimValue = 0;
	      pAb->AbHeight.DimSameDimension = TRUE;
	      pAb->AbHeight.DimUserSpecified = FALSE;
	    }
	  else if (pAb->AbVertPos.PosAbRef == NULL
		   || pAb->AbHeight.DimPosition.PosEdge == HorizRef
		   || pAb->AbHeight.DimPosition.PosAbRef == NULL
		   || pAb->AbHeight.DimPosition.PosAbRef == pAb)
	    {
	      /* invalid Height rule */
	      fprintf (stderr, "Bad Height rule on %s\n", AbsBoxType (pAb, TRUE));
	      pAb->AbHeight.DimIsPosition = FALSE;
	      pAb->AbHeight.DimAbRef = NULL;
	      pAb->AbHeight.DimValue = 20;	/* fixed height */
	      pAb->AbHeight.DimUnit = UnPoint;
	      pAb->AbHeight.DimUserSpecified = FALSE;
	    }
	  /* check if the relative box is not already dead */
	  else if (IsDead (pAb->AbVertPos.PosAbRef))
	    {
	      fprintf (stderr, "Dimension refers a dead box");
	      pAb->AbHeight.DimIsPosition = FALSE;
	      pAb->AbHeight.DimAbRef = NULL;
	      pAb->AbHeight.DimValue = 0;
	      pAb->AbHeight.DimUnit = UnPoint;
	      pAb->AbHeight.DimUserSpecified = FALSE;
	    }
	}

      if ((horizRef && !pAb->AbWidth.DimIsPosition) ||
	  (!horizRef && !pAb->AbHeight.DimIsPosition))
	{
	  /* It's not a stretchable box */
	  if (horizRef)
	    {
	      pDimAb = &pAb->AbWidth;
	      pBox->BxContentWidth = FALSE;
	    }
	  else
	    {
	      pDimAb = &pAb->AbHeight;
	      pBox->BxContentHeight = FALSE;
	    }

	  if (pDimAb->DimAbRef && IsDead (pDimAb->DimAbRef))
	    {
	      /* the dimension refers a dead box */
	      pDimAb->DimAbRef = NULL;
	      pDimAb->DimValue = -1;
	    }
	  else if (pDimAb->DimAbRef == pAb && !pDimAb->DimSameDimension)
	    {
	      if (horizRef && pAb->AbHeight.DimUnit == UnPoint)
		pDimAb->DimUnit = UnPoint;
	      else if (!horizRef && pAb->AbWidth.DimUnit == UnPoint)
		pDimAb->DimUnit = UnPoint;
	    }
	  else if (!horizRef &&
		   pAb->AbLeafType == LtGraphics &&
		   pAb->AbShape == 'a' &&
		   pDimAb->DimAbRef == NULL)
	    {
	      /* force the circle height to be equal to its width */
	      pDimAb->DimAbRef = pAb;
	      pDimAb->DimSameDimension = FALSE;
	      pDimAb->DimValue = 0;
	      pDimAb->DimUserSpecified = FALSE;
	      if (pAb->AbWidth.DimUnit == UnPoint)
		pDimAb->DimUnit = UnPoint;
	      else
		pDimAb->DimUnit = UnPixel;
	    }
	  else if (pAb->AbLeafType == LtCompound &&
		   horizRef && pDimAb->DimAbRef &&
		   pDimAb->DimUnit != UnAuto &&
		   pDimAb->DimAbRef == pAb->AbEnclosing &&
		   pAb->AbEnclosing->AbWidth.DimAbRef == NULL &&
		   pAb->AbEnclosing->AbWidth.DimValue == -1)
	    {
	      /* the boxe width depends on the parent
		 and the parent width depends on the contents */
	      pDimAb->DimAbRef = NULL;
	      pDimAb->DimValue = -1;
	    }
	  else if (pAb->AbLeafType == LtCompound &&
		   !horizRef && pDimAb->DimAbRef &&
		   pDimAb->DimUnit != UnAuto &&
		   pDimAb->DimAbRef == pAb->AbEnclosing &&
		   pAb->AbEnclosing->AbHeight.DimAbRef == NULL &&
		   pAb->AbEnclosing->AbHeight.DimValue == -1)
	    {
	      /* the boxe height depends on the parent
		 and the parent height depends on the contents */
	      pDimAb->DimAbRef = NULL;
	      pDimAb->DimValue = -1;
	    }

	  GetExtraMargins (pBox, NULL, &t, &b, &l, &r);
	  dx = pBox->BxLMargin + pBox->BxLBorder + pBox->BxLPadding + pBox->BxRMargin + pBox->BxRBorder + pBox->BxRPadding + l + r;
	  dy = pBox->BxTMargin + pBox->BxTBorder + pBox->BxTPadding + pBox->BxBMargin + pBox->BxBBorder + pBox->BxBPadding + t + b;
	  if (pParentAb == NULL)
	    {
	      /* It's the root box */
	      inLine = FALSE;
	      if (horizRef)
		{
		  if (pDimAb->DimValue == 0 && pDimAb->DimUnit != UnAuto)
		    /* inherited from the contents */
		    pBox->BxContentWidth = TRUE;
		  else
		    {
		      /* inherited from the window */
		      GetSizesFrame (frame, &val, &i);
		      if (pDimAb->DimValue < 0)
			val += pDimAb->DimValue;
		      else if (pDimAb->DimUnit == UnPercent)
			val = PixelValue (pDimAb->DimValue, UnPercent,
					  (PtrAbstractBox) val, 0);
		      else if (pDimAb->DimUnit == UnAuto)
			val = PixelValue (100, UnPercent,
					  (PtrAbstractBox) val, 0);
		      else
			val = PixelValue (pDimAb->DimValue, pDimAb->DimUnit, pAb,
					  ViewFrameTable[frame - 1].FrMagnification);
		      if (pDimAb->DimValue < 0 || pDimAb->DimUnit == UnPercent ||
			  pDimAb->DimUnit == UnAuto)
			/* the rule gives the outside value */
			val = val - dx;
		      ResizeWidth (pBox, pBox, NULL, val - pBox->BxW, 0, 0, 0, frame);
		    }
		}
	      else
		{
		  if (pDimAb->DimValue == 0 || pDimAb->DimUnit == UnAuto)
		    /* inherited from the contents */
		    pBox->BxContentHeight = TRUE;
		  else
		    {
		      /* inherited from the window */
		      GetSizesFrame (frame, &i, &val);
		      if (pDimAb->DimValue < 0)
			val += pDimAb->DimValue;
		      else if (pDimAb->DimUnit == UnPercent)
			val = PixelValue (pDimAb->DimValue, UnPercent,
					  (PtrAbstractBox) val, 0);
		      else
			/* explicit value */
			val = PixelValue (pDimAb->DimValue, pDimAb->DimUnit, pAb,
					  ViewFrameTable[frame - 1].FrMagnification);
		      if (pDimAb->DimValue < 0 || pDimAb->DimUnit == UnPercent)
			/* the rule gives the outside value */
			val = val - dy;
		      ResizeHeight (pBox, pBox, NULL, val - pBox->BxH, 0, 0, frame);
		    }
		}
	    }
	  else
	    {
	      /* it's not the root box */
	      inLine = (/*pAb->AbFloat == 'N' &&*/!pAb->AbNotInLine &&
			(pAb->AbDisplay == 'I' ||
			 pParentAb->AbBox->BxType == BoBlock ||
			 pParentAb->AbBox->BxType == BoFloatBlock ||
			 pParentAb->AbBox->BxType == BoGhost ||
			 pParentAb->AbBox->BxType == BoFloatGhost));
	      if (horizRef)
		{
		  if (pBox->BxType == BoCell)
		    {
		      /* get the related column */
		      if (pDimAb->DimAbRef)
			pColumn = pDimAb->DimAbRef;
		      else
			pColumn = pAb->AbHorizPos.PosAbRef;
		      if (pColumn)
			InsertDimRelation (pColumn->AbBox, pBox, TRUE, TRUE, FALSE);
		      if (pDimAb->DimUnit == UnAuto)
			{
			  pDimAb->DimAbRef = NULL;
			  pDimAb->DimValue = -1;
			}
		    }
		  else
		    {
		      pColumn = NULL;
		      /* check if the relative box is not already dead */
		      if (pDimAb->DimUnit == UnAuto)
			{
			  if (pAb->AbFloat != 'N' ||
			      pAb->AbNotInLine ||
			      pAb->AbDisplay == 'I' ||
			      /* if the parent inherits from contents */
			      (!pParentAb->AbWidthChange &&
			       pParentAb->AbWidth.DimUnit == UnAuto &&
			       pParentAb->AbWidth.DimAbRef != pParentAb->AbEnclosing &&
			       pParentAb->AbWidth.DimValue == -1) ||
			      /* check the parent rule */
			      (pParentAb->AbWidthChange &&
			       pParentAb->AbEnclosing &&
			       pParentAb->AbEnclosing->AbWidth.DimUnit == UnAuto &&
			       pParentAb->AbEnclosing->AbWidth.DimAbRef != pParentAb->AbEnclosing->AbEnclosing &&
			       pParentAb->AbEnclosing->AbWidth.DimValue == -1))
			    {
			      /* inherit from contents */
			      pDimAb->DimAbRef = NULL;
			      pDimAb->DimValue = -1;		  
			      pBox->BxContentWidth = TRUE;
			    }
			  else
			    {
			      /* inherit from the parent box */
			      pDimAb->DimAbRef = pAb->AbEnclosing;
			      pDimAb->DimValue = 0;		  
			      pBox->BxContentWidth = FALSE;
			    }
			}
		    }

		  if ((inLine && pAb->AbLeafType == LtText) ||
		      (pDimAb->DimAbRef == NULL && pDimAb->DimValue < 0))
		    /* inherited from the contents */
		    pBox->BxContentWidth = TRUE;
		  else if (pDimAb->DimAbRef == NULL ||
			   (pDimAb->DimAbRef == pParentAb &&
			    (pDimAb->DimUnit == UnPercent || pDimAb->DimUnit == UnAuto)))
		    {
		      /* percentage or explicit value */
		      if (pDimAb->DimUnit == UnPercent || pDimAb->DimUnit == UnAuto)
			{
			  if (pBox->BxType != BoCell)
			    {
			      if (pDimAb->DimUnit != UnAuto)
				{
				  while (pParentAb &&
					 !pParentAb->AbWidth.DimIsPosition &&
					 pParentAb->AbWidth.DimValue < 0 &&
					 pParentAb->AbWidth.DimAbRef == NULL)
				    pParentAb = pParentAb->AbEnclosing;
				}
			      /* inherited from the parent */
			      if (pParentAb)
				i = pParentAb->AbBox->BxW;
			      else
				GetSizesFrame (frame, &i, &val);
			      if (pDimAb->DimUnit == UnPercent)
				val = PixelValue (pDimAb->DimValue, UnPercent,
						  (PtrAbstractBox) i, 0);
			      else /* UnAuto */
				val = PixelValue (100, UnPercent, 
						  (PtrAbstractBox) i, 0);
			      /* the rule gives the outside value */
			      val = val - dx;
			      if (pParentAb)
				InsertDimRelation (pParentAb->AbBox, pBox,
						   pDimAb->DimSameDimension, horizRef,
						   inLine);
			    }
			}
		      else
			/* explicit value */
			val = PixelValue (pDimAb->DimValue, pDimAb->DimUnit, pAb,
					  ViewFrameTable[frame - 1].FrMagnification);
		      ResizeWidth (pBox, pBox, NULL, val - pBox->BxW, 0, 0, 0, frame);
		    }
		  else
		    {
		      pPosAb = &pAb->AbHorizPos;
		      if (pDimAb->DimAbRef == pParentAb &&
			  pDimAb->DimUnit != UnAuto &&
			  pParentAb->AbWidth.DimAbRef == NULL &&
			  pParentAb->AbWidth.DimValue <= 0 &&
			  (inLine || pPosAb->PosAbRef != pParentAb ||
			   pPosAb->PosRefEdge != Left ||
			   pPosAb->PosEdge != Left))
			{
			  while (pParentAb &&
				 ((pParentAb->AbWidth.DimAbRef == NULL &&
				   pParentAb->AbWidth.DimValue <= 0) ||
				  pParentAb->AbInLine ||
				  pParentAb->AbBox->BxType == BoGhost ||
				  pParentAb->AbBox->BxType == BoFloatGhost))
			    /* look for the right ancestor */
			    pParentAb = pParentAb->AbEnclosing;
			  if (pParentAb == NULL)
			    {
			      /* inherited from the contents */
			      pBox->BxContentWidth = TRUE;
			      pDimAb->DimAbRef = NULL;
			      pDimAb->DimValue = -1;
			      pDimAb->DimUnit = UnRelative;
			    }
			  else
			    pDimAb->DimAbRef = pParentAb;
			}
		      else if (pDimAb->DimAbRef == pAb && pDimAb->DimSameDimension)
			{
			  /* The dimension cannot depend on itself */
			  /* inherited from the contents */
			  pBox->BxContentWidth = TRUE;
			  pDimAb->DimAbRef = NULL;
			  pDimAb->DimValue = -1;
			  pDimAb->DimUnit = UnRelative;
			}

		      if (pDimAb->DimAbRef)
			{
			  /* Inherit from a box */
			  pRefBox = pDimAb->DimAbRef->AbBox;
			  if (pRefBox == NULL)
			    {
			      /* forwards reference */
			      pRefBox = GetBox (pDimAb->DimAbRef);
			      if (pRefBox != NULL)
				pDimAb->DimAbRef->AbBox = pRefBox;
			    }
			    
			  if (pRefBox)
			    {
			      if (pDimAb->DimAbRef == pParentAb)
				{
				  /* inherited from the parent */
				  /* same dimension? */
				  if (pDimAb->DimSameDimension)
				    val = pRefBox->BxW;
				  else
				    val = pRefBox->BxH;
				  
				  if (inLine && pDimAb->DimSameDimension)
				    {
				      pLine = SearchLine (pBox, frame);
				      if (pLine)
					val = pLine->LiXMax;
				    }
				}
			      else
				{
				  /* same dimension? */
				  if (pDimAb->DimSameDimension)
				    val = pRefBox->BxWidth;
				  else
				    val = pRefBox->BxHeight;
				}

			      /* Convert the distance value */
			      if (pDimAb->DimUnit == UnPercent)
				val = PixelValue (pDimAb->DimValue, UnPercent,
						  (PtrAbstractBox) val, 0);
			      else if (pDimAb->DimUnit == UnAuto)
				val = PixelValue (100, UnPercent,
						  (PtrAbstractBox) val, 0);
			      else
				val += PixelValue (pDimAb->DimValue, pDimAb->DimUnit, pAb,
						   ViewFrameTable[frame - 1].FrMagnification);
			      /* the rule gives the outside value */
			      val = val - dx;
			      ResizeWidth (pBox, pBox, NULL, val - pBox->BxW, 0, 0, 0, frame);
			      /* Marks out of structure relations */
			      if (pDimAb->DimAbRef != pParentAb
				  && pDimAb->DimAbRef->AbEnclosing != pParentAb)
				pBox->BxWOutOfStruct = TRUE;
			      
			      /* Store dependencies */
			      if (pBox->BxType != BoCell)
				InsertDimRelation (pDimAb->DimAbRef->AbBox, pBox,
						   pDimAb->DimSameDimension, horizRef, inLine);
			    }
			}
		    }
		}
	      else
		{
		  pDimAb = &pAb->AbHeight;
		  pEl = pAb->AbElement;
		  if ((inLine && pAb->AbLeafType == LtText) ||
		      (pDimAb->DimAbRef == NULL && pDimAb->DimValue < 0))
		    /* inherited from the contents */
		    pBox->BxContentHeight = TRUE;
		  else if (pDimAb->DimAbRef == NULL ||
			   (pDimAb->DimAbRef == pParentAb &&
			    pDimAb->DimUnit == UnPercent))
		    {
		      if (pDimAb->DimUnit == UnPercent &&
			  (inLine ||
			   TypeHasException (ExcIsTable, pEl->ElTypeNumber,
					     pEl->ElStructSchema) ||
			   TypeHasException (ExcIsRow, pEl->ElTypeNumber,
					     pEl->ElStructSchema) ||
			   TypeHasException (ExcIsCell, pEl->ElTypeNumber,
					     pEl->ElStructSchema)))
			{
			  /* mismatch: inherited from the contents */
			  pBox->BxContentHeight = TRUE;
			  pDimAb->DimAbRef = NULL;
			  pDimAb->DimValue = -1;
			  pDimAb->DimUnit = UnRelative;
			  val = pBox->BxH;
			}
		      else if (pDimAb->DimUnit == UnPercent)
			{
			  while (pParentAb &&
				 !pParentAb->AbHeight.DimIsPosition &&
				 pParentAb->AbHeight.DimValue < 0 &&
				 pParentAb->AbHeight.DimAbRef == NULL)
			    pParentAb = pParentAb->AbEnclosing;
			  /* inherited from the parent */
			  if (pParentAb)
			    val = PixelValue (pDimAb->DimValue, UnPercent, 
					      (PtrAbstractBox) (pParentAb->AbBox->BxH), 0);
			  else
			    {
			      GetSizesFrame (frame, &val, &i);
			      val = PixelValue (pDimAb->DimValue, UnPercent, 
						(PtrAbstractBox) i, 0);
			    }
			  /* the rule gives the outside value */
			  val = val - dy;
			  if (pParentAb)
			    InsertDimRelation (pParentAb->AbBox, pBox,
					       pDimAb->DimSameDimension, horizRef, inLine);
			}
		      else
			/* explicit value */
			val = PixelValue (pDimAb->DimValue, pDimAb->DimUnit, pAb, ViewFrameTable[frame - 1].FrMagnification);
		      ResizeHeight (pBox, pBox, NULL, val - pBox->BxH, 0, 0, frame);
		    }
		  else
		    {
		      pPosAb = &pAb->AbVertPos;
		      if (pDimAb->DimAbRef == pParentAb &&
			  pParentAb->AbEnclosing != NULL &&
			  /* parent depends on the content */
			  pParentAb->AbHeight.DimAbRef == NULL &&
			  pParentAb->AbHeight.DimValue <= 0 &&
			  (pPosAb->PosAbRef != pParentAb || pPosAb->PosRefEdge != Top
			   || pPosAb->PosEdge != Top))
			{
			  /* mismatch: inherited from the contents */
			  pBox->BxContentHeight = TRUE;
			  pDimAb->DimAbRef = NULL;
			  pDimAb->DimValue = -1;
			  pDimAb->DimUnit = UnRelative;
			}
		      else
			{
			  if (pDimAb->DimAbRef == pAb && pDimAb->DimSameDimension)
			    {
			      /* The dimension cannot depend on itself */
			      /* inherited from the contents */
			      pBox->BxContentHeight = TRUE;
			      pDimAb->DimAbRef = NULL;
			      pDimAb->DimValue = -1;
			      pDimAb->DimUnit = UnRelative;
			    }
			  else
			    {
			      /* Inherit from a box */
			      pRefBox = pDimAb->DimAbRef->AbBox;
			      if (pRefBox == NULL)
				{
				  /* forwards reference */
				  pRefBox = GetBox (pDimAb->DimAbRef);
				  if (pRefBox != NULL)
				    pDimAb->DimAbRef->AbBox = pRefBox;
				}
			    
			      if (pRefBox != NULL)
				{
				  if (pDimAb->DimAbRef == pParentAb)
				    {
				      /* inherited from the parent */
				      /* same dimension? */
				      if (pDimAb->DimSameDimension)
					val = pRefBox->BxH;
				      else
					val = pRefBox->BxW;
				
				      if (inLine && !pDimAb->DimSameDimension)
					{
					  /* remove the indentation value  */
					  if (pParentAb->AbIndentUnit == UnPercent)
					    delta = PixelValue (pParentAb->AbIndent, UnPercent, (PtrAbstractBox) val, 0);
					  else
					    delta = PixelValue (pParentAb->AbIndent, pParentAb->AbIndentUnit, pParentAb, ViewFrameTable[frame - 1].FrMagnification);
					  if (pParentAb->AbIndent > 0)
					    val -= delta;
					  else if (pParentAb->AbIndent < 0)
					    val += delta;
					}
				    }
				  else
				    {
				      /* inherited from another box */
				      /* same dimension? */
				      if (pDimAb->DimSameDimension)
					val = pRefBox->BxHeight;
				      else
					val = pRefBox->BxWidth;
				    }
				
				  if (pDimAb->DimUnit == UnPercent)
				    val = PixelValue (pDimAb->DimValue, UnPercent, (PtrAbstractBox) val, 0);
				  else
				    val += PixelValue (pDimAb->DimValue, pDimAb->DimUnit, pAb, ViewFrameTable[frame - 1].FrMagnification);
				  /* the rule gives the outside value */
				  val = val - dy;
				  ResizeHeight (pBox, pBox, NULL, val - pBox->BxH, 0, 0, frame);
				  /* Marks out of structure relations */
				  if (pDimAb->DimAbRef != pParentAb
				      && pDimAb->DimAbRef->AbEnclosing != pParentAb)
				    pBox->BxHOutOfStruct = TRUE;
				  
				  /* Store dependencies */
				  if (pDimAb->DimAbRef == pAb &&
				      pDimAb->DimSameDimension)
				    {
				      /* Check errors */
				      if (horizRef)
					fprintf (stderr, "Bad HorizPos rule on %s\n", AbsBoxType (pAb, TRUE));
				      else
					fprintf (stderr, "Bad VertPos rule on %s\n", AbsBoxType (pAb, TRUE));
				    }
				  InsertDimRelation (pDimAb->DimAbRef->AbBox, pBox,
						     pDimAb->DimSameDimension, horizRef, inLine);
				}
			    }
			}
		    }
		}
	    }
	}
      /* C'est une boite elastique */
      else if (horizRef)
	{
	  pDimAb = &pAb->AbWidth;
	  /* Box elastique en X */
	  pPosAb = &pDimAb->DimPosition;
	  op = OpWidth;
	  /* On teste si la relation est hors structure */
	  if (pPosAb->PosAbRef != pParentAb
	      && pPosAb->PosAbRef->AbEnclosing != pParentAb)
	    pBox->BxWOutOfStruct = TRUE;
	  else if (pPosAb->PosAbRef->AbBox != NULL)
	    pBox->BxWOutOfStruct = pPosAb->PosAbRef->AbBox->BxXOutOfStruct;
	  
	  /* Des boites voisines heritent de la relation hors-structure ? */
	  if (pParentAb != NULL)
	    {
	      pChildAb = pParentAb->AbFirstEnclosed;
	      while (pChildAb != NULL)
		{
		  if (pChildAb != pAb && pChildAb->AbBox != NULL)
		    {
		      /* Si c'est un heritage on note l'indication hors-structure */
		      if (pChildAb->AbHorizPos.PosAbRef == pAb
			  && pChildAb->AbHorizPos.PosRefEdge != Left)
			{
			  if (!IsXPosComplete (pChildAb->AbBox))
			    /* la boite  est maintenant placee en absolu */
			    pChildAb->AbBox->BxXToCompute = TRUE;
			  pChildAb->AbBox->BxXOutOfStruct = TRUE;
			  if (pChildAb->AbEnclosing == pAb->AbEnclosing)
			    pChildAb->AbHorizEnclosing = pAb->AbHorizEnclosing;
			  PropagateXOutOfStruct (pChildAb, TRUE, pChildAb->AbHorizEnclosing);
			}
		      if (pChildAb->AbVertPos.PosAbRef == pAb
			  && pChildAb->AbVertPos.PosRefEdge != Top
			  && pAb->AbLeafType == LtCompound
			  && pAb->AbInLine)
			{
			  if (!IsYPosComplete (pChildAb->AbBox))
			    /* la boite  est maintenant placee en absolu */
			    pChildAb->AbBox->BxYToCompute = TRUE;
			  pChildAb->AbBox->BxYOutOfStruct = TRUE;
			  if (pChildAb->AbEnclosing == pAb->AbEnclosing)
			    pChildAb->AbVertEnclosing = pAb->AbVertEnclosing;
			  PropagateYOutOfStruct (pChildAb, TRUE, pChildAb->AbVertEnclosing);
			}
		    }
		  pChildAb = pChildAb->AbNext;
		}
	    }

	  /* Decalage par rapport a la boite distante */
	  pRefBox = pPosAb->PosAbRef->AbBox;
	  if (pRefBox == NULL)
	    {
	      /* On doit resoudre une reference en avant */
	      if (!IsDead (pPosAb->PosAbRef))
		pRefBox = GetBox (pPosAb->PosAbRef);
	      if (pRefBox != NULL)
		pPosAb->PosAbRef->AbBox = pRefBox;
	    }

	  if (pRefBox != NULL)
	    {
	      /* regarde si la position depend d'une boite invisible */
	      if (pPosAb->PosAbRef->AbVisibility < ViewFrameTable[frame - 1].FrVisibility)
		delta = 0;
	      else
		{
		  /* Convert the distance value */
		  if (pAb->AbEnclosing == NULL)
		    dim = 0;
		  else
		    dim = pAb->AbEnclosing->AbBox->BxW;
		  if (pPosAb->PosUnit == UnPercent)
		    delta = PixelValue (pPosAb->PosDistance, UnPercent,
					(PtrAbstractBox) dim, 0);
		  else
		    delta = PixelValue (pPosAb->PosDistance, pPosAb->PosUnit,
					pAb, ViewFrameTable[frame - 1].FrMagnification);
		  if (pPosAb->PosDeltaUnit == UnPercent)
		    delta += PixelValue (pPosAb->PosDistDelta, UnPercent,
					(PtrAbstractBox) dim, 0);
		  else
		    delta += PixelValue (pPosAb->PosDistDelta, pPosAb->PosDeltaUnit,
					pAb, ViewFrameTable[frame - 1].FrMagnification);
		}
	      val = pRefBox->BxXOrg + delta;
	      switch (pPosAb->PosRefEdge)
		{
		case Left:
		  break;
		case Right:
		  val += pRefBox->BxWidth;
		  break;
		case VertMiddle:
		  val += pRefBox->BxWidth / 2;
		  break;
		case VertRef:
		  val += pRefBox->BxVertRef;
		  break;
		default:
		  break;
		}

	      /* Calcule la largeur de la boite */
	      val = val - pBox->BxXOrg - pBox->BxWidth;
	      /* La boite n'a pas de point fixe */
	      pBox->BxHorizEdge = NoEdge;
	      InsertPosRelation (pBox, pRefBox, op, pPosAb->PosEdge, pPosAb->PosRefEdge);
	      
	      if (!IsXPosComplete (pBox))
		/* la boite  devient maintenant placee en absolu */
		pBox->BxXToCompute = TRUE;
	      /* La boite est marquee elastique */
	      pBox->BxHorizFlex = TRUE;
	      pRefBox->BxMoved = NULL;
	      MoveBoxEdge (pBox, pRefBox, op, val, frame, TRUE);
	    }
	}
      else
	{
	  pDimAb = &pAb->AbHeight;
	  /* Box elastique en Y */
	  pPosAb = &pDimAb->DimPosition;
	  op = OpHeight;	  
	  /* On teste si la relation est hors structure */
	  if (pPosAb->PosAbRef != pParentAb
	      && pPosAb->PosAbRef->AbEnclosing != pParentAb)
	    pBox->BxHOutOfStruct = TRUE;
	  else if (pPosAb->PosAbRef->AbBox != NULL)
	    pBox->BxHOutOfStruct = pPosAb->PosAbRef->AbBox->BxYOutOfStruct;
	  
	  /* Des boites voisines heritent de la relation hors-structure ? */
	  if (pParentAb != NULL /* && pBox->BxHOutOfStruct */ )
	    {
	      pChildAb = pParentAb->AbFirstEnclosed;
	      while (pChildAb != NULL)
		{
		  if (pChildAb != pAb && pChildAb->AbBox != NULL)
		    {
		      /* Si c'est un heritage on note l'indication hors-structure */
		      if (pChildAb->AbVertPos.PosAbRef == pAb
			  && pChildAb->AbVertPos.PosRefEdge != Top)
			{
			  if (!IsYPosComplete (pChildAb->AbBox))
			    /* la boite  est maintenant placee en absolu */
			    pChildAb->AbBox->BxYToCompute = TRUE;
			  pChildAb->AbBox->BxYOutOfStruct = TRUE;
			  if (pChildAb->AbEnclosing == pAb->AbEnclosing)
			    pChildAb->AbVertEnclosing = pAb->AbVertEnclosing;
			  PropagateYOutOfStruct (pChildAb, TRUE, pChildAb->AbVertEnclosing);
			}
		    }
		  pChildAb = pChildAb->AbNext;
		}
	    }

	  /* Decalage par rapport a la boite distante */
	  pRefBox = pPosAb->PosAbRef->AbBox;
	  if (pRefBox == NULL)
	    {
	      /* On doit resoudre une reference en avant */
	      pRefBox = GetBox (pPosAb->PosAbRef);
	      if (pRefBox != NULL)
		pPosAb->PosAbRef->AbBox = pRefBox;
	    }

	  if (pRefBox != NULL)
	    {
	      /* regarde si la position depend d'une boite invisible */
	      if (pPosAb->PosAbRef->AbVisibility < ViewFrameTable[frame - 1].FrVisibility)
		delta = 0;
	      else
		{
		  /* Convert the distance value */
		  if (pAb->AbEnclosing == NULL)
		    dim = 0;
		  else
		    dim = pAb->AbEnclosing->AbBox->BxH;
		  if (pPosAb->PosUnit == UnPercent)
		    delta = PixelValue (pPosAb->PosDistance, UnPercent,
					(PtrAbstractBox) dim, 0);
		  else
		    delta = PixelValue (pPosAb->PosDistance, pPosAb->PosUnit,
					pAb, ViewFrameTable[frame - 1].FrMagnification);
		  if (pPosAb->PosDeltaUnit == UnPercent)
		    delta += PixelValue (pPosAb->PosDistDelta, UnPercent,
					(PtrAbstractBox) dim, 0);
		  else
		    delta += PixelValue (pPosAb->PosDistDelta, pPosAb->PosDeltaUnit,
					pAb, ViewFrameTable[frame - 1].FrMagnification);
		}
	      val = pRefBox->BxYOrg + delta;
	      switch (pPosAb->PosRefEdge)
		{
		case Bottom:
		  val += pRefBox->BxHeight;
		  break;
		case HorizMiddle:
		  val += pRefBox->BxHeight / 2;
		  break;
		case HorizRef:
		  val += pRefBox->BxHorizRef;
		  break;
		default:
		  break;
		}

	      /* Calcule la hauteur de la boite */
	      val = val - pBox->BxYOrg - pBox->BxHeight;
	      /* La boite n'a pas de point fixe */
	      pBox->BxVertEdge = NoEdge;
	      InsertPosRelation (pBox, pRefBox, op, pPosAb->PosEdge, pPosAb->PosRefEdge);

	      if (!IsYPosComplete (pBox))
		/* la boite  devient maintenant placee en absolu */
		pBox->BxYToCompute = TRUE;
	      /* La boite est marquee elastique */
	      pBox->BxVertFlex = TRUE;
	      ClearBoxMoved (pBox);
	      MoveBoxEdge (pBox, pRefBox, op, val, frame, FALSE);
	    }
	}
    }

  /* La regle de dimension est interpretee */
  if (horizRef)
    {
      pAb->AbWidthChange = FALSE;
      /* Marque dans la boite si la dimension depend du contenu ou non */
      pBox->BxRuleWidth = 0;
      defaultDim = pBox->BxContentWidth;
    }
  else
    {
      pAb->AbHeightChange = FALSE;
      /* Marque dans la boite si la dimension depend du contenu ou non */
      pBox->BxRuleHeigth = 0;
      defaultDim = pBox->BxContentHeight;
    }

  /* break down the temporary link of moved boxes */
  ClearBoxMoved (pBox);
  return (defaultDim);
}


/*----------------------------------------------------------------------
  ComputeAxisRelation applique la regle de positionnement donnee en 
  parametre a` la boite d'indice pBox. L'axe              
  horizontal ou vertical de la boite, selon que horizRef  
  est VRAI ou FAUX est mis a` jour.                       
  ----------------------------------------------------------------------*/
void ComputeAxisRelation (AbPosition rule, PtrBox pBox, int frame, ThotBool horizRef)
{
  PtrBox              pRefBox;
  PtrAbstractBox      pRefAb;
  PtrAbstractBox      pAb;
  BoxEdge             refEdge, localEdge;
  int                 x, y, dist;
  int                 t, b, l, r;

  pRefAb = rule.PosAbRef;
  pAb = pBox->BxAbstractBox;
   /* don't manage invisible boxes */
   if (pAb->AbVisibility < ViewFrameTable[frame - 1].FrVisibility)
     return;
 
  /* Verifie que la position ne depend pas d'un pave mort */
  if (pRefAb &&
      (IsDead (pRefAb) ||
       pRefAb->AbVisibility < ViewFrameTable[frame - 1].FrVisibility))
    pRefAb = NULL;

  if (pRefAb == NULL)
    {
      /* default rule */
      pRefBox = pBox;
      if (horizRef)
	{
	  refEdge = Left;
	  localEdge = VertRef;
	  if (rule.PosUnit == UnPercent)
	    dist = PixelValue (rule.PosDistance, UnPercent,
			       (PtrAbstractBox) (pBox->BxW), 0);
	  else
	    dist = 0;
	  if (rule.PosDeltaUnit == UnPercent)
	    dist += PixelValue (rule.PosDistDelta, UnPercent,
			       (PtrAbstractBox) (pBox->BxW), 0);
	}
      else
	{
	  refEdge = HorizRef;
	  localEdge = HorizRef;
	  if (rule.PosUnit == UnPercent)
	    dist = PixelValue (rule.PosDistance, UnPercent,
			       (PtrAbstractBox) (pBox->BxH), 0);
	  else
	    dist = BoxFontBase (pBox->BxFont);
	  if (rule.PosDeltaUnit == UnPercent)
	    dist += PixelValue (rule.PosDistDelta, UnPercent,
			       (PtrAbstractBox) (pBox->BxH), 0);
	}
    }
  else
    {
      /* explicit rule */
      refEdge = rule.PosRefEdge;
      localEdge = rule.PosEdge;
      /* Convert the distance value */
      if (rule.PosUnit == UnPercent)
	{
	  if (horizRef)
	    dist = PixelValue (rule.PosDistance, UnPercent,
			       (PtrAbstractBox) ( pAb->AbEnclosing->AbBox->BxW), 0);
	  else
	    dist = PixelValue (rule.PosDistance, UnPercent,
			       (PtrAbstractBox) (pAb->AbEnclosing->AbBox->BxH), 0);
	}
      else
	dist = PixelValue (rule.PosDistance, rule.PosUnit, pAb,
			   ViewFrameTable[frame - 1].FrMagnification);
      if (rule.PosDeltaUnit == UnPercent)
	{
	  if (horizRef)
	    dist += PixelValue (rule.PosDistDelta, UnPercent,
			       (PtrAbstractBox) (pAb->AbEnclosing->AbBox->BxW), 0);
	  else
	    dist += PixelValue (rule.PosDistDelta, UnPercent,
			       (PtrAbstractBox) (pAb->AbEnclosing->AbBox->BxH), 0);
	}
      else
	dist += PixelValue (rule.PosDistDelta, rule.PosDeltaUnit, pAb,
			   ViewFrameTable[frame - 1].FrMagnification);
      
      pRefBox = pRefAb->AbBox;
      if (pRefBox == NULL)
	{
	  /* On doit resoudre une reference en avant */
	  pRefBox = GetBox (pRefAb);
	  if (pRefBox)
	    pRefAb->AbBox = pRefBox;
	  else
	    {
	      if (horizRef)
		pAb->AbVertRefChange = FALSE;
	      else
		pAb->AbHorizRefChange = FALSE;
	      return;	/* plus de boite libre */
	    }
	}
    }

  /* Deplacement par rapport a la boite distante */
  /* L'axe est place par rapport a la boite elle-meme */
  if (pRefBox == pBox ||
      pBox->BxType == BoGhost ||
      pBox->BxType == BoFloatGhost ||
      pBox->BxType == BoBlock ||
      pBox->BxType == BoFloatBlock)
    {
      GetExtraMargins (pBox, NULL, &t, &b, &l, &r);
      x = pBox->BxLMargin + pBox->BxLBorder + pBox->BxLPadding + l;
      y = pBox->BxTMargin + pBox->BxTBorder + pBox->BxTPadding + t;
    }
  /* L'axe est place par rapport a une incluse */
  else if (Propagate != ToSiblings)
    {
      /* Il faut peut-etre envisager que pRefBox soit une boite coupee */
      x = pRefBox->BxXOrg - pBox->BxXOrg;
      y = pRefBox->BxYOrg - pBox->BxYOrg;
    }
  else
    {
      x = pRefBox->BxXOrg;
      y = pRefBox->BxYOrg;
    }

  switch (refEdge)
    {
    case Left:
    case Top:
      break;
    case Bottom:
      y += pRefBox->BxH;
      break;
    case Right:
      x += pRefBox->BxW;
      break;
    case HorizRef:
      if (pRefBox != pBox)
	y += pRefBox->BxHorizRef;
      break;
    case VertRef:
      if (pRefBox != pBox)
	x += pRefBox->BxVertRef;
      break;
    case HorizMiddle:
      y += pRefBox->BxHeight / 2;
      break;
    case VertMiddle:
      x += pRefBox->BxWidth / 2;
      break;
    case NoEdge:
      break;
    }

  /* Met a jour l'axe de la boite */
  if (horizRef)
    {
      x = x + dist - pBox->BxVertRef;
      MoveVertRef (pBox, NULL, x, frame);
      /* la regle axe de reference est interpretee */
      pAb->AbVertRefChange = FALSE;
      if (pRefBox != NULL)
	InsertPosRelation (pBox, pRefBox, OpHorizRef, localEdge, refEdge);
    }
  else
    {
      y = y + dist - pBox->BxHorizRef;
      MoveHorizRef (pBox, NULL, y, frame);
      /* la regle axe de reference est interpretee */
      pAb->AbHorizRefChange = FALSE;
      if (pRefBox != NULL)
	InsertPosRelation (pBox, pRefBox, OpVertRef, localEdge, refEdge);
    }
  
  /* break down the temporary link of moved boxes */
  ClearBoxMoved (pBox);
}


/*----------------------------------------------------------------------
  RemovePosRelation removes the typed link that exists from the
  pOrginBox to the pTargetBox.
  The type could be horizontal or vertical position or axis.
  The parameter pCurrentAb, when not NULL, points to the abstract box
  of which the position rule is removed and it used to avoid ambiguity
  for bidirectional relations.
  A relation associated to a position rule that refers pCurrentAb must
  not be removed.
  When pCurrentAb is NULL, we're removing a reverse link.
  ----------------------------------------------------------------------*/
static ThotBool RemovePosRelation (PtrBox pOrginBox, PtrBox pTargetBox,
				   PtrAbstractBox pCurrentAb, ThotBool Pos,
				   ThotBool Axe, ThotBool horizRef)
{
  PtrPosRelations     pPreviousPosRel;
  PtrPosRelations     precpos;
  PtrPosRelations     pPosRel;
  PtrAbstractBox      pAb;
  BoxRelation        *pRelation;
  int                 i, found;
  ThotBool            loop;
  ThotBool            notEmpty;
  
  /* Look for the removed entry and the last used entry */
  found = 0;
  i = 0;
  pPreviousPosRel = NULL;
  pOrginBox->BxMoved = NULL;
  pPosRel = pOrginBox->BxPosRelations;
  precpos = NULL;
  loop = TRUE;
  if (pPosRel != NULL)
    while (loop)
      {
	i = 1;
	notEmpty = pPosRel->PosRTable[i - 1].ReBox != NULL;
	while (i <= MAX_RELAT_POS && notEmpty)
	  {
	    pRelation = &pPosRel->PosRTable[i - 1];
	    if (horizRef)
	      {
		/* horizontal relation */
		if (pRelation->ReBox == pTargetBox &&
		    ((Axe && pRelation->ReOp == OpHorizRef) ||
		     (Pos && pRelation->ReOp == OpHorizInc) ||
		     (Pos && pCurrentAb == NULL && pRelation->ReOp == OpHorizDep) ||
		     /* stretched dimension */
		     (!Pos && !Axe && pRelation->ReOp == OpWidth)))
		  {
		    /* the relation is found */
		    found = i;
		    pPreviousPosRel = pPosRel;
		  }
		else if (found == 0 && Pos && pRelation->ReOp == OpHorizDep &&
			 pCurrentAb != NULL && pRelation->ReBox == pTargetBox)
		  {
		    
		    /* it's the position relation of pCurrentAb */
		    pAb = pRelation->ReBox->BxAbstractBox;
		    pAb = pAb->AbHorizPos.PosAbRef;
		    
		    if (pAb == NULL)
		      /* a rule NIL, get the associated abstract box */
		      pAb = GetPosRelativeAb (pRelation->ReBox->BxAbstractBox, horizRef);
		    if (pAb != pCurrentAb)
		      {
			found = i;
			pPreviousPosRel = pPosRel;
		      }
		  }
	      }
	    else
	      {
		/* vertical relation */
		if (pRelation->ReBox == pTargetBox &&
		    ((Axe && pRelation->ReOp == OpVertRef) ||
		     (Pos && pRelation->ReOp == OpVertInc) ||
		     (Pos && pCurrentAb == NULL && pRelation->ReOp == OpVertDep) ||
		     /* stretched dimension */
		     (!Pos && !Axe && pRelation->ReOp == OpHeight)))
		  {
		    /* the relation is found */
		    found = i;
		    pPreviousPosRel = pPosRel;
		  }
		else if (found == 0 && Pos && pRelation->ReOp == OpVertDep &&
			 pCurrentAb != NULL && pRelation->ReBox == pTargetBox)
		  {
		    /* it's the position relation of pCurrentAb */
		    pAb = pRelation->ReBox->BxAbstractBox;
		    pAb = pAb->AbVertPos.PosAbRef;
		    if (pAb == NULL)
		      /* a rule NIL, get the associated abstract box */
		      pAb = GetPosRelativeAb (pRelation->ReBox->BxAbstractBox, horizRef);
		    if (pAb != pCurrentAb)
		      {
			found = i;
			pPreviousPosRel = pPosRel;
		      }
		  }
	      }
	    i++;
	    if (i <= MAX_RELAT_POS)
	      notEmpty = pPosRel->PosRTable[i - 1].ReBox != NULL;
	  }
	
	if (pPosRel->PosRNext == NULL)
	  loop = FALSE;
	else
	  {
	    precpos = pPosRel;
	    /* Next block */
	    pPosRel = pPosRel->PosRNext;
	  }
      }

  if (found > 0)
    {
      /* reorganize the list */
      pRelation = &pPreviousPosRel->PosRTable[found - 1];      
      if (pCurrentAb != NULL &&
	  (pRelation->ReOp == OpHorizDep || pRelation->ReOp == OpVertDep))
	/* Remove the reverse relation */
	loop = RemovePosRelation (pRelation->ReBox, pOrginBox, NULL, Pos, Axe, horizRef);
      
      if (i > 1)
	{
	  i--;
	  pRelation->ReBox = pPosRel->PosRTable[i - 1].ReBox;
	  pRelation->ReRefEdge = pPosRel->PosRTable[i - 1].ReRefEdge;
	  pRelation->ReOp = pPosRel->PosRTable[i - 1].ReOp;
	}
      
      /* free a block if necessary */
      if (i == 1)
	{
	  if (precpos == NULL)
	    pOrginBox->BxPosRelations = NULL;
	  else
	    precpos->PosRNext = NULL;
	  FreePosBlock (&pPosRel);
	}
      else
	pPosRel->PosRTable[i - 1].ReBox = NULL;
      return TRUE;
    }
  else
    return FALSE;
}


/*----------------------------------------------------------------------
  RemoveDimRelation removes if it exists the horizontal or vertical link
  from pOrginBox to pTargetBox and compact the list.
  Return TRUE if the operation is done.
  ----------------------------------------------------------------------*/
static ThotBool RemoveDimRelation (PtrBox pOrginBox, PtrBox pTargetBox,
				   ThotBool horizRef)
{
  PtrDimRelations     pFoundDimRel;
  PtrDimRelations     pPreviousDimRel;
  PtrDimRelations     pDimRel;
  int                 i, found;
  ThotBool            loop;
  ThotBool            notEmpty;
  ThotBool            result;

  i = 0;
  /* Cela peut etre une dimension elastique */
  if ((horizRef && pTargetBox->BxHorizFlex) ||
      (!horizRef && pTargetBox->BxVertFlex))
    result = RemovePosRelation (pOrginBox, pTargetBox, NULL, FALSE, FALSE, horizRef);
  else
    {
      /* On recherche l'entree a detruire et la derniere entree occupee */
      found = 0;
      pFoundDimRel = NULL;
      if (horizRef)
	pDimRel = pOrginBox->BxWidthRelations;
      else
	pDimRel = pOrginBox->BxHeightRelations;
      pPreviousDimRel = NULL;
      loop = TRUE;
      if (pDimRel != NULL)
	while (loop)
	  {
	    i = 1;
	    notEmpty = pDimRel->DimRTable[i - 1] != NULL;
	    while (i <= MAX_RELAT_DIM && notEmpty)
	      {
		/* Est-ce l'entree a detruire ? */
		if (pDimRel->DimRTable[i - 1] == pTargetBox)
		  {
		    found = i;
		    pFoundDimRel = pDimRel;
		  }
		i++;
		if (i <= MAX_RELAT_DIM)
		  notEmpty = pDimRel->DimRTable[i - 1] != NULL;
	      }
	    
	    if (pDimRel->DimRNext == NULL)
	      loop = FALSE;
	    else
	      {
		pPreviousDimRel = pDimRel;
		/* Bloc suivant */
		pDimRel = pDimRel->DimRNext;
	      }
	  }
      
      /* On a trouve -> on retasse la liste */
      if (found > 0)
	{
	  i--;
	  pFoundDimRel->DimRTable[found - 1] = pDimRel->DimRTable[i - 1];
	  pFoundDimRel->DimRSame[found - 1] = pDimRel->DimRSame[i - 1];
	  
	  /* Faut-il liberer le dernier bloc de relations ? */
	  if (i == 1)
	    {
	      if (pPreviousDimRel == NULL)
		if (horizRef)
		  pOrginBox->BxWidthRelations = NULL;
		else
		  pOrginBox->BxHeightRelations = NULL;
	      else
		pPreviousDimRel->DimRNext = NULL;
	      FreeDimBlock (&pDimRel);
	    }
	  else
	    pDimRel->DimRTable[i - 1] = NULL;
	  result = TRUE;
	}
      else
	/* On n'a pas trouve */
	result = FALSE;
    }
  return result;
}


/*----------------------------------------------------------------------
  ClearOutOfStructRelation removes out of structure relations of the box
  pBox.
  ----------------------------------------------------------------------*/
void ClearOutOfStructRelation (PtrBox pBox)
{
  PtrBox              pOrginBox;
  PtrAbstractBox      pAb;

  pAb = pBox->BxAbstractBox;
  /* On detruit la relation de position horizontale hors-structure */
  if (pBox->BxXOutOfStruct)
    {
      if (pAb->AbHorizPos.PosAbRef == NULL)
	pOrginBox = NULL;
      else
	pOrginBox = pAb->AbHorizPos.PosAbRef->AbBox;
      if (pOrginBox != NULL)
	RemovePosRelation (pOrginBox, pBox, NULL, TRUE, FALSE, TRUE);
      /* Annule les relations hors-structure */
      PropagateXOutOfStruct (pAb, FALSE, pAb->AbHorizEnclosing);
    }
  
  /* On detruit la relation de position verticale hors-structure */
  if (pBox->BxYOutOfStruct)
    {
      if (pAb->AbVertPos.PosAbRef == NULL)
	pOrginBox = NULL;
      else
	pOrginBox = pAb->AbVertPos.PosAbRef->AbBox;
      if (pOrginBox != NULL)
	RemovePosRelation (pOrginBox, pBox, NULL, TRUE, FALSE, FALSE);
      /* Annule les relations hors-structure */
      PropagateYOutOfStruct (pAb, FALSE, pAb->AbVertEnclosing);
    }

  /* On detruit la relation de largeur hors-structure */
  if (pBox->BxWOutOfStruct)
    {
      /* Est-ce une dimension elastique ? */
      if (pBox->BxHorizFlex)
	{
	  pOrginBox = pAb->AbWidth.DimPosition.PosAbRef->AbBox;
	  if (pOrginBox != NULL)
	    RemovePosRelation (pOrginBox, pBox, NULL, FALSE, FALSE, TRUE);
	}
      else
	{
	  pOrginBox = pAb->AbWidth.DimAbRef->AbBox;
	  if (pOrginBox != NULL)
	    RemoveDimRelation (pOrginBox, pBox, TRUE);
	}
    }
  /* On detruit la relation de hauteur hors-structure */
  pOrginBox = NULL;
  if (pBox->BxHOutOfStruct)
    {
      /* Est-ce une dimension elastique ? */
      if (pBox->BxVertFlex)
	{
	  if (pAb->AbHeight.DimPosition.PosAbRef != NULL)
	    pOrginBox = pAb->AbHeight.DimPosition.PosAbRef->AbBox;
	  if (pOrginBox != NULL)
	    RemovePosRelation (pOrginBox, pBox, NULL, FALSE, FALSE, FALSE);
	}
      else
	{
	  if (pAb->AbHeight.DimAbRef != NULL)
	    pOrginBox = pAb->AbHeight.DimAbRef->AbBox;
	   if (pOrginBox != NULL)
	     RemoveDimRelation (pOrginBox, pBox, FALSE);
	}
    }
}


/*----------------------------------------------------------------------
  ClearPosRelation recherche la boite dont depend la position horizontale
  (si horizRef est Vrai) sinon verticale de pBox :
  - Elle peut dependre de son englobante (relation OpInclus chez elle).
  - Elle peut dependre d'une voisine (deux relations OpLie).
  - Elle peut avoir une relation hors-structure (deux relations OpLie).
  Si cette dependance existe encore, on detruit les relations entre les
  deux boites. L'indicateur BtX(Y)HorsStruct indique que la relation est
  hors-structure.
  ----------------------------------------------------------------------*/
void ClearPosRelation (PtrBox pBox, ThotBool horizRef)
{
  PtrAbstractBox      pRefAb;
  PtrAbstractBox      pCurrentAb;
  ThotBool            loop;

  pRefAb = pBox->BxAbstractBox;
  loop = TRUE;
  /* Process differently when the box is linked with a OutOfStruct box? */
  if ((horizRef && pBox->BxXOutOfStruct) || (!horizRef && pBox->BxYOutOfStruct))
    {
      /* ClearOutOfStructRelation removes out of structure relations */
      if (!IsDead (pRefAb))
	{
	  /* go up to the root abstract box */
	  pCurrentAb = pRefAb;
	  /* La relation hors-structure peut etre heritee d'une boite voisine */
	  while (pCurrentAb->AbEnclosing)
	    pCurrentAb = pCurrentAb->AbEnclosing;
	  
	  /* look for a relation with this out of structure relation */
	  while (loop && pCurrentAb)
	    {
	      if (pCurrentAb->AbBox)
		loop = !RemovePosRelation (pBox, pCurrentAb->AbBox, pRefAb, TRUE, FALSE, horizRef);
	      pCurrentAb = NextAbToCheck (pCurrentAb, pRefAb);
	    }
	  
	  /* this out of structure relation is removed */
	  if (horizRef)
	    {
	      pBox->BxXOutOfStruct = FALSE;
	      /* sibling boxes inherited this out of structure relation? */
	      pCurrentAb = pRefAb->AbEnclosing;
	      if (pCurrentAb)
		{
		  /* check the parent and siblings */
		  pCurrentAb = pCurrentAb->AbFirstEnclosed;
		  while (pCurrentAb)
		    {
		      if (pCurrentAb != pRefAb && pCurrentAb->AbBox)
			{
			  /* Si c'est un heritage on retire l'indication
			     hors-structure */
			  if (pCurrentAb->AbBox->BxXOutOfStruct
			      && pCurrentAb->AbHorizPos.PosAbRef == pRefAb)
			    pCurrentAb->AbBox->BxXOutOfStruct = FALSE;
			  else if (pCurrentAb->AbBox->BxWOutOfStruct &&
				   pCurrentAb->AbWidth.DimIsPosition &&
				   pCurrentAb->AbWidth.DimPosition.PosAbRef == pRefAb)
			    pCurrentAb->AbBox->BxWOutOfStruct = FALSE;
			}
		      pCurrentAb = pCurrentAb->AbNext;
		    }
		}
	    }
	  else
	    {
	      pBox->BxYOutOfStruct = FALSE;
	      /* Des boites voisines ont herite de la relation
		 hors-structure ? */
	      pCurrentAb = pRefAb->AbEnclosing;
		  if (pCurrentAb != NULL)
		    {
		      pCurrentAb = pCurrentAb->AbFirstEnclosed;
		      while (pCurrentAb)
			{
			  if (pCurrentAb != pRefAb && pCurrentAb->AbBox)
			    {
			      /* Si c'est un heritage on retire l'indication
				 hors-structure */
			      if (pCurrentAb->AbBox->BxYOutOfStruct
				  && pCurrentAb->AbVertPos.PosAbRef == pRefAb)
				pCurrentAb->AbBox->BxYOutOfStruct = FALSE;
			      else if (pCurrentAb->AbBox->BxHOutOfStruct &&
				       pCurrentAb->AbHeight.DimIsPosition &&
				       pCurrentAb->AbHeight.DimPosition.PosAbRef == pRefAb)
				pCurrentAb->AbBox->BxHOutOfStruct = FALSE;
			    }
			  pCurrentAb = pCurrentAb->AbNext;
			}
		    }
	    }
	}
    }

  if (loop)
    {
      /* check local relations? */
      pCurrentAb = pRefAb->AbEnclosing;
      /* check a relation with the parent box or a sibling box */
      while (loop && pCurrentAb)
	{
	  if (pCurrentAb->AbBox)
	    loop = !RemovePosRelation (pBox, pCurrentAb->AbBox, pRefAb, TRUE, FALSE, horizRef);
	  if (pCurrentAb == pRefAb->AbEnclosing)
	    pCurrentAb = pCurrentAb->AbFirstEnclosed;
	  else
	    pCurrentAb = pCurrentAb->AbNext;
	}
    }
}

/*----------------------------------------------------------------------
   ClearAxisRelation recherche la boite dont depend l'axe de reference
   horizontal (si horizRef est Vrai) sinon vertical de     
   pBox:					
   - Il peut dependre d'elle meme (une relation chez elle).
   - Il peut dependre d'une englobee (une relation chez    
   l'englobee).                                            
   - Il peut dependre d'une voisine (une relation chez la  
   voisine).                                               
   Si cette dependance existe encore, on detruit la        
   relation.                                               
  ----------------------------------------------------------------------*/
void ClearAxisRelation (PtrBox pBox, ThotBool horizRef)
{
   PtrAbstractBox      pAb;
   PtrAbstractBox      pRefAb;
   ThotBool            loop;

   pRefAb = pBox->BxAbstractBox;
   loop = TRUE;
   /* On recherche dans la descendance la dependance de l'axe de reference */
   pAb = pRefAb;
   while (loop && pAb != NULL)
     {
	if (pAb->AbBox != NULL)
	   loop = !RemovePosRelation (pAb->AbBox, pBox, NULL, FALSE, TRUE, horizRef);
	if (pAb == pRefAb)
	   pAb = pAb->AbFirstEnclosed;
	else
	   pAb = pAb->AbNext;
     }

   /* On recherche chez les voisines la dependance de l'axe de reference */
   pAb = pRefAb->AbEnclosing;
   if (pAb != NULL)
     {
	pAb = pAb->AbFirstEnclosed;
	while (loop && pAb != NULL)
	  {
	     if (pAb != pRefAb && pAb->AbBox != NULL)
		loop = !RemovePosRelation (pAb->AbBox, pBox, NULL, FALSE, TRUE, horizRef);
	     pAb = pAb->AbNext;
	  }
     }
}


/*----------------------------------------------------------------------
   ClearAllRelations removes all relations of the removed box pBox.
   Remove all local relations and their reverse relations
   Remove remaining relation in the parent box.
  ----------------------------------------------------------------------*/
void ClearAllRelations (PtrBox pBox, int frame)
{

  PtrAbstractBox      pAb;
  PtrAbstractBox      pRefAb;
  PtrPosRelations     pPosRel;
  BoxRelation        *pRelation;
  int                 i;
  ThotBool            loop;
  ThotBool            notEmpty;

  pRefAb = pBox->BxAbstractBox;
  pPosRel = pBox->BxPosRelations;
  pBox->BxPosRelations = NULL;
  loop = TRUE;
  if (pPosRel != NULL)
    while (loop)
      {
	i = 1;
	notEmpty = pPosRel->PosRTable[i - 1].ReBox != NULL;
	while (i <= MAX_RELAT_POS && notEmpty)
	  {
	    pRelation = &pPosRel->PosRTable[i - 1];
	    /* Remove the reverse relation */
	    if (pRelation->ReOp == OpHorizDep)
	      RemovePosRelation (pRelation->ReBox, pBox, NULL, TRUE, TRUE, TRUE);
	    else if (pRelation->ReOp == OpVertDep)
	      RemovePosRelation (pRelation->ReBox, pBox, NULL, TRUE, TRUE, FALSE);
	    i++;
	    if (i <= MAX_RELAT_POS)
	      notEmpty = pPosRel->PosRTable[i - 1].ReBox != NULL;
	  }
	
	if (pPosRel->PosRNext == NULL)
	  loop = FALSE;
	/* Next block */
	FreePosBlock (&pPosRel);
      }

  /* Remove remaining relations in the enclosing box */
   pAb = pRefAb->AbEnclosing;
   if (pAb != NULL)
     {
	/* vertical */
	RemovePosRelation (pAb->AbBox, pBox, NULL, FALSE, TRUE, FALSE);
	/*horizontal */
	RemovePosRelation (pAb->AbBox, pBox, NULL, FALSE, TRUE, TRUE);
     }

   /* Remove dimension relations */
   ClearDimRelation (pBox, TRUE, frame);
   ClearDimRelation (pBox, FALSE, frame);
}


/*----------------------------------------------------------------------
  ClearDimRelation looks for the box that gives the current box dimension
  It could be  the enclosing, a sibling, or a relation out of structure
  (the relation is stored in the other box).
   Si cette dependance existe encore, on detruit la        
   relation. L'indicateur BtLg(Ht)HorsStruct indique que   
   la relation est hors-structure.                         
  ----------------------------------------------------------------------*/
void ClearDimRelation (PtrBox pBox, ThotBool horizRef, int frame)
{
  PtrAbstractBox      pRefAb;
  PtrAbstractBox      pAb;
  ThotBool            loop;

  if (!pBox)
    return;
  pRefAb = pBox->BxAbstractBox;
  pAb = pRefAb->AbEnclosing;
  loop = FALSE;
  if ((horizRef && pBox->BxWOutOfStruct) ||
      (!horizRef && pBox->BxHOutOfStruct))
    {
      /* relation out of structure */
      if (!IsDead (pRefAb))
	{
	  /* got to the root abstract box */
	  if (pAb)
	    while (pAb->AbEnclosing)
	      pAb = pAb->AbEnclosing;
	  
	  /* look every where the invert relation */
	  loop = TRUE;
	  if (horizRef)
	    {
	      if (pBox->BxHorizFlex)
		{
		  /* strechable box */
		  while (loop && pAb)
		    {
		      if (pAb->AbBox)
			loop = !RemovePosRelation (pAb->AbBox, pBox, NULL, FALSE, FALSE, horizRef);
		      if (pAb)
			pAb = NextAbToCheck (pAb, pRefAb);
		    }
		  
		  /* Il faut retablir le point fixe */
		  pBox->BxHorizEdge = pBox->BxAbstractBox->AbHorizPos.PosEdge;
		  /* La boite n'est pas inversee */
		  pBox->BxHorizInverted = FALSE;
		  /* La dimension n'est plus elastique */
		  pBox->BxHorizFlex = FALSE;
		  /* Annule la largeur de la boite */
		  ResizeWidth (pBox, NULL, NULL, -pBox->BxW, 0, 0, 0, frame);
		}
	      else
		/* not strechable box */
		while (loop && pAb)
		  {
		    if (pAb->AbBox)
		      loop = !RemoveDimRelation (pAb->AbBox, pBox, horizRef);
		    if (pAb)
		      pAb = NextAbToCheck (pAb, pRefAb);
		  }

	      /* not out of structure relations now */
	      pBox->BxWOutOfStruct = FALSE;
	    }
	  else
	    {
	      if (pBox->BxVertFlex)
		{
		  /* strechable box */
		  while (loop && pAb)
		    {
		      if (pAb->AbBox)
			loop = !RemovePosRelation (pAb->AbBox, pBox, NULL, FALSE, FALSE, horizRef);
		      if (pAb)
			pAb = NextAbToCheck (pAb, pRefAb);
		    }
		  
		  /* Il faut retablir le point fixe */
		  pBox->BxVertEdge = pBox->BxAbstractBox->AbVertPos.PosEdge;
		  /* La boite n'est pas inversee */
		  pBox->BxVertInverted = FALSE;
		  /* La dimension n'est plus elastique */
		  pBox->BxVertFlex = FALSE;
		  /* Annule la hauteur de la boite */
		  ResizeHeight (pBox, NULL, NULL, -pBox->BxH, 0, 0, frame);
		}
	      else
		/* not strechable box */
		while (loop && pAb)
		  {
		    if (pAb->AbBox)
		      loop = !RemoveDimRelation (pAb->AbBox, pBox, horizRef);
		    if (pAb)
		      pAb = NextAbToCheck (pAb, pRefAb);
		  }
	    }
	}
    }
  else
    {
      /* Remove a possible relation from the enclosing boxe */
      if (pAb)
	{
	  loop = !RemoveDimRelation (pAb->AbBox, pBox, horizRef);
	  if (loop)
	    pAb = pAb->AbFirstEnclosed;
	}

      /* Remove a pposible relation from sibling boxes */
      while (loop && pAb)
	{
	  if (pAb->AbBox && pAb != pRefAb)
	    {
	      loop = !RemoveDimRelation (pAb->AbBox, pBox, horizRef);
	      if (loop)
		pAb = pAb->AbNext;
	    }
	  else
	    pAb = pAb->AbNext;
	}
      
      /* La dimension est elastique en X ? */
      if (horizRef && pBox->BxHorizFlex)
	{
	  /* La boite n'est pas inversee */
	  pBox->BxHorizInverted = FALSE;
	  /* La dimension n'est plus elastique */
	  pBox->BxHorizFlex = FALSE;
	  /* Il faut retablir le point fixe */
	  pBox->BxHorizEdge = pBox->BxAbstractBox->AbHorizPos.PosEdge;
	  /* Annule la largeur de la boite */
	  ResizeWidth (pBox, NULL, NULL, -pBox->BxW, 0, 0, 0, frame);
	}
      
      /* La dimension est elastique en Y ? */
      if (!horizRef && pBox->BxVertFlex)
	{
	  /* La boite n'est pas inversee */
	  pBox->BxVertInverted = FALSE;
	  /* La dimension n'est plus elastique */
	  pBox->BxVertFlex = FALSE;
	  /* Il faut retablir le point fixe */
	  pBox->BxVertEdge = pBox->BxAbstractBox->AbVertPos.PosEdge;
	  /* Annule la hauteur de la boite */
	  ResizeHeight (pBox, NULL, NULL, -pBox->BxH, 0, 0, frame);
	}
    }

  /* remove a possible dim relation from the root box */
  pAb = pRefAb->AbEnclosing;
  if (pAb)
    {
      while (pAb->AbEnclosing)
	pAb = pAb->AbEnclosing;
      RemoveDimRelation (pAb->AbBox, pBox, horizRef);
    }
}
