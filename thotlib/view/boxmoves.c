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
 * This module maintains constraints between boxes
 *
 * Author: I. Vatton (INRIA)
 *
 */

#include "ustring.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "appdialogue.h"

#define THOT_EXPORT extern
#include "boxes_tv.h"
#include "appdialogue_tv.h"

#include "boxmoves_f.h"
#include "boxrelations_f.h"
#include "buildboxes_f.h"
#include "buildlines_f.h"
#include "exceptions_f.h"
#include "font_f.h"
#include "frame_f.h"
#include "memory_f.h"
#include "windowdisplay_f.h"


/*----------------------------------------------------------------------
   IsXYPosComplete retourne les indicateurs de placement absolu     
   horizRef et vertRef pour la boi^te pBox.                
   L'indicateur est vrai si on est pas dans une cre'ation  
   initiale ou si une boi^te englobante de la boi^te pBox  
   est e'lastique horizontalement (verticalement) ou si    
   une boi^te englobante a une relation hors-structure.    
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                IsXYPosComplete (PtrBox pBox, ThotBool * horizRef, ThotBool * vertRef)
#else  /* __STDC__ */
void                IsXYPosComplete (pBox, horizRef, vertRef)
PtrBox              pBox;
ThotBool           *horizRef;
ThotBool           *vertRef;

#endif /* __STDC__ */
{
   PtrBox              pParentBox;

   pParentBox = pBox;
   *vertRef = (Propagate != ToSiblings);
   *horizRef = *vertRef;
   while ((!*vertRef || !*horizRef) && pParentBox != NULL)
     {
	if (!*vertRef)
	   *vertRef = (pParentBox->BxVertFlex || pParentBox->BxYOutOfStruct);
	if (!*horizRef)
	   *horizRef = (pParentBox->BxHorizFlex || pParentBox->BxXOutOfStruct);
	/* Remonte a la boite englobante */
	if (pParentBox->BxAbstractBox->AbEnclosing == NULL)
	   pParentBox = NULL;
	else
	   pParentBox = pParentBox->BxAbstractBox->AbEnclosing->AbBox;
     }

   if (pBox->BxType == BoBlock && pBox->BxYToCompute)
      /* C'est trop tot pour placer le contenu du bloc de lignes en Y */
      *vertRef = FALSE;
}


/*----------------------------------------------------------------------
   IsXPosComplete retourne vrai si on est pas dans une cre'ation    
   initiale ou si une boi^te englobante de la boi^te pBox  
   est e'lastique horizontalement ou si elle a un          
   positionnement horizontal hors-structure.               
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool            IsXPosComplete (PtrBox pBox)
#else  /* __STDC__ */
ThotBool            IsXPosComplete (pBox)
PtrBox              pBox;

#endif /* __STDC__ */
{
   PtrBox              pParentBox;
   ThotBool            Ok;

   pParentBox = pBox;
   Ok = (Propagate != ToSiblings);
   while (!Ok && pParentBox != NULL)
     {
	Ok = (pParentBox->BxHorizFlex || pParentBox->BxXOutOfStruct);
	/* Remonte a la boite englobante */
	if (pParentBox->BxAbstractBox->AbEnclosing == NULL)
	   pParentBox = NULL;
	else
	   pParentBox = pParentBox->BxAbstractBox->AbEnclosing->AbBox;
     }
   return (Ok);
}


/*----------------------------------------------------------------------
   IsYPosComplete retourne vrai si on est pas dans une cre'ation    
   initiale ou si une boi^te englobante de la boi^te pBox  
   est e'lastique verticalement ou si elle a un            
   positionnement vertical hors-structure.                 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool            IsYPosComplete (PtrBox pBox)
#else  /* __STDC__ */
ThotBool            IsYPosComplete (pBox)
PtrBox              pBox;

#endif /* __STDC__ */
{
   PtrBox              pParentBox;
   ThotBool            Ok;

   pParentBox = pBox;
   Ok = (Propagate != ToSiblings);
   while (!Ok && pParentBox != NULL)
     {
	Ok = (pParentBox->BxVertFlex || pParentBox->BxYOutOfStruct);
	/* Remonte a la boite englobante */
	if (pParentBox->BxAbstractBox->AbEnclosing == NULL)
	   pParentBox = NULL;
	else
	   pParentBox = pParentBox->BxAbstractBox->AbEnclosing->AbBox;
     }
   return (Ok);
}


/*----------------------------------------------------------------------
   IsParentBox retourne vrai si pBox est une englobante de la boite 
   pRefBox.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool            IsParentBox (PtrBox pBox, PtrBox pRefBox)
#else  /* __STDC__ */
ThotBool            IsParentBox (pBox, pRefBox)
PtrBox              pBox;
PtrBox              pRefBox;

#endif /* __STDC__ */
{
  PtrAbstractBox      pAb;
  ThotBool            equal;

  if (pRefBox == NULL || pBox == NULL)
    return (FALSE);
  else
    {
      /* Recherche dans la parente de pRefBox y compris elle-meme */
      pAb = pRefBox->BxAbstractBox;
      equal = FALSE;
      while (!equal && pAb != NULL)
	{
	  equal = pAb->AbBox == pBox;
	  pAb = pAb->AbEnclosing;
	}
      return (equal);
    }
}


/*----------------------------------------------------------------------
   IsSiblingBox retourne vrai si pBox a le meme pave pere que la    
   boite pRefBox et n'est pas la boite pBox.           
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static ThotBool     IsSiblingBox (PtrBox pBox, PtrBox pRefBox)
#else  /* __STDC__ */
static ThotBool     IsSiblingBox (pBox, pRefBox)
PtrBox              pBox;
PtrBox              pRefBox;

#endif /* __STDC__ */
{
   if (pRefBox == pBox)
      return (FALSE);
   else if (pRefBox == NULL)
      return (FALSE);
   else if (pBox == NULL)
      return (FALSE);
   else if (pRefBox->BxAbstractBox->AbEnclosing == pBox->BxAbstractBox->AbEnclosing)
      return (TRUE);
   else
      return (FALSE);
}


/*----------------------------------------------------------------------
   MirrorPolyline inverse les points de la polyline.               
   Les parame`tres horizRef et vertRef                     
   indiquent si la boi^te e'lastique a` l'origine du       
   traitement est inverse'e dans chacun des sens.          
   inAbtractBox est Vrai quand l'inversion doit            
   s'appliquer aux repe`res du pave et non de la boite.    
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         MirrorPolyline (PtrAbstractBox pAb, ThotBool horizRef, ThotBool vertRef, ThotBool inAbtractBox)
#else  /* __STDC__ */
static void         MirrorPolyline (pAb, horizRef, vertRef, inAbtractBox)
PtrAbstractBox      pAb;
ThotBool            horizRef;
ThotBool            vertRef;
ThotBool            inAbtractBox;

#endif /* __STDC__ */
{
   PtrTextBuffer       pSourceBuff, pTargetBuff;
   PtrBox              pBox;
   int                 xMax, yMax;
   int                 val, i, j;
   float               xRatio, yRatio;
   int                 pointIndex;

   pBox = pAb->AbBox;
   j = 1;
   if (inAbtractBox)
     {
	/* maj des reperes du pave a partir de la boite */
	pSourceBuff = pBox->BxBuffer;
	pTargetBuff = pAb->AbPolyLineBuffer;
	xRatio = pBox->BxXRatio;
	yRatio = pBox->BxYRatio;
     }
   else
     {
	/* maj des reperes de la boite a partir du pave */
	pSourceBuff = pAb->AbPolyLineBuffer;
	pTargetBuff = pBox->BxBuffer;
	/* annule les deformations precedentes */
	xRatio = 1;
	yRatio = 1;
	pBox->BxXRatio = 1;
	pBox->BxYRatio = 1;
	pTargetBuff->BuPoints[0].XCoord = pSourceBuff->BuPoints[0].XCoord;
	pTargetBuff->BuPoints[0].YCoord = pSourceBuff->BuPoints[0].YCoord;
     }

   /* inversions eventuelles par rapport aux milieux */
   xMax = pSourceBuff->BuPoints[0].XCoord;
   yMax = pSourceBuff->BuPoints[0].YCoord;
   val = pBox->BxNChars;
   for (i = 1; i < val; i++)
     {
	if (j >= pSourceBuff->BuLength)
	  {
	     if (pSourceBuff->BuNext != NULL)
	       {
		  /* Changement de buffer */
		  pSourceBuff = pSourceBuff->BuNext;
		  pTargetBuff = pTargetBuff->BuNext;
		  j = 0;
	       }
	  }

	pointIndex = (int) ((float) pSourceBuff->BuPoints[j].XCoord / xRatio);
	if (horizRef)
	   /* inversion des points en x */
	   pTargetBuff->BuPoints[j].XCoord = xMax - pointIndex;
	else
	   pTargetBuff->BuPoints[j].XCoord = pointIndex;

	pointIndex = (int) ((float) pSourceBuff->BuPoints[j].YCoord / yRatio);
	if (vertRef)
	   /* inversion des points en y */
	   pTargetBuff->BuPoints[j].YCoord = yMax - pointIndex;
	else
	   pTargetBuff->BuPoints[j].YCoord = pointIndex;

	j++;
     }

   if (pBox->BxPictInfo != NULL && !inAbtractBox)
     {
	/* le calcul des points de controle doit etre reexecute */
	free ((STRING) pBox->BxPictInfo);
	pBox->BxPictInfo = NULL;
     }
}


/*----------------------------------------------------------------------
   MirrorShape ajuste les trace's graphiques en fonction des       
   inversions des boi^tes e'lastiques.                     
   Si le pave' passe' en parame`tre est un pave' compose'  
   la demande de conversion est transmise aux pave's fils. 
   Les parame`tres horizRef et vertRef                     
   indiquent si la boi^te e'lastique a` l'origine du       
   traitement est inverse'e dans chacun des sens.          
   inAbtractBox est Vrai quand le caracte`re de            
   re'fe'rence correspond au caracte`re saisi (TraceReel)  
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                MirrorShape (PtrAbstractBox pAb, ThotBool horizRef, ThotBool vertRef, ThotBool inAbtractBox)
#else  /* __STDC__ */
void                MirrorShape (pAb, horizRef, vertRef, inAbtractBox)
PtrAbstractBox      pAb;
ThotBool            horizRef;
ThotBool            vertRef;
ThotBool            inAbtractBox;

#endif /* __STDC__ */
{
   PtrAbstractBox      pChildAb;
   CHAR_T              shape;

   if (pAb->AbLeafType == LtCompound && !inAbtractBox)
     {
	/* Transmet le traitement aux paves fils */
	pChildAb = pAb->AbFirstEnclosed;
	while (pChildAb != NULL)
	  {
	     if (pChildAb->AbLeafType == LtGraphics
		 || pChildAb->AbLeafType == LtPolyLine
		 || pChildAb->AbLeafType == LtCompound)
		MirrorShape (pChildAb, horizRef, vertRef, inAbtractBox);
	     pChildAb = pChildAb->AbNext;
	  }
     }
   else if (pAb->AbLeafType == LtPolyLine)
      MirrorPolyline (pAb, horizRef, vertRef, inAbtractBox);
   else if (pAb->AbLeafType == LtGraphics)
     {
	/* Ajuste le graphique de la boite */
	if (inAbtractBox)
	   /* le caractere de reference est le trace reel */
	   shape = pAb->AbRealShape;
	else
	   /* le caractere de reference est le trace de l'element */
	   shape = pAb->AbShape;

	if (horizRef)
	   switch (shape)
		 {
		    case '\\':
		       shape = '/';
		       break;
		    case '/':
		       shape = '\\';
		       break;
		    case '>':
		       shape = '<';
		       break;
		    case '<':
		       shape = '>';
		       break;
		    case 'l':
		       shape = 'r';
		       break;
		    case 'r':
		       shape = 'l';
		       break;
		    case 'o':
		       shape = 'e';
		       break;
		    case 'e':
		       shape = 'o';
		       break;
		    case 'O':
		       shape = 'E';
		       break;
		    case 'E':
		       shape = 'O';
		       break;
		    case 'W':
		       shape = 'Z';
		       break;
		    case 'Z':
		       shape = 'W';
		       break;
		    case 'X':
		       shape = 'Y';
		       break;
		    case 'Y':
		       shape = 'X';
		       break;
		    default:
		       break;
		 }

	if (vertRef)
	   switch (shape)
		 {
		    case '\\':
		       shape = '/';
		       break;
		    case '/':
		       shape = '\\';
		       break;
		    case '^':
		       shape = 'V';
		       break;
		    case 'V':
		       shape = '^';
		       break;
		    case 't':
		       shape = 'b';
		       break;
		    case 'b':
		       shape = 't';
		       break;
		    case 'o':
		       shape = 'O';
		       break;
		    case 'O':
		       shape = 'o';
		       break;
		    case 'e':
		       shape = 'E';
		       break;
		    case 'E':
		       shape = 'e';
		       break;
		    case 'W':
		       shape = 'X';
		       break;
		    case 'Z':
		       shape = 'Y';
		       break;
		    case 'X':
		       shape = 'W';
		       break;
		    case 'Y':
		       shape = 'Z';
		       break;
		    default:
		       break;
		 }

	/* Ajuste le graphique de la boite */
	if (inAbtractBox)
	   /* le caractere de reference est le trace reel */
	   pAb->AbShape = shape;
	else
	   /* le caractere de reference est le trace de l'element */
	   pAb->AbRealShape = shape;
     }
}


/*----------------------------------------------------------------------
   XEdgesExchange inverse les reperes horizontaux position et    
   dimension de la boite elastique pBox. Si la boite est   
   une boite graphique les dessins sont modifies.          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                XEdgesExchange (PtrBox pBox, OpRelation op)
#else  /* __STDC__ */
void                XEdgesExchange (pBox, op)
PtrBox              pBox;
OpRelation          op;

#endif /* __STDC__ */
{
   PtrAbstractBox      pAb;
   PtrPosRelations     pPosRel;
   int                 i;
   BoxEdge             oldPosEdge;
   BoxEdge             newPosEdge;
   BoxEdge             oldDimEdge;
   BoxEdge             newDimEdge;
   PtrBox              pRefBox;

   pAb = pBox->BxAbstractBox;

   /* prend le repere symetrique dans le positionnement */
   /* Ancien repere de position */
   oldPosEdge = pAb->AbHorizPos.PosEdge;
   /* Nouveau repere de position */
   newPosEdge = oldPosEdge;
   /* Ancien repere de dimension */
   oldDimEdge = oldPosEdge;
   /* Nouveau repere de dimension */
   newDimEdge = oldPosEdge;
   if (oldPosEdge == Left)
     {
	newPosEdge = Right;
	pAb->AbHorizPos.PosEdge = newPosEdge;
	oldDimEdge = Right;
	newDimEdge = Left;
	pAb->AbWidth.DimPosition.PosEdge = newDimEdge;
     }
   else if (oldPosEdge == Right)
     {
	newPosEdge = Left;
	pAb->AbHorizPos.PosEdge = newPosEdge;
	oldDimEdge = Left;
	newDimEdge = Right;
	pAb->AbWidth.DimPosition.PosEdge = newDimEdge;
     }

   /* Mise a jour du point fixe de la boite */
   if (op == OpWidth && pBox->BxHorizEdge == oldPosEdge)
      pBox->BxHorizEdge = newPosEdge;
   else if (op == OpHorizDep && pBox->BxHorizEdge == oldDimEdge)
      pBox->BxHorizEdge = newDimEdge;

   /* Les reperes sont inverses */
   pBox->BxHorizInverted = !pBox->BxHorizInverted;

   /* Modifie enventuellement le dessin de la boite */
   if (oldPosEdge != newPosEdge)
      MirrorShape (pAb, pBox->BxHorizInverted, pBox->BxVertInverted, FALSE);

   /* Note la boite qui fixe la position de la boite elastique */
   if (pAb->AbHorizPos.PosAbRef == NULL)
      pRefBox = NULL;
   else
      pRefBox = pAb->AbHorizPos.PosAbRef->AbBox;

   /* Met a jour les relations de la boite */
   i = 0;
   pPosRel = pBox->BxPosRelations;
   while (pPosRel != NULL)
     {
	if (pPosRel->PosRTable[i].ReBox == NULL)
	  {
	     pPosRel = pPosRel->PosRNext;
	     i = -1;
	  }
	/* Est-ce une relation avec le repere de position ? */
	else if (pPosRel->PosRTable[i].ReRefEdge == oldPosEdge
		 && pPosRel->PosRTable[i].ReBox == pRefBox
		 && pPosRel->PosRTable[i].ReOp != OpHorizRef)
	   pPosRel->PosRTable[i].ReRefEdge = newPosEdge;
	i++;
     }
}


/*----------------------------------------------------------------------
   YEdgesExchange inverse les reperes verticaux position et dimension 
   de la boite elastique pBox. Si la boite est une boite   
   graphique, les dessins sont modifies.                   
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                YEdgesExchange (PtrBox pBox, OpRelation op)
#else  /* __STDC__ */
void                YEdgesExchange (pBox, op)
PtrBox              pBox;
OpRelation          op;

