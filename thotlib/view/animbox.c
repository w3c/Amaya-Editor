/*
 *
 *  (c) COPYRIGHT INRIA, 2003-2003
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
/*
 * Handle Animated Boxes
 *
 * Authors: P. Cheyrou (INRIA)
 *
 */
#include "ustring.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "picture.h"
#include "message.h"
#include "language.h"
#include "appdialogue.h"
#include "frame.h"
#include "content.h"

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "page_tv.h"
#include "platform_tv.h"
#include "edit_tv.h"
#include "modif_tv.h"
#include "frame_tv.h"
#include "appdialogue_tv.h"

#include "absboxes_f.h"
#include "abspictures_f.h"
#include "applicationapi_f.h"
#include "attributes_f.h"
#include "attrpresent_f.h"
#include "boxselection_f.h"
#include "buildboxes_f.h"
#include "changeabsbox_f.h"
#include "changepresent_f.h"
#include "content_f.h"
#include "createabsbox_f.h"
#include "createpages_f.h"
#include "exceptions_f.h"
#include "memory_f.h"
#include "presrules_f.h"
#include "presvariables_f.h"
#include "references_f.h"
#include "schemas_f.h"
#include "structmodif_f.h"
#include "tree_f.h"
#include "appli_f.h"
#include "frame_f.h"
#include "contentapi_f.h"

#ifdef _GL
#include "glwindowdisplay.h"
#endif /* _GL */
#include <math.h>

#ifdef _GL
static int Animated_Frame = 0;
static int Clipx, Clipy, ClipxMax, ClipyMax;

#define Min(number, min) (number = ( (number < 0) ? min : ((number < min) ? number : min)) )
#define Max(number, max) (number = ( (number < 0) ? max : ((number > max) ? number : max)) )

/*----------------------------------------------------------------------
  Define Minimum to be sure to get real clip.
  ----------------------------------------------------------------------*/
static void SetBaseClipping ()
{
  Clipx = -1;
  Clipy = -1;
  ClipyMax = -1;
  ClipxMax = -1;
}
/*----------------------------------------------------------------------
  UpdateClipping : Define Area to be recomputed by animation
  ----------------------------------------------------------------------*/
static void UpdateClipping (PtrAbstractBox pAb)
{
  while (pAb != NULL)
    {  
      /* if (!(pAb->AbBox->BxClipH || pAb->AbBox->BxClipW)) */
      /* 	ComputeBoundingBox (pAb->AbBox, */
      /* 			    Animated_Frame, */
      /* 			    0, 4096, */
      /* 			    0, 4096); */

      if (pAb->AbBox->BxClipH || pAb->AbBox->BxClipW)
	{
	  Min (Clipx, pAb->AbBox->BxClipX);		    
	  Max (ClipxMax, pAb->AbBox->BxClipX + pAb->AbBox->BxClipW);

	  Min (Clipy, pAb->AbBox->BxClipY);		    
	  Max (ClipyMax, pAb->AbBox->BxClipY + pAb->AbBox->BxClipH);
	}
      else if (pAb->AbBox->BxWidth || pAb->AbBox->BxHeight)
	{
	  Min (Clipx, pAb->AbBox->BxXOrg);		    
	  Max (ClipxMax, pAb->AbBox->BxXOrg + pAb->AbBox->BxWidth);

	  Min (Clipy, pAb->AbBox->BxYOrg);		    
	  Max (ClipyMax, pAb->AbBox->BxYOrg + pAb->AbBox->BxHeight);

	}
      UpdateClipping (pAb->AbFirstEnclosed);
      
      pAb = pAb->AbNext;
    }
}
/*----------------------------------------------------------------------
  interpolate_double_value : Compute a the value corresponding to a time
  using the "regle de trois" ("rule of tree")
  ----------------------------------------------------------------------*/
static double interpolate_double_value (int from, int to, 
					AnimTime current_time,
					AnimTime duration)
{
  return ((double)from + ((current_time / duration)  * (double)(to - from)));
}
/*----------------------------------------------------------------------
  ApplyStrokeColorToAllBoxes : Recursivly apply the property
  ----------------------------------------------------------------------*/
