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
#include "font_f.h"
#include "boxlocate_f.h"

#ifdef _GL
#include "glwindowdisplay.h"
#endif /* _GL */
#include <math.h>

#ifdef ARCTEST
#define A 0.60310579
#define B 0.05160454 
#define C 0.55913709
#define D 1.4087812
#define ARCTAN(X) ((A - B*X^2 + C/(X^2 + D))*X)
#else /* ARCTEST */
#define ARCTAN(X) (atan(X))
#endif /* ARCTEST */

#define ALLOC_POINTS 50

#ifdef _GL
static int Animated_Frame = 0;
static int Clipx, Clipy, ClipxMax, ClipyMax;

#define Min(number, min) (number = ( (number < 0) ? min : (((number - min) < 0.00001) ? number : min)) )
#define Max(number, max) (number = ( (number < 0) ? max : (((number - max) > 0.00001) ? number : max)) )
#endif /* _GL */

/*----------------------------------------------------------------------
  populate_path_proportion : interpolate proportion of each point over
  total path length

 PtrPathSeg      FirstPathSeg; linked list of segment defining the path
  float           length;      total length 
  ThotPath        *Path;       The Path
  float           *Proportion; per segment % of total length

  ----------------------------------------------------------------------*/
void populate_path_proportion (void *anim_info)
{
#ifdef _GL
  int        i, npoints = 0;
  int        x,y;
  float      totallength;
  int        *subpathStart = NULL;
  ThotPoint  *points;
  AnimPath   *pop_path;
  float      *proportion, *angle;
  Animated_Element *animated;

  animated = (Animated_Element *) (anim_info);
  pop_path = (AnimPath *) animated->from;
  points = (ThotPoint *) TtaGetMemory (ALLOC_POINTS * sizeof(ThotPoint));
  points = BuildPolygonForPath (((PtrPathSeg) (pop_path->FirstPathSeg)), 
				Animated_Frame,
				&npoints, 
				&subpathStart);
  proportion = (float *) TtaGetMemory (npoints * sizeof(float));
  angle = (float *) TtaGetMemory (npoints * sizeof(float));
  totallength = 0;
  proportion[0] = 0;
  angle[0] = 0;
  for (i = 1; i < npoints; i++)
    {
      x = points[i].x - points[i-1].x;
      y = points[i].y - points[i-1].y;

      if (x != 0)
	angle[i] = ARCTAN (y/x);
      else
	angle[i] = 90;
      totallength +=  sqrt ((double) x*x + y*y);
      proportion[i] = totallength;
    }
  for (i = 1; i < npoints; i++)
    {
      proportion[i] = proportion[i] / totallength;
    }
  pop_path->Proportion = proportion;
  pop_path->length = totallength;
  pop_path->npoints = npoints;
  pop_path->Path = points;
#endif /* _GL */
}
/*----------------------------------------------------------------------
  populate_values_proportion : interpolate proportion of each point over
  total path length

 PtrPathSeg      FirstPathSeg; linked list of values list
  float           length;      total length 
  ThotPath        *Path;       The Path
  float           *Proportion; per segment % of total length

  ----------------------------------------------------------------------*/
void populate_values_proportion (void *anim_info)
{
#ifdef _GL
  int        i, npoints = 0;
  int        x,y;
  float      totallength;
  int        *subpathStart = NULL;
  ThotPoint  *points;
  AnimPath   *pop_path;
  float      *proportion;
  Animated_Element *animated;

  animated = (Animated_Element *) (anim_info);
  pop_path = (AnimPath *) animated->from;
  points = (ThotPoint *) TtaGetMemory (ALLOC_POINTS * sizeof(ThotPoint));
  points = BuildPolygonForPath (((PtrPathSeg) (pop_path->FirstPathSeg)), 
				Animated_Frame,
				&npoints, 
				&subpathStart);
  proportion = (float *) TtaGetMemory (npoints * sizeof(float));
  totallength = 0;
  proportion[0] = 0;
  for (i = 1; i < npoints; i++)
    {
      x = points[i].x - points[i-1].x;
      y = points[i].y - points[i-1].y;

      totallength +=  sqrt ((double) x*x + y*y);
      proportion[i] = totallength;
    }
  for (i = 1; i < npoints; i++)
    {
      proportion[i] = proportion[i] / totallength;
    }
  pop_path->Proportion = proportion;
  pop_path->length = totallength;
  pop_path->npoints = npoints;
  pop_path->Path = points;
#endif /* _GL */
}
/*----------------------------------------------------------------------
  populate_fromto_proportion : interpolate proportion of each point over
  total values list

 PtrPathSeg      FirstPathSeg; linked list of values
  float           length;      total length 
  ThotPath        *Path;       The Path
  float           *Proportion; per segment % of total length

  ----------------------------------------------------------------------*/
