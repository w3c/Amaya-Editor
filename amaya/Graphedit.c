#ifdef GRAPHML
/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996-2000
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * This module contains editing functions for handling GraphML objects.
 *
 * Author: I. Vatton
 *	   V. Quint
 */

/* Included headerfiles */
#define THOT_EXPORT
#include "amaya.h"
#include "css.h"
#include "trans.h"
#include "view.h"
 
#include "GraphML.h"
#include "HTML.h"
#ifndef _WINDOWS
#include "Graph.xpm"
#include "GraphNo.xpm"
#include "line.xpm"
#include "rect.xpm"
#include "roundrect.xpm"
#include "circle.xpm"
#include "oval.xpm"
#include "polyline.xpm"
#include "polygon.xpm"
#include "spline.xpm"
#include "closed.xpm"
#include "label.xpm"
#include "text.xpm"
#include "group.xpm"
#endif /* _WINDOWS */
#define FormGraph 0
#define MenuGraph 1
#define MAX_GRAPH 2

static Pixmap   iconGraph;
static Pixmap   iconGraphNo;
static int      GraphButton;
static Pixmap   mIcons[12];
static int      GraphDialogue;
static ThotBool PaletteDisplayed = FALSE;
#define BUFFER_LENGTH 100

#ifdef _WINDOWS
#include "wininclude.h"
#define iconGraph 22
#define iconGraphNo 22
#endif /* _WINDOWS */

#include "fetchXMLname_f.h"
#include "GraphMLbuilder_f.h"
#include "html2thot_f.h"
#include "HTMLpresentation_f.h"
#include "XLinkedit_f.h"

/*----------------------------------------------------------------------
 SetEmptyShapeAttrSubTree
 A GraphML drawing is about to be saved. Set the EmptyShape attribute
 on all geometric shapes that do not contain any other element.
 This attribute is used by the translation schema (GraphMLT.T) to
 generate a closing tag.
 -----------------------------------------------------------------------*/
#ifdef __STDC__
static void SetEmptyShapeAttrSubTree (Element el, Document doc)
#else /* __STDC__*/
static void SetEmptyShapeAttrSubTree(el, doc)
Element     el;
Document    doc;
#endif /* __STDC__*/
{
  ElementType	elType;
  AttributeType	attrType;
  Attribute	attr;
  Element	child, content;
  SSchema	GraphMLSchema;
  ThotBool      empty;

  /* some initialization */
  elType = TtaGetElementType (el);
  GraphMLSchema = elType.ElSSchema;
  attrType.AttrSSchema = GraphMLSchema;
  attrType.AttrTypeNum = GraphML_ATTR_IntEmptyShape;

  /* test all descendants of the element that are in the GraphML namespace */
  child = TtaGetFirstChild (el);
  while (child)
     {
     elType = TtaGetElementType (child);
     if (elType.ElSSchema == GraphMLSchema)
        /* this child is in the GraphML namespace */
	{
        if (elType.ElTypeNum == GraphML_EL_Spline ||
	    elType.ElTypeNum == GraphML_EL_ClosedSpline ||
	    elType.ElTypeNum == GraphML_EL_rect ||
	    elType.ElTypeNum == GraphML_EL_circle ||
	    elType.ElTypeNum == GraphML_EL_ellipse ||
	    elType.ElTypeNum == GraphML_EL_line_ ||
	    elType.ElTypeNum == GraphML_EL_polyline ||
	    elType.ElTypeNum == GraphML_EL_polygon ||
	    elType.ElTypeNum == GraphML_EL_image)
	   /* this element is concerned by the IntEmptyShape attribute */
	   {
	   /* check its children */
	   content = TtaGetFirstChild (child);
	   empty = TRUE;
	   while (content && empty)
	     {
	       elType = TtaGetElementType (content);
	       if (elType.ElSSchema != GraphMLSchema)
		 /* this child is not in the GraphML namespace */
		 empty = FALSE;
	       else
		 if (elType.ElTypeNum != GraphML_EL_GRAPHICS_UNIT &&
		     elType.ElTypeNum != GraphML_EL_XMLcomment)
		   /* this is not a Thot graphics leaf nor a comment */
		   empty = FALSE;
		 else
		   /* check next child */
		   TtaNextSibling (&content);
	     }

	   attr = TtaGetAttribute (child, attrType);
	   if (empty)
	      /* child does not contain any significant element */
	      {
	      if (!attr)
		 /* there is no IntEmptyShape attribute. Create one */
		 {
		 attr = TtaNewAttribute (attrType);
		 TtaAttachAttribute (child, attr, doc);
		 TtaSetAttributeValue (attr,
				       GraphML_ATTR_IntEmptyShape_VAL_yes_,
				       child, doc);
		 }
	      }
	   else
	      /* child is not empty */
	      if (attr)
		 /* there is an IntEmptyShape attribute. Delete it */
		 TtaRemoveAttribute (child, attr, doc);
	   }
	/* test all descendants of that descendant */
	SetEmptyShapeAttrSubTree(child, doc);
	}
     TtaNextSibling (&child);
     }
}

/*----------------------------------------------------------------------
 SetEmptyShapeAttribute
 A GraphML drawing is about to be saved. Set the EmptyShape attribute
 on all geometric shapes that do not contain any other element.
 This attribute is used by the translation schema (GraphMLT.T) to
 generate a closing tag.
 -----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool       SetEmptyShapeAttribute (NotifyElement *event)
#else /* __STDC__*/
ThotBool       SetEmptyShapeAttribute(event)
NotifyElement *event;
#endif /* __STDC__*/
{
  SetEmptyShapeAttrSubTree (event->element, event->document);
  return FALSE; /* let Thot perform normal operation */
}

