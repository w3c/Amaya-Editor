/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2002
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * Manage box selections
 *
 * Author: I. Vatton (INRIA)
 *
 */

#include "thot_gui.h"
#include "thot_sys.h"

#include "constmedia.h"
#include "typemedia.h"
#include "message.h"
#include "appdialogue.h"

#define THOT_EXPORT extern
#include "creation_tv.h"
#include "boxes_tv.h"
#include "frame_tv.h"
#include "select_tv.h"
#include "appdialogue_tv.h"
#include "edit_tv.h"

#include "appli_f.h"
#include "applicationapi_f.h"
#include "boxlocate_f.h"
#include "boxmoves_f.h"
#include "boxselection_f.h"
#include "buildboxes_f.h"
#include "displayselect_f.h"
#include "editcommands_f.h"
#include "exceptions_f.h"
#include "font_f.h"
#include "frame_f.h"
#include "memory_f.h"
#include "scroll_f.h"
#include "structcreation_f.h"
#include "textcommands_f.h"
#include "viewapi_f.h"

/*----------------------------------------------------------------------
   ClearAbstractBoxSelection parcours l'arborescence pour annuler  
   toutes ls selections de pave.                           
  ----------------------------------------------------------------------*/
static void ClearAbstractBoxSelection (PtrAbstractBox pAb)
{
   PtrAbstractBox      pChildAb;
   PtrAbstractBox      pAbbox1;

   pAbbox1 = pAb;
   if (pAbbox1->AbSelected)
     {
	/* ce pave est selectionne */
	pAbbox1->AbSelected = FALSE;
     }
   else if (pAb->AbLeafType == LtCompound)
     {
	/* on parcours le sous-arbre */
	pChildAb = pAbbox1->AbFirstEnclosed;
	while (pChildAb != NULL)
	  {
	     ClearAbstractBoxSelection (pChildAb);
	     pChildAb = pChildAb->AbNext;
	  }
     }
}


/*----------------------------------------------------------------------
   ClearViewSelMarks annule la selection courante dans la fenetre.   
  ----------------------------------------------------------------------*/
void                ClearViewSelMarks (int frame)
{
   ViewFrame          *pFrame;

   if (frame > 0)
     {
	pFrame = &ViewFrameTable[frame - 1];
	if (pFrame->FrAbstractBox != NULL)
	   ClearAbstractBoxSelection (pFrame->FrAbstractBox);
	pFrame->FrSelectOneBox = FALSE;
	pFrame->FrSelectionBegin.VsBox = NULL;
	pFrame->FrSelectionEnd.VsBox = NULL;
     }
}

/*----------------------------------------------------------------------
   ClearViewSelection bascule et annule la mise en evidence de la   
   selection dans la fenetre.                              
  ----------------------------------------------------------------------*/
