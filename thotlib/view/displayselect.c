
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

/*
   visu.c : visualisation des Selections.

   I. Vatton - Mai 87
   IV : Aout 92 coupure des mots
   IV : Juin 93 polylines
 */

#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"

#define EXPORT extern
#include "img.var"
#include "environ.var"
#include "select.var"
#include "edit.var"

#include "es.f"
#include "visu.f"
#include "font.f"
#include "except.f"

/* ---------------------------------------------------------------------- */
/* |    VisuPartiel trace le contour de la boite de texte pBox          | */
/* |            dans les limitesde la fenetre entre x1 et x2.           | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         VisuPartiel (int frame, int x1, int x2, PtrBox pBox)
#else  /* __STDC__ */
static void         VisuPartiel (frame, x1, x2, pBox)
int                 frame;
int                 x1;
int                 x2;
PtrBox            pBox;

#endif /* __STDC__ */
{
   int                 larg, haut;
   int                 yh, h;
   PtrBox            boxmere;
   ViewFrame            *pFrame;
   PtrAbstractBox             pAbbox1;

   pFrame = &FntrTable[frame - 1];
   if (pBox->BxAbstractBox != NULL)
      /* On limite la visibilite de la selection aux portions de texte */
      /* affichees dans les paragraphes.                               */
     {
	/* Si on holophraste la racine du document */
	/* le texte n'a pas de pave englobant */
	if (pBox->BxAbstractBox->AbEnclosing == NULL)
	   boxmere = pBox;
	else
	  {
	     boxmere = pBox->BxAbstractBox->AbEnclosing->AbBox;
	     while (boxmere->BxType == BoGhost)
	       {
		  pAbbox1 = boxmere->BxAbstractBox;
		  if (pAbbox1->AbEnclosing == NULL)
		     boxmere = pBox;
		  else
		     boxmere = pAbbox1->AbEnclosing->AbBox;
	       }
	  }
	/* clipping par rapport a la boite englobante */
	haut = boxmere->BxYOrg + boxmere->BxHeight - pFrame->FrYOrg;
	/* +2 pour le curseur de fin de ligne */
	larg = boxmere->BxXOrg + boxmere->BxWidth + 2 - pFrame->FrXOrg;

	yh = pBox->BxYOrg - pFrame->FrYOrg;
	h = pBox->BxHeight;
	if (yh > haut)
	   h = 0;
	else if (yh + h > haut)
	   h = haut - yh;

	x1 = x1 + pBox->BxXOrg - pFrame->FrXOrg;
	if (x1 > larg)
	   larg = 0;
	else
	  {
	     x2 = x2 + pBox->BxXOrg - pFrame->FrXOrg;
	     if (x2 > larg)
		larg -= x1;
	     else
		larg = x2 - x1;
	  }
	Invideo (frame, larg, h, x1, yh);
     }
}


/* ---------------------------------------------------------------------- */
/* |    VisuBoite trace le contour de la boite pBox dans les limites de | */
/* |            la fenetre.                                             | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         VisuBoite (int frame, PtrBox pBox, int pointselect)
#else  /* __STDC__ */
static void         VisuBoite (frame, pBox, pointselect)
int                 frame;
PtrBox            pBox;
int                 pointselect;

