
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

/*
   pos.c : gestion des placements absolus des boites.
   Major changes:
   I. Vatton - Mai 87
 */

#include "thot_sys.h"
#include "libmsg.h"
#include "message.h"
#include "functions.h"
#include "constmedia.h"
#include "typemedia.h"

#define EXPORT extern
#include "img.var"
#include "environ.var"

#include "memory.f"
#include "imabs.f"
#include "img.f"
#include "dep.f"
#include "rel.f"
#include "pos.f"
#include "font.f"

/* ---------------------------------------------------------------------- */
/* |    VoirXHorsStruct regarde si des boites ont des relations         | */
/* |            hors-structure avec la boite passee en parametre et     | */
/* |            doivent etre placees en X absolu.                       | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void         VoirXHorsStruct (PtrBox pBox, int SeuilVisu, int frame)

#else  /* __STDC__ */
static void         VoirXHorsStruct (pBox, SeuilVisu, frame)
PtrBox            pBox;
int                 SeuilVisu;
int                 frame;

#endif /* __STDC__ */

{
   PtrPosRelations      adpos;
   int                 i;
   boolean             nonnul;
   BoxRelation           *pRe1;


   adpos = pBox->BxPosRelations;
   while (adpos != NULL)
     {
	i = 1;
	nonnul = adpos->PosRTable[i - 1].ReBox != NULL;
	while (i <= MAX_RELAT_POS && nonnul)
	  {
	     pRe1 = &adpos->PosRTable[i - 1];
	     if (pRe1->ReBox->BxAbstractBox != NULL)
		/* Relation hors-struture sur l'origine de la boite */
		if (pRe1->ReOp == OpHorizDep
		    && pRe1->ReBox->BxXOutOfStruct
		    && pRe1->ReBox->BxAbstractBox->AbHorizPos.PosAbRef == pBox->BxAbstractBox
		    && pRe1->ReBox->BxXToCompute)
		  {
		     /* La boite distante va etre placee */
		     pRe1->ReBox->BxXToCompute = False;
		     Placer (pRe1->ReBox->BxAbstractBox, SeuilVisu, frame, True, False);
		     VoirXHorsStruct (pRe1->ReBox, SeuilVisu, frame);
		  }
	     i++;
	     if (i <= MAX_RELAT_POS)
		nonnul = adpos->PosRTable[i - 1].ReBox != NULL;
	  }
	adpos = adpos->PosRNext;
     }
}				/* VoirXHorsStruct */

/* ---------------------------------------------------------------------- */
/* |    VoirYHorsStruct regarde si des boites ont des relations         | */
/* |            hors-structure avec la boite passee en parametre et     | */
/* |            doivent etre placees en Y absolu.                       | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void         VoirYHorsStruct (PtrBox pBox, int SeuilVisu, int frame)

#else  /* __STDC__ */
static void         VoirYHorsStruct (pBox, SeuilVisu, frame)
PtrBox            pBox;
int                 SeuilVisu;
int                 frame;

#endif /* __STDC__ */

{
   PtrPosRelations      adpos;
   int                 i;
   boolean             nonnul;
   BoxRelation           *pRe1;


   adpos = pBox->BxPosRelations;
   while (adpos != NULL)
     {
	i = 1;
	nonnul = adpos->PosRTable[i - 1].ReBox != NULL;
	while (i <= MAX_RELAT_POS && nonnul)
	  {
	     pRe1 = &adpos->PosRTable[i - 1];
	     if (pRe1->ReBox->BxAbstractBox != NULL)
		/* Relation hors-struture sur l'origine de la boite */
		if (pRe1->ReOp == OpVertDep
		    && pRe1->ReBox->BxYOutOfStruct
		    && pRe1->ReBox->BxAbstractBox->AbVertPos.PosAbRef == pBox->BxAbstractBox
		    && pRe1->ReBox->BxYToCompute)
		  {
		     /* La boite distante va etre placee */
		     pRe1->ReBox->BxYToCompute = False;
		     Placer (pRe1->ReBox->BxAbstractBox, SeuilVisu, frame, False, True);
		     VoirYHorsStruct (pRe1->ReBox, SeuilVisu, frame);
		  }
	     i++;
	     if (i <= MAX_RELAT_POS)
		nonnul = adpos->PosRTable[i - 1].ReBox != NULL;
	  }
	adpos = adpos->PosRNext;
	/* Bloc suivant */
     }
}				/* VoirYHorsStruct */

/* ---------------------------------------------------------------------- */
/* |    Placer met a` jour toutes les origines des boi^tes correpondant | */
/* |            aux pave's inclus dans pAb.                          | */
/* |            A chaque niveau la proce'dure additionnne le de'calage  | */
/* |            de la boi^te englobante aux origines des boi^tes        | */
/* |            incluses, en X et en Y selon la valeur de l'indicateur  | */
/* |            EnX et EnY et du status de la boi^te englobe'e.         | */
/* |            Si ne'cessaire, la proce'dure ve'rifie l'englobement.   | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                Placer (PtrAbstractBox pAb, int SeuilVisu, int frame, boolean EnX, boolean EnY)

#else  /* __STDC__ */
void                Placer (pAb, SeuilVisu, frame, EnX, EnY)
PtrAbstractBox             pAb;
int                 SeuilVisu;
int                 frame;
boolean             EnX;
boolean             EnY;

