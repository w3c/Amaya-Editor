/*
   lig.c :  gestion de la mise en lignes des boites
   I. Vatton
 */

#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"

#define EXPORT extern
#include "img.var"

/* Nombre maximum de coupures de mots consecutives */
static int          MaxSiblingBreaks = 2;

#include "boxmoves_f.h"
#include "boxlocate_f.h"
#include "buildboxes_f.h"
#include "memory_f.h"
#include "boxselection_f.h"
#include "buildlines_f.h"
#include "font_f.h"
#include "frame_f.h"
#include "hyphen_f.h"


/* ---------------------------------------------------------------------- */
/* | GetNextBox rend l'adresse de la boite associee au pave vivant qui  | */
/* |            suit pAb.                                               | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
PtrBox            Suivante (PtrAbstractBox pAb)
#else  /* __STDC__ */
PtrBox            Suivante (pAb)
PtrAbstractBox     pAb;
#endif /* __STDC__ */
{
   PtrAbstractBox  pNextAb;
   boolean         loop;
   PtrBox          result;

   /* verifie que le pave existe toujours */
   if (pAb == NULL)
      result = NULL;
   else
     {
	pNextAb = pAb->AbNext;
	loop = TRUE;
	while (loop)
	   if (pNextAb == NULL)
	      /* Est-ce la derniere boite fille d'une boite eclatee */
	      if (pAb->AbEnclosing->AbBox->BxType == BoGhost)
		{
		   /* remonte la hierarchie */
		   pAb = pAb->AbEnclosing;
		   pNextAb = pAb->AbNext;
		}
	      else
		 loop = FALSE;
	   else if (pNextAb->AbDead)
	      pNextAb = pNextAb->AbNext;
	   else if (pNextAb->AbBox == NULL)
	      pNextAb = pNextAb->AbNext;
	/* Est-ce un pave compose eclate ? */
	   else if (pNextAb->AbBox->BxType == BoGhost)
	     {
		/* descend la hierarchie */
		while (loop)
		   if (pNextAb->AbBox == NULL)
		      pNextAb = pNextAb->AbNext;
		   else if (pNextAb->AbBox->BxType == BoGhost)
		      pNextAb = pNextAb->AbFirstEnclosed;
		   else
		      loop = FALSE;
		loop = TRUE;
		pAb = pNextAb;
	     }
	   else
	      loop = FALSE;
	if (pNextAb == NULL)
	   result = NULL;
	else
	   result = pNextAb->AbBox;
     }
   return result;
}


/* ---------------------------------------------------------------------- */
/* |  GetPreviousBox rend l'adresse de la boite associee au pave vivant | */
/* |            qui precede pAb.                                        | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static PtrBox     Precedente (PtrAbstractBox pAb)
#else  /* __STDC__ */
static PtrBox     Precedente (pAb)
PtrAbstractBox    pAb;
#endif /* __STDC__ */
{
   PtrAbstractBox pNextAb;
   boolean        loop;
   PtrBox         result;

   pNextAb = pAb->AbPrevious;
   loop = TRUE;
   while (loop)
      if (pNextAb == NULL)
	 /* Est-ce la derniere boite fille d'une boite eclatee */
	 if (pAb->AbEnclosing->AbBox->BxType == BoGhost)
	   {
	      /* remonte la hierarchie */
	      pAb = pAb->AbEnclosing;
	      pNextAb = pAb->AbPrevious;
	   }
	 else
	    loop = FALSE;
      else if (pNextAb->AbDead)
	 pNextAb = pNextAb->AbPrevious;
      else if (pNextAb->AbBox == NULL)
	 pNextAb = pNextAb->AbPrevious;
   /* Est-ce un pave compose eclate ? */
      else if (pNextAb->AbBox->BxType == BoGhost)
	{
	   /* descend la hierarchie */
	   while (pNextAb->AbBox->BxType == BoGhost)
	     {
		pNextAb = pNextAb->AbFirstEnclosed;
		/* recherche le dernier pave fils */
		while (pNextAb->AbNext != NULL)
		   pNextAb = pNextAb->AbNext;
	     }
	   pAb = pNextAb;
	}
      else
	 loop = FALSE;

   if (pNextAb == NULL)
      result = NULL;
   else
      result = pNextAb->AbBox;
   return result;
}


/* ---------------------------------------------------------------------- */
/* |    Adjust calcule la largeur des blancs a` afficher dans la ligne  | */
/* |            pour l'ajuster. La procedure met a` jour les positions  | */
/* |            en x et la largeur des boites incluses.                 | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void    Adjust (PtrBox pParentBox, PtrLine pLine, int frame, boolean orgXComplete, boolean orgYComplete)
#else  /* __STDC__ */
static void    Adjust (pParentBox, pLine, frame, orgXComplete, orgYComplete)
PtrBox            pParentBox;
PtrLine           pLine;
int               frame;
boolean           orgXComplete;
boolean           orgYComplete;
#endif /* __STDC__ */
{
   PtrBox         pBox, pBoxInLine;
   int            length, baseline;
   int            nSpaces;

   nSpaces = 0;	   /* nombre de blancs */
   length = 0;	   /* taille des chaines de caracteres sans blanc */
   baseline = pLine->LiYOrg + pLine->LiHorizRef;

   /* Calcul de la longueur de la ligne en supprimant les blancs */
   if (pLine->LiFirstPiece != NULL)
      pBoxInLine = pLine->LiFirstPiece;
   else
      pBoxInLine = pLine->LiFirstBox;
   do				/* Boucle sur les boites de la ligne */
     {
	if (pBoxInLine->BxType == BoSplit)
	   pBox = pBoxInLine->BxNexChild;
	else
	   pBox = pBoxInLine;
	if (pBox->BxAbstractBox->AbLeafType == LtText)
	  {
	     pBox->BxWidth -= pBox->BxNSpaces * CarWidth (BLANC, pBox->BxFont);
	     nSpaces += pBox->BxNSpaces;
	  }

	length += pBox->BxWidth;
	/* passe a la boite suivante */
	pBoxInLine = Suivante (pBox->BxAbstractBox);
     }

   /* Calcul de la taille des blancs en pixels */
   while (!(pBox == pLine->LiLastBox || pBox == pLine->LiLastPiece));
   if (pLine->LiXMax > length)
     {
	pLine->LiNPixels = pLine->LiXMax - length;
	if (nSpaces == 0)
	   pLine->LiSpaceWidth = 0;
	else
	   pLine->LiSpaceWidth = pLine->LiNPixels / nSpaces;
	pLine->LiNSpaces = nSpaces;
	/* Largeur minimum de ligne */
	pLine->LiMinLength = length + nSpaces * MIN_BLANC;
	pLine->LiNPixels -= pLine->LiSpaceWidth * nSpaces;	/* Pixels restants a repartir */
     }
   else
     {
	pLine->LiNPixels = 0;
	pLine->LiSpaceWidth = 0;
	pLine->LiNSpaces = 0;
	pLine->LiMinLength = length;
     }

   /* Met a jour l'origine, la base et la largeur de chaque boite */
   length = pLine->LiXOrg;
   if (orgXComplete)
      length += pParentBox->BxXOrg;
   if (orgYComplete)
      baseline += pParentBox->BxYOrg;

   nSpaces = pLine->LiNPixels;
   if (pLine->LiFirstPiece != NULL)
      pBoxInLine = pLine->LiFirstPiece;
   else
      pBoxInLine = pLine->LiFirstBox;

   /* Boucle sur les boites de la ligne */
   do
     {
	if (pBoxInLine->BxType == BoSplit)
	   pBox = pBoxInLine->BxNexChild;
	else
	   pBox = pBoxInLine;
	DepOrgX (pBox, NULL, length - pBox->BxXOrg, frame);
	DepOrgY (pBox, NULL, baseline - pBox->BxHorizRef - pBox->BxYOrg, frame);
	/* Repartition des pixels */
	if (pBox->BxAbstractBox->AbLeafType == LtText)
	  {
	     if (nSpaces > pBox->BxNSpaces)
		pBox->BxNPixels = pBox->BxNSpaces;
	     else
		pBox->BxNPixels = nSpaces;

	     nSpaces -= pBox->BxNPixels;
	     pBox->BxSpaceWidth = pLine->LiSpaceWidth;
	     pBox->BxWidth = pBox->BxWidth + pBox->BxNSpaces * pLine->LiSpaceWidth + pBox->BxNPixels;
	  }

	length += pBox->BxWidth;
	/* passe a la boite suivante */
	pBoxInLine = Suivante (pBox->BxAbstractBox);
     }
   while (!(pBox == pLine->LiLastBox || pBox == pLine->LiLastPiece));
}


/* ---------------------------------------------------------------------- */
/* |    round fait un arrondi float -> int.                             | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
int           round (float e)
#else  /* __STDC__ */
int           round (e)
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


/* ---------------------------------------------------------------------- */
/* |    Aligner aligne les bases des boites contenues dans la ligne     | */
/* |            designee. Decale de delta l'origine de chaque boite     | */
/* |            dans la ligne et indique que les boites ne sont pas     | */
/* |            justifiees.                                             | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                Aligner (PtrBox pParentBox, PtrLine pLine, int delta, int frame, boolean orgXComplete, boolean orgYComplete)
#else  /* __STDC__ */
void                Aligner (pParentBox, pLine, delta, frame, orgXComplete, orgYComplete)
PtrBox            pParentBox;
PtrLine           pLine;
int               delta;
int               frame;
boolean           orgXComplete;
boolean           orgYComplete;
#endif /* __STDC__ */
{
   PtrBox         pBox, pBoxInLine;
   int            baseline, x;

   /* Base de la ligne */
   baseline = pLine->LiYOrg + pLine->LiHorizRef;
   x = delta;
   if (orgXComplete)
      x += pParentBox->BxXOrg;
   if (orgYComplete)
      baseline += pParentBox->BxYOrg;

   /* Premiere boite */
   if (pLine->LiFirstPiece != NULL)
      pBoxInLine = pLine->LiFirstPiece;
   else
      pBoxInLine = pLine->LiFirstBox;

   /* Boucle sur les boites de la ligne */
   do
     {
	if (pBoxInLine->BxType == BoSplit)
	   pBox = pBoxInLine->BxNexChild;
	else
	   pBox = pBoxInLine;
	DepOrgX (pBox, NULL, x - pBox->BxXOrg, frame);
	DepOrgY (pBox, NULL, baseline - pBox->BxHorizRef - pBox->BxYOrg, frame);
	x += pBox->BxWidth;
	pBoxInLine->BxSpaceWidth = 0;
	/* passe a la boite suivante */
	pBoxInLine = Suivante (pBox->BxAbstractBox);
     }
   while (!(pBox == pLine->LiLastBox || pBox == pLine->LiLastPiece));
}