static void ApplyStrokeColorToAllBoxes (PtrAbstractBox pAb, int result)
{
 while (pAb != NULL)
    {      
      pAb->AbBox->VisibleModification = TRUE;
      pAb->AbForeground = result;
      ApplyStrokeColorToAllBoxes (pAb->AbFirstEnclosed, result);
      pAb = pAb->AbNext;
    }
}
/*----------------------------------------------------------------------
  ApplyFillColorToAllBoxes : Recursivly apply the property
  ----------------------------------------------------------------------*/
static void ApplyFillColorToAllBoxes (PtrAbstractBox pAb, int result)
{
 while (pAb != NULL)
    {      
      pAb->AbBox->VisibleModification = TRUE;
      pAb->AbBackground = result; 
      ApplyFillColorToAllBoxes (pAb->AbFirstEnclosed, result);
      pAb = pAb->AbNext;
    }
}
/*----------------------------------------------------------------------
  ApplyOpacityToAllBoxes : Recursivly apply the property
  ----------------------------------------------------------------------*/
static void ApplyOpacityToAllBoxes (PtrAbstractBox pAb, int result)
{
  while (pAb != NULL)
    {      
      pAb->AbBox->VisibleModification = TRUE;
      pAb->AbOpacity = result;
      ApplyOpacityToAllBoxes (pAb->AbFirstEnclosed, result);
      pAb = pAb->AbNext;
    }
}
/*----------------------------------------------------------------------
  ApplyXToAllBoxes  : Recursivly apply the property
  ----------------------------------------------------------------------*/
static void ApplyXToAllBoxes (PtrAbstractBox pAb, float result)
{
  while (pAb != NULL)
    {      
      pAb->AbBox->VisibleModification = TRUE;
      pAb->AbBox->BxXOrg = result;
      pAb->AbBox->BxClipX = result;  
      ApplyXToAllBoxes (pAb->AbFirstEnclosed, result);
      pAb = pAb->AbNext;
    }
}
/*----------------------------------------------------------------------
  ApplyYToAllBoxes : Recursivly apply the property
  ----------------------------------------------------------------------*/
static void ApplyYToAllBoxes (PtrAbstractBox pAb, float result)
{
  while (pAb != NULL)
    {      
      pAb->AbBox->VisibleModification = TRUE;
      pAb->AbBox->BxYOrg = result;
      pAb->AbBox->BxClipY = result;
      ApplyYToAllBoxes (pAb->AbFirstEnclosed, result);
      pAb = pAb->AbNext;
    }
}
/*----------------------------------------------------------------------
   : Recursivly apply the property
  ----------------------------------------------------------------------*/
static void ApplyWidthToAllBoxes (PtrAbstractBox pAb, float result)
{
  while (pAb != NULL)
    {      
      pAb->AbBox->VisibleModification = TRUE;
      pAb->AbBox->BxW = result;
      pAb->AbBox->BxClipW = result;     
      ApplyWidthToAllBoxes (pAb->AbFirstEnclosed, result);
      pAb = pAb->AbNext;
    }
}
/*----------------------------------------------------------------------
   : Recursivly apply the property
  ----------------------------------------------------------------------*/
static void ApplyHeightToAllBoxes (PtrAbstractBox pAb, float result)
{
  while (pAb != NULL)
    {      
      pAb->AbBox->VisibleModification = TRUE;
      pAb->AbBox->BxH = result;
      pAb->AbBox->BxClipH = result;
      ApplyHeightToAllBoxes (pAb->AbFirstEnclosed, result);
      pAb = pAb->AbNext;
    }
}
/*----------------------------------------------------------------------
  animate_box_animate : Animate any property of an element				
  ----------------------------------------------------------------------*/
