/*
 *
 *  (c) COPYRIGHT INRIA and W3C, 2002-2010
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * This module contains browsing & editing functions to handle SVG animations.
 *
 * Author: Pierre Geneves
 *	  
 */

#ifdef _WX
#include "wx/wx.h"
#endif /* _WX */

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

#ifdef _SVG
#include "Timeline.h"
#ifdef _WINDOWS
#include <commctrl.h>
#endif /* _WINDOWS */
#ifdef _WINGUI
#include "wininclude.h"
#endif /* _WINGUI */
#if defined(_UNIX) || defined(_WINDOWS)
#define max(a,b)	((a) > (b) ? (a) : (b))
#define min(a,b)	((a) < (b) ? (a) : (b))
#endif /* _UNIX || _WINDOWS */


#ifdef _WX
#include "appdialogue_wx.h"
#endif /* _WX */

/* code convention : 
   basedoc : edited document
   timelinedoc : timeline document
*/


#define ct_empty_string ""

/* path to help */
#define ct_timeline_help_file "timeline_help.html"

#ifndef _GL
#define ct_image_slider_filename "slider.gif"
#else /*_GL*/
#define ct_image_slider_filename "slidergl.gif"
#endif /*_GL*/
/* path to images used in the interface */
#define ct_image_c1_filename "collapse1.png"
#define ct_image_c2_filename "collapse2.png"
#define ct_image_toolbar "timelinetoolbar_sh4.png"
#define ct_image_anim_color "anim_color_sh.png"
#define ct_image_anim_motion "anim_motion_sh.png"
#define ct_image_anim_help "anim_help_sh.png"
#define ct_image_anim_color_disabled "anim_color_sh_db.png"
#define ct_image_anim_motion_disabled "anim_motion_sh_db.png"
#define ct_image_cross "timeline_cross.gif"
#define ct_image_arrow "timeline_arrow.gif"

/* width & height of these images */
#define ct_w_image_collapse 9
#define ct_h_image_collapse 9
#define ct_w_image_slider 11
#define ct_h_image_slider 1200
#define ct_w_image_toolbar 98
#define ct_h_image_toolbar 31

#define ct_w_image_color 26
#define ct_h_image_color 26
#define ct_w_image_motion 26
#define ct_h_image_motion 26
#define ct_w_image_help 26
#define ct_h_image_help 26

#define ct_w_image_cross 21
#define ct_h_image_cross 21
#define ct_w_image_arrow 16
#define ct_h_image_arrow 16



/* timeline settings */
#define ct_left_buttons 2
#define ct_sep_buttons -2
#define ct_top_buttons 8
#define ct_top_toolbar 4
#define ct_vscrollbar_w 32
#define ct_top 14
#define ct_top_bar 48
#define ct_left_bar 190
#define ct_sep_bar 5

/* line settings */
#define ct_graduation_height 5
#define ct_graduation_font_family "Verdana"
#define ct_graduation_font_size 9

/* animated elements settings */
#define ct_elem_font_family "Verdana"
#define ct_elem_font_size 12


/* title groups settings */
#define ct_default_title_group_height 28
#define ct_default_title_group_width 180


/* expanded groups settings */
#define ct_static_bar_height 9
#define ct_static_bar_color "white"
#define ct_expanded_group_fill_color "silver"

/* collapsed groups settings */
#define ct_collapsed_group_height 28
#define ct_collapsed_anim_color "#8F8D8F" /* gray */

/* period settings */
#define ct_offset_y_period 1
#define ct_period_height 8


/* Colors of each type of animation */
#define ct_animate_color "#FFF120" /* yellow */
#define ct_set_color "#FF585F" /* red */
#define ct_animatemotion_color "#4FF54F" /* green */
#define ct_animatecolor_color "#5078EF" /* blue */
#define ct_animatetransform_color "#37D1C0" 


/* by default, a document that contains no animation
   is shown with a timeline of 10s */
#define ct_default_anim_period_of_doc 10

/* maximum number of animated elements handled */
#define MAX_ANIMATED_ELTS 999

/* maximum number of animations handled for a specific element */
#define MAX_ANIMATIONS 99

/* Forbid the enlargement of timeline of more than 1000s at a time, 
   useful to prevent Amaya from crashing when the user enters
   a very big value for slider position */
#define MAX_ENLARGE_TIMELINE 1000


/* mapping between <animation..> elements 
   of the edited document and elements representing
   animation in the timeline document */
typedef struct 
{
  Element animation_tag;    /* animation tag of the edited document */
  Element period;	    /* element on timeline representing an animation tag */
} tmapping_animations;


/* mapping between animated elements 
   of the edited document and elements representing
   animated elements in the timeline document*/
typedef struct 
{
  Element animated_elem;  /* animated element of the base document */
  Element title_group;    /* title group of the animated element on the timeline */
  Element exp_group; /* expanded group of animated element on the timeline */
  Element col_group;      /* collapsed group of animated element on the timeline,
                             image_collapse is the second child of col_group */
  int nb_periods;			/* number of periods in expanded_group */
  tmapping_animations mapping_animations[MAX_ANIMATIONS];
  
} tmapping_animated;
typedef tmapping_animated* pmapping_animated;


/* Mapping between opened documents in Amaya 
   and their timeline documents */
typedef struct
{
  Document timelinedoc; /* timeline document of document */
  View timelineView;    /* view of timeline */
  Document helpdoc; /* help */
  View helpview;

  /* mapping */
  tmapping_animated mapping_animated[MAX_ANIMATED_ELTS];
  int nb_relations_animated;

  /* shorcuts to elements of timelinedoc*/
  Element current_el;
  Element timing_text;
  int vertical_pos;
	
  Element slider;

  Element line1;
  Element line2;
  Element line3;
  Element line4;
  Element line5;

  Element image_color;
  Element image_motion;
  Element image_interp;

  ThotBool motion_animation_creation_enabled;
  ThotBool color_animation_creation_enabled;

  ThotBool definition_of_motion_period;
  ThotBool definition_of_color_period;
  
  Element cross; /* cross in basedoc used to define motion */
  Element arrow; /* arrow in timelinedoc */

  pmapping_animated current_edited_mapping;
  ThotBool can_delete_status_text;

  int bgcolor_start;
  int fgcolor_start;

  double anim_start;

  int previous_x;
  int previous_y;
  int x;
  int y;

} TTimeline;


/* timeline documents of documents, "dt" is for "document_timeline" */
static TTimeline dt[DocumentTableLength];


static double time_sep; /* pixels */

static int current_timeline_width; /* pixels */
static double current_timeline_end; /* end timing (s) */

static int tmp_previous_x;
static int previous_slider_position;

void Set_slider_position_from_doc(Document doc, double curtime);

#endif /* _SVG */



/*----------------------------------------------------------------------
  Build_path_to_image_dir
  Returns path to directory where images of the interface are stored
  ----------------------------------------------------------------------*/
#ifdef _SVG
static void Build_path_to_image_dir(char* buffer) 
{
  sprintf (buffer, "%s%camaya%c", TtaGetEnvString ("THOTDIR"), DIR_SEP, DIR_SEP);
}
#endif /* _SVG */



/*----------------------------------------------------------------------
  Build_path_to_help_dir
  Returns path to directory where images of the interface are stored
  ----------------------------------------------------------------------*/
#ifdef _SVG
static void Build_path_to_help_dir(char* buffer) 
{
  sprintf (buffer, "%s%cdoc%chtml%c", TtaGetEnvString ("THOTDIR"), DIR_SEP,
           DIR_SEP, DIR_SEP);
}
#endif /* _SVG */



#if 0 /* these functions seems to not being used anymore */
/*----------------------------------------------------------------------
  Init_tmapping_animations_struct
  ----------------------------------------------------------------------*/
#ifdef _SVG
static void Init_tmapping_animations_struct(tmapping_animations * m) 
{
  m->animation_tag = NULL;
  m->period = NULL;
}
#endif /* _SVG */

/*----------------------------------------------------------------------
  Init_tmapping_animated_struct
  ----------------------------------------------------------------------*/
#ifdef _SVG
static void Init_tmapping_animated_struct(tmapping_animated * m) 
{
  m->animated_elem = NULL;
  m->title_group = NULL;
  m->exp_group = NULL;
  m->col_group = NULL;
  m->nb_periods = 0;
}
#endif /* _SVG */
#endif /* 0 */


/*----------------------------------------------------------------------
  Init_timeline_struct
  initializes a TTimeline structure
  ----------------------------------------------------------------------*/
#ifdef _SVG
static void Init_timeline_struct(int k) 
{
  // reset all the structure fields
  memset(&dt[k], 0, sizeof(TTimeline));

  // then only set the none zero fields
  dt[k].vertical_pos = ct_top_bar;
}
#endif /* _SVG */


/*----------------------------------------------------------------------
  Time_to_xposition
  Convert a clock value to x position (pixels)
  ----------------------------------------------------------------------*/
#ifdef _SVG
static int Time_to_xposition(double t) 
{
  return ct_left_bar + 1 + (int)(t * time_sep);
}
#endif /* _SVG */


/*----------------------------------------------------------------------
  Get_timeline_of_doc
  Returns timeline document and its view of a document (export to HTMLactions.c)
  ----------------------------------------------------------------------*/
void Get_timeline_of_doc(Document basedoc, Document *timelinedoc, View *timelineview)
{
#ifdef _SVG
  *timelinedoc = dt[basedoc].timelinedoc;
  *timelineview = dt[basedoc].timelineView;
#endif /* _SVG */
}


/*----------------------------------------------------------------------
  Free_timeline_of_doc
  ----------------------------------------------------------------------*/
void Free_timeline_of_doc(Document doc)
{
#ifdef _SVG
  Init_timeline_struct(doc);
  FreeDocumentResource (dt[doc].timelinedoc);
#endif /* _SVG */
}


/*----------------------------------------------------------------------
  InitSVGAnim 
  initializes SVGAnim context, called in InitAmaya()
  ----------------------------------------------------------------------*/
void InitSVGAnim ()
{
#ifdef _SVG
  int k;

#ifdef _GL
  TtaRegisterTimeEvent (&Set_slider_position_from_doc);
#endif /* _GL */
  for (k = 0; k < DocumentTableLength; k++)
    Init_timeline_struct(k);
#endif /* _SVG */
}


/* Following functions deal with mappings */
/*----------------------------------------------------------------------
  Get_basedoc_of
  Returns the base document of a timeline document
  ----------------------------------------------------------------------*/
#ifdef _SVG
static Document Get_basedoc_of (Document timelinedoc) 
{
  Document k = 0;

  while (k < DocumentTableLength && dt[k].timelinedoc != timelinedoc)
    k++;
  return k;
}
#endif /* _SVG */


/*----------------------------------------------------------------------
  Get_mapping_from_title_group
  ----------------------------------------------------------------------*/
#ifdef _SVG
static pmapping_animated Get_mapping_from_title_group (Document basedoc, Element tg) 
{
  pmapping_animated res = NULL;
  int               k = 0, trouve = 0;

  while (!trouve && k < dt[basedoc].nb_relations_animated)
    if (dt[basedoc].mapping_animated[k].title_group == tg) 
      {
        trouve = 1;
        res = &(dt[basedoc].mapping_animated[k]);
      } 
    else 
      k++;

  return res;
}
#endif /* _SVG */


/*----------------------------------------------------------------------
  Get_mapping_from_exp_group
  ----------------------------------------------------------------------*/
#ifdef _SVG
static pmapping_animated Get_mapping_from_exp_group (Document basedoc, Element eg) 
{
  pmapping_animated res = NULL;
  int               k = 0, trouve = 0;

  while ((!trouve) && (k<dt[basedoc].nb_relations_animated))
    if (dt[basedoc].mapping_animated[k].exp_group==eg) 
      {
        trouve = 1;
        res = &(dt[basedoc].mapping_animated[k]);
      }
    else 
      k++;

  return res;
}
#endif /* _SVG */


/*----------------------------------------------------------------------
  Get_mapping_from_animated_elem
  ----------------------------------------------------------------------*/
#ifdef _SVG
static pmapping_animated Get_mapping_from_animated_elem (Document basedoc, Element ae) 
{
  pmapping_animated res = NULL;
  int               k = 0, trouve = 0;

  while ((!trouve) && (k<dt[basedoc].nb_relations_animated))
    if (dt[basedoc].mapping_animated[k].animated_elem==ae) 
      {
        trouve = 1;
        res = &(dt[basedoc].mapping_animated[k]);
      }
    else 
      k++;

  return res;
}
#endif /* _SVG */


/*----------------------------------------------------------------------
  Get_title_group_of_animated_element
  use mapping between elements to return title group
  ----------------------------------------------------------------------*/
#ifdef _SVG
static Element Get_title_group_of_animated_element (Document basedoc, Element an) 
{
  Element res = NULL;
  int     k = 0, trouve=0;

  while ((!trouve) && (k<dt[basedoc].nb_relations_animated)) 
    if (dt[basedoc].mapping_animated[k].animated_elem==an) 
      {
        trouve = 1;
        res = dt[basedoc].mapping_animated[k].title_group;
      }
    else 
      k++;
  return res;
}
#endif /* _SVG */


/*----------------------------------------------------------------------
  Get_animated_element_of_col_group
  use mapping between elements to return animated element
  ----------------------------------------------------------------------*/
#ifdef _SVG
static Element Get_animated_element_of_col_group (Document basedoc, Element cp)
{
  Element res = NULL;
  int     k = 0, trouve = 0;

  while ((!trouve) && (k<dt[basedoc].nb_relations_animated)) 
    if (dt[basedoc].mapping_animated[k].col_group==cp) 
      {
        trouve = 1;
        res = dt[basedoc].mapping_animated[k].animated_elem;
      }
    else 
      k++;
  return res;
}
#endif /* _SVG */


/*----------------------------------------------------------------------
  Is_animated
  returns 1 iff el has one or more animation
  ----------------------------------------------------------------------*/
#ifdef _SVG
static int Is_animated (Document basedoc, Element el) 
{
  int res = 0;
  int k = 0;

  while ((!res) && (k<dt[basedoc].nb_relations_animated)) 
    if (dt[basedoc].mapping_animated[k].animated_elem==el) {
      res = 1;
    } 
    else 
      k++;

  return res;
}
#endif /* _SVG */


/*----------------------------------------------------------------------
  Set_expanded_group_of
  maps newly created expanded group
  ----------------------------------------------------------------------*/
#ifdef _SVG
static void Set_expanded_group_of (Document basedoc, Element element_anime,
                                   Element new_fresh_group) 
{
  int k = 0, trouve = 0;
	
  while ((!trouve) && (k<dt[basedoc].nb_relations_animated)) 
    if (dt[basedoc].mapping_animated[k].animated_elem==element_anime) 
      {
        trouve = 1;
        dt[basedoc].mapping_animated[k].exp_group = new_fresh_group;
      }
    else 
      k++;
}
#endif /* _SVG */

/*----------------------------------------------------------------------
  Set_collapsed_group_of
  maps newly created collapsed group
  ----------------------------------------------------------------------*/
#ifdef _SVG
static void Set_collapsed_group_of (Document basedoc, Element element_anime,
                                    Element new_fresh_group) 
{
  int k = 0, trouve=0;

  while ((!trouve) && (k<dt[basedoc].nb_relations_animated)) 
    if (dt[basedoc].mapping_animated[k].animated_elem==element_anime) 
      {
        trouve = 1;
        dt[basedoc].mapping_animated[k].col_group = new_fresh_group;
      }
    else 
      k++;
}
#endif /* _SVG */


/*----------------------------------------------------------------------
  Get_animation_tag_from_period
  ----------------------------------------------------------------------*/
#ifdef _SVG
static Element Get_animation_tag_from_period (Document basedoc,
                                              Element exp_group, Element pr) 
{
  Element res = NULL;
  int     k = 0, j = 0, trouve_g = 0, trouve_p = 0;

  while ((!trouve_g) && (k<dt[basedoc].nb_relations_animated)) 
    if (dt[basedoc].mapping_animated[k].exp_group==exp_group) 
      {

        trouve_g = 1;	
        while ((!trouve_p) && (j<dt[basedoc].mapping_animated[k].nb_periods))
          if (dt[basedoc].mapping_animated[k].mapping_animations[j].period==pr)
            {
              trouve_p = 1;
              res = dt[basedoc].mapping_animated[k].mapping_animations[j].animation_tag;
            }
          else j++;

      }
    else 
      k++;
  return res;
}
#endif /* _SVG */


/*----------------------------------------------------------------------
  Get_period_from_animation_tag
  returns timeline interface period of associated animation tag
  ----------------------------------------------------------------------*/
#ifdef _SVG
static Element Get_period_from_animation_tag (pmapping_animated pm,
                                              Element animtag) 
{
  Element res = NULL;
  int     j = 0, trouve_a = 0;
	
  while ((!trouve_a) && (j<pm->nb_periods))
    if (pm->mapping_animations[j].animation_tag==animtag) 
      {
        trouve_a= 1;
        res = pm->mapping_animations[j].period;
      }
    else j++;
  return res;
}
#endif /* _SVG */


/*----------------------------------------------------------------------
  TimelineParseColorAttribute
  Create or update a specific presentation rule for element el that reflects
  the value of attribute attr, which is equivalent to a CSS property (fill,
  stroke).
  ----------------------------------------------------------------------*/
#ifdef _SVG
static void TimelineParseColorAttribute (int attrType, Attribute attr, Element el,
                                         Document doc, ThotBool delete_)
{
#define buflen 200
  char               css_command[buflen+20];
  int                length;
  char               *text;

  text = NULL;
  /* the attribute value is a character string */
  length = TtaGetTextAttributeLength (attr) + 2;
  text = (char *)TtaGetMemory (length);
  if (text != NULL)
    TtaGiveTextAttributeValue (attr, text, &length);

  /* builds the equivalent CSS rule */
  css_command[0] = EOS;
  switch (attrType)
    {
    case Timeline_ATTR_fill:
      sprintf (css_command, "fill: %s", text);
      break;
    case Timeline_ATTR_stroke:
      sprintf (css_command, "stroke: %s", text);
      break;
    case Timeline_ATTR_font_family:
      sprintf (css_command, "font-family: %s", text);
      break;
    case Timeline_ATTR_font_size:
      sprintf (css_command, "font-size: %s", text);
      break;
    default:
      break;
    }

  /* parse the equivalent CSS rule */
  if (css_command[0] != EOS)
    ParseHTMLSpecificStyle (el, css_command, doc, 2000, delete_);
  if (text)
    TtaFreeMemory (text);
}
#endif /* _SVG */


/*----------------------------------------------------------------------
  TimelineParseCoordAttribute
  Create or update a specific presentation rule for element el that reflects
  the value of the x, y, x1, x2, y1, y2 attribute attr.
  ----------------------------------------------------------------------*/