/*----------------------------------------------------------------------
 ExtendSelectGraphMLElement
 The user wants to add a new element in the current selection.
 -----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool       ExtendSelectGraphMLElement (NotifyElement *event)
#else /* __STDC__*/
ThotBool       ExtendSelectGraphMLElement(event)
NotifyElement *event;
#endif /* __STDC__*/
{
   Element	firstSel, newFirstSel, ancestor, parent, selEl;
   ElementType	elType, ancestType, parentType;
   int		c1, i;
   SSchema	graphSSchema;

   TtaGiveFirstSelectedElement (event->document, &firstSel, &c1, &i);
   if (firstSel == NULL)
      /* the first selected element is not in the same document */
      return TRUE;	/* Don't let Thot perform normal operation */
   if (firstSel == event->element)
      return FALSE;     /* Let Thot perform normal operation */
   /* get the common ancestor */
   ancestor = TtaGetCommonAncestor (firstSel, event->element);
   if (ancestor == NULL)
      return TRUE;	/* Don't let Thot perform normal operation */
   graphSSchema = TtaGetSSchema (TEXT("GraphML"), event->document);
   ancestType = TtaGetElementType (ancestor);
   if (ancestType.ElSSchema != graphSSchema)
      /* common ancestor is not a GraphML element */
      {
      /* is the common ancestor within a GraphML element? */
      parent = ancestor;
      do
	{
	parent = TtaGetParent (parent);
	if (parent != NULL)
	  parentType = TtaGetElementType (parent);
	}
      while (parent != NULL && parentType.ElSSchema != graphSSchema);
      if (parent)
	 /* the common ancestor is within a GraphML element. Let Thot
	    perform normal operation: selection is being extended within
	    a foreignObject */
	 return FALSE;
      else
         return TRUE;	/* abort selection */
      }

   newFirstSel = firstSel;
   elType = TtaGetElementType (firstSel);
   if (elType.ElSSchema != graphSSchema ||
        (elType.ElTypeNum != GraphML_EL_g &&
	 elType.ElTypeNum != GraphML_EL_Spline &&
	 elType.ElTypeNum != GraphML_EL_ClosedSpline &&
	 elType.ElTypeNum != GraphML_EL_rect &&
	 elType.ElTypeNum != GraphML_EL_circle &&
	 elType.ElTypeNum != GraphML_EL_ellipse &&
	 elType.ElTypeNum != GraphML_EL_line_ &&
	 elType.ElTypeNum != GraphML_EL_polyline &&
	 elType.ElTypeNum != GraphML_EL_polygon &&
	 elType.ElTypeNum != GraphML_EL_text_ &&
	 elType.ElTypeNum != GraphML_EL_image &&
	 elType.ElTypeNum != GraphML_EL_foreignObject))
      {
      elType.ElSSchema = graphSSchema;
      elType.ElTypeNum = GraphML_EL_GraphicsElement;
      newFirstSel = TtaGetTypedAncestor (newFirstSel, elType);
      }

   selEl = event->element;
   elType = TtaGetElementType (selEl);
   if (elType.ElSSchema != graphSSchema ||
        (elType.ElTypeNum != GraphML_EL_g &&
	 elType.ElTypeNum != GraphML_EL_Spline &&
	 elType.ElTypeNum != GraphML_EL_ClosedSpline &&
	 elType.ElTypeNum != GraphML_EL_rect &&
	 elType.ElTypeNum != GraphML_EL_circle &&
	 elType.ElTypeNum != GraphML_EL_ellipse &&
	 elType.ElTypeNum != GraphML_EL_line_ &&
	 elType.ElTypeNum != GraphML_EL_polyline &&
	 elType.ElTypeNum != GraphML_EL_polygon &&
	 elType.ElTypeNum != GraphML_EL_text_ &&
	 elType.ElTypeNum != GraphML_EL_image &&
	 elType.ElTypeNum != GraphML_EL_foreignObject))
      {
      elType.ElSSchema = graphSSchema;
      elType.ElTypeNum = GraphML_EL_GraphicsElement;
      selEl = TtaGetTypedAncestor (selEl, elType);
      }

   if (TtaGetParent (newFirstSel) != TtaGetParent (selEl))
     {
     ancestor = TtaGetCommonAncestor (newFirstSel, selEl);
     while (newFirstSel != NULL && TtaGetParent (newFirstSel) != ancestor)
         newFirstSel = TtaGetParent (newFirstSel);
     while (selEl != NULL && TtaGetParent (selEl) != ancestor)
         selEl = TtaGetParent (selEl);
     }

   if (newFirstSel != firstSel)
      TtaSelectElement (event->document, newFirstSel);
   TtaAddElementToSelection (event->document, selEl);
   return TRUE; /* Don't let Thot perform normal operation */
}
 
/*----------------------------------------------------------------------
 AttrCoordChanged
 -----------------------------------------------------------------------*/
#ifdef __STDC__
void             AttrCoordChanged (NotifyAttribute *event)
#else /* __STDC__*/
void             AttrCoordChanged (event)
NotifyAttribute *event;
#endif /* __STDC__*/
{
   ParseCoordAttribute (event->attribute, event->element, event->document);
}

/*----------------------------------------------------------------------
 AttrTransformChanged
 -----------------------------------------------------------------------*/
#ifdef __STDC__
void             AttrTransformChanged (NotifyAttribute *event)
#else /* __STDC__*/
void             AttrTransformChanged (event)
NotifyAttribute *event;
#endif /* __STDC__*/
{
   ParseTransformAttribute (event->attribute, event->element, event->document,
			    FALSE);
}

/*----------------------------------------------------------------------
   AttrTransformDelete : attribute transform will be
   deleted. Remove the corresponding style presentation.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool         AttrTransformDelete (NotifyAttribute * event)
#else
ThotBool         AttrTransformDelete (event)
NotifyAttribute *event;
#endif
{
  ParseTransformAttribute (event->attribute, event->element, event->document,
			  TRUE);
  return FALSE; /* let Thot perform normal operation */
}

/*----------------------------------------------------------------------
 TranslatePointsAttribute
 update attribute "points" for element el according its content
 -----------------------------------------------------------------------*/