void                ClearViewSelection (int frame)
{
   ViewFrame          *pFrame;
   PtrBox              pBox, pBox1, pBox2;
   PtrAbstractBox      pAb1, pAb2;
   int                 x1, x2;

   if (frame > 0)
     {
	pFrame = &ViewFrameTable[frame - 1];
	SetNewSelectionStatus (frame, pFrame->FrAbstractBox, FALSE);
	if (pFrame->FrSelectionBegin.VsBox && pFrame->FrSelectionEnd.VsBox)
	  {
	    /* begginning of the selection */
	    pBox1 = pFrame->FrSelectionBegin.VsBox;
	    pAb1 = pBox1->BxAbstractBox;
	    /* end of the selection */
	    pBox2 = pFrame->FrSelectionEnd.VsBox;
	    pAb2 = pBox2->BxAbstractBox;
	    pFrame->FrSelectOneBox = FALSE;
	    pFrame->FrSelectionBegin.VsBox = NULL;
	    pFrame->FrSelectionEnd.VsBox = NULL;

	    /* ready to un/display the current selection */
	    if (pBox1 == pBox2)
	      {
		/* only one box is selected */
		if (pBox1->BxType == BoGhost ||
		    (pAb1 != NULL && pAb1->AbElement != NULL &&
		     FrameTable[frame].FrView == 1 &&
		     TypeHasException (ExcHighlightChildren,
				       pAb1->AbElement->ElTypeNumber,
				       pAb1->AbElement->ElStructSchema)))
		  /* the highlight is transmitted to children */
		  DrawBoxSelection (frame, pBox1);
		else
		  {
		    if (pFrame->FrSelectionBegin.VsIndBox == 0 ||
			pAb1->AbLeafType == LtPolyLine ||
			pAb1->AbLeafType == LtPath)
		      {
			/* the whole box is selected */
			x1 = pBox1->BxXOrg;
			x2 = pBox1->BxXOrg + pBox1->BxWidth;
		      }
		    else
		      {
			x1 = pBox1->BxXOrg + pFrame->FrSelectionBegin.VsXPos;
			x2 = pBox1->BxXOrg + pFrame->FrSelectionEnd.VsXPos;
		      }
		    if (x1 == x2)
		      /* removing the caret at the end of a text */
		      x2 = x1 + 2;
		    DefClip (frame, x1, pBox1->BxYOrg, x2,
			     pBox1->BxYOrg + pBox1->BxHeight);
		  }
	      }
	    else if (pAb1 == pAb2)
	      {
		/* several pieces of a split box are selected */
		/* the first one */
		x1 = pBox1->BxXOrg + pFrame->FrSelectionBegin.VsXPos;
		x2 = pBox1->BxXOrg + pBox1->BxWidth;
		DefClip (frame, x1, pBox1->BxYOrg, x2,
			 pBox1->BxYOrg + pBox1->BxHeight);
		/* intermediate boxes */
		pBox1 = pBox1->BxNexChild;
		while (pBox1 != pBox2)
		  {
		    DefClip (frame, pBox1->BxXOrg, pBox1->BxYOrg,
			     pBox1->BxXOrg + pBox1->BxWidth,
			     pBox1->BxYOrg + pBox1->BxHeight);
		    pBox1 = pBox1->BxNexChild;
		  }
		/* the last one */
		x1 = pBox2->BxXOrg;
		x2 = pBox2->BxXOrg + pFrame->FrSelectionEnd.VsXPos;
		DefClip (frame, x1, pBox2->BxYOrg, x2,
			 pBox2->BxYOrg + pBox2->BxHeight);
	      }
	    else
	      {
		/* undisplay the beginning of the selection */
		if (pBox1->BxType == BoGhost ||
		    (pAb1 != NULL &&
		     FrameTable[frame].FrView == 1 &&
		     TypeHasException (ExcHighlightChildren,
				       pAb1->AbElement->ElTypeNumber,
				       pAb1->AbElement->ElStructSchema)))
		  /* the highlight is transmitted to children */
		  DrawBoxSelection (frame, pBox1);
		else
		  {
		    if (pFrame->FrSelectionBegin.VsIndBox == 0)
		      {
			/* the whole box is selected */
			x1 = pBox1->BxXOrg;
			x2 = pBox1->BxXOrg + pBox1->BxWidth;
		      }
		    else
		      {
			x1 = pBox1->BxXOrg + pFrame->FrSelectionBegin.VsXPos;
			x2 = pBox1->BxXOrg + pBox1->BxWidth;
		      }
		    DefClip (frame, x1, pBox1->BxYOrg, x2,
			     pBox1->BxYOrg + pBox1->BxHeight);
		    if (pBox1->BxType == BoPiece ||
			pBox1->BxType == BoDotted)
		      {
			/* unselect the end of the split text */
			pBox = pBox1->BxNexChild;
			while (pBox)
			  {
			    DefClip (frame, pBox->BxXOrg, pBox->BxYOrg,
				     pBox->BxXOrg + pBox->BxWidth,
				     pBox->BxYOrg + pBox->BxHeight);
			    pBox = pBox->BxNexChild;
			  }
		      }
		  }
		
		/* undisplay the end of the selection */
		if (pBox2->BxType == BoGhost ||
		    (pAb2 != NULL &&
		     FrameTable[frame].FrView == 1 &&
		     TypeHasException (ExcHighlightChildren,
				       pAb2->AbElement->ElTypeNumber,
				       pAb2->AbElement->ElStructSchema)))
		  /* the highlight is transmitted to children */
		  DrawBoxSelection (frame, pBox2);
		else
		  {
		    if (pFrame->FrSelectionEnd.VsIndBox == 0)
		      {
			/* the whole box is selected */
			x1 = pBox2->BxXOrg;
			x2 = pBox2->BxXOrg + pBox2->BxWidth;
		      }
		    else
		      {
			x1 = pBox2->BxXOrg;
			x2 = pBox2->BxXOrg + pFrame->FrSelectionEnd.VsXPos;
		      }
		    DefClip (frame, x1, pBox2->BxYOrg, x2,
			     pBox2->BxYOrg + pBox2->BxHeight);
		    if (pBox2->BxType == BoPiece ||
			pBox1->BxType == BoDotted)
		      {
			/* select the begin of the split text */
			pBox =  pAb2->AbBox->BxNexChild;
			while (pBox && pBox != pBox2)
			  {
			    DefClip (frame, pBox->BxXOrg, pBox->BxYOrg,
				     pBox->BxXOrg + pBox->BxWidth,
				     pBox->BxYOrg + pBox->BxHeight);
			    pBox = pBox->BxNexChild;
			  }
		      }
		  }
	      }
	  }
     }
}