#ifdef _SVG
static void TimelineParseCoordAttribute (Attribute attr, Element el, Document doc)
{
  int                  length, attrKind, ruleType;
  char                *text, *ptr;
  AttributeType        attrType;
  PresentationValue    pval;
  PresentationContext  ctxt;

  length = TtaGetTextAttributeLength (attr) + 2;
  text = (char *)TtaGetMemory (length);
  if (text != NULL)
    {
      /* get the value of the x or y attribute */
      TtaGiveTextAttributeValue (attr, text, &length);
      /* parse the attribute value (a number followed by a unit) */
      ptr = text;
      ptr = (char*)TtaSkipBlanks (ptr);
      ptr = ParseCSSUnit (ptr, &pval);
      if (pval.typed_data.unit != UNIT_INVALID)
        {
          if (pval.typed_data.unit == UNIT_BOX)
            pval.typed_data.unit = UNIT_PX;
          pval.typed_data.mainValue = TRUE;
          /* decide of the presentation rule to be created or updated */
          TtaGiveAttributeType (attr, &attrType, &attrKind);
          if (attrType.AttrTypeNum == Timeline_ATTR_x)
            ruleType = PRHorizPos;
          else if (attrType.AttrTypeNum == Timeline_ATTR_y)
            ruleType = PRVertPos;
          else if (attrType.AttrTypeNum == Timeline_ATTR_x1)
            ruleType = PRHorizPos;
          else if (attrType.AttrTypeNum == Timeline_ATTR_y1)
            ruleType = PRVertPos;
          else if (attrType.AttrTypeNum == Timeline_ATTR_x2)
            ruleType = PRWidth;
          else if (attrType.AttrTypeNum == Timeline_ATTR_y2)
            ruleType = PRHeight;
          else
            return;
          ctxt = TtaGetSpecificStyleContext (doc);
          /* the specific presentation is not a CSS rule */
          ctxt->cssSpecificity = 2000;
          ctxt->important = TRUE;
          ctxt->destroy = FALSE;
          TtaSetStylePresentation (ruleType, el, NULL, ctxt, pval);
          TtaFreeMemory (ctxt);
        }
      TtaFreeMemory (text);
    }
}
#endif /* _SVG */


/*----------------------------------------------------------------------
  TimelineParseWidthHeightAttribute
  Create or update a specific presentation rule for element el that reflects
  the value of attribute attr, which is width_, height_.
  ----------------------------------------------------------------------*/
#ifdef _SVG
static ThotBool TimelineParseWidthHeightAttribute (Attribute attr, Element el,
                                                   Document doc, 
                                                   ThotBool delete_)
{
  ThotBool             ret = FALSE;
  AttributeType	       attrType;
  ElementType          elType;
  Element              child;
  int		       length, attrKind, ruleType;
  char		      *text, *ptr;
  PresentationValue    pval;
  PresentationContext  ctxt;

  ret = FALSE; /* let Thot perform normal operation */
  elType = TtaGetElementType (el);
  text = NULL;
  if (attr && !delete_)
    {
      length = TtaGetTextAttributeLength (attr) + 2;
      text = (char *)TtaGetMemory (length);
      if (!text)
        return ret;
    }
  ctxt = TtaGetSpecificStyleContext (doc);
  /* the specific presentation is not a CSS rule */
  ctxt->cssSpecificity = 2000;
  ctxt->important = TRUE;
  ctxt->destroy = FALSE;
  /* decide of the presentation rule to be created or updated */
  TtaGiveAttributeType (attr, &attrType, &attrKind);
  if (attrType.AttrTypeNum == Timeline_ATTR_width_)
    ruleType = PRWidth;
  else if (attrType.AttrTypeNum == Timeline_ATTR_height_)
    ruleType = PRHeight;   
  else
    ruleType = PRWidth;
  if (delete_)
    /* attribute deleted */
    if (ruleType != PRXRadius && ruleType != PRYRadius)
      /* attribute madatory. Do not delete */
      ret = TRUE;
    else
      {
        /* that's the radius of a rounded corner. Get the graphics leaf
           which has the specific presentation rule to be removed */
        child = TtaGetFirstChild (el);
        while (child &&
               TtaGetElementType(child).ElTypeNum != GRAPHICS_UNIT)
          TtaNextSibling (&child);
        pval.typed_data.value = 0;
        pval.typed_data.unit = UNIT_PX;
        ctxt->important = TRUE;
        ctxt->destroy = FALSE;
        TtaSetStylePresentation (ruleType, child, NULL, ctxt, pval);
        ctxt->destroy = TRUE;
        TtaSetStylePresentation (ruleType, child, NULL, ctxt, pval);
        ret = FALSE; /* let Thot perform normal operation */
      }
  else
    /* attribute created or modified */
    {
      /* get the value of the attribute */
      TtaGiveTextAttributeValue (attr, text, &length); 
      /* parse the attribute value (a number followed by a unit) */
      ptr = text;
      ptr = (char*)TtaSkipBlanks (ptr);
      ptr = ParseCSSUnit (ptr, &pval);
      if (pval.typed_data.unit != UNIT_INVALID)
        {
          if (pval.typed_data.unit == UNIT_BOX)
            pval.typed_data.unit = UNIT_PX;
          if (ruleType != PRXRadius && ruleType != PRYRadius)
            /* it's not attribute ry or ry for a rectangle */
            {
              if (pval.typed_data.value == 0)
                /* a value of 0 disables rendering of this element */
                ruleType = PRVisibility;
              else
                {
                  /* if there was a value of 0 previously, enable rendering */
                  ctxt->destroy = TRUE;
                  TtaSetStylePresentation (PRVisibility, el, NULL, ctxt, pval);
                  ctxt->destroy = FALSE;
                }
            }
	
          /* set the specific presentation value of the box */
          if (el)
            TtaSetStylePresentation (ruleType, el, NULL, ctxt, pval);
        }
      if (text)
        TtaFreeMemory (text);
    }
  TtaFreeMemory (ctxt);
  return ret;
}
#endif /* _SVG */


/*----------------------------------------------------------------------
  Insert_rectangle 
  primitive to generate Timeline Element rect via thot api
  ----------------------------------------------------------------------*/
#ifdef _SVG
static Element Insert_rectangle(Document timelinedoc, Element root,
                                const char* fill_color, const char* stroke_color,
                                int x, int y, int w, int h, int Insert_enfant,
                                int typeNum) 
{
  Element              el = NULL;
  AttributeType        attrType;
  Attribute            attr;
  Element              child;
  ElementType          elType,childType;
  char		       buffer[512];

  elType = TtaGetElementType (root);
  elType.ElTypeNum = typeNum;
  el=TtaNewElement (timelinedoc,elType);

  attrType.AttrSSchema = elType.ElSSchema;

  /* fill color */
  attrType.AttrTypeNum = Timeline_ATTR_fill;
  attr = TtaNewAttribute (attrType);
  TtaAttachAttribute (el, attr, timelinedoc);
  TtaSetAttributeText (attr, fill_color, el, timelinedoc);	
  TimelineParseColorAttribute (attrType.AttrTypeNum, attr, el,
                               timelinedoc, FALSE);

  /* stroke color */
  attrType.AttrTypeNum = Timeline_ATTR_stroke;
  attr = TtaNewAttribute (attrType);
  TtaAttachAttribute (el, attr, timelinedoc);
  TtaSetAttributeText (attr, stroke_color, el, timelinedoc);	
  TimelineParseColorAttribute (attrType.AttrTypeNum, attr, el,
                               timelinedoc, FALSE); 

  /* Position */
  attrType.AttrTypeNum = Timeline_ATTR_x;
  attr = TtaNewAttribute (attrType);
  TtaAttachAttribute (el, attr, timelinedoc);
  sprintf (buffer, "%dpx", x);
  TtaSetAttributeText (attr, buffer, el, timelinedoc);
  TimelineParseCoordAttribute (attr, el, timelinedoc);

  attrType.AttrTypeNum = Timeline_ATTR_y;
  attr = TtaNewAttribute (attrType);
  TtaAttachAttribute (el, attr, timelinedoc);
  sprintf (buffer, "%dpx", y);
  TtaSetAttributeText (attr, buffer, el, timelinedoc);
  TimelineParseCoordAttribute (attr, el, timelinedoc);
	
  /* Size */
  attrType.AttrTypeNum = Timeline_ATTR_width_;
  attr = TtaNewAttribute (attrType);
  TtaAttachAttribute (el, attr, timelinedoc);
  sprintf (buffer, "%dpx", w);
  TtaSetAttributeText (attr, buffer, el, timelinedoc);
  TimelineParseWidthHeightAttribute (attr, el, timelinedoc,FALSE);

  attrType.AttrTypeNum = Timeline_ATTR_height_;
  attr = TtaNewAttribute (attrType);
  TtaAttachAttribute (el, attr, timelinedoc);
  sprintf (buffer, "%dpx", h);
  TtaSetAttributeText (attr, buffer, el, timelinedoc);
  TimelineParseWidthHeightAttribute (attr, el, timelinedoc,FALSE);

  /* insert */
  if (Insert_enfant)
    TtaInsertFirstChild (&el,root,timelinedoc);
  else
    TtaInsertSibling (el,root,FALSE,timelinedoc);

  /* Thot api structure compatibility */
  childType.ElSSchema = elType.ElSSchema;
  childType.ElTypeNum = Timeline_EL_GRAPHICS_UNIT;
  child = TtaNewElement (timelinedoc, childType);
  TtaInsertFirstChild (&child, el, timelinedoc);
  TtaSetGraphicsShape (child, 'C', timelinedoc);

  return el;
}
#endif /* _SVG */


/*----------------------------------------------------------------------
  Insert_tline 
  primitive to generate Timeline Element line via thot api
  ----------------------------------------------------------------------*/
#ifdef _SVG
static Element Insert_tline(Document timelinedoc, Element root, const char* couleur,
                            int x1, int y1, int x2, int y2, int Insert_enfant) 
{
  Element              el = NULL;
  AttributeType        attrType;
  Attribute            attr;
  Element              child;
  ElementType          elType,childType;
  char                 buffer[512];
	
  elType = TtaGetElementType (root);
  elType.ElTypeNum = Timeline_EL_tline;
  el = TtaNewElement (timelinedoc,elType);
  attrType.AttrSSchema = elType.ElSSchema;
  /* Color */
  attrType.AttrTypeNum = Timeline_ATTR_stroke;
  attr = TtaNewAttribute (attrType);
  TtaAttachAttribute (el, attr, timelinedoc);
  TtaSetAttributeText (attr, couleur, el, timelinedoc);	
  TimelineParseColorAttribute (attrType.AttrTypeNum, attr, el,
                               timelinedoc, FALSE);

  /* ext1 */
  attrType.AttrTypeNum = Timeline_ATTR_x1;
  attr = TtaNewAttribute (attrType);
  TtaAttachAttribute (el, attr, timelinedoc);
  sprintf (buffer, "%dpx", x1);
  TtaSetAttributeText (attr, buffer, el, timelinedoc);	
  TimelineParseCoordAttribute (attr, el, timelinedoc);
    
  attrType.AttrTypeNum = Timeline_ATTR_y1;
  attr = TtaNewAttribute (attrType);
  TtaAttachAttribute (el, attr, timelinedoc);
  sprintf (buffer, "%dpx", y1);
  TtaSetAttributeText (attr, buffer, el, timelinedoc);	
  TimelineParseCoordAttribute (attr, el, timelinedoc);
  
  /* ext2*/
  attrType.AttrTypeNum = Timeline_ATTR_x2;
  attr = TtaNewAttribute (attrType);
  TtaAttachAttribute (el, attr, timelinedoc);
  sprintf (buffer, "%dpx", x2);
  TtaSetAttributeText (attr, buffer, el, timelinedoc);	
  TimelineParseCoordAttribute (attr, el, timelinedoc);
    
  attrType.AttrTypeNum = Timeline_ATTR_y2;
  attr = TtaNewAttribute (attrType);
  TtaAttachAttribute (el, attr, timelinedoc);
  sprintf (buffer, "%dpx", y2);
  TtaSetAttributeText (attr, buffer, el, timelinedoc);	
  TimelineParseCoordAttribute (attr, el, timelinedoc);
    
  /* insert */
  if (Insert_enfant)
    TtaInsertFirstChild (&el,root,timelinedoc);
  else
    TtaInsertSibling (el,root,FALSE,timelinedoc);
	
  /* Thot api structure compatibility */
  childType.ElSSchema = elType.ElSSchema;
  childType.ElTypeNum = Timeline_EL_GRAPHICS_UNIT;
  child = TtaNewElement (timelinedoc, childType);
  TtaInsertFirstChild (&child, el, timelinedoc);
  TtaSetGraphicsShape (child, 'g', timelinedoc);

  return el;
}
#endif /* _SVG */


/*----------------------------------------------------------------------
  Insert_text 
  primitive to generate Timeline Element text via thot api
  ----------------------------------------------------------------------*/
#ifdef _SVG
static Element Insert_text(Document timelinedoc, Element root, const char* couleur,
                           const char* text, const char* font_family, int font_size,
                           int x, int y, int Insert_enfant, int type_num) 
{
  Element              el = NULL;
  AttributeType        attrType;
  Attribute            attr;
  Element              child;
  ElementType          elType,childType;
  char		       buffer[512];

  elType = TtaGetElementType (root);
  elType.ElTypeNum = type_num;
  el = TtaNewElement (timelinedoc,elType);
  attrType.AttrSSchema = elType.ElSSchema;

  /* Color */
  attrType.AttrTypeNum = Timeline_ATTR_fill;
  attr = TtaNewAttribute (attrType);
  TtaAttachAttribute (el, attr, timelinedoc);
  TtaSetAttributeText (attr, couleur, el, timelinedoc);	
  TimelineParseColorAttribute (attrType.AttrTypeNum, attr, el,
                               timelinedoc, FALSE);

  /* Position */

  if (type_num!=Timeline_EL_text_id) 
    {
      attrType.AttrTypeNum = Timeline_ATTR_x;
      attr = TtaNewAttribute (attrType);
      TtaAttachAttribute (el, attr, timelinedoc);
      sprintf (buffer, "%dpx", x);
      TtaSetAttributeText (attr, buffer, el, timelinedoc);
      TimelineParseCoordAttribute (attr, el, timelinedoc);

      attrType.AttrTypeNum = Timeline_ATTR_y;
      attr = TtaNewAttribute (attrType);
      TtaAttachAttribute (el, attr, timelinedoc);
      sprintf (buffer, "%dpx", y);
      TtaSetAttributeText (attr, buffer, el, timelinedoc);
      TimelineParseCoordAttribute (attr, el, timelinedoc);
    }

  if (font_family) 
    {
      attrType.AttrTypeNum = Timeline_ATTR_font_family;
      attr = TtaNewAttribute (attrType);
      TtaAttachAttribute (el, attr, timelinedoc);
      TtaSetAttributeText (attr, font_family, el, timelinedoc);
      TimelineParseColorAttribute (attrType.AttrTypeNum, attr, el,
                                   timelinedoc, FALSE);
    }


  if (font_size) 
    {
      attrType.AttrTypeNum = Timeline_ATTR_font_size;
      attr = TtaNewAttribute (attrType);
      TtaAttachAttribute (el, attr, timelinedoc);
      sprintf (buffer, "%d", font_size);
      TtaSetAttributeText (attr, buffer, el, timelinedoc);
      TimelineParseColorAttribute (attrType.AttrTypeNum, attr, el,
                                   timelinedoc, FALSE);
    }
  
  /* insert */
  if (Insert_enfant)
    TtaInsertFirstChild (&el,root,timelinedoc);
  else
    TtaInsertSibling (el,root,FALSE,timelinedoc);
	
  /* Thot api structure compatibility */
  childType.ElSSchema = elType.ElSSchema;
  childType.ElTypeNum = Timeline_EL_TEXT_UNIT;
  child = TtaNewElement (timelinedoc, childType);
  TtaAppendTextContent (child,(unsigned char *)text,timelinedoc);
  TtaInsertFirstChild (&child, el, timelinedoc);

  return el;
}
#endif /* _SVG */


/*----------------------------------------------------------------------
  Insert_image 
  primitive to generate Timeline Element image via thot api
  ----------------------------------------------------------------------*/
#ifdef _SVG
static Element Insert_image(Document timelinedoc, Element root,
                            char* text, int x, int y, int w, int h,
                            int Insert_enfant, int type_num) 
{
  Element              el = NULL;
  AttributeType        attrType;
  Attribute            attr;
  Element              child;
  ElementType          elType, childType;
  char		       buffer[512];
	
  elType = TtaGetElementType (root);
  elType.ElTypeNum = type_num;
  el = TtaNewElement (timelinedoc,elType);
  attrType.AttrSSchema = elType.ElSSchema;

  /* Position */
  attrType.AttrTypeNum = Timeline_ATTR_x;
  attr = TtaNewAttribute (attrType);
  TtaAttachAttribute (el, attr, timelinedoc);
  sprintf (buffer, "%dpx", x);
  TtaSetAttributeText (attr, buffer, el, timelinedoc);
  TimelineParseCoordAttribute (attr, el, timelinedoc);
	
  attrType.AttrTypeNum = Timeline_ATTR_y;
  attr = TtaNewAttribute (attrType);
  TtaAttachAttribute (el, attr, timelinedoc);
  sprintf (buffer, "%dpx", y);
  TtaSetAttributeText (attr, buffer, el, timelinedoc);
  TimelineParseCoordAttribute (attr, el, timelinedoc);

  /* src */
  attrType.AttrTypeNum = Timeline_ATTR_xlink_href;
  attr = TtaNewAttribute (attrType);
  TtaAttachAttribute (el, attr, timelinedoc);
  TtaSetAttributeText (attr, text, el, timelinedoc);

  /* insert */
  if (Insert_enfant)
    TtaInsertFirstChild (&el, root, timelinedoc);
  else
    TtaInsertSibling (el, root, FALSE, timelinedoc);
	
  /* Thot api structure compatibility */
  childType.ElSSchema = elType.ElSSchema;
  childType.ElTypeNum = Timeline_EL_PICTURE_UNIT;
  child = TtaNewElement (timelinedoc, childType);
  TtaInsertFirstChild (&child, el, timelinedoc);
  TtaSetTextContent (child, (unsigned char *)text, SPACE, timelinedoc);
  return el;
}
#endif /* _SVG */


/*----------------------------------------------------------------------
  Display_cross
  primitive to insert a cross in edited document
  ----------------------------------------------------------------------*/