#ifdef __STDC__
static void TranslatePointsAttribute (Element el, Document doc, int delta, ThotBool horiz)
#else /* __STDC__*/
static void TranslatePointsAttribute (el, doc, delta, horiz)
Element     el;
Document    doc;
int         delta;
ThotBool    horiz;
#endif /* __STDC__*/
{
  Element		child;
  ElementType		elType;
  AttributeType	        attrType;
  Attribute		attr;
  TypeUnit		unit;
  CHAR_T		buffer[512], buffer1[8];
  int			nbPoints, point, x, y, posX, posY;

  elType = TtaGetElementType (el);
  if (elType.ElTypeNum == GraphML_EL_line_)
    {
#ifdef IV
      if (horiz)
	{
	  /* update the first point */
	  attrType.AttrTypeNum = GraphML_ATTR_x1;
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
	  usprintf (buffer, TEXT("%dpx"), x);
	  TtaSetAttributeText (attr, buffer, el, doc);
	  /* update the last point */
	  attrType.AttrTypeNum = GraphML_ATTR_x2;
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
	  usprintf (buffer, TEXT("%dpx"), x);
	  TtaSetAttributeText (attr, buffer, el, doc);
	}
      else
	{
	  /* update the first point */
	  attrType.AttrTypeNum = GraphML_ATTR_y1;
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
	  usprintf (buffer, TEXT("%dpx"), y);
	  TtaSetAttributeText (attr, buffer, el, doc);
	  /* update the last point */
	  attrType.AttrTypeNum = GraphML_ATTR_y2;
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
	  usprintf (buffer, TEXT("%dpx"), y);
	  TtaSetAttributeText (attr, buffer, el, doc);
	}
#endif /* IV */
    }
  else
    {
      child = TtaGetFirstChild (el);
      if (child != NULL)
	do
	  {
	    elType = TtaGetElementType (child);
	    if (elType.ElTypeNum != GraphML_EL_GRAPHICS_UNIT)
	      TtaNextSibling (&child);
	  }
	while (child != NULL && elType.ElTypeNum != GraphML_EL_GRAPHICS_UNIT);
      
      if (child != NULL)
	{
	  nbPoints = TtaGetPolylineLength (child);
	  if (nbPoints <= 0)
	    /* nothing to do */
	    return;
	  
	  attrType.AttrSSchema = elType.ElSSchema;
	  if (horiz)
	    {
	      posX = delta;
	      posY = 0;
	    }
	  else
	    {
	      posX = 0;
	      posY = delta;
	    }
	  buffer[0] = EOS;
	  for (point = 1; point <= nbPoints; point++)
	    {
	      TtaGivePolylinePoint (child, point, unit, &x, &y);
	      if (point > 1)
		ustrcat (buffer, TEXT(" "));
	      usprintf (buffer1, TEXT("%d"), x + posX);
	      ustrcat (buffer, buffer1);
	      ustrcat (buffer, TEXT(","));
	      usprintf (buffer1, TEXT("%d"), y + posY);
	      ustrcat (buffer, buffer1);
	    }
	  attrType.AttrTypeNum = GraphML_ATTR_points;
	  attr = TtaGetAttribute (el, attrType);
	  if (attr == NULL)
	    {
	      attr = TtaNewAttribute (attrType);
	      TtaAttachAttribute (el, attr, doc);
	    }
	  TtaSetAttributeText (attr, buffer, el, doc);
	}
    }
}

/*----------------------------------------------------------------------
 UpdatePositionAttribute
 update attribute "position" for element el according its attributes
 "IntPosX" and "IntPosY".
 -----------------------------------------------------------------------*/
#ifdef __STDC__
static void UpdatePositionAttribute (Element el, Document doc, int org, int dim, ThotBool horiz)
#else /* __STDC__*/
static void UpdatePositionAttribute (el, doc, org, dim, horiz)
Element     el;
Document    doc;
int         org;
int         dim;;
ThotBool    horiz;
#endif /* __STDC__*/
{
  ElementType		elType;
  AttributeType	        attrType;
  Attribute             attr;
  CHAR_T		buffer[32];

  elType = TtaGetElementType (el);
  attrType.AttrSSchema = elType.ElSSchema;
  if (elType.ElTypeNum == GraphML_EL_circle ||
      elType.ElTypeNum == GraphML_EL_ellipse)
    {
      /* move the center */
      org = org + dim / 2;
      if (horiz)
	attrType.AttrTypeNum = GraphML_ATTR_cx;
      else
	attrType.AttrTypeNum = GraphML_ATTR_cy;
    }
  else if (elType.ElTypeNum == GraphML_EL_rect ||
	   elType.ElTypeNum == GraphML_EL_text_ ||
	   elType.ElTypeNum == GraphML_EL_tspan ||
	   elType.ElTypeNum == GraphML_EL_image ||
	   elType.ElTypeNum == GraphML_EL_foreignObject)
    {
      /* move the origin */
      if (horiz)
	attrType.AttrTypeNum = GraphML_ATTR_x;
      else
	attrType.AttrTypeNum = GraphML_ATTR_y;
    }
  else if (elType.ElTypeNum == GraphML_EL_line_)
    {
      if (horiz)
	attrType.AttrTypeNum = GraphML_ATTR_x1;
      else
	attrType.AttrTypeNum = GraphML_ATTR_y1;
    }
  else
    /* no attribute available */
    return;

  usprintf (buffer, TEXT("%dpx"), org);  
  attr = TtaGetAttribute (el, attrType);
  if (attr == NULL)
    /* element el has no position attribute */
    {
      attr = TtaNewAttribute (attrType);
      TtaAttachAttribute (el, attr, doc);
      TtaSetAttributeText (attr, buffer, el, doc);
      TtaRegisterAttributeCreate (attr, el, doc);
    }
  else
    {
      TtaRegisterAttributeReplace (attr, el, doc);
      TtaSetAttributeText (attr, buffer, el, doc);
    }
}

/*----------------------------------------------------------------------
 AttrWidthHeightChanged
 -----------------------------------------------------------------------*/
#ifdef __STDC__
void             AttrWidthHeightChanged (NotifyAttribute *event)
#else /* __STDC__*/
void             AttrWidthHeightChanged (event)
NotifyAttribute *event;
#endif /* __STDC__*/
{
   ParseWidthHeightAttribute (event->attribute, event->element,
			      event->document, FALSE);
}

/*----------------------------------------------------------------------
 AttrWidthHeightDelete
 -----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool         AttrWidthHeightDelete (NotifyAttribute *event)
#else /* __STDC__*/
ThotBool         AttrWidthHeightDelete (event)
NotifyAttribute *event;
#endif /* __STDC__*/
{
  return ParseWidthHeightAttribute (event->attribute, event->element,
				    event->document, TRUE);
}

/*----------------------------------------------------------------------
 UpdateWidthHeightAttribute
 Attribute "IntWidth" or "IntHeight" has been modified for element el.
 Update the corresponding attribute "height_" or "width_" accordingly.
 -----------------------------------------------------------------------*/
