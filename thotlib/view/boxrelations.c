
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

/*
   rel.c : gestion des Relations entre boites
   France Logiciel numero de depot 88-39-001-00
   Major changes:
   I. Vatton - Aout 86
   IV : Aout 92 dimensions minimales
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

#include "memory.f"
#include "appli.f"
#include "dep.f"
#include "imabs.f"
#include "font.f"



/* ---------------------------------------------------------------------- */
/* |    RelPos etablit les liens de dependance entre les deux reperes   | */
/* |            des boites :                                            | */
/* |            - double sens dans le cas du positionnement entre deux  | */
/* |            soeurs.                                                 | */
/* |            - sens unique dans le cas du positionnement entre fille | */
/* |            et mere.                                                | */
/* |            - sens repere vers axe pour la definition des axes de   | */
/* |            reference (un deplacement du repere de abox modifie     | */
/* |            l'axe de reference de debox).                           | */
/* |            - sens inverse dans le cas d'une dimension elastique    | */
/* |            (un deplacement du repere de abox modifie la dimension  | */
/* |            de debox).                                              | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         RelPos (PtrBox debox, PtrBox abox, OpRelation op, BoxEdge rep1, BoxEdge rep2)
#else  /* __STDC__ */
static void         RelPos (debox, abox, op, rep1, rep2)
PtrBox            debox;
PtrBox            abox;
OpRelation          op;
BoxEdge         rep1;
BoxEdge         rep2;

#endif /* __STDC__ */
{
   int                 i;
   boolean             boucle;
   boolean             nul;
   PtrPosRelations      cepos;
   PtrPosRelations      adpos;
   PtrPosRelations      pTa1;
   BoxRelation           *pRe1;

   i = 0;
   /* PcFirst sens de la dependance dans les positionnements entre soeurs */
   if (op != OpHorizRef && op != OpVertRef && op != OpWidth && op != OpHeight)
     {
	/* On recherche une entree libre */
	adpos = debox->BxPosRelations;
	cepos = NULL;
	boucle = True;
	while (boucle && adpos != NULL)
	  {
	     pTa1 = adpos;
	     i = 0;
	     cepos = adpos;
	     do
	       {
		  i++;
		  nul = pTa1->PosRTable[i - 1].ReBox == NULL;
	       }
	     while (!(i == MAX_RELAT_POS || nul));

	     if (nul)
		boucle = False;
	     else
		adpos = pTa1->PosRNext;
	     /* Bloc suivant */
	  }

	/* Faut-il creer un nouveau bloc de relations ? */
	if (boucle)
	  {
	     GetBPos (&adpos);
	     if (cepos == NULL)
		debox->BxPosRelations = adpos;
	     else
		cepos->PosRNext = adpos;
	     i = 1;
	  }
	pRe1 = &adpos->PosRTable[i - 1];
	pRe1->ReRefEdge = rep1;
	pRe1->ReBox = abox;
	pRe1->ReOp = op;
     }

   /* Deuxieme sens de la dependance */
   if (op != OpHorizInc && op != OpVertInc)
     {
	/* On recherche une entree libre */
	adpos = abox->BxPosRelations;
	cepos = NULL;
	boucle = True;
	while (boucle && adpos != NULL)
	  {
	     pTa1 = adpos;
	     i = 0;
	     cepos = adpos;
	     do
	       {
		  i++;
		  nul = pTa1->PosRTable[i - 1].ReBox == NULL;
	       }
	     while (!(i == MAX_RELAT_POS || nul));

	     if (nul)
		boucle = False;
	     else
		adpos = pTa1->PosRNext;	/* Bloc suivant */
	  }

	/* Faut-il creer un nouveau bloc de relations ? */
	if (boucle)
	  {
	     GetBPos (&adpos);
	     if (cepos == NULL)
		abox->BxPosRelations = adpos;
	     else
		cepos->PosRNext = adpos;
	     i = 1;
	  }
	pRe1 = &adpos->PosRTable[i - 1];
	pRe1->ReRefEdge = rep2;
	pRe1->ReBox = debox;
	pRe1->ReOp = op;
     }
}				/* RelPos */

/* ---------------------------------------------------------------------- */
/* |    RelDim etablit le lien de dependance entre les dimensions       | */
/* |            horizontales ou verticales des deux boites (debox vers  | */
/* |            abox).                                                  | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         RelDim (PtrBox debox, PtrBox abox, boolean MemeDim, boolean EnX)
#else  /* __STDC__ */
static void         RelDim (debox, abox, MemeDim, EnX)
PtrBox            debox;
PtrBox            abox;
boolean             MemeDim;
boolean             EnX;

#endif /* __STDC__ */
{
   int                 i;
   boolean             boucle;
   boolean             nul;
   PtrDimRelations      cedim;
   PtrDimRelations      addim;
   PtrDimRelations      pTa1;


   i = 0;
   /* On determine la dimension affectee */
   if (EnX)
      if (MemeDim)
	 addim = debox->BxWidthRelations;
      else
	 addim = debox->BxHeightRelations;
   else if (MemeDim)
      addim = debox->BxHeightRelations;
   else
      addim = debox->BxWidthRelations;

   /* On recherche une entree libre */
   cedim = NULL;
   boucle = True;
   while (boucle && addim != NULL)
     {
	pTa1 = addim;
	i = 0;
	cedim = addim;
	do
	  {
	     i++;
	     nul = pTa1->DimRTable[i - 1] == NULL;
	  }
	while (!(i == MAX_RELAT_DIM || nul));

	if (nul)
	   boucle = False;
	else
	   addim = pTa1->DimRNext;
     }

   /* Faut-il creer un nouveau bloc de relations ? */
   if (boucle)
     {
	GetBDim (&addim);
	if (cedim == NULL)
	   if (EnX)
	      debox->BxWidthRelations = addim;
	   else
	      debox->BxHeightRelations = addim;
	else
	   cedim->DimRNext = addim;
	i = 1;
     }

   addim->DimRTable[i - 1] = abox;
   addim->DimRSame[i - 1] = MemeDim;
}				/* RelDim */

/* ---------------------------------------------------------------------- */
/* |    PavDePos retourne le pointeur sur le pave de reference pour le  | */
/* |            positionnement implicite, horizontal ou vertical,       | */
/* |            du pave lepave.                                         | */
/* |            Si la valeur rendue est NULL, le pave se positionne par | */
/* |            rapport au pave englobant.                              | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static PtrAbstractBox      PavDePos (PtrAbstractBox lepave, boolean EnX)

#else  /* __STDC__ */
static PtrAbstractBox      PavDePos (lepave, EnX)
PtrAbstractBox             lepave;
boolean             EnX;

#endif /* __STDC__ */

