/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1998-2000
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 
/*
 *
 * GraphMLbuilder
 *
 * Author: V. Quint
 *
 */
 

#define THOT_EXPORT extern
#include "amaya.h"
#include "css.h"
#include "GraphML.h"
#include "HTML.h"
#include "parser.h"
#include "styleparser_f.h"
#include "style.h"

/* mapping table of attribute values */

static AttrValueMapping GraphMLAttrValueMappingTable[] =
{ 
   {GraphML_ATTR_arrowhead, TEXT("both"), GraphML_ATTR_arrowhead_VAL_both},
   {GraphML_ATTR_arrowhead, TEXT("end"), GraphML_ATTR_arrowhead_VAL_end_},
   {GraphML_ATTR_arrowhead, TEXT("none"), GraphML_ATTR_arrowhead_VAL_none_},
   {GraphML_ATTR_arrowhead, TEXT("start"), GraphML_ATTR_arrowhead_VAL_start},
   {GraphML_ATTR_linestyle_, TEXT("dashed"), GraphML_ATTR_linestyle__VAL_dashed_},
   {GraphML_ATTR_linestyle_, TEXT("dotted"), GraphML_ATTR_linestyle__VAL_dotted_},
   {GraphML_ATTR_linestyle_, TEXT("solid"), GraphML_ATTR_linestyle__VAL_solid_},
   {0, TEXT(""), 0}			/* Last entry. Mandatory */
};

#define MaxMsgLength 200

#include "fetchXMLname_f.h"
#include "html2thot_f.h"
#include "XMLparser_f.h"


