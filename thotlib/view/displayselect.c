
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
/* |    VisuPartiel trace le contour de la boite de texte ibox          | */
/* |            dans les limitesde la fenetre entre x1 et x2.           | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         VisuPartiel (int frame, int x1, int x2, PtrBox ibox)
#else  /* __STDC__ */
static void         VisuPartiel (frame, x1, x2, ibox)
int                 frame;
int                 x1;
int                 x2;
PtrBox            ibox;

#endif /* __STDC__ */
{
   int                 larg, haut;
   int                 yh, h;
   PtrBox            boxmere;
   ViewFrame            *pFe1;
   PtrAbstractBox             pPa1;

   pFe1 = &FntrTable[frame - 1];
   if (ibox->BxAbstractBox != NULL)
      /* On limite la visibilite de la selection aux portions de texte */
      /* affichees dans les paragraphes.                               */
     {
	/* Si on holophraste la racine du document */
	/* le texte n'a pas de pave englobant */
	if (ibox->BxAbstractBox->AbEnclosing == NULL)
	   boxmere = ibox;
	else
	  {
	     boxmere = ibox->BxAbstractBox->AbEnclosing->AbBox;
	     while (boxmere->BxType == BoGhost)
	       {
		  pPa1 = boxmere->BxAbstractBox;
		  if (pPa1->AbEnclosing == NULL)
		     boxmere = ibox;
		  else
		     boxmere = pPa1->AbEnclosing->AbBox;
	       }
	  }
	/* clipping par rapport a la boite englobante */
	haut = boxmere->BxYOrg + boxmere->BxHeight - pFe1->FrYOrg;
	/* +2 pour le curseur de fin de ligne */
	larg = boxmere->BxXOrg + boxmere->BxWidth + 2 - pFe1->FrXOrg;

	yh = ibox->BxYOrg - pFe1->FrYOrg;
	h = ibox->BxHeight;
	if (yh > haut)
	   h = 0;
	else if (yh + h > haut)
	   h = haut - yh;

	x1 = x1 + ibox->BxXOrg - pFe1->FrXOrg;
	if (x1 > larg)
	   larg = 0;
	else
	  {
	     x2 = x2 + ibox->BxXOrg - pFe1->FrXOrg;
	     if (x2 > larg)
		larg -= x1;
	     else
		larg = x2 - x1;
	  }
	Invideo (frame, larg, h, x1, yh);
     }
}


/* ---------------------------------------------------------------------- */
/* |    VisuBoite trace le contour de la boite ibox dans les limites de | */
/* |            la fenetre.                                             | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         VisuBoite (int frame, PtrBox ibox, int pointselect)
#else  /* __STDC__ */
static void         VisuBoite (frame, ibox, pointselect)
int                 frame;
PtrBox            ibox;
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
   ViewFrame            *pFe1;
   PtrAbstractBox             pPa1;


   if (ibox != NULL)
     {
	pFe1 = &FntrTable[frame - 1];
	pPa1 = ibox->BxAbstractBox;

	if (ibox->BxType == BoGhost
	    || (pPa1 != NULL
		&& ExceptTypeElem (ExcHighlightChildren, pPa1->AbElement->ElTypeNumber, pPa1->AbElement->ElSructSchema)))
	  {
	     /* -> La boite est eclatee (boite fantome) */
	     /* On visualise toutes les boites filles */
	     box1 = pPa1->AbFirstEnclosed->AbBox;
	     while (box1 != NULL)
	       {
		  VisuBoite (frame, box1, 0);
		  pPa1 = box1->BxAbstractBox;
		  if (pPa1->AbNext != NULL)
		     box1 = pPa1->AbNext->AbBox;
		  else
		     box1 = NULL;
	       }
	  }
	else if (ibox->BxType != BoSplit)
	  {
	     /* La boite est entiere */
	     xgauche = ibox->BxXOrg - pFe1->FrXOrg;
	     yhaut = ibox->BxYOrg - pFe1->FrYOrg;
	     ybas = yhaut + ibox->BxHeight;
	     xdroit = xgauche + ibox->BxWidth;
	     xmin = xgauche + ibox->BxWidth / 2;
	     yhmoyen = yhaut + ibox->BxHeight / 2;

	     if (pPa1 == NULL)
		/* C'est une boite sans pave */
		Invideo (frame, xdroit - xgauche, ybas - yhaut, xgauche, yhaut);
	     else if (pPa1->AbLeafType == LtPlyLine && ibox->BxNChars > 1)
	       {
		  /* C'est une boite polyline */
		  /* On marque le(s) point(s) caracteristique(s) de la polyline */
		  /* si la polyline contient au moins 1 point (effectif) */
		  adbuff = ibox->BxBuffer;
		  xgauche = ibox->BxXOrg - pFe1->FrXOrg;
		  yhaut = ibox->BxYOrg - pFe1->FrYOrg;
		  j = 1;
		  nb = ibox->BxNChars;
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
				   xgauche + PtEnPixel (adbuff->BuPoints[j].XCoord / 1000, 0) - 2,
				   yhaut + PtEnPixel (adbuff->BuPoints[j].YCoord / 1000, 1) - 2);
		       j++;
		    }
	       }
	     else if (pPa1->AbLeafType == LtPicture)
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
	     else if (pPa1->AbLeafType == LtGraphics && pPa1->AbVolume != 0)
		/* C'est une boite graphique */
		/* On marque en noir les points caracteristiques de la boite */
		switch (pPa1->AbRealShape)
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
	     box1 = ibox->BxNexChild;
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
void                MajPavSelect (int frame, PtrAbstractBox adpave, boolean Etat)
#else  /* __STDC__ */
void                MajPavSelect (frame, adpave, Etat)
int                 frame;
PtrAbstractBox             adpave;
boolean             Etat;

