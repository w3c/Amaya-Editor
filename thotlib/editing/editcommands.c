/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2002
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * This module handles user commands.
 *
 * Author: I. Vatton (INRIA)
 *
 */

#include "thot_gui.h"
#include "thot_sys.h"
#include "libmsg.h"
#include "message.h"
#include "language.h"
#include "constmedia.h"
#include "typemedia.h"
#include "picture.h"
#include "appaction.h"
#include "appdialogue.h"

/* variables */
#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "boxes_tv.h"
#include "frame_tv.h"
#include "edit_tv.h"
#include "modif_tv.h"
#include "select_tv.h"
#include "appdialogue_tv.h"

/* Thot Clipboard information */
static PictInfo     PictClipboard;
static LeafType     ClipboardType;
static Language     ClipboardLanguage = 0;

/* X Clipboard */
static struct _TextBuffer XClipboard;

/* text element where the last insertion is done */
static PtrElement   LastInsertElText;
static PtrElement   LastInsertElement;
static int          LastInsertThotWindow;

/* attribute for which a presentation abstract box has been modified */
static PtrAttribute LastInsertAttr;
static PtrElement   LastInsertAttrElem;
static ThotBool     FromKeyboard;

#include "abspictures_f.h"
#include "actions_f.h"
#include "appdialogue_f.h"
#include "appli_f.h"
#include "applicationapi_f.h"
#include "attributes_f.h"
#include "boxmoves_f.h"
#include "boxlocate_f.h"
#include "boxparams_f.h"
#include "boxselection_f.h"
#include "buildboxes_f.h"
#include "buildlines_f.h"
#include "callback_f.h"
#include "changepresent_f.h"
#include "content_f.h"
#include "documentapi_f.h"
#include "docs_f.h"
#include "displayselect_f.h"
#include "displayview_f.h"
#include "editcommands_f.h"
#include "font_f.h"
#include "frame_f.h"
#include "geom_f.h"
#include "language_f.h"
#include "memory_f.h"
#include "picture_f.h"
#include "presentationapi_f.h"
#include "scroll_f.h"
#include "structcommands_f.h"
#include "structcreation_f.h"
#include "structmodif_f.h"
#include "structschema_f.h"
#include "structselect_f.h"
#include "textcommands_f.h"
#include "tree_f.h"
#include "uconvert_f.h"
#include "ustring_f.h"
#include "units_f.h"
#include "undo_f.h"
#include "unstructchange_f.h"
#include "viewapi_f.h"
#include "views_f.h"
#include "windowdisplay_f.h"

#ifdef _WINDOWS 
#include "wininclude.h"
#endif /* _WINDOWS */

/*----------------------------------------------------------------------
   CopyString computes the width of the source text and copies it into the
   target buffer if target parameter is not NULL.
  ----------------------------------------------------------------------*/
static void CopyString (Buffer source, Buffer target, int count,
			SpecFont font, int *sourceInd, int *targetInd,
			int *width, int *nSpaces, int *nChars)
{
  int                 nb;
  CHAR_T              car;

  nb = 0;
  while (nb < count)
    {
      car = source[*sourceInd];
      if (target)
	/* the copy is effectively done */
	target[*targetInd] = car;
      if (car == EOS)
	/* end of string: return the real length */
	nb = count;
      else
	{
	  (*sourceInd)++;
	  (*targetInd)++;
	  (nb)++;
	  (*nChars)++;
	  if (car == SPACE)
            (*nSpaces)++;
	  *width += BoxCharacterWidth (car, font);
	}
    }
}

/*----------------------------------------------------------------------
   RegisterInHistory
   register element pEl in the editing history, to allow UNDO
  ----------------------------------------------------------------------*/
static void RegisterInHistory (PtrElement pEl, int frame,
			       int firstCharIndex, int lastCharIndex)
{
   PtrDocument         pDoc;
   int                 view;
   ThotBool            opened;

   GetDocAndView (frame, &pDoc, &view);
   opened = pDoc->DocEditSequence;
   if (!opened)
     OpenHistorySequence (pDoc, pEl, pEl, firstCharIndex, lastCharIndex);
   AddEditOpInHistory (pEl, pDoc, TRUE, TRUE);
   if (!opened)
     CloseHistorySequence (pDoc);
}

/*----------------------------------------------------------------------
   APPtextModify envoie un message qui notifie qu'un texte est     
   modifie'.                                               
  ----------------------------------------------------------------------*/
static ThotBool APPtextModify (PtrElement pEl, int frame, ThotBool pre)
{
   PtrElement          pParentEl;
   ThotBool            result;
   NotifyOnTarget      notifyEl;
   PtrDocument         pDoc;
   int                 view;
   ThotBool            ok;

   GetDocAndView (frame, &pDoc, &view);
   result = FALSE;
   pParentEl = pEl;
   while (pParentEl != NULL)
     {
	notifyEl.event = TteElemTextModify;
	notifyEl.document = (Document) IdentDocument (pDoc);
	notifyEl.element = (Element) pParentEl;
	notifyEl.target = (Element) pEl;
	notifyEl.targetdocument = (Document) IdentDocument (pDoc);
	ok = CallEventType ((NotifyEvent *) & notifyEl, pre);
	result = result || ok;
	pParentEl = pParentEl->ElParent;
     }
   return result;
}

/*----------------------------------------------------------------------
   APPattrModify envoie un message qui notifie qu'un attribut est  
   modifie'.                                               
  ----------------------------------------------------------------------*/
static ThotBool APPattrModify (PtrAttribute pAttr, PtrElement pEl,
			       int frame, ThotBool pre)
{
   ThotBool            result;
   PtrDocument         pDoc;
   int                 view;
   NotifyAttribute     notifyAttr;

   GetDocAndView (frame, &pDoc, &view);
   notifyAttr.event = TteAttrModify;
   notifyAttr.document = (Document) IdentDocument (pDoc);
   notifyAttr.element = (Element) pEl;
   notifyAttr.attribute = (Attribute) pAttr;
   notifyAttr.attributeType.AttrSSchema = (SSchema) (pAttr->AeAttrSSchema);
   notifyAttr.attributeType.AttrTypeNum = pAttr->AeAttrNum;
   result = CallEventAttribute (&notifyAttr, pre);
   return result;
}


/*----------------------------------------------------------------------
   Retourne les informations sur le point d'insertion sachant que le 
   pave se'lectionne' est pAb (si pAb != NULL) :                     
   - la boite, le buffer, l'index dans le buffer,                  
   - le de'calage x depuis le de'but de la boite,                  
   - et le nombre de caracteres pre'ce'dents dans la boite.        
  ----------------------------------------------------------------------*/
static void GiveInsertPoint (PtrAbstractBox pAb, int frame, PtrBox *pBox,
			     PtrTextBuffer *pBuffer, int *ind, int *x,
			     int *previousChars)
{
   ViewSelection      *pViewSel;
   ThotBool            ok;
   ThotBool            endOfPicture;

   /* Si le pave n'est pas identifie on prend */
   /* le pave de la premiere boite selectionnee  */
   pViewSel = &ViewFrameTable[frame - 1].FrSelectionBegin;
   endOfPicture = FALSE;
   if (pAb == NULL && pViewSel->VsBox)
     pAb = pViewSel->VsBox->BxAbstractBox;

   /* S'il n'y a pas de pave selectionne */
   if (pAb != NULL)
     if (pAb->AbLeafType == LtPicture && pViewSel->VsIndBox == 1)
       endOfPicture =TRUE;
   /* Tant que le pave est un pave de presentation on saute au pave suivant */
   /* ne saute pas les paves de presentation modifiables, i.e. les paves */
   /* qui affichent la valeur d'un attribut */
   do
     {
       if (pAb)
	 ok = ((pAb->AbPresentationBox && !pAb->AbCanBeModified)
	       || (pAb->AbLeafType == LtPicture && endOfPicture));
       else
	 ok = FALSE;
       if (ok)
	 pAb = pAb->AbNext;
     }
   while (ok);

   if (pAb == NULL)
     {
	*pBox = NULL;
	*pBuffer = NULL;
	*ind = 0;
	*x = 0;
	*previousChars = 0;
	return;
     }
   else if (pAb->AbLeafType == LtText)
     {
	if (pViewSel->VsBox && pViewSel->VsBox->BxAbstractBox == pAb)
	  {
	     *pBox = pViewSel->VsBox;
	     *pBuffer = pViewSel->VsBuffer;
	     *ind = pViewSel->VsIndBuf;
	     *x = pViewSel->VsXPos;
	     *previousChars = pViewSel->VsIndBox;
	  }
	else
	  {
	     *pBox = pAb->AbBox;
	     /* Si la boite est coupee */
	     if ((*pBox)->BxType == BoSplit)
		*pBox = (*pBox)->BxNexChild;
	     *pBuffer = pAb->AbText;
	     *ind = 0;
	     *x = 0;
	     *previousChars = 0;
	  }
     }
   else
     {
	*pBox = pAb->AbBox;
	*pBuffer = NULL;
	*ind = 0;
	*x = 0;
	*previousChars = 0;
     }
}


/*----------------------------------------------------------------------
  CloseTextInsertionWithControl: finish the text insertion.
  Return TRUE if the current context could be modified by external
  application.
  ----------------------------------------------------------------------*/
static ThotBool CloseTextInsertionWithControl ()
{
  PtrElement          pEl;
  PtrBox              pBox;
  PtrBox              pSelBox;
  PtrTextBuffer       pBuffer;
  PtrTextBuffer       pbuff;
  ViewFrame          *pFrame;
  ViewSelection      *pViewSel;
  ViewSelection      *pViewSelEnd;
#if !defined(_WINDOWS) && !defined (_GTK)
  ThotEvent              event;
#endif /* _WINDOWS && _GTK */
  int                 nChars;
  int                 i, j;
  int                 ind;
  int                 frame;
  ThotBool            notified;

  /* No more enclosing cell */
  LastInsertCell = NULL;
  /* current frame for the selection */
  frame = ActiveFrame;
  notified = FALSE;
  if (frame > 0)
    {
      if (TextInserting)
	{
	  /* close the current insertion */
	  TextInserting = FALSE;
	  /* Where is the insert point (&i not used)? */
	  GiveInsertPoint (NULL, frame, &pSelBox, &pBuffer, &ind, &i, &j);
	  if (pSelBox == NULL || pSelBox->BxAbstractBox == NULL ||
	      pSelBox->BxAbstractBox->AbLeafType != LtText)
	    /* no more selection */
	    return (notified);
	  if (pBuffer && (ind > 0 || ind >= pBuffer->BuLength))
	    {
	      if (pBuffer->BuLength == 0 && pSelBox->BxBuffer != pBuffer)
		/* remove the empty buffer */
		pBuffer = DeleteBuffer (pBuffer, frame);
	    }
	  else if (pBuffer && pBuffer->BuPrevious)
	    {
	      pbuff = pBuffer->BuPrevious;
	      i = FULL_BUFFER - pbuff->BuLength;
	      nChars = pBuffer->BuLength;
	      if (pbuff->BuLength == 0 && pSelBox->BxNChars != 0)
		/* remove the empty buffer */
		pbuff = DeleteBuffer (pbuff, frame);
	      else if (nChars < 50 && i >= nChars)
		{
		  /* collaps buffer contents */
		  ustrncpy (&pbuff->BuContent[pbuff->BuLength],
			    &pBuffer->BuContent[0], nChars);
		  i = pbuff->BuLength;	/* old length */
		  /* update boxes */
		  if (j == 0)
		    pBox = pSelBox;
		  else
		    /* start from the next child */
		    pBox = pSelBox->BxNexChild;
		  while (pBox && pBox->BxBuffer == pBuffer)
		    {
		      pBox->BxBuffer = pbuff;
		      pBox->BxIndChar += i;
		      pBox = pBox->BxNexChild;
		    }

		  /* free the empty buffer */
		  pBuffer = DeleteBuffer (pBuffer, frame);
		  pbuff->BuLength += nChars;
		  pbuff->BuContent[pbuff->BuLength] = EOS;
		}
	    }

	  /* Check if the insertion starts at the beginning of a box */
	  pBox = pSelBox->BxAbstractBox->AbBox;
	  if (j == 0)
	    {
	      if (pSelBox->BxBuffer->BuLength != 0 &&
		  pSelBox->BxIndChar > pSelBox->BxBuffer->BuLength &&
		  pSelBox->BxNChars > 0)
		{
		/* the insertion at the end of a buffer was removed */
		pSelBox->BxIndChar = 0;
		}
	      /* update the split box */
	      if (pBox->BxBuffer != pSelBox->BxBuffer &&
		  pBox->BxNexChild == pSelBox)
		pBox->BxBuffer = pBuffer;
	      else if (ind == 0 && pBox->BxBuffer == pSelBox->BxBuffer)
		pBox->BxBuffer = pBuffer;
	      pSelBox->BxBuffer = pBuffer;
	    }
	  /* update the abstract box */
	  pSelBox->BxAbstractBox->AbText = pBox->BxBuffer;

	  /* Quand le texte insere' se trouve dans un bloc de lignes */
	  /* on reformate le bloc de ligne pour retirer les          */
	  /* compressions de texte et couper eventuellement les mots */
	  while (pBox)
	    {
	      if (pBox->BxAbstractBox == NULL)
		pBox = NULL;
	      else if (pBox->BxAbstractBox->AbEnclosing &&
		       pBox->BxAbstractBox->AbEnclosing->AbBox)
		{
		  pBox = pBox->BxAbstractBox->AbEnclosing->AbBox;
		  if (pBox->BxType == BoBlock)
		    {
		      LastInsertParagraph = pBox->BxAbstractBox;
		      LastInsertElement = LastInsertParagraph->AbElement;
		      LastInsertThotWindow = frame;
		      pBox = NULL;
		    }
		  else if (pBox->BxType != BoGhost)
		    pBox = NULL;
		}
	    }
	  pFrame = &ViewFrameTable[frame - 1];
	  pViewSel = &pFrame->FrSelectionBegin;
	  pViewSelEnd = &pFrame->FrSelectionEnd;
	  if (pSelBox->BxAbstractBox->AbPresentationBox &&
	      pSelBox->BxAbstractBox->AbCreatorAttr)
	    {
	      /* update the selection within the attribute */
	      FirstSelectedCharInAttr = pSelBox->BxFirstChar + pViewSelEnd->VsIndBox + 1;
	      LastSelectedCharInAttr = pSelBox->BxFirstChar + pViewSel->VsIndBox + 1;
	      SelPosition = (FirstSelectedCharInAttr < LastSelectedCharInAttr);
	    }
	  NewContent (pSelBox->BxAbstractBox);
	  /* update the new selection */
	  if (pViewSel->VsBox)
	    {
	      i = pViewSel->VsBox->BxFirstChar + pViewSel->VsIndBox;
	      /* Faut-il changer l'autre extremite de la selection ? */
	      pBox = pViewSelEnd->VsBox;
	      if (pBox)
		{
		  if (pBox->BxAbstractBox == pViewSel->VsBox->BxAbstractBox)
		    {
		      j = pBox->BxFirstChar + pViewSelEnd->VsIndBox;
		      ChangeSelection (frame, pViewSel->VsBox->BxAbstractBox, i,
				       FALSE, TRUE, FALSE, FALSE);
		      if (pViewSel->VsBox->BxAbstractBox != pBox->BxAbstractBox ||
			  i != j)
			ChangeSelection (frame, pBox->BxAbstractBox, j, TRUE, TRUE,
					 FALSE, FALSE);
		    }
		  else
		    ChangeSelection (frame, pViewSel->VsBox->BxAbstractBox, i,
				     FALSE, TRUE, FALSE, FALSE);
		}
	      else
		ChangeSelection (frame, pViewSel->VsBox->BxAbstractBox, i, FALSE,
				 TRUE, FALSE, FALSE);
	      /* Nouvelle position de reference du curseur */
	      ClickX = pViewSel->VsBox->BxXOrg + pViewSel->VsXPos - pFrame->FrXOrg;
	    }
	     
	  if (LastInsertElText)
	    {
	      /* Notify the end of text insertion */
	      pEl = LastInsertElText;
	      LastInsertElText = NULL;
	      APPtextModify (pEl, frame, FALSE);
	      notified = TRUE;
	    }
	  else if (LastInsertAttr)
	    {
	      /* Notify the end of attribute change */
	      LastInsertAttr = NULL;
	      LastInsertAttrElem = NULL;
	      /* the notification was already done by NewContent */
	      notified = TRUE;
	    }
	}
    }

#ifndef _WINDOWS
#ifndef _GTK
  /* remove waiting expose events */
  while (XCheckMaskEvent (TtDisplay, (long) ExposureMask, (ThotEvent *) &event))
    {
      if (event.type == GraphicsExpose || event.type == Expose)
	{
	  frame = GetWindowFrame (event.xexpose.window);
	  FrameToRedisplay (event.xexpose.window, frame, (XExposeEvent *) & event);
	  XtDispatchEvent (&event);
	}
    }
#endif /*_GTK */
#endif /* !_WINDOWS */
  return (notified);
}


/*----------------------------------------------------------------------
   SetInsert determine le point d'insertion en fonction de la      
   selection courante et de la nature attendue :           
   On verifie avant tout que le debut de la selection      
   visualisee correspond bien au debut de la selection     
   reelle (le debut de selection peut se trouver dans un   
   element de document non visualise).                     
   - Si la nature est LtReference n'importe qu'elle nature 
   terminale (texte, image, graphique symbole) est         
   autorisee.                                              
   - Si le pave est un pave compose vide, on insere dedans.
   - Si le pave correspond a la nature, on insere dedans.  
   - Si le pave precedent a la meme nature, on insere a`   
   la fin du pave.                                         
   - Sinon, on insere un pave de cette nature avant le pave
   selectionne.                                            
   Le parametre del est Vrai si on cherche a detruire un   
   caractere. Si c'est le cas et que la marque d'insertion 
   se trouve en debut d'un pave, la procedure regarde si   
   la destruction de caractere peut s'appliquer au pave    
   precedent.                                              
  ----------------------------------------------------------------------*/
