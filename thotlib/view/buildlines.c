
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

/*
   lig.c :  gestion de la mise en lignes des boites
   I. Vatton - Novembre 84
   France Logiciel numero de depot 88-39-001-00
   IV : Aout 92 dimensions minimales
   IV : Aout 92 coupure des mots
 */

#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"

#define EXPORT extern
#include "img.var"
/* Nombre maximum de coupures de mots consecutives */
static int          maxcoupuresvoisines = 2;

#include "dep.f"
#include "des.f"
#include "img.f"
#include "memory.f"
#include "sel.f"
#include "lig.f"
#include "font.f"
#include "hyphen.f"

#ifdef __STDC__
extern void         DefClip (int, int, int, int, int);

#else  /* __STDC__ */
extern void         DefClip ();

#endif /* __STDC__ */

/* ---------------------------------------------------------------------- */
/* |    Suivante rend l'adresse de la boite associee au pave vivant qui | */
/* |            suit pAb.                                            | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
PtrBox            Suivante (PtrAbstractBox pAb)

#else  /* __STDC__ */
PtrBox            Suivante (pAb)
PtrAbstractBox             pAb;

#endif /* __STDC__ */

{
   PtrAbstractBox             nextpave;
   boolean             boucle;
   PtrBox            result;

   /* On verifie que le pave existe toujours */
   if (pAb == NULL)
      result = NULL;
   else
     {
	nextpave = pAb->AbNext;
	boucle = TRUE;
	while (boucle)
	   if (nextpave == NULL)
	      /* Est-ce la derniere boite fille d'une boite eclatee */
	      if (pAb->AbEnclosing->AbBox->BxType == BoGhost)
		{
		   /* On remonte la hierarchie */
		   pAb = pAb->AbEnclosing;
		   nextpave = pAb->AbNext;
		}
	      else
		 boucle = FALSE;
	   else if (nextpave->AbDead)
	      nextpave = nextpave->AbNext;
	   else if (nextpave->AbBox == NULL)
	      nextpave = nextpave->AbNext;
	/* Est-ce un pave compose eclate ? */
	   else if (nextpave->AbBox->BxType == BoGhost)
	     {
		/* On descend la hierarchie */
		while (boucle)
		   if (nextpave->AbBox == NULL)
		      nextpave = nextpave->AbNext;
		   else if (nextpave->AbBox->BxType == BoGhost)
		      nextpave = nextpave->AbFirstEnclosed;
		   else
		      boucle = FALSE;
		boucle = TRUE;
		pAb = nextpave;
	     }
	   else
	      boucle = FALSE;
	if (nextpave == NULL)
	   result = NULL;
	else
	   result = nextpave->AbBox;
     }
   return result;
}


/* ---------------------------------------------------------------------- */
/* |    Precedente rend l'adresse de la boite associee au pave vivant   | */
/* |            qui precede pAb.                                     | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static PtrBox     Precedente (PtrAbstractBox pAb)

#else  /* __STDC__ */
static PtrBox     Precedente (pAb)
PtrAbstractBox             pAb;

#endif /* __STDC__ */

{
   PtrAbstractBox             nextpave;
   boolean             boucle;
   PtrBox            result;

   nextpave = pAb->AbPrevious;
   boucle = TRUE;
   while (boucle)
      if (nextpave == NULL)
	 /* Est-ce la derniere boite fille d'une boite eclatee */
	 if (pAb->AbEnclosing->AbBox->BxType == BoGhost)
	   {
	      /* On remonte la hierarchie */
	      pAb = pAb->AbEnclosing;
	      nextpave = pAb->AbPrevious;
	   }
	 else
	    boucle = FALSE;
      else if (nextpave->AbDead)
	 nextpave = nextpave->AbPrevious;
      else if (nextpave->AbBox == NULL)
	 nextpave = nextpave->AbPrevious;
   /* Est-ce un pave compose eclate ? */
      else if (nextpave->AbBox->BxType == BoGhost)
	{
	   /* On descend la hierarchie */
	   while (nextpave->AbBox->BxType == BoGhost)
	     {
		nextpave = nextpave->AbFirstEnclosed;
		/* On recherche le dernier pave fils */
		while (nextpave->AbNext != NULL)
		   nextpave = nextpave->AbNext;
	     }
	   pAb = nextpave;
	}
      else
	 boucle = FALSE;

   if (nextpave == NULL)
      result = NULL;
   else
      result = nextpave->AbBox;
   return result;
}


/* ---------------------------------------------------------------------- */
/* |    Ajuster calcule la largeur des blancs a` afficher dans la ligne | */
/* |            pour l'ajuster. La procedure met a` jour les positions  | */
/* |            en x et la largeur des boites incluses.                 | */
/* ---------------------------------------------------------------------- */


#ifdef __STDC__
static void         Ajuster (PtrBox boxmere, PtrLine adligne, int frame, boolean absoluEnX, boolean absoluEnY)

#else  /* __STDC__ */
static void         Ajuster (boxmere, adligne, frame, absoluEnX, absoluEnY)
PtrBox            boxmere;
PtrLine            adligne;
int                 frame;
boolean             absoluEnX;
boolean             absoluEnY;

#endif /* __STDC__ */

{
   PtrBox            box1, box2;
   int                 ilg, base;
   int                 nbblanc;


   nbblanc = 0;			/* nombre de blancs */
   ilg = 0;			/* taille des chaines de caracteres sans blanc */
   base = adligne->LiYOrg + adligne->LiHorizRef;

   /* Calcul de la longueur de la ligne en supprimant les blancs */
   if (adligne->LiFirstPiece != NULL)
      box2 = adligne->LiFirstPiece;
   else
      box2 = adligne->LiFirstBox;
   do				/* Boucle sur les boites de la ligne */
     {
	if (box2->BxType == BoSplit)
	   box1 = box2->BxNexChild;
	else
	   box1 = box2;
	if (box1->BxAbstractBox->AbLeafType == LtText)
	  {
	     box1->BxWidth -= box1->BxNSpaces * CarWidth (BLANC, box1->BxFont);
	     nbblanc += box1->BxNSpaces;
	  }

	ilg += box1->BxWidth;
	/* On passe a la boite suivante */
	box2 = Suivante (box1->BxAbstractBox);
     }

   /* Calcul de la taille des blancs en pixels */
   while (!(box1 == adligne->LiLastBox || box1 == adligne->LiLastPiece));
   if (adligne->LiXMax > ilg)
     {
	adligne->LiNPixels = adligne->LiXMax - ilg;
	if (nbblanc == 0)
	   adligne->LiSpaceWidth = 0;
	else
	   adligne->LiSpaceWidth = adligne->LiNPixels / nbblanc;
	adligne->LiNSpaces = nbblanc;
	/* Largeur minimum de ligne */
	adligne->LiMinLength = ilg + nbblanc * MIN_BLANC;
	adligne->LiNPixels -= adligne->LiSpaceWidth * nbblanc;	/* Pixels restants a repartir */
     }
   else
     {
	adligne->LiNPixels = 0;
	adligne->LiSpaceWidth = 0;
	adligne->LiNSpaces = 0;
	adligne->LiMinLength = ilg;
     }

   /* Met a jour l'origine, la base et la largeur de chaque boite */
   ilg = adligne->LiXOrg;
   if (absoluEnX)
      ilg += boxmere->BxXOrg;
   if (absoluEnY)
      base += boxmere->BxYOrg;

   nbblanc = adligne->LiNPixels;
   if (adligne->LiFirstPiece != NULL)
      box2 = adligne->LiFirstPiece;
   else
      box2 = adligne->LiFirstBox;

   /* Boucle sur les boites de la ligne */
   do
     {
	if (box2->BxType == BoSplit)
	   box1 = box2->BxNexChild;
	else
	   box1 = box2;
	DepOrgX (box1, NULL, ilg - box1->BxXOrg, frame);
	DepOrgY (box1, NULL, base - box1->BxHorizRef - box1->BxYOrg, frame);
	/* Repartition des pixels */
	if (box1->BxAbstractBox->AbLeafType == LtText)
	  {
	     if (nbblanc > box1->BxNSpaces)
		box1->BxNPixels = box1->BxNSpaces;
	     else
		box1->BxNPixels = nbblanc;

	     nbblanc -= box1->BxNPixels;
	     box1->BxSpaceWidth = adligne->LiSpaceWidth;
	     box1->BxWidth = box1->BxWidth + box1->BxNSpaces * adligne->LiSpaceWidth + box1->BxNPixels;
	  }

	ilg += box1->BxWidth;
	/* On passe a la boite suivante */
	box2 = Suivante (box1->BxAbstractBox);
     }
   while (!(box1 == adligne->LiLastBox || box1 == adligne->LiLastPiece));
}


/* ---------------------------------------------------------------------- */
/* |    Aligner aligne les bases des boites contenues dans la ligne     | */
/* |            designee. Decale de delta l'origine de chaque boite     | */
/* |            dans la ligne et indique que les boites ne sont pas     | */
/* |            justifiees.                                             | */
/* ---------------------------------------------------------------------- */


#ifdef __STDC__
void                Aligner (PtrBox boxmere, PtrLine adligne, int delta, int frame, boolean absoluEnX, boolean absoluEnY)

#else  /* __STDC__ */
void                Aligner (boxmere, adligne, delta, frame, absoluEnX, absoluEnY)
PtrBox            boxmere;
PtrLine            adligne;
int                 delta;
int                 frame;
boolean             absoluEnX;
boolean             absoluEnY;

#endif /* __STDC__ */

{
   PtrBox            box1, box2;
   int                 base, x;


   /* Base de la ligne */
   base = adligne->LiYOrg + adligne->LiHorizRef;
   x = delta;
   if (absoluEnX)
      x += boxmere->BxXOrg;
   if (absoluEnY)
      base += boxmere->BxYOrg;

   /* Premiere boite */
   if (adligne->LiFirstPiece != NULL)
      box2 = adligne->LiFirstPiece;
   else
      box2 = adligne->LiFirstBox;

   /* Boucle sur les boites de la ligne */
   do
     {
	if (box2->BxType == BoSplit)
	   box1 = box2->BxNexChild;
	else
	   box1 = box2;
	DepOrgX (box1, NULL, x - box1->BxXOrg, frame);
	DepOrgY (box1, NULL, base - box1->BxHorizRef - box1->BxYOrg, frame);
	x += box1->BxWidth;
	box2->BxSpaceWidth = 0;
	/* On passe a la boite suivante */
	box2 = Suivante (box1->BxAbstractBox);
     }
   while (!(box1 == adligne->LiLastBox || box1 == adligne->LiLastPiece));
}


/*debut */
/* ---------------------------------------------------------------------- */
/* |    round fait un arrondi float -> int.                             | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
int                 round (float e)

#else  /* __STDC__ */
int                 round (e)
float               e;

#endif /* __STDC__ */

{
   register int        res;

   if (e < 0.0)
      res = e - 0.5;
   else
      res = e + 0.5;
   return res;
}
/*fin */
/*debut */
/* ---------------------------------------------------------------------- */
/* |    CarCoup calcule le nombre de caracte`res nbcar a` placer dans   | */
/* |            la premie`re boi^te coupe'e pour la limiter a` une      | */
/* |            longueur de ilg en pixels.                              | */
/* |            Retourne le nombre de caracte`res a` sauter, soit 1 ou  | */
/* |            plus si la coupure tombe sur un blanc, -1 si le dernier | */
/* |            mot est coupe' et un tiret doit e^tre engendre', -2 si  | */
/* |            le dernier mot est coupe' sans besoin de tiret, 0 si la | */
/* |            coupure ne correspond pas a` un point de coupure.       | */
/* |            Rend la position a` laquelle le point de coupure peut   | */
/* |            e^tre inse're' :                                        | */
/* |            - le nombre de caracteres : nbcar (blancs compris sauf  | */
/* |            les derniers).                                          | */
/* |            - la longueur de cette nouvelle sous-chaine : lgcoup    | */
/* |            (avec les blancs).                                      | */
/* |            - le nombre de blancs qu'elle contient : nbblanc        | */
/* |            (sauf les derniers).                                    | */
/* |            - l'adresse du buffer du 1er caractere apres coupure.   | */
/* |            - l'index dans ce buffer du 1er caractere apres coupure.| */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static int          CarCoup (PtrLine adligne, PtrBox pBox, int ilg, ptrfont font, int *nbcar, int *lgcoup, int *nbblanc, int *newicar, PtrTextBuffer * newbuff)

#else  /* __STDC__ */
static int          CarCoup (adligne, pBox, ilg, font, nbcar, lgcoup, nbblanc, newicar, newbuff)
PtrLine            adligne;
PtrBox            pBox;
int                 ilg;
ptrfont             font;
int                *nbcar;
int                *lgcoup;
int                *nbblanc;
int                *newicar;
PtrTextBuffer     *newbuff;

#endif /* __STDC__ */