#endif /* __STDC__ */

{
   int                 x, y, i;
   PtrBox            box1;
   int                 larg, haut;
   PtrAbstractBox             pavefils;
   boolean             eclate;
   boolean             Peclate;
   boolean             nonnul;
   PtrPosRelations      adpos;
   boolean             newX;
   boolean             newY;
   boolean             placeenX;
   boolean             placeenY;
   boolean             reenglobx;
   boolean             reengloby;
   PtrBox            pBo1;
   PtrBox            pBox;
   BoxRelation           *pRe1;

   /* Origine de la boite du pave le plus englobant */
   pBox = pAb->AbBox;
   x = pBox->BxXOrg;
   y = pBox->BxYOrg;
   larg = pBox->BxWidth;
   haut = pBox->BxHeight;
   Peclate = pBox->BxType == BoGhost;
   pavefils = pAb->AbFirstEnclosed;

   /* Indique s'il faut reevaluer l'englobement du contenu apres mise a jour */
   reenglobx = False;
   reengloby = False;
   /* EnX et EnY indiquent que la boite mere (pBox) transmet son decalage */
   /* newX et newY indiquent que la boite fille (pBo1) accepte le decalage */
   /* placeenX et placeenY indiquent que la boite fille transmet son decalage */

   /* Transforme origines relatives des boites filles en origines absolues */
   if (pAb->AbVisibility >= SeuilVisu)
      while (pavefils != NULL)
	{

	   pBo1 = pavefils->AbBox;
	   if (pBo1 != NULL)
	     {
		eclate = Peclate || pBo1->BxType == BoGhost;

		/* Decale boites englobees dont l'origine depend de l'englobante */
		/* La boite est coupee, on decale les boites de coupure */
		if (pBo1->BxType == BoSplit)
		  {
		     box1 = pBo1->BxNexChild;
		     while (box1 != NULL)
		       {
			  if (EnX)
			     box1->BxXOrg += x;
			  if (EnY)
			     box1->BxYOrg += y;
			  box1 = box1->BxNexChild;
		       }
		  }
		else
		  {
		     /* S'il s'agit d'un bloc de ligne elastique */
		     /* il faut reevaluer l'englobement vertical */
		     if (pBo1->BxType == BoBlock && pBo1->BxHorizFlex)
			reengloby = True;
		     /* Regarde si la boite est positionnee en X dans l'englobante */
		     box1 = BoiteHInclus (pBo1, NULL);
		     placeenX = pBo1->BxXToCompute;
		     if (box1 == NULL)
			newX = True;
		     else if (pBo1->BxXOutOfStruct)
			newX = False;
		     else if (box1->BxHorizFlex && box1 != pBo1)
			/* Le decalage des boites voisines liees a la boite elastique */
			/* est deja effectue par l'appel de ChngBElast */
			newX = False;
		     else
			newX = True;

		     /* On regarde si la boite doit etre placee en X absolu */
		     if (EnX)
		       {
			  if (newX)
			    {
			       /* Si le deplacement est nul on ne peut executer DepOrgX */
			       if (x == 0)
				  newX = False;
			    }
			  /* Il faut placer les boites positionnees par rapport la racine */
			  else if (pavefils->AbHorizPos.PosAbRef == FntrTable[frame - 1].FrAbstractBox)
			     placeenX = True;
		       }
		     else
			placeenX = False;

		     /* On regarde si la boite est positionnee en Y dans l'englobante */
		     box1 = BoiteVInclus (pBo1, NULL);
		     placeenY = pBo1->BxYToCompute;
		     if (box1 == NULL)
			newY = True;
		     else if (pBo1->BxYOutOfStruct)
			newY = False;
		     else if (box1->BxVertFlex && box1 != pBo1)
			/* Le decalage des boites voisines liees a la boite elastique */
			/* est deja effectue par l'appel de ChngBElast */
			newY = False;
		     else
			newY = True;

		     /* On regarde si la boite doit etre placee en Y absolu */
		     if (EnY)
		       {

			  if (newY)
			    {
			       /* Si le deplacement est nul on ne peut executer DepOrgY */
			       if (y == 0)
				  newY = False;
			    }
			  /* Il faut placer les boites positionnee par rapport la racine */
			  else if (pavefils->AbVertPos.PosAbRef == FntrTable[frame - 1].FrAbstractBox)
			     placeenY = True;
		       }
		     else
			placeenY = False;

		     /* Le contenu des boites elastiques et hors-structures est deja place */
		     if (pBo1->BxHorizFlex || pBo1->BxXOutOfStruct)
		       {
			  placeenX = False;
			  /*if (pBo1->BxType == BoBlock) */
			  /* La reevaluation du bloc de lignes place le contenu en Y */
			  /*placeenY = False; */
		       }
		     if (pBo1->BxVertFlex || pBo1->BxYOutOfStruct)
			placeenY = False;
		     /* On detecte les erreurs d'englobement de la boite englobee */
		     if (!eclate && pBo1->BxType != BoPicture
		     /* Ne verifie pas les boites placees par relation hors-structure */
			 && !pBo1->BxXOutOfStruct && !pBo1->BxYOutOfStruct)
			if (pavefils->AbHorizEnclosing && placeenX
			    && (pBo1->BxXOrg < 0 || pBo1->BxXOrg > larg))
			  {
			     Erreur = True;
			     if (HardMsgAff)
			       {
				  pavefils->AbSelected = True;	/* Mise en evidence de l'erreur, pas une 
								   selection ! */
				  /* Erreur sur l'inclusion des boites */
				  TtaDisplaySimpleMessageString (LIB, INFO, LIB_HORIZ_POS_MISTAKE, TypePave (pavefils));

			       }
			  }
			else if (pavefils->AbVertEnclosing && placeenY
			       && (pBo1->BxYOrg < 0 || pBo1->BxYOrg > haut))
			  {
			     Erreur = True;
			     if (HardMsgAff)
			       {
				  pavefils->AbSelected = True;	/* Mise en evidence de l'erreur, pas une 
								   selection ! */
				  /* Erreur sur l'inclusion des boites */
				  TtaDisplaySimpleMessageString (LIB, INFO, LIB_VERTIC_POS_MISTAKE, TypePave (pavefils));

			       }
			  }
			else if (pBo1->BxWidth < 0)
			  {
			     pBo1->BxWidth = 1;
			     Erreur = True;
			     if (HardMsgAff)
			       {
				  pavefils->AbSelected = True;
				  /* Mise en evidence de l'erreur, pas une selection ! */
				  /* ** Erreur sur les dimensions des boites */
				  TtaDisplaySimpleMessageString (LIB, INFO, LIB_HORIZ_SIZING_MISTAKE, TypePave (pavefils));

			       }
			  }
			else if (pBo1->BxHeight < 0)
			  {
			     pBo1->BxHeight = 1;
			     Erreur = True;
			     if (HardMsgAff)
			       {
				  pavefils->AbSelected = True;
				  /* Mise en evidence de l'erreur, pas une selection ! */
				  /* ** Erreur sur les dimensions des boites */
				  TtaDisplaySimpleMessageString (LIB, INFO, LIB_VERTIC_SIZING_MISTAKE, TypePave (pavefils));

			       }
			  }
		     /* On decale la boite positionnee en X dans l'englobante */
		     if (EnX && newX)
		       {
			  i = pBo1->BxXOrg + pBo1->BxWidth - larg;
			  /* On regarde s'il s'agit d'une boite elastique */
			  if (pBo1->BxHorizFlex)
			    {
			       /* Initialise la file des boites deplacees */
			       box1 = pavefils->AbHorizPos.PosAbRef->AbBox;
			       box1->BxMoved = NULL;
			       /* Pas de deplacement du contenu des boites qui */
			       /*  dependent de la boite elastique             */
			       ChngBElast (pBo1, box1, OpHorizDep, x, frame, True);
			    }
/**BE:AOUT*/ 
			  else if (!placeenX)
			     /**BE:AOUT*//* il faut deplacer tout le contenu de la boite */
/**BE:AOUT*/ DepXContenu (pBo1, x, frame);
			  else
			     pBo1->BxXOrg += x;

			  pBo1->BxXToCompute = False;	/* La boite est placee */

			  /* On detecte les debordements de la boite englobante */
			  if (pavefils->AbHorizEnclosing
			      && !eclate
			      && i > 1
			      && !pBo1->BxHorizFlex
			      && !pBox->BxHorizFlex)
			    {
			       Erreur = True;
			       if (HardMsgAff)
				 {
				    pavefils->AbSelected = True;
				    /* Mise en evidence de l'erreur, pas une selection ! */
				    /* Erreur sur l'inclusion des boites */
				    TtaDisplaySimpleMessageString (LIB, INFO, LIB_HORIZ_BOX_OVERFLOW, TypePave (pavefils));

				 }
			    }
			  /* Decale les boites qui ont des relations hors-structure avec */
			  /* la boite deplacee et met a jour les dimensions elastiques   */
			  /* des boites liees a la boite deplacee.                       */
			  adpos = pBo1->BxPosRelations;
			  while (adpos != NULL)
			    {
			       i = 1;
			       nonnul = adpos->PosRTable[i - 1].ReBox != NULL;
			       /* Si la boite est elastique, les relations */
			       /* hors-structure sont deja traitees.       */
			       if (!pBo1->BxHorizFlex)
				  while (i <= MAX_RELAT_POS && nonnul)
				    {
				       pRe1 = &adpos->PosRTable[i - 1];
				       if (pRe1->ReBox->BxAbstractBox != NULL)
					 {
					    /* Initialise la file des boites deplacees */
					    pBo1->BxMoved = NULL;
					    /* Relation hors-struture sur l'origine de la boite */
					    if (pRe1->ReOp == OpHorizDep
					     && pRe1->ReBox->BxXOutOfStruct
						&& pRe1->ReBox->BxAbstractBox->AbHorizPos.PosAbRef == pavefils)
					      {
						 if (pRe1->ReBox->BxHorizFlex)
						    ChngBElast (pRe1->ReBox, pBo1, pRe1->ReOp, x, frame, True);
						 else
						    DepOrgX (pRe1->ReBox, pBo1, x, frame);
						 /* La boite distante est placee */
						 pRe1->ReBox->BxXToCompute = False;
					      }
					    /* Relation sur la largeur elastique de la boite */
					    else if (pRe1->ReOp == OpWidth)
					      {
						 /* Pas de deplacement du contenu des boites qui */
						 /*  dependent de la boite elastique             */
						 ChngBElast (pRe1->ReBox, pBo1, pRe1->ReOp, x, frame, True);
					      }
					 }
				       i++;
				       if (i <= MAX_RELAT_POS)
					  nonnul = adpos->PosRTable[i - 1].ReBox != NULL;
				    }
			       adpos = adpos->PosRNext;
			    }
		       }
		     /* On ne decale pas la boite, mais le fait de deplacer */
		     /* l'englobante sans deplacer une englobee peut        */
		     /* modifier la largeur de la boite englobante.         */
		     else if (EnX && pBo1->BxXOutOfStruct)
			reenglobx = True;
		     /* On traite les relations hors-structures des boites non */
		     /* decalees mais qui doivent etre placees en X absolu     */
		     if (placeenX && !newX)
		       {
			  VoirXHorsStruct (pBo1, SeuilVisu, frame);
			  /* La boite est placee */
			  pBo1->BxXToCompute = False;
		       }
		     /* On decale la boite positionnee en Y dans l'englobante */
		     if (EnY && newY)
		       {
			  i = pBo1->BxYOrg + pBo1->BxHeight - haut;

			  /* On regarde s'il s'agit d'une boite elastique */
			  if (pBo1->BxVertFlex)
			    {
			       /* Initialise la file des boites deplacees */
			       box1 = pavefils->AbVertPos.PosAbRef->AbBox;
			       box1->BxMoved = NULL;
			       /* Pas de deplacement du contenu des boites qui */
			       /*  dependent de la boite elastique             */
			       ChngBElast (pBo1, box1, OpVertDep, y, frame, False);
			    }
/**BE:AOUT*/ 
			  else if (!placeenY)
			     /**BE:AOUT*//* il faut deplacer tout le contenu de la boite */
/**BE:AOUT*/ DepYContenu (pBo1, y, frame);
			  else
			     pBo1->BxYOrg += y;
			  pBo1->BxYToCompute = False;	/* La boite est placee */
			  /* On detecte les debordements en Y de la boite englobante */
			  if (pavefils->AbVertEnclosing
			      && !eclate
			      && i > 1
			      && !pBo1->BxVertFlex
			      && !pBox->BxVertFlex)
			    {
			       Erreur = True;
			       if (HardMsgAff)
				 {
				    pavefils->AbSelected = True;
				    /* Mise en evidence de l'erreur, pas une selection ! */
				    /* Erreur sur l'inclusion des boites */
				    TtaDisplaySimpleMessageString (LIB, INFO, LIB_VERTIC_BOX_OVERFLOW, TypePave (pavefils));

				 }
			    }
			  /* Decale les boites qui ont des relations hors-structure avec */
			  /* la boite deplacee et met a jour les dimensions elastiques   */
			  /* des boites liees a la boite deplacee.                       */
			  adpos = pBo1->BxPosRelations;
			  while (adpos != NULL)
			    {
			       i = 1;
			       nonnul = adpos->PosRTable[i - 1].ReBox != NULL;
			       /* Si la boite est elastique, les relations */
			       /* hors-structure sont deja traitees.       */
			       if (!pBo1->BxVertFlex)
				  while (i <= MAX_RELAT_POS && nonnul)
				    {
				       pRe1 = &adpos->PosRTable[i - 1];
				       if (pRe1->ReBox->BxAbstractBox != NULL)
					 {
					    /* Initialise la file des boites deplacees */
					    pBo1->BxMoved = NULL;
					    /* Relation hors-struture sur l'origine de la boite */
					    if (pRe1->ReOp == OpVertDep
					     && pRe1->ReBox->BxYOutOfStruct
						&& pRe1->ReBox->BxAbstractBox->AbVertPos.PosAbRef == pavefils)
					      {
						 if (pRe1->ReBox->BxVertFlex)
						    ChngBElast (pRe1->ReBox, pBo1, pRe1->ReOp, y, frame, False);
						 else
						    DepOrgY (pRe1->ReBox, pBo1, y, frame);
						 /* La boite distante est placee */
						 pRe1->ReBox->BxYToCompute = False;
					      }
					    /* Relation sur la hauteur elastique de la boite */
					    else if (pRe1->ReOp == OpHeight)
					      {
						 /* Pas de deplacement du contenu des boites qui */
						 /*  dependent de la boite elastique             */
						 ChngBElast (pRe1->ReBox, pBo1, pRe1->ReOp, y, frame, False);
					      }
					 }
				       i++;
				       if (i <= MAX_RELAT_POS)
					  nonnul = adpos->PosRTable[i - 1].ReBox != NULL;
				    }
			       adpos = adpos->PosRNext;
			    }
		       }
		     /* On ne decale pas la boite, mais le fait de deplacer */
		     /* l'englobante sans deplacer une englobee peut        */
		     /* modifier la hauteur de la boite englobante.         */
		     else if (EnY && pBo1->BxYOutOfStruct)
			reengloby = True;

		     /* On traite les relations hors-structures des boites non */
		     /* decalees mais qui doivent etre placees en Y absolu     */
		     if (placeenY && !newY)
		       {
			  VoirYHorsStruct (pBo1, SeuilVisu, frame);
			  /* La boite est placee */
			  pBo1->BxYToCompute = False;
		       }
		     /* On traite les origines des boites de niveau inferieur */
		     if (placeenX || placeenY)
			Placer (pavefils, SeuilVisu, frame, placeenX, placeenY);
		  }
	     }
	   pavefils = pavefils->AbNext;	/* On passe au suivant */
	}
   /* Si une dimension de la boite depend du contenu et qu'une des  */
   /* boites filles est positionnee par une relation hors-structure */
   /* --> il faut reevaluer la dimension correspondante.            */
   if (reenglobx && pBox->BxContentWidth)
      DiffereEnglobement (pBox, True);

   if (reengloby && pBox->BxContentHeight)
      DiffereEnglobement (pBox, False);
}				/* Placer */


