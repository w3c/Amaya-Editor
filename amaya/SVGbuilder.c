/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1998-2002
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 
/*
 *
 * SVGbuilder
 *
 * Authors: V. Quint
 *          I. Vatton
 */
 
#define THOT_EXPORT extern
#include "amaya.h"
#include "css.h"
#include "SVG.h"
#include "HTML.h"
#include "XLink.h"
#include "parser.h"
#include "registry.h"
#include "style.h"

/* mapping table of attribute values */

static AttrValueMapping SVGAttrValueMappingTable[] =
{ 
   {SVG_ATTR_xml_space, "default", SVG_ATTR_xml_space_VAL_xml_space_default},
   {SVG_ATTR_xml_space, "preserve", SVG_ATTR_xml_space_VAL_xml_space_preserve},

   {SVG_ATTR_externalResourcesRequired, "false", SVG_ATTR_externalResourcesRequired_VAL_false},
   {SVG_ATTR_externalResourcesRequired, "true", SVG_ATTR_externalResourcesRequired_VAL_true},

   {SVG_ATTR_font_style, "inherit", SVG_ATTR_font_style_VAL_inherit},
   {SVG_ATTR_font_style, "italic", SVG_ATTR_font_style_VAL_italic},
   {SVG_ATTR_font_style, "normal", SVG_ATTR_font_style_VAL_normal_},
   {SVG_ATTR_font_style, "oblique", SVG_ATTR_font_style_VAL_oblique_},

   {SVG_ATTR_font_variant, "inherit", SVG_ATTR_font_variant_VAL_inherit},
   {SVG_ATTR_font_variant, "normal", SVG_ATTR_font_variant_VAL_normal_},
   {SVG_ATTR_font_variant, "small-caps", SVG_ATTR_font_variant_VAL_small_caps},

   {SVG_ATTR_font_weight, "100", SVG_ATTR_font_weight_VAL_w100},
   {SVG_ATTR_font_weight, "200", SVG_ATTR_font_weight_VAL_w200},
   {SVG_ATTR_font_weight, "300", SVG_ATTR_font_weight_VAL_w300},
   {SVG_ATTR_font_weight, "400", SVG_ATTR_font_weight_VAL_w400},
   {SVG_ATTR_font_weight, "500", SVG_ATTR_font_weight_VAL_w500},
   {SVG_ATTR_font_weight, "600", SVG_ATTR_font_weight_VAL_w600},
   {SVG_ATTR_font_weight, "700", SVG_ATTR_font_weight_VAL_w700},
   {SVG_ATTR_font_weight, "800", SVG_ATTR_font_weight_VAL_w800},
   {SVG_ATTR_font_weight, "900", SVG_ATTR_font_weight_VAL_w900},
   {SVG_ATTR_font_weight, "bold", SVG_ATTR_font_weight_VAL_bold_},
   {SVG_ATTR_font_weight, "bolder", SVG_ATTR_font_weight_VAL_bolder},
   {SVG_ATTR_font_weight, "inherit", SVG_ATTR_font_weight_VAL_inherit},
   {SVG_ATTR_font_weight, "lighter", SVG_ATTR_font_weight_VAL_lighter},
   {SVG_ATTR_font_weight, "normal", SVG_ATTR_font_weight_VAL_normal_},

   {SVG_ATTR_direction_, "ltr", SVG_ATTR_direction__VAL_ltr_},
   {SVG_ATTR_direction_, "rtl", SVG_ATTR_direction__VAL_rtl_},
   {SVG_ATTR_direction_, "inherit", SVG_ATTR_direction__VAL_inherit},

   {SVG_ATTR_unicode_bidi, "normal", SVG_ATTR_unicode_bidi_VAL_normal_},
   {SVG_ATTR_unicode_bidi, "embed", SVG_ATTR_unicode_bidi_VAL_embed_},
   {SVG_ATTR_unicode_bidi, "bidi-override", SVG_ATTR_unicode_bidi_VAL_bidi_override},
   {SVG_ATTR_unicode_bidi, "inherit", SVG_ATTR_unicode_bidi_VAL_inherit},

   {0, "", 0}			/* Last entry. Mandatory */
};

#define MaxMsgLength 200

#include "css_f.h"
#include "fetchXMLname_f.h"
#include "html2thot_f.h"
#include "styleparser_f.h"
#include "Xml2thot_f.h"

/*----------------------------------------------------------------------
   SVGGetDTDName
   Return in DTDname the name of the DTD to be used for parsing the
   content of element named elementName.
   This element type appear with an 'X' in the ElemMappingTable.
  ----------------------------------------------------------------------*/
void      SVGGetDTDName (char *DTDname, char *elementName)
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
void   MapSVGAttribute (char *attrName, AttributeType *attrType,
			    char* elementName, ThotBool *level, Document doc)
{
  attrType->AttrSSchema = GetSVGSSchema (doc);
  MapXMLAttribute (SVG_TYPE, attrName, elementName, level, doc, &(attrType->AttrTypeNum));
}

/*----------------------------------------------------------------------
   MapSVGAttributeValue
   Search in the Attribute Value Mapping Table the entry for the attribute
   ThotAtt and its value AttrVal. Returns the corresponding Thot value.
  ----------------------------------------------------------------------*/
void    MapSVGAttributeValue (char* AttrVal, AttributeType attrType, int *value)
{
   int                 i;

   *value = 0;
   i = 0;
   while (SVGAttrValueMappingTable[i].ThotAttr != attrType.AttrTypeNum &&
	  SVGAttrValueMappingTable[i].ThotAttr != 0)
     i++;
   if (SVGAttrValueMappingTable[i].ThotAttr == attrType.AttrTypeNum)
     do
       if (!strcmp (SVGAttrValueMappingTable[i].XMLattrValue, AttrVal))
	 *value = SVGAttrValueMappingTable[i].ThotAttrValue;
       else
	 i++;
     while (*value == 0 &&
	    SVGAttrValueMappingTable[i].ThotAttr == attrType.AttrTypeNum);
}