void populate_fromto_proportion (void *anim_info)
{
#ifdef _GL
  int        i, npoints = 0;
  int        x,y;
  float      totallength;
  int        *subpathStart = NULL;
  ThotPoint  *points;
  AnimPath   *pop_path;
  float      *proportion;
  Animated_Element *animated;

  animated = (Animated_Element *) (anim_info);
  pop_path = (AnimPath *) animated->from;
  points = (ThotPoint *) TtaGetMemory (ALLOC_POINTS * sizeof(ThotPoint));
  points = BuildPolygonForPath (((PtrPathSeg) (pop_path->FirstPathSeg)), 
				Animated_Frame,
				&npoints, 
				&subpathStart);
  proportion = (float *) TtaGetMemory (npoints * sizeof(float));
  totallength = 0;
  proportion[0] = 0;
  for (i = 1; i < npoints; i++)
    {
      x = points[i].x - points[i-1].x;
      y = points[i].y - points[i-1].y;

      totallength +=  sqrt ((double) x*x + y*y);
      proportion[i] = totallength;
    }
  for (i = 1; i < npoints; i++)
    {
      proportion[i] = proportion[i] / totallength;
    }
  pop_path->Proportion = proportion;
  pop_path->length = totallength;
  pop_path->npoints = npoints;
  pop_path->Path = points;
#endif /* _GL */
}


#ifdef _GL
/*----------------------------------------------------------------------
  GetTransformation : Get A transformation by its type in the linked list 
  of transformatin applied on a element 
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

  Min (Clipx, pAb->AbEnclosing->AbBox->BxClipX);		    
  Max (ClipxMax, 
       (pAb->AbEnclosing->AbBox->BxClipX + pAb->AbEnclosing->AbBox->BxClipW));

  Min (Clipy, pAb->AbEnclosing->AbBox->BxClipY);		    
  Max (ClipyMax, 
       (pAb->AbEnclosing->AbBox->BxClipY + pAb->AbEnclosing->AbBox->BxClipH));
}
/*----------------------------------------------------------------------
  interpolate_double_value : Compute a the value corresponding to a time
  using the "regle de trois" ("rule of tree")
  ----------------------------------------------------------------------*/
static double interpolate_double_value (float from, float to, 
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

      /* test if it's a group*/
      if (! TypeHasException (ExcIsGroup, pAb->AbElement->ElTypeNumber,
		       pAb->AbElement->ElStructSchema))
	ApplyOpacityToAllBoxes (pAb->AbFirstEnclosed, result);
      pAb = pAb->AbNext;
    }
}
/*----------------------------------------------------------------------
  ApplyXToAllBoxes  : Recursivly apply the property
  ----------------------------------------------------------------------*/
static void ApplyXToAllBoxes (PtrAbstractBox pAb, float result)
{
  PtrBox        pBox;
  PtrElement    El;
  PtrTransform  Trans;
  int           doc, view;

  while (pAb != NULL)
    {   
      pBox = pAb->AbBox;
      pBox->VisibleModification = TRUE;
      if (pAb->AbElement->ElSystemOrigin)
	{
	  El = pAb->AbElement;
	  if (El->ElTransform)
	    Trans = GetTransformation (El->ElTransform, 
				       PtElTranslate); 	  
	  if (Trans == NULL)
	    {
	      Trans = TtaNewTransformTranslate (result, 0, FALSE);
	      FrameToView (Animated_Frame, &doc, &view);
	      TtaReplaceTransform ((Element) El, Trans, doc); 
	    }  
	  Trans->XScale = result;
	}
      else
	{
	  pBox->BxXOrg = result;
	  ApplyXToAllBoxes (pAb->AbFirstEnclosed, result);
	}
      pAb = pAb->AbNext;
    }
}
/*----------------------------------------------------------------------
  ApplyYToAllBoxes : Recursivly apply the property
  ----------------------------------------------------------------------*/