/*----------------------------------------------------------------------
   GraphMLGetDTDName
   Return in DTDname the name of the DTD to be used for parsing the
   content of element named elementName.
   This element type appear with an 'X' in the ElemMappingTable.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void      GraphMLGetDTDName (STRING DTDname, STRING elementName)
#else
void      GraphMLGetDTDName (DTDname, elementName)
STRING DTDname;
STRING elementName;
 
#endif
{
   if (ustrcmp (elementName, TEXT("math")) == 0)
      ustrcpy (DTDname, TEXT("MathML"));
   else if (ustrcmp (elementName, TEXT("label")) == 0 ||
	    ustrcmp (elementName, TEXT("text")) == 0)
      ustrcpy (DTDname, TEXT("HTML"));
   else
      ustrcpy (DTDname, TEXT(""));
}

/*----------------------------------------------------------------------
   MapGraphMLAttribute
   Search in the Attribute Mapping Table the entry for the
   attribute of name Attr and returns the corresponding Thot attribute type.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void           MapGraphMLAttribute (CHAR_T *attrName, AttributeType *attrType, CHAR_T* elementName, Document doc)
#else
void           MapGraphMLAttribute (Attr, attrType, elementName, doc)
CHAR_T        *attrName;
AttributeType *attrType;
CHAR_T        *elementName;
Document       doc;
#endif
{
  attrType->AttrSSchema = GetGraphMLSSchema (doc);
  MapXMLAttribute (GRAPH_TYPE, attrName, elementName, doc, &(attrType->AttrTypeNum));
}

/*----------------------------------------------------------------------
   MapGraphMLAttributeValue
   Search in the Attribute Value Mapping Table the entry for the attribute
   ThotAtt and its value AttrVal. Returns the corresponding Thot value.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                MapGraphMLAttributeValue (CHAR_T* AttrVal, AttributeType attrType, int *value)
#else
void                MapGraphMLAttributeValue (AttrVal, attrType, value)
CHAR_T*             AttrVal;
AttributeType       attrType;
int		   *value;
#endif
{
   int                 i;

   *value = 0;
   i = 0;
   while (GraphMLAttrValueMappingTable[i].ThotAttr != attrType.AttrTypeNum &&
	  GraphMLAttrValueMappingTable[i].ThotAttr != 0)
      i++;
   if (GraphMLAttrValueMappingTable[i].ThotAttr == attrType.AttrTypeNum)
      do
	 if (!ustrcasecmp (GraphMLAttrValueMappingTable[i].XMLattrValue, AttrVal))
	    *value = GraphMLAttrValueMappingTable[i].ThotAttrValue;
	 else
	    i++;
      while (*value <= 0 && GraphMLAttrValueMappingTable[i].ThotAttr != 0);
}

/*----------------------------------------------------------------------
   MapGraphMLEntity
   Search that entity in the entity table and return the corresponding value.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void	MapGraphMLEntity (STRING entityName, STRING entityValue, STRING alphabet)
#else
void	MapGraphMLEntity (entityName, entityValue, alphabet)
STRING  entityName;
STRING  entityValue;
STRING  alphabet;
#endif
{
   entityValue[0] = EOS;
   *alphabet = EOS;
}

/*----------------------------------------------------------------------
   GraphMLEntityCreated
   A GraphML entity has been created by the XML parser.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void        GraphMLEntityCreated (USTRING entityValue, Language lang, STRING entityName, Document doc)
#else
void        GraphMLEntityCreated (entityValue, lang, entityName, doc)
USTRING     entityValue;
Language    lang;
STRING      entityName;
Document    doc;
#endif
{

}

/*----------------------------------------------------------------------
   CreateGraphicalLeaf
   Create a GRAPHICS_UNIT element as the last child of element el if it
   does not exist yet.
   Return that GRAPHICS_UNIT element.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static Element      CreateGraphicalLeaf (char shape, Element el, Document doc, ThotBool changeShape)
#else
static Element      CreateGraphicalLeaf (shape, el, doc, changeShape)
char                shape;
Element             el;
Document            doc;
ThotBool            changeShape;

#endif
{
   ElementType	elType;
   Element	leaf, child;
   CHAR_T       oldShape;

   leaf = NULL;
   child = TtaGetLastChild (el);
   if (child != NULL)
      /* there is a child element */
      {
      elType = TtaGetElementType (child);
      if (elType.ElTypeNum == GraphML_EL_GRAPHICS_UNIT)
	 {
	 oldShape = TtaGetGraphicsShape (child);
	 leaf = child;
	 if (oldShape == EOS ||
             (changeShape && oldShape != shape))
	    TtaSetGraphicsShape (child, shape, doc);
	 }
      }
   if (leaf == NULL)
      /* create the graphical element */
      {
      elType.ElSSchema = GetGraphMLSSchema (doc);
      elType.ElTypeNum = GraphML_EL_GRAPHICS_UNIT;
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
#ifdef __STDC__
Element         CreateGraphicLeaf (Element el, Document doc, ThotBool *closed, int arrowHead)
#else
Element         CreateGraphicLeaf (el, doc, closed, arrowHead)
Element		el;
Document	doc;
ThotBool		*closed;
int		arrowHead;

#endif
{
   ElementType elType;
   Element     leaf;
   char        shape;

   leaf = NULL;
   *closed = FALSE;
   elType = TtaGetElementType (el);
   switch (elType.ElTypeNum)
       {
       case GraphML_EL_Line_:
	  switch (arrowHead)
		{
		case GraphML_ATTR_arrowhead_VAL_none_:
			shape = 'w';
			break;
		case GraphML_ATTR_arrowhead_VAL_start:
			shape = 'x';
			break;
		case GraphML_ATTR_arrowhead_VAL_end_:
			shape = 'y';
			break;
		case GraphML_ATTR_arrowhead_VAL_both:
			shape = 'z';
			break;
		default:
			shape = 'w';
			break;
		}
	  leaf = CreateGraphicalLeaf (shape, el, doc, arrowHead != 0);
	  break;
       case GraphML_EL_Rectangle:
	  leaf = CreateGraphicalLeaf ('R', el, doc, FALSE);
	  *closed = TRUE;
	  break;
       case GraphML_EL_RoundRect:
	  leaf = CreateGraphicalLeaf ('C', el, doc, FALSE);
	  *closed = TRUE;
	  break;
       case GraphML_EL_Circle:
	  leaf = CreateGraphicalLeaf ('a', el, doc, FALSE);
	  *closed = TRUE;
	  break;
       case GraphML_EL_Oval:
	  leaf = CreateGraphicalLeaf ('c', el, doc, FALSE);
	  *closed = TRUE;
	  break;
       case GraphML_EL_Polyline:
	  switch (arrowHead)
		{
		case GraphML_ATTR_arrowhead_VAL_none_:
			shape = 'S';
			break;
		case GraphML_ATTR_arrowhead_VAL_start:
			shape = 'N';
			break;
		case GraphML_ATTR_arrowhead_VAL_end_:
			shape = 'U';
			break;
		case GraphML_ATTR_arrowhead_VAL_both:
			shape = 'M';
			break;
		default:
			shape = 'S';
			break;
		}
	  leaf = CreateGraphicalLeaf (shape, el, doc, arrowHead != 0);
	  break;
       case GraphML_EL_Polygon:
	  leaf = CreateGraphicalLeaf ('p', el, doc, FALSE);
	  *closed = TRUE;
	  break;
       case GraphML_EL_Spline:
	  switch (arrowHead)
		{
		case GraphML_ATTR_arrowhead_VAL_none_:
			shape = 'B';
			break;
		case GraphML_ATTR_arrowhead_VAL_start:
			shape = 'F';
			break;
		case GraphML_ATTR_arrowhead_VAL_end_:
			shape = 'A';
			break;
		case GraphML_ATTR_arrowhead_VAL_both:
			shape = 'D';
			break;
		default:
			shape = 'B';
			break;
		}
	  leaf = CreateGraphicalLeaf (shape, el, doc, arrowHead != 0);
	  break;
       case GraphML_EL_ClosedSpline:
	  leaf = CreateGraphicalLeaf ('s', el, doc, FALSE);
	  *closed = TRUE;
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
#ifdef __STDC__
static void     CreateEnclosingElement (Element el, ElementType elType, Document doc)
#else
static void     CreateEnclosingElement (el, elType, doc)
Element		el;
ElementType	elType;
Document	doc;

#endif
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
   ParseFillStrokeAttributes
   Create or update a specific presentation rule for element el that reflects
   the value of attribute attr, which is fill, stroke or stroke-width
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void      ParseFillStrokeAttributes (int attrType, Attribute attr, Element el, Document doc, ThotBool delete)
#else
void      ParseFillStrokeAttributes (attrType, attr, el, doc, delete)
int             attrType;
Attribute	attr;
Element		el;
Document	doc;
ThotBoool       delete;
#endif
{
#define buflen 50
   CHAR_T               css_command[buflen+20];
   int                  length, attrKind;
   STRING               text;

   length = TtaGetTextAttributeLength (attr) + 2;
   text = TtaAllocString (length);
   if (text != NULL)
      {
      /* get the value of the attribute */
      TtaGiveTextAttributeValue (attr, text, &length);
      /* builds the equivalent CSS rule */
      if (attrType == GraphML_ATTR_fill)
	  usprintf (css_command, TEXT("fill: %s"), text);
      else if (attrType == GraphML_ATTR_stroke)
          usprintf (css_command, TEXT("stroke: %s"), text);
      else if (attrType == GraphML_ATTR_stroke_width)
          usprintf (css_command, TEXT("stroke-width: %s"), text);
      /* parse the CSS rule */
      ParseHTMLSpecificStyle (el, css_command, doc, delete);
      TtaFreeMemory (text);
      }
}

/*----------------------------------------------------------------------
   GraphMLElementComplete
   Check the Thot structure of the GraphML element el.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void      GraphMLElementComplete (Element el, Document doc, int *error)
#else
void      GraphMLElementComplete (el, doc)
Element		el;
Document	doc;
int             *error
#endif
{
   ElementType		elType, parentType, newType;
   Element		child, parent, new, leaf;
   PRule		fillPatternRule, newPRule;
   SSchema	        GraphMLSSchema;
   ThotBool		closedShape;

   *error = 0;
   elType = TtaGetElementType (el);
   GraphMLSSchema = GetGraphMLSSchema (doc);
   if (elType.ElSSchema != GraphMLSSchema)
     /* this is not a GraphML element. It's the HTML element <XMLGraphics>, or
	any other element containing a GraphML expression */
     {
     if (TtaGetFirstChild (el) == NULL && !TtaIsLeaf (elType))
	/* this element is empty. Create a GraphML element as it's child */
	{
	newType.ElSSchema = GraphMLSSchema;
	newType.ElTypeNum = GraphML_EL_GraphML;
	new = TtaNewElement (doc, newType);
	TtaInsertFirstChild (&new, el, doc);
	/* Create a placeholder within the GraphML element */
        newType.ElTypeNum = GraphML_EL_GraphicsElement;
	child = TtaNewElement (doc, newType);
	TtaInsertFirstChild (&child, new, doc);
	}
     }
   else
     {
     /* if the parent element is defined by a different SSchema, insert
        a GraphML root element between the element and its parent */
     parent = TtaGetParent (el);
     parentType = TtaGetElementType (parent);
     if (parentType.ElSSchema != elType.ElSSchema)
        if (elType.ElTypeNum != GraphML_EL_GraphML)
	  {
	  newType.ElSSchema = GraphMLSSchema;
	  newType.ElTypeNum = GraphML_EL_GraphML;
	  CreateEnclosingElement (el, newType, doc);
	  }

     /* if it's a Label element, create a HTMLfragment to contain
        the HTML elements */
     if (elType.ElTypeNum == GraphML_EL_Label)
	{
	child = TtaGetFirstChild (el);
	if (child != NULL)
	   {
	   newType.ElSSchema = TtaGetSSchema (TEXT("HTML"), doc);
	   newType.ElTypeNum = HTML_EL_HTMLfragment;
	   CreateEnclosingElement (child, newType, doc);
	   }
	}

     /* if it's an image element, create a PICTURE_UNIT child */
     else if (elType.ElTypeNum == GraphML_EL_image)
       {
	 /* create the graphical element */
	 newType.ElSSchema = elType.ElSSchema;
	 newType.ElTypeNum = GraphML_EL_PICTURE_UNIT;
	 leaf = TtaNewElement (doc, newType);
	 TtaInsertFirstChild (&leaf, el, doc);
       }

     /* if it's a graphic primitive, create a GRAPHIC_UNIT leaf as a child
	of the element, if it has not been done when creating attributes
	(points, arrowhead) */
     else
       {
	 leaf = CreateGraphicLeaf (el, doc, &closedShape, 0);
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
       }
     }
}


/*----------------------------------------------------------------------
 UpdateInternalAttrForPoly
 Create attributes IntPosX, IntPosY, IntWidth, IntHeight for an element
 Polyline, Spline, Polygon or ClosedSpline.
 Change coords of control points accordingly.
 -----------------------------------------------------------------------*/
#ifdef __STDC__
void UpdateInternalAttrForPoly (Element el, Element leaf, Document doc, int minX, int minY, int maxX, int maxY, ThotBool setIntPosition)
#else /* __STDC__*/
void UpdateInternalAttrForPoly (el, leaf, doc, minX, minY, maxX, maxY, setIntPosition)
     Element el;
     Element leaf;
     Document doc;
     int minX;
     int minY;
     int maxX;
     int maxY;
     ThotBool setIntPosition;

#endif /* __STDC__*/
{
   Attribute		attr;
   AttributeType	attrType;
   int			height, width, i, nbPoints, x, y;
   TypeUnit		unit;

   unit = UnPoint;
   width = maxX - minX;
   height = maxY - minY;
   nbPoints = TtaGetPolylineLength (leaf);
   for (i = 1; i <= nbPoints; i++)
      {
      TtaGivePolylinePoint (leaf, i, unit, &x, &y);
      TtaModifyPointInPolyline (leaf, i, unit, x-minX, y-minY, doc);
      }
   TtaChangeLimitOfPolyline (leaf, unit, width, height, doc);

   attrType.AttrSSchema = GetGraphMLSSchema (doc);
   if (setIntPosition)
      {
      attrType.AttrTypeNum = GraphML_ATTR_IntPosX;
      attr = TtaGetAttribute (el, attrType);
      if (attr != NULL)
         x = TtaGetAttributeValue (attr);
      else
         {
         attr = TtaNewAttribute (attrType);
         TtaAttachAttribute (el, attr, doc);
         x = 0;
         }
      TtaSetAttributeValue (attr, x+minX, el, doc);
   
      attrType.AttrTypeNum = GraphML_ATTR_IntPosY;
      attr = TtaGetAttribute (el, attrType);
      if (attr != NULL)
         y = TtaGetAttributeValue (attr);
      else
         {
         attr = TtaNewAttribute (attrType);
         TtaAttachAttribute (el, attr, doc);
         y = 0;
         }
      TtaSetAttributeValue (attr, y+minY, el, doc);
      }

   attrType.AttrTypeNum = GraphML_ATTR_IntWidth;
   attr = TtaGetAttribute (el, attrType);
   if (attr == NULL)
      {
      attr = TtaNewAttribute (attrType);
      TtaAttachAttribute (el, attr, doc);
      }
   TtaSetAttributeValue (attr, width, el, doc);

   attrType.AttrTypeNum = GraphML_ATTR_IntHeight;
   attr = TtaGetAttribute (el, attrType);
   if (attr == NULL)
      {
      attr = TtaNewAttribute (attrType);
      TtaAttachAttribute (el, attr, doc);
      }
   TtaSetAttributeValue (attr, height, el, doc);
}


/*----------------------------------------------------------------------
   CreatePoints
   Process the points attribute
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void            CreatePoints (Attribute attr, Element el, Document doc)
#else
void            CreatePoints (attr, el, doc)
Attribute	attr;
Element		el;
Document	doc;

#endif
{
   Element		leaf;
   int			length, x, y, nbPoints, maxX, maxY, minX, minY, i;
   TypeUnit		unit;
   STRING		text, ptr;
   ThotBool		closed;

   /* create (or get) the Graphics leaf according to the element type */
   leaf = CreateGraphicLeaf (el, doc, &closed, 0);
   if (leaf == NULL)
      return;
   /* text attribute. Get its value */
   length = TtaGetTextAttributeLength (attr) + 2;
   text = TtaAllocString (length);
   if (text != NULL)
      {
      /* first, delete all points in the polyline */
      nbPoints = TtaGetPolylineLength (leaf);
      for (i = 1; i <= nbPoints; i++)
	  TtaDeletePointInPolyline (leaf, i, doc);

      TtaGiveTextAttributeValue (attr, text, &length);
      ptr = text;
      nbPoints = 0;
      minX = minY = 32000;
      maxX = maxY = 0;
      unit = UnPoint;
      while (*ptr != EOS)
         {
         x = y = 0;
         usscanf (ptr, TEXT("%d"), &x);
         if (x > maxX)
            maxX = x;
         if (x < minX)
   	    minX = x;
         ptr = SkipInt (ptr);
         ptr = SkipSep (ptr);
         if (ptr)
            usscanf (ptr, TEXT("%d"), &y);
         if (y > maxY)
            maxY = y;
         if (y < minY)
   	    minY = y;
         ptr = SkipInt (ptr);
         ptr = SkipSep (ptr);
         nbPoints++;
         TtaAddPointInPolyline (leaf, nbPoints, unit, x, y, doc);
         }
      UpdateInternalAttrForPoly (el, leaf, doc, minX, minY, maxX, maxY, TRUE);
      TtaFreeMemory (text);
      }
}

/*----------------------------------------------------------------------
   ParsePositionAttribute
   Create or update attributes IntPosX and IntPosY according to the
   value of the position attribute attr.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void      ParsePositionAttribute (Attribute attr, Element el, Document doc)
#else
void      ParsePositionAttribute (attr, el, doc)
Attribute	attr;
Element		el;
Document	doc;

#endif
{
   int                  length, x, y, attrKind;
   STRING               text, ptr;
   AttributeType        attrType;
   Attribute            attrX, attrY;

   length = TtaGetTextAttributeLength (attr) + 2;
   text = TtaAllocString (length);
   if (text != NULL)
      {
      /* get the value of the position attribute */
      TtaGiveTextAttributeValue (attr, text, &length);
      x = y = 0;
      ptr = text;
      usscanf (ptr, TEXT("%d"), &x);
      ptr = SkipInt (ptr);
      ptr = SkipSep (ptr);
      if (ptr)
         usscanf (ptr, TEXT("%d"), &y);

      /* Search the IntPosX attribute */
      TtaGiveAttributeType (attr, &attrType, &attrKind);
      attrType.AttrTypeNum = GraphML_ATTR_IntPosX;
      attrX = TtaGetAttribute (el, attrType);
      if (attrX == NULL)
        {
        /* create it */
        attrX = TtaNewAttribute (attrType);
        TtaAttachAttribute (el, attrX, doc);
        }
      TtaSetAttributeValue (attrX, x, el, doc);

      /* Search the IntPosY attribute */
      attrType.AttrTypeNum = GraphML_ATTR_IntPosY;
      attrY = TtaGetAttribute (el, attrType);
      if (attrY == NULL)
        {
        /* create it */
        attrY = TtaNewAttribute (attrType);
        TtaAttachAttribute (el, attrY, doc);
        }
      TtaSetAttributeValue (attrY, y, el, doc);
      TtaFreeMemory (text);
      }
}

