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
 * This module handles document contents: text and graphics
 *
 * Authors: V. Quint (INRIA)
 *          I. Vatton (INRIA) - Polylines
 *
 */

#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "appstruct.h"
#include "typecorr.h"
#include "appdialogue.h"
#include "application.h"

#define THOT_EXPORT extern
#include "select_tv.h"
#include "edit_tv.h"
#include "appdialogue_tv.h"

#include "applicationapi_f.h"
#include "memory_f.h"
#include "tree_f.h"
#include "references_f.h"
#include "callback_f.h"
#include "changeabsbox_f.h"
#include "thotmsg_f.h"


/*----------------------------------------------------------------------
   	NewTextBuffer acquiert un nouveau buffer de texte, l'initialise	
   	le chaine apres le buffer pBuf et le rattache au meme element	
   	feuille.							
   	Retourne un pointeur sur le nouveau buffer.			
  ----------------------------------------------------------------------*/
#ifdef __STDC__
PtrTextBuffer       NewTextBuffer (PtrTextBuffer pBuf)

#else  /* __STDC__ */
PtrTextBuffer       NewTextBuffer (pBuf)
PtrTextBuffer       pBuf;

#endif /* __STDC__ */

{
   PtrTextBuffer       pNewBuf;

   GetTextBuffer (&pNewBuf);
   pNewBuf->BuNext = pBuf->BuNext;
   pNewBuf->BuPrevious = pBuf;
   if (pNewBuf->BuNext != NULL)
      pNewBuf->BuNext->BuPrevious = pNewBuf;
   pBuf->BuNext = pNewBuf;
   return pNewBuf;
}


/*----------------------------------------------------------------------
   	CreateTextBuffer  cree un premier buffer de texte pour un 	
   	element feuille							
   	pEl est l'element feuille qui doit etre de type texte.		
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateTextBuffer (PtrElement pEl)

#else  /* __STDC__ */
void                CreateTextBuffer (pEl)
PtrElement          pEl;

#endif /* __STDC__ */

{
   PtrTextBuffer       pBuf;

   pEl->ElLeafType = LtText;
   pEl->ElTextLength = 0;
   GetTextBuffer (&pBuf);
   pEl->ElText = pBuf;
}

/*----------------------------------------------------------------------
   	DeleteTextBuffer  supprime un buffer de texte dont le pointeur est	
   	passe' en parametre.						
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                DeleteTextBuffer (PtrTextBuffer * pBuf)

#else  /* __STDC__ */
void                DeleteTextBuffer (pBuf)
PtrTextBuffer      *pBuf;

#endif /* __STDC__ */

{
   if (*pBuf != NULL)
     {
	if ((*pBuf)->BuPrevious != NULL)
	   (*pBuf)->BuPrevious->BuNext = (*pBuf)->BuNext;
	if ((*pBuf)->BuNext != NULL)
	   (*pBuf)->BuNext->BuPrevious = (*pBuf)->BuPrevious;
	FreeTextBuffer (*pBuf);
	*pBuf = NULL;
     }
}


/*----------------------------------------------------------------------
   	SplitTextElement   coupe en deux un element feuille de type texte.	
   	pEl: l'element de texte a couper.				
   	rank: indice du caractere devant lequel on coupe		
   	pDoc: document auquel appartient l'element a couper

  au retour:
	pSecondPart: l'element de texte correspondant a la partie
		     apres la coupure.
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                SplitTextElement (PtrElement pEl, int rank, PtrDocument pDoc, ThotBool withAppEvent, PtrElement *pSecondPart)

#else  /* __STDC__ */
void                SplitTextElement (pEl, rank, pDoc, withAppEvent, pSecondPart)
PtrElement          pEl;
int                 rank;
PtrDocument         pDoc;
ThotBool            withAppEvent;
PtrElement	    *pSecondPart;

#endif /* __STDC__ */

