/*
 * frame.c : incremental display in frames.
 */

#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "libmsg.h"
#include "message.h"

#undef EXPORT
#define EXPORT extern
#include "boxes_tv.h"
#include "platform_tv.h"

#include "displaybox_f.h"
#include "appli_f.h"
#include "windowdisplay_f.h"
#include "boxlocate_f.h"
#include "frame_f.h"
#include "font_f.h"
#include "absboxes_f.h"
#include "displayselect_f.h"

/** ------------------------------------------------------------------- 
 *   GetXYOrg : do a coordinate shift related to current frame.
 *  ------------------------------------------------------------------- **/

#ifdef __STDC__
void                GetXYOrg (int frame, int *XOrg, int *YOrg)

#else  /* __STDC__ */
void                GetXYOrg (frame, XOrg, YOrg)
int                 frame;
int                *XOrg;
int                *YOrg;

#endif /* __STDC__ */

{
   ViewFrame            *pFrame;

   pFrame = &ViewFrameTable[frame - 1];
   *XOrg = pFrame->FrXOrg;
   *YOrg = pFrame->FrYOrg;
}

/** ----------------------------------------------------------------------
 *      DefClip defines the area of the frame which need to be redrawn.
 *  ---------------------------------------------------------------------- **/

#ifdef __STDC__
void                DefClip (int frame, int xd, int yd, int xf, int yf)

#else  /* __STDC__ */
void                DefClip (frame, xd, yd, xf, yf)
int                 frame;
int                 xd;
int                 yd;
int                 xf;
int                 yf;

#endif /* __STDC__ */

{
   int                 width, height;
   ViewFrame            *pFrame;

   if (frame > 0 && frame <= MAX_FRAME)
     {
	pFrame = &ViewFrameTable[frame - 1];
	/* Faut-il prendre toute la largeur de la fenetre? */
	if (xd == xf && xd == -1)
	  {
	     DimFenetre (frame, &width, &height);
	     pFrame->FrClipXBegin = pFrame->FrXOrg;
	     pFrame->FrClipXEnd = width + pFrame->FrXOrg;
	  }
	/* On termine un reaffichage */
	else if (xd == xf && xd == 0)
	  {
	     pFrame->FrClipXBegin = 0;
	     pFrame->FrClipXEnd = 0;
	  }
	/* Faut-il initialiser la zone de reaffichage? */
	else if (pFrame->FrClipXBegin == pFrame->FrClipXEnd && pFrame->FrClipXBegin == 0)
	  {
	     pFrame->FrClipXBegin = xd;
	     pFrame->FrClipXEnd = xf;
	  }
	/* On met a jour la zone de reaffichage */
	else
	  {
	     if (pFrame->FrClipXBegin > xd)
		pFrame->FrClipXBegin = xd;
	     if (pFrame->FrClipXEnd < xf)
		pFrame->FrClipXEnd = xf;
	  }
	/* Faut-il prendre toute la heighteur de la fenetre? */
	if (yd == yf && yd == -1)
	  {
	     DimFenetre (frame, &width, &height);
	     pFrame->FrClipYBegin = pFrame->FrYOrg;
	     pFrame->FrClipYEnd = height + pFrame->FrYOrg;
	     /* On termine un reaffichage */
	  }
	else if (yd == yf && yd == 0)
	  {
	     pFrame->FrClipYBegin = 0;
	     pFrame->FrClipYEnd = 0;
	  }
	/* Faut-il initialiser la zone de reaffichage? */
	else if (pFrame->FrClipYBegin == pFrame->FrClipYEnd && pFrame->FrClipYBegin == 0)
	  {
	     pFrame->FrClipYBegin = yd;
	     pFrame->FrClipYEnd = yf;
	  }
	/* On met a jour la zone de reaffichage */
	else
	  {
	     if (pFrame->FrClipYBegin > yd)
		pFrame->FrClipYBegin = yd;
	     if (pFrame->FrClipYEnd < yf)
		pFrame->FrClipYEnd = yf;
	  }
     }
}				/*DefClip */


