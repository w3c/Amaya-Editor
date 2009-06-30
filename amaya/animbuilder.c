
/*
 *
 *  (c) COPYRIGHT INRIA and W3C, 2002-2005
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * This module contains browsing functions to handle SVG animations.
 *
 * Author: Paul Cheyrou-lagreze (INRIA)
 *	  
 */

/* Included headerfiles */
#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "amaya.h"
#include "css.h"
#include "trans.h"
#include "view.h"
#include "content.h"

#include "SVG.h"
#include "HTML.h"
#include "HTMLactions_f.h"
#include "init_f.h"
#include "HTMLedit_f.h"
#include "EDITimage_f.h"
#include "SVGbuilder_f.h"
#include "AHTURLTools_f.h"
#include "html2thot_f.h"
#include "styleparser_f.h"

#include "SVGedit_f.h"
#include "anim_f.h"

#ifdef _WINDOWS
#include <commctrl.h>
#endif /* _WINDOWS */


/*----------------------------------------------------------------------
  get_int_attribute_from_el: Get a int value from an xml attribute
  ----------------------------------------------------------------------*/
static int get_int_attribute_from_el (Element el, int Attribut_Type)
{
#ifdef _SVG
  AttributeType attrType;
  Attribute attr = NULL;
  ElementType elType = TtaGetElementType (el);
  int result;
  
  attrType.AttrSSchema = elType.ElSSchema;
  attrType.AttrTypeNum = Attribut_Type;  
  attr = TtaGetAttribute (el, attrType);
  result = TtaGetAttributeValue (attr);
  return result;
#endif /* _SVG */
}
/*----------------------------------------------------------------------
  get_attribute_from_el: Get an xml attribute
  ----------------------------------------------------------------------*/
static Attribute get_attribute_from_el (Element el, int Attribut_Type)
{
#ifdef _SVG
  AttributeType attrType;
  Attribute attr = NULL;
  ElementType elType = TtaGetElementType (el);
  
  attrType.AttrSSchema = elType.ElSSchema;
  attrType.AttrTypeNum = Attribut_Type;  
  attr = TtaGetAttribute (el, attrType);
  return attr;
#endif /* _SVG */
}
/*----------------------------------------------------------------------
  get_intptr_attribute_from_el: Get a int value from an xml attribute,
  but stored as an allocated ptr
  ----------------------------------------------------------------------*/
static int *get_intptr_attribute_from_el (Element el, int Attribut_Type)
{
#ifdef _SVG
  AttributeType attrType;
  Attribute     attr = NULL;
  ElementType   elType = TtaGetElementType (el);
  int          *result;
  
  attrType.AttrSSchema = elType.ElSSchema;
  attrType.AttrTypeNum = Attribut_Type;  
  attr = TtaGetAttribute (el, attrType);
  result = (int *) TtaGetAttributeValue (attr);
  return result;
#endif /* _SVG */
}

/*----------------------------------------------------------------------
  get_char_attribute_from_el: Get a string 
  value from an xml attribute, but stored as an allocated ptr
  ----------------------------------------------------------------------*/
char *get_char_attribute_from_el (Element el, int Attribut_Type)
{
#ifdef _SVG
  int length;
  char *text = NULL, *ptr;
  AttributeType attrType;
  Attribute attr = NULL;
  ElementType elType = TtaGetElementType (el);
  
  attrType.AttrSSchema = elType.ElSSchema;
  attrType.AttrTypeNum = Attribut_Type;
  attr = TtaGetAttribute (el, attrType);
  length = TtaGetTextAttributeLength (attr);
  if (length == 0)
    return NULL;
  text = (char *)TtaGetMemory (length+1);
  if (text) 
    {
      /* get the value of the x attribute */
      TtaGiveTextAttributeValue (attr, text, &length);
      /* Parse the attribute value 
         (a number followed by a unit) */
      ptr = text;
      /*TtaFreeMemory (text);*/
    }
  return text;
#endif /* _SVG */
}


/*----------------------------------------------------------------------
  register_animated_element: store animation linked list in an element 
  ----------------------------------------------------------------------*/