static void SetInsert (PtrAbstractBox *pAb, int *frame, LeafType nat, ThotBool del)
{
  PtrAbstractBox      pSelAb;
  PtrBox              pBox;
  ViewFrame          *pFrame;
  ViewSelection      *pViewSel;
  LeafType            natureToCreate;
  int                 i;
  ThotBool            moveSelection;
  ThotBool            notified, before;
  
  *pAb = NULL;
  notified = FALSE;
  /* verifie la validite du debut de la selection */
  if (*frame > 0)
    {
      pFrame = &ViewFrameTable[*frame - 1];
      pViewSel = &pFrame->FrSelectionBegin;
      /* Si la selection porte sur des caracteres, le debut de la selection */
      /* visualisee correspond forcement au debut de la selection reelle    */
      /* Sinon il faut verifier la validite de la selection */
      if (pViewSel->VsBox == NULL || pViewSel->VsIndBox == 0)
	TtaSetFocus ();
      pBox = pViewSel->VsBox;

      /* Il faut maintenant verifier la nature demandee */
      i = 1;			/* Par defaut insere avant le 1er caractere */
      moveSelection = FALSE;
      if (nat == LtReference)
	natureToCreate = LtText;
      else
	natureToCreate = nat;
      
      if (pBox == NULL)
	*pAb = NULL;
      else
	*pAb = pBox->BxAbstractBox;

      pSelAb = *pAb;
      if (pSelAb)
	{
	  /* deplace l'insertion avant le pave selectionne pour detruire */
	  if (del && pViewSel->VsIndBox == 0 &&
	      (pBox == pSelAb->AbBox || pBox == pSelAb->AbBox->BxNexChild))
	    {
	      pSelAb = pSelAb->AbPrevious;
	      *pAb = NULL;
	      if (pSelAb != NULL && pSelAb->AbCanBeModified &&
		  !pSelAb->AbReadOnly && pSelAb->AbLeafType == natureToCreate)
		{
		  moveSelection = TRUE;
		  notified = CloseTextInsertionWithControl ();
		  if (!notified)
		    {
		      i = pSelAb->AbVolume + 1;
		      *pAb = pSelAb;
		    }
		}
	    }
	  /* deplace l'insertion dans le pave de composition vide */
	  else if (pSelAb->AbLeafType == LtCompound && pSelAb->AbFirstEnclosed == NULL)
	    {
	      *pAb = CreateALeaf (pSelAb, frame, natureToCreate, TRUE);
	      moveSelection = TRUE;
	    }
	  /* deplace l'insertion avant ou apres le pave selectionne */
	  else if (!pSelAb->AbCanBeModified || pSelAb->AbReadOnly ||
		   pSelAb->AbLeafType == LtCompound ||
		   (pSelAb->AbLeafType != nat && nat != LtReference &&
		    !(pSelAb->AbLeafType == LtSymbol && nat == LtText) &&
		    !(pSelAb->AbLeafType == LtPolyLine && nat == LtGraphics)))
	    {
	      moveSelection = TRUE;
	      if (pViewSel->VsXPos > 0)
		{
		  /* insert after */
		  before = FALSE;
		  pSelAb = pSelAb->AbNext;
		}
	      else
		{
		  /* insert before */
		  before = TRUE;
		  pSelAb = pSelAb->AbPrevious;
		}
	      notified = CloseTextInsertionWithControl ();
	      if (pSelAb == NULL)
		*pAb = CreateALeaf (*pAb, frame, natureToCreate, before);
	      else if (!pSelAb->AbCanBeModified || pSelAb->AbReadOnly ||
		        natureToCreate != LtText ||
		        pSelAb->AbLeafType != natureToCreate)
		*pAb = CreateALeaf (*pAb, frame, natureToCreate, !before);
	      else
		{
		  if (before)
		    i = pSelAb->AbVolume + 1;
		  else
		    i = 1;
		  *pAb = pSelAb;
		}
	    }
	}
	  
      if (notified)
	/* selection could be modified by the application re-do the work */
	SetInsert (pAb, frame, nat, del);
      else
	{
	  pSelAb = *pAb;
	  if (pSelAb && pSelAb->AbElement && moveSelection)
	    {
	    /* signale le changement de selection a l'editeur */
	    if (pSelAb->AbVolume == 0)
	      ChangeSelection (*frame, pSelAb, 0, FALSE, TRUE, FALSE, FALSE);
	    else
	      ChangeSelection (*frame, pSelAb, i, FALSE, TRUE, FALSE, FALSE);
	    }
	}
    }
}


/*----------------------------------------------------------------------
   Cree un buffer apres celui donne en parametre.                  
   Met a jour les chainages et rend le pointeur sur le nouveau     
   buffer.                                                         
  ----------------------------------------------------------------------*/
static PtrTextBuffer GetNewBuffer (PtrTextBuffer pBuffer, int frame)
{
  PtrTextBuffer       pNewBuffer;
  ViewFrame          *pFrame;
  ViewSelection      *pViewSel;
  ViewSelection      *pViewSelEnd;

  GetTextBuffer (&pNewBuffer);
  pNewBuffer->BuPrevious = pBuffer;
  if (pBuffer == NULL)
    pNewBuffer->BuNext = NULL;
  else
    {
      pNewBuffer->BuNext = pBuffer->BuNext;
      pBuffer->BuNext = pNewBuffer;
      if (pNewBuffer->BuNext != NULL)
	pNewBuffer->BuNext->BuPrevious = pNewBuffer;
    }

  if (frame)
    {
      /* update selection marks */
      pFrame = &ViewFrameTable[frame - 1];
      pViewSel = &pFrame->FrSelectionBegin;
      if (pViewSel->VsBuffer == pBuffer)
	{
	  if (pFrame->FrSelectionEnd.VsBuffer == pViewSel->VsBuffer)
	    {
	      pViewSelEnd = &pFrame->FrSelectionEnd;
	      pViewSelEnd->VsBuffer = pNewBuffer;
	      pViewSelEnd->VsIndBuf = pViewSelEnd->VsIndBuf - pViewSel->VsIndBuf;
	    }
	  pViewSel->VsBuffer = pNewBuffer;
	  pViewSel->VsIndBuf = 0;
	}
    }
  return pNewBuffer;
}

/*----------------------------------------------------------------------
   Copie les caracteres de la chaine de buffers pSourceBuffer      
   a partir de startInd, jusqu'a endInd du buffer pEndBuffer       
   vers la chaine de buffers pTargetBuffer a partir de             
   targetInd si pTargetBuffer n'est pas Nil.                       
   Si pTargetBuffer vaut NULL, evalue simplement les valeurs       
   width, nSpaces et nChars. Si pEndBuffer vaut NULL endInd doit   
   valoir 0.                                                       
   Rend :                                                          
   - le pointeur sur le dernier buffer destination utilise.        
   - la largeur, le nombre de blancs et de caracteres copies.      
  ----------------------------------------------------------------------*/
static void CopyBuffers (SpecFont font, int frame, int startInd, int endInd,
			 int targetInd, PtrTextBuffer pSourceBuffer,
			 PtrTextBuffer pEndBuffer,
			 PtrTextBuffer *pTargetBuffer,
			 int *width, int *nSpaces, int *nChars)
{
   PtrTextBuffer       pBuffer;
   STRING              target;
   int                 sourceInd;
   int                 sourceLength;
   int                 targetlength;

   /* get the starting point of the copy */
   pBuffer = pSourceBuffer;
   sourceInd = startInd;
   *width = 0;
   *nChars = 0;
   *nSpaces = 0;
   while (pBuffer)
     {
	if (*pTargetBuffer == NULL)
	  {
	     /* we don't want to effectively copy the text */
	     targetInd = 0;
	     target = NULL;
	  }
	else
	   target = (*pTargetBuffer)->BuContent;

	if (pBuffer == pEndBuffer)
	  {
	     /* prevent buffer overflow */
	     if (endInd > pEndBuffer->BuLength)
		sourceLength = pEndBuffer->BuLength - sourceInd;
	     else
		sourceLength = endInd - sourceInd;
	  }
	else
	   sourceLength = pBuffer->BuLength - sourceInd;
	targetlength = FULL_BUFFER - targetInd;
	if (sourceLength <= targetlength)
	  {
	    /* end of source buffer */
	    CopyString (pBuffer->BuContent, target, sourceLength,
			font, &sourceInd, &targetInd, width, nSpaces,
			nChars);
	    if (pBuffer == pEndBuffer)
	      pBuffer = NULL;
	    else
	      pBuffer = pBuffer->BuNext;
	    sourceInd = 0;
	  }
	else
	  {
	    /* end of target buffer */
	    CopyString (pBuffer->BuContent, target, targetlength,
			font, &sourceInd, &targetInd, width, nSpaces,
			nChars);
	    (*pTargetBuffer)->BuLength = FULL_BUFFER;
	    (*pTargetBuffer)->BuContent[FULL_BUFFER] = EOS;
	    *pTargetBuffer = GetNewBuffer (*pTargetBuffer, frame);
	    targetInd = 0;
	  }
     }

   if (*pTargetBuffer)
     {
	/* text was copied: update target buffer information */
	(*pTargetBuffer)->BuLength = targetInd;
	(*pTargetBuffer)->BuContent[targetInd] = EOS;
     }
}

/*----------------------------------------------------------------------
   Debute l'insertion de caracteres dans une boite de texte.      
  ----------------------------------------------------------------------*/
static void StartTextInsertion (PtrAbstractBox pAb, int frame, PtrBox pSelBox,
				PtrTextBuffer pBuffer, int ind, int prev)
{
   PtrBox              pBox;
   PtrTextBuffer       pPreviousBuffer;
   PtrTextBuffer       pNewBuffer;
   int                 k, i;

   /* get the active frame for the selection */
   if (frame == 0)
     frame = ActiveFrame;
   if (frame > 0)
     {
	/* get the insert point (&i not used) */
        if (pAb == NULL)
	   GiveInsertPoint (NULL, frame, &pSelBox, &pBuffer, &ind, &i, &prev);
	TextInserting = TRUE;
	/* the split box */
	pBox = pSelBox->BxAbstractBox->AbBox;

	/* Notify the application that the content will be changed */
	if (pSelBox->BxAbstractBox->AbPresentationBox &&
	    pSelBox->BxAbstractBox->AbCanBeModified)
	  {
	    /* it's an abstract box that displays the attribute value */
	     if (LastInsertAttr != pBox->BxAbstractBox->AbCreatorAttr)
	       {
		  LastInsertAttr = pBox->BxAbstractBox->AbCreatorAttr;
		  LastInsertAttrElem = pBox->BxAbstractBox->AbElement;
		  APPattrModify (LastInsertAttr, LastInsertAttrElem, frame, TRUE);
		  /* Don't register the editing operation in the history:
		     procedure NewContent does the job */
	       }
	  }
	else if (LastInsertElText != pBox->BxAbstractBox->AbElement)
	  {
	    /* it's a text element */
	     LastInsertElText = pBox->BxAbstractBox->AbElement;
	     APPtextModify (LastInsertElText, frame, TRUE);
	     /* register the editing operation in the history */
	     i = FirstSelectedChar;
	     k = LastSelectedChar;
	     if (SelPosition)
	        k--;
	     RegisterInHistory (LastInsertElText, frame, i, k);
	  }

	/* Memorize  the enclosing cell */
	LastInsertCell = GetParentCell (pBox);
	/* First buffer of the abstract box */
	pNewBuffer = pSelBox->BxAbstractBox->AbText;
	/* Insertion at the beginning of the box */
	if (ind == 0 && prev == 0 && pBuffer->BuLength > 0)
	  {
	     /* previous buffer */
	     pPreviousBuffer = pBuffer->BuPrevious;
	     if (pNewBuffer == pBuffer)
	       {
		 /* add a buffer before */
		  GetTextBuffer (&pNewBuffer);
		  pNewBuffer->BuNext = pBuffer;
		  pNewBuffer->BuLength = 0;
		  pNewBuffer->BuContent[0] = EOS;
		  if (pPreviousBuffer == NULL)
		     pNewBuffer->BuPrevious = NULL;
		  else
		    {
		       pNewBuffer->BuPrevious = pPreviousBuffer;
		       pPreviousBuffer->BuNext = pNewBuffer;
		    }
		  pBuffer->BuPrevious = pNewBuffer;
		  /* update the split box */
		  pBox->BxBuffer = pNewBuffer;
		  /* update the selected box */
		  pSelBox->BxAbstractBox->AbText = pNewBuffer;
		  pSelBox->BxIndChar = 0;
		  pSelBox->BxBuffer = pNewBuffer;
	       }
	     else
	       {
		 /* add into the existing previous buffer */
		  pSelBox->BxIndChar = pPreviousBuffer->BuLength;
		  pSelBox->BxBuffer = pPreviousBuffer;
	       }
	  }
	else if (ind > 0 && ind < pBuffer->BuLength)
	  {
	    /* split the current buffer to prepare the insertion */
	    pNewBuffer = pBuffer;
	    pBuffer = GetNewBuffer (pBuffer, frame);
	    /* move the beginning of the buffer */
	    k = ustrlen (&pNewBuffer->BuContent[ind]);
	    ustrncpy (&pBuffer->BuContent[0], &pNewBuffer->BuContent[ind], k);
	    pBuffer->BuContent[k] = EOS;
	    pBuffer->BuLength = k;
	    pNewBuffer->BuContent[ind] = EOS;
	    pNewBuffer->BuLength = ind;
	    /* update following piece of a split box */
	    pBox = pSelBox->BxNexChild;
	    while (pBox)
	      {
		if (pBox->BxBuffer == pNewBuffer)
		  {
		    pBox->BxBuffer = pBuffer;
		    pBox->BxIndChar -= ind;
		    pBox = pBox->BxNexChild;
		  }
		else
		  pBox = NULL;
	      }
	  }
     }
}

/*----------------------------------------------------------------------
   Assure la presence d'un atome texte de la bonne langue a la      
   position indiquee (qui doit etre un atome texte).
  ----------------------------------------------------------------------*/
static void NewTextLanguage (PtrAbstractBox pAb, int charIndex, Language lang)
{
  PtrElement          pEl, pNextEl, pSecond, pNext;
  PtrDocument         pDoc;

  pEl = pAb->AbElement;
  if (pEl != NULL && pEl->ElStructSchema != NULL)
    if (pEl->ElLeafType == LtText)
      {
	pDoc = DocumentOfElement (pEl);
	if (pEl->ElTextLength > 0)
	  {
	    pNext = pEl->ElNext;
	    SplitTextElement (pEl, charIndex, pDoc, FALSE, &pNextEl, FALSE);
	    BuildAbsBoxSpliText (pEl, pNextEl, pNext, pDoc);
	    if (pEl->ElTextLength > 0 && pNextEl->ElTextLength > 0)
	      {
		pNext = pNextEl->ElNext;
		SplitTextElement (pNextEl, 1, pDoc, FALSE, &pSecond, FALSE);
		BuildAbsBoxSpliText (pNextEl, pSecond, pNext, pDoc);
		pEl = pNextEl;
	      }
	    else if (pEl->ElTextLength > 0)
	      pEl = pNextEl;
	    AbstractImageUpdated (pDoc);
	  }
	if (pEl && pEl->ElStructSchema)
	  {
	    /* change la langue dans la feuille de texte */
	    ChangeLanguage (pDoc, pEl, lang, TRUE);
	    AbstractImageUpdated (pDoc);
	    RedisplayDocViews (pDoc);
	    SelectElement (pDoc, pEl, FALSE, FALSE);
	  }
      }
}

/*----------------------------------------------------------------------
   Create a new element or move to previous element if the language changes
   Return TRUE if there is a notification to the application and the
   current selection could be modified.
  ----------------------------------------------------------------------*/
static ThotBool GiveAbsBoxForLanguage (int frame, PtrAbstractBox *pAb, int keyboard)
{
  PtrAbstractBox      pSelAb;
  PtrBox              pBox;
  PtrAttribute        pHeritAttr;
  PtrElement          pElAttr;  
  ViewSelection      *pViewSel;
  Language            language, plang;
  unsigned char       text[100];
  int                 index;
  ThotBool            cut;
  ThotBool            notification;

  language = 0;
  pViewSel = &ViewFrameTable[frame - 1].FrSelectionBegin;
  notification = FALSE;
  pSelAb = *pAb;
  plang = pSelAb->AbLang;
  if (keyboard == -1)
    /* l'utilisateur a saisi un caractere au clavier */
    if (plang < TtaGetFirstUserLanguage())
      /* le contenu du pave courant avait ete saisi par palette */
      /* recherche la langue dans les ancetres */
      {
	pHeritAttr = GetTypedAttrAncestor (pSelAb->AbElement->ElParent, 1,
					   NULL, &pElAttr); 
	if (pHeritAttr != NULL && pHeritAttr->AeAttrText != NULL)
	  {
	    CopyBuffer2MBs (pHeritAttr->AeAttrText, 0, text, 100);
	    language = TtaGetLanguageIdFromName (text);
	  }
	else
	  /* les ancetres n'ont pas d'attribut langue */
	  language = TtaGetDefaultLanguage ();
      }
    else
      language = plang;
  else if (keyboard == 2)
    /* insert a standard character */
    if (TtaGetScript (plang) == 'L')
      language = plang;
    else
      language = TtaGetDefaultLanguage ();
  else if (keyboard == 3)
    /* insert a character from the greek palette */
    language = TtaGetLanguageIdFromScript ('G');
  else
    language = 0;

  if (pSelAb->AbLeafType == LtText)
    if (plang != language && plang != 0)
      {
	notification = CloseTextInsertionWithControl ();
	if (!notification)
	  {
	    /* the selection could be modified by the application */
	    cut = TRUE;
	    pBox = pViewSel->VsBox;
	    if (pBox)
	      {
		index = pBox->BxFirstChar + pViewSel->VsIndBox;
		if (index <= 1)
		  {
		    pSelAb = pSelAb->AbPrevious;
		    if (pSelAb &&
			pSelAb->AbLeafType == LtText &&
			pSelAb->AbLang == language &&
			pSelAb->AbCanBeModified && !pSelAb->AbReadOnly)
		      {
			cut = FALSE;
			ChangeSelection (frame, pSelAb, pSelAb->AbVolume + 1,
					 FALSE, TRUE, FALSE, FALSE);
		      }
		  }
		
		/* S'il faut couper, on appelle l'editeur */
		if (cut)
		  NewTextLanguage (*pAb, index, language);
		/* la boite peut avoir change */
		pBox = pViewSel->VsBox;
		if (pBox)
		  *pAb = pBox->BxAbstractBox;
		else
		  *pAb = NULL;
	      }
	  }
      }
  return (notification);
}


/*----------------------------------------------------------------------
  IsLineBreakInside returns TRUE if a line-break character is found in
  the current string.
  Parameters pBuffer and ind give the starting character of the string
  and length gives the string length.
  ----------------------------------------------------------------------*/