/*----------------------------------------------------------------------
   ParseCoordAttribute
   Create or update a specific presentation rule for element el that reflects
   the value of the x or y attribute attr.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void      ParseCoordAttribute (Attribute attr, Element el, Document doc)
#else
void      ParseCoordAttribute (attr, el, doc)
Attribute	attr;
Element		el;
Document	doc;

#endif
{
   int                  length, val, attrKind, ruleType;
   STRING               text, ptr;
   AttributeType        attrType;
   PresentationValue    pval;
   PresentationContext  ctxt;

   length = TtaGetTextAttributeLength (attr) + 2;
   text = TtaAllocString (length);
   if (text != NULL)
      {
      /* decide of the presentation rule to be created or updated */
      TtaGiveAttributeType (attr, &attrType, &attrKind);
      if (attrType.AttrTypeNum == GraphML_ATTR_x)
          ruleType = PRHorizPos;
      else if (attrType.AttrTypeNum == GraphML_ATTR_y)
          ruleType = PRVertPos;
      else if (attrType.AttrTypeNum == GraphML_ATTR_dx)
          ruleType = PRHorizPos;
      else if (attrType.AttrTypeNum == GraphML_ATTR_dy)
          ruleType = PRVertPos;
      else
	  return;
      /* get the value of the x or y attribute */
      TtaGiveTextAttributeValue (attr, text, &length);
      ctxt = TtaGetSpecificStyleContext (doc);
      ctxt->destroy = FALSE;
      /* parse the attribute value (a number followed by a unit) */
      ptr = text;
      ptr = TtaSkipWCBlanks (ptr);
      ptr = ParseCSSUnit (ptr, &pval);
      if (pval.typed_data.unit != STYLE_UNIT_INVALID)
	 TtaSetStylePresentation (ruleType, el, NULL, ctxt, pval);
      TtaFreeMemory (ctxt);
      TtaFreeMemory (text);
      }
}

