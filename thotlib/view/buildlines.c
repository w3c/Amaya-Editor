
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
/* |            suit adpave.                                            | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
PtrBox            Suivante (PtrAbstractBox adpave)

#else  /* __STDC__ */
PtrBox            Suivante (adpave)
PtrAbstractBox             adpave;

#endif /* __STDC__ */

{
   PtrAbstractBox             nextpave;
   boolean             boucle;
   PtrBox            result;

   /* On verifie que le pave existe toujours */
   if (adpave == NULL)
      result = NULL;
   else
     {
	nextpave = adpave->AbNext;
	boucle = True;
	while (boucle)
	   if (nextpave == NULL)
	      /* Est-ce la derniere boite fille d'une boite eclatee */
	      if (adpave->AbEnclosing->AbBox->BxType == BoGhost)
		{
		   /* On remonte la hierarchie */
		   adpave = adpave->AbEnclosing;
		   nextpave = adpave->AbNext;
		}
	      else
		 boucle = False;
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
		      boucle = False;
		boucle = True;
		adpave = nextpave;
	     }
	   else
	      boucle = False;
	if (nextpave == NULL)
	   result = NULL;
	else
	   result = nextpave->AbBox;
     }
   return result;
}


/* ---------------------------------------------------------------------- */
/* |    Precedente rend l'adresse de la boite associee au pave vivant   | */
/* |            qui precede adpave.                                     | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static PtrBox     Precedente (PtrAbstractBox adpave)

#else  /* __STDC__ */
static PtrBox     Precedente (adpave)
PtrAbstractBox             adpave;

#endif /* __STDC__ */

{
   PtrAbstractBox             nextpave;
   boolean             boucle;
   PtrBox            result;

   nextpave = adpave->AbPrevious;
   boucle = True;
   while (boucle)
      if (nextpave == NULL)
	 /* Est-ce la derniere boite fille d'une boite eclatee */
	 if (adpave->AbEnclosing->AbBox->BxType == BoGhost)
	   {
	      /* On remonte la hierarchie */
	      adpave = adpave->AbEnclosing;
	      nextpave = adpave->AbPrevious;
	   }
	 else
	    boucle = False;
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
	   adpave = nextpave;
	}
      else
	 boucle = False;

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
static int          CarCoup (PtrLine adligne, PtrBox ibox, int ilg, ptrfont font, int *nbcar, int *lgcoup, int *nbblanc, int *newicar, PtrTextBuffer * newbuff)

#else  /* __STDC__ */
static int          CarCoup (adligne, ibox, ilg, font, nbcar, lgcoup, nbblanc, newicar, newbuff)
PtrLine            adligne;
PtrBox            ibox;
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
   *nbcar = ibox->BxNChars;
   *lgcoup = ibox->BxWidth;
   *nbblanc = ibox->BxNSpaces;

   icar = ibox->BxFirstChar;		/* index dans le buffer */
   adbuff = ibox->BxBuffer;
   /* lgnew est la largeur utilisee pour calculer la mise en ligne */
   /* largeur est la largeur reelle du texte                       */
   lgnew = 0;
   largeur = 0;
   lgcar = 0;
   lgmot = 0;
   cptblanc = 0;
   nonfini = True;
   lgblanc = CarWidth (BLANC, font);
   bcomp = lgblanc;
   langue = ibox->BxAbstractBox->AbLanguage;

   if (ilg != ibox->BxWidth - 1)
     {
	/* L'appel de CarCoup ne concerne pas les blancs en fin de paragraphe */
	boxmere = ibox->BxAbstractBox->AbEnclosing->AbBox;
	/* On remonte jusqu'a la boite bloc de lignes */
	while (boxmere->BxType == BoGhost)
	   boxmere = boxmere->BxAbstractBox->AbEnclosing->AbBox;
	/* Si la ligne est justifiee on prend des blancs compresses */
	if (boxmere->BxAbstractBox->AbJustify)
	   bcomp = round ((float) (bcomp * 7) / (float) (10));
     }

   i = 0;
   cpt = ibox->BxNChars;

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
	     nonfini = False;
/*-- Si on veut simplement supprimer les blancs de fin de ligne */
	     if (ilg >= ibox->BxWidth)
	       {
		  /* Pointe le caractere apres la fin de boite */
		  *newicar = 1;
		  *newbuff = NULL;
		  *lgcoup = ibox->BxWidth;
		  *nbcar = ibox->BxNChars;
		  *nbblanc = ibox->BxNSpaces;

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
		nonfini = False;
		/* Pointe le premier caractere apres la coupure */
		*newbuff = adbuff;
		*newicar = icar;
		*lgcoup = ibox->BxWidth;
		*nbcar = ibox->BxNChars;
		*nbblanc = ibox->BxNSpaces;
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
      nonfini = True;
   else if (ilg >= ibox->BxWidth)
      /* Il a peut-etre des blancs a supprimer */
      nonfini = True;
   else
      nonfini = False;

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
		   nonfini = False;
	     else
		icar--;

	     *lgcoup -= lgblanc;
	     saut++;
	     (*nbcar)--;
	     (*nbblanc)--;
	  }
	else
	   nonfini = False;
     }

   if (ilg < ibox->BxWidth && Hyphenable (ibox) && lgmot > 1 && !Insert)
     {
	/* Il faut verifier les coupures en fin de lignes precedentes */
	nonfini = True;
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
		nonfini = False;
	  }			/*if adligne != NULL */

	if (nonfini)
	  {
	     /* On cherche reellement a couper le dernier mot de la boite */
	     if (saut == 0)
	       {
		  /* On essaie de couper le mot unique de la boite */
		  largeur = ilg;
		  adbuff = ibox->BxBuffer;
		  icar = ibox->BxFirstChar;
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
       && (ibox == adligne->LiFirstBox || ibox == adligne->LiFirstPiece)
       && *newbuff != NULL)
     {
	/* Engendre un hyphen */
	saut = -1;
	*lgcoup += CarWidth (173, font);

	/* Retire un ou plusieurs caracteres pour loger l'hyphen */
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
      nonfini = True;
   else
      nonfini = False;

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
		     nonfini = False;
		     (*newicar)++;
		  }
	     else
		(*newicar)++;

	     saut++;
	  }
	else
	   nonfini = False;
     }				/*while */
   return saut;
}