/** ----------------------------------------------------------------------
 *      DefRegion store the area of frame which need to be redrawn.
 *  ---------------------------------------------------------------------- **/
#ifdef __STDC__
void                DefRegion (int frame, int xd, int yd, int xf, int yf)

#else  /* __STDC__ */
void                DefRegion (frame, xd, yd, xf, yf)
int                 frame;
int                 xd;
int                 yd;
int                 xf;
int                 yf;

#endif /* __STDC__ */

{
   ViewFrame            *pFrame;


   pFrame = &ViewFrameTable[frame - 1];
   DefClip (frame, xd + pFrame->FrXOrg, yd + pFrame->FrYOrg, xf + pFrame->FrXOrg, yf + pFrame->FrYOrg);
}

/** ----------------------------------------------------------------------
 *      TtaRefresh redraw all the frame of all the loaded documents.
 *  ---------------------------------------------------------------------- **/
void                TtaRefresh ()
{
   int                 frame;

   for (frame = 1; frame <= MAX_FRAME; frame++)
     {
	if (ViewFrameTable[frame - 1].FrAbstractBox != NULL)
	  {
	     /* force le reaffichage de toute la fenetre */
	     DefClip (frame, -1, -1, -1, -1);
	     (void) RedrawFrameBottom (frame, 0);
	  }
     }
}

/** ----------------------------------------------------------------------
 *      RedrawFrameTop redraw from bottom to top a frame.
 *		The delta parameter indicates the height of a scroll
 *		back which may take place before recomputing the abstract
 *		image.
 *              The area is cleaned before redrawing.
 *		The origin coordinates of the abstract boxes are expected
 *		to be already computed.
 *              Return non zero if new abstract boxes were added in order
 *		to build the corresponding abstract image.
 *  ---------------------------------------------------------------------- **/
#ifdef __STDC__
boolean             RedrawFrameTop (int frame, int delta)

#else  /* __STDC__ */
boolean             RedrawFrameTop (frame, delta)
int                 frame;
int                 delta;

#endif /* __STDC__ */