#endif /* __STDC__ */
{
   int                 xgauche, yhaut;
   int                 xmin;
   int                 xdroit, ybas, i;
   int                 yhmoyen;
   int                 j, nb;
   PtrTextBuffer      adbuff;
   PtrBox            box1;
   ViewFrame            *pFrame;
   PtrAbstractBox             pAbbox1;


   if (pBox != NULL)
     {
	pFrame = &FntrTable[frame - 1];
	pAbbox1 = pBox->BxAbstractBox;

	if (pBox->BxType == BoGhost
	    || (pAbbox1 != NULL
		&& TypeHasException (ExcHighlightChildren, pAbbox1->AbElement->ElTypeNumber, pAbbox1->AbElement->ElSructSchema)))
	  {
	     /* -> La boite est eclatee (boite fantome) */
	     /* On visualise toutes les boites filles */
	     box1 = pAbbox1->AbFirstEnclosed->AbBox;
	     while (box1 != NULL)
	       {
		  VisuBoite (frame, box1, 0);
		  pAbbox1 = box1->BxAbstractBox;
		  if (pAbbox1->AbNext != NULL)
		     box1 = pAbbox1->AbNext->AbBox;
		  else
		     box1 = NULL;
	       }
	  }
	else if (pBox->BxType != BoSplit)
	  {
	     /* La boite est entiere */
	     xgauche = pBox->BxXOrg - pFrame->FrXOrg;
	     yhaut = pBox->BxYOrg - pFrame->FrYOrg;
	     ybas = yhaut + pBox->BxHeight;
	     xdroit = xgauche + pBox->BxWidth;
	     xmin = xgauche + pBox->BxWidth / 2;
	     yhmoyen = yhaut + pBox->BxHeight / 2;

	     if (pAbbox1 == NULL)
		/* C'est une boite sans pave */
		Invideo (frame, xdroit - xgauche, ybas - yhaut, xgauche, yhaut);
	     else if (pAbbox1->AbLeafType == LtPlyLine && pBox->BxNChars > 1)
	       {
		  /* C'est une boite polyline */
		  /* On marque le(s) point(s) caracteristique(s) de la polyline */
		  /* si la polyline contient au moins 1 point (effectif) */
		  adbuff = pBox->BxBuffer;
		  xgauche = pBox->BxXOrg - pFrame->FrXOrg;
		  yhaut = pBox->BxYOrg - pFrame->FrYOrg;
		  j = 1;
		  nb = pBox->BxNChars;
		  for (i = 1; i < nb; i++)
		    {
		       if (j >= adbuff->BuLength)
			 {
			    if (adbuff->BuNext != NULL)
			      {
				 /* Changement de buffer */
				 adbuff = adbuff->BuNext;
				 j = 0;
			      }
			 }
		       if (pointselect == 0 || pointselect == i)
			  Invideo (frame, HANDLE_WIDTH, HANDLE_WIDTH,
				   xgauche + PointToPixel (adbuff->BuPoints[j].XCoord / 1000) - 2,
				   yhaut + PointToPixel (adbuff->BuPoints[j].YCoord / 1000) - 2);
		       j++;
		    }
	       }
	     else if (pAbbox1->AbLeafType == LtPicture)
	       {
		  /* 4 points caracteristiques */
		  Invideo (frame, HANDLE_WIDTH, HANDLE_WIDTH, xgauche - 2, yhaut - 2);
		  Invideo (frame, HANDLE_WIDTH, HANDLE_WIDTH, xmin - 2, yhaut - 2);
		  Invideo (frame, HANDLE_WIDTH, HANDLE_WIDTH, xdroit - 3, yhaut - 2);
		  Invideo (frame, HANDLE_WIDTH, HANDLE_WIDTH, xgauche - 2, yhmoyen - 2);
		  Invideo (frame, HANDLE_WIDTH, HANDLE_WIDTH, xdroit - 3, yhmoyen - 2);
		  Invideo (frame, HANDLE_WIDTH, HANDLE_WIDTH, xgauche - 2, ybas - 3);
		  Invideo (frame, HANDLE_WIDTH, HANDLE_WIDTH, xmin - 2, ybas - 3);
		  Invideo (frame, HANDLE_WIDTH, HANDLE_WIDTH, xdroit - 3, ybas - 3);
	       }
	     else if (pAbbox1->AbLeafType == LtGraphics && pAbbox1->AbVolume != 0)
		/* C'est une boite graphique */
		/* On marque en noir les points caracteristiques de la boite */
		switch (pAbbox1->AbRealShape)
		      {
			 case ' ':
			 case 'R':
			 case '0':
			 case '1':
			 case '2':
			 case '3':
			 case '4':
			 case '5':
			 case '6':
			 case '7':
			 case '8':
			    /* 8 points caracteristiques */
			    Invideo (frame, HANDLE_WIDTH, HANDLE_WIDTH, xgauche - 2, yhaut - 2);
			    Invideo (frame, HANDLE_WIDTH, HANDLE_WIDTH, xmin - 2, yhaut - 2);
			    Invideo (frame, HANDLE_WIDTH, HANDLE_WIDTH, xdroit - 3, yhaut - 2);
			    Invideo (frame, HANDLE_WIDTH, HANDLE_WIDTH, xgauche - 2, yhmoyen - 2);
			    Invideo (frame, HANDLE_WIDTH, HANDLE_WIDTH, xdroit - 3, yhmoyen - 2);
			    Invideo (frame, HANDLE_WIDTH, HANDLE_WIDTH, xgauche - 2, ybas - 3);
			    Invideo (frame, HANDLE_WIDTH, HANDLE_WIDTH, xmin - 2, ybas - 3);
			    Invideo (frame, HANDLE_WIDTH, HANDLE_WIDTH, xdroit - 3, ybas - 3);
			    break;
			 case 'C':
			 case 'L':
			 case 'c':
			 case 'P':
			 case 'Q':
			    /* 4 points caracteristiques */
			    Invideo (frame, HANDLE_WIDTH, HANDLE_WIDTH, xmin - 2, yhaut - 2);
			    Invideo (frame, HANDLE_WIDTH, HANDLE_WIDTH, xgauche - 2, yhmoyen - 2);
			    Invideo (frame, HANDLE_WIDTH, HANDLE_WIDTH, xdroit - 3, yhmoyen - 2);
			    Invideo (frame, HANDLE_WIDTH, HANDLE_WIDTH, xmin - 2, ybas - 3);
			    break;
			 case 'W':
			    Invideo (frame, HANDLE_WIDTH, HANDLE_WIDTH, xgauche - 2, yhaut - 2);
			    Invideo (frame, HANDLE_WIDTH, HANDLE_WIDTH, xdroit - 3, yhaut - 2);
			    Invideo (frame, HANDLE_WIDTH, HANDLE_WIDTH, xdroit - 3, ybas - 2);
			    break;
			 case 'X':
			    Invideo (frame, HANDLE_WIDTH, HANDLE_WIDTH, xdroit - 3, yhaut - 2);
			    Invideo (frame, HANDLE_WIDTH, HANDLE_WIDTH, xdroit - 3, ybas - 2);
			    Invideo (frame, HANDLE_WIDTH, HANDLE_WIDTH, xgauche - 2, ybas - 2);
			    break;
			 case 'Y':
			    Invideo (frame, HANDLE_WIDTH, HANDLE_WIDTH, xdroit - 3, ybas - 2);
			    Invideo (frame, HANDLE_WIDTH, HANDLE_WIDTH, xgauche - 2, ybas - 2);
			    Invideo (frame, HANDLE_WIDTH, HANDLE_WIDTH, xgauche - 2, yhaut - 2);
			    break;
			 case 'Z':
			    Invideo (frame, HANDLE_WIDTH, HANDLE_WIDTH, xgauche - 2, ybas - 2);
			    Invideo (frame, HANDLE_WIDTH, HANDLE_WIDTH, xgauche - 2, yhaut - 2);
			    Invideo (frame, HANDLE_WIDTH, HANDLE_WIDTH, xdroit - 3, yhaut - 2);
			    break;

			 case 'h':
			 case '<':
			 case '>':
			    /* 2 points caracteristiques */
			    Invideo (frame, HANDLE_WIDTH, HANDLE_WIDTH, xgauche - 2, yhmoyen - 2);
			    Invideo (frame, HANDLE_WIDTH, HANDLE_WIDTH, xdroit - 3, yhmoyen - 2);
			    break;
			 case 't':
			    /* 3 points caracteristiques */
			    Invideo (frame, HANDLE_WIDTH, HANDLE_WIDTH, xgauche - 2, yhaut - 2);
			    Invideo (frame, HANDLE_WIDTH, HANDLE_WIDTH, xmin - 2, yhaut - 2);
			    Invideo (frame, HANDLE_WIDTH, HANDLE_WIDTH, xdroit - 3, yhaut - 2);
			    break;
			 case 'b':
			    /* 3 points caracteristiques */
			    Invideo (frame, HANDLE_WIDTH, HANDLE_WIDTH, xgauche - 2, ybas - 3);
			    Invideo (frame, HANDLE_WIDTH, HANDLE_WIDTH, xmin - 2, ybas - 3);
			    Invideo (frame, HANDLE_WIDTH, HANDLE_WIDTH, xdroit - 3, ybas - 3);
			    break;
			 case 'v':
			 case '^':
			 case 'V':
			    /* 2 points caracteristiques */
			    Invideo (frame, HANDLE_WIDTH, HANDLE_WIDTH, xmin - 2, yhaut - 2);
			    Invideo (frame, HANDLE_WIDTH, HANDLE_WIDTH, xmin - 2, ybas - 3);
			    break;
			 case 'l':
			    /* 3 points caracteristiques */
			    Invideo (frame, HANDLE_WIDTH, HANDLE_WIDTH, xgauche - 2, yhaut - 2);
			    Invideo (frame, HANDLE_WIDTH, HANDLE_WIDTH, xgauche - 2, yhmoyen - 2);
			    Invideo (frame, HANDLE_WIDTH, HANDLE_WIDTH, xgauche - 2, ybas - 3);
			    break;
			 case 'r':
			    /* 3 points caracteristiques */
			    Invideo (frame, HANDLE_WIDTH, HANDLE_WIDTH, xdroit - 3, yhaut - 2);
			    Invideo (frame, HANDLE_WIDTH, HANDLE_WIDTH, xdroit - 3, yhmoyen - 2);
			    Invideo (frame, HANDLE_WIDTH, HANDLE_WIDTH, xdroit - 3, ybas - 3);
			    break;
			 case '\\':
			 case 'O':
			 case 'e':
			    /* 2 points caracteristiques */
			    Invideo (frame, HANDLE_WIDTH, HANDLE_WIDTH, xgauche - 2, yhaut - 2);
			    Invideo (frame, HANDLE_WIDTH, HANDLE_WIDTH, xdroit - 3, ybas - 3);
			    break;
			 case '/':
			 case 'o':
			 case 'E':
			    /* 2 points caracteristiques */
			    Invideo (frame, HANDLE_WIDTH, HANDLE_WIDTH, xdroit - 3, yhaut - 2);
			    Invideo (frame, HANDLE_WIDTH, HANDLE_WIDTH, xgauche - 2, ybas - 3);
			    break;
			 default:
			    break;
		      }
	     /* C'est une boite d'un autre type */
	     else
		Invideo (frame, xdroit - xgauche, ybas - yhaut, xgauche, yhaut);
	  }
	else
	  {
	     /* La boite est coupee */
	     /* Calcul des points caracteristiques de la premiere boite coupee */
	     box1 = pBox->BxNexChild;
	     while (box1 != NULL)
	       {
		  VisuBoite (frame, box1, 0);
		  box1 = box1->BxNexChild;
	       }
	  }
     }
}