static void ApplyYToAllBoxes (PtrAbstractBox pAb, float result)
{
  PtrBox        pBox;
  PtrElement    El;
  PtrTransform  Trans;
  int           doc, view;

  while (pAb != NULL)
    {   
      pBox = pAb->AbBox;
      pBox->VisibleModification = TRUE;
      if (pAb->AbElement->ElSystemOrigin)
	{
	  El = pAb->AbElement;
	  if (El->ElTransform)
	    Trans = GetTransformation (El->ElTransform, 
				       PtElTranslate); 	  
	  if (Trans == NULL)
	    {
	      Trans = TtaNewTransformTranslate (0, result, FALSE);
	      FrameToView (Animated_Frame, &doc, &view);
	      TtaReplaceTransform ((Element) El, Trans, doc); 
	    }  
	  Trans->YScale = result;
	}
      else
	{
	  if (pAb->AbLeafType == LtText)
	    result -= BoxFontBase (pBox->BxFont);
	  pBox->BxYOrg = result;
	  ApplyYToAllBoxes (pAb->AbFirstEnclosed, result);
	}
      pAb = pAb->AbNext;
    }
}
/*----------------------------------------------------------------------
  ApplyWidthToAllBoxes : Recursivly apply the property
  ----------------------------------------------------------------------*/
static void ApplyWidthToAllBoxes (PtrAbstractBox pAb, float result)
{
  while (pAb != NULL)
    {      
      pAb->AbBox->VisibleModification = TRUE;
      pAb->AbBox->BxW = result;     
      ApplyWidthToAllBoxes (pAb->AbFirstEnclosed, result);
      pAb = pAb->AbNext;
    }
}
/*----------------------------------------------------------------------
  ApplyHeightToAllBoxes : Recursivly apply the property
  ----------------------------------------------------------------------*/
static void ApplyHeightToAllBoxes (PtrAbstractBox pAb, float result)
{
  while (pAb != NULL)
    {      
      pAb->AbBox->VisibleModification = TRUE;
      pAb->AbBox->BxH = result;
      ApplyHeightToAllBoxes (pAb->AbFirstEnclosed, result);
      pAb = pAb->AbNext;
    }
}
/*----------------------------------------------------------------------
   : Recursivly apply the property
  ----------------------------------------------------------------------*/