#ifdef __STDC__
static void UpdateWidthHeightAttribute (Element el, Document doc, int dim, ThotBool horiz)
#else /* __STDC__*/
static void UpdateWidthHeightAttribute (el, doc, dim, horiz)
Element     el;
Document    doc;
int         dim;
ThotBool    horiz;
#endif /* __STDC__*/
{
  ElementType		elType;
  AttributeType	        attrType;
  Attribute             attr;
  CHAR_T		buffer[32];

  elType = TtaGetElementType (el);
  attrType.AttrSSchema = elType.ElSSchema;
  if (elType.ElTypeNum == GraphML_EL_circle)
    {
      /* transform into a radius */
      dim /= 2;
      attrType.AttrTypeNum = GraphML_ATTR_r;
    }
  else if (elType.ElTypeNum == GraphML_EL_ellipse)
    {
      /* transform into a radius */
      dim /= 2;
      if (horiz)
	attrType.AttrTypeNum = GraphML_ATTR_rx;
      else
	attrType.AttrTypeNum = GraphML_ATTR_ry;
    }
  else if (elType.ElTypeNum == GraphML_EL_rect ||
	   elType.ElTypeNum == GraphML_EL_text_ ||
	   elType.ElTypeNum == GraphML_EL_tspan ||
	   elType.ElTypeNum == GraphML_EL_image ||
	   elType.ElTypeNum == GraphML_EL_foreignObject ||
	   elType.ElTypeNum == GraphML_EL_Spline ||
	   elType.ElTypeNum == GraphML_EL_ClosedSpline ||
	   elType.ElTypeNum == GraphML_EL_polyline ||
	   elType.ElTypeNum == GraphML_EL_polygon)
    {
      if (horiz)
	attrType.AttrTypeNum = GraphML_ATTR_width_;
      else
	attrType.AttrTypeNum = GraphML_ATTR_height_;
    }
  else if (elType.ElTypeNum == GraphML_EL_line_)
    {
      if (horiz)
	attrType.AttrTypeNum = GraphML_ATTR_x2;
      else
	attrType.AttrTypeNum = GraphML_ATTR_y2;
    }
  else
    /* no attribute available */
    return;

  usprintf (buffer, TEXT("%dpx"), dim);
  attr = TtaGetAttribute (el, attrType);
  if (attr == NULL)
    /* element el has no position attribute */
    {
      attr = TtaNewAttribute (attrType);
      TtaAttachAttribute (el, attr, doc);
      TtaSetAttributeText (attr, buffer, el, doc);
      TtaRegisterAttributeCreate (attr, el, doc);
    }
  else
    {
      TtaRegisterAttributeReplace (attr, el, doc);
      TtaSetAttributeText (attr, buffer, el, doc);
    }
}

/*----------------------------------------------------------------------
 AttrFillStrokeModified
 -----------------------------------------------------------------------*/
#ifdef __STDC__
void             AttrFillStrokeModified(NotifyAttribute *event)
#else /* __STDC__*/
void             AttrFillStrokeModified(event)
NotifyAttribute *event;
#endif /* __STDC__*/
{
  ParseFillStrokeAttributes (event->attributeType.AttrTypeNum,
			     event->attribute, event->element,
			     event->document, FALSE);
}
 
/*----------------------------------------------------------------------
   AttrFillStrokeDelete : attribute fill, stroke or stroke-width will be
   deleted. Remove the corresponding style presentation.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool            AttrFillStrokeDelete (NotifyAttribute * event)
#else
ThotBool            AttrFillStrokeDelete (event)
NotifyAttribute    *event;
#endif
{
  ParseFillStrokeAttributes (event->attributeType.AttrTypeNum,
			     event->attribute, event->element,
			     event->document, TRUE);
  return FALSE; /* let Thot perform normal operation */
}

/*----------------------------------------------------------------------
 GraphElemPasted
 An element has been pasted.
 If the element is an XLink, update the link.
 -----------------------------------------------------------------------*/
#ifdef __STDC__
void           GraphElemPasted (NotifyElement *event)
#else /* __STDC__*/
void           GraphElemPasted(event)
NotifyElement *event;
#endif /* __STDC__*/
{
  XLinkPasted (event);
}

/*----------------------------------------------------------------------
 AttrArrowHeadModified
 -----------------------------------------------------------------------*/
#ifdef __STDC__
void             AttrArrowHeadModified (NotifyAttribute *event)
#else /* __STDC__*/
void             AttrArrowHeadModified (event)
NotifyAttribute *event;
#endif /* __STDC__*/
{
  int		value;
  ThotBool	closed;

  if (!event->attribute)
     value = GraphML_ATTR_arrowhead_VAL_none_;
  else
     value = TtaGetAttributeValue (event->attribute);
  (void) CreateGraphicLeaf (event->element, event->document, &closed, value);
}

/*----------------------------------------------------------------------
 DeleteAttrPoints
 -----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool         DeleteAttrPoints (NotifyAttribute *event)
#else /* __STDC__*/
ThotBool         DeleteAttrPoints (event)
NotifyAttribute *event;
#endif /* __STDC__*/
{
  /* prevents Thot from deleting the points attribute */
  return TRUE;
}
 

/*----------------------------------------------------------------------
 AttrPointsModified
 -----------------------------------------------------------------------*/
#ifdef __STDC__
void             AttrPointsModified (NotifyAttribute *event)
#else /* __STDC__*/
void             AttrPointsModified (event)
NotifyAttribute *event;
#endif /* __STDC__*/
{
  CreatePoints (event->attribute, event->element, event->document);
}

