/*
 *
 *  (c) COPYRIGHT INRIA, 1999.
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
#include "viewcommands_f.h"
#include "views_f.h"

/*----------------------------------------------------------------------
   ReplaceString
   remplace dans l'element texte pEl la chaine	
   commencant au caractere firstChar et de longueur stringLen par  
   la chaine replaceStr de longueur replaceLen.                    
   Selectionne la chaine remplace'e si select est True.		
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                ReplaceString (PtrDocument pDoc, PtrElement pEl, int firstChar, int stringLen, CHAR_T replaceStr[THOT_MAX_CHAR], int replaceLen, ThotBool select)

#else  /* __STDC__ */
void                ReplaceString (pDoc, pEl, firstChar, stringLen, replaceStr, replaceLen, select)
PtrDocument         pDoc;
PtrElement          pEl;
int                 firstChar;
int                 stringLen;
CHAR_T                replaceStr[THOT_MAX_CHAR];
int                 replaceLen;
ThotBool            select;

#endif /* __STDC__ */

{
   int                 ibuf1, ibuf2, len, diff, dvol, view, i;
   PtrTextBuffer       pBuf1, pBuf2, pBufn;
   PtrAbstractBox      pAb;
   PtrElement          pAsc;
   NotifyOnTarget      notifyEl;
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
	DontReplace = DontReplace || CallEventType ((NotifyEvent *) & notifyEl, TRUE);
	pAsc = pAsc->ElParent;
     }
   if (DontReplace)
      return;

   /* cherche le buffer du premier caractere a remplacer: pBuf1 */
   pBuf1 = pEl->ElText;
   len = 0;
   /* longueur cumulee des buffers de texte precedant le */
   /* buffer contenant le 1er caractere */
   while (len + pBuf1->BuLength < firstChar)
     {
	len += pBuf1->BuLength;
	pBuf1 = pBuf1->BuNext;
     }
   ibuf1 = firstChar - len;
   /* index dans le buffer de texte pointe par pBuf1 */
   /* du 1er caractere a remplacer */
   /* cherche le buffer du dernier caractere a remplacer: pBuf2 */
   pBuf2 = pBuf1;
   while (len + pBuf2->BuLength < firstChar + stringLen - 1)
     {
	len += pBuf2->BuLength;
	pBuf2 = pBuf2->BuNext;
     }
   ibuf2 = firstChar + stringLen - 1 - len;
   /* index dans le buffer de texte */
   /* pointe' par pBuf2 du dernier caractere a remplacer */
   if (replaceLen > stringLen)
      /* la chaine de remplacement est plus longue que la chaine */
      /* a remplacer */
     {
	diff = replaceLen - stringLen;
	if (pBuf2->BuLength + diff > THOT_MAX_CHAR - 1)
	   /* il n'y a pas assez de place */
	   /* ajoute un buffer apres le dernier buffer de la chaine */
	   /* a remplacer */
	  {
	     pBufn = pBuf2->BuNext;
	     GetTextBuffer (&pBuf2->BuNext);
	     pBuf2->BuNext->BuPrevious = pBuf2;
	     pBuf2->BuNext->BuNext = pBufn;
	     if (pBufn != NULL)
		pBufn->BuPrevious = pBuf2->BuNext;
	     pBufn = pBuf2->BuNext;
	     /* recopie la fin du buffer dans le nouveau */
	     for (i = 0; i <= pBuf2->BuLength + diff - THOT_MAX_CHAR; i++)
		pBufn->BuContent[i] = pBuf2->BuContent[THOT_MAX_CHAR - 1 - diff + i];
	     pBufn->BuLength = pBuf2->BuLength + diff - THOT_MAX_CHAR + 1;
	     pBufn->BuContent[pBufn->BuLength] = EOS;
	     pBuf2->BuLength -= pBufn->BuLength;
	     pBuf2->BuContent[pBuf2->BuLength] = EOS;
	  }
	/* decale a droite les caracteres qui suivent la chaine */
	/* a remplacer dans le buffer pBuf2 */
	for (i = pBuf2->BuLength; i >= ibuf2; i--)
	   pBuf2->BuContent[i + diff] = pBuf2->BuContent[i];
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
   /* copie la chaine de remplacement */
   len = 0;
   pBufn = pBuf1;
   i = ibuf1;
   while (len < replaceLen)
     {
	pBufn->BuContent[i - 1] = replaceStr[len++];
	i++;
	if (i > pBufn->BuLength)
	  {
	     pBufn->BuContent[i - 1] = EOS;
	     pBufn->BuLength = i - 1;
	     pBufn = pBufn->BuNext;
	     i = 1;
	  }
     }
   if (diff > 0)
     {
	pBufn->BuContent[i - 1] = EOS;
	pBufn->BuLength = i - 1;
     }
   /* met a jour le volume de l'element */
   dvol = replaceLen - stringLen;
   pEl->ElTextLength = pEl->ElTextLength + dvol;
   pEl->ElVolume = pEl->ElTextLength;
   /* met a jour le volume de tous les elements ascendants */
   pAsc = pEl->ElParent;
   while (pAsc != NULL)
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
	     if (!AssocView (pEl))
		pDoc->DocViewModifiedAb[view] = Enclosing (pAb, pDoc->DocViewModifiedAb[view]);
	     else
		pDoc->DocAssocModifiedAb[pEl->ElAssocNum - 1] =
		   Enclosing (pAb, pDoc->DocAssocModifiedAb[pEl->ElAssocNum - 1]);
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
	   i = firstChar + replaceLen - 1;
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

#ifdef __STDC__
static ThotBool     EquivalentChar (CHAR_T c1, CHAR_T c2, ThotBool caseEquiv)

#else  /* __STDC__ */
static ThotBool     EquivalentChar (c1, c2, caseEquiv)
CHAR_T                c1;
CHAR_T                c2;
ThotBool            caseEquiv;

#endif /* __STDC__ */

{
   if (caseEquiv)
     {
	if ((c1 >= TEXT('A') && c1 <= TEXT('Z')) || (c1 >= TEXT('\300') && c1 <= TEXT('\336')))
	   c1 = (CHAR_T) ((int) (c1) + 32);
	if ((c2 >= TEXT('A') && c2 <= TEXT('Z')) || (c2 >= TEXT('\300') && c2 <= TEXT('\336')))
	   c2 = (CHAR_T) ((int) (c2) + 32);
     }
   return (c1 == c2);
}


/*----------------------------------------------------------------------
   SameString rend vrai si la chaine cherchee est a` la position   
   indiquee par (pBuf,ind).                                
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static ThotBool     SameString (PtrTextBuffer pBuf, int ind, ThotBool caseEquiv, CHAR_T strng[THOT_MAX_CHAR], int strngLen)

#else  /* __STDC__ */
static ThotBool     SameString (pBuf, ind, caseEquiv, strng, strngLen)
PtrTextBuffer       pBuf;
int                 ind;
ThotBool            caseEquiv;
CHAR_T                strng[THOT_MAX_CHAR];
int                 strngLen;

#endif /* __STDC__ */
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
	else if (!EquivalentChar (pBuf->BuContent[ind], strng[ind2 - 1], caseEquiv))
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
   ContentAndStringEqual teste qu'on est bien positionne' sur une chaine de texte 
   egale a` la chaine cherchee.                            
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool            ContentAndStringEqual (PtrElement firstEl, int firstChar, PtrElement lastEl, int lastChar, ThotBool caseEquiv, CHAR_T strng[THOT_MAX_CHAR], int strngLen)
#else  /* __STDC__ */
ThotBool            ContentAndStringEqual (firstEl, firstChar, lastEl, lastChar, caseEquiv, strng, strngLen)
PtrElement          firstEl;
int                 firstChar;
PtrElement          lastEl;
int                 lastChar;
ThotBool            caseEquiv;
CHAR_T                strng[THOT_MAX_CHAR];
int                 strngLen;

#endif /* __STDC__ */
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

#ifdef __STDC__
static void         FwdSearchString (PtrTextBuffer pBuf, int ind, ThotBool * found, int *firstChar, ThotBool caseEquiv, CHAR_T strng[THOT_MAX_CHAR])

#else  /* __STDC__ */
static void         FwdSearchString (pBuf, ind, found, firstChar, caseEquiv, strng)
PtrTextBuffer       pBuf;
int                 ind;
ThotBool           *found;
int                *firstChar;
ThotBool            caseEquiv;
CHAR_T                strng[THOT_MAX_CHAR];

#endif /* __STDC__ */

{
   int                 ir;
   int                 ix;
   int                 icx;
   PtrTextBuffer       pBufx;
   ThotBool            stop;

   stop = FALSE;
   ix = 0;
   ir = 1;
   icx = 0;
   pBufx = NULL;
   /* index dans ChaineCherchee du caractere a comparer */
   do
     {
	if (pBuf->BuContent[ind - 1] == EOS)
	  {
	     pBuf = pBuf->BuNext;
	     if (pBuf != NULL)
		/* saute les buffers vides */
	       {
		  ind = 0;
		  do
		     if (pBuf->BuLength > 0)
			/* buffer non vide */
			ind = 1;
		     else
			/* buffer vide */
		       {
			  /* buffer suivant */
			  pBuf = pBuf->BuNext;
			  /* fin si dernier buffer */
			  stop = pBuf == NULL;
		       }
		  while (!stop && ind != 1);
	       }
	     else
		stop = TRUE;
	  }
	if (!stop)
	   if (EquivalentChar (pBuf->BuContent[ind - 1], strng[ir - 1], caseEquiv))
	     {
		if (ir == 1)
		  {
		     ix = ind;
		     pBufx = pBuf;
		     icx = *firstChar;
		  }
		if (strng[ir] == EOS)
		  {
		     *found = TRUE;
		     stop = TRUE;
		  }
		else
		  {
		     ind++;
		     ir++;
		  }
	     }
	   else if (ix != 0)
	     {
		ind = ix + 1;
		pBuf = pBufx;
		*firstChar = icx + 1;
		ix = 0;
		ir = 1;
	     }
	   else
	     {
		ind++;
		(*firstChar)++;
	     }
     }
   while (!stop);
}


/*----------------------------------------------------------------------
   BackSearchString recherche une chaine en arriere.               
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static void         BackSearchString (PtrTextBuffer pBuf, int ind, ThotBool * found, int *firstChar, ThotBool caseEquiv, CHAR_T strng[THOT_MAX_CHAR], int strngLen)

#else  /* __STDC__ */
static void         BackSearchString (pBuf, ind, found, firstChar, caseEquiv, strng, strngLen)
PtrTextBuffer       pBuf;
int                 ind;
ThotBool           *found;
int                *firstChar;
ThotBool            caseEquiv;
CHAR_T                strng[THOT_MAX_CHAR];
int                 strngLen;

#endif /* __STDC__ */

{
   int                 ir;
   int                 ix;
   int                 icx;
   PtrTextBuffer       pBufx;
   ThotBool            stop;

   stop = FALSE;
   ix = 0;
   ir = strngLen;
   icx = 0;
   pBufx = NULL;
   /* index dans ChaineCherchee du caractere a comparer */
   do
     {
	if (ind < 1)
	  {
	     pBuf = pBuf->BuPrevious;
	     if (pBuf != NULL)
		ind = pBuf->BuLength;
	     else
		stop = TRUE;
	  }
	if (!stop && ind > 0)
	   if (EquivalentChar (pBuf->BuContent[ind - 1], strng[ir - 1], caseEquiv))
	     {
		if (ir == strngLen)
		  {
		     ix = ind;
		     pBufx = pBuf;
		     icx = *firstChar;
		  }
		if (ir == 1)
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
	   else if (ix != 0)
	     {
		ind = ix - 1;
		pBuf = pBufx;
		*firstChar = icx - 1;
		ix = 0;
		ir = strngLen;
	     }
	   else
	     {
		ind--;
		(*firstChar)--;
	     }
     }
   while (!stop);
}


/*----------------------------------------------------------------------
   SearchText recherche dans le document pDoc la chaine de		
   caracteres decrite par les variables strng et strngLen.		
   A l'appel, les variables firstEl, firstChar, lastEl,            
   lastChar indiquent la region ou on cherche.                     
   Au retour, elles indiquent la chaine trouvee si la fonction     
   retourne Vrai. (La fonction retourne faux en cas d'echec).      
  ----------------------------------------------------------------------*/

#ifdef __STDC__
ThotBool            SearchText (PtrDocument pDoc, PtrElement * firstEl, int *firstChar, PtrElement * lastEl, int *lastChar, ThotBool forward, ThotBool caseEquiv, CHAR_T strng[THOT_MAX_CHAR], int strngLen)

#else  /* __STDC__ */
ThotBool            SearchText (pDoc, firstEl, firstChar, lastEl, lastChar, forward, caseEquiv, strng, strngLen)
PtrDocument         pDoc;
PtrElement         *firstEl;
int                *firstChar;
PtrElement         *lastEl;
int                *lastChar;
ThotBool            forward;
ThotBool            caseEquiv;
CHAR_T                strng[THOT_MAX_CHAR];
int                 strngLen;

#endif /* __STDC__ */

{

   PtrElement          pEl;
   int                 i;
   int                 ibuf;
   int                 ichar;
   ThotBool            found;
   PtrTextBuffer       pBuf;
   ThotBool            result;
   PtrElement          pAncest;

   result = FALSE;
   if (*firstEl != NULL)
     {
	/* on n'a pas encore trouve' la chaine cherchee */
	found = FALSE;
	pEl = *firstEl;
	if (pEl->ElStructSchema == NULL)
	  /* the element doesn't exist anymore */
	  return FALSE;
	/* on cherche d'abord la chaine */
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
		     /* index dans le buffer de texte pBuf du 1er caractere a tester */
		     ibuf = *firstChar - i;
		     ichar = *firstChar;
		     FwdSearchString (pBuf, ibuf, &found, &ichar, caseEquiv, strng);
		  }
	     while (!found && pEl != NULL)
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
			       FwdSearchString (pEl->ElText, 1, &found, &ichar, caseEquiv,
						strng);
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
		     ichar = *firstChar - 1;
		     BackSearchString (pBuf, ibuf, &found, &ichar, caseEquiv,
				       strng, strngLen);
		  }
	     while (!found && pEl != NULL)
	       {
		  pEl = BackSearchTypedElem (pEl, CharString + 1, NULL);
		  if (pEl != NULL)
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
			       BackSearchString (pBuf, pBuf->BuLength, &found, &ichar, caseEquiv,
						 strng, strngLen);
			    }
		    }
	       }
	     if (found)
		ichar = ichar - strngLen + 1;
	  }
	if (found)
	   /* on a trouve' la chaine cherchee */
	   /* l'element trouve' est pointe' par pEl et ichar est le rang */
	   /* dans cet element du 1er caractere de la chaine trouvee */
	   if (*lastEl != NULL)
	      /* il faut s'arreter avant l'extremite' du document */
	      if (pEl == *lastEl)
		 /* la chaine trouvee est dans l'element ou il faut s'arreter */
		{
		   if (forward)
		     {
			if (ichar + strngLen - 1 > *lastChar)
			   /* la chaine trouvee se termine au-dela du caractere ou il */
			   /* faut s'arreter, on fait comme si on n'avait pas trouve' */
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
	if (found)
	  {
	     *firstEl = pEl;
	     *lastEl = pEl;
	     *firstChar = ichar;
	     /* *lastChar est le rang du caractere qui suit le dernier */
	     /* caractere de la chaine trouvee dans le buffer */
	     *lastChar = ichar + strngLen;
	     result = TRUE;
	  }
     }
   return result;
}


#define MAX_NAT_TABLE 10
/*----------------------------------------------------------------------
   AddNatureToTable      met le schema de structure pSS dans la	
   table natureTable et retourne dans LgTableNat le nombre		
   d'entrees de la table.                                          
   Si onlyOne est vrai, on ne met chaque schema qu'une fois.	
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static void         AddNatureToTable (PtrSSchema pSS, PtrSSchema natureTable[MAX_NAT_TABLE], int *natureTableLen, ThotBool onlyOne)

#else  /* __STDC__ */
static void         AddNatureToTable (pSS, natureTable, natureTableLen, onlyOne)
PtrSSchema          pSS;
PtrSSchema          natureTable[MAX_NAT_TABLE];
int                *natureTableLen;
ThotBool            onlyOne;

#endif /* __STDC__ */

{
   int                 n;
   ThotBool            present;

   present = FALSE;
   if (onlyOne)
     {
	/* verifie que ce schema n'est pas deja dans la table */
	n = 0;
	while (n < *natureTableLen && !present)
	   if (natureTable[n++]->SsCode == pSS->SsCode)
	      present = TRUE;
     }
   if (!present)
      /* le schema n'est pas dans la table */
     {
	/* met le schema dans la table si elle n'est pas pleine */
	if (*natureTableLen < MAX_NAT_TABLE)
	   natureTable[(*natureTableLen)++] = pSS;
	/* cherche les natures utilisees par ce schema */
	SearchNatures (pSS, natureTable, natureTableLen, onlyOne);
     }
}


/*----------------------------------------------------------------------
   SearchNatures cherche toutes les natures explicitement              
   mentionnees dans le schema de structure pSS et		
   effectivement utilises, ainsi que les schemas de nature 
   charge's dynamiquement et les extensions de ces schemas.
   Met ces schemas dans la table natureTable et retourne	
   retourne dans natureTableLen le nombre d'entrees de la  
   table.                                                  
   Si onlyOne est vrai, on ne met chaque schema qu'une fois
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                SearchNatures (PtrSSchema pSS, PtrSSchema natureTable[MAX_NAT_TABLE], int *natureTableLen, ThotBool onlyOne)

#else  /* __STDC__ */
void                SearchNatures (pSS, natureTable, natureTableLen, onlyOne)
PtrSSchema          pSS;
PtrSSchema          natureTable[MAX_NAT_TABLE];
int                *natureTableLen;
ThotBool            onlyOne;

#endif /* __STDC__ */

{
   int                 j;
   SRule              *pRule;

   /* cherche les regles de nature dans la table des regles */
   for (j = 0; j < pSS->SsNRules; j++)
     {
	pRule = &pSS->SsRule[j];
	if (pRule->SrConstruct == CsNatureSchema)
	   /* c'est une regle de nature */
	   if (pRule->SrSSchemaNat != NULL)
	      if (pRule->SrSSchemaNat->SsNObjects > 0)
		 /* la nature est effectivement utilisee */
		 AddNatureToTable (pRule->SrSSchemaNat, natureTable, natureTableLen, onlyOne);
     }
   /* met les extensions du schema dans la table */
   while (pSS->SsNextExtens != NULL)
     {
	pSS = pSS->SsNextExtens;
	AddNatureToTable (pSS, natureTable, natureTableLen, onlyOne);
     }
}


/*----------------------------------------------------------------------
   SearchPageBreak recherche a partir de l'element pEl un element	
   	BreakPage concernant la vue view. Si relative est faux, pageNum	
   	represent le numero de la page cherchee, sinon, c'est le nombre	
   	de pages a sauter (ce nombre peut etre negatif, pour un saut	
   	en arriere).							
  ----------------------------------------------------------------------*/
#ifdef __STDC__
PtrElement          SearchPageBreak (PtrElement pEl, int view, int pageNum, ThotBool relative)
#else  /* __STDC__ */
PtrElement          SearchPageBreak (pEl, view, pageNum, relative)
PtrElement          pEl;
int                 view;
int                 pageNum;
ThotBool            relative;

#endif /* __STDC__ */
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
		     if (relative)
		       {
			  result = pElP;
			  pageNum--;
		       }
		     else
			/* on cherche la page de numero pageNum */
		     if (pElP->ElPageNumber == pageNum)
			/* c'est la page cherchee */
		       {
			  result = pElP;
			  pageNum = 0;
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
#ifdef __STDC__
void                ScrollPageToTop (PtrElement pPage, int view, PtrDocument pDoc)
#else  /* __STDC__ */
void                ScrollPageToTop (pPage, view, pDoc)
PtrElement          pPage;
int                 view;
PtrDocument         pDoc;

#endif /* __STDC__ */
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
		if (pAb->AbLeafType == LtGraphics
		    && pAb->AbShape == 'h'
		    && pAb->AbHorizPos.PosAbRef == NULL)
		  {
		     pAbGraph = pAb;
		     pAb = NULL;
		  }
		else
		   pAb = pAb->AbNext;
	     if (pAbGraph != NULL)
	       {
		  /* cherche la fenetre correspondant a la vue */
		  if (pPage->ElAssocNum == 0)
		     frame = pDoc->DocViewFrame[view - 1];
		  else
		     frame = pDoc->DocAssocFrame[pPage->ElAssocNum - 1];
		  ShowBox (frame, pAbGraph->AbBox, 0, 0);
	       }
	  }
     }
}