static ThotBool IsLineBreakInside (PtrTextBuffer pBuffer, int ind, int length)
{
   int                 c;
   int                 nChars;
   int                 i;
   int                 j;

   if (pBuffer == NULL)
     return (FALSE);
   if (ind >= pBuffer->BuLength && pBuffer->BuNext)
     {
       /* End of the current buffer -> start with the next buffer */
	pBuffer = pBuffer->BuNext;
	i = 0;
     }
   else
      i = ind;
   /* number of characters in the buffer */
   nChars = pBuffer->BuLength;
   j = 0;

   /* Recherche s'il y a un caractere de coupure forcee */
   while (i < nChars && j < length)
     {
	c = pBuffer->BuContent[i];
	if (c == BREAK_LINE || c == NEW_LINE)
	  /* a break-line found */
	   return (TRUE);
	/* Next character */
	i++;
	if (i == nChars && pBuffer->BuNext)
	  {
	    pBuffer = pBuffer->BuNext;
	    i = 0;
	    nChars = pBuffer->BuLength;
	  }
	j++;
     }
   return (FALSE);
}


/*----------------------------------------------------------------------
   TtcInsertGraph insert a graphics                                
  ----------------------------------------------------------------------*/
void  TtcInsertGraph (Document document, View view, unsigned char c)
{
   DisplayMode         dispMode;
   int                 frame;
   ThotBool            lock = TRUE;

   if (document != 0)
     {
       /* avoid to redisplay step by step */
       dispMode = TtaGetDisplayMode (document);
       if (dispMode == DisplayImmediately)
	 TtaSetDisplayMode (document, DeferredDisplay);
       /* lock tables formatting */
       if (ThotLocalActions[T_islock])
	 {
	   (*ThotLocalActions[T_islock]) (&lock);
	   if (!lock)
	     /* table formatting is not loked, lock it now */
	     (*ThotLocalActions[T_lock]) ();
	 }

	frame = GetWindowNumber (document, view);
	InsertChar (frame, c, 1);

	if (!lock)
	  /* unlock table formatting */
	  (*ThotLocalActions[T_unlock]) ();
	if (dispMode == DisplayImmediately)
	  TtaSetDisplayMode (document, dispMode);
     }
}


/*----------------------------------------------------------------------
   Termine l'insertion de caracteres dans une boite de texte       
  ----------------------------------------------------------------------*/
void CloseTextInsertion ()
{
  ThotBool withAppliControl;

  withAppliControl = CloseTextInsertionWithControl ();
}

/*----------------------------------------------------------------------
   CloseParagraphInsertion teste s'il faut reformater un paragraphe  
   suite a` une insertion.                                 
   Le dernier bloc de lignes e'dite' est reformate' de`s   
   que la se'lection sort du bloc de lignes. Le but est de 
   retirer les extensions/compressions de texte et couper  
   e'ventuellement les mots.                               
  ----------------------------------------------------------------------*/
void CloseParagraphInsertion (PtrAbstractBox pAb, int frame)
{
  PtrBox              pBox;

  if (LastInsertParagraph != NULL)
    {
    if (LastInsertParagraph->AbElement != LastInsertElement
	|| LastInsertParagraph->AbBox == NULL)
      /* Ce n'est plus la peine de reformater le bloc de lignes */
      LastInsertParagraph = NULL;
    else if (LastInsertThotWindow != frame)
      {
	/* a change a priori de paragraphe */
	/* Reevalue le bloc de lignes */
	RecomputeLines (LastInsertParagraph, NULL, NULL, LastInsertThotWindow);
	/* Et l'affiche */
	RedrawFrameBottom (LastInsertThotWindow, 0, NULL);
	LastInsertParagraph = NULL;
      }
    else
      {
	/* Est-ce que la selection a quitte le dernier bloc de ligne */
	pAb = pAb->AbEnclosing;
	while (pAb != NULL)
	  if (pAb == LastInsertParagraph || pAb->AbBox == NULL)
	    /* n'a pas change de paragraphe */
	    pAb = NULL;
	  else
	    {
	      pBox = pAb->AbBox;
	      if (pBox != NULL)
		{
		  if (pBox->BxType == BoGhost)
		    pAb = pAb->AbEnclosing;
		  else
		    {
		      /* Reevalue le bloc de lignes */
		      RecomputeLines (LastInsertParagraph, NULL, NULL, LastInsertThotWindow);
		      /* Et l'affiche */
		      RedrawFrameBottom (LastInsertThotWindow, 0, NULL);
		      LastInsertParagraph = NULL;
		      pAb = NULL;
		    }
		}
	    }
      }
    }
}

/*----------------------------------------------------------------------
   insere dans la boite pBox.                                      
  ----------------------------------------------------------------------*/
static void LoadSymbol (char c, PtrLine pLine, ThotBool defaultHeight,
			ThotBool defaultWidth, PtrBox pBox, PtrAbstractBox pAb,
			int frame)
{
   int                 xDelta, yDelta;

   if (!APPgraphicModify (pAb->AbElement, (int)c, frame, TRUE))
     {
       pAb->AbShape = c;
       /* Dimensions du symbole */
       pAb->AbVolume = 1;
       if (defaultWidth || defaultHeight)
	 GiveSymbolSize (pAb, &xDelta, &yDelta);
       
       /* met a jour la boite */
       if (defaultWidth)
	 xDelta -= pBox->BxWidth;
       else
	 xDelta = 0;
       
       if (defaultHeight)
	 yDelta -= pBox->BxHeight;
       else
	 yDelta = 0;
       
       BoxUpdate (pBox, pLine, 0, 0, xDelta, 0, yDelta, frame, FALSE);
       /* adjust the width of some symbols */
       ResizeHeight (pBox, NULL, NULL, 0, 0, 0, frame);
       APPgraphicModify (pAb->AbElement, (int)c, frame, FALSE);
     }
}


/*----------------------------------------------------------------------
   Charge un graphique ou une polyline.                            
  ----------------------------------------------------------------------*/
static void LoadShape (char c, PtrLine pLine, ThotBool defaultHeight,
		       ThotBool defaultWidth, PtrBox pBox, PtrAbstractBox pAb,
		       int frame)
{
  ViewFrame          *pFrame;
  PtrTextBuffer       pBuffer;
  int                 xDelta, yDelta;
  int                 width, height;
  int                 x, y;

  pFrame = &ViewFrameTable[frame - 1];
  if (!APPgraphicModify (pAb->AbElement, (int) c, frame, TRUE))
    {
      /* efface la selection precedente */
      switch (c)
	{
	case 'S':	/* polyline */
	case 'U':	/* polyline with forward arrow */
	case 'N':	/* polyline with backward arrow */
	case 'M':	/* polyline with arrows at both ends */
	case 'w':	/* segment */
	case 'x':	/* segment with forward arrow */
	case 'y':	/* segment with backward arrow */
	case 'z':	/* segment with arrows at both ends */
	case 'B':	/* Beziers (ouvertes) */
	case 'A':	/* Beziers (ouvertes) flechees vers avant */
	case 'F':	/* Beziers (ouvertes) flechees vers arriere */
	case 'D':	/* Beziers (ouvertes) flechees dns les deux sens */
	case 'p':	/* polygone */
	case 's':	/* spline fermee */
	  pAb->AbPolyLineShape = c;
	  width = pBox->BxWidth;
	  height = pBox->BxHeight;
	  if (pBox->BxBuffer == NULL)
	    {
	      /* Changement de graphique simple a polyline */
	      pAb->AbLeafType = LtPolyLine;
	      pAb->AbElement->ElLeafType = LtPolyLine;
	      /* creation du buffer initial pour ranger les points de la polyline */
	      GetTextBuffer (&(pAb->AbPolyLineBuffer));
	      pAb->AbPolyLineBuffer->BuLength = 1;
	      pAb->AbPolyLineBuffer->BuPoints[0].XCoord = LogicalValue (width, UnPixel, NULL,
					      ViewFrameTable[frame - 1].FrMagnification);
	      pAb->AbPolyLineBuffer->BuPoints[0].YCoord = LogicalValue (height, UnPixel, NULL,
					      ViewFrameTable[frame - 1].FrMagnification);
	      GetTextBuffer (&(pBox->BxBuffer));
	      /* initialise la dimension de la boite polyline */
	      pBox->BxBuffer->BuLength = 1;
	      pBox->BxBuffer->BuPoints[0].XCoord = pAb->AbPolyLineBuffer->BuPoints[0].XCoord;
	      pBox->BxBuffer->BuPoints[0].YCoord = pAb->AbPolyLineBuffer->BuPoints[0].YCoord;
	      pBox->BxNChars = 1;
	    }
	  
	  if (pBox->BxNChars == 1)
	    {
	      /* il faut saisir les points de la polyline */
	      x = pBox->BxXOrg - pFrame->FrXOrg;
	      y = pBox->BxYOrg - pFrame->FrYOrg;
	      if (c == 'w' || c == 'x' || c == 'y' || c == 'z')
		pAb->AbVolume = PolyLineCreation (frame, &x, &y, pBox, 2);
	      else
		pAb->AbVolume = PolyLineCreation (frame, &x, &y, pBox, 0);
	      pAb->AbElement->ElNPoints = pAb->AbVolume;
	      pBox->BxNChars = pAb->AbVolume;
	      DisplayPointSelection (frame, pBox, 0);
#ifndef _WINDOWS
	      pBox->BxXRatio = 1;
	      pBox->BxYRatio = 1;
#endif /* _WINDOWS */
	    }

	  /* on force le reaffichage de la boite (+ les points de selection) */
	  DefClip (frame, pBox->BxXOrg - EXTRA_GRAPH, pBox->BxYOrg - EXTRA_GRAPH, pBox->BxXOrg + width + EXTRA_GRAPH, pBox->BxYOrg + height + EXTRA_GRAPH);
	  break;

	default:	/* un graphique simple */
	  if (pBox->BxBuffer != NULL)
	    {
	      /* Transformation polyline en graphique simple */
	      pAb->AbLeafType = LtGraphics;
	      /* libere les points de controle */
	      if (pBox->BxPictInfo != NULL)
		{
		  free ((STRING) pBox->BxPictInfo);
		  pBox->BxPictInfo = NULL;
		}
	      /* il faut liberer les buffers */
	      pBox->BxNChars = 1;
	      pBuffer = pBox->BxBuffer;
	      while (pBuffer != NULL)
		{
		  pBox->BxBuffer = pBuffer->BuNext;
		  FreeTextBuffer (pBuffer);
		  pBuffer = pBox->BxBuffer;
		}
	      pBuffer = pAb->AbPolyLineBuffer;
	      while (pBuffer != NULL)
		{
		  pAb->AbPolyLineBuffer = pBuffer->BuNext;
		  FreeTextBuffer (pBuffer);
		  pBuffer = pAb->AbPolyLineBuffer;
		}
	    }

	  pAb->AbShape = c;
	  pAb->AbVolume = 1;
	  if (c == 'g' && pAb->AbEnclosing &&
	      pAb->AbEnclosing->AbWidth.DimIsPosition)
	    {
	      x = pBox->BxXOrg - pFrame->FrXOrg;
	      y = pBox->BxYOrg - pFrame->FrYOrg;
	      LineCreation (frame, pBox, &x, &y, &xDelta, &yDelta);
	      /* Update the enclosing stretchable box */
	      pAb = pAb->AbEnclosing;
	      NewPosition (pAb, x, 0, y, 0, frame, TRUE);
	      NewDimension (pAb, xDelta, yDelta, frame, TRUE);
	    }
	  else
	    {
	      if (c == 'C')
		/* rectangle with rounded corners */
		{
		  pAb->AbRx = 0;
		  pAb->AbRy = 0;
		}
	      /* Dimensions du symbole */
	      GiveGraphicSize (pAb, &xDelta, &yDelta);
	      /* met a jour la boite */
	      if (defaultWidth)
		xDelta -= pBox->BxWidth;
	      else
		xDelta = 0;
	      if (defaultHeight)
		yDelta -= pBox->BxHeight;
	      else
		yDelta = 0;
	      BoxUpdate (pBox, pLine, 0, 0, xDelta, 0, yDelta, frame, FALSE);
	    }
	}
      /* but could notify its parent */
      APPgraphicModify (pAb->AbElement, (int) c, frame, FALSE);
    }
}


/*----------------------------------------------------------------------
   insere dans la boite pBox.                                     
  ----------------------------------------------------------------------*/
static void LoadPictFile (PtrLine pLine, ThotBool defaultHeight,
			  ThotBool defaultWidth, PtrBox pBox, PtrAbstractBox pAb,
			  int frame)
{
  if (pAb->AbLeafType == LtPicture)
    {
      /* give access to the application image menu */
      if (!APPtextModify (pAb->AbElement, frame, TRUE))
	/* register the editing operation in the history */
	RegisterInHistory (pAb->AbElement, frame, 0, 0);	     
    }
}


/*----------------------------------------------------------------------
   ClearClipboard cleans up all allocated buffers.
  ----------------------------------------------------------------------*/
void ClearClipboard (PtrTextBuffer clipboard)
{
  PtrTextBuffer       pBuffer, pNext;

  pBuffer = clipboard;
  if (pBuffer->BuLength != 0)
    {
      pBuffer->BuLength = 0;
      pBuffer->BuContent[0] = EOS;
      pBuffer = pBuffer->BuNext;
      while (pBuffer)
	{
	  pNext = pBuffer->BuNext;
	  pBuffer = DeleteBuffer (pBuffer, ActiveFrame);
	  pBuffer = pNext;
	}
    }
}


/*----------------------------------------------------------------------
   sauve le contenu de la se'lection donne'e en parame`tre         
   dans le registre X ou Thot.                                     
   charsDelta donne le nombre de caracte`res pre'ce'dents,         
   ind l'indice du premier caracte`re dans le buffer et pBuffer le 
   premier buffer de texte.                                        
   Retourne :                                                      
   - charsDelta le nombre de caracte`res copie's                   
   - spacesDelta le nombre de blancs copie's                       
   - xDelta la largeur de la chaine copie'e                        
  ----------------------------------------------------------------------*/
static void SaveInClipboard (int *charsDelta, int *spacesDelta, int *xDelta,
			     int ind, PtrTextBuffer pBuffer, PtrAbstractBox pAb,
			     int frame, PtrTextBuffer clipboard)
{
  PtrTextBuffer       pTargetBuffer;
  ViewFrame          *pFrame;
  PictInfo           *image;
  int                 i;

  /* detruit la sauvegarde precedente */
  ClearClipboard (clipboard);
  /* sauve la selection courante */
  if (pAb != NULL)
    {
      if (pAb->AbVolume != 0)
	{
	  ClipboardType = pAb->AbLeafType;
	  ClipboardLanguage = TtaGetDefaultLanguage ();
	  if (pAb->AbLeafType == LtSymbol || pAb->AbLeafType == LtGraphics)
	    {
	      clipboard->BuLength = 1;
	      clipboard->BuContent[0] = pAb->AbShape;
	      /* caractere trace ou symbole */
	      clipboard->BuContent[1] = EOS;
	    }
	  else if (pAb->AbLeafType == LtPicture)
	    {
	      image = (PictInfo *) pAb->AbPictInfo;
	      i = strlen (image->PicFileName);
	      /* nom du fichier image */
	      CopyMBs2Buffer (image->PicFileName, clipboard, 0, i);
	      CopyPictInfo ((int *) &PictClipboard, (int *) image);
	    }
	  else
	    {
	      pFrame = &ViewFrameTable[frame - 1];
	      if (pAb->AbLeafType == LtText && *charsDelta < pAb->AbVolume)
		{
		  /* enable the Paste command */
		  if (FirstSavedElement == NULL &&
		      ClipboardThot.BuLength == 0)
		    /* switch the Paste entry in all documents */
		    SwitchPaste (NULL, TRUE);
		  
		  ClipboardLanguage = pAb->AbLang;
		  /* sauve le texte selectionne dans la feuille */
		  i = 0;	/* Indice de debut */
		  pTargetBuffer = &ClipboardThot;
		  CopyBuffers (pAb->AbBox->BxFont, frame, ind,
			       pFrame->FrSelectionEnd.VsIndBuf, i,
			       pBuffer, pFrame->FrSelectionEnd.VsBuffer,
			       &pTargetBuffer, xDelta, spacesDelta, charsDelta);
		}
	      else
		{
		  *xDelta = 0;
		  *charsDelta = 0;
		  *spacesDelta = 0;
		}
	    }
	}
    }
}


/*----------------------------------------------------------------------
  RemoveSelection deletes the contents of the current selection.
   - charsDelta le nombre de caracteres copies                     
   - spacesDelta le nombre de blancs copies                        
   - xDelta la largeur de la chaine copiee                         
  ----------------------------------------------------------------------*/