/*----------------------------------------------------------------------
   TtaClearViewSelections unselects and clears all current displayed
   selections.                                  
  ----------------------------------------------------------------------*/
void                TtaClearViewSelections ()
{
   int                 frame;

   /* manage all frames */
   for (frame = 1; frame <= MAX_FRAME; frame++)
      if (ViewFrameTable[frame - 1].FrAbstractBox != NULL)
	{
	  ClearViewSelection (frame);
	  DisplayFrame (frame);
	}
}

/*----------------------------------------------------------------------
   UpdateViewSelMarks met a jour les marques de selection de frame   
   apres insertion ou destruction de caracteres.           
  ----------------------------------------------------------------------*/
void    UpdateViewSelMarks (int frame, int xDelta, int spaceDelta, int charDelta)
{
   ViewFrame          *pFrame;
   ViewSelection      *pViewSel;

   pFrame = &ViewFrameTable[frame - 1];
   pViewSel = &pFrame->FrSelectionBegin;
   pViewSel->VsXPos += xDelta;
   pViewSel->VsIndBox += charDelta;
   pViewSel->VsNSpaces += spaceDelta;
   pViewSel = &pFrame->FrSelectionEnd;
   if (pViewSel->VsBox == pFrame->FrSelectionBegin.VsBox)
     {
	pViewSel->VsXPos += xDelta;
	pViewSel->VsIndBox += charDelta;
	pViewSel->VsNSpaces += spaceDelta;
     }
}


/*----------------------------------------------------------------------
   Detruit le buffer donne en parametre, met a jour les marques de 
   selection et rend le pointeur sur le buffer precedent.          
  ----------------------------------------------------------------------*/
PtrTextBuffer       DeleteBuffer (PtrTextBuffer pBuffer, int frame)
{
   PtrTextBuffer       pNextBuffer;
   PtrTextBuffer       pPreviousBuffer;
   int                 length = 0;
   ViewFrame          *pFrame;
   ViewSelection      *pViewSel;
   ViewSelection      *pViewSelEnd;

   pNextBuffer = pBuffer->BuNext;
   pPreviousBuffer = pBuffer->BuPrevious;
   if (pPreviousBuffer != NULL)
     {
	pPreviousBuffer->BuNext = pNextBuffer;
	length = pPreviousBuffer->BuLength;
     }
   if (pNextBuffer != NULL)
      pNextBuffer->BuPrevious = pPreviousBuffer;

   /* Mise a jour des marques de selection courante */
   if (frame > 0)
     {
       pFrame = &ViewFrameTable[frame - 1];
       pViewSel = &pFrame->FrSelectionBegin;
       if (pViewSel->VsBuffer == pBuffer)
	 {
	   if (pFrame->FrSelectionEnd.VsBuffer == pViewSel->VsBuffer)
	     {
	       pViewSelEnd = &pFrame->FrSelectionEnd;
	       if (pPreviousBuffer != NULL)
		 {
		   /* deplace la selection dans les buffers */
		   pViewSelEnd->VsIndBuf += length;
		   pViewSelEnd->VsBuffer = pPreviousBuffer;
		 }
	       else
		 {
		   pViewSelEnd->VsIndBuf = 1;
		   pViewSelEnd->VsBuffer = pNextBuffer;
		 }
	     }
	   pViewSel->VsBuffer = pPreviousBuffer;
	   
	   if (pPreviousBuffer != NULL)
	     {
	       /* deplace la selection dans les buffers */
	       pViewSel->VsIndBuf += length;
	       pViewSel->VsBuffer = pPreviousBuffer;
	     }
	   else
	     {
	       pViewSel->VsIndBuf = 1;
	       pViewSel->VsBuffer = pNextBuffer;
	     }
	 }
     }

   FreeTextBuffer (pBuffer);
   return pPreviousBuffer;
}


/*----------------------------------------------------------------------
   LocateBuffer parcours les buffers de la boite de texte pour trouver
   celui qui contient le caractere d'indice global index   
   ainsi que son indice dans ce buffer.                    
  ----------------------------------------------------------------------*/