void register_animated_element (Element animated, Document doc)
{
#ifdef _SVG
  void       *anim_info;
  ElementType elType;
  double      start, duration;
  char       *repeatcount;  
  int         repeat;
  long int    val;

  start = duration = 0;
  anim_info = TtaNewAnimation ();
  Read_time_info (animated, &start, &duration);
  TtaSetAnimationTime (anim_info, start, duration);
  elType = TtaGetElementType (animated);

  switch (get_int_attribute_from_el (animated, SVG_ATTR_fill_))
    {
    case SVG_ATTR_fill__VAL_remove_:
      TtaAddAnimRemove (anim_info);
      break;
    case SVG_ATTR_fill__VAL_freeze:
      TtaAddAnimFreeze (anim_info);
      break;
    default:
      TtaAddAnimRemove (anim_info);
      break;
    } 

  switch (get_int_attribute_from_el (animated, SVG_ATTR_additive))
    {
    case SVG_ATTR_additive_VAL_replace:
      TtaSetAnimReplace (anim_info, TRUE);
      break;
    case SVG_ATTR_additive_VAL_sum:
      TtaSetAnimReplace (anim_info, FALSE);
      break;
    default:
      TtaSetAnimReplace (anim_info, TRUE);
      break;
    }

  repeatcount = get_char_attribute_from_el  (animated, SVG_ATTR_repeatCount);
  if (repeatcount)
    {    
      repeat = atoi (repeatcount);      
      if (repeat)
        TtaAddAnimRepeatCount (repeat, anim_info);
      else
        if (strlen (repeatcount) == 10 && 
            strcasecmp ("indefinite", repeatcount) == 0)
          TtaAddAnimRepeatCount (-1, anim_info);
      TtaFreeMemory (repeatcount);
    }
  
  switch (elType.ElTypeNum)
    {      
    case SVG_EL_animateMotion :      
      if (get_attribute_from_el (animated, SVG_ATTR_values))
        {
          TtaAddAnimMotionValues ((void *) ParseValuesDataAttribute (get_attribute_from_el (animated, 
                                                                                            SVG_ATTR_values),
                                                                     animated, 
                                                                     doc),
                                  anim_info);
        }
      else if (get_attribute_from_el (animated, SVG_ATTR_from) &&
               get_attribute_from_el (animated, SVG_ATTR_to_))
        {
          TtaAddAnimMotionFromTo ((void *) ParseFromToDataAttribute (get_attribute_from_el (animated, 
                                                                                            SVG_ATTR_from), 
                                                                     get_attribute_from_el (animated, 
                                                                                            SVG_ATTR_to_), 
                                                                     animated,
                                                                     doc),
                                  anim_info);
        }
      else if (get_attribute_from_el (animated, SVG_ATTR_path_))
        {
          TtaAddAnimMotionPath ((void *) ParsePathDataAttribute (get_attribute_from_el (animated, 
                                                                                        SVG_ATTR_path_), 
                                                                 animated, 
                                                                 doc, 
                                                                 FALSE),
                                anim_info);
        }
      TtaSetAnimTypetoMotion (anim_info);
      break;      

    case SVG_EL_animateColor :
      TtaAddAnimAttrName ((void *) get_char_attribute_from_el (animated, SVG_ATTR_attributeName_),
                          anim_info);
      TtaAddAnimFrom ((void *) get_char_attribute_from_el (animated, SVG_ATTR_from), 
                      anim_info);
      TtaAddAnimTo ((void *) get_char_attribute_from_el (animated, SVG_ATTR_to_), 
                    anim_info);
      TtaSetAnimTypetoColor (anim_info);
      break;      

    case SVG_EL_animateTransform :
      val = (long int) get_intptr_attribute_from_el (animated, SVG_ATTR_type_);
      TtaAddAnimAttrName ((void *) val, anim_info);
      TtaAddAnimFrom ((void *) get_char_attribute_from_el (animated, SVG_ATTR_from), 
                      anim_info);
      TtaAddAnimTo ((void *) get_char_attribute_from_el (animated, SVG_ATTR_to_), 
                    anim_info);
      TtaSetAnimTypetoTransform (anim_info);
      break;

    case SVG_EL_animate : 
      TtaAddAnimAttrName ((void *) get_char_attribute_from_el (animated, SVG_ATTR_attributeName_), 
                          anim_info);
      TtaAddAnimFrom ((void *) get_char_attribute_from_el (animated, SVG_ATTR_from), 
                      anim_info);
      TtaAddAnimTo ((void *) get_char_attribute_from_el (animated, SVG_ATTR_to_), 
                    anim_info);
      TtaSetAnimTypetoAnimate (anim_info);
      break;
    case SVG_EL_set_ : 	
      TtaAddAnimAttrName ((void *) get_char_attribute_from_el (animated, SVG_ATTR_attributeName_), 
                          anim_info);
      TtaAddAnimTo ((void *) get_char_attribute_from_el (animated, SVG_ATTR_to_), 
                    anim_info);
      TtaSetAnimTypetoSet (anim_info);
      break;
    default:
      TtaFreeAnimation (anim_info);
      return;
    }
  TtaAppendAnim (animated, anim_info);
#endif /* _SVG */
}