#ifdef _SVG
static void Display_cross(Document basedoc,
                          Element animated_element, int x, int y) 
{
  Element              el = NULL, root, svg;
  AttributeType        attrType;
  Attribute            attr;
  Element              child;
  ElementType          elType, childType;
  char                 buffer[512];
  ThotBool             basedoc_state;

  basedoc_state = TtaIsDocumentModified (basedoc);
  root = TtaGetMainRoot (basedoc);
  elType = TtaGetElementType (root);
  elType.ElSSchema = TtaGetSSchema ("SVG", basedoc);
  elType.ElTypeNum = SVG_EL_Timeline_cross;
  el = TtaNewElement (basedoc,elType);
  attrType.AttrSSchema = elType.ElSSchema;

  /* Position */
  attrType.AttrTypeNum = SVG_ATTR_x;
  attr = TtaNewAttribute (attrType);
  TtaAttachAttribute (el, attr, basedoc);
  sprintf (buffer, "%dpx", x);
  TtaSetAttributeText (attr, buffer, el, basedoc);
  ParseCoordAttribute (attr, el, basedoc);

  attrType.AttrTypeNum = SVG_ATTR_y;
  attr = TtaNewAttribute (attrType);
  TtaAttachAttribute (el, attr, basedoc);
  sprintf (buffer, "%dpx", y);
  TtaSetAttributeText (attr, buffer, el, basedoc);
  ParseCoordAttribute (attr, el, basedoc);

  /* width and height */
  attrType.AttrTypeNum = SVG_ATTR_width_;
  attr = TtaNewAttribute (attrType);
  TtaAttachAttribute (el, attr, basedoc);
  sprintf (buffer, "%dpx", ct_w_image_cross);
  TtaSetAttributeText (attr, buffer, el, basedoc);
  ParseWidthHeightAttribute(attr, el, basedoc, FALSE);

  attrType.AttrTypeNum = SVG_ATTR_height_;
  attr = TtaNewAttribute (attrType);
  TtaAttachAttribute (el, attr, basedoc);
  sprintf (buffer, "%dpx", ct_h_image_cross);
  TtaSetAttributeText (attr, buffer, el, basedoc);
  ParseWidthHeightAttribute (attr, el, basedoc, FALSE);
  
  /* insert */
  elType.ElTypeNum = SVG_EL_SVG;
  svg = TtaSearchTypedElement (elType, SearchInTree, root);
  TtaInsertSibling (el, animated_element, FALSE, basedoc);
  
  /* Thot api structure compatibility */
  childType.ElSSchema = elType.ElSSchema;
  childType.ElTypeNum = SVG_EL_PICTURE_UNIT;
  child = TtaNewElement (basedoc, childType);
  TtaInsertFirstChild (&child, el, basedoc);
	
  /* src */
  Build_path_to_image_dir (buffer);
  strcat (buffer, ct_image_cross);
  TtaSetTextContent (child, (unsigned char *)buffer, SPACE, basedoc);

  dt[basedoc].cross = el;
  if (!basedoc_state)
    TtaSetDocumentUnmodified (basedoc);	
}
#endif /* _SVG */


/*----------------------------------------------------------------------
  Delete_cross
  ----------------------------------------------------------------------*/
#ifdef _SVG
static void Delete_cross (Document basedoc) 
{
  TtaDeleteTree (dt[basedoc].cross, basedoc);
}
#endif /* _SVG */


/*----------------------------------------------------------------------
  Insert_group
  primitive to generate Timeline Element group via thot api
  type_num : Timeline_EL_abstract_group or Timeline_EL_gt
  ----------------------------------------------------------------------*/
#ifdef _SVG
static Element Insert_group(Document timelinedoc, Element root,
                            int Insert_enfant, int type_num) 
{
  Element              el = NULL;
  AttributeType        attrType;
  ElementType          elType;
  
  elType = TtaGetElementType (root);
  elType.ElTypeNum = type_num;
  el = TtaNewElement (timelinedoc,elType);
  attrType.AttrSSchema = elType.ElSSchema;
  /* insert */
  if (Insert_enfant)
    TtaInsertFirstChild (&el,root,timelinedoc);
  else
    TtaInsertSibling (el,root,FALSE,timelinedoc);
	
  return el;
}
#endif /* _SVG */


/*----------------------------------------------------------------------
  Get_id_of
  returns the id of a basedoc Element 
  ----------------------------------------------------------------------*/	
#ifdef _SVG
static int Get_id_of (Element el, char* buffer)
{
  int           length = 0;
  AttributeType attrType;
  Attribute     attr = NULL;
  ElementType   elType = TtaGetElementType (el);
  
  attrType.AttrSSchema = elType.ElSSchema;
  attrType.AttrTypeNum = Timeline_ATTR_id;
  attr = TtaGetAttribute (el, attrType);
  if (attr) 
    {
      length = TtaGetTextAttributeLength (attr);
      if (length) 
        {
          length += 2;
          TtaGiveTextAttributeValue (attr, buffer, &length);
        }
    }
  return (length);
}
#endif /* _SVG */
 

/*----------------------------------------------------------------------
  Get_height_of_SVG_el
  return the height of a SVG Element 
  ----------------------------------------------------------------------*/
#ifdef _SVG
static int Get_height_of_SVG_el (Element el)
{
  int                  r = 0;
  int                  length;
  char                *text = NULL, *ptr;
  PresentationValue    pval;
  AttributeType        attrType;
  Attribute            attr = NULL;
  ElementType          elType = TtaGetElementType (el);
   
  attrType.AttrSSchema = elType.ElSSchema;
  attrType.AttrTypeNum = SVG_ATTR_height_;
  attr = TtaGetAttribute (el, attrType);
  length = TtaGetTextAttributeLength (attr) + 2;
  text = (char *)TtaGetMemory (length);
  if (text) 
    {
      /* get the value of the x attribute */
      TtaGiveTextAttributeValue (attr, text, &length);
      /* parse the attribute value (a number followed by a unit) */
      ptr = text;
      ptr = ParseNumber (ptr, &pval);
      r = pval.typed_data.value;
      TtaFreeMemory (text);
    }

  return r;
}
#endif /* _SVG */


/*----------------------------------------------------------------------
  Get_width_of_SVG_el
  return the width of a SVG Element 
  ----------------------------------------------------------------------*/
#ifdef _SVG
static int Get_width_of_SVG_el (Element el)
{
  int                   r = 0;
  int                  length;
  char                *text = NULL, *ptr;
  PresentationValue    pval;
  AttributeType        attrType;
  Attribute            attr = NULL;
  ElementType          elType = TtaGetElementType (el);
   
  attrType.AttrSSchema = elType.ElSSchema;
  attrType.AttrTypeNum = SVG_ATTR_width_;
  attr = TtaGetAttribute (el, attrType);
  length = TtaGetTextAttributeLength (attr) + 2;
  text = (char *)TtaGetMemory (length);
  if (text) 
    {
      /* get the value of the x attribute */
      TtaGiveTextAttributeValue (attr, text, &length);
      /* parse the attribute value (a number followed by a unit) */
      ptr = text;
      ptr = ParseNumber (ptr, &pval);
      r = pval.typed_data.value;
      TtaFreeMemory (text);
    }

  return r;
}
#endif /* _SVG */


/*----------------------------------------------------------------------
  Get_center_y_of_SVG_el
  returns vertical position of the center of an SVG Element,
  To do: this is incomplete and should handle SVG transformations
  ----------------------------------------------------------------------*/	
#ifdef _SVG
static int Get_center_y_of_SVG_el (Element el)
{
  int                  length, res = 0, r = 0;
  char                *text = NULL, *ptr;
  PresentationValue    pval;
  AttributeType        attrType;
  Attribute            attr = NULL;
  ElementType          elType = TtaGetElementType (el);
   
  attrType.AttrSSchema = elType.ElSSchema;
  attrType.AttrTypeNum = SVG_ATTR_cy;
  attr = TtaGetAttribute (el, attrType);
  /* check for cy */
  if (attr) 
    {
      length = TtaGetTextAttributeLength (attr) + 2;
      text = (char *)TtaGetMemory (length);
   
      if (text) 
        {
          /* get the value of the attribute */
          TtaGiveTextAttributeValue (attr, text, &length);
          /* parse the attribute value (a number followed by a unit) */
          ptr = text;
          ptr = ParseNumber (ptr, &pval);
          res = pval.typed_data.value;
          TtaFreeMemory (text);
        }
    } 
  else 
    { /* no cx, check for y and height */
      attrType.AttrTypeNum = SVG_ATTR_y;
      attr = TtaGetAttribute (el, attrType);
      length = TtaGetTextAttributeLength (attr) + 2;
      text = (char *)TtaGetMemory (length);
      if (text) 
        {
          /* get the value of the attribute */
          TtaGiveTextAttributeValue (attr, text, &length);
          /* parse the attribute value (a number followed by a unit) */
          ptr = text;
          ptr = ParseNumber (ptr, &pval);
          r = pval.typed_data.value;
          TtaFreeMemory (text);
        }

      res = r + Get_height_of_SVG_el (el)/2;
    }

  return res;
}
#endif /* _SVG */


/*----------------------------------------------------------------------
  Get_center_x_of_SVG_el
  returns horizontal position of the center of an SVG Element,
  To do: this is incomplete and should handle SVG transformations
  ----------------------------------------------------------------------*/	
#ifdef _SVG
static int Get_center_x_of_SVG_el (Element el)
{
  int                  length, res = 0, r = 0;
  char                *text = NULL, *ptr;
  PresentationValue    pval;
  AttributeType        attrType;
  Attribute            attr = NULL;
  ElementType          elType = TtaGetElementType (el);
   
  attrType.AttrSSchema = elType.ElSSchema;
  attrType.AttrTypeNum = SVG_ATTR_cx;
  attr = TtaGetAttribute (el, attrType);
  /* check for cx */
  if (attr) 
    {
      length = TtaGetTextAttributeLength (attr) + 2;
      text = (char *)TtaGetMemory (length);
   
      if (text) 
        {
          /* get the value of the attribute */
          TtaGiveTextAttributeValue (attr, text, &length);
          /* parse the attribute value (a number followed by a unit) */
          ptr = text;
          ptr = ParseNumber (ptr, &pval);
          res = pval.typed_data.value;
          TtaFreeMemory (text);
        }
    } 
  else 
    { /* no cx, check for x and width */
      attrType.AttrTypeNum = SVG_ATTR_x;
      attr = TtaGetAttribute (el, attrType);
      length = TtaGetTextAttributeLength (attr) + 2;
      text = (char *)TtaGetMemory (length);
      if (text) 
        {
          /* get the value of the attribute */
          TtaGiveTextAttributeValue (attr, text, &length);
          /* parse the attribute value (a number followed by a unit) */
          ptr = text;
          ptr = ParseNumber (ptr, &pval);
          r = pval.typed_data.value;
          TtaFreeMemory (text);
        }
      res = r + Get_width_of_SVG_el (el)/2;
    }
  return res;
}
#endif /* _SVG */


/*----------------------------------------------------------------------
  Get_y_of
  returns vertical position of a Timeline Element 
  ----------------------------------------------------------------------*/	
#ifdef _SVG
static int Get_y_of (Element el)
{
  int                  res = 0;
  int                  length, r = 0;
  char                *text = NULL, *ptr;
  PresentationValue    pval;
  AttributeType        attrType;
  Attribute            attr = NULL;
  Element              parent = TtaGetParent (el);
  ElementType          elType = TtaGetElementType (el);
   
  attrType.AttrSSchema = elType.ElSSchema;
  attrType.AttrTypeNum = Timeline_ATTR_y;
  attr = TtaGetAttribute (el, attrType);
  length = TtaGetTextAttributeLength (attr) + 2;
  text = (char *)TtaGetMemory (length);
  if (text) 
    {
      /* get the value of the attribute */
      TtaGiveTextAttributeValue (attr, text, &length);
      /* parse the attribute value (a number followed by a unit) */
      ptr = text;
      ptr = ParseNumber (ptr, &pval);
      r = pval.typed_data.value;
      TtaFreeMemory (text);
    }
  res = r;

  /* handle possible translation of parent abstract group: */
  r = 0;
  attrType.AttrTypeNum = Timeline_ATTR_transform;
  attr = TtaGetAttribute (parent, attrType);
  text = NULL;
  if (attr) 
    {
      length = TtaGetTextAttributeLength (attr) + 2;
      text = (char *)TtaGetMemory (length);
      if (text) 
        {
          /* get the value of the attribute */
          TtaGiveTextAttributeValue (attr, text, &length);
          /* parse the attribute value (a number followed by a unit) */
          ptr = text;
          /* skip "translate(0," : */
          ptr += strlen("translate(0,");
          ptr = ParseNumber (ptr, &pval);
          r = pval.typed_data.value;
          TtaFreeMemory (text);
        }	
    }
  res += r;
  return res;
}
#endif /* _SVG */


/*----------------------------------------------------------------------
  Get_height_of
  return the height of a Timeline Element 
  ----------------------------------------------------------------------*/
#ifdef _SVG
static int Get_height_of (Element el)
{
  int                   r = 0;
  int                  length;
  char                *text = NULL, *ptr;
  PresentationValue    pval;
  AttributeType        attrType;
  Attribute            attr = NULL;
  ElementType          elType = TtaGetElementType (el);
   
  attrType.AttrSSchema = elType.ElSSchema;
  attrType.AttrTypeNum = Timeline_ATTR_height_;
  attr = TtaGetAttribute (el, attrType);
  length = TtaGetTextAttributeLength (attr) + 2;
  text = (char *)TtaGetMemory (length);
  if (text) 
    {
      /* get the value of the x attribute */
      TtaGiveTextAttributeValue (attr, text, &length);
      /* parse the attribute value (a number followed by a unit) */
      ptr = text;
      ptr = ParseNumber (ptr, &pval);
      r = pval.typed_data.value;
      TtaFreeMemory (text);
    }
  return r;
}
#endif /* _SVG */


/*----------------------------------------------------------------------
  Inc_height
  Increments the height of timeline element el by inc_h
  ----------------------------------------------------------------------*/
#ifdef _SVG
static void Inc_height(Document timelinedoc, Element el, int inc_h)
{
  AttributeType     attrType;
  Attribute         attr = NULL;
  char              buffer[512], *text = NULL, *ptr;
  ElementType       elType = TtaGetElementType (el);
  int               existing_value = 0, length;
  PresentationValue pval;

  attrType.AttrSSchema = elType.ElSSchema;
  attrType.AttrTypeNum = Timeline_ATTR_height_;
  attr = TtaGetAttribute (el, attrType);
  /* attr exists */
  length = TtaGetTextAttributeLength (attr) + 2;
  text = (char *)TtaGetMemory (length);
  if (text) 
    {
      TtaGiveTextAttributeValue (attr, text, &length);
      ptr = text;
      ptr = ParseNumber (ptr, &pval);
      existing_value = pval.typed_data.value;
      TtaFreeMemory (text);
    }

  sprintf (buffer, "%dpx", existing_value+inc_h);
  TtaSetAttributeText (attr, buffer, el, timelinedoc);
  TimelineParseWidthHeightAttribute (attr, el, timelinedoc, FALSE);
}
#endif /* _SVG */


/*----------------------------------------------------------------------
  Get_width_of
  return the width of a Timeline Element 
  ----------------------------------------------------------------------*/
#ifdef _SVG
static int Get_width_of (Element el)
{
  int                  r = 0;
  int                  length;
  char                *text = NULL, *ptr;
  PresentationValue    pval;
  AttributeType        attrType;
  Attribute            attr = NULL;
  ElementType          elType = TtaGetElementType (el);
   
  attrType.AttrSSchema = elType.ElSSchema;
  attrType.AttrTypeNum = Timeline_ATTR_width_;
  attr = TtaGetAttribute (el, attrType);
  length = TtaGetTextAttributeLength (attr) + 2;
  text = (char *)TtaGetMemory (length);
  if (text) 
    {
      /* get the value of the x attribute */
      TtaGiveTextAttributeValue (attr, text, &length);
      /* parse the attribute value (a number followed by a unit) */
      ptr = text;
      ptr = ParseNumber (ptr, &pval);
      r = pval.typed_data.value;
      TtaFreeMemory (text);
    }
  return r;
}
#endif /* _SVG */


/*----------------------------------------------------------------------
  Set_y_translation
  write the attribute transform="translate(0,y)" 
  ----------------------------------------------------------------------*/
#ifdef _SVG
static void Set_y_translation (Document timelinedoc, Element found, int hg) 
{ 
  AttributeType        attrType;
  Attribute            attr;
  PresentationValue    pval;
  ElementType          elType;
  Element              root;
  int                  existing_value = 0, length;
  char		       buffer[512];
  char                *text = NULL, *ptr;

  root = TtaGetMainRoot (timelinedoc);
  elType = TtaGetElementType (root);
  elType.ElTypeNum = Timeline_EL_abstract_group;
	
  attrType.AttrSSchema = elType.ElSSchema;
  attrType.AttrTypeNum = Timeline_ATTR_transform;
  attr = TtaGetAttribute (found, attrType);
						
  if (attr) 
    {
      length = TtaGetTextAttributeLength (attr) + 2;
      text = (char *)TtaGetMemory (length);
      if (text) 
        {
          /* get the value of the attribute */
          TtaGiveTextAttributeValue (attr, text, &length);
          /* parse the attribute value (a number followed by a unit) */
          ptr = text+strlen("translate(0,");
          ptr = ParseNumber (ptr, &pval);
          existing_value = pval.typed_data.value;
          TtaFreeMemory (text);
        }	
    }
  else 
    {
      attr = TtaNewAttribute (attrType);
      TtaAttachAttribute (found, attr, timelinedoc);
    }
					
  sprintf (buffer, "translate(0,%d);", existing_value+hg);	
  TtaSetAttributeText (attr, buffer, found, timelinedoc);
  ParseTransformAttribute(attr, found, timelinedoc, FALSE, FALSE);	
}
#endif /* _SVG */


/*----------------------------------------------------------------------
  Set_y
  write the attribute y=""
  ----------------------------------------------------------------------*/
#ifdef _SVG
static void Set_y (Document timelinedoc, Element found, int ny) 
{ 
  AttributeType        attrType;
  Attribute            attr;
  char		       buffer[512];
  ElementType          elType;
	
  elType = TtaGetElementType (found);
  attrType.AttrSSchema = elType.ElSSchema;
  attrType.AttrTypeNum = Timeline_ATTR_y;
  attr = TtaGetAttribute (found, attrType);
					
  sprintf (buffer, "%dpx", ny);	
  TtaSetAttributeText (attr, buffer, found, timelinedoc);
  TimelineParseCoordAttribute (attr, found, timelinedoc);	
}
#endif /* _SVG */


/*----------------------------------------------------------------------
  Set_src
  write the attribute src=""
  ----------------------------------------------------------------------*/
#ifdef _SVG
static void Set_src (Document timelinedoc, Element found, char* buffer) 
{ 
  AttributeType        attrType;
  Attribute            attr;
  ElementType          elType;
  Element              child;


  elType = TtaGetElementType (found);
  attrType.AttrSSchema = elType.ElSSchema;
  attrType.AttrTypeNum = Timeline_ATTR_xlink_href;
  attr = TtaGetAttribute (found, attrType);			
  TtaSetAttributeText (attr, buffer, found, timelinedoc);

  /* Thot api structure compatibility */
  child = TtaGetFirstChild (found);
  TtaSetTextContent (child, (unsigned char *)buffer, SPACE, timelinedoc);
}
#endif /* _SVG */


