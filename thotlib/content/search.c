/*
 *
 *  (c) COPYRIGHT INRIA, 1999-2002
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * This module is part of the Thot library.
 */
 
/*
 * Search character strings within documents
 *
 * Author: V. Quint (INRIA)
 *
 */

#include "thot_sys.h"
#include "ustring.h"
#include "constmedia.h"
#include "typemedia.h"
#include "fileaccess.h"
#include "appaction.h"
#include "appstruct.h"
#include "message.h"
#include "typecorr.h"
#include "appdialogue.h"

#define THOT_EXPORT extern
#include "platform_tv.h"
#include "edit_tv.h"
#include "appdialogue_tv.h"

#include "absboxes_f.h"
#include "applicationapi_f.h"
#include "attrpresent_f.h"
#include "boxselection_f.h"
#include "callback_f.h"
#include "changeabsbox_f.h"
#include "createabsbox_f.h"
#include "documentapi_f.h"
#include "exceptions_f.h"
#include "fileaccess_f.h"
#include "memory_f.h"
#include "references_f.h"
#include "schemas_f.h"
#include "scroll_f.h"
#include "search_f.h"
#include "searchmenu_f.h"
#include "structcreation_f.h"
#include "structmodif_f.h"
#include "structschema_f.h"
#include "structselect_f.h"
#include "tree_f.h"
#include "uconvert_f.h"
#include "views_f.h"

/*----------------------------------------------------------------------
   ReplaceString
   remplace dans l'element texte pEl la chaine	
   commencant au caractere firstChar et de longueur stringLen par  
   la chaine replaceStr de longueur replaceLen.                    
   Selectionne la chaine remplace'e si select est True.		
  ----------------------------------------------------------------------*/