/* ---------------------------------------------------------------------- */
/* |    SurLaPage marque tous les paves ascendants comme coupe's par    | */
/* |            la limite de page.                                      | */
/* |            Quand la limite de page coupe un pave' non se'cable la  | */
/* |            limite de page est alors remonte'e pour rejeter hors    | */
/* |            page le pave' et le processus est repris au de'but.     | */
/* |            Dans ce cas, au retour de la fonction le parame`tre     | */
/* |            haut est modifie' et modifpage est poste' a` Vrai.      | */
/* ---------------------------------------------------------------------- */


#ifdef __STDC__
static void         SurLaPage (PtrAbstractBox pAb, int *haut, boolean * modifpage)

#else  /* __STDC__ */
static void         SurLaPage (pAb, haut, modifpage)
PtrAbstractBox             pAb;
int                *haut;
boolean            *modifpage;

#endif /* __STDC__ */

{

   if (pAb != NULL)
     {
	if (!pAb->AbAcceptPageBreak && *haut > pAb->AbBox->BxYOrg)
	  {
	     /* La boite est sur la limite de page mais non secable */
	     /* deplace la limite de page sur l'origine de la boite */
	     *haut = pAb->AbBox->BxYOrg;
	     *modifpage = True;
	  }
	else if (!pAb->AbOnPageBreak)
	  {
	     pAb->AbOnPageBreak = True;
	     pAb->AbAfterPageBreak = False;
	     /* On traite le pave pere */
	     if (pAb->AbVertEnclosing)
		SurLaPage (pAb->AbEnclosing, haut, modifpage);
	  }
     }
}				/*SurLaPage */