{
   PtrTextBuffer       pBuf;
   int                 i, len;
   PtrElement          pEl2;
   PtrTextBuffer       pBuf1;
   NotifyElement       notifyEl;
   NotifyOnTarget      notifyTxt;
   PtrElement          pSibling;
   PtrElement          pAsc;
   int                 nSiblings;
   ThotBool            dontCut;

   *pSecondPart = NULL;
   if (pEl != NULL)
     {
	dontCut = FALSE;
	if (withAppEvent)
	  {
	     /* demande a l'application si on peut creer ce type d'element */
	     notifyEl.event = TteElemNew;
	     notifyEl.document = (Document) IdentDocument (pDoc);
	     notifyEl.element = (Element) (pEl->ElParent);
	     notifyEl.elementType.ElTypeNum = pEl->ElTypeNumber;
	     notifyEl.elementType.ElSSchema = (SSchema) (pEl->ElStructSchema);
	     pSibling = pEl;
	     nSiblings = 1;
	     while (pSibling->ElPrevious != NULL)
	       {
		  nSiblings++;
		  pSibling = pSibling->ElPrevious;
	       }
	     notifyEl.position = nSiblings;
	     if (CallEventType ((NotifyEvent *) & notifyEl, TRUE))
		/* l'application refuse */
		dontCut = TRUE;
	     else if (rank <= pEl->ElTextLength)
		/* on va couper le texte */
	       {
		  /* envoie l'evenement ElemTextModify.Pre a qui le demande */
		  pAsc = pEl;
		  while (pAsc != NULL)
		    {
		       notifyTxt.event = TteElemTextModify;
		       notifyTxt.document = (Document) IdentDocument (pDoc);
		       notifyTxt.element = (Element) pAsc;
		       notifyTxt.target = (Element) pEl;
		       notifyTxt.targetdocument = (Document) IdentDocument (pDoc);
		       dontCut = dontCut || CallEventType ((NotifyEvent *) & notifyTxt, TRUE);
		       pAsc = pAsc->ElParent;
		    }
	       }
	  }
	/* copie l'element de texte avec attributs et presentation specifique */
	pEl2 = ReplicateElement (pEl, pDoc);
	pEl2->ElVolume = 0;
	pEl2->ElTextLength = 0;
	*pSecondPart = pEl2;
	/* acquiert un buffer de texte pour la 2eme partie */
	CreateTextBuffer (pEl2);
	/* chaine le nouvel element */
	InsertElementAfter (pEl, pEl2);
	if (rank <= pEl->ElTextLength)
	   /* le nouvel element n'a pas une longueur nulle */
	  {
	     /* longueur du nouvel element */
	     pEl2->ElTextLength = pEl->ElTextLength - rank + 1;
	     pEl2->ElVolume = pEl2->ElTextLength;
	     /* nouvelle longueur de l'ancien */
	     pEl->ElTextLength = rank - 1;
	     pEl->ElVolume = pEl->ElTextLength;
	     /* cherche le buffer (pointe' par pBuf) ou a lieu la coupure */
	     pBuf = pEl->ElText;
	     i = 0;
	     while (i + pBuf->BuLength < rank)
	       {
		  i += pBuf->BuLength;
		  pBuf = pBuf->BuNext;
	       }
	     /* longueur +1 du texte qui restera dans le dernier buffer (pBuf)
	        de la premiere partie */
	     len = rank - i;
	     pBuf1 = pEl2->ElText;
	     pBuf1->BuNext = pBuf->BuNext;
	     if (pBuf1->BuNext != NULL)
		pBuf1->BuNext->BuPrevious = pEl2->ElText;
	     /* copie la 2eme partie du buffer coupe' dans le nouveau buffer */
	     i = 0;
	     do
	       {
		  pBuf1->BuContent[i] = pBuf->BuContent[len + i - 1];
		  i++;
	       }
	     while (pBuf1->BuContent[i - 1] != EOS);
	     pBuf1->BuLength = i - 1;
	     pBuf1 = pBuf;
	     /* met a jour le dernier buffer de la premiere partie */
	     pBuf1->BuNext = NULL;
	     pBuf1->BuContent[len - 1] = EOS;
	     pBuf1->BuLength = len - 1;
	     /* essaie de vider ce dernier buffer dans le precedent */
	     if (pBuf1->BuPrevious != NULL)
		/* il y a un buffer precedent */
		if (pBuf1->BuPrevious->BuLength + pBuf1->BuLength < THOT_MAX_CHAR)
		   /* ca peut tenir */
		  {
		     i = 0;
		     while (i <= pBuf1->BuLength)
		       {
			  pBuf1->BuPrevious->BuContent[pBuf1->BuPrevious->BuLength + i] =
			     pBuf1->BuContent[i];
			  i++;
		       }
		     pBuf1->BuPrevious->BuLength += pBuf1->BuLength;
		     /* supprime le dernier buffer de la premiere partie : il est */
		     /* vide */
		     pBuf1->BuPrevious->BuNext = NULL;
		     FreeTextBuffer (pBuf);
		  }
	     /* accroche les buffers de la deuxieme partie a leur element */
	     pBuf = pEl2->ElText;
	     while (pBuf != NULL)
		pBuf = pBuf->BuNext;
	     if (withAppEvent)
	       {
		  /* envoie l'evenement ElemTextModify.Post a qui le demande */
		  pAsc = pEl;
		  while (pAsc != NULL)
		    {
		       notifyTxt.event = TteElemTextModify;
		       notifyTxt.document = (Document) IdentDocument (pDoc);
		       notifyTxt.element = (Element) pAsc;
		       notifyTxt.target = (Element) pEl;
		       notifyTxt.targetdocument = (Document) IdentDocument (pDoc);
		       CallEventType ((NotifyEvent *) & notifyTxt, FALSE);
		       pAsc = pAsc->ElParent;
		    }
	       }
	  }
	if (withAppEvent)
	  {
	     /* envoie l'evenement ElemNew.Post au nouvel element de texte */
	     notifyEl.event = TteElemNew;
	     notifyEl.document = (Document) IdentDocument (pDoc);
	     notifyEl.element = (Element) pEl2;
	     notifyEl.elementType.ElTypeNum = pEl2->ElTypeNumber;
	     notifyEl.elementType.ElSSchema = (SSchema) (pEl2->ElStructSchema);
	     notifyEl.position = 0;
	     CallEventType ((NotifyEvent *) & notifyEl, FALSE);
	  }
     }
}


/*--------------------------------------------------------------------
   	MergeTextElements   si l'element pEl est une feuille de texte et si	
   	son suivant est egalement une feuille de texte dans le meme	
   	alphabet, reunit les deux elements sucessifs en un seul.	
   	Le deuxieme element, devenu vide, n'est pas libere' et un	
   	pointeur sur cet element est retourne dans pFreeEl.
	Le parametre booleen removeAbsBox indique s'il faut liberer les
        paves de l'element qui va etre detruit.

        Cette fonction retourne TRUE si la fusion a lieu,
                                FALSE sinon.
  ----------------------------------------------------------------------*/