/* ---------------------------------------------------------------------- */
/* | SearchBreak calcule le nombre de caracte`res nChars a` placer dans  | */
/* |            la premie`re boi^te coupe'e pour la limiter a` une      | */
/* |            longueur de length en pixels.                           | */
/* |            Retourne le nombre de caracte`res a` sauter, soit 1 ou  | */
/* |            plus si la coupure tombe sur un blanc, -1 si le dernier | */
/* |            mot est coupe' et un tiret doit e^tre engendre', -2 si  | */
/* |            le dernier mot est coupe' sans besoin de tiret, 0 si la | */
/* |            coupure ne correspond pas a` un point de coupure.       | */
/* |            Rend la position a` laquelle le point de coupure peut   | */
/* |            e^tre inse're' :                                        | */
/* |            - le nombre de caracteres : nChars (blancs compris sauf | */
/* |            les derniers).                                          | */
/* |            - la longueur de cette nouvelle sous-chaine :           | */
/* |              breakLength (avec les blancs).                        | */
/* |            - le nombre de blancs qu'elle contient : nSpaces        | */
/* |            (sauf les derniers).                                    | */
/* |            - l'adresse du buffer du 1er caractere apres coupure.   | */
/* |            - l'index dans ce buffer du 1er caractere apres coupure.| */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static int   SearchBreak (PtrLine pLine, PtrBox pBox, int length, ptrfont font, int *nChars, int *breakLength, int *nSpaces, int *newIndex, PtrTextBuffer *pNewBuff)
#else  /* __STDC__ */
static int   SearchBreak (pLine, pBox, length, font, nChars, breakLength, nSpaces, newIndex, pNewBuff)
PtrLine            pLine;
PtrBox             pBox;
int                length;
ptrfont            font;
int               *nChars;
int               *breakLength;
int               *nSpaces;
int               *newIndex;
PtrTextBuffer     *pNewBuff;
#endif /* __STDC__ */
{
   PtrLine            pPreviousLine;
   int                i, count;
   int                carWidth, newWidth;
   int                width;
   int                wordWidth, charIndex;
   int                dummySpaces, spaceWidth;
   int                spaceAdjust;
   int                spaceCount;
   unsigned char      character;
   PtrTextBuffer      pBuffer;
   PtrBox             pParentBox;
   boolean            still;
   Language           language;

   /* Initialisations */
   dummySpaces = 0;
   character = 0;
   *pNewBuff = NULL;
   *newIndex = 1;
   *nChars = pBox->BxNChars;
   *breakLength = pBox->BxWidth;
   *nSpaces = pBox->BxNSpaces;

   charIndex = pBox->BxFirstChar;  /* index dans le buffer */
   pBuffer = pBox->BxBuffer;
   /* newWidth est la largeur utilisee pour calculer la mise en ligne */
   /* largeur est la largeur reelle du texte                       */
   newWidth = 0;
   width = 0;
   carWidth = 0;
   wordWidth = 0;
   spaceCount = 0;
   still = TRUE;
   spaceWidth = CarWidth (BLANC, font);
   spaceAdjust = spaceWidth;
   language = pBox->BxAbstractBox->AbLanguage;

   if (length != pBox->BxWidth - 1)
     {
	/* L'appel de SearchBreak ne concerne pas les blancs en fin de paragraphe */
	pParentBox = pBox->BxAbstractBox->AbEnclosing->AbBox;
	/* remonte jusqu'a la boite bloc de lignes */
	while (pParentBox->BxType == BoGhost)
	   pParentBox = pParentBox->BxAbstractBox->AbEnclosing->AbBox;
	/* Si la ligne est justifiee on prend des blancs compresses */
	if (pParentBox->BxAbstractBox->AbJustify)
	   spaceAdjust = round ((float) (spaceAdjust * 7) / (float) (10));
     }

   i = 0;
   count = pBox->BxNChars;

   /* Recherche un point de coupure */
   while (still)
     {
	/* Largeur du caractere suivant */
	character = (unsigned char) (pBuffer->BuContent[charIndex - 1]);
	if (character == NUL)
	   carWidth = 0;
	else if (character == BLANC)
	   carWidth = spaceAdjust;
	else
	   carWidth = CarWidth (character, font);

	/* Si le caractere ne rentre pas dans la ligne */
	/* ou si on arrive a la fin de la boite        */
	if ((newWidth + carWidth > length || i >= count) && (i != 0))
	  {
	     still = FALSE;
/*-- Si on veut simplement supprimer les blancs de fin de ligne */
	     if (length >= pBox->BxWidth)
	       {
		  /* Pointe le caractere apres la fin de boite */
		  *newIndex = 1;
		  *pNewBuff = NULL;
		  *breakLength = pBox->BxWidth;
		  *nChars = pBox->BxNChars;
		  *nSpaces = pBox->BxNSpaces;

		  /* Pointe le dernier caractere de la boite */
		  if (charIndex == 1 && pBuffer->BuPrevious != NULL)
		    {
		       /* Le caractere precedent se trouve dans un autre buffer */
		       pBuffer = pBuffer->BuPrevious;
		       charIndex = pBuffer->BuLength;
		    }
		  else
		     /* Le caractere precedent se trouve dans le meme buffer */
		     charIndex--;
	       }
	     /* Si le caractere courant est un blanc */
	     else if (character == BLANC)
	       {
		  /* coupe sur le dernier blanc rencontre' */
		  dummySpaces = 1;
		  if (spaceCount == 0)
		     *nChars = wordWidth;	/* dernier blanc non compris */
		  else
		     (*nChars) += wordWidth;		/* dernier blanc non compris */
		  *breakLength = width;
		  *nSpaces = spaceCount;

		  /* Pointe le premier caractere apres la coupure */
		  if (charIndex >= pBuffer->BuLength && pBuffer->BuNext != NULL)
		    {
		       /* Le caractere suivant se trouve dans un autre buffer */
		       *pNewBuff = pBuffer->BuNext;
		       *newIndex = 1;
		    }
		  else
		    {
		       /* Le caractere suivant se trouve dans le meme buffer */
		       *pNewBuff = pBuffer;
		       *newIndex = charIndex + 1;
		    }		/*else */

		  /* Pointe le dernier caractere avant la coupure */
		  if (charIndex == 1 && pBuffer->BuPrevious != NULL)
		    {
		       /* Le caractere precedent se trouve dans un autre buffer */
		       pBuffer = pBuffer->BuPrevious;
		       charIndex = pBuffer->BuLength;
		    }
		  else
		     /* Le caractere precedent se trouve dans le meme buffer */
		     charIndex--;
	       }
	     /* Si on n'a pas trouve de caractere blanc */
	     else if (spaceCount == 0)
	       {
		  (*nChars) = wordWidth;
		  *breakLength = width;
		  *nSpaces = 0;

		  /* Pointe le premier caractere apres la coupure */
		  *pNewBuff = pBuffer;
		  *newIndex = charIndex;

		  /* Pointe le dernier caractere avant la coupure */
		  if (charIndex == 1 && pBuffer->BuPrevious != NULL)
		    {
		       /* Le caractere precedent se trouve dans un autre buffer */
		       pBuffer = pBuffer->BuPrevious;
		       charIndex = pBuffer->BuLength;
		    }
		  else
		     /* Le caractere precedent se trouve dans le meme buffer */
		     charIndex--;
	       }
	     /* Coupure sur un blanc precedent */
	     else
	       {
		  dummySpaces = 1;
		  *nSpaces = spaceCount - 1;

		  pBuffer = *pNewBuff;
		  charIndex = *newIndex;
		  /* Pointe le premier caractere apres la coupure */
		  if (charIndex >= pBuffer->BuLength && pBuffer->BuNext != NULL)
		    {
		       /* Le caractere suivant se trouve dans un autre buffer */
		       *pNewBuff = pBuffer->BuNext;
		       *newIndex = 1;
		    }
		  else
		     /* Le caractere suivant se trouve dans le meme buffer */
		     (*newIndex)++;

		  /* Pointe le dernier caractere avant la coupure */
		  if (charIndex == 1 && pBuffer->BuPrevious != NULL)
		    {
		       /* Le caractere precedent se trouve dans un autre buffer */
		       pBuffer = pBuffer->BuPrevious;
		       charIndex = pBuffer->BuLength;
		    }
		  else
		     /* Le caractere precedent se trouve dans le meme buffer */
		     charIndex--;
	       }
	  }
	/* Si c'est une fin de buffer */
	else if (character == NUL)
	   /* Si c'est la fin de la boite */
	   if (pBuffer->BuNext == NULL)
	     {
		still = FALSE;
		/* Pointe le premier caractere apres la coupure */
		*pNewBuff = pBuffer;
		*newIndex = charIndex;
		*breakLength = pBox->BxWidth;
		*nChars = pBox->BxNChars;
		*nSpaces = pBox->BxNSpaces;
		charIndex--;
	     }
	/* change de buffer */
	   else
	     {
		pBuffer = pBuffer->BuNext;
		charIndex = 1;
	     }
	/* Si c'est un caractere blanc */
	else if (character == BLANC)
	  {
	     /* Cela peut etre un point de coupure */
	     *pNewBuff = pBuffer;
	     *newIndex = charIndex;
	     /* Longueur de la chaine dernier blanc non compris */
	     if (spaceCount == 0)
		*nChars = wordWidth;
	     else
		(*nChars) += wordWidth;
	     spaceCount++;
	     *breakLength = width;
	     wordWidth = 1;		/* prepare un nouveau mot */
	     i++;		/* nombre de caracteres traites */
	     charIndex++;		/* indice du prochain caractere */
	     newWidth += carWidth;
	     width += spaceWidth;
	  }
	/*  Si c'est un autre caractere */
	else
	  {
	     wordWidth++;
	     i++;		/* nombre de caracteres traites */
	     charIndex++;		/* indice du prochain caractere */
	     newWidth += carWidth;
	     width += carWidth;
	  }
     }

   /* supprime eventuellement les blancs en fin de ligne avant la coupure */
   if (pBuffer != NULL && dummySpaces != 0)
      /* Il a peut-etre des blancs a supprimer */
      still = TRUE;
   else if (length >= pBox->BxWidth)
      /* Il a peut-etre des blancs a supprimer */
      still = TRUE;
   else
      still = FALSE;

   while (still && *nChars > 0)
     {
	character = (unsigned char) (pBuffer->BuContent[charIndex - 1]);
	if (character == BLANC)
	  {
	     /* Est-ce que la coupure a lieu en fin de boite ? */
	     if (*pNewBuff == NULL)
	       {
		  *pNewBuff = pBuffer;
		  *newIndex = charIndex + 1;
	       }

	     /* passe au caractere precedent */
	     if (charIndex == 1)
		if (pBuffer->BuPrevious != NULL)
		  {
		     pBuffer = pBuffer->BuPrevious;
		     charIndex = pBuffer->BuLength;
		  }
		else
		   still = FALSE;
	     else
		charIndex--;

	     *breakLength -= spaceWidth;
	     dummySpaces++;
	     (*nChars)--;
	     (*nSpaces)--;
	  }
	else
	   still = FALSE;
     }

   if (length < pBox->BxWidth && Hyphenable (pBox) && wordWidth > 1 && !Insert)
     {
	/* Il faut verifier les coupures en fin de lignes precedentes */
	still = TRUE;
	if (pLine != NULL)
	  {
	     pPreviousLine = pLine->LiPrevious;
	     count = 0;
	     while (count < MaxSiblingBreaks && pPreviousLine != NULL)
		if (pPreviousLine->LiLastPiece != NULL)
		   if (pPreviousLine->LiLastPiece->BxType == BoDotted)
		     {
			/* Il y a une coupure en fin de ligne */
			/* on regarde encore la precedente */
			pPreviousLine = pPreviousLine->LiPrevious;
			count++;
		     }
		   else
		      pPreviousLine = NULL;
		else
		   pPreviousLine = NULL;

	     if (count == MaxSiblingBreaks)
		/* refuse de couper le dernier mot */
		still = FALSE;
	  }			/*if pLine != NULL */

	if (still)
	  {
	     /* cherche reellement a couper le dernier mot de la boite */
	     if (dummySpaces == 0)
	       {
		  /* essaie de couper le mot unique de la boite */
		  width = length;
		  pBuffer = pBox->BxBuffer;
		  charIndex = pBox->BxFirstChar;
		  wordWidth = CutLastWord (font, language, &pBuffer, &charIndex, &width, &still);
		  /* Si la coupure a reussi */
		  if (wordWidth > 0)
		    {
		       /* met a jour le point de coupure */
		       *pNewBuff = pBuffer;
		       *newIndex = charIndex;
		       *breakLength = 0;
		       *nChars = 0;
		    }
	       }
	     else
	       {
		  /* essaie de couper le mot qui suit le dernier blanc */
		  width = length - *breakLength - dummySpaces * spaceAdjust;
		  wordWidth = CutLastWord (font, language, pNewBuff, newIndex, &width, &still);
	       }

	     if (wordWidth > 0)
	       {
		  /* Il faut mettre a jour le nombre de caracteres et */
		  /* la longueur de la premiere partie du texte coupe */
		  *nChars += dummySpaces + wordWidth;
		  *nSpaces += dummySpaces;
		  *breakLength += width + dummySpaces * spaceWidth;
		  if (still)
		     /* Il faut engendrer un tiret d'hyphenation en fin de boite */
		     dummySpaces = -1;
		  else
		     /* Pas de tiret a engendrer */
		     dummySpaces = -2;
	       }
	  }
     }

   /* Si on n'a pas trouve de coupure et que le mot ne peut pas */
   /* etre rejete a la ligne suivante (1er mot de la ligne */
   /* -> on force la coupure apres au moins 1 caractere */
   /* */
   if (dummySpaces == 0
       && (pBox == pLine->LiFirstBox || pBox == pLine->LiFirstPiece)
       && *pNewBuff != NULL)
     {
	/* Engendre un hyphen */
	dummySpaces = -1;
	*breakLength += CarWidth (173, font);

	/* RemoveElement un ou plusieurs caracteres pour loger l'hyphen */
	while (*breakLength > length && *nChars > 1)
	  {
	     /* passe au caractere precedent */
	     if (*newIndex == 1)
		if ((*pNewBuff)->BuPrevious != NULL)
		  {
		     *pNewBuff = (*pNewBuff)->BuPrevious;
		     *newIndex = (*pNewBuff)->BuLength;
		  }
		else
		   return dummySpaces;
	     else
		(*newIndex)--;

	     *breakLength -= CarWidth (character, font);
	     (*nChars)--;
	  }
     }

   /* supprime eventuellement les blancs en debut de ligne apres la coupure */
   if (*pNewBuff != NULL && dummySpaces > 0)
      /* Il a peut-etre des blancs a supprimer */
      still = TRUE;
   else
      still = FALSE;

   while (still)
     {
	character = (unsigned char) ((*pNewBuff)->BuContent[*newIndex - 1]);
	if (character == BLANC)
	  {

	     /* passe au caractere suivant */
	     if (*newIndex >= (*pNewBuff)->BuLength)
		if ((*pNewBuff)->BuNext != NULL)
		  {
		     *pNewBuff = (*pNewBuff)->BuNext;
		     *newIndex = 1;
		  }
		else
		  {
		     still = FALSE;
		     (*newIndex)++;
		  }
	     else
		(*newIndex)++;

	     dummySpaces++;
	  }
	else
	   still = FALSE;
     }
   return dummySpaces;
}