/*----------------------------------------------------------------------
   ParseWidthHeightAttribute
   Create or update a specific presentation rule for element el that reflects
   the value of attribute attr, which is width_ or height_
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void      ParseWidthHeightAttribute (Attribute attr, Element el, Document doc)
#else
void      ParseWidthHeightAttribute (attr, el, doc)
Attribute	attr;
Element		el;
Document	doc;

#endif
{
   AttributeType	attrType;
   int			length, val, attrKind, ruleType;
   STRING		text, ptr;
   PresentationValue    pval;
   PresentationContext  ctxt;

   length = TtaGetTextAttributeLength (attr) + 2;
   text = TtaAllocString (length);
   if (text != NULL)
      {
      /* decide of the presentation rule to be created or updated */
      TtaGiveAttributeType (attr, &attrType, &attrKind);
      if (attrType.AttrTypeNum == GraphML_ATTR_width_)
         ruleType = PRWidth;
      else if (attrType.AttrTypeNum == GraphML_ATTR_height_)
         ruleType = PRHeight;
      else
	 return;
      /* get the value of the width_ or height_ attribute */
      TtaGiveTextAttributeValue (attr, text, &length); 
      ctxt = TtaGetSpecificStyleContext (doc);
      ctxt->destroy = FALSE;
      /* parse the attribute value (a number followed by a unit) */
      ptr = text;
      ptr = TtaSkipWCBlanks (ptr);
      ptr = ParseCSSUnit (ptr, &pval);
      if (pval.typed_data.unit != STYLE_UNIT_INVALID)
	 {
         if (pval.typed_data.value == 0)
	    /* disable rendering of this svg graphics */
	    ruleType = PRVisibility;
	 else
	    {
	    ctxt->destroy = TRUE;
	    TtaSetStylePresentation (PRVisibility, el, NULL, ctxt, pval);
            ctxt->destroy = FALSE;
	    }
	 TtaSetStylePresentation (ruleType, el, NULL, ctxt, pval);
	 }
      TtaFreeMemory (ctxt);
      TtaFreeMemory (text);
      }
}