#endif /* __STDC__ */
{
   PtrAbstractBox      pAb;
   PtrPosRelations     pPosRel;
   int                 i;
   BoxEdge             oldPosEdge;
   BoxEdge             newPosEdge;
   BoxEdge             oldDimEdge;
   BoxEdge             newDimEdge;
   PtrBox              pRefBox;

   pAb = pBox->BxAbstractBox;

   /* prend le repere symetrique dans le positionnement */
   oldPosEdge = pAb->AbVertPos.PosEdge;
   /* Nouveau repere de position */
   newPosEdge = oldPosEdge;
   /* Ancien repere de dimension */
   oldDimEdge = oldPosEdge;
   /* Nouveau repere de dimension */
   newDimEdge = oldPosEdge;
   if (oldPosEdge == Top)
     {
	newPosEdge = Bottom;
	pAb->AbVertPos.PosEdge = newPosEdge;
	oldDimEdge = Bottom;
	newDimEdge = Top;
	pAb->AbHeight.DimPosition.PosEdge = newDimEdge;
     }
   else if (oldPosEdge == Bottom)
     {
	newPosEdge = Top;
	pAb->AbVertPos.PosEdge = newPosEdge;
	oldDimEdge = Top;
	newDimEdge = Bottom;
	pAb->AbHeight.DimPosition.PosEdge = newDimEdge;
     }

   /* Mise a jour du point fixe de la boite */
   if (op == OpHeight && pBox->BxVertEdge == oldPosEdge)
      pBox->BxVertEdge = newPosEdge;
   else if (op == OpVertDep && pBox->BxVertEdge == oldDimEdge)
      pBox->BxVertEdge = newDimEdge;

   /* Les reperes sont inverses */
   pBox->BxVertInverted = !pBox->BxVertInverted;

   /* Modifie enventuellement le dessin de la boite */
   if (oldPosEdge != newPosEdge)
      MirrorShape (pAb, pBox->BxHorizInverted, pBox->BxVertInverted, FALSE);

   /* Note la boite qui fixe la position de la boite elastique */
   if (pAb->AbVertPos.PosAbRef == NULL)
      pRefBox = NULL;
   else
      pRefBox = pAb->AbVertPos.PosAbRef->AbBox;

   /* Met a jour les relations de la boite */
   i = 0;
   pPosRel = pBox->BxPosRelations;
   while (pPosRel != NULL)
     {
	if (pPosRel->PosRTable[i].ReBox == NULL)
	  {
	     pPosRel = pPosRel->PosRNext;
	     i = -1;
	  }
	/* Est-ce une relation avec le repere de position ? */
	else if (pPosRel->PosRTable[i].ReRefEdge == oldPosEdge
		 && pPosRel->PosRTable[i].ReBox == pRefBox
		 && pPosRel->PosRTable[i].ReOp != OpVertRef)
	   pPosRel->PosRTable[i].ReRefEdge = newPosEdge;
	i++;
     }
}


/*----------------------------------------------------------------------
   ChangeDefaultHeight met a` jour la largeur du contenu de la boi^te 
   Cette proce'dure ve'rifie que la re`gle du minimum est  
   respecte'e. Eventuellement e'change hauteur re'elle et  
   autre hauteur.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ChangeDefaultHeight (PtrBox pBox, PtrBox pSourceBox, int height, int frame)
#else  /* __STDC__ */
void                ChangeDefaultHeight (pBox, pSourceBox, height, frame)
PtrBox              pBox;
PtrBox              pSourceBox;
int                 height;
int                 frame;

#endif /* __STDC__ */
{
   int                 delta;

   if (pBox != NULL)
     {
	/* Regarde si la hauteur reelle actuelle depend du contenu */
	if (pBox->BxContentHeight)
	  {
	     /* La hauteur reelle est egale a la hauteur du contenu */
	     if (height < pBox->BxRuleHeigth)
	       {
		  /* Il faut echanger la hauteur reelle avec l'autre hauteur */
		  delta = pBox->BxRuleHeigth - pBox->BxHeight;
		  pBox->BxRuleHeigth = height;
		  pBox->BxContentHeight = !pBox->BxContentHeight;
		  ResizeHeight (pBox, pSourceBox, NULL, delta, frame);
	       }
	     else
		/* Mise a jour de la hauteur du contenu */
		ResizeHeight (pBox, pSourceBox, NULL, height - pBox->BxHeight, frame);
	  }
	else if (!pBox->BxAbstractBox->AbHeight.DimIsPosition && pBox->BxAbstractBox->AbHeight.DimMinimum)
	  {
	     /* La hauteur reelle est egale au minimum */
	     if (height > pBox->BxHeight)
	       {
		  /* Il faut echanger la hauteur reelle avec l'autre hauteur */
		  pBox->BxRuleHeigth = pBox->BxHeight;
		  pBox->BxContentHeight = !pBox->BxContentHeight;
		  ResizeHeight (pBox, pSourceBox, NULL, height - pBox->BxHeight, frame);
	       }
	     else
		/* Mise a jour de la hauteur du contenu */
		pBox->BxRuleHeigth = height;
	  }
     }
}


/*----------------------------------------------------------------------
   ChangeDefaultWidth met a` jour la largeur du contenu de la boi^te 
   Cette proce'dure ve'rifie que la re`gle du minimum est  
   respecte'e. Eventuellement e'change largeur re'elle et  
   autre largeur.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ChangeDefaultWidth (PtrBox pBox, PtrBox pSourceBox, int width, int spaceDelta, int frame)
#else  /* __STDC__ */
void                ChangeDefaultWidth (pBox, pSourceBox, width, spaceDelta, frame)
PtrBox              pBox;
PtrBox              pSourceBox;
int                 width;
int                 spaceDelta;
int                 frame;

#endif /* __STDC__ */
{
   int                 delta;

   if (pBox != NULL)
     {
	/* Regarde si la largeur reelle actuelle depend du contenu */
	if (pBox->BxContentWidth)
	  {
	     /* La largeur reelle est egale a la largeur du contenu */
	     if (width < pBox->BxRuleWidth)
	       {
		  /* Il faut echanger la largeur reelle avec l'autre largeur */
		  delta = pBox->BxRuleWidth - pBox->BxWidth;
		  pBox->BxRuleWidth = width;
		  pBox->BxContentWidth = !pBox->BxContentWidth;
		  if (delta != 0)
		     ResizeWidth (pBox, pSourceBox, NULL, delta, spaceDelta, frame);
	       }
	     else
		/* Mise a jour de la largeur du contenu */
		ResizeWidth (pBox, pSourceBox, NULL, width - pBox->BxWidth, spaceDelta, frame);
	  }
	else if (!pBox->BxAbstractBox->AbWidth.DimIsPosition && pBox->BxAbstractBox->AbWidth.DimMinimum)
	  {
	     /* La largeur reelle est egale au minimum */
	     if (width > pBox->BxWidth)
	       {
		  /* Il faut echanger la largeur reelle avec l'autre largeur */
		  pBox->BxRuleWidth = pBox->BxWidth;
		  pBox->BxContentWidth = !pBox->BxContentWidth;
		  ResizeWidth (pBox, pSourceBox, NULL, width - pBox->BxWidth, spaceDelta, frame);
	       }
	     else
		/* Mise a jour de la largeur du contenu */
		pBox->BxRuleWidth = width;
	  }
     }
}


/*----------------------------------------------------------------------
   ChangeWidth propage la modification sur la largeur de la    
   boi^te pBox. Cette proce'dure ve'rifie la re`gle du     
   minimum. La largeur re'elle ou l'autre largeur de la    
   boi^te sera modifie'e.                                  
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ChangeWidth (PtrBox pBox, PtrBox pSourceBox, PtrBox pFromBox, int delta, int spaceDelta, int frame)
#else  /* __STDC__ */
void                ChangeWidth (pBox, pSourceBox, pFromBox, delta, spaceDelta, frame)
PtrBox              pBox;
PtrBox              pSourceBox;
PtrBox              pFromBox;
int                 delta;
int                 spaceDelta;
int                 frame;
#endif /* __STDC__ */
{
  int                 width;
  ThotBool            minimumRule;


  if (pBox != NULL)
    {
      minimumRule = (!pBox->BxAbstractBox->AbWidth.DimIsPosition
		     && pBox->BxAbstractBox->AbWidth.DimMinimum);

      if (minimumRule && IsParentBox (pFromBox, pBox)
	  && pFromBox->BxContentWidth)
	/* If the minimum size depends on an enclosing box and the current
	 * size of this box depends on the content size,
	 * don't change the value
	 */
	return;

      /* Regarde si la largeur reelle actuelle depend du contenu */
      if (pBox->BxContentWidth)
	{
	  /* Il y a une regle de minimum mais ce n'est la largeur reelle */
	  width = pBox->BxRuleWidth + delta;
	  if (width > pBox->BxWidth)
	    {
	      /* Il faut echanger la largeur reelle avec l'autre largeur */
	      pBox->BxRuleWidth = pBox->BxWidth;
	      pBox->BxContentWidth = !pBox->BxContentWidth;
	      ResizeWidth (pBox, pSourceBox, pFromBox, width - pBox->BxWidth, spaceDelta, frame);
	    }
	  else
	    /* Mise a jour de la largeur minimum */
	    pBox->BxRuleWidth = width;
	}
      else if (minimumRule)
	{
	  /* La largeur reelle est egale au minimum */
	  width = pBox->BxWidth + delta;
	  if (width < pBox->BxRuleWidth)
	    {
	      /* Il faut echanger la largeur reelle avec l'autre largeur */
	      width = pBox->BxRuleWidth;
	      pBox->BxRuleWidth = pBox->BxWidth + delta;
	      pBox->BxContentWidth = !pBox->BxContentWidth;
	      ResizeWidth (pBox, pSourceBox, pFromBox, width - pBox->BxWidth, spaceDelta, frame);
	    }
	  else
	    /* Mise a jour de la largeur minimum */
	    ResizeWidth (pBox, pSourceBox, pFromBox, delta, spaceDelta, frame);
	}
      else
	ResizeWidth (pBox, pSourceBox, pFromBox, delta, spaceDelta, frame);
    }
}


/*----------------------------------------------------------------------
   ChangeHeight propage la modification sur la hauteur de la      
   boi^te pBox. Cette proce'dure ve'rifie la re`gle du     
   minimum. La hauteur re'elle ou l'autre hauteur de la    
   boi^te sera modifie'e.                                  
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ChangeHeight (PtrBox pBox, PtrBox pSourceBox, PtrBox pFromBox, int delta, int frame)
#else  /* __STDC__ */
void                ChangeHeight (pBox, pSourceBox, pFromBox, delta, frame)
PtrBox              pBox;
PtrBox              pSourceBox;
PtrBox              pFromBox;
int                 delta;
int                 frame;
#endif /* __STDC__ */
{
  int                 height;
  ThotBool            minimumRule;

  if (pBox != NULL)
    {
      minimumRule = (!pBox->BxAbstractBox->AbHeight.DimIsPosition
		     && pBox->BxAbstractBox->AbHeight.DimMinimum);

      if (minimumRule && IsParentBox (pFromBox, pBox)
	  && pFromBox->BxContentHeight)
	/* If the minimum size depends on an enclosing box and the current
	 * size of this box depends on the content size,
	 * don't change the value
	 */
	return;

      /* Regarde si la hauteur reelle actuelle depend du contenu */
      if (pBox->BxContentHeight)
	{
	  /* Il y a une regle de minimum mais ce n'est la hauteur reelle */
	  height = pBox->BxRuleHeigth + delta;
	  if (height > pBox->BxHeight)
	    {
	      /* Il faut echanger la hauteur reelle avec l'autre hauteur */
	      pBox->BxRuleHeigth = pBox->BxHeight;
	      pBox->BxContentHeight = !pBox->BxContentHeight;
	      ResizeHeight (pBox, pSourceBox, pFromBox, height - pBox->BxHeight, frame);
	    }
	  else
	    /* Mise a jour de la hauteur minimum */
	    pBox->BxRuleHeigth = height;
	}
      else if (minimumRule)
	{
	  /* La hauteur courante est egale au minimum */
	  height = pBox->BxHeight + delta;
	  if (height < pBox->BxRuleHeigth)
	    {
	      /* Il faut echanger la hauteur reelle avec l'autre hauteur */
	      height = pBox->BxRuleHeigth;
	      pBox->BxRuleHeigth = pBox->BxHeight + delta;
	      pBox->BxContentHeight = !pBox->BxContentHeight;
	      ResizeHeight (pBox, pSourceBox, pFromBox, height - pBox->BxHeight, frame);
	    }
	  else
	    ResizeHeight (pBox, pSourceBox, pFromBox, delta, frame);
	}
      else
	ResizeHeight (pBox, pSourceBox, pFromBox, delta, frame);
    }
}


/*----------------------------------------------------------------------
   MoveBoxEdge inverse les reperes et eventuellement le dessin de   
   la boite elastique pBox quand la dimension devient      
   negative.                                               
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                MoveBoxEdge (PtrBox pBox, PtrBox pSourceBox, OpRelation op, int delta, int frame, ThotBool horizRef)
#else  /* __STDC__ */
void                MoveBoxEdge (pBox, pSourceBox, op, delta, frame, horizRef)
PtrBox              pBox;
PtrBox              pSourceBox;
OpRelation          op;
int                 delta;
int                 frame;
ThotBool            horizRef;

#endif /* __STDC__ */
{
   PtrAbstractBox      pAb;
   BoxEdge             oldPosEdge;
   int                 translation;

   pAb = pBox->BxAbstractBox;
   translation = 0;
   /* Il faut verifier que l'on pas deja en train de traiter cette boite */
   if (pAb != NULL && delta != 0 && pBox->BxPacking == 0)
     {
	/* indique que le traitement est en cours */
	pBox->BxPacking = 1;
	if (horizRef)
	  {
	     /* Recherche le repere deplace et le repere fixe de la boite elastique */
	     if (op == OpWidth)
		oldPosEdge = pAb->AbWidth.DimPosition.PosEdge;

	     else
	       {
		  oldPosEdge = pAb->AbHorizPos.PosEdge;
		  /* Met a jour la pile des boites pour ne pas deplacer pSourceBox */
		  if (pSourceBox == NULL)
		    pBox->BxMoved = pSourceBox;
		  else if (pSourceBox->BxMoved != pBox)
		    pBox->BxMoved = pSourceBox;
	       }

	     /* calcule le changement de largeur */
	     if (oldPosEdge == Left)
	       {
		  delta = -delta;
		  pBox->BxHorizEdge = Right;
	       }
	     else if (oldPosEdge == Right)
		pBox->BxHorizEdge = Left;
	     else if (oldPosEdge == VertMiddle)
	       {
		  delta = delta * 2;
		  if (op == OpHorizDep)
		     pBox->BxHorizEdge = pAb->AbWidth.DimPosition.PosEdge;
		  else
		     pBox->BxHorizEdge = pAb->AbHorizPos.PosEdge;
	       }

	     /* Faut-il echanger les reperes horizontaux ? */
	     if (delta < 0 && -delta > pBox->BxWidth)
	       {
		  /* Inversion de la boite en horizontal */
		  XEdgesExchange (pBox, op);

		  /* Translation de l'origine */
		  delta = -delta - 2 * pBox->BxWidth;
		  if (pBox->BxHorizEdge == Right)
		     translation = -pBox->BxWidth;
		  else if (pBox->BxHorizEdge == Left)
		     translation = pBox->BxWidth;
		  XMove (pBox, pSourceBox, translation, frame);
	       }
	     ResizeWidth (pBox, pSourceBox, NULL, delta, 0, frame);

	     /* retablit le point fixe */
	     pBox->BxHorizEdge = NoEdge;
	  }
	else
	  {
	     /* Recherche le repere deplace et le repere fixe de la boite elastique */
	     if (op == OpHeight)
		oldPosEdge = pAb->AbHeight.DimPosition.PosEdge;

	     else
	       {
		  oldPosEdge = pAb->AbVertPos.PosEdge;
		  /* Met a jour la pile des boites pour ne pas deplacer pSourceBox */
		  if (pSourceBox == NULL)
		    pBox->BxMoved = pSourceBox;
		  else if (pSourceBox->BxMoved != pBox)
		    pBox->BxMoved = pSourceBox;
	       }

	     /* calcule le changement de hauteur */
	     if (oldPosEdge == Top)
	       {
		  delta = -delta;
		  pBox->BxVertEdge = Bottom;
	       }
	     else if (oldPosEdge == Bottom)
		pBox->BxVertEdge = Top;
	     else if (oldPosEdge == HorizMiddle)
	       {
		  delta = delta * 2;
		  if (op == OpVertDep)
		     pBox->BxVertEdge = pAb->AbHeight.DimPosition.PosEdge;
		  else
		     pBox->BxVertEdge = pAb->AbVertPos.PosEdge;
	       }

	     /* Faut-il echanger les reperes verticaux ? */
	     if (delta < 0 && -delta > pBox->BxHeight)
	       {
		  /* Inversion de la boite en vertical */
		  YEdgesExchange (pBox, op);

		  /* Translation de l'origine */
		  delta = -delta - 2 * pBox->BxHeight;
		  if (pBox->BxVertEdge == Bottom)
		     translation = -pBox->BxHeight;
		  else if (pBox->BxVertEdge == Top)
		     translation = pBox->BxHeight;
		  YMove (pBox, pSourceBox, translation, frame);

	       }
	     ResizeHeight (pBox, pSourceBox, NULL, delta, frame);
	     /* retablit le point fixe */
	     pBox->BxVertEdge = NoEdge;
	  }
	/* indique que le traitement est termine */
	pBox->BxPacking = 0;
	/* retablit le chainage des deplacements */
	if (pSourceBox == NULL)
	  pBox->BxMoved = pSourceBox;
	else if (pSourceBox->BxMoved != pBox)
	  pBox->BxMoved = pSourceBox;
     }
}


