/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
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

/* paragraphe to be reformatted after insertion */
static PtrAbstractBox LastInsertParagraph;
static PtrAbstractBox LastInsertCell;

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
#include "displayview_f.h"
#include "editcommands_f.h"
#include "font_f.h"
#include "frame_f.h"
#include "geom_f.h"
#include "language_f.h"
#include "memory_f.h"
#include "picture_f.h"
#include "scroll_f.h"
#include "structcommands_f.h"
#include "structcreation_f.h"
#include "structmodif_f.h"
#include "structschema_f.h"
#include "structselect_f.h"
#include "textcommands_f.h"
#include "tree_f.h"
#include "units_f.h"
#include "undo_f.h"
#include "unstructchange_f.h"
#include "viewapi_f.h"
#include "views_f.h"
#include "windowdisplay_f.h"

#ifdef _WINDOWS 
#include "wininclude.h"
#endif /* _WINDOWS */

#ifdef _WINDOWS
#ifdef __STDC__
static BOOL sameString (USTRING str1, USTRING str2)
#else  /* __STDC__ */
static BOOL sameString (str1, str2)
USTRING str1;
USTRING str2;
#endif /* __STDC__ */
{
   int i;

   int l1 = ustrlen (str1);
   int l2 = ustrlen (str2);
   if (l1 != l2)
      return FALSE;
   for (i = 0; i < l1; i++)
       if ((unsigned int) str1[i] != (unsigned int) str2 [i])
          return FALSE ;
   return TRUE;
}
#endif /* _WINDOWS */

/*----------------------------------------------------------------------
   CopyString computes the width of the source text and copies it into the
   target buffer if target parameter is not NULL.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         CopyString (Buffer source, Buffer target, int count, ptrfont font, int *sourceInd, int *targetInd, int *width, int *nSpaces, int *nChars)
#else  /* __STDC__ */
static void         CopyString (source, target, count, font, sourceInd, targetInd, width, nSpaces, nChars)
PtrTextBuffer       source;
PtrTextBuffer       target;
int                 count;
ptrfont             font;
int                *sourceInd;
int                *targetInd;
int                *width;
int                *nSpaces;
int                *nChars;

#endif /* __STDC__ */
{
   int                 nb;
   CHAR_T              car;

   nb = 0;
   while (nb < count)
     {
	car = source[*sourceInd - 1];
	if (target != NULL)
	   /* copy needed */
	   target[*targetInd - 1] = car;
	if (car == EOS)
	   /* end of string: return the real length */
	   nb = count + 1;
	else
	  {
	     (*sourceInd)++;
	     (*targetInd)++;
	     (nb)++;
	     (*nChars)++;
	     if (car == SPACE)
            (*nSpaces)++;
	     *width += CharacterWidth ((UCHAR_T) car, font);
	  }
     }
}

/*----------------------------------------------------------------------
   RegisterInHistory
   register element pEl in the editing history, to allow UNDO
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void     RegisterInHistory (PtrElement pEl, int frame,
				   int firstCharIndex, int lastCharIndex)
#else  /* __STDC__ */
static void     RegisterInHistory (pEl, frame, firstCharIndex, lastCharIndex)
PtrElement      pEl;
int             frame;
int		firstCharIndex;
int		lastCharIndex

#endif /* __STDC__ */
{
   PtrDocument         pDoc;
   int                 view;
   ThotBool            assoc;

   GetDocAndView (frame, &pDoc, &view, &assoc);
   OpenHistorySequence (pDoc, pEl, pEl, firstCharIndex, lastCharIndex);
   AddEditOpInHistory (pEl, pDoc, TRUE, TRUE);
   CloseHistorySequence (pDoc);
}

/*----------------------------------------------------------------------
   APPtextModify envoie un message qui notifie qu'un texte est     
   modifie'.                                               
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static ThotBool     APPtextModify (PtrElement pEl, int frame, ThotBool pre)
#else  /* __STDC__ */
static              APPtextModify (pEl, frame, pre)
PtrElement          pEl;
int                 frame;
ThotBool            pre;
#endif /* __STDC__ */
{
   PtrElement          pParentEl;
   ThotBool            result;
   NotifyOnTarget      notifyEl;
   PtrDocument         pDoc;
   int                 view;
   ThotBool            assoc;
   ThotBool            ok;

   GetDocAndView (frame, &pDoc, &view, &assoc);
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
#ifdef __STDC__
static ThotBool     APPattrModify (PtrAttribute pAttr, PtrElement pEl, int frame, ThotBool pre)
#else  /* __STDC__ */
static ThotBool     APPattrModify (pAttr, pEl, frame, pre)
PtrAttribute        pAttr;
PtrElement          pEl;
int                 frame;
ThotBool            pre;
#endif /* __STDC__ */
{
   ThotBool            result;
   PtrDocument         pDoc;
   int                 view;
   ThotBool            assoc;
   NotifyAttribute     notifyAttr;

   GetDocAndView (frame, &pDoc, &view, &assoc);
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
#ifdef __STDC__
static void         GiveInsertPoint (PtrAbstractBox pAb, int frame, PtrBox * pBox, PtrTextBuffer * pBuffer, int *ind, int *x, int *previousChars)
#else  /* __STDC__ */
static void         GiveInsertPoint (pAb, frame, pBox, pBuffer, ind, x, previousChars)
PtrAbstractBox      pAb;
PtrBox             *pBox;
PtrTextBuffer      *pBuffer;
int                 frame;
int                *ind;
int                *x;
int                *previousChars;
#endif /* __STDC__ */
{
   ViewSelection      *pViewSel;
   ThotBool            OK;
   ThotBool            endOfPicture;

   /* Si le pave n'est pas identifie on prend */
   /* le pave de la premiere boite selectionnee  */
   pViewSel = &ViewFrameTable[frame - 1].FrSelectionBegin;
   endOfPicture = FALSE;
   if (pAb == NULL && pViewSel->VsBox != NULL)
     {
       pAb = pViewSel->VsBox->BxAbstractBox;
     }

   /* S'il n'y a pas de pave selectionne */
   if (pAb != NULL)
     if (pAb->AbLeafType == LtPicture && pViewSel->VsIndBox == 1)
       endOfPicture =TRUE;
      /* Tant que le pave est un pave de presentation on saute au pave suivant */
      /* ne saute pas les paves de presentation modifiables, i.e. les paves */
      /* qui affichent la valeur d'un attribut */
      do
	{
	   if (pAb != NULL)
	      OK = ((pAb->AbPresentationBox && !pAb->AbCanBeModified)
		    || (pAb->AbLeafType == LtPicture && endOfPicture));
	   else
	      OK = FALSE;
	   if (OK)
	      pAb = pAb->AbNext;
	}
      while (OK);

   if (pAb == NULL)
     {
	*pBox = NULL;
	*pBuffer = NULL;
	*ind = 1;
	*x = 0;
	*previousChars = 0;
	return;
     }
   else if (pAb->AbLeafType == LtText)
     {
	if (pViewSel->VsBox != 0 && pViewSel->VsBox->BxAbstractBox == pAb)
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
	     *ind = 1;
	     *x = 0;
	     *previousChars = 0;
	  }
     }
   else
     {
	*pBox = pAb->AbBox;
	*pBuffer = NULL;
	*ind = 1;
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
   PtrAttribute        pAttr;
   PtrElement          pEl;
   PtrBox              pBox;
   PtrBox              pSelBox;
   PtrTextBuffer       pBuffer;
   PtrTextBuffer       pbuff;
   ViewFrame          *pFrame;
   ViewSelection      *pViewSel;
   ViewSelection      *pViewSelEnd;
#  ifndef _WINDOWS
   ThotEvent              event;
#  endif /* !_WINDOWS */
   int                 nChars;
   int                 i, j;
   int                 ind;
   int                 frame;
   ThotBool            notified;

   /* No more enclosing cell */
   LastInsertCell = NULL;
   /* recupere la fenetre active pour la selection */
   frame = ActiveFrame;
   notified = FALSE;
   if (frame > 0)
     {
	if (TextInserting)
	  {
	     /* termine l'insertion courante */
	     TextInserting = FALSE;

	     /* Recherche le point d'insertion (&i non utilise) */
	     GiveInsertPoint (NULL, frame, &pSelBox, &pBuffer, &ind, &i, &j);
	     if (pSelBox == NULL)
	       /* plus de selection courante */
	       return (notified);
	     /* Est-ce que le buffer d'insertion est vide ? */
	     if ((pBuffer != NULL) && (ind > 1 || ind > pBuffer->BuLength))
	       {
		  if (pBuffer->BuLength == 0 && pSelBox->BxBuffer != pBuffer)
		     pBuffer = DeleteBuffer (pBuffer, frame);
	       }
	     /* regroupe eventuellement les buffers */
	     else if ((pBuffer != NULL) && (pBuffer->BuPrevious != NULL))
	       {
		  pbuff = pBuffer->BuPrevious;
		  i = FULL_BUFFER - pbuff->BuLength;
		  nChars = pBuffer->BuLength;
		  if (pbuff->BuLength == 0 && pSelBox->BxNChars != 0)
		     pbuff = DeleteBuffer (pbuff, frame);
		  else if (nChars < 50 && i >= nChars)
		    {
		       ustrncpy (&pbuff->BuContent[pbuff->BuLength], &pBuffer->BuContent[0], nChars);
		       i = pbuff->BuLength;	/* Ancienne longueur */
		       /* met a jour les indices de debut des boites de coupure */
		       if (j == 0)
			  pBox = pSelBox;
		       /* La boite courante est concernee */
		       else
			  pBox = pSelBox->BxNexChild;

		       /* commence a partir de la suivante */
		       while (pBox != NULL)
			 {
			    if (pBox->BxBuffer == pBuffer)
			      {
				 pBox->BxBuffer = pbuff;
				 pBox->BxFirstChar += i;
				 pBox = pBox->BxNexChild;
			      }
			    else if (pBox->BxBuffer == pBuffer)
			       pBox = pBox->BxNexChild;

			    else
			       pBox = NULL;
			 }

		       /* Liberation du buffer */
		       pBuffer = DeleteBuffer (pBuffer, frame);
		       pbuff->BuLength += nChars;
		       /* fin de chaine */
		       pbuff->BuContent[pbuff->BuLength] = EOS;
		    }
	       }

	     /* Est-ce que l'insertion se trouve en debut de boite ? */
	     pBox = pSelBox->BxAbstractBox->AbBox;
	     if (j == 0)
	       {
		  /* Insertion en fin de buffer annulee ? */
		  if ((pSelBox->BxBuffer->BuLength != 0) &&
		      (pSelBox->BxFirstChar > pSelBox->BxBuffer->BuLength && pSelBox->BxNChars > 0))
		     pSelBox->BxFirstChar = 1;
		  /* Faut-il mettre a jour la boite mere ? */
		  if (pBox->BxBuffer != pSelBox->BxBuffer && pBox->BxNexChild == pSelBox)
		     pBox->BxBuffer = pBuffer;
		  else if (ind == 1 && pBox->BxBuffer == pSelBox->BxBuffer)
		     pBox->BxBuffer = pBuffer;
		  pSelBox->BxBuffer = pBuffer;
	       }

	     /* met a jour le pave */
	     pSelBox->BxAbstractBox->AbText = pBox->BxBuffer;

	     /* Quand le texte insere' se trouve dans un bloc de lignes */
	     /* on reformate le bloc de ligne pour retirer les          */
	     /* compressions de texte et couper eventuellement les mots */
	     while (pBox != NULL)
		if (pBox->BxAbstractBox == NULL)
		   pBox = NULL;
		else if (pBox->BxAbstractBox->AbEnclosing != NULL)
		   if (pBox->BxAbstractBox->AbEnclosing->AbBox != NULL)
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

	     NewContent (pSelBox->BxAbstractBox);
	     /* signale la nouvelle selection courante */
	     pFrame = &ViewFrameTable[frame - 1];
	     pViewSel = &pFrame->FrSelectionBegin;
	     pViewSelEnd = &pFrame->FrSelectionEnd;
	     if (pViewSel->VsBox != NULL)
	       {
		  i = pViewSel->VsBox->BxIndChar + pViewSel->VsIndBox;
		  if (pViewSel->VsIndBuf > 0)
		     i++;

		  /* Faut-il changer l'autre extremite de la selection ? */
		  pBox = pViewSelEnd->VsBox;
		  if (pBox != NULL)
		     if (pBox->BxAbstractBox == pViewSel->VsBox->BxAbstractBox)
		       {
			  j = pBox->BxIndChar + pViewSelEnd->VsIndBox;
			  if (pViewSelEnd->VsIndBuf > 0)
			     j++;
			  ChangeSelection (frame, pViewSel->VsBox->BxAbstractBox, i, FALSE, TRUE, FALSE, FALSE);
			  if (pViewSel->VsBox->BxAbstractBox != pBox->BxAbstractBox || i != j)
			     ChangeSelection (frame, pBox->BxAbstractBox, j, TRUE, TRUE, FALSE, FALSE);
		       }
		     else
			ChangeSelection (frame, pViewSel->VsBox->BxAbstractBox, i, FALSE, TRUE, FALSE, FALSE);
		  else
		     ChangeSelection (frame, pViewSel->VsBox->BxAbstractBox, i, FALSE, TRUE, FALSE, FALSE);
		  /* Nouvelle position de reference du curseur */
		  ClickX = pViewSel->VsBox->BxXOrg + pViewSel->VsXPos - pFrame->FrXOrg;
	       }

	     if (LastInsertElText != NULL)
	       {
		 /* Notify the end of text insertion */
		 pEl = LastInsertElText;
		 LastInsertElText = NULL;
		 APPtextModify (pEl, frame, FALSE);
		 notified = TRUE;
	       }
	     else if (LastInsertAttr != NULL)
	       {
		 /* Notify the end of attribute change */
		 pAttr = LastInsertAttr;
		 pEl = LastInsertAttrElem;
		 LastInsertAttr = NULL;
		 LastInsertAttrElem = NULL;
		 APPattrModify (pAttr, pEl, frame, FALSE);
		 notified = TRUE;
	       }
	  }
     }

   /* elimine systematiquement les exposes en attente */
#ifndef _WINDOWS
#ifndef _GTK
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
#ifdef __STDC__
static void         SetInsert (PtrAbstractBox *pAb, int *frame, LeafType nat, ThotBool del)
#else  /* __STDC__ */
static void         SetInsert (pAb, frame, nat, del)
PtrAbstractBox     *pAb;
int                *frame;
LeafType            nat;
ThotBool            del;

#endif /* __STDC__ */
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
      if (pViewSel->VsBox == NULL || pViewSel->VsIndBuf == 0)
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
      if (pSelAb != NULL)
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
		    !(pSelAb->AbLeafType == LtSymbol && nat == LtText)))
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
	  if (pSelAb != NULL && pSelAb->AbElement != NULL && moveSelection)
	    /* signale le changement de selection a l'editeur */
	    if (pSelAb->AbVolume == 0)
	      ChangeSelection (*frame, pSelAb, 0, FALSE, TRUE, FALSE, FALSE);
	    else
	      ChangeSelection (*frame, pSelAb, i, FALSE, TRUE, FALSE, FALSE);
	}
    }
}


