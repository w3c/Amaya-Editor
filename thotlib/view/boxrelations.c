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
static void       RelPos (PtrBox pOrginBox, PtrBox pTargetBox, OpRelation op, BoxEdge rep1, BoxEdge rep2)
#else  /* __STDC__ */
static void       RelPos (pOrginBox, pTargetBox, op, rep1, rep2)
PtrBox            pOrginBox;
PtrBox            pTargetBox;
OpRelation        op;
BoxEdge           rep1;
BoxEdge           rep2;
#endif /* __STDC__ */
{
   int                 i;
   boolean             boucle;
   boolean             nul;
   PtrPosRelations     cepos;
   PtrPosRelations     adpos;
   BoxRelation        *pRe1;

   i = 0;
   /* PcFirst sens de la dependance dans les positionnements entre soeurs */
   if (op != OpHorizRef && op != OpVertRef && op != OpWidth && op != OpHeight)
     {
	/* On recherche une entree libre */
	adpos = pOrginBox->BxPosRelations;
	cepos = NULL;
	boucle = TRUE;
	while (boucle && adpos != NULL)
	  {
	     i = 0;
	     cepos = adpos;
	     do
	       {
		  i++;
		  nul = adpos->PosRTable[i - 1].ReBox == NULL;
	       }
	     while (!(i == MAX_RELAT_POS || nul));

	     if (nul)
		boucle = FALSE;
	     else
		adpos = adpos->PosRNext;
	     /* Bloc suivant */
	  }

	/* Faut-il creer un nouveau bloc de relations ? */
	if (boucle)
	  {
	     GetBPos (&adpos);
	     if (cepos == NULL)
		pOrginBox->BxPosRelations = adpos;
	     else
		cepos->PosRNext = adpos;
	     i = 1;
	  }
	pRe1 = &adpos->PosRTable[i - 1];
	pRe1->ReRefEdge = rep1;
	pRe1->ReBox = pTargetBox;
	pRe1->ReOp = op;
     }

   /* Deuxieme sens de la dependance */
   if (op != OpHorizInc && op != OpVertInc)
     {
	/* On recherche une entree libre */
	adpos = pTargetBox->BxPosRelations;
	cepos = NULL;
	boucle = TRUE;
	while (boucle && adpos != NULL)
	  {
	     i = 0;
	     cepos = adpos;
	     do
	       {
		  i++;
		  nul = adpos->PosRTable[i - 1].ReBox == NULL;
	       }
	     while (!(i == MAX_RELAT_POS || nul));

	     if (nul)
		boucle = FALSE;
	     else
		adpos = adpos->PosRNext;	/* Bloc suivant */
	  }

	/* Faut-il creer un nouveau bloc de relations ? */
	if (boucle)
	  {
	     GetBPos (&adpos);
	     if (cepos == NULL)
		pTargetBox->BxPosRelations = adpos;
	     else
		cepos->PosRNext = adpos;
	     i = 1;
	  }
	pRe1 = &adpos->PosRTable[i - 1];
	pRe1->ReRefEdge = rep2;
	pRe1->ReBox = pOrginBox;
	pRe1->ReOp = op;
     }
}


/* ---------------------------------------------------------------------- */
/* | InsertDimRelation etablit le lien entre les dimensions             | */
/* |         horizontales ou verticales des deux boites (pOrginBox vers | */
/* |         pTargetBox).                                               | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void       RelDim (PtrBox pOrginBox, PtrBox pTargetBox, boolean MemeDim, boolean horizRef)
#else  /* __STDC__ */
static void       RelDim (pOrginBox, pTargetBox, MemeDim, horizRef)
PtrBox            pOrginBox;
PtrBox            pTargetBox;
boolean             MemeDim;
boolean             horizRef;
#endif /* __STDC__ */
{
   int                 i;
   boolean             boucle;
   boolean             nul;
   PtrDimRelations      cedim;
   PtrDimRelations      pDimRel;

   i = 0;
   /* On determine la dimension affectee */
   if (horizRef)
      if (MemeDim)
	 pDimRel = pOrginBox->BxWidthRelations;
      else
	 pDimRel = pOrginBox->BxHeightRelations;
   else if (MemeDim)
      pDimRel = pOrginBox->BxHeightRelations;
   else
      pDimRel = pOrginBox->BxWidthRelations;

   /* On recherche une entree libre */
   cedim = NULL;
   boucle = TRUE;
   while (boucle && pDimRel != NULL)
     {
	i = 0;
	cedim = pDimRel;
	do
	  {
	     i++;
	     nul = pDimRel->DimRTable[i - 1] == NULL;
	  }
	while (!(i == MAX_RELAT_DIM || nul));

	if (nul)
	   boucle = FALSE;
	else
	   pDimRel = pDimRel->DimRNext;
     }

   /* Faut-il creer un nouveau bloc de relations ? */
   if (boucle)
     {
	GetBDim (&pDimRel);
	if (cedim == NULL)
	   if (horizRef)
	      pOrginBox->BxWidthRelations = pDimRel;
	   else
	      pOrginBox->BxHeightRelations = pDimRel;
	else
	   cedim->DimRNext = pDimRel;
	i = 1;
     }

   pDimRel->DimRTable[i - 1] = pTargetBox;
   pDimRel->DimRSame[i - 1] = MemeDim;
}


/* ---------------------------------------------------------------------- */
/* |    PavDePos retourne le pointeur sur le pave de reference pour le  | */
/* |            positionnement implicite, horizontal ou vertical,       | */
/* |            du pave lepave.                                         | */
/* |            Si la valeur rendue est NULL, le pave se positionne par | */
/* |            rapport au pave englobant.                              | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static PtrAbstractBox   PavDePos (PtrAbstractBox lepave, boolean horizRef)
#else  /* __STDC__ */
static PtrAbstractBox   PavDePos (lepave, horizRef)
PtrAbstractBox      lepave;
boolean             horizRef;
#endif /* __STDC__ */
{
   boolean          encore;
   PtrAbstractBox   pAb;

   encore = TRUE;
   pAb = lepave->AbPrevious;
   while (encore)
      if (pAb == NULL)
	 encore = FALSE;
      else if (pAb->AbDead)
	 pAb = pAb->AbPrevious;
      else if (horizRef)
	 if (pAb->AbHorizPos.PosAbRef == NULL)
	    encore = FALSE;
	 else
	    pAb = pAb->AbPrevious;
      else if (pAb->AbVertPos.PosAbRef == NULL)
	 encore = FALSE;
      else
	 pAb = pAb->AbPrevious;

   return pAb;
}				/* PavDePos */


/* ---------------------------------------------------------------------- */
/* |    PropageXHorsStruct propage l'indicateurs hors-structure.        | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         PropageXHorsStruct (PtrAbstractBox lepave, boolean indicateur, boolean englobe)
#else  /* __STDC__ */
static void         PropageXHorsStruct (lepave, indicateur, englobe)
PtrAbstractBox             lepave;
boolean             indicateur;
boolean             englobe;

#endif /* __STDC__ */
{
   PtrAbstractBox             pAb;
   PtrBox            pBox;

   /* Recherche un pave frere qui depend ce celui-ci */
   if (lepave->AbEnclosing == NULL)
      return;
   else
      pAb = lepave->AbEnclosing->AbFirstEnclosed;

   while (pAb != NULL)
     {
	pBox = pAb->AbBox;
	if (pAb == lepave || pBox == NULL || pAb->AbDead)
	   ;			/* inutile de traiter ce pave */
	else if (pBox->BxXOutOfStruct == indicateur)
	   ;			/* inutile de traiter ce pave */
	else if (pAb->AbHorizPos.PosAbRef == lepave && !pAb->AbHorizPosChange)
	  {
	     /* Dependance de position */
	     pBox->BxXOutOfStruct = indicateur;
	     /* Propage aussi le non englobement */
	     if (pAb->AbEnclosing == lepave->AbEnclosing)
		/* une boite soeur positionnee par rapport a une boite */
		/* elastique non englobee n'est pas englobee elle-meme */
		pAb->AbHorizEnclosing = englobe;
	     PropageXHorsStruct (pAb, indicateur, pAb->AbHorizEnclosing);
	  }
	else if (pAb->AbWidth.DimIsPosition
	&& pAb->AbWidth.DimPosition.PosAbRef == lepave && !pAb->AbWidthChange)
	   /* Dependance de dimension */
	   pBox->BxWOutOfStruct = indicateur;

	/* passe au pave suivant */
	pAb = pAb->AbNext;
     }
}


