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
 * visualisation of Selections.
 *
 * Author: I. Vatton (INRIA)
 *
 */

#include "ustring.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "appdialogue.h"

#define THOT_EXPORT extern
#include "boxes_tv.h"
#include "platform_tv.h"
#include "select_tv.h"
#include "edit_tv.h"
#include "frame_tv.h"

#include "windowdisplay_f.h"
#include "displayselect_f.h"
#include "font_f.h"
#include "units_f.h"
#include "exceptions_f.h"


/*----------------------------------------------------------------------
   DisplayBoxSelection trace le contour de la boite pBox dans les   
   limites de la fenetre.                                   
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         DisplayBoxSelection (int frame, PtrBox pBox, int pointselect)
#else  /* __STDC__ */
static void         DisplayBoxSelection (frame, pBox, pointselect)
int                 frame;
PtrBox              pBox;
int                 pointselect;

#endif /* __STDC__ */
{
  PtrBox              pChildBox;
  ViewFrame          *pFrame;
  PtrAbstractBox      pAb;
  PtrTextBuffer       pBuffer;
  int                 leftX, rightX;
  int                 topY, bottomY;
  int                 minX;
  int                 middleY;
  int                 i, j, n;

  if (pBox != NULL)
    {
      pFrame = &ViewFrameTable[frame - 1];
      pAb = pBox->BxAbstractBox;
      
      if (pBox->BxType == BoGhost
	  || (pAb != NULL
	      && TypeHasException (ExcHighlightChildren, pAb->AbElement->ElTypeNumber, pAb->AbElement->ElStructSchema)))
	{
	  /* -> La boite est eclatee (boite fantome) */
	  /* On visualise toutes les boites filles */
	  if (pAb->AbFirstEnclosed != NULL)
	    {
	      pChildBox = pAb->AbFirstEnclosed->AbBox;
	      while (pChildBox != NULL)
		{
		  DisplayBoxSelection (frame, pChildBox, 0);
		  pAb = pChildBox->BxAbstractBox;
		  if (pAb->AbNext != NULL)
		    pChildBox = pAb->AbNext->AbBox;
		  else
		    pChildBox = NULL;
		}
	    }
	}
      else if (pBox->BxType != BoSplit)
	{
	  /* La boite est entiere */
	  leftX = pBox->BxXOrg - pFrame->FrXOrg;
	  topY = pBox->BxYOrg - pFrame->FrYOrg;
	  bottomY = topY + pBox->BxHeight;
	  rightX = leftX + pBox->BxWidth;
	  minX = leftX + pBox->BxWidth / 2;
	  middleY = topY + pBox->BxHeight / 2;

	  if (pAb == NULL)
	    /* C'est une boite sans pave */
	    VideoInvert (frame, rightX - leftX, bottomY - topY, leftX, topY);
	  else if (pAb->AbLeafType == LtPolyLine && pBox->BxNChars > 1)
	    {
	      /* C'est une boite polyline */
	      /* On marque le(s) point(s) caracteristique(s) de la polyline */
	      /* si la polyline contient au moins 1 point (effectif) */
	      pBuffer = pBox->BxBuffer;
	      leftX = pBox->BxXOrg - pFrame->FrXOrg;
	      topY = pBox->BxYOrg - pFrame->FrYOrg;
	      j = 1;
	      n = pBox->BxNChars;
	      for (i = 1; i < n; i++)
		{
		  if (j >= pBuffer->BuLength)
		    {
		      if (pBuffer->BuNext != NULL)
			{
			  /* Changement de buffer */
			  pBuffer = pBuffer->BuNext;
			  j = 0;
			}
		    }
		  if (pointselect == 0 || pointselect == i)
		    VideoInvert (frame, HANDLE_WIDTH, HANDLE_WIDTH,
				 leftX + PointToPixel (pBuffer->BuPoints[j].XCoord / 1000) - 2,
				 topY + PointToPixel (pBuffer->BuPoints[j].YCoord / 1000) - 2);
		  j++;
		}
	    }
	  else if (pAb->AbLeafType == LtPicture)
	    {
	      /* 4 points caracteristiques */
	      VideoInvert (frame, HANDLE_WIDTH, HANDLE_WIDTH, leftX - 2, topY - 2);
	      VideoInvert (frame, HANDLE_WIDTH, HANDLE_WIDTH, minX - 2, topY - 2);
	      VideoInvert (frame, HANDLE_WIDTH, HANDLE_WIDTH, rightX - 3, topY - 2);
	      VideoInvert (frame, HANDLE_WIDTH, HANDLE_WIDTH, leftX - 2, middleY - 2);
	      VideoInvert (frame, HANDLE_WIDTH, HANDLE_WIDTH, rightX - 3, middleY - 2);
	      VideoInvert (frame, HANDLE_WIDTH, HANDLE_WIDTH, leftX - 2, bottomY - 3);
	      VideoInvert (frame, HANDLE_WIDTH, HANDLE_WIDTH, minX - 2, bottomY - 3);
	      VideoInvert (frame, HANDLE_WIDTH, HANDLE_WIDTH, rightX - 3, bottomY - 3);
	    }
	  else if (pAb->AbLeafType == LtGraphics && pAb->AbVolume != 0)
	    /* C'est une boite graphique */
	    /* On marque en noir les points caracteristiques de la boite */
	    switch (pAb->AbRealShape)
	      {
	      case SPACE:
	      case TEXT('R'):
	      case TEXT('0'):
	      case TEXT('1'):
	      case TEXT('2'):
	      case TEXT('3'):
	      case TEXT('4'):
	      case TEXT('5'):
	      case TEXT('6'):
	      case TEXT('7'):
	      case TEXT('8'):
		/* 8 points caracteristiques */
		VideoInvert (frame, HANDLE_WIDTH, HANDLE_WIDTH, leftX - 2, topY - 2);
		VideoInvert (frame, HANDLE_WIDTH, HANDLE_WIDTH, minX - 2, topY - 2);
		VideoInvert (frame, HANDLE_WIDTH, HANDLE_WIDTH, rightX - 3, topY - 2);
		VideoInvert (frame, HANDLE_WIDTH, HANDLE_WIDTH, leftX - 2, middleY - 2);
		VideoInvert (frame, HANDLE_WIDTH, HANDLE_WIDTH, rightX - 3, middleY - 2);
		VideoInvert (frame, HANDLE_WIDTH, HANDLE_WIDTH, leftX - 2, bottomY - 3);
		VideoInvert (frame, HANDLE_WIDTH, HANDLE_WIDTH, minX - 2, bottomY - 3);
		VideoInvert (frame, HANDLE_WIDTH, HANDLE_WIDTH, rightX - 3, bottomY - 3);
		break;
	      case 'C':
	      case 'L':
	      case 'a':
	      case 'c':
	      case 'P':
	      case 'Q':
		/* 4 points caracteristiques */
		VideoInvert (frame, HANDLE_WIDTH, HANDLE_WIDTH, minX - 2, topY - 2);
		VideoInvert (frame, HANDLE_WIDTH, HANDLE_WIDTH, leftX - 2, middleY - 2);
		VideoInvert (frame, HANDLE_WIDTH, HANDLE_WIDTH, rightX - 3, middleY - 2);
		VideoInvert (frame, HANDLE_WIDTH, HANDLE_WIDTH, minX - 2, bottomY - 3);
		break;
	      case 'W':
		VideoInvert (frame, HANDLE_WIDTH, HANDLE_WIDTH, leftX - 2, topY - 2);
		VideoInvert (frame, HANDLE_WIDTH, HANDLE_WIDTH, rightX - 3, topY - 2);
		VideoInvert (frame, HANDLE_WIDTH, HANDLE_WIDTH, rightX - 3, bottomY - 2);
		break;
	      case 'X':
		VideoInvert (frame, HANDLE_WIDTH, HANDLE_WIDTH, rightX - 3, topY - 2);
		VideoInvert (frame, HANDLE_WIDTH, HANDLE_WIDTH, rightX - 3, bottomY - 2);
		VideoInvert (frame, HANDLE_WIDTH, HANDLE_WIDTH, leftX - 2, bottomY - 2);
		break;
	      case 'Y':
		VideoInvert (frame, HANDLE_WIDTH, HANDLE_WIDTH, rightX - 3, bottomY - 2);
		VideoInvert (frame, HANDLE_WIDTH, HANDLE_WIDTH, leftX - 2, bottomY - 2);
		VideoInvert (frame, HANDLE_WIDTH, HANDLE_WIDTH, leftX - 2, topY - 2);
		break;
	      case 'Z':
		VideoInvert (frame, HANDLE_WIDTH, HANDLE_WIDTH, leftX - 2, bottomY - 2);
		VideoInvert (frame, HANDLE_WIDTH, HANDLE_WIDTH, leftX - 2, topY - 2);
		VideoInvert (frame, HANDLE_WIDTH, HANDLE_WIDTH, rightX - 3, topY - 2);
		break;
		
	      case 'h':
	      case '<':
	      case '>':
		/* 2 points caracteristiques */
		VideoInvert (frame, HANDLE_WIDTH, HANDLE_WIDTH, leftX - 2, middleY - 2);
		VideoInvert (frame, HANDLE_WIDTH, HANDLE_WIDTH, rightX - 3, middleY - 2);
		break;
	      case 't':
		/* 3 points caracteristiques */
		VideoInvert (frame, HANDLE_WIDTH, HANDLE_WIDTH, leftX - 2, topY - 2);
		VideoInvert (frame, HANDLE_WIDTH, HANDLE_WIDTH, minX - 2, topY - 2);
		VideoInvert (frame, HANDLE_WIDTH, HANDLE_WIDTH, rightX - 3, topY - 2);
		break;
	      case 'b':
		/* 3 points caracteristiques */
		VideoInvert (frame, HANDLE_WIDTH, HANDLE_WIDTH, leftX - 2, bottomY - 3);
		VideoInvert (frame, HANDLE_WIDTH, HANDLE_WIDTH, minX - 2, bottomY - 3);
		VideoInvert (frame, HANDLE_WIDTH, HANDLE_WIDTH, rightX - 3, bottomY - 3);
		break;
	      case 'v':
	      case '^':
	      case 'V':
		/* 2 points caracteristiques */
		VideoInvert (frame, HANDLE_WIDTH, HANDLE_WIDTH, minX - 2, topY - 2);
		VideoInvert (frame, HANDLE_WIDTH, HANDLE_WIDTH, minX - 2, bottomY - 3);
		break;
	      case 'l':
		/* 3 points caracteristiques */
		VideoInvert (frame, HANDLE_WIDTH, HANDLE_WIDTH, leftX - 2, topY - 2);
		VideoInvert (frame, HANDLE_WIDTH, HANDLE_WIDTH, leftX - 2, middleY - 2);
		VideoInvert (frame, HANDLE_WIDTH, HANDLE_WIDTH, leftX - 2, bottomY - 3);
		break;
	      case 'r':
		/* 3 points caracteristiques */
		VideoInvert (frame, HANDLE_WIDTH, HANDLE_WIDTH, rightX - 3, topY - 2);
		VideoInvert (frame, HANDLE_WIDTH, HANDLE_WIDTH, rightX - 3, middleY - 2);
		VideoInvert (frame, HANDLE_WIDTH, HANDLE_WIDTH, rightX - 3, bottomY - 3);
		break;
	      case '\\':
	      case 'O':
	      case 'e':
		/* 2 points caracteristiques */
		VideoInvert (frame, HANDLE_WIDTH, HANDLE_WIDTH, leftX - 2, topY - 2);
		VideoInvert (frame, HANDLE_WIDTH, HANDLE_WIDTH, rightX - 3, bottomY - 3);
		break;
	      case '/':
	      case 'o':
	      case 'E':
		/* 2 points caracteristiques */
		VideoInvert (frame, HANDLE_WIDTH, HANDLE_WIDTH, rightX - 3, topY - 2);
		VideoInvert (frame, HANDLE_WIDTH, HANDLE_WIDTH, leftX - 2, bottomY - 3);
		break;
	      default:
		break;
	      }
	  /* C'est une boite d'un autre type */
	  else
	    VideoInvert (frame, rightX - leftX, bottomY - topY, leftX, topY);
	}
      else
	{
	  /* La boite est coupee */
	  /* Calcul des points caracteristiques de la premiere boite coupee */
	  pChildBox = pBox->BxNexChild;
	  while (pChildBox != NULL)
	    {
	      DisplayBoxSelection (frame, pChildBox, 0);
	      pChildBox = pChildBox->BxNexChild;
	    }
	}
    }
}

