/*
   rel.c : gestion des Relations entre boites
   I. Vatton
 */


#include "thot_sys.h"
#include "thot_gui.h"
#include "libmsg.h"
#include "message.h"
#include "constmedia.h"
#include "typemedia.h"
#include "frame.h"

#define EXPORT extern
#include "img.var"

#include "memory_f.h"
#include "appli_f.h"
#include "boxmoves_f.h"
#include "absboxes_f.h"
#include "font_f.h"


/* ---------------------------------------------------------------------- */
/* |  GetPosRelativeAb retourne le pointeur sur le pave de reference    | */
/* |            pour le positionnement implicite, horizontal/vertical,  | */
/* |            du pave pCurrentAb.                                     | */
/* |            Si la valeur rendue est NULL, le pave se positionne par | */
/* |            rapport au pave englobant.                              | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static PtrAbstractBox   GetPosRelativeAb (PtrAbstractBox pCurrentAb, boolean horizRef)
#else  /* __STDC__ */
static PtrAbstractBox   GetPosRelativeAb (pCurrentAb, horizRef)
PtrAbstractBox      pCurrentAb;
boolean             horizRef;
#endif /* __STDC__ */
{
   boolean          still;
   PtrAbstractBox   pAb;

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


/* ---------------------------------------------------------------------- */
/* |  InsertPosRelation etablit les liens de dependance entre les deux  | */
/* |            repers des boites :                                     | */
/* |            - double sens dans le cas du positionnement entre deux  | */
/* |            soeurs.                                                 | */
/* |            - sens unique dans le cas du positionnement entre fille | */
/* |            et mere.                                                | */
/* |            - sens repere vers axe pour la definition des axes de   | */
/* |            reference (un deplacement du repere de pTargetBox       | */
/* |            modifie l'axe de reference de pOrginBox).               | */
/* |            - sens inverse dans le cas d'une dimension elastique    | */
/* |            (un deplacement du repere de pTargetBox modifie la      | */
/* |            dimension de pOrginBox).                                | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void       RelPos (PtrBox pOrginBox, PtrBox pTargetBox, OpRelation op, BoxEdge originEdge, BoxEdge targetEdge)
#else  /* __STDC__ */
static void       RelPos (pOrginBox, pTargetBox, op, originEdge, targetEdge)
PtrBox            pOrginBox;
PtrBox            pTargetBox;
OpRelation        op;
BoxEdge           originEdge;
BoxEdge           targetEdge;
#endif /* __STDC__ */
{
   int                 i;
   boolean             loop;
   boolean             empty;
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
		  i++;
		  empty = (pPosRel->PosRTable[i - 1].ReBox == NULL);
	       }
	     while (i != MAX_RELAT_POS && empty);

	     if (empty)
		loop = FALSE;
	     else
		pPosRel = pPosRel->PosRNext;
	     /* Bloc suivant */
	  }

	/* Faut-il creer un nouveau bloc de relations ? */
	if (loop)
	  {
	     GetBPos (&pPosRel);
	     if (pPreviousPosRel == NULL)
		pOrginBox->BxPosRelations = pPosRel;
	     else
		pPreviousPosRel->PosRNext = pPosRel;
	     i = 1;
	  }
	pRelation = &pPosRel->PosRTable[i - 1];
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
		  i++;
		  empty = pPosRel->PosRTable[i - 1].ReBox == NULL;
	       }
	     while (i != MAX_RELAT_POS && !empty);

	     if (empty)
		loop = FALSE;
	     else
	       /* Bloc suivant */
	       pPosRel = pPosRel->PosRNext;
	  }

	/* Faut-il creer un nouveau bloc de relations ? */
	if (loop)
	  {
	     GetBPos (&pPosRel);
	     if (pPreviousPosRel == NULL)
		pTargetBox->BxPosRelations = pPosRel;
	     else
		pPreviousPosRel->PosRNext = pPosRel;
	     i = 1;
	  }
	pRelation = &pPosRel->PosRTable[i - 1];
	pRelation->ReRefEdge = targetEdge;
	pRelation->ReBox = pOrginBox;
	pRelation->ReOp = op;
     }
}


/* ---------------------------------------------------------------------- */
/* | InsertDimRelation etablit le lien entre les dimensions             | */
/* |         horizontales ou verticales des deux boites (pOrginBox vers | */
/* |         pTargetBox).                                               | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void       RelDim (PtrBox pOrginBox, PtrBox pTargetBox, boolean sameDimension, boolean horizRef)
#else  /* __STDC__ */
static void       RelDim (pOrginBox, pTargetBox, sameDimension, horizRef)
PtrBox            pOrginBox;
PtrBox            pTargetBox;
boolean           sameDimension;
boolean           horizRef;
#endif /* __STDC__ */
{
   int                 i;
   boolean             loop;
   boolean             empty;
   PtrDimRelations     pPreviousDimRel;
   PtrDimRelations     pDimRel;

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
	     i++;
	     empty = pDimRel->DimRTable[i - 1] == NULL;
	  }
	while (i != MAX_RELAT_DIM && !empty);

	if (empty)
	   loop = FALSE;
	else
	   pDimRel = pDimRel->DimRNext;
     }

   /* Faut-il creer un nouveau bloc de relations ? */
   if (loop)
     {
	GetBDim (&pDimRel);
	if (pPreviousDimRel == NULL)
	   if (horizRef)
	      pOrginBox->BxWidthRelations = pDimRel;
	   else
	      pOrginBox->BxHeightRelations = pDimRel;
	else
	   pPreviousDimRel->DimRNext = pDimRel;
	i = 1;
     }

   pDimRel->DimRTable[i - 1] = pTargetBox;
   pDimRel->DimRSame[i - 1] = sameDimension;
}


