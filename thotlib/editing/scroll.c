/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2008
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * Handle scrolling in document frames
 *
 * Author: I. Vatton (INRIA)
 *
 */
#include "thot_gui.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "appdialogue.h"
#include "picture.h"

#define THOT_EXPORT extern
#include "boxes_tv.h"
#include "appdialogue_tv.h"
#include "frame_tv.h"

#include "absboxes_f.h"
#include "appli_f.h"
#include "applicationapi_f.h"
#include "boxselection_f.h"
#include "buildboxes_f.h"
#include "editcommands_f.h"
#include "exceptions_f.h"
#include "frame_f.h"
#include "structselect_f.h"
#include "textcommands_f.h"
#include "windowdisplay_f.h"

#ifdef _GL
#include "glwindowdisplay.h"
#endif /*_GL*/

/*----------------------------------------------------------------------
  VerticalScroll scrolls forward (delta > 0) or backward (delta < 0).
  Moves by raster-op a part of the window and displays the new part:                  
  - forwardr by RedrawFrameBottom,                       
  - backward by RedrawFrameTop.                     
  The parameter selection is 1 if threre is a selection, 0 if not.
  ----------------------------------------------------------------------*/
void VerticalScroll (int frame, int delta, int selection)
{
  int                 y, height;
  int                 max, width;
  int                 lframe, hframe;
  PtrBox              srcbox;
  ViewFrame          *pFrame;
  PtrAbstractBox      pAb;
  ThotBool            add;
  
  if (delta != 0 && GL_prepare (frame))
    {
      pFrame = &ViewFrameTable[frame - 1];
      if (pFrame->FrReady && pFrame->FrAbstractBox)
        {
          pAb = pFrame->FrAbstractBox;
          /* On termine l'insertion courante */
          CloseTextInsertion ();
          srcbox = pAb->AbBox;
          /* Limites du scroll */
          if (srcbox)
            {
              /* A priori pas de paves ajoutes */
              add = FALSE;
              /* Au plus, la limite du document + le debordement vertical*/
              GetSizesFrame (frame, &lframe, &hframe);
              max = srcbox->BxYOrg + srcbox->BxHeight - pFrame->FrYOrg - hframe;
              if (!pAb->AbTruncatedTail && delta > max)
                {
                  JumpIntoView (frame, 100);
                  return;
                }
              if (pAb->AbTruncatedTail)
                max = delta;
              if (pAb->AbTruncatedHead)
                y = delta;
              else
                {
                  y = -pFrame->FrYOrg;
                  /* Il faut respecter la marge initiale si elle est
                     positive */
                  if (srcbox->BxYOrg < 0)
                    y += srcbox->BxYOrg;
                }
              
              /* Le Scroll est possible --> Calcule l'amplitude du Scroll*/
              if ((delta > 0 && max > 0) || (delta < 0 && y < 0))
                {
                  if (delta > 0)
                    {
                      /* SCROLL forward */
                      if (delta > max)
                        delta = max;
                      y = delta;
                      height = hframe - y;
                      if (lframe > srcbox->BxWidth)
                        /* pay attention to positioning boxes */
                        width = srcbox->BxWidth;
                      else
                        width = lframe + 1;
                      Scroll (frame, width, height, 0, y, 0, 0);
		      height = pFrame->FrYOrg + hframe;
		      DefClip (frame, pFrame->FrXOrg, height,
			       pFrame->FrXOrg + lframe, 
			       height + delta);
                      add = RedrawFrameBottom (frame, delta, NULL);
                    }
                  else
                    {
                      /* SCROLL backward */
                      if (delta < y)
                        delta = y;
                      height = hframe + delta;
                      if (lframe > srcbox->BxWidth)
                        /* pay attention to positioning boxes */
                        width = srcbox->BxWidth;
                      else
                        width = lframe + 1;
                      y = -delta;
                      Scroll (frame, width, height, 0, 0, 0, y);
                      height = pFrame->FrYOrg;
                      DefClip (frame, pFrame->FrXOrg, height + delta,
                               pFrame->FrXOrg + lframe, height);
                      add = RedrawFrameTop (frame, -delta);
                    }
                  /* recompute scrolls */
                  CheckScrollingWidthHeight (frame);
                  UpdateScrollbars (frame);
                }
              
              if (selection != 0)
                if (add)
                  {
                    ClearViewSelMarks (frame);
                    /* On supprime l'ancienne selection */
                    ShowSelection (pFrame->FrAbstractBox, FALSE);
                    /* On reprend la nouvelle */
                    /* On reallume la selection deja visualisee */
                  }
#ifdef _GL
	      /* to be sure the scrolled page has been displayed */
	      GL_Swap( frame );
#endif /* _GL */
            }
        }
    }
}