/* ---------------------------------------------------------------------- */
/* |    HorsDeLaPage marque tous les paves ascendants comme coupe's     | */
/* |            par la limite de page s'ils ne sont pas de'ja`          | */
/* |            marque's comme sur la page ou hors de la page et si     | */
/* |            le pave' est englobe' verticalement.                    | */
/* ---------------------------------------------------------------------- */


#ifdef __STDC__
static void         HorsDeLaPage (PtrAbstractBox pAb, int *haut, boolean * modifpage)

#else  /* __STDC__ */
static void         HorsDeLaPage (pAb, haut, modifpage)
PtrAbstractBox             pAb;
int                *haut;
boolean            *modifpage;

#endif /* __STDC__ */

{
   PtrAbstractBox             pavepere;

   if (pAb != NULL)
     {
	pAb->AbOnPageBreak = False;
	pAb->AbAfterPageBreak = True;
	pavepere = pAb->AbEnclosing;
	if (pAb->AbVertEnclosing && pavepere != NULL)
	   /* Le pere est sur la page ou hors de la page */
	   if (pavepere->AbBox->BxType == BoGhost)
	     {
		if (!pavepere->AbOnPageBreak)
		   HorsDeLaPage (pavepere, haut, modifpage);
	     }
	   else if (!pavepere->AbAfterPageBreak)
	      SurLaPage (pavepere, haut, modifpage);
     }
}				/*HorsDeLaPage */


