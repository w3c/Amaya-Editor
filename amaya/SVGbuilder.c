/*
 *
 *  (c) COPYRIGHT INRIA and W3C, 1998-2010
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 *
 * SVGbuilder
 *
 * Authors: V. Quint
 *          I. Vatton
 *          P. Cheyrou-Lagreze
 */

#define THOT_EXPORT extern
#include "amaya.h"
#include "content.h"
#include "css.h"
#include "HTML.h"
#include "parser.h"
#include "registry.h"
#include "style.h"
#include "SVG.h"
#include "XLink.h"

#define MaxMsgLength 200

#include "anim_f.h"
#include "animbuilder_f.h"
#include "css_f.h"
#include "fetchXMLname_f.h"
#include "html2thot_f.h"
#include "HTMLactions_f.h"
#include "styleparser_f.h"
#include "SVGbuilder_f.h"
#include "Xml2thot_f.h"
#include "SVGedit_f.h"

/*----------------------------------------------------------------------
  SVGGetDTDName
  Return in DTDname the name of the DTD to be used for parsing the
  content of element named elementName.
  This element type appear with an 'X' in the ElemMappingTable.
  ----------------------------------------------------------------------*/
void SVGGetDTDName (char *DTDname, char *elementName)
{
  if (strcmp (elementName, "math") == 0)
    strcpy (DTDname, "MathML");
  else
    strcpy (DTDname, "");
}

/*----------------------------------------------------------------------
  MapSVGAttribute
  Search in the Attribute Mapping Table the entry for the
  attribute of name Attr and returns the corresponding Thot attribute type.
  ----------------------------------------------------------------------*/
void MapSVGAttribute (char *attrName, AttributeType *attrType,
                      char* elementName, ThotBool *level, Document doc)
{
  attrType->AttrSSchema = GetSVGSSchema (doc);
  MapXMLAttribute (SVG_TYPE, attrName, elementName, level, doc,
                   &(attrType->AttrTypeNum));
}

/*----------------------------------------------------------------------
  MapSVGAttributeValue
  Search in the Attribute Value Mapping Table the entry for the attribute
  ThotAtt and its value attVal. Returns the corresponding Thot value.
  ----------------------------------------------------------------------*/
void MapSVGAttributeValue (char* attVal, const AttributeType * attrType, int *value)
{
  MapXMLAttributeValue (SVG_TYPE, attVal, attrType, value);
}

/*----------------------------------------------------------------------
  MapSVGEntity
  Search that entity in the entity table and return the corresponding value.
  ----------------------------------------------------------------------*/
void MapSVGEntity (char *entityName, char *entityValue, char *script)
{
  entityValue[0] = EOS;
  *script = EOS;
}

/*----------------------------------------------------------------------
  SVGEntityCreated
  A SVG entity has been created by the XML parser.
  ----------------------------------------------------------------------*/
void SVGEntityCreated (unsigned char *entityValue, Language lang,
                       char *entityName, Document doc)
{
}

/*----------------------------------------------------------------------
  SetAttributeOnRoot
  Put attribute att on the <svg> element to which element el belongs.
  ----------------------------------------------------------------------*/
static void SetAttributeOnRoot (Element el, int att, Document doc)
{
  ElementType     elType, parentType;
  Element         root, parent, prev;
  AttributeType   attrType;
  Attribute       attr;

  /* find the root of the current SVG tree */
  root = NULL;
  elType = TtaGetElementType (el);
  parent = TtaGetParent (el);
  while (parent && !root)
    {
      parentType = TtaGetElementType (parent);
      prev = parent;
      parent = TtaGetParent (parent);
      if (parentType.ElSSchema == elType.ElSSchema &&
	  parentType.ElTypeNum == SVG_EL_SVG)
	/* this is an <svg> element */
	if (parent)
	  {
	    parentType = TtaGetElementType (parent);
	    if (parentType.ElTypeNum == SVG_EL_Document)
	      /* its parent is the root of the document */
	      root = prev;
	    else if (parentType.ElSSchema != elType.ElSSchema)
	      /* its parent is in a different namespace */
	      root = prev;
	  }
    }
  if (root)
    /* put the attribute on the root if it is not present yet */
    {
      attrType.AttrSSchema = elType.ElSSchema;
      attrType.AttrTypeNum = att;
      attr = TtaGetAttribute (root, attrType);
      if (!attr)
	{
	  attr = TtaNewAttribute (attrType);
	  TtaAttachAttribute (root, attr, doc);
	}
    } 
}

/*----------------------------------------------------------------------
  ParseFloatAttribute : 
  Parse the value of a float data attribute
  ----------------------------------------------------------------------*/
static float ParseFloatAttribute (Attribute attr)
{
  int                  length;
  char                *text, *ptr;
  PresentationValue    pval;

  length = TtaGetTextAttributeLength (attr) + 2;
  text = (char *)TtaGetMemory (length);
  if (text != NULL)
    {
      TtaGiveTextAttributeValue (attr, text, &length);
      /* parse the attribute value (just a number) */
      ptr = text;
      ptr = (char*)TtaSkipBlanks (ptr);
      ptr = ParseClampedUnit (ptr, &pval);
      TtaFreeMemory (text);
      return (float) pval.typed_data.value/1000;
    }
  return 0;
}

/*----------------------------------------------------------------------
  ParseCSSequivAttribute
  Create or update a specific presentation rule for element el that reflects
  the value of attribute attr, which is equivalent to a CSS property (fill,
  stroke, stroke-width, font-family, font-size, font-style, font-variant,
  font-weight, text-decoration, opacity, fill-opacity, stroke-opacity,
  fill-rule).
  ----------------------------------------------------------------------*/