/*----------------------------------------------------------------------
  HorizontalScroll effectue un scroll en avant (delta > 0) ou en arriere   
  (delta < 0).                                            
  Deplacement par rasterop d'une partie de la fenetre et  
  reaffichage du reste de la fenetre par RedrawFrameBottom.    
  Le parametre selection indique s'il faut gerer la       
  selection (valeur 1) ou non (valeur 0).                 
  ----------------------------------------------------------------------*/
void HorizontalScroll (int frame, int delta, int selection)
{
  int                 x, height;
  int                 min, max, width;
  int                 lframe, hframe;
  PtrBox              srcbox;
  ViewFrame          *pFrame;
  
  if (delta != 0 && GL_prepare (frame))
    {
      pFrame = &ViewFrameTable[frame - 1];
      if (pFrame->FrReady && pFrame->FrAbstractBox != NULL)
        {
          CloseTextInsertion ();
          /* finish the current insertion */
          srcbox = pFrame->FrAbstractBox->AbBox;
          GetSizesFrame (frame, &lframe, &hframe);
          /* FrameTable[frame].FrScrollOrg is negative or null */
          min = FrameTable[frame].FrScrollOrg - pFrame->FrXOrg;
          /* Right limit is the current scroll width + extra */
          max = min + FrameTable[frame].FrScrollWidth - lframe;
          /* Left limit is the current scroll width - extra */
          /* keep the right margin if the document is smaller than the window*/
          if (srcbox->BxXOrg  < 0)
            min += srcbox->BxXOrg;

          if ((delta > 0 && max > 0) || (delta < 0 && min < 0))
            {
              /* Scroll is possible -> compute the delta */
              height = hframe + 1;
              if (delta > 0)
                {
                  /* Scroll right */
                  if (delta > max)
                    delta = max;
                  x = delta;
                  width = lframe - x + 1;
                  Scroll (frame, width, height, x, 0, 0, 0);
                  width = pFrame->FrXOrg + lframe;
                  DefClip (frame, width, pFrame->FrYOrg, width + x,
                           pFrame->FrYOrg + hframe);
                }
              else
                {
                  /* Scroll left */
                  if (delta < min)
                    delta = min;
                  x = -delta;
                  width = lframe - x + 1;
                  Scroll (frame, width, height, 0, 0, x, 0);
                  width = pFrame->FrXOrg;
                  DefClip (frame, width - x, pFrame->FrYOrg, width,
                           pFrame->FrYOrg + hframe);
                }
              
              /* display the rest of the window */
              pFrame->FrXOrg += delta;
              RedrawFrameBottom (frame, 0, NULL);
              /* recompute the scroll bars */
              UpdateScrollbars (frame);
            }
#ifdef _GL
#ifdef DEBUG_MAC
printf ("HorizontalScroll:GL_Swap frame=%d\n",frame);
#endif /* DEBUG_MAC */
          /* to be sure the scrolled page has been displayed */
          GL_Swap( frame );
#endif /* _GL */
        }
    }
}

#ifdef IV
/*----------------------------------------------------------------------
  ShowYPosition forces the root box position in the frame.
  The parameter y gives the requested poosition
  The parameter height give the frame height
  ----------------------------------------------------------------------*/
