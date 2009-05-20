/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2009
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * This module handles document contents: text and graphics
 *
 * Authors: V. Quint (INRIA)
 *          I. Vatton (INRIA) - Polylines
 *          F. Wang - More operations on transform matrix and paths/polyline,
 *                    shape recognition.
 *
 */
#include "thot_gui.h"
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
  GetViewBoxTransformation
  ----------------------------------------------------------------------*/
void GetViewBoxTransformation (PtrTransform Trans, int width, int height,
                               float *xtrans,  float *ytrans,
                               double *xscale, double *yscale,
                               ThotBool *istranslated, ThotBool *isscaled)
{
  float     x_trans, y_trans, s_width, s_height;
  double    x_scale, y_scale; 
  ThotBool  is_translated, is_scaled, is_x_clipped, is_y_clipped;

  x_trans = y_trans = 0;
  x_scale = y_scale = 1;
  is_translated = is_scaled = is_x_clipped = is_y_clipped = FALSE;
  while (Trans)
    {
      if (Trans->TransType == PtElViewBox)
        {
          /* if it's an image with only a preserveAspectRatio attribute and no
             viewBox attribute, Trans->VbWidth and Trans->VbHeight are negative
             and the real size of the image should be used instead (TODO) */
          if (Trans->VbWidth > 0)
            x_scale = (double) (width / Trans->VbWidth);
          if (Trans->VbHeight > 0)
            y_scale = (double) (height / Trans->VbHeight);
          if (Trans->VbAspectRatio == ArNone)
            /* Do not force uniform scaling: the element's bounding box exactly
               matches the viewport rectangle */
            {
              /* done */;
            }
          else
            /* Force uniform scaling (preserve aspect ratio) */
            {
              if (Trans->VbMeetOrSlice == MsMeet)
                if (x_scale > y_scale)
                  x_scale = y_scale;
                else
                  y_scale = x_scale;
              else if (Trans->VbMeetOrSlice == MsSlice)
                if (x_scale > y_scale)
                  {
                    y_scale = x_scale;
                    is_y_clipped = TRUE;
                  }
                else
                  {
                    x_scale = y_scale;
                    is_x_clipped = TRUE;
                  }

              if (Trans->VbAspectRatio == ArXMinYMin)
                {
                  x_trans = 0;
                  y_trans = 0;
                }
              else if (Trans->VbAspectRatio == ArXMidYMin)
                {
                  y_trans = 0;
                  s_width = Trans->VbWidth * x_scale;
                  if (width != s_width)
                    x_trans = (width - s_width) / 2;
                }
              else if (Trans->VbAspectRatio == ArXMaxYMin)
                {
                  y_trans = 0;
                  s_width = Trans->VbWidth * x_scale;
                  if (width != s_width)
                    x_trans = (width - s_width);
                }
              else if (Trans->VbAspectRatio == ArXMinYMid)
                {
                  x_trans = 0;
                  s_height = Trans->VbHeight * y_scale;
                  if (height != s_height)
                    y_trans = (height - s_height) / 2;
                }
              else if (Trans->VbAspectRatio == ArXMidYMid)
                /* align the middle of the element's
                   viewBox with the middle of the viewport */
                {
                  s_height = Trans->VbHeight * y_scale;
                  s_width = Trans->VbWidth * x_scale;
                  if (width != s_width)
                    x_trans = (width - s_width) / 2;
                  if (height != s_height)
                    y_trans = (height - s_height) / 2;
                }
              else if (Trans->VbAspectRatio == ArXMaxYMid)
                {
                  s_height = Trans->VbHeight * y_scale;
                  s_width = Trans->VbWidth * x_scale;
                  if (width != s_width)
                    x_trans = (width - s_width);
                  if (height != s_height)
                    y_trans = (height - s_height) / 2;                  
                }
              else if (Trans->VbAspectRatio == ArXMinYMax)
                {
                  x_trans = 0;
                  s_height = Trans->VbHeight * y_scale;
                  if (height != s_height)
                    y_trans = (height - s_height);                  
                }
              else if (Trans->VbAspectRatio == ArXMidYMax)
                {
                  s_height = Trans->VbHeight * y_scale;
                  s_width = Trans->VbWidth * x_scale;
                  if (width != s_width)
                    x_trans = (width - s_width) / 2;
                  if (height != s_height)
                    y_trans = (height - s_height);
                }
              else if (Trans->VbAspectRatio == ArXMaxYMax)
                {
                  s_height = Trans->VbHeight * y_scale;
                  s_width = Trans->VbWidth * x_scale;
                  if (width != s_width)
                    x_trans = (width - s_width);
                  if (height != s_height)
                    y_trans = (height - s_height);
                }
            }

          if (x_scale != 1 || y_scale != 1)
            is_scaled = TRUE;

          x_trans -= Trans->VbXTranslate * x_scale;  
          y_trans -= Trans->VbYTranslate * y_scale;
          if (x_trans != 0 || y_trans != 0)
            is_translated = TRUE;
        }
      Trans = Trans->Next;
    }

  // return values
  *xtrans =  x_trans;
  *ytrans = y_trans;
  *xscale = x_scale;
  *yscale = y_scale;
  *istranslated = is_translated;
  *isscaled = is_scaled;
}


