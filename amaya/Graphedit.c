#ifdef GRAPHML
/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996.
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
#include "html2thot_f.h"
#include "GraphMLbuilder_f.h"
#include "HTMLpresentation_f.h"

/*----------------------------------------------------------------------
 SetEmptyShapeAttrSubTree
 A GraphML drawing is about to be saved. Set the EmptyShape attribute
 on all closed geometric shapes that do not contain any Label element.
 This attribute is used by the translation schema (GraphMLT.T) to
 generate a closing tag.
 -----------------------------------------------------------------------*/
#ifdef __STDC__
static void SetEmptyShapeAttrSubTree (Element el, Document doc)
#else /* __STDC__*/
static void SetEmptyShapeAttrSubTree(el, doc)
Element el;
Document doc;
#endif /* __STDC__*/
{
  ElementType	elType, labelType;
  AttributeType	attrType;
  Attribute	attr;
  Element	child, labelEl;
  SSchema	GraphMLSchema;

  elType = TtaGetElementType (el);
  GraphMLSchema = elType.ElSSchema;
  attrType.AttrSSchema = GraphMLSchema;
  attrType.AttrTypeNum = GraphML_ATTR_IntEmptyShape;
  labelType.ElSSchema = GraphMLSchema;
  labelType.ElTypeNum = GraphML_EL_Label;

  child = TtaGetFirstChild (el);
  while (child)
     {
     elType = TtaGetElementType (child);
     if (elType.ElSSchema == GraphMLSchema)
	{
        if (elType.ElTypeNum == GraphML_EL_Rectangle ||
	    elType.ElTypeNum == GraphML_EL_RoundRect ||
	    elType.ElTypeNum == GraphML_EL_Circle ||
	    elType.ElTypeNum == GraphML_EL_Oval ||
	    elType.ElTypeNum == GraphML_EL_Polygon ||
	    elType.ElTypeNum == GraphML_EL_ClosedSpline)
	   {
	   labelEl = TtaSearchTypedElement (labelType, SearchInTree, child);
	   attr = TtaGetAttribute (child, attrType);
	   if (!labelEl)
	      /* child does not contain any Label element */
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
	      /* there is a label element */
	      if (attr)
		 /* there is an IntEmptyShape attribute. Delete it */
		 TtaRemoveAttribute (child, attr, doc);
	   }
	SetEmptyShapeAttrSubTree(child, doc);
	}
     TtaNextSibling (&child);
     }
}