/*----------------------------------------------------------------------
   XMoveAllEnclosed deplace horizontalement le contenu englobe de la 
   boite pBox.                                             
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                XMoveAllEnclosed (PtrBox pBox, int delta, int frame)
#else  /* __STDC__ */
void                XMoveAllEnclosed (pBox, delta, frame)
PtrBox              pBox;
int                 delta;
int                 frame;

#endif /* __STDC__ */
{
   PtrAbstractBox      pChildAb;
   PtrBox              pChildBox;
   PtrBox              pParentBox;
   BoxRelation        *pRelation;
   PtrPosRelations     pPosRel;
   int                 i;
   ThotBool            notEmpty;
   ThotBool            toHorizPack;

   if (pBox != NULL && (delta != 0 || pBox->BxXToCompute))
     {
	/* enregistre la hierarchie des boites dont le */
	/* traitement de l'englobement doit etre differe  */
	pParentBox = PackBoxRoot;
	if (pBox->BxAbstractBox->AbEnclosing != NULL)
	   PackBoxRoot = pBox /*->BxAbstractBox->AbEnclosing->AbBox*/ ;

	/* Si PackBoxRoot est une boite mere de la boite   */
	/* pFromBoxedemment designee on garde l'ancienne boite */
	if (IsParentBox (PackBoxRoot, pParentBox))
	   PackBoxRoot = pParentBox;

	/* Si la boite est mise en lignes */
	if (pBox->BxType == BoSplit)
	  {
	     /* decale les boites de coupure */
	     pChildBox = pBox->BxNexChild;
	     while (pChildBox != NULL)
	       {
		  pChildBox->BxXOrg += delta;
		  pChildBox = pChildBox->BxNexChild;
	       }
	  }

	/* analyse les relations pour savoir comment deplacer la boite */
	else if (pBox->BxAbstractBox != NULL)
	  {
	     /* La boite est elastique et n'est pas en cours de traitement */
	     if (pBox->BxHorizFlex && (!pBox->BxAbstractBox->AbLeafType == LtCompound || pBox->BxPacking == 0))
		 MoveBoxEdge (pBox, NULL, OpHorizDep, delta, frame, TRUE);
	     /* Dans les autres cas */
	     else
	       {
		  /* simple translation */
		  pBox->BxXOrg += delta;

		  /* Faut-il mettre a jour le rectangle de reaffichage ? */
		  if (!pBox->BxAbstractBox->AbHorizEnclosing && ReadyToDisplay)
		    {
		       /* Prend en compte une zone de debordement des graphiques */
		       if (pBox->BxAbstractBox->AbLeafType == LtGraphics)
			  i = EXTRA_GRAPH;
		       else
			  i = 0;
		       if (delta > 0)
			  DefClip (frame, pBox->BxXOrg - delta - i, pBox->BxYOrg - i,
				   pBox->BxXOrg + pBox->BxWidth + i, pBox->BxYOrg + pBox->BxHeight + i);
		       else
			  DefClip (frame, pBox->BxXOrg - i, pBox->BxYOrg - i,
				   pBox->BxXOrg + pBox->BxWidth - delta + i, pBox->BxYOrg + pBox->BxHeight + i);
		    }

		  /* decale les boites qui ont des relations hors-structure avec */
		  /* la boite deplacee et on met a jour les dimensions elastiques   */
		  /* des boites liees a la boite deplacee.                          */
		  pPosRel = pBox->BxPosRelations;
		  while (pPosRel != NULL)
		    {
		       i = 0;
		       notEmpty = pPosRel->PosRTable[i].ReBox != NULL;
		       while (i < MAX_RELAT_POS && notEmpty)
			 {
			    pRelation = &pPosRel->PosRTable[i];
			    if (pRelation->ReBox->BxAbstractBox != NULL)
			      {
				 /* Relation hors-struture sur l'origine de la boite */
				 if (pRelation->ReOp == OpHorizDep && pRelation->ReBox->BxXOutOfStruct
				     && pRelation->ReBox->BxAbstractBox->AbHorizPos.PosAbRef == pBox->BxAbstractBox)
				   {

				      /* refuse de deplacer une boite englobante de pBox */
				      if (IsParentBox (pRelation->ReBox, pBox)) ;

				      else if (pRelation->ReBox->BxHorizFlex)
					 MoveBoxEdge (pRelation->ReBox, pBox, pRelation->ReOp, delta, frame, TRUE);
				      else
					 XMove (pRelation->ReBox, pBox, delta, frame);

				   }
				 /* Relation sur la largeur elastique de la boite */
				 else if (pRelation->ReOp == OpWidth)
				    MoveBoxEdge (pRelation->ReBox, pBox, pRelation->ReOp, delta, frame, TRUE);
			      }

			    i++;
			    if (i < MAX_RELAT_POS)
			       notEmpty = (pPosRel->PosRTable[i].ReBox != NULL);
			 }
		       /* next block */
		       pPosRel = pPosRel->PosRNext;
		    }

		  /* Decale des boites englobees dont l'origine depend de l'englobante */
		  pChildAb = pBox->BxAbstractBox->AbFirstEnclosed;
		  /* Traite le niveau inferieur */
		  toHorizPack = FALSE;
		  /* Si la boite est en cours de deplacement -> il faut transmettre */
		  /* la valeur de l'origine plutot que la valeur du decalage        */
		  if (pBox->BxXToCompute)
		     delta = pBox->BxXOrg;

		  while (pChildAb != NULL)
		    {
		       if (pChildAb->AbBox != NULL)
			  if (pChildAb->AbBox->BxXOutOfStruct)
			     toHorizPack = TRUE;
			  else
			    {
			       if (pBox->BxXToCompute && !pChildAb->AbBox->BxHorizFlex)
				  /* Additionne le decalage de la boite */
				  pChildAb->AbBox->BxXToCompute = TRUE;
			       XMoveAllEnclosed (pChildAb->AbBox, delta, frame);
			    }
		       pChildAb = pChildAb->AbNext;
		    }
		  pBox->BxXToCompute = FALSE;	/* le decalage eventuel est pris en compte */

		  /* Si la largeur de la boite depend du contenu et qu'une des     */
		  /* boites filles est positionnee par une relation hors-structure */
		  /* --> il faut reevaluer la largeur correspondante.              */
		  if (toHorizPack &&
		      (pBox->BxContentWidth || (!pBox->BxAbstractBox->AbWidth.DimIsPosition && pBox->BxAbstractBox->AbWidth.DimMinimum)))
		     RecordEnclosing (pBox, TRUE);
	       }
	  }
	/* restaure */
	PackBoxRoot = pParentBox;
     }
}


/*----------------------------------------------------------------------
   YMoveAllEnclosed deplace verticalement tout le contenu englobe   
   de la boite pBox.                                    
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                YMoveAllEnclosed (PtrBox pBox, int delta, int frame)
#else  /* __STDC__ */
void                YMoveAllEnclosed (pBox, delta, frame)
PtrBox              pBox;
int                 delta;
int                 frame;

#endif /* __STDC__ */
{
   PtrAbstractBox      pChildAb;
   PtrBox              pChildBox;
   PtrBox              pParentBox;
   BoxRelation        *pRelation;
   PtrPosRelations     pPosRel;
   int                 i;
   ThotBool            notEmpty;
   ThotBool            toVertPack;

   if (pBox != NULL && (delta != 0 || pBox->BxYToCompute))
     {
	/* enregistre la hierarchie des boites dont le */
	/* traitement de l'englobement doit etre differe  */
	pParentBox = PackBoxRoot;
	if (pBox->BxAbstractBox->AbEnclosing != NULL)
	   PackBoxRoot = pBox /*->BxAbstractBox->AbEnclosing->AbBox*/ ;

	/* Si PackBoxRoot est une boite mere de la boite   */
	/* pFromBoxedemment designee on garde l'ancienne boite */
	if (IsParentBox (PackBoxRoot, pParentBox))
	   PackBoxRoot = pParentBox;

	/* Si la boite est mise en lignes */
	if (pBox->BxType == BoSplit)
	  {
	     /* decale les boites de coupure */
	     pChildBox = pBox->BxNexChild;
	     while (pChildBox != NULL)
	       {
		  pChildBox->BxYOrg += delta;
		  pChildBox = pChildBox->BxNexChild;
	       }

	  }

	/* analyse les relations pour savoir comment deplacer la boite */
	else if (pBox->BxAbstractBox != NULL)
	  {
	    /*-> La boite est elastique et n'est pas en cours de traitement */
	     if (pBox->BxVertFlex && (!pBox->BxAbstractBox->AbLeafType == LtCompound || pBox->BxPacking == 0))
		 MoveBoxEdge (pBox, NULL, OpVertDep, delta, frame, FALSE);
	     /*-> Dans les autres cas */
	     else
	       {
		  /* simple translation */
		  pBox->BxYOrg += delta;

		  /* Faut-il mettre a jour le rectangle de reaffichage ? */
		  if (!pBox->BxAbstractBox->AbVertEnclosing && ReadyToDisplay)
		    {
		       /* Prend en compte une zone de debordement des graphiques */
		       if (pBox->BxAbstractBox->AbLeafType == LtGraphics)
			  i = EXTRA_GRAPH;
		       else
			  i = 0;
		       if (delta > 0)
			  DefClip (frame, pBox->BxXOrg - i, pBox->BxYOrg - delta - i,
				   pBox->BxXOrg + pBox->BxWidth + i, pBox->BxYOrg + pBox->BxHeight + i);
		       else
			  DefClip (frame, pBox->BxXOrg - i, pBox->BxYOrg - i,
				   pBox->BxXOrg + pBox->BxWidth + i,
				 pBox->BxYOrg + pBox->BxHeight - delta + i);
		    }

		  /* decale les boites qui ont des relations hors-structure avec */
		  /* la boite deplacee et on met a jour les dimensions elastiques   */
		  /* des boites liees a la boite deplacee.                          */
		  pPosRel = pBox->BxPosRelations;
		  while (pPosRel != NULL)
		    {
		       i = 0;
		       notEmpty = pPosRel->PosRTable[i].ReBox != NULL;
		       while (i < MAX_RELAT_POS && notEmpty)
			 {
			    pRelation = &pPosRel->PosRTable[i];
			    if (pRelation->ReBox->BxAbstractBox != NULL)
			      {
				 /* Relation hors-struture sur l'origine de la boite */
				 if (pRelation->ReOp == OpVertDep
				     && pRelation->ReBox->BxYOutOfStruct
				     && pRelation->ReBox->BxAbstractBox->AbVertPos.PosAbRef == pBox->BxAbstractBox)
				   {

				      /* refuse de deplacer une boite englobante de pBox */
				      if (IsParentBox (pRelation->ReBox, pBox))
					 ;
				      else if (pRelation->ReBox->BxVertFlex)
					 MoveBoxEdge (pRelation->ReBox, pBox, pRelation->ReOp, delta, frame, FALSE);
				      else
					 YMove (pRelation->ReBox, pBox, delta, frame);

				   }
				 /* Relation sur la hauteur elastique de la boite */
				 else if (pRelation->ReOp == OpHeight)
				    MoveBoxEdge (pRelation->ReBox, pBox, pRelation->ReOp, delta, frame, FALSE);
			      }
			    i++;
			    if (i < MAX_RELAT_POS)
			       notEmpty = pPosRel->PosRTable[i].ReBox != NULL;
			 }
		       /* next block */
		       pPosRel = pPosRel->PosRNext;
		    }

		  /* traite les fils englobes ou places par rapport a l'englobante */
		  /* Traite le niveau inferieur */
		  pChildAb = pBox->BxAbstractBox->AbFirstEnclosed;
		  toVertPack = FALSE;
		  /* Si la boite est en cours de deplacement -> il faut transmettre */
		  /* la valeur de l'origine plutot que la valeur du decalage        */
		  if (pBox->BxYToCompute)
		     delta = pBox->BxYOrg;
		  while (pChildAb != NULL)
		    {
		       if (pChildAb->AbBox != NULL)
			 {
			    if (pChildAb->AbBox->BxYOutOfStruct)
			       toVertPack = TRUE;
			    else
			      {
				 if (pBox->BxYToCompute && !pChildAb->AbBox->BxVertFlex)
				    /* Additionne le decalage de la boite */
				    pChildAb->AbBox->BxYToCompute = TRUE;
				 YMoveAllEnclosed (pChildAb->AbBox, delta, frame);
			      }
			 }
		       pChildAb = pChildAb->AbNext;
		    }
		  pBox->BxYToCompute = FALSE;	/* le decalage eventuel est pris en compte */

		  /* Si la hauteur de la boite depend du contenu et qu'une des     */
		  /* boites filles est positionnee par une relation hors-structure */
		  /* --> il faut reevaluer la hauteur correspondante.              */
		  if (toVertPack &&
		      (pBox->BxContentHeight || (!pBox->BxAbstractBox->AbHeight.DimIsPosition && pBox->BxAbstractBox->AbHeight.DimMinimum)))
		     RecordEnclosing (pBox, FALSE);
	       }
	  }
	/* restaure */
	PackBoxRoot = pParentBox;
     }
}


/*----------------------------------------------------------------------
   MoveVertRef deplace l'axe de reference de la boite pBox dans la  
   frametre frame et les boites qui en dependent.      
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                MoveVertRef (PtrBox pBox, PtrBox pFromBox, int delta, int frame)
#else  /* __STDC__ */
void                MoveVertRef (pBox, pFromBox, delta, frame)
PtrBox              pBox;
PtrBox              pFromBox;
int                 delta;
int                 frame;

