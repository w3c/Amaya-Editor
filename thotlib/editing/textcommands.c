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
 * Module dedicated to manage text commands.
 *
 * Authors: I. Vatton (INRIA)
 *          R. Guetari (W3C/INRIA) - Unicode and Windows version
 * Separation between structured and unstructured mode : S. Bonhomme (INRIA)
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
#include "select_tv.h"
#include "appdialogue_tv.h"

static ThotBool RightExtended;
static ThotBool LeftExtended;
static ThotBool Retry = FALSE;

#include "applicationapi_f.h"
#include "appli_f.h"
#include "tree_f.h"
#include "textcommands_f.h"
#include "editcommands_f.h"
#include "structcreation_f.h"
#include "scroll_f.h"
#include "boxmoves_f.h"
#include "boxlocate_f.h"
#include "views_f.h"
#include "callback_f.h"
#include "windowdisplay_f.h"
#include "font_f.h"
#include "geom_f.h"
#include "buildboxes_f.h"
#include "picture_f.h"
#include "abspictures_f.h"
#include "buildlines_f.h"
#include "memory_f.h"
#include "structmodif_f.h"
#include "boxparams_f.h"
#include "boxselection_f.h"
#include "structselect_f.h"
#include "content_f.h"
#include "viewapi_f.h"

/*----------------------------------------------------------------------
   IsTextLeaf teste si un pave est un pave de texte modifiable.     
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static ThotBool     IsTextLeaf (PtrAbstractBox pave)
#else  /* __STDC__ */
static ThotBool     IsTextLeaf (pave)
PtrAbstractBox      pave;

#endif /* __STDC__ */
{
   ThotBool            result;

   if (pave == NULL)
      result = FALSE;
   else
      result = (!pave->AbPresentationBox || pave->AbCanBeModified) && pave->AbLeafType == LtText;
   return result;
}


/*----------------------------------------------------------------------
   MoveInLine deplace la selection textuelle.                      
   - frame designe la fenetre de travail.                  
   - toend indique que l'on va a la fin de la ligne.       
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         MoveInLine (int frame, ThotBool toend)
#else  /* __STDC__ */
static void         MoveInLine (frame, toend)
int                 frame;
ThotBool            toend;

#endif /* __STDC__ */
{
   PtrLine             pLine;
   PtrAbstractBox      pAb;
   PtrBox              pBox;
   int                 nChars;
   ViewSelection      *pViewSel;

   pViewSel = &ViewFrameTable[frame - 1].FrSelectionBegin;
   pLine = pViewSel->VsLine;
   nChars = 0;

   if (pLine == NULL)
      return;
   else if (toend)
     {
	/* Prend la derniere boite de la ligne */
	if (pLine->LiLastPiece != NULL)
	   pBox = pLine->LiLastPiece;
	else
	   pBox = pLine->LiLastBox;

	pAb = pBox->BxAbstractBox;
	if (IsTextLeaf (pAb))
	   nChars = pBox->BxIndChar + pBox->BxNChars + 1;
     }
   else
     {
	if (pLine->LiFirstPiece != NULL)
	   pBox = pLine->LiFirstPiece;
	else
	   pBox = pLine->LiFirstBox;

	pAb = pBox->BxAbstractBox;
	if (IsTextLeaf (pAb))
	   nChars = pBox->BxIndChar + 1;
     }
   /* Reinitialise la selection */
   ChangeSelection (frame, pAb, nChars, FALSE, TRUE, FALSE, FALSE);
}