void ReplaceString (PtrDocument pDoc, PtrElement pEl, int firstChar,
		    int stringLen, unsigned char *replaceStr,
		    int replaceLen, ThotBool select)
{
  PtrTextBuffer       pBuf1, pBuf2, pBufn;
  PtrAbstractBox      pAb;
  PtrElement          pAsc;
  NotifyOnTarget      notifyEl;
  CHAR_T             *s;
  int                 ibuf1, ibuf2, len, diff, dvol, view, i;
  ThotBool            DontReplace;
  ThotBool            visible;

  if (pEl->ElTypeNumber != CharString + 1)
    return;
  /* envoie l'evenement ElemTextModify.Pre a qui le demande */
  DontReplace = FALSE;
  pAsc = pEl;
  while (pAsc != NULL)
    {
      notifyEl.event = TteElemTextModify;
      notifyEl.document = (Document) IdentDocument (pDoc);
      notifyEl.element = (Element) pAsc;
      notifyEl.target = (Element) pEl;
      notifyEl.targetdocument = (Document) IdentDocument (pDoc);
      DontReplace = DontReplace || CallEventType ((NotifyEvent *) & notifyEl,
						  TRUE);
      pAsc = pAsc->ElParent;
    }
  if (DontReplace)
    return;

  /* buffer of the first character to be replaced */
  pBuf1 = pEl->ElText;
  len = 0;
  while (len + pBuf1->BuLength < firstChar)
    {
      len += pBuf1->BuLength;
      pBuf1 = pBuf1->BuNext;
    }
  /* index of the first character to be replaced */
  ibuf1 = firstChar - len - 1;
  /* buffer of the character after the replaced string */
  pBuf2 = pBuf1;
  while (len + pBuf2->BuLength < firstChar + stringLen)
    {
      len += pBuf2->BuLength;
      pBuf2 = pBuf2->BuNext;
    }
  /* index of the character after the replaced string */
  ibuf2 = firstChar + stringLen - 1 - len;
  if (replaceLen > stringLen)
    /* la chaine de remplacement est plus longue que la chaine */
    /* a remplacer */
    {
      diff = replaceLen - stringLen;
      if (pBuf2->BuLength + diff > FULL_BUFFER)
	/* il n'y a pas assez de place */
	/* ajoute un buffer apres le dernier buffer de la chaine */
	/* a remplacer */
	{
	  /* add a new buffer */
	  pBufn = pBuf2->BuNext;
	  GetTextBuffer (&pBuf2->BuNext);
	  pBuf2->BuNext->BuPrevious = pBuf2;
	  pBuf2->BuNext->BuNext = pBufn;
	  if (pBufn)
	    pBufn->BuPrevious = pBuf2->BuNext;
	  pBufn = pBuf2->BuNext;
	  /* move the end of the buffer into the new buffer */
	  ustrncpy (&pBufn->BuContent[0], &pBuf2->BuContent[ibuf2],
		    pBuf2->BuLength - ibuf2);
	  pBufn->BuLength = pBuf2->BuLength - ibuf2;
	  pBufn->BuContent[pBufn->BuLength] = EOS;
	  pBuf2->BuLength -= pBufn->BuLength;
	  pBuf2->BuContent[pBuf2->BuLength] = EOS;
	}
      else
	{
	  /* move rigth the text that follows the selection */
	  for (i = pBuf2->BuLength - 1; i >= ibuf2; i--)
	    pBuf2->BuContent[i + diff] = pBuf2->BuContent[i];
	}
      pBuf2->BuLength += diff;
      diff = 0;
    }
  else if (replaceLen < stringLen)
    /* la chaine de remplacement est plus courte que la chaine */
    /* a remplacer */
    {
      diff = stringLen - replaceLen;
      if (diff > ibuf2)
	diff = ibuf2;
      /* decale a gauche les caracteres qui suivent la chaine */
      /* a remplacer */
      for (i = ibuf2 - diff; i < pBuf2->BuLength + 1 - diff; i++)
	pBuf2->BuContent[i] = pBuf2->BuContent[i + diff];
      pBuf2->BuLength -= diff;
      diff = stringLen - replaceLen - diff;
    }
  else
    /* les deux chaines ont meme longueur */
    diff = 0;

  /* copy the remplacing string (dialogue is supposed ISO_8859_1) */
  s = TtaConvertIsoToCHAR (replaceStr, ISO_8859_1);
  len = 0;
  pBufn = pBuf1;
  i = ibuf1;
  while (len < replaceLen)
    {
      pBufn->BuContent[i] = replaceStr[len++];
      i++;
      if (i > pBufn->BuLength)
	{
	  pBufn->BuContent[i] = EOS;
	  pBufn->BuLength = i - 1;
	  pBufn = pBufn->BuNext;
	  i = 0;
	}
    }
  TtaFreeMemory (s);

  if (diff > 0)
    {
      pBufn->BuContent[i] = EOS;
      pBufn->BuLength = i;
    }
  /* met a jour le volume de l'element */
  dvol = replaceLen - stringLen;
  pEl->ElTextLength = pEl->ElTextLength + dvol;
  pEl->ElVolume = pEl->ElTextLength;
  /* met a jour le volume de tous les elements ascendants */
  pAsc = pEl->ElParent;
  while (pAsc)
    {
      pAsc->ElVolume = pAsc->ElVolume + dvol;
      pAsc = pAsc->ElParent;
    }
  /* change le volume des paves de l'element modifie' */
  visible = FALSE;
  for (view = 0; view < MAX_VIEW_DOC; view++)
    {
      pAb = pEl->ElAbstractBox[view];
      if (pAb != NULL)
	{
	  visible = TRUE;
	  pAb->AbChange = TRUE;
	  pDoc->DocViewModifiedAb[view] = Enclosing (pAb,
						pDoc->DocViewModifiedAb[view]);
	  dvol = pAb->AbVolume - pEl->ElTextLength;
	  do
	    {
	      /* met a jour le volume du premier pave */
	      pAb->AbVolume -= dvol;
	      pAb = pAb->AbEnclosing;
	    }
	  while (pAb != NULL);
	}
    }
  /* reaffiche toutes les views */
  TtaClearViewSelections ();
  if (visible)
    {
      /* Update Abstract views */
      if (ThotLocalActions[T_AIupdate] != NULL)
	(*ThotLocalActions[T_AIupdate]) (pDoc);
      /* Redisplay views */
      if (ThotLocalActions[T_redisplay] != NULL)
	(*ThotLocalActions[T_redisplay]) (pDoc);
    }

  /* si l'element TEXTE modifie' appartient soit a un element copie' */
  /* dans des paves par une regle Copy, soit a un element inclus */
  /* dans d'autres, il faut reafficher ses copies */
  RedisplayCopies (pEl, pDoc, TRUE);

  /* selectionne la chaine remplacee */
  if (select)
    {
      if (replaceLen < 2)
	i = firstChar;
      else
	i = firstChar + replaceLen;
      SelectStringWithEvent (pDoc, pEl, firstChar, i);
    }
  /* le document a ete modifie' */
  SetDocumentModified (pDoc, TRUE, replaceLen);
  /* envoie l'evenement ElemTextModify.Post a qui le demande */
  pAsc = pEl;
  while (pAsc != NULL)
    {
      notifyEl.event = TteElemTextModify;
      notifyEl.document = (Document) IdentDocument (pDoc);
      notifyEl.element = (Element) pAsc;
      notifyEl.target = (Element) pEl;
      notifyEl.targetdocument = (Document) IdentDocument (pDoc);
      CallEventType ((NotifyEvent *) & notifyEl, FALSE);
      pAsc = pAsc->ElParent;
    }
}

