
/*
 *
 *  (c) COPYRIGHT INRIA and W3C, 2002-2003
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
#define THOT_EXPORT
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


#ifdef _GL
static Pixmap   iconAnim;
static Pixmap   iconAnimNo;
static int      AnimButton;

#ifndef _WINDOWS

#include "xpm/animplay.xpm"
#include "xpm/animstop.xpm"

#else /* _WINDOWS */

#include "wininclude.h"
#define iconAnim 29
#define iconAnimNo 29
#endif /*_WINDOWS*/
#endif /*_GL*/

/*----------------------------------------------------------------------
  SwitchIconAnimPlay
  ----------------------------------------------------------------------*/
void SwitchIconAnimPlay (Document doc, View view, ThotBool state)
{
#ifdef _GL
  if (state)
    TtaChangeButton (doc, view, AnimButton, (ThotIcon)iconAnim, state);
  else
    TtaChangeButton (doc, view, AnimButton, (ThotIcon)iconAnimNo, state);
#endif /* _GL */
}

#ifdef _GL
/*----------------------------------------------------------------------
  AnimPlay: Launch the timer needed to change time.
  ----------------------------------------------------------------------*/
void Anim_Play (Document document, View view)
{
 TtaPlay (document, view);
}
#endif /*_GL*/

/*----------------------------------------------------------------------
  AddAnimPlayButton: Add a play button
  ----------------------------------------------------------------------*/
void AddAnimPlayButton (Document doc, View view)
{
#ifdef _GL
#ifndef _WINDOWS
  iconAnim = TtaCreatePixmapLogo (animplay_xpm);
  iconAnimNo = TtaCreatePixmapLogo (animstop_xpm);
#endif /* _WINDOWS */
  AnimButton = TtaAddButton (doc,
 			     1,  
			     (ThotIcon)iconAnim,
			     Anim_Play,
			     "Anim_Play",
			     TtaGetMessage (AMAYA, AM_BUTTON_ANIM),
			     TBSTYLE_BUTTON, TRUE);
#endif /*_GL*/
}

/*----------------------------------------------------------------------
  get_int_attribute_from_el: Get a int value from an xml attribute
  ----------------------------------------------------------------------*/
static int get_int_attribute_from_el (Element el, int Attribut_Type)
{
#ifdef _SVGANIM
  AttributeType attrType;
  Attribute attr = NULL;
  ElementType elType = TtaGetElementType (el);
  int result;
  
  attrType.AttrSSchema = elType.ElSSchema;
  attrType.AttrTypeNum = Attribut_Type;  
  attr = TtaGetAttribute (el, attrType);
  result = TtaGetAttributeValue (attr);
  
  return result;

#endif /* _SVGANIM */
}
/*----------------------------------------------------------------------
  get_int_attribute_from_el: Get a int value from an xml attribute
  ----------------------------------------------------------------------*/
static Attribute get_attribute_from_el (Element el, int Attribut_Type)
{
#ifdef _SVGANIM
  AttributeType attrType;
  Attribute attr = NULL;
  ElementType elType = TtaGetElementType (el);
  
  attrType.AttrSSchema = elType.ElSSchema;
  attrType.AttrTypeNum = Attribut_Type;  
  attr = TtaGetAttribute (el, attrType);
  
  return attr;

#endif /* _SVGANIM */
}
/*----------------------------------------------------------------------
  get_intptr_attribute_from_el: Get a int value from an xml attribute,
  but stored as an allocated ptr
  ----------------------------------------------------------------------*/
static int *get_intptr_attribute_from_el (Element el, int Attribut_Type)
{
#ifdef _SVGANIM
  AttributeType attrType;
  Attribute attr = NULL;
  ElementType elType = TtaGetElementType (el);
  int *result;
  
  result = TtaGetMemory (sizeof (int));
  attrType.AttrSSchema = elType.ElSSchema;
  attrType.AttrTypeNum = Attribut_Type;  
  attr = TtaGetAttribute (el, attrType);
  *result = TtaGetAttributeValue (attr);
  
  return result;

#endif /* _SVGANIM */
}

/*----------------------------------------------------------------------
  get_char_attribute_from_el: Get a string 
  value from an xml attribute, but stored as an allocated ptr
  ----------------------------------------------------------------------*/
char *get_char_attribute_from_el (Element el, int Attribut_Type)
{
#ifdef _SVGANIM
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
  text = TtaGetMemory (length+1);
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
#endif /* _SVGANIM */
}


/*----------------------------------------------------------------------
  register_animated_element: store animation linked list in an element 
  ----------------------------------------------------------------------*/
void register_animated_element (Element animated, Document doc)
{
#ifdef _SVGANIM
  void *anim_info;
  ElementType elType;
  double start, duration;
  char *repeatcount;  
  int repeat;

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
	TtaAddAnimPath ((void *) ParsePathDataAttribute (get_attribute_from_el (animated, 
										SVG_ATTR_path_), 
							 animated, 
							 doc, 
							 FALSE),
			anim_info);
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
      TtaAddAnimAttrName ((void *) get_intptr_attribute_from_el (animated, SVG_ATTR_type_), 
			  anim_info);
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
      TtaSetAnimTypetoSet (anim_info);
      break;
    }
  TtaAppendAnim (animated, anim_info);
#endif /* _SVGANIM */
}
