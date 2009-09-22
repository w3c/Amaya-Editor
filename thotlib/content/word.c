/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2009
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * This module locates words in Thot documents.
 *
 * Authors: I.Vatton, H. Richy, E. Picheral (INRIA)
 *
 */

#include "thot_gui.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "language.h"

#define THOT_EXPORT extern
#include "select_tv.h"

static PtrDocument  DocSDomain;
static PtrElement   FirstElSDomain;
static PtrElement   LastElSDomain;
static int          FirstCharSDomain;
static int          LastCharSDomain;

#include "structselect_f.h"
#include "tree_f.h"
#include "hyphen_f.h"

/*----------------------------------------------------------------------
   NextCharacter retourne le caractere qui suit le caractere courant     
   buffer[rank] et met a jour buffer et rank.              
   S'il n'y a pas de caractere suivant retourne le         
   caractere NULL.                                         
  ----------------------------------------------------------------------*/
CHAR_T NextCharacter (PtrTextBuffer *buffer, int *rank)
{
  if (*buffer == NULL)
    return EOS;
  else
    {
      (*rank)++;
      if (*rank >= (*buffer)->BuLength)
	{
	  /* get next buffer in the same Text element */
	  *rank = 0;
	  *buffer = (*buffer)->BuNext;
	  if (*buffer == NULL)
	    return EOS;
	  else
	    return ((*buffer)->BuContent[*rank]);
	}
      else
	return (*buffer)->BuContent[*rank];
    }
}

/*----------------------------------------------------------------------
   PreviousCharacter retourne le caractere qui precede le caractere        
   courant buffer[rank] et met a jour buffer et rank.      
   S'il n'y a pas de caractere precedent retourne le       
   caractere `\0`.                                         
  ----------------------------------------------------------------------*/