/*----------------------------------------------------------------------
  Set_x
  write the attribute x=""
  ----------------------------------------------------------------------*/
#ifdef _SVG
static void Set_x (Document timelinedoc, Element found, int nx) 
{ 
  AttributeType        attrType;
  Attribute            attr;
  char		       buffer[512];
  ElementType          elType;
	
  elType = TtaGetElementType (found);
  attrType.AttrSSchema = elType.ElSSchema;
  attrType.AttrTypeNum = Timeline_ATTR_x;
  attr = TtaGetAttribute (found, attrType);
					
  sprintf (buffer, "%dpx", nx);	
  TtaSetAttributeText (attr, buffer, found, timelinedoc);
  TimelineParseCoordAttribute (attr, found, timelinedoc);	
}
#endif /* _SVG */


/*----------------------------------------------------------------------
  Set_width
  write the attribute width=""
  ----------------------------------------------------------------------*/
#ifdef _SVG
static void Set_width (Document timelinedoc, Element found, int nw) 
{ 
  AttributeType        attrType;
  Attribute            attr;
  char		       buffer[512];
  ElementType          elType;
	
  elType = TtaGetElementType (found);
  attrType.AttrSSchema = elType.ElSSchema;
  attrType.AttrTypeNum = Timeline_ATTR_width_;
  attr = TtaGetAttribute (found, attrType);
					
  sprintf (buffer, "%dpx", nw);	
  TtaSetAttributeText (attr, buffer, found, timelinedoc);
  TimelineParseWidthHeightAttribute (attr, found, timelinedoc, FALSE);	
}
#endif /* _SVG */


/*----------------------------------------------------------------------
  Translate_following_elements
  update vertical position of groups that are on bottom of vpos
  hg is the vertical translation amount and can be either
  negative or positive
  ----------------------------------------------------------------------*/
#ifdef _SVG
static void Translate_following_elements(Document basedoc, Document timelinedoc,
                                         int vpos, int hg) 
{
  Element t_group, e_group, c_group;
  int     yfound, k;

  for (k = 0; k < dt[basedoc].nb_relations_animated; k++) 
    {
      /* translate groups */
      t_group = dt[basedoc].mapping_animated[k].title_group;
      e_group = dt[basedoc].mapping_animated[k].exp_group;
      c_group = dt[basedoc].mapping_animated[k].col_group;
      yfound = Get_y_of (TtaGetFirstChild (t_group));
      if (yfound > vpos)
        Set_y_translation (timelinedoc, t_group, hg);

      if (e_group) 
        {
          yfound = Get_y_of (TtaGetFirstChild (e_group));
          if (yfound > vpos)
            Set_y_translation (timelinedoc, e_group, hg);
        }
			
      if (c_group) 
        {
          yfound = Get_y_of (TtaGetFirstChild (c_group));
          if (yfound > vpos)
            Set_y_translation (timelinedoc, c_group, hg);
        }
    }
}
#endif /* _SVG */ 


/*----------------------------------------------------------------------
  TimelineParseTimeAttribute
  return value of a begin or dur or end attribute
  return value unit is s.
  @@@ TEMP : for now : only values in seconds are supported
 
  ----------------------------------------------------------------------*/
#ifdef _SVG
static double TimelineParseTimeAttribute (Attribute attr)
{
  double               r = 0;
  int                  length;
  char                *text = NULL, *ptr;
  PresentationValue    pval;

  length = TtaGetTextAttributeLength (attr) + 2;
  text = (char *)TtaGetMemory (length);
   
  if (text) 
    {
      /* get the value of the begin or end or dur attribute */
      TtaGiveTextAttributeValue (attr, text, &length);
      /* parse the attribute value (a number followed by a unit) */
      ptr = text;
      ptr = ParseNumber (ptr, &pval);
      r = pval.typed_data.value;
	   
      if (pval.typed_data.real)
        r /= 1000.0;

      /* look for following unit: ms? ... enhance the parser! */
      TtaFreeMemory (text);
    }
  return r;
}
#endif /* _SVG */


/*----------------------------------------------------------------------
  Get_nom_element_anime
  return name of animated object in buffer
  return !=0 if name present
  ----------------------------------------------------------------------*/
#ifdef _SVG
static int Get_nom_element_anime(Element el, char* buffer)
{
  Attribute            attr = NULL;
  int                  length = 512;
  AttributeType        attrType;
  ElementType          elType;
		
  elType = TtaGetElementType (el);
  attrType.AttrSSchema = elType.ElSSchema;
  attrType.AttrTypeNum = SVG_ATTR_id;
  attr = TtaGetAttribute (el,attrType);
  if (attr) 
    {
      length = TtaGetTextAttributeLength (attr);
      TtaGiveTextAttributeValue (attr, buffer, &length);
    }

  return ((attr) && (length));
}
#endif /* _SVG */


/*----------------------------------------------------------------------
  Read_time_info
  reads the start moment and duration of an animation element
  Warning : returned values are in s.
  ----------------------------------------------------------------------*/
void Read_time_info(Element el, double* start, double* duration) 
{
#ifdef _SVG
  AttributeType attrType;
  Attribute attr = NULL;
  ElementType elType = TtaGetElementType (el);

  /* look for begin and dur attributes */
  attrType.AttrSSchema = elType.ElSSchema;
  attrType.AttrTypeNum = SVG_ATTR_begin_;
			
  attr = TtaGetAttribute (el, attrType);
  if (attr)
    *start = TimelineParseTimeAttribute(attr);
		
  attrType.AttrTypeNum = SVG_ATTR_dur;
  attr = TtaGetAttribute (el,attrType);
  if (attr)
    *duration = TimelineParseTimeAttribute(attr);
#endif /* _SVG */
}


/*----------------------------------------------------------------------
  Inc_time_start
  ----------------------------------------------------------------------*/
#ifdef _SVG
static void Inc_time_start(Document basedoc, Element edited_anim, double inc_time)
{
  AttributeType attrType;
  Attribute     attr = NULL;
  char          buffer[512];
  ElementType   elType = TtaGetElementType (edited_anim);
  double        existing_value = 0.0;

  /* create or overwrite begin attribute */
  attrType.AttrSSchema = elType.ElSSchema;
  attrType.AttrTypeNum = SVG_ATTR_begin_;		
  attr = TtaGetAttribute (edited_anim, attrType);
  if (attr) 
    {
      existing_value = TimelineParseTimeAttribute (attr);
      TtaRegisterAttributeReplace (attr, edited_anim, basedoc);
    }
  else 
    {
      attr = TtaNewAttribute (attrType);
      TtaAttachAttribute (edited_anim, attr, basedoc);
      TtaRegisterAttributeCreate (attr, edited_anim, basedoc);
    }

  sprintf (buffer, "%.1fs", (float)(existing_value+inc_time));
  TtaSetAttributeText (attr, buffer, edited_anim, basedoc);
}
#endif /* _SVG */


/*----------------------------------------------------------------------
  Write_time_info
  outputs the start moment and duration of an animation element
  ----------------------------------------------------------------------*/
#ifdef _SVG
static void Write_time_info(Document basedoc, Element edited_anim,
                            double start, double duration) 
{
  AttributeType attrType;
  Attribute     attr = NULL;
  char          buffer[512];
  ElementType   elType = TtaGetElementType (edited_anim);

  /* create or overwrite begin attribute */
  attrType.AttrSSchema = elType.ElSSchema;
  attrType.AttrTypeNum = SVG_ATTR_begin_;	
  attr = TtaGetAttribute (edited_anim, attrType);
  if (!attr) 
    {
      attr = TtaNewAttribute (attrType);
      TtaAttachAttribute (edited_anim, attr, basedoc);
      TtaRegisterAttributeCreate (attr, edited_anim, basedoc);
    } 
  else 
    TtaRegisterAttributeReplace (attr, edited_anim, basedoc);

  sprintf (buffer, "%.1fs", (float) start);
  TtaSetAttributeText (attr, buffer, edited_anim, basedoc);

  /* create or overwrite dur attribute */
  attrType.AttrTypeNum = SVG_ATTR_dur;	
  attr = TtaGetAttribute (edited_anim, attrType);
  if (!attr) 
    {
      attr = TtaNewAttribute (attrType);
      TtaAttachAttribute (edited_anim, attr, basedoc);
      TtaRegisterAttributeCreate (attr, edited_anim, basedoc);
    } 
  else
    TtaRegisterAttributeReplace (attr, edited_anim, basedoc);

  sprintf (buffer, "%.1fs", (float) duration);
  TtaSetAttributeText (attr, buffer, edited_anim, basedoc);

}
#endif /* _SVG */


/*----------------------------------------------------------------------
  Search_first_anim_in_tree
  search in tree for the first animation tag
  element is the root of the tree
  ----------------------------------------------------------------------*/
#ifdef _SVG
static Element Search_first_anim_in_tree(Document basedoc, Element element)
{
  Element       res = NULL;
  ElementType   searchedType1, searchedType2;
  ElementType   searchedType3, searchedType4, searchedType5;
  SSchema       svg_schema;

  svg_schema = TtaGetSSchema ("SVG", basedoc);
  if (svg_schema) 
    {
      searchedType1.ElSSchema = svg_schema;
      searchedType1.ElTypeNum = SVG_EL_animate;
      searchedType2.ElSSchema = svg_schema;
      searchedType2.ElTypeNum = SVG_EL_animateMotion;
      searchedType3.ElSSchema = svg_schema;
      searchedType3.ElTypeNum = SVG_EL_animateTransform;
      searchedType4.ElSSchema = svg_schema;
      searchedType4.ElTypeNum = SVG_EL_animateColor;
      searchedType5.ElSSchema = svg_schema;
      searchedType5.ElTypeNum = SVG_EL_set_;
	
      res = TtaSearchElementAmong5Types (searchedType1, searchedType2,
                                         searchedType3, searchedType4,
                                         searchedType5, SearchInTree, element);	
    }
  return res;
}
#endif /* _SVG */

/*----------------------------------------------------------------------
  Search_next_anim_in_tree
  search in tree for the next animation tag 
  element is the previous animation tag
  ----------------------------------------------------------------------*/
#ifdef _SVG
static Element Search_next_anim_in_tree(Document basedoc, Element element)
{
  Element        res = NULL;
  ElementType   searchedType1, searchedType2;
  ElementType   searchedType3, searchedType4, searchedType5;
  SSchema       svg_schema;

  svg_schema = TtaGetSSchema ("SVG", basedoc);
  if (svg_schema) 
    {
      searchedType1.ElSSchema = svg_schema;
      searchedType1.ElTypeNum = SVG_EL_animate;
      searchedType2.ElSSchema = svg_schema;
      searchedType2.ElTypeNum = SVG_EL_animateMotion;
      searchedType3.ElSSchema = svg_schema;
      searchedType3.ElTypeNum = SVG_EL_animateTransform;
      searchedType4.ElSSchema = svg_schema;
      searchedType4.ElTypeNum = SVG_EL_animateColor;
      searchedType5.ElSSchema = svg_schema;
      searchedType5.ElTypeNum = SVG_EL_set_;

      res = TtaSearchElementAmong5Types (searchedType1, searchedType2,
                                         searchedType3, searchedType4,
                                         searchedType5, SearchForward, element);	
    }
  return res;
}
#endif /* _SVG */


/*----------------------------------------------------------------------
  Compute_global_anim_period_of_doc
  computes global anim period of document basedoc
  returned values are expressed in ms. 
  ----------------------------------------------------------------------*/
#ifdef _SVG
static void Compute_global_anim_period_of_doc (Document basedoc,
                                               double* tmin, double* tmax)
{
  int     k = 1;
  double  start, duration;
  Element found;

  *tmin = 0;
  *tmax = ct_default_anim_period_of_doc;

  found = Search_first_anim_in_tree (basedoc, TtaGetMainRoot (basedoc));

  while (found) 
    {
      Read_time_info (found, &start, &duration);	
      if (k == 1) 
        {
          *tmin = start;
          *tmax = start+duration;
        }
      else 
        {
          *tmin = min (start, *tmin);
          *tmax = max (duration+start, *tmax);
        }

      k++;
      found = Search_next_anim_in_tree (basedoc, found);
    }
}
#endif /* _SVG */


/*----------------------------------------------------------------------
  Create_title_group
  generate the title group of an animated element on Timeline UI  
  ----------------------------------------------------------------------*/
#ifdef _SVG
static void Create_title_group(Document basedoc, Document timelinedoc,
                               int ty, Element parent, int collapsed)
{
  Element temp_el;
  int     vertical_pos = 0;
  char    buffer[512], *utf8val;
	
  /* title rect*/
  dt[basedoc].current_el = Insert_group (timelinedoc, dt[basedoc].current_el, 0,
                                         Timeline_EL_abstract_group);
  Set_y_translation (timelinedoc, dt[basedoc].current_el, ty);

  temp_el = Insert_rectangle (timelinedoc, dt[basedoc].current_el, "white",
                              "black", 2, vertical_pos, ct_default_title_group_width,
                              ct_default_title_group_height, 1, Timeline_EL_rect_id);	
	
  Build_path_to_image_dir (buffer);
  if (collapsed)
    strcat (buffer, ct_image_c2_filename);
  else
    strcat (buffer, ct_image_c1_filename);
		
  temp_el = Insert_image (timelinedoc, temp_el, buffer, 4, vertical_pos+10,
                          ct_w_image_collapse, ct_h_image_collapse, 0,
                          Timeline_EL_image_collapse);
  if (!Get_nom_element_anime (parent, buffer))
    {
      utf8val = (char *)TtaConvertByteToMbs ((unsigned char *)
                                             TtaGetMessage (AMAYA, AM_SVGANIM_NO_ID),
                                             TtaGetDefaultCharset ());
      strcpy (buffer, utf8val);
      TtaFreeMemory (utf8val);
    }
  temp_el = Insert_text (timelinedoc, temp_el, "black", buffer, ct_elem_font_family,
                         ct_elem_font_size, 18, vertical_pos+6, 0,
                         Timeline_EL_text_id); 
}
#endif /* _SVG */


/*----------------------------------------------------------------------
  Create_expanded_group
  generate the expanded group of an animated element "parent" on Timeline UI  
  first anim is its first animation child
  returns the first animation node which has not the same parent
  ----------------------------------------------------------------------*/
#ifdef _SVG
static Element Create_expanded_group(Document basedoc, Document timelinedoc,
                                     int ty, Element parent, Element first_anim,
                                     pmapping_animated mapping, int* hauteur)
{
  Element     found = first_anim;
  Element     temp_el;
  int         k, vertical_pos = 0;
  double      start, duration;
  ElementType elType;
  char        color[512];

  /* expanded group */	 	
  dt[basedoc].current_el = Insert_group (timelinedoc, dt[basedoc].current_el,
                                         0, Timeline_EL_abstract_group);	
  Set_y_translation(timelinedoc, dt[basedoc].current_el, ty);
  k=0;
  mapping->nb_periods = k;
  while ((found) && (TtaGetParent(found)==parent)) 
    {

      Read_time_info (found, &start, &duration);
			
      /* periods */
      if (k==0)
        temp_el = Insert_rectangle (timelinedoc, dt[basedoc].current_el,
                                    ct_static_bar_color, ct_static_bar_color,
                                    ct_left_bar,
                                    vertical_pos+6+k*(6+ct_static_bar_height),
                                    current_timeline_width, ct_static_bar_height,
                                    1, Timeline_EL_rect_interface);
      else
        temp_el = Insert_rectangle (timelinedoc, temp_el, ct_static_bar_color,
                                    ct_static_bar_color,  ct_left_bar,
                                    vertical_pos+6+k*(6+ct_static_bar_height),
                                    current_timeline_width, ct_static_bar_height,
                                    0, Timeline_EL_rect_interface);
		
      /* A color for each animation type */
      elType = TtaGetElementType (found);
			
      switch(elType.ElTypeNum) 
        {
        case SVG_EL_animate : 
          strcpy(color, ct_animate_color); break;
        case SVG_EL_set_ : 
          strcpy(color,ct_set_color); break;
        case SVG_EL_animateMotion : 
          strcpy(color,ct_animatemotion_color); break;
        case SVG_EL_animateColor : 
          strcpy(color,ct_animatecolor_color); break;
        case SVG_EL_animateTransform : 
          strcpy(color,ct_animatetransform_color); break;
        default : 
          strcpy(color,"black"); break;
        }

      temp_el = Insert_rectangle (timelinedoc, temp_el, color, "none",
                                  Time_to_xposition (start),
                                  vertical_pos+6+k*(6+ct_static_bar_height)+ct_offset_y_period,
                                  (int) (duration * time_sep), ct_period_height, 0,
                                  Timeline_EL_exp_period);
			
      /* update mapping of <animate> tags */
      mapping->mapping_animations[mapping->nb_periods].animation_tag = found;
      mapping->mapping_animations[mapping->nb_periods].period = temp_el;

      k++;
      mapping->nb_periods = k;
      found = Search_next_anim_in_tree (basedoc, found);
    }
	
  *hauteur = max( 6 +(k)*(ct_static_bar_height+6),
                  ct_default_title_group_height - ct_static_bar_height - 6);
  if (k==1)
    { 
      /* there is only one animated element :*/
      temp_el = Insert_rectangle (timelinedoc, dt[basedoc].current_el,
                                  ct_expanded_group_fill_color, "black",
                                  ct_left_bar-2, vertical_pos,
                                  current_timeline_width+4,
                                  ct_collapsed_group_height,1,
                                  Timeline_EL_rect_interface);
		
      /* increase vertical position of the only one animation element
         to center it vertically inside expanded group */
      TtaNextSibling (&temp_el);
      Set_y (timelinedoc, temp_el, (ct_collapsed_group_height-ct_static_bar_height)/2);
      TtaNextSibling (&temp_el);
      Set_y (timelinedoc, temp_el, (ct_collapsed_group_height-ct_static_bar_height)/2+1);
    }
  else
    temp_el = Insert_rectangle (timelinedoc, dt[basedoc].current_el,
                                ct_expanded_group_fill_color, "black",
                                ct_left_bar-2, vertical_pos,
                                current_timeline_width+4, *hauteur,1,
                                Timeline_EL_rect_interface);
  return found;
}
#endif /* _SVG */		


/*----------------------------------------------------------------------
  Remove_image_collapse_from_title_group
  ----------------------------------------------------------------------*/
#ifdef _SVG
static void Remove_image_collapse_from_title_group (Document basedoc,
                                                    Element animated_el)
{
  Element  im;
  Document timelinedoc = dt[basedoc].timelinedoc;

  /* get title group:*/
  im = Get_title_group_of_animated_element (basedoc, animated_el);
  /* image_collapse is the second child of title_group*/
  im = TtaGetFirstChild (im);
  TtaNextSibling (&im);
  TtaDeleteTree (im, timelinedoc); 
}
#endif /* _SVG */	


