/*
 *
 *  (c) COPYRIGHT INRIA, 2003-2009
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
/*
 * Handle Animated Boxes
 *
 * Authors: P. Cheyrou (INRIA)
 *
 */
#include "thot_gui.h"
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
#include "boxes_tv.h"
#include "page_tv.h"
#include "platform_tv.h"
#include "edit_tv.h"
#include "modif_tv.h"
#include "frame_tv.h"
#include "appdialogue_tv.h"

#include "absboxes_f.h"
#include "abspictures_f.h"
#include "appdialogue_wx.h"
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
#include "frame_f.h"
#include "displayview_f.h"


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
static float Clipx, Clipy, ClipxMax, ClipyMax;

#define Min(number, min) (number = ((number < 0) ? min : (((number - min) < 0.00001) ? number : min)) )
#define Max(number, max) (number = ((number < 0) ? max : (((number - max) > 0.00001) ? number : max)) )
#endif /* _GL */

/*----------------------------------------------------------------------
  T_atof prevents the bad conversion of French Windows platforms.
  ----------------------------------------------------------------------*/
static float T_atof (char *ptr)
{
  float   val = 0.;
  char    *c;

  val = (float)atof (".5");
  if (val == 0.5)
    val = (float)atof (ptr);
  else
    {
      c = strstr (ptr, ".");
      if (c)
        *c = ',';
      val = (float)atof (ptr);
      if (c)
        *c = '.';
    }
  return val;
}

#ifdef _GL
/*----------------------------------------------------------------------
  ComputePropandAngle : interpolate proportion of each point over
  total list of point
  
  float           length;      total length 
  ThotPath        *Path;       The Path
  float           *Proportion; per segment % of total length
  ----------------------------------------------------------------------*/
static void ComputePropandAngle (AnimPath   *pop_path)
{
  int        i, npoints = 0;
  float      x, y, totallength, length;
  ThotPoint  *points;
  float      *proportion, *angle;

  points = pop_path->Path;
  npoints = pop_path->npoints;
  proportion = (float *) TtaGetMemory (npoints * sizeof(float));
  angle = (float *) TtaGetMemory (npoints * sizeof(float));
  totallength = 0;
  proportion[0] = 0;
  for (i = 1; i < npoints; i++)
    {
      x = (float)(points[i].x - points[i-1].x);
      y = (float)(points[i].y - points[i-1].y);
      length = (float)sqrt(x*x + y*y);
      /*
        /|
        / |
        /  |
        /   |
        /    |
        /     |
        /      |
        /      _|
        / angle| |
        +-------+-+
        <--- x -->
	  
        cos j = a1 / square root(a1*a1 + a2*a2) 
        if (length > 0)
        angle[i] = acos (x / length)
        else
        angle[i] = 90;
      */
      if (fabs (y) < 0.0001)
        angle[i] = (float)ARCTAN (x/y);
      else
        angle[i] = 90.;

      totallength +=  length;
      proportion[i] = totallength;
    }
  if (totallength == 0)
    totallength = 1;

  for (i = 1; i < npoints; i++)
    {
      proportion[i] = proportion[i] / totallength;
    }
  pop_path->Proportion = proportion;
  pop_path->length = totallength;
  pop_path->Tangent_angle = angle;
}
#endif /* _GL */

/*----------------------------------------------------------------------
  populate_path_proportion : 

  PtrPathSeg      FirstPathSeg; linked list of segment defining the path
  ----------------------------------------------------------------------*/
void populate_path_proportion (void *anim_info)
{
#ifdef _GL
  int        npoints = 0;
  int        *subpathStart = NULL;
  ThotPoint  *points;
  AnimPath   *pop_path;
  Animated_Element *animated;

  animated = (Animated_Element *) (anim_info);
  pop_path = (AnimPath *) animated->from;
  //points = (ThotPoint *) TtaGetMemory (ALLOC_POINTS * sizeof(ThotPoint));
  points = BuildPolygonForPath (((PtrPathSeg) (pop_path->FirstPathSeg)), 
                                Animated_Frame,
                                &npoints, 
                                &subpathStart);

  pop_path->npoints = npoints;
  pop_path->Path = points;
  ComputePropandAngle (pop_path);
#endif /* _GL */
}