/*----------------------------------------------------------------------
   EquivalentChar compare les caracteres c1 et c2, en confondant   
   les majuscules et minuscules si caseEquiv est vrai.        
  ----------------------------------------------------------------------*/
static ThotBool EquivalentChar (CHAR_T c1, CHAR_T c2, ThotBool caseEquiv)
{
  int                 v1, v2;

  if (caseEquiv)
    {
      v1 = (int) c1;
      v2 = (int) c2;
      if ((v1 >= 65 && v1 <= 90) || (v1 >= 192 && v1 <= 294))
	c1 = (CHAR_T) (v1 + 32);
      if ((v2 >= 65 && v2 <= 90) || (c2 >= 192 && c2 <= 294))
	c2 = (CHAR_T) (v2 + 32);
    }
  return (c1 == c2);
}


/*----------------------------------------------------------------------
   SameString rend vrai si la chaine cherchee est a` la position   
   indiquee par (pBuf,ind).                                
  ----------------------------------------------------------------------*/
static ThotBool SameString (PtrTextBuffer pBuf, int ind, ThotBool caseEquiv,
			    CHAR_T *strng, int strngLen)
{
  ThotBool            equal, stop;
  int                 ind2;

  equal = FALSE;
  stop = FALSE;
  ind2 = 1;
  while (!stop)
    {
      while (pBuf != NULL && pBuf->BuLength <= ind)
	{
	  ind -= pBuf->BuLength;
	  pBuf = pBuf->BuNext;
	}
      if (pBuf == NULL || ind2 > strngLen)
	{
	  equal = ind2 > strngLen;
	  stop = True;
	}
      else if (!EquivalentChar (pBuf->BuContent[ind], strng[ind2 - 1],
				caseEquiv))
	stop = True;
      else
	{
	  ind++;
	  ind2++;
	}
    }
  return equal;
}

/*----------------------------------------------------------------------
   ContentAndStringEqual teste qu'on est bien positionne' sur une chaine
   de texte  egale a` la chaine cherchee.                            
  ----------------------------------------------------------------------*/
ThotBool ContentAndStringEqual (PtrElement firstEl, int firstChar,
				PtrElement lastEl, int lastChar,
				ThotBool caseEquiv,
				CHAR_T *strng, int strngLen)
{
  PtrTextBuffer       pBuf;
  ThotBool            ok;

  ok = FALSE;
  if (firstEl == lastEl && firstEl != NULL)
    if (firstEl->ElTerminal && firstEl->ElLeafType == LtText)
      {
	pBuf = firstEl->ElText;
	firstChar--;
	while (pBuf != NULL && firstChar > pBuf->BuLength)
	  {
	    firstChar -= pBuf->BuLength;
	    pBuf = pBuf->BuNext;
	  }
	ok = SameString (pBuf, firstChar, caseEquiv, strng, strngLen);
      }
  return ok;
}

/*----------------------------------------------------------------------
   FwdSearchString recherche une chaine en avant.                  
   Attention: On ne trouve pas la chaine cherchee si elle est      
   coupee par une marque de page ou par un changement de feuille   
   texte du a des regles de presentation specifiques ou des        
   attributs differents.                                           
  ----------------------------------------------------------------------*/