/*----------------------------------------------------------------------
 GraphicsPRuleChange
 A presentation rule is going to be changed by Thot.
 -----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool            GraphicsPRuleChange (NotifyPresentation *event)
#else /* __STDC__*/
ThotBool            GraphicsPRuleChange (event)
NotifyPresentation *event;
#endif /* __STDC__*/
{
  Element       el, span;
  PRule         presRule;
  Document      doc;
  ElementType   elType;
  int           presType;
  int           mainView, unit;
  int           x, y, width, height;
  ThotBool      ret;
 
  ret = FALSE; /* let Thot perform normal operation */
  el = event->element;
  elType = TtaGetElementType (el);
  doc = event->document;
  if (elType.ElSSchema != GetGraphMLSSchema (doc))
    return (ret); /* let Thot perform normal operation */

  presType = event->pRuleType;
  if (presType != PRHeight      &&  presType != PRWidth      &&
      presType != PRVertPos     &&  presType != PRHorizPos   &&
      presType != PRSize        &&  presType != PRStyle      &&
      presType != PRWeight      &&  presType != PRFont       &&
      presType != PRLineStyle   &&  presType != PRLineWeight &&
      presType != PRBackground  &&  presType != PRForeground &&
      presType != PRFillPattern)
     return (TRUE);   /* don't let Thot do it */

  presRule = event->pRule;
 
  if (TtaGetConstruct (el) == ConstructBasicType)
    /* it's a basic type. Move the PRule to the parent element */
    {
      if (MakeASpan (el, &span, doc))
	{
	MovePRule (presRule, el, span, doc, FALSE);
        el = span;
	}
      else
	{
        el = TtaGetParent (el);
        MovePRule (presRule, event->element, el, doc, FALSE);
	}
    }

  if (presType == PRSize        ||  presType == PRStyle      ||
      presType == PRWeight      ||  presType == PRFont       ||
      presType == PRLineStyle   ||  presType == PRLineWeight ||
      presType == PRBackground  ||  presType == PRForeground ||
      presType == PRFillPattern)
    {
      SetStyleAttribute (doc, el);
      TtaSetDocumentModified (doc);
    }

  else if (presType == PRVertPos || presType == PRHorizPos ||
           presType == PRHeight ||  presType == PRWidth)
    {
    unit = TtaGetPRuleUnit (presRule);
    mainView = TtaGetViewFromName (doc, "Formatted_view");
    /* TtaGiveBoxPosition (el, doc, mainView, unit, &x, &y);*/
    TtaGiveBoxSize (el, doc, 1, unit, &width, &height);
    if (presType == PRVertPos)
      {
	if (elType.ElTypeNum == GraphML_EL_Spline ||
	    elType.ElTypeNum == GraphML_EL_ClosedSpline ||
	    elType.ElTypeNum == GraphML_EL_polyline ||
	    elType.ElTypeNum == GraphML_EL_polygon ||
	    elType.ElTypeNum == GraphML_EL_line_)
	  TranslatePointsAttribute (el, doc, y, FALSE);
	else
	  {
	  /* the new value is the old one plus the difference */
	    y = TtaGetPRuleValue (presRule);
	    UpdatePositionAttribute (el, doc, y, height, FALSE);
	  }
      }
    else if (presType == PRHorizPos)
      {
	if (elType.ElTypeNum == GraphML_EL_Spline ||
	    elType.ElTypeNum == GraphML_EL_ClosedSpline ||
	    elType.ElTypeNum == GraphML_EL_polyline ||
	    elType.ElTypeNum == GraphML_EL_polygon ||
	    elType.ElTypeNum == GraphML_EL_line_)
	  TranslatePointsAttribute (el, doc, x, FALSE);
	else
	  {
	    /* the new value is the old one plus the difference */
	    x = TtaGetPRuleValue (presRule);
	    UpdatePositionAttribute (el, doc, x, width, TRUE);
	}
      }
    else if (presType == PRHeight &&
	     (elType.ElTypeNum == GraphML_EL_Spline ||
	      elType.ElTypeNum == GraphML_EL_ClosedSpline ||
	      elType.ElTypeNum == GraphML_EL_rect ||
	      elType.ElTypeNum == GraphML_EL_ellipse ||
	      elType.ElTypeNum == GraphML_EL_polyline ||
	      elType.ElTypeNum == GraphML_EL_polygon ||
	      elType.ElTypeNum == GraphML_EL_line_))
      {
	/* the new value is the old one plus the delta */
	height = TtaGetPRuleValue (presRule);
	UpdateWidthHeightAttribute (el, doc, height, FALSE);
      }
    else if (presType == PRWidth &&
	     (elType.ElTypeNum == GraphML_EL_Spline ||
	      elType.ElTypeNum == GraphML_EL_ClosedSpline ||
	      elType.ElTypeNum == GraphML_EL_rect ||
	      elType.ElTypeNum == GraphML_EL_circle ||
	      elType.ElTypeNum == GraphML_EL_ellipse ||
	      elType.ElTypeNum == GraphML_EL_polyline ||
	      elType.ElTypeNum == GraphML_EL_polygon ||
	      elType.ElTypeNum == GraphML_EL_line_))
      {
	/* the new value is the old one plus the delta */
	width = TtaGetPRuleValue (presRule);
	UpdateWidthHeightAttribute (el, doc, width, TRUE);
      }
    }
  return ret; /* let Thot perform normal operation */
}
/*----------------------------------------------------------------------
   ControlPointChanged
   A control point has been changed in a polyline, a polygon,
   a Spline or a ClosedSpline.  Update the points attribute.
 -----------------------------------------------------------------------*/
#ifdef __STDC__
void ControlPointChanged (NotifyOnValue *event)
#else /* __STDC__*/
void ControlPointChanged(event)
     NotifyOnValue *event;
#endif /* __STDC__*/
{
  /*************
   int	minX, minY, maxX, maxY;

   UpdatePointsAttribute (event->element, event->document, &minX, &minY, &maxX,
			  &maxY);
  ***************/
}

/*----------------------------------------------------------------------
 GraphLeafDeleted
 A GRAPHICS_UNIT element has been deleted. Delete its siblings
 and its parent.
 -----------------------------------------------------------------------*/
#ifdef __STDC__
void GraphLeafDeleted (NotifyElement *event)
#else /* __STDC__*/
void GraphLeafDeleted(event)
     NotifyElement *event;
#endif /* __STDC__*/
{
   /* don't delete anything if event is sent by Undo */
   if (!event->info)
      TtaDeleteTree (event->element, event->document);
}

/*----------------------------------------------------------------------
 ExportForeignObject
 A foreignObject element will be generated in the output file.
 Associate a Namespace attribute with its child. This attribute will be
 generated with the child.
 -----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool ExportForeignObject (NotifyElement *event)
#else /* __STDC__*/
ThotBool ExportForeignObject (event)
     NotifyElement *event;
#endif /* __STDC__*/
{
  Element       child;
  ElementType   elType;
  Attribute     attr;
  AttributeType attrType;

  child = TtaGetFirstChild (event->element);
  while (child)
    {
      elType = TtaGetElementType (child);
      if (!ustrcmp (TtaGetSSchemaName (elType.ElSSchema), TEXT("HTML")))
	 /* child is an HTML element */
	{
        attrType.AttrTypeNum = GraphML_ATTR_Namespace;
        attrType.AttrSSchema = TtaGetElementType (event->element).ElSSchema;
	attr = TtaNewAttribute (attrType);
	TtaAttachAttribute (child, attr, event->document);
        TtaSetAttributeText (attr, TEXT("http://www.w3.org/1999/xhtml"), child,
			     event->document);
	}
      TtaNextSibling (&child);
    }
  return FALSE; /* let Thot perform normal operation */
}

/*----------------------------------------------------------------------
 NameSpaceGenerated
 An attribute Namespace has been generated for a child of a foreign
 element. Delete that attribute.
 -----------------------------------------------------------------------*/
#ifdef __STDC__
void NameSpaceGenerated (NotifyAttribute *event)
#else /* __STDC__*/
void NameSpaceGenerated (event)
     NotifyAttribute *event;
#endif /* __STDC__*/
{
   TtaRemoveAttribute (event->element, event->attribute, event->document);
}

