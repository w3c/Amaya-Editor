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
static ThotBool Moving = FALSE;

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
   LocateLeafBox get the box located at the position x+xDelta, y+yDelta.
   if the box is still the same (pFrom) the position is shifted by
   xDelta and yDelta.                     
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         LocateLeafBox (int frame, View view, int x, int y, int xDelta, int yDelta, PtrBox pFrom, ThotBool extendSel)
#else  /* __STDC__ */
static void         LocateLeafBox (frame, view, x, y, xDelta, yDelta, pFrom, extendSel)
int                 frame;
View                view;
int                 x;
int                 y;
int                 xDelta;
int                 yDelta;
PtrBox              pFrom;
ThotBool            extendSel;
#endif /* __STDC__ */
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
   else if (yDelta < 0 && pFrame->FrYOrg > 0 && y + yDelta < pFrame->FrYOrg)
     {
       do
	 {
	   org = pLastBox->BxYOrg;
	   TtcLineUp (doc, view);
	   /* update the new position */
	   if (org != pLastBox->BxYOrg)
	     y = y + org - pLastBox->BxYOrg;
	 }
       while (pFrame->FrYOrg > 0 && y + yDelta < pFrame->FrYOrg &&
	      (y > 0 || pFrame->FrAbstractBox->AbTruncatedHead));
     }
   else if (yDelta > 0 && y + yDelta > pFrame->FrYOrg + h)
     {
       do
	 {
	   org = pLastBox->BxYOrg;
	   TtcLineDown (doc, view);
	   /* update the new position */
	   if (org != pLastBox->BxYOrg)
	     y = y + org - pLastBox->BxYOrg;
	 }
       while (y + yDelta > pFrame->FrYOrg + h &&
	      (y < pFrame->FrAbstractBox->AbBox->BxHeight || pFrame->FrAbstractBox->AbTruncatedTail));
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
		  LocateClickedChar (pBox, &pBuffer, &x, &index, &nChars, &nbbl);
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
   PtrBox              pBox, pBoxBegin, pBoxEnd;
   ViewFrame          *pFrame;
   ViewSelection      *pViewSel;
   ViewSelection      *pViewSelEnd;
   int                 frame, x, y, i;
   int                 xDelta, yDelta;
   int                 h, w, doc;
   int                 indpos, xpos;
   ThotBool            top = TRUE;

   indpos = 0;
   xpos = 0;
   CloseInsertion ();
   frame = GetWindowNumber (document, view);
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

       /* Check if boxed are visible */
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
		   /* show the beginning of the selection */
		   doc = FrameTable[frame].FrDoc;
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
		   x = pBox->BxXOrg + xpos;
		   y = pBox->BxYOrg + (pBox->BxHeight / 2);
		   xDelta = -2;
		   LocateLeafBox (frame, view, x, y, xDelta, 0, pBox, extendSel);
		 }
	     }
	   /* Get the last X position */
	   ClickX = pBoxBegin->BxXOrg + pViewSel->VsXPos - pFrame->FrXOrg;
	   break;
	   
	 case 2:	/* Forward one character (^F) */
	   if (pBox != NULL)
	     {
	       if (!extendSel || !LeftExtended)
		 {
		   /* move the right extremity */
		   pBox = pBoxEnd;
		   if (!extendSel && pViewSelEnd->VsBox &&
		       pViewSelEnd->VsBox->BxType == BoGhost)
		     x = pBox->BxNChars;
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
		   /* show the beginning of the selection */
		   doc = FrameTable[frame].FrDoc;
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
		   x = pBox->BxXOrg + pBox->BxWidth;
		   y = pBox->BxYOrg + (pBox->BxHeight / 2);
		   xDelta = 2;
		   LocateLeafBox (frame, view, x, y, xDelta, 0, pBox, extendSel);
		 }
	     }
	   /* Get the last X position */
	   ClickX = pBoxBegin->BxXOrg + pViewSel->VsXPos - pFrame->FrXOrg;
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
	       pBox = pBoxEnd;
	       x = pViewSelEnd->VsXPos + pBox->BxXOrg;
	       if (pViewSelEnd->VsBuffer && pViewSelEnd->VsIndBuf < pViewSelEnd->VsBuffer->BuLength)
		 x = x - CharacterWidth (pViewSelEnd->VsBuffer->BuContent[pViewSelEnd->VsIndBuf], pBox->BxFont);
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
	 }
       Moving = FALSE;
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
