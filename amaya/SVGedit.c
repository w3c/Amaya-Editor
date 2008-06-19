/*
 *
 *  (c) COPYRIGHT INRIA and W3C, 1996-2008
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * This module contains editing functions for handling SVG objects.
 *
 * Author: I. Vatton
 *	   V. Quint
 */

#ifdef _WX
#include "wx/wx.h"
#endif /* _WX */

/* Included headerfiles */
#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "amaya.h"
#include "css.h"
#include "parser.h"
#include "trans.h"
#include "view.h"
#include "content.h"
#include "document.h"

#include "SVG.h"
#include "HTML.h"

#include "libmanag_f.h"
#include "anim_f.h"
#include "Mathedit_f.h"
#include "SVGedit_f.h"
#include "UIcss_f.h"
#include "templateUtils_f.h"

static ThotIcon   iconGraph;
static ThotIcon   iconGraphNo;
static ThotIcon   mIcons[12];
static ThotBool PaletteDisplayed = FALSE;
static ThotBool InCreation = FALSE;

#ifdef _WINGUI
#include "wininclude.h"
#define iconGraph 22
#define iconGraphNo 22
#endif /* _WINGUI */

#include "EDITimage_f.h"
#include "fetchXMLname_f.h"
#include "html2thot_f.h"
#include "HTMLactions_f.h"
#include "HTMLedit_f.h"
#include "HTMLpresentation_f.h"
#include "init_f.h"
#include "SVGbuilder_f.h"
#include "XLinkedit_f.h"
#include "Xmlbuilder_f.h"
#include <math.h>
#include "styleparser_f.h"

extern int ActiveFrame;

#ifdef _WX
#include "appdialogue_wx.h"
#include "paneltypes_wx.h"
#endif /* _WX */

#ifdef _GTK
/* used for the close palette callback*/
ThotWidget CatWidget(int ref);
#endif/*  _GTK */

#ifdef _WINDOWS
#include <commctrl.h>
#endif /* _WINDOWS */

#ifdef _SVG
/*----------------------------------------------------------------------
  StyleElemPasted
  An element style has been pasted.
  -----------------------------------------------------------------------*/
void StylePasted (NotifyElement *event)
{
  CreateCSSRules (event->element, event->document);
}

/*----------------------------------------------------------------------
  ExportForeignObject
  A foreignObject element will be generated in the output file.
  Associate a Namespace attribute with its child. This attribute will be
  generated with the child.
  -----------------------------------------------------------------------*/
ThotBool ExportForeignObject (NotifyElement *event)
{
#ifdef OLD
  /* useless with the support of xml namespaces */
  Element       child;
  ElementType   elType;
  Attribute     attr;
  AttributeType attrType;

  child = TtaGetFirstChild (event->element);
  while (child)
    {
      elType = TtaGetElementType (child);
      if (!strcmp (TtaGetSSchemaName (elType.ElSSchema), "HTML"))
        /* child is an HTML element */
        {
          attrType.AttrTypeNum = SVG_ATTR_Namespace;
          attrType.AttrSSchema = TtaGetElementType (event->element).ElSSchema;
          attr = TtaNewAttribute (attrType);
          TtaAttachAttribute (child, attr, event->document);
          TtaSetAttributeText (attr, "http://www.w3.org/1999/xhtml", child,
                               event->document);
        }
      TtaNextSibling (&child);
    }
#endif /* OLD */
  return FALSE; /* let Thot perform normal operation */
}

/*----------------------------------------------------------------------
  NameSpaceGenerated
  An attribute Namespace has been generated for a child of a foreign
  element. Delete that attribute.
  -----------------------------------------------------------------------*/
void NameSpaceGenerated (NotifyAttribute *event)
{
  TtaRemoveAttribute (event->element, event->attribute, event->document);
}

/*----------------------------------------------------------------------
  A new element has been selected.
  Check that this element can be selected.
  Synchronize selection in source view.      
  ----------------------------------------------------------------------*/
void GraphicsSelectionChanged (NotifyElement * event)
{
  Element      asc, use;
  ElementType  elType;
  int          elemType = 0;

  /* if element is within a "use" or "tref" element, select that element
     instead */
  use = NULL;
  asc = TtaGetParent (event->element);
  /* look for the highest level use ancestor */
  while (asc)
    {
      elType = TtaGetElementType (asc);
      if (event->elementType.ElSSchema == elType.ElSSchema)
        {
          if (elType.ElTypeNum == SVG_EL_use_ ||
              elType.ElTypeNum == SVG_EL_tref)
            {
              use = asc;
              elemType = elType.ElTypeNum;
            }
          else if (elType.ElTypeNum == SVG_EL_desc ||
                   elType.ElTypeNum == SVG_EL_title ||
                   elType.ElTypeNum == SVG_EL_metadata)
            /* even if a desc, title, or metadata is within a use or tref
               element, selection is allowed within the desc, title, or
               metadata element */
            asc = NULL;
        }
      if (asc)
        asc = TtaGetParent (asc);
    }
  if (use)
    /* there is a use ancestor. Select it */
    {
      TtaSelectElement (event->document, use);
      event->element = use;
      event->elementType.ElTypeNum = elemType;
    }
  CheckSynchronize (event);
  Selection_changed_in_basedoc (event);
  /* update the displayed style information */
  SynchronizeAppliedStyle (event);
  UnFrameMath ();
  
  UpdateXmlElementListTool(event->element,event->document);
  TtaSetStatusSelectedElement(event->document, 1, event->element);
  TtaRaisePanel(WXAMAYA_PANEL_SVG);
}

/*----------------------------------------------------------------------
  ExtendSelectSVGElement
  The user wants to add a new element in the current selection.
  -----------------------------------------------------------------------*/