{
   PtrLine            pLi1;

/*fin */
   int                 i, cpt;
   int                 lgcar, lgnew;
   int                 largeur;
   int                 lgmot, icar;
   int                 saut, lgblanc;
   unsigned char       car;
   PtrTextBuffer      adbuff;
   PtrBox            boxmere;
   int                 bcomp;
   int                 cptblanc;
   boolean             nonfini;
   Language            langue;

   /* Initialisations */
   saut = 0;
   car = 0;
   *newbuff = NULL;
   *newicar = 1;
   *nbcar = pBox->BxNChars;
   *lgcoup = pBox->BxWidth;
   *nbblanc = pBox->BxNSpaces;

   icar = pBox->BxFirstChar;		/* index dans le buffer */
   adbuff = pBox->BxBuffer;
   /* lgnew est la largeur utilisee pour calculer la mise en ligne */
   /* largeur est la largeur reelle du texte                       */
   lgnew = 0;
   largeur = 0;
   lgcar = 0;
   lgmot = 0;
   cptblanc = 0;
   nonfini = TRUE;
   lgblanc = CarWidth (BLANC, font);
   bcomp = lgblanc;
   langue = pBox->BxAbstractBox->AbLanguage;

   if (ilg != pBox->BxWidth - 1)
     {
	/* L'appel de CarCoup ne concerne pas les blancs en fin de paragraphe */
	boxmere = pBox->BxAbstractBox->AbEnclosing->AbBox;
	/* On remonte jusqu'a la boite bloc de lignes */
	while (boxmere->BxType == BoGhost)
	   boxmere = boxmere->BxAbstractBox->AbEnclosing->AbBox;
	/* Si la ligne est justifiee on prend des blancs compresses */
	if (boxmere->BxAbstractBox->AbJustify)
	   bcomp = round ((float) (bcomp * 7) / (float) (10));
     }

   i = 0;
   cpt = pBox->BxNChars;

   /* Recherche un point de coupure */
   while (nonfini)
     {
	/* Largeur du caractere suivant */
	car = (unsigned char) (adbuff->BuContent[icar - 1]);
	if (car == NUL)
	   lgcar = 0;
	else if (car == BLANC)
	   lgcar = bcomp;
	else
	   lgcar = CarWidth (car, font);

	/* Si le caractere ne rentre pas dans la ligne */
	/* ou si on arrive a la fin de la boite        */
	if ((lgnew + lgcar > ilg || i >= cpt) && (i != 0))
	  {
	     nonfini = FALSE;
/*-- Si on veut simplement supprimer les blancs de fin de ligne */
	     if (ilg >= pBox->BxWidth)
	       {
		  /* Pointe le caractere apres la fin de boite */
		  *newicar = 1;
		  *newbuff = NULL;
		  *lgcoup = pBox->BxWidth;
		  *nbcar = pBox->BxNChars;
		  *nbblanc = pBox->BxNSpaces;

		  /* Pointe le dernier caractere de la boite */
		  if (icar == 1 && adbuff->BuPrevious != NULL)
		    {
		       /* Le caractere precedent se trouve dans un autre buffer */
		       adbuff = adbuff->BuPrevious;
		       icar = adbuff->BuLength;
		    }
		  else
		     /* Le caractere precedent se trouve dans le meme buffer */
		     icar--;
	       }
	     /* Si le caractere courant est un blanc */
	     else if (car == BLANC)
	       {
		  /* On coupe sur le dernier blanc rencontre' */
		  saut = 1;
		  if (cptblanc == 0)
		     *nbcar = lgmot;	/* dernier blanc non compris */
		  else
		     (*nbcar) += lgmot;		/* dernier blanc non compris */
		  *lgcoup = largeur;
		  *nbblanc = cptblanc;

		  /* Pointe le premier caractere apres la coupure */
		  if (icar >= adbuff->BuLength && adbuff->BuNext != NULL)
		    {
		       /* Le caractere suivant se trouve dans un autre buffer */
		       *newbuff = adbuff->BuNext;
		       *newicar = 1;
		    }
		  else
		    {
		       /* Le caractere suivant se trouve dans le meme buffer */
		       *newbuff = adbuff;
		       *newicar = icar + 1;
		    }		/*else */

		  /* Pointe le dernier caractere avant la coupure */
		  if (icar == 1 && adbuff->BuPrevious != NULL)
		    {
		       /* Le caractere precedent se trouve dans un autre buffer */
		       adbuff = adbuff->BuPrevious;
		       icar = adbuff->BuLength;
		    }
		  else
		     /* Le caractere precedent se trouve dans le meme buffer */
		     icar--;
	       }
	     /* Si on n'a pas trouve de caractere blanc */
	     else if (cptblanc == 0)
	       {
		  (*nbcar) = lgmot;
		  *lgcoup = largeur;
		  *nbblanc = 0;

		  /* Pointe le premier caractere apres la coupure */
		  *newbuff = adbuff;
		  *newicar = icar;

		  /* Pointe le dernier caractere avant la coupure */
		  if (icar == 1 && adbuff->BuPrevious != NULL)
		    {
		       /* Le caractere precedent se trouve dans un autre buffer */
		       adbuff = adbuff->BuPrevious;
		       icar = adbuff->BuLength;
		    }
		  else
		     /* Le caractere precedent se trouve dans le meme buffer */
		     icar--;
	       }
	     /* Coupure sur un blanc precedent */
	     else
	       {
		  saut = 1;
		  *nbblanc = cptblanc - 1;

		  adbuff = *newbuff;
		  icar = *newicar;
		  /* Pointe le premier caractere apres la coupure */
		  if (icar >= adbuff->BuLength && adbuff->BuNext != NULL)
		    {
		       /* Le caractere suivant se trouve dans un autre buffer */
		       *newbuff = adbuff->BuNext;
		       *newicar = 1;
		    }
		  else
		     /* Le caractere suivant se trouve dans le meme buffer */
		     (*newicar)++;

		  /* Pointe le dernier caractere avant la coupure */
		  if (icar == 1 && adbuff->BuPrevious != NULL)
		    {
		       /* Le caractere precedent se trouve dans un autre buffer */
		       adbuff = adbuff->BuPrevious;
		       icar = adbuff->BuLength;
		    }
		  else
		     /* Le caractere precedent se trouve dans le meme buffer */
		     icar--;
	       }
	  }
	/* Si c'est une fin de buffer */
	else if (car == NUL)
	   /* Si c'est la fin de la boite */
	   if (adbuff->BuNext == NULL)
	     {
		nonfini = FALSE;
		/* Pointe le premier caractere apres la coupure */
		*newbuff = adbuff;
		*newicar = icar;
		*lgcoup = pBox->BxWidth;
		*nbcar = pBox->BxNChars;
		*nbblanc = pBox->BxNSpaces;
		icar--;
	     }
	/* On change de buffer */
	   else
	     {
		adbuff = adbuff->BuNext;
		icar = 1;
	     }
	/* Si c'est un caractere blanc */
	else if (car == BLANC)
	  {
	     /* Cela peut etre un point de coupure */
	     *newbuff = adbuff;
	     *newicar = icar;
	     /* Longueur de la chaine dernier blanc non compris */
	     if (cptblanc == 0)
		*nbcar = lgmot;
	     else
		(*nbcar) += lgmot;
	     cptblanc++;
	     *lgcoup = largeur;
	     lgmot = 1;		/* prepare un nouveau mot */
	     i++;		/* nombre de caracteres traites */
	     icar++;		/* indice du prochain caractere */
	     lgnew += lgcar;
	     largeur += lgblanc;
	  }
	/*  Si c'est un autre caractere */
	else
	  {
	     lgmot++;
	     i++;		/* nombre de caracteres traites */
	     icar++;		/* indice du prochain caractere */
	     lgnew += lgcar;
	     largeur += lgcar;
	  }
     }

   /* On supprime eventuellement les blancs en fin de ligne avant la coupure */
   if (adbuff != NULL && saut != 0)
      /* Il a peut-etre des blancs a supprimer */
      nonfini = TRUE;
   else if (ilg >= pBox->BxWidth)
      /* Il a peut-etre des blancs a supprimer */
      nonfini = TRUE;
   else
      nonfini = FALSE;

   while (nonfini && *nbcar > 0)
     {
	car = (unsigned char) (adbuff->BuContent[icar - 1]);
	if (car == BLANC)
	  {
	     /* Est-ce que la coupure a lieu en fin de boite ? */
	     if (*newbuff == NULL)
	       {
		  *newbuff = adbuff;
		  *newicar = icar + 1;
	       }

	     /* On passe au caractere precedent */
	     if (icar == 1)
		if (adbuff->BuPrevious != NULL)
		  {
		     adbuff = adbuff->BuPrevious;
		     icar = adbuff->BuLength;
		  }
		else
		   nonfini = FALSE;
	     else
		icar--;

	     *lgcoup -= lgblanc;
	     saut++;
	     (*nbcar)--;
	     (*nbblanc)--;
	  }
	else
	   nonfini = FALSE;
     }

   if (ilg < pBox->BxWidth && Hyphenable (pBox) && lgmot > 1 && !Insert)
     {
	/* Il faut verifier les coupures en fin de lignes precedentes */
	nonfini = TRUE;
	if (adligne != NULL)
	  {
	     pLi1 = adligne->LiPrevious;
	     cpt = 0;
	     while (cpt < maxcoupuresvoisines && pLi1 != NULL)
		if (pLi1->LiLastPiece != NULL)
		   if (pLi1->LiLastPiece->BxType == BoDotted)
		     {
			/* Il y a une coupure en fin de ligne */
			/* on regarde encore la precedente */
			pLi1 = pLi1->LiPrevious;
			cpt++;
		     }
		   else
		      pLi1 = NULL;
		else
		   pLi1 = NULL;

	     if (cpt == maxcoupuresvoisines)
		/* On refuse de couper le dernier mot */
		nonfini = FALSE;
	  }			/*if adligne != NULL */

	if (nonfini)
	  {
	     /* On cherche reellement a couper le dernier mot de la boite */
	     if (saut == 0)
	       {
		  /* On essaie de couper le mot unique de la boite */
		  largeur = ilg;
		  adbuff = pBox->BxBuffer;
		  icar = pBox->BxFirstChar;
		  lgmot = CutLastWord (font, langue, &adbuff, &icar, &largeur, &nonfini);
		  /* Si la coupure a reussi */
		  if (lgmot > 0)
		    {
		       /* On met a jour le point de coupure */
		       *newbuff = adbuff;
		       *newicar = icar;
		       *lgcoup = 0;
		       *nbcar = 0;
		    }
	       }
	     else
	       {
		  /* On essaie de couper le mot qui suit le dernier blanc */
		  largeur = ilg - *lgcoup - saut * bcomp;
		  lgmot = CutLastWord (font, langue, newbuff, newicar, &largeur, &nonfini);
	       }

	     if (lgmot > 0)
	       {
		  /* Il faut mettre a jour le nombre de caracteres et */
		  /* la longueur de la premiere partie du texte coupe */
		  *nbcar += saut + lgmot;
		  *nbblanc += saut;
		  *lgcoup += largeur + saut * lgblanc;
		  if (nonfini)
		     /* Il faut engendrer un tiret d'hyphenation en fin de boite */
		     saut = -1;
		  else
		     /* Pas de tiret a engendrer */
		     saut = -2;
	       }
	  }			/*if nonfini */
     }

   /* Si on n'a pas trouve de coupure et que le mot ne peut pas */
   /* etre rejete a la ligne suivante (1er mot de la ligne */
   /* -> on force la coupure apres au moins 1 caractere */
   /* */
   if (saut == 0
       && (pBox == adligne->LiFirstBox || pBox == adligne->LiFirstPiece)
       && *newbuff != NULL)
     {
	/* Engendre un hyphen */
	saut = -1;
	*lgcoup += CarWidth (173, font);

	/* RemoveElement un ou plusieurs caracteres pour loger l'hyphen */
	while (*lgcoup > ilg && *nbcar > 1)
	  {
	     /* On passe au caractere precedent */
	     if (*newicar == 1)
		if ((*newbuff)->BuPrevious != NULL)
		  {
		     *newbuff = (*newbuff)->BuPrevious;
		     *newicar = (*newbuff)->BuLength;
		  }
		else
		   return saut;
	     else
		(*newicar)--;

	     *lgcoup -= CarWidth (car, font);
	     (*nbcar)--;
	  }

     }

   /* On supprime eventuellement les blancs en debut de ligne apres la coupure */
   if (*newbuff != NULL && saut > 0)
      /* Il a peut-etre des blancs a supprimer */
      nonfini = TRUE;
   else
      nonfini = FALSE;

   while (nonfini)
     {
	car = (unsigned char) ((*newbuff)->BuContent[*newicar - 1]);
	if (car == BLANC)
	  {

	     /* On passe au caractere suivant */
	     if (*newicar >= (*newbuff)->BuLength)
		if ((*newbuff)->BuNext != NULL)
		  {
		     *newbuff = (*newbuff)->BuNext;
		     *newicar = 1;
		  }
		else
		  {
		     nonfini = FALSE;
		     (*newicar)++;
		  }
	     else
		(*newicar)++;

	     saut++;
	  }
	else
	   nonfini = FALSE;
     }				/*while */
   return saut;
}


/* ---------------------------------------------------------------------- */
/* |    CouperBoite coupe la boi^te entiere d'indice pBox correspondant | */
/* |            a` l'image abstraite Pv en deux boi^tes. Si force est   | */
/* |            Vrai coupe meme sur un caracte`re, sinon on ne coupe que| */
/* |            sur un blanc. La premie`re boi^te issue de la coupure   | */
/* |            contiendra nbcar caracte`res. Sa longueur sera          | */
/* |            infe'rieure a` lgmax. Le blanc sera perdu. Le reste est | */
/* |            mis dans la boi^te de coupure suivante. Les chai^nages  | */
/* |            sont mis a` jour.                                       | */
/* ---------------------------------------------------------------------- */


#ifdef __STDC__
static void         CouperBoite (PtrLine adligne, PtrBox pBox, int lgmax, PtrAbstractBox Pv, boolean force)

#else  /* __STDC__ */
static void         CouperBoite (adligne, pBox, lgmax, Pv, force)
PtrLine            adligne;
PtrBox            pBox;
int                 lgmax;
PtrAbstractBox             Pv;
boolean             force;

#endif /* __STDC__ */