#endif /* __STDC__ */
{
   int                 i, j, k;
   PtrAbstractBox      pAb;
   PtrAbstractBox      pCurrentAb;
   PtrBox              pNextBox;
   PtrBox              pRefBox;
   PtrPosRelations     pPosRel;
   BoxRelation        *pRelation;
   ThotBool            toMove;
   ThotBool            notEmpty;

   if (pBox != NULL && delta != 0)
     {
	if (pBox->BxAbstractBox != NULL)
	   if (!pBox->BxAbstractBox->AbDead)
	     {
		/* Verifie que le deplacement n'est pas deja traite */
		toMove = TRUE;
		pNextBox = pFromBox;
		pRefBox = pBox;
		while (pNextBox != NULL)
		   if (pNextBox == pBox)
		     {
			pNextBox = NULL;
			toMove = FALSE;
		     }
		   else
		     {
			if (pNextBox->BxMoved == NULL)
			   /* boite ancetre */
			   pRefBox = pNextBox;
			pNextBox = pNextBox->BxMoved;
		     }

		if (toMove)
		  {
		     pBox->BxVertRef += delta;
		     pCurrentAb = pBox->BxAbstractBox;
		     /* met a jour la pile des boites traitees */
		     pBox->BxMoved = pFromBox;
		     /* regarde si les regles de dependance sont valides */
		     if (pCurrentAb->AbEnclosing != NULL)
			if (pCurrentAb->AbEnclosing->AbBox != NULL)
			   toMove = pCurrentAb->AbEnclosing->AbBox->BxType != BoGhost;

		     if (toMove)

			if (pBox->BxHorizEdge == VertRef)
			  {
			     delta = -delta;
			     toMove = TRUE;	/* Il faut verifier l'englobement */
			     /* evalue la partie de la fenetre a reafficher */
			     if (ReadyToDisplay && pBox->BxType != BoSplit)
			       {
				  i = pBox->BxXOrg;
				  j = pBox->BxXOrg + pBox->BxWidth;
				  /* Prend en compte une zone de debordement des graphiques */
				  if (pBox->BxAbstractBox->AbLeafType == LtGraphics)
				     k = EXTRA_GRAPH;
				  else
				     k = 0;
				  if (delta > 0)
				     j += delta;
				  else
				     i += delta;
				  DefClip (frame, i - k, pBox->BxYOrg - k, j + k, pBox->BxYOrg + pBox->BxHeight + k);
			       }

			     if (IsXPosComplete (pBox))
			       {
				  XMoveAllEnclosed (pBox, delta, frame);
				  /* a pu detruire le chainage des boites deplacees */
				  pBox->BxMoved = pFromBox;
			       }
			     else
				pBox->BxXOrg += delta;

			     /* decale aussi les boites liees a la boite deplacee */
			     pPosRel = pBox->BxPosRelations;
			     while (pPosRel != NULL)
			       {
				  i = 0;
				  notEmpty = (pPosRel->PosRTable[i].ReBox != NULL);
				  while (i < MAX_RELAT_POS && notEmpty)
				    {
				       pRelation = &pPosRel->PosRTable[i];
				       if (pRelation->ReBox->BxAbstractBox != NULL)
					 {
					    /* cote gauche */
					    /* cote droit */
					    /* milieu vertical */
					    if (pRelation->ReBox != pBox
						&& pRelation->ReRefEdge != VertRef)
					       if (pRelation->ReOp == OpHorizRef)
						  MoveVertRef (pRelation->ReBox, pBox, delta, frame);
					       else if ((pRelation->ReOp == OpHorizDep
							 && pRelation->ReBox->BxAbstractBox->AbWidth.DimIsPosition)
					       || pRelation->ReOp == OpWidth)
						  MoveBoxEdge (pRelation->ReBox, pBox, pRelation->ReOp, delta, frame, TRUE);
					       else if (pRelation->ReOp == OpHorizDep)
						  XMove (pRelation->ReBox, pBox, delta, frame);
					 }

				       i++;
				       if (i < MAX_RELAT_POS)
					  notEmpty = (pPosRel->PosRTable[i].ReBox != NULL);
				    }
				  /* next block */
				  pPosRel = pPosRel->PosRNext;
			       }
			  }

		     /* Sinon, on deplace les boites liees a l'axe de reference */
			else
			  {
			     /* Deplacement de boites voisines */
			     toMove = FALSE;
			     pPosRel = pBox->BxPosRelations;
			     while (pPosRel != NULL)
			       {
				  i = 0;
				  notEmpty = (pPosRel->PosRTable[i].ReBox != NULL);
				  while (i < MAX_RELAT_POS && notEmpty)
				    {
				       pRelation = &pPosRel->PosRTable[i];
				       if (pRelation->ReBox->BxAbstractBox != NULL)
					 {
					    /* vertical reference */
					    if (pRelation->ReRefEdge == VertRef)
					       if (pRelation->ReOp == OpHorizRef)
						  MoveVertRef (pRelation->ReBox, pFromBox, delta, frame);
					       else if ((pRelation->ReOp == OpHorizDep
							 && pRelation->ReBox->BxAbstractBox->AbWidth.DimIsPosition)
					       || pRelation->ReOp == OpWidth)
						  MoveBoxEdge (pRelation->ReBox, pBox, pRelation->ReOp, delta, frame, TRUE);
					       else if (pRelation->ReOp == OpHorizDep)
						 {
						    XMove (pRelation->ReBox, pBox, delta, frame);
						    /* verify enclosing */
						    toMove = TRUE;
						 }
					 }

				       i++;
				       if (i < MAX_RELAT_POS)
					  notEmpty = (pPosRel->PosRTable[i].ReBox != NULL);
				    }
				  /* next block */
				  pPosRel = pPosRel->PosRNext;
			       }

			     /* deplace des boites incluses */
			     pNextBox = NULL;
			     if (IsXPosComplete (pBox) && pBox->BxType != BoBlock)
			       {
				  pAb = pCurrentAb->AbFirstEnclosed;
				  while (pAb != NULL)
				    {
				       if (!pAb->AbDead
					   && pAb->AbHorizPos.PosAbRef == pCurrentAb
					   && pAb->AbHorizPos.PosRefEdge == VertRef)
					 {
					    pNextBox = pAb->AbBox;
					    XMove (pNextBox, pBox, delta, frame);
					 }
				       pAb = pAb->AbNext;
				    }

				  /* Faut-il reevaluer l'englobement de la boite pBox ? */
				  /*et si ce n'est pas une boite fille de pBox */
				  if (pNextBox != NULL && Propagate == ToAll)
				     WidthPack (pCurrentAb, pRefBox, frame);
			       }
			  }

		     /* respecte les contraintes d'englobement */
		     pAb = pCurrentAb->AbEnclosing;
		     if (toMove)
			if (pAb == NULL)
			  {
			     if (Propagate == ToAll && pBox->BxXOrg < 0)
				XMoveAllEnclosed (pBox, -pBox->BxXOrg, frame);
			  }
		     /* Verifie l'englobement des boites de la hierarchie voisine */
		     /* sauf si l'englobement des boites doit etre differe        */
			else if (!pAb->AbInLine
				 && pAb->AbBox->BxType != BoGhost
				 && !IsParentBox (pAb->AbBox, pRefBox)
				 && !IsParentBox (pAb->AbBox, PackBoxRoot))
			   WidthPack (pAb, pRefBox, frame);
		  }
	     }
     }
}

/*----------------------------------------------------------------------
   MoveHorizRef deplace l'axe de reference de la boite pBox dans la 
   frame frame et les boites qui en dependent.         
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                MoveHorizRef (PtrBox pBox, PtrBox pFromBox, int delta, int frame)
#else  /* __STDC__ */
void                MoveHorizRef (pBox, pFromBox, delta, frame)
PtrBox              pBox;
PtrBox              pFromBox;
int                 delta;
int                 frame;

#endif /* __STDC__ */
{
   int                 i, j, k;
   PtrAbstractBox      pAb;
   PtrAbstractBox      pCurrentAb;
   PtrBox              pNextBox;
   PtrBox              pRefBox;
   PtrPosRelations     pPosRel;
   BoxRelation        *pRelation;
   ThotBool            toMove;
   ThotBool            notEmpty;

   if (pBox != NULL && delta != 0)
     {
	if (pBox->BxAbstractBox != NULL)
	   if (!pBox->BxAbstractBox->AbDead)
	     {
		/* Verifie que le deplacement n'est pas deja traite */
		toMove = TRUE;
		pNextBox = pFromBox;
		pRefBox = pBox;
		while (pNextBox != NULL)
		   if (pNextBox == pBox)
		     {
			pNextBox = NULL;
			toMove = FALSE;
		     }
		   else
		     {
			if (pNextBox->BxMoved == NULL)
			   pRefBox = pNextBox;	/* boite ancetre */
			pNextBox = pNextBox->BxMoved;
		     }

		if (toMove)
		  {
		     pBox->BxHorizRef += delta;
		     pCurrentAb = pBox->BxAbstractBox;
		     /* met a jour la pile des boites traitees */
		     pBox->BxMoved = pFromBox;
		     /* regarde si les regles de dependance sont valides */
		     if (pCurrentAb->AbEnclosing != NULL)
			if (pCurrentAb->AbEnclosing->AbBox != NULL)
			   toMove = pCurrentAb->AbEnclosing->AbBox->BxType != BoGhost;

		     /* Si le point fixe de la boite est l'axe de reference -> deplace */
		     if (toMove)

			if (pBox->BxVertEdge == HorizRef)
			  {
			     delta = -delta;
			     toMove = TRUE;	/* Il faut verifier l'englobement */
			     /* evalue la partie de la fenetre a reafficher */
			     if (ReadyToDisplay && pBox->BxType != BoSplit)
			       {
				  i = pBox->BxYOrg;
				  j = pBox->BxYOrg + pBox->BxHeight;
				  /* Prend en compte une zone de debordement des graphiques */
				  if (pBox->BxAbstractBox->AbLeafType == LtGraphics)
				     k = EXTRA_GRAPH;
				  else
				     k = 0;
				  if (delta > 0)
				     j += delta;
				  else
				     i += delta;
				  DefClip (frame, pBox->BxXOrg - k, i - k, pBox->BxXOrg + pBox->BxWidth + k, j + k);
			       }

			     if (IsYPosComplete (pBox))
			       {
				  YMoveAllEnclosed (pBox, delta, frame);
				  /* a pu detruire le chainage des boites deplacees */
				  pBox->BxMoved = pFromBox;
			       }
			     else
				pBox->BxYOrg += delta;

			     /* decale aussi les boites liees a la boite deplacee */
			     pPosRel = pBox->BxPosRelations;
			     while (pPosRel != NULL)
			       {
				  i = 0;
				  notEmpty = pPosRel->PosRTable[i].ReBox != NULL;
				  while (i < MAX_RELAT_POS && notEmpty)
				    {
				       pRelation = &pPosRel->PosRTable[i];
				       if (pRelation->ReBox->BxAbstractBox != NULL)
					 {
					    /* cote superieur */
					    /* cote inferieur */
					    /* milieu horizontal */
					    if (pRelation->ReBox != pBox && pRelation->ReRefEdge != HorizRef)
					       if (pRelation->ReOp == OpVertRef)
						  MoveHorizRef (pRelation->ReBox, pBox, delta, frame);
					       else if ((pRelation->ReOp == OpVertDep
							 && pRelation->ReBox->BxAbstractBox->AbHeight.DimIsPosition)
							|| pRelation->ReOp == OpHeight)
						  MoveBoxEdge (pRelation->ReBox, pBox, pRelation->ReOp, delta, frame, FALSE);
					       else if (pRelation->ReOp == OpVertDep)
						  YMove (pRelation->ReBox, pBox, delta, frame);
					 }

				       i++;
				       if (i < MAX_RELAT_POS)
					  notEmpty = pPosRel->PosRTable[i].ReBox != NULL;
				    }
				  pPosRel = pPosRel->PosRNext;
			       }
			  }

		     /* Sinon on deplace les boites liees a l'axe de reference */
			else
			  {
			     /* Deplacement de boites voisines ? */
			     toMove = FALSE;
			     pPosRel = pBox->BxPosRelations;
			     while (pPosRel != NULL)
			       {
				  i = 0;
				  notEmpty = pPosRel->PosRTable[i].ReBox != NULL;
				  while (i < MAX_RELAT_POS && notEmpty)
				    {
				       pRelation = &pPosRel->PosRTable[i];
				       if (pRelation->ReBox->BxAbstractBox != NULL)
					 {
					    /* reference horizontale */
					    if (pRelation->ReRefEdge == HorizRef)
					       if (pRelation->ReOp == OpVertRef)
						  MoveHorizRef (pRelation->ReBox, pFromBox, delta, frame);
					       else if ((pRelation->ReOp == OpVertDep
							 && pRelation->ReBox->BxAbstractBox->AbHeight.DimIsPosition)
							|| pRelation->ReOp == OpHeight)
						  MoveBoxEdge (pRelation->ReBox, pBox, pRelation->ReOp, delta, frame, FALSE);
					       else if (pRelation->ReOp == OpVertDep)
						 {
						    /* deplace une voisine */
						    YMove (pRelation->ReBox, pBox, delta, frame);
						    /* verify enclosing */
						    toMove = TRUE;
						 }
					 }

				       i++;
				       if (i < MAX_RELAT_POS)
					  notEmpty = pPosRel->PosRTable[i].ReBox != NULL;
				    }
				  /* next block */
				  pPosRel = pPosRel->PosRNext;
			       }

			     /* Deplacement de boites incluses ? */
			     pNextBox = NULL;
			     if (IsYPosComplete (pBox) && pBox->BxType != BoBlock)
			       {
				  pAb = pCurrentAb->AbFirstEnclosed;
				  while (pAb != NULL)
				    {
				       if (!pAb->AbDead
					   && pAb->AbVertPos.PosAbRef == pCurrentAb
					   && pAb->AbVertPos.PosRefEdge == HorizRef)
					 {
					    pNextBox = pAb->AbBox;
					    YMove (pNextBox, pBox, delta, frame);
					 }
				       pAb = pAb->AbNext;
				    }

				  /* Faut-il reevaluer l'englobement de la boite pBox ? */
				  /* et si ce n'est pas une boite fille de pBox */
				  if (pNextBox != NULL && Propagate == ToAll)
				     HeightPack (pCurrentAb, pRefBox, frame);
			       }
			  }

		     /* respecte les contraintes d'englobement */
		     pAb = pCurrentAb->AbEnclosing;
		     if (toMove)
			if (pAb == NULL)
			  {
			     if (Propagate == ToAll && pBox->BxYOrg < 0)
				YMoveAllEnclosed (pBox, -pBox->BxYOrg, frame);
			  }
			else if (pAb->AbInLine
				 || pAb->AbBox->BxType == BoGhost)
			  {
			     if (Propagate == ToAll)
				EncloseInLine (pBox, frame, pAb);
			  }
		     /* Verifie l'englobement des boites de la hierarchie voisine */
		     /* sauf si l'englobement des boites doit etre differe        */
			else if (!IsParentBox (pAb->AbBox, pRefBox)
				 && !IsParentBox (pAb->AbBox, PackBoxRoot))
			   HeightPack (pAb, pRefBox, frame);
		  }
	     }
     }
}


/*----------------------------------------------------------------------
   ResizeWidth modifie la largeur de la boite pBox correspondant au  
   frame suite au changement de largeur de la boite            
   origine pSourceBox.                                         
   respecte les contraintes de position :                      
   - Toute boite liee a` un des cotes deplaces est             
   deplacee.                                                 
   - On met a` jour la base de la boite pBox si necessaire.    
   On respecte les contraintes de dimension :                  
   - On met a` jour les largeurs de boites qui en              
   dependent.                                                
   Le parametre spaceDelta correspond au nombre de caracteres  
   blanc ajoutes (>0) ou retires (<0). Il n'a de sens que      
   quand la boite texte appartient a` une ligne justifiee.     
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ResizeWidth (PtrBox pBox, PtrBox pSourceBox, PtrBox pFromBox, int delta, int spaceDelta, int frame)
#else  /* __STDC__ */
void                ResizeWidth (pBox, pSourceBox, pFromBox, delta, spaceDelta, frame)
PtrBox              pBox;
PtrBox              pSourceBox;
PtrBox              pFromBox;
int                 delta;
int                 spaceDelta;
int                 frame;