#ifdef __STDC__
ThotBool            MergeTextElements (PtrElement pEl, PtrElement * pFreeEl, PtrDocument pDoc,
				       ThotBool withAppEvent, ThotBool removeAbsBox)

#else  /* __STDC__ */
ThotBool            MergeTextElements (pEl, pFreeEl, pDoc, withAppEvent, removeAbsBox)
PtrElement          pEl;
PtrElement         *pFreeEl;
PtrDocument         pDoc;
ThotBool            withAppEvent;
ThotBool            removeAbsBox;

#endif /* __STDC__ */

{
   PtrElement          pEl1, pEl2;
   PtrElement          pSibling;
   PtrElement          pAsc;
   PtrTextBuffer       pBuf1, pBuf2;
   PtrReference        pRef1, pRef2;
   NotifyElement       notifyEl;
   NotifyOnTarget      notifyTxt;
   int                 i;
   int                 nSiblings;
   ThotBool            merge;

   merge = FALSE;

   *pFreeEl = NULL;
   if (pEl != NULL)
     {
	pEl1 = pEl;
	pEl2 = pEl1->ElNext;

        if ((ElementIsReadOnly (pEl1)) ||
            (ElementIsReadOnly (pEl2)))
           /*** One or both elements are protected! ***/
           return FALSE;

	if (pEl2 != NULL && pEl1->ElLeafType == LtText && pEl1->ElTerminal)
	   if (pEl2->ElLeafType == LtText && pEl2->ElTerminal)
	      if (pEl2->ElLanguage == pEl->ElLanguage)
		 if (pEl1->ElSource == NULL && pEl2->ElSource == NULL)
		    if (pEl1->ElStructSchema->SsRule[pEl1->ElTypeNumber - 1].SrConstruct != CsConstant)
		       if (pEl2->ElStructSchema->SsRule[pEl2->ElTypeNumber - 1].SrConstruct != CsConstant)
			 {
			    merge = TRUE;
			    if (withAppEvent)
			      {
				 /* envoie l'evenement ElemDelete.Pre et demande a */
				 /* l'application si elle est d'accord pour detruire le 2eme element */
				 notifyEl.event = TteElemDelete;
				 notifyEl.document = (Document) IdentDocument (pDoc);
				 notifyEl.element = (Element) pEl2;
				 notifyEl.elementType.ElTypeNum = pEl2->ElTypeNumber;
				 notifyEl.elementType.ElSSchema = (SSchema) (pEl2->ElStructSchema);
				 notifyEl.position = TTE_MERGE_DELETE_ITEM;
				 notifyEl.info = 0;
				 if (CallEventType ((NotifyEvent *) & notifyEl, TRUE))
				    /* l'application refuse la destruction, on ne fait rien */
				    merge = FALSE;
				 else
				   {
				      /* envoie l'evenement ElemTextModify.Pre a l'element */
				      /* de texte qui sera conserve' et a ses englobants */
				      pAsc = pEl;
				      while (pAsc != NULL && merge)
					{
					   notifyTxt.event = TteElemTextModify;
					   notifyTxt.document = (Document) IdentDocument (pDoc);
					   notifyTxt.element = (Element) pAsc;
					   notifyTxt.target = (Element) pEl1;
					   notifyTxt.targetdocument = (Document) IdentDocument (pDoc);
					   merge = !CallEventType ((NotifyEvent *) & notifyTxt, TRUE);
					   pAsc = pAsc->ElParent;
					}
				   }
			      }
			    if (merge && pEl1->ElText != NULL 
				&& pEl2->ElText != NULL)
			      {
#ifndef NODISPLAY
                                 /* Si l'element est selectionne, alors annule la selection */
				 if ((pEl2 == FirstSelectedElement) ||
				     (pEl2 == LastSelectedElement))
                                    TtaClearViewSelections ();

                                 /* Detruit eventuellement les paves du 2ieme element */
                                 if (removeAbsBox)
                                    DestroyAbsBoxes (pEl2, pDoc, FALSE);
#endif
				 /* cherche le dernier buffer de texte du premier element */
				 pBuf1 = pEl1->ElText;
				 while (pBuf1->BuNext != NULL)
				    pBuf1 = pBuf1->BuNext;
				 /* premier buffer du deuxieme element */
				 pBuf2 = pEl2->ElText;
				 if (pBuf1->BuLength + pBuf2->BuLength < THOT_MAX_CHAR)

				    /* copie le contenu du premier buffer du 2eme element */
				    /* a la fin du dernier buffer du 1er element */
				   {
				      i = 0;
				      do
					{
					   i++;
					   pBuf1->BuContent[pBuf1->BuLength + i - 1] = pBuf2->BuContent
					      [i - 1];
					}
				      while (pBuf2->BuContent[i - 1] != EOS);
				      pBuf1->BuLength += pBuf2->BuLength;
				      /* libere le buffer vide */
				      pBuf1->BuNext = pBuf2->BuNext;
				      if (pBuf1->BuNext != NULL)
					 pBuf1->BuNext->BuPrevious = pBuf1;
				      FreeTextBuffer (pBuf2);
				      pBuf2 = pBuf1->BuNext;
				   }
				 else
				    /* chaine les deux buffers */
				   {
				      pBuf1->BuNext = pBuf2;
				      pBuf2->BuPrevious = pBuf1;
				   }
				 /* met a jour les pointeurs d'element de la 2eme partie */
				 while (pBuf2 != NULL)
				    pBuf2 = pBuf2->BuNext;
				 /* met a jour la longueur du premier element */
				 pEl1->ElTextLength += pEl2->ElTextLength;
				 pEl1->ElVolume = pEl1->ElTextLength;

				 /* reporte les references au 2eme element sur le premier */
				 if (pEl2->ElReferredDescr != NULL)
				    /* le 2eme element est reference */
				    if (pEl1->ElReferredDescr == NULL)
				       /* le premier element n'est pas reference' */
				       /* on met le descripteur d'element reference' du 2eme */
				       /* element sur le premier */
				      {
					 pEl1->ElReferredDescr = pEl2->ElReferredDescr;
					 if (!pEl1->ElReferredDescr->ReExternalRef)
					    pEl1->ElReferredDescr->ReReferredElem = pEl;
					 pEl2->ElReferredDescr = NULL;
				      }
				    else
				       /* le premier element est deja reference' */
				      {
					 pRef2 = pEl2->ElReferredDescr->ReFirstReference;
					 /* dechaine et libere le descripteur d'element */
					 /* reference' du 2eme element */
					 pEl2->ElReferredDescr->ReFirstReference = NULL;
					 DeleteAllReferences (pEl2);
					 /* ajoute les references du 2eme element a la suite
					    de celles du 1er */
					 pRef1 = pEl1->ElReferredDescr->ReFirstReference;
					 if (pRef1 != NULL)
					   {
					      while (pRef1->RdNext != NULL)
						 pRef1 = pRef1->RdNext;
					      pRef1->RdNext = pRef2;
					      pRef2->RdPrevious = pRef1;
					   }
					 else
					    pEl1->ElReferredDescr->ReFirstReference = pRef2;
					 while (pRef2 != NULL)
					   {
					      pRef2->RdReferred = pEl1->ElReferredDescr;
					      pRef2 = pRef2->RdNext;
					   }
				      }
				 pEl2->ElText = NULL;
				 *pFreeEl = pEl2;
				 pEl2->ElVolume = 0;
				 if (withAppEvent)
				   {
				      /* prepare l'evenement ElemDelete.Post pour le 2eme elem. */
				      notifyEl.event = TteElemDelete;
				      notifyEl.document = (Document) IdentDocument (pDoc);
				      notifyEl.element = (Element) (pEl2->ElParent);
				      notifyEl.elementType.ElTypeNum = pEl2->ElTypeNumber;
				      notifyEl.elementType.ElSSchema = (SSchema) (pEl2->ElStructSchema);
				      nSiblings = 0;
				      pSibling = pEl2;
				      while (pSibling->ElPrevious != NULL)
					{
					   nSiblings++;
					   pSibling = pSibling->ElPrevious;
					}
				      notifyEl.position = nSiblings;
				      notifyEl.info = 0;
				   }
				 /* dechaine le 2eme element */
				 RemoveElement (pEl2);

				 if (withAppEvent)
				    {
				    /* envoie l'evenement ElemTextModify.Post a qui le demande */
				    pAsc = pEl1;
				    while (pAsc != NULL)
					{
					   notifyTxt.event = TteElemTextModify;
					   notifyTxt.document = (Document) IdentDocument (pDoc);
					   notifyTxt.element = (Element) pAsc;
					   notifyTxt.target = (Element) pEl1;
					   notifyTxt.targetdocument = (Document) IdentDocument (pDoc);
					   CallEventType ((NotifyEvent *) & notifyTxt, FALSE);
					   pAsc = pAsc->ElParent;
					}
				    /* envoie l'evenement ElemDelete.Post pour le 2eme elem. */
				    CallEventType ((NotifyEvent *) & notifyEl, FALSE);
				    }
				 /* modifie la selection si l'element retire' constitue l'une */
				 /* des extremites de cette selection */
				 if (pEl2 == FirstSelectedElement)
				    FirstSelectedElement = pEl;
				 if (pEl2 == LastSelectedElement)
				    LastSelectedElement = pEl;
			      }
			 }
     }

     return merge;

}