/*----------------------------------------------------------------------
   SetNewSelectionStatus parcourt l'arborescence pour basculer la    
   mise en evidence de la selection et forcer le nouvel    
   etat de selection.                                      
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                SetNewSelectionStatus (int frame, PtrAbstractBox pAb, ThotBool status)
#else  /* __STDC__ */
void                SetNewSelectionStatus (frame, pAb, status)
int                 frame;
PtrAbstractBox      pAb;
ThotBool            status;

#endif /* __STDC__ */
{
   PtrAbstractBox      pChildAb;
   ViewFrame          *pFrame;

   if (pAb != NULL)
     {
	/* Le pave est selectionne */
	if (pAb->AbSelected)
	  {
	     pFrame = &ViewFrameTable[frame - 1];
	     /* On ne visualise pas les bornes de la selection */
	     if (pFrame->FrSelectionBegin.VsBox == NULL ||
		 pFrame->FrSelectionEnd.VsBox == NULL)
		DisplayBoxSelection (frame, pAb->AbBox, 0);
	     else if (pAb != pFrame->FrSelectionBegin.VsBox->BxAbstractBox &&
		      pAb != pFrame->FrSelectionEnd.VsBox->BxAbstractBox)
		DisplayBoxSelection (frame, pAb->AbBox, 0);
	     pAb->AbSelected = status;
	  }
	else
	   /* Sinon on parcours le sous-arbre */
	  {
	     pChildAb = pAb->AbFirstEnclosed;
	     while (pChildAb != NULL)
	       {
		  SetNewSelectionStatus (frame, pChildAb, status);
		  pChildAb = pChildAb->AbNext;
	       }
	  }
     }
}

