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
 * Handle scrolling in document frames
 *
 * Author: I. Vatton (INRIA)
 *
 */

#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "appdialogue.h"

#define THOT_EXPORT extern
#include "boxes_tv.h"
#include "appdialogue_tv.h"
#include "frame_tv.h"

#include "absboxes_f.h"
#include "appli_f.h"
#include "applicationapi_f.h"
#include "boxselection_f.h"
#include "buildboxes_f.h"
#include "frame_f.h"
#include "structselect_f.h"
#include "textcommands_f.h"
#include "windowdisplay_f.h"


/*----------------------------------------------------------------------
   VerticalScroll effectue un scroll en avant (delta > 0) ou en arriere   
   (delta<0).                                              
   Deplacement par raster-op d'une partie de la fenetre et 
   reaffichage du reste de la fenetre :                    
   - soit en avant par RedrawFrameBottom,                       
   - soit en arriere par RedrawFrameTop.                     
   Le parametre selection indique s'il faut ge'rer la      
   selection (valeur 1) ou non (valeur 0).                 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                VerticalScroll (int frame, int delta, int selection)
#else  /* __STDC__ */
void                VerticalScroll (frame, delta, selection)
int                 frame;
int                 delta;
int                 selection;
#endif /* __STDC__ */

{
   int                 y, height;
   int                 max, width;
   int                 lframe, hframe;
   PtrBox              srcbox;
   ThotBool            add;
   ViewFrame          *pFrame;
   PtrAbstractBox      pAbb;

   if (delta != 0)
     {
	pFrame = &ViewFrameTable[frame - 1];
	if (pFrame->FrReady && pFrame->FrAbstractBox != NULL)
	  {
	     pAbb = pFrame->FrAbstractBox;
	     /* On termine l'insertion courante */
	     CloseInsertion ();
	     srcbox = pAbb->AbBox;
	     /* Limites du scroll */
	     if (srcbox != NULL)
	       {
		  /* On eteint la selection */
		  if (selection != 0 && ThotLocalActions[T_switchsel])
		    (*ThotLocalActions[T_switchsel]) (frame, FALSE);

		  /* A priori pas de paves ajoutes */
		  add = FALSE;
		  /* Au plus, la limite du document + le debordement vertical */
		  GetSizesFrame (frame, &lframe, &hframe);
		  if (pAbb->AbTruncatedTail)
		     max = delta;
		  else
		     max = srcbox->BxYOrg + srcbox->BxHeight - pFrame->FrYOrg - hframe;
		  if (pAbb->AbTruncatedHead)
		     y = delta;
		  else
		    {
		       y = -pFrame->FrYOrg;
		       /* Il faut respecter la marge initiale si elle est positive */
		       if (srcbox->BxYOrg < 0)
			  y += srcbox->BxYOrg;
		    }

		  /* Le Scroll est possible --> Calcule l'amplitude du Scroll */
		  if ((delta > 0 && max > 0) || (delta < 0 && y < 0))
		     if (delta > 0)
		       {
			  /* SCROLL AVANT */
			  if (delta > max)
			     delta = max;
			  y = delta;
			  height = hframe - y;
			  width = lframe + 1;
			  Scroll (frame, width, height, 0, y, 0, 0);
			  height = pFrame->FrYOrg + hframe;
			  DefClip (frame, pFrame->FrXOrg, height,
				   pFrame->FrXOrg + lframe, height + delta);
			  add = RedrawFrameBottom (frame, delta);
		       }
		     else
		       {
			  /* SCROLL ARRIERE */
			  if (delta < y)
			     delta = y;
			  height = hframe + delta;
			  width = lframe + 1;
			  y = -delta;
			  Scroll (frame, width, height, 0, 0, 0, y);
			  height = pFrame->FrYOrg;
			  DefClip (frame, pFrame->FrXOrg, height + delta,
				   pFrame->FrXOrg + lframe, height);
			  add = RedrawFrameTop (frame, -delta);
			  /* On reallume eventuellement une selection cachee */
		       }

		  /* recompute scrolls */
		  CheckScrollingWidth (frame);
		  UpdateScrollbars (frame);

		  if (selection != 0)
		     if (add)
		       {
			  ClearViewSelMarks (frame);
			  /* On supprime l'ancienne selection */
			  ShowSelection (pFrame->FrAbstractBox, FALSE);
			  /* On reprend la nouvelle */
			  /* On reallume la selection deja visualisee */
		       }
		     else if (ThotLocalActions[T_switchsel])
		       (*ThotLocalActions[T_switchsel]) (frame, TRUE);
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
#ifdef __STDC__
void                HorizontalScroll (int frame, int delta, int selection)

#else  /* __STDC__ */
void                HorizontalScroll (frame, delta, selection)
int                 frame;
int                 delta;
int                 selection;

#endif /* __STDC__ */

{
  int                 x, height;
  int                 min, max, width;
  int                 lframe, hframe;
  PtrBox              srcbox;
  ViewFrame          *pFrame;

  if (delta != 0)
    {
      pFrame = &ViewFrameTable[frame - 1];
      if (pFrame->FrReady && pFrame->FrAbstractBox != NULL)
	{
	  CloseInsertion ();
	  /* finish the current insertion */
	  srcbox = pFrame->FrAbstractBox->AbBox;
	  /* switch off the selection */
	  if (selection != 0 && ThotLocalActions[T_switchsel])
	    (*ThotLocalActions[T_switchsel]) (frame, FALSE);
	  GetSizesFrame (frame, &lframe, &hframe);
	  /* FrameTable[frame].FrScrollOrg is negative or null */
	  min = FrameTable[frame].FrScrollOrg - pFrame->FrXOrg;
	  /* Right limit is the current scroll width + extra */
	  max = min + FrameTable[frame].FrScrollWidth - lframe;
	  /* Left limit is the current scroll width - extra */
	  /* keep the right margin if the document is smaller than the window */
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
		  DefClip (frame, width, pFrame->FrYOrg, width + x, pFrame->FrYOrg + hframe);
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
		  DefClip (frame, width - x, pFrame->FrYOrg, width, pFrame->FrYOrg + hframe);
		}

	      /* display the rest of the window */
	      pFrame->FrXOrg += delta;
	      RedrawFrameBottom (frame, 0);
	      /* recompute the scroll bars */
	      UpdateScrollbars (frame);
	    }
	  
	  /* switch on the selection */
	  if (selection != 0 && ThotLocalActions[T_switchsel])
	    (*ThotLocalActions[T_switchsel]) (frame, TRUE);
	}
    }
}


/*----------------------------------------------------------------------
   ShowYPosition force la position du bord haut de la boi^te        
   racine de la vue dans sa framee^tre.                    
   La parame`tre frame de'signe la fenetree^tre de la vue. 
   Le parame`tre y donne la position demande'e et le       
   parame`tre haut la hauteur de la fenetree^tre.          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ShowYPosition (int frame, int y, int height)
#else  /* __STDC__ */
void                ShowYPosition (frame, y, height)
int                 frame;
int                 y;
int                 height;

#endif /* __STDC__ */
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
		       /* Regarde si le cadrage en haut n'est pas deja effectue */
		       delta = pBox->BxYOrg - shift;
		       if (delta < 0)
			  /* Cadre la boite racine sur le bord gauche de la fenetre */
			  VerticalScroll (frame, delta, 1);
		    }
	       }
	     else if (y == height && !pFrame->FrAbstractBox->AbTruncatedTail)	/* cadrage en bas */
	       {
		  /* Recupere la hauteur de la fenetre */
		  GetSizesFrame (frame, &delta, &height);
		  /* Regarde si le cadrage en bas n'est pas deja effectue */
		  delta = pBox->BxYOrg + pBox->BxHeight - pFrame->FrYOrg - height;
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


/*----------------------------------------------------------------------
   PositionAbsBox rend la position de l'image abstraite de la fenetre^tre      
   frame dans le document.                                 
   Cette fonction retourne la valeur :                              
   -1 si l'image abstraite est vide.                                
   0 si l'image abstraite est complete.                            
   1 si l'image abstraite est situe'e au de'but du document.       
   2 si l'image abstraite est situe'e a` la fin du document.       
   3 si l'image abstraite est situe'e au milieu du document.       
   nbCharBegin indique le nombre de carate`res avant l'image abstraite.  
   nbCharEnd indique le nombre de carate`res apres l'image abstraite.    
   total indique le nombre total de carate`res du document.        
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                 PositionAbsBox (int frame, int *nbCharBegin, int *nbCharEnd, int *total)
#else  /* __STDC__ */
int                 PositionAbsBox (frame, nbCharBegin, nbCharEnd, total)
int                 frame;
int                *nbCharBegin;
int                *nbCharEnd;
int                *total;
#endif /* __STDC__ */

{
   ViewFrame          *pFrame;
   PtrBox              premiere;
   PtrBox              derniere;
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
   else if (!pFrame->FrAbstractBox->AbTruncatedHead && !pFrame->FrAbstractBox->AbTruncatedTail)
     {
	premiere = pFrame->FrAbstractBox->AbBox;
	*total = premiere->BxHeight;
	return 0;
     }
   else
     {
	/* Repere la position de l'image abstraite dans le document */
	premiere = pFrame->FrAbstractBox->AbBox->BxNext;
	derniere = pFrame->FrAbstractBox->AbBox->BxPrevious;
	VolumeTree (pFrame->FrAbstractBox, premiere->BxAbstractBox, derniere->BxAbstractBox,
		    nbCharBegin, nbCharEnd, total);

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
   - Ypos : la position en X du premier cararte`re visualise'.     
   - width : la largeur de la portion de l'arbre visualise'e.    
   - height : la hauteur de la portion de l'arbre visualise'e.    
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ComputeDisplayedChars (int frame, int *Xpos, int *Ypos, int *width, int *height)

#else  /* __STDC__ */
void                ComputeDisplayedChars (frame, Xpos, Ypos, width, height)
int                 frame;
int                *Xpos;
int                *Ypos;
int                *width;
raint              *height;

#endif /* __STDC__ */

{
   PtrBox              pBoxFirst;
   PtrBox              pBoxLast;
   PtrBox              pBox;
   ViewFrame          *pFrame;
   int                 upDocPos, lowDocPos;
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
     }
   else
      return;

   /* Limits of the document */
   /* Bottom limit */
   upDocPos = pFrame->FrYOrg - pBox->BxYOrg;
   /* Top limit */
   lowDocPos = upDocPos + h;
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
   /*      |  Document   |a                  */
   /*    +-+-------------+-+     +-+         */
   /*    |    Concrete     |\    | |         */
   /*    |                 | \   | |a        */
   /*    |+---------------+|  \  | |         */
   /*    ||               ||\  --|-|         */
   /*    ||   ViewFrame   || ====|=|slider   */
   /*    ||               ||/  --|-|         */
   /*    |+---------------+|  /  | |         */
   /*    |                 | /   | |b        */
   /*    |     Image       |/    | |         */
   /*    +-+-------------+-+     +-+         */
   /*      |             |b                  */
   /*      +-------------+                   */

   /** Compute the part of the document which is not formatted **/
   if ((!pFrame->FrAbstractBox->AbTruncatedHead) && (!pFrame->FrAbstractBox->AbTruncatedTail))
     {
	/* The whole document is formatted */
	if (pFrame->FrYOrg - pBox->BxYOrg <= 0)
	   /* The beginning of the document */
	   min = 0;
	else if (pFrame->FrYOrg >= pBox->BxYOrg)
	   /* le debordement du haut n'est pas visualible */
	   min = 0;
	else
	   min = - pFrame->FrYOrg + pBox->BxYOrg;	/* marge haut */

	if (pBox->BxYOrg + pBox->BxHeight <= pFrame->FrYOrg + h)
	   /* Le bas de l'Picture Concrete + le debordement du bas sont visibles */
	   max = 0;
	else if (pBox->BxYOrg + pBox->BxHeight >= pFrame->FrYOrg + h)
	   /* le debordement du bas n'est pas visualible */
	   max = 0;
	else
	   /* marge restant en bas */
	   max = pBox->BxYOrg + pBox->BxHeight - pFrame->FrYOrg - h;
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
	     while (pBoxFirst->BxAbstractBox->AbPresentationBox && pBoxFirst != pBoxLast)
		pBoxFirst = pBoxFirst->BxNext;
	     while (pBoxLast->BxAbstractBox->AbPresentationBox && pBoxFirst != pBoxLast)
		pBoxLast = pBoxLast->BxPrevious;

	     /* Evalue la hauteur et la position Y par rapport a l'image abstraite */
	     VolumeTree (pFrame->FrAbstractBox, pBoxFirst->BxAbstractBox, pBoxLast->BxAbstractBox, &min, &max, &vtotal);
	     /* min donne le volume qui precede l'Picture Concrete */
	     /* max donne le volume qui suit l'Picture Concrete */

	     /* Calcule le nombre de caracteres representes par un pixel */
	     carparpix = (float) vtotal / (float) h;
	     if (min > 0)
	       min = (int) ((float) min / carparpix);
	     if (max > 0)
	       max = (int) ((float) max / carparpix);
	     /* Portion du scroll occupee par l'Picture Concrete */
	     h = h - min - max;
	  }
     }

   /* Rapport hauteur Picture Concrete sur hauteur portion du scroll */
   ratio = (float) h / (float) pBox->BxHeight;
   if (upDocPos > 0)
     {
	/* Il reste une portion de document en haut de la fenetre */
	*Ypos = (int) ((float) upDocPos * ratio) + min;
	if (*Ypos < 4)
	   *Ypos = 4;
     }
   else
      *Ypos = min;

   if (lowDocPos < pBox->BxHeight)
     {
	/* Calcul de la portion de document en bas de la fenetre */
	*height = (int) ((float) (pBox->BxHeight - lowDocPos) * ratio) + max;
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
   ShowBox force la position de la boi^te dans la fenetree^tre        
   affiche'e sur l'e'cran.                                 
   La parame`tre pBox identifie la boi^te a` montrer.      
   Le parame`tre position indique s'il faut afficher en    
   haut (0), au milieu (1) ou en bas (2).                  
   Si postion = 0, percent indique la distance entre le   
   haut de la fenetre et le haut de la boite, exprimee en  
   pourcentage de la hauteur de la fenetre.                
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ShowBox (int frame, PtrBox pBox, int position, int percent)
#else  /* __STDC__ */
void                ShowBox (frame, pBox, position, percent)
int                 frame;
PtrBox              pBox;
int                 position;
int                 percent;

#endif /* __STDC__ */
{
   int                 ymin, ymax;
   int                 width, height;
   int                 y, dy, h;
   ViewFrame          *pFrame;

   if (pBox == NULL)
      return;
   while (pBox->BxType == BoGhost)
     pBox = pBox->BxAbstractBox->AbEnclosing->AbBox;

   if (pBox->BxType == BoSplit)
     pBox = pBox->BxNexChild;

   pFrame = &ViewFrameTable[frame - 1];
   y = pBox->BxYOrg;
   h = pBox->BxHeight;
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
   else
      {
      RedrawFrameBottom (frame, dy);
      /* Mise a jour des ascenseurs */
      UpdateScrollbars (frame);
      }
}

/*----------------------------------------------------------------------
   IsScrolled regarde si la marque d'insertion (de'but de se'lection) 
   est visible dans la fenetree^tre affiche'e sur l'e'cran.        
   Si c'est le cas, la fonction rend la valeur vrai.       
   Si cela n'est pas le cas, la fonction demande le        
   de'placement horizontal ou vertical de la portion       
   d'image concre`te affiche'e et rend la valeur fausse.   
   Le parame`tre selection indique s'il faut ge'rer la     
   se'lection (valeur 1) ou non (valeur 0).                
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool            IsScrolled (int frame, int selection)
#else  /* __STDC__ */
ThotBool            IsScrolled (frame, selection)
int                 frame;
int                 selection;

#endif /* __STDC__ */
{
   int                 xmin, xmax;
   int                 ymin, ymax;
   int                 x, y, dx, dy, h;
   ViewFrame          *pFrame;
   PtrBox              pBo1;
   ThotBool            result;

   pFrame = &ViewFrameTable[frame - 1];
   pBo1 = pFrame->FrSelectionBegin.VsBox;
   x = pBo1->BxXOrg + pFrame->FrSelectionBegin.VsXPos;
   y = pBo1->BxYOrg;
   h = pBo1->BxHeight;
   GetSizesFrame (frame, &dx, &dy);
   xmin = pFrame->FrXOrg;
   xmax = xmin + dx;
   dx = dx / 2;
   ymin = pFrame->FrYOrg;
   ymax = ymin + dy;
   dy = dy / 2;

   /* On debloque eventuellement l'affichage */
   pFrame->FrReady = TRUE;
   if (pBo1->BxAbstractBox->AbHorizPos.PosUserSpecified)
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

   if (pBo1->BxAbstractBox->AbVertPos.PosUserSpecified)
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
   ShowSelectedBox montre le de'but de la se'lection dans la      
   frame et rend cette frame active si l'indicateur actif     
   est Vrai.                                               
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ShowSelectedBox (int frame, ThotBool actif)
#else  /* __STDC__ */
void                ShowSelectedBox (frame, actif)
int                 frame;
ThotBool            actif;

#endif /* __STDC__ */
{
   ViewFrame          *pFrame;
   PtrBox              pBo1;
   int                 xmin, xmax;
   int                 ymin, ymax;
   int                 x, y, dx, dy;

   /* Faut-il rendre la fenetre active (frame d'insertion de texte) */
   if (actif)
      ChangeSelFrame (frame);

   pFrame = &ViewFrameTable[frame - 1];
   if (pFrame->FrSelectionBegin.VsBox != NULL && pFrame->FrReady)
     {
	pBo1 = pFrame->FrSelectionBegin.VsBox;

	/* Verifie qu'une des boites selectionnees est visible */
	while (pBo1 != NULL && pBo1->BxAbstractBox != NULL &&
	       pBo1->BxAbstractBox->AbVisibility < pFrame->FrVisibility)
	  {
	     if (pBo1->BxAbstractBox->AbSelected || pBo1 == pFrame->FrSelectionBegin.VsBox)
		pBo1 = pBo1->BxNext;
	     else
		return;		/* aucune boite n'est visible */
	  }
	if (pBo1 != NULL)
	  {
	     x = pBo1->BxXOrg;
	     y = pBo1->BxYOrg;
	     GetSizesFrame (frame, &dx, &dy);
	     xmin = pFrame->FrXOrg;
	     xmax = xmin + dx;
	     ymin = pFrame->FrYOrg;
	     ymax = ymin + dy;
	     /* MilieuX de la fenetre */
	     dx = pFrame->FrSelectionBegin.VsXPos;
	     dy = 13;

	     if (pBo1->BxAbstractBox != NULL)
	       {
		 if (!pBo1->BxAbstractBox->AbHorizPos.PosUserSpecified)
		   /* the box position is not given by the user */
		   if (x + dx < xmin + 10)
		     /* scroll the window */
		     HorizontalScroll (frame, x + dx - xmin - 10, 0);
		   else if (x + dx > xmax - 10)
		     /* scroll the window */
		     HorizontalScroll (frame, x + dx - xmax + 10, 0);

		 if (!pBo1->BxAbstractBox->AbVertPos.PosUserSpecified)
		   /* the box position is not given by the user */
		   if (y + pBo1->BxHeight < ymin + dy)
		     /* scroll the window */
		     VerticalScroll (frame, y + pBo1->BxHeight - ymin - dy, 0);
		   else if (y > ymax - dy)
		     /* scroll the window */
		     VerticalScroll (frame, y - ymax + dy, 0);
	       }
	  }
     }
}


/*----------------------------------------------------------------------
   IsAbsBoxVisible retourne Vrai si le pAb deborde de la          
   frame du document.                                       
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool            IsAbsBoxVisible (int frame, PtrAbstractBox pAb)
#else  /* __STDC__ */
ThotBool            IsAbsBoxVisible (frame, pAb)
int                 frame;
PtrAbstractBox      pAb;

#endif /* __STDC__ */
{
   ViewFrame          *pFrame;
   int                 y, ymax;
   int                 dx, dy;

   pFrame = &ViewFrameTable[frame - 1];
   if (pAb == NULL)
      return (FALSE);		/* pas de pave a tester */
   else if (pAb->AbBox == NULL)
      return (FALSE);		/* pas de boite a tester */
   else if (pAb->AbVisibility < pFrame->FrVisibility || pAb->AbBox->BxType == BoGhost)
      return (FALSE);		/* la boite n'est pas visible par definition */
   else
     {
	y = pAb->AbBox->BxYOrg;
	GetSizesFrame (frame, &dx, &dy);
	ymax = pFrame->FrYOrg + dy;
	if (y <= ymax)
	   return (FALSE);
	else
	   return (TRUE);
     }
}