void ShowYPosition (int frame, int y, int height)
{
  ViewFrame          *pFrame;
  PtrBox              pBox;
  int                 delta;
  int                 shift;

  pFrame = &ViewFrameTable[frame - 1];
  if (pFrame->FrAbstractBox != NULL)
    {
      pBox = pFrame->FrAbstractBox->AbBox;
      if (pBox != NULL)
        {
          /* il faut tenir compte du decalage possible de l'origine */
          /* de la fenetre par rapport au debut de l'Picture Concrete */
          shift = pFrame->FrYOrg;

          if (y == 0)	/* cadrage en haut */
            {
              if (pFrame->FrAbstractBox->AbTruncatedHead)
                JumpIntoView (frame, 0);
              else
                {
                  /* Regarde si le cadrage en haut n'est pas deja
                     effectue */
                  delta = pBox->BxYOrg - shift;
                  if (delta < 0)
                    /* Cadre la boite racine sur le bord gauche de la
                       fenetre */
                    VerticalScroll (frame, delta, 1);
                }
            }
          else if (y == height && !pFrame->FrAbstractBox->AbTruncatedTail)
            /* cadrage en bas */
            {
              /* Recupere la hauteur de la fenetre */
              GetSizesFrame (frame, &delta, &height);
              /* Regarde si le cadrage en bas n'est pas deja effectue */
              delta = pBox->BxYOrg + pBox->BxHeight - pFrame->FrYOrg -
                height;
              if (delta > 0)
                /* Cadre la boite racine en bas de la fenetreentre */
                VerticalScroll (frame, delta, 1);
            }
          else
            {
              /* Regarde si un scroll a la fin du document est demande */
              /* alors que le document n'est pas complet en fin        */
              if (y == height)
                JumpIntoView (frame, 100);
              else
                {
                  /* nouvelle position demandee */
                  delta = (int) ((float) (y * pBox->BxHeight) / (float) height);
                  /* decalage */
                  delta = delta + pBox->BxYOrg - shift;
                  VerticalScroll (frame, delta, 1);
                }
            }
        }
    }
}
#endif

/*----------------------------------------------------------------------
  PositionAbsBox gives the position of the box in the frame.
  Return
   -1: the frame is empty
    0: the frame is full
    1 the top of the document is displayed
    2 the bottom of the document is displayed
    3 neither the top nor the bottom is displayed
  nbCharBegin: gives the number of undisplayed characters before
  nbCharEnd: gives the number of undisplayed characters before
  total: gives the number of characters of the doccument
  ----------------------------------------------------------------------*/
int PositionAbsBox (int frame, int *nbCharBegin, int *nbCharEnd, int *total)
{
  ViewFrame          *pFrame;
  PtrBox              first;
  PtrBox              last;
  int                 h, l;

  /* Initialisation */
  *nbCharBegin = 0;
  *nbCharEnd = 0;
  *total = 0;

  /* Recupere les informations sur la fenetre */
  if (frame < 1 || frame >= MAX_FRAME)
    return -1;

  GetSizesFrame (frame, &l, &h);
  pFrame = &ViewFrameTable[frame - 1];
  /* Est-ce qu'il y a une vue ? */
  if (pFrame->FrAbstractBox == NULL)
    return -1;
  /* Regarde si l'image est complete */
  else if (!pFrame->FrAbstractBox->AbTruncatedHead &&
           !pFrame->FrAbstractBox->AbTruncatedTail)
    {
      first = pFrame->FrAbstractBox->AbBox;
      *total = first->BxHeight;
      return 0;
    }
  else
    {
      /* Repere la position de l'image abstraite dans le document */
      first = pFrame->FrAbstractBox->AbBox->BxNext;
      last = pFrame->FrAbstractBox->AbBox->BxPrevious;
      VolumeTree (pFrame->FrAbstractBox, first->BxAbstractBox,
                  last->BxAbstractBox, nbCharBegin, nbCharEnd, total);

      /* L'image se trouve au debut du document ? */
      if (!pFrame->FrAbstractBox->AbTruncatedHead)
        return 1;
      /* L'image se trouve a la fin du document ? */
      else if (!pFrame->FrAbstractBox->AbTruncatedTail)
        return 2;
      else
        return 3;
    }
}