/* ---------------------------------------------------------------------- */
/* |    CoupSurPage teste la position d'un pave' par rapport a` la      | */
/* |            limite de page. Elle rend Vrai si la boi^te du pave'    | */
/* |            est incluse dans la page. Sinon le pave' est coupe'     | */
/* |            par la limite ou se situe au dela` de la limite et les  | */
/* |            indicateurs correspondants du pave' sont positionne's.  | */
/* |            Le parame`tre haut donne la position de la limite de    | */
/* |            page exprime'e en pixels.                               | */
/* |            Si un pave' de'borde verticalement de sa boi^te         | */
/* |            englobante et que ce pave' n'est pas se'cable, alors    | */
/* |            la boi^te englobante est conside're'e comme coupe'e par | */
/* |            la limite de page.                                      | */
/* |            Quand la limite de page coupe un pave' non se'cable la  | */
/* |            limite de page est alors remonte'e pour rejeter hors    | */
/* |            page le pave' et le processus est repris au de'but.     | */
/* |            Dans ce cas, au retour de la fonction le parame`tre     | */
/* |            haut est modifie' et modifpage est poste' a` Vrai.      | */
/* ---------------------------------------------------------------------- */


#ifdef __STDC__
static void         CoupSurPage (PtrAbstractBox Pv, int *haut, boolean * modifpage)

#else  /* __STDC__ */
static void         CoupSurPage (Pv, haut, modifpage)
PtrAbstractBox             Pv;
int                *haut;
boolean            *modifpage;

#endif /* __STDC__ */