/*----------------------------------------------------------------------
   MapSVGEntity
   Search that entity in the entity table and return the corresponding value.
  ----------------------------------------------------------------------*/
void   MapSVGEntity (char *entityName, char *entityValue, char *script)
{
  entityValue[0] = EOS;
  *script = EOS;
}

/*----------------------------------------------------------------------
   SVGEntityCreated
   A SVG entity has been created by the XML parser.
  ----------------------------------------------------------------------*/
void    SVGEntityCreated (unsigned char *entityValue, Language lang,
			      char *entityName, Document doc)
{
}

/*----------------------------------------------------------------------
   ParseCSSequivAttribute
   Create or update a specific presentation rule for element el that reflects
   the value of attribute attr, which is equivalent to a CSS property (fill,
   stroke, stroke-width, font-family, font-size, font-style, font-variant,
   font-weight).
  ----------------------------------------------------------------------*/
void   ParseCSSequivAttribute (int attrType, Attribute attr, Element el,
			       Document doc, ThotBool delete)
{
#define buflen 200
  char               css_command[buflen+20];
  int                length, val;
  char               *text;

  text = NULL;
  /* get the value of the attribute */
  if (attrType == SVG_ATTR_font_style ||
      attrType == SVG_ATTR_font_variant ||
      attrType == SVG_ATTR_font_weight)
    /* enumerated value */
    val = TtaGetAttributeValue (attr);
  else
    /* the attribute value is a character string */
    {
      length = TtaGetTextAttributeLength (attr) + 2;
      text = TtaGetMemory (length);
      if (text != NULL)
	TtaGiveTextAttributeValue (attr, text, &length);
    }

  /* builds the equivalent CSS rule */
  switch (attrType)
    {
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
	case SVG_ATTR_font_weight_VAL_bolder:
	  sprintf (css_command, "font-weight: bolder");
	case SVG_ATTR_font_weight_VAL_lighter:
	  sprintf (css_command, "font-weight: lighter");
	case SVG_ATTR_font_weight_VAL_w100:
	  sprintf (css_command, "font-weight: 100");
	case SVG_ATTR_font_weight_VAL_w200:
	  sprintf (css_command, "font-weight: 200");
	case SVG_ATTR_font_weight_VAL_w300:
	  sprintf (css_command, "font-weight: 300");
	case SVG_ATTR_font_weight_VAL_w400:
	  sprintf (css_command, "font-weight: 400");
	case SVG_ATTR_font_weight_VAL_w500:
	  sprintf (css_command, "font-weight: 500");
	case SVG_ATTR_font_weight_VAL_w600:
	  sprintf (css_command, "font-weight: 600");
	case SVG_ATTR_font_weight_VAL_w700:
	  sprintf (css_command, "font-weight: 700");
	case SVG_ATTR_font_weight_VAL_w800:
	  sprintf (css_command, "font-weight: 800");
	case SVG_ATTR_font_weight_VAL_w900:
	  sprintf (css_command, "font-weight: 900");
	case SVG_ATTR_font_weight_VAL_inherit:
	  sprintf (css_command, "font-weight: inherit");
	  break;
	}
    case SVG_ATTR_text_decoration:
      sprintf (css_command, "text-decoration: %s", text);
      break;
      break;
    }

  /* parse the equivalent CSS rule */
  ParseHTMLSpecificStyle (el, css_command, doc, 0, delete);
  if (text)
    TtaFreeMemory (text);
}

/*----------------------------------------------------------------------
   CreateGraphicalLeaf
   Create a GRAPHICS_UNIT element as the last child of element el if it
   does not exist yet.
   Return that GRAPHICS_UNIT element.
  ----------------------------------------------------------------------*/