/*----------------------------------------------------------------------
  ComputeDisplayedChars e'value la portion d'image concre`te affiche'e    
  dans la fenetree^tre frame.                                     
  La proce'dure rend :                                            
  - Xpos : la position en X du premier cararte`re visualise'.     
  - Ypos : la position en Y du premier cararte`re visualise'.     
  - width : la largeur de la portion de l'arbre visualise'e.    
  - height : la hauteur de la portion de l'arbre visualise'e.    
  ----------------------------------------------------------------------*/
void ComputeDisplayedChars (int frame, int *Xpos, int *Ypos, int *width, int *height)
{
  PtrBox              pBoxFirst, pBoxLast;
  PtrBox              pBox;
  ViewFrame          *pFrame;
  PtrFlow             pFlow;
  int                 upFrameLimit, lowFrameLimit;
  int                 min, max;
  int                 h, l, htotal;
  int                 vtotal;
  float               carparpix;
  float               ratio;

  *Xpos = 0;
  *Ypos = 0;
  *width = 1;
  *height = 1;

  if (frame < 1 || frame >= MAX_FRAME)
    return;

  /* get window sizes */
  GetSizesFrame (frame, &l, &h);
  htotal = h;			/* height of the scroll bar */
  pFrame = &ViewFrameTable[frame - 1];

  if (pFrame->FrAbstractBox != NULL)
    {
      pBox = pFrame->FrAbstractBox->AbBox;
      if (pBox == NULL)
        return;
      if (pFrame->FrAbstractBox->AbElement == NULL)
        return;
    }
  else
    return;

  /* Limits of the document */
  /* Bottom limit */
  upFrameLimit = pFrame->FrYOrg - pBox->BxYOrg;
  /* Top limit */
  lowFrameLimit = upFrameLimit + h;
  /* FrameTable[frame].FrScrollOrg is negative or null */
  /* Left limit is the current scroll width */
  min = FrameTable[frame].FrScrollOrg - pFrame->FrXOrg;
  /* Right limit is the current scroll width */
  max = min + FrameTable[frame].FrScrollWidth - l;

  /***     Horizontal scrolling      ***/
  /*        +-------------+             */
  /*        |  Document   |             */
  /* +------+-------------+---------+   */
  /* |  Scrolling zone    |         |   */
  /* |      |             |         |   */
  /* |    +-+-------------+---+     |   */
  /* |    | |             |   |     |   */
  /* |<---| |             |   |---->|   */
  /* | min| |      ViewFrame  | max |   */
  /* |    | |             |   |     |   */
  /* |    +-+-------------+---+     |   */
  /* +------+-------------+---------+   */
  /*        +-------------+             */
  /* ratio of the width of the Concrete Image with the scrolling width */
  ratio = (float) l / (float) FrameTable[frame].FrScrollWidth;

  if (min < 0)
    {
      /* percent of what remains on the left part */
      *Xpos = (int) ((float) (-min) * ratio);
      /* position of the slider */
      if (*Xpos < 4)
        *Xpos = 4;
    }
  else
    /* there is nothing on the left */
    *Xpos = 0;

  if (max > 0)
    {
      /* percent of what remains on the right part */
      *width = (int) ((float) max * ratio);
      /* width of the slider */
      if (*width < 4)
        *width = l - 4 - *Xpos;
      else
        *width = - *width + l - *Xpos;
    }
  else
    /* there is nothing on the left */
    *width = l - *Xpos;

  /***     Vertical scrolling             ***/
  /*      +-------------+                   */
  /*      |  Document   |min                */
  /*    +-+-------------+-+     +-+         */
  /*    |    Concrete     |\    | |         */
  /*    |                 | \   | |max      */
  /*    |+---------------+|  \  | |         */
  /*    ||               ||\  --|-|         */
  /*    ||   ViewFrame   || ====|=|slider   */
  /*    ||               ||/  --|-|         */
  /*    |+---------------+|  /  | |         */
  /*    |                 | /   | |max      */
  /*    |     Image       |/    | |         */
  /*    +-+-------------+-+     +-+         */
  /*      |             |max                */
  /*      +-------------+                   */

  /** Compute the part of the document which is not formatted **/
  if ((!pFrame->FrAbstractBox->AbTruncatedHead) &&
      (!pFrame->FrAbstractBox->AbTruncatedTail))
    {
      /* The whole document is formatted */
      min = 0;
      max = 0;
    }
  else
    {
      if (pBox->BxNext == NULL)
        {
          /* le document est vide (que la racine) */
          min = 0;
          max = 0;
        }
      else
        {
          pBoxFirst = pBox->BxNext;	/* pBoxFirst boite terminale */
          pBoxLast = pBox->BxPrevious;	/* pBoxLast boite terminale */
          /* Elimine les boites de presentations */
          while (pBoxFirst->BxAbstractBox->AbPresentationBox &&
                 pBoxFirst != pBoxLast)
            pBoxFirst = pBoxFirst->BxNext;
          if (pBoxFirst != pBoxLast)
            while (pBoxLast->BxAbstractBox->AbPresentationBox &&
                   pBoxLast->BxPrevious->BxAbstractBox &&
                   pBoxFirst != pBoxLast)
              pBoxLast = pBoxLast->BxPrevious;

          /* Evalue la hauteur et la position Y par rapport a l'image
             abstraite */
          VolumeTree (pFrame->FrAbstractBox, pBoxFirst->BxAbstractBox,
                      pBoxLast->BxAbstractBox, &min, &max, &vtotal);
          /* min gives the volume before the displayed part */
          /* max gives the volume after the displayed part */
          /* Compute the number of characters represented by a pixel height */
          carparpix = (float) vtotal / (float) h;
          if (min > 0)
            min = (int) ((float) min / carparpix);
          if (max > 0)
            max = (int) ((float) max / carparpix);
          /* slider height representing the displayed part */
          h = h - min - max;
        }
    }

  /* check if there are extra flow */
  pFlow = pFrame->FrFlow;
  while (pFlow)
    {
      if (pFlow->FlRootBox && pFlow->FlRootBox->AbBox &&
          pFlow->FlRootBox->AbBox->BxHeight > pBox->BxHeight)
        pBox->BxHeight = pFlow->FlRootBox->AbBox->BxHeight;
      pFlow = pFlow->FlNext;
    }
  /* Rapport hauteur Picture Concrete sur hauteur portion du scroll */
  ratio = (float) h / (float) pBox->BxHeight;
  if (upFrameLimit > 0)
    {
      /* Il reste une portion de document en haut de la fenetre */
      *Ypos = (int) ((float) upFrameLimit * ratio) + min;
      if (*Ypos < 4)
        *Ypos = 4;
    }
  else
    *Ypos = min;

  if (lowFrameLimit < pBox->BxHeight)
    {
      /* Calcul de la portion de document en bas de la fenetre */
      *height = (int) ((float) (pBox->BxHeight - lowFrameLimit) * ratio) + max;
      /* Calcul de la hauteur du slider */
      if (*height < 4)
        *height = htotal - 4 - *Ypos;
      else
        *height = -*height + htotal - *Ypos;
    }
  else
    *height = htotal - *Ypos - max;

  /* Finalement on verifie que la largeur */
  /* la hauteur sont au moins egal a 1    */
  if (*width < 1)
    *width = 1;
  if (*height < 1)
    *height = 1;
}

