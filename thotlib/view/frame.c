/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2001
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
#include "exceptions_f.h"
#include "font_f.h"
#include "frame_f.h"
#include "picture_f.h"
#include "xwindowdisplay_f.h"

/*----------------------------------------------------------------------
   GetXYOrg : do a coordinate shift related to current frame.
  ----------------------------------------------------------------------*/
void GetXYOrg (int frame, int *XOrg, int *YOrg)
{
   ViewFrame          *pFrame;

   pFrame = &ViewFrameTable[frame - 1];
   *XOrg = pFrame->FrXOrg;
   *YOrg = pFrame->FrYOrg;
}

/*----------------------------------------------------------------------
   DefClip defines the area of the frame which need to be redrawn.
  ----------------------------------------------------------------------*/
void DefClip (int frame, int xd, int yd, int xf, int yf)
{
   int                 width, height;
   ViewFrame          *pFrame;

   if ((xd == xf && xd == 0 && (yd != yf || yd != 0)) ||
       (yd == yf && yd == 0 && (xd != xf || xd != 0)))
     return;
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
	else if (xd == xf && xd == 0 && yd == yf && yd == 0)
	  {
	     pFrame->FrClipXBegin = 0;
	     pFrame->FrClipXEnd = 0;
	  }
	/* Should we initalize the area redrawn */
	else if (pFrame->FrClipXBegin == pFrame->FrClipXEnd &&
		 pFrame->FrClipXBegin == 0)
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
	else if (xd == xf && xd == 0 && yd == yf && yd == 0)
	  {
	     pFrame->FrClipYBegin = 0;
	     pFrame->FrClipYEnd = 0;
	  }
	/* Should we initalize the area redrawn */
	else if (pFrame->FrClipYBegin == pFrame->FrClipYEnd &&
		 pFrame->FrClipYBegin == 0)
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
}


/*----------------------------------------------------------------------
   DefRegion store the area of frame which need to be redrawn.
  ----------------------------------------------------------------------*/
void DefRegion (int frame, int xd, int yd, int xf, int yf)
{
  ViewFrame          *pFrame;

  pFrame = &ViewFrameTable[frame - 1];
  DefClip (frame, xd + pFrame->FrXOrg, yd + pFrame->FrYOrg,
	  xf + pFrame->FrXOrg, yf + pFrame->FrYOrg);
}

/*----------------------------------------------------------------------
   TtaRefresh redraw all the frame of all the loaded documents.
  ----------------------------------------------------------------------*/
void TtaRefresh ()
{
   int                 frame;

   for (frame = 1; frame <= MAX_FRAME; frame++)
     {
	if (ViewFrameTable[frame - 1].FrAbstractBox != NULL)
	  {
	     /* enforce redrawing of the whole frame */
	     DefClip (frame, -1, -1, -1, -1);
	     (void) RedrawFrameBottom (frame, 0, NULL);
	  }
     }
}


/*----------------------------------------------------------------------
   AddBoxToCreate store in adbloc the list of child boxes to be created,
   from the most englobing box down to pBox itself.
   It ensure unicity of boxes referenced in adbloc.
  ----------------------------------------------------------------------*/
static void    AddBoxToCreate (PtrBox * tocreate, PtrBox pBox, int frame)
{
  PtrAbstractBox      pAb;
  int                 i;

  /* Look for and ancestor abstract boxe with PavGraphCreation flag set */
  pAb = pBox->BxAbstractBox->AbEnclosing;
  i = 0;
  while (i == 0)
    {
      if (pAb == NULL)
	i = 1;
      else if (pAb->AbBox->BxHorizFlex || pAb->AbBox->BxVertFlex)
	{
	  i = 1;
	  if (pAb->AbLeafType != LtGraphics || pAb->AbShape != 'g')
	    pAb = NULL;
	}
      else if (pAb->AbHorizPos.PosUserSpecified ||
	       pAb->AbVertPos.PosUserSpecified ||
	       pAb->AbWidth.DimUserSpecified ||
	       pAb->AbHeight.DimUserSpecified)
      i = 1;
      else
	pAb = pAb->AbEnclosing;
    }

   /* There is an englobing abstract box */
   if (pAb != NULL)
      AddBoxToCreate (tocreate, pAb->AbBox, frame);
   /* Add this box to create, if there is no englobing box */
   /* already registered and if this box is visible.       */
   if (*tocreate == NULL &&
       pBox->BxAbstractBox->AbVisibility >= ViewFrameTable[frame - 1].FrVisibility)
      *tocreate = pBox;
}