#endif /* __STDC__ */
{
  PtrBox              pNextBox;
  PtrLine             pLine;
  PtrAbstractBox      pAb;
  PtrAbstractBox      pCurrentAb;
  PtrPosRelations     pPosRel;
  PtrDimRelations     pDimRel;
  BoxRelation        *pRelation;
  ViewSelection      *pViewSel;
  int                 i, j, k;
  int                 orgTrans, middleTrans, endTrans;
  ThotBool            notEmpty;
  ThotBool            toMove;
  ThotBool            absoluteMove;
  ThotBool            externalRef;
  
  if (pBox != NULL && delta != 0)
    {
      /* Faut-il nettoyer la file des boites deplacees */
      if (pSourceBox == NULL && pFromBox == NULL)
	pBox->BxMoved = NULL;
      
      if (pBox->BxAbstractBox != NULL)
	if (!pBox->BxAbstractBox->AbDead)
	  {
	    /* verifie que la largeur d'une boite ne devient pas negative */
	    /*if (Propagate != ToSiblings && delta < 0 && -delta > pBox->BxWidth)
	      delta = -pBox->BxWidth;*/
	    /* Valeurs limites avant deplacement */
	    i = pBox->BxXOrg;
	    j = i + pBox->BxWidth;
	    pCurrentAb = pBox->BxAbstractBox;
	    /* Ce n'est pas une boite elastique: RAZ sur file boites deplacees */
	    if (!pBox->BxHorizFlex)
	      pBox->BxMoved = NULL;
	    
	    /* Force la reevaluation des points de controle de la polyline */
	    if (pCurrentAb->AbLeafType == LtPolyLine && pBox->BxPictInfo != NULL)
	      {
		/* libere les points de controle */
		free ((STRING) pBox->BxPictInfo);
		pBox->BxPictInfo = NULL;
	      }
	    
	    /* regarde si les regles de dependance sont valides */
	    toMove = TRUE;
	    if (pCurrentAb->AbEnclosing != NULL && pCurrentAb->AbEnclosing->AbBox != NULL)
		toMove = (pCurrentAb->AbEnclosing->AbBox->BxType != BoGhost &&
			  pCurrentAb->AbEnclosing->AbBox->BxType != BoBlock);
	    
	    /* respecte les contraintes de position */
	    /* Point fixe sur l'origine */
	    if (!toMove || pBox->BxHorizEdge == Left || pBox->BxHorizEdge == VertRef)
	      {
		/* Mise a jour du reaffichage */
		/* Valeurs de deplacements des reperes */
		orgTrans = 0;
		middleTrans = (pBox->BxWidth + delta) / 2 - pBox->BxWidth / 2;
		endTrans = delta;
	      }
	    /* Point fixe sur le milieu */
	    else if (pBox->BxHorizEdge == VertMiddle)
	      {
		/* L'origine de la boite et le cote inferieur sont deplaces */
		orgTrans = pBox->BxWidth / 2 - (pBox->BxWidth + delta) / 2;
		endTrans = delta + orgTrans;
		/* corrige les erreurs d'arrondi */
		middleTrans = 0;
	      }
	    /* Point fixe sur le cote droit */
	    else
	      {
		/* L'origine de la boite et le milieu sont deplaces */
		orgTrans = -delta;
		middleTrans = pBox->BxWidth / 2 - (pBox->BxWidth + delta) / 2;
		endTrans = 0;
	      }
	    pBox->BxWidth += delta;
	    pBox->BxXOrg += orgTrans;
	    
	    /* evalue la partie de la fenetre a reafficher */
	    if (ReadyToDisplay
		&& pBox->BxType != BoSplit
		/* Il ne faut pas tenir compte de la boite si elle */
		/* n'est pas encore placee dans l'image concrete   */
		&& !pBox->BxXToCompute
		&& !pBox->BxYToCompute
		&& (orgTrans != 0
		    || pCurrentAb->AbFirstEnclosed == NULL
		    /* redisplay filled boxes */
		    || pCurrentAb->AbFillBox
		    || pCurrentAb->AbPictBackground != NULL))
	      {
		if (pCurrentAb->AbLeafType == LtText)
		  {
		    k = 0;
		    if (orgTrans == 0)
		      i = j;
		    else if (orgTrans < 0)
		      i += orgTrans;

		    if (endTrans == 0)
		      j = i;
		    else if (endTrans > 0)
		      j += endTrans;
		  }
		else
		  {
		    /* Prend en compte une zone de debordement des graphiques */
		    if (pBox->BxAbstractBox->AbLeafType == LtGraphics)
		      k = EXTRA_GRAPH;
		    else
		      k = 0;
		    if (orgTrans < 0)
		      i += orgTrans;
		    if (endTrans > 0)
		      j += endTrans;
		  }

		DefClip (frame, i - k, pBox->BxYOrg - k, j + k, pBox->BxYOrg + pBox->BxHeight + k);
	      }
	    
	    /* Deplacement de boites voisines et de l'englobante ? */
	    pPosRel = pBox->BxPosRelations;
	    while (pPosRel != NULL)
	      {
		i = 0;
		notEmpty = pPosRel->PosRTable[i].ReBox != NULL;
		while (i < MAX_RELAT_POS && notEmpty)
		  {
		    pRelation = &pPosRel->PosRTable[i];
		    if (pRelation->ReBox->BxAbstractBox != NULL)
		      /* Ignore la relation inverse de la boite elastique */
		      if (!pBox->BxHorizFlex
			  || pRelation->ReOp != OpHorizDep
			  || pCurrentAb == pRelation->ReBox->BxAbstractBox->AbHorizPos.PosAbRef)
			switch (pRelation->ReRefEdge)
			  {
			    /* cote gauche */
			  case Left:
			    if (pRelation->ReOp == OpHorizRef)
			      {
				if (pRelation->ReBox != pBox)
				  MoveVertRef (pRelation->ReBox, pSourceBox, orgTrans, frame);
			      }
			     else if (toMove)
			       {
				 if (pRelation->ReOp == OpHorizInc)
				   {
				     if (!pBox->BxHorizFlex)
				       XMove (pBox, NULL, -orgTrans, frame);
				   }
				 else if ((pRelation->ReOp == OpHorizDep && pRelation->ReBox->BxHorizFlex)
					  || pRelation->ReOp == OpWidth)
				   MoveBoxEdge (pRelation->ReBox, pBox, pRelation->ReOp, orgTrans, frame, TRUE);
				 else if (pRelation->ReBox != pSourceBox)
				   XMove (pRelation->ReBox, pBox, orgTrans, frame);
			       }
			    break;
			    /* milieu vertical */
			  case VertMiddle:
			    if (pRelation->ReOp == OpHorizRef)
			      {
				if (pRelation->ReBox == pBox)
				  {
				    MoveVertRef (pBox, NULL, delta / 2, frame);
				    pBox->BxMoved = pFromBox;	/* restaure le chainage */
				  }
				else
				  MoveVertRef (pRelation->ReBox, pSourceBox, middleTrans, frame);
			      }
			     else if (toMove)
			       {
				 if (pRelation->ReOp == OpHorizInc)
				   {
				     if (!pBox->BxHorizFlex)
				       XMove (pBox, NULL, -middleTrans, frame);
				   }
				 else if ((pRelation->ReOp == OpHorizDep && pRelation->ReBox->BxHorizFlex)
					  || pRelation->ReOp == OpWidth)
				   MoveBoxEdge (pRelation->ReBox, pBox, pRelation->ReOp, middleTrans, frame, TRUE);
				 else if (pRelation->ReBox != pSourceBox)
				   XMove (pRelation->ReBox, pBox, middleTrans, frame);
			       }
			    break;
			    /* cote droit */
			  case Right:
			    if (pRelation->ReOp == OpHorizRef)
			      {
				if (pRelation->ReBox == pBox)
				  {
				    MoveVertRef (pBox, NULL, delta, frame);
				    pBox->BxMoved = pFromBox;	/* restaure le chainage */
				  }
				else
				  MoveVertRef (pRelation->ReBox, pSourceBox, endTrans, frame);
			      }
			     else if (toMove)
			       {
				 if (pRelation->ReOp == OpHorizInc)
				   {
				     if (!pBox->BxHorizFlex)
				       XMove (pBox, NULL, -endTrans, frame);
				   }
				 else if ((pRelation->ReOp == OpHorizDep && pRelation->ReBox->BxHorizFlex)
					  || pRelation->ReOp == OpWidth)
				   MoveBoxEdge (pRelation->ReBox, pBox, pRelation->ReOp, endTrans, frame, TRUE);
				 else if (pRelation->ReBox != pSourceBox)
				   XMove (pRelation->ReBox, pBox, endTrans, frame);
			       }
			    break;
			  default:
			    break;
			  }
		    
		    i++;
		    if (i < MAX_RELAT_POS)
		      notEmpty = (pPosRel->PosRTable[i].ReBox != NULL);
		  }
		pPosRel = pPosRel->PosRNext;	/* Bloc suivant */
	      }

	    /* Note si la boite est placee en absolu ou non */
	    absoluteMove = IsXPosComplete (pBox);
	    
	    /* Deplacement de boites incluses ou reevaluation du bloc de lignes */
	    if (absoluteMove
		|| pCurrentAb->AbWidth.DimAbRef != NULL
		|| pCurrentAb->AbWidth.DimValue != 0)
	      /* La boite n'est pas en cours de creation */
	      /* ou elle est a l'interieur d'une boite elastique */
	      /* ou elle est a l'interieur d'une boite hors-structure */
	      /* ou elle n'herite pas de la taille de son contenu */
	      if (pBox->BxType == BoBlock)
		RecomputeLines (pCurrentAb, pBox->BxFirstLine, pSourceBox, frame);
	      else
		{
		  pAb = pCurrentAb->AbFirstEnclosed;
		  while (pAb != NULL)
		    {
		      if (!pAb->AbDead && pAb->AbBox != NULL)
			{
			  pNextBox = pAb->AbBox;
			  /* regarde si la boite est liee a son englobante */
			  pPosRel = pNextBox->BxPosRelations;
			  while (pPosRel != NULL)
			    {
			      i = 0;
			      notEmpty = (pPosRel->PosRTable[i].ReBox != NULL);
			      while (i < MAX_RELAT_POS && notEmpty)
				{
				  pRelation = &pPosRel->PosRTable[i];
				  if (pRelation->ReOp == OpHorizInc
				      && pRelation->ReRefEdge != VertRef)
				    switch (pAb->AbHorizPos.PosRefEdge)
				      {
					/* milieu vertical */
				      case VertMiddle:
					if (absoluteMove)
					  j = middleTrans;
					else
					  /* il faut reevaluer le centrage */
					  j = (pBox->BxWidth - pNextBox->BxWidth) / 2 - pNextBox->BxXOrg;
					if (pNextBox->BxHorizFlex)
					  MoveBoxEdge (pNextBox, pBox, pRelation->ReOp, j, frame, TRUE);
					else
					  XMove (pNextBox, pBox, j, frame);
					break;
					/* cote droit */
				      case Right:
					if (absoluteMove)
					  j = endTrans;
					else
					  /* il faut reevaluer le cadrage */
					  j = pBox->BxWidth - pNextBox->BxWidth - pNextBox->BxXOrg;
					if (pNextBox->BxHorizFlex)
					  MoveBoxEdge (pNextBox, pBox, pRelation->ReOp, j, frame, TRUE);
					else
					  XMove (pNextBox, pBox, j, frame);
					break;
					/* cote gauche */
				      default:
					if (absoluteMove)
					  j = orgTrans;
					else
					  /* il faut reevaluer le cadrage */
					  j = 0;
					if (pNextBox->BxHorizFlex)
					  MoveBoxEdge (pNextBox, pBox, pRelation->ReOp, j, frame, TRUE);
					else
					  XMove (pNextBox, pBox, j, frame);
					break;
				      }
				  
				  i++;
				  if (i < MAX_RELAT_POS)
				    notEmpty = (pPosRel->PosRTable[i].ReBox != NULL);
				}
			      pPosRel = pPosRel->PosRNext;	/* Bloc suivant */
			    }
			}
		      pAb = pAb->AbNext;
		    }
		}
	    
	    /* respecte les contraintes de dimension */
	    pDimRel = pBox->BxWidthRelations;
	    while (pDimRel != NULL)
	      {
		i = 0;
		pNextBox = pDimRel->DimRTable[i];
		while (i < MAX_RELAT_DIM && pNextBox != NULL)
		  {
		    pAb = pNextBox->BxAbstractBox;
		    if (pAb)
		       /* Est-ce la meme dimension ? */
		       if (pDimRel->DimRSame[i])
			 /* Oui => Changement de largeur */
			 {
			   if (pAb->AbWidth.DimUnit == UnPercent)
			     /* Le changement de taille est un pourcentage */
			     orgTrans = pBox->BxWidth * pAb->AbWidth.DimValue / 100 - pNextBox->BxWidth;
			   else
			     orgTrans = delta;	/* + ou - une constante */
			   /* evite de boucler sur l'automodification */
			   if (pNextBox != pBox)
			     ChangeWidth (pNextBox, pSourceBox, pBox, orgTrans, spaceDelta, frame);
			 }
		       else
			 /* Non => Changement de hauteur */
			 {
			   /* L'heritage porte sur la hauteur de la ligne? */
			   if (pAb->AbEnclosing == pCurrentAb
			       && (pCurrentAb->AbInLine || pCurrentAb->AbBox->BxType == BoGhost))
			     {
			       pLine = SearchLine (pNextBox);
			       if (pLine == NULL)
				 /* la ligne n'est pas encore construite */
				 orgTrans = 0;
			       else
				 orgTrans = pLine->LiHeight - pNextBox->BxHeight;
			     }
			   /* Le changement de taille est un pourcentage */
			   else if (pAb->AbHeight.DimUnit == UnPercent)
			     orgTrans = pBox->BxWidth * pAb->AbHeight.DimValue / 100 - pNextBox->BxHeight;
			   else
			     orgTrans = delta;
			   ChangeHeight (pNextBox, pSourceBox, NULL, orgTrans, frame);
			 }
		    
		    i++;
		    if (i < MAX_RELAT_DIM)
		      pNextBox = pDimRel->DimRTable[i];
		  }
		pDimRel = pDimRel->DimRNext;
	      }
	    
	    /* respecte les contraintes d'englobement */
	    pAb = pCurrentAb->AbEnclosing;	/* pave pere */
	    if (!toMove)
	      {
		/* Si la boite n'est pas fille d'une boite eclatee */
		/* --> on recherche la boite bloc de lignes        */
		while (!pAb->AbInLine || pAb->AbBox->BxType == BoGhost)
		  pAb = pAb->AbEnclosing;
	      }
	    
	    if (pAb == NULL)
	      {
		/* C'est la racine de l'image concrete */
		if (Propagate == ToAll && pBox->BxXOrg < 0)
		  XMoveAllEnclosed (pBox, -pBox->BxXOrg, frame);
	      }
	    /* PackBoxRoot a refaire si la boite est englobee */
	    else if (pCurrentAb->AbHorizEnclosing)
	      {
		/* note l'origine externe ou non de la modification de largeur */
		externalRef = !IsParentBox (pAb->AbBox, pSourceBox);
		
		/* si pSourceBox n'est pas une fille il faut propager */
		if ((Propagate == ToAll || externalRef)
		    /* et si l'englobement n'est pas fait par une boite soeur */
		    && !IsSiblingBox (pBox, pFromBox)
		    && !IsSiblingBox (pBox, pSourceBox))
		  {		    
		    /* Inclusion dans un bloc de ligne */
		    if (pAb->AbInLine)
		      {
			pViewSel = &ViewFrameTable[frame - 1].FrSelectionBegin;
			if (pBox == pViewSel->VsBox)
			  pLine = pViewSel->VsLine;
			else
			  pLine = SearchLine (pBox);
			UpdateLineBlock (pAb, pLine, pBox, delta, spaceDelta, frame);
		      }
		    /* Si l'englobement n'est pas prevu en fin de traitement */
		    else if (pAb->AbBox != PackBoxRoot &&
			     !IsParentBox (pAb->AbBox, PackBoxRoot) /*&&
			     pAb->AbBox != pFromBox*/)
		      /* differe le traitement de l'englobement   */
		      /* quand la mise a jour a une origine externe  */
		      if (Propagate != ToAll)
			RecordEnclosing (pAb->AbBox, TRUE);
		    /* l'englobement d'une boite ne peut etre traite */
		    /* plus de deux fois (sinon on boucle).      */
		      else if (pAb->AbBox->BxCycles <= 1)
			{
			  if (pAb->AbBox->BxType == BoCell &&
			      ThotLocalActions[T_checkcolumn])
			    (*ThotLocalActions[T_checkcolumn]) (pAb, NULL, frame);
			  else
			    WidthPack (pAb, pSourceBox, frame);
			}
		  }
		else if (!pCurrentAb->AbNew
			 && Propagate == ToSiblings
			 && pCurrentAb->AbLeafType == LtCompound
			 && pCurrentAb->AbInLine && !pBox->BxYToCompute)
		  {
		    /* La largeur de la boite mise en lignes est donnee par une */
		    /* boite suivante, il faut verifier l'englobement vertical */
		    if (pAb->AbBox->BxType != BoTable)
		      HeightPack (pAb, pSourceBox, frame);
		    Propagate = ToSiblings;
		  }
	      }

	    if (pBox->BxType == BoTable && pBox->BxCycles == 0 &&
		ThotLocalActions[T_resizetable])
	      (*ThotLocalActions[T_resizetable]) (pCurrentAb, frame);
	  }
    }
}

/*----------------------------------------------------------------------
   ResizeHeight modifie la hauteur de la boite pBox correspondant au 
   frame suite au changement de hauteur de la boite origine   
   pSourceBox. On respecte les contraintes de position :      
   - Toute boite liee a` un des cotes deplaces est            
   deplacee.                                                
   - met a` jour la base de la boite pBox si necessaire.      
   On respecte les contraintes de dimension :                 
   - On met a` jour les hauteurs des boites qui en            
   dependent.                                               
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ResizeHeight (PtrBox pBox, PtrBox pSourceBox, PtrBox pFromBox, int delta, int frame)
#else  /* __STDC__ */
void                ResizeHeight (pBox, pSourceBox, pFromBox, delta, frame)
PtrBox              pBox;
PtrBox              pSourceBox;
PtrBox              pFromBox;
int                 delta;
int                 frame;