/* ---------------------------------------------------------------------- */
/* |  PropagateXOutOfStruct propage l'indicateur hors-structure.        | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         PropagateXOutOfStruct (PtrAbstractBox pCurrentAb, boolean status, boolean enclosed)
#else  /* __STDC__ */
static void         PropagateXOutOfStruct (pCurrentAb, status, enclosed)
PtrAbstractBox      pCurrentAb;
boolean             status;
boolean             enclosed;
#endif /* __STDC__ */
{
   PtrAbstractBox    pAb;
   PtrBox            pBox;

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
	     /* Propage aussi le non englobement */
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


/* ---------------------------------------------------------------------- */
/* |    PropagateYOutOfStruct propage l'indicateur hors-structure.         | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         PropagateYOutOfStruct (PtrAbstractBox pCurrentAb, boolean status, boolean enclosed)
#else  /* __STDC__ */
static void         PropagateYOutOfStruct (pCurrentAb, status, enclosed)
PtrAbstractBox      pCurrentAb;
boolean             status;
boolean             enclosed;
#endif /* __STDC__ */
{
   PtrAbstractBox    pAb;
   PtrBox            pBox;

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
	     /* Propage aussi le non englobement */
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


/* ---------------------------------------------------------------------- */
/* |    Positionner applique la regle de positionnement, horizontale ou | */
/* |            verticale selon horizRef sur la boite d'indice pBox.    | */
/* |            Les origines de la boite BxXOrg ou BxYOrg sont mises a` | */
/* |            jour et les relations de dependance sont enregistrees.  | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void              Positionner (AbPosition rule, PtrBox pBox, int frame, boolean horizRef)
#else  /* __STDC__ */
void              Positionner (rule, pBox, frame, horizRef)
AbPosition        rule;
PtrBox            pBox;
int               frame;
boolean           horizRef;
#endif /* __STDC__ */
{
   int               x, y, dist;
   PtrAbstractBox    pAb;
   PtrAbstractBox    pCurrentAb;
   PtrBox            pCurrentBox;
   BoxEdge           refEdge, localEdge;
   OpRelation        op;
   AbPosition       *pPosAb;

   /* On calcule la position de reference */
   op = (OpRelation) 0;
   refEdge = (BoxEdge) 0;
   DimFenetre (frame, &x, &y);
   pCurrentBox = NULL;
   pAb = rule.PosAbRef;
   pCurrentAb = pBox->BxAbstractBox;
   /* Verifie que la position ne depend pas d'un pave mort */
   if (pAb != NULL && pAb->AbDead)
     {
	printf ("Position relative to a dead box");
	pAb = NULL;
     }

   if (horizRef)
     {
	/* On verifie que la boite ne se place pas par rapport a elle-meme */
	if (pAb == pCurrentAb)
	  {
	     pCurrentAb->AbHorizPos.PosAbRef = NULL;
	     if (pCurrentAb->AbWidth.DimIsPosition)
	       {
		  pCurrentAb->AbWidth.DimIsPosition = FALSE;
		  pCurrentAb->AbWidth.DimUserSpecified = FALSE;
	       }
	     /* il ne faut pas interpreter cette regle */
	     pAb = NULL;
	  }

	/* SRule par defaut */
	if (pAb == NULL)
	  {
	     if (pCurrentAb->AbEnclosing == NULL)
	       {
		  /* Position dans la fenetre */
		  refEdge = rule.PosRefEdge;
		  localEdge = rule.PosEdge;
		  /* Convert the distance value */
		  if (rule.PosUnit == UnPercent)
		     dist = PixelValue (rule.PosDistance, UnPercent, (PtrAbstractBox) x);
		  else
		     dist = PixelValue (rule.PosDistance, rule.PosUnit, pCurrentAb);
	       }
	     else
	       {
		  /* Il existe un pave precedent ayant la regle par defaut ? */
		  pAb = GetPosRelativeAb (pCurrentAb, horizRef);
		  /* Si oui -> A droite de sa boite */
		  if (pAb != NULL && rule.PosUnit != UnPercent)
		    {
		       pCurrentBox = pAb->AbBox;
		       dist = 0;	/* A droite de la precedente */
		       refEdge = Right;
		       localEdge = Left;
		       op = OpHorizDep;
		    }
		  else
		    {
		       /* Initialise sur l'origine de l'englobante (flottante) */
		       pAb = pCurrentAb->AbEnclosing;
		       pCurrentBox = pAb->AbBox;
		       if (rule.PosUnit == UnPercent)
			  /* poucentage de la largeur de l'englobant */
			  dist = PixelValue (rule.PosDistance, UnPercent, (PtrAbstractBox) pCurrentAb->AbEnclosing);
		       else
			  dist = 0;
		       refEdge = Left;
		       localEdge = Left;
		       if (pBox->BxHorizFlex)
			  /* On traite comme une relation voisine */
			  op = OpHorizDep;
		       else
			  op = OpHorizInc;
		    }
	       }
	  }
	/* SRule explicite */
	else
	  {
	     /* La position d'une boite elastique est toujours traitee comme voisinage */
	     if (pCurrentAb->AbEnclosing == pAb && !pBox->BxHorizFlex)
		op = OpHorizInc;
	     else
	       {
		  op = OpHorizDep;
		  pBox->BxXToCompute = FALSE;	/* nouvelle regle de placement */
		  if (pAb->AbEnclosing != pCurrentAb->AbEnclosing)
		    {
		       /* La boite est liee a une boite hors-structure */
		       if (!XEnAbsolu (pBox))
			  /* la boite  devient maintenant placee en absolu */
			  pBox->BxXToCompute = TRUE;
		       pBox->BxXOutOfStruct = TRUE;
		       PropagateXOutOfStruct (pCurrentAb, TRUE, pCurrentAb->AbHorizEnclosing);
		    }
		  else if (pAb->AbBox != NULL)
		    {
		       if (pAb->AbBox->BxXOutOfStruct
			   || (pAb->AbBox->BxHorizFlex)
			   || (pAb->AbBox->BxWOutOfStruct && refEdge != Left))
			 {
			    /* La boite herite la relation hors-structure */
			    /* ou bien elle est liee a une dimension hors-structure */
			    if (!XEnAbsolu (pBox))
			       /* la boite  devient maintenant placee en absolu */
			       pBox->BxXToCompute = TRUE;
			    pBox->BxXOutOfStruct = TRUE;
			    PropagateXOutOfStruct (pCurrentAb, TRUE, pCurrentAb->AbHorizEnclosing);
			 }
		    }

	       }

	     refEdge = rule.PosRefEdge;
	     localEdge = rule.PosEdge;
	     /* Convert the distance value */
	     if (rule.PosUnit == UnPercent)
	       {
		  dist = PixelValue (rule.PosDistance, UnPercent, (PtrAbstractBox) pCurrentAb->AbEnclosing->AbBox->BxWidth);
		  /* Change the rule for further updates */
		  pCurrentAb->AbHorizPos.PosDistance = dist;
		  pCurrentAb->AbHorizPos.PosUnit = UnPixel;
	       }
	     else
		dist = PixelValue (rule.PosDistance, rule.PosUnit, pCurrentAb);
	  }
     }
   else
      /* EnY */
     {
	/* On verifie que la boite ne se place pas par rapport a elle-meme */
	if (pAb == pCurrentAb)
	  {
	     pCurrentAb->AbVertPos.PosAbRef = NULL;
	     if (pCurrentAb->AbHeight.DimIsPosition)
	       {
		  pCurrentAb->AbHeight.DimIsPosition = FALSE;
		  pCurrentAb->AbHeight.DimUserSpecified = FALSE;
	       }
	     pAb = NULL;	/* Il ne faut pas interpreter cette regle */
	  }

	/* SRule par defaut */
	if (pAb == NULL)
	  {
	     if (pCurrentAb->AbEnclosing == NULL)
	       {
		  /* Position dans la fenetre */
		  refEdge = rule.PosRefEdge;
		  localEdge = rule.PosEdge;
		  /* Convert the distance value */
		  if (rule.PosUnit == UnPercent)
		     dist = PixelValue (rule.PosDistance, UnPercent, (PtrAbstractBox) y);
		  else
		     dist = PixelValue (rule.PosDistance, rule.PosUnit, pCurrentAb);
	       }
	     else
	       {
		  /* Il existe un pave precedent ayant la regle par defaut ? */
		  pAb = GetPosRelativeAb (pCurrentAb, horizRef);
		  /* Si oui -> A droite de sa boite */
		  if (pAb != NULL)
		    {
		       pCurrentBox = pAb->AbBox;
		       dist = 0;	/* A droite de la precedente */
		       refEdge = HorizRef;
		       localEdge = HorizRef;
		       op = OpVertDep;
		    }
		  else
		    {
		       /* Initialise sur l'origine de l'englobante (flottante) */
		       pAb = pCurrentAb->AbEnclosing;
		       pCurrentBox = pAb->AbBox;
		       dist = 0;
		       refEdge = Top;
		       localEdge = Top;
		       if (pBox->BxVertFlex)
			  /* On traite comme une relation voisine */
			  op = OpVertDep;
		       else
			  op = OpVertInc;
		    }
	       }
	  }
	/* SRule explicite */
	else
	  {
	     /* La position d'une boite elastique est toujours traitee comme voisinage */
	     if (pCurrentAb->AbEnclosing == pAb && !pBox->BxVertFlex)
		op = OpVertInc;
	     else
	       {
		  op = OpVertDep;
		  pBox->BxYToCompute = FALSE;	/* nouvelle regle de placement */
		  if (pAb->AbEnclosing != pCurrentAb->AbEnclosing)
		    {
		       /* La boite est liee a une boite hors-structure */
		       if (!YEnAbsolu (pBox))
			  /* la boite  est maintenant placee en absolu */
			  pBox->BxYToCompute = TRUE;
		       pBox->BxYOutOfStruct = TRUE;
		       PropagateYOutOfStruct (pCurrentAb, TRUE, pCurrentAb->AbVertEnclosing);
		    }
		  else if (pAb->AbBox != NULL)
		    {
		       if (pAb->AbBox->BxYOutOfStruct
			   || (pAb->AbBox->BxHorizFlex && pAb->AbLeafType == LtCompound && pAb->AbInLine && refEdge != Top)
			   || (pAb->AbBox->BxHOutOfStruct && refEdge != Top))
			 {
			    /* La boite herite la relation hors-structure */
			    /* ou bien elle est liee a une dimension hors-structure */
			    if (!YEnAbsolu (pBox))
			       /* la boite  est maintenant placee en absolu */
			       pBox->BxYToCompute = TRUE;
			    pBox->BxYOutOfStruct = TRUE;
			    PropagateYOutOfStruct (pCurrentAb, TRUE, pCurrentAb->AbVertEnclosing);
			 }
		    }
	       }

	     refEdge = rule.PosRefEdge;
	     localEdge = rule.PosEdge;
	     /* Convert the distance value */
	     if (rule.PosUnit == UnPercent)
	       {
		  dist = PixelValue (rule.PosDistance, UnPercent, (PtrAbstractBox) pCurrentAb->AbEnclosing->AbBox->BxHeight);
		  /* Change the rule for further updates */
		  pCurrentAb->AbVertPos.PosDistance = dist;
		  pCurrentAb->AbVertPos.PosUnit = UnPixel;
	       }
	     else
		dist = PixelValue (rule.PosDistance, rule.PosUnit, pCurrentAb);
	  }
     }

   /* Deplacement par rapport a la boite distante */
   if (pAb != NULL)
     {
	pCurrentBox = pAb->AbBox;
	if (pCurrentBox == NULL)
	  {
	     /* On doit resoudre une reference en avant */
	     pCurrentBox = GetBox (pAb);
	     if (pCurrentBox != NULL)
		pAb->AbBox = pCurrentBox;
	     else
		/* plus de boite libre */
		return;
	  }

	x = pCurrentBox->BxXOrg;
	y = pCurrentBox->BxYOrg;
	switch (refEdge)
	      {
		 case Left:
		 case Top:
		    break;
		 case Bottom:
		    y += pCurrentBox->BxHeight;
		    break;
		 case Right:
		    x += pCurrentBox->BxWidth;
		    break;
		 case HorizRef:
		    y += pCurrentBox->BxHorizRef;
		    break;
		 case VertRef:
		    x += pCurrentBox->BxVertRef;
		    break;
		 case HorizMiddle:
		    y += pCurrentBox->BxHeight / 2;
		    break;
		 case VertMiddle:
		    x += pCurrentBox->BxWidth / 2;
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
   switch (localEdge)
	 {
	    case Left:
	       pBox->BxHorizEdge = Left;
	       break;
	    case Right:
	       x -= pBox->BxWidth;
	       pBox->BxHorizEdge = Right;
	       break;
	    case VertMiddle:
	       x -= pBox->BxWidth / 2;
	       pBox->BxHorizEdge = VertMiddle;
	       break;
	    case VertRef:
	       x -= pBox->BxVertRef;
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
	       y -= pBox->BxHeight;
	       pBox->BxVertEdge = Bottom;
	       break;
	    case HorizMiddle:
	       y -= pBox->BxHeight / 2;
	       pBox->BxVertEdge = HorizMiddle;
	       break;
	    case HorizRef:
	       y -= pBox->BxHorizRef;
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
	       if (horizRef)
		  pBox->BxHorizEdge = Left;
	       else
		  pBox->BxVertEdge = Top;
	       break;
	 }

   /* regarde si la position depend d'une boite invisible */
   if (pBox->BxAbstractBox->AbVisibility < FntrTable[frame - 1].FrVisibility)
      dist = 0;
   else if (pAb != NULL)
      if (pAb->AbVisibility < FntrTable[frame - 1].FrVisibility)
	 dist = 0;

   /* Met a jour l'origine de la boite suivant la relation indiquee */
   if (horizRef)
     {
	if (!pBox->BxHorizFlex)
	  {
	     x = x + dist - pBox->BxXOrg;
	     if (x == 0 && pBox->BxXToCompute)
		/* Force le placement des boites filles */
		DepXContenu (pBox, x, frame);
	     else
		DepOrgX (pBox, NULL, x, frame);
	  }
	pCurrentAb->AbHorizPosChange = FALSE;	/* la regle de position est interpretee */
     }
   else
     {
	if (!pBox->BxVertFlex)
	  {
	     y = y + dist - pBox->BxYOrg;
	     if (y == 0 && pBox->BxYToCompute)
		/* Force le placement des boites filles */
		DepYContenu (pBox, y, frame);
	     else
		DepOrgY (pBox, NULL, y, frame);
	  }
	pCurrentAb->AbVertPosChange = FALSE;	/* la regle de position est interpretee */
     }

   /* Il faut mettre a jour les dependances des boites */
   if (pAb != NULL && pCurrentBox != NULL)
     {
	RelPos (pBox, pCurrentBox, op, localEdge, refEdge);
	pCurrentBox->BxMoved = NULL;

	if (horizRef && pBox->BxHorizFlex)
	   ChngBElast (pBox, pCurrentBox, op, x + dist - pBox->BxXOrg, frame, TRUE);
	else if (!horizRef && pBox->BxVertFlex)
	   ChngBElast (pBox, pCurrentBox, op, y + dist - pBox->BxYOrg, frame, FALSE);
     }
}				/* Positionner */

/* ---------------------------------------------------------------------- */
/* |    Dimensionner applique la regle de dimensionnement, horizontale  | */
/* |            ou verticale selon horizRef du pave pAb.                  | */
/* |            Si la dimension depend du contenu de la boite la        | */
/* |            fonction rend la valeur Vrai.                           | */
/* |            Si au contraire, elle depend de celle d'un autre pave,  | */
/* |            elle est mise a` jour et la relation de dependance entre| */
/* |            les boites est enregistree.                             | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
boolean             Dimensionner (PtrAbstractBox pAb, int frame, boolean horizRef)
#else  /* __STDC__ */
boolean             Dimensionner (pAb, frame, horizRef)
PtrAbstractBox      pAb;
int                 frame;
boolean             horizRef;
#endif /* __STDC__ */
{
   int               val, delta, i;
   PtrBox            pCurrentBox;
   PtrAbstractBox    pCurrentAb;
   boolean           misenligne;
   boolean           setdim;
   ptrfont           font;
   OpRelation        op;
   AbDimension      *pDimAb;
   AbPosition       *pPosAb;
   PtrAbstractBox    cepave;
   PtrBox            pBo1;

   pBo1 = pAb->AbBox;
   setdim = FALSE;		/* A priori, la dimension ne depend pas de son contenu */

   /* On verifie que la boite est visible */
   if (pAb->AbVisibility >= FntrTable[frame - 1].FrVisibility)
     {
	pCurrentAb = pAb->AbEnclosing;
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
		      || pAb->AbWidth.DimPosition.PosEdge == VertRef
		      || pAb->AbWidth.DimPosition.PosAbRef == NULL
		      || pAb->AbWidth.DimPosition.PosAbRef == pAb)
	       {
		  /* Il y a une erreur de dimension */
		  /* Erreur sur le schema de presentation */
		  TtaDisplayMessage (INFO, TtaGetMessage(LIB, BAD_HORIZ_SIZING_SEE_PRS_SCH), AbsBoxType (pAb));
		  pAb->AbWidth.DimIsPosition = FALSE;
		  pAb->AbWidth.DimAbRef = NULL;
		  pAb->AbWidth.DimValue = 20;	/* largeur fixe */
		  pAb->AbWidth.DimUnit = UnPoint;
		  pAb->AbWidth.DimUserSpecified = FALSE;
	       }

	     /* verifie que la dimension ne depend pas d'un pave mort */
	     else if (pAb->AbHorizPos.PosAbRef->AbDead)
	       {
		  printf ("Dimension relative to a dead box");
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
		      || pAb->AbHeight.DimPosition.PosEdge == HorizRef
		      || pAb->AbHeight.DimPosition.PosAbRef == NULL
		      || pAb->AbHeight.DimPosition.PosAbRef == pAb)
	       {
		  /* Il y a une erreur de dimension */
		  /* Erreur sur le schema de presentation */
		  TtaDisplayMessage (INFO, TtaGetMessage(LIB, BAD_VERT_SIZING_SEE_PRS_SCH), AbsBoxType (pAb));
		  pAb->AbHeight.DimIsPosition = FALSE;
		  pAb->AbHeight.DimAbRef = NULL;
		  pAb->AbHeight.DimValue = 20;	/* hauteur fixe */
		  pAb->AbHeight.DimUnit = UnPoint;
		  pAb->AbHeight.DimUserSpecified = FALSE;
	       }

	     /* verifie que la dimension ne depend pas d'un pave mort */
	     else if (pAb->AbVertPos.PosAbRef->AbDead)
	       {
		  printf ("Dimension relative to a dead box");
		  pAb->AbHeight.DimIsPosition = FALSE;
		  pAb->AbHeight.DimAbRef = NULL;
		  pAb->AbHeight.DimValue = 20;	/* hauteur fixe */
		  pAb->AbHeight.DimUnit = UnPoint;
		  pAb->AbHeight.DimUserSpecified = FALSE;
	       }
	  }

	/* Est-ce une boite non-elastique ? */
	if ((horizRef && !pAb->AbWidth.DimIsPosition) || (!horizRef && !pAb->AbHeight.DimIsPosition))
	  {
	     font = pBo1->BxFont;
	     /* Est-ce que la dimension est exprimee en points typo. ? */
	     if (horizRef)
		pDimAb = &pAb->AbWidth;
	     else
		pDimAb = &pAb->AbHeight;

	     /* verifie que la dimension ne depend pas d'un pave mort */
	     if (pDimAb->DimAbRef != NULL && pDimAb->DimAbRef->AbDead)
	       {
		  printf ("Dimension relative to a dead box");
		  pDimAb->DimAbRef = NULL;
		  pDimAb->DimValue = 0;
	       }

	     /* Est-ce la boite racine ? */
	     if (pCurrentAb == NULL)
		/* la largeur est contrainte (par heritage ou imposee) ? */
		if (horizRef)
		  {
		     if (pDimAb->DimValue == 0)
			setdim = TRUE;
		     else
		       {
			  DimFenetre (frame, &val, &i);
			  if (pDimAb->DimValue < 0)
			     val += pDimAb->DimValue;
			  else if (pDimAb->DimUnit == UnPercent)
			     val = PixelValue (pDimAb->DimValue, UnPercent, (PtrAbstractBox) val);
			  else
			     val = PixelValue (pDimAb->DimValue, pDimAb->DimUnit, pAb);
			  ModLarg (pBo1, pBo1, NULL, val - pBo1->BxWidth, 0, frame);
		       }
		  }
	     /* la hauteur est contrainte (par heritage ou imposee) ? */
		else
		  {
		     if (pDimAb->DimValue == 0)
			setdim = TRUE;
		     else
		       {
			  DimFenetre (frame, &i, &val);
			  if (pDimAb->DimValue < 0)
			     val = 0;
			  else if (pDimAb->DimUnit == UnPercent)
			     val = PixelValue (pDimAb->DimValue, UnPercent, (PtrAbstractBox) val);
			  else
			     val = PixelValue (pDimAb->DimValue, pDimAb->DimUnit, pAb);
			  ModHaut (pBo1, pBo1, NULL, val - pBo1->BxHeight, frame);
		       }
		  }

	     /* Ce n'est pas la boite racine */
	     else
	       {
		  misenligne = pCurrentAb->AbInLine || pCurrentAb->AbBox->BxType == BoGhost;
		  /* La largeur est contrainte (par heritage ou imposee) ? */
		  if (horizRef)
		     /* PcFirst cas de coherence */
		     /* Le texte mis en ligne DOIT prendre sa taille */
		     if (misenligne && pAb->AbLeafType == LtText)
			setdim = TRUE;
		  /* Dimension fixee */
		     else if (pAb->AbWidth.DimAbRef == NULL)
			if (pAb->AbWidth.DimValue <= 0)
			   setdim = TRUE;	/* A calculer */
			else
			  {
			     /* Convert the distance value */
			     if (pDimAb->DimUnit == UnPercent)
			       {
				  delta = PixelValue (pDimAb->DimValue, UnPercent, (PtrAbstractBox) pCurrentAb->AbBox->BxWidth);
				  RelDim (pCurrentAb->AbBox, pBo1, pDimAb->DimSameDimension, horizRef);
			       }
			     else
				delta = PixelValue (pDimAb->DimValue, pDimAb->DimUnit, pAb);
			     ModLarg (pBo1, pBo1, NULL, delta - pBo1->BxWidth, 0, frame);
			  }
		  /* Deuxieme cas de coherence */
		  /* La boite ne peut pas prendre la taille de son englobante si : */
		  /* -> L'englobante prend la taille de son contenu */
		     else
		       {
			  pPosAb = &pAb->AbHorizPos;
			  if (pAb->AbWidth.DimAbRef == pCurrentAb && pCurrentAb->AbEnclosing != NULL
			      && pCurrentAb->AbWidth.DimAbRef == NULL && pCurrentAb->AbWidth.DimValue <= 0
			  /* -> ET l'englobante est mise en lignes (extensibles) */
			  /* OU la boite n'est pas collee au cote gauche de son englobante */
			      && (misenligne || pPosAb->PosAbRef != pCurrentAb
				  || pPosAb->PosRefEdge != Left || pPosAb->PosEdge != Left))
			    {
			       pDimAb = &pAb->AbWidth;
			       setdim = TRUE;
			       pDimAb->DimAbRef = NULL;
			       pDimAb->DimValue = 0;
			       pDimAb->DimUnit = UnRelative;
			    }
			  /* La dimension ne peut dependre d'elle-meme */
			  else
			    {
			       pDimAb = &pAb->AbWidth;
			       if (pDimAb->DimAbRef == pAb && pDimAb->DimSameDimension)
				 {
				    setdim = TRUE;
				    pDimAb->DimAbRef = NULL;
				    pDimAb->DimValue = 0;
				    pDimAb->DimUnit = UnRelative;
				 }
			       /* Herite de la dimension d'une autre boite */
			       else
				 {
				    pCurrentBox = pDimAb->DimAbRef->AbBox;
				    if (pCurrentBox == NULL)
				      {
					 /* On doit resoudre une reference en avant */
					 pCurrentBox = GetBox (pDimAb->DimAbRef);
					 if (pCurrentBox != NULL)
					    pDimAb->DimAbRef->AbBox = pCurrentBox;
				      }

				    if (pCurrentBox != NULL)
				      {
					 /* regarde s'il s'agit de la meme dimension */
					 if (pDimAb->DimSameDimension)
					    val = pCurrentBox->BxWidth;
					 else
					    val = pCurrentBox->BxHeight;
					 /* Quand la boite prend la largeur de la ligne qui */
					 /* l'englobe ->retire la valeur de l'indentation   */
					 if (pDimAb->DimAbRef == pCurrentAb && misenligne && pDimAb->DimSameDimension)
					   {
					      if (pCurrentAb->AbIndentUnit == UnPercent)
						 delta = PixelValue (pCurrentAb->AbIndent, UnPercent, (PtrAbstractBox) val);
					      else
						 delta = PixelValue (pCurrentAb->AbIndent, pCurrentAb->AbIndentUnit, pCurrentAb);
					      if (pCurrentAb->AbIndent > 0)
						 val -= delta;
					      else if (pCurrentAb->AbIndent < 0)
						 val += delta;
					   }

					 /* Convert the distance value */
					 if (pDimAb->DimUnit == UnPercent)
					    val = PixelValue (pDimAb->DimValue, UnPercent, (PtrAbstractBox) val);
					 else
					    val += PixelValue (pDimAb->DimValue, pDimAb->DimUnit, pAb);
					 ModLarg (pBo1, pBo1, NULL, val - pBo1->BxWidth, 0, frame);
					 /* On teste si la relation est hors structure */
					 if (pDimAb->DimAbRef != pCurrentAb
					     && pDimAb->DimAbRef->AbEnclosing != pCurrentAb)
					    pBo1->BxWOutOfStruct = TRUE;

					 /* On note les dependances des largeurs des boites */
					 RelDim (pDimAb->DimAbRef->AbBox, pBo1, pDimAb->DimSameDimension, horizRef);
				      }
				 }
			    }
		       }
		  /* la hauteur est contrainte (par heritage ou imposee) ? */
		  else
		     /* PcFirst cas de coherence */
		     /* / Le texte mis en ligne DOIT prendre sa taille */
		  if (misenligne && pAb->AbLeafType == LtText)
		     setdim = TRUE;
		  /* Dimension fixee */
		  else if (pAb->AbHeight.DimAbRef == NULL)
		     if (pAb->AbHeight.DimValue == 0)
			setdim = TRUE;	/* A calculer */
		     else
		       {
			  /* Convert the distance value */
			  if (pDimAb->DimUnit == UnPercent)
			     delta = PixelValue (pDimAb->DimValue, UnPercent, (PtrAbstractBox) pCurrentAb->AbBox->BxHeight);
			  else
			     delta = PixelValue (pDimAb->DimValue, pDimAb->DimUnit, pAb);
			  ModHaut (pBo1, pBo1, NULL, delta - pBo1->BxHeight, frame);
		       }
		  /* Deuxieme cas de coherence */
		  /* La boite ne peut pas prendre la taille de son englobante si : */
		  /* -> L'englobante est mise en ligne */
		  else if (misenligne && pAb->AbHeight.DimAbRef == pCurrentAb
			   && (pAb->AbLeafType == LtPicture || pAb->AbLeafType == LtCompound))
		    {
		       pDimAb = &pAb->AbHeight;
		       setdim = TRUE;
		       pDimAb->DimAbRef = NULL;
		       pDimAb->DimValue = 0;
		       pDimAb->DimUnit = UnRelative;
		    }
		  /* Troisieme cas de coherence */
		  /* La boite ne peut pas prendre la taille de son englobante si : */
		  /* -> L'englobante prend la taille de son contenu */
		  else
		    {
		       pPosAb = &pAb->AbVertPos;
		       if (pAb->AbHeight.DimAbRef == pCurrentAb
			   && pCurrentAb->AbEnclosing != NULL
			   && pCurrentAb->AbHeight.DimAbRef == NULL && pCurrentAb->AbHeight.DimValue <= 0
		       /* ET la boite n'est pas collee au cote superieur de son englobante */
			   && (pPosAb->PosAbRef != pCurrentAb || pPosAb->PosRefEdge != Top
			       || pPosAb->PosEdge != Top))
			 {
			    pDimAb = &pAb->AbHeight;
			    setdim = TRUE;
			    pDimAb->DimAbRef = NULL;
			    pDimAb->DimValue = 0;
			    pDimAb->DimUnit = UnRelative;
			 }
		       /* La dimension ne peut dependre d'elle-meme */
		       else
			 {
			    pDimAb = &pAb->AbHeight;
			    if (pDimAb->DimAbRef == pAb && pDimAb->DimSameDimension)
			      {
				 setdim = TRUE;
				 pDimAb->DimAbRef = NULL;
				 pDimAb->DimValue = 0;
				 pDimAb->DimUnit = UnRelative;
			      }
			    /* Herite de la dimension d'une autre boite */
			    else
			      {
				 pCurrentBox = pDimAb->DimAbRef->AbBox;
				 if (pCurrentBox == NULL)
				   {
				      /* On doit resoudre une reference en avant */
				      pCurrentBox = GetBox (pDimAb->DimAbRef);
				      if (pCurrentBox != NULL)
					 pDimAb->DimAbRef->AbBox = pCurrentBox;
				   }

				 if (pCurrentBox != NULL)
				   {
				      /* regarde s'il s'agit de la meme dimension */
				      if (pDimAb->DimSameDimension)
					 val = pCurrentBox->BxHeight;
				      else
					 val = pCurrentBox->BxWidth;

				      /* Quand la boite prend la largeur de la ligne qui */
				      /* l'englobe -> retire la valeur de l'indentation  */
				      if (pDimAb->DimAbRef == pCurrentAb && misenligne
					  && !pDimAb->DimSameDimension)
					{
					   if (pCurrentAb->AbIndentUnit == UnPercent)
					      delta = PixelValue (pCurrentAb->AbIndent, UnPercent, (PtrAbstractBox) val);
					   else
					      delta = PixelValue (pCurrentAb->AbIndent, pCurrentAb->AbIndentUnit, pCurrentAb);
					   if (pCurrentAb->AbIndent > 0)
					      val -= delta;
					   else if (pCurrentAb->AbIndent < 0)
					      val += delta;
					}

				      /* Convert the distance value */
				      if (pDimAb->DimUnit == UnPercent)
					 val = PixelValue (pDimAb->DimValue, UnPercent, (PtrAbstractBox) val);
				      else
					 val += PixelValue (pDimAb->DimValue, pDimAb->DimUnit, pAb);
				      ModHaut (pBo1, pBo1, NULL, val - pBo1->BxHeight, frame);
				      /* On teste si la relation est hors structure */
				      if (pDimAb->DimAbRef != pCurrentAb
					  && pDimAb->DimAbRef->AbEnclosing != pCurrentAb)
					 pBo1->BxHOutOfStruct = TRUE;

				      /* On note les dependances des hauteurs des boites */
				      if (pDimAb->DimAbRef == pAb && pDimAb->DimSameDimension)
					{
					   /* Il y a une erreur de dimension */
					   /* Erreur sur le schema de presentation */
					   if (horizRef)
					      TtaDisplayMessage (INFO, TtaGetMessage(LIB, BAD_HORIZ_SIZING_SEE_PRS_SCH), AbsBoxType (pAb));
					   else
					      TtaDisplayMessage (INFO, TtaGetMessage(LIB, BAD_VERT_SIZING_SEE_PRS_SCH), AbsBoxType (pAb));

					}
				      RelDim (pDimAb->DimAbRef->AbBox, pBo1, pDimAb->DimSameDimension, horizRef);
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
	     if (pPosAb->PosAbRef != pCurrentAb
		 && pPosAb->PosAbRef->AbEnclosing != pCurrentAb)
		pBo1->BxWOutOfStruct = TRUE;
	     else if (pPosAb->PosAbRef->AbBox != NULL)
		pBo1->BxWOutOfStruct = pPosAb->PosAbRef->AbBox->BxXOutOfStruct;

	     /* Des boites voisines heritent de la relation hors-structure ? */
	     if (pCurrentAb != NULL)
	       {
		  cepave = pCurrentAb->AbFirstEnclosed;
		  while (cepave != NULL)
		    {
		       if (cepave != pAb && cepave->AbBox != NULL)
			 {
			    /* Si c'est un heritage on note l'indication hors-structure */
			    if (cepave->AbHorizPos.PosAbRef == pAb
			      && cepave->AbHorizPos.PosRefEdge != Left)
			      {
				 if (!XEnAbsolu (cepave->AbBox))
				    /* la boite  est maintenant placee en absolu */
				    cepave->AbBox->BxXToCompute = TRUE;
				 cepave->AbBox->BxXOutOfStruct = TRUE;
				 if (cepave->AbEnclosing == pAb->AbEnclosing)
				    cepave->AbHorizEnclosing = pAb->AbHorizEnclosing;
				 PropagateXOutOfStruct (cepave, TRUE, cepave->AbHorizEnclosing);
			      }

			    if (cepave->AbVertPos.PosAbRef == pAb
				&& cepave->AbVertPos.PosRefEdge != Top
				&& pAb->AbLeafType == LtCompound
				&& pAb->AbInLine)
			      {
				 if (!YEnAbsolu (cepave->AbBox))
				    /* la boite  est maintenant placee en absolu */
				    cepave->AbBox->BxYToCompute = TRUE;
				 cepave->AbBox->BxYOutOfStruct = TRUE;
				 if (cepave->AbEnclosing == pAb->AbEnclosing)
				    cepave->AbVertEnclosing = pAb->AbVertEnclosing;
				 PropagateYOutOfStruct (cepave, TRUE, cepave->AbVertEnclosing);
			      }
			 }
		       cepave = cepave->AbNext;
		    }
	       }

	     /* Decalage par rapport a la boite distante */
	     pCurrentBox = pPosAb->PosAbRef->AbBox;
	     if (pCurrentBox == NULL)
	       {
		  /* On doit resoudre une reference en avant */
		  if (!pPosAb->PosAbRef->AbDead)
		     pCurrentBox = GetBox (pPosAb->PosAbRef);
		  if (pCurrentBox != NULL)
		     pPosAb->PosAbRef->AbBox = pCurrentBox;
	       }

	     if (pCurrentBox != NULL)
	       {
		  /* regarde si la position depend d'une boite invisible */
		  if (pPosAb->PosAbRef->AbVisibility < FntrTable[frame - 1].FrVisibility)
		     delta = 0;
		  else if (pPosAb->PosUnit == UnPercent)
		    {
		       /* Convert the distance value */
		       if (pAb->AbEnclosing == NULL)
			  delta = 0;
		       else
			  delta = PixelValue (pPosAb->PosDistance, pPosAb->PosUnit,
			  (PtrAbstractBox) pAb->AbEnclosing->AbBox->BxWidth);
		    }
		  else
		     /* Convert the distance value */
		     delta = PixelValue (pPosAb->PosDistance, pPosAb->PosUnit, pAb);

		  val = pCurrentBox->BxXOrg + delta;
		  switch (pPosAb->PosRefEdge)
			{
			   case Left:
			      break;
			   case Right:
			      val += pCurrentBox->BxWidth;
			      break;
			   case VertMiddle:
			      val += pCurrentBox->BxWidth / 2;
			      break;
			   case VertRef:
			      val += pCurrentBox->BxVertRef;
			      break;
			   default:
			      break;
			}

		  /* Calcule la largeur de la boite */
		  val = val - pBo1->BxXOrg - pBo1->BxWidth;
		  /* La boite n'a pas de point fixe */
		  pBo1->BxHorizEdge = NoEdge;
		  RelPos (pBo1, pCurrentBox, op, pPosAb->PosEdge, pPosAb->PosRefEdge);

		  if (!XEnAbsolu (pBo1))
		     /* la boite  devient maintenant placee en absolu */
		     pBo1->BxXToCompute = TRUE;
		  /* La boite est marquee elastique */
		  pBo1->BxHorizFlex = TRUE;
		  pCurrentBox->BxMoved = NULL;
		  ChngBElast (pBo1, pCurrentBox, op, val, frame, TRUE);
	       }
	  }
	else
	  {
	     pDimAb = &pAb->AbHeight;
	     /* Box elastique en Y */
	     pPosAb = &pDimAb->DimPosition;
	     op = OpHeight;

	     /* On teste si la relation est hors structure */
	     if (pPosAb->PosAbRef != pCurrentAb
		 && pPosAb->PosAbRef->AbEnclosing != pCurrentAb)
		pBo1->BxHOutOfStruct = TRUE;
	     else if (pPosAb->PosAbRef->AbBox != NULL)
		pBo1->BxHOutOfStruct = pPosAb->PosAbRef->AbBox->BxYOutOfStruct;

	     /* Des boites voisines heritent de la relation hors-structure ? */
	     if (pCurrentAb != NULL /* && pBo1->BxHOutOfStruct */ )
	       {
		  cepave = pCurrentAb->AbFirstEnclosed;
		  while (cepave != NULL)
		    {
		       if (cepave != pAb && cepave->AbBox != NULL)
			  /* Si c'est un heritage on note l'indication hors-structure */
			  if (cepave->AbVertPos.PosAbRef == pAb
			      && cepave->AbVertPos.PosRefEdge != Top)
			    {
			       if (!YEnAbsolu (cepave->AbBox))
				  /* la boite  est maintenant placee en absolu */
				  cepave->AbBox->BxYToCompute = TRUE;
			       cepave->AbBox->BxYOutOfStruct = TRUE;
			       if (cepave->AbEnclosing == pAb->AbEnclosing)
				  cepave->AbVertEnclosing = pAb->AbVertEnclosing;
			       PropagateYOutOfStruct (cepave, TRUE, cepave->AbVertEnclosing);
			    }
		       cepave = cepave->AbNext;
		    }
	       }

	     /* Decalage par rapport a la boite distante */
	     pCurrentBox = pPosAb->PosAbRef->AbBox;
	     if (pCurrentBox == NULL)
	       {
		  /* On doit resoudre une reference en avant */
		  pCurrentBox = GetBox (pPosAb->PosAbRef);
		  if (pCurrentBox != NULL)
		     pPosAb->PosAbRef->AbBox = pCurrentBox;
	       }

	     if (pCurrentBox != NULL)
	       {
		  /* regarde si la position depend d'une boite invisible */
		  if (pPosAb->PosAbRef->AbVisibility < FntrTable[frame - 1].FrVisibility)
		     delta = 0;
		  else if (pPosAb->PosUnit == UnPercent)
		    {
		       /* Convert the distance value */
		       if (pAb->AbEnclosing == NULL)
			  delta = 0;
		       else
			  delta = PixelValue (pPosAb->PosDistance, pPosAb->PosUnit,
			  (PtrAbstractBox) pAb->AbEnclosing->AbBox->BxHeight);
		    }
		  else
		     /* Convert the distance value */
		     delta = PixelValue (pPosAb->PosDistance, pPosAb->PosUnit, pAb);

		  val = pCurrentBox->BxYOrg + delta;
		  switch (pPosAb->PosRefEdge)
			{
			   case Bottom:
			      val += pCurrentBox->BxHeight;
			      break;
			   case HorizMiddle:
			      val += pCurrentBox->BxHeight / 2;
			      break;
			   case HorizRef:
			      val += pCurrentBox->BxHorizRef;
			      break;
			   default:
			      break;
			}

		  /* Calcule la hauteur de la boite */
		  val = val - pBo1->BxYOrg - pBo1->BxHeight;
		  /* La boite n'a pas de point fixe */
		  pBo1->BxVertEdge = NoEdge;
		  RelPos (pBo1, pCurrentBox, op, pPosAb->PosEdge, pPosAb->PosRefEdge);

		  if (!YEnAbsolu (pBo1))
		     /* la boite  devient maintenant placee en absolu */
		     pBo1->BxYToCompute = TRUE;
		  /* La boite est marquee elastique */
		  pBo1->BxVertFlex = TRUE;
		  pCurrentBox->BxMoved = NULL;
		  ChngBElast (pBo1, pCurrentBox, op, val, frame, FALSE);
	       }
	  }
     }

   /* La regle de dimension est interpretee */
   if (horizRef)
     {
	pAb->AbWidthChange = FALSE;
	/* Marque dans la boite si la dimension depend du contenu ou non */
	pBo1->BxRuleWidth = 0;
	if (setdim)
	   pBo1->BxContentWidth = TRUE;
	else
	   pBo1->BxContentWidth = FALSE;
     }
   else
     {
	pAb->AbHeightChange = FALSE;
	/* Marque dans la boite si la dimension depend du contenu ou non */
	pBo1->BxRuleHeigth = 0;
	if (setdim)
	   pBo1->BxContentHeight = TRUE;
	else
	   pBo1->BxContentHeight = FALSE;
     }

   return setdim;
}


/* ---------------------------------------------------------------------- */
/* |    PlacerAxe applique la regle de positionnement donnee en         | */
/* |            parametre a` la boite d'indice pBox.                    | */
/* |            L'axe horizontal ou vertical de la boite, selon que horizRef | */
/* |            est VRAI ou FAUX est mis a` jour.                       | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void              PlacerAxe (AbPosition rule, PtrBox pBox, int frame, boolean horizRef)
#else  /* __STDC__ */
void              PlacerAxe (rule, pBox, frame, horizRef)
AbPosition        rule;
PtrBox            pBox;
int               frame;
boolean           horizRef;

#endif /* __STDC__ */
{
   int            x, y, dist;
   PtrBox         pCurrentBox;
   PtrAbstractBox pAb;
   PtrAbstractBox pCurrentAb;
   BoxEdge        refEdge, localEdge;

   /* Calcule la position de reference */
   pAb = rule.PosAbRef;
   pCurrentAb = pBox->BxAbstractBox;
   /* Verifie que la position ne depend pas d'un pave mort */
   if (pAb != NULL && pAb->AbDead)
     {
	printf ("Axe relative to a dead box");
	pAb = NULL;
     }

   /* SRule par defaut */
   if (pAb == NULL)
     {
	pCurrentBox = pBox;
	if (horizRef)
	  {
	     refEdge = Left;
	     localEdge = VertRef;
	     if (rule.PosUnit == UnPercent)
		dist = PixelValue (rule.PosDistance, UnPercent, (PtrAbstractBox) pBox->BxWidth);
	     else
		dist = 0;
	  }
	else
	  {
	     refEdge = Bottom;
	     localEdge = HorizRef;
	     if (rule.PosUnit == UnPercent)
		dist = PixelValue (rule.PosDistance, UnPercent, (PtrAbstractBox) pBox->BxHeight);
	     if (pCurrentAb->AbLeafType == LtText)
		dist = FontBase (pBox->BxFont) - pBox->BxHeight;
	     else
		dist = 0;
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
		dist = PixelValue (rule.PosDistance, UnPercent, (PtrAbstractBox) pCurrentAb->AbEnclosing->AbBox->BxWidth);
	     else
		dist = PixelValue (rule.PosDistance, UnPercent, (PtrAbstractBox) pCurrentAb->AbEnclosing->AbBox->BxHeight);
	  }
	else
	   dist = PixelValue (rule.PosDistance, rule.PosUnit, pCurrentAb);

	pCurrentBox = pAb->AbBox;
	if (pCurrentBox == NULL)
	  {
	     /* On doit resoudre une reference en avant */
	     pCurrentBox = GetBox (pAb);
	     if (pCurrentBox != NULL)
		pAb->AbBox = pCurrentBox;
	     else
	       {
		  if (horizRef)
		     pCurrentAb->AbVertRefChange = FALSE;
		  else
		     pCurrentAb->AbHorizRefChange = FALSE;
		  return;	/* plus de boite libre */
	       }
	  }
     }

   /* Deplacement par rapport a la boite distante */
   /* L'axe est place par rapport a la boite elle-meme */
   if (pCurrentBox == pBox
       || pBox->BxType == BoGhost
       || (pCurrentAb->AbInLine && pCurrentAb->AbLeafType == LtCompound))
     {
	x = 0;
	y = 0;
     }
   /* L'axe est place par rapport a une incluse */
   else if (Propage != ToSiblings)
     {
	/* Il faut peut-etre envisager que pCurrentBox soit une boite coupee */
	x = pCurrentBox->BxXOrg - pBox->BxXOrg;
	y = pCurrentBox->BxYOrg - pBox->BxYOrg;
     }
   else
     {
	x = pCurrentBox->BxXOrg;
	y = pCurrentBox->BxYOrg;
     }

   switch (refEdge)
	 {
	    case Left:
	    case Top:
	       break;
	    case Bottom:
	       y += pCurrentBox->BxHeight;
	       break;
	    case Right:
	       x += pCurrentBox->BxWidth;
	       break;
	    case HorizRef:
	       y += pCurrentBox->BxHorizRef;
	       break;
	    case VertRef:
	       x += pCurrentBox->BxVertRef;
	       break;
	    case HorizMiddle:
	       y += pCurrentBox->BxHeight / 2;
	       break;
	    case VertMiddle:
	       x += pCurrentBox->BxWidth / 2;
	       break;
	    case NoEdge:
	       break;
	 }

   /* Met a jour l'axe de la boite */
   if (horizRef)
     {
	x = x + dist - pBox->BxVertRef;
	DepAxe (pBox, NULL, x, frame);
	/* la regle axe de reference est interpretee */
	pCurrentAb->AbVertRefChange = FALSE;
	if (pCurrentBox != NULL)
	   RelPos (pBox, pCurrentBox, OpHorizRef, localEdge, refEdge);
     }
   else
     {
	y = y + dist - pBox->BxHorizRef;
	DepBase (pBox, NULL, y, frame);
	/* la regle axe de reference est interpretee */
	pCurrentAb->AbHorizRefChange = FALSE;
	if (pCurrentBox != NULL)
	   RelPos (pBox, pCurrentBox, OpVertRef, localEdge, refEdge);
     }
}


/* ---------------------------------------------------------------------- */
/* |    BoiteHInclus recherche la boite qui relie horizontalement pBox  | */
/* |            a` son englobante :                                     | */
/* |            - Si on n'a pas modifie les relations, cette boite est  | */
/* |            memorisee (BxHorizInc).                                 | */
/* |            - Si la boite possede la relation OpHorizInc, c'est     | */
/* |            elle-meme.                                              | */
/* |            - Sinon on regarde la boite soeur dont elle depend.     | */
/* |            Retourne son adresse sinon NULL.                        | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
PtrBox            BoiteHInclus (PtrBox pBox, PtrBox prec)
#else  /* __STDC__ */
PtrBox            BoiteHInclus (pBox, prec)
PtrBox            pBox;
PtrBox            prec;
#endif /* __STDC__ */
{
   PtrBox            box1;
   int               i;
   PtrPosRelations   pPosRel;
   boolean           nonempty;
   BoxRelation      *pRelation;
   PtrBox            Result;

   /* On verifie que la boite n'a pas deja ete examinee */
   if (prec != NULL)
     {
	box1 = prec->BxMoved;
	while (box1 != NULL && box1 != pBox)
	   box1 = box1->BxMoved;
     }
   else
      box1 = NULL;

   if (box1 == NULL)
     {

	/* On met a jour la pile des boites traitees */
	pBox->BxMoved = prec;
	/* regarde si on connait deja la boite qui la relie a l'englobante */
	if (pBox->BxHorizInc != NULL)
	   box1 = pBox->BxHorizInc;
	else
	   box1 = NULL;

	/* Si la position de la boite depend d'une boite externe on prend la */
	/* boite elle meme comme reference.                                  */
	if (pBox->BxXOutOfStruct)
	   box1 = pBox;

	/* regarde si la boite est reliee a son englobante */
	pPosRel = pBox->BxPosRelations;
	while (box1 == NULL && pPosRel != NULL)
	  {
	     i = 1;
	     nonempty = pPosRel->PosRTable[i - 1].ReBox != NULL;
	     while (i <= MAX_RELAT_POS && nonempty)
		if (pPosRel->PosRTable[i - 1].ReOp == OpHorizInc)
		  {
		     box1 = pBox;
		     i = MAX_RELAT_POS + 1;
		     /* On a trouve */
		  }
		else
		  {
		     i++;
		     if (i <= MAX_RELAT_POS)
			nonempty = pPosRel->PosRTable[i - 1].ReBox != NULL;
		  }

	     pPosRel = pPosRel->PosRNext;
	     /* Bloc suivant */
	  }

	/* Sinon on recherche la boite soeur qui l'est */
	pPosRel = pBox->BxPosRelations;
	while (box1 == NULL && pPosRel != NULL)
	  {
	     i = 1;
	     nonempty = pPosRel->PosRTable[i - 1].ReBox != NULL;
	     while (i <= MAX_RELAT_POS && nonempty)
	       {
		  pRelation = &pPosRel->PosRTable[i - 1];
		  if (pRelation->ReBox->BxAbstractBox != NULL)
		     if (pRelation->ReOp == OpHorizDep
		     /* Si c'est la bonne relation de dependance */
			 && pRelation->ReBox->BxAbstractBox->AbHorizPos.PosAbRef != pBox->BxAbstractBox)
		       {
			  /* Si la position de la boite depend d'une boite elastique */
			  /* on prend la boite elastique comme reference             */
			  if (pRelation->ReBox->BxHorizFlex)
			     box1 = pRelation->ReBox;
			  else
			     box1 = BoiteHInclus (pRelation->ReBox, pBox);


			  /* Est-ce que l'on a trouve la boite qui donne la position ? */
			  if (box1 != NULL)
			    {
			       i = MAX_RELAT_POS + 1;
			       /* La position depend d'une relation hors-structure ? */
			       if (box1->BxXOutOfStruct)
				  pBox->BxXOutOfStruct = TRUE;
			    }
			  else
			    {
			      i++;
			      if (i <= MAX_RELAT_POS)
				nonempty = pPosRel->PosRTable[i - 1].ReBox != NULL;
			    }
		       }
		     else
		       {
			 i++;
			 if (i <= MAX_RELAT_POS)
			   nonempty = pPosRel->PosRTable[i - 1].ReBox != NULL;
		       }
	       }
	     /* Bloc suivant */
	     pPosRel = pPosRel->PosRNext;
	  }

	pBox->BxHorizInc = box1;
	Result = box1;
     }
   else
      Result = NULL;
   return Result;
}


/* ---------------------------------------------------------------------- */
/* |    BoiteVInclus recherche la boite qui relie verticalement pBox    | */
/* |            a` son englobante :                                     | */
/* |            - Si on n'a pas modifie les relations, cette boite est  | */
/* |            memorisee (BxVertInc).                                  | */
/* |            - Si la boite possede la relation OpVertInc, c'est      | */
/* |            elle-meme.                                              | */
/* |            - Sinon on regarde la boite soeur dont elle depend.     | */
/* |            Retourne son adresse sinon NULL.                        | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
PtrBox            BoiteVInclus (PtrBox pBox, PtrBox prec)
#else  /* __STDC__ */
PtrBox            BoiteVInclus (pBox, prec)
PtrBox            pBox;
PtrBox            prec;

#endif /* __STDC__ */
{
   PtrBox            box1;
   int               i;
   PtrPosRelations   pPosRel;
   boolean           nonempty;
   BoxRelation      *pRelation;
   PtrBox            result;

   /* On verifie que la boite n'a pas deja ete examinee */
   if (prec != NULL)
     {
	box1 = prec->BxMoved;
	while (box1 != NULL && box1 != pBox)
	   box1 = box1->BxMoved;
     }
   else
      box1 = NULL;

   if (box1 == NULL)
     {

	/* On met a jour la pile des boites traitees */
	pBox->BxMoved = prec;

	/* regarde si on connait deja la boite qui la relie a l'englobante */
	if (pBox->BxVertInc != NULL)
	   box1 = pBox->BxVertInc;
	else
	   box1 = NULL;

	/* Si la position de la boite depend d'une boite externe on prend la */
	/* boite elle meme comme reference.                                  */
	if (pBox->BxYOutOfStruct)
	   box1 = pBox;

	/* regarde si la boite est reliee a son englobante */
	pPosRel = pBox->BxPosRelations;
	while (box1 == NULL && pPosRel != NULL)
	  {
	     i = 1;
	     nonempty = pPosRel->PosRTable[i - 1].ReBox != NULL;
	     while (i <= MAX_RELAT_POS && nonempty)
		if (pPosRel->PosRTable[i - 1].ReOp == OpVertInc)
		  {
		     box1 = pBox;
		     i = MAX_RELAT_POS + 1;	/* On a trouve */
		  }
		else
		  {
		     i++;
		     if (i <= MAX_RELAT_POS)
			nonempty = pPosRel->PosRTable[i - 1].ReBox != NULL;
		  }
	     pPosRel = pPosRel->PosRNext;	/* Bloc suivant */

	  }

	/* Sinon on recherche la boite soeur qui l'est */
	pPosRel = pBox->BxPosRelations;
	while (box1 == NULL && pPosRel != NULL)
	  {
	     i = 1;
	     nonempty = pPosRel->PosRTable[i - 1].ReBox != NULL;
	     while (i <= MAX_RELAT_POS && nonempty)
	       {
		  pRelation = &pPosRel->PosRTable[i - 1];
		  if (pRelation->ReBox->BxAbstractBox != NULL)
		     if (pRelation->ReOp == OpVertDep
		     /* Si c'est la bonne relation de dependance */
			 && pRelation->ReBox->BxAbstractBox->AbVertPos.PosAbRef != pBox->BxAbstractBox)
		       {

			  /* Si la position de la boite depend d'une boite elastique */
			  /* on prend la boite elastique comme reference             */
			  if (pRelation->ReBox->BxVertFlex)
			     box1 = pRelation->ReBox;
			  else
			     box1 = BoiteVInclus (pRelation->ReBox, pBox);
			  /* Est-ce que l'on a trouve la boite qui donne la position ? */
			  if (box1 != NULL)
			    {
			       i = MAX_RELAT_POS + 1;
			       /* La position depend d'une relation hors-structure ? */
			       if (box1->BxYOutOfStruct)
				  pBox->BxYOutOfStruct = TRUE;
			    }
			  else
			    {
			       i++;
			       if (i <= MAX_RELAT_POS)
				  nonempty = pPosRel->PosRTable[i - 1].ReBox != NULL;
			    }
		       }
		     else
		       {
			  i++;
			  if (i <= MAX_RELAT_POS)
			     nonempty = pPosRel->PosRTable[i - 1].ReBox != NULL;
		       }
	       }
	     /* Bloc suivant */
	     pPosRel = pPosRel->PosRNext;
	  }

	pBox->BxVertInc = box1;
	result = box1;
     }
   else
      result = NULL;
   return result;
}


/* ---------------------------------------------------------------------- */
/* |    RemovePosRelation defait, s'il existe, le lien de dependance de position ou| */
/* |            d'axe horizontal ou vertical de la boite pOrginBox et   | */
/* |            retasse la liste des liens.                             | */
/* |            Le parametre pCurrentAb, quand il est non nul, indique  | */
/* |            le pave dont on annule la regle de position et sert a`  | */
/* |            retirer l'ambiguite des relations doubles entre boites  | */
/* |            soeurs : si on trouve une relation associee a` une      | */
/* |            regle de position qui reference pCurrentAb, elle        | */
/* |            ne doit pas etre detruite. Quand pCurrentAb est nul, on | */
/* |            connait la boite pTargetBox referencee dans le lien que | */
/* |            l'on veut detruire (c'est alors une destruction de lien | */
/* |            inverse).                                               | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static boolean      RemovePosRelation (PtrBox pOrginBox, PtrBox pTargetBox, PtrAbstractBox pCurrentAb, boolean Pos, boolean Axe, boolean horizRef)
#else  /* __STDC__ */
static boolean      RemovePosRelation (pOrginBox, pTargetBox, pCurrentAb, Pos, Axe, horizRef)
PtrBox              pOrginBox;
PtrBox              pTargetBox;
PtrAbstractBox      pCurrentAb;
boolean             Pos;
boolean             Axe;
boolean             horizRef;
#endif /* __STDC__ */
{
   int                 i, trouve;
   int                 j, k;
   boolean             loop;
   boolean             nonempty;
   PtrPosRelations     pPreviousPosRel;
   PtrPosRelations     precpos;
   PtrPosRelations     pPosRel;
   PtrAbstractBox      pAb;
   BoxRelation        *pRelation;
   boolean             result;

   /* On recherche l'entree a detruire et la derniere entree occupee */
   trouve = 0;
   i = 0;
   pPreviousPosRel = NULL;
   pPosRel = pOrginBox->BxPosRelations;
   precpos = NULL;
   loop = TRUE;
   if (pPosRel != NULL)
      while (loop)
	{
	   i = 1;
	   nonempty = pPosRel->PosRTable[i - 1].ReBox != NULL;
	   while (i <= MAX_RELAT_POS && nonempty)
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
			  trouve = i;
			  pPreviousPosRel = pPosRel;
		       }

		     /* Est-ce la relation de position du pave pCurrentAb ? */
		     else if (trouve == 0 && Pos && pRelation->ReOp == OpHorizDep
			      && pCurrentAb != NULL && pRelation->ReBox == pTargetBox)
		       {
			  pAb = pRelation->ReBox->BxAbstractBox;
			  pAb = pAb->AbHorizPos.PosAbRef;

			  /* Si la position du pave distant est donnee par une    */
			  /* regle NULL, il faut rechercher le pave dont il depend */
			  if (pAb == NULL)
			     pAb = GetPosRelativeAb (pRelation->ReBox->BxAbstractBox, horizRef);

			  /* On a bien trouve la relation de positionnement       */
			  /* du pave pCurrentAb et non une relation de positionnement */
			  /* du pave distant par rapport au pave pCurrentAb ?         */
			  if (pAb != pCurrentAb)
			    {
			       trouve = i;
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
		     trouve = i;
		     pPreviousPosRel = pPosRel;
		  }

		/* Est-ce la relation de position du pave pCurrentAb ? */
		else if (trouve == 0 && Pos && pRelation->ReOp == OpVertDep
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
			  trouve = i;
			  pPreviousPosRel = pPosRel;
		       }
		  }
		i++;
		if (i <= MAX_RELAT_POS)
		   nonempty = pPosRel->PosRTable[i - 1].ReBox != NULL;
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
   if (trouve > 0)
     {
	pRelation = &pPreviousPosRel->PosRTable[trouve - 1];

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
	     FreeBPos (&pPosRel);
	  }
	else
	   pPosRel->PosRTable[i - 1].ReBox = NULL;
	result = TRUE;
     }
   else
      result = FALSE;
   return result;
}


/* ---------------------------------------------------------------------- */
/* |    DelDim defait, s'il existe, le lien de de'pendance de dimension | */
/* |            horizontale ou verticale de la boite pOrginBox vers la boite| */
/* |            pTargetBox et retasse la liste des liens.                     | */
/* |            Rend la valeur Vrai si l'operation a ete executee.      | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static boolean      DelDim (PtrBox pOrginBox, PtrBox pTargetBox, boolean horizRef)
#else  /* __STDC__ */
static boolean      DelDim (pOrginBox, pTargetBox, horizRef)
PtrBox            pOrginBox;
PtrBox            pTargetBox;
boolean             horizRef;

#endif /* __STDC__ */
{
   int                 i, trouve;
   boolean             loop;
   boolean             nonempty;
   PtrDimRelations      pPreviousDimRel;
   PtrDimRelations      precdim;
   PtrDimRelations      pDimRel;
   boolean             result;


   i = 0;
   /* Cela peut etre une dimension elastique */
   if ((horizRef && pTargetBox->BxHorizFlex) || (!horizRef && pTargetBox->BxVertFlex))
      result = RemovePosRelation (pOrginBox, pTargetBox, NULL, FALSE, FALSE, horizRef);
   else
     {
	/* On recherche l'entree a detruire et la derniere entree occupee */
	trouve = 0;
	pPreviousDimRel = NULL;
	if (horizRef)
	   pDimRel = pOrginBox->BxWidthRelations;
	else
	   pDimRel = pOrginBox->BxHeightRelations;
	precdim = NULL;
	loop = TRUE;
	if (pDimRel != NULL)

	   while (loop)
	     {
		i = 1;
		nonempty = pDimRel->DimRTable[i - 1] != NULL;
		while (i <= MAX_RELAT_DIM && nonempty)
		  {
		     /* Est-ce l'entree a detruire ? */
		     if (pDimRel->DimRTable[i - 1] == pTargetBox)
		       {
			  trouve = i;
			  pPreviousDimRel = pDimRel;
		       }
		     i++;
		     if (i <= MAX_RELAT_DIM)
			nonempty = pDimRel->DimRTable[i - 1] != NULL;
		  }

		if (pDimRel->DimRNext == NULL)
		   loop = FALSE;
		else
		  {
		     precdim = pDimRel;
		     pDimRel = pDimRel->DimRNext;
		     /* Bloc suivant */
		  }
	     }

	/* On a trouve -> on retasse la liste */
	if (trouve > 0)
	  {
	     i--;
	     pPreviousDimRel->DimRTable[trouve - 1] = pDimRel->DimRTable[i - 1];
	     pPreviousDimRel->DimRSame[trouve - 1] = pDimRel->DimRSame[i - 1];

	     /* Faut-il liberer le dernier bloc de relations ? */
	     if (i == 1)
	       {
		  if (precdim == NULL)
		     if (horizRef)
			pOrginBox->BxWidthRelations = NULL;
		     else
			pOrginBox->BxHeightRelations = NULL;
		  else
		     precdim->DimRNext = NULL;
		  FreeBDim (&pDimRel);
	       }
	     else
		pDimRel->DimRTable[i - 1] = NULL;
	     result = TRUE;
	  }
	/* On n'a pas trouve */
	else
	   result = FALSE;
     }
   return result;
}


/* ---------------------------------------------------------------------- */
/* |    RazHorsEnglobe detruit les relations hors hierarchie de la boite| */
/* |            pTargetBox.                                             | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                RazHorsEnglobe (PtrBox pTargetBox)
#else  /* __STDC__ */
void                RazHorsEnglobe (pTargetBox)
PtrBox            pTargetBox;

#endif /* __STDC__ */
{
   PtrBox            pOrginBox;
   PtrAbstractBox             pAbbox1;

   pAbbox1 = pTargetBox->BxAbstractBox;

   /* On detruit la relation de position horizontale hors-structure */
   if (pTargetBox->BxXOutOfStruct)
     {
	if (pAbbox1->AbHorizPos.PosAbRef == NULL)
	   pOrginBox = NULL;
	else
	   pOrginBox = pAbbox1->AbHorizPos.PosAbRef->AbBox;
	if (pOrginBox != NULL)
	   RemovePosRelation (pOrginBox, pTargetBox, NULL, TRUE, FALSE, TRUE);

	/* Annule les relations hors-structure */
	PropagateXOutOfStruct (pAbbox1, FALSE, pAbbox1->AbHorizEnclosing);
     }

   /* On detruit la relation de position verticale hors-structure */
   if (pTargetBox->BxYOutOfStruct)
     {
	if (pAbbox1->AbVertPos.PosAbRef == NULL)
	   pOrginBox = NULL;
	else
	   pOrginBox = pAbbox1->AbVertPos.PosAbRef->AbBox;
	if (pOrginBox != NULL)
	   RemovePosRelation (pOrginBox, pTargetBox, NULL, TRUE, FALSE, FALSE);

	/* Annule les relations hors-structure */
	PropagateYOutOfStruct (pAbbox1, FALSE, pAbbox1->AbVertEnclosing);
     }

   /* On detruit la relation de largeur hors-structure */
   if (pTargetBox->BxWOutOfStruct)
      /* Est-ce une dimension elastique ? */
      if (pTargetBox->BxHorizFlex)
	{
	   pOrginBox = pAbbox1->AbWidth.DimPosition.PosAbRef->AbBox;
	   if (pOrginBox != NULL)
	      RemovePosRelation (pOrginBox, pTargetBox, NULL, FALSE, FALSE, TRUE);
	}
      else
	{
	   pOrginBox = pAbbox1->AbWidth.DimAbRef->AbBox;
	   if (pOrginBox != NULL)
	      DelDim (pOrginBox, pTargetBox, TRUE);
	}

   /* On detruit la relation de hauteur hors-structure */
   pOrginBox = NULL;
   if (pTargetBox->BxHOutOfStruct)
      /* Est-ce une dimension elastique ? */
      if (pTargetBox->BxVertFlex)
	{
	   if (pAbbox1->AbHeight.DimPosition.PosAbRef != NULL)
	      pOrginBox = pAbbox1->AbHeight.DimPosition.PosAbRef->AbBox;
	   if (pOrginBox != NULL)
	      RemovePosRelation (pOrginBox, pTargetBox, NULL, FALSE, FALSE, FALSE);
	}
      else
	{
	   if (pAbbox1->AbHeight.DimAbRef != NULL)
	      pOrginBox = pAbbox1->AbHeight.DimAbRef->AbBox;
	   if (pOrginBox != NULL)
	      DelDim (pOrginBox, pTargetBox, FALSE);
	}
}				/* RazHorsEnglobe */

/* ---------------------------------------------------------------------- */
/* |    ProchainPave recherche le prochain pave du pave dupave en       | */
/* |            sautant le pave de reference paveref. Le parcours de    | */
/* |            l'arborescence se fait de bas en haut et de gauche a`   | */
/* |            droite.                                                 | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static PtrAbstractBox      ProchainPave (PtrAbstractBox dupave, PtrAbstractBox paveref)
#else  /* __STDC__ */
static PtrAbstractBox      ProchainPave (dupave, paveref)
PtrAbstractBox             dupave;
PtrAbstractBox             paveref;

#endif /* __STDC__ */
{

   PtrAbstractBox             nextpave;

   /* Il y a un premier fils different du pave reference ? */
   if (dupave->AbFirstEnclosed != NULL && dupave->AbFirstEnclosed != paveref)
      nextpave = dupave->AbFirstEnclosed;
   /* Il y a un frere different du pave reference ? */
   else if (dupave->AbNext != NULL && dupave->AbNext != paveref)
      nextpave = dupave->AbNext;
   /* Sinon on remonte dans la hierarchie */
   else
     {
	nextpave = NULL;
	while (dupave != NULL)
	  {
	     /* On saute le pave reference */
	     if (dupave->AbFirstEnclosed == paveref && paveref->AbNext != NULL)
		dupave = paveref;
	     else if (dupave->AbNext == paveref)
		dupave = paveref;

	     /* On recherche un prochain frere d'un pere */
	     if (dupave->AbNext != NULL)
	       {
		  nextpave = dupave->AbNext;
		  dupave = NULL;
	       }
	     else
	       {
		  /* On passe au pere tant que ce n'est pas la racine */
		  dupave = dupave->AbEnclosing;
		  if (dupave->AbFirstEnclosed == paveref)
		     dupave = dupave->AbEnclosing;
		  if (dupave != NULL)
		     if (dupave->AbEnclosing == NULL)
			dupave = NULL;
	       }
	  }
     }
   return nextpave;
}				/* ProchainPave */

/* ---------------------------------------------------------------------- */
/* |    RazPosition recherche la boite dont depend la position          | */
/* |            horizontale (si horizRef est Vrai) sinon verticale de   | */
/* |            pOrginBox :						| */
/* |            - Elle peut dependre de son englobante                  | */
/* |            (relation OpInclus chez elle).                          | */
/* |            - Elle peut dependre d'une voisine (deux                | */
/* |            relations OpLie).                                       | */
/* |            - Elle peut avoir une relation hors-structure           | */
/* |            (deux relations OpLie).                                 | */
/* |            Si cette dependance existe encore, on detruit les       | */
/* |            relations entre les deux boites. L'indicateur           | */
/* |            BtX(Y)HorsStruct indique que la relation est            | */
/* |            hors-structure.                                         | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                RazPosition (PtrBox pOrginBox, boolean horizRef)
#else  /* __STDC__ */
void                RazPosition (pOrginBox, horizRef)
PtrBox              pOrginBox;
boolean             horizRef;

#endif /* __STDC__ */
{
   PtrAbstractBox      pAb;
   PtrAbstractBox      pCurrentAb;
   boolean             loop;

   pAb = pOrginBox->BxAbstractBox;
   /* Est-ce une relation hors-structure ? */
   if ((horizRef && pOrginBox->BxXOutOfStruct) || (!horizRef && pOrginBox->BxYOutOfStruct))
     {
	/* Si la boite est detruite la procedure RazHorsEnglobe detruit */
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
		  pCurrentAb = ProchainPave (pCurrentAb, pAb);
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
	  }			/*while */
     }
}				/* RazPosition */

/* ---------------------------------------------------------------------- */
/* |    RazAxe recherche la boite dont depend l'axe de reference        | */
/* |            horizontal (si horizRef est Vrai) sinon vertical de     | */
/* |            pOrginBox :  						| */
/* |            - Il peut dependre d'elle meme (une relation chez elle).| */
/* |            - Il peut dependre d'une englobee (une relation chez    | */
/* |            l'englobee).                                            | */
/* |            - Il peut dependre d'une voisine (une relation chez la  | */
/* |            voisine).                                               | */
/* |            Si cette dependance existe encore, on detruit la        | */
/* |            relation.                                               | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void              RazAxe (PtrBox pOrginBox, boolean horizRef)
#else  /* __STDC__ */
void              RazAxe (pOrginBox, horizRef)
PtrBox            pOrginBox;
boolean           horizRef;
#endif /* __STDC__ */
{
   boolean             loop;
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


/* ---------------------------------------------------------------------- */
/* |    RazLiens detruit toutes les relations avec la boite pTargetBox  | */
/* |            chez ses voisines, son englobante et les relations hors | */
/* |            hierarchie :                                            | */
/* |            -> la relation OpVertRef verticale.                     | */
/* |            -> la relation OpHorizRef horizontale.                  | */
/* |            -> les relations de Position.                           | */
/* |            -> les relations de positions et de dimensions hors     | */
/* |            hierarchie.                                             | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                RazLiens (PtrBox pTargetBox)
#else  /* __STDC__ */
void                RazLiens (pTargetBox)
PtrBox            pTargetBox;
#endif /* __STDC__ */
{
   PtrAbstractBox             pAb;
   PtrAbstractBox             pCurrentAb;

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
	RazHorsEnglobe (pTargetBox);
     }
}


/* ---------------------------------------------------------------------- */
/* |    RazDim recherche la boite dont depend la dimension horizontale  | */
/* |            ou verticale de pOrginBox :                             | */
/* |            - Elle peut dependre de son englobante (une relation    | */
/* |            chez l'englobante).                                     | */
/* |            - Elle peut dependre d'une voisine (une relation chez   | */
/* |            la voisine).                                            | */
/* |            - Elle peut avoir une relation hors-structure           | */
/* |            (relation chez l'autre).                                | */
/* |            Si cette dependance existe encore, on detruit la        | */
/* |            relation. L'indicateur BtLg(Ht)HorsStruct indique que   | */
/* |            la relation est hors-structure.                         | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                RazDim (PtrBox pOrginBox, boolean horizRef, int frame)
#else  /* __STDC__ */
void                RazDim (pOrginBox, horizRef, frame)
PtrBox            pOrginBox;
boolean             horizRef;
int                 frame;

#endif /* __STDC__ */
{
   boolean             loop;
   PtrAbstractBox             pAb;
   PtrAbstractBox             pCurrentAb;


   pAb = pOrginBox->BxAbstractBox;
   pCurrentAb = pAb->AbEnclosing;
   loop = FALSE;

   /* Est-ce une relation hors-structure en X ? */
   if ((horizRef && pOrginBox->BxWOutOfStruct) || (!horizRef && pOrginBox->BxHOutOfStruct))
     {
	/* Si la boite est detruite la procedure RazHorsEnglobe detruit */
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
			       pCurrentAb = ProchainPave (pCurrentAb, pAb);
			 }

		       /* Il faut retablir le point fixe */
		       pOrginBox->BxHorizEdge = pOrginBox->BxAbstractBox->AbHorizPos.PosEdge;

		       /* La boite n'est pas inversee */
		       pOrginBox->BxHorizInverted = FALSE;

		       /* La dimension n'est plus elastique */
		       pOrginBox->BxHorizFlex = FALSE;

		       /* Annule la largeur de la boite */
		       ModLarg (pOrginBox, NULL, NULL, -pOrginBox->BxWidth, 0, frame);

		    }
		  /* La dimension n'est pas elastique en X */
		  else
		     while (loop && pCurrentAb != NULL)
		       {
			  if (pCurrentAb->AbBox != NULL)
			     loop = !DelDim (pCurrentAb->AbBox, pOrginBox, horizRef);
			  if (pCurrentAb != NULL)
			     pCurrentAb = ProchainPave (pCurrentAb, pAb);
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
			       pCurrentAb = ProchainPave (pCurrentAb, pAb);
			 }

		       /* Il faut retablir le point fixe */
		       pOrginBox->BxVertEdge = pOrginBox->BxAbstractBox->AbVertPos.PosEdge;

		       /* La boite n'est pas inversee */
		       pOrginBox->BxVertInverted = FALSE;

		       /* La dimension n'est plus elastique */
		       pOrginBox->BxVertFlex = FALSE;

		       /* Annule la hauteur de la boite */
		       ModHaut (pOrginBox, NULL, NULL, -pOrginBox->BxHeight, frame);
		    }
		  /* La dimension n'est pas elastique en Y */
		  else
		     while (loop && pCurrentAb != NULL)
		       {
			  if (pCurrentAb->AbBox != NULL)
			     loop = !DelDim (pCurrentAb->AbBox, pOrginBox, horizRef);
			  if (pCurrentAb != NULL)
			     pCurrentAb = ProchainPave (pCurrentAb, pAb);
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
	     loop = !DelDim (pCurrentAb->AbBox, pOrginBox, horizRef);
	     if (loop)
		pCurrentAb = pCurrentAb->AbFirstEnclosed;
	  }

	/* Est-ce que la dimension de la boite depend d'une voisine ? */
	while (loop && pCurrentAb != NULL)
	  {
	     if (pCurrentAb->AbBox != NULL && pCurrentAb != pAb)
	       {
		  loop = !DelDim (pCurrentAb->AbBox, pOrginBox, horizRef);
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
	     ModLarg (pOrginBox, NULL, NULL, -pOrginBox->BxWidth, 0, frame);
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
	     ModHaut (pOrginBox, NULL, NULL, -pOrginBox->BxHeight, frame);
	  }
     }

}