static void animate_box_animate (PtrElement El,
				 Animated_Element *animated,
				 AnimTime current_time)
{
  int doc, view;
  PtrAbstractBox pAb = NULL;
  double result;
  
  if (animated->AttrName == NULL)
    return;
  
  if (strcasecmp (animated->AttrName, "opacity") == 0)
    {
      
      FrameToView (Animated_Frame, &doc, &view);
      pAb = El->ElAbstractBox[view - 1];
      if (pAb)
	if (pAb->AbFirstEnclosed)
	  {	  
	    
	    result = interpolate_double_value (atof ((char *) animated->from), 
					       atof ((char *) animated->to),
					       current_time,
					       animated->duration);
	    ApplyOpacityToAllBoxes (pAb->AbFirstEnclosed, (int) (result*1000));
	    UpdateClipping (pAb->AbFirstEnclosed);
	  }      
    }
  else if (strcasecmp (animated->AttrName, "x") == 0)
    {
      FrameToView (Animated_Frame, &doc, &view);
      pAb = El->ElAbstractBox[view - 1];
      if (pAb)
	if (pAb->AbFirstEnclosed)
	  {	  
	    
	    UpdateClipping (pAb->AbFirstEnclosed);

	    result = interpolate_double_value (atof ((char *) animated->from), 
					       atof ((char *) animated->to),
					       current_time,
					       animated->duration);
	    ApplyXToAllBoxes (pAb->AbFirstEnclosed, (float) result);
	   
	    UpdateClipping (pAb->AbFirstEnclosed);
	    
	  }
      
    }
  else if (strcasecmp (animated->AttrName, "y") == 0)
    {
      FrameToView (Animated_Frame, &doc, &view);
      pAb = El->ElAbstractBox[view - 1];
      if (pAb)
	if (pAb->AbFirstEnclosed)
	  {	  
	    
	    	    UpdateClipping (pAb->AbFirstEnclosed);
	    
	    result = interpolate_double_value (atof ((char *) animated->from), 
					       atof ((char *) animated->to),
					       current_time,
					       animated->duration);
	    ApplyYToAllBoxes (pAb->AbFirstEnclosed, (float) result);
	   
	    UpdateClipping (pAb->AbFirstEnclosed);
	    
	  }
      
    }
  else if (strcasecmp (animated->AttrName, "width") == 0)
    {
      FrameToView (Animated_Frame, &doc, &view);
      pAb = El->ElAbstractBox[view - 1];
      if (pAb)
	if (pAb->AbFirstEnclosed)
	  {	  
	    
	    UpdateClipping (pAb->AbFirstEnclosed);
	    
	    result = interpolate_double_value (atof ((char *) animated->from), 
					       atof ((char *) animated->to),
					       current_time,
					       animated->duration);
	    ApplyWidthToAllBoxes (pAb->AbFirstEnclosed, (float) result);
	   
	    UpdateClipping (pAb->AbFirstEnclosed);
	    
	  }
      
    }
else if (strcasecmp (animated->AttrName, "height") == 0)
    {
      FrameToView (Animated_Frame, &doc, &view);
      pAb = El->ElAbstractBox[view - 1];
      if (pAb)
	if (pAb->AbFirstEnclosed)
	  {	  
	    
	    UpdateClipping (pAb->AbFirstEnclosed);	    
	    result = interpolate_double_value (atof ((char *) animated->from), 
					       atof ((char *) animated->to),
					       current_time,
					       animated->duration);
	    ApplyHeightToAllBoxes (pAb->AbFirstEnclosed, (float) result);	   
	    UpdateClipping (pAb->AbFirstEnclosed);
	    
	  }
      
    }
}
/*----------------------------------------------------------------------
  
  ----------------------------------------------------------------------*/
static PtrTransform GetTransformation (PtrTransform Trans, 
				       TransformType _trans_type)
{
  ThotBool not_found;

  not_found = TRUE;
  while (not_found && Trans)
    {
      if (Trans->TransType != _trans_type)
	Trans = Trans->Next;
      else
	not_found = FALSE;      
    }
  return Trans;   
}
/*----------------------------------------------------------------------
  animate_box_transformation : animate the scale, translate, skew, rotate...
  ----------------------------------------------------------------------*/