/*----------------------------------------------------------------------
   	CopyText   cree, pour l'element pEl, une copie de la chaine	
   	de buffers de texte pointee par pBuf et retourne un pointeur	
   	sur le premier buffer de la copie realisee.			
  ----------------------------------------------------------------------*/

#ifdef __STDC__
PtrTextBuffer       CopyText (PtrTextBuffer pBuf, PtrElement pEl)

#else  /* __STDC__ */
PtrTextBuffer       CopyText (pBuf, pEl)
PtrTextBuffer       pBuf;
PtrElement          pEl;

#endif /* __STDC__ */

{
   PtrTextBuffer       pBufSource1, pBufSource2, pBufCopy1, pBufCopy2,
                       ret;

   /* acquiert un premier buffer de texte */
   GetTextBuffer (&pBufCopy1);
   ret = pBufCopy1;
   pBufSource1 = pBuf;
   *pBufCopy1 = *pBufSource1;
   pBufCopy1->BuNext = NULL;
   pBufCopy1->BuPrevious = NULL;
   pBufCopy1->BuLength = pBufSource1->BuLength;
   /* traite les buffers suivants */
   pBufSource2 = pBufSource1->BuNext;
   while (pBufSource2 != NULL)
     {
	GetTextBuffer (&pBufCopy2);
	*pBufCopy2 = *pBufSource2;
	pBufCopy2->BuPrevious = pBufCopy1;
	pBufCopy1->BuNext = pBufCopy2;
	pBufCopy1 = pBufCopy2;
	pBufSource2 = pBufSource2->BuNext;
     }
   return ret;
}