{
   int                 org;
   PtrAbstractBox             pavefils;
   PtrBox            box1;
   PtrBox            prec;
   PtrBox            first;
   boolean             retour;

   /* A priori la limite de page n'est pas deplacee */
   *modifpage = False;
   /* et il faut examiner les paves fils */

   if (!Pv->AbDead && Pv->AbBox != NULL)
     {

	/* On verifie les limites de la boite du pave */
	box1 = Pv->AbBox;

	/* --- La boite du pave est coupee en lignes ------------------------- */
	if (box1->BxType == BoSplit)
	  {
	     prec = box1;
	     box1 = box1->BxNexChild;
	     retour = True;
	     first = box1;	/* memorise la premiere boite */
	     /* A priori la boite est dans la page */
	     Pv->AbAfterPageBreak = False;
	     Pv->AbOnPageBreak = False;
	     /* Ce n'est pas la peine de continuer le calcul */
	     /* des coupures de boites quand la limite de    */
	     /* page est deplacee */
	     while (!*modifpage && retour && box1 != NULL)
	       {
		  /* Origine de la boite de coupure */
		  org = box1->BxYOrg;
		  if (org + box1->BxHeight <= *haut)
		     ;		/* La boite est a l'interieur de la page */
		  else if (org >= *haut)
		    {
		       /* Il faut memoriser la boite de coupure coupee */
		       box1->BxAbstractBox->AbBox->BxMoved = box1;
		       if (box1 == first)
			  /* La boite est hors page */
			  HorsDeLaPage (Pv, haut, modifpage);
		       else if (prec->BxType == BoDotted && prec->BxNSpaces == 0)
			 {
			    /* La derniere boite de la page est hyphenee */
			    /* et n'est pas secable sur un blanc */
			    if (prec == first)
			       /* Le pave est note hors de la page */
			       HorsDeLaPage (Pv, haut, modifpage);
			    else
			      {
				 /* On deplace la limite de page */
				 *haut = prec->BxYOrg;
				 *modifpage = True;
			      }
			 }
		       else
			  /* La boite est sur la limite de page */
			  SurLaPage (Pv, haut, modifpage);
		       retour = False;
		    }
		  else
		    {
		       /* La boite est sur la limite de page */
		       /* deplace la limite de page sur l'origine de la boite */
		       *haut = org;
		       *modifpage = True;
		    }

		  prec = box1;
		  box1 = box1->BxNexChild;
	       }		/*while */
	  }
	/* --- Dans le cas genenral ----------------------------------------- */
	else
	  {
	     /* Si la boite composee n'est pas eclatee */
	     if (box1->BxType != BoGhost)
	       {
		  /* Origine de la boite de coupure */
		  org = box1->BxYOrg;
		  if (org + box1->BxHeight <= *haut)
		    {
		       /* La boite est dans la page */
		       Pv->AbAfterPageBreak = False;
		       Pv->AbOnPageBreak = False;
		    }
		  else if (org >= *haut)
		     /* La boite est hors page */
		     HorsDeLaPage (Pv, haut, modifpage);
		  else if (!Pv->AbAcceptPageBreak || Pv->AbLeafType == LtText || Pv->AbLeafType == LtSymbol)
		    {
		       /* La boite est sur la limite de page mais non secable */
		       /* deplace la limite de page sur l'origine de la boite */
		       *haut = org;
		       *modifpage = True;
		    }
		  else if (Pv->AbVertEnclosing)
		    {
		       /* La boite est sur la limite de page, secable et englobee */
		       if (Pv->AbFirstEnclosed == NULL)
			  /* On attend la boite terminale pour remonter l'indicateur */
			  SurLaPage (Pv, haut, modifpage);
		    }
		  else
		    {
		       /* La boite est sur la limite de page, secable et non englobee */
		       Pv->AbOnPageBreak = True;
		       Pv->AbAfterPageBreak = False;
		    }
	       }		/*if != BoGhost */
	     else
	       {
		  Pv->AbOnPageBreak = False;
		  Pv->AbAfterPageBreak = False;
	       }

	     /* On traite les paves fils */
	     pavefils = Pv->AbFirstEnclosed;
	     /* Ce n'est pas la peine de continuer le calcul */
	     /* des coupures de boites quand la limite de    */
	     /* page est deplacee */
	     while (pavefils != NULL && !*modifpage)
	       {
#ifdef __COLPAGE__
		  /* on saute les paves de colonnes pour arriver a la */
		  /* derniere car c'est toujours pour la derniere */
		  /* colonne qu'on evalue la coupure */
		  while (pavefils->AbElement->ElTypeNumber == ord (PageBreak) + 1
			 && (pavefils->AbElement->ElPageType == ColBegin
			  || pavefils->AbElement->ElPageType == ColComputed
		       || pavefils->AbElement->ElPageType == ColUser
			 || pavefils->AbElement->ElPageType == ColGroup)
			 && pavefils->AbNext != NULL)
		     pavefils = pavefils->AbNext;
#endif /* __COLPAGE__ */
		  CoupSurPage (pavefils, haut, modifpage);
		  /* On passe au suivant */
		  pavefils = pavefils->AbNext;
	       }
	  }			/*else */

     }
}				/* CoupSurPage */