static void FwdSearchString (PtrTextBuffer pBuf, int ind, ThotBool *found,
			     int *firstChar, ThotBool caseEquiv,
			     CHAR_T *strng)
{
  int                 ir;
  int                 ix;
  int                 icx;
  PtrTextBuffer       pBufx;
  ThotBool            stop;

  stop = FALSE;
  *found = FALSE;
  ix = -1;
  ir = 0;
  icx = 0;
  pBufx = NULL;
  /* index dans ChaineCherchee du caractere a comparer */
  do
    {
      if (pBuf->BuContent[ind] == EOS || ind >= pBuf->BuLength)
	{
	  pBuf = pBuf->BuNext;
	  ind = 0;
	  while (pBuf && pBuf->BuLength == 0)
	    /* skip the empty buffer */
	    pBuf = pBuf->BuNext;
	  stop = (pBuf == NULL);
	}
      if (!stop)
	{
	  if (EquivalentChar (pBuf->BuContent[ind], strng[ir], caseEquiv))
	    {
	      if (ir == 0)
		{
		  /* remember the current position in buffers */
		  ix = ind;
		  pBufx = pBuf;
		  icx = *firstChar;
		}
	      /* check if the end of the string is reached */
	      ir++;
	      if (strng[ir] == EOS)
		{
		  *found = TRUE;
		  stop = TRUE;
		}
	      else
		ind++;
	    }
	  else if (pBufx)
	    {
	      /* restart the search */
	      ind = ix + 1;
	      pBuf = pBufx;
	      *firstChar = icx + 1;
	      pBufx = NULL;
	      ir = 0;
	    }
	  else
	    {
	      ind++;
	      (*firstChar)++;
	    }
	}
    }
  while (!stop);
}

/*----------------------------------------------------------------------
   BackSearchString recherche une chaine en arriere.               
  ----------------------------------------------------------------------*/
static void BackSearchString (PtrTextBuffer pBuf, int ind, ThotBool *found,
			      int *firstChar, ThotBool caseEquiv,
			      CHAR_T *strng, int strngLen)
{
  int                 ir;
  int                 ix;
  int                 icx;
  PtrTextBuffer       pBufx;
  ThotBool            stop;

  stop = FALSE;
  *found = FALSE;
  ix = -1;
  strngLen--;
  ir = strngLen;
  icx = 0;
  pBufx = NULL;
  /* index dans ChaineCherchee du caractere a comparer */
  do
    {
      if (ind < 0)
	{
	  pBuf = pBuf->BuPrevious;
	  while (pBuf && pBuf->BuLength == 0)
	    /* skip the empty buffer */
	    pBuf = pBuf->BuPrevious;
	  stop = (pBuf == NULL);
	  if (pBuf != NULL)
	    ind = pBuf->BuLength - 1;
	}
      if (!stop)
	{
	  if (EquivalentChar (pBuf->BuContent[ind], strng[ir], caseEquiv))
	    {
	      if (ir == strngLen)
		{
		  /* remember the current position in buffers */
		  ix = ind;
		  pBufx = pBuf;
		  icx = *firstChar;
		}
	      /* check if the beginning of the string is reached */
	      if (ir == 0)
		{
		  *found = TRUE;
		  stop = TRUE;
		}
	      else
		{
		  ind--;
		  ir--;
		}
	    }
	  else if (pBufx)
	    {
	      /* restart the search */
	      ind = ix - 1;
	      pBuf = pBufx;
	      *firstChar = icx - 1;
	      pBufx = NULL;
	      ir = strngLen;
	    }
	  else
	    {
	      ind--;
	      (*firstChar)--;
	    }
	}
    }
  while (!stop);
}

/*----------------------------------------------------------------------
   SearchText looks for the string described by text and textLen.
   Parameters firstEl, firstChar, lastEl, lastChar give the search context.
   Au retour, elles indiquent la chaine trouvee si la fonction     
   retourne Vrai. (La fonction retourne faux en cas d'echec).      
  ----------------------------------------------------------------------*/
