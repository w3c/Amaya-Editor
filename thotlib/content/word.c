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
 * This module locates words in Thot documents.
 *
 * Authors: H. Richy, E. Picheral (INRIA)
 *
 */

#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "language.h"

static PtrDocument  DocInitialSelection;
static PtrElement   firstElInitialSelection;
static PtrElement   lastElInitialSelection;
static int          firstCharInitialSelection;
static int          lastCharInitialSelection;

#include "structselect_f.h"
#include "tree_f.h"
#include "hyphen_f.h"

/*----------------------------------------------------------------------
   NextCharacter retourne le caractere qui suit le caractere courant     
   buffer[rank] et met a jour buffer et rank.              
   S'il n'y a pas de caractere suivant retourne le         
   caractere NULL.                                         
  ----------------------------------------------------------------------*/

#ifdef __STDC__
unsigned char       NextCharacter (PtrTextBuffer * buffer, int *rank)

#else  /* __STDC__ */
unsigned char       NextCharacter (buffer, rank)
PtrTextBuffer      *buffer;
int                *rank;

#endif /* __STDC__ */

{

   if (*buffer == NULL)
      return (0);
   else
     {
	(*rank)++;

	if (*rank >= (*buffer)->BuLength)
	  {
	     /* get next buffer in the same Text element */
	     *rank = 0;
	     *buffer = (*buffer)->BuNext;
	     if (*buffer == NULL)
		return (0);
	     else
		return ((*buffer)->BuContent[*rank]);
	  }
	else
	   return ((*buffer)->BuContent[*rank]);
     }
}

/*----------------------------------------------------------------------
   PreviousCharacter retourne le caractere qui precede le caractere        
   courant buffer[rank] et met a jour buffer et rank.      
   S'il n'y a pas de caractere precedent retourne le       
   caractere `\0`.                                         
  ----------------------------------------------------------------------*/

#ifdef __STDC__
unsigned char       PreviousCharacter (PtrTextBuffer * buffer, int *rank)

#else  /* __STDC__ */
unsigned char       PreviousCharacter (buffer, rank)
PtrTextBuffer      *buffer;
int                *rank;

#endif /* __STDC__ */

{

   if (*buffer == NULL)
      return (0);
   else
     {
	(*rank)--;
	if (*rank < 0)
	  {
	     /* get previous buffer in the same Text element */
	     *rank = 0;
	     *buffer = (*buffer)->BuPrevious;
	     if (*buffer == NULL)
		return (0);
	     else
	       {
		  *rank = (*buffer)->BuLength - 1;
		  return ((*buffer)->BuContent[*rank]);
	       }
	  }
	else
	   return ((*buffer)->BuContent[*rank]);
     }
}

/*----------------------------------------------------------------------
   MotOk teste que la chaine designee constitue bien un MOT.       
  ----------------------------------------------------------------------*/

#ifdef __STDC__
boolean             MotOk (PtrElement firstEl, int firstChar, PtrElement lastEl, int lastChar)

#else  /* __STDC__ */
boolean             MotOk (firstEl, firstChar, lastEl, lastChar)
PtrElement          firstEl;
int                 firstChar;
PtrElement          lastEl;
int                 lastChar;

#endif /* __STDC__ */

{
   if (lastChar > 0)
     {
	if (lastEl->ElTextLength >= lastChar)
	   /* there is a following character in the same element */
	   if (!IsSeparatorChar (lastEl->ElText->BuContent[lastChar - 1]))
	      /* it's not a word separator. The string is not a word */
	      return (FALSE);
	if (firstChar > 1)
	   /* there is a previous character in the same element */
	   if (!IsSeparatorChar (firstEl->ElText->BuContent[firstChar - 2]))
	      /* it's not a word separator. The string is not a word */
	      return (FALSE);
     }
   return (TRUE);
}


/*----------------------------------------------------------------------
   InitSearchDomain rend vrai si l'initialisation s'est bien passee   
   Le parame`tre domain vaut :                                     
   0 si on recherche avant la selection                          
   1 si on recherche dans la selection                           
   2 si on recherche apres la selection                          
   3 si on recherche dans tout le document.                      
   Le parame`tre context pointe sur le contexte de domaine de      
   recherche a` initialiser.                                       
  ----------------------------------------------------------------------*/

#ifdef __STDC__
boolean             InitSearchDomain (int domain, PtrSearchContext context)