static void RemoveSelection (int charsDelta, int spacesDelta, int xDelta,
			     ThotBool defaultHeight, ThotBool defaultWidth,
			     PtrLine pLine, PtrBox pBox, PtrAbstractBox pAb,
			     int frame, ThotBool notify)
{
  PtrTextBuffer       pTargetBuffer;
  PtrTextBuffer       pSourceBuffer;
  PictInfo           *image;
  ViewFrame          *pFrame;
  ViewSelection      *pViewSel, *pViewSelEnd;
  SpecFont             font;
  int                 sourceInd, targetInd;
  int                 length;
  int                 i;
  int                 adjust;
  int                 yDelta;
  int                 width, height;

  font = pBox->BxFont;
  width = BoxCharacterWidth (109, font);
  height = BoxFontHeight (font);
  if (pAb->AbVolume == 0)
    DefClip (frame, 0, 0, 0, 0);
  else
    switch (pAb->AbLeafType)
      {
      case LtText:
	pFrame = &ViewFrameTable[frame - 1];
	pViewSel = &pFrame->FrSelectionBegin;
	pViewSelEnd = &pFrame->FrSelectionEnd;
	/* Note que le texte de l'e'le'ment va changer */
	StartTextInsertion (NULL, frame, NULL, NULL, 0, 0);
	/* fusionne le premier et dernier buffer de la selection */
	/* target of the moving */
	pTargetBuffer = pViewSel->VsBuffer;
	targetInd = pViewSel->VsIndBuf;
	/* source of the moving */
	pSourceBuffer = pViewSelEnd->VsBuffer;
	sourceInd = pViewSelEnd->VsIndBuf;
	if (SelPosition)
	  {
	    if (sourceInd >= pSourceBuffer->BuLength - 1)
	      {
		pSourceBuffer = pSourceBuffer->BuNext;
		sourceInd = 0;
	      }
	    else
	      sourceInd++;
	  }
	if (pSourceBuffer)
	  {
	    /* nombre de caracteres a copier */
	    i = pSourceBuffer->BuLength - sourceInd;
	    if (i <= 0)
	      /* no moving */
	      i = targetInd;
	    else
	      {
		length = FULL_BUFFER - targetInd;
		/* deplace en deux fois? */
		if (i > length)
		  {
		    ustrncpy (&pTargetBuffer->BuContent[targetInd],
			      &pSourceBuffer->BuContent[sourceInd], length);
		    pTargetBuffer->BuLength = FULL_BUFFER;
		    pTargetBuffer->BuContent[FULL_BUFFER] = EOS;
		    targetInd = 0;
		    sourceInd += length;
		    i -= length;
		    pTargetBuffer = pTargetBuffer->BuNext;
		  }
		ustrncpy (&pTargetBuffer->BuContent[targetInd],
			  &pSourceBuffer->BuContent[sourceInd], i);
		i += targetInd;
	      }
	  }
	else
	  i = pViewSel->VsIndBuf;
	pTargetBuffer->BuLength = i;
	pTargetBuffer->BuContent[i] = EOS;
	
	/* Faut-il liberer le buffer 'pTargetBuffer' ? */
	if (pTargetBuffer->BuLength == 0 && charsDelta != pAb->AbVolume)
	  {
	    /* Est-ce que l'on libere le premier buffer du pave ? */
	    if (pTargetBuffer == pAb->AbText)
	      {
		pAb->AbText = pSourceBuffer->BuNext;
		pAb->AbBox->BxBuffer = pAb->AbText;
	      }

	    /* Est-ce que l'on libere le premier buffer d'une boite ? */
	    if (pTargetBuffer == pBox->BxBuffer)
	      pBox->BxBuffer = pSourceBuffer->BuNext;
	    
	    if (pTargetBuffer == pViewSel->VsBuffer)
	      {
	    /* the selection points to a freed buffer */
		if (pSourceBuffer && pSourceBuffer->BuNext)
		  /* move after the deleted part */
		  pViewSel->VsBuffer = pSourceBuffer->BuNext;
		else
		  {
		    /* move before */
		    pViewSel->VsBuffer = pTargetBuffer->BuPrevious;
		    if (pViewSel->VsBuffer)
		      pViewSel->VsIndBuf = pViewSel->VsBuffer->BuLength;
		  }
	      }
	    
	    if (pTargetBuffer == pSourceBuffer)
	      pSourceBuffer = NULL;
	    pTargetBuffer = DeleteBuffer (pTargetBuffer, frame);
	  }
	
	/* remove empty buffers */
	while (pSourceBuffer && pSourceBuffer != pTargetBuffer)
	  pSourceBuffer = DeleteBuffer (pSourceBuffer, frame);
	
	/* prepare the box update */
	if (pBox->BxSpaceWidth != 0)
	  {
	    /* adjusted box */
	    i = BoxCharacterWidth (SPACE, font);
	    adjust = xDelta + (pBox->BxSpaceWidth - i) * spacesDelta;
	  }
	else
	  adjust = 0;
	if (charsDelta == pAb->AbVolume)
	  {
	    /* the box becomes empty */
	    xDelta = pAb->AbBox->BxWidth - width;
	    if (pBox == pAb->AbBox && adjust != 0)
	      adjust = xDelta;
	    else
	      adjust = 0;
	    /* the whole paragraph should be recomputed */
	    pViewSel->VsBox = pAb->AbBox;
	  }
	else if (pViewSel->VsIndBuf >= pViewSel->VsBuffer->BuLength)
	  {
	    /* En fin de buffer -> on va chercher le caractere suivant */
	    pSourceBuffer = pViewSel->VsBuffer->BuNext;
	    /* Il peut etre vide s'il y a des blancs en fin de ligne */
	    if (pSourceBuffer)
	      {
		/* deplace la selection sur le debut du buffer suivant */
		pViewSel->VsBuffer = pSourceBuffer;
		pViewSel->VsIndBuf = 0;
	      }
	  }
	
	/* the selection points to the next character */
	pViewSelEnd->VsBox = pViewSel->VsBox;
	pViewSelEnd->VsIndBox = pViewSel->VsIndBox;
	pFrame->FrSelectOnePosition = TRUE;
	SelPosition = TRUE;
	/* box update */
	pAb->AbVolume -= charsDelta;
	BoxUpdate (pAb->AbBox, pLine, -charsDelta, -spacesDelta, -xDelta,
		   -adjust, 0, frame, FALSE);
	if (notify)
	  CloseTextInsertion ();
	break;
	
      case LtPicture:
	pAb->AbVolume = 0;
	/* met a jour la boite */
	image = (PictInfo *) pBox->BxPictInfo;
	if (image->PicPixmap != 0)
	  {
	    if (defaultWidth)
	      xDelta = image->PicWArea - width;
	    else
	      xDelta = 0;
	    if (defaultHeight)
	      yDelta = image->PicHArea - height;
	    else
	      yDelta = 0;
	    BoxUpdate (pBox, pLine, 0, 0, -xDelta, 0, -yDelta, frame, FALSE);
	  }
	break;
	
      case LtSymbol:
      case LtGraphics:
	/* send an event to the application and register the change
	   in the Undo queue */
	APPgraphicModify (pBox->BxAbstractBox->AbElement,
			  (int) pAb->AbShape, frame, TRUE);
	if (defaultWidth)
	  xDelta = pBox->BxWidth - width;
	else
	  xDelta = 0;
	if (defaultHeight)
	  yDelta = pBox->BxHeight - height;
	else
	  yDelta = 0;
	BoxUpdate (pBox, pLine, -1, 0, -xDelta, 0, -yDelta, frame, FALSE);
	if (pAb->AbLeafType == LtGraphics && pAb->AbShape == 'C')
	  {
	    pAb->AbRx = 0;
	    pAb->AbRy = 0;
	  }
	pAb->AbVolume = 0;
	pAb->AbShape = EOS;
	APPgraphicModify (pBox->BxAbstractBox->AbElement,
			  (int) pAb->AbShape, frame, FALSE);
	break;
	
      default:
	break;
      }
}


/*----------------------------------------------------------------------
   detruit sans sauver le contenu de la selection courante.        
  ----------------------------------------------------------------------*/
static void DeleteSelection (ThotBool defaultHeight, ThotBool defaultWidth,
			     PtrLine pLine, PtrBox pBox, PtrAbstractBox pAb,
			     int frame, ThotBool notify)
{
  PtrTextBuffer       pTargetBuffer;
  ViewFrame          *pFrame;
  int                 i, end;
  int                 xDelta, charsDelta;
  int                 spacesDelta;

  charsDelta = 0;
  if (pAb)
    {
      if (pAb->AbLeafType == LtText)
	{
	  pFrame = &ViewFrameTable[frame - 1];
	  /* get values xDelta, spacesDelta, charsDelta */
	  if (pFrame->FrSelectionBegin.VsIndBox == pAb->AbVolume)
	    {
	      xDelta = 0;
	      spacesDelta = 0;
	    }
	  else
	    {
	      /* index of the beginning */
	      pTargetBuffer = NULL;
	      end = pFrame->FrSelectionEnd.VsIndBuf;
	      if (SelPosition)
		/* suppress the next char */
		end++;
	      i = 0;
	      CopyBuffers (pBox->BxFont, frame,
			   pFrame->FrSelectionBegin.VsIndBuf,
			   end, i,
			   pFrame->FrSelectionBegin.VsBuffer,
			   pFrame->FrSelectionEnd.VsBuffer, &pTargetBuffer,
			   &xDelta, &spacesDelta, &charsDelta);
	    }
	}
    }
  /* remove the contents of the current selection */
  if (charsDelta > 0)
    RemoveSelection (charsDelta, spacesDelta, xDelta, defaultHeight,
		     defaultWidth, pLine, pBox, pAb, frame, notify);
}


/*----------------------------------------------------------------------
   insere le dernier clipboard X ou Thot sauve.                    
   - charsDelta donne le nombre de caracteres copies               
   - spacesDelta donne le nombre de blancs copies                  
   - xDelta donne la largeur de la chaine copiee                   
  ----------------------------------------------------------------------*/
static void PasteClipboard (ThotBool defaultHeight, ThotBool defaultWidth,
			    PtrLine pLine, PtrBox pBox, PtrAbstractBox pAb,
			    int frame, PtrTextBuffer clipboard)
{
  PtrTextBuffer       pBuffer;
  PtrTextBuffer       pNewBuffer;
  ViewFrame          *pFrame;
  ViewSelection      *pViewSel;
  ViewSelection      *pViewSelEnd;
  PtrTextBuffer       pCurrentBuffer;
  SpecFont             font;
  int                 ind;
  int                 i, l;
  int                 xDelta, yDelta;
  int                 spacesDelta, charsDelta;
  int                 adjust;
  int                 height;
  int                 width;
  PictInfo           *image;

   font = pBox->BxFont;
   width = 2; /* see GiveTextSize function */
   height = BoxFontHeight (font);

   switch (pAb->AbLeafType)
	 {
	    case LtText:
	       pFrame = &ViewFrameTable[frame - 1];
	       pViewSel = &pFrame->FrSelectionBegin;
	       pViewSelEnd = &pFrame->FrSelectionEnd;

	       /* Coupure des buffers pour l'insertion */
	       if (!TextInserting)
		  StartTextInsertion (NULL, frame, NULL, NULL, 0, 0);

	       /* Insertion en fin de buffer */
	       if (pViewSel->VsIndBuf >= pViewSel->VsBuffer->BuLength)
		 {
		    /* insertion en fin de buffer courant */
		    pBuffer = pViewSel->VsBuffer;
		    ind = pViewSel->VsIndBuf;
		    /* Pas besoin d'essayer de recoller les buffer */
		    TextInserting = FALSE;
		 }
	       else
		 {
		    /* buffer debut d'insertion */
		   pBuffer = pViewSel->VsBuffer;
		   if ( pBuffer->BuPrevious)
		     pBuffer = pBuffer->BuPrevious;
		   /* index debut d'insertion */
		   ind = pBuffer->BuLength;
		 }

	       /* Insertion des caracteres */
	       pNewBuffer = pBuffer;
	       CopyBuffers (font, frame, 0, 0, ind, clipboard, NULL,
			    &pNewBuffer, &xDelta, &spacesDelta,
			    &charsDelta);
	       /* termine l'insertion */
	       pAb->AbVolume += charsDelta;
	       /* Move the selection at the end of inserted text */
	       pViewSel->VsIndBox = pViewSel->VsIndBox + charsDelta;
	       pViewSelEnd->VsIndBox = pViewSel->VsIndBox;
	       pViewSel->VsXPos = pViewSel->VsXPos + xDelta;
	       pViewSelEnd->VsXPos = pViewSel->VsXPos;
               /* last inserted buffer */
               pViewSel->VsBuffer = pNewBuffer;
               pViewSelEnd->VsBuffer = pNewBuffer;
               pViewSel->VsIndBuf = pNewBuffer->BuLength;
               pViewSelEnd->VsIndBuf = pNewBuffer->BuLength;
	       pViewSelEnd->VsBox = pViewSel->VsBox;
	       /* +nombre de blancs */
	       pViewSel->VsNSpaces = pViewSel->VsNSpaces + spacesDelta;
	       pViewSelEnd->VsNSpaces = pViewSel->VsNSpaces;
	       pViewSel->VsLine = pLine;
	       pViewSelEnd->VsLine = pLine;
	       pFrame->FrSelectOnePosition = TRUE;
	       /* the new selection is not shown */
	       pFrame->FrSelectShown = FALSE;
    	       if (pAb->AbVolume == charsDelta)
		 /* the box was empty */
		 xDelta -= /*width*/pBox->BxWidth;
	       if (pBox->BxSpaceWidth != 0)
		 {
		    /* Si la boite est adjustifiee */
		    i = BoxCharacterWidth (SPACE, font);
		    adjust = xDelta + (pBox->BxSpaceWidth - i) * spacesDelta;
		 }
	       else
		  adjust = 0;

	       /* Si le clipboard contient un caractere de coupure forcee */
	       /* il faut demander la reevaluation de la mise en lignes  */
	       if (IsLineBreakInside (pBox->BxBuffer, pBox->BxFirstChar, pBox->BxNChars + charsDelta))
		  BoxUpdate (pBox, pLine, charsDelta, spacesDelta, xDelta, adjust,
			     0, frame, TRUE);
	       else
		  BoxUpdate (pBox, pLine, charsDelta, spacesDelta, xDelta, adjust,
			     0, frame, FALSE);
	       CloseTextInsertion ();
	       break;
	    case LtPicture:
	       pCurrentBuffer = pAb->AbElement->ElText;
	       /* met a jour la boite */
	       image = (PictInfo *) pBox->BxPictInfo;
	       CopyPictInfo ((int *) image, (int *) &PictClipboard);
	       i = clipboard->BuLength;
	       ustrncpy (pCurrentBuffer->BuContent, clipboard->BuContent, i);
	       /* Termine la chaine de caracteres */
	       pCurrentBuffer->BuContent[i] = EOS;
	       pCurrentBuffer->BuLength = i;
	       i = 400;
	       image->PicFileName = TtaGetMemory (i);
	       /* i should be too short to store non ascii characters */
	       l = CopyBuffer2MBs (clipboard, 0, image->PicFileName, i - 1);
	       SetCursorWatch (frame);
	       LoadPicture (frame, pBox, image);
	       ResetCursorWatch (frame);
	       if (image->PicPixmap != 0)
		 {
		    pAb->AbVolume = i;
		    xDelta = image->PicWArea - width;
		    yDelta = image->PicHArea - height;
		    BoxUpdate (pBox, pLine, 0, 0, xDelta, 0, yDelta, frame, FALSE);
		 }
	       break;
	    case LtSymbol:
	       LoadSymbol ((char)clipboard->BuContent[0], pLine, defaultHeight,
			   defaultWidth, pBox, pAb, frame);
	       break;
	    case LtPolyLine:
	    case LtGraphics:
	       LoadShape ((char)clipboard->BuContent[0], pLine, defaultHeight,
			  defaultWidth, pBox, pAb, frame);
	       break;
	    default:
	       break;
	 }
}

/*----------------------------------------------------------------------
   Traite les commandes TextInserting Cut Paste Copy Oops          
   ainsi que l'insertion des Graphiques Images et Symboles         
  ----------------------------------------------------------------------*/