/*----------------------------------------------------------------------
   CreateGraphicElement
   Create a Graphics element.
   entry is the number of the entry chosen by the user in the Graphics
   palette.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         CreateGraphicElement (int entry)
#else
static void         CreateGraphicElement (entry)
int                 construct;
 
#endif
{
   Document	    doc;
   Element	    last, first, graphRoot, newEl, sibling, selEl;
   Element          child, parent, elem;
   ElementType      elType, selType, newType, childType;
   AttributeType    attrType;
   Attribute        attr;
   SSchema	    docSchema, graphSchema;
   DisplayMode      dispMode;
   char		    shape;
   STRING           name;
   int		    c1, c2, i, j, w, h;
   int	            oldStructureChecking;
   ThotBool	    found;

   doc = TtaGetSelectedDocument ();
   if (doc == 0)
      /* there is no selection. Nothing to do */
      return;
   TtaGiveLastSelectedElement (doc, &last, &c2, &j);
   TtaGiveFirstSelectedElement (doc, &first, &c1, &i);
   selEl = first;
   newEl = NULL;
   child = NULL;

   /* Are we in a drawing? */
   docSchema = TtaGetDocumentSSchema (doc);
   graphSchema = GetGraphMLSSchema (doc);
   elType.ElTypeNum = GraphML_EL_GraphML;
   elType.ElSSchema = graphSchema;
   attrType.AttrSSchema = graphSchema;
   graphRoot = TtaGetTypedAncestor (first, elType);
   if (graphRoot == NULL)
      /* the current selection is not in a GraphML element, create one */
      {
      selType = TtaGetElementType (first);
      name = TtaGetSSchemaName (selType.ElSSchema);
      if (ustrcmp (name, TEXT("HTML")))
	 /* selection is not in an HTML element. */
         return;
      graphSchema = TtaNewNature (doc, docSchema, TEXT("GraphML"), TEXT("GraphMLP"));
      TtaCreateElement (elType, doc);
      TtaGiveFirstSelectedElement (doc, &graphRoot, &c1, &i);
      }

   /* look for the element (sibling) in front of which the new element will be
      created */
   sibling = first;
   found = FALSE;
   do
	{
         parent = TtaGetParent (sibling);
	 if (parent)
	    {
	    elType = TtaGetElementType (parent);
	    if (elType.ElSSchema == graphSchema &&
		(elType.ElTypeNum == GraphML_EL_g ||
		 elType.ElTypeNum == GraphML_EL_GraphML))
		found = TRUE;
	    else
		sibling = parent;
	    }
	}
   while (parent && !found);

   if (!parent)
      {
      parent = graphRoot;
      sibling = TtaGetFirstChild (graphRoot);
      }

   TtaOpenUndoSequence (doc, first, last, c1, c2);

   newType.ElSSchema = graphSchema;
   newType.ElTypeNum = 0;
   shape = EOS;

   switch (entry)
    {
    case 0:	/* line */
	newType.ElTypeNum = GraphML_EL_line_;
	shape = '\\';
	break;
    case 1:	/* rectangle */
	newType.ElTypeNum = GraphML_EL_rect;
	shape = 'C';
	break;
    case 2:	/* rectangle with rounded corners */
	newType.ElTypeNum = GraphML_EL_rect;
	shape = 'C';
	break;
    case 3:	/* circle */
	newType.ElTypeNum = GraphML_EL_circle;
	shape = 'a';
	break;
    case 4:	/* ellipse */
	newType.ElTypeNum = GraphML_EL_ellipse;
	shape = 'c';
	break;
    case 5:	/* polyline */
	newType.ElTypeNum = GraphML_EL_polyline;
	shape = 'S';
	break;
    case 6:	/* polygon */
	newType.ElTypeNum = GraphML_EL_polygon;
	shape = 'p';
	break;
    case 7:	/* spline */
	newType.ElTypeNum = GraphML_EL_Spline;
	shape = 'B';
	break;
    case 8:	/* closed spline */
	newType.ElTypeNum = GraphML_EL_ClosedSpline;
	shape = 's';
	break;
    case 9:	/* foreignObject with some HTML code */
        newType.ElTypeNum = GraphML_EL_foreignObject;
	break;
    case 10:	/* text */
	newType.ElTypeNum = GraphML_EL_text_;
	break;
    case 11:	/* group */
	newType.ElTypeNum = 0;
	break;
    default:
	break;
    }

   if (newType.ElTypeNum > 0)
     {
       dispMode = TtaGetDisplayMode (doc);
       /* ask Thot to stop displaying changes made in the document */
       if (dispMode == DisplayImmediately)
         TtaSetDisplayMode (doc, DeferredDisplay);

       /* for rectangles, circle, ellipse, and text, ask for an elastic box */
       if (newType.ElTypeNum == GraphML_EL_rect ||
	   newType.ElTypeNum == GraphML_EL_circle ||
	   newType.ElTypeNum == GraphML_EL_ellipse ||
	   newType.ElTypeNum == GraphML_EL_text_ ||
	   newType.ElTypeNum == GraphML_EL_foreignObject)
	 TtaAskFirstCreation ();
       /* create the new element */
       newEl = TtaNewElement (doc, newType);
       if (!sibling)
         TtaInsertFirstChild (&newEl, parent, doc);
       else
	 TtaInsertSibling (newEl, sibling, TRUE, doc);

       /* create a child for the new element */
       if (shape != EOS)
         /* create a graphic leaf according to the element's type */
	 {
	   childType.ElSSchema = graphSchema;
	   childType.ElTypeNum = GraphML_EL_GRAPHICS_UNIT;
	   child = TtaNewElement (doc, childType);
	   TtaInsertFirstChild (&child, newEl, doc);
	   TtaSetGraphicsShape (child, shape, doc);
	   selEl = child;
	   if (entry == 2)
	     /* rectangle with rounded corners */
	     {
	       /* create a default rx attribute */
	       attrType.AttrTypeNum = GraphML_ATTR_rx;
	       attr = TtaNewAttribute (attrType);
	       TtaAttachAttribute (newEl, attr, doc);
	       TtaSetAttributeText (attr, TEXT("5px"), newEl, doc);
	       ParseWidthHeightAttribute (attr, newEl, doc, FALSE);
	     }
	 }
       else if (newType.ElTypeNum == GraphML_EL_text_)
	 /* create a TEXT leaf */
	 {
	   childType.ElSSchema = graphSchema;
	   childType.ElTypeNum = GraphML_EL_TEXT_UNIT;
	   child = TtaNewElement (doc, childType);
	   TtaInsertFirstChild (&child, newEl, doc);
	   selEl = child;
	 }
       else if (newType.ElTypeNum == GraphML_EL_foreignObject)
	 /* create an HTML DIV element in the new element */
	 {
	   /* the document is supposed to be HTML */
	   childType.ElSSchema = TtaNewNature (doc, docSchema, TEXT("HTML"),
					       TEXT("HTMLP"));
	   childType.ElTypeNum = HTML_EL_Division;
	   child = TtaNewTree (doc, childType, "");
	   /* do not check the Thot abstract tree against the structure */
	   /* schema when inserting this element */
	   oldStructureChecking = TtaGetStructureChecking (doc);
	   TtaSetStructureChecking (0, doc);
	   TtaInsertFirstChild (&child, newEl, doc);
	   TtaSetStructureChecking (oldStructureChecking, doc);
	   /* select the first leaf */
	   elem = child;
	   do
	     {
	       selEl = elem;
	       elem = TtaGetFirstChild (elem);
	     }
	   while (elem != NULL);
	 }
       TtaRegisterElementCreate (newEl, doc);

       /* ask Thot to display changes made in the document */
       TtaSetDisplayMode (doc, dispMode);
     }

   if (selEl != NULL)
     /* select the right element */
     TtaSelectElement (doc, selEl);
   
   if (shape == 'S' || shape == 'p' || shape == 'B' || shape == 's')
     /* multipoints element. Let the user enter the points */
     {
       TtaGiveBoxSize (parent, doc, 1, UnPoint, &w, &h);

       TtaChangeLimitOfPolyline (child, UnPoint, w, h, doc);
       TtcInsertGraph (doc, 1, shape);
       dispMode = TtaGetDisplayMode (doc);
       /* ask Thot to stop displaying changes made in the document */
       if (dispMode == DisplayImmediately)
         TtaSetDisplayMode (doc, DeferredDisplay);
       /************
       UpdatePointsAttribute (newEl, doc, &minX, &minY, &maxX, &maxY);
       UpdatePositionOfPoly (newEl, child, doc, minX, minY, maxX, maxY);
       *************/
       /* ask Thot to display changes made in the document */
       TtaSetDisplayMode (doc, dispMode);
     }
   TtaCloseUndoSequence (doc);
   TtaSetDocumentModified (doc);
}