ThotBool SearchText (PtrDocument pDoc, PtrElement *firstEl, int *firstChar,
		     PtrElement *lastEl, int *lastChar, ThotBool forward,
		     ThotBool caseEquiv, unsigned char *text,
		     int textLen)
{
  PtrElement          pEl;
  PtrElement          pAncest;
  PtrTextBuffer       pBuf;
  CHAR_T             *tmp;
  int                 i;
  int                 ibuf;
  int                 ichar;
  ThotBool            found;
  ThotBool            result;

  result = FALSE;
  if (*firstEl)
    {
      /* on n'a pas encore trouve' la chaine cherchee */
      found = FALSE;
      pEl = *firstEl;
      if (pEl->ElStructSchema == NULL)
	/* the element doesn't exist anymore */
	return FALSE;
      /* on cherche d'abord la chaine */
      tmp = TtaConverMbsToCHAR (text);
      if (forward)
	/* Recherche en avant */
	{
	  /* lastChar designe le caractere qui suit le dernier caractere */
	  /* selectionne' */
	  if (pEl->ElTerminal && pEl->ElLeafType == LtText)
	    if (*firstChar >= 1 && *firstChar <= pEl->ElTextLength)
	      /* commencer la recherche dans l'element courant */
	      /*  cherche le buffer du dernier caractere selectionne' */
	      {
		pBuf = pEl->ElText;
		i = 0;
		while (i + pBuf->BuLength < *firstChar)
		  {
		    i += pBuf->BuLength;
		    pBuf = pBuf->BuNext;
		  }
		/* index dans le buffer de texte pBuf du 1er caractere
		   a tester */
		ibuf = *firstChar - 1 - i;
		ichar = *firstChar;
		FwdSearchString (pBuf, ibuf, &found, &ichar, caseEquiv, tmp);
	      }
	  while (!found && pEl)
	    {
	      pEl = FwdSearchTypedElem (pEl, CharString + 1, NULL);
	      if (pEl != NULL)
		/* on a trouve un element de texte */
		{
		  /* on verifie que cet element ne fait pas partie d'une */
		  /* inclusion */
		  pAncest = pEl;
		  while (pAncest->ElParent != NULL &&
			 pAncest->ElSource == NULL)
		    pAncest = pAncest->ElParent;
		  if (pAncest->ElSource == NULL)
		    /* on n'est pas dans une inclusion */
		    if (!ElementIsHidden (pEl))
		      /* l'element n'est pas dans une partie cachee */
		      {
			ichar = 1;
			FwdSearchString (pEl->ElText, 0, &found, &ichar,
					 caseEquiv, tmp);
		      }
		}
	    }
	}
      else
	/* Recherche en arriere */
	{
	  if (pEl->ElTerminal && pEl->ElLeafType == LtText)
	    if (*firstChar >= 2)
	      /* commencer la recherche dans l'element courant */
	      /* cherche le buffer du premier caractere selectionne' */
	      {
		pBuf = pEl->ElText;
		i = 0;
		while (i + pBuf->BuLength < *firstChar - 1)
		  {
		    i += pBuf->BuLength;
		    pBuf = pBuf->BuNext;
		  }
		ibuf = *firstChar - i - 1;
		/* index dans le buffer de texte */
		/* pointe par pBuf du 1er caractere a tester */
		ichar = *firstChar;
		BackSearchString (pBuf, ibuf, &found, &ichar, caseEquiv,
				  tmp, textLen);
	      }
	  while (!found && pEl != NULL)
	    {
	      pEl = BackSearchTypedElem (pEl, CharString + 1, NULL);
	      if (pEl)
		/* on a trouve' un element de texte */
		{
		  /* on verifie que cet element ne fait pas partie d'une */
		  /* inclusion */
		  pAncest = pEl;
		  while (pAncest->ElParent != NULL &&
			 pAncest->ElSource == NULL)
		    pAncest = pAncest->ElParent;
		  if (pAncest->ElSource == NULL)
		    /* on n'est pas dans une inclusion */
		    if (!ElementIsHidden (pEl))
		      /* l'element n'est pas dans une partie cachee */
		      {
			pBuf = pEl->ElText;
			while (pBuf->BuNext != NULL)
			  pBuf = pBuf->BuNext;
			ichar = pEl->ElTextLength;
			BackSearchString (pBuf, pBuf->BuLength - 1, &found, &ichar,
					  caseEquiv, tmp, textLen);
		      }
		}
	    }
	  if (found)
	    ichar = ichar - textLen + 1;
	}
      TtaFreeMemory (tmp);
      if (found)
	/* on a trouve' la chaine cherchee */
	/* l'element trouve' est pointe' par pEl et ichar est le rang */
	/* dans cet element du 1er caractere de la chaine trouvee */
	if (*lastEl != NULL)
	  {
	    /* il faut s'arreter avant l'extremite' du document */
	    if (pEl == *lastEl)
	      /* la chaine trouvee est dans l'element ou il faut s'arreter */
	      {
		if (forward)
		  {
		    if (ichar + textLen - 1 > *lastChar)
		      /* la chaine trouvee se termine au-dela du caractere ou
		         il faut s'arreter, on fait comme si on n'avait pas
			 trouve' */
		      found = FALSE;
		  }
		else
		  {
		    if (*lastChar > 0)
		      if (ichar < *lastChar)
			found = FALSE;
		  }
	      }
	    else if (forward)
	      {
		if (ElemIsBefore (*lastEl, pEl))
		  /* l'element trouve' est apres l'element de fin, on fait */
		  /* comme si on n'avait pas trouve' */
		  found = FALSE;
	      }
	    else if (ElemIsBefore (pEl, *lastEl))
	      found = FALSE;
	  }
      if (found)
	{
	  *firstEl = pEl;
	  *lastEl = pEl;
	  *firstChar = ichar;
	  /* *lastChar est le rang du caractere qui suit le dernier */
	  /* caractere de la chaine trouvee dans le buffer */
	  *lastChar = ichar + textLen;
	  result = TRUE;
	}
    }
  return result;
}

