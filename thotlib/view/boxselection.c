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
 * Manage box selections
 *
 * Author: I. Vatton (INRIA)
 *
 */

#include "ustring.h"
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
#include "font_f.h"
#include "memory_f.h"
#include "scroll_f.h"
#include "structcreation_f.h"
#include "textcommands_f.h"
#include "viewapi_f.h"

/*----------------------------------------------------------------------
   ClearAbstractBoxSelection parcours l'arborescence pour annuler  
   toutes ls selections de pave.                           
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         ClearAbstractBoxSelection (PtrAbstractBox pAb)
#else  /* __STDC__ */
static void         ClearAbstractBoxSelection (pAb)
PtrAbstractBox      pAb;

#endif /* __STDC__ */
{
   PtrAbstractBox      pChildAb;
   PtrAbstractBox      pAbbox1;

   pAbbox1 = pAb;
   if (pAbbox1->AbSelected)
     {
	/* ce pave est selectionne */
	pAbbox1->AbSelected = FALSE;
     }
   else
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
   SwitchSelection bascule la mise en e'vidence de la se'lection dans 
   la fene^tre frame :                                     
   - si toShow est Vrai et que la se'lection est eteinte,  
   - ou si toShow est Faux et la se'lection allume'e.      
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                SwitchSelection (int frame, ThotBool toShow)
#else  /* __STDC__ */
void                SwitchSelection (frame, toShow)
int                 frame;
ThotBool            toShow;

#endif /* __STDC__ */
{
   ViewFrame          *pFrame;
   Document doc;

   /* visualisation la selection locale */
   if (frame > 0)
     {
       doc = FrameTable[frame].FrDoc;
       pFrame = &ViewFrameTable[frame - 1];
       /* compare le booleen toShow et l'etat de la selection */
       if (toShow && !pFrame->FrSelectShown)
	 DisplayCurrentSelection (frame, TRUE);
       else if (!toShow && pFrame->FrSelectShown)
	 DisplayCurrentSelection (frame, TRUE);
     }
}


/*----------------------------------------------------------------------
  TtaSwitchSelection switches on or off the selection in the current 
  document view according to the toShow value:
  - TRUE if on
  - FALSE if off
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaSwitchSelection (Document document, View view, ThotBool toShow)
#else  /* __STDC__ */
void                TtaSwitchSelection (document, view, toShow)
Document            document;
View                view;
ThotBool            toShow;

#endif /* __STDC__ */
{
  int               frame;

   frame = GetWindowNumber (document, view);
   SwitchSelection (frame, toShow);
}

/*----------------------------------------------------------------------
   ClearViewSelMarks annule la selection courante dans la fenetre.   
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ClearViewSelMarks (int frame)
#else  /* __STDC__ */
void                ClearViewSelMarks (frame)
int                 frame;

#endif /* __STDC__ */
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
#ifdef __STDC__
void                ClearViewSelection (int frame)
#else  /* __STDC__ */
void                ClearViewSelection (frame)
int                 frame;

#endif /* __STDC__ */
{
   ViewFrame          *pFrame;

   if (frame > 0)
     {
	pFrame = &ViewFrameTable[frame - 1];
	/* eteint la selection ssi elle est allumee */
	if (pFrame->FrSelectShown)
	   DisplayCurrentSelection (frame, FALSE);
	else
	   SetNewSelectionStatus (frame, pFrame->FrAbstractBox, FALSE);
	pFrame->FrSelectOneBox = FALSE;
	pFrame->FrSelectionBegin.VsBox = NULL;
	pFrame->FrSelectionEnd.VsBox = NULL;
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
   for (frame = 0; frame < MAX_FRAME; frame++)
      if (ViewFrameTable[frame].FrAbstractBox != NULL)
	 ClearViewSelection (frame + 1);
}

/*----------------------------------------------------------------------
   UpdateViewSelMarks met a jour les marques de selection de frame   
   apres insertion ou destruction de caracteres.           
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                UpdateViewSelMarks (int frame, int xDelta, int spaceDelta, int charDelta)
#else  /* __STDC__ */
void                UpdateViewSelMarks (frame, xDelta, spaceDelta, charDelta)
int                 frame;
int                 xDelta;
int                 spaceDelta;
int                 charDelta;

#endif /* __STDC__ */
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
#ifdef __STDC__
PtrTextBuffer       DeleteBuffer (PtrTextBuffer pBuffer, int frame)
#else  /* __STDC__ */
PtrTextBuffer       DeleteBuffer (pBuffer, frame)
PtrTextBuffer       pBuffer;
int                 frame;

#endif /* __STDC__ */
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
#ifdef __STDC__
static void         LocateBuffer (PtrTextBuffer * pBuffer, int *index)
#else  /* __STDC__ */
static void         LocateBuffer (pBuffer, index)
PtrTextBuffer      *pBuffer;
int                *index;

#endif /* __STDC__ */
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
#ifdef __STDC__
void                ComputeViewSelMarks (ViewSelection * selMark)
#else  /* __STDC__ */
void                ComputeViewSelMarks (selMark)
ViewSelection      *selMark;

#endif /* __STDC__ */
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
		else if (pBox->BxNexChild->BxNChars == 0 && SearchLine (pBox->BxNexChild) == NULL)
		   stop = TRUE;
		else
		   stop = FALSE;

		/* Est-ce que la selection est en fin de boite ? */
		if (stop)
		  {
		     dummy = i - max + beginInd;
		     /* Position dans les blancs de fin de ligne */
		     selMark->VsIndBox = pBox->BxNChars + dummy;
		     selMark->VsXPos = pBox->BxWidth;
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

	/* a trouve la boite de coupure */
	if (!stop)
	  {
	     selMark->VsIndBox = beginInd + i;
	     /* Reevaluation du decalage dans la boite */
	     /* 0 si on prend la largeur reelle du blanc */
	     spaceWidth = pBox->BxSpaceWidth;
	     /* Index du premier caractere a traiter */
	     x = pBox->BxFirstChar;
	     GiveTextParams (pBox->BxBuffer, selMark->VsIndBox, pBox->BxFont, &x, &spaceWidth);
	     selMark->VsXPos = x;
	     selMark->VsNSpaces = spaceWidth;
	     /* ajoute eventuellement les pixels repartis */
	     if (pBox->BxSpaceWidth != 0)
		if (selMark->VsNSpaces < pBox->BxNPixels)
		   selMark->VsXPos += selMark->VsNSpaces;
		else
		   selMark->VsXPos += pBox->BxNPixels;
	  }
	selMark->VsBox = pBox;
     }
   selMark->VsLine = SearchLine (selMark->VsBox);
}

/*----------------------------------------------------------------------
   InsertViewSelMarks pose la selection courante sur la portion de   
   document visualisee dans une frame du Mediateur. Le     
   pave pAb correspond soit au debut de la selection       
   (debut est Vrai), soit la fin de la selection (fin est  
   vrai), soit les deux. Le parametre firstChar donne      
   lindice du premier caractere selectionne ou 0 si tout   
   le pave est selectionne.                                
   Le parametre lastChar donne l'indice du caractere qui   
   suit le dernier selectionne'.                           
   Le parametre alone indique que la selection reelle     
   donc visualisee porte sur un seul et unique pave.       
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                InsertViewSelMarks (int frame, PtrAbstractBox pAb, int firstChar, int lastChar, ThotBool startSelection, ThotBool endSelection, ThotBool alone)
#else  /* __STDC__ */
void                InsertViewSelMarks (frame, pAb, firstChar, lastChar, startSelection, endSelection, alone)
int                 frame;
PtrAbstractBox      pAb;
int                 firstChar;
int                 lastChar;
ThotBool            startSelection;
ThotBool            endSelection;
ThotBool            alone;

#endif /* __STDC__ */
{
   PtrLine             adline;
   PtrTextBuffer       pBuffer;
   int                 ind, charIndex;
   PtrBox              pBox;
   ViewFrame          *pFrame;
   ViewSelection      *pViewSel;

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
	     adline = SearchLine (pBox);

	     /* verifie la coherence des indices de caracteres */
	     if (pAb->AbLeafType == LtText)
		/* C'est une feuille de texte */
	       {
		  if (firstChar == 0 && lastChar != 0)
		     firstChar = 1;
		  else if (firstChar > 1 && lastChar == 0)
		     lastChar = pAb->AbVolume;
		  else if (firstChar == 0 && pAb->AbVolume != 0)
		    {
		       firstChar = 1;	/* selection tout le texte du pave */
		       lastChar = pAb->AbVolume;
		    }
	       }
	     else if (pAb->AbLeafType != LtPolyLine && pAb->AbLeafType != LtPicture)
		firstChar = 0;

	     /* memorise si la selection relle porte sur un seul pave ou non */
	     pFrame->FrSelectOneBox = alone;
	     /* et si elle indique seulement une position */
	     pFrame->FrSelectOnePosition = SelPosition;

	     /* La selection porte sur le pave complet ou un point de controle */
	     /* de pave polyline */
	     if (firstChar == 0 || pAb->AbVolume == 0 || pAb->AbLeafType == LtPolyLine || pAb->AbLeafType == LtPicture)
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
		       if (endSelection && pAb->AbLeafType != LtPolyLine && pAb->AbLeafType != LtPicture)
			  /* tout selectionne */
			  pViewSel->VsIndBox = 0;
		       else
			  pViewSel->VsIndBox = firstChar;
		       pViewSel->VsIndBuf = ind;
		       pViewSel->VsBuffer = pBuffer;
		       pViewSel->VsLine = adline;
		       if (pAb->AbLeafType == LtPicture && firstChar > 0)
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
		       if (pAb->AbLeafType == LtPicture && firstChar > 0)
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
		       /* startSelection et endSelection sur le meme caractere */
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
			    /* startSelection et endSelection sur deux caracteres differents */
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
			 }

		       /* recherche la position limite du caractere */
		       pBox = pViewSel->VsBox;
		       if (pBox->BxNChars == 0 && pBox->BxType == BoComplete)
			  pViewSel->VsXPos += pBox->BxWidth;
		       else if (pViewSel->VsIndBox == pBox->BxNChars)
			  pViewSel->VsXPos += 2;
		       else
			 {
			    charIndex = (int) (pViewSel->VsBuffer->BuContent[pViewSel->VsIndBuf - 1]);
			    if (charIndex == SPACE && pBox->BxSpaceWidth != 0)
			       pViewSel->VsXPos += pBox->BxSpaceWidth;
			    else
			       pViewSel->VsXPos += CharacterWidth ((unsigned char) charIndex, pBox->BxFont);
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
#ifdef __STDC__
ThotBool            IsAbstractBoxDisplayed (PtrAbstractBox pAb, int frame)
#else  /* __STDC__ */
ThotBool            IsAbstractBoxDisplayed (pAb, frame)
PtrAbstractBox      pAb;
int                 frame;

#endif /* __STDC__ */
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
   if (pAb->AbBox->BxYOrg + pAb->AbBox->BxHeight < min || pAb->AbBox->BxYOrg > max)
      return FALSE;
   else
      return TRUE;
}