static void animate_box_transformation (PtrElement El,
					Animated_Element *animated,
					AnimTime current_time)
{
  int              doc, view;
  float            fx, fy, tx, ty, rotf, rott;
  float            result;
  int              trans_type;
  PtrAbstractBox   pAb = NULL;
  PtrTransform     Trans = NULL;
  
  if (animated->AttrName == NULL)
    return;
  
  trans_type = *animated->AttrName;
  
  /*translate, scale, rotate_, skewX, skewY*/
  switch (trans_type)
    {
    case 1 : /*TRANSLATE*/
      FrameToView (Animated_Frame, &doc, &view);
      if (strstr ((char *) animated->to, ","))
	{
	  tx = atof ((char *) animated->to);
	  ty =  atof (strstr ((char *) animated->to, ",") + 1);
	}
      else
	tx = ty =  atof ((char *) animated->to);

      if (strstr ((char *) animated->from, ","))
	{
	  fx = atof ((char *) animated->from);
	  fy = atof (strstr ((char *) animated->from, ",") + 1);
	}
      else
	fx = fy =  atof ((char *) animated->from);

      tx = (float) interpolate_double_value (fx, 
					     tx,
					     current_time,
					     animated->duration);
      ty = (float) interpolate_double_value (fy, 
					     ty,
					     current_time,
					     animated->duration);

      if (El->ElTransform)
	Trans = GetTransformation (El->ElTransform, 
				   PtElTranslate); 
      
      if (Trans == NULL)
	{
	  Trans = TtaNewTransformTranslate (tx, ty, FALSE);
	  TtaReplaceTransform ((Element) El, Trans, doc); 
	  /* TtaAppendTransform ((Element) El, Trans, doc); */	  
	}
      pAb = El->ElAbstractBox[view - 1];
      if (pAb)
	if (pAb->AbFirstEnclosed)
	  {
	    UpdateClipping (pAb->AbFirstEnclosed);

	    Trans->XScale = tx;
	    Trans->YScale = ty;
	    /* Trans->Angle = (float) ty; */

	    ComputeBoundingBox (pAb->AbFirstEnclosed->AbBox, 
				Animated_Frame, 
				0, 4096, 
				0, 4096);

	    UpdateClipping (pAb->AbFirstEnclosed);
	  }
      
      break;

    case 2 : /*SCALE*/
      FrameToView (Animated_Frame, &doc, &view);

      if (strstr ((char *) animated->to, ","))
	{
	  tx = atof ((char *) animated->to);
	  ty =  atof (strstr ((char *) animated->to, ",") + 1);
	}
      else
	tx = ty =  atof ((char *) animated->to);

      if (strstr ((char *) animated->from, ","))
	{
	  fx = atof ((char *) animated->from);
	  fy = atof (strstr ((char *) animated->from, ",") + 1);
	}
      else
	fx = fy =  atof ((char *) animated->from);

      tx = (float) interpolate_double_value (fx, 
					     tx,
					     current_time,
					     animated->duration);
      ty = (float) interpolate_double_value (fy, 
					     ty,
					     current_time,
					     animated->duration);
      
      if (El->ElTransform)
	Trans = GetTransformation (El->ElTransform, 
				   PtElScale); 
      
      if (Trans == NULL)
	{
	  Trans = TtaNewTransformScale (tx, ty, FALSE);
	  TtaReplaceTransform ((Element) El, Trans, doc); 
	  /* TtaAppendTransform ((Element) El, Trans, doc);	   */
	}
      pAb = El->ElAbstractBox[view - 1];
      if (pAb)
	if (pAb->AbFirstEnclosed)
	  {
	    UpdateClipping (pAb->AbFirstEnclosed);
	    Trans->XScale = tx;
	    Trans->YScale = ty;
	    ComputeBoundingBox (pAb->AbFirstEnclosed->AbBox, 
				Animated_Frame, 
				0, FrameTable[Animated_Frame].FrWidth, 
				0, FrameTable[Animated_Frame].FrHeight);
	    UpdateClipping (pAb->AbFirstEnclosed);
	  }
      
      break;

    case 3 :/*Rotate*/
      FrameToView (Animated_Frame, &doc, &view);
      tx = ty = fx = fy = 0;
      if (strstr ((char *) animated->to, ","))
	{
	  rott = atof ((char *) animated->to);
	  if (strstr (strstr ((char *) animated->to, ",") + 1, ","))
	    {
	      tx =  atof (strstr ((char *) animated->to, ",") + 1);
	      ty =  atof (strstr (strstr ((char *) animated->to, ",") + 1, ",") + 1);
	    }
	  else
	    tx = ty = atof (strstr ((char *) animated->to, ",") + 1);
	}
      else
	rott =  atof ((char *) animated->to);
      if (strstr ((char *) animated->from, ","))
	{
	  rotf = atof ((char *) animated->from);
	  if (strstr (strstr ((char *) animated->from, ",") + 1, ","))
	    {
	      fx =  atof (strstr ((char *) animated->from, ",") + 1);
	      fy =  atof (strstr (strstr ((char *) animated->from, ",") + 1, ",") + 1);
	    }
	  else
	    fx = fy = atof (strstr ((char *) animated->from, ",") + 1);
	}
      else
	rotf =  atof ((char *) animated->from);
      if (tx && fx)
	tx = (float) interpolate_double_value (fx, 
					       tx,
					       current_time,
					       animated->duration);
      if (ty && fy)
	ty = (float) interpolate_double_value (fy, 
					       ty,
					       current_time,
					       animated->duration);
      rott = (float)interpolate_double_value (rotf, 
					      rott,
					      current_time,
					      animated->duration);      
      if (El->ElTransform)
	Trans = GetTransformation (El->ElTransform, 
				   PtElRotate);       
      if (Trans == NULL)
	{
	  Trans = TtaNewTransformRotate (rott, tx, ty);
	  TtaReplaceTransform ((Element) El, Trans, doc); 
	  /* TtaAppendTransform ((Element) El, Trans, doc);	  */ 
	}
      pAb = El->ElAbstractBox[view - 1];
      if (pAb)
	if (pAb->AbFirstEnclosed)
	  {
	    UpdateClipping (pAb->AbFirstEnclosed);	    
	    Trans->Angle = rott;
	    Trans->XRotate = tx;
	    Trans->YRotate = ty;
	    ComputeBoundingBox (pAb->AbFirstEnclosed->AbBox, 
				Animated_Frame, 
				0, FrameTable[Animated_Frame].FrWidth, 
				0, FrameTable[Animated_Frame].FrHeight);
	    UpdateClipping (pAb->AbFirstEnclosed);
	  }
      
      break;
      
    case 4 :/*skewX*/
      FrameToView (Animated_Frame, &doc, &view);
      result = (float)interpolate_double_value (atof ((char *) animated->from), 
						atof ((char *) animated->to),
						current_time,
						animated->duration);      
      if (El->ElTransform)
	Trans = GetTransformation (El->ElTransform, 
				   PtElSkewX);       
      if (Trans == NULL)
	{
	  Trans = TtaNewTransformSkewX (result);
	  TtaReplaceTransform ((Element) El, Trans, doc); 
	  /* TtaAppendTransform ((Element) El, Trans, doc);	   */
	}
      pAb = El->ElAbstractBox[view - 1];
      if (pAb)
	if (pAb->AbFirstEnclosed)
	  {
	    UpdateClipping (pAb->AbFirstEnclosed);	    
	    Trans->Factor = result;
	    ComputeBoundingBox (pAb->AbFirstEnclosed->AbBox, 
				Animated_Frame, 
				0, FrameTable[Animated_Frame].FrWidth, 
				0, FrameTable[Animated_Frame].FrHeight);
	    UpdateClipping (pAb->AbFirstEnclosed);
	  }      
      break;
     
    case 5 :/*SKEWY*/
      FrameToView (Animated_Frame, &doc, &view);
      result = (float)interpolate_double_value (atof ((char *) animated->from), 
						atof ((char *) animated->to),
						current_time,
						animated->duration);      
      if (El->ElTransform)
	Trans = GetTransformation (El->ElTransform, 
				   PtElSkewY);       
      if (Trans == NULL)
	{
	  Trans = TtaNewTransformSkewY (result);
	  TtaReplaceTransform ((Element) El, Trans, doc); 
	  /* TtaAppendTransform ((Element) El, Trans, doc);	   */
	}
      pAb = El->ElAbstractBox[view - 1];
      if (pAb)
	if (pAb->AbFirstEnclosed)
	  {
	    	    UpdateClipping (pAb->AbFirstEnclosed);
	    Trans->Factor = result;
	    ComputeBoundingBox (pAb->AbFirstEnclosed->AbBox, 
				Animated_Frame, 
				0, FrameTable[Animated_Frame].FrWidth, 
				0, FrameTable[Animated_Frame].FrHeight);
	    UpdateClipping (pAb->AbFirstEnclosed);
	  }
      
      break;
    }  
}
/*----------------------------------------------------------------------
  animate_box_color : Animate the color propoerty
  ----------------------------------------------------------------------*/