/* ---------------------------------------------------------------------- */
/* | Coupe la boite pBox sur un caractere de rupture de ligne.          | */
/* | width = la largeur du de'but de la boite jusqu'au point de coupure | */
/* | brWidth = largeur du caracte`re qui force la coupure            | */
/* | nCharssl = nombre de caracte`res avant la coupure                  | */
/* | nSpaces = nombre d'espaces avant le point de coupure               | */
/* | newIndex = indice du debut de texte apres le point de coupure      | */
/* | pNewBuff = buffer du debut de texte apres le point de coupure      | */
/* | pRootAb = pave racine de la vue (pour mise a jour des chainages)   | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void    ManageBreakLine (PtrBox pBox, int width, int brWidth, int nCharssl, int nSpaces, int newIndex, PtrTextBuffer pNewBuff, PtrAbstractBox pRootAb)
#else  /* __STDC__ */
static void    ManageBreakLine (pBox, width, brWidth, nCharssl, nSpaces, newIndex, pNewBuff, pRootAb)
PtrBox            pBox;
int               width;
int               brWidth;
int               nCharssl;
int               nSpaces;
int               newIndex;
PtrTextBuffer     pNewBuff;
PtrAbstractBox    pRootAb;
#endif /* __STDC__ */
{
   PtrBox            ibox1, ibox2;
   int               baseline;
   int               height;
   PtrBox            pPreviousBox, pNextBox;
   PtrAbstractBox    pAb;
   ptrfont           font;

   /* Initialisation */
   ibox1 = NULL;
   pAb = pBox->BxAbstractBox;
   height = pBox->BxHeight;
   baseline = pBox->BxHorizRef;
   pPreviousBox = pBox->BxPrevious;
   pNextBox = pBox->BxNext;
   font = pBox->BxFont;

   /* Creation de la boite de coupure pour le texte apres la coupure */
   if (pNewBuff != NULL)
     {
	/* Si la boite est entiere, il faut une boite de coupure */
	/* pour contenir le texte pPreviousBox le point de coupure      */
	if (pBox->BxType == BoComplete)
	  {
	     ibox1 = GetBox (pAb);
	     if (ibox1 == NULL)
	       /* plus de boite */
	       return;
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
	ibox2->BxHeight = height;
	ibox2->BxFont = font;
	ibox2->BxUnderline = pBox->BxUnderline;
	ibox2->BxThickness = pBox->BxThickness;
	ibox2->BxHorizRef = baseline;
	ibox2->BxType = BoPiece;
	ibox2->BxBuffer = pNewBuff;
	ibox2->BxNexChild = NULL;
	ibox2->BxIndChar = pBox->BxIndChar + nCharssl + 1;
	ibox2->BxFirstChar = newIndex;
	ibox2->BxNChars = pBox->BxNChars - nCharssl - 1;
	ibox2->BxNSpaces = pBox->BxNSpaces - nSpaces;
	ibox2->BxWidth = pBox->BxWidth - width - brWidth;

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
	     ibox1->BxHeight = height;
	     ibox1->BxHorizRef = baseline;
	     ibox1->BxType = BoPiece;
	     ibox1->BxBuffer = pBox->BxBuffer;
	     ibox1->BxFirstChar = pBox->BxFirstChar;
	     ibox1->BxWidth = width;
	     ibox1->BxNSpaces = nSpaces;
	     ibox1->BxNChars = nCharssl;

	     /* Modifie le chainage des boites terminales */
	     ibox1->BxNexChild = ibox2;
	     ibox1->BxPrevious = pPreviousBox;
	     if (pPreviousBox != NULL)
		pPreviousBox->BxNext = ibox1;
	     else
		pRootAb->AbBox->BxNext = ibox1;
	     ibox1->BxNext = ibox2;
	     ibox2->BxPrevious = ibox1;
	     ibox2->BxNext = pNextBox;
	     if (pNextBox != NULL)
		pNextBox->BxPrevious = ibox2;
	     else
		pRootAb->AbBox->BxPrevious = ibox2;

	     /* Mise a jour de la boite englobante */
	     pBox->BxType = BoSplit;
	     pBox->BxNexChild = ibox1;
	  }
	/* Si la boite est deja une boite de coupure, il faut la mettre a jour */
	else
	  {
	     pBox->BxWidth = width;
	     pBox->BxNSpaces = nSpaces;
	     pBox->BxNChars = nCharssl;

	     /* Modifie les boites terminales */
	     pBox->BxNexChild = ibox2;
	     pBox->BxNext = ibox2;
	     ibox2->BxPrevious = pBox;
	     ibox2->BxNext = pNextBox;
	     if (pNextBox != NULL)
		pNextBox->BxPrevious = ibox2;
	     else
		pRootAb->AbBox->BxPrevious = ibox2;
	  }

     }
}


/* ---------------------------------------------------------------------- */
/* |    BreakBox coupe la boi^te entiere d'indice pBox correspondant    | */
/* |            a` l'image abstraite pAb en deux boi^tes. Si force est  | */
/* |            Vrai coupe meme sur un caracte`re, sinon on ne coupe que| */
/* |            sur un blanc. La premie`re boi^te issue de la coupure   | */
/* |            contiendra nChars caracte`res. Sa longueur sera         | */
/* |            infe'rieure a` maxLength. Le blanc est perdu et le reste| */
/* |            mis dans la boi^te de coupure suivante. Les chai^nages  | */
/* |            sont mis a` jour.                                       | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         BreakBox (PtrLine pLine, PtrBox pBox, int maxLength, PtrAbstractBox pRootAb, boolean force)
#else  /* __STDC__ */
static void         BreakBox (pLine, pBox, maxLength, pRootAb, force)
PtrLine             pLine;
PtrBox              pBox;
int                 maxLength;
PtrAbstractBox      pRootAb;
boolean             force;
#endif /* __STDC__ */
{
   PtrBox           ibox1, ibox2;
   int              baseline, larg;
   int              newIndex, height;
   int              spaceWidth, lostPixels;
   int              nSpaces, nChars;
   PtrBox           pPreviousBox, pNextBox;
   PtrTextBuffer    pNewBuff;
   PtrAbstractBox   pAb;
   ptrfont          font;

   /* Initialisations */
   pAb = pBox->BxAbstractBox;
   height = pBox->BxHeight;
   baseline = pBox->BxHorizRef;
   pPreviousBox = pBox->BxPrevious;
   pNextBox = pBox->BxNext;
   font = pBox->BxFont;

   /* Si coupure sur un blanc le blanc de coupure est perdu */
   lostPixels = SearchBreak (pLine, pBox, maxLength, font, &nChars, &larg, &nSpaces, &newIndex, &pNewBuff);

   /* Creation de la 1ere boite qui contient le debut de la chaine */
   if (lostPixels <= 0)
      spaceWidth = 0;
   else
      spaceWidth = CarWidth (BLANC, font);

   /* Creation de deux boites de coupure :                           */
   /* - dans le cas general si SearchBreak a trouve' un point de coupure */
   /*   soit un blanc (lostPixels>0), soit un point d'hyphe'nation  avec  */
   /*   ge'ne'ration d'un tiret (lostPixels=-1) ou sans (lostPixels=-2)        */
   /* - si la coupure est force'e, sur n'importe quel caractere      */
   /* - pour dummySpaceser les derniers blancs de fin de texte a` condition */
   /*   qu'il lostPixels au moins un blanc pour justifier le texte restant */
   if (pNewBuff != NULL
       && (lostPixels != 0 || nSpaces != 0 || force)
       && (pBox->BxWidth != maxLength || lostPixels != pBox->BxNSpaces))
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
   if (lostPixels == -2)
      lostPixels = 0;

   if (ibox1 != NULL && ibox2 != NULL)
     {
	/* Initialise le contenu de cette nouvelle boite */
	ibox1->BxIndChar = 0;
	ibox1->BxContentWidth = TRUE;
	ibox1->BxContentHeight = TRUE;
	ibox1->BxFont = font;
	ibox1->BxUnderline = pBox->BxUnderline;
	ibox1->BxThickness = pBox->BxThickness;
	ibox1->BxHeight = height;
	ibox1->BxWidth = larg;
	ibox1->BxHorizRef = baseline;
	if (lostPixels == -1)
	  {
	     /* La boite doit etre completee par un tiret d'hyphenation */
	     ibox1->BxType = BoDotted;
	     larg -= CarWidth (173, font);
	     lostPixels = 0;		/* il n'y a pas de blanc dummySpacese */
	  }
	else
	   ibox1->BxType = BoPiece;
	ibox1->BxBuffer = pBox->BxBuffer;
	ibox1->BxNChars = nChars;
	ibox1->BxNSpaces = nSpaces;
	ibox1->BxFirstChar = pBox->BxFirstChar;

	/* Initialise le contenu de la deuxieme boite */
	ibox2->BxContentWidth = TRUE;
	ibox2->BxContentHeight = TRUE;
	ibox2->BxHeight = height;
	ibox2->BxFont = font;
	ibox2->BxUnderline = pBox->BxUnderline;
	ibox2->BxThickness = pBox->BxThickness;
	ibox2->BxHorizRef = baseline;
	ibox2->BxType = BoPiece;
	ibox2->BxBuffer = pNewBuff;
	ibox2->BxIndChar = nChars + lostPixels;
	ibox2->BxFirstChar = newIndex;
	ibox2->BxNChars = pBox->BxNChars - lostPixels - nChars;
	ibox2->BxNSpaces = pBox->BxNSpaces - lostPixels - nSpaces;
	ibox2->BxWidth = pBox->BxWidth - larg - lostPixels * spaceWidth;

	/* Si la boite est vide on annule les autres valeurs */
	if (ibox2->BxNChars == 0)
	  {
	     ibox2->BxWidth = 0;
	     ibox2->BxNSpaces = 0;
	  }

	/* Modifie le chainage des boites terminales */
	ibox1->BxPrevious = pPreviousBox;
	if (pPreviousBox != NULL)
	   pPreviousBox->BxNext = ibox1;
	else
	   pRootAb->AbBox->BxNext = ibox1;
	ibox1->BxNext = ibox2;
	ibox2->BxPrevious = ibox1;
	ibox1->BxNexChild = ibox2;
	ibox2->BxNexChild = NULL;
	ibox2->BxNext = pNextBox;
	if (pNextBox != NULL)
	   pNextBox->BxPrevious = ibox2;
	else
	   pRootAb->AbBox->BxPrevious = ibox2;

	/* Mise a jour de la boite englobante */
	pBox->BxType = BoSplit;
	pBox->BxNexChild = ibox1;
     }

}