static Element CreateGraphicalLeaf (char shape, Element el, Document doc)
{
  ElementType	   elType;
  Element	   leaf, child;
  char             oldShape;

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
	  if (oldShape == EOS || oldShape != shape)
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
static Element  CreateGraphicLeaf (Element el, Document doc, ThotBool *closed)
{
   ElementType elType;
   Element     leaf;

   leaf = NULL;
   *closed = FALSE;
   elType = TtaGetElementType (el);
   switch (elType.ElTypeNum)
     {
     case SVG_EL_rect:
       leaf = CreateGraphicalLeaf ('C', el, doc);
       *closed = TRUE;
       break;

     case SVG_EL_circle:
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
static void     CreateEnclosingElement (Element el, ElementType elType, Document doc)
{
   Element	new, prev, next, child;

   new = TtaNewElement (doc, elType);
   TtaInsertSibling (new, el, TRUE, doc);
   next = el;
   TtaNextSibling (&next);
   TtaRemoveTree (el, doc);
   TtaInsertFirstChild (&el, new, doc);
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
void             SetGraphicDepths (Document doc, Element el)
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
  ctxt->cssSpecificity = 0;
  ctxt->destroy = FALSE;
  pval.typed_data.value = 0; /*TtaGetDepth (el, doc, 1)*/
  pval.typed_data.unit = STYLE_UNIT_REL;
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
   CopyUseContent
   Copy the subtree pointed by the href URI as a subtree of element el,
   which is od type use.
  ----------------------------------------------------------------------*/
void  CopyUseContent (Element el, Document doc, char *href)
{
  Element              source, curEl, copy, child, elFound;
  ElementType          elType;
  Attribute            attr;
  AttributeType        attrType;
  SearchDomain         direction;
  int                  i, length, oldStructureChecking;
  char *              id;

  /* look for an element with an id attribute with the same value as the
     href attribute */
  elType = TtaGetElementType (el);
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
		id = TtaGetMemory (length + 1);
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
  if (source)
    /* the element to be copied in the use element has been found */
    {
      /* remove the old copy if there is one */
      child = TtaGetFirstChild (el);
      while (child)
	  if (TtaIsTranscludedElement (child))
	    /* that's the old copy. remove it */
	    {
	      TtaRemoveTree (child, doc);
	      child = NULL;
	    }
	  else
	    TtaNextSibling (&child);

      /* make a copy of the source element */
      copy = TtaNewTranscludedElement (doc, source);
      /* remove the id attribute from the copy */
      attr = TtaGetAttribute (copy, attrType);
      /* insert it as the last child of the use element */
      if (attr)
	TtaRemoveAttribute (copy, attr, doc);
      child = TtaGetLastChild (el);
      oldStructureChecking = TtaGetStructureChecking (doc);
      TtaSetStructureChecking (0, doc);
      if (child)
	TtaInsertSibling (copy, child, FALSE, doc);
      else
	TtaInsertFirstChild (&copy, el, doc);
      TtaSetStructureChecking (oldStructureChecking, doc);
    }
}

/*----------------------------------------------------------------------
   CheckHrefAttr
   If element el has a href attribute from the XLink namespace, replace
   that attribute by a href attribute in the SVG namespace with the
   same value.
  ----------------------------------------------------------------------*/
static void CheckHrefAttr (Element el, Document doc)
{
  SSchema              XLinkSSchema;
  AttributeType        attrType;
  Attribute            attr;
  int                  length;
  char                 *href;

  XLinkSSchema = GetXLinkSSchema (doc);
  if (XLinkSSchema)
    /* the XLink namespace is used in that document */
    {
      attrType.AttrSSchema = XLinkSSchema;
      attrType.AttrTypeNum = XLink_ATTR_href_;
      attr = TtaGetAttribute (el, attrType);
      if (attr)
	/* it has an XLink href attribute */
	{
	  /* get its value */
	  length = TtaGetTextAttributeLength (attr);
	  href = TtaGetMemory (length + 1);
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
}

/*----------------------------------------------------------------------
   EvaluateFeatures
   Evaluates the requiredFeatures attribute
  ----------------------------------------------------------------------*/
static ThotBool EvaluateFeatures (Attribute attr)
{
  int          length;
  char         *text, *ptr;
  ThotBool     ok, supported;

  ok = False;
  length = TtaGetTextAttributeLength (attr);
  if (length > 0)
    {
      text = TtaGetMemory (length + 2);
      if (text)
	{
	  TtaGiveTextAttributeValue (attr, text, &length);
	  ptr = text;
	  ptr = TtaSkipBlanks (ptr);
	  supported = True;
	  while (*ptr != EOS && supported)
	    {
	      /* only SVG static is supported in this version of Amaya */
	      if (strcmp (ptr, "org.w3c.svg") &&
		  strcmp (ptr, "org.w3c.svg.static"))
		supported = False;
	      if (supported)
		/* this feature is supported. Check to the next one */
		{
		  while (*ptr != EOS && *ptr != SPACE && *ptr != BSPACE &&
			 *ptr != EOL && *ptr != TAB && *ptr != CR)
		    ptr++;
		  ptr = TtaSkipBlanks (ptr);
		}
	    }
	  ok = supported;
	  TtaFreeMemory (text);
	}
    }
  return ok;
}

/*----------------------------------------------------------------------
   EvaluateExtensions
   Evaluates the requiredExtensions attribute
  ----------------------------------------------------------------------*/
static ThotBool EvaluateExtensions (Attribute attr)
{
  /* this version of Amaya does not support any SVG extension */
  return False;
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
      text = TtaGetMemory (length + 2);
      if (text)
	{
	  /* get the list of languages accepted by the user */
	  acceptLang = TtaGetEnvString ("ACCEPT_LANGUAGES");
	  if (!acceptLang)
	    return (ok);
	  acceptLang = TtaSkipBlanks (acceptLang);
	  if (*acceptLang != EOS)
	    /* the list of user's preferred languages is not empty */
	    {
	      TtaGiveTextAttributeValue (attr, text, &length);
	      ptr = text;
	      ptr = TtaSkipBlanks (ptr);
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
				 *pref != EOL && *pref != TAB && *pref != CR)
			    pref++;
			  if (*pref == ',')
			    pref++;
			  pref = TtaSkipBlanks (pref);
			}
		    }
		  if (!ok)
		    {
		      while (*ptr != EOS && *ptr != ',' && *ptr != SPACE &&
			     *ptr != BSPACE && *ptr != EOL && *ptr != TAB &&
			     *ptr != CR)
			ptr++;
		      if (*ptr == ',')
			ptr++;
		      ptr = TtaSkipBlanks (ptr);
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
  ctxt->cssSpecificity = 0;   /* the presentation rule to be set is not a CSS rule */
  pval.typed_data.unit = STYLE_UNIT_PX;
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
   SVGElementComplete
   Check the Thot structure of the SVG element el.
  ----------------------------------------------------------------------*/
void SVGElementComplete (Element el, Document doc, int *error)
{
   ElementType		elType, parentType, newType;
   Element		child, parent, new, leaf;
   AttributeType        attrType;
   Attribute            attr;
   int                  length;
   PRule		fillPatternRule, newPRule;
   SSchema	        SVGSSchema;
   char                *text, *href;
   ThotBool		closedShape, parseCSS;

   *error = 0;
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
	new = TtaNewElement (doc, newType);
	TtaInsertFirstChild (&new, el, doc);
	/* Create a placeholder within the SVG element */
        newType.ElTypeNum = SVG_EL_GraphicsElement;
	child = TtaNewElement (doc, newType);
	TtaInsertFirstChild (&child, new, doc);
	}
     }
   else
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
	      /* apply a deph rule to each child */
	      SetGraphicDepths (doc, el);
	   }
	}

     switch (elType.ElTypeNum)
       {
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

       case SVG_EL_use_:
	 /* it's a use element */
	 /* if it has a href attribute from the XLink namespace, replace
	    that attribute by a href attribute from the SVG namespace */
	 CheckHrefAttr (el, doc);
	 /* make a transclusion of the element addressed by its xlink_href
	    attribute after the last child */
         /* first, get the xlink:href attribute */
	 attrType.AttrSSchema = elType.ElSSchema;
	 attrType.AttrTypeNum = SVG_ATTR_xlink_href;
	 attr = TtaGetAttribute (el, attrType);
	 if (attr)
	   /* the use element has a xlink:href attribute */
	   {
	     /* get its value */
	     length = TtaGetTextAttributeLength (attr);
	     href = TtaGetMemory (length + 1);
	     TtaGiveTextAttributeValue (attr, href, &length);
	     CopyUseContent (el, doc, href);
	     TtaFreeMemory (href);
	   }
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
	 /* Search the  type attribute */
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
	     text = TtaGetMemory (length + 1);
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
		 ReadCSSRules (doc, NULL, text,
			       TtaGetElementLineNumber (el), FALSE);
		 TtaFreeMemory (text);
	       }
	   }
	 break;
 
       default:
	 /* if it's a graphic primitive, create a GRAPHIC_UNIT leaf as a child
	    of the element, if it has not been done when creating attributes
	    (points, rx, ry) */
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
	 break;
       }
     }
}