ThotBool ExtendSelectSVGElement (NotifyElement *event)
{
  Element	firstSel, newFirstSel, ancestor, parent, selEl;
  ElementType	elType, ancestType, parentType;
  int		c1, i;
  SSchema	SvgSSchema;

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
  SvgSSchema = TtaGetSSchema ("SVG", event->document);
  ancestType = TtaGetElementType (ancestor);
  if (ancestType.ElSSchema != SvgSSchema)
    /* common ancestor is not a SVG element */
    {
      /* is the common ancestor within a SVG element? */
      parent = ancestor;
      do
        {
          parent = TtaGetParent (parent);
          if (parent != NULL)
            parentType = TtaGetElementType (parent);
        }
      while (parent != NULL && parentType.ElSSchema != SvgSSchema);
      if (parent)
        /* the common ancestor is within a SVG element. Let Thot
           perform normal operation: selection is being extended within
           a foreignObject */
        return FALSE;
      else
        return TRUE;	/* abort selection */
    }

  newFirstSel = firstSel;
  elType = TtaGetElementType (firstSel);
  if (elType.ElSSchema != SvgSSchema ||
      (elType.ElTypeNum != SVG_EL_g &&
       elType.ElTypeNum != SVG_EL_path &&
       elType.ElTypeNum != SVG_EL_rect &&
       elType.ElTypeNum != SVG_EL_circle_ &&
       elType.ElTypeNum != SVG_EL_ellipse &&
       elType.ElTypeNum != SVG_EL_line_ &&
       elType.ElTypeNum != SVG_EL_polyline &&
       elType.ElTypeNum != SVG_EL_polygon &&
       elType.ElTypeNum != SVG_EL_text_ &&
       elType.ElTypeNum != SVG_EL_image &&
       elType.ElTypeNum != SVG_EL_switch &&
       elType.ElTypeNum != SVG_EL_SVG))
    {
      elType.ElSSchema = SvgSSchema;
      elType.ElTypeNum = SVG_EL_GraphicsElement;
      newFirstSel = TtaGetTypedAncestor (newFirstSel, elType);
    }

  selEl = event->element;
  elType = TtaGetElementType (selEl);
  if (elType.ElSSchema != SvgSSchema ||
      (elType.ElTypeNum != SVG_EL_g &&
       elType.ElTypeNum != SVG_EL_path &&
       elType.ElTypeNum != SVG_EL_rect &&
       elType.ElTypeNum != SVG_EL_circle_ &&
       elType.ElTypeNum != SVG_EL_ellipse &&
       elType.ElTypeNum != SVG_EL_line_ &&
       elType.ElTypeNum != SVG_EL_polyline &&
       elType.ElTypeNum != SVG_EL_polygon &&
       elType.ElTypeNum != SVG_EL_text_ &&
       elType.ElTypeNum != SVG_EL_image &&
       elType.ElTypeNum != SVG_EL_switch &&
       elType.ElTypeNum != SVG_EL_SVG))
    {
      elType.ElSSchema = SvgSSchema;
      elType.ElTypeNum = SVG_EL_GraphicsElement;
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
void AttrCoordChanged (NotifyAttribute *event)
{
  ParseCoordAttribute (event->attribute, event->element, event->document);
}

/*----------------------------------------------------------------------
  AttrTransformChanged
  -----------------------------------------------------------------------*/
void AttrTransformChanged (NotifyAttribute *event)
{
  ParseTransformAttribute (event->attribute, event->element, event->document,
                           FALSE);
  /*******   CheckSVGRoot (event->document, event->element); *****/
}

/*----------------------------------------------------------------------
  AttrTransformDelete : attribute transform will be
  deleted. Remove the corresponding style presentation.
  ----------------------------------------------------------------------*/
ThotBool AttrTransformDelete (NotifyAttribute * event)
{
  ParseTransformAttribute (event->attribute, event->element, event->document,
                           TRUE);
  return FALSE; /* let Thot perform normal operation */
}

/*----------------------------------------------------------------------
  EvaluateSwitch
  An attribute requiredFeatures, requiredExtensions or systemLanguage
  has been created, modified or deleted.
  If the parent is a switch element, reevaluate the test attributes
  for all children of the switch element.
  -----------------------------------------------------------------------*/
void EvaluateSwitch (NotifyAttribute *event)
{
  Element         parent;
  ElementType     elType;

  parent = TtaGetParent (event->element);
  if (parent)
    {
      elType = TtaGetElementType (parent);
      if (elType.ElTypeNum == SVG_EL_switch &&
          !strcmp (TtaGetSSchemaName (elType.ElSSchema), "SVG"))
        EvaluateTestAttrs (parent, event->document);
    }
}

/*----------------------------------------------------------------------
  AttrPathDataChanged
  -----------------------------------------------------------------------*/
void AttrPathDataChanged (NotifyAttribute *event)
{
  ParsePathDataAttribute (event->attribute, event->element, event->document, TRUE);
}

/*----------------------------------------------------------------------
  AttrPathDataDelete
  The user tries to delete attribute d of a path element.
  Don't let him/her do that!
  ----------------------------------------------------------------------*/
ThotBool AttrPathDataDelete (NotifyAttribute * event)
{
  return TRUE; /* don't let Thot perform normal operation */
}

/*----------------------------------------------------------------------
  UpdateAttrText creates or updates the text attribute attr of the
  element el.
  The parameter delta is TRUE when the value is 
  The parameter update is TRUE when the attribute must be parsed after
  the change.
  -----------------------------------------------------------------------*/
static void UpdateAttrText (Element el, Document doc, AttributeType attrType,
                            int value, ThotBool delta, ThotBool update)
{
#define BUFFER_LENGTH 64
  char		buffer[BUFFER_LENGTH], unit[BUFFER_LENGTH];
  Attribute     attr;
  int           v, e;
  int           pval, pe, i;

  attr = TtaGetAttribute (el, attrType);
  if (attr == NULL)
    {
      /* it's a new attribute */
      attr = TtaNewAttribute (attrType);
      TtaAttachAttribute (el, attr, doc);

      /* by default generate pixel values */
      sprintf (buffer, "%dpx", value);
      TtaSetAttributeText (attr, buffer, el, doc);
      TtaRegisterAttributeCreate (attr, el, doc);
    }
  else
    {
      /* get the current unit */
      i = BUFFER_LENGTH - 1;
      TtaGiveTextAttributeValue (attr, buffer, &i);
      /* check if the value includes decimals */
      i = 0;
      v = 0;
      while (buffer[i] != EOS && !v)
        {
          if (buffer[i] == '.')
            {
              buffer[i] = EOS;
              sscanf (buffer, "%d", &pval);	      
              v = i + 1;
            }
          i++;
        }
      unit[0] = EOS;
      pe = 0;
      if (v)
        sscanf (&buffer[v], "%d%s", &pe, unit);
      else
        sscanf (buffer, "%d%s", &pval, unit);
      /* convert the value according to the current unit */
      if (!strcmp (unit, "em") || !strcmp (unit, "ex"))
        {
          v = value / 10;
          e = value - (v * 10);
        }
      else if (!strcmp (unit, "pc"))
        {
          v = value / 12;
          e = value - (v * 12);
        }
      else if (!strcmp (unit, "in"))
        {
          v = value / 72;
          e = value - (v * 72);
        }
      else if (!strcmp (unit, "cm"))
        {
          v = (value + 14) / 28;
          e = value - (v * 28);
        }
      else if (!strcmp (unit, "mm"))
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
        sprintf (buffer, "%d.%d%s", v, e, unit);
      else
        sprintf (buffer, "%d%s", v, unit);
      TtaRegisterAttributeReplace (attr, el, doc);
      TtaSetAttributeText (attr, buffer, el, doc);
    }
  if (update)
    {
      /* generate the specific presentation */
      if (attrType.AttrTypeNum == SVG_ATTR_x ||
          attrType.AttrTypeNum == SVG_ATTR_y ||
          attrType.AttrTypeNum == SVG_ATTR_cx ||
          attrType.AttrTypeNum == SVG_ATTR_cy ||
          attrType.AttrTypeNum == SVG_ATTR_x1 ||
          attrType.AttrTypeNum == SVG_ATTR_y1 ||
          attrType.AttrTypeNum == SVG_ATTR_x2 ||
          attrType.AttrTypeNum == SVG_ATTR_y2 ||
          attrType.AttrTypeNum == SVG_ATTR_dx ||
          attrType.AttrTypeNum == SVG_ATTR_dy)
        ParseCoordAttribute (attr, el, doc);
      else
        ParseWidthHeightAttribute (attr, el, doc, FALSE);
    }
}

/*----------------------------------------------------------------------
  UpdatePositionAttribute
  update position attributes (x, y, cx, cy, x1, y1) for element el according
  to parameter pos.
  If el is a foreignObject, update the position of the alternate text, if
  there is an alternate text.
  -----------------------------------------------------------------------*/
static void UpdatePositionAttribute (Element el, Document doc, int pos,
                                     ThotBool horiz)
{
  ElementType		elType, parentType, siblingType;
  AttributeType	        attrType;
  Attribute             attr, newAttr;
  Element               parent, sibling, textEl;
  int                   length;
  char                  *value;
  ThotBool              new_;

  elType = TtaGetElementType (el);
  attrType.AttrSSchema = elType.ElSSchema;
  if (elType.ElTypeNum == SVG_EL_circle_ ||
      elType.ElTypeNum == SVG_EL_ellipse)
    {
      if (horiz)
        attrType.AttrTypeNum = SVG_ATTR_cx;
      else
        attrType.AttrTypeNum = SVG_ATTR_cy;
    }
  else if (elType.ElTypeNum == SVG_EL_rect ||
           elType.ElTypeNum == SVG_EL_text_ ||
           elType.ElTypeNum == SVG_EL_tspan ||
           elType.ElTypeNum == SVG_EL_image ||
           elType.ElTypeNum == SVG_EL_foreignObject ||
           elType.ElTypeNum == SVG_EL_SVG)
    {
      /* move the origin */
      if (horiz)
        attrType.AttrTypeNum = SVG_ATTR_x;
      else
        attrType.AttrTypeNum = SVG_ATTR_y;
    }
  else if (elType.ElTypeNum == SVG_EL_line_)
    {
      if (horiz)
        attrType.AttrTypeNum = SVG_ATTR_x1;
      else
        attrType.AttrTypeNum = SVG_ATTR_y1;
    }
  else
    /* no attribute available */
    return;

  UpdateAttrText (el, doc, attrType, pos, FALSE, FALSE);

  if (elType.ElTypeNum == SVG_EL_foreignObject)
    /* it's a foreignObject. If it's a child of a switch element and if
       its next sibling a SVG text element, this sibling is considered as
       an alternate text and its position is updated to be the same */
    {
      parent = TtaGetParent (el);
      if (el)
        {
          parentType = TtaGetElementType (parent);
          if (parentType.ElTypeNum == SVG_EL_switch &&
              parentType.ElSSchema == elType.ElSSchema)
            /* it's a child of a switch element */
            {
              textEl = NULL;
              sibling = el;
              while (sibling)
                {
                  TtaNextSibling (&sibling);
                  siblingType = TtaGetElementType (sibling);
                  if (siblingType.ElTypeNum == SVG_EL_text_ &&
                      siblingType.ElSSchema == elType.ElSSchema)
                    {
                      textEl = sibling;
                      sibling = NULL;
                    }
                  else if ((siblingType.ElTypeNum != SVG_EL_XMLcomment &&
                            siblingType.ElTypeNum != SVG_EL_XMLPI) ||
                           siblingType.ElSSchema != elType.ElSSchema)
                    sibling = NULL;
                }
              if (textEl)
                /* the foreignObject is followed by a text. Copy attribute */
                {
                  attr = TtaGetAttribute (el, attrType);
                  if (attr)
                    {
                      newAttr = TtaGetAttribute (textEl, attrType);
                      new_ = !newAttr;
                      if (!newAttr)
                        {
                          newAttr = TtaNewAttribute (attrType);
                          TtaAttachAttribute (textEl, newAttr, doc);
                        }
                      length = TtaGetTextAttributeLength (attr);
                      value = (char *)TtaGetMemory (length + 1);
                      TtaGiveTextAttributeValue (attr, value, &length);
                      if (!new_)
                        TtaRegisterAttributeReplace (newAttr, textEl, doc);
                      TtaSetAttributeText (newAttr, value, textEl, doc);
                      if (new_)
                        TtaRegisterAttributeCreate (newAttr, textEl, doc);
                      TtaFreeMemory (value);
                    }
                } 
            } 
        } 
    }
}

/*----------------------------------------------------------------------
  AttrWidthHeightChanged
  -----------------------------------------------------------------------*/
void AttrWidthHeightChanged (NotifyAttribute *event)
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
static void UpdateWidthHeightAttribute (Element el, Document doc, int dim,
                                        ThotBool horiz)
{
  ElementType		elType;
  AttributeType	        attrType;
  float                 val;
  int                   width, height;

  elType = TtaGetElementType (el);
  attrType.AttrSSchema = TtaGetSSchema ("SVG", doc);//elType.ElSSchema;
  if (elType.ElTypeNum == SVG_EL_circle_)
    {
      /* express width or height as a radius */
      dim /= 2;
      attrType.AttrTypeNum = SVG_ATTR_r;
      UpdateAttrText (el, doc, attrType, dim, FALSE, FALSE);
    }
  else if (elType.ElTypeNum == SVG_EL_ellipse)
    {
      /* express width or height as a radius */
      dim /= 2;
      if (horiz)
        attrType.AttrTypeNum = SVG_ATTR_rx;
      else
        attrType.AttrTypeNum = SVG_ATTR_ry;
      UpdateAttrText (el, doc, attrType, dim, FALSE, FALSE);
    }
  else if (elType.ElTypeNum == SVG_EL_SVG ||
           elType.ElTypeNum == SVG_EL_rect ||
           elType.ElTypeNum == SVG_EL_image ||
           elType.ElTypeNum == SVG_EL_foreignObject ||
           elType.ElTypeNum == SVG_EL_SVG)
    {
      if (horiz)
        attrType.AttrTypeNum = SVG_ATTR_width_;
      else
        attrType.AttrTypeNum = SVG_ATTR_height_;
      UpdateAttrText (el, doc, attrType, dim, FALSE, FALSE);
    }
  else if (elType.ElTypeNum == SVG_EL_line_)
    {
      if (horiz)
        attrType.AttrTypeNum = SVG_ATTR_x2;
      else
        attrType.AttrTypeNum = SVG_ATTR_y2;
      UpdateAttrText (el, doc, attrType, dim, FALSE, FALSE);
    }
  else if (elType.ElTypeNum == SVG_EL_polyline ||
           elType.ElTypeNum == SVG_EL_polygon)
    {
      /* make it a transform (scale) attribute */
      TtaGiveBoxSize (el, doc, 1, UnPixel, &width, &height);
      val = 0;
      if (horiz && width != 0)
        val = (float)dim / (float)width;
      else if (height != 0)
        val = (float)dim / (float)height;
      if (fabs (val) > 0.00001)
        UpdateTransformAttr (el, doc, "scale", val, horiz, FALSE);
    }
  else
    /* no attribute available */
    return;
}

/*----------------------------------------------------------------------
  AttrCSSequivModified
  -----------------------------------------------------------------------*/
void AttrCSSequivModified(NotifyAttribute *event)
{
  ParseCSSequivAttribute (event->attributeType.AttrTypeNum,
                          event->attribute, event->element,
                          event->document, FALSE);
}
 
/*----------------------------------------------------------------------
  AttrCSSequivDelete : attribute fill, stroke or stroke-width will be
  deleted. Remove the corresponding style presentation.
  ----------------------------------------------------------------------*/
ThotBool AttrCSSequivDelete (NotifyAttribute * event)
{
  ParseCSSequivAttribute (event->attributeType.AttrTypeNum,
                          event->attribute, event->element,
                          event->document, TRUE);
  return FALSE; /* let Thot perform normal operation */
}

/*----------------------------------------------------------------------
  AttrTextAnchorModified
  -----------------------------------------------------------------------*/
void AttrTextAnchorModified (NotifyAttribute *event)
{
  SetTextAnchor (event->attribute, event->element, event->document, FALSE);
}
 
/*----------------------------------------------------------------------
  AttrTextAnchorDelete : attribute text_anchor will be
  deleted. Remove the corresponding presentation.
  ----------------------------------------------------------------------*/
ThotBool AttrTextAnchorDelete (NotifyAttribute * event)
{
  SetTextAnchor (event->attribute, event->element, event->document, TRUE);
  return FALSE; /* let Thot perform normal operation */
}

/*----------------------------------------------------------------------
  DeleteAttrPoints
  -----------------------------------------------------------------------*/
ThotBool DeleteAttrPoints (NotifyAttribute *event)
{
  /* prevents Thot from deleting the points attribute */
  return TRUE;
}

/*----------------------------------------------------------------------
  AttrPointsModified
  -----------------------------------------------------------------------*/
void AttrPointsModified (NotifyAttribute *event)
{
  ParsePointsAttribute (event->attribute, event->element, event->document);
}

/*----------------------------------------------------------------------
  AttrCoordDelete
  -----------------------------------------------------------------------*/
ThotBool AttrCoordDelete (NotifyAttribute *event)
{
  int                  ruleType;
  Attribute            attr;
  AttributeType        attrType;
  ElementType          elType;
  PresentationValue    pval;
  PresentationContext  ctxt;
  ThotBool             mainVal;

  elType = TtaGetElementType (event->element);
  attrType.AttrSSchema = elType.ElSSchema;
  if (event->attributeType.AttrTypeNum == SVG_ATTR_x)
    {
      ruleType = PRHorizPos;
      attrType.AttrTypeNum = SVG_ATTR_dx;
      mainVal = TRUE;
    }
  else if (event->attributeType.AttrTypeNum == SVG_ATTR_y)
    {
      ruleType = PRVertPos;
      attrType.AttrTypeNum = SVG_ATTR_dy;
      mainVal = TRUE;
    }
  else if (event->attributeType.AttrTypeNum == SVG_ATTR_dx)
    {
      ruleType = PRHorizPos;
      attrType.AttrTypeNum = SVG_ATTR_x;
      mainVal = FALSE;
    }
  else if (event->attributeType.AttrTypeNum == SVG_ATTR_dy)
    {
      ruleType = PRVertPos;
      attrType.AttrTypeNum = SVG_ATTR_y;
      mainVal = FALSE;
    }
  else
    return (FALSE);

  /* set the corresponding field in the position rule to zero */
  ctxt = TtaGetSpecificStyleContext (event->document);
  ctxt->cssSpecificity = 0;
  pval.typed_data.value = 0;
  pval.typed_data.unit = UNIT_PX;
  pval.typed_data.mainValue = mainVal;
  ctxt->destroy = FALSE;
  TtaSetStylePresentation (ruleType, event->element, NULL, ctxt, pval);
  /* if there no x attribute (for dx) or y attribute (for dy), remove the
     PRule */
  attr = TtaGetAttribute (event->element, attrType);
  if (!attr)
    {
      ctxt->destroy = TRUE;
      TtaSetStylePresentation (ruleType, event->element, NULL, ctxt, pval);
    }
  TtaFreeMemory (ctxt);
  return FALSE; /* let Thot perform normal operation */
}

/*----------------------------------------------------------------------
  AttrBaselineShiftChanged
  -----------------------------------------------------------------------*/
void AttrBaselineShiftChanged (NotifyAttribute *event)
{
  ParseBaselineShiftAttribute (event->attribute, event->element,
                               event->document, FALSE);
}

/*----------------------------------------------------------------------
  AttrBaselineShiftDelete
  -----------------------------------------------------------------------*/
ThotBool AttrBaselineShiftDelete (NotifyAttribute *event)
{
  ParseBaselineShiftAttribute (event->attribute, event->element,
                               event->document, TRUE);
  return FALSE; /* let Thot perform normal operation */
}

#endif /* _SVG */

/*----------------------------------------------------------------------
  CheckSVGRoot checks that the svg root element includes element el.
  -----------------------------------------------------------------------*/
void CheckSVGRoot (Document doc, Element el)
{
#ifdef _SVG
  Element          SvgRoot, child;
  ElementType      elType;
  AttributeType    attrType;
  SSchema	   SvgSchema;
  PRule            rule;
  TypeUnit         unit;
  int              x, y, w, h, val;
  int              wR, hR, dummy;
  int              dw,dh;

  dw = 0;
  dh = 0;
  SvgSchema = GetSVGSSchema (doc);
  elType.ElTypeNum = SVG_EL_SVG;
  elType.ElSSchema = SvgSchema;
  attrType.AttrSSchema = SvgSchema;
  SvgRoot = TtaGetTypedAncestor (el, elType);
  while (SvgRoot)
    {
      /* check first the position of the new element in pixel value */
      TtaGiveBoxPosition (el, doc, 1, UnPixel, &x, &y);
      /* get the unit of the SVG width */
      rule = TtaGetPRule (SvgRoot, PRWidth);
      if (rule)
        unit = (TypeUnit)TtaGetPRuleUnit (rule);
      else
        unit = UnPixel;
      dh = dw = 0;
      TtaGiveBoxSize (SvgRoot, doc, 1, UnPixel, &wR, &dummy);
      elType = TtaGetElementType (el);
      if (x < 0 && elType.ElTypeNum != SVG_EL_tspan)
        {
          /* translate the whole SVG contents */
          child = TtaGetFirstChild (SvgRoot);
          val = -x;
          while (child)
            {
              elType = TtaGetElementType (child);
              if (elType.ElTypeNum == SVG_EL_polyline ||
                  elType.ElTypeNum == SVG_EL_polygon)
                TranslateElement (el, doc, val, UnPixel, TRUE, FALSE);
              else
                {
                  if (elType.ElTypeNum == SVG_EL_circle_ ||
                      elType.ElTypeNum == SVG_EL_ellipse)
                    attrType.AttrTypeNum = SVG_ATTR_cx;
                  else if (elType.ElTypeNum == SVG_EL_rect ||
                           elType.ElTypeNum == SVG_EL_text_ ||
                           elType.ElTypeNum == SVG_EL_tspan ||
                           elType.ElTypeNum == SVG_EL_image ||
                           elType.ElTypeNum == SVG_EL_foreignObject ||
                           elType.ElTypeNum == SVG_EL_SVG)
                    attrType.AttrTypeNum = SVG_ATTR_x;
                  else if (elType.ElTypeNum == SVG_EL_line_)
                    {
                      attrType.AttrTypeNum = SVG_ATTR_x1;
                      /************** convert to the right value */
                      UpdateAttrText (child, doc, attrType, val, TRUE, TRUE);
                      attrType.AttrTypeNum = SVG_ATTR_x2;
                    }
                  else
                    attrType.AttrTypeNum = 0;
                  if (attrType.AttrTypeNum != 0)
                    /************** convert to the right value */
                    UpdateAttrText (child, doc, attrType, val, TRUE, TRUE);
                }
              /* check if the SVG width includes that element */
              TtaGiveBoxPosition (child, doc, 1, UnPixel, &x, &dummy);
              TtaGiveBoxSize (child, doc, 1, UnPixel, &w, &h);
              TtaGiveBoxSize (SvgRoot, doc, 1, UnPixel, &wR, &hR);
              dw = w + x - wR;
              if (dw > 0)
                {
                  /* increase the width of the SVG element */
                  attrType.AttrTypeNum = SVG_ATTR_width_;
                  /************** convert to the right value */
                  UpdateAttrText (SvgRoot, doc, attrType, x + w, FALSE, TRUE);
                }
              /* next element */
              TtaNextSibling (&child);
            }
        }
      else if ( elType.ElTypeNum != SVG_EL_line_)
        {
          /* check if the SVG width includes that element */
          TtaGiveBoxPosition (el, doc, 1, UnPixel, &x, &dummy);
          TtaGiveBoxSize (el, doc, 1, UnPixel, &w, &h);
          dw = w + x - wR;
          if (dw > 0)
            {
              /* increase the width of the SVG element */
              attrType.AttrTypeNum = SVG_ATTR_width_;
              /************** convert to the right value */
              UpdateAttrText (SvgRoot, doc, attrType, x + w, FALSE, TRUE);
            }
        }
      /* get the unit of the SVG width */
      rule = TtaGetPRule (SvgRoot, PRHeight);
      if (rule)
        unit = (TypeUnit)TtaGetPRuleUnit (rule);
      else
        unit = UnPixel;
      TtaGiveBoxSize (SvgRoot, doc, 1, UnPixel, &dummy, &hR);
      elType = TtaGetElementType (el);
      if (y < 0 && elType.ElTypeNum != SVG_EL_tspan)
        {
          /* translate the whole SVG contents */
          child = TtaGetFirstChild (SvgRoot);
          val = -y;
          while (child)
            {
              elType = TtaGetElementType (child);
              if (elType.ElTypeNum == SVG_EL_polyline ||
                  elType.ElTypeNum == SVG_EL_polygon)
                TranslateElement (el, doc, val, UnPixel, FALSE, FALSE);
              else
                {
                  if (elType.ElTypeNum == SVG_EL_circle_ ||
                      elType.ElTypeNum == SVG_EL_ellipse)
                    attrType.AttrTypeNum = SVG_ATTR_cy;
                  else if (elType.ElTypeNum == SVG_EL_rect ||
                           elType.ElTypeNum == SVG_EL_text_ ||
                           elType.ElTypeNum == SVG_EL_tspan ||
                           elType.ElTypeNum == SVG_EL_image ||
                           elType.ElTypeNum == SVG_EL_foreignObject ||
                           elType.ElTypeNum == SVG_EL_SVG)
                    attrType.AttrTypeNum = SVG_ATTR_y;
                  else if (elType.ElTypeNum == SVG_EL_line_)
                    {
                      attrType.AttrTypeNum = SVG_ATTR_y1;
                      /************** convert to the right value */
                      UpdateAttrText (child, doc, attrType, val, TRUE, TRUE);
                      attrType.AttrTypeNum = SVG_ATTR_y2;
                    }
                  else
                    attrType.AttrTypeNum = 0;
                  if (attrType.AttrTypeNum != 0)
                    /************** convert to the right value */
                    UpdateAttrText (child, doc, attrType, val, TRUE, TRUE);
                }
              /* check if the SVG height includes that element */
              TtaGiveBoxPosition (child, doc, 1, UnPixel, &dummy, &y);
              TtaGiveBoxSize (child, doc, 1, UnPixel, &w, &h);
              TtaGiveBoxSize (SvgRoot, doc, 1, UnPixel, &wR, &hR);
              dh = h + y - hR;
              if (dh > 0)
                {
                  /* increase the height of the root element */
                  attrType.AttrTypeNum = SVG_ATTR_height_;
                  /************** convert to the right value */
                  UpdateAttrText (SvgRoot, doc, attrType, y + h, FALSE, TRUE);
                }
              /* next element */
              TtaNextSibling (&child);
            }
        }
      else if ( elType.ElTypeNum != SVG_EL_line_)
        {
          /* check if the SVG height includes that element */
          TtaGiveBoxPosition (el, doc, 1, UnPixel, &dummy, &y);
          TtaGiveBoxSize (el, doc, 1, UnPixel, &w, &h);
          dh = h + y - hR;
          if (dh > 0)
            {
              /* increase the height of the root element */
              attrType.AttrTypeNum = SVG_ATTR_height_;
              /************** convert to the right value */
              UpdateAttrText (SvgRoot, doc, attrType, y + h, FALSE, TRUE);
            }
        }

      if (dw > 0 || dh > 0)
        {
          if (dw < 0)
            dw = 0;
          if (dh < 0)
            dh = 0;
          /* handle included polylines */
          child = TtaGetFirstChild (SvgRoot);
          while (child)
            {
              elType = TtaGetElementType (child);
              if (elType.ElTypeNum == SVG_EL_polyline ||
                  elType.ElTypeNum == SVG_EL_polygon)
                /************** convert to the right value */
                UpdatePositionOfPoly (child, doc, 0, 0, wR + dw, hR + dh);
              /* next element */
              TtaNextSibling (&child);
            }
        }

      /* check enclosing SGV */
      el = TtaGetParent (SvgRoot);
      if (el)
        SvgRoot = TtaGetTypedAncestor (el, elType);
      else
        SvgRoot = NULL;
    }
#endif /* _SVG */
}

#ifdef _SVG
/*----------------------------------------------------------------------
  NewGraphElement
  An element will be pasted
  -----------------------------------------------------------------------*/
ThotBool NewGraphElem (NotifyOnValue *event)
{
  int           profile;

  // is it a compound document?
  profile = TtaGetDocumentProfile (event->document);
  if (profile == L_Strict || profile == L_Basic)
    {
      /* cannot insert here */
      TtaDisplaySimpleMessage (CONFIRM, AMAYA, AM_NOT_ALLOWED);
      return TRUE;
    }
  else
    return FALSE; /* let Thot perform normal operation */
}

/*----------------------------------------------------------------------
  GraphElemPasted
  An element has been pasted.
  If the element is an XLink, update the link.
  -----------------------------------------------------------------------*/
void GraphElemPasted (NotifyElement *event)
{
  ElementType    elType;
  SSchema	       SvgSchema;
  AttributeType  attrType;
  Attribute      attr;
  Element        parent;
  int            profile;

  XLinkPasted (event);
  /* check that the svg element includes that element */
  /*****  CheckSVGRoot (event->document, event->element); ****/
  SetGraphicDepths (event->document, event->element);

  // it's a compound document
  profile = TtaGetDocumentProfile (event->document);
  if (DocumentTypes[event->document] == docMath ||
      profile == L_Strict || profile == L_Basic)
    return;
  else if (DocumentTypes[event->document] != docSVG &&
           DocumentMeta[event->document])
    DocumentMeta[event->document]->compound = TRUE;

  /* Set the namespace declaration if it's an <svg> element that is not
     within an element belonging to the SVG namespace */
  SvgSchema = GetSVGSSchema (event->document);
  elType = TtaGetElementType (event->element);
  if (elType.ElTypeNum == SVG_EL_SVG && elType.ElSSchema == SvgSchema)
    /* it's an <svg> element */
    {
      parent = TtaGetParent (event->element);
      if (parent)
        {
          elType = TtaGetElementType (parent);
          if (elType.ElSSchema != SvgSchema)
            /* the parent element is not in the SVG namespace */
            {
              /* Put a namespace declaration on the pasted <svg> element */
              TtaSetUriSSchema (elType.ElSSchema, SVG_URI);
              TtaSetANamespaceDeclaration (event->document, event->element,
                                           NULL, SVG_URI);
              /* put a version attribute on the <svg> element */
              attrType.AttrSSchema = SvgSchema;
              attrType.AttrTypeNum = SVG_ATTR_version;
              attr = TtaNewAttribute (attrType);
              TtaAttachAttribute (event->element, attr, event->document);
              TtaSetAttributeText (attr, SVG_VERSION, event->element,
                                   event->document);
            }
        }
    }
  else if (elType.ElTypeNum == SVG_EL_title &&
           elType.ElSSchema == SvgSchema)
    /* the pasted element is a title element. Update the window title */
    UpdateTitle (event->element, event->document);

  /* Check attribute NAME or ID in order to make sure that its value */
  /* is unique in the document */
  MakeUniqueName (event->element, event->document, TRUE, FALSE);
}

/*----------------------------------------------------------------------
  SVGTextPasted
  Some text has been pasted in a text leaf.
  ----------------------------------------------------------------------*/
void SVGTextPasted (NotifyElement *event)
{
  Element      parent;
  ElementType  elType;

  parent = TtaGetParent (event->element);
  if (parent)
    {
      /* remove all attributes attached to the pasted HTML_EL_TEXT_UNIT */
      RemoveTextAttributes (event->element, event->document);
      elType = TtaGetElementType (parent);
      if (elType.ElTypeNum == SVG_EL_title &&
          !strcmp (TtaGetSSchemaName (elType.ElSSchema), "SVG"))
        /* the parent of the text leaf is a title element. If it's the main
           title of the SVG document, update the window title */
        UpdateTitle (parent, event->document);
      else
        /* the parent of the text leaf is not a title. Check if it's a PI
           referring to a style sheet */
        XmlStyleSheetPasted (event);
    }
}

/*----------------------------------------------------------------------
  GlobalSVGAttrInMenu
  Called by Thot when building the Attributes menu.
  Prevent Thot from including a global attribute in the menu if the selected
  element does not accept this attribute.
  ----------------------------------------------------------------------*/
ThotBool GlobalSVGAttrInMenu (NotifyAttribute * event)
{
  ElementType         elType, parentType;
  Element             parent;
  const char         *attr;

  elType = TtaGetElementType (event->element);

  /* don't put any attribute on Thot elements that are not SVG elements */
  if (elType.ElTypeNum == SVG_EL_XMLcomment ||
      elType.ElTypeNum == SVG_EL_XMLcomment_line ||
      elType.ElTypeNum == SVG_EL_XMLPI ||
      elType.ElTypeNum == SVG_EL_XMLPI_line ||
      elType.ElTypeNum == SVG_EL_Unknown_namespace ||
      elType.ElTypeNum == SVG_EL_DOCTYPE ||
      elType.ElTypeNum == SVG_EL_DOCTYPE_line)
    return TRUE;

  /* don't put any attribute on text fragments that are within DOCTYPE,
     comments, PIs, etc. */
  if (elType.ElTypeNum == SVG_EL_TEXT_UNIT)
    {
      parent = TtaGetParent (event->element);
      if (parent)
        {
          parentType = TtaGetElementType (parent);
          if (parentType.ElTypeNum == SVG_EL_XMLcomment ||
              parentType.ElTypeNum == SVG_EL_XMLcomment_line ||
              parentType.ElTypeNum == SVG_EL_XMLPI ||
              parentType.ElTypeNum == SVG_EL_XMLPI_line ||
              parentType.ElTypeNum == SVG_EL_Unknown_namespace ||
              parentType.ElTypeNum == SVG_EL_DOCTYPE ||
              parentType.ElTypeNum == SVG_EL_DOCTYPE_line)
            return TRUE;
        }
    }

  attr = GetXMLAttributeName (event->attributeType, elType, event->document);
  if (attr[0] == EOS)
    return TRUE;	/* don't put an invalid attribute in the menu */

  /* handle only Global attributes */
  if (event->attributeType.AttrTypeNum != SVG_ATTR_id &&
      event->attributeType.AttrTypeNum != SVG_ATTR_class &&
      event->attributeType.AttrTypeNum != SVG_ATTR_style_ &&
      event->attributeType.AttrTypeNum != SVG_ATTR_xml_space)
    /* it's not a global attribute. Accept it */
#ifdef TEMPLATES
    return ValidateTemplateAttrInMenu(event);
#else /* TEMPLATES */
    return FALSE;
#endif /* TEMPLATES */

  if (strcmp (TtaGetSSchemaName (elType.ElSSchema),"SVG"))
    /* it's not a SVG element, don't put a SVG attribute in the menu */
    return TRUE;

#ifdef TEMPLATES
  return ValidateTemplateAttrInMenu(event);
#else /* TEMPLATES */
  return FALSE;
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
  UpdateStyleOrSvgAttr
  update (or create) the style attribute or the SVG attribute corresponding
  the presentation rule of type presType for element el.
  -----------------------------------------------------------------------*/
static void UpdateStyleOrSvgAttr (int presType, Element el, Document doc)
{

  ElementType    elType;
  AttributeType  attrType;
  Attribute      attr;

  elType = TtaGetElementType (el);
  attrType.AttrSSchema = elType.ElSSchema;
  switch (presType)
    {
    case PRSize:
      attrType.AttrTypeNum = SVG_ATTR_font_size;
      break;
    case PRStyle:
      attrType.AttrTypeNum = SVG_ATTR_font_style;
      break;
    case PRWeight:
      attrType.AttrTypeNum = SVG_ATTR_font_weight;
      break;
    case PRFont:
      attrType.AttrTypeNum = SVG_ATTR_font_family;
      break;
    case PRLineWeight:
      attrType.AttrTypeNum = SVG_ATTR_stroke_width;
      break;
    case PRBackground:
      attrType.AttrTypeNum = SVG_ATTR_fill;
      break;
    case PRForeground:
      attrType.AttrTypeNum = SVG_ATTR_stroke;
      break;
    case PRFillPattern:
      attrType.AttrTypeNum = 0;
      break;
    case PRLineStyle:
      attrType.AttrTypeNum = 0;
      break;
    default:
      attrType.AttrTypeNum = 0;
      break;
    }
  /* is there already an attribute for the same property? */
  if (attrType.AttrTypeNum == 0)
    attr = NULL;
  else
    attr = TtaGetAttribute (el, attrType);
  if (attr)
    /* There is an SVG attribute for the same property. Remove it */
    {
      TtaRegisterAttributeDelete (attr, el, doc);
      TtaRemoveAttribute (el, attr, doc);
    }
  SetStyleAttribute (doc, el);
  TtaSetDocumentModified (doc);
}

/*----------------------------------------------------------------------
  GraphicsPRuleChange
  A presentation rule is going to be changed by Thot.
  -----------------------------------------------------------------------*/
ThotBool GraphicsPRuleChange (NotifyPresentation *event)
{
  Element       el, span, sibling;
  PRule         presRule;
  Document      doc;
  ElementType   elType;
  TypeUnit      unit;
  int           presType;
  int           mainView;
  int           x, y, width, height;
 
  el = event->element;
  elType = TtaGetElementType (el);
  doc = event->document;
  if (elType.ElSSchema != GetSVGSSchema (doc))
    return (FALSE); /* let Thot perform normal operation */

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
  /* if a style property is being changed, we have its new value in the
     PRule to set the corresponding SVG style attribute, but if its a
     change in the geometry, we have the old value to see the
     difference */
  if (TtaGetConstruct (el) == ConstructBasicType)
    /* it's a basic type. Move the PRule to the parent element if it is
       the only child of its parent. Otherwise, create a tspan element */
    {
      sibling = el;
      TtaNextSibling (&sibling);
      if (!sibling)
        {
          sibling = el;
          TtaPreviousSibling (&sibling);
        }
      if (sibling && MakeASpan (el, &span, doc, presRule))
        el = span;
      else
        {
          el = TtaGetParent (el);
          MovePRule (presRule, event->element, el, doc, FALSE);
        }
    }

  if (presType != PRVertPos && presType != PRHorizPos &&
      presType != PRHeight  && presType != PRWidth)
    UpdateStyleOrSvgAttr (presType, el, doc);
  else
    {
      unit = (TypeUnit)TtaGetPRuleUnit (presRule);
      mainView = TtaGetViewFromName (doc, "Formatted_view");
      if (presType == PRVertPos)
        {
          /* the new value is the old one plus the difference */
          y = event->value;
          if (elType.ElTypeNum == SVG_EL_polyline ||
              elType.ElTypeNum == SVG_EL_polygon)
            TranslateElement (el, doc, y, unit, FALSE, FALSE);
          else
            UpdatePositionAttribute (el, doc, y, FALSE);
        }
      else if (presType == PRHorizPos)
        {
          /* the new value is the old one plus the difference */
          x = event->value;
          if (elType.ElTypeNum == SVG_EL_polyline ||
              elType.ElTypeNum == SVG_EL_polygon)
            TranslateElement (el, doc, x, unit, TRUE, FALSE);
          else
            UpdatePositionAttribute (el, doc, x, TRUE);
        }
      else if (presType == PRHeight &&
               (elType.ElTypeNum == SVG_EL_SVG ||
                elType.ElTypeNum == SVG_EL_rect ||
                elType.ElTypeNum == SVG_EL_ellipse ||
                elType.ElTypeNum == SVG_EL_polyline ||
                elType.ElTypeNum == SVG_EL_polygon ||
                elType.ElTypeNum == SVG_EL_line_ ||
                elType.ElTypeNum == SVG_EL_image ||
                elType.ElTypeNum == SVG_EL_foreignObject))
        {
          /* the new value is the old one plus the delta */
          height = event->value;
          UpdateWidthHeightAttribute (el, doc, height, FALSE);
        }
      else if (presType == PRWidth &&
               (elType.ElTypeNum == SVG_EL_SVG ||
                elType.ElTypeNum == SVG_EL_rect ||
                elType.ElTypeNum == SVG_EL_circle_ ||
                elType.ElTypeNum == SVG_EL_ellipse ||
                elType.ElTypeNum == SVG_EL_polyline ||
                elType.ElTypeNum == SVG_EL_polygon ||
                elType.ElTypeNum == SVG_EL_line_ ||
                elType.ElTypeNum == SVG_EL_image ||
                elType.ElTypeNum == SVG_EL_foreignObject))
        {
          /* the new value is the old one plus the delta */
          width = event->value;
          UpdateWidthHeightAttribute (el, doc, width, TRUE);
        }
    }
  return FALSE; /* let Thot perform normal operation */
}

/*----------------------------------------------------------------------
  GraphicsPRuleDeleted
  A specific presentation rule has been deleted by the user, update
  the "style" attribute accordingly.
  -----------------------------------------------------------------------*/
void GraphicsPRuleDeleted (NotifyPresentation *event)
{
  ElementType    elType;
  Document       doc;
  Element        el;
  int            presType;

  el = event->element;
  elType = TtaGetElementType (el);
  doc = event->document;
  if (elType.ElSSchema != GetSVGSSchema (doc))
    return;

  presType = event->pRuleType;
  if (presType == PRSize       || presType == PRStyle      ||
      presType == PRWeight     || presType == PRFont       ||
      presType == PRLineStyle  || presType == PRLineWeight ||
      presType == PRBackground || presType == PRForeground ||
      presType == PRFillPattern)
    UpdateStyleOrSvgAttr (presType, el, doc);
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
  DisplayMode     dispMode;
  char           *text, *buffer;
  int             i, length;
  int             x, y, minX, minY, maxX, maxY;

  el = event->element;
  elType = TtaGetElementType (el);
  doc = event->document;
  attrType.AttrSSchema = elType.ElSSchema;
  if (elType.ElSSchema != GetSVGSSchema (doc))
    return;
  if (elType.ElTypeNum == SVG_EL_polyline ||
      elType.ElTypeNum == SVG_EL_polygon)
    {
      child = TtaGetFirstChild (el);
      length = TtaGetPolylineLength (child);
      /* get all points */
      buffer = (char *)TtaGetMemory (20);
      text = (char *)TtaGetMemory (length * 20);
      text[0] = EOS;
      minX = minY = 32000;
      maxX = maxY = 0;
      i = 1;
      while (i <= length)
        {
          TtaGivePolylinePoint (child, i, UnPixel, &x, &y);
          if (x > maxX)
            maxX = x;
          if (x < minX)
            minX = x;
          if (y > maxY)
            maxY = y;
          if (y < minY)
            minY = y;
          sprintf (buffer, "%d,%d", x, y);
          strcat (text, buffer);
          if (i < length)
            strcat (text, " ");
          i++;
        }
      TtaFreeMemory (buffer);

      dispMode = TtaGetDisplayMode (doc);
      /* ask Thot to stop displaying changes made to the document*/
      if (dispMode == DisplayImmediately)
        TtaSetDisplayMode (doc, DeferredDisplay);

      attrType.AttrTypeNum = SVG_ATTR_points;
      attr = TtaGetAttribute (el, attrType);
      if (attr == NULL)
        {
          /* it's a new attribute */
          attr = TtaNewAttribute (attrType);
          TtaAttachAttribute (el, attr, doc);
        }
      TtaSetAttributeText (attr, text, el, doc);
      TtaFreeMemory (text);
      UpdatePositionOfPoly (el, doc, minX, minY, maxX, maxY);
      TtaSetDisplayMode (doc, dispMode);
    }
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
  InheritPRule
  Check if any ancestor of element el has a PRule of type property
  and return the PRule found or NULL if not found.
  -----------------------------------------------------------------------*/
/* static PRule InheritPRule (Element el, int property) */
/* { */
/*   Element     asc; */
/*   PRule       inheritedPRule; */

/*   inheritedPRule = NULL; */
/*   asc = TtaGetParent (el); */
/*   while (asc && !inheritedPRule) */
/*     { */
/*       inheritedPRule = TtaGetPRule (asc, property); */
/*       if (!inheritedPRule) */
/*         asc = TtaGetParent (asc); */
/*     } */
/*   return (inheritedPRule); */
/* } */

/*----------------------------------------------------------------------
  InheritAttribute
  Check if any ancestor of element el has an attribute of type attrType
  and return the attribute found or NULL if not found.
  Check only ancestors defined in the same Thot schema (aka namespace) as
  element el.
  -----------------------------------------------------------------------*/
static Attribute InheritAttribute (Element el, AttributeType attrType)
{
  Element     asc;
  SSchema     sch;
  Attribute   inheritedAttr;

  inheritedAttr = NULL;
  sch = TtaGetElementType(el).ElSSchema;
  asc = TtaGetParent (el);
  while (asc && !inheritedAttr)
    {
      if (TtaGetElementType(asc).ElSSchema != sch)
        asc = TtaGetParent (asc);
      else
        {
          inheritedAttr = TtaGetAttribute (asc, attrType);
          if (!inheritedAttr)
            asc = TtaGetParent (asc);
        }
    }
  return (inheritedAttr);
}
#endif /* _SVG */

/*----------------------------------------------------------------------
  CreateGraphicElement
  Create a Graphics element.
  entry is the number of the entry chosen by the user in the Graphics
  palette.
  ----------------------------------------------------------------------*/
void CreateGraphicElement (Document doc, View view, int entry)
{
#ifdef _SVG
  Element	    first, SvgRoot, newEl, sibling, selEl;
  Element           child, parent, elem, switch_, foreignObj, altText, leaf;
  ElementType       elType, selType, newType, childType;
  AttributeType     attrType, attrTypeHTML;
  Attribute         attr, inheritedAttr;
  SSchema	    docSchema, SvgSchema;
  DisplayMode       dispMode;
  Language          lang;
  char		    shape;
  char             *path;
  int		    c1, i, w, h, dir, svgDir;
  int               docModified;
  ThotBool	    found, newGraph = FALSE, oldStructureChecking;
  ThotBool          isFilled, isFormattedView;

  int x1, y1, x2, y2, x3, y3, x4, y4, x5, y5, x6, y6, lx, ly;
  unsigned short      red, green, blue;

  _ParserData context;

  int error;

  char buffer[300];
  char stroke_color[10], fill_color[10];

  /* Check that a document is selected */
  if (doc == 0)
    {
      TtaDisplaySimpleMessage (CONFIRM, AMAYA, AM_NO_INSERT_POINT);
      return;
    }

  context.doc = doc;
 
  /* Check that whether we are in formatted or strutured view. */
  if (view == 1) isFormattedView = TRUE;
  else if(view == 2) isFormattedView = FALSE;
  else return;

  TtaGiveFirstSelectedElement (doc, &first, &c1, &i);
  if (first)
    {
      parent = TtaGetParent (first);
      if (TtaIsReadOnly (parent))
        /* do not create new elements within a read-only element */
	{
	  TtaDisplaySimpleMessage (CONFIRM, LIB, TMSG_EL_RO);
	  return;
	}
    }
  TtaOpenUndoSequence (doc, NULL, NULL, 0, 0);
  selEl = first;
  newEl = NULL;
  child = NULL;
  docModified = TtaIsDocumentModified (doc);
  /* Are we in a drawing? */
  docSchema = TtaGetDocumentSSchema (doc);
  SvgSchema = GetSVGSSchema (doc);
  attrType.AttrSSchema = SvgSchema;
  elType = TtaGetElementType (selEl);
  if (elType.ElTypeNum == SVG_EL_SVG &&
      elType.ElSSchema == SvgSchema)
    SvgRoot = selEl;
  else
    {
      elType.ElTypeNum = SVG_EL_SVG;
      elType.ElSSchema = SvgSchema;
      SvgRoot = TtaGetTypedAncestor (first, elType);
      if (SvgRoot == NULL)
        /* the current selection is not in a SVG element, create one */
        {
          selType = TtaGetElementType (first);
          /* Allow an SVG element only within an HTML or a generic XML element */
          if (strcmp (TtaGetSSchemaName (selType.ElSSchema), "HTML"))
            {
              /* It's not an HTML element. Is it a generic XML element ? */
              if (!TtaIsXmlSSchema (selType.ElSSchema))
                {
                  /* It's not a generic XML element */
                  TtaCancelLastRegisteredSequence (doc);
		  TtaDisplaySimpleMessage (CONFIRM, AMAYA, AM_NOT_ALLOWED);
                  return;
                }
            }
          SvgSchema = TtaNewNature (doc, docSchema, NULL, "SVG", "SVGP");
          if (TtaIsSelectionEmpty ())
            {
              /* try to create the SVG here */
              TtaCreateElement (elType, doc);
              TtaGiveFirstSelectedElement (doc, &elem, &c1, &i);
              selType = TtaGetElementType (elem);
              if (selType.ElTypeNum != elType.ElTypeNum)
                SvgRoot = TtaGetTypedAncestor (elem, elType);
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
              SvgRoot = TtaNewElement (doc, elType);
              TtaInsertSibling (SvgRoot, first, FALSE, doc);
              first = SvgRoot;
              newGraph = TRUE;
            }
          if (SvgRoot)
            /* a root SVG element was created. Create the required attributes*/
            {
              attrType.AttrTypeNum = SVG_ATTR_version;
              attr = TtaNewAttribute (attrType);
              TtaAttachAttribute (SvgRoot, attr, doc);
              TtaSetAttributeText (attr, SVG_VERSION, SvgRoot, doc);

	      attrType.AttrTypeNum = SVG_ATTR_width_;
              attr = TtaNewAttribute (attrType);
	      TtaAttachAttribute (SvgRoot, attr, doc);
	      TtaSetAttributeText (attr, "500", SvgRoot, doc);

	      ParseWidthHeightAttribute (attr, SvgRoot, doc, FALSE);

	      attrType.AttrTypeNum = SVG_ATTR_height_;
              attr = TtaNewAttribute (attrType);
	      TtaAttachAttribute (SvgRoot, attr, doc);
	      TtaSetAttributeText (attr, "200", SvgRoot, doc);
	      ParseWidthHeightAttribute (attr, SvgRoot, doc, FALSE);
            }
        }
    }

  /* look for the element (sibling) in front of which the new element will be
     created */
  sibling = NULL;
  if (first == SvgRoot)
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
              if (elType.ElSSchema == SvgSchema &&
                  (elType.ElTypeNum == SVG_EL_g ||
                   elType.ElTypeNum == SVG_EL_SVG))
                found = TRUE;
              else
                sibling = parent;
            }
        }
      while (parent && !found);
    }

  if (!parent)
    {
      parent = SvgRoot;
      sibling = TtaGetLastChild (SvgRoot);
    }


  if(isFormattedView)
    {
      /* Select the SVG element where we draw, so that we can see its border */
      TtaSelectElement(doc, SvgRoot);
    }
  
  newType.ElSSchema = SvgSchema;
  newType.ElTypeNum = 0;
  shape = EOS;
  isFilled = TRUE;

  switch (entry)
    {
    case 0:	/* line */
      newType.ElTypeNum = SVG_EL_line_;
      isFilled = FALSE;
      //shape = 'g';
      break;

    case 1:	/* rectangle */
      newType.ElTypeNum = SVG_EL_rect;
      //shape = 'C';
      break;

    case 2:	/* rectangle with rounded corners */
      newType.ElTypeNum = SVG_EL_rect;
      //shape = 'C';
      break;

    case 3:	/* circle */
      newType.ElTypeNum = SVG_EL_circle_;
      //shape = 'a';
      break;

    case 4:	/* ellipse */
      newType.ElTypeNum = SVG_EL_ellipse;
      //shape = 'c';
      break;

    case 5:	/* polyline */
      newType.ElTypeNum = SVG_EL_polyline;
      isFilled = FALSE;
      //shape = 'S';
      break;

    case 6:	/* polygon */
      newType.ElTypeNum = SVG_EL_polygon;
      shape = 'p';
      break;

    case 7:	/* spline */
      newType.ElTypeNum = SVG_EL_path;
      isFilled = FALSE;
      shape = 'B';
      break;

    case 8:	/* closed spline */
      newType.ElTypeNum = SVG_EL_path;
      shape = 's';
      break;

    case 9:	/* switch and foreignObject with some HTML code */
      if(isFormattedView)
	 newType.ElTypeNum = SVG_EL_g;
      else
	newType.ElTypeNum = SVG_EL_switch;
      break;

    case 10:	/* text */
      newType.ElTypeNum = SVG_EL_text_;
      break;

    case 11:	/* group */
      /* Normally, the program don't reach this point */
      newType.ElTypeNum = 0;
      break;
 
    case 12: /* Simple arrow */
      newType.ElTypeNum = SVG_EL_path;
      isFilled = FALSE;
      break;

    case 13: /* Double arrow */
      newType.ElTypeNum = SVG_EL_path;
      isFilled = FALSE;
      break;

    case 14: /* Zigzag */
      newType.ElTypeNum = SVG_EL_polyline;
      isFilled = FALSE;
      break;

    case 15: /* square */
      newType.ElTypeNum = SVG_EL_rect;
      break;

    case 16: /* rounded square */
      newType.ElTypeNum = SVG_EL_rect;
      break;

    case 17: /* diamond */
      newType.ElTypeNum = SVG_EL_polygon;
      break;

    case 18: /* trapezium */
      newType.ElTypeNum = SVG_EL_polygon;
      break;

    case 19: /* parallelogram */
      newType.ElTypeNum = SVG_EL_polygon;
      break;

    case 20: /* equilateral triangle */
      newType.ElTypeNum = SVG_EL_polygon;
      break;

    case 21: /* isosceles triangle */
      newType.ElTypeNum = SVG_EL_polygon;
      break;

    case 22: /* rectangle triangle */
      newType.ElTypeNum = SVG_EL_polygon;
      break;

    case 23: /* cube */
      newType.ElTypeNum = SVG_EL_path;
      break;

    case 24: /* parallelepiped */
      newType.ElTypeNum = SVG_EL_path;
      break;

    case 25: /* cylinder */
      newType.ElTypeNum = SVG_EL_path;
      break;

    default:
      newType.ElTypeNum = SVG_EL_rect;
      break;
    }
  
  InCreation = TRUE;
  if (newType.ElTypeNum > 0)
    {
      dispMode = TtaGetDisplayMode (doc);
      /* ask Thot to stop displaying changes made in the document */
      if (dispMode == DisplayImmediately)
        TtaSetDisplayMode (doc, DeferredDisplay);

/*       /\* for rectangles, circle, ellipse, and text, ask for an elastic box *\/ */
/*       if (newType.ElTypeNum == SVG_EL_rect ||  */
/* 	    newType.ElTypeNum == SVG_EL_circle_ || */
/* 	    newType.ElTypeNum == SVG_EL_ellipse || */
/*           newType.ElTypeNum == SVG_EL_text_) */
/*         TtaAskFirstCreation (); */

      /* create the new element */
      newEl = TtaNewElement (doc, newType);

      if(isFormattedView)
	{
	  /* Insert the child as the last element (i.e. in the foreground)
	     of the SvgRoot */
	  sibling = TtaGetLastChild(SvgRoot);
	  if (!sibling)
	    TtaInsertFirstChild (&newEl, SvgRoot, doc);
	  else
	    TtaInsertSibling (newEl, sibling, FALSE, doc);
	}
      else
	{
	  if (!sibling)
	    TtaInsertFirstChild (&newEl, parent, doc);
	  else
	    {
	      elType = TtaGetElementType (sibling);
	      if (elType.ElSSchema == SvgSchema &&
		  elType.ElTypeNum == SVG_EL_GraphicsElement)
		/* the new element replaces the existing, empty element */
		TtaInsertFirstChild (&newEl, sibling, doc);
	      else
		TtaInsertSibling (newEl, sibling, FALSE, doc);
	    }
	}

  
      if(!(entry >= 5 && entry <= 11))
	{
	  /* Basic Shapes and lines */

	  selEl = newEl;

	  if(isFormattedView)
	    AskSurroundingBox(&x1, &y1, &x2, &y2, doc, entry, SvgRoot);
	  else
	    {
	      /* TODO: add a dialog box ? */
	      x1 = 0; y1 = 0;
	      x2 = 50; y2 = 50;
	    }
	  
	  lx = x2 - x1;
	  ly = y2 - y1;

	  switch(entry)
	    {
	    case 0: /* Line */
	      SVGElementComplete (&context, newEl, &error);

	      attrType.AttrTypeNum = SVG_ATTR_x1;
	      UpdateAttrText (newEl, doc, attrType, x1, FALSE, TRUE);

	      attrType.AttrTypeNum = SVG_ATTR_y1;
	      UpdateAttrText (newEl, doc, attrType, y1, FALSE, TRUE);

	      attrType.AttrTypeNum = SVG_ATTR_x2;
	      UpdateAttrText (newEl, doc, attrType, x2, FALSE, TRUE);

	      attrType.AttrTypeNum = SVG_ATTR_y2;
	      UpdateAttrText (newEl, doc, attrType, y2, FALSE, TRUE);

	      break;

	    case 12: /* Simple Arrow */
              attrType.AttrTypeNum = SVG_ATTR_d;
              attr = TtaNewAttribute (attrType);
              TtaAttachAttribute (newEl, attr, doc);

	      x3 = x1; y3 = y1;
	      x4 = x2; y4 = y2;
	      GetArrowCoord(&x3, &y3, &x4, &y4);

	      sprintf(buffer, "M %d %d L %d %d M %d %d L %d %d %d %d",
		      x1, y1, x2, y2,
		      x3, y3, x2, y2, x4, y4
		      );
              TtaSetAttributeText (attr, buffer, newEl, doc);
	      ParsePathDataAttribute (attr, newEl, doc, TRUE);
	      break;

	    case 13: /* Double Arrow */
              attrType.AttrTypeNum = SVG_ATTR_d;
              attr = TtaNewAttribute (attrType);
              TtaAttachAttribute (newEl, attr, doc);

	      x3 = x1; y3 = y1;
	      x4 = x2; y4 = y2;
	      GetArrowCoord(&x3, &y3, &x4, &y4);

	      x5 = x2; y5 = y2;
	      x6 = x1; y6 = y1;
	      GetArrowCoord(&x5, &y5, &x6, &y6);

	      sprintf(buffer, "M %d %d L %d %d M %d %d L %d %d %d %d M %d %d L %d %d %d %d",
		      x1, y1, x2, y2,
		      x3, y3, x2, y2, x4, y4,
		      x5, y5, x1, y1, x6, y6
		      );

              TtaSetAttributeText (attr, buffer, newEl, doc);
	      ParsePathDataAttribute (attr, newEl, doc, TRUE);
	      break;

	    case 14: /* Zigzag */
              attrType.AttrTypeNum = SVG_ATTR_points;
              attr = TtaNewAttribute (attrType);
              TtaAttachAttribute (newEl, attr, doc);
	      sprintf(buffer, "%d %d %d %d %d %d %d %d",
		      x1,y1,
		      x1,y1+(y2-y1)/2,
		      x2,y1+(y2-y1)/2,
		      x2,y2
		      );
              TtaSetAttributeText (attr, buffer, newEl, doc);
	      ParsePointsAttribute (attr, newEl, doc);
	      break;

	      /* Square */
	    case 15:
	      if(ly < lx)lx = ly; else ly = lx;

	      /* Rectangle */
	    case 1:
	      SVGElementComplete (&context, newEl, &error);

	      attrType.AttrTypeNum = SVG_ATTR_x;
	      UpdateAttrText (newEl, doc, attrType, x1, FALSE, TRUE);

	      attrType.AttrTypeNum = SVG_ATTR_y;
	      UpdateAttrText (newEl, doc, attrType, y1, FALSE, TRUE);

	      attrType.AttrTypeNum = SVG_ATTR_width_;
	      UpdateAttrText (newEl, doc, attrType, lx, FALSE, TRUE);

	      attrType.AttrTypeNum = SVG_ATTR_height_;
	      UpdateAttrText (newEl, doc, attrType, ly, FALSE, TRUE);

	      break;

	      /* Rounded Square */
	    case 16:
	      if(ly < lx)lx = ly; else ly = lx;

	      /* Rounded-Rectangle */
	    case 2:
	      SVGElementComplete (&context, newEl, &error);

	      attrType.AttrTypeNum = SVG_ATTR_x;
	      UpdateAttrText (newEl, doc, attrType, x1, FALSE, TRUE);

	      attrType.AttrTypeNum = SVG_ATTR_y;
	      UpdateAttrText (newEl, doc, attrType, y1, FALSE, TRUE);

	      attrType.AttrTypeNum = SVG_ATTR_width_;
	      UpdateAttrText (newEl, doc, attrType, lx, FALSE, TRUE);

	      attrType.AttrTypeNum = SVG_ATTR_height_;
	      UpdateAttrText (newEl, doc, attrType, ly, FALSE, TRUE);

              attrType.AttrTypeNum = SVG_ATTR_rx;
              attr = TtaNewAttribute (attrType);
              TtaAttachAttribute (newEl, attr, doc);
              TtaSetAttributeText (attr, "5px", newEl, doc);
              ParseWidthHeightAttribute (attr, newEl, doc, FALSE);
	      break;

	      /* Circle */
	    case 3:
	      if(ly < lx)lx = ly; else ly = lx;
	      SVGElementComplete (&context, newEl, &error);

	      attrType.AttrTypeNum = SVG_ATTR_cx;
	      UpdateAttrText (newEl, doc, attrType, x1+lx/2, FALSE, TRUE);

	      attrType.AttrTypeNum = SVG_ATTR_cy;
	      UpdateAttrText (newEl, doc, attrType, y1+ly/2, FALSE, TRUE);

	      attrType.AttrTypeNum = SVG_ATTR_r;
	      UpdateAttrText (newEl, doc, attrType, lx/2, FALSE, TRUE);

	      break;

	      /* Ellipse */
	    case 4:
	      SVGElementComplete (&context, newEl, &error);

	      attrType.AttrTypeNum = SVG_ATTR_cx;
	      UpdateAttrText (newEl, doc, attrType, x1+lx/2, FALSE, TRUE);

	      attrType.AttrTypeNum = SVG_ATTR_cy;
	      UpdateAttrText (newEl, doc, attrType, y1+ly/2, FALSE, TRUE);

	      attrType.AttrTypeNum = SVG_ATTR_rx;
	      UpdateAttrText (newEl, doc, attrType, lx/2, FALSE, TRUE);

	      attrType.AttrTypeNum = SVG_ATTR_ry;
	      UpdateAttrText (newEl, doc, attrType, ly/2, FALSE, TRUE);;
	      break;

	    case 17: /* diamond */
              attrType.AttrTypeNum = SVG_ATTR_points;
              attr = TtaNewAttribute (attrType);
              TtaAttachAttribute (newEl, attr, doc);
	      sprintf(buffer, "%d %d %d %d %d %d %d %d",
		      x1         , y1 + ly/2,
		      x1 + lx/2, y1         ,
		      x2         , y1 + ly/2,
		      x1 + lx/2, y2          
		      );
              TtaSetAttributeText (attr, buffer, newEl, doc);
	      ParsePointsAttribute (attr, newEl, doc);
	      break;

	    case 18: /* trapezium */
              attrType.AttrTypeNum = SVG_ATTR_points;
              attr = TtaNewAttribute (attrType);
              TtaAttachAttribute (newEl, attr, doc);
	      sprintf(buffer, "%d %d %d %d %d %d %d %d",
		      x1, y2,
		      x1 + lx / 4, y1,
		      x2 - lx / 4, y1,
		      x2, y2
		      );
              TtaSetAttributeText (attr, buffer, newEl, doc);
	      ParsePointsAttribute (attr, newEl, doc);
	      break;

	    case 19: /* parallelogram */
              attrType.AttrTypeNum = SVG_ATTR_points;
              attr = TtaNewAttribute (attrType);
              TtaAttachAttribute (newEl, attr, doc);
	      sprintf(buffer, "%d %d %d %d %d %d %d %d",
		      x1 + lx/4, y1,
		      x2, y1,
		      x2 - lx / 4, y2,
		      x1, y2
		      );
              TtaSetAttributeText (attr, buffer, newEl, doc);
	      ParsePointsAttribute (attr, newEl, doc);
	      break;

	    case 20: /* equilateral triangle */
              attrType.AttrTypeNum = SVG_ATTR_points;
              attr = TtaNewAttribute (attrType);
              TtaAttachAttribute (newEl, attr, doc);
	      lx = (int) (floor(2 *  ly / sqrt(3)));
	      sprintf(buffer, "%d %d %d %d %d %d",
		      x1 + lx/2, y1,
		      x1 + lx, y2,
		      x1, y2
		      );
              TtaSetAttributeText (attr, buffer, newEl, doc);
	      ParsePointsAttribute (attr, newEl, doc);
	      break;

	    case 21: /* isosceles triangle */
              attrType.AttrTypeNum = SVG_ATTR_points;
              attr = TtaNewAttribute (attrType);
              TtaAttachAttribute (newEl, attr, doc);
	      sprintf(buffer, "%d %d %d %d %d %d",
		      x1 + lx/2, y1,
		      x2, y2,
		      x1, y2
		      );
              TtaSetAttributeText (attr, buffer, newEl, doc);
	      ParsePointsAttribute (attr, newEl, doc);
	      break;

	    case 22: /* rectangle triangle */
              attrType.AttrTypeNum = SVG_ATTR_points;
              attr = TtaNewAttribute (attrType);
              TtaAttachAttribute (newEl, attr, doc);
	      sprintf(buffer, "%d %d %d %d %d %d",
		      x1, y1,
		      x2, y1,
		      x1, y2
		      );
              TtaSetAttributeText (attr, buffer, newEl, doc);
	      ParsePointsAttribute (attr, newEl, doc);
	      break;

	    case 23: /* cube */
	      if(ly < lx)
		{
		  lx = ly;
		  x2 = x1 + lx;
		}
	      else
		{		
		  ly = lx;	      
		  y2 = y1 + lx;
		}

	    case 24: /* parallelepiped */
              attrType.AttrTypeNum = SVG_ATTR_d;
              attr = TtaNewAttribute (attrType);
              TtaAttachAttribute (newEl, attr, doc);
	      sprintf(buffer, "M %d %d L %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d M %d %d L %d %d",
		      x1, y1+ly/4,
		      x1+lx/4, y1,
		      x2, y1,
		      x2, y2-ly/4,
		      x2-lx/4, y2,
		      x1,y2,
		      x1, y1+ly/4,
		      x2-lx/4, y1+ly/4,
		      x2-lx/4, y2,
		      x2-lx/4, y1+ly/4,
		      x2, y1
		      );
              TtaSetAttributeText (attr, buffer, newEl, doc);
	      ParsePathDataAttribute (attr, newEl, doc, TRUE);
	      break;

	    case 25: /* Cylinder */
              attrType.AttrTypeNum = SVG_ATTR_d;
              attr = TtaNewAttribute (attrType);
              TtaAttachAttribute (newEl, attr, doc);
	      sprintf(buffer, "M %d %d L %d %d A %d %d 0 0 0 %d %d L %d %d A %d %d 0 0 0 %d %d A %d %d 0 0 0 %d %d",
		      x1, y1+ly/6,
		      x1, y2-ly/6,
		      lx/2, ly/6,
		      x2, y2-ly/6,
	
		      x2, y1+ly/6,
		      lx/2, ly/6,
		      x1, y1+ly/6,
		      lx/2, ly/6,
		      x2, y1+ly/6
		      );
              TtaSetAttributeText (attr, buffer, newEl, doc);
	      ParsePathDataAttribute (attr, newEl, doc, TRUE);
	      
	      break;

	    default:
	      break;
	    }
	}
      else if(entry == 5 /* entry >= 5 && entry <= 8*/)
	{
	  selEl = newEl;

	  /* Polyline and curves */
	  if(isFormattedView)
	    AskShapePoints (doc, entry, SvgRoot);
	  
	  /* TODO... */
	
	  /*	attrType.AttrTypeNum = SVG_ATTR_points;
		attr = TtaNewAttribute (attrType);
		TtaAttachAttribute (newEl, attr, doc);


		TtaSetAttributeText (attr, buffer, newEl, doc);
		ParsePointsAttribute (attr, newEl, doc);*/
	}
      else if (entry == 9)
        /* create a foreignObject containing an XHTML div element within the
           new element */
        {
	  if(isFormattedView)
	    {
	      /* Ask the position and size */
	    AskSurroundingBox(&x1, &y1, &x2, &y2, doc, entry, SvgRoot);
	  
	    /* create a transform=translate attribute */
	    attrType.AttrTypeNum = SVG_ATTR_transform;
	    attr = TtaNewAttribute (attrType);
	    TtaAttachAttribute (newEl, attr, doc);
	    sprintf(buffer, "translate(%d,%d)", x1, y1);
	    TtaSetAttributeText (attr, buffer, newEl, doc);
	    ParseTransformAttribute (attr, newEl, doc, FALSE);

	    /* Create a switch element */
	    childType.ElSSchema = SvgSchema;
	    childType.ElTypeNum = SVG_EL_switch;
	    switch_ = TtaNewElement (doc, childType);
	    TtaInsertFirstChild (&switch_, newEl, doc);
	    }
	  else
	    switch_ = newEl;

	  /* Create a foreign Object */
          childType.ElSSchema = SvgSchema;
          childType.ElTypeNum = SVG_EL_foreignObject;
          foreignObj = TtaNewElement (doc, childType);
          TtaInsertFirstChild (&foreignObj, switch_, doc);

          /* associate a requiredExtensions attribute with the foreignObject
             element */
          attrType.AttrTypeNum = SVG_ATTR_requiredExtensions;
          attr = TtaNewAttribute (attrType);
          TtaAttachAttribute (foreignObj, attr, doc);
          TtaSetAttributeText (attr, XHTML_URI, foreignObj, doc);

	  /* Size of the foreignObject */
          attrType.AttrTypeNum = SVG_ATTR_width_;
          UpdateAttrText (foreignObj, doc, attrType, x2 - x1, FALSE, TRUE);
          attrType.AttrTypeNum = SVG_ATTR_height_;
          UpdateAttrText (foreignObj, doc, attrType, y2 - y1, FALSE, TRUE);

          /* the document is supposed to be HTML */
          childType.ElSSchema = TtaNewNature (doc, docSchema, NULL, "HTML",
                                              "HTMLP");
          childType.ElTypeNum = HTML_EL_Division;
          child = TtaNewTree (doc, childType, "");

          /* do not check the Thot abstract tree against the structure */
          /* schema when inserting this element */
          oldStructureChecking = TtaGetStructureChecking (doc);
          TtaSetStructureChecking (FALSE, doc);

          /* insert the new <div> element */
          TtaInsertFirstChild (&child, foreignObj, doc);

          /* put an XHTML namespace declaration on the <div> element */
          TtaSetUriSSchema (childType.ElSSchema, XHTML_URI);
          TtaSetANamespaceDeclaration (doc, child, NULL, XHTML_URI);
          TtaSetStructureChecking (oldStructureChecking, doc);

          /* create an alternate SVG text element for viewers that are not
             able to display embedded MathML */
          elType.ElSSchema = SvgSchema;
          elType.ElTypeNum = SVG_EL_text_;
          altText = TtaNewElement (doc, elType);
          TtaInsertSibling (altText, foreignObj, FALSE, doc);
          elType.ElTypeNum = SVG_EL_TEXT_UNIT;
          leaf = TtaNewElement (doc, elType);
          TtaInsertFirstChild (&leaf, altText, doc);
          lang = TtaGetLanguageIdFromScript('L');
          TtaSetTextContent (leaf,
			     (unsigned char *)"foreignObject not supported" ,
			     lang, doc);

          /* is there a SVG direction attribute on any ancestor element? */
          attrType.AttrTypeNum = SVG_ATTR_direction_;
          inheritedAttr = InheritAttribute (foreignObj, attrType);
          dir = -1;
          if (!inheritedAttr)
            /* no direction attribute. Create a HTML dir attribute with
               value ltr */
            dir = HTML_ATTR_dir_VAL_ltr_;
          else
            {
              svgDir = TtaGetAttributeValue (inheritedAttr);
              switch (svgDir)
                {
                case SVG_ATTR_direction__VAL_ltr_ :
                  dir = HTML_ATTR_dir_VAL_ltr_;
                  break;
                case SVG_ATTR_direction__VAL_rtl_ :
                  dir = HTML_ATTR_dir_VAL_rtl_;
                  break;
                case SVG_ATTR_direction__VAL_inherit :
                  dir = -1;
                  break;
                }
            }
          if (dir >= 0)
            {
              /* create a dir attribute for the div element */
              attrTypeHTML.AttrSSchema = childType.ElSSchema;
              attrTypeHTML.AttrTypeNum = HTML_ATTR_dir;
              attr = TtaNewAttribute (attrTypeHTML);
              TtaAttachAttribute (child, attr, doc);	   
              TtaSetAttributeValue (attr, dir, child, doc);
            }
          /* select the first leaf */
          elem = child;

          do
            {
              selEl = elem;
              elem = TtaGetFirstChild (elem);
            }
          while (elem != NULL);

	  /* set the visibility of the alternate text */
	  EvaluateTestAttrs (switch_, doc);
        }
      else if (entry == 10)
        /* creation of a TEXT leaf */
        {
          childType.ElSSchema = SvgSchema;
          childType.ElTypeNum = SVG_EL_TEXT_UNIT;
          child = TtaNewElement (doc, childType);
          TtaInsertFirstChild (&child, newEl, doc);
          selEl = child;

	  if(isFormattedView)
	    {
	      /* Ask where the user wants to insert the text */
	      AskSurroundingBox(&x1, &y1, &x2, &y2, doc, entry, SvgRoot);

	      attrType.AttrTypeNum = SVG_ATTR_x;
	      UpdateAttrText (newEl, doc, attrType, x1, FALSE, TRUE);
	      
	      attrType.AttrTypeNum = SVG_ATTR_y;
	      UpdateAttrText (newEl, doc, attrType, y1, FALSE, TRUE);
	    }
        }



      if (newGraph)
	TtaRegisterElementCreate (SvgRoot, doc);
      else
	TtaRegisterElementCreate (newEl, doc);
     
      /* ask Thot to display changes made in the document */
      TtaSetDisplayMode (doc, dispMode);
    }


  /* create attributes fill and stroke */
  if (
      newType.ElTypeNum == SVG_EL_line_ ||
      newType.ElTypeNum == SVG_EL_rect ||
      newType.ElTypeNum == SVG_EL_circle_ ||
      newType.ElTypeNum == SVG_EL_ellipse ||
      newType.ElTypeNum == SVG_EL_polyline ||
      newType.ElTypeNum == SVG_EL_polygon ||
      newType.ElTypeNum == SVG_EL_path)
    {
  
      /* Get the stroke color */
      if (Current_Color != -1)
	TtaGiveThotRGB (Current_Color, &red, &green, &blue);
      else
	TtaGiveThotRGB (0, &red, &green, &blue);
      sprintf(stroke_color, "#%02x%02x%02x", red, green, blue);

      /* Get the fill color */
      if (Current_BackgroundColor != -1 && isFilled)
	{
	  TtaGiveThotRGB (Current_BackgroundColor, &red, &green, &blue);
	  sprintf(fill_color , "#%02x%02x%02x", red, green, blue);
	}
      else
	sprintf(fill_color , "none");

      /* Apply the style */
      if(newType.ElTypeNum == SVG_EL_line_)
	sprintf(buffer, "stroke:%s", stroke_color);
      else
	sprintf(buffer, "stroke:%s; fill:%s", stroke_color, fill_color);

      ParseHTMLSpecificStyle (newEl, buffer, doc, 0, FALSE);

      attrType.AttrTypeNum = SVG_ATTR_style_;
      attr = TtaNewAttribute (attrType);
      TtaAttachAttribute (newEl, attr, doc);
      TtaSetAttributeText (attr, buffer, newEl, doc);
  
    } 


  /******************** TODO: clean up this former code *********************/

  /* create a child for the new element */
  if (shape != EOS)
    /* create a graphic leaf according to the element's type */
    {
      childType.ElSSchema = SvgSchema;
      childType.ElTypeNum = SVG_EL_GRAPHICS_UNIT;
      child = TtaNewElement (doc, childType);
      TtaInsertFirstChild (&child, newEl, doc);
      TtaSetGraphicsShape (child, shape, doc);
      if (entry == 2)
	/* rectangle with rounded corners */
	{
	  /* create a default rx attribute */
	  attrType.AttrTypeNum = SVG_ATTR_rx;
	  attr = TtaNewAttribute (attrType);
	  TtaAttachAttribute (newEl, attr, doc);
	  TtaSetAttributeText (attr, "5px", newEl, doc);
	  ParseWidthHeightAttribute (attr, newEl, doc, FALSE);
	}
    }
  
  if (shape == 'S' || shape == 'p' || shape == 'B' ||
      shape == 's' || shape == 'g')
    /* multipoints element. Let the user enter the points */
    {
      if (shape != 'g')
        {
          TtaGiveBoxSize (parent, doc, 1, UnPixel, &w, &h);
          TtaChangeLimitOfPolyline (child, UnPixel, w, h, doc);
        }
      /* select the leaf element and ask the user to enter the points */
      TtaSelectElement (doc, child);
      TtcInsertGraph (doc, 1, shape);
      /* the user has created the points */
      if (shape != 'g')
        {
          if (TtaGetElementVolume (child) < 3)
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
          else if (shape == 'B' || /* open spline */
                   shape == 's')   /* closed spline */
            /* transform a Thot curve into a SVG path */
            {
              TtaCancelLastRegisteredOperation (doc);
              path = TtaTransformCurveIntoPath (child);
              TtaRemoveTree (child, doc);
              TtaInsertFirstChild (&child, newEl, doc);
              attrType.AttrTypeNum = SVG_ATTR_d;
              attr = TtaNewAttribute (attrType);
              TtaAttachAttribute (newEl, attr, doc);
              TtaSetAttributeText (attr, path, newEl, doc);
              TtaFreeMemory (path);
              ParsePathDataAttribute (attr, newEl, doc, TRUE);
              if (newGraph)
                TtaRegisterElementCreate (SvgRoot, doc);
              else
                TtaRegisterElementCreate (newEl, doc);
            }
        }
    }

  /***********************************************************************/

  if (selEl != NULL)
    /* select the right element */
    TtaSelectElement (doc, selEl);
  
  /* adapt the size of the SVG root element if necessary */
  InCreation = FALSE;

  /*CheckSVGRoot (doc, newEl);
    SetGraphicDepths (doc, SvgRoot);*/

  TtaCloseUndoSequence (doc);
  TtaSetDocumentModified (doc);
  if (newType.ElTypeNum == 0)
    InitInfo ("      ", TtaGetMessage (AMAYA, AM_NOT_AVAILABLE));
#endif /* _SVG */
}


#ifdef _SVG
/*----------------------------------------------------------------------
  CreateGroup
  Create a g element surrounding the selected elements
  ----------------------------------------------------------------------*/
static void CreateGroup ()
{
  Document	doc;
  Element	el, prevSel, prevChild, group, parent;
  ElementType	elType;
  AttributeType	attrType;
  int		c1, i, minX, minY;
  DisplayMode	dispMode;
  ThotBool	position;
  SSchema       docSchema, SvgSchema;

  doc = TtaGetSelectedDocument ();
  if (doc == 0)
    /* there is no selection. Nothing to do */
    return;
  TtaGiveFirstSelectedElement (doc, &el, &c1, &i);
  if (el == NULL)
    /* no selection. Return */
    return;

  if (el)
    {
      parent = TtaGetParent (el);
      if (TtaIsReadOnly (parent))
        /* do not create new elements within a read-only element */
        return;
    }

  dispMode = TtaGetDisplayMode (doc);
  /* ask Thot to stop displaying changes made in the document */
  if (dispMode == DisplayImmediately)
    TtaSetDisplayMode (doc, DeferredDisplay);

  TtaOpenUndoSequence (doc, NULL, NULL, 0, 0);
  prevSel = NULL;
  prevChild = NULL;

  /* Check whether the selection is in a SVG element */
  docSchema = TtaGetDocumentSSchema (doc);
  SvgSchema = GetSVGSSchema (doc);
  attrType.AttrSSchema = SvgSchema;
  elType = TtaGetElementType (el);
  if (!(elType.ElTypeNum == SVG_EL_SVG &&
	elType.ElSSchema == SvgSchema))
    {

      elType.ElTypeNum = SVG_EL_SVG;
      elType.ElSSchema = SvgSchema;
      parent = TtaGetTypedAncestor (el, elType);
      if(parent)
	{
	  /* Create a Group element */
	  elType = TtaGetElementType (el);
	  elType.ElTypeNum = SVG_EL_g;
	  group = TtaNewElement (doc, elType);

	  /* insert the new group element */
	  TtaInsertSibling (group, el, TRUE, doc);
	  TtaRegisterElementCreate (group, doc);

	  attrType.AttrSSchema = elType.ElSSchema;
	  minX = minY = 32000;
	  position = FALSE;
	  while (el != NULL)
	    {
	      if (prevSel != NULL)
		{
		  TtaRegisterElementDelete (prevSel, doc);
		  TtaRemoveTree (prevSel, doc);
		  if (prevChild == NULL)
		    TtaInsertFirstChild (&prevSel, group, doc);
		  else
		    TtaInsertSibling (prevSel, prevChild, FALSE, doc);
		  TtaRegisterElementCreate (prevSel, doc);
		  prevChild = prevSel;
		}
	      prevSel = el;
	      TtaGiveNextSelectedElement (doc, &el, &c1, &i);
	    }
	  if (prevSel != NULL)
	    {
	      TtaRegisterElementDelete (prevSel, doc);
	      TtaRemoveTree (prevSel, doc);
	      if (prevChild == NULL)
		TtaInsertFirstChild (&prevSel, group, doc);
	      else
		TtaInsertSibling (prevSel, prevChild, FALSE, doc);      
	      TtaRegisterElementCreate (prevSel, doc);
	    }
	  TtaSelectElement (doc, group);

	}
    }

  TtaCloseUndoSequence (doc);
  /* ask Thot to display changes made in the document */
  TtaSetDisplayMode (doc, dispMode);
    
}


/*----------------------------------------------------------------------
  CallbackGraph: manage Graph dialogue events.
  ----------------------------------------------------------------------*/
static void CallbackGraph (int ref, int typedata, char *data)
{
  Document           doc;
  long int           val = (long int) data;
  int                profile;

  ref -= GraphDialogue;
  if (ref == MenuGraph1)
    {
      ref = MenuGraph;
      val += 6;
    }
  switch (ref)
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
          // is it a compound document
          profile = TtaGetDocumentProfile (doc);
          if (profile == L_Strict || profile == L_Basic)
            {
              /* no selection. Nothing to do */
              TtaDisplaySimpleMessage (CONFIRM, AMAYA, AM_NOT_ALLOWED);
              return;
            }
          else if (DocumentTypes[doc] != docSVG && DocumentMeta[doc])
            DocumentMeta[doc]->compound = TRUE;
/*           /\* there is a selection *\/ */
/*           if (val == 11) */
/*             CreateGroup (); */
/*           else */
/*             CreateGraphicElement (val); */
        }
      break;
 
    default:
      break;
    }
}
#endif /* _SVG */