static void animate_box_color (PtrElement El,
			       Animated_Element *animated,
			       AnimTime current_time)
{
  int doc, view;
  PtrAbstractBox pAb = NULL;
  unsigned short fromred, fromgreen, fromblue, 
    tored, togreen, toblue, 
    resultred, resultgreen, resultblue;
  double         proportion;
  int            result;

  proportion = current_time / animated->duration;
  TtaGiveRGB ((char *) animated->from, &fromred, &fromgreen, &fromblue);
  TtaGiveRGB ((char *) animated->to, &tored, &togreen, &toblue);
  resultred = (unsigned short) (fromred + proportion * (tored - fromred));
  resultgreen = (unsigned short) (fromgreen + proportion * (togreen - fromgreen));
  resultblue = (unsigned short) (fromblue + proportion * (toblue - fromblue));
  result = TtaGetThotColor (resultred, resultgreen, resultblue);
  FrameToView (Animated_Frame, &doc, &view);
  pAb = El->ElAbstractBox[view - 1];
  if (pAb)
    if (pAb->AbFirstEnclosed)
      {	    	
	if (strcasecmp (animated->AttrName, "fill") == 0)
	  ApplyFillColorToAllBoxes (pAb->AbFirstEnclosed, result);
	else if (strcasecmp (animated->AttrName, "stroke") == 0)
	  ApplyStrokeColorToAllBoxes (pAb->AbFirstEnclosed, result);	   
	UpdateClipping (pAb->AbFirstEnclosed);
      }
}
/*----------------------------------------------------------------------
  animate_box_motion : Animate the position of a box using a path
  ----------------------------------------------------------------------*/