static void         ContentEditing (int editType)
{
  PtrBox              pBox;
  PtrBox              pSelBox;
  PtrAbstractBox      pAb, pCell;
  PtrAbstractBox      pLastAb, pBlock;
  AbDimension        *pPavD1;
  ViewSelection      *pViewSel;
  ViewSelection      *pViewSelEnd;
  ViewFrame          *pFrame;
  PtrTextBuffer       pBuffer;
  Propagation         savePropagate;
  PtrLine             pLine;
  int                 x, y;
  int                 i, j;
  int                 spacesDelta, charsDelta;
  int                 frame;
  ThotBool            still, ok, textPasted;
  ThotBool            defaultWidth, defaultHeight;
  ThotBool            show, graphEdit;

  /* termine l'insertion de caracteres en cours */
  CloseTextInsertion ();
  pCell = NULL;
  textPasted = FALSE;
  graphEdit = FALSE;

  /* Traitement de la Commande PASTE de l'application */
  if (editType == TEXT_PASTE && ClipboardThot.BuLength == 0 && !FromKeyboard)
    {
      if (ThotLocalActions[T_cmdpaste] != NULL)
	(*ThotLocalActions[T_cmdpaste]) ();
    }
  /* Traitement des Commandes INSERT, CUT, PASTE, COPY, OOPS */
  else
    {
      /* recupere la fenetre active pour la selection */
      frame = ActiveFrame;
      /*-- recherche le pave concerne --*/
      if (frame <= 0)
	return;
      else
	{
	  pBox = ViewFrameTable[frame - 1].FrSelectionBegin.VsBox;
	  /* verifie qu'une selection courante est visualisee */
	  if (pBox == NULL)
	    {
	      TtaSetFocus ();
	      /* a pu ici changer de frame active pour la selection */
	      frame = ActiveFrame;
	      if (frame == 0)
		return;	/* nothing to do */
	      else
		{
		  pBox = ViewFrameTable[frame - 1].FrSelectionBegin.VsBox;
		  if (pBox == NULL)
		    return;	/* nothing to do */
		  pAb = pBox->BxAbstractBox;
		}
	    }
	  else
	    pAb = pBox->BxAbstractBox;
	  if (pAb->AbReadOnly &&
	      (editType == TEXT_INSERT || editType == TEXT_PASTE))
	    /* the box to be edited is read-only */
	    /* can't insert or paste within a read-only char string */
	    return;
	  if (editType == TEXT_DEL || editType == TEXT_CUT)
	    {
	      if (AbsBoxSelectedAttr)
		{
		  if (AbsBoxSelectedAttr->AbReadOnly)
		    /* selection within a read-only attribute value */
		    return;
		}
	      else
		{
		  if (ElementIsReadOnly (FirstSelectedElement->ElParent))
		    /* the parent element is read-only.
		       Don't delete anything*/
		    return;
		  else if (ElementIsReadOnly (FirstSelectedElement))
		    /* the parent is not read-only */
		    /* the selected element itself is read-only */
		    if (FirstSelectedElement == LastSelectedElement &&
			FirstSelectedElement->ElTerminal &&
			FirstSelectedElement->ElLeafType == LtText)
		      /* selection is within a single text element */
		      if (FirstSelectedChar > 1 ||
			  (LastSelectedChar > 0 &&
			   LastSelectedChar < LastSelectedElement->ElVolume))
			/* the text element is not completely selected */
			return;
		}
	    }
	}
      /*-- La commande coller concerne le mediateur --*/
      if (editType == TEXT_PASTE && !FromKeyboard)
	/* Il faut peut-etre deplacer la selection courante */
	SetInsert (&pAb, &frame, ClipboardType, FALSE);

      pFrame = &ViewFrameTable[frame - 1];
      pViewSel = &pFrame->FrSelectionBegin;
      show = (documentDisplayMode[FrameTable[frame].FrDoc - 1] == DisplayImmediately);
      if (pBox && editType == TEXT_SUP)
	{
	  /* don't remove the selection if it is at the end of the text */
	  if (pAb->AbLeafType == LtText &&
	      pBox == ViewFrameTable[frame - 1].FrSelectionEnd.VsBox &&
	      pViewSel->VsIndBox >= pBox->BxNChars &&
	      pBox->BxAbstractBox->AbVolume != 0)
	    return;
	}

      if (editType == TEXT_INSERT && !FromKeyboard)
	{
	  if (pBox && pViewSel->VsIndBox != 0)
	    {
	      if (pAb->AbLeafType == LtPolyLine)
		{
		  /* ignore single segments (with or without arrow head(s)) */
		  if (pAb->AbPolyLineShape != 'w' &&
		      pAb->AbPolyLineShape != 'x' &&
		      pAb->AbPolyLineShape != 'y' &&
		      pAb->AbPolyLineShape != 'z')
		    /* Add a point to a polyline */
		    {
		      still = (pAb->AbPolyLineShape == 'p' || /* plygon */
			       pAb->AbPolyLineShape == 's');  /* closed curve*/
		      if (!APPgraphicModify (pAb->AbElement,
					     pAb->AbPolyLineShape, frame,TRUE))
			{
			  x = pBox->BxXOrg - pFrame->FrXOrg;
			  y = pBox->BxYOrg - pFrame->FrYOrg;
			  i = pViewSel->VsIndBox;
			  pBox->BxNChars = PolyLineExtension (frame, &x, &y,
					       pBox, pBox->BxNChars, i, still);
			  graphEdit = TRUE;
			  pAb->AbVolume = pBox->BxNChars;
			  if (pBox->BxPictInfo != NULL)
			    {
			      /* reevalue les points de controle */
			      free ((STRING) pBox->BxPictInfo);
			      pBox->BxPictInfo = NULL;
			    }
			}
		    }
		  else
		    /* la commande est realisee par l'application */
		    pAb = NULL;
		}
	      else
		{
		  if (ThotLocalActions[T_insertpaste] != NULL)
		    (*ThotLocalActions[T_insertpaste]) (TRUE, FALSE,
							'L', &ok);
		  else
		    ok = FALSE;
		  if (ok)
		    /* la commande est realisee par l'application */
		    pAb = NULL;
		}
	    }
	  else if (pAb->AbLeafType != LtPicture)
	    {
	      if (ThotLocalActions[T_insertpaste] != NULL)
		(*ThotLocalActions[T_insertpaste]) (TRUE, FALSE,
						    'L', &ok);
	      else
		ok = FALSE;
	      if (ok)
		/* la commande est realisee par l'application */
		pAb = NULL;
	    }
	}

      if (pAb)
	{
	  pLastAb = NULL;
	  /*-- Les commandes sont traitees dans l'application */
	  /* si la selection ne porte que sur un pave */
	  pBox = ViewFrameTable[frame - 1].FrSelectionEnd.VsBox;
	  if (pBox == NULL)
	    pAb = NULL;
	  else
	    {
	      pLastAb = pBox->BxAbstractBox;
	      /* saute les paves de presentation selectionnes */
	      while (pLastAb != pAb && pLastAb->AbPresentationBox)
		pLastAb = pLastAb->AbPrevious;
	    }
	  
	  /* Recherche le point d'insertion (&i non utilise) */
	  GiveInsertPoint (pAb, frame, &pBox, &pBuffer, &i, &x, &charsDelta);
	  if (pBox == NULL)
	    {
	      /* take in account another box */
	      if (ThotLocalActions[T_deletenextchar] != NULL &&
		  (editType == TEXT_CUT || editType == TEXT_DEL ||
		   editType == TEXT_SUP))
		{
		  (*ThotLocalActions[T_deletenextchar]) (frame, pAb->AbElement,
							 FALSE);
		  return;
		}
	      else
		pAb = NULL;
	    }
	  else if (pAb)
	    pAb = pBox->BxAbstractBox;
	  
	  if (pAb)
	    {
	      if (FirstSelectedElement != LastSelectedElement ||
		  pAb != pLastAb)
		/* more than one element or one abstract box */
		pAb = NULL;
	      else if (pAb->AbElement != FirstSelectedElement &&
		       FirstSelectedElement &&
			(editType == TEXT_CUT || editType == TEXT_DEL ||
			 editType == TEXT_SUP || editType == TEXT_COPY))
		pAb = NULL;
	      else if ((pAb->AbLeafType == LtText && editType == TEXT_INSERT)||
		       pAb->AbLeafType == LtCompound || /*le pave est compose*/
		       pAb->AbLeafType == LtPageColBreak) /* marque de page */
		pAb = NULL;
	      else if ((editType == TEXT_CUT || editType == TEXT_DEL ||
			editType == TEXT_SUP || editType == TEXT_COPY) &&
		       (pAb->AbVolume == 0 || pAb->AbLeafType == LtGraphics ||
			pAb->AbLeafType == LtPath ||
			pAb->AbLeafType == LtPicture))
		/* coupe ou copie un pave vide ou graphique ou une image */
		pAb = NULL;
	      else if ((editType == TEXT_CUT || editType == TEXT_COPY) &&
		       pAb->AbLeafType == LtPolyLine)
		/* coupe ou copie un pave Polyline */
		pAb = NULL;
	      else if (!pAb->AbCanBeModified && editType != TEXT_COPY)
		{
		  if (pViewSel->VsIndBox != 0 ||
		      pFrame->FrSelectionEnd.VsIndBox != 0) 
		    /* il n'est pas modifiable */
		    pAb = NULL;
		  else if (pAb->AbEnclosing &&
			   !pAb->AbEnclosing->AbCanBeModified)
		    /* il n'est pas modifiable */
		    pAb = NULL;
		}
	    }
	}

      if (pAb == NULL)
	{
	  /* Le traitement concerne l'application */
	  if ((editType == TEXT_DEL || editType == TEXT_SUP) && !FromKeyboard)
	    CutCommand (FALSE);	/* Couper sans sauver */
	  else if (editType == TEXT_CUT || editType == TEXT_COPY)
	    {
	      SaveInClipboard (&charsDelta, &spacesDelta, &x, 0, NULL, pAb,
			       frame, &ClipboardThot);
	      /* vide le clipboard du Mediateur */
	      if (editType == TEXT_CUT && !FromKeyboard)
		CutCommand (TRUE);
	      else if (editType == TEXT_COPY && !FromKeyboard)
		CopyCommand ();
	    }
	}
      else
	{
	  /* check enclosing cell */
	  pCell = GetParentCell (pBox);
	  
	  if ((editType == TEXT_DEL ||editType == TEXT_SUP) &&
	      pAb->AbLeafType == LtPolyLine)
	    {
	      if (pViewSel->VsIndBox != 0)
		{
		  if (pBox->BxNChars > 3 &&
		      pAb->AbPolyLineShape != 'w' &&
		      pAb->AbPolyLineShape != 'x' &&
		      pAb->AbPolyLineShape != 'y' &&
		      pAb->AbPolyLineShape != 'z')
		    {
		      charsDelta = pViewSel->VsIndBox;
		      if (!APPgraphicModify (pAb->AbElement,
					     pAb->AbPolyLineShape, frame,TRUE))
			{
			  /* Destruction du point courant de la polyline */
			  DeletePointInPolyline (&(pAb->AbPolyLineBuffer),
						 charsDelta);
			  DeletePointInPolyline (&(pBox->BxBuffer),
						 charsDelta);
			  graphEdit = TRUE;
			  (pBox->BxNChars)--;
			  if (pBox->BxPictInfo != NULL)
			    {
				/* reevalue les points de controle */
			      free ((STRING) pBox->BxPictInfo);
			      pBox->BxPictInfo = NULL;
			    }
			  (pAb->AbVolume)--;
			  if (charsDelta == pBox->BxNChars)
			    {
			      /* on vient de detruire le dernier point
				 de la polyline */
			      if (charsDelta == 1)
				charsDelta = 0;
			      else
				charsDelta = 1;
			    }
			  pViewSel->VsIndBox = charsDelta;
			  /* on force le reaffichage de la boite
			     (+ les points de selection) */
			  DefClip (frame, pBox->BxXOrg - EXTRA_GRAPH,
				   pBox->BxYOrg - EXTRA_GRAPH,
				   pBox->BxXOrg + pBox->BxWidth + EXTRA_GRAPH,
				  pBox->BxYOrg + pBox->BxHeight + EXTRA_GRAPH);
			}
		    }
		}
	      else
		{
		  /* Destruction de toute la polyline */
		  CutCommand (FALSE);
		  pAb = NULL;	/* le traitement est termine */
		}
	    }
	  else
	    {
	      /* regroupe ici les traitements realises par le mediateur */
	      /* bloque affichage de la fenetre */
	      if (editType != TEXT_COPY)
		pFrame->FrReady = FALSE;
	      
	      /* Si la selection debutait sur une boite de presentation */
	      /* il faut deplacer la selection sur le premier caractere */
	      /* de la boite de texte (ou en fin de boite vide)         */
	      if (editType != TEXT_COPY && pBox != pViewSel->VsBox)
		{
		  ClearViewSelection (frame);	/* efface la selection */
		  pViewSel->VsBox = pBox;
		  pViewSel->VsBuffer = pBuffer;
		  pViewSel->VsIndBuf = i;
		  pViewSel->VsIndBox = charsDelta;
		  pViewSel->VsXPos = 0;
		  pViewSel->VsNSpaces = 0;
		}
	      
	      pLine = pFrame->FrSelectionBegin.VsLine;
	      
	      /* Initialisation du rectangle d'affichage */
	      DefClip (frame, 0, 0, 0, 0);
	      if (pBox->BxType == BoSplit)
		{
		  pSelBox = pBox->BxNexChild;
		  DefClip (frame, pSelBox->BxXOrg, pSelBox->BxYOrg,
			   pSelBox->BxXOrg + pSelBox->BxWidth,
			   pSelBox->BxYOrg + pSelBox->BxHeight);
		}
	      else
		{
		  DefClip (frame, pBox->BxXOrg, pBox->BxYOrg,
			   pBox->BxXOrg + pBox->BxWidth,
			   pBox->BxYOrg + pBox->BxHeight);
		}
	      
	      /* Est-ce que les dimensions de la boite dependent du contenu */
	      pPavD1 = &pAb->AbWidth;
	      if (pPavD1->DimIsPosition)
		/* Boite elastique */
		defaultWidth = FALSE;
	      else if (pPavD1->DimAbRef == NULL && pPavD1->DimValue < 0)
		defaultWidth = TRUE;
	      else
		defaultWidth = FALSE;
	      
	      pPavD1 = &pAb->AbHeight;
	      if (pPavD1->DimIsPosition)
		/* Boite elastique */
		defaultHeight = FALSE;
	      else if (pPavD1->DimAbRef == NULL && pPavD1->DimValue < 0)
		defaultHeight = TRUE;
	      else
		/* Dimensions d'une boite terminale vide */
		defaultHeight = FALSE;
		
	      /* Traitement */
	      if (editType == TEXT_INSERT && pAb->AbLeafType == LtPicture &&
		  !FromKeyboard)
		LoadPictFile (pLine, defaultHeight, defaultWidth, pBox, pAb,
			      frame);
	      else if (editType == TEXT_CUT && !FromKeyboard)
		{
		  SaveInClipboard (&charsDelta, &spacesDelta, &x, i, pBuffer,
				   pAb, frame, &ClipboardThot);
		  if (ClipboardThot.BuLength == 0)
		    {
		      if (ThotLocalActions[T_deletenextchar] != NULL)
			(*ThotLocalActions[T_deletenextchar]) (frame,
							pAb->AbElement, FALSE);
		      else
			/* Pas de reaffichage */
			DefClip (frame, 0, 0, 0, 0);
		      /* Inutile de mettre a jour la selection */
		      pAb = NULL;
		    }
		  else
		    RemoveSelection (charsDelta, spacesDelta, x, defaultHeight,
				     defaultWidth, pLine, pBox, pAb, frame, TRUE);
		}
	      else if ((editType == TEXT_DEL || editType == TEXT_SUP) &&
		       !FromKeyboard)
		if (pAb->AbVolume == 0 ||
		    pViewSel->VsIndBox + pViewSel->VsBox->BxFirstChar > pAb->AbVolume)
		  {
		    /* current selection is at the element end */
		    if (ThotLocalActions[T_deletenextchar] != NULL)
		      (*ThotLocalActions[T_deletenextchar]) (frame,
						       pAb->AbElement, FALSE);
		    else
		      /* Pas de reaffichage */
		      DefClip (frame, 0, 0, 0, 0);
		    /* Il n'est pas necessaire de mettre a jour la selection */
		    pFrame->FrReady = TRUE;
		    pAb = NULL;
		  }
		else
		  DeleteSelection (defaultHeight, defaultWidth, pLine, pBox,
				   pAb, frame, editType != TEXT_SUP);
	      else if (editType == TEXT_PASTE && !FromKeyboard)
		{
		  /* Verifie que l'script du clipboard correspond a celui
		     du pave */
		  if (ClipboardLanguage == 0)
		    ClipboardLanguage = TtaGetDefaultLanguage ();
		  if (pAb->AbLeafType != LtText &&
		      pAb->AbLang != ClipboardLanguage)
		    {
		      /* charsDelta contient le nombre de carateres qui
			 precedent dans la boite */
		      NewTextLanguage (pAb, charsDelta + pBox->BxFirstChar + 1,
				       ClipboardLanguage);
		      pBox = pViewSel->VsBox;
		      if (pBox != NULL)
			{
			  DefClip (frame, pBox->BxXOrg, pBox->BxYOrg,
				   pBox->BxXOrg + pBox->BxWidth,
				   pBox->BxYOrg + pBox->BxHeight);
			  pAb = pBox->BxAbstractBox;
			}
		      else
			pAb = NULL;
		    }

		  if (pAb != NULL)
		    {
		      PasteClipboard (defaultHeight, defaultWidth, pLine, 
				      pBox, pAb, frame, &ClipboardThot);
		      textPasted = TRUE;
		    }
		}
	      else if (editType == TEXT_COPY && !FromKeyboard)
		{
		  SaveInClipboard (&charsDelta, &spacesDelta, &x, i, pBuffer,
				   pAb, frame, &ClipboardThot);
		  /* Reset the clipping */
		  DefClip (frame, 0, 0, 0, 0);
		  /* the selection is not changed */
		  pAb = NULL;
		}
	      else if (editType == TEXT_X_PASTE && !FromKeyboard)
		{
		  PasteClipboard (defaultHeight, defaultWidth, pLine, pBox,
				  pAb, frame, &XClipboard);
		  textPasted = TRUE;
		}
	      else if (pAb->AbLeafType == LtPicture && FromKeyboard)
		LoadPictFile (pLine, defaultHeight, defaultWidth, pBox, pAb,
			      frame);
	      else if (pAb->AbLeafType == LtSymbol && FromKeyboard)
		LoadSymbol ((char) editType, pLine, defaultHeight,
			    defaultWidth, pBox, pAb, frame);
	      else if ((pAb->AbLeafType == LtGraphics ||
			pAb->AbLeafType == LtPolyLine) &&
		       FromKeyboard)
		LoadShape ((char) editType, pLine, defaultHeight, defaultWidth,
			   pBox, pAb, frame);
	    }
	}
      
      if (pCell != NULL && ThotLocalActions[T_checkcolumn])
	{
	  /* we have to propage the position to children */
	  savePropagate = Propagate;
	  Propagate = ToChildren;
	  pBlock = SearchEnclosingType (pBox->BxAbstractBox, BoBlock);
	  if (pBlock != NULL)
	    RecomputeLines (pBlock, NULL, NULL, frame);
	  (*ThotLocalActions[T_checkcolumn]) (pCell, NULL, frame);
	  /* restore propagate mode */
	  Propagate = savePropagate;
	  /* Manage differed enclosings */
	  ComputeEnclosing (frame);
	}
      
      if (pAb)
	{
	  if (pAb->AbLeafType == LtGraphics)
	    pAb->AbRealShape = pAb->AbShape;
	  if (pAb->AbLeafType == LtGraphics ||
	      pAb->AbLeafType == LtPolyLine)
	    {
	      /* remonte a la recherche d'un ancetre elastique */
	      pLastAb = pAb;
	      while (pLastAb != NULL)
		{
		  pSelBox = pLastAb->AbBox;
		  if (pSelBox->BxHorizFlex || pSelBox->BxVertFlex)
		    {
		      MirrorShape (pAb, pSelBox->BxHorizInverted,
				   pSelBox->BxVertInverted,FromKeyboard);
		      /* on arrete */
		      pLastAb = NULL;
		    }
		  else
		    pLastAb = pLastAb->AbEnclosing;
		}
	    }
	  
	  /* reaffiche si necessaire */
	  if (editType != TEXT_COPY)
	    {
	      pFrame->FrReady = TRUE;
	      /* redisplay */
	      if (show)
		still = RedrawFrameBottom (frame, 0, NULL);
	    }
	  
	  if (pAb->AbElement == NULL)
	    /* le pave a disparu entre temps */
	    return;
	  
	  if (editType != TEXT_SUP)
	    NewContent (pAb);
	  if (textPasted)
	    /* send event TteElemTextModify.Post */
	    APPtextModify (pAb->AbElement, frame, FALSE);
	  else if (graphEdit)
	    APPgraphicModify (pAb->AbElement,pAb->AbPolyLineShape, frame,
			      FALSE);
	  /* signale la nouvelle selection courante */
	  if ((editType == TEXT_CUT || editType == TEXT_PASTE ||
	       editType == TEXT_X_PASTE || editType == TEXT_DEL ||
	       editType == TEXT_SUP) &&
	      pViewSel->VsBox != NULL)
	    {
	      pViewSelEnd = &pFrame->FrSelectionEnd;
	      i = pViewSel->VsBox->BxFirstChar + pViewSel->VsIndBox;
	      j = pViewSelEnd->VsBox->BxFirstChar + pViewSelEnd->VsIndBox;
	      ChangeSelection (frame, pViewSel->VsBox->BxAbstractBox, i,
			       FALSE, TRUE, FALSE, FALSE);
	      if (pAb->AbLeafType != LtPolyLine && j != i)
		ChangeSelection (frame,
				 pViewSelEnd->VsBox->BxAbstractBox, j,
				 TRUE, FALSE, FALSE, FALSE);
	    }
	}
    }
}


/*----------------------------------------------------------------------
   Insere un caractere dans une boite de texte.                    
  ----------------------------------------------------------------------*/