#endif /* __STDC__ */
{
   PtrAbstractBox             pavefils;
   ViewFrame            *pFe1;

   if (adpave != NULL)
     {
	/* Le pave est selectionne */
	if (adpave->AbSelected)
	  {
	     pFe1 = &FntrTable[frame - 1];
	     /* On ne visualise pas les bornes de la selection */
	     if (pFe1->FrSelectionBegin.VsBox == NULL ||
		 pFe1->FrSelectionEnd.VsBox == NULL)
		VisuBoite (frame, adpave->AbBox, 0);
	     else if (adpave != pFe1->FrSelectionBegin.VsBox->BxAbstractBox &&
		      adpave != pFe1->FrSelectionEnd.VsBox->BxAbstractBox)
		VisuBoite (frame, adpave->AbBox, 0);
	     adpave->AbSelected = Etat;
	  }
	else
	   /* Sinon on parcours le sous-arbre */
	  {
	     pavefils = adpave->AbFirstEnclosed;
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
   PtrBox            ibox;
   PtrAbstractBox             adpave;
   ViewFrame            *pFe1;
   ViewSelection            *pMa1;
   PtrBox            pBo1;
   ViewSelection            *pMa2;

   /* On ne visualise la selection que si la selection est coherente */
   pFe1 = &FntrTable[frame - 1];
   pMa1 = &pFe1->FrSelectionBegin;
   if (pMa1->VsBox != NULL && pFe1->FrSelectionEnd.VsBox != NULL)
     {
	/* Est-ce que les marques de selection sont dans la meme boite ? */
	if (pFe1->FrSelectionEnd.VsBox == pMa1->VsBox)
	   if (!StructSelectionMode && pFe1->FrSelectOnePosition)
	      /* on ne visualise que la position */
	      VisuPartiel (frame, pMa1->VsXPos, pMa1->VsXPos + 2, pMa1->VsBox);
	   else if (pMa1->VsBuffer == NULL
		    || (pMa1->VsBox->BxNChars == 0 && pMa1->VsBox->BxType == BoComplete))
	      VisuBoite (frame, pMa1->VsBox, pMa1->VsIndBox);
	   else
	      VisuPartiel (frame, pMa1->VsXPos, pFe1->FrSelectionEnd.VsXPos, pMa1->VsBox);
	else
	   /* Les marques de selection sont dans deux boites differentes */
	   /* Si les deux bornes de la selection sont compatibles */
	  {
	     adpave = NULL;
	     pBo1 = pMa1->VsBox;
	     if (pMa1->VsBuffer == NULL || pBo1->BxNChars == 0)
		VisuBoite (frame, pMa1->VsBox, 0);
	     else
	       {
		  adpave = pBo1->BxAbstractBox;
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
		       ibox = pBo1->BxNexChild;
		       while (ibox != NULL &&
			      ibox != pFe1->FrSelectionEnd.VsBox)
			 {
			    if (ibox->BxNChars > 0)
			       VisuBoite (frame, ibox, 0);
			    ibox = ibox->BxNexChild;
			 }
		    }
	       }
	     pMa2 = &pFe1->FrSelectionEnd;
	     pBo1 = pMa2->VsBox;
	     if (pMa2->VsBuffer == NULL || pBo1->BxNChars == 0)
		VisuBoite (frame, pMa2->VsBox, 0);
	     else
	       {
		  /* Parcours les boites coupees soeurs */
		  if ((pBo1->BxType == BoPiece || pBo1->BxType == BoDotted)
		      && pBo1->BxAbstractBox != adpave)
		    {
		       ibox = pBo1->BxAbstractBox->AbBox->BxNexChild;
		       while (ibox != NULL &&
			      ibox != pMa2->VsBox)
			 {
			    VisuBoite (frame, ibox, 0);
			    ibox = ibox->BxNexChild;
			 }
		    }
		  VisuPartiel (frame, 0, pMa2->VsXPos, pMa2->VsBox);
	       }
	  }
	/* Bascule l'indicateur de la selection allumee */
	pFe1->FrSelectShown = !pFe1->FrSelectShown;

	MajPavSelect (frame, pFe1->FrAbstractBox, Etat);
     }
   else if (Etat == False)
      /* Annule la selection meme s'il n'y a plus de boite selectionnee */
      pFe1->FrSelectShown = False;
}
/* End Of Module visu */