/*----------------------------------------------------------------------
   UnknownSVGNameSpace
   Create an element that belongs to a non-supported namespace
  ----------------------------------------------------------------------*/
void               UnknownSVGNameSpace (ParserData *context, char* content)
{
   ElementType     elType;
   Element         elInv, elText;

   /* Create a new Invalid_element */
   elType.ElSSchema = GetXMLSSchema (SVG_TYPE, context->doc);
   elType.ElTypeNum = SVG_EL_Unknown_namespace;
   elInv = TtaNewElement (context->doc, elType);
   if (elInv != NULL)
     {
       XmlSetElemLineNumber (elInv);
       InsertXmlElement (&elInv);
       context->lastElementClosed = TRUE;
       elType.ElTypeNum = SVG_EL_TEXT_UNIT;
       elText = TtaNewElement (context->doc, elType);
       XmlSetElemLineNumber (elText);
       TtaInsertFirstChild (&elText, elInv, context->doc);
       TtaSetTextContent (elText, content, context->language, context->doc);
       TtaSetAccessRight (elText, ReadOnly, context->doc);
     }
}

/*----------------------------------------------------------------------
 UpdatePositionOfPoly
 Set position, width and height for an element polyline or polygon.
 Change coords of control points accordingly.
 -----------------------------------------------------------------------*/