static void animate_box_motion (PtrElement El,
			       Animated_Element *animated,
			       AnimTime current_time)
{
  /*AnimPath \ {path, pts_prop}*/
  /*Get path->height = path length*/
    /* Get prop_point = pos_point/lenght for each point*/
  /*interpolate prop upon time*/
  /* get propx1 < prop > propx2 in pts_prop array*/

  /*
    pts_prop = (pts_prop - pts_prop1) / (pts_prop2 - pts_prop1);
    x = x1 + (pts_prop*(x2 -x1));
    y = y1 + (pts_prop*(y2 -y1));
    applyX
    applyY
  */
}
/*----------------------------------------------------------------------
  is_animated_now : Compute if animation appply for this box upon time
  ----------------------------------------------------------------------*/
static ThotBool is_animated_now (Animated_Element *animated, AnimTime *current_time)
{
  if (*current_time > animated->start) 
    {
      *current_time = *current_time - animated->start;
      if (*current_time < animated->duration)
	return TRUE;
      else if (animated->repeatCount > 1)
	{
	  if (animated->repeatCount > ((int)(*current_time/animated->duration)))
	    *current_time = fmod (*current_time, animated->duration);
	    return TRUE;
	}
      else
	{
	  switch (animated->Fill)
	    {
	    case Freeze:
	      *current_time = animated->duration;
	      return TRUE;
	    case Otherfill:
	      *current_time = animated->start;
	      return TRUE;
	    default:
	      *current_time = animated->start;
	      return TRUE;
	    }
	}
    }
  return FALSE;
}
/*----------------------------------------------------------------------
  animate_box : animate a a box using all animation that apply on him
  ----------------------------------------------------------------------*/