/* ---------------------------------------------------------------------- */
/* |    PropageYHorsStruct propage l'indicateur hors-structure.         | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         PropageYHorsStruct (PtrAbstractBox lepave, boolean indicateur, boolean englobe)
#else  /* __STDC__ */
static void         PropageYHorsStruct (lepave, indicateur, englobe)
PtrAbstractBox             lepave;
boolean             indicateur;
boolean             englobe;

#endif /* __STDC__ */
{
   PtrAbstractBox             pAb;
   PtrBox            pBox;

   /* Recherche un pave frere qui depend ce celui-ci */
   if (lepave->AbEnclosing == NULL)
      return;
   else
      pAb = lepave->AbEnclosing->AbFirstEnclosed;

   while (pAb != NULL)
     {
	pBox = pAb->AbBox;
	if (pAb == lepave || pBox == NULL || pAb->AbDead)
	   ;			/* inutile de traiter ce pave */
	else if (pBox->BxYOutOfStruct == indicateur)
	   ;			/* inutile de traiter ce pave */
	else if (pAb->AbVertPos.PosAbRef == lepave && !pAb->AbVertPosChange)
	  {
	     pBox->BxYOutOfStruct = indicateur;
	     /* Propage aussi le non englobement */
	     if (pAb->AbEnclosing == lepave->AbEnclosing)
		/* une boite soeur positionnee par rapport a une boite */
		/* elastique non englobee n'est pas englobee elle-meme */
		pAb->AbVertEnclosing = englobe;
	     PropageYHorsStruct (pAb, indicateur, pAb->AbVertEnclosing);
	  }
	else if (pAb->AbHeight.DimIsPosition
	&& pAb->AbHeight.DimPosition.PosAbRef == lepave && !pAb->AbHeightChange)
	   pBox->BxHOutOfStruct = indicateur;

	/* passe au pave suivant */
	pAb = pAb->AbNext;
     }
}


/* ---------------------------------------------------------------------- */
/* |    Positionner applique la regle de positionnement, horizontale ou | */
/* |            verticale selon horizRef sur la boite d'indice pBox.         | */
/* |            Les origines de la boite BxXOrg ou BxYOrg sont mises a` | */
/* |            jour et les relations de dependance sont enregistrees.  | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                Positionner (AbPosition regle, PtrBox pBox, int frame, boolean horizRef)
#else  /* __STDC__ */
void                Positionner (regle, pBox, frame, horizRef)
AbPosition         regle;
PtrBox            pBox;
int                 frame;
boolean             horizRef;