/*----------------------------------------------------------------------
   CreateGroup
   Create a g element surrounding the selected elements
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         CreateGroup ()
#else
static void         CreateGroup ()
#endif
{
   Document	doc;
   Element	el, prevSel, prevChild, group;
   ElementType	elType;
   AttributeType	attrType;
   int		c1, i, minX, minY;
   /******
   Attribute	attr;
   int          posX, poxY;
   *********/
   DisplayMode	dispMode;
   ThotBool	position;

   doc = TtaGetSelectedDocument ();
   if (doc == 0)
      /* there is no selection. Nothing to do */
      return;
   TtaGiveFirstSelectedElement (doc, &el, &c1, &i);
   if (el == NULL)
      /* no selection. Return */
      return;

   dispMode = TtaGetDisplayMode (doc);
   /* ask Thot to stop displaying changes made in the document */
   if (dispMode == DisplayImmediately)
      TtaSetDisplayMode (doc, DeferredDisplay);

   prevSel = NULL;
   prevChild = NULL;
   /* Create a Group element */
   elType = TtaGetElementType (el);
   elType.ElTypeNum = GraphML_EL_g;
   group = TtaNewElement (doc, elType);
   /* insert the new group element */
   TtaInsertSibling (group, el, TRUE, doc);

   attrType.AttrSSchema = elType.ElSSchema;
   minX = minY = 32000;
   position = FALSE;
   while (el != NULL)
      {
/***************
      attrType.AttrTypeNum = GraphML_ATTR_IntPosX;
      attr = TtaGetAttribute (el, attrType);
      if (attr == NULL)
         posX = 0;
      else
	 {
         posX = TtaGetAttributeValue (attr);
	 position = TRUE;
	 }
      if (posX < minX)
	 minX = posX;

      attrType.AttrTypeNum = GraphML_ATTR_IntPosY;
      attr = TtaGetAttribute (el, attrType);
      if (attr == NULL)
         posY = 0;
      else
	 {
         posY = TtaGetAttributeValue (attr);
	 position = TRUE;
	 }
      if (posY < minY)
	 minY = posY;
******************/

      if (prevSel != NULL)
	 {
	 TtaRemoveTree (prevSel, doc);
	 if (prevChild == NULL)
	    TtaInsertFirstChild (&prevSel, group, doc);
	 else
	    TtaInsertSibling (prevSel, prevChild, FALSE, doc);
	 prevChild = prevSel;
	 }
      prevSel = el;
      TtaGiveNextSelectedElement (doc, &el, &c1, &i);
      }
   if (prevSel != NULL)
      {
      TtaRemoveTree (prevSel, doc);
      if (prevChild == NULL)
	 TtaInsertFirstChild (&prevSel, group, doc);
      else
         TtaInsertSibling (prevSel, prevChild, FALSE, doc);      
      }

/****************
   if (position)
     {
     attrType.AttrTypeNum = GraphML_ATTR_IntPosX;
     attr = TtaNewAttribute (attrType);
     TtaAttachAttribute (group, attr, doc);
     TtaSetAttributeValue (attr, minX, group, doc);
     attrType.AttrTypeNum = GraphML_ATTR_IntPosY;
     attr = TtaNewAttribute (attrType);
     TtaAttachAttribute (group, attr, doc);
     TtaSetAttributeValue (attr, minY, group, doc);

     UpdatePositionAttribute (attr, group, doc);
     el = TtaGetFirstChild (group);
     while (el != NULL)
        {
        attrType.AttrTypeNum = GraphML_ATTR_IntPosX;
        attr = TtaGetAttribute (el, attrType);
        if (attr != NULL)
	   {
           posX = TtaGetAttributeValue (attr);
	   TtaSetAttributeValue (attr, posX - minX, el, doc);
	   }
        attrType.AttrTypeNum = GraphML_ATTR_IntPosY;
        attr = TtaGetAttribute (el, attrType);
        if (attr != NULL)
	   {
           posY = TtaGetAttributeValue (attr);
	   TtaSetAttributeValue (attr, posY - minY, el, doc);
	   }
        UpdatePositionAttribute (attr, el, doc);
        TtaNextSibling (&el);
        }
     }
*******************/
   /* ask Thot to display changes made in the document */
   TtaSetDisplayMode (doc, dispMode);
}


/*----------------------------------------------------------------------
   CallbackGraph: manage Graph dialogue events.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         CallbackGraph (int ref, int typedata, STRING data)
#else
static void         CallbackGraph (ref, typedata, data)
int                 ref;
int                 typedata;
STRING              data;
 
#endif
{
  Document           doc;
 
  switch (ref - GraphDialogue)
    {
    case FormGraph:
      /* the user has clicked the DONE button in the Graphics dialog box */
      PaletteDisplayed = FALSE;
      TtaDestroyDialogue (ref);    
      break;
 
    case MenuGraph:
      /* the user has selected an entry in the Graphics palette */
      doc = TtaGetSelectedDocument ();
      if (doc > 0)
        /* there is a selection */
	if ((int) data == 11)
	   CreateGroup ();
	else
           CreateGraphicElement ((int) data);
      break;
 
    default:
      break;
    }
}