/*----------------------------------------------------------------------
   StringAndTextEqual compare une chaine de caracteres (String) et 
   un texte contenu dans une suite de buffers. Retourne TRUE si    
   les contenus sont egaux.                                        
  ----------------------------------------------------------------------*/

#ifdef __STDC__
ThotBool            StringAndTextEqual (STRING String, PtrTextBuffer pBuf)

#else  /* __STDC__ */
ThotBool            StringAndTextEqual (String, pBuf)
STRING              String;
PtrTextBuffer       pBuf;

#endif /* __STDC__ */

{
   ThotBool            equal;
   int                 l, lenRest;
   STRING              pChar;

   equal = FALSE;
   if (pBuf == NULL && String == NULL)
      /* les deux sont vides, egalite' */
      equal = TRUE;
   else if (pBuf != NULL && String != NULL)
      if (String[0] == EOS && pBuf->BuLength == 0)
	 equal = TRUE;
      else
	{
	   pChar = String;
	   lenRest = ustrlen (String);
	   equal = TRUE;
	   /* parcourt les buffers de texte successifs */
	   while (pBuf != NULL && lenRest > 0 && equal)
	     {
		if (pBuf->BuLength > 0)
		  {
		     if (pBuf->BuLength > lenRest)
			equal = FALSE;
		     else
		       {
			  if (pBuf->BuLength < lenRest)
			     l = pBuf->BuLength;
			  else
			     l = lenRest;
			  equal = (ustrncmp (pChar, pBuf->BuContent, l) == 0);
			  pChar += l;
			  lenRest -= l;
		       }
		  }
		pBuf = pBuf->BuNext;
	     }
	   if (pBuf != NULL || lenRest > 0)
	      equal = FALSE;
	}
   return equal;
}


/*----------------------------------------------------------------------
   TextsEqual compare deux textes contenus dans des suites de     
   buffers. Retourne TRUE si les deux textes sont egaux.		
  ----------------------------------------------------------------------*/

#ifdef __STDC__
ThotBool            TextsEqual (PtrTextBuffer pBuf1, PtrTextBuffer pBuf2)

#else  /* __STDC__ */
ThotBool            TextsEqual (pBuf1, pBuf2)
PtrTextBuffer       pBuf1;
PtrTextBuffer       pBuf2;

#endif /* __STDC__ */

{
   ThotBool            equal;
   PtrTextBuffer       pTB1, pTB2;
   STRING              pChar1;
   STRING              pChar2;
   int                 len, lenRest1, lenRest2;

   equal = FALSE;
   if (pBuf1 == NULL && pBuf2 == NULL)
      equal = TRUE;
   else if (pBuf1 != NULL && pBuf2 != NULL)
     {
	pTB1 = pBuf1;
	lenRest1 = pTB1->BuLength;
	pChar1 = &pTB1->BuContent[0];
	pTB2 = pBuf2;
	lenRest2 = pTB2->BuLength;
	pChar2 = &pTB2->BuContent[0];
	equal = TRUE;
	/* parcourt les deux chaines de buffers */
	while (pTB1 != NULL && pTB2 != NULL && equal)
	  {
	     if (lenRest1 == 0)
	       {
		  pTB1 = pTB1->BuNext;
		  if (pTB1 != NULL)
		    {
		       lenRest1 = pTB1->BuLength;
		       pChar1 = &pTB1->BuContent[0];
		    }
	       }
	     if (lenRest2 == 0)
	       {
		  pTB2 = pTB2->BuNext;
		  if (pTB2 != NULL)
		    {
		       lenRest2 = pTB2->BuLength;
		       pChar2 = &pTB2->BuContent[0];
		    }
	       }
	     if (lenRest1 > 0 && lenRest2 > 0)
	       {
		  if (lenRest1 > lenRest2)
		     len = lenRest2;
		  else
		     len = lenRest1;
		  equal = (ustrncmp (pChar1, pChar2, len) == 0);
		  lenRest1 -= len;
		  lenRest2 -= len;
		  pChar1 += len;
		  pChar2 += len;
	       }
	  }
	if (pTB1 != NULL || pTB2 != NULL || lenRest1 > 0 || lenRest2 > 0)
	   equal = FALSE;
     }
   return equal;
}


/*----------------------------------------------------------------------
   CopyTextToText copie a la fin de la suite de buffers       
   commencant au buffer pCopyBuf le texte contenu dans le buffer	
   pSrceBuf et dans les buffers suivants. Ajoute eventuellement	
   de nouveaux buffers a la fin de la suite de buffers pointee par	
   pCopyBuf.							
   Retourne dans len le nombre de caracteres copie's.		
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                CopyTextToText (PtrTextBuffer pSrceBuf, PtrTextBuffer pCopyBuf, int *len)

#else  /* __STDC__ */
void                CopyTextToText (pSrceBuf, pCopyBuf, len)
PtrTextBuffer       pSrceBuf;
PtrTextBuffer       pCopyBuf;
int                *len;

