/*
 * Copyright (c) 1996 INRIA, All rights reserved
 */

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

/*----------------------------------------------------------------------
   GetXYOrg : do a coordinate shift related to current frame.
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                GetXYOrg (int frame, int *XOrg, int *YOrg)

#else  /* __STDC__ */
void                GetXYOrg (frame, XOrg, YOrg)
int                 frame;
int                *XOrg;
int                *YOrg;

#endif /* __STDC__ */

{
   ViewFrame          *pFrame;

   pFrame = &ViewFrameTable[frame - 1];
   *XOrg = pFrame->FrXOrg;
   *YOrg = pFrame->FrYOrg;
}

/*----------------------------------------------------------------------
   DefClip defines the area of the frame which need to be redrawn.
  ----------------------------------------------------------------------*/

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
   ViewFrame          *pFrame;

   if (frame > 0 && frame <= MAX_FRAME)
     {
	pFrame = &ViewFrameTable[frame - 1];
	/* Should we take the whole width of the frame ? */
	if (xd == xf && xd == -1)
	  {
	     GetSizesFrame (frame, &width, &height);
	     pFrame->FrClipXBegin = pFrame->FrXOrg;
	     pFrame->FrClipXEnd = width + pFrame->FrXOrg;
	  }
	/* finish the redrawing */
	else if (xd == xf && xd == 0)
	  {
	     pFrame->FrClipXBegin = 0;
	     pFrame->FrClipXEnd = 0;
	  }
	/* Should we initalize the area redrawn */
	else if (pFrame->FrClipXBegin == pFrame->FrClipXEnd && pFrame->FrClipXBegin == 0)
	  {
	     pFrame->FrClipXBegin = xd;
	     pFrame->FrClipXEnd = xf;
	  }
	/* Update the coordinates of the area redrawn */
	else
	  {
	     if (pFrame->FrClipXBegin > xd)
		pFrame->FrClipXBegin = xd;
	     if (pFrame->FrClipXEnd < xf)
		pFrame->FrClipXEnd = xf;
	  }
	/* Should we take the whole height of the frame ? */
	if (yd == yf && yd == -1)
	  {
	     GetSizesFrame (frame, &width, &height);
	     pFrame->FrClipYBegin = pFrame->FrYOrg;
	     pFrame->FrClipYEnd = height + pFrame->FrYOrg;
	     /* On termine un reaffichage */
	  }
	else if (yd == yf && yd == 0)
	  {
	     pFrame->FrClipYBegin = 0;
	     pFrame->FrClipYEnd = 0;
	  }
	/* Should we initalize the area redrawn */
	else if (pFrame->FrClipYBegin == pFrame->FrClipYEnd && pFrame->FrClipYBegin == 0)
	  {
	     pFrame->FrClipYBegin = yd;
	     pFrame->FrClipYEnd = yf;
	  }
	/* Update the coordinates of the area redrawn */
	else
	  {
	     if (pFrame->FrClipYBegin > yd)
		pFrame->FrClipYBegin = yd;
	     if (pFrame->FrClipYEnd < yf)
		pFrame->FrClipYEnd = yf;
	  }
     }
}				/*DefClip */


/*----------------------------------------------------------------------
   DefRegion store the area of frame which need to be redrawn.
  ----------------------------------------------------------------------*/
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
   ViewFrame          *pFrame;


   pFrame = &ViewFrameTable[frame - 1];
   DefClip (frame, xd + pFrame->FrXOrg, yd + pFrame->FrYOrg, xf + pFrame->FrXOrg, yf + pFrame->FrYOrg);
}

/*----------------------------------------------------------------------
   TtaRefresh redraw all the frame of all the loaded documents.
  ----------------------------------------------------------------------*/
void                TtaRefresh ()
{
   int                 frame;

   for (frame = 1; frame <= MAX_FRAME; frame++)
     {
	if (ViewFrameTable[frame - 1].FrAbstractBox != NULL)
	  {
	     /* enforce redrawing of the whole frame */
	     DefClip (frame, -1, -1, -1, -1);
	     (void) RedrawFrameBottom (frame, 0);
	  }
     }
}

/*----------------------------------------------------------------------
   RedrawFrameTop redraw from bottom to top a frame.
   The delta parameter indicates the height of a scroll
   back which may take place before recomputing the abstract
   image.
   The area is cleaned before redrawing.
   The origin coordinates of the abstract boxes are expected
   to be already computed.
   Return non zero if new abstract boxes were added in order
   to build the corresponding abstract image.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
boolean             RedrawFrameTop (int frame, int delta)

#else  /* __STDC__ */
boolean             RedrawFrameTop (frame, delta)
int                 frame;
int                 delta;

#endif /* __STDC__ */