CHAR_T PreviousCharacter (PtrTextBuffer *buffer, int *rank)
{
  if (*buffer == NULL)
    return EOS;
  else
    {
      (*rank)--;
      if (*rank < 0)
	{
	  /* get previous buffer in the same Text element */
	  *rank = 0;
	  *buffer = (*buffer)->BuPrevious;
	  if (*buffer == NULL)
	    return EOS;
	  else
	    {
	      *rank = (*buffer)->BuLength - 1;
	      return (*buffer)->BuContent[*rank];
	    }
	}
      else
	return (*buffer)->BuContent[*rank];
    }
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
ThotBool InitSearchDomain (int domain, PtrSearchContext context)
{
  ThotBool            ok;

  if (context == NULL)
    return (FALSE);
  else
    {
      context->STree = 0;
      context->SWholeDocument = FALSE;
    }

  /* get curreent selection */
  ok = GetCurrentSelection (&DocSDomain,
			    &FirstElSDomain, &LastElSDomain,
			    &FirstCharSDomain, &LastCharSDomain);
  if (LastElSDomain && LastCharSDomain == 0 &&
      LastElSDomain == FirstElSDomain && FirstCharSDomain == 0)
    /* the whole element is selected */
    FirstCharSDomain = LastElSDomain->ElVolume;
  if (FirstElSDomain && FirstCharSDomain == 0)
    /* the whole element is selected */
    FirstCharSDomain = 1;

  /* store the word search domain */
  if (domain == 3)
    /* the whole document */
    {
      context->SStartElement = NULL;
      context->STree = 0;
      context->SWholeDocument = TRUE;
      context->SEndElement = NULL;
      context->SStartChar = 1;
      context->SEndChar = 1;
      context->SStartToEnd = TRUE;
    }
  else if (!ok)
    /* There is no selection */
    context->SDocument = NULL;
  else if (context->SDocument != DocSDomain)
    /* current selection is not in the document of interest */
    context->SDocument = NULL;
  else
    {
      context->SWholeDocument = FALSE;
      if (domain == 0)
	/* Search before selection */
	{
	  context->SStartElement = NULL;
	  context->SStartChar = 1;
	  context->SEndElement = FirstElSDomain;
	  context->SEndChar = FirstCharSDomain;
	  context->SStartToEnd = FALSE;
	}
      else if (domain == 1)
	/* search within selection */
	{
	  context->SStartElement = FirstElSDomain;
	  context->SStartChar = FirstCharSDomain;
	  context->SEndElement = LastElSDomain;
	  context->SEndChar = LastCharSDomain;
	  context->SStartToEnd = TRUE;
	}
      else if (domain == 2)
	/* search after selection */
	{
	  context->SStartElement = LastElSDomain;
	  context->SStartChar = LastCharSDomain;
	  if (context->SStartChar < 1)
	    context->SStartChar = 1;
	  if (LastElSDomain && !LastElSDomain->ElTerminal &&
	      (LastCharSDomain == 0 ||
	       LastCharSDomain > LastElSDomain->ElTextLength))
	    {
	      context->SStartElement = NextElement (LastElSDomain);
	      context->SStartChar = 1;
	    }
	  context->SEndElement = NULL;
	  context->SEndChar = 1;
	  context->SStartToEnd = TRUE;
	}
    }
  
  if (context->SDocument == NULL)
    {
      /* no search domain */
      context->SStartElement = NULL;
      context->SEndElement = NULL;
      context->SStartChar = 1;
      context->SEndChar = 1;
      context->STree = 0;
      context->SWholeDocument = FALSE;
      return (FALSE);
    }
  return (TRUE);
}


/*----------------------------------------------------------------------
   UpdateDuringSearch met a jour la selection initiale                
  ----------------------------------------------------------------------*/
void UpdateDuringSearch (PtrElement pEl, int len)
{
  if (pEl == LastElSDomain && LastCharSDomain != 0)
    /* initial selection is within the element */
    /* change position of last character of search domain */
    LastCharSDomain += len;
}


/*----------------------------------------------------------------------
   RestoreAfterSearch re'tablit la se'lection initiale et nettoie     
  ----------------------------------------------------------------------*/
void RestoreAfterSearch ()
{
  int                 prevLen, endChar;

  CancelSelection ();
  endChar = LastCharSDomain;
  if (endChar > 0)
    {
      if (FirstCharSDomain > 0)
        {
          if (FirstElSDomain == LastElSDomain)
            prevLen = endChar;
          else
            prevLen = 0;
          SelectString (DocSDomain, FirstElSDomain,
                        FirstCharSDomain, prevLen);
        }
      else
        SelectElement (DocSDomain, FirstElSDomain, TRUE, TRUE, TRUE);
    }
   
  if (LastElSDomain != FirstElSDomain && LastElSDomain != NULL)
    ExtendSelection (LastElSDomain, endChar, TRUE, FALSE, FALSE);
}

/*----------------------------------------------------------------------
  SearchNextWord look for the next word in the search domain from the
  current end position.
  Returns the selected word, its beginning and end postion.
  The parameter context points to the search domain.
  ----------------------------------------------------------------------*/
ThotBool SearchNextWord (PtrElement *curEl, int *beginning, int *end,
			 CHAR_T *word, PtrSearchContext context)
{
  PtrElement          pEl, endEl;
  PtrElement          pAncestor;
  PtrTextBuffer       pBuf;
  CHAR_T             charact;
  int                 iChar, endChar;
  int                 len;
  int                 index;

  pEl = *curEl;
  iChar = *end;
  *beginning = iChar;
  word[0] = EOS;

  if (pEl == NULL && context)
    {
      /* search start */
      pEl = context->SStartElement;
      if (!AbsBoxSelectedAttr)
	/* don't change this value when an attribute value is selected */
	iChar = context->SStartChar;
      else if (iChar == 0)
	iChar = 1;
      if (pEl == NULL && context->SDocument)
	pEl = context->SDocument->DocDocElement;
    }

  /* Check that element is a Text element and that the research does */
  /* not start on the last character */
  if (pEl)
    {
      if (pEl->ElTypeNumber != CharString + 1 || iChar >= pEl->ElTextLength)
	{
	  /* there is no buffer */
	  pBuf = NULL;
	  iChar = 1;
	}
      else
	/* first buffer of element */
	pBuf = pEl->ElText;
    }

  /* looks for the end of the research domain */
  if (context == NULL)
    {
      endEl = NULL;
      endChar = 1;
    }
  else
    {
      endEl = context->SEndElement;
      endChar = context->SEndChar;
    }

  while (pBuf == NULL && pEl)
    {
      /* Search the first non-empty and non-protected element */
      pEl = FwdSearchTypedElem (pEl, CharString + 1, NULL, NULL);
      iChar = 1;
      if (pEl)
	{
	  if (endEl && ElemIsBefore (endEl, pEl))
	    /* the element found is after the end of the domain */
	    pEl = NULL;
	}

      if (pEl)
	/* on verifie que cet element ne fait pas partie d'une inclusion et */
	/* n'est pas cache' a l'utilisateur */
	{
	  pAncestor = pEl;
	  while (pAncestor->ElParent && pAncestor->ElSource == NULL)
	    pAncestor = pAncestor->ElParent;
	  if (pAncestor->ElSource == NULL && !ElementIsHidden (pEl) &&
	      pEl->ElTextLength != 0)
	    /* on n'est pas dans une inclusion et l'element n'est pas cache' */
	    /* On saute les elements vides */
	    pBuf = pEl->ElText;
	}
    }

  if (pEl == NULL || iChar < 0)
    return (FALSE);
  else if (pEl == endEl && iChar >= endChar)
    /* On est arrive a la fin du domaine de recherche */
    return (FALSE);
  else
    {
      /* Calcule l'index de depart de la recherche */
      if (iChar > 0)
	len = iChar - 1;
      else
	len = iChar;
      while (len >= pBuf->BuLength)
	{
	  /* On passe au buffer suivant */
	  len -= pBuf->BuLength;
	  if (pBuf->BuNext == NULL)
	    len = pBuf->BuLength - 1;
	  else
	    pBuf = pBuf->BuNext;
	}
      index = len;
      charact = pBuf->BuContent[index];
      /* On se place au debut du mot */
      while (charact != EOS && IsSeparatorChar (charact) &&
	     (pEl != endEl || iChar < endChar))
	{
	  charact = NextCharacter (&pBuf, &index);
	  iChar++;
	}

      /* Recherche le premier separateur apres le mot */
      /* On verifie que l'on ne depasse pas la fin du domaine de recherche */
      len = 0;
      *beginning = iChar;
      while (len < MAX_WORD_LEN-1 && charact != EOS &&
	     !IsSeparatorChar (charact) &&
	     (pEl != endEl || iChar < endChar))
	{
	  word[len++] = charact;
	  charact = NextCharacter (&pBuf, &index);
	  iChar++;
	}

      /* positionne les valeurs de retour */
      word[len] = EOS;
      *curEl = pEl;
      *end = iChar;
      /* Si on a trouve effectivement un mot */
      if (len > 0)
	return (TRUE);
      else
	/* On peut etre en fin de feuille qui se termine par un espace */
	/* On continue la recherche */
	return (SearchNextWord (curEl, beginning, end, word, context));
    }
}


/*----------------------------------------------------------------------
  SearchPreviousWord look for the next word in the search domain from the
  current beginning position.
  Returns the selected word, its beginning and end postion.
  The parameter context points to the search domain.
  ----------------------------------------------------------------------*/
ThotBool SearchPreviousWord (PtrElement *curEl, int *beginning, int *end,
			     CHAR_T *word, PtrSearchContext context)
{
  PtrElement          pEl, endEl;
  PtrElement          pAncestor;
  PtrTextBuffer       pBuf;
  CHAR_T             charact;
  CHAR_T              reverse[MAX_WORD_LEN];
  int                 iChar, endChar, j;
  int                 len;
  int                 index;

  pEl = *curEl;
  iChar = *beginning;
  *end = iChar;
  word[0] = EOS;
  if (pEl == NULL && context)
    {
      /* C'est le debut de la recherche */
      pEl = context->SEndElement;
      if (!AbsBoxSelectedAttr)
	{
	  /* don't change this value when an attribute value is selected */
	  iChar = context->SEndChar;
	  if (iChar > 2)
	    /* La fin de selection pointe toujours sur un caratere plus loin */
	    iChar--;
	}
      if (pEl == NULL && context->SDocument)
	pEl = context->SDocument->DocDocElement;
    }
  iChar--;

  /* Check that element is a Text element and that the research does */
  /* not start on the last character */
  if (pEl)
    {
      if (pEl->ElTypeNumber != CharString + 1 || iChar < 1)
	{
	  /* there is no buffer */
	  pBuf = NULL;
	  iChar = 1;
	}
      else
	/* first buffer of element */
	pBuf = pEl->ElText;
    }

  /* Determine l'element limite du contexte de recherche */
  if (context == NULL)
    {
      endEl = NULL;
      endChar = 1;
    }
  else
    {
      endEl = context->SStartElement;
      endChar = context->SStartChar;
    }

  while (pBuf == NULL && pEl)
    {
      /* Recherche le premier element texte non vide */
      pEl = BackSearchTypedElem (pEl, CharString + 1, NULL, NULL);
      if (pEl)
	{
	  if (endEl && ElemIsBefore (pEl, endEl))
	    /* l'element trouve' est avant l'element de debut, */
	    /* on fait comme si on n'avait pas trouve' */
	    pEl = NULL;
	}

      /* On saute les elements vides */
      if (pEl)
	/* on verifie que cet element ne fait pas partie d'une inclusion et */
	/* n'est pas cache' a l'utilisateur */
	{
	  pAncestor = pEl;
	  while (pAncestor->ElParent && pAncestor->ElSource == NULL)
	    pAncestor = pAncestor->ElParent;
	  if (pAncestor->ElSource == NULL && !ElementIsHidden (pEl) &&
	      pEl->ElTextLength != 0)
	    {
	      /* on n'est pas dans une inclusion et l'element n'est pas cache' */
	      iChar = pEl->ElTextLength;
	      pBuf = pEl->ElText;
	    }
	}
    }
  
  if (pEl == NULL || iChar < 0)
    return (FALSE);
  else if (pEl == endEl && iChar < endChar)
    /* On est arrive a la fin du domaine de recherche */
    return (FALSE);
  else
    {
      /* Calcule l'index de depart de la recherche */
      len = iChar - 1;
      while (len >= pBuf->BuLength)
	{
	  /* On passe au buffer suivant */
	  len -= pBuf->BuLength;
	  if (pBuf->BuNext == NULL)
	    len = pBuf->BuLength - 1;
	  else
	    pBuf = pBuf->BuNext;
	}

      index = len;
      charact = pBuf->BuContent[index];
      /* On se place a la fin du mot */
      while (charact != EOS && iChar >= 0 &&
	     IsSeparatorChar (charact) &&
	     (pEl != endEl || iChar >= endChar))
	{
	  charact = PreviousCharacter (&pBuf, &index);
	  iChar--;
	}

      /* On se place au debut du mot et recupere le mot a l'envers */
      len = 0;
      while (len < MAX_WORD_LEN-1 && charact != EOS && iChar >= 0 &&
	     !IsSeparatorChar (charact) &&
	     (pEl != endEl || iChar >= endChar))
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
      *beginning = iChar;
      *end = iChar + len;
      /* Si on a trouve effectivement un mot */
      if (len > 0)
	return (TRUE);
      else
	/* On peut etre en fin de feuille qui se termine par un espace */
	/* On continue la recherche */
	return (SearchPreviousWord (curEl, beginning, end, word, context));
    }
}