/* ---------------------------------------------------------------------- */
/* |    CouperBoite coupe la boi^te entiere d'indice ibox correspondant | */
/* |            a` l'image abstraite Pv en deux boi^tes. Si force est   | */
/* |            Vrai coupe meme sur un caracte`re, sinon on ne coupe que| */
/* |            sur un blanc. La premie`re boi^te issue de la coupure   | */
/* |            contiendra nbcar caracte`res. Sa longueur sera          | */
/* |            infe'rieure a` lgmax. Le blanc sera perdu. Le reste est | */
/* |            mis dans la boi^te de coupure suivante. Les chai^nages  | */
/* |            sont mis a` jour.                                       | */
/* ---------------------------------------------------------------------- */


#ifdef __STDC__
static void         CouperBoite (PtrLine adligne, PtrBox ibox, int lgmax, PtrAbstractBox Pv, boolean force)

#else  /* __STDC__ */
static void         CouperBoite (adligne, ibox, lgmax, Pv, force)
PtrLine            adligne;
PtrBox            ibox;
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
   PtrAbstractBox             adpave;
   ptrfont             font;

   /* Initialisations */
   adpave = ibox->BxAbstractBox;
   haut = ibox->BxHeight;
   base = ibox->BxHorizRef;
   avant = ibox->BxPrevious;
   apres = ibox->BxNext;
   font = ibox->BxFont;

   /* Si coupure sur un blanc le blanc de coupure est perdu */
   reste = CarCoup (adligne, ibox, lgmax, font, &nbcar, &larg, &nbblanc, &newicar, &newbuff);

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
       && (ibox->BxWidth != lgmax || reste != ibox->BxNSpaces))
     {
	ibox1 = GetBox (adpave);
	ibox2 = GetBox (adpave);
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
	ibox1->BxContentWidth = True;
	ibox1->BxContentHeight = True;
	ibox1->BxFont = font;
	ibox1->BxUnderline = ibox->BxUnderline;
	ibox1->BxThickness = ibox->BxThickness;
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
	ibox1->BxBuffer = ibox->BxBuffer;
	ibox1->BxNChars = nbcar;
	ibox1->BxNSpaces = nbblanc;
	ibox1->BxFirstChar = ibox->BxFirstChar;

	/* Initialise le contenu de la deuxieme boite */
	ibox2->BxContentWidth = True;
	ibox2->BxContentHeight = True;
	ibox2->BxHeight = haut;
	ibox2->BxFont = font;
	ibox2->BxUnderline = ibox->BxUnderline;
	ibox2->BxThickness = ibox->BxThickness;
	ibox2->BxHorizRef = base;
	ibox2->BxType = BoPiece;
	ibox2->BxBuffer = newbuff;
	ibox2->BxIndChar = nbcar + reste;
	ibox2->BxFirstChar = newicar;
	ibox2->BxNChars = ibox->BxNChars - reste - nbcar;
	ibox2->BxNSpaces = ibox->BxNSpaces - reste - nbblanc;
	ibox2->BxWidth = ibox->BxWidth - larg - reste * lgbl;

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
	ibox->BxType = BoSplit;
	ibox->BxNexChild = ibox1;
     }

}