/*----------------------------------------------------------------------
   Cree un buffer apres celui donne en parametre.                  
   Met a jour les chainages et rend le pointeur sur le nouveau     
   buffer.                                                         
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static PtrTextBuffer GetNewBuffer (PtrTextBuffer pBuffer, int frame)
#else  /* __STDC__ */
static PtrTextBuffer GetNewBuffer (pBuffer, frame)
PtrTextBuffer       pBuffer;
int                 frame;

#endif /* __STDC__ */
{
   PtrTextBuffer       pNewBuffer;
   PtrTextBuffer       pCurrentBuffer;
   ViewFrame          *pFrame;
   ViewSelection      *pViewSel;
   ViewSelection      *pViewSelEnd;

   GetTextBuffer (&pNewBuffer);
   pCurrentBuffer = pNewBuffer;
   pCurrentBuffer->BuPrevious = pBuffer;
   if (pBuffer == NULL)
      pCurrentBuffer->BuNext = NULL;
   else
     {
	pCurrentBuffer->BuNext = pBuffer->BuNext;
	pBuffer->BuNext = pNewBuffer;
	if (pCurrentBuffer->BuNext != NULL)
	   pCurrentBuffer->BuNext->BuPrevious = pNewBuffer;
     }

   /* Mise a jour des marques de selection courante */
   pFrame = &ViewFrameTable[frame - 1];
   pViewSel = &pFrame->FrSelectionBegin;
   if (pViewSel->VsBuffer == pBuffer)
     {
	if (pFrame->FrSelectionEnd.VsBuffer == pViewSel->VsBuffer)
	  {
	     pViewSelEnd = &pFrame->FrSelectionEnd;
	     pViewSelEnd->VsBuffer = pNewBuffer;
	     pViewSelEnd->VsIndBuf = pViewSelEnd->VsIndBuf - pFrame->FrSelectionBegin.VsIndBuf + 1;
	  }
	pViewSel->VsBuffer = pNewBuffer;
	pViewSel->VsIndBuf = 1;
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
#ifdef __STDC__
static void         CopyBuffers (ptrfont font, int frame, int startInd, int endInd,
				 int targetInd, PtrTextBuffer pSourceBuffer,
				 PtrTextBuffer pEndBuffer,
				 PtrTextBuffer * pTargetBuffer,
				 int *width, int *nSpaces, int *nChars)
#else  /* __STDC__ */
static void         CopyBuffers (font, frame, startInd, endInd, targetInd, pSourceBuffer, pEndBuffer, pTargetBuffer, width, nSpaces, nChars)
ptrfont             font;
int                 frame;
int                 startInd;
int                 endInd;
int                 targetInd;
PtrTextBuffer       pSourceBuffer;
PtrTextBuffer       pEndBuffer;
PtrTextBuffer      *pTargetBuffer;
int                *width;
int                *nSpaces;
int                *nChars;

#endif /* __STDC__ */
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
   while (pBuffer != NULL)
     {
	if (*pTargetBuffer == NULL)
	  {
	     /* we don't want to effectively copy the text */
	     targetInd = 1;
	     target = NULL;
	  }
	else
	   target = (*pTargetBuffer)->BuContent;

	if (pBuffer == pEndBuffer)
	  {
	     /* prevent buffer overflow */
	     if (endInd > pEndBuffer->BuLength)
		sourceLength = pEndBuffer->BuLength - sourceInd + 1;
	     else
		sourceLength = endInd - sourceInd + 1;
	  }
	else
	   sourceLength = pBuffer->BuLength - sourceInd + 1;
	targetlength = THOT_MAX_CHAR - targetInd;
	if (sourceLength <= targetlength)
	  {
	    /* end of source buffer */
	    CopyString (pBuffer->BuContent, target, sourceLength, font, &sourceInd, &targetInd, width, nSpaces, nChars);
	    if (pBuffer == pEndBuffer)
	      pBuffer = NULL;
	    else
	      pBuffer = pBuffer->BuNext;
	    sourceInd = 1;
	  }
	else
	  {
	    /* end of target buffer */
	    CopyString (pBuffer->BuContent, target, targetlength, font, &sourceInd, &targetInd, width, nSpaces, nChars);
	    (*pTargetBuffer)->BuLength = FULL_BUFFER;
	    (*pTargetBuffer)->BuContent[THOT_MAX_CHAR - 1] = EOS;
	    *pTargetBuffer = GetNewBuffer (*pTargetBuffer, frame);
	    targetInd = 1;
	  }
     }

   if (*pTargetBuffer != NULL)
     {
	/* text was copied: update target buffer information */
	(*pTargetBuffer)->BuLength = targetInd - 1;
	(*pTargetBuffer)->BuContent[targetInd - 1] = EOS;
     }
}

/*----------------------------------------------------------------------
   Debute l'insertion de caracteres dans une boite de texte.      
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         StartTextInsertion (PtrAbstractBox pAb, int frame, PtrBox pSelBox, PtrTextBuffer pBuffer, int ind, int prev)
#else  /* __STDC__ */
static void         StartTextInsertion (pAb, frame, pSelBox, pBuffer, ind, prev)
PtrAbstractBox      pAb;
int                 frame;
PtrBox              pSelBox;
PtrTextBuffer       pBuffer;
int                 ind;
int                 prev;
#endif /* __STDC__ */
{
   PtrBox              pBox;
   PtrTextBuffer       pPreviousBuffer;
   PtrTextBuffer       pNewBuffer;
   int                 k, i;

   /* recupere la fenetre active pour la selection */
   if (frame == 0)
     frame = ActiveFrame;
   if (frame > 0)
     {
	/* Recherche le point d'insertion (&i non utilise) */
        if (pAb == NULL)
	   GiveInsertPoint (NULL, frame, &pSelBox, &pBuffer, &ind, &i, &prev);
	TextInserting = TRUE;
	/* boite entiere */
	pBox = pSelBox->BxAbstractBox->AbBox;

	/* Note que le texte de l'element va changer */
	if (pSelBox->BxAbstractBox->AbPresentationBox &&
	    pSelBox->BxAbstractBox->AbCanBeModified)
	   /* c'est un pave de presentation affichant la valeur d'un attribut */
	  {
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
	    /* c'est un element feuille */
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
	/* PcFirst buffer de texte du pave */
	pNewBuffer = pSelBox->BxAbstractBox->AbText;

	/* Insertion en debut de boite */
	if (ind == 1 && prev == 0 && pBuffer->BuLength > 0)
	  {
	     /* buffer precedent */
	     pPreviousBuffer = pBuffer->BuPrevious;
	     /* Faut-il ajouter un buffer en debut de boite ? */
	     if (pNewBuffer == pBuffer)
	       {
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
		  /* Mise a jour de la boite entiere et du pave */
		  pSelBox->BxAbstractBox->AbText = pNewBuffer;
		  pBox->BxBuffer = pNewBuffer;
		  /* Mise a jour de la boite */
		  /* Index dans le buffer */
		  pSelBox->BxFirstChar = 1;
		  pSelBox->BxBuffer = pNewBuffer;
	       }
	     else
		/* a la suite du buffer precedent */
	       {
		  /* Mise a jour de la boite */
		  pSelBox->BxFirstChar = pPreviousBuffer->BuLength + 1;
		  pSelBox->BxBuffer = pPreviousBuffer;
	       }
	  }
	/* coupe le buffer courant en deux pour traiter les insertions */
	else if (ind > 1)
	   if (ind <= pBuffer->BuLength)
	     {
		pNewBuffer = pBuffer;
		i = ind;
		pBuffer = GetNewBuffer (pBuffer, frame);
		k = ustrlen (&pNewBuffer->BuContent[i - 1]);
		/* longueur a copier */
		ustrncpy (&pBuffer->BuContent[0], &pNewBuffer->BuContent[i - 1], k);
		pBuffer->BuContent[k] = EOS;
		pBuffer->BuLength = k;
		pNewBuffer->BuContent[i - 1] = EOS;
		i--;
		pNewBuffer->BuLength = i;

		/* Mise a jour des boites de coupure suivantes */
		pBox = pSelBox->BxNexChild;
		while (pBox != NULL)
		  {
		     if (pBox->BxBuffer == pNewBuffer)
		       {
			  pBox->BxBuffer = pBuffer;
			  pBox->BxFirstChar -= i;
			  pBox = pBox->BxNexChild;
		       }
		     else
			pBox = NULL;
		  }
	     }
     }
}


/*----------------------------------------------------------------------
   Create a new element or move to previous element if the language changes
   Return TRUE if there is a notification to the application and the
   current selection could be modified.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static ThotBool     GiveAbsBoxForLanguage (int frame, PtrAbstractBox * pAb, int keyboard)
#else  /* __STDC__ */
static ThotBool    GiveAbsBoxForLanguage (frame, pAb, keyboard)
int                 frame;
PtrAbstractBox     *pAb;
int                 keyboard;

#endif /* __STDC__ */
{
  PtrAbstractBox      pSelAb;
  PtrBox              pBox;
  PtrAttribute        pHeritAttr;
  PtrElement          pElAttr;  
  ViewSelection      *pViewSel;
  Language            language, plang;
  int                 index;
  ThotBool            cut;
  ThotBool            notification;
  ThotBool	      setAttribute = TRUE;

  language = 0;
  pViewSel = &ViewFrameTable[frame - 1].FrSelectionBegin;
  notification = FALSE;
  pSelAb = *pAb;
  plang = pSelAb->AbLanguage;
  if (keyboard == -1)
    if (plang < TtaGetFirstUserLanguage())
      /* le contenu du pave a ete saisi par palette */
      /* et on a saisi au clavier : recherche la langue dans les ancetres */
      {
	pHeritAttr = GetTypedAttrAncestor (pSelAb->AbElement->ElParent, 1, NULL, &pElAttr); 
	if (pHeritAttr != NULL)
	  if (pHeritAttr->AeAttrText != NULL)
	    {
	      language = TtaGetLanguageIdFromName (pHeritAttr->AeAttrText->BuContent);
	      setAttribute = FALSE;
	    }
      }
    else
      language = plang;
  else if (keyboard == 2)
    /* une langue latine saisie */
    if (TtaGetAlphabet (plang) == TEXT('L'))
      language = plang;
    else
      language = TtaGetLanguageIdFromAlphabet (TEXT('L'));
  else if (keyboard == 3)
    /* une langue greque saisie */
    if (TtaGetAlphabet (plang) == TEXT('G'))
      language = plang;
    else
      language = TtaGetLanguageIdFromAlphabet (TEXT('G'));
  else
    language = 0;

  if (pSelAb->AbLeafType == LtText)
    if (plang != language && plang != 0)
      {
	notification = CloseTextInsertionWithControl ();
	if (!notification)
	  {
	    /* selection could not be modified by the application */
	    cut = TRUE;
	    pBox = pViewSel->VsBox;
	    if (pBox != NULL)
	      {
		index = pBox->BxIndChar + pViewSel->VsIndBox + 1;
		if (index <= 1)
		  {
		    pSelAb = pSelAb->AbPrevious;
		    if (pSelAb != NULL)
		      if (pSelAb->AbLeafType == LtText &&
			  pSelAb->AbLanguage == language &&
			  pSelAb->AbCanBeModified && !pSelAb->AbReadOnly)
			{
			  cut = FALSE;
			  ChangeSelection (frame, pSelAb, pSelAb->AbVolume + 1,
					   FALSE, TRUE, FALSE, FALSE);
			}
		  }
		
		/* S'il faut couper, on appelle l'editeur */
		if (cut)
		  NewTextLanguage (*pAb, index, language, setAttribute);
		/* la boite peut avoir change */
		pBox = pViewSel->VsBox;
		if (pBox != NULL)
		  *pAb = pBox->BxAbstractBox;
		else
		  *pAb = NULL;
	      }
	  }
      }
  return (notification);
}


/*----------------------------------------------------------------------
   Teste s'il y a un caracte`re de coupure de ligne dans le texte     
   donne' en parame`tre (premier buffer, indice de de'but, longueur). 
   Si le parame`tre charWidth est nul, regarde tous les buffers.      
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static ThotBool     IsLineBreakInside (PtrTextBuffer pBuffer, int ind, int charWidth)
#else  /* __STDC__ */
static ThotBool     IsLineBreakInside (pBuffer, ind, charWidth)
PtrTextBuffer       pBuffer;
int                 ind;
int                 charWidth;

#endif /* __STDC__ */
{
   int                 c;
   int                 nChars;
   int                 i;
   int                 j;

   /* Si l'index debute en fin de buffer on passe au buffer suivant */
   if (ind > pBuffer->BuLength && pBuffer->BuNext != NULL)
     {
	pBuffer = pBuffer->BuNext;
	i = 0;
     }
   else
      i = ind - 1;
   /* Nombre de caracteres dans le buffer */
   nChars = pBuffer->BuLength - 1;
   j = 0;

   /* Recherche s'il y a un caractere de coupure forcee */
   while (i <= nChars && j < charWidth)
     {
	c = pBuffer->BuContent[i];
	/* Est-ce un caractere de coupure forcee ? */
	if (c == BREAK_LINE || c == NEW_LINE)
	   return (TRUE);
	/* Est-ce qu'il faut passer au buffer suivant ? */
	else if (i == nChars && pBuffer->BuNext != NULL)
	  {
	     pBuffer = pBuffer->BuNext;
	     i = 0;
	     nChars = pBuffer->BuLength - 1;
	  }
	/* Sinon on continue la recherche */
	else
	   i++;

	/* Est-ce que la longueur de la chaine est limitee */
	if (charWidth != 0)
	   j++;
     }
   return (FALSE);
}


/*----------------------------------------------------------------------
   RedisplayOneChar affiche ou efface le caractere car dans la     
   fenetre courante frame.                                 
   La positon x,y correspond a la position du caractere    
   dans l'image concrete affichee et non la position       
   dans la fenetre.                                        
   La fonction op indique s'il s'agit d'une boite          
   active (1) ou non (0).                                  
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         RedisplayOneChar (int frame, int x, int y, CHAR_T c, ptrfont font, PtrBox pBox)
#else  /* __STDC__ */
static void         RedisplayOneChar (frame, x, y, c, font, pBox)
int                 frame;
int                 x;
int                 y;
CHAR_T                c;
ptrfont             font;
PtrBox              pBox;

#endif /* __STDC__ */
{
   ViewFrame          *pFrame;
   int                 xd, yd;
   int                 readOnlyStatus;
   int                 linkStatus;
   int                 fg;

   pFrame = &ViewFrameTable[frame - 1];
   xd = x - pFrame->FrXOrg;
   yd = y - pFrame->FrYOrg;

   fg = pBox->BxAbstractBox->AbForeground;
   linkStatus = (int) pBox->BxAbstractBox->AbSensitive;
   readOnlyStatus = (int) pBox->BxAbstractBox->AbReadOnly;
   if (pBox->BxAbstractBox->AbBox->BxShadow)
     DrawChar ('*', frame, xd, yd, font, readOnlyStatus, linkStatus, fg);
   else
     DrawChar (c, frame, xd, yd, font, readOnlyStatus, linkStatus, fg);
   DisplayUnderline (frame, xd + CharacterWidth (c, font), yd, font, pBox->BxUnderline, pBox->BxThickness, CharacterWidth (c, font), readOnlyStatus, linkStatus, fg);
   /* Affichage effectue */
   DefClip (frame, 0, 0, 0, 0);
}


/*----------------------------------------------------------------------
   TtcInsertGraph insert a graphics                                
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtcInsertGraph (Document document, View view, CHAR_T c)
#else  /* __STDC__ */
void                TtcInsertGraph (document, view, c)
Document            document;
View                view;
CHAR_T                c;

#endif /* __STDC__ */
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
#ifdef __STDC__
void                CloseParagraphInsertion (PtrAbstractBox pAb, int frame)
#else  /* __STDC__ */
void                CloseParagraphInsertion (pAb, frame)
PtrAbstractBox      pAb;
int                 frame;

#endif /* __STDC__ */
{
   PtrBox              pBox;

   if (LastInsertParagraph != NULL)
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
	   if (ThotLocalActions[T_switchsel])
	     (*ThotLocalActions[T_switchsel]) (LastInsertThotWindow, FALSE);
	   RedrawFrameBottom (LastInsertThotWindow, 0);
	   if (ThotLocalActions[T_switchsel])
	     (*ThotLocalActions[T_switchsel]) (LastInsertThotWindow, TRUE);
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
		      if (pBox->BxType == BoGhost)
			 pAb = pAb->AbEnclosing;
		      else
			{
			   /* Reevalue le bloc de lignes */
			   RecomputeLines (LastInsertParagraph, NULL, NULL, LastInsertThotWindow);
			   /* Et l'affiche */
			   RedrawFrameBottom (LastInsertThotWindow, 0);
			   LastInsertParagraph = NULL;
			   pAb = NULL;
			}
		}
	}
}