{
   PtrBox              pBox;
   PtrBox              min, max;
   int                 y, x, vol, h, l;
   int                 height, bottom;
   int                 framexmin;
   int                 framexmax;
   int                 frameymin;
   int                 frameymax;
   boolean             isbelow;
   boolean             toadd;
   int                 plane;
   int                 nextplane;
   PtrBox              firstbox;
   ViewFrame          *pFrame;
   PtrBox              pBo1;
   PtrBox              pBo2;

   /* are new abstract boxes needed */
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
	DefineClipping (frame, pFrame->FrXOrg, pFrame->FrYOrg, &framexmin, &frameymin, &framexmax, &frameymax, 1);
	GetSizesFrame (frame, &l, &h);
	height = pFrame->FrYOrg;
	bottom = height + h;

	/* Is there a need to redisplay part of the frame ? */
	if (framexmin < framexmax && frameymin < frameymax)
	  {
	     /* Search the first visible box starting from the */
	     /* bottom, i.e. the last box drawn                */
	     pBox = pFrame->FrAbstractBox->AbBox->BxPrevious;
	     if (pBox == NULL)
		/* empty document */
		pBox = pFrame->FrAbstractBox->AbBox;
	     isbelow = TRUE;
	     min = NULL;
	     max = NULL;
	     vol = 0;
	     /* half a frame under the bottom of the window */
	     x = bottom + h / 2;
	     while (isbelow)
	       {
		  pBo1 = pBox;
		  if (pBo1->BxYOrg < bottom)
		     isbelow = FALSE;
		  else
		    {
		       if (pBo1->BxYOrg > x)
			  if (pBo1->BxType != BoPiece && pBo1->BxType != BoDotted)
			     /* this is not a breaking box */
			     vol += pBo1->BxAbstractBox->AbVolume;
			  else if (pBo1->BxAbstractBox->AbBox->BxNexChild == pBox)
			     /* this is the first breaking box */
			     vol += pBo1->BxAbstractBox->AbVolume;
		       if (pBo1->BxPrevious == NULL)
			  isbelow = FALSE;
		       pBox = pBo1->BxPrevious;
		    }
	       }

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
		       pBo1 = pBox;
		       if (pBo1->BxAbstractBox->AbDepth == plane)
			  /* The box is drawn in the current plane */
			 {
			    y = pBo1->BxYOrg + pBo1->BxHeight;

			    /* take into account the filling of the end of the block */
			    x = pBo1->BxXOrg + pBo1->BxWidth + pBo1->BxEndOfBloc;

			    /* Save the box on top or the first not visible */
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

			    /* Skip to next box */
			    pBox = pBo1->BxPrevious;
			 }
		       else if (pBo1->BxAbstractBox->AbDepth < plane)
			 {
			    /* keep the lowest value for plane depth */
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

	     /* The updated area is redrawn */
	     DefClip (frame, 0, 0, 0, 0);
	     RemoveClipping (frame);

	     /* If needed complete the partial existing image */
	     pBox = pFrame->FrAbstractBox->AbBox;
	     if (!FrameUpdating && !TextInserting)
	       {
		  pBo1 = pBox;

		  /* The concrete image is being filled */
		  FrameUpdating = TRUE;

		  /* Abstract image overflow of half a frame both on top and bottom */
		  y = height - pBo1->BxYOrg;
		  x = h / 2;

		  if (vol > 0 && y > x)
		     /* Compute the volume to remove */
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
			      }
			 }
		       pFrame->FrVolume = pFrame->FrAbstractBox->AbVolume - vol - y;
		    }

		  /* A piece of the abstract image need to be rebuild on top */
		  else if (pFrame->FrAbstractBox->AbTruncatedHead && y < 0)
		    {
		       /* Free abstract boxes at the bottom */
		       if (vol > 0 && vol < pFrame->FrAbstractBox->AbVolume)
			 {
			    DecreaseVolume (FALSE, vol, frame);
			    DefClip (frame, 0, 0, 0, 0);
			    /* Fill on top, so shift the whole concrete image */
			 }

		       /* Volume to add */
		       if (pFrame->FrAbstractBox == NULL)
			 {
			    printf ("ERR: No more abastract boxes in %d\n", frame);
			    vol = -pFrame->FrVolume;
			 }
		       else
			  vol = pFrame->FrVolume - pFrame->FrAbstractBox->AbVolume;

		       /* Height to add */
		       x -= y;
		       if (vol <= 0)
			 {
			    /* volume of the area to add */
			    vol = x * l;
			    /* convert in number of chars */
			    vol = GetCharsCapacity (vol);
			 }
		       if (min != NULL)
			 {
			    pBo2 = min;
			    y = pBo2->BxYOrg;

			    /* previous frame limit */
			    x = y + pBo2->BxHeight;
			 }
		       IncreaseVolume (TRUE, vol, frame);

		       /* Need to add abstract boxes */
		       toadd = TRUE;

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
		       else
			  /* No previous box. The frame is drawn */
			  /* on top of the concrete image */
			  pFrame->FrYOrg = 0;

		       /* Image should be complete */
		       FrameUpdating = FALSE;
		       isbelow = RedrawFrameTop (frame, 0);
		    }

		  /* A piece of the concrete image lack at the bottom */
		  else if (pFrame->FrAbstractBox->AbTruncatedTail && bottom > pBo1->BxYOrg + pBo1->BxHeight)
		    {
		       y = pBo1->BxYOrg + pBo1->BxHeight;

		       /* volume of the area to add */
		       vol = (bottom - y) * l;

		       /* convert in number of chars */
		       vol = GetCharsCapacity (vol);

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
			  isbelow = RedrawFrameBottom (frame, 0);
		    }

		  /* Volume computed is sufficient */
		  else
		     pFrame->FrVolume = pFrame->FrAbstractBox->AbVolume;

		  /* update of image is finished */
		  FrameUpdating = FALSE;
	       }
	  }
	else
	   /* The nodified area is not visible */
	   DefClip (frame, 0, 0, 0, 0);

     }
   else if (pFrame->FrReady)
     {
	/* Nothing to draw */
	DefClip (frame, 0, 0, 0, 0);
	RemoveClipping (frame);
     }

   return toadd;
}				/* function RedrawFrameTop */