#else  /* __STDC__ */
boolean             InitSearchDomain (domain, context)
int                 domain;
PtrSearchContext    context;

#endif /* __STDC__ */

{
   boolean             ok;

   if (context == NULL)
      return (FALSE);
   else
     {
	context->STree = 0;
	context->SWholeDocument = FALSE;
     }

   /* get curreent selection */
   ok = GetCurrentSelection (&DocInitialSelection,
			  &firstElInitialSelection, &lastElInitialSelection,
		     &firstCharInitialSelection, &lastCharInitialSelection);
   /* store the word search domain */
   if (domain == 3)
      /* the whole document */
     {
	context->SStartElement = NULL;
	context->STree = 0;
	context->SWholeDocument = TRUE;
	context->SEndElement = NULL;
	context->SStartChar = 0;
	context->SEndChar = 0;
	context->SStartToEnd = TRUE;
     }
   else if (!ok)
      /* There is no selection */
      context->SDocument = NULL;
   else if (context->SDocument != DocInitialSelection)
      /* current selection is not in the document of interest */
      context->SDocument = NULL;
   else
     {
	context->SWholeDocument = FALSE;
	if (domain == 0)
	   /* Search before selection */
	  {
	     context->SStartElement = NULL;
	     context->SStartChar = 0;
	     context->SEndElement = firstElInitialSelection;
	     context->SEndChar = firstCharInitialSelection;
	     context->SStartToEnd = FALSE;
	  }
	else if (domain == 1)
	   /* search within selection */
	  {
	     context->SStartElement = firstElInitialSelection;
	     context->SStartChar = firstCharInitialSelection - 1;
	     context->SEndElement = lastElInitialSelection;
	     context->SEndChar = lastCharInitialSelection - 1;
	     context->SStartToEnd = TRUE;
	  }
	else if (domain == 2)
	   /* search after selection */
	  {
	     context->SStartElement = lastElInitialSelection;
	     context->SStartChar = lastCharInitialSelection - 1;
	     if (context->SStartChar < 0)
		context->SStartChar = 0;
	     if (lastElInitialSelection != NULL)
		if (lastCharInitialSelection == 0 ||
		    lastCharInitialSelection > lastElInitialSelection->ElTextLength)
		   if (!lastElInitialSelection->ElTerminal)
		     {
			context->SStartElement = NextElement (lastElInitialSelection);
			context->SStartChar = 0;
		     }
	     context->SEndElement = NULL;
	     context->SEndChar = 0;
	     context->SStartToEnd = TRUE;
	  }
     }

   if (context->SDocument == NULL)
     {
	/* no search domain */
	context->SStartElement = NULL;
	context->SEndElement = NULL;
	context->SStartChar = 0;
	context->SEndChar = 0;
	context->STree = 0;
	context->SWholeDocument = FALSE;
	return (FALSE);
     }
   return (TRUE);
}


/*----------------------------------------------------------------------
   UpdateDuringSearch met a jour la selection initiale                
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                UpdateDuringSearch (PtrElement pEl, int len)

#else  /* __STDC__ */
void                UpdateDuringSearch (pEl, len)
PtrElement          pEl;
int                 len;

#endif /* __STDC__ */

{
   if (pEl == lastElInitialSelection)
      /* initial selection is within the element */
      if (lastCharInitialSelection != 0)
	 /* change position of last character of search domain */
	 lastCharInitialSelection += len;
}


/*----------------------------------------------------------------------
   RestoreAfterSearch re'tablit la se'lection initiale et nettoie     
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                RestoreAfterSearch ()

#else  /* __STDC__ */
void                RestoreAfterSearch ()
#endif				/* __STDC__ */

{
   int                 prevLen, endChar;

   CancelSelection ();
   endChar = lastCharInitialSelection;
   if (endChar > 0)

      if (firstCharInitialSelection > 0)
	{
	   if (firstElInitialSelection == lastElInitialSelection)
	      prevLen = endChar;
	   else
	      prevLen = 0;
	   SelectString (DocInitialSelection, firstElInitialSelection,
			 firstCharInitialSelection, prevLen);
	}
      else
	 SelectElement (DocInitialSelection, firstElInitialSelection, TRUE, TRUE);

   if (lastElInitialSelection != firstElInitialSelection &&
       lastElInitialSelection != NULL)
      ExtendSelection (lastElInitialSelection, endChar, TRUE, FALSE, FALSE);
}