/*----------------------------------------------------------------------
 ExtendSelectGraphMLElement
 The user wants to add a new element in the current selection.
 -----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool ExtendSelectGraphMLElement (NotifyElement *event)
#else /* __STDC__*/
ThotBool ExtendSelectGraphMLElement(event)
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
   /* get the common ancestor */
   ancestor = TtaGetCommonAncestor (firstSel, event->element);
   if (ancestor == NULL)
      return TRUE;	/* Don't let Thot perform normal operation */
   graphSSchema = TtaGetSSchema (TEXT("GraphML"), event->document);
   ancestType = TtaGetElementType (ancestor);
   if (ancestType.ElSSchema != graphSSchema)
      /* common ancestor is not a GraphML element */
      {
      /* is the common ancestor within a Label ? */
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
	    a Label */
	 return FALSE;
      else
         return TRUE;	/* abort selection */
      }

   newFirstSel = firstSel;
   elType = TtaGetElementType (firstSel);
   if (elType.ElSSchema != graphSSchema ||
        (elType.ElTypeNum != GraphML_EL_Rectangle &&
	 elType.ElTypeNum != GraphML_EL_RoundRect &&
	 elType.ElTypeNum != GraphML_EL_Circle &&
	 elType.ElTypeNum != GraphML_EL_Oval &&
	 elType.ElTypeNum != GraphML_EL_Polyline &&
	 elType.ElTypeNum != GraphML_EL_Polygon &&
	 elType.ElTypeNum != GraphML_EL_Spline &&
	 elType.ElTypeNum != GraphML_EL_ClosedSpline &&
	 elType.ElTypeNum != GraphML_EL_Text_ &&
	 elType.ElTypeNum != GraphML_EL_Math &&
	 elType.ElTypeNum != GraphML_EL_Group))
      {
      elType.ElSSchema = graphSSchema;
      elType.ElTypeNum = GraphML_EL_GraphicalElement;
      newFirstSel = TtaGetTypedAncestor (newFirstSel, elType);
      }

   selEl = event->element;
   elType = TtaGetElementType (selEl);
   if (elType.ElSSchema != graphSSchema ||
        (elType.ElTypeNum != GraphML_EL_Rectangle &&
	 elType.ElTypeNum != GraphML_EL_RoundRect &&
	 elType.ElTypeNum != GraphML_EL_Circle &&
	 elType.ElTypeNum != GraphML_EL_Oval &&
	 elType.ElTypeNum != GraphML_EL_Polyline &&
	 elType.ElTypeNum != GraphML_EL_Polygon &&
	 elType.ElTypeNum != GraphML_EL_Spline &&
	 elType.ElTypeNum != GraphML_EL_ClosedSpline &&
	 elType.ElTypeNum != GraphML_EL_Text_ &&
	 elType.ElTypeNum != GraphML_EL_Math &&
	 elType.ElTypeNum != GraphML_EL_Group))
      {
      elType.ElSSchema = graphSSchema;
      elType.ElTypeNum = GraphML_EL_GraphicalElement;
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
 SetEmptyShapeAttribute
 A GraphML drawing is about to be saved. Set the EmptyShape attribute
 on all closed geometric shapes that do not contain any Label element.
 This attribute is used by the translation schema (GraphMLT.T) to
 generate a closing tag.
 -----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool SetEmptyShapeAttribute (NotifyElement *event)
#else /* __STDC__*/
ThotBool SetEmptyShapeAttribute(event)
     NotifyElement *event;
#endif /* __STDC__*/
{
  SetEmptyShapeAttrSubTree (event->element, event->document);
  return FALSE; /* let Thot perform normal operation */
}

/*----------------------------------------------------------------------
 AttrPositionChanged
 -----------------------------------------------------------------------*/
#ifdef __STDC__
void AttrPositionChanged (NotifyAttribute *event)
#else /* __STDC__*/
void AttrPositionChanged (event)
     NotifyAttribute *event;
#endif /* __STDC__*/
{
   ParsePositionAttribute (event->attribute, event->element, event->document);
}


/*----------------------------------------------------------------------
 UpdatePointsAttribute
 update attribute "points" for element el according its content
 -----------------------------------------------------------------------*/
#ifdef __STDC__
static void UpdatePointsAttribute (Element el, Document doc, int *minX, int *minY, int *maxX, int *maxY)
#else /* __STDC__*/
static void UpdatePointsAttribute (el, doc, minX, minY, maxX, maxY)
     Element el;
     Document doc;
     int *minX;
     int *minY;
     int *maxX;
     int *maxY;
#endif /* __STDC__*/

{
  Element		child;
  ElementType		elType;
  AttributeType	        attrType;
  Attribute		attr, attrX, attrY;
  TypeUnit		unit;
  CHAR_T			buffer[512], buffer1[8];
  int			nbPoints, point, x, y, posX, posY;
  int                   mainView;

  mainView = TtaGetViewFromName (doc, "Formatted_view");
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
      attrType.AttrTypeNum = GraphML_ATTR_position;
      attr = TtaGetAttribute (el, attrType);
      unit = UnPoint;
      posX = posY = 0;
      if (attr == NULL)
	/* element el has no position attribute, get its IntPosX and IntPosY
	   attributes */
	{
	  attrType.AttrTypeNum = GraphML_ATTR_IntPosX;
	  attrX = TtaGetAttribute (el, attrType);
	  attrType.AttrTypeNum = GraphML_ATTR_IntPosY;
	  attrY = TtaGetAttribute (el, attrType);
	  if (attrX == NULL && attrY == NULL)
	    TtaGiveBoxPosition (el, doc, mainView, unit, &posX, &posY);
	  else
	    {
	      if (attrX != NULL)
		posX = TtaGetAttributeValue (attrX);
	      if (attrY != NULL)
		posY = TtaGetAttributeValue (attrY);
	    }
	}
      *minX = *minY = 32000;
      *maxX = *maxY = 0;
      buffer[0] = EOS;
      for (point = 1; point <= nbPoints; point++)
	{
	  TtaGivePolylinePoint (child, point, unit, &x, &y);
	  if (x > *maxX)
	    *maxX = x;
	  if (x < *minX)
	    *minX = x;
	  if (y > *maxY)
	    *maxY = y;
	  if (y < *minY)
	    *minY = y;
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

/*----------------------------------------------------------------------
 UpdatePositionAttribute
 update attribute "position" for element el according its attributes
 "IntPosX" and "IntPosY".
 -----------------------------------------------------------------------*/
#ifdef __STDC__
static void UpdatePositionAttribute (Attribute attr, Element el, Document doc)
#else /* __STDC__*/
static void UpdatePositionAttribute (attr, el, doc)
     Attribute attr;
     Element el;
     Document doc;
#endif /* __STDC__*/
{
   AttributeType	attrType;
   ElementType		elType;
   CHAR_T			buffer[32], buffer1[8];
   int			attrKind, posX, posY, minX, minY, maxX, maxY;
   ThotBool		changePoints;

   elType = TtaGetElementType (el);
   if (elType.ElTypeNum == GraphML_EL_Line_)
     changePoints = TRUE;
   else
     {
       changePoints = FALSE;
       TtaGiveAttributeType (attr, &attrType, &attrKind);
       attrType.AttrTypeNum = GraphML_ATTR_IntPosX;
       attr = TtaGetAttribute (el, attrType);
       if (attr == NULL)
	 posX = 0;
       else
	 posX = TtaGetAttributeValue (attr);
       usprintf (buffer, TEXT("%d"), posX);
       attrType.AttrTypeNum = GraphML_ATTR_IntPosY;
       attr = TtaGetAttribute (el, attrType);
       if (attr == NULL)
	 posY = 0;
       else
	 posY = TtaGetAttributeValue (attr);
       usprintf (buffer1, TEXT("%d"), posY);
       ustrcat (buffer, TEXT(", "));
       ustrcat (buffer, buffer1);
       attrType.AttrTypeNum = GraphML_ATTR_position;
       attr = TtaGetAttribute (el, attrType);
       if (attr == NULL)
	 /* element el has no position attribute */
	 {
	   attr = TtaNewAttribute (attrType);
	   TtaAttachAttribute (el, attr, doc);
	   if (elType.ElTypeNum == GraphML_EL_Polyline ||
	       elType.ElTypeNum == GraphML_EL_Polygon ||
	       elType.ElTypeNum == GraphML_EL_Spline ||
	       elType.ElTypeNum == GraphML_EL_ClosedSpline)
	     changePoints = TRUE;
	 }
       TtaSetAttributeText (attr, buffer, el, doc);
     }
   if (changePoints)
      /* change the points attribute according to the new position */
      UpdatePointsAttribute (el, doc, &minX, &minY, &maxX, &maxY);
}

/*----------------------------------------------------------------------
 AttrIntPosChanged
 -----------------------------------------------------------------------*/
#ifdef __STDC__
void AttrIntPosChanged (NotifyAttribute *event)
#else /* __STDC__*/
void AttrIntPosChanged (event)
     NotifyAttribute *event;
#endif /* __STDC__*/
{
   UpdatePositionAttribute (event->attribute, event->element, event->document);
}

/*----------------------------------------------------------------------
 AttrWidthChanged
 -----------------------------------------------------------------------*/
#ifdef __STDC__
void AttrWidthChanged (NotifyAttribute *event)
#else /* __STDC__*/
void AttrWidthChanged (event)
     NotifyAttribute *event;
#endif /* __STDC__*/
{
   ParseWidthHeightAttribute (event->attribute, event->element, event->document);
}

/*----------------------------------------------------------------------
 AttrHeightChanged
 -----------------------------------------------------------------------*/
#ifdef __STDC__
void AttrHeightChanged (NotifyAttribute *event)
#else /* __STDC__*/
void AttrHeightChanged (event)
     NotifyAttribute *event;
#endif /* __STDC__*/
{
   ParseWidthHeightAttribute (event->attribute, event->element, event->document);
}

/*----------------------------------------------------------------------
 UpdateWidthHeightAttribute
 Attribute "IntWidth" or "IntHeight" has been modified for element el.
 Update the corresponding attribute "height_" or "width_" accordingly.
 -----------------------------------------------------------------------*/
#ifdef __STDC__
static void UpdateWidthHeightAttribute (Attribute attr, Element el, Document doc)
#else /* __STDC__*/
static void UpdateWidthHeightAttribute (attr, el, doc)
     Attribute attr;
     Element el;
     Document doc;
#endif /* __STDC__*/
{
  ElementType           elType;
   AttributeType	attrType;
   Attribute		extAttr;
   CHAR_T			buffer[10];
   int			attrKind, val;
   int                  minX, minY, maxX, maxY;

   elType = TtaGetElementType (el);
   if (elType.ElTypeNum == GraphML_EL_Line_)
     UpdatePointsAttribute (el, doc, &minX, &minY, &maxX, &maxY);
   else
     {
       TtaGiveAttributeType (attr, &attrType, &attrKind);
       if (attrType.AttrTypeNum == GraphML_ATTR_IntWidth)
	 attrType.AttrTypeNum = GraphML_ATTR_width_;
       else if (attrType.AttrTypeNum == GraphML_ATTR_IntHeight)
	 attrType.AttrTypeNum = GraphML_ATTR_height_;
       extAttr = TtaGetAttribute (el, attrType);
       if (extAttr == NULL)
	 {
	   extAttr = TtaNewAttribute (attrType);
	   TtaAttachAttribute (el, extAttr, doc);
	 }
       val = TtaGetAttributeValue (attr);
       usprintf (buffer, TEXT("%d"), val);
       TtaSetAttributeText (extAttr, buffer, el, doc);
     }
}

/*----------------------------------------------------------------------
 AttrIntSizeChanged
 Attribute IntWidth or IntHeight has been modified
 -----------------------------------------------------------------------*/
#ifdef __STDC__
void AttrIntSizeChanged (NotifyAttribute *event)
#else /* __STDC__*/
void AttrIntSizeChanged (event)
     NotifyAttribute *event;
#endif /* __STDC__*/
{
   UpdateWidthHeightAttribute (event->attribute, event->element, event->document);
}

/*----------------------------------------------------------------------
 AttrDirectionModified
 -----------------------------------------------------------------------*/
#ifdef __STDC__
void AttrDirectionModified (NotifyAttribute *event)
#else /* __STDC__*/
void AttrDirectionModified(event)
     NotifyAttribute *event;
#endif /* __STDC__*/
{
  ParseDirAndSpaceAttributes (event->element, NULL, event->document);
}
 
/*----------------------------------------------------------------------
 AttrSpaceModified
 -----------------------------------------------------------------------*/
#ifdef __STDC__
void AttrSpaceModified (NotifyAttribute *event)
#else /* __STDC__*/
void AttrSpaceModified(event)
     NotifyAttribute *event;
#endif /* __STDC__*/
{
  ParseDirAndSpaceAttributes (event->element, NULL, event->document);
}

/*----------------------------------------------------------------------
 GraphElemPasted
 An element has been pasted.  If its parent is a Group with an
 attribute "direction", create the corresponding internal attributes.
 -----------------------------------------------------------------------*/
#ifdef __STDC__
void GraphElemPasted (NotifyElement *event)
#else /* __STDC__*/
void GraphElemPasted(event)
     NotifyElement *event;
#endif /* __STDC__*/
{
  Element	parent;
  ElementType	elType;

  parent = TtaGetParent (event->element);
  elType = TtaGetElementType (parent);
  if (elType.ElTypeNum == GraphML_EL_Group)
     ParseDirAndSpaceAttributes (parent, event->element, event->document);
}

/*----------------------------------------------------------------------
 AttrArrowHeadModified
 -----------------------------------------------------------------------*/
#ifdef __STDC__
void AttrArrowHeadModified (NotifyAttribute *event)
#else /* __STDC__*/
void AttrArrowHeadModified (event)
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
ThotBool DeleteAttrPoints (NotifyAttribute *event)
#else /* __STDC__*/
ThotBool DeleteAttrPoints (event)
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
void AttrPointsModified (NotifyAttribute *event)
#else /* __STDC__*/
void AttrPointsModified (event)
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
ThotBool   GraphicsPRuleChange (NotifyPresentation *event)
#else /* __STDC__*/
ThotBool   GraphicsPRuleChange (event)
           NotifyPresentation *event;
#endif /* __STDC__*/
{
  Element       el;
  PRule         presRule;
  Document      doc;
  ElementType   elType;
  AttributeType attrType, otherAttrType;
  Attribute     attr, otherAttr;
  DisplayMode   dispMode;
  int           presType;
  int           mainView, i, val, unit;
  int           xPos, yPos, width, height, otherVal;
  ThotBool      ret;
 
  ret = FALSE; /* let Thot perform normal operation */
  presType = event->pRuleType;
  if (presType != PRVertPos && presType != PRHorizPos &&
      presType != PRHeight && presType != PRWidth)
    {
      ret = ChangePRule (event);
      return (ret); /* let Thot perform or not normal operation */
    }
 
  el = event->element;
  doc = event->document;
  presRule = event->pRule;
 
  mainView = TtaGetViewFromName (doc, "Formatted_view");
  elType = TtaGetElementType (el);
  if (elType.ElSSchema != GetGraphMLSSchema (doc))
    return (ret); /* let Thot perform normal operation */
 
  attrType.AttrSSchema = elType.ElSSchema;
  /* switch off the current selection */
  /*TtaSwitchSelection (doc, mainView, FALSE);*/
  dispMode = TtaGetDisplayMode (doc);
  /* ask Thot to stop displaying changes made in the document */
  if (dispMode == DisplayImmediately)
    TtaSetDisplayMode (doc, DeferredDisplay);
  if (presType == PRVertPos || presType == PRHorizPos)
    {
      if (elType.ElTypeNum == GraphML_EL_Rectangle ||
          elType.ElTypeNum == GraphML_EL_RoundRect ||
          elType.ElTypeNum == GraphML_EL_Circle ||
          elType.ElTypeNum == GraphML_EL_Oval ||
          elType.ElTypeNum == GraphML_EL_Polyline ||
          elType.ElTypeNum == GraphML_EL_Polygon ||
          elType.ElTypeNum == GraphML_EL_Spline ||
          elType.ElTypeNum == GraphML_EL_ClosedSpline ||
          elType.ElTypeNum == GraphML_EL_Text_ ||
          elType.ElTypeNum == GraphML_EL_Math ||
          elType.ElTypeNum == GraphML_EL_Group)
        {
          TtaGiveBoxPosition (el, doc, mainView, UnPoint, &xPos, &yPos);
	  unit = TtaGetPRuleUnit (presRule);
          if (presType == PRVertPos)
            {
	      /* the new value is the old one plus the delta */
	      TtaGiveBoxPosition (el, doc, mainView, unit, &val, &i);
	      val += TtaGetPRuleValue (presRule);
              attrType.AttrTypeNum = GraphML_ATTR_IntPosY;
              otherAttrType.AttrTypeNum = GraphML_ATTR_IntPosX;
              otherVal = xPos;
              ret = TRUE; /* don't let Thot perform normal operation */
            }
          else
            {
	      /* the new value is the old one plus the delta */
	      TtaGiveBoxPosition (el, doc, mainView, unit, &i, &val);
	      val += TtaGetPRuleValue (presRule);
              attrType.AttrTypeNum = GraphML_ATTR_IntPosX;
              otherAttrType.AttrTypeNum = GraphML_ATTR_IntPosY;
              otherVal = yPos;
              ret = TRUE; /* don't let Thot perform normal operation */
            }
 
          attr = TtaGetAttribute (el, attrType);
          if (attr == NULL)
            {
              attr = TtaNewAttribute (attrType);
              TtaAttachAttribute (el, attr, doc);
              /* create also a IntPosX attribute is we are creating a IntPosY
                 attribute, and conversely */
              otherAttrType.AttrSSchema = attrType.AttrSSchema;
              otherAttr = TtaNewAttribute (otherAttrType);
              TtaAttachAttribute (el, otherAttr, doc);
              TtaSetAttributeValue (otherAttr, otherVal, el, doc);
            }
          TtaSetAttributeValue (attr, val, el, doc);
          UpdatePositionAttribute (attr, el, doc);
        }
    }
  else if (presType == PRHeight)
    {
      if (elType.ElTypeNum == GraphML_EL_Rectangle ||
          elType.ElTypeNum == GraphML_EL_RoundRect ||
          elType.ElTypeNum == GraphML_EL_Oval ||
          elType.ElTypeNum == GraphML_EL_Polyline ||
          elType.ElTypeNum == GraphML_EL_Polygon ||
          elType.ElTypeNum == GraphML_EL_Spline ||
          elType.ElTypeNum == GraphML_EL_ClosedSpline)
        {
	  /* the new value is the old one plus the delta */
	  unit = TtaGetPRuleUnit (presRule);
	  TtaGiveBoxSize (el, doc, 1, unit, &i, &height);
	  height += TtaGetPRuleValue (presRule);
          attrType.AttrTypeNum = GraphML_ATTR_IntHeight;
          attr = TtaGetAttribute (el, attrType);
          if (attr == NULL)
            {
              attr = TtaNewAttribute (attrType);
              TtaAttachAttribute (el, attr, doc);
            }
          TtaSetAttributeValue (attr, height, el, doc);
          UpdateWidthHeightAttribute (attr, el, doc);
          ret = TRUE; /* don't let Thot perform normal operation */
        }
    }
  else if (presType == PRWidth)
    {
      if (elType.ElTypeNum == GraphML_EL_Rectangle ||
          elType.ElTypeNum == GraphML_EL_RoundRect ||
          elType.ElTypeNum == GraphML_EL_Circle ||
          elType.ElTypeNum == GraphML_EL_Oval ||
          elType.ElTypeNum == GraphML_EL_Polyline ||
          elType.ElTypeNum == GraphML_EL_Polygon ||
          elType.ElTypeNum == GraphML_EL_Spline ||
          elType.ElTypeNum == GraphML_EL_ClosedSpline ||
          elType.ElTypeNum == GraphML_EL_Text_)
        {
	  /* the new value is the old one plus the delta */
	  unit = TtaGetPRuleUnit (presRule);
	  TtaGiveBoxSize (el, doc, 1, unit, &width, &i);
	  width += TtaGetPRuleValue (presRule);
          attrType.AttrTypeNum = GraphML_ATTR_IntWidth;
          attr = TtaGetAttribute (el, attrType);
          if (attr == NULL)
            {
              attr = TtaNewAttribute (attrType);
              TtaAttachAttribute (el, attr, doc);
            }
          TtaSetAttributeValue (attr, width, el, doc);
          UpdateWidthHeightAttribute (attr, el, doc);
          ret = TRUE; /* don't let Thot perform normal operation */
        }
    }
  TtaSetDisplayMode (doc, dispMode);
   /* switch on the current selection */
   /* TtaSwitchSelection (doc, mainView, TRUE);*/
  return ret; /* let Thot perform normal operation */
}

/*----------------------------------------------------------------------
   ControlPointChanged
   A control point has been changed in a Line_, a Polyline, a Polygon,
   a Spline or a ClosedSpline.  Update the points attribute.
 -----------------------------------------------------------------------*/
#ifdef __STDC__
void ControlPointChanged (NotifyOnValue *event)
#else /* __STDC__*/
void ControlPointChanged(event)
     NotifyOnValue *event;
#endif /* __STDC__*/
{
   int	minX, minY, maxX, maxY;

   UpdatePointsAttribute (event->element, event->document, &minX, &minY, &maxX,
			  &maxY);
}

/*----------------------------------------------------------------------
 GraphLeafDeleted
 A GRAPHICS_UNIT element has been deleted. Delete its Label sibling
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
   Document	        doc;
   Element	        last, first, graphRoot, newEl, sibling, selEl;
   Element          child, parent, elem;
   ElementType      elType, wrapperType, newType, childType;
   AttributeType	attrType;
   Attribute        attr;
   SSchema	        docSchema, GraphMLSSchema;
   DisplayMode      dispMode;
   char		        shape;
   int		        c1, c2, i, j, w, h, minX, minY, maxX, maxY;
   ThotBool	        found, automaticPlacement;
   int	            oldStructureChecking;

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
   GraphMLSSchema = GetGraphMLSSchema (doc);
   elType.ElTypeNum = GraphML_EL_GraphML;
   elType.ElSSchema = GraphMLSSchema;
   attrType.AttrSSchema = GraphMLSSchema;
   graphRoot = TtaGetTypedAncestor (first, elType);
   if (graphRoot == NULL)
      /* the current selection is not in a GraphML element, create one */
      {
      wrapperType = TtaGetElementType (first);
      if (ustrcmp (TtaGetSSchemaName (wrapperType.ElSSchema), TEXT("HTML")))
	 /* selection is not in an HTML element. */
         return;
      wrapperType.ElTypeNum = HTML_EL_XMLGraphics;
      TtaCreateElement (wrapperType, doc);
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
	    if (elType.ElSSchema == GraphMLSSchema &&
		(elType.ElTypeNum == GraphML_EL_Group ||
		 elType.ElTypeNum == GraphML_EL_GraphML))
		found = TRUE;
	    else
		sibling = parent;
	    }
	}
   while (parent && !found);

   automaticPlacement = FALSE;
   if (!parent)
      {
      parent = graphRoot;
      sibling = TtaGetFirstChild (graphRoot);
      }
   else
      /* if parent has a "direction" attribute, the user has not to choose
	 the position of this element */
      {
      attrType.AttrTypeNum = GraphML_ATTR_direction;
      if (TtaGetAttribute (parent, attrType))
	 automaticPlacement = TRUE;
      }

   TtaOpenUndoSequence (doc, first, last, c1, c2);

   newType.ElSSchema = GraphMLSSchema;
   newType.ElTypeNum = 0;
   shape = EOS;

   switch (entry)
    {
    case 0:	/* line */
	newType.ElTypeNum = GraphML_EL_Line_;
	shape = 'w';
	break;
    case 1:	/* rectangle */
	newType.ElTypeNum = GraphML_EL_Rectangle;
	shape = 'R';
	break;
    case 2:	/* rounded rectangle */
	newType.ElTypeNum = GraphML_EL_RoundRect;
	shape = 'C';
	break;
    case 3:	/* circle */
	newType.ElTypeNum = GraphML_EL_Circle;
	shape = 'a';
	break;
    case 4:	/* oval */
	newType.ElTypeNum = GraphML_EL_Oval;
	shape = 'c';
	break;
    case 5:	/* polyline */
	newType.ElTypeNum = GraphML_EL_Polyline;
	shape = 'S';
	break;
    case 6:	/* polygon */
	newType.ElTypeNum = GraphML_EL_Polygon;
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
    case 9:	/* label */
	/* a label can be inserted only in some types of elements (closed
	shapes), only when there is no Label in the element and only if a
        single element is selected */
        if (first == last)
	   /* a single element is selected */
	   {
	   elType = TtaGetElementType (first);
	   if (elType.ElTypeNum == GraphML_EL_GRAPHICS_UNIT)
	      {
	      parent = TtaGetParent (first);
	      elType = TtaGetElementType (parent);
	      }
	   else
	      parent = first;
	   
	   if (elType.ElTypeNum == GraphML_EL_Rectangle ||
	       elType.ElTypeNum == GraphML_EL_RoundRect ||
	       elType.ElTypeNum == GraphML_EL_Circle ||
	       elType.ElTypeNum == GraphML_EL_Oval ||
	       elType.ElTypeNum == GraphML_EL_Polygon ||
	       elType.ElTypeNum == GraphML_EL_ClosedSpline)
	      {
	      elem = TtaGetFirstChild (parent);
	      found = FALSE;
	      while (!found && elem)
		{
		elType = TtaGetElementType (elem);
		if (elType.ElTypeNum == GraphML_EL_Label &&
		    elType.ElSSchema == GraphMLSSchema)
		   found = TRUE;
		else
		   TtaNextSibling (&elem);
		}
	      if (!found)
		 newType.ElTypeNum = GraphML_EL_Label;
	      sibling = NULL;
	      }
	   }
	break;
    case 10:	/* text */
	newType.ElTypeNum = GraphML_EL_Text_;
	break;
    case 11:	/* group */
	newType.ElTypeNum = 0;
	break;
    default:
	break;
    }

   if (newType.ElTypeNum > 0)
     {
       /* switch off the current selection */
       /*TtaSwitchSelection (doc, 1, FALSE);*/
       dispMode = TtaGetDisplayMode (doc);
       /* ask Thot to stop displaying changes made in the document */
       if (dispMode == DisplayImmediately)
         TtaSetDisplayMode (doc, DeferredDisplay);

       /* for rectangles, circle, oval, and text, ask for an elastic box,
	  except when the parent is a Group with a "direction" attribute */
       if (!automaticPlacement)
         if (newType.ElTypeNum == GraphML_EL_Rectangle ||
	     newType.ElTypeNum == GraphML_EL_RoundRect ||
	     newType.ElTypeNum == GraphML_EL_Circle ||
	     newType.ElTypeNum == GraphML_EL_Oval ||
	     newType.ElTypeNum == GraphML_EL_Text_ ||
	     newType.ElTypeNum == GraphML_EL_Math)
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
	   childType.ElSSchema = GraphMLSSchema;
	   childType.ElTypeNum = GraphML_EL_GRAPHICS_UNIT;
	   child = TtaNewElement (doc, childType);
	   TtaInsertFirstChild (&child, newEl, doc);
	   TtaSetGraphicsShape (child, shape, doc);
	   selEl = child;
	 }
       else if (newType.ElTypeNum == GraphML_EL_Label ||
		newType.ElTypeNum == GraphML_EL_Text_)
	 /* create an HTML DIV element in the new element */
	 {
	   /* the document is supposed to be HTML */
	   childType.ElSSchema = docSchema;
	   childType.ElTypeNum = HTML_EL_HTMLfragment;
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

       /* if the parent element is a Group, create the internal attribute
	  corresponding to attribute direction of the parent element */
       elType = TtaGetElementType (parent);
       if (elType.ElTypeNum == GraphML_EL_Group)
         ParseDirAndSpaceAttributes (parent, newEl, doc);

       /* ask Thot to display changes made in the document */
       TtaSetDisplayMode (doc, dispMode);
     }

   if (selEl != NULL)
     /* select the right element */
     TtaSelectElement (doc, selEl);
   
   if (shape == 'S' || shape == 'w' || shape == 'p' || shape == 'B' ||
       shape == 's')
     /* multipoints element. Let the user enter the points */
     {
       if (automaticPlacement)
	 {
	   w = 80;
	   attrType.AttrTypeNum = GraphML_ATTR_IntWidth;
	   attr = TtaNewAttribute (attrType);
	   TtaAttachAttribute (newEl, attr, doc);
	   TtaSetAttributeValue (attr, w, newEl, doc);
	   UpdateWidthHeightAttribute (attr, newEl, doc);
	   h = 60;
	   attrType.AttrTypeNum = GraphML_ATTR_IntHeight;
	   attr = TtaNewAttribute (attrType);
	   TtaAttachAttribute (newEl, attr, doc);
	   TtaSetAttributeValue (attr, h, newEl, doc);
	   UpdateWidthHeightAttribute (attr, newEl, doc);
	 }
       else
         TtaGiveBoxSize (parent, doc, 1, UnPoint, &w, &h);

       TtaChangeLimitOfPolyline (child, UnPoint, w, h, doc);
       TtcInsertGraph (doc, 1, shape);
       dispMode = TtaGetDisplayMode (doc);
       /* ask Thot to stop displaying changes made in the document */
       if (dispMode == DisplayImmediately)
         TtaSetDisplayMode (doc, DeferredDisplay);
       UpdatePointsAttribute (newEl, doc, &minX, &minY, &maxX, &maxY);
       UpdateInternalAttrForPoly (newEl, child, doc, minX, minY, maxX, maxY,
				  !automaticPlacement);
       /* ask Thot to display changes made in the document */
       TtaSetDisplayMode (doc, dispMode);
       /* switch on the current selection */
       /*TtaSwitchSelection (doc, 1, TRUE);*/
     }
   TtaCloseUndoSequence (doc);
   TtaSetDocumentModified (doc);
}

/*----------------------------------------------------------------------
   CreateGroup
   Create a Group surrounding the selected elements
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
   Attribute	attr;
   AttributeType	attrType;
   int		c1, i, posX, posY, minX, minY;
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

   /* switch off the current selection */
   /*TtaSwitchSelection (doc, 1, FALSE);*/
   dispMode = TtaGetDisplayMode (doc);
   /* ask Thot to stop displaying changes made in the document */
   if (dispMode == DisplayImmediately)
      TtaSetDisplayMode (doc, DeferredDisplay);

   prevSel = NULL;
   prevChild = NULL;
   /* Create a Group element */
   elType = TtaGetElementType (el);
   elType.ElTypeNum = GraphML_EL_Group;
   group = TtaNewElement (doc, elType);
   /* insert the new Group element */
   TtaInsertSibling (group, el, TRUE, doc);

   attrType.AttrSSchema = elType.ElSSchema;
   minX = minY = 32000;
   position = FALSE;
   while (el != NULL)
      {
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
   /* ask Thot to display changes made in the document */
   TtaSetDisplayMode (doc, dispMode);
   /* switch off the current selection */
   /*TtaSwitchSelection (doc, 1, FALSE);*/
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
