/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2003
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

#ifdef _GLANIM
static int Animated_Frame = 0;
static int Clipx, Clipy, ClipxMax, ClipyMax;

#define Min(number, min) (number = ( (number < 0) ? min : ((number < min) ? number : min)) )
#define Max(number, max) (number = ( (number < 0) ? max : ((number > max) ? number : max)) )

static void SetBaseClipping ()
{
  Clipx = -1;
  Clipy = -1;
  ClipyMax = -1;
  ClipxMax = -1;
}

static void UpdateClipping (PtrAbstractBox pAb)
{
  while (pAb != NULL)
    {  
      if (!(pAb->AbBox->BxClipH || pAb->AbBox->BxClipW))
	ComputeBoundingBox (pAb->AbBox,
			    Animated_Frame,
			    0, 4096,
			    0, 4096);

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
      else
	UpdateClipping (pAb->AbFirstEnclosed);
      
      pAb = pAb->AbNext;
    }
}

static double interpolate_double_value (int from, int to, 
					AnimTime current_time,
					AnimTime duration)
{
  return (from + ((fmod (current_time, duration) * (to - from)) / duration));
}

static void ApplyOpacityToAllBoxes (PtrAbstractBox pAb, int result)
{
  while (pAb != NULL)
    {      
      pAb->AbOpacity = result;
      ApplyOpacityToAllBoxes (pAb->AbFirstEnclosed, result);
      pAb = pAb->AbNext;
    }
}

static void ApplyXToAllBoxes (PtrAbstractBox pAb, float result)
{
  while (pAb != NULL)
    {      
      pAb->AbBox->BxXOrg = result;
      pAb->AbBox->BxClipX = result;
      pAb->AbHorizPosChange = TRUE;      
      ApplyXToAllBoxes (pAb->AbFirstEnclosed, result);
      pAb = pAb->AbNext;
    }
}
static void ApplyYToAllBoxes (PtrAbstractBox pAb, float result)
{
  while (pAb != NULL)
    {      
      pAb->AbBox->BxYOrg = result;
      pAb->AbBox->BxClipY = result;
      pAb->AbVertPosChange = TRUE;
      ApplyYToAllBoxes (pAb->AbFirstEnclosed, result);
      pAb = pAb->AbNext;
    }
}
static void animate_box_animate (PtrElement El,
				 Animated_Element *animated,
				 AnimTime current_time)
{
  int doc, view;
  PtrAbstractBox pAb = NULL;
  double result;
  
  if (animated->AttrName == NULL)
    return;
  
  if (strcmp (animated->AttrName, "opacity") == 0)
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
  else if (strcmp (animated->AttrName, "x") == 0)
    {
      FrameToView (Animated_Frame, &doc, &view);
      pAb = El->ElAbstractBox[view - 1];
      if (pAb)
	if (pAb->AbFirstEnclosed)
	  {	  
	    
	    /* UpdateClipping (pAb->AbFirstEnclosed); */

	    result = interpolate_double_value (atof ((char *) animated->from), 
					       atof ((char *) animated->to),
					       current_time,
					       animated->duration);
	    ApplyXToAllBoxes (pAb->AbFirstEnclosed, (float) result);
	   
	    UpdateClipping (pAb->AbFirstEnclosed);
	    
	  }
      
    }
  else if (strcmp (animated->AttrName, "y") == 0)
    {
      FrameToView (Animated_Frame, &doc, &view);
      pAb = El->ElAbstractBox[view - 1];
      if (pAb)
	if (pAb->AbFirstEnclosed)
	  {	  
	    
	    /* 	    UpdateClipping (pAb->AbFirstEnclosed); */
	    
	    result = interpolate_double_value (atof ((char *) animated->from), 
					       atof ((char *) animated->to),
					       current_time,
					       animated->duration);
	    ApplyYToAllBoxes (pAb->AbFirstEnclosed, (float) result);
	   
	    UpdateClipping (pAb->AbFirstEnclosed);
	    
	  }
      
    }
}

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