/*----------------------------------------------------------------------
  FreeSVG free SVG context.
  ----------------------------------------------------------------------*/
void FreeSVG ()
{
#ifdef _SVG
#if defined(_WX)
  if (iconGraph)
    delete iconGraph;
  if (iconGraphNo)
    delete iconGraphNo;
  for (int i = 0; i < 12; i++)
    {
      if(mIcons[i])
        delete mIcons[i];
    }

  iconGraph =   (ThotIcon) NULL;
  iconGraphNo = (ThotIcon) NULL;
  memset( mIcons, 0, 12 * sizeof(ThotIcon) );
#endif /* defined(_WX) */
#endif /* _SVG */
}

/*----------------------------------------------------------------------
  InitSVG initializes SVG context.
  ----------------------------------------------------------------------*/
void InitSVG ()
{
#ifdef _SVG
#if defined(_GTK)
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
#endif /* #if defined(_GTK) */
   
  GraphDialogue = TtaSetCallback ((Proc)CallbackGraph, MAX_GRAPH);
#endif /* _SVG */
}


/*----------------------------------------------------------------------
  SVGElementTypeInMenu
  -----------------------------------------------------------------------*/
ThotBool SVGElementTypeInMenu (NotifyElement *event)
{
  return TRUE; /* prevent Thot from putting this element name in the
                  element creation menu */
}

