#ifdef GRAPHML
/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996-2001
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

static Pixmap   iconGraph;
static Pixmap   iconGraphNo;
static int      GraphButton;
static Pixmap   mIcons[12];
static ThotBool PaletteDisplayed = FALSE;
static ThotBool InCreation = FALSE;
#define oldHrefMaxLen 400
static  CHAR_T  oldXlinkHrefValue[oldHrefMaxLen];

#define BUFFER_LENGTH 100

#ifdef _WINDOWS
#include "wininclude.h"
#define iconGraph 22
#define iconGraphNo 22
#endif /* _WINDOWS */

#include "EDITimage_f.h"
#include "fetchXMLname_f.h"
#include "GraphMLbuilder_f.h"
#include "html2thot_f.h"
#include "HTMLactions_f.h"
#include "HTMLpresentation_f.h"
#include "init_f.h"
#include "XLinkedit_f.h"

/*----------------------------------------------------------------------
 SetEmptyShapeAttrSubTree
 A GraphML drawing is about to be saved. Set the IntEmptyShape attribute
 on all geometric shapes that do not contain any other element.
 This attribute is used by the translation schema (GraphMLT.T) to
 generate a closing tag.
 -----------------------------------------------------------------------*/
static void SetEmptyShapeAttrSubTree (Element el, Document doc)
{
  ElementType	elType, childType;
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
        if (elType.ElTypeNum == GraphML_EL_path ||
	    elType.ElTypeNum == GraphML_EL_rect ||
	    elType.ElTypeNum == GraphML_EL_circle ||
	    elType.ElTypeNum == GraphML_EL_ellipse ||
	    elType.ElTypeNum == GraphML_EL_line_ ||
	    elType.ElTypeNum == GraphML_EL_polyline ||
	    elType.ElTypeNum == GraphML_EL_polygon ||
	    elType.ElTypeNum == GraphML_EL_use_ ||
	    elType.ElTypeNum == GraphML_EL_image)
	   /* this element is concerned by the IntEmptyShape attribute */
	   {
	   /* check its children */
	   content = TtaGetFirstChild (child);
	   empty = TRUE;
	   while (content && empty)
	     {
	       childType = TtaGetElementType (content);
	       if (childType.ElSSchema != GraphMLSchema)
		 /* this child is not in the GraphML namespace */
		 empty = FALSE;
	       else
		 {
		 if (childType.ElTypeNum != GraphML_EL_GRAPHICS_UNIT &&
		     childType.ElTypeNum != GraphML_EL_XMLcomment)
		   {
		   /* this is not a Thot graphics leaf nor a comment */
		   if (elType.ElTypeNum != GraphML_EL_use_)
		      empty = FALSE;
	           else
		      /* in a use element, transcluded elements don't count */
		      if (!TtaIsTranscludedElement (content))
			 empty = FALSE;
		   }
		 }
	       if (empty)
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
ThotBool       SetEmptyShapeAttribute (NotifyElement *event)
{
  SetEmptyShapeAttrSubTree (event->element, event->document);
  return FALSE; /* let Thot perform normal operation */
}

/*----------------------------------------------------------------------
   A new element has been selected. Synchronize selection in source view.      
  ----------------------------------------------------------------------*/
void                GraphicsSelectionChanged (NotifyElement * event)
{
   SynchronizeSourceView (event);
}

/*----------------------------------------------------------------------
 ExtendSelectGraphMLElement
 The user wants to add a new element in the current selection.
 -----------------------------------------------------------------------*/
ThotBool       ExtendSelectGraphMLElement (NotifyElement *event)
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
	 elType.ElTypeNum != GraphML_EL_path &&
	 elType.ElTypeNum != GraphML_EL_rect &&
	 elType.ElTypeNum != GraphML_EL_circle &&
	 elType.ElTypeNum != GraphML_EL_ellipse &&
	 elType.ElTypeNum != GraphML_EL_line_ &&
	 elType.ElTypeNum != GraphML_EL_polyline &&
	 elType.ElTypeNum != GraphML_EL_polygon &&
	 elType.ElTypeNum != GraphML_EL_text_ &&
	 elType.ElTypeNum != GraphML_EL_image &&
	 elType.ElTypeNum != GraphML_EL_foreignObject &&
	 elType.ElTypeNum != GraphML_EL_GraphML))
      {
      elType.ElSSchema = graphSSchema;
      elType.ElTypeNum = GraphML_EL_GraphicsElement;
      newFirstSel = TtaGetTypedAncestor (newFirstSel, elType);
      }

   selEl = event->element;
   elType = TtaGetElementType (selEl);
   if (elType.ElSSchema != graphSSchema ||
        (elType.ElTypeNum != GraphML_EL_g &&
	 elType.ElTypeNum != GraphML_EL_path &&
	 elType.ElTypeNum != GraphML_EL_rect &&
	 elType.ElTypeNum != GraphML_EL_circle &&
	 elType.ElTypeNum != GraphML_EL_ellipse &&
	 elType.ElTypeNum != GraphML_EL_line_ &&
	 elType.ElTypeNum != GraphML_EL_polyline &&
	 elType.ElTypeNum != GraphML_EL_polygon &&
	 elType.ElTypeNum != GraphML_EL_text_ &&
	 elType.ElTypeNum != GraphML_EL_image &&
	 elType.ElTypeNum != GraphML_EL_foreignObject &&
	 elType.ElTypeNum != GraphML_EL_GraphML))
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
void             AttrCoordChanged (NotifyAttribute *event)
{
   ParseCoordAttribute (event->attribute, event->element, event->document);
}

/*----------------------------------------------------------------------
 AttrTransformChanged
 -----------------------------------------------------------------------*/
void             AttrTransformChanged (NotifyAttribute *event)
{
   ParseTransformAttribute (event->attribute, event->element, event->document,
			    FALSE);
}

/*----------------------------------------------------------------------
   AttrTransformDelete : attribute transform will be
   deleted. Remove the corresponding style presentation.
  ----------------------------------------------------------------------*/
ThotBool         AttrTransformDelete (NotifyAttribute * event)
{
  ParseTransformAttribute (event->attribute, event->element, event->document,
			  TRUE);
  return FALSE; /* let Thot perform normal operation */
}

/*----------------------------------------------------------------------
 TranslatePointsAttribute
 update attribute "points" for element el according its content
 -----------------------------------------------------------------------*/
static void TranslatePointsAttribute (Element el, Document doc, int delta, TypeUnit unit, ThotBool horiz)
{
  Element		child;
  ElementType		elType;
  AttributeType	        attrType;
  Attribute		attr;
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
  UpdateAttrText creates or updates the text attribute attr of the
  element el.
  The parameter delta is TRUE when the value is 
  The parameter update is TRUE when the attribute must be parsed after the change.
 -----------------------------------------------------------------------*/
static void   UpdateAttrText (Element el, Document doc, AttributeType attrType, int value, ThotBool delta, ThotBool update)
{
  CHAR_T		buffer[32], unit[32];
  Attribute             attr;
  int                   v, e;
  int                   pval, pe, i;

  attr = TtaGetAttribute (el, attrType);
  if (attr == NULL)
    {
      /* it's a new attribute */
      attr = TtaNewAttribute (attrType);
      TtaAttachAttribute (el, attr, doc);

      /* by default generate pixel values */
      usprintf (buffer, TEXT("%dpx"), value);
      TtaSetAttributeText (attr, buffer, el, doc);
      TtaRegisterAttributeCreate (attr, el, doc);
    }
  else
    {
      /* get the current unit */
      i = 32;
      TtaGiveTextAttributeValue (attr, buffer, &i);
      /* check if the value includes decimals */
      i = 0;
      v = 0;
      while (buffer[i] != WC_EOS && !v)
	{
	  if (buffer[i] == TEXT('.'))
	    {
	      buffer[i] = WC_EOS;
	      usscanf (buffer, TEXT("%d"), &pval);	      
	      v = i + 1;
	    }
	  i++;
	}
      unit[0] = WC_EOS;
      pe = 0;
      if (v)
	usscanf (&buffer[v], TEXT("%d%s"), &pe, unit);
      else
	usscanf (buffer, TEXT("%d%s"), &pval, unit);
      /* convert the value according to the current unit */
      if (!ustrcmp (unit, "em") || !ustrcmp (unit, "ex"))
	{
	  v = value / 10;
	  e = value - (v * 10);
	}
      else if (!ustrcmp (unit, "pc"))
	{
	  v = value / 12;
	  e = value - (v * 12);
	}
      else if (!ustrcmp (unit, "in"))
	{
	  v = value / 72;
	  e = value - (v * 72);
	}
      else if (!ustrcmp (unit, "cm"))
	{
	  v = (value + 14) / 28;
	  e = value - (v * 28);
	}
      else if (!ustrcmp (unit, "mm"))
	{
	  value = value * 10;
	  v = (value + 14) / 28;
	  e = value - (v * 28);
	}
      else
	{
	  v = value;
	  e = 0;
	}
      if (delta)
	{
	  v = v + pval;
	  e = e + pe;
	}
      if (e > 0)
	usprintf (buffer, TEXT("%d.%d%s"), v, e, unit);
      else
	usprintf (buffer, TEXT("%d%s"), v, unit);
      TtaRegisterAttributeReplace (attr, el, doc);
      TtaSetAttributeText (attr, buffer, el, doc);
    }
  if (update)
    {
      /* generate the specific presentation */
      if (attrType.AttrTypeNum == GraphML_ATTR_x ||
	  attrType.AttrTypeNum == GraphML_ATTR_y ||
	  attrType.AttrTypeNum == GraphML_ATTR_cx ||
	  attrType.AttrTypeNum == GraphML_ATTR_cy ||
	  attrType.AttrTypeNum == GraphML_ATTR_x1 ||
	  attrType.AttrTypeNum == GraphML_ATTR_y1 ||
	  attrType.AttrTypeNum == GraphML_ATTR_x2 ||
	  attrType.AttrTypeNum == GraphML_ATTR_y2 ||
	  attrType.AttrTypeNum == GraphML_ATTR_dx ||
	  attrType.AttrTypeNum == GraphML_ATTR_dy)
	ParseCoordAttribute (attr, el, doc);
      else
	ParseWidthHeightAttribute (attr, el, doc, FALSE);
    }
}


/*----------------------------------------------------------------------
 UpdatePositionAttribute
 update attribute "position" for element el according its attributes
 "IntPosX" and "IntPosY".
 -----------------------------------------------------------------------*/
static void UpdatePositionAttribute (Element el, Document doc, int org, int dim, ThotBool horiz)
{
  ElementType		elType;
  AttributeType	        attrType;

  elType = TtaGetElementType (el);
  attrType.AttrSSchema = elType.ElSSchema;
  if (elType.ElTypeNum == GraphML_EL_circle ||
      elType.ElTypeNum == GraphML_EL_ellipse)
    {
      if (horiz)
	attrType.AttrTypeNum = GraphML_ATTR_cx;
      else
	attrType.AttrTypeNum = GraphML_ATTR_cy;
    }
  else if (elType.ElTypeNum == GraphML_EL_rect ||
	   elType.ElTypeNum == GraphML_EL_text_ ||
	   elType.ElTypeNum == GraphML_EL_tspan ||
	   elType.ElTypeNum == GraphML_EL_image ||
	   elType.ElTypeNum == GraphML_EL_foreignObject ||
	   elType.ElTypeNum == GraphML_EL_GraphML)
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

  UpdateAttrText (el, doc, attrType, org, FALSE, FALSE);
}

/*----------------------------------------------------------------------
 AttrWidthHeightChanged
 -----------------------------------------------------------------------*/
void             AttrWidthHeightChanged (NotifyAttribute *event)
{
   ParseWidthHeightAttribute (event->attribute, event->element,
			      event->document, FALSE);
}

/*----------------------------------------------------------------------
 AttrWidthHeightDelete
 -----------------------------------------------------------------------*/
ThotBool         AttrWidthHeightDelete (NotifyAttribute *event)
{
  return ParseWidthHeightAttribute (event->attribute, event->element,
				    event->document, TRUE);
}

/*----------------------------------------------------------------------
 UpdateWidthHeightAttribute
 Attribute "IntWidth" or "IntHeight" has been modified for element el.
 Update the corresponding attribute "height_" or "width_" accordingly.
 -----------------------------------------------------------------------*/
static void UpdateWidthHeightAttribute (Element el, Document doc, int dim, ThotBool horiz)
{
  ElementType		elType;
  AttributeType	        attrType;

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
	   elType.ElTypeNum == GraphML_EL_GraphML ||
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

  UpdateAttrText (el, doc, attrType, dim, FALSE, FALSE);
}

/*----------------------------------------------------------------------
 AttrFillStrokeModified
 -----------------------------------------------------------------------*/
void             AttrFillStrokeModified(NotifyAttribute *event)
{
  ParseFillStrokeAttributes (event->attributeType.AttrTypeNum,
			     event->attribute, event->element,
			     event->document, FALSE);
}
 
/*----------------------------------------------------------------------
   AttrFillStrokeDelete : attribute fill, stroke or stroke-width will be
   deleted. Remove the corresponding style presentation.
  ----------------------------------------------------------------------*/
ThotBool            AttrFillStrokeDelete (NotifyAttribute * event)
{
  ParseFillStrokeAttributes (event->attributeType.AttrTypeNum,
			     event->attribute, event->element,
			     event->document, TRUE);
  return FALSE; /* let Thot perform normal operation */
}

/*----------------------------------------------------------------------
 AttrArrowHeadModified
 -----------------------------------------------------------------------*/
void             AttrArrowHeadModified (NotifyAttribute *event)
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
ThotBool         DeleteAttrPoints (NotifyAttribute *event)
{
  /* prevents Thot from deleting the points attribute */
  return TRUE;
}
 

/*----------------------------------------------------------------------
 AttrPointsModified
 -----------------------------------------------------------------------*/
void             AttrPointsModified (NotifyAttribute *event)
{
  CreatePoints (event->attribute, event->element, event->document);
}

/*----------------------------------------------------------------------
  CheckGraphMLRoot checks that the svg root element includes element el.
 -----------------------------------------------------------------------*/
void             CheckGraphMLRoot (Document doc, Element el)
{
  Element          graphRoot, child;
  ElementType      elType;
  AttributeType    attrType;
  SSchema	   graphSchema;
  PRule            rule;
  TypeUnit         unit;
  int              x, y, w, h, val;
  int              wR, hR, dummy;
  int              dw,dh;

  dw = 0;
  dh = 0;
  graphSchema = GetGraphMLSSchema (doc);
  elType.ElTypeNum = GraphML_EL_GraphML;
  elType.ElSSchema = graphSchema;
  attrType.AttrSSchema = graphSchema;
  graphRoot = TtaGetTypedAncestor (el, elType);
  while (graphRoot)
    {
      /* check first the position of the new element */
      TtaGiveBoxPosition (el, doc, 1, UnPixel, &x, &y);
      /* get the unit of the SVG width */
      rule = TtaGetPRule (graphRoot, PRWidth);
      if (rule)
	unit = TtaGetPRuleUnit (rule);
      else
	unit = UnPixel;
      dh = dw = 0;
      TtaGiveBoxSize (graphRoot, doc, 1, unit, &wR, &dummy);
      elType = TtaGetElementType (el);
      if (x < 0 && elType.ElTypeNum != GraphML_EL_tspan)
	{
	  /* translate the whole SVG contents */
	  child = TtaGetFirstChild (graphRoot);
	  val = -x;
	  while (child)
	    {
	      elType = TtaGetElementType (child);
	      if (elType.ElTypeNum == GraphML_EL_polyline ||
		  elType.ElTypeNum == GraphML_EL_polygon)
		TranslatePointsAttribute (el, doc, val, unit, TRUE);
	      else
		{
		  if (elType.ElTypeNum == GraphML_EL_circle ||
		      elType.ElTypeNum == GraphML_EL_ellipse)
		    attrType.AttrTypeNum = GraphML_ATTR_cx;
		  else if (elType.ElTypeNum == GraphML_EL_rect ||
			   elType.ElTypeNum == GraphML_EL_text_ ||
			   elType.ElTypeNum == GraphML_EL_tspan ||
			   elType.ElTypeNum == GraphML_EL_image ||
			   elType.ElTypeNum == GraphML_EL_foreignObject ||
			   elType.ElTypeNum == GraphML_EL_GraphML)
		    attrType.AttrTypeNum = GraphML_ATTR_x;
		  else if (elType.ElTypeNum == GraphML_EL_line_)
		    {
		      attrType.AttrTypeNum = GraphML_ATTR_x1;
		      UpdateAttrText (child, doc, attrType, val, TRUE, TRUE);
		      attrType.AttrTypeNum = GraphML_ATTR_x2;
		    }
		  else
		    attrType.AttrTypeNum = 0;
		  if (attrType.AttrTypeNum != 0)
		    UpdateAttrText (child, doc, attrType, val, TRUE, TRUE);
		}
	      /* check if the SVG width includes that element */
	      TtaGiveBoxPosition (child, doc, 1, unit, &x, &dummy);
	      TtaGiveBoxSize (child, doc, 1, unit, &w, &h);
	      TtaGiveBoxSize (graphRoot, doc, 1, unit, &wR, &hR);
	      dw = w + x - wR;
	      if (dw > 0)
		{
		  /* increase the width of the SVG element */
		  attrType.AttrTypeNum = GraphML_ATTR_width_;
		  UpdateAttrText (graphRoot, doc, attrType, x + w, FALSE, TRUE);
		}
	      /* next element */
	      TtaNextSibling (&child);
	    }
	}
      else if ( elType.ElTypeNum != GraphML_EL_line_)
	{
	  /* check if the SVG width includes that element */
	  TtaGiveBoxPosition (el, doc, 1, unit, &x, &dummy);
	  TtaGiveBoxSize (el, doc, 1, unit, &w, &h);
	  dw = w + x - wR;
	  if (dw > 0)
	    {
	      /* increase the width of the SVG element */
	      attrType.AttrTypeNum = GraphML_ATTR_width_;
	      UpdateAttrText (graphRoot, doc, attrType, x + w, FALSE, TRUE);
	    }
	}
      /* get the unit of the SVG width */
      rule = TtaGetPRule (graphRoot, PRHeight);
      if (rule)
	unit = TtaGetPRuleUnit (rule);
      else
	unit = UnPixel;
      TtaGiveBoxSize (graphRoot, doc, 1, unit, &dummy, &hR);
      elType = TtaGetElementType (el);
      if (y < 0 && elType.ElTypeNum != GraphML_EL_tspan)
	{
	  /* translate the whole SVG contents */
	  child = TtaGetFirstChild (graphRoot);
	  val = -y;
	  while (child)
	    {
	      elType = TtaGetElementType (child);
	      if (elType.ElTypeNum == GraphML_EL_polyline ||
		  elType.ElTypeNum == GraphML_EL_polygon)
		TranslatePointsAttribute (el, doc, val, unit, FALSE);
	      else
		{
		  if (elType.ElTypeNum == GraphML_EL_circle ||
		      elType.ElTypeNum == GraphML_EL_ellipse)
		    attrType.AttrTypeNum = GraphML_ATTR_cy;
		  else if (elType.ElTypeNum == GraphML_EL_rect ||
			   elType.ElTypeNum == GraphML_EL_text_ ||
			   elType.ElTypeNum == GraphML_EL_tspan ||
			   elType.ElTypeNum == GraphML_EL_image ||
			   elType.ElTypeNum == GraphML_EL_foreignObject ||
			   elType.ElTypeNum == GraphML_EL_GraphML)
		    attrType.AttrTypeNum = GraphML_ATTR_y;
		  else if (elType.ElTypeNum == GraphML_EL_line_)
		    {
		      attrType.AttrTypeNum = GraphML_ATTR_y1;
		      UpdateAttrText (child, doc, attrType, val, TRUE, TRUE);
		      attrType.AttrTypeNum = GraphML_ATTR_y2;
		    }
		  else
		    attrType.AttrTypeNum = 0;
		  if (attrType.AttrTypeNum != 0)
		    UpdateAttrText (child, doc, attrType, val, TRUE, TRUE);
		}
	      /* check if the SVG height includes that element */
	      TtaGiveBoxPosition (child, doc, 1, unit, &dummy, &y);
	      TtaGiveBoxSize (child, doc, 1, unit, &w, &h);
	      TtaGiveBoxSize (graphRoot, doc, 1, unit, &wR, &hR);
	      dh = h + y - hR;
	      if (dh > 0)
		{
		  /* increase the height of the root element */
		  attrType.AttrTypeNum = GraphML_ATTR_height_;
		  UpdateAttrText (graphRoot, doc, attrType, y + h, FALSE, TRUE);
		}
	      /* next element */
	      TtaNextSibling (&child);
	    }
	}
      else if ( elType.ElTypeNum != GraphML_EL_line_)
	{
	  /* check if the SVG height includes that element */
	  TtaGiveBoxPosition (el, doc, 1, unit, &dummy, &y);
	  TtaGiveBoxSize (el, doc, 1, unit, &w, &h);
	  dh = h + y - hR;
	  if (dh > 0)
	    {
	      /* increase the height of the root element */
	      attrType.AttrTypeNum = GraphML_ATTR_height_;
	      UpdateAttrText (graphRoot, doc, attrType, y + h, FALSE, TRUE);
	    }
	}

      if (dw > 0 || dh > 0)
	{
	  if (dw < 0)
	    dw = 0;
	  if (dh < 0)
	    dh = 0;
	  /* handle included polylines */
	  child = TtaGetFirstChild (graphRoot);
	  while (child)
	    {
	      elType = TtaGetElementType (child);
	      if (elType.ElTypeNum == GraphML_EL_polyline ||
		  elType.ElTypeNum == GraphML_EL_polygon)
		UpdatePositionOfPoly (child, doc, 0, 0, wR + dw, hR + dh);
	      /* next element */
	      TtaNextSibling (&child);
	    }
	}

      /* check enclosing SGV */
      el = TtaGetParent (graphRoot);
      if (el)
	graphRoot = TtaGetTypedAncestor (el, elType);
      else
	graphRoot = NULL;
    }
}

/*----------------------------------------------------------------------
 GraphElemPasted
 An element has been pasted.
 If the element is an XLink, update the link.
 -----------------------------------------------------------------------*/
void           GraphElemPasted (NotifyElement *event)
{
  XLinkPasted (event);
  /* check that the svg element includes that element */
  CheckGraphMLRoot (event->document, event->element);
  SetGraphicDepths (event->document, event->element);
}

/*----------------------------------------------------------------------
 GraphicsPRuleChanged
 A presentation rule is going to be changed by Thot.
 -----------------------------------------------------------------------*/
void                GraphicsChanged (NotifyOnValue *event)
{
  if (InCreation)
    /* don't check anything during the creation */
    return;

    /* check that the enclosing svg element still encompasses the
       element whose size or position has just been changed */
  CheckGraphMLRoot (event->document, event->element);
}

/*----------------------------------------------------------------------
 GraphicsPRuleChange
 A presentation rule is going to be changed by Thot.
 -----------------------------------------------------------------------*/
ThotBool            GraphicsPRuleChange (NotifyPresentation *event)
{
  Element       el, span;
  PRule         presRule;
  Document      doc;
  ElementType   elType;
  TypeUnit      unit;
  int           presType;
  int           mainView;
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

  if (presType == PRSize      ||  presType == PRStyle      ||
      presType == PRWeight    ||  presType == PRFont       ||
      presType == PRLineStyle ||  presType == PRLineWeight ||
      presType == PRBackground || presType == PRForeground ||
      presType == PRFillPattern)
    {
      SetStyleAttribute (doc, el);
      TtaSetDocumentModified (doc);
    }
  else if (presType == PRVertPos || presType == PRHorizPos ||
           presType == PRHeight  || presType == PRWidth)
    {
      unit = TtaGetPRuleUnit (presRule);
      mainView = TtaGetViewFromName (doc, "Formatted_view");
      TtaGiveBoxSize (el, doc, 1, unit, &width, &height);
      if (presType == PRVertPos)
	{
	  /* the new value is the old one plus the difference */
	  y = TtaGetPRuleValue (presRule);
	  if (elType.ElTypeNum == GraphML_EL_polyline ||
	      elType.ElTypeNum == GraphML_EL_polygon)
	    TranslatePointsAttribute (el, doc, y, unit, FALSE);
	  else
	    UpdatePositionAttribute (el, doc, y, height, FALSE);
	}
      else if (presType == PRHorizPos)
	{
	  /* the new value is the old one plus the difference */
	  x = TtaGetPRuleValue (presRule);
	  if (elType.ElTypeNum == GraphML_EL_polyline ||
	      elType.ElTypeNum == GraphML_EL_polygon)
	    TranslatePointsAttribute (el, doc, x, unit, TRUE);
	  else
	    UpdatePositionAttribute (el, doc, x, width, TRUE);
	}
      else if (presType == PRHeight &&
	       (elType.ElTypeNum == GraphML_EL_rect ||
		elType.ElTypeNum == GraphML_EL_ellipse ||
		elType.ElTypeNum == GraphML_EL_polyline ||
		elType.ElTypeNum == GraphML_EL_polygon ||
		elType.ElTypeNum == GraphML_EL_line_ ||
		elType.ElTypeNum == GraphML_EL_image ||
		elType.ElTypeNum == GraphML_EL_foreignObject))
	{
	  /* the new value is the old one plus the delta */
	  height = TtaGetPRuleValue (presRule);
	  UpdateWidthHeightAttribute (el, doc, height, FALSE);
	}
      else if (presType == PRWidth &&
	       (elType.ElTypeNum == GraphML_EL_rect ||
		elType.ElTypeNum == GraphML_EL_circle ||
		elType.ElTypeNum == GraphML_EL_ellipse ||
		elType.ElTypeNum == GraphML_EL_polyline ||
		elType.ElTypeNum == GraphML_EL_polygon ||
		elType.ElTypeNum == GraphML_EL_line_ ||
		elType.ElTypeNum == GraphML_EL_image ||
		elType.ElTypeNum == GraphML_EL_foreignObject))
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
   A control point has been changed in a polyline or a polygon.
   Update the points attribute.
 -----------------------------------------------------------------------*/
void ControlPointChanged (NotifyOnValue *event)
{
  Element         el, child;
  Document        doc;
  ElementType     elType;
  AttributeType	  attrType;
  Attribute       attr;
  STRING          text, buffer;
  int             i, length;
  int             x, y;

  el = event->element;
  elType = TtaGetElementType (el);
  doc = event->document;
  attrType.AttrSSchema = elType.ElSSchema;
  if (elType.ElSSchema != GetGraphMLSSchema (doc))
    return;
  if (elType.ElTypeNum == GraphML_EL_polyline ||
      elType.ElTypeNum == GraphML_EL_polygon)
     {
	child = TtaGetFirstChild (el);
	length = TtaGetPolylineLength (child);
	/* get all points */
	i = 1;
	buffer = TtaAllocString (20);
	text = TtaAllocString (length * 20);
	text[0] = EOS;
	while (i <= length)
	  {
	     TtaGivePolylinePoint (child, i, UnPixel, &x, &y);
	     usprintf (buffer, TEXT("%d,%d"), x, y);
	     ustrcat (text, buffer);
	     if (i < length)
	       ustrcat (text, TEXT(" "));
	     i++;
	  }
	TtaFreeMemory (buffer);
	attrType.AttrTypeNum = GraphML_ATTR_points;
	attr = TtaGetAttribute (el, attrType);
	if (attr == NULL)
	  {
	    /* it's a new attribute */
	    attr = TtaNewAttribute (attrType);
	    TtaAttachAttribute (el, attr, doc);
	  }
	TtaSetAttributeText (attr, text, el, doc);
	TtaFreeMemory (text);
     }
  if (!InCreation)
    /* don't check anything during the creation */
    /* check that the enclosing svg element still encompasses the
       element whose size or position has just been changed */
    CheckGraphMLRoot (doc, el);
}

/*----------------------------------------------------------------------
 GraphLeafDeleted
 A GRAPHICS_UNIT element has been deleted. Delete its siblings
 and its parent.
 -----------------------------------------------------------------------*/
void GraphLeafDeleted (NotifyElement *event)
{
   /* don't delete anything if event is sent by Undo */
   if (!event->info)
      TtaDeleteTree (event->element, event->document);
}

/*----------------------------------------------------------------------
 PastePicture
 -----------------------------------------------------------------------*/
ThotBool PastePicture (NotifyOnValue *event)
{
  /* code to be written */
  return FALSE; /* let Thot perform normal operation */
}

/*----------------------------------------------------------------------
 ExportForeignObject
 A foreignObject element will be generated in the output file.
 Associate a Namespace attribute with its child. This attribute will be
 generated with the child.
 -----------------------------------------------------------------------*/
ThotBool ExportForeignObject (NotifyElement *event)
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
void             NameSpaceGenerated (NotifyAttribute *event)
{
   TtaRemoveAttribute (event->element, event->attribute, event->document);
}

/*----------------------------------------------------------------------
   CreateGraphicElement
   Create a Graphics element.
   entry is the number of the entry chosen by the user in the Graphics
   palette.
  ----------------------------------------------------------------------*/
void         CreateGraphicElement (int entry)
{
  Document	    doc;
  Element	    first, graphRoot, newEl, sibling, selEl;
  Element           child, parent, elem;
  ElementType       elType, selType, newType, childType;
  AttributeType     attrType;
  Attribute         attr;
  SSchema	    docSchema, graphSchema;
  DisplayMode       dispMode;
  char		    shape;
  STRING            name;
  int		    c1, i, w, h;
  int	            oldStructureChecking;
  int               docModified;
  ThotBool	    found, newGraph = FALSE;

  doc = TtaGetSelectedDocument ();
  if (doc == 0)
    /* there is no selection. Nothing to do */
    return;
  TtaGiveFirstSelectedElement (doc, &first, &c1, &i);
  TtaOpenUndoSequence (doc, NULL, NULL, 0, 0);
  selEl = first;
  newEl = NULL;
  child = NULL;
  docModified = TtaIsDocumentModified (doc);
  /* Are we in a drawing? */
  docSchema = TtaGetDocumentSSchema (doc);
  graphSchema = GetGraphMLSSchema (doc);
  attrType.AttrSSchema = graphSchema;
  elType = TtaGetElementType (selEl);
  if (elType.ElTypeNum == GraphML_EL_GraphML && elType.ElSSchema == graphSchema)
    graphRoot = selEl;
  else
    {
      elType.ElTypeNum = GraphML_EL_GraphML;
      elType.ElSSchema = graphSchema;
      graphRoot = TtaGetTypedAncestor (first, elType);
      if (graphRoot == NULL)
	/* the current selection is not in a GraphML element, create one */
	{
	  selType = TtaGetElementType (first);
	  name = TtaGetSSchemaName (selType.ElSSchema);
	  if (ustrcmp (name, TEXT("HTML")))
	    {
	      /* selection is not in an HTML element. */
	      TtaCancelLastRegisteredSequence (doc);
	      return;
	    }
	  graphSchema = TtaNewNature (doc, docSchema, TEXT("GraphML"), TEXT("GraphMLP"));
	  if (TtaIsSelectionEmpty ())
	    {
	      /* try to create the SVG here */
	      TtaCreateElement (elType, doc);
	      TtaGiveFirstSelectedElement (doc, &graphRoot, &c1, &i);
	    }
	  else
	    {
	      /* look for a position around */
	      parent = first;
	      do
		{
		  first = parent;
		  parent = TtaGetParent (first);
		  selType = TtaGetElementType (parent);
		}
	      while (selType.ElTypeNum != HTML_EL_BODY &&
		     selType.ElTypeNum != HTML_EL_Division );
	      
	      /* create and insert a SVG element here */
	      graphRoot = TtaNewElement (doc, elType);
	      TtaInsertSibling (graphRoot, first, FALSE, doc);
	      first = graphRoot;
	      newGraph = TRUE;
	    }
	}
    }

  /* look for the element (sibling) in front of which the new element will be
     created */
  if (first == graphRoot)
    parent = NULL;
  else
    {
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
    }

  if (!parent)
    {
      parent = graphRoot;
      sibling = TtaGetLastChild (graphRoot);
    }

  newType.ElSSchema = graphSchema;
  newType.ElTypeNum = 0;
  shape = EOS;

  switch (entry)
    {
    case 0:	/* line */
      newType.ElTypeNum = GraphML_EL_line_;
      shape = 'g';
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
      /* newType.ElTypeNum = GraphML_EL_path;        ********
	 shape = 'B'; */
      break;
    case 8:	/* closed spline */
      /* newType.ElTypeNum = GraphML_EL_path;        ********
	 shape = 's'; */
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
  InCreation = TRUE;
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
	TtaInsertSibling (newEl, sibling, FALSE, doc);
      
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
	  attrType.AttrTypeNum = GraphML_ATTR_width_;
	  UpdateAttrText (newEl, doc, attrType, 100, FALSE, TRUE);
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
      if (newGraph)
	TtaRegisterElementCreate (graphRoot, doc);
      else
	TtaRegisterElementCreate (newEl, doc);

      /* ask Thot to display changes made in the document */
      TtaSetDisplayMode (doc, dispMode);
    }
  
  if (selEl != NULL)
    /* select the right element */
    TtaSelectElement (doc, selEl);
  
  if (shape == 'S' || shape == 'p' || shape == 'B' || shape == 's' || shape == 'g')
    /* multipoints element. Let the user enter the points */
    {
      if (shape != 'g')
	{
	  TtaGiveBoxSize (parent, doc, 1, UnPixel, &w, &h);
	  TtaChangeLimitOfPolyline (child, UnPixel, w, h, doc);
	}
      TtcInsertGraph (doc, 1, shape);
      if (shape != 'g' && TtaGetVolume (selEl) < 3)
	{
	  /* the polyline doesn't have enough points */
	  TtaDeleteTree (newEl, doc);
	  TtaCancelLastRegisteredSequence (doc);
	  if (!docModified)
	    TtaSetDocumentUnmodified (doc);
	  TtaSelectElement (doc, first);
	  InCreation = FALSE;
	  return;
	}
    }
  /* adapt the size of the SVG root element if necessary */
  InCreation = FALSE;
  CheckGraphMLRoot (doc, newEl);
  SetGraphicDepths (doc, graphRoot);
  TtaCloseUndoSequence (doc);
  TtaSetDocumentModified (doc);
  if (newType.ElTypeNum == 0)
    InitInfo (TEXT("      "), TtaGetMessage (AMAYA, AM_NOT_AVAILABLE));
}

/*----------------------------------------------------------------------
   CreateGroup
   Create a g element surrounding the selected elements
  ----------------------------------------------------------------------*/
static void         CreateGroup ()
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
static void         CallbackGraph (int ref, int typedata, STRING data)
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
	{
	  /* there is a selection */
	  if ((int) data == 11)
	    CreateGroup ();
	  else
	    CreateGraphicElement ((int) data);
	}
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
static void         ShowGraphicsPalette (Document doc, View view)
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
  CreateGraphicsDlgWindow (TtaGetThotWindow (GetWindowNumber (doc, view)));
# endif /* _WINDOWS */
}
#endif /* GRAPHML */

/*----------------------------------------------------------------------
   AddGraphicsButton    
  ----------------------------------------------------------------------*/
void                AddGraphicsButton (Document doc, View view)
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
void              SwitchIconGraph (Document doc, View view, ThotBool state)
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
void                SVGCreated (NotifyElement * event)
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
void                TspanCreated (NotifyElement * event)
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

/*----------------------------------------------------------------------
   AttrXlinkHrefWillBeChanged: attribute xlink:href will be modified.
   Keep its initial value in case an invalid value be entered.
  ----------------------------------------------------------------------*/
ThotBool            AttrXlinkHrefWillBeChanged (NotifyAttribute * event)
{
   Element             el;
   int                 len;

   el = event->element;
   len = TtaGetTextAttributeLength (event->attribute);
   if (len >= oldHrefMaxLen)
      len = oldHrefMaxLen - 1;
   TtaGiveTextAttributeValue (event->attribute, oldXlinkHrefValue, &len);
   oldXlinkHrefValue[len] = WC_EOS;
   return FALSE;  /* let Thot perform normal operation */
}

/*----------------------------------------------------------------------
 AttrXlinkHrefChanged
 -----------------------------------------------------------------------*/
void             AttrXlinkHrefChanged (NotifyAttribute *event)
{
   ElementType   elType;
   STRING        text;
   int           length;

   length = TtaGetTextAttributeLength (event->attribute);
   if (length <= 0)
     /* attribute empty. Invalid. restore previous value */
     {
     TtaSetAttributeText (event->attribute, oldXlinkHrefValue, event->element,
			  event->document);
     return;
     }
   text = TtaAllocString (length + 1);
   TtaGiveTextAttributeValue (event->attribute, text, &length);
   elType = TtaGetElementType (event->element);
   if (elType.ElTypeNum == GraphML_EL_image)
     ComputeSRCattribute (event->element, event->document, 0, event->attribute,
			  text);
   else if (elType.ElTypeNum == GraphML_EL_use_)
     CopyUseContent (event->element, event->document, text);
   TtaFreeMemory (text);
}

/*----------------------------------------------------------------------
 DeleteAttrXlinkHref
 -----------------------------------------------------------------------*/
ThotBool         DeleteAttrXlinkHref (NotifyAttribute *event)
{
  /* prevents Thot from deleting the xlink:href attribute */
  return TRUE;
}

#if 0
/*** This is an experiment to test how SVG could be animated ***/
/*** Works for document ~quint/Talks/AC-Nov00/all.htm only   ***/
/*** See also file GraphML.A                                 ***/

static int oldValue = 0;

/*----------------------------------------------------------------------
   TextWillChangeInGroup
   The user is about to change a character string within a g element
   Store the number of that g element.
 -----------------------------------------------------------------------*/
ThotBool TextWillChangeInGroup (NotifyOnTarget *event)
{
  Element     leaf;
  int         len;
  STRING      text;
  Language    lang;

  leaf = event->target;
  len = TtaGetTextLength (leaf) + 1;
  text = TtaAllocString (len);
  TtaGiveTextContent (leaf, text, &len, &lang);
  usscanf (text, TEXT("%d"), &oldValue);
  TtaFreeMemory (text);
  return FALSE; /* let Thot perform normal operation */
}

/*----------------------------------------------------------------------
   TextChangedInGroup
   A character string has been modified within a g element
   If the g element has an attribute class="animatable", exchange that g
   element with the one that has the same number.
  ----------------------------------------------------------------------*/
void                TextChangedInGroup (NotifyOnTarget *event)
{
  Element       group1, group2, leaf1, leaf2, parent, child;
  ElementType   elType;
  Attribute     attr;
  AttributeType attrType;
  Document      doc;
  PRule         presRuleX1, presRuleX2, presRuleY1, presRuleY2;
  CHAR_T        buffer[8];
  STRING        text;
  Language      lang;
  int           x, y, xx, yy, x1, y1, x2, y2, i, len, num;
  ThotBool      found, neighbour;

  /* group1: the g element whose number is changed by the user */
  group1 = event->element;
  doc = event->document;
  /* if the g element does not have attribute class="animatable" don't do
     anything */
  elType = TtaGetElementType (group1);
  attrType.AttrSSchema = elType.ElSSchema;
  attrType.AttrTypeNum = GraphML_ATTR_class;
  attr = TtaGetAttribute (group1, attrType);
  if (!attr)
    /* there is no class attribute */
     return;
  len = TtaGetTextAttributeLength (attr)+1;
  text = TtaAllocString (len);
  TtaGiveTextAttributeValue (attr, text, &len);
  found = (strcmp(text, "animatable") == 0);
  TtaFreeMemory (text);
  if (!found)
     /* class is not "animatable" */
     return;

  /* get the content of the text element that has been changed */
  leaf1 = event->target;
  len = TtaGetTextLength (leaf1) + 1;
  text = TtaAllocString (len);
  TtaGiveTextContent (leaf1, text, &len, &lang);
  /* convert that content into a number */
  usscanf (text, TEXT("%d"), &num);
  TtaFreeMemory (text);
  /* only cubes 4 to 9 can be moved */
  if (num < 4 || num > 9 || oldValue < 4 || oldValue > 9)
    {
      /* this cube can't be moved. Restore its original number */
      usprintf (buffer, TEXT("%d"), oldValue);
      TtaSetTextContent (leaf1, buffer, lang, doc);
      return;
    }
  /* look for the other cube containing the same number */
  parent = TtaGetParent (group1);
  group2 = TtaGetFirstChild (parent);
  found = False;
  /* check all sibling elements */
  while (group2 && !found)
    {
    /* skip the cube that has been changed by the user */
    if (group2 != group1)
      {
	elType = TtaGetElementType (group2);
	if (elType.ElTypeNum == GraphML_EL_g)
	  /* this is a g element */
	  {
	    /* get its last child */
	    child = TtaGetLastChild (group2);
	    if (child)
	      {
		elType = TtaGetElementType (child);
		if (elType.ElTypeNum == GraphML_EL_text_)
		  /* the last child is a SVG text element */
		  {
		    /* get its content */
		    leaf2 = TtaGetFirstChild (child);
		    if (leaf2)
		      {
			elType = TtaGetElementType (leaf2);
			if (elType.ElTypeNum == GraphML_EL_TEXT_UNIT)
			  {
			    len = TtaGetTextLength (leaf2) + 1;
			    text = TtaAllocString (len);
			    TtaGiveTextContent (leaf2, text, &len, &lang);
			    /* convert the content into a number */
			    usscanf (text, TEXT("%d"), &i);
			    TtaFreeMemory (text);
			    if (i == num)
			      /* that's the same number */
			      found = TRUE;
			  }
		      }
		  }
	      }
	  }
      }
    if (!found)
      TtaNextSibling (&group2);
    }

  if (!found)
    /* there is no other cube with that number. Restore the original number
       and return */
    {
      usprintf (buffer, TEXT("%d"), oldValue);
      TtaSetTextContent (leaf1, buffer, lang, doc);
      return;
    }

  /* check if the 2 cubes to be exchanged are neighbours */
  if (num - oldValue == 1 || oldValue - num == 1)
    neighbour = TRUE;
  else
    neighbour = FALSE;

  /* the other cube take the previous number of the modified cube */
  usprintf (buffer, TEXT("%d"), oldValue);
  TtaSetTextContent (leaf2, buffer, lang, doc);

  /* get the original coordinates of both cubes */
  presRuleX1 = TtaGetPRule (group1, PRHorizPos);
  if (presRuleX1)
     x1 = TtaGetPRuleValue (presRuleX1);
  else
     x1 = 0;
  presRuleX2 = TtaGetPRule (group2, PRHorizPos);
  if (presRuleX2)
     x2 = TtaGetPRuleValue (presRuleX2);
  else
     x2 = 0;
  presRuleY1 = TtaGetPRule (group1, PRVertPos);
  if (presRuleY1)
     y1 = TtaGetPRuleValue (presRuleY1);
  else
     y1 = 0;
  presRuleY2 = TtaGetPRule (group2, PRVertPos);
  if (presRuleY2)
     y2 = TtaGetPRuleValue (presRuleY2);
  else
     y2 = 0;

  /* if both cubes are not neighbour, move them down to y=100 */
  if (!neighbour)
    {
      y = y1;  yy = y2;
      do
	{
	  if (y < 100)
	    {
	      y++;
	      TtaSetPRuleValue (group1, presRuleY1, y, doc);
	    }
	  if (yy < 100)
	    {
	      yy++;
	      TtaSetPRuleValue (group2, presRuleY2, yy, doc);
	    }      
	}
      while (y < 100 && yy < 100);
    }

  /* move both cubes horizontally: exchange their positions */
  x = x1;  xx = x2;
  do
    {
      if (x != x2)
	{
	  if (x < x2)
	    x++;
	  else
	    x--;
	  TtaSetPRuleValue (group1, presRuleX1, x, doc);
	}
      if (xx != x1)
	{
	  if (xx < x1)
	    xx++;
	  else
	    xx--;
	  TtaSetPRuleValue (group2, presRuleX2, xx, doc);
	}
    }
  while (x != x2 && xx != x1);

  /* move both cubes up */
  if (!neighbour)
    {
      do
	{
	  if (y > y2)
	    {
	      y--;
	      TtaSetPRuleValue (group1, presRuleY1, y, doc);
	    }
	  if (yy > y1)
	    {
	      yy--;
	      TtaSetPRuleValue (group2, presRuleY2, yy, doc);
	    }      
	}
      while (y != y2  && yy != y1);
    }
  /* that's it! */
}
#endif