static void         LocateBuffer (PtrTextBuffer * pBuffer, int *index)
{
   ThotBool            still;

   still = (*pBuffer != NULL);
   while (still)
     {
       /* Est-ce le bon buffer ? */
       if ((*pBuffer)->BuLength < *index)
	 /* Non : il faut passer au buffer suivant */
	 if ((*pBuffer)->BuNext == NULL)
	   {
	     /* arrive en fin de liste de buffers sans trouver le caractere */
	     *index = (*pBuffer)->BuLength + 1;
	     still = FALSE;
	   }
	 else
	   /* passe au buffer suivant */
	   {
	     *index -= (*pBuffer)->BuLength;
	     *pBuffer = (*pBuffer)->BuNext;
	   }
       else
	 still = FALSE;
     }
}

/*----------------------------------------------------------------------
   ComputeViewSelMarks calcule la marque de selection connaissant la 
   boite entiere VsBox, le buffer VsBuffer et l'index du   
   caractere VsIndBuf marque'.                             
   Deduit l'index caractere (VsIndBox), le nombre de blancs
   le precedant VsNSpaces, la position dans la boite VsXPos
   et la ligne contenant la boite VsLine.                  
  ----------------------------------------------------------------------*/
void                ComputeViewSelMarks (ViewSelection *selMark)
{
  PtrTextBuffer       pBuffer;
  PtrTextBuffer       pSelBuffer;
  PtrBox              pBox;
  int                 beginInd, x;
  int                 spaceWidth;
  int                 max, i;
  int                 dummy;
  ThotBool            stop;

  if (selMark->VsBox->BxAbstractBox->AbLeafType == LtText)
    {
      /* note l'index et le buffer du caractere precedant la marque */
      pSelBuffer = selMark->VsBuffer;
      pBox = selMark->VsBox;
      if (selMark->VsIndBuf == 1 && pSelBuffer != pBox->BxBuffer)
	{
	  if (pSelBuffer->BuPrevious == NULL)
	    pSelBuffer = pBox->BxBuffer;
	  else
	    /* En debut de buffer */
	    pSelBuffer = pSelBuffer->BuPrevious;
	  i = pSelBuffer->BuLength;
	}
      else
	/* En fin ou en cours de buffer */
	i = selMark->VsIndBuf - 1;
      stop = FALSE;

      /* Est-ce une boite coupee ? */
      if (pBox->BxType == BoSplit)
	pBox = pBox->BxNexChild;

      /* Recherche l'index du caractere et la boite de coupure */
      pBuffer = pBox->BxBuffer;
      beginInd = 1 - pBox->BxFirstChar;
      max = pBox->BxNChars;
      /* Calcule le saut entre cette boite et la suivante pour */
      /* determiner si on peut selectionner en fin de boite */
      if (pBox->BxNexChild == NULL)
	dummy = 0;
      else
	dummy = pBox->BxNexChild->BxIndChar - pBox->BxIndChar - pBox->BxNChars;

      /* Boucle tant que le caractere designe se trouve dans */
      /* le buffer suivant ou dans la boite suivante */
      while (!stop && (pBuffer != pSelBuffer || max - beginInd + dummy <= i))
	if (max - beginInd + dummy <= i || pBuffer == NULL)
	  {
	    /* Box de coupure GetNextBox */
	    /* Cas particulier des blancs supprimes en fin de boite */
	    /* Est-ce qu'il y a une boite apres ? */
	    if (pBox->BxNexChild == NULL)
	      stop = TRUE;
	    /* Il existe une boite apres mais c'est une boite fantome */
	    else if (pBox->BxNexChild->BxNChars == 0 &&
		     SearchLine (pBox->BxNexChild) == NULL)
	      stop = TRUE;
	    else
	      stop = FALSE;

	    /* Est-ce que la selection est en fin de boite ? */
	    if (stop)
	      {
		dummy = i - max + beginInd;
		/* Position dans les blancs de fin de ligne */
		selMark->VsIndBox = pBox->BxNChars + dummy;
		selMark->VsXPos = pBox->BxLMargin + pBox->BxLBorder + pBox->BxLPadding + pBox->BxW;
		selMark->VsNSpaces = pBox->BxNSpaces + dummy;
	      }
	    /* Sinon on passe a la boite suivante */
	    else
	      {
		pBox = pBox->BxNexChild;
		beginInd = 1 - pBox->BxFirstChar;
		max = pBox->BxNChars;
		pBuffer = pBox->BxBuffer;
		/* Calcule le saut entre cette boite et la suivante pour */
		/* determiner si on peut selectionner en fin de boite */
		if (pBox->BxNexChild == NULL)
		  dummy = 0;
		else
		  dummy = pBox->BxNexChild->BxIndChar - pBox->BxIndChar - pBox->BxNChars;
		/* Cas particulier du premier caractere d'une boite coupee */
		if (pSelBuffer == pBuffer->BuPrevious)
		  {
		    selMark->VsIndBox = 0;
		    selMark->VsXPos = 0;
		    selMark->VsNSpaces = 0;
		    stop = TRUE;
		  }
	      }
	  }
	else
	  {
	    beginInd += pBuffer->BuLength;
	    pBuffer = pBuffer->BuNext;
	  }

      /* on a trouve' la boite de coupure */
      if (!stop)
	{
	  selMark->VsIndBox = beginInd + i;
	  /* Reevaluation du decalage dans la boite */
	  /* 0 si on prend la largeur reelle du blanc */
	  spaceWidth = pBox->BxSpaceWidth;
	  /* Index du premier caractere a traiter */
	  x = pBox->BxFirstChar;
	  GiveTextParams (pBox->BxBuffer, selMark->VsIndBox, pBox->BxFont,
			  &x, &spaceWidth);
	  selMark->VsXPos = x + pBox->BxLMargin + pBox->BxLBorder + pBox->BxLPadding;
	  selMark->VsNSpaces = spaceWidth;
	  /* ajoute eventuellement les pixels repartis */
	  if (pBox->BxSpaceWidth != 0)
	    {
	      if (selMark->VsNSpaces < pBox->BxNPixels)
		selMark->VsXPos += selMark->VsNSpaces;
	      else
		selMark->VsXPos += pBox->BxNPixels;
	    }
	}
      selMark->VsBox = pBox;
    }
  selMark->VsLine = SearchLine (selMark->VsBox);
}