/*----------------------------------------------------------------------
   AddBoxToCreate store in adbloc the list of child boxes to be created,
   from the most englobing box down to pBox itself.
   It ensure unicity of boxes referenced in adbloc.
  ----------------------------------------------------------------------*/


#ifdef __STDC__
static void         AddBoxToCreate (PtrBox * tocreate, PtrBox pBox, int frame)

#else  /* __STDC__ */
static void         AddBoxToCreate (tocreate, pBox, frame)
PtrBox             *tocreate;
PtrBox              pBox;
int                 frame;

#endif /* __STDC__ */

{
   PtrAbstractBox      pAbbox1;
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

/*----------------------------------------------------------------------
   RedrawFrameBottom redraw from top to bottom a frame.
   The delta parameter indicates the height of a scroll
   which may take place before recomputing the abstract
   image.
   The area is cleaned before redrawing.
   The origin coordinates of the abstract boxes are expected
   to be already computed.
   Return non zero if new abstract boxes were added in order
   to build the corresponding abstract image.
  ----------------------------------------------------------------------*/

#ifdef __STDC__
boolean             RedrawFrameBottom (int frame, int delta)

#else  /* __STDC__ */
boolean             RedrawFrameBottom (frame, delta)
int                 frame;
int                 delta;

#endif /* __STDC__ */

{
   PtrBox              pBox;
   PtrBox              min /*, max */ ;
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
   PtrBox              firstbox;
   ViewFrame          *pFrame;
   PtrBox              pBo1;
   PtrAbstractBox      pAbbox1;
   PtrBox              pBo2;
   PtrBox              ToCreate;
   int                 i;

   /* are new abstract boxes needed */
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
	DefineClipping (frame, pFrame->FrXOrg, pFrame->FrYOrg, &framexmin, &frameymin, &framexmax, &frameymax, 1);
	GetSizesFrame (frame, &l, &h);
	height = pFrame->FrYOrg;
	bottom = height + h;

	/* Search the first visible box or the one below */
	pBox = pFrame->FrAbstractBox->AbBox->BxNext;
	if (pBox == NULL)
	   pBox = pFrame->FrAbstractBox->AbBox;		/* empty document */

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

/** skip box to create dynamically **/
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
	     DirectCreation (ToCreate, frame);
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
		       DirectCreation (ToCreate, frame);
		    }
		  else
		     pAbbox1 = pAbbox1->AbNext;
	       }
	     return (FALSE);
	  }

	/* The updated area is redrawn */
	DefClip (frame, 0, 0, 0, 0);
	RemoveClipping (frame);

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
		  IncreaseVolume (TRUE, GetCharsCapacity (height), frame);
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
		       vol = GetCharsCapacity (vol);
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

	     /* Volume computed is sufficient */
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
	RemoveClipping (frame);
     }
   FirstCreation = FALSE;

#ifdef _WINDOWS
   WIN_ReleaseDeviceContext ();
#endif /* _WINDOWS */

   return toadd;
}				/* end of function RedrawFrameBottom */

/*----------------------------------------------------------------------
   DisplayFrame display one view of the document in frame.
   If a part of the abstract image is selected, the
   corresponding concrete image is centered in the frame.
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                DisplayFrame (int frame)
#else  /* __STDC__ */
void                DisplayFrame (frame)
int                 frame;

#endif /* __STDC__ */
{
   ViewFrame          *pFrame;
   int                 w, h;

   /* Check that the frame exists */
   pFrame = &ViewFrameTable[frame - 1];
   if (pFrame->FrAbstractBox != NULL)
     {
	/* Drawing of the updated area */
	RedrawFrameBottom (frame, 0);

	/* recompute scrolls */
	UpdateScrollbars (frame);

     }
   else
     {
	/* clean the frame */
	GetSizesFrame (frame, &w, &h);
	Clear (frame, w, h, 0, 0);
     }
}