/*----------------------------------------------------------------------
  NewTextBuffer acquiert un nouveau buffer de texte, l'initialise	
  le chaine apres le buffer pBuf et le rattache au meme element	
  feuille.							
  Retourne un pointeur sur le nouveau buffer.			
  ----------------------------------------------------------------------*/
PtrTextBuffer NewTextBuffer (PtrTextBuffer pBuf)
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
void CreateTextBuffer (PtrElement pEl)
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
void DeleteTextBuffer (PtrTextBuffer *pBuf)
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
  elBreak: appel par la procedure BreakElement
  au retour:
  pSecondPart: l'element de texte correspondant a la partie
  apres la coupure.
  ----------------------------------------------------------------------*/
void SplitTextElement (PtrElement pEl, int rank, PtrDocument pDoc,
                       ThotBool withAppEvent, PtrElement *pSecondPart,
                       ThotBool elBreak)
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
          notifyEl.info = 0; /* not sent by undo */
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
      if (!dontCut)
        {
          /* copie l'element de texte avec attributs et presentation
             specifique */
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
                  if (pBuf->BuNext)
                    pBuf = pBuf->BuNext;
                  else
                    rank = i;
                }
              /* longueur+1 du texte qui restera dans le dernier buffer
                 (pBuf) de la premiere partie */
              len = rank - i - 1;
              pBuf1 = pEl2->ElText;
              pBuf1->BuNext = pBuf->BuNext;
              if (pBuf1->BuNext != NULL)
                pBuf1->BuNext->BuPrevious = pEl2->ElText;
              /* copie la 2eme partie du buffer coupe' dans le nouveau
                 buffer */
              i = 0;
              do
                {
                  pBuf1->BuContent[i] = pBuf->BuContent[len + i];
                  i++;
                }
              while (pBuf1->BuContent[i - 1] != EOS);
              pBuf1->BuLength = i - 1;
              /* met a jour le dernier buffer de la premiere partie */
              pBuf->BuNext = NULL;
              pBuf->BuContent[len] = EOS;
              pBuf->BuLength = len;
              /* essaie de vider ce dernier buffer dans le precedent */
              pBuf1 = pBuf->BuPrevious;
              if (pBuf1 &&
                  pBuf1->BuLength + pBuf->BuLength <= FULL_BUFFER)
                /* il y a un buffer precedent et ca peut tenir */
                {
                  i = 0;
                  while (i <= pBuf->BuLength)
                    {
                      pBuf1->BuContent[pBuf1->BuLength + i] = pBuf->BuContent[i];
                      i++;
                    }
                  pBuf1->BuLength += pBuf->BuLength;
                  /* supprime le dernier buffer de la premiere partie :
                     il est vide */
                  pBuf1->BuNext = NULL;
                  FreeTextBuffer (pBuf);
                }
              /* accroche les buffers de la deuxieme partie a leur element */
              pBuf = pEl2->ElText;
              while (pBuf)
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
                      if (elBreak)
                        notifyTxt.targetdocument = 0;
                      else
                        notifyTxt.targetdocument = (Document)IdentDocument (pDoc);
                      CallEventType ((NotifyEvent *) & notifyTxt, FALSE);
                      pAsc = pAsc->ElParent;
                    }
                }
            }
          if (withAppEvent)
            {
              /* envoie l'evenement ElemNew.Post au nouvel element de texte*/
              notifyEl.event = TteElemNew;
              notifyEl.document = (Document) IdentDocument (pDoc);
              notifyEl.element = (Element) pEl2;
              notifyEl.info = 0; /* not sent by undo */
              notifyEl.elementType.ElTypeNum = pEl2->ElTypeNumber;
              notifyEl.elementType.ElSSchema = (SSchema)(pEl2->ElStructSchema);
              notifyEl.position = 0;
              CallEventType ((NotifyEvent *) & notifyEl, FALSE);
            }
        }
    }
}


/*--------------------------------------------------------------------
  MergeTextElements   si l'element pEl est une feuille de texte et si
  son suivant est egalement une feuille de texte dans le meme
  script, reunit les deux elements sucessifs en un seul.
  Le deuxieme element, devenu vide, n'est pas libere' et un
  pointeur sur cet element est retourne dans pFreeEl.
  Le parametre booleen removeAbsBox indique s'il faut liberer les
  paves de l'element qui va etre detruit.
  Cette fonction retourne TRUE si la fusion a lieu, FALSE sinon.
  ----------------------------------------------------------------------*/