#endif /* __STDC__ */
{
  PtrBox              pNextBox;
  PtrLine             pLine;
  PtrAbstractBox      pAb/*, pCell*/;
  PtrAbstractBox      pCurrentAb;
  PtrPosRelations     pPosRel;
  PtrDimRelations     pDimRel;
  BoxRelation        *pRelation;
  ptrfont             font;
  float               value;
  int                 i, j, k;
  int                 orgTrans, middleTrans, endTrans;
  ThotBool            notEmpty;
  ThotBool            toMove;
  ThotBool            absoluteMove;
  ThotBool            externalRef;

   if (pBox != NULL && delta != 0)
     {
       /* Faut-il nettoyer la file des boites deplacees */
       if (pSourceBox == NULL && pFromBox == NULL)
	 pBox->BxMoved = NULL;
       
       if (pBox->BxAbstractBox != NULL)
	 if (!pBox->BxAbstractBox->AbDead)
	   {
	     /* verifie que la hauteur d'une boite ne devient pas negative */
	     /*if (Propagate != ToSiblings && delta < 0 && -delta > pBox->BxHeight)
	       delta = -pBox->BxHeight;*/
	     /* Valeurs limites avant deplacement */
	     i = pBox->BxYOrg;
	     j = i + pBox->BxHeight;
	     pCurrentAb = pBox->BxAbstractBox;
	     
	     /* Ce n'est pas une boite elastique: RAZ sur file des boites deplacees */
	     if (!pBox->BxVertFlex)
	       pBox->BxMoved = NULL;
	     
	     /* Force la reevaluation des points de controle de la polyline */
	     if (pCurrentAb->AbLeafType == LtPolyLine && pBox->BxPictInfo != NULL)
	       {
		 /* libere les points de controle */
		 free ((STRING) pBox->BxPictInfo);
		 pBox->BxPictInfo = NULL;
	       }
	     
	     /* regarde si les regles de dependance sont valides */
	     toMove = TRUE;
	     if (pCurrentAb->AbEnclosing != NULL)
	       if (pCurrentAb->AbEnclosing->AbBox != NULL)
		 toMove = pCurrentAb->AbEnclosing->AbBox->BxType != BoGhost;
	     
	     /* respecte les contraintes de position */
	     /*=> Point fixe sur l'origine */
	     if (!toMove || pBox->BxVertEdge == Top || pBox->BxVertEdge == HorizRef)
	       {
		 /* Valeurs de deplacement des reperes */
		 orgTrans = 0;
		 middleTrans = (pBox->BxHeight + delta) / 2 - pBox->BxHeight / 2;
		 endTrans = delta;
	       }
	     /*=> Point fixe sur le milieu */
	     else if (pBox->BxVertEdge == HorizMiddle)
	       {
		 /* L'origine de la boite et le cote inferieur sont deplaces */
		 orgTrans = pBox->BxHeight / 2 - (pBox->BxHeight + delta) / 2;
		 endTrans = delta + orgTrans;	/* corrige les erreurs d'arrondi */
		 middleTrans = 0;
	       }
	     /*=> Point fixe sur le cote inferieur */
	     else
	       {
		 /* L'origine de la boite et le milieu sont deplaces */
		 orgTrans = -delta;
		 middleTrans = pBox->BxHeight / 2 - (pBox->BxHeight + delta) / 2;
		 endTrans = 0;
	       }
	     pBox->BxHeight += delta;
	     pBox->BxYOrg += orgTrans;
	     
	     /* evalue la partie de la fenetre a reafficher */
	     if (ReadyToDisplay
		 && pBox->BxType != BoSplit
		 /* Il ne faut pas tenir compte de la boite si elle */
		 /* n'est pas encore placee dans l'image concrete   */
		 && !pBox->BxXToCompute
		 && !pBox->BxYToCompute
		 && (orgTrans != 0
		     || pCurrentAb->AbFirstEnclosed == NULL
		     /* redisplay filled boxes */
		     || pCurrentAb->AbFillBox
		     || pCurrentAb->AbPictBackground != NULL))
	       {
		 if (pCurrentAb->AbLeafType == LtText)
		   {
		     k = 0;
		     if (orgTrans == 0)
		       i = j;
		     else if (orgTrans < 0)
		       i += orgTrans;
		     if (endTrans == 0)
		       j = i;
		     else if (endTrans > 0)
		       j += endTrans;
		   }
		 else
		   {
		     /* Prend en compte une zone de debordement des graphiques */
		     if (pBox->BxAbstractBox->AbLeafType == LtGraphics)
		       k = EXTRA_GRAPH;
		     else
		       k = 0;
		     if (orgTrans < 0)
		       i += orgTrans;
		     if (endTrans > 0)
		       j += endTrans;
		   }
		 DefClip (frame, pBox->BxXOrg - k, i - k, pBox->BxXOrg + pBox->BxWidth + k, j + k);
	       }
	     
	     /* Deplacement de boites voisines et de l'englobante ? */
	     pPosRel = pBox->BxPosRelations;
	     while (pPosRel != NULL)
	       {
		 i = 0;
		 notEmpty = (pPosRel->PosRTable[i].ReBox != NULL);
		 while (i < MAX_RELAT_POS && notEmpty)
		   {
		     pRelation = &pPosRel->PosRTable[i];
		     if (pRelation->ReBox->BxAbstractBox != NULL)
		       /* Ignore la relation inverse de la boite elastique */
		       if (!pBox->BxVertFlex
			   || pRelation->ReOp != OpVertDep
			   || pCurrentAb == pRelation->ReBox->BxAbstractBox->AbVertPos.PosAbRef)
			 switch (pRelation->ReRefEdge)
			   {
			     /* cote superieur */
			   case Top:
			     if (pRelation->ReOp == OpVertRef)
			       {
				 if (pRelation->ReBox != pBox)
				   MoveHorizRef (pRelation->ReBox, pSourceBox, orgTrans, frame);
			       }
			     else if (toMove)
			       {
				 if (pRelation->ReOp == OpVertInc)
				   {
				     if (!pBox->BxVertFlex)
				       YMove (pBox, NULL, -orgTrans, frame);
				   }
				 else if ((pRelation->ReOp == OpVertDep && pRelation->ReBox->BxVertFlex)
					  || pRelation->ReOp == OpHeight)
				   MoveBoxEdge (pRelation->ReBox, pBox, pRelation->ReOp, orgTrans, frame, FALSE);
				 else if (pRelation->ReBox != pSourceBox)
				   YMove (pRelation->ReBox, pBox, orgTrans, frame);
			       }
			     break;
			     /* milieu horizontal */
			   case HorizMiddle:
			     if (pRelation->ReOp == OpVertRef)
			       {
				 if (pRelation->ReBox == pBox)
				   {
				     MoveHorizRef (pBox, NULL, delta / 2, frame);
				     pBox->BxMoved = pFromBox;	/* restaure le chainage */
				   }
				 else
				   MoveHorizRef (pRelation->ReBox, pSourceBox, middleTrans, frame);
			       }
			     else if (toMove)
			       {
				 if (pRelation->ReOp == OpVertInc)
				   {
				     if (!pBox->BxVertFlex)
				       YMove (pBox, NULL, -middleTrans, frame);
				   }
				 else if ((pRelation->ReOp == OpVertDep && pRelation->ReBox->BxVertFlex)
					  || pRelation->ReOp == OpHeight)
				   MoveBoxEdge (pRelation->ReBox, pBox, pRelation->ReOp, middleTrans, frame, FALSE);
				 else if (pRelation->ReBox != pSourceBox)
				   YMove (pRelation->ReBox, pBox, middleTrans, frame);
			       }
			     break;
			     /* cote inferieur */
			   case Bottom:
			     if (pRelation->ReOp == OpVertRef)
			       {
				 if (pRelation->ReBox == pBox)
				   {
				     if (pCurrentAb->AbLeafType == LtText
					 && pCurrentAb->AbHorizRef.PosAbRef == NULL)
				       j = FontBase (pBox->BxFont) - pBox->BxHorizRef;
				     else
				       j = delta;
				     MoveHorizRef (pBox, NULL, j, frame);
				     pBox->BxMoved = pFromBox;	/* restaure le chainage */
				   }
				 else
				   MoveHorizRef (pRelation->ReBox, pSourceBox, endTrans, frame);
			       }
			     else if (toMove)
			       {
				 if (pRelation->ReOp == OpVertInc)
				   {
				     if (!pBox->BxVertFlex)
				       YMove (pBox, NULL, -endTrans, frame);
				   }
				 else if ((pRelation->ReOp == OpVertDep && pRelation->ReBox->BxVertFlex)
					  || pRelation->ReOp == OpHeight)
				   MoveBoxEdge (pRelation->ReBox, pBox, pRelation->ReOp, endTrans, frame, FALSE);
				 else if (pRelation->ReBox != pSourceBox)
				   YMove (pRelation->ReBox, pBox, endTrans, frame);
			       }
			     break;
			   default:
			     break;
			   }
		     i++;
		     if (i < MAX_RELAT_POS)
		       notEmpty = pPosRel->PosRTable[i].ReBox != NULL;
		   }
		 /* next block */
		 pPosRel = pPosRel->PosRNext;
	       }
	     
	     /* Note si la boite est placee en absolu ou non */
	     absoluteMove = IsYPosComplete (pBox);
	     
	     /* Deplacement de boites incluses ? */
	     if (absoluteMove && pBox->BxType == BoBlock)
	       {
		 /* Il faut traiter les blocs de lignes elastiques */
		 /* qui ne sont pas en cours de placement          */
		 if (orgTrans != 0)
		   {
		     /* decale aussi les boites mises en ligne */
		     pAb = pCurrentAb->AbFirstEnclosed;
		     while (pAb != NULL)
		       {
			 if (!pAb->AbDead)
			   YMoveAllEnclosed (pAb->AbBox, orgTrans, frame);
			 pAb = pAb->AbNext;
		       }
		   }
	       }
	     /* Si on n'est pas en cours de creation */
	     /* ou si elle est a l'interieur d'une boite elastique */
	     /* ou si elle est a l'interieur d'une boite hors-structure */
	     /* ou si la boite n'herite pas de la taille de son contenu */
	     else if (absoluteMove
		      || pCurrentAb->AbHeight.DimAbRef != NULL
		      || pCurrentAb->AbHeight.DimValue != 0)
	       {
		 pAb = pCurrentAb->AbFirstEnclosed;
		 while (pAb != NULL)
		   {
		     if (!pAb->AbDead && pAb->AbBox != NULL)
		       {
			 pNextBox = pAb->AbBox;
			 /* regarde si la boite est liee a son englobante */
			 pPosRel = pNextBox->BxPosRelations;
			 while (pPosRel != NULL)
			   {
			     i = 0;
			     notEmpty = (pPosRel->PosRTable[i].ReBox != NULL);
			     while (i < MAX_RELAT_POS && notEmpty)
			       {
				 pRelation = &pPosRel->PosRTable[i];
				 if (pRelation->ReOp == OpVertInc
				     && pRelation->ReRefEdge != HorizRef)
				   {
				     switch (pAb->AbVertPos.PosRefEdge)
				       {
					 /* milieu horizontal */
				       case HorizMiddle:
					 if (absoluteMove)
					   j = middleTrans;
					 else
					   /* il faut reevaluer le centrage */
					   j = (pBox->BxHeight - pNextBox->BxHeight) / 2 - pNextBox->BxYOrg;
					 if (pNextBox->BxVertFlex)
					   MoveBoxEdge (pNextBox, pBox, pRelation->ReOp, j, frame, FALSE);
					 else
					   YMove (pNextBox, pBox, j, frame);
					 break;
					 /* cote inferieur */
				       case Bottom:
					 if (absoluteMove)
					   j = endTrans;
					 else
					   /* il faut reevaluer le cadrage */
					   j = pBox->BxHeight - pNextBox->BxHeight - pNextBox->BxYOrg;
					 if (pNextBox->BxVertFlex)
					   MoveBoxEdge (pNextBox, pBox, pRelation->ReOp, j, frame, FALSE);
					 else
					   YMove (pNextBox, pBox, j, frame);
					 break;
					 /* cote superieur */
				       default:
					 if (absoluteMove)
					   j = orgTrans;
					 else
					   /* il faut reevaluer le cadrage */
					   j = 0;
					 if (pNextBox->BxVertFlex)
					   MoveBoxEdge (pNextBox, pBox, pRelation->ReOp, j, frame, FALSE);
					 else
					   YMove (pNextBox, pBox, j, frame);
					 break;
				       }
				   }
				 i++;
				 if (i < MAX_RELAT_POS)
				   notEmpty = pPosRel->PosRTable[i].ReBox != NULL;
			       }
			     /* next block */
			     pPosRel = pPosRel->PosRNext;
			   }
		       }
		     pAb = pAb->AbNext;
		   }
	       }
	     
	     /* respecte les contraintes de dimension */
	     pDimRel = pBox->BxHeightRelations;
	     while (pDimRel != NULL)
	       {
		 i = 0;
		 pNextBox = pDimRel->DimRTable[i];
		 while (i < MAX_RELAT_DIM && pNextBox != NULL)
		   {
		     pAb = pNextBox->BxAbstractBox;
		     
		     /* Est-ce la meme dimension ? */
		     if (pDimRel->DimRSame[i])
		       /* Oui => Changement de hauteur */
		       {
			 /* L'heritage porte sur la hauteur de la ligne ? */
			 if (pAb->AbEnclosing == pCurrentAb
			     && (pCurrentAb->AbInLine || pCurrentAb->AbBox->BxType == BoGhost))
			   {
			     pLine = SearchLine (pNextBox);
			     if (pLine == NULL)
			       /* la ligne n'est pas encore construite */
			       orgTrans = 0;
			     else
			       orgTrans = pLine->LiHeight - pNextBox->BxHeight;
			   }
			 /* Le changement de taille est un pourcentage */
			 else if (pAb->AbHeight.DimUnit == UnPercent)
			   orgTrans = pBox->BxHeight * pAb->AbHeight.DimValue / 100 - pNextBox->BxHeight;
			 else
			   orgTrans = delta;
			 /* evite de boucler sur l'automodification */
			 if (pNextBox != pBox)
			   ChangeHeight (pNextBox, pSourceBox, pBox, orgTrans, frame);
		       }
		     else
		       /* Non => Changement de largeur */
		       {
			 if (pAb->AbWidth.DimUnit == UnPercent)
			   /* Le changement de taille est un pourcentage */
			   orgTrans = pBox->BxHeight * pAb->AbWidth.DimValue / 100 - pNextBox->BxWidth;
			 else
			   /* + ou - une constante */
			   orgTrans = delta;
			 ChangeWidth (pNextBox, pSourceBox, NULL, orgTrans, 0, frame);
		       }
		     i++;
		     if (i < MAX_RELAT_DIM)
		       pNextBox = pDimRel->DimRTable[i];
		   }
		 /* Bloc suivant */
		 pDimRel = pDimRel->DimRNext;
	       }
	     
	     /* respecte les contraintes d'englobement */
	     pAb = pCurrentAb->AbEnclosing;
	     if (!toMove)
	       {
		 /* Si la boite n'est pas fille d'une boite eclatee */
		 /* --> on recherche la boite bloc de lignes        */
		 while (!pAb->AbInLine || pAb->AbBox->BxType == BoGhost)
		   pAb = pAb->AbEnclosing;
	       }
	     
	     if (pAb == NULL)
	       {
		 /* C'est la racine de l'image concrete */
		 if (Propagate == ToAll && pBox->BxYOrg < 0)
		   YMoveAllEnclosed (pBox, -pBox->BxYOrg, frame);
	       }
	     /* PackBoxRoot a refaire si la boite est englobee */
	     else if (pCurrentAb->AbVertEnclosing)
	       {
		 /* note l'origine externe ou non de la modification de hauteur */
		 externalRef = !IsParentBox (pAb->AbBox, pSourceBox);
		 
		 /* si pSourceBox n'est pas une fille il faut propager */
		 if ((Propagate == ToAll || externalRef)
		     /* et si l'englobement n'est pas fait par une boite soeur */
		     && !IsSiblingBox (pBox, pFromBox)
		     && !IsSiblingBox (pBox, pSourceBox))
		   {
		     if (pAb->AbInLine)
		       /* Inclusion dans un bloc de ligne */
		       EncloseInLine (pBox, frame, pAb);
		     /*else if ((pCell = GetParentCell (pBox)) &&
			      pAb->AbBox->BxPacking <= 1)
		       {
			 HeightPack (pAb, pSourceBox, frame);
		       }
		     */
		     /* Si l'englobement n'est pas prevu en fin de traitement */
		     else if (pAb->AbBox != PackBoxRoot &&
			      !IsParentBox (pAb->AbBox, PackBoxRoot) /*&&
			      pAb->AbBox != pFromBox*/)
		       /* differe le traitement de l'englobement   */
		       /* quand la mise a jour a une origine externe  */
		       if (Propagate != ToAll && pAb->AbBox->BxType != BoCell)
			 RecordEnclosing (pAb->AbBox, FALSE);
		     /* l'englobement d'une boite ne peut etre traite */
		     /* plus de deux fois (sinon on boucle).      */
		       else if (pAb->AbBox->BxPacking <= 1)
			 HeightPack (pAb, pSourceBox, frame);
		   }
	       }
	   }
     }
   
   /* Traitement specifique des largeurs de symboles */
   if (pBox != NULL)
     {
       if (pBox->BxAbstractBox->AbLeafType == LtSymbol)
	 {
	   i = 0;
	   font = pBox->BxFont;
	   value = 1 + ((float) (pBox->BxHeight * 0.6) / (float) FontHeight (font));
	   switch (pBox->BxAbstractBox->AbShape)
	     {
	     case 'd':	/*integrale double */
	       i = CharacterWidth (231, font) + CharacterWidth (231, font) / 2;
	     case 'i':	/*integrale */
	     case 'c':	/*integrale circulaire */
	       i = (int) ((float) CharacterWidth (231, font) * value);
	       ResizeWidth (pBox, NULL, NULL, i - pBox->BxWidth, 0, frame);
	       break;
	     case '(':
	     case ')':
	     case '{':
	     case '}':
	     case '[':
	     case ']':
	       i = (int) ((float) CharacterWidth (230, font) * value);
	       ResizeWidth (pBox, NULL, NULL, i - pBox->BxWidth, 0, frame);
	       break;
	     default:
	       break;
	     }
	 }
     }
}


/*----------------------------------------------------------------------
   XMove deplace l'origine de la boite pBox, donnee en parametre,  
   de delta. On respecte les contraintes de position.      
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                XMove (PtrBox pBox, PtrBox pFromBox, int delta, int frame)
#else  /* __STDC__ */
void                XMove (pBox, pFromBox, delta, frame)
PtrBox              pBox;
PtrBox              pFromBox;
int                 delta;
int                 frame;