{
   PtrBox            ibox1, ibox2;
   int                 base, larg;
   int                 newicar, haut;
   int                 lgbl, reste;
   int                 nbblanc, nbcar;
   PtrBox            avant, apres;
   PtrTextBuffer      newbuff;
   PtrAbstractBox             pAb;
   ptrfont             font;

   /* Initialisations */
   pAb = pBox->BxAbstractBox;
   haut = pBox->BxHeight;
   base = pBox->BxHorizRef;
   avant = pBox->BxPrevious;
   apres = pBox->BxNext;
   font = pBox->BxFont;

   /* Si coupure sur un blanc le blanc de coupure est perdu */
   reste = CarCoup (adligne, pBox, lgmax, font, &nbcar, &larg, &nbblanc, &newicar, &newbuff);

   /* Creation de la 1ere boite qui contient le debut de la chaine */
   if (reste <= 0)
      lgbl = 0;
   else
      lgbl = CarWidth (BLANC, font);

   /* Creation de deux boites de coupure :                           */
   /* - dans le cas general si Carcoup a trouve' un point de coupure */
   /*   soit un blanc (reste>0), soit un point d'hyphe'nation  avec  */
   /*   ge'ne'ration d'un tiret (reste=-1) ou sans (reste=-2)        */
   /* - si la coupure est force'e, sur n'importe quel caractere      */
   /* - pour sauter les derniers blancs de fin de texte a` condition */
   /*   qu'il reste au moins un blanc pour justifier le texte restant */
   if (newbuff != NULL
       && (reste != 0 || nbblanc != 0 || force)
       && (pBox->BxWidth != lgmax || reste != pBox->BxNSpaces))
     {
	ibox1 = GetBox (pAb);
	ibox2 = GetBox (pAb);
     }
   else
     {
	ibox1 = NULL;
	ibox2 = NULL;
     }

   /* Traitement particulier des coupures de mots sans genenration du tiret */
   if (reste == -2)
      reste = 0;

   if (ibox1 != NULL && ibox2 != NULL)
     {
	/* Initialise le contenu de cette nouvelle boite */
	ibox1->BxIndChar = 0;
	ibox1->BxContentWidth = TRUE;
	ibox1->BxContentHeight = TRUE;
	ibox1->BxFont = font;
	ibox1->BxUnderline = pBox->BxUnderline;
	ibox1->BxThickness = pBox->BxThickness;
	ibox1->BxHeight = haut;
	ibox1->BxWidth = larg;
	ibox1->BxHorizRef = base;
	if (reste == -1)
	  {
	     /* La boite doit etre completee par un tiret d'hyphenation */
	     ibox1->BxType = BoDotted;
	     larg -= CarWidth (173, font);
	     reste = 0;		/* il n'y a pas de blanc saute */
	  }
	else
	   ibox1->BxType = BoPiece;
	ibox1->BxBuffer = pBox->BxBuffer;
	ibox1->BxNChars = nbcar;
	ibox1->BxNSpaces = nbblanc;
	ibox1->BxFirstChar = pBox->BxFirstChar;

	/* Initialise le contenu de la deuxieme boite */
	ibox2->BxContentWidth = TRUE;
	ibox2->BxContentHeight = TRUE;
	ibox2->BxHeight = haut;
	ibox2->BxFont = font;
	ibox2->BxUnderline = pBox->BxUnderline;
	ibox2->BxThickness = pBox->BxThickness;
	ibox2->BxHorizRef = base;
	ibox2->BxType = BoPiece;
	ibox2->BxBuffer = newbuff;
	ibox2->BxIndChar = nbcar + reste;
	ibox2->BxFirstChar = newicar;
	ibox2->BxNChars = pBox->BxNChars - reste - nbcar;
	ibox2->BxNSpaces = pBox->BxNSpaces - reste - nbblanc;
	ibox2->BxWidth = pBox->BxWidth - larg - reste * lgbl;

	/* Si la boite est vide on annule les autres valeurs */
	if (ibox2->BxNChars == 0)
	  {
	     ibox2->BxWidth = 0;
	     ibox2->BxNSpaces = 0;
	  }

	/* Modifie le chainage des boites terminales */
	ibox1->BxPrevious = avant;
	if (avant != NULL)
	   avant->BxNext = ibox1;
	else
	   Pv->AbBox->BxNext = ibox1;
	ibox1->BxNext = ibox2;
	ibox2->BxPrevious = ibox1;
	ibox1->BxNexChild = ibox2;
	ibox2->BxNexChild = NULL;
	ibox2->BxNext = apres;
	if (apres != NULL)
	   apres->BxPrevious = ibox2;
	else
	   Pv->AbBox->BxPrevious = ibox2;

	/* Mise a jour de la boite englobante */
	pBox->BxType = BoSplit;
	pBox->BxNexChild = ibox1;
     }

}


/* ---------------------------------------------------------------------- */
/* | Coupe la boite pBox sur un caracte`re de coupure force'e.          | */
/* | large = la largeur du de'but de la boite jusqu'au point de coupure | */
/* | lgcarsp = largeur du caracte`re qui force la coupure               | */
/* | nbcarsl = nombre de caracte`res avant la coupure                   | */
/* | nbblanc = nombre d'espaces avant le point de coupure               | */
/* | newicar = indice du debut de texte apres le point de coupure       | */
/* | newbuff = buffer du debut de texte apres le point de coupure       | */
/* | Pv = AbstractBox' racine de la vue (pour mise a jour des chainages)       | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void         cutbox (PtrBox pBox, int large, int lgcarsp, int nbcarsl, int nbblanc, int newicar, PtrTextBuffer newbuff, PtrAbstractBox Pv)

#else  /* __STDC__ */
static void         cutbox (pBox, large, lgcarsp, nbcarsl, nbblanc, newicar, newbuff, Pv)
PtrBox            pBox;
int                 large;
int                 lgcarsp;
int                 nbcarsl;
int                 nbblanc;
int                 newicar;
PtrTextBuffer      newbuff;
PtrAbstractBox             Pv;

#endif /* __STDC__ */

{
   PtrBox            ibox1, ibox2;
   int                 base;
   int                 haut;
   PtrBox            avant, apres;
   PtrAbstractBox             pAb;
   ptrfont             font;


   /* Initialisation */
   ibox1 = NULL;
   pAb = pBox->BxAbstractBox;
   haut = pBox->BxHeight;
   base = pBox->BxHorizRef;
   avant = pBox->BxPrevious;
   apres = pBox->BxNext;
   font = pBox->BxFont;

   /* Creation de la boite de coupure pour le texte apres la coupure */
   if (newbuff != NULL)
     {
	/* Si la boite est entiere, il faut une boite de coupure */
	/* pour contenir le texte avant le point de coupure      */
	if (pBox->BxType == BoComplete)
	  {
	     ibox1 = GetBox (pAb);
	     if (ibox1 == NULL)
		goto Label_10;	/* plus de boite */
	  }
	ibox2 = GetBox (pAb);
     }
   else
      ibox2 = NULL;

   /* Initialisation de cette boite */
   if (ibox2 != NULL)
     {
	ibox2->BxContentWidth = TRUE;
	ibox2->BxContentHeight = TRUE;
	ibox2->BxHeight = haut;
	ibox2->BxFont = font;
	ibox2->BxUnderline = pBox->BxUnderline;
	ibox2->BxThickness = pBox->BxThickness;
	ibox2->BxHorizRef = base;
	ibox2->BxType = BoPiece;
	ibox2->BxBuffer = newbuff;
	ibox2->BxNexChild = NULL;
	ibox2->BxIndChar = pBox->BxIndChar + nbcarsl + 1;
	ibox2->BxFirstChar = newicar;
	ibox2->BxNChars = pBox->BxNChars - nbcarsl - 1;
	ibox2->BxNSpaces = pBox->BxNSpaces - nbblanc;
	ibox2->BxWidth = pBox->BxWidth - large - lgcarsp;

	/* Si la boite est entiere, il faut une boite de coupure */
	/* pour contenir le texte avant le point de coupure      */
	if (pBox->BxType == BoComplete)
	  {
	     /* Initialisation de la boite ibox1 */
	     ibox1->BxContentWidth = TRUE;
	     ibox1->BxContentHeight = TRUE;
	     ibox1->BxIndChar = 0;
	     ibox1->BxUnderline = pBox->BxUnderline;
	     ibox1->BxThickness = pBox->BxThickness;
	     ibox1->BxFont = font;
	     ibox1->BxHeight = haut;
	     ibox1->BxHorizRef = base;
	     ibox1->BxType = BoPiece;
	     ibox1->BxBuffer = pBox->BxBuffer;
	     ibox1->BxFirstChar = pBox->BxFirstChar;
	     ibox1->BxWidth = large;
	     ibox1->BxNSpaces = nbblanc;
	     ibox1->BxNChars = nbcarsl;

	     /* Modifie le chainage des boites terminales */
	     ibox1->BxNexChild = ibox2;
	     ibox1->BxPrevious = avant;
	     if (avant != NULL)
		avant->BxNext = ibox1;
	     else
		Pv->AbBox->BxNext = ibox1;
	     ibox1->BxNext = ibox2;
	     ibox2->BxPrevious = ibox1;
	     ibox2->BxNext = apres;
	     if (apres != NULL)
		apres->BxPrevious = ibox2;
	     else
		Pv->AbBox->BxPrevious = ibox2;

	     /* Mise a jour de la boite englobante */
	     pBox->BxType = BoSplit;
	     pBox->BxNexChild = ibox1;
	  }
	/* Si la boite est deja une boite de coupure, il faut la mettre a jour */
	else
	  {
	     pBox->BxWidth = large;
	     pBox->BxNSpaces = nbblanc;
	     pBox->BxNChars = nbcarsl;

	     /* Modifie les boites terminales */
	     pBox->BxNexChild = ibox2;
	     pBox->BxNext = ibox2;
	     ibox2->BxPrevious = pBox;
	     ibox2->BxNext = apres;
	     if (apres != NULL)
		apres->BxPrevious = ibox2;
	     else
		Pv->AbBox->BxPrevious = ibox2;
	  }

     }

 Label_10:;
}


/* ---------------------------------------------------------------------- */
/* | Teste s'il y a un caracte`re de coupure force'e dans la boite pBox | */
/* | Si oui retourne la valeur Vrai, sinon la valeur FALSE.             | */
/* | Au retour :                                                        | */
/* | large = la largeur du de'but de la boite jusqu'au point de coupure | */
/* | lgcarsp = largeur du caracte`re qui force la coupure               | */
/* | nbcarsl = nombre de caracte`res avant la coupure                   | */
/* | nbblanc = nombre d'espaces avant le point de coupure               | */
/* | newicar = indice du debut de texte apres le point de coupure       | */
/* | newbuff = buffer du debut de texte apres le point de coupure       | */
/* | Pv = AbstractBox' racine de la vue (pour mise a jour des chainages)       | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
boolean             testcsl (PtrBox pBox, int *large, int *lgcarspec, int *nbcarsl, int *nbblanc, int *newicar, PtrTextBuffer * newbuff)

#else  /* __STDC__ */
boolean             testcsl (pBox, large, lgcarspec, nbcarsl, nbblanc, newicar, newbuff)
PtrBox            pBox;
int                *large;
int                *lgcarspec;
int                *nbcarsl;
int                *nbblanc;
int                *newicar;
PtrTextBuffer     *newbuff;

#endif /* __STDC__ */

{
   int                 i, j;
   unsigned char       car;
   int                 nbcar;
   boolean             csexist;
   PtrTextBuffer      adbuff;
   ptrfont             font;


   csexist = FALSE;
   *large = 0;
   *nbcarsl = 0;
   *nbblanc = 0;
   j = 1;
   adbuff = pBox->BxBuffer;
   nbcar = pBox->BxNChars;
   font = pBox->BxFont;
   i = pBox->BxFirstChar;
   *newbuff = adbuff;
   *newicar = 1;

   /* Si la boite debute en fin de buffer -> on passe au buffer suivant */
   if (adbuff != NULL)
      if (i > adbuff->BuLength)
	{
	   adbuff = adbuff->BuNext;
	   i = 1;
	}			/*if */

   while (j <= nbcar && !(csexist) && adbuff != NULL)
     {
	car = (unsigned char) (adbuff->BuContent[i - 1]);
	if (car == SAUT_DE_LIGNE)	/* Est-ce un caractere de coupure forcee ? */
	  {
	     csexist = TRUE;
	     *lgcarspec = CarWidth (car, font);
	     /* Faut-il changer de buffer ? */
	     if (i >= adbuff->BuLength)
	       {
		  /* La caractere de coupure force' est en fin de texte */
		  if (adbuff->BuNext == NULL)
		    {
		       /* On reste en fin de buffer */
		       j = nbcar;
		       i++;
		    }
		  /* Sinon on passe au debut du buffer suivant */
		  else
		    {
		       adbuff = adbuff->BuNext;
		       i = 1;
		    }
	       }
	     else
		i++;

	     *newbuff = adbuff;
	     *newicar = i;
	     *nbcarsl = j - 1;
	     return csexist;
	  }
	else
	   /* Sinon on continue la recherche */
	  {
	     if (car == BLANC)
	       {
		  (*nbblanc)++;
		  *large += CarWidth (BLANC, font);
	       }
	     else
		*large += CarWidth (car, font);
	  }
	if (i >= adbuff->BuLength)	/* Faut-il changer de buffer ? */
	  {
	     adbuff = adbuff->BuNext;
	     if (adbuff == NULL)
		j = nbcar;
	     i = 1;
	  }
	else
	   i++;
	j++;
     }
   return csexist;
}


/* ---------------------------------------------------------------------- */
/* |    ReCouperBoite recoupe la boite pBox deja coupee pour l'image    | */
/* |            abstraite Pv. La coupure a lieu meme s'il n'y a pas de  | */
/* |            blanc (coupure forcee). La boite pBox contiendra nbcar  | */
/* |            caracteres.                                             | */
/* ---------------------------------------------------------------------- */


#ifdef __STDC__
static void         ReCouperBoite (PtrLine adligne, PtrBox pBox, int lgmax, PtrAbstractBox Pv)

#else  /* __STDC__ */
static void         ReCouperBoite (adligne, pBox, lgmax, Pv)
PtrLine            adligne;
PtrBox            pBox;
int                 lgmax;
PtrAbstractBox             Pv;

#endif /* __STDC__ */