{
   PtrBox            pBox;
   PtrBox            min, max;
   int                 y, x, vol, h, l;
   int                 height, bottom;
   int                 framexmin;
   int                 framexmax;
   int                 frameymin;
   int                 frameymax;
   boolean             audessous;
   boolean             toadd;
   int                 plane;
   int                 nextplane;
   PtrBox            firstbox;
   ViewFrame            *pFrame;
   PtrBox            pBo1;
   PtrBox            pBo2;

   /* A priori on n'ajoute pas de nouveaux paves */
   toadd = FALSE;
   pFrame = &ViewFrameTable[frame - 1];
   if (pFrame->FrReady && pFrame->FrAbstractBox != NULL
       && pFrame->FrClipXBegin < pFrame->FrClipXEnd
       && pFrame->FrClipYBegin < pFrame->FrClipYEnd)
     {
	pFrame->FrYOrg -= delta;
	framexmin = pFrame->FrClipXBegin;
	framexmax = pFrame->FrClipXEnd;
	frameymin = pFrame->FrClipYBegin;
	frameymax = pFrame->FrClipYEnd;
	SetClip (frame, pFrame->FrXOrg, pFrame->FrYOrg, &framexmin, &frameymin, &framexmax, &frameymax, 1);
	DimFenetre (frame, &l, &h);
	height = pFrame->FrYOrg;
	bottom = height + h;
	/* S'il y a quelque chose a reafficher ? */
	if (framexmin < framexmax && frameymin < frameymax)
	  {
	     /* Recherche la 1ere boite au dessus du bottom de la fenetre */
	     /* La recherche commence par la derniere boite affichee */
	     pBox = pFrame->FrAbstractBox->AbBox->BxPrevious;
	     if (pBox == NULL)
		/* Document vide */
		pBox = pFrame->FrAbstractBox->AbBox;
	     audessous = TRUE;
	     min = NULL;
	     max = NULL;
	     vol = 0;
	     /* 1/2 frame au dessous du bottom de la fenetre */
	     x = bottom + h / 2;
	     while (audessous)
	       {
		  pBo1 = pBox;
		  if (pBo1->BxYOrg < bottom)
		     audessous = FALSE;
		  else
		    {
		       if (pBo1->BxYOrg > x)
			  if (pBo1->BxType != BoPiece && pBo1->BxType != BoDotted)
			     /* Ce n'est pas une boite de coupure */
			     vol += pBo1->BxAbstractBox->AbVolume;
			  else if (pBo1->BxAbstractBox->AbBox->BxNexChild == pBox)
			     /* C'est la 1ere boite de coupure */
			     vol += pBo1->BxAbstractBox->AbVolume;
		       if (pBo1->BxPrevious == NULL)
			  audessous = FALSE;
		       pBox = pBo1->BxPrevious;
		    }
	       }

	     /* On affiche les planes dans l'orde n a 0 */
	     plane = 65536;
	     nextplane = plane - 1;
	     firstbox = pBox;
	     while (plane != nextplane)
		/* Il y a un nouveau plane a afficher */
	       {
		  plane = nextplane;
		  pBox = firstbox;
		  /* On reparcourt toutes les boites */

		  /* Affiche toutes les boites non encore entierement affichees */
		  while (pBox != NULL)
		    {
		       pBo1 = pBox;
		       if (pBo1->BxAbstractBox->AbDepth == plane)
			  /* La boite est affichee dans le plane courant */
			 {
			    y = pBo1->BxYOrg + pBo1->BxHeight;
			    /* Il faut tenir compte du remplissage de fin de bloc */
			    x = pBo1->BxXOrg + pBo1->BxWidth + pBo1->BxEndOfBloc;
			    /* On note la derniere boite au dessus ou non visible */
			    if (y > height && pBo1->BxYOrg < bottom)
			      {
				 if (max == NULL)
				    max = pBox;
				 min = pBox;
			      }
			    if (y >= frameymin
				&& pBo1->BxYOrg <= frameymax
				&& x >= framexmin
				&& pBo1->BxXOrg <= framexmax)
			       DisplayBox (pBox, frame);
			    /* On passe a la boite nextante */
			    pBox = pBo1->BxPrevious;
			 }
		       else if (pBo1->BxAbstractBox->AbDepth < plane)
			  /* On retient la plus grande valeur de plane inferieur */
			 {
			    if (plane == nextplane)
			       nextplane = pBo1->BxAbstractBox->AbDepth;
			    else if (pBo1->BxAbstractBox->AbDepth > nextplane)
			       nextplane = pBo1->BxAbstractBox->AbDepth;
			    pBox = pBo1->BxPrevious;
			 }
		       else
			  pBox = pBo1->BxPrevious;
		    }
	       }

	     /* La zone modifiee est affichee */
	     DefClip (frame, 0, 0, 0, 0);
	     ResetClip (frame);

	     /* On complete eventuellement l'image partielle */
	     pBox = pFrame->FrAbstractBox->AbBox;
	     if (!FrameUpdating && !TextInserting)
	       {
		  pBo1 = pBox;
		  FrameUpdating = TRUE;
		  /* On est en train de completer l'image */
		  y = height - pBo1->BxYOrg;
		  x = h / 2;

		  /* L'image depasse d'une 1/2 frame en height et en bottom */
		  if (vol > 0 && y > x)
		     /* On calcule le volume a retirer */
		    {
		       pBox = pBo1->BxNext;
		       height -= x;
		       y = 0;
		       while (pBox != NULL)
			 {
			    pBo2 = pBox;
			    if (pBo2->BxYOrg + pBo2->BxHeight > height)
			       pBox = NULL;
			    else
			      {
				 pBox = pBo2->BxNext;
				 if (pBo2->BxType != BoPiece && pBo2->BxType != BoDotted)
				    y += pBo2->BxAbstractBox->AbVolume;
				 else if (pBo2->BxNexChild == NULL)
				    y += pBo2->BxAbstractBox->AbVolume;
				 /* else while */
			      }
			 }
		       pFrame->FrVolume = pFrame->FrAbstractBox->AbVolume - vol - y;
		    }

		  /* Il manque un morceau d'image concrete en height de la fenetre */
		  else if (pFrame->FrAbstractBox->AbTruncatedHead && y < 0)
		    {
		       /* On libere des paves en bottom */
		       if (vol > 0 && vol < pFrame->FrAbstractBox->AbVolume)
			 {
			    DecreaseVolume (FALSE, vol, frame);
			    DefClip (frame, 0, 0, 0, 0);
			    /* On complete en height -> On decale toute l'image concrete */
			 }

		       /* Volume a toaddr */
		       if (pFrame->FrAbstractBox == NULL)
			 {
			    printf ("ERR: plus de pave dans %d\n", frame);
			    vol = -pFrame->FrVolume;
			 }
		       else
			  vol = pFrame->FrVolume - pFrame->FrAbstractBox->AbVolume;

		       /* Hauteur a toaddr */
		       x -= y;
		       if (vol <= 0)
			 {
			    /* Volume de la surface manquante */
			    vol = x * l;
			    /* Evaluation en caracteres */
			    vol = VolumCar (vol);
			 }
		       if (min != NULL)
			 {
			    pBo2 = min;
			    y = pBo2->BxYOrg;
			    /* Ancienne limite de la fenetre */
			    x = y + pBo2->BxHeight;
			 }
		       IncreaseVolume (TRUE, vol, frame);
		       /* On a toadd des paves */
		       toadd = TRUE;
		       /* Il faut repositionner la fenetre dans l'image concrete */
		       if (min != NULL)
			 {
			    pBo2 = min;
			    y = -y + pBo2->BxYOrg;
			    /* y=deplacement de l'ancienne 1ere boite */
			    /* Ce qui est deja affiche depend de l'ancienne 1ere boite */
			    pFrame->FrYOrg += y;
			    /* x = limite du reaffichage apres decalage */
			    if (y > 0)
			       x = pBo2->BxYOrg + pBo2->BxHeight;
			    /* Nouvelle position limite */
			    pFrame->FrClipYEnd = x;
			 }
		       else
			  /* Il n'y a pas d'ancienne boite : */
			  /* la fenetre est cadree en height de l'image */
			  pFrame->FrYOrg = 0;

		       /* On a fini de completer l'image */
		       FrameUpdating = FALSE;
		       audessous = RedrawFrameTop (frame, 0);
		    }

		  /* Il manque un morceau d'image concrete en bottom de la fenetre */
		  else if (pFrame->FrAbstractBox->AbTruncatedTail && bottom > pBo1->BxYOrg + pBo1->BxHeight)
		    {
		       y = pBo1->BxYOrg + pBo1->BxHeight;
		       /* Volume de la surface manquante */
		       vol = (bottom - y) * l;
		       /* Evaluation en caracteres */
		       vol = VolumCar (vol);

		       /* On nettoie le bottom de la fenetre */
		       Clear (frame, l, pFrame->FrYOrg + h - y, 0, y);
		       /* Il faut eviter de boucler quand le volume n'est pas modifie */
		       y = pFrame->FrAbstractBox->AbVolume;
		       IncreaseVolume (FALSE, vol, frame);
		       y -= pFrame->FrAbstractBox->AbVolume;
		       /* On a fini de completer l'image */
		       FrameUpdating = FALSE;
		       if (y == 0)
			  printf ("ERR: Il n'y a rien a toaddr\n");
		       else
			  /* On n'a peut-etre pas fini de completer l'image */
			  audessous = RedrawFrameBottom (frame, 0);
		    }

		  /* Le volume est satisfaisant */
		  else
		     pFrame->FrVolume = pFrame->FrAbstractBox->AbVolume;
		  /* On a fini de completer l'image */
		  FrameUpdating = FALSE;
	       }
	  }
	else
	   /* La zone modifiee n'est pas visible */
	   DefClip (frame, 0, 0, 0, 0);

     }
   else if (pFrame->FrReady)
     {
	/* Il n'y avait rien a afficher */
	DefClip (frame, 0, 0, 0, 0);
	ResetClip (frame);
     }

   return toadd;
}				/* function RedrawFrameTop */