/*----------------------------------------------------------------------
   LocateLeafBox repe`re la boite terminale a' la position x+xDelta
   y+yDelta. Si la selection reste identique le decalage  
   est incremente de xDelta et yDelta.                     
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         LocateLeafBox (int frame, int x, int y, int xDelta, int yDelta, PtrBox pFrom, ThotBool extendSel)
#else  /* __STDC__ */
static void         LocateLeafBox (frame, x, y, xDelta, yDelta, pFrom, extendSel)
int                 frame;
int                 x;
int                 y;
int                 xDelta;
int                 yDelta;
PtrBox              pFrom;
ThotBool            extendSel;
#endif /* __STDC__ */
{
   int                 index;
   PtrBox              pBox, pLastBox;
   PtrTextBuffer       pBuffer;
   PtrAbstractBox      pAb;
   int                 nbbl;
   int                 nChars;

   if (pFrom != NULL)
     pLastBox = pFrom;
   else
     /* pLastBox = current selected box */
     pLastBox = ViewFrameTable[frame - 1].FrSelectionBegin.VsBox;
   /* skip presentation boxes */
   while (pLastBox->BxNext != NULL &&
	  pLastBox->BxAbstractBox->AbPresentationBox &&
	  /* constants */
	  !pLastBox->BxAbstractBox->AbCanBeModified)
     pLastBox = pLastBox->BxNext;

   pBox = GetLeafBox (pLastBox, frame, &x, &y, xDelta, yDelta);
   nChars = 0;
   if (pBox != NULL)
     {
	pAb = pBox->BxAbstractBox;
	if (pAb != NULL)
	  {
	     if (IsTextLeaf (pAb))
	       {
		  x -= pBox->BxXOrg;
		  LocateClickedChar (pBox, &pBuffer, &x, &index, &nChars, &nbbl);
		  nChars = pBox->BxIndChar + nChars + 1;
	       }
	     /* Change the selection */
	     if (extendSel)
	       ChangeSelection (frame, pAb, nChars, TRUE, TRUE, FALSE, FALSE);
	     else
	       ChangeSelection (frame, pAb, nChars, FALSE, TRUE, FALSE, FALSE);
	  }
     }
}