#ifdef __COLPAGE__
/* ---------------------------------------------------------------------- */
/* |    RazPage remet a faux les boolens AbOnPageBreak et PageHorsPage     | */
/* |            dans tous les paves du sous-arbre pAb                  | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         RazPage (PtrAbstractBox pAb)
#else  /* __STDC__ */
static void         RazPage (pAb)
PtrAbstractBox             pAb;

#endif /* __STDC__ */
{
   PtrAbstractBox             pP;

   pP = pAb;
   pP->AbOnPageBreak = False;
   pP->AbAfterPageBreak = False;
   pP = pP->AbFirstEnclosed;
   while (pP != NULL)
     {
	RazPage (pP);
	pP = pP->AbNext;
     }
}
#endif /* __COLPAGE__ */


/* ---------------------------------------------------------------------- */
/* |    MarqueCoupure teste la position d'un pave' par rapport a` la    | */
/* |            limite de page. Elle rend Vrai si la boi^te du pave'    | */
/* |            est incluse dans la page. Sinon le pave' est coupe'     | */
/* |            par la limite ou se situe au dela` de la limite et les  | */
/* |            indicateurs correspondants du pave' sont positionne's.  | */
/* |            Le parame`tre page donne la position de la limite de    | */
/* |            page.                                                   | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
boolean             MarqueCoupure (PtrAbstractBox Pv, int *page)
#else  /* __STDC__ */
boolean             MarqueCoupure (Pv, page)
PtrAbstractBox             Pv;
int                *page;

#endif /* __STDC__ */
{
   int                 haut;
   boolean             result;

   haut = PixelValue (*page, UnPoint, Pv);
#ifdef __COLPAGE__
   /* comme dans le cas des colonnes, il y a des branches de l'image */
   /* qui ne sont pas examinees et mises a jour par CoupSurPage, */
   /* on parcourt l'arbre pour mettre AbOnPageBreak et AbAfterPageBreak a faux */
   RazPage (Pv);
#endif /* __COLPAGE__ */
   result = True;
   /* Tant que la limite de page change on recalcule */
   /* quelles sont les boites coupees */
   while (result)
      CoupSurPage (Pv, &haut, &result);
   result = !Pv->AbOnPageBreak;
   /* Faut-il traduire la hauteur de page ? */
   *page = PixelValue (haut, UnPoint, Pv);
   return result;
}				/*MarqueCoupure */


/* ---------------------------------------------------------------------- */
/* |    HautCoupure indique quelles sont les conditions de coupure du   | */
/* |            pave' passe' en parame`tre :                            | */
/* |            - ht = hauteur de la boi^te du pave'.                   | */
/* |            - pos = position de la boite du pave dans la page.      | */
/* |            - nbcar = nombre de caracte`res du pave' qui entrent    | */
/* |            dans la page s'il est de type texte, sinon le volume du | */
/* |            pave'.                                                  | */
/* |            Les hauteurs sont exprime'es suivant la valeur du       | */
/* |            parame`tre EnPt, en points typographiques (valeur Vrai) | */
/* |            ou en unite's logiques (Valeur Faux).                   | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                HautCoupure (PtrAbstractBox Pv, boolean EnPt, int *ht, int *pos, int *nbcar)

#else  /* __STDC__ */
void                HautCoupure (Pv, EnPt, ht, pos, nbcar)
PtrAbstractBox             Pv;
boolean             EnPt;
int                *ht;
int                *pos;
int                *nbcar;

#endif /* __STDC__ */