/*----------------------------------------------------------------------
  DisplayStringSelection trace le contour d'une chaine de charcteres
  dans la boite de texte pBox dans les limites de la fenetre entre leftX
  et rightX.
  S'il s'agit d'une polyline, allume le point selectionne.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         DisplayStringSelection (int frame, int leftX, int rightX, PtrBox pBox)
#else  /* __STDC__ */
static void         DisplayStringSelection (frame, leftX, rightX, pBox)
int                 frame;
int                 leftX;
int                 rightX;
PtrBox              pBox;
#endif /* __STDC__ */
{
  PtrBox              pParentBox;
  ViewFrame          *pFrame;
  PtrAbstractBox      pAb;
  int                 width, height;
  int                 topY, h;

  pFrame = &ViewFrameTable[frame - 1];
  if (pBox->BxAbstractBox != NULL)
    /* On limite la visibilite de la selection aux portions de texte */
    /* affichees dans les paragraphes.                               */
    {
      /* Si on holophraste la racine du document */
      /* le texte n'a pas de pave englobant */
      if (pBox->BxAbstractBox->AbEnclosing == NULL)
	pParentBox = pBox;
      else
	{
	  pParentBox = pBox->BxAbstractBox->AbEnclosing->AbBox;
	  while (pParentBox->BxType == BoGhost)
	    {
	      pAb = pParentBox->BxAbstractBox;
	      if (pAb->AbEnclosing == NULL)
		pParentBox = pBox;
	      else
		pParentBox = pAb->AbEnclosing->AbBox;
	    }
	}
      /* clipped by the enclosing box */
      height = pParentBox->BxYOrg + pParentBox->BxHeight - pFrame->FrYOrg;
      /* and the scrolling zone */
      width = FrameTable[frame].FrScrollOrg + FrameTable[frame].FrScrollWidth - pFrame->FrXOrg;

      topY = pBox->BxYOrg - pFrame->FrYOrg;
      h = pBox->BxHeight;
      if (topY > height)
	h = 0;
      else if (topY + h > height)
	h = height - topY;
      
      leftX = leftX + pBox->BxXOrg - pFrame->FrXOrg;
      if (leftX > width)
	width = 0;
      else
	{
	  rightX = rightX + pBox->BxXOrg - pFrame->FrXOrg;
	  if (rightX > width)
	    width -= leftX;
	  else
	    width = rightX - leftX;
	}
      VideoInvert (frame, width, h, leftX, topY);
    }
}