{
   PtrBox            box2, apres;
   int                 oldlg, lgbl;
   int                 nbcar;
   int                 larg, reste, pl;
   int                 newicar, nbblanc;
   int                 haut, base;
   PtrTextBuffer      newbuff;
   PtrAbstractBox             pAb;
   ptrfont             font;
   int                 oldlarge, oldnbbl;

   pAb = pBox->BxAbstractBox;
   haut = pBox->BxHeight;
   base = pBox->BxHorizRef;
   apres = pBox->BxNext;
   pl = pBox->BxIndChar;
   oldlg = pBox->BxNChars;
   font = pBox->BxFont;
   oldlarge = pBox->BxWidth;
   oldnbbl = pBox->BxNSpaces;

   /* Si coupure sur un blanc le blanc de coupure est perdu */
   reste = CarCoup (adligne, pBox, lgmax, font, &nbcar, &larg, &nbblanc, &newicar, &newbuff);
   if (reste <= 0)
      lgbl = 0;
   else
      lgbl = CarWidth (BLANC, font);

   /* Creation de la boite suivante qui contient la fin de la chaine */
   /* - dans le cas general si Carcoup a trouve' un point de coupure */
   /*   soit un blanc (reste>0), soit un point d'hyphe'nation  avec  */
   /*   ge'ne'ration d'un tiret (reste=-1) ou sans (reste=-2)        */
   /* - pour sauter les derniers blancs de fin de texte a` condition */
   /*   qu'il reste au moins un blanc pour justifier le texte restant */
   if (newbuff != NULL
/**528*/  && (reste != 0 || nbblanc != 0 || oldnbbl == 0)
       && (pBox->BxWidth != lgmax || reste != pBox->BxNSpaces))
      box2 = GetBox (pAb);
   else
      box2 = NULL;


   /* Coupure du mot sans generation du tiret */
   if (reste == -2)
      reste = 0;

   /* Initialise le contenu de cette nouvelle boite */
   if (box2 != NULL)
     {

	pBox->BxNexChild = box2;
	pBox->BxNext = box2;

	/* Mise a jour de la  boite recoupee */
	pBox->BxNChars = nbcar;
	pBox->BxWidth = larg;
	pBox->BxNSpaces = nbblanc;
	if (reste == -1)
	  {
	     /* La boite doit etre completee par un tiret d'hyphenation */
	     pBox->BxType = BoDotted;
	     larg -= CarWidth (173, font);
	     reste = 0;		/* il n'y a pas de blanc saute */
	  }
	else
	   pBox->BxType = BoPiece;

	box2->BxContentWidth = TRUE;
	box2->BxContentHeight = TRUE;
	box2->BxHeight = haut;
	box2->BxFont = font;
	box2->BxUnderline = pBox->BxUnderline;
	box2->BxThickness = pBox->BxThickness;
	box2->BxHorizRef = base;
	box2->BxType = BoPiece;
	box2->BxBuffer = newbuff;
	box2->BxNexChild = NULL;

	/* Modifie le chainage des boites terminales */
	box2->BxPrevious = pBox;
	box2->BxNext = apres;
	if (apres != NULL)
	   apres->BxPrevious = box2;
	else
	   Pv->AbBox->BxPrevious = box2;

	/* Si reste > 0 -> Il faut retirer les caracteres blanc de la boite */
	box2->BxIndChar = pl + reste + nbcar;
	box2->BxFirstChar = newicar;
	box2->BxNChars = oldlg - reste - nbcar;
	box2->BxNSpaces = oldnbbl - reste - nbblanc;
	box2->BxWidth = oldlarge - larg - reste * lgbl;

	/* Si la boite est vide on annule les autres valeurs */
	if (box2->BxNChars == 0)
	  {
	     box2->BxWidth = 0;
	     box2->BxNSpaces = 0;
	  }
     }
}


/* ---------------------------------------------------------------------- */
/* |    AjoutBoite      TODO: Expliquer.                                | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                AjoutBoite (PtrBox pBox, int *sousbase, int *surbase, PtrLine adligne)

#else  /* __STDC__ */
void                AjoutBoite (pBox, sousbase, surbase, adligne)
PtrBox            pBox;
int                *sousbase;
int                *surbase;
PtrLine            adligne;

#endif /* __STDC__ */

{
   adligne->LiRealLength += pBox->BxWidth;
   /* Calcule la hauteur courante de la ligne */
   if (*surbase < pBox->BxHorizRef)
      *surbase = pBox->BxHorizRef;
   if (*sousbase < pBox->BxHeight - pBox->BxHorizRef)
      *sousbase = pBox->BxHeight - pBox->BxHorizRef;
}


/* ---------------------------------------------------------------------- */
/* |    RemplirLigne remplit la ligne dont l'adresse est passee en      | */
/* |            parametre pour l'image abstraite Pv.                    | */
/*CP |          Le parametre uneSuite determine si la paragraphe a ou   | */
/*CP |          n'a pas de suite dans une page suivante.                | */
/* |            On suppose LiFirstBox et LiFirstPiece pre'alablement remplis.     | */
/* |            On calcule l'indice de la derniere boite qui peut       | */
/* |            entrer dans la ligne et on met a` jour le descripteur   | */
/* |            de ligne : LiLastBox et LiLastPiece (si coupee).                | */
/* |            Les positions des boites mises en lignes sont calculees | */
/* |            par rapport a` la boite racine.                         | */
/* |            Au retour : plein indique si la ligne est pleine ou non.| */
/* ---------------------------------------------------------------------- */
#ifdef __COLPAGE__
#ifdef __STDC__
void                RemplirLigne (PtrLine adligne, PtrAbstractBox Pv, boolean uneSuite, boolean * plein, boolean * ajust)

#else  /* __STDC__ */
void                RemplirLigne (adligne, Pv, uneSuite, plein, ajust)
PtrLine            adligne;
PtrAbstractBox             Pv;
boolean             uneSuite;
boolean            *plein;
boolean            *ajust;

#endif /* __STDC__ */

#else  /* __COLPAGE__ */
#ifdef __STDC__
void                RemplirLigne (PtrLine adligne, PtrAbstractBox Pv, boolean * plein, boolean * ajust)

#else  /* __STDC__ */
void                RemplirLigne (adligne, Pv, plein, ajust)
PtrLine            adligne;
PtrAbstractBox             Pv;
boolean            *plein;
boolean            *ajust;

#endif /* __STDC__ */
#endif /* __COLPAGE__ */

{
   int                 xi, xmax;
   int                 lmax;
   boolean             nonfini;
   boolean             coup;
   PtrBox            pBox;
   PtrBox            nbox;
   int                 surbase;
   int                 sousbase;
   PtrBox            lastbox;
   int                 large;
   int                 lgcarspec;
   int                 nbcarsl;
   int                 nbblanc;
   int                 newicar;
   PtrTextBuffer      newbuff;
   boolean             csexist;

   *plein = TRUE;
   *ajust = TRUE;
   coup = FALSE;
   nonfini = TRUE;
   xmax = adligne->LiXMax;
   xi = 0;
   surbase = 0;
   sousbase = 0;
   nbox = adligne->LiFirstBox;	/* la boite que l'on traite */
   adligne->LiLastPiece = NULL;
   csexist = FALSE;

   /* nbox est la boite courante que l'on traite    */
   /* pBox est la derniere boite mise dans la ligne */

   /* La premiere boite est la suite d'une boite deja coupee ? */
   if (adligne->LiFirstPiece != NULL)
     {
	/* La boite de coupure entre obligatoirement dans la ligne */
	pBox = adligne->LiFirstPiece;

	/* Est-ce que l'on rencontre une coupure force dans la boite ? */
	csexist = testcsl (pBox, &large, &lgcarspec, &nbcarsl, &nbblanc, &newicar, &newbuff);
	/* La coupure forcee arrive avant la fin de ligne */
	if (csexist && large + xi <= xmax)
	  {
	     cutbox (pBox, large, lgcarspec, nbcarsl, nbblanc, newicar, newbuff, Pv);
	     *ajust = FALSE;
	     nonfini = FALSE;
	  }
	/* La boite entre dans toute entiere dans la ligne */
	else if (pBox->BxWidth + xi <= xmax)
	  {
	     nbox = Suivante (pBox->BxAbstractBox);
	     if (nbox == NULL)
	       {
		  *plein = FALSE;
		  nonfini = FALSE;
	       }
	  }
	/* Il faut couper une autre fois la boite */
	else
	  {
	     nonfini = FALSE;
	     ReCouperBoite (adligne, pBox, xmax - xi, Pv);
	  }

	pBox = adligne->LiFirstPiece;
	xi += pBox->BxWidth;
     }
   else
      /* Il n'y a pas encore de boite dans la ligne */
      pBox = NULL;

   /* On recherche la boite qu'il faut couper */
   while (nonfini)

      /* On refuse de mettre en ligne une boite non englobee */
      if (!nbox->BxAbstractBox->AbHorizEnclosing)
	{
	   *plein = TRUE;
	   nonfini = FALSE;

	   /* Est-ce la premiere boite de la ligne ? */
	   if (pBox == NULL)
	      adligne->LiLastPiece = adligne->LiFirstPiece;
	   /* Est-ce que la boite precedente est coupee ? */
	   else if (pBox->BxType == BoPiece || pBox->BxType == BoDotted)
	      adligne->LiLastPiece = adligne->LiFirstPiece;
	}
      else
	{
	   /* Recherche si on doit forcer un return dans la boite */
	   if (nbox->BxAbstractBox->AbAcceptLineBreak)
	      csexist = testcsl (nbox, &large, &lgcarspec, &nbcarsl, &nbblanc, &newicar, &newbuff);
	   else
	      csexist = FALSE;

	   /* La boite produite par le return force' tient dans la ligne ? */
	   if (csexist && large + xi <= xmax)
	     {
		nonfini = FALSE;
		cutbox (nbox, large, lgcarspec, nbcarsl, nbblanc, newicar, newbuff, Pv);
		*ajust = FALSE;
		if (nbox->BxNexChild != NULL)
		  {
		     pBox = nbox->BxNexChild;
		     /* Est-ce la boite unique de la ligne ? */
		     if (nbox == adligne->LiFirstBox)
			adligne->LiFirstPiece = pBox;
		  }
	     }
	   /* La boite tient dans la ligne ? */
	   else if (nbox->BxWidth + xi <= xmax)
	     {
		pBox = nbox;	/* La boite entre dans la ligne */
		xi += nbox->BxWidth;
		nbox = Suivante (nbox->BxAbstractBox);
		if (nbox == NULL)
		  {
		     *plein = FALSE;
		     nonfini = FALSE;
		  }
	     }
	   /* Il faut couper la boite ou une boite precedente */
	   else
	     {
		coup = TRUE;
		nonfini = FALSE;
	     }
	}

   /* Traitement des coupures de boites */
   if (coup)
     {
	/* On va essayer de couper la boite nbox ou une boite precedente */
	lmax = xmax - xi;

	/* Est-ce la boite unique de la ligne ? */
	if (nbox == adligne->LiFirstBox)
	   /* Si elle n'est pas secable -> on laisse deborder */
	   if (!nbox->BxAbstractBox->AbAcceptLineBreak
	       || nbox->BxAbstractBox->AbLeafType != LtText)
	      pBox = nbox;	/* derniere boite de la ligne */
	/* Sinon on coupe la boite texte en un point quelconque */
	   else
	     {
		CouperBoite (adligne, nbox, lmax, Pv, TRUE);	/* coupure forcee */
		if (nbox->BxNexChild != NULL)
		  {
		     pBox = nbox->BxNexChild;
		     adligne->LiFirstPiece = pBox;
		  }
	     }
	/* Il y a au moins une autre boite dans la ligne */
	else
	  {
	     /* On essaie de couper la boite texte secable */
	     if (nbox->BxAbstractBox->AbLeafType == LtText
		 && nbox->BxAbstractBox->AbAcceptLineBreak
	     /* avec au moins un blanc ou avec coupure de mot */
		 && (Hyphenable (nbox) || nbox->BxNSpaces != 0))
	       {
		  CouperBoite (adligne, nbox, lmax, Pv, FALSE);		/* coupure sur blanc */
		  if (nbox->BxNexChild != NULL)
		    {
		       /* On a trouve le point de coupure */
		       pBox = nbox->BxNexChild;
		       coup = FALSE;
		    }
	       }

	     lastbox = nbox;
	     /* Si la coupure a echoue' on recherche en arriere un point de coupure */
	     while (coup)
	       {
		  /* On essaye de couper la boite precedente */
		  if (nbox != NULL)
		     nbox = Precedente (nbox->BxAbstractBox);

		  /* Est-ce que l'on bute sur la premiere boite de la ligne ? */
		  if (nbox == adligne->LiFirstPiece)
		     coup = FALSE;
		  else if (nbox == adligne->LiFirstBox)
		    {
		       coup = FALSE;
		       if (adligne->LiFirstPiece != NULL)
			  nbox = adligne->LiFirstPiece;
		    }

		  /* Si c'est une boite de texte secable */
		  if (nbox->BxAbstractBox->AbLeafType == LtText
		      && nbox->BxWidth != 0	/* visible */
		      && nbox->BxAbstractBox->AbAcceptLineBreak
		  /* avec au moins un blanc si on est en insertion */
		      && nbox->BxNSpaces != 0)

		     /* On coupe sur le dernier blanc de la boite */
		     if (nbox->BxType == BoPiece)
		       {
			  ReCouperBoite (adligne, nbox, nbox->BxWidth - 1, Pv);
			  pBox = nbox;
		       }
		     else
		       {
			  CouperBoite (adligne, nbox, nbox->BxWidth - 1, Pv, FALSE);
			  pBox = nbox->BxNexChild;
			  /* Est-ce que l'on a coupe la premiere boite de la ligne ? */
			  if (nbox == adligne->LiFirstBox && adligne->LiFirstPiece == NULL)
			     adligne->LiFirstPiece = pBox;
			  coup = FALSE;
		       }
		  /* Si la coupure sur un blanc echoue on coupe la derniere boite de la ligne */
		  else if (!coup)
		    {
		       /* Si la derniere boite est secable -> force sa coupure */
		       if (lastbox->BxAbstractBox->AbAcceptLineBreak
			   && lastbox->BxAbstractBox->AbLeafType == LtText)
			 {
			    CouperBoite (adligne, lastbox, lmax, Pv, TRUE);	/* coupure forcee */
			    pBox = lastbox->BxNexChild;
			 }
		       /* Si la boite est seule dans la ligne -> laisse deborder */
		       else if (lastbox == adligne->LiFirstBox)
			  pBox = lastbox;
		       /* sinon on coupe avant la derniere boite */
		       else
			 {
			    pBox = Precedente (lastbox->BxAbstractBox);
			    /* Si c'est la premiere boite de la ligne et que celle-ci ets coupee */
			    if (pBox == adligne->LiFirstBox && adligne->LiFirstPiece != NULL)
			       pBox = adligne->LiFirstPiece;
			 }
		    }
		  /* Sinon on continue la recherche en arriere */
		  else
		     pBox = nbox;
	       }
	  }
     }

   /* On ajoute toutes les boites de la ligne */
   if (adligne->LiFirstPiece != NULL)
      nbox = adligne->LiFirstPiece;
   else
      nbox = adligne->LiFirstBox;

   nonfini = TRUE;
   while (nonfini)
     {
	if (nbox->BxType == BoSplit)
	   nbox = nbox->BxNexChild;
	AjoutBoite (nbox, &sousbase, &surbase, adligne);
	if (nbox == pBox)
	   nonfini = FALSE;
	else
	   nbox = Suivante (nbox->BxAbstractBox);
     }

   /* On termine le chainage */
   if (pBox->BxType == BoPiece || pBox->BxType == BoDotted)
     {
	adligne->LiLastPiece = pBox;
	adligne->LiLastBox = pBox->BxAbstractBox->AbBox;
     }
   else
      adligne->LiLastBox = pBox;

   /* On teste s'il reste des boites a mettre en ligne */
   if (pBox->BxNexChild == NULL && Suivante (pBox->BxAbstractBox) == NULL)
      *plein = FALSE;

   /* On coupe les blancs en fin de ligne pleine */
#ifdef __COLPAGE
   if ((uneSuite || *plein) && pBox->BxAbstractBox->AbLeafType == LtText && pBox->BxNSpaces != 0)
#else  /* __COLPAGE__ */
   if (*plein && pBox->BxAbstractBox->AbLeafType == LtText && pBox->BxNSpaces != 0)
#endif /* __COLPAGE__ */
      if (adligne->LiLastPiece == NULL)
	{
	   lmax = pBox->BxWidth;
	   CouperBoite (adligne, pBox, lmax, Pv, FALSE);	/*coupure sur un caractere refusee */
	   if (pBox->BxNexChild != NULL)
	     {
		/* On remplace la boite entiere par la boite de coupure */
		adligne->LiRealLength = adligne->LiRealLength - lmax + pBox->BxNexChild->BxWidth;
		adligne->LiLastPiece = pBox->BxNexChild;
	     }
	}
      else if (adligne->LiLastPiece->BxNexChild == NULL)
	{
	   pBox = adligne->LiLastPiece;
	   lmax = pBox->BxWidth;
	   ReCouperBoite (adligne, pBox, lmax, Pv);
	   /* On met a jour la largeur de la ligne */
	   adligne->LiRealLength = adligne->LiRealLength - lmax + pBox->BxWidth;
	}

   /* Calcule la hauteur et la base de la ligne */
   adligne->LiHeight = sousbase + surbase;
   adligne->LiHorizRef = surbase;
   /* les boites ne sont pas positionnees dans la ligne */
}


