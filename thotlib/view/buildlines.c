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
 * This module handles line constructions
 *
 * Author: I. Vatton (INRIA)
 *
 */

#include "ustring.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"

#define THOT_EXPORT extern
#include "boxes_tv.h"

#include "boxmoves_f.h"
#include "boxlocate_f.h"
#include "buildboxes_f.h"
#include "memory_f.h"
#include "boxselection_f.h"
#include "buildlines_f.h"
#include "font_f.h"
#include "frame_f.h"
#include "hyphen_f.h"

#define SPACE_VALUE_MIN  3
#define SPACE_VALUE_MAX  6
/* max number of consecutive hyphens */
#define MAX_SIBLING_HYPHENS 2


/*----------------------------------------------------------------------
   GetNextBox rend l'adresse de la boite associee au pave vivant qui  
   suit pAb.                                               
  ----------------------------------------------------------------------*/
#ifdef __STDC__
PtrBox              GetNextBox (PtrAbstractBox pAb)
#else  /* __STDC__ */
PtrBox              GetNextBox (pAb)
PtrAbstractBox      pAb;

#endif /* __STDC__ */
{
   PtrAbstractBox      pNextAb;
   ThotBool            loop;
   PtrBox              result;

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


/*----------------------------------------------------------------------
   GetPreviousBox rend l'adresse de la boite associee au pave vivant 
   qui precede pAb.                                        
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static PtrBox       GetPreviousBox (PtrAbstractBox pAb)
#else  /* __STDC__ */
static PtrBox       GetPreviousBox (pAb)
PtrAbstractBox      pAb;

#endif /* __STDC__ */
{
   PtrAbstractBox      pNextAb;
   ThotBool            loop;
   PtrBox              result;

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
	   while (!pNextAb->AbDead && pNextAb->AbBox->BxType == BoGhost)
	     {
	       if (pNextAb->AbFirstEnclosed->AbDead)
		 pNextAb->AbBox->BxType = BoComplete;
	       else
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


/*----------------------------------------------------------------------
   Adjust calcule la largeur des blancs a` afficher dans la ligne  
   pour l'ajuster. La procedure met a` jour les positions  
   en x et la largeur des boites incluses.                 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         Adjust (PtrBox pParentBox, PtrLine pLine, int frame, ThotBool orgXComplete, ThotBool orgYComplete)
#else  /* __STDC__ */
static void         Adjust (pParentBox, pLine, frame, orgXComplete, orgYComplete)
PtrBox              pParentBox;
PtrLine             pLine;
int                 frame;
ThotBool            orgXComplete;
ThotBool            orgYComplete;

#endif /* __STDC__ */
{
   PtrBox              pBox, pBoxInLine;
   int                 length, baseline;
   int                 nSpaces;

   nSpaces = 0;			/* nombre de blancs */
   length = 0;			/* taille des chaines de caracteres sans blanc */
   baseline = pLine->LiYOrg + pLine->LiHorizRef;

   /* Calcul de la longueur de la ligne en supprimant les blancs */
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

	if (!pBoxInLine->BxAbstractBox->AbNotInLine)
	  {
	    if (pBox->BxAbstractBox->AbLeafType == LtText)
	      {
		pBox->BxWidth -= pBox->BxNSpaces * CharacterWidth (_SPACE_, pBox->BxFont);
		nSpaces += pBox->BxNSpaces;
	      }

	    length += pBox->BxWidth;
	  }
	/* passe a la boite suivante */
	pBoxInLine = GetNextBox (pBox->BxAbstractBox);
     }
   while (pBox != pLine->LiLastBox && pBox != pLine->LiLastPiece);

   /* Calcul de la taille des blancs en pixels */
   if (pLine->LiXMax > length)
     {
	pLine->LiNPixels = pLine->LiXMax - length;
	if (nSpaces == 0)
	   pLine->LiSpaceWidth = 0;
	else
	   pLine->LiSpaceWidth = pLine->LiNPixels / nSpaces;
	pLine->LiNSpaces = nSpaces;
	/* Largeur minimum de ligne */
	pLine->LiMinLength = length + nSpaces * SPACE_VALUE_MIN;
	/* Pixels restants a repartir */
	pLine->LiNPixels -= pLine->LiSpaceWidth * nSpaces;
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

	if (!pBoxInLine->BxAbstractBox->AbNotInLine)
	  {
	    XMove (pBox, NULL, length - pBox->BxXOrg, frame);
	    YMove (pBox, NULL, baseline - pBox->BxHorizRef - pBox->BxYOrg, frame);
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
	  }
	/* passe a la boite suivante */
	pBoxInLine = GetNextBox (pBox->BxAbstractBox);
     }
   while (pBox != pLine->LiLastBox && pBox != pLine->LiLastPiece);
}


/*----------------------------------------------------------------------
   FloatToInt fait un arrondi float -> int.                        
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                 FloatToInt (float e)
#else  /* __STDC__ */
int                 FloatToInt (e)
float               e;

#endif /* __STDC__ */
{
   int                 value;

   if (e < 0.0)
      value = (int) (e - 0.5);
   else
      value = (int) (e + 0.5);
   return value;
}


/*----------------------------------------------------------------------
   Align aligne les bases des boites contenues dans la ligne       
   designee. Decale de delta l'origine de chaque boite     
   dans la ligne et indique que les boites ne sont pas     
   justifiees.                                             
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                Align (PtrBox pParentBox, PtrLine pLine, int delta, int frame, ThotBool orgXComplete, ThotBool orgYComplete)
#else  /* __STDC__ */
void                Align (pParentBox, pLine, delta, frame, orgXComplete, orgYComplete)
PtrBox              pParentBox;
PtrLine             pLine;
int                 delta;
int                 frame;
ThotBool            orgXComplete;
ThotBool            orgYComplete;

#endif /* __STDC__ */
{
   PtrBox              pBox, pBoxInLine;
   int                 baseline, x;

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

	if (!pBox->BxAbstractBox->AbNotInLine)
	  {
	    XMove (pBox, NULL, x - pBox->BxXOrg, frame);
	    YMove (pBox, NULL, baseline - pBox->BxHorizRef - pBox->BxYOrg, frame);
	    x += pBox->BxWidth;
	    pBoxInLine->BxSpaceWidth = 0;
	  }
	/* passe a la boite suivante */
	pBoxInLine = GetNextBox (pBox->BxAbstractBox);
     }
   while (pBox != pLine->LiLastBox && pBox != pLine->LiLastPiece);
}


/*----------------------------------------------------------------------
   SearchBreak calcule le nombre de caracte`res nChars a` placer dans 
   la premie`re boi^te coupe'e pour la limiter a` une      
   longueur de length en pixels.                           
   Retourne le nombre de caracte`res a` sauter, soit 1 ou  
   plus si la coupure tombe sur un blanc, -1 si le dernier 
   mot est coupe' et un tiret doit e^tre engendre', -2 si  
   le dernier mot est coupe' sans besoin de tiret, 0 si la 
   coupure ne correspond pas a` un point de coupure.       
   Rend la position a` laquelle le point de coupure peut   
   e^tre inse're' :                                        
   - le nombre de caracteres : nChars (blancs compris sauf 
   les derniers).                                          
   - la longueur de cette nouvelle sous-chaine :           
   boxWidth (avec les blancs).                           
   - le nombre de blancs qu'elle contient : nSpaces        
   (sauf les derniers).                                    
   - l'adresse du buffer du 1er caractere apres coupure.   
   - l'index dans ce buffer du 1er caractere apres coupure.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static int          SearchBreak (PtrLine pLine, PtrBox pBox, int length, ptrfont font, int *nChars, int *boxWidth, int *nSpaces, int *newIndex, PtrTextBuffer * pNewBuff)
#else  /* __STDC__ */
static int          SearchBreak (pLine, pBox, length, font, nChars, boxWidth, nSpaces, newIndex, pNewBuff)
PtrLine             pLine;
PtrBox              pBox;
int                 length;
ptrfont             font;
int                *nChars;
int                *boxWidth;
int                *nSpaces;
int                *newIndex;
PtrTextBuffer      *pNewBuff;

#endif /* __STDC__ */
{
   PtrLine             pPreviousLine;
   int                 i, count;
   int                 carWidth, newWidth;
   int                 width;
   int                 wordWidth, charIndex;
   int                 dummySpaces, spaceWidth;
   int                 spaceAdjust;
   int                 spaceCount;
   UCHAR_T               character;
   PtrTextBuffer       pBuffer;
   PtrBox              pParentBox;
   ThotBool            still;
   Language            language;

   /* Initialisations */
   dummySpaces = 0;
   character = 0;
   *pNewBuff = NULL;
   *newIndex = 1;
   *nChars = pBox->BxNChars;
   *boxWidth = pBox->BxWidth;
   *nSpaces = pBox->BxNSpaces;

   charIndex = pBox->BxFirstChar;	/* index dans le buffer */
   pBuffer = pBox->BxBuffer;
   /* newWidth est la largeur utilisee pour calculer la mise en ligne */
   /* largeur est la largeur reelle du texte                       */
   newWidth = 0;
   width = 0;
   carWidth = 0;
   wordWidth = 0;
   spaceCount = 0;
   still = TRUE;
   spaceWidth = CharacterWidth (_SPACE_, font);
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
	   spaceAdjust = FloatToInt ((float) (spaceAdjust * 7) / (float) (10));
     }

   i = 0;
   count = pBox->BxNChars;

   /* Recherche un point de coupure */
   while (still)
     {
	/* Largeur du caractere suivant */
	character = (UCHAR_T) (pBuffer->BuContent[charIndex - 1]);
	if (character == NUL)
	   carWidth = 0;
	else if (character == _SPACE_)
	   carWidth = spaceAdjust;
	else
	   carWidth = CharacterWidth (character, font);

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
		  *boxWidth = pBox->BxWidth;
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
	     else if (character == _SPACE_)
	       {
		  /* coupe sur le dernier blanc rencontre' */
		  dummySpaces = 1;
		  if (spaceCount == 0)
		     *nChars = wordWidth;	/* dernier blanc non compris */
		  else
		     (*nChars) += wordWidth;	/* dernier blanc non compris */
		  *boxWidth = width;
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
		  *boxWidth = width;
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
		*boxWidth = pBox->BxWidth;
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
	else if (character == _SPACE_)
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
	     *boxWidth = width;
	     wordWidth = 1;	/* prepare un nouveau mot */
	     i++;		/* nombre de caracteres traites */
	     charIndex++;	/* indice du prochain caractere */
	     newWidth += carWidth;
	     width += spaceWidth;
	  }
	/*  Si c'est un autre caractere */
	else
	  {
	     wordWidth++;
	     i++;		/* nombre de caracteres traites */
	     charIndex++;	/* indice du prochain caractere */
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
	if (character == _SPACE_)
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

	     *boxWidth -= spaceWidth;
	     dummySpaces++;
	     (*nChars)--;
	     (*nSpaces)--;
	  }
	else
	   still = FALSE;
     }

   if (length < pBox->BxWidth && CanHyphen (pBox) && wordWidth > 1 && !TextInserting)
     {
	/* Il faut verifier les coupures en fin de lignes precedentes */
	still = TRUE;
	if (pLine != NULL)
	  {
	     pPreviousLine = pLine->LiPrevious;
	     count = 0;
	     while (count < MAX_SIBLING_HYPHENS && pPreviousLine != NULL)
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

	     if (count == MAX_SIBLING_HYPHENS)
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
		  wordWidth = HyphenLastWord (font, language, &pBuffer, &charIndex, &width, &still);
		  /* Si la coupure a reussi */
		  if (wordWidth > 0)
		    {
		       /* met a jour le point de coupure */
		       *pNewBuff = pBuffer;
		       *newIndex = charIndex;
		       *boxWidth = 0;
		       *nChars = 0;
		    }
	       }
	     else
	       {
		  /* essaie de couper le mot qui suit le dernier blanc */
		  width = length - *boxWidth - dummySpaces * spaceAdjust;
		  wordWidth = HyphenLastWord (font, language, pNewBuff, newIndex, &width, &still);
	       }

	     if (wordWidth > 0)
	       {
		  /* Il faut mettre a jour le nombre de caracteres et */
		  /* la longueur de la premiere partie du texte coupe */
		  *nChars += dummySpaces + wordWidth;
		  *nSpaces += dummySpaces;
		  *boxWidth += width + dummySpaces * spaceWidth;
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
	*boxWidth += CharacterWidth (173, font);

	/* RemoveElement un ou plusieurs caracteres pour loger l'hyphen */
	while (*boxWidth > length && *nChars > 1)
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

	     *boxWidth -= CharacterWidth (character, font);
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
	if (character == _SPACE_)
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


/*----------------------------------------------------------------------
   Coupe la boite pBox sur un caractere de rupture de ligne.          
   width = la largeur du de'but de la boite jusqu'au point de coupure 
   breakWidth = largeur du caracte`re qui force la coupure            
   nCharssl = nombre de caracte`res avant la coupure                  
   nSpaces = nombre d'espaces avant le point de coupure               
   newIndex = indice du debut de texte apres le point de coupure      
   pNewBuff = buffer du debut de texte apres le point de coupure      
   pRootAb = pave racine de la vue (pour mise a jour des chainages)   
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         ManageBreakLine (PtrBox pBox, int width, int breakWidth, int nCharssl, int nSpaces, int newIndex, PtrTextBuffer pNewBuff, PtrAbstractBox pRootAb)
#else  /* __STDC__ */
static void         ManageBreakLine (pBox, width, breakWidth, nCharssl, nSpaces, newIndex, pNewBuff, pRootAb)
PtrBox              pBox;
int                 width;
int                 breakWidth;
int                 nCharssl;
int                 nSpaces;
int                 newIndex;
PtrTextBuffer       pNewBuff;
PtrAbstractBox      pRootAb;

#endif /* __STDC__ */
{
   PtrBox              ibox1, ibox2;
   PtrBox              pPreviousBox, pNextBox;
   PtrAbstractBox      pAb;
   ptrfont             font;
   int                 baseline;
   int                 height;

   if (nCharssl < 0)
     printf ("Error in ManageBreakLine\n");
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
	else if (pBox->BxType == BoSplit)
	  {
	    /* keep the last created piece of that box */
	    while (pBox->BxNexChild != NULL)
	      pBox = pBox->BxNexChild;
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
	ibox2->BxWidth = pBox->BxWidth - width - breakWidth;

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


/*----------------------------------------------------------------------
   BreakMainBox coupe la boi^te entiere d'indice pBox correspondant   
   a` l'image abstraite pAb en deux boi^tes. Si force est  
   Vrai coupe meme sur un caracte`re, sinon on ne coupe que
   sur un blanc. La premie`re boi^te issue de la coupure   
   contiendra nChars caracte`res. Sa longueur sera         
   infe'rieure a` maxLength. Le blanc est perdu et le reste
   mis dans la boi^te de coupure suivante. Les chai^nages  
   sont mis a` jour.                                       
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         BreakMainBox (PtrLine pLine, PtrBox pBox, int maxLength, PtrAbstractBox pRootAb, ThotBool force)
#else  /* __STDC__ */
static void         BreakMainBox (pLine, pBox, maxLength, pRootAb, force)
PtrLine             pLine;
PtrBox              pBox;
int                 maxLength;
PtrAbstractBox      pRootAb;
ThotBool            force;

#endif /* __STDC__ */
{
   PtrBox              pFirstBox, pRemainBox;
   int                 baseline, larg;
   int                 newIndex, height;
   int                 spaceWidth, lostPixels;
   int                 nSpaces, nChars;
   PtrBox              pPreviousBox, pNextBox;
   PtrTextBuffer       pNewBuff;
   PtrAbstractBox      pAb;
   ptrfont             font;

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
      spaceWidth = CharacterWidth (_SPACE_, font);

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
	pFirstBox = GetBox (pAb);
	pRemainBox = GetBox (pAb);
     }
   else
     {
	pFirstBox = NULL;
	pRemainBox = NULL;
     }

   /* Traitement particulier des coupures de mots sans genenration du tiret */
   if (lostPixels == -2)
      lostPixels = 0;

   if (pFirstBox != NULL && pRemainBox != NULL)
     {
	/* Initialise le contenu de cette nouvelle boite */
	pFirstBox->BxIndChar = 0;
	pFirstBox->BxContentWidth = TRUE;
	pFirstBox->BxContentHeight = TRUE;
	pFirstBox->BxFont = font;
	pFirstBox->BxUnderline = pBox->BxUnderline;
	pFirstBox->BxThickness = pBox->BxThickness;
	pFirstBox->BxHeight = height;
	pFirstBox->BxWidth = larg;
	pFirstBox->BxHorizRef = baseline;
	if (lostPixels == -1)
	  {
	     /* La boite doit etre completee par un tiret d'hyphenation */
	     pFirstBox->BxType = BoDotted;
	     larg -= CharacterWidth (173, font);
	     lostPixels = 0;	/* il n'y a pas de blanc dummySpacese */
	  }
	else
	   pFirstBox->BxType = BoPiece;
	pFirstBox->BxBuffer = pBox->BxBuffer;
	pFirstBox->BxNChars = nChars;
	pFirstBox->BxNSpaces = nSpaces;
	pFirstBox->BxFirstChar = pBox->BxFirstChar;

	/* Initialise le contenu de la deuxieme boite */
	pRemainBox->BxContentWidth = TRUE;
	pRemainBox->BxContentHeight = TRUE;
	pRemainBox->BxHeight = height;
	pRemainBox->BxFont = font;
	pRemainBox->BxUnderline = pBox->BxUnderline;
	pRemainBox->BxThickness = pBox->BxThickness;
	pRemainBox->BxHorizRef = baseline;
	pRemainBox->BxType = BoPiece;
	pRemainBox->BxBuffer = pNewBuff;
	pRemainBox->BxIndChar = nChars + lostPixels;
	pRemainBox->BxFirstChar = newIndex;
	pRemainBox->BxNChars = pBox->BxNChars - lostPixels - nChars;
	pRemainBox->BxNSpaces = pBox->BxNSpaces - lostPixels - nSpaces;
	pRemainBox->BxWidth = pBox->BxWidth - larg - lostPixels * spaceWidth;

	/* Si la boite est vide on annule les autres valeurs */
	if (pRemainBox->BxNChars == 0)
	  {
	     pRemainBox->BxWidth = 0;
	     pRemainBox->BxNSpaces = 0;
	  }

	/* Modifie le chainage des boites terminales */
	pFirstBox->BxPrevious = pPreviousBox;
	if (pPreviousBox != NULL)
	   pPreviousBox->BxNext = pFirstBox;
	else
	   pRootAb->AbBox->BxNext = pFirstBox;
	pFirstBox->BxNext = pRemainBox;
	pRemainBox->BxPrevious = pFirstBox;
	pFirstBox->BxNexChild = pRemainBox;
	pRemainBox->BxNexChild = NULL;
	pRemainBox->BxNext = pNextBox;
	if (pNextBox != NULL)
	   pNextBox->BxPrevious = pRemainBox;
	else
	   pRootAb->AbBox->BxPrevious = pRemainBox;

	/* Mise a jour de la boite englobante */
	pBox->BxType = BoSplit;
	pBox->BxNexChild = pFirstBox;
     }
}


/*----------------------------------------------------------------------
   Teste s'il y a un caractere de rupture de ligne dans la boite pBox 
   Si oui retourne la valeur Vrai, sinon la valeur FALSE.             
   Au retour :                                                        
   boxWidth = la largeur de la boite jusqu'au point de coupure        
   breakWidth = largeur du caracte`re qui force la coupure            
   boxLength = nombre de caracte`res avant la coupure                 
   nSpaces = nombre d'espaces avant le point de coupure               
   newIndex = indice du debut de texte apres le point de coupure      
   pNewBuff = buffer du debut de texte apres le point de coupure      
   pRootAb = pave racine de la vue (pour mise a jour des chainages)
   wordWidth = la largeur du plus grand mot de la boite.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static ThotBool     FindBreakLine (PtrBox pBox, int *boxWidth, int *breakWidth, int *boxLength, int *nSpaces, int *newIndex, PtrTextBuffer * pNewBuff, int *wordWidth)
#else  /* __STDC__ */
static ThotBool     FindBreakLine (pBox, boxWidth, breakWidth, boxLength, nSpaces, newIndex, pNewBuff, wordWidth)
PtrBox              pBox;
int                *boxWidth;
int                *breakWidth;
int                *boxLength;
int                *nSpaces;
int                *newIndex;
PtrTextBuffer      *pNewBuff;
int                *wordWidth;
#endif /* __STDC__ */
{
   PtrTextBuffer       pBuffer;
   ptrfont             font;
   UCHAR_T               character;
   int                 i, j, l;
   int                 nChars;
   int                 wWidth;
   ThotBool            found;

   found = FALSE;
   *boxWidth = 0;
   *boxLength = 0;
   *nSpaces = 0;
   *wordWidth = 0;
   wWidth = 0;
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
	}

   while (j <= nChars && !found && pBuffer != NULL)
     {
	character = (unsigned char) (pBuffer->BuContent[i - 1]);
	/* Est-ce un caractere de coupure forcee ? */
	if (character == BREAK_LINE || character ==  NEW_LINE)
	  {
	     found = TRUE;
	     *breakWidth = CharacterWidth (BREAK_LINE, font);
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
	     *boxLength = j - 1;
	  }
	else
	  {
	    /* Sinon on continue la recherche */
	     if (character == _SPACE_)
	       {
		 (*nSpaces)++;
		 *boxWidth += CharacterWidth (_SPACE_, font);
		 /* compare word widths */
		 if (*wordWidth < wWidth)
		   *wordWidth = wWidth;
		 wWidth = 0;
	       }
	     else
	       {
		 l = CharacterWidth (character, font);
		 *boxWidth += l;
		 wWidth += l;
	       }
	  }

	if (!found)
	  {
	    /* next buffer ? */
	    if (i >= pBuffer->BuLength)
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
     }

   if (*wordWidth < wWidth)
     /* compare word widths */
     *wordWidth = wWidth;
   return (found);
}


/*----------------------------------------------------------------------
   AddBoxInLine ajout une boite dans la ligne.                     
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         AddBoxInLine (PtrBox pBox, int *descent, int *ascent, PtrLine pLine)
#else  /* __STDC__ */
static void         AddBoxInLine (pBox, descent, ascent, pLine)
PtrBox              pBox;
int                *descent;
int                *ascent;
PtrLine             pLine;

#endif /* __STDC__ */
{
   pLine->LiRealLength += pBox->BxWidth;
   /* Calcule la hauteur courante de la ligne */
   if (*ascent < pBox->BxHorizRef)
      *ascent = pBox->BxHorizRef;
   if (*descent < pBox->BxHeight - pBox->BxHorizRef)
      *descent = pBox->BxHeight - pBox->BxHorizRef;
}


/*----------------------------------------------------------------------
   BreakPieceOfBox recoupe la boite pBox deja coupee pour l'image    
   abstraite pAb. La coupure a lieu meme s'il n'y a pas de 
   blanc (coupure forcee). La boite pBox contiendra nChars 
   caracteres.                                             
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         BreakPieceOfBox (PtrLine pLine, PtrBox pBox, int maxLength, PtrAbstractBox pRootAb)
#else  /* __STDC__ */
static void         BreakPieceOfBox (pLine, pBox, maxLength, pRootAb)
PtrLine             pLine;
PtrBox              pBox;
int                 maxLength;
PtrAbstractBox      pRootAb;

#endif /* __STDC__ */
{
   PtrBox              pRemainBox, pNextBox;
   int                 oldlg, spaceWidth;
   int                 nChars;
   int                 larg, lostPixels, pl;
   int                 newIndex, nSpaces;
   int                 height, baseline;
   PtrTextBuffer       pNewBuff;
   PtrAbstractBox      pAb;
   ptrfont             font;
   int                 oldWidth, oldnSpaces;

   if (pBox->BxNChars <= 1)
     return;
   pAb = pBox->BxAbstractBox;
   height = pBox->BxHeight;
   baseline = pBox->BxHorizRef;
   pNextBox = pBox->BxNext;
   pl = pBox->BxIndChar;
   oldlg = pBox->BxNChars;
   font = pBox->BxFont;
   oldWidth = pBox->BxWidth;
   oldnSpaces = pBox->BxNSpaces;

   /* Si coupure sur un blanc le blanc de coupure est perdu */
   lostPixels = SearchBreak (pLine, pBox, maxLength, font, &nChars, &larg, &nSpaces, &newIndex, &pNewBuff);
   if (lostPixels <= 0)
      spaceWidth = 0;
   else
      spaceWidth = CharacterWidth (_SPACE_, font);

   /* Creation de la boite suivante qui contient la fin de la chaine */
   /* - dans le cas general si SearchBreak a trouve' un point de coupure */
   /*   soit un blanc (lostPixels>0), soit un point d'hyphe'nation  avec */
   /*   ge'ne'ration d'un tiret (lostPixels=-1) ou sans (lostPixels=-2)  */
   /* - pour sauter les derniers blancs de fin de texte a` condition    */
   /*   qu'il reste au moins un blanc pour justifier le texte restant */
   if (pNewBuff != NULL
       && (lostPixels != 0 || nSpaces != 0 || oldnSpaces == 0)
       && (pBox->BxWidth != maxLength || lostPixels != pBox->BxNSpaces))
      pRemainBox = GetBox (pAb);
   else
      pRemainBox = NULL;


   /* Coupure du mot sans generation du tiret */
   if (lostPixels == -2)
      lostPixels = 0;

   /* Initialise le contenu de cette nouvelle boite */
   if (pRemainBox != NULL)
     {

	pBox->BxNexChild = pRemainBox;
	pBox->BxNext = pRemainBox;

	/* Mise a jour de la  boite recoupee */
	pBox->BxNChars = nChars;
	pBox->BxWidth = larg;
	pBox->BxNSpaces = nSpaces;
	if (lostPixels == -1)
	  {
	     /* La boite doit etre completee par un tiret d'hyphenation */
	     pBox->BxType = BoDotted;
	     larg -= CharacterWidth (173, font);
	     lostPixels = 0;	/* il n'y a pas de blanc dummySpacese */
	  }
	else
	   pBox->BxType = BoPiece;

	pRemainBox->BxContentWidth = TRUE;
	pRemainBox->BxContentHeight = TRUE;
	pRemainBox->BxHeight = height;
	pRemainBox->BxFont = font;
	pRemainBox->BxUnderline = pBox->BxUnderline;
	pRemainBox->BxThickness = pBox->BxThickness;
	pRemainBox->BxHorizRef = baseline;
	pRemainBox->BxType = BoPiece;
	pRemainBox->BxBuffer = pNewBuff;
	pRemainBox->BxNexChild = NULL;

	/* Modifie le chainage des boites terminales */
	pRemainBox->BxPrevious = pBox;
	pRemainBox->BxNext = pNextBox;
	if (pNextBox != NULL)
	   pNextBox->BxPrevious = pRemainBox;
	else
	   pRootAb->AbBox->BxPrevious = pRemainBox;

	/* Si lostPixels > 0 -> Il faut retirer les caracteres blanc de la boite */
	pRemainBox->BxIndChar = pl + lostPixels + nChars;
	pRemainBox->BxFirstChar = newIndex;
	pRemainBox->BxNChars = oldlg - lostPixels - nChars;
	pRemainBox->BxNSpaces = oldnSpaces - lostPixels - nSpaces;
	pRemainBox->BxWidth = oldWidth - larg - lostPixels * spaceWidth;

	/* Si la boite est vide on annule les autres valeurs */
	if (pRemainBox->BxNChars == 0)
	  {
	     pRemainBox->BxWidth = 0;
	     pRemainBox->BxNSpaces = 0;
	  }
     }
}


/*----------------------------------------------------------------------
  FillLine remplit la ligne dont l'adresse est passee en parametre pour
  l'image abstraite pRootAb.                 
  Le parametre notComplete determine si la paragraphe a ou n'a pas de suite
  dans une ligne suivante.
  On suppose LiFirstBox et LiFirstPiece pre'alablement remplis et
  calcule l'indice de la derniere boite qui peut entrer dans la ligne et
  on met a` jour le descripteur de ligne : LiLastBox et LiLastPiece
  (si coupee). Les positions des boites mises en lignes sont calculees
  par rapport a` la boite racine.

  Au retour :
  full = indique si la ligne est pleine ou non.
  adjust = indique si la ligne doit etre justifiee ou non.
  la largeur minimum de la ligne (element non secable ou plus long mot).
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static int          FillLine (PtrLine pLine, PtrAbstractBox pRootAb, ThotBool notComplete, ThotBool *full, ThotBool *adjust, ThotBool *breakLine)
#else  /* __STDC__ */
static int          FillLine (pLine, pRootAb, notComplete, full, adjust, breakLine)
PtrLine             pLine;
PtrAbstractBox      pRootAb;
ThotBool            notComplete;
ThotBool           *full;
ThotBool           *adjust;
ThotBool           *breakLine
#endif /* __STDC__ */
{
   PtrTextBuffer       pNewBuff;
   PtrBox              pBox;
   PtrBox              pNextBox;
   PtrBox              lastbox;
   int                 ascent;
   int                 descent;
   int                 width;
   int                 breakWidth;
   int                 boxLength;
   int                 nSpaces;
   int                 newIndex, wordWidth;
   int                 xi, maxX;
   int                 maxLength, minWidth;
   ThotBool            still;
   ThotBool            toCut;
   ThotBool            found;

   *full = TRUE;
   *adjust = TRUE;
   *breakLine = FALSE;
   toCut = FALSE;
   still = TRUE;
   maxX = pLine->LiXMax;
   minWidth = 0;
   wordWidth = 0;
   xi = 0;
   ascent = 0;
   descent = 0;
   /* la boite que l'on traite */
   pNextBox = pLine->LiFirstBox;
   pLine->LiLastPiece = NULL;
   found = FALSE;

   /* pNextBox est la boite courante que l'on traite    */
   /* pBox est la derniere boite mise dans la ligne */
   /* La premiere boite est la suite d'une boite deja coupee ? */
   if (pLine->LiFirstPiece != NULL)
     {
	/* La boite de coupure entre obligatoirement dans la ligne */
	pBox = pLine->LiFirstPiece;

	/* Est-ce que l'on rencontre une coupure force dans la boite ? */
	found = FindBreakLine (pBox, &width, &breakWidth, &boxLength, &nSpaces, &newIndex, &pNewBuff, &wordWidth);
	/* La coupure forcee arrive avant la fin de ligne */
	if (found && width + xi <= maxX)
	  {
	     *adjust = FALSE;
	     still = FALSE;
	     *breakLine = TRUE;
	     if (pBox->BxNChars > 1)
	       ManageBreakLine (pBox, width, breakWidth, boxLength, nSpaces, newIndex, pNewBuff, pRootAb);
	  }
	/* La boite entre dans toute entiere dans la ligne */
	else if (pBox->BxWidth + xi <= maxX)
	  {
	     pNextBox = GetNextBox (pBox->BxAbstractBox);
	     if (pNextBox == NULL)
	       {
		  *full = FALSE;
		  still = FALSE;
	       }
	  }
	/* Il faut couper une autre fois la boite */
	else
	  {
	     still = FALSE;
	     BreakPieceOfBox (pLine, pBox, maxX - xi, pRootAb);
	  }

	pBox = pLine->LiFirstPiece;
	xi += pBox->BxWidth;
     }
   else
      /* Il n'y a pas encore de boite dans la ligne */
      pBox = NULL;

   /* recherche la boite qu'il faut couper */
   while (still)
     {
       /* refuse de mettre en ligne une boite non englobee */
       if (!pNextBox->BxAbstractBox->AbHorizEnclosing)
	 {
	   *full = TRUE;
	   still = FALSE;
	   
	   /* Est-ce la premiere boite de la ligne ? */
	   if (pBox == NULL)
	     pLine->LiLastPiece = pLine->LiFirstPiece;
	   /* Est-ce que la boite precedente est coupee ? */
	   else if (pBox->BxType == BoPiece || pBox->BxType == BoDotted)
	     pLine->LiLastPiece = pLine->LiFirstPiece;
	 }
       else if (pNextBox->BxAbstractBox->AbNotInLine)
	 {
	   /* skip over the box */
	   pNextBox = GetNextBox (pNextBox->BxAbstractBox);
	   if (pNextBox == NULL)
	     {
	       *full = FALSE;
	       still = FALSE;
	     }
	 }
       else
	 {
	   /* Recherche si on doit forcer un return dans la boite */
	   if (pNextBox->BxAbstractBox->AbAcceptLineBreak)
	     found = FindBreakLine (pNextBox, &width, &breakWidth, &boxLength, &nSpaces, &newIndex, &pNewBuff, &wordWidth);
	   else
	     {
	       found = FALSE;
	       wordWidth = pNextBox->BxWidth;
	     }
	   
	   /* La boite produite par le return force' tient dans la ligne ? */
	   if (found && width + xi <= maxX)
	     {
	       still = FALSE;
	       *adjust = FALSE;
	       *breakLine = TRUE;
	       if (pNextBox->BxNChars == 1)
		 /* just the Break character in the box */
		 /* it's not necessary to cut it */
		 pBox = pNextBox;
	       else
		 {
		   ManageBreakLine (pNextBox, width, breakWidth, boxLength, nSpaces, newIndex, pNewBuff, pRootAb);
		   if (pNextBox->BxNexChild != NULL)
		     {
		       pBox = pNextBox->BxNexChild;
		       /* Est-ce la boite unique de la ligne ? */
		       if (pNextBox == pLine->LiFirstBox)
			 pLine->LiFirstPiece = pBox;
		     }
		 }
	     }
	   /* La boite tient dans la ligne ? */
	   else if (pNextBox->BxWidth + xi <= maxX)
	     {
	       /* La boite entre dans la ligne */
	       pBox = pNextBox;
	       xi += pNextBox->BxWidth;
	       pNextBox = GetNextBox (pNextBox->BxAbstractBox);
	       if (pNextBox == NULL)
		 {
		   *full = FALSE;
		   still = FALSE;
		 }
	     }
	   /* Il faut couper la boite ou une boite precedente */
	   else
	     {
	       toCut = TRUE;
	       still = FALSE;
	     }
	 }
       /* compare different word widths */
       if (minWidth < wordWidth)
	 minWidth = wordWidth;
       wordWidth = 0;
     }
   /* compare different word widths */
   if (minWidth < wordWidth)
     minWidth = wordWidth;
   wordWidth = 0;

   /* Traitement des coupures de boites */
   if (toCut)
     {
	/* va essayer de couper la boite pNextBox ou une boite precedente */
	maxLength = maxX - xi;

	/* Est-ce la boite unique de la ligne ? */
	if (pNextBox == pLine->LiFirstBox)
	   /* Si elle n'est pas secable -> on laisse deborder */
	   if (!pNextBox->BxAbstractBox->AbAcceptLineBreak
	       || pNextBox->BxAbstractBox->AbLeafType != LtText)
	     {
	       pBox = pNextBox;	/* derniere boite de la ligne */
	       /* look at whether next boxes are not in line */
	       lastbox = pBox;
	       still = (lastbox != NULL);
	       while (still)
		 {
		   lastbox = GetNextBox (lastbox->BxAbstractBox);
		   if (lastbox == NULL)
		     still = FALSE;
		   else if (!lastbox->BxAbstractBox->AbNotInLine)
		     still = FALSE;
		   else
		     pBox = lastbox;
		 }
	     }
	/* Sinon on coupe la boite texte en un point quelconque */
	   else
	     {
		/* coupure forcee */
		BreakMainBox (pLine, pNextBox, maxLength, pRootAb, TRUE);
		if (pNextBox->BxNexChild != NULL)
		  {
		     pBox = pNextBox->BxNexChild;
		     pLine->LiFirstPiece = pBox;
		  }
	     }
	/* Il y a au moins une autre boite dans la ligne */
	else
	  {
	     /* essaie de couper la boite texte secable */
	     if (pNextBox->BxAbstractBox->AbLeafType == LtText
		 && pNextBox->BxAbstractBox->AbAcceptLineBreak
	     /* avec au moins un blanc ou avec coupure de mot */
		 && (CanHyphen (pNextBox) || pNextBox->BxNSpaces != 0))
	       {
		  /* coupure sur blanc */
		  BreakMainBox (pLine, pNextBox, maxLength, pRootAb, FALSE);
		  if (pNextBox->BxNexChild != NULL)
		    {
		       /* a trouve le point de coupure */
		       pBox = pNextBox->BxNexChild;
		       toCut = FALSE;
		    }
	       }

	     lastbox = pNextBox;
	     /* Si la coupure a echoue' on recherche en arriere un point de coupure */
	     while (toCut)
	       {
		  /* essaye de couper la boite precedente */
		  if (pNextBox != NULL)
		     pNextBox = GetPreviousBox (pNextBox->BxAbstractBox);

		  /* on bute sur la premiere boite de la ligne ? */
		  if (pNextBox == pLine->LiFirstPiece)
		     toCut = FALSE;
		  else if (pNextBox == pLine->LiFirstBox)
		    {
		       toCut = FALSE;
		       if (pLine->LiFirstPiece != NULL)
			  pNextBox = pLine->LiFirstPiece;
		    }

		  if (pNextBox->BxAbstractBox->AbLeafType == LtText
		      && !pNextBox->BxAbstractBox->AbNotInLine
		      /* Si c'est une boite de texte secable */
		      && pNextBox->BxWidth != 0		/* visible */
		      && pNextBox->BxAbstractBox->AbAcceptLineBreak
		      /* avec au moins un blanc si on est en insertion */
		      && pNextBox->BxNSpaces != 0)

		     /* coupe sur le dernier blanc de la boite */
		     if (pNextBox->BxType == BoPiece)
		       {
			  BreakPieceOfBox (pLine, pNextBox, pNextBox->BxWidth - 1, pRootAb);
			  pBox = pNextBox;
		       }
		     else
		       {
			  BreakMainBox (pLine, pNextBox, pNextBox->BxWidth - 1, pRootAb, FALSE);
			  pBox = pNextBox->BxNexChild;
			  /* on a coupe la premiere boite de la ligne ? */
			  if (pNextBox == pLine->LiFirstBox && pLine->LiFirstPiece == NULL)
			     pLine->LiFirstPiece = pBox;
			  toCut = FALSE;
		       }
		  /* Si la coupure sur un blanc echoue on coupe la derniere boite de la ligne */
		  else if (!toCut)
		    {
		       /* Si la derniere boite est secable, force sa coupure */
		       if (lastbox->BxAbstractBox->AbAcceptLineBreak
			   && lastbox->BxAbstractBox->AbLeafType == LtText)
			 {
			    BreakMainBox (pLine, lastbox, maxLength, pRootAb, TRUE);	/* coupure forcee */
			    pBox = lastbox->BxNexChild;
			 }
		       /* Si la boite est seule dans la ligne, laisse deborder */
		       else if (lastbox == pLine->LiFirstBox)
			  pBox = lastbox;
		       /* sinon on coupe avant la derniere boite */
		       else
			 {
			    pBox = GetPreviousBox (lastbox->BxAbstractBox);
			    /* Si c'est la premiere boite de la ligne et que celle-ci ets coupee */
			    if (pBox == pLine->LiFirstBox && pLine->LiFirstPiece != NULL)
			       pBox = pLine->LiFirstPiece;
			 }
		    }
		  /* Sinon on continue la recherche en arriere */
		  else
		     pBox = pNextBox;
	       }
	  }
     }


   /* ajoute toutes les boites de la ligne */
   if (pLine->LiFirstPiece != NULL)
      pNextBox = pLine->LiFirstPiece;
   else
      pNextBox = pLine->LiFirstBox;

   still = TRUE;
   while (still)
     {
	if (pNextBox->BxType == BoSplit)
	   pNextBox = pNextBox->BxNexChild;
	if (!pNextBox->BxAbstractBox->AbNotInLine)
	  AddBoxInLine (pNextBox, &descent, &ascent, pLine);
	if (pNextBox == pBox)
	   still = FALSE;
	else
	   pNextBox = GetNextBox (pNextBox->BxAbstractBox);
     }

   /* termine le chainage */
   if (pBox->BxType == BoPiece || pBox->BxType == BoDotted)
     {
	pLine->LiLastPiece = pBox;
	pLine->LiLastBox = pBox->BxAbstractBox->AbBox;
     }
   else
      pLine->LiLastBox = pBox;

   /* teste s'il reste des boites a mettre en ligne */
   if (pBox->BxNexChild == NULL && GetNextBox (pBox->BxAbstractBox) == NULL)
      *full = FALSE;

   /* coupe les blancs en fin de ligne pleine */
   if ((notComplete || *full) && pBox->BxAbstractBox->AbLeafType == LtText && pBox->BxNSpaces != 0)
      if (pLine->LiLastPiece == NULL)
	{
	   maxLength = pBox->BxWidth;
	   /*coupure sur un caractere refusee */
	   BreakMainBox (pLine, pBox, maxLength, pRootAb, FALSE);
	   if (pBox->BxNexChild != NULL)
	     {
		/* remplace la boite entiere par la boite de coupure */
		pLine->LiRealLength = pLine->LiRealLength - maxLength + pBox->BxNexChild->BxWidth;
		pLine->LiLastPiece = pBox->BxNexChild;
	     }
	}
      else if (pLine->LiLastPiece->BxNexChild == NULL)
	{
	   pBox = pLine->LiLastPiece;
	   maxLength = pBox->BxWidth;
	   BreakPieceOfBox (pLine, pBox, maxLength, pRootAb);
	   /* met a jour la largeur de la ligne */
	   pLine->LiRealLength = pLine->LiRealLength - maxLength + pBox->BxWidth;
	}

   /* Calcule la hauteur et la base de la ligne */
   pLine->LiHeight = descent + ascent;
   pLine->LiHorizRef = ascent;
   return (minWidth);
}


/*----------------------------------------------------------------------
   RemoveAdjustement recalcule la largueur de boite apres suppression 
   de la justification. Met a jour les marques de selection
   que la boite soit justifiee ou non.                     
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         RemoveAdjustement (PtrBox pBox, int spaceWidth)
#else  /* __STDC__ */
static void         RemoveAdjustement (pBox, spaceWidth)
PtrBox              pBox;
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


/*----------------------------------------------------------------------
   RemoveBreaks libere les boites de coupure qui ont pu etre creees a`
   partir de la boite mere pBox.                           
   L'indicateur chgDF ou changeSelectEnd est bascule si la boite     
   referencee par la marque Debut ou Fin de Selection est  
   liberee.                                                
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         RemoveBreaks (PtrBox pBox, int frame, ThotBool *changeSelectBegin, ThotBool *changeSelectEnd)
#else  /* __STDC__ */
static void         RemoveBreaks (pBox, frame, changeSelectBegin, changeSelectEnd)
PtrBox              pBox;
int                 frame;
ThotBool           *changeSelectBegin;
ThotBool           *changeSelectEnd;

#endif /* __STDC__ */
{
   PtrBox              pFirstBox;
   PtrBox              pRemainBox;
   PtrBox              pNextBox;
   int                 x, width;
   int                 number;
   int                 lostPixels;
   int                 diff;
   PtrAbstractBox      pAb;
   ViewFrame          *pFrame;
   ViewSelection      *pViewSel;

   pFrame = &ViewFrameTable[frame - 1];
   if (pBox != NULL && pBox->BxAbstractBox != NULL)
     {
	pAb = pBox->BxAbstractBox;
	if (pAb != NULL && pAb->AbLeafType == LtText)
	  {
	     x = CharacterWidth (_SPACE_, pBox->BxFont);
	     /* met a jour les marques de selection */
	     if (pFrame->FrSelectionBegin.VsBox == pBox)
	       {
		  /* Box entiere */
		  pFrame->FrSelectionBegin.VsBox = pAb->AbBox;
		  *changeSelectBegin = TRUE;
	       }
	     if (pFrame->FrSelectionEnd.VsBox == pBox)
	       {
		  /* Box entiere */
		  pFrame->FrSelectionEnd.VsBox = pAb->AbBox;
		  *changeSelectEnd = TRUE;
	       }

	     /* va liberer des boites coupure */
	     if (pBox->BxType == BoComplete)
		RemoveAdjustement (pBox, x);
	     else
	       {
		  /* 1e boite de coupure liberee */
		  pRemainBox = pBox->BxNexChild;
		  pBox->BxNexChild = NULL;
		  if (pBox->BxType == BoSplit)
		    {
		       /* met a jour la boite coupee */
		       pBox->BxType = BoComplete;
		       pBox->BxPrevious = pRemainBox->BxPrevious;
		       /* transmet la position courante de la boite */
		       pBox->BxXOrg = pRemainBox->BxXOrg;
		       pBox->BxYOrg = pRemainBox->BxYOrg;
		       if (pBox->BxPrevious != NULL)
			  pBox->BxPrevious->BxNext = pBox;
		       else
			  pFrame->FrAbstractBox->AbBox->BxNext = pBox;

		       width = 0;
		       number = 0;
		       lostPixels = 0;
		    }
		  else
		    {
		       /* met a jour la derniere boite de coupure conservee */
		       RemoveAdjustement (pBox, x);
		       width = pBox->BxWidth;
		       /* retire l'espace reserve au tiret d'hyphenation */
		       if (pBox->BxType == BoDotted)
			 {
			    width -= CharacterWidth (173, pBox->BxFont);
			    pBox->BxType = BoPiece;
			 }
		       number = pBox->BxNSpaces;
		       lostPixels = pBox->BxIndChar + pBox->BxNChars;
		    }

		  /* Libere les boites de coupure */
		  if (pRemainBox != NULL)
		    {
		       do
			 {
			    pFirstBox = pRemainBox;
			    RemoveAdjustement (pFirstBox, x);
			    /* Nombre de blancs sautes */
			    diff = pFirstBox->BxIndChar - lostPixels;
			    if (diff > 0)
			      {
				 /* ajoute les blancs sautes */
				 width += diff * x;
				 number += diff;
			      }
			    else if (pFirstBox->BxType == BoDotted)
			       /* retire la largeur du tiret d'hyphenation */
			       width -= CharacterWidth (173, pFirstBox->BxFont);

			    /* Si la boite suivante n'est pas vide */
			    if (pFirstBox->BxNChars > 0)
			      {
				 number += pFirstBox->BxNSpaces;
				 width += pFirstBox->BxWidth;
			      }
			    lostPixels = pFirstBox->BxIndChar + pFirstBox->BxNChars;
			    pNextBox = pFirstBox->BxNext;
			    pRemainBox = FreeBox (pFirstBox);

			    /* Prepare la mise a jour des marques de selection */
			    pViewSel = &pFrame->FrSelectionBegin;
			    if (pViewSel->VsBox == pFirstBox)
			      {
				 /* Box entiere */
				 pViewSel->VsBox = pAb->AbBox;
				 *changeSelectBegin = TRUE;
			      }
			    pViewSel = &pFrame->FrSelectionEnd;
			    if (pViewSel->VsBox == pFirstBox)
			      {
				 /* Box entiere */
				 pViewSel->VsBox = pAb->AbBox;
				 *changeSelectEnd = TRUE;
			      }
			 }
		       while (pRemainBox != NULL);

		       /* Met a jour la boite de coupure */
		       if (pBox->BxType == BoPiece)
			 {
			    pBox->BxNChars = pBox->BxAbstractBox->AbBox->BxNChars - pBox->BxIndChar;
			    /* comptabilise les blancs ignores de fin de boite */
			    lostPixels = lostPixels - pBox->BxIndChar - pBox->BxNChars;
			    if (lostPixels > 0)
			      {
				 width += lostPixels * x;
				 number += lostPixels;
			      }
			    pBox->BxWidth = width;
			    pBox->BxNSpaces = number;
			 }

		       /* Termine la mise a jour des chainages */
		       pBox->BxNext = pNextBox;
		       if (pNextBox != NULL)
			  pNextBox->BxPrevious = pBox;
		       else
			  pFrame->FrAbstractBox->AbBox->BxPrevious = pBox;
		    }
	       }
	  }
	/* Pour les autres natures */
	else
	  {
	     if (pFrame->FrSelectionBegin.VsBox == pBox)
		*changeSelectBegin = TRUE;
	     if (pFrame->FrSelectionEnd.VsBox == pBox)
		*changeSelectEnd = TRUE;
	  }
     }
}


/*----------------------------------------------------------------------
   ComputeLines cre'e les lignes ne'cessaires pour contenir les     
   boi^tes filles de'ja` cre'e'es. Le parame`tre frame     
   de'signe la fenetree^tre concerne'e. Les boi^tes filles 
   sont positionne'es:                                     
   - par rapport a` la boi^te englobante si le             
   de'placement est relatif,                             
   - sinon par rapport a` la racine de l'image concrete.   
   Rend la hauteur calcule'e du bloc de lignes.            
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ComputeLines (PtrBox pBox, int frame, int *height)
#else  /* __STDC__ */
void                ComputeLines (pBox, frame, height)
PtrBox              pBox;
int                 frame;
int                *height;
#endif /* __STDC__ */
{
  PtrLine             pPreviousLine;
  PtrLine             pLine;
  PtrAbstractBox      pChildAb;
  PtrAbstractBox      pAb, pRootAb;
  PtrBox              pBoxToBreak;
  PtrBox              pNextBox;
  AbPosition         *pPosAb;
  int                 x, lineSpacing, indentLine;
  int                 org, width, noWrappedWidth;
  int                 lostPixels, minWidth;
  ThotBool            toAdjust;
  ThotBool            breakLine;
  ThotBool            orgXComplete;
  ThotBool            orgYComplete;
  ThotBool            extensibleBox;
  ThotBool            full;
  ThotBool            still;
  ThotBool            toLineSpace;

  extensibleBox = pBox->BxContentWidth;
  /* Remplissage de la boite bloc de ligne */
  x = 0;
  noWrappedWidth = 0;
  pAb = pBox->BxAbstractBox;
  pNextBox = NULL;
  full = TRUE;
  pRootAb = ViewFrameTable[frame - 1].FrAbstractBox;
  /* evalue si le positionnement en X et en Y doit etre absolu */
  IsXYPosComplete (pBox, &orgXComplete, &orgYComplete);
  /* Construction complete du bloc de ligne */
  if (pBox->BxFirstLine == NULL)
    {
      /* reset the value of the width without wrapping */
      pBox->BxMaxWidth = 0;
      pBox->BxMinWidth = 0;
      /* recherche la premiere boite mise en ligne */
      pChildAb = pAb->AbFirstEnclosed;
      still = (pChildAb != NULL);
      pNextBox = NULL;
      while (still)
	if (pChildAb == NULL)
	  still = FALSE;
	/* Est-ce que le pave est mort ? */
	else if (pChildAb->AbDead || pChildAb->AbNotInLine)
	  pChildAb = pChildAb->AbNext;
	else if (pChildAb->AbBox->BxType == BoGhost)
	  /* descend la hierarchie */
	  pChildAb = pChildAb->AbFirstEnclosed;
	else
	  {
	    /* Sinon c'est la boite du pave */
	    pNextBox = pChildAb->AbBox;
	    still = FALSE;
	  }
    }

  width = pBox->BxWidth;
  if (width > CharacterWidth (119, pBox->BxFont)/*'w' */ || extensibleBox)
    {
      /* Calcul de l'interligne */
      lineSpacing = PixelValue (pAb->AbLineSpacing, pAb->AbLineSpacingUnit, pAb, ViewFrameTable[frame - 1].FrMagnification);
      /* Calcul de l'indentation */
      if (extensibleBox)
	{
	  indentLine = 0;
	  width = 3000;
	}
      else if (pAb->AbIndentUnit == UnPercent)
	indentLine = PixelValue (pAb->AbIndent, UnPercent, (PtrAbstractBox) width, 0);
      else
	indentLine = PixelValue (pAb->AbIndent, pAb->AbIndentUnit, pAb, ViewFrameTable[frame - 1].FrMagnification);
      if (pAb->AbIndent < 0)
	indentLine = -indentLine;
      
      if (pBox->BxFirstLine == NULL)
	{
	  if (pNextBox == NULL)
	    /* Rien a mettre en ligne */
	    full = FALSE;
	  else
	    {
	      GetLine (&pLine);
	      pBox->BxFirstLine = pLine;
	    }
	  
	  pBoxToBreak = NULL;
	  pPreviousLine = NULL;
	  /* hauteur de la boite bloc de lignes */
	  *height = 0;
	  /* origine de la nouvelle ligne */
	  org = 0;
	  toLineSpace = FALSE;
	}
      else
	{
	  /* Reconstruction partielle du bloc de ligne */
	  pPreviousLine = pBox->BxLastLine;
	  pChildAb = pPreviousLine->LiLastBox->BxAbstractBox;
	  pBoxToBreak = pPreviousLine->LiLastPiece;
	  /* hauteur boite bloc de lignes */
	  *height = pPreviousLine->LiYOrg + pPreviousLine->LiHeight;
	  /* Origine de la future ligne */
	  if (pChildAb->AbHorizEnclosing && !pChildAb->AbNotInLine)
	    {
	      /* C'est une boite englobee */
	      org = pPreviousLine->LiYOrg + pPreviousLine->LiHorizRef + lineSpacing;
	      /* utilise l'interligne */
	      toLineSpace = TRUE;
	    }
	  else
	    {
	      /* La boite n'est pas englobee (Page) */
	      /* colle la nouvelle ligne en dessous de celle-ci */
	      org = *height;
	      toLineSpace = FALSE;
	    }

	  if (pBoxToBreak != NULL)
	    {
	      pBoxToBreak = pBoxToBreak->BxNexChild;
	      /* Cas particulier de la derniere boite coupee */
	      if (pBoxToBreak != NULL)
		{
		  pNextBox = pPreviousLine->LiLastBox;
		  /* is it empty ? */
		  if (pBoxToBreak->BxNChars == 0)
		    do
		      pNextBox = GetNextBox (pNextBox->BxAbstractBox);
		    while (pNextBox != NULL && pNextBox->BxAbstractBox->AbNotInLine);
		  }
	    }
	  
	  if (pNextBox == NULL)
	    pNextBox = GetNextBox (pChildAb);
	  
	  if (pNextBox == NULL)
	    full = FALSE;	/* Rien a mettre en ligne */
	  /* prepare la nouvelle ligne */
	  else
	    {
	      GetLine (&pLine);
	      pPreviousLine->LiNext = pLine;
	      /* Si la 1ere boite nouvellement mise en lignes n'est pas englobee */
	      if (!pNextBox->BxAbstractBox->AbHorizEnclosing
		  && !pNextBox->BxAbstractBox->AbNotInLine)
		{
		  org = *height;
		  /* colle la nouvelle ligne en dessous */
		  toLineSpace = FALSE;
		}
	    }
	}

      /* Insert new lines as long as they are full */
      /* ----------------------------------------- */
      while (full)
	{
	  /* Initialize the new line */
	  pLine->LiPrevious = pPreviousLine;
	  /* regarde si la boite deborde des lignes */
	  if (!pNextBox->BxAbstractBox->AbHorizEnclosing)
	    {
	      pLine->LiXOrg = pNextBox->BxXOrg;
	      /* Colle la boite en dessous de la precedente */
	      pLine->LiYOrg = *height;
	      if (extensibleBox)
		/* no limit for an extensible line */
		pLine->LiXMax = 3000;
	      else
		pLine->LiXMax = pNextBox->BxWidth;
	      pLine->LiHeight = pNextBox->BxHeight;
	      pLine->LiFirstBox = pNextBox;
	      pLine->LiLastBox = pNextBox;
	      pLine->LiFirstPiece = NULL;
	      pLine->LiLastPiece = NULL;
	      if (Propagate != ToSiblings || pBox->BxVertFlex)
		org = pBox->BxYOrg + *height;
	      else
		org = *height;
	      YMove (pNextBox, NULL, org - pNextBox->BxYOrg, frame);
	      *height += pLine->LiHeight;
	      org = *height;
	      toLineSpace = FALSE;
	      pBoxToBreak = NULL;
	    }
	  else if (!pNextBox->BxAbstractBox->AbNotInLine)
	    {
	      /* Indentation des lignes */
	      if (pPreviousLine != NULL || pAb->AbTruncatedHead)
		{
		  if (pAb->AbIndent < 0)
		    pLine->LiXOrg = indentLine;
		}
	      else if (pAb->AbIndent > 0)
		pLine->LiXOrg = indentLine;
	      if (pLine->LiXOrg >= width)
		pLine->LiXOrg = 0;
	      pLine->LiXMax = width - pLine->LiXOrg;
	      pLine->LiFirstBox = pNextBox;
	      /* ou  NULL si la boite est entiere */
	      pLine->LiFirstPiece = pBoxToBreak;

	      /* Remplissage de la ligne au maximum */
	      minWidth = FillLine (pLine, pRootAb, pAb->AbTruncatedTail, &full, &toAdjust, &breakLine);
	      if (pBox->BxMinWidth < minWidth)
		pBox->BxMinWidth = minWidth;
	      /* Positionnement de la ligne en respectant l'interligne */
	      if (toLineSpace)
		org -= pLine->LiHorizRef;
	      /* verifie que les lignes ne se chevauchent pas */
	      if (org < *height)
		org = *height;

	      pLine->LiYOrg = org;
	      *height = org + pLine->LiHeight;
	      org = org + pLine->LiHorizRef + lineSpacing;
	      toLineSpace = TRUE;

	      /* is there a breaked box ? */
	      if (pLine->LiLastPiece != NULL)
		pBoxToBreak = pLine->LiLastPiece->BxNexChild;
	      else
		pBoxToBreak = NULL;
	      /* Update the no wrapped width of the block */
	      noWrappedWidth += pLine->LiRealLength;
	      if (!breakLine && pBoxToBreak != NULL)
		{
		  /* take undisplayed spaces into account */
		  lostPixels = pBoxToBreak->BxIndChar - pLine->LiLastPiece->BxNChars - pLine->LiLastPiece->BxIndChar;
		  if (lostPixels != 0)
		    {
		      lostPixels = lostPixels * CharacterWidth (_SPACE_, pBoxToBreak->BxFont);
		      noWrappedWidth += lostPixels;
		    }
		  if (pLine->LiLastPiece->BxType == BoDotted)
		    /* remove the dash width */
		    noWrappedWidth -= CharacterWidth (173, pBoxToBreak->BxFont);
		}
	      if (breakLine || !full)
		{
		  if (noWrappedWidth > pBox->BxMaxWidth)
		    pBox->BxMaxWidth = noWrappedWidth;
		  noWrappedWidth = 0;
		}

	      /* Teste le cadrage des lignes */
	      if (toAdjust && (full || pAb->AbTruncatedTail) && pAb->AbJustify)
		Adjust (pBox, pLine, frame, orgXComplete, orgYComplete);
	      else
		{
		  x = pLine->LiXOrg;
		  if (pAb->AbAdjust == AlignCenter)
		    x += (pLine->LiXMax - pLine->LiRealLength) / 2;
		  else if (pAb->AbAdjust == AlignRight)
		    x = x + pLine->LiXMax - pLine->LiRealLength;
		  /* Decale toutes les boites de la ligne */
		  Align (pBox, pLine, x, frame, orgXComplete, orgYComplete);
		}
	    }

	  if (pLine->LiLastBox != NULL)
	    {
	      pNextBox = pLine->LiLastBox;
	      do
		pNextBox = GetNextBox (pNextBox->BxAbstractBox);
	      while (pNextBox != NULL && pNextBox->BxAbstractBox->AbNotInLine);
	    }
	  else
	    pNextBox = NULL;
	  
	  /* is there a breaked box */
	  if (pBoxToBreak != NULL)
	    /* is it empty ? */
	    if (pBoxToBreak->BxNChars > 0)
	      pNextBox = pLine->LiLastBox;
	    else if (pNextBox == NULL)
	      pNextBox = pLine->LiLastBox;

	  /* Est-ce la derniere ligne du bloc de ligne ? */
	  if (full)
	    {
	      if (pNextBox == NULL)
		{
		  /* Il n'y a plus de boite a traiter */
		  full = FALSE;
		  pBox->BxLastLine = pLine;
		}
	      else
		{
		  /* prepare la ligne suivante */
		  pPreviousLine = pLine;
		  GetLine (&pLine);
		  pPreviousLine->LiNext = pLine;
		}
	    }
	  else
	    {
	      pBox->BxLastLine = pLine;
	      /* Note la largeur de la fin de bloc si le remplissage est demande */
	      if (pAb->AbAdjust == AlignLeftDots)
		{
		  if (pLine->LiLastPiece != NULL)
		    pNextBox = pLine->LiLastPiece;
		  else
		    {
		      pNextBox = pLine->LiLastBox;
		      /* regarde si la derniere boite est invisible */
		      while (pNextBox->BxWidth == 0 && pNextBox != pLine->LiFirstBox && pNextBox != pLine->LiFirstPiece)
			pNextBox = pNextBox->BxPrevious;
		    }
		  pNextBox->BxEndOfBloc = pBox->BxXOrg + width - pNextBox->BxXOrg - pNextBox->BxWidth;
		}
	    }
	}

      if (noWrappedWidth > pBox->BxMaxWidth)
	pBox->BxMaxWidth = noWrappedWidth;
      noWrappedWidth = 0;
      /* met a jour la base du bloc de lignes   */
      /* s'il depend de la premiere boite englobee */
      if (pAb->AbHorizRef.PosAbRef == pAb->AbFirstEnclosed && pBox->BxFirstLine != NULL)
	{
	  pPosAb = &pAb->AbHorizRef;
	  x = PixelValue (pPosAb->PosDistance, pPosAb->PosUnit, pAb, ViewFrameTable[frame - 1].FrMagnification);
	  MoveHorizRef (pBox, NULL, pBox->BxFirstLine->LiHorizRef + x - pBox->BxHorizRef, frame);
	}
    }
  else
    {
      *height = FontHeight (pBox->BxFont);
      if (pNextBox != NULL && !pNextBox->BxAbstractBox->AbHorizEnclosing)
	do
	  pNextBox = GetNextBox (pNextBox->BxAbstractBox);
	while (pNextBox != NULL && pNextBox->BxAbstractBox->AbNotInLine);
      if (pNextBox != NULL)
	{
	  /* compute minimum and maximum width of the paragraph */
	  GetLine (&pLine);
	  /* no limit as an extensible line */
	  pLine->LiXMax = 3000;
	  pLine->LiFirstBox = pNextBox;
	  pLine->LiFirstPiece = NULL;
	  pLine->LiLastPiece = NULL;
	  pBox->BxMinWidth = FillLine (pLine, pRootAb, pAb->AbTruncatedTail, &full, &toAdjust, &breakLine);
	  if (full)
	    {
	      /* the last box has been broken */
	      full = FALSE;
	      still = FALSE;
	    RemoveBreaks (pLine->LiLastBox, frame, &full, &still);
	    }
	  pBox->BxMaxWidth = pLine->LiRealLength;
	  if (pLine->LiHeight > *height)
	    *height = pLine->LiHeight;
	  FreeLine (pLine);
	}
    }
}


/*----------------------------------------------------------------------
   ShiftLine decale de x les boites de la ligne de pBox incluse    
   dans le boc de ligne pAb et la ligne pLine.             
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         ShiftLine (PtrLine pLine, PtrAbstractBox pAb, PtrBox pBox, int x, int frame)
#else  /* __STDC__ */
static void         ShiftLine (pLine, pAb, pBox, x, frame)
PtrLine             pLine;
PtrAbstractBox      pAb;
PtrBox              pBox;
int                 x;
int                 frame;
#endif /* __STDC__ */
{
   PtrBox              pFirstBox, pSaveBox;
   PtrBox              pLastBox;
   ViewFrame          *pFrame;
   int                 xd, xf;
   int                 yd, yf;
   int                 i;
   ThotBool            status;

   pLastBox = NULL;
   pSaveBox = pBox;
   pLine->LiRealLength += x;
   /* prepare le reaffichage de la ligne */
   status = ReadyToDisplay;
   ReadyToDisplay = FALSE;
   /* Bornes de reaffichage a priori */
   pFrame = &ViewFrameTable[frame - 1];
   xd = pFrame->FrClipXBegin;
   xf = pFrame->FrClipXEnd;
   yd = pSaveBox->BxYOrg + pSaveBox->BxHorizRef - pLine->LiHorizRef;
   yf = yd + pLine->LiHeight;

   /* reaffiche la fin de la ligne */
   if (pAb->AbAdjust == AlignLeft
       || pAb->AbAdjust == AlignLeftDots
   /* on force le cadrage a gauche si le bloc de lignes prend la largeur */
   /* du contenu */
       || pAb->AbBox->BxContentWidth)
     {
	if (pLine->LiLastPiece == NULL)
	   pLastBox = pLine->LiLastBox;
	else
	   pLastBox = pLine->LiLastPiece;
	if (pLastBox != pBox)
	  {
	     xf = pLastBox->BxXOrg + pLastBox->BxWidth;
	     if (x > 0)
		xf += x;
	  }

	/* Note la largeur de la fin de bloc si le remplissage est demande! */

	if (pAb->AbAdjust == AlignLeftDots && pLine->LiNext == NULL)
	   pLastBox->BxEndOfBloc -= x;
     }
   /* reaffiche toute la ligne */
   else if (pAb->AbAdjust == AlignCenter)
     {
	i = x;
	x = (pLine->LiXMax - pLine->LiRealLength) / 2 - (pLine->LiXMax + i - pLine->LiRealLength) / 2;
	if (pLine->LiFirstPiece == NULL)
	   pFirstBox = pLine->LiFirstBox;
	else
	   pFirstBox = pLine->LiFirstPiece;
	xd = pFirstBox->BxXOrg;
	if (x < 0)
	   xd += x;
	if (pLine->LiLastPiece == NULL)
	   pLastBox = pLine->LiLastBox;
	else
	   pLastBox = pLine->LiLastPiece;
	if (pLastBox != pBox)
	   xf = pLastBox->BxXOrg + pLastBox->BxWidth;
	if (x < 0)
	   xf -= x;

	/* decale les boites precedentes */
	XMove (pFirstBox, NULL, x, frame);
	while (pFirstBox != pBox)
	  {
	     pFirstBox = GetNextBox (pFirstBox->BxAbstractBox);
	     if (pFirstBox->BxType == BoSplit)
		pFirstBox = pFirstBox->BxNexChild;
	     XMove (pFirstBox, NULL, x, frame);
	  }
	/* decale les boites suivantes */
	x = -x;
     }
   /* reaffiche le debut de la ligne */
   else if (pAb->AbAdjust == AlignRight)
     {
	x = -x;
	pLastBox = pBox;
	if (pLine->LiFirstPiece == NULL)
	   pBox = pLine->LiFirstBox;
	else
	   pBox = pLine->LiFirstPiece;
	xd = pBox->BxXOrg;
	if (x < 0)
	   xd += x;
	XMove (pBox, NULL, x, frame);
     }

   /* decale les boites suivantes */
   while (pBox != pLastBox && pBox != NULL)
     {
	pBox = GetNextBox (pBox->BxAbstractBox);
	if (pBox != NULL && pBox->BxNexChild != NULL && pBox->BxType == BoSplit)
	  pBox = pBox->BxNexChild;
	if (pBox != NULL && !pBox->BxAbstractBox->AbNotInLine)
	  XMove (pBox, NULL, x, frame);
     }
   DefClip (frame, xd, yd, xf, yf);
   ReadyToDisplay = status;
}


/*----------------------------------------------------------------------
   CompressLine compresse ou e'tend la ligne justifiee suite a`    
   l'ajout d'espaces et un ecart de xDelta pixels.         
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         CompressLine (PtrLine pLine, int xDelta, int frame, int spaceDelta)
#else  /* __STDC__ */
static void         CompressLine (pLine, xDelta, frame, spaceDelta)
PtrLine             pLine;
int                 xDelta;
int                 frame;
int                 spaceDelta;

#endif /* __STDC__ */
{
   int                 nSpaces;
   int                 length, opixel;
   int                 spaceValue, remainder;
   PtrBox              pFirstBox, pBox;
   ViewSelection      *pViewSel;
   ThotBool            status;

   remainder = pLine->LiNPixels - xDelta;
   spaceValue = 0;
   nSpaces = pLine->LiNSpaces;
   status = ReadyToDisplay;
   ReadyToDisplay = FALSE;

   /* Faut-il changer la largeur des blancs ? */
   if (xDelta > 0)
      while (remainder < 0)
	{
	   remainder += nSpaces;
	   spaceValue++;
	}
   else
      while (remainder >= nSpaces)
	{
	   remainder -= nSpaces;
	   spaceValue--;
	}

   /* met a jour la ligne */
   pLine->LiNPixels = remainder;
   /* pour chaque blanc insere ou retire on compte la largeur minimale */
   xDelta -= spaceDelta * (pLine->LiSpaceWidth - SPACE_VALUE_MIN);
   pLine->LiMinLength += xDelta;
   pLine->LiSpaceWidth -= spaceValue;

   /* prepare le reaffichage de toute la ligne */
   if (pLine->LiFirstPiece != NULL)
      pBox = pLine->LiFirstPiece;
   else
      pBox = pLine->LiFirstBox;
   length = pBox->BxXOrg;
   DefClip (frame, length, pBox->BxYOrg + pBox->BxHorizRef - pLine->LiHorizRef,
	    length + pLine->LiXMax,
     pBox->BxYOrg + pBox->BxHorizRef - pLine->LiHorizRef + pLine->LiHeight);

   /* met a jour chaque boite */
   do
     {
	if (pBox->BxType == BoSplit)
	   pFirstBox = pBox->BxNexChild;
	else
	   pFirstBox = pBox;

	if (!pBox->BxAbstractBox->AbNotInLine)
	  {
	    XMove (pFirstBox, NULL, length - pFirstBox->BxXOrg, frame);
	    if (pFirstBox->BxAbstractBox->AbLeafType == LtText && pFirstBox->BxNChars != 0)
	      {
		pFirstBox->BxWidth -= pFirstBox->BxNSpaces * spaceValue;
		pFirstBox->BxSpaceWidth = pLine->LiSpaceWidth;
		
		/* Repartition des pixels */
		opixel = pFirstBox->BxNPixels;
		if (remainder > pFirstBox->BxNSpaces)
		  pFirstBox->BxNPixels = pFirstBox->BxNSpaces;
		else
		  pFirstBox->BxNPixels = remainder;
		pFirstBox->BxWidth = pFirstBox->BxWidth - opixel + pFirstBox->BxNPixels;
		remainder -= pFirstBox->BxNPixels;
		
		/* Faut-il mettre a jour les marques de selection ? */
		pViewSel = &ViewFrameTable[frame - 1].FrSelectionBegin;
		if (pViewSel->VsBox == pFirstBox)
		  {
		    pViewSel->VsXPos -= pViewSel->VsNSpaces * spaceValue;
		    if (opixel < pViewSel->VsNSpaces)
		      pViewSel->VsXPos -= opixel;
		    else
		      pViewSel->VsXPos -= pViewSel->VsNSpaces;
		    if (pFirstBox->BxNPixels < pViewSel->VsNSpaces)
		      pViewSel->VsXPos += pFirstBox->BxNPixels;
		    else
		      pViewSel->VsXPos += pViewSel->VsNSpaces;
		  }

		pViewSel = &ViewFrameTable[frame - 1].FrSelectionEnd;
		if (pViewSel->VsBox == pFirstBox)
		  {
		    pViewSel->VsXPos -= pViewSel->VsNSpaces * spaceValue;
		    if (opixel < pViewSel->VsNSpaces)
		      pViewSel->VsXPos -= opixel;
		    else
		      pViewSel->VsXPos -= pViewSel->VsNSpaces;
		    if (pFirstBox->BxNPixels < pViewSel->VsNSpaces)
		      pViewSel->VsXPos += pFirstBox->BxNPixels;
		    else
		      pViewSel->VsXPos += pViewSel->VsNSpaces;
		    if (pViewSel->VsIndBox < pFirstBox->BxNChars
			&& pViewSel->VsBuffer->BuContent[pViewSel->VsIndBuf - 1] == SPACE)
		      pViewSel->VsXPos -= spaceValue;
		  }
	      }
	    length += pFirstBox->BxWidth;
	  }
	pBox = GetNextBox (pFirstBox->BxAbstractBox);
     }
   while (pFirstBox != pLine->LiLastBox && pFirstBox != pLine->LiLastPiece);
   ReadyToDisplay = status;
}


/*----------------------------------------------------------------------
   RemoveLines libere les lignes acquises pour l'affichage du bloc de 
   lignes a` partir et y compris la ligne passee en        
   parametre. Libere toutes les boites coupees creees pour 
   ces lignes.						
   		changeSelectBegin et changeSelectEnd sont bascules si   
   la boite referencee par la marque Debut ou Fin de       
   Selection est liberee.                                  
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                RemoveLines (PtrBox pBox, int frame, PtrLine pFirstLine, ThotBool * changeSelectBegin, ThotBool * changeSelectEnd)
#else  /* __STDC__ */
void                RemoveLines (pBox, frame, pFirstLine, changeSelectBegin, changeSelectEnd)
PtrBox              pBox;
int                 frame;
PtrLine             pFirstLine;
ThotBool           *changeSelectBegin;
ThotBool           *changeSelectEnd;
#endif /* __STDC__ */
{
  PtrBox              pFirstBox;
  PtrLine             pNextLine;
  PtrLine             pLine;
  PtrAbstractBox      pAb;

  *changeSelectBegin = FALSE;
  *changeSelectEnd = FALSE;

  pLine = pFirstLine;
  if (pLine != NULL && pBox->BxType == BoBlock)
    {
      pFirstBox = NULL;
      /* Mise a jour du chainage des lignes */
      if (pLine->LiPrevious != NULL)
	{
	  pFirstBox = pLine->LiFirstBox;
	  pLine->LiPrevious->LiNext = NULL;
	  /* Est-ce que la premiere boite est une boite suite ? */
	  if (pLine->LiPrevious->LiLastBox == pFirstBox)
	    RemoveBreaks (pLine->LiFirstPiece, frame, changeSelectBegin, changeSelectEnd);
	  else
	    RemoveBreaks (pFirstBox, frame, changeSelectBegin, changeSelectEnd);
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
	  pNextLine = pLine->LiNext;
	  FreeLine (pLine);
	  pLine = pNextLine;
	}
      
      /* Liberation des boites de coupure */
      if (pFirstBox != NULL)
	pFirstBox = GetNextBox (pFirstBox->BxAbstractBox);
      else
	{
	  /* recherche la premiere boite mise en ligne */
	  pAb = pBox->BxAbstractBox->AbFirstEnclosed;
	  while (pFirstBox == NULL && pAb != NULL)
	    if (pAb->AbBox == NULL)
	      /* la boite a deja ete detruite */
	      pAb = pAb->AbNext;
	    else if (pAb->AbBox->BxType == BoGhost)
	      /* c'est un pave fantome -> descend la hierarchie */
	      pAb = pAb->AbFirstEnclosed;
	    else
	      /* Sinon c'est la boite du pave */
	      pFirstBox = pAb->AbBox;
	}
      
      while (pFirstBox != NULL)
	{
	  RemoveBreaks (pFirstBox, frame, changeSelectBegin, changeSelectEnd);
	  pFirstBox = GetNextBox (pFirstBox->BxAbstractBox);
	}
    }
}


/*----------------------------------------------------------------------
   RecomputeLines reevalue le bloc de ligne pAb a` partir de la      
   ligne pFirstLine dans la fenetre frame suite au changement 
   de largeur de la boite pFirstBox.                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                RecomputeLines (PtrAbstractBox pAb, PtrLine pFirstLine, PtrBox pFirstBox, int frame)
#else  /* __STDC__ */
void                RecomputeLines (pAb, pFirstLine, pFirstBox, frame)
PtrAbstractBox      pAb;
PtrLine             pFirstLine;
PtrBox              pFirstBox;
int                 frame;

#endif /* __STDC__ */
{
   int                 l, h, height;
   int                 width;
   ThotBool            changeSelectBegin;
   ThotBool            changeSelectEnd;
   ThotBool            status;
   Propagation         propagateStatus;
   UCHAR_T               charIndex;
   PtrLine             pLine;
   PtrBox              pBox;
   PtrBox              pSelBox;
   ViewFrame          *pFrame;
   ViewSelection      *pSelBegin;
   ViewSelection      *pSelEnd;

   /* Si la boite est eclatee, on remonte jusqu'a la boite bloc de lignes */
   while (pAb->AbBox->BxType == BoGhost)
      pAb = pAb->AbEnclosing;

   pBox = pAb->AbBox;
   if (pBox != NULL)
     {
       if (pBox->BxFirstLine == NULL)
	 pLine = NULL;
       else
	 {
	   if (pFirstLine == NULL)
	     pLine = pBox->BxFirstLine;
	   else
	     {
	       /* regarde si cette ligne fait bien parti de ce bloc de lignes */
	       pLine = pBox->BxFirstLine;
	       while (pLine != pFirstLine && pLine != NULL)
		 pLine = pLine->LiNext;
	       if (pLine == NULL)
		 /* cette ligne ne fait plus parti du bloc de lignes */
		 return;
	     }
	 }

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
	/* parce que RemoveLines va liberer toutes les boites de coupure.    */
	if (pFirstBox != NULL)
	   if (pFirstBox->BxType == BoPiece || pFirstBox->BxType == BoDotted)
	      pFirstBox = pFirstBox->BxAbstractBox->AbBox;

	status = ReadyToDisplay;
	ReadyToDisplay = FALSE;
	RemoveLines (pBox, frame, pLine, &changeSelectBegin, &changeSelectEnd);
	if (pBox->BxFirstLine == NULL)
	  {
	     /* fait reevaluer la mise en lignes et on recupere */
	     /* la hauteur et la largeur du contenu de la boite */
	     GiveEnclosureSize (pAb, frame, &width, &height);
	  }
	else
	  {
	     ComputeLines (pBox, frame, &height);
	     if (pBox->BxContentWidth)
	       {
		 /* it's an extensible bloc of lines */
		 width = pBox->BxMaxWidth;
		 h = pBox->BxYOrg;
		 pBox->BxWidth = pBox->BxMaxWidth;
	       }
	     width = 0;
	  }
	ReadyToDisplay = status;

	/* Zone affichee apres modification */
	/* Il ne faut pas tenir compte de la boite si elle */
	/* n'est pas encore placee dans l'image concrete   */
	if (ReadyToDisplay && !pBox->BxXToCompute && !pBox->BxYToCompute)
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
	pFrame = &ViewFrameTable[frame - 1];
	pSelBegin = &pFrame->FrSelectionBegin;
	if (changeSelectBegin && pSelBegin->VsBox != NULL)
	  {
	     /* Si la selection a encore un sens */
	     if (pSelBegin->VsBox->BxAbstractBox != NULL)
		ComputeViewSelMarks (pSelBegin);
	  }

	pSelEnd = &pFrame->FrSelectionEnd;
	if (changeSelectEnd && pSelEnd->VsBox != NULL)
	  {
	     /* Si la selection a encore un sens */
	     if (pSelEnd->VsBox->BxAbstractBox != NULL)
	       {

		  if (pSelEnd->VsBuffer == pSelBegin->VsBuffer
		      && pSelEnd->VsIndBuf == pSelBegin->VsIndBuf)
		    {
		       pSelEnd->VsIndBox = pSelBegin->VsIndBox;
		       pSelEnd->VsXPos = pSelBegin->VsXPos;
		       pSelEnd->VsBox = pSelBegin->VsBox;
		       pSelEnd->VsNSpaces = pSelBegin->VsNSpaces;
		       pSelEnd->VsLine = pSelBegin->VsLine;
		    }
		  else
		     ComputeViewSelMarks (pSelEnd);

		  /* Recherche la position limite du caractere */
		  pSelBox = pSelEnd->VsBox;
		  if (pSelBox->BxAbstractBox->AbLeafType != LtText)
		     pSelEnd->VsXPos += pSelBox->BxWidth;
		  else if (pSelBox->BxNChars == 0 && pSelBox->BxType == BoComplete)
		     pSelEnd->VsXPos += pSelBox->BxWidth;
		  else if (pSelEnd->VsIndBox == pSelBox->BxNChars)
		     pSelEnd->VsXPos += 2;
		  else
		    {
		       charIndex = (unsigned char) (pSelEnd->VsBuffer->BuContent[pSelEnd->VsIndBuf - 1]);
		       if (charIndex == _SPACE_ && pSelBox->BxSpaceWidth != 0)
			  pSelEnd->VsXPos += pSelBox->BxSpaceWidth;
		       else
			  pSelEnd->VsXPos += CharacterWidth (charIndex, pSelBox->BxFont);
		    }
	       }
	  }

	if (width != 0 && width != pBox->BxWidth)
	   ChangeDefaultWidth (pBox, pFirstBox, width, 0, frame);
	/* Faut-il conserver la hauteur ? */
	if (height != 0)
	  {
	     /* Il faut propager la modification de hauteur */
	     propagateStatus = Propagate;
	     /* We certainly need to re-check the height of enclosing elements */
	     /*if (propagateStatus == ToChildren)
	       RecordEnclosing (pBox, FALSE);*/
	     ChangeDefaultHeight (pBox, pFirstBox, height, frame);
	     Propagate = propagateStatus;
	  }
     }
}


/*----------------------------------------------------------------------
   UpdateLineBlock met a` jour le bloc de ligne (pAb) apres modification  
   de la largeur de la boite incluse pBox de xDelta.       
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                UpdateLineBlock (PtrAbstractBox pAb, PtrLine pLine, PtrBox pBox, int xDelta, int spaceDelta, int frame)
#else  /* __STDC__ */
void                UpdateLineBlock (pAb, pLine, pBox, xDelta, spaceDelta, frame)
PtrAbstractBox      pAb;
PtrLine             pLine;
PtrBox              pBox;
int                 xDelta;
int                 spaceDelta;
int                 frame;

#endif /* __STDC__ */
{
   int                 lostPixels;
   int                 maxlostPixels;
   int                 nChars;
   int                 realLength = 0;
   int                 width, maxLength;
   int                 newIndex;
   int                 nSpaces;
   PtrTextBuffer       pNewBuff;
   PtrBox              pFirstBox;
   PtrBox              pParentBox;
   AbDimension        *pDimAb;
   PtrLine             pLi2;

   /* Si la boite est eclatee, on remonte jusqu'a la boite bloc de lignes */
   while (pAb->AbBox->BxType == BoGhost)
      pAb = pAb->AbEnclosing;
   pParentBox = pAb->AbBox;

   if (pLine != NULL)
     {
	pDimAb = &pAb->AbWidth;
	/* C'est une ligne extensible --> on l'etend */
	if (pParentBox->BxContentWidth)
	  {
	     /* Si le bloc de lignes deborde de sa dimension minimale */
	     if (!pDimAb->DimIsPosition && pDimAb->DimMinimum
		 && pParentBox->BxWidth + xDelta < pParentBox->BxRuleWidth)
	       {
		  /* Il faut prendre la largeur du minimum */
		  pParentBox->BxContentWidth = FALSE;
		  RecomputeLines (pAb, NULL, NULL, frame);
	       }
	     else
	       {
		  ShiftLine (pLine, pAb, pBox, xDelta, frame);
		  pLine->LiXMax = pLine->LiRealLength;
		  ResizeWidth (pParentBox, pParentBox, NULL, xDelta, 0, frame);
	       }
	  }
	/* Si le bloc de lignes deborde de sa dimension minimale */
	else if (!pDimAb->DimIsPosition && pDimAb->DimMinimum
		 && pLine->LiRealLength + xDelta > pParentBox->BxWidth)
	  {
	     /* Il faut prendre la largeur du contenu */
	     pParentBox->BxContentWidth = TRUE;
	     RecomputeLines (pAb, NULL, NULL, frame);
	  }
	/* C'est une ligne non extensible */
	else
	  {
	     /* calcule la place qu'il reste dans la ligne courante */
	     pLine->LiNSpaces += spaceDelta;
	     maxlostPixels = pLine->LiNSpaces * SPACE_VALUE_MAX + xDelta;
	     if (pLine->LiSpaceWidth > 0)
	       {
		  /* Line justifiee */
		  lostPixels = CharacterWidth (_SPACE_, pBox->BxFont);
		  realLength = pLine->LiRealLength + xDelta - spaceDelta * (pLine->LiSpaceWidth - lostPixels);
		  lostPixels = pLine->LiXMax - pLine->LiMinLength;
	       }
	     else
		lostPixels = pLine->LiXMax - pLine->LiRealLength;

	     /* Est-ce que la boite debordait de la ligne ? */
	     if (pBox->BxWidth - xDelta > pLine->LiXMax)
	       {
		 /* Pixels liberes dans la ligne */
		  lostPixels = pLine->LiXMax - pBox->BxWidth;
		  if (lostPixels > 0)
		     RecomputeLines (pAb, pLine, NULL, frame);
	       }
	     /* Peut-on compresser ou completer la ligne ? */
	     else if ((xDelta > 0 && xDelta <= lostPixels)
		      || (xDelta < 0 && (lostPixels < maxlostPixels
		  || (pLine->LiPrevious == NULL && pLine->LiNext == NULL))))
		if (pLine->LiSpaceWidth == 0)
		   ShiftLine (pLine, pAb, pBox, xDelta, frame);
		else
		  {
		     CompressLine (pLine, xDelta, frame, spaceDelta);
		     pLine->LiRealLength = realLength;
		  }
	     /* Peut-on eviter la reevaluation du bloc de lignes ? */
	     else if (xDelta < 0)
	       {

		  /* Peut-on remonter le premier mot de la ligne courante ? */
		  if (pLine->LiPrevious != NULL)
		    {
		       /* Largeur restante */
		       maxLength = pLine->LiPrevious->LiXMax - pLine->LiPrevious->LiRealLength - SPACE_VALUE_MAX;
		       if (pLine->LiFirstPiece != NULL)
			  pFirstBox = pLine->LiFirstPiece;
		       else
			  pFirstBox = pLine->LiFirstBox;
		       if (pFirstBox->BxWidth > maxLength)
			  if (pFirstBox->BxAbstractBox->AbLeafType == LtText && maxLength > 0)
			    {
			       /* Elimine le cas des lignes sans blanc */
			       if (pLine->LiNSpaces == 0)
				  maxLength = 1;	/* force la reevaluation */
			       else
				 {
				    nChars = pFirstBox->BxNChars;
				    /* regarde si on peut trouver une coupure */
				    maxLength = SearchBreak (pLine, pFirstBox, maxLength, pFirstBox->BxFont, &nChars, &width,
					    &nSpaces, &newIndex, &pNewBuff);
				 }

			       if (maxLength > 0)
				  RecomputeLines (pAb, pLine->LiPrevious, NULL, frame);
			    }
			  else
			     maxLength = 0;
		    }
		  else
		     maxLength = 0;

		  /* Peut-on remonter le premier mot de la ligne suivante ? */
		  if (maxLength == 0)
		    {
		       if (pLine->LiNext != NULL)
			 {
			    maxLength = pLine->LiXMax - pLine->LiRealLength - xDelta;
			    pLi2 = pLine->LiNext;
			    if (pLi2->LiFirstPiece != NULL)
			       pFirstBox = pLi2->LiFirstPiece;
			    else
			       pFirstBox = pLi2->LiFirstBox;

			    if (pFirstBox->BxWidth > maxLength)
			       if (pFirstBox->BxAbstractBox->AbLeafType == LtText && maxLength > 0)
				 {
				    /* Elimine le cas des lignes sans blanc */
				    if (pLi2->LiNSpaces == 0)
				       maxLength = 1;	/* force la reevaluation */
				    else
				      {
					 nChars = pFirstBox->BxNChars;
					 /* regarde si on peut trouver une coupure */
					 maxLength = SearchBreak (pLi2, pFirstBox, maxLength, pFirstBox->BxFont,
								  &nChars, &width, &nSpaces, &newIndex, &pNewBuff);
				      }
				 }
			       else
				  maxLength = 0;
			 }

		       if (maxLength > 0)
			  RecomputeLines (pAb, pLine, NULL, frame);
		       else if (pLine->LiSpaceWidth == 0)
			  ShiftLine (pLine, pAb, pBox, xDelta, frame);
		       else
			 {
			    CompressLine (pLine, xDelta, frame, spaceDelta);
			    pLine->LiRealLength = realLength;
			 }
		    }
		  else
		     RecomputeLines (pAb, pLine, NULL, frame);
	       }
	     else
		RecomputeLines (pAb, pLine, NULL, frame);
	  }
     }
}


/*----------------------------------------------------------------------
   EncloseInLine assure l'englobement de la boite pBox  dans la      
   ligne du pave pAb et propage les modifications necessaires.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                EncloseInLine (PtrBox pBox, int frame, PtrAbstractBox pAb)
#else  /* __STDC__ */
void                EncloseInLine (pBox, frame, pAb)
PtrBox              pBox;
int                 frame;
PtrAbstractBox      pAb;

#endif /* __STDC__ */
{
  PtrBox              pFirstBox;
  PtrBox              pPieceBox;
  PtrBox              pParentBox;
  int                 ascent, descent;
  int                 i, h;
  int                 pos, linespacing;
  PtrLine             pLine;
  PtrLine             pNextLine;

  pParentBox = pAb->AbBox;
  if (Propagate != ToSiblings || pParentBox->BxVertFlex)
    {
      pLine = SearchLine (pBox);
      if (pLine)
	{
	  pNextLine = pLine->LiNext;
	  ascent = 0;
	  descent = 0;
	  linespacing = PixelValue (pAb->AbLineSpacing, pAb->AbLineSpacingUnit, pAb, ViewFrameTable[frame - 1].FrMagnification);

	  if (!pBox->BxAbstractBox->AbHorizEnclosing)
	    {
	      /* The box is out of lines (like page breaks) */
	      pLine->LiHorizRef = pBox->BxHorizRef;
	      pLine->LiHeight = pBox->BxHeight;
	      descent = pLine->LiHeight - pLine->LiHorizRef;
	      /* move the box */
	      i = pParentBox->BxYOrg + pLine->LiYOrg - pBox->BxYOrg;
	      YMove (pBox, NULL, i, frame);
	      /* delta of the block height if it's the last line */
	      h = pLine->LiYOrg + pLine->LiHeight - pParentBox->BxHeight;
	    }
	  else
	    {
	      /* The box is split in lines */
	      /* compute the line ascent and the line descent */
	      pPieceBox = pLine->LiFirstBox;
	      if (pPieceBox->BxType == BoSplit && pLine->LiFirstPiece)
		pPieceBox = pLine->LiFirstPiece;
	      /* loop on included boxes */
	      do
		{
		  if (pPieceBox->BxType == BoSplit)
		    pFirstBox = pPieceBox->BxNexChild;
		  else
		    pFirstBox = pPieceBox;
		  
		  if (ascent < pFirstBox->BxHorizRef)
		    ascent = pFirstBox->BxHorizRef;
		  i = pFirstBox->BxHeight - pFirstBox->BxHorizRef;
		  if (descent < i)
		    descent = i;
		  /* next box */
		  pPieceBox = GetNextBox (pFirstBox->BxAbstractBox);
		}
	      while (pPieceBox && pFirstBox != pLine->LiLastBox && pFirstBox != pLine->LiLastPiece);
	      if (pLine->LiPrevious)
		{
		  /* new position of the current line */
		  if (linespacing < pLine->LiPrevious->LiHorizRef + ascent)
		    /* we refuse to overlaps 2 lines */
		    i = pLine->LiPrevious->LiHorizRef + ascent;
		  else
		    i = linespacing;
		  pos = pLine->LiPrevious->LiYOrg + pLine->LiPrevious->LiHorizRef
		    + i - ascent;
		  /* vertical shifting of the current line baseline */
		  i = pos - pLine->LiYOrg + ascent - pLine->LiHorizRef;
		}
	      else
		{
		  /* new position of the current line */
		  pos = 0;
		  /* vertical shifting of the current line baseline */
		  i = pos - pLine->LiYOrg + ascent - pLine->LiHorizRef;
		}
	      
	      /* move the line contents */
	      pLine->LiYOrg = pos;
	      if (i)
		{
		  /* align boxes of the current line */
		  pPieceBox = pLine->LiFirstBox;
		  if (pPieceBox->BxType == BoSplit && pLine->LiFirstPiece)
		    pPieceBox = pLine->LiFirstPiece;
		  
		  do
		    {
		      if (pPieceBox->BxType == BoSplit)
			pFirstBox = pPieceBox->BxNexChild;
		      else
			pFirstBox = pPieceBox;
		      
		      if (pFirstBox != pBox)
			YMove (pFirstBox, NULL, i, frame);
		      pPieceBox = GetNextBox (pFirstBox->BxAbstractBox);
		    }
		  while (pPieceBox && pFirstBox != pLine->LiLastBox && pFirstBox != pLine->LiLastPiece);
		}
	      
	      /* change the baseline of the current line */
	      i = ascent - pLine->LiHorizRef;
	      pLine->LiHorizRef = ascent;
	      if (i)
		{
		  /* move the block baseline if it's inherited from the first line */
		  if (pAb->AbHorizRef.PosAbRef == pAb->AbFirstEnclosed &&
		      !pLine->LiPrevious)
		    MoveHorizRef (pParentBox, NULL, i, frame);
		}
	      
	      /* move the box */
	      /* vertical shifting of the box */
	      i = pParentBox->BxYOrg + pLine->LiYOrg + pLine->LiHorizRef - pBox->BxHorizRef - pBox->BxYOrg;
	      if (i)
		YMove (pBox, NULL, i, frame);
	      
	      /* update the rest of the block */
	      pLine->LiHeight = descent + ascent;
	      /* delta of the block height if it's the last line */
	      h = pLine->LiYOrg + pLine->LiHeight - pParentBox->BxHeight;
	    }

	  /* move next lines */
	  if (pNextLine)
	    {
	      /* new position of the next line */
	      if (!pBox->BxAbstractBox->AbHorizEnclosing ||
		  linespacing < descent + pNextLine->LiHorizRef)
		/* we set the next line under the previous one */
		i = descent + pNextLine->LiHorizRef;
	      else
		i = linespacing;
	      pos = pLine->LiYOrg + pLine->LiHorizRef + i - pNextLine->LiHorizRef;
	      /* vertical shifting of the next lines */
	      h = pos - pNextLine->LiYOrg;
	      if (h)
		{
		  while (pNextLine)
		    {
		      pNextLine->LiYOrg += h;
		      pPieceBox = pNextLine->LiFirstBox;
		      if (pPieceBox->BxType == BoSplit && pNextLine->LiFirstPiece)
			pPieceBox = pNextLine->LiFirstPiece;
		      do
			{
			  if (pPieceBox->BxType == BoSplit)
			    pFirstBox = pPieceBox->BxNexChild;
			  else
			    pFirstBox = pPieceBox;
			  YMove (pFirstBox, NULL, h, frame);
			  pPieceBox = GetNextBox (pFirstBox->BxAbstractBox);
			}
		      while (pPieceBox && pFirstBox != pNextLine->LiLastBox &&
			     pFirstBox != pNextLine->LiLastPiece);
		  
		      pNextLine = pNextLine->LiNext;
		    }
	      
		}
	    }
	  /* update the block height */
	  if (pParentBox->BxContentHeight)
	    ChangeDefaultHeight (pParentBox, pParentBox, pParentBox->BxHeight + h, frame);
	}
    }
}