/*----------------------------------------------------------------------
  SVGCreated
  An svg element has been created.
  It has at least two attributes (width and height) that are made
  mandatory by the S schema. Parse the value of these attributes.
  ----------------------------------------------------------------------*/
void SVGCreated (NotifyElement * event)
{
  ElementType	elType, parentType;
  Element       parent;
  AttributeType	attrType;
  Attribute	attr;

  elType = TtaGetElementType (event->element);
  /* Set the namespace declaration if the parent element is in a different
     namespace */
  parent = TtaGetParent (event->element);
  if (parent)
    {
      parentType = TtaGetElementType (parent);
      if (parentType.ElSSchema != elType.ElSSchema)
        /* the parent element is not in the SVG namespace. Put a namespace
           declaration on the  <svg> element */
        {
          TtaSetUriSSchema (elType.ElSSchema, SVG_URI);
          TtaSetANamespaceDeclaration (event->document, event->element, NULL,
                                       SVG_URI);
        }
    }
  attrType.AttrSSchema = elType.ElSSchema;
  attrType.AttrTypeNum = SVG_ATTR_width_;
  attr = TtaGetAttribute (event->element, attrType);
  if (attr)
    ParseWidthHeightAttribute (attr, event->element, event->document, FALSE);
  attrType.AttrTypeNum = SVG_ATTR_height_;
  attr = TtaGetAttribute (event->element, attrType);
  if (attr)
    ParseWidthHeightAttribute (attr, event->element, event->document, FALSE);
}

