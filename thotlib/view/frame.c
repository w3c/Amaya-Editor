/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2003
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
#include "buildlines_f.h"
#include "memory_f.h"

#ifdef _GL
#include <GL/gl.h>
#include <GL/glu.h>
#include <math.h>
#include "glwindowdisplay.h"
#endif /*_GL*/


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
   ViewFrame          *pFrame;
   int                 width, height;
   int                 xb, xe, yb, ye;
   int                 scrollx, scrolly;

   if ((xd == xf && xd == 0 && (yd != yf || yd != 0)) ||
       (yd == yf && yd == 0 && (xd != xf || xd != 0)))
     return;
   else if (frame == 0 || frame > MAX_FRAME)
     return;
   else
     {
       /* exchange limits for rigth-to-left string */
       if (xf < xd)
	 {
	   xe = xd;
	   xd = xf;
	   xf = xe;
	 }
       if (yf < yd)
	 {
	   ye = yd;
	   yd = yf;
	   yf = ye;
	 }
     }
   pFrame = &ViewFrameTable[frame - 1];
   scrollx = pFrame->FrXOrg;	
   scrolly = pFrame->FrYOrg; 
   xb = pFrame->FrClipXBegin - scrollx;
   xe = pFrame->FrClipXEnd - scrollx - xb; 
   yb = pFrame->FrClipYBegin - scrolly;
   ye = pFrame->FrClipYEnd - scrolly - yb;	
   if (xd == xf && xd == -1)
     {
       /* repaint the whole frame */
       GetSizesFrame (frame, &width, &height);
       pFrame->FrClipXBegin = scrollx;
       pFrame->FrClipXEnd = width + scrollx;
       pFrame->FrClipYBegin = scrolly;
       pFrame->FrClipYEnd = height + scrolly;
     }
   else if (xd == xf && xd == 0 && yd == yf && yd == 0)
     {
       /* clean up */
       pFrame->FrClipXBegin = 0;
       pFrame->FrClipXEnd = 0;
       pFrame->FrClipYBegin = 0;
       pFrame->FrClipYEnd = 0;
     }
   else if (pFrame->FrClipXBegin == pFrame->FrClipXEnd &&
	    pFrame->FrClipXBegin == 0xd == xf &&
	    pFrame->FrClipYBegin == pFrame->FrClipYEnd &&
	    pFrame->FrClipYBegin == 0)
     {
       /* clean up */
       pFrame->FrClipXBegin = xd;
       pFrame->FrClipXEnd = xf;
       pFrame->FrClipYBegin = yd;
       pFrame->FrClipYEnd = yf;
     }
   else
     {
       /* Update the coordinates of the area redrawn */
       if (pFrame->FrClipXBegin > xd)
	 pFrame->FrClipXBegin = xd;
       if (pFrame->FrClipXEnd < xf)
	 pFrame->FrClipXEnd = xf;
       /* update the coordinates of the area redrawn */
       if (pFrame->FrClipYBegin > yd)
	 pFrame->FrClipYBegin = yd;
       if (pFrame->FrClipYEnd < yf)
	 pFrame->FrClipYEnd = yf;
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
static void AddBoxToCreate (PtrBox * tocreate, PtrBox pBox, int frame)
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
void DrawFilledBox (PtrAbstractBox pAb, int frame, int xmin,
			   int xmax, int ymin, int ymax)
{
  PtrBox              pBox;
  ViewFrame          *pFrame;
  PictInfo           *imageDesc;
  PictureScaling      pres;
  int                 x, y;
  int                 xd, yd;
  int                 width, height;
  int                 w, h;
  ThotBool            setWindow;

  pBox = pAb->AbBox;
  if (pBox->BxType == BoGhost || pBox->BxType == BoFloatGhost)
    return;
  pFrame = &ViewFrameTable[frame - 1];
  x = pFrame->FrXOrg;
  y = pFrame->FrYOrg;
  GetSizesFrame (frame, &w, &h);
  if (pBox == NULL)
    return;
  setWindow = (pAb == pFrame->FrAbstractBox);
  if (!setWindow &&
      TypeHasException (ExcSetWindowBackground, pAb->AbElement->ElTypeNumber,
			pAb->AbElement->ElStructSchema))
    /* repaint the whole window when the fill applies to the document */
    setWindow = (pAb->AbDocView == 1 &&
		 (pAb->AbEnclosing == NULL || /* document */
		  pAb->AbEnclosing->AbEnclosing == NULL || /* html */
		  pAb->AbEnclosing->AbEnclosing->AbEnclosing == NULL) /* body */);
  if (setWindow)
    {
      /* get the maximum of the window size and the root box size */
      if (pBox->BxWidth > w)
	w = pBox->BxWidth;
      if (pBox->BxHeight > h)
	h = pBox->BxHeight;
      width = w + 1;
      height = h + 1;
      xd = xmin;
      yd = ymin;
      if (pBox->BxFill)
	/* draw the box background */
	DrawRectangle (frame, 0, 0, xd - x, yd - y, width, height,
		       pAb->AbForeground, pAb->AbBackground,
		       pAb->AbFillPattern);
    }
  else
    {
      xd = pBox->BxXOrg + pBox->BxLMargin;
      yd = pBox->BxYOrg + pBox->BxTMargin;
      width = pBox->BxWidth - pBox->BxLMargin - pBox->BxRMargin;
      height = pBox->BxHeight - pBox->BxTMargin - pBox->BxBMargin;
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

  if (yd + height >= ymin && yd <= ymax &&
      (setWindow || (xd + width >= xmin && xd <= xmax)))
    {
      DisplayBorders (pBox, frame, xd - x, yd - y, width, height);
      /* draw over the padding */
      if (!setWindow || pAb->AbSelected)
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
	  if (!setWindow && pBox->BxFill && pAb->AbFillPattern)
	    /* draw the box background */
	    DrawRectangle (frame, 0, 0, xd - x, yd - y, width, height,
			   pAb->AbForeground, pAb->AbBackground,
			   pAb->AbFillPattern);
	  if (imageDesc)
	    {
	      /* draw the background image the default presentation is repeat */
	      pres = imageDesc->PicPresent;
	      if (pres == DefaultPres)
		pres = FillFrame;
	      if (pres == YRepeat || pres == FillFrame || !pAb->AbTruncatedHead)
		DrawPicture (pBox, imageDesc, frame,  xd - x, yd - y, width, height);
	    }
	}
    }
}

/*----------------------------------------------------------------------
 OpacityAndTransformNext : Test before going on to a next Box
  ----------------------------------------------------------------------*/
static void OpacityAndTransformNext (PtrAbstractBox pAb, int plane, int frame,
				     int xmin, int xmax, int ymin, int ymax,
				     ThotBool activate_opacity)
{
#ifdef _GL
  if (pAb->AbElement && pAb->AbDepth == plane)
    {
      if (TypeHasException (ExcIsGroup, pAb->AbElement->ElTypeNumber,
			    pAb->AbElement->ElStructSchema) && 
	  pAb->AbOpacity != 1000 && pAb->AbOpacity != 0 &&
	  activate_opacity &&
	  ((xmax - xmin) > 0) && 
	  ((ymax - ymin) > 0))
	{
	  if (!pAb->AbBox->VisibleModification && 
	      pAb->AbBox->Post_computed_Pic)
	    DisplayOpaqueGroup (pAb, frame, xmin, xmax, ymin, ymax, FALSE);
	  else
	    {
	      if (pAb->AbBox->Pre_computed_Pic)
		{	      
		  OpaqueGroupTexturize (pAb, frame, xmin, xmax, ymin, ymax, FALSE);
		  ClearOpaqueGroup (pAb, frame, xmin, xmax, ymin, ymax);
		  DisplayOpaqueGroup (pAb, frame, xmin, xmax, ymin, ymax, TRUE);
		  /* Unless we can know when 
		     a box gets its picture or 
		     when changes are efffectives*/
		  OpaqueGroupTextureFree (pAb, frame);	
		}
	    }
	}     
      if (IsTransformed (pAb->AbElement->ElTransform))
	DisplayTransformationExit ();
    }
#endif /* _GL */
}

#ifdef _GL
/*----------------------------------------------------------------------
  OriginSystemExit : pop from current matrix stack
  ----------------------------------------------------------------------*/
static void OriginSystemExit (PtrAbstractBox pAb, ViewFrame  *pFrame, 
			 int plane,  int OldXOrg, int OldYOrg,
			 int ClipXOfFirstCoordSys, int ClipYOfFirstCoordSys)
{
  PtrBox              pBox;

  pBox = pAb->AbBox;
  if (pAb->AbElement->ElSystemOrigin && 
      plane == pAb->AbDepth)
    {
      DisplayTransformationExit ();
      if (pBox && 
	  (OldXOrg != 0 || OldYOrg != 0) &&
	  (ClipXOfFirstCoordSys == pBox->BxClipX &&
	  ClipYOfFirstCoordSys == pBox->BxClipY))
	{
	  pFrame->FrXOrg = OldXOrg;
	  pFrame->FrYOrg = OldYOrg;
	  OldXOrg = 0;
	  OldYOrg = 0;
	}
      /*  GL_PopClip (); */
    }
}
#endif /* _GL */

/*--------------------------------------------------------
  GetBoxTransformedCoord : Transform windows coordinate x, y
  to the transformed system  of the seeked box
  --------------------------------------------------------*/
void GetBoxTransformedCoord  (PtrAbstractBox pAbSeeked, int frame,
			      int *lowerx, int *higherx, 
			      int *x, int *y)
{
#ifdef _GL
  PtrAbstractBox      pAb;
  PtrBox              pBox;
  ViewFrame          *pFrame;
  int                 plane;
  int                 nextplane;
  int                 l, h;
  ThotBool            FrameUpdatingStatus, FormattedFrame;
  int                 OldXOrg, OldYOrg, 
                      ClipXOfFirstCoordSys, ClipYOfFirstCoordSys;

  double              winx, winy, finalx, finaly, finalz;
  int                 viewport[4];
  double              projection_view[16];
  double              model_view[16];
  int                 base_y;
  ThotBool            is_transformed = FALSE;

  FrameUpdatingStatus = FrameUpdating;
  FrameUpdating = TRUE;  
  pFrame = &ViewFrameTable[frame - 1];
  pAb = pFrame->FrAbstractBox;
  GetSizesFrame (frame, &l, &h);
  pBox = pAb->AbBox;
  if (pBox == NULL)
    return;
  /* Display planes in reverse order from biggest to lowest */
  plane = 65536;
  nextplane = plane - 1;
  pAb = pFrame->FrAbstractBox;
  if (FrameTable[frame].FrView == 1)
    FormattedFrame = TRUE;
  else
    {
    FormattedFrame = FALSE;

      *lowerx += pFrame->FrXOrg;
      *higherx += pFrame->FrXOrg;
      *y += pFrame->FrYOrg;

      return;

    }
  OldXOrg = 0;
  OldYOrg = 0;
  ClipXOfFirstCoordSys = ClipYOfFirstCoordSys = 0;
  glGetDoublev (GL_MODELVIEW_MATRIX, model_view);
  glGetDoublev (GL_PROJECTION_MATRIX, projection_view);
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
	      if (pAb->AbElement && pAb->AbDepth == plane)
		{ 
		  if (FormattedFrame)
		    {
		      /* If the coord sys origin is translated, 
			 it must be before any other transfromation*/
		      if (pAb->AbElement->ElSystemOrigin)
			{
			  is_transformed = TRUE;
			  DisplayBoxTransformation (pAb->AbElement->ElTransform, 
						  pFrame->FrXOrg, pFrame->FrYOrg);
			}
		      /* Normal transformation*/
		      if (pAb->AbElement->ElTransform)
			{
			  is_transformed = TRUE;
			  DisplayTransformation (pAb->AbElement->ElTransform, 
						 pBox->BxWidth, 
						 pBox->BxHeight);
			}
		      if (pAb->AbElement->ElSystemOrigin)
			{ 
			  if (pFrame->FrXOrg ||pFrame->FrYOrg)
			    {
			      OldXOrg = pFrame->FrXOrg;
			      OldYOrg = pFrame->FrYOrg;
			      pFrame->FrXOrg = 0;
			      pFrame->FrYOrg = 0;
			      ClipXOfFirstCoordSys = pBox->BxClipX;
			      ClipYOfFirstCoordSys = pBox->BxClipY;
			    }
			}
		    }
		  if (pAb == pAbSeeked && pAb->AbLeafType != LtCompound &&
		      is_transformed) 
		    {  
		      glGetDoublev (GL_MODELVIEW_MATRIX, model_view);
		      glGetDoublev (GL_PROJECTION_MATRIX, projection_view);		      
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
	  if (pAb->AbLeafType == LtCompound && 
	      pAb->AbFirstEnclosed)
	    /* get the first child */
	    pAb = pAb->AbFirstEnclosed;
	  else if (pAb->AbNext)	    
	    {	 
	      if (FormattedFrame)
		{
		  OpacityAndTransformNext (pAb, plane, frame, 0, 0, 0, 0, FALSE);
		  OriginSystemExit (pAb, pFrame, plane, OldXOrg, OldYOrg, 
				    ClipXOfFirstCoordSys, ClipYOfFirstCoordSys);
		}
	      /* get the next sibling */
	      pAb = pAb->AbNext;
	    }
	  else
	    {
	      /* go up in the tree */
	      while (pAb->AbEnclosing && 
		     pAb->AbEnclosing->AbNext == NULL)
		{
		  if (FormattedFrame)
		    {
		      OpacityAndTransformNext (pAb, plane, frame, 0, 0, 0, 0, FALSE);		  	
		      OriginSystemExit (pAb, pFrame, plane, OldXOrg, OldYOrg, 
					ClipXOfFirstCoordSys, ClipYOfFirstCoordSys);
		    }
		  pAb = pAb->AbEnclosing;
		}
	      if (FormattedFrame)
		{	
		  OpacityAndTransformNext (pAb, plane, frame, 0, 0, 0, 0, FALSE);
		  OriginSystemExit (pAb, pFrame, plane, OldXOrg, OldYOrg, 
				    ClipXOfFirstCoordSys, ClipYOfFirstCoordSys);	  
		}
	      pAb = pAb->AbEnclosing;
	      if (pAb)
		{
		  if (FormattedFrame)
		    {
		      OpacityAndTransformNext (pAb, plane, frame, 0, 0, 0, 0, FALSE);
		      OriginSystemExit (pAb, pFrame, plane, OldXOrg, OldYOrg, 
					ClipXOfFirstCoordSys, ClipYOfFirstCoordSys);	  
		    }
		  pAb = pAb->AbNext;
		}
	    }
	}
    } 
  FrameUpdating = FrameUpdatingStatus;

  if (is_transformed)
    {
      viewport[0] = 0; viewport[1] = 0; viewport[2] = l; viewport[3] = h;
      winx = (double) *lowerx;
      winy = (double) (h - *y);
      
      if (GL_TRUE == gluUnProject (winx, winy, 0.0,
				   model_view,
				   projection_view,
				   viewport,
				   &finalx, &finaly, &finalz))
	{
	  base_y = FloatToInt ((float) finaly);
	  if (*lowerx != *higherx)
	    {
	      *lowerx = FloatToInt ((float) finalx);
	      winx = (double) *higherx;
	      winy = (double) (h - *y);
	      gluUnProject (winx, winy, 0.0,
			    model_view,
			    projection_view,
			    viewport,
			    &finalx, &finaly, &finalz);

	      *higherx = FloatToInt ((float) finalx);
	      base_y = FloatToInt ((float) finaly);
	    }
	  else
	    {
	      *higherx = *lowerx = FloatToInt ((float) finalx);
	      *y = base_y;
	    }
	}
    }
  else
    {
      *lowerx += pFrame->FrXOrg;
      *higherx += pFrame->FrXOrg;
      *y += pFrame->FrYOrg;
    }
  /*Make sure the transformation stack is empty*/
  glLoadIdentity ();

#endif /* _GL */
}

#ifdef _GL
/*----------------------------------------------------------------------
  SyncBoundingboxes : sync Bounding box of a group according to openGL 
  Compued values
  ----------------------------------------------------------------------*/
static void SyncBoundingboxesReal (PtrAbstractBox pInitAb, 
				   int XFrame, int YFrame, int frame)
{
  PtrAbstractBox      pAb;  
  PtrBox              box;
  int                 x, y;
  
  box = pInitAb->AbBox;
  x = box->BxClipX - XFrame;
  y = box->BxClipY - YFrame;
  pAb = pInitAb->AbFirstEnclosed;
  while (pAb != NULL)
    {
      if (pAb->AbLeafType != LtCompound)
	{
	  box = pAb->AbBox;
	  if (box->BxType == BoPiece ||
	      box->BxType == BoScript ||
	      box->BxType == BoMulScript ||
	      box->BxType == BoSplit)
	    {
	      while (box != NULL)
		{
		  box->BxClipX -= XFrame;
		  box->BxClipY -= YFrame;
		  box = box->BxNexChild;
		}
	    }
	}
      else
	{
	  if (pAb->AbElement->ElSystemOrigin)
	    {
	      CoordinateSystemUpdate (pAb, frame, 
				      pAb->AbBox->BxXOrg, 
				      pAb->AbBox->BxYOrg);
	      /* pAb->AbBox->BxXOrg = 0; */
	      /* pAb->AbBox->BxYOrg = 0; */
	      SyncBoundingboxesReal (pAb, XFrame, YFrame, frame);
	    }
	  else 
	    {
	      /* compute children of this box*/
	      SyncBoundingboxesReal (pAb, XFrame, YFrame, frame);
	    }
	}
      pAb = pAb->AbNext;
    }
  box = pInitAb->AbBox;
  box->BxClipX = x;
  box->BxClipY = y;
}

/*----------------------------------------------------------------------
  SyncBoundingboxes : sync Bounding box of a group
  ----------------------------------------------------------------------*/
static void SyncBoundingboxes (PtrAbstractBox pInitAb, 
			       int XFrame, int YFrame, int frame, 
			       int FrXOrg, int FrYOrg)
{
  PtrAbstractBox      pAb;  
  PtrBox              box;
  int                 x, y, w, h;
   
  box = pInitAb->AbBox;
  box->BxClipX = box->BxXOrg - FrXOrg;
  box->BxClipY = box->BxYOrg - FrYOrg;
  box->BxClipW = box->BxWidth; 
  box->BxClipH = box->BxHeight; 
  x = box->BxClipX;
  y = box->BxClipY;
  w = box->BxClipW; 
  h = box->BxClipH; 
  pAb = pInitAb->AbFirstEnclosed;
  while (pAb != NULL)
    {
      if (!pAb->AbDead && pAb->AbBox)
	{
	  if (pAb->AbLeafType != LtCompound)
	    {
	      box = pAb->AbBox;
	      box->BxClipX = box->BxXOrg - FrXOrg;
	      box->BxClipY = box->BxYOrg - FrYOrg;
	      box->BxClipW = box->BxWidth; 
	      box->BxClipH = box->BxHeight; 
	      if (box->BxType == BoPiece ||
		  box->BxType == BoScript ||
		  box->BxType == BoMulScript ||
		  box->BxType == BoSplit)
		{
		  box = box->BxNexChild;
		  while (box != NULL)
		    {
		      box->BxClipX = box->BxXOrg - FrXOrg;
		      box->BxClipY = box->BxYOrg - FrYOrg;
		      box->BxClipW = box->BxWidth; 
		      box->BxClipH = box->BxHeight; 
		      box = box->BxNexChild;
		    }
		}
	      box = pAb->AbBox;
	    }
	  else
	    {
	      if (pAb->AbElement->ElSystemOrigin && 
		  FrameTable[frame].FrView == 1)
		{
		  CoordinateSystemUpdate (pAb, frame, 
					  pAb->AbBox->BxXOrg, 
					  pAb->AbBox->BxYOrg);
		  /* pAb->AbBox->BxXOrg = 0; */
		  /* pAb->AbBox->BxYOrg = 0; */
		  SyncBoundingboxesReal (pAb, XFrame, YFrame, frame);
		}
	      else
		/* compute children of this box*/
		SyncBoundingboxes (pAb, XFrame, YFrame, frame, FrXOrg, FrYOrg);
	    }
	}
      pAb = pAb->AbNext;
    }
  /*   box = pInitAb->AbBox; */
  /*   box->BxClipX = x; */
  /*   box->BxClipY = y; */
  /*   box->BxClipW = w; */
  /*   box->BxClipH = h; */
}

/*----------------------------------------------------------------------------------
  ComputeBoundingBoxes : Compute clipping coordinate for each box.
  --------------------------------------------------------------------------------*/
static void ComputeBoundingBoxes (int frame, int xmin, int xmax, int ymin, int ymax,
				  PtrAbstractBox pInitAb)
{
  PtrAbstractBox      pAb, specAb;
  PtrBox              pBox, box;
  PtrBox              topBox;
  ViewFrame          *pFrame;
  /* PictInfo           *imageDesc; */
  int                 plane;
  int                 nextplane;
  int                 winTop, winBottom;
  int                 bt, bb;
  int                 l, h;
  ThotBool            userSpec = FALSE;
  int                 Clipx, Clipy, Clipw, Cliph; 
  ThotBool            FormattedFrame;

  pFrame = &ViewFrameTable[frame - 1];
  GetSizesFrame (frame, &l, &h);
  if (xmax == 0 && ymax == 0)
    {
      xmax = l; 
      ymax = h;
    }
  winTop = pFrame->FrYOrg;
  winBottom = winTop + h;
  pBox = pInitAb->AbBox;
  if (pBox == NULL)
    return;
  /* Display planes in reverse order from biggest to lowest */
  plane = 65536;
  nextplane = plane - 1;
  topBox = NULL;
  Clipx = -1;	  
  Clipy = -1; 
  Clipw = 0; 
  Cliph = 0;  
  if (FrameTable[frame].FrView == 1)
    FormattedFrame = TRUE;
  else
    FormattedFrame = FALSE;
  while (plane != nextplane && pInitAb->AbFirstEnclosed)
    /* there is a new plane to display */
    {
      plane = nextplane;
      /* Draw all the boxes not yet displayed */
      pAb = pInitAb->AbFirstEnclosed;
      while (pAb)
	{ 
	  if (pAb->AbDepth == plane &&
	      pAb != pFrame->FrAbstractBox &&
	      pAb->AbBox)
	    {
	      /* box in the current plane */
	      pBox = pAb->AbBox;
	      if (pAb->AbElement && FormattedFrame)
		{
		  if (pAb->AbElement->ElSystemOrigin)
		    DisplayBoxTransformation (pAb->AbElement->ElTransform, 
					      pFrame->FrXOrg, pFrame->FrYOrg);
		  if (pAb->AbElement->ElTransform)
		    DisplayTransformation (pAb->AbElement->ElTransform, 
					   pBox->BxWidth, 
					   pBox->BxHeight);     
		}
	      if (pAb->AbOpacity != 1000
		  && !TypeHasException (ExcIsGroup, pAb->AbElement->ElTypeNumber,
					pAb->AbElement->ElStructSchema))
		{
		  if (pAb->AbFirstEnclosed)
		    {
		      pAb->AbFirstEnclosed->AbFillOpacity = pAb->AbOpacity;      
		      pAb->AbFirstEnclosed->AbStrokeOpacity = pAb->AbOpacity; 
		    }
		  else 
		    {
		      pAb->AbFillOpacity = pAb->AbOpacity;      
		      pAb->AbStrokeOpacity = pAb->AbOpacity; 
		    }
		   	      
		}
	      if (pAb->AbLeafType == LtCompound)
		{
		  /* Initialize bounding box*/
		/*   pBox->BxClipX = 0;	   */
/* 		  pBox->BxClipY = 0;  */
/* 		  pBox->BxClipW = 0;  */
/* 		  pBox->BxClipH = 0;  */
		  if (pAb->AbVisibility >= pFrame->FrVisibility &&
		      (pBox->BxDisplay || pAb->AbSelected))
		    {
		      ComputeFilledBox (pBox, frame, xmin, xmax, ymin, ymax);
		    }		      
		  if (pBox->BxNew && pAb->AbFirstEnclosed == NULL)
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
			      /* AddBoxToCreate (create, specAb->AbBox, frame); */
			      userSpec = TRUE;
			    }
			  else
			    specAb = specAb->AbEnclosing;
			}
		    }
		  ComputeBoundingBoxes (frame, xmin, xmax, ymin, ymax, pAb);		      
		}
	      else
		{
		  /* look for the box displayed at the top of the window */
		  if (pBox->BxType == BoMulScript ||
		      pBox->BxType == BoSplit)
		    {
		      /* the box itself doen't give right positions */
		      box = pBox->BxNexChild;
		      bt = box->BxClipY;
		      /* don't take into account the last empty box */
		      while (box->BxNexChild &&
			     (box->BxNexChild->BxNChars > 0 ||
			      box->BxNexChild->BxNexChild))
			box = box->BxNexChild;
		      bb = box->BxClipY + box->BxClipH;
		    }
		  else
		    {
		      bt = pBox->BxClipY;
		      bb = pBox->BxClipY + pBox->BxClipH;
		    }
		  {
		    if (pBox->BxType == BoSplit || pBox->BxType == BoMulScript)
		      /* the box itself doen't give right positions */
		      box = pBox->BxNexChild;
		    else
		      box = pBox;
		    if (topBox == NULL)
		      topBox = box;
		    else if (bt >= winTop && topBox->BxClipY < winTop)
		      /* the top of the box should be visible */
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
				/* AddBoxToCreate (create, specAb->AbBox, frame); */
				userSpec = TRUE;
			      }
			    else
			      specAb = specAb->AbEnclosing;
			  }
		      }
		    if (!userSpec)
		      {
			if (pBox->BxType == BoSplit || 
			    pBox->BxType == BoMulScript)
			  while (pBox->BxNexChild)
			    {
			      pBox = pBox->BxNexChild;
			      ComputeBoundingBox (pBox, frame, xmin, xmax, ymin, ymax);
			    }
			else
			  ComputeBoundingBox (pBox, frame, xmin, xmax, ymin, ymax);

		      }
		  }
		} 
	      if (pAb->AbElement && FormattedFrame)
		{
		  if (pAb->AbElement->ElSystemOrigin)
		    DisplayTransformationExit ();
		  if (IsTransformed (pAb->AbElement->ElTransform))
		    DisplayTransformationExit ();
		}      
	      /* X and Y is the smallest of all enclosed boxes*/
	      if (pBox->BxBoundinBoxComputed)
		{
		  if (Clipx == -1)
		    Clipx = pBox->BxClipX;
		  else 
		    if (pBox->BxClipX < Clipx) 
		      {
			Clipw += Clipx - pBox->BxClipX;
			Clipx = pBox->BxClipX;	  
		      }
		  if (Clipy == -1)
		    Clipy = pBox->BxClipY;
		  else 
		    if (pBox->BxClipY < Clipy) 
		      {
			Cliph += Clipy - pBox->BxClipY;
			Clipy = pBox->BxClipY; 
		      }
		  /* Make sure that Height and Width is correct...*/
		  if ((pBox->BxClipW + pBox->BxClipX) > (Clipx + Clipw))
		    {
		      Clipw = (pBox->BxClipW + pBox->BxClipX) - Clipx;		  
		    }
		  if ((pBox->BxClipY + pBox->BxClipH) > (Clipy + Cliph))
		    {
		      Cliph = (pBox->BxClipY + pBox->BxClipH) - Clipy;		  
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
	  /* get the next sibling */
	  pAb = pAb->AbNext;
	}
    }
  pBox = pInitAb->AbBox;
  if (Clipx != -1)
    pBox->BxClipX = Clipx;	  
  if (Clipy != -1)
    pBox->BxClipY = Clipy; 
  pBox->BxClipW = Clipw; 
  pBox->BxClipH = Cliph; 
  if (pBox->BxClipW && pBox->BxClipH)
    pBox->BxBoundinBoxComputed = TRUE;
  else
    pBox->BxBoundinBoxComputed = FALSE;
}
#endif /*_GL*/