/*----------------------------------------------------------------------
   NextTree Pour le contexte de recherche context, cherche le  
   prochain arbre a traiter, lorsqu'on est dans une recherche qui  
   porte sur tout le document.                                     
   Met a jour le contexte de recherche pour pouvoir relancer la    
   recherche dans le nouvel arbre a traiter.                       
   Retourne TRUE si un arbre suivant a ete trouve' et, dans ce cas,
   elCourant et carCourant representent la position ou il faut     
   relancer la recherche.                                          
  ----------------------------------------------------------------------*/

#ifdef __STDC__
boolean             NextTree (PtrElement * pEl, int *charIndx, PtrSearchContext context)

#else  /* __STDC__ */
boolean             NextTree (pEl, charIndx, context)
PtrElement         *pEl;
int                *charIndx;
PtrSearchContext    context;

#endif /* __STDC__ */

{
   int                 i;
   boolean             ret;

   *pEl = NULL;
   *charIndx = 0;
   ret = FALSE;
   if (context != NULL)
      if (context->SWholeDocument && context->STree >= 0)
	{
	   i = context->STree;
	   context->STree = -1;
	   if (context->SStartToEnd)
	      /* search forward */
	     {
		i++;
		while (i < MAX_ASSOC_DOC && context->STree < 0)
		   if (context->SDocument->DocAssocRoot[i - 1] != NULL)
		      context->STree = i;
		   else
		      i++;
	     }
	   else
	      /* search backward */
	     {
		i--;
		while (i > 0 && context->STree < 0)
		   if (context->SDocument->DocAssocRoot[i - 1] != NULL)
		      context->STree = i;
		   else
		      i--;
		if (i == 0)
		   if (context->SDocument->DocRootElement != NULL)
		      context->STree = 0;
	     }
	   if (context->STree == 0)
	      *pEl = context->SDocument->DocRootElement;
	   else if (context->STree > 0)
	      *pEl = context->SDocument->DocAssocRoot[i - 1];
	   if (*pEl != NULL)
	      if (!context->SStartToEnd)
		{
		   *pEl = LastLeaf (*pEl);
		   *charIndx = (*pEl)->ElVolume;
		}
	   ret = (context->STree >= 0);
	}
   return ret;

}

/*----------------------------------------------------------------------
   SearchNextWord recherche dans le domaine de recherche le mot qui	
   suit la position courante.                                  	
   Retourne le mot selectionne et met a jour le pointeur           
   a la fin du mot selectionne.                                    
   Le parame`tre context pointe sur le contexte de domaine de      
   recherche concerne' ou NULL si aucun domaine fixe'.             
  ----------------------------------------------------------------------*/

#ifdef __STDC__
boolean             SearchNextWord (PtrElement * curEl, int *curChar, char word[MAX_WORD_LEN], PtrSearchContext context)

#else  /* __STDC__ */
boolean             SearchNextWord (curEl, curChar, word, context)
PtrElement         *curEl;
int                *curChar;
char                word[MAX_WORD_LEN];
PtrSearchContext    context;

#endif /* __STDC__ */

