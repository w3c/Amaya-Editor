/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 
/*
 * frame.c : incremental display in frames.
 *
 * Authors: I. Vatton (INRIA)
 *          R. Guetari (W3C/INRIA) - Unicode, Windows version and Plug-ins
 *
 */

#include "ustring.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "picture.h"
#include "libmsg.h"
#include "message.h"
#include "appdialogue.h"
#ifdef _WINDOWS
#include "wininclude.h"
#endif /* _WINDOWS */

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "boxes_tv.h"
#include "edit_tv.h"
#include "platform_tv.h"
#include "frame_tv.h"
#include "picture_tv.h"

#include "absboxes_f.h"
#include "appli_f.h"
#include "boxlocate_f.h"
#include "boxmoves_f.h"
#include "buildboxes_f.h"
#include "displaybox_f.h"
#include "displayselect_f.h"
#include "font_f.h"
#include "frame_f.h"
#include "picture_f.h"
#include "windowdisplay_f.h"

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
   RedrawFilledBoxes redraw from top to bottom all filled boxes.
   Clipping is done by xmin, xmax, ymin, ymax.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         RedrawFilledBoxes (int frame, int xmin, int xmax, int ymin, int ymax)
#else  /* __STDC__ */
static void         RedrawFilledBoxes (frame, xmin, xmax, ymin, ymax)
int                 frame;
int                 xmin;
int                 xmax;
int                 ymin;
int                 ymax;
#endif /* __STDC__ */
{
  PtrAbstractBox      pAb, pAbChild;
  PtrBox              pBox, pBoxChild;
  ViewFrame          *pFrame;
  PictInfo           *imageDesc;
  int                 x, y;
  int                 xd, yd;
  int                 width, height;
  
  pFrame = &ViewFrameTable[frame - 1];
  pAb = pFrame->FrAbstractBox;
  x = pFrame->FrXOrg;
  y = pFrame->FrYOrg;
  pBox = pAb->AbBox;
  if (pBox == NULL)
    return;

   imageDesc = (PictInfo *) pAb->AbPictBackground;
   if (imageDesc &&
       (!pAb->AbTruncatedHead ||
	(imageDesc->PicPresent != XRepeat &&
	 imageDesc->PicPresent != RealSize)))
     DrawPicture (pBox, imageDesc, frame);
   else if (pAb->AbFillBox)
     {
       /* paint the whole window */
       GetSizesFrame (frame, &xd, &yd);
       /* todo: clip when backgroud will be printed */
       DrawRectangle (frame, pBox->BxThickness, pAb->AbLineStyle,
                    x, y, xd, yd, 0, 0, pAb->AbForeground,
                    pAb->AbBackground, pAb->AbFillPattern);
     }

   while (pBox->BxNextBackground != NULL)
     {
       pBox = pBox->BxNextBackground;
       pAb = pBox->BxAbstractBox;
       if (pBox->BxType == BoGhost)
	 {
	   /* see child elements */
	   pAbChild = pAb->AbFirstEnclosed;
	   while (pAbChild != NULL && !pAbChild->AbDead && pAbChild->AbBox->BxType == BoGhost)
	     pAbChild = pAbChild->AbFirstEnclosed;
	   if (pAbChild != NULL && !pAbChild->AbDead && pAbChild->AbBox != NULL)
	     {
	       pBoxChild = pAbChild->AbBox;
	       /* if the box is split take child boxes */
	       if (pBoxChild->BxType == BoSplit)
		 pBoxChild = pBoxChild->BxNexChild;
	       do
		 {
		   xd = pBoxChild->BxXOrg;
		   yd = pBoxChild->BxYOrg;
		   width = pBoxChild->BxWidth;
		   height = pBoxChild->BxHeight;
		   if (Printing)
		     {
		       /* clipping sur l'origine */
		       if (xd < x)
			 {
			   width = width - x + xd;
			   xd = x;
			 }
		       if (yd < y)
			 {
			   height = height - y + yd;
			   yd = y;
			 }
		       /* limite la largeur a la valeur du clipping */
		       if (xd + width > xmax)
			 width = xmax - xd;
		       /* limite la hauteur a la valeur du clipping */
		       if (yd + height > ymax)
			 height = ymax - yd;
		     }
		   if (yd + height >= ymin
		       && yd <= ymax
		       && xd + width >= xmin
		       && xd <= xmax)
		     {
		       imageDesc = (PictInfo *) pAb->AbPictBackground;
		       if (pAb->AbPictBackground &&
			   (!pAb->AbTruncatedHead ||
			    (imageDesc->PicPresent != XRepeat &&
			     imageDesc->PicPresent != RealSize)))
			 DrawPicture (pBoxChild, (PictInfo *) pAb->AbPictBackground, frame);
		       else
			 DrawRectangle (frame, pBox->BxThickness, pAb->AbLineStyle,
					xd - x, yd - y,
					width, height, 0, 0, pAb->AbForeground,
					pAb->AbBackground, pAb->AbFillPattern);
		     }
		   pBoxChild = pBoxChild->BxNext;
		 }
	       while (pBoxChild != NULL && IsParentBox (pBox, pBoxChild));
	     }
	 }
       else
	 {
	   xd = pBox->BxXOrg;
	   yd = pBox->BxYOrg;
	   width = pBox->BxWidth;
	   height = pBox->BxHeight;
	   if (Printing)
	     {
	       /* clipping sur l'origine */
	       if (xd < x)
		 {
		   width = width - x + xd;
		   xd = x;
		 }
	       if (yd < y)
		 {
		   height = height - y + yd;
		   yd = y;
		 }
	       /* limite la largeur a la valeur du clipping */
	       if (xd + width > xmax)
		 width = xmax - xd;
	       /* limite la hauteur a la valeur du clipping */
	       if (yd + height > ymax)
		 height = ymax - yd;
	     }
	   if (yd + height >= ymin
	       && yd <= ymax
	       && xd + width >= xmin
	       && xd <= xmax)
	     if (pAb->AbPictBackground)
	       DrawPicture (pBox, (PictInfo *) pAb->AbPictBackground, frame);
	     else
	       DrawRectangle (frame, pBox->BxThickness, pAb->AbLineStyle,
			      xd - x, yd - y,
			      width, height, 0, 0, pAb->AbForeground,
			      pAb->AbBackground, pAb->AbFillPattern);
	 }
     }
}