/*----------------------------------------------------------------------
   Commandes de deplacement                                           
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         MovingCommands (int code, Document document, View view, ThotBool extendSel)
#else  /* __STDC__ */
static void         MovingCommands (code, document, view, extendSel)
int                 code;
Document            document;
View                view;
ThotBool            extendSel;
#endif /* __STDC__ */
{
   PtrBox              pBox = NULL;
   ViewFrame          *pFrame;
   ViewSelection      *pViewSel;
   ViewSelection      *pViewSelEnd;
   int                 frame, x, y;
   int                 xDelta, yDelta;
   int                 h, w;
   int                 indpos, xpos;
   ThotBool            ok;

   indpos = 0;
   xpos = 0;
   if (code == 9)
     {
	if (ThotLocalActions[T_editfunc] != NULL)
	   (*ThotLocalActions[T_editfunc]) (TEXT_DEL);
     }
   else
     {
	CloseInsertion ();
	frame = GetWindowNumber (document, view);
	if (frame > 0)
	  {
	     pFrame = &ViewFrameTable[frame - 1];
	     pViewSel = &(pFrame->FrSelectionBegin);
	     pViewSelEnd = &(pFrame->FrSelectionEnd);
	     if (pViewSel->VsBox != NULL)
	       {
		  ok = TRUE;
		  pBox = pViewSel->VsBox;
		  indpos = pViewSel->VsIndBox;
		  xpos = pViewSel->VsXPos;
		  while (pBox != NULL && pBox->BxType == BoGhost &&
			 pBox->BxAbstractBox != NULL &&
			 pBox->BxAbstractBox->AbFirstEnclosed != NULL)
		    {
		      /* the real selection is on child elements */
		      pBox = pBox->BxAbstractBox->AbFirstEnclosed->AbBox;
		      if (code == 2)
			{
			  /* take the last child into account */
			  while (pBox->BxAbstractBox->AbNext != NULL)
			    pBox = pBox->BxAbstractBox->AbNext->AbBox;
			  while (pBox->BxAbstractBox->AbPrevious != NULL &&
				 pBox->BxAbstractBox->AbPresentationBox)
			    pBox = pBox->BxAbstractBox->AbPrevious->AbBox;
			  indpos = pBox->BxAbstractBox->AbBox->BxNChars;
			  xpos = pBox->BxAbstractBox->AbBox->BxWidth;
			}
		    }
		  GetSizesFrame (frame, &w, &h);
		  if (pBox->BxYOrg + pBox->BxHeight <= pFrame->FrYOrg ||
		      pBox->BxYOrg >= pFrame->FrYOrg + h)
		    /* l'element n'est pas visible */
		    pBox = NULL;
	       }
	     else
	       {
		 if (!Retry)
		   {
		     /* initialize a selection and retry */
		     Retry = TRUE;
		     /* look for the first leaf box */
		     if (pFrame->FrAbstractBox != NULL)
		       pBox = pFrame->FrAbstractBox->AbBox;
		     if (pBox != NULL)
		       pBox = pBox->BxNext;
		     LocateSelectionInView (frame, pBox->BxXOrg - pFrame->FrXOrg,
					    pBox->BxYOrg - pFrame->FrYOrg, 2);
		     Retry = FALSE;
		     return;
		   }
		 else
		   {
		     ok = FALSE;
		     pBox = NULL;
		   }
	       }

	     /* could we shrink the current extended selection */
	     if (extendSel)
	       {
		 if (FirstSelectedChar == LastSelectedChar &&
		     FirstSelectedElement == LastSelectedElement &&
		     SelPosition)
		   {
		     RightExtended = FALSE;
		     LeftExtended = FALSE;
		   }
		 else if (!RightExtended && !LeftExtended)
		   {
		     if (FirstSelectedElement == FixedElement && FirstSelectedChar == FixedChar)
		       {
			 RightExtended = FALSE;
			 LeftExtended = TRUE;
		       }
		     else
		       {
			 RightExtended = TRUE;
			 LeftExtended = FALSE;
		       }
		   }
	       }
	     /* doesn't change the current Shrink value in other cases */
	     switch (code)
	       {
	       case 1:	/* Backward one character (^B) */
		 if (pBox != NULL)
		   {
		     if (extendSel && RightExtended)
		       {
			 pBox = pViewSelEnd->VsBox;
			 x = pViewSelEnd->VsIndBox + pBox->BxIndChar;
		       }
		     else
		       x = indpos + pBox->BxIndChar;
		     if (x > 0)
		       {
			 if (extendSel)
			   {
			     if (RightExtended &&
				 FirstSelectedChar == LastSelectedChar &&
				 FirstSelectedElement == LastSelectedElement)
			       {
				 /* a single insert point */
				 ChangeSelection (frame, pBox->BxAbstractBox, x, FALSE, TRUE, FALSE, FALSE);
				 RightExtended = FALSE;
			       }
			     else if (SelPosition &&
				      FirstSelectedChar == LastSelectedChar &&
				      FirstSelectedElement == LastSelectedElement)
			       {
				 /* select one character */
				 ChangeSelection (frame, pBox->BxAbstractBox, x, TRUE, TRUE, FALSE, FALSE);
				 LeftExtended = TRUE;
			       }
			     else
			       /* extend the selection */
			       ChangeSelection (frame, pBox->BxAbstractBox, x, TRUE, TRUE, FALSE, FALSE);
			   }
			 else
			   /* the x is equal to FirstSelectedChar - 1 */
			   ChangeSelection (frame, pBox->BxAbstractBox, x, FALSE, TRUE, FALSE, FALSE);
		       }
		     else
		       {
			 x = pBox->BxXOrg + xpos;
			 y = pBox->BxYOrg + (pBox->BxHeight / 2);
			 xDelta = -2;
			 LocateLeafBox (frame, x, y, xDelta, 0, pBox, extendSel);
		       }
		   }
		 break;
		 
	       case 2:	/* Forward one character (^F) */
		 if (pBox != NULL)
		   {
		     if (extendSel && !LeftExtended)
		       {
			 pBox = pViewSelEnd->VsBox;
			 x = pViewSelEnd->VsIndBox + pBox->BxIndChar;
		       }
		     else
		       x = indpos + pBox->BxIndChar;
		     if (x < pBox->BxAbstractBox->AbBox->BxNChars)
		       {
			 if (extendSel)
			   {
			     if (LeftExtended &&
				 FirstSelectedChar == LastSelectedChar &&
				 FirstSelectedElement == LastSelectedElement)
			       {
				 /* a single insert point */
				 ChangeSelection (frame, pBox->BxAbstractBox, x + 1, FALSE, TRUE, FALSE, FALSE);
				 LeftExtended = FALSE;
			       }
			     else if (SelPosition &&
				      FirstSelectedChar == LastSelectedChar &&
				      FirstSelectedElement == LastSelectedElement)
			       {
				 /* select one character */
				 ChangeSelection (frame, pBox->BxAbstractBox, x + 1, TRUE, TRUE, FALSE, FALSE);
				 RightExtended = TRUE;
			       }
			     else
			       /* extend the end of the current selection */
			       ChangeSelection (frame, pBox->BxAbstractBox, x + 2, TRUE, TRUE, FALSE, FALSE);
			   }
			 else
			   ChangeSelection (frame, pBox->BxAbstractBox, x + 2, FALSE, TRUE, FALSE, FALSE);
		       }
		     else
		       {
			 x = pBox->BxXOrg + pBox->BxWidth;
			 y = pBox->BxYOrg + (pBox->BxHeight / 2);
			 xDelta = 2;
			 LocateLeafBox (frame, x, y, xDelta, 0, pBox, extendSel);
		       }
		   }
		 break;
		 
	       case 3:	/* End of line (^E) */
		 if (pBox != NULL)
		   MoveInLine (frame, TRUE);
		 break;

	       case 4:	/* Beginning of line (^A) */
		 if (pBox != NULL)
		   MoveInLine (frame, FALSE);
		 break;
		 
	       case 7:	/* Next line (^N) */
		 if (pBox != NULL)
		   {
		     if (extendSel)
		       {
			 RightExtended = TRUE;
			 pBox = pViewSelEnd->VsBox;
		       }
		     y = pBox->BxYOrg + pBox->BxHeight;
		     yDelta = 10;
		     LocateLeafBox (frame, ClickX - pFrame->FrXOrg, y, 0, yDelta, NULL, extendSel);
		     ok = FALSE;
		   }
		 else
		   TtcLineDown (document, view);
		 break;
		 
	       case 8:	/* Previous line (^P) */
		 if (pBox != NULL)
		   {
		     if (extendSel && RightExtended)
		       pBox = pViewSelEnd->VsBox;
		     y = pBox->BxYOrg;
		     yDelta = -10;
		     LocateLeafBox (frame, ClickX - pFrame->FrXOrg, y, 0, yDelta, NULL, extendSel);
		     ok = FALSE;
		   }
		 else
		   TtcLineUp (document, view);
		 break;
	       }
	     
	     /* Get the last X position */
	     if (ok)
	       {
		 if (extendSel && RightExtended && (code == 2 || code == 7))
		   ClickX = pViewSelEnd->VsBox->BxXOrg + pViewSelEnd->VsXPos - pFrame->FrXOrg;
		 else
		   ClickX = pViewSel->VsBox->BxXOrg + pViewSel->VsXPos - pFrame->FrXOrg;
	       }
	  }
     }
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtcPreviousChar (Document document, View view)
#else  /* __STDC__ */
void                TtcPreviousChar (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   MovingCommands (1, document, view, FALSE);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtcNextChar (Document document, View view)
#else  /* __STDC__ */
void                TtcNextChar (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   MovingCommands (2, document, view, FALSE);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtcPreviousLine (Document document, View view)
#else  /* __STDC__ */
void                TtcPreviousLine (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   MovingCommands (8, document, view, FALSE);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtcNextLine (Document document, View view)
#else  /* __STDC__ */
void                TtcNextLine (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   MovingCommands (7, document, view, FALSE);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtcStartOfLine (Document document, View view)
#else  /* __STDC__ */
void                TtcStartOfLine (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   MovingCommands (4, document, view, FALSE);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtcEndOfLine (Document document, View view)
#else  /* __STDC__ */
void                TtcEndOfLine (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   MovingCommands (3, document, view, FALSE);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtcPreviousSelChar (Document document, View view)
#else  /* __STDC__ */
void                TtcPreviousSelChar (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   MovingCommands (1, document, view, TRUE);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtcNextSelChar (Document document, View view)
#else  /* __STDC__ */
void                TtcNextSelChar (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   MovingCommands (2, document, view, TRUE);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtcPreviousSelLine (Document document, View view)
#else  /* __STDC__ */
void                TtcPreviousSelLine (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   MovingCommands (8, document, view, TRUE);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtcNextSelLine (Document document, View view)
#else  /* __STDC__ */
void                TtcNextSelLine (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   MovingCommands (7, document, view, TRUE);
}

/*----------------------------------------------------------------------
   CopyXClipboard insere le contenu de la selection courante dans   
   le Xbuffer pour transmettre la selection X.             
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static int          CopyXClipboard (USTRING* buffer, View view)
#else  /* __STDC__ */
static int          CopyXClipboard (buffer, view)
USTRING*            buffer;
View                view;
#endif /* __STDC__ */
{
   PtrTextBuffer       clipboard;
   PtrDocument         pDoc;
   PtrElement          pFirstEl, pLastEl;
   PtrElement          pEl;
   PtrAbstractBox      pBlock, pOldBlock;
   USTRING             Xbuffer;
   int                 i, j, max;
   int                 k, lg, maxLength;
   int                 firstChar, lastChar;
   int                 v;

   j = 0;
   /* Recupere la selection courante */
   if (!GetCurrentSelection (&pDoc, &pFirstEl, &pLastEl, &firstChar, &lastChar))
      /* Rien a copier */
      return 0;

   if (lastChar == 0)
      /* Il faut prendre tout le contenu de tout l'element */
      lastChar = pLastEl->ElVolume;

   /* Calcule la longueur du Xbuffer a produire */
   if (pFirstEl == pLastEl)
      maxLength = lastChar - firstChar;		/* un seul element */
   else
     {
	maxLength = pFirstEl->ElVolume - firstChar;	/* volume 1er element */
	pEl = pFirstEl;
	while (pEl != NULL)
	  {
	     pEl = FwdSearchTypedElem (pEl, CharString + 1, NULL);
	     if (pEl != NULL)
		if (ElemIsBefore (pLastEl, pEl))
		   /* l'element trouve' est apres l'element de fin, on */
		   /* fait comme si on n'avait pas trouve' */
		   pEl = NULL;

	     /* On ajoute le volume de l'element */
	     if (pEl != NULL)
		if (pEl == pLastEl)
		   maxLength += lastChar;
		else
		   maxLength += pEl->ElVolume;
	  }
     }

   if (maxLength == 0)
      /* Rien a recopier */
      return 0;

   /* On reserve un volant de 100 caracteres pour ajouter des CR */
   max = maxLength + 100;
   /* Alloue un Xbuffer de la longueur voulue */
   Xbuffer = TtaAllocString (max);

   if (*buffer)
     TtaFreeMemory (*buffer);
   *buffer = Xbuffer;
   /* Recopie le texte dans le Xbuffer */
   i = 0;
   lg = 0;

   /* note la vue concernee */
   if (view > 100)
     v = 0;
   else
     v = view - 1;

   pEl = pFirstEl;
   /* Teste si le premier element est de type texte */
   if (pEl->ElTerminal && pEl->ElLeafType == LtText)
     {
	clipboard = pEl->ElText;

	/* On saute les firstChar premiers caracteres */
	k = 0;
	j = clipboard->BuLength;
	while (clipboard != NULL && lg < firstChar)
	  {
	     j = clipboard->BuLength;
	     if (j > firstChar - lg)
	       {
		  /* La fin du buffer est a copier */
		  k = firstChar - lg - 1;	/* decalage dans le clipboard */
		  j -= k;	/* longueur restante dans le clipboard */
		  lg = firstChar;
	       }
	     else
	       {
		  /* Il faut sauter tout le buffer */
		  lg += j;
		  clipboard = clipboard->BuNext;
		  j = 0;
	       }
	  }

	/* Recopie le texte du premier element */
	lg = 0;
	while (clipboard != NULL && i < max && lg < maxLength)
	  {
	     if (j >= max - i)
		j = max - i - 1;	/* deborde du buffer */
	     if (j > maxLength - lg)
		j = maxLength - lg;	/* fin du texte a copier */
	     ustrncpy (&Xbuffer[i], &clipboard->BuContent[k], j);
	     i += j;
	     lg += j;
	     k = 0;
	     clipboard = clipboard->BuNext;
	     if (clipboard != NULL)
		j = clipboard->BuLength;
	  }
     }

   /* Recopie le texte des elements suivants */
   pOldBlock = NULL;
   while (pEl != NULL)
     {
	pEl = FwdSearchTypedElem (pEl, CharString + 1, NULL);
	if (pEl != NULL)
	  {
	     /* l'element trouve' est pointe' par pEl */
	     if (pEl != pLastEl)
		/* l'element trouve' n'est pas l'element ou il faut s'arreter */
		if (ElemIsBefore (pLastEl, pEl))
		   /* l'element trouve' est apres l'element de fin, on */
		   /* fait comme si on n'avait pas trouve' */
		   pEl = NULL;

	     if (pEl != NULL)
	       {
                  pBlock = SearchEnclosingType (pEl->ElAbstractBox[v], BoBlock);
		  if (i != 0 && pBlock != pOldBlock && pOldBlock != NULL)
		     /* Ajoute un \n en fin d'element */
		     ustrcpy (&Xbuffer[i++], TEXT("\n\n"));

		  /* Recopie le texte de l'element */
		  pOldBlock = pBlock;
		  clipboard = pEl->ElText;
		  while (clipboard != NULL && i < max && lg < maxLength)
		    {
		       j = clipboard->BuLength;
		       if (j >= max - i)
			  j = max - i - 1;
		       if (j > maxLength - lg)
			  j = maxLength - lg;
		       ustrncpy (&Xbuffer[i], clipboard->BuContent, j);
		       i += j;
		       lg += j;
		       clipboard = clipboard->BuNext;
		    }
	       }
	  }
     }
   Xbuffer [i] = 0;
   return i;
}

/*----------------------------------------------------------------------
   TtcCopyToClipboard                                              
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtcCopyToClipboard (Document document, View view)
#else  /* __STDC__ */
void                TtcCopyToClipboard (document, view)
Document            document;
View                view;
#endif
{
#  ifdef _WINDOWS
   ClipboardLength = CopyXClipboard (&Xbuffer, view);
#  else /* _WINDOWS */
   int                 frame;

   ThotWindow          w, wind;
   XSelectionClearEvent clear;

   /* Signale que l'on prend la selection */
   if (document == 0)
      frame = (int)FrRef[0];
   else
      frame = GetWindowNumber (document, view);

   /* Signale que l'on prend la selection */
   w = XGetSelectionOwner (TtDisplay, XA_PRIMARY);
   wind = FrRef[frame];
   if (w != None && w != wind)
     {
	clear.display = TtDisplay;
	clear.window = w;
	clear.selection = XA_PRIMARY;
	clear.time = CurrentTime;
	XSendEvent (TtDisplay, w, TRUE, NoEventMask, (ThotEvent *) & clear);
     }

   if (w != wind)
     {
	XSetSelectionOwner (TtDisplay, XA_PRIMARY, wind, CurrentTime);
	w = XGetSelectionOwner (TtDisplay, XA_PRIMARY);
	if (w != wind)
	   TtaDisplaySimpleMessage (INFO, LIB, TMSG_X_BUF_UNMODIFIED);
     }

   /* Recopie la selection courante */
   ClipboardLength = CopyXClipboard (&Xbuffer, view);
   /* Annule le cutbuffer courant */
   XStoreBuffer (TtDisplay, Xbuffer, ClipboardLength, 0);
#  endif /* _WINDOWS */
}