{
   int                 haut;
   int                 org, i;
   int                 hfont;
   PtrBox            box1;
   PtrBox            pBo1;
   PtrTextBuffer      adbuff;
   boolean             encore;


   *nbcar = 0;
   *pos = 0;
   *ht = 0;
   /* On calcule la position et la hauteur du pave */
   box1 = Pv->AbBox;
   if (box1 != NULL)
     {

	/* La boite du pave est coupee en lignes */
	if (box1->BxType == BoSplit)
	  {
	     pBo1 = box1->BxNexChild;
	     org = pBo1->BxYOrg;

	     /* On recherche la derniere boite de coupure */
	     while (pBo1->BxNexChild != NULL)
		pBo1 = pBo1->BxNexChild;

	     haut = pBo1->BxYOrg + pBo1->BxHeight - org;

	     /* BxMoved est la boite sur laquelle passe la limite */
	     box1 = box1->BxMoved;

	     /* Nombre de caracteres qui entrent dans la page */
	     if (Pv->AbOnPageBreak && box1 != NULL)
	       {
		  *nbcar = box1->BxIndChar;
		  /* Il ne faut pas couper le dernier mot d'une page     */
		  /* donc si la boite precedente est de type BtAvectrait */
		  /* la limite de la page est deplacee sur le blanc qui  */
		  /* precede ce mot */
		  if (*nbcar != 0)
		     if (box1->BxPrevious->BxType == BoDotted)
			if (box1->BxPrevious->BxNSpaces != 0)
			  {
			     /* On recheche en arriere le blanc precedent */
			     adbuff = box1->BxBuffer;
			     i = box1->BxFirstChar - 1;
			     encore = True;
			     while (encore)
				if (adbuff->BuContent[i] == ' ')
				  {
				     /* On a trouve le blanc */
				     encore = False;
				     /* Debute le nouveau mot au caractere suivant */
				     (*nbcar)++;
				  }
				else
				  {
				     (*nbcar)--;
				     if (i == 0)
					if (adbuff->BuPrevious != NULL)
					  {
					     adbuff = adbuff->BuPrevious;
					     i = adbuff->BuLength - 1;
					  }
					else
					   /* On arrete */
					   encore = False;
				     else
					i--;
				  }	/*else */
			  }
	       }
	  }
	/* La boite du pave est eclatee sur plusieurs lignes */
	else if (box1->BxType == BoGhost)
	  {
	     /* Il faut descendre tant que l'on a des boites eclatees */
	     while (box1->BxType == BoGhost)
		box1 = box1->BxAbstractBox->AbFirstEnclosed->AbBox;
	     /* On prend la position de la premiere boite */
	     if (box1->BxType == BoSplit)
		/* Il faut prendre la position de la 1ere boite de coupure */
		box1 = box1->BxNexChild;
	     org = box1->BxYOrg;
	     haut = 0;
	     while (box1 != NULL)
	       {
		  /* On prend la limite inferieur */
		  if (box1->BxType == BoPiece)
		     /* il faut aller chercher la derniere boite de coupure */
		     while (box1->BxNexChild != NULL)
			box1 = box1->BxNexChild;
		  i = box1->BxYOrg + box1->BxHeight;
		  if (i > haut)
		     haut = i;
		  if (box1->BxAbstractBox->AbNext == NULL)
		     box1 = NULL;
		  else
		     box1 = box1->BxAbstractBox->AbNext->AbBox;
	       }
	     haut -= org;	/* La hauteur de la boite eclatee */
	  }
	else
	  {
	     org = box1->BxYOrg;
	     haut = box1->BxHeight;
	  }

	/* On traduit les valeurs pixel dans l'unite demandee */
	if (EnPt)
	  {
	     *pos = PixelEnPt (org, 0);
	     *ht = PixelEnPt (haut, 0);
	  }
	else
	  {
	     hfont = FontHeight (box1->BxFont);
	     *pos = org * 10 / hfont;
	     *ht = haut * 10 / hfont;
	  }
     }
}				/* HautCoupure */


/* ---------------------------------------------------------------------- */
/* |    PosPavePt rend la position en points typographiques du pave     | */
/* |            passe' en parametre.                                    | */
/* |            Cette position est relative au pave englobant.          | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                PosPavePt (PtrAbstractBox Pv, int *xCoord, int *yCoord)
#else  /* __STDC__ */
void                PosPavePt (Pv, xCoord, yCoord)
PtrAbstractBox             Pv;
int                *xCoord;
int                *yCoord;

#endif /* __STDC__ */
{

   int                 x, y;

   if (Pv != NULL)
      if (Pv->AbBox != NULL)
	{
	   x = Pv->AbBox->BxXOrg;
	   y = Pv->AbBox->BxYOrg;
	   Pv = Pv->AbEnclosing;
	   if (Pv != NULL)
	     {
		/* decalage par rapport a l'englobant */
		x -= Pv->AbBox->BxXOrg;
		y -= Pv->AbBox->BxXOrg;
	     }
	   *xCoord = PixelEnPt (x, 0);
	   *yCoord = PixelEnPt (y, 1);
	}
}


/* ---------------------------------------------------------------------- */
/* |    DimPavePt rend les dimensions en points typographiques du pave  | */
/* |            passe' en parametre.                                    | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                DimPavePt (PtrAbstractBox Pv, int *hauteur, int *largeur)
#else  /* __STDC__ */
void                DimPavePt (Pv, hauteur, largeur)
PtrAbstractBox             Pv;
int                *hauteur;
int                *largeur;

#endif /* __STDC__ */
{

   if (Pv != NULL)
      if (Pv->AbBox != NULL)
	{
	   *hauteur = PixelEnPt (Pv->AbBox->BxHeight, 0);
	   *largeur = PixelEnPt (Pv->AbBox->BxWidth, 1);
	}
}


/* ---------------------------------------------------------------------- */
/* |    MarqueAPlacer marque le sous-arbre des paves en cours de        | */
/* |            placement.                                              | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                MarqueAPlacer (PtrAbstractBox pAb, boolean EnX, boolean EnY)
#else  /* __STDC__ */
void                MarqueAPlacer (pAb, EnX, EnY)
PtrAbstractBox             pAb;
boolean             EnX;
boolean             EnY;

#endif /* __STDC__ */
{
   PtrAbstractBox             pavefils;
   PtrBox            pBox;

   pBox = pAb->AbBox;
   if (pBox == NULL)
      return;

   /* Les boites englobees des boites elastiques */
   /* sont toujours placees en absolue           */
   if (pBox->BxHorizFlex || pBox->BxXOutOfStruct)
      EnX = False;
   if (pBox->BxVertFlex || pBox->BxYOutOfStruct)
      EnY = False;

   if (pBox->BxType != BoSplit)
     {
	pBox->BxXToCompute = EnX;
	pBox->BxYToCompute = EnY;
     }

   /* Marque les paves englobes */
   pavefils = pAb->AbFirstEnclosed;
   while (pavefils != NULL)
     {
	MarqueAPlacer (pavefils, EnX, EnY);
	pavefils = pavefils->AbNext;
     }
}

/* End Of Module pos */