/*----------------------------------------------------------------------
  ShowBox displays the box pBox at the requested position in the window.
  The parameter position is:
  0 for the top of the window
  1 for the middle of the window
  2 for the bottom of the window
  When the position = 0, percent gives the percent from the top of the
  window.
  scrollUpdate is TRUE when scrollbars must be recomputed
  ----------------------------------------------------------------------*/
void ShowBox (int frame, PtrBox pBox, int position, int percent,
              ThotBool scrollUpdate)
{
  PtrAbstractBox      pBlock;
  PtrBox              pBox1;
  PtrLine             pLine;
  ViewFrame          *pFrame;
  int                 ymin, ymax;
  int                 width, height;
  int                 y, dy, h;

  if (pBox == NULL)
    return;
  pBox1 = pBox;
  pBlock = NULL;
  pLine = NULL;
  if (pBox->BxType == BoGhost ||
      pBox->BxType == BoStructGhost ||
      pBox->BxType == BoFloatGhost)
    {
      while (pBox &&
             (pBox->BxType == BoGhost ||
              pBox->BxType == BoStructGhost ||
              pBox->BxType == BoFloatGhost))
        pBox = pBox->BxAbstractBox->AbFirstEnclosed->AbBox;
      if (!pBox)
        {
          pBox = pBox1;
          while (pBox->BxType == BoGhost ||
                 pBox->BxType == BoStructGhost ||
                 pBox->BxType == BoFloatGhost)
            pBox = pBox->BxAbstractBox->AbEnclosing->AbBox;
        }
      /* manage the line instead of the box itself */
      pLine = SearchLine (pBox, frame);
      if (pLine)
        {
          pBlock = pBox->BxAbstractBox;
          while (pBlock && pBlock->AbBox->BxType != BoBlock &&
                 pBlock->AbBox->BxType != BoFloatBlock &&
                 pBlock->AbBox->BxType != BoCellBlock)
            pBlock = pBlock->AbEnclosing;
        }
    }

  if (pBox->BxType == BoSplit)
    pBox = pBox->BxNexChild;

  pFrame = &ViewFrameTable[frame - 1];
  if (pBlock && pBlock->AbBox)
    {
      y = pBlock->AbBox->BxYOrg + pLine->LiYOrg;
      h = pLine->LiHeight;
    }
  else
    {
      y = pBox->BxYOrg;
      h = pBox->BxHeight;
    }
  /* largeur et hauteur de la fenetre */
  GetSizesFrame (frame, &width, &height);
  ymin = pFrame->FrYOrg;
  ymax = ymin + height;

  /* On debloque eventuellement l'affichage */
  pFrame->FrReady = TRUE;

  if (position == 0)
    /* Affiche le haut de la boite a pourcent du haut de la fenetre */
    dy = y - ymin - ((height * percent) / 100);
  else if (position == 1)
    /* Centre le milieu de la boite sur le milieu de la fenetre */
    dy = y + (h / 2) - ymin - (height / 2);
  else
    /* Affiche en bas de la fenetre */
    dy = y + h - ymax;

  /* Il faut realiser l'affichage par scroll ou par appel explicite */
  if (dy != 0)
    VerticalScroll (frame, dy, 1);
  else if (GL_prepare (frame))
    {
      RedrawFrameBottom (frame, dy, NULL);
#ifdef _GL
      /* to be sure the scrolled page has been displayed */
      GL_Swap (frame);
#endif /* _GL */
      /* Mise a jour des ascenseurs */
      if (scrollUpdate)
        UpdateScrollbars (frame);
    }
}