static void animate_box (PtrElement El,
			 AnimTime current_time)
{
  Animated_Element *animated = NULL;
  AnimTime          rel_time;

  if (El)
    if (El->animation)
      {      
	animated = (Animated_Element *) El->animation;
	while (animated)
	  {
	    rel_time = current_time;
	    if (is_animated_now(animated, &rel_time))
	      {
		switch (animated->AnimType)
		  {
		  case Color:
		    if (animated->from && animated->to)
		      animate_box_color (El, animated, rel_time);
		    break;
		  
		  case Transformation:
		    if (animated->from && animated->to)
		      animate_box_transformation (El, animated, rel_time);
		    break;
		  
		  case Motion:
		    if (animated->from && animated->to)
		      animate_box_motion (El, animated, rel_time);
		    break;
		  
		  case Animate:
		    if (animated->from && animated->to)
		      animate_box_animate (El, animated, rel_time);
		    break;
		  
		  case Set:
		    break;
		  
		  case OtherAnim:
		    break;
		  
		  default:
		    break;      
		  }
	      }
	    animated = animated->next;	      
	  }
      }
    
}
#endif /* _GL */
/*----------------------------------------------------------------------
  AnimatedBoxAdd : Add a reference to animated element in the frame
  ----------------------------------------------------------------------*/
void AnimatedBoxAdd (PtrElement element)
{
#ifdef _GL 
  Animated_Cell *current;

  if (FrameTable[ActiveFrame].Animated_Boxes == NULL)
    {
      FrameTable[ActiveFrame].Animated_Boxes = TtaGetMemory (sizeof(Animated_Cell));
      current = (Animated_Cell *) FrameTable[ActiveFrame].Animated_Boxes;
    }
  else
    {
      current = (Animated_Cell *) FrameTable[ActiveFrame].Animated_Boxes;
      while (current->Next)
	{
	  if (current->El == element)
	    return;
	  current = current->Next;
	}
      
      current->Next = TtaGetMemory (sizeof(Animated_Cell));
      current = current->Next;
    }
  current->Next = NULL;
  current->El = element;
  /* current->El = element->ElParent; */
#endif /* _GL */
}
/*----------------------------------------------------------------------
  AnimatedBoxDel : Delete a reference to an animated element
  ----------------------------------------------------------------------*/
void AnimatedBoxDel (PtrElement element)
{
#ifdef _GL 
  Animated_Cell *current, *previous;
  ThotBool not_found = TRUE;
  
  if (element->animation)
    {
      if (FrameTable[ActiveFrame].Animated_Boxes != NULL)
	{
	  previous = NULL;
	  current = (Animated_Cell *) FrameTable[ActiveFrame].Animated_Boxes;
	  while (current && not_found)
	    {
	      if (current->El == element)
		not_found = FALSE;
	      else
		{
		  previous = current;	  
		  current = current->Next;
		}	      
	    }
	  
	  if (not_found)
	    return;

	  if (previous)
	    {	      
	      if (previous && current && current->Next)
		previous->Next = current->Next;
	      else
		previous->Next = NULL;
	    }	  
	  else
	    FrameTable[ActiveFrame].Animated_Boxes = NULL;
	  TtaFreeMemory (current);
	}
    }
#endif /* _GL */
}
/*----------------------------------------------------------------------
  FreeAnimatedBox : Free Allocated resources
  ----------------------------------------------------------------------*/
void FreeAnimatedBox (Animated_Cell *current)
{
#ifdef _GL 
  if (current->Next)
    FreeAnimatedBox (current->Next);
  TtaFreeMemory (current);  
#endif /* _GL */
}
/*----------------------------------------------------------------------
  Animate_boxes : Animate All animated boxe 
and define region that need redisplay
  ----------------------------------------------------------------------*/
void Animate_boxes (int frame, 
		    AnimTime current_time)
{
#ifdef _GL 
  Animated_Cell *current = FrameTable[frame].Animated_Boxes;

  /* Time update
     RefreshAnimBoxes ()
     Get Clipping Box ()
     DefFrame ()
     RedrawFrameBottom ()   */ 
  Animated_Frame = frame;
  
  SetBaseClipping ();
  
  while (current)
    {
      animate_box (current->El, current_time);
      current = current->Next;
    }
  DefRegion (frame, Clipx, Clipy, ClipxMax, ClipyMax);
  FrameTable[frame].DblBuffNeedSwap = TRUE;
#endif /* _GL */
}
