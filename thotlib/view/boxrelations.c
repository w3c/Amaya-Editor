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
 * gestion des Relations entre boites
 *
 * Author: I. Vatton (INRIA)
 *
 */

#include "ustring.h"
#include "thot_sys.h"
#include "thot_gui.h"
#include "libmsg.h"
#include "message.h"
#include "constmedia.h"
#include "typemedia.h"
#include "frame.h"

#define THOT_EXPORT extern
#include "boxes_tv.h"

#include "memory_f.h"
#include "appli_f.h"
#include "boxmoves_f.h"
#include "absboxes_f.h"
#include "font_f.h"

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         ClearBoxMoved (PtrBox pBox)
#else  /* __STDC__ */
static void         ClearBoxMoved (pBox)
PtrBox              pBox;
#endif /* __STDC__ */
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
#ifdef __STDC__
static PtrAbstractBox GetPosRelativeAb (PtrAbstractBox pCurrentAb, ThotBool horizRef)
#else  /* __STDC__ */
static PtrAbstractBox GetPosRelativeAb (pCurrentAb, horizRef)
PtrAbstractBox      pCurrentAb;
ThotBool            horizRef;

#endif /* __STDC__ */
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
#ifdef __STDC__
static void         InsertPosRelation (PtrBox pOrginBox, PtrBox pTargetBox, OpRelation op, BoxEdge originEdge, BoxEdge targetEdge)
#else  /* __STDC__ */
static void         InsertPosRelation (pOrginBox, pTargetBox, op, originEdge, targetEdge)
PtrBox              pOrginBox;
PtrBox              pTargetBox;
OpRelation          op;
BoxEdge             originEdge;
BoxEdge             targetEdge;