/*----------------------------------------------------------------------
  IsScrolled checks if the insert point is visible in the frame.
  The parameter selection is set to 1 if the selection must be updated.
  Return:
  -  TRUE is all is okay
  -  FALSE if a scroll was done
  ----------------------------------------------------------------------*/
ThotBool IsScrolled (int frame, int selection)
{
  int                 xmin, xmax;
  int                 ymin, ymax;
  int                 x, y, dx, dy, h;
  ViewFrame          *pFrame;
  PtrBox              pBox;
  PtrAbstractBox      pDraw;
  ThotBool            result;

  pFrame = &ViewFrameTable[frame - 1];
  pBox = pFrame->FrSelectionBegin.VsBox;
  if (pBox == NULL)
    return TRUE;

  // check if the slection is within a SVG draw
  pDraw =GetParentDraw (pBox);
  if (pDraw)
    pBox = pDraw->AbBox;

#ifdef _GL
  if (pBox->BxBoundinBoxComputed)
    {
      x = pBox->BxClipX + pFrame->FrXOrg;
      y = pBox->BxClipY + pFrame->FrYOrg;
    }
  else
#endif /* _GL*/
    {
      x = pBox->BxXOrg + pFrame->FrSelectionBegin.VsXPos;
      y = pBox->BxYOrg;
    }

  h = pBox->BxHeight;
  GetSizesFrame (frame, &dx, &dy);
  xmin = pFrame->FrXOrg;
  xmax = xmin + dx;
  dx = dx / 2;
  ymin = pFrame->FrYOrg;
  ymax = ymin + dy;
  dy = dy / 2;

  /* On debloque eventuellement l'affichage */
  pFrame->FrReady = TRUE;
  if (pBox->BxAbstractBox->AbHorizPos.PosUserSpecified)
    /* C'est une creation interactive de boite, la boite sera */
    /* automatiquement placee dans la fenetre au moment de sa creation */
    result = TRUE;
  else if (x < xmin || x > xmax)
    {
      /* Deplacement du cadre le la fenetre */
      HorizontalScroll (frame, x - xmin - dx, selection);
      result = FALSE;
    }
  else
    result = TRUE;

  if (pBox->BxAbstractBox->AbVertPos.PosUserSpecified)
    /* C'est une creation interactive de boite, la boite sera */
    /* automatiquement placee dans la fenetre au moment de sa creation */
    result = TRUE;
  else if (y < ymin || y + h > ymax)
    {
      /* Deplacement du cadre le la fenetre */
      VerticalScroll (frame, y - ymin - dy, selection);
      result = FALSE;
    }
  else
    result = TRUE;
  return result;
}