{
   boolean             encore;
   PtrAbstractBox             adpave;

   encore = True;
   adpave = lepave->AbPrevious;
   while (encore)
      if (adpave == NULL)
	 encore = False;
      else if (adpave->AbDead)
	 adpave = adpave->AbPrevious;
      else if (EnX)
	 if (adpave->AbHorizPos.PosAbRef == NULL)
	    encore = False;
	 else
	    adpave = adpave->AbPrevious;
      else if (adpave->AbVertPos.PosAbRef == NULL)
	 encore = False;
      else
	 adpave = adpave->AbPrevious;

   return adpave;
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
   PtrAbstractBox             adpave;
   PtrBox            ibox;

   /* Recherche un pave frere qui depend ce celui-ci */
   if (lepave->AbEnclosing == NULL)
      return;
   else
      adpave = lepave->AbEnclosing->AbFirstEnclosed;

   while (adpave != NULL)
     {
	ibox = adpave->AbBox;
	if (adpave == lepave || ibox == NULL || adpave->AbDead)
	   ;			/* inutile de traiter ce pave */
	else if (ibox->BxXOutOfStruct == indicateur)
	   ;			/* inutile de traiter ce pave */
	else if (adpave->AbHorizPos.PosAbRef == lepave && !adpave->AbHorizPosChange)
	  {
	     /* Dependance de position */
	     ibox->BxXOutOfStruct = indicateur;
	     /* Propage aussi le non englobement */
	     if (adpave->AbEnclosing == lepave->AbEnclosing)
		/* une boite soeur positionnee par rapport a une boite */
		/* elastique non englobee n'est pas englobee elle-meme */
		adpave->AbHorizEnclosing = englobe;
	     PropageXHorsStruct (adpave, indicateur, adpave->AbHorizEnclosing);
	  }
	else if (adpave->AbWidth.DimIsPosition
	&& adpave->AbWidth.DimPosition.PosAbRef == lepave && !adpave->AbWidthChange)
	   /* Dependance de dimension */
	   ibox->BxWOutOfStruct = indicateur;

	/* passe au pave suivant */
	adpave = adpave->AbNext;
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
   PtrAbstractBox             adpave;
   PtrBox            ibox;

   /* Recherche un pave frere qui depend ce celui-ci */
   if (lepave->AbEnclosing == NULL)
      return;
   else
      adpave = lepave->AbEnclosing->AbFirstEnclosed;

   while (adpave != NULL)
     {
	ibox = adpave->AbBox;
	if (adpave == lepave || ibox == NULL || adpave->AbDead)
	   ;			/* inutile de traiter ce pave */
	else if (ibox->BxYOutOfStruct == indicateur)
	   ;			/* inutile de traiter ce pave */
	else if (adpave->AbVertPos.PosAbRef == lepave && !adpave->AbVertPosChange)
	  {
	     ibox->BxYOutOfStruct = indicateur;
	     /* Propage aussi le non englobement */
	     if (adpave->AbEnclosing == lepave->AbEnclosing)
		/* une boite soeur positionnee par rapport a une boite */
		/* elastique non englobee n'est pas englobee elle-meme */
		adpave->AbVertEnclosing = englobe;
	     PropageYHorsStruct (adpave, indicateur, adpave->AbVertEnclosing);
	  }
	else if (adpave->AbHeight.DimIsPosition
	&& adpave->AbHeight.DimPosition.PosAbRef == lepave && !adpave->AbHeightChange)
	   ibox->BxHOutOfStruct = indicateur;

	/* passe au pave suivant */
	adpave = adpave->AbNext;
     }
}


/* ---------------------------------------------------------------------- */
/* |    Positionner applique la regle de positionnement, horizontale ou | */
/* |            verticale selon EnX sur la boite d'indice ibox.         | */
/* |            Les origines de la boite BxXOrg ou BxYOrg sont mises a` | */
/* |            jour et les relations de dependance sont enregistrees.  | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                Positionner (AbPosition regle, PtrBox ibox, int frame, boolean EnX)
#else  /* __STDC__ */
void                Positionner (regle, ibox, frame, EnX)
AbPosition         regle;
PtrBox            ibox;
int                 frame;
boolean             EnX;

#endif /* __STDC__ */
{
   int                 x, y, dist;
   PtrAbstractBox             adpave;
   PtrAbstractBox             lepave;
   PtrBox            dbox;
   BoxEdge         rd, rl;
   OpRelation          op;
   AbPosition        *pPavP1;

   /* On calcule la position de reference */
   op = (OpRelation) 0;
   rd = (BoxEdge) 0;
   DimFenetre (frame, &x, &y);
   dbox = NULL;
   adpave = regle.PosAbRef;
   lepave = ibox->BxAbstractBox;
   /* Verifie que la position ne depend pas d'un pave mort */
   if (adpave != NULL && adpave->AbDead)
     {
	printf ("Position sur un pave mort");
	adpave = NULL;
     }

   if (EnX)
     {
	/* On verifie que la boite ne se place pas par rapport a elle-meme */
	if (adpave == lepave)
	  {
	     lepave->AbHorizPos.PosAbRef = NULL;
	     if (lepave->AbWidth.DimIsPosition)
	       {
		  lepave->AbWidth.DimIsPosition = False;
		  lepave->AbWidth.DimUserSpecified = False;
	       }
	     adpave = NULL;	/* Il ne faut pas interpreter cette regle */
	  }

	/* SRule par defaut */
	if (adpave == NULL)
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
		  adpave = PavDePos (lepave, EnX);
		  /* Si oui -> A droite de sa boite */
		  if (adpave != NULL && regle.PosUnit != UnPercent)
		    {
		       dbox = adpave->AbBox;
		       dist = 0;	/* A droite de la precedente */
		       rd = Right;
		       rl = Left;
		       op = OpHorizDep;
		    }
		  else
		    {
		       /* Initialise sur l'origine de l'englobante (flottante) */
		       adpave = lepave->AbEnclosing;
		       dbox = adpave->AbBox;
		       if (regle.PosUnit == UnPercent)
			  /* poucentage de la largeur de l'englobant */
			  dist = PixelValue (regle.PosDistance, UnPercent, (PtrAbstractBox) lepave->AbEnclosing);
		       else
			  dist = 0;
		       rd = Left;
		       rl = Left;
		       if (ibox->BxHorizFlex)
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
	     if (lepave->AbEnclosing == adpave && !ibox->BxHorizFlex)
		op = OpHorizInc;
	     else
	       {
		  op = OpHorizDep;
		  ibox->BxXToCompute = False;	/* nouvelle regle de placement */
		  if (adpave->AbEnclosing != lepave->AbEnclosing)
		    {
		       /* La boite est liee a une boite hors-structure */
		       if (!XEnAbsolu (ibox))
			  /* la boite  devient maintenant placee en absolu */
			  ibox->BxXToCompute = True;
		       ibox->BxXOutOfStruct = True;
		       PropageXHorsStruct (lepave, True, lepave->AbHorizEnclosing);
		    }
		  else if (adpave->AbBox != NULL)
		    {
		       if (adpave->AbBox->BxXOutOfStruct
			   || (adpave->AbBox->BxHorizFlex)
			   || (adpave->AbBox->BxWOutOfStruct && rd != Left))
			 {
			    /* La boite herite la relation hors-structure */
			    /* ou bien elle est liee a une dimension hors-structure */
			    if (!XEnAbsolu (ibox))
			       /* la boite  devient maintenant placee en absolu */
			       ibox->BxXToCompute = True;
			    ibox->BxXOutOfStruct = True;
			    PropageXHorsStruct (lepave, True, lepave->AbHorizEnclosing);
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
	if (adpave == lepave)
	  {
	     lepave->AbVertPos.PosAbRef = NULL;
	     if (lepave->AbHeight.DimIsPosition)
	       {
		  lepave->AbHeight.DimIsPosition = False;
		  lepave->AbHeight.DimUserSpecified = False;
	       }
	     adpave = NULL;	/* Il ne faut pas interpreter cette regle */
	  }

	/* SRule par defaut */
	if (adpave == NULL)
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
		  adpave = PavDePos (lepave, EnX);
		  /* Si oui -> A droite de sa boite */
		  if (adpave != NULL)
		    {
		       dbox = adpave->AbBox;
		       dist = 0;	/* A droite de la precedente */
		       rd = HorizRef;
		       rl = HorizRef;
		       op = OpVertDep;
		    }
		  else
		    {
		       /* Initialise sur l'origine de l'englobante (flottante) */
		       adpave = lepave->AbEnclosing;
		       dbox = adpave->AbBox;
		       dist = 0;
		       rd = Top;
		       rl = Top;
		       if (ibox->BxVertFlex)
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
	     if (lepave->AbEnclosing == adpave && !ibox->BxVertFlex)
		op = OpVertInc;
	     else
	       {
		  op = OpVertDep;
		  ibox->BxYToCompute = False;	/* nouvelle regle de placement */
		  if (adpave->AbEnclosing != lepave->AbEnclosing)
		    {
		       /* La boite est liee a une boite hors-structure */
		       if (!YEnAbsolu (ibox))
			  /* la boite  est maintenant placee en absolu */
			  ibox->BxYToCompute = True;
		       ibox->BxYOutOfStruct = True;
		       PropageYHorsStruct (lepave, True, lepave->AbVertEnclosing);
		    }
		  else if (adpave->AbBox != NULL)
		    {
		       if (adpave->AbBox->BxYOutOfStruct
			   || (adpave->AbBox->BxHorizFlex && adpave->AbLeafType == LtCompound && adpave->AbInLine && rd != Top)
			   || (adpave->AbBox->BxHOutOfStruct && rd != Top))
			 {
			    /* La boite herite la relation hors-structure */
			    /* ou bien elle est liee a une dimension hors-structure */
			    if (!YEnAbsolu (ibox))
			       /* la boite  est maintenant placee en absolu */
			       ibox->BxYToCompute = True;
			    ibox->BxYOutOfStruct = True;
			    PropageYHorsStruct (lepave, True, lepave->AbVertEnclosing);
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
   if (adpave != NULL)
     {
	dbox = adpave->AbBox;
	if (dbox == NULL)
	  {
	     /* On doit resoudre une reference en avant */
	     dbox = GetBox (adpave);
	     if (dbox != NULL)
		adpave->AbBox = dbox;
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
	       ibox->BxHorizEdge = Left;
	       break;
	    case Right:
	       x -= ibox->BxWidth;
	       ibox->BxHorizEdge = Right;
	       break;
	    case VertMiddle:
	       x -= ibox->BxWidth / 2;
	       ibox->BxHorizEdge = VertMiddle;
	       break;
	    case VertRef:
	       x -= ibox->BxVertRef;
	       pPavP1 = &ibox->BxAbstractBox->AbVertRef;
	       if (pPavP1->PosAbRef == ibox->BxAbstractBox)
		  if (pPavP1->PosRefEdge == VertMiddle)
		     ibox->BxHorizEdge = VertMiddle;
		  else if (pPavP1->PosRefEdge == Right)
		     ibox->BxHorizEdge = Right;
		  else
		     ibox->BxHorizEdge = Left;
	       else
		  ibox->BxHorizEdge = VertRef;
	       break;
	    case Top:
	       ibox->BxVertEdge = Top;
	       break;
	    case Bottom:
	       y -= ibox->BxHeight;
	       ibox->BxVertEdge = Bottom;
	       break;
	    case HorizMiddle:
	       y -= ibox->BxHeight / 2;
	       ibox->BxVertEdge = HorizMiddle;
	       break;
	    case HorizRef:
	       y -= ibox->BxHorizRef;
	       pPavP1 = &ibox->BxAbstractBox->AbHorizRef;
	       if (pPavP1->PosAbRef == ibox->BxAbstractBox)
		  if (pPavP1->PosRefEdge == HorizMiddle)
		     ibox->BxVertEdge = HorizMiddle;
		  else if (pPavP1->PosRefEdge == Bottom)
		     ibox->BxVertEdge = Bottom;
		  else
		     ibox->BxVertEdge = Top;
	       else
		  ibox->BxVertEdge = HorizRef;
	       break;
	    default:
	       if (EnX)
		  ibox->BxHorizEdge = Left;
	       else
		  ibox->BxVertEdge = Top;
	       break;
	 }

   /* On regarde si la position depend d'une boite invisible */
   if (ibox->BxAbstractBox->AbVisibility < FntrTable[frame - 1].FrVisibility)
      dist = 0;
   else if (adpave != NULL)
      if (adpave->AbVisibility < FntrTable[frame - 1].FrVisibility)
	 dist = 0;

   /* Met a jour l'origine de la boite suivant la relation indiquee */
   if (EnX)
     {
	if (!ibox->BxHorizFlex)
	  {
	     x = x + dist - ibox->BxXOrg;
	     if (x == 0 && ibox->BxXToCompute)
		/* Force le placement des boites filles */
		DepXContenu (ibox, x, frame);
	     else
		DepOrgX (ibox, NULL, x, frame);
	  }
	lepave->AbHorizPosChange = False;	/* la regle de position est interpretee */
     }
   else
     {
	if (!ibox->BxVertFlex)
	  {
	     y = y + dist - ibox->BxYOrg;
	     if (y == 0 && ibox->BxYToCompute)
		/* Force le placement des boites filles */
		DepYContenu (ibox, y, frame);
	     else
		DepOrgY (ibox, NULL, y, frame);
	  }
	lepave->AbVertPosChange = False;	/* la regle de position est interpretee */
     }

   /* Il faut mettre a jour les dependances des boites */
   if (adpave != NULL && dbox != NULL)
     {
	RelPos (ibox, dbox, op, rl, rd);
	dbox->BxMoved = NULL;

	if (EnX && ibox->BxHorizFlex)
	   ChngBElast (ibox, dbox, op, x + dist - ibox->BxXOrg, frame, True);
	else if (!EnX && ibox->BxVertFlex)
	   ChngBElast (ibox, dbox, op, y + dist - ibox->BxYOrg, frame, False);
     }
}				/* Positionner */

/* ---------------------------------------------------------------------- */
/* |    Dimensionner applique la regle de dimensionnement, horizontale  | */
/* |            ou verticale selon EnX du pave adpave.                  | */
/* |            Si la dimension depend du contenu de la boite la        | */
/* |            fonction rend la valeur Vrai.                           | */
/* |            Si au contraire, elle depend de celle d'un autre pave,  | */
/* |            elle est mise a` jour et la relation de dependance entre| */
/* |            les boites est enregistree.                             | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
boolean             Dimensionner (PtrAbstractBox adpave, int frame, boolean EnX)
#else  /* __STDC__ */
boolean             Dimensionner (adpave, frame, EnX)
PtrAbstractBox             adpave;
int                 frame;
boolean             EnX;

#endif /* __STDC__ */
{
   int                 val, delta, i;
   PtrBox            dbox;
   PtrAbstractBox             lepave;
   boolean             misenligne;
   boolean             setdim;
   ptrfont             font;
   OpRelation          op;
   AbDimension       *pPavD1;
   AbPosition        *pPavP1;
   PtrAbstractBox             cepave;
   PtrBox            pBo1;

   pBo1 = adpave->AbBox;
   setdim = False;		/* A priori, la dimension ne depend pas de son contenu */

   /* On verifie que la boite est visible */
   if (adpave->AbVisibility >= FntrTable[frame - 1].FrVisibility)
     {
	lepave = adpave->AbEnclosing;
	/* Les cas de coherence sur les boites elastiques */
	/* Les reperes Position et Dimension doivent etre differents */
	/* Ces reperes ne peuvent pas etre l'axe de reference        */
	if (EnX && adpave->AbWidth.DimIsPosition)
	  {
	     if (adpave->AbHorizPos.PosEdge == adpave->AbWidth.DimPosition.PosEdge
		 || adpave->AbHorizPos.PosEdge == VertMiddle
		 || adpave->AbHorizPos.PosEdge == VertRef)
	       {
		  /* Erreur sur le schema de presentation */
		  if (adpave->AbWidth.DimPosition.PosEdge == Left)
		     adpave->AbHorizPos.PosEdge = Right;
		  else if (adpave->AbWidth.DimPosition.PosEdge == Right)
		     adpave->AbHorizPos.PosEdge = Left;
	       }
	     else if (adpave->AbHorizPos.PosAbRef == NULL
		      || adpave->AbWidth.DimPosition.PosEdge == VertRef
		      || adpave->AbWidth.DimPosition.PosAbRef == NULL
		      || adpave->AbWidth.DimPosition.PosAbRef == adpave)
	       {
		  /* Il y a une erreur de dimension */
		  /* Erreur sur le schema de presentation */
		  TtaDisplaySimpleMessageString (LIB, INFO, LIB_HORIZ_SIZING_MISTAKE_SEE_PRES_SCHEM, TypePave (adpave));
		  adpave->AbWidth.DimIsPosition = False;
		  adpave->AbWidth.DimAbRef = NULL;
		  adpave->AbWidth.DimValue = 20;	/* largeur fixe */
		  adpave->AbWidth.DimUnit = UnPoint;
		  adpave->AbWidth.DimUserSpecified = False;
	       }

	     /* verifie que la dimension ne depend pas d'un pave mort */
	     else if (adpave->AbHorizPos.PosAbRef->AbDead)
	       {
		  printf ("Dimension par rapport a un pave mort");
		  adpave->AbWidth.DimIsPosition = False;
		  adpave->AbWidth.DimAbRef = NULL;
		  adpave->AbWidth.DimValue = 20;	/* largeur fixe */
		  adpave->AbWidth.DimUnit = UnPoint;
		  adpave->AbWidth.DimUserSpecified = False;
	       }
	  }
	else if (!EnX && adpave->AbHeight.DimIsPosition)
	  {
	     if (adpave->AbVertPos.PosEdge == adpave->AbHeight.DimPosition.PosEdge
		 || adpave->AbVertPos.PosEdge == HorizMiddle
		 || adpave->AbVertPos.PosEdge == HorizRef)
	       {
		  /* Erreur sur le schema de presentation */
		  if (adpave->AbHeight.DimPosition.PosEdge == Top)
		     adpave->AbVertPos.PosEdge = Bottom;
		  else if (adpave->AbHeight.DimPosition.PosEdge == Bottom)
		     adpave->AbVertPos.PosEdge = Top;
	       }
	     else if (adpave->AbVertPos.PosAbRef == NULL
		      || adpave->AbHeight.DimPosition.PosEdge == HorizRef
		      || adpave->AbHeight.DimPosition.PosAbRef == NULL
		      || adpave->AbHeight.DimPosition.PosAbRef == adpave)
	       {
		  /* Il y a une erreur de dimension */
		  /* Erreur sur le schema de presentation */
		  TtaDisplaySimpleMessageString (LIB, INFO, LIB_VERTIC_SIZING_MISTAKE_SEE_PRES_SCHEM, TypePave (adpave));
		  adpave->AbHeight.DimIsPosition = False;
		  adpave->AbHeight.DimAbRef = NULL;
		  adpave->AbHeight.DimValue = 20;	/* hauteur fixe */
		  adpave->AbHeight.DimUnit = UnPoint;
		  adpave->AbHeight.DimUserSpecified = False;
	       }

	     /* verifie que la dimension ne depend pas d'un pave mort */
	     else if (adpave->AbVertPos.PosAbRef->AbDead)
	       {
		  printf ("Dimension par rapport a un pave mort");
		  adpave->AbHeight.DimIsPosition = False;
		  adpave->AbHeight.DimAbRef = NULL;
		  adpave->AbHeight.DimValue = 20;	/* hauteur fixe */
		  adpave->AbHeight.DimUnit = UnPoint;
		  adpave->AbHeight.DimUserSpecified = False;
	       }
	  }

	/* Est-ce une boite non-elastique ? */
	if ((EnX && !adpave->AbWidth.DimIsPosition) || (!EnX && !adpave->AbHeight.DimIsPosition))
	  {
	     font = pBo1->BxFont;
	     /* Est-ce que la dimension est exprimee en points typo. ? */
	     if (EnX)
		pPavD1 = &adpave->AbWidth;
	     else
		pPavD1 = &adpave->AbHeight;

	     /* verifie que la dimension ne depend pas d'un pave mort */
	     if (pPavD1->DimAbRef != NULL && pPavD1->DimAbRef->AbDead)
	       {
		  printf ("Dimension par rapport a un pave mort");
		  pPavD1->DimAbRef = NULL;
		  pPavD1->DimValue = 0;
	       }

	     /* Est-ce la boite racine ? */
	     if (lepave == NULL)
		/* la largeur est contrainte (par heritage ou imposee) ? */
		if (EnX)
		  {
		     if (pPavD1->DimValue == 0)
			setdim = True;
		     else
		       {
			  DimFenetre (frame, &val, &i);
			  if (pPavD1->DimValue < 0)
			     val += pPavD1->DimValue;
			  else if (pPavD1->DimUnit == UnPercent)
			     val = PixelValue (pPavD1->DimValue, UnPercent, (PtrAbstractBox) val);
			  else
			     val = PixelValue (pPavD1->DimValue, pPavD1->DimUnit, adpave);
			  ModLarg (pBo1, pBo1, NULL, val - pBo1->BxWidth, 0, frame);
		       }
		  }
	     /* la hauteur est contrainte (par heritage ou imposee) ? */
		else
		  {
		     if (pPavD1->DimValue == 0)
			setdim = True;
		     else
		       {
			  DimFenetre (frame, &i, &val);
			  if (pPavD1->DimValue < 0)
			     val = 0;
			  else if (pPavD1->DimUnit == UnPercent)
			     val = PixelValue (pPavD1->DimValue, UnPercent, (PtrAbstractBox) val);
			  else
			     val = PixelValue (pPavD1->DimValue, pPavD1->DimUnit, adpave);
			  ModHaut (pBo1, pBo1, NULL, val - pBo1->BxHeight, frame);
		       }
		  }

	     /* Ce n'est pas la boite racine */
	     else
	       {
		  misenligne = lepave->AbInLine || lepave->AbBox->BxType == BoGhost;
		  /* La largeur est contrainte (par heritage ou imposee) ? */
		  if (EnX)
		     /* PcFirst cas de coherence */
		     /* Le texte mis en ligne DOIT prendre sa taille */
		     if (misenligne && adpave->AbLeafType == LtText)
			setdim = True;
		  /* Dimension fixee */
		     else if (adpave->AbWidth.DimAbRef == NULL)
			if (adpave->AbWidth.DimValue <= 0)
			   setdim = True;	/* A calculer */
			else
			  {
			     /* Convert the distance value */
			     if (pPavD1->DimUnit == UnPercent)
			       {
				  delta = PixelValue (pPavD1->DimValue, UnPercent, (PtrAbstractBox) lepave->AbBox->BxWidth);
				  RelDim (lepave->AbBox, pBo1, pPavD1->DimSameDimension, EnX);
			       }
			     else
				delta = PixelValue (pPavD1->DimValue, pPavD1->DimUnit, adpave);
			     ModLarg (pBo1, pBo1, NULL, delta - pBo1->BxWidth, 0, frame);
			  }
		  /* Deuxieme cas de coherence */
		  /* La boite ne peut pas prendre la taille de son englobante si : */
		  /* -> L'englobante prend la taille de son contenu */
		     else
		       {
			  pPavP1 = &adpave->AbHorizPos;
			  if (adpave->AbWidth.DimAbRef == lepave && lepave->AbEnclosing != NULL
			      && lepave->AbWidth.DimAbRef == NULL && lepave->AbWidth.DimValue <= 0
			  /* -> ET l'englobante est mise en lignes (extensibles) */
			  /* OU la boite n'est pas collee au cote gauche de son englobante */
			      && (misenligne || pPavP1->PosAbRef != lepave
				  || pPavP1->PosRefEdge != Left || pPavP1->PosEdge != Left))
			    {
			       pPavD1 = &adpave->AbWidth;
			       setdim = True;
			       pPavD1->DimAbRef = NULL;
			       pPavD1->DimValue = 0;
			       pPavD1->DimUnit = UnRelative;
			    }
			  /* La dimension ne peut dependre d'elle-meme */
			  else
			    {
			       pPavD1 = &adpave->AbWidth;
			       if (pPavD1->DimAbRef == adpave && pPavD1->DimSameDimension)
				 {
				    setdim = True;
				    pPavD1->DimAbRef = NULL;
				    pPavD1->DimValue = 0;
				    pPavD1->DimUnit = UnRelative;
				 }
			       /* Herite de la dimension d'une autre boite */
			       else
				 {
				    dbox = pPavD1->DimAbRef->AbBox;
				    if (dbox == NULL)
				      {
					 /* On doit resoudre une reference en avant */
					 dbox = GetBox (pPavD1->DimAbRef);
					 if (dbox != NULL)
					    pPavD1->DimAbRef->AbBox = dbox;
				      }

				    if (dbox != NULL)
				      {
					 /* On regarde s'il s'agit de la meme dimension */
					 if (pPavD1->DimSameDimension)
					    val = dbox->BxWidth;
					 else
					    val = dbox->BxHeight;
					 /* Quand la boite prend la largeur de la ligne qui */
					 /* l'englobe ->retire la valeur de l'indentation   */
					 if (pPavD1->DimAbRef == lepave && misenligne && pPavD1->DimSameDimension)
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
					 if (pPavD1->DimUnit == UnPercent)
					    val = PixelValue (pPavD1->DimValue, UnPercent, (PtrAbstractBox) val);
					 else
					    val += PixelValue (pPavD1->DimValue, pPavD1->DimUnit, adpave);
					 ModLarg (pBo1, pBo1, NULL, val - pBo1->BxWidth, 0, frame);
					 /* On teste si la relation est hors structure */
					 if (pPavD1->DimAbRef != lepave
					     && pPavD1->DimAbRef->AbEnclosing != lepave)
					    pBo1->BxWOutOfStruct = True;

					 /* On note les dependances des largeurs des boites */
					 RelDim (pPavD1->DimAbRef->AbBox, pBo1, pPavD1->DimSameDimension, EnX);
				      }
				 }
			    }
		       }
		  /* la hauteur est contrainte (par heritage ou imposee) ? */
		  else
		     /* PcFirst cas de coherence */
		     /* / Le texte mis en ligne DOIT prendre sa taille */
		  if (misenligne && adpave->AbLeafType == LtText)
		     setdim = True;
		  /* Dimension fixee */
		  else if (adpave->AbHeight.DimAbRef == NULL)
		     if (adpave->AbHeight.DimValue == 0)
			setdim = True;	/* A calculer */
		     else
		       {
			  /* Convert the distance value */
			  if (pPavD1->DimUnit == UnPercent)
			     delta = PixelValue (pPavD1->DimValue, UnPercent, (PtrAbstractBox) lepave->AbBox->BxHeight);
			  else
			     delta = PixelValue (pPavD1->DimValue, pPavD1->DimUnit, adpave);
			  ModHaut (pBo1, pBo1, NULL, delta - pBo1->BxHeight, frame);
		       }
		  /* Deuxieme cas de coherence */
		  /* La boite ne peut pas prendre la taille de son englobante si : */
		  /* -> L'englobante est mise en ligne */
		  else if (misenligne && adpave->AbHeight.DimAbRef == lepave
			   && (adpave->AbLeafType == LtPicture || adpave->AbLeafType == LtCompound))
		    {
		       pPavD1 = &adpave->AbHeight;
		       setdim = True;
		       pPavD1->DimAbRef = NULL;
		       pPavD1->DimValue = 0;
		       pPavD1->DimUnit = UnRelative;
		    }
		  /* Troisieme cas de coherence */
		  /* La boite ne peut pas prendre la taille de son englobante si : */
		  /* -> L'englobante prend la taille de son contenu */
		  else
		    {
		       pPavP1 = &adpave->AbVertPos;
		       if (adpave->AbHeight.DimAbRef == lepave
			   && lepave->AbEnclosing != NULL
			   && lepave->AbHeight.DimAbRef == NULL && lepave->AbHeight.DimValue <= 0
		       /* ET la boite n'est pas collee au cote superieur de son englobante */
			   && (pPavP1->PosAbRef != lepave || pPavP1->PosRefEdge != Top
			       || pPavP1->PosEdge != Top))
			 {
			    pPavD1 = &adpave->AbHeight;
			    setdim = True;
			    pPavD1->DimAbRef = NULL;
			    pPavD1->DimValue = 0;
			    pPavD1->DimUnit = UnRelative;
			 }
		       /* La dimension ne peut dependre d'elle-meme */
		       else
			 {
			    pPavD1 = &adpave->AbHeight;
			    if (pPavD1->DimAbRef == adpave && pPavD1->DimSameDimension)
			      {
				 setdim = True;
				 pPavD1->DimAbRef = NULL;
				 pPavD1->DimValue = 0;
				 pPavD1->DimUnit = UnRelative;
			      }
			    /* Herite de la dimension d'une autre boite */
			    else
			      {
				 dbox = pPavD1->DimAbRef->AbBox;
				 if (dbox == NULL)
				   {
				      /* On doit resoudre une reference en avant */
				      dbox = GetBox (pPavD1->DimAbRef);
				      if (dbox != NULL)
					 pPavD1->DimAbRef->AbBox = dbox;
				   }

				 if (dbox != NULL)
				   {
				      /* On regarde s'il s'agit de la meme dimension */
				      if (pPavD1->DimSameDimension)
					 val = dbox->BxHeight;
				      else
					 val = dbox->BxWidth;

				      /* Quand la boite prend la largeur de la ligne qui */
				      /* l'englobe -> retire la valeur de l'indentation  */
				      if (pPavD1->DimAbRef == lepave && misenligne
					  && !pPavD1->DimSameDimension)
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
				      if (pPavD1->DimUnit == UnPercent)
					 val = PixelValue (pPavD1->DimValue, UnPercent, (PtrAbstractBox) val);
				      else
					 val += PixelValue (pPavD1->DimValue, pPavD1->DimUnit, adpave);
				      ModHaut (pBo1, pBo1, NULL, val - pBo1->BxHeight, frame);
				      /* On teste si la relation est hors structure */
				      if (pPavD1->DimAbRef != lepave
					  && pPavD1->DimAbRef->AbEnclosing != lepave)
					 pBo1->BxHOutOfStruct = True;

				      /* On note les dependances des hauteurs des boites */
				      if (pPavD1->DimAbRef == adpave && pPavD1->DimSameDimension)
					{
					   /* Il y a une erreur de dimension */
					   /* Erreur sur le schema de presentation */
					   if (EnX)
					      TtaDisplaySimpleMessageString (LIB, INFO, LIB_HORIZ_SIZING_MISTAKE_SEE_PRES_SCHEM, TypePave (adpave));
					   else
					      TtaDisplaySimpleMessageString (LIB, INFO, LIB_VERTIC_SIZING_MISTAKE_SEE_PRES_SCHEM, TypePave (adpave));

					}
				      RelDim (pPavD1->DimAbRef->AbBox, pBo1, pPavD1->DimSameDimension, EnX);
				   }
			      }
			 }
		    }
	       }
	  }
	/* C'est une boite elastique */
	else if (EnX)
	  {
	     pPavD1 = &adpave->AbWidth;

	     /* Box elastique en X */
	     pPavP1 = &pPavD1->DimPosition;
	     op = OpWidth;
	     /* On teste si la relation est hors structure */
	     if (pPavP1->PosAbRef != lepave
		 && pPavP1->PosAbRef->AbEnclosing != lepave)
		pBo1->BxWOutOfStruct = True;
	     else if (pPavP1->PosAbRef->AbBox != NULL)
		pBo1->BxWOutOfStruct = pPavP1->PosAbRef->AbBox->BxXOutOfStruct;

	     /* Des boites voisines heritent de la relation hors-structure ? */
	     if (lepave != NULL)
	       {
		  cepave = lepave->AbFirstEnclosed;
		  while (cepave != NULL)
		    {
		       if (cepave != adpave && cepave->AbBox != NULL)
			 {
			    /* Si c'est un heritage on note l'indication hors-structure */
			    if (cepave->AbHorizPos.PosAbRef == adpave
			      && cepave->AbHorizPos.PosRefEdge != Left)
			      {
				 if (!XEnAbsolu (cepave->AbBox))
				    /* la boite  est maintenant placee en absolu */
				    cepave->AbBox->BxXToCompute = True;
				 cepave->AbBox->BxXOutOfStruct = True;
				 if (cepave->AbEnclosing == adpave->AbEnclosing)
				    cepave->AbHorizEnclosing = adpave->AbHorizEnclosing;
				 PropageXHorsStruct (cepave, True, cepave->AbHorizEnclosing);
			      }

			    if (cepave->AbVertPos.PosAbRef == adpave
				&& cepave->AbVertPos.PosRefEdge != Top
				&& adpave->AbLeafType == LtCompound
				&& adpave->AbInLine)
			      {
				 if (!YEnAbsolu (cepave->AbBox))
				    /* la boite  est maintenant placee en absolu */
				    cepave->AbBox->BxYToCompute = True;
				 cepave->AbBox->BxYOutOfStruct = True;
				 if (cepave->AbEnclosing == adpave->AbEnclosing)
				    cepave->AbVertEnclosing = adpave->AbVertEnclosing;
				 PropageYHorsStruct (cepave, True, cepave->AbVertEnclosing);
			      }
			 }
		       cepave = cepave->AbNext;
		    }
	       }

	     /* Decalage par rapport a la boite distante */
	     dbox = pPavP1->PosAbRef->AbBox;
	     if (dbox == NULL)
	       {
		  /* On doit resoudre une reference en avant */
		  if (!pPavP1->PosAbRef->AbDead)
		     dbox = GetBox (pPavP1->PosAbRef);
		  if (dbox != NULL)
		     pPavP1->PosAbRef->AbBox = dbox;
	       }

	     if (dbox != NULL)
	       {
		  /* On regarde si la position depend d'une boite invisible */
		  if (pPavP1->PosAbRef->AbVisibility < FntrTable[frame - 1].FrVisibility)
		     delta = 0;
		  else if (pPavP1->PosUnit == UnPercent)
		    {
		       /* Convert the distance value */
		       if (adpave->AbEnclosing == NULL)
			  delta = 0;
		       else
			  delta = PixelValue (pPavP1->PosDistance, pPavP1->PosUnit,
			  (PtrAbstractBox) adpave->AbEnclosing->AbBox->BxWidth);
		    }
		  else
		     /* Convert the distance value */
		     delta = PixelValue (pPavP1->PosDistance, pPavP1->PosUnit, adpave);

		  val = dbox->BxXOrg + delta;
		  switch (pPavP1->PosRefEdge)
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
		  RelPos (pBo1, dbox, op, pPavP1->PosEdge, pPavP1->PosRefEdge);

		  if (!XEnAbsolu (pBo1))
		     /* la boite  devient maintenant placee en absolu */
		     pBo1->BxXToCompute = True;
		  /* La boite est marquee elastique */
		  pBo1->BxHorizFlex = True;
		  dbox->BxMoved = NULL;
		  ChngBElast (pBo1, dbox, op, val, frame, True);
	       }
	  }
	else
	  {
	     pPavD1 = &adpave->AbHeight;
	     /* Box elastique en Y */
	     pPavP1 = &pPavD1->DimPosition;
	     op = OpHeight;

	     /* On teste si la relation est hors structure */
	     if (pPavP1->PosAbRef != lepave
		 && pPavP1->PosAbRef->AbEnclosing != lepave)
		pBo1->BxHOutOfStruct = True;
	     else if (pPavP1->PosAbRef->AbBox != NULL)
		pBo1->BxHOutOfStruct = pPavP1->PosAbRef->AbBox->BxYOutOfStruct;

	     /* Des boites voisines heritent de la relation hors-structure ? */
	     if (lepave != NULL /* && pBo1->BxHOutOfStruct */ )
	       {
		  cepave = lepave->AbFirstEnclosed;
		  while (cepave != NULL)
		    {
		       if (cepave != adpave && cepave->AbBox != NULL)
			  /* Si c'est un heritage on note l'indication hors-structure */
			  if (cepave->AbVertPos.PosAbRef == adpave
			      && cepave->AbVertPos.PosRefEdge != Top)
			    {
			       if (!YEnAbsolu (cepave->AbBox))
				  /* la boite  est maintenant placee en absolu */
				  cepave->AbBox->BxYToCompute = True;
			       cepave->AbBox->BxYOutOfStruct = True;
			       if (cepave->AbEnclosing == adpave->AbEnclosing)
				  cepave->AbVertEnclosing = adpave->AbVertEnclosing;
			       PropageYHorsStruct (cepave, True, cepave->AbVertEnclosing);
			    }
		       cepave = cepave->AbNext;
		    }
	       }

	     /* Decalage par rapport a la boite distante */
	     dbox = pPavP1->PosAbRef->AbBox;
	     if (dbox == NULL)
	       {
		  /* On doit resoudre une reference en avant */
		  dbox = GetBox (pPavP1->PosAbRef);
		  if (dbox != NULL)
		     pPavP1->PosAbRef->AbBox = dbox;
	       }

	     if (dbox != NULL)
	       {
		  /* On regarde si la position depend d'une boite invisible */
		  if (pPavP1->PosAbRef->AbVisibility < FntrTable[frame - 1].FrVisibility)
		     delta = 0;
		  else if (pPavP1->PosUnit == UnPercent)
		    {
		       /* Convert the distance value */
		       if (adpave->AbEnclosing == NULL)
			  delta = 0;
		       else
			  delta = PixelValue (pPavP1->PosDistance, pPavP1->PosUnit,
			  (PtrAbstractBox) adpave->AbEnclosing->AbBox->BxHeight);
		    }
		  else
		     /* Convert the distance value */
		     delta = PixelValue (pPavP1->PosDistance, pPavP1->PosUnit, adpave);

		  val = dbox->BxYOrg + delta;
		  switch (pPavP1->PosRefEdge)
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
		  RelPos (pBo1, dbox, op, pPavP1->PosEdge, pPavP1->PosRefEdge);

		  if (!YEnAbsolu (pBo1))
		     /* la boite  devient maintenant placee en absolu */
		     pBo1->BxYToCompute = True;
		  /* La boite est marquee elastique */
		  pBo1->BxVertFlex = True;
		  dbox->BxMoved = NULL;
		  ChngBElast (pBo1, dbox, op, val, frame, False);
	       }
	  }
     }

   /* La regle de dimension est interpretee */
   if (EnX)
     {
	adpave->AbWidthChange = False;
	/* Marque dans la boite si la dimension depend du contenu ou non */
	pBo1->BxRuleWidth = 0;
	if (setdim)
	   pBo1->BxContentWidth = True;
	else
	   pBo1->BxContentWidth = False;
     }
   else
     {
	adpave->AbHeightChange = False;
	/* Marque dans la boite si la dimension depend du contenu ou non */
	pBo1->BxRuleHeigth = 0;
	if (setdim)
	   pBo1->BxContentHeight = True;
	else
	   pBo1->BxContentHeight = False;
     }

   return setdim;
}				/* Dimensionner */


/* ---------------------------------------------------------------------- */
/* |    PlacerAxe applique la regle de positionnement donnee en         | */
/* |            parametre a` la boite d'indice ibox.                    | */
/* |            L'axe horizontal ou vertical de la boite, selon que EnX | */
/* |            est VRAI ou FAUX est mis a` jour.                       | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                PlacerAxe (AbPosition regle, PtrBox ibox, int frame, boolean EnX)
#else  /* __STDC__ */
void                PlacerAxe (regle, ibox, frame, EnX)
AbPosition         regle;
PtrBox            ibox;
int                 frame;
boolean             EnX;

#endif /* __STDC__ */
{
   int                 x, y, dist;
   PtrBox            dbox;
   PtrAbstractBox             adpave;
   PtrAbstractBox             lepave;
   BoxEdge         rd, rl;

   /* Calcule la position de reference */
   adpave = regle.PosAbRef;
   lepave = ibox->BxAbstractBox;
   /* Verifie que la position ne depend pas d'un pave mort */
   if (adpave != NULL && adpave->AbDead)
     {
	printf ("Position axe sur un  pave mort");
	adpave = NULL;
     }

   /* SRule par defaut */
   if (adpave == NULL)
     {
	dbox = ibox;
	if (EnX)
	  {
	     rd = Left;
	     rl = VertRef;
	     if (regle.PosUnit == UnPercent)
		dist = PixelValue (regle.PosDistance, UnPercent, (PtrAbstractBox) ibox->BxWidth);
	     else
		dist = 0;
	  }
	else
	  {
	     rd = Bottom;
	     rl = HorizRef;
	     if (regle.PosUnit == UnPercent)
		dist = PixelValue (regle.PosDistance, UnPercent, (PtrAbstractBox) ibox->BxHeight);
	     if (lepave->AbLeafType == LtText)
		dist = FontBase (ibox->BxFont) - ibox->BxHeight;
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
	     if (EnX)
		dist = PixelValue (regle.PosDistance, UnPercent, (PtrAbstractBox) lepave->AbEnclosing->AbBox->BxWidth);
	     else
		dist = PixelValue (regle.PosDistance, UnPercent, (PtrAbstractBox) lepave->AbEnclosing->AbBox->BxHeight);
	  }
	else
	   dist = PixelValue (regle.PosDistance, regle.PosUnit, lepave);

	dbox = adpave->AbBox;
	if (dbox == NULL)
	  {
	     /* On doit resoudre une reference en avant */
	     dbox = GetBox (adpave);
	     if (dbox != NULL)
		adpave->AbBox = dbox;
	     else
	       {
		  if (EnX)
		     lepave->AbVertRefChange = False;
		  else
		     lepave->AbHorizRefChange = False;
		  return;	/* plus de boite libre */
	       }
	  }
     }

   /* Deplacement par rapport a la boite distante */
   /* L'axe est place par rapport a la boite elle-meme */
   if (dbox == ibox
       || ibox->BxType == BoGhost
       || (lepave->AbInLine && lepave->AbLeafType == LtCompound))
     {
	x = 0;
	y = 0;
     }
   /* L'axe est place par rapport a une incluse */
   else if (Propage != ToSiblings)
     {
	/* Il faut peut-etre envisager que dbox soit une boite coupee */
	x = dbox->BxXOrg - ibox->BxXOrg;
	y = dbox->BxYOrg - ibox->BxYOrg;
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
   if (EnX)
     {
	x = x + dist - ibox->BxVertRef;
	DepAxe (ibox, NULL, x, frame);
	lepave->AbVertRefChange = False;	/* la regle axe de reference est interpretee */
	if (dbox != NULL)
	   RelPos (ibox, dbox, OpHorizRef, rl, rd);
     }
   else
     {
	y = y + dist - ibox->BxHorizRef;
	DepBase (ibox, NULL, y, frame);
	lepave->AbHorizRefChange = False;	/* la regle axe de reference est interpretee */
	if (dbox != NULL)
	   RelPos (ibox, dbox, OpVertRef, rl, rd);
     }
}				/* PlacerAxe */

/* ---------------------------------------------------------------------- */
/* |    BoiteHInclus recherche la boite qui relie horizontalement ibox  | */
/* |            a` son englobante :                                     | */
/* |            - Si on n'a pas modifie les relations, cette boite est  | */
/* |            memorisee (BxHorizInc).                                  | */
/* |            - Si la boite possede la relation OpHorizInc, c'est      | */
/* |            elle-meme.                                              | */
/* |            - Sinon on regarde la boite soeur dont elle depend.     | */
/* |            Retourne son adresse sinon NULL.                                | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
PtrBox            BoiteHInclus (PtrBox ibox, PtrBox prec)

#else  /* __STDC__ */
PtrBox            BoiteHInclus (ibox, prec)
PtrBox            ibox;
PtrBox            prec;

#endif /* __STDC__ */

{
   PtrBox            box1;
   int                 i;
   PtrPosRelations      adpos;
   boolean             nonnul;
   PtrPosRelations      pTa1;
   BoxRelation           *pRe1;
   PtrBox            Result;

   /* On verifie que la boite n'a pas deja ete examinee */
   if (prec != NULL)
     {
	box1 = prec->BxMoved;
	while (box1 != NULL && box1 != ibox)
	   box1 = box1->BxMoved;
     }
   else
      box1 = NULL;

   if (box1 == NULL)
     {

	/* On met a jour la pile des boites traitees */
	ibox->BxMoved = prec;
	/* On regarde si on connait deja la boite qui la relie a l'englobante */
	if (ibox->BxHorizInc != NULL)
	   box1 = ibox->BxHorizInc;
	else
	   box1 = NULL;

	/* Si la position de la boite depend d'une boite externe on prend la */
	/* boite elle meme comme reference.                                  */
	if (ibox->BxXOutOfStruct)
	   box1 = ibox;

	/* On regarde si la boite est reliee a son englobante */
	adpos = ibox->BxPosRelations;
	while (box1 == NULL && adpos != NULL)
	  {
	     pTa1 = adpos;
	     i = 1;
	     nonnul = pTa1->PosRTable[i - 1].ReBox != NULL;
	     while (i <= MAX_RELAT_POS && nonnul)
		if (pTa1->PosRTable[i - 1].ReOp == OpHorizInc)
		  {
		     box1 = ibox;
		     i = MAX_RELAT_POS + 1;
		     /* On a trouve */
		  }
		else
		  {
		     i++;
		     if (i <= MAX_RELAT_POS)
			nonnul = pTa1->PosRTable[i - 1].ReBox != NULL;
		  }

	     adpos = pTa1->PosRNext;
	     /* Bloc suivant */
	  }

	/* Sinon on recherche la boite soeur qui l'est */
	adpos = ibox->BxPosRelations;
	while (box1 == NULL && adpos != NULL)
	  {
	     pTa1 = adpos;
	     i = 1;
	     nonnul = pTa1->PosRTable[i - 1].ReBox != NULL;
	     while (i <= MAX_RELAT_POS && nonnul)
	       {
		  pRe1 = &pTa1->PosRTable[i - 1];
		  if (pRe1->ReBox->BxAbstractBox != NULL)
		     if (pRe1->ReOp == OpHorizDep
		     /* Si c'est la bonne relation de dependance */
			 && pRe1->ReBox->BxAbstractBox->AbHorizPos.PosAbRef != ibox->BxAbstractBox)
		       {
			  /* Si la position de la boite depend d'une boite elastique */
			  /* on prend la boite elastique comme reference             */
			  if (pRe1->ReBox->BxHorizFlex)
			     box1 = pRe1->ReBox;
			  else
			     box1 = BoiteHInclus (pRe1->ReBox, ibox);


			  /* Est-ce que l'on a trouve la boite qui donne la position ? */
			  if (box1 != NULL)
			    {
			       i = MAX_RELAT_POS + 1;
			       /* La position depend d'une relation hors-structure ? */
			       if (box1->BxXOutOfStruct)
				  ibox->BxXOutOfStruct = True;
			    }
			  else
			    {
			       i++;
			       if (i <= MAX_RELAT_POS)
				  nonnul = pTa1->PosRTable[i - 1].ReBox != NULL;
			    }
		       }
		     else
		       {
			  i++;
			  if (i <= MAX_RELAT_POS)
			     nonnul = pTa1->PosRTable[i - 1].ReBox != NULL;
		       }
	       }
	     adpos = pTa1->PosRNext;	/* Bloc suivant */
	  }

	ibox->BxHorizInc = box1;
	Result = box1;
     }
   else
      Result = NULL;
   return Result;
}				/* BoiteHInclus */

/* ---------------------------------------------------------------------- */
/* |    BoiteVInclus recherche la boite qui relie verticalement ibox    | */
/* |            a` son englobante :                                     | */
/* |            - Si on n'a pas modifie les relations, cette boite est  | */
/* |            memorisee (BxVertInc).                                  | */
/* |            - Si la boite possede la relation OpVertInc, c'est      | */
/* |            elle-meme.                                              | */
/* |            - Sinon on regarde la boite soeur dont elle depend.     | */
/* |            Retourne son adresse sinon NULL.                        | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
PtrBox            BoiteVInclus (PtrBox ibox, PtrBox prec)
#else  /* __STDC__ */
PtrBox            BoiteVInclus (ibox, prec)
PtrBox            ibox;
PtrBox            prec;

#endif /* __STDC__ */
{
   PtrBox            box1;
   int                 i;
   PtrPosRelations      adpos;
   boolean             nonnul;
   PtrPosRelations      pTa1;
   BoxRelation           *pRe1;
   PtrBox            result;

   /* On verifie que la boite n'a pas deja ete examinee */
   if (prec != NULL)
     {
	box1 = prec->BxMoved;
	while (box1 != NULL && box1 != ibox)
	   box1 = box1->BxMoved;
     }
   else
      box1 = NULL;

   if (box1 == NULL)
     {

	/* On met a jour la pile des boites traitees */
	ibox->BxMoved = prec;

	/* On regarde si on connait deja la boite qui la relie a l'englobante */
	if (ibox->BxVertInc != NULL)
	   box1 = ibox->BxVertInc;
	else
	   box1 = NULL;

	/* Si la position de la boite depend d'une boite externe on prend la */
	/* boite elle meme comme reference.                                  */
	if (ibox->BxYOutOfStruct)
	   box1 = ibox;

	/* On regarde si la boite est reliee a son englobante */
	adpos = ibox->BxPosRelations;
	while (box1 == NULL && adpos != NULL)
	  {
	     pTa1 = adpos;
	     i = 1;
	     nonnul = pTa1->PosRTable[i - 1].ReBox != NULL;
	     while (i <= MAX_RELAT_POS && nonnul)
		if (pTa1->PosRTable[i - 1].ReOp == OpVertInc)
		  {
		     box1 = ibox;
		     i = MAX_RELAT_POS + 1;	/* On a trouve */
		  }
		else
		  {
		     i++;
		     if (i <= MAX_RELAT_POS)
			nonnul = pTa1->PosRTable[i - 1].ReBox != NULL;
		  }
	     adpos = pTa1->PosRNext;	/* Bloc suivant */

	  }

	/* Sinon on recherche la boite soeur qui l'est */
	adpos = ibox->BxPosRelations;
	while (box1 == NULL && adpos != NULL)
	  {
	     pTa1 = adpos;
	     i = 1;
	     nonnul = pTa1->PosRTable[i - 1].ReBox != NULL;
	     while (i <= MAX_RELAT_POS && nonnul)
	       {
		  pRe1 = &pTa1->PosRTable[i - 1];
		  if (pRe1->ReBox->BxAbstractBox != NULL)
		     if (pRe1->ReOp == OpVertDep
		     /* Si c'est la bonne relation de dependance */
			 && pRe1->ReBox->BxAbstractBox->AbVertPos.PosAbRef != ibox->BxAbstractBox)
		       {

			  /* Si la position de la boite depend d'une boite elastique */
			  /* on prend la boite elastique comme reference             */
			  if (pRe1->ReBox->BxVertFlex)
			     box1 = pRe1->ReBox;
			  else
			     box1 = BoiteVInclus (pRe1->ReBox, ibox);
			  /* Est-ce que l'on a trouve la boite qui donne la position ? */
			  if (box1 != NULL)
			    {
			       i = MAX_RELAT_POS + 1;
			       /* La position depend d'une relation hors-structure ? */
			       if (box1->BxYOutOfStruct)
				  ibox->BxYOutOfStruct = True;
			    }
			  else
			    {
			       i++;
			       if (i <= MAX_RELAT_POS)
				  nonnul = pTa1->PosRTable[i - 1].ReBox != NULL;
			    }
		       }
		     else
		       {
			  i++;
			  if (i <= MAX_RELAT_POS)
			     nonnul = pTa1->PosRTable[i - 1].ReBox != NULL;
		       }
	       }
	     adpos = pTa1->PosRNext;	/* Bloc suivant */
	  }

	ibox->BxVertInc = box1;
	result = box1;
     }
   else
      result = NULL;
   return result;
}				/* BoiteVInclus */

/* ---------------------------------------------------------------------- */
/* |    DelPos defait, s'il existe, le lien de dependance de position ou| */
/* |            d'axe horizontal ou vertical de la boite debox et       | */
/* |            retasse la liste des liens.                             | */
/* |            Le parametre lepave, quand il est non nul, indique le   | */
/* |            pave dont on annule la regle de position et sert a`     | */
/* |            retirer l'ambiguite des relations doubles entre boites  | */
/* |            soeurs : si on trouve une relation associee a` une      | */
/* |            regle de position qui reference lepave, cette relation  | */
/* |            ne doit pas etre detruite. Quand lepave est nul, on     | */
/* |            connait la boite abox referencee dans le lien que l'on  | */
/* |            veut detruire (c'est alors une destruction de lien      | */
/* |            inverse).                                               | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static boolean      DelPos (PtrBox debox, PtrBox abox, PtrAbstractBox lepave, boolean Pos, boolean Axe, boolean EnX)
#else  /* __STDC__ */
static boolean      DelPos (debox, abox, lepave, Pos, Axe, EnX)
PtrBox            debox;
PtrBox            abox;
PtrAbstractBox             lepave;
boolean             Pos;
boolean             Axe;
boolean             EnX;

#endif /* __STDC__ */
{
   int                 i, trouve;
   int                 j, k;
   boolean             boucle;
   boolean             nonnul;
   PtrPosRelations      cepos;
   PtrPosRelations      precpos;
   PtrPosRelations      adpos;
   PtrAbstractBox             adpave;
   PtrPosRelations      pTa1;
   BoxRelation           *pRe1;
   boolean             result;


   /* On recherche l'entree a detruire et la derniere entree occupee */
   trouve = 0;
   i = 0;
   cepos = NULL;
   adpos = debox->BxPosRelations;
   precpos = NULL;
   boucle = True;
   if (adpos != NULL)
      while (boucle)
	{
	   pTa1 = adpos;
	   i = 1;
	   nonnul = pTa1->PosRTable[i - 1].ReBox != NULL;
	   while (i <= MAX_RELAT_POS && nonnul)
	     {
		pRe1 = &pTa1->PosRTable[i - 1];
		/* Si c'est une relation morte, on retasse la liste */
		if (pRe1->ReBox->BxAbstractBox == NULL)
		  {
		     j = i;
		     while (j < MAX_RELAT_POS)
		       {
			  k = j + 1;
			  pTa1->PosRTable[j - 1].ReBox = pTa1->PosRTable[k - 1].ReBox;
			  pTa1->PosRTable[j - 1].ReRefEdge = pTa1->PosRTable[k - 1].ReRefEdge;
			  pTa1->PosRTable[j - 1].ReOp = pTa1->PosRTable[k - 1].ReOp;
			  if (pTa1->PosRTable[k - 1].ReBox == NULL)
			     j = MAX_RELAT_POS;
			  else
			    {
			       j++;
			       /* Faut-il annuler la derniere entree ? */
			       if (j == MAX_RELAT_POS)
				  pTa1->PosRTable[j - 1].ReBox = NULL;
			    }
		       }

		     /* C'etait la derniere relation dans la table ? */
		     if (i == MAX_RELAT_POS)
			pRe1->ReBox = NULL;
		     else
			i--;	/* Il faut reexaminer cette entree */
		  }
		/* Si c'est une relation en X */
		else if (EnX)
		  {
		     /* Est-ce l'entree a detruite ? */
		     if (pRe1->ReBox == abox
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
			      && lepave != NULL && pRe1->ReBox == abox)
		       {
			  adpave = pRe1->ReBox->BxAbstractBox;
			  adpave = adpave->AbHorizPos.PosAbRef;

			  /* Si la position du pave distant est donnee par une    */
			  /* regle NULL, il faut rechercher le pave dont il depend */
			  if (adpave == NULL)
			     adpave = PavDePos (pRe1->ReBox->BxAbstractBox, EnX);

			  /* On a bien trouve la relation de positionnement       */
			  /* du pave lepave et non une relation de positionnement */
			  /* du pave distant par rapport au pave lepave ?         */
			  if (adpave != lepave)
			    {
			       trouve = i;
			       cepos = adpos;
			    }
		       }
		  }
		/* Si c'est une relation en Y */
		/* Est-ce l'entree a detruite ? */
		else if (pRe1->ReBox == abox
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
			 && lepave != NULL && pRe1->ReBox == abox)
		  {
		     adpave = pRe1->ReBox->BxAbstractBox;
		     adpave = adpave->AbVertPos.PosAbRef;

		     /* Si la position du pave distant est donnee par une    */
		     /* regle NULL, il faut rechercher le pave dont il depend */
		     if (adpave == NULL)
			adpave = PavDePos (pRe1->ReBox->BxAbstractBox, EnX);

		     /* On a bien trouve la relation de positionnement       */
		     /* du pave lepave et non une relation de positionnement */
		     /* du pave distant par rapport au pave lepave ?         */
		     if (adpave != lepave)
		       {
			  trouve = i;
			  cepos = adpos;
		       }
		  }
		i++;
		if (i <= MAX_RELAT_POS)
		   nonnul = pTa1->PosRTable[i - 1].ReBox != NULL;
	     }

	   if (pTa1->PosRNext == NULL)
	      boucle = False;
	   else
	     {
		precpos = adpos;
		adpos = pTa1->PosRNext;	/* Bloc suivant */
	     }
	}
   /* On a trouve -> on retasse la liste */
   if (trouve > 0)
     {
	pRe1 = &cepos->PosRTable[trouve - 1];

	/* Faut-il defaire la relation inverse ? */
	if (lepave != NULL && (pRe1->ReOp == OpHorizDep || pRe1->ReOp == OpVertDep))
	   boucle = DelPos (pRe1->ReBox, debox, NULL, Pos, Axe, EnX);

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
		debox->BxPosRelations = NULL;
	     else
		precpos->PosRNext = NULL;
	     FreeBPos (&adpos);
	  }
	else
	   adpos->PosRTable[i - 1].ReBox = NULL;
	result = True;
     }
   else
      result = False;
   return result;
}				/* DelPos */

/* ---------------------------------------------------------------------- */
/* |    DelDim defait, s'il existe, le lien de de'pendance de dimension | */
/* |            horizontale ou verticale de la boite debox vers la boite| */
/* |            abox et retasse la liste des liens.                     | */
/* |            Rend la valeur Vrai si l'operation a ete executee.      | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static boolean      DelDim (PtrBox debox, PtrBox abox, boolean EnX)
#else  /* __STDC__ */
static boolean      DelDim (debox, abox, EnX)
PtrBox            debox;
PtrBox            abox;
boolean             EnX;

#endif /* __STDC__ */
{
   int                 i, trouve;
   boolean             boucle;
   boolean             nonnul;
   PtrDimRelations      cedim;
   PtrDimRelations      precdim;
   PtrDimRelations      addim;
   PtrDimRelations      pTa1;
   boolean             result;


   i = 0;
   /* Cela peut etre une dimension elastique */
   if ((EnX && abox->BxHorizFlex) || (!EnX && abox->BxVertFlex))
      result = DelPos (debox, abox, NULL, False, False, EnX);
   else
     {
	/* On recherche l'entree a detruire et la derniere entree occupee */
	trouve = 0;
	cedim = NULL;
	if (EnX)
	   addim = debox->BxWidthRelations;
	else
	   addim = debox->BxHeightRelations;
	precdim = NULL;
	boucle = True;
	if (addim != NULL)

	   while (boucle)
	     {
		pTa1 = addim;
		i = 1;
		nonnul = pTa1->DimRTable[i - 1] != NULL;
		while (i <= MAX_RELAT_DIM && nonnul)
		  {
		     /* Est-ce l'entree a detruire ? */
		     if (pTa1->DimRTable[i - 1] == abox)
		       {
			  trouve = i;
			  cedim = addim;
		       }
		     i++;
		     if (i <= MAX_RELAT_DIM)
			nonnul = pTa1->DimRTable[i - 1] != NULL;
		  }

		if (pTa1->DimRNext == NULL)
		   boucle = False;
		else
		  {
		     precdim = addim;
		     addim = pTa1->DimRNext;
		     /* Bloc suivant */
		  }
	     }

	/* On a trouve -> on retasse la liste */
	if (trouve > 0)
	  {
	     i--;
	     cedim->DimRTable[trouve - 1] = addim->DimRTable[i - 1];
	     cedim->DimRSame[trouve - 1] = addim->DimRSame[i - 1];

	     /* Faut-il liberer le dernier bloc de relations ? */
	     if (i == 1)
	       {
		  if (precdim == NULL)
		     if (EnX)
			debox->BxWidthRelations = NULL;
		     else
			debox->BxHeightRelations = NULL;
		  else
		     precdim->DimRNext = NULL;
		  FreeBDim (&addim);
	       }
	     else
		addim->DimRTable[i - 1] = NULL;
	     result = True;
	  }
	/* On n'a pas trouve */
	else
	   result = False;
     }
   return result;
}				/* DelDim */

/* ---------------------------------------------------------------------- */
/* |    RazHorsEnglobe detruit les relations hors hierarchie de la boite| */
/* |            abox.                                                   | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                RazHorsEnglobe (PtrBox abox)
#else  /* __STDC__ */
void                RazHorsEnglobe (abox)
PtrBox            abox;

#endif /* __STDC__ */
{
   PtrBox            debox;
   PtrAbstractBox             pPa1;

   pPa1 = abox->BxAbstractBox;

   /* On detruit la relation de position horizontale hors-structure */
   if (abox->BxXOutOfStruct)
     {
	if (pPa1->AbHorizPos.PosAbRef == NULL)
	   debox = NULL;
	else
	   debox = pPa1->AbHorizPos.PosAbRef->AbBox;
	if (debox != NULL)
	   DelPos (debox, abox, NULL, True, False, True);

	/* Annule les relations hors-structure */
	PropageXHorsStruct (pPa1, False, pPa1->AbHorizEnclosing);
     }

   /* On detruit la relation de position verticale hors-structure */
   if (abox->BxYOutOfStruct)
     {
	if (pPa1->AbVertPos.PosAbRef == NULL)
	   debox = NULL;
	else
	   debox = pPa1->AbVertPos.PosAbRef->AbBox;
	if (debox != NULL)
	   DelPos (debox, abox, NULL, True, False, False);

	/* Annule les relations hors-structure */
	PropageYHorsStruct (pPa1, False, pPa1->AbVertEnclosing);
     }

   /* On detruit la relation de largeur hors-structure */
   if (abox->BxWOutOfStruct)
      /* Est-ce une dimension elastique ? */
      if (abox->BxHorizFlex)
	{
	   debox = pPa1->AbWidth.DimPosition.PosAbRef->AbBox;
	   if (debox != NULL)
	      DelPos (debox, abox, NULL, False, False, True);
	}
      else
	{
	   debox = pPa1->AbWidth.DimAbRef->AbBox;
	   if (debox != NULL)
	      DelDim (debox, abox, True);
	}

   /* On detruit la relation de hauteur hors-structure */
   debox = NULL;
   if (abox->BxHOutOfStruct)
      /* Est-ce une dimension elastique ? */
      if (abox->BxVertFlex)
	{
	   if (pPa1->AbHeight.DimPosition.PosAbRef != NULL)
	      debox = pPa1->AbHeight.DimPosition.PosAbRef->AbBox;
	   if (debox != NULL)
	      DelPos (debox, abox, NULL, False, False, False);
	}
      else
	{
	   if (pPa1->AbHeight.DimAbRef != NULL)
	      debox = pPa1->AbHeight.DimAbRef->AbBox;
	   if (debox != NULL)
	      DelDim (debox, abox, False);
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
/* |            horizontale (si EnX est Vrai) sinon verticale de debox :| */
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
void                RazPosition (PtrBox debox, boolean EnX)
#else  /* __STDC__ */
void                RazPosition (debox, EnX)
PtrBox            debox;
boolean             EnX;

#endif /* __STDC__ */
{
   PtrAbstractBox             adpave;
   PtrAbstractBox             lepave;
   boolean             boucle;


   adpave = debox->BxAbstractBox;
   /* Est-ce une relation hors-structure ? */
   if ((EnX && debox->BxXOutOfStruct) || (!EnX && debox->BxYOutOfStruct))
     {
	/* Si la boite est detruite la procedure RazHorsEnglobe detruit */
	/* automatiquement cette relation                               */
	if (!adpave->AbDead)
	  {
	     /* On remonte a la racine depuis le pave pere */
	     lepave = adpave;
	     /* La relation hors-structure peut etre heritee d'une boite voisine */
	     while (lepave->AbEnclosing != NULL)
		lepave = lepave->AbEnclosing;
	     boucle = True;


	     /* Recherche dans debox l'ancienne relation de positionnement */
	     while (boucle && lepave != NULL)
	       {
		  if (lepave->AbBox != NULL)
		     boucle = !DelPos (debox, lepave->AbBox, adpave, True, False, EnX);
		  lepave = ProchainPave (lepave, adpave);
	       }

	     /* La relation hors-structure est detruite */
	     if (EnX)
	       {
		  debox->BxXOutOfStruct = False;

		  /* Des boites voisines ont herite de la relation hors-structure ? */
		  lepave = adpave->AbEnclosing;
		  if (lepave != NULL)
		    {
		       /* On regarde tous les freres */
		       lepave = lepave->AbFirstEnclosed;
		       while (lepave != NULL)
			 {
			    if (lepave != adpave && lepave->AbBox != NULL)
			       /* Si c'est un heritage on retire l'indication hors-structure */
			       if (lepave->AbBox->BxXOutOfStruct
			       && lepave->AbHorizPos.PosAbRef == adpave)
				  lepave->AbBox->BxXOutOfStruct = False;
			       else if (lepave->AbBox->BxWOutOfStruct
					&& lepave->AbWidth.DimIsPosition
			       && lepave->AbWidth.DimPosition.PosAbRef == adpave)
				  lepave->AbBox->BxWOutOfStruct = False;

			    lepave = lepave->AbNext;
			 }
		    }
	       }
	     else
	       {
		  debox->BxYOutOfStruct = False;

		  /* Des boites voisines ont herite de la relation hors-structure ? */
		  lepave = adpave->AbEnclosing;
		  if (lepave != NULL)
		    {
		       lepave = lepave->AbFirstEnclosed;
		       while (lepave != NULL)
			 {
			    if (lepave != adpave && lepave->AbBox != NULL)
			       /* Si c'est un heritage on retire l'indication hors-structure */
			       if (lepave->AbBox->BxYOutOfStruct
			       && lepave->AbVertPos.PosAbRef == adpave)
				  lepave->AbBox->BxYOutOfStruct = False;
			       else if (lepave->AbBox->BxHOutOfStruct
					&& lepave->AbHeight.DimIsPosition
			       && lepave->AbHeight.DimPosition.PosAbRef == adpave)
				  lepave->AbBox->BxHOutOfStruct = False;

			    lepave = lepave->AbNext;
			 }

		    }
	       }
	  }
     }
   /* Est-ce une relation avec une boite voisine ? */
   else
     {
	lepave = adpave->AbEnclosing;
	boucle = True;

	/* Recherche dans la boite debox l'ancienne relation de positionnement */
	while (boucle && lepave != NULL)
	  {
	     if (lepave->AbBox != NULL)
		boucle = !DelPos (debox, lepave->AbBox, adpave, True, False, EnX);
	     if (lepave == adpave->AbEnclosing)
		lepave = lepave->AbFirstEnclosed;
	     else
		lepave = lepave->AbNext;
	  }			/*while */
     }
}				/* RazPosition */

/* ---------------------------------------------------------------------- */
/* |    RazAxe recherche la boite dont depend l'axe de reference        | */
/* |            horizontal (si EnX est Vrai) sinon vertical de debox :  | */
/* |            - Il peut dependre d'elle meme (une relation chez elle).| */
/* |            - Il peut dependre d'une englobee (une relation chez    | */
/* |            l'englobee).                                            | */
/* |            - Il peut dependre d'une voisine (une relation chez la  | */
/* |            voisine).                                               | */
/* |            Si cette dependance existe encore, on detruit la        | */
/* |            relation.                                               | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                RazAxe (PtrBox debox, boolean EnX)
#else  /* __STDC__ */
void                RazAxe (debox, EnX)
PtrBox            debox;
boolean             EnX;

#endif /* __STDC__ */
{
   boolean             boucle;
   PtrAbstractBox             adpave;
   PtrAbstractBox             lepave;


   lepave = debox->BxAbstractBox;
   boucle = True;

   /* On recherche dans la descendance la dependance de l'axe de reference */
   adpave = lepave;
   while (boucle && adpave != NULL)
     {
	if (adpave->AbBox != NULL)
	   boucle = !DelPos (adpave->AbBox, debox, NULL, False, True, EnX);
	if (adpave == lepave)
	   adpave = adpave->AbFirstEnclosed;
	else
	   adpave = adpave->AbNext;

     }

   /* On recherche chez les voisines la dependance de l'axe de reference */
   adpave = lepave->AbEnclosing;
   if (adpave != NULL)
     {
	adpave = adpave->AbFirstEnclosed;
	while (boucle && adpave != NULL)
	  {
	     if (adpave != lepave && adpave->AbBox != NULL)
		boucle = !DelPos (adpave->AbBox, debox, NULL, False, True, EnX);
	     adpave = adpave->AbNext;
	  }
     }
}				/* RazAxe */

/* ---------------------------------------------------------------------- */
/* |    RazLiens detruit toutes les relations avec la boite abox chez   | */
/* |            ses voisines, son englobante et les relations hors      | */
/* |            hierarchie :                                            | */
/* |            -> la relation OpVertRef verticale.                        | */
/* |            -> la relation OpHorizRef horizontale.                      | */
/* |            -> les relations de Position.                           | */
/* |            -> les relations de positions et de dimensions hors     | */
/* |            hierarchie.                                             | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                RazLiens (PtrBox abox)
#else  /* __STDC__ */
void                RazLiens (abox)
PtrBox            abox;

#endif /* __STDC__ */
{
   PtrAbstractBox             adpave;
   PtrAbstractBox             lepave;


   lepave = abox->BxAbstractBox;
   adpave = lepave->AbEnclosing;
   if (adpave != NULL)
     {
	/* On detruit les liens d'axe qui la concerne chez l'englobante */
	DelPos (adpave->AbBox, abox, NULL, False, True, False);	/*en Y */
	DelPos (adpave->AbBox, abox, NULL, False, True, True);	/*en X */

	/* On detruit les liens qui la concerne chez toutes ses voisines */
	adpave = adpave->AbFirstEnclosed;
	while (adpave != NULL)
	  {
	     if (adpave->AbBox != NULL && adpave != lepave)
	       {
		  /* En X : enleve la regle de position d'axe ou de dimension */
		  DelPos (adpave->AbBox, abox, NULL, True, False, False);
		  DelPos (adpave->AbBox, abox, NULL, False, True, False);
		  DelPos (adpave->AbBox, abox, NULL, False, False, False);

		  /* En Y : enleve la regle de position d'axe ou de dimension */
		  DelPos (adpave->AbBox, abox, NULL, True, False, True);
		  DelPos (adpave->AbBox, abox, NULL, False, True, True);
		  DelPos (adpave->AbBox, abox, NULL, False, False, True);
	       }
	     adpave = adpave->AbNext;
	  }
	/* On detruit eventuellement les liens non hierarchiques */
	RazHorsEnglobe (abox);
     }
}				/* RazLiens */

/* ---------------------------------------------------------------------- */
/* |    RazDim recherche la boite dont depend la dimension horizontale  | */
/* |            ou verticale de debox :                                 | */
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
void                RazDim (PtrBox debox, boolean EnX, int frame)
#else  /* __STDC__ */
void                RazDim (debox, EnX, frame)
PtrBox            debox;
boolean             EnX;
int                 frame;

#endif /* __STDC__ */
{
   boolean             boucle;
   PtrAbstractBox             adpave;
   PtrAbstractBox             lepave;


   adpave = debox->BxAbstractBox;
   lepave = adpave->AbEnclosing;
   boucle = False;

   /* Est-ce une relation hors-structure en X ? */
   if ((EnX && debox->BxWOutOfStruct) || (!EnX && debox->BxHOutOfStruct))
     {
	/* Si la boite est detruite la procedure RazHorsEnglobe detruit */
	/* automatiquement cette relation                               */
	if (!adpave->AbDead)
	  {
	     /* On remonte a la racine depuis le pave pere */
	     lepave = adpave->AbEnclosing;
	     /* La relation hors-structure peut etre heritee d'une boite voisine */
	     if (lepave != NULL)
		if (lepave != NULL)
		   while (lepave->AbEnclosing != NULL)
		      lepave = lepave->AbEnclosing;


	     /* Recherche dans toute l'arborecence la relation inverse */
	     boucle = True;
	     if (EnX)
	       {
		  /* La dimension est elastique en X ? */
		  if (debox->BxHorizFlex)
		    {
		       while (boucle && lepave != NULL)
			 {
			    if (lepave->AbBox != NULL)
			       boucle = !DelPos (lepave->AbBox, debox, NULL, False, False, EnX);
			    if (lepave != NULL)
			       lepave = ProchainPave (lepave, adpave);
			 }

		       /* Il faut retablir le point fixe */
		       debox->BxHorizEdge = debox->BxAbstractBox->AbHorizPos.PosEdge;

		       /* La boite n'est pas inversee */
		       debox->BxHorizInverted = False;

		       /* La dimension n'est plus elastique */
		       debox->BxHorizFlex = False;

		       /* Annule la largeur de la boite */
		       ModLarg (debox, NULL, NULL, -debox->BxWidth, 0, frame);

		    }
		  /* La dimension n'est pas elastique en X */
		  else
		     while (boucle && lepave != NULL)
		       {
			  if (lepave->AbBox != NULL)
			     boucle = !DelDim (lepave->AbBox, debox, EnX);
			  if (lepave != NULL)
			     lepave = ProchainPave (lepave, adpave);
		       }

		  /* La relation hors-structure est detruite */
		  debox->BxWOutOfStruct = False;
	       }
	     else
	       {
		  /* La dimension est elastique en Y ? */
		  if (debox->BxVertFlex)
		    {
		       while (boucle && lepave != NULL)
			 {
			    if (lepave->AbBox != NULL)
			       boucle = !DelPos (lepave->AbBox, debox, NULL, False, False, EnX);
			    if (lepave != NULL)
			       lepave = ProchainPave (lepave, adpave);
			 }

		       /* Il faut retablir le point fixe */
		       debox->BxVertEdge = debox->BxAbstractBox->AbVertPos.PosEdge;

		       /* La boite n'est pas inversee */
		       debox->BxVertInverted = False;

		       /* La dimension n'est plus elastique */
		       debox->BxVertFlex = False;

		       /* Annule la hauteur de la boite */
		       ModHaut (debox, NULL, NULL, -debox->BxHeight, frame);
		    }
		  /* La dimension n'est pas elastique en Y */
		  else
		     while (boucle && lepave != NULL)
		       {
			  if (lepave->AbBox != NULL)
			     boucle = !DelDim (lepave->AbBox, debox, EnX);
			  if (lepave != NULL)
			     lepave = ProchainPave (lepave, adpave);
		       }

		  /* La relation hors-structure est detruite */
		  debox->BxHOutOfStruct = False;
	       }

	  }
     }

   else
     {
	/* Est-ce que la dimension depend de l'englobante ? */
	if (lepave != NULL)
	  {
	     boucle = !DelDim (lepave->AbBox, debox, EnX);
	     if (boucle)
		lepave = lepave->AbFirstEnclosed;
	  }

	/* Est-ce que la dimension de la boite depend d'une voisine ? */
	while (boucle && lepave != NULL)
	  {
	     if (lepave->AbBox != NULL && lepave != adpave)
	       {
		  boucle = !DelDim (lepave->AbBox, debox, EnX);
		  if (boucle)
		     lepave = lepave->AbNext;
	       }
	     else
		lepave = lepave->AbNext;
	  }

	/* La dimension est elastique en X ? */
	if (EnX && debox->BxHorizFlex)
	  {
	     /* La boite n'est pas inversee */
	     debox->BxHorizInverted = False;

	     /* La dimension n'est plus elastique */
	     debox->BxHorizFlex = False;

	     /* Il faut retablir le point fixe */
	     debox->BxHorizEdge = debox->BxAbstractBox->AbHorizPos.PosEdge;

	     /* Annule la largeur de la boite */
	     ModLarg (debox, NULL, NULL, -debox->BxWidth, 0, frame);
	  }

	/* La dimension est elastique en Y ? */
	if (!EnX && debox->BxVertFlex)
	  {
	     /* La boite n'est pas inversee */
	     debox->BxVertInverted = False;

	     /* La dimension n'est plus elastique */
	     debox->BxVertFlex = False;

	     /* Il faut retablir le point fixe */
	     debox->BxVertEdge = debox->BxAbstractBox->AbVertPos.PosEdge;

	     /* Annule la hauteur de la boite */
	     ModHaut (debox, NULL, NULL, -debox->BxHeight, frame);
	  }
     }

}				/* RazDim */

/* End Of Module rel */