#endif /* __STDC__ */
{
  int                 i;
  ThotBool            loop;
  ThotBool            empty;
  PtrPosRelations     pPreviousPosRel;
  PtrPosRelations     pPosRel;
  BoxRelation        *pRelation;

  i = 0;
  /* PcFirst sens de la dependance dans les positionnements entre soeurs */
  if (op != OpHorizRef && op != OpVertRef && op != OpWidth && op != OpHeight)
    {
      /* On recherche une entree libre */
      pPosRel = pOrginBox->BxPosRelations;
      pPreviousPosRel = NULL;
      loop = TRUE;
      while (loop && pPosRel != NULL)
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
	    pPosRel = pPosRel->PosRNext;
	  /* Bloc suivant */
	}

      /* Faut-il creer un nouveau bloc de relations ? */
      if (loop)
	{
	  GetPosBlock (&pPosRel);
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

  /* Deuxieme sens de la dependance */
  if (op != OpHorizInc && op != OpVertInc)
    {
      /* On recherche une entree libre */
      pPosRel = pTargetBox->BxPosRelations;
      pPreviousPosRel = NULL;
      loop = TRUE;
      while (loop && pPosRel != NULL)
	{
	  i = 0;
	  pPreviousPosRel = pPosRel;
	  do
	    {
	      empty = pPosRel->PosRTable[i].ReBox == NULL;
	      i++;
	    }
	  while (i != MAX_RELAT_POS && !empty);
	  
	  if (empty)
	    {
	      loop = FALSE;
	      i--;
	    }
	  else
	    /* Bloc suivant */
	    pPosRel = pPosRel->PosRNext;
	}

      /* Faut-il creer un nouveau bloc de relations ? */
      if (loop)
	{
	  GetPosBlock (&pPosRel);
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
   InsertDimRelation etablit le lien entre les dimensions             
   horizontales ou verticales des deux boites (pOrginBox vers 
   pTargetBox).
   Si sameDimension est Faux, il faut inverser horizRef.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         InsertDimRelation (PtrBox pOrginBox, PtrBox pTargetBox, ThotBool sameDimension, ThotBool horizRef)
#else  /* __STDC__ */
static void         InsertDimRelation (pOrginBox, pTargetBox, sameDimension, horizRef)
PtrBox              pOrginBox;
PtrBox              pTargetBox;
ThotBool            sameDimension;
ThotBool            horizRef;
#endif /* __STDC__ */
{
  PtrDimRelations     pPreviousDimRel;
  PtrDimRelations     pDimRel;
  int                 i;
  ThotBool            loop;
  ThotBool            empty;

  if (!sameDimension)
    horizRef = !horizRef;

  i = 0;
  /* On determine la dimension affectee */
  if (horizRef)
    if (sameDimension)
      pDimRel = pOrginBox->BxWidthRelations;
    else
      pDimRel = pOrginBox->BxHeightRelations;
  else if (sameDimension)
    pDimRel = pOrginBox->BxHeightRelations;
  else
    pDimRel = pOrginBox->BxWidthRelations;

  /* On recherche une entree libre */
  pPreviousDimRel = NULL;
  loop = TRUE;
  while (loop && pDimRel != NULL)
    {
      i = 0;
      pPreviousDimRel = pDimRel;
      do
	{
	  empty = pDimRel->DimRTable[i] == NULL;
	  i++;
	}
      while (i != MAX_RELAT_DIM && !empty);

      if (empty)
	{
	  loop = FALSE;
	  i--;
	}
      else
	pDimRel = pDimRel->DimRNext;
    }

  /* Faut-il creer un nouveau bloc de relations ? */
  if (loop)
    {
      GetDimBlock (&pDimRel);
      if (pPreviousDimRel == NULL)
	if (horizRef)
	  pOrginBox->BxWidthRelations = pDimRel;
	else
	  pOrginBox->BxHeightRelations = pDimRel;
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
#ifdef __STDC__
static PtrAbstractBox NextAbToCheck (PtrAbstractBox pAb, PtrAbstractBox pRefAb)
#else  /* __STDC__ */
static PtrAbstractBox NextAbToCheck (pAb, pRefAb)
PtrAbstractBox      pAb;
PtrAbstractBox      pRefAb;

#endif /* __STDC__ */
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
#ifdef __STDC__
static void         PropagateXOutOfStruct (PtrAbstractBox pCurrentAb, ThotBool status, ThotBool enclosed)
#else  /* __STDC__ */
static void         PropagateXOutOfStruct (pCurrentAb, status, enclosed)
PtrAbstractBox      pCurrentAb;
ThotBool            status;
ThotBool            enclosed;

#endif /* __STDC__ */
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
	if (pAb == pCurrentAb || pBox == NULL || pAb->AbDead)
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
	else if (pAb->AbWidth.DimIsPosition
		 && pAb->AbWidth.DimPosition.PosAbRef == pCurrentAb && !pAb->AbWidthChange)
	   /* Dependance de dimension */
	   pBox->BxWOutOfStruct = status;

	/* passe au pave suivant */
	pAb = pAb->AbNext;
     }
}


/*----------------------------------------------------------------------
   PropagateYOutOfStruct propage l'indicateur hors-structure.         
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         PropagateYOutOfStruct (PtrAbstractBox pCurrentAb, ThotBool status, ThotBool enclosed)
#else  /* __STDC__ */
static void         PropagateYOutOfStruct (pCurrentAb, status, enclosed)
PtrAbstractBox      pCurrentAb;
ThotBool            status;
ThotBool            enclosed;

#endif /* __STDC__ */
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
	if (pAb == pCurrentAb || pBox == NULL || pAb->AbDead)
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
#ifdef __STDC__
void                SetPositionConstraint (BoxEdge localEdge, PtrBox pBox, int *val)
#else  /* __STDC__ */
void                SetPositionConstraint (localEdge, pBox, val)
BoxEdge             localEdge;
PtrBox              pBox;
int                *val;

#endif /* __STDC__ */
{
  AbPosition         *pPosAb;

   /* Calcule l'origine de la boite et les points fixes */
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
	 if (pPosAb->PosRefEdge == VertMiddle)
	   pBox->BxHorizEdge = VertMiddle;
	 else if (pPosAb->PosRefEdge == Right)
	   pBox->BxHorizEdge = Right;
	 else
	   pBox->BxHorizEdge = Left;
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
	 if (pPosAb->PosRefEdge == HorizMiddle)
	   pBox->BxVertEdge = HorizMiddle;
	 else if (pPosAb->PosRefEdge == Bottom)
	   pBox->BxVertEdge = Bottom;
	 else
	   pBox->BxVertEdge = Top;
       else
	 pBox->BxVertEdge = HorizRef;
       break;
     default:
       break;
     }
}

/*----------------------------------------------------------------------
  ComputeMarginPaddinAndBorder applies margin, padding, and border rules.
  Relation between values:
  <-LMargin-><-LBorder-><-LPadding-><-W-><-RPadding-><-RBorder-><-LRargin->
  <---------------------------------Width--------------------------------->
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ComputeMPB (PtrAbstractBox pAb, int frame, ThotBool horizRef)
#else  /* __STDC__ */
void                ComputeMPB (pAb, frame, horizRef)
PrtAbstractBox      pAb;
int                 frame;
ThotBool            horizRef;
#endif /* __STDC__ */
{
  PtrBox              pBox;
  int                 dim, i;

  if (horizRef)
    {
      /* reference for computing percent rules */
      if (pAb->AbEnclosing)
	{
	  pBox = pAb->AbEnclosing->AbBox;
	  dim = pBox->BxW;
	}
      else
	GetSizesFrame (frame, &dim, &i);

      /* left margin */
      if (pAb->AbLeftMarginUnit == UnPercent)
	pAb->AbBox->BxLMargin = PixelValue (pAb->AbLeftMargin, UnPercent, (PtrAbstractBox) dim, 0);
      else if (pAb->AbLeftMarginUnit != UnAuto)
	pAb->AbBox->BxLMargin = PixelValue (pAb->AbLeftMargin, pAb->AbLeftMarginUnit, pAb, ViewFrameTable[frame - 1].FrMagnification);
      /* right margin */
      if (pAb->AbRightMarginUnit == UnPercent)
	pAb->AbBox->BxRMargin = PixelValue (pAb->AbRightMargin, UnPercent, (PtrAbstractBox) dim, 0);
      else if (pAb->AbRightMarginUnit != UnAuto)
	pAb->AbBox->BxRMargin = PixelValue (pAb->AbRightMargin, pAb->AbRightMarginUnit, pAb, ViewFrameTable[frame - 1].FrMagnification);

      /* left padding */
      if (pAb->AbLeftPaddingUnit == UnPercent)
	pAb->AbBox->BxLPadding = PixelValue (pAb->AbLeftPadding, UnPercent, (PtrAbstractBox) dim, 0);
      else if (pAb->AbLeftMarginUnit != UnAuto)
	pAb->AbBox->BxLPadding = PixelValue (pAb->AbLeftPadding, pAb->AbLeftPaddingUnit, pAb, ViewFrameTable[frame - 1].FrMagnification);
      /* right padding */
      if (pAb->AbRightPaddingUnit == UnPercent)
	pAb->AbBox->BxRPadding = PixelValue (pAb->AbRightPadding, UnPercent, (PtrAbstractBox) dim, 0);
      else
	pAb->AbBox->BxRPadding = PixelValue (pAb->AbRightPadding, pAb->AbRightPaddingUnit, pAb, ViewFrameTable[frame - 1].FrMagnification);

      /* left border */
      if (pAb->AbLeftBorderUnit == UnPercent)
	pAb->AbBox->BxLBorder = PixelValue (pAb->AbLeftBorder, UnPercent, (PtrAbstractBox) dim, 0);
      else
	pAb->AbBox->BxLBorder = PixelValue (pAb->AbLeftBorder, pAb->AbLeftBorderUnit, pAb, ViewFrameTable[frame - 1].FrMagnification);
      /* right border */
      if (pAb->AbRightBorderUnit == UnPercent)
	pAb->AbBox->BxRBorder = PixelValue (pAb->AbRightBorder, UnPercent, (PtrAbstractBox) dim, 0);
      else
	pAb->AbBox->BxRBorder = PixelValue (pAb->AbRightBorder, pAb->AbRightBorderUnit, pAb, ViewFrameTable[frame - 1].FrMagnification);

      /* Manage auto margins */
      if (pAb->AbLeftMarginUnit == UnAuto && pAb->AbRightMarginUnit == UnAuto)
	{
	  pBox = pAb->AbBox;
	  pAb->AbBox->BxLMargin = (dim - pBox->BxLPadding - pBox->BxRPadding - pBox->BxLBorder - pBox->BxRBorder) / 2;
	  pAb->AbBox->BxRMargin = pAb->AbBox->BxLMargin;
	}
      else if (pAb->AbLeftMarginUnit == UnAuto)
	{
	  pBox = pAb->AbBox;
	  pAb->AbBox->BxLMargin = dim - pBox->BxRMargin - pBox->BxLPadding - pBox->BxRPadding - pBox->BxLBorder - pBox->BxRBorder;
	}
      else if (pAb->AbRightMarginUnit == UnAuto)
	{
	  pBox = pAb->AbBox;
	  pAb->AbBox->BxRMargin = dim - pBox->BxLMargin - pBox->BxLPadding - pBox->BxRPadding - pBox->BxLBorder - pBox->BxRBorder;
	}
    }
  else
    {
      /* reference for computing percent rules */
      if (pAb->AbEnclosing)
	{
	  pBox = pAb->AbEnclosing->AbBox;
	  dim = pBox->BxH;
	}
      else
	GetSizesFrame (frame, &i, &dim);

      /* top margin */
      if (pAb->AbTopMarginUnit == UnPercent)
	pAb->AbBox->BxTMargin = PixelValue (pAb->AbTopMargin, UnPercent, (PtrAbstractBox) dim, 0);
      else if (pAb->AbTopMarginUnit != UnAuto)
	pAb->AbBox->BxTMargin = PixelValue (pAb->AbTopMargin, pAb->AbTopMarginUnit, pAb, ViewFrameTable[frame - 1].FrMagnification);
      /* bottom margin */
      if (pAb->AbBottomMarginUnit == UnPercent)
	pAb->AbBox->BxBMargin = PixelValue (pAb->AbBottomMargin, UnPercent, (PtrAbstractBox) dim, 0);
      else if (pAb->AbBottomMarginUnit != UnAuto)
	pAb->AbBox->BxBMargin = PixelValue (pAb->AbBottomMargin, pAb->AbBottomMarginUnit, pAb, ViewFrameTable[frame - 1].FrMagnification);

      /* top padding */
      if (pAb->AbTopPaddingUnit == UnPercent)
	pAb->AbBox->BxTPadding = PixelValue (pAb->AbTopPadding, UnPercent, (PtrAbstractBox) dim, 0);
      else
	pAb->AbBox->BxTPadding = PixelValue (pAb->AbTopPadding, pAb->AbTopPaddingUnit, pAb, ViewFrameTable[frame - 1].FrMagnification);
      /* bottom padding */
      if (pAb->AbBottomPaddingUnit == UnPercent)
	pAb->AbBox->BxBPadding = PixelValue (pAb->AbBottomPadding, UnPercent, (PtrAbstractBox) dim, 0);
      else
	pAb->AbBox->BxBPadding = PixelValue (pAb->AbBottomPadding, pAb->AbBottomPaddingUnit, pAb, ViewFrameTable[frame - 1].FrMagnification);

      /* top border */
      if (pAb->AbTopBorderUnit == UnPercent)
	pAb->AbBox->BxTBorder = PixelValue (pAb->AbTopBorder, UnPercent, (PtrAbstractBox) dim, 0);
      else
	pAb->AbBox->BxTBorder = PixelValue (pAb->AbTopBorder, pAb->AbTopBorderUnit, pAb, ViewFrameTable[frame - 1].FrMagnification);
      /* bottom border */
      if (pAb->AbBottomBorderUnit == UnPercent)
	pAb->AbBox->BxBBorder = PixelValue (pAb->AbBottomBorder, UnPercent, (PtrAbstractBox) dim, 0);
      else
	pAb->AbBox->BxBBorder = PixelValue (pAb->AbBottomBorder, pAb->AbBottomBorderUnit, pAb, ViewFrameTable[frame - 1].FrMagnification);

      /* Manage auto margins */
      if (pAb->AbTopMarginUnit == UnAuto && pAb->AbBottomMarginUnit == UnAuto)
	{
	  pBox = pAb->AbBox;
	  pAb->AbBox->BxTMargin = (dim - pBox->BxTPadding - pBox->BxBPadding - pBox->BxTBorder - pBox->BxBBorder) / 2;
	  pAb->AbBox->BxBMargin = pAb->AbBox->BxTMargin;
	}
      else if (pAb->AbTopMarginUnit == UnAuto)
	{
	  pBox = pAb->AbBox;
	  pAb->AbBox->BxTMargin = dim - pBox->BxBMargin - pBox->BxTPadding - pBox->BxBPadding - pBox->BxTBorder - pBox->BxBBorder;
	}
      else if (pAb->AbBottomMarginUnit == UnAuto)
	{
	  pBox = pAb->AbBox;
	  pAb->AbBox->BxBMargin = dim - pBox->BxTMargin - pBox->BxTPadding - pBox->BxBPadding - pBox->BxTBorder - pBox->BxBBorder;
	}
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
#ifdef __STDC__
void                ComputePosRelation (AbPosition rule, PtrBox pBox, int frame, ThotBool horizRef)
#else  /* __STDC__ */
void                ComputePosRelation (rule, pBox, frame, horizRef)
AbPosition          rule;
PtrBox              pBox;
int                 frame;
ThotBool            horizRef;
#endif /* __STDC__ */
{
  PtrAbstractBox      pRefAb;
  PtrAbstractBox      pAb;
  PtrBox              pRefBox;
  BoxEdge             refEdge, localEdge;
  OpRelation          op;
  int                 x, y, dist, dim;
  int                 t, l, b, r;
  ThotBool            sibling = FALSE;
  
  /* On calcule la position de reference */
  op = (OpRelation) 0;
  refEdge = (BoxEdge) 0;
  GetSizesFrame (frame, &x, &y);
  pRefBox = NULL;
  pRefAb = rule.PosAbRef;
  pAb = pBox->BxAbstractBox;
  if (pRefAb != NULL && pRefAb->AbDead)
    {
      fprintf (stderr, "Position refers a dead box");
      pRefAb = NULL;
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
		dist = PixelValue (rule.PosDistance, rule.PosUnit, pAb, ViewFrameTable[frame - 1].FrMagnification);
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
		    dist = PixelValue (rule.PosDistance, UnPercent, (PtrAbstractBox) pAb->AbEnclosing, 0);
		  else
		    dist = 0;
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
	      else if (pRefAb->AbBox != NULL)
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
	  if (rule.PosUnit == UnPercent)
	    {
	      dim = pAb->AbEnclosing->AbBox->BxW;
	      dist = PixelValue (rule.PosDistance, UnPercent, (PtrAbstractBox) dim, 0);
	      /* Change the rule for further updates */
	      pAb->AbHorizPos.PosDistance = dist;
	      pAb->AbHorizPos.PosUnit = UnPixel;
	    }
	  else
	    dist = PixelValue (rule.PosDistance, rule.PosUnit, pAb, ViewFrameTable[frame - 1].FrMagnification);
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
		dist = PixelValue (rule.PosDistance, rule.PosUnit, pAb, ViewFrameTable[frame - 1].FrMagnification);
	    }
	  else
	    {
	      /* there is an enclosing box */
	      pRefAb = GetPosRelativeAb (pAb, horizRef);
	      if (pRefAb != NULL)
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
	  if (pAb->AbEnclosing == pRefAb && pBox->BxVertFlex && pBox->BxType == BoCell)
	    if (!pBox->BxVertInverted &&
	      (rule.PosRefEdge != Top || rule.PosEdge != Top))
	      {
		/* a specific patch for vertically extended cells */
		pAb->AbVertPos.PosRefEdge = Top;
		pAb->AbVertPos.PosEdge = Top;
		rule.PosRefEdge = Top;
		rule.PosEdge = Top;
	      }
	    else if (pBox->BxVertInverted &&
	      (rule.PosRefEdge != Top || rule.PosEdge != Bottom))
	      {
		/* a specific patch for vertacally extended cells */
		pAb->AbVertPos.PosRefEdge = Bottom;
		pAb->AbVertPos.PosEdge = Bottom;
		pAb->AbVertPos.PosDistance = 0;
		rule.PosRefEdge = Top;
		rule.PosEdge = Bottom;
		rule.PosDistance = 0;
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
	      else if (pRefAb->AbBox != NULL)
		{
		  /* depend on a sibling box */
		  sibling = TRUE;
		  if (pRefAb->AbBox->BxYOutOfStruct
		      || (pRefAb->AbBox->BxHorizFlex && pRefAb->AbLeafType == LtCompound && pRefAb->AbInLine && refEdge != Top)
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
	  if (rule.PosUnit == UnPercent)
	    {
	      dim = pAb->AbEnclosing->AbBox->BxH;
	      dist = PixelValue (rule.PosDistance, UnPercent, (PtrAbstractBox) dim, 0);
	      /* Change the rule for further updates */
	      pAb->AbVertPos.PosDistance = dist;
	      pAb->AbVertPos.PosUnit = UnPixel;
	    }
	  else
	    dist = PixelValue (rule.PosDistance, rule.PosUnit, pAb, ViewFrameTable[frame - 1].FrMagnification);
	}
    }

  if (pRefAb != NULL)
    {
      /* Depend on another box */
      pRefBox = pRefAb->AbBox;
      if (pRefBox == NULL)
	{
	  /* the referred box doesn't already exist */
	  pRefBox = GetBox (pRefAb);
	  if (pRefBox != NULL)
	    pRefAb->AbBox = pRefBox;
	  else
	    /* memory allocation trouble */
	    return;
	}
      
      x = pRefBox->BxXOrg;
      y = pRefBox->BxYOrg;
      if (pRefAb == pAb->AbEnclosing)
	{
	  t = pRefBox->BxTMargin + pRefBox->BxTBorder + pRefBox->BxTPadding;
	  l = pRefBox->BxLMargin + pRefBox->BxLBorder + pRefBox->BxLPadding;
	  b = pRefBox->BxBMargin + pRefBox->BxBBorder + pRefBox->BxBPadding;
	  r = pRefBox->BxRMargin + pRefBox->BxRBorder + pRefBox->BxRPadding;
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
	  y += t;
	  if (sibling && localEdge == Bottom)
	    {
	      /* only take the larger margin into account */
	      if (pBox->BxBMargin < pRefBox->BxTMargin)
		y -= pBox->BxBMargin;
	      else
		y -= pRefBox->BxTMargin;
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
	  y = y + pRefBox->BxHeight - b ;
	  if (sibling && localEdge == Top)
	    {
	      /* only take the larger margin into account */
	      if (pBox->BxTMargin < pRefBox->BxBMargin)
		y -= pBox->BxTMargin;
	      else
		y -= pRefBox->BxBMargin;
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
      SetPositionConstraint (localEdge, pBox, &y);
    }

   /* regarde si la position depend d'une boite invisible */
   if (pBox->BxAbstractBox->AbVisibility < ViewFrameTable[frame - 1].FrVisibility)
     dist = 0;
   else if (pRefAb != NULL)
     if (pRefAb->AbVisibility < ViewFrameTable[frame - 1].FrVisibility)
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
   GetHPosRelativePos search the box which horizontally links   
   pBox to is enclosing:                                
   - If relations were not updated, that box is BxHorizInc
   - If the box has the relation OpHorizInc, it's itself
   - Else get the sibling box which gives the position.     
   Return a box or NULL.                        
  ----------------------------------------------------------------------*/
#ifdef __STDC__
PtrBox              GetHPosRelativePos (PtrBox pBox, PtrBox pPreviousBox)
#else  /* __STDC__ */
PtrBox              GetHPosRelativePos (pBox, pPreviousBox)
PtrBox              pBox;
PtrBox              pPreviousBox;

#endif /* __STDC__ */
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
		    pRelativeBox = GetHPosRelativePos (pRelation->ReBox, pBox);
		  
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
   - If the box has the relation OpHorizInc, it's itself
   - Else get the sibling box which gives the position.     
   Return a box or NULL.                        
  ----------------------------------------------------------------------*/
#ifdef __STDC__
PtrBox              GetVPosRelativeBox (PtrBox pBox, PtrBox pPreviousBox)
#else  /* __STDC__ */
PtrBox              GetVPosRelativeBox (pBox, pPreviousBox)
PtrBox              pBox;
PtrBox              pPreviousBox;

#endif /* __STDC__ */
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
  There are 4 different case:
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
#ifdef __STDC__
ThotBool            ComputeDimRelation (PtrAbstractBox pAb, int frame, ThotBool horizRef)
#else  /* __STDC__ */
ThotBool            ComputeDimRelation (pAb, frame, horizRef)
PtrAbstractBox      pAb;
int                 frame;
ThotBool            horizRef;

#endif /* __STDC__ */
{
  PtrBox              pRefBox;
  PtrBox              pBox;
  PtrAbstractBox      pParentAb;
  PtrAbstractBox      pChildAb, pAncestor;
  OpRelation          op;
  AbDimension        *pDimAb;
  AbPosition         *pPosAb;
  int                 val, delta, i;
  ThotBool            inLine;
  ThotBool            defaultDim;

  pBox = pAb->AbBox;
  /* On verifie que la boite est visible */
  if (pAb->AbVisibility >= ViewFrameTable[frame - 1].FrVisibility)
    {
      pParentAb = pAb->AbEnclosing;
      /* Les cas de coherence sur les boites elastiques */
      /* Les reperes Position et Dimension doivent etre differents */
      /* Ces reperes ne peuvent pas etre l'axe de reference        */
      if (horizRef && pAb->AbWidth.DimIsPosition)
	{
	  if (pAb->AbHorizPos.PosEdge == pAb->AbWidth.DimPosition.PosEdge
	      || pAb->AbHorizPos.PosEdge == VertMiddle
	      || pAb->AbHorizPos.PosEdge == VertRef)
	    {
	      /* Erreur sur le schema de presentation */
	      if (pAb->AbWidth.DimPosition.PosEdge == Left)
		pAb->AbHorizPos.PosEdge = Right;
	      else if (pAb->AbWidth.DimPosition.PosEdge == Right)
		pAb->AbHorizPos.PosEdge = Left;
	    }
	  else if (pAb->AbHorizPos.PosAbRef == NULL
		   && (pAb->AbWidth.DimPosition.PosAbRef == NULL ||
		       pAb->AbWidth.DimPosition.PosAbRef == pParentAb))
	    {
	      /* prend la dimension de l'englobant */
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
	      /* Il y a une erreur de dimension */
	      /* Erreur sur le schema de presentation */
	      fprintf (stderr, "Bad Width rule on %s\n", AbsBoxType (pAb, TRUE));
	      pAb->AbWidth.DimIsPosition = FALSE;
	      pAb->AbWidth.DimAbRef = NULL;
	      pAb->AbWidth.DimValue = 20;	/* largeur fixe */
	      pAb->AbWidth.DimUnit = UnPoint;
	      pAb->AbWidth.DimUserSpecified = FALSE;
	    }
	  /* verifie que la dimension ne depend pas d'un pave mort */
	  else if (pAb->AbHorizPos.PosAbRef->AbDead)
	    {
	      fprintf (stderr, "Dimension refers a dead box");
	      pAb->AbWidth.DimIsPosition = FALSE;
	      pAb->AbWidth.DimAbRef = NULL;
	      pAb->AbWidth.DimValue = 20;	/* largeur fixe */
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
	      /* Erreur sur le schema de presentation */
	      if (pAb->AbHeight.DimPosition.PosEdge == Top)
		pAb->AbVertPos.PosEdge = Bottom;
	      else if (pAb->AbHeight.DimPosition.PosEdge == Bottom)
		pAb->AbVertPos.PosEdge = Top;
	    }
	  else if (pAb->AbVertPos.PosAbRef == NULL
		   && (pAb->AbHeight.DimPosition.PosAbRef == NULL ||
		       pAb->AbHeight.DimPosition.PosAbRef == pParentAb))
	    {
	      /* prend la dimension de l'englobant */
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
	      /* Il y a une erreur de dimension */
	      /* Erreur sur le schema de presentation */
	      fprintf (stderr, "Bad Height rule on %s\n", AbsBoxType (pAb, TRUE));
	      pAb->AbHeight.DimIsPosition = FALSE;
	      pAb->AbHeight.DimAbRef = NULL;
	      pAb->AbHeight.DimValue = 20;	/* hauteur fixe */
	      pAb->AbHeight.DimUnit = UnPoint;
	      pAb->AbHeight.DimUserSpecified = FALSE;
	    }
	  /* verifie que la dimension ne depend pas d'un pave mort */
	  else if (pAb->AbVertPos.PosAbRef->AbDead)
	    {
	      fprintf (stderr, "Dimension refers a dead box");
	      pAb->AbHeight.DimIsPosition = FALSE;
	      pAb->AbHeight.DimAbRef = NULL;
	      pAb->AbHeight.DimValue = 0;
	      pAb->AbHeight.DimUnit = UnPoint;
	      pAb->AbHeight.DimUserSpecified = FALSE;
	    }
	}

      /* Is it a stretchable box? */
      if ((horizRef && !pAb->AbWidth.DimIsPosition) || (!horizRef && !pAb->AbHeight.DimIsPosition))
	{
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

	  /* check if the relative box is not already dead */
	  if (pDimAb->DimAbRef != NULL && pDimAb->DimAbRef->AbDead)
	    {
	      fprintf (stderr, "Dimension refers a dead box");
	      pDimAb->DimAbRef = NULL;
	      pDimAb->DimValue = 0;
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
	  
	  if (pParentAb == NULL)
	    /* It's the root box */
	    if (horizRef)
	      {
		if (pDimAb->DimValue == 0)
		  /* inherited from the contents */
		  pBox->BxContentWidth = TRUE;
		else
		  {
		    /* inherited from the window */
		    GetSizesFrame (frame, &val, &i);
		    if (pDimAb->DimValue < 0)
		      val += pDimAb->DimValue;
		    else if (pDimAb->DimUnit == UnPercent)
		      val = PixelValue (pDimAb->DimValue, UnPercent, (PtrAbstractBox) val, 0);
		    else
		      val = PixelValue (pDimAb->DimValue, pDimAb->DimUnit, pAb, ViewFrameTable[frame - 1].FrMagnification);
		    if (pDimAb->DimValue < 0 || pDimAb->DimUnit == UnPercent)
		      /* the rule gives the outside value */
		      val = val - pBox->BxLMargin - pBox->BxLBorder - pBox->BxLPadding - pBox->BxRMargin - pBox->BxRBorder - pBox->BxRPadding;
		    ResizeWidth (pBox, pBox, NULL, val - pBox->BxW, 0, 0, 0, frame);
		  }
	      }
	    else
	      {
		if (pDimAb->DimValue == 0)
		  /* inherited from the contents */
		  pBox->BxContentHeight = TRUE;
		else
		  {
		    /* inherited from the window */
		    GetSizesFrame (frame, &i, &val);
		    if (pDimAb->DimValue < 0)
		      val += pDimAb->DimValue;
		    else if (pDimAb->DimUnit == UnPercent)
		      val = PixelValue (pDimAb->DimValue, UnPercent, (PtrAbstractBox) val, 0);
		    else
		      /* explicit value */
		      val = PixelValue (pDimAb->DimValue, pDimAb->DimUnit, pAb, ViewFrameTable[frame - 1].FrMagnification);
		    if (pDimAb->DimValue < 0 || pDimAb->DimUnit == UnPercent)
		      /* the rule gives the outside value */
		      val = val - pBox->BxTMargin - pBox->BxTBorder - pBox->BxTPadding - pBox->BxBMargin - pBox->BxBBorder - pBox->BxBPadding;
		    ResizeHeight (pBox, pBox, NULL, val - pBox->BxH, 0, 0, frame);
		  }
	      }
	  else
	    {
	      /* it's not the root box */
	      inLine = pParentAb->AbInLine || pParentAb->AbBox->BxType == BoGhost;
	      if (horizRef)
		if (inLine && pAb->AbLeafType == LtText)
		  /* inherited from the contents */
		  pBox->BxContentWidth = TRUE;
		else if (pAb->AbWidth.DimAbRef == NULL)
		  if (pAb->AbWidth.DimValue <= 0)
		    /* inherited from the contents */
		    pBox->BxContentWidth = TRUE;
		  else
		    {
		      if (pDimAb->DimUnit == UnPercent)
			{
			  /* inherited from the parent */
			  val = PixelValue (pDimAb->DimValue, UnPercent, (PtrAbstractBox) pParentAb->AbBox->BxW, 0);
			  /* the rule gives the outside value */
			  val = val - pBox->BxLMargin - pBox->BxLBorder - pBox->BxLPadding - pBox->BxRMargin - pBox->BxRBorder - pBox->BxRPadding;
			  InsertDimRelation (pParentAb->AbBox, pBox, pDimAb->DimSameDimension, horizRef);
			}
		      else
			/* explicit value */
			val = PixelValue (pDimAb->DimValue, pDimAb->DimUnit, pAb, ViewFrameTable[frame - 1].FrMagnification);
		      ResizeWidth (pBox, pBox, NULL, val - pBox->BxW, 0, 0, 0, frame);
		    }
		else
		  {
		    pPosAb = &pAb->AbHorizPos;
		    pDimAb = &pAb->AbWidth;
		    if (pDimAb->DimAbRef == pParentAb &&
			pParentAb->AbWidth.DimAbRef == NULL &&
			pParentAb->AbWidth.DimValue <= 0 &&
			(inLine || pPosAb->PosAbRef != pParentAb ||
			 pPosAb->PosRefEdge != Left ||
			 pPosAb->PosEdge != Left))
		      {
			/* look for the right ancestor */
			pAncestor = pParentAb->AbEnclosing;
			while (pAncestor != NULL &&
			       ((pAncestor->AbWidth.DimAbRef == NULL && pAncestor->AbWidth.DimValue <= 0) ||
				pAncestor->AbInLine ||
				pAncestor->AbBox->BxType == BoGhost))
			  pAncestor = pAncestor->AbEnclosing;
			if (pAncestor == NULL)
			  {
			    /* inherited from the contents */
			    pBox->BxContentWidth = TRUE;
			    pDimAb->DimAbRef = NULL;
			    pDimAb->DimValue = 0;
			    pDimAb->DimUnit = UnRelative;
			  }
			else
			  pDimAb->DimAbRef = pAncestor;
		      }
		    else if (pDimAb->DimAbRef == pAb && pDimAb->DimSameDimension)
		      {
			/* The dimension cannot depend on itself */
			/* inherited from the contents */
			pBox->BxContentWidth = TRUE;
			pDimAb->DimAbRef = NULL;
			pDimAb->DimValue = 0;
			pDimAb->DimUnit = UnRelative;
		      }

		    if (pDimAb->DimAbRef != NULL)
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
				  val = pRefBox->BxW;
				else
				  val = pRefBox->BxH;

				if (inLine && pDimAb->DimSameDimension)
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
				/* same dimension? */
				if (pDimAb->DimSameDimension)
				  val = pRefBox->BxWidth;
				else
				  val = pRefBox->BxHeight;
			      }

			    /* Convert the distance value */
			    if (pDimAb->DimUnit == UnPercent)
			      val = PixelValue (pDimAb->DimValue, UnPercent, (PtrAbstractBox) val, 0);
			    else
			      val += PixelValue (pDimAb->DimValue, pDimAb->DimUnit, pAb, ViewFrameTable[frame - 1].FrMagnification);
			    /* the rule gives the outside value */
			    val = val - pBox->BxLMargin - pBox->BxLBorder - pBox->BxLPadding - pBox->BxRMargin - pBox->BxRBorder - pBox->BxRPadding;
			    ResizeWidth (pBox, pBox, NULL, val - pBox->BxW, 0, 0, 0, frame);
			    /* Marks out of structure relations */
			    if (pDimAb->DimAbRef != pParentAb
				&& pDimAb->DimAbRef->AbEnclosing != pParentAb)
			      pBox->BxWOutOfStruct = TRUE;
			    
			    /* Store dependencies */
			    InsertDimRelation (pDimAb->DimAbRef->AbBox, pBox, pDimAb->DimSameDimension, horizRef);
			  }
		      }
		  }
	      else
		{
		  pDimAb = &pAb->AbHeight;
		  if (inLine && pAb->AbLeafType == LtText)
		    /* inherited from the contents */
		    pBox->BxContentHeight = TRUE;
		  else if (pDimAb->DimAbRef == NULL)
		    {
		      if (pDimAb->DimValue == 0)
			/* inherited from the contents */
			pBox->BxContentHeight = TRUE;
		      else
			{
			  if (pDimAb->DimUnit == UnPercent)
			    {
			      /* inherited from the parent */
			      val = PixelValue (pDimAb->DimValue, UnPercent, (PtrAbstractBox) pParentAb->AbBox->BxH, 0);
			      /* the rule gives the outside value */
			      val = val - pBox->BxTMargin - pBox->BxTBorder - pBox->BxTPadding - pBox->BxBMargin - pBox->BxBBorder - pBox->BxBPadding;
			      InsertDimRelation (pParentAb->AbBox, pBox, pDimAb->DimSameDimension, horizRef);
			    }
			  else
			    /* explicit value */
			    val = PixelValue (pDimAb->DimValue, pDimAb->DimUnit, pAb, ViewFrameTable[frame - 1].FrMagnification);
			  ResizeHeight (pBox, pBox, NULL, val - pBox->BxH, 0, 0, frame);
			}
		    }
		  else if (inLine && pDimAb->DimAbRef == pParentAb
			   && (pAb->AbLeafType == LtPicture || pAb->AbLeafType == LtCompound))
		    {
		      /* inherited from the contents */
		      pBox->BxContentHeight = TRUE;
		      pDimAb->DimAbRef = NULL;
		      pDimAb->DimValue = 0;
		      pDimAb->DimUnit = UnRelative;
		    }
		  else
		    {
		      pPosAb = &pAb->AbVertPos;
		      if (pDimAb->DimAbRef == pParentAb
			  && pParentAb->AbEnclosing != NULL
			  && pParentAb->AbHeight.DimAbRef == NULL && pParentAb->AbHeight.DimValue <= 0
			  && (pPosAb->PosAbRef != pParentAb || pPosAb->PosRefEdge != Top
			      || pPosAb->PosEdge != Top))
			{
			  /* inherited from the contents */
			  pBox->BxContentHeight = TRUE;
			  pDimAb->DimAbRef = NULL;
			  pDimAb->DimValue = 0;
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
			      pDimAb->DimValue = 0;
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
				  val = val - pBox->BxTMargin - pBox->BxTBorder - pBox->BxTPadding - pBox->BxBMargin - pBox->BxBBorder - pBox->BxBPadding;
				  ResizeHeight (pBox, pBox, NULL, val - pBox->BxH, 0, 0, frame);
				  /* Marks out of structure relations */
				  if (pDimAb->DimAbRef != pParentAb
				      && pDimAb->DimAbRef->AbEnclosing != pParentAb)
				    pBox->BxHOutOfStruct = TRUE;
				  
				  /* Store dependencies */
				  if (pDimAb->DimAbRef == pAb && pDimAb->DimSameDimension)
				    {
				      /* Check errors */
				      if (horizRef)
					fprintf (stderr, "Bad HorizPos rule on %s\n", AbsBoxType (pAb, TRUE));
				      else
					fprintf (stderr, "Bad VertPos rule on %s\n", AbsBoxType (pAb, TRUE));
				    }
				  InsertDimRelation (pDimAb->DimAbRef->AbBox, pBox, pDimAb->DimSameDimension, horizRef);
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
	      if (!pPosAb->PosAbRef->AbDead)
		pRefBox = GetBox (pPosAb->PosAbRef);
	      if (pRefBox != NULL)
		pPosAb->PosAbRef->AbBox = pRefBox;
	    }

	  if (pRefBox != NULL)
	    {
	      /* regarde si la position depend d'une boite invisible */
	      if (pPosAb->PosAbRef->AbVisibility < ViewFrameTable[frame - 1].FrVisibility)
		delta = 0;
	      else if (pPosAb->PosUnit == UnPercent)
		{
		  /* Convert the distance value */
		  if (pAb->AbEnclosing == NULL)
		    delta = 0;
		  else
		    delta = PixelValue (pPosAb->PosDistance, pPosAb->PosUnit,
					(PtrAbstractBox) pAb->AbEnclosing->AbBox->BxW, 0);
		}
	      else
		/* Convert the distance value */
		delta = PixelValue (pPosAb->PosDistance, pPosAb->PosUnit, pAb, ViewFrameTable[frame - 1].FrMagnification);
	      
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
	      else if (pPosAb->PosUnit == UnPercent)
		{
		  /* Convert the distance value */
		  if (pAb->AbEnclosing == NULL)
		    delta = 0;
		  else
		    delta = PixelValue (pPosAb->PosDistance, pPosAb->PosUnit,
					(PtrAbstractBox) pAb->AbEnclosing->AbBox->BxH, 0);
		}
	      else
		/* Convert the distance value */
		delta = PixelValue (pPosAb->PosDistance, pPosAb->PosUnit, pAb, ViewFrameTable[frame - 1].FrMagnification);
	      
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
#ifdef __STDC__
void                ComputeAxisRelation (AbPosition rule, PtrBox pBox, int frame, ThotBool horizRef)
#else  /* __STDC__ */
void                ComputeAxisRelation (rule, pBox, frame, horizRef)
AbPosition          rule;
PtrBox              pBox;
int                 frame;
ThotBool            horizRef;

#endif /* __STDC__ */
{
   int                 x, y, dist;
   PtrBox              pRefBox;
   PtrAbstractBox      pRefAb;
   PtrAbstractBox      pAb;
   BoxEdge             refEdge, localEdge;

   /* Calcule la position de reference */
   pRefAb = rule.PosAbRef;
   pAb = pBox->BxAbstractBox;
   /* Verifie que la position ne depend pas d'un pave mort */
   if (pRefAb != NULL && pRefAb->AbDead)
     {
	fprintf (stderr, "Position refers a dead box");
	pRefAb = NULL;
     }

   /* SRule par defaut */
   if (pRefAb == NULL)
     {
	pRefBox = pBox;
	if (horizRef)
	  {
	     refEdge = Left;
	     localEdge = VertRef;
	     if (rule.PosUnit == UnPercent)
		dist = PixelValue (rule.PosDistance, UnPercent, (PtrAbstractBox) pBox->BxW, 0);
	     else
		dist = 0;
	  }
	else
	  {
	     refEdge = Bottom;
	     localEdge = HorizRef;
	     if (rule.PosUnit == UnPercent)
		dist = PixelValue (rule.PosDistance, UnPercent, (PtrAbstractBox) pBox->BxH, 0);
	     else
		dist = FontBase (pBox->BxFont) - pBox->BxHeight;
	  }
     }
   /* SRule explicite */
   else
     {
	refEdge = rule.PosRefEdge;
	localEdge = rule.PosEdge;
	/* Convert the distance value */
	if (rule.PosUnit == UnPercent)
	  {
	     if (horizRef)
		dist = PixelValue (rule.PosDistance, UnPercent, (PtrAbstractBox) pAb->AbEnclosing->AbBox->BxW, 0);
	     else
		dist = PixelValue (rule.PosDistance, UnPercent, (PtrAbstractBox) pAb->AbEnclosing->AbBox->BxH, 0);
	  }
	else
	   dist = PixelValue (rule.PosDistance, rule.PosUnit, pAb, ViewFrameTable[frame - 1].FrMagnification);

	pRefBox = pRefAb->AbBox;
	if (pRefBox == NULL)
	  {
	     /* On doit resoudre une reference en avant */
	     pRefBox = GetBox (pRefAb);
	     if (pRefBox != NULL)
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
   if (pRefBox == pBox
       || pBox->BxType == BoGhost
       || (pAb->AbInLine && pAb->AbLeafType == LtCompound))
     {
	x = pBox->BxLMargin + pBox->BxLBorder + pBox->BxLPadding;
	y = pBox->BxTMargin + pBox->BxTBorder + pBox->BxTPadding;
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
       y += pRefBox->BxHeight;
       break;
     case Right:
       x += pRefBox->BxWidth;
       break;
     case HorizRef:
       y += pRefBox->BxHorizRef;
       break;
     case VertRef:
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
   RemovePosRelation defait, s'il existe, le lien de position ou   
   d'axe horizontal ou vertical de la boite pOrginBox et   
   retasse la liste des liens.                             
   Le parametre pCurrentAb, quand il est non nul, indique  
   le pave dont on annule la regle de position et sert a`  
   retirer l'ambiguite des relations doubles entre boites  
   soeurs : si on trouve une relation associee a` une      
   regle de position qui reference pCurrentAb, elle        
   ne doit pas etre detruite. Quand pCurrentAb est nul, on 
   connait la boite pTargetBox referencee dans le lien que 
   l'on veut detruire (c'est alors une destruction de lien 
   inverse).                                               
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static ThotBool     RemovePosRelation (PtrBox pOrginBox, PtrBox pTargetBox, PtrAbstractBox pCurrentAb, ThotBool Pos, ThotBool Axe, ThotBool horizRef)
#else  /* __STDC__ */
static ThotBool     RemovePosRelation (pOrginBox, pTargetBox, pCurrentAb, Pos, Axe, horizRef)
PtrBox              pOrginBox;
PtrBox              pTargetBox;
PtrAbstractBox      pCurrentAb;
ThotBool            Pos;
ThotBool            Axe;
ThotBool            horizRef;

#endif /* __STDC__ */
{
   int                 i, found;
   int                 j, k;
   ThotBool            loop;
   ThotBool            notEmpty;
   PtrPosRelations     pPreviousPosRel;
   PtrPosRelations     precpos;
   PtrPosRelations     pPosRel;
   PtrAbstractBox      pAb;
   BoxRelation        *pRelation;

   /* On recherche l'entree a detruire et la derniere entree occupee */
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
		/* Si c'est une relation morte, on retasse la liste */
		if (pRelation->ReBox->BxAbstractBox == NULL)
		  {
		     j = i;
		     while (j < MAX_RELAT_POS)
		       {
			  k = j + 1;
			  pPosRel->PosRTable[j - 1].ReBox = pPosRel->PosRTable[k - 1].ReBox;
			  pPosRel->PosRTable[j - 1].ReRefEdge = pPosRel->PosRTable[k - 1].ReRefEdge;
			  pPosRel->PosRTable[j - 1].ReOp = pPosRel->PosRTable[k - 1].ReOp;
			  if (pPosRel->PosRTable[k - 1].ReBox == NULL)
			     j = MAX_RELAT_POS;
			  else
			    {
			       j++;
			       /* Faut-il annuler la derniere entree ? */
			       if (j == MAX_RELAT_POS)
				  pPosRel->PosRTable[j - 1].ReBox = NULL;
			    }
		       }

		     /* C'etait la derniere relation dans la table ? */
		     if (i == MAX_RELAT_POS)
			pRelation->ReBox = NULL;
		     else
			/* il faut reexaminer cette entree */
			i--;
		  }
		/* Si c'est une relation en X */
		else if (horizRef)
		  {
		     /* Est-ce l'entree a detruite ? */
		     if (pRelation->ReBox == pTargetBox
			 && ((Axe && pRelation->ReOp == OpHorizRef)
			     || (Pos && pRelation->ReOp == OpHorizInc)
			     || (Pos && pCurrentAb == NULL && pRelation->ReOp == OpHorizDep)
		     /* Ni Axe ni Pos quand il s'agit d'une dimension elastique */
			   || (!Pos && !Axe && pRelation->ReOp == OpWidth)))
		       {
			  found = i;
			  pPreviousPosRel = pPosRel;
		       }

		     /* Est-ce la relation de position du pave pCurrentAb ? */
		     else if (found == 0 && Pos && pRelation->ReOp == OpHorizDep
		     && pCurrentAb != NULL && pRelation->ReBox == pTargetBox)
		       {
			  pAb = pRelation->ReBox->BxAbstractBox;
			  pAb = pAb->AbHorizPos.PosAbRef;

			  /* Si la position du pave distant est donnee par une    */
			  /* regle NULL, il faut rechercher le pave dont il depend */
			  if (pAb == NULL)
			     pAb = GetPosRelativeAb (pRelation->ReBox->BxAbstractBox, horizRef);

			  /* On a bien found la relation de positionnement       */
			  /* du pave pCurrentAb et non une relation de positionnement */
			  /* du pave distant par rapport au pave pCurrentAb ?         */
			  if (pAb != pCurrentAb)
			    {
			       found = i;
			       pPreviousPosRel = pPosRel;
			    }
		       }
		  }
		/* Si c'est une relation en Y */
		/* Est-ce l'entree a detruite ? */
		else if (pRelation->ReBox == pTargetBox
			 && ((Axe && pRelation->ReOp == OpVertRef)
			     || (Pos && pRelation->ReOp == OpVertInc)
			     || (Pos && pCurrentAb == NULL && pRelation->ReOp == OpVertDep)
		   /* Ni Axe ni Pos quand il s'agit d'une dimension elastique */
			  || (!Pos && !Axe && pRelation->ReOp == OpHeight)))
		  {
		     found = i;
		     pPreviousPosRel = pPosRel;
		  }

		/* Est-ce la relation de position du pave pCurrentAb ? */
		else if (found == 0 && Pos && pRelation->ReOp == OpVertDep
		    && pCurrentAb != NULL && pRelation->ReBox == pTargetBox)
		  {
		     pAb = pRelation->ReBox->BxAbstractBox;
		     pAb = pAb->AbVertPos.PosAbRef;

		     /* Si la position du pave distant est donnee par une    */
		     /* regle NULL, il faut rechercher le pave dont il depend */
		     if (pAb == NULL)
			pAb = GetPosRelativeAb (pRelation->ReBox->BxAbstractBox, horizRef);

		     /* On a bien trouve la relation de positionnement       */
		     /* du pave pCurrentAb et non une relation de positionnement */
		     /* du pave distant par rapport au pave pCurrentAb ?         */
		     if (pAb != pCurrentAb)
		       {
			  found = i;
			  pPreviousPosRel = pPosRel;
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
		pPosRel = pPosRel->PosRNext;	/* Bloc suivant */
	     }
	}
   /* On a trouve -> on retasse la liste */
   if (found > 0)
     {
	pRelation = &pPreviousPosRel->PosRTable[found - 1];

	/* Faut-il defaire la relation inverse ? */
	if (pCurrentAb != NULL && (pRelation->ReOp == OpHorizDep || pRelation->ReOp == OpVertDep))
	   loop = RemovePosRelation (pRelation->ReBox, pOrginBox, NULL, Pos, Axe, horizRef);

	if (i > 1)
	  {
	     i--;
	     pRelation->ReBox = pPosRel->PosRTable[i - 1].ReBox;
	     pRelation->ReRefEdge = pPosRel->PosRTable[i - 1].ReRefEdge;
	     pRelation->ReOp = pPosRel->PosRTable[i - 1].ReOp;
	  }

	/* Faut-il liberer le dernier bloc de relations ? */
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
   RemoveDimRelation defait, s'il existe, le lien de dimension     
   horizontale ou verticale de la boite pOrginBox vers la boite
   pTargetBox et retasse la liste des liens.                   
   Rend la valeur Vrai si l'operation a ete executee.          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static ThotBool     RemoveDimRelation (PtrBox pOrginBox, PtrBox pTargetBox, ThotBool horizRef)
#else  /* __STDC__ */
static ThotBool     RemoveDimRelation (pOrginBox, pTargetBox, horizRef)
PtrBox              pOrginBox;
PtrBox              pTargetBox;
ThotBool            horizRef;

#endif /* __STDC__ */
{
   int                 i, found;
   ThotBool            loop;
   ThotBool            notEmpty;
   PtrDimRelations     pFoundDimRel;
   PtrDimRelations     pPreviousDimRel;
   PtrDimRelations     pDimRel;
   ThotBool            result;

   i = 0;
   /* Cela peut etre une dimension elastique */
   if ((horizRef && pTargetBox->BxHorizFlex) || (!horizRef && pTargetBox->BxVertFlex))
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
   ClearXOutOfStructRelation detruit les relations hors hierarchie  
   de la boite pTargetBox.                                
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ClearXOutOfStructRelation (PtrBox pTargetBox)
#else  /* __STDC__ */
void                ClearXOutOfStructRelation (pTargetBox)
PtrBox              pTargetBox;

#endif /* __STDC__ */
{
   PtrBox              pOrginBox;
   PtrAbstractBox      pAb;

   pAb = pTargetBox->BxAbstractBox;

   /* On detruit la relation de position horizontale hors-structure */
   if (pTargetBox->BxXOutOfStruct)
     {
	if (pAb->AbHorizPos.PosAbRef == NULL)
	   pOrginBox = NULL;
	else
	   pOrginBox = pAb->AbHorizPos.PosAbRef->AbBox;
	if (pOrginBox != NULL)
	   RemovePosRelation (pOrginBox, pTargetBox, NULL, TRUE, FALSE, TRUE);

	/* Annule les relations hors-structure */
	PropagateXOutOfStruct (pAb, FALSE, pAb->AbHorizEnclosing);
     }

   /* On detruit la relation de position verticale hors-structure */
   if (pTargetBox->BxYOutOfStruct)
     {
	if (pAb->AbVertPos.PosAbRef == NULL)
	   pOrginBox = NULL;
	else
	   pOrginBox = pAb->AbVertPos.PosAbRef->AbBox;
	if (pOrginBox != NULL)
	   RemovePosRelation (pOrginBox, pTargetBox, NULL, TRUE, FALSE, FALSE);

	/* Annule les relations hors-structure */
	PropagateYOutOfStruct (pAb, FALSE, pAb->AbVertEnclosing);
     }

   /* On detruit la relation de largeur hors-structure */
   if (pTargetBox->BxWOutOfStruct)
      /* Est-ce une dimension elastique ? */
      if (pTargetBox->BxHorizFlex)
	{
	   pOrginBox = pAb->AbWidth.DimPosition.PosAbRef->AbBox;
	   if (pOrginBox != NULL)
	      RemovePosRelation (pOrginBox, pTargetBox, NULL, FALSE, FALSE, TRUE);
	}
      else
	{
	   pOrginBox = pAb->AbWidth.DimAbRef->AbBox;
	   if (pOrginBox != NULL)
	      RemoveDimRelation (pOrginBox, pTargetBox, TRUE);
	}

   /* On detruit la relation de hauteur hors-structure */
   pOrginBox = NULL;
   if (pTargetBox->BxHOutOfStruct)
      /* Est-ce une dimension elastique ? */
      if (pTargetBox->BxVertFlex)
	{
	   if (pAb->AbHeight.DimPosition.PosAbRef != NULL)
	      pOrginBox = pAb->AbHeight.DimPosition.PosAbRef->AbBox;
	   if (pOrginBox != NULL)
	      RemovePosRelation (pOrginBox, pTargetBox, NULL, FALSE, FALSE, FALSE);
	}
      else
	{
	   if (pAb->AbHeight.DimAbRef != NULL)
	      pOrginBox = pAb->AbHeight.DimAbRef->AbBox;
	   if (pOrginBox != NULL)
	      RemoveDimRelation (pOrginBox, pTargetBox, FALSE);
	}
}


/*----------------------------------------------------------------------
   ClearPosRelation recherche la boite dont depend la position     
   horizontale (si horizRef est Vrai) sinon verticale de   
   pOrginBox :						
   - Elle peut dependre de son englobante                  
   (relation OpInclus chez elle).                          
   - Elle peut dependre d'une voisine (deux                
   relations OpLie).                                       
   - Elle peut avoir une relation hors-structure           
   (deux relations OpLie).                                 
   Si cette dependance existe encore, on detruit les       
   relations entre les deux boites. L'indicateur           
   BtX(Y)HorsStruct indique que la relation est            
   hors-structure.                                         
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ClearPosRelation (PtrBox pOrginBox, ThotBool horizRef)
#else  /* __STDC__ */
void                ClearPosRelation (pOrginBox, horizRef)
PtrBox              pOrginBox;
ThotBool            horizRef;

#endif /* __STDC__ */
{
   PtrAbstractBox      pAb;
   PtrAbstractBox      pCurrentAb;
   ThotBool            loop;

   pAb = pOrginBox->BxAbstractBox;
   /* Est-ce une relation hors-structure ? */
   if ((horizRef && pOrginBox->BxXOutOfStruct) || (!horizRef && pOrginBox->BxYOutOfStruct))
     {
	/* Si la boite est detruite la procedure ClearXOutOfStructRelation detruit */
	/* automatiquement cette relation                               */
	if (!pAb->AbDead)
	  {
	     /* On remonte a la racine depuis le pave pere */
	     pCurrentAb = pAb;
	     /* La relation hors-structure peut etre heritee d'une boite voisine */
	     while (pCurrentAb->AbEnclosing != NULL)
		pCurrentAb = pCurrentAb->AbEnclosing;
	     loop = TRUE;


	     /* Recherche dans pOrginBox l'ancienne relation de positionnement */
	     while (loop && pCurrentAb != NULL)
	       {
		  if (pCurrentAb->AbBox != NULL)
		     loop = !RemovePosRelation (pOrginBox, pCurrentAb->AbBox, pAb, TRUE, FALSE, horizRef);
		  pCurrentAb = NextAbToCheck (pCurrentAb, pAb);
	       }

	     /* La relation hors-structure est detruite */
	     if (horizRef)
	       {
		  pOrginBox->BxXOutOfStruct = FALSE;

		  /* Des boites voisines ont herite de la relation hors-structure ? */
		  pCurrentAb = pAb->AbEnclosing;
		  if (pCurrentAb != NULL)
		    {
		       /* regarde tous les freres */
		       pCurrentAb = pCurrentAb->AbFirstEnclosed;
		       while (pCurrentAb != NULL)
			 {
			    if (pCurrentAb != pAb && pCurrentAb->AbBox != NULL)
			       /* Si c'est un heritage on retire l'indication hors-structure */
			       if (pCurrentAb->AbBox->BxXOutOfStruct
				   && pCurrentAb->AbHorizPos.PosAbRef == pAb)
				  pCurrentAb->AbBox->BxXOutOfStruct = FALSE;
			       else if (pCurrentAb->AbBox->BxWOutOfStruct
					&& pCurrentAb->AbWidth.DimIsPosition
					&& pCurrentAb->AbWidth.DimPosition.PosAbRef == pAb)
				  pCurrentAb->AbBox->BxWOutOfStruct = FALSE;

			    pCurrentAb = pCurrentAb->AbNext;
			 }
		    }
	       }
	     else
	       {
		  pOrginBox->BxYOutOfStruct = FALSE;

		  /* Des boites voisines ont herite de la relation hors-structure ? */
		  pCurrentAb = pAb->AbEnclosing;
		  if (pCurrentAb != NULL)
		    {
		       pCurrentAb = pCurrentAb->AbFirstEnclosed;
		       while (pCurrentAb != NULL)
			 {
			    if (pCurrentAb != pAb && pCurrentAb->AbBox != NULL)
			       /* Si c'est un heritage on retire l'indication hors-structure */
			       if (pCurrentAb->AbBox->BxYOutOfStruct
				   && pCurrentAb->AbVertPos.PosAbRef == pAb)
				  pCurrentAb->AbBox->BxYOutOfStruct = FALSE;
			       else if (pCurrentAb->AbBox->BxHOutOfStruct
					&& pCurrentAb->AbHeight.DimIsPosition
					&& pCurrentAb->AbHeight.DimPosition.PosAbRef == pAb)
				  pCurrentAb->AbBox->BxHOutOfStruct = FALSE;

			    pCurrentAb = pCurrentAb->AbNext;
			 }

		    }
	       }
	  }
     }
   /* Est-ce une relation avec une boite voisine ? */
   else
     {
	pCurrentAb = pAb->AbEnclosing;
	loop = TRUE;

	/* Recherche dans la boite pOrginBox l'ancienne relation de positionnement */
	while (loop && pCurrentAb != NULL)
	  {
	     if (pCurrentAb->AbBox != NULL)
		loop = !RemovePosRelation (pOrginBox, pCurrentAb->AbBox, pAb, TRUE, FALSE, horizRef);
	     if (pCurrentAb == pAb->AbEnclosing)
		pCurrentAb = pCurrentAb->AbFirstEnclosed;
	     else
		pCurrentAb = pCurrentAb->AbNext;
	  }
     }
}

/*----------------------------------------------------------------------
   ClearAxisRelation recherche la boite dont depend l'axe de reference
   horizontal (si horizRef est Vrai) sinon vertical de     
   pOrginBox :  						
   - Il peut dependre d'elle meme (une relation chez elle).
   - Il peut dependre d'une englobee (une relation chez    
   l'englobee).                                            
   - Il peut dependre d'une voisine (une relation chez la  
   voisine).                                               
   Si cette dependance existe encore, on detruit la        
   relation.                                               
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ClearAxisRelation (PtrBox pOrginBox, ThotBool horizRef)
#else  /* __STDC__ */
void                ClearAxisRelation (pOrginBox, horizRef)
PtrBox              pOrginBox;
ThotBool            horizRef;

#endif /* __STDC__ */
{
   ThotBool            loop;
   PtrAbstractBox      pAb;
   PtrAbstractBox      pCurrentAb;


   pCurrentAb = pOrginBox->BxAbstractBox;
   loop = TRUE;

   /* On recherche dans la descendance la dependance de l'axe de reference */
   pAb = pCurrentAb;
   while (loop && pAb != NULL)
     {
	if (pAb->AbBox != NULL)
	   loop = !RemovePosRelation (pAb->AbBox, pOrginBox, NULL, FALSE, TRUE, horizRef);
	if (pAb == pCurrentAb)
	   pAb = pAb->AbFirstEnclosed;
	else
	   pAb = pAb->AbNext;

     }

   /* On recherche chez les voisines la dependance de l'axe de reference */
   pAb = pCurrentAb->AbEnclosing;
   if (pAb != NULL)
     {
	pAb = pAb->AbFirstEnclosed;
	while (loop && pAb != NULL)
	  {
	     if (pAb != pCurrentAb && pAb->AbBox != NULL)
		loop = !RemovePosRelation (pAb->AbBox, pOrginBox, NULL, FALSE, TRUE, horizRef);
	     pAb = pAb->AbNext;
	  }
     }
}


/*----------------------------------------------------------------------
   ClearAllRelations detruit toutes les relations avec pTargetBox     
   chez ses voisines, son englobante et les relations hors 
   hierarchie :                                            
   -> la relation OpVertRef verticale.                     
   -> la relation OpHorizRef horizontale.                  
   -> les relations de Position.                           
   -> les relations de positions et de dimensions hors     
   hierarchie.                                             
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ClearAllRelations (PtrBox pTargetBox)
#else  /* __STDC__ */
void                ClearAllRelations (pTargetBox)
PtrBox              pTargetBox;

#endif /* __STDC__ */
{
   PtrAbstractBox      pAb;
   PtrAbstractBox      pCurrentAb;

   pCurrentAb = pTargetBox->BxAbstractBox;
   pAb = pCurrentAb->AbEnclosing;
   if (pAb != NULL)
     {
	/* On detruit les liens d'axe qui la concerne chez l'englobante */
	/*en Y */
	RemovePosRelation (pAb->AbBox, pTargetBox, NULL, FALSE, TRUE, FALSE);
	/*en X */
	RemovePosRelation (pAb->AbBox, pTargetBox, NULL, FALSE, TRUE, TRUE);

	/* On detruit les liens qui la concerne chez toutes ses voisines */
	pAb = pAb->AbFirstEnclosed;
	while (pAb != NULL)
	  {
	     if (pAb->AbBox != NULL && pAb != pCurrentAb)
	       {
		  /* enleve la regle de position d'axe ou de dimension */
		  RemovePosRelation (pAb->AbBox, pTargetBox, NULL, TRUE, FALSE, FALSE);
		  RemovePosRelation (pAb->AbBox, pTargetBox, NULL, FALSE, TRUE, FALSE);
		  RemovePosRelation (pAb->AbBox, pTargetBox, NULL, FALSE, FALSE, FALSE);

		  /* enleve la regle de position d'axe ou de dimension */
		  RemovePosRelation (pAb->AbBox, pTargetBox, NULL, TRUE, FALSE, TRUE);
		  RemovePosRelation (pAb->AbBox, pTargetBox, NULL, FALSE, TRUE, TRUE);
		  RemovePosRelation (pAb->AbBox, pTargetBox, NULL, FALSE, FALSE, TRUE);
	       }
	     pAb = pAb->AbNext;
	  }
	/* On detruit eventuellement les liens non hierarchiques */
	ClearXOutOfStructRelation (pTargetBox);
     }
}


/*----------------------------------------------------------------------
   ClearDimRelation recherche la boite dont depend la dimension      
   horizontale ou verticale de pOrginBox :                
   - Elle peut dependre de son englobante (une relation    
   chez l'englobante).                                     
   - Elle peut dependre d'une voisine (une relation chez   
   la voisine).                                            
   - Elle peut avoir une relation hors-structure           
   (relation chez l'autre).                                
   Si cette dependance existe encore, on detruit la        
   relation. L'indicateur BtLg(Ht)HorsStruct indique que   
   la relation est hors-structure.                         
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ClearDimRelation (PtrBox pOrginBox, ThotBool horizRef, int frame)
#else  /* __STDC__ */
void                ClearDimRelation (pOrginBox, horizRef, frame)
PtrBox              pOrginBox;
ThotBool            horizRef;
int                 frame;

#endif /* __STDC__ */
{
   ThotBool            loop;
   PtrAbstractBox      pAb;
   PtrAbstractBox      pCurrentAb;


   pAb = pOrginBox->BxAbstractBox;
   pCurrentAb = pAb->AbEnclosing;
   loop = FALSE;

   /* Est-ce une relation hors-structure en X ? */
   if ((horizRef && pOrginBox->BxWOutOfStruct) || (!horizRef && pOrginBox->BxHOutOfStruct))
     {
	/* Si la boite est detruite la procedure ClearXOutOfStructRelation detruit */
	/* automatiquement cette relation                               */
	if (!pAb->AbDead)
	  {
	     /* On remonte a la racine depuis le pave pere */
	     pCurrentAb = pAb->AbEnclosing;
	     /* La relation hors-structure peut etre heritee d'une boite voisine */
	     if (pCurrentAb != NULL)
		if (pCurrentAb != NULL)
		   while (pCurrentAb->AbEnclosing != NULL)
		      pCurrentAb = pCurrentAb->AbEnclosing;


	     /* Recherche dans toute l'arborecence la relation inverse */
	     loop = TRUE;
	     if (horizRef)
	       {
		  /* La dimension est elastique en X ? */
		  if (pOrginBox->BxHorizFlex)
		    {
		       while (loop && pCurrentAb != NULL)
			 {
			    if (pCurrentAb->AbBox != NULL)
			       loop = !RemovePosRelation (pCurrentAb->AbBox, pOrginBox, NULL, FALSE, FALSE, horizRef);
			    if (pCurrentAb != NULL)
			       pCurrentAb = NextAbToCheck (pCurrentAb, pAb);
			 }

		       /* Il faut retablir le point fixe */
		       pOrginBox->BxHorizEdge = pOrginBox->BxAbstractBox->AbHorizPos.PosEdge;

		       /* La boite n'est pas inversee */
		       pOrginBox->BxHorizInverted = FALSE;

		       /* La dimension n'est plus elastique */
		       pOrginBox->BxHorizFlex = FALSE;

		       /* Annule la largeur de la boite */
		       ResizeWidth (pOrginBox, NULL, NULL, -pOrginBox->BxW, 0, 0, 0, frame);

		    }
		  /* La dimension n'est pas elastique en X */
		  else
		     while (loop && pCurrentAb != NULL)
		       {
			  if (pCurrentAb->AbBox != NULL)
			     loop = !RemoveDimRelation (pCurrentAb->AbBox, pOrginBox, horizRef);
			  if (pCurrentAb != NULL)
			     pCurrentAb = NextAbToCheck (pCurrentAb, pAb);
		       }

		  /* La relation hors-structure est detruite */
		  pOrginBox->BxWOutOfStruct = FALSE;
	       }
	     else
	       {
		  /* La dimension est elastique en Y ? */
		  if (pOrginBox->BxVertFlex)
		    {
		       while (loop && pCurrentAb != NULL)
			 {
			    if (pCurrentAb->AbBox != NULL)
			       loop = !RemovePosRelation (pCurrentAb->AbBox, pOrginBox, NULL, FALSE, FALSE, horizRef);
			    if (pCurrentAb != NULL)
			       pCurrentAb = NextAbToCheck (pCurrentAb, pAb);
			 }

		       /* Il faut retablir le point fixe */
		       pOrginBox->BxVertEdge = pOrginBox->BxAbstractBox->AbVertPos.PosEdge;

		       /* La boite n'est pas inversee */
		       pOrginBox->BxVertInverted = FALSE;

		       /* La dimension n'est plus elastique */
		       pOrginBox->BxVertFlex = FALSE;

		       /* Annule la hauteur de la boite */
		       ResizeHeight (pOrginBox, NULL, NULL, -pOrginBox->BxH, 0, 0, frame);
		    }
		  /* La dimension n'est pas elastique en Y */
		  else
		     while (loop && pCurrentAb != NULL)
		       {
			  if (pCurrentAb->AbBox != NULL)
			     loop = !RemoveDimRelation (pCurrentAb->AbBox, pOrginBox, horizRef);
			  if (pCurrentAb != NULL)
			     pCurrentAb = NextAbToCheck (pCurrentAb, pAb);
		       }

		  /* La relation hors-structure est detruite */
		  pOrginBox->BxHOutOfStruct = FALSE;
	       }
	  }
     }

   else
     {
	/* Est-ce que la dimension depend de l'englobante ? */
	if (pCurrentAb != NULL)
	  {
	     loop = !RemoveDimRelation (pCurrentAb->AbBox, pOrginBox, horizRef);
	     if (loop)
		pCurrentAb = pCurrentAb->AbFirstEnclosed;
	  }

	/* Est-ce que la dimension de la boite depend d'une voisine ? */
	while (loop && pCurrentAb != NULL)
	  {
	     if (pCurrentAb->AbBox != NULL && pCurrentAb != pAb)
	       {
		  loop = !RemoveDimRelation (pCurrentAb->AbBox, pOrginBox, horizRef);
		  if (loop)
		     pCurrentAb = pCurrentAb->AbNext;
	       }
	     else
		pCurrentAb = pCurrentAb->AbNext;
	  }

	/* La dimension est elastique en X ? */
	if (horizRef && pOrginBox->BxHorizFlex)
	  {
	     /* La boite n'est pas inversee */
	     pOrginBox->BxHorizInverted = FALSE;

	     /* La dimension n'est plus elastique */
	     pOrginBox->BxHorizFlex = FALSE;

	     /* Il faut retablir le point fixe */
	     pOrginBox->BxHorizEdge = pOrginBox->BxAbstractBox->AbHorizPos.PosEdge;

	     /* Annule la largeur de la boite */
	     ResizeWidth (pOrginBox, NULL, NULL, -pOrginBox->BxW, 0, 0, 0, frame);
	  }

	/* La dimension est elastique en Y ? */
	if (!horizRef && pOrginBox->BxVertFlex)
	  {
	     /* La boite n'est pas inversee */
	     pOrginBox->BxVertInverted = FALSE;

	     /* La dimension n'est plus elastique */
	     pOrginBox->BxVertFlex = FALSE;

	     /* Il faut retablir le point fixe */
	     pOrginBox->BxVertEdge = pOrginBox->BxAbstractBox->AbVertPos.PosEdge;

	     /* Annule la hauteur de la boite */
	     ResizeHeight (pOrginBox, NULL, NULL, -pOrginBox->BxH, 0, 0, frame);
	  }
     }

}
