/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2003
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

static ThotBool      RightExtended = FALSE;
static ThotBool      LeftExtended = FALSE;
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
#include "ustring_f.h"
#include "viewapi_f.h"
#include "views_f.h"
#include "windowdisplay_f.h"
#include "word_f.h"


/*----------------------------------------------------------------------
   IsTextLeaf teste si un pave est un pave de texte modifiable.     
  ----------------------------------------------------------------------*/
static ThotBool IsTextLeaf (PtrAbstractBox pave)
{
   ThotBool            result;

   if (pave == NULL)
      result = FALSE;
   else
      result = (!pave->AbPresentationBox || pave->AbCanBeModified) &&
	pave->AbLeafType == LtText;
   return result;
}


/*----------------------------------------------------------------------
  MoveInLine moves the text selection.
  The parameter frame gives the concerned frame.
  The parameter toend is TRUE when moving to the end of the line.
  The parameter extendSel is TRUE when the selection is extended.
  ----------------------------------------------------------------------*/
static void MoveInLine (int frame, ThotBool toend, ThotBool extendSel)
{
   PtrLine             pLine;
   PtrAbstractBox      pAb;
   PtrBox              pBox, ibox;
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
	if (pLine->LiLastPiece)
	   pBox = pLine->LiLastPiece;
	else
	   pBox = pLine->LiLastBox;
	ibox = pBox;
	while (ibox && ibox->BxAbstractBox->AbPresentationBox &&
	       ibox != pLine->LiFirstPiece && ibox != pLine->LiFirstBox)
	  /* try to select a box which is not a presentation box */
	  ibox = ibox->BxPrevious;
	if (ibox && ibox != pBox)
	  pBox = ibox;
	pAb = pBox->BxAbstractBox;
	if (IsTextLeaf (pAb))
	   nChars = pBox->BxFirstChar + pBox->BxNChars;
     }
   else
     {
	if (pLine->LiFirstPiece)
	   pBox = pLine->LiFirstPiece;
	else
	   pBox = pLine->LiFirstBox;
	ibox = pBox;
	while (ibox && ibox->BxAbstractBox->AbPresentationBox &&
	       ibox != pLine->LiLastPiece && ibox != pLine->LiLastBox)
	  /* try to select a box which is not a presentation box */
	  ibox = ibox->BxNext;
	if (ibox && ibox != pBox)
	  pBox = ibox;
	pAb = pBox->BxAbstractBox;
	if (IsTextLeaf (pAb))
	   nChars = pBox->BxFirstChar;
     }
   /* update the selection */
   ChangeSelection (frame, pAb, nChars, extendSel, !toend, FALSE, FALSE);
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
   PtrBox              pBox, endBox;
   PtrTextBuffer       pBuffer;
   PtrAbstractBox      pAb;
   int                 index;
   int                 nbbl;
   int                 nChars, org;
   int                 h, w, doc;

   pFrame = &ViewFrameTable[frame - 1];
   if (pFrom != NULL)
     endBox = pFrom;
   else
     /* endBox = current selected box */
     endBox = pFrame->FrSelectionBegin.VsBox;

   /* skip presentation boxes */
   while (endBox->BxNext != NULL &&
	  endBox->BxAbstractBox->AbPresentationBox &&
	  /* constants */
	  !endBox->BxAbstractBox->AbCanBeModified)
     endBox = endBox->BxNext;

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
	   org = endBox->BxYOrg;
	   TtcLineUp (doc, view);
	   /* update the new position */
	   if (org != endBox->BxYOrg)
	     y = y - org + endBox->BxYOrg;
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
	   org = endBox->BxYOrg;
	   TtcLineDown (doc, view);
	   /* update the new position */
	   if (org != endBox->BxYOrg)
	     y = y - org + endBox->BxYOrg;
	 }
       while (y + yDelta > pFrame->FrYOrg + h &&
	      /* we don't see the bottom of the box */
	      (/* the frame is not on the top of the concrete image */
	       pFrame->FrYOrg + h < pFrame->FrAbstractBox->AbBox->BxHeight ||
	       /* we don't see the top of the document */
	       pFrame->FrAbstractBox->AbTruncatedTail));
     }

   if ( RightExtended && yDelta < 0 &&
	(endBox->BxFirstChar + endBox->BxNChars == LastSelectedChar ||
	 (endBox->BxFirstChar + endBox->BxNChars == LastSelectedElement->ElVolume + 1 &&
	  LastSelectedChar == 0)) &&
	x < endBox->BxXOrg + endBox->BxH + endBox->BxLMargin + endBox->BxLBorder + endBox->BxLPadding)
     /* stay in the same box */
     pBox = endBox;
   else
     pBox = GetLeafBox (endBox, frame, &x, &y, xDelta, yDelta);
   if (pBox)
     {
	pAb = pBox->BxAbstractBox;
	if (pAb)
	  {
	     if (IsTextLeaf (pAb))
	       {
		  x -= pBox->BxXOrg;
		  LocateClickedChar (pBox, extendSel, &pBuffer, &x, &index,
				     &nChars, &nbbl);
		  if (LeftExtended && nChars > 0 && nChars <= pBox->BxNChars)
		    nChars--;
		  nChars = pBox->BxFirstChar + nChars;
		  if (extendSel && yDelta > 0 &&
			   pAb->AbElement == LastSelectedElement &&
			   LastSelectedElement &&
			   nChars <= LastSelectedChar)
		    nChars = LastSelectedElement->ElVolume + 1;
	       }
	     else
	       nChars = 0;
	     /* Change the selection */
	     if (extendSel)
	       ChangeSelection (frame, pAb, nChars, TRUE, LeftExtended, FALSE, FALSE);
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
static void MovingCommands (int code, Document doc, View view,
			    ThotBool extendSel)
{
  PtrBox              pBox, pBoxBegin, pBoxEnd, ibox;
  PtrElement          pEl = NULL, firstEl, lastEl;
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
  int                 firstC, lastC;
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
      if (pBoxBegin)
	{
	  xpos = pViewSel->VsXPos;
	  while (pBoxBegin && pBoxBegin->BxType == BoGhost &&
		 pBoxBegin->BxAbstractBox &&
		 pBoxBegin->BxAbstractBox->AbFirstEnclosed)
	    /* the real selection is on child elements */
	    pBoxBegin = pBoxBegin->BxAbstractBox->AbFirstEnclosed->AbBox;
	}
      
      /* end of the selection */
      pBoxEnd = pViewSelEnd->VsBox;
      if (pBoxEnd != NULL)
	{
	  while (pBoxEnd != NULL && pBoxEnd->BxType == BoGhost &&
		 pBoxEnd->BxAbstractBox &&
		 pBoxEnd->BxAbstractBox->AbFirstEnclosed)
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
      if (DocSelectedAttr)
	{
	  /* work within an attribute */
	  firstC = FirstSelectedCharInAttr;
	  lastC = LastSelectedCharInAttr;
	  firstEl = NULL;
	  lastEl = NULL;
	}
      else
	{
	  firstC = FirstSelectedChar;
	  lastC = LastSelectedChar;
	  firstEl = FirstSelectedElement;
	  lastEl = LastSelectedElement;
	  if (firstC == lastC && firstC == 0 && firstEl == lastEl)
	    lastC = firstEl->ElVolume + 1;
	}
      /* could we shrink the current extended selection */
      if (extendSel)
	{
	  if (firstC == lastC &&
	      firstEl == lastEl &&
	      SelPosition)
	    {
	      RightExtended = FALSE;
	      LeftExtended = FALSE;
	    }
	  else if (!RightExtended && !LeftExtended)
	    {
	      if (firstEl == FixedElement &&
		  firstC == FixedChar)
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
	  if (pBox)
	    {
	      done = FALSE;
	      pEl = pBox->BxAbstractBox->AbElement;
	      if (pBox->BxAbstractBox->AbPresentationBox)
		/* the selection is within an attribute value */
		done = FALSE;
	      else if (!strcmp(pEl->ElStructSchema->SsName, "MathML") &&
		       MathMoveBackwardCursorFunction != NULL)
		done = MathMoveBackwardCursorFunction ();
	      else if (!strcmp (pEl->ElStructSchema->SsName, "SVG") &&
		       (!pEl->ElTerminal || pEl->ElLeafType != LtText))
		{
		  TtcPreviousElement (doc, view);
		  done = TRUE;
		}
	      if (!done)
		{
		  if (extendSel && RightExtended)
		    {
		      /* move the right extremity */
		      pBox = pBoxEnd;
		      x = lastC;
		      xpos = pViewSelEnd->VsXPos;
		    }
		  else
		    /* move the left extremity */
		    x = firstC;
		  if (x > 1)
		    {
		      if (extendSel)
			{
			  if (RightExtended && firstC == lastC - 1 &&
			      firstEl == lastEl)
			    {
			      /* a single insert point */
			      ChangeSelection (frame, pBox->BxAbstractBox, FixedChar,
					       FALSE, TRUE, FALSE, FALSE);
			      RightExtended = FALSE;
			    }
			  else if (SelPosition &&
				   firstC == lastC &&
				   firstEl == lastEl)
			    {
			      /* select one character */
			      LeftExtended = TRUE;
			      ChangeSelection (frame, pBox->BxAbstractBox, x - 1,
					       TRUE, LeftExtended, FALSE, FALSE);
			    }
			  else
			    /* extend the selection */
			    ChangeSelection (frame, pBox->BxAbstractBox, x - 1,
					     TRUE, LeftExtended, FALSE, FALSE);
			}
		      else
			/* the x is equal to first */
			ChangeSelection (frame, pBox->BxAbstractBox, x - 1,
					 FALSE, TRUE, FALSE, FALSE);
		      /* show the beginning of the selection */
		      if (pBoxBegin->BxXOrg + pViewSel->VsXPos + 4 < pFrame->FrXOrg)
			{
			  if (pFrame->FrXOrg > 0)
			    TtcScrollLeft (doc, view);
			}
		      else if (pBoxBegin->BxXOrg + pViewSel->VsXPos > pFrame->FrXOrg + w)
			HorizontalScroll (frame, pBoxBegin->BxXOrg + pViewSel->VsXPos - pFrame->FrXOrg - w, 0);
		    }
		  else if (pBox->BxPrevious)
		    {
		      /* a new box before will be selected */
		      ibox = pBox->BxPrevious;
		      while (ibox &&
			     ibox->BxAbstractBox->AbPresentationBox)
			ibox = ibox->BxPrevious;
		      if (ibox)
			{
			  /* check if boxes are within the same line */
			  pLine = SearchLine (pBox);
			  if (pLine && pLine == SearchLine (ibox))
			    {
			      /* moving to the beginning of the current box */
			      y = pBox->BxYOrg + (pBox->BxHeight / 2);
			      x = pBox->BxXOrg + xpos;
			      if (pBox->BxScript == 'A' ||
				  pBox->BxScript == 'H')
				x += pBox->BxWidth + 2;
			    }
			  else
			    {
			      /* moving to the end of the next box */
			      y = ibox->BxYOrg + ibox->BxHeight;
			      x = ibox->BxXOrg;
			      if (ibox->BxScript != 'A' &&
				  ibox->BxScript != 'H')
				x += ibox->BxWidth + 2;
			    }
			  xDelta = -2;
			  LocateLeafBox (frame, view, x, y, xDelta, 0, pBox, extendSel);
			}
		    }
		}
	    }
	  /* Get the last X position */
	  ClickX = pBoxBegin->BxXOrg + pViewSel->VsXPos - pFrame->FrXOrg;
	  break;
	  
	case 2:	/* Forward one character (->) */
	  if (pBox)
	    {
	      done = FALSE;
	      pEl = pBox->BxAbstractBox->AbElement;
	      if (pBox->BxAbstractBox->AbPresentationBox)
		/* the selection is within an attribute value */
		done = FALSE;
	      else if (!strcmp(pEl->ElStructSchema->SsName, "MathML") &&
		       MathMoveForwardCursorFunction != NULL)
		done = MathMoveForwardCursorFunction ();
	      else if (!strcmp (pEl->ElStructSchema->SsName, "SVG") &&
		       (!pEl->ElTerminal || pEl->ElLeafType != LtText))
		{
		  TtcNextElement (doc, view);
		  done = TRUE;
		}
	      if (!done)
		{
		  if (!extendSel || !LeftExtended)
		    {
		      /* move the right extremity */
		      pBox = pBoxEnd;
		      pEl = pBox->BxAbstractBox->AbElement;
		      if ((!extendSel && pViewSelEnd->VsBox &&
			  pViewSelEnd->VsBox->BxType == BoGhost) ||
		      (pBox->BxAbstractBox &&
				pBox->BxAbstractBox->AbLeafType == LtCompound) ||
		      (pEl->ElStructSchema->SsRule->SrElem[pEl->ElTypeNumber - 1]->SrConstruct == CsConstant))
			  {
			  if (pBox->BxNChars)
			    x =  pBox->BxNChars;
			  else
			  x = 1;
			  }
		      else
			x = lastC;
		    }
		  else
		    /* move the left extremity */
		    x = firstC;
		  if (x <= pBox->BxAbstractBox->AbBox->BxNChars)
		    {
		      if (extendSel)
			{
			  if (LeftExtended && firstC == lastC - 1 &&
			      firstEl == lastEl)
			    {
			      /* a single insert point */
			      LeftExtended = FALSE;
			      ChangeSelection (frame, pBox->BxAbstractBox, FixedChar,
					       FALSE, TRUE, FALSE, FALSE);
			    }
			  else if (SelPosition &&
				   firstC == lastC &&
				   firstEl == lastEl)
			    {
			      /* select one character */
			      RightExtended = TRUE;
			      ChangeSelection (frame, pBox->BxAbstractBox, x + 1,
					       TRUE, LeftExtended, FALSE, FALSE);
			    }
			  else
			    /* extend the end of the current selection */
			    ChangeSelection (frame, pBox->BxAbstractBox, x + 1,
					     TRUE, LeftExtended, FALSE, FALSE);
			}
		      else
			ChangeSelection (frame, pBox->BxAbstractBox, x + 1,
					 FALSE, TRUE, FALSE, FALSE);
		      /* show the end of the selection */
		      if (pBoxEnd->BxXOrg + pViewSelEnd->VsXPos > pFrame->FrXOrg + w)
			{
			  if (FrameTable[frame].FrScrollOrg + FrameTable[frame].FrScrollWidth > pFrame->FrXOrg + w)
			    TtcScrollRight (doc, view);
			}
		      else if (pBoxEnd->BxXOrg + pViewSelEnd->VsXPos - 4 < pFrame->FrXOrg)
			HorizontalScroll (frame, pBoxEnd->BxXOrg + pViewSelEnd->VsXPos - 4 - pFrame->FrXOrg, 0);
		    }
		  else if (pBox->BxNext)
		    {
		      /* a new box after will be selected */
		      ibox = pBox->BxNext;
		      while (ibox &&
			     ibox->BxAbstractBox->AbPresentationBox)
			ibox = ibox->BxNext;
		      if (ibox)
			{
			  /* check if boxes are within the same line */
			  pLine = SearchLine (pBox);
			  if (pLine && pLine == SearchLine (ibox))
			    {
			      /* moving to the end of the current box */
			      y = pBox->BxYOrg + (pBox->BxHeight / 2);
			      x = pBox->BxXOrg;
			      if (pBox->BxScript != 'A' &&
				  pBox->BxScript != 'H')
				x += pBox->BxWidth;
			      else
				x -= 2;
			    }
			  else
			    {
			      /* moving to the beginning of the next box */
			      y = ibox->BxYOrg;
			      x = ibox->BxXOrg;
			      if (ibox->BxScript == 'A' ||
				  ibox->BxScript == 'H')
				x += ibox->BxWidth;
			      else
				x -= 2;
			    }
			  xDelta = 2;
			  LocateLeafBox (frame, view, x, y, xDelta, 0, pBox, extendSel);
			}
		    }
		}
	    }
	  /* Get the last X position */
	  ClickX = pBoxBegin->BxXOrg + pViewSel->VsXPos - pFrame->FrXOrg;
	  break;

	case 3:	/* End of line (^E) */
	  if (pBox)
	    MoveInLine (frame, TRUE, extendSel);
	  if (pViewSel->VsBox)
	    /* Get the last X position */
	    ClickX = pViewSel->VsBox->BxXOrg + pViewSel->VsXPos - pFrame->FrXOrg;
	  break;
	   
	case 4:	/* Beginning of line (^A) */
	  if (pBox)
	    MoveInLine (frame, FALSE, extendSel);
	  if (pViewSel->VsBox)
	    /* Get the last X position */
	    ClickX = pViewSel->VsBox->BxXOrg + pViewSel->VsXPos - pFrame->FrXOrg;
	  break;
	   
	case 7:	/* Next line (^N) */
	  if (pBox)
	    pEl = pBox->BxAbstractBox->AbElement;
	  if (pBox && !pBox->BxAbstractBox->AbPresentationBox &&
	      pEl && !strcmp (pEl->ElStructSchema->SsName, "SVG") &&
	      (!pEl->ElTerminal || pEl->ElLeafType != LtText))
	    /* the selection is not  within an attribute value */
	    TtcNextElement (doc, view);
	  else if (pBox)
	    {
	      done = FALSE;
	      pBox = pBoxEnd;
	      if (SelPosition || RightExtended)
		x = pViewSel->VsXPos + pBoxBegin->BxXOrg;
	      else
		x = pViewSelEnd->VsXPos + pBox->BxXOrg;
	      y = pBox->BxYOrg + pBox->BxHeight;
	      yDelta = 10;
	      /* store the end position of the selection as the new reference */
	      if (extendSel && LeftExtended)
		{
		  i = pBoxEnd->BxYOrg - pBoxBegin->BxYOrg;
		  if (i < 15 /* less than 150% of yDelta */)
		    {
		      /* no more extension */
		      LeftExtended = FALSE;
		      extendSel = FALSE;
		      ChangeSelection (frame, pBox->BxAbstractBox, FixedChar,
				       FALSE, TRUE, FALSE, FALSE);
		      done = TRUE;
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
	      if (!done)
		LocateLeafBox (frame, view, x, y, 0, yDelta, pBox, extendSel);
	    }
	  break;
	   
	case 8:	/* Previous line (^P) */
	  if (pBox)
	    pEl = pBox->BxAbstractBox->AbElement;
	  if (pBox && !pBox->BxAbstractBox->AbPresentationBox &&
	      pEl && !strcmp (pEl->ElStructSchema->SsName, "SVG") &&
	      (!pEl->ElTerminal || pEl->ElLeafType != LtText))
	    TtcPreviousElement (doc, view);
	  else if (pBox)
	    {
	      done = FALSE;
	      y = pBoxBegin->BxYOrg;
	      if (SelPosition || RightExtended)
		x = pViewSel->VsXPos + pBoxBegin->BxXOrg;
	      else
		x = pViewSelEnd->VsXPos + pBoxEnd->BxXOrg;
	      yDelta = -10;
	      if (extendSel && RightExtended)
		{
		  i = pBoxEnd->BxYOrg - pBoxBegin->BxYOrg;
		  if (i < 15 /* less than 150% of yDelta */)
		    {
		      /* no more extension */
		      RightExtended = FALSE;
		      extendSel = FALSE;
		      ChangeSelection (frame, pBox->BxAbstractBox, FixedChar,
				       FALSE, TRUE, FALSE, FALSE);
		      done = TRUE;
		    }
		  else
		    {
		      /* just decrease the curent extension */
		      y = pBoxEnd->BxYOrg;
		      pBox = pBoxEnd;
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
	      else if (extendSel)
		LeftExtended = TRUE;
	      if (!done)
		LocateLeafBox (frame, view, x, y, 0, yDelta, pBox, extendSel);
	    }
	  break;
	   
	case 9:	/* Previous word (^<-) */
	  WordSearchContext.SDocument = LoadedDocument[doc - 1];
	  WordSearchContext.SStartToEnd = FALSE;
	  if (RightExtended)
	    {
	      /* shrink the current selection */
	      first = lastC;
	      pEl = lastEl;
	    }
	  else
	    {
	      /* extend the current selection */
	      first = firstC;
	      pEl = firstEl;
	    }
	  done = SearchPreviousWord (&pEl, &first, &last, word, &WordSearchContext);
	  if ((RightExtended && last >= lastC) ||
	      (LeftExtended && first == firstC - 1))
	    /* It was not the beginning of the next word */
	    done = SearchPreviousWord (&pEl, &first, &last, word, &WordSearchContext);
	  if (extendSel)
	    {
	      if (!LeftExtended && firstEl == FixedElement &&
		  last <= FixedChar)
		{
		  /* change the extension direction */
		  RightExtended = FALSE;
		  LeftExtended = TRUE;
		}
	      if (LeftExtended)
		i = first;
	      else
		i = last;
	      if (pEl->ElAbstractBox[view - 1])
		ChangeSelection (frame, pEl->ElAbstractBox[view - 1], i,
				 TRUE, LeftExtended, FALSE, FALSE);
	    }
	  else
	    {
	      SelectString (LoadedDocument[doc - 1], pEl, first, first - 1);
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
	       last = firstC;
	       pEl = firstEl;
	     }
	   else
	     {
	       /* extend the current selection */
	       last = lastC;
	       pEl = lastEl;
	     }
	   done = SearchNextWord (&pEl, &first, &last, word, &WordSearchContext);
	   if ((!LeftExtended && first == lastC) ||
	       (!RightExtended && first >= firstC))
	     /* It was not the beginning of the next word */
	     done = SearchNextWord (&pEl, &first, &last, word, &WordSearchContext);
	   if (extendSel)
	     {
	       if (LeftExtended && firstEl == FixedElement &&
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
		 ChangeSelection (frame, pEl->ElAbstractBox[view - 1], i,
				  TRUE, LeftExtended, FALSE, FALSE);
	     }
	   else
	     {
	       SelectString (LoadedDocument[doc - 1], pEl, first, first - 1);
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
void TtcPreviousChar (Document document, View view)
{
   MovingCommands (1, document, view, FALSE);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void TtcNextChar (Document document, View view)
{
   MovingCommands (2, document, view, FALSE);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void TtcPreviousLine (Document document, View view)
{
   MovingCommands (8, document, view, FALSE);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void TtcNextLine (Document document, View view)
{
   MovingCommands (7, document, view, FALSE);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void TtcStartOfLine (Document document, View view)
{
   MovingCommands (4, document, view, FALSE);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void TtcSelStartOfLine (Document document, View view)
{
   MovingCommands (4, document, view, TRUE);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void TtcEndOfLine (Document document, View view)
{
   MovingCommands (3, document, view, FALSE);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void TtcSelEndOfLine (Document document, View view)
{
   MovingCommands (3, document, view, TRUE);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void TtcPreviousSelChar (Document document, View view)
{
   MovingCommands (1, document, view, TRUE);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void TtcNextSelChar (Document document, View view)
{
   MovingCommands (2, document, view, TRUE);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void TtcPreviousSelLine (Document document, View view)
{
   MovingCommands (8, document, view, TRUE);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void TtcNextSelLine (Document document, View view)
{
   MovingCommands (7, document, view, TRUE);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void TtcPreviousWord (Document document, View view)
{
   MovingCommands (9, document, view, FALSE);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void TtcNextWord (Document document, View view)
{
   MovingCommands (10, document, view, FALSE);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void TtcPreviousSelWord (Document document, View view)
{
   MovingCommands (9, document, view, TRUE);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void TtcNextSelWord (Document document, View view)
{
   MovingCommands (10, document, view, TRUE);
}

/*----------------------------------------------------------------------
   CopyXClipboard insere le contenu de la selection courante dans   
   le buffer pour transmettre la selection X.             
  ----------------------------------------------------------------------*/
static int CopyXClipboard (unsigned char **buffer, View view)
{
  PtrTextBuffer       clipboard;
  PtrDocument         pDoc;
  PtrElement          pFirstEl, pLastEl;
  PtrElement          pEl;
  PtrAbstractBox      pBlock, pOldBlock;
  CHAR_T             *text;
  int                 i, max;
  int                 lg, maxLength;
  int                 firstChar, lastChar;
  int                 v, ind;
  ThotBool            inAttr;

  /* get the current view */
  v = view - 1;
  inAttr = FALSE;
  maxLength = 0;
  clipboard = NULL;
  lg = 1;
  pFirstEl = NULL;
  pLastEl = NULL;
  firstChar = 0;
  if (DocSelectedAttr)
    {
      /* Selection is within an attribute */
      pDoc = DocSelectedAttr;
      firstChar = FirstSelectedCharInAttr;
      lastChar = LastSelectedCharInAttr;
      maxLength = LastSelectedCharInAttr - FirstSelectedCharInAttr;
      inAttr = TRUE;
      clipboard = AbsBoxSelectedAttr->AbText;
    }
  else if (SelectedDocument)
    {
      /* selection within an element */
      pDoc = SelectedDocument;
      pFirstEl = FirstSelectedElement;
      pLastEl = LastSelectedElement;
      firstChar = FirstSelectedChar;
      lastChar = LastSelectedChar;
      if (!SelPosition && lastChar == 0)
	{
	/* get the content of the whole element */
	lastChar = pLastEl->ElVolume + 1;
	firstChar = 1;
	}
      if (pFirstEl->ElTypeNumber != CharString + 1)
	/* if it's an image firstChar is not significant here. Set it to 0 */
	firstChar = 1;

      /* Calcule la longueur du buffer a produire */
      if (pFirstEl == pLastEl && pFirstEl->ElTerminal)
	/* only one element */
	maxLength = lastChar - firstChar;
      else
	{
	  if (pFirstEl->ElTerminal)
	    maxLength = pFirstEl->ElVolume - firstChar;    /* volume 1er element */
	  pEl = pFirstEl;
	  while (pEl)
	    {
	      pEl = FwdSearchTypedElem (pEl, CharString + 1, NULL);
	      if (pEl && ElemIsBefore (pLastEl, pEl))
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
      if (pFirstEl->ElTerminal && pFirstEl->ElLeafType == LtText)
	clipboard = pFirstEl->ElText;
    }

  if (maxLength <= 0)
    /* nothing selected */
    return 0;
  /* Adding 100 characters for extra CR */
  max = maxLength + lg;
  /* Allocate a buffer with the right length */
  text = TtaGetMemory (max * sizeof (CHAR_T));
  /* Copy the text into the buffer */
  i = 0;
  ind = firstChar - 1;
  if (clipboard)
    {
      /* The first element is a text, locate the first character */
      while (clipboard && ind > clipboard->BuLength)
	{
	  ind -= clipboard->BuLength;
	  clipboard = clipboard->BuNext;
	}

      /* copy the text of the first element */
      while (clipboard && i < max && maxLength)
	{
	  lg = clipboard->BuLength - ind;
	  if (lg > maxLength)
	    lg = maxLength;
	  ustrncpy (&text[i], &clipboard->BuContent[ind], lg);
	  i += lg;
	  ind = 0;
	  maxLength -= lg;
	  clipboard = clipboard->BuNext;
	}
    }
  
  /* copy the text of following elements */
  pOldBlock = NULL;
  pEl = pFirstEl;
  while (pEl)
    {
      pEl = FwdSearchTypedElem (pEl, CharString + 1, NULL);
      if (pEl)
	{
	  if (pEl != pLastEl && ElemIsBefore (pLastEl, pEl))
	      /* it's after the last element */
	    pEl = NULL;	  
	  if (pEl)
	    {
	      pBlock = SearchEnclosingType (pEl->ElAbstractBox[v], BoBlock);
	      if (i != 0 && pBlock != pOldBlock && pOldBlock)
		{
		  /* Add new lines */
		  text[i++] = NEW_LINE;
		  text[i] = EOS;
		}
	      /* Recopie le texte de l'element */
	      pOldBlock = pBlock;
	      clipboard = pEl->ElText;
	      while (clipboard && i < max && maxLength)
		{
		  lg = clipboard->BuLength;
		  if (lg > maxLength)
		    lg = maxLength;
		  /* check the validity of lg */
		  if (lg + i > max - 1)
		    lg = max - i - 1;
		  ustrncpy (&text[i], clipboard->BuContent, lg);
		  i += lg;
		  maxLength -= lg;
		  clipboard = clipboard->BuNext;
		}
	    }
	}
    }
  text[i] = EOS;
  if (*buffer)
    TtaFreeMemory (*buffer);
#ifdef _I18N_
  /* What is the encoding used by external applications ??? */
  *buffer = TtaConvertCHARToByte (text, TtaGetDefaultCharset ());
  TtaFreeMemory (text);
#else /* _I18N_ */
  *buffer = text;
#endif /* _I18N_ */
  return i;
}

/*----------------------------------------------------------------------
   TtcCopyToClipboard
  ----------------------------------------------------------------------*/
void TtcCopyToClipboard (Document doc, View view)
{
#ifdef _GTK
  unsigned char     *buffer = NULL;
  int                len;

  /* Must get the selection */
  len = CopyXClipboard (&buffer, view);
  if (len)
    {
      ClipboardLength = len;
      if (Xbuffer)
	free (Xbuffer);
      Xbuffer = buffer;
    }
  else if (Xbuffer)
    {
	TtaFreeMemory (Xbuffer);
      Xbuffer = NULL;
    }
#else /* _GTK */
#ifndef _WINDOWS
  int                  frame;
  ThotWindow           w, wind;
  XSelectionClearEvent clear;

  if (doc == 0)
    frame = FrRef[0];
  else
    frame = GetWindowNumber (doc, view);
  /* Get the X-Selection */
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
    }
#endif /* _WINDOWS */
  /* Store the current selection */
  ClipboardLength = CopyXClipboard (&Xbuffer, view);
#ifndef _WINDOWS
  /* Annule le cutbuffer courant */
  XStoreBuffer (TtDisplay, Xbuffer, ClipboardLength, 0);
#endif /* _WINDOWS */
#endif /* _GTK */
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
  CHAR_T              c;
  int                 first, last;
  int                 doc, i;
  ThotBool            isSep;

  doc = FrameTable[frame].FrDoc;
  pAb = pBox->BxAbstractBox;
  if (frame >= 1 && doc > 0 && index >= 0)
    {
      /* check if a leaf box is selected */
      c = pBuffer->BuContent[index];
      if (c != SPACE && c != EOS)
	{
	  /* look for the beginning of the word */
	  buffer = pBuffer;
	  first = pos;
	  i = index;
	  c = PreviousCharacter (&buffer, &i);
	  isSep = IsSeparatorChar (c);
	  while (first > 1 && !isSep && c != EOS)
	    {
	      first--;
	      c = PreviousCharacter (&buffer, &i);
	      isSep = IsSeparatorChar (c);
	    }
	  /* look for the end of the word */
	  buffer = pBuffer;
	  last = pos + 1;
	  i = index;
	  c = NextCharacter (&buffer, &i);
	  isSep = IsSeparatorChar (c);
	  while (!isSep && c != EOS)
	    {
	      last++;
	      c = NextCharacter (&buffer, &i);
	      isSep = IsSeparatorChar (c);
	    }
	  pDoc = LoadedDocument[doc - 1];
	  if (pAb->AbPresentationBox && pAb->AbCanBeModified)
	    /* user has clicked in the presentation box displaying an */
	    /* attribute value */
	    SelectStringInAttr (pDoc, pAb, first, last, TRUE);
	  else
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
	  if (pBox->BxType == BoSplit || pBox->BxType == BoMulScript)
	    {
	      /* look for the right box */
	      pBox = pBox->BxNexChild;
	      while (pBox && pBox->BxNexChild && i >= pBox->BxNexChild->BxFirstChar)
		pBox = pBox->BxNexChild;
	    }
	  i -= pBox->BxFirstChar;
	  /* now look for the right position in buffers */
	  pBuffer = pBox->BxBuffer;
	  index = pBox->BxIndChar + i;
	  while (pBuffer && (index > pBuffer->BuLength))
	    {
	      index = index - pBuffer->BuLength;
	      pBuffer = pBuffer->BuNext;
	    }
	  if (pBuffer)
	    {
	      frame = GetWindowNumber (doc, view);
	      SelectCurrentWord (frame, pBox, pos, index, pBuffer, FALSE);
	    }
	}
    }
}