/* ---------------------------------------------------------------------- */
/* |    MajPavSelect parcourt l'arborescence pour basculer la mise en   | */
/* |            evidence de la selection et forcer le nouvel Etat de    | */
/* |            selection.                                              | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                MajPavSelect (int frame, PtrAbstractBox pAb, boolean Etat)
#else  /* __STDC__ */
void                MajPavSelect (frame, pAb, Etat)
int                 frame;
PtrAbstractBox             pAb;
boolean             Etat;

#endif /* __STDC__ */
{
   PtrAbstractBox             pavefils;
   ViewFrame            *pFrame;

   if (pAb != NULL)
     {
	/* Le pave est selectionne */
	if (pAb->AbSelected)
	  {
	     pFrame = &FntrTable[frame - 1];
	     /* On ne visualise pas les bornes de la selection */
	     if (pFrame->FrSelectionBegin.VsBox == NULL ||
		 pFrame->FrSelectionEnd.VsBox == NULL)
		VisuBoite (frame, pAb->AbBox, 0);
	     else if (pAb != pFrame->FrSelectionBegin.VsBox->BxAbstractBox &&
		      pAb != pFrame->FrSelectionEnd.VsBox->BxAbstractBox)
		VisuBoite (frame, pAb->AbBox, 0);
	     pAb->AbSelected = Etat;
	  }
	else
	   /* Sinon on parcours le sous-arbre */
	  {
	     pavefils = pAb->AbFirstEnclosed;
	     while (pavefils != NULL)
	       {
		  MajPavSelect (frame, pavefils, Etat);
		  pavefils = pavefils->AbNext;
	       }
	  }
     }
}


