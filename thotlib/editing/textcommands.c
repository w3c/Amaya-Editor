/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2001
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * This module implements text commands.
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

static ThotBool      RightExtended;
static ThotBool      LeftExtended;
static ThotBool      Retry = FALSE;
static ThotBool      Moving = FALSE;
static SearchContext WordSearchContext = {
  NULL, 0, NULL, 0, NULL, 0, TRUE, TRUE
};

/* cursor movement callback procedures for Math */
static Func     MathMoveForwardCursorFunction = NULL;
static Func     MathMoveBackwardCursorFunction = NULL;

#include "abspictures_f.h"
#include "applicationapi_f.h"
#include "appli_f.h"
#include "boxlocate_f.h"
#include "boxmoves_f.h"
#include "boxparams_f.h"
#include "boxselection_f.h"
#include "buildboxes_f.h"
#include "buildlines_f.h"
#include "callback_f.h"
#include "content_f.h"
#include "editcommands_f.h"
#include "font_f.h"
#include "geom_f.h"
#include "hyphen_f.h"
#include "memory_f.h"
#include "picture_f.h"
#include "scroll_f.h"
#include "structmodif_f.h"
#include "structcreation_f.h"
#include "structselect_f.h"
#include "textcommands_f.h"
#include "tree_f.h"
#include "viewapi_f.h"
#include "views_f.h"
#include "windowdisplay_f.h"
#include "word_f.h"


/*----------------------------------------------------------------------
   IsTextLeaf teste si un pave est un pave de texte modifiable.     
  ----------------------------------------------------------------------*/
static ThotBool     IsTextLeaf (PtrAbstractBox pave)
{
   ThotBool            result;

   if (pave == NULL)
      result = FALSE;
   else
      result = (!pave->AbPresentationBox || pave->AbCanBeModified) && pave->AbLeafType == LtText;
   return result;
}


/*----------------------------------------------------------------------
  MoveInLine moves the text selection.
  The parameter frame gives the concerned frame.
  The parameter toend is TRUE when moving to the end of the line.
  ----------------------------------------------------------------------*/
static void         MoveInLine (int frame, ThotBool toend)
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
	/* get the last box in the line */
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
   /* update the selection */
   ChangeSelection (frame, pAb, nChars, FALSE, TRUE, FALSE, FALSE);
}


/*----------------------------------------------------------------------
   LocateLeafBox gets the box located at the position x+xDelta, y+yDelta.
   if the box is still the same (pFrom) the position is shifted by
   xDelta and yDelta.                     
  ----------------------------------------------------------------------*/