{
   PtrElement          pEl, endEl;
   PtrElement          pAncestor;
   int                 iChar, endChar;
   int                 len;
   int                 index;
   PtrTextBuffer       pBuf;
   unsigned char       charact;

   pEl = *curEl;
   iChar = *curChar;
   word[0] = EOS;

   if (pEl == NULL && context != NULL)
     {
	/* search start */
	pEl = context->SStartElement;
	iChar = context->SStartChar;
	if (pEl == NULL && context->SDocument != NULL)
	   pEl = context->SDocument->DocRootElement;
     }

   /* Check that element is a Text element and that the research does */
   /* not start on the last character */
   if (pEl != NULL)
      if (pEl->ElTypeNumber != CharString + 1 || iChar + 1 >= pEl->ElTextLength)
	{
	   /* there is no buffer */
	   pBuf = NULL;
	   iChar = 0;
	}
      else
	 /* first buffer of element */
	 pBuf = pEl->ElText;

   /* looks for the end of the research domain */
   if (context == NULL)
     {
	endEl = NULL;
	endChar = 0;
     }
   else
     {
	endEl = context->SEndElement;
	endChar = context->SEndChar;
     }

   while (pBuf == NULL && pEl != NULL)
     {
	/* Search the first non-empty and non-protected element */
	pEl = FwdSearchTypedElem (pEl, CharString + 1, NULL);
	iChar = 0;
	if (pEl != NULL)
	  {
	     if (endEl != NULL)
		if (ElemIsBefore (endEl, pEl))
		   /* the element found is after the end of the domain */
		   pEl = NULL;
	  }
	else if (context != NULL)
	   if (context->SWholeDocument)
	     {
		/* get the next tree to process */
		if (NextTree (&pEl, &iChar, context))
		   if (pEl->ElTypeNumber != CharString + 1)
		      pEl = FwdSearchTypedElem (pEl, CharString + 1, NULL);
	     }

	if (pEl != NULL)
	   /* on verifie que cet element ne fait pas partie d'une inclusion et */
	   /* n'est pas cache' a l'utilisateur */
	  {
	     pAncestor = pEl;
	     while (pAncestor->ElParent != NULL && pAncestor->ElSource == NULL)
		pAncestor = pAncestor->ElParent;
	     if (pAncestor->ElSource == NULL)
		/* on n'est pas dans une inclusion */
		if (!ElementIsHidden (pEl))
		   /* l'element n'est pas cache' */
		   /* On saute les elements vides */
		   if (pEl->ElTextLength != 0)
		      pBuf = pEl->ElText;
	  }
     }

   if (pEl == NULL)
      return (FALSE);
   else if (pEl == endEl && iChar >= endChar)
      /* On est arrive a la fin du domaine de recherche */
      return (FALSE);
   else
     {

	/* Calcule l'index de depart de la recherche */
	len = iChar;
	while (len >= pBuf->BuLength)
	  {
	     /* On passe au buffer suivant */
	     len -= pBuf->BuLength;
	     pBuf = pBuf->BuNext;
	  }
	index = len;
	charact = pBuf->BuContent[index];

	/* On se place au debut du mot */
	while (charact != 0 && IsSeparatorChar (charact)
	       && (pEl != endEl || iChar < endChar))
	  {
	     charact = NextCharacter (&pBuf, &index);
	     iChar++;
	  }

	/* Recherche le premier separateur apres le mot */
	/* On verifie que l'on ne depasse pas la fin du domaine de recherche */
	len = 0;
	while (len < MAX_WORD_LEN-1 && charact != 0
	       && !IsSeparatorChar (charact)
	       && (pEl != endEl || iChar < endChar))
	  {
	     word[len++] = charact;
	     charact = NextCharacter (&pBuf, &index);
	     iChar++;
	  }

	/* positionne les valeurs de retour */
	word[len] = EOS;
	*curEl = pEl;
	*curChar = iChar;
	/* Si on a trouve effectivement un mot */
	if (len > 0)
	   return (TRUE);
	else
	   /* On peut etre en fin de feuille qui se termine par un espace */
	   /* On continue la recherche */
	   return (SearchNextWord (curEl, curChar, word, context));
     }
}


/*----------------------------------------------------------------------
   SearchPreviousWord recheche dans le domaine de recherche le mot   
   qui precede la position courante.                       
   Retourne le mot selectionne et met a jour le pointeur   
   au debut du mot selectionne.                            
   Le parame`tre context pointe sur le contexte de domaine de      
   recherche concerne' ou NULL si aucun domaine fixe'.             
  ----------------------------------------------------------------------*/

#ifdef __STDC__
boolean             SearchPreviousWord (PtrElement * curEl, int *curChar, char word[MAX_WORD_LEN], PtrSearchContext context)

#else  /* __STDC__ */
boolean             SearchPreviousWord (curEl, curChar, word, context)
PtrElement         *curEl;
int                *curChar;
char                word[MAX_WORD_LEN];
PtrSearchContext    context;

#endif /* __STDC__ */