#ifndef _GL
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
  ThotBool            userSpec = FALSE;

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
		  if (pBox->BxNew && pAb->AbFirstEnclosed == NULL)
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
		}
	      else
		{
		  /* look for the box displayed at the top of the window */
		  if (pBox->BxType == BoSplit || pBox->BxType == BoMulScript)
		    {
		      /* the box itself doesn't give right positions */
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
		      if (pBox->BxType == BoSplit || pBox->BxType == BoMulScript)
			/* the box itself doen't give right positions */
			box = pBox->BxNexChild;
		      else
			box = pBox;
		      if (topBox == NULL)
			topBox = box;
		      else if (bt >= winTop && topBox->BxYOrg < winTop)
			/* the top of the box should be visible */
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
			  if (pBox->BxType == BoSplit || 
			      pBox->BxType == BoMulScript)
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
	  if (pAb->AbLeafType == LtCompound && 
	      pAb->AbFirstEnclosed)
	    /* get the first child */
	    pAb = pAb->AbFirstEnclosed;
	  else if (pAb->AbNext)
	    /* get the next sibling */
	    {
	      OpacityAndTransformNext (pAb, plane, frame, xmin, xmax, ymin, ymax, FALSE);	      
	      pAb = pAb->AbNext;
	    }
	  else
	    {
	      /* go up in the tree */
	      while (pAb->AbEnclosing && 
		     pAb->AbEnclosing->AbNext == NULL)
		{
		  OpacityAndTransformNext (pAb, plane, frame, xmin, xmax, ymin, ymax, FALSE);
		  pAb = pAb->AbEnclosing;
		}
	      OpacityAndTransformNext (pAb, plane, frame, xmin, xmax, ymin, ymax, FALSE);
	      pAb = pAb->AbEnclosing;
	      if (pAb)
		{
		  OpacityAndTransformNext (pAb, plane, frame, xmin, xmax, ymin, ymax, FALSE);
		  pAb = pAb->AbNext;
		}
	    }
	}
    }
  return topBox;
}
#else /* _GL */
/*----------------------------------------------------------------------
  NoBoxModif : True if box or box's son not modified
  ----------------------------------------------------------------------*/