#endif /* __STDC__ */

{
   PtrTextBuffer       pTBSrce, pTBDest;

   *len = 0;
   if (pSrceBuf != NULL && pCopyBuf != NULL)
     {
	/* cherche le dernier buffer de la chaine des buffers destination */
	pTBDest = pCopyBuf;
	while (pTBDest->BuNext != NULL)
	   pTBDest = pTBDest->BuNext;
	pTBSrce = pSrceBuf;
	/* parcourt la chaine des buffers source */
	while (pTBSrce != NULL)
	  {
	     if (pTBSrce->BuLength > 0)
		/* ce buffer source n'est pas vide, on le copie */
	       {
		  if (THOT_MAX_CHAR - 1 - pTBDest->BuLength <= pTBSrce->BuLength)
		     /* pas assez de place dans le buffer destination */
		     /* pour copier tout le texte du buffer source, on */
		     /* prend un nouveau buffer destination */
		     pTBDest = NewTextBuffer (pTBDest);
		  ustrcpy (pTBDest->BuContent + pTBDest->BuLength, pTBSrce->BuContent);
		  pTBDest->BuLength += pTBSrce->BuLength;
		  *len += pTBSrce->BuLength;
	       }
	     /* buffer source suivant */
	     pTBSrce = pTBSrce->BuNext;
	  }
     }
}


/*----------------------------------------------------------------------
   CopyStringToText copie a la fin de la suite de buffers      
   commencant au buffer pCopyBuf le contenu de la chaine srceStrn.	
   Ajoute eventuellement de nouveaux buffers a la fin de la suite  
   de buffers.                                                     
   Retourne dans LgCopiee le nombre de caracteres copie's.         
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                CopyStringToText (CHAR_T* srceStrn, PtrTextBuffer pCopyBuf, int *LgCopiee)

#else  /* __STDC__ */
void                CopyStringToText (srceStrn, pCopyBuf, LgCopiee)
CHAR_T*             srceStrn;
PtrTextBuffer       pCopyBuf;
int                *LgCopiee;

#endif /* __STDC__ */

{
   PtrTextBuffer       pTBDest;
   CHAR_T*             pSrce;
   int                 lenRest, len;

   *LgCopiee = 0;
   if (pCopyBuf != NULL && srceStrn != NULL)
     {
	pTBDest = pCopyBuf;
	/* longueur restant a copier */
	lenRest = ustrlen (srceStrn);
	*LgCopiee = lenRest;
	pSrce = srceStrn;
	/* cherche le dernier buffer de la chaine de destination */
	while (pTBDest->BuNext != NULL)
	   pTBDest = pTBDest->BuNext;
	/* boucle tant qu'il reste du texte a copier */
	while (lenRest > 0)
	  {
	     if (THOT_MAX_CHAR - 1 - pTBDest->BuLength <= lenRest)
		/* le texte restant ne tient pas dans le buffer destination */
		if (pTBDest->BuLength < THOT_MAX_CHAR - 2)
		   /* il reste un peu de place, on copiera ce qu'on peut */
		   len = THOT_MAX_CHAR - 1 - pTBDest->BuLength;
		else
		   /* on prend un nouveau buffer destination */
		  {
		     pTBDest = NewTextBuffer (pTBDest);
		     len = 0;
		  }
	     else
		/* le texte restant tient dans le buffer destination */
		len = lenRest;
	     if (len > 0)
		/* on fait la copie */
	       {
		  ustrncpy (pTBDest->BuContent + pTBDest->BuLength, pSrce, len);
		  pSrce += len;
		  pTBDest->BuLength += len;
		  pTBDest->BuContent[pTBDest->BuLength] = WC_EOS;
		  lenRest -= len;
	       }
	  }
     }
}

/*----------------------------------------------------------------------
   CopyTextToString copie dans la chaine pStrCpy le texte	
   contenu dans le buffer pSrceBuf et dans les buffers suivants.	
   A l'appel, len contient la longueur maximum a copier.           
   Au retour, len contient le nombre de caracteres copie's.        
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                CopyTextToString (PtrTextBuffer pSrceBuf, CHAR_T* pStrCpy, int *len)

#else  /* __STDC__ */
void                CopyTextToString (pSrceBuf, pStrCpy, len)
PtrTextBuffer       pSrceBuf;
CHAR_T*             pStrCpy;
int                *len;

#endif /* __STDC__ */

{
   int                 LgMax;
   int                 l;
   PtrTextBuffer       pTBSrce;
   STRING              pDest;

   if (pSrceBuf == NULL || pStrCpy == NULL || *len <= 0)
      *len = 0;
   else
     {
	LgMax = *len;
	*len = 0;
	pTBSrce = pSrceBuf;
	pDest = pStrCpy;
	while (pTBSrce != NULL && LgMax > 0)
	  {
	     if (pTBSrce->BuLength > 0)
	       {
		  if (pTBSrce->BuLength < LgMax)
		     l = pTBSrce->BuLength;
		  else
		     l = LgMax;
		  ustrncpy (pDest, pTBSrce->BuContent, l);
		  pDest += l;
		  *pDest = EOS;
		  LgMax -= l;
		  *len += l;
	       }
	     pTBSrce = pTBSrce->BuNext;
	  }
     }
}