/*----------------------------------------------------------------------
  SVGWillBeDeleted
  An svg element will be deleted.
  Update the namespace declarations linked to that element
  ----------------------------------------------------------------------*/
ThotBool SVGWillBeDeleted (NotifyElement * event)
{
  TtaFreeElemNamespaceDeclarations (event->document, event->element);
  return FALSE; /* let Thot perform normal operation */
}
 
/*----------------------------------------------------------------------
  TspanCreated
  A tspan element has been created by the user hitting a Enter key
  within a text element. Create attributes x and dy.
  ----------------------------------------------------------------------*/
void TspanCreated (NotifyElement * event)
{
  ElementType	elType;
  Element       ancestor;
  AttributeType	attrType;
  Attribute	attr;
  int           x, y;
  char          buffer[50];

  elType = TtaGetElementType (event->element);
  attrType.AttrSSchema = elType.ElSSchema;

  attrType.AttrTypeNum = SVG_ATTR_x;
  attr = TtaGetAttribute (event->element, attrType);
  if (!attr)
    {
      attr = TtaNewAttribute (attrType);
      TtaAttachAttribute (event->element, attr, event->document);
    }
  x = 0;
  /* look for the enclosing text element */
  ancestor = TtaGetParent (event->element);
  while (ancestor)
    {
      elType = TtaGetElementType (ancestor);
      if (elType.ElTypeNum == SVG_EL_text_ &&
          !strcmp (TtaGetSSchemaName (elType.ElSSchema), "SVG"))
        /* it's a text element. Get its position */
        {
          TtaGiveBoxPosition (ancestor, event->document, 1, UnPixel, &x, &y);
          ancestor = NULL;
        }
      else
        /* not a text. look for the next ancestor */
        ancestor = TtaGetParent (ancestor);
    }
  sprintf (buffer, "%dpx", x);
  TtaSetAttributeText (attr, buffer, event->element, event->document);
  ParseCoordAttribute (attr, event->element, event->document);

  attrType.AttrTypeNum = SVG_ATTR_dy;
  attr = TtaGetAttribute (event->element, attrType);
  if (!attr)
    {
      attr = TtaNewAttribute (attrType);
      TtaAttachAttribute (event->element, attr, event->document);
    }
  TtaSetAttributeText (attr, "1em", event->element, event->document);
  ParseCoordAttribute (attr, event->element, event->document);
}

