/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1998.
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
#include "GraphML.h"
#include "HTML.h"
#include "parser.h"


static AttributeMapping GraphMLAttributeMappingTable[] =
{
   /* The first entry MUST be unknown_attr */
   /* The rest of this table MUST be in alphabetical order */
   {TEXT("unknown_attr"), TEXT(""), 'A', GraphML_ATTR_Invalid_attribute},

   {TEXT("align"), TEXT(""), 'A', GraphML_ATTR_align},
   {TEXT("arrowhead"), TEXT(""), 'A', GraphML_ATTR_arrowhead},
   {TEXT("class"), TEXT(""), 'A', GraphML_ATTR_class},
   {TEXT("depth"), TEXT(""), 'A', GraphML_ATTR_depth_},
   {TEXT("direction"), TEXT(""), 'A', GraphML_ATTR_direction},
   {TEXT("height"), TEXT(""), 'A', GraphML_ATTR_height_},
   {TEXT("hspace"), TEXT(""), 'A', GraphML_ATTR_hspace},
   {TEXT("id"), TEXT(""), 'A', GraphML_ATTR_id},
   {TEXT("linewidth"), TEXT(""), 'A', GraphML_ATTR_linewidth},
   {TEXT("linestyle"), TEXT(""), 'A', GraphML_ATTR_linestyle_},
   {TEXT("link"), TEXT(""), 'A', GraphML_ATTR_link},
   {TEXT("position"), TEXT(""), 'A', GraphML_ATTR_position},
   {TEXT("points"), TEXT(""), 'A', GraphML_ATTR_points},
   {TEXT("style"), TEXT(""), 'A', GraphML_ATTR_style_},
   {TEXT("valign"), TEXT(""), 'A', GraphML_ATTR_valign},
   {TEXT("vspace"), TEXT(""), 'A', GraphML_ATTR_vspace},
   {TEXT("width"), TEXT(""), 'A', GraphML_ATTR_width_},
   {TEXT("zzghost"), TEXT(""), 'A', GraphML_ATTR_Ghost_restruct},

   {TEXT(""), TEXT(""), EOS, 0}		/* Last entry. Mandatory */
};

/* mapping table of attribute values */