ThotBool NoBoxModif (PtrAbstractBox      pinitAb)
{
  PtrAbstractBox      pAb;

  if (pinitAb->AbBox->VisibleModification || pinitAb->AbSelected)
    return FALSE;
  pAb = pinitAb->AbFirstEnclosed;
  while (pAb)
    {
      if (pAb->AbBox->VisibleModification || pAb->AbSelected)
	return FALSE;
      if (pAb->AbFirstEnclosed)
	/* get the first child */
	pAb = pAb->AbFirstEnclosed;
      else if (pAb->AbNext)	    
	{
	  /* get the next sibling */
	  pAb = pAb->AbNext;
	}
      else
	{
	  /* go up in the tree */
	  while (pAb->AbEnclosing && 
		 pAb->AbEnclosing->AbNext == NULL)
	    {
	      pAb = pAb->AbEnclosing;
	    }
	  pAb = pAb->AbEnclosing;
	  if (pAb)
	    {
	      pAb = pAb->AbNext;
	    }
	}
    }
  return TRUE;
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
  ThotBool            userSpec = FALSE;
  ThotBool            FrameUpdatingStatus, FormattedFrame;
  int                 x_real_min, x_real_max,
                      y_real_min, y_real_max;
  int                 OldXOrg, OldYOrg, 
                      ClipXOfFirstCoordSys, ClipYOfFirstCoordSys;
  ThotBool            NotGroupOpacityDisplayed;

  FrameUpdatingStatus = FrameUpdating;
  FrameUpdating = TRUE;  
  pFrame = &ViewFrameTable[frame - 1];
  pAb = pFrame->FrAbstractBox;
  GetSizesFrame (frame, &l, &h);
  winTop = 0;
  winBottom = h;
  pBox = pAb->AbBox;
  *tVol = *bVol = 0;
  if (pBox == NULL)
    return NULL;
  /* Display planes in reverse order from biggest to lowest */
  plane = 65536;
  nextplane = plane - 1;
  topBox = NULL;
  pAb = pFrame->FrAbstractBox;
  if (FrameTable[frame].FrView == 1)
    FormattedFrame = TRUE;
  else
    FormattedFrame = FALSE;
  x_real_min = xmin - pFrame->FrXOrg;
  x_real_max = xmax - pFrame->FrXOrg;
  y_real_min = ymin - pFrame->FrYOrg;
  y_real_max = ymax - pFrame->FrYOrg;
  OldXOrg = 0;
  OldYOrg = 0;
  ClipXOfFirstCoordSys = ClipYOfFirstCoordSys = 0;
  if (pBox->BxDisplay || pAb->AbSelected)
    DrawFilledBox (pAb, frame, xmin, xmax, ymin, ymax);
  while (plane != nextplane)
    /* there is a new plane to display */
    {
      plane = nextplane;
      /* Draw all the boxes not yet displayed */
      pAb = pFrame->FrAbstractBox;
      NotGroupOpacityDisplayed = TRUE;
      while (pAb)
	{
	  if (pAb->AbDepth == plane &&
	      pAb != pFrame->FrAbstractBox &&
	      pAb->AbBox)
	    {
	      /* box in the current plane */
	      pBox = pAb->AbBox;
	      if (pAb->AbElement && pAb->AbDepth == plane 
		  && NotGroupOpacityDisplayed)
		{ 
		  if (FormattedFrame)
		    {
		      /* If the coord sys origin is translated, 
			 it must be before any other transfromation*/
		      if (pAb->AbElement->ElSystemOrigin)
			DisplayBoxTransformation (pAb->AbElement->ElTransform, 
						  pFrame->FrXOrg, pFrame->FrYOrg);
		      /* Normal transformation*/
		      if (pAb->AbElement->ElTransform)
			DisplayTransformation (pAb->AbElement->ElTransform, 
					       pBox->BxWidth, 
					       pBox->BxHeight);
		      if (pAb->AbElement->ElSystemOrigin)
			{
			  /*Need to Get REAL computed COORD ORIG
			    instead of Computing Bounding Boxes forever...
			    As it's am "optimisation it'll come later :
			    if computed, no more compute, use synbounding, 
			    else compute if near screen"*/

			  /* if (pBox->BxTransformationComputed) */
			  /* 		      { */
			  /* 			GetBoxTransformed (pAb->AbElement->ElTransform,  */
			  /* 					   &(pBox->BxClipX),  */
			  /* 					   &(pBox->BxClipY)); */
			  /* 			pBox->BxClipX -= pFrame->FrXOrg; */
			  /* 			pBox->BxClipY -= pFrame->FrYOrg; */
			  /* 		      } */
			  /*  if (pBox->BxClipY + pBox->BxClipH >= y_real_min  && */
			  /* 			  pBox->BxClipY <= y_real_max &&  */
			  /* 			  pBox->BxClipX + pBox->BxClipW >= x_real_min && */
			  /* 			  pBox->BxClipX <= x_real_max) */
			  {
			    if (pFrame->FrXOrg ||pFrame->FrYOrg)
			      {
				OldXOrg = pFrame->FrXOrg;
				OldYOrg = pFrame->FrYOrg;
				pFrame->FrXOrg = 0;
				pFrame->FrYOrg = 0;
				ComputeBoundingBoxes (frame, 
						      x_real_min, x_real_max,
						      y_real_min, y_real_max, pAb);
				ClipXOfFirstCoordSys = pBox->BxClipX;
				ClipYOfFirstCoordSys = pBox->BxClipY;
			      }
			    else
			      ComputeBoundingBoxes (frame, 
						    x_real_min, x_real_max,
						    y_real_min, y_real_max, pAb);
			  }
			  /* New Coordinate system means Clipping around */
			  /* 			GL_PushClip (pBox->BxClipX, pBox->BxClipY,  */
			  /* 					pBox->BxClipW, pBox->BxClipH);   */
			}
		      if (pAb->AbOpacity != 1000)
			{
			  if (TypeHasException (ExcIsGroup, pAb->AbElement->ElTypeNumber,
						pAb->AbElement->ElStructSchema) )
			    {
			      if (pAb->AbOpacity == 0 ||
				  (NoBoxModif (pAb) &&
				   pAb->AbBox->Post_computed_Pic != NULL))
				{
				  NotGroupOpacityDisplayed = FALSE;
				  continue;
				}
			      else
				  {
				    if (pAb->AbBox->Post_computed_Pic)
				      {
					FreeGlTextureNoCache (pAb->AbBox->Post_computed_Pic);
					TtaFreeMemory (pAb->AbBox->Post_computed_Pic);
					pAb->AbBox->Post_computed_Pic = NULL; 
				      }
				    OpaqueGroupTexturize (pAb, frame, x_real_min, x_real_max, y_real_min, y_real_max, TRUE);
				    ClearOpaqueGroup (pAb, frame, x_real_min, x_real_max, y_real_min, y_real_max);
				  }
			    }
			  else
			    {
			      if (pAb->AbFirstEnclosed)
				{
				  pAb->AbFirstEnclosed->AbFillOpacity = pAb->AbOpacity;      
				  pAb->AbFirstEnclosed->AbStrokeOpacity = pAb->AbOpacity; 
				}
			      else 
				{
				  pAb->AbFillOpacity = pAb->AbOpacity;      
				  pAb->AbStrokeOpacity = pAb->AbOpacity; 
				}
			    }		      
			}
		    }
		  if (pAb->AbLeafType == LtCompound)
		    {
		      if (pAb->AbVisibility >= pFrame->FrVisibility &&
			  (pBox->BxDisplay || pAb->AbSelected))
			DrawFilledBox (pAb, frame, xmin, xmax, ymin, ymax);
		      if (pBox->BxNew && pAb->AbFirstEnclosed == NULL)
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
		    }
		  else
		    {
		      /* look for the box displayed at the top of the window */
		      if (pBox->BxType == BoSplit || 
			  pBox->BxType == BoMulScript)
			{
			  /* the box itself doesn't give right positions */
			  box = pBox->BxNexChild;
			  bt = box->BxClipY;

			  /* don't take into account the last empty box */
			  while (box->BxNexChild &&
				 (box->BxNexChild->BxNChars > 0 ||
				  box->BxNexChild->BxNexChild))
			    box = box->BxNexChild;
			  bb = box->BxClipY + box->BxClipH;

			}
		      else
			{
			  bt = pBox->BxClipY;
			  bb = pBox->BxClipY + pBox->BxClipH;
			}
		      if (bb < winTop)
			/* the box is not visible */
			*tVol = *tVol + pBox->BxAbstractBox->AbVolume;
		      else if (bt > winBottom)
			/* the box is not visible */
			*bVol = *bVol + pBox->BxAbstractBox->AbVolume;
		      else
			{
			  if (pBox->BxType == BoSplit || pBox->BxType == BoMulScript)
			    /* the box itself doen't give right positions */
			    box = pBox->BxNexChild;
			  else
			    box = pBox;
			  if (topBox == NULL)
			    topBox = box;
			  else if (bt >= winTop && 
				   topBox->BxClipY < winTop)
			    /* the top of the box should be visible */
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
			      if (pBox->BxType == BoSplit || 
				  pBox->BxType == BoMulScript)
				while (pBox->BxNexChild)
				  {
				    pBox = pBox->BxNexChild;
				    if (pBox->BxClipY + pBox->BxClipH >= y_real_min  &&
					pBox->BxClipY <= y_real_max && 
					pBox->BxClipX + pBox->BxClipW  + pBox->BxEndOfBloc>= x_real_min &&
					pBox->BxClipX <= x_real_max)
				      {
					DisplayBox (pBox, frame, xmin, xmax, ymin, ymax);
				      }				    
				  }
			      else if (bb >= y_real_min  &&
				       bt <= y_real_max && 
				       pBox->BxClipX + pBox->BxClipW >= x_real_min &&
				       pBox->BxClipX <= x_real_max)
				{
				  DisplayBox (pBox, frame, xmin, xmax, ymin, ymax);
				}
			      else if (pBox->BxType == BoPicture)
				{
				  imageDesc = (PictInfo *)pBox->BxPictInfo;
				  if (bb < winTop ||
				      bt > winBottom ||
				      pBox->BxClipX + pBox->BxClipW  + pBox->BxEndOfBloc< x_real_min ||
				      pBox->BxClipX > x_real_max)
				    UnmapImage (imageDesc);
				  else if (imageDesc->PicType >= PLUGIN_FORMAT)
				    /* redisplay plugins */
				    {
				      DisplayBox (pBox, frame, xmin, xmax, ymin, ymax);
				    }
				}
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
	  if (pAb->AbLeafType == LtCompound && 
	      pAb->AbFirstEnclosed && 
	      NotGroupOpacityDisplayed)
	    /* get the first child */
	    pAb = pAb->AbFirstEnclosed;
	  else if (pAb->AbNext)	    
	    {	 
	      if (FormattedFrame)
		{
		  OpacityAndTransformNext (pAb, plane, frame, x_real_min, x_real_max, y_real_min, y_real_max, TRUE);
		  OriginSystemExit (pAb, pFrame, plane, OldXOrg, OldYOrg, 
				    ClipXOfFirstCoordSys, ClipYOfFirstCoordSys);
		} 
	      NotGroupOpacityDisplayed = TRUE;
	      /* get the next sibling */
	      pAb = pAb->AbNext;
	    }
	  else
	    {
	      /* go up in the tree */
	      while (pAb->AbEnclosing && 
		     pAb->AbEnclosing->AbNext == NULL)
		{
		  if (FormattedFrame)
		    {
		      OpacityAndTransformNext (pAb, plane, frame, x_real_min, x_real_max, y_real_min, y_real_max, TRUE);		  	
		      OriginSystemExit (pAb, pFrame, plane, OldXOrg, OldYOrg, 
					ClipXOfFirstCoordSys, ClipYOfFirstCoordSys);
		    }
		  NotGroupOpacityDisplayed = TRUE;
		  pAb = pAb->AbEnclosing;
		}
	      if (FormattedFrame)
		{	
		  OpacityAndTransformNext (pAb, plane, frame, x_real_min, x_real_max, y_real_min, y_real_max, TRUE);
		  OriginSystemExit (pAb, pFrame, plane, OldXOrg, OldYOrg, 
				    ClipXOfFirstCoordSys, ClipYOfFirstCoordSys);	  
		}
	      NotGroupOpacityDisplayed = TRUE;
	      pAb = pAb->AbEnclosing;
	      if (pAb)
		{
		  if (FormattedFrame)
		    {
		      OpacityAndTransformNext (pAb, plane, frame, x_real_min, x_real_max, y_real_min, y_real_max, TRUE);
		      OriginSystemExit (pAb, pFrame, plane, OldXOrg, OldYOrg, 
					ClipXOfFirstCoordSys, ClipYOfFirstCoordSys);	  
		    }
		  NotGroupOpacityDisplayed = TRUE;
		  pAb = pAb->AbNext;
		}
	    }
	}
    } 
  FrameUpdating = FrameUpdatingStatus;  
  return topBox;
}
#endif  /* _GL */

/*--------------------------------------------------------
  ComputeABoundingBox : Compute a unique bounding box, 
  but with current matrix stack
  --------------------------------------------------------*/
void ComputeABoundingBox (PtrAbstractBox pAbSeeked, int frame)
{
#ifdef _GL
  PtrAbstractBox      pAb;
  PtrBox              pBox;
  ViewFrame          *pFrame;
  int                 plane;
  int                 nextplane;
  int                 l, h;
  ThotBool            FrameUpdatingStatus, FormattedFrame;
  int                 OldXOrg, OldYOrg, 
    ClipXOfFirstCoordSys, ClipYOfFirstCoordSys;
   
  FrameUpdatingStatus = FrameUpdating;
  FrameUpdating = TRUE;  
  pFrame = &ViewFrameTable[frame - 1];
  pAb = pFrame->FrAbstractBox;
  GetSizesFrame (frame, &l, &h);
  pBox = pAb->AbBox;
  if (pBox == NULL)
    return;
  if (FrameTable[frame].FrView == 1)
    FormattedFrame = TRUE;
  else
    FormattedFrame = FALSE;
  if (FormattedFrame || 
      IsBoxTransformed (pAbSeeked->AbBox))
    {
      /* Display planes in reverse order from biggest to lowest */
      plane = 65536;
      nextplane = plane - 1;
      pAb = pFrame->FrAbstractBox;      
      OldXOrg = 0;
      OldYOrg = 0;
      ClipXOfFirstCoordSys = ClipYOfFirstCoordSys = 0;
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
		  if (pAb->AbElement && 
		      pAb->AbDepth == plane)
		    { 		     
		      /* If the coord sys origin is translated, 
			 it must be before any other transfromation*/
		      if (pAb->AbElement->ElSystemOrigin)
			DisplayBoxTransformation (pAb->AbElement->ElTransform, 
						  pFrame->FrXOrg, pFrame->FrYOrg);
		      /* Normal transformation*/
		      if (pAb->AbElement->ElTransform)
			DisplayTransformation (pAb->AbElement->ElTransform, 
					       pBox->BxWidth, 
					       pBox->BxHeight);
		      if (pAb->AbElement->ElSystemOrigin)
			{ 
			  if (pFrame->FrXOrg ||pFrame->FrYOrg)
			    {
			      OldXOrg = pFrame->FrXOrg;
			      OldYOrg = pFrame->FrYOrg;
			      pFrame->FrXOrg = 0;
			      pFrame->FrYOrg = 0;
			      ClipXOfFirstCoordSys = pBox->BxClipX;
			      ClipYOfFirstCoordSys = pBox->BxClipY;
			    }
			}		
		      if (pAb == pAbSeeked) 
			{
			  if (pAb->AbLeafType == LtCompound)
			    {
			      if (pAb->AbVisibility >= pFrame->FrVisibility &&
				  (pBox->BxDisplay || pAb->AbSelected))
				ComputeFilledBox (pAb->AbBox, frame, 0, l, 0, h);
			      ComputeBoundingBoxes (frame, 0, l, 0, h, pAb);
			    }
			  else
			    {
			      if (pBox->BxType == BoSplit || 
				  pBox->BxType == BoMulScript)
				{
				  while (pBox->BxNexChild)
				    {
				      pBox = pBox->BxNexChild;			
				      ComputeBoundingBox (pBox, frame, 0, l, 0, h);
				    }
				}
			      else
				{
				  ComputeBoundingBox (pBox, frame,  0, l, 0, h);
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
	      if (pAb->AbLeafType == LtCompound && 
		  pAb->AbFirstEnclosed)
		/* get the first child */
		pAb = pAb->AbFirstEnclosed;
	      else if (pAb->AbNext)	    
		{	 		  
		  OpacityAndTransformNext (pAb, plane, frame, 0, 0, 0, 0, FALSE);
		  OriginSystemExit (pAb, pFrame, plane, OldXOrg, OldYOrg, 
				    ClipXOfFirstCoordSys, ClipYOfFirstCoordSys);		   
		  /* get the next sibling */
		  pAb = pAb->AbNext;
		}
	      else
		{
		  /* go up in the tree */
		  while (pAb->AbEnclosing && 
			 pAb->AbEnclosing->AbNext == NULL)
		    {		     
		      OpacityAndTransformNext (pAb, plane, frame, 0, 0, 0, 0, FALSE);		  	
		      OriginSystemExit (pAb, pFrame, plane, OldXOrg, OldYOrg, 
					ClipXOfFirstCoordSys, ClipYOfFirstCoordSys);
		      pAb = pAb->AbEnclosing;
		    }
		  
		  OpacityAndTransformNext (pAb, plane, frame, 0, 0, 0, 0, FALSE);
		  OriginSystemExit (pAb, pFrame, plane, OldXOrg, OldYOrg, 
				    ClipXOfFirstCoordSys, ClipYOfFirstCoordSys);	  
		  pAb = pAb->AbEnclosing;
		  if (pAb)
		    {
		      OpacityAndTransformNext (pAb, plane, frame, 0, 0, 0, 0, FALSE);
		      OriginSystemExit (pAb, pFrame, plane, OldXOrg, OldYOrg, 
					ClipXOfFirstCoordSys, ClipYOfFirstCoordSys);	  
		      pAb = pAb->AbNext;
		    }
		}
	    }
	} 
    }
  else
    ComputeBoundingBoxes (frame, 0, l, 0, h, pAbSeeked);
  FrameUpdating = FrameUpdatingStatus;
#endif /* _GL */
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
ThotBool RedrawFrameTop (int frame, int scroll)
{
  PtrBox              topBox = NULL;
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
	   pFrame->FrYOrg - scroll + h > ymin
	   )
    {
      pFrame->FrYOrg -= scroll;
#ifdef _GL
      SyncBoundingboxes (pFrame->FrAbstractBox, 0, -scroll, frame, pFrame->FrXOrg, pFrame->FrYOrg);
#endif /* _GL */
      top = pFrame->FrYOrg;
      bottom = top + h;
      /* used to store boxes created on the fly */
      create = NULL;
      tVol = bVol = 0;
#if defined(_WINDOWS) && !defined(_WIN_PRINT)
      WIN_GetDeviceContext (frame);
#endif /* __WINDOWS && !_WINT_PRINT */
#ifdef _GL      
      if (GL_prepare (frame))
	{
#endif /* _GL */
	  DefineClipping (frame, pFrame->FrXOrg, pFrame->FrYOrg,
			  &xmin, &ymin, &xmax, &ymax, 1);
	  
	  /* Is there a need to redisplay part of the frame ? */
	  if (xmin < xmax && ymin < ymax)
	    topBox = DisplayAllBoxes (frame, xmin, xmax, ymin, ymax,
				      &create, &tVol, &bVol);
	  
	  /* The updated area is redrawn */
	  DefClip (frame, 0, 0, 0, 0);
	  RemoveClipping (frame);
#if defined(_WINDOWS) && !defined(_WIN_PRINT)
	  WIN_ReleaseDeviceContext ();
#endif /*  _WINDOWS && !WIN_PRINT */
	  if (!ShowOnePage)
	    {
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
		      /* Recompute the location of the frame in the abstract image */
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
		}
	      /* update of image is finished */
	      FrameUpdating = FALSE;
	    }
#ifdef _GL
	  GL_realize (frame);
	}
#endif /* _GL */ 
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
ThotBool RedrawFrameBottom (int frame, int scroll, PtrAbstractBox subtree)
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
  ThotBool            toadd;

  /* are new abstract boxes needed */
  toadd = FALSE;
  pFrame = &ViewFrameTable[frame - 1];
  GetSizesFrame (frame, &l, &h);
  xmin = pFrame->FrClipXBegin;
  xmax = pFrame->FrClipXEnd;
  ymin = pFrame->FrClipYBegin;
  ymax = pFrame->FrClipYEnd;
  if (pFrame->FrAbstractBox && 
      pFrame->FrAbstractBox->AbElement == NULL)
    pFrame->FrAbstractBox = NULL;
  if (!pFrame->FrReady || 
      pFrame->FrAbstractBox == NULL)
    return toadd;
  else if (xmin < xmax &&
	   ymin < ymax &&
	   pFrame->FrXOrg < xmax &&
	   pFrame->FrYOrg + scroll < ymax &&
	   pFrame->FrXOrg + l > xmin &&
	   pFrame->FrYOrg + scroll + h > ymin
     )
    {
      pFrame->FrYOrg += scroll;
#ifdef _GL
      SyncBoundingboxes (pFrame->FrAbstractBox, 0, scroll, frame, pFrame->FrXOrg, pFrame->FrYOrg);
#endif /* _GL */
      top = pFrame->FrYOrg;
      bottom = top + h;
      tVol = bVol = 0;
      delta = top - h / 2;
      pRootBox = topBox = NULL;
      y = 0;
      /* used to store boxes created on the fly */
      create = NULL;
      /* Redraw from top to bottom all filled boxes */
#if defined(_WINDOWS) && !defined(_WIN_PRINT)
      WIN_GetDeviceContext (frame);
#endif /* __WINDOWS && !_WINT_PRINT */
      /* Is there a need to redisplay part of the frame ? */
      if (xmin < xmax && ymin < ymax)
	{ 
#ifdef _GL
	  if (GL_prepare (frame))
	    {
#endif /*_GL*/
	      DefineClipping (frame, pFrame->FrXOrg, pFrame->FrYOrg,
			      &xmin, &ymin, &xmax, &ymax, 1);

	      topBox = DisplayAllBoxes (frame, xmin, xmax, ymin,
					ymax, &create, &tVol, &bVol);
	      /* The updated area is redrawn */
	      DefClip (frame, 0, 0, 0, 0);
	      RemoveClipping (frame);
#if defined(_WINDOWS) && !defined(_WIN_PRINT)
	      WIN_ReleaseDeviceContext ();
#endif /* __WINDOWS && !_WINT_PRINT */
	      if (!ShowOnePage)
		{
		  /* if needed complete the partial existing image */
		  pRootBox = pFrame->FrAbstractBox->AbBox;
		  if (!FrameUpdating && 
		      (!TextInserting || scroll > 0))
		    {
		      /* The concrete image is being updated */
		      FrameUpdating = TRUE;
		      y = top - pRootBox->BxYOrg;
		      if (pFrame->FrAbstractBox->AbInLine)
			FrameUpdating = FALSE;
		      else if (pFrame->FrAbstractBox->AbTruncatedHead && 
			       y < 0)
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
			  (y >= 0
/* || (!pFrame->FrAbstractBox->AbHeight.DimIsPosition &&
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
			  if (tVol)
			    Clear (frame, l, y, 0, pRootBox->BxYOrg + pRootBox->BxHeight);
			  /* don't loop is volume didn't change */
			  tVol = pFrame->FrAbstractBox->AbVolume;
			  /* Volume to add */
			  bottom = (y + h / 2) * l;
			  IncreaseVolume (FALSE, GetCharsCapacity (bottom), frame);
			  tVol -= pFrame->FrAbstractBox->AbVolume;
			  /* Image should be completed */
			  FrameUpdating = FALSE;
			  if (tVol != 0)
			    /* Maybe the image is not complete yet */
			    RedrawFrameBottom (frame, 0, NULL);
			}
		      else
			{
			  /* Volume computed is sufficient */
			  /* Is a cleanup of the bottom of frame needed ? */
			  if (y > 0)
			    Clear (frame, l, y, 0, 
				   pRootBox->BxYOrg + pRootBox->BxHeight);
			  pFrame->FrVolume = pFrame->FrAbstractBox->AbVolume;
			}
		    }
		  /* update of image is finished */
		  FrameUpdating = FALSE;
		}
#ifdef _GL 
	      GL_realize (frame);
	    }
#endif /* _GL */
	  /* Interactive creation of boxes */
	  if (create)
	    {
	      pAb = create->BxAbstractBox;
	      DirectCreation (create, frame);
	      /* Volume computed is sufficient */
	      /* Is a cleanup of the bottom of frame needed ? */
	      if (y > 0)
		Clear (frame, l, y, 0, pRootBox->BxYOrg + pRootBox->BxHeight);
	      pFrame->FrVolume = pFrame->FrAbstractBox->AbVolume;
	      /* update of image is finished */
	      FrameUpdating = FALSE;
	    }
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
void DisplayFrame (int frame)
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