static void LocateLeafBox (int frame, View view, int x, int y, int xDelta,
			   int yDelta, PtrBox pFrom, ThotBool extendSel)
{
   ViewFrame          *pFrame;
   PtrBox              pBox, pLastBox;
   PtrTextBuffer       pBuffer;
   PtrAbstractBox      pAb;
   int                 index;
   int                 nbbl;
   int                 nChars, org;
   int                 h, w, doc;

   pFrame = &ViewFrameTable[frame - 1];
   if (pFrom != NULL)
     pLastBox = pFrom;
   else
     /* pLastBox = current selected box */
     pLastBox = pFrame->FrSelectionBegin.VsBox;

   /* skip presentation boxes */
   while (pLastBox->BxNext != NULL &&
	  pLastBox->BxAbstractBox->AbPresentationBox &&
	  /* constants */
	  !pLastBox->BxAbstractBox->AbCanBeModified)
     pLastBox = pLastBox->BxNext;

   /* do we need to scroll the document */
   doc = FrameTable[frame].FrDoc;
   GetSizesFrame (frame, &w, &h);
   if (xDelta < 0 && pFrame->FrXOrg > 0 && x + xDelta < 0)
       TtcScrollLeft (doc, view);
   else if (xDelta > 0 && x + xDelta > pFrame->FrXOrg + w)
       TtcScrollRight (doc, view);
   else if (yDelta < 0 && pFrame->FrAbstractBox->AbBox->BxNext &&
	    y + yDelta < pFrame->FrYOrg + pFrame->FrAbstractBox->AbBox->BxNext->BxYOrg)
     {
       /* try to select up to the top of the current displayed frame */
       do
	 {
	   /* scroll as long as the top of the view is not reached */
	   org = pLastBox->BxYOrg;
	   TtcLineUp (doc, view);
	   /* update the new position */
	   if (org != pLastBox->BxYOrg)
	     y = y - org + pLastBox->BxYOrg;
	 }
       while (y + yDelta < pFrame->FrYOrg &&
	      /* we don't see the top of the box */
	      (y < 0 ||
	       /* the frame is not on the top of the concrete image */
	       pFrame->FrYOrg > 0 ||
	       /* we don't see the top of the document */
	       pFrame->FrAbstractBox->AbTruncatedHead));
     }
   else if (yDelta > 0 && pFrame->FrAbstractBox->AbBox->BxPrevious &&
	    (y + yDelta > pFrame->FrYOrg + h ||
	     y + yDelta > pFrame->FrAbstractBox->AbBox->BxPrevious->BxYOrg + pFrame->FrAbstractBox->AbBox->BxPrevious->BxHeight))
     {
       /* try to select down to the bottom of the current displayed frame */
       do
	 {
	   /* scroll as long as the bottom of the view is not reached */
	   org = pLastBox->BxYOrg;
	   TtcLineDown (doc, view);
	   /* update the new position */
	   if (org != pLastBox->BxYOrg)
	     y = y - org + pLastBox->BxYOrg;
	 }
       while (y + yDelta > pFrame->FrYOrg + h &&
	      /* we don't see the bottom of the box */
	      (/* the frame is not on the top of the concrete image */
	       pFrame->FrYOrg + h < pFrame->FrAbstractBox->AbBox->BxHeight ||
	       /* we don't see the top of the document */
	       pFrame->FrAbstractBox->AbTruncatedTail));
     }

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
		  LocateClickedChar (pBox, extendSel, &pBuffer, &x, &index, &nChars, &nbbl);
		  if (extendSel && LeftExtended && nChars == pBox->BxNChars)
		    /* add the last char in the left selection */
		    nChars = pBox->BxIndChar + nChars;
		  else
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
   TtaSetMoveForwardCallback permet de connecter une fonction de
   l'application a la touche de deplacement du curseur vers l'avant
  ----------------------------------------------------------------------*/
void  TtaSetMoveForwardCallback (Func callbackFunc)
{
   MathMoveForwardCursorFunction = callbackFunc;
}

/*----------------------------------------------------------------------
   TtaSetMoveBackwardCallback permet de connecter une fonction de
   l'application a la touche de deplacement du curseur vers l'arriere
  ----------------------------------------------------------------------*/
void  TtaSetMoveBackwardCallback (Func callbackFunc)
{
   MathMoveBackwardCursorFunction = callbackFunc;
}

/*----------------------------------------------------------------------
   Commandes de deplacement                                           
  ----------------------------------------------------------------------*/
static void MovingCommands (int code, Document doc, View view, ThotBool extendSel)
{
   PtrBox              pBox, pBoxBegin, pBoxEnd;
   PtrElement          pEl;
   PtrLine             pLine;
   ViewFrame          *pFrame;
   ViewSelection      *pViewSel;
   ViewSelection      *pViewSelEnd;
   CHAR_T              word[MAX_WORD_LEN];
   int                 frame, x, y, i;
   int                 xDelta, yDelta;
   int                 h, w;
   int                 indpos, xpos;
   int                 first, last;
   ThotBool            done, top = TRUE;

   indpos = 0;
   xpos = 0;
   CloseInsertion ();
   frame = GetWindowNumber (doc, view);
   if (frame > 0)
     {
       pFrame = &ViewFrameTable[frame - 1];
       /* reformat the current paragraph if necessary */
       if (ThotLocalActions[T_updateparagraph])
	 (*ThotLocalActions[T_updateparagraph]) (pFrame->FrAbstractBox, frame);
       pViewSel = &(pFrame->FrSelectionBegin);
       pViewSelEnd = &(pFrame->FrSelectionEnd);
       /* beginning of the selection */
       pBoxBegin = pViewSel->VsBox;
       if (pBoxBegin != NULL)
	 {
	   indpos = pViewSel->VsIndBox;
	   xpos = pViewSel->VsXPos;
	   while (pBoxBegin != NULL && pBoxBegin->BxType == BoGhost &&
		  pBoxBegin->BxAbstractBox != NULL &&
		  pBoxBegin->BxAbstractBox->AbFirstEnclosed != NULL)
	       /* the real selection is on child elements */
	       pBoxBegin = pBoxBegin->BxAbstractBox->AbFirstEnclosed->AbBox;
	 }

       /* end of the selection */
       pBoxEnd = pViewSelEnd->VsBox;
       if (pBoxEnd != NULL)
	 {
	   while (pBoxEnd != NULL && pBoxEnd->BxType == BoGhost &&
		  pBoxEnd->BxAbstractBox != NULL &&
		  pBoxEnd->BxAbstractBox->AbFirstEnclosed != NULL)
	     {
	       /* the real selection is on child elements */
	       pBoxEnd = pBoxEnd->BxAbstractBox->AbFirstEnclosed->AbBox;
	       /* take the last child into account */
	       while (pBoxEnd->BxAbstractBox->AbNext != NULL)
		 pBoxEnd = pBoxEnd->BxAbstractBox->AbNext->AbBox;
	       while (pBoxEnd->BxAbstractBox->AbPrevious != NULL &&
		      pBoxEnd->BxAbstractBox->AbPresentationBox)
		 pBoxEnd = pBoxEnd->BxAbstractBox->AbPrevious->AbBox;
	     }
	 }

       /* Check if boxes are visible */
       GetSizesFrame (frame, &w, &h);
       if (!RightExtended && !LeftExtended)
	 {
	   if (pBoxBegin != NULL &&
	       (pBoxBegin->BxYOrg + pBoxBegin->BxHeight <= pFrame->FrYOrg ||
		pBoxBegin->BxYOrg >= pFrame->FrYOrg + h))
	     {
	       /* the element is not displayed within the window */
	       top = pBoxBegin->BxYOrg + pBoxBegin->BxHeight <= pFrame->FrYOrg;
	       pBoxBegin = NULL;
	     }
	   if (pBoxEnd != NULL &&
	       (pBoxEnd->BxYOrg + pBoxEnd->BxHeight <= pFrame->FrYOrg ||
		pBoxEnd->BxYOrg >= pFrame->FrYOrg + h))
	     {
	       /* the element is not displayed within the window */
	       pBoxEnd = NULL;
	     }
	 }
       if (pBoxBegin == NULL && pBoxEnd != NULL)
	 pBoxBegin = pBoxEnd;
       else if (pBoxBegin != NULL && pBoxEnd == NULL)
	 pBoxEnd = pBoxBegin;

       pBox = pBoxBegin;
       if (pBox == NULL)
	 {
	   if (!Retry)
	     {
	       /* initialize a selection and retry */
	       Retry = TRUE;
	       ClickX = 0;
	       if (top)
		 ClickY = 0;
	       else
		 ClickY = h;
	       LocateSelectionInView (frame, 0, ClickY, 2);
	       Retry = FALSE;
	       return;
	     }
	   else
	     pBox = NULL;
	 }
       /* avoid to process two moving commands at the same time */
       if (Moving)
	 return;
       else
	 Moving = TRUE;
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
	       if (FirstSelectedElement == FixedElement &&
		   FirstSelectedChar == FixedChar)
		 {
		   RightExtended = TRUE;
		   LeftExtended = FALSE;
		 }
	       else
		 {
		   RightExtended = FALSE;
		   LeftExtended = TRUE;
		 }
	     }
	 }

       /* doesn't change the current Shrink value in other cases */
       switch (code)
	 {
	 case 1:	/* Backward one character (<-) */
	   if (pBox != NULL)
	     {
	     done = FALSE;
	     pEl = pBox->BxAbstractBox->AbElement;
	     if (!strcmp(pEl->ElStructSchema->SsName, "MathML") &&
		 MathMoveBackwardCursorFunction != NULL)
	       done = MathMoveBackwardCursorFunction ();
	     if (!done)
	       {
	       if (extendSel && RightExtended)
		 {
		   /* move the right extremity */
		   pBox = pBoxEnd;
		   x = pViewSelEnd->VsIndBox + pBox->BxIndChar;
		   xpos = pViewSelEnd->VsXPos;
		 }
	       else
		 /* move the left extremity */
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
			   ChangeSelection (frame, pBox->BxAbstractBox, FixedChar, FALSE, TRUE, FALSE, FALSE);
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
		   /* show the beginning of the selection */
		   if (pBoxBegin->BxXOrg + pViewSel->VsXPos + 4 < pFrame->FrXOrg)
		     {
		       if (pFrame->FrXOrg > 0)
			 TtcScrollLeft (doc, view);
		     }
		   else if (pBoxBegin->BxXOrg + pViewSel->VsXPos > pFrame->FrXOrg + w)
		     HorizontalScroll (frame, pBoxBegin->BxXOrg + pViewSel->VsXPos - pFrame->FrXOrg - w, 0);
		 }
	       else
		 {
		   /* a new box will be selected */
		   /* check if the box is within a line */
		   GetLine (&pLine);
		   if (pLine)
		     {
		       y = pBox->BxYOrg + (pBox->BxHeight / 2);
		       x = pBox->BxXOrg + xpos;
		     }
		   else
		     {
		       /* moving outside a block of lines */
		       y = pBox->BxYOrg - 2;
		       x = pBox->BxXOrg + pBox->BxWidth;
		     }
		   xDelta = -2;
		   LocateLeafBox (frame, view, x, y, xDelta, 0, pBox, extendSel);
		 }
	       }
	     }
	   /* Get the last X position */
	   ClickX = pBoxBegin->BxXOrg + pViewSel->VsXPos - pFrame->FrXOrg;
	   break;
	   
	 case 2:	/* Forward one character (->) */
	   if (pBox != NULL)
	     {
	     done = FALSE;
	     pEl = pBox->BxAbstractBox->AbElement;
	     if (!strcmp(pEl->ElStructSchema->SsName, "MathML") &&
		 MathMoveForwardCursorFunction != NULL)
	       done = MathMoveForwardCursorFunction ();
	     if (!done)
	       {
	       if (!extendSel || !LeftExtended)
		 {
		   /* move the right extremity */
		   pBox = pBoxEnd;
		   pEl = pBox->BxAbstractBox->AbElement;
		   if (!extendSel && pViewSelEnd->VsBox &&
		       pViewSelEnd->VsBox->BxType == BoGhost)
		     x = pBox->BxNChars;
		   else if ( pBox->BxAbstractBox &&
			     pBox->BxAbstractBox->AbLeafType == LtCompound)
		     x = pBox->BxNChars;
		   else if (pEl->ElStructSchema->SsRule[pEl->ElTypeNumber - 1].SrConstruct == CsConstant)
		     x =  pBox->BxNChars;
		   else
		     x = pViewSelEnd->VsIndBox + pBox->BxIndChar;
		 }
	       else
		 /* move the left extremity */
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
			   ChangeSelection (frame, pBox->BxAbstractBox, FixedChar, FALSE, TRUE, FALSE, FALSE);
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
		   /* show the beginning of the selection */
		   if (pBoxEnd->BxXOrg + pViewSelEnd->VsXPos > pFrame->FrXOrg + w)
		     {
		       if (FrameTable[frame].FrScrollOrg + FrameTable[frame].FrScrollWidth > pFrame->FrXOrg + w)
			 TtcScrollRight (doc, view);
		     }
		   else if (pBoxEnd->BxXOrg + pViewSelEnd->VsXPos - 4 < pFrame->FrXOrg)
		     HorizontalScroll (frame, pBoxEnd->BxXOrg + pViewSelEnd->VsXPos - 4 - pFrame->FrXOrg, 0);
		 }
	       else
		 {
		   /* check if the box is within a line */
		   GetLine (&pLine);
		   if (pLine)
		     {
		       y = pBox->BxYOrg + (pBox->BxHeight / 2);
		       x = pBox->BxXOrg + pBox->BxWidth;
		     }
		   else
		     {
		       /* moving ouside a block of lines */
		       y = pBox->BxYOrg + pBox->BxHeight + 2;
		       x = pBox->BxXOrg;
		     }
		   xDelta = 2;
		   LocateLeafBox (frame, view, x, y, xDelta, 0, pBox, extendSel);
		 }
	       }
	     }
	   /* Get the last X position */
	   ClickX = pBoxBegin->BxXOrg + pViewSel->VsXPos - pFrame->FrXOrg;
	   break;
	   
	 case 3:	/* End of line (^E) */
	   if (pBox != NULL)
	     MoveInLine (frame, TRUE);
	   if (pViewSel->VsBox)
	     /* Get the last X position */
	     ClickX = pViewSel->VsBox->BxXOrg + pViewSel->VsXPos - pFrame->FrXOrg;
	   break;
	   
	 case 4:	/* Beginning of line (^A) */
	   if (pBox != NULL)
	     MoveInLine (frame, FALSE);
	   if (pViewSel->VsBox)
	     /* Get the last X position */
	     ClickX = pViewSel->VsBox->BxXOrg + pViewSel->VsXPos - pFrame->FrXOrg;
	   break;
	   
	 case 7:	/* Next line (^N) */
	   if (pBox != NULL)
	     {
	       pBox = pBoxEnd;
	       x = pViewSelEnd->VsXPos + pBox->BxXOrg;
	       if (pViewSelEnd->VsBuffer && pViewSelEnd->VsIndBuf < pViewSelEnd->VsBuffer->BuLength)
		 x -= CharacterWidth (pViewSelEnd->VsBuffer->BuContent[pViewSelEnd->VsIndBuf], pBox->BxFont);
	       y = pBox->BxYOrg + pBox->BxHeight;
	       yDelta = 10;
	       /* store the end position of the selection as the new reference */
	       if (extendSel && LeftExtended)
		 {
		   i = pBoxBegin->BxYOrg - pBoxEnd->BxYOrg;
		   if (i < 5 && i > -5)
		     {
		       /* change the extension direction */
		       LeftExtended = FALSE;
		       RightExtended = TRUE;
		     }
		   else
		     {
		       /* just decrease the current extension */
		       y = pBoxBegin->BxYOrg;
		       x = ClickX + pFrame->FrXOrg;
		       pBox = pBoxBegin;
		     }
		 }
	       /* there was a drag, but it's finished now */
	       else if (!extendSel)
		 {
		   if (!SelPosition &&
		       (pBoxBegin != pBoxEnd ||
			!IsConstantConstructor (pBox->BxAbstractBox->AbElement)))
		     /* changing from an extension to a simple selection */
		     ClickX = x - pFrame->FrXOrg;
		   else
		     /* take the original position into account */
		     x = ClickX + pFrame->FrXOrg;
		 }
	       else
		 RightExtended = TRUE;

	       LocateLeafBox (frame, view, x, y, 0, yDelta, pBox, extendSel);
	     }
	   break;
	   
	 case 8:	/* Previous line (^P) */
	   if (pBox != NULL)
	     {
	       y = pBoxBegin->BxYOrg;
	       x = ClickX + pFrame->FrXOrg;
	       yDelta = -10;
	       if (extendSel && RightExtended)
		 {
		   i = pBoxBegin->BxYOrg - pBoxEnd->BxYOrg;
		   if (i < 5 && i > -5)
		     {
		       /* change the extension direction */
		       RightExtended = FALSE;
		       LeftExtended = TRUE;
		     }
		   else
		     {
		       /* just decrease the curent extension */
		       y = pBoxEnd->BxYOrg;
		       x = pViewSelEnd->VsXPos + pBoxEnd->BxXOrg;
		       pBox = pBoxEnd;
		     }
		 }
	       else if (extendSel)
		 LeftExtended = TRUE;

	       LocateLeafBox (frame, view, x, y, 0, yDelta, pBox, extendSel);
	     }
	   break;
	   
	 case 9:	/* Previous word (^<-) */
	   WordSearchContext.SDocument = LoadedDocument[doc - 1];
	   WordSearchContext.SStartToEnd = FALSE;
	   if (RightExtended)
	     {
	       /* shrink the current selection */
	       first = LastSelectedChar;
	       pEl = LastSelectedElement;
	     }
	   else
	     {
	       /* extend the current selection */
	       first = FirstSelectedChar - 1;
	       pEl = FirstSelectedElement;
	     }
	   done = SearchPreviousWord (&pEl, &first, &last, word, &WordSearchContext);
	   if ((RightExtended && last >= LastSelectedChar) ||
	       (LeftExtended && first == FirstSelectedChar - 1))
	     /* It was not the beginning of the next word */
	     done = SearchPreviousWord (&pEl, &first, &last, word, &WordSearchContext);
	   if (extendSel)
	     {
	       if (RightExtended && FirstSelectedElement == FixedElement &&
		   last <= FixedChar)
		 {
		 /* change the extension direction */
		   RightExtended = FALSE;
		   LeftExtended = TRUE;
		 }
	       if (LeftExtended)
		 i = first + 1;
	       else
		 i = last;
	       if (pEl->ElAbstractBox[view - 1])
		 ChangeSelection (frame, pEl->ElAbstractBox[view - 1], i, TRUE, TRUE, FALSE, FALSE);
	     }
	   else
	     {
	       SelectString (LoadedDocument[doc - 1], pEl, first + 1, first);
	       /* remove the extension direction */
	       LeftExtended = FALSE;
	     }
	   break;
	   
	 case 10:	/* Next word (^->) */
	   WordSearchContext.SDocument =  LoadedDocument[doc - 1];
	   WordSearchContext.SStartToEnd = TRUE;
	   if (LeftExtended)
	     {
	       /* shrink the current selection */
	       last = FirstSelectedChar;
	       pEl = FirstSelectedElement;
	     }
	   else
	     {
	       /* extend the current selection */
	       last = LastSelectedChar;
	       pEl = LastSelectedElement;
	     }
	   done = SearchNextWord (&pEl, &first, &last, word, &WordSearchContext);
	   if ((!LeftExtended && first == LastSelectedChar) ||
	       (LeftExtended && first >= FirstSelectedChar))
	     /* It was not the beginning of the next word */
	     done = SearchNextWord (&pEl, &first, &last, word, &WordSearchContext);
	   if (extendSel)
	     {
	       if (LeftExtended && FirstSelectedElement == FixedElement &&
		   first >= FixedChar)
		 {
		 /* change the extension direction */
		   RightExtended = TRUE;
		   LeftExtended = FALSE;
		 }
	       if (LeftExtended)
		 i = first + 1;
	       else
		 i = last;
	       if (pEl->ElAbstractBox[view - 1])
		 ChangeSelection (frame, pEl->ElAbstractBox[view - 1], i, TRUE, TRUE, FALSE, FALSE);
	     }
	   else
	     {
	       SelectString (LoadedDocument[doc - 1], pEl, first + 1, first);
	       /* remove the extension direction */
	       LeftExtended = FALSE;
	     }
	   break;
	 }
       Moving = FALSE;
     }
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void                TtcPreviousChar (Document document, View view)
{
   MovingCommands (1, document, view, FALSE);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void                TtcNextChar (Document document, View view)
{
   MovingCommands (2, document, view, FALSE);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void                TtcPreviousLine (Document document, View view)
{
   MovingCommands (8, document, view, FALSE);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void                TtcNextLine (Document document, View view)
{
   MovingCommands (7, document, view, FALSE);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void                TtcStartOfLine (Document document, View view)
{
   MovingCommands (4, document, view, FALSE);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void                TtcEndOfLine (Document document, View view)
{
   MovingCommands (3, document, view, FALSE);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void                TtcPreviousSelChar (Document document, View view)
{
   MovingCommands (1, document, view, TRUE);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void                TtcNextSelChar (Document document, View view)
{
   MovingCommands (2, document, view, TRUE);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void                TtcPreviousSelLine (Document document, View view)
{
   MovingCommands (8, document, view, TRUE);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void                TtcNextSelLine (Document document, View view)
{
   MovingCommands (7, document, view, TRUE);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void                TtcPreviousWord (Document document, View view)
{
   MovingCommands (9, document, view, FALSE);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void                TtcNextWord (Document document, View view)
{
   MovingCommands (10, document, view, FALSE);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void                TtcPreviousSelWord (Document document, View view)
{
   MovingCommands (9, document, view, TRUE);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void                TtcNextSelWord (Document document, View view)
{
   MovingCommands (10, document, view, TRUE);
}

/*----------------------------------------------------------------------
   CopyXClipboard insere le contenu de la selection courante dans   
   le Xbuffer pour transmettre la selection X.             
  ----------------------------------------------------------------------*/
static int          CopyXClipboard (USTRING *buffer, View view)
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
   ThotBool            inAttr;

   j = 0;
   inAttr = FALSE;
   /* Recupere la selection courante */
   if (!GetCurrentSelection (&pDoc, &pFirstEl, &pLastEl, &firstChar, &lastChar))
      /* Rien a copier */
      return 0;

   lg = 1;
   if (lastChar == 0)
      /* Il faut prendre tout le contenu de tout l'element */
      lastChar = pLastEl->ElVolume;

   /* Calcule la longueur du Xbuffer a produire */
   if (pFirstEl == pLastEl)
     /* only one element */
      maxLength = lastChar - firstChar;
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
	       {
		 if (pEl == pLastEl)
		   maxLength += lastChar;
		 else
		   maxLength += pEl->ElVolume;
		 lg += 2;
	       }
	  }
     }

   if (pDoc == DocSelectedAttr)
     {
       /* Selection is within an attribute */
       firstChar = FirstSelectedCharInAttr;
       lastChar = LastSelectedCharInAttr;
       maxLength = LastSelectedCharInAttr - FirstSelectedCharInAttr;
       inAttr = TRUE;
       clipboard = AbsBoxSelectedAttr->AbText;
     }
   else if (maxLength == 0)
     /* nothing selected */
     return 0;
   else if (pFirstEl->ElTerminal && pFirstEl->ElLeafType == LtText)
     clipboard = pFirstEl->ElText;
   else
     clipboard = NULL;

   /* Adding 100 characters for extra CR */
   max = maxLength + lg;
   /* Allocate the Xbuffer with the right length */
   Xbuffer = TtaAllocString (max);

   if (*buffer)
     TtaFreeMemory (*buffer);
   *buffer = Xbuffer;
   /* Copy the text into the Xbuffer */
   i = 0;
   lg = 0;

   /* get the current view */
   if (view > 100)
     v = 0;
   else
     v = view - 1;

   /* Teste si le premier element est de type texte */
   if (clipboard)
     {
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
   pEl = pFirstEl;
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
		     ustrcpy (&Xbuffer[i++], "\n\n");

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
void TtcCopyToClipboard (Document doc, View view)
{
#ifndef _WINDOWS
   int                  frame;
   ThotWindow           w, wind;
   XSelectionClearEvent clear;

   /* Signale que l'on prend la selection */
   if (doc == 0)
      frame = (int)FrRef[0];
   else
      frame = GetWindowNumber (doc, view);

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

#endif /* _WINDOWS */
   /* Recopie la selection courante */
   ClipboardLength = CopyXClipboard (&Xbuffer, view);
#ifndef _WINDOWS
   /* Annule le cutbuffer courant */
   XStoreBuffer (TtDisplay, Xbuffer, ClipboardLength, 0);
#endif /* _WINDOWS */
}


/*----------------------------------------------------------------------
  SelectCurrentWord
  selects the word at the current position.
  The current position is given by the current box, the current buffer
  and the index in the buffer.
  ----------------------------------------------------------------------*/
void SelectCurrentWord (int frame, PtrBox pBox, int pos, int index,
			PtrTextBuffer pBuffer, ThotBool inClipboard)
{
  PtrTextBuffer       buffer;
  PtrDocument         pDoc;
  PtrAbstractBox      pAb;
  UCHAR_T              c;
  int                 first, last;
  int                 doc, i;
  ThotBool            isSep;

  doc = FrameTable[frame].FrDoc;
  pAb = pBox->BxAbstractBox;
  if (frame >= 1 && doc > 0 && index > 0)
    {
      /* check if a leaf box is selected */
      index--;
      c = pBuffer->BuContent[index];
      if (c != WC_SPACE && c != WC_EOS)
	{
	  /* look for the beginning of the word */
	  buffer = pBuffer;
	  first = pos;
	  i = index;
	  c = PreviousCharacter (&buffer, &i);
	  isSep = IsSeparatorChar (c);
	  while (first > 1 && !isSep && c != WC_EOS)
	    {
	      first--;
	      c = PreviousCharacter (&buffer, &i);
	      isSep = IsSeparatorChar (c);
	    }
	  /* look for the beginning of the word */
	  buffer = pBuffer;
	  last = pos;
	  i = index - 1;
	  c = NextCharacter (&buffer, &i);
	  isSep = IsSeparatorChar (c);
	  while (!isSep && c != WC_EOS)
	    {
	      last++;
	      c = NextCharacter (&buffer, &i);
	      isSep = IsSeparatorChar (c);
	    }
	  /*while (c == WC_SPACE)
	    {
	    last++;
	    c = NextCharacter (&buffer, &i);
	    }*/
	  if (isSep)
	    last--;
	  pDoc = LoadedDocument[doc - 1];
	  SelectString (pDoc, pAb->AbElement, first, last);
	  if (inClipboard)
	    ClipboardLength = CopyXClipboard (&Xbuffer, pAb->AbDocView);
	}
    }
}

/*----------------------------------------------------------------------
  TtaSelectWord selects the word around the current point in an element
  ----------------------------------------------------------------------*/
void TtaSelectWord (Element element, int pos, Document doc, View view)
{
  PtrElement       pEl = (PtrElement)element;
  PtrAbstractBox   pAb;
  PtrBox           pBox;
  PtrTextBuffer    pBuffer;
  int              v, index;
  int              frame, i;

  if (pEl->ElTerminal && pEl->ElLeafType == LtText)
    {
      /* get the abstract box of the element */
      if (view > 100)
	v = 0;
      else
	v = view - 1;
      pAb = pEl->ElAbstractBox[v];
      if (pAb && pAb->AbBox)
	{
	  /* look for the box, the buffer and the index in the buffer */
	  pBox = pAb->AbBox;
	  i = pos;
	  if (pBox->BxType == BoSplit)
	    {
	      pBox = pBox->BxNexChild;
	      while (pBox && pBox->BxNChars < i && pBox->BxNexChild)
		{
		  i = i - pBox->BxNChars;
		  /* remove the delta between two boxes */
		  i = i - pBox->BxNexChild->BxIndChar + pBox->BxNChars + pBox->BxIndChar;
		  pBox = pBox->BxNexChild;
		}
	    }
	  pBuffer = pBox->BxBuffer;
	  index =  pBox->BxFirstChar;
	  while (i > pBuffer->BuLength - index + 1)
	    {
	      i = i - pBuffer->BuLength + index - 1;
	      index = 1;
	      pBuffer = pBuffer->BuNext;
	    }
	  frame = GetWindowNumber (doc, view);
	  SelectCurrentWord (frame, pBox, pos, i, pBuffer, FALSE);
	}
    }
}