/* ---------------------------------------------------------------------- */
/* |    BlocDeLigne cre'e les lignes ne'cessaires pour contenir les     | */
/* |            boi^tes filles de'ja` cre'e'es. Le parame`tre frame     | */
/* |            de'signe la fenetree^tre concerne'e. Les boi^tes filles | */
/* |            sont positionne'es:                                     | */
/* |            - par rapport a` la boi^te englobante si le             | */
/* |              de'placement est relatif,                             | */
/* |            - sinon par rapport a` la racine de l'Picture Concre`te.  | */
/* |            Rend la hauteur calcule'e du bloc de lignes.            | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                BlocDeLigne (PtrBox pBox, int frame, int *haut)

#else  /* __STDC__ */
void                BlocDeLigne (pBox, frame, haut)
PtrBox            pBox;
int                 frame;
int                *haut;

#endif /* __STDC__ */

{
   PtrLine            adligne1;
   PtrLine            adligne2;
   PtrLine            adligne3;
   PtrAbstractBox             pavefils;
   boolean             complet;
   boolean             encore;
   boolean             interlignage;
   PtrBox            boxcoup;
   PtrBox            box1;
   int                 x, itl, idl;
   int                 org;
   PtrAbstractBox             pAbbox1;
   PtrBox            pBo2;
   PtrLine            pLi2;
   AbPosition        *pPavP1;
   boolean             ajustif;
   boolean             absoluEnX;
   boolean             absoluEnY;

   /* Remplissage de la boite bloc de ligne */
   x = 0;
   pAbbox1 = pBox->BxAbstractBox;

   /* evalue si le positionnement en X et en Y doit etre absolu */
   XYEnAbsolu (pBox, &absoluEnX, &absoluEnY);

   if (pBox->BxWidth > CarWidth (119, pBox->BxFont))	/*'w' */
     {
	/* Acquisition d'une ligne */
	complet = TRUE;
	box1 = NULL;
	/* Calcul de l'interligne */
	itl = PixelValue (pAbbox1->AbLineSpacing, pAbbox1->AbLineSpacingUnit, pAbbox1);
	/* Calcul de l'indentation */
	if (pAbbox1->AbIndentUnit == UnPercent)
	   idl = PixelValue (pAbbox1->AbIndent, UnPercent, pBox->BxWidth);
	else
	   idl = PixelValue (pAbbox1->AbIndent, pAbbox1->AbIndentUnit, pAbbox1);
	if (pAbbox1->AbIndent < 0)
	   idl = -idl;

	/* Construction complete du bloc de ligne */
	if (pBox->BxFirstLine == NULL)
	  {
	     /* On recherche la premiere boite mise en ligne */
	     pavefils = pAbbox1->AbFirstEnclosed;
	     encore = pavefils != NULL;
	     while (encore)
		/* Est-ce que le pave est mort ? */
		if (pavefils->AbDead)
		  {
		     box1 = Suivante (pavefils);
		     encore = FALSE;
		  }
	     /* Est-ce que le pave est eclate ? */
		else if (pavefils->AbBox->BxType == BoGhost)
		  {
		     /* On descend la hierarchie */
		     pavefils = pavefils->AbFirstEnclosed;
		     encore = pavefils != NULL;
		  }
	     /* Sinon c'est la boite du pave */
		else
		  {
		     box1 = pavefils->AbBox;
		     encore = FALSE;
		  }

	     if (box1 == NULL)
		complet = FALSE;	/* Rien a mettre en ligne */
	     else
	       {
		  GetLine (&adligne2);
		  pBox->BxFirstLine = adligne2;
	       }

	     boxcoup = NULL;
	     adligne1 = NULL;
	     *haut = 0;		/* hauteur de la boite bloc de lignes */
	     org = 0;		/* origine de la nouvelle ligne */
	     interlignage = FALSE;
	  }
	/* Reconstruction partielle du bloc de ligne */
	else
	  {
	     adligne1 = pBox->BxLastLine;
	     pavefils = adligne1->LiLastBox->BxAbstractBox;
	     boxcoup = adligne1->LiLastPiece;
	     *haut = adligne1->LiYOrg + adligne1->LiHeight;	/* hauteur boite bloc de lignes */
	     /* Origine de la future ligne */
	     if (pavefils->AbHorizEnclosing)
	       {
		  /* C'est une boite englobee */
		  org = adligne1->LiYOrg + adligne1->LiHorizRef + itl;
		  /* On utilise l'interligne */
		  interlignage = TRUE;
	       }
	     else
	       {
		  /* La boite n'est pas englobee (Page) */
		  org = *haut;	/* On colle la nouvelle ligne en dessous de celle-ci */
		  interlignage = FALSE;
	       }

	     if (boxcoup != NULL)
	       {
		  boxcoup = boxcoup->BxNexChild;
		  /* Cas particulier de la derniere boite coupee */
		  if (boxcoup != NULL)
		     box1 = adligne1->LiLastBox;
	       }

	     if (box1 == NULL)
		box1 = Suivante (pavefils);

	     if (box1 == NULL)
		complet = FALSE;	/* Rien a mettre en ligne */
	     /* On prepare la nouvelle ligne */
	     else
	       {
		  GetLine (&adligne2);
		  adligne1->LiNext = adligne2;
		  /* Si la 1ere boite nouvellement mise en lignes n'est pas englobee */
		  if (!box1->BxAbstractBox->AbHorizEnclosing)
		    {
		       org = *haut;
		       /* On colle la nouvelle ligne en dessous */
		       interlignage = FALSE;
		    }
	       }
	  }

	/* On cree des lignes tant qu'elles sont pleines */
	while (complet)
	  {
	     /* Initialise la ligne */
	     adligne2->LiPrevious = adligne1;
	     /* On regarde si la boite deborde des lignes */
	     if (!box1->BxAbstractBox->AbHorizEnclosing)
	       {
		  pBo2 = box1;
		  adligne2->LiXOrg = pBo2->BxXOrg;
		  adligne2->LiYOrg = *haut;	/* Colle la boite en dessous de la precedente */
		  adligne2->LiXMax = pBo2->BxWidth;
		  adligne2->LiHeight = pBo2->BxHeight;
		  adligne2->LiFirstBox = box1;
		  adligne2->LiLastBox = box1;
		  adligne2->LiFirstPiece = NULL;
		  adligne2->LiLastPiece = NULL;
		  if (Propage != ToSiblings || pBox->BxVertFlex)
		     org = pBox->BxYOrg + *haut;
		  else
		     org = *haut;
		  DepOrgY (box1, NULL, org - pBo2->BxYOrg, frame);
		  *haut += adligne2->LiHeight;
		  org = *haut;
		  interlignage = FALSE;
	       }
	     else
	       {
		  /* Indentation des lignes */
#ifdef __COLPAGE__
		  if (adligne1 != NULL || pAbbox1->AbTruncatedHead)
#else  /* __COLPAGE__ */
		  if (adligne1 != NULL)
#endif /* __COLPAGE__ */
		    {
		       if (pAbbox1->AbIndent < 0)
			  adligne2->LiXOrg = idl;
		    }
		  else if (pAbbox1->AbIndent > 0)
		     adligne2->LiXOrg = idl;
		  if (adligne2->LiXOrg >= pBox->BxWidth)
		     adligne2->LiXOrg = 0;
		  adligne2->LiXMax = pBox->BxWidth - adligne2->LiXOrg;
		  adligne2->LiFirstBox = box1;
		  adligne2->LiFirstPiece = boxcoup;	/* ou  NULL si la boite est entiere */

		  /* Remplissage de la ligne au maximum */
#ifdef __COLPAGE__
		  RemplirLigne (adligne2, FntrTable[frame - 1].FrAbstractBox, pAbbox1->AbTruncatedTail, &complet, &ajustif);
#else  /* __COLPAGE__ */
		  RemplirLigne (adligne2, FntrTable[frame - 1].FrAbstractBox, &complet, &ajustif);
#endif /* __COLPAGE__ */

		  /* Positionnement de la ligne en respectant l'interligne */
		  if (interlignage)
		     org -= adligne2->LiHorizRef;
		  /* On verifie que les lignes ne se chevauchent pas */
		  if (org < *haut)
		     org = *haut;

		  adligne2->LiYOrg = org;
		  *haut = org + adligne2->LiHeight;
		  org = org + adligne2->LiHorizRef + itl;
		  interlignage = TRUE;
		  /* Teste le cadrage des lignes */
#ifdef __COLPAGE__
		  if (ajustif && (complet || pAbbox1->AbTruncatedTail) && pAbbox1->AbJustify)
#else  /* __COLPAGE__ */
		  if (ajustif && complet && pAbbox1->AbJustify)
#endif /* __COLPAGE__ */
		     Ajuster (pBox, adligne2, frame, absoluEnX, absoluEnY);
		  else
		    {
		       x = adligne2->LiXOrg;
		       if (pAbbox1->AbAdjust == AlignCenter)
			  x += (adligne2->LiXMax - adligne2->LiRealLength) / 2;
		       else if (pAbbox1->AbAdjust == AlignRight)
			  x = x + adligne2->LiXMax - adligne2->LiRealLength;
		       /* Decale toutes les boites de la ligne */
		       Aligner (pBox, adligne2, x, frame, absoluEnX, absoluEnY);
		    }
	       }

	     /* Est-ce la derniere ligne du bloc de ligne ? */
	     if (complet)
	       {
		  box1 = Suivante (adligne2->LiLastBox->BxAbstractBox);
		  if (adligne2->LiLastPiece == NULL)
		     boxcoup = NULL;
		  else
		    {
		       boxcoup = adligne2->LiLastPiece->BxNexChild;
		       /* Est-ce qu'il y a une boite coupee ? */
		       if (boxcoup != NULL)
			  /* Est-ce une boite vide ? */
			  if (boxcoup->BxNChars > 0)
			     box1 = adligne2->LiLastBox;
			  else if (box1 == NULL)
			     box1 = adligne2->LiLastBox;
			  else
			     boxcoup = NULL;
		    }

		  if (box1 == NULL)
		    {
		       /* Il n'y a plus de boite a traiter */
		       complet = FALSE;
		       pBox->BxLastLine = adligne2;
		    }
		  else
		    {
		       /* On prepare la ligne suivante */
		       adligne1 = adligne2;
		       adligne3 = adligne2;
		       GetLine (&adligne2);
		       adligne3->LiNext = adligne2;
		    }
	       }
	     else
	       {
		  pBox->BxLastLine = adligne2;
		  /* Note la largeur de la fin de bloc si le remplissage est demande */
		  if (pAbbox1->AbAdjust == AlignLeftDots)
		    {
		       pLi2 = adligne2;
		       if (pLi2->LiLastPiece != NULL)
			  box1 = pLi2->LiLastPiece;
		       else
			 {
			    box1 = pLi2->LiLastBox;
			    /* regarde si la derniere boite est invisible */
			    while (box1->BxWidth == 0 && box1 != pLi2->LiFirstBox && box1 != pLi2->LiFirstPiece)
			       box1 = box1->BxPrevious;
			 }
		       box1->BxEndOfBloc = pBox->BxXOrg + pBox->BxWidth - box1->BxXOrg - box1->BxWidth;
		    }
	       }
	  }

	/* On met a jour la base du bloc de lignes   */
	/* s'il depend de la premiere boite englobee */
	if (pAbbox1->AbHorizRef.PosAbRef == pAbbox1->AbFirstEnclosed && pBox->BxFirstLine != NULL)
	  {
	     pPavP1 = &pAbbox1->AbHorizRef;
	     x = PixelValue (pPavP1->PosDistance, pPavP1->PosUnit, pAbbox1);
	     DepBase (pBox, NULL, pBox->BxFirstLine->LiHorizRef + x - pBox->BxHorizRef, frame);
	  }
     }
   else
      *haut = FontHeight (pBox->BxFont);
}