/*----------------------------------------------------------------------
  UsePasted
  A use element has been pasted by the user.
  Copy the referred element.
  ----------------------------------------------------------------------*/
void UsePasted (NotifyElement * event)
{
  ElementType	elType;
  AttributeType	attrType;
  Attribute	attr;
  int           length;
  char          *href;
  ThotBool      ok;

  /* first, get the xlink:href attribute */
  elType = TtaGetElementType (event->element);
  attrType.AttrSSchema = elType.ElSSchema;
  attrType.AttrTypeNum = SVG_ATTR_xlink_href;
  attr = TtaGetAttribute (event->element, attrType);
  if (attr)
    /* the use element has a xlink:href attribute */
    {
      /* get its value */
      length = TtaGetTextAttributeLength (attr);
      href = (char *)TtaGetMemory (length + 1);
      TtaGiveTextAttributeValue (attr, href, &length);
      ok = CopyUseContent (event->element, event->document, href);
      TtaFreeMemory (href);
    }
  /* Check attribute NAME or ID in order to make sure that its value */
  /* is unique in the document */
  MakeUniqueName (event->element, event->document, TRUE, FALSE);
}

/*----------------------------------------------------------------------
  AttrXlinkHrefChanged
  -----------------------------------------------------------------------*/
void AttrXlinkHrefChanged (NotifyAttribute *event)
{
  ElementType   elType;
  char         *text;
  int           length;
  ThotBool      ok;

  length = TtaGetTextAttributeLength (event->attribute);
  if (length <= 0)
    /* attribute empty. Invalid. restore previous value */
    return;
  text = (char *)TtaGetMemory (length + 1);
  TtaGiveTextAttributeValue (event->attribute, text, &length);
  elType = TtaGetElementType (event->element);
  if (elType.ElTypeNum == SVG_EL_image)
    ComputeSRCattribute (event->element, event->document, 0, event->attribute,
                         text);
  else if (elType.ElTypeNum == SVG_EL_use_ ||
           elType.ElTypeNum == SVG_EL_tref)
    ok = CopyUseContent (event->element, event->document, text);
  TtaFreeMemory (text);
}