/*----------------------------------------------------------------------
   ClearText annule le texte contenu dans le buffer pBuf  
   et libere les buffers suivants.                                 
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                ClearText (PtrTextBuffer pBuf)

#else  /* __STDC__ */
void                ClearText (pBuf)
PtrTextBuffer       pBuf;

#endif /* __STDC__ */

{
   PtrTextBuffer       pNextBuf;

   if (pBuf != NULL)
     {
	pBuf = pBuf;
	pBuf->BuLength = 0;
	pBuf->BuContent[0] = EOS;
	pNextBuf = pBuf->BuNext;
	pBuf->BuNext = NULL;
	pBuf = pNextBuf;
	while (pBuf != NULL)
	  {
	     pNextBuf = pBuf->BuNext;
	     FreeTextBuffer (pBuf);
	     pBuf = pNextBuf;
	  }
     }
}

/*----------------------------------------------------------------------
   AddPointInPolyline ajoute un point de rang rank dans la liste   
   des points de contro^le de la polyline.                 
   On saute le point 0 (coordonnees du point limite).      
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                AddPointInPolyline (PtrTextBuffer firstBuffer, int rank, int x, int y)

#else  /* __STDC__ */
void                AddPointInPolyline (firstBuffer, rank, x, y)
PtrTextBuffer       firstBuffer;
int                 rank;
int                 x;
int                 y;

#endif /* __STDC__ */

{
   PtrTextBuffer       pBuf, pNextBuf;
   int                 i, j;
   PolyLinePoint       savePoint, oldSavePoint;
   ThotBool            stop;

   /* cherche le buffer contenant le point de rang rank */
   pBuf = firstBuffer;
   /* on saute le point 0 (coordonnees du point limite) */
   rank++;
   while (rank > pBuf->BuLength && pBuf->BuNext != NULL)
     {
	rank -= pBuf->BuLength;
	pBuf = pBuf->BuNext;
     }
   if (rank > pBuf->BuLength)
      /* on va ajouter le nouveau point en derniere position */
     {
	rank = pBuf->BuLength + 1;
	if (rank > MAX_POINT_POLY)
	   /* le dernier buffer est plein, on en ajoute un */
	  {
	     pBuf = NewTextBuffer (pBuf);
	     rank = 1;
	  }
	/* ce buffer a un point de plus */
	pBuf->BuLength++;
     }
   else
      /* il faut decaler les points suivants d'une position */
     {
	i = rank;
	pNextBuf = pBuf;
	stop = FALSE;
	do
	  {
	     if (pNextBuf != pBuf)
		/* on n'est plus sur le buffer ou on va mettre le */
		/* nouveau point, on sauve temporairement le point */
		/* a inserer en tete, qui vient de la fin du buffer */
		/* precedent */
		oldSavePoint = savePoint;
	     /* on garde le dernier point du buffer, pour le remettre */
	     /* en tete du buffer suivant */
	     if (pNextBuf->BuLength > 0)
		savePoint = pNextBuf->BuPoints[pNextBuf->BuLength - 1];
	     /* on decale tous les points d'une position vers la droite */
	     for (j = pNextBuf->BuLength - 1; j >= i; j--)
		pNextBuf->BuPoints[j] = pNextBuf->BuPoints[j - 1];
	     if (pNextBuf != pBuf)
		/* on met en tete du buffer le point qui debordait du */
		/* buffer precedent */
		pNextBuf->BuPoints[0] = oldSavePoint;
	     if (pNextBuf->BuLength < MAX_POINT_POLY)
		/* ce buffer n'est pas plein */
	       {
		  /* on arrete les decalages */
		  stop = TRUE;
		  /* on ajoute en fin le dernier point, qui a ete ecrase' */
		  /* par le decalage */
		  if (pNextBuf->BuLength > 0)
		     pNextBuf->BuPoints[pNextBuf->BuLength] = savePoint;
		  /* ce buffer a un point de plus */
		  pNextBuf->BuLength++;
	       }
	     else
	       {
		  if (pNextBuf->BuNext == NULL)
		     /* c'etait le dernier buffer, on en ajoute un nouveau */
		     pNextBuf = NewTextBuffer (pNextBuf);
		  else
		     /* on passe au buffer suivant */
		     pNextBuf = pNextBuf->BuNext;
		  i = 1;
	       }
	  }
	while (!stop);
     }
   /* met le nouveau point a sa place */
   pBuf->BuPoints[rank - 1].XCoord = x;
   pBuf->BuPoints[rank - 1].YCoord = y;
   /* verifie le point limite */
   if (x > firstBuffer->BuPoints[0].XCoord)
      firstBuffer->BuPoints[0].XCoord = x;
   if (y > firstBuffer->BuPoints[0].YCoord)
      firstBuffer->BuPoints[0].YCoord = y;
}

/*----------------------------------------------------------------------
   DeletePointInPolyline detruit le point de rang rank dans la     
   liste des points de contro^le de la polyline.           
   On saute le point 0 (coordonnees du point limite).      
   Le pointeur firstBuffer peut e^tre modifie' par la      
   proce'dure.                                             
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                DeletePointInPolyline (PtrTextBuffer * firstBuffer, int rank)

#else  /* __STDC__ */
void                DeletePointInPolyline (firstBuffer, rank)
PtrTextBuffer      *firstBuffer;
int                 rank;

#endif /* __STDC__ */