/*----------------------------------------------------------------------
   InitGraphML initializes GraphML context.
  ----------------------------------------------------------------------*/
void                InitGraphML ()
{
#  ifndef _WINDOWS
   iconGraph = TtaCreatePixmapLogo (Graph_xpm);
   iconGraphNo = TtaCreatePixmapLogo (GraphNo_xpm);
   mIcons[0] = TtaCreatePixmapLogo (line_xpm);
   mIcons[1] = TtaCreatePixmapLogo (rect_xpm);
   mIcons[2] = TtaCreatePixmapLogo (roundrect_xpm);
   mIcons[3] = TtaCreatePixmapLogo (circle_xpm);
   mIcons[4] = TtaCreatePixmapLogo (oval_xpm);
   mIcons[5] = TtaCreatePixmapLogo (polyline_xpm);
   mIcons[6] = TtaCreatePixmapLogo (polygon_xpm);
   mIcons[7] = TtaCreatePixmapLogo (spline_xpm);
   mIcons[8] = TtaCreatePixmapLogo (closed_xpm);
   mIcons[9] = TtaCreatePixmapLogo (label_xpm);
   mIcons[10] = TtaCreatePixmapLogo (text_xpm);
   mIcons[11] = TtaCreatePixmapLogo (group_xpm);
#  endif /* _WINDOWS */
   GraphDialogue = TtaSetCallback (CallbackGraph, MAX_GRAPH);
}

/*----------------------------------------------------------------------
   ShowGraphicsPalette displays the Graphics palette
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         ShowGraphicsPalette (Document doc, View view)
#else
static void         ShowGraphicsPalette (doc, view)
Document            doc;
View                view;
#endif
{
   if (!TtaGetDocumentAccessMode (doc))
     /* the document is in ReadOnly mode */
     return;

# ifndef _WINDOWS
  if (!PaletteDisplayed)
    {
      PaletteDisplayed = TRUE;
 
      /* Dialogue box for the graphics palette */
      TtaNewSheet (GraphDialogue + FormGraph, TtaGetViewFrame (doc, view),
                   TtaGetMessage (AMAYA, AM_BUTTON_GRAPHICS),
                   0, NULL, TRUE, 1, 'L', D_DONE);
      TtaNewIconMenu (GraphDialogue + MenuGraph, GraphDialogue + FormGraph, 0,
                   NULL, 12, mIcons, FALSE);
      TtaSetMenuForm (GraphDialogue + MenuGraph, 0);
      TtaSetDialoguePosition ();
    }
  TtaShowDialogue (GraphDialogue + FormGraph, TRUE);
# else /* _WINDOWS */
  CreateGraphicsDlgWindow (GraphDialogue, FormGraph, MenuGraph, TtaGetThotWindow (GetWindowNumber (doc, view)));
# endif /* _WINDOWS */
}
#endif /* GRAPHML */

/*----------------------------------------------------------------------
   AddGraphicsButton    
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                AddGraphicsButton (Document doc, View view)
#else
void                AddGraphicsButton (doc, view)
Document            doc;
View                view;
#endif
{
#ifdef GRAPHML
  GraphButton = TtaAddButton (doc, 1, iconGraph, ShowGraphicsPalette, "ShowGraphicsPalette",
			      TtaGetMessage (AMAYA, AM_BUTTON_GRAPHICS),
			      TBSTYLE_BUTTON, TRUE);
#endif /* GRAPHML */
}

/*----------------------------------------------------------------------
  SwitchIconGraph
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void              SwitchIconGraph (Document doc, View view, ThotBool state)
#else  /* __STDC__ */
void              SwitchIconGraph (doc, view, state)
Document          doc;
 View             view;
ThotBool          state;
#endif /* __STDC__ */
{
#ifdef GRAPHML
  if (state)
    TtaChangeButton (doc, view, GraphButton, iconGraph, state);
  else
    TtaChangeButton (doc, view, GraphButton, iconGraphNo, state);
#endif /* GRAPHML */
}

/*----------------------------------------------------------------------
   SVGCreated
   An svg element has been created.
   It has at least two attributes (width and height) that are made
   mandatory by the S schema. Parse the value of these attributes.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                SVGCreated (NotifyElement * event)
#else  /* __STDC__ */
void                SVGCreated (event)
NotifyElement      *event;

#endif /* __STDC__ */
{
  ElementType	elType;
  AttributeType	attrType;
  Attribute	attr;

  elType = TtaGetElementType (event->element);
  attrType.AttrSSchema = elType.ElSSchema;
  attrType.AttrTypeNum = GraphML_ATTR_width_;
  attr = TtaGetAttribute (event->element, attrType);
  if (attr)
     ParseWidthHeightAttribute (attr, event->element, event->document, FALSE);
  attrType.AttrTypeNum = GraphML_ATTR_height_;
  attr = TtaGetAttribute (event->element, attrType);
  if (attr)
     ParseWidthHeightAttribute (attr, event->element, event->document, FALSE);
}

/*----------------------------------------------------------------------
   TspanCreated
   A tspan element has been created by the user hitting a Enter key
   witihn a text element. Create attributes x and dy.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TspanCreated (NotifyElement * event)
#else  /* __STDC__ */
void                TspanCreated (event)
NotifyElement      *event;

#endif /* __STDC__ */
{
  ElementType	elType;
  AttributeType	attrType;
  Attribute	attr;

  elType = TtaGetElementType (event->element);
  attrType.AttrSSchema = elType.ElSSchema;

  attrType.AttrTypeNum = GraphML_ATTR_x;
  attr = TtaGetAttribute (event->element, attrType);
  if (!attr)
    {
      attr = TtaNewAttribute (attrType);
      TtaAttachAttribute (event->element, attr, event->document);
    }
  TtaSetAttributeText (attr, TEXT("0"), event->element, event->document);
  ParseCoordAttribute (attr, event->element, event->document);

  attrType.AttrTypeNum = GraphML_ATTR_dy;
  attr = TtaGetAttribute (event->element, attrType);
  if (!attr)
    {
      attr = TtaNewAttribute (attrType);
      TtaAttachAttribute (event->element, attr, event->document);
    }
  TtaSetAttributeText (attr, TEXT("1em"), event->element, event->document);
  ParseCoordAttribute (attr, event->element, event->document);
}