/*----------------------------------------------------------------------
  ShowSelectedBox shows the beginning of the selection within the frame
  and makes this frame active if the parameter active is TRUE.
  ----------------------------------------------------------------------*/
void ShowSelectedBox (int frame, ThotBool active)
{
  ViewFrame          *pFrame;
  PtrBox              pBox;
  PtrAbstractBox      pAb = NULL, pDraw;
  int                 xmin, xmax;
  int                 ymin, ymax;
  int                 x, y, dx, dy, w, h;

#ifndef _WX
  /* do not update the ActiveFrame when drawing because the selected frame is not allways the drawn one */
  if (active)
    /* make the window active */
    ChangeSelFrame (frame);
#endif /* _WX */

  pFrame = &ViewFrameTable[frame - 1];
  if (pFrame->FrSelectionBegin.VsBox != NULL && pFrame->FrReady)
    {
      pBox = pFrame->FrSelectionBegin.VsBox;
      if (pBox)
        pAb = pBox->BxAbstractBox;
      if (pBox &&
          (pBox->BxType == BoGhost ||
           pBox->BxType == BoStructGhost ||
           pBox->BxType == BoFloatGhost))
        {
          // get the position of the fist visible box
          while (pAb && pAb->AbBox &&
                 (pAb->AbBox->BxType == BoGhost ||
                  pAb->AbBox->BxType == BoStructGhost ||
                  pAb->AbBox->BxType == BoFloatGhost))
            pAb = pAb->AbFirstEnclosed;
          if (pAb)
            pBox = pAb->AbBox;
          else
            pBox = NULL;
        }
      /* Check if almost one box is displayed */
      while (pBox && pBox->BxAbstractBox &&
             pBox->BxAbstractBox->AbVisibility < pFrame->FrVisibility)
        {
          if (pBox->BxAbstractBox->AbSelected ||
              pBox == pFrame->FrSelectionBegin.VsBox)
            pBox = pBox->BxNext;
          else
            /* no box found */
            return;
        }

      // check the show of the SVG element instead of enclosed constructs
      if (pAb && FrameTable[frame].FrView == 1)
        {
          pDraw = GetParentDraw (pBox);
          if (pDraw)
            {
              pAb = pDraw;
              pBox = pAb->AbBox;
            }
        }

      if (pBox && (pBox->BxType == BoSplit || pBox->BxType == BoMulScript))
        pBox = pBox->BxNexChild;
      if (pBox)
        {
#ifdef _GL
          if (pBox->BxBoundinBoxComputed)
            {
              x = pBox->BxClipX + pFrame->FrXOrg;
              y = pBox->BxClipY + pFrame->FrYOrg;
            }
          else
#endif /* _GL*/
            {
              x = pBox->BxXOrg;
              y = pBox->BxYOrg;
            }
          GetSizesFrame (frame, &w, &h);
          xmin = pFrame->FrXOrg;
          xmax = xmin + w;
          ymin = pFrame->FrYOrg;
          ymax = ymin + h;
          /* center in the window */
          dx = pFrame->FrSelectionBegin.VsXPos;
          dy = 13;
          w /= 2;
          h /= 2;
          if (pBox->BxAbstractBox)
            {
              if (!pBox->BxAbstractBox->AbHorizPos.PosUserSpecified)
                /* the box position is not given by the user */
                {
                  if (x + dx < xmin + 10)
                    /* scroll the window */
                    HorizontalScroll (frame, x + dx - xmin - w, 0);
                  else if (x + dx > xmax - 10)
                    /* scroll the window */
                    HorizontalScroll (frame, x + dx - xmax + w, 0);
                }

              if (!pBox->BxAbstractBox->AbVertPos.PosUserSpecified)
                /* the box position is not given by the user */
                {
#ifdef _GL
                  if (pBox->BxBoundinBoxComputed &&
                      y + pBox->BxClipH < ymin + dy)
                    /* scroll the window */
                    VerticalScroll (frame, y + pBox->BxClipH - ymin - h, 0);
                    else if (y + pBox->BxHeight < ymin + dy)
                    /* scroll the window */
                    VerticalScroll (frame, y + pBox->BxHeight - ymin - h, 0);
                  else if (y > ymax - dy)
                    /* scroll the window */
                    VerticalScroll (frame, y - ymax + h, 0);
#endif /* _GL */
                }
            }
        }
    }
}