ThotBool MergeTextElements (PtrElement pEl, PtrElement *pFreeEl, PtrDocument pDoc,
                            ThotBool withAppEvent, ThotBool removeAbsBox)
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
      
      if ((ElementIsReadOnly (pEl1)) || (ElementIsReadOnly (pEl2)))
        /*** One or both elements are protected! ***/
        return FALSE;
      
      if (pEl2 != NULL && pEl1->ElLeafType == LtText && pEl1->ElTerminal &&
          pEl2->ElLeafType == LtText && pEl2->ElTerminal &&
          pEl2->ElLanguage == pEl->ElLanguage &&
          pEl1->ElSource == NULL && pEl2->ElSource == NULL &&
          pEl1->ElStructSchema->SsRule->SrElem[pEl1->ElTypeNumber - 1]->SrConstruct != CsConstant &&
          pEl2->ElStructSchema->SsRule->SrElem[pEl2->ElTypeNumber - 1]->SrConstruct != CsConstant)
        {
          merge = TRUE;
          if (withAppEvent)
            {
              /* envoie l'evenement ElemDelete.Pre et demande a */
              /* l'application si elle est d'accord pour detruire le 2eme element */
              notifyEl.event = TteElemDelete;
              notifyEl.document = (Document) IdentDocument (pDoc);
              notifyEl.element = (Element) pEl2;
              notifyEl.info = 0; /* not sent by undo */
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
              if (pBuf1->BuLength + pBuf2->BuLength <= FULL_BUFFER)
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
                {
                  /* le 2eme element est reference */
                  if (pEl1->ElReferredDescr == NULL)
                    /* le premier element n'est pas reference' */
                    /* on met le descripteur d'element reference' du 2eme */
                    /* element sur le premier */
                    {
                      pEl1->ElReferredDescr = pEl2->ElReferredDescr;
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
                  notifyEl.info = 0; /* not sent by undo */
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
PtrTextBuffer CopyText (PtrTextBuffer pBuf, PtrElement pEl)
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
  CopyPath
  create a copy of the string of path elements pointed to by firstPathEl
  and return a pointer to the first element of the copy.
  ----------------------------------------------------------------------*/
PtrPathSeg CopyPath (PtrPathSeg firstPathEl)
{
  PtrPathSeg    pSourcePa, pPrevPa, pPa, first;
   
  pSourcePa = firstPathEl;
  first = NULL;
  pPrevPa = NULL;
  while (pSourcePa)
    {
      GetPathSeg (&pPa);
      *pPa = *pSourcePa;
      if (!first)
        {
          first = pPa;
          pPa->PaPrevious = NULL;
        }
      else
        {
          pPrevPa->PaNext = pPa;
          pPa->PaPrevious = pPrevPa;
        }
      pPa->PaNext = NULL;
      pPrevPa = pPa;
      pSourcePa = pSourcePa->PaNext;
    }
  return first;
}

/*----------------------------------------------------------------------
  StringAndTextEqual compare a character string (text) and the contents
  of text buffers.
  Return TRUE if equals.
  ----------------------------------------------------------------------*/
ThotBool StringAndTextEqual (const char *text, PtrTextBuffer pBuf)
{
  const unsigned char *ptr;
  wchar_t             wc;
  int                 i;
  int                 l, length;
  ThotBool            equal;

  equal = FALSE;
  if (pBuf == NULL && text == NULL)
    /* les deux sont vides, egalite' */
    equal = TRUE;
  else if (pBuf && text)
    {
      if (text[0] == EOS && pBuf->BuLength == 0)
        equal = TRUE;
      else
        {
          length = strlen (text);
          ptr = (unsigned char *)text;
          equal = TRUE;
          /* parcourt les buffers de texte successifs */
          while (pBuf && length > 0 && equal)
            {
              if (pBuf->BuLength > 0)
                {
                  l = 0;
                  i = 0;
                  while (equal && l < length && i < pBuf->BuLength)
                    {
                      l += TtaMBstringToWC (&ptr, &wc);
                      equal = (wc == pBuf->BuContent[i]);
                      i++;
                    }
                  length -= l;
                }
              pBuf = pBuf->BuNext;
            }
          if (pBuf || length > 0)
            equal = FALSE;
        }
    }
  return equal;
}


/*----------------------------------------------------------------------
  TextsEqual compare deux textes contenus dans des suites de     
  buffers. Retourne TRUE si les deux textes sont egaux.		
  ----------------------------------------------------------------------*/
ThotBool TextsEqual (PtrTextBuffer pBuf1, PtrTextBuffer pBuf2)
{
  PtrTextBuffer       pTB1, pTB2;
  STRING              pChar1;
  STRING              pChar2;
  int                 len, length1, length2;
  ThotBool            equal;

  equal = FALSE;
  if (pBuf1 == NULL && pBuf2 == NULL)
    equal = TRUE;
  else if (pBuf1 != NULL && pBuf2 != NULL)
    {
      pTB1 = pBuf1;
      length1 = pTB1->BuLength;
      pChar1 = &pTB1->BuContent[0];
      pTB2 = pBuf2;
      length2 = pTB2->BuLength;
      pChar2 = &pTB2->BuContent[0];
      equal = TRUE;
      /* parcourt les deux chaines de buffers */
      while (pTB1 != NULL && pTB2 != NULL && equal)
        {
          if (length1 == 0)
            {
              pTB1 = pTB1->BuNext;
              if (pTB1 != NULL)
                {
                  length1 = pTB1->BuLength;
                  pChar1 = &pTB1->BuContent[0];
                }
            }
          if (length2 == 0)
            {
              pTB2 = pTB2->BuNext;
              if (pTB2 != NULL)
                {
                  length2 = pTB2->BuLength;
                  pChar2 = &pTB2->BuContent[0];
                }
            }
          if (length1 > 0 && length2 > 0)
            {
              if (length1 > length2)
                len = length2;
              else
                len = length1;
              equal = (ustrncmp (pChar1, pChar2, len) == 0);
              length1 -= len;
              length2 -= len;
              pChar1 += len;
              pChar2 += len;
            }
        }
      if (pTB1 != NULL || pTB2 != NULL || length1 > 0 || length2 > 0)
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
void CopyTextToText (PtrTextBuffer pSrceBuf, PtrTextBuffer pCopyBuf, int *len)
{
  PtrTextBuffer       pTBSrce, pTBDest;

  *len = 0;
  if (pSrceBuf && pCopyBuf)
    {
      /* cherche le dernier buffer de la chaine des buffers destination */
      pTBDest = pCopyBuf;
      while (pTBDest->BuNext)
        pTBDest = pTBDest->BuNext;
      pTBSrce = pSrceBuf;
      /* parcourt la chaine des buffers source */
      while (pTBSrce)
        {
          if (pTBSrce->BuLength > 0)
            /* ce buffer source n'est pas vide, on le copie */
            {
              if (FULL_BUFFER - pTBDest->BuLength < pTBSrce->BuLength)
                /* pas assez de place dans le buffer destination */
                /* pour copier tout le texte du buffer source, on */
                /* prend un nouveau buffer destination */
                pTBDest = NewTextBuffer (pTBDest);
              ustrcpy (pTBDest->BuContent + pTBDest->BuLength,
                       pTBSrce->BuContent);
              pTBDest->BuLength += pTBSrce->BuLength;
              *len += pTBSrce->BuLength;
            }
          /* buffer source suivant */
          pTBSrce = pTBSrce->BuNext;
        }
    }
}


/*----------------------------------------------------------------------
  CopyMBs2Buffer copies max characters of the src string at the position pos in
  the buffer pBuf.
  In _I18N_ mode max is a UTF-8 string length;
  Adds new buffers if necessary and converts multibytes to wide characters
  if necessary.
  Returns the number of (wide )characters added in buffers.
  ----------------------------------------------------------------------*/
int CopyMBs2Buffer (const unsigned char *src, PtrTextBuffer pBuf, int pos, int slength)
{
  PtrTextBuffer       pNext;
  int                 l, length;
  int                 max = slength;

  /* number of wide characters added */
  length = 0;
  /* continue while there is still text */
  while (max > 0 && pBuf && src && src[0] != EOS)
    {
      l = 0;
      while (pos < FULL_BUFFER && max > l && src[0] != EOS)
        {
          l += TtaMBstringToWC (&src, &pBuf->BuContent[pos]);
          pos++;
          length++;
        }
      if (src[0] == EOS)
        max = 0;
      pBuf->BuLength = pos;
      pBuf->BuContent[pos] = EOS;
      max -= l;
      if (max > 0)
        {
          if (pBuf->BuNext == NULL)
            { 
              /* add a new buffer */
              GetTextBuffer (&pNext);
              if (pNext)
                {
                  pBuf->BuNext = pNext;
                  pNext->BuPrevious = pBuf;
                }
            }
          pBuf = pBuf->BuNext;
          pos = 0;
        }
    }
  return length;
}

/*----------------------------------------------------------------------
  CopyBuffer2MBs copies max characters from the position pos in the
  buffer pBuf to dest string.
  In _I18N_ mode max a UTF-8 string length
  Returns the number of Multi Byte characters copied in des.
  ----------------------------------------------------------------------*/
int CopyBuffer2MBs (PtrTextBuffer pBuf, int pos, unsigned char *des, int max)
{
  unsigned char       s[10], *ptr;
  int                 l, length;

  length = 0;
  /* continue while there is still text */
  while (max > 0 && pBuf)
    {
      l = 0;
      while (pos < pBuf->BuLength && max > 0)
        {
          ptr = s;
          l = TtaWCToMBstring (pBuf->BuContent[pos], &ptr);
          pos++;
          if (l <= max)
            {
              /* there is enough space to insert these characters */
              strncpy ((char *)&des[length], (char *)s, l);
              length += l;
            }
          max -= l;
        }

      if (max > 0)
        {
          /* move to the new buffer */
          pBuf = pBuf->BuNext;
          pos = 0;
        }
    }
  des[length] = EOS;
  return length;
}


/*----------------------------------------------------------------------
  CopyStringToBuffer adds the src string at the end of buffers.
  Adds new buffers if necessary and converts into wide characters.    
  Returns the number of copied characters.
  ----------------------------------------------------------------------*/
void CopyStringToBuffer (unsigned char *src, PtrTextBuffer pDestBuf, int *length)
{
  PtrTextBuffer       pBuf;
  int                 max;

  *length = 0;
  if (pDestBuf && src)
    {
      pBuf = pDestBuf;
      /* string length to copy */
      max = strlen ((char *)src);
      *length = max;
      /* look for the end of buffers */
      while (pBuf->BuNext != NULL)
        pBuf = pBuf->BuNext;
      CopyMBs2Buffer (src, pBuf, pBuf->BuLength, max);
    }
}


/*----------------------------------------------------------------------
  ClearText annule le texte contenu dans le buffer pBuf  
  et libere les buffers suivants.                                 
  ----------------------------------------------------------------------*/
void ClearText (PtrTextBuffer pBuf)
{
  PtrTextBuffer       pNextBuf;

  if (pBuf != NULL)
    {
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
  des points de controle de la polyline.                 
  On saute le point 0 (coordonnees du point limite).      
  ----------------------------------------------------------------------*/
int AddPointInPolyline (PtrTextBuffer firstBuffer, int rank, int x, int y,
			 ThotBool IsBarycenter, ThotBool IsClosed)
{
  PtrTextBuffer       pBuf, pNextBuf, pBufEnd;
  int                 i, j;
  PolyLinePoint       savePoint = {0,0}, oldSavePoint = {0,0};
  ThotBool            stop;
  ThotBool            addAtEnd = FALSE;

  PolyLinePoint previousPoint = {0, 0}, nextPoint = {0, 0};
  float coeff1, coeff2;

  int newrank = rank;

  if(IsBarycenter)
    {
      if(x + y == 0)
      /* The barycenter is ill-defined, take the center as a default point */
	x = y = 1;

      if(firstBuffer->BuLength - 1 < 2)
	{
	  /* We need at least two points to define a barycenter */
	  IsBarycenter = FALSE;
	  x = y = 0;
    	}
    }

  /* cherche le buffer contenant le point de rang rank */
  pBuf = firstBuffer;

  /* on saute le point 0 (coordonnees du point limite) */
  rank++;
  while (rank > pBuf->BuLength && pBuf->BuNext != NULL)
    {
      rank -= pBuf->BuLength;
      pBuf = pBuf->BuNext;
    }

  /* Is rank greater than the number of points? */
  if(rank > pBuf->BuLength)
    {
      rank = pBuf->BuLength + 1;
      addAtEnd = TRUE;
    }

  if(IsBarycenter)
    {
      /* Get the last buffer */
      for(pBufEnd = pBuf; pBufEnd->BuNext; pBufEnd = pBufEnd->BuNext);

      if(pBuf == firstBuffer && rank - 1 == 1)
	{
	  /*  we want to insert a point at the first position
	      O = position of the new point
	   */
	  if(IsClosed)
	    {
	      /*
		[LastPoint]---O----[FirstPoint]
		|                    |
		.--.--.--.--.--.--.--.
               */
	      addAtEnd = TRUE;
	      rank = pBufEnd->BuLength + 1;
	      newrank = -1;
	      pBuf = pBufEnd;
	      previousPoint.XCoord = pBufEnd->BuPoints[pBufEnd->BuLength - 1].XCoord;
	      previousPoint.YCoord = pBufEnd->BuPoints[pBufEnd->BuLength - 1].YCoord;
	      nextPoint.XCoord = firstBuffer->BuPoints[1].XCoord;
	      nextPoint.YCoord = firstBuffer->BuPoints[1].YCoord;
	    }
	  else
	    {
	      /* 
		      O--[FirstPoint]--.--.--
	       */
	      x = 1;
	      y = 0;
	      previousPoint.XCoord = 2*firstBuffer->BuPoints[1].XCoord
		- firstBuffer->BuPoints[2].XCoord;

	      previousPoint.YCoord = 2*firstBuffer->BuPoints[1].YCoord
		- firstBuffer->BuPoints[2].YCoord;
	    }
	}
      else if(addAtEnd)
	{
	  /*  we want to insert a point at the last position
	      O = position of the new point
	   */
	  if(IsClosed)
	    {
	      /*
		[FirstPoint]--O---[LastPoint]
		|                    |
		.--.--.--.--.--.--.--.
               */
	      rank = 2;
	      previousPoint.XCoord = firstBuffer->BuPoints[1].XCoord;
	      previousPoint.YCoord = firstBuffer->BuPoints[1].YCoord;
	      addAtEnd = FALSE;
	    }
	  else
	    {
	      /* 
		      --.--.--[LastPoint]--O
	       */
	      x = 0;
	      y = 1;
	      if(pBufEnd->BuLength > 2)
		{
		  savePoint.XCoord = pBufEnd->BuPoints[pBufEnd->BuLength - 2].XCoord;
		  savePoint.YCoord = pBufEnd->BuPoints[pBufEnd->BuLength - 2].YCoord;
		}
	      else
		{
		  savePoint.XCoord = pBufEnd->BuPrevious->BuPoints[pBufEnd->BuPrevious->BuLength - 1].XCoord;
		  savePoint.YCoord = pBufEnd->BuPrevious->BuPoints[pBufEnd->BuPrevious->BuLength - 1].YCoord;
		}

	      previousPoint.XCoord = pBufEnd->BuPoints[pBufEnd->BuLength - 1].XCoord;
	      previousPoint.YCoord = pBufEnd->BuPoints[pBufEnd->BuLength - 1].YCoord;
	      nextPoint.XCoord = 2*previousPoint.XCoord - savePoint.XCoord;
	      nextPoint.YCoord = 2*previousPoint.YCoord - savePoint.YCoord;
	    }
	}
      }

  if (addAtEnd)
    /* on va ajouter le nouveau point en derniere position */
    {
      
      if ((unsigned)rank > MAX_POINT_POLY)
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
          if ((unsigned)pNextBuf->BuLength < MAX_POINT_POLY)
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

  /* The point is a barycenter: compute the coordinates (x,y) */
  if(IsBarycenter)
    {
      coeff1 = x;
      coeff2 = y;

      if(!addAtEnd)
	{
	  if(rank < pBuf->BuLength)
	    {
	      /* The next point is at position rank */
	      nextPoint.XCoord = pBuf->BuPoints[rank].XCoord;
	      nextPoint.YCoord = pBuf->BuPoints[rank].YCoord;
	    }
	  else
	    {
	      nextPoint.XCoord = pBuf->BuNext->BuPoints[0].XCoord;
	      nextPoint.YCoord = pBuf->BuNext->BuPoints[0].YCoord;
	    }

	  if((pBuf == firstBuffer && rank > 2) ||
	     (pBuf != firstBuffer && rank >= 2))
	    {
	      /* the previous point is at rank - 2 */
	      previousPoint.XCoord = pBuf->BuPoints[rank - 2].XCoord;
	      previousPoint.YCoord = pBuf->BuPoints[rank - 2].YCoord;
	    }
	  else if(pBuf != firstBuffer)
	    {
	      /* the previous point is at the end of the previous buffer */
	      previousPoint.XCoord = pBuf->BuPrevious->BuPoints[pBuf->BuPrevious->BuLength - 1].XCoord;
	      previousPoint.YCoord = pBuf->BuPrevious->BuPoints[pBuf->BuPrevious->BuLength - 1].YCoord;
	    }
	}

      x = (int)(((coeff1*previousPoint.XCoord +
		  coeff2*nextPoint.XCoord)/(coeff2+coeff1)));
      
      y = (int)((coeff1*previousPoint.YCoord +
		 coeff2*nextPoint.YCoord)/(coeff2+coeff1));
    }

  /* met le nouveau point a sa place */
  pBuf->BuPoints[rank - 1].XCoord = x;
  pBuf->BuPoints[rank - 1].YCoord = y;
  
  /* verifie le point limite */
  if (x > firstBuffer->BuPoints[0].XCoord)
    firstBuffer->BuPoints[0].XCoord = x;
  if (y > firstBuffer->BuPoints[0].YCoord)
    firstBuffer->BuPoints[0].YCoord = y;

  return newrank;
}

/*----------------------------------------------------------------------
  DeletePointInPolyline detruit le point de rang rank dans la     
  liste des points de contro^le de la polyline.           
  On saute le point 0 (coordonnees du point limite).      
  Le pointeur firstBuffer peut e^tre modifie' par la      
  proce'dure.                                             
  ----------------------------------------------------------------------*/
void DeletePointInPolyline (PtrTextBuffer * firstBuffer, int rank)
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
void ModifyPointInPolyline (PtrTextBuffer firstBuffer, int rank, int x, int y)
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
  textLength: the number of characters contained in the element.
  In _I18N_ mode the length corresponds to the UTF-8 string.
  ----------------------------------------------------------------------*/
int TtaGetTextLength (Element element)
{
  PtrElement          pEl;
  int                 length;
  PtrTextBuffer       pBuf;
  unsigned char       c[10], *ptr;
  int                 i, l;

  UserErrorCode = 0;
  length = 0;
  pEl = (PtrElement) element;
  if (element == NULL)
    TtaError (ERR_invalid_parameter);
  else if (!pEl->ElTerminal)
    TtaError (ERR_invalid_element_type);
  else if (pEl->ElLeafType != LtText && pEl->ElLeafType != LtPicture)
    TtaError (ERR_invalid_element_type);
  else
    {
      length = pEl->ElTextLength;
      pBuf = pEl->ElText;
      l = 0;
      while (pBuf != NULL && length > 0)
        {
          i = 0;
          while (i < pBuf->BuLength && i < length)
            {
              ptr = c;
              l += TtaWCToMBstring (pBuf->BuContent[i], &ptr);
              i++;
            }
          /* next buffer */
          length -= pBuf->BuLength;
          pBuf = pBuf->BuNext;
        }
      length = l;
    }
  return length;
}

/*----------------------------------------------------------------------
  TtaGiveTextContent

  Returns the content of a Text basic element.
  Parameters:
  element: the element of interest. This element must be a basic
  element of type Text.
  buffer: the buffer that will contain the text. This buffer
  must be at least of size length.
  length: maximum length of that buffer.
  In _I18N_ mode the length corresponds to the UTF-8 string.
  Return parameters:
  buffer: (the buffer contains the text).
  length: actual length of the text in the buffer.
  language: language of the text.
  In _I18N_ mode returns a UTF-8 string.
  ----------------------------------------------------------------------*/
void TtaGiveTextContent (Element element, unsigned char *buffer, int *length,
                         Language *language)
{
  PtrElement          pEl;
  PtrTextBuffer       pBuf;
  unsigned char      *ptr;
  int                 len, l;

  UserErrorCode = 0;
  pEl = (PtrElement) element;
  if (element == NULL || buffer == NULL)
    TtaError (ERR_invalid_parameter);
  else if (!pEl->ElTerminal)
    TtaError (ERR_invalid_element_type);
  else if (pEl->ElLeafType != LtText && pEl->ElLeafType != LtPicture)
    TtaError (ERR_invalid_element_type);
  else
    {
      len = 0;
      pBuf = pEl->ElText;
      ptr = buffer;
      while (pBuf != NULL && len < (*length) - 1)
        {
          l = 0;
          while (l < pBuf->BuLength && len < *length)
            {
              len += TtaWCToMBstring (pBuf->BuContent[l], &ptr);
              l++;
            }
          pBuf = pBuf->BuNext;
        }
      *length = len;
      *ptr = EOS;
      *language = pEl->ElLanguage;
    }
}
/*----------------------------------------------------------------------
  TtaCopyPath : Copy Path linked list 
  ----------------------------------------------------------------------*/
void *TtaCopyPath (void *void_src)
{
#ifdef _GL
  int npoints;

  AnimPath *pop_path = (AnimPath *) void_src;
  AnimPath *new_path = (AnimPath *) TtaGetMemory (sizeof(AnimPath));

  memset (new_path, 0, sizeof(AnimPath));

  npoints = pop_path->npoints + 1;

  new_path->npoints = pop_path->npoints;
  new_path->length = pop_path->length;

  new_path->Path = (ThotPoint *)TtaGetMemory (npoints * sizeof(ThotPoint));
  new_path->Proportion = (float *)TtaGetMemory (npoints * sizeof(float));
  new_path->Tangent_angle = (float *)TtaGetMemory (npoints * sizeof(float));

  npoints --;

  memcpy (new_path->Path, pop_path->Path, npoints*sizeof(ThotPoint));
  memcpy (new_path->Proportion, pop_path->Proportion, npoints * sizeof(float));
  memcpy (new_path->Tangent_angle, pop_path->Tangent_angle, npoints * sizeof(float));

  return new_path;
#else /*_GL*/
  return NULL;
#endif /* _GL */
}
/*----------------------------------------------------------------------
  TtaCopyAnim : Copy anim linked list 
  ----------------------------------------------------------------------*/
void *TtaCopyAnim (void *void_src)
{
  Animated_Element *dest, *current;
  Animated_Element *src = (Animated_Element *) void_src;
  
  dest = (Animated_Element*)TtaNewAnimation ();
  current = dest; 
  while (src)
    {
      current->duration = src->duration;
      current->start = src->start;
      current->action_time = src->action_time;
      current->AnimType = src->AnimType;
      current->Fill = src->Fill;
      current->repeatCount = src->repeatCount;

      if (current->AnimType == Motion)
        {
          if (src->from)
            current->from = TtaCopyPath (src->from);
        }
      else
        {
          if (src->from)
            {	 
              current->from = TtaGetMemory (strlen ((char*)src->from) + 1);     
              strcpy ((char*)current->from, (char*)src->from); 
            }
          if (src->to)
            {
              current->to = TtaGetMemory (strlen ((char*)src->to) + 1);     
              strcpy ((char*)current->to, (char*)src->to);
            }
        }
      if (src->AttrName)
        switch (current->AnimType)
          {
          case Motion:
            break;
          case Color:
          case Set:
          case Animate:
            current->AttrName = TtaStrdup (src->AttrName);  
            break;   
          case Transformation:
            // the pointer is used as an interger
            current->AttrName = src->AttrName;      
            break; 
          default:
            break; 
          } 
      if (src->next)
        {
          current->next = (Animated_Element*)TtaNewAnimation ();
          current = current->next; 
        }
      else
        current->next = NULL;
     
      src = src->next;
    } 
  return (void *) dest;
}


/*----------------------------------------------------------------------
  TtaReplaceTransform

  Insert or Replace a Transform at the beginning of a Graphics basic element
  if a transformation of the same type exists in the list, it is replaced
  Parameters:
  element: the Graphics element to be modified.
  transform: the path segment to be inserted.
  document: the document containing the element.
  ----------------------------------------------------------------------*/
void TtaReplaceTransform (Element element, void *transform, 
                          Document document)
{
  PtrTransform       pPa, pPrevPa;

  UserErrorCode = 0;
  if (element == NULL)
    TtaError (ERR_invalid_parameter);
  else
    /* verifies the parameter document */
    if (document < 1 || document > MAX_DOCUMENTS)
      TtaError (ERR_invalid_document_parameter);
    else if (LoadedDocument[document - 1] == NULL)
      TtaError (ERR_invalid_document_parameter);
    else
      /* parameter document is correct */
      {
        if (element && transform)
          {
            pPa = ((PtrElement) element)->ElTransform;
            pPrevPa = NULL;
            while (pPa)
              {
                if (pPa->TransType == ((PtrTransform)transform)->TransType)
                  {
                    if (pPrevPa == NULL)
                      ((PtrElement) element)->ElTransform = (PtrTransform) transform;
                    else
                      pPrevPa->Next = (PtrTransform) transform;
                    ((PtrTransform) transform)->Next = pPa->Next;
                    TtaFreeMemory (pPa);	
                    return;
                  }	       
                pPrevPa = pPa;
                pPa = pPa->Next;
              }
            if (pPrevPa == NULL)
              ((PtrElement) element)->ElTransform = (PtrTransform) transform;
            else
              pPrevPa->Next = (PtrTransform) transform;
          }
      }
}

/*----------------------------------------------------------------------
  TtaCopyTransform
  Copy a Linked List of transforms into another struct
  ----------------------------------------------------------------------*/
void *TtaCopyTransform(void *void_pPa)
{
  PtrTransform result_first, current;  
  PtrTransform pPa = (PtrTransform) void_pPa;
  
  result_first = (Transform*)TtaNewTransform ();
  current = result_first;      
  while (pPa)
    {      
      current->TransType = pPa->TransType;    
      switch (pPa->TransType)
        {
        case PtElScale:
        case PtElTranslate:
        case PtElAnimTranslate:
          current->XScale = pPa->XScale;	  
          current->YScale = pPa->YScale;	  
          break;
        case PtElViewBox:
          current->VbXTranslate = pPa->VbXTranslate;
          current->VbYTranslate = pPa->VbYTranslate;
          current->VbWidth = pPa->VbWidth;
          current->VbHeight = pPa->VbHeight;
          current->VbAspectRatio = pPa->VbAspectRatio;
          current->VbMeetOrSlice = pPa->VbMeetOrSlice;
          break;
        case PtElRotate:
        case PtElAnimRotate:
          current->XRotate = pPa->XRotate;	  
          current->YRotate = pPa->YRotate;	  
          current->TrAngle = pPa->TrAngle;
          break;  
        case PtElMatrix:
          current->AMatrix = pPa->AMatrix;
          current->BMatrix = pPa->BMatrix;
          current->CMatrix = pPa->CMatrix;
          current->DMatrix = pPa->DMatrix;
          current->EMatrix = pPa->EMatrix;
          current->FMatrix = pPa->FMatrix;
          break;	  
        case PtElSkewX:
        case PtElSkewY:
          current->TrFactor = pPa->TrFactor;
          break;	  
        default:
          break;
        }	       
      if (pPa->Next)
        {
          current->Next = (Transform*)TtaGetMemory (sizeof (Transform));
          current = current->Next;
        }
      else
        current->Next = NULL;
      pPa = pPa->Next;
    }
  return (void *) result_first;  
}
/*----------------------------------------------------------------------
  TtaNewBoxTransformTranslate
  ---------------------------------------------------------------------- */
void *TtaNewBoxTransformTranslate (float x, float y)   
{
  PtrTransform pPa;

  pPa = (Transform*)TtaNewTransform ();
  pPa->TransType = PtElBoxTranslate;
  pPa->XScale = x;
  pPa->YScale = y;
  return (pPa);
}

/*----------------------------------------------------------------------
  TtaNewTransformTranslate
  ---------------------------------------------------------------------- */
void *TtaNewTransformTranslate (float x, float y)   
{
  PtrTransform pPa;

  pPa = (Transform*)TtaNewTransform ();
  pPa->TransType = PtElTranslate;
  pPa->XScale = x;
  pPa->YScale = y;
  return (pPa);
}
/*----------------------------------------------------------------------
  TtaNewTransformAnimTranslate
  ---------------------------------------------------------------------- */
void *TtaNewTransformAnimTranslate (float x, float y)   
{
  PtrTransform pPa;

  pPa = (Transform*)TtaNewTransform ();
  pPa->TransType = PtElAnimTranslate;
  pPa->XScale = x;
  pPa->YScale = y;
  return (pPa);
}
/*----------------------------------------------------------------------
  TtaNewTransformAnimRotate
  ---------------------------------------------------------------------- */
void *TtaNewTransformAnimRotate (float angle, float x_scale, 
                                 float y_scale)				   
{
  PtrTransform pPa;

  pPa = (Transform*)TtaNewTransform ();
  pPa->TransType = PtElAnimRotate;
  pPa->TrAngle = angle;
  pPa->XRotate = x_scale;
  pPa->YRotate = y_scale;
  return (pPa);
}

/*----------------------------------------------------------------------
  TtaNewTransformRotate
  ---------------------------------------------------------------------- */
void *TtaNewTransformRotate (float angle, float x_scale, 
                             float y_scale)				   
{
  PtrTransform pPa;

  pPa = (Transform*)TtaNewTransform ();
  pPa->TransType = PtElRotate;
  pPa->TrAngle = angle;
  pPa->XRotate = x_scale;
  pPa->YRotate = y_scale;
  return (pPa);
}

/*----------------------------------------------------------------------
  TtaNewTransformSkewX
  ---------------------------------------------------------------------- */
void *TtaNewTransformSkewX (float factor)
				   
{
  PtrTransform pPa;

  pPa = (Transform*)TtaNewTransform ();
  pPa->TransType = PtElSkewX;
  pPa->TrFactor = factor;
  return (pPa);
}

/*----------------------------------------------------------------------
  TtaNewTransformSkewY
  ---------------------------------------------------------------------- */
void *TtaNewTransformSkewY (float factor)
{
  PtrTransform pPa;

  pPa = (Transform*)TtaNewTransform ();
  pPa->TransType = PtElSkewY;
  pPa->TrFactor = factor;
  return (pPa);
}
/*----------------------------------------------------------------------
  TtaNewTransformScale
  ---------------------------------------------------------------------- */
void *TtaNewTransformScale (float x_scale, float y_scale)	   
{
  PtrTransform pPa;

  pPa = (Transform*)TtaNewTransform ();
  pPa->TransType = PtElScale;   
  pPa->XScale = x_scale;
  pPa->YScale = y_scale;
  return (pPa);
}

/*----------------------------------------------------------------------
  TtaNewTransformViewBox
  ---------------------------------------------------------------------- */
void *TtaNewTransformViewBox (float x, float y, float w, float h, int align, int meetOrSlice)	   
{
  PtrTransform pPa;

  pPa = (Transform*)TtaNewTransform ();
  pPa->TransType = PtElViewBox;
  pPa->VbXTranslate = x;
  pPa->VbYTranslate = y;
  pPa->VbWidth = w;
  pPa->VbHeight = h;
  pPa->VbAspectRatio = (ViewBoxAspectRatio) align;
  pPa->VbMeetOrSlice = (ViewBoxMeetOrSlice) meetOrSlice;
  return (pPa);
}

/*----------------------------------------------------------------------
  TtaNewTransformMatrix
  ---------------------------------------------------------------------- */
void *TtaNewTransformMatrix (float a, float b, float c,
                             float d, float e, float f)
{
  PtrTransform pPa;

  pPa = (Transform*)TtaNewTransform ();
  pPa->TransType = PtElMatrix;
  pPa->AMatrix = a;
  pPa->BMatrix = b;
  pPa->CMatrix = c;
  pPa->DMatrix = d;
  pPa->EMatrix = e;
  pPa->FMatrix = f;
  return (pPa);
}