static AttrValueMapping GraphMLAttrValueMappingTable[] =
{ 
   {GraphML_ATTR_align, TEXT("center"), GraphML_ATTR_align_VAL_center_},
   {GraphML_ATTR_align, TEXT("left"), GraphML_ATTR_align_VAL_left_},
   {GraphML_ATTR_align, TEXT("right"), GraphML_ATTR_align_VAL_right_},
   {GraphML_ATTR_arrowhead, TEXT("both"), GraphML_ATTR_arrowhead_VAL_both},
   {GraphML_ATTR_arrowhead, TEXT("end"), GraphML_ATTR_arrowhead_VAL_end_},
   {GraphML_ATTR_arrowhead, TEXT("none"), GraphML_ATTR_arrowhead_VAL_none_},
   {GraphML_ATTR_arrowhead, TEXT("start"), GraphML_ATTR_arrowhead_VAL_start},
   {GraphML_ATTR_direction, TEXT("down"), GraphML_ATTR_direction_VAL_down},
   {GraphML_ATTR_direction, TEXT("left"), GraphML_ATTR_direction_VAL_left_},
   {GraphML_ATTR_direction, TEXT("right"), GraphML_ATTR_direction_VAL_right_},
   {GraphML_ATTR_direction, TEXT("up"), GraphML_ATTR_direction_VAL_up},
   {GraphML_ATTR_linestyle_, TEXT("dashed"), GraphML_ATTR_linestyle__VAL_dashed_},
   {GraphML_ATTR_linestyle_, TEXT("dotted"), GraphML_ATTR_linestyle__VAL_dotted_},
   {GraphML_ATTR_linestyle_, TEXT("solid"), GraphML_ATTR_linestyle__VAL_solid_},
   {GraphML_ATTR_link, TEXT("document"), GraphML_ATTR_link_VAL_document},
   {GraphML_ATTR_link, TEXT("extended"), GraphML_ATTR_link_VAL_extended},
   {GraphML_ATTR_link, TEXT("group"), GraphML_ATTR_link_VAL_group},
   {GraphML_ATTR_link, TEXT("locator"), GraphML_ATTR_link_VAL_locator},
   {GraphML_ATTR_link, TEXT("simple"), GraphML_ATTR_link_VAL_simple},
   {GraphML_ATTR_valign, TEXT("bottom"), GraphML_ATTR_valign_VAL_bottom_},
   {GraphML_ATTR_valign, TEXT("middle"), GraphML_ATTR_valign_VAL_middle},
   {GraphML_ATTR_valign, TEXT("top"), GraphML_ATTR_valign_VAL_top_},

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
   else if (ustrcmp (elementName, TEXT("label")) == 0 || ustrcmp (elementName, TEXT("text")) == 0)
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
void           MapGraphMLAttribute (CHAR_T* Attr, AttributeType *attrType, CHAR_T* elementName, Document doc)
#else
void           MapGraphMLAttribute (Attr, attrType, elementName, doc)
CHAR_T*        Attr;
AttributeType* attrType;
CHAR_T*        elementName;
Document       doc;
#endif
{
   int                 i;

   attrType->AttrTypeNum = 0;
   attrType->AttrSSchema = NULL;
   i = 0;
   do
      if (ustrcasecmp (GraphMLAttributeMappingTable[i].XMLattribute, Attr))
	 i++;
      else
	 if (GraphMLAttributeMappingTable[i].XMLelement[0] == EOS)
	       {
	       attrType->AttrTypeNum = GraphMLAttributeMappingTable[i].ThotAttribute;
	       attrType->AttrSSchema = GetGraphMLSSchema (doc);
	       }
	 else if (!ustrcasecmp (GraphMLAttributeMappingTable[i].XMLelement, elementName))
	       {
	       attrType->AttrTypeNum = GraphMLAttributeMappingTable[i].ThotAttribute;
	       attrType->AttrSSchema = GetGraphMLSSchema (doc);
	       }
	 else
	       i++;
   while (attrType->AttrTypeNum <= 0 && GraphMLAttributeMappingTable[i].AttrOrContent != EOS);
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
void	MapGraphMLEntity (STRING entityName, STRING entityValue, int valueLength, STRING alphabet)
#else
void	MapGraphMLEntity (entityName, entityValue, valueLength, alphabet)
STRING entityName;
STRING entityValue;
int valueLength;
STRING alphabet;

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
USTRING entityValue;
Language lang;
STRING entityName;
Document doc;

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
   CHAR_T		oldShape;

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
	 if (changeShape && oldShape != shape)
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
   ParseDirAndSpaceAttributes
   Depending on the values of attributes direction, vspace and hspace
   of element "group", create (or update) attributes IntLeftDistance,
   IntRightDistance, IntUpDistance, IntDownDistance for all children
   if el is NULL or for element el if it's not NULL.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void      ParseDirAndSpaceAttributes (Element group, Element el, Document doc)
#else
void      ParseDirAndSpaceAttributes (group, el, doc)
Element		group;
Element		el;
Document	doc;

#endif
{
   ElementType		elType;
   Element		child;
   AttributeType	attrType, spaceAttrType;
   Attribute		dirAttr, spaceAttr, attr, intAttr;
   int			length, val, dir;
   STRING		text, ptr;
   DisplayMode		dispMode;

   elType = TtaGetElementType (group);
   attrType.AttrSSchema = elType.ElSSchema;
   attrType. AttrTypeNum = GraphML_ATTR_direction;
   dirAttr = TtaGetAttribute (group, attrType);
   if (dirAttr)
      /* Do nothing if attribute "direction" is missing */
      {
      dir = TtaGetAttributeValue (dirAttr);

      dispMode = TtaGetDisplayMode (doc);
      if (dispMode == DisplayImmediately)
         TtaSetDisplayMode (doc, DeferredDisplay);
   
      /* remove existing attributes Int*Distance, position, IntPosX, IntPosY
         from all children */
      if (el)
         child = el;
      else
         child = TtaGetFirstChild (group);
      while (child != NULL)
         {
         attrType. AttrTypeNum = GraphML_ATTR_IntLeftDistance;
         attr = TtaGetAttribute (child, attrType);
         if (attr)
	    TtaRemoveAttribute (child, attr, doc);
         attrType. AttrTypeNum = GraphML_ATTR_IntRightDistance;
         attr = TtaGetAttribute (child, attrType);
         if (attr)
	    TtaRemoveAttribute (child, attr, doc);
         attrType. AttrTypeNum = GraphML_ATTR_IntUpDistance;
         attr = TtaGetAttribute (child, attrType);
         if (attr)
	    TtaRemoveAttribute (child, attr, doc);
         attrType. AttrTypeNum = GraphML_ATTR_IntDownDistance;
         attr = TtaGetAttribute (child, attrType);
         if (attr)
	    TtaRemoveAttribute (child, attr, doc);
         attrType. AttrTypeNum = GraphML_ATTR_position;
         attr = TtaGetAttribute (child, attrType);
         if (attr)
	    TtaRemoveAttribute (child, attr, doc);
         attrType. AttrTypeNum = GraphML_ATTR_IntPosX;
         attr = TtaGetAttribute (child, attrType);
         if (attr)
	    TtaRemoveAttribute (child, attr, doc);
         attrType. AttrTypeNum = GraphML_ATTR_IntPosY;
         attr = TtaGetAttribute (child, attrType);
         if (attr)
	    TtaRemoveAttribute (child, attr, doc);
         if (el)
            child = NULL;
         else
            TtaNextSibling (&child);
         }

      spaceAttrType.AttrSSchema = elType.ElSSchema;
      switch (dir)
	   {
	   case GraphML_ATTR_direction_VAL_left_:
		attrType. AttrTypeNum = GraphML_ATTR_IntLeftDistance;
		spaceAttrType.AttrTypeNum = GraphML_ATTR_hspace;
		break;
	   case GraphML_ATTR_direction_VAL_right_:
		attrType. AttrTypeNum = GraphML_ATTR_IntRightDistance;
		spaceAttrType.AttrTypeNum = GraphML_ATTR_hspace;
		break;
	   case GraphML_ATTR_direction_VAL_up:
		attrType. AttrTypeNum = GraphML_ATTR_IntUpDistance;
		spaceAttrType.AttrTypeNum = GraphML_ATTR_vspace;
		break;
	   case GraphML_ATTR_direction_VAL_down:
		attrType. AttrTypeNum = GraphML_ATTR_IntDownDistance;
		spaceAttrType.AttrTypeNum = GraphML_ATTR_vspace;
		break;
	   }
      val = 0;
      spaceAttr = TtaGetAttribute (group, spaceAttrType);
      if (spaceAttr)
        {
        length = TtaGetTextAttributeLength (spaceAttr) + 2;
        text = TtaAllocString (length);
        if (text != NULL)
          {
          /* get the value of the text attribute */
          TtaGiveTextAttributeValue (spaceAttr, text, &length); 
          /* parse the text value and extract the internal value */
          ptr = text;
          usscanf (ptr, TEXT("%d"), &val);
          ptr = SkipInt (ptr);
          ptr = SkipSep (ptr);
          TtaFreeMemory (text);
          }
        }

      if (el)
         child = el;
      else
         child = TtaGetFirstChild (group);
      while (child != NULL)
         {
         intAttr = TtaNewAttribute (attrType);
         TtaAttachAttribute (child, intAttr, doc);
         TtaSetAttributeValue (intAttr, val, child, doc);
         if (el)
            child = NULL;
         else
            TtaNextSibling (&child);
         }

      TtaSetDisplayMode (doc, dispMode);
      }
}

/*----------------------------------------------------------------------
   GraphMLElementComplete
   Check the Thot structure of the GraphML element el.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void      GraphMLElementComplete (Element el, Document doc)
#else
void      GraphMLElementComplete (el, doc)
Element		el;
Document	doc;

#endif
{
   ElementType		elType, parentType, newType;
   Element		child, parent, new, leaf;
   PRule		fillPatternRule, newPRule;
   SSchema	        GraphMLSSchema;
   ThotBool		closedShape;

   elType = TtaGetElementType (el);
   GraphMLSSchema = GetGraphMLSSchema (doc);
   if (elType.ElSSchema != GraphMLSSchema)
     /* this is not a GraphML element. It's the HTML element <XMLGraphics>, or
	any other element containing a GraphML expression */
     {
     if (TtaGetFirstChild (el) == NULL && !TtaIsLeaf (elType))
	/* this element is empty. Create a GraphML element as it's child */
	{
	elType.ElSSchema = GraphMLSSchema;
	elType.ElTypeNum = GraphML_EL_GraphML;
	new = TtaNewElement (doc, elType);
	TtaInsertFirstChild (&new, el, doc);
	/* Create a placeholder within the GraphML element */
        elType.ElTypeNum = GraphML_EL_GraphicalElement;
	child = TtaNewElement (doc, elType);
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

     /* if it's a Label or Text_ element, create a HTMLfragment to contain
        the HTML elements */
     if (elType.ElTypeNum == GraphML_EL_Label ||
	 elType.ElTypeNum == GraphML_EL_Text_)
	{
	child = TtaGetFirstChild (el);
	if (child != NULL)
	   {
	   elType = TtaGetElementType (child);
	   elType.ElTypeNum = HTML_EL_HTMLfragment;
	   CreateEnclosingElement (child, elType, doc);
	   }
	}
     /* if it's a Group element, process its attributes direction, vspace
        and hspace */
     else if (elType.ElTypeNum == GraphML_EL_Group)
	ParseDirAndSpaceAttributes (el, NULL, doc);
     else
	{
        /* if it's a graphic primitive, create a GRAPHIC_UNIT leaf as a child
	of the element, if it has not been done when creating attributes
	(points, arrowhead) */
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
   ParseWidthHeightAttribute
   Create or update attributes IntWidth or IntHeight according to the
   value of attribute attr, which is width_ or height_
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
   AttributeType	attrType, newAttrType;
   Attribute		intAttr;
   int			attrKind, length, l;
   STRING		text, ptr;

   length = TtaGetTextAttributeLength (attr) + 2;
   text = TtaAllocString (length);
   if (text != NULL)
      {
      /* get the value of the text attribute */
      TtaGiveTextAttributeValue (attr, text, &length); 
      /* decide of the type of the internal attribute to be created or
         updated */
      TtaGiveAttributeType (attr, &attrType, &attrKind);
      newAttrType.AttrSSchema = attrType.AttrSSchema;
      if (attrType.AttrTypeNum == GraphML_ATTR_width_)
          newAttrType.AttrTypeNum = GraphML_ATTR_IntWidth;
      else if (attrType.AttrTypeNum == GraphML_ATTR_height_)
          newAttrType.AttrTypeNum = GraphML_ATTR_IntHeight;
      /* get the internal attribute */
      intAttr = TtaGetAttribute (el, newAttrType);
      if (intAttr == NULL)
        {
        /* it does not exist, create it */
        intAttr = TtaNewAttribute (newAttrType);
        TtaAttachAttribute (el, intAttr, doc);
        }
      /* parse the text value and extract the internal value */
      l = 0;
      ptr = text;
      usscanf (ptr, TEXT("%d"), &l);
      ptr = SkipInt (ptr);
      ptr = SkipSep (ptr);
      /* set the internal value */
      TtaSetAttributeValue (intAttr, l, el, doc);

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
     case GraphML_ATTR_width_:
     case GraphML_ATTR_height_:
	ParseWidthHeightAttribute (attr, el, doc);
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