/*----------------------------------------------------------------------
  DeleteAttrXlinkHref
  -----------------------------------------------------------------------*/
ThotBool DeleteAttrXlinkHref (NotifyAttribute *event)
{
  /* prevents Thot from deleting the xlink:href attribute */
  return TRUE;
}
 

/*----------------------------------------------------------------------
  AttrAnimTimeChanged
  Callback to update timeline
  -----------------------------------------------------------------------*/
void AttrAnimTimeChanged (NotifyAttribute *event)
{
#ifdef _SVG
	Update_period_position_and_size (event->document, event->element);
#endif /* _SVG */
}


/*----------------------------------------------------------------------
  Timeline_cross_prule_modified
  Callback used to define animation motion
  -----------------------------------------------------------------------*/
void Timeline_cross_prule_modified (NotifyPresentation *event)
{
#ifdef _SVG
	Key_position_defined (event->document, event->element);
#endif /* _SVG */
}


/*----------------------------------------------------------------------
  CreateSVG_Line
  ----------------------------------------------------------------------*/
void CreateSVG_Line (Document document, View view)
{
  CreateGraphicElement (document, view, 0);
}

/*----------------------------------------------------------------------
  CreateSVG_Rectangle
  ----------------------------------------------------------------------*/
void CreateSVG_Rectangle (Document document, View view)
{
  CreateGraphicElement (document, view, 1);
}