void   UpdatePositionOfPoly (Element el, Document doc, int minX, int minY, int maxX, int maxY)
{
   PRule                pRule;
   Element              leaf;
   int			height, width, i, nbPoints, x, y;
   TypeUnit		unit;
   PresentationValue    pval;
   PresentationContext  ctxt;

   leaf = TtaGetFirstChild (el);
   ctxt = TtaGetSpecificStyleContext (doc);
   /* the specific presentation is not a CSS rule */
   ctxt->cssSpecificity = 0;
   ctxt->destroy = FALSE;
   pval.typed_data.unit = STYLE_UNIT_PX;

   unit = UnPixel;
   width = maxX - minX;
   height = maxY - minY;
   nbPoints = TtaGetPolylineLength (leaf);
   for (i = 1; i <= nbPoints; i++)
      {
      TtaGivePolylinePoint (leaf, i, unit, &x, &y);
      TtaModifyPointInPolyline (leaf, i, unit, x-minX, y-minY, doc);
      }
   TtaChangeLimitOfPolyline (leaf, unit, width, height, doc);

   pRule = TtaGetPRule (el, PRHorizPos);
   if (pRule)
     {
     x = TtaGetPRuleValue (pRule);
     unit = TtaGetPRuleUnit (pRule);
     }
   else
     x = 0;
   pval.typed_data.value = x+minX;
   pval.typed_data.real = FALSE;
   TtaSetStylePresentation (PRHorizPos, el, NULL, ctxt, pval);

   pRule = TtaGetPRule (el, PRVertPos);
   if (pRule)
     {
     y = TtaGetPRuleValue (pRule);
     unit = TtaGetPRuleUnit (pRule);
     }
   else
     y = 0;
   pval.typed_data.value = y+minY;
   pval.typed_data.real = FALSE;
   TtaSetStylePresentation (PRVertPos, el, NULL, ctxt, pval);

   pval.typed_data.value = width;
   pval.typed_data.real = FALSE;
   TtaSetStylePresentation (PRWidth, el, NULL, ctxt, pval);
          
   pval.typed_data.value = height;
   pval.typed_data.real = FALSE;
   TtaSetStylePresentation (PRHeight, el, NULL, ctxt, pval);

   TtaFreeMemory (ctxt);
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
   AttributeType        attrType;
   PresentationValue    pval;
   PresentationContext  ctxt;

   length = TtaGetTextAttributeLength (attr) + 2;
   text = TtaGetMemory (length);
   if (text != NULL)
      {
      /* get the value of the x or y attribute */
      TtaGiveTextAttributeValue (attr, text, &length);
      /* parse the attribute value (a number followed by a unit) */
      ptr = text;
      ptr = TtaSkipBlanks (ptr);
      ptr = ParseCSSUnit (ptr, &pval);
      if (pval.typed_data.unit != STYLE_UNIT_INVALID)
	{
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
	    ruleType = PRWidth;
	  else if (attrType.AttrTypeNum == SVG_ATTR_y2)
	    ruleType = PRHeight;
	  else if (attrType.AttrTypeNum == SVG_ATTR_dx)
	    ruleType = PRHorizPos;
	  else if (attrType.AttrTypeNum == SVG_ATTR_dy)
	    ruleType = PRVertPos;
	  else
	    return;
	  ctxt = TtaGetSpecificStyleContext (doc);
	  /* the specific presentation is not a CSS rule */
	  ctxt->cssSpecificity = 0;
	  ctxt->destroy = FALSE;
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
				    ThotBool delete)
{
   AttributeType	attrType;
   ElementType          elType;
   Element              child;
   int			length, attrKind, ruleType;
   char		       *text, *ptr;
   PresentationValue    pval;
   PresentationContext  ctxt;
   ThotBool             ret;

   ret = FALSE; /* let Thot perform normal operation */
   elType = TtaGetElementType (el);
   text = NULL;
   if (attr && !delete)
     {
       length = TtaGetTextAttributeLength (attr) + 2;
       text = TtaGetMemory (length);
       if (!text)
	 return ret;
     }
   ctxt = TtaGetSpecificStyleContext (doc);
   /* the specific presentation is not a CSS rule */
   ctxt->cssSpecificity = 0;
   ctxt->destroy = FALSE;
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
   if (delete)
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
          pval.typed_data.unit = STYLE_UNIT_PX;
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
      ptr = TtaSkipBlanks (ptr);
      ptr = ParseCSSUnit (ptr, &pval);
      if (pval.typed_data.unit != STYLE_UNIT_INVALID)
	 {
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
	 if ((elType.ElTypeNum == SVG_EL_ellipse ||
	      elType.ElTypeNum == SVG_EL_circle) &&
	     (attrType.AttrTypeNum == SVG_ATTR_r ||
	      attrType.AttrTypeNum == SVG_ATTR_rx ||
	      attrType.AttrTypeNum == SVG_ATTR_ry))
	   /* that's the radius of a circle or an ellipse,
	      multiply the value by 2 to set the width or height of the box */
	   pval.typed_data.value *= 2;
         if (ruleType == PRXRadius || ruleType == PRYRadius)
	   /* that's the radius of a rounded corner. Get the graphics leaf
	      which will receive the specific presentation rule */
	   {
	     child = TtaGetFirstChild (el);
	     while (child &&
		    TtaGetElementType(child).ElTypeNum != GRAPHICS_UNIT)
	        TtaNextSibling (&child);
	     el = child;
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

/*----------------------------------------------------------------------
   GetNumber
   Parse an integer or floating point number and skip to next token.
   Return the value of that number in param number and moves ptr
   to the next token to be parsed.
  ----------------------------------------------------------------------*/
static char *     GetNumber (char *ptr, int* number)
{
  int      integer, nbdecimal, exponent, i;
  char     *decimal;
  ThotBool negative, negativeExp;

  *number = 0;
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
	  *ptr == EOL    || *ptr == TAB   || *ptr == CR))
    ptr++;
  return (ptr);
}

/*----------------------------------------------------------------------
   ParsePointsAttribute
   Process the points attribute
  ----------------------------------------------------------------------*/
void ParsePointsAttribute (Attribute attr, Element el, Document doc)
{
   Element		leaf;
   TypeUnit		unit;
   char		       *text, *ptr;
   int			length, x, y, nbPoints, maxX, maxY, minX, minY, i;
   ThotBool		closed, error;

   /* create (or get) the Graphics leaf according to the element type */
   leaf = CreateGraphicLeaf (el, doc, &closed);
   if (leaf == NULL)
      return;

   /* text attribute. Get its value */
   length = TtaGetTextAttributeLength (attr) + 2;
   text = TtaGetMemory (length);
   if (text)
      {
      /* first, delete all points in the polyline */
      nbPoints = TtaGetPolylineLength (leaf);
      for (i = 1; i <= nbPoints; i++)
	  TtaDeletePointInPolyline (leaf, i, doc);
      /* get the content of the points attribute */
      TtaGiveTextAttributeValue (attr, text, &length);
      ptr = text;
      error = FALSE;
      ptr = TtaSkipBlanks (ptr);
      nbPoints = 0;
      minX = minY = 32000;
      maxX = maxY = 0;
      unit = UnPixel;
      while (*ptr != EOS)
         {
         x = y = 0;
	 ptr = GetNumber (ptr, &x);
         if (x > maxX)
            maxX = x;
         if (x < minX)
   	    minX = x;
	 if (*ptr == EOS)
	   error = TRUE;
         if (*ptr == ',')
	   {
	     ptr++;
	     ptr = TtaSkipBlanks (ptr);
	   }
	 ptr = GetNumber (ptr, &y);
         if (y > maxY)
            maxY = y;
         if (y < minY)
   	    minY = y;
         nbPoints++;
         TtaAddPointInPolyline (leaf, nbPoints, unit, x, y, doc);
         if (*ptr == ',')
	   {
	     ptr++;
	     ptr = TtaSkipBlanks (ptr);
	   }	 
         }
      UpdatePositionOfPoly (el, doc, 0, 0, maxX, maxY);
      TtaFreeMemory (text);
      }
}

/*----------------------------------------------------------------------
   ParseTransformAttribute
   Parse the value of a transform attribute
  ----------------------------------------------------------------------*/
void ParseTransformAttribute (Attribute attr, Element el, Document doc,
			      ThotBool delete)
{
   int                  length, a, b, c, d, e, f, x, y, angle;
   char                *text, *ptr;
   PresentationValue    pval;
   PresentationContext  ctxt;
   ThotBool             error;

   length = TtaGetTextAttributeLength (attr) + 2;
   text = TtaGetMemory (length);
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
	   ptr = TtaSkipBlanks (ptr);
	   if (!strncmp (ptr, "matrix", 6))
	     {
	       ptr += 6;
	       ptr = TtaSkipBlanks (ptr);
	       if (*ptr != '(')
		 error = TRUE;
	       else
		 {
		   ptr++;
		   ptr = TtaSkipBlanks (ptr);
		   ptr = GetNumber (ptr, &a);
		   if (*ptr == ',')
		     {
		       ptr++;
		       ptr = TtaSkipBlanks (ptr);
		     }
		   ptr = GetNumber (ptr, &b);
		   if (*ptr == ',')
		     {
		       ptr++;
		       ptr = TtaSkipBlanks (ptr);
		     }
		   ptr = GetNumber (ptr, &c);
		   if (*ptr == ',')
		     {
		       ptr++;
		       ptr = TtaSkipBlanks (ptr);
		     }
		   ptr = GetNumber (ptr, &d);
		   if (*ptr == ',')
		     {
		       ptr++;
		       ptr = TtaSkipBlanks (ptr);
		     }
		   ptr = GetNumber (ptr, &e);
		   if (*ptr == ',')
		     {
		       ptr++;
		       ptr = TtaSkipBlanks (ptr);
		     }
		   ptr = GetNumber (ptr, &f);
		   if (*ptr != ')')
		     error = TRUE;
		   else
		     {
		       ptr++;
		       pval.typed_data.value = 0;
		       pval.typed_data.unit = STYLE_UNIT_PX;
		       pval.typed_data.real = FALSE;
		       ctxt = TtaGetSpecificStyleContext (doc);
		       ctxt->cssSpecificity = 0; /* this is not a CSS rule */
		       ctxt->destroy = delete;
		       /****** process values a, b, c, d *****/
		       /* value e specifies an horizontal translation */
		       if (e != 0)
			 {
			   pval.typed_data.value = e;
			   TtaSetStylePresentation (PRHorizPos, el, NULL,
						    ctxt, pval);
			 }
		       /* value f specifies a vertical translation */
		       if (f != 0)
			 {
			   pval.typed_data.value = f;
			   TtaSetStylePresentation (PRVertPos, el, NULL,
						    ctxt, pval);
			 }
		       TtaFreeMemory (ctxt);
		     }
		 }
	     }
	   else if (!strncmp (ptr, "translate", 9))
	     {
	       x = 0;  y = 0;
	       ptr += 9;
	       ptr = TtaSkipBlanks (ptr);
	       pval.typed_data.value = 0;
	       pval.typed_data.unit = STYLE_UNIT_PX;
	       pval.typed_data.real = FALSE;
	       if (*ptr != '(')
		 error = TRUE;
	       else
		 {
		   ptr++;
		   ptr = TtaSkipBlanks (ptr);
		   ptr = GetNumber (ptr, &x);
		   pval.typed_data.value = x;
		   ctxt = TtaGetSpecificStyleContext (doc);
		   ctxt->cssSpecificity = 0;     /* this is not a CSS rule */
		   ctxt->destroy = delete;
		   TtaSetStylePresentation (PRHorizPos, el, NULL, ctxt, pval);
		   if (*ptr == ')')
		     pval.typed_data.value = 0;
		   else
		     {
		       if (*ptr == ',')
			 {
			   ptr++;
			   ptr = TtaSkipBlanks (ptr);
			 }
		       ptr = GetNumber (ptr, &y);
		       pval.typed_data.value = y;
		     }
		   TtaSetStylePresentation (PRVertPos, el, NULL, ctxt, pval);
		   if (*ptr == ')')
		     ptr++;
		   else
		     error = TRUE;
		   TtaFreeMemory (ctxt);
		 }
	     }
	   else if (!strncmp (ptr, "scale", 5))
	     {
	       ptr += 5;
	       ptr = TtaSkipBlanks (ptr);
	       if (*ptr != '(')
		 error = TRUE;
	       else
		 {
		   ptr++;
		   ptr = TtaSkipBlanks (ptr);
		   ptr = GetNumber (ptr, &x);
		   if (*ptr == ')')
		     y = 0;
		   else
		     {
		       if (*ptr == ',')
			 {
			   ptr++;
			   ptr = TtaSkipBlanks (ptr);
			 }
		       ptr = GetNumber (ptr, &y);
		     }
		   if (*ptr == ')')
		     {
		       ptr++;
		       /****** process x and y ******/
		     }
		   else
		     error = TRUE;
		 }
	     }
	   else if (!strncmp (ptr, "rotate", 6))
	     {
	       ptr += 6;
	       ptr = TtaSkipBlanks (ptr);
	       if (*ptr != '(')
		 error = TRUE;
	       else
		 {
		   ptr++;
		   ptr = TtaSkipBlanks (ptr);
		   ptr = GetNumber (ptr, &angle);
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
			   ptr = TtaSkipBlanks (ptr);
			 }
		       ptr = GetNumber (ptr, &x);
		       if (*ptr == ',')
			 {
			   ptr++;
			   ptr = TtaSkipBlanks (ptr);
			 }
		       ptr = GetNumber (ptr, &y);
		     }
		   if (*ptr == ')')
		     {
		       ptr++;
		       /****** process angle, x and y ******/
		     }
		   else
		     error = TRUE;
		 }
	     }
	   else if (!strncmp (ptr, "skewX", 5))
	     {
	       ptr += 5;
	       ptr = TtaSkipBlanks (ptr);
	       if (*ptr != '(')
		 error = TRUE;
	       else
		 {
		   ptr++;
		   ptr = TtaSkipBlanks (ptr);
		   ptr = GetNumber (ptr, &x);
		   if (*ptr == ')')
		     {
		       ptr++;
		       /****** process x ******/
		     }
		   else
		     error = TRUE;
		 }
	     }
	   else if (!strncmp (ptr, "skewY", 5))
	     {
	       ptr += 5;
	       ptr = TtaSkipBlanks (ptr);
	       if (*ptr != '(')
		 error = TRUE;
	       else
		 {
		   ptr++;
		   ptr = TtaSkipBlanks (ptr);
		   ptr = GetNumber (ptr, &y);
		   if (*ptr == ')')
		     {
		       ptr++;
		       /****** process y ******/
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
	       ptr = TtaSkipBlanks (ptr);
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
void ParsePathDataAttribute (Attribute attr, Element el, Document doc)
{
   int          length, x, y, x1, y1, x2, y2, xcur, ycur, xinit, yinit,
                x2prev, y2prev, x1prev, y1prev, rx, ry, xAxisRotation,
                largeArcFlag, sweepFlag;
   Element      leaf;
   PathSegment  seg;
   ThotBool     relative, newSubpath;
   char         *text, *ptr;
   char         command, prevCommand;

   /* create (or get) the Graphics leaf */
   leaf = CreateGraphicalLeaf (EOS, el, doc);
   if (leaf == NULL)
      return;

   /* if the leaf element is a graphic element, turn it into a path */
   TtaAppendPathSeg (leaf, NULL, doc);
   /* get a buffer for reading the attribute value */
   length = TtaGetTextAttributeLength (attr) + 2;
   text = TtaGetMemory (length);
   if (text)
      {
      /* get the content of the path data attribute */
      TtaGiveTextAttributeValue (attr, text, &length);
      /* parse the attribute content */
      ptr = text;
      ptr = TtaSkipBlanks (ptr);
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
      while (command != EOS)
         {
         relative = TRUE;
         switch (command)
	   {
	   case 'M':
	     relative = FALSE;
	   case 'm':
	     /* moveto */
             ptr = TtaSkipBlanks (ptr);
	     if (relative)
	       {
	       x = xcur;
	       y = ycur;
	       }
	     ptr = GetNumber (ptr, &xcur);
             ptr = GetNumber (ptr, &ycur);
	     if (relative)
	       {
	       xcur += x;
	       ycur += y;
	       }
	     xinit = xcur;
             yinit = ycur;
	     newSubpath = TRUE;
	     break;

	   case 'Z':
	     relative = FALSE;
	   case 'z':
	     /* close path */
	     /* draw a line from (xcur, ycur) to (xinit, yinit) */
	     seg = TtaNewPathSegLine (xcur, ycur, xinit, yinit, newSubpath);
	     TtaAppendPathSeg (leaf, seg, doc);
	     newSubpath = FALSE;
	     xcur = xinit;
	     ycur = yinit;
	     break;

	   case 'L':
	     relative = FALSE;
	   case 'l':
	     /* lineto */
             ptr = TtaSkipBlanks (ptr);
             ptr = GetNumber (ptr, &x);
             ptr = GetNumber (ptr, &y);
	     if (relative)
	       {
	       x += xcur;
	       y += ycur;
	       }
	     /* draw a line from (xcur, ycur) to (x, y) */
	     seg = TtaNewPathSegLine (xcur, ycur, x, y, newSubpath);
	     TtaAppendPathSeg (leaf, seg, doc);
	     newSubpath = FALSE;
	     xcur = x;
	     ycur = y;
	     break;

	   case 'H':
	     relative = FALSE;
	   case 'h':
	     /* horizontal lineto */
             ptr = TtaSkipBlanks (ptr);
             ptr = GetNumber (ptr, &x);
	     if (relative)
	       x += xcur;
	     /* draw a line from (xcur, ycur) to (x, ycur) */
	     seg = TtaNewPathSegLine (xcur, ycur, x, ycur, newSubpath);
	     TtaAppendPathSeg (leaf, seg, doc);
	     newSubpath = FALSE;
	     xcur = x;
	     break;

	   case 'V':
	     relative = FALSE;
	   case 'v':
	     /* vertical lineto */
             ptr = TtaSkipBlanks (ptr);
             ptr = GetNumber (ptr, &y);
	     if (relative)
	       y += ycur;
	     /* draw a line from (xcur, ycur) to (xcur, y) */
	     seg = TtaNewPathSegLine (xcur, ycur, xcur, y, newSubpath);
	     TtaAppendPathSeg (leaf, seg, doc);
	     newSubpath = FALSE;
	     ycur = y;
	     break;

	   case 'C':
	     relative = FALSE;
	   case 'c':
	     /* curveto */
             ptr = TtaSkipBlanks (ptr);
             ptr = GetNumber (ptr, &x1);
             ptr = GetNumber (ptr, &y1);
             ptr = GetNumber (ptr, &x2);
             ptr = GetNumber (ptr, &y2);
	     ptr = GetNumber (ptr, &x);
             ptr = GetNumber (ptr, &y);
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
		and (x2, y2) as the control point at the end of the curve */
	     seg = TtaNewPathSegCubic (xcur, ycur, x, y, x1, y1, x2, y2,
				       newSubpath);
	     TtaAppendPathSeg (leaf, seg, doc);
	     newSubpath = FALSE;
             xcur = x;
             ycur = y;
             x2prev = x2;
             y2prev = y2;
	     break;

	   case 'S':
	     relative = FALSE;
	   case 's':
	     /* smooth curveto */
             ptr = TtaSkipBlanks (ptr);
             ptr = GetNumber (ptr, &x2);
             ptr = GetNumber (ptr, &y2);
	     ptr = GetNumber (ptr, &x);
             ptr = GetNumber (ptr, &y);
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
		and (x2, y2) as the control point at the end of the curve */
	     seg = TtaNewPathSegCubic (xcur, ycur, x, y, x1, y1, x2, y2,
				       newSubpath);
	     TtaAppendPathSeg (leaf, seg, doc);
	     newSubpath = FALSE;
             xcur = x;
             ycur = y;
             x2prev = x2;
             y2prev = y2;
	     break;

	   case 'Q':
	     relative = FALSE;
	   case 'q':
	     /* quadratic Bezier curveto */
             ptr = TtaSkipBlanks (ptr);
             ptr = GetNumber (ptr, &x1);
             ptr = GetNumber (ptr, &y1);
	     ptr = GetNumber (ptr, &x);
             ptr = GetNumber (ptr, &y);
	     if (relative)
	       {
	       x1 += xcur;
	       y1 += ycur;
	       x += xcur;
	       y += ycur;
	       }
	     /* draw a quadratic Bezier curve from (xcur, ycur) to (x, y) using
                (x1, y1) as the control point */
	     seg = TtaNewPathSegQuadratic (xcur, ycur, x, y, x1, y1, newSubpath);
	     TtaAppendPathSeg (leaf, seg, doc);
	     newSubpath = FALSE;
             xcur = x;
             ycur = y;
             x1prev = x1;
             y1prev = y1;
	     break;

	   case 'T':
	     relative = FALSE;
	   case 't':
	     /* smooth quadratic Bezier curveto */
             ptr = TtaSkipBlanks (ptr);
	     ptr = GetNumber (ptr, &x);
             ptr = GetNumber (ptr, &y);
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
	     /* draw a quadratic Bezier curve from (xcur, ycur) to (x, y) using
                (x1, y1) as the control point */
	     seg = TtaNewPathSegQuadratic (xcur, ycur, x, y, x1, y1, newSubpath);
	     TtaAppendPathSeg (leaf, seg, doc);
	     newSubpath = FALSE;
             xcur = x;
             ycur = y;
             x1prev = x1;
             y1prev = y1;
	     break;

	   case 'A':
	     relative = FALSE;
	   case 'a':
	     /* elliptical arc */
             ptr = TtaSkipBlanks (ptr);
             ptr = GetNumber (ptr, &rx);            /* must be non-negative */
             ptr = GetNumber (ptr, &ry);            /* must be non-negative */
             ptr = GetNumber (ptr, &xAxisRotation);
             ptr = GetNumber (ptr, &largeArcFlag);  /* must be "0" or "1" */
             ptr = GetNumber (ptr, &sweepFlag);     /* must be "0" or "1" */
	     ptr = GetNumber (ptr, &x);
             ptr = GetNumber (ptr, &y);
	     if (relative)
	       {
	       x += xcur;
	       y += ycur;
	       }
	     /* draw an elliptical arc from (xcur, ycur) to (x, y) */
	     seg = TtaNewPathSegArc (xcur, ycur, x, y, rx, ry, xAxisRotation,
				     largeArcFlag == 1, sweepFlag == 1,
				     newSubpath);
	     TtaAppendPathSeg (leaf, seg, doc);
	     newSubpath = FALSE;
             xcur = x;
             ycur = y;
	     break;

	   default:
	     /* unknown command. error. stop parsing. */
	     command = EOS;
	     break;
	   }
	 if (command != EOS)
	   {
	   prevCommand = command;
	   if (command == 'Z' || command == 'z')
	     /* don't expect coordinates after a close path command, only
		a new command or end of the string */
	     {
             ptr = TtaSkipBlanks (ptr);	     
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
}

/*----------------------------------------------------------------------
   SVGAttributeComplete
   The XML parser has read attribute attr for element el in document doc.
  ----------------------------------------------------------------------*/
void      SVGAttributeComplete (Attribute attr, Element el, Document doc)
{
   AttributeType	attrType, attrType1;
   Attribute            intAttr;
   ElementType          elType;
   Element		leaf;
   int			attrKind;
   ThotBool		closed;

   TtaGiveAttributeType (attr, &attrType, &attrKind);

   switch (attrType.AttrTypeNum)
     {
     case SVG_ATTR_height_:
     case SVG_ATTR_width_:
     case SVG_ATTR_r:
	ParseWidthHeightAttribute (attr, el, doc, FALSE);
	break;
     case SVG_ATTR_font_family:
     case SVG_ATTR_font_size:
     case SVG_ATTR_font_style:
     case SVG_ATTR_font_variant:
     case SVG_ATTR_font_weight:
     case SVG_ATTR_text_decoration:
     case SVG_ATTR_fill:
     case SVG_ATTR_stroke:
     case SVG_ATTR_stroke_width:
        ParseCSSequivAttribute (attrType.AttrTypeNum, attr, el, doc, FALSE);
	break;
     case SVG_ATTR_transform:
        ParseTransformAttribute (attr, el, doc, FALSE);
	break;
     case SVG_ATTR_points:
	ParsePointsAttribute (attr, el, doc);
	break;
     case SVG_ATTR_x:
     case SVG_ATTR_y:
     case SVG_ATTR_cx:
     case SVG_ATTR_cy:
     case SVG_ATTR_x1:
     case SVG_ATTR_y1:
     case SVG_ATTR_x2:
     case SVG_ATTR_y2:
     case SVG_ATTR_dx:
     case SVG_ATTR_dy:
	ParseCoordAttribute (attr, el, doc);
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
     case SVG_ATTR_d:
	ParsePathDataAttribute (attr, el, doc);
        break;
     case SVG_ATTR_Language:
        if (el == TtaGetRootElement (doc))
	  /* it's the lang attribute on the root element */
	  /* set the RealLang attribute */
	  {
	    attrType1.AttrSSchema = attrType.AttrSSchema ;
	    attrType1.AttrTypeNum = MathML_ATTR_RealLang;
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
     default:
	break;
     }
}

/* end of module */