/* ---------------------------------------------------------------------- */
/* |    DecalLigne decale de x les boites de la ligne de pBox incluse   | */
/* |            dans le boc de ligne Pv et la ligne adligne.            | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void         DecalLigne (PtrLine adligne, PtrAbstractBox Pv, PtrBox pBox, int x, int frame)

#else  /* __STDC__ */
static void         DecalLigne (adligne, Pv, pBox, x, frame)
PtrLine            adligne;
PtrAbstractBox             Pv;
PtrBox            pBox;
int                 x;
int                 frame;

#endif /* __STDC__ */

{
   PtrBox            box1, iboxSave;
   PtrBox            boxN;
   int                 xd, xf;
   int                 yd, yf;
   ViewFrame            *pFrame;
   int                 i;

   boxN = NULL;
   iboxSave = pBox;
   adligne->LiRealLength += x;
   /* On prepare le reaffichage de la ligne */
   EvalAffich = FALSE;
   /* Bornes de reaffichage a priori */
   pFrame = &FntrTable[frame - 1];
   xd = pFrame->FrClipXBegin;
   xf = pFrame->FrClipXEnd;
   yd = iboxSave->BxYOrg + iboxSave->BxHorizRef - adligne->LiHorizRef;
   yf = yd + adligne->LiHeight;

   /* On reaffiche la fin de la ligne */
   if (Pv->AbAdjust == AlignLeft
       || Pv->AbAdjust == AlignLeftDots
   /* on force le cadrage a gauche si le bloc de lignes prend la largeur */
   /* du contenu */
       || Pv->AbBox->BxContentWidth)
     {
	if (adligne->LiLastPiece == NULL)
	   boxN = adligne->LiLastBox;
	else
	   boxN = adligne->LiLastPiece;
	if (boxN != pBox)
	  {
	     xf = boxN->BxXOrg + boxN->BxWidth;
	     if (x > 0)
		xf += x;
	  }

	/* Note la largeur de la fin de bloc si le remplissage est demande! */

	if (Pv->AbAdjust == AlignLeftDots && adligne->LiNext == NULL)
	   boxN->BxEndOfBloc -= x;
     }
   /* On reaffiche toute la ligne */
   else if (Pv->AbAdjust == AlignCenter)
     {
	i = x;
	x = (adligne->LiXMax - adligne->LiRealLength) / 2 - (adligne->LiXMax + i - adligne->LiRealLength) / 2;
	if (adligne->LiFirstPiece == NULL)
	   box1 = adligne->LiFirstBox;
	else
	   box1 = adligne->LiFirstPiece;
	xd = box1->BxXOrg;
	if (x < 0)
	   xd += x;
	if (adligne->LiLastPiece == NULL)
	   boxN = adligne->LiLastBox;
	else
	   boxN = adligne->LiLastPiece;
	if (boxN != pBox)
	   xf = boxN->BxXOrg + boxN->BxWidth;
	if (x < 0)
	   xf -= x;

	/* On decale les boites precedentes */
	DepOrgX (box1, NULL, x, frame);
	while (box1 != pBox)
	  {
	     box1 = Suivante (box1->BxAbstractBox);
	     if (box1->BxType == BoSplit)
		box1 = box1->BxNexChild;
	     DepOrgX (box1, NULL, x, frame);
	  }
	/* On decale les boites suivantes */
	x = -x;
     }
   /* On reaffiche le debut de la ligne */
   else if (Pv->AbAdjust == AlignRight)
     {
	x = -x;
	boxN = pBox;
	if (adligne->LiFirstPiece == NULL)
	   pBox = adligne->LiFirstBox;
	else
	   pBox = adligne->LiFirstPiece;
	xd = iboxSave->BxXOrg;
	if (x < 0)
	   xd += x;
	DepOrgX (pBox, NULL, x, frame);
     }

   /* On decale les boites suivantes */
   while (pBox != boxN && pBox != NULL)
     {
	pBox = Suivante (pBox->BxAbstractBox);
	if (pBox != NULL && pBox->BxNexChild != NULL && pBox->BxType == BoSplit)
	   pBox = pBox->BxNexChild;
	DepOrgX (pBox, NULL, x, frame);
     }
   DefClip (frame, xd, yd, xf, yf);
   EvalAffich = TRUE;
}


/* ---------------------------------------------------------------------- */
/* |    CompresLigne compresse ou e'tend la ligne justifiee suite a`    | */
/* |            l'ajout de dblanc espaces et un ecart de dx pixels.     | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void         CompresLigne (PtrLine adligne, int dx, int frame, int dblanc)

#else  /* __STDC__ */
static void         CompresLigne (adligne, dx, frame, dblanc)
PtrLine            adligne;
int                 dx;
int                 frame;
int                 dblanc;

#endif /* __STDC__ */

{
   int                 nbblanc;
   int                 ilg, opixel;
   int                 bl, rest;
   PtrBox            box1, box2;
   ViewSelection            *pMa1;

   rest = adligne->LiNPixels - dx;
   bl = 0;
   nbblanc = adligne->LiNSpaces;
   EvalAffich = FALSE;

   /* Faut-il changer la largeur des blancs ? */
   if (dx > 0)
      while (rest < 0)
	{
	   rest += nbblanc;
	   bl++;
	}
   else
      while (rest >= nbblanc)
	{
	   rest -= nbblanc;
	   bl--;
	}

   /* On met a jour la ligne */
   adligne->LiNPixels = rest;
   /* Pour chaque blanc insere ou retire on compte la largeur minimale */
   dx -= dblanc * (adligne->LiSpaceWidth - MIN_BLANC);
   adligne->LiMinLength += dx;
   adligne->LiSpaceWidth -= bl;

   /* On prepare le reaffichage de toute la ligne */
   if (adligne->LiFirstPiece != NULL)
      box2 = adligne->LiFirstPiece;
   else
      box2 = adligne->LiFirstBox;
   ilg = box2->BxXOrg;
   DefClip (frame, ilg, box2->BxYOrg + box2->BxHorizRef - adligne->LiHorizRef,
	    ilg + adligne->LiXMax,
	    box2->BxYOrg + box2->BxHorizRef - adligne->LiHorizRef + adligne->LiHeight);

   /* On met a jour chaque boite */
   do
     {
	if (box2->BxType == BoSplit)
	   box1 = box2->BxNexChild;
	else
	   box1 = box2;

	DepOrgX (box1, NULL, ilg - box1->BxXOrg, frame);
	if (box1->BxAbstractBox->AbLeafType == LtText && box1->BxNChars != 0)
	  {
	     box1->BxWidth -= box1->BxNSpaces * bl;
	     box1->BxSpaceWidth = adligne->LiSpaceWidth;

	     /* Repartition des pixels */
	     opixel = box1->BxNPixels;
	     if (rest > box1->BxNSpaces)
		box1->BxNPixels = box1->BxNSpaces;
	     else
		box1->BxNPixels = rest;
	     box1->BxWidth = box1->BxWidth - opixel + box1->BxNPixels;
	     rest -= box1->BxNPixels;

	     /* Faut-il mettre a jour les marques de selection ? */
	     pMa1 = &FntrTable[frame - 1].FrSelectionBegin;
	     if (pMa1->VsBox == box1)
	       {
		  pMa1->VsXPos -= pMa1->VsNSpaces * bl;
		  if (opixel < pMa1->VsNSpaces)
		     pMa1->VsXPos -= opixel;
		  else
		     pMa1->VsXPos -= pMa1->VsNSpaces;
		  if (box1->BxNPixels < pMa1->VsNSpaces)
		     pMa1->VsXPos += box1->BxNPixels;
		  else
		     pMa1->VsXPos += pMa1->VsNSpaces;
	       }
	     pMa1 = &FntrTable[frame - 1].FrSelectionEnd;
	     if (pMa1->VsBox == box1)
	       {
		  pMa1->VsXPos -= pMa1->VsNSpaces * bl;
		  if (opixel < pMa1->VsNSpaces)
		     pMa1->VsXPos -= opixel;
		  else
		     pMa1->VsXPos -= pMa1->VsNSpaces;
		  if (box1->BxNPixels < pMa1->VsNSpaces)
		     pMa1->VsXPos += box1->BxNPixels;
		  else
		     pMa1->VsXPos += pMa1->VsNSpaces;
		  if (pMa1->VsIndBox < box1->BxNChars
		      && pMa1->VsBuffer->BuContent[pMa1->VsIndBuf - 1] == ' ')
		     pMa1->VsXPos -= bl;
	       }
	  }
	ilg += box1->BxWidth;
	box2 = Suivante (box1->BxAbstractBox);
     }
   while (!(box1 == adligne->LiLastBox || box1 == adligne->LiLastPiece));
   EvalAffich = TRUE;
}


/* ---------------------------------------------------------------------- */
/* |    RazJustif recalcule la largueur d'une boite apres suppression de| */
/* |            la justification. Met a jour les marques de selection   | */
/* |            que la boite soit justifiee ou non.                     | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void         RazJustif (PtrBox pBox, int lgblanc)

#else  /* __STDC__ */
static void         RazJustif (pBox, lgblanc)
PtrBox            pBox;
int                 lgblanc;

#endif /* __STDC__ */

{
   int                 x;
   int                 l;

   /* Box justifiee -> On met a jour sa largeur et les marques */
   if (pBox->BxSpaceWidth != 0)
     {
	x = pBox->BxSpaceWidth - lgblanc;	/* blanc justifie - blanc de la police */
	l = pBox->BxWidth - x * pBox->BxNSpaces - pBox->BxNPixels;
	pBox->BxWidth = l;
	pBox->BxSpaceWidth = 0;
	pBox->BxNPixels = 0;
     }
}


/* ---------------------------------------------------------------------- */
/* |    DispCoup libere les boites de coupure qui ont pu etre creees a` | */
/* |            partir de la boite mere pBox.                           | */
/* |            L'indicateur chgDF ou chgFS est bascule si la boite     | */
/* |            referencee par la marque Debut ou Fin de Selection est  | */
/* |            liberee.                                                | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void         DispCoup (PtrBox pBox, int frame, boolean * chgDS, boolean * chgFS)

#else  /* __STDC__ */
static void         DispCoup (pBox, frame, chgDS, chgFS)
PtrBox            pBox;
int                 frame;
boolean            *chgDS;
boolean            *chgFS;

#endif /* __STDC__ */