/*----------------------------------------------------------------------
   SearchPageBreak recherche a partir de l'element pEl un element
   	BreakPage concernant la vue view. Si relative est faux, pageNum
   	represent le numero de la page cherchee, sinon, c'est le nombre
   	de pages a sauter (ce nombre peut etre negatif, pour un saut
   	en arriere).
  ----------------------------------------------------------------------*/
PtrElement SearchPageBreak (PtrElement pEl, int view, int pageNum,
			    ThotBool relative)
{
  PtrElement          pElP;
  PtrElement          result;

  pElP = pEl;
  result = pEl;
  if (pageNum > 0)
    {
      if (relative)
	/* c'est un deplacement relatif. Si on est sur une page, on */
	/* l'ignore et on commence la recherche a partir du suivant */
	if (pEl->ElTypeNumber == PageBreak + 1)
	  pEl = NextElement (pEl);
      pElP = pEl;
      while (pageNum > 0 && pElP != NULL)
	{
	  pElP = FwdSearchTypedElem (pEl, PageBreak + 1, NULL);
	  if (pElP != NULL)
	    {
	      if (pElP->ElViewPSchema == view)
		{
		  if (relative)
		    {
		      result = pElP;
		      pageNum--;
		    }
		  else if (pElP->ElPageNumber == pageNum)
		    /* on cherche la page de numero pageNum */
		    /* c'est la page cherchee */
		    {
		      result = pElP;
		      pageNum = 0;
		    }
		}
	      pEl = pElP;
	    }
	}
    }
  else
    while (pageNum < 0 && pElP != NULL)
      {
	pElP = BackSearchTypedElem (pEl, PageBreak + 1, NULL);
	if (pElP != NULL)
	  {
	    if (pElP->ElViewPSchema == view)
	      {
		result = pElP;
		pageNum++;
	      }
	    pEl = pElP;
	  }
      }
  return result;
}

/*----------------------------------------------------------------------
   ScrollPageToTop positionne le document pDoc dans sa frame       
   de facon que le filet du saut de page pPage soit en	
   haut de la fenetre.                                     
   view indique la vue concernee (vue du document).      
  ----------------------------------------------------------------------*/
void ScrollPageToTop (PtrElement pPage, int view, PtrDocument pDoc)
{
  PtrAbstractBox      pAbGraph;
  PtrAbstractBox      pAb;
  int                 frame;

  /* le code qui suit etait precedemment dans MoveToPage */
  if (pPage != NULL)
    {
      CheckAbsBox (pPage, view, pDoc, TRUE, FALSE);
      if (pPage->ElAbstractBox[view - 1] != NULL)
	{
	  pAb = pPage->ElAbstractBox[view - 1]->AbFirstEnclosed;
	  pAbGraph = NULL;
	  while (pAb != NULL)
	    /* recherche du filet */
	    if (pAb->AbLeafType == LtGraphics &&
		pAb->AbShape == 'h' &&
		pAb->AbHorizPos.PosAbRef == NULL)
	      {
		pAbGraph = pAb;
		pAb = NULL;
	      }
	    else
	      pAb = pAb->AbNext;
	  if (pAbGraph != NULL)
	    {
	      /* fenetre correspondant a la vue */
	      frame = pDoc->DocViewFrame[view - 1];
	      ShowBox (frame, pAbGraph->AbBox, 0, 0);
	    }
	}
    }
}