/** ----------------------------------------------------------------------
 *      AddBoxToCreate store in adbloc the list of child boxes to be created,
 *		from the most englobing box down to pBox itself.
 *		It ensure unicity of boxes referenced in adbloc.
 *  ---------------------------------------------------------------------- **/


#ifdef __STDC__
static void                AddBoxToCreate (PtrBox * tocreate, PtrBox pBox, int frame)

#else  /* __STDC__ */
static void                AddBoxToCreate (tocreate, pBox, frame)
PtrBox           *tocreate;
PtrBox            pBox;
int                 frame;

#endif /* __STDC__ */

{
   PtrAbstractBox             pAbbox1;
   int                 i;

   /* Look for and ancestor abstract boxe with PavGraphCreation flag set */
   pAbbox1 = pBox->BxAbstractBox->AbEnclosing;
   i = 0;
   while (i == 0)
      if (pAbbox1 == NULL)
	 i = 1;
      else if (pAbbox1->AbBox->BxHorizFlex
	       || pAbbox1->AbBox->BxVertFlex)
	{
	   i = 1;
	   pAbbox1 = NULL;
	}
      else if ((pAbbox1->AbHorizPos.PosUserSpecified)
	       || (pAbbox1->AbVertPos.PosUserSpecified)
	       || (pAbbox1->AbWidth.DimUserSpecified)
	       || (pAbbox1->AbHeight.DimUserSpecified))
	 i = 1;
      else
	 pAbbox1 = pAbbox1->AbEnclosing;

   /* There is an englobing abstract box */
   if (pAbbox1 != NULL)
      AddBoxToCreate (tocreate, pAbbox1->AbBox, frame);

   /* Add this box to create, if there is no englobing box */
   /* already registered and if this box is visible.       */
   if (*tocreate == NULL
       && pBox->BxAbstractBox->AbVisibility >= ViewFrameTable[frame - 1].FrVisibility)
      *tocreate = pBox;
}				/*function AddBoxToCreate */