#endif /* __STDC__ */
{
   int                 x, y, dist;
   PtrAbstractBox             pAb;
   PtrAbstractBox             lepave;
   PtrBox            dbox;
   BoxEdge         rd, rl;
   OpRelation          op;
   AbPosition        *pPosAb;

   /* On calcule la position de reference */
   op = (OpRelation) 0;
   rd = (BoxEdge) 0;
   DimFenetre (frame, &x, &y);
   dbox = NULL;
   pAb = regle.PosAbRef;
   lepave = pBox->BxAbstractBox;
   /* Verifie que la position ne depend pas d'un pave mort */
   if (pAb != NULL && pAb->AbDead)
     {
	printf ("Position sur un pave mort");
	pAb = NULL;
     }

   if (horizRef)
     {
	/* On verifie que la boite ne se place pas par rapport a elle-meme */
	if (pAb == lepave)
	  {
	     lepave->AbHorizPos.PosAbRef = NULL;
	     if (lepave->AbWidth.DimIsPosition)
	       {
		  lepave->AbWidth.DimIsPosition = FALSE;
		  lepave->AbWidth.DimUserSpecified = FALSE;
	       }
	     pAb = NULL;	/* Il ne faut pas interpreter cette regle */
	  }

	/* SRule par defaut */
	if (pAb == NULL)
	  {
	     if (lepave->AbEnclosing == NULL)
	       {
		  /* Position dans la fenetre */
		  rd = regle.PosRefEdge;
		  rl = regle.PosEdge;
		  /* Convert the distance value */
		  if (regle.PosUnit == UnPercent)
		     dist = PixelValue (regle.PosDistance, UnPercent, (PtrAbstractBox) x);
		  else
		     dist = PixelValue (regle.PosDistance, regle.PosUnit, lepave);
	       }
	     else
	       {
		  /* Il existe un pave precedent ayant la regle par defaut ? */
		  pAb = PavDePos (lepave, horizRef);
		  /* Si oui -> A droite de sa boite */
		  if (pAb != NULL && regle.PosUnit != UnPercent)
		    {
		       dbox = pAb->AbBox;
		       dist = 0;	/* A droite de la precedente */
		       rd = Right;
		       rl = Left;
		       op = OpHorizDep;
		    }
		  else
		    {
		       /* Initialise sur l'origine de l'englobante (flottante) */
		       pAb = lepave->AbEnclosing;
		       dbox = pAb->AbBox;
		       if (regle.PosUnit == UnPercent)
			  /* poucentage de la largeur de l'englobant */
			  dist = PixelValue (regle.PosDistance, UnPercent, (PtrAbstractBox) lepave->AbEnclosing);
		       else
			  dist = 0;
		       rd = Left;
		       rl = Left;
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
	     if (lepave->AbEnclosing == pAb && !pBox->BxHorizFlex)
		op = OpHorizInc;
	     else
	       {
		  op = OpHorizDep;
		  pBox->BxXToCompute = FALSE;	/* nouvelle regle de placement */
		  if (pAb->AbEnclosing != lepave->AbEnclosing)
		    {
		       /* La boite est liee a une boite hors-structure */
		       if (!XEnAbsolu (pBox))
			  /* la boite  devient maintenant placee en absolu */
			  pBox->BxXToCompute = TRUE;
		       pBox->BxXOutOfStruct = TRUE;
		       PropageXHorsStruct (lepave, TRUE, lepave->AbHorizEnclosing);
		    }
		  else if (pAb->AbBox != NULL)
		    {
		       if (pAb->AbBox->BxXOutOfStruct
			   || (pAb->AbBox->BxHorizFlex)
			   || (pAb->AbBox->BxWOutOfStruct && rd != Left))
			 {
			    /* La boite herite la relation hors-structure */
			    /* ou bien elle est liee a une dimension hors-structure */
			    if (!XEnAbsolu (pBox))
			       /* la boite  devient maintenant placee en absolu */
			       pBox->BxXToCompute = TRUE;
			    pBox->BxXOutOfStruct = TRUE;
			    PropageXHorsStruct (lepave, TRUE, lepave->AbHorizEnclosing);
			 }
		    }

	       }

	     rd = regle.PosRefEdge;
	     rl = regle.PosEdge;
	     /* Convert the distance value */
	     if (regle.PosUnit == UnPercent)
	       {
		  dist = PixelValue (regle.PosDistance, UnPercent, (PtrAbstractBox) lepave->AbEnclosing->AbBox->BxWidth);
		  /* Change the rule for further updates */
		  lepave->AbHorizPos.PosDistance = dist;
		  lepave->AbHorizPos.PosUnit = UnPixel;
	       }
	     else
		dist = PixelValue (regle.PosDistance, regle.PosUnit, lepave);
	  }
     }
   else
      /* EnY */
     {
	/* On verifie que la boite ne se place pas par rapport a elle-meme */
	if (pAb == lepave)
	  {
	     lepave->AbVertPos.PosAbRef = NULL;
	     if (lepave->AbHeight.DimIsPosition)
	       {
		  lepave->AbHeight.DimIsPosition = FALSE;
		  lepave->AbHeight.DimUserSpecified = FALSE;
	       }
	     pAb = NULL;	/* Il ne faut pas interpreter cette regle */
	  }

	/* SRule par defaut */
	if (pAb == NULL)
	  {
	     if (lepave->AbEnclosing == NULL)
	       {
		  /* Position dans la fenetre */
		  rd = regle.PosRefEdge;
		  rl = regle.PosEdge;
		  /* Convert the distance value */
		  if (regle.PosUnit == UnPercent)
		     dist = PixelValue (regle.PosDistance, UnPercent, (PtrAbstractBox) y);
		  else
		     dist = PixelValue (regle.PosDistance, regle.PosUnit, lepave);
	       }
	     else
	       {
		  /* Il existe un pave precedent ayant la regle par defaut ? */
		  pAb = PavDePos (lepave, horizRef);
		  /* Si oui -> A droite de sa boite */
		  if (pAb != NULL)
		    {
		       dbox = pAb->AbBox;
		       dist = 0;	/* A droite de la precedente */
		       rd = HorizRef;
		       rl = HorizRef;
		       op = OpVertDep;
		    }
		  else
		    {
		       /* Initialise sur l'origine de l'englobante (flottante) */
		       pAb = lepave->AbEnclosing;
		       dbox = pAb->AbBox;
		       dist = 0;
		       rd = Top;
		       rl = Top;
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
	     if (lepave->AbEnclosing == pAb && !pBox->BxVertFlex)
		op = OpVertInc;
	     else
	       {
		  op = OpVertDep;
		  pBox->BxYToCompute = FALSE;	/* nouvelle regle de placement */
		  if (pAb->AbEnclosing != lepave->AbEnclosing)
		    {
		       /* La boite est liee a une boite hors-structure */
		       if (!YEnAbsolu (pBox))
			  /* la boite  est maintenant placee en absolu */
			  pBox->BxYToCompute = TRUE;
		       pBox->BxYOutOfStruct = TRUE;
		       PropageYHorsStruct (lepave, TRUE, lepave->AbVertEnclosing);
		    }
		  else if (pAb->AbBox != NULL)
		    {
		       if (pAb->AbBox->BxYOutOfStruct
			   || (pAb->AbBox->BxHorizFlex && pAb->AbLeafType == LtCompound && pAb->AbInLine && rd != Top)
			   || (pAb->AbBox->BxHOutOfStruct && rd != Top))
			 {
			    /* La boite herite la relation hors-structure */
			    /* ou bien elle est liee a une dimension hors-structure */
			    if (!YEnAbsolu (pBox))
			       /* la boite  est maintenant placee en absolu */
			       pBox->BxYToCompute = TRUE;
			    pBox->BxYOutOfStruct = TRUE;
			    PropageYHorsStruct (lepave, TRUE, lepave->AbVertEnclosing);
			 }
		    }
	       }

	     rd = regle.PosRefEdge;
	     rl = regle.PosEdge;
	     /* Convert the distance value */
	     if (regle.PosUnit == UnPercent)
	       {
		  dist = PixelValue (regle.PosDistance, UnPercent, (PtrAbstractBox) lepave->AbEnclosing->AbBox->BxHeight);
		  /* Change the rule for further updates */
		  lepave->AbVertPos.PosDistance = dist;
		  lepave->AbVertPos.PosUnit = UnPixel;
	       }
	     else
		dist = PixelValue (regle.PosDistance, regle.PosUnit, lepave);
	  }
     }

   /* Deplacement par rapport a la boite distante */
   if (pAb != NULL)
     {
	dbox = pAb->AbBox;
	if (dbox == NULL)
	  {
	     /* On doit resoudre une reference en avant */
	     dbox = GetBox (pAb);
	     if (dbox != NULL)
		pAb->AbBox = dbox;
	     else
		/* plus de boite libre */
		return;
	  }

	x = dbox->BxXOrg;
	y = dbox->BxYOrg;
	switch (rd)
	      {
		 case Left:
		 case Top:
		    break;
		 case Bottom:
		    y += dbox->BxHeight;
		    break;
		 case Right:
		    x += dbox->BxWidth;
		    break;
		 case HorizRef:
		    y += dbox->BxHorizRef;
		    break;
		 case VertRef:
		    x += dbox->BxVertRef;
		    break;
		 case HorizMiddle:
		    y += dbox->BxHeight / 2;
		    break;
		 case VertMiddle:
		    x += dbox->BxWidth / 2;
		    break;
		 default:
		    break;
	      }
     }
   else
     {
	switch (rd)
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
   switch (rl)
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

   /* On regarde si la position depend d'une boite invisible */
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
	lepave->AbHorizPosChange = FALSE;	/* la regle de position est interpretee */
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
	lepave->AbVertPosChange = FALSE;	/* la regle de position est interpretee */
     }

   /* Il faut mettre a jour les dependances des boites */
   if (pAb != NULL && dbox != NULL)
     {
	RelPos (pBox, dbox, op, rl, rd);
	dbox->BxMoved = NULL;

	if (horizRef && pBox->BxHorizFlex)
	   ChngBElast (pBox, dbox, op, x + dist - pBox->BxXOrg, frame, TRUE);
	else if (!horizRef && pBox->BxVertFlex)
	   ChngBElast (pBox, dbox, op, y + dist - pBox->BxYOrg, frame, FALSE);
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
PtrAbstractBox             pAb;
int                 frame;
boolean             horizRef;

#endif /* __STDC__ */
{
   int                 val, delta, i;
   PtrBox            dbox;
   PtrAbstractBox             lepave;
   boolean             misenligne;
   boolean             setdim;
   ptrfont             font;
   OpRelation          op;
   AbDimension       *pDimAb;
   AbPosition        *pPosAb;
   PtrAbstractBox             cepave;
   PtrBox            pBo1;

   pBo1 = pAb->AbBox;
   setdim = FALSE;		/* A priori, la dimension ne depend pas de son contenu */

   /* On verifie que la boite est visible */
   if (pAb->AbVisibility >= FntrTable[frame - 1].FrVisibility)
     {
	lepave = pAb->AbEnclosing;
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
		  printf ("Dimension par rapport a un pave mort");
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
		  printf ("Dimension par rapport a un pave mort");
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
		  printf ("Dimension par rapport a un pave mort");
		  pDimAb->DimAbRef = NULL;
		  pDimAb->DimValue = 0;
	       }

	     /* Est-ce la boite racine ? */
	     if (lepave == NULL)
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
		  misenligne = lepave->AbInLine || lepave->AbBox->BxType == BoGhost;
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
				  delta = PixelValue (pDimAb->DimValue, UnPercent, (PtrAbstractBox) lepave->AbBox->BxWidth);
				  RelDim (lepave->AbBox, pBo1, pDimAb->DimSameDimension, horizRef);
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
			  if (pAb->AbWidth.DimAbRef == lepave && lepave->AbEnclosing != NULL
			      && lepave->AbWidth.DimAbRef == NULL && lepave->AbWidth.DimValue <= 0
			  /* -> ET l'englobante est mise en lignes (extensibles) */
			  /* OU la boite n'est pas collee au cote gauche de son englobante */
			      && (misenligne || pPosAb->PosAbRef != lepave
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
				    dbox = pDimAb->DimAbRef->AbBox;
				    if (dbox == NULL)
				      {
					 /* On doit resoudre une reference en avant */
					 dbox = GetBox (pDimAb->DimAbRef);
					 if (dbox != NULL)
					    pDimAb->DimAbRef->AbBox = dbox;
				      }

				    if (dbox != NULL)
				      {
					 /* On regarde s'il s'agit de la meme dimension */
					 if (pDimAb->DimSameDimension)
					    val = dbox->BxWidth;
					 else
					    val = dbox->BxHeight;
					 /* Quand la boite prend la largeur de la ligne qui */
					 /* l'englobe ->retire la valeur de l'indentation   */
					 if (pDimAb->DimAbRef == lepave && misenligne && pDimAb->DimSameDimension)
					   {
					      if (lepave->AbIndentUnit == UnPercent)
						 delta = PixelValue (lepave->AbIndent, UnPercent, (PtrAbstractBox) val);
					      else
						 delta = PixelValue (lepave->AbIndent, lepave->AbIndentUnit, lepave);
					      if (lepave->AbIndent > 0)
						 val -= delta;
					      else if (lepave->AbIndent < 0)
						 val += delta;
					   }

					 /* Convert the distance value */
					 if (pDimAb->DimUnit == UnPercent)
					    val = PixelValue (pDimAb->DimValue, UnPercent, (PtrAbstractBox) val);
					 else
					    val += PixelValue (pDimAb->DimValue, pDimAb->DimUnit, pAb);
					 ModLarg (pBo1, pBo1, NULL, val - pBo1->BxWidth, 0, frame);
					 /* On teste si la relation est hors structure */
					 if (pDimAb->DimAbRef != lepave
					     && pDimAb->DimAbRef->AbEnclosing != lepave)
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
			     delta = PixelValue (pDimAb->DimValue, UnPercent, (PtrAbstractBox) lepave->AbBox->BxHeight);
			  else
			     delta = PixelValue (pDimAb->DimValue, pDimAb->DimUnit, pAb);
			  ModHaut (pBo1, pBo1, NULL, delta - pBo1->BxHeight, frame);
		       }
		  /* Deuxieme cas de coherence */
		  /* La boite ne peut pas prendre la taille de son englobante si : */
		  /* -> L'englobante est mise en ligne */
		  else if (misenligne && pAb->AbHeight.DimAbRef == lepave
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
		       if (pAb->AbHeight.DimAbRef == lepave
			   && lepave->AbEnclosing != NULL
			   && lepave->AbHeight.DimAbRef == NULL && lepave->AbHeight.DimValue <= 0
		       /* ET la boite n'est pas collee au cote superieur de son englobante */
			   && (pPosAb->PosAbRef != lepave || pPosAb->PosRefEdge != Top
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
				 dbox = pDimAb->DimAbRef->AbBox;
				 if (dbox == NULL)
				   {
				      /* On doit resoudre une reference en avant */
				      dbox = GetBox (pDimAb->DimAbRef);
				      if (dbox != NULL)
					 pDimAb->DimAbRef->AbBox = dbox;
				   }

				 if (dbox != NULL)
				   {
				      /* On regarde s'il s'agit de la meme dimension */
				      if (pDimAb->DimSameDimension)
					 val = dbox->BxHeight;
				      else
					 val = dbox->BxWidth;

				      /* Quand la boite prend la largeur de la ligne qui */
				      /* l'englobe -> retire la valeur de l'indentation  */
				      if (pDimAb->DimAbRef == lepave && misenligne
					  && !pDimAb->DimSameDimension)
					{
					   if (lepave->AbIndentUnit == UnPercent)
					      delta = PixelValue (lepave->AbIndent, UnPercent, (PtrAbstractBox) val);
					   else
					      delta = PixelValue (lepave->AbIndent, lepave->AbIndentUnit, lepave);
					   if (lepave->AbIndent > 0)
					      val -= delta;
					   else if (lepave->AbIndent < 0)
					      val += delta;
					}

				      /* Convert the distance value */
				      if (pDimAb->DimUnit == UnPercent)
					 val = PixelValue (pDimAb->DimValue, UnPercent, (PtrAbstractBox) val);
				      else
					 val += PixelValue (pDimAb->DimValue, pDimAb->DimUnit, pAb);
				      ModHaut (pBo1, pBo1, NULL, val - pBo1->BxHeight, frame);
				      /* On teste si la relation est hors structure */
				      if (pDimAb->DimAbRef != lepave
					  && pDimAb->DimAbRef->AbEnclosing != lepave)
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
	     if (pPosAb->PosAbRef != lepave
		 && pPosAb->PosAbRef->AbEnclosing != lepave)
		pBo1->BxWOutOfStruct = TRUE;
	     else if (pPosAb->PosAbRef->AbBox != NULL)
		pBo1->BxWOutOfStruct = pPosAb->PosAbRef->AbBox->BxXOutOfStruct;

	     /* Des boites voisines heritent de la relation hors-structure ? */
	     if (lepave != NULL)
	       {
		  cepave = lepave->AbFirstEnclosed;
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
				 PropageXHorsStruct (cepave, TRUE, cepave->AbHorizEnclosing);
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
				 PropageYHorsStruct (cepave, TRUE, cepave->AbVertEnclosing);
			      }
			 }
		       cepave = cepave->AbNext;
		    }
	       }

	     /* Decalage par rapport a la boite distante */
	     dbox = pPosAb->PosAbRef->AbBox;
	     if (dbox == NULL)
	       {
		  /* On doit resoudre une reference en avant */
		  if (!pPosAb->PosAbRef->AbDead)
		     dbox = GetBox (pPosAb->PosAbRef);
		  if (dbox != NULL)
		     pPosAb->PosAbRef->AbBox = dbox;
	       }

	     if (dbox != NULL)
	       {
		  /* On regarde si la position depend d'une boite invisible */
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

		  val = dbox->BxXOrg + delta;
		  switch (pPosAb->PosRefEdge)
			{
			   case Left:
			      break;
			   case Right:
			      val += dbox->BxWidth;
			      break;
			   case VertMiddle:
			      val += dbox->BxWidth / 2;
			      break;
			   case VertRef:
			      val += dbox->BxVertRef;
			      break;
			   default:
			      break;
			}

		  /* Calcule la largeur de la boite */
		  val = val - pBo1->BxXOrg - pBo1->BxWidth;
		  /* La boite n'a pas de point fixe */
		  pBo1->BxHorizEdge = NoEdge;
		  RelPos (pBo1, dbox, op, pPosAb->PosEdge, pPosAb->PosRefEdge);

		  if (!XEnAbsolu (pBo1))
		     /* la boite  devient maintenant placee en absolu */
		     pBo1->BxXToCompute = TRUE;
		  /* La boite est marquee elastique */
		  pBo1->BxHorizFlex = TRUE;
		  dbox->BxMoved = NULL;
		  ChngBElast (pBo1, dbox, op, val, frame, TRUE);
	       }
	  }
	else
	  {
	     pDimAb = &pAb->AbHeight;
	     /* Box elastique en Y */
	     pPosAb = &pDimAb->DimPosition;
	     op = OpHeight;

	     /* On teste si la relation est hors structure */
	     if (pPosAb->PosAbRef != lepave
		 && pPosAb->PosAbRef->AbEnclosing != lepave)
		pBo1->BxHOutOfStruct = TRUE;
	     else if (pPosAb->PosAbRef->AbBox != NULL)
		pBo1->BxHOutOfStruct = pPosAb->PosAbRef->AbBox->BxYOutOfStruct;

	     /* Des boites voisines heritent de la relation hors-structure ? */
	     if (lepave != NULL /* && pBo1->BxHOutOfStruct */ )
	       {
		  cepave = lepave->AbFirstEnclosed;
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
			       PropageYHorsStruct (cepave, TRUE, cepave->AbVertEnclosing);
			    }
		       cepave = cepave->AbNext;
		    }
	       }

	     /* Decalage par rapport a la boite distante */
	     dbox = pPosAb->PosAbRef->AbBox;
	     if (dbox == NULL)
	       {
		  /* On doit resoudre une reference en avant */
		  dbox = GetBox (pPosAb->PosAbRef);
		  if (dbox != NULL)
		     pPosAb->PosAbRef->AbBox = dbox;
	       }

	     if (dbox != NULL)
	       {
		  /* On regarde si la position depend d'une boite invisible */
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

		  val = dbox->BxYOrg + delta;
		  switch (pPosAb->PosRefEdge)
			{
			   case Bottom:
			      val += dbox->BxHeight;
			      break;
			   case HorizMiddle:
			      val += dbox->BxHeight / 2;
			      break;
			   case HorizRef:
			      val += dbox->BxHorizRef;
			      break;
			   default:
			      break;
			}

		  /* Calcule la hauteur de la boite */
		  val = val - pBo1->BxYOrg - pBo1->BxHeight;
		  /* La boite n'a pas de point fixe */
		  pBo1->BxVertEdge = NoEdge;
		  RelPos (pBo1, dbox, op, pPosAb->PosEdge, pPosAb->PosRefEdge);

		  if (!YEnAbsolu (pBo1))
		     /* la boite  devient maintenant placee en absolu */
		     pBo1->BxYToCompute = TRUE;
		  /* La boite est marquee elastique */
		  pBo1->BxVertFlex = TRUE;
		  dbox->BxMoved = NULL;
		  ChngBElast (pBo1, dbox, op, val, frame, FALSE);
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
}				/* Dimensionner */


/* ---------------------------------------------------------------------- */
/* |    PlacerAxe applique la regle de positionnement donnee en         | */
/* |            parametre a` la boite d'indice pBox.                    | */
/* |            L'axe horizontal ou vertical de la boite, selon que horizRef | */
/* |            est VRAI ou FAUX est mis a` jour.                       | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                PlacerAxe (AbPosition regle, PtrBox pBox, int frame, boolean horizRef)
#else  /* __STDC__ */
void                PlacerAxe (regle, pBox, frame, horizRef)
AbPosition         regle;
PtrBox            pBox;
int                 frame;
boolean             horizRef;

#endif /* __STDC__ */
{
   int                 x, y, dist;
   PtrBox            dbox;
   PtrAbstractBox             pAb;
   PtrAbstractBox             lepave;
   BoxEdge         rd, rl;

   /* Calcule la position de reference */
   pAb = regle.PosAbRef;
   lepave = pBox->BxAbstractBox;
   /* Verifie que la position ne depend pas d'un pave mort */
   if (pAb != NULL && pAb->AbDead)
     {
	printf ("Position axe sur un  pave mort");
	pAb = NULL;
     }

   /* SRule par defaut */
   if (pAb == NULL)
     {
	dbox = pBox;
	if (horizRef)
	  {
	     rd = Left;
	     rl = VertRef;
	     if (regle.PosUnit == UnPercent)
		dist = PixelValue (regle.PosDistance, UnPercent, (PtrAbstractBox) pBox->BxWidth);
	     else
		dist = 0;
	  }
	else
	  {
	     rd = Bottom;
	     rl = HorizRef;
	     if (regle.PosUnit == UnPercent)
		dist = PixelValue (regle.PosDistance, UnPercent, (PtrAbstractBox) pBox->BxHeight);
	     if (lepave->AbLeafType == LtText)
		dist = FontBase (pBox->BxFont) - pBox->BxHeight;
	     else
		dist = 0;
	  }
     }
   /* SRule explicite */
   else
     {
	rd = regle.PosRefEdge;
	rl = regle.PosEdge;
	/* Convert the distance value */
	if (regle.PosUnit == UnPercent)
	  {
	     if (horizRef)
		dist = PixelValue (regle.PosDistance, UnPercent, (PtrAbstractBox) lepave->AbEnclosing->AbBox->BxWidth);
	     else
		dist = PixelValue (regle.PosDistance, UnPercent, (PtrAbstractBox) lepave->AbEnclosing->AbBox->BxHeight);
	  }
	else
	   dist = PixelValue (regle.PosDistance, regle.PosUnit, lepave);

	dbox = pAb->AbBox;
	if (dbox == NULL)
	  {
	     /* On doit resoudre une reference en avant */
	     dbox = GetBox (pAb);
	     if (dbox != NULL)
		pAb->AbBox = dbox;
	     else
	       {
		  if (horizRef)
		     lepave->AbVertRefChange = FALSE;
		  else
		     lepave->AbHorizRefChange = FALSE;
		  return;	/* plus de boite libre */
	       }
	  }
     }

   /* Deplacement par rapport a la boite distante */
   /* L'axe est place par rapport a la boite elle-meme */
   if (dbox == pBox
       || pBox->BxType == BoGhost
       || (lepave->AbInLine && lepave->AbLeafType == LtCompound))
     {
	x = 0;
	y = 0;
     }
   /* L'axe est place par rapport a une incluse */
   else if (Propage != ToSiblings)
     {
	/* Il faut peut-etre envisager que dbox soit une boite coupee */
	x = dbox->BxXOrg - pBox->BxXOrg;
	y = dbox->BxYOrg - pBox->BxYOrg;
     }
   else
     {
	x = dbox->BxXOrg;
	y = dbox->BxYOrg;
     }

   switch (rd)
	 {
	    case Left:
	    case Top:
	       break;
	    case Bottom:
	       y += dbox->BxHeight;
	       break;
	    case Right:
	       x += dbox->BxWidth;
	       break;
	    case HorizRef:
	       y += dbox->BxHorizRef;
	       break;
	    case VertRef:
	       x += dbox->BxVertRef;
	       break;
	    case HorizMiddle:
	       y += dbox->BxHeight / 2;
	       break;
	    case VertMiddle:
	       x += dbox->BxWidth / 2;
	       break;
	    case NoEdge:
	       break;
	 }

   /* Met a jour l'axe de la boite */
   if (horizRef)
     {
	x = x + dist - pBox->BxVertRef;
	DepAxe (pBox, NULL, x, frame);
	lepave->AbVertRefChange = FALSE;	/* la regle axe de reference est interpretee */
	if (dbox != NULL)
	   RelPos (pBox, dbox, OpHorizRef, rl, rd);
     }
   else
     {
	y = y + dist - pBox->BxHorizRef;
	DepBase (pBox, NULL, y, frame);
	lepave->AbHorizRefChange = FALSE;	/* la regle axe de reference est interpretee */
	if (dbox != NULL)
	   RelPos (pBox, dbox, OpVertRef, rl, rd);
     }
}				/* PlacerAxe */

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
   int                 i;
   PtrPosRelations      adpos;
   boolean             nonnul;
   BoxRelation           *pRe1;
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
	/* On regarde si on connait deja la boite qui la relie a l'englobante */
	if (pBox->BxHorizInc != NULL)
	   box1 = pBox->BxHorizInc;
	else
	   box1 = NULL;

	/* Si la position de la boite depend d'une boite externe on prend la */
	/* boite elle meme comme reference.                                  */
	if (pBox->BxXOutOfStruct)
	   box1 = pBox;

	/* On regarde si la boite est reliee a son englobante */
	adpos = pBox->BxPosRelations;
	while (box1 == NULL && adpos != NULL)
	  {
	     i = 1;
	     nonnul = adpos->PosRTable[i - 1].ReBox != NULL;
	     while (i <= MAX_RELAT_POS && nonnul)
		if (adpos->PosRTable[i - 1].ReOp == OpHorizInc)
		  {
		     box1 = pBox;
		     i = MAX_RELAT_POS + 1;
		     /* On a trouve */
		  }
		else
		  {
		     i++;
		     if (i <= MAX_RELAT_POS)
			nonnul = adpos->PosRTable[i - 1].ReBox != NULL;
		  }

	     adpos = adpos->PosRNext;
	     /* Bloc suivant */
	  }

	/* Sinon on recherche la boite soeur qui l'est */
	adpos = pBox->BxPosRelations;
	while (box1 == NULL && adpos != NULL)
	  {
	     i = 1;
	     nonnul = adpos->PosRTable[i - 1].ReBox != NULL;
	     while (i <= MAX_RELAT_POS && nonnul)
	       {
		  pRe1 = &adpos->PosRTable[i - 1];
		  if (pRe1->ReBox->BxAbstractBox != NULL)
		     if (pRe1->ReOp == OpHorizDep
		     /* Si c'est la bonne relation de dependance */
			 && pRe1->ReBox->BxAbstractBox->AbHorizPos.PosAbRef != pBox->BxAbstractBox)
		       {
			  /* Si la position de la boite depend d'une boite elastique */
			  /* on prend la boite elastique comme reference             */
			  if (pRe1->ReBox->BxHorizFlex)
			     box1 = pRe1->ReBox;
			  else
			     box1 = BoiteHInclus (pRe1->ReBox, pBox);


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
				  nonnul = adpos->PosRTable[i - 1].ReBox != NULL;
			    }
		       }
		     else
		       {
			  i++;
			  if (i <= MAX_RELAT_POS)
			     nonnul = adpos->PosRTable[i - 1].ReBox != NULL;
		       }
	       }
	     adpos = adpos->PosRNext;	/* Bloc suivant */
	  }

	pBox->BxHorizInc = box1;
	Result = box1;
     }
   else
      Result = NULL;
   return Result;
}				/* BoiteHInclus */

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
   int                 i;
   PtrPosRelations      adpos;
   boolean             nonnul;
   BoxRelation           *pRe1;
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

	/* On regarde si on connait deja la boite qui la relie a l'englobante */
	if (pBox->BxVertInc != NULL)
	   box1 = pBox->BxVertInc;
	else
	   box1 = NULL;

	/* Si la position de la boite depend d'une boite externe on prend la */
	/* boite elle meme comme reference.                                  */
	if (pBox->BxYOutOfStruct)
	   box1 = pBox;

	/* On regarde si la boite est reliee a son englobante */
	adpos = pBox->BxPosRelations;
	while (box1 == NULL && adpos != NULL)
	  {
	     i = 1;
	     nonnul = adpos->PosRTable[i - 1].ReBox != NULL;
	     while (i <= MAX_RELAT_POS && nonnul)
		if (adpos->PosRTable[i - 1].ReOp == OpVertInc)
		  {
		     box1 = pBox;
		     i = MAX_RELAT_POS + 1;	/* On a trouve */
		  }
		else
		  {
		     i++;
		     if (i <= MAX_RELAT_POS)
			nonnul = adpos->PosRTable[i - 1].ReBox != NULL;
		  }
	     adpos = adpos->PosRNext;	/* Bloc suivant */

	  }

	/* Sinon on recherche la boite soeur qui l'est */
	adpos = pBox->BxPosRelations;
	while (box1 == NULL && adpos != NULL)
	  {
	     i = 1;
	     nonnul = adpos->PosRTable[i - 1].ReBox != NULL;
	     while (i <= MAX_RELAT_POS && nonnul)
	       {
		  pRe1 = &adpos->PosRTable[i - 1];
		  if (pRe1->ReBox->BxAbstractBox != NULL)
		     if (pRe1->ReOp == OpVertDep
		     /* Si c'est la bonne relation de dependance */
			 && pRe1->ReBox->BxAbstractBox->AbVertPos.PosAbRef != pBox->BxAbstractBox)
		       {

			  /* Si la position de la boite depend d'une boite elastique */
			  /* on prend la boite elastique comme reference             */
			  if (pRe1->ReBox->BxVertFlex)
			     box1 = pRe1->ReBox;
			  else
			     box1 = BoiteVInclus (pRe1->ReBox, pBox);
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
				  nonnul = adpos->PosRTable[i - 1].ReBox != NULL;
			    }
		       }
		     else
		       {
			  i++;
			  if (i <= MAX_RELAT_POS)
			     nonnul = adpos->PosRTable[i - 1].ReBox != NULL;
		       }
	       }
	     adpos = adpos->PosRNext;	/* Bloc suivant */
	  }

	pBox->BxVertInc = box1;
	result = box1;
     }
   else
      result = NULL;
   return result;
}				/* BoiteVInclus */

/* ---------------------------------------------------------------------- */
/* |    DelPos defait, s'il existe, le lien de dependance de position ou| */
/* |            d'axe horizontal ou vertical de la boite pOrginBox et       | */
/* |            retasse la liste des liens.                             | */
/* |            Le parametre lepave, quand il est non nul, indique le   | */
/* |            pave dont on annule la regle de position et sert a`     | */
/* |            retirer l'ambiguite des relations doubles entre boites  | */
/* |            soeurs : si on trouve une relation associee a` une      | */
/* |            regle de position qui reference lepave, cette relation  | */
/* |            ne doit pas etre detruite. Quand lepave est nul, on     | */
/* |            connait la boite pTargetBox referencee dans le lien que l'on  | */
/* |            veut detruire (c'est alors une destruction de lien      | */
/* |            inverse).                                               | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static boolean      DelPos (PtrBox pOrginBox, PtrBox pTargetBox, PtrAbstractBox lepave, boolean Pos, boolean Axe, boolean horizRef)
#else  /* __STDC__ */
static boolean      DelPos (pOrginBox, pTargetBox, lepave, Pos, Axe, horizRef)
PtrBox            pOrginBox;
PtrBox            pTargetBox;
PtrAbstractBox             lepave;
boolean             Pos;
boolean             Axe;
boolean             horizRef;

#endif /* __STDC__ */
{
   int                 i, trouve;
   int                 j, k;
   boolean             boucle;
   boolean             nonnul;
   PtrPosRelations      cepos;
   PtrPosRelations      precpos;
   PtrPosRelations      adpos;
   PtrAbstractBox             pAb;
   BoxRelation           *pRe1;
   boolean             result;


   /* On recherche l'entree a detruire et la derniere entree occupee */
   trouve = 0;
   i = 0;
   cepos = NULL;
   adpos = pOrginBox->BxPosRelations;
   precpos = NULL;
   boucle = TRUE;
   if (adpos != NULL)
      while (boucle)
	{
	   i = 1;
	   nonnul = adpos->PosRTable[i - 1].ReBox != NULL;
	   while (i <= MAX_RELAT_POS && nonnul)
	     {
		pRe1 = &adpos->PosRTable[i - 1];
		/* Si c'est une relation morte, on retasse la liste */
		if (pRe1->ReBox->BxAbstractBox == NULL)
		  {
		     j = i;
		     while (j < MAX_RELAT_POS)
		       {
			  k = j + 1;
			  adpos->PosRTable[j - 1].ReBox = adpos->PosRTable[k - 1].ReBox;
			  adpos->PosRTable[j - 1].ReRefEdge = adpos->PosRTable[k - 1].ReRefEdge;
			  adpos->PosRTable[j - 1].ReOp = adpos->PosRTable[k - 1].ReOp;
			  if (adpos->PosRTable[k - 1].ReBox == NULL)
			     j = MAX_RELAT_POS;
			  else
			    {
			       j++;
			       /* Faut-il annuler la derniere entree ? */
			       if (j == MAX_RELAT_POS)
				  adpos->PosRTable[j - 1].ReBox = NULL;
			    }
		       }

		     /* C'etait la derniere relation dans la table ? */
		     if (i == MAX_RELAT_POS)
			pRe1->ReBox = NULL;
		     else
			i--;	/* Il faut reexaminer cette entree */
		  }
		/* Si c'est une relation en X */
		else if (horizRef)
		  {
		     /* Est-ce l'entree a detruite ? */
		     if (pRe1->ReBox == pTargetBox
			 && ((Axe && pRe1->ReOp == OpHorizRef)
			     || (Pos && pRe1->ReOp == OpHorizInc)
			  || (Pos && lepave == NULL && pRe1->ReOp == OpHorizDep)
		     /* Ni Axe ni Pos quand il s'agit d'une dimension elastique */
			     || (!Pos && !Axe && pRe1->ReOp == OpWidth)))
		       {
			  trouve = i;
			  cepos = adpos;
		       }

		     /* Est-ce la relation de position du pave lepave ? */
		     else if (trouve == 0 && Pos && pRe1->ReOp == OpHorizDep
			      && lepave != NULL && pRe1->ReBox == pTargetBox)
		       {
			  pAb = pRe1->ReBox->BxAbstractBox;
			  pAb = pAb->AbHorizPos.PosAbRef;

			  /* Si la position du pave distant est donnee par une    */
			  /* regle NULL, il faut rechercher le pave dont il depend */
			  if (pAb == NULL)
			     pAb = PavDePos (pRe1->ReBox->BxAbstractBox, horizRef);

			  /* On a bien trouve la relation de positionnement       */
			  /* du pave lepave et non une relation de positionnement */
			  /* du pave distant par rapport au pave lepave ?         */
			  if (pAb != lepave)
			    {
			       trouve = i;
			       cepos = adpos;
			    }
		       }
		  }
		/* Si c'est une relation en Y */
		/* Est-ce l'entree a detruite ? */
		else if (pRe1->ReBox == pTargetBox
			 && ((Axe && pRe1->ReOp == OpVertRef)
			     || (Pos && pRe1->ReOp == OpVertInc)
			  || (Pos && lepave == NULL && pRe1->ReOp == OpVertDep)
		   /* Ni Axe ni Pos quand il s'agit d'une dimension elastique */
			     || (!Pos && !Axe && pRe1->ReOp == OpHeight)))
		  {
		     trouve = i;
		     cepos = adpos;
		  }

		/* Est-ce la relation de position du pave lepave ? */
		else if (trouve == 0 && Pos && pRe1->ReOp == OpVertDep
			 && lepave != NULL && pRe1->ReBox == pTargetBox)
		  {
		     pAb = pRe1->ReBox->BxAbstractBox;
		     pAb = pAb->AbVertPos.PosAbRef;

		     /* Si la position du pave distant est donnee par une    */
		     /* regle NULL, il faut rechercher le pave dont il depend */
		     if (pAb == NULL)
			pAb = PavDePos (pRe1->ReBox->BxAbstractBox, horizRef);

		     /* On a bien trouve la relation de positionnement       */
		     /* du pave lepave et non une relation de positionnement */
		     /* du pave distant par rapport au pave lepave ?         */
		     if (pAb != lepave)
		       {
			  trouve = i;
			  cepos = adpos;
		       }
		  }
		i++;
		if (i <= MAX_RELAT_POS)
		   nonnul = adpos->PosRTable[i - 1].ReBox != NULL;
	     }

	   if (adpos->PosRNext == NULL)
	      boucle = FALSE;
	   else
	     {
		precpos = adpos;
		adpos = adpos->PosRNext;	/* Bloc suivant */
	     }
	}
   /* On a trouve -> on retasse la liste */
   if (trouve > 0)
     {
	pRe1 = &cepos->PosRTable[trouve - 1];

	/* Faut-il defaire la relation inverse ? */
	if (lepave != NULL && (pRe1->ReOp == OpHorizDep || pRe1->ReOp == OpVertDep))
	   boucle = DelPos (pRe1->ReBox, pOrginBox, NULL, Pos, Axe, horizRef);

	if (i > 1)
	  {
	     i--;
	     pRe1->ReBox = adpos->PosRTable[i - 1].ReBox;
	     pRe1->ReRefEdge = adpos->PosRTable[i - 1].ReRefEdge;
	     pRe1->ReOp = adpos->PosRTable[i - 1].ReOp;
	  }

	/* Faut-il liberer le dernier bloc de relations ? */
	if (i == 1)
	  {
	     if (precpos == NULL)
		pOrginBox->BxPosRelations = NULL;
	     else
		precpos->PosRNext = NULL;
	     FreeBPos (&adpos);
	  }
	else
	   adpos->PosRTable[i - 1].ReBox = NULL;
	result = TRUE;
     }
   else
      result = FALSE;
   return result;
}				/* DelPos */

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
   boolean             boucle;
   boolean             nonnul;
   PtrDimRelations      cedim;
   PtrDimRelations      precdim;
   PtrDimRelations      pDimRel;
   boolean             result;


   i = 0;
   /* Cela peut etre une dimension elastique */
   if ((horizRef && pTargetBox->BxHorizFlex) || (!horizRef && pTargetBox->BxVertFlex))
      result = DelPos (pOrginBox, pTargetBox, NULL, FALSE, FALSE, horizRef);
   else
     {
	/* On recherche l'entree a detruire et la derniere entree occupee */
	trouve = 0;
	cedim = NULL;
	if (horizRef)
	   pDimRel = pOrginBox->BxWidthRelations;
	else
	   pDimRel = pOrginBox->BxHeightRelations;
	precdim = NULL;
	boucle = TRUE;
	if (pDimRel != NULL)

	   while (boucle)
	     {
		i = 1;
		nonnul = pDimRel->DimRTable[i - 1] != NULL;
		while (i <= MAX_RELAT_DIM && nonnul)
		  {
		     /* Est-ce l'entree a detruire ? */
		     if (pDimRel->DimRTable[i - 1] == pTargetBox)
		       {
			  trouve = i;
			  cedim = pDimRel;
		       }
		     i++;
		     if (i <= MAX_RELAT_DIM)
			nonnul = pDimRel->DimRTable[i - 1] != NULL;
		  }

		if (pDimRel->DimRNext == NULL)
		   boucle = FALSE;
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
	     cedim->DimRTable[trouve - 1] = pDimRel->DimRTable[i - 1];
	     cedim->DimRSame[trouve - 1] = pDimRel->DimRSame[i - 1];

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
/* |            pTargetBox.                                                   | */
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
	   DelPos (pOrginBox, pTargetBox, NULL, TRUE, FALSE, TRUE);

	/* Annule les relations hors-structure */
	PropageXHorsStruct (pAbbox1, FALSE, pAbbox1->AbHorizEnclosing);
     }

   /* On detruit la relation de position verticale hors-structure */
   if (pTargetBox->BxYOutOfStruct)
     {
	if (pAbbox1->AbVertPos.PosAbRef == NULL)
	   pOrginBox = NULL;
	else
	   pOrginBox = pAbbox1->AbVertPos.PosAbRef->AbBox;
	if (pOrginBox != NULL)
	   DelPos (pOrginBox, pTargetBox, NULL, TRUE, FALSE, FALSE);

	/* Annule les relations hors-structure */
	PropageYHorsStruct (pAbbox1, FALSE, pAbbox1->AbVertEnclosing);
     }

   /* On detruit la relation de largeur hors-structure */
   if (pTargetBox->BxWOutOfStruct)
      /* Est-ce une dimension elastique ? */
      if (pTargetBox->BxHorizFlex)
	{
	   pOrginBox = pAbbox1->AbWidth.DimPosition.PosAbRef->AbBox;
	   if (pOrginBox != NULL)
	      DelPos (pOrginBox, pTargetBox, NULL, FALSE, FALSE, TRUE);
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
	      DelPos (pOrginBox, pTargetBox, NULL, FALSE, FALSE, FALSE);
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
/* |            horizontale (si horizRef est Vrai) sinon verticale de pOrginBox :| */
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
PtrBox            pOrginBox;
boolean             horizRef;

#endif /* __STDC__ */
{
   PtrAbstractBox             pAb;
   PtrAbstractBox             lepave;
   boolean             boucle;


   pAb = pOrginBox->BxAbstractBox;
   /* Est-ce une relation hors-structure ? */
   if ((horizRef && pOrginBox->BxXOutOfStruct) || (!horizRef && pOrginBox->BxYOutOfStruct))
     {
	/* Si la boite est detruite la procedure RazHorsEnglobe detruit */
	/* automatiquement cette relation                               */
	if (!pAb->AbDead)
	  {
	     /* On remonte a la racine depuis le pave pere */
	     lepave = pAb;
	     /* La relation hors-structure peut etre heritee d'une boite voisine */
	     while (lepave->AbEnclosing != NULL)
		lepave = lepave->AbEnclosing;
	     boucle = TRUE;


	     /* Recherche dans pOrginBox l'ancienne relation de positionnement */
	     while (boucle && lepave != NULL)
	       {
		  if (lepave->AbBox != NULL)
		     boucle = !DelPos (pOrginBox, lepave->AbBox, pAb, TRUE, FALSE, horizRef);
		  lepave = ProchainPave (lepave, pAb);
	       }

	     /* La relation hors-structure est detruite */
	     if (horizRef)
	       {
		  pOrginBox->BxXOutOfStruct = FALSE;

		  /* Des boites voisines ont herite de la relation hors-structure ? */
		  lepave = pAb->AbEnclosing;
		  if (lepave != NULL)
		    {
		       /* On regarde tous les freres */
		       lepave = lepave->AbFirstEnclosed;
		       while (lepave != NULL)
			 {
			    if (lepave != pAb && lepave->AbBox != NULL)
			       /* Si c'est un heritage on retire l'indication hors-structure */
			       if (lepave->AbBox->BxXOutOfStruct
			       && lepave->AbHorizPos.PosAbRef == pAb)
				  lepave->AbBox->BxXOutOfStruct = FALSE;
			       else if (lepave->AbBox->BxWOutOfStruct
					&& lepave->AbWidth.DimIsPosition
			       && lepave->AbWidth.DimPosition.PosAbRef == pAb)
				  lepave->AbBox->BxWOutOfStruct = FALSE;

			    lepave = lepave->AbNext;
			 }
		    }
	       }
	     else
	       {
		  pOrginBox->BxYOutOfStruct = FALSE;

		  /* Des boites voisines ont herite de la relation hors-structure ? */
		  lepave = pAb->AbEnclosing;
		  if (lepave != NULL)
		    {
		       lepave = lepave->AbFirstEnclosed;
		       while (lepave != NULL)
			 {
			    if (lepave != pAb && lepave->AbBox != NULL)
			       /* Si c'est un heritage on retire l'indication hors-structure */
			       if (lepave->AbBox->BxYOutOfStruct
			       && lepave->AbVertPos.PosAbRef == pAb)
				  lepave->AbBox->BxYOutOfStruct = FALSE;
			       else if (lepave->AbBox->BxHOutOfStruct
					&& lepave->AbHeight.DimIsPosition
			       && lepave->AbHeight.DimPosition.PosAbRef == pAb)
				  lepave->AbBox->BxHOutOfStruct = FALSE;

			    lepave = lepave->AbNext;
			 }

		    }
	       }
	  }
     }
   /* Est-ce une relation avec une boite voisine ? */
   else
     {
	lepave = pAb->AbEnclosing;
	boucle = TRUE;

	/* Recherche dans la boite pOrginBox l'ancienne relation de positionnement */
	while (boucle && lepave != NULL)
	  {
	     if (lepave->AbBox != NULL)
		boucle = !DelPos (pOrginBox, lepave->AbBox, pAb, TRUE, FALSE, horizRef);
	     if (lepave == pAb->AbEnclosing)
		lepave = lepave->AbFirstEnclosed;
	     else
		lepave = lepave->AbNext;
	  }			/*while */
     }
}				/* RazPosition */

/* ---------------------------------------------------------------------- */
/* |    RazAxe recherche la boite dont depend l'axe de reference        | */
/* |            horizontal (si horizRef est Vrai) sinon vertical de pOrginBox :  | */
/* |            - Il peut dependre d'elle meme (une relation chez elle).| */
/* |            - Il peut dependre d'une englobee (une relation chez    | */
/* |            l'englobee).                                            | */
/* |            - Il peut dependre d'une voisine (une relation chez la  | */
/* |            voisine).                                               | */
/* |            Si cette dependance existe encore, on detruit la        | */
/* |            relation.                                               | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                RazAxe (PtrBox pOrginBox, boolean horizRef)
#else  /* __STDC__ */
void                RazAxe (pOrginBox, horizRef)
PtrBox            pOrginBox;
boolean             horizRef;

#endif /* __STDC__ */
{
   boolean             boucle;
   PtrAbstractBox             pAb;
   PtrAbstractBox             lepave;


   lepave = pOrginBox->BxAbstractBox;
   boucle = TRUE;

   /* On recherche dans la descendance la dependance de l'axe de reference */
   pAb = lepave;
   while (boucle && pAb != NULL)
     {
	if (pAb->AbBox != NULL)
	   boucle = !DelPos (pAb->AbBox, pOrginBox, NULL, FALSE, TRUE, horizRef);
	if (pAb == lepave)
	   pAb = pAb->AbFirstEnclosed;
	else
	   pAb = pAb->AbNext;

     }

   /* On recherche chez les voisines la dependance de l'axe de reference */
   pAb = lepave->AbEnclosing;
   if (pAb != NULL)
     {
	pAb = pAb->AbFirstEnclosed;
	while (boucle && pAb != NULL)
	  {
	     if (pAb != lepave && pAb->AbBox != NULL)
		boucle = !DelPos (pAb->AbBox, pOrginBox, NULL, FALSE, TRUE, horizRef);
	     pAb = pAb->AbNext;
	  }
     }
}				/* RazAxe */

/* ---------------------------------------------------------------------- */
/* |    RazLiens detruit toutes les relations avec la boite pTargetBox chez   | */
/* |            ses voisines, son englobante et les relations hors      | */
/* |            hierarchie :                                            | */
/* |            -> la relation OpVertRef verticale.                        | */
/* |            -> la relation OpHorizRef horizontale.                      | */
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
   PtrAbstractBox             lepave;


   lepave = pTargetBox->BxAbstractBox;
   pAb = lepave->AbEnclosing;
   if (pAb != NULL)
     {
	/* On detruit les liens d'axe qui la concerne chez l'englobante */
	DelPos (pAb->AbBox, pTargetBox, NULL, FALSE, TRUE, FALSE);	/*en Y */
	DelPos (pAb->AbBox, pTargetBox, NULL, FALSE, TRUE, TRUE);	/*en X */

	/* On detruit les liens qui la concerne chez toutes ses voisines */
	pAb = pAb->AbFirstEnclosed;
	while (pAb != NULL)
	  {
	     if (pAb->AbBox != NULL && pAb != lepave)
	       {
		  /* En X : enleve la regle de position d'axe ou de dimension */
		  DelPos (pAb->AbBox, pTargetBox, NULL, TRUE, FALSE, FALSE);
		  DelPos (pAb->AbBox, pTargetBox, NULL, FALSE, TRUE, FALSE);
		  DelPos (pAb->AbBox, pTargetBox, NULL, FALSE, FALSE, FALSE);

		  /* En Y : enleve la regle de position d'axe ou de dimension */
		  DelPos (pAb->AbBox, pTargetBox, NULL, TRUE, FALSE, TRUE);
		  DelPos (pAb->AbBox, pTargetBox, NULL, FALSE, TRUE, TRUE);
		  DelPos (pAb->AbBox, pTargetBox, NULL, FALSE, FALSE, TRUE);
	       }
	     pAb = pAb->AbNext;
	  }
	/* On detruit eventuellement les liens non hierarchiques */
	RazHorsEnglobe (pTargetBox);
     }
}


/* ---------------------------------------------------------------------- */
/* |    RazDim recherche la boite dont depend la dimension horizontale  | */
/* |            ou verticale de pOrginBox :                                 | */
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
   boolean             boucle;
   PtrAbstractBox             pAb;
   PtrAbstractBox             lepave;


   pAb = pOrginBox->BxAbstractBox;
   lepave = pAb->AbEnclosing;
   boucle = FALSE;

   /* Est-ce une relation hors-structure en X ? */
   if ((horizRef && pOrginBox->BxWOutOfStruct) || (!horizRef && pOrginBox->BxHOutOfStruct))
     {
	/* Si la boite est detruite la procedure RazHorsEnglobe detruit */
	/* automatiquement cette relation                               */
	if (!pAb->AbDead)
	  {
	     /* On remonte a la racine depuis le pave pere */
	     lepave = pAb->AbEnclosing;
	     /* La relation hors-structure peut etre heritee d'une boite voisine */
	     if (lepave != NULL)
		if (lepave != NULL)
		   while (lepave->AbEnclosing != NULL)
		      lepave = lepave->AbEnclosing;


	     /* Recherche dans toute l'arborecence la relation inverse */
	     boucle = TRUE;
	     if (horizRef)
	       {
		  /* La dimension est elastique en X ? */
		  if (pOrginBox->BxHorizFlex)
		    {
		       while (boucle && lepave != NULL)
			 {
			    if (lepave->AbBox != NULL)
			       boucle = !DelPos (lepave->AbBox, pOrginBox, NULL, FALSE, FALSE, horizRef);
			    if (lepave != NULL)
			       lepave = ProchainPave (lepave, pAb);
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
		     while (boucle && lepave != NULL)
		       {
			  if (lepave->AbBox != NULL)
			     boucle = !DelDim (lepave->AbBox, pOrginBox, horizRef);
			  if (lepave != NULL)
			     lepave = ProchainPave (lepave, pAb);
		       }

		  /* La relation hors-structure est detruite */
		  pOrginBox->BxWOutOfStruct = FALSE;
	       }
	     else
	       {
		  /* La dimension est elastique en Y ? */
		  if (pOrginBox->BxVertFlex)
		    {
		       while (boucle && lepave != NULL)
			 {
			    if (lepave->AbBox != NULL)
			       boucle = !DelPos (lepave->AbBox, pOrginBox, NULL, FALSE, FALSE, horizRef);
			    if (lepave != NULL)
			       lepave = ProchainPave (lepave, pAb);
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
		     while (boucle && lepave != NULL)
		       {
			  if (lepave->AbBox != NULL)
			     boucle = !DelDim (lepave->AbBox, pOrginBox, horizRef);
			  if (lepave != NULL)
			     lepave = ProchainPave (lepave, pAb);
		       }

		  /* La relation hors-structure est detruite */
		  pOrginBox->BxHOutOfStruct = FALSE;
	       }

	  }
     }

   else
     {
	/* Est-ce que la dimension depend de l'englobante ? */
	if (lepave != NULL)
	  {
	     boucle = !DelDim (lepave->AbBox, pOrginBox, horizRef);
	     if (boucle)
		lepave = lepave->AbFirstEnclosed;
	  }

	/* Est-ce que la dimension de la boite depend d'une voisine ? */
	while (boucle && lepave != NULL)
	  {
	     if (lepave->AbBox != NULL && lepave != pAb)
	       {
		  boucle = !DelDim (lepave->AbBox, pOrginBox, horizRef);
		  if (boucle)
		     lepave = lepave->AbNext;
	       }
	     else
		lepave = lepave->AbNext;
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