/*----------------------------------------------------------------------
   GraphMLAttributeComplete
   The XML parser has read attribute attr for element el in document doc.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void      GraphMLAttributeComplete (Attribute attr, Element el, Document doc)
#else
void      GraphMLAttributeComplete (attr, el, doc)
Attribute	attr;
Element		el;
Document	doc;

#endif
{
   AttributeType	attrType;
   Element		leaf;
   int			attrKind, value;
   ThotBool		closed;

   TtaGiveAttributeType (attr, &attrType, &attrKind);

   switch (attrType.AttrTypeNum)
     {
     case GraphML_ATTR_position:
	ParsePositionAttribute (attr, el, doc);
	break;
     case GraphML_ATTR_x:
     case GraphML_ATTR_y:
     case GraphML_ATTR_dx:
     case GraphML_ATTR_dy:
	ParseCoordAttribute (attr, el, doc);
	break;
     case GraphML_ATTR_width_:
     case GraphML_ATTR_height_:
	ParseWidthHeightAttribute (attr, el, doc);
	break;
     case GraphML_ATTR_fill:
     case GraphML_ATTR_stroke:
     case GraphML_ATTR_stroke_width:
        ParseFillStrokeAttributes (attrType.AttrTypeNum, attr, el, doc, FALSE);
	break;
     case GraphML_ATTR_points:
	CreatePoints (attr, el, doc);
	break;

     case GraphML_ATTR_arrowhead:
	value = TtaGetAttributeValue (attr);
	leaf = CreateGraphicLeaf (el, doc, &closed, value);
	break;

     default:
	break;
     }
}

/* end of module */