{
   PtrTextBuffer       pBuf;
   int                 j;

   pBuf = *firstBuffer;
   /* on saute le point 0 (coordonnees du point limite) */
   rank++;
   /* cherche le buffer contenant le point de rang rank */
   while (rank > pBuf->BuLength && pBuf->BuNext != NULL)
     {
	rank -= pBuf->BuLength;
	pBuf = pBuf->BuNext;
     }
   if (rank > pBuf->BuLength)
      /* on va supprimer le dernier point */
      rank = pBuf->BuLength;
   /* on decale les points suivants d'une position vers la gauche */
   for (j = rank; j < pBuf->BuLength; j++)
      pBuf->BuPoints[j - 1] = pBuf->BuPoints[j];
   /* il y a un point de moins dans le buffer */
   pBuf->BuLength--;
   if (pBuf->BuLength == 0)
      /* le buffer est vide, on le supprime */
     {
	if (pBuf->BuPrevious != NULL)
	   pBuf->BuPrevious->BuNext = pBuf->BuNext;
	else
	   *firstBuffer = pBuf->BuNext;
	if (pBuf->BuNext != NULL)
	   pBuf->BuNext->BuPrevious = pBuf->BuPrevious;
	FreeTextBuffer (pBuf);
     }
}

/*----------------------------------------------------------------------
   ModifyPointInPolyline
   Modifie le point de rang rank dans la liste des points de contro^le
   de la polyline.
   On saute le point 0 (coordonnees du point limite).      
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                ModifyPointInPolyline (PtrTextBuffer firstBuffer, int rank, int x, int y)

#else  /* __STDC__ */
void                ModifyPointInPolyline (firstBuffer, rank, x, y)
PtrTextBuffer       firstBuffer;
int                 rank;
int                 x;
int                 y;

#endif /* __STDC__ */

{
   PtrTextBuffer       pBuf;

   /* on saute le point 0 (coordonnees du point limite) */
   rank++;
   /* cherche le buffer contenant le point de rang rank */
   pBuf = firstBuffer;
   while (rank > pBuf->BuLength && pBuf->BuNext != NULL)
     {
	rank -= pBuf->BuLength;
	pBuf = pBuf->BuNext;
     }
   if (rank > pBuf->BuLength)
      /* on va remplacer le dernier point */
      rank = pBuf->BuLength;
   /* met le nouveau point a sa place */
   pBuf->BuPoints[rank - 1].XCoord = x;
   pBuf->BuPoints[rank - 1].YCoord = y;
   /* verifie le point limite */
   if (x > firstBuffer->BuPoints[0].XCoord)
      firstBuffer->BuPoints[0].XCoord = x;
   if (y > firstBuffer->BuPoints[0].YCoord)
      firstBuffer->BuPoints[0].YCoord = y;
}


/*----------------------------------------------------------------------
   TtaGetTextLength

   Returns the length of a Text basic element.

   Parameter:
   element: the element of interest. This element must be a basic
   element of type Text.

   Return value:
   textLength (number of characters) of the character string
   contained in the element.

  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                 TtaGetTextLength (Element element)
#else  /* __STDC__ */
int                 TtaGetTextLength (element)
Element             element;
#endif /* __STDC__ */
{
   int                 textLength;

   UserErrorCode = 0;
   textLength = 0;
   if (element == NULL)
     TtaError (ERR_invalid_parameter);
   else if (!((PtrElement) element)->ElTerminal)
     TtaError (ERR_invalid_element_type);
   else if (((PtrElement) element)->ElLeafType != LtText &&
	    ((PtrElement) element)->ElLeafType != LtPicture)
     TtaError (ERR_invalid_element_type);
   else
     textLength = ((PtrElement) element)->ElTextLength;
   return textLength;
}

/*----------------------------------------------------------------------
   TtaGiveTextContent

   Returns the content of a Text basic element.

   Parameters:
   element: the element of interest. This element must be a basic
   element of type Text.
   buffer: the buffer that will contain the text.
   length: maximum length of that buffer.

   Return parameters:
   buffer: (the buffer contains the text).
   length: actual length of the text in the buffer.
   language: language of the text.

  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaGiveTextContent (Element element, STRING buffer, int *length, Language * language)
#else  /* __STDC__ */
void                TtaGiveTextContent (element, buffer, length, language)
Element             element;
STRING              buffer;
int                *length;
Language           *language;

#endif /* __STDC__ */
{
   PtrTextBuffer       pBuf;
   STRING              ptr;
   int                 len, l;

   UserErrorCode = 0;
   if (element == NULL)
	TtaError (ERR_invalid_parameter);
   else if (!((PtrElement) element)->ElTerminal)
	TtaError (ERR_invalid_element_type);
   else if (((PtrElement) element)->ElLeafType != LtText &&
	    ((PtrElement) element)->ElLeafType != LtPicture)
	TtaError (ERR_invalid_element_type);
   else
     {
	len = 0;
	pBuf = ((PtrElement) element)->ElText;
	ptr = buffer;
	while (pBuf != NULL && len < (*length) - 1)
	  {
	     if ((*length) < len + pBuf->BuLength + 1)
		l = (*length) - len;
	     else
		l = pBuf->BuLength + 1;
	     ustrncpy (ptr, pBuf->BuContent, l);
	     ptr = ptr + (l - 1);
	     len = len + (l - 1);
	     pBuf = pBuf->BuNext;
	  }
	*length = len;
	*ptr = EOS;
	*language = ((PtrElement) element)->ElLanguage;
     }
}