/*----------------------------------------------------------------------
  DrawFilledBox draws a box with background or borders.
   Clipping is done by xmin, xmax, ymin, ymax.
  ----------------------------------------------------------------------*/
static void DrawFilledBox (PtrAbstractBox pAb, int frame, int xmin,
						   int xmax, int ymin, int ymax)
{
  PtrBox              pBox;
  ViewFrame          *pFrame;
  PictInfo           *imageDesc;
  PictureScaling      picPresent;
  int                 x, y;
  int                 xd, yd;
  int                 width, height;
  int                 w, h;
 
  pBox = pAb->AbBox;
  if (pBox->BxType == BoGhost)
    return;
  pFrame = &ViewFrameTable[frame - 1];
  x = pFrame->FrXOrg;
  y = pFrame->FrYOrg;
  GetSizesFrame (frame, &w, &h);
  if (pBox == NULL)
    return;
  if (pAb == pFrame->FrAbstractBox)
    {
      /* get the maximum of the window size and the root box size */
      if (pBox->BxWidth > w)
	w = pBox->BxWidth;
      if (pBox->BxHeight > h)
	h = pBox->BxHeight;
      width = w + 1;
      height = h + 1;
      if (pBox->BxFill
#ifndef _WINDOWS
	  && Printing
#endif /* _WINDOWS */
	  ) /****/
	{
	  /* draw the box background */
	  xd = xmin - pFrame->FrXOrg;
	  yd = ymin - pFrame->FrYOrg;
	  DrawRectangle (frame, 0, 0, xd, yd, width, height,
			 pAb->AbForeground, pAb->AbBackground,
			 pAb->AbFillPattern);
	}
      xd = pBox->BxXOrg;
      yd = pBox->BxYOrg;
      /* fill the whole window surface */
      width -= xd;
      height -= yd;
    }
  else
    {
      xd = pBox->BxXOrg + pBox->BxLMargin;
      yd = pBox->BxYOrg + pBox->BxTMargin;
      /* GetSizesFrame (frame, &width, &height); */
      width = pBox->BxWidth - pBox->BxLMargin - pBox->BxRMargin;
      height = pBox->BxHeight - pBox->BxTMargin - pBox->BxBMargin;
    }

      /* clipping on the origin */
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
      /* clipping on the width */
      if (xd + width > xmax)
        width = xmax - xd + 1;
      /* clipping on the height */
      if (yd + height > ymax)
        height = ymax - yd + 1; 
      if (yd + height >= ymin && yd <= ymax &&
	  (pAb == pFrame->FrAbstractBox ||
	   (xd + width >= xmin && xd <= xmax)))
	{
	  DisplayBorders (pBox, frame, xd - x, yd - y, width, height);
	  /* draw over the padding */
	  if (pAb == pFrame->FrAbstractBox)
	    {
	      xd = pBox->BxXOrg;
	      yd = pBox->BxYOrg;
	      /* fill the whole window surface */
	      width = w + 1 - xd;
	      height = h + 1 - yd;
	    }
	  else
	    {
	      xd =  pBox->BxXOrg + pBox->BxLMargin + pBox->BxLBorder;
	      yd = pBox->BxYOrg + pBox->BxTMargin + pBox->BxTBorder;
	      width = pBox->BxW + pBox->BxLPadding + pBox->BxRPadding;
	      height = pBox->BxH + pBox->BxTPadding + pBox->BxBPadding;
	      /* clipping on the origin */
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
	      /* clipping on the width */
	      if (xd + width > xmax)
		width = xmax - xd + 1;
	      /* clipping on the height */
	      if (yd + height > ymax)
		height = ymax - yd + 1;
	    }
	  imageDesc = (PictInfo *) pAb->AbPictBackground;
	  if (pAb->AbSelected)
	    {
	      /* draw the box selection */
	      if (pAb->AbVolume == 0 && pBox->BxWidth <= 2)
		DrawRectangle (frame, 0, 0, xd - x, yd - y, width, height, 0, InsertColor, 2);
	      else
		DrawRectangle (frame, 0, 0, xd - x, yd - y, width, height, 0, SelColor, 2);
	    }
	  else
	    {
	      if (pAb != pFrame->FrAbstractBox && pBox->BxFill)
		/* draw the box background */
		DrawRectangle (frame, 0, 0, xd - x, yd - y, width, height,
			       pAb->AbForeground, pAb->AbBackground,
			       pAb->AbFillPattern);
	      if (imageDesc)
		{
		  /* draw the background image the default presentation is repeat */
		  picPresent = imageDesc->PicPresent;
		  if (picPresent == DefaultPres)
		    picPresent = FillFrame;
		  if (picPresent == YRepeat || picPresent == FillFrame ||
		      (picPresent == XRepeat && !pAb->AbTruncatedHead))
		    DrawPicture (pBox, imageDesc, frame,  xd - x, yd - y, width, height);
		  else if (!pAb->AbTruncatedHead)
		    /* the clipping will work automatically */
		    DrawPicture (pBox, imageDesc, frame,
				 pBox->BxXOrg + pBox->BxLMargin + pBox->BxLBorder + pBox->BxLPadding - x,
				 pBox->BxYOrg + pBox->BxTMargin + pBox->BxTBorder + pBox->BxTPadding + FrameTable[frame].FrTopMargin - y,
				 pBox->BxW, pBox->BxH);
		}
	    }
	}
}