/** ----------------------------------------------------------------------
 *      RedrawFrameBottom redraw from top to bottom a frame.
 *		The delta parameter indicates the height of a scroll
 *		which may take place before recomputing the abstract
 *		image.
 *              The area is cleaned before redrawing.
 *		The origin coordinates of the abstract boxes are expected
 *		to be already computed.
 *              Return non zero if new abstract boxes were added in order
 *		to build the corresponding abstract image.
 *  ---------------------------------------------------------------------- **/

#ifdef __STDC__
boolean             RedrawFrameBottom (int frame, int delta)

#else  /* __STDC__ */
boolean             RedrawFrameBottom (frame, delta)
int                 frame;
int                 delta;

#endif /* __STDC__ */

{
   PtrBox            pBox;
   PtrBox            min /*, max */ ;
   int                 y, x, vol, h, l;
   int                 height, bottom;
   int                 framexmin;
   int                 framexmax;
   int                 frameymin;
   int                 frameymax;
   boolean             ontop;
   boolean             toadd;
   int                 plane;
   int                 nextplane;
   PtrBox            firstbox;
   ViewFrame            *pFrame;
   PtrBox            pBo1;
   PtrAbstractBox             pAbbox1;
   PtrBox            pBo2;
   PtrBox            ToCreate;
   int                 i;

   /* used to store new abstract boxes */
   toadd = FALSE;
   /* used to store boxes created on the fly */
   ToCreate = NULL;

   pFrame = &ViewFrameTable[frame - 1];
   if (pFrame->FrReady && pFrame->FrAbstractBox != NULL
       && pFrame->FrClipXBegin < pFrame->FrClipXEnd
       && pFrame->FrClipYBegin < pFrame->FrClipYEnd)
     {
	pFrame->FrYOrg += delta;
	framexmin = pFrame->FrClipXBegin;
	framexmax = pFrame->FrClipXEnd;
	frameymin = pFrame->FrClipYBegin;
	frameymax = pFrame->FrClipYEnd;
	SetClip (frame, pFrame->FrXOrg, pFrame->FrYOrg, &framexmin, &frameymin, &framexmax, &frameymax, 1);
	DimFenetre (frame, &l, &h);
	height = pFrame->FrYOrg;
	bottom = height + h;

	/* Search the first visible box or the one below */
	pBox = pFrame->FrAbstractBox->AbBox->BxNext;
	if (pBox == NULL)
	   pBox = pFrame->FrAbstractBox->AbBox;	/* empty document */

	ontop = TRUE;
	min = NULL;
	vol = 0;
	x = height - h / 2;

	/* Display planes in reverse order from biggest to lowest */
	plane = 65536;
	nextplane = plane - 1;
	firstbox = pBox;
	while (plane != nextplane)
	   /* there is a new plane to display */
	  {
	     plane = nextplane;
	     pBox = firstbox;
	     /* browse all the boxes */

	     /* Draw all the boxes not yet displayed */
	     while (pBox != NULL)
	       {

		  /* Will this volume be on top ? */
		  if (ontop)
                     if (pBox->BxYOrg + pBox->BxHeight < height - h / 4)
		       {
                          if ((pBox->BxType != BoPiece &&
			       pBox->BxType != BoDotted) ||
			      pBox->BxNexChild == NULL)
			     vol += pBox->BxAbstractBox->AbVolume;
		       }
		     else
			ontop = FALSE;

		  if (pBox->BxAbstractBox != NULL && pBox->BxAbstractBox->AbDepth == plane)
		    {
		       /* The box is drawn in the current plane */
		       y = pBox->BxYOrg + pBox->BxHeight;

		       /* take into account the filling of the end of the block */
		       x = pBox->BxXOrg + pBox->BxWidth + pBox->BxEndOfBloc;

		       /* Save the first visible box */
		       if (y > height && pBox->BxYOrg < bottom)
			 {
			    if (min == NULL)
			       min = pBox;
			 }

		       /* If a box is drawn for the first time, check if   */
		       /* it (or one of it's ancestors) has to be created  */
		       /* dynamically */
		       if (pBox->BxNew)
			 {
			    /* this is no a new box */
			    pBox->BxNew = 0;
			    pAbbox1 = pBox->BxAbstractBox;
			    i = 0;
			    while (i == 0)
			       if (pAbbox1 == NULL)
				  i = 1;
			       else if (pAbbox1->AbWidth.DimIsPosition
					|| pAbbox1->AbHeight.DimIsPosition)
				 {
				    i = 1;
				    pAbbox1 = NULL;
				 }
			       else if ((pAbbox1->AbHorizPos.PosUserSpecified)
					|| (pAbbox1->AbVertPos.PosUserSpecified)
					|| (pAbbox1->AbWidth.DimUserSpecified)
					|| (pAbbox1->AbHeight.DimUserSpecified))
				  i = 1;
			       else
				  pAbbox1 = pAbbox1->AbEnclosing;
			 }
		       else
			  pAbbox1 = NULL;

/*** skip box to create interractively ***/
		       if (pAbbox1 != NULL)
			  /* store the box to create */
			  AddBoxToCreate (&ToCreate, pAbbox1->AbBox, frame);
		       else if (y >= frameymin
				&& pBox->BxYOrg <= frameymax
				&& x >= framexmin
				&& pBox->BxXOrg <= framexmax)
			  DisplayBox (pBox, frame);

		       /* Skip to next box */
		       pBox = pBox->BxNext;
		    }
		  else if (pBox->BxAbstractBox != NULL && pBox->BxAbstractBox->AbDepth < plane)
		    {
		       /* keep the lowest value for plane depth */
		       if (plane == nextplane)
			  nextplane = pBox->BxAbstractBox->AbDepth;
		       else if (pBox->BxAbstractBox->AbDepth > nextplane)
			  nextplane = pBox->BxAbstractBox->AbDepth;
		       pBox = pBox->BxNext;
		    }
		  else
		     pBox = pBox->BxNext;
	       }
	  }

	/* Interractive creation of boxes missing */
	if (ToCreate != NULL)
	  {
	     ModeCreation (ToCreate, frame);
	     /* Should the son's boxes being handled too ? */
	     pAbbox1 = ToCreate->BxAbstractBox;
	     if (pAbbox1 != NULL)
		pAbbox1 = pAbbox1->AbFirstEnclosed;
	     while (pAbbox1 != NULL)
	       {
		  if ((pAbbox1->AbHorizPos.PosUserSpecified)
		      || (pAbbox1->AbVertPos.PosUserSpecified)
		      || (pAbbox1->AbWidth.DimUserSpecified)
		      || (pAbbox1->AbHeight.DimUserSpecified))
		    {
		       ToCreate = pAbbox1->AbBox;
		       ModeCreation (ToCreate, frame);
		    }
		  else
		     pAbbox1 = pAbbox1->AbNext;
	       }
	     return (FALSE);
	  }

	/* The updated area is redrawn */
	DefClip (frame, 0, 0, 0, 0);
	ResetClip (frame);

	/* If needed complete the partial existing image */
	pBox = pFrame->FrAbstractBox->AbBox;
	if (!FrameUpdating && (!TextInserting || delta > 0))
	  {
	     pBo1 = pBox;
	     FrameUpdating = TRUE;

	     /* The concrete image is being filled */
	     y = height - pBo1->BxYOrg;

	     if (pFrame->FrAbstractBox->AbInLine)
		FrameUpdating = FALSE;

	     /* It lacks a pice of the concrete image at the frame top */
	     else if (pFrame->FrAbstractBox->AbTruncatedHead && y < 0)
	       {
		  /* Fill on top, so shift the whole concrete image */
		  height = h / 2 - y;

		  /* Height to add */
		  height = height * l;

		  /* Volume of the area to recompute */
		  if (min != NULL)
		    {
		       pBo2 = min;
		       y = pBo2->BxYOrg;	/* previous location */
		       x = y + pBo2->BxHeight;
		    }
		  IncreaseVolume (TRUE, VolumCar (height), frame);
		  toadd = TRUE;

		  /* Adding abstract boxes at the beginning */
		  /* Recompute the loaction of the frame in the abstract image */
		  if (min != NULL)
		    {
		       pBo2 = min;
		       y = -y + pBo2->BxYOrg;

		       /* y equal the shift of previous first box */
		       /* What's already displayed is related to this */
		       /* previous first box location */
		       pFrame->FrYOrg += y;

		       /* x equal the limit of redrawing after shifting */
		       if (y > 0)
			  x = pBo2->BxYOrg + pBo2->BxHeight;

		       /* new limit */
		       pFrame->FrClipYEnd = x;
		    }
		  ontop = RedrawFrameTop (frame, 0);
	       }
	     y = pFrame->FrYOrg + h - pBo1->BxYOrg - pBo1->BxHeight;
	     x = h / 2;

	     /* Abstract image overflow of half a frame both on top and bottom */
	     if (vol > 0 && -y > x)
		/* compute the volume to substract */
	       {
		  pBox = pBo1->BxPrevious;
		  bottom += x;
		  y = 0;
		  while (pBox != NULL)
		    {
		       pBo2 = pBox;
		       if (pBo2->BxYOrg < bottom)
			  pBox = NULL;
		       else
			 {
			    pBox = pBo2->BxPrevious;
			    if (pBo2->BxType != BoPiece && pBo2->BxType != BoDotted)
			       y += pBo2->BxAbstractBox->AbVolume;
			    else if (pBo2->BxAbstractBox->AbBox->BxNexChild == pBox)
			       y += pBo2->BxAbstractBox->AbVolume;
			 }
		    }
		  pFrame->FrVolume = pFrame->FrAbstractBox->AbVolume - vol - y;
	       }

	     /* It still lack a some abtract image area at the bottom of the frame */
	     else if (pFrame->FrAbstractBox->AbTruncatedTail && y > 0)
		/* free abstract boxes on top of the frame */
	       {
		  if (vol > 0 && vol < pFrame->FrAbstractBox->AbVolume)
		    {
		       if (min != NULL)
			  y = min->BxYOrg;
		       DecreaseVolume (TRUE, vol, frame);
		       DefClip (frame, 0, 0, 0, 0);

		       /* check location of frame in concrete image */
		       if (min != NULL)
			  pFrame->FrYOrg = pFrame->FrYOrg - y + min->BxYOrg;
		    }
		  if (pFrame->FrAbstractBox == NULL)
		    {
		       printf ("ERR: No more abstract box in %d\n", frame);
		       vol = -pFrame->FrVolume;
		    }
		  else
		     vol = pFrame->FrVolume - pFrame->FrAbstractBox->AbVolume;

		  /* Volume to add */
		  if (vol <= 0)
		    {
		       /* volume of the area to add */
		       vol = (y + x) * l;
		       /* convert in number of chars */
		       vol = VolumCar (vol);
		    }

		  y = pBo1->BxYOrg + pBo1->BxHeight;

		  /* cleanup the bottom of the frame */
		  Clear (frame, l, pFrame->FrYOrg + h - y, 0, y);

		  /* don't loop is volume didn't change */
		  y = pFrame->FrAbstractBox->AbVolume;
		  IncreaseVolume (FALSE, vol, frame);
		  y -= pFrame->FrAbstractBox->AbVolume;

		  /* Image should be complete */
		  FrameUpdating = FALSE;

		  if (y == 0)
		     printf ("ERR: Nothing to add\n");
		  else
		     /* Maybe image is not complete yet */
		     ontop = RedrawFrameBottom (frame, 0);
	       }

	     /* Le volume est satisfaisant */
	     else
	       {
		  /* Is a cleanup of the bottom of frame needed ? */
		  if (y > 0)
		     Clear (frame, l, y, 0, h - y);
		  pFrame->FrVolume = pFrame->FrAbstractBox->AbVolume;
	       }

	     /* update of image is finished */
	     FrameUpdating = FALSE;
	  }


     }
   else if (pFrame->FrReady)
     {
	/* Nothing to draw */
	DefClip (frame, 0, 0, 0, 0);
	ResetClip (frame);
     }
   FirstCreation = FALSE;

#ifdef NEW_WILLOWS
   WIN_ReleaseDeviceContext ();
#endif /* NEW_WILLOWS */

   return toadd;
}				/* end of function RedrawFrameBottom */

/** ----------------------------------------------------------------------
 *      DisplayFrame display one view of the document in frame.
 *		If a part of the abstract image is selected, the
 *		corresponding concrete image is centered in the frame.
 *  ---------------------------------------------------------------------- **/

#ifdef __STDC__
void                DisplayFrame (int frame)
#else  /* __STDC__ */
void                DisplayFrame (frame)
int                 frame;
#endif /* __STDC__ */
{
   ViewFrame            *pFrame;
   int                 l, h;

   /* Check that the frame exists */
   pFrame = &ViewFrameTable[frame - 1];
   if (pFrame->FrAbstractBox != NULL)
     {
	/* Drawing of the updated area */
	RedrawFrameBottom (frame, 0);

	/* recompute scrolls */
	MajScrolls (frame);

     }
   else
     {
	/* clean the frame */
	DimFenetre (frame, &l, &h);
	Clear (frame, l, h, 0, 0);
     }
}