{
   PtrBox            ibox1;
   PtrBox            ibox2;
   PtrBox            bsuiv;
   int                 x, larg;
   int                 nombre;
   int                 reste;
   int                 diff;
   PtrAbstractBox             pAbbox1;
   ViewFrame            *pFrame;
   ViewSelection            *pMa1;

   if (pBox != NULL)
     {
	pAbbox1 = pBox->BxAbstractBox;
	if (pAbbox1->AbLeafType == LtText)
	  {
	     x = CarWidth (BLANC, pBox->BxFont);
	     /* On met a jour les marques de selection */
	     pFrame = &FntrTable[frame - 1];
	     if (pFrame->FrSelectionBegin.VsBox == pBox)
	       {
		  pFrame->FrSelectionBegin.VsBox = pAbbox1->AbBox;	/* Box entiere */
		  *chgDS = TRUE;
	       }
	     if (pFrame->FrSelectionEnd.VsBox == pBox)
	       {
		  pFrame->FrSelectionEnd.VsBox = pAbbox1->AbBox;	/* Box entiere */
		  *chgFS = TRUE;
	       }

	     /* On va liberer des boites coupure */
	     if (pBox->BxType == BoComplete)
		RazJustif (pBox, x);
	     else
	       {
		  ibox2 = pBox->BxNexChild;		/* 1e boite de coupure liberee */
		  pBox->BxNexChild = NULL;
		  if (pBox->BxType == BoSplit)
		    {
		       /* On met a jour la boite coupee */
		       pBox->BxType = BoComplete;
		       pBox->BxPrevious = ibox2->BxPrevious;
		       /* On transmet la position courante de la boite */
		       pBox->BxXOrg = ibox2->BxXOrg;
		       pBox->BxYOrg = ibox2->BxYOrg;
		       if (pBox->BxPrevious != NULL)
			  pBox->BxPrevious->BxNext = pBox;
		       else
			  FntrTable[frame - 1].FrAbstractBox->AbBox->BxNext = pBox;

		       larg = 0;
		       nombre = 0;
		       reste = 0;
		    }
		  else
		    {
		       /* On met a jour la derniere boite de coupure conservee */
		       RazJustif (pBox, x);
		       larg = pBox->BxWidth;
		       /* Il faut retirer l'espace reserve au tiret d'hyphenation */
		       if (pBox->BxType == BoDotted)
			 {
			    larg -= CarWidth (173, pBox->BxFont);
			    pBox->BxType = BoPiece;
			 }
		       nombre = pBox->BxNSpaces;
		       reste = pBox->BxIndChar + pBox->BxNChars;
		    }

		  /* Libere les boites de coupure */
		  if (ibox2 != NULL)
		    {
		       do
			 {
			    ibox1 = ibox2;
			    RazJustif (ibox1, x);
			    /* Nombre de blancs sautes */
			    diff = ibox1->BxIndChar - reste;
			    if (diff > 0)
			      {
				 larg += diff * x;	/* On ajoute les blancs saute */
				 nombre += diff;
			      }
			    else if (ibox1->BxType == BoDotted)
			       /* Il faut retirer la largeur du tiret d'hyphenation */
			       larg -= CarWidth (173, ibox1->BxFont);

			    /* Si la boite suivante n'est pas vide */
			    if (ibox1->BxNChars > 0)
			      {
				 nombre += ibox1->BxNSpaces;
				 larg += ibox1->BxWidth;
			      }
			    reste = ibox1->BxIndChar + ibox1->BxNChars;
			    bsuiv = ibox1->BxNext;
			    ibox2 = FreeBox (ibox1);

			    /* Prepare la mise a jour des marques de selection */
			    pFrame = &FntrTable[frame - 1];
			    pMa1 = &pFrame->FrSelectionBegin;
			    if (pMa1->VsBox == ibox1)
			      {
				 pMa1->VsBox = pAbbox1->AbBox;	/* Box entiere */
				 *chgDS = TRUE;
			      }
			    pMa1 = &pFrame->FrSelectionEnd;
			    if (pMa1->VsBox == ibox1)
			      {
				 pMa1->VsBox = pAbbox1->AbBox;	/* Box entiere */
				 *chgFS = TRUE;
			      }
			 }
		       while (!(ibox2 == NULL));

		       /* Met a jour la boite de coupure */
		       if (pBox->BxType == BoPiece)
			 {
			    pBox->BxNChars = pBox->BxAbstractBox->AbBox->BxNChars - pBox->BxIndChar;
			    /* Il faut comptabiliser les blancs ignores de fin de boite */
			    reste = reste - pBox->BxIndChar - pBox->BxNChars;
			    if (reste > 0)
			      {
				 larg += reste * x;
				 nombre += reste;
			      }
			    pBox->BxWidth = larg;
			    pBox->BxNSpaces = nombre;
			 }
		       /* Termine la mise a jour des chainages */
		       pBox->BxNext = bsuiv;
		       if (bsuiv != NULL)
			  bsuiv->BxPrevious = pBox;
		       else
			  FntrTable[frame - 1].FrAbstractBox->AbBox->BxPrevious = pBox;

		    }
	       }
	  }
	/* Pour les autres natures */
	else
	  {
	     pFrame = &FntrTable[frame - 1];
	     if (pFrame->FrSelectionBegin.VsBox == pBox)
		*chgDS = TRUE;
	     if (pFrame->FrSelectionEnd.VsBox == pBox)
		*chgFS = TRUE;
	  }
     }
}


/* ---------------------------------------------------------------------- */
/* |    DispBloc libere les lignes acquises pour l'affichage du bloc de | */
/* |            lignes a` partir et y compris la ligne passee en        | */
/* |            parametre. Libere toutes les boites coupees creees pour | */
/* |            ces lignes. L'indicateur chgDF ou chgFS est bascule si  | */
/* |            la boite referencee par la marque Debut ou Fin de       | */
/* |            Selection est liberee.                                  | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                DispBloc (PtrBox pBox, int frame, PtrLine premligne, boolean * chgDS, boolean * chgFS)

#else  /* __STDC__ */
void                DispBloc (pBox, frame, premligne, chgDS, chgFS)
PtrBox            pBox;
int                 frame;
PtrLine            premligne;
boolean            *chgDS;
boolean            *chgFS;

#endif /* __STDC__ */

{
   PtrBox            box1;
   PtrLine            lignesuiv;
   PtrLine            adligne;
   PtrAbstractBox             pAb;

   *chgDS = FALSE;
   *chgFS = FALSE;
   adligne = premligne;
   if (adligne != NULL)
     {
	if (pBox->BxType == BoBlock)
	  {
	     /* Mise a jour du chainage des lignes */
	     box1 = NULL;
	     if (adligne->LiPrevious != NULL)
	       {
		  adligne->LiPrevious->LiNext = NULL;
		  box1 = adligne->LiFirstBox;
		  /* Est-ce que la premiere boite est une boite suite ? */
		  if (adligne->LiPrevious->LiLastBox == box1)
		     DispCoup (adligne->LiFirstPiece, frame, chgDS, chgFS);
		  else
		     DispCoup (box1, frame, chgDS, chgFS);
		  pBox->BxLastLine = adligne->LiPrevious;
	       }
	     else
	       {
		  pBox->BxFirstLine = NULL;
		  pBox->BxLastLine = NULL;
	       }

	     /* Liberation des lignes */
	     while (adligne != NULL)
	       {
		  lignesuiv = adligne->LiNext;
		  FreeLine (adligne);
		  adligne = lignesuiv;
	       }

	     /* Liberation des boites de coupure */
	     if (box1 != NULL)
		box1 = Suivante (box1->BxAbstractBox);
	     else
	       {
		  /* On recherche la premiere boite mise en ligne */
		  pAb = pBox->BxAbstractBox->AbFirstEnclosed;
		  while (box1 == NULL && pAb != NULL)
		     /* Est-ce que le pave est mort ? */
		     if (pAb->AbDead)
		       {
			  box1 = Suivante (pAb);
			  if (box1 != NULL)
			     pAb = box1->BxAbstractBox;
			  else
			     pAb = NULL;
		       }
		  /* Est-ce que le pave est eclate ? */
		     else if (pAb->AbBox->BxType == BoGhost)
			pAb = pAb->AbFirstEnclosed;	/* On descend la hierarchie */
		  /* Sinon c'est la boite du pave */
		     else
			box1 = pAb->AbBox;
	       }

	     while (box1 != NULL)
	       {
		  DispCoup (box1, frame, chgDS, chgFS);
		  box1 = Suivante (box1->BxAbstractBox);
	       }
	  }
     }
}


/* ---------------------------------------------------------------------- */
/* |    ReevalBloc reevalue le bloc de ligne pAb a` partir de la     | */
/* |            ligne lig dans la fenetre frame suite au changement de  | */
/* |            largeur de la boite org.                                | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                ReevalBloc (PtrAbstractBox pAb, PtrLine lig, PtrBox org, int frame)

#else  /* __STDC__ */
void                ReevalBloc (pAb, lig, org, frame)
PtrAbstractBox             pAb;
PtrLine            lig;
PtrBox            org;
int                 frame;

#endif /* __STDC__ */

{
   int                 l, h, haut;
   int                 large;
   boolean             chgDS;
   boolean             chgFS;
   boolean             SvAff;
   unsigned char       icar;
   PtrLine            adligne;
   Propagation      SvPrpg;
   PtrBox            pBox;
   ViewFrame            *pFrame;
   ViewSelection            *pMa1;
   ViewSelection            *pMa0;
   PtrBox            pBo2;

   /* Si la boite est eclatee, on remonte jusqu'a la boite bloc de lignes */
   while (pAb->AbBox->BxType == BoGhost)
      pAb = pAb->AbEnclosing;

   pBox = pAb->AbBox;
   if (pBox != NULL)
     {
	adligne = lig;

	if (adligne == NULL)
	   adligne = pBox->BxFirstLine;

	/* Zone affichee avant modification */
	if (adligne == NULL)
	  {
	     l = 0;
	     h = pBox->BxYOrg;
	  }
	else
	  {
	     l = adligne->LiXOrg + adligne->LiXMax;
	     h = pBox->BxYOrg + adligne->LiYOrg;
	  }

	/* Si l'origne de la reevaluation du bloc de ligne vient d'une boite */
	/* de coupure, il faut deplacer cette origne sur la boite coupee     */
	/* parce que DispBloc va liberer toutes les boites de coupure.       */
	if (org != NULL)
	   if (org->BxType == BoPiece || org->BxType == BoDotted)
	      org = org->BxAbstractBox->AbBox;

	SvAff = EvalAffich;
	EvalAffich = FALSE;
	DispBloc (pBox, frame, adligne, &chgDS, &chgFS);
	if (pBox->BxFirstLine == NULL)
	  {
	     /* On fait reevaluer la mise en lignes et on recupere */
	     /* la hauteur et la largeur du contenu de la boite */
	     EvalComp (pAb, frame, &large, &haut);
	  }
	else
	  {
	     BlocDeLigne (pBox, frame, &haut);
	     large = 0;
	  }
	EvalAffich = SvAff;

	/* Zone affichee apres modification */
	/* Il ne faut pas tenir compte de la boite si elle */
	/* n'est pas encore placee dans l'image concrete   */
/**MIN*/ if (EvalAffich && !pBox->BxXToCompute && !pBox->BxYToCompute)
	  {
	     if (pBox->BxWidth > l)
		l = pBox->BxWidth;
	     l += pBox->BxXOrg;
	     if (haut > pBox->BxHeight)
		DefClip (frame, pBox->BxXOrg, h, l, pBox->BxYOrg + haut);
	     else
		DefClip (frame, pBox->BxXOrg, h, l, pBox->BxYOrg + pBox->BxHeight);
	  }

	/* Faut-il reevaluer les marques de selection ? */
	pFrame = &FntrTable[frame - 1];
	pMa0 = &pFrame->FrSelectionBegin;
	if (chgDS && pMa0->VsBox != NULL)
	  {
	     /* Si la selection a encore un sens */
	     if (pMa0->VsBox->BxAbstractBox != NULL)
		ReevalMrq (pMa0);
	  }

	pMa1 = &pFrame->FrSelectionEnd;
	if (chgFS && pMa1->VsBox != NULL)
	  {
	     /* Si la selection a encore un sens */
	     if (pMa1->VsBox->BxAbstractBox != NULL)
	       {

		  if (pMa1->VsBuffer == pMa0->VsBuffer
		      && pMa1->VsIndBuf == pMa0->VsIndBuf)
		    {
		       pMa1->VsIndBox = pMa0->VsIndBox;
		       pMa1->VsXPos = pMa0->VsXPos;
		       pMa1->VsBox = pMa0->VsBox;
		       pMa1->VsNSpaces = pMa0->VsNSpaces;
		       pMa1->VsLine = pMa0->VsLine;
		    }
		  else
		     ReevalMrq (pMa1);

		  /* Recherche la position limite du caractere */
		  pBo2 = pMa1->VsBox;
		  if (pBo2->BxAbstractBox->AbLeafType != LtText)
		     pMa1->VsXPos += pBo2->BxWidth;
		  else if (pBo2->BxNChars == 0 && pBo2->BxType == BoComplete)
		     pMa1->VsXPos += pBo2->BxWidth;
		  else if (pMa1->VsIndBox == pBo2->BxNChars)
		     pMa1->VsXPos += 2;
		  else
		    {
		       icar = (unsigned char) (pMa1->VsBuffer->BuContent[pMa1->VsIndBuf - 1]);
		       if (icar == BLANC && pBo2->BxSpaceWidth != 0)
			  pMa1->VsXPos += pBo2->BxSpaceWidth;
		       else
			  pMa1->VsXPos += CarWidth (icar, pBo2->BxFont);
		    }
	       }
	  }

	if (large != 0 && large != pBox->BxWidth)
	   ChangeLgContenu (pBox, org, large, 0, frame);
	/* Faut-il conserver la hauteur ? */
	if (haut != 0)
	  {
	     /* Il faut propager la modification de hauteur */
	     SvPrpg = Propage;
	     /*if (SvPrpg == ToChildren) Propage = ToAll; */
	     ChangeHtContenu (pBox, org, haut, frame);
	     Propage = SvPrpg;
	  }
     }
}


/* ---------------------------------------------------------------------- */
/* |    MajBloc met a` jour le bloc de ligne (pave) apres modification  | */
/* |            de la largeur de la boite incluse pBox de dx.           | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                MajBloc (PtrAbstractBox pave, PtrLine adligne, PtrBox pBox, int dx, int dbl, int frame)

#else  /* __STDC__ */
void                MajBloc (pave, adligne, pBox, dx, dbl, frame)
PtrAbstractBox             pave;
PtrLine            adligne;
PtrBox            pBox;
int                 dx;
int                 dbl;
int                 frame;

#endif /* __STDC__ */