/* ---------------------------------------------------------------------- */
/* | Teste s'il y a un caractere de rupture de ligne dans la boite pBox | */
/* | Si oui retourne la valeur Vrai, sinon la valeur FALSE.             | */
/* | Au retour :                                                        | */
/* | width = la largeur du de'but de la boite jusqu'au point de coupure | */
/* | brWidth = largeur du caracte`re qui force la coupure            | */
/* | nCharssl = nombre de caracte`res avant la coupure                  | */
/* | nSpaces = nombre d'espaces avant le point de coupure               | */
/* | newIndex = indice du debut de texte apres le point de coupure      | */
/* | pNewBuff = buffer du debut de texte apres le point de coupure      | */
/* | pRootAb = pave racine de la vue (pour mise a jour des chainages)   | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
boolean             SearchBreakLine (PtrBox pBox, int *width, int *brWidthec, int *nCharssl, int *nSpaces, int *newIndex, PtrTextBuffer * pNewBuff)

#else  /* __STDC__ */
boolean             SearchBreakLine (pBox, width, brWidthec, nCharssl, nSpaces, newIndex, pNewBuff)
PtrBox            pBox;
int                *width;
int                *brWidthec;
int                *nCharssl;
int                *nSpaces;
int                *newIndex;
PtrTextBuffer     *pNewBuff;

#endif /* __STDC__ */

{
   int                 i, j;
   unsigned char       car;
   int                 nChars;
   boolean             csexist;
   PtrTextBuffer      pBuffer;
   ptrfont             font;


   csexist = FALSE;
   *width = 0;
   *nCharssl = 0;
   *nSpaces = 0;
   j = 1;
   pBuffer = pBox->BxBuffer;
   nChars = pBox->BxNChars;
   font = pBox->BxFont;
   i = pBox->BxFirstChar;
   *pNewBuff = pBuffer;
   *newIndex = 1;

   /* Si la boite debute en fin de buffer -> on passe au buffer suivant */
   if (pBuffer != NULL)
      if (i > pBuffer->BuLength)
	{
	   pBuffer = pBuffer->BuNext;
	   i = 1;
	}			/*if */

   while (j <= nChars && !(csexist) && pBuffer != NULL)
     {
	car = (unsigned char) (pBuffer->BuContent[i - 1]);
	if (car == SAUT_DE_LIGNE)	/* Est-ce un caractere de coupure forcee ? */
	  {
	     csexist = TRUE;
	     *brWidthec = CarWidth (car, font);
	     /* Faut-il changer de buffer ? */
	     if (i >= pBuffer->BuLength)
	       {
		  /* La caractere de coupure force' est en fin de texte */
		  if (pBuffer->BuNext == NULL)
		    {
		       /* lostPixels en fin de buffer */
		       j = nChars;
		       i++;
		    }
		  /* Sinon on passe au debut du buffer suivant */
		  else
		    {
		       pBuffer = pBuffer->BuNext;
		       i = 1;
		    }
	       }
	     else
		i++;

	     *pNewBuff = pBuffer;
	     *newIndex = i;
	     *nCharssl = j - 1;
	     return csexist;
	  }
	else
	   /* Sinon on continue la recherche */
	  {
	     if (car == BLANC)
	       {
		  (*nSpaces)++;
		  *width += CarWidth (BLANC, font);
	       }
	     else
		*width += CarWidth (car, font);
	  }
	if (i >= pBuffer->BuLength)	/* Faut-il changer de buffer ? */
	  {
	     pBuffer = pBuffer->BuNext;
	     if (pBuffer == NULL)
		j = nChars;
	     i = 1;
	  }
	else
	   i++;
	j++;
     }
   return csexist;
}


/* ---------------------------------------------------------------------- */
/* |    ReBreakBox recoupe la boite pBox deja coupee pour l'image    | */
/* |            abstraite pAb. La coupure a lieu meme s'il n'y a pas de  | */
/* |            blanc (coupure forcee). La boite pBox contiendra nChars  | */
/* |            caracteres.                                             | */
/* ---------------------------------------------------------------------- */


#ifdef __STDC__
static void        ReBreakBox (PtrLine pLine, PtrBox pBox, int maxLength, PtrAbstractBox pRootAb)

#else  /* __STDC__ */
static void        ReBreakBox (pLine, pBox, maxLength, pRootAb)
PtrLine            pLine;
PtrBox             pBox;
int                maxLength;
PtrAbstractBox     pRootAb;

#endif /* __STDC__ */