/*----------------------------------------------------------------------
  populate_values_proportion : interpolate proportion of each point over
  total path length
  ----------------------------------------------------------------------*/
void populate_values_proportion (void *anim_info)
{
#ifdef _GL
  Animated_Element *animated;

  animated = (Animated_Element *) (anim_info);
  ComputePropandAngle ((AnimPath *) animated->from);
#endif /* _GL */
}

/*----------------------------------------------------------------------
  populate_fromto_proportion : interpolate proportion of each point over
  total values list
  ----------------------------------------------------------------------*/
void populate_fromto_proportion (void *anim_info)
{
#ifdef _GL
  Animated_Element *animated;

  animated = (Animated_Element *) (anim_info);
  ComputePropandAngle ((AnimPath *) animated->from);
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

#ifdef _NEXT
/*----------------------------------------------------------------------
  TtaAnimationReplaceTransform 
  ----------------------------------------------------------------------*/
static void TtaAnimationReplaceTransform (AnimTime current_time,
                                          PtrTransform TransNew,
                                          PtrElement El)
{
  ThotBool not_found;
  PtrTransform Trans;
  PtrTransform previous = NULL;
  Document doc;
  View view;

  not_found = TRUE; 
  if (El->ElTransform)
    {
      Trans = El->ElTransform;
      while (not_found && Trans)
        {
          if (Trans->TransType != TransNew->TransType)
            {
              previous = Trans;
              Trans = Trans->Next;
            }
          else
            not_found = FALSE;      
        }
    }	 
  else  
    {
      FrameToView (Animated_Frame, &doc, &view);
      TtaReplaceTransform ((Element) El, TransNew, doc); 
    }
}
#endif /* _NEXT */

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
static void UpdateClipping (PtrAbstractBox pAb, int w, int h)
{
  /*  PtrBox box = pAb->AbEnclosing->AbBox; */
  PtrBox box = pAb->AbBox;
  if (box &&
      (box->BxClipX + box->BxClipW) > 0 &&
      (box->BxClipY + box->BxClipH) > 0 &&
      box->BxClipX < w &&
      box->BxClipY < h)
    {
      Min (Clipx, box->BxClipX);		    
      Max (ClipxMax, (box->BxClipX + box->BxClipW));
      Min (Clipy, box->BxClipY);		    
      Max (ClipyMax, (box->BxClipY + box->BxClipH));
    }
}

/*----------------------------------------------------------------------
  interpolate_double_value : Compute a the value corresponding to a time
  using the "regle de trois" ("rule of three")
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
  while (pAb)
    {      
      pAb->AbBox->VisibleModification = TRUE;
      pAb->AbBackground = result;
      pAb->AbGradientBackground = FALSE;
      ApplyFillColorToAllBoxes (pAb->AbFirstEnclosed, result);
      pAb = pAb->AbNext;
    }
}

/*----------------------------------------------------------------------
  ApplyOpacityToAllBoxes : Recursivly apply the property
  ----------------------------------------------------------------------*/
static void ApplyOpacityToAllBoxes (PtrAbstractBox pAb, int result)
{
  while (pAb)
    {      
      pAb->AbBox->VisibleModification = TRUE;
      pAb->AbOpacity = result;
      /* test if it's a group*/
      if (!pAb->AbPresentationBox &&
          !TypeHasException (ExcIsGroup, pAb->AbElement->ElTypeNumber,
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
  PtrTransform  Trans = NULL;
  int           doc, view;

  while (pAb)
    {
      pBox = pAb->AbBox;
      pBox->VisibleModification = TRUE;
      if (pAb->AbElement->ElSystemOrigin)
        {
          El = pAb->AbElement;
          if (El->ElTransform)
            Trans = GetTransformation (El->ElTransform, PtElTranslate); 	  
          if (Trans == NULL)
            {
              Trans = (PtrTransform)TtaNewTransformTranslate (result, 0);
              FrameToView (Animated_Frame, &doc, &view);
              TtaReplaceTransform ((Element) El, Trans, doc); 
            }  
          Trans->XScale = result;
        }
      else
        {
          pBox->BxXOrg = (int)result;
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
  PtrTransform  Trans = NULL;
  int           doc, view;

  while (pAb)
    {   
      pBox = pAb->AbBox;
      pBox->VisibleModification = TRUE;
      if (pAb->AbElement->ElSystemOrigin)
        {
          El = pAb->AbElement;
          if (El->ElTransform)
            Trans = GetTransformation (El->ElTransform, PtElTranslate); 	  
          if (Trans == NULL)
            {
              Trans = (PtrTransform)TtaNewTransformTranslate (0, result);
              FrameToView (Animated_Frame, &doc, &view);
              TtaReplaceTransform ((Element) El, Trans, doc); 
            }  
          Trans->YScale = result;
        }
      else
        {
          if (pAb->AbLeafType == LtText)
            result -= BoxFontBase (pBox->BxFont);
          pBox->BxYOrg = (int)result;
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
  while (pAb)
    {      
      pAb->AbBox->VisibleModification = TRUE;
      pAb->AbBox->BxW = (int)result;     
      ApplyWidthToAllBoxes (pAb->AbFirstEnclosed, result);
      pAb = pAb->AbNext;
    }
}

/*----------------------------------------------------------------------
  ApplyHeightToAllBoxes : Recursivly apply the property
  ----------------------------------------------------------------------*/
static void ApplyHeightToAllBoxes (PtrAbstractBox pAb, float result)
{
  while (pAb)
    {      
      pAb->AbBox->VisibleModification = TRUE;
      pAb->AbBox->BxH = (int)result;
      ApplyHeightToAllBoxes (pAb->AbFirstEnclosed, result);
      pAb = pAb->AbNext;
    }
}

/*----------------------------------------------------------------------
  : Recursivly apply the property
  ----------------------------------------------------------------------*/
static void ApplyFontSizeToAllBoxes (PtrAbstractBox pAb, int result)
{

  while (pAb)
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
  int            doc, view;
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
  if (pAb && pAb->AbFirstEnclosed)
      {	    
        if (animated->AttrName)
          {
            if (strlen (animated->AttrName) == 4 &&
                !strcasecmp (animated->AttrName, "fill"))
              ApplyFillColorToAllBoxes (pAb->AbFirstEnclosed, result);
            else if (strlen (animated->AttrName) == 6 &&
                     !strcasecmp (animated->AttrName, "stroke"))
              ApplyStrokeColorToAllBoxes (pAb->AbFirstEnclosed, result);
          }
        else
          ApplyFillColorToAllBoxes (pAb->AbFirstEnclosed, result);
      }
}

/*----------------------------------------------------------------------
  animate_box_animate : Animate any property of an element				
  ----------------------------------------------------------------------*/
static void animate_box_set (PtrElement El,
                             Animated_Element *animated,
                             AnimTime current_time)
{
  int            doc, view;
  PtrAbstractBox pAb = NULL;
  double         result;
  
  if (animated->AttrName == NULL)
    return;
  
  if (!strcasecmp (animated->AttrName, "opacity"))
    {  
      FrameToView (Animated_Frame, &doc, &view);
      pAb = El->ElAbstractBox[view - 1];
      if (pAb && pAb->AbFirstEnclosed)
        {	  
          result = 1000 * T_atof ((char *) animated->to);
          pAb->AbOpacity = (int)result;
          /*If it's an opaque group manage the opacity*/
          if (!pAb->AbPresentationBox &&
              !TypeHasException (ExcIsGroup, pAb->AbElement->ElTypeNumber,
                                 pAb->AbElement->ElStructSchema))
            ApplyOpacityToAllBoxes (pAb->AbFirstEnclosed, (int) (result));
          }
    }
  else if (!strcasecmp (animated->AttrName, "x"))
    {
      FrameToView (Animated_Frame, &doc, &view);
      pAb = El->ElAbstractBox[view - 1];
      if (pAb)
        if (pAb->AbFirstEnclosed)
          {
            result = T_atof ((char *) animated->to);
            ApplyXToAllBoxes (pAb->AbFirstEnclosed, (float) result);
          }
      
    }
  else if (!strcasecmp (animated->AttrName, "y"))
    {
      FrameToView (Animated_Frame, &doc, &view);
      pAb = El->ElAbstractBox[view - 1];
      if (pAb)
        if (pAb->AbFirstEnclosed)
          {
            result = T_atof ((char *) animated->to);
            ApplyYToAllBoxes (pAb->AbFirstEnclosed, (float) result);
          }
      
    }
  else if (!strcasecmp (animated->AttrName, "width"))
    {
      FrameToView (Animated_Frame, &doc, &view);
      pAb = El->ElAbstractBox[view - 1];
      if (pAb)
        if (pAb->AbFirstEnclosed)
          {
            result = T_atof ((char *) animated->to);
            ApplyWidthToAllBoxes (pAb->AbFirstEnclosed, (float) result);
          }
    }
  else if (!strcasecmp (animated->AttrName, "height"))
    {
      FrameToView (Animated_Frame, &doc, &view);
      pAb = El->ElAbstractBox[view - 1];
      if (pAb)
        if (pAb->AbFirstEnclosed)
          {
            result = T_atof ((char *) animated->to);
            ApplyHeightToAllBoxes (pAb->AbFirstEnclosed, (float) result);	   
          }
      
    }
  else if (!strcasecmp (animated->AttrName, "font-size"))
    {
      FrameToView (Animated_Frame, &doc, &view);
      pAb = El->ElAbstractBox[view - 1];
      if (pAb && pAb->AbFirstEnclosed)
        result = T_atof ((char *) animated->to);
      
    }
  else if (!strcasecmp (animated->AttrName, "fill") || 
           !strcasecmp (animated->AttrName, "stroke"))
    animate_box_color (El, animated, current_time);    
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
            result = 1000 * interpolate_double_value (T_atof ((char *) animated->from), 
                                                      T_atof ((char *) animated->to),
                                                      current_time,
                                                      animated->duration);

            pAb->AbOpacity = (int) result;
            /*If it's an opaque group manage the opacity*/
            if (!TypeHasException (ExcIsGroup, pAb->AbElement->ElTypeNumber,
                                   pAb->AbElement->ElStructSchema))
              ApplyOpacityToAllBoxes (pAb->AbFirstEnclosed, (int) (result));
          }      
    }
  else if (strcasecmp (animated->AttrName, "x") == 0)
    {
      FrameToView (Animated_Frame, &doc, &view);
      pAb = El->ElAbstractBox[view - 1];
      if (pAb)
        if (pAb->AbFirstEnclosed)
          {
            result = interpolate_double_value (T_atof ((char *) animated->from), 
                                               T_atof ((char *) animated->to),
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
            result = interpolate_double_value (T_atof ((char *) animated->from), 
                                               T_atof ((char *) animated->to),
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
            result = interpolate_double_value (T_atof ((char *) animated->from), 
                                               T_atof ((char *) animated->to),
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
            result = interpolate_double_value (T_atof ((char *) animated->from), 
                                               T_atof ((char *) animated->to),
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
            result = interpolate_double_value (T_atof ((char *) animated->from), 
                                               T_atof ((char *) animated->to),
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
  skip white spaces
  ----------------------------------------------------------------------*/
static char *skipWsp (char *ptr)
{
  while (*ptr == SPACE || *ptr == EOL || *ptr == TAB || *ptr == __CR__)
    ptr++;
  return (ptr);
}

/*----------------------------------------------------------------------
  skip white spaces or a comma preceded and/or followed by white spaces
  ----------------------------------------------------------------------*/
static char *skipCommaWsp (char *ptr)
{
  while (*ptr == SPACE || *ptr == EOL || *ptr == TAB || *ptr == __CR__)
    ptr++;
  if (*ptr == ',')
    {
      ptr++;
      while (*ptr == SPACE || *ptr == EOL || *ptr == TAB || *ptr == __CR__)
	ptr++;
    }
  return (ptr);
}

/*----------------------------------------------------------------------
  skip a number
  ----------------------------------------------------------------------*/
static char *skipNumber (char *ptr)
{
  while (*ptr != SPACE && *ptr != EOL && *ptr != TAB && *ptr != __CR__ &&
	 *ptr != ',' && *ptr != EOS)
    ptr++;
  return (ptr);
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
  long int         trans_type;
  PtrTransform     Trans = NULL;
  char*            ptr;

  if (animated->AttrName == NULL)
    return;
  
  trans_type = (long int)(animated->AttrName);
  /*translate, scale, rotate_, skewX, skewY*/
  switch (trans_type)
    {
    case 1 : /* translate */
      FrameToView (Animated_Frame, &doc, &view);
      tx = ty = fx = fy = 0; /* default values */
      /* parse the "to" attribute */
      ptr = (char *) animated->to;
      ptr = skipWsp (ptr);
      tx = T_atof (ptr);
      ptr = skipNumber (ptr);
      ptr = skipCommaWsp (ptr);
      if (*ptr == EOS)
	ty = tx;
      else
	ty = T_atof (ptr);
      /* parse the "from" attribute */
      ptr = (char *) animated->from;
      ptr = skipWsp (ptr);
      fx = T_atof (ptr);
      ptr = skipNumber (ptr);
      ptr = skipCommaWsp (ptr);
      if (*ptr == EOS)
	fy = fx;
      else
	fy = T_atof (ptr);

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
          Trans = (PtrTransform)TtaNewTransformTranslate (tx, ty);
          TtaReplaceTransform ((Element) El, Trans, doc); 
        }
      Trans->XScale = tx;
      Trans->YScale = ty;
      break;

    case 2 : /* scale */
      FrameToView (Animated_Frame, &doc, &view);
      tx = ty = fx = fy = 0; /* default values */
      /* parse the "to" attribute */
      ptr = (char *) animated->to;
      ptr = skipWsp (ptr);
      tx = T_atof (ptr);
      ptr = skipNumber (ptr);
      ptr = skipCommaWsp (ptr);
      if (*ptr == EOS)
	ty = tx;
      else
	ty = T_atof (ptr);
      /* parse the "from" attribute */
      ptr = (char *) animated->from;
      ptr = skipWsp (ptr);
      fx = T_atof (ptr);
      ptr = skipNumber (ptr);
      ptr = skipCommaWsp (ptr);
      if (*ptr == EOS)
	fy = fx;
      else
	fy = T_atof (ptr);

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
          Trans = (PtrTransform)TtaNewTransformScale (tx, ty);
          TtaReplaceTransform ((Element) El, Trans, doc); 
        }
      Trans->XScale = tx;
      Trans->YScale = ty;
      break;

    case 3 : /* Rotate */
      FrameToView (Animated_Frame, &doc, &view);
      rott = tx = ty = rotf = fx = fy = 0; /* default values */
      /* parse the "to" attribute */
      ptr = (char *) animated->to;
      ptr = skipWsp (ptr);
      rott = T_atof (ptr); /* angle */
      ptr = skipNumber (ptr);
      ptr = skipCommaWsp (ptr);
      if (*ptr != EOS)
	{
	  tx = T_atof (ptr);  /* x coord. of center of rotation */
	  ptr = skipNumber (ptr);
	  ptr = skipCommaWsp (ptr);
	  ty = T_atof (ptr); /* y coord. of center of rotation */
	}
      /* parse the "from" attribute */
      ptr = (char *) animated->from;
      ptr = skipWsp (ptr);
      rotf = T_atof (ptr); /* angle */
      ptr = skipNumber (ptr);
      ptr = skipCommaWsp (ptr);
      if (*ptr != EOS)
	{
	  fx = T_atof (ptr);  /* x coord. of center of rotation */
	  ptr = skipNumber (ptr);
	  ptr = skipCommaWsp (ptr);
	  fy = T_atof (ptr);  /* y coord. of center of rotation */
	}

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
          Trans = (PtrTransform)TtaNewTransformRotate (rott, tx, ty);
          TtaReplaceTransform ((Element) El, Trans, doc); 
        }
      Trans->TrAngle = rott;
      Trans->XRotate = tx;
      Trans->YRotate = ty;
      break;
      
    case 4 : /* skewX */
      FrameToView (Animated_Frame, &doc, &view);
      /* parse the "to" attribute */
      ptr = (char *) animated->to;
      ptr = skipWsp (ptr);
      tx = T_atof (ptr);
      /* parse the "from" attribute */
      ptr = (char *) animated->from;
      ptr = skipWsp (ptr);
      fx = T_atof (ptr);
      result = (float)interpolate_double_value (fx, 
                                                tx,
                                                current_time,
                                                animated->duration);      
      if (El->ElTransform)
        Trans = GetTransformation (El->ElTransform, 
                                   PtElSkewX);       
      if (Trans == NULL)
        {
          Trans = (PtrTransform)TtaNewTransformSkewX (result);
          TtaReplaceTransform ((Element) El, Trans, doc); 
        }
      Trans->TrFactor = result;
      break;
     
    case 5 : /* skewY */
      FrameToView (Animated_Frame, &doc, &view);
      /* parse the "to" attribute */
      ptr = (char *) animated->to;
      ptr = skipWsp (ptr);
      ty = T_atof (ptr);
      /* parse the "from" attribute */
      ptr = (char *) animated->from;
      ptr = skipWsp (ptr);
      fy = T_atof (ptr);
      result = (float)interpolate_double_value (fy,
                                                ty,
                                                current_time,
                                                animated->duration);      
      if (El->ElTransform)
        Trans = GetTransformation (El->ElTransform, 
                                   PtElSkewY);       
      if (Trans == NULL)
        {
          Trans = (PtrTransform)TtaNewTransformSkewY (result);
          TtaReplaceTransform ((Element) El, Trans, doc); 
        }
      Trans->TrFactor = result;
      break;
    }  
}

/*----------------------------------------------------------------------
  animate_box_motion : Animate the position of a box using a path
  ----------------------------------------------------------------------*/
static void animate_box_motion (PtrElement El, Animated_Element *animated,
                                AnimTime current_time)
{
  AnimPath         *pop_path;
  float            *proportion;
  float            result;
  float            x,y,x1,y1,x2,y2;
  int              doc, view,i;
  PtrAbstractBox   pAb = NULL;
  PtrTransform     Trans = NULL;

  pop_path = (AnimPath *) animated->from;
  proportion = pop_path->Proportion;
  result = (float) interpolate_double_value (0, 1,
                                             current_time,
                                             animated->duration); 
  i = 0;
  while (result > proportion[i] && i < (pop_path->npoints - 1))
    i++;

  if (i > 0)
    {
      x2 = (float)pop_path->Path[i].x;
      y2 = (float)pop_path->Path[i].y;      
      x1 = (float)pop_path->Path[i-1].x;
      y1 = (float)pop_path->Path[i-1].y;            
      result = (result - proportion[i-1]) / (proportion[i] - proportion[i-1]);      
      x = x1 + (result*(x2 -x1));
      y = y1 + (result*(y2 -y1));
      FrameToView (Animated_Frame, &doc, &view);
      pAb = El->ElAbstractBox[view - 1];
      if (pAb)
        if (pAb->AbFirstEnclosed)
          {
            ApplyXToAllBoxes (pAb->AbFirstEnclosed, 0);
            ApplyYToAllBoxes (pAb->AbFirstEnclosed, 0); 

            if (El->ElTransform)
              Trans = GetTransformation (El->ElTransform, 
                                         PtElAnimTranslate); 	  
            if (Trans == NULL)
              {
                Trans = (PtrTransform)TtaNewTransformAnimTranslate (x, y);	    
                TtaReplaceTransform ((Element) El, Trans, doc); 
              }
            Trans->XScale = x;
            Trans->YScale = y;
          }
      /*calculate normals to the path and rotate accordingly*/
      if (0) 
        {
          if (El->ElTransform)
            Trans = GetTransformation (El->ElTransform, 
                                       PtElAnimRotate);
          if (Trans == NULL)
            {
              Trans = (PtrTransform)TtaNewTransformAnimRotate (pop_path->Tangent_angle[i], 0, 0); 
              TtaReplaceTransform ((Element) El, Trans, doc); 
            }
          Trans->TrAngle = pop_path->Tangent_angle[i];
          Trans->XRotate = 0;
          Trans->YRotate = 0;
        }
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
        *current_time = *current_time - animated->start;
      else if (animated->repeatCount > 1 && 
               (((float)animated->repeatCount) - 
                ((float)(*current_time/animated->duration))) > 0.00001)
        *current_time = fmod (*current_time, animated->duration);
      else if (animated->repeatCount == -1)
        *current_time = fmod (*current_time, animated->duration);
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
          (animated->action_time - (animated->start + animated->duration))  > 0.00001)
        {
          *current_time = animated->start + animated->duration;
          return ANIMATING;
        }
      else if ((*current_time - animated->start) > 0.00001 && 
               fabs(animated->action_time - (animated->start)) > 0.00001)
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
  ThotBool          isnotfinished, willbenotfinished;
  int               animating_state;
  PtrAbstractBox    pAb;
  int               w,h;
  
  isnotfinished = FALSE;
  willbenotfinished = FALSE;
  if (El)
    if (El->ElAnimation)
      {    
        pAb = El->ElAbstractBox[0];
        if (pAb)
          {
            w = FrameTable[Animated_Frame].FrWidth;
            h = FrameTable[Animated_Frame].FrHeight;
            animated = (Animated_Element *) El->ElAnimation;

            while (animated)
              {
                rel_time = current_time;
                animating_state = is_animated_now (animated, &rel_time);
                if (animating_state == WILL_BE_ANIMATING)
                  willbenotfinished = TRUE;

                if (animating_state == ANIMATING)
                  {
                    if (!isnotfinished)
                      {
                        isnotfinished = TRUE;
                        UpdateClipping (pAb, w, h);
                      }
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
                    /*Store last animation render success*/
                    animated->action_time = rel_time;
                  }
                animated = animated->next;	      
              }
            if (isnotfinished)
              UpdateClipping (pAb, w, h);
          }
      }
  return (isnotfinished || willbenotfinished);    
}
#endif /* _GL */

/*----------------------------------------------------------------------
  Animate_boxes: Animates All animated boxe and define the region that 
  needs to be redisplayed.
  ----------------------------------------------------------------------*/
ThotBool Animate_boxes (int frame, AnimTime current_time)
{
#ifdef _GL 
  ViewFrame        *pFrame;
  Animated_Cell    *current;
  ThotBool          isfinished;

  if (TtaFrameIsShown (frame))
    {
      /* Time update */
      current = (Animated_Cell *)FrameTable[frame].Animated_Boxes;
      Animated_Frame = frame;
      SetBaseClipping ();
      isfinished = TRUE;
      while (current)
        {      
          if (GL_prepare (frame)) // to be sure the canvas is current one
            if (animate_box (current->El, current_time))
              isfinished = FALSE;
          current = current->Next;
        }
      ComputeChangedBoundingBoxes (Animated_Frame);
      if (isfinished)
        return TRUE;
      
      if (Clipx - 1 < 0.00001)
        Clipx = 1;
      if (Clipy - 1 < 0.00001)
        Clipy = 1;
      pFrame = &ViewFrameTable[frame - 1];
      DefClip (frame, 
               (int)(Clipx - 1) + pFrame->FrXOrg, 
               (int)(Clipy - 1)+ pFrame->FrYOrg, 
               (int)(ClipxMax + 1) + pFrame->FrXOrg, 
               (int)(ClipyMax + 1)+ pFrame->FrYOrg);
      
      FrameTable[frame].DblBuffNeedSwap = TRUE;
    }
  return FALSE;
#endif /* _GL */
  return TRUE;
}