#endif /* __STDC__ */
{
   PtrBox              pNextBox;
   PtrAbstractBox      pAb;
   PtrAbstractBox      pCurrentAb;
   PtrPosRelations     pPosRel;
   BoxRelation        *pRelation;
   int                 i, j, k;
   ThotBool            toComplete;
   ThotBool            notEmpty;
   ThotBool            checkParent;
   ThotBool            absoluteMove;

   if (pBox != NULL && delta != 0)
     {
	pCurrentAb = pBox->BxAbstractBox;
	if (pCurrentAb != NULL)
	   if (!pCurrentAb->AbDead)
	     {
		/* verifie que la boite n'a pas ete deja deplacee */
		if (pFromBox != NULL)
		  {
		     pNextBox = pFromBox->BxMoved;
		     while (pNextBox != NULL)
			if (pNextBox == pBox)
			   return;
			else
			   pNextBox = pNextBox->BxMoved;
		  }
		/* met a jour la pile des boites traitees */
		pBox->BxMoved = pFromBox;
		/* memorise que la boite doit etre placee */
		toComplete = pBox->BxXToCompute;
		/* evalue la partie de la fenetre a reafficher */
		if (ReadyToDisplay)
		  {
		     if (pBox->BxType != BoSplit)
		       {
			  i = pBox->BxXOrg;
			  j = pBox->BxXOrg + pBox->BxWidth;
			  /* Prend en compte une zone de debordement des graphiques */
			  if (pBox->BxAbstractBox->AbLeafType == LtGraphics)
			     k = EXTRA_GRAPH;
			  else
			     k = 0;
			  if (delta > 0)
			     j += delta;
			  else
			     i += delta;
			  if (pBox->BxHeight > 0)
			     DefClip (frame, i - k, pBox->BxYOrg - k, j + k, pBox->BxYOrg + pBox->BxHeight + k);
		       }
		     /* Est-ce un pave non englobe ? */
		     else if (!pCurrentAb->AbVertEnclosing)
			if (delta > 0)
			   DefClip (frame, pBox->BxXOrg - delta, pBox->BxYOrg,
				    pBox->BxXOrg + pBox->BxWidth, pBox->BxYOrg + pBox->BxHeight);
			else
			   DefClip (frame, pBox->BxXOrg, pBox->BxYOrg, pBox->BxXOrg +
				    pBox->BxWidth - delta, pBox->BxYOrg + pBox->BxHeight);
		  }

		/* deplace seulement l'origine de la boite si elle est en cours  */
		/* de creation et que ce n'est pas une boite elastique, sinon on    */
		/*  deplace tout le contenu (boites englobees)                      */
		absoluteMove = IsXPosComplete (pBox);

		/* Teste si la position de la boite est absolue ou relative */
		if (absoluteMove)
		  {
		     XMoveAllEnclosed (pBox, delta, frame);
		     /* a pu detruire le chainage des boites deplacees */
		     pBox->BxMoved = pFromBox;
		     /* evite d'introduire une boucle */
		     pNextBox = pFromBox;
		     while (pNextBox != NULL)
			if (pNextBox->BxMoved == pFromBox)
			  {
			     pNextBox->BxMoved = NULL;
			     pNextBox = NULL;
			  }
			else
			   pNextBox = pNextBox->BxMoved;
		  }
		else
		   pBox->BxXOrg += delta;

		/* Regarde si les regles de dependance sont valides */
		checkParent = TRUE;
		if (pCurrentAb->AbEnclosing != NULL)
		  if (pCurrentAb->AbEnclosing->AbBox != NULL)
		    checkParent = (pCurrentAb->AbEnclosing->AbBox->BxType != BoGhost);

		/* Decale les boites dependantes qui restent a deplacer */
		pPosRel = pBox->BxPosRelations;
		while (pPosRel != NULL)
		  {
		    i = 0;
		    notEmpty = (pPosRel->PosRTable[i].ReBox != NULL);
		    while (i < MAX_RELAT_POS && notEmpty)
		      {
			pRelation = &pPosRel->PosRTable[i];
			if (pRelation->ReBox->BxAbstractBox != NULL
			    && pRelation->ReBox->BxType != BoGhost)
			  {
			    /* cote gauche */
			    /* cote droit */
			    /* milieu vertical */
			    /* ref. verticale */
			    if (pRelation->ReOp == OpHorizRef)
			      {
				/* Sauf l'axe de reference de la boite elle-meme */
				if (pRelation->ReBox != pBox)
				  {
				    pAb = pCurrentAb->AbEnclosing;
				    if (pAb != NULL)
				      pNextBox = pAb->AbBox;
				    else
				      pNextBox = NULL;
				    if (pRelation->ReBox != pNextBox || Propagate == ToAll)
				      MoveVertRef (pRelation->ReBox, pBox, delta, frame);
				  }
			      }
			    /* Ignore la relation inverse de la boite elastique */
			    else if (pBox->BxHorizFlex
				     && pCurrentAb != pRelation->ReBox->BxAbstractBox->AbHorizPos.PosAbRef)
				;
			    
			    /* ne decale pas les boites qui ont des relations  */
			    /* hors-structure avec la boite deplacee et on ne met */
			    /* pas a jour les dimensions elastiques des boites    */
			    /* liees a la boite deplacee si elles ont ete         */
			    /* traitees par XMoveAllEnclosed.                     */
			    
			    else if (absoluteMove)
			      {
				if (!pBox->BxHorizFlex || toComplete)
				  {
				    /* le travail n'a pas ete fait dans XMoveAllEnclosed */
				    if (pRelation->ReOp == OpHorizDep && !pRelation->ReBox->BxXOutOfStruct)
				      /* Relation conforme a la structure sur l'origine de boite */
				      if (pRelation->ReBox->BxHorizFlex
					  /* si la boite n'est pas une boite fille */
					  && pCurrentAb != pRelation->ReBox->BxAbstractBox->AbEnclosing
					  && pCurrentAb == pRelation->ReBox->BxAbstractBox->AbHorizPos.PosAbRef)
					MoveBoxEdge (pRelation->ReBox, pBox, pRelation->ReOp, delta, frame, TRUE);
				    /* Relation conforme a la structure sur la largeur de boite */
				      else
					XMove (pRelation->ReBox, pBox, delta, frame);
				  }
			      }
			    else if (pRelation->ReOp == OpHorizDep && !pRelation->ReBox->BxHorizFlex)
			      XMove (pRelation->ReBox, pBox, delta, frame);
			    else if (((pRelation->ReOp == OpHorizDep
				       && pCurrentAb == pRelation->ReBox->BxAbstractBox->AbHorizPos.PosAbRef))
				     || pRelation->ReOp == OpWidth)
			      MoveBoxEdge (pRelation->ReBox, pBox, pRelation->ReOp, delta, frame, TRUE);
			  }
			
			/* a pu detruire le chainage des boites deplacees */
			pBox->BxMoved = pFromBox;
			i++;
			if (i < MAX_RELAT_POS)
			  notEmpty = pPosRel->PosRTable[i].ReBox != NULL;
		      }
		    /* next block */
		    pPosRel = pPosRel->PosRNext;
		  }

		/* Si le calcul de la largeur de la boite englobante est a refaire */
		pAb = pCurrentAb->AbEnclosing;
		if (checkParent && pBox->BxXOutOfStruct && pAb != NULL)
		   /* ne peut traiter l'englobement d'une boite si cette boite */
		   /* est en cours de placement ou si ce traitement est differe   */
		   if (!pAb->AbBox->BxXToCompute
		       && !IsParentBox (pAb->AbBox, PackBoxRoot)
		   /* si pSourceBox est une fille il faut la propagation a toutes */
		       && (Propagate == ToAll || !IsParentBox (pAb->AbBox, pFromBox))
		   /* et si l'englobement n'est pas fait par une boite soeur */
		       && !IsSiblingBox (pBox, pFromBox)
		   /* et si la boite deplacee est effectivement englobee */
		       && pCurrentAb->AbHorizEnclosing)
		      WidthPack (pAb, pFromBox, frame);
	     }
     }
}

/*----------------------------------------------------------------------
   YMove deplace l'origine de la boite pBox, donnee en parametre,
   de delta. On respecte les contraintes de position.      
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                YMove (PtrBox pBox, PtrBox pFromBox, int delta, int frame)
#else  /* __STDC__ */
void                YMove (pBox, pFromBox, delta, frame)
PtrBox              pBox;
PtrBox              pFromBox;
int                 delta;
int                 frame;

#endif /* __STDC__ */
{
   PtrBox              pNextBox;
   PtrAbstractBox      pAb;
   PtrAbstractBox      pCurrentAb;
   PtrPosRelations     pPosRel;
   BoxRelation        *pRelation;
   int                 i, j, k;
   ThotBool            toComplete;
   ThotBool            notEmpty;
   ThotBool            checkParent;
   ThotBool            absoluteMove;

   if (pBox != NULL && delta != 0)
     {
	pCurrentAb = pBox->BxAbstractBox;
	if (pCurrentAb != NULL)
	   if (!pCurrentAb->AbDead)
	     {
		/* verifie que la boite n'a pas ete deja deplacee */
		if (pFromBox != NULL)
		  {
		     pNextBox = pFromBox->BxMoved;
		     while (pNextBox != NULL)
			if (pNextBox == pBox)
			   return;
			else
			   pNextBox = pNextBox->BxMoved;
		  }
		/* met a jour la pile des boites traitees */
		pBox->BxMoved = pFromBox;
		/* memorise que la boite doit etre placee */
		toComplete = pBox->BxYToCompute;
		/* evalue la partie de la fenetre a reafficher */
		if (ReadyToDisplay)
		  {
		     if (pBox->BxType != BoSplit)
		       {
			  i = pBox->BxYOrg;
			  j = pBox->BxYOrg + pBox->BxHeight;
			  /* Prend en compte une zone de debordement des graphiques */
			  if (pBox->BxAbstractBox->AbLeafType == LtGraphics)
			     k = EXTRA_GRAPH;
			  else
			     k = 0;
			  if (delta > 0)
			     j += delta;
			  else
			     i += delta;
			  if (pBox->BxWidth > 0 || k > 0)
			     DefClip (frame, pBox->BxXOrg - k, i - k, pBox->BxXOrg + pBox->BxWidth + k, j + k);
		       }
		     /* Est-ce un pave non englobe ? */
		     else if (!pCurrentAb->AbHorizEnclosing)
			if (delta > 0)
			   DefClip (frame, pBox->BxXOrg, pBox->BxYOrg - delta,
				    pBox->BxXOrg + pBox->BxWidth, pBox->BxYOrg + pBox->BxHeight);
			else
			   DefClip (frame, pBox->BxXOrg, pBox->BxYOrg, pBox->BxXOrg +
				    pBox->BxWidth, pBox->BxYOrg + pBox->BxHeight - delta);
		  }

		/* deplace seulement l'origine de la boite si elle est en cours  */
		/* de creation et que ce n'est pas une boite elastique, sinon on    */
		/*  deplace tout le contenu (boites englobees)                      */
		absoluteMove = IsYPosComplete (pBox);

		/* Teste si la position de la boite est absolue ou relative */
		if (absoluteMove)
		  {
		     YMoveAllEnclosed (pBox, delta, frame);
		     /* a pu detruire le chainage des boites deplacees */
		     pBox->BxMoved = pFromBox;
		     /* evite d'introduire une boucle */
		     pNextBox = pFromBox;
		     while (pNextBox != NULL)
			if (pNextBox->BxMoved == pFromBox)
			  {
			     pNextBox->BxMoved = NULL;
			     pNextBox = NULL;
			  }
			else
			   pNextBox = pNextBox->BxMoved;
		  }
		else
		   pBox->BxYOrg += delta;

		/* Regarde si les regles de dependance sont valides */
		checkParent = TRUE;
		if (pCurrentAb->AbEnclosing != NULL)
		  if (pCurrentAb->AbEnclosing->AbBox != NULL)
		    checkParent = (pCurrentAb->AbEnclosing->AbBox->BxType != BoGhost);

		/* decale les boites dependantes qui restent a deplacer */

		pPosRel = pBox->BxPosRelations;
		while (pPosRel != NULL)
		  {
		    i = 0;
		    notEmpty = (pPosRel->PosRTable[i].ReBox != NULL);
		    while (i < MAX_RELAT_POS && notEmpty)
		      {
			pRelation = &pPosRel->PosRTable[i];
			if (pRelation->ReBox->BxAbstractBox != NULL
			    && pRelation->ReBox->BxType != BoGhost)
			  {
			    /* cote superieur */
			    /* cote inferieur */
			    /* milieu horizontal */
			    /* ref. horizontale */
			    if (pRelation->ReOp == OpVertRef)
			      {
				/* Sauf l'axe de reference de la boite elle-meme */
				if (pRelation->ReBox != pBox)
				  {
				    pAb = pCurrentAb->AbEnclosing;
				    if (pAb != NULL)
				      pNextBox = pAb->AbBox;
				    else
				      pNextBox = NULL;
				    if (pRelation->ReBox != pNextBox || Propagate == ToAll)
				      MoveHorizRef (pRelation->ReBox, pBox, delta, frame);
				  }
			      }
			    /* Ignore la relation inverse de la boite elastique */
			    else if (pBox->BxVertFlex
				     && pCurrentAb != pRelation->ReBox->BxAbstractBox->AbVertPos.PosAbRef)
				;
			    /* ne decale pas les boites qui ont des relations  */
			    /* hors-structure avec la boite deplacee et on ne met */
			    /* pas a jour les dimensions elastiques dess boites   */
			    /* liees a la boite deplacee si elles ont ete         */
			    /* traitees par YMoveAllEnclosed.                     */
			    
			    else if (absoluteMove)
			      {
				if (!pBox->BxVertFlex || toComplete)
				  {
				    /* le travail n'a pas ete fait dans YMoveAllEnclosed */
				    /* Relation conforme a la structure sur l'origine de boite */
				    if (pRelation->ReOp == OpVertDep && !pRelation->ReBox->BxYOutOfStruct)
				      if (pRelation->ReBox->BxVertFlex
					  /* si la boite n'est pas une boite fille */
					  && pCurrentAb != pRelation->ReBox->BxAbstractBox->AbEnclosing
					  && pCurrentAb == pRelation->ReBox->BxAbstractBox->AbVertPos.PosAbRef)
					MoveBoxEdge (pRelation->ReBox, pBox, pRelation->ReOp, delta, frame, FALSE);
				      else
					YMove (pRelation->ReBox, pBox, delta, frame);
				  }
			      }
			    else if (pRelation->ReOp == OpVertDep && !pRelation->ReBox->BxVertFlex)
			      YMove (pRelation->ReBox, pBox, delta, frame);
			    else if ((pRelation->ReOp == OpVertDep
				      && pCurrentAb == pRelation->ReBox->BxAbstractBox->AbVertPos.PosAbRef)
				     || pRelation->ReOp == OpHeight)
			      MoveBoxEdge (pRelation->ReBox, pBox, pRelation->ReOp, delta, frame, FALSE);
			  }
			
			/* a pu detruire le chainage des boites deplacees */
			pBox->BxMoved = pFromBox;
			i++;
			if (i < MAX_RELAT_POS)
			  notEmpty = pPosRel->PosRTable[i].ReBox != NULL;
		      }
		    /* Bloc suivant */
		    pPosRel = pPosRel->PosRNext;
		  }

		/* Si le calcul de la hauteur de la boite englobante est a refaire */
		pAb = pCurrentAb->AbEnclosing;
		if (checkParent && pBox->BxYOutOfStruct && pAb != NULL)
		   /* ne peut traiter l'englobement d'une boite si cette boite */
		   /* est en cours de placement ou si ce traitement est differe   */
		   if (!pAb->AbBox->BxYToCompute
		       && !IsParentBox (pAb->AbBox, PackBoxRoot)
		   /* si pSourceBox est une fille il faut la propagation a toutes */
		       && (Propagate == ToAll || !IsParentBox (pAb->AbBox, pFromBox))
		   /* et si l'englobement n'est pas fait par une boite soeur */
		       && !IsSiblingBox (pBox, pFromBox)
		   /* et si la boite deplacee est effectivement englobee */
		       && pCurrentAb->AbVertEnclosing)
		      HeightPack (pAb, pFromBox, frame);
	     }
     }
}

/*----------------------------------------------------------------------
   WidthPack verifie l'inclusion en largeur des boites englobees dans
   le pave pAb suite au changement de largeur de la        
   boite origine pSourceBox.                               
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                WidthPack (PtrAbstractBox pAb, PtrBox pSourceBox, int frame)
#else  /* __STDC__ */
void                WidthPack (pAb, pSourceBox, frame)
PtrAbstractBox      pAb;
PtrBox              pSourceBox;
int                 frame;