void InsertChar (int frame, CHAR_T c, int keyboard)
{
  PtrTextBuffer       pBuffer;
  PtrAbstractBox      pAb, pBlock;

  PtrBox              pBox;
#ifdef IV
  PtrBox              box;
#endif
  PtrBox              pSelBox;
  ViewSelection      *pViewSel;
  ViewSelection      *pViewSelEnd;
  ViewFrame          *pFrame;
  SpecFont            font;
  LeafType            nat;
  Propagation         savePropagate;
  int                 xx, xDelta, adjust;
  int                 spacesDelta;
  int                 topY, bottomY;
  int                 charsDelta, pix;
  int                 visib, zoom;
  int                 ind;
  int                 previousChars;
#ifdef _I18N_
  char                script;
#endif /* _I18N_ */
  ThotBool            beginOfBox;
  ThotBool            toDelete;
  ThotBool            toSplit;
  ThotBool            saveinsert;
  ThotBool            notification = FALSE;

  toDelete = (c == 127);
  /* Selon la valeur du parametre keyboard on essaie d'inserer */
  if (keyboard == 0)
    nat = LtSymbol;
  else if (keyboard == 1)
    nat = LtGraphics;
  else
    nat = LtText;
  /* recupere la selection active */
  SetInsert (&pAb, &frame, nat, toDelete);

  /* Ou se trouve la marque d'insertion ? */
  if (frame > 0)
    {
      pFrame = &ViewFrameTable[frame - 1];
      pViewSel = &pFrame->FrSelectionBegin;
      pViewSelEnd = &pFrame->FrSelectionEnd;
      
      if (pAb == NULL && nat != LtText)
	return;

      if (pAb == NULL)
	{
	  /* detruit dans l'element precedent ou fusionne les elements */
	  if (pViewSel->VsBox != 0)
	    {
	      pAb = pViewSel->VsBox->BxAbstractBox;
	      CloseTextInsertion ();
	      if (ThotLocalActions[T_deletenextchar] != NULL)
		(*ThotLocalActions[T_deletenextchar]) (frame, pAb->AbElement,
						       TRUE);
	    }
	}
      else
	{
	  /* Si la boite n'est pas visible */
	  if (pAb->AbVisibility < pFrame->FrVisibility)
	    {
	      /* change la visibilite de la fenetre avant d'inserer */
	      GetFrameParams (frame, &visib, &zoom);
	      SetFrameParams (frame, pAb->AbVisibility, zoom);
	      InsertChar (frame, c, keyboard);
	      return;
	    }

	  switch (pAb->AbLeafType)
	    {
	    case LtText:
	      /* prend la boite d'script courant ou au besoin */
	      /* cree un nouveau texte avec le bon script */
	      if (!toDelete)
		notification = GiveAbsBoxForLanguage (frame, &pAb,
						      keyboard);
	      if (notification)
		/* selection could be modified by the application */
		InsertChar (frame, c, keyboard);
	      else
		{
		  /* Look for the insert point */
		  GiveInsertPoint (pAb, frame, &pSelBox, &pBuffer,
				   &ind, &xx, &previousChars);
		  if (pAb)
		    {
		      /* keyboard ni Symbol ni Graphique */
		      /* bloque l'affichage de la fenetre */
		      pFrame->FrReady = FALSE;
		      
		      /* initialise l'insertion */
		      if (!TextInserting)
			StartTextInsertion (pAb, frame, pSelBox,
					    pBuffer, ind,
					    previousChars);
		      font = pSelBox->BxFont;
		      
		      if (pBuffer == NULL)
			return;
		      /* the selection should at the end of a buffer */
		      if (ind < pBuffer->BuLength && pBuffer->BuPrevious)
			{
			  pBuffer = pBuffer->BuPrevious;
			  ind = pBuffer->BuLength;
			}
		      
		      /* prepare le reaffichage */
		      /* point d'insertion en x */
		      xx += pSelBox->BxXOrg;
		      /* point d'insertion superieur en y */
		      topY = pSelBox->BxYOrg;
		      /* point d'insertion inferieur en y */
		      bottomY = topY + pSelBox->BxHeight;
#ifdef _GL
		      if (xx)
			xx = xx - 1;
		      if (bottomY + 1 < FrameTable[frame].FrHeight)
			bottomY = bottomY + 1;
		      DefClip (frame, 
			       xx, topY, 
			       xx, bottomY);
#else /*_GL*/
		      DefClip (frame, xx, topY, xx, bottomY);
#endif /*_GL*/
		      /* Est-on au debut d'une boite entiere ou coupee ? */
		      pBox = pAb->AbBox->BxNexChild;
		      if ((pBox == NULL || pSelBox == pBox) &&
			  previousChars == 0)
			beginOfBox = TRUE;
		      else
			beginOfBox = FALSE;
		  
		      if (toDelete)
			{
			/* ========================== Delete */
			if (beginOfBox)
			  {
			    /* no removable character here */
			    CloseTextInsertion ();
			    if (ThotLocalActions[T_deletenextchar] != NULL)
			      (*ThotLocalActions[T_deletenextchar]) (frame,
							 pAb->AbElement, TRUE);
			    pFrame->FrReady = TRUE;
			    return;
			  }
			else
			  {
			    if (pBuffer->BuLength == 0)
			      {
				if (pBuffer->BuPrevious)
				{
				  /* if pBuffer = first buffer of the box? */
				  if (pSelBox->BxBuffer == pBuffer)
				    {
				      /* free the current buffer */
				      pBuffer = DeleteBuffer (pBuffer, frame);
				      pSelBox->BxBuffer = pBuffer;
				      if (pSelBox->BxIndChar == 0)
					{
					  /* update the piece of box */
					  pBox->BxBuffer = pBuffer;
					  pAb->AbText = pBuffer;
					  /* Is there an empty box before? */
					  if (pBox->BxNexChild != pSelBox &&
					      pBox->BxNexChild)
					    pBox->BxNexChild->BxBuffer = pBuffer;
					}
				      else if (pSelBox->BxPrevious->BxNChars == 0)
					/* update the previous box */
					pSelBox->BxPrevious->BxBuffer = pBuffer;
				    }
				  else
				    /* free the buffer */
				    pBuffer = DeleteBuffer (pBuffer, frame);
				}
			      else if (pBuffer->BuNext)
				{
				  /* pBuffer = first buffer of the box */
				  /* update the box */
				  pSelBox->BxBuffer = pBuffer->BuNext;
				  /* free the buffer */
				  pBuffer = DeleteBuffer (pBuffer, frame);
				  pBuffer = pSelBox->BxBuffer;
				  if (pSelBox->BxFirstChar == 1)
				    {
				      pAb->AbText = pBuffer;
				      if (pBox && pBox != pSelBox)
					{
					  pBox->BxBuffer = pBuffer;
					  /* Is there an empty box before? */
					  if (pBox->BxNexChild != pSelBox &&
					      pBox->BxNexChild)
					    pBox->BxNexChild->BxBuffer = pBuffer;
					}
				    }
				  else if (pSelBox->BxPrevious->BxNChars == 0)
				    /* update the previous box */
				    pSelBox->BxPrevious->BxBuffer = pBuffer;
				}
			      }
			    /* remove the character in the buffer */
			    pBuffer->BuLength--;
			    c = pBuffer->BuContent[pBuffer->BuLength];
			    pBuffer->BuContent[pBuffer->BuLength] = EOS;
			    
			    /* update the selection at the end of the buffer */
			    if (pViewSel->VsBuffer == pBuffer)
			      {
				pViewSel->VsIndBuf--;
				ind--;
				pViewSelEnd->VsIndBuf--;
			      }
			    if (pBuffer->BuLength == 0)
			      if (pBuffer->BuPrevious)
				{
				  /* free that buffer */
				  if (pSelBox->BxBuffer == pBuffer)
				    {
				      /* and update the box */
				      pBuffer = DeleteBuffer (pBuffer, frame);
				      pSelBox->BxBuffer = pBuffer;
				      /* the index will be decremented */
				      pSelBox->BxIndChar = pBuffer->BuLength + 1;
				      if (pSelBox->BxFirstChar == 1)
					{
					  pAb->AbText = pBuffer;
					  if (pBox && pBox != pSelBox)
					    {
					      /* update the split box */
					      pBox->BxBuffer = pBuffer;
					      if (pBox->BxNexChild != pSelBox &&
						  pBox->BxNexChild)
						/* there is an empty box before */
						pBox->BxNexChild->BxBuffer = pBuffer;
					    }
					}
				      else if (pSelBox->BxPrevious->BxNChars == 0)
					/* update the previous box */
					pSelBox->BxPrevious->BxBuffer = pBuffer;
				    }
				  else
				    pBuffer = DeleteBuffer (pBuffer, frame);
				}
			    
			    /* Initialise la detruction d'un caractere */
			    charsDelta = -1;
			    pix = 0;
			    adjust = 0;
			    if (c == SPACE)
			      spacesDelta = -1;
			    else
			      spacesDelta = 0;
			    toSplit = FALSE;
			    
			    if (previousChars == 1 &&
				pSelBox->BxType == BoComplete &&
				pSelBox->BxNChars == 1)
			      {
				/* the box becomes empty */
				/* update selection marks */
				xDelta = BoxCharacterWidth (109, font);
				pViewSel->VsXPos = 0;
				pViewSel->VsIndBox = 0;
				pViewSel->VsNSpaces = 0;
				pViewSelEnd->VsXPos = xDelta;
				pViewSelEnd->VsIndBox = 0;
				pViewSelEnd->VsNSpaces = 0;
				/* prepare the box update */
				xDelta -= pSelBox->BxWidth;
				pFrame->FrClipXBegin = pSelBox->BxXOrg;
			      }
			    else if (previousChars == 0 && c != SPACE)
			      {
				/* delete a broken word or a Ctrl Return */
				/* recompute the paragraph */
				toSplit = TRUE;
				/* move the selection mark to the previous box */
				pSelBox = pSelBox->BxPrevious;
				/* update the redisplayed area */
				topY = pSelBox->BxYOrg;
				DefClip (frame, xx, topY, xx, bottomY);
				if ((c == BREAK_LINE || c == NEW_LINE) &&
				    pAb->AbBox->BxNChars == 1)
				  {
				    /* the box becomes empty */
				    xDelta = BoxCharacterWidth (109, font);
				    pFrame->FrClipXBegin = pSelBox->BxXOrg;
				    /* update selection marks */
				    pSelBox = pAb->AbBox;
				    pViewSel->VsXPos = 0;
				    pViewSel->VsIndBox = 0;
				    if (pViewSel->VsLine)
				      pViewSel->VsLine = pViewSel->VsLine->LiPrevious;
				    pViewSelEnd->VsBox = pSelBox;
				    pViewSelEnd->VsLine = pViewSel->VsLine;
				    pViewSelEnd->VsXPos = xDelta;
				    pViewSelEnd->VsIndBox = 0;
				    pViewSel->VsBox = pSelBox;
				  }
				else
				  {
				    xDelta = - BoxCharacterWidth (c, font);
				    pViewSel->VsBox = pSelBox;
				    pViewSel->VsIndBox = pSelBox->BxNChars;
				    if (pViewSel->VsLine)
				      pViewSel->VsLine = pViewSel->VsLine->LiPrevious;
				    pViewSelEnd->VsBox = pSelBox;
				    pViewSelEnd->VsIndBox = pViewSel->VsIndBox;
				    pViewSelEnd->VsLine = pViewSel->VsLine;
				    pFrame->FrClipXBegin += xDelta;
				  }
			      }
			    else if ((previousChars > pSelBox->BxNChars ||
				      previousChars == 0) &&
				     c == SPACE)
			      {
				/* removing a space between two boxes */
				/* recompute the paragraph */
				toSplit = TRUE;
				xDelta = - BoxCharacterWidth (c, font);
				if (previousChars == 0)
				  {
				    /* selection at the beginning of the box  */
				    /* recompute from the previous box */
				    pBox = pSelBox->BxPrevious;
				    if (pViewSel->VsLine)
				      pViewSel->VsLine = pViewSel->VsLine->LiPrevious;
				    if (pBox->BxNChars == 0 &&
					pBox->BxFirstChar == 1)
				      {
					/* the empty previous box is the first */
					pBox = pAb->AbBox;
					/*
					  update the first position of the selected
					  box because a space is removed before
					*/
					pSelBox->BxFirstChar--;
					if (pBox->BxNChars == 1)
					  /* the box becomes empty */
					  xDelta = BoxCharacterWidth (109, font) - pBox->BxWidth;
				      }
				    else if (pBox->BxFirstChar == 1)
				      {
					/* recompute the whole split box */
					pBox = pAb->AbBox;
					/*
					  update the first position of the selected
					  box because a space is removed before
					*/
					pSelBox->BxFirstChar--;
				      }
				    pSelBox = pBox;
				  }
				else
				  {
				    pViewSel->VsIndBox--;
				    pViewSelEnd->VsIndBox--;
				  }
				/* update the displayed area */
				pFrame->FrClipXBegin += xDelta;
			      }
			    else
			      {
				/* other cases */
				if (c == SPACE)
				  {
				    xDelta = - BoxCharacterWidth (SPACE, font);
				    adjust = - pSelBox->BxSpaceWidth;
				    if (adjust < 0)
				      {
					if (pSelBox->BxNPixels >= pViewSel->VsNSpaces)
					  pix = -1;
				      }
				  }
				else if (c == EOS)
				  /* null character */
				  xDelta = 0;
				else
				  xDelta = - BoxCharacterWidth (c, font);
				/* update the displayed area */
				pFrame->FrClipXBegin += xDelta;
				/* update selection marks */
				if (adjust)
				  UpdateViewSelMarks (frame, adjust + pix, spacesDelta, charsDelta);
				else
				  UpdateViewSelMarks (frame, xDelta, spacesDelta, charsDelta);
			      }
			  }
			}
		      else
			{
			  /* Insert a new character */
			  charsDelta = 1;
			  pix = 0;
			  adjust = 0;
			  if (c == SPACE)
			    spacesDelta = 1;
			  else
			    spacesDelta = 0;
			  toSplit = FALSE;
			  toDelete = FALSE;
#ifdef _I18N_
			  script = TtaGetCharacterScript (c);
#ifdef IV /* not enough stable */
			  if  (script != ' ' && script != 'D' &&
			       pSelBox && pSelBox->BxScript != EOS &&
			       pSelBox->BxScript != script)
			    {
			      /* update the clipping */
			      DefClip (frame, pSelBox->BxXOrg, topY,
				       pSelBox->BxXOrg + pSelBox->BxWidth, bottomY);
			      /* split the current box */
			      pBox = SplitForScript (pSelBox, pAb, pSelBox->BxScript,
						     previousChars,
						     xx - pSelBox->BxXOrg, pSelBox->BxH,
						     pViewSel->VsNSpaces,
						     ind, pBuffer,
						     ViewFrameTable[frame - 1].FrAbstractBox->AbBox);
			      pBox->BxScript = pSelBox->BxScript;
			      /* add the new box */
			      box = GetBox (pAb);
			      pSelBox = pBox->BxPrevious;
			      pSelBox->BxNexChild = box;
			      pSelBox->BxNext = box;
			      box->BxPrevious = pSelBox;
			      box->BxNext = pBox;
			      box->BxNexChild = pBox;
			      pBox->BxPrevious = box;
			      box->BxType = BoScript;
			      box->BxScript = script;
			      box->BxAbstractBox = pAb;
			      box->BxIndChar = ind;
			      box->BxContentWidth = TRUE;
			      box->BxContentHeight = TRUE;
			      box->BxFont = pBox->BxFont;
			      box->BxUnderline = pBox->BxUnderline;
			      box->BxThickness = pBox->BxThickness;
			      box->BxHorizRef = pBox->BxHorizRef;
			      box->BxH = pBox->BxH;
			      box->BxHeight = pBox->BxHeight;
			      box->BxW = 0;
			      box->BxWidth = 0;
			      box->BxTMargin = pBox->BxTMargin;
			      box->BxTBorder = pBox->BxTBorder;
			      box->BxTPadding = pBox->BxTPadding;
			      box->BxBMargin = pBox->BxBMargin;
			      box->BxBBorder = pBox->BxBBorder;
			      box->BxBPadding = pBox->BxBPadding;
			      box->BxRMargin = 0;
			      box->BxRBorder = 0;
			      box->BxRPadding = 0;
			      box->BxBuffer = pBuffer;
			      box->BxFirstChar = pBox->BxFirstChar;
			      box->BxNChars = 0;
			      box->BxNSpaces = 0;
			      box->BxYOrg = pBox->BxYOrg;
			      if (pAb->AbDirection == 'L')
				/* insert after the first box */
				box->BxXOrg = pBox->BxXOrg;
			      else
				/* insert before the first box */
				box->BxXOrg = pSelBox->BxXOrg;
			      /*xx = 0;
				previousChars = 0;*/
			      pSelBox = pAb->AbBox;
			      toSplit = TRUE;
			    }
#endif /* IV */
#endif /* _I18N_ */
			  
			  /* Si la selection debutait sur une boite de presentation */
			  /* il faut deplacer la selection sur le premier caractere */
			  /* de la boite de texte (ou en fin de boite vide)         */
			  if (pSelBox != pViewSel->VsBox)
			    {
			      pViewSel->VsBox = pSelBox;
			      pViewSel->VsBuffer = pBuffer;
			      pViewSel->VsIndBuf = ind;
			      pViewSel->VsIndBox = previousChars;
			      pViewSel->VsXPos = 0;
			      pViewSel->VsNSpaces = 0;
			    }
			  
			  /* ajoute le caractere dans la chaine des buffers */
			  if (pBuffer->BuLength == FULL_BUFFER)
			    pBuffer = GetNewBuffer (pBuffer, frame);
			  pBuffer->BuContent[pBuffer->BuLength] = c;
			  pBuffer->BuLength++;
			  pBuffer->BuContent[pBuffer->BuLength] = EOS;
			  if (pBuffer == pViewSel->VsBuffer)
			    {
			      /* Selection en fin de boite */
			      pViewSel->VsIndBuf++;
			      pViewSelEnd->VsIndBuf++;
			    }
			  
			  /* ==> La boite entiere n'est plus vide */
			  if (pSelBox->BxNChars == 0 &&
			      pSelBox->BxType == BoComplete)
			    {
			      /* Mise a jour des marques */
			      xDelta = BoxCharacterWidth (c, font);
			      pViewSel->VsXPos = xDelta;
			      pViewSel->VsIndBox = charsDelta;
			      pViewSel->VsNSpaces = spacesDelta;
			      pViewSelEnd->VsXPos = xDelta + 2;
			      pViewSelEnd->VsIndBox = charsDelta;
			      pViewSelEnd->VsNSpaces = spacesDelta;
			      /* Le caractere insere' est un Ctrl Return ? */
			      if (c == BREAK_LINE || c == NEW_LINE)
				{
				  /* il faut reevaluer la mise en ligne */
				  toSplit = TRUE;
				  xDelta = 0;
				}
			      DefClip (frame, pSelBox->BxXOrg, pSelBox->BxYOrg,
				       pSelBox->BxXOrg + pSelBox->BxWidth,
				       pSelBox->BxYOrg + pSelBox->BxHeight);
			      /* Prepare la mise a jour de la boite */
			      xDelta -= pSelBox->BxWidth;
			    }
			  /* ==> Insertion d'un caractere entre deux boites */
			  else if (previousChars > pSelBox->BxNChars ||
				   /* ==> ou d'un blanc en fin de boite      */
				   (c == SPACE &&
				    (previousChars == pSelBox->BxNChars ||
				     previousChars == 0)))
			    {
			      /* Prepare la mise a jour de la boite */
			      toSplit = TRUE;
			      xDelta = BoxCharacterWidth (c, font);
			      if (c == SPACE)
				adjust = pSelBox->BxSpaceWidth;
			      if (previousChars == 0 && pSelBox->BxFirstChar == 1)
				{
				  /* Reevaluation of the whole split box */
				  pSelBox = pAb->AbBox;
				  UpdateViewSelMarks (frame, xDelta, spacesDelta, charsDelta);
				}
			      else if (previousChars == 0)
				{
				  /* Si la selection est en debut de boite  */
				  /* on force la reevaluation du bloc de    */
				  /* lignes a partir de la boite precedente */
				  pSelBox = pSelBox->BxPrevious;
				  if (pViewSel->VsLine != NULL)
				    pViewSel->VsLine = pViewSel->VsLine->LiPrevious;
				}
			      else
				UpdateViewSelMarks (frame, xDelta, spacesDelta, charsDelta);
			      
			    }
			  /* ==> Les autres cas d'insertion */
			  else
			    {
			      if (c == SPACE)
				{
				  xDelta = BoxCharacterWidth (SPACE, font);
				  adjust = pSelBox->BxSpaceWidth;
				  if (adjust > 0)
				    if (pSelBox->BxNPixels > pViewSel->VsNSpaces)
				      pix = 1;
				}
			      else if (c == BREAK_LINE || c == NEW_LINE)
				/* Ctrl Return */
				{
				  /* il faut reevaluer la mise en ligne */
				  toSplit = TRUE;
				  xDelta = BoxCharacterWidth (c, font);
				}
			      else if (c == EOS)
				/* Caractere Nul */
				xDelta = 0;
			      else
				xDelta = BoxCharacterWidth (c, font);
			      
			      if (/*beginOfBox && */previousChars == 0)
				{
				  /* insert at the beginning of a box */
				  pSelBox->BxBuffer = pBuffer;
				  pSelBox->BxIndChar = ind;
				  if (pSelBox->BxNChars == 0)
				    /* the box is not empty now, get the right width */
				    pSelBox->BxWidth = 0;
				}
			      
			      /* Mise a jour de la selection dans la boite */
			      if (adjust != 0)
				UpdateViewSelMarks (frame, adjust + pix, spacesDelta, charsDelta);
			      else
				UpdateViewSelMarks (frame, xDelta, spacesDelta, charsDelta);
			    }
			}
		      
		      /* Mise a jour de la boite */
		      if (IsLineBreakInside (pSelBox->BxBuffer, pSelBox->BxIndChar, pSelBox->BxNChars))
			toSplit = TRUE;
		      BoxUpdate (pSelBox, pViewSel->VsLine, charsDelta,
				 spacesDelta, xDelta, adjust, 0, frame, toSplit);
		      /* Mise a jour du volume du pave */
		      pAb->AbVolume += charsDelta;
		      
		      /* Check enclosing cell */
		      if (LastInsertCell && ThotLocalActions[T_checkcolumn])
			{
			  /* we have to propage position to cell children */
			  savePropagate = Propagate;
			  Propagate = ToChildren;
			  pBlock = SearchEnclosingType (pAb, BoBlock);
			  if (pBlock != NULL)
			    RecomputeLines (pBlock, NULL, NULL, frame);
			  (*ThotLocalActions[T_checkcolumn]) (LastInsertCell,
							      NULL, frame);
			  /* restore propagate mode */
			  Propagate = savePropagate;
			}
		      /* Traitement des englobements retardes */
		      ComputeEnclosing (frame);
		      
		      /* evite le traitement de la fin d'insertion */
		      saveinsert = TextInserting;
		      TextInserting = FALSE;
		      
		      /* teste si l'on peut optimiser le reaffichage */
		      if (IsScrolled (frame, 0))
			  {
			    if (!toDelete)
			      {
				/* largeur du rectangle d'affichage */
				charsDelta = pFrame->FrClipXEnd - pFrame->FrClipXBegin;
				/* largeur du caractere ajoute/detruit */
				if (xDelta < 0)
				  xDelta = -xDelta;
				/* This test was used with the old selection
				if (pFrame->FrClipYBegin == topY &&
				    pFrame->FrClipYEnd == bottomY &&
				    xDelta >= charsDelta &&
				    (pFrame->FrClipXBegin == xx ||
				     pFrame->FrClipXEnd == xx))
				  ;
				end of old text */
			      }
			    DefClip (frame, pFrame->FrClipXBegin, topY,
				     pFrame->FrClipXEnd + 2, bottomY);
			    RedrawFrameBottom (frame, 0, NULL);
			  }
		      
		      /* restaure l'indicateur d'insertion */
		      TextInserting = saveinsert;
		      /* Affiche la nouvelle selection */
		      pFrame->FrReady = TRUE;
		    }
		}
	      break;
	      /* Saisie d'un caractere de symbole ou graphique */
	    case LtSymbol:
	      if (keyboard == 3)
		/* From Greek palette (Adobe Symbol font) */
		/* convert the character into its Thot symbol equivalent */
		{
		  switch ((int) c)
		    {
		    case 40:  c = '('; break; /* ( */
		    case 41:  c = ')'; break; /* ) */
		    case 45:  c = 'h'; break; /* - horizontal bar */
		    case 60:  c = '<'; break; /* pointy bracket */
		    case 62:  c = '>'; break; /* pointy bracket */
		    case 80:  c = 'P'; break; /* Pi */
		    case 83:  c = 'S'; break; /* Sigma */
		    case 91:  c = '['; break; /* [ */
		    case 93:  c = ']'; break; /* ] */
		    case 95:  c = 'h'; break; /* _ horizontal bar */
		    case 123: c = '{'; break; /* { */
		    case 124: c = '|'; break; /* | */
		    case 125: c = '}'; break; /* } */
		    case 172: c = 'L'; break; /* Left arrow */
		    case 173: c = '^'; break; /* Top arrow */
		    case 174: c = 'R'; break; /* Right arrow */
		    case 175: c = 'V'; break; /* bottom arrow */
		    case 189: c = '|'; break; /* | */
		    case 190: c = 'h'; break; /* - horizontal bar */
		    case 199: c = 'I'; break; /* Inter */
		    case 200: c = 'U'; break; /* Union */
		    case 213: c = 'P'; break; /* Pi */
		    case 214: c = 'r'; break; /* Root */
		    case 225: c = '<'; break; /* pointy bracket */
		    case 229: c = 'S'; break; /* Sigma */
		    case 241: c = '>'; break; /* pointy bracket */
		    case 242: c = 'i'; break; /* Integral */
		    default:  c = ' '; break;
		    }
		  keyboard = -1;   /* simulate keyboard input */
		}
	      if (keyboard == -1 || keyboard == 0)
		/* saisie au clavier ou par la palette Math symbols */
		{
		  FromKeyboard = TRUE;
		  ContentEditing ((int) c);
		  FromKeyboard = FALSE;
		}
	      break;
	    case LtGraphics:
	    case LtPolyLine:
	      if (keyboard == 1)
		{
		  FromKeyboard = TRUE;
		  ContentEditing ((int) c);
		  FromKeyboard = FALSE;
		}
	      break;
	    case LtPicture:
	      {
		FromKeyboard = TRUE;
		ContentEditing ((int) c);
		FromKeyboard = FALSE;
	      }
	    break;
	    default:
	      break;
	    }
	  SetDocumentModified (LoadedDocument[FrameTable[frame].FrDoc - 1], TRUE, 1); 
	}
    }
}