/*----------------------------------------------------------------------
  Image_collapse_for_title_group ()
  Check for presence of image collapse
  ----------------------------------------------------------------------*/
#ifdef _SVG
static ThotBool Image_collapse_for_title_group (Document basedoc,
                                                Element animated_el)
{
  Element     tg, found;
  ElementType elType;

  /* get title group:*/
  tg = Get_title_group_of_animated_element (basedoc, animated_el);
  elType = TtaGetElementType (tg);
  elType.ElTypeNum = Timeline_EL_image_collapse;

  found = TtaSearchTypedElement (elType, SearchInTree, tg);
  return (found != NULL);
}
#endif /* _SVG */	


/*----------------------------------------------------------------------
  Add_image_collapse_on_title_group
  ----------------------------------------------------------------------*/
#ifdef _SVG
static void Add_image_collapse_on_title_group (Document basedoc,
                                               Element animated_el, int collapsed)
{
  char buffer[512];
  Element temp_el;
  Document timelinedoc = dt[basedoc].timelinedoc;

  /* get title group:*/
  temp_el = Get_title_group_of_animated_element (basedoc, animated_el);
  /* image_collapse should be the second child of title_group*/
  temp_el = TtaGetFirstChild (temp_el);

  Build_path_to_image_dir (buffer);
  if (collapsed)
    strcat (buffer, ct_image_c2_filename);
  else
    strcat (buffer, ct_image_c1_filename);

  Insert_image (timelinedoc, temp_el, buffer, 4, 10,
                ct_w_image_collapse, ct_h_image_collapse, 0,
                Timeline_EL_image_collapse);

}
#endif /* _SVG */	


/*----------------------------------------------------------------------
  Create_collapsed_group
  generate the collapsed group of an animated element "parent" on Timeline UI  
  first anim is its first animation child
  returns the first animation node which has not the same parent
  ----------------------------------------------------------------------*/
#ifdef _SVG
static Element Create_collapsed_group(Document basedoc, Document timelinedoc,
                                      int ty, Element parent,
                                      pmapping_animated mapping, Element first_anim)
{
  Element found = first_anim;
  Element temp_el;
  int     k, vertical_pos = 0, h;
  double  start, duration, tmin = 0.0, tmax = 0.0;
	
  k=0;
  while ((found) && (TtaGetParent(found)==parent)) 
    {
      Read_time_info (found, &start, &duration);
      if (k==0) 
        {
          tmin = start;
          tmax = start+duration;
        }
      else 
        {
          tmin = min (start, tmin);
          tmax = max (duration+start, tmax);
        }
 
      k++;
      found = Search_next_anim_in_tree (basedoc, found);
    }
		
  mapping->nb_periods = 0;

  if (k==1) 
    {
      /* there is only one animation, expand group: */
      Create_expanded_group (basedoc, timelinedoc, ty, parent, first_anim, mapping, &h);
      /* remove image_collapse from title group:*/
      Remove_image_collapse_from_title_group (basedoc, parent);
    }
  else 
    {
      /* collapsed group */	 	
      dt[basedoc].current_el = Insert_group (timelinedoc, dt[basedoc].current_el,
                                             0, Timeline_EL_abstract_group);	
      Set_y_translation (timelinedoc, dt[basedoc].current_el, ty);

      temp_el = Insert_rectangle (timelinedoc, dt[basedoc].current_el,
                                  ct_expanded_group_fill_color, "black",
                                  ct_left_bar-2, vertical_pos, current_timeline_width+4,
                                  ct_collapsed_group_height,1,
                                  Timeline_EL_rect_interface);

      temp_el = Insert_rectangle (timelinedoc, temp_el, ct_static_bar_color,
                                  ct_static_bar_color,  ct_left_bar, vertical_pos+10,
                                  current_timeline_width, ct_static_bar_height, 0,
                                  Timeline_EL_rect_interface);
      temp_el = Insert_rectangle (timelinedoc, temp_el, ct_collapsed_anim_color,
                                  "none",  Time_to_xposition (tmin),
                                  vertical_pos+10+ct_offset_y_period,
                                  (int) ((tmax-tmin) * time_sep), ct_period_height, 0,
                                  Timeline_EL_col_period);
    }

  return found;
}
#endif /* _SVG */		


/*----------------------------------------------------------------------
  Draw_graduation
  Draw "1s, 2s, 3s..." on line
  ----------------------------------------------------------------------*/
#ifdef _SVG
static Element Draw_graduation (Document timelinedoc, Element position,
                                double ti, double tf)
{
  Element temp_el = position;
  int     i, top;
  char    buffer[512];

  if ((tf-ti)<MAX_ENLARGE_TIMELINE)
    for (i = (int)ti; i<= (int)tf; i++) 
      {	
        if (i) 
          top = ct_top+4;
        else 
          top = ct_top-1;
			
        temp_el = Insert_tline (timelinedoc, temp_el, "black",
                                ct_left_bar+(int)(i*time_sep), top,
                                ct_left_bar+(int)(i*time_sep),
                                ct_top+4+ct_graduation_height, 0);
        temp_el = Insert_tline (timelinedoc, temp_el, "silver",
                                ct_left_bar+(int)(i*time_sep)+1, top,
                                ct_left_bar+(int)(i*time_sep)+1,
                                ct_top+4+ct_graduation_height, 0);
			
        sprintf (buffer, "%ds", i);
        temp_el = Insert_text (timelinedoc, temp_el, "black", buffer,
                               ct_graduation_font_family, ct_graduation_font_size,
                               ct_left_bar+(int)(i*time_sep)-6,
                               ct_top+4+ct_graduation_height+ct_graduation_font_size+2,
                               0, Timeline_EL_text_);
        TtaSetAccessRight (temp_el, ReadOnly, timelinedoc);
      }
  return temp_el;
}
#endif /* _SVG */


/*----------------------------------------------------------------------
  Build_timeline
  Constructs the Timeline document of an opened document
  ----------------------------------------------------------------------*/
#ifdef _SVG
static void Build_timeline (Document basedoc, char* timelineName)
{
  Element           el, found, root, temp_el, parent, baseroot;
  pmapping_animated mapping;
  int               h_current, left, x_timeline, y_timeline, w_timeline;
  int               h_timeline, period_w_size;
  double            t_debut_toutes_anim, t_fin_toutes_anim;
  char              buffer[512];
  /* params used to specify a position for the view in wxWidgets version */
  int               window_id     = -1;
  int               page_id       = -1;
  int               page_position = -1;
  ThotBool          oldStructureChecking;  
	
  el = NULL;
  root = NULL;
  /* create the document */
  dt[basedoc].timelinedoc = TtaInitDocument ("Timeline", "Timeline", "Timeline", 0);
  TtaSetPSchema (dt[basedoc].timelinedoc, "TimelineP");
  
  /* get customizable default position & size*/
  TtaGetViewGeometry (dt[basedoc].timelinedoc, 
                      "Timeline_view", 
                      &x_timeline, &y_timeline, &w_timeline, &h_timeline);

  Compute_global_anim_period_of_doc (basedoc, 
                                     &t_debut_toutes_anim, 
                                     &t_fin_toutes_anim);
	
  period_w_size = max( w_timeline-ct_left_bar - ct_vscrollbar_w, 
                       (int) (3/2.0 * ct_left_bar));
  time_sep = period_w_size / t_fin_toutes_anim;
  current_timeline_width = period_w_size;
	
  /* do not check the Thot abstract tree against the structure 
     schema when inserting this element */
  oldStructureChecking = TtaGetStructureChecking (dt[basedoc].timelinedoc);
  TtaSetStructureChecking (FALSE, dt[basedoc].timelinedoc);

  root = TtaGetRootElement (dt[basedoc].timelinedoc);
  TtaSetElCoordinateSystem (root);
  baseroot = TtaGetRootElement (basedoc);

  /* build the graphic timeline of document basedoc */
  /* document independant code */
  /* line */
  dt[basedoc].line1 = Insert_tline (dt[basedoc].timelinedoc, root, "black",
                                    ct_left_bar, ct_top, ct_left_bar+period_w_size,
                                    ct_top, 1);
  dt[basedoc].line2 = Insert_tline (dt[basedoc].timelinedoc, dt[basedoc].line1,
                                    "white", ct_left_bar, ct_top+1,
                                    ct_left_bar+period_w_size, ct_top+1, 0);
  dt[basedoc].line3 = Insert_tline (dt[basedoc].timelinedoc, dt[basedoc].line2,
                                    "gray", ct_left_bar, ct_top+2,
                                    ct_left_bar+period_w_size, ct_top+2, 0);
  dt[basedoc].line4 = Insert_tline (dt[basedoc].timelinedoc, dt[basedoc].line3,
                                    "black", ct_left_bar, ct_top+3,
                                    ct_left_bar+period_w_size, ct_top+3, 0);
  dt[basedoc].line5 = Insert_tline (dt[basedoc].timelinedoc, dt[basedoc].line4,
                                    "silver", ct_left_bar, ct_top+4,
                                    ct_left_bar+period_w_size, ct_top+4, 0);
  dt[basedoc].current_el = dt[basedoc].line5;

  /* document dependant code */
  h_current = ct_top_bar;
  found = Search_first_anim_in_tree (basedoc, baseroot);
  while (found) 
    {
      /* animated element */
      parent = TtaGetParent (found);
      /* increase nb_relations_animated here to be able to access 
         partly built mapping inside Create_collapsed group
         following access to number of relations in this function 
         should be nb_relations_animated-1*/
		
      dt[basedoc].nb_relations_animated++;
      mapping = &(dt[basedoc].mapping_animated[dt[basedoc].nb_relations_animated-1]);

      /* update mapping of animated element as soon as created :*/
      mapping->animated_elem = parent;

      Create_title_group (basedoc, dt[basedoc].timelinedoc, h_current, parent, 1) ;
      /* maps title group*/
      mapping->title_group = dt[basedoc].current_el;
		
      found =	Create_collapsed_group (basedoc, dt[basedoc].timelinedoc, h_current, parent, mapping, found);
      /* maps collapsed group*/
      /* warning :  if only one animation then maps expanded group else maps collapsed group :*/
      if (mapping->nb_periods==1)
        mapping->exp_group = dt[basedoc].current_el;
      else
        mapping->col_group = dt[basedoc].current_el;
	
      h_current += ct_collapsed_group_height+2;
    }

  dt[basedoc].vertical_pos = h_current;
  /* document independant code */
  /* toolbar */
  Build_path_to_image_dir (buffer);
  strcat(buffer, ct_image_toolbar);
  temp_el = Insert_image (dt[basedoc].timelinedoc, dt[basedoc].current_el,
                          buffer, ct_left_bar - ct_w_image_toolbar,
                          ct_top_toolbar , ct_w_image_toolbar, ct_h_image_toolbar,
                          0, Timeline_EL_image_toolbar);

  /* buttons */	
  left = ct_left_buttons;
  Build_path_to_image_dir (buffer);
  strcat(buffer, ct_image_anim_help);
  Insert_image (dt[basedoc].timelinedoc, temp_el, buffer, left, ct_top_buttons,
                ct_w_image_help, ct_h_image_help, 0,Timeline_EL_image_help);

  left += ct_w_image_help + ct_sep_buttons;
  Build_path_to_image_dir (buffer);
  strcat(buffer, ct_image_anim_motion_disabled);
  dt[basedoc].image_motion = Insert_image (dt[basedoc].timelinedoc, temp_el,
                                           buffer, left, ct_top_buttons,
                                           ct_w_image_motion, ct_h_image_motion,
                                           0, Timeline_EL_image_motion);

  left += ct_w_image_motion + ct_sep_buttons;
  Build_path_to_image_dir (buffer);
  strcat(buffer, ct_image_anim_color_disabled);
  dt[basedoc].image_color = Insert_image (dt[basedoc].timelinedoc, temp_el,
                                          buffer, left, ct_top_buttons ,
                                          ct_w_image_color, ct_h_image_color,
                                          0, Timeline_EL_image_color);

  left += ct_w_image_color + ct_sep_buttons;
  /*	Build_path_to_image_dir (buffer);
    strcat(buffer, ct_image_anim_help);
    dt[basedoc].image_interp = Insert_image (dt[basedoc].timelinedoc,
    temp_el, buffer, left, ct_top_buttons , ct_w_image_help,
    ct_h_image_help, 0, Timeline_EL_image_help);
  */

  /* timing info */
  sprintf (buffer, "%.2fs", (float) 0.0);	
  dt[basedoc].timing_text = Insert_text ( dt[basedoc].timelinedoc, temp_el, "black",
                                          buffer, ct_elem_font_family, 
                                          ct_elem_font_size,
                                          ct_left_bar - ct_w_image_toolbar + 32, 10,
                                          0, Timeline_EL_timing_text);

  /* graduation */
  temp_el = Draw_graduation (dt[basedoc].timelinedoc, temp_el, 0.0, t_fin_toutes_anim);
  current_timeline_end = t_fin_toutes_anim;

  /* slider */
  Build_path_to_image_dir (buffer);
  strcat(buffer, ct_image_slider_filename);
  dt[basedoc].slider = Insert_image (dt[basedoc].timelinedoc, temp_el,
                                     buffer, ct_left_bar-6, ct_top-3,
                                     ct_w_image_slider, ct_h_image_slider, 0,
                                     Timeline_EL_image_slider);

  /* Amaya should not ask the user to save Timeline document because of modifications*/
  TtaSetDocumentUnmodified (dt[basedoc].timelinedoc);
  /* timeline has been built */
  TtaSetStructureChecking (oldStructureChecking, dt[basedoc].timelinedoc);


#ifdef _WX
  /* calculate the parent window id, the page id and the page position */
  window_id = TtaGetDocumentWindowId( basedoc, -1 );
  TtaGetDocumentPageId( basedoc, -1, &page_id, &page_position );
  /* force the view to be shown in the bottom frame */
  page_position = 2;
  wxASSERT_MSG( window_id > 0 && page_id >=0, _T("The timeline window or page parent is wrong."));
#endif /* _WX */

  /* Now build the frame to contains this view */
  dt[basedoc].timelineView = TtaOpenMainView (dt[basedoc].timelinedoc,
                                              "timeline",
                                              x_timeline, y_timeline,
                                              w_timeline, h_timeline,
                                              FALSE, FALSE,
                                              window_id, /* window_id */
                                              page_id, /* page_id */
                                              page_position /* page_position */ );

  if (dt[basedoc].timelineView)
    TtaChangeWindowTitle (dt[basedoc].timelinedoc, dt[basedoc].timelineView,
                          timelineName, TtaGetDefaultCharset ());
  else
    {
      TtaCloseDocument (dt[basedoc].timelinedoc);
      Init_timeline_struct (basedoc);
    }
}
#endif /* _SVG */


/*----------------------------------------------------------------------
  Enlarge_group_items
  ----------------------------------------------------------------------*/
#ifdef _SVG
static void Enlarge_group_items (Document timelinedoc, Element group, int inc_w) 
{
  Element           found;
  ElementType       st;
  Attribute         attr;
  AttributeType     attrType;
  PresentationValue pval;
  char             *text, buffer[512];
  char             *ptr;
  int               l;

  st = TtaGetElementType (TtaGetMainRoot (timelinedoc));
  st.ElTypeNum = Timeline_EL_rect_interface;

  found = TtaSearchTypedElement(st, SearchInTree, group); 
  attrType.AttrSSchema = st.ElSSchema;
  attrType.AttrTypeNum = Timeline_ATTR_width_;

  while ((found) && (TtaGetParent (found) == group)) 
    {
      attr = TtaGetAttribute (found, attrType);
      l = TtaGetTextAttributeLength (attr);
      text = NULL;
      text = (char *)TtaGetMemory (l + 1);
      if (text) 
        {
          TtaGiveTextAttributeValue (attr, text, &l);
          ptr = text;
          ParseNumber (ptr, &pval);
          TtaFreeMemory (text);
        }
      sprintf (buffer, "%dpx", pval.typed_data.value+inc_w);
      TtaSetAttributeText (attr, buffer, found, timelinedoc);
      TimelineParseWidthHeightAttribute (attr, found, timelinedoc, FALSE);

      found = TtaSearchTypedElement(st, SearchForward, found); 
    }
}
#endif /* _SVG */


/*----------------------------------------------------------------------
  Enlarge_line
  ----------------------------------------------------------------------*/
#ifdef _SVG
static void Enlarge_line (Document timelinedoc, Element line, int inc_w) 
{
  Attribute         attr;
  AttributeType     attrType;
  PresentationValue pval;
  char             *ptr;
  char             *text = NULL, buffer[512];
  int               l;

  attrType.AttrSSchema = TtaGetElementType (line).ElSSchema;
  attrType.AttrTypeNum = Timeline_ATTR_x2;
  attr = TtaGetAttribute (line, attrType);
  l = TtaGetTextAttributeLength (attr);
  text = (char *)TtaGetMemory (l + 1);
  if (text) 
    {
      TtaGiveTextAttributeValue (attr, text, &l);
      ptr = text;
      ParseNumber (ptr, &pval);
      TtaFreeMemory (text);
    }
  sprintf (buffer, "%dpx", pval.typed_data.value+inc_w);
  TtaSetAttributeText (attr, buffer, line, timelinedoc);
  TimelineParseCoordAttribute (attr, line, timelinedoc);
}
#endif /* _SVG */


/*----------------------------------------------------------------------
  Enlarge_timeline
  ----------------------------------------------------------------------*/
#ifdef _SVG
static void Enlarge_timeline (Document basedoc, Document timelinedoc, int x) 
{
  int     delta_t, k = 0, inc_w, inc_lines;
  Element cgroup, egroup;

  /* enlarge line */
  delta_t = 1 + (int)((x-ct_left_bar-current_timeline_end*time_sep)/time_sep);
  Draw_graduation (timelinedoc, dt[basedoc].current_el, current_timeline_end,
                   current_timeline_end+delta_t);

  inc_lines = (int)(time_sep*delta_t);
  inc_w = 6+x-(current_timeline_width+ct_left_bar);
  Enlarge_line (timelinedoc, dt[basedoc].line1, inc_lines);
  Enlarge_line (timelinedoc, dt[basedoc].line2, inc_lines);
  Enlarge_line (timelinedoc, dt[basedoc].line3, inc_lines);
  Enlarge_line (timelinedoc, dt[basedoc].line4, inc_lines);
  Enlarge_line (timelinedoc, dt[basedoc].line5, inc_lines);

  /* enlarge abstract groups elements */
  for (k=0; k<(dt[basedoc]).nb_relations_animated; k++) 
    {
      egroup = (dt[basedoc]).mapping_animated[k].exp_group;
      cgroup = (dt[basedoc]).mapping_animated[k].col_group;
		
      if (egroup)
        Enlarge_group_items (timelinedoc, egroup, inc_w); 
      if (cgroup)
        Enlarge_group_items (timelinedoc, cgroup, inc_w); 
    }
  current_timeline_width += inc_w;
  current_timeline_end += delta_t;
}
#endif /* _SVG */