#endif /* __STDC__ */
{
   int                 val, width;
   int                 x, i, j, k;
   PtrAbstractBox      pChildAb;
   PtrAbstractBox      pRelativeAb;
   PtrAbstractBox      pRefAb;
   PtrBox              pChildBox;
   PtrBox              pRelativeBox;
   PtrBox              pBox;
   AbDimension        *pDimAb;
   AbPosition         *pPosAb;
   ThotBool            notEmpty;
   ThotBool            toMove;
   ThotBool            absoluteMove;

   /* verifie que la largeur de la boite depend du contenu et qu'on */
   /* n'est pas deja en train de traiter l'englobement de cette boite  */
   pBox = pAb->AbBox;
   pDimAb = &pAb->AbWidth;
   if (pBox->BxType == BoBlock || pBox->BxType == BoCell)
     /* don't pack a block or a cell but transmit to enclosing box */
     WidthPack (pAb->AbEnclosing, pSourceBox, frame);
   else if (pBox->BxType == BoGhost)
     return;
   else if (pBox->BxContentWidth || (!pDimAb->DimIsPosition && pDimAb->DimMinimum))
     {
	/* indique que l'englobement horizontal est en cours de traitement */
	pBox->BxCycles += 1;

	/* verifie l'encadrement et l'englobement */
	/* Faut-il prendre l'origine en relatif ou en absolu ? */
	absoluteMove = IsXPosComplete (pBox);
	if (absoluteMove)
	   x = pBox->BxXOrg;
	else
	   x = 0;

	/* Initialise la position extreme droite sur le cote gauche actuel */
	width = x;
	/* Initialise la position extreme gauche sur le cote droit actuel */
	val = x + pBox->BxWidth;
	notEmpty = FALSE;
	toMove = FALSE;		/* on n'a rien deplace */

	/* Le cote gauche de la plus a gauche des boites englobees doit */
	/* etre colle au cote gauche de la boite englobante et la       */
	/* largeur de la boite englobante est delimitee par le cote     */
	/* droit le plus a droite des boites englobees.                 */

	pChildAb = pAb->AbFirstEnclosed;
	while (pChildAb != NULL)
	  {
	     pChildBox = pChildAb->AbBox;
	     if (!pChildAb->AbDead && pChildBox != NULL
		 && pChildAb->AbHorizEnclosing
		 && (pChildAb->AbWidth.DimAbRef != pAb || pChildBox->BxContentWidth))
	       {
		  /* Recherche la boite dont elle depend */
		  pRelativeBox = GetHPosRelativePos (pChildBox, NULL);
		  if (pRelativeBox != NULL)
		     if (pRelativeBox->BxAbstractBox != NULL)
		       {
			  pRelativeAb = pRelativeBox->BxAbstractBox;
			  /* note le pave dont depend la largeur de pChildAb */
			  pRefAb = pChildAb->AbWidth.DimAbRef;
			  if (pRelativeAb->AbHorizPos.PosAbRef == NULL)
			    {
			       /* Box est mobile, on tient compte de son origine */
			       notEmpty = TRUE;
			       if (pChildBox->BxXOrg < val)
				  val = pChildBox->BxXOrg;	/* valeur minimum */
			       i = pChildBox->BxXOrg + pChildBox->BxWidth;
			    }
			  /* La position de la boite depend de la taille de l'englobante ? */
			  else if (pRelativeAb->AbHorizPos.PosAbRef == pAb
			      && (pRelativeAb->AbHorizPos.PosRefEdge != Left
				  || (pRelativeAb->AbWidth.DimAbRef == pAb
				      && !pRelativeAb->AbWidth.DimIsPosition
			       && pChildAb->AbHorizPos.PosRefEdge != Left)))
			     i = x + pChildBox->BxWidth;
			  /* La taille de la boite depend d'une boite externe ? */
			  else if (pChildBox->BxWOutOfStruct)
			    {
			       /* qui herite de la taille de l'englobante ? */
			       if (IsParentBox (pRefAb->AbBox, pBox)
				   && pRefAb->AbWidth.DimAbRef == NULL
				   && pRefAb->AbWidth.DimValue == 0
				 && pRefAb->AbBox->BxWidth == pBox->BxWidth)
				  i = x;
			       else
				  /* evalue l'encadrement et l'englobement */
			       if (pChildBox->BxXOrg < x)
				  i = x + pChildBox->BxWidth;
			       else
				  i = pChildBox->BxXOrg + pChildBox->BxWidth;
			    }
			  else
			     /* evalue l'encadrement et l'englobement */
			  if (pChildBox->BxXOrg < x)
			     i = x + pChildBox->BxWidth;
			  else
			     i = pChildBox->BxXOrg + pChildBox->BxWidth;
			  if (i > width)
			     width = i;
		       }
	       }
	     pChildAb = pChildAb->AbNext;
	  }

	val = -val + x;		/* Decalage de la position extreme gauche */
	if (notEmpty)
	   width += val;	/* Nouvelle position extreme droite */
	if (width == x && pAb->AbVolume == 0)
	   GiveTextSize (pAb, &width, &x, &i);
	else
	   width -= x;
	x = width - pBox->BxWidth;	/* Difference de largeur */

	/* Faut-il deplacer les boites englobees ? */
	pChildAb = pAb->AbFirstEnclosed;
	if (notEmpty && val != 0)
	   while (pChildAb != NULL)
	     {
		pChildBox = pChildAb->AbBox;
		if (!pChildAb->AbDead && pChildBox != NULL && pChildAb->AbHorizEnclosing)
		  {

		     /* Est-ce une boite mobile a deplacer ? */
		     /* Recherche la boite dont elle depend */
		     pRelativeBox = GetHPosRelativePos (pChildBox, NULL);
		     if (pRelativeBox != NULL)
			if (pRelativeBox->BxAbstractBox != NULL)
			   if (pRelativeBox->BxAbstractBox->AbHorizPos.PosAbRef == NULL)
			     {
				/* met a jour la zone de reaffichage */
				if (ReadyToDisplay)
				  {
				     /* Prend en compte une zone de debordement des graphiques */
				     if (pChildBox->BxAbstractBox->AbLeafType == LtGraphics)
					k = EXTRA_GRAPH;
				     else
					k = 0;
				     i = pChildBox->BxXOrg;
				     j = pChildBox->BxXOrg + pChildBox->BxWidth;
				     if (val > 0)
					j += val;
				     else
					i += val;
				     DefClip (frame, i - k, pChildBox->BxYOrg - k, j + k, pChildBox->BxYOrg + pChildBox->BxHeight + k);
				  }

				if (IsXPosComplete (pChildBox))
				   XMoveAllEnclosed (pChildBox, val, frame);
				else
				   pChildBox->BxXOrg += val;

				/* Est-ce que cette boite deplace l'axe de l'englobante ? */
				pPosAb = &pAb->AbVertRef;
				if (pPosAb->PosAbRef == pChildAb)
				  {
				     toMove = TRUE;
				     pChildBox->BxMoved = NULL;
				     /* Est-ce que la relation porte sur l'axe de reference ? */
				     if (pPosAb->PosRefEdge != VertRef)
					MoveVertRef (pBox, pChildBox, val, frame);
				     /* Recalcule la position de l'axe */
				     else
				       {
					  if (pPosAb->PosUnit == UnPercent)
					     i = PixelValue (pPosAb->PosDistance, UnPercent, (PtrAbstractBox) pAb->AbBox->BxWidth, 0);
					  else
					     i = PixelValue (pPosAb->PosDistance, pPosAb->PosUnit, pAb, ViewFrameTable[frame - 1].FrMagnification);
					  i = i + pChildBox->BxXOrg + pChildBox->BxVertRef - pBox->BxXOrg;
					  MoveVertRef (pBox, pChildBox, i - pBox->BxVertRef, frame);
				       }
				  }
			     }
		  }
		pChildAb = pChildAb->AbNext;
	     }

	/* Faut-il changer la largeur de la boite englobante ? */
	if (x != 0)
	   ChangeDefaultWidth (pBox, pSourceBox, width, 0, frame);
	/* Faut-il verifier l'englobement au dessus ? */
	else if (toMove)
	   if (pAb->AbEnclosing == NULL)
	     {
		if (pBox->BxXOrg < 0)
		   XMoveAllEnclosed (pBox, -pBox->BxXOrg, frame);
	     }
	   else if (!pAb->AbEnclosing->AbInLine
		    && pAb->AbEnclosing->AbBox->BxType != BoGhost)
	      WidthPack (pAb->AbEnclosing, pSourceBox, frame);

	/* Indique que le traitement de l'englobement horizontal est termine */
	pBox->BxCycles -= 1;
     }
   /* Si la boite prend la largeur minimum, il faut quand meme      */
   /* evaluer la largeur du contenu et verifier la regle du minimum */
   else if (!pDimAb->DimIsPosition && pDimAb->DimMinimum)
     {
	GiveEnclosureSize (pAb, frame, &width, &val);
	ChangeDefaultWidth (pBox, pSourceBox, width, 0, frame);
     }
   /* Si la boite prend la largeur du pere qui a une regle minimum, il   */
   /* faut evaluer la largeur du contenu et verifier la regle du minimum */
   else if (!pDimAb->DimIsPosition && pDimAb->DimAbRef == pAb->AbEnclosing &&
	    pAb->AbEnclosing != NULL &&
	    !pAb->AbEnclosing->AbWidth.DimIsPosition &&
	    pAb->AbEnclosing->AbWidth.DimMinimum)
     {
	GiveEnclosureSize (pAb, frame, &val, &width);
	ChangeDefaultWidth (pAb->AbEnclosing->AbBox, pSourceBox, width, 0, frame);
     }

}

/*----------------------------------------------------------------------
   HeightPack  verifie l'inclusion en hauteur des boites englobees
   dans le pave pAb suite au changement de hauteur de la    
   boite origine pSourceBox.                                
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                HeightPack (PtrAbstractBox pAb, PtrBox pSourceBox, int frame)
#else  /* __STDC__ */
void                HeightPack (pAb, pSourceBox, frame)
PtrAbstractBox      pAb;
PtrBox              pSourceBox;
int                 frame;

#endif /* __STDC__ */
{
   int                 val, height;
   int                 y, i, j, k;
   PtrAbstractBox      pChildAb;
   PtrAbstractBox      pRelativeAb;
   PtrAbstractBox      pRefAb;
   PtrBox              pChildBox;
   PtrBox              pRelativeBox;
   PtrBox              pBox;
   AbDimension        *pDimAb;
   AbPosition         *pPosAb;
   ThotBool            notEmpty;
   ThotBool            toMove;
   ThotBool            absoluteMove;

   /* verifie que la hauteur de la boite depend du contenu et qu'on */
   /* n'est pas deja en train de traiter l'englobement de cette boite  */

   pBox = pAb->AbBox;
   pDimAb = &pAb->AbHeight;
   if (pBox->BxType == BoBlock)
     /* don't pack a block or a cell but transmit to enclosing box */
     HeightPack (pAb->AbEnclosing, pSourceBox, frame);
   else if (pBox->BxType == BoGhost)
     return;
   else if (pBox->BxContentHeight || (!pDimAb->DimIsPosition && pDimAb->DimMinimum))
     {

	/* indique que l'englobement vertical est en cours de traitement */
	pBox->BxPacking += 1;

	/* verifie l'encadrement et l'englobement */
	/* Faut-il prendre l'origine en relatif ou en absolu ? */
	absoluteMove = IsYPosComplete (pBox);
	if (absoluteMove)
	   y = pBox->BxYOrg;
	else
	   y = 0;

	/* Initialise la position extreme basse sur le cote superieur actuel */
	height = y;
	/* Initialise la position extreme haute sur le cote inferieur actuel */
	val = y + pBox->BxHeight;
	notEmpty = FALSE;
	/* on n'a rien deplace */
	toMove = FALSE;

	/* Le cote superieur de la plus haute boite englobee doit etre  */
	/* colle au cote superieur de la boite englobante et la hauteur */
	/* de la boite englobante est delimitee par le cote inferieur   */
	/* le plus bas des boites englobees.                          */
	pChildAb = pAb->AbFirstEnclosed;
	while (pChildAb != NULL)
	  {
	     pChildBox = pChildAb->AbBox;
	     if (!pChildAb->AbDead
		 && pChildBox != NULL
		 && pChildAb->AbVertEnclosing
		 && (pChildAb->AbHeight.DimAbRef != pAb || pChildBox->BxContentHeight))
	       {
		  /* Recherche la boite dont elle depend */
		  pRelativeBox = GetVPosRelativeBox (pChildBox, NULL);
		  if (pRelativeBox != NULL)
		     if (pRelativeBox->BxAbstractBox != NULL)
		       {
			  pRelativeAb = pRelativeBox->BxAbstractBox;
			  /* note le pave dont depend la hauteur de pChildAb */
			  pRefAb = pChildAb->AbHeight.DimAbRef;
			  if (pRelativeAb->AbVertPos.PosAbRef == NULL)
			    {
			       /* Box mobile, on tient compte de son origine */
			       notEmpty = TRUE;
			       if (pChildBox->BxYOrg < val)
				  val = pChildBox->BxYOrg;	/* valeur minimum */
			       i = pChildBox->BxYOrg + pChildBox->BxHeight;
			    }
			  /* La position de la boite depend de la taille de englobante ? */
			  else if (pRelativeAb->AbVertPos.PosAbRef == pAb
				&& (pRelativeAb->AbVertPos.PosRefEdge != Top
				    || (pRelativeAb->AbHeight.DimAbRef == pAb
					&& !pRelativeAb->AbHeight.DimIsPosition
					&& pChildAb->AbVertPos.PosRefEdge != Top)))
			     i = y + pChildBox->BxHeight;
			  /* La taille de la boite depend d'une boite externe ? */
			  else if (pChildBox->BxHOutOfStruct)
			    {
			       /* qui herite de la taille de l'englobante ? */
			       if (IsParentBox (pRefAb->AbBox, pBox)
				   && pRefAb->AbHeight.DimAbRef == NULL
				   && pRefAb->AbHeight.DimValue == 0
				   && pRefAb->AbBox->BxHeight == pBox->BxHeight)
				  i = y;
			       else
				  /* evalue l'encadrement et l'englobement */
			       if (pChildBox->BxYOrg < y)
				  i = y + pChildBox->BxHeight;
			       else
				  i = pChildBox->BxYOrg + pChildBox->BxHeight;
			    }
			  else
			  if (pChildBox->BxYOrg < y)
			     /* evalue l'encadrement et l'englobement */
			     i = y + pChildBox->BxHeight;
			  else
			     i = pChildBox->BxYOrg + pChildBox->BxHeight;
			  if (i > height)
			     height = i;
		       }
	       }
	     pChildAb = pChildAb->AbNext;
	  }

	val = -val + y;		/* Decalage de la position extreme haute */
	if (notEmpty)
	   height += val;		/* Nouvelle position extreme basse */
	if (height == y && pAb->AbVolume == 0)
	   GiveTextSize (pAb, &y, &height, &i);
	else
	   height -= y;
	y = height - pBox->BxHeight;	/* Difference de hauteur */

	/* Faut-il deplacer les boites englobees ? */
	pChildAb = pAb->AbFirstEnclosed;
	if (notEmpty && val != 0)
	   while (pChildAb != NULL)
	     {
		pChildBox = pChildAb->AbBox;
		if (!pChildAb->AbDead && pChildBox != NULL && pChildAb->AbVertEnclosing)
		  {
		     /* Est-ce une boite mobile a deplacer ? */
		     /* Recherche la boite dont elle depend */
		     pRelativeBox = GetVPosRelativeBox (pChildBox, NULL);
		     if (pRelativeBox != NULL)
			if (pRelativeBox->BxAbstractBox != NULL)
			   if (pRelativeBox->BxAbstractBox->AbVertPos.PosAbRef == NULL)
			     {
				/* met a jour la zone de reaffichage */
				if (ReadyToDisplay)
				  {
				     i = pChildBox->BxYOrg;
				     j = pChildBox->BxYOrg + pChildBox->BxHeight;
				     /* Prend en compte une zone de debordement des graphiques */
				     if (pChildBox->BxAbstractBox->AbLeafType == LtGraphics)
					k = EXTRA_GRAPH;
				     else
					k = 0;
				     if (val > 0)
					j += val;
				     else
					i += val;
				     DefClip (frame, pChildBox->BxXOrg - k, i - k, pChildBox->BxXOrg
					   + pChildBox->BxWidth + k, j + k);
				  }

				if (IsYPosComplete (pChildBox))
				   YMoveAllEnclosed (pChildBox, val, frame);
				else
				   pChildBox->BxYOrg += val;

				/* Est-ce que cette boite deplace la base de l'englobante ? */
				pPosAb = &pAb->AbHorizRef;
				if (pPosAb->PosAbRef == pChildAb)
				  {
				     toMove = TRUE;
				     pChildBox->BxMoved = NULL;
				     /* Est-ce que la relation porte sur l'axe de reference ? */
				     if (pPosAb->PosRefEdge != HorizRef)
					MoveHorizRef (pBox, pChildBox, val, frame);
				     /* Recalcule la position de l'axe */
				     else
				       {
					  if (pPosAb->PosUnit == UnPercent)
					     i = PixelValue (pPosAb->PosDistance, UnPercent, (PtrAbstractBox) pAb->AbBox->BxHeight, 0);
					  else
					     i = PixelValue (pPosAb->PosDistance, pPosAb->PosUnit, pAb, ViewFrameTable[frame - 1].FrMagnification);
					  i = i + pChildBox->BxYOrg + pChildBox->BxHorizRef - pBox->BxYOrg;
					  MoveHorizRef (pBox, pChildBox, i - pBox->BxHorizRef, frame);
				       }
				  }
			     }
		  }
		pChildAb = pChildAb->AbNext;
	     }

	/* Faut-il changer la hauteur de la boite englobante ? */
	if (y != 0)
	   ChangeDefaultHeight (pBox, pSourceBox, height, frame);
	/* Faut-il verifier l'englobement au dessus ? */
	else if (toMove)
	   if (pAb->AbEnclosing == NULL)
	     {
		if (pBox->BxYOrg < 0)
		   YMoveAllEnclosed (pBox, -pBox->BxYOrg, frame);
	     }
	   else if (pAb->AbEnclosing->AbInLine)
	      EncloseInLine (pBox, frame, pAb->AbEnclosing);
	   else if (pAb->AbEnclosing->AbBox->BxType == BoGhost ||
		    pAb->AbEnclosing->AbBox->BxType == BoBlock)
	     {
		/* Il faut remonter au pave de mise en lignes */
		while (pAb->AbEnclosing->AbBox->BxType == BoGhost)
		   pAb = pAb->AbEnclosing;
		EncloseInLine (pBox, frame, pAb->AbEnclosing);
	     }
	   else
	      HeightPack (pAb->AbEnclosing, pSourceBox, frame);

	/* indique que le traitement de l'englobement vertical est termine */
	pBox->BxPacking -= 1;
     }
   /* Si la boite prend la hauteur minimum, il faut quand meme      */
   /* evaluer la hauteur du contenu et verifier la regle du minimum */
   else if (!pDimAb->DimIsPosition && pDimAb->DimMinimum)
     {
	GiveEnclosureSize (pAb, frame, &val, &height);
	ChangeDefaultHeight (pBox, pSourceBox, height, frame);
     }
   /* Si la boite prend la hauteur du pere qui a une regle minimum, il   */
   /* faut evaluer la hauteur du contenu et verifier la regle du minimum */
   else if (!pDimAb->DimIsPosition && pDimAb->DimAbRef == pAb->AbEnclosing &&
	    pAb->AbEnclosing != NULL &&
	    !pAb->AbEnclosing->AbHeight.DimIsPosition &&
	    pAb->AbEnclosing->AbHeight.DimMinimum)
     {
	GiveEnclosureSize (pAb, frame, &val, &height);
	ChangeDefaultHeight (pAb->AbEnclosing->AbBox, pSourceBox, height, frame);
     }

}