/*----------------------------------------------------------------------
   PasteXCliboard reads nbytes from the buffer and calls Paste_X as
   many times as necessary with the characters read.     
  ----------------------------------------------------------------------*/
static void PasteXClipboard (unsigned char *src, int nbytes)
{
  PtrTextBuffer       clipboard;
  PtrAbstractBox      pAb;
  PtrDocument         pDoc;
  PtrElement          pEl;
  DisplayMode         dispMode;
  Document            doc;
  CHAR_T             *buffer, b;
  int                 i, j;
  int                 frame, lg;
  ThotBool            lock = TRUE;

  /* check the current selection */
  if (!GetCurrentSelection (&pDoc, &pEl, &pEl, &i, &i))
    /* cannot insert */
    return;
  frame = ActiveFrame;
  SetInsert (&pAb, &frame, LtText, FALSE);
  if (pAb == NULL)
    /* invalid selection */
    return;
  /* clean up the X clipboard */
  clipboard = &XClipboard;
  ClearClipboard (clipboard);
  /* number of characters in the last buffer of the clipboard X */
  j = 0;
  /* number of characters in the whole X clipboard */
  lg = 0;
  if (src)
    {
#ifdef _I18N_
      /* What is the encoding used by external applications ??? */
      buffer = TtaConvertByteToCHAR (src, TtaGetDefaultCharset ());
#else /* _I18N_ */
      buffer = src;
#endif /* _I18N_ */
      doc = IdentDocument (pDoc);
      dispMode = TtaGetDisplayMode (doc);
      if (dispMode == DisplayImmediately)
	TtaSetDisplayMode (doc, DeferredDisplay);
      /* lock tables formatting */
      if (ThotLocalActions[T_islock])
	{
	  (*ThotLocalActions[T_islock]) (&lock);
	  if (!lock)
	    /* table formatting is not loked, lock it now */
	    (*ThotLocalActions[T_lock]) ();
	}

      i = 0;
      while (i < nbytes)
	{
	  if (j == FULL_BUFFER)
	    {
	      /* Allocate a new buffer */
	      clipboard->BuLength = j;
	      clipboard = GetNewBuffer (clipboard, ActiveFrame);
	      j = 0;
	    }
	  
	  b = buffer[i];
	  if (b == 8 || b == 12 || b == 160)
	    /* BS, FF, nbsp becomes a space */
	    clipboard->BuContent[j++] = SPACE;
	  else if (b == EOL)
	    {
	      clipboard->BuContent[j++] = b;
	      /* should we generate a break-line ???? */
	    }
	  else if (b >= 32)
	    clipboard->BuContent[j++] = b;
	  i++;
	}

#ifdef _I18N_
      TtaFreeMemory (buffer);
#endif /* _I18N_ */
      /* Paste the last X clipboard buffer */
      if (j > 0)
	{
	  clipboard->BuLength = j;
	  ContentEditing (TEXT_X_PASTE);
	}
      if (!lock)
	/* unlock table formatting */
	(*ThotLocalActions[T_unlock]) ();
      if (dispMode == DisplayImmediately)
	TtaSetDisplayMode (doc, dispMode);
    }
}

#ifdef _WINDOWS
/*----------------------------------------------------------------------
   PasteXClipboardW reads nchars from the buffer and calls Paste_X as
   many times as necessary with the characters read.     
  ----------------------------------------------------------------------*/
/* MJD: Not sure we need double functions here (PasteXClipboard and PasteXClipboardW);
   better to use only one (PasteXClipboard, with signature of current PasteXClipboardW). */
static void PasteXClipboardW (wchar_t* src, int nchars)
{
  PtrTextBuffer       clipboard;
  PtrAbstractBox      pAb;
  PtrDocument         pDoc;
  PtrElement          pEl;
  DisplayMode         dispMode;
  Document            doc;
  CHAR_T             *buffer, b;
  int                 i, j;
  int                 frame, lg;
  ThotBool            lock = TRUE;

  /* check the current selection */
  if (!GetCurrentSelection (&pDoc, &pEl, &pEl, &i, &i))
    /* cannot insert */
    return;
  frame = ActiveFrame;
  SetInsert (&pAb, &frame, LtText, FALSE);
  if (pAb == NULL)
    /* invalid selection */
    return;
  /* clean up the X clipboard */
  clipboard = &XClipboard;
  ClearClipboard (clipboard);
  /* number of characters in the last buffer of the clipboard X */
  j = 0;
  /* number of characters in the whole X clipboard */
  lg = 0;
  if (src)
    {
      buffer = src;
      doc = IdentDocument (pDoc);
      dispMode = TtaGetDisplayMode (doc);
      if (dispMode == DisplayImmediately)
	TtaSetDisplayMode (doc, DeferredDisplay);
      /* lock tables formatting */
      if (ThotLocalActions[T_islock])
	{
	  (*ThotLocalActions[T_islock]) (&lock);
	  if (!lock)
	    /* table formatting is not loked, lock it now */
	    (*ThotLocalActions[T_lock]) ();
	}

      i = 0;
      while (i < nchars)
	{
	  if (j == FULL_BUFFER)
	    {
	      /* Allocate a new buffer */
	      clipboard->BuLength = j;
	      clipboard = GetNewBuffer (clipboard, ActiveFrame);
	      j = 0;
	    }
	  
	  b = buffer[i++];
	  if (b == 8 || b == 12 || b == 160)
	    /* BS, FF, nbsp becomes a space */
	    clipboard->BuContent[j++] = SPACE;
	  else if (b == EOL)
	    {
	      clipboard->BuContent[j++] = b;
	      /* should we generate a break-line ???? */
	    }
	  else if (b >= 32)
	    clipboard->BuContent[j++] = b;
	}

      /* Paste the last X clipboard buffer */
      if (j > 0)
	{
	  clipboard->BuLength = j;
	  ContentEditing (TEXT_X_PASTE);
	}
      if (!lock)
	/* unlock table formatting */
	(*ThotLocalActions[T_unlock]) ();
      if (dispMode == DisplayImmediately)
	TtaSetDisplayMode (doc, dispMode);
    }
}
#endif /* _WINDOWS */

/*----------------------------------------------------------------------
   TtcInsertChar insert a character
  ----------------------------------------------------------------------*/
void TtcInsertChar (Document doc, View view, CHAR_T c)
{
  ViewSelection      *pViewSel;
  PtrAbstractBox      pAb;
  PtrAbstractBox      draw;
  DisplayMode         dispMode;
  PtrDocument         pDoc;
  PtrElement          firstEl, lastEl;
  int                 firstChar, lastChar;
  int                 frame;
  ThotBool            lock = TRUE;

  if (doc != 0)
    {
      frame = GetWindowNumber (doc, view);
      /* Check if we are changing the active frame */
      if (frame != ActiveFrame)
	{
	  /* yes close the previous insertion */
	  CloseTextInsertion ();
	  if (ActiveFrame > 0 && FrameTable[ActiveFrame].FrDoc != doc)
	    return;
	  else
	    /* use the right frame */
	    ActiveFrame = frame;
	  /* work in the right document
	     frame = ActiveFrame;
	     doc =  FrameTable[ActiveFrame].FrDoc;*/
	}
      pViewSel = &ViewFrameTable[frame - 1].FrSelectionBegin;
      if (pViewSel->VsBox &&
	  pViewSel->VsBox->BxAbstractBox &&
	  pViewSel->VsBox->BxAbstractBox->AbReadOnly)
	{
	  if (pViewSel->VsBox->BxAbstractBox->AbEnclosing->AbReadOnly)
	    /* cannot insert here */
	    return;
	  else if (pViewSel->VsIndBox + pViewSel->VsBox->BxFirstChar > 0 &&
		   pViewSel->VsIndBox + pViewSel->VsBox->BxFirstChar < pViewSel->VsBox->BxAbstractBox->AbVolume)
	    /* cannot insert here */
	    return;
	}

      /* start the undo sequence */
      GetCurrentSelection (&pDoc, &firstEl, &lastEl, &firstChar, &lastChar);
      if (pDoc && pViewSel->VsBox)
	{
	  /* avoid to redisplay step by step */
	  dispMode = TtaGetDisplayMode (doc);
	  if (dispMode == DisplayImmediately)
	    TtaSetDisplayMode (doc, DeferredDisplay);
	  OpenHistorySequence (pDoc, firstEl, lastEl, firstChar, lastChar);
	  /* lock tables formatting */
	  if (ThotLocalActions[T_islock])
	    {
	      (*ThotLocalActions[T_islock]) (&lock);
	      if (!lock)
		/* table formatting is not loked, lock it now */
		(*ThotLocalActions[T_lock]) ();
	    }
      
	  if (!StructSelectionMode &&
	      !ViewFrameTable[frame - 1].FrSelectOnePosition &&
	      !pViewSel->VsBox->BxAbstractBox->AbReadOnly)
	    {
	      /* Delete the current selection */
	      CloseTextInsertion ();
	      if (pViewSel->VsBox)
		{
		  pAb = pViewSel->VsBox->BxAbstractBox;
		  draw = GetParentDraw (pViewSel->VsBox);
		  if (pAb->AbLeafType == LtText || pAb->AbLeafType == LtSymbol)
		    ContentEditing (TEXT_SUP);
		  else if (draw)
		    {
		      /* move the selection and reapply the command */
		      pViewSel->VsBox = draw->AbBox;
		      pViewSel->VsBuffer = NULL;
		      pViewSel->VsIndBuf = 0;
		      pViewSel->VsIndBox = 0;
		      pViewSel->VsXPos = 0;
		      pViewSel->VsNSpaces = 0;
		      pViewSel->VsLine = NULL;
		      pViewSel = &ViewFrameTable[frame - 1].FrSelectionEnd;
		      pViewSel->VsBox = draw->AbBox;
		      pViewSel->VsBuffer = NULL;
		      pViewSel->VsIndBuf = 0;
		      pViewSel->VsIndBox = 0;
		      pViewSel->VsXPos = draw->AbBox->BxWidth;
		      pViewSel->VsNSpaces = 0;
		      pViewSel->VsLine = NULL;
		      TtcInsertChar (doc, view, c);
		      /* restore the display mode */
		      if (dispMode == DisplayImmediately)
			TtaSetDisplayMode (doc, DisplayImmediately);
		      return;
		    }
		  else if (pAb->AbLeafType == LtPicture ||
			   pAb->AbLeafType == LtGraphics)
		    ContentEditing (TEXT_SUP);
		  else if (pAb->AbLeafType != LtCompound || pAb->AbVolume != 0)
		    TtcPreviousChar (doc, view);
		}
	    }
	  InsertChar (frame, c, -1);
	  if (!lock)
	    /* unlock table formatting */
	    (*ThotLocalActions[T_unlock]) ();

	  /* close the undo sequence */
	  CloseHistorySequence (pDoc);

	  /* compare the document encoding and the character value */
	  if (pDoc->DocCharset == US_ASCII && c > 127)
	    {
	      /* force the ISO-latin-1 */
	      pDoc->DocCharset = ISO_8859_1;
	      pDoc->DocDefaultCharset = FALSE;
	    }
	  if (dispMode == DisplayImmediately)
	    TtaSetDisplayMode (doc, dispMode);
	}
    }
}