/*----------------------------------------------------------------------
   RedrawFrameTop redraw from bottom to top a frame.
   The scroll parameter indicates the height of a scroll
   back which may take place before recomputing the abstract
   image.
   The area is cleaned before redrawing.
   The origin coordinates of the abstract boxes are expected
   to be already computed.
   Return non zero if new abstract boxes were added in order
   to build the corresponding abstract image.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool            RedrawFrameTop (int frame, int scroll)
#else  /* __STDC__ */
ThotBool            RedrawFrameTop (frame, scroll)
int                 frame;
int                 scroll;
#endif /* __STDC__ */
{
   PtrBox              pBox;
   PtrBox              pTopBox, pBottomBox;
   PtrBox              pFirstBox;
   PtrBox              pRootBox;
   ViewFrame          *pFrame;
   int                 y, x, vol, h, l;
   int                 height, bottom;
   int                 framexmin;
   int                 framexmax;
   int                 frameymin;
   int                 frameymax;
   int                 plane, delta;
   int                 nextplane;
   ThotBool            isbelow;
   ThotBool            toadd;

   /* are new abstract boxes needed */
   toadd = FALSE;

   pFrame = &ViewFrameTable[frame - 1];
   GetSizesFrame (frame, &l, &h);
   if (!pFrame->FrReady || pFrame->FrAbstractBox == NULL)
     return toadd;
   else if (pFrame->FrClipXBegin < pFrame->FrClipXEnd
	    && pFrame->FrClipYBegin < pFrame->FrClipYEnd
	    && pFrame->FrXOrg < pFrame->FrClipXEnd
	    && pFrame->FrYOrg - scroll < pFrame->FrClipYEnd
	    && pFrame->FrXOrg + l > pFrame->FrClipXBegin
	    && pFrame->FrYOrg - scroll + h > pFrame->FrClipYBegin)
     {
	pFrame->FrYOrg -= scroll;
	framexmin = pFrame->FrClipXBegin;
	framexmax = pFrame->FrClipXEnd;
	frameymin = pFrame->FrClipYBegin;
	frameymax = pFrame->FrClipYEnd;
	DefineClipping (frame, pFrame->FrXOrg, pFrame->FrYOrg, &framexmin, &frameymin, &framexmax, &frameymax, 1);
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
	     pTopBox = NULL;
	     pBottomBox = NULL;
	     vol = 0;
	     /* half a frame under the bottom of the window */
	     delta = bottom + h / 2;
	     while (isbelow)
	       {
		  if (pBox->BxYOrg < bottom)
		     isbelow = FALSE;
		  else
		    {
		       if (pBox->BxYOrg > delta)
			  if (pBox->BxType != BoPiece && pBox->BxType != BoDotted)
			     /* this is not a breaking box */
			     vol += pBox->BxAbstractBox->AbVolume;
			  else if (pBox->BxAbstractBox->AbBox->BxNexChild == pBox)
			     /* this is the first breaking box */
			     vol += pBox->BxAbstractBox->AbVolume;
		       if (pBox->BxPrevious == NULL)
			  isbelow = FALSE;
                       else
			 {
			   y = pBox->BxYOrg + pBox->BxHeight;
			   /* add the filling of the end of the block */
			   x = pBox->BxXOrg + pBox->BxWidth + pBox->BxEndOfBloc;
			   if ((pBox->BxType == BoPicture) &&
			       !((y >= pFrame->FrYOrg)     &&
				 (pBox->BxYOrg <= (pFrame->FrYOrg + h)) &&
				 (x >= pFrame->FrXOrg)                  &&
				 (pBox->BxXOrg <= (pFrame->FrXOrg + l))))
			     UnmapImage ((PictInfo *)pBox->BxPictInfo);
			 }
		       if (pBox->BxPrevious == pBox)
			 /* detect cycles */
			 pBox = NULL;
		       else
			 pBox = pBox->BxPrevious;
		    }
	       }

	     /* Redraw from top to bottom all filled boxes */
	     RedrawFilledBoxes (frame, framexmin, framexmax, frameymin, frameymax);
	     /* Display planes in reverse order from biggest to lowest */
	     plane = 65536;
	     nextplane = plane - 1;
	     pFirstBox = pBox;
	     while (plane != nextplane)
		/* there is a new plane to display */
	       {
		  plane = nextplane;
		  pBox = pFirstBox;
		  /* browse all the boxes */

		  /* Draw all the boxes not yet displayed */
		  while (pBox != NULL)
		    {
		       if (pBox->BxAbstractBox->AbDepth == plane)
			  /* The box is drawn in the current plane */
			 {
			    y = pBox->BxYOrg + pBox->BxHeight;
			    /* add the filling of the end of the block */
			    x = pBox->BxXOrg + pBox->BxWidth + pBox->BxEndOfBloc;

			    /* Save the box on top or the first not visible */
			    if (y > height && pBox->BxYOrg < bottom)
			      {
				 if (pBottomBox == NULL)
				    pBottomBox = pBox;
				 pTopBox = pBox;
			      }

			    if (y >= frameymin
				&& pBox->BxYOrg <= frameymax
				&& x >= framexmin
				&& pBox->BxXOrg <= framexmax)
			      DisplayBox (pBox, frame);
                            else if (pBox->BxType == BoPicture)
			      {
				if (!((y >= pFrame->FrYOrg) &&
				      (pBox->BxYOrg <= (pFrame->FrYOrg + h)) &&
				      (x >= pFrame->FrXOrg) &&
				      (pBox->BxXOrg <= (pFrame->FrXOrg + l))))
				  UnmapImage ((PictInfo *)pBox->BxPictInfo);
				else if (((PictInfo *)pBox->BxPictInfo)->PicType >= PLUGIN_FORMAT)
				  /* redisplay plugins */
				  DisplayBox (pBox, frame);
			      }

			    /* Skip to next box */
			    if (pBox->BxPrevious == pBox)
			      /* detect cycles */
			      pBox = NULL;
			    else
			      pBox = pBox->BxPrevious;
			 }
		       else if (pBox->BxAbstractBox->AbDepth < plane)
			 {
			    /* keep the lowest value for plane depth */
			    if (plane == nextplane)
			       nextplane = pBox->BxAbstractBox->AbDepth;
			    else if (pBox->BxAbstractBox->AbDepth > nextplane)
			       nextplane = pBox->BxAbstractBox->AbDepth;
			    if (pBox->BxPrevious == pBox)
			      /* detect cycles */
			      pBox = NULL;
			    else
			      pBox = pBox->BxPrevious;
			 }
		       else if (pBox->BxPrevious == pBox)
			 /* detect cycles */
			 pBox = NULL;
		       else
			  pBox = pBox->BxPrevious;
		    }
	       }

	     /* The updated area is redrawn */
	     DefClip (frame, 0, 0, 0, 0);
	     RemoveClipping (frame);

	     /* If needed complete the partial existing image */
	     pRootBox = pFrame->FrAbstractBox->AbBox;
	     if (!FrameUpdating && !TextInserting)
	       {
		  /* The concrete image is being filled */
		  FrameUpdating = TRUE;

		  /* Abstract image overflow of half a frame both on top and bottom */
		  y = height - pRootBox->BxYOrg;
		  delta = h / 2;

		  if (vol > 0 && y > delta)
		     /* Compute the volume to remove */
		    {
		       pBox = pRootBox->BxNext;
		       height -= delta;
		       y = 0;
		       while (pBox != NULL)
			 {
			    pBottomBox = pBox;
			    if (pBottomBox->BxYOrg + pBottomBox->BxHeight > height)
			       pBox = NULL;
			    else
			      {
				 pBox = pBox->BxNext;
				 if (pBottomBox->BxType != BoPiece && pBottomBox->BxType != BoDotted)
				    y += pBottomBox->BxAbstractBox->AbVolume;
				 else if (pBottomBox->BxNexChild == NULL)
				    y += pBottomBox->BxAbstractBox->AbVolume;
			      }
			 }
		       pFrame->FrVolume = pFrame->FrAbstractBox->AbVolume - vol - y;
		    }

		  /* It lacks some abtract image at the top of the frame */
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
		       delta -= y;
		       if (vol <= 0)
			 {
			    /* volume of the area to add */
			    vol = delta * l;
			    /* convert in number of chars */
			    vol = GetCharsCapacity (vol);
			 }
		       if (pTopBox != NULL)
			 {
			    y = pTopBox->BxYOrg;

			    /* previous frame limit */
			    delta = y + pTopBox->BxHeight;
			 }
		       IncreaseVolume (TRUE, vol, frame);

		       /* Need to add abstract boxes */
		       toadd = TRUE;

		       /* Recompute the loaction of the frame in the abstract image */
		       if (pTopBox != NULL)
			 {
			    y = -y + pTopBox->BxYOrg;

			    /* y equal the shift of previous first box */
			    /* What's already displayed is related to this */
			    /* previous first box location */
			    pFrame->FrYOrg += y;

			    /* delta equal the limit of redrawing after shifting */
			    if (y > 0)
			       delta = pTopBox->BxYOrg + pTopBox->BxHeight;

			    /* new limit */
			    pFrame->FrClipYEnd = delta;
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
		  else if (pFrame->FrAbstractBox->AbTruncatedTail &&
			   (bottom > pRootBox->BxYOrg + pRootBox->BxHeight ||
			    (!pFrame->FrAbstractBox->AbHeight.DimIsPosition &&
			     pFrame->FrAbstractBox->AbHeight.DimMinimum &&
			     !pRootBox->BxContentHeight)))
		    {
		       y = pRootBox->BxYOrg + pRootBox->BxHeight;

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
   else
     {
	/* Nothing to draw */
	DefClip (frame, 0, 0, 0, 0);
	RemoveClipping (frame);
     }
   return toadd;
}


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
   PtrAbstractBox      pAb;
   int                 i;

   /* Look for and ancestor abstract boxe with PavGraphCreation flag set */
   pAb = pBox->BxAbstractBox->AbEnclosing;
   i = 0;
   while (i == 0)
      if (pAb == NULL)
	 i = 1;
      else if (pAb->AbBox->BxHorizFlex
	       || pAb->AbBox->BxVertFlex)
	{
	   i = 1;
	   pAb = NULL;
	}
      else if ((pAb->AbHorizPos.PosUserSpecified)
	       || (pAb->AbVertPos.PosUserSpecified)
	       || (pAb->AbWidth.DimUserSpecified)
	       || (pAb->AbHeight.DimUserSpecified))
	 i = 1;
      else
	 pAb = pAb->AbEnclosing;

   /* There is an englobing abstract box */
   if (pAb != NULL)
      AddBoxToCreate (tocreate, pAb->AbBox, frame);

   /* Add this box to create, if there is no englobing box */
   /* already registered and if this box is visible.       */
   if (*tocreate == NULL
       && pBox->BxAbstractBox->AbVisibility >= ViewFrameTable[frame - 1].FrVisibility)
      *tocreate = pBox;
}


/*----------------------------------------------------------------------
   RedrawFrameBottom redraw from top to bottom a frame.
   The scroll parameter indicates the height of a scroll
   which may take place before recomputing the abstract
   image.
   The area is cleaned before redrawing.
   The origin coordinates of the abstract boxes are expected
   to be already computed.
   Return non zero if new abstract boxes were added in order
   to build the corresponding abstract image.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool            RedrawFrameBottom (int frame, int scroll)
#else  /* __STDC__ */
ThotBool            RedrawFrameBottom (frame, scroll)
int                 frame;
int                 scroll;
#endif /* __STDC__ */
{
   PtrBox              pBox;
   PtrBox              pTopBox;
   PtrBox              pRootBox;
   PtrBox              ToCreate;
   PtrBox              pFirstBox;
   ViewFrame          *pFrame;
   PtrAbstractBox      pAb;
   int                 plane;
   int                 nextplane;
   int                 i, delta;
   int                 y, x, vol, h, l;
   int                 height, bottom;
   int                 framexmin;
   int                 framexmax;
   int                 frameymin;
   int                 frameymax;
   ThotBool            ontop;
   ThotBool            toadd;

#  ifdef _WINDOWS
   WIN_GetDeviceContext (frame);
#  endif /* _WINDOWS */

   /* are new abstract boxes needed */
   toadd = FALSE;
   /* used to store boxes created on the fly */
   ToCreate = NULL;
   pFrame = &ViewFrameTable[frame - 1];
   GetSizesFrame (frame, &l, &h);
   if (!pFrame->FrReady || pFrame->FrAbstractBox == NULL)
     return toadd;
   else if (pFrame->FrClipXBegin < pFrame->FrClipXEnd
	    && pFrame->FrClipYBegin < pFrame->FrClipYEnd
	    && pFrame->FrXOrg < pFrame->FrClipXEnd
	    && pFrame->FrYOrg + scroll < pFrame->FrClipYEnd
	    && pFrame->FrXOrg + l > pFrame->FrClipXBegin
	    && pFrame->FrYOrg + scroll + h > pFrame->FrClipYBegin)
     {
        pFrame->FrYOrg += scroll;
	framexmin = pFrame->FrClipXBegin;
	framexmax = pFrame->FrClipXEnd;
	frameymin = pFrame->FrClipYBegin;
	frameymax = pFrame->FrClipYEnd;

	DefineClipping (frame, pFrame->FrXOrg, pFrame->FrYOrg, &framexmin, &frameymin, &framexmax, &frameymax, 1);
	height = pFrame->FrYOrg;
	bottom = height + h;

	/* Search the first visible box or the one below */
	if (pFrame->FrAbstractBox->AbBox == NULL)
	  pBox = NULL;
	else
	  {
	    pBox = pFrame->FrAbstractBox->AbBox->BxNext;
	    if (pBox == NULL)
	      /* empty document */
	      pBox = pFrame->FrAbstractBox->AbBox;
	  }

	ontop = TRUE;
	pTopBox = NULL;
	vol = 0;
	delta = height - h / 2;

	/* Redraw from top to bottom all filled boxes */
	RedrawFilledBoxes (frame, framexmin, framexmax, frameymin, frameymax);
	/* Display planes in reverse order from biggest to lowest */
	plane = 65536;
	nextplane = plane - 1;
	pFirstBox = pBox;
	while (plane != nextplane)
	   /* there is a new plane to display */
	  {
	     plane = nextplane;
	     pBox = pFirstBox;
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
			    if (pTopBox == NULL)
			       pTopBox = pBox;
			 }

		       /* If a box is drawn for the first time, check if   */
		       /* it (or one of it's ancestors) has to be created  */
		       /* dynamically */
		       if (pBox->BxNew)
			 {
			    /* this is no a new box */
			    pBox->BxNew = 0;
			    pAb = pBox->BxAbstractBox;
			    i = 0;
			    while (i == 0)
			       if (pAb == NULL)
				  i = 1;
			       else if (pAb->AbWidth.DimIsPosition
					|| pAb->AbHeight.DimIsPosition)
				 {
				    i = 1;
				    pAb = NULL;
				 }
			       else if ((pAb->AbHorizPos.PosUserSpecified)
				    || (pAb->AbVertPos.PosUserSpecified)
				      || (pAb->AbWidth.DimUserSpecified)
				    || (pAb->AbHeight.DimUserSpecified))
				  i = 1;
			       else
				  pAb = pAb->AbEnclosing;
			 }
		       else
			  pAb = NULL;

                       /** skip box to create dynamically **/
		       if (pAb != NULL)
			  /* store the box to create */
			  AddBoxToCreate (&ToCreate, pAb->AbBox, frame);
		       else  if (y >= frameymin            && 
				 pBox->BxYOrg <= frameymax && 
				 x >= framexmin            && 
				 pBox->BxXOrg <= framexmax)
			 DisplayBox (pBox, frame);
		       else if (pBox->BxType == BoPicture)
			 {
			   if (!((y >= pFrame->FrYOrg) &&
				 (pBox->BxYOrg <= (pFrame->FrYOrg + h)) &&
				 (x >= pFrame->FrXOrg) &&
				 (pBox->BxXOrg <= (pFrame->FrXOrg + l))))
			     UnmapImage ((PictInfo *)pBox->BxPictInfo);
			   else if (((PictInfo *)pBox->BxPictInfo)->PicType >= PLUGIN_FORMAT)
			     /* redisplay plugins */
			     DisplayBox (pBox, frame);
			 }
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

	/* Interactive creation of boxes missing */
	if (ToCreate != NULL)
	  {
	     DirectCreation (ToCreate, frame);
	     /* Should the son's boxes being handled too ? */
	     pAb = ToCreate->BxAbstractBox;
	     if (pAb != NULL)
		pAb = pAb->AbFirstEnclosed;
	     while (pAb != NULL)
	       {
		  if ((pAb->AbHorizPos.PosUserSpecified)
		      || (pAb->AbVertPos.PosUserSpecified)
		      || (pAb->AbWidth.DimUserSpecified)
		      || (pAb->AbHeight.DimUserSpecified))
		    {
		       ToCreate = pAb->AbBox;
		       DirectCreation (ToCreate, frame);
		    }
		  else
		     pAb = pAb->AbNext;
	       }
	     return (FALSE);
	  }

	/* The updated area is redrawn */
	DefClip (frame, 0, 0, 0, 0);
	RemoveClipping (frame);

	/* If needed complete the partial existing image */
	pRootBox = pFrame->FrAbstractBox->AbBox;
	if (!FrameUpdating && (!TextInserting || scroll > 0))
	  {
	     FrameUpdating = TRUE;

	     /* The concrete image is being filled */
	     y = height - pRootBox->BxYOrg;

	     if (pFrame->FrAbstractBox->AbInLine)
		FrameUpdating = FALSE;

	     /* It lacks a piece of the concrete image at the frame top */
	     else if (pFrame->FrAbstractBox->AbTruncatedHead && y < 0)
	       {
		  /* Fill on top, so shift the whole concrete image */
		  height = h / 2 - y;

		  /* Height to add */
		  height = height * l;

		  /* Volume of the area to recompute */
		  if (pTopBox != NULL)
		    {
		      /* register previous location */
		       y = pTopBox->BxYOrg;
		       delta = y + pTopBox->BxHeight;
		    }
		  IncreaseVolume (TRUE, GetCharsCapacity (height), frame);
		  toadd = TRUE;

		  /* Adding abstract boxes at the beginning */
		  /* Recompute the loaction of the frame in the abstract image */
		  if (pTopBox != NULL)
		    {
		       y = -y + pTopBox->BxYOrg;

		       /* y equal the shift of previous first box */
		       /* What's already displayed is related to this */
		       /* previous first box location */
		       pFrame->FrYOrg += y;

		       /* delta equal the limit of redrawing after shifting */
		       if (y > 0)
			  delta = pTopBox->BxYOrg + pTopBox->BxHeight;

		       /* new limit */
		       pFrame->FrClipYEnd = delta;
		    }
		  ontop = RedrawFrameTop (frame, 0);
	       }
	     y = pFrame->FrYOrg + h - pRootBox->BxYOrg - pRootBox->BxHeight;
	     delta = h / 2;

	     /* Abstract image overflow of half a frame both on top and bottom */
	     if (vol > 0 && -y > delta)
		/* compute the volume to substract */
	       {
		  pBox = pRootBox->BxPrevious;
		  bottom += delta;
		  y = 0;
		  while (pBox != NULL)
		    {
		       pTopBox = pBox;
		       if (pTopBox->BxYOrg < bottom)
			  pBox = NULL;
		       else
			 {
			    pBox = pBox->BxPrevious;
			    if (pTopBox->BxType != BoPiece && pTopBox->BxType != BoDotted)
			       y += pTopBox->BxAbstractBox->AbVolume;
			    else if (pTopBox->BxAbstractBox->AbBox->BxNexChild == pBox)
			       y += pTopBox->BxAbstractBox->AbVolume;
			 }
		    }
		  pFrame->FrVolume = pFrame->FrAbstractBox->AbVolume - vol - y;
	       }

	     /* It lacks some abtract image at the bottom of the frame */
	     else if (pFrame->FrAbstractBox->AbTruncatedTail &&
		      (y > 0 ||
		       (!pFrame->FrAbstractBox->AbHeight.DimIsPosition &&
			pFrame->FrAbstractBox->AbHeight.DimMinimum &&
			!pRootBox->BxContentHeight)))
		/* free abstract boxes on top of the frame */
	       {
		  if (vol > 0 && vol < pFrame->FrAbstractBox->AbVolume)
		    {
		       if (pTopBox != NULL)
			  y = pTopBox->BxYOrg;
		       DecreaseVolume (TRUE, vol, frame);
		       DefClip (frame, 0, 0, 0, 0);

		       /* check location of frame in concrete image */
		       if (pTopBox != NULL)
			  pFrame->FrYOrg = pFrame->FrYOrg - y + pTopBox->BxYOrg;
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
		       vol = (y + delta) * l;
		       /* convert in number of chars */
		       vol = GetCharsCapacity (vol);
		    }

		  y = pRootBox->BxYOrg + pRootBox->BxHeight;

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
   else
     {
	/* Nothing to draw */
	DefClip (frame, 0, 0, 0, 0);
	RemoveClipping (frame);
     }

   FirstCreation = FALSE;

#  ifdef _WINDOWS
   WIN_ReleaseDeviceContext ();
#  endif /* _WINDOWS */
   return toadd;
}


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
      CheckScrollingWidth (frame);
      UpdateScrollbars (frame);
    }
   else
     {
       /* clean the frame */
       GetSizesFrame (frame, &w, &h);
       Clear (frame, w, h, 0, 0);
     }
}