{
   PtrBox           box2, pNextBox;
   int              oldlg, spaceWidth;
   int              nChars;
   int              larg, lostPixels, pl;
   int              newIndex, nSpaces;
   int              height, baseline;
   PtrTextBuffer    pNewBuff;
   PtrAbstractBox   pAb;
   ptrfont          font;
   int              oldwidth, oldnbbl;

   pAb = pBox->BxAbstractBox;
   height = pBox->BxHeight;
   baseline = pBox->BxHorizRef;
   pNextBox = pBox->BxNext;
   pl = pBox->BxIndChar;
   oldlg = pBox->BxNChars;
   font = pBox->BxFont;
   oldwidth = pBox->BxWidth;
   oldnbbl = pBox->BxNSpaces;

   /* Si coupure sur un blanc le blanc de coupure est perdu */
   lostPixels = SearchBreak (pLine, pBox, maxLength, font, &nChars, &larg, &nSpaces, &newIndex, &pNewBuff);
   if (lostPixels <= 0)
      spaceWidth = 0;
   else
      spaceWidth = CarWidth (BLANC, font);

   /* Creation de la boite suivante qui contient la fin de la chaine */
   /* - dans le cas general si SearchBreak a trouve' un point de coupure */
   /*   soit un blanc (lostPixels>0), soit un point d'hyphe'nation  avec  */
   /*   ge'ne'ration d'un tiret (lostPixels=-1) ou sans (lostPixels=-2)        */
   /* - pour dummySpaceser les derniers blancs de fin de texte a` condition */
   /*   qu'il lostPixels au moins un blanc pour justifier le texte restant */
   if (pNewBuff != NULL
/**528*/  && (lostPixels != 0 || nSpaces != 0 || oldnbbl == 0)
       && (pBox->BxWidth != maxLength || lostPixels != pBox->BxNSpaces))
      box2 = GetBox (pAb);
   else
      box2 = NULL;


   /* Coupure du mot sans generation du tiret */
   if (lostPixels == -2)
      lostPixels = 0;

   /* Initialise le contenu de cette nouvelle boite */
   if (box2 != NULL)
     {

	pBox->BxNexChild = box2;
	pBox->BxNext = box2;

	/* Mise a jour de la  boite recoupee */
	pBox->BxNChars = nChars;
	pBox->BxWidth = larg;
	pBox->BxNSpaces = nSpaces;
	if (lostPixels == -1)
	  {
	     /* La boite doit etre completee par un tiret d'hyphenation */
	     pBox->BxType = BoDotted;
	     larg -= CarWidth (173, font);
	     lostPixels = 0;		/* il n'y a pas de blanc dummySpacese */
	  }
	else
	   pBox->BxType = BoPiece;

	box2->BxContentWidth = TRUE;
	box2->BxContentHeight = TRUE;
	box2->BxHeight = height;
	box2->BxFont = font;
	box2->BxUnderline = pBox->BxUnderline;
	box2->BxThickness = pBox->BxThickness;
	box2->BxHorizRef = baseline;
	box2->BxType = BoPiece;
	box2->BxBuffer = pNewBuff;
	box2->BxNexChild = NULL;

	/* Modifie le chainage des boites terminales */
	box2->BxPrevious = pBox;
	box2->BxNext = pNextBox;
	if (pNextBox != NULL)
	   pNextBox->BxPrevious = box2;
	else
	   pRootAb->AbBox->BxPrevious = box2;

	/* Si lostPixels > 0 -> Il faut retirer les caracteres blanc de la boite */
	box2->BxIndChar = pl + lostPixels + nChars;
	box2->BxFirstChar = newIndex;
	box2->BxNChars = oldlg - lostPixels - nChars;
	box2->BxNSpaces = oldnbbl - lostPixels - nSpaces;
	box2->BxWidth = oldwidth - larg - lostPixels * spaceWidth;

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
void                AjoutBoite (PtrBox pBox, int *sousbase, int *surbase, PtrLine pLine)

#else  /* __STDC__ */
void                AjoutBoite (pBox, sousbase, surbase, pLine)
PtrBox            pBox;
int                *sousbase;
int                *surbase;
PtrLine            pLine;

#endif /* __STDC__ */

{
   pLine->LiRealLength += pBox->BxWidth;
   /* Calcule la hauteur courante de la ligne */
   if (*surbase < pBox->BxHorizRef)
      *surbase = pBox->BxHorizRef;
   if (*sousbase < pBox->BxHeight - pBox->BxHorizRef)
      *sousbase = pBox->BxHeight - pBox->BxHorizRef;
}


/* ---------------------------------------------------------------------- */
/* |    RemplirLigne remplit la ligne dont l'adresse est passee en      | */
/* |            parametre pour l'image abstraite pRootAb.                    | */
/*CP |          Le parametre uneSuite determine si la paragraphe a ou   | */
/*CP |          n'a pas de suite dans une page suivante.                | */
/* |            suppose LiFirstBox et LiFirstPiece pre'alablement remplis.     | */
/* |            calcule l'indice de la derniere boite qui peut       | */
/* |            entrer dans la ligne et on met a` jour le descripteur   | */
/* |            de ligne : LiLastBox et LiLastPiece (si coupee).                | */
/* |            Les positions des boites mises en lignes sont calculees | */
/* |            par rapport a` la boite racine.                         | */
/* |            Au retour : plein indique si la ligne est pleine ou non.| */
/* ---------------------------------------------------------------------- */
#ifdef __COLPAGE__
#ifdef __STDC__
void                RemplirLigne (PtrLine pLine, PtrAbstractBox pRootAb, boolean uneSuite, boolean * plein, boolean * ajust)

#else  /* __STDC__ */
void                RemplirLigne (pLine, pRootAb, uneSuite, plein, ajust)
PtrLine            pLine;
PtrAbstractBox             pRootAb;
boolean             uneSuite;
boolean            *plein;
boolean            *ajust;

#endif /* __STDC__ */

#else  /* __COLPAGE__ */
#ifdef __STDC__
void                RemplirLigne (PtrLine pLine, PtrAbstractBox pRootAb, boolean * plein, boolean * ajust)

#else  /* __STDC__ */
void                RemplirLigne (pLine, pRootAb, plein, ajust)
PtrLine            pLine;
PtrAbstractBox             pRootAb;
boolean            *plein;
boolean            *ajust;

#endif /* __STDC__ */
#endif /* __COLPAGE__ */

{
   int                 xi, xmax;
   int                 lmax;
   boolean             still;
   boolean             coup;
   PtrBox            pBox;
   PtrBox            nbox;
   int                 surbase;
   int                 sousbase;
   PtrBox            lastbox;
   int                 width;
   int                 brWidthec;
   int                 nCharssl;
   int                 nSpaces;
   int                 newIndex;
   PtrTextBuffer      pNewBuff;
   boolean             csexist;

   *plein = TRUE;
   *ajust = TRUE;
   coup = FALSE;
   still = TRUE;
   xmax = pLine->LiXMax;
   xi = 0;
   surbase = 0;
   sousbase = 0;
   nbox = pLine->LiFirstBox;	/* la boite que l'on traite */
   pLine->LiLastPiece = NULL;
   csexist = FALSE;

   /* nbox est la boite courante que l'on traite    */
   /* pBox est la derniere boite mise dans la ligne */

   /* La premiere boite est la suite d'une boite deja coupee ? */
   if (pLine->LiFirstPiece != NULL)
     {
	/* La boite de coupure entre obligatoirement dans la ligne */
	pBox = pLine->LiFirstPiece;

	/* Est-ce que l'on rencontre une coupure force dans la boite ? */
	csexist = SearchBreakLine (pBox, &width, &brWidthec, &nCharssl, &nSpaces, &newIndex, &pNewBuff);
	/* La coupure forcee arrive avant la fin de ligne */
	if (csexist && width + xi <= xmax)
	  {
	     ManageBreakLine (pBox, width, brWidthec, nCharssl, nSpaces, newIndex, pNewBuff, pRootAb);
	     *ajust = FALSE;
	     still = FALSE;
	  }
	/* La boite entre dans toute entiere dans la ligne */
	else if (pBox->BxWidth + xi <= xmax)
	  {
	     nbox = Suivante (pBox->BxAbstractBox);
	     if (nbox == NULL)
	       {
		  *plein = FALSE;
		  still = FALSE;
	       }
	  }
	/* Il faut couper une autre fois la boite */
	else
	  {
	     still = FALSE;
	     ReBreakBox (pLine, pBox, xmax - xi, pRootAb);
	  }

	pBox = pLine->LiFirstPiece;
	xi += pBox->BxWidth;
     }
   else
      /* Il n'y a pas encore de boite dans la ligne */
      pBox = NULL;

   /* recherche la boite qu'il faut couper */
   while (still)

      /* refuse de mettre en ligne une boite non englobee */
      if (!nbox->BxAbstractBox->AbHorizEnclosing)
	{
	   *plein = TRUE;
	   still = FALSE;

	   /* Est-ce la premiere boite de la ligne ? */
	   if (pBox == NULL)
	      pLine->LiLastPiece = pLine->LiFirstPiece;
	   /* Est-ce que la boite precedente est coupee ? */
	   else if (pBox->BxType == BoPiece || pBox->BxType == BoDotted)
	      pLine->LiLastPiece = pLine->LiFirstPiece;
	}
      else
	{
	   /* Recherche si on doit forcer un return dans la boite */
	   if (nbox->BxAbstractBox->AbAcceptLineBreak)
	      csexist = SearchBreakLine (nbox, &width, &brWidthec, &nCharssl, &nSpaces, &newIndex, &pNewBuff);
	   else
	      csexist = FALSE;

	   /* La boite produite par le return force' tient dans la ligne ? */
	   if (csexist && width + xi <= xmax)
	     {
		still = FALSE;
		ManageBreakLine (nbox, width, brWidthec, nCharssl, nSpaces, newIndex, pNewBuff, pRootAb);
		*ajust = FALSE;
		if (nbox->BxNexChild != NULL)
		  {
		     pBox = nbox->BxNexChild;
		     /* Est-ce la boite unique de la ligne ? */
		     if (nbox == pLine->LiFirstBox)
			pLine->LiFirstPiece = pBox;
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
		     still = FALSE;
		  }
	     }
	   /* Il faut couper la boite ou une boite precedente */
	   else
	     {
		coup = TRUE;
		still = FALSE;
	     }
	}

   /* Traitement des coupures de boites */
   if (coup)
     {
	/* va essayer de couper la boite nbox ou une boite precedente */
	lmax = xmax - xi;

	/* Est-ce la boite unique de la ligne ? */
	if (nbox == pLine->LiFirstBox)
	   /* Si elle n'est pas secable -> on laisse deborder */
	   if (!nbox->BxAbstractBox->AbAcceptLineBreak
	       || nbox->BxAbstractBox->AbLeafType != LtText)
	      pBox = nbox;	/* derniere boite de la ligne */
	/* Sinon on coupe la boite texte en un point quelconque */
	   else
	     {
		BreakBox (pLine, nbox, lmax, pRootAb, TRUE);	/* coupure forcee */
		if (nbox->BxNexChild != NULL)
		  {
		     pBox = nbox->BxNexChild;
		     pLine->LiFirstPiece = pBox;
		  }
	     }
	/* Il y a au moins une autre boite dans la ligne */
	else
	  {
	     /* essaie de couper la boite texte secable */
	     if (nbox->BxAbstractBox->AbLeafType == LtText
		 && nbox->BxAbstractBox->AbAcceptLineBreak
	     /* avec au moins un blanc ou avec coupure de mot */
		 && (Hyphenable (nbox) || nbox->BxNSpaces != 0))
	       {
		  BreakBox (pLine, nbox, lmax, pRootAb, FALSE);		/* coupure sur blanc */
		  if (nbox->BxNexChild != NULL)
		    {
		       /* a trouve le point de coupure */
		       pBox = nbox->BxNexChild;
		       coup = FALSE;
		    }
	       }

	     lastbox = nbox;
	     /* Si la coupure a echoue' on recherche en arriere un point de coupure */
	     while (coup)
	       {
		  /* essaye de couper la boite precedente */
		  if (nbox != NULL)
		     nbox = Precedente (nbox->BxAbstractBox);

		  /* Est-ce que l'on bute sur la premiere boite de la ligne ? */
		  if (nbox == pLine->LiFirstPiece)
		     coup = FALSE;
		  else if (nbox == pLine->LiFirstBox)
		    {
		       coup = FALSE;
		       if (pLine->LiFirstPiece != NULL)
			  nbox = pLine->LiFirstPiece;
		    }

		  /* Si c'est une boite de texte secable */
		  if (nbox->BxAbstractBox->AbLeafType == LtText
		      && nbox->BxWidth != 0	/* visible */
		      && nbox->BxAbstractBox->AbAcceptLineBreak
		  /* avec au moins un blanc si on est en insertion */
		      && nbox->BxNSpaces != 0)

		     /* coupe sur le dernier blanc de la boite */
		     if (nbox->BxType == BoPiece)
		       {
			  ReBreakBox (pLine, nbox, nbox->BxWidth - 1, pRootAb);
			  pBox = nbox;
		       }
		     else
		       {
			  BreakBox (pLine, nbox, nbox->BxWidth - 1, pRootAb, FALSE);
			  pBox = nbox->BxNexChild;
			  /* Est-ce que l'on a coupe la premiere boite de la ligne ? */
			  if (nbox == pLine->LiFirstBox && pLine->LiFirstPiece == NULL)
			     pLine->LiFirstPiece = pBox;
			  coup = FALSE;
		       }
		  /* Si la coupure sur un blanc echoue on coupe la derniere boite de la ligne */
		  else if (!coup)
		    {
		       /* Si la derniere boite est secable -> force sa coupure */
		       if (lastbox->BxAbstractBox->AbAcceptLineBreak
			   && lastbox->BxAbstractBox->AbLeafType == LtText)
			 {
			    BreakBox (pLine, lastbox, lmax, pRootAb, TRUE);	/* coupure forcee */
			    pBox = lastbox->BxNexChild;
			 }
		       /* Si la boite est seule dans la ligne -> laisse deborder */
		       else if (lastbox == pLine->LiFirstBox)
			  pBox = lastbox;
		       /* sinon on coupe avant la derniere boite */
		       else
			 {
			    pBox = Precedente (lastbox->BxAbstractBox);
			    /* Si c'est la premiere boite de la ligne et que celle-ci ets coupee */
			    if (pBox == pLine->LiFirstBox && pLine->LiFirstPiece != NULL)
			       pBox = pLine->LiFirstPiece;
			 }
		    }
		  /* Sinon on continue la recherche en arriere */
		  else
		     pBox = nbox;
	       }
	  }
     }

   /* ajoute toutes les boites de la ligne */
   if (pLine->LiFirstPiece != NULL)
      nbox = pLine->LiFirstPiece;
   else
      nbox = pLine->LiFirstBox;

   still = TRUE;
   while (still)
     {
	if (nbox->BxType == BoSplit)
	   nbox = nbox->BxNexChild;
	AjoutBoite (nbox, &sousbase, &surbase, pLine);
	if (nbox == pBox)
	   still = FALSE;
	else
	   nbox = Suivante (nbox->BxAbstractBox);
     }

   /* termine le chainage */
   if (pBox->BxType == BoPiece || pBox->BxType == BoDotted)
     {
	pLine->LiLastPiece = pBox;
	pLine->LiLastBox = pBox->BxAbstractBox->AbBox;
     }
   else
      pLine->LiLastBox = pBox;

   /* teste s'il lostPixels des boites a mettre en ligne */
   if (pBox->BxNexChild == NULL && Suivante (pBox->BxAbstractBox) == NULL)
      *plein = FALSE;

   /* coupe les blancs en fin de ligne pleine */
#ifdef __COLPAGE
   if ((uneSuite || *plein) && pBox->BxAbstractBox->AbLeafType == LtText && pBox->BxNSpaces != 0)
#else  /* __COLPAGE__ */
   if (*plein && pBox->BxAbstractBox->AbLeafType == LtText && pBox->BxNSpaces != 0)
#endif /* __COLPAGE__ */
      if (pLine->LiLastPiece == NULL)
	{
	   lmax = pBox->BxWidth;
	   BreakBox (pLine, pBox, lmax, pRootAb, FALSE);	/*coupure sur un caractere refusee */
	   if (pBox->BxNexChild != NULL)
	     {
		/* remplace la boite entiere par la boite de coupure */
		pLine->LiRealLength = pLine->LiRealLength - lmax + pBox->BxNexChild->BxWidth;
		pLine->LiLastPiece = pBox->BxNexChild;
	     }
	}
      else if (pLine->LiLastPiece->BxNexChild == NULL)
	{
	   pBox = pLine->LiLastPiece;
	   lmax = pBox->BxWidth;
	   ReBreakBox (pLine, pBox, lmax, pRootAb);
	   /* met a jour la largeur de la ligne */
	   pLine->LiRealLength = pLine->LiRealLength - lmax + pBox->BxWidth;
	}

   /* Calcule la hauteur et la base de la ligne */
   pLine->LiHeight = sousbase + surbase;
   pLine->LiHorizRef = surbase;
   /* les boites ne sont pas positionnees dans la ligne */
}


/* ---------------------------------------------------------------------- */
/* |    RazJustif recalcule la largueur d'une boite apres suppression de| */
/* |            la justification. Met a jour les marques de selection   | */
/* |            que la boite soit justifiee ou non.                     | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         RazJustif (PtrBox pBox, int spaceWidth)
#else  /* __STDC__ */
static void         RazJustif (pBox, spaceWidth)
PtrBox            pBox;
int                 spaceWidth;
#endif /* __STDC__ */
{
   int                 x;
   int                 l;

   /* Box justifiee -> met a jour sa largeur et les marques */
   if (pBox->BxSpaceWidth != 0)
     {
	/* blanc justifie - blanc de la police */
	x = pBox->BxSpaceWidth - spaceWidth;
	l = pBox->BxWidth - x * pBox->BxNSpaces - pBox->BxNPixels;
	pBox->BxWidth = l;
	pBox->BxSpaceWidth = 0;
	pBox->BxNPixels = 0;
     }
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
void                BlocDeLigne (PtrBox pBox, int frame, int *height)

#else  /* __STDC__ */
void                BlocDeLigne (pBox, frame, height)
PtrBox            pBox;
int                 frame;
int                *height;

#endif /* __STDC__ */

{
   PtrLine            pLine1;
   PtrLine            pLine2;
   PtrLine            pLine3;
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
   AbPosition        *pPosAb;
   boolean             ajustif;
   boolean             orgXComplete;
   boolean             orgYComplete;

   /* Remplissage de la boite bloc de ligne */
   x = 0;
   pAbbox1 = pBox->BxAbstractBox;

   /* evalue si le positionnement en X et en Y doit etre absolu */
   XYEnAbsolu (pBox, &orgXComplete, &orgYComplete);

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
	     /* recherche la premiere boite mise en ligne */
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
		     /* descend la hierarchie */
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
		  GetLine (&pLine2);
		  pBox->BxFirstLine = pLine2;
	       }

	     boxcoup = NULL;
	     pLine1 = NULL;
	     *height = 0;		/* hauteur de la boite bloc de lignes */
	     org = 0;		/* origine de la nouvelle ligne */
	     interlignage = FALSE;
	  }
	/* Reconstruction partielle du bloc de ligne */
	else
	  {
	     pLine1 = pBox->BxLastLine;
	     pavefils = pLine1->LiLastBox->BxAbstractBox;
	     boxcoup = pLine1->LiLastPiece;
	     *height = pLine1->LiYOrg + pLine1->LiHeight;	/* hauteur boite bloc de lignes */
	     /* Origine de la future ligne */
	     if (pavefils->AbHorizEnclosing)
	       {
		  /* C'est une boite englobee */
		  org = pLine1->LiYOrg + pLine1->LiHorizRef + itl;
		  /* utilise l'interligne */
		  interlignage = TRUE;
	       }
	     else
	       {
		  /* La boite n'est pas englobee (Page) */
		  org = *height;	/* colle la nouvelle ligne en dessous de celle-ci */
		  interlignage = FALSE;
	       }

	     if (boxcoup != NULL)
	       {
		  boxcoup = boxcoup->BxNexChild;
		  /* Cas particulier de la derniere boite coupee */
		  if (boxcoup != NULL)
		     box1 = pLine1->LiLastBox;
	       }

	     if (box1 == NULL)
		box1 = Suivante (pavefils);

	     if (box1 == NULL)
		complet = FALSE;	/* Rien a mettre en ligne */
	     /* prepare la nouvelle ligne */
	     else
	       {
		  GetLine (&pLine2);
		  pLine1->LiNext = pLine2;
		  /* Si la 1ere boite nouvellement mise en lignes n'est pas englobee */
		  if (!box1->BxAbstractBox->AbHorizEnclosing)
		    {
		       org = *height;
		       /* colle la nouvelle ligne en dessous */
		       interlignage = FALSE;
		    }
	       }
	  }

	/* cree des lignes tant qu'elles sont pleines */
	while (complet)
	  {
	     /* Initialise la ligne */
	     pLine2->LiPrevious = pLine1;
	     /* regarde si la boite deborde des lignes */
	     if (!box1->BxAbstractBox->AbHorizEnclosing)
	       {
		  pBo2 = box1;
		  pLine2->LiXOrg = pBo2->BxXOrg;
		  pLine2->LiYOrg = *height;	/* Colle la boite en dessous de la precedente */
		  pLine2->LiXMax = pBo2->BxWidth;
		  pLine2->LiHeight = pBo2->BxHeight;
		  pLine2->LiFirstBox = box1;
		  pLine2->LiLastBox = box1;
		  pLine2->LiFirstPiece = NULL;
		  pLine2->LiLastPiece = NULL;
		  if (Propage != ToSiblings || pBox->BxVertFlex)
		     org = pBox->BxYOrg + *height;
		  else
		     org = *height;
		  DepOrgY (box1, NULL, org - pBo2->BxYOrg, frame);
		  *height += pLine2->LiHeight;
		  org = *height;
		  interlignage = FALSE;
	       }
	     else
	       {
		  /* Indentation des lignes */
#ifdef __COLPAGE__
		  if (pLine1 != NULL || pAbbox1->AbTruncatedHead)
#else  /* __COLPAGE__ */
		  if (pLine1 != NULL)
#endif /* __COLPAGE__ */
		    {
		       if (pAbbox1->AbIndent < 0)
			  pLine2->LiXOrg = idl;
		    }
		  else if (pAbbox1->AbIndent > 0)
		     pLine2->LiXOrg = idl;
		  if (pLine2->LiXOrg >= pBox->BxWidth)
		     pLine2->LiXOrg = 0;
		  pLine2->LiXMax = pBox->BxWidth - pLine2->LiXOrg;
		  pLine2->LiFirstBox = box1;
		  pLine2->LiFirstPiece = boxcoup;	/* ou  NULL si la boite est entiere */

		  /* Remplissage de la ligne au maximum */
#ifdef __COLPAGE__
		  RemplirLigne (pLine2, FntrTable[frame - 1].FrAbstractBox, pAbbox1->AbTruncatedTail, &complet, &ajustif);
#else  /* __COLPAGE__ */
		  RemplirLigne (pLine2, FntrTable[frame - 1].FrAbstractBox, &complet, &ajustif);
#endif /* __COLPAGE__ */

		  /* Positionnement de la ligne en respectant l'interligne */
		  if (interlignage)
		     org -= pLine2->LiHorizRef;
		  /* verifie que les lignes ne se chevauchent pas */
		  if (org < *height)
		     org = *height;

		  pLine2->LiYOrg = org;
		  *height = org + pLine2->LiHeight;
		  org = org + pLine2->LiHorizRef + itl;
		  interlignage = TRUE;
		  /* Teste le cadrage des lignes */
#ifdef __COLPAGE__
		  if (ajustif && (complet || pAbbox1->AbTruncatedTail) && pAbbox1->AbJustify)
#else  /* __COLPAGE__ */
		  if (ajustif && complet && pAbbox1->AbJustify)
#endif /* __COLPAGE__ */
		     Adjust (pBox, pLine2, frame, orgXComplete, orgYComplete);
		  else
		    {
		       x = pLine2->LiXOrg;
		       if (pAbbox1->AbAdjust == AlignCenter)
			  x += (pLine2->LiXMax - pLine2->LiRealLength) / 2;
		       else if (pAbbox1->AbAdjust == AlignRight)
			  x = x + pLine2->LiXMax - pLine2->LiRealLength;
		       /* Decale toutes les boites de la ligne */
		       Aligner (pBox, pLine2, x, frame, orgXComplete, orgYComplete);
		    }
	       }

	     /* Est-ce la derniere ligne du bloc de ligne ? */
	     if (complet)
	       {
		  box1 = Suivante (pLine2->LiLastBox->BxAbstractBox);
		  if (pLine2->LiLastPiece == NULL)
		     boxcoup = NULL;
		  else
		    {
		       boxcoup = pLine2->LiLastPiece->BxNexChild;
		       /* Est-ce qu'il y a une boite coupee ? */
		       if (boxcoup != NULL)
			  /* Est-ce une boite vide ? */
			  if (boxcoup->BxNChars > 0)
			     box1 = pLine2->LiLastBox;
			  else if (box1 == NULL)
			     box1 = pLine2->LiLastBox;
			  else
			     boxcoup = NULL;
		    }

		  if (box1 == NULL)
		    {
		       /* Il n'y a plus de boite a traiter */
		       complet = FALSE;
		       pBox->BxLastLine = pLine2;
		    }
		  else
		    {
		       /* prepare la ligne suivante */
		       pLine1 = pLine2;
		       pLine3 = pLine2;
		       GetLine (&pLine2);
		       pLine3->LiNext = pLine2;
		    }
	       }
	     else
	       {
		  pBox->BxLastLine = pLine2;
		  /* Note la largeur de la fin de bloc si le remplissage est demande */
		  if (pAbbox1->AbAdjust == AlignLeftDots)
		    {
		       pLi2 = pLine2;
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

	/* met a jour la base du bloc de lignes   */
	/* s'il depend de la premiere boite englobee */
	if (pAbbox1->AbHorizRef.PosAbRef == pAbbox1->AbFirstEnclosed && pBox->BxFirstLine != NULL)
	  {
	     pPosAb = &pAbbox1->AbHorizRef;
	     x = PixelValue (pPosAb->PosDistance, pPosAb->PosUnit, pAbbox1);
	     DepBase (pBox, NULL, pBox->BxFirstLine->LiHorizRef + x - pBox->BxHorizRef, frame);
	  }
     }
   else
      *height = FontHeight (pBox->BxFont);
}


/* ---------------------------------------------------------------------- */
/* |    DecalLigne decale de x les boites de la ligne de pBox incluse   | */
/* |            dans le boc de ligne pAb et la ligne pLine.            | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void         DecalLigne (PtrLine pLine, PtrAbstractBox pAb, PtrBox pBox, int x, int frame)

#else  /* __STDC__ */
static void         DecalLigne (pLine, pAb, pBox, x, frame)
PtrLine            pLine;
PtrAbstractBox             pAb;
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
   pLine->LiRealLength += x;
   /* prepare le reaffichage de la ligne */
   EvalAffich = FALSE;
   /* Bornes de reaffichage a priori */
   pFrame = &FntrTable[frame - 1];
   xd = pFrame->FrClipXBegin;
   xf = pFrame->FrClipXEnd;
   yd = iboxSave->BxYOrg + iboxSave->BxHorizRef - pLine->LiHorizRef;
   yf = yd + pLine->LiHeight;

   /* reaffiche la fin de la ligne */
   if (pAb->AbAdjust == AlignLeft
       || pAb->AbAdjust == AlignLeftDots
   /* on force le cadrage a gauche si le bloc de lignes prend la largeur */
   /* du contenu */
       || pAb->AbBox->BxContentWidth)
     {
	if (pLine->LiLastPiece == NULL)
	   boxN = pLine->LiLastBox;
	else
	   boxN = pLine->LiLastPiece;
	if (boxN != pBox)
	  {
	     xf = boxN->BxXOrg + boxN->BxWidth;
	     if (x > 0)
		xf += x;
	  }

	/* Note la largeur de la fin de bloc si le remplissage est demande! */

	if (pAb->AbAdjust == AlignLeftDots && pLine->LiNext == NULL)
	   boxN->BxEndOfBloc -= x;
     }
   /* reaffiche toute la ligne */
   else if (pAb->AbAdjust == AlignCenter)
     {
	i = x;
	x = (pLine->LiXMax - pLine->LiRealLength) / 2 - (pLine->LiXMax + i - pLine->LiRealLength) / 2;
	if (pLine->LiFirstPiece == NULL)
	   box1 = pLine->LiFirstBox;
	else
	   box1 = pLine->LiFirstPiece;
	xd = box1->BxXOrg;
	if (x < 0)
	   xd += x;
	if (pLine->LiLastPiece == NULL)
	   boxN = pLine->LiLastBox;
	else
	   boxN = pLine->LiLastPiece;
	if (boxN != pBox)
	   xf = boxN->BxXOrg + boxN->BxWidth;
	if (x < 0)
	   xf -= x;

	/* decale les boites precedentes */
	DepOrgX (box1, NULL, x, frame);
	while (box1 != pBox)
	  {
	     box1 = Suivante (box1->BxAbstractBox);
	     if (box1->BxType == BoSplit)
		box1 = box1->BxNexChild;
	     DepOrgX (box1, NULL, x, frame);
	  }
	/* decale les boites suivantes */
	x = -x;
     }
   /* reaffiche le debut de la ligne */
   else if (pAb->AbAdjust == AlignRight)
     {
	x = -x;
	boxN = pBox;
	if (pLine->LiFirstPiece == NULL)
	   pBox = pLine->LiFirstBox;
	else
	   pBox = pLine->LiFirstPiece;
	xd = iboxSave->BxXOrg;
	if (x < 0)
	   xd += x;
	DepOrgX (pBox, NULL, x, frame);
     }

   /* decale les boites suivantes */
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
static void         CompresLigne (PtrLine pLine, int dx, int frame, int dblanc)

#else  /* __STDC__ */
static void         CompresLigne (pLine, dx, frame, dblanc)
PtrLine            pLine;
int                 dx;
int                 frame;
int                 dblanc;

#endif /* __STDC__ */

{
   int                 nSpaces;
   int                 length, opixel;
   int                 bl, rest;
   PtrBox            box1, box2;
   ViewSelection            *pMa1;

   rest = pLine->LiNPixels - dx;
   bl = 0;
   nSpaces = pLine->LiNSpaces;
   EvalAffich = FALSE;

   /* Faut-il changer la largeur des blancs ? */
   if (dx > 0)
      while (rest < 0)
	{
	   rest += nSpaces;
	   bl++;
	}
   else
      while (rest >= nSpaces)
	{
	   rest -= nSpaces;
	   bl--;
	}

   /* met a jour la ligne */
   pLine->LiNPixels = rest;
   /* Pour chaque blanc insere ou retire on compte la largeur minimale */
   dx -= dblanc * (pLine->LiSpaceWidth - MIN_BLANC);
   pLine->LiMinLength += dx;
   pLine->LiSpaceWidth -= bl;

   /* prepare le reaffichage de toute la ligne */
   if (pLine->LiFirstPiece != NULL)
      box2 = pLine->LiFirstPiece;
   else
      box2 = pLine->LiFirstBox;
   length = box2->BxXOrg;
   DefClip (frame, length, box2->BxYOrg + box2->BxHorizRef - pLine->LiHorizRef,
	    length + pLine->LiXMax,
	    box2->BxYOrg + box2->BxHorizRef - pLine->LiHorizRef + pLine->LiHeight);

   /* met a jour chaque boite */
   do
     {
	if (box2->BxType == BoSplit)
	   box1 = box2->BxNexChild;
	else
	   box1 = box2;

	DepOrgX (box1, NULL, length - box1->BxXOrg, frame);
	if (box1->BxAbstractBox->AbLeafType == LtText && box1->BxNChars != 0)
	  {
	     box1->BxWidth -= box1->BxNSpaces * bl;
	     box1->BxSpaceWidth = pLine->LiSpaceWidth;

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
	length += box1->BxWidth;
	box2 = Suivante (box1->BxAbstractBox);
     }
   while (!(box1 == pLine->LiLastBox || box1 == pLine->LiLastPiece));
   EvalAffich = TRUE;
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
   int                 lostPixels;
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
	     /* met a jour les marques de selection */
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

	     /* va liberer des boites coupure */
	     if (pBox->BxType == BoComplete)
		RazJustif (pBox, x);
	     else
	       {
		  ibox2 = pBox->BxNexChild;		/* 1e boite de coupure liberee */
		  pBox->BxNexChild = NULL;
		  if (pBox->BxType == BoSplit)
		    {
		       /* met a jour la boite coupee */
		       pBox->BxType = BoComplete;
		       pBox->BxPrevious = ibox2->BxPrevious;
		       /* transmet la position courante de la boite */
		       pBox->BxXOrg = ibox2->BxXOrg;
		       pBox->BxYOrg = ibox2->BxYOrg;
		       if (pBox->BxPrevious != NULL)
			  pBox->BxPrevious->BxNext = pBox;
		       else
			  FntrTable[frame - 1].FrAbstractBox->AbBox->BxNext = pBox;

		       larg = 0;
		       nombre = 0;
		       lostPixels = 0;
		    }
		  else
		    {
		       /* met a jour la derniere boite de coupure conservee */
		       RazJustif (pBox, x);
		       larg = pBox->BxWidth;
		       /* Il faut retirer l'espace reserve au tiret d'hyphenation */
		       if (pBox->BxType == BoDotted)
			 {
			    larg -= CarWidth (173, pBox->BxFont);
			    pBox->BxType = BoPiece;
			 }
		       nombre = pBox->BxNSpaces;
		       lostPixels = pBox->BxIndChar + pBox->BxNChars;
		    }

		  /* Libere les boites de coupure */
		  if (ibox2 != NULL)
		    {
		       do
			 {
			    ibox1 = ibox2;
			    RazJustif (ibox1, x);
			    /* Nombre de blancs sautes */
			    diff = ibox1->BxIndChar - lostPixels;
			    if (diff > 0)
			      {
				/* ajoute les blancs sautes */
				 larg += diff * x;
				 nombre += diff;
			      }
			    else if (ibox1->BxType == BoDotted)
			       /* retire la largeur du tiret d'hyphenation */
			       larg -= CarWidth (173, ibox1->BxFont);

			    /* Si la boite suivante n'est pas vide */
			    if (ibox1->BxNChars > 0)
			      {
				 nombre += ibox1->BxNSpaces;
				 larg += ibox1->BxWidth;
			      }
			    lostPixels = ibox1->BxIndChar + ibox1->BxNChars;
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
			    lostPixels = lostPixels - pBox->BxIndChar - pBox->BxNChars;
			    if (lostPixels > 0)
			      {
				 larg += lostPixels * x;
				 nombre += lostPixels;
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
/* |            ces lignes.						| */
/* |		changeSelectBegin et changeSelectEnd sont bascules si   | */
/* |            la boite referencee par la marque Debut ou Fin de       | */
/* |            Selection est liberee.                                  | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                DispBloc (PtrBox pBox, int frame, PtrLine premligne, boolean * changeSelectBegin, boolean * changeSelectEnd)
#else  /* __STDC__ */
void                DispBloc (pBox, frame, premligne, changeSelectBegin, changeSelectEnd)
PtrBox            pBox;
int                 frame;
PtrLine            premligne;
boolean            *changeSelectBegin;
boolean            *changeSelectEnd;
#endif /* __STDC__ */
{
   PtrBox            box1;
   PtrLine            lignesuiv;
   PtrLine            pLine;
   PtrAbstractBox             pAb;

   *changeSelectBegin = FALSE;
   *changeSelectEnd = FALSE;
   pLine = premligne;
   if (pLine != NULL)
     {
	if (pBox->BxType == BoBlock)
	  {
	     /* Mise a jour du chainage des lignes */
	     box1 = NULL;
	     if (pLine->LiPrevious != NULL)
	       {
		  pLine->LiPrevious->LiNext = NULL;
		  box1 = pLine->LiFirstBox;
		  /* Est-ce que la premiere boite est une boite suite ? */
		  if (pLine->LiPrevious->LiLastBox == box1)
		     DispCoup (pLine->LiFirstPiece, frame, changeSelectBegin, changeSelectEnd);
		  else
		     DispCoup (box1, frame, changeSelectBegin, changeSelectEnd);
		  pBox->BxLastLine = pLine->LiPrevious;
	       }
	     else
	       {
		  pBox->BxFirstLine = NULL;
		  pBox->BxLastLine = NULL;
	       }

	     /* Liberation des lignes */
	     while (pLine != NULL)
	       {
		  lignesuiv = pLine->LiNext;
		  FreeLine (pLine);
		  pLine = lignesuiv;
	       }

	     /* Liberation des boites de coupure */
	     if (box1 != NULL)
		box1 = Suivante (box1->BxAbstractBox);
	     else
	       {
		  /* recherche la premiere boite mise en ligne */
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
			pAb = pAb->AbFirstEnclosed;	/* descend la hierarchie */
		  /* Sinon c'est la boite du pave */
		     else
			box1 = pAb->AbBox;
	       }

	     while (box1 != NULL)
	       {
		  DispCoup (box1, frame, changeSelectBegin, changeSelectEnd);
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
   int                 l, h, height;
   int                 width;
   boolean             changeSelectBegin;
   boolean             changeSelectEnd;
   boolean             SvAff;
   unsigned char       charIndex;
   PtrLine            pLine;
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
	pLine = lig;

	if (pLine == NULL)
	   pLine = pBox->BxFirstLine;

	/* Zone affichee avant modification */
	if (pLine == NULL)
	  {
	     l = 0;
	     h = pBox->BxYOrg;
	  }
	else
	  {
	     l = pLine->LiXOrg + pLine->LiXMax;
	     h = pBox->BxYOrg + pLine->LiYOrg;
	  }

	/* Si l'origne de la reevaluation du bloc de ligne vient d'une boite */
	/* de coupure, il faut deplacer cette origne sur la boite coupee     */
	/* parce que DispBloc va liberer toutes les boites de coupure.       */
	if (org != NULL)
	   if (org->BxType == BoPiece || org->BxType == BoDotted)
	      org = org->BxAbstractBox->AbBox;

	SvAff = EvalAffich;
	EvalAffich = FALSE;
	DispBloc (pBox, frame, pLine, &changeSelectBegin, &changeSelectEnd);
	if (pBox->BxFirstLine == NULL)
	  {
	     /* fait reevaluer la mise en lignes et on recupere */
	     /* la hautteur et la largeur du contenu de la boite */
	     EvalComp (pAb, frame, &width, &height);
	  }
	else
	  {
	     BlocDeLigne (pBox, frame, &height);
	     width = 0;
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
	     if (height > pBox->BxHeight)
		DefClip (frame, pBox->BxXOrg, h, l, pBox->BxYOrg + height);
	     else
		DefClip (frame, pBox->BxXOrg, h, l, pBox->BxYOrg + pBox->BxHeight);
	  }

	/* Faut-il reevaluer les marques de selection ? */
	pFrame = &FntrTable[frame - 1];
	pMa0 = &pFrame->FrSelectionBegin;
	if (changeSelectBegin && pMa0->VsBox != NULL)
	  {
	     /* Si la selection a encore un sens */
	     if (pMa0->VsBox->BxAbstractBox != NULL)
		ReevalMrq (pMa0);
	  }

	pMa1 = &pFrame->FrSelectionEnd;
	if (changeSelectEnd && pMa1->VsBox != NULL)
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
		       charIndex = (unsigned char) (pMa1->VsBuffer->BuContent[pMa1->VsIndBuf - 1]);
		       if (charIndex == BLANC && pBo2->BxSpaceWidth != 0)
			  pMa1->VsXPos += pBo2->BxSpaceWidth;
		       else
			  pMa1->VsXPos += CarWidth (charIndex, pBo2->BxFont);
		    }
	       }
	  }

	if (width != 0 && width != pBox->BxWidth)
	   ChangeLgContenu (pBox, org, width, 0, frame);
	/* Faut-il conserver la hauteur ? */
	if (height != 0)
	  {
	     /* Il faut propager la modification de hauteur */
	     SvPrpg = Propage;
	     /*if (SvPrpg == ToChildren) Propage = ToAll; */
	     ChangeHtContenu (pBox, org, height, frame);
	     Propage = SvPrpg;
	  }
     }
}


/* ---------------------------------------------------------------------- */
/* |    MajBloc met a` jour le bloc de ligne (pave) apres modification  | */
/* |            de la largeur de la boite incluse pBox de dx.           | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                MajBloc (PtrAbstractBox pave, PtrLine pLine, PtrBox pBox, int dx, int dbl, int frame)

#else  /* __STDC__ */
void                MajBloc (pave, pLine, pBox, dx, dbl, frame)
PtrAbstractBox             pave;
PtrLine            pLine;
PtrBox            pBox;
int                 dx;
int                 dbl;
int                 frame;

#endif /* __STDC__ */

{
   int                 lostPixels;
   int                 maxlostPixels;
   PtrBox            box1;
   int                 nChars;
   int                 nlcour = 0;
   int                 larg, lmax;
   int                 newIndex;
   int                 nSpaces;
   PtrTextBuffer      pNewBuff;
   PtrBox            pBo1;
   AbDimension       *pDimAb;
   PtrLine            pLi2;

   /* Si la boite est eclatee, on remonte jusqu'a la boite bloc de lignes */
   while (pave->AbBox->BxType == BoGhost)
      pave = pave->AbEnclosing;
   pBo1 = pave->AbBox;

   if (pLine != NULL)
     {
	pDimAb = &pave->AbWidth;
	/* C'est une ligne extensible --> on l'etend */
	if (pBo1->BxContentWidth)
	  {
	     /* Si le bloc de lignes deborde de sa dimension minimale */
	     if (!pDimAb->DimIsPosition && pDimAb->DimMinimum
		 && pBo1->BxWidth + dx < pBo1->BxRuleWidth)
	       {
		  /* Il faut prendre la largeur du minimum */
		  pBo1->BxContentWidth = FALSE;
		  ReevalBloc (pave, NULL, NULL, frame);
	       }
	     else
	       {
		  DecalLigne (pLine, pave, pBox, dx, frame);
		  pLine->LiXMax = pLine->LiRealLength;
		  ModLarg (pBo1, pBo1, NULL, dx, 0, frame);
	       }
	  }
	/* Si le bloc de lignes deborde de sa dimension minimale */
	else if (!pDimAb->DimIsPosition && pDimAb->DimMinimum
		 && pLine->LiRealLength + dx > pBo1->BxWidth)
	  {
	     /* Il faut prendre la largeur du contenu */
	     pBo1->BxContentWidth = TRUE;
	     ReevalBloc (pave, NULL, NULL, frame);
	  }
	/* C'est une ligne non extensible */
	else
	  {
	     /* calcule la place qu'il lostPixels dans la ligne courante */
	     pLine->LiNSpaces += dbl;
	     maxlostPixels = pLine->LiNSpaces * MAX_BLANC + dx;
	     if (pLine->LiSpaceWidth > 0)
	       {
		  /* Line justifiee */
		  lostPixels = CarWidth (BLANC, pBox->BxFont);
		  nlcour = pLine->LiRealLength + dx - dbl * (pLine->LiSpaceWidth - lostPixels);
		  lostPixels = pLine->LiXMax - pLine->LiMinLength;
	       }
	     else
		lostPixels = pLine->LiXMax - pLine->LiRealLength;

	     /* Est-ce que la boite debordait de la ligne ? */
	     if (pBox->BxWidth - dx > pLine->LiXMax)
	       {
		  lostPixels = pLine->LiXMax - pBox->BxWidth;	/* Pixels liberes dans la ligne */
		  if (lostPixels > 0)
		     ReevalBloc (pave, pLine, NULL, frame);
	       }
	     /* Peut-on compresser ou completer la ligne ? */
	     else if ((dx > 0 && dx <= lostPixels)
		      || (dx < 0 && (lostPixels < maxlostPixels
		  || (pLine->LiPrevious == NULL && pLine->LiNext == NULL))))
		if (pLine->LiSpaceWidth == 0)
		   DecalLigne (pLine, pave, pBox, dx, frame);
		else
		  {
		     CompresLigne (pLine, dx, frame, dbl);
		     pLine->LiRealLength = nlcour;
		  }
	     /* Peut-on eviter la reevaluation du bloc de lignes ? */
	     else if (dx < 0)
	       {

		  /* Peut-on remonter le premier mot de la ligne courante ? */
		  if (pLine->LiPrevious != NULL)
		    {
		       /* Largeur restante */
		       lmax = pLine->LiPrevious->LiXMax - pLine->LiPrevious->LiRealLength - MAX_BLANC;
		       if (pLine->LiFirstPiece != NULL)
			  box1 = pLine->LiFirstPiece;
		       else
			  box1 = pLine->LiFirstBox;
		       if (box1->BxWidth > lmax)
			  if (box1->BxAbstractBox->AbLeafType == LtText && lmax > 0)
			    {
			       /* Elimine le cas des lignes sans blanc */
			       if (pLine->LiNSpaces == 0)
				  lmax = 1;	/* force la reevaluation */
			       else
				 {
				    nChars = box1->BxNChars;
				    /* regarde si on peut trouver une coupure */
 lmax = SearchBreak (pLine, box1, lmax, box1->BxFont, &nChars, &larg,
			 &nSpaces, &newIndex, &pNewBuff);
				 }

			       if (lmax > 0)
				  ReevalBloc (pave, pLine->LiPrevious, NULL, frame);
			    }
			  else
			     lmax = 0;
		    }
		  else
		     lmax = 0;

		  /* Peut-on remonter le premier mot de la ligne suivante ? */
		  if (lmax == 0)
		    {
		       if (pLine->LiNext != NULL)
			 {
			    lmax = pLine->LiXMax - pLine->LiRealLength - dx;
			    pLi2 = pLine->LiNext;
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
					 nChars = box1->BxNChars;
					 /* regarde si on peut trouver une coupure */
					 lmax = SearchBreak (pLi2, box1, lmax, box1->BxFont,
			 &nChars, &larg, &nSpaces, &newIndex, &pNewBuff);
				      }
				 }
			       else
				  lmax = 0;
			 }

		       if (lmax > 0)
			  ReevalBloc (pave, pLine, NULL, frame);
		       else if (pLine->LiSpaceWidth == 0)
			  DecalLigne (pLine, pave, pBox, dx, frame);
		       else
			 {
			    CompresLigne (pLine, dx, frame, dbl);
			    pLine->LiRealLength = nlcour;
			 }
		    }
		  else
		     ReevalBloc (pave, pLine, NULL, frame);
	       }
	     else
		ReevalBloc (pave, pLine, NULL, frame);
	  }
     }
}


/* ---------------------------------------------------------------------- */
/* |  EnglLigne assure l'englobement de la boite de frame dans la     | */
/* |         ligne du pave pAb et propage les modifications necessaires.| */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                EnglLigne (PtrBox pBox, int frame, PtrAbstractBox pAb)
#else  /* __STDC__ */
void                EnglLigne (pBox, frame, pAb)
PtrBox            pBox;
int               frame;
PtrAbstractBox    pAb;
#endif /* __STDC__ */
{
   PtrBox         box1;
   PtrBox         box2;
   int            sur, h;
   int            i, sous;
   int            pos;
   int            sousdiff;
   PtrLine        pLi1;
   PtrLine        pLi2;
   PtrBox         pBo1;

   /* AbDimension   *pDimAb; */

   pBo1 = pAb->AbBox;
   if (Propage != ToSiblings || pBo1->BxVertFlex)
     {
	pLi1 = DesLigne (pBox);
	/*pDimAb = &pAb->AbHeight; */

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
		  /* deplace la boite */
		  i = pBo1->BxYOrg + pLi1->LiYOrg - pBox->BxYOrg;
		  DepOrgY (pBox, NULL, i, frame);

		  /* decale les lignes suivantes */
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

		       /* met a jour la hauteur du bloc de lignes */
		       if (pBo1->BxContentHeight)
			  ChangeHtContenu (pBo1, pBo1, pBo1->BxHeight + sous, frame);
		    }
	       }
	     else
	       {
		  /* calcule la base et la hauteur sous-base de la ligne */
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
		       /* passe a la boite suivante */
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

		  /* place la boite pBox dans la ligne */

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

		  /* met a jour le bloc de ligne */
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

			    /* met a jour la base du bloc de lignes   */
			    /* s'il depend de la premiere boite englobee */
			    if (pAb->AbHorizRef.PosAbRef == pAb->AbFirstEnclosed)
			       DepBase (pBo1, NULL, i, frame);

			    /* aligne les boites dans la ligne */
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

		     pLi1->LiYOrg -= i;		/* deplace la ligne */


		  /* decale les lignes suivantes */
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

		  /* termine la mise a jour du bloc de lignes */
		  if (h != 0 && pBo1->BxContentHeight)
		     ChangeHtContenu (pBo1, pBo1, pBo1->BxHeight + h, frame);
	       }
	  }
     }
}