/*----------------------------------------------------------------------
   insere dans la boite pBox.                                      
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         LoadSymbol (char c, PtrLine pLine, ThotBool defaultHeight, ThotBool defaultWidth, PtrBox pBox, PtrAbstractBox pAb, int frame)
#else  /* __STDC__ */
static void         LoadSymbol (c, pLine, defaultHeight, defaultWidth, pBox, pAb, frame)
char                c;
PtrLine             pLine;
ThotBool            defaultHeight;
ThotBool            defaultWidth;
PtrBox              pBox;
PtrAbstractBox      pAb;
int                 frame;

#endif /* __STDC__ */
{
   int                 xDelta, yDelta;

   if (!APPgraphicModify (pBox->BxAbstractBox->AbElement, (int) c, frame, TRUE))
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
	APPgraphicModify (pBox->BxAbstractBox->AbElement, (int) c, frame, FALSE);
     }
}


/*----------------------------------------------------------------------
   Charge un graphique ou une polyline.                            
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         LoadShape (char c, PtrLine pLine, ThotBool defaultHeight, ThotBool defaultWidth, PtrBox pBox, PtrAbstractBox pAb, int frame)
#else  /* __STDC__ */
static void         LoadShape (c, pLine, defaultHeight, defaultWidth, pBox, pAb, frame)
char                c;
PtrLine             pLine;
ThotBool            defaultHeight;
ThotBool            defaultWidth;
PtrBox              pBox;
PtrAbstractBox      pAb;
int                 frame;