{
   int                 reste;
   int                 maxreste;
   PtrBox            box1;
   int                 nbcar;
   int                 nlcour = 0;
   int                 larg, lmax;
   int                 newicar;
   int                 nbblanc;
   PtrTextBuffer      newbuff;
   PtrBox            pBo1;
   AbDimension       *pPavD1;
   PtrLine            pLi2;

   /* Si la boite est eclatee, on remonte jusqu'a la boite bloc de lignes */
   while (pave->AbBox->BxType == BoGhost)
      pave = pave->AbEnclosing;
   pBo1 = pave->AbBox;

   if (adligne != NULL)
     {
	pPavD1 = &pave->AbWidth;
	/* C'est une ligne extensible --> on l'etend */
	if (pBo1->BxContentWidth)
	  {
	     /* Si le bloc de lignes deborde de sa dimension minimale */
	     if (!pPavD1->DimIsPosition && pPavD1->DimMinimum
		 && pBo1->BxWidth + dx < pBo1->BxRuleWidth)
	       {
		  /* Il faut prendre la largeur du minimum */
		  pBo1->BxContentWidth = FALSE;
		  ReevalBloc (pave, NULL, NULL, frame);
	       }
	     else
	       {
		  DecalLigne (adligne, pave, pBox, dx, frame);
		  adligne->LiXMax = adligne->LiRealLength;
		  ModLarg (pBo1, pBo1, NULL, dx, 0, frame);
	       }
	  }
	/* Si le bloc de lignes deborde de sa dimension minimale */
	else if (!pPavD1->DimIsPosition && pPavD1->DimMinimum
		 && adligne->LiRealLength + dx > pBo1->BxWidth)
	  {
	     /* Il faut prendre la largeur du contenu */
	     pBo1->BxContentWidth = TRUE;
	     ReevalBloc (pave, NULL, NULL, frame);
	  }
	/* C'est une ligne non extensible */
	else
	  {
	     /* On calcule la place qu'il reste dans la ligne courante */
	     adligne->LiNSpaces += dbl;
	     maxreste = adligne->LiNSpaces * MAX_BLANC + dx;
	     if (adligne->LiSpaceWidth > 0)
	       {
		  /* Line justifiee */
		  reste = CarWidth (BLANC, pBox->BxFont);
		  nlcour = adligne->LiRealLength + dx - dbl * (adligne->LiSpaceWidth - reste);
		  reste = adligne->LiXMax - adligne->LiMinLength;
	       }
	     else
		reste = adligne->LiXMax - adligne->LiRealLength;

	     /* Est-ce que la boite debordait de la ligne ? */
	     if (pBox->BxWidth - dx > adligne->LiXMax)
	       {
		  reste = adligne->LiXMax - pBox->BxWidth;	/* Pixels liberes dans la ligne */
		  if (reste > 0)
		     ReevalBloc (pave, adligne, NULL, frame);
	       }
	     /* Peut-on compresser ou completer la ligne ? */
	     else if ((dx > 0 && dx <= reste)
		      || (dx < 0 && (reste < maxreste
		  || (adligne->LiPrevious == NULL && adligne->LiNext == NULL))))
		if (adligne->LiSpaceWidth == 0)
		   DecalLigne (adligne, pave, pBox, dx, frame);
		else
		  {
		     CompresLigne (adligne, dx, frame, dbl);
		     adligne->LiRealLength = nlcour;
		  }
	     /* Peut-on eviter la reevaluation du bloc de lignes ? */
	     else if (dx < 0)
	       {

		  /* Peut-on remonter le premier mot de la ligne courante ? */
		  if (adligne->LiPrevious != NULL)
		    {
		       /* Largeur restante */
		       lmax = adligne->LiPrevious->LiXMax - adligne->LiPrevious->LiRealLength - MAX_BLANC;
		       if (adligne->LiFirstPiece != NULL)
			  box1 = adligne->LiFirstPiece;
		       else
			  box1 = adligne->LiFirstBox;
		       if (box1->BxWidth > lmax)
			  if (box1->BxAbstractBox->AbLeafType == LtText && lmax > 0)
			    {
			       /* Elimine le cas des lignes sans blanc */
			       if (adligne->LiNSpaces == 0)
				  lmax = 1;	/* force la reevaluation */
			       else
				 {
				    nbcar = box1->BxNChars;
				    /* On regarde si on peut trouver une coupure */
/**HYP*/ lmax = CarCoup (adligne, box1, lmax, box1->BxFont, &nbcar, &larg,
			 &nbblanc, &newicar, &newbuff);
				 }

			       if (lmax > 0)
				  ReevalBloc (pave, adligne->LiPrevious, NULL, frame);
			    }
			  else
			     lmax = 0;
		    }
		  else
		     lmax = 0;

		  /* Peut-on remonter le premier mot de la ligne suivante ? */
		  if (lmax == 0)
		    {
		       if (adligne->LiNext != NULL)
			 {
			    lmax = adligne->LiXMax - adligne->LiRealLength - dx;
			    pLi2 = adligne->LiNext;
			    if (pLi2->LiFirstPiece != NULL)
			       box1 = pLi2->LiFirstPiece;
			    else
			       box1 = pLi2->LiFirstBox;

			    if (box1->BxWidth > lmax)
			       if (box1->BxAbstractBox->AbLeafType == LtText && lmax > 0)
				 {
				    /* Elimine le cas des lignes sans blanc */
				    if (pLi2->LiNSpaces == 0)
				       lmax = 1;	/* force la reevaluation */
				    else
				      {
					 nbcar = box1->BxNChars;
					 /* On regarde si on peut trouver une coupure */
/**HYP*/ lmax = CarCoup (pLi2, box1, lmax, box1->BxFont,
			 &nbcar, &larg, &nbblanc, &newicar, &newbuff);
				      }
				 }
			       else
				  lmax = 0;
			 }

		       if (lmax > 0)
			  ReevalBloc (pave, adligne, NULL, frame);
		       else if (adligne->LiSpaceWidth == 0)
			  DecalLigne (adligne, pave, pBox, dx, frame);
		       else
			 {
			    CompresLigne (adligne, dx, frame, dbl);
			    adligne->LiRealLength = nlcour;
			 }
		    }
		  else
		     ReevalBloc (pave, adligne, NULL, frame);
	       }
	     else
		ReevalBloc (pave, adligne, NULL, frame);
	  }
     }
}


/* ---------------------------------------------------------------------- */
/* |    EnglLigne assure l'englobement de la boite de frame dans la ligne       | */
/* |            du pave pAb et propage les modifications necessaires.| */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                EnglLigne (PtrBox pBox, int frame, PtrAbstractBox pAb)

#else  /* __STDC__ */
void                EnglLigne (pBox, frame, pAb)
PtrBox            pBox;
int                 frame;
PtrAbstractBox             pAb;

#endif /* __STDC__ */

{
   PtrBox            box1;
   PtrBox            box2;
   int                 sur, h;
   int                 i, sous;
   int                 pos;
   int                 sousdiff;
   PtrLine            pLi1;
   PtrLine            pLi2;
   PtrBox            pBo1;

   /* AbDimension   *pPavD1; */

   pBo1 = pAb->AbBox;
   if (Propage != ToSiblings || pBo1->BxVertFlex)
     {
	pLi1 = DesLigne (pBox);
	/*pPavD1 = &pAb->AbHeight; */

	if (pLi1 != NULL)
	  {
	     sur = 0;
	     sous = 0;

	     /* ===> Est-ce une boite qui echappe a l'englobement (Page) ? */
	     if (!pBox->BxAbstractBox->AbHorizEnclosing)
	       {
		  /* La position de la ligne est inchangee */
		  sur = pBox->BxHorizRef;
		  sous = pBox->BxHeight - pLi1->LiHeight;	/* Decalage des lignes suivantes */
		  pLi1->LiHorizRef = sur;
		  pLi1->LiHeight = pBox->BxHeight;
		  /* On deplace la boite */
		  i = pBo1->BxYOrg + pLi1->LiYOrg - pBox->BxYOrg;
		  DepOrgY (pBox, NULL, i, frame);

		  /* On decale les lignes suivantes */
		  if (sous != 0)
		    {
		       pLi2 = pLi1->LiNext;
		       while (pLi2 != NULL)
			 {
			    pLi2->LiYOrg += sous;
			    if (pLi2->LiFirstPiece != NULL)
			       box2 = pLi2->LiFirstPiece;
			    else
			       box2 = pLi2->LiFirstBox;

			    do
			      {

				 if (box2->BxType == BoSplit)
				    box1 = box2->BxNexChild;
				 else
				    box1 = box2;

				 DepOrgY (box1, NULL, sous, frame);
				 box2 = Suivante (box1->BxAbstractBox);
			      }
			    while (!(box1 == pLi2->LiLastBox || box1 == pLi2->LiLastPiece));

			    pLi2 = pLi2->LiNext;
			 }

		       /* On met a jour la hauteur du bloc de lignes */
		       if (pBo1->BxContentHeight)
			  ChangeHtContenu (pBo1, pBo1, pBo1->BxHeight + sous, frame);
		    }
	       }
	     else
	       {
		  /* On calcule la base et la hauteur sous-base de la ligne */
		  if (pLi1->LiFirstPiece != NULL)
		     box2 = pLi1->LiFirstPiece;
		  else
		     box2 = pLi1->LiFirstBox;

		  /* Boucle sur les boites de la ligne */
		  do
		    {
		       if (box2->BxType == BoSplit)
			  box1 = box2->BxNexChild;
		       else
			  box1 = box2;

		       if (sur < box1->BxHorizRef)
			  sur = box1->BxHorizRef;
		       i = box1->BxHeight - box1->BxHorizRef;
		       if (sous < i)
			  sous = i;
		       /* On passe a la boite suivante */
		       box2 = Suivante (box1->BxAbstractBox);
		    }
		  while (!(box2 == NULL || box1 == pLi1->LiLastBox || box1 == pLi1->LiLastPiece));
		  /* Valeurs limites du bas de la ligne precedente */
		  /* et de la position de reference ligne courante */
		  if (pLi1->LiPrevious != NULL)
		    {
		       h = pLi1->LiPrevious->LiYOrg + pLi1->LiPrevious->LiHeight;
		       pos = pLi1->LiPrevious->LiYOrg + pLi1->LiPrevious->LiHorizRef
			  + PixelValue (pAb->AbLineSpacing, pAb->AbLineSpacingUnit, pAb) - sur;
		    }
		  else
		    {
		       h = 0;
		       pos = 0;
		    }

		  /* On place la boite pBox dans la ligne */

		  i = pLi1->LiYOrg + pLi1->LiHorizRef - pBox->BxHorizRef;
		  /* Si la boite chevauche la ligne precedente ? */
		  if (i < h)
		    {
		       h -= i;	/* deplacement de la ligne */
		       i += h;	/* nouvelle origine */
		       pLi1->LiYOrg = i;
		    }
		  /* S'il faut remonter la ligne */
		  else if ((pLi1->LiYOrg > pos || pLi1->LiPrevious == NULL)
			   && sur < pLi1->LiHorizRef)
		    {
		       h = sur - pLi1->LiHorizRef;	/* deplacement du contenu de ligne */
		       i += h;
		    }
		  else
		     h = 0;

		  i = i + pBo1->BxYOrg - pBox->BxYOrg;
		  DepOrgY (pBox, NULL, i, frame);

		  /* On met a jour le bloc de ligne */
		  i = sur - pLi1->LiHorizRef;	/* ecart de ligne de base */
		  sousdiff = sous - pLi1->LiHeight + pLi1->LiHorizRef;	/*ecart en dessous */
		  pLi1->LiHorizRef = sur;
		  pLi1->LiHeight = sous + sur;

		  /* Est-ce que la ligne (de base) est deplacee ? */
		  if (h != 0 || pLi1->LiPrevious == NULL)
		    {
		       /* Le contenu de la ligne et les lignes suivantes sont affectes */

		       if (i != 0)
			 {

			    /* On met a jour la base du bloc de lignes   */
			    /* s'il depend de la premiere boite englobee */
			    if (pAb->AbHorizRef.PosAbRef == pAb->AbFirstEnclosed)
			       DepBase (pBo1, NULL, i, frame);

			    /* On aligne les boites dans la ligne */
			    if (pLi1->LiFirstPiece != NULL)
			       box2 = pLi1->LiFirstPiece;
			    else
			       box2 = pLi1->LiFirstBox;

			    do
			      {

				 if (box2->BxType == BoSplit)
				    box1 = box2->BxNexChild;
				 else
				    box1 = box2;

				 if (box1 != pBox)
				    DepOrgY (box1, NULL, i, frame);
				 box2 = Suivante (box1->BxAbstractBox);
			      }
			    while (!(box2 == NULL || box1 == pLi1->LiLastBox || box1 == pLi1->LiLastPiece));

			 }

		    }
		  /* Sinon la ligne n'est pas deplacee */
		  else
		     /* Origine+base constante (mise a jour de l'origine) */

		     pLi1->LiYOrg -= i;		/* On deplace la ligne */


		  /* On decale les lignes suivantes */
		  if (pLi1->LiNext != NULL)
		    {

		       /* Valeurs limites du bas de la ligne courante et */
		       /* de la position de reference ligne suivante     */
		       pos = pLi1->LiYOrg + pLi1->LiHorizRef
			  + PixelValue (pAb->AbLineSpacing, pAb->AbLineSpacingUnit, pAb)
			  - pLi1->LiNext->LiHorizRef;
		       i = pLi1->LiYOrg + pLi1->LiHeight;

		       /* Si la ligne chevauche la ligne suivante ? */
		       if (pLi1->LiNext->LiYOrg < i)
			  h = i - pLi1->LiNext->LiYOrg;
		       /* Si les lignes suivantes doivent etre remontees */
		       else if (pLi1->LiNext->LiYOrg > pos && i <= pos)
			  h = pos - pLi1->LiNext->LiYOrg;
		       else if (pLi1->LiNext->LiYOrg > i && i > pos)
			  h = i - pLi1->LiNext->LiYOrg;

		       pLi2 = pLi1->LiNext;
		       while (pLi2 != NULL)
			 {
			    pLi2->LiYOrg += h;
			    if (pLi2->LiFirstPiece != NULL)
			       box2 = pLi2->LiFirstPiece;
			    else
			       box2 = pLi2->LiFirstBox;

			    do
			      {

				 if (box2->BxType == BoSplit)
				    box1 = box2->BxNexChild;
				 else
				    box1 = box2;

				 DepOrgY (box1, NULL, h, frame);
				 box2 = Suivante (box1->BxAbstractBox);
			      }
			    while (!(box1 == pLi2->LiLastBox || box1 == pLi2->LiLastPiece));

			    pLi2 = pLi2->LiNext;
			 }
		    }
		  else
		     /* modification de la hauteur du bloc de ligne */
		     h = i + sousdiff;

		  /* On termine la mise a jour du bloc de lignes */
		  if (h != 0 && pBo1->BxContentHeight)
		     ChangeHtContenu (pBo1, pBo1, pBo1->BxHeight + h, frame);
	       }
	  }
     }
}