void ParseCSSequivAttribute (int attrType, Attribute attr, Element el,
                             Document doc, ThotBool delete_)
{
  char               css_command[MAX_LENGTH + 20];
  int                length, val = 0;
  float              value;
  char              *text;

  text = NULL;
  /* get the value of the attribute */
  if (attrType == SVG_ATTR_font_style ||
      attrType == SVG_ATTR_font_variant ||
      attrType == SVG_ATTR_font_weight ||
      attrType == SVG_ATTR_fill_rule)
    /* enumerated value */
    val = TtaGetAttributeValue (attr);
  else
    /* the attribute value is a character string */
    {
      length = MAX_LENGTH - 1;
      text = (char *)TtaGetMemory (length + 1);
      TtaGiveTextAttributeValue (attr, text, &length);
      text[MAX_LENGTH - 1] = EOS;
    }

  /* builds the equivalent CSS rule */
  css_command[0] = EOS;
  switch (attrType)
    {
    case SVG_ATTR_color:
      sprintf (css_command, "color: %s", text);
      break;
    case SVG_ATTR_fill:
      sprintf (css_command, "fill: %s", text);
      break;
    case SVG_ATTR_stroke:
      sprintf (css_command, "stroke: %s", text);
      break;
    case SVG_ATTR_stroke_width:
      sprintf (css_command, "stroke-width: %s", text);
      break;
    case SVG_ATTR_font_family:
      sprintf (css_command, "font-family: %s", text);
      break;
    case SVG_ATTR_font_size:
      sprintf (css_command, "font-size: %s", text);
      break;
    case SVG_ATTR_font_style:
      switch (val)
        {
        case SVG_ATTR_font_style_VAL_normal_:
          sprintf (css_command, "font-style: normal");
          break;
        case SVG_ATTR_font_style_VAL_italic:
          sprintf (css_command, "font-style: italic");
          break;
        case SVG_ATTR_font_style_VAL_oblique_:
          sprintf (css_command, "font-style: oblique");
          break;
        case SVG_ATTR_font_style_VAL_inherit:
          sprintf (css_command, "font-style: inherit");
          break;
        }
      break;
    case SVG_ATTR_font_variant:
      switch (val)
        {
        case SVG_ATTR_font_variant_VAL_normal_:
          sprintf (css_command, "font-variant: normal");
          break;
        case SVG_ATTR_font_variant_VAL_small_caps:
          sprintf (css_command, "font-variant: small-caps");
          break;
        case SVG_ATTR_font_variant_VAL_inherit:
          sprintf (css_command, "font-variant: inherit");
          break;
        }
      break;
    case SVG_ATTR_font_weight:
      switch (val)
        {
        case SVG_ATTR_font_weight_VAL_normal_:
          sprintf (css_command, "font-weight: normal");
          break;
        case SVG_ATTR_font_weight_VAL_bold_:
          sprintf (css_command, "font-weight: bold");
          break;
        case SVG_ATTR_font_weight_VAL_bolder:
          sprintf (css_command, "font-weight: bolder");
          break;
        case SVG_ATTR_font_weight_VAL_lighter:
          sprintf (css_command, "font-weight: lighter");
          break;
        case SVG_ATTR_font_weight_VAL_w100:
          sprintf (css_command, "font-weight: 100");
          break;
        case SVG_ATTR_font_weight_VAL_w200:
          sprintf (css_command, "font-weight: 200");
          break;
        case SVG_ATTR_font_weight_VAL_w300:
          sprintf (css_command, "font-weight: 300");
          break;
        case SVG_ATTR_font_weight_VAL_w400:
          sprintf (css_command, "font-weight: 400");
          break;
        case SVG_ATTR_font_weight_VAL_w500:
          sprintf (css_command, "font-weight: 500");
          break;
        case SVG_ATTR_font_weight_VAL_w600:
          sprintf (css_command, "font-weight: 600");
          break;
        case SVG_ATTR_font_weight_VAL_w700:
          sprintf (css_command, "font-weight: 700");
          break;
        case SVG_ATTR_font_weight_VAL_w800:
          sprintf (css_command, "font-weight: 800");
          break;
        case SVG_ATTR_font_weight_VAL_w900:
          sprintf (css_command, "font-weight: 900");
          break;
        case SVG_ATTR_font_weight_VAL_inherit:
          sprintf (css_command, "font-weight: inherit");
          break;
        }
      break;
    case SVG_ATTR_text_decoration:
      sprintf (css_command, "text-decoration: %s", text);
      break;
    case SVG_ATTR_opacity_:
      value = ParseFloatAttribute (attr);
      if (value > 1.0)
        sprintf (css_command, "opacity: 1.0");
      else if (value < 0)
        sprintf (css_command, "opacity: 0.0");
      else
        sprintf (css_command, "opacity: %s", text);
      break;
    case SVG_ATTR_stroke_opacity:
      value = ParseFloatAttribute (attr);
      if (value > 1.0)
        sprintf (css_command, "stroke-opacity: 1.0");
      else if (value < 0)
        sprintf (css_command, "stroke-opacity: 0.0");
      else
        sprintf (css_command, "stroke-opacity: %s", text);
      break;
    case SVG_ATTR_fill_opacity:
      value = ParseFloatAttribute (attr);
      if (value > 1.0)
        sprintf (css_command, "fill-opacity: 1.0");
      else if (value < 0)
        sprintf (css_command, "fill-opacity: 0.0");
      else
        sprintf (css_command, "fill-opacity: %s", text);
      break;
    case SVG_ATTR_stop_opacity:
      value = ParseFloatAttribute (attr);
      if (value > 1.0)
        sprintf (css_command, "stop-opacity: 1.0");
      else if (value < 0)
        sprintf (css_command, "stop-opacity: 0.0");
      else
        sprintf (css_command, "stop-opacity: %s", text);
      break;
    case SVG_ATTR_fill_rule:
      switch (val)
        {
        case SVG_ATTR_fill_rule_VAL_nonzero:
          sprintf (css_command, "fill-rule: nonzero");
          break;
        case SVG_ATTR_fill_rule_VAL_evenodd:
          sprintf (css_command, "fill-rule: evenodd");
          break;
        case SVG_ATTR_fill_rule_VAL_inherit:
          sprintf (css_command, "fill-rule: inherit");
          break;
        }
      break;
    case SVG_ATTR_stop_color:
      sprintf (css_command, "stop-color: %s", text);
      break;
    case SVG_ATTR_marker_start:
      sprintf (css_command, "marker-start: %s", text);
      break;
    case SVG_ATTR_marker_mid:
      sprintf (css_command, "marker-mid: %s", text);
      break;
    case SVG_ATTR_marker_end:
      sprintf (css_command, "marker-end: %s", text);
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

/*----------------------------------------------------------------------
  CreateGraphicalLeaf
  Create a GRAPHICS_UNIT element as the last child of element el if it
  does not exist yet.
  Return that GRAPHICS_UNIT element.
  ----------------------------------------------------------------------*/
Element CreateGraphicalLeaf (char shape, Element el, Document doc)
{
  ElementType	   elType;
  Element	       leaf, child;
  char           oldShape;

  leaf = NULL;
  child = TtaGetLastChild (el);
  if (child != NULL)
    /* there is a child element */
    {
      elType = TtaGetElementType (child);
      if (elType.ElTypeNum == SVG_EL_GRAPHICS_UNIT)
        {
          oldShape = TtaGetGraphicsShape (child);
          leaf = child;
          if (oldShape == EOS || 
              oldShape != shape)
            TtaSetGraphicsShape (child, shape, doc);
        }
    }
  elType = TtaGetElementType (el);
  if (leaf == NULL)
    /* create the graphical element */
    {
      elType.ElTypeNum = SVG_EL_GRAPHICS_UNIT;
      leaf = TtaNewElement (doc, elType);
      if (child == NULL)
        TtaInsertFirstChild (&leaf, el, doc);
      else
        TtaInsertSibling (leaf, child, FALSE, doc);
      TtaSetGraphicsShape (leaf, shape, doc);
    }
  return leaf;
}

/*----------------------------------------------------------------------
  CreateGraphicLeaf
  Create a graphical leaf element as the last child of element el,
  according to the type of el.
  Returns the created (or existing) element.
  When returning, closed indicates whether the shape is closed or not.
  ----------------------------------------------------------------------*/
Element CreateGraphicLeaf (Element el, Document doc, ThotBool *closed)
{
  ElementType elType;
  Element     leaf;

  PRule               rule;
  int w,h;
  ThotBool             shape_recognition;

  if(!TtaGetEnvBoolean ("ENABLE_SHAPE_RECOGNITION", &shape_recognition))
    shape_recognition = TRUE;

  leaf = NULL;
  *closed = FALSE;
  elType = TtaGetElementType (el);
  switch (elType.ElTypeNum)
    {
    case SVG_EL_rect:
      if(shape_recognition)
        {
          rule = TtaGetPRule(el, PRWidth);
          w = TtaGetPRuleValue (rule);
          rule = TtaGetPRule(el, PRHeight);
          h = TtaGetPRuleValue (rule);
          
          if(w == h)
            leaf = CreateGraphicalLeaf (1, el, doc);
          else
            leaf = CreateGraphicalLeaf ('C', el, doc);
        }
      else
        leaf = CreateGraphicalLeaf ('C', el, doc);
      *closed = TRUE;
      break;

    case SVG_EL_circle_:
      leaf = CreateGraphicalLeaf ('a', el, doc);
      *closed = TRUE;
      break;

    case SVG_EL_ellipse:
      leaf = CreateGraphicalLeaf ('c', el, doc);
      *closed = TRUE;
      break;

    case SVG_EL_line_:
      leaf = CreateGraphicalLeaf ('g', el, doc);
      break;

    case SVG_EL_polyline:
      leaf = CreateGraphicalLeaf ('S', el, doc);
      break;

    case SVG_EL_polygon:
      leaf = CreateGraphicalLeaf ('p', el, doc);
      *closed = TRUE;
      break;

    case SVG_EL_path:
      leaf = CreateGraphicalLeaf (EOS, el, doc);
      *closed = FALSE;
      break;
    default:
      break;
    }
  return leaf;
}

/*----------------------------------------------------------------------
  CreateEnclosingElement
  Create an element of type elType, insert it in document doc at the
  position of element el and moves el and its following siblings within
  this new element.
  ----------------------------------------------------------------------*/
static void CreateEnclosingElement (Element el, ElementType elType, Document doc)
{
  Element	new_, prev, next, child;

  new_ = TtaNewElement (doc, elType);
  TtaInsertSibling (new_, el, TRUE, doc);
  next = el;
  TtaNextSibling (&next);
  TtaRemoveTree (el, doc);
  TtaInsertFirstChild (&el, new_, doc);
  prev = el;
  while (next != NULL)
    {
      child = next;
      TtaNextSibling (&next);
      TtaRemoveTree (child, doc);
      TtaInsertSibling (child, prev, FALSE, doc);
      prev = child;
    }
}

/*----------------------------------------------------------------------
  SetGraphicDepths forces a depth to each SVG child.
  -----------------------------------------------------------------------*/
void  SetGraphicDepths (Document doc, Element el)
{
#ifdef IV
  Element              child;
  ElementType          elType;
  SSchema	       graphSchema;
  PresentationValue    pval;
  PresentationContext  ctxt;

  /* look for the SVG root */
  graphSchema = GetSVGSSchema (doc);
  elType = TtaGetElementType (el);
  if (elType.ElTypeNum != SVG_EL_SVG || elType.ElSSchema != graphSchema)
    {
      elType.ElTypeNum = SVG_EL_SVG;
      elType.ElSSchema = graphSchema;
      el = TtaGetTypedAncestor (el, elType);
    }
  /* set depth to all SVG elements */
  ctxt = TtaGetSpecificStyleContext (doc);
  /* the specific presentation is not a CSS rule */
  ctxt->cssSpecificity = 2000;
  ctxt->destroy = FALSE;
  pval.typed_data.value = 0; /*TtaGetDepth (el, doc, 1)*/
  pval.typed_data.unit = UNIT_REL;
  pval.typed_data.real = FALSE;
  child = TtaGetLastChild (el);
  while (child)
    {
      TtaSetStylePresentation (PRDepth, child, NULL, ctxt, pval);
      TtaPreviousSibling (&child);
      pval.typed_data.value += 5;
    }
  TtaFreeMemory (ctxt);
#endif
}

/*----------------------------------------------------------------------
  CopyTRefContent
  Copy the text leaves of the subtree pointed by source as children
  of element el, which is of type tref.
  ----------------------------------------------------------------------*/
void CopyTRefContent (Element source, Element el, Document doc)
{
  Element        text, copy, child;
  ElementType    elType, commentType;

  elType = TtaGetElementType (el);
  elType.ElTypeNum = SVG_EL_TEXT_UNIT;
  commentType.ElSSchema = elType.ElSSchema;
  commentType.ElTypeNum = SVG_EL_XMLcomment;
  text = source;
  do
    {
      text = TtaSearchTypedElementInTree (elType, SearchForward, source, text);
      /* ignore text elements that are within comments */
      if (text && !TtaGetTypedAncestor (text, commentType))
        {
          copy = TtaNewTranscludedElement (doc, text);
          /* insert that leaf as the last child of the tref element */
          child = TtaGetLastChild (el);
          if (child)
            TtaInsertSibling (copy, child, FALSE, doc);
          else
            TtaInsertFirstChild (&copy, el, doc);
        }
    }
  while (text);
}

/*----------------------------------------------------------------------
  CopyUseContent
  Copy the subtree pointed by the href URI as a subtree of element el,
  which is of type use, tref, linearGradient or radialGradient.
  Return TRUE if successful.
  ----------------------------------------------------------------------*/
ThotBool CopyUseContent (Element el, Document doc, char *href)
{
  Element              source, curEl, copy, prevCopy, child, nextChild, elFound;
  ElementType          elType, childType;
  Attribute            attr;
  AttributeType        attrType;
  SearchDomain         direction;
  int                  i, length, attrKind;
  float                offset;
  char *               id;
  ThotBool             isUse, isTref, oldStructureChecking;

  /* look for an element with an id attribute with the same value as the
     href attribute */
  elType = TtaGetElementType (el);
  isUse = (elType.ElTypeNum == SVG_EL_use_);
  isTref = (elType.ElTypeNum == SVG_EL_tref);
  attrType.AttrSSchema = elType.ElSSchema;
  attrType.AttrTypeNum = SVG_ATTR_id;
  /* search backwards first */
  direction = SearchBackward;
  source = NULL;
  if (href[0] == '#') /* handles only internal links */
    for (i = 1; i <= 2 && source == NULL; i++)
      {
        curEl = el;
        do
          {
            TtaSearchAttribute (attrType, direction, curEl, &elFound, &attr);
            if (attr)
              /* an id attribute has been found */
              {
                /* get its value */
                length = TtaGetTextAttributeLength (attr);
                id = (char *)TtaGetMemory (length + 1);
                TtaGiveTextAttributeValue (attr, id, &length);
                /* compare with the xlink:href attribute of the use element */
                if (!strcasecmp (&href[1], id))
                  /* same  values. we found it */
                  source = elFound;
                TtaFreeMemory (id);
              }
            curEl = elFound;
          }
        while (elFound && !source);
        /* search forward if not found */
        direction = SearchForward;
      }
  if (!source)
    return FALSE;
  else
    /* the element to be copied in the element has been found */
    {
      /* remove the old copy if there is one */
      child = TtaGetFirstChild (el);
      while (child)
        {
          nextChild = child;
          TtaNextSibling (&nextChild);
          if (TtaIsTranscludedElement (child))
            /* that's an old copy. remove it */
            TtaRemoveTree (child, doc);
          child = nextChild;
        }

      oldStructureChecking = TtaGetStructureChecking (doc);
      if (oldStructureChecking)
        TtaSetStructureChecking (FALSE, doc);

      if (isUse)
        /* it's a use element. Copy the source element itself */
        {
          /* make a copy of the source element */
          copy = TtaNewTranscludedElement (doc, source);
          /* remove the id attribute from the copy */
          attr = TtaGetAttribute (copy, attrType);
          if (attr)
            TtaRemoveAttribute (copy, attr, doc);
          /* insert the copy as the last child of the use element */
          child = TtaGetLastChild (el);
          if (child)
            TtaInsertSibling (copy, child, FALSE, doc);
          else
            TtaInsertFirstChild (&copy, el, doc);
	  /* if the use element refers to a gradient, associate the gradient
	     with the new child of the use element */
	  TtaCopyGradientUse (copy);
        }
      else if (isTref)
        /* it's a tref, linearGradient or radialGradient element.
	   Copy all the contents of the source element */
        CopyTRefContent (source, el, doc);
      else if (elType.ElTypeNum ==SVG_EL_linearGradient ||
	       elType.ElTypeNum ==SVG_EL_radialGradient)
	/* it's a gradient. Copy the gradient stops and update attributes */
	{
	  /* copy the gradient stops */
	  child = TtaGetFirstChild (source);
	  prevCopy = NULL;
	  while (child)
	    {
	      childType = TtaGetElementType (child);
	      if (childType.ElTypeNum == SVG_EL_stop &&
		  childType.ElSSchema == elType.ElSSchema)
		{
		  copy = TtaCopyTree (child, doc, doc, el);
		  if (!prevCopy)
		    TtaInsertFirstChild (&copy, el, doc);
		  else
		    TtaInsertSibling (copy, prevCopy, FALSE, doc);
		  TtaNewGradientStop (copy, el);
		  /* Put attribute IsCopy to indicate that this copy must not
		     be saved with the document */
		  attrType.AttrSSchema = elType.ElSSchema;
		  attrType.AttrTypeNum = SVG_ATTR_IsCopy;
		  attr = TtaNewAttribute (attrType);
		  TtaAttachAttribute (copy, attr, doc);
		  /* copy attribute offset */
		  attrType.AttrTypeNum = SVG_ATTR_offset;
		  attr = TtaGetAttribute (copy, attrType);
		  if (attr)
		    {
		      offset = ParseNumberPercentAttribute (attr);
		      TtaSetGradientStopOffset (offset, copy);
		    }
		  prevCopy = copy;
		}
	      TtaNextSibling (&child);
	    }
	  /* update attributes */
	  attr = NULL;
	  TtaNextAttribute (el, &attr);
	  while (attr)
	    {
	      TtaGiveAttributeType (attr, &attrType, &attrKind);
	      if (attrType.AttrSSchema == elType.ElSSchema)
		switch (attrType.AttrTypeNum)
		  {
		  case SVG_ATTR_x1:
		    TtaSetLinearGradientx1 (ParseNumberPercentAttribute (attr), el);
		    break;
		  case SVG_ATTR_y1:
		    TtaSetLinearGradienty1 (ParseNumberPercentAttribute (attr), el);
		    break;
		  case SVG_ATTR_x2:
		    TtaSetLinearGradientx2 (ParseNumberPercentAttribute (attr), el);
		    break;
		  case SVG_ATTR_y2:
		    TtaSetLinearGradienty2 (ParseNumberPercentAttribute (attr), el);
		    break;
		  case SVG_ATTR_spreadMethod:
		    TtaSetGradientSpreadMethod (TtaGetAttributeValue (attr), el);
		    break;
		  case SVG_ATTR_gradientUnits:
		    TtaSetGradientUnits ((TtaGetAttributeValue (attr) == SVG_ATTR_gradientUnits_VAL_userSpaceOnUse), el);
		    break;
		  case SVG_ATTR_gradientTransform:
		    ParseTransformAttribute (attr, el, doc, FALSE, TRUE);
		    break;
		  case SVG_ATTR_r:
		    TtaSetRadialGradientRadius (ParseNumberPercentAttribute (attr), el);
		    break;
		  case SVG_ATTR_cx:
		    TtaSetRadialGradientcx (ParseNumberPercentAttribute (attr), el);
		    break;
		  case SVG_ATTR_cy:
		    TtaSetRadialGradientcy (ParseNumberPercentAttribute (attr), el);
		    break;
		  case SVG_ATTR_fx:
		    TtaSetRadialGradientfx (ParseNumberPercentAttribute (attr), el);
		    break;
		  case SVG_ATTR_fy:
		    TtaSetRadialGradientfy (ParseNumberPercentAttribute (attr), el);
		    break;
		  }
	      TtaNextAttribute (el, &attr);
	    }
	}

      if (oldStructureChecking)
        TtaSetStructureChecking (oldStructureChecking, doc);
    }
  return TRUE;
}

/*----------------------------------------------------------------------
  SkipBlanksAndComma skips all spaces, tabs, linefeeds, newlines and
  commas at the beginning of the string and returns the pointer to the
  new position. 
  ----------------------------------------------------------------------*/
static char *SkipBlanksAndComma (char *ptr)
{
  while (*ptr == SPACE || *ptr == BSPACE || *ptr == EOL ||
         *ptr == TAB || *ptr == __CR__ || *ptr == ',')
    ptr++;
  return (ptr);
}

/*----------------------------------------------------------------------
  GetFloat
  Parse an integer or floating point number and skip to the next token.
  Return the value of that number in number and moves ptr to the next
  token to be parsed.
  ----------------------------------------------------------------------*/
static char *GetFloat (char *ptr, float* number)
{
  int      i;
  char     *start, c;
  float     val = 0.;
  ThotBool negative, decimal, exponent, useDotForFloat;

  /* test if the system uses dot or comma in the float syntax */
  sscanf (".5", "%f", &val);
  useDotForFloat = (val == 0.5);
  negative = FALSE;
  decimal = FALSE;
  exponent = FALSE;
  *number = 0.;
  /* read the sign */
  if (*ptr == '+')
    ptr++;
  else if (*ptr == '-')
    {
      ptr++;
      negative = TRUE;
    }

  start = ptr;
  /* read the integer part */
  while (*ptr != EOS && *ptr >= '0' && *ptr <= '9')
    ptr++;
  if (*ptr == '.')
    /* there is a decimal part */
    {
      if (!useDotForFloat)
        *ptr = ',';
      ptr++;
      decimal = TRUE;
      while (*ptr != EOS &&  *ptr >= '0' && *ptr <= '9')
        ptr++;
    }

  if (*ptr == 'e' || *ptr == 'E')
    /* there is an exponent, parse it */
    {
      exponent = TRUE;
      ptr++;
      /* read the sign of the exponent */
      if (*ptr == '+')
        ptr++;
      else if (*ptr == '-')
        ptr++;
      while (*ptr != EOS &&  *ptr >= '0' && *ptr <= '9')
        ptr++;
    }
  /* remove possible extra characters */
  c = *ptr;
	if (c != EOS)
    *ptr = EOS;
  if (exponent)
    sscanf (start, "%e", number);
  else if (decimal)
    sscanf (start, "%f", number);
  else
    {
      sscanf (start, "%d", &i);
      *number = (float)i;
    }

  if (negative)
    *number = - *number;
  /* restore extra characters */
  *ptr = c;

  /* skip the following spaces */
  while (*ptr != EOS &&
         (*ptr == ',' || *ptr == SPACE || *ptr == BSPACE ||
          *ptr == EOL    || *ptr == TAB   || *ptr == __CR__))
    ptr++;
  return (ptr);
}

/*----------------------------------------------------------------------
  ParseviewBoxAttribute
  Parse the value of a viewbox attribute
  ----------------------------------------------------------------------*/
static void ParseviewBoxAttribute (Attribute attr, Element el, Document doc,
                                   float* x, float* y, float* width,
                                   float* height, ThotBool delete_)
{
  int                  length;
  char                *text, *ptr;

  *x = 0;
  *y = 0;
  *width = 0;
  *height = 0;
  length = TtaGetTextAttributeLength (attr) + 2;
  text = (char *)TtaGetMemory (length);
  if (text)
    {
      /* get the value of the attribute */
      TtaGiveTextAttributeValue (attr, text, &length);
      /* parse the attribute value */
      ptr = text;
      if (*ptr != EOS)
        {
          /* skip space characters */
          ptr = SkipBlanksAndComma (ptr);
          ptr = GetFloat (ptr, x);
          ptr = SkipBlanksAndComma (ptr);
          ptr = GetFloat (ptr, y);
          ptr = SkipBlanksAndComma (ptr);
          ptr = GetFloat (ptr, width);
          ptr = SkipBlanksAndComma (ptr);
          ptr = GetFloat (ptr, height);
        }       
      TtaFreeMemory (text);
    }
}

/*----------------------------------------------------------------------
  CopyAMarker
  creates a copy of element marker for the given vertex of element leaf and
  inserts it in document doc as a child of element el (a <path>, a <line>,
  a <polyline> or a <polygon>) right after element leaf.
  ----------------------------------------------------------------------*/
void CopyAMarker (Element marker, Element el, Element leaf, int vertex,
		  Document doc)
{
  ElementType          elType;
  Element              copy, child, ascend;
  AttributeType        attrType;
  Attribute            attr;
  char                 *val, buffer[50];
  int                  x, y, length, w, h, i;
  float                scaleX, scaleY, vBX, vBY, vBWidth, vBHeight;
  double               angle;
  PresentationContext  context = NULL;
  PresentationValue    presValue;
  ThotBool             oldStructureChecking, strokeWidth;

  elType = TtaGetElementType (el);
  oldStructureChecking = TtaGetStructureChecking (doc);
  if (oldStructureChecking)
    TtaSetStructureChecking (FALSE, doc);

  /* make a copy of the marker element */
  copy = TtaNewTranscludedElement (doc, marker);
  /* mark the new Coordinate System */
  TtaSetElCoordinateSystem (copy);
  /* remove the id attribute from the copy */
  attrType.AttrSSchema = elType.ElSSchema;
  attrType.AttrTypeNum = SVG_ATTR_id;
  attr = TtaGetAttribute (copy, attrType);
  if (attr)
    TtaRemoveAttribute (copy, attr, doc);

  /* insert the copy in the tree as the last child of the host element */
  child = TtaGetLastChild (el);
  if (child)
    TtaInsertSibling (copy, child, FALSE, doc);
  else
    TtaInsertFirstChild (&copy, el, doc);

  /* add a transform (translate) to the copied marker to place it on the
     relevant vertex of the host element */
  if (elType.ElTypeNum == SVG_EL_polyline ||
      elType.ElTypeNum == SVG_EL_polygon ||
      elType.ElTypeNum == SVG_EL_line_)
    TtaGivePolylinePoint (leaf, vertex, UnPixel, &x, &y);
  else if (elType.ElTypeNum == SVG_EL_path)
    TtaGivePathPoint (leaf, vertex, UnPixel, &x, &y);
  else
    { x = 0;   y = 0; }
  if (x != 0 || y != 0)
    TtaAppendTransform (copy, TtaNewTransformTranslate (x, y));

  /* add a rotation corresponding to the orient attribute of the marker */
  angle = 0;
  attrType.AttrTypeNum = SVG_ATTR_orient;
  attr = TtaGetAttribute (marker, attrType);
  if (attr)
    /* there is an orient attribute. Get its value */
    {
      length = TtaGetTextAttributeLength (attr);
      val = (char *)TtaGetMemory (length + 1);
      TtaGiveTextAttributeValue (attr, val, &length);
      if (!strcmp (val, "auto"))
        /* angle="auto". Compute the angle for the relevant vertex */
        {
          if (elType.ElTypeNum == SVG_EL_polyline ||
              elType.ElTypeNum == SVG_EL_polygon ||
              elType.ElTypeNum == SVG_EL_line_)
            TtaGivePolylineAngle (leaf, vertex, &angle);
          else if (elType.ElTypeNum == SVG_EL_path)
            TtaGivePathAngle (leaf, vertex, &angle);
        }
      else
        /* not "auto". Parse the value of the angle */
        {
          /* @@@@ to be written @@@@ */
        }
      TtaFreeMemory (val);
    }
  if (angle != 0)
    TtaAppendTransform (copy, TtaNewTransformRotate (angle, 0, 0));

  /* add a scaling to match the coordinate system indicated by the
     'markerUnits' attribute */
  scaleX = 1;
  strokeWidth = TRUE;  /* default value */
  attrType.AttrTypeNum = SVG_ATTR_markerUnits;
  attr = TtaGetAttribute (marker, attrType);
  if (attr)
    /* there is a 'markerUnits' attribute. Get its value */
    {
      if (TtaGetAttributeValue(attr) == SVG_ATTR_markerUnits_VAL_userSpaceOnUse)
        strokeWidth = FALSE;
    }
  if (strokeWidth)
    {
      strokeWidth = FALSE;
      ascend = el;
      do
	{
	  if (TtaGetStylePresentation (PRLineWeight, ascend, NULL, context,
				       &presValue) == 0)
	    {
	      strokeWidth = TRUE;
	      if (presValue.typed_data.real)
		scaleX = presValue.typed_data.value / 1000;
	      else
		scaleX = presValue.typed_data.value;
	      if (presValue.typed_data.unit == UNIT_REL)
		scaleX = scaleX / 10;
	    }
	  else
	    ascend = TtaGetParent (ascend);
	}
      while (!strokeWidth && ascend);
    }
  scaleY = scaleX;
  if (scaleX != 1 || scaleY != 1)
    TtaAppendTransform (copy, TtaNewTransformScale (scaleX, scaleY));

  /* add a translate to put the reference point of the marker on the vertex of
     the host element, using the 'refX' and 'refY' attributes of the marker */
  attrType.AttrTypeNum = SVG_ATTR_markerWidth;
  attr = TtaGetAttribute (marker, attrType);
  if (attr)
    {
      i = 50;
      TtaGiveTextAttributeValue (attr, buffer, &i);
      sscanf (buffer, "%d", &w);
    }
  else
    w = 3;
  attrType.AttrTypeNum = SVG_ATTR_markerHeight;
  attr = TtaGetAttribute (marker, attrType);
  if (attr)
    {
      i = 50;
      TtaGiveTextAttributeValue (attr, buffer, &i);
      sscanf (buffer, "%d", &h);
    }
  else
    h = 3;
  /* get the viewBox attribute and its values */
  attrType.AttrTypeNum = SVG_ATTR_viewBox;
  attr = TtaGetAttribute (marker, attrType);
  if (!attr)
    {
      scaleX = 1;
      scaleY = 1;
    }
  else
    {
      ParseviewBoxAttribute (attr, marker, doc, &vBX, &vBY, &vBWidth, &vBHeight,
			     FALSE);
      /* compute scaling with preserveAspectRatio = xMidYMid meet */
      scaleX = w / vBWidth;
      scaleY = h / vBHeight;
      if (scaleX < scaleY)
	scaleY = scaleX;
      else
	scaleX = scaleY;
    }

  attrType.AttrTypeNum = SVG_ATTR_refX;
  attr = TtaGetAttribute (marker, attrType);
  if (attr)
    {
      i = 50;
      TtaGiveTextAttributeValue (attr, buffer, &i);
      sscanf (buffer, "%d", &x);
    }
  else
    x = 0;
  attrType.AttrTypeNum = SVG_ATTR_refY;
  attr = TtaGetAttribute (marker, attrType);
  if (attr)
    {
      i = 50;
      TtaGiveTextAttributeValue (attr, buffer, &i);
      sscanf (buffer, "%d", &y);
    }
  else
    y = 0;
  if (x != 0 || y != 0)
    TtaAppendTransform (copy, TtaNewTransformTranslate (-x*scaleX, -y*scaleY));
  /* Scale the coordinate system to set the coordinate system to viewBox units*/
  if (scaleX != 1 || scaleY != 1)
    TtaAppendTransform (copy, TtaNewTransformScale (scaleX, scaleY));

  if (oldStructureChecking)
    TtaSetStructureChecking (oldStructureChecking, doc);
}

/*----------------------------------------------------------------------
  GenerateMarkers
  Apply a CSS rule marker* to element el in document doc.
  Parameter marker is the marker element to be used.
  Parameter position indicates where the marker has to be put on element pEl:
    0: all vertices
    1: start vertex
    2: mid vertices
    3: end vertex
  This function is called when applying a marker* CSS rule (see module
  presrules.c)
  ----------------------------------------------------------------------*/
void GenerateMarkers (Element el, Document doc, Element marker, int position)
{
  Element         leaf;
  ElementType     elType;
  int             length, i;

  if (!el || !marker || doc == 0)
    /* invalid parameter */
    return;
  /* get the child of element pEl that is a graphic leaf */
  leaf = TtaGetFirstChild (el);
  while (leaf && TtaGetElementType(leaf).ElTypeNum != GRAPHICS_UNIT)
    TtaNextSibling (&leaf);
  if (!leaf)
    return;
  /* get the number of vertices in the graphic leaf */
  elType = TtaGetElementType (el);
  if (elType.ElTypeNum == SVG_EL_polyline ||
      elType.ElTypeNum == SVG_EL_polygon)
    length = TtaGetPolylineLength (leaf) - 1;
  else if (elType.ElTypeNum == SVG_EL_path)
    length = TtaNumberOfPointsInPath (leaf) - 1;
  else
    length = 1;
  /* copy the marker element(s) after the graphic leaf */
  if (position == 1)
    /* marker-start */
    CopyAMarker (marker, el, leaf, 1, doc);
  else if (position == 3)
    /* marker-end */
    CopyAMarker (marker, el, leaf, length+1, doc);
  else if (position == 2)
    /* marker-mid */
    {
      for (i = 2; i <= length; i++)
	CopyAMarker (marker, el, leaf, i, doc);
    }
  else if (position == 0)
    /* marker */
    {
      for (i = 1; i <= length+1; i++)
	CopyAMarker (marker, el, leaf, i, doc);
    }
}

/*----------------------------------------------------------------------
  CheckHrefAttr
  If element el has a href attribute from the XLink namespace, replace
  that attribute by a href attribute in the SVG namespace with the
  same value.
  ----------------------------------------------------------------------*/
static Attribute CheckHrefAttr (Element el, Document doc)
{
  Element              root;
  SSchema              XLinkSSchema;
  AttributeType        attrType;
  Attribute            attr;
  int                  length;
  char                 *href;

  attr = NULL;
  XLinkSSchema = GetXLinkSSchema (doc);
  if (XLinkSSchema)
    /* the XLink namespace is used in that document */
    {
      root = TtaGetRootElement (doc);
      TtaSetANamespaceDeclaration (doc, root, "xlink", XLink_URI);
      attrType.AttrSSchema = XLinkSSchema;
      attrType.AttrTypeNum = XLink_ATTR_href_;
      attr = TtaGetAttribute (el, attrType);
      if (attr)
        /* it has an XLink href attribute */
        {
          /* get its value */
          length = TtaGetTextAttributeLength (attr);
          href = (char *)TtaGetMemory (length + 1);
          TtaGiveTextAttributeValue (attr, href, &length);
          /* delete the XLink href attribute */
          TtaRemoveAttribute (el, attr, doc);
          /* create a new href attribute in the SVG namespace */
          attrType.AttrSSchema = TtaGetElementType(el).ElSSchema;
          attrType.AttrTypeNum = SVG_ATTR_xlink_href;
          attr = TtaNewAttribute (attrType);
          TtaAttachAttribute (el, attr, doc);
          /* copy the value */
          TtaSetAttributeText (attr, href, el, doc);
          TtaFreeMemory (href);
        }
    }
  return attr;
}

/*----------------------------------------------------------------------
  NextWhiteSpace
  ----------------------------------------------------------------------*/
static void NextWhiteSpace(char **ptr)
{
  while (**ptr != EOS && **ptr != SPACE && **ptr != BSPACE &&
	 **ptr != EOL && **ptr != TAB && **ptr != __CR__)
    (*ptr)++;
}

/*----------------------------------------------------------------------
  IsSupportedFeature, IsSupportedExtension
  ----------------------------------------------------------------------*/
#define CHECK(str) if (!strcmp (ptr, str)) return TRUE

static ThotBool IsSupportedFeature(char *ptr)
{
  /* SVG 1.0 feature strings (deprecated) */
  CHECK("org.w3c.svg");
  CHECK("org.w3c.svg.static");

  /* SVG 1.1 feature strings */
  CHECK("http://www.w3.org/TR/SVG11/feature#SVG");
  //CHECK("http://www.w3.org/TR/SVG11/feature#SVGDOM");
  //CHECK("http://www.w3.org/TR/SVG11/feature#SVG-static");
  //CHECK("http://www.w3.org/TR/SVG11/feature#SVGDOM-static")
  //CHECK("http://www.w3.org/TR/SVG11/feature#SVG-animation")
  //CHECK("http://www.w3.org/TR/SVG11/feature#SVGDOM-animation")
  //CHECK("http://www.w3.org/TR/SVG11/feature#SVG-dynamic");
  //CHECK("http://www.w3.org/TR/SVG11/feature#SVGDOM-dynamic");

  CHECK("http://www.w3.org/TR/SVG11/feature#CoreAttribute");
  CHECK("http://www.w3.org/TR/SVG11/feature#Structure");
  CHECK("http://www.w3.org/TR/SVG11/feature#BasicStructure");
  CHECK("http://www.w3.org/TR/SVG11/feature#ContainerAttribute");
  CHECK("http://www.w3.org/TR/SVG11/feature#ConditionalProcessing");
  //CHECK("http://www.w3.org/TR/SVG11/feature#Image");
  CHECK("http://www.w3.org/TR/SVG11/feature#Style");
  //CHECK("http://www.w3.org/TR/SVG11/feature#ViewportAttribute");
  CHECK("http://www.w3.org/TR/SVG11/feature#Shape");
  //CHECK("http://www.w3.org/TR/SVG11/feature#Text");
  CHECK("http://www.w3.org/TR/SVG11/feature#BasicText");
  //CHECK("http://www.w3.org/TR/SVG11/feature#PaintAttribute");
  //CHECK("http://www.w3.org/TR/SVG11/feature#BasicPaintAttribute");
  CHECK("http://www.w3.org/TR/SVG11/feature#OpacityAttribute");
  //CHECK("http://www.w3.org/TR/SVG11/feature#GraphicsAttribute");
  CHECK("http://www.w3.org/TR/SVG11/feature#BasicGraphicsAttribute");
  //CHECK("http://www.w3.org/TR/SVG11/feature#Marker");
  //CHECK("http://www.w3.org/TR/SVG11/feature#ColorProfile");
  //CHECK("http://www.w3.org/TR/SVG11/feature#Gradient");
  //CHECK("http://www.w3.org/TR/SVG11/feature#Pattern");
  //CHECK("http://www.w3.org/TR/SVG11/feature#Clip");
  //CHECK("http://www.w3.org/TR/SVG11/feature#BasicClip");
  //CHECK("http://www.w3.org/TR/SVG11/feature#Mask");
  //CHECK("http://www.w3.org/TR/SVG11/feature#Filter");
  //CHECK("http://www.w3.org/TR/SVG11/feature#BasicFilter");
  //CHECK("http://www.w3.org/TR/SVG11/feature#DocumentEventsAttribute");
  //CHECK("http://www.w3.org/TR/SVG11/feature#GraphicalEventsAttribute");
  //CHECK("http://www.w3.org/TR/SVG11/feature#AnimationEventsAttribute");
  //CHECK("http://www.w3.org/TR/SVG11/feature#Cursor");
  CHECK("http://www.w3.org/TR/SVG11/feature#Hyperlinking");
  CHECK("http://www.w3.org/TR/SVG11/feature#XlinkAttribute");
  //CHECK("http://www.w3.org/TR/SVG11/feature#ExternalResourcesRequired");
  //CHECK("http://www.w3.org/TR/SVG11/feature#View");
  //CHECK("http://www.w3.org/TR/SVG11/feature#Script");
  CHECK("http://www.w3.org/TR/SVG11/feature#Animation");
  //CHECK("http://www.w3.org/TR/SVG11/feature#Font");
  //CHECK("http://www.w3.org/TR/SVG11/feature#BasicFont");
  CHECK("http://www.w3.org/TR/SVG11/feature#Extensibility");

  return FALSE;
}

static ThotBool IsSupportedExtension(char *ptr)
{
  printf("%s\n", ptr);

  /* only XHTML and MathML are considered as supported extensions*/
  CHECK(XHTML_URI);
  CHECK(MathML_URI);
  return FALSE;
}

#undef CHECK

/*----------------------------------------------------------------------
  EvaluateFeatures
  Evaluates the requiredFeatures attribute
  ----------------------------------------------------------------------*/
static ThotBool EvaluateFeatures (Attribute attr)
{
  int          length;
  char         *text, *ptr, *ptr2;
  ThotBool     supported;

  supported = FALSE;
  length = TtaGetTextAttributeLength (attr);
  if (length > 0)
    {
      text = (char *)TtaGetMemory (length + 2);
      if (text)
        {
	  supported = TRUE;
          TtaGiveTextAttributeValue (attr, text, &length);
          ptr = text;
	  ptr = (char*)TtaSkipBlanks (ptr);
          while (*ptr != EOS && supported)
            {
	      /* Move ptr2 to the next white space */
	      ptr2 = ptr;
	      NextWhiteSpace(&ptr2);

	      if(*ptr2 == EOS)
		{
		  /* It's the end of the string */
		  supported = IsSupportedFeature(ptr);
		  break;
		}

	      /* Check if the feature is supported */
	      *ptr2 = EOS; supported = IsSupportedFeature(ptr);

	      /* Move to the next feature */
	      *ptr2 = ' '; ptr = (char*)TtaSkipBlanks (ptr2);
            }
          TtaFreeMemory (text);
        }
    }
  return supported;
}

/*----------------------------------------------------------------------
  EvaluateExtensions
  Evaluates the requiredExtensions attribute
  ----------------------------------------------------------------------*/
static ThotBool EvaluateExtensions (Attribute attr)
{
  int          length;
  char         *text, *ptr, *ptr2;
  ThotBool     supported;

  supported = FALSE;
  length = TtaGetTextAttributeLength (attr);
  if (length > 0)
    {
      text = (char *)TtaGetMemory (length + 2);
      if (text)
        {
	  supported = TRUE;
          TtaGiveTextAttributeValue (attr, text, &length);
          ptr = text;
	  ptr = (char*)TtaSkipBlanks (ptr);
          while (*ptr != EOS && supported)
            {
	      /* Move ptr2 to the next white space */
	      ptr2 = ptr;
	      NextWhiteSpace(&ptr2);

	      if(*ptr2 == EOS)
		{
		  /* It's the end of the string */
		  supported = IsSupportedExtension(ptr);
		  break;
		}

	      /* Check if the extension is supported */
	      *ptr2 = EOS; supported = IsSupportedExtension(ptr);

	      /* Move to the next extension */
	      *ptr2 = ' '; ptr = (char*)TtaSkipBlanks (ptr2);
	    }
          TtaFreeMemory (text);
        }
    }
  return supported;
}

/*----------------------------------------------------------------------
  EvaluateSystemLanguage
  Evaluates the systemLanguage attribute
  ----------------------------------------------------------------------*/
static ThotBool EvaluateSystemLanguage (Attribute attr)
{
  int          length;
  char         *text, *ptr, *acceptLang, *pref;
  ThotBool     ok;

  ok = False;
  length = TtaGetTextAttributeLength (attr);
  if (length > 0)
    {
      text = (char *)TtaGetMemory (length + 2);
      if (text)
        {
          /* get the list of languages accepted by the user */
          acceptLang = TtaGetEnvString ("ACCEPT_LANGUAGES");
          if (!acceptLang)
            return (ok);
          acceptLang = (char*)TtaSkipBlanks (acceptLang);
          if (*acceptLang != EOS)
            /* the list of user's preferred languages is not empty */
            {
              TtaGiveTextAttributeValue (attr, text, &length);
              ptr = text;
              ptr = (char*)TtaSkipBlanks (ptr);
              while (*ptr != EOS && !ok)
                {
                  pref = acceptLang;
                  while (*pref != EOS && !ok)
                    {
                      if (strncmp (ptr, pref, 2) == 0)
                        ok = True;
                      else
                        {
                          while (*pref != EOS && *pref != ',' &&
                                 *pref != SPACE && *pref != BSPACE &&
                                 *pref != EOL && *pref != TAB && *pref != __CR__)
                            pref++;
                          if (*pref == ',')
                            pref++;
                          pref = (char*)TtaSkipBlanks (pref);
                        }
                    }
                  if (!ok)
                    {
                      while (*ptr != EOS && *ptr != ',' && *ptr != SPACE &&
                             *ptr != BSPACE && *ptr != EOL && *ptr != TAB &&
                             *ptr != __CR__)
                        ptr++;
                      if (*ptr == ',')
                        ptr++;
                      ptr = (char*)TtaSkipBlanks (ptr);
                    }
                }
            }
          TtaFreeMemory (text);
        }
    }
  return ok;
}

/*----------------------------------------------------------------------
  EvaluateTestAttrs
  Evaluates the requiredFeatures, requiredExtensions and systemLanguage
  attributes on the direct child elements of element el and selects
  the child to be rendered
  ----------------------------------------------------------------------*/
void EvaluateTestAttrs (Element el, Document doc)
{
  ElementType          elType;
  Element              child, renderedChild;
  AttributeType        attrType;
  Attribute            attr;
  SSchema              SVGSSchema;
  PresentationValue    pval;
  PresentationContext  ctxt;
  ThotBool             ok;

  renderedChild = NULL;
  attrType.AttrSSchema = TtaGetElementType (el).ElSSchema;
  ctxt = TtaGetSpecificStyleContext (doc);
  ctxt->cssSpecificity = 2000;   /* the presentation rule to be set is not a CSS rule */
  pval.typed_data.unit = UNIT_PX;
  pval.typed_data.value = 0;
  pval.typed_data.real = FALSE;
  SVGSSchema = TtaGetElementType(el).ElSSchema;
  /* process all children in order */
  child = TtaGetFirstChild (el);
  while (child)
    {
      /* if this child is a comment or a processing instruction, skip it */
      elType = TtaGetElementType (child);
      if (elType.ElSSchema == SVGSSchema &&
          elType.ElTypeNum != SVG_EL_XMLcomment &&
          elType.ElTypeNum != SVG_EL_XMLPI &&
          elType.ElTypeNum != SVG_EL_TEXT_UNIT &&
          elType.ElTypeNum != SVG_EL_Unknown_namespace)
        {
          ctxt->destroy = FALSE; /* we will most probably create a PRule
                                    Visibility: 0; for this child */
          if (!renderedChild)
            /* we have not encountered yet a child that can be rendered */
            {
              ok = True;
              attrType.AttrTypeNum = SVG_ATTR_requiredFeatures;
              attr = TtaGetAttribute (child, attrType);
              if (attr)
                ok = EvaluateFeatures (attr);
              if (ok)
                {
                  attrType.AttrTypeNum = SVG_ATTR_requiredExtensions;
                  attr = TtaGetAttribute (child, attrType);
                  if (attr)
                    ok = EvaluateExtensions (attr);
                  if (ok)
                    {
                      attrType.AttrTypeNum = SVG_ATTR_systemLanguage;
                      attr = TtaGetAttribute (child, attrType);
                      if (attr)
                        ok = EvaluateSystemLanguage (attr);
                    }
                }
              if (ok)
                /* all test attributes return True. Select that child by
                   removing its visibility rule if it got one */
                {
                  renderedChild = child;
                  ctxt->destroy = TRUE;
                }
            }
          /* set or remove a visibility PRule for this child */
          TtaSetStylePresentation (PRVisibility, child, NULL, ctxt, pval);
        }
      TtaNextSibling (&child);
    }
  TtaFreeMemory (ctxt);
}

/*----------------------------------------------------------------------
  SetTextAnchorTree
  Apply the value of a text-anchor attribute to a tree.
  ----------------------------------------------------------------------*/
static void SetTextAnchorTree (Element el, PresentationContext ctxt,
                               SSchema SvgSSchema, Attribute deletedAttr)
{
  ElementType         elType;
  Element             child;
  AttributeType       attrType;
  Attribute           attr;
  int                 val;
  PresentationValue   v;

  attrType.AttrSSchema = SvgSSchema;
  attrType.AttrTypeNum = SVG_ATTR_text_anchor;
  attr = TtaGetAttribute (el, attrType);
  if (attr && attr == deletedAttr)
    attr = NULL;
  if (attr)
    {
      val = TtaGetAttributeValue (attr);
      if (val == SVG_ATTR_text_anchor_VAL_inherit)
        /* that's text-anchor="inherit". Ignore it */
        attr = NULL;
    }
  if (!attr)
    /* this element does not have a text-anchor attribute. Continue */
    {
      elType = TtaGetElementType (el);
      if (elType.ElSSchema == SvgSSchema &&
          (elType.ElTypeNum == SVG_EL_text_ ||
           elType.ElTypeNum == SVG_EL_tspan ||
           elType.ElTypeNum == SVG_EL_tref  ||
           elType.ElTypeNum == SVG_EL_altGlyph ||
           elType.ElTypeNum == SVG_EL_textPath))
        /* this element is interested */
        {
          v.data = 0;
          v.typed_data.mainValue = TRUE;
          TtaSetStylePresentation (PRHorizPos, el, NULL, ctxt, v);
        }
      else
        /* look further for an interested element */
        {
          child = TtaGetFirstChild (el);
          while (child)
            {
              SetTextAnchorTree (child, ctxt, SvgSSchema, deletedAttr);
              TtaNextSibling (&child);
            }
        }
    }
}

/*----------------------------------------------------------------------
  SetTextAnchor
  Update (or create) the position rule of element el according to the
  value of the text-anchor attribute attr.
  ----------------------------------------------------------------------*/
void SetTextAnchor (Attribute attr, Element el, Document doc, ThotBool delete_)
{
  Attribute            attr1;
  AttributeType        attrType;
  Element              ancestor;
  int                  val, val1, attrKind;
  SSchema              SvgSSchema;
  ThotBool             found;
  PresentationContext  ctxt;

  TtaGiveAttributeType (attr, &attrType, &attrKind);
  SvgSSchema = attrType.AttrSSchema;
  val = TtaGetAttributeValue (attr);
  ctxt = TtaGetSpecificStyleContext (doc);
  if (delete_ || val == SVG_ATTR_text_anchor_VAL_inherit)
    /* attribute text-anchor is being deleted or inherited */
    {
      ctxt->type = PositionLeft;
      /* is there a text-anchor attribute on an ancestor? */
      ancestor = TtaGetParent (el);
      found = FALSE;
      while (ancestor && !found)
        {
          attr1 = TtaGetAttribute (ancestor, attrType);
          if (attr1)
            /* this ancestor has an attribute text-anchor */
            {
              val1 = TtaGetAttributeValue (attr1);
              if (val1 != SVG_ATTR_text_anchor_VAL_inherit)
                /* ignore it if it's "inherit" */
                {
                  if (val1 == val)
                    /* the inherited value is the same as the value of the
                       deleted attribute. done. */
                    el = NULL;
                  else
                    /* the inherited value has to be applied to element el
                       and all its descendants */
                    val = val1;
                  found = TRUE; /* don't look for more ancestors */
                }
            }
          if (!found)
            ancestor = TtaGetParent (ancestor);
        }
      if (!found)
        {
          /* ancestors don't have attribute text-anchor */
          if (val == SVG_ATTR_text_anchor_VAL_start)
            /* removing or inheriting the default value. Done. */
            el = NULL;
          else
            /* apply the default value to element el and all its descendants*/
            val = SVG_ATTR_text_anchor_VAL_start;
        }
    }
  if (el)
    /* applies the new value to element el and all its descendants */
    {
      if (val == SVG_ATTR_text_anchor_VAL_start)
        ctxt->type = PositionLeft;
      else if (val == SVG_ATTR_text_anchor_VAL_middle)
        ctxt->type = PositionVertMiddle;
      else if (val == SVG_ATTR_text_anchor_VAL_end__)
        ctxt->type = PositionRight;
      else
        ctxt->type = PositionLeft;
      ctxt->cssSpecificity = 2000;
      ctxt->important = FALSE;
      ctxt->destroy = FALSE;
      SetTextAnchorTree (el, ctxt, SvgSSchema, attr);
    }
  TtaFreeMemory (ctxt);
}

/*----------------------------------------------------------------------
  CreateCSSRules
  The style element el is created within the SVG document doc. Parse its
  content to create the equivalent style sheet.
  ----------------------------------------------------------------------*/
void CreateCSSRules (Element el, Document doc)
{
  ElementType     elType;
  AttributeType   attrType;
  Attribute       attr;
  int             length;
  char            *text;
  ThotBool        parseCSS;

  /* Search the  type attribute */
  elType = TtaGetElementType (el);
  attrType.AttrSSchema = elType.ElSSchema;
  attrType.AttrTypeNum = SVG_ATTR_type;
  attr = TtaGetAttribute (el, attrType);
  parseCSS = FALSE;
  if (attr == NULL)
    /* no type attribute. Assume CSS by default */
    parseCSS = TRUE;
  else
    /* the style element has a type attribute */
    /* get its value */
    {
      length = TtaGetTextAttributeLength (attr);
      text = (char *)TtaGetMemory (length + 1);
      TtaGiveTextAttributeValue (attr, text, &length);
      if (!strcasecmp (text, "text/css"))
        parseCSS = TRUE;
      TtaFreeMemory (text);
    }
  if (parseCSS)
    {
      text = GetStyleContents (el);
      if (text)
        {
          ReadCSSRules (doc, NULL, text, NULL, TtaGetElementLineNumber (el),
                        FALSE, el); 
          TtaFreeMemory (text);
        }
    }
}

/*----------------------------------------------------------------------
  ParsePreserveAspectRatioAttribute
  Parse the value of a preserveAspectRatio attribute
  ----------------------------------------------------------------------*/
static void ParsePreserveAspectRatioAttribute (Attribute attr, Element el,
                                            Document doc, int* align,
                                            int* meetOrSlice, ThotBool delete_)
{
  int                  length;
  char                *text, *ptr;

  length = TtaGetTextAttributeLength (attr) + 2;
  text = (char *)TtaGetMemory (length);
  *align = 0;
  *meetOrSlice = 0;
  if (text)
    {
      /* get the value attribute */
      TtaGiveTextAttributeValue (attr, text, &length);
      /* parse the attribute value */
      ptr = text;
      if (*ptr != EOS)
        {
          /* skip space characters */
          ptr = (char*)TtaSkipBlanks (ptr);
          if (!strncmp (ptr, "defer", 5))
            /* ignore value "defer" */
            {
              ptr+= 5;
              ptr = (char*)TtaSkipBlanks (ptr);
            }
          /* check the <align> value */
          if (!strncmp (ptr, "none", 4))
            {
              *align = 1;
              ptr += 4;
            }
          else if (!strncmp (ptr, "xMinYMin", 8))
            {
              *align = 2;
              ptr += 8;
            }
          else if (!strncmp (ptr, "xMidYMin", 8))
            {
              *align = 3;
              ptr += 8;
            }
          else if (!strncmp (ptr, "xMaxYMin", 8))
            {
              *align = 4;
              ptr += 8;
            }
          else if (!strncmp (ptr, "xMinYMid", 8))
            {
              *align = 5;
              ptr += 8;
            }
          else if (!strncmp (ptr, "xMidYMid", 8))
            {
              *align = 6;
              ptr += 8;
            }
          else if (!strncmp (ptr, "xMaxYMid", 8))
            {
              *align = 7;
              ptr += 8;
            }
          else if (!strncmp (ptr, "xMinYMax", 8))
            {
              *align = 8;
              ptr += 8;
            }
          else if (!strncmp (ptr, "xMidYMax", 8))
            {
              *align = 9;
              ptr += 8;
            }
          else if (!strncmp (ptr, "xMaxYMax", 8))
            {
              *align = 10;
              ptr += 8;
            }

          if (*align > 1)
            /* a valid <align> value was found, look for a <meetOrSlice>
               value */
            {
              ptr = SkipBlanksAndComma (ptr);
              if (!strncmp (ptr, "slice", 5))
                *meetOrSlice = 2;
              else
                *meetOrSlice = 1; /* meet, default value */
            }
        }
    }
}

/*----------------------------------------------------------------------
  SVGElementCreated
  The XML parser has just inserted a new element in the abstract tree,
  with all its attributes, but without its children.
  ----------------------------------------------------------------------*/
void SVGElementCreated (Element el, Document doc)
{
  ElementType		elType;
  AttributeType attrType;
  Attribute     attr, attrViewBox, attrAspectRatio;
  float         vBX, vBY, vBWidth, vBHeight;
  int           align, meetOrSlice;
  char          msgBuffer[100];

  elType = TtaGetElementType (el);

  if (elType.ElTypeNum == SVG_EL_SVG ||
      elType.ElTypeNum == SVG_EL_symbol_ ||
      elType.ElTypeNum == SVG_EL_image ||
      elType.ElTypeNum == SVG_EL_foreignObject ||
      elType.ElTypeNum == SVG_EL_marker ||
      elType.ElTypeNum == SVG_EL_pattern ||
      elType.ElTypeNum == SVG_EL_view)
    /* process attributes preserveAspectRatio and viewBox */
    {
      attrType.AttrSSchema = elType.ElSSchema;
      attrType.AttrTypeNum = SVG_ATTR_viewBox;
      attrViewBox = TtaGetAttribute (el, attrType);
      if (attrViewBox || elType.ElTypeNum == SVG_EL_image)
        {
          if (attrViewBox)
            ParseviewBoxAttribute (attrViewBox, el, doc, &vBX, &vBY, &vBWidth,
                                   &vBHeight, FALSE);
          else
            {
              vBX = 0; vBY = 0; vBWidth = -1; vBHeight = -1;
            }
          attrType.AttrTypeNum = SVG_ATTR_preserveAspectRatio;
          attrAspectRatio = TtaGetAttribute (el, attrType);
          if (attrAspectRatio)
            ParsePreserveAspectRatioAttribute (attrAspectRatio, el, doc,
                                               &align, &meetOrSlice, FALSE);
          else
            {
              align = 6; /* preserveAspectRatio = xMidYMid by default */
              meetOrSlice = 1; /* meet by default */
            }
          if (attrViewBox || attrAspectRatio)
            TtaInsertTransform (el, TtaNewTransformViewBox (vBX, vBY, vBWidth,
                                           vBHeight, align, meetOrSlice), doc);
        }
    }
  else if (elType.ElTypeNum == SVG_EL_ellipse)
    /* an ellipse. If attributes rx and/or ry are missing, create
       a default presentation rule for the default value: 0 */
    {
      attrType.AttrSSchema = elType.ElSSchema;
      attrType.AttrTypeNum = SVG_ATTR_rx;
      attr = TtaGetAttribute (el, attrType);
      if (!attr)
        {
          sprintf (msgBuffer, "Attribute rx mandatory in ellipse");
          XmlParseError (errorParsing, (unsigned char *)msgBuffer, 0);
        }
      attrType.AttrTypeNum = SVG_ATTR_ry;
      attr = TtaGetAttribute (el, attrType);
      if (!attr)
        {
          sprintf (msgBuffer, "Attribute ry mandatory in ellipse");
          XmlParseError (errorParsing, (unsigned char *)msgBuffer, 0);
        }
    }
}

/*----------------------------------------------------------------------
  InstanciateUseElements
  Find all <use> element in the subtree of element el and instanciate
  those with no child.
  ----------------------------------------------------------------------*/
static void InstanciateUseElements (Element el, Document doc)
{
  Element       use;
  ElementType   elType;
  Attribute     attr;
  AttributeType attrType;
  int           length;
  char          *href;

  elType = TtaGetElementType (el);
  elType.ElTypeNum = SVG_EL_use_;
  attrType.AttrSSchema = elType.ElSSchema;
  attrType.AttrTypeNum = SVG_ATTR_xlink_href;
  use = el;
  do
    {
      use = TtaSearchTypedElementInTree (elType, SearchForward, el, use);
      if (use && !TtaGetFirstChild (use))
        {
          attr = TtaGetAttribute (use, attrType);
          if (attr)
            {
              length = TtaGetTextAttributeLength (attr);
              href = (char *)TtaGetMemory (length + 1);
              TtaGiveTextAttributeValue (attr, href, &length);
              CopyUseContent (use, doc, href);
              TtaFreeMemory (href);
            }
        }
    }
  while (use);
}

/*----------------------------------------------------------------------
  GetNumber
  Parse an integer or floating point number and skip to the next token.
  Return the value of that number in number and moves ptr to the next
  token to be parsed.
  If the string to be parsed is not a valid number, set error to TRUE.
  ----------------------------------------------------------------------*/
char *SVG_GetNumber (char *ptr, int* number, ThotBool *error)
{
  int      integer, nbdecimal, exponent, i;
  char     *decimal;
  ThotBool negative, negativeExp;

  *number = 0;
  *error = FALSE;
  integer = 0;
  nbdecimal = 0;
  decimal = NULL;
  negative = FALSE;
  /* read the sign */
  if (*ptr == '+')
    ptr++;
  else if (*ptr == '-')
    {
      ptr++;
      negative = TRUE;
    }

  if (*ptr < '0' || *ptr > '9')
    {
      *error = TRUE;
      ptr++;
      return (ptr);
    }
  /* read the integer part */
  while (*ptr != EOS && *ptr >= '0' && *ptr <= '9')
    {
      integer *= 10;
      integer += *ptr - '0';
      ptr++;
    }
  if (*ptr == '.')
    /* there is a decimal part */
    {
      ptr++;
      decimal = ptr;
      while (*ptr != EOS &&  *ptr >= '0' && *ptr <= '9')
        {
          nbdecimal++;
          ptr++;
        }
    }

  if (*ptr != 'e' && *ptr != 'E')
    /* no exponent */
    {
      if (nbdecimal > 0)
        /* there are some digits after the decimal point */
        {
          if (*decimal >= '5' && *decimal <= '9')
            /* the first digit after the point is 5 of greater
               round up the value to the next integer */
            integer++;
        }
    }
  else
    /* there is an exponent part, parse it */
    {
      ptr++;
      negativeExp = FALSE;
      /* read the sign of the exponent */
      if (*ptr == '+')
        ptr++;
      else if (*ptr == '-')
        {
          ptr++;
          negativeExp = TRUE;
        }
      exponent = 0;

      if (*ptr < '0' || *ptr > '9')
        {
          *error = TRUE;
          ptr++;
          return (ptr);
        }

      while (*ptr != EOS &&  *ptr >= '0' && *ptr <= '9')
        {
          exponent *= 10;
          exponent += *ptr - '0';
          ptr++;
        }
      if (exponent > 0)
        {
          if (negativeExp)
            {
              for (i = 0; i < exponent; i++)
                integer /= 10;
            }
          else
            {
              for (i = 0; i < exponent; i++)
                {
                  integer *= 10;
                  if (i < nbdecimal)
                    {
                      integer += *decimal - '0';
                      decimal++;
                    }
                }
            }
        }
    }

  if (negative)
    *number = - integer;
  else
    *number = integer;

  /* skip the following spaces */
  while (*ptr != EOS &&
         (*ptr == ',' || *ptr == SPACE || *ptr == BSPACE ||
          *ptr == EOL    || *ptr == TAB   || *ptr == __CR__))
    ptr++;
  return (ptr);
}

/*----------------------------------------------------------------------
  ParsePointsBuffer
  Process the points attribute
  ----------------------------------------------------------------------*/
void ParsePointsBuffer (char *text, Element leaf, Document doc)
{
  DisplayMode  dispMode;
  TypeUnit		   unit;
  char		    *ptr;
  int          x, y, nbPoints, maxX, maxY, minX, minY, i;
  ThotBool     error;

  if (text)
    {
      dispMode = TtaGetDisplayMode (doc);
      if (dispMode == DisplayImmediately)
        TtaSetDisplayMode (doc, DeferredDisplay);

      /* first, delete all points in the polyline */
      nbPoints = TtaGetPolylineLength (leaf);
      for (i = 1; i <= nbPoints; i++)
        TtaDeletePointInPolyline (leaf, i, doc);

      // set new points
      ptr = text;
      error = FALSE;
      ptr = (char*)TtaSkipBlanks (ptr);
      nbPoints = 0;
      minX = minY = 32000;
      maxX = maxY = 0;
      unit = UnPixel;
      while (*ptr != EOS && !error)
        {
          x = y = 0;
          ptr = SVG_GetNumber (ptr, &x, &error);
          if (x > maxX)
            maxX = x;
          if (x < minX)
            minX = x;
          if (*ptr == EOS)
            error = TRUE;
          if (*ptr == ',')
            {
              ptr++;
              ptr = (char*)TtaSkipBlanks (ptr);
            }
          if (!error)
            {
              ptr = SVG_GetNumber (ptr, &y, &error);
              if (y > maxY)
                maxY = y;
              if (y < minY)
                minY = y;
              if (!error)
                {
                  nbPoints++;
                  TtaAddPointInPolyline (leaf, nbPoints, unit, x, y, doc, FALSE);
                  if (*ptr == ',')
                    {
                      ptr++;
                      ptr = (char*)TtaSkipBlanks (ptr);
                    }
                }
            }
        }
      TtaSetDisplayMode (doc, dispMode);
    }
}

/*----------------------------------------------------------------------
  ConvertLineAttributesToPath 
  ----------------------------------------------------------------------*/
char *ConvertLineAttributesToPath (Element el)
{
  ElementType		       elType;
  AttributeType        attrType;
  Attribute            attr;
  char                 *buffer;
  int                  length, l;

  elType = TtaGetElementType (el);
  // update an enclosing polyline
  buffer = (char *)TtaGetMemory (100);
  buffer[0] = EOS;
  attrType.AttrSSchema = elType.ElSSchema;
  attrType.AttrTypeNum = SVG_ATTR_x1;
  attr = TtaGetAttribute (el, attrType);
  if (attr)
    {
      length = 10;
      TtaGiveTextAttributeValue (attr, buffer, &length);
      l = strlen (buffer)-1;
      while (l > 0 && !isdigit(buffer[l]))
        buffer[l--] = EOS;
      strcat (buffer, ",");
    }
  attrType.AttrTypeNum = SVG_ATTR_y1;
  attr = TtaGetAttribute (el, attrType);
  if (attr)
    {
      length = 10;
      l = strlen(buffer);
      TtaGiveTextAttributeValue (attr, &buffer[l], &length);
      l = strlen (buffer)-1;
      while (l > 0 && !isdigit(buffer[l]))
        buffer[l--] = EOS;
      strcat (buffer, " ");
    }
  attrType.AttrTypeNum = SVG_ATTR_x2;
  attr = TtaGetAttribute (el, attrType);
  if (attr)
    {
      length = 10;
      l = strlen(buffer);
      TtaGiveTextAttributeValue (attr, &buffer[l], &length);
      l = strlen (buffer)-1;
      while (l > 0 && !isdigit(buffer[l]))
        buffer[l--] = EOS;
      strcat (buffer, ",");
    }
  attrType.AttrTypeNum = SVG_ATTR_y2;
  attr = TtaGetAttribute (el, attrType);
  if (attr)
    {
      length = 10;
      l = strlen(buffer);
      TtaGiveTextAttributeValue (attr, &buffer[l], &length);
      l = strlen (buffer)-1;
      while (l > 0 && !isdigit(buffer[l]))
        buffer[l--] = EOS;
    }
  return buffer;
}

/*----------------------------------------------------------------------
  GraphicLeafComplete
  ----------------------------------------------------------------------*/
void GraphicLeafComplete(Document doc, Element el)
{
  Element              leaf;
  ElementType	       elType;
  int                  w, h, rx = 0,ry = 0;
  PresentationContext  ctxt;
  PresentationValue    pval;
  ThotBool	       closedShape;
  ThotBool             shape_recognition;
  PRule		       fillPatternRule, newPRule;

  leaf = CreateGraphicLeaf (el, doc, &closedShape);
  /* if it's a closed shape, move the FillPattern rule to that leaf */
  if (closedShape && leaf)
    {
      fillPatternRule = TtaGetPRule (el, PRFillPattern);
      if (fillPatternRule != NULL)
	{
	  newPRule = TtaCopyPRule (fillPatternRule);
	  TtaAttachPRule (leaf, newPRule, doc);
	  TtaRemovePRule (el, fillPatternRule, doc);
	}
    }

  if (!TtaGetEnvBoolean ("ENABLE_SHAPE_RECOGNITION", &shape_recognition))
    shape_recognition = TRUE;
  elType = TtaGetElementType (el);

  /* Check the geometric properties of the leaf */
  if(shape_recognition && (elType.ElTypeNum == SVG_EL_polygon ||
			   elType.ElTypeNum == SVG_EL_path))
    {
      if (CheckGeometricProperties(doc, leaf, &w, &h, &rx, &ry))
	{
	  ctxt = TtaGetSpecificStyleContext (doc);
	  /* the specific presentation is not a CSS rule */
	  ctxt->cssSpecificity = 2000;
	  ctxt->destroy = FALSE;
	  pval.typed_data.real = FALSE;
	  pval.typed_data.unit = UNIT_PX;

	  pval.typed_data.value = w;
	  TtaSetStylePresentation (PRWidth, el, NULL, ctxt, pval);
	  pval.typed_data.value = h;
	  TtaSetStylePresentation (PRHeight, el, NULL, ctxt, pval);

	  if(rx)
	    {
	      pval.typed_data.value = rx;
	      TtaSetStylePresentation (PRXRadius, el, NULL, ctxt, pval);
	    }
		  
	  if(ry)
	    {
	      pval.typed_data.value = ry;
	      TtaSetStylePresentation (PRYRadius, el, NULL, ctxt, pval);
	    }

	  /* Update transform attribute */
	  UpdateTransformMatrix(doc, el);

	  /* Update points attribute */
	  UpdatePointsOrPathAttribute(doc, el, w, h, FALSE);
	}
    }
}

/*----------------------------------------------------------------------
  SVGCheckInsert
  The XML parser has just created an element in the SVG namespace, before
  parsing any of its attribute.
  ----------------------------------------------------------------------*/
void SVGCheckInsert (Element *el, Element parent,
		     Document doc, ThotBool *inserted)
{
  ElementType  elType;

  elType = TtaGetElementType (*el);
  if (elType.ElTypeNum == SVG_EL_linearGradient)
    TtaNewGradient (TRUE, *el);
  else if (elType.ElTypeNum == SVG_EL_radialGradient)
    TtaNewGradient (FALSE, *el);
  else if (elType.ElTypeNum == SVG_EL_stop)
    TtaNewGradientStop (*el, parent);
  *inserted = FALSE;
  return;
}

/*----------------------------------------------------------------------
  SVGElementComplete
  Check the Thot structure of the SVG element el.
  ----------------------------------------------------------------------*/
void SVGElementComplete (ParserData *context, Element el, int *error)
{
  Document             doc;   
  ElementType	       elType, parentType, newType;
  Element	       child, parent, new_, leaf;
  AttributeType        attrType;
  Attribute            attr;
  int                  length;
  SSchema	       SVGSSchema;
  char                 *href, *buffer;
  ThotBool	       ok, closed;

  *error = 0;
  doc = context->doc;
  elType = TtaGetElementType (el);
  SVGSSchema = GetSVGSSchema (doc);
  if (elType.ElSSchema != SVGSSchema)
    /* this is not a SVG element. It's the HTML element <XMLGraphics>, or
       any other element containing a SVG expression */
    {
      if (TtaGetFirstChild (el) == NULL && !TtaIsLeaf (elType))
        /* this element is empty. Create a SVG element as it's child */
        {
          newType.ElSSchema = SVGSSchema;
          newType.ElTypeNum = SVG_EL_SVG;
          new_ = TtaNewElement (doc, newType);
          TtaInsertFirstChild (&new_, el, doc);
          /* Create a placeholder within the SVG element */
          newType.ElTypeNum = SVG_EL_GraphicsElement;
          child = TtaNewElement (doc, newType);
          TtaInsertFirstChild (&child, new_, doc);
        }
    }
  else
    /* this element belongs to the SVG vocabulary */
    {
      /* if the parent element is defined by a different SSchema, insert
         a SVG root element between the element and its parent */
      parent = TtaGetParent (el);
      if (parent)
        {
          parentType = TtaGetElementType (parent);
          if (parentType.ElSSchema != elType.ElSSchema)
            {
              if (elType.ElTypeNum != SVG_EL_SVG)
                {
                  newType.ElSSchema = SVGSSchema;
                  newType.ElTypeNum = SVG_EL_SVG;
                  CreateEnclosingElement (el, newType, doc);
                }
              else
                /* apply a depth rule to each child */
                SetGraphicDepths (doc, el);
            }
        }

      if (elType.ElTypeNum == SVG_EL_SVG ||
          elType.ElTypeNum == SVG_EL_g ||
          elType.ElTypeNum == SVG_EL_defs ||
          elType.ElTypeNum == SVG_EL_symbol_ ||
          elType.ElTypeNum == SVG_EL_marker ||
          elType.ElTypeNum == SVG_EL_use_ ||
          elType.ElTypeNum == SVG_EL_switch ||
          elType.ElTypeNum == SVG_EL_text_ ||
          elType.ElTypeNum == SVG_EL_tspan ||
          elType.ElTypeNum == SVG_EL_tref ||
          elType.ElTypeNum == SVG_EL_textPath ||
          elType.ElTypeNum == SVG_EL_altGlyph ||
          elType.ElTypeNum == SVG_EL_pattern ||
          elType.ElTypeNum == SVG_EL_clipPath ||
          elType.ElTypeNum == SVG_EL_mask ||
          elType.ElTypeNum == SVG_EL_filter ||
          elType.ElTypeNum == SVG_EL_feImage ||
          elType.ElTypeNum == SVG_EL_a ||
          elType.ElTypeNum == SVG_EL_font_ ||
          elType.ElTypeNum == SVG_EL_glyph ||
          elType.ElTypeNum == SVG_EL_missing_glyph ||
          elType.ElTypeNum == SVG_EL_foreignObject)
        /* add textPath, altGlyph, clipPath, pattern, mask,
           filter, feImage, font, glyph, missing_glyph */
        /* this element may have a text-anchor attribute */
        {
          attrType.AttrSSchema = elType.ElSSchema;
          attrType.AttrTypeNum = SVG_ATTR_text_anchor;
          attr = TtaGetAttribute (el, attrType);
          if (attr)
            SetTextAnchor (attr, el, doc, FALSE);
        }

      if (elType.ElTypeNum == SVG_EL_set_ || 
          elType.ElTypeNum == SVG_EL_animate ||    
          elType.ElTypeNum == SVG_EL_animateColor ||       
          elType.ElTypeNum == SVG_EL_animateMotion ||
          elType.ElTypeNum == SVG_EL_animateTransform)
        register_animated_element (el, doc);

      switch (elType.ElTypeNum)
        {	 
        case SVG_EL_foreignObject:
        case SVG_EL_symbol_:
        case SVG_EL_marker:
          /* case SVG_EL_view: */
          TtaSetElCoordinateSystem (el);
          break;

        case SVG_EL_g:
          TtaSetElCoordinateSystem (el);
          break;

        case SVG_EL_SVG:
          TtaSetElCoordinateSystem (el);
          /* if the SVG element has a UnresolvedRef attribute, process all
             use elements in the subtree that have not been instanciated yet */
          attrType.AttrSSchema = elType.ElSSchema;
          attrType.AttrTypeNum = SVG_ATTR_UnresolvedRef;
          attr = TtaGetAttribute (el, attrType);
          if (attr)
            {
              InstanciateUseElements (el, doc);
              TtaRemoveAttribute (el, attr, doc);
            } 
          break;

        case SVG_EL_image:
          /* it's an image element */
          /* create a PICTURE_UNIT child */
          newType.ElSSchema = elType.ElSSchema;
          newType.ElTypeNum = SVG_EL_PICTURE_UNIT;
          leaf = TtaNewElement (doc, newType);
          TtaInsertFirstChild (&leaf, el, doc);
          /* if it has a href attribute from the XLink namespace, replace
             that attribute by a href attribute from the SVG namespace */
          CheckHrefAttr (el, doc);
          break;

        case SVG_EL_title:
          UpdateTitle (el, doc);
          break;

        case SVG_EL_use_:
        case SVG_EL_tref:
        case SVG_EL_linearGradient:
        case SVG_EL_radialGradient:
          /* make a transclusion of the element addressed by its xlink_href
             attribute after the last child */
          /* if the element has a href attribute from the XLink namespace,
	     replace that attribute by a href attribute from the SVG namespace*/
          attr = CheckHrefAttr (el, doc);
          /* first, get the xlink:href attribute */
	  if (!attr)
	    {
	      attrType.AttrSSchema = elType.ElSSchema;
	      attrType.AttrTypeNum = SVG_ATTR_xlink_href;
	      attr = TtaGetAttribute (el, attrType);
	    }
          if (attr)
            /* the use element has a xlink:href attribute */
            {
              /* get its value */
              length = TtaGetTextAttributeLength (attr);
              href = (char *)TtaGetMemory (length + 1);
              TtaGiveTextAttributeValue (attr, href, &length);
              ok = CopyUseContent (el, doc, href);
              TtaFreeMemory (href);
              if (!ok)
                /* the referred element was not found. It may be a forward
                   reference to an element that has not been parsed yet.
                   We should retry when the document is complete. */
		SetAttributeOnRoot (el, SVG_ATTR_UnresolvedRef, doc);
            }
          break;

	case SVG_EL_path:
        case SVG_EL_polyline:
        case SVG_EL_polygon:
          GraphicLeafComplete(doc, el);
          break;

        case SVG_EL_a:
          /* it's an anchor element */
          /* if it has a href attribute from the XLink namespace, replace
             that attribute by a href attribute from the SVG namespace */
          CheckHrefAttr (el, doc);
          break;

        case SVG_EL_switch:
          /* it's a switch element */
          /* Evaluate the requiredFeatures, requiredExtensions and
             systemLanguage attributes on its direct child elements to select
             the child to be rendered */
          EvaluateTestAttrs (el, doc);
          break;

        case SVG_EL_style__:
          /* it's a style element, parse its contents as a style sheet */
          CreateCSSRules (el, doc);
          break;

        case SVG_EL_defs:
        case SVG_EL_mask:
        case SVG_EL_pattern:
        case SVG_EL_filter:
        case SVG_EL_feImage:
        case SVG_EL_font_:
        case SVG_EL_glyph:
        case SVG_EL_missing_glyph:
          break;

        case SVG_EL_line_:
          /* create (or get) the Graphics leaf according to the element type */
          leaf = CreateGraphicLeaf (el, doc, &closed);
          if (leaf == NULL)
            return;
          buffer = ConvertLineAttributesToPath (el);
          ParsePointsBuffer (buffer, leaf, doc);
          TtaFreeMemory (buffer);
          buffer = NULL;
          break;

        default:
          /* if it's a graphic primitive, create a GRAPHIC_UNIT leaf as a child
             of the element, if it has not been done when creating attributes
             (points, rx, ry) */
	  GraphicLeafComplete(doc, el);
          break;
        }
    }
}

/*----------------------------------------------------------------------
  UnknownSVGNameSpace
  The element doesn't belong to a supported namespace
  ----------------------------------------------------------------------*/
void UnknownSVGNameSpace (ParserData *context, Element *unknownEl,
                          char* content)
{
  ElementType     elType;
  Element         elText;

  /* Create a new Invalid_element */
  elType.ElSSchema = GetXMLSSchema (SVG_TYPE, context->doc);
  elType.ElTypeNum = SVG_EL_Unknown_namespace;
  *unknownEl = TtaNewElement (context->doc, elType);
  if (*unknownEl != NULL)
    {
      XmlSetElemLineNumber (*unknownEl);
      InsertXmlElement (unknownEl);
      context->lastElementClosed = TRUE;
      elType.ElTypeNum = SVG_EL_TEXT_UNIT;
      elText = TtaNewElement (context->doc, elType);
      XmlSetElemLineNumber (elText);
      TtaInsertFirstChild (&elText, *unknownEl, context->doc);
      TtaSetTextContent (elText, (unsigned char *)content, context->language, context->doc);
      TtaSetAccessRight (elText, ReadOnly, context->doc);
    }
}

/*----------------------------------------------------------------------
  UpdateTransformAttr
  update the "transform" attribute of element el to shift it by
  delta unit(s) horizontally (if firstParam) or vertically.
  increment indicates whether delta is an increment or the total value of
  the translation (only for translations).
  -----------------------------------------------------------------------*/
void UpdateTransformAttr (Element el, Document doc, const char *operation,
                          float value, ThotBool firstParam, ThotBool increment)
{
  ElementType           elType;
  AttributeType	        attrType;
  Attribute		          attr;
  ThotBool              error, found;
  char                  buffer[512];
  char                 *text, *ptr, *newText, *newPtr;
  int                   length, opLen, otherValue;
  float                 origValue;

  if (!strcmp (operation, "scale"))
    otherValue = 1;
  else
    otherValue = 0;
  elType = TtaGetElementType (el);
  attrType.AttrSSchema = elType.ElSSchema;
  attrType.AttrTypeNum = SVG_ATTR_transform;
  attr = TtaGetAttribute (el, attrType);
  if (attr == NULL)
    {
      attr = TtaNewAttribute (attrType);
      TtaAttachAttribute (el, attr, doc);
      if (firstParam)
        if (otherValue)
          sprintf (buffer, "%s(%f,%d)", operation, value, otherValue);
        else
          sprintf (buffer, "%s(%d,0)", operation, (int)value);
      else
        if (otherValue)
          sprintf (buffer, "%s(%d,%f)", operation, otherValue, value);
        else
          sprintf (buffer, "%s(0,%d)", operation, (int)value);
      TtaSetAttributeText (attr, buffer, el, doc);
      TtaRegisterAttributeCreate (attr, el, doc);
    }
  else
    {
      length = TtaGetTextAttributeLength (attr);
      text = (char *)TtaGetMemory (length + 1);
      if (text)
        {
          TtaGiveTextAttributeValue (attr, text, &length);
          ptr = text;
          newText = (char *)TtaGetMemory (length + 50);
          if (newText)
            {
              newPtr = newText;
              error = FALSE;
              found = FALSE;
              opLen = strlen(operation);
              while (*ptr != EOS && !error)
                {
                  if (!strncmp (ptr, operation, opLen))
                    {
                      found = TRUE;
                      strncpy (newPtr, ptr, opLen);
                      ptr += opLen; newPtr += opLen;
                      ptr = (char*)TtaSkipBlanks (ptr);
                      if (*ptr != '(')
                        error = TRUE;
                      else
                        {
                          *newPtr = '('; newPtr++;
                          ptr++;
                          ptr = (char*)TtaSkipBlanks (ptr);
                          if (firstParam)
                            {
                              if (increment || otherValue)
                                GetFloat (ptr, &origValue);
                              else
                                origValue = 0;
                              if (otherValue)
                                sprintf (newPtr, "%f", origValue*value);
                              else
                                sprintf (newPtr, "%d", (int)(origValue+value));
                              while (*newPtr != EOS)
                                newPtr++;
                              while (*ptr != ',' && *ptr != ')' &&
                                     *ptr != EOS)
                                ptr++;
                            }
                          else
                            {
                              while (*ptr != ',' && *ptr != ')' &&
                                     *ptr != EOS)
                                {
                                  *newPtr = *ptr;
                                  ptr++; newPtr++;
                                }
                              if (*ptr != EOS)
                                {
                                  *newPtr = ','; newPtr++;
                                  if (*ptr == ',')
                                    {
                                      ptr++;
                                      ptr = (char*)TtaSkipBlanks (ptr);
                                    }
                                  if (*ptr != ')' && *ptr != EOS &&
                                      (increment || otherValue))
                                    GetFloat (ptr, &origValue);
                                  else
                                    origValue = 0;
                                  if (otherValue)
                                    sprintf (newPtr, "%f", origValue*value);
                                  else
                                    sprintf (newPtr, "%d", (int)(origValue+value));
                                  while (*newPtr != EOS)
                                    newPtr++;
                                  while (*ptr != ')' && *ptr != EOS)
                                    ptr++;
                                }
                            }
                        }
                    }
                  else
                    {
                      *newPtr = *ptr;
                      ptr++; newPtr++;
                    }
                }
              *newPtr = EOS;
              if (!found)
                {
                  strcpy (newPtr, operation);
                  newPtr += opLen;
                  if (firstParam)
                    if (otherValue)
                      sprintf (newPtr, "(%f,%d)", value, (int)otherValue);
                    else
                      sprintf (newPtr, "(%d,0)", (int)value);
                  else
                    if (otherValue)
                      sprintf (newPtr, "(%d,%f)", (int)otherValue, value);
                    else
                      sprintf (newPtr, "(0,%d)", (int)value);
                }
              TtaRegisterAttributeReplace (attr, el, doc);
              TtaSetAttributeText (attr, newText, el, doc);
              TtaFreeMemory (newText);
            }
          TtaFreeMemory (text);
        }
    }
}

/*----------------------------------------------------------------------
  TranslateElement
  Translate element el by delta unit(s) horizontally (if horiz) or vertically.
  increment indicates wheter delta is an increment or the total value of
  the translation.
  -----------------------------------------------------------------------*/
void TranslateElement (Element el, Document doc, int delta, TypeUnit unit,
                       ThotBool horiz, ThotBool increment)
{
  ElementType           elType;

  elType = TtaGetElementType (el);
  if (elType.ElTypeNum == SVG_EL_line_)
    {
#ifdef IV
      AttributeType	        attrType;
      Attribute		        attr;
      char		        buffer[512];
      int			length;

      if (horiz)
        {
          /* update the first point */
          attrType.AttrTypeNum = SVG_ATTR_x1;
          attr = TtaGetAttribute (el, attrType);
          length = 50;
          if (attr == NULL)
            /* element el has no position attribute */
            {
              attr = TtaNewAttribute (attrType);
              TtaAttachAttribute (el, attr, doc);
              x = 0;
              unit = UnPixel;
            }
          else
            GetAttributeValueAndUnit (attr, &x, &unit);
          sprintf (buffer, "%dpx", x);
          TtaSetAttributeText (attr, buffer, el, doc);
          /* update the last point */
          attrType.AttrTypeNum = SVG_ATTR_x2;
          attr = TtaGetAttribute (el, attrType);
          if (attr == NULL)
            /* element el has no position attribute */
            {
              attr = TtaNewAttribute (attrType);
              TtaAttachAttribute (el, attr, doc);
              x = 0;
              unit = UnPixel;
            }
          else
            {
              x = 0;
            }
          sprintf (buffer, "%dpx", x);
          TtaSetAttributeText (attr, buffer, el, doc);
        }
      else
        {
          /* update the first point */
          attrType.AttrTypeNum = SVG_ATTR_y1;
          attr = TtaGetAttribute (el, attrType);
          length = 50;
          if (attr == NULL)
            /* element el has no position attribute */
            {
              attr = TtaNewAttribute (attrType);
              TtaAttachAttribute (el, attr, doc);
              y = 0;
              unit = UnPixel;
            }
          else
            {
              y = 0;
            }
          sprintf (buffer, "%dpx", y);
          TtaSetAttributeText (attr, buffer, el, doc);
          /* update the last point */
          attrType.AttrTypeNum = SVG_ATTR_y2;
          attr = TtaGetAttribute (el, attrType);
          if (attr == NULL)
            /* element el has no position attribute */
            {
              attr = TtaNewAttribute (attrType);
              TtaAttachAttribute (el, attr, doc);
              y = 0;
              unit = UnPixel;
            }
          else
            {
              y = 0;
            }
          sprintf (buffer, "%dpx", y);
          TtaSetAttributeText (attr, buffer, el, doc);
        }
#endif /* IV */
    }
  else
    /* update (or create) the transform attribute for the element */
    UpdateTransformAttr (el, doc, "translate", (float)delta, horiz, increment);
}

/*----------------------------------------------------------------------
  UpdatePositionOfPoly
  Set position, width and height for an element polyline or polygon.
  Change coords of control points accordingly.
  -----------------------------------------------------------------------*/
void UpdatePositionOfPoly (Element el, Document doc, int minX, int minY,
                           int maxX, int maxY)
{
  /*PRule                pRule;*/
  Element              leaf;
  Attribute            attr;
  AttributeType        attrType;
  int			height, width, i, nbPoints, x, y;
  TypeUnit		unit;
  PresentationValue    pval;
  PresentationContext  ctxt;
  char                 *buffer, *text;

  leaf = TtaGetFirstChild (el);  /* Thot Graphic element */
  ctxt = TtaGetSpecificStyleContext (doc);
  /* the specific presentation is not a CSS rule */
  ctxt->cssSpecificity = 2000;
  ctxt->destroy = FALSE;
  pval.typed_data.unit = UNIT_PX;

  unit = UnPixel;
  width = maxX; height = maxY;
  if (minX != 0 || minY != 0)
    {
      /* translate all coordinates by (-minX, -minY), both in the Thot
         Graphic leaf element and in the "points" attribute */
      nbPoints = TtaGetPolylineLength (leaf);
      buffer = (char *)TtaGetMemory (20);
      text = (char *)TtaGetMemory (nbPoints * 20);
      text[0] = EOS;
      for (i = 1; i <= nbPoints; i++)
        {
          TtaGivePolylinePoint (leaf, i, unit, &x, &y);
          x -= minX;  y -= minY;
          TtaModifyPointInPolyline (leaf, i, unit, x, y, doc);
          sprintf (buffer, "%d,%d", x, y);
          strcat (text, buffer);
          if (i < nbPoints)
            strcat (text, " ");
        }
      attrType.AttrSSchema = TtaGetElementType(el).ElSSchema;
      attrType.AttrTypeNum = SVG_ATTR_points;
      attr = TtaGetAttribute (el, attrType);
      if (!attr)
        {
          attr = TtaNewAttribute (attrType);
          TtaAttachAttribute (el, attr, doc);
        }
      TtaSetAttributeText (attr, text, el, doc);
      TtaFreeMemory (buffer);
      TtaFreeMemory (text);
    }

  width = maxX - minX;
  height = maxY - minY;
  TtaChangeLimitOfPolyline (leaf, unit, width, height, doc);
  pval.typed_data.value = width;
  pval.typed_data.real = FALSE;
  TtaSetStylePresentation (PRWidth, el, NULL, ctxt, pval);    
  pval.typed_data.value = height;
  pval.typed_data.real = FALSE;
  TtaSetStylePresentation (PRHeight, el, NULL, ctxt, pval);

  /*
    Append a translation rather than setting the presentation rules, otherwise
    the transform data is incorrect - F.Wang

    if (minX != 0)
    {
      pRule = TtaGetPRule (el, PRHorizPos);
      if (pRule)
        {
          x = TtaGetPRuleValue (pRule);
          unit = (TypeUnit)TtaGetPRuleUnit (pRule);
        }
      else
        x = 0;
      pval.typed_data.value = x+minX;
      pval.typed_data.real = FALSE;
      pval.typed_data.mainValue = TRUE;
      TtaSetStylePresentation (PRHorizPos, el, NULL, ctxt, pval);
    }

  if (minY != 0)
    {
      pRule = TtaGetPRule (el, PRVertPos);
      if (pRule)
        {
          y = TtaGetPRuleValue (pRule);
          unit = (TypeUnit)TtaGetPRuleUnit (pRule);
        }
      else
        y = 0;
      pval.typed_data.value = y+minY;
      pval.typed_data.real = FALSE;
      pval.typed_data.mainValue = TRUE;
      TtaSetStylePresentation (PRVertPos, el, NULL, ctxt, pval);
      }*/

  TtaFreeMemory (ctxt);

  /* create or update the transform attribute to represent an additional
     translation by (minX, minY) */
  if (minX != 0)
    TranslateElement (el, doc, minX, unit, TRUE, TRUE);
    
  if (minY != 0)
    TranslateElement (el, doc, minY, unit, FALSE, TRUE);

  TtaAppendTransform (el, TtaNewTransformTranslate((float)minX, (float)minY));
}

/*----------------------------------------------------------------------
  ParseCoordAttribute
  Create or update a specific presentation rule for element el that reflects
  the value of the x, y, cx, cy, x1, x2, y1, y2, dx, or dy attribute attr.
  ----------------------------------------------------------------------*/
void ParseCoordAttribute (Attribute attr, Element el, Document doc)
{
  int                  length, attrKind, ruleType;
  char                *text, *ptr;
  ElementType          elType;
  AttributeType        attrType;
  PresentationValue    pval;
  PresentationContext  ctxt;
  ThotBool             important;


  elType = TtaGetElementType (el);
/*   if (elType.ElTypeNum == SVG_EL_line_ && */
/*       !strcmp (TtaGetSSchemaName (elType.ElSSchema), "SVG")) */
/*     return; */

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
      if (pval.typed_data.unit == UNIT_BOX)
        pval.typed_data.unit = UNIT_PX;
      if (pval.typed_data.unit != UNIT_INVALID)
        {
          important = FALSE;
          pval.typed_data.mainValue = TRUE;
          /* decide of the presentation rule to be created or updated */
          TtaGiveAttributeType (attr, &attrType, &attrKind);
          if (attrType.AttrTypeNum == SVG_ATTR_x)
            ruleType = PRHorizPos;
          else if (attrType.AttrTypeNum == SVG_ATTR_y)
            ruleType = PRVertPos;
          else if (attrType.AttrTypeNum == SVG_ATTR_cx)
            ruleType = PRHorizPos;
          else if (attrType.AttrTypeNum == SVG_ATTR_cy)
            ruleType = PRVertPos;
          else if (attrType.AttrTypeNum == SVG_ATTR_x1)
            ruleType = PRHorizPos;
          else if (attrType.AttrTypeNum == SVG_ATTR_y1)
            ruleType = PRVertPos;
          else if (attrType.AttrTypeNum == SVG_ATTR_x2)
            {
              ruleType = PRWidth;
              important = TRUE;
            }
          else if (attrType.AttrTypeNum == SVG_ATTR_y2) 
            {
              ruleType = PRHeight;
              important = TRUE;
            }
          else if (attrType.AttrTypeNum == SVG_ATTR_dx)
            {
              ruleType = PRHorizPos;
              pval.typed_data.mainValue = FALSE;
            }
          else if (attrType.AttrTypeNum == SVG_ATTR_dy)
            {
              ruleType = PRVertPos;
              pval.typed_data.mainValue = FALSE;
            }
          else
            return;
          ctxt = TtaGetSpecificStyleContext (doc);
          /* the specific presentation is not a CSS rule */
          ctxt->cssSpecificity = 2000;
          ctxt->destroy = FALSE;
          ctxt->important = important;
          TtaSetStylePresentation (ruleType, el, NULL, ctxt, pval);
          TtaFreeMemory (ctxt);
        }
      TtaFreeMemory (text);
    }
}

/*----------------------------------------------------------------------
  ParseWidthHeightAttribute
  Create or update a specific presentation rule for element el that reflects
  the value of attribute attr, which is width_, height_, r, rx, or ry.
  ----------------------------------------------------------------------*/
ThotBool ParseWidthHeightAttribute (Attribute attr, Element el, Document doc,
                                    ThotBool delete_)
{
  AttributeType	       attrType, attrType2;
  Attribute            attr2 = NULL;
  ElementType          elType;
  Element              child;
  int		               length, attrKind, ruleType;
  char		            *text, *ptr;
  PresentationValue    pval;
  PresentationContext  ctxt;
  ThotBool             ret;

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
  else if (attr == NULL || el == NULL)
    return ret;

  /* decide of the presentation rule to be created or updated */
  TtaGiveAttributeType (attr, &attrType, &attrKind);
  if (attrType.AttrTypeNum == SVG_ATTR_width_)
    ruleType = PRWidth;
  else if (attrType.AttrTypeNum == SVG_ATTR_height_)
    ruleType = PRHeight;
  else if (attrType.AttrTypeNum == SVG_ATTR_r)
    ruleType = PRWidth;
  else if (attrType.AttrTypeNum == SVG_ATTR_rx)
    if (elType.ElTypeNum == SVG_EL_rect)
      ruleType = PRXRadius;
    else
      ruleType = PRWidth;
  else if (attrType.AttrTypeNum == SVG_ATTR_ry)
    if (elType.ElTypeNum == SVG_EL_rect)
      ruleType = PRYRadius;
    else
      ruleType = PRHeight;
  else
    ruleType = PRWidth;

  if (ruleType == PRXRadius || ruleType == PRYRadius)
    /* that's the radius of a rounded corner. Get the graphics leaf
       which will receive the specific presentation rule */
    {
      child = TtaGetFirstChild (el);
      while (child &&
             TtaGetElementType(child).ElTypeNum != GRAPHICS_UNIT)
        TtaNextSibling (&child);
      // When a radius doesn't exist, apply the other radius value
      attrType2.AttrSSchema =  attrType.AttrSSchema;
      if (attrType.AttrTypeNum == SVG_ATTR_ry)
        attrType2.AttrTypeNum = SVG_ATTR_rx;
      else
        attrType2.AttrTypeNum = SVG_ATTR_ry;
      attr2 = TtaGetAttribute (el, attrType2);
    }
  else
    child = el;

  if (delete_)
    {
      /* attribute deleted */
      if (ruleType != PRXRadius && ruleType != PRYRadius)
        /* attribute mandatory. Do not delete */
        return TRUE;
      else
        {
          if (attr2)
            {
              length = TtaGetTextAttributeLength (attr) + 2;
              text = (char *)TtaGetMemory (length);
              TtaGiveTextAttributeValue (attr2, text, &length); 
            }
        }
    }
  else
    /* attribute created or modified */
    TtaGiveTextAttributeValue (attr, text, &length);

  if (child)
    {
      ctxt = TtaGetSpecificStyleContext (doc);
      /* the specific presentation is not a CSS rule */
      ctxt->cssSpecificity = 2000;
      if (text)
        {
          /* parse the attribute value (a number followed by a unit) */
          ptr = text;
          ptr = (char*)TtaSkipBlanks (ptr);
          ptr = ParseCSSUnit (ptr, &pval);
          if (pval.typed_data.unit == UNIT_BOX)
            pval.typed_data.unit = UNIT_PX;
          if (pval.typed_data.unit != UNIT_INVALID)
            {
              if ((elType.ElTypeNum == SVG_EL_ellipse ||
                   elType.ElTypeNum == SVG_EL_circle_) &&
                  (attrType.AttrTypeNum == SVG_ATTR_r ||
                   attrType.AttrTypeNum == SVG_ATTR_rx ||
                   attrType.AttrTypeNum == SVG_ATTR_ry))
                /* that's the radius of a circle or an ellipse,
                   multiply the value by 2 to set the width or height of the box */
                pval.typed_data.value *= 2;
            }
          /* set the specific presentation value of the box */
          ctxt->destroy = FALSE;
          TtaSetStylePresentation (ruleType, child, NULL, ctxt, pval);
          // update if necessary the other radius
          if (attr2 == NULL && ruleType == PRXRadius)
            TtaSetStylePresentation (PRYRadius, child, NULL, ctxt, pval);
          else if (attr2 == NULL && ruleType == PRYRadius)
            TtaSetStylePresentation (PRXRadius, child, NULL, ctxt, pval);
          TtaFreeMemory (text);
        }
      else
        {
          pval.typed_data.value = 0;
          pval.typed_data.unit = UNIT_PX;
          ctxt->destroy = FALSE;
          TtaSetStylePresentation (ruleType, child, NULL, ctxt, pval);
          ctxt->destroy = TRUE;
          TtaSetStylePresentation (ruleType, child, NULL, ctxt, pval);
          if (ruleType == PRXRadius)
            ruleType = PRYRadius;
          else
            ruleType = PRXRadius;
          ctxt->destroy = FALSE;
          TtaSetStylePresentation (ruleType, child, NULL, ctxt, pval);
          ctxt->destroy = TRUE;
          TtaSetStylePresentation (ruleType, child, NULL, ctxt, pval);
        }
      TtaFreeMemory (ctxt);
    }
  return ret;
}

/*----------------------------------------------------------------------
  ParseBaselineShiftAttribute
  Process the baseline-shift attribute
  ----------------------------------------------------------------------*/
void ParseBaselineShiftAttribute (Attribute attr, Element el, Document doc,
                                  ThotBool delete_)
{
  int                  length;
  char                 *text, *ptr;
  PresentationValue    pval;
  PresentationContext  ctxt;

  /* allowed values are
     baseline | sub | super | <percentage> | <length> | inherit */
  length = TtaGetTextAttributeLength (attr) + 2;
  text = (char *)TtaGetMemory (length);
  if (text)
    {
      /* get the value of the attribute */
      TtaGiveTextAttributeValue (attr, text, &length);
      /* parse the attribute value */
      ptr = text;
      /* skip space characters */
      ptr = (char*)TtaSkipBlanks (ptr);
      if (!strncmp (ptr, "baseline", 8))
        {
          pval.typed_data.value = 0;
          pval.typed_data.unit = UNIT_REL;
          pval.typed_data.real = FALSE;
        }
      else if (!strncmp (ptr, "sub", 3))
        {
          pval.typed_data.value = -3;
          pval.typed_data.unit = UNIT_REL;
          pval.typed_data.real = FALSE;
        }
      else if (!strncmp (ptr, "super", 5))
        {
          pval.typed_data.value = 4;
          pval.typed_data.unit = UNIT_REL;
          pval.typed_data.real = FALSE;
        }
      else if (!strncmp (ptr, "inherit", 7))
        {
          pval.typed_data.value = 0;
          pval.typed_data.unit = UNIT_REL;
          pval.typed_data.real = FALSE;
        }
      else
        {
          /* parse <percentage> or <length> */
          ptr = ParseCSSUnit (ptr, &pval);
          if (pval.typed_data.unit == UNIT_BOX)
            pval.typed_data.unit = UNIT_EM;
          else if (pval.typed_data.unit == UNIT_PERCENT)
            /* it's a percentage */
            {
              /* convert it into a relative size */
              pval.typed_data.unit = UNIT_REL;
              pval.typed_data.value /= 10;
            }
        }
      ctxt = TtaGetSpecificStyleContext (doc);
      /* the specific presentation is not a CSS rule */
      ctxt->cssSpecificity = 2000;
      ctxt->destroy = delete_;
      TtaSetStylePresentation (PRHorizRef, el, NULL, ctxt, pval);
      TtaFreeMemory (ctxt);
      TtaFreeMemory (text);
    }
}

/*----------------------------------------------------------------------
  ParsePointsAttribute
  Process the points attribute
  ----------------------------------------------------------------------*/
void ParsePointsAttribute (Attribute attr, Element el, Document doc)
{
  Element		   leaf;
  char		    *text;
  int          length;
  ThotBool     closed;

  /* create (or get) the Graphics leaf according to the element type */
  leaf = CreateGraphicLeaf (el, doc, &closed);
  if (leaf == NULL)
    return;
  /* text attribute. Get its value */
  length = TtaGetTextAttributeLength (attr) + 2;
  text = (char *)TtaGetMemory (length);
  if (text)
    {
      /* get the content of the points attribute */
      TtaGiveTextAttributeValue (attr, text, &length);
      ParsePointsBuffer (text, leaf, doc);
      /* This set the top left corner of the polyline to (0,0), and
         consequently added a translate attribute. Because the user may
         not want the XML structure to change, I removed it. - F.Wang
      if (nbPoints > 0)
         UpdatePositionOfPoly (el, doc, minX, minY, maxX, maxY); */
      TtaFreeMemory (text);
    }
}

/*----------------------------------------------------------------------
  ParseTransformAttribute
  Parse the value of a transform attribute
  if parameter gradient is TRUE, it's a transformGradient attribute
  ----------------------------------------------------------------------*/
void ParseTransformAttribute (Attribute attr, Element el, Document doc,
                              ThotBool delete_, ThotBool gradient)
{
  int                  length;
  float                scaleX, scaleY, x, y, a, b, c, d, e, f, angle;
  char                *text, *ptr;
  ThotBool             error;
#ifndef _GL
  PresentationValue    pval;
  PresentationContext  ctxt;
#endif /* _GL */

  length = TtaGetTextAttributeLength (attr) + 2;
  text = (char *)TtaGetMemory (length);
  if (text)
    {
      /* get the content of the transform attribute */
      TtaGiveTextAttributeValue (attr, text, &length);
      /* parse the attribute content */
      ptr = text;
      error = FALSE;
      while (*ptr != EOS && !error)
        {
          /* skip space characters */
          ptr = (char*)TtaSkipBlanks (ptr);
          if (!strncmp (ptr, "matrix", 6))
            {
              ptr += 6;
              ptr = (char*)TtaSkipBlanks (ptr);
              if (*ptr != '(')
                error = TRUE;
              else
                {
                  ptr++;
                  ptr = (char*)TtaSkipBlanks (ptr);
                  ptr = GetFloat (ptr, &a);
                  if (*ptr == ',')
                    {
                      ptr++;
                      ptr = (char*)TtaSkipBlanks (ptr);
                    }
                  ptr = GetFloat (ptr, &b);
                  if (*ptr == ',')
                    {
                      ptr++;
                      ptr = (char*)TtaSkipBlanks (ptr);
                    }
                  ptr = GetFloat (ptr, &c);
                  if (*ptr == ',')
                    {
                      ptr++;
                      ptr = (char*)TtaSkipBlanks (ptr);
                    }
                  ptr = GetFloat (ptr, &d);
                  if (*ptr == ',')
                    {
                      ptr++;
                      ptr = (char*)TtaSkipBlanks (ptr);
                    }
                  ptr = GetFloat (ptr, &e);
                  if (*ptr == ',')
                    {
                      ptr++;
                      ptr = (char*)TtaSkipBlanks (ptr);
                    }
                  ptr = GetFloat (ptr, &f);
                  if (*ptr != ')')
                    error = TRUE;
                  else
                    {
                      ptr++;
#ifdef _GL
		      if (gradient)
			TtaAppendGradientTransform (el, 
				     TtaNewTransformMatrix (a, b, c, d, e, f));
		      else
			TtaAppendTransform (el, 
					    TtaNewTransformMatrix (a, b, c,
								   d, e, f));
#else /* _GL */
                      pval.typed_data.value = 0;
                      pval.typed_data.unit = UNIT_PX;
                      pval.typed_data.real = FALSE;
                      pval.typed_data.mainValue = TRUE;
                      ctxt = TtaGetSpecificStyleContext (doc);
                      ctxt->cssSpecificity = 2000; /* this is not a CSS rule */
                      ctxt->destroy = delete_;
                      /****** process values a, b, c, d *****/
                      /* value e specifies an horizontal translation */
                      if (e != 0)
                        {
                          pval.typed_data.value = (int)e;
                          TtaSetStylePresentation (PRHorizPos, el, NULL,
                                                   ctxt, pval);
                        }
                      /* value f specifies a vertical translation */
                      if (f != 0)
                        {
                          pval.typed_data.value = (int)f;
                          TtaSetStylePresentation (PRVertPos, el, NULL,
                                                   ctxt, pval);
                        }
                      TtaFreeMemory (ctxt);
#endif /* _GL */
                    }
                }
            }
          else if (!strncmp (ptr, "translate", 9))
            {
              x = 0;  y = 0;
              ptr += 9;
              ptr = (char*)TtaSkipBlanks (ptr);
              if (*ptr != '(')
                error = TRUE;
              else
                {
                  ptr++;
                  ptr = (char*)TtaSkipBlanks (ptr);
                  ptr = GetFloat (ptr, &x);
#ifndef _GL
                  pval.typed_data.value = 0;
                  pval.typed_data.unit = UNIT_PX;
                  pval.typed_data.real = FALSE;
                  pval.typed_data.value = (int)x;
                  pval.typed_data.mainValue = TRUE;
                  ctxt = TtaGetSpecificStyleContext (doc);
                  ctxt->cssSpecificity = 2000;     /* this is not a CSS rule */
                  ctxt->destroy = delete_;
                  TtaSetStylePresentation (PRHorizPos, el, NULL, ctxt, pval);
#endif /* _GL */
                  /* New Version */
                  if (*ptr != ')')
                    {
                      if (*ptr == ',')
                        {
                          ptr++;
                          ptr = (char*)TtaSkipBlanks (ptr);
                        }
                      ptr = GetFloat (ptr, &y);
                    }
                  if (*ptr == ')')
                    ptr++;
                  else
                    error = TRUE;
#ifdef _GL
		  if (gradient)
                    TtaAppendGradientTransform (el,
					      TtaNewTransformTranslate (x, y));
		  else
                    TtaAppendTransform (el, TtaNewTransformTranslate (x, y));
#else /* _GL */
                  pval.typed_data.value = (int)y;
                  TtaSetStylePresentation (PRVertPos, el, NULL, ctxt, pval);
                  TtaFreeMemory (ctxt);
#endif /* _GL */
                }
            }
          else if (!strncmp (ptr, "scale", 5))
            {
              ptr += 5;
              ptr = (char*)TtaSkipBlanks (ptr);
              if (*ptr != '(')
                error = TRUE;
              else
                {
                  ptr++;
                  ptr = (char*)TtaSkipBlanks (ptr);
                  ptr = GetFloat (ptr, &scaleX);
                  if (*ptr == ')')
                    scaleY = scaleX;
                  else
                    {
                      if (*ptr == ',')
                        {
                          ptr++;
                          ptr = (char*)TtaSkipBlanks (ptr);
                        }
                      ptr = GetFloat (ptr, &scaleY);
                    }
                  if (*ptr == ')')
                    {
                      ptr++;
#ifdef _GL
		      if (gradient)
                        TtaAppendGradientTransform (el, 
                                        TtaNewTransformScale (scaleX, scaleY));
		      else
                       TtaAppendTransform (el, 
					   TtaNewTransformScale (scaleX, 
								 scaleY));
#endif /* _GL */
                    }
                  else
                    error = TRUE;
                }
            }
          else if (!strncmp (ptr, "rotate", 6))
            {
              ptr += 6;
              ptr = (char*)TtaSkipBlanks (ptr);
              if (*ptr != '(')
                error = TRUE;
              else
                {
                  ptr++;
                  ptr = (char*)TtaSkipBlanks (ptr);
                  ptr = GetFloat (ptr, &angle);
                  if (*ptr == ')')
                    {
                      x = 0;
                      y = 0;
                    }
                  else
                    {
                      if (*ptr == ',')
                        {
                          ptr++;
                          ptr = (char*)TtaSkipBlanks (ptr);
                        }
                      ptr = GetFloat (ptr, &x);
                      if (*ptr == ',')
                        {
                          ptr++;
                          ptr = (char*)TtaSkipBlanks (ptr);
                        }
                      ptr = GetFloat (ptr, &y);
                    }
                  if (*ptr == ')')
                    {
                      ptr++;
#ifdef _GL
		      if (gradient)
			TtaAppendGradientTransform (el, 
					 TtaNewTransformRotate (angle, x, y));
		      else
			TtaAppendTransform (el, 
					    TtaNewTransformRotate (angle, x, y));
#endif /* _GL */
                    }
                  else
                    error = TRUE;
                }
            }
          else if (!strncmp (ptr, "skewX", 5))
            {
              ptr += 5;
              ptr = (char*)TtaSkipBlanks (ptr);
              if (*ptr != '(')
                error = TRUE;
              else
                {
                  ptr++;
                  ptr = (char*)TtaSkipBlanks (ptr);
                  ptr = GetFloat (ptr, &x);
                  if (*ptr == ')')
                    {
                      ptr++;
#ifdef _GL
		      if (gradient)
			TtaAppendGradientTransform (el, 
						    TtaNewTransformSkewX (x));
		      else
			TtaAppendTransform (el, 
					    TtaNewTransformSkewX (x));
#endif /* _GL */
                    }
                  else
                    error = TRUE;
                }
            }
          else if (!strncmp (ptr, "skewY", 5))
            {
              ptr += 5;
              ptr = (char*)TtaSkipBlanks (ptr);
              if (*ptr != '(')
                error = TRUE;
              else
                {
                  ptr++;
                  ptr = (char*)TtaSkipBlanks (ptr);
                  ptr = GetFloat (ptr, &y);
                  if (*ptr == ')')
                    {
                      ptr++;
#ifdef _GL
		      if (gradient)
			TtaAppendGradientTransform (el, 
						    TtaNewTransformSkewY (y));
		      else
			TtaAppendTransform (el, 
					    TtaNewTransformSkewY (y));
#endif /* _GL */
                    }
                  else
                    error = TRUE;
                }
            }
          else
            /* unexpected token, ignore the rest */
            error = TRUE;

          if (!error)
            {
              /* skip spaces and the optional comma */
              ptr = (char*)TtaSkipBlanks (ptr);
              if (*ptr == ',')
                ptr++;
            }
        }
      TtaFreeMemory (text);
    }
}

/*----------------------------------------------------------------------
  ParsePathDataAttribute
  Parse the value of a path data attribute
  ----------------------------------------------------------------------*/
void *ParseValuesDataAttribute (Attribute attr, Element el, Document doc)
{
  int          length;
  float        x, y;
  char         *text, *ptr;
  void         *anim_seg = NULL;
     
  anim_seg = TtaNewAnimPath (doc);    
  /* get a buffer for reading the attribute value */
  length = TtaGetTextAttributeLength (attr) + 2;
  text = (char *)TtaGetMemory (length);
  if (text)
    {
      /* get the content of the data attribute */
      TtaGiveTextAttributeValue (attr, text, &length);
      /* parse the attribute content */
      ptr = text;
      while (*ptr != EOS)
        {
          ptr = (char*)TtaSkipBlanks (ptr);
          ptr = GetFloat (ptr, &x);
          if (*ptr == ';')
            {
              y = x;
            }
          else
            {
              ptr = (char*)TtaSkipBlanks (ptr);
              ptr = GetFloat (ptr, &y);
            }
          TtaAnimPathAddPoint (anim_seg, x, y);
          ptr++;
        }
      TtaFreeMemory (text);
    }
  return anim_seg;
}
/*----------------------------------------------------------------------
  ParseFromToDataAttribute
  Parse the value of a path data attribute
  ----------------------------------------------------------------------*/
void *ParseFromToDataAttribute (Attribute attrfrom, Attribute attrto,
                                Element el, Document doc)
{
  int          length;
  float         x, y;
  char         *text, *ptr;
  void         *anim_seg = NULL;
     
  anim_seg = TtaNewAnimPath (doc);
  /* get a buffer for reading the attribute value */
  length = TtaGetTextAttributeLength (attrfrom) + 2;
  text = (char *)TtaGetMemory (length);
  if (text)
    {
      /* get the content of the data attribute */
      TtaGiveTextAttributeValue (attrfrom, text, &length);
      /* parse the attribute content */
      ptr = text;
      ptr = (char*)TtaSkipBlanks (ptr);
      ptr = GetFloat (ptr, &x);
      ptr = GetFloat (ptr, &y);
      TtaAnimPathAddPoint (anim_seg, x, y);
      TtaFreeMemory (text);
    }

  /* get a buffer for reading the attribute value */
  length = TtaGetTextAttributeLength (attrto) + 2;
  text = (char *)TtaGetMemory (length);
  if (text)
    {
      /* get the content of the data attribute */
      TtaGiveTextAttributeValue (attrto, text, &length);
      /* parse the attribute content */
      ptr = text;
      ptr = (char*)TtaSkipBlanks (ptr);
      ptr = GetFloat (ptr, &x);
      ptr = GetFloat (ptr, &y);
      TtaFreeMemory (text);
      TtaAnimPathAddPoint (anim_seg, x, y);
    }
  return anim_seg;
}
/*----------------------------------------------------------------------
  ParsePathDataAttribute
  Parse the value of a path data attribute
  ----------------------------------------------------------------------*/
void *ParsePathDataAttribute (Attribute attr, Element el, Document doc, ThotBool IsDrawn)
{
  int          length, x, y, x1, y1, x2, y2, xcur, ycur, xinit, yinit,
    x2prev, y2prev, x1prev, y1prev, rx, ry, xAxisRotation,
    largeArcFlag, sweepFlag;
  Element      leaf;
  PathSegment  seg;
  ThotBool     relative, newSubpath, error;
  char         *text, *ptr;
  char         command, prevCommand;
  void         *anim_seg = NULL;

  /* create (or get) the Graphics leaf */
  if (IsDrawn)
    {
      leaf = CreateGraphicalLeaf (EOS, el, doc);
      if (leaf == NULL)
        return NULL;
      /* if the leaf element is a graphic element, turn it into a path */
      TtaAppendPathSeg (leaf, NULL, doc);
    }
  else
    {
      leaf = el;
      anim_seg = TtaNewAnimPath (doc);
    }
   
  /* get a buffer for reading the attribute value */
  length = TtaGetTextAttributeLength (attr) + 2;
  text = (char *)TtaGetMemory (length);
  if (text)
    {
      /* get the content of the path data attribute */
      TtaGiveTextAttributeValue (attr, text, &length);
      /* parse the attribute content */
      ptr = text;
      ptr = (char*)TtaSkipBlanks (ptr);
      command = *ptr;
      ptr++;
      prevCommand = EOS;
      xcur = 0;
      ycur = 0;
      xinit = 0;
      yinit = 0;
      x1prev = 0;
      y1prev = 0;
      x2prev = 0;
      y2prev = 0;
      newSubpath = FALSE;
      error = FALSE;
      while (command != EOS && !error)
        {
          relative = TRUE;
          ptr = (char*)TtaSkipBlanks (ptr);
          switch (command)
            {
            case 'M':
              relative = FALSE;
            case 'm':
              /* moveto */
              if (relative)
                {
                  x = xcur;
                  y = ycur;
                }
              ptr = SVG_GetNumber (ptr, &xcur, &error);
              if (!error)
                ptr = SVG_GetNumber (ptr, &ycur, &error);
              if (!error)
                {
                  if (relative)
                    {
                      xcur += x;
                      ycur += y;
                    }
                  xinit = xcur;
                  yinit = ycur;
                  newSubpath = TRUE;
                }
              break;

            case 'Z':
              relative = FALSE;
            case 'z':
              /* close path */
              /* draw a line from (xcur, ycur) to (xinit, yinit) */
              if(!(xcur == xinit && ycur == yinit))
                {
                  seg = TtaNewPathSegLine (xcur, ycur, xinit, yinit,
                                           newSubpath);
                  if (IsDrawn)
                    TtaAppendPathSeg (leaf, seg, doc);
                  else
                    TtaAppendPathSegToAnim (anim_seg, seg, doc);
                  
                  newSubpath = FALSE;
                  xcur = xinit;
                  ycur = yinit;
                }
              break;

            case 'L':
              relative = FALSE;
            case 'l':
              /* lineto */
              ptr = SVG_GetNumber (ptr, &x, &error);
              if (!error)
                ptr = SVG_GetNumber (ptr, &y, &error);
              if (!error)
                {
                  if (relative)
                    {
                      x += xcur;
                      y += ycur;
                    }
                  /* draw a line from (xcur, ycur) to (x, y) */
                  seg = TtaNewPathSegLine (xcur, ycur, x, y, newSubpath);
                  if (IsDrawn)
                    TtaAppendPathSeg (leaf, seg, doc);
                  else
                    TtaAppendPathSegToAnim (anim_seg, seg, doc);
                  newSubpath = FALSE;
                  xcur = x;
                  ycur = y;
                }
              break;

            case 'H':
              relative = FALSE;
            case 'h':
              /* horizontal lineto */
              ptr = SVG_GetNumber (ptr, &x, &error);
              if (!error)
                {
                  if (relative)
                    x += xcur;
                  /* draw a line from (xcur, ycur) to (x, ycur) */
                  seg = TtaNewPathSegLine (xcur, ycur, x, ycur, newSubpath);
                  if (IsDrawn)
                    TtaAppendPathSeg (leaf, seg, doc);
                  else
                    TtaAppendPathSegToAnim (anim_seg, seg, doc);
                  newSubpath = FALSE;
                  xcur = x;
                }
              break;

            case 'V':
              relative = FALSE;
            case 'v':
              /* vertical lineto */
              ptr = SVG_GetNumber (ptr, &y, &error);
              if (!error)
                {
                  if (relative)
                    y += ycur;
                  /* draw a line from (xcur, ycur) to (xcur, y) */
                  seg = TtaNewPathSegLine (xcur, ycur, xcur, y, newSubpath);
                  if (IsDrawn)
                    TtaAppendPathSeg (leaf, seg, doc);
                  else
                    TtaAppendPathSegToAnim (anim_seg, seg, doc);
                  newSubpath = FALSE;
                  ycur = y;
                }
              break;

            case 'C':
              relative = FALSE;
            case 'c':
              /* curveto */
              ptr = SVG_GetNumber (ptr, &x1, &error);
              if (!error)
                ptr = SVG_GetNumber (ptr, &y1, &error);
              if (!error)
                ptr = SVG_GetNumber (ptr, &x2, &error);
              if (!error)
                ptr = SVG_GetNumber (ptr, &y2, &error);
              if (!error)
                ptr = SVG_GetNumber (ptr, &x, &error);
              if (!error)
                ptr = SVG_GetNumber (ptr, &y, &error);
              if (!error)
                {
                  if (relative)
                    {
                      x1 += xcur;
                      y1 += ycur;
                      x2 += xcur;
                      y2 += ycur;
                      x += xcur;
                      y += ycur;
                    }
                  /* draw a cubic Bezier curve from (xcur, ycur) to (x, y) using
                     (x1, y1) as the control point at the beginning of the curve
                     and (x2, y2) as the control point at the end of the curve*/
                  seg = TtaNewPathSegCubic (xcur, ycur, x, y, x1, y1, x2, y2,
                                            newSubpath);
                  if (IsDrawn)
                    TtaAppendPathSeg (leaf, seg, doc);
                  else
                    TtaAppendPathSegToAnim (anim_seg, seg, doc);
                  newSubpath = FALSE;
                  xcur = x;
                  ycur = y;
                  x2prev = x2;
                  y2prev = y2;
                }
              break;

            case 'S':
              relative = FALSE;
            case 's':
              /* smooth curveto */
              ptr = SVG_GetNumber (ptr, &x2, &error);
              if (!error)
                ptr = SVG_GetNumber (ptr, &y2, &error);
              if (!error)
                ptr = SVG_GetNumber (ptr, &x, &error);
              if (!error)
                ptr = SVG_GetNumber (ptr, &y, &error);
              if (!error)
                {
                  if (relative)
                    {
                      x2 += xcur;
                      y2 += ycur;
                      x += xcur;
                      y += ycur;
                    }
                  /* compute the first control point */
                  if (prevCommand == 'C' || prevCommand == 'c' || 
                      prevCommand == 'S' || prevCommand == 's')
                    {
                      x1 = 2*xcur - x2prev;
                      y1 = 2*ycur - y2prev;
                    }
                  else
                    {
                      x1 = xcur;
                      y1 = ycur;
                    }
                  /* draw a cubic Bezier curve from (xcur, ycur) to (x, y) using
                     (x1, y1) as the control point at the beginning of the curve
                     and (x2, y2) as the control point at the end of the curve*/
                  seg = TtaNewPathSegCubic (xcur, ycur, x, y, x1, y1, x2, y2,
                                            newSubpath);
                  if (IsDrawn)
                    TtaAppendPathSeg (leaf, seg, doc);
                  else
                    TtaAppendPathSegToAnim (anim_seg, seg, doc);
                  newSubpath = FALSE;
                  xcur = x;
                  ycur = y;
                  x2prev = x2;
                  y2prev = y2;
                }
              break;

            case 'Q':
              relative = FALSE;
            case 'q':
              /* quadratic Bezier curveto */
              ptr = SVG_GetNumber (ptr, &x1, &error);
              if (!error)
                ptr = SVG_GetNumber (ptr, &y1, &error);
              if (!error)
                ptr = SVG_GetNumber (ptr, &x, &error);
              if (!error)
                ptr = SVG_GetNumber (ptr, &y, &error);
              if (!error)
                {
                  if (relative)
                    {
                      x1 += xcur;
                      y1 += ycur;
                      x += xcur;
                      y += ycur;
                    }
                  /* draw a quadratic Bezier curve from (xcur, ycur) to (x, y)
                     using (x1, y1) as the control point */
                  seg = TtaNewPathSegQuadratic (xcur, ycur, x, y, x1, y1,
                                                newSubpath);
                  if (IsDrawn)
                    TtaAppendPathSeg (leaf, seg, doc);
                  else
                    TtaAppendPathSegToAnim (anim_seg, seg, doc);
                  newSubpath = FALSE;
                  xcur = x;
                  ycur = y;
                  x1prev = x1;
                  y1prev = y1;
                }
              break;

            case 'T':
              relative = FALSE;
            case 't':
              /* smooth quadratic Bezier curveto */
              ptr = SVG_GetNumber (ptr, &x, &error);
              if (!error)
                ptr = SVG_GetNumber (ptr, &y, &error);
              if (!error)
                {
                  if (relative)
                    {
                      x += xcur;
                      y += ycur;
                    }
                  /* compute the control point */
                  if (prevCommand == 'Q' || prevCommand == 'q' || 
                      prevCommand == 'T' || prevCommand == 't')
                    {
                      x1 = xcur + (xcur - x1prev);
                      y1 = ycur + (ycur - y1prev);
                    }
                  else
                    {
                      x1 = xcur;
                      y1 = ycur;
                    }
                  /* draw a quadratic Bezier curve from (xcur, ycur) to (x, y)
                     using (x1, y1) as the control point */
                  seg = TtaNewPathSegQuadratic (xcur, ycur, x, y, x1, y1,
                                                newSubpath);
                  if (IsDrawn)
                    TtaAppendPathSeg (leaf, seg, doc);
                  else
                    TtaAppendPathSegToAnim (anim_seg, seg, doc);
                  newSubpath = FALSE;
                  xcur = x;
                  ycur = y;
                  x1prev = x1;
                  y1prev = y1;
                }
              break;

            case 'A':
              relative = FALSE;
            case 'a':
              /* elliptical arc */
              ptr = SVG_GetNumber (ptr, &rx, &error);    /* must be non-negative */
              if (rx < 0)
                error = TRUE;
              if (!error)
                ptr = SVG_GetNumber (ptr, &ry, &error);  /* must be non-negative */
              if (ry < 0)
                error = TRUE;
              if (!error)
                ptr = SVG_GetNumber (ptr, &xAxisRotation, &error);
              if (!error)
                ptr = SVG_GetNumber (ptr, &largeArcFlag, &error);  
              /* must be "0" or "1" */
              if (largeArcFlag != 0 && largeArcFlag != 1)
                error = TRUE;
              if (!error)
                ptr = SVG_GetNumber (ptr, &sweepFlag, &error);
              /* must be "0" or "1" */
              if (sweepFlag != 0 && sweepFlag != 1)
                error = TRUE;
              if (!error)
                ptr = SVG_GetNumber (ptr, &x, &error);
              if (!error)
                ptr = SVG_GetNumber (ptr, &y, &error);
              if (!error)
                {
                  if (relative)
                    {
                      x += xcur;
                      y += ycur;
                    }
                  /* draw an elliptical arc from (xcur, ycur) to (x, y) */
                  seg = TtaNewPathSegArc (xcur, ycur, x, y, rx, ry,
                                          xAxisRotation, largeArcFlag == 1,
                                          sweepFlag == 1, newSubpath);
                  if (IsDrawn)
                    TtaAppendPathSeg (leaf, seg, doc);
                  else
                    TtaAppendPathSegToAnim (anim_seg, seg, doc);
                  newSubpath = FALSE;
                  xcur = x;
                  ycur = y;
                }
              break;

            default:
              /* unknown command. error. stop parsing. */
              error = TRUE;
              command = EOS;
              break;
            }

          if (command != EOS && !error)
            {
              prevCommand = command;
              if (command == 'Z' || command == 'z')
                /* don't expect coordinates after a close path command, only
                   a new command or end of the string */
                {
                  ptr = (char*)TtaSkipBlanks (ptr);	     
                  command = *ptr;
                  ptr++;
                }
              else
                if (*ptr != '+' && *ptr != '-' &&
                    (*ptr < '0' || *ptr > '9'))
                  /* no more coordinates. New command */
                  {
                    command = *ptr;
                    ptr++;
                  }
                else
                  /* more coordinates */
                  /* if it's after a moveto command, interpret additional
                     coordinates as if it was a lineto command */
                  if (command == 'M')
                    command = 'L';
                  else if (command == 'm')
                    command = 'l';
            }
        }
      TtaFreeMemory (text);
    }
  if (IsDrawn)
    return NULL;
  else
    return anim_seg;
}

/*----------------------------------------------------------------------
  ParseNumberPercentAttribute : 
  Parse the value of a <number> or <percentage> attribute
  ----------------------------------------------------------------------*/
float ParseNumberPercentAttribute (Attribute attr)
{
  float                value;
  int                  length;
  char                *text, *ptr;
  PresentationValue    pval;

  value = 0;
  length = TtaGetTextAttributeLength (attr) + 2;
  text = (char *)TtaGetMemory (length);
  if (text != NULL)
    {
      TtaGiveTextAttributeValue (attr, text, &length);
      /* parse the attribute value */
      ptr = text;
      ptr = (char*)TtaSkipBlanks (ptr);
      ptr = ParseCSSUnit (ptr, &pval);
      TtaFreeMemory (text);
      if (pval.typed_data.unit == UNIT_BOX)
	{
	  if (pval.typed_data.real)
	    value = (float) pval.typed_data.value/1000;
	  else
	    value = (float) pval.typed_data.value;
	}
      else if (pval.typed_data.unit == UNIT_PERCENT)
	/* it's a percentage */
	value = (float) pval.typed_data.value/100;
    }
  return value;
}

/*----------------------------------------------------------------------
  SVGAttributeComplete
  The XML parser has read attribute attr for element el in document doc.
  ----------------------------------------------------------------------*/
void SVGAttributeComplete (Attribute attr, Element el, Document doc)
{
  AttributeType	       attrType, attrType1;
  Attribute            intAttr;
  ElementType          elType;
  Element	       leaf;
  int		       attrKind, method, value;
  ThotBool	       closed;
  float                offset;

  TtaGiveAttributeType (attr, &attrType, &attrKind);
  switch (attrType.AttrTypeNum)
    {
    case SVG_ATTR_opacity_:
    case SVG_ATTR_stroke_opacity:
    case SVG_ATTR_fill_opacity:
    case SVG_ATTR_stop_opacity:
    case SVG_ATTR_fill_rule:
      ParseCSSequivAttribute (attrType.AttrTypeNum, attr, el, doc, FALSE);
      break;
    case SVG_ATTR_height_:
    case SVG_ATTR_width_:
      ParseWidthHeightAttribute (attr, el, doc, FALSE);
      break;
    case SVG_ATTR_r:
      elType = TtaGetElementType (el);
      if (elType.ElTypeNum == SVG_EL_radialGradient)
	TtaSetRadialGradientRadius (ParseNumberPercentAttribute (attr), el);
      else
	ParseWidthHeightAttribute (attr, el, doc, FALSE);
      break;
    case SVG_ATTR_font_family:
    case SVG_ATTR_font_size:
    case SVG_ATTR_font_style:
    case SVG_ATTR_font_variant:
    case SVG_ATTR_font_weight:
    case SVG_ATTR_text_decoration:
    case SVG_ATTR_color:
    case SVG_ATTR_stop_color:
    case SVG_ATTR_fill:
    case SVG_ATTR_stroke:
    case SVG_ATTR_stroke_width:
      ParseCSSequivAttribute (attrType.AttrTypeNum, attr, el, doc, FALSE);
      break;
    case SVG_ATTR_transform:
      ParseTransformAttribute (attr, el, doc, FALSE, FALSE);
      break;
    case SVG_ATTR_points:
      ParsePointsAttribute (attr, el, doc);
      break;
    case SVG_ATTR_x:
    case SVG_ATTR_y:
    case SVG_ATTR_dx:
    case SVG_ATTR_dy:
      ParseCoordAttribute (attr, el, doc);
      break;
      
    case SVG_ATTR_cx:
      elType = TtaGetElementType (el);
      if (elType.ElTypeNum == SVG_EL_radialGradient)
	TtaSetRadialGradientcx (ParseNumberPercentAttribute (attr), el);
      else
	ParseCoordAttribute (attr, el, doc);
      break;
    case SVG_ATTR_cy:
      elType = TtaGetElementType (el);
      if (elType.ElTypeNum == SVG_EL_radialGradient)
	TtaSetRadialGradientcy (ParseNumberPercentAttribute (attr), el);
      else
	ParseCoordAttribute (attr, el, doc);
      break;
    case SVG_ATTR_fx:
      elType = TtaGetElementType (el);
      if (elType.ElTypeNum == SVG_EL_radialGradient)
	TtaSetRadialGradientfx (ParseNumberPercentAttribute (attr), el);
      else
	ParseCoordAttribute (attr, el, doc);
      break;
    case SVG_ATTR_fy:
      elType = TtaGetElementType (el);
      if (elType.ElTypeNum == SVG_EL_radialGradient)
	TtaSetRadialGradientfy (ParseNumberPercentAttribute (attr), el);
      else
	ParseCoordAttribute (attr, el, doc);
      break;
    case SVG_ATTR_x1:
      elType = TtaGetElementType (el);
      if (elType.ElTypeNum == SVG_EL_linearGradient)
        TtaSetLinearGradientx1 (ParseNumberPercentAttribute (attr), el);
      //else
      //  ParseCoordAttribute (attr, el, doc);
      break;
    case SVG_ATTR_y1:
      elType = TtaGetElementType (el);
      if (elType.ElTypeNum == SVG_EL_linearGradient)
        TtaSetLinearGradienty1 (ParseNumberPercentAttribute (attr), el);
      //else
      //  ParseCoordAttribute (attr, el, doc);
      break;
    case SVG_ATTR_x2:
      elType = TtaGetElementType (el);
      if (elType.ElTypeNum == SVG_EL_linearGradient)
        TtaSetLinearGradientx2 (ParseNumberPercentAttribute (attr), el);
      //else
      //  ParseCoordAttribute (attr, el, doc);
      break;
    case SVG_ATTR_y2:
      elType = TtaGetElementType (el);
      if (elType.ElTypeNum == SVG_EL_linearGradient)
        TtaSetLinearGradienty2 (ParseNumberPercentAttribute (attr), el);
      //else
      //  ParseCoordAttribute (attr, el, doc);
      break;
	
    case SVG_ATTR_rx:
    case SVG_ATTR_ry:
      elType = TtaGetElementType (el);
      if (elType.ElTypeNum == SVG_EL_rect)
        /* attribute rx or ry for a rect element.
           create the GRAPHICS_UNIT child to put the corresponding
           specific presentation rule on it */
        leaf = CreateGraphicLeaf (el, doc, &closed);	   
      ParseWidthHeightAttribute (attr, el, doc, FALSE);
      break;
    case SVG_ATTR_baseline_shift:
      ParseBaselineShiftAttribute (attr, el, doc, FALSE);
      break;
    case SVG_ATTR_text_anchor:
      SetTextAnchor (attr, el, doc, FALSE);
      break;
    case SVG_ATTR_d:
      ParsePathDataAttribute (attr, el, doc, TRUE);
      break;
    case SVG_ATTR_Language:
      if (el == TtaGetRootElement (doc))
        /* it's the lang attribute on the root element */
        /* set the RealLang attribute */
        {
          attrType1.AttrSSchema = attrType.AttrSSchema ;
          attrType1.AttrTypeNum = SVG_ATTR_RealLang;
          if (!TtaGetAttribute (el, attrType1))
            /* it's not present. Add it */
            {
              intAttr = TtaNewAttribute (attrType1);
              TtaAttachAttribute (el, intAttr, doc);
              TtaSetAttributeValue (intAttr, SVG_ATTR_RealLang_VAL_Yes_,
                                    el, doc);
            }
        }
      break;
    case SVG_ATTR_spreadMethod:
      /* get the value of attribute spreadMethod */
      method = TtaGetAttributeValue (attr);
      TtaSetGradientSpreadMethod (method, el);
      break;
    case SVG_ATTR_gradientUnits:
      elType = TtaGetElementType (el);
      if (elType.ElTypeNum == SVG_EL_linearGradient ||
	  elType.ElTypeNum == SVG_EL_radialGradient)
	{
	  /* get the value of attribute gradientUnits */
	  value = TtaGetAttributeValue (attr);
	  TtaSetGradientUnits ((value == SVG_ATTR_gradientUnits_VAL_userSpaceOnUse), el);
	}
      break;
    case SVG_ATTR_gradientTransform:
      ParseTransformAttribute (attr, el, doc, FALSE, TRUE);
      break;
    case SVG_ATTR_offset:
      /* parse <number> or <percentage> */
      offset = ParseNumberPercentAttribute (attr);
      TtaSetGradientStopOffset (offset, el);
      break;
    case SVG_ATTR_id:
      elType = TtaGetElementType (el);
      if (elType.ElTypeNum != SVG_EL_use_ && elType.ElTypeNum != SVG_EL_marker)
	CheckUniqueName (el, doc, attr, attrType);
      break;

    case SVG_ATTR_marker_start:
    case SVG_ATTR_marker_mid:
    case SVG_ATTR_marker_end:
      ParseCSSequivAttribute (attrType.AttrTypeNum, attr, el, doc, FALSE);
      break;

    default:
      break;
    }
}