/*----------------------------------------------------------------------
  DisplayAllBoxes crosses the Abstract tree from top to bottom
  and left to rigth to display all visible boxes.
  Parameters xmin, xmax, ymin, ymax give the clipped area.
  The parameter create returns the box that must be interactively
  created by the user.
  tVol and bVol return the volume of not displayed boxes on thetop and
  on the bottom of the window.
  ----------------------------------------------------------------------*/
PtrBox DisplayAllBoxes (int frame, int xmin, int xmax, int ymin, int ymax,
			PtrBox *create, int *tVol, int *bVol)
{
  PtrAbstractBox      pAb, specAb;
  PtrBox              pBox, box;
  PtrBox              topBox;
  ViewFrame          *pFrame;
  PictInfo           *imageDesc;
  int                 plane;
  int                 nextplane;
  int                 winTop, winBottom;
  int                 bt, bb;
  int                 l, h;
  ThotBool            userSpec;

  pFrame = &ViewFrameTable[frame - 1];
  pAb = pFrame->FrAbstractBox;
  GetSizesFrame (frame, &l, &h);
  winTop = pFrame->FrYOrg;
  winBottom = winTop + h;
  pBox = pAb->AbBox;
  *tVol = *bVol = 0;
  if (pBox == NULL)
    return NULL;
  /* Display planes in reverse order from biggest to lowest */
  plane = 65536;
  nextplane = plane - 1;
  topBox = NULL;
  pAb = pFrame->FrAbstractBox;
  if (pBox->BxDisplay || pAb->AbSelected)
    DrawFilledBox (pAb, frame, xmin, xmax, ymin, ymax);
  while (plane != nextplane)
    /* there is a new plane to display */
    {
      plane = nextplane;
      /* Draw all the boxes not yet displayed */
      pAb = pFrame->FrAbstractBox;
      while (pAb)
	{
	  if (pAb->AbDepth == plane &&
	      pAb != pFrame->FrAbstractBox &&
	      pAb->AbBox)
	    {
	      /* box in the current plane */
	      pBox = pAb->AbBox;
	      if (pAb->AbLeafType == LtCompound)
		{
		  if (pAb->AbVisibility >= pFrame->FrVisibility &&
		      (pBox->BxDisplay || pAb->AbSelected))
		    DrawFilledBox (pAb, frame, xmin, xmax, ymin, ymax);
		}
	      else
		{
		  /* look for the box displayed at the top of the window */
		  if (pBox->BxType == BoSplit)
		    {
		      /* the box itself doen't give right positions */
		      box = pBox->BxNexChild;
		      bt = box->BxYOrg;
		      /* don't take into account the last empty box */
		      while (box->BxNexChild &&
			     (box->BxNexChild->BxNChars > 0 ||
			      box->BxNexChild->BxNexChild))
			box = box->BxNexChild;
		      bb = box->BxYOrg + box->BxHeight;
		    }
		  else
		    {
		      bt = pBox->BxYOrg;
		      bb = pBox->BxYOrg + pBox->BxHeight;
		    }
		  if (bb < winTop)
		    /* the box is not visible */
		    *tVol = *tVol + pBox->BxAbstractBox->AbVolume;
		  else if (bt > winBottom)
		    /* the box is not visible */
		    *bVol = *bVol + pBox->BxAbstractBox->AbVolume;
		  else
		    {
		      if (pBox->BxType == BoSplit)
			/* the box itself doen't give right positions */
			box = pBox->BxNexChild;
		      else
			box = pBox;
		      if (topBox == NULL)
			topBox = box;
		      else if (box->BxYOrg < topBox->BxYOrg)
			topBox = box;
		      userSpec = FALSE;
		      if (pBox->BxNew)
			{
			  /* this is a new box */
			  pBox->BxNew = 0;
			  specAb = pAb;
			  while (!userSpec && specAb)
			    {
			      if (specAb->AbWidth.DimIsPosition ||
				  specAb->AbHeight.DimIsPosition)
				specAb = NULL;
			      else if (specAb->AbHorizPos.PosUserSpecified ||
				       specAb->AbVertPos.PosUserSpecified ||
				       specAb->AbWidth.DimUserSpecified ||
				       specAb->AbHeight.DimUserSpecified)
				{
				  /* one paramater is given by the user */
				  AddBoxToCreate (create, specAb->AbBox, frame);
				  userSpec = TRUE;
				}
			      else
				specAb = specAb->AbEnclosing;
			    }
			}
		      if (!userSpec)
			{
			  if (pBox->BxType == BoSplit)
			    while (pBox->BxNexChild)
			      {
				pBox = pBox->BxNexChild;
				if (pBox->BxYOrg + pBox->BxHeight >= ymin  &&
				    pBox->BxYOrg <= ymax && 
				    pBox->BxXOrg + pBox->BxWidth + pBox->BxEndOfBloc >= xmin &&
				    pBox->BxXOrg <= xmax)
				  DisplayBox (pBox, frame, xmin, xmax, ymin, ymax);
			      }
			  else if (bb >= ymin  &&
				   bt <= ymax && 
				   pBox->BxXOrg + pBox->BxWidth + pBox->BxEndOfBloc >= xmin &&
				   pBox->BxXOrg <= xmax)
			    DisplayBox (pBox, frame, xmin, xmax, ymin, ymax);
			  else if (pBox->BxType == BoPicture)
			    {
			      imageDesc = (PictInfo *)pBox->BxPictInfo;
			      if (bb < winTop ||
				  bt > winBottom ||
				  pBox->BxXOrg + pBox->BxWidth + pBox->BxEndOfBloc < pFrame->FrXOrg ||
				  pBox->BxXOrg > pFrame->FrXOrg + l)
				UnmapImage (imageDesc);
			      else if (imageDesc->PicType >= PLUGIN_FORMAT)
				/* redisplay plugins */
				DisplayBox (pBox, frame, xmin, xmax, ymin, ymax);
			    }
			}
		    }
		}
	    }
	  else if (pAb->AbDepth < plane)
	    {
	      /* keep the lowest value for plane depth */
	      if (plane == nextplane)
		nextplane = pAb->AbDepth;
	      else if (pAb->AbDepth > nextplane)
		nextplane = pAb->AbDepth;
	    }
	  /* get next abstract box */
	  if (pAb->AbLeafType == LtCompound && pAb->AbFirstEnclosed)
	    /* get the first child */
	    pAb = pAb->AbFirstEnclosed;
	  else if (pAb->AbNext)
	    /* get the next sibling */
	    pAb = pAb->AbNext;
	  else
	    {
	      /* go up in the tree */
	      while (pAb->AbEnclosing && pAb->AbEnclosing->AbNext == NULL)
		pAb = pAb->AbEnclosing;
	      pAb = pAb->AbEnclosing;
	      if (pAb)
		pAb = pAb->AbNext;
	    }
	}
    }
  return topBox;
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
ThotBool            RedrawFrameTop (int frame, int scroll)
{
  PtrBox              topBox;
  PtrBox              create;
  PtrBox              pRootBox;
  ViewFrame          *pFrame;
  int                 y, tVol, bVol, h, l;
  int                 top, bottom;
  int                 xmin;
  int                 xmax;
  int                 ymin;
  int                 ymax;
  int                 delta;
  ThotBool            toadd;

  /* are new abstract boxes needed */
  toadd = FALSE;
  pFrame = &ViewFrameTable[frame - 1];
  GetSizesFrame (frame, &l, &h);
  xmin = pFrame->FrClipXBegin;
  xmax = pFrame->FrClipXEnd;
  ymin = pFrame->FrClipYBegin;
  ymax = pFrame->FrClipYEnd;
  if (!pFrame->FrReady || pFrame->FrAbstractBox == NULL)
    return toadd;
  else if (xmin < xmax && ymin < ymax &&
	   pFrame->FrXOrg < xmax &&
	   pFrame->FrYOrg - scroll < ymax &&
	   pFrame->FrXOrg + l > xmin &&
	   pFrame->FrYOrg - scroll + h > ymin)
    {
      pFrame->FrYOrg -= scroll;
      top = pFrame->FrYOrg;
      bottom = top + h;
      /* used to store boxes created on the fly */
      create = NULL;
      tVol = bVol = 0;
#if defined(_WINDOWS) && !defined(_WIN_PRINT)
  WIN_GetDeviceContext (frame);
#endif /* __WINDOWS && !_WINT_PRINT */
      DefineClipping (frame, pFrame->FrXOrg, pFrame->FrYOrg,
		      &xmin, &ymin, &xmax, &ymax, 1);

      /* Is there a need to redisplay part of the frame ? */
      if (xmin < xmax && ymin < ymax)
	topBox = DisplayAllBoxes (frame, xmin, xmax, ymin, ymax, &create, &tVol, &bVol);
	  
      /* The updated area is redrawn */
      DefClip (frame, 0, 0, 0, 0);
      RemoveClipping (frame);
#if defined(_WINDOWS) && !defined(_WIN_PRINT)
  WIN_ReleaseDeviceContext ();
#endif /*  _WINDOWS && !WIN_PRINT */

      /* if needed complete the partial existing image */
      pRootBox = pFrame->FrAbstractBox->AbBox;
      if (!FrameUpdating && !TextInserting)
	{
	  /* The concrete image is being updated */
	  FrameUpdating = TRUE;
	  y = top - pRootBox->BxYOrg;
	  if (pFrame->FrAbstractBox->AbTruncatedHead && y < 0)
	    {
	      /* it lacks some abstract image at the top of the frame */
	      if (bVol > 0 && bVol < pFrame->FrAbstractBox->AbVolume)
		{
		  /* free abstract boxes at the bottom */
		  DecreaseVolume (FALSE, bVol, frame);
		  DefClip (frame, 0, 0, 0, 0);
		}
	      if (pFrame->FrAbstractBox == NULL)
		{
		  printf ("ERR: No more abstract boxes in %d\n", frame);
		  bVol = -pFrame->FrVolume;
		}
	      else
		bVol = pFrame->FrVolume - pFrame->FrAbstractBox->AbVolume;
	      
	      /* Volume to add */
	      top = (h / 2 - y) * l;
	      delta = 0;
	      /* Volume of the area to recompute */
	      if (topBox)
		{
		  /* register previous location */
		  y = topBox->BxYOrg;
		  delta = y + topBox->BxHeight;
		}
	      /* Adding abstract boxes at the beginning */
	      IncreaseVolume (TRUE, GetCharsCapacity (top), frame);
	      toadd = TRUE;
	      /* Recompute the loaction of the frame in the abstract image */
	      if (topBox)
		{
		  y = -y + topBox->BxYOrg;
		  /* y equal the shift of previous first box */
		  /* What's already displayed is related to this */
		  /* previous first box location */
		  pFrame->FrYOrg += y;
		  /* delta equal the limit of redrawing after shifting */
		  if (y > 0)
		    delta = topBox->BxYOrg + topBox->BxHeight;
		  /* new limit */
		  pFrame->FrClipYEnd = delta;
		}
	      else
		/* No previous box. The frame is drawn */
		/* on top of the concrete image */
		pFrame->FrYOrg = 0;
	      /* Image should be complete */
	      FrameUpdating = FALSE;
	      RedrawFrameTop (frame, 0);
	    }

	  y = bottom - pRootBox->BxYOrg - pRootBox->BxHeight;
	  if (pFrame->FrAbstractBox->AbTruncatedTail &&
	      (y >= 0/* || (!pFrame->FrAbstractBox->AbHeight.DimIsPosition &&
			 pFrame->FrAbstractBox->AbHeight.DimMinimum &&
			 !pRootBox->BxContentHeight)*/))
	    {
	      /* it lacks some abstract image at the bottom of the frame */
	      /* cleanup the bottom of the frame */
	      Clear (frame, l, y, 0, pRootBox->BxYOrg + pRootBox->BxHeight);
	      /* don't loop is volume didn't change */
	      tVol = pFrame->FrAbstractBox->AbVolume;
	      /* Volume to add */
	      bottom = (y + h / 2) * l;
	      IncreaseVolume (FALSE, GetCharsCapacity (bottom), frame);
	      tVol -= pFrame->FrAbstractBox->AbVolume;
	      /* Image should be completed */
	      FrameUpdating = FALSE;
	      if (tVol == 0)
		printf ("ERR: Nothing to add\n");
	      else
		/* Maybe image is not complete yet */
		RedrawFrameBottom (frame, 0, NULL);
	    }
	  else
	    /* Volume computed is sufficient */
	    pFrame->FrVolume = pFrame->FrAbstractBox->AbVolume;
	  
	  /* update of image is finished */
	  FrameUpdating = FALSE;
	}
    }
  else
    /* The modified area is not visible */
    DefClip (frame, 0, 0, 0, 0);
   return toadd;
}


/*----------------------------------------------------------------------
   RedrawFrameBottom redraw from top to bottom a frame.
   The scroll parameter indicates the height of a scroll
   which may take place before recomputing the abstract
   image.
   The area is cleaned before redrawing.
   The origin coordinates of the abstract boxes are expected
   to be already computed.
   The parameter subtree gives the root of the redisplayed subtree. When
   the subtree is NULL the whole tree is taken into account.
   Return non zero if new abstract boxes were added in order
   to build the corresponding abstract image.
  ----------------------------------------------------------------------*/
ThotBool     RedrawFrameBottom (int frame, int scroll, PtrAbstractBox subtree)
{
  PtrBox              topBox;
  PtrBox              pRootBox;
  PtrBox              create;
  ViewFrame          *pFrame;
  PtrAbstractBox      pAb;
  int                 delta;
  int                 y, tVol, bVol, h, l;
  int                 top, bottom;
  int                 xmin;
  int                 xmax;
  int                 ymin;
  int                 ymax;
  ThotBool            stop;
  ThotBool            toadd;

  /* are new abstract boxes needed */
  toadd = FALSE;
  pFrame = &ViewFrameTable[frame - 1];
  GetSizesFrame (frame, &l, &h);
  xmin = pFrame->FrClipXBegin;
  xmax = pFrame->FrClipXEnd;
  ymin = pFrame->FrClipYBegin;
  ymax = pFrame->FrClipYEnd;
  if (!pFrame->FrReady || pFrame->FrAbstractBox == NULL)
    return toadd;
  else if (xmin < xmax &&
	   ymin < ymax &&
	   pFrame->FrXOrg < xmax &&
	   pFrame->FrYOrg + scroll < ymax &&
	   pFrame->FrXOrg + l > xmin &&
	   pFrame->FrYOrg + scroll + h > ymin)
    {
      pFrame->FrYOrg += scroll;
      top = pFrame->FrYOrg;
      bottom = top + h;
      tVol = bVol = 0;
      delta = top - h / 2;
      topBox = NULL;
      /* used to store boxes created on the fly */
      create = NULL;
      /* Redraw from top to bottom all filled boxes */
#if defined(_WINDOWS) && !defined(_WIN_PRINT)
  WIN_GetDeviceContext (frame);
#endif /* __WINDOWS && !_WINT_PRINT */
      DefineClipping (frame, pFrame->FrXOrg, pFrame->FrYOrg,
		      &xmin, &ymin, &xmax, &ymax, 1);
      /* Is there a need to redisplay part of the frame ? */
      if (xmin < xmax && ymin < ymax)
	{
	  topBox = DisplayAllBoxes (frame, xmin, xmax, ymin, ymax, &create, &tVol, &bVol);
	  
	  /* Interactive creation of boxes */
	  if (create)
	    {
	      pAb = create->BxAbstractBox;
	      DirectCreation (create, frame);
	      /* Should son's boxes being handled too ? */
	      while (pAb && pAb->AbFirstEnclosed)
		{
		  pAb = pAb->AbFirstEnclosed;
		  stop = FALSE;
		  while (!stop && pAb)
		    {
		      if (pAb->AbHorizPos.PosUserSpecified ||
			  pAb->AbVertPos.PosUserSpecified ||
			  pAb->AbWidth.DimUserSpecified ||
			  pAb->AbHeight.DimUserSpecified)
			DirectCreation (pAb->AbBox, frame);
		      if (pAb->AbNext)
			pAb = pAb->AbNext;
		      else
			stop = TRUE;
		    }
		}
	      return (FALSE);
	    }
	}
      /* The updated area is redrawn */
      DefClip (frame, 0, 0, 0, 0);
      RemoveClipping (frame);
#if defined(_WINDOWS) && !defined(_WIN_PRINT)
  WIN_ReleaseDeviceContext ();
#endif /* __WINDOWS && !_WINT_PRINT */

      /* if needed complete the partial existing image */
      pRootBox = pFrame->FrAbstractBox->AbBox;
      if (!FrameUpdating && (!TextInserting || scroll > 0))
	{
	  /* The concrete image is being updated */
	  FrameUpdating = TRUE;
	  y = top - pRootBox->BxYOrg;
	  if (pFrame->FrAbstractBox->AbInLine)
	    FrameUpdating = FALSE;
	  else if (pFrame->FrAbstractBox->AbTruncatedHead && y < 0)
	    {
	      /* it lacks a piece of the concrete image at the frame top */
	      /* filling on top will shift the whole concrete image */
	      top = h / 2 - y;
	      /* Height to add */
	      top = top * l;
	      /* Volume of the area to recompute */
	      if (topBox != NULL)
		{
		  /* register previous location */
		  y = topBox->BxYOrg;
		  delta = y + topBox->BxHeight;
		}
	      /* Adding abstract boxes at the beginning */
	      IncreaseVolume (TRUE, GetCharsCapacity (top), frame);
	      toadd = TRUE;
	      /* Recompute the location of the frame in the abstract image */
	      if (topBox != NULL)
		{
		  y = -y + topBox->BxYOrg;
		  /* y equal the shift of previous first box */
		  /* What's already displayed is related to this */
		  /* previous first box location */
		  pFrame->FrYOrg += y;
		  /* delta equal the limit of redrawing after shifting */
		  if (y > 0)
		    delta = topBox->BxYOrg + topBox->BxHeight;
		  /* new limit */
		  pFrame->FrClipYEnd = delta;
		}
	      RedrawFrameTop (frame, 0);
	    }
	  y = bottom - pRootBox->BxYOrg - pRootBox->BxHeight;
	  if (pFrame->FrAbstractBox->AbTruncatedTail &&
		   (y >= 0/* || (!pFrame->FrAbstractBox->AbHeight.DimIsPosition &&
			      pFrame->FrAbstractBox->AbHeight.DimMinimum &&
			      !pRootBox->BxContentHeight)*/))
	    {
	      /* it lacks some abstract image at the bottom of the frame */
	      if (tVol > 0 && tVol < pFrame->FrAbstractBox->AbVolume)
		{
		  /* free abstract boxes on top of the frame */
		  if (topBox)
		    y = topBox->BxYOrg;
		  DecreaseVolume (TRUE, tVol, frame);
		  DefClip (frame, 0, 0, 0, 0);
		  /* check location of frame in concrete image */
		  if (topBox)
		    pFrame->FrYOrg = pFrame->FrYOrg - y + topBox->BxYOrg;
		}
	      if (pFrame->FrAbstractBox == NULL)
		{
		  printf ("ERR: No more abstract box in %d\n", frame);
		  tVol = -pFrame->FrVolume;
		}
	      else
		tVol = pFrame->FrVolume - pFrame->FrAbstractBox->AbVolume;
	      
	      /* cleanup the bottom of the frame */
	      Clear (frame, l, y, 0, pRootBox->BxYOrg + pRootBox->BxHeight);
	      /* don't loop is volume didn't change */
	      tVol = pFrame->FrAbstractBox->AbVolume;
	      /* Volume to add */
	      bottom = (y + h / 2) * l;
	      IncreaseVolume (FALSE, GetCharsCapacity (bottom), frame);
	      tVol -= pFrame->FrAbstractBox->AbVolume;
	      /* Image should be completed */
	      FrameUpdating = FALSE;
	      if (tVol == 0)
		printf ("ERR: Nothing to add\n");
	      else
		/* Maybe image is not complete yet */
		RedrawFrameBottom (frame, 0, NULL);
	    }
	  else
	    {
	      /* Volume computed is sufficient */
	      /* Is a cleanup of the bottom of frame needed ? */
	      if (y > 0)
		Clear (frame, l, y, 0, pRootBox->BxYOrg + pRootBox->BxHeight);
	      pFrame->FrVolume = pFrame->FrAbstractBox->AbVolume;
	    }

	  /* update of image is finished */
	  FrameUpdating = FALSE;
	}
    }
  else
    {
      /* Nothing to draw */
#if defined(_WINDOWS) && !defined(_WIN_PRINT)
      WIN_GetDeviceContext (frame);
#endif /* __WINDOWS && !_WINT_PRINT */
      DefClip (frame, 0, 0, 0, 0);
      RemoveClipping (frame);
#if defined(_WINDOWS) && !defined(_WIN_PRINT)
      WIN_ReleaseDeviceContext ();
#endif /* __WINDOWS && !_WINT_PRINT */
    }
  FirstCreation = FALSE;
  return toadd;
}


/*----------------------------------------------------------------------
   DisplayFrame display one view of the document in frame.
   If a part of the abstract image is selected, the
   corresponding concrete image is centered in the frame.
  ----------------------------------------------------------------------*/
void                DisplayFrame (int frame)
{
  ViewFrame          *pFrame;
  int                 w, h;

  if (documentDisplayMode[frame - 1] == DisplayImmediately)
    {
      /* Check that the frame exists */
      pFrame = &ViewFrameTable[frame - 1];
      if (pFrame->FrAbstractBox != NULL)
	{
	  /* Drawing of the updated area */
	  RedrawFrameBottom (frame, 0, NULL);
	  
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
}