static void ApplyFontSizeToAllBoxes (PtrAbstractBox pAb, int result)
{

  while (pAb != NULL)
    {      
      pAb->AbBox->VisibleModification = TRUE;
      ChangeFontsetSize ((int) result, pAb->AbBox, Animated_Frame);
      ApplyFontSizeToAllBoxes (pAb->AbFirstEnclosed, result);
      pAb = pAb->AbNext;
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
  TtaGiveRGB ((char *) animated->to, &tored, &togreen, &toblue);
  if (animated->from)
    {
      TtaGiveRGB ((char *) animated->from, &fromred, &fromgreen, &fromblue);

      resultred = (unsigned short) (fromred + proportion * (tored - fromred));
      resultgreen = (unsigned short) (fromgreen + proportion * (togreen - fromgreen));
      resultblue = (unsigned short) (fromblue + proportion * (toblue - fromblue));
      result = TtaGetThotColor (resultred, resultgreen, resultblue);
    }
  else
    result = TtaGetThotColor (tored, togreen, toblue);
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
  animate_box_animate : Animate any property of an element				
  ----------------------------------------------------------------------*/
static void animate_box_set (PtrElement El,
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
	    result = 1000*atof ((char *) animated->to);
	    pAb->AbOpacity = result;
	    /*If it's an opaque group manage the opacity*/
	    if (!TypeHasException (ExcIsGroup, pAb->AbElement->ElTypeNumber,
				  pAb->AbElement->ElStructSchema))
	      ApplyOpacityToAllBoxes (pAb->AbFirstEnclosed, (int) (result));
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
	    result = atof ((char *) animated->to);
	    ApplyXToAllBoxes (pAb->AbFirstEnclosed, (float) result);
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
	    result = atof ((char *) animated->to);
	    ApplyYToAllBoxes (pAb->AbFirstEnclosed, (float) result);
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
	    result = atof ((char *) animated->to);
	    ApplyWidthToAllBoxes (pAb->AbFirstEnclosed, (float) result);
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
	    result = atof ((char *) animated->to);
	    ApplyHeightToAllBoxes (pAb->AbFirstEnclosed, (float) result);	   
	  }
      
    }
  else if (strcasecmp (animated->AttrName, "font-size") == 0)
    {
      FrameToView (Animated_Frame, &doc, &view);
      pAb = El->ElAbstractBox[view - 1];
      if (pAb)
	if (pAb->AbFirstEnclosed)
	  {	  
	    UpdateClipping (pAb->AbFirstEnclosed);	    
	    result = atof ((char *) animated->to);
	  }
      
    }
  else if (strcasecmp (animated->AttrName, "fill") == 0 || 
	   strcasecmp (animated->AttrName, "stroke") == 0)
    {
      animate_box_color (El, animated, current_time);    
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
	    result = 1000 * interpolate_double_value (atof ((char *) animated->from), 
						      atof ((char *) animated->to),
						      current_time,
						      animated->duration);

	    pAb->AbOpacity = result;
	    /*If it's an opaque group manage the opacity*/
	    if (!TypeHasException (ExcIsGroup, pAb->AbElement->ElTypeNumber,
				  pAb->AbElement->ElStructSchema))
	      ApplyOpacityToAllBoxes (pAb->AbFirstEnclosed, (int) (result));
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
	  }
      
    }
  else if (strcasecmp (animated->AttrName, "font-size") == 0)
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
	  }
      
    }
  else if (strcasecmp (animated->AttrName, "fill") == 0 || 
	   strcasecmp (animated->AttrName, "stroke") == 0)
    {
      animate_box_color (El, animated, current_time);    
    }
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
	}
      pAb = El->ElAbstractBox[view - 1];
      if (pAb)
	if (pAb->AbFirstEnclosed)
	  {
	    UpdateClipping (pAb->AbFirstEnclosed);

	    Trans->XScale = tx;
	    Trans->YScale = ty;
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
	}
      pAb = El->ElAbstractBox[view - 1];
      if (pAb)
	if (pAb->AbFirstEnclosed)
	  {
	    UpdateClipping (pAb->AbFirstEnclosed);
	    Trans->XScale = tx;
	    Trans->YScale = ty;
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
	}
      pAb = El->ElAbstractBox[view - 1];
      if (pAb)
	if (pAb->AbFirstEnclosed)
	  {
	    UpdateClipping (pAb->AbFirstEnclosed);	    
	    Trans->Angle = rott;
	    Trans->XRotate = tx;
	    Trans->YRotate = ty;
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
	}
      pAb = El->ElAbstractBox[view - 1];
      if (pAb)
	if (pAb->AbFirstEnclosed)
	  {
	    UpdateClipping (pAb->AbFirstEnclosed);	    
	    Trans->Factor = result;
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
	}
      pAb = El->ElAbstractBox[view - 1];
      if (pAb)
	if (pAb->AbFirstEnclosed)
	  {
	    UpdateClipping (pAb->AbFirstEnclosed);
	    Trans->Factor = result;
	  }
      
      break;
    }  
}

/*----------------------------------------------------------------------
  animate_box_motion : Animate the position of a box using a path
  ----------------------------------------------------------------------*/
static void animate_box_motion (PtrElement El,
			       Animated_Element *animated,
			       AnimTime current_time)
{
  AnimPath         *pop_path;
  float            *proportion;
  float            result;
  float            x,y,x1,y1,x2,y2;
  int              doc, view,i;
  PtrAbstractBox   pAb = NULL;

  pop_path = (AnimPath *) animated->from;
  proportion = pop_path->Proportion;
  result = (float) interpolate_double_value (0, 
					     1,
					     current_time,
					     animated->duration); 
  i = 0;
  while (result > proportion[i] && 
	 i < (pop_path->npoints - 1) )
    {
      i++;
    }
  if (i > 0)
    {
      x2 = pop_path->Path[i].x;
      y2 = pop_path->Path[i].y;      
      x1 = pop_path->Path[i-1].x;
      y1 = pop_path->Path[i-1].y;            
      result = (result - proportion[i-1]) / (proportion[i] - proportion[i-1]);      
      x = x1 + (result*(x2 -x1));
      y = y1 + (result*(y2 -y1));
      FrameToView (Animated_Frame, &doc, &view);
      pAb = El->ElAbstractBox[view - 1];
      if (pAb)
	if (pAb->AbFirstEnclosed)
	  {	    
	    UpdateClipping (pAb->AbFirstEnclosed);	    
	    ApplyXToAllBoxes (pAb->AbFirstEnclosed, (float) x);
	    ApplyYToAllBoxes (pAb->AbFirstEnclosed, (float) y);	   
	  }
      /*calculate normals to the path and rotate accordingly*/      
      /*TtaNewTransformAnimRotate (angle, x_scale, y_scale); */
    }      
}