/*----------------------------------------------------------------------
   DisplayCurrentSelection shows or hides the current selection.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                DisplayCurrentSelection (int frame, ThotBool status)
#else  /* __STDC__ */
void                DisplayCurrentSelection (frame, status)
int                 frame;
ThotBool            status;

#endif /* __STDC__ */
{
  PtrBox              pBox;
  PtrBox              pSelBox;
  PtrAbstractBox      pAb;
  ViewFrame          *pFrame;
  ViewSelection      *pViewSel;
  ViewSelection      *pViewSelEnd;

  /* On ne visualise la selection que si la selection est coherente */
  if (documentDisplayMode[FrameTable[frame].FrDoc - 1] == DisplayImmediately)
    {
      pFrame = &ViewFrameTable[frame - 1];
      pViewSel = &pFrame->FrSelectionBegin;
      if (pViewSel->VsBox != NULL && pFrame->FrSelectionEnd.VsBox != NULL)
	{
	  /* Est-ce que les marques de selection sont dans la meme boite ? */
	  if (pFrame->FrSelectionEnd.VsBox == pViewSel->VsBox)
	    if (pFrame->FrSelectOnePosition)
	      {
		/* on ne visualise qu'une position position */
		if (pViewSel->VsBox->BxAbstractBox->AbLeafType == LtPolyLine)
		  DisplayBoxSelection (frame, pViewSel->VsBox, pViewSel->VsIndBox);
		else
		  DisplayStringSelection (frame, pViewSel->VsXPos, pViewSel->VsXPos + 2, pViewSel->VsBox);
	      }
	    else if (pViewSel->VsBuffer == NULL
		     || (pViewSel->VsBox->BxNChars == 0
			 && (pViewSel->VsBox->BxType != BoSplit
			     || pViewSel->VsBox->BxType != BoPiece)))
	      DisplayBoxSelection (frame, pViewSel->VsBox, pViewSel->VsIndBox);
	    else
	      DisplayStringSelection (frame, pViewSel->VsXPos, pFrame->FrSelectionEnd.VsXPos, pViewSel->VsBox);
	  else
	    /* Les marques de selection sont dans deux boites differentes */
	    /* Si les deux bornes de la selection sont compatibles */
	    {
	      pAb = NULL;
	      pSelBox = pViewSel->VsBox;
	      if (pViewSel->VsBuffer == NULL || pSelBox->BxNChars == 0)
		DisplayBoxSelection (frame, pViewSel->VsBox, 0);
	      else
		{
		  pAb = pSelBox->BxAbstractBox;
		  /* Est-ce que la selection debute en fin de boite ? */
		  if (pViewSel->VsXPos == pSelBox->BxWidth)
		    DisplayStringSelection (frame, pViewSel->VsXPos,
					    pSelBox->BxWidth + 2, pViewSel->VsBox);
		  else
		    DisplayStringSelection (frame, pViewSel->VsXPos, pSelBox->BxWidth,
					    pViewSel->VsBox);
		  /* Parcours les boites coupees soeurs */
		  if (pSelBox->BxType == BoPiece || pSelBox->BxType == BoDotted)
		    {
		      pBox = pSelBox->BxNexChild;
		      while (pBox != NULL &&
			     pBox != pFrame->FrSelectionEnd.VsBox)
			{
			  if (pBox->BxNChars > 0)
			    DisplayBoxSelection (frame, pBox, 0);
			  pBox = pBox->BxNexChild;
			}
		    }
		}
	      pViewSelEnd = &pFrame->FrSelectionEnd;
	      pSelBox = pViewSelEnd->VsBox;
	      if (pViewSelEnd->VsBuffer == NULL || pSelBox->BxNChars == 0)
		DisplayBoxSelection (frame, pViewSelEnd->VsBox, 0);
	      else
		{
		  /* Parcours les boites coupees soeurs */
		  if ((pSelBox->BxType == BoPiece || pSelBox->BxType == BoDotted)
		      && pSelBox->BxAbstractBox != pAb)
		    {
		      pBox = pSelBox->BxAbstractBox->AbBox->BxNexChild;
		      while (pBox != NULL &&
			     pBox != pViewSelEnd->VsBox)
			{
			  DisplayBoxSelection (frame, pBox, 0);
			  pBox = pBox->BxNexChild;
			}
		    }
		  DisplayStringSelection (frame, 0, pViewSelEnd->VsXPos, pViewSelEnd->VsBox);
		}
	    }
	  /* Bascule l'indicateur de la selection allumee */
	  pFrame->FrSelectShown = !pFrame->FrSelectShown;
	  
	  SetNewSelectionStatus (frame, pFrame->FrAbstractBox, status);
	}
      else if (status == FALSE)
	/* Annule la selection meme s'il n'y a plus de boite selectionnee */
	pFrame->FrSelectShown = FALSE;
    }
}