/*----------------------------------------------------------------------
  ShowTimeLineWindow
  displays the Timeline window of a document
  ----------------------------------------------------------------------*/
void ShowTimeLineWindow (Document document, View view)
{
#ifdef _SVG
  View     v;
  Document t;

  /*      	- already created -> show it
          - not created     -> create it */
  Get_timeline_of_doc (document, &t, &v);
  if (t)
    TtaRaiseView (dt[document].timelinedoc, v);
  else 
    {
      Build_timeline (document, TtaGetMessage (AMAYA, AM_BUTTON_ANIM));
      Get_timeline_of_doc (document, &t, &v);
      TtaDisableScrollbars (t, v);
    }
#endif /* _SVG */
}

/*----------------------------------------------------------------------
  ShowSource
  Temporary hack to display the source code of a generated and not saved 
  timeline document
  ----------------------------------------------------------------------*/
void ShowSourceOfTimeline (Document document, View view)
{
#ifdef _SVG
  /* this function is temporary, only for debugging purposes*/
  CHARSET          charset;
  char            *tempdocument;
  char            *s;
  char  	   documentname[MAX_LENGTH];
  char  	   tempdir[MAX_LENGTH];
  Document         sourceDoc;
  NotifyElement    event;
  int              non_sav;
  int              ok = -1;

  non_sav = (!DocumentURLs[document]);
  if (non_sav) 
    DocumentURLs[document] = TtaStrdup ("C:\\temp.svg");
  
  if (DocumentTypes[document] != docHTML &&
      DocumentTypes[document] != docSVG &&
      DocumentTypes[document] != docXml &&
      DocumentTypes[document] != docMath)
    return;

  if (DocumentSource[document])
    TtaRaiseView (DocumentSource[document], 1);
  else
    {
      tempdocument = GetLocalPath (document, DocumentURLs[document]);
      if (non_sav)
        {
          tempdocument = TtaStrdup ("C:\\temp2.svg");
          ok = TtaExportDocumentWithNewLineNumbers (document,
                                                    tempdocument, "TimelineT", TRUE);
        }

      TtaExtractName (tempdocument, tempdir, documentname);
      sourceDoc = InitDocAndView (document,
                                  FALSE /* replaceOldDoc */,
                                  TRUE /* inNewWindow */,
                                  documentname, docSource, document, FALSE,
                                  L_Other, 0, CE_ABSOLUTE);   
      if (sourceDoc > 0)
        {
          DocumentSource[document] = sourceDoc;
          s = TtaStrdup (DocumentURLs[document]);
          DocumentURLs[sourceDoc] = s;
          DocumentMeta[sourceDoc] = DocumentMetaDataAlloc ();
          DocumentMeta[sourceDoc]->form_data = NULL;
          DocumentMeta[sourceDoc]->initial_url = NULL;
          DocumentMeta[sourceDoc]->method = CE_ABSOLUTE;
          DocumentMeta[sourceDoc]->xmlformat = FALSE;
          DocumentMeta[sourceDoc]->compound = FALSE;	
          if (non_sav)
            {
              DocumentMeta[document] = DocumentMetaDataAlloc ();
              DocumentMeta[document]->form_data = NULL;
              DocumentMeta[document]->initial_url = NULL;
              DocumentMeta[document]->method = CE_ABSOLUTE;
              DocumentMeta[document]->xmlformat = FALSE;
              DocumentMeta[document]->compound = FALSE;
            }
	 
          if (DocumentMeta[document]->content_type)
            DocumentMeta[sourceDoc]->content_type = TtaStrdup (DocumentMeta[document]->content_type);
          if (DocumentMeta[document]->charset)
            DocumentMeta[sourceDoc]->charset = TtaStrdup (DocumentMeta[document]->charset); 
          if (DocumentMeta[document]->content_location)
            DocumentMeta[sourceDoc]->content_location = TtaStrdup (DocumentMeta[document]->content_location);
          if (DocumentMeta[document]->full_content_location)
            DocumentMeta[sourceDoc]->full_content_location = TtaStrdup (DocumentMeta[document]->full_content_location);
          DocumentTypes[sourceDoc] = docSource;
          charset = TtaGetDocumentCharset (document);
          if (charset == UNDEFINED_CHARSET)
            {
              if (DocumentMeta[document]->xmlformat)
                TtaSetDocumentCharset (SavingDocument, UTF_8, FALSE);
              else
                TtaSetDocumentCharset (SavingDocument, ISO_8859_1, FALSE);
            }
          else
            TtaSetDocumentCharset (sourceDoc, charset, FALSE);
          DocNetworkStatus[sourceDoc] = AMAYA_NET_INACTIVE;
          StartParser (sourceDoc, tempdocument, documentname, tempdir,
                       tempdocument, TRUE, FALSE);
          SetWindowTitle (document, sourceDoc, 0);
	 
#ifdef ANNOTATIONS
          TtaSetMenuOff (sourceDoc, 1, Annotations_);
#endif
          if (TtaIsDocumentModified (document))
            DocStatusUpdate (document, TRUE);
          event.document = document;
          SynchronizeSourceView (&event);
        }
      TtaFreeMemory (tempdocument);
    }
#endif /* _SVG */
}
/*----------------------------------------------------------------------
  Image_collapse_on_click
  A Callback : handles a click on + or - to expand/collapse groups 
  ----------------------------------------------------------------------*/
#ifdef _SVG
static void Image_collapse_on_click(NotifyElement *event)
{
  int               length, hg, vpos, hep;
  char              buffer[512], im2[512], im1[512];
  Language          lang;  
  Element           parent, title_group, first_found, new_fresh_group;
  ElementType       elType;
  pmapping_animated mapping;
  Document          basedoc = Get_basedoc_of (event->document);
  DisplayMode       dp = TtaGetDisplayMode (event->document);
	
  TtaSetDisplayMode (event->document, DeferredDisplay);
  lang = TtaGetDefaultLanguage ();
  parent = TtaGetParent (event->element);
  title_group = TtaGetParent (parent);
  elType = TtaGetElementType (parent);

  length = 512;
  TtaGiveTextContent (event->element, (unsigned char *)buffer, &length, &lang);
		
  mapping = Get_mapping_from_title_group (basedoc, title_group);

  Build_path_to_image_dir (im1);
  strcat (im1, ct_image_c1_filename);
  Build_path_to_image_dir (im2);
  strcat (im2, ct_image_c2_filename);

  if (!strcmp (buffer, im1)) 
    { /* - -> + collapse*/
      /* update image */
      TtaSetTextContent (event->element, (unsigned char *)im2, SPACE,
                         event->document);
							 
      /* create collapsed group */
      first_found = Search_first_anim_in_tree (basedoc, mapping->animated_elem);
      vpos = Get_y_of (TtaGetFirstChild (title_group));
      Create_collapsed_group (basedoc, event->document, vpos, mapping->animated_elem,
                              mapping, first_found);
      new_fresh_group = dt[basedoc].current_el;

      hep = Get_height_of(TtaGetFirstChild(mapping->exp_group));

      /* delete expanded group */
      TtaDeleteTree (mapping->exp_group, event->document); 
				
      /* update mapping*/
      Set_collapsed_group_of (basedoc, mapping->animated_elem, new_fresh_group);
      Set_expanded_group_of (basedoc, mapping->animated_elem, NULL);
	
      /* translate visible groups on the interface */
      Translate_following_elements (basedoc, event->document, vpos,
                                    -(hep-ct_collapsed_group_height));
      dt[basedoc].vertical_pos -= (hep-ct_collapsed_group_height);
    }
  else 
    { /* + -> -  expand */
      /* update image */
      TtaSetTextContent (event->element, (unsigned char *)im1, SPACE, event->document);

      /* create expanded group  */
      first_found = Search_first_anim_in_tree (basedoc, mapping->animated_elem);
      vpos = Get_y_of (TtaGetFirstChild (title_group));
      Create_expanded_group (basedoc, event->document, vpos, mapping->animated_elem,
                             first_found, mapping, &hg);
      new_fresh_group = dt[basedoc].current_el;

      /* delete collapsed group */
      TtaDeleteTree (mapping->col_group, event->document);

      /* update mapping*/
      Set_expanded_group_of (basedoc, mapping->animated_elem, new_fresh_group);
      Set_collapsed_group_of (basedoc, mapping->animated_elem, NULL);
		
      /* translate visible groups on the interface */
      Translate_following_elements (basedoc, event->document, vpos,
                                    hg-ct_collapsed_group_height);
      dt[basedoc].vertical_pos += hg-ct_collapsed_group_height;	
    }

  TtaSetDisplayMode (event->document, dp);
}
#endif /* _SVG */



/*----------------------------------------------------------------------
  Set_slider_position
  unit of param f is second
  ----------------------------------------------------------------------*/
void Set_slider_position (Document basedoc, Document timelinedoc, double f) 
{
#ifdef _SVG
  char          buffer[512];
  AttributeType attrType;
  Attribute     attr;
  int           x;

  attrType.AttrSSchema = TtaGetElementType (dt[basedoc].slider).ElSSchema;
  attrType.AttrTypeNum = Timeline_ATTR_x;

  attr = TtaGetAttribute (dt[basedoc].slider, attrType);
  x = (int) (f*time_sep+ct_left_bar-6);
  sprintf (buffer, "%dpx", x);
  TtaSetAttributeText (attr, buffer, dt[basedoc].slider,timelinedoc);
  TimelineParseCoordAttribute (attr, dt[basedoc].slider, timelinedoc);

  if (x>ct_left_bar+current_timeline_width)
    Enlarge_timeline (basedoc, timelinedoc, x);
#endif /* _SVG */
}

/*----------------------------------------------------------------------
  Set_slider_position_from_doc
  unit of param f is second
  ----------------------------------------------------------------------*/
void Set_slider_position_from_doc (Document basedoc,  double f)
{
#ifdef _SVG
  char     buffer[512];
  Document timelinedoc;
  View     timelineview;
  Element  tm = dt[basedoc].timing_text;
  int      x = (int) (f*time_sep + ct_left_bar - 6);
 
  Get_timeline_of_doc (basedoc, &timelinedoc, &timelineview);
  if (timelinedoc == 0)
    return;

  /* Modulo */
  if (x > ct_left_bar + current_timeline_width)
    {
      f = fmod (f, (double)current_timeline_width / time_sep);
      x = (int) (f*time_sep + ct_left_bar - 6);
    }
  /* Update timing_text */
  if (tm)
    {
      Element child = TtaGetFirstChild (tm);
      Language lang = TtaGetDefaultLanguage ();
      double d;
	    
      d = (x + 6 - ct_left_bar) / time_sep;
      sprintf (buffer, "%.2fs", (float) d);
      TtaSetTextContent (child, (unsigned char *)buffer, lang, timelinedoc);
    }
  Set_slider_position (basedoc, timelinedoc, f);
#endif /* _SVG */
}


/*----------------------------------------------------------------------
  TimelineTextPostModify
  callback on text modify
  ----------------------------------------------------------------------*/
void TimelineTextPostModify (NotifyOnTarget *event)
{	
#ifdef _SVG
  DisplayMode       dp = TtaGetDisplayMode (event->document);
  Language          lang = TtaGetDefaultLanguage ();
  Document          basedoc;
  Attribute         attr;
  AttributeType     attrType;
  Element           parent, tg, el;
  ElementType       elType;
  PresentationValue pval;
  char             *value = NULL, *s, buffer[512];
  int               length, firstchar, lastchar;
  double            d;
  pmapping_animated pmapping;

  basedoc = Get_basedoc_of (event->document);
  parent = TtaGetParent (event->element);
  elType = TtaGetElementType (parent);
  TtaSetDisplayMode (event->document, DeferredDisplay);
  if (elType.ElTypeNum == Timeline_EL_timing_text)
    {
      length = TtaGetTextLength (event->element);
      value = (char *)TtaGetMemory (length + 1);
      if (value) 
        {
          TtaGiveTextContent (event->element, (unsigned char *)value, &length, &lang);
          s = value;
          ParseNumber (s, &pval);
          TtaFreeMemory (value);
        }

      d = pval.typed_data.value;
      if (pval.typed_data.real)
        d /= 1000.0;
      Set_slider_position (basedoc, event->document, d);
    }
  else if (elType.ElTypeNum == Timeline_EL_text_id)
    {
      /* edit id in basedoc */
      tg = TtaGetParent (parent);
      pmapping = Get_mapping_from_title_group (basedoc, tg);
      
      elType = TtaGetElementType (pmapping->animated_elem);
      attrType.AttrSSchema = elType.ElSSchema;
      attrType.AttrTypeNum = SVG_ATTR_id;
      attr = TtaGetAttribute (pmapping->animated_elem, attrType);					
      if (!attr) 
        {
          attr = TtaNewAttribute (attrType);
          TtaAttachAttribute (pmapping->animated_elem, attr, basedoc);
        }

      TtaGiveFirstSelectedElement (basedoc, &el, &firstchar, &lastchar);
      TtaOpenUndoSequence (basedoc, el, el, firstchar, lastchar);
      TtaRegisterAttributeReplace (attr, pmapping->animated_elem, basedoc);
      length = TtaGetTextLength (event->element);
      value = (char *)TtaGetMemory (length + 2);
      if (value) 
        {
          TtaGiveTextContent (event->element, (unsigned char *)value, &length, &lang);
          TtaSetAttributeText (attr, value, pmapping->animated_elem, basedoc);  
          TtaFreeMemory (value);
        }
      /* Check attribute ID in order to make sure that its value */
      /* is unique in the document */
      MakeUniqueName (pmapping->animated_elem, basedoc, TRUE, TRUE);
      Get_id_of (pmapping->animated_elem, buffer);
      TtaSetTextContent (event->element, (unsigned char *)buffer, SPACE,
                         dt[basedoc].timelinedoc);
      TtaCloseUndoSequence (basedoc);
      TtaSetDocumentModified (basedoc);
    }

  TtaSetDisplayMode (event->document, dp);
  TtaSetDocumentUnmodified (event->document);
#endif /* _SVG */
}


/*----------------------------------------------------------------------
  Show_selected_element 
  draw selection in WYSIWYG view
  ----------------------------------------------------------------------*/
#ifdef _SVG
static void Show_selected_element (NotifyElement *event) 
{
  pmapping_animated mapping;
  Document          basedoc = Get_basedoc_of (event->document);
  Element           title_group = TtaGetParent (event->element);

  mapping = Get_mapping_from_title_group (basedoc, title_group);
  TtaSelectElement (basedoc, mapping->animated_elem);
}
#endif /* _SVG */




/*----------------------------------------------------------------------
  Show_selected_anim
  draw selection in WYSIWYG view
  ----------------------------------------------------------------------*/
#ifdef _SVG
static void Show_selected_anim (NotifyElement *event) 
{
  Attribute         attr;
  AttributeType     attrType;
  ElementType       elType;
  char             *text;
  int               length;
  char              buffer[512];
  pmapping_animated mapping;
  Element           anim;
  Document          basedoc = Get_basedoc_of (event->document);
  Element           exp_group = TtaGetParent (event->element);

  mapping = Get_mapping_from_exp_group (basedoc, exp_group);
  anim = Get_animation_tag_from_period (basedoc, exp_group, event->element);
  TtaSelectElement (basedoc, anim);

  /* display animated attribute in status bar : */
  elType = TtaGetElementType (anim);	
  attrType.AttrSSchema = elType.ElSSchema;
  attrType.AttrTypeNum = SVG_ATTR_attributeName_;
  attr = TtaGetAttribute (anim, attrType);
  if (attr) 
    {
      text = NULL;
      /* the attribute value is a character string */
      length = TtaGetTextAttributeLength (attr) + 2;
      text = (char *)TtaGetMemory (length);
      if (text) 
        {
          TtaGiveTextAttributeValue (attr, text, &length);
          strcpy (buffer, TtaGetMessage (AMAYA, AM_SVGANIM_ANIMATED_ATTR));
          strcat (buffer, " : ");
          strcat (buffer, text); 
          TtaSetStatus (event->document, dt[basedoc].timelineView, buffer, NULL);
          dt[basedoc].can_delete_status_text = TRUE;
          TtaFreeMemory (text);
        }
    }
}
#endif /* _SVG */


/*----------------------------------------------------------------------
  Enable_color_anim_creation
  ----------------------------------------------------------------------*/
#ifdef _SVG
static void Enable_color_anim_creation (Document basedoc) 
{
  char     buffer[512];
  Document timelinedoc = dt[basedoc].timelinedoc;

  Build_path_to_image_dir (buffer);
  strcat (buffer, ct_image_anim_color);
  Set_src (timelinedoc, dt[basedoc].image_color, buffer);

  dt[basedoc].color_animation_creation_enabled = TRUE;
}
#endif /* _SVG */


/*----------------------------------------------------------------------
  Enable_motion_anim_creation
  ----------------------------------------------------------------------*/
#ifdef _SVG
static void Enable_motion_anim_creation (Document basedoc) 
{
  char     buffer[512];
  Document timelinedoc = dt[basedoc].timelinedoc;

  Build_path_to_image_dir (buffer);
  strcat (buffer, ct_image_anim_motion);
  Set_src (timelinedoc, dt[basedoc].image_motion, buffer);

  dt[basedoc].motion_animation_creation_enabled = TRUE;
}
#endif /* _SVG */


/*----------------------------------------------------------------------
  Disable_color_anim_creation
  ----------------------------------------------------------------------*/
#ifdef _SVG
static void Disable_color_anim_creation (Document basedoc) 
{
  char     buffer[512];
  Document timelinedoc = dt[basedoc].timelinedoc;
	
  Build_path_to_image_dir (buffer);
  strcat (buffer, ct_image_anim_color_disabled);
  Set_src (timelinedoc, dt[basedoc].image_color, buffer);
  dt[basedoc].color_animation_creation_enabled = FALSE;
}
#endif /* _SVG */


/*----------------------------------------------------------------------
  Disable_motion_anim_creation
  ----------------------------------------------------------------------*/
#ifdef _SVG
static void Disable_motion_anim_creation (Document basedoc) 
{
  char     buffer[512];
  Document timelinedoc = dt[basedoc].timelinedoc;
	
  Build_path_to_image_dir (buffer);
  strcat (buffer, ct_image_anim_motion_disabled);
  Set_src (timelinedoc, dt[basedoc].image_motion, buffer);
  dt[basedoc].motion_animation_creation_enabled = FALSE;
}
#endif /* _SVG */

	



/*----------------------------------------------------------------------
  TimelineElSelection
  ----------------------------------------------------------------------*/