/*----------------------------------------------------------------------
  InsertViewSelMarks inserts selection makes into the displayed boxes.
  The abstract box pAb concerns the beginning of the selection
  (startSelection=TRUE) and/or the end of the selection (endSelection=TRUE).
  The parameter firstChar gives the index of the first selected character
  or 0 if the whole abstract box is selected.
  The parameter lastChar gives the index of the character that follows
  the last selected character.
  The parameter alone is set to TRUE when only one abstract box is selected.
  ----------------------------------------------------------------------*/
void InsertViewSelMarks (int frame, PtrAbstractBox pAb, int firstChar,
			 int lastChar, ThotBool startSelection,
			 ThotBool endSelection, ThotBool alone)
{
  PtrLine             adline;
  PtrTextBuffer       pBuffer;
  PtrBox              pBox;
  ViewFrame          *pFrame;
  ViewSelection      *pViewSel;
  int                 ind, charIndex, w;
  ThotBool            graphSel, rtl;

  /* Verifie s'il faut reformater le dernier paragraphe edite */
  if (ThotLocalActions[T_updateparagraph] != NULL)
    (*ThotLocalActions[T_updateparagraph]) (pAb, frame);

  if (pAb != NULL && frame > 0)
    {
      pFrame = &ViewFrameTable[frame - 1];
      if (pAb->AbBox != NULL)
	{
	  /* eteint la selection */
	  pBox = pAb->AbBox;
	  rtl = (pAb->AbDirection == 'R');
	  adline = SearchLine (pBox);
	  graphSel = (pAb->AbLeafType == LtPolyLine ||
		      pAb->AbLeafType == LtPath ||
		      pAb->AbLeafType == LtGraphics);

	  /* verifie la coherence des indices de caracteres */
	  if (pAb->AbLeafType == LtText)
	    /* C'est une feuille de texte */
	    {
	      if (firstChar == 0 && lastChar != 0)
		firstChar = 1;
	      else if (firstChar > 1 && lastChar == 0)
		lastChar = pAb->AbVolume;
	      else if (startSelection &&firstChar == 0 && pAb->AbVolume != 0)
		{
		  /* select the whole text */
		  firstChar = 1;
		  lastChar = pAb->AbVolume;
		}
	    }
	  else if (!graphSel && pAb->AbLeafType != LtPicture)
	    firstChar = 0;
	  
	  /* memorise si la selection relle porte sur un seul pave ou non */
	  pFrame->FrSelectOneBox = alone;
	  /* et si elle indique seulement une position */
	  pFrame->FrSelectOnePosition = SelPosition;
	  
	  /* La selection porte sur le pave complet ou un point d'un path */
	  /* ou d'une polyline */
	  if (firstChar == 0 || pAb->AbVolume == 0 ||
	      graphSel || pAb->AbLeafType == LtPicture)
	    {
	      /* Est-ce une boite de texte ? */
	      if (pAb->AbLeafType == LtText)
		{
		  ind = 1;
		  pBuffer = pAb->AbText;
		}
	      else
		{
		  ind = 0;
		  pBuffer = NULL;
		}

	      /* memorise les marques de selection */
	      if (startSelection)
		{
		  pViewSel = &pFrame->FrSelectionBegin;
		  pViewSel->VsBox = pBox;
		  if (endSelection && !graphSel &&
		      pAb->AbLeafType != LtPicture)
		    /* tout selectionne */
		    pViewSel->VsIndBox = 0;
		  else
		    pViewSel->VsIndBox = firstChar;
		  pViewSel->VsIndBuf = ind;
		  pViewSel->VsBuffer = pBuffer;
		  pViewSel->VsLine = adline;
		  if (pAb->AbLeafType == LtPicture && firstChar > 0)
		    pViewSel->VsXPos = pBox->BxLMargin + pBox->BxLBorder + pBox->BxLPadding + pBox->BxW;
		  else if (rtl)
		    /* right-to-left writing */
		    pViewSel->VsXPos = pBox->BxWidth;
		  else
		    pViewSel->VsXPos = 0;
		  pViewSel->VsNSpaces = 0;
		}
	      if (endSelection || SelPosition)
		{
		  pViewSel = &pFrame->FrSelectionEnd;
		  pViewSel->VsBox = pBox;
		  pViewSel->VsIndBox = 0;
		  pViewSel->VsIndBuf = ind;
		  pViewSel->VsBuffer = pBuffer;
		  pViewSel->VsLine = adline;
		  if ((pAb->AbLeafType == LtPicture && firstChar > 0) ||
		      (pAb->AbLeafType == LtSymbol && firstChar == 0))
		    pViewSel->VsXPos = pBox->BxLMargin + pBox->BxLBorder + pBox->BxLPadding + pBox->BxW;
		  else if (rtl)
		    /* right-to-left writing */
		    pViewSel->VsXPos = pBox->BxWidth;
		  else
		    pViewSel->VsXPos = 0;
		  pViewSel->VsNSpaces = 0;
		}
	    }
	  /* La selection porte sur une sous-chaine */
	  else
	    {
	      /* recherche le buffer et l'index dans ce buffer */
	      if (startSelection)
		ind = firstChar;
	      else
		ind = lastChar;
	      pBuffer = pAb->AbText;
	      if (ind > pAb->AbVolume)
		{
		  /* En fin de boite */
		  charIndex = pAb->AbVolume;
		  /* recherche le buffer et l'indice */
		  LocateBuffer (&pBuffer, &ind);
		}
	      else
		{
		  /* Au milieu de la boite */
		  charIndex = ind - 1;
		  /* recherche le buffer et l'indice */
		  LocateBuffer (&pBuffer, &ind);
		}
	      
	      /* met a jour le debut de selection */
	      if (startSelection)
		{
		  pViewSel = &pFrame->FrSelectionBegin;
		  pViewSel->VsBox = pBox;
		  pViewSel->VsIndBox = charIndex;
		  pViewSel->VsIndBuf = ind;
		  pViewSel->VsBuffer = pBuffer;
		  ComputeViewSelMarks (&pFrame->FrSelectionBegin);
		}
	      /* met a jour la fin de selection */
	      if (endSelection)
		{
		  pViewSel = &pFrame->FrSelectionEnd;
		  /* startSelection and endSelection on the same character */
		  if (startSelection && firstChar >= lastChar)
		    {
		      pViewSel->VsBox = pFrame->FrSelectionBegin.VsBox;
		      pViewSel->VsIndBox = pFrame->FrSelectionBegin.VsIndBox;
		      pViewSel->VsLine = pFrame->FrSelectionBegin.VsLine;
		      pViewSel->VsBuffer = pFrame->FrSelectionBegin.VsBuffer;
		      pViewSel->VsIndBuf = pFrame->FrSelectionBegin.VsIndBuf;
		      pViewSel->VsXPos = pFrame->FrSelectionBegin.VsXPos;
		      pViewSel->VsNSpaces = pFrame->FrSelectionBegin.VsNSpaces;
		    }
		  else
		    {
		      /* startSelection et endSelection sur deux caracteres
			 differents */
		      if (startSelection)
			{
			  pBuffer = pAb->AbText;
			  ind = lastChar;
			  if (ind > pAb->AbVolume)
			    {
			      /* En fin de boite */
			      charIndex = pAb->AbVolume;
			      /* recherche le buffer et l'indice */
			      LocateBuffer (&pBuffer, &ind);
			    }
			  else
			    {
			      /* Au milieu de la boite */
			      charIndex = ind - 1;
			      /* recherche le buffer et l'indice */
			      LocateBuffer (&pBuffer, &ind);
			    }
			}
		      pViewSel->VsBox = pBox;
		      pViewSel->VsIndBox = charIndex;
		      pViewSel->VsIndBuf = ind;
		      pViewSel->VsBuffer = pBuffer;
		      ComputeViewSelMarks (&pFrame->FrSelectionEnd);
		      /* the box could be updated */
		      pBox =  pViewSel->VsBox;
		    }
		  
		  /* recherche la position limite du caractere */
		  pBox = pViewSel->VsBox;
		  if (pBox->BxNChars == 0 && pBox->BxType == BoComplete)
		    {
		      if (rtl)
			/* right-to-left writing */
			pViewSel->VsXPos -= pBox->BxW;
		      else
			pViewSel->VsXPos += pBox->BxW;
		    }
		  else if (pViewSel->VsIndBox == pBox->BxNChars)
		    pViewSel->VsXPos += 2;
		  else
		    {
		      charIndex = (int) (pViewSel->VsBuffer->BuContent[pViewSel->VsIndBuf - 1]);
		      if (charIndex == SPACE && pBox->BxSpaceWidth != 0)
			pViewSel->VsXPos += pBox->BxSpaceWidth;
		      else
			pViewSel->VsXPos += BoxCharacterWidth ((unsigned char) charIndex, pBox->BxFont);
		    }
		}
	    }

	  /* pViewSel points to the right selector */
	  /* ready to display the current selection */
	  if (startSelection && endSelection)
	    {
	      /* only one box is selected */
	      pBox = pViewSel->VsBox;
	      if (pBox->BxType == BoGhost ||
		  (pAb != NULL &&
		   FrameTable[frame].FrView == 1 &&
		   TypeHasException (ExcHighlightChildren,
				     pAb->AbElement->ElTypeNumber,
				     pAb->AbElement->ElStructSchema)))
		/* the highlight is transmitted to children */
		DrawBoxSelection (frame, pBox);
	      else
		{
		  if (pBox != pFrame->FrSelectionBegin.VsBox)
		    {
		      /* several pieces of a split box are selected */
		      /* the last one */
		      if (rtl)
			/* right-to-left writing */
			DefClip (frame, pBox->BxXOrg + pViewSel->VsXPos, pBox->BxYOrg,
				 pBox->BxXOrg + pBox->BxWidth,
				 pBox->BxYOrg + pBox->BxHeight);
		      else
			DefClip (frame, pBox->BxXOrg, pBox->BxYOrg,
				 pBox->BxXOrg + pViewSel->VsXPos,
				 pBox->BxYOrg + pBox->BxHeight);
		      /* the first one */
		      pBox = pFrame->FrSelectionBegin.VsBox;
		      DefClip (frame,
			       pBox->BxXOrg + pFrame->FrSelectionBegin.VsXPos,
			       pBox->BxYOrg, pBox->BxXOrg + pBox->BxWidth,
			       pBox->BxYOrg + pBox->BxHeight);
		      /* intermediate boxes */
		      pBox = pBox->BxNexChild;
		      while (pBox != pFrame->FrSelectionEnd.VsBox)
			{
			  DefClip (frame, pBox->BxXOrg, pBox->BxYOrg,
				   pBox->BxXOrg + pBox->BxWidth,
				   pBox->BxYOrg + pBox->BxHeight);
			  pBox = pBox->BxNexChild;
			}
		    }
		  else if (pFrame->FrSelectionBegin.VsIndBox == 0 || graphSel)
		    {
		      /* the whole box is selected */
		      w =  pBox->BxWidth;
		      if (w == 0)
			w = 2;
		      DefClip (frame, pBox->BxXOrg, pBox->BxYOrg,
			       pBox->BxXOrg + w,
			       pBox->BxYOrg + pBox->BxHeight);
		    }
		  else
		    {
		      if (pFrame->FrSelectionBegin.VsXPos == pViewSel->VsXPos)
			w = 2;
		      else
			w = 0;
		      /* a substring or a point of the box is selected */
		      DefClip (frame,
			       pBox->BxXOrg + pFrame->FrSelectionBegin.VsXPos,
			       pBox->BxYOrg, pBox->BxXOrg + pViewSel->VsXPos + w,
			       pBox->BxYOrg + pBox->BxHeight);
		    }
		}
	    }
	  else if (endSelection)
	    {
	      /* display the end of the selection */
	      pBox = pViewSel->VsBox;
	      if (pBox->BxType == BoGhost ||
		  (pAb != NULL &&
		   FrameTable[frame].FrView == 1 &&
		   TypeHasException (ExcHighlightChildren,
				     pAb->AbElement->ElTypeNumber,
				     pAb->AbElement->ElStructSchema)))
		/* the highlight is transmitted to children */
		DrawBoxSelection (frame, pBox);
	      else
		{
		  if (pViewSel->VsIndBox == 0)
		    /* the whole box is selected */
		    DefClip (frame, pBox->BxXOrg, pBox->BxYOrg,
			     pBox->BxXOrg + pBox->BxWidth,
			     pBox->BxYOrg + pBox->BxHeight);
		  else
		    /* a substring or a point of the box is selected */
		    DefClip (frame, pBox->BxXOrg, pBox->BxYOrg,
			     pBox->BxXOrg + pViewSel->VsXPos,
			     pBox->BxYOrg + pBox->BxHeight);
		  if (pBox->BxType == BoPiece ||
		      pBox->BxType == BoDotted)
		    {
		      /* select the begin of the split text */
		      pBox = pAb->AbBox->BxNexChild;
		      while (pBox != pViewSel->VsBox)
			{
			  DefClip (frame, pBox->BxXOrg, pBox->BxYOrg,
				   pBox->BxXOrg + pBox->BxWidth,
				   pBox->BxYOrg + pBox->BxHeight);
			  pBox = pBox->BxNexChild;
			}
		    }
		}
	    }
	  else
	    {
	      /* display the beginning of the selection */
	      pBox = pViewSel->VsBox;
	      if (pBox->BxType == BoGhost ||
		  (pAb != NULL &&
		   FrameTable[frame].FrView == 1 &&
		   TypeHasException (ExcHighlightChildren,
				     pAb->AbElement->ElTypeNumber,
				     pAb->AbElement->ElStructSchema)))
		/* the highlight is transmitted to children */
		DrawBoxSelection (frame, pBox);
	      else
		{
		  if (pViewSel->VsIndBox == 0)
		    /* the whole box is selected */
		    DefClip (frame, pBox->BxXOrg, pBox->BxYOrg,
			     pBox->BxXOrg + pBox->BxWidth,
			     pBox->BxYOrg + pBox->BxHeight);
		  else
		    /* a substring or a point of the box is selected */
		    DefClip (frame, pBox->BxXOrg + pViewSel->VsXPos,
			     pBox->BxYOrg, pBox->BxXOrg + pBox->BxWidth,
			     pBox->BxYOrg + pBox->BxHeight);
		  if (pBox->BxType == BoPiece ||
		      pBox->BxType == BoDotted)
		    {
		      /* select the end of the split text */
		      pBox = pBox->BxNexChild;
		      while (pBox)
			{
			  DefClip (frame, pBox->BxXOrg, pBox->BxYOrg,
				   pBox->BxXOrg + pBox->BxWidth,
				   pBox->BxYOrg + pBox->BxHeight);
			  pBox = pBox->BxNexChild;
			}
		    }
		}
	    }
	}
    }
}

/*----------------------------------------------------------------------
   IsAbstractBoxDisplayed rend la valeur vrai si le pave est affiche'
   dans le frame.                                          
  ----------------------------------------------------------------------*/
ThotBool            IsAbstractBoxDisplayed (PtrAbstractBox pAb, int frame)
{
   int                 min, max;

   if (pAb == NULL)
      return FALSE;
   else
      while (pAb->AbBox == NULL)
	 /* remonte au pavee englobant cree */
	 if (pAb->AbEnclosing == NULL)
	    return FALSE;
	 else
	    pAb = pAb->AbEnclosing;

   /* regarde si le pavee est affiche dans la fenetre */
   GetSizesFrame (frame, &min, &max);
   min = ViewFrameTable[frame - 1].FrYOrg;
   max += min;
   if (pAb->AbBox->BxYOrg + pAb->AbBox->BxHeight < min ||
       pAb->AbBox->BxYOrg > max)
      return FALSE;
   else
      return TRUE;
}