#define ANIMATING 1
#define WILL_BE_ANIMATING 2
#define NOT_ANIMATING 3

/*----------------------------------------------------------------------
  is_animated_now : Compute if animation appply for this box upon time
  ----------------------------------------------------------------------*/
static int is_animated_now (Animated_Element *animated, AnimTime *current_time)
{

  if ((*current_time - animated->start) > 0.0001)
    {
      if ((*current_time - (animated->start + animated->duration)) <= 0.00001) 
	{
	  *current_time = *current_time - animated->start;

	}
      else if (animated->repeatCount > 1 && 
	       (((float)animated->repeatCount) - 
		((float)(*current_time/animated->duration))) > 0.00001)
	*current_time = fmod (*current_time, animated->duration);
      else if (animated->repeatCount == -1)
	{
	  *current_time = fmod (*current_time, animated->duration);
	}
      else if (animated->AnimType != Set) 
	{
	  switch (animated->Fill)
	    {
	    case Freeze:
	      *current_time = (AnimTime) animated->duration;
	      break;
	    case Otherfill:
	      *current_time = (AnimTime) 0;
	      break;
	    default:
	      *current_time = (AnimTime) 0;
	      break;
	    }	   
	}
    }
  else
    return WILL_BE_ANIMATING;

  if (animated->AnimType == Set) 
    {
      if ((*current_time -  (animated->start + animated->duration)) > 0.00001 &&
	  fabs (animated->action_time - (animated->start + animated->duration))  > 0.00001)
	{
	  *current_time = animated->start + animated->duration;
	  return ANIMATING;
	}
      else if ((*current_time - animated->start) > 0.00001 && 
	       fabs (animated->action_time - (animated->start)) > 0.00001)
	{
	  *current_time = animated->start;
	  return ANIMATING;
	}
      else
	return NOT_ANIMATING;
    }
  /* Detect diffences between last action played and now*/
  if (fabs (animated->action_time - *current_time) > 0.00001)
    return ANIMATING;
  else
    return NOT_ANIMATING;
}
/*----------------------------------------------------------------------
  animate_box : animate a a box using all animation that apply on him
  ----------------------------------------------------------------------*/
static ThotBool animate_box (PtrElement El, AnimTime current_time)
{
  Animated_Element *animated = NULL;
  AnimTime          rel_time;
  ThotBool          isnotfinished;
  int               animating_state;

  isnotfinished = FALSE;
  if (El)
    if (El->ElAnimation)
      {      
	animated = (Animated_Element *) El->ElAnimation;
	while (animated)
	  {
	    rel_time = current_time;
            animating_state = is_animated_now (animated, &rel_time);
	    if (animating_state == WILL_BE_ANIMATING)
	      isnotfinished = TRUE;
	    if (animating_state == ANIMATING)
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
		    if (animated->from)
		      animate_box_motion (El, animated, rel_time);
		    break;
		  
		  case Animate:
		    if (animated->from && animated->to)
		      animate_box_animate (El, animated, rel_time);
		    break;
		  
		  case Set:
		    if (animated->to)
		      animate_box_set (El, animated, rel_time);
		    break;
		  
		  case OtherAnim:
		    break;
		  
		  default:
		    break;      
		  }
		isnotfinished = TRUE;
		/*Store last animation render success*/
		animated->action_time = rel_time;
	      }
	    animated = animated->next;	      
	  }
      }
  return isnotfinished;    
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
  
  if (element->ElAnimation)
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
ThotBool Animate_boxes (int frame, 
		    AnimTime current_time)
{
#ifdef _GL 
  Animated_Cell *current = FrameTable[frame].Animated_Boxes;
  ThotBool isfinished;

  /* Time update
     RefreshAnimBoxes ()
     Get Clipping Box ()
     DefFrame ()
     RedrawFrameBottom ()   */ 
  Animated_Frame = frame;
  SetBaseClipping ();
  isfinished = TRUE;
  while (current)
    {
      if (animate_box (current->El, current_time))
	isfinished = FALSE;
      current = current->Next;
    }
  if (isfinished)
    return TRUE;
  DefRegion (frame, Clipx, Clipy, ClipxMax, ClipyMax);
  FrameTable[frame].DblBuffNeedSwap = TRUE;
  return FALSE;
#endif /* _GL */
  return TRUE;
}