ThotBool TimelineElSelection (NotifyElement *event)
{
  ThotBool    res = FALSE;
#ifdef _SVG
  Element     parent;
  ElementType elType;
  Document    basedoc = Get_basedoc_of(event->document);
	
  parent = event->element;
  elType = TtaGetElementType (parent);

  /* Remove status bar text on next click after an animation has been defined */
  if (dt[basedoc].can_delete_status_text) 
    {
      TtaSetStatus (event->document, dt[basedoc].timelineView, ct_empty_string, NULL);
      dt[basedoc].can_delete_status_text = FALSE;

      /* Remove the arrow */
      TtaDeleteTree (dt[basedoc].arrow, dt[basedoc].timelinedoc);
      dt[basedoc].arrow = NULL;
    }

  /* an animated element is being selected, enable animation creation */
  if (elType.ElTypeNum == Timeline_EL_rect_id) 
    { 
      res = TRUE;
      Show_selected_element (event);	
      Enable_color_anim_creation (basedoc);
      Enable_motion_anim_creation (basedoc);
    } else if (elType.ElTypeNum == Timeline_EL_exp_period) { 
    res = TRUE;
    Show_selected_anim (event);	
  } 
  else 
    {
      /* an other timeline element is being selected, disable animation creation */
      Disable_color_anim_creation (basedoc);
      Disable_motion_anim_creation (basedoc);
	
      /* forbid "Document" selection :*/
      res = TRUE;

      if (TtaGetParent (parent)) 
        {
          elType = TtaGetElementType (TtaGetParent (parent));
          if (elType.ElTypeNum == Timeline_EL_timing_text ||
              elType.ElTypeNum == Timeline_EL_text_id)
            res = FALSE;
        }
    }

  TtaSetDocumentUnmodified (event->document);

#endif /* _SVG */
  return res;
}


/*----------------------------------------------------------------------
  Apply_hpos_mini
  Forbids element's position to be < xmin 
  ----------------------------------------------------------------------*/
#ifdef _SVG
static int Apply_hpos_mini (Document basedoc, Element el, int xmin) 
{
  int           x = 0;
  char          buffer[512];
  Attribute     attr;
  AttributeType attrType;
  ElementType   elType;
  PRule         presRuleX;

  presRuleX = TtaGetPRule (el, PRHorizPos);
  if (presRuleX)
    x = TtaGetPRuleValue (presRuleX);
  x = max (x, xmin/**TtaGetZoom(dt[basedoc].timelinedoc,dt[basedoc].timelineView)*/);

  sprintf (buffer, "%dpx", x);
  elType = TtaGetElementType (el);

  attrType.AttrSSchema = elType.ElSSchema;
  attrType.AttrTypeNum = Timeline_ATTR_x;

  attr = TtaGetAttribute (el, attrType);
  if (attr) 
    {
      TtaSetAttributeText (attr, buffer, el, dt[basedoc].timelinedoc);
      TimelineParseCoordAttribute (attr, el, dt[basedoc].timelinedoc);
    }
  return x;
}
#endif /* _SVG */


/*----------------------------------------------------------------------
  Apply_new_width
  ----------------------------------------------------------------------*/
#ifdef _SVG
static int Apply_new_width (Document doc, Element el) 
{
  int           nw = 0;
  char          buffer[512];
  Attribute     attr;
  AttributeType attrType;
  ElementType   elType;
  PRule         presRuleX;

  presRuleX = TtaGetPRule (el, PRWidth);
  if (presRuleX)
    nw = TtaGetPRuleValue (presRuleX);
  sprintf (buffer, "%dpx", nw);
  elType = TtaGetElementType (el);

  attrType.AttrSSchema = elType.ElSSchema;
  attrType.AttrTypeNum = Timeline_ATTR_width_;

  attr = TtaGetAttribute (el, attrType);
  if (attr) 
    {
      TtaSetAttributeText (attr, buffer, el, doc);
      TimelineParseWidthHeightAttribute (attr, el, doc, FALSE);
    }
  return nw;
}
#endif /* _SVG */







/*----------------------------------------------------------------------
  Generate_animate_color
  ----------------------------------------------------------------------*/
#ifdef _SVG
static void Generate_animate_color (Document basedoc, int from_c, int to_c,
                                    double start, double duration,
                                    pmapping_animated pm, Element period) 
{
  ElementType    elType, anType;
  Element        animtag, lastchild;
  AttributeType  attrType;
  Attribute      attr;
  char           buffer[512];
  unsigned short r,g,b;
  ThotBool       sav;

  elType.ElSSchema = TtaGetSSchema ("SVG", basedoc);
  elType.ElTypeNum = SVG_EL_animateColor;
  animtag = TtaNewElement (basedoc, elType);

  /* set attributes */
  attrType.AttrSSchema = elType.ElSSchema;

  /* begin */
  attrType.AttrTypeNum = SVG_ATTR_begin_;
  attr = TtaNewAttribute (attrType);
  TtaAttachAttribute (animtag, attr, basedoc);
  sprintf (buffer, "%.2fs", (float) (max (0.0, start)));	
  TtaSetAttributeText (attr, buffer, animtag, basedoc);

  /* dur */
  attrType.AttrTypeNum = SVG_ATTR_dur;
  attr = TtaNewAttribute (attrType);
  TtaAttachAttribute (animtag, attr, basedoc);
  sprintf (buffer, "%.2fs", (float) duration);	
  TtaSetAttributeText (attr, buffer, animtag, basedoc);

	
  /* from */
  attrType.AttrTypeNum = SVG_ATTR_from;
  attr = TtaNewAttribute (attrType);
  TtaAttachAttribute (animtag, attr, basedoc);
  TtaGiveThotRGB (from_c, &r, &g, &b);
  /* sprintf (buffer, "#%02X%02X%02X", r, g, b);*/
  sprintf (buffer, "rgb(%d,%d,%d)", r, g, b);
  TtaSetAttributeText (attr, buffer, animtag, basedoc);

  /* to */
  attrType.AttrTypeNum = SVG_ATTR_to_;
  attr = TtaNewAttribute (attrType);
  TtaAttachAttribute (animtag, attr, basedoc);
  TtaGiveThotRGB (to_c, &r, &g, &b);
  sprintf (buffer, "rgb(%d,%d,%d)", r, g, b);
  TtaSetAttributeText (attr, buffer, animtag, basedoc);


  /* fill */
  attrType.AttrTypeNum = SVG_ATTR_fill_;
  attr = TtaNewAttribute (attrType);
  TtaAttachAttribute (animtag, attr, basedoc);
  TtaSetAttributeValue (attr, SVG_ATTR_fill__VAL_freeze, animtag, basedoc);


  /* AttributeType */
  attrType.AttrTypeNum = SVG_ATTR_attributeType;
  attr = TtaNewAttribute (attrType);
  TtaAttachAttribute (animtag, attr, basedoc);
  strcpy(buffer,"CSS");	
  TtaSetAttributeText (attr, buffer, animtag, basedoc);


  /* AttributeName */
  attrType.AttrTypeNum = SVG_ATTR_attributeName_;
  attr = TtaNewAttribute (attrType);
  TtaAttachAttribute (animtag, attr, basedoc);

  anType = TtaGetElementType (pm->animated_elem);
  strcpy(buffer,"fill");
  TtaSetAttributeText (attr, buffer, animtag, basedoc);

  /* insert */ 
  lastchild = TtaGetLastChild (pm->animated_elem);

  sav = TtaGetStructureChecking (basedoc);
  TtaSetStructureChecking (FALSE, basedoc);
  TtaInsertSibling (animtag, lastchild, FALSE, basedoc);
  TtaSetStructureChecking (sav, basedoc);

  /* update mapping of <animate> tags */
  pm->mapping_animations[pm->nb_periods].animation_tag = animtag;
  pm->mapping_animations[pm->nb_periods].period = period;
  pm->nb_periods = pm->nb_periods + 1;

  if ((pm->nb_periods >= 2) && (!Image_collapse_for_title_group (basedoc,
                                                                 pm->animated_elem)))
    Add_image_collapse_on_title_group (basedoc, pm->animated_elem, 0);

  TtaSetDocumentModified (basedoc);
}
#endif /* _SVG */


/*----------------------------------------------------------------------
  Generate_animate_motion
  ----------------------------------------------------------------------*/
#ifdef _SVG
static void Generate_animate_motion (Document basedoc, int previous_x, int previous_y,
                                     int x, int y, double start, double duration,
                                     pmapping_animated pm, Element period) 
{
  ElementType   elType;
  ThotBool      sav;
  Element       animtag, lastchild;
  AttributeType attrType;
  Attribute     attr;
  char          buffer[512];

  elType.ElSSchema = TtaGetSSchema ("SVG", basedoc);
  elType.ElTypeNum = SVG_EL_animateMotion;
  animtag = TtaNewElement (basedoc, elType);

  /* set attributes */
  attrType.AttrSSchema = elType.ElSSchema;

  /* begin */
  attrType.AttrTypeNum = SVG_ATTR_begin_;
  attr = TtaNewAttribute (attrType);
  TtaAttachAttribute (animtag, attr, basedoc);
  sprintf (buffer, "%.2fs", (float) (max (0.0, start)));	
  TtaSetAttributeText (attr, buffer, animtag, basedoc);

  /* dur */
  attrType.AttrTypeNum = SVG_ATTR_dur;
  attr = TtaNewAttribute (attrType);
  TtaAttachAttribute (animtag, attr, basedoc);
  sprintf (buffer, "%.2fs", (float) duration);	
  TtaSetAttributeText (attr, buffer, animtag, basedoc);


  /* path */
  attrType.AttrTypeNum = SVG_ATTR_path_;
  attr = TtaNewAttribute (attrType);
  TtaAttachAttribute (animtag, attr, basedoc);
  sprintf (buffer, "M %d %d L %d %d", 0, 0, x-previous_x, y-previous_y);
  TtaSetAttributeText (attr, buffer, animtag, basedoc);


  /* fill */
  attrType.AttrTypeNum = SVG_ATTR_fill_;
  attr = TtaNewAttribute (attrType);
  TtaAttachAttribute (animtag, attr, basedoc);
  TtaSetAttributeValue (attr, SVG_ATTR_fill__VAL_freeze, animtag, basedoc);

  /* insert */ 
  lastchild = TtaGetLastChild (pm->animated_elem);


  sav = TtaGetStructureChecking (basedoc);
  TtaSetStructureChecking (FALSE, basedoc);
  TtaInsertSibling (animtag, lastchild, FALSE, basedoc);
  TtaSetStructureChecking (sav, basedoc);


  /* update mapping of <animate> tags */
  pm->mapping_animations[pm->nb_periods].animation_tag = animtag;
  pm->mapping_animations[pm->nb_periods].period = period;
  pm->nb_periods = pm->nb_periods + 1;

  if ((pm->nb_periods >= 2) && (!Image_collapse_for_title_group (basedoc,
                                                                 pm->animated_elem)))
    Add_image_collapse_on_title_group (basedoc, pm->animated_elem, 0);

  TtaSetDocumentModified (basedoc);
}
#endif /* _SVG */


/*----------------------------------------------------------------------
  Timeline_finished_moving_slider
  A Callback : the slider has been moved 
  ----------------------------------------------------------------------*/
void Timeline_finished_moving_slider(NotifyPresentation *event)
{
#ifdef _SVG
  char        buffer[512];
  Document    basedoc = Get_basedoc_of (event->document);
  Element     tm = dt[basedoc].timing_text, egroup = NULL, brect, insertel;
  DisplayMode dp;
  Element     new_period;
  int         vpos, fgcolor, bgcolor, x;
  double      end, d;

  /* move the slider and enlarge timeline if necessary */
  x = Apply_hpos_mini (basedoc, event->element, ct_left_bar-6);
  if (x>ct_left_bar+current_timeline_width)
    Enlarge_timeline (basedoc, event->document, x);
	
  end = (x+6-(ct_left_bar+1))/time_sep;
	
  /* update timing_text */
  d = (x+6-ct_left_bar)/time_sep;
  if (tm)
    {
      Element child = TtaGetFirstChild (tm);
      Language lang = TtaGetDefaultLanguage();
	    
      
      sprintf (buffer, "%.2fs", (float) d);
      TtaSetTextContent (child, (unsigned char *)buffer, lang, event->document);
    }

#ifdef _GL	
  TtaSetDocumentCurrentTime (d, basedoc);
#endif /* _GL */

  /* editing modes */
	
  if ((dt[basedoc].definition_of_motion_period) || 
      (dt[basedoc].definition_of_color_period))
    {
      egroup = dt[basedoc].current_edited_mapping->exp_group;
      brect = TtaGetFirstChild (egroup);
      insertel = TtaGetLastChild (egroup);
	    
      /* define animation period: */	
      vpos = Get_height_of (brect) - ct_static_bar_height - 6 + ct_offset_y_period;
	    
      if (dt[basedoc].definition_of_motion_period)
        {
          new_period = Insert_rectangle (event->document, insertel,
                                         ct_animatemotion_color,
                                         "none", previous_slider_position+6,
                                         vpos, x - previous_slider_position, 
                                         ct_period_height, 0, Timeline_EL_exp_period);
		
		
		
          /* generate animation in basedoc */
          Generate_animate_motion (basedoc, dt[basedoc].previous_x,
                                   dt[basedoc].previous_y, dt[basedoc].x,
                                   dt[basedoc].y,
                                   dt[basedoc].anim_start, 
                                   end-dt[basedoc].anim_start,
                                   dt[basedoc].current_edited_mapping,new_period);
		
          dt[basedoc].definition_of_motion_period = FALSE;
          TtaSetStatus (event->document, dt[basedoc].timelineView,
                        TtaGetMessage (AMAYA, AM_SVGANIM_ANIM_DEFINED), NULL);
          dt[basedoc].can_delete_status_text = TRUE;
		
          dt[basedoc].current_edited_mapping = NULL;
		
          /* Remove the cross */
          if (dt[basedoc].cross)
            Delete_cross (basedoc);
		
		
        }
      else
        if (dt[basedoc].definition_of_color_period)
          {
            new_period = Insert_rectangle (event->document, insertel,
                                           ct_animatecolor_color,
                                           "none", previous_slider_position+6,
                                           vpos, x - previous_slider_position, 
                                           ct_period_height, 0, Timeline_EL_exp_period);
		  
            TtaSetStatus ( dt[basedoc].timelinedoc, dt[basedoc].timelineView,
                           TtaGetMessage (AMAYA, AM_SVGANIM_COLOR_HINT3), NULL);
		  
            /* force a redisplay before letting the user choose ending color */
            dp = TtaGetDisplayMode (event->document);
            TtaSetDisplayMode (event->document, NoComputedDisplay);
            TtaSetDisplayMode (event->document, DisplayImmediately);
            TtaSetDisplayMode (event->document, dp);
            /*****	TtcGetPaletteColors (&fgcolor, &bgcolor, TRUE); ****/
            fgcolor = Current_Color;
            bgcolor = Current_BackgroundColor;
            /* force a redisplay after letting the user choose ending color */
            dp = TtaGetDisplayMode (event->document);
            TtaSetDisplayMode (event->document, NoComputedDisplay);
            TtaSetDisplayMode (event->document, DisplayImmediately);
            TtaSetDisplayMode (event->document, dp);
		  
		  
            /* generate animation in basedoc */
            Generate_animate_color (basedoc, dt[basedoc].bgcolor_start,
                                    bgcolor, dt[basedoc].anim_start, 
                                    end-dt[basedoc].anim_start,
                                    dt[basedoc].current_edited_mapping,new_period);
		  
            dt[basedoc].definition_of_color_period = FALSE;			
            dt[basedoc].current_edited_mapping = NULL;
		  
            TtaSetStatus ( dt[basedoc].timelinedoc, dt[basedoc].timelineView,
                           TtaGetMessage (AMAYA, AM_SVGANIM_ANIM_DEFINED), NULL);
            dt[basedoc].can_delete_status_text = TRUE;
          }
    }
	
  TtaSetDocumentUnmodified (event->document);
#endif /* _SVG */
}


/*----------------------------------------------------------------------
  Timeline_exp_period_modified
  A Callback: an exp_period element has been resized or moved
  ----------------------------------------------------------------------*/
void Timeline_exp_period_modified (NotifyPresentation *event)
{
#ifdef _SVG
  Document basedoc = Get_basedoc_of (event->document);
  Element  edited_anim, exp_group, el;
  double   start, duration;
  int      nw, x, firstchar, lastchar;

  /* visual retroaction*/
  x = Apply_hpos_mini (basedoc, event->element, ct_left_bar);
  start = (x+6-(ct_left_bar+1))/time_sep;
  nw = Apply_new_width (event->document, event->element);
  x += nw;
  duration = nw/time_sep;
  if (x>ct_left_bar+current_timeline_width)
    Enlarge_timeline (basedoc, event->document, x);
	
  exp_group = TtaGetParent (event->element);

  TtaGiveFirstSelectedElement (basedoc, &el, &firstchar, &lastchar);
  TtaOpenUndoSequence (basedoc, el, el, firstchar, lastchar);
	
  /* basedoc modification */
  edited_anim = Get_animation_tag_from_period (basedoc, exp_group, event->element);
  Write_time_info (basedoc, edited_anim, start, duration);

  TtaCloseUndoSequence (basedoc);
  TtaSetDocumentModified (basedoc);
  TtaSetDocumentUnmodified (event->document);
#endif /* _SVG */
}


/*----------------------------------------------------------------------
  Timeline_col_period_pre_modify
  A Callback: a col_period element is about to be moved
  ----------------------------------------------------------------------*/
ThotBool Timeline_col_period_pre_modify (NotifyPresentation *event)
{
  ThotBool res = FALSE;

#ifdef _SVG
  tmp_previous_x = event->value;
#endif /* _SVG */
  return res;
}


/*----------------------------------------------------------------------
  Timeline_slider_pre_modify
  Callback: the slider element is about to be moved
  ----------------------------------------------------------------------*/
ThotBool Timeline_slider_pre_modify (NotifyPresentation *event)
{
  ThotBool      res = FALSE;
#ifdef _SVG
  PRule         presRuleX;

  presRuleX = TtaGetPRule (event->element, PRHorizPos);
  if (presRuleX)
    previous_slider_position = TtaGetPRuleValue (presRuleX);
  else 
    previous_slider_position = 0;
#endif /* _SVG */
  return res;
}


/*----------------------------------------------------------------------
  Timeline_col_period_modified
  A Callback: a col_period element has been moved
  ----------------------------------------------------------------------*/