{
   PtrElement          pEl, endEl;
   PtrElement          pAncestor;
   int                 iChar, endChar, j;
   int                 len;
   int                 index;
   PtrTextBuffer       pBuf;
   unsigned char       charact;
   char                reverse[MAX_WORD_LEN];

   pEl = *curEl;
   iChar = *curChar;
   word[0] = EOS;

   if (pEl == NULL)
     {
	/* C'est le debut de la recherche */
	pEl = context->SEndElement;
	iChar = context->SEndChar;
	if (iChar > 2)
	   /* La fin de selection pointe toujours sur un caratere plus loin */
	   iChar--;
     }
   iChar--;

   /* Verifie que l'element est de type texte */
   /* et que la recherche ne debute pas sur le dernier caractere */
   if (pEl->ElTypeNumber != CharString + 1 || iChar <= 0)
     {
	/* On n'a pas trouve de buffer */
	pBuf = NULL;
	iChar = 0;
     }
   else
     {
	/* 1er Buffer de l'element */
	pBuf = pEl->ElText;
     }

   /* Determine l'element limite du contexte de recherche */
   if (context == NULL)
     {
	endEl = NULL;
	endChar = 0;
     }
   else
     {
	endEl = context->SStartElement;
	endChar = context->SStartChar;
     }

   while (pBuf == NULL && pEl != NULL)
     {
	/* Recherche le premier element texte non vide */
	pEl = BackSearchTypedElem (pEl, CharString + 1, NULL);
	if (pEl != NULL)
	  {
	     if (endEl != NULL)
		if (ElemIsBefore (pEl, endEl))
		   /* l'element trouve' est avant l'element de debut, */
		   /* on fait comme si on n'avait pas trouve' */
		   pEl = NULL;
	  }
	else if (context != NULL)
	   /* Si on recherche dans tout le document on change d'arbre */
	   if (context->SWholeDocument)
	     {
		/* cherche l'arbre a traiter avant celui ou` on n'a pas trouve' */
		if (NextTree (&pEl, &iChar, context))
		   /* Il se peut que l'element rendu soit de type texte */
		   if (pEl->ElTypeNumber != CharString + 1)
		      pEl = BackSearchTypedElem (pEl, CharString + 1, NULL);
	     }

	/* On saute les elements vides */
	if (pEl != NULL)
	   /* on verifie que cet element ne fait pas partie d'une inclusion et */
	   /* n'est pas cache' a l'utilisateur */
	  {
	     pAncestor = pEl;
	     while (pAncestor->ElParent != NULL && pAncestor->ElSource == NULL)
		pAncestor = pAncestor->ElParent;
	     if (pAncestor->ElSource == NULL)
		/* on n'est pas dans une inclusion */
		if (!ElementIsHidden (pEl))
		   /* l'element n'est pas cache' */
		   if (pEl->ElTextLength != 0)
		     {
			iChar = pEl->ElTextLength - 1;
			pBuf = pEl->ElText;
		     }
	  }
     }

   if (pEl == NULL)
      return (FALSE);
   else if (pEl == endEl && iChar < endChar)
      /* On est arrive a la fin du domaine de recherche */
      return (FALSE);
   else
     {
	/* Calcule l'index de depart de la recherche */
	len = iChar;
	while (len >= pBuf->BuLength)
	  {
	     /* On passe au buffer suivant */
	     len -= pBuf->BuLength;
	     pBuf = pBuf->BuNext;
	  }

	index = len;
	charact = pBuf->BuContent[index];
	/* On se place a la fin du mot */
	while (charact != 0 && iChar >= 0 && IsSeparatorChar (charact)
	       && (pEl != endEl || iChar >= endChar))
	  {
	     charact = PreviousCharacter (&pBuf, &index);
	     iChar--;
	  }

	/* On se place au debut du mot et recupere le mot a l'envers */
	len = 0;
	while (len < MAX_WORD_LEN-1 && charact != 0 && iChar >= 0
	       && !IsSeparatorChar (charact)
	       && (pEl != endEl || iChar >= endChar))
	  {
	     reverse[len++] = charact;
	     charact = PreviousCharacter (&pBuf, &index);
	     iChar--;
	  }

	/* Recopie le mot a l'endroit */
	iChar++;
	j = 0;
	while (j < len)
	  {
	     word[j] = reverse[len - 1 - j];
	     j++;
	  }

	/* positionne les valeurs de retour */
	word[len] = EOS;
	*curEl = pEl;
	*curChar = iChar;
	/* Si on a trouve effectivement un mot */
	if (len > 0)
	   return (TRUE);
	else
	   /* On peut etre en fin de feuille qui se termine par un espace */
	   /* On continue la recherche */
	   return (SearchPreviousWord (curEl, curChar, word, context));
     }
}