static void animate_box_transformation (PtrElement El,
					Animated_Element *animated,
					AnimTime current_time)
{
  int doc, view;
  float result;
  int trans_type;
  PtrAbstractBox pAb = NULL;
  PtrTransform Trans = NULL;
  
  
  if (animated->AttrName == NULL)
    return;
  
  trans_type = *animated->AttrName;
  
  /*translate, scale, rotate_, skewX, skewY*/
  switch (trans_type)
    {
    case 1 : /*TRANSLATE*/
      FrameToView (Animated_Frame, &doc, &view);
      result = (float) interpolate_double_value (atof ((char *) animated->from), 
						 atof ((char *) animated->to),
						 current_time,
						 animated->duration);
      if (El->ElTransform)
	Trans = GetTransformation (El->ElTransform, 
				   PtElTranslate); 
      
      if (Trans == NULL)
	{
	  Trans = TtaNewTransformTranslate (result, result, FALSE);
	  TtaAppendTransform ((Element) El, Trans, doc);	  
	}
      pAb = El->ElAbstractBox[view - 1];
      if (pAb)
	if (pAb->AbFirstEnclosed)
	  {
	    /* 	    UpdateClipping (pAb->AbFirstEnclosed); */

	    Trans->XScale = result;
	    Trans->YScale = result;
	    Trans->Angle = (float) result;

	    ComputeBoundingBox (pAb->AbFirstEnclosed->AbBox, 
				Animated_Frame, 
				0, 4096, 
				0, 4096);

	    UpdateClipping (pAb->AbFirstEnclosed);
	  }
      
      break;

    case 2 : /*SCALE*/
      FrameToView (Animated_Frame, &doc, &view);
      result = (float)interpolate_double_value (atof ((char *) animated->from), 
						atof ((char *) animated->to),
						current_time,
						animated->duration);
      
      if (El->ElTransform)
	Trans = GetTransformation (El->ElTransform, 
				   PtElScale); 
      
      if (Trans == NULL)
	{
	  Trans = TtaNewTransformScale (result, result, FALSE);
	  TtaAppendTransform ((Element) El, Trans, doc);	  
	}
      pAb = El->ElAbstractBox[view - 1];
      if (pAb)
	if (pAb->AbFirstEnclosed)
	  {
	    /* 	    UpdateClipping (pAb->AbFirstEnclosed); */
	    Trans->XScale = result;
	    Trans->YScale = result;
	    ComputeBoundingBox (pAb->AbFirstEnclosed->AbBox, 
				Animated_Frame, 
				0, FrameTable[Animated_Frame].FrWidth, 
				0, FrameTable[Animated_Frame].FrHeight);
	    UpdateClipping (pAb->AbFirstEnclosed);
	  }
      
      break;

    case 3 :/*Rotate*/
      FrameToView (Animated_Frame, &doc, &view);
      result = (float)interpolate_double_value (atof ((char *) animated->from), 
						atof ((char *) animated->to),
						current_time,
						animated->duration);
      
      if (El->ElTransform)
	Trans = GetTransformation (El->ElTransform, 
				   PtElRotate); 
      
      if (Trans == NULL)
	{
	  Trans = TtaNewTransformRotate (result, 0, 0);
	  TtaAppendTransform ((Element) El, Trans, doc);	  
	}
      pAb = El->ElAbstractBox[view - 1];
      if (pAb)
	if (pAb->AbFirstEnclosed)
	  {
	    /* 	    UpdateClipping (pAb->AbFirstEnclosed); */
	    
	    Trans->Angle = result;
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
	  TtaAppendTransform ((Element) El, Trans, doc);	  
	}
      pAb = El->ElAbstractBox[view - 1];
      if (pAb)
	if (pAb->AbFirstEnclosed)
	  {
	    /* 	    UpdateClipping (pAb->AbFirstEnclosed); */
	    
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
	  TtaAppendTransform ((Element) El, Trans, doc);	  
	}
      pAb = El->ElAbstractBox[view - 1];
      if (pAb)
	if (pAb->AbFirstEnclosed)
	  {
	    /* 	    UpdateClipping (pAb->AbFirstEnclosed); */
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


static void animate_box_color ()
{

}

static void animate_box_motion ()
{

}
static ThotBool is_animated_now (Animated_Element *animated, AnimTime current_time)
{
  if (current_time > animated->start) 
    /* && */
    /*       current_time < animated->start + animated->duration) */
    {
      return TRUE;
    }
  else
    return FALSE;
}


static void animate_box (PtrElement El,
			 AnimTime current_time)
{
  Animated_Element *animated = NULL;
 
  if (El)
    if (El->animation)
      {      
	animated = (Animated_Element *) El->animation;
	while (animated)
	  {
	      
	    if (is_animated_now(animated, current_time))
	      {
		switch (animated->AnimType)
		  {
		  case Color:
		    animate_box_color ();
		    break;
		  
		  case Transformation:
		    animate_box_transformation (El, animated, current_time);
		    break;
		  
		  case Motion:
		    animate_box_motion ();
		    break;
		  
		  case Set:
		    break;
		  
		  case Animate:
		    animate_box_animate (El, animated, current_time);
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
#endif /* _GLANIM */


void AnimatedBoxAdd (PtrElement element)
{
#ifdef _GLANIM 
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
#endif /* _GLANIM */
}

void AnimatedBoxDel (PtrElement element)
{
#ifdef _GLANIM 
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
#endif /* _GLANIM */
}

void FreeAnimatedBox (Animated_Cell *current)
{
#ifdef _GLANIM 
  if (current->Next)
    FreeAnimatedBox (current->Next);
  TtaFreeMemory (current);  
#endif /* _GLANIM */
}



void Animate_boxes (int frame, 
		    AnimTime current_time)
{
#ifdef _GLANIM 
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
#endif /* _GLANIM */
}