void Timeline_col_period_modified (NotifyPresentation *event)
{
#ifdef _SVG

  Document basedoc = Get_basedoc_of (event->document);
  Element  col_group, animated_elem, edited_anim, el;
  int      nw, x, firstchar, lastchar;
  double   start, duration;
  double   inc_time;

  /* visual retroaction*/
  x = Apply_hpos_mini (basedoc, event->element, ct_left_bar);
  start = (x+6-(ct_left_bar+1))/time_sep;
  nw = Get_width_of (event->element);
  duration = nw/time_sep;
  if (x + nw > ct_left_bar+current_timeline_width)
    Enlarge_timeline (basedoc, event->document, x + nw);
	
  col_group = TtaGetParent (event->element);

  /* basedoc modification */
  /* modify the attribute start="" of all childrens */ 
  animated_elem = Get_animated_element_of_col_group (basedoc, col_group);

  inc_time = (x - tmp_previous_x) / time_sep;

  TtaGiveFirstSelectedElement (basedoc, &el, &firstchar, &lastchar);
  TtaOpenUndoSequence (basedoc, el, el, firstchar, lastchar);
	
  edited_anim = Search_first_anim_in_tree (basedoc, animated_elem);
  while ((edited_anim) && (TtaGetParent (edited_anim) == animated_elem)) 
    {
      Inc_time_start (basedoc, edited_anim, inc_time);
      edited_anim = Search_next_anim_in_tree (basedoc, edited_anim);
    }

  TtaCloseUndoSequence (basedoc);
  TtaSetDocumentModified (basedoc);
  TtaSetDocumentUnmodified (event->document);
#endif /* _SVG */
}


/*----------------------------------------------------------------------
  TimelineWindowClose
  A Callback: a timeline window has been closed
  ----------------------------------------------------------------------*/
void TimelineWindowClose (NotifyDialog * event) 
{
#ifdef _SVG
  int k = 0;

  while ((k<DocumentTableLength) && dt[k].timelinedoc!=event->document) 
    k++;	
  if (k!=DocumentTableLength) 
    Init_timeline_struct(k);
  FreeDocumentResource (event->document);
#endif /* _SVG */
}


/*----------------------------------------------------------------------
  Update_period_position_and_size
  ----------------------------------------------------------------------*/
void Update_period_position_and_size (Document basedoc, Element animtag) 
{
#ifdef _SVG
  double            start, duration;
  Element           pr, animated_el, first_found, new_fresh_group;
  pmapping_animated pm;
  int               x_start, x, vpos;
	
  animated_el = TtaGetParent (animtag);
  pm = Get_mapping_from_animated_elem (basedoc, animated_el);
  Read_time_info (animtag, &start, &duration);

  x_start = Time_to_xposition (start);
  duration *= time_sep;
  x = x_start + (int) duration;

  if (pm) 
    {
      /* 2 cases : collapsed or expanded */
      if (pm->exp_group) 
        {
          /* expanded */
          pr = Get_period_from_animation_tag (pm, animtag);
          Set_x (dt[basedoc].timelinedoc, pr, x_start);
          Set_width (dt[basedoc].timelinedoc, pr, (int) duration);
        } 
      else /* collapsed */ 
        {
          /* recompute group */	
			
          first_found = Search_first_anim_in_tree (basedoc, pm->animated_elem);
          vpos = Get_y_of (TtaGetFirstChild (pm->title_group));
          Create_collapsed_group (basedoc, dt[basedoc].timelinedoc, vpos,
                                  pm->animated_elem, pm, first_found);
          new_fresh_group = dt[basedoc].current_el;

          /* delete old collapsed group */
          TtaDeleteTree (pm->col_group, dt[basedoc].timelinedoc); 
				
          /* update mapping*/
          Set_collapsed_group_of (basedoc, pm->animated_elem, new_fresh_group);
        }
    }
  if (x>ct_left_bar+current_timeline_width)
    Enlarge_timeline (basedoc, dt[basedoc].timelinedoc, x);
#endif /* _SVG */
}


/*----------------------------------------------------------------------
  Update_element_id_on_timeline
  ----------------------------------------------------------------------*/
void Update_element_id_on_timeline (NotifyAttribute* event)
{
#ifdef _SVG
  Element           animated_el, text_id;
  pmapping_animated pm;
  Document          basedoc = event->document, timelinedoc;
  char              buffer[512], *utf8val;
  ElementType       elType;

  timelinedoc = dt[basedoc].timelinedoc;
  animated_el = event->element;
  pm = Get_mapping_from_animated_elem (basedoc, animated_el);
	
  if (pm) 
    {
      elType = TtaGetElementType (TtaGetMainRoot (timelinedoc)); 
      elType.ElTypeNum = Timeline_EL_text_id;
      text_id = TtaSearchTypedElement(elType, SearchInTree, pm->title_group); 
      /* get thot text el*/
      text_id = TtaGetFirstChild (text_id);
	
      if (text_id) 
        {
          if (event->event == TteAttrDelete) 
            {
              utf8val = (char *)TtaConvertByteToMbs ((unsigned char *)
                                                     TtaGetMessage (AMAYA, AM_SVGANIM_NO_ID),
                                                     TtaGetDefaultCharset ());
              TtaSetTextContent (text_id, (unsigned char *)utf8val,
                                 SPACE, timelinedoc);
              TtaFreeMemory (utf8val);
            }
          else if (event->event == TteAttrCreate) 
            {
              if (Get_id_of (event->element, buffer))
                TtaSetTextContent (text_id,
                                   (unsigned char *)buffer, SPACE, timelinedoc);
              else
                TtaSetTextContent (text_id, (unsigned char *)"",
                                   SPACE, timelinedoc);
            }
          else if (event->event == TteAttrModify) 
            {
              if (Get_id_of (event->element, buffer))
                TtaSetTextContent (text_id, (unsigned char *)buffer,
                                   SPACE, timelinedoc);
              else
                TtaSetTextContent (text_id, (unsigned char *)"",
                                   SPACE, timelinedoc);
            } 
        }
    }
#endif /* _SVG */
}


/*----------------------------------------------------------------------
  Prepare_and_add_a_period
  ----------------------------------------------------------------------*/
#ifdef _SVG
static void Prepare_and_add_a_period (Document basedoc, Document timelinedoc,
                                      pmapping_animated pm)
{
  Element exp_rect, im,first_found, new_fresh_group;
  int     nh, vpos, hg;
  char    buffer[512];

  if (!(pm->exp_group)) 
    { /* if group is collapsed then expand group :*/
      first_found = Search_first_anim_in_tree (basedoc, pm->animated_elem);
      vpos = Get_y_of (TtaGetFirstChild (pm->title_group));
      Create_expanded_group (basedoc, dt[basedoc].timelinedoc, vpos,
                             pm->animated_elem, first_found, pm, &hg);
      new_fresh_group = dt[basedoc].current_el;

      /* delete old collapsed group */
      TtaDeleteTree (pm->col_group, dt[basedoc].timelinedoc); 
				
      /* update mapping*/
      Set_expanded_group_of (basedoc, pm->animated_elem, new_fresh_group);
      Set_collapsed_group_of (basedoc, pm->animated_elem, NULL);

      /* translate visible groups on the interface */
      Translate_following_elements (basedoc, timelinedoc, vpos,
                                    hg-ct_collapsed_group_height);
      dt[basedoc].vertical_pos += hg-ct_collapsed_group_height;

      /* set image collapsed to expanded */
      im = pm->title_group;
      /* image_collapse is the second child of title_group*/
      im = TtaGetFirstChild (im);
      TtaNextSibling (&im);

      Build_path_to_image_dir (buffer);
      strcat (buffer, ct_image_c1_filename);
      Set_src (timelinedoc, im, buffer);
    }

  exp_rect = TtaGetFirstChild (pm->exp_group);
  Inc_height (timelinedoc, exp_rect, ct_static_bar_height+6);
  dt[basedoc].vertical_pos += ct_static_bar_height+6;
  nh = Get_height_of (exp_rect);
  Insert_rectangle (timelinedoc, exp_rect, ct_static_bar_color,
                    ct_static_bar_color,
                    ct_left_bar,  nh - (6+ct_static_bar_height), 
                    current_timeline_width, ct_static_bar_height, 0,
                    Timeline_EL_rect_interface);

  Translate_following_elements (basedoc, timelinedoc, Get_y_of (exp_rect),
                                ct_static_bar_height+6);

  /* put an arrow */
  Build_path_to_image_dir (buffer);
  strcat(buffer, ct_image_arrow);

  dt[basedoc].arrow = Insert_image (timelinedoc, exp_rect, buffer, 
                                    ct_left_bar-ct_w_image_arrow-3,
                                    nh - (6+ct_static_bar_height)-4,
                                    ct_w_image_arrow, ct_h_image_arrow, 0,
                                    Timeline_EL_image_toolbar);
}
#endif /* _SVG */


/*----------------------------------------------------------------------
  Key_position_defined
  ----------------------------------------------------------------------*/
void Key_position_defined (Document basedoc, Element cross) 
{
#ifdef _SVG
  PRule         presRule;

  presRule = TtaGetPRule (cross, PRHorizPos);
  if (presRule)
    dt[basedoc].x = TtaGetPRuleValue (presRule);
  presRule = TtaGetPRule (cross, PRVertPos);
  if (presRule)
    dt[basedoc].y = TtaGetPRuleValue (presRule);

  dt[basedoc].definition_of_motion_period = TRUE;
  TtaSetStatus ( dt[basedoc].timelinedoc, dt[basedoc].timelineView,
                 TtaGetMessage (AMAYA, AM_SVGANIM_MOTION_HINT2), NULL);

#endif /* _SVG */
}


/*----------------------------------------------------------------------
  Define_motion_anim
  ----------------------------------------------------------------------*/
#ifdef _SVG
static void Define_motion_anim (NotifyElement *event)
{
  Element           el;
  ElementType       elType;
  int               fc, lc, h, x = 0;
  pmapping_animated pm;
  PRule             presRuleX;
  Document          basedoc = Get_basedoc_of (event->document);
  Document          timelinedoc = event->document;

  DisplayMode dp = TtaGetDisplayMode (event->document);
  TtaSetDisplayMode (event->document, DeferredDisplay);
  TtaGiveFirstSelectedElement (basedoc, &el, &fc, &lc);
  /* handle multiple selections with TtaGiveNextSelectedElement  ? */

  elType = TtaGetElementType (el);
  if ((elType.ElTypeNum == SVG_EL_TEXT_UNIT)
      || (elType.ElTypeNum == SVG_EL_GRAPHICS_UNIT)
      || (elType.ElTypeNum == SVG_EL_SYMBOL_UNIT)
      || (elType.ElTypeNum == SVG_EL_PICTURE_UNIT))
    el = TtaGetParent (el);

  /* check if the node already has animations */
  if (!Is_animated (basedoc, el)) 
    {  
      /* if the node has no animation yet, create a title group, ... */ 
		
      dt[basedoc].nb_relations_animated++;
      pm = &(dt[basedoc].mapping_animated[dt[basedoc].nb_relations_animated-1]);

      /* update pm of animated element as soon as created */
      pm->animated_elem = el;

      Create_title_group (basedoc, event->document, dt[basedoc].vertical_pos, el,0);

      /* maps title group */
      pm->title_group = dt[basedoc].current_el;

      Create_expanded_group (basedoc, dt[basedoc].timelinedoc, dt[basedoc].vertical_pos, 
                             el, NULL, pm, &h);
      dt[basedoc].vertical_pos += h+2;
      pm->exp_group = dt[basedoc].current_el;

      Remove_image_collapse_from_title_group (basedoc, pm->animated_elem);
    }

  /* show expanded group, enlarge it and build a new period */
  pm = Get_mapping_from_animated_elem (basedoc, el);
  Prepare_and_add_a_period (basedoc, timelinedoc, pm);
  elType = TtaGetElementType (pm->animated_elem);
  dt[basedoc].previous_x = Get_center_x_of_SVG_el (pm->animated_elem);
  dt[basedoc].previous_y = Get_center_y_of_SVG_el (pm->animated_elem);
	
  Display_cross (basedoc,
                 pm->animated_elem,
                 dt[basedoc].previous_x - ct_w_image_cross/2 ,
                 dt[basedoc].previous_y - ct_h_image_cross/2);

  dt[basedoc].current_edited_mapping = pm;

  /* get "from" value */
  presRuleX = TtaGetPRule (dt[basedoc].slider, PRHorizPos);
  if (presRuleX)
    x = TtaGetPRuleValue (presRuleX);
  dt[basedoc].anim_start = (x+6-(ct_left_bar+1))/time_sep;


  TtaSetStatus (event->document, dt[basedoc].timelineView,
                TtaGetMessage (AMAYA, AM_SVGANIM_MOTION_HINT1), NULL);
  TtaSetDisplayMode (event->document, dp);

}
#endif /* _SVG */


/*----------------------------------------------------------------------
  Show_timeline_help 
  ----------------------------------------------------------------------*/
#ifdef _SVG
static void Show_timeline_help (NotifyElement *event)
{
#define x_help 100
#define y_help 200
#define w_help 400
#define h_help 300
  Document basedoc;
  char     buffer[512];

  Build_path_to_help_dir (buffer);
  strcat (buffer, ct_timeline_help_file);
  basedoc = Get_basedoc_of (event->document);
  if (!dt[basedoc].helpdoc) 
    {
#ifdef _WX
      /* open a new window to display the new document */
      dt[basedoc].helpdoc = GetAmayaDoc (buffer, NULL, 0, basedoc, CE_HELP,
                                         FALSE, FALSE, FALSE);
#endif /* _WX */
      /* set the document in Read Only mode */
      TtaSetDocumentAccessMode (dt[basedoc].helpdoc, 0);
    }
  else 
    {
      /* help window already shown so close help window */
      TtaCloseView (dt[basedoc].helpdoc, 1);
      FreeDocumentResource (dt[basedoc].helpdoc);
      TtaCloseDocument (dt[basedoc].helpdoc);
      dt[basedoc].helpdoc = 0;
      dt[basedoc].helpview = 0;
    }
}
#endif /* _SVG */


/*----------------------------------------------------------------------
  Selection_changed_in_basedoc 
  ----------------------------------------------------------------------*/
void Selection_changed_in_basedoc (NotifyElement * event)
{
#ifdef _SVG
  ElementType elType;
  Element     el = TtaGetParent (event->element);

  elType = TtaGetElementType (el);
  elType.ElTypeNum = SVG_EL_animateMotion;
  if (TtaCanInsertFirstChild (elType, el, event->document))
    Enable_motion_anim_creation (event->document);
  else
    Disable_motion_anim_creation (event->document);

  elType.ElTypeNum = SVG_EL_animateColor;
  if (TtaCanInsertFirstChild (elType, el, event->document))
    Enable_color_anim_creation (event->document);
  else
    Disable_color_anim_creation (event->document);
#endif /* _SVG */
}



/*----------------------------------------------------------------------
  Define_color_anim 
  ----------------------------------------------------------------------*/
#ifdef _SVG
static void Define_color_anim (NotifyElement *event)
{
  Element           el;
  ElementType       elType;
  int               fc, lc, x = 0, h;
  pmapping_animated pm;
  DisplayMode       dp;
  PRule             presRuleX;
  Document          basedoc = Get_basedoc_of (event->document);
  Document          timelinedoc = event->document;

  TtaGiveFirstSelectedElement (basedoc, &el, &fc, &lc);
  /* handle multiple selections with TtaGiveNextSelectedElement  ? */
  elType = TtaGetElementType (el);
  if ((elType.ElTypeNum == SVG_EL_TEXT_UNIT)
      || (elType.ElTypeNum == SVG_EL_GRAPHICS_UNIT)
      || (elType.ElTypeNum == SVG_EL_SYMBOL_UNIT)
      || (elType.ElTypeNum == SVG_EL_PICTURE_UNIT))
    el = TtaGetParent (el);

  /* check if the node already has animations */
  if (!Is_animated (basedoc, el)) 
    {  
      /* if the node has no animation yet, create a title group, ... */ 
      dt[basedoc].nb_relations_animated++;
      pm = &(dt[basedoc].mapping_animated[dt[basedoc].nb_relations_animated-1]);

      /* update pm of animated element as soon as created */
      pm->animated_elem = el;
      Create_title_group (basedoc, event->document, dt[basedoc].vertical_pos, el,0);
      /* maps title group */
      pm->title_group = dt[basedoc].current_el;

      Create_expanded_group (basedoc, dt[basedoc].timelinedoc, dt[basedoc].vertical_pos, 
                             el, NULL, pm, &h);
      dt[basedoc].vertical_pos += h+2;
      pm->exp_group = dt[basedoc].current_el;

      Remove_image_collapse_from_title_group (basedoc, pm->animated_elem);
    }


  pm = Get_mapping_from_animated_elem (basedoc, el);

  dp = TtaGetDisplayMode (event->document);
  TtaSetDisplayMode (event->document, DeferredDisplay);
  Prepare_and_add_a_period (basedoc, timelinedoc, pm);
  TtaSetDisplayMode (event->document, dp);
	
  dt[basedoc].current_edited_mapping = pm;
  TtaSetStatus (event->document, dt[basedoc].timelineView,
                TtaGetMessage (AMAYA, AM_SVGANIM_COLOR_HINT1), NULL);

  /* get starting color values */
  /*****	TtcGetPaletteColors (&fgcolor, &bgcolor, TRUE); ****/
  dt[basedoc].fgcolor_start = Current_Color;
  dt[basedoc].bgcolor_start = Current_BackgroundColor;
  /* get "from" value */
  presRuleX = TtaGetPRule (dt[basedoc].slider, PRHorizPos);
  if (presRuleX)
    x = TtaGetPRuleValue (presRuleX);
  dt[basedoc].anim_start = (x+6-(ct_left_bar+1))/time_sep;

  /* switch to edit mode*/
  dt[basedoc].definition_of_color_period = TRUE;
  TtaSetStatus ( dt[basedoc].timelinedoc, dt[basedoc].timelineView,
                 TtaGetMessage (AMAYA, AM_SVGANIM_COLOR_HINT2), NULL);
}
#endif /* _SVG */


/*----------------------------------------------------------------------
  TimelineElClicked
  General callback on left click
  ----------------------------------------------------------------------*/
ThotBool TimelineElClicked (NotifyElement *event)
{
  ThotBool    res = FALSE;
#ifdef _SVG
  Element     parent;
  ElementType elType;
  Document    basedoc = Get_basedoc_of (event->document);
	
  parent = TtaGetParent (event->element);
  elType = TtaGetElementType (parent);
  if (elType.ElTypeNum == Timeline_EL_image_collapse) 
    Image_collapse_on_click(event);
  else if (elType.ElTypeNum == Timeline_EL_image_color) 
    {
      if (dt[basedoc].color_animation_creation_enabled) 
        {
          Disable_motion_anim_creation (basedoc);
          Disable_color_anim_creation (basedoc);
          Define_color_anim (event);
        }
    }
  else if (elType.ElTypeNum == Timeline_EL_image_motion) 
    {
      if (dt[basedoc].motion_animation_creation_enabled) 
        {
          Disable_color_anim_creation (basedoc);
          Disable_motion_anim_creation (basedoc);
          Define_motion_anim (event);
        }
    }
  else if (elType.ElTypeNum == Timeline_EL_image_help)
    Show_timeline_help (event);

  TtaSetDocumentUnmodified (event->document);
#endif /* _SVG */
  return res;
}