/*----------------------------------------------------------------------
  CreateSVG_RoundedRectangle
  ----------------------------------------------------------------------*/
void CreateSVG_RoundedRectangle (Document document, View view)
{
  CreateGraphicElement (document, view, 2);
}

/*----------------------------------------------------------------------
  CreateSVG_Circle
  ----------------------------------------------------------------------*/
void CreateSVG_Circle (Document document, View view)
{
  CreateGraphicElement (document, view, 3);
}

/*----------------------------------------------------------------------
  CreateSVG_Ellipse
  ----------------------------------------------------------------------*/
void CreateSVG_Ellipse (Document document, View view)
{
  CreateGraphicElement (document, view, 4);
}

/*----------------------------------------------------------------------
  CreateSVG_Polyline
  ----------------------------------------------------------------------*/
void CreateSVG_Polyline (Document document, View view)
{
  CreateGraphicElement (document, view, 5);
}

/*----------------------------------------------------------------------
  CreateSVG_Polygon
  ----------------------------------------------------------------------*/
void CreateSVG_Polygon (Document document, View view)
{
  CreateGraphicElement (document, view, 6);
}

/*----------------------------------------------------------------------
  CreateSVG_Spline
  ----------------------------------------------------------------------*/
void CreateSVG_Spline (Document document, View view)
{
  CreateGraphicElement (document, view, 7);
}

/*----------------------------------------------------------------------
  CreateSVG_Closed
  ----------------------------------------------------------------------*/
void CreateSVG_Closed (Document document, View view)
{
  CreateGraphicElement (document, view, 8);
}

/*----------------------------------------------------------------------
  CreateSVG_ForeignObject
  ----------------------------------------------------------------------*/
void CreateSVG_ForeignObject (Document document, View view)
{
  CreateGraphicElement (document, view, 9);
}

/*----------------------------------------------------------------------
  CreateSVG_Text
  ----------------------------------------------------------------------*/
void CreateSVG_Text (Document document, View view)
{
  CreateGraphicElement (document, view, 10);
}
 
/*----------------------------------------------------------------------
  CreateSVG_Group
  ----------------------------------------------------------------------*/
void CreateSVG_Group (Document document, View view)
{
  CreateGroup();
}

/*----------------------------------------------------------------------
  CreateSVG_SimpleArrow
  ----------------------------------------------------------------------*/
void CreateSVG_SimpleArrow (Document document, View view)
{
  CreateGraphicElement (document, view, 12);
}

/*----------------------------------------------------------------------
  CreateSVG_DoubleArrow
  ----------------------------------------------------------------------*/
void CreateSVG_DoubleArrow (Document document, View view)
{
  CreateGraphicElement (document, view, 13);
}

/*----------------------------------------------------------------------
  CreateSVG_Zigzag
  ----------------------------------------------------------------------*/
void CreateSVG_Zigzag (Document document, View view)
{
  CreateGraphicElement (document, view, 14);
}

/*----------------------------------------------------------------------
  CreateSVG_Square
  ----------------------------------------------------------------------*/
void CreateSVG_Square (Document document, View view)
{
  CreateGraphicElement (document, view, 15);
}

/*----------------------------------------------------------------------
  CreateSVG_RoundedSquare
  ----------------------------------------------------------------------*/
void CreateSVG_RoundedSquare (Document document, View view)
{
  CreateGraphicElement (document, view, 16);
}

/*----------------------------------------------------------------------
  CreateSVG_Diamond
  ----------------------------------------------------------------------*/
void CreateSVG_Diamond (Document document, View view)
{
  CreateGraphicElement (document, view, 17);
}

/*----------------------------------------------------------------------
  CreateSVG_Trapezium
  ----------------------------------------------------------------------*/
void CreateSVG_Trapezium (Document document, View view)
{
  CreateGraphicElement (document, view, 18);
}

/*----------------------------------------------------------------------
  CreateSVG_Parallelogram
  ----------------------------------------------------------------------*/
void CreateSVG_Parallelogram (Document document, View view)
{
  CreateGraphicElement (document, view, 19);
}

/*----------------------------------------------------------------------
  CreateSVG_EquilateralTriangle
  ----------------------------------------------------------------------*/
void CreateSVG_EquilateralTriangle (Document document, View view)
{
  CreateGraphicElement (document, view, 20);
}

/*----------------------------------------------------------------------
  CreateSVG_IsoscelesTriangle
  ----------------------------------------------------------------------*/
void CreateSVG_IsoscelesTriangle (Document document, View view)
{
  CreateGraphicElement (document, view, 21);
}

/*----------------------------------------------------------------------
  CreateSVG_RectangleTriangle
  ----------------------------------------------------------------------*/
void CreateSVG_RectangleTriangle (Document document, View view)
{
  CreateGraphicElement (document, view, 22);
}

/*----------------------------------------------------------------------
  CreateSVG_Cube
  ----------------------------------------------------------------------*/
void CreateSVG_Cube (Document document, View view)
{
  CreateGraphicElement (document, view, 23);
}

/*----------------------------------------------------------------------
  CreateSVG_Parallelepiped
  ----------------------------------------------------------------------*/
void CreateSVG_Parallelepiped (Document document, View view)
{
  CreateGraphicElement (document, view, 24);
}

/*----------------------------------------------------------------------
  CreateSVG_Cylinder
  ----------------------------------------------------------------------*/
void CreateSVG_Cylinder (Document document, View view)
{
  CreateGraphicElement (document, view, 25);
}