/*----------------------------------------------------------------------
   TtcCutSelection                                                    
  ----------------------------------------------------------------------*/
void TtcCutSelection (Document doc, View view)
{
   DisplayMode         dispMode;
   int                 frame;
   ThotBool            lock = TRUE;
#ifdef _WINDOWS
   HANDLE              hMem   = 0;
   LPTSTR              lpData = 0;
   char               *ptrData;
   LPTSTR              pBuff;
   int                 ndx;
#endif /* _WINDOWS */

   if (doc == 0)
     return;
   /* avoid to redisplay step by step */
   dispMode = TtaGetDisplayMode (doc);
   if (dispMode == DisplayImmediately)
     TtaSetDisplayMode (doc, DeferredDisplay);
   /* lock tables formatting */
   if (ThotLocalActions[T_islock])
     {
       (*ThotLocalActions[T_islock]) (&lock);
       if (!lock)
	 /* table formatting is not loked, lock it now */
	 (*ThotLocalActions[T_lock]) ();
     }

   frame = GetWindowNumber (doc, view);
   if (frame != ActiveFrame)
     {
       /* yes close the previous insertion */
       CloseTextInsertion ();
       if (ActiveFrame > 0 && FrameTable[ActiveFrame].FrDoc != doc)
	 return;
       else
	 /* use the right frame */
	 ActiveFrame = frame;
     }
#ifdef _WINDOWS
   TtcCopyToClipboard (doc, view);

   if (!OpenClipboard (FrRef[frame]))
     WinErrorBox (FrRef [frame], "TtcCutSelection (1)");
   else
     {
       EmptyClipboard ();
       hMem   = GlobalAlloc (GHND, ClipboardLength + 1);
       lpData = (LPTSTR) GlobalLock (hMem);
       ptrData = lpData;
       pBuff  = (LPTSTR) Xbuffer;
       for (ndx = 0; ndx < ClipboardLength; ndx++)
	 *ptrData++ = *pBuff++;
       *ptrData = 0;

       GlobalUnlock (hMem);
       SetClipboardData (CF_TEXT, hMem);
       /* add Unicode clipboard here (CF_UNICODETEXT) */
       CloseClipboard ();
     }
#endif /* _WINDOWS */
   ContentEditing (TEXT_CUT);

   if (!lock)
     /* unlock table formatting */
     (*ThotLocalActions[T_unlock]) ();
   if (dispMode == DisplayImmediately)
     TtaSetDisplayMode (doc, dispMode);
}

/*----------------------------------------------------------------------
   TtcDeletePreviousChar                                           
  ----------------------------------------------------------------------*/
void TtcDeletePreviousChar (Document doc, View view)
{
  ViewSelection      *pViewSel;
  DisplayMode         dispMode;
  int                 frame;
  ThotBool            delPrev, moveAfter;
  ThotBool            lock = TRUE;

  if (doc != 0)
    {
      frame = GetWindowNumber (doc, view);
      if (frame != ActiveFrame)
	{
	  /* yes close the previous insertion */
	  CloseTextInsertion ();
	  if (ActiveFrame > 0 && FrameTable[ActiveFrame].FrDoc != doc)
	    return;
	  else
	    /* use the right frame */
	    ActiveFrame = frame;
	  /* work in the right document
	     frame = ActiveFrame;
	     doc =  FrameTable[ActiveFrame].FrDoc;*/
	}
      delPrev = (StructSelectionMode || ViewFrameTable[frame - 1].FrSelectOnePosition);
      pViewSel = &ViewFrameTable[frame - 1].FrSelectionBegin;
      if (delPrev)
	/* remove the current empty element even if there is an insert point */
	delPrev = (pViewSel->VsBox != NULL && pViewSel->VsBox->BxAbstractBox->AbVolume != 0);
      else
	/* remove the previous char if the selection is at the end of the text */
	delPrev = (pViewSel->VsBox != NULL &&
		   pViewSel->VsBox->BxAbstractBox->AbLeafType == LtText &&
		   pViewSel->VsIndBox >= pViewSel->VsBox->BxNChars);
      
      /* avoid to redisplay step by step */
      dispMode = TtaGetDisplayMode (doc);
      if (dispMode == DisplayImmediately)
	TtaSetDisplayMode (doc, DeferredDisplay);
      /* lock tables formatting */
      if (ThotLocalActions[T_islock])
	{
	  (*ThotLocalActions[T_islock]) (&lock);
	  if (!lock)
	    /* table formatting is not loked, lock it now */
	    (*ThotLocalActions[T_lock]) ();
	}
      
      if (delPrev)
	{
	  pViewSel = &ViewFrameTable[frame - 1].FrSelectionBegin;
	  if (pViewSel->VsBox != NULL &&
	      pViewSel->VsBox->BxAbstractBox != NULL &&
	      (!pViewSel->VsBox->BxAbstractBox->AbReadOnly ||
	       pViewSel->VsBox->BxFirstChar + pViewSel->VsIndBox == 0 ||
	       (pViewSel->VsBox->BxFirstChar + pViewSel->VsIndBox == 1 &&
		pViewSel->VsBox->BxAbstractBox->AbVolume == 1)))
	    InsertChar (frame, 127, -1);
	}
      else
	{
	  /* delete the current selection instead of the previous char */
	  CloseTextInsertion ();
	  /* by default doen't change the selection after the delete */
	  moveAfter = FALSE;
	  if (pViewSel->VsBox != NULL)
	    {
	      moveAfter = (pViewSel->VsBox->BxAbstractBox->AbLeafType != LtText ||
			   pViewSel->VsBox->BxAbstractBox->AbVolume == 0);
	      ContentEditing (TEXT_SUP);
	    }
	  pViewSel = &ViewFrameTable[frame - 1].FrSelectionEnd;
	  if (moveAfter &&
	      pViewSel->VsBox && pViewSel->VsIndBox < pViewSel->VsBox->BxNChars)
	    TtcPreviousChar (doc, view);
	}
      
      if (!lock)
	/* unlock table formatting */
	(*ThotLocalActions[T_unlock]) ();
      if (dispMode == DisplayImmediately)
	TtaSetDisplayMode (doc, dispMode);
    }
}


/*----------------------------------------------------------------------
   TtcDeleteSelection                                                 
  ----------------------------------------------------------------------*/
void TtcDeleteSelection (Document doc, View view)
{
   DisplayMode         dispMode;
   ThotBool            lock = TRUE;

   if (doc == 0)
     return;
   /* avoid to redisplay step by step */
   dispMode = TtaGetDisplayMode (doc);
   if (dispMode == DisplayImmediately)
     TtaSetDisplayMode (doc, DeferredDisplay);
   /* lock tables formatting */
   if (ThotLocalActions[T_islock])
     {
       (*ThotLocalActions[T_islock]) (&lock);
       if (!lock)
	 /* table formatting is not loked, lock it now */
	 (*ThotLocalActions[T_lock]) ();
     }
   
   ContentEditing (TEXT_DEL);
   
   if (!lock)
     /* unlock table formatting */
     (*ThotLocalActions[T_unlock]) ();
   if (dispMode == DisplayImmediately)
     TtaSetDisplayMode (doc, dispMode);
}


/*----------------------------------------------------------------------
   TtcInclude                                                         
  ----------------------------------------------------------------------*/
void TtcInclude (Document document, View view)
{
   DisplayMode         dispMode;
   ThotBool            lock = TRUE;
   ThotBool            ok;

   if (ThotLocalActions[T_insertpaste] != NULL && document != 0)
     {
	/* avoid to redisplay step by step */
	dispMode = TtaGetDisplayMode (document);
	if (dispMode == DisplayImmediately)
	  TtaSetDisplayMode (document, DeferredDisplay);
	/* lock tables formatting */
	if (ThotLocalActions[T_islock])
	  {
	    (*ThotLocalActions[T_islock]) (&lock);
	    if (!lock)
	      /* table formatting is not loked, lock it now */
	      (*ThotLocalActions[T_lock]) ();
	  }
	(*ThotLocalActions[T_insertpaste]) (FALSE, FALSE, 'L', &ok);

	if (!lock)
	  /* unlock table formatting */
	  (*ThotLocalActions[T_unlock]) ();
	if (dispMode == DisplayImmediately)
	  TtaSetDisplayMode (document, dispMode);
     }
}

/*----------------------------------------------------------------------
   TtcPastefromX                                                      
  ----------------------------------------------------------------------*/
void TtcPasteFromClipboard (Document document, View view)
{
   DisplayMode         dispMode;
   int                 frame;
#if !defined(_WINDOWS) && !defined(_GTK)
   int                 i;
   ThotWindow          w, wind;
#endif /* _WINDOWS && _GTK */
   ThotBool            lock = TRUE;

   if (document == 0)
      return;
   /* avoid to redisplay step by step */
   dispMode = TtaGetDisplayMode (document);
   if (dispMode == DisplayImmediately)
     TtaSetDisplayMode (document, DeferredDisplay);
   /* lock tables formatting */
   if (ThotLocalActions[T_islock])
     {
       (*ThotLocalActions[T_islock]) (&lock);
       if (!lock)
	 /* table formatting is not loked, lock it now */
	 (*ThotLocalActions[T_lock]) ();
     }

   frame = GetWindowNumber (document, view);
#ifndef _WINDOWS
#ifdef _GTK
   if (Xbuffer)
     PasteXClipboard (Xbuffer, strlen(Xbuffer)); 
#else
   w = XGetSelectionOwner (TtDisplay, XA_PRIMARY);
   wind = FrRef[frame];
   if (w == None)
     {
	/* Pas de selection courante -> on regarde s'il y a un cutbuffer */
	Xbuffer = XFetchBytes (TtDisplay, &i);
	if (Xbuffer)
	   PasteXClipboard (Xbuffer, i);
     }
   else
      XConvertSelection (TtDisplay, XA_PRIMARY, XA_STRING, XA_CUT_BUFFER0,
			 wind, CurrentTime);
#endif /* _GTK*/
#endif /* _WINDOWS */
   if (!lock)
     /* unlock table formatting */
     (*ThotLocalActions[T_unlock]) ();
   if (dispMode == DisplayImmediately)
     TtaSetDisplayMode (document, dispMode);
   /*TtcPaste (document, view);*/
}


/*----------------------------------------------------------------------
   TtcInsert                                                          
  ----------------------------------------------------------------------*/
void TtcInsert (Document document, View view)
{
   ContentEditing (TEXT_INSERT);
}


/*----------------------------------------------------------------------
    TtcCopySelection
  ----------------------------------------------------------------------*/
void TtcCopySelection (Document doc, View view)
{
  int                frame;
#ifdef _WINDOWS
  HANDLE             hMem   = 0;
  char              *lpData = NULL;
  char              *pBuff;
  HWND               activeWnd;
#endif /* _WINDOW */

  if (doc == 0)
    return;
  frame = GetWindowNumber (doc, view);
  if (frame != ActiveFrame)
    {
      if (ActiveFrame > 0 && FrameTable[ActiveFrame].FrDoc != doc)
	return;
      else
	/* use the right frame */
	ActiveFrame = frame;
    }
#ifdef _WINDOWS
  activeWnd = GetFocus ();
  if (activeWnd == FrRef [frame])
    {
      TtcCopyToClipboard (doc, view);
      if (OpenClipboard (FrRef[frame]))
	{
	  EmptyClipboard ();
	  /* if the clipboard buffer is empty, don't copy anything into it */
	  if (Xbuffer)
	    {
	      hMem   = GlobalAlloc (GHND, ClipboardLength + 1);
	      lpData = GlobalLock (hMem);
	      pBuff  = Xbuffer;
	      lstrcpy (lpData, Xbuffer);
	      GlobalUnlock (hMem);
	      if (!SetClipboardData (CF_TEXT, hMem))
		WinErrorBox (NULL, "");
	      CloseClipboard ();
	    }
	} 
    }
#endif /* _WINDOWS */
   ContentEditing (TEXT_COPY);
}


/*----------------------------------------------------------------------
   TtcPaste
  ----------------------------------------------------------------------*/
void TtcPaste (Document doc, View view)
{
  DisplayMode         dispMode;
  PtrAbstractBox      pAb;
  PtrAbstractBox      draw;
  ViewSelection      *pViewSel;
#ifdef _WINDOWS
  HANDLE              hMem;
  char               *lpData;
  int                 lpDatalength;
#endif /* _WINDOWS */
  PtrDocument         pDoc;
  PtrElement          firstEl, lastEl;
  int                 firstChar, lastChar;
  int                 frame;
  ThotBool            lock = TRUE;

  if (doc != 0)
    {
      frame = GetWindowNumber (doc, view);
      CloseTextInsertion ();
      if (frame != ActiveFrame)
	{
	  if (ActiveFrame > 0 && FrameTable[ActiveFrame].FrDoc != doc)
	    return;
	  else
	    /* use the right frame */
	    ActiveFrame = frame;
	  /* work in the right document
	     frame = ActiveFrame;
	     doc =  FrameTable[ActiveFrame].FrDoc;*/
	}

      pViewSel = &ViewFrameTable[frame - 1].FrSelectionBegin;
      /* start the undo sequence */
      GetCurrentSelection (&pDoc, &firstEl, &lastEl, &firstChar, &lastChar);
      if (pDoc)
	{
	  /* avoid to redisplay step by step */
	  dispMode = TtaGetDisplayMode (doc);
	  if (dispMode == DisplayImmediately)
	    TtaSetDisplayMode (doc, DeferredDisplay);

	  /* start the undo sequence */
	  OpenHistorySequence (pDoc, firstEl, lastEl, firstChar, lastChar);
	  /* lock tables formatting */
	  if (ThotLocalActions[T_islock])
	    {
	      (*ThotLocalActions[T_islock]) (&lock);
	      if (!lock)
		/* table formatting is not loked, lock it now */
		(*ThotLocalActions[T_lock]) ();
	    }
	  
	  if (!StructSelectionMode && !ViewFrameTable[frame - 1].FrSelectOnePosition)
	    {
	      /* Delete the current selection */
	      if (pViewSel->VsBox != NULL)
		{
		  pAb = pViewSel->VsBox->BxAbstractBox;
		  draw = GetParentDraw (pViewSel->VsBox);
		  if (pAb->AbLeafType == LtText || pAb->AbLeafType == LtSymbol)
		    ContentEditing (TEXT_SUP);

		  else if (!draw && (pAb->AbLeafType == LtPicture ||
				     pAb->AbLeafType == LtGraphics))
		    ContentEditing (TEXT_SUP);
		  else if (!draw &&
			   (pAb->AbLeafType != LtCompound || pAb->AbVolume != 0))
		    TtcPreviousChar (doc, view);
		}
	    }
#ifdef _WINDOWS
	  OpenClipboard (FrRef [frame]);
	  if (hMem = GetClipboardData (CF_UNICODETEXT))
	    {
	      wchar_t* lpData = (wchar_t*) GlobalLock (hMem);
	      lpDatalength = wcslen (lpData);
	      if (Xbuffer == NULL /*|| wcscmp (@@@@Xbuffer, lpData)*/) /* MJD: trial-and-error hacking */
		PasteXClipboardW (lpData, lpDatalength);
	      else  
		ContentEditing (TEXT_PASTE);
	      GlobalUnlock (hMem);
	    }
	  /* the CF_TEXT part is probably not necessary, because
	     CF_UNICODETEXT is supported from Win9x up, by the system,
	     even for non-Unicode applications */
	  else if (hMem = GetClipboardData (CF_TEXT))
	    {
	      lpData = GlobalLock (hMem);
	      lpDatalength = strlen (lpData);
	      if (Xbuffer == NULL || strcmp (Xbuffer, lpData))
		PasteXClipboard (lpData, lpDatalength);
	      else  
		ContentEditing (TEXT_PASTE);
	      GlobalUnlock (hMem);
	    }
	  else 
	    ContentEditing (TEXT_PASTE);
	  CloseClipboard ();
#else /* _WINDOWS */
	  ContentEditing (TEXT_PASTE);
#endif /* _WINDOWS */
	  if (!lock)
	    /* unlock table formatting */
	    (*ThotLocalActions[T_unlock]) ();

	  /* close the undo sequence */
	  CloseHistorySequence (pDoc);
	  if (dispMode == DisplayImmediately)
	    TtaSetDisplayMode (doc, dispMode);
	}
    }
}

/*---------------------------------------------------------------------
   EditingLoadResources connecte les fonctions d'edition              
  ----------------------------------------------------------------------*/
void                EditingLoadResources ()
{
   if (ThotLocalActions[T_editfunc] == NULL)
     {
	/* Connecte les actions d'edition */
	TteConnectAction (T_updateparagraph, (Proc) CloseParagraphInsertion);
	TteConnectAction (T_stopinsert, (Proc) CloseTextInsertion);
	TteConnectAction (T_pasteclipboard, (Proc) PasteXClipboard);
	TteConnectAction (T_editfunc, (Proc) ContentEditing);
	TteConnectAction (T_insertchar, (Proc) InsertChar);
	TteConnectAction (T_AIupdate, (Proc) AbstractImageUpdated);
	TteConnectAction (T_redisplay, (Proc) RedisplayDocViews);
	TteConnectAction (T_freesavedel, (Proc) FreeSavedElements);
	TteConnectAction (T_clearhistory, (Proc) ClearHistory);
	TteConnectAction (T_openhistory, (Proc) OpenHistorySequence);
	TteConnectAction (T_addhistory, (Proc) AddEditOpInHistory);
	TteConnectAction (T_attraddhistory, (Proc) AddAttrEditOpInHistory);
	TteConnectAction (T_cancelhistory, (Proc) CancelLastEditFromHistory);
	TteConnectAction (T_closehistory, (Proc) CloseHistorySequence);

	MenuActionList[0].Call_Action = (Proc) TtcInsertChar;
	MenuActionList[CMD_DeletePrevChar].Call_Action = (Proc) TtcDeletePreviousChar;
	MenuActionList[CMD_DeleteSelection].Call_Action = (Proc) TtcDeleteSelection;

	MenuActionList[CMD_PasteFromClipboard].Call_Action = (Proc) TtcPasteFromClipboard;
	LastInsertCell = NULL;
	LastInsertParagraph = NULL;
	LastInsertElText = NULL;
	LastInsertAttr = NULL;
	LastInsertAttrElem = NULL;
	FromKeyboard = FALSE;
     }
}