/* ---------------------------------------------------------------------- */
/* |    VisuSel visualise ou efface la selection courante dans la       | */
/* |            frame.                                          | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                VisuSel (int frame, boolean Etat)
#else  /* __STDC__ */
void                VisuSel (frame, Etat)
int                 frame;
boolean             Etat;

#endif /* __STDC__ */
{
   PtrBox            pBox;
   PtrAbstractBox             pAb;
   ViewFrame            *pFrame;
   ViewSelection            *pMa1;
   PtrBox            pBo1;
   ViewSelection            *pMa2;

   /* On ne visualise la selection que si la selection est coherente */
   pFrame = &FntrTable[frame - 1];
   pMa1 = &pFrame->FrSelectionBegin;
   if (pMa1->VsBox != NULL && pFrame->FrSelectionEnd.VsBox != NULL)
     {
	/* Est-ce que les marques de selection sont dans la meme boite ? */
	if (pFrame->FrSelectionEnd.VsBox == pMa1->VsBox)
	   if (!StructSelectionMode && pFrame->FrSelectOnePosition)
	      /* on ne visualise que la position */
	      VisuPartiel (frame, pMa1->VsXPos, pMa1->VsXPos + 2, pMa1->VsBox);
	   else if (pMa1->VsBuffer == NULL
		    || (pMa1->VsBox->BxNChars == 0 && pMa1->VsBox->BxType == BoComplete))
	      VisuBoite (frame, pMa1->VsBox, pMa1->VsIndBox);
	   else
	      VisuPartiel (frame, pMa1->VsXPos, pFrame->FrSelectionEnd.VsXPos, pMa1->VsBox);
	else
	   /* Les marques de selection sont dans deux boites differentes */
	   /* Si les deux bornes de la selection sont compatibles */
	  {
	     pAb = NULL;
	     pBo1 = pMa1->VsBox;
	     if (pMa1->VsBuffer == NULL || pBo1->BxNChars == 0)
		VisuBoite (frame, pMa1->VsBox, 0);
	     else
	       {
		  pAb = pBo1->BxAbstractBox;
		  /* Est-ce que la selection debute en fin de boite ? */
		  if (pMa1->VsXPos == pBo1->BxWidth)
		     VisuPartiel (frame, pMa1->VsXPos,
				  pBo1->BxWidth + 2, pMa1->VsBox);
		  else
		     VisuPartiel (frame, pMa1->VsXPos, pBo1->BxWidth,
				  pMa1->VsBox);
		  /* Parcours les boites coupees soeurs */
		  if (pBo1->BxType == BoPiece || pBo1->BxType == BoDotted)
		    {
		       pBox = pBo1->BxNexChild;
		       while (pBox != NULL &&
			      pBox != pFrame->FrSelectionEnd.VsBox)
			 {
			    if (pBox->BxNChars > 0)
			       VisuBoite (frame, pBox, 0);
			    pBox = pBox->BxNexChild;
			 }
		    }
	       }
	     pMa2 = &pFrame->FrSelectionEnd;
	     pBo1 = pMa2->VsBox;
	     if (pMa2->VsBuffer == NULL || pBo1->BxNChars == 0)
		VisuBoite (frame, pMa2->VsBox, 0);
	     else
	       {
		  /* Parcours les boites coupees soeurs */
		  if ((pBo1->BxType == BoPiece || pBo1->BxType == BoDotted)
		      && pBo1->BxAbstractBox != pAb)
		    {
		       pBox = pBo1->BxAbstractBox->AbBox->BxNexChild;
		       while (pBox != NULL &&
			      pBox != pMa2->VsBox)
			 {
			    VisuBoite (frame, pBox, 0);
			    pBox = pBox->BxNexChild;
			 }
		    }
		  VisuPartiel (frame, 0, pMa2->VsXPos, pMa2->VsBox);
	       }
	  }
	/* Bascule l'indicateur de la selection allumee */
	pFrame->FrSelectShown = !pFrame->FrSelectShown;

	MajPavSelect (frame, pFrame->FrAbstractBox, Etat);
     }
   else if (Etat == FALSE)
      /* Annule la selection meme s'il n'y a plus de boite selectionnee */
      pFrame->FrSelectShown = FALSE;
}
/* End Of Module visu */