/* ---------------------------------------------------------------------- */
/* | Coupe la boite ibox sur un caracte`re de coupure force'e.          | */
/* | large = la largeur du de'but de la boite jusqu'au point de coupure | */
/* | lgcarsp = largeur du caracte`re qui force la coupure               | */
/* | nbcarsl = nombre de caracte`res avant la coupure                   | */
/* | nbblanc = nombre d'espaces avant le point de coupure               | */
/* | newicar = indice du debut de texte apres le point de coupure       | */
/* | newbuff = buffer du debut de texte apres le point de coupure       | */
/* | Pv = AbstractBox' racine de la vue (pour mise a jour des chainages)       | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void         cutbox (PtrBox ibox, int large, int lgcarsp, int nbcarsl, int nbblanc, int newicar, PtrTextBuffer newbuff, PtrAbstractBox Pv)

#else  /* __STDC__ */
static void         cutbox (ibox, large, lgcarsp, nbcarsl, nbblanc, newicar, newbuff, Pv)
PtrBox            ibox;
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
   PtrAbstractBox             adpave;
   ptrfont             font;


   /* Initialisation */
   ibox1 = NULL;
   adpave = ibox->BxAbstractBox;
   haut = ibox->BxHeight;
   base = ibox->BxHorizRef;
   avant = ibox->BxPrevious;
   apres = ibox->BxNext;
   font = ibox->BxFont;

   /* Creation de la boite de coupure pour le texte apres la coupure */
   if (newbuff != NULL)
     {
	/* Si la boite est entiere, il faut une boite de coupure */
	/* pour contenir le texte avant le point de coupure      */
	if (ibox->BxType == BoComplete)
	  {
	     ibox1 = GetBox (adpave);
	     if (ibox1 == NULL)
		goto Label_10;	/* plus de boite */
	  }
	ibox2 = GetBox (adpave);
     }
   else
      ibox2 = NULL;

   /* Initialisation de cette boite */
   if (ibox2 != NULL)
     {
	ibox2->BxContentWidth = True;
	ibox2->BxContentHeight = True;
	ibox2->BxHeight = haut;
	ibox2->BxFont = font;
	ibox2->BxUnderline = ibox->BxUnderline;
	ibox2->BxThickness = ibox->BxThickness;
	ibox2->BxHorizRef = base;
	ibox2->BxType = BoPiece;
	ibox2->BxBuffer = newbuff;
	ibox2->BxNexChild = NULL;
	ibox2->BxIndChar = ibox->BxIndChar + nbcarsl + 1;
	ibox2->BxFirstChar = newicar;
	ibox2->BxNChars = ibox->BxNChars - nbcarsl - 1;
	ibox2->BxNSpaces = ibox->BxNSpaces - nbblanc;
	ibox2->BxWidth = ibox->BxWidth - large - lgcarsp;

	/* Si la boite est entiere, il faut une boite de coupure */
	/* pour contenir le texte avant le point de coupure      */
	if (ibox->BxType == BoComplete)
	  {
	     /* Initialisation de la boite ibox1 */
	     ibox1->BxContentWidth = True;
	     ibox1->BxContentHeight = True;
	     ibox1->BxIndChar = 0;
	     ibox1->BxUnderline = ibox->BxUnderline;
	     ibox1->BxThickness = ibox->BxThickness;
	     ibox1->BxFont = font;
	     ibox1->BxHeight = haut;
	     ibox1->BxHorizRef = base;
	     ibox1->BxType = BoPiece;
	     ibox1->BxBuffer = ibox->BxBuffer;
	     ibox1->BxFirstChar = ibox->BxFirstChar;
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
	     ibox->BxType = BoSplit;
	     ibox->BxNexChild = ibox1;
	  }
	/* Si la boite est deja une boite de coupure, il faut la mettre a jour */
	else
	  {
	     ibox->BxWidth = large;
	     ibox->BxNSpaces = nbblanc;
	     ibox->BxNChars = nbcarsl;

	     /* Modifie les boites terminales */
	     ibox->BxNexChild = ibox2;
	     ibox->BxNext = ibox2;
	     ibox2->BxPrevious = ibox;
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
/* | Teste s'il y a un caracte`re de coupure force'e dans la boite ibox | */
/* | Si oui retourne la valeur Vrai, sinon la valeur False.             | */
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
boolean             testcsl (PtrBox ibox, int *large, int *lgcarspec, int *nbcarsl, int *nbblanc, int *newicar, PtrTextBuffer * newbuff)

#else  /* __STDC__ */
boolean             testcsl (ibox, large, lgcarspec, nbcarsl, nbblanc, newicar, newbuff)
PtrBox            ibox;
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


   csexist = False;
   *large = 0;
   *nbcarsl = 0;
   *nbblanc = 0;
   j = 1;
   adbuff = ibox->BxBuffer;
   nbcar = ibox->BxNChars;
   font = ibox->BxFont;
   i = ibox->BxFirstChar;
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
	     csexist = True;
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
/* |    ReCouperBoite recoupe la boite ibox deja coupee pour l'image    | */
/* |            abstraite Pv. La coupure a lieu meme s'il n'y a pas de  | */
/* |            blanc (coupure forcee). La boite ibox contiendra nbcar  | */
/* |            caracteres.                                             | */
/* ---------------------------------------------------------------------- */


#ifdef __STDC__
static void         ReCouperBoite (PtrLine adligne, PtrBox ibox, int lgmax, PtrAbstractBox Pv)

#else  /* __STDC__ */
static void         ReCouperBoite (adligne, ibox, lgmax, Pv)
PtrLine            adligne;
PtrBox            ibox;
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
   PtrAbstractBox             adpave;
   ptrfont             font;
   int                 oldlarge, oldnbbl;

   adpave = ibox->BxAbstractBox;
   haut = ibox->BxHeight;
   base = ibox->BxHorizRef;
   apres = ibox->BxNext;
   pl = ibox->BxIndChar;
   oldlg = ibox->BxNChars;
   font = ibox->BxFont;
   oldlarge = ibox->BxWidth;
   oldnbbl = ibox->BxNSpaces;

   /* Si coupure sur un blanc le blanc de coupure est perdu */
   reste = CarCoup (adligne, ibox, lgmax, font, &nbcar, &larg, &nbblanc, &newicar, &newbuff);
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
       && (ibox->BxWidth != lgmax || reste != ibox->BxNSpaces))
      box2 = GetBox (adpave);
   else
      box2 = NULL;


   /* Coupure du mot sans generation du tiret */
   if (reste == -2)
      reste = 0;

   /* Initialise le contenu de cette nouvelle boite */
   if (box2 != NULL)
     {

	ibox->BxNexChild = box2;
	ibox->BxNext = box2;

	/* Mise a jour de la  boite recoupee */
	ibox->BxNChars = nbcar;
	ibox->BxWidth = larg;
	ibox->BxNSpaces = nbblanc;
	if (reste == -1)
	  {
	     /* La boite doit etre completee par un tiret d'hyphenation */
	     ibox->BxType = BoDotted;
	     larg -= CarWidth (173, font);
	     reste = 0;		/* il n'y a pas de blanc saute */
	  }
	else
	   ibox->BxType = BoPiece;

	box2->BxContentWidth = True;
	box2->BxContentHeight = True;
	box2->BxHeight = haut;
	box2->BxFont = font;
	box2->BxUnderline = ibox->BxUnderline;
	box2->BxThickness = ibox->BxThickness;
	box2->BxHorizRef = base;
	box2->BxType = BoPiece;
	box2->BxBuffer = newbuff;
	box2->BxNexChild = NULL;

	/* Modifie le chainage des boites terminales */
	box2->BxPrevious = ibox;
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
void                AjoutBoite (PtrBox ibox, int *sousbase, int *surbase, PtrLine adligne)

#else  /* __STDC__ */
void                AjoutBoite (ibox, sousbase, surbase, adligne)
PtrBox            ibox;
int                *sousbase;
int                *surbase;
PtrLine            adligne;

#endif /* __STDC__ */

{
   adligne->LiRealLength += ibox->BxWidth;
   /* Calcule la hauteur courante de la ligne */
   if (*surbase < ibox->BxHorizRef)
      *surbase = ibox->BxHorizRef;
   if (*sousbase < ibox->BxHeight - ibox->BxHorizRef)
      *sousbase = ibox->BxHeight - ibox->BxHorizRef;
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
   PtrBox            ibox;
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

   *plein = True;
   *ajust = True;
   coup = False;
   nonfini = True;
   xmax = adligne->LiXMax;
   xi = 0;
   surbase = 0;
   sousbase = 0;
   nbox = adligne->LiFirstBox;	/* la boite que l'on traite */
   adligne->LiLastPiece = NULL;
   csexist = False;

   /* nbox est la boite courante que l'on traite    */
   /* ibox est la derniere boite mise dans la ligne */

   /* La premiere boite est la suite d'une boite deja coupee ? */
   if (adligne->LiFirstPiece != NULL)
     {
	/* La boite de coupure entre obligatoirement dans la ligne */
	ibox = adligne->LiFirstPiece;

	/* Est-ce que l'on rencontre une coupure force dans la boite ? */
	csexist = testcsl (ibox, &large, &lgcarspec, &nbcarsl, &nbblanc, &newicar, &newbuff);
	/* La coupure forcee arrive avant la fin de ligne */
	if (csexist && large + xi <= xmax)
	  {
	     cutbox (ibox, large, lgcarspec, nbcarsl, nbblanc, newicar, newbuff, Pv);
	     *ajust = False;
	     nonfini = False;
	  }
	/* La boite entre dans toute entiere dans la ligne */
	else if (ibox->BxWidth + xi <= xmax)
	  {
	     nbox = Suivante (ibox->BxAbstractBox);
	     if (nbox == NULL)
	       {
		  *plein = False;
		  nonfini = False;
	       }
	  }
	/* Il faut couper une autre fois la boite */
	else
	  {
	     nonfini = False;
	     ReCouperBoite (adligne, ibox, xmax - xi, Pv);
	  }

	ibox = adligne->LiFirstPiece;
	xi += ibox->BxWidth;
     }
   else
      /* Il n'y a pas encore de boite dans la ligne */
      ibox = NULL;

   /* On recherche la boite qu'il faut couper */
   while (nonfini)

      /* On refuse de mettre en ligne une boite non englobee */
      if (!nbox->BxAbstractBox->AbHorizEnclosing)
	{
	   *plein = True;
	   nonfini = False;

	   /* Est-ce la premiere boite de la ligne ? */
	   if (ibox == NULL)
	      adligne->LiLastPiece = adligne->LiFirstPiece;
	   /* Est-ce que la boite precedente est coupee ? */
	   else if (ibox->BxType == BoPiece || ibox->BxType == BoDotted)
	      adligne->LiLastPiece = adligne->LiFirstPiece;
	}
      else
	{
	   /* Recherche si on doit forcer un return dans la boite */
	   if (nbox->BxAbstractBox->AbAcceptLineBreak)
	      csexist = testcsl (nbox, &large, &lgcarspec, &nbcarsl, &nbblanc, &newicar, &newbuff);
	   else
	      csexist = False;

	   /* La boite produite par le return force' tient dans la ligne ? */
	   if (csexist && large + xi <= xmax)
	     {
		nonfini = False;
		cutbox (nbox, large, lgcarspec, nbcarsl, nbblanc, newicar, newbuff, Pv);
		*ajust = False;
		if (nbox->BxNexChild != NULL)
		  {
		     ibox = nbox->BxNexChild;
		     /* Est-ce la boite unique de la ligne ? */
		     if (nbox == adligne->LiFirstBox)
			adligne->LiFirstPiece = ibox;
		  }
	     }
	   /* La boite tient dans la ligne ? */
	   else if (nbox->BxWidth + xi <= xmax)
	     {
		ibox = nbox;	/* La boite entre dans la ligne */
		xi += nbox->BxWidth;
		nbox = Suivante (nbox->BxAbstractBox);
		if (nbox == NULL)
		  {
		     *plein = False;
		     nonfini = False;
		  }
	     }
	   /* Il faut couper la boite ou une boite precedente */
	   else
	     {
		coup = True;
		nonfini = False;
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
	      ibox = nbox;	/* derniere boite de la ligne */
	/* Sinon on coupe la boite texte en un point quelconque */
	   else
	     {
		CouperBoite (adligne, nbox, lmax, Pv, True);	/* coupure forcee */
		if (nbox->BxNexChild != NULL)
		  {
		     ibox = nbox->BxNexChild;
		     adligne->LiFirstPiece = ibox;
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
		  CouperBoite (adligne, nbox, lmax, Pv, False);		/* coupure sur blanc */
		  if (nbox->BxNexChild != NULL)
		    {
		       /* On a trouve le point de coupure */
		       ibox = nbox->BxNexChild;
		       coup = False;
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
		     coup = False;
		  else if (nbox == adligne->LiFirstBox)
		    {
		       coup = False;
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
			  ibox = nbox;
		       }
		     else
		       {
			  CouperBoite (adligne, nbox, nbox->BxWidth - 1, Pv, False);
			  ibox = nbox->BxNexChild;
			  /* Est-ce que l'on a coupe la premiere boite de la ligne ? */
			  if (nbox == adligne->LiFirstBox && adligne->LiFirstPiece == NULL)
			     adligne->LiFirstPiece = ibox;
			  coup = False;
		       }
		  /* Si la coupure sur un blanc echoue on coupe la derniere boite de la ligne */
		  else if (!coup)
		    {
		       /* Si la derniere boite est secable -> force sa coupure */
		       if (lastbox->BxAbstractBox->AbAcceptLineBreak
			   && lastbox->BxAbstractBox->AbLeafType == LtText)
			 {
			    CouperBoite (adligne, lastbox, lmax, Pv, True);	/* coupure forcee */
			    ibox = lastbox->BxNexChild;
			 }
		       /* Si la boite est seule dans la ligne -> laisse deborder */
		       else if (lastbox == adligne->LiFirstBox)
			  ibox = lastbox;
		       /* sinon on coupe avant la derniere boite */
		       else
			 {
			    ibox = Precedente (lastbox->BxAbstractBox);
			    /* Si c'est la premiere boite de la ligne et que celle-ci ets coupee */
			    if (ibox == adligne->LiFirstBox && adligne->LiFirstPiece != NULL)
			       ibox = adligne->LiFirstPiece;
			 }
		    }
		  /* Sinon on continue la recherche en arriere */
		  else
		     ibox = nbox;
	       }
	  }
     }

   /* On ajoute toutes les boites de la ligne */
   if (adligne->LiFirstPiece != NULL)
      nbox = adligne->LiFirstPiece;
   else
      nbox = adligne->LiFirstBox;

   nonfini = True;
   while (nonfini)
     {
	if (nbox->BxType == BoSplit)
	   nbox = nbox->BxNexChild;
	AjoutBoite (nbox, &sousbase, &surbase, adligne);
	if (nbox == ibox)
	   nonfini = False;
	else
	   nbox = Suivante (nbox->BxAbstractBox);
     }

   /* On termine le chainage */
   if (ibox->BxType == BoPiece || ibox->BxType == BoDotted)
     {
	adligne->LiLastPiece = ibox;
	adligne->LiLastBox = ibox->BxAbstractBox->AbBox;
     }
   else
      adligne->LiLastBox = ibox;

   /* On teste s'il reste des boites a mettre en ligne */
   if (ibox->BxNexChild == NULL && Suivante (ibox->BxAbstractBox) == NULL)
      *plein = False;

   /* On coupe les blancs en fin de ligne pleine */
#ifdef __COLPAGE
   if ((uneSuite || *plein) && ibox->BxAbstractBox->AbLeafType == LtText && ibox->BxNSpaces != 0)
#else  /* __COLPAGE__ */
   if (*plein && ibox->BxAbstractBox->AbLeafType == LtText && ibox->BxNSpaces != 0)
#endif /* __COLPAGE__ */
      if (adligne->LiLastPiece == NULL)
	{
	   lmax = ibox->BxWidth;
	   CouperBoite (adligne, ibox, lmax, Pv, False);	/*coupure sur un caractere refusee */
	   if (ibox->BxNexChild != NULL)
	     {
		/* On remplace la boite entiere par la boite de coupure */
		adligne->LiRealLength = adligne->LiRealLength - lmax + ibox->BxNexChild->BxWidth;
		adligne->LiLastPiece = ibox->BxNexChild;
	     }
	}
      else if (adligne->LiLastPiece->BxNexChild == NULL)
	{
	   ibox = adligne->LiLastPiece;
	   lmax = ibox->BxWidth;
	   ReCouperBoite (adligne, ibox, lmax, Pv);
	   /* On met a jour la largeur de la ligne */
	   adligne->LiRealLength = adligne->LiRealLength - lmax + ibox->BxWidth;
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
void                BlocDeLigne (PtrBox ibox, int frame, int *haut)

#else  /* __STDC__ */
void                BlocDeLigne (ibox, frame, haut)
PtrBox            ibox;
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
   PtrAbstractBox             pPa1;
   PtrBox            pBo2;
   PtrLine            pLi2;
   AbPosition        *pPavP1;
   boolean             ajustif;
   boolean             absoluEnX;
   boolean             absoluEnY;

   /* Remplissage de la boite bloc de ligne */
   x = 0;
   pPa1 = ibox->BxAbstractBox;

   /* evalue si le positionnement en X et en Y doit etre absolu */
   XYEnAbsolu (ibox, &absoluEnX, &absoluEnY);

   if (ibox->BxWidth > CarWidth (119, ibox->BxFont))	/*'w' */
     {
	/* Acquisition d'une ligne */
	complet = True;
	box1 = NULL;
	/* Calcul de l'interligne */
	itl = PixelValue (pPa1->AbLineSpacing, pPa1->AbLineSpacingUnit, pPa1);
	/* Calcul de l'indentation */
	if (pPa1->AbIndentUnit == UnPercent)
	   idl = PixelValue (pPa1->AbIndent, UnPercent, ibox->BxWidth);
	else
	   idl = PixelValue (pPa1->AbIndent, pPa1->AbIndentUnit, pPa1);
	if (pPa1->AbIndent < 0)
	   idl = -idl;

	/* Construction complete du bloc de ligne */
	if (ibox->BxFirstLine == NULL)
	  {
	     /* On recherche la premiere boite mise en ligne */
	     pavefils = pPa1->AbFirstEnclosed;
	     encore = pavefils != NULL;
	     while (encore)
		/* Est-ce que le pave est mort ? */
		if (pavefils->AbDead)
		  {
		     box1 = Suivante (pavefils);
		     encore = False;
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
		     encore = False;
		  }

	     if (box1 == NULL)
		complet = False;	/* Rien a mettre en ligne */
	     else
	       {
		  GetLine (&adligne2);
		  ibox->BxFirstLine = adligne2;
	       }

	     boxcoup = NULL;
	     adligne1 = NULL;
	     *haut = 0;		/* hauteur de la boite bloc de lignes */
	     org = 0;		/* origine de la nouvelle ligne */
	     interlignage = False;
	  }
	/* Reconstruction partielle du bloc de ligne */
	else
	  {
	     adligne1 = ibox->BxLastLine;
	     pavefils = adligne1->LiLastBox->BxAbstractBox;
	     boxcoup = adligne1->LiLastPiece;
	     *haut = adligne1->LiYOrg + adligne1->LiHeight;	/* hauteur boite bloc de lignes */
	     /* Origine de la future ligne */
	     if (pavefils->AbHorizEnclosing)
	       {
		  /* C'est une boite englobee */
		  org = adligne1->LiYOrg + adligne1->LiHorizRef + itl;
		  /* On utilise l'interligne */
		  interlignage = True;
	       }
	     else
	       {
		  /* La boite n'est pas englobee (Page) */
		  org = *haut;	/* On colle la nouvelle ligne en dessous de celle-ci */
		  interlignage = False;
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
		complet = False;	/* Rien a mettre en ligne */
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
		       interlignage = False;
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
		  if (Propage != ToSiblings || ibox->BxVertFlex)
		     org = ibox->BxYOrg + *haut;
		  else
		     org = *haut;
		  DepOrgY (box1, NULL, org - pBo2->BxYOrg, frame);
		  *haut += adligne2->LiHeight;
		  org = *haut;
		  interlignage = False;
	       }
	     else
	       {
		  /* Indentation des lignes */
#ifdef __COLPAGE__
		  if (adligne1 != NULL || pPa1->AbTruncatedHead)
#else  /* __COLPAGE__ */
		  if (adligne1 != NULL)
#endif /* __COLPAGE__ */
		    {
		       if (pPa1->AbIndent < 0)
			  adligne2->LiXOrg = idl;
		    }
		  else if (pPa1->AbIndent > 0)
		     adligne2->LiXOrg = idl;
		  if (adligne2->LiXOrg >= ibox->BxWidth)
		     adligne2->LiXOrg = 0;
		  adligne2->LiXMax = ibox->BxWidth - adligne2->LiXOrg;
		  adligne2->LiFirstBox = box1;
		  adligne2->LiFirstPiece = boxcoup;	/* ou  NULL si la boite est entiere */

		  /* Remplissage de la ligne au maximum */
#ifdef __COLPAGE__
		  RemplirLigne (adligne2, FntrTable[frame - 1].FrAbstractBox, pPa1->AbTruncatedTail, &complet, &ajustif);
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
		  interlignage = True;
		  /* Teste le cadrage des lignes */
#ifdef __COLPAGE__
		  if (ajustif && (complet || pPa1->AbTruncatedTail) && pPa1->AbJustify)
#else  /* __COLPAGE__ */
		  if (ajustif && complet && pPa1->AbJustify)
#endif /* __COLPAGE__ */
		     Ajuster (ibox, adligne2, frame, absoluEnX, absoluEnY);
		  else
		    {
		       x = adligne2->LiXOrg;
		       if (pPa1->AbAdjust == AlignCenter)
			  x += (adligne2->LiXMax - adligne2->LiRealLength) / 2;
		       else if (pPa1->AbAdjust == AlignRight)
			  x = x + adligne2->LiXMax - adligne2->LiRealLength;
		       /* Decale toutes les boites de la ligne */
		       Aligner (ibox, adligne2, x, frame, absoluEnX, absoluEnY);
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
		       complet = False;
		       ibox->BxLastLine = adligne2;
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
		  ibox->BxLastLine = adligne2;
		  /* Note la largeur de la fin de bloc si le remplissage est demande */
		  if (pPa1->AbAdjust == AlignLeftDots)
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
		       box1->BxEndOfBloc = ibox->BxXOrg + ibox->BxWidth - box1->BxXOrg - box1->BxWidth;
		    }
	       }
	  }

	/* On met a jour la base du bloc de lignes   */
	/* s'il depend de la premiere boite englobee */
	if (pPa1->AbHorizRef.PosAbRef == pPa1->AbFirstEnclosed && ibox->BxFirstLine != NULL)
	  {
	     pPavP1 = &pPa1->AbHorizRef;
	     x = PixelValue (pPavP1->PosDistance, pPavP1->PosUnit, pPa1);
	     DepBase (ibox, NULL, ibox->BxFirstLine->LiHorizRef + x - ibox->BxHorizRef, frame);
	  }
     }
   else
      *haut = FontHeight (ibox->BxFont);
}


/* ---------------------------------------------------------------------- */
/* |    DecalLigne decale de x les boites de la ligne de ibox incluse   | */
/* |            dans le boc de ligne Pv et la ligne adligne.            | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void         DecalLigne (PtrLine adligne, PtrAbstractBox Pv, PtrBox ibox, int x, int frame)

#else  /* __STDC__ */
static void         DecalLigne (adligne, Pv, ibox, x, frame)
PtrLine            adligne;
PtrAbstractBox             Pv;
PtrBox            ibox;
int                 x;
int                 frame;

#endif /* __STDC__ */

{
   PtrBox            box1, iboxSave;
   PtrBox            boxN;
   int                 xd, xf;
   int                 yd, yf;
   ViewFrame            *pFe1;
   int                 i;

   boxN = NULL;
   iboxSave = ibox;
   adligne->LiRealLength += x;
   /* On prepare le reaffichage de la ligne */
   EvalAffich = False;
   /* Bornes de reaffichage a priori */
   pFe1 = &FntrTable[frame - 1];
   xd = pFe1->FrClipXBegin;
   xf = pFe1->FrClipXEnd;
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
	if (boxN != ibox)
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
	if (boxN != ibox)
	   xf = boxN->BxXOrg + boxN->BxWidth;
	if (x < 0)
	   xf -= x;

	/* On decale les boites precedentes */
	DepOrgX (box1, NULL, x, frame);
	while (box1 != ibox)
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
	boxN = ibox;
	if (adligne->LiFirstPiece == NULL)
	   ibox = adligne->LiFirstBox;
	else
	   ibox = adligne->LiFirstPiece;
	xd = iboxSave->BxXOrg;
	if (x < 0)
	   xd += x;
	DepOrgX (ibox, NULL, x, frame);
     }

   /* On decale les boites suivantes */
   while (ibox != boxN && ibox != NULL)
     {
	ibox = Suivante (ibox->BxAbstractBox);
	if (ibox != NULL && ibox->BxNexChild != NULL && ibox->BxType == BoSplit)
	   ibox = ibox->BxNexChild;
	DepOrgX (ibox, NULL, x, frame);
     }
   DefClip (frame, xd, yd, xf, yf);
   EvalAffich = True;
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
   EvalAffich = False;

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
   EvalAffich = True;
}


/* ---------------------------------------------------------------------- */
/* |    RazJustif recalcule la largueur d'une boite apres suppression de| */
/* |            la justification. Met a jour les marques de selection   | */
/* |            que la boite soit justifiee ou non.                     | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void         RazJustif (PtrBox ibox, int lgblanc)

#else  /* __STDC__ */
static void         RazJustif (ibox, lgblanc)
PtrBox            ibox;
int                 lgblanc;

#endif /* __STDC__ */

{
   int                 x;
   int                 l;

   /* Box justifiee -> On met a jour sa largeur et les marques */
   if (ibox->BxSpaceWidth != 0)
     {
	x = ibox->BxSpaceWidth - lgblanc;	/* blanc justifie - blanc de la police */
	l = ibox->BxWidth - x * ibox->BxNSpaces - ibox->BxNPixels;
	ibox->BxWidth = l;
	ibox->BxSpaceWidth = 0;
	ibox->BxNPixels = 0;
     }
}


/* ---------------------------------------------------------------------- */
/* |    DispCoup libere les boites de coupure qui ont pu etre creees a` | */
/* |            partir de la boite mere ibox.                           | */
/* |            L'indicateur chgDF ou chgFS est bascule si la boite     | */
/* |            referencee par la marque Debut ou Fin de Selection est  | */
/* |            liberee.                                                | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void         DispCoup (PtrBox ibox, int frame, boolean * chgDS, boolean * chgFS)

#else  /* __STDC__ */
static void         DispCoup (ibox, frame, chgDS, chgFS)
PtrBox            ibox;
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
   PtrAbstractBox             pPa1;
   ViewFrame            *pFe1;
   ViewSelection            *pMa1;

   if (ibox != NULL)
     {
	pPa1 = ibox->BxAbstractBox;
	if (pPa1->AbLeafType == LtText)
	  {
	     x = CarWidth (BLANC, ibox->BxFont);
	     /* On met a jour les marques de selection */
	     pFe1 = &FntrTable[frame - 1];
	     if (pFe1->FrSelectionBegin.VsBox == ibox)
	       {
		  pFe1->FrSelectionBegin.VsBox = pPa1->AbBox;	/* Box entiere */
		  *chgDS = True;
	       }
	     if (pFe1->FrSelectionEnd.VsBox == ibox)
	       {
		  pFe1->FrSelectionEnd.VsBox = pPa1->AbBox;	/* Box entiere */
		  *chgFS = True;
	       }

	     /* On va liberer des boites coupure */
	     if (ibox->BxType == BoComplete)
		RazJustif (ibox, x);
	     else
	       {
		  ibox2 = ibox->BxNexChild;		/* 1e boite de coupure liberee */
		  ibox->BxNexChild = NULL;
		  if (ibox->BxType == BoSplit)
		    {
		       /* On met a jour la boite coupee */
		       ibox->BxType = BoComplete;
		       ibox->BxPrevious = ibox2->BxPrevious;
		       /* On transmet la position courante de la boite */
		       ibox->BxXOrg = ibox2->BxXOrg;
		       ibox->BxYOrg = ibox2->BxYOrg;
		       if (ibox->BxPrevious != NULL)
			  ibox->BxPrevious->BxNext = ibox;
		       else
			  FntrTable[frame - 1].FrAbstractBox->AbBox->BxNext = ibox;

		       larg = 0;
		       nombre = 0;
		       reste = 0;
		    }
		  else
		    {
		       /* On met a jour la derniere boite de coupure conservee */
		       RazJustif (ibox, x);
		       larg = ibox->BxWidth;
		       /* Il faut retirer l'espace reserve au tiret d'hyphenation */
		       if (ibox->BxType == BoDotted)
			 {
			    larg -= CarWidth (173, ibox->BxFont);
			    ibox->BxType = BoPiece;
			 }
		       nombre = ibox->BxNSpaces;
		       reste = ibox->BxIndChar + ibox->BxNChars;
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
			    pFe1 = &FntrTable[frame - 1];
			    pMa1 = &pFe1->FrSelectionBegin;
			    if (pMa1->VsBox == ibox1)
			      {
				 pMa1->VsBox = pPa1->AbBox;	/* Box entiere */
				 *chgDS = True;
			      }
			    pMa1 = &pFe1->FrSelectionEnd;
			    if (pMa1->VsBox == ibox1)
			      {
				 pMa1->VsBox = pPa1->AbBox;	/* Box entiere */
				 *chgFS = True;
			      }
			 }
		       while (!(ibox2 == NULL));

		       /* Met a jour la boite de coupure */
		       if (ibox->BxType == BoPiece)
			 {
			    ibox->BxNChars = ibox->BxAbstractBox->AbBox->BxNChars - ibox->BxIndChar;
			    /* Il faut comptabiliser les blancs ignores de fin de boite */
			    reste = reste - ibox->BxIndChar - ibox->BxNChars;
			    if (reste > 0)
			      {
				 larg += reste * x;
				 nombre += reste;
			      }
			    ibox->BxWidth = larg;
			    ibox->BxNSpaces = nombre;
			 }
		       /* Termine la mise a jour des chainages */
		       ibox->BxNext = bsuiv;
		       if (bsuiv != NULL)
			  bsuiv->BxPrevious = ibox;
		       else
			  FntrTable[frame - 1].FrAbstractBox->AbBox->BxPrevious = ibox;

		    }
	       }
	  }
	/* Pour les autres natures */
	else
	  {
	     pFe1 = &FntrTable[frame - 1];
	     if (pFe1->FrSelectionBegin.VsBox == ibox)
		*chgDS = True;
	     if (pFe1->FrSelectionEnd.VsBox == ibox)
		*chgFS = True;
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
void                DispBloc (PtrBox ibox, int frame, PtrLine premligne, boolean * chgDS, boolean * chgFS)

#else  /* __STDC__ */
void                DispBloc (ibox, frame, premligne, chgDS, chgFS)
PtrBox            ibox;
int                 frame;
PtrLine            premligne;
boolean            *chgDS;
boolean            *chgFS;

#endif /* __STDC__ */

{
   PtrBox            box1;
   PtrLine            lignesuiv;
   PtrLine            adligne;
   PtrAbstractBox             adpave;

   *chgDS = False;
   *chgFS = False;
   adligne = premligne;
   if (adligne != NULL)
     {
	if (ibox->BxType == BoBlock)
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
		  ibox->BxLastLine = adligne->LiPrevious;
	       }
	     else
	       {
		  ibox->BxFirstLine = NULL;
		  ibox->BxLastLine = NULL;
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
		  adpave = ibox->BxAbstractBox->AbFirstEnclosed;
		  while (box1 == NULL && adpave != NULL)
		     /* Est-ce que le pave est mort ? */
		     if (adpave->AbDead)
		       {
			  box1 = Suivante (adpave);
			  if (box1 != NULL)
			     adpave = box1->BxAbstractBox;
			  else
			     adpave = NULL;
		       }
		  /* Est-ce que le pave est eclate ? */
		     else if (adpave->AbBox->BxType == BoGhost)
			adpave = adpave->AbFirstEnclosed;	/* On descend la hierarchie */
		  /* Sinon c'est la boite du pave */
		     else
			box1 = adpave->AbBox;
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
/* |    ReevalBloc reevalue le bloc de ligne adpave a` partir de la     | */
/* |            ligne lig dans la fenetre frame suite au changement de  | */
/* |            largeur de la boite org.                                | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                ReevalBloc (PtrAbstractBox adpave, PtrLine lig, PtrBox org, int frame)

#else  /* __STDC__ */
void                ReevalBloc (adpave, lig, org, frame)
PtrAbstractBox             adpave;
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
   PtrBox            ibox;
   ViewFrame            *pFe1;
   ViewSelection            *pMa1;
   ViewSelection            *pMa0;
   PtrBox            pBo2;

   /* Si la boite est eclatee, on remonte jusqu'a la boite bloc de lignes */
   while (adpave->AbBox->BxType == BoGhost)
      adpave = adpave->AbEnclosing;

   ibox = adpave->AbBox;
   if (ibox != NULL)
     {
	adligne = lig;

	if (adligne == NULL)
	   adligne = ibox->BxFirstLine;

	/* Zone affichee avant modification */
	if (adligne == NULL)
	  {
	     l = 0;
	     h = ibox->BxYOrg;
	  }
	else
	  {
	     l = adligne->LiXOrg + adligne->LiXMax;
	     h = ibox->BxYOrg + adligne->LiYOrg;
	  }

	/* Si l'origne de la reevaluation du bloc de ligne vient d'une boite */
	/* de coupure, il faut deplacer cette origne sur la boite coupee     */
	/* parce que DispBloc va liberer toutes les boites de coupure.       */
	if (org != NULL)
	   if (org->BxType == BoPiece || org->BxType == BoDotted)
	      org = org->BxAbstractBox->AbBox;

	SvAff = EvalAffich;
	EvalAffich = False;
	DispBloc (ibox, frame, adligne, &chgDS, &chgFS);
	if (ibox->BxFirstLine == NULL)
	  {
	     /* On fait reevaluer la mise en lignes et on recupere */
	     /* la hauteur et la largeur du contenu de la boite */
	     EvalComp (adpave, frame, &large, &haut);
	  }
	else
	  {
	     BlocDeLigne (ibox, frame, &haut);
	     large = 0;
	  }
	EvalAffich = SvAff;

	/* Zone affichee apres modification */
	/* Il ne faut pas tenir compte de la boite si elle */
	/* n'est pas encore placee dans l'image concrete   */
/**MIN*/ if (EvalAffich && !ibox->BxXToCompute && !ibox->BxYToCompute)
	  {
	     if (ibox->BxWidth > l)
		l = ibox->BxWidth;
	     l += ibox->BxXOrg;
	     if (haut > ibox->BxHeight)
		DefClip (frame, ibox->BxXOrg, h, l, ibox->BxYOrg + haut);
	     else
		DefClip (frame, ibox->BxXOrg, h, l, ibox->BxYOrg + ibox->BxHeight);
	  }

	/* Faut-il reevaluer les marques de selection ? */
	pFe1 = &FntrTable[frame - 1];
	pMa0 = &pFe1->FrSelectionBegin;
	if (chgDS && pMa0->VsBox != NULL)
	  {
	     /* Si la selection a encore un sens */
	     if (pMa0->VsBox->BxAbstractBox != NULL)
		ReevalMrq (pMa0);
	  }

	pMa1 = &pFe1->FrSelectionEnd;
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

	if (large != 0 && large != ibox->BxWidth)
	   ChangeLgContenu (ibox, org, large, 0, frame);
	/* Faut-il conserver la hauteur ? */
	if (haut != 0)
	  {
	     /* Il faut propager la modification de hauteur */
	     SvPrpg = Propage;
	     /*if (SvPrpg == ToChildren) Propage = ToAll; */
	     ChangeHtContenu (ibox, org, haut, frame);
	     Propage = SvPrpg;
	  }
     }
}


/* ---------------------------------------------------------------------- */
/* |    MajBloc met a` jour le bloc de ligne (pave) apres modification  | */
/* |            de la largeur de la boite incluse ibox de dx.           | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                MajBloc (PtrAbstractBox pave, PtrLine adligne, PtrBox ibox, int dx, int dbl, int frame)

#else  /* __STDC__ */
void                MajBloc (pave, adligne, ibox, dx, dbl, frame)
PtrAbstractBox             pave;
PtrLine            adligne;
PtrBox            ibox;
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
		  pBo1->BxContentWidth = False;
		  ReevalBloc (pave, NULL, NULL, frame);
	       }
	     else
	       {
		  DecalLigne (adligne, pave, ibox, dx, frame);
		  adligne->LiXMax = adligne->LiRealLength;
		  ModLarg (pBo1, pBo1, NULL, dx, 0, frame);
	       }
	  }
	/* Si le bloc de lignes deborde de sa dimension minimale */
	else if (!pPavD1->DimIsPosition && pPavD1->DimMinimum
		 && adligne->LiRealLength + dx > pBo1->BxWidth)
	  {
	     /* Il faut prendre la largeur du contenu */
	     pBo1->BxContentWidth = True;
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
		  reste = CarWidth (BLANC, ibox->BxFont);
		  nlcour = adligne->LiRealLength + dx - dbl * (adligne->LiSpaceWidth - reste);
		  reste = adligne->LiXMax - adligne->LiMinLength;
	       }
	     else
		reste = adligne->LiXMax - adligne->LiRealLength;

	     /* Est-ce que la boite debordait de la ligne ? */
	     if (ibox->BxWidth - dx > adligne->LiXMax)
	       {
		  reste = adligne->LiXMax - ibox->BxWidth;	/* Pixels liberes dans la ligne */
		  if (reste > 0)
		     ReevalBloc (pave, adligne, NULL, frame);
	       }
	     /* Peut-on compresser ou completer la ligne ? */
	     else if ((dx > 0 && dx <= reste)
		      || (dx < 0 && (reste < maxreste
		  || (adligne->LiPrevious == NULL && adligne->LiNext == NULL))))
		if (adligne->LiSpaceWidth == 0)
		   DecalLigne (adligne, pave, ibox, dx, frame);
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
			  DecalLigne (adligne, pave, ibox, dx, frame);
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
/* |            du pave adpave et propage les modifications necessaires.| */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                EnglLigne (PtrBox ibox, int frame, PtrAbstractBox adpave)

#else  /* __STDC__ */
void                EnglLigne (ibox, frame, adpave)
PtrBox            ibox;
int                 frame;
PtrAbstractBox             adpave;

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

   pBo1 = adpave->AbBox;
   if (Propage != ToSiblings || pBo1->BxVertFlex)
     {
	pLi1 = DesLigne (ibox);
	/*pPavD1 = &adpave->AbHeight; */

	if (pLi1 != NULL)
	  {
	     sur = 0;
	     sous = 0;

	     /* ===> Est-ce une boite qui echappe a l'englobement (Page) ? */
	     if (!ibox->BxAbstractBox->AbHorizEnclosing)
	       {
		  /* La position de la ligne est inchangee */
		  sur = ibox->BxHorizRef;
		  sous = ibox->BxHeight - pLi1->LiHeight;	/* Decalage des lignes suivantes */
		  pLi1->LiHorizRef = sur;
		  pLi1->LiHeight = ibox->BxHeight;
		  /* On deplace la boite */
		  i = pBo1->BxYOrg + pLi1->LiYOrg - ibox->BxYOrg;
		  DepOrgY (ibox, NULL, i, frame);

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
			  + PixelValue (adpave->AbLineSpacing, adpave->AbLineSpacingUnit, adpave) - sur;
		    }
		  else
		    {
		       h = 0;
		       pos = 0;
		    }

		  /* On place la boite ibox dans la ligne */

		  i = pLi1->LiYOrg + pLi1->LiHorizRef - ibox->BxHorizRef;
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

		  i = i + pBo1->BxYOrg - ibox->BxYOrg;
		  DepOrgY (ibox, NULL, i, frame);

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
			    if (adpave->AbHorizRef.PosAbRef == adpave->AbFirstEnclosed)
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

				 if (box1 != ibox)
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
			  + PixelValue (adpave->AbLineSpacing, adpave->AbLineSpacingUnit, adpave)
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