#endif /* __STDC__ */
{
  PtrAbstractBox      draw;
  PtrTextBuffer       pBuffer;
  int                 xDelta, yDelta;
  int                 width, height;
  int                 x, y;

  if (!APPgraphicModify (pBox->BxAbstractBox->AbElement, (int) c, frame, TRUE))
    {
      if (ThotLocalActions[T_switchsel])
	(*ThotLocalActions[T_switchsel]) (frame, FALSE);
      /* efface la selection precedente */
      switch (c)
	{
	case 'S':	/* Segments */
	case 'U':	/* Segments vers avant */
	case 'N':	/* Segments fleches vers arriere */
	case 'M':	/* Segments fleches dans les deux sens */
	case 'w':	/* Segments (2 points) */
	case 'x':	/* Segments (2 points) forward arrow */
	case 'y':	/* Segments (2 points) backward arrow */
	case 'z':	/* Segments (2 points) arrows on both directions */
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
	      pAb->AbPolyLineBuffer->BuPoints[0].XCoord = PixelToPoint (width) * 1000;
	      pAb->AbPolyLineBuffer->BuPoints[0].YCoord = PixelToPoint (height) * 1000;
	      GetTextBuffer (&(pBox->BxBuffer));
	      /* initialise la dimension de la boite polyline */
	      pBox->BxBuffer->BuLength = 1;
	      pBox->BxBuffer->BuPoints[0].XCoord = PixelToPoint (width) * 1000;
	      pBox->BxBuffer->BuPoints[0].YCoord = PixelToPoint (height) * 1000;
	      pBox->BxNChars = 1;
	    }
	  
	  if (pBox->BxNChars == 1)
	    {
	      /* il faut saisir les points de la polyline */
	      x = pBox->BxXOrg - ViewFrameTable[frame - 1].FrXOrg;
	      y = pBox->BxYOrg - ViewFrameTable[frame - 1].FrYOrg;
	      draw = GetParentDraw (pBox);
	      if (c == 'w' || c == 'x' || c == 'y' || c == 'z')
		pAb->AbVolume = PolyLineCreation (frame, &x, &y, pBox, draw, 2);
	      else
		pAb->AbVolume = PolyLineCreation (frame, &x, &y, pBox, draw, 0);
	      pBox->BxNChars = pAb->AbVolume;
#ifdef IV
	      if (draw)
		{
		  /* on force le reaffichage de la boite (+ les points de selection) */
		  DefClip (frame, pBox->BxXOrg - EXTRA_GRAPH, pBox->BxYOrg - EXTRA_GRAPH, pBox->BxXOrg + width + EXTRA_GRAPH, pBox->BxYOrg + height + EXTRA_GRAPH);
		  x += ViewFrameTable[frame - 1].FrXOrg;
		  y += ViewFrameTable[frame - 1].FrYOrg;
		if (x != pBox->BxXOrg || y != pBox->BxYOrg)
		  NewPosition (pAb, x, y, frame, TRUE);
		width = PointToPixel (pBox->BxBuffer->BuPoints[0].XCoord / 1000);
		height = PointToPixel (pBox->BxBuffer->BuPoints[0].YCoord / 1000);
		if (width != pBox->BxWidth || height != pBox->BxHeight)
		  NewDimension (pAb, width, height, frame, TRUE);
		}
#endif
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
      /* Reaffiche la selection */
      if (ThotLocalActions[T_switchsel])
	(*ThotLocalActions[T_switchsel]) (frame, FALSE);
      APPgraphicModify (pBox->BxAbstractBox->AbElement, (int) c, frame, FALSE);
    }
}


/*----------------------------------------------------------------------
   insere dans la boite pBox.                                     
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         LoadPictFile (PtrLine pLine, ThotBool defaultHeight, ThotBool defaultWidth, PtrBox pBox, PtrAbstractBox pAb, int frame)
#else  /* __STDC__ */
static void         LoadPictFile (pLine, defaultHeight, defaultWidth, pBox, pAb, frame)
PtrLine             pLine;
ThotBool            defaultHeight;
ThotBool            defaultWidth;
PtrBox              pBox;
PtrAbstractBox      pAb;
int                 frame;

#endif /* __STDC__ */
{
   CHAR_T                buffer[FULL_BUFFER];
   STRING              ptr;
   int                 i;
   int                 xDelta, yDelta;
   int                 type;
   int                 pres;
   PictInfo           *pictInfo;
   ThotBool            ok;

   if (pAb->AbLeafType == LtPicture)
     {
       pictInfo = (PictInfo *) pAb->AbPictInfo;
       if ((pictInfo != NULL) && (pictInfo->PicFileName != NULL))
	 {
	   ustrcpy (buffer, pictInfo->PicFileName);
	   type = pictInfo->PicType;
	   pres = pictInfo->PicPresent;
	   /* Is it an empty picture ? */
	   if (type == -1)
	     {
	       type = GIF_FORMAT;
	       pres = (int) ReScale;
	     }
 	 }
       else
	 {
	   type = GIF_FORMAT;
	   pres = (int) ReScale;
	 }
       /* dans quel document est-on ? */
       /* saisit le nom du fichier image */
       if (!APPtextModify (pAb->AbElement, frame, TRUE))
	 {
	   /* register the editing operation in the history */
	   RegisterInHistory (pAb->AbElement, frame, 0, 0);	     

	   if (ThotLocalActions[T_imagemenu] != NULL)
	     (*ThotLocalActions[T_imagemenu]) (buffer, &ok, &type, &pres, pBox);
	   else
	     ok = FALSE;

	   if (ok)
	     {
	       /* something change */
	       i = ustrlen (buffer);
	       /* longueur de la chaine a copier */
	       if (i != 0)
		 {
		   pictInfo = (PictInfo *) pBox->BxPictInfo;
		   /* free the pictInfo contents, but not the struture */
		   FreePictInfo (pictInfo);
		   ustrcpy (pictInfo->PicFileName, buffer);
		   pictInfo->PicPresent = (PictureScaling) pres;
		   pictInfo->PicType = type;
		   SetCursorWatch (frame);
		   LoadPicture (frame, pBox, pictInfo);
		   ResetCursorWatch (frame);
		   if (pictInfo->PicPixmap != 0)
		     {
		       pAb->AbVolume = ustrlen (buffer);
		       /* met a jour la boite */
		       if (!defaultWidth)
			 xDelta = 0;
		       else
			 /* difference de largeur */
			 xDelta = pictInfo->PicWArea - pBox->BxWidth;
		       if (!defaultHeight)
			 yDelta = 0;
		       else
			 /* difference de largeur */
			 yDelta = pictInfo->PicHArea - pBox->BxHeight;
		       BoxUpdate (pBox, pLine, 0, 0, xDelta, 0, yDelta, frame, FALSE);
		     }
		   else
		     {
		       ptr = buffer;
		       TtaDisplayMessage (INFO, TtaGetMessage (LIB, TMSG_LIB_UNKNOWN_TYPE), &ptr);
		     }
		 }
	       NewContent (pAb);
	       APPtextModify (pAb->AbElement, frame, FALSE);
	     }		/*if BuildPictureMenu */
	   pAb = NULL;	/* rien a faire de plus pour les images */
	 }
     }
   else
     TtaDisplaySimpleMessage (INFO, LIB, TMSG_INSERTING_IMP);
}


/*----------------------------------------------------------------------
   ClearClipboard vide le contenu du Clipboard X ou Thot.          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ClearClipboard (PtrTextBuffer clipboard)
#else  /* __STDC__ */
void                ClearClipboard (clipboard)
PtrTextBuffer       clipboard;

#endif /* __STDC__ */
{
   PtrTextBuffer       pBuffer;

   /* Reinitialise le contenu du Clipboard X ou Thot */
   pBuffer = clipboard;
   if (pBuffer->BuLength != 0)
     {
	pBuffer->BuLength = 0;
	pBuffer->BuContent[0] = EOS;
	pBuffer = pBuffer->BuNext;
	while (pBuffer != NULL)
	  {
	     pBuffer = DeleteBuffer (pBuffer, ActiveFrame);
	     pBuffer = pBuffer->BuNext;
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
#ifdef __STDC__
static void         SaveInClipboard (int *charsDelta, int *spacesDelta, int *xDelta, int ind, PtrTextBuffer pBuffer, PtrAbstractBox pAb, int frame, PtrTextBuffer clipboard)
#else  /* __STDC__ */
static void         SaveInClipboard (charsDelta, spacesDelta, xDelta, ind, pBuffer, pAb, frame, clipboard)
int                *charsDelta;
int                *spacesDelta;
int                *xDelta;
int                 ind;
PtrTextBuffer       pBuffer;
PtrAbstractBox      pAb;
int                 frame;
PtrTextBuffer       clipboard;

#endif /* __STDC__ */
{
   PtrTextBuffer       pTargetBuffer;
   ViewFrame          *pFrame;
   int                 i;
   PictInfo           *pictInfo;

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
		  pictInfo = (PictInfo *) pAb->AbPictInfo;
		  i = ustrlen (pictInfo->PicFileName);
		  /* nom du fichier image */
		  ustrcpy (&(clipboard->BuContent[0]), pictInfo->PicFileName);
		  clipboard->BuLength = i;
		  clipboard->BuContent[i] = EOS;	/* Termine la chaine */
		  CopyPictInfo ((int *) &PictClipboard, (int *) pictInfo);
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

		       ClipboardLanguage = pAb->AbLanguage;
		       /* sauve le texte selectionne dans la feuille */
		       i = 1;	/* Indice de debut */
		       pTargetBuffer = &ClipboardThot;
		       CopyBuffers (pAb->AbBox->BxFont, frame, ind,
				    pFrame->FrSelectionEnd.VsIndBuf, i, pBuffer, pFrame->FrSelectionEnd.VsBuffer,
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
   detruit le contenu de la selection apres execution de la copie.   
   - charsDelta le nombre de caracteres copies                     
   - spacesDelta le nombre de blancs copies                        
   - xDelta la largeur de la chaine copiee                         
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         RemoveSelection (int charsDelta, int spacesDelta, int xDelta, ThotBool defaultHeight, ThotBool defaultWidth, PtrLine pLine, PtrBox pBox, PtrAbstractBox pAb, int frame)
#else  /* __STDC__ */
static void         RemoveSelection (charsDelta, spacesDelta, xDelta, defaultHeight, defaultWidth, pLine, pBox, pAb, frame)
int                 charsDelta;
int                 spacesDelta;
int                 xDelta;
ThotBool            defaultHeight;
ThotBool            defaultWidth;
PtrLine             pLine;
PtrBox              pBox;
PtrAbstractBox      pAb;
int                 frame;

#endif /* __STDC__ */
{
   PtrTextBuffer       pTargetBuffer;
   PtrTextBuffer       pSourceBuffer;
   PictInfo           *pictInfo;
   ViewFrame          *pFrame;
   ViewSelection      *pViewSel;
   ptrfont             font;
   int                 sourceInd, targetInd;
   int                 length;
   int                 i;
   int                 adjust;
   int                 yDelta;
   int                 width, height;

   font = pBox->BxFont;
   width = CharacterWidth ('m', font);
   height = FontHeight (font);
   if (pAb->AbVolume == 0)
     {
	TtaDisplaySimpleMessage (INFO, LIB, TMSG_NOTHING_TO_DEL);
	DefClip (frame, 0, 0, 0, 0);
     }
   else
      switch (pAb->AbLeafType)
	    {
	       case LtText:
		  pFrame = &ViewFrameTable[frame - 1];
		  pViewSel = &pFrame->FrSelectionBegin;

		  /* Note que le texte de l'e'le'ment va changer */
		  StartTextInsertion (NULL, frame, NULL, NULL, 0, 0);
		  /* fusionne le premier et dernier buffer de la selection */
		  /* premier buffer=destination */
		  pTargetBuffer = pViewSel->VsBuffer;
		  /* indice de destination */
		  targetInd = pViewSel->VsIndBuf;
		  /* dernier buffer=pSourceBuffer */
		  pSourceBuffer = pFrame->FrSelectionEnd.VsBuffer;
		  /* indice de pSourceBuffer */
		  sourceInd = pFrame->FrSelectionEnd.VsIndBuf;
		  /* nombre de caracteres a copier */
		  i = pSourceBuffer->BuLength - sourceInd;

		  /* ==>Nombre de caracteres a copier nul */
		  if (i <= 0)
		     i = targetInd - 1;
		  /* ==>Il y a des caracteres a deplacer */
		  else
		    {
		       length = THOT_MAX_CHAR - targetInd;
		       /* deplace en deux fois? */
		       if (i > length)
			 {
			    ustrncpy (&pTargetBuffer->BuContent[targetInd - 1], &pSourceBuffer->BuContent[sourceInd], length);
			    pTargetBuffer->BuLength = FULL_BUFFER;
			    pTargetBuffer->BuContent[THOT_MAX_CHAR - 1] = EOS;
			    targetInd = 1;
			    sourceInd += length;
			    i -= length;
			    pTargetBuffer = pTargetBuffer->BuNext;
			 }
		       ustrncpy (&pTargetBuffer->BuContent[targetInd - 1], &pSourceBuffer->BuContent[sourceInd], i);
		       i = i + targetInd - 1;
		    }

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

		       /* Si la selection courante reference le buffer libere */
		       if (pTargetBuffer == pViewSel->VsBuffer)
			  if (pSourceBuffer->BuNext != NULL)	/* Apres la destruction */
			     pViewSel->VsBuffer = pSourceBuffer->BuNext;
			  else
			     /* ElemIsBefore la destruction */
			    {
			       pViewSel->VsBuffer = pTargetBuffer->BuPrevious;
			       if (pViewSel->VsBuffer != NULL)
				  pViewSel->VsIndBuf = pViewSel->VsBuffer->BuLength + 1;
			    }

		       if (pTargetBuffer == pSourceBuffer)
			  pSourceBuffer = NULL;
		       pTargetBuffer = DeleteBuffer (pTargetBuffer, frame);
		    }

		  /* detruit les buffers qui ont ete vides */
		  while (pSourceBuffer != NULL && pSourceBuffer != pTargetBuffer)
		     pSourceBuffer = DeleteBuffer (pSourceBuffer, frame);

		  /* Evaluation des changements sur la boite */
		  if (pBox->BxSpaceWidth != 0)
		    {
		       /* Si la boite est adjustifiee */
		       i = CharacterWidth (SPACE, font);
		       adjust = xDelta + (pBox->BxSpaceWidth - i) * spacesDelta;
		    }
		  else
		     adjust = 0;
		  if (charsDelta == pAb->AbVolume)
		    {
		       /* La boite devient vide */
		       yDelta = width;
		       /* Largeur forcee */
		       xDelta = pAb->AbBox->BxWidth - yDelta;
		       if (pBox == pAb->AbBox && adjust != 0)
			  adjust = xDelta;
		       else
			  /* Le bloc de ligne est reevalue */
			  adjust = 0;
		       pViewSel->VsBox = pAb->AbBox;
		    }
		  else if (pViewSel->VsIndBox == pBox->BxNChars - charsDelta)
		     /* En fin de boite */
		     yDelta = 2;
		  else if (pViewSel->VsIndBuf > pViewSel->VsBuffer->BuLength)
		    {
		       /* En fin de buffer -> on va chercher le caractere suivant */
		       pSourceBuffer = pViewSel->VsBuffer->BuNext;
		       /* Il peut etre vide s'il y a des blancs en fin de ligne */
		       if (pSourceBuffer == NULL)
			  yDelta = 2;
		       else
			 {
			    /* deplace la selection sur le debut du buffer suivant */
			    pViewSel->VsBuffer = pSourceBuffer;
			    pViewSel->VsIndBuf = 1;
			    yDelta = CharacterWidth (pSourceBuffer->BuContent[0], font);
			 }
		    }
		  else
		     yDelta = CharacterWidth (pViewSel->VsBuffer->BuContent[pViewSel->VsIndBuf - 1], font);

		  /* Mise a jour de la selection sur le caractere suivant */
		  pFrame->FrSelectionEnd.VsBox = pViewSel->VsBox;
		  pFrame->FrSelectionEnd.VsXPos = pViewSel->VsXPos + yDelta;
		  pFrame->FrSelectionEnd.VsBuffer = pViewSel->VsBuffer;
		  pFrame->FrSelectionEnd.VsIndBuf = pViewSel->VsIndBuf;
		  pFrame->FrSelectionEnd.VsIndBox = pViewSel->VsIndBox;
		  pFrame->FrSelectionEnd.VsNSpaces = pViewSel->VsNSpaces;
		  pFrame->FrSelectionEnd.VsLine = pLine;

		  /* Mise a jour des boites */
		  pAb->AbVolume -= charsDelta;
		  BoxUpdate (pAb->AbBox, pLine, -charsDelta, -spacesDelta, -xDelta, -adjust, 0, frame, FALSE);
		  CloseTextInsertion ();
		  break;

	       case LtPicture:
		  /* pPa1->AbPictInfo->PicFileName->BuLength = 0; */
		  /*ustrcpy(pPa1->AbPictInfo->PicFileName, ""); */
		  pAb->AbVolume = 0;
		  /* met a jour la boite */
		  pictInfo = (PictInfo *) pBox->BxPictInfo;
		  if (pictInfo->PicPixmap != 0)
		    {
		       if (defaultWidth)
			  xDelta = pictInfo->PicWArea - width;
		       else
			  xDelta = 0;
		       if (defaultHeight)
			  yDelta = pictInfo->PicHArea - height;
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
		  pAb->AbVolume = 0;
		  pAb->AbShape = EOS;
		  break;

	       default:
		  break;
	    }
}


/*----------------------------------------------------------------------
   detruit sans sauver le contenu de la selection courante.        
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         DeleteSelection (ThotBool defaultHeight, ThotBool defaultWidth, PtrLine pLine, PtrBox pBox, PtrAbstractBox pAb, int frame)
#else  /* __STDC__ */
static void         DeleteSelection (defaultHeight, defaultWidth, pLine, pBox, pAb, frame)
ThotBool            defaultHeight;
ThotBool            defaultWidth;
PtrLine             pLine;
PtrBox              pBox;
PtrAbstractBox      pAb;
int                 frame;

#endif /* __STDC__ */
{
   PtrTextBuffer       pTargetBuffer;
   ViewFrame          *pFrame;
   int                 i;
   int                 xDelta, charsDelta;
   int                 spacesDelta;

   if (pAb != NULL)
     {
	if (pAb->AbLeafType == LtText)
	  {
	     pFrame = &ViewFrameTable[frame - 1];
	     /* calcule les valeurs xDelta, spacesDelta, charsDelta */
	     if (pFrame->FrSelectionBegin.VsIndBox == pAb->AbVolume)
	       {
		  xDelta = 0;
		  spacesDelta = 0;
		  charsDelta = 0;
	       }
	     else
	       {
		  /* Indice de debut */
		  i = 1;
		  pTargetBuffer = NULL;
		  CopyBuffers (pBox->BxFont, frame, pFrame->FrSelectionBegin.VsIndBuf, pFrame->FrSelectionEnd.VsIndBuf, i,
			       pFrame->FrSelectionBegin.VsBuffer, pFrame->FrSelectionEnd.VsBuffer, &pTargetBuffer, &xDelta, &spacesDelta, &charsDelta);
	       }
	  }
     }
   /* coupe le contenu de la selection courante */
   RemoveSelection (charsDelta, spacesDelta, xDelta, defaultHeight, defaultWidth, pLine, pBox, pAb, frame);
}


/*----------------------------------------------------------------------
   insere le dernier clipboard X ou Thot sauve.                    
   - charsDelta donne le nombre de caracteres copies               
   - spacesDelta donne le nombre de blancs copies                  
   - xDelta donne la largeur de la chaine copiee                   
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         PasteClipboard (ThotBool defaultHeight, ThotBool defaultWidth, PtrLine pLine, PtrBox pBox, PtrAbstractBox pAb, int frame, PtrTextBuffer clipboard)
#else  /* __STDC__ */
static void         PasteClipboard (defaultHeight, defaultWidth, pLine, pBox, pAb, frame, clipboard)
ThotBool            defaultHeight;
ThotBool            defaultWidth;
PtrLine             pLine;
PtrBox              pBox;
PtrAbstractBox      pAb;
int                 frame;
PtrTextBuffer       clipboard;

#endif /* __STDC__ */
{
   int                 ind, endInd;
   PtrTextBuffer       pBuffer;
   PtrTextBuffer       pNewBuffer;
   ViewFrame          *pFrame;
   ViewSelection      *pViewSel;
   ViewSelection      *pViewSelEnd;
   PtrTextBuffer       pCurrentBuffer;
   int                 i;
   int                 xDelta, yDelta;
   int                 spacesDelta, charsDelta;
   int                 adjust;
   ptrfont             font;
   int                 height;
   int                 width;
   PictInfo           *pictInfo;

   font = pBox->BxFont;
   width = 2; /* see GiveTextSize function */
   height = FontHeight (font);

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
	       if (pViewSel->VsIndBuf > pViewSel->VsBuffer->BuLength)
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
		    pBuffer = pViewSel->VsBuffer->BuPrevious;
		    /* index debut d'insertion */
		    ind = pBuffer->BuLength + 1;
		 }

	       /* Insertion des caracteres */
	       pNewBuffer = pBuffer;
	       CopyBuffers (font, frame, 1, 0, ind, clipboard, NULL, &pNewBuffer, &xDelta, &spacesDelta, &charsDelta);
	       /* index fin d'insertion */
	       endInd = pNewBuffer->BuLength + 1;

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
	       /* last inserted index */
	       pViewSel->VsIndBuf = endInd;
	       pViewSelEnd->VsIndBuf = endInd;
	       pViewSelEnd->VsBox = pViewSel->VsBox;
	       /* +nombre de blancs */
	       pViewSel->VsNSpaces = pViewSel->VsNSpaces + spacesDelta;
	       pViewSelEnd->VsNSpaces = pViewSel->VsNSpaces;
	       pViewSel->VsLine = pLine;
	       pViewSelEnd->VsLine = pLine;
	       pFrame->FrSelectOnePosition = TRUE;
	       /* the new selection is not shown */
	       pFrame->FrSelectShown = FALSE;
	       /* Si la boite etait vide */
	       if (pAb->AbVolume == charsDelta)
		  xDelta -= width;
	       if (pBox->BxSpaceWidth != 0)
		 {
		    /* Si la boite est adjustifiee */
		    i = CharacterWidth (SPACE, font);
		    adjust = xDelta + (pBox->BxSpaceWidth - i) * spacesDelta;
		 }
	       else
		  adjust = 0;

	       /* Si le clipboard contient un caractere de coupure forcee */
	       /* il faut demander la reevaluation de la mise en lignes  */
	       if (IsLineBreakInside (pBox->BxBuffer, pBox->BxFirstChar, pBox->BxNChars + charsDelta + 1))
		  BoxUpdate (pBox, pLine, charsDelta, spacesDelta, xDelta, adjust, 0, frame, TRUE);
	       else
		  BoxUpdate (pBox, pLine, charsDelta, spacesDelta, xDelta, adjust, 0, frame, FALSE);
	       CloseTextInsertion ();
	       break;
	    case LtPicture:
	       pCurrentBuffer = pAb->AbElement->ElText;
	       /* met a jour la boite */
	       i = clipboard->BuLength;
	       ustrncpy (&pCurrentBuffer->BuContent[0], &clipboard->BuContent[0], i);
	       /* Termine la chaine de caracteres */
	       pCurrentBuffer->BuContent[i] = EOS;
	       pCurrentBuffer->BuLength = i;
	       pictInfo = (PictInfo *) pBox->BxPictInfo;
	       CopyPictInfo ((int *) pictInfo, (int *) &PictClipboard);
	       pictInfo->PicFileName = pCurrentBuffer->BuContent;
	       SetCursorWatch (frame);
	       LoadPicture (frame, pBox, pictInfo);
	       ResetCursorWatch (frame);
	       if (pictInfo->PicPixmap != 0)
		 {
		    pAb->AbVolume = i;
		    xDelta = pictInfo->PicWArea - width;
		    yDelta = pictInfo->PicHArea - height;
		    BoxUpdate (pBox, pLine, 0, 0, xDelta, 0, yDelta, frame, FALSE);
		 }
	       break;
	    case LtSymbol:
	       LoadSymbol ((char)clipboard->BuContent[0], pLine, defaultHeight, defaultWidth, pBox, pAb, frame);
	       break;
	    case LtPolyLine:
	    case LtGraphics:
	       LoadShape ((char)clipboard->BuContent[0], pLine, defaultHeight, defaultWidth, pBox, pAb, frame);
	       break;
	    default:
	       break;
	 }
}


/*----------------------------------------------------------------------
   Traite les commandes TextInserting Cut Paste Copy Oops          
   ainsi que l'insertion des Graphiques Images et Symboles         
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         ContentEditing (int editType)
#else  /* __STDC__ */
static void         ContentEditing (editType)
int                 editType;

#endif /* __STDC__ */
{
   PtrBox              pBox;
   PtrBox              pSelBox;
   PtrAbstractBox      pAb, pCell, draw;
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
#ifdef IV
   int                 width, height;
#endif
   int                 spacesDelta, charsDelta;
   int                 frame;
   ThotBool            still, ok, textPasted;
   ThotBool            defaultWidth, defaultHeight;

   /* termine l'insertion de caracteres en cours */
   CloseTextInsertion ();
   pCell = NULL;
   textPasted = FALSE;

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
	  {
	     TtaDisplaySimpleMessage (INFO, LIB, TMSG_SEL_BEFORE);
	     return;
	  }
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
		     return;	/* ne peut rien faire */
		  else
		    {
		       pBox = ViewFrameTable[frame - 1].FrSelectionBegin.VsBox;
		       if (pBox == NULL)
			  return;	/* ne peut rien faire */
		       pAb = pBox->BxAbstractBox;
		    }
	       }
	     else
		pAb = pBox->BxAbstractBox;
	     if (pAb->AbReadOnly &&
		 (editType == TEXT_CUT || editType == TEXT_INSERT || editType == TEXT_PASTE))
	        return;
	  }
        /*-- La commande coller concerne le mediateur --*/
	if (editType == TEXT_PASTE && !FromKeyboard)
	   /* Il faut peut-etre deplacer la selection courante */
	   SetInsert (&pAb, &frame, ClipboardType, FALSE);

	pFrame = &ViewFrameTable[frame - 1];
	pViewSel = &pFrame->FrSelectionBegin;
	if (pBox && editType == TEXT_SUP)
	  {
	    /* don't remove the selection if it is at the end of the text */
	    if (pAb->AbLeafType == LtText &&
		pBox == ViewFrameTable[frame - 1].FrSelectionEnd.VsBox &&
		pViewSel->VsIndBox >= pBox->BxNChars)
	      return;
	  }

	if (editType == TEXT_INSERT && !FromKeyboard)
	   if (pBox != NULL && pViewSel->VsIndBox != 0)
	     {
		if (pAb->AbLeafType == LtPolyLine)
		  {
		    if (pAb->AbPolyLineShape != 'w' &&
                pAb->AbPolyLineShape != 'x' &&
                pAb->AbPolyLineShape != 'y' &&
                pAb->AbPolyLineShape != 'z')
		      {
			/* Ajout de points dans une polyline */
			still = (pAb->AbPolyLineShape == 'p' || pAb->AbPolyLineShape == 's');
			x = pBox->BxXOrg - pFrame->FrXOrg;
			y = pBox->BxYOrg - pFrame->FrYOrg;
			i = pViewSel->VsIndBox;
			draw = GetParentDraw (pBox);
			pBox->BxNChars = PolyLineExtension (frame,
							    &x, &y,
							    pBox, draw,
							    pBox->BxNChars,
							    i, still);
			pAb->AbVolume = pBox->BxNChars;
#ifdef IV
			if (draw)
			  {
			    /* on force le reaffichage de la boite (+ les points de selection) */
			    width = pBox->BxWidth;
			    height = pBox->BxHeight;
			    x += pFrame->FrXOrg;
			    y += pFrame->FrYOrg;
			    DefClip (frame, pBox->BxXOrg - EXTRA_GRAPH, pBox->BxYOrg - EXTRA_GRAPH, pBox->BxXOrg + width + EXTRA_GRAPH, pBox->BxYOrg + height + EXTRA_GRAPH);
			    if (x != pBox->BxXOrg || y != pBox->BxYOrg)
			      NewPosition (pAb, x, y, frame, TRUE);
			    width = PointToPixel (pBox->BxBuffer->BuPoints[0].XCoord / 1000);
			    height = PointToPixel (pBox->BxBuffer->BuPoints[0].YCoord / 1000);
			    if (width != pBox->BxWidth || height != pBox->BxHeight)
			      NewDimension (pAb, width, height, frame, TRUE);
			  }
#else
			if (pBox->BxPictInfo != NULL)
			  {
			    /* reevalue les points de controle */
			    free ((STRING) pBox->BxPictInfo);
			    pBox->BxPictInfo = NULL;
			  }
#endif
		      }
		    else
		      /* la commande est realisee par l'application */
		      pAb = NULL;
		  }
		else
		  {
		     if (ThotLocalActions[T_insertpaste] != NULL)
			(*ThotLocalActions[T_insertpaste]) (TRUE, FALSE, TEXT('L'), &ok);
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
		   (*ThotLocalActions[T_insertpaste]) (TRUE, FALSE, TEXT('L'), &ok);
		else
		   ok = FALSE;
		if (ok)
		   /* la commande est realisee par l'application */
		   pAb = NULL;
	     }

	if (pAb != NULL)
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
		     (editType == TEXT_CUT || editType == TEXT_DEL || editType == TEXT_SUP))
		   {
		     (*ThotLocalActions[T_deletenextchar]) (frame, pAb->AbElement, FALSE);
		     return;
		   }
		 else
		   pAb = NULL;
	       }
	     else if (pAb != NULL)
		pAb = pBox->BxAbstractBox;

	     if (pAb != NULL)
	       {
		  if (pAb != pLastAb	/* il y a plus d'un pave */
		   || (pAb->AbLeafType == LtText && editType == TEXT_INSERT)
		      || pAb->AbLeafType == LtCompound	/* le pave est compose */
		      || pAb->AbLeafType == LtPageColBreak)	/* c'est une marque de page */
		     pAb = NULL;
		  else if ((editType == TEXT_CUT || editType == TEXT_DEL ||
			    editType == TEXT_SUP || editType == TEXT_COPY) &&
			   (pAb->AbVolume == 0 || pAb->AbLeafType == LtGraphics ||
			    pAb->AbLeafType == LtPicture))
		     /* coupe ou copie un pave vide ou graphique ou une image */
		     pAb = NULL;
		  else if ((editType == TEXT_CUT || editType == TEXT_COPY) && pAb->AbLeafType == LtPolyLine)
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
		  SaveInClipboard (&charsDelta, &spacesDelta, &x, 0, NULL, pAb, frame, &ClipboardThot);
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
		    if (pBox->BxNChars <= 3)
		      TtaDisplaySimpleMessage (INFO, LIB, TMSG_TWO_POINTS_IN_POLYLINE_NEEDED);
		    else if (pAb->AbPolyLineShape != 'w' &&
			     pAb->AbPolyLineShape != 'x' &&
			     pAb->AbPolyLineShape != 'y' &&
			     pAb->AbPolyLineShape != 'z')
		      {
			/* Destruction du point courant de la polyline */
			charsDelta = pViewSel->VsIndBox;
			DeletePointInPolyline (&(pAb->AbPolyLineBuffer), charsDelta);
			DeletePointInPolyline (&(pBox->BxBuffer), charsDelta);
			(pBox->BxNChars)--;
			if (pBox->BxPictInfo != NULL)
			  {
			    /* reevalue les points de controle */
			    free ((STRING) pBox->BxPictInfo);
			    pBox->BxPictInfo = NULL;
			  }
			(pAb->AbVolume)--;
			if (charsDelta == pBox->BxNChars)
			  /* on vient de detruire le dernier point de la polyline */
			  if (charsDelta == 1)
			    charsDelta = 0;
			  else
			    charsDelta = 1;
			pViewSel->VsIndBox = charsDelta;
			/* on force le reaffichage de la boite (+ les points de selection) */
			DefClip (frame, pBox->BxXOrg - EXTRA_GRAPH, pBox->BxYOrg - EXTRA_GRAPH, pBox->BxXOrg + pBox->BxWidth + EXTRA_GRAPH, pBox->BxYOrg + pBox->BxHeight + EXTRA_GRAPH);
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
		else if (pPavD1->DimAbRef == NULL && pPavD1->DimValue == 0)
		  defaultWidth = TRUE;
		else
		  defaultWidth = FALSE;
		
		pPavD1 = &pAb->AbHeight;
		if (pPavD1->DimIsPosition)
		  /* Boite elastique */
		  defaultHeight = FALSE;
		else if (pPavD1->DimAbRef == NULL && pPavD1->DimValue == 0)
		  defaultHeight = TRUE;
		else
		  /* Dimensions d'une boite terminale vide */
		  defaultHeight = FALSE;
		
		/* Traitement */
		if (editType == TEXT_INSERT && pAb->AbLeafType == LtPicture && !FromKeyboard)
		  LoadPictFile (pLine, defaultHeight, defaultWidth, pBox, pAb, frame);
		else if (editType == TEXT_CUT && !FromKeyboard)
		  {
		    SaveInClipboard (&charsDelta, &spacesDelta, &x, i, pBuffer, pAb, frame, &ClipboardThot);
		    if (ClipboardThot.BuLength == 0)
		      {
			if (ThotLocalActions[T_deletenextchar] != NULL)
			  (*ThotLocalActions[T_deletenextchar]) (frame, pAb->AbElement, FALSE);
			else
			  {
			    TtaDisplaySimpleMessage (INFO, LIB, TMSG_NOTHING_TO_DEL);
			    /* Pas de reaffichage */
			    DefClip (frame, 0, 0, 0, 0);
			  }
			/* Il n'est pas necessaire de mettre a jour la selection */
			pAb = NULL;
		      }
		    else
		      RemoveSelection (charsDelta, spacesDelta, x, defaultHeight, defaultWidth, pLine, pBox, pAb, frame);
		  }
		else if ((editType == TEXT_DEL || editType == TEXT_SUP) && !FromKeyboard)
		  if (pAb->AbVolume == 0 ||
		      pViewSel->VsIndBox + pViewSel->VsBox->BxIndChar == pAb->AbVolume)
		    {
		      /* current selection is at the element end */
		      if (ThotLocalActions[T_deletenextchar] != NULL)
			(*ThotLocalActions[T_deletenextchar]) (frame, pAb->AbElement, FALSE);
		      else
			{
			  TtaDisplaySimpleMessage (INFO, LIB, TMSG_NOTHING_TO_DEL);
			  /* Pas de reaffichage */
			  DefClip (frame, 0, 0, 0, 0);
			  pFrame->FrReady = TRUE;
			}
		      /* Il n'est pas necessaire de mettre a jour la selection */
		      pAb = NULL;
		    }
		  else
		    DeleteSelection (defaultHeight, defaultWidth, pLine, pBox, pAb, frame);
		else if (editType == TEXT_PASTE && !FromKeyboard)
		  {
		    /* Verifie que l'alphabet du clipboard correspond a celui du pave */
		    if (ClipboardLanguage == 0)
		      ClipboardLanguage = TtaGetDefaultLanguage ();
		    if (pAb->AbLeafType != LtText && pAb->AbLanguage != ClipboardLanguage)
		      {
			/* charsDelta contient le nombre de carateres qui precedent dans la boite */
			NewTextLanguage (pAb, charsDelta + pBox->BxIndChar + 1, ClipboardLanguage, TRUE);
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
		      PasteClipboard (defaultHeight, defaultWidth, pLine, pBox, pAb, frame, &ClipboardThot);
		      textPasted = TRUE;
		      }
		  }
		else if (editType == TEXT_COPY && !FromKeyboard)
		  {
		    SaveInClipboard (&charsDelta, &spacesDelta, &x, i, pBuffer, pAb, frame, &ClipboardThot);
		    /* Pas de reaffichage */
		    DefClip (frame, 0, 0, 0, 0);
		    /* Il n'est pas necessaire de mettre a jour la selection */
		    pAb = NULL;
		  }
		else if (editType == TEXT_X_PASTE && !FromKeyboard)
		  {
		  PasteClipboard (defaultHeight, defaultWidth, pLine, pBox, pAb, frame, &XClipboard);
		  textPasted = TRUE;
		  }
		else if (pAb->AbLeafType == LtPicture && FromKeyboard)
		  LoadPictFile (pLine, defaultHeight, defaultWidth, pBox, pAb, frame);
		else if (pAb->AbLeafType == LtSymbol && FromKeyboard)
		  LoadSymbol ((char) editType, pLine, defaultHeight, defaultWidth, pBox, pAb, frame);
		else if ((pAb->AbLeafType == LtGraphics || pAb->AbLeafType == LtPolyLine) && FromKeyboard)
		  LoadShape ((char) editType, pLine, defaultHeight, defaultWidth, pBox, pAb, frame);
	      }
          }

	if (pCell != NULL && ThotLocalActions[T_checkcolumn])
	  {
	    /* we have to propage position to cell children */
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

	if (pAb != NULL)
	  {
	     if (pAb->AbLeafType == LtGraphics)
		pAb->AbRealShape = pAb->AbShape;
	     if (pAb->AbLeafType == LtGraphics || pAb->AbLeafType == LtPolyLine)
	       {
		  /* remonte a la recherche d'un ancetre elastique */
		  pLastAb = pAb;
		  while (pLastAb != NULL)
		    {
		       pSelBox = pLastAb->AbBox;
		       if (pSelBox->BxHorizFlex || pSelBox->BxVertFlex)
			 {
			    MirrorShape (pAb, pSelBox->BxHorizInverted, pSelBox->BxVertInverted, FromKeyboard);
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
		 if (ThotLocalActions[T_switchsel])
		   (*ThotLocalActions[T_switchsel]) (frame, FALSE);
		 /* debloque l'affichage */
		 still = RedrawFrameBottom (frame, 0);
		 if (ThotLocalActions[T_switchsel])
		   (*ThotLocalActions[T_switchsel]) (frame, TRUE);
	       }

	     if (pAb->AbElement == NULL)
		/* le pave a disparu entre temps */
		return;

	     NewContent (pAb);
	     if (textPasted)
	        /* send event TteElemTextModify.Post */
	        APPtextModify (pAb->AbElement, frame, FALSE);

	     /* signale la nouvelle selection courante */
	     if ((editType == TEXT_CUT || editType == TEXT_PASTE ||
		  editType == TEXT_X_PASTE || editType == TEXT_DEL ||
		  editType == TEXT_SUP) &&
		 pViewSel->VsBox != NULL)
	       {
		  pViewSelEnd = &pFrame->FrSelectionEnd;
		  i = pViewSel->VsBox->BxIndChar + pViewSel->VsIndBox;
		  if (pViewSel->VsIndBuf > 0)
		     i++;
		  j = pViewSelEnd->VsBox->BxIndChar + pViewSelEnd->VsIndBox;
		  if (pViewSelEnd->VsIndBuf > 0)
		     j++;
		  ChangeSelection (frame, pViewSel->VsBox->BxAbstractBox, i, FALSE, TRUE, FALSE, FALSE);
		  if (pAb->AbLeafType != LtPolyLine)
		     if (j != i)
			ChangeSelection (frame, pViewSelEnd->VsBox->BxAbstractBox, j, TRUE, TRUE, FALSE, FALSE);
	       }
	  }
     }
}

/*----------------------------------------------------------------------
   Insere un caractere dans une boite de texte.                    
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                InsertChar (int frame, UCHAR_T c, int keyboard)
#else  /* __STDC__ */
void                InsertChar (frame, c, keyboard)
int                 frame;
UCHAR_T       c;
int                 keyboard;
#endif /* __STDC__ */
{
  PtrTextBuffer       pBuffer;
  PtrAbstractBox      pAb, pBlock;
  PtrBox              pBox;
  PtrBox              pSelBox;
  ViewSelection      *pViewSel;
  ViewSelection      *pViewSelEnd;
  ViewFrame          *pFrame;
  ptrfont             font;
  LeafType            nat;
  Propagation         savePropagate;
  int                 xx, xDelta, adjust;
  int                 spacesDelta;
  int                 topY, bottomY;
  int                 charsDelta, pix;
  int                 visib, zoom;
  int                 ind;
  int                 previousChars;
  ThotBool            beginOfBox;
  ThotBool            toDelete;
  ThotBool            toSplit;
  ThotBool            saveinsert;
  ThotBool            notification = FALSE;

  toDelete = (c == (CHAR_T) (127));
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
		(*ThotLocalActions[T_deletenextchar]) (frame, pAb->AbElement, TRUE);
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
	      /* prend la boite d'alphabet courant ou au besoin */
	      /* cree un nouveau texte avec le bon alphabet */
	      if (!toDelete)
		notification = GiveAbsBoxForLanguage (frame, &pAb, keyboard);
	      if (notification)
		/* selection could be modified by the application */
		InsertChar (frame, c, keyboard);
	      else
		{
		  /* selection could not be modified by the application */

		  /* Recherche le point d'insertion du texte */
		  GiveInsertPoint (pAb, frame, &pSelBox, &pBuffer, &ind, &xx, &previousChars);
		  
		  if (pAb != NULL)
		    {
		      /* keyboard ni Symbol ni Graphique */
		      /* bloque l'affichage de la fenetre */
		      pFrame->FrReady = FALSE;
		      
		      /* initialise l'insertion */
		      if (!TextInserting)
			StartTextInsertion (pAb, frame, pSelBox, pBuffer, ind, previousChars);
		      font = pSelBox->BxFont;
		      
		      if (pBuffer == NULL)
			return;
		      /* La selection doit se trouver en fin de buffer */
		      if (ind <= pBuffer->BuLength && pBuffer->BuPrevious != NULL)
			pBuffer = pBuffer->BuPrevious;
		      
		      /* prepare le reaffichage */
		      /* point d'insertion en x */
		      xx += pSelBox->BxXOrg;
		      /* point d'insertion superieur en y */
		      topY = pSelBox->BxYOrg;
		      /* point d'insertion inferieur en y */
		      bottomY = topY + pSelBox->BxHeight;
		      DefClip (frame, xx, topY, xx, bottomY);
		      
		      /* Est-on au debut d'une boite entiere ou coupee ? */
		      pBox = pAb->AbBox->BxNexChild;
		      if ((pBox == NULL || pSelBox == pBox) && previousChars == 0)
			beginOfBox = TRUE;
		      else
			beginOfBox = FALSE;
		  
		      if (toDelete)	/* ========================== Delete */
			/* n'a rien a detruire */
			if (beginOfBox)
			  {
			    CloseTextInsertion ();
			    if (ThotLocalActions[T_deletenextchar] != NULL)
			      (*ThotLocalActions[T_deletenextchar]) (frame, pAb->AbElement, TRUE);
			    pFrame->FrReady = TRUE;
			    return;
			  }
			else
			  {
			    /* efface la selection precedente */
			    if (ThotLocalActions[T_switchsel])
			      (*ThotLocalActions[T_switchsel]) (frame, FALSE);
			    /* libere le buffer vide */
			    if (pBuffer->BuLength == 0)
			      if (pBuffer->BuPrevious != NULL)
				{
				  /* Si pBuffer = 1er buffer de la boite ? */
				  if (pSelBox->BxBuffer == pBuffer)
				    {
				      /* Liberation du buffer */
				      pBuffer = DeleteBuffer (pBuffer, frame);
				      pSelBox->BxBuffer = pBuffer;
				      
				      /* MAJ de la boite coupee ? */
				      if (pSelBox->BxFirstChar == 0)
					{
					  pBox->BxBuffer = pBuffer;
					  pAb->AbText = pBuffer;
					  /* S'il y a une boite vide avant pSelBox */
					  if (pBox->BxNexChild != pSelBox && pBox->BxNexChild != NULL)
					    pBox->BxNexChild->BxBuffer = pBuffer;
					}
				      /* MAJ de la boite precedente */
				      else if (pSelBox->BxPrevious->BxNChars == 0)
					pSelBox->BxPrevious->BxBuffer = pBuffer;
				    }
				  else
				    /* Liberation du buffer */
				    pBuffer = DeleteBuffer (pBuffer, frame);
				}
			    /* Sinon pBuffer est le 1er buffer de la boite */
			      else if (pBuffer->BuNext != NULL)
				{
				  /* Le nouveau 1er buffer est le suivant */
				  pSelBox->BxBuffer = pBuffer->BuNext;
				  /* Liberation du buffer */
				  pBuffer = DeleteBuffer (pBuffer, frame);
				  pBuffer = pSelBox->BxBuffer;
				  
				  /* MAJ de la boite coupee ? */
				  if (pSelBox->BxFirstChar == 0)
				    {
				      pBox->BxBuffer = pBuffer;
				      pAb->AbText = pBuffer;
				      /* S'il y a une boite vide avant pSelBox */
				      if (pBox->BxNexChild != pSelBox && pBox->BxNexChild != NULL)
					pBox->BxNexChild->BxBuffer = pBuffer;
				    }
				  /* MAJ de la boite precedente */
				  else if (pSelBox->BxPrevious->BxNChars == 0)
				    pSelBox->BxPrevious->BxBuffer = pBuffer;
				}	/*else if */
			    
			    /* enleve le caractere dans la chaine des buffers */
			    c = pBuffer->BuContent[pBuffer->BuLength - 1];
			    pBuffer->BuContent[pBuffer->BuLength - 1] = EOS;
			    pBuffer->BuLength--;
			    
			    /* met a jour la selection en fin de buffer */
			    if (pViewSel->VsBuffer == pBuffer)
			      {
				pViewSel->VsIndBuf--;
				ind--;
				if (pViewSelEnd->VsBuffer == pBuffer)
				  pViewSelEnd->VsIndBuf--;
			      }
			    
			    /* libere le buffer vide */
			    if (pBuffer->BuLength == 0)
			      if (pBuffer->BuPrevious != NULL)
				{
				  /* Si pBuffer = 1er buffer de la boite ? */
				  if (pSelBox->BxBuffer == pBuffer)
				    {
				      /* Liberation du buffer */
				      pBuffer = DeleteBuffer (pBuffer, frame);
				      pSelBox->BxBuffer = pBuffer;
				      
				      /* MAJ de la boite coupee ? */
				      if (pSelBox->BxFirstChar == 0)
					{
					  pBox->BxBuffer = pBuffer;
					  pAb->AbText = pBuffer;
					  /* S'il y a une boite vide avant pSelBox */
					  if (pBox->BxNexChild != pSelBox && pBox->BxNexChild != NULL)
					    pBox->BxNexChild->BxBuffer = pBuffer;
					}
				      /* MAJ de la boite precedente */
				      else if (pSelBox->BxPrevious->BxNChars == 0)
					pSelBox->BxPrevious->BxBuffer = pBuffer;
				    }
				  else
				    /* Liberation du buffer */
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
			    
			    /* ==> La boite entiere devient vide */
			    if (previousChars == 1 && pSelBox->BxType == BoComplete
				&& pSelBox->BxNChars == 1)
			      {
				/* Mise a jour des marques */
				xDelta = CharacterWidth ('m', font);
				pViewSel->VsXPos = 0;
				pViewSel->VsIndBox = 0;
				pViewSel->VsNSpaces = 0;
				if (pViewSelEnd->VsBox == pFrame->FrSelectionBegin.VsBox)
				  {
				    pViewSelEnd->VsXPos = xDelta;
				    pViewSelEnd->VsIndBox = 0;
				    pViewSelEnd->VsNSpaces = 0;
				  }
				
				/* Prepare la mise a jour de la boite */
				xDelta -= pSelBox->BxWidth;
				pFrame->FrClipXBegin = pSelBox->BxXOrg;
			      }
			    /* ==> detruit un caractere dans un mot coupe */
			    /*     ou une coupure forcee (Ctrl Return)       */
			    else if (previousChars == 0 && c != SPACE)
			      {
				/* Le bloc de ligne et marques de selection sont reevalues */
				/* -> deplace la marque de selection */
				pSelBox = pSelBox->BxPrevious;
				/* L'origine du reaffichage est modifiee */
				topY = pSelBox->BxYOrg;
				DefClip (frame, xx, topY, xx, bottomY);
				
				/* Prepare la mise a jour de la boite */
				toSplit = TRUE;
				
				/* Est-ce un boite qui ne contenait qu'un Ctrl Return ? */
				if ((c == (UCHAR_T) BREAK_LINE ||
				     c == (UCHAR_T) NEW_LINE) &&
				    pAb->AbBox->BxNChars == 1)
				  {
				    /* La boite entiere devient vide */
				    xDelta = CharacterWidth ('m', font);
				    pFrame->FrClipXBegin = pSelBox->BxXOrg;
				    /* Mise a jour de la selection */
				    pSelBox = pAb->AbBox;
				    pViewSel->VsXPos = 0;
				    pViewSel->VsIndBox = 0;
				    if (pViewSel->VsLine != NULL)
				      pViewSel->VsLine = pViewSel->VsLine->LiPrevious;
				    
				    if (pViewSelEnd->VsBox == pFrame->FrSelectionBegin.VsBox)
				      {
					pViewSelEnd->VsBox = pSelBox;
					pViewSelEnd->VsLine = pViewSel->VsLine;
					pViewSelEnd->VsXPos = xDelta;
					pViewSelEnd->VsIndBox = 0;
				      }
				    pViewSel->VsBox = pSelBox;
				  }
				else
				  {
				    xDelta = -CharacterWidth (c, font);
				    pViewSel->VsBox = pSelBox;
				    if (pViewSel->VsLine != NULL)
				      pViewSel->VsLine = pViewSel->VsLine->LiPrevious;
				    
				    if (pFrame->FrSelectionEnd.VsBuffer == pViewSel->VsBuffer
					&& pFrame->FrSelectionEnd.VsIndBuf == pViewSel->VsIndBuf)
				      {
					pFrame->FrSelectionEnd.VsBox = pSelBox;
					pFrame->FrSelectionEnd.VsLine = pViewSel->VsLine;
				      }
				    pFrame->FrClipXBegin += xDelta;
				  }
			      }
			    /* ==> supprime un blanc de fin de ligne entre deux boites */
			    else if ((previousChars > pSelBox->BxNChars || previousChars == 0)
				     && c == SPACE)
			      {
				/* Le bloc de ligne et marques de selection sont reevalues */
				/* il faut reevaluer la mise en ligne */
				toSplit = TRUE;
				xDelta = -CharacterWidth (c, font);
				if (previousChars == 0)
				  {
				    /* Si la selection est en debut de boite  */
				    /* on force la reevaluation du bloc de    */
				    /* lignes a partir de la boite precedente */
				    pBox = pSelBox->BxPrevious;
				    if (pViewSel->VsLine != NULL)
				      pViewSel->VsLine = pViewSel->VsLine->LiPrevious;
				    
				    /* Si la boite precedente est vide      */
				    /* et c'est la premier boite de coupure */
				    if (pBox->BxNChars == 0 && pBox->BxIndChar == 0)
				      {
					pBox = pAb->AbBox;
					/* Est-ce que la boite devient vide ? */
					if (pBox->BxNChars == 1)
					  xDelta = CharacterWidth ('m', font) - pBox->BxWidth;
				      }
				    /* Reevaluation du debut de la boite coupee ? */
				    else if (pBox->BxIndChar == 0)
				      pBox = pAb->AbBox;
				    pSelBox = pBox;
				  }	/* if previousChars=0 */
				
				/* Prepare l'affichage */
				pFrame->FrClipXBegin += xDelta;
			      }
			    /* ==> Les autre cas de supression */
			    else
			      {
				if (c == SPACE)
				  {
				    xDelta = -CharacterWidth (SPACE, font);
				    adjust = -pSelBox->BxSpaceWidth;
				    if (adjust < 0)
				      {
					if (pSelBox->BxNPixels >= pViewSel->VsNSpaces)
					  pix = -1;
				      }
				  }
				else if (c == EOS)
				  /* Caractere Nul */
				  xDelta = 0;
				else
				  xDelta = -CharacterWidth (c, font);
				
				pFrame->FrClipXBegin += xDelta;
				/* Mise a jour de la selection dans la boite */
				if (adjust != 0)
				  UpdateViewSelMarks (frame, adjust + pix, spacesDelta, charsDelta);
				else
				  UpdateViewSelMarks (frame, xDelta, spacesDelta, charsDelta);
			      }	/*else dans les autre cas */
			  } /* ====================================== Delete */
		      else
			{
			  /* efface la selection precedente */
			  if (ThotLocalActions[T_switchsel])
			    (*ThotLocalActions[T_switchsel]) (frame, FALSE);
			  /* Initialise l'insertion d'un caractere */
			  charsDelta = 1;
			  pix = 0;
			  adjust = 0;
			  if (c == SPACE)
			    spacesDelta = 1;
			  else
			    spacesDelta = 0;
			  toSplit = FALSE;
			  toDelete = FALSE;
			  
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
			  
			  pBuffer->BuLength++;
			  pBuffer->BuContent[pBuffer->BuLength - 1] = c;
			  pBuffer->BuContent[pBuffer->BuLength] = EOS;
			  if (pBuffer == pViewSel->VsBuffer)
			    {
			      /* Selection en fin de boite */
			      pViewSel->VsIndBuf++;
			      ind++;
			      if (pFrame->FrSelectionEnd.VsBuffer == pViewSel->VsBuffer)
				pFrame->FrSelectionEnd.VsIndBuf = pViewSel->VsIndBuf;
			    }
			  
			  /* ==> La boite entiere n'est plus vide */
			  if (pSelBox->BxNChars == 0 && pSelBox->BxType == BoComplete)
			    {
			      /* Mise a jour des marques */
			      xDelta = CharacterWidth (c, font);
			      pViewSel->VsXPos = xDelta;
			      pViewSel->VsIndBox = charsDelta;
			      pViewSel->VsNSpaces = spacesDelta;
			      if (pViewSelEnd->VsBox == pFrame->FrSelectionBegin.VsBox)
				if (pViewSelEnd->VsBox == pFrame->FrSelectionBegin.VsBox)
				  {
				    pViewSelEnd->VsXPos = xDelta + 2;
				    pViewSelEnd->VsIndBox = charsDelta;
				    pViewSelEnd->VsNSpaces = spacesDelta;
				  }
			      
			      /* Le caractere insere' est un Ctrl Return ? */
			      if (c == (UCHAR_T) BREAK_LINE ||
				  c == (UCHAR_T) NEW_LINE)
				{
				  /* il faut reevaluer la mise en ligne */
				  toSplit = TRUE;
				  xDelta = 0;
				}
			      
			      pFrame->FrClipXBegin = pSelBox->BxXOrg;
			      pFrame->FrClipXEnd = pSelBox->BxXOrg + pSelBox->BxWidth;
			      
			      /* Prepare la mise a jour de la boite */
			      xDelta -= pSelBox->BxWidth;
			    }
			  /* ==> Insertion d'un caractere entre deux boites */
			  else if (previousChars > pSelBox->BxNChars
				   /* ==> ou d'un blanc en fin de boite      */
				   || (c == SPACE && (previousChars == pSelBox->BxNChars || previousChars == 0)))
			    {
			      /* Prepare la mise a jour de la boite */
			      toSplit = TRUE;
			      xDelta = CharacterWidth (c, font);
			      
			      if (c == SPACE)
				adjust = pSelBox->BxSpaceWidth;
			      UpdateViewSelMarks (frame, xDelta, spacesDelta, charsDelta);
			      
			      /* Reevaluation du debut de la boite coupee ? */
			      if (previousChars == 0 && pSelBox->BxIndChar == 0)
				pSelBox = pAb->AbBox;
			      else if (previousChars == 0)
				{
				  /* Si la selection est en debut de boite  */
				  /* on force la reevaluation du bloc de    */
				  /* lignes a partir de la boite precedente */
				  pSelBox = pSelBox->BxPrevious;
				  if (pViewSel->VsLine != NULL)
				    pViewSel->VsLine = pViewSel->VsLine->LiPrevious;
				}	/*else if previousChars == 0 */
			      
			    }
			  /* ==> Les autres cas d'insertion */
			  else
			    {
			      if (c == SPACE)
				{
				  xDelta = CharacterWidth (SPACE, font);
				  adjust = pSelBox->BxSpaceWidth;
				  if (adjust > 0)
				    if (pSelBox->BxNPixels > pViewSel->VsNSpaces)
				      pix = 1;
				}
			      else if (c == (UCHAR_T) BREAK_LINE ||
				       c == (UCHAR_T) NEW_LINE)
				/* Ctrl Return */
				{
				  /* il faut reevaluer la mise en ligne */
				  toSplit = TRUE;
				  xDelta = CharacterWidth (c, font);
				}
			      else if (c == EOS)
				/* Caractere Nul */
				xDelta = 0;
			      else
				xDelta = CharacterWidth (c, font);
			      
			      /* Est-ce une insertion en debut de boite ? */
			      if (previousChars == 0)
				{
				  pSelBox->BxBuffer = pBuffer;
				  pSelBox->BxFirstChar = pBuffer->BuLength;
				  /* Est-ce une boite de coupure vide ? */
				  if (pSelBox->BxNChars == 0)
				    /* retire la largeur minimum */
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
		      if (IsLineBreakInside (pSelBox->BxBuffer, pSelBox->BxFirstChar, pSelBox->BxNChars + 1))
			toSplit = TRUE;
		      BoxUpdate (pSelBox, pViewSel->VsLine, charsDelta, spacesDelta, xDelta, adjust, 0, frame, toSplit);
		      /* Mise a jour du volume du pave */
		      pAb->AbVolume += charsDelta;
		      
		      /* Check enclosing cell */
		      if (LastInsertCell != NULL && ThotLocalActions[T_checkcolumn])
			{
			  /* we have to propage position to cell children */
			  savePropagate = Propagate;
			  Propagate = ToChildren;
			  pBlock = SearchEnclosingType (pAb, BoBlock);
			  if (pBlock != NULL)
			    RecomputeLines (pBlock, NULL, NULL, frame);
			  (*ThotLocalActions[T_checkcolumn]) (LastInsertCell, NULL, frame);
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
			if (toDelete)
			  RedrawFrameBottom (frame, 0);
			else
			  {
			    /* largeur du rectangle d'affichage */
			    charsDelta = pFrame->FrClipXEnd - pFrame->FrClipXBegin;
			    /* largeur du caractere ajoute/detruit */
			    if (xDelta < 0)
			      xDelta = -xDelta;
			    if (pFrame->FrClipYBegin == topY && pFrame->FrClipYEnd == bottomY
				&& xDelta >= charsDelta && (pFrame->FrClipXBegin == xx || pFrame->FrClipXEnd == xx))
			      RedisplayOneChar (frame, pFrame->FrClipXBegin, topY, c, font, pSelBox);
			    else
			      RedrawFrameBottom (frame, 0);
			  }
		      
		      /* restaure l'indicateur d'insertion */
		      TextInserting = saveinsert;
		      /* Affiche la nouvelle selection */
		      if (ThotLocalActions[T_switchsel])
			(*ThotLocalActions[T_switchsel]) (frame, TRUE);
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
		    case 40: c = '('; break; /* ( */
		    case 41: c = ')'; break; /* ) */
		    case 80: c = 'P'; break; /* Pi */
		    case 83: c = 'S'; break; /* Sigma */
		    case 91: c = '['; break; /* [ */
		    case 93: c = ']'; break; /* ] */
		    case 123: c = '{'; break; /* { */
		    case 125: c = '}'; break; /* } */
		    case 172: c = '<'; break; /* Left arrow */
		    case 173: c = '^'; break; /* Top arrow */
		    case 174: c = '>'; break; /* Right arrow */
		    case 175: c = 'V'; break; /* bottom arrow */
		    case 199: c = 'I'; break; /* Inter */
		    case 200: c = 'U'; break; /* Union */
		    case 213: c = 'P'; break; /* Pi */
		    case 214: c = 'r'; break; /* Root */
		    case 229: c = 'S'; break; /* Sigma */
		    case 242: c = 'i'; break; /* Integral */
		    default: c = ' '; break;
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
	      if (toDelete)
		TtaDisplaySimpleMessage (INFO, LIB, TMSG_NOTHING_TO_DEL);
	      else
		TtaDisplaySimpleMessage (INFO, LIB, TMSG_INSERTING_IMP);
	      break;
	    }
	  SetDocumentModified (LoadedDocument[FrameTable[frame].FrDoc - 1], TRUE, 1); 
	}
    }
}


/*----------------------------------------------------------------------
   PasteXClipboard lit les nbytes du buffer et appelle autant de   
   fois que necessaire Paste_X sur les caracteres lus.     
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                PasteXClipboard (USTRING Xbuffer, int nbytes)
#else  /* __STDC__ */
void                PasteXClipboard (Xbuffer, nbytes)
USTRING     Xbuffer;
int                 nbytes;

#endif /* __STDC__ */
{
  PtrTextBuffer       clipboard;
  PtrAbstractBox      pAb;
  PtrBox              pSelBox;
  PtrTextBuffer       pBuffer;
  PtrDocument         pDoc;
  PtrElement          pEl;
  DisplayMode         dispMode;
  Document            doc;
  int                 i, j, ind;
  int                 b, previousChars;
  int                 frame, lg;
  ThotBool            lock = TRUE;

  /* check the current selection */
  if (!GetCurrentSelection (&pDoc, &pEl, &pEl, &b, &b))
    {
      /* cannot insert */
      TtaDisplaySimpleMessage (INFO, LIB, TMSG_INSERTING_IMP);
      return;
    }
  frame = ActiveFrame;
  SetInsert (&pAb, &frame, LtText, FALSE);
  if (pAb == NULL)
    {
      /* invalid selection */
      TtaDisplaySimpleMessage (INFO, LIB, TMSG_INSERTING_IMP);
      return;
    }

  /* Calcule la position du debut de la selection courante */
  /* l'index dans le buffer, le decalage x ne sont pas utilises */
  GiveInsertPoint (pAb, frame, &pSelBox, &pBuffer, &ind, &b, &previousChars);
  /* initialise l'insertion */
  previousChars += pSelBox->BxIndChar + 1;

  /* clean up the X clipboard */
  clipboard = &XClipboard;
  ClearClipboard (clipboard);
  
  /* number of characters in the last buffer of the clipboard X */
  j = 0;
  /* number of characters in the whole X clipboard */
  lg = 0;
  if (Xbuffer != NULL)
    {
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
      for (i = 0; i < nbytes; i++)
	{
	  if (j == FULL_BUFFER)
	    {
	      /* Allocate a new buffer */
	      clipboard->BuLength = j;
	      clipboard = GetNewBuffer (clipboard, ActiveFrame);
	      j = 0;
	    }
	  
	  b = (int) Xbuffer[i];
	  /* Traduit le BS, FF et blanc dur en espace */
	  if (b == 8 || b == 12 || b == 160)
	    clipboard->BuContent[j++] = SPACE;
	  if (b == EOL)
	    clipboard->BuContent[j++] = EOL;
	  /* Sinon on filtre que les caracteres imprimables */
#     ifdef _I18N_
      else if (iswprint ((CHAR_T)b))
#     else  /* !_I18N_ */
	  else if ((b >= 32 && b < 127)
		   || (b >= 177 && b < 254))
#     endif /* !_I18N_ */
	    clipboard->BuContent[j++] = Xbuffer[i];
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


/*----------------------------------------------------------------------
   TtcInsertChar insert a character                                
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtcInsertChar (Document document, View view, CHAR_T c)
#else  /* __STDC__ */
void                TtcInsertChar (document, view, c)
Document            document;
View                view;
CHAR_T                c;

#endif /* __STDC__ */
{
   ViewSelection      *pViewSel;
   PtrAbstractBox      pAb;
   DisplayMode         dispMode;
   int                 frame;
   ThotBool            lock = TRUE;

   if (document != 0)
     {
	frame = GetWindowNumber (document, view);
	pViewSel = &ViewFrameTable[frame - 1].FrSelectionBegin;
	/* Check if we are changing the active frame */
	if (frame != ActiveFrame)
	  {
	    /* yes close the previous insertion */
	    CloseTextInsertion ();
	    ActiveFrame = frame;
	  }
	if (pViewSel->VsBox != NULL &&
	    pViewSel->VsBox->BxAbstractBox != NULL &&
	    pViewSel->VsBox->BxAbstractBox->AbReadOnly)
	  {
	    if (pViewSel->VsBox->BxAbstractBox->AbEnclosing->AbReadOnly)
	      /* cannot insert here */
	      return;
	    else if (pViewSel->VsIndBox + pViewSel->VsBox->BxIndChar > 0 &&
		     pViewSel->VsIndBox + pViewSel->VsBox->BxIndChar < pViewSel->VsBox->BxAbstractBox->AbVolume)
	      /* cannot insert here */
	      return;
	  }

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

	if (!StructSelectionMode && !ViewFrameTable[frame - 1].FrSelectOnePosition)
	  {
	    /* Delete the current selection */
	    CloseTextInsertion ();
	    if (pViewSel->VsBox != NULL)
	      {
		pAb = pViewSel->VsBox->BxAbstractBox;
	      if (pAb->AbLeafType == LtPicture
		  ||  pAb->AbLeafType == LtSymbol
		  ||  pAb->AbLeafType == LtGraphics
		  ||  pAb->AbLeafType == LtText)
		ContentEditing (TEXT_SUP);
	      else if (pAb->AbLeafType != LtCompound || pAb->AbVolume != 0)
		TtcPreviousChar (document, view);
	      }
	  }
	InsertChar (frame, c, -1);
	if (!lock)
	  /* unlock table formatting */
	  (*ThotLocalActions[T_unlock]) ();
	if (dispMode == DisplayImmediately)
	  TtaSetDisplayMode (document, dispMode);
     }
}


/*----------------------------------------------------------------------
   TtcCutSelection                                                    
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtcCutSelection (Document document, View view)
#else  /* __STDC__ */
void                TtcCutSelection (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   DisplayMode         dispMode;
   ThotBool            lock = TRUE;
#ifdef _WINDOWS
   HANDLE  hMem   = 0;
   LPTSTR  lpData = 0;
   CHAR_T* ptrData;
   LPTSTR  pBuff;
   int     ndx;
   int     frame;
#endif /* _WINDOWS */

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

#ifdef _WINDOWS
   frame = GetWindowNumber (document, view);
   TtcCopyToClipboard (document, view);

   if (!OpenClipboard (FrRef[frame]))
      WinErrorBox (FrRef [frame], TEXT("TtcCutSelection (1)"));
   else {
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
      CloseClipboard ();
   }
#endif /* _WINDOWS */
   ContentEditing (TEXT_CUT);

   if (!lock)
     /* unlock table formatting */
     (*ThotLocalActions[T_unlock]) ();
   if (dispMode == DisplayImmediately)
     TtaSetDisplayMode (document, dispMode);
}

/*----------------------------------------------------------------------
   TtcDeletePreviousChar                                           
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtcDeletePreviousChar (Document document, View view)
#else  /* __STDC__ */
void                TtcDeletePreviousChar (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   ViewSelection      *pViewSel;
   DisplayMode         dispMode;
   int                 frame;
   ThotBool            delPrev, moveAfter;
   ThotBool            lock = TRUE;

   if (document != 0)
     {
       frame = GetWindowNumber (document, view);
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

       if (delPrev)
	 {
	   pViewSel = &ViewFrameTable[frame - 1].FrSelectionBegin;
	   if (pViewSel->VsBox != NULL &&
	       pViewSel->VsBox->BxAbstractBox != NULL &&
	       (!pViewSel->VsBox->BxAbstractBox->AbReadOnly ||
		pViewSel->VsBox->BxIndChar + pViewSel->VsIndBox == 0 ||
		(pViewSel->VsBox->BxIndChar + pViewSel->VsIndBox == 1 &&
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
	   if (moveAfter)
	     TtcPreviousChar (document, view);
	 }

       if (!lock)
	 /* unlock table formatting */
	 (*ThotLocalActions[T_unlock]) ();
       if (dispMode == DisplayImmediately)
	 TtaSetDisplayMode (document, dispMode);
     }
}


/*----------------------------------------------------------------------
   TtcDeleteSelection                                                 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtcDeleteSelection (Document document, View view)
#else  /* __STDC__ */
void                TtcDeleteSelection (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   DisplayMode         dispMode;
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
   
   ContentEditing (TEXT_DEL);
   
   if (!lock)
     /* unlock table formatting */
     (*ThotLocalActions[T_unlock]) ();
   if (dispMode == DisplayImmediately)
     TtaSetDisplayMode (document, dispMode);
}


/*----------------------------------------------------------------------
   TtcInclude                                                         
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtcInclude (Document document, View view)
#else  /* __STDC__ */
void                TtcInclude (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
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
	(*ThotLocalActions[T_insertpaste]) (FALSE, FALSE, TEXT('L'), &ok);

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
#ifdef __STDC__
void                TtcPasteFromClipboard (Document document, View view)
#else  /* __STDC__ */
void                TtcPasteFromClipboard (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   DisplayMode         dispMode;
   int                 frame;
#ifndef _WINDOWS
   int                 i;
   ThotWindow          w, wind;
#endif /* _WINDOWS */
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
   w = XGetSelectionOwner (TtDisplay, XA_PRIMARY);
   wind = FrRef[frame];
   if (w == None)
     {
	/* Pas de selection courante -> on regarde s'il y a un cutbuffer */
	Xbuffer = (USTRING) XFetchBytes (TtDisplay, &i);
	if (Xbuffer)
	   PasteXClipboard (Xbuffer, i);
     }
   else
      XConvertSelection (TtDisplay, XA_PRIMARY, XA_STRING, XA_CUT_BUFFER0, wind, CurrentTime);
#endif /* _WINDOWS */

   if (!lock)
     /* unlock table formatting */
     (*ThotLocalActions[T_unlock]) ();
   if (dispMode == DisplayImmediately)
     TtaSetDisplayMode (document, dispMode);
}


/*----------------------------------------------------------------------
   TtcInsert                                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtcInsert (Document document, View view)
#else  /* __STDC__ */
void                TtcInsert (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   ContentEditing (TEXT_INSERT);
}


/*----------------------------------------------------------------------
   copy                                                               
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtcCopySelection (Document document, View view)
#else  /* __STDC__ */
void                TtcCopySelection (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
#ifdef _WINDOWS
   HANDLE hMem   = 0;
   CHAR_T* lpData = 0;
   CHAR_T* ptrData;
   CHAR_T* pBuff;
   int    frame;

   /* @@@@@ */
   HWND   activeWnd;
   /* @@@@@ */

   frame = GetWindowNumber (document, view);

   /* @@@@@ */
   activeWnd = GetFocus ();
   /* @@@@@ */

   if (activeWnd != FrRef [frame]) {
      frame = -1;
      if (!OpenClipboard (activeWnd))
         WinErrorBox (FrRef [frame], TEXT("TtcCopySelection (1)"));
      else {
           EmptyClipboard ();
           SendMessage (activeWnd, WM_COPY, 0, 0);
           CloseClipboard ();
           SwitchPaste (NULL, TRUE);
	  } 
   } else {
          TtcCopyToClipboard (document, view);

          if (!OpenClipboard (FrRef[frame]))
             WinErrorBox (FrRef [frame], TEXT("TtcCopySelection (2)"));
          else {
               EmptyClipboard ();

               hMem   = GlobalAlloc (GHND, ClipboardLength + 1);
               lpData = GlobalLock (hMem);
               ptrData = lpData;
               pBuff  = Xbuffer;
			   lstrcpy (lpData, Xbuffer);
               /* for (ndx = 0; ndx < ClipboardLength; ndx++)
                   *ptrData++ = *pBuff++;
			   *ptrData = 0; */

               GlobalUnlock (hMem);

               if (!SetClipboardData (CF_TEXT, hMem))
                  WinErrorBox (NULL, TEXT(""));
               CloseClipboard ();
		  } 
   }
#endif /* _WINDOWS */
   ContentEditing (TEXT_COPY);
}


/*----------------------------------------------------------------------
   TtcPaste                                                           
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtcPaste (Document document, View view)
#else  /* __STDC__ */
void                TtcPaste (document, view)
Document            document;
View                view;
#endif /* __STDC__ */
{
   DisplayMode         dispMode;
   ThotBool            lock = TRUE;
#ifdef _WINDOWS
   HANDLE hMem;
   STRING lpData;
   int    lpDatalength;
   int    frame = GetWindowNumber (document, view);
#endif /* _WINDOWS */
   
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
#ifdef _WINDOWS
   OpenClipboard (FrRef [frame]);
   if (hMem = GetClipboardData (CF_TEXT)) {
      lpData = GlobalLock (hMem);
      lpDatalength = ustrlen (lpData);
	  if ((Xbuffer == NULL) || !sameString (Xbuffer, (USTRING)lpData))
         PasteXClipboard ((USTRING) lpData, lpDatalength);
	  else  
         ContentEditing (TEXT_PASTE);
      GlobalUnlock (hMem);
   } else 
        ContentEditing (TEXT_PASTE);
   CloseClipboard ();
#else /* _WINDOWS */
   ContentEditing (TEXT_PASTE);
#endif /* _WINDOWS */
   if (!lock)
     /* unlock table formatting */
     (*ThotLocalActions[T_unlock]) ();
   if (dispMode == DisplayImmediately)
     TtaSetDisplayMode (document, dispMode);
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
	MenuActionList[0].User_Action = (UserProc) NULL;
	MenuActionList[CMD_DeletePrevChar].Call_Action = (Proc) TtcDeletePreviousChar;
	MenuActionList[CMD_DeletePrevChar].User_Action = (UserProc) NULL;
	MenuActionList[CMD_DeleteSelection].Call_Action = (Proc) TtcDeleteSelection;
	MenuActionList[CMD_DeleteSelection].User_Action = (UserProc) NULL;

	MenuActionList[CMD_PasteFromClipboard].Call_Action = (Proc) TtcPasteFromClipboard;
	MenuActionList[CMD_PasteFromClipboard].User_Action = (UserProc) NULL;
	LastInsertCell = NULL;
	LastInsertParagraph = NULL;
	LastInsertElText = NULL;
	LastInsertAttr = NULL;
	LastInsertAttrElem = NULL;
	FromKeyboard = FALSE;
     }
}