/*----------------------------------------------------------------------
  IsAbsBoxVisible retourne Vrai si le pAb deborde de la          
  frame du document.                                       
  ----------------------------------------------------------------------*/
ThotBool IsAbsBoxVisible (int frame, PtrAbstractBox pAb)
{
  ViewFrame          *pFrame;
  int                 y, ymax;
  int                 dx, dy;

  pFrame = &ViewFrameTable[frame - 1];
  if (pAb == NULL)
    return (FALSE);		/* pas de pave a tester */
  else if (pAb->AbBox == NULL)
    return (FALSE);		/* pas de boite a tester */
  else if (pAb->AbVisibility < pFrame->FrVisibility ||
           pAb->AbBox->BxType == BoGhost ||
           pAb->AbBox->BxType == BoStructGhost ||
           pAb->AbBox->BxType == BoFloatGhost)
    return (FALSE);		/* la boite n'est pas visible par definition */
  else
    {
#ifndef _GL
      y = pAb->AbBox->BxYOrg;
      GetSizesFrame (frame, &dx, &dy);
      ymax = pFrame->FrYOrg + dy;
#else /* _GL */
      y = pAb->AbBox->BxClipY;
      GetSizesFrame (frame, &dx, &dy);
      ymax = dy;
#endif /* _GL */
      if (y <= ymax)
        return (FALSE);
      else
        return (TRUE);
    }
}
