/*
 *
 *  (c) COPYRIGHT INRIA and W3C, 1996-2010
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * This module contains editing functions for handling SVG objects.
 *
 * Author: I. Vatton
 *         V. Quint
 *         F. Wang - SVG panel
 */

/* Included headerfiles */

#ifdef _WX
#include "wx/wx.h"
#endif /* _WX */

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "amaya.h"
#include "css.h"
#include "parser.h"
#include "trans.h"
#include "view.h"
#include "content.h"
#include "document.h"
#include <math.h>
#include "svgedit.h"

#include "SVG.h"
#include "HTML.h"
#include "XLink.h"

#include "anim_f.h"
#include "Mathedit_f.h"
#include "SVGedit_f.h"
#include "UIcss_f.h"
#include "templateUtils_f.h"
#include "templates.h"
#include "templates_f.h"

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
#include "styleparser_f.h"
#include "registry_wx.h"
#include "wxdialogapi_f.h"

static ThotBool   PaletteDisplayed = FALSE;
static ThotBool   SVG_root_copied = FALSE;
static Element    PastedGraphics = NULL;
static Element    SavedGraphics = NULL;
static Element    SavedDefs = NULL;

extern int ActiveFrame;

#ifdef _WX
#include "appdialogue_wx.h"
#include "paneltypes_wx.h"
#endif /* _WX */

#ifdef _WINDOWS
#include <commctrl.h>
#endif /* _WINDOWS */

/*----------------------------------------------------------------------
  ClearSVGDefs removes all saved SVG definitions
  ----------------------------------------------------------------------*/
void ClearSVGDefs ()
{
  if (SavedDefs)
    {
      TtaDeleteTree (SavedDefs, 0);
      SavedDefs = NULL;
    }
  SavedGraphics = NULL;
  SVG_root_copied = FALSE;
}

#ifdef _SVG
/*----------------------------------------------------------------------
  GetGraphicsUnit
  -----------------------------------------------------------------------*/
Element GetGraphicsUnit(Element element)
{
  Element leaf;
  ElementType   elType;

  for(leaf = TtaGetFirstChild(element);
      leaf;
      TtaNextSibling(&leaf))
    {
      elType = TtaGetElementType(leaf);
      if(elType.ElTypeNum == SVG_EL_GRAPHICS_UNIT)
	break;
    }

  return leaf;
}

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
  GetReferredDef returns the referred symbol or marker included in the
  root element.
  ----------------------------------------------------------------------*/
static Element GetReferredDef (Attribute attr, Element root, SSchema svgSchema)
{
  Element              child, next;
  AttributeType        idType;
  Attribute            idAttr;
  char                *attrVal = NULL, *start, *end, *id;
  int                  length;
  
  if (attr)
    /* the element has this attribute */
    {
      /* get its value */
      length = TtaGetTextAttributeLength (attr);
      attrVal = (char *)TtaGetMemory (length + 1);
      TtaGiveTextAttributeValue (attr, attrVal, &length);
      start = strstr (attrVal, "#");
      if (start)
        {
          start++;
          end = strstr (attrVal, ")");
          if (end)
            *end = EOS;
        }
      else
        start = attrVal;
      /* look for the id */
      idType.AttrSSchema = svgSchema;
      idType.AttrTypeNum = SVG_ATTR_id;
      child = TtaGetFirstChild (root);
      while (child)
        {
          next = child;
          TtaNextSibling (&next);
          // check if the element has an id
          idAttr = TtaGetAttribute (child, idType);
          if (idAttr)
            {
              length = TtaGetTextAttributeLength (idAttr);
              id = (char *)TtaGetMemory (length + 1);
              TtaGiveTextAttributeValue (idAttr, id, &length);
              if (!strcmp (start, id))
                {
                  // the referred element is found
                  TtaFreeMemory (id);
                  return child;
                }
              TtaFreeMemory (id);
            }
          child = next;
        }
      TtaFreeMemory (attrVal);
    }
  // not found
  return NULL;
}

/*----------------------------------------------------------------------
  SaveSVGDefs saves referred markers or symbols
  ----------------------------------------------------------------------*/
void SaveSVGDefs (Element el, Document doc)
{
  Element              svgCanvas, defs = NULL, prev, ref, copy;
  ElementType	         elType;
  AttributeType        attrType;
  Attribute            attr;
  int                  typenum;
  SSchema	             svgSchema;

  svgSchema = TtaGetSSchema ("SVG", doc);
  if (SVG_root_copied)
    return;
  if (svgSchema)
    {
      elType = TtaGetElementType (el);
      if (elType.ElSSchema == svgSchema && elType.ElTypeNum == SVG_EL_SVG)
        {
          // the whole SVG element is copied
          ClearSVGDefs ();
          SVG_root_copied = TRUE;
        }
      else if (elType.ElSSchema == svgSchema &&
               (elType.ElTypeNum == SVG_EL_use_ ||
                elType.ElTypeNum == SVG_EL_marker))
        {
          /* Look for the ancestor svgCanvas ans definitions */
          typenum = elType.ElTypeNum;
          elType.ElTypeNum = SVG_EL_SVG;
          elType.ElSSchema = svgSchema;
          svgCanvas = TtaGetTypedAncestor (el, elType);
          if (svgCanvas)
            {
              elType.ElTypeNum = SVG_EL_defs;
              defs = TtaSearchTypedElement (elType, SearchInTree, svgCanvas);
            }
          if (defs && defs != TtaGetParent (el))
            {
              if (SavedDefs == NULL)
                // create if necessary a saved defs
                SavedDefs = TtaNewElement (doc, elType);
              else if (SavedGraphics != svgCanvas)
                {
                  // the svgCanvas changed
                  do
                    {
                      copy = TtaGetFirstChild (SavedDefs);
                      if (copy)
                        TtaDeleteTree (copy, 0);
                    }
                  while (copy);
                }
              SavedGraphics = svgCanvas;
              if (typenum == SVG_EL_use_)
                {
                  // get the referred symbol
                  attrType.AttrSSchema = elType.ElSSchema;
                  attrType.AttrTypeNum = SVG_ATTR_xlink_href;
                  attr = TtaGetAttribute (el, attrType);
                  if (attr && !GetReferredDef (attr, SavedDefs, svgSchema))
                    {
                      // the symbol is not already saved
                      ref = GetReferredDef (attr, defs, svgSchema);
                      if (ref)
                        {
                          copy = TtaCopyTree (ref, doc, doc, SavedDefs);
                          TtaInsertFirstChild (&copy, SavedDefs, doc);
                        }
                    }
                  else
                    {
                      attrType.AttrSSchema = GetXLinkSSchema (doc);
                      attrType.AttrTypeNum = XLink_ATTR_href_;
                      if (attr && !GetReferredDef (attr, SavedDefs, svgSchema))
                        {
                          // the symbol is not already saved
                          ref = GetReferredDef (attr, defs, svgSchema);
                          if (ref)
                            {
                              copy = TtaCopyTree (ref, doc, doc, SavedDefs);
                              TtaInsertFirstChild (&copy, SavedDefs, doc);
                            }
                        }
                    }
                }
              else
                {
                  prev = el;
                  TtaPreviousSibling (&prev);
                  elType = TtaGetElementType (prev);
                  if (elType.ElTypeNum == SVG_EL_marker)
                    // all markers of the element are already managed
                    return;
                  el = TtaGetParent (el);
                  attrType.AttrSSchema = elType.ElSSchema;
                  attrType.AttrTypeNum = SVG_ATTR_marker_start;
                  attr = TtaGetAttribute (el, attrType);
                  if (attr && !GetReferredDef (attr, SavedDefs, svgSchema))
                    {
                      // the symbol is not already saved
                      ref = GetReferredDef (attr, defs, svgSchema);
                      if (ref)
                        {
                          copy = TtaCopyTree (ref, doc, doc, SavedDefs);
                          TtaInsertFirstChild (&copy, SavedDefs, doc);
                        }
                    }
                  attrType.AttrTypeNum = SVG_ATTR_marker_mid;
                  attr = TtaGetAttribute (el, attrType);
                  if (attr && !GetReferredDef (attr, SavedDefs, svgSchema))
                    {
                      // the symbol is not already saved
                      ref = GetReferredDef (attr, defs, svgSchema);
                      if (ref)
                        {
                          copy = TtaCopyTree (ref, doc, doc, SavedDefs);
                          TtaInsertFirstChild (&copy, SavedDefs, doc);
                        }
                    }
                  attrType.AttrTypeNum = SVG_ATTR_marker_end;
                  attr = TtaGetAttribute (el, attrType);
                  if (attr && !GetReferredDef (attr, SavedDefs, svgSchema))
                    {
                      // the symbol is not already saved
                      ref = GetReferredDef (attr, defs, svgSchema);
                      if (ref)
                        {
                          copy = TtaCopyTree (ref, doc, doc, SavedDefs);
                          TtaInsertFirstChild (&copy, SavedDefs, doc);
                        }
                    }
                }
            }
         }
    }
  else
    ClearSVGDefs ();
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void InsertSVGDefs (Element el, Document doc)
{
  Element              svgCanvas, defs = NULL, child, next, copy;
  ElementType	         elType;
  AttributeType        idType;
  Attribute            idAttr;
  SSchema	             svgSchema;

  if (SavedDefs)
    {
      svgSchema = TtaGetSSchema ("SVG", doc);
      if (svgSchema)
        {
          elType = TtaGetElementType (el);
          if (elType.ElSSchema == svgSchema &&
              (elType.ElTypeNum == SVG_EL_symbol_ ||
               elType.ElTypeNum == SVG_EL_marker))
            {
               /* Search the ancestor which is a direct child of the svgCanvas */
              el = TtaGetParent (el);
              elType = TtaGetElementType (el);
              if (elType.ElTypeNum == SVG_EL_defs)
                return;
              elType.ElTypeNum = SVG_EL_SVG;
              elType.ElSSchema = svgSchema;
              svgCanvas = TtaGetTypedAncestor (el, elType);
              if (svgCanvas)
                {
                  if (svgCanvas == SavedGraphics)
                    // nothing to do
                    return;
                  elType.ElTypeNum = SVG_EL_defs;
                  defs = TtaSearchTypedElement (elType, SearchInTree, svgCanvas);
                }
              if (defs)
                {
                  /* look for the id */
                  idType.AttrSSchema = svgSchema;
                  idType.AttrTypeNum = SVG_ATTR_id;
                  child = TtaGetFirstChild (SavedDefs);
                  while (child)
                    {
                      next = child;
                      TtaNextSibling (&next);
                      // check if the element is already there
                      idAttr = TtaGetAttribute (child, idType);
                      if (idAttr && !GetReferredDef (idAttr, defs, svgSchema))
                        {
                          copy = TtaCopyTree (child, doc, doc, svgCanvas);
                          TtaInsertFirstChild (&copy, defs, doc);
                          TtaRegisterElementCreate (copy, doc);
                        }
                      child = next;
                    }
                }
              else
                {
                  defs = TtaCopyTree (SavedDefs, doc, doc, svgCanvas);
                  TtaInsertFirstChild (&defs, svgCanvas, doc);
                  TtaRegisterElementCreate (defs, doc);
                }
            }
        }
    }
}

/*----------------------------------------------------------------------
  CopyGraphElem
  An element will be copied
  -----------------------------------------------------------------------*/
ThotBool CopyGraphElem (NotifyElement *event)
{
  SaveSVGDefs (event->element, event->document);
  return FALSE; /* let Thot perform normal operation */
}

/*----------------------------------------------------------------------
  A new element has been selected.
  Check that this element can be selected.
  Synchronize selection in source view.
  ----------------------------------------------------------------------*/
void GraphicsSelectionChanged (NotifyElement * event)
{
  Element      asc, use;
  AttributeType    attrType;
  ElementType  elType;
  int          elemType = 0;
  SSchema      svgSchema;
  Document doc;
  View view;

  TtaGiveActiveView(&doc, &view);

  svgSchema = GetSVGSSchema (event->document);
  attrType.AttrSSchema = svgSchema;
  elType = TtaGetElementType (event->element);
  
  if (view == 1 && elType.ElSSchema == svgSchema &&
      /* In formatted view, you can only select the direct children of the
         svgCanvas */
      (elType.ElTypeNum == SVG_EL_g ||
       elType.ElTypeNum == SVG_EL_path ||
       elType.ElTypeNum == SVG_EL_rect ||
       elType.ElTypeNum == SVG_EL_circle_ ||
       elType.ElTypeNum == SVG_EL_ellipse ||
       elType.ElTypeNum == SVG_EL_line_ ||
       elType.ElTypeNum == SVG_EL_polyline ||
       elType.ElTypeNum == SVG_EL_polygon ||
       elType.ElTypeNum == SVG_EL_text_ ||
       elType.ElTypeNum == SVG_EL_image ||
       elType.ElTypeNum == SVG_EL_switch ||
       elType.ElTypeNum == SVG_EL_GRAPHICS_UNIT)
      )
    {
    }
  else
    {
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
    }

  /* Update the style panel */
  if(event->elementType.ElTypeNum != SVG_EL_GraphicsElement)
    UpdateStylePanelSVG(doc, view, event->element);

  CheckSynchronize (event);
  Selection_changed_in_basedoc (event);
  /* update the displayed style information */
  SynchronizeAppliedStyle (event);
  UnFrameMath ();

  //UpdateXmlElementListTool(event->element,event->document);
  TtaSetStatusSelectedElement(event->document, 1, event->element);
#ifdef TEMPLATES
  if (!IsTemplateDocument (event->document))
#endif /* TEMPLATES */
    TtaRaiseDoctypePanels(WXAMAYA_DOCTYPE_SVG);
}

/*----------------------------------------------------------------------
  ExtendSelectSVGElement
  The user wants to add a new element in the current selection.
  -----------------------------------------------------------------------*/
ThotBool ExtendSelectSVGElement (NotifyElement *event)
{
  Element	    firstSel, newFirstSel, ancestor, parent, selEl;
  ElementType	elType, ancestType, parentType;
  int		      c1, i;
  SSchema	    svgSchema;

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
  svgSchema = TtaGetSSchema ("SVG", event->document);
  ancestType = TtaGetElementType (ancestor);
  if (ancestType.ElSSchema != svgSchema)
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
      while (parent != NULL && parentType.ElSSchema != svgSchema);
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
  if (elType.ElSSchema != svgSchema ||
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
      elType.ElSSchema = svgSchema;
      elType.ElTypeNum = SVG_EL_GraphicsElement;
      newFirstSel = TtaGetTypedAncestor (newFirstSel, elType);
    }

  selEl = event->element;
  elType = TtaGetElementType (selEl);
  if (elType.ElSSchema != svgSchema ||
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
      elType.ElSSchema = svgSchema;
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
  if (event->info == 1)
    ParseCoordAttribute (event->attribute, event->element, event->document);
}

/*----------------------------------------------------------------------
  AttrTransformChanged
  -----------------------------------------------------------------------*/
void AttrTransformChanged (NotifyAttribute *event)
{
  TtaRemoveTransform (event->document, event->element);
  ParseTransformAttribute (event->attribute, event->element, event->document,
                           FALSE, FALSE);
  /*******   CheckSVGRoot (event->document, event->element); *****/
}

/*----------------------------------------------------------------------
  AttrTransformDelete : attribute transform will be
  deleted. Remove the corresponding style presentation.
  ----------------------------------------------------------------------*/
ThotBool AttrTransformDelete (NotifyAttribute * event)
{
  TtaRemoveTransform (event->document, event->element);
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
  TtaRemovePathData (event->document, event->element);
  ParsePathDataAttribute (event->attribute, event->element, event->document, TRUE);
}

/*----------------------------------------------------------------------
  AttrPathDataDelete
  The user tries to delete attribute d of a path element.
  Don't let him/her do that!
  ----------------------------------------------------------------------*/
ThotBool AttrPathDataDelete (NotifyAttribute * event)
{
  if (event->info == 1)
    // undo operation: let thotlib manage the attribute
    return FALSE;
  TtaRemovePathData (event->document, event->element);
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
  Attribute            attr;
  int                  v, e;
  int                  pval, pe, i;
  char		             buffer[BUFFER_LENGTH], unit[BUFFER_LENGTH];
  ThotBool             delete_ = FALSE;

  attr = TtaGetAttribute (el, attrType);
  if (value <= 0 &&
      (attrType.AttrTypeNum == SVG_ATTR_rx || attrType.AttrTypeNum == SVG_ATTR_ry))
    {
      if (attr)
        {
          TtaRegisterAttributeDelete (attr, el, doc);
          TtaRemoveAttribute (el, attr, doc);
        }
      delete_ = TRUE;
    }
  else if (attr == NULL)
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
      else if (delete_ && attr == NULL)
        {
          attr = TtaNewAttribute (attrType);
          ParseWidthHeightAttribute (attr, el, doc, delete_);
          TtaRemoveAttribute (NULL, attr, 0);
        }
      else
        ParseWidthHeightAttribute (attr, el, doc, delete_);
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
  //ThotBool withundo = (event->info != 1);
  ParseWidthHeightAttribute (event->attribute, event->element,
                             event->document, FALSE);
}

/*----------------------------------------------------------------------
  AttrWidthHeightDelete
  -----------------------------------------------------------------------*/
ThotBool AttrWidthHeightDelete (NotifyAttribute *event)
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
  attrType.AttrSSchema = TtaGetSSchema ("SVG", doc);/* elType.ElSSchema; */
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
           elType.ElTypeNum == SVG_EL_foreignObject)
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
           elType.ElTypeNum == SVG_EL_polygon ||
           elType.ElTypeNum == SVG_EL_g)
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


/*----------------------------------------------------------------------
  NewGraphElement
  An element will be pasted
  -----------------------------------------------------------------------*/
ThotBool NewGraphElem (NotifyOnValue *event)
{
  ElementType    elType;
  int           profile;

  // keep in memory tthe parent of the pasted element
  elType = TtaGetElementType (event->element);
  if (elType.ElSSchema &&
      !strcmp (TtaGetSSchemaName (elType.ElSSchema), "SVG"))
    PastedGraphics = event->element;
  else
    PastedGraphics = NULL;

  /* is it a compound document? */
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
  Element        el, parent;
  int            profile, doc;

  XLinkPasted (event);
  el = event->element;
  doc = event->document;
  /* it's a compound document */
  profile = TtaGetDocumentProfile (doc);
  if (DocumentTypes[doc] == docMath ||
      profile == L_Strict || profile == L_Basic)
    return;
  else if (DocumentTypes[doc] != docSVG && DocumentMeta[doc])
    DocumentMeta[doc]->compound = TRUE;

  /* Set the namespace declaration if it's an <svg> element that is not
     within an element belonging to the SVG namespace */
  elType = TtaGetElementType (el);
  if (elType.ElSSchema &&
      !strcmp (TtaGetSSchemaName (elType.ElSSchema), "SVG"))
    {
      if (elType.ElTypeNum == SVG_EL_SVG)
        /* it's an <svg> element */
        SVGCreated (event);
      else if (elType.ElTypeNum == SVG_EL_title)
        /* the pasted element is a title element. Update the window title */
        UpdateTitle (el, doc);
      else
        {
          InsertSVGDefs (el, doc);
          // check if the element must be shifted
          parent = TtaGetParent (el);
          if (parent == PastedGraphics)
            // translate 
            TtaApplyMatrixTransform (doc, el, 1, 0, 0, 1, 4, 4);
        }
    }

  /* Check attribute NAME or ID in order to make sure that its value */
  /* is unique in the document */
  MakeUniqueName (el, doc, TRUE, FALSE);
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
    return CheckTemplateAttrInMenu(event);
#else /* TEMPLATES */
  return FALSE;
#endif /* TEMPLATES */

  if (strcmp (TtaGetSSchemaName (elType.ElSSchema),"SVG"))
    /* it's not a SVG element, don't put a SVG attribute in the menu */
    return TRUE;

#ifdef TEMPLATES
  return CheckTemplateAttrInMenu(event);
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
  GetElementData gets the SVG title or descriptor (according to the
  given el_type_num) value of of the element el.
  Return a string that gives the value or NULL.
  The returned string mist be freed.
  -----------------------------------------------------------------------*/
char *GetElementData (Document doc, Element el, SSchema sschema, int el_type_num)
{
  Element child;
  ElementType       elType;
  Language       lang;
  int len;
  char *text;

  // check the requested type
  if(el_type_num != SVG_EL_title && el_type_num != SVG_EL_desc)
    return NULL;

  elType = TtaGetElementType (el);
  if(elType.ElSSchema != sschema)
    return NULL;

  child = TtaGetFirstChild(el);
  while(child)
    {
      elType = TtaGetElementType (child);
      if(elType.ElSSchema == sschema && 
         elType.ElTypeNum == el_type_num)
        break;
     
      TtaNextSibling(&child);
    }

  if(child == NULL)
    return NULL;

  child = TtaGetFirstChild(child);
  elType = TtaGetElementType (child);
  if(elType.ElTypeNum != SVG_EL_TEXT_UNIT)
    return NULL;

  len = TtaGetTextLength(child);
  if(len == 0)
    return NULL;
  text = (char *) TtaGetMemory (len+1);
  TtaGiveTextContent (child, (unsigned char *)text, &len, &lang);
  return text;
}

/*----------------------------------------------------------------------
  SetElementData creates or update the title or description attached to
  the element el.
  The parameter value 
  -----------------------------------------------------------------------*/
ThotBool SetElementData (Document doc, Element el,
                         SSchema sschema, int el_type_num, char *value)
{
  Element child, text_unit;
  ElementType       elType;
  ThotBool remove;

  if(el_type_num != SVG_EL_title && el_type_num != SVG_EL_desc)
    return FALSE;

  elType = TtaGetElementType (el);
  if(elType.ElSSchema != sschema)
    return FALSE;

  child = TtaGetFirstChild(el);
  while(child)
    {
      elType = TtaGetElementType (child);
      if(elType.ElSSchema == sschema && 
         elType.ElTypeNum == el_type_num)
        break;
      TtaNextSibling(&child);
    }

  remove = (value == NULL || *value == EOS);

  if(child == NULL)
    {
      if(remove)
	/* Nothing to remove */
        return TRUE;

      /* No element found, insert one */
      elType.ElSSchema = sschema;
      elType.ElTypeNum = el_type_num;
      child = TtaNewElement (doc, elType);
      TtaInsertFirstChild(&child, el, doc);

      elType.ElTypeNum = SVG_EL_TEXT_UNIT;
      text_unit = TtaNewElement (doc, elType);
      TtaInsertFirstChild(&text_unit, child, doc);
      TtaRegisterElementCreate (child, doc);
    }
  else
    {
      if(remove)
        {
          TtaRegisterElementDelete (child, doc);
          TtaDeleteTree(child, doc);
          return TRUE;
        }

      text_unit = TtaGetFirstChild(child);
      elType = TtaGetElementType (text_unit);
      if(elType.ElTypeNum != SVG_EL_TEXT_UNIT)
        return FALSE;
      TtaRegisterElementReplace (text_unit, doc);
    }

  TtaSetTextContent(text_unit, (unsigned char *)value,
                    TtaGetDefaultLanguage (), doc);
  return TRUE;
}

/*----------------------------------------------------------------------
  GetAncestorCanvasAndObject
  Get all the elements necessary to draw SVG:
  
  - svgCanvas: the innermost <svg> element containing el
  - svgAncestor: the ancestor last <svg> ancestor of el, so we suppose
  that no transform are applied to it.
  - el: The element clicked/selected SVG object. If it is not a direct
  child of the svgCanvas, then we take the 
  
  ----------------------------------------------------------------------*/
ThotBool GetAncestorCanvasAndObject (Document doc, Element *el,
                                     Element *svgAncestor, Element *svgCanvas)
{
#ifdef _SVG
  Element parent, newEl;
  ElementType elType;
  SSchema	    docSchema, svgSchema;

  if (doc == 0)return FALSE;

  docSchema = TtaGetDocumentSSchema (doc);
  svgSchema = GetSVGSSchema (doc);

  if (el)
    {
      elType = TtaGetElementType (*el);

      /* Check whether el is an SVG element */
      if (elType.ElSSchema != svgSchema)
        return FALSE;

      newEl = *el;
      parent = TtaGetParent(newEl);

      /* Look for el and svgCanvas */
      while(parent)
        {
          elType = TtaGetElementType (parent);
          if (elType.ElTypeNum == SVG_EL_SVG &&
              elType.ElSSchema == svgSchema)
            break;

          newEl = parent;
          parent = TtaGetParent(parent);
        }

      *el = newEl;
      *svgCanvas = parent;
    }

  if (*svgCanvas == NULL)return FALSE;
  
  /* Look for svgAncestor */
  *svgAncestor = *svgCanvas;
  parent = *svgAncestor;
  TtaGetParent(parent);

  while(parent)
    {
      elType = TtaGetElementType (parent);
      if (elType.ElTypeNum == SVG_EL_SVG && elType.ElSSchema == svgSchema)
        *svgAncestor = parent;
      parent = TtaGetParent(parent);
    }

  return TRUE;

#endif /* _SVG */
}


/*----------------------------------------------------------------------
  searchMarkers
  ----------------------------------------------------------------------*/
static Element searchMarkers(Document doc, Element svg, const char *marker_id)
{
  SSchema           svgSchema;
  ElementType       defsType, markerType;
  Element defs, marker;

  AttributeType  attrType;
  Attribute      attr;

  char buffer[MAX_LENGTH];
  int len;

  svgSchema = GetSVGSSchema (doc);
  defsType.ElTypeNum = SVG_EL_defs;
  defsType.ElSSchema = svgSchema;
  markerType.ElTypeNum = SVG_EL_marker;
  markerType.ElSSchema = svgSchema;

  attrType.AttrSSchema = svgSchema;
  attrType.AttrTypeNum = SVG_ATTR_id;

  len = strlen(marker_id);
  if(len >= MAX_LENGTH)
    return NULL;

  for(defs = TtaSearchTypedElement (defsType, SearchInTree, svg); defs;
      defs = TtaSearchTypedElementInTree (defsType, SearchForward, svg, defs))
    {
      for (marker = TtaSearchTypedElement (markerType, SearchInTree, defs);
           marker;
           marker = TtaSearchTypedElementInTree (markerType, SearchForward,
                                                 defs, marker))
        {
          /* Get the id attribute */
          attr = TtaGetAttribute (marker, attrType);
          if (attr)
            {
              len = MAX_LENGTH - 1;
              TtaGiveTextAttributeValue (attr, buffer, &len);
              
              if(!strcmp(buffer, marker_id))
                return marker;
            }
        }
    }

  return NULL;
}

/*----------------------------------------------------------------------
  LoadSVG_Markers
  Check if an SVG marker of id marker_id exists in the document
  If no marker is found, load one from the file resources/svg/markers.svg
  return TRUE = success
  ----------------------------------------------------------------------*/
static ThotBool LoadSVG_Markers (Document doc, const char *marker_id,
                                 Element svgCanvas, SSchema svgSchema)
{
  Element           el, defs, marker;
  ElementType       elType;
  Document          markersDoc;  
  const char       *name = "markers.svg";
  wxString          path;
  char             *path2;
  DisplayMode       dispMode = TtaGetDisplayMode (doc); 
  ThotBool          oldStructureChecking = TtaGetStructureChecking (doc);

  if (svgCanvas == NULL)
    return FALSE;
  /* Search markers in the <svg/> root */
  if (searchMarkers (doc, svgCanvas, marker_id) != NULL)
    return TRUE;

  /* The marker_id was not found: open markers.svg */
  path = TtaGetResourcePathWX (WX_RESOURCES_SVG, name);
  path2 = TtaStrdup (path.mb_str(wxConvUTF8));
  markersDoc = GetAmayaDoc (path2, NULL,
                            0, 0, CE_TEMPLATE, FALSE, NULL, NULL);
  TtaFreeMemory (path2);

  /* Search marker_id in markersDoc */
  elType.ElTypeNum = SVG_EL_SVG;
  elType.ElSSchema = svgSchema;
  el = TtaGetMainRoot (markersDoc);
  el = TtaSearchTypedElement (elType, SearchInTree, el);
  marker = searchMarkers (markersDoc, el, marker_id);
  if (marker == NULL)
    {
      /* The marker_id does not match any marker in markers.svg */
      TtaRemoveDocumentReference (markersDoc);
      return FALSE;
    }

  /* Create or get a <defs> element */
  elType.ElTypeNum = SVG_EL_defs;
  elType.ElSSchema = svgSchema;
  defs = TtaSearchTypedElement (elType, SearchInTree, svgCanvas);
  if (defs == NULL)
    {
      /* No <defs/> element: create one as the first child of the <svg/> */
      defs = TtaNewElement(doc, elType);
      TtaInsertFirstChild(&defs, svgCanvas, doc);
      TtaRegisterElementCreate (defs, doc);
    }
  
  /* Copy the marker into the document */
  el = TtaCopyTree (marker, markersDoc, doc, defs);
  TtaInsertFirstChild (&el, defs, doc);
  TtaRegisterElementCreate (el, doc);

  TtaRemoveDocumentReference (markersDoc);
  TtaSetStructureChecking (oldStructureChecking, doc);
  TtaSetDisplayMode (doc, dispMode);
  return TRUE;
}

/*----------------------------------------------------------------------
  AttachMarker

  attrnum is SVG_ATTR_marker_end, SVG_ATTR_marker_start, SVG_ATTR_marker_mid
  and it is assumed that it does not exist.

  return TRUE = success
  ----------------------------------------------------------------------*/
static ThotBool AttachMarker (Document doc, Element el, int attrnum,
                              const char *marker_id, Element svgCanvas,
                              SSchema svgSchema)
{
  Attribute     attr;
  AttributeType attrType;
  char         *buffer;

  buffer = (char *)TtaGetMemory(strlen(marker_id) + 10);
  if(buffer == NULL)
    return FALSE;
    
  if (!LoadSVG_Markers (doc, marker_id, svgCanvas, svgSchema))
    return FALSE;
  TtaExtendUndoSequence (doc);
  attrType.AttrTypeNum = attrnum;
  attrType.AttrSSchema = svgSchema;
  attr = TtaNewAttribute (attrType);
  TtaAttachAttribute (el, attr, doc);
  sprintf(buffer, "url(#%s)", marker_id);
  TtaSetAttributeText (attr, buffer, el, doc);
  TtaRegisterAttributeCreate (attr, el, doc);
  // generate a copy of the marker
  ParseCSSequivAttribute (attrnum, attr, el, doc, FALSE);
  TtaFreeMemory(buffer);
  return TRUE;
}

/*----------------------------------------------------------------------
  CreateGraphicElement
  Create a Graphics element.
  entry is the number of the entry chosen by the user in the Graphics
  palette.
  ----------------------------------------------------------------------*/
void CreateGraphicElement (Document doc, View view, int entry)
{
#ifdef _SVG
  Document          tmpDoc;
  Element           svgAncestor, svgCanvas, root;
  Element	          first, newEl, sibling, selEl, next;
  Element           child, parent, elem, switch_, foreignObj, altText, leaf;
  ElementType       elType, selType, newType, childType;
  AttributeType     attrType, attrTypeHTML;
  Attribute         attr, inheritedAttr;
  SSchema	          docSchema, svgSchema;
  DisplayMode       dispMode;
  Language          lang;
  int		            c1, i, dir, svgDir, profile;
  int               docModified, error, w, h, shape;
  int               x1, y1, x2, y2, x3, y3, x4, y4, lx, ly;
  float             valx, valy;
  _ParserData       context;
  char              buffer[500], buffer2[200];
  /* Move this elsewhere when markers are used more */
  const char       *Arrow1Mend_id = "Arrow1Mend";
  const char       *Arrow1Mstart_id = "Arrow1Mstart";
  ThotBool	    found, newSVG = FALSE, replaceGraph = FALSE;
  ThotBool          created = FALSE;
  ThotBool          oldStructureChecking;
  ThotBool          isFormattedView, closed;   

  /* Check that a document is selected */
  if (doc == 0 || !TtaGetDocumentAccessMode (doc))
    {
      TtaDisplaySimpleMessage (CONFIRM, AMAYA, AM_NO_INSERT_POINT);
      return;
    }
  if (DocumentTypes[doc] == docSource ||
      DocumentTypes[doc] == docText || DocumentTypes[doc] == docLog)
    // cannot apply to a text file
    return;
  profile = TtaGetDocumentProfile (doc);
  if (profile == L_Strict || profile == L_Basic)
    {
      /* cannot insert here */
      TtaDisplaySimpleMessage (CONFIRM, AMAYA, AM_INVALID_SELECTION);
      return;
    }
  else if (DocumentTypes[doc] != docSVG && DocumentMeta[doc])
    {
      DocumentMeta[doc]->compound = TRUE;
      if (!DocumentMeta[doc]->xmlformat)
        {
          // the document becomes an XML document
          DocumentMeta[doc]->xmlformat = TRUE;
          root = TtaGetRootElement (doc);
          TtaSetANamespaceDeclaration (doc, root, NULL, XHTML_URI);
        }
    }
  context.doc = doc;
  /* Check that whether we are in formatted or strutured view. */
  if (view == 1)
    isFormattedView = TRUE;
  else if (view == 2)
    isFormattedView = FALSE;
  else return;

  TtaGiveFirstSelectedElement (doc, &first, &c1, &i);
  if (first == NULL && DocumentTypes[doc] == docSVG &&
      TtaGetSelectedDocument () == 0)
    {
      // get the main SVG element as the selected element
      first = TtaGetRootElement (doc);
      elType = TtaGetElementType (first);
    }
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
  else
    {
      TtaDisplaySimpleMessage (CONFIRM, AMAYA, AM_INVALID_SELECTION);
      return;
    }

  TtaOpenUndoSequence (doc, NULL, NULL, 0, 0);
  selEl = first;
  newEl = NULL;
  child = NULL;
  lx = ly = 1; // default width height
  docModified = TtaIsDocumentModified (doc);
  /* Are we in a drawing? */
  docSchema = TtaGetDocumentSSchema (doc);
  svgSchema = GetSVGSSchema (doc);
  attrType.AttrSSchema = svgSchema;
  elType = TtaGetElementType (selEl);
  if (elType.ElTypeNum == SVG_EL_SVG && elType.ElSSchema == svgSchema)
    svgCanvas = selEl;
  else
    {
      elType.ElTypeNum = SVG_EL_SVG;
      elType.ElSSchema = svgSchema;
      svgCanvas = TtaGetTypedAncestor (first, elType);
      if (svgCanvas == NULL)
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
          svgSchema = TtaNewNature (doc, docSchema, NULL, "SVG", "SVGP");
          if (TtaIsSelectionEmpty ())
            {
              /* try to create the SVG here */
              TtaCreateElement (elType, doc);
              TtaGiveFirstSelectedElement (doc, &elem, &c1, &i);
              selType = TtaGetElementType (elem);
              if (selType.ElTypeNum != elType.ElTypeNum)
                svgCanvas = TtaGetTypedAncestor (elem, elType);
              newSVG = TRUE;
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
              svgCanvas = TtaNewElement (doc, elType);
              TtaInsertSibling (svgCanvas, first, FALSE, doc);
              first = svgCanvas;
              newSVG = TRUE;
            }

          if (svgCanvas)
            /* a root SVG element was created. Create the required attributes*/
            {
              // mark the new Coordinate System
              TtaSetElCoordinateSystem (svgCanvas);
              /*attrType.AttrTypeNum = SVG_ATTR_version;
                attr = TtaNewAttribute (attrType);
                TtaAttachAttribute (svgCanvas, attr, doc);
                TtaSetAttributeText (attr, SVG_VERSION, svgCanvas, doc);*/
              parent = TtaGetParent (svgCanvas);
              TtaGiveBoxSize (parent, doc, 1, UnPixel, &w, &h);
              sprintf (buffer, "%d", w);
              attrType.AttrTypeNum = SVG_ATTR_width_;
              attr = TtaGetAttribute (svgCanvas, attrType);
              if (attr == NULL)
                {
                  attr = TtaNewAttribute (attrType);
                  TtaAttachAttribute (svgCanvas, attr, doc);
                }
              TtaSetAttributeText (attr, buffer, svgCanvas, doc);
              ParseWidthHeightAttribute (attr, svgCanvas, doc, FALSE);

              attrType.AttrTypeNum = SVG_ATTR_height_;
              attr = TtaGetAttribute (svgCanvas, attrType);
              if (attr == NULL)
                {
                  attr = TtaNewAttribute (attrType);
                  TtaAttachAttribute (svgCanvas, attr, doc);
                }
              TtaSetAttributeText (attr, "300", svgCanvas, doc);
              ParseWidthHeightAttribute (attr, svgCanvas, doc, FALSE);
              // center the svg element
              attrType.AttrTypeNum = SVG_ATTR_style_;
              attr = TtaGetAttribute (svgCanvas, attrType);
              if (attr == NULL)
                {
                  attr = TtaNewAttribute (attrType);
                  TtaAttachAttribute (svgCanvas, attr, doc);
                }
              strcpy (buffer, "margin-left: auto; margin-right: auto");
              TtaSetAttributeText (attr, buffer, svgCanvas, doc);
              ParseHTMLSpecificStyle (svgCanvas, buffer, doc, 1000, FALSE);
              // point to this default selection
              selEl = svgCanvas;
            }
        }
    }

  /* Look for the outermost <svg> element containg the svgCanvas. */
  GetAncestorCanvasAndObject(doc, NULL, &svgAncestor, &svgCanvas);

  /* look for the element (sibling) in front of which the new element will be
     created */
  sibling = NULL;
  if (first == svgCanvas)
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
              if (elType.ElSSchema == svgSchema &&
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
      parent = svgCanvas;
      sibling = TtaGetLastChild (svgCanvas);
    }


  /* Check whether the sibling is a graphics element */
  elType = TtaGetElementType (sibling);
  replaceGraph = (elType.ElTypeNum == SVG_EL_GraphicsElement);

  if (isFormattedView)
    {
      /* Select the SVG element where we draw, so that we can see the frame */
      TtaSelectElement(doc, svgCanvas);
    }

  newType.ElSSchema = svgSchema;
  newType.ElTypeNum = 0;

  switch (entry)
    {
    case -2: /* template */
      newType.ElTypeNum = SVG_EL_g;
      break;

    case -1: /* <svg/> */
      newType.ElTypeNum = SVG_EL_SVG;
      break;

    case 0:	/* line */
      newType.ElTypeNum = SVG_EL_line_;
      break;

    case 1:	/* rectangle */
    case 2:	/* rectangle with rounded corners */
    case 15: /* square */
    case 16: /* rounded square */
      if (svgCanvas == svgAncestor)
        newType.ElTypeNum = SVG_EL_rect;
      else
        newType.ElTypeNum = SVG_EL_polygon;
      break;

      newType.ElTypeNum = SVG_EL_rect;
      break;

      newType.ElTypeNum = SVG_EL_rect;
      break;

    case 3:	/* circle */
      newType.ElTypeNum = SVG_EL_circle_;
      break;

    case 4:	/* ellipse */
      newType.ElTypeNum = SVG_EL_ellipse;
      break;

    case 5:	/* polyline */
      newType.ElTypeNum = SVG_EL_polyline;
      break;

    case 6:	/* polygon */
      newType.ElTypeNum = SVG_EL_polygon;
      break;

    case 7:	/* spline */
      newType.ElTypeNum = SVG_EL_path;
      break;

    case 8:	/* closed spline */
      newType.ElTypeNum = SVG_EL_path;
      break;

      /* switch and foreignObject */
    case 9: /* XHTML */
    case 56: /* MathML */
      if (isFormattedView)
        newType.ElTypeNum = SVG_EL_g;
      else
        newType.ElTypeNum = SVG_EL_switch;
      break;

    case 10:	/* text */
      newType.ElTypeNum = SVG_EL_text_;
      break;

    case 12: /* Simple start arrow */
      newType.ElTypeNum = SVG_EL_line_;
      break;

    case 13: /* Simple end arrow */
      newType.ElTypeNum = SVG_EL_line_;
      break;

    case 14: /* Double arrow */
      newType.ElTypeNum = SVG_EL_line_;
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

    default:
      break;
    }

  dispMode = TtaGetDisplayMode (doc);
  if (newType.ElTypeNum > 0 && (entry != -1 || !newSVG))
    {
      /* ask Thot to stop displaying changes made in the document */
      if (dispMode == DisplayImmediately)
        TtaSetDisplayMode (doc, DeferredDisplay);

      /* create the new element */
      newEl = TtaNewElement (doc, newType);
      if (isFormattedView)
        {
          /* Insert the element as the last child (i.e. in the foreground)
             of the svgCanvas */
          if (!replaceGraph)
            {
              sibling = TtaGetLastChild(svgCanvas);
              if (!sibling)
                TtaInsertFirstChild (&newEl, svgCanvas, doc);
              else
                TtaInsertSibling (newEl, sibling, FALSE, doc);
            }
          else if (!sibling)
            TtaInsertFirstChild (&newEl, parent, doc);
          else
            {
              elType = TtaGetElementType (sibling);
              if (elType.ElSSchema == svgSchema &&
                  elType.ElTypeNum == SVG_EL_GraphicsElement)
                /* the new element replaces the existing, empty element */
                TtaInsertFirstChild (&newEl, sibling, doc);
              else
                TtaInsertSibling (newEl, sibling, FALSE, doc);
            }
          // the top element is now inserted
          sibling = NULL;
          replaceGraph = FALSE;
        }

      if (entry == -2 && LastSVGelement != NULL)
        {
          // load the temporary document without display
          tmpDoc = GetAmayaDoc (LastSVGelement, NULL,
                                0, 0, CE_TEMPLATE, FALSE, NULL, NULL);
          TtaFreeMemory(LastSVGelement);
          LastSVGelement = NULL;
          parent = TtaGetMainRoot(tmpDoc);
          elType.ElSSchema = svgSchema;
          elType.ElTypeNum = SVG_EL_SVG;
          parent = TtaSearchTypedElement(elType, SearchForward, parent);
          if (parent)
            {
              // get the width and height of the template
              attrType.AttrTypeNum = SVG_ATTR_width_;
              attr = TtaGetAttribute (parent, attrType);
              if (attr)
                {
                  i = 50;
                  TtaGiveTextAttributeValue (attr, buffer, &i);
                 sscanf (buffer, "%d", &w);
                }
              else
                w = 200;
              attrType.AttrTypeNum = SVG_ATTR_height_;
              attr = TtaGetAttribute (parent, attrType);
              if (attr)
                {
                  i = 50;
                  TtaGiveTextAttributeValue (attr, buffer, &i);
                  sscanf (buffer, "%d", &h);
                }
              else
                h = 200;
              // default width height
              lx = w;
              ly = h;
              if (isFormattedView)
                created = AskSurroundingBox(doc, svgAncestor, svgCanvas,
                                            entry, &x1, &y1, &x2, &y2,
                                            &x3, &y3, &x4, &y4, &lx, &ly);
              else
                {
                  /* TODO: add a dialog box? */
                  created = FALSE;
                  x1 = y1 = 0;
                }
              
              /* mark the new Coordinate System */
              TtaSetElCoordinateSystem (newEl);
              child = TtaGetFirstChild(parent);
              next = NULL;
              while (child)
                {
                  elem = TtaCopyTree(child, tmpDoc, doc, newEl);
                  if (next)
                    TtaInsertSibling (elem, next, FALSE, doc);
                  else
                    TtaInsertFirstChild (&elem, newEl, doc);
                  if (created)
                    // adapt to the current given surrounding box
                    // UpdateSVGElement (doc, elem, w, h, x1, y1, lx - w, ly - h);
                  next = elem;
                  TtaNextSibling(&child);
                }
              if (created)
                {
                  valx = valy = 1;
                  if (w)
                    valx = (float)lx / (float)w;
                  if (h)
                    valy = (float)ly / (float)h;
                  //TtaGiveBoxPosition (newEl, doc, 1, UnPixel, TRUE, &x4, &y4);
                  attrType.AttrTypeNum = SVG_ATTR_transform;
                  attr = TtaNewAttribute (attrType);
                  TtaAttachAttribute (newEl, attr, doc);
                  sprintf(buffer, "translate(%d,%d) scale(%f,%f)", x1, y1, valx,valy);
                  TtaSetAttributeText (attr, buffer, newEl, doc);
                  ParseTransformAttribute (attr, newEl, doc, FALSE, FALSE);
                }
              created = TRUE;
            }
          TtaRemoveDocumentReference (tmpDoc);

          selEl = newEl;
          /* Update the title */
          SetElementData (doc, newEl, svgSchema, SVG_EL_title, LastSVGelementTitle);
          TtaFreeMemory (LastSVGelementTitle);
        }

      if (entry == -1)
        {
          /* <svg/> */
          lx = 500;
          ly = 300;
          if (isFormattedView)
            {
              created = AskSurroundingBox(doc, svgAncestor, svgCanvas,
                                          entry, &x1, &y1, &x2, &y2,
                                          &x3, &y3, &x4, &y4, &lx, &ly);
            }
          else
            {
              /* TODO: add a dialog box? */
              x1 = y1 = 0;
              created = TRUE;
            }

          attrType.AttrTypeNum = SVG_ATTR_x;
          UpdateAttrText (newEl, doc, attrType, x1, FALSE, TRUE);
          attrType.AttrTypeNum = SVG_ATTR_y;
          UpdateAttrText (newEl, doc, attrType, y1, FALSE, TRUE);
          attrType.AttrTypeNum = SVG_ATTR_width_;
          UpdateAttrText (newEl, doc, attrType, lx, FALSE, TRUE);
          attrType.AttrTypeNum = SVG_ATTR_height_;
          UpdateAttrText (newEl, doc, attrType, ly, FALSE, TRUE);
          selEl = newEl;
        }
      else if ((0 < entry && entry <= 4) || (entry > 14 && entry <= 25))
        {
          /* Basic Shapes and lines */
          selEl = newEl;
          /*
            1-------------2
            |             |
            |             |
            |             |
            3-------------4
          */
          lx = ly = 100;
          if (isFormattedView)
            {
              created = AskSurroundingBox(doc, svgAncestor, svgCanvas,
                                          entry, &x1, &y1, &x2, &y2,
                                          &x3, &y3, &x4, &y4, &lx, &ly);
            }
          else
            {
              /* TODO: add a dialog box ? */
              x1 = x3 = 0;
              x2 = x4 = 100;
              y1 = y2 = 0;
              y3 = y4 = 100;
              created = TRUE;
            }

          if (created)
            {
              switch(entry)
                {
                case 15: /* Square */
                case 1: /* Rectangle */
                case 16: /* Rounded Square */
                case 2:  /* Rounded-Rectangle */
                  if (svgCanvas == svgAncestor)
                    {
                      if (x4 < x1)x1 = x4;
                      if (y4 < y1)y1 = y4;

                      attrType.AttrTypeNum = SVG_ATTR_x;
                      UpdateAttrText (newEl, doc, attrType, x1, FALSE, TRUE);
                      attrType.AttrTypeNum = SVG_ATTR_y;
                      UpdateAttrText (newEl, doc, attrType, y1, FALSE, TRUE);
                      attrType.AttrTypeNum = SVG_ATTR_width_;
                      UpdateAttrText (newEl, doc, attrType, lx, FALSE, TRUE);
                      attrType.AttrTypeNum = SVG_ATTR_height_;
                      UpdateAttrText (newEl, doc, attrType, ly, FALSE, TRUE);
                      SVGElementComplete (&context, newEl, &error);
                      if (entry == 16 || entry == 2)
                        {
                          attrType.AttrTypeNum = SVG_ATTR_rx;
                          attr = TtaNewAttribute (attrType);
                          TtaAttachAttribute (newEl, attr, doc);
                          sprintf(buffer, "%dpx", lx/4);
                          TtaSetAttributeText (attr, buffer, newEl, doc);
                          ParseWidthHeightAttribute (attr, newEl, doc, FALSE);
                          attrType.AttrTypeNum = SVG_ATTR_ry;
                          attr = TtaNewAttribute (attrType);
                          TtaAttachAttribute (newEl, attr, doc);
                          sprintf(buffer, "%dpx", ly/4);
                          TtaSetAttributeText (attr, buffer, newEl, doc);
                          ParseWidthHeightAttribute (attr, newEl, doc, FALSE);
                          SVGElementComplete (&context, newEl, &error);
                        }
                    }
                  else
                    {
                      attrType.AttrTypeNum = SVG_ATTR_points;
                      attr = TtaNewAttribute (attrType);
                      TtaAttachAttribute (newEl, attr, doc);
                      sprintf(buffer, "%d %d %d %d %d %d %d %d",
                              x1,y1,
                              x2,y2,
                              x4,y4,
                              x3,y3
                              );
                      TtaSetAttributeText (attr, buffer, newEl, doc);
                      ParsePointsAttribute (attr, newEl, doc);
                    }

                  break;

                  /* Circle */
                case 3:
                  attrType.AttrTypeNum = SVG_ATTR_cx;
                  UpdateAttrText (newEl, doc, attrType, (x1+x4)/2, FALSE, TRUE);

                  attrType.AttrTypeNum = SVG_ATTR_cy;
                  UpdateAttrText (newEl, doc, attrType, (y1+y4)/2, FALSE, TRUE);

                  attrType.AttrTypeNum = SVG_ATTR_r;
                  UpdateAttrText (newEl, doc, attrType, lx/2, FALSE, TRUE);


                  SVGElementComplete (&context, newEl, &error);
                  break;

                  /* Ellipse */
                case 4:
                  attrType.AttrTypeNum = SVG_ATTR_cx;
                  UpdateAttrText (newEl, doc, attrType, (x1+x4)/2, FALSE, TRUE);

                  attrType.AttrTypeNum = SVG_ATTR_cy;
                  UpdateAttrText (newEl, doc, attrType, (y1+y4)/2, FALSE, TRUE);

                  attrType.AttrTypeNum = SVG_ATTR_rx;
                  UpdateAttrText (newEl, doc, attrType, lx/2, FALSE, TRUE);

                  attrType.AttrTypeNum = SVG_ATTR_ry;
                  UpdateAttrText (newEl, doc, attrType, ly/2, FALSE, TRUE);;

                  SVGElementComplete (&context, newEl, &error);
                  break;

                case 17: /* diamond */
                  attrType.AttrTypeNum = SVG_ATTR_points;
                  attr = TtaNewAttribute (attrType);
                  TtaAttachAttribute (newEl, attr, doc);
                  sprintf(buffer, "%d %d %d %d %d %d %d %d",
                          (x1+x3)/2  , (y1+y3)/2,
                          (x1+x2)/2  , (y1+y2)/2,
                          (x2+x4)/2  , (y2+y4)/2,
                          (x4+x3)/2  , (y4+y3)/2
                          );
                  TtaSetAttributeText (attr, buffer, newEl, doc);
                  ParsePointsAttribute (attr, newEl, doc);
                  SVGElementComplete (&context, newEl, &error);
                  break;

                case 18: /* trapezium */
                  attrType.AttrTypeNum = SVG_ATTR_points;
                  attr = TtaNewAttribute (attrType);
                  TtaAttachAttribute (newEl, attr, doc);
                  sprintf(buffer, "%d %d %d %d %d %d %d %d",
                          x3, y3,
                          (3*x1+x2)/4, (3*y1+y2)/4,
                          (3*x2+x1)/4, (3*y2+y1)/4,
                          x4, y4
                          );
                  TtaSetAttributeText (attr, buffer, newEl, doc);
                  ParsePointsAttribute (attr, newEl, doc);
                  SVGElementComplete (&context, newEl, &error);
                  break;

                case 19: /* parallelogram */
                  attrType.AttrTypeNum = SVG_ATTR_points;
                  attr = TtaNewAttribute (attrType);
                  TtaAttachAttribute (newEl, attr, doc);
                  sprintf(buffer, "%d %d %d %d %d %d %d %d",
                          x3, y3,
                          (3*x1+x2)/4, (3*y1+y2)/4,
                          x2,y2,
                          (3*x4+x3)/4, (3*y4+y3)/4
                          );
                  TtaSetAttributeText (attr, buffer, newEl, doc);
                  ParsePointsAttribute (attr, newEl, doc);
                  SVGElementComplete (&context, newEl, &error);
                  break;

                case 20: /* equilateral triangle */
                  attrType.AttrTypeNum = SVG_ATTR_points;
                  attr = TtaNewAttribute (attrType);
                  TtaAttachAttribute (newEl, attr, doc);

                  sprintf(buffer, "%d %d %d %d %d %d",
                          (x1+x2)/2, (y1+y2)/2,
                          x3,y3,
                          x4,y4
                          );
                  TtaSetAttributeText (attr, buffer, newEl, doc);
                  ParsePointsAttribute (attr, newEl, doc);
                  SVGElementComplete (&context, newEl, &error);
                  break;

                case 21: /* isosceles triangle */
                  attrType.AttrTypeNum = SVG_ATTR_points;
                  attr = TtaNewAttribute (attrType);
                  TtaAttachAttribute (newEl, attr, doc);
                  sprintf(buffer, "%d %d %d %d %d %d",
                          (x1+x2)/2, (y1+y2)/2,
                          x3, y3,
                          x4, y4
                          );
                  TtaSetAttributeText (attr, buffer, newEl, doc);
                  ParsePointsAttribute (attr, newEl, doc);
                  SVGElementComplete (&context, newEl, &error);
                  break;

                case 22: /* rectangle triangle */
                  attrType.AttrTypeNum = SVG_ATTR_points;
                  attr = TtaNewAttribute (attrType);
                  TtaAttachAttribute (newEl, attr, doc);
                  sprintf(buffer, "%d %d %d %d %d %d",
                          x1, y1,
                          x2, y2,
                          x3, y3
                          );
                  TtaSetAttributeText (attr, buffer, newEl, doc);
                  ParsePointsAttribute (attr, newEl, doc);
                  SVGElementComplete (&context, newEl, &error);
                  break;

                default:
                  break;
                }
            }
        }
      else if (entry == 0 || (entry >= 12 && entry <= 14) ||
               (entry >= 5 && entry <= 8))
        {
          /* Polyline and curves */
          selEl = newEl;

          if (isFormattedView)
            {
              elType = TtaGetElementType(newEl);
              CreateGraphicLeaf (newEl, doc, &closed);
              created = TRUE;
            }
          else created = FALSE;
        }
      else if (entry == 9 || entry == 56)
        /* create a foreignObject containing an
           XHTML <div/> / MathML <math/> element */
        {
          created = TRUE;
          if (isFormattedView)
            {
              /* Ask the position and size */
              AskSurroundingBox(doc, svgAncestor, svgCanvas,
                                9, &x1, &y1, &x2, &y2,
                                &x3, &y3, &x4, &y4,
                                &lx, &ly);

              /* create a transform=translate attribute */
              attrType.AttrTypeNum = SVG_ATTR_transform;
              attr = TtaNewAttribute (attrType);
              TtaAttachAttribute (newEl, attr, doc);
              sprintf(buffer, "translate(%d,%d)", x1, y1);
              TtaSetAttributeText (attr, buffer, newEl, doc);
              ParseTransformAttribute (attr, newEl, doc, FALSE, FALSE);

              /* Create a switch element */
              childType.ElSSchema = svgSchema;
              childType.ElTypeNum = SVG_EL_switch;
              switch_ = TtaNewElement (doc, childType);
              TtaInsertFirstChild (&switch_, newEl, doc);
            }
          else
            switch_ = newEl;

          /* Create a foreign Object */
          childType.ElSSchema = svgSchema;
          childType.ElTypeNum = SVG_EL_foreignObject;
          foreignObj = TtaNewElement (doc, childType);
          TtaInsertFirstChild (&foreignObj, switch_, doc);

          /* associate a requiredExtensions attribute with the foreignObject
             element */
          attrType.AttrTypeNum = SVG_ATTR_requiredExtensions;
          attr = TtaNewAttribute (attrType);
          TtaAttachAttribute (foreignObj, attr, doc);

          if (entry == 9)
            TtaSetAttributeText (attr, XHTML_URI, foreignObj, doc);
          else 
            TtaSetAttributeText (attr, MathML_URI, foreignObj, doc);

          if (isFormattedView)
            {
              /* Size of the foreignObject */
              attrType.AttrTypeNum = SVG_ATTR_width_;
              UpdateAttrText (foreignObj, doc, attrType, 100, FALSE, TRUE);
              attrType.AttrTypeNum = SVG_ATTR_height_;
              UpdateAttrText (foreignObj, doc, attrType, 50, FALSE, TRUE);
            }

          if (entry == 9)
            {
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
            }
          else
            selEl = foreignObj;

	  
          /* create an alternate SVG text element for viewers that are not
             able to display embedded XHTML/MathML */
          elType.ElSSchema = svgSchema;
          elType.ElTypeNum = SVG_EL_text_;
          altText = TtaNewElement (doc, elType);
          TtaInsertSibling (altText, foreignObj, FALSE, doc);
          elType.ElTypeNum = SVG_EL_TEXT_UNIT;
          leaf = TtaNewElement (doc, elType);
          TtaInsertFirstChild (&leaf, altText, doc);
          lang = TtaGetLanguageIdFromScript('L');
          if(entry == 9)
            TtaSetTextContent (leaf,
                               (unsigned char *)"embedded XHTML not supported" ,
                               lang, doc);
          else
            TtaSetTextContent (leaf,
                               (unsigned char *)"embedded MathML not supported" ,
                               lang, doc);


          /* set the visibility of the alternate text */
          EvaluateTestAttrs (switch_, doc);
        }
      else if (entry == 10)
        /* creation of a TEXT leaf */
        {
          created = TRUE;
	  
          childType.ElSSchema = svgSchema;
          childType.ElTypeNum = SVG_EL_TEXT_UNIT;
          child = TtaNewElement (doc, childType);
          TtaInsertFirstChild (&child, newEl, doc);
          selEl = child;
          if (isFormattedView)
            {
              /* Ask where the user wants to insert the text */
              AskSurroundingBox(doc, svgAncestor, svgCanvas,
                                entry, &x1, &y1, &x2, &y2,
                                &x3, &y3, &x4, &y4,
                                &lx, &ly);

              attrType.AttrTypeNum = SVG_ATTR_x;
              UpdateAttrText (newEl, doc, attrType, x1, FALSE, TRUE);

              attrType.AttrTypeNum = SVG_ATTR_y;
              UpdateAttrText (newEl, doc, attrType, y1, FALSE, TRUE);
            }
        }

      if (created)
        {
          if (replaceGraph)
            {
              TtaRegisterElementDelete (sibling, doc);
              TtaDeleteTree(sibling, doc);
            }
          if (newSVG)
            TtaRegisterElementCreate (svgCanvas, doc);
          else
            TtaRegisterElementCreate (newEl, doc);
        }
      else
        {
          TtaDeleteTree(newEl, doc);
          newEl = NULL;
        }

      if (entry == 56)
        {
          /* Creation of a MathML Foreign object: go back to the Mathedit
             module */
          /* ask Thot to display changes made in the document */
          TtaSetDisplayMode (doc, dispMode);
          TtaSelectElement (doc, foreignObj);
          TtaCloseUndoSequence (doc);
          return;
        }
    }

  /* create attributes fill and stroke */
  if (entry != -1 && created && 
      (newType.ElTypeNum == SVG_EL_g ||
       newType.ElTypeNum == SVG_EL_line_ ||
       newType.ElTypeNum == SVG_EL_rect ||
       newType.ElTypeNum == SVG_EL_circle_ ||
       newType.ElTypeNum == SVG_EL_ellipse ||
       newType.ElTypeNum == SVG_EL_polyline ||
       newType.ElTypeNum == SVG_EL_polygon ||
       newType.ElTypeNum == SVG_EL_path))
    {
      // force the fill of known shapes
      *buffer = EOS;
      if (entry == 0 || (entry >= 12 && entry <= 14) || (entry >= 5 && entry <= 8))
        {
          /* don't fill during the creation */
          strcpy (buffer2, "stroke:black; fill:none");
          ParseHTMLSpecificStyle (newEl, buffer2, doc, 0, FALSE);

          /* Use a different shape to ask points of the polyline/curve */
          if (entry >= 12 && entry <= 14)
            shape = 0;
          else
            shape = entry;
          created = AskShapePoints (doc, svgAncestor, svgCanvas, shape, newEl);
          if (created)
            {
              if (LastSVGelementIsFilled)
                {
                  strcpy (buffer2, "fill:#9dc2de");
                  ParseHTMLSpecificStyle (newEl, buffer2, doc, 0, FALSE);
                }
              if (entry == 12 || entry == 14)
                AttachMarker (doc, newEl, SVG_ATTR_marker_start, Arrow1Mstart_id,
                              svgCanvas, svgSchema);
              if (entry == 13 || entry == 14)
                AttachMarker (doc, newEl, SVG_ATTR_marker_end, Arrow1Mend_id,
                              svgCanvas, svgSchema);
              UpdatePointsOrPathAttribute (doc, newEl, 0, 0, TRUE);
              SVGElementComplete (&context, newEl, &error);
              //UpdateMarkers(newEl, doc);
            }
          else
            {
              /* Actually, the user don't create the shape */
              TtaRegisterElementDelete (newEl, doc);
              TtaDeleteTree(newEl, doc);
              newEl = NULL;
              TtaSelectElement(doc, sibling);
            }
        }
      
      if (created)
        {
          sprintf (buffer,  "stroke: black; stroke-opacity: 1; stroke-width: 1; ");
          if (LastSVGelementIsFilled)
            strcat (buffer, "fill: #9dc2de");
          else if(entry != 0)
            strcat (buffer, "fill:none");
          
          ParseHTMLSpecificStyle (newEl, buffer, doc, 0, FALSE);
          attrType.AttrTypeNum = SVG_ATTR_style_;
          attr = TtaNewAttribute (attrType);
          TtaAttachAttribute (newEl, attr, doc);
          TtaSetAttributeText (attr, buffer, newEl, doc);
          //  Update the style panel to display current properties
          UpdateStylePanelSVG (doc, 1, newEl);
        }
    }
  /* ask Thot to display changes made in the document */
  TtaSetDisplayMode (doc, dispMode);

  if (selEl != NULL)
    /* select the right element */
    TtaSelectElement (doc, selEl);

  /* adapt the size of the SVG root element if necessary */
  /* CheckSVGRoot (doc, newEl);
     SetGraphicDepths (doc, svgCanvas);*/

  TtaCloseUndoSequence (doc);
  TtaSetDocumentModified (doc);
#endif /* _SVG */
}

/*----------------------------------------------------------------------
  SelectGraphicElement
  ----------------------------------------------------------------------*/
void SelectGraphicElement (Document doc, View view)
{
#ifdef _SVG
  Element    svgAncestor, svgCanvas;
  Element	   first, sibling, parent;
  int		     c1, c2;
  int        x1, y1, x2, y2, x3, y3, x4, y4, lx, ly;
  float      xmin, xmax, ymin, ymax;
  float      x, y, width, height;
  ThotBool  IsFirst;

  /* Check that a document is selected */
  if (doc == 0)
    return;
  /* Check that whether we are in formatted view. */
  if (view != 1 )
    return;
  TtaGiveFirstSelectedElement (doc, &first, &c1, &c2);
  if (first)
    {
      parent = TtaGetParent (first);
      if (TtaIsReadOnly (parent))
        /* do modify read-only element */
        {
          TtaDisplaySimpleMessage (CONFIRM, LIB, TMSG_EL_RO);
          return;
        }
    }
  else
    /* no selection */
    return;

  if (!GetAncestorCanvasAndObject(doc, &first,
                                  &svgAncestor, &svgCanvas))
    return;

  TtaSelectElement(doc, svgCanvas);
  /* Ask a box surrounding the element the user wants to select */
  AskSurroundingBox(doc, svgAncestor, svgCanvas, 42,
                    &x1, &y1, &x2, &y2, &x3, &y3, &x4, &y4,
                    &lx, &ly);
  xmin = x1;
  xmax = x4;
  ymin = y1;
  ymax = y4;
  TtaUnselect(doc);

  /* Look for each child whether it is inside the box */
  for(sibling = TtaGetFirstChild(svgCanvas), IsFirst = TRUE;
      sibling;
      TtaNextSibling(&sibling)
      )
    {
      GetPositionAndSizeInParentSpace(doc, sibling, &x, &y, &width, &height);
      if (x >= xmin && x + width <= xmax &&
          y >= ymin && y + height <= ymax)
        {
          if (IsFirst)
            {
              TtaSelectElement(doc, sibling);  
              IsFirst = FALSE;
            }
          else
            TtaAddElementToSelection(doc, sibling);
        }
    }

#endif /* _SVG */
}

/*----------------------------------------------------------------------
  EditGraphicElement
  entry is the number of the entry chosen by the user in the Graphics
  palette.
  ----------------------------------------------------------------------*/
void EditGraphicElement (Document doc, View view, int entry)
{
#ifdef _SVG
  DisplayMode  dispMode;
  int		       c1, c2;
  ThotBool     done;
  Element      first, parent;
  SSchema      svgSchema;
#define MAX_TITLE 50
  char         title[MAX_TITLE+1];
  char         desc[MAX_LENGTH+1];
  char        *title_, *desc_;

  /* Check that a document is selected */
  if (doc == 0)return;
  svgSchema = GetSVGSSchema (doc);

  TtaGiveFirstSelectedElement (doc, &first, &c1, &c2);
  if (first)
    {
      parent = TtaGetParent (first);
      if (TtaIsReadOnly (parent))
        /* do not modify read-only element */
        {
          TtaDisplaySimpleMessage (CONFIRM, LIB, TMSG_EL_RO);
          return;
        }
    }
  else
    /* no selection */
    return;

  TtaOpenUndoSequence (doc, NULL, NULL, 0, 0);
  dispMode = TtaGetDisplayMode (doc);
  /* ask Thot to stop displaying changes made in the document */
  if (dispMode == DisplayImmediately)
    TtaSetDisplayMode (doc, DeferredDisplay);

  switch (entry)
    {
    case 55:
      done = GenerateDesc (doc, view, first);
      break;

    case 54:
      /* Get the current title */
      title_ = GetElementData(doc, first, svgSchema, SVG_EL_title);
      if(title_)
        {
          strncpy(title, title_, MAX_TITLE);
          title[MAX_TITLE] = EOS;
          TtaFreeMemory(title_);
        }
      else
        *title = EOS;

      /* Get the current descriptor */
      desc_ = GetElementData(doc, first, svgSchema, SVG_EL_desc);
      if(desc_)
        {
          strncpy (desc, desc_, MAX_LENGTH);
          desc[MAX_LENGTH] = EOS;
          TtaFreeMemory (desc_);
        }
      else
        *desc = EOS;

      /* Dialog box */
      done = QueryTitleAndDescFromUser(title, MAX_TITLE,
                                       desc, MAX_LENGTH);
      if(done)
        {
          SetElementData(doc, first, svgSchema, SVG_EL_desc, desc);
          SetElementData(doc, first, svgSchema, SVG_EL_title, title);
        }

      break;

    default:
      TtaDisplaySimpleMessage (CONFIRM, AMAYA, AM_NOT_AVAILABLE);
      break;
    }
  
  TtaCloseUndoSequence (doc);
  if (!done)
    /* no change done */
    TtaCancelLastRegisteredSequence (doc);
  /* ask Thot to display changes made in the document */
  TtaSetDisplayMode (doc, dispMode);
  TtaSetDocumentModified (doc);
#endif /* _SVG */
}

/*----------------------------------------------------------------------
  TransformGraphicElement
  Apply a transformation to a Graphics element.
  entry is the number of the entry chosen by the user in the Graphics
  palette.
  ----------------------------------------------------------------------*/
void TransformGraphicElement (Document doc, View view, int entry)
{
#ifdef _SVG
  Element          *selected, tmp_el;
  Element          svgAncestor, svgCanvas, group;
  Element	         first, sibling, sibling2, child, parent;
  DisplayMode      dispMode;
  int              *position = NULL;
  int              tmp_pos, nb_selected;
  int		           c1, c2, i, j = 0, k = 0;
  float            x, y, width, height;
  float            xmin, ymin, xmax, ymax, xcenter, ycenter;
  ThotBool         isFormattedView, done = TRUE;
  ThotBool         isDistribution, sortSelection;
  ElementType      elType;
  SSchema          svgSchema;

  /* Check that a document is selected */
  if (doc == 0)return;

  svgSchema = GetSVGSSchema (doc);
  /* Check that whether we are in formatted or strutured view. */
  if (view == 1) isFormattedView = TRUE;
  else if (view == 2)isFormattedView = FALSE;
  else return;

  TtaGiveFirstSelectedElement (doc, &first, &c1, &c2);
  if (first)
    {
      parent = TtaGetParent (first);
      if (TtaIsReadOnly (parent))
        /* do modify read-only element */
        {
          TtaDisplaySimpleMessage (CONFIRM, LIB, TMSG_EL_RO);
          return;
        }
    }
  else
    /* no selection */
    return;

  /* If the element is a TEXT_UNIT, get the <text/> ancestor */
  elType = TtaGetElementType(first);
  if(elType.ElSSchema == svgSchema &&
     elType.ElTypeNum == SVG_EL_TEXT_UNIT)
    {
      elType.ElTypeNum = SVG_EL_text_;
      child = TtaGetTypedAncestor(first, elType);
      if(child != NULL)
        first = child;
    }

  /* Get the <svg/> ancestor, canvas and check that the first selected 
     element is a child of canvas */
  child = first;
  if (!GetAncestorCanvasAndObject(doc, &child,
                                  &svgAncestor, &svgCanvas))
    return;
  if (child != first)
    return;

  /* Count how many children of svgCanvas are selected*/
  for (nb_selected = 0, sibling = first;
      sibling;
      TtaGiveNextSelectedElement(doc, &sibling, &c1, &c2)
      )
    {
      if (TtaGetParent(sibling) == svgCanvas)
        nb_selected++;
    }

  /* Check if there are enough elements selected */
  isDistribution = (46 <= entry && entry <= 53);
  if (nb_selected == 0 || (isDistribution && nb_selected < 3))
    return;

  /* Put all the pointers to the selected children into a table */
  selected = (Element *)(TtaGetMemory(nb_selected * sizeof(Element)));
  if (selected == NULL)
    return;

  /* For distribution, allocate a table of positions */
  if(isDistribution)
    {
      position = (int *)(TtaGetMemory(nb_selected * sizeof(int)));
      if(position == NULL)
        return;
    }

  /* For some operations, the order of the selection is important */
  sortSelection = (entry == 11 || entry == 26 ||
                   (29 <= entry && entry <= 32));

  for (i = 0, sibling = first;
       i < nb_selected;
       TtaGiveNextSelectedElement(doc, &sibling, &c1, &c2)
      )
    {
      /* Is the element a child the svgCanvas */
      if (TtaGetParent(sibling) == svgCanvas)
        {
          selected[i] = sibling;

          if (sortSelection)
            {
              /* Search where the element must be inserted */
              for (j = 0; j < i; j++)
                {
                  if(TtaIsBefore(selected[i], selected[j]))
                    break;
                }
	      
              if (j < i)
                {
                  /* selected[i] must be inserted before selected[j] */
                  tmp_el = selected[i];
                  for (k = i; k > j; k--)
                    selected[k] = selected[k-1];
                  selected[j] = tmp_el;
                }
            }

          i++;
        }
    }

  TtaOpenUndoSequence (doc, NULL, NULL, 0, 0);
  dispMode = TtaGetDisplayMode (doc);
  /* ask Thot to stop displaying changes made in the document */
  if (dispMode == DisplayImmediately)
    TtaSetDisplayMode (doc, DeferredDisplay);

  switch (entry)
    {
    case 11: /* group */
      elType.ElSSchema = svgSchema;
      elType.ElTypeNum = SVG_EL_g;
      group = TtaNewElement (doc, elType);

      /* insert the new group element */
      TtaInsertSibling (group, selected[nb_selected - 1], FALSE, doc);
      TtaRegisterElementCreate (group, doc);

      for (i = 0; i < nb_selected; i++)
        {
          TtaRegisterElementDelete (selected[i], doc);
          TtaRemoveTree (selected[i], doc);
          if(i == 0)
            TtaInsertFirstChild (&selected[i], group, doc);
          else
            TtaInsertSibling (selected[i], selected[i-1], FALSE, doc);

          TtaRegisterElementCreate (selected[i], doc);
        }
      TtaSelectElement (doc, group);
      break;
    case 26:	/* Ungroup */
      for (i = 0; i < nb_selected; i++)
        {
          Ungroup (doc, selected[i]);
        }
      break;
    case 27:   /* Flip Vertically */
    case 28:   /* Flip Horizontally */
      for (i = 0; i < nb_selected; i++)
        FlipElementInParentSpace(doc, selected[i], entry == 28);
      break;
    case 29:   /* BringToFront */
      for (i = 0, sibling = TtaGetLastChild(svgCanvas); i < nb_selected; i++)
        {
          child = selected[i];
          if (child != sibling)
            {
              TtaRegisterElementDelete (child, doc);
              TtaRemoveTree(child, doc);
              TtaInsertSibling(child, sibling, FALSE, doc);
              TtaRegisterElementCreate (child, doc);
              sibling = child;
            }
        }
      break;
    case 30:   /* BringForward */
      for(i = nb_selected - 1, sibling2 = NULL; i >= 0; i--)
        {
          child = selected[i];
          sibling = selected[i];
          TtaNextSibling(&sibling);
          
          if (sibling != sibling2)
            {
              TtaRegisterElementDelete (child, doc);
              TtaRemoveTree(child, doc);
              TtaInsertSibling(child, sibling, FALSE, doc);
              TtaRegisterElementCreate (child, doc);
            }
          sibling2 = child;
        }
      break;
    case 31:   /* SendBackward */
      for(i = 0, sibling2 = NULL; i < nb_selected; i++)
        {
          child = selected[i];
          sibling = selected[i];
          TtaPreviousSibling(&sibling);

          if (sibling != sibling2)
            {
              TtaRegisterElementDelete (child, doc);
              TtaRemoveTree(child, doc);
              TtaInsertSibling(child, sibling, TRUE, doc);
              TtaRegisterElementCreate (child, doc);
            }
          sibling2 = child;
        }
      break;
    case 32:   /* SendToBack */
      for (i = 0, sibling = TtaGetFirstChild(svgCanvas); i < nb_selected; i++)
        {
          child = selected[i];
          if (child != sibling)
            {
              TtaRegisterElementDelete (child, doc);
              TtaRemoveTree(child, doc);
              TtaInsertSibling(child, sibling, i == 0, doc);
              TtaRegisterElementCreate (child, doc);
              sibling = child;
            }
        }
      break;
    case 33:   /* RotateAntiClockWise */
    case 34:   /* RotateClockWise */
      for (i = 0; i < nb_selected; i++)
        RotateElementInParentSpace(doc, selected[i], entry == 33 ? 90 : -90);
      break;
    case 35:   /* AlignLeft */
    case 36:   /* AlignCenter */
    case 37:   /* AlignRight */
    case 38:   /* AlignTop */
    case 39:   /* AlignMiddle */
    case 40:   /* AlignBottom */
      if (nb_selected == 1)
        {
          /* Only one element is selected: do the alignment in the svgCanvas */
          TtaGiveBoxSize (svgCanvas, doc, 1, UnPixel, &c1, &c2);
          xmax = (float)c1;ymax = (float)c2;

          GetPositionAndSizeInParentSpace(doc, selected[0],
                                          &x, &y, &width, &height);
          switch(entry)
            {
            case 35:   /* AlignLeft */
              MoveElementInParentSpace(doc, selected[0], 0, y);
              break;
            case 36:   /* AlignCenter */
              MoveElementInParentSpace(doc, selected[0], ((float)(xmax-width))/2, y);
              break;
            case 37:   /* AlignRight */
              MoveElementInParentSpace(doc, selected[0], xmax-width, y);
              break;
            case 38:   /* AlignTop */
              MoveElementInParentSpace(doc, selected[0], x, 0);
              break;
            case 39:   /* AlignMiddle */
              MoveElementInParentSpace(doc, selected[0], x, ((float)(ymax-height))/2);
              break;
            case 40:   /* AlignBottom */
              MoveElementInParentSpace(doc, selected[0], x, ymax-height);
              break;
            }
	  
        }
      else
        {
          /* More than one element */
          GetPositionAndSizeInParentSpace(doc, selected[0],
                                          &x, &y, &width, &height);
          xmin = x; xmax = x + width;
          ymin = y; ymax = y + height;

          for (i = 1; i < nb_selected; i++)
            {
              GetPositionAndSizeInParentSpace(doc, selected[i],
                                              &x, &y, &width, &height);
              if (x < xmin)xmin = x;
              if (y < ymin)ymin = y;
              if (x + width > xmax)xmax = x + width;
              if (y + height > ymax)ymax = y + height;
            }

          xcenter = (xmin+xmax)/2;
          ycenter = (ymin+ymax)/2;
          for (i = 0; i < nb_selected; i++)
            {
              GetPositionAndSizeInParentSpace(doc, selected[i],
                                              &x, &y, &width, &height);
              switch(entry)
                {
                case 35:   /* AlignLeft */
                  MoveElementInParentSpace(doc, selected[i], xmin, y);
                  break;
                case 36:   /* AlignCenter */
                  MoveElementInParentSpace(doc, selected[i],
                                           xcenter - ((float)width)/2, y);
                  break;
                case 37:   /* AlignRight */
                  MoveElementInParentSpace(doc, selected[i], xmax - width, y);
                  break;
                case 38:   /* AlignTop */
                  MoveElementInParentSpace(doc, selected[i], x, ymin);
                  break;
                case 39:   /* AlignMiddle */
                  MoveElementInParentSpace(doc, selected[i], x,
                                           ycenter - ((float)height)/2);
                  break;
                case 40:   /* AlignBottom */
                  MoveElementInParentSpace(doc, selected[i], x, ymax - height);
                  break;
                }
            }
        }
      break;
    case 41:   /* Rotate */
      if (isFormattedView)
        done = AskTransform(doc, svgAncestor, svgCanvas, 2, selected[0]);
      break;
    case 43:   /* Skew */
      if (isFormattedView)
        done = AskTransform(doc, svgAncestor, svgCanvas, 4, selected[0]);
      break;
    case 44:   /* Scale */
      if (isFormattedView)
        done = AskTransform(doc, svgAncestor, svgCanvas, 1, selected[0]);
      break;
    case 45: /* Translate */
      if (isFormattedView)
        done = AskTransform(doc, svgAncestor, svgCanvas, 17, selected[0]);
      break;
    case 46:   /* DistributeLeft */
    case 47:   /* DistributeCenter */
    case 48:   /* DistributeRight */
    case 49:   /* DistributeTop */
    case 50:   /* DistributeMiddle */
    case 51:   /* DistributeBottom */
    case 52:   /* DistributeHorizontalSpacing */
    case 53:   /* DistributeVerticalSpacing */
      /* Get positions of selected elements and sort them */
      for(i = 0; i < nb_selected; i++)
        {
          GetPositionAndSizeInParentSpace(doc, selected[i],
                                          &x, &y, &width, &height);
          switch(entry)
            {
            case 46:   /* DistributeLeft */
              position[i] = (int)x;
              break;
            case 47:   /* DistributeCenter */
            case 52:
              position[i] = (int)(x+width/2);
              break;
            case 48:   /* DistributeRight */
              position[i] = (int)(x+width);
              break;
            case 49:   /* DistributeTop */
              position[i] = (int)y;
              break;
            case 50:   /* DistributeMiddle */
            case 53:
              position[i] = (int)(y+height/2);
              break;
            case 51:   /* DistributeBottom */
              position[i] = (int)(y+height);
              break;
            }

          /* Search where the element must be inserted */
          for(j = 0; j < i; j++)
            {
              if(position[i] < position[j])
                break;
            }

          if(j < i)
            {
              /* selected[i] must be inserted before selected[j] */
              tmp_el = selected[i];
              tmp_pos = position[i];
              for(k = i; k > j; k--)
                {
                  selected[k] = selected[k-1];
                  position[k] = position[k-1];
                }
              selected[j] = tmp_el;
              position[j] = tmp_pos;
            }
        }

      /* Now we can distribute the elements
      We are going to compute:
      j = left side
      k = (n-1)*delta
      Where delta is the distance we want to be constant.
      */
      if(entry == 52 || entry == 53)
        {
          /* It's a distribution according to space */
          for(i = 0; i < nb_selected; i++)
            {
              GetPositionAndSizeInParentSpace(doc, selected[i],
                                              &x, &y, &width, &height);

              switch(entry)
                {
                case 52:
                  if(i == 0) { k = (int)(-x); j = (int)x; }
                  else if(i == nb_selected - 1)k+=(int)(x+width);

                  k-=(int)width;
                  break;

                case 53:
                  if(i == 0) { k = (int)(-y); j = (int)y; }
                  else if(i == nb_selected - 1)k+=(int)(y+height);

                  k-=(int)height;
                  break;
                }
            }
        }
      else
        {
          /* It's a distribution according to position. */
          j = position[0];
          k = position[nb_selected - 1] - j;
        }

      for(i = 0; i < nb_selected; i++)
        {
          GetPositionAndSizeInParentSpace(doc, selected[i],
                                          &x, &y, &width, &height);
          switch(entry)
            {
            case 52:   /* DistributeHorizontalSpacing */
            case 46:   /* DistributeLeft */
              MoveElementInParentSpace(doc, selected[i],
                                       j + ((float)(i*k))/(nb_selected-1),
                                       y);
              break;
            case 47:   /* DistributeCenter */
              MoveElementInParentSpace(doc, selected[i],
                                       j + ((float)(i*k))/(nb_selected-1)
                                       - ((float)width)/2,
                                       y);
              break;
            case 48:   /* DistributeRight */
              MoveElementInParentSpace(doc, selected[i],
                                       j + ((float)(i*k))/(nb_selected-1)
                                       - width,
                                       y);
              break;
            case 53: /* DistributeVerticalSpacing */
            case 49:   /* DistributeTop */
              MoveElementInParentSpace(doc, selected[i],
                                       x,
                                       j + ((float)(i*k))/(nb_selected-1));
              break;
            case 50:   /* DistributeMiddle */
              MoveElementInParentSpace(doc, selected[i],
                                       x,
                                       j + ((float)(i*k))/(nb_selected-1)
                                       - ((float)height/2));
              break;
            case 51:   /* DistributeBottom */
              MoveElementInParentSpace(doc, selected[i],
                                       x,
                                       j + ((float)(i*k))/(nb_selected-1)
                                       - height);
              break;
            }

          /* For distribution according to space, update the origin */
          if(entry == 52)j+=(int)width;
          else if(entry == 53)j+=(int)height;
        }
      break;
    }
  
  TtaFreeMemory(selected);
  if(isDistribution)
    TtaFreeMemory(position);

  TtaCloseUndoSequence (doc);
  if (!done)
    /* no change done */
    TtaCancelLastRegisteredSequence (doc);
  /* ask Thot to display changes made in the document */
  TtaSetDisplayMode (doc, dispMode);
  TtaSetDocumentModified (doc);
#endif /* _SVG */
}

/*----------------------------------------------------------------------
  UpdateTransformMatrix
  ----------------------------------------------------------------------*/
void UpdateTransformMatrix (Document doc, Element el)
{
  char         *buffer;
  Attribute     attr;
  AttributeType attrType;
  ThotBool      new_, open;
  SSchema      svgSchema = GetSVGSSchema (doc);

  /* Get the new attribute value */
  buffer = TtaGetTransformAttributeValue(doc, el);

  /* Check if the attribute already exists */
  attrType.AttrSSchema = svgSchema;
  attrType.AttrTypeNum = SVG_ATTR_transform;
  attr = TtaGetAttribute (el, attrType);

  /* check if the undo sequence is open */
  open = !TtaHasUndoSequence (doc);
  if (open)
    TtaOpenUndoSequence (doc, NULL, NULL, 0, 0);
  if (buffer == NULL)
    {
      if (attr)
        {
          /* Remove the current transform attribute */
          TtaRegisterAttributeDelete (attr, el, doc);
          TtaRemoveAttribute (el, attr, doc);
        }
    }
  else
    { 
      new_ = (attr == NULL);
      if (new_)
        {
          attr = TtaNewAttribute (attrType);
          TtaAttachAttribute (el, attr, doc);
        }
      else
        TtaRegisterAttributeReplace (attr, el, doc);
      TtaSetAttributeText (attr, buffer, el, doc);
      if (new_)
        TtaRegisterAttributeCreate (attr, el, doc);
      TtaFreeMemory(buffer);
    }

  /* Update the attribute menu */
  TtaUpdateAttrMenu(doc);
  if (open)
    TtaCloseUndoSequence (doc);
}

/*----------------------------------------------------------------------
  UpdateSVGElement updates the element after a move of dx,dy and a
  resize of dw,dh.
  -----------------------------------------------------------------------*/
void UpdateSVGElement (Document doc, Element el, int oldw, int oldh,
                          int dx, int dy, int dw, int dh)
{
  char         *value = NULL, *text = NULL, *ptr;
  char          buff[10], command, shape;
  Attribute     attr;
  AttributeType attrType;
  ElementType   elType;
  Element       child;
  float         ratiow, ratioh;
  int           x, y, x1, y1, x2, y2, len, i = 0;
  int           rotation, largeArcFlag, sweepFlag;
  ThotBool      error = FALSE, relative;

  if (el == NULL)
    return;

  elType = TtaGetElementType (el);
  attrType.AttrSSchema = elType.ElSSchema;
  if (strcmp (TtaGetSSchemaName (elType.ElSSchema), "SVG"))
    return;
  else
    {
      attrType.AttrTypeNum = SVG_ATTR_transform;
      attr = TtaGetAttribute (el, attrType);
      if (attr)
        {
          // update the current transform attribute
          ParseTransformAttribute (attr, el, doc, FALSE, FALSE);
          return;
        }
    }

  // ratio
  if (oldw)
    ratiow = (float)dw / (float)oldw;
  else
    ratiow = 0;
  if (oldh)
    ratioh = (float)dh / (float)oldh;
  else
    ratioh = 0;

  if (elType.ElTypeNum == SVG_EL_g || elType.ElTypeNum == SVG_EL_SVG)
    {
      child = TtaGetFirstChild(el);
      while (child)
        {
          UpdateSVGElement (doc, child, oldw, oldh, dx, dy, dw, dh);
          TtaNextSibling (&child);
        }
      return;
    }
  else if (elType.ElTypeNum == SVG_EL_path)
    {
      /* It's a Path */
      attrType.AttrTypeNum = SVG_ATTR_d;
      attr = TtaGetAttribute (el, attrType);
      len = TtaGetTextAttributeLength (attr) + 1;
      value = (char *)TtaGetMemory (len);
      TtaGiveTextAttributeValue (attr, value, &len);
      len *= 2;
      text = (char *)TtaGetMemory (len);
      memset (text, 0 ,len);
      ptr = value;
      while (*ptr != EOS && !error)
        {
          relative = TRUE;
          command = *ptr;
          ptr++;
          x = y = x1 = y1 = x2 = y2 = 0;
          ptr = (char*)TtaSkipBlanks (ptr);
          switch (command)
            {
            case 'M':
            case 'L':
            case 'T':
              relative = FALSE;
            case 'm':
              /* moveto */
            case 'l':
              /* lineto */
            case 't':
              /* smooth quadratic Bezier curveto */
              ptr = SVG_GetNumber (ptr, &x, &error);
              if (!error)
                ptr = SVG_GetNumber (ptr, &y, &error);
              break;

            case 'Z':
            case 'z':
              /* close path */
              break;

            case 'H':
              relative = FALSE;
            case 'h':
              /* horizontal lineto */
              ptr = SVG_GetNumber (ptr, &x, &error);
              break;

            case 'V':
              relative = FALSE;
            case 'v':
              /* vertical lineto */
              ptr = SVG_GetNumber (ptr, &y, &error);
              break;

            case 'C':
              relative = FALSE;
            case 'c':
              /* curveto */
              ptr = SVG_GetNumber (ptr, &x, &error);
              if (!error)
                ptr = SVG_GetNumber (ptr, &y, &error);
              if (!error)
                ptr = SVG_GetNumber (ptr, &x1, &error);
              if (!error)
                ptr = SVG_GetNumber (ptr, &y1, &error);
              if (!error)
                ptr = SVG_GetNumber (ptr, &x2, &error);
              if (!error)
                ptr = SVG_GetNumber (ptr, &y2, &error);
              break;

            case 'S':
            case 'Q':
              relative = FALSE;
            case 's':
              /* smooth curveto */
            case 'q':
              /* quadratic Bezier curveto */
              ptr = SVG_GetNumber (ptr, &x, &error);
              if (!error)
                ptr = SVG_GetNumber (ptr, &y, &error);
              if (!error)
                ptr = SVG_GetNumber (ptr, &x1, &error);
              if (!error)
                ptr = SVG_GetNumber (ptr, &y1, &error);
              break;

            case 'A':
              relative = FALSE;
            case 'a':
              /* elliptical arc */
              ptr = SVG_GetNumber (ptr, &x, &error);    /* must be non-negative */
              if (x < 0)
                error = TRUE;
              if (!error)
                ptr = SVG_GetNumber (ptr, &y, &error);  /* must be non-negative */
              if (y < 0)
                error = TRUE;
              if (!error)
                ptr = SVG_GetNumber (ptr, &rotation, &error);
              if (!error)
                ptr = SVG_GetNumber (ptr, &largeArcFlag, &error);   /* must be "0" or "1" */
              if (largeArcFlag != 0 && largeArcFlag != 1)
                error = TRUE;
              if (!error)
                ptr = SVG_GetNumber (ptr, &sweepFlag, &error); /* must be "0" or "1" */
              if (sweepFlag != 0 && sweepFlag != 1)
                error = TRUE;
              if (!error)
                ptr = SVG_GetNumber (ptr, &x1, &error);
              if (!error)
                ptr = SVG_GetNumber (ptr, &y1, &error);
               break;

            default:
              /* unknown command. error. stop parsing. */
              ptr = SVG_GetNumber (ptr, &x, &error);
              if (!error)
                ptr = SVG_GetNumber (ptr, &y, &error);              
              x += (int)(ratiow * x);
              y += (int)(ratioh * y);
              if (!relative)
                {
                  x += dx;
                  y += dy;
                }
                sprintf (&text[i], "%d,%d", x, y);
              break;
              }

          if (!error)
            {
              x += (int)(ratiow * x);
              y += (int)(ratioh * y);
              if (!relative)
                {
                  x += dx;
                  y += dy;
                }
              // todo check the length of text
              if (command == 'Z' || command == 'z')
                sprintf (&text[i], "%c", command);
              else if (command == 'H' || command == 'h')
                sprintf (&text[i], "%c %d", command, x);
              else if (command == 'V' || command == 'v')
                sprintf (&text[i], "%c %d", command, y);
              else
                sprintf (&text[i], "%c %d,%d", command, x, y);
              i += strlen (&text[i]);
              if (command == 'A' || command == 'a')
                {
                  sprintf (&text[i], " %d %d %d ", rotation, largeArcFlag, sweepFlag);
                  i += strlen (&text[i]);
                }
              if (command == 'C' || command == 'c' ||
                  command == 'S' || command == 's' ||
                  command == 'Q' || command == 'q'||
                  command == 'A' || command == 'a')
                {
                  x1 += (int)(ratiow * x1);
                  y1 += (int)(ratioh * y1);
                  if (!relative)
                    {
                      x1 += dx; y1 += dy;
                    }
                  sprintf (&text[i], " %d,%d", x1, y1);
                  i += strlen (&text[i]);
                }
              if (command == 'C' || command == 'c')
                {
                  x2 += (int)(ratiow * x2);
                  y2 += (int)(ratioh * y2);
                  if (!relative)
                    {
                      x1 += dx; y1 += dy;
                      x2 += dx; y2 += dy;
                    }
                  sprintf (&text[i], " %d,%d", x2, y2);
                  i += strlen (&text[i]);
                }
              if (*ptr  == ',')
                ptr++;
              ptr = (char*)TtaSkipBlanks (ptr);
              if (*ptr != EOS)
                text[i++] = SPACE;
            }
        }
      
      if (!error)
        {
          TtaSetAttributeText (attr, text, el, doc);
          TtaRegisterAttributeReplace (attr, el, doc);
          TtaRemovePathData (doc, el);
          ParsePathDataAttribute (attr, el, doc, TRUE);
        }
#ifdef AMYA_DEBUG
printf ("------------->UpdateSVGElement\nold=\"%s\"\nnew=\"%s\"\n",value,text);
#endif
      TtaFreeMemory (text);
      TtaFreeMemory (value);
    }
  else if (elType.ElTypeNum == SVG_EL_line_)
    {
      attrType.AttrTypeNum = SVG_ATTR_x1;
      len = 10;
      attr = TtaGetAttribute (el, attrType);
      TtaGiveTextAttributeValue (attr, buff, &len);
      sscanf (buff, "%d", &x);
      sprintf (buff, "%d", (int)(ratiow * x) + dx);
      TtaSetAttributeText (attr, buff, el, doc);
      ParseCoordAttribute (attr, el, doc);

      attrType.AttrTypeNum = SVG_ATTR_y1;
      len = 10;
      attr = TtaGetAttribute (el, attrType);
      TtaGiveTextAttributeValue (attr, buff, &len);
      sscanf (buff, "%d", &y);
      sprintf (buff, "%d", (int)(ratioh * y) + dy);
      TtaSetAttributeText (attr, buff, el, doc);
      ParseCoordAttribute (attr, el, doc);

      attrType.AttrTypeNum = SVG_ATTR_x2;
      len = 10;
      attr = TtaGetAttribute (el, attrType);
      TtaGiveTextAttributeValue (attr, buff, &len);
      sscanf (buff, "%d", &x);
      sprintf (buff, "%d", (int)(ratiow * x) + dx);
      TtaSetAttributeText (attr, buff, el, doc);
      ParseCoordAttribute (attr, el, doc);

      attrType.AttrTypeNum = SVG_ATTR_y2;
      len = 10;
      attr = TtaGetAttribute (el, attrType);
      TtaGiveTextAttributeValue (attr, buff, &len);
      sscanf (buff, "%d", &y);
      sprintf (buff, "%d", (int)(ratioh * y) + dy);
      TtaSetAttributeText (attr, buff, el, doc);
      ParseCoordAttribute (attr, el, doc);

     value = ConvertLineAttributesToPath (el);
     ParsePointsBuffer (value, GetGraphicsUnit (el), doc);
     TtaFreeMemory (value);
    }
  else if (elType.ElTypeNum == SVG_EL_polyline ||
          elType.ElTypeNum == SVG_EL_polygon)
    {
      attrType.AttrTypeNum = SVG_ATTR_points;
      attr = TtaGetAttribute (el, attrType);
      len = TtaGetTextAttributeLength (attr) + 1;
      value = (char *)TtaGetMemory (len);
      TtaGiveTextAttributeValue (attr, value, &len);
      len *= 2;
      text = (char *)TtaGetMemory (len);
      memset (text, 0 ,len);
      ptr = value;
      while (*ptr != EOS && !error)
        {
          x = y = 0;
          ptr = SVG_GetNumber (ptr, &x, &error);
          if (*ptr == EOS)
            error = TRUE;
          else if (*ptr == ',')
            {
              ptr++;
              ptr = (char*)TtaSkipBlanks (ptr);
            }
          if (!error)
            ptr = SVG_GetNumber (ptr, &y, &error);
          if (!error)
            {
              // now convert values
              x += (int)(ratiow * x) + dx;
              y += (int)(ratioh * y) + dy;
              // todo check the length of text
              sprintf (&text[i], "%d,%d", x, y);
              i += strlen (&text[i]);
              if (*ptr  == ',')
                ptr++;
              ptr = (char*)TtaSkipBlanks (ptr);
              if (*ptr != EOS)
                text[i++] = SPACE;
            }
        }
      if (!error)
        {
          TtaRegisterAttributeReplace (attr, el, doc);
          TtaSetAttributeText (attr, text, el, doc);
          ParsePointsAttribute (attr, el, doc);
        }
#ifdef AMYA_DEBUG
printf ("------------->UpdateSVGElement\nold=\"%s\"\nnew=\"%s\"\n",value,text);
#endif
      TtaFreeMemory (text);
      TtaFreeMemory (value);
    }
  else if (elType.ElTypeNum == SVG_EL_rect ||
           elType.ElTypeNum == SVG_EL_circle_ ||
           elType.ElTypeNum == SVG_EL_ellipse ||
           elType.ElTypeNum == SVG_EL_image ||
           elType.ElTypeNum == SVG_EL_foreignObject)
    {
      attrType.AttrTypeNum = SVG_ATTR_x;
      len = 10;
      attr = TtaGetAttribute (el, attrType);
      if (attr)
        {
          TtaGiveTextAttributeValue (attr, buff, &len);
          sscanf (buff, "%d", &x);
        }
      attrType.AttrTypeNum = SVG_ATTR_y;
      len = 10;
      attr = TtaGetAttribute (el, attrType);
      if (attr)
        {
          TtaGiveTextAttributeValue (attr, buff, &len);
          sscanf (buff, "%d", &y);
        }
      if (elType.ElTypeNum != SVG_EL_image &&
          elType.ElTypeNum != SVG_EL_foreignObject)
        {
          dw = 0;
          dh = 0;
        }
      else
        {
          attrType.AttrTypeNum = SVG_ATTR_width_;
          len = 10;
          attr = TtaGetAttribute (el, attrType);
          if (attr)
            {
              TtaGiveTextAttributeValue (attr, buff, &len);
              sscanf (buff, "%d", &x2);
            }
          dw += x2;
          attrType.AttrTypeNum = SVG_ATTR_height_;
          len = 10;
          attr = TtaGetAttribute (el, attrType);
          if (attr)
            {
              TtaGiveTextAttributeValue (attr, buff, &len);
              sscanf (buff, "%d", &y2);
            }
          dh += y2;
        }
        
      if (elType.ElTypeNum == SVG_EL_circle_ ||
          elType.ElTypeNum == SVG_EL_ellipse)
        shape = 'c';
      else
        shape = 'g';
      UpdateShapeElement (doc, el, shape, x + dx, y + dy, dw, dh, -1, -1);
    }
}

/*----------------------------------------------------------------------
  UpdatePointsOrPathAttribute
  ----------------------------------------------------------------------*/
void UpdatePointsOrPathAttribute (Document doc, Element el, int w, int h,
                                  ThotBool withUndo)
{
  char         *buffer = NULL, value[20];
  Attribute     attr;
  AttributeType attrType;
  ElementType   elType;
  Element       leaf;
  int           i, v;
  SSchema       svgSchema = GetSVGSSchema (doc);
  ThotBool      isPath = FALSE, isLine = FALSE;
  ThotBool      new_, open = FALSE;

  /* Check whether the element is a Path or a polyline/polygon */
  elType = TtaGetElementType (el);
  attrType.AttrSSchema = elType.ElSSchema;
  if (elType.ElSSchema != svgSchema)
    {
      if (elType.ElTypeNum != HTML_EL_AREA)
        return;
      attrType.AttrTypeNum = HTML_ATTR_coords;
    }
  else if (elType.ElTypeNum == SVG_EL_path)
    {
    /* It's a Path */
      isPath = TRUE;
      attrType.AttrTypeNum = SVG_ATTR_d;
    }
  else if (elType.ElTypeNum == SVG_EL_line_)
    {
      isLine = TRUE;
      attrType.AttrTypeNum = SVG_ATTR_x1;
    }
  else if (elType.ElTypeNum != SVG_EL_polyline &&
          elType.ElTypeNum != SVG_EL_polygon)
    /* It's not a Polygon/polyline */
    return;
  else
    attrType.AttrTypeNum = SVG_ATTR_points;

  /* Get the attribute value from the GRAPHICS leaf */
  leaf = GetGraphicsUnit(el);
  if (isPath)
    buffer = TtaGetPathAttributeValue (leaf, w, h);
  else
    buffer = TtaGetPointsAttributeValue (leaf, w, h);

  /* Check if the attribute already exists */
  attr = TtaGetAttribute (el, attrType);
  /* check if the undo sequence is open */
  if (withUndo)
    {
      open = !TtaHasUndoSequence (doc);
      if (open)
        {
          TtaOpenUndoSequence (doc, NULL, NULL, 0, 0);
          TtaSetDocumentModified(doc);
        }
    }

  if (buffer == NULL)
    {
      if (attr && !isLine)
        {
          /* Remove the current path attribute */
          if (withUndo)
            TtaRegisterAttributeDelete (attr, el, doc);
          TtaRemoveAttribute (el, attr, doc);
        }
      return;
    }
  if (withUndo)
    TtaRegisterElementReplace (el, doc);
  withUndo = FALSE;

  if (isLine)
    {
      i = 0;
      while (isLine)
        {
          v = 0;
          while (isdigit(buffer[i]))
            value[v++] = buffer[i++];
          value[v] = EOS;
          new_ = (attr == NULL);
          if (new_)
            {
              attr = TtaNewAttribute (attrType);
              TtaAttachAttribute (el, attr, doc);
            }
          else if (withUndo)
            TtaRegisterAttributeReplace (attr, el, doc);
          
          TtaSetAttributeText (attr, value, el, doc);
          if (withUndo && new_)
            TtaRegisterAttributeCreate (attr, el, doc);
          if (attrType.AttrTypeNum == SVG_ATTR_x1)
            attrType.AttrTypeNum = SVG_ATTR_y1;
          else if (attrType.AttrTypeNum == SVG_ATTR_y1)
            attrType.AttrTypeNum = SVG_ATTR_x2;
          else if (attrType.AttrTypeNum == SVG_ATTR_x2)
            attrType.AttrTypeNum = SVG_ATTR_y2;
          else
            isLine = FALSE;
          if (isLine)
            {
              while (buffer[i] != EOS && !isdigit(buffer[i]))
                i++;
              attr = TtaGetAttribute (el, attrType);
            }
        }
    }
  else
    {
      new_ = (attr == NULL);
      if (new_)
        {
          attr = TtaNewAttribute (attrType);
          TtaAttachAttribute (el, attr, doc);
        }
      else if (withUndo)
        TtaRegisterAttributeReplace (attr, el, doc);
      TtaSetAttributeText (attr, buffer, el, doc);
      if (withUndo && new_)
        TtaRegisterAttributeCreate (attr, el, doc);
    }
 
  TtaFreeMemory(buffer);

  /* Update the attribute menu */
  TtaUpdateAttrMenu(doc);
  if (withUndo && open)
    {
      TtaCloseUndoSequence (doc);
      TtaSetDocumentUnmodified(doc);
    }
}

/*----------------------------------------------------------------------
  UpdateShapeElement updates one or several shape parameters.
  A negative value is not updated
  ----------------------------------------------------------------------*/
void UpdateShapeElement (Document doc, Element el, char shape,
                         int x, int y, int width, int height,
                         int rx, int ry)
{
  SSchema       svgSchema = GetSVGSSchema (doc);
  AttributeType attrType;
  DisplayMode   dispMode;
  ThotBool      open;

  attrType.AttrSSchema = svgSchema;
  dispMode = TtaGetDisplayMode (doc);
  /* ask Thot to stop displaying changes made to the document*/
  if (dispMode == DisplayImmediately)
    TtaSetDisplayMode (doc, DeferredDisplay);

  /* check if the undo sequence is open */
  open = !TtaHasUndoSequence (doc);
  if (open)
    TtaOpenUndoSequence (doc, NULL, NULL, 0, 0);
  if (rx >= 0)
    {
      attrType.AttrTypeNum = SVG_ATTR_rx;
      UpdateAttrText (el, doc,  attrType, rx, FALSE, TRUE);
    }
  if (ry >= 0)
    {
      attrType.AttrTypeNum = SVG_ATTR_ry;
      UpdateAttrText (el, doc,  attrType, ry, FALSE, TRUE);
    }
  switch (shape)
    {
    case 1:
    case 'C':
    case 7:
    case 8:
      if (width >= 0)
        UpdateWidthHeightAttribute (el, doc, width, TRUE);
      if (height >= 0)
        UpdateWidthHeightAttribute (el, doc, height, FALSE);
      if (x >= 0)
        UpdatePositionAttribute (el, doc, x, TRUE);
      if (y >= 0)
        UpdatePositionAttribute (el, doc, y, FALSE);
      break;
    case 'a': /* circle */
    case 'c': /* ellipse */
      if (width >= 0)
        {
          if (x != -1)
            {
              x += (width/2);
              UpdatePositionAttribute (el, doc, x, TRUE);
            }
          UpdateWidthHeightAttribute (el, doc, width, TRUE);
        }
      if (height >= 0)
        {
          if (y != -1)
            {
              y += (height/2);
              UpdatePositionAttribute (el, doc, y, FALSE);
            }
          UpdateWidthHeightAttribute (el, doc, height, FALSE);
        }
      break;
    default:
      /* triangles trapezium*/
      UpdateTransformMatrix (doc, el);
      if (width >= 0 && height >= 0)
        UpdatePointsOrPathAttribute(doc, el, width, height, TRUE);
      break;
    }

  /* Update the attribute menu */
  TtaUpdateAttrMenu(doc);
  if (open)
    TtaCloseUndoSequence (doc);
  TtaSetDisplayMode (doc, dispMode);
}

/*----------------------------------------------------------------------
  RotateElementInParentSpace
  ----------------------------------------------------------------------*/
void RotateElementInParentSpace(Document doc, Element el, float theta)
{
  float X, Y, width, height, cx, cy, cost, sint;

  /* Get the coordinates in Parent Space */
  GetPositionAndSizeInParentSpace(doc, el, &X, &Y, &width, &height);
  cx = X + width/2;
  cy = Y + height/2;

  /* Apply a rotation */
  theta *= ((float)M_PI)/180;
  cost = cos(theta);
  sint = sin(theta);
  TtaApplyMatrixTransform (doc, el, 1, 0, 0, 1, -cx, -cy);
  TtaApplyMatrixTransform (doc, el, cost, -sint, sint, cost, 0, 0);
  TtaApplyMatrixTransform (doc, el, 1, 0, 0, 1, +cx, +cy);

  /* Update the attribute */
  UpdateTransformMatrix(doc, el);
}

/*----------------------------------------------------------------------
  FlipElementInParentSpace
  According to the parameter horiz, flip the element el vertically or
  horizontally, using the system of coordinates of its parent.
  ----------------------------------------------------------------------*/
void FlipElementInParentSpace(Document doc, Element el, ThotBool horiz)
{
  float X, Y, width, height, cx, cy;

  /* Get the coordinates in Parent Space */
  GetPositionAndSizeInParentSpace(doc, el, &X, &Y, &width, &height);
  cx = X + width/2;
  cy = Y + height/2;

  /* Apply flip an get the new transform matrix */
  TtaApplyMatrixTransform (doc, el, 1, 0, 0, 1, -cx, -cy);
  
  if (horiz)
    TtaApplyMatrixTransform (doc, el, 1, 0, 0, -1, 0, 0);
  else
    TtaApplyMatrixTransform (doc, el, -1, 0, 0, 1, 0, 0);

  TtaApplyMatrixTransform (doc, el, 1, 0, 0, 1, +cx, +cy);

  /* Update the attribute */
  UpdateTransformMatrix(doc, el);
}

/*----------------------------------------------------------------------
  MoveElementInParentSpace
  Move the element el at the position (x,y) in the system of coordinates
  of its parent.
  ----------------------------------------------------------------------*/
void MoveElementInParentSpace(Document doc, Element el, float x, float y)
{
  float X, Y, width, height;

  GetPositionAndSizeInParentSpace(doc, el, &X, &Y, &width, &height);
  
  /* Apply translation an get the new transform matrix */
  TtaApplyMatrixTransform (doc, el, 1, 0, 0, 1, x - X, y - Y);

  /* Update the attribute */
  UpdateTransformMatrix(doc, el);
}

/*----------------------------------------------------------------------
  GetPositionAndSizeInParentSpace returns the relative position in the
  parent element and the size.
----------------------------------------------------------------------*/
void GetPositionAndSizeInParentSpace (Document doc, Element el, float *X,
                                      float *Y, float *width, float *height)
{ 
#ifdef _SVG
  Element	         parent;
  ElementType      elType;
  int              dummy1,dummy2;

  if (!el)
    return;

  /* Check whether the parent is an SVG element */
  parent = TtaGetParent(el);
  elType = TtaGetElementType (parent);
  if (elType.ElSSchema == NULL || strcmp (TtaGetSSchemaName (elType.ElSSchema), "SVG"))
    return;

  /* Get the position and size of the box
   * X,Y---width-----
   *  |             |
   * height         |
   *  |             |
   *  ---------------
  */

  TtaGiveBoxPosition (el, doc, 1, UnPixel, TRUE, &dummy1, &dummy2);
  *X = (float)(dummy1);
  *Y = (float)(dummy2);

  TtaGiveBoxSize (el, doc, 1, UnPixel, &dummy1, &dummy2);
  *width = (float)(dummy1);
  *height = (float)(dummy2);
#endif /* _SVG */
}

/*----------------------------------------------------------------------
  CreateGroup
  Create a g element surrounding the selected elements
  ----------------------------------------------------------------------*/
void CreateGroup ()
{
#ifdef _SVG
  Document	doc;
  Element	el, prevSel, prevChild, group, parent;
  ElementType	elType;
  AttributeType	attrType;
  int		c1, i;
  DisplayMode	dispMode;
  ThotBool	position;
  SSchema       docSchema, svgSchema;

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
  svgSchema = GetSVGSSchema (doc);
  attrType.AttrSSchema = svgSchema;
  elType = TtaGetElementType (el);
  if (!(elType.ElTypeNum == SVG_EL_SVG &&
        elType.ElSSchema == svgSchema))
    {

      elType.ElTypeNum = SVG_EL_SVG;
      elType.ElSSchema = svgSchema;
      parent = TtaGetTypedAncestor (el, elType);
      if (parent)
        {
          /* Create a Group element */
          elType = TtaGetElementType (el);
          elType.ElTypeNum = SVG_EL_g;
          group = TtaNewElement (doc, elType);

          /* insert the new group element */
          TtaInsertSibling (group, el, TRUE, doc);
          TtaRegisterElementCreate (group, doc);

          attrType.AttrSSchema = elType.ElSSchema;
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

  TtaSetDocumentModified (doc);
#endif /* _SVG */
}

/*----------------------------------------------------------------------
  Ungroup
  Ungroup the elements of a g element
  ----------------------------------------------------------------------*/
void Ungroup (Document doc, Element el)
{
#ifdef _SVG
  ElementType   elType;
  SSchema       svgSchema;  
  Element child, nextchild, sibling;
  Attribute attr;
  AttributeType attrType;
  float a,b,c,d,e,f;

  svgSchema = GetSVGSSchema (doc);
  elType = TtaGetElementType (el);

  if (!(elType.ElTypeNum == SVG_EL_g &&
        elType.ElSSchema == svgSchema))
    /* The element is not a g: nothing to do */
    
    return;

  /* Check if the attribute already exists */
  attrType.AttrSSchema = elType.ElSSchema;
  attrType.AttrTypeNum = SVG_ATTR_transform;
  attr = TtaGetAttribute (el, attrType);

  /* Get the transform matrix of the g element */
  if (attr)TtaGetMatrixTransform(doc, el, &a, &b, &c, &d, &e, &f);

  child = TtaGetFirstChild(el);
  sibling = el;
  while(child)
    {
      nextchild = child;
      TtaNextSibling(&nextchild);

      elType = TtaGetElementType(child);
      if (!(elType.ElTypeNum == SVG_EL_title ||
            elType.ElTypeNum == SVG_EL_desc))
        {
          /* Move the child */
          TtaRegisterElementDelete (child, doc);
          TtaRemoveTree(child, doc);
          TtaInsertSibling(child, sibling, FALSE, doc);
          TtaRegisterElementCreate (child, doc);

          if (attr)
            {
              /* Apply the transformation matrix to the children */
              TtaApplyMatrixTransform (doc, child, a, b, c, d, e, f);
              UpdateTransformMatrix(doc, child);
            }

          sibling = child;

        }
      child = nextchild;
    }

  TtaRegisterElementDelete (el, doc);
  TtaDeleteTree(el, doc);
#endif /* _SVG */
}

#ifdef _SVG
/*----------------------------------------------------------------------
  CallbackGraph: manage Graph dialogue events.
  ----------------------------------------------------------------------*/
static void CallbackGraph (int ref, int typedata, char *data)
{
  long int           val = (long int) data;

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
  // free saved defs elements
  ClearSVGDefs ();
#endif /* _SVG */
}

/*----------------------------------------------------------------------
  InitSVG initializes SVG context.
  ----------------------------------------------------------------------*/
void InitSVG ()
{
#ifdef _SVG
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
  ElementType	  elType, parentType;
  Element       parent, el;
  AttributeType	attrType;
  Attribute	    attr;
  int           w = 0 , h = 0, doc;
  char          text[20];

  el = event->element;
  doc = event->document;
  elType = TtaGetElementType (el);
  /* Set the namespace declaration if the parent element is in a different
     namespace */
  parent = TtaGetParent (el);
  if (parent)
    {
      parentType = TtaGetElementType (parent);
      if (parentType.ElSSchema != elType.ElSSchema)
        /* the parent element is not in the SVG namespace. Put a namespace
           declaration on the  <svg> element */
        {
          TtaSetUriSSchema (elType.ElSSchema, SVG_URI);
          TtaSetANamespaceDeclaration (doc, el, "svg", SVG_URI);
          TtaGiveBoxSize (parent, doc, 1, UnPixel, &w, &h);
        }
    }
  // now force a width and height attribute
  attrType.AttrSSchema = elType.ElSSchema;
  attrType.AttrTypeNum = SVG_ATTR_width_;
  attr = TtaGetAttribute (el, attrType);
  if (attr == NULL)
    {
      attr = TtaNewAttribute (attrType);
      TtaAttachAttribute (el, attr, doc);
    }
  if (w == 0)
    TtaSetAttributeText (attr, "500", el, doc);
  else
    {
      memset (text, 0, sizeof(text));
      sprintf (text, "%d", w);
      TtaSetAttributeText (attr, text, el, doc);
    }
  ParseWidthHeightAttribute (attr, el, doc, FALSE);
  attrType.AttrTypeNum = SVG_ATTR_height_;
  attr = TtaGetAttribute (el, attrType);
  if (attr == NULL)
    {
      attr = TtaNewAttribute (attrType);
      TtaAttachAttribute (el, attr, doc);
    }
  TtaSetAttributeText (attr, "300", el, doc);
  ParseWidthHeightAttribute (attr, el, doc, FALSE);
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
          TtaGiveBoxPosition (ancestor, event->document, 1, UnPixel, FALSE, &x, &y);
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

typedef struct object_
{
  Element el;
  char *title;
  float cx, cy;
  float w, h;
} object;

/*----------------------------------------------------------------------
  GetAbsolutePosition
  -----------------------------------------------------------------------*/
static const char *GetAbsolutePosition(object el, float w, float h)
{
  float px = el.cx / w;
  float py = el.cy / h;

  if(px < .3)
    {
      if(py < .3)
        return TtaGetMessage(AMAYA, AM_SVG_ABSOLUTE_TOP_LEFT);
      else if(py > .6)
        return TtaGetMessage(AMAYA, AM_SVG_ABSOLUTE_BOTTOM_LEFT);
      else
        return TtaGetMessage(AMAYA, AM_SVG_ABSOLUTE_LEFT);
    }
  else if (px > .6)
    {
      if(py < .3)
        return TtaGetMessage(AMAYA, AM_SVG_ABSOLUTE_TOP_RIGHT);
      else if(py > .6)
        return TtaGetMessage(AMAYA, AM_SVG_ABSOLUTE_BOTTOM_RIGHT);
      else
        return TtaGetMessage(AMAYA, AM_SVG_ABSOLUTE_RIGHT);
    }
  else
    {
      if(py < .3)
        return TtaGetMessage(AMAYA, AM_SVG_ABSOLUTE_TOP);
      else if(py > .6)
        return TtaGetMessage(AMAYA, AM_SVG_ABSOLUTE_BOTTOM);
      else
        return TtaGetMessage(AMAYA, AM_SVG_ABSOLUTE_CENTER);
    }

  return NULL;
}

/*----------------------------------------------------------------------
  GetRelativePosition
  -----------------------------------------------------------------------*/
static const char *GetRelativePosition(object ref, object obj)
{
  int dx, dy;

  if(ref.cx+.5*ref.w <= obj.cx-.5*obj.w)
    dx = +1;
  else if(ref.cx-.5*ref.w >= obj.cx+.5*obj.w)
    dx = -1;
  else dx = 0;

  if(ref.cy+.5*ref.h <= obj.cy-.5*obj.h)
    dy = +1;
  else if(ref.cy-.5*ref.h >= obj.cy+.5*obj.h)
    dy = -1;
  else dy = 0;

  if(dx == -1)
    {
      if(dy == -1)
        return TtaGetMessage(AMAYA, AM_SVG_RELATIVE_LEFT_ABOVE);
      else if(dy == 1)
        return TtaGetMessage(AMAYA, AM_SVG_RELATIVE_LEFT_BELOW);
      else
        return TtaGetMessage(AMAYA, AM_SVG_RELATIVE_LEFT);
    }
  else if(dx == +1)
    {
      if(dy == -1)
        return TtaGetMessage(AMAYA, AM_SVG_RELATIVE_RIGHT_ABOVE);
      else if(dy == 1)
        return TtaGetMessage(AMAYA, AM_SVG_RELATIVE_RIGHT_BELOW);
      else
        return TtaGetMessage(AMAYA, AM_SVG_RELATIVE_RIGHT);
    }
  else if(dx == 0)
    {
      if(dy == -1)
        return TtaGetMessage(AMAYA, AM_SVG_RELATIVE_ABOVE);
      else if(dy == 1)
        return TtaGetMessage(AMAYA, AM_SVG_RELATIVE_BELOW);
      else
        return TtaGetMessage(AMAYA, AM_SVG_RELATIVE_IN_FRONT_OF);
    }

  return NULL;
}

static ThotBool StrCat(char **desc, int *max_length, int *length, char *buffer)
{
  *length += strlen(buffer);
  if(*length >= *max_length)
    {
      *max_length += MAX_LENGTH;
      *desc = (char *)realloc ( *desc, (size_t)max_length );
      if(*desc == NULL)
        return FALSE;
    }
  strcat ( *desc, buffer);
  return TRUE;
}

/*----------------------------------------------------------------------
  GetDistanceBetweenCenters
  -----------------------------------------------------------------------*/
static float GetDistanceBetweenCenters(object a, object b)
{
  float dx, dy;
  dx = a.cx - b.cx;
  dy = a.cy - b.cy;
  return sqrt(dx*dx + dy*dy);
}

/*----------------------------------------------------------------------
  GenerateDesc
  ----------------------------------------------------------------------*/
ThotBool GenerateDesc (Document doc, View view, Element el)
{
  int dummy1, dummy2;
  float w, h;
  
  ElementType       elType;
  SSchema	    svgSchema = GetSVGSSchema (doc);
  Element child;
  int i, j, j_nearest, nb;
  float r_nearest, r;

  object *children;
  
  char *desc;
  int length = 0;
  int max_length = MAX_LENGTH;
  char buffer[MAX_LENGTH];
  ThotBool still;

  if(doc == 0 || el == NULL)
    return FALSE;

  elType = TtaGetElementType (el);
  if (elType.ElSSchema != svgSchema)
    return FALSE;

  /* Get the size of the element */
  TtaGiveBoxSize (el, doc, 1, UnPixel, &dummy1, &dummy2);
  w = (float)(dummy1);
  h = (float)(dummy2);

  /* Count how manu children there are */
  child = TtaGetFirstChild(el);
  nb = 0;
  while(child)
    {
      nb++;
      TtaNextSibling(&child);
    }

  if(nb == 0)return FALSE;

  desc = (char *)malloc(max_length);
  if(desc == NULL)return FALSE;
  *desc = '\0';

  /* Get information on the children */
  children = (object *)TtaGetMemory(sizeof(object) * nb);
  if(children == NULL)return FALSE;

  for(child = TtaGetFirstChild(el), i = 0;
      child;
      TtaNextSibling(&child))
    {
      children[i].title = GetElementData(doc, child, svgSchema,
                                         SVG_EL_title);
       
      if(children[i].title != NULL)
        {
          /* This child has a title, take it into account */
          children[i].el = child;
          GetPositionAndSizeInParentSpace(doc,
                                          children[i].el,
                                          &(children[i].cx),
                                          &(children[i].cy),
                                          &(children[i].w),
                                          &(children[i].h));
	   
          children[i].cx += (children[i].w/2);
          children[i].cy += (children[i].h/2);
	   
          i++;
        }      
    }
   
  nb = i;

  if(nb > 0)
    {
      sprintf(buffer, TtaGetMessage(AMAYA, AM_SVG_THERE_ARE), nb);
      still = StrCat(&desc, &max_length, &length, buffer);

      for(i = 0; i < nb && still; i++)
        {

          /* Give absolute position */
          sprintf(buffer, "[%s] %s",
                  children[i].title,
                  GetAbsolutePosition(children[i], w, h)
                  );
          still = StrCat(&desc, &max_length, &length, buffer);
          if(!still)break;
	   
          if(i > 0)
            {
              strcpy(buffer, ", ");
              still = StrCat(&desc, &max_length, &length, buffer);
              if(!still)break;
	       
              /* Search for the nearest object */
              j_nearest = 0;
              r_nearest = GetDistanceBetweenCenters(children[0], children[i]);
              for(j = 1; j < i; j++)
                {
                  r = GetDistanceBetweenCenters(children[j], children[i]);
                  if(r < r_nearest)
                    {
                      j_nearest = j;
                      r_nearest = r;
                    }
                }
	       
              /* Give the relative position */
              sprintf(buffer, "%s [%s]",
                      GetRelativePosition(children[j_nearest], children[i]),
                      children[j_nearest].title);
              still = StrCat(&desc, &max_length, &length, buffer);
              if(!still)break;
            }
	   
	   
          strcpy(buffer, ". ");
          StrCat(&desc, &max_length, &length, buffer);
        }
     

      /* Free the memory */
      for(i = 0; i < nb; i++)
        TtaFreeMemory(children[i].title);
    }
   
  TtaFreeMemory(children);

  if(nb > 0)
    SetElementData(doc, el, svgSchema, SVG_EL_desc, desc);

  free(desc);

  return (nb > 0);
}

/*----------------------------------------------------------------------
  CreateSVG_Template
  ----------------------------------------------------------------------*/
void CreateSVG_Template (Document document, View view)
{
  CreateGraphicElement (document, view, -2);
}

/*----------------------------------------------------------------------
  CreateSVG_Svg
  ----------------------------------------------------------------------*/
void CreateSVG_Svg (Document document, View view)
{
  CreateGraphicElement (document, view, -1);
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
  TransformGraphicElement (document, view, 11);
}

/*----------------------------------------------------------------------
  CreateSVG_StartArrow
  ----------------------------------------------------------------------*/
void CreateSVG_StartArrow (Document document, View view)
{
  CreateGraphicElement (document, view, 12);
}

/*----------------------------------------------------------------------
  CreateSVG_EndArrow
  ----------------------------------------------------------------------*/
void CreateSVG_EndArrow (Document document, View view)
{
  CreateGraphicElement (document, view, 13);
}

/*----------------------------------------------------------------------
  CreateSVG_DoubleArrow
  ----------------------------------------------------------------------*/
void CreateSVG_DoubleArrow (Document document, View view)
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
  TransformSVG_Ungroup
  ----------------------------------------------------------------------*/
void TransformSVG_Ungroup (Document document, View view)
{
  TransformGraphicElement (document, view, 26);
}

/*----------------------------------------------------------------------
  TransformSVG_FlipVertically
  ----------------------------------------------------------------------*/
void TransformSVG_FlipVertically (Document document, View view)
{
  TransformGraphicElement (document, view, 27);
}

/*----------------------------------------------------------------------
  TransformSVG_FlipHorizontally
  ----------------------------------------------------------------------*/
void TransformSVG_FlipHorizontally (Document document, View view)
{
  TransformGraphicElement (document, view, 28);
}
      
/*----------------------------------------------------------------------
  TransformSVG_BringToFront
  ----------------------------------------------------------------------*/
void TransformSVG_BringToFront (Document document, View view)
{
  TransformGraphicElement (document, view, 29);
}

/*----------------------------------------------------------------------
  TransformSVG_BringForward
  ----------------------------------------------------------------------*/
void TransformSVG_BringForward (Document document, View view)
{
  TransformGraphicElement (document, view, 30);
}

/*----------------------------------------------------------------------
  TransformSVG_SendBackward
  ----------------------------------------------------------------------*/
void TransformSVG_SendBackward (Document document, View view)
{
  TransformGraphicElement (document, view, 31);
}

/*----------------------------------------------------------------------
  TransformSVG_SendToBack
  ----------------------------------------------------------------------*/
void TransformSVG_SendToBack (Document document, View view)
{
  TransformGraphicElement (document, view, 32);
}

/*----------------------------------------------------------------------
  TransformSVG_RotateAntiClockWise
  ----------------------------------------------------------------------*/
void TransformSVG_AntiClockWise (Document document, View view)
{
  TransformGraphicElement (document, view, 33);
}

/*----------------------------------------------------------------------
  TransformSVG_RotateClockWise
  ----------------------------------------------------------------------*/
void TransformSVG_ClockWise (Document document, View view)
{
  TransformGraphicElement (document, view, 34);
}

/*----------------------------------------------------------------------
  TransformSVG_AlignLeft
  ----------------------------------------------------------------------*/
void TransformSVG_AlignLeft (Document document, View view)
{
  TransformGraphicElement (document, view, 35);
}

/*----------------------------------------------------------------------
  TransformSVG_AlignCenter
  ----------------------------------------------------------------------*/
void TransformSVG_AlignCenter (Document document, View view)
{
  TransformGraphicElement (document, view, 36);
}

/*----------------------------------------------------------------------
  TransformSVG_AlignRight
  ----------------------------------------------------------------------*/
void TransformSVG_AlignRight (Document document, View view)
{
  TransformGraphicElement (document, view, 37);
}

/*----------------------------------------------------------------------
  TransformSVG_AlignTop
  ----------------------------------------------------------------------*/
void TransformSVG_AlignTop (Document document, View view)
{
  TransformGraphicElement (document, view, 38);
}

/*----------------------------------------------------------------------
  TransformSVG_AlignMiddle
  ----------------------------------------------------------------------*/
void TransformSVG_AlignMiddle (Document document, View view)
{
  TransformGraphicElement (document, view, 39);
}

/*----------------------------------------------------------------------
  TransformSVG_AlignBottom
  ----------------------------------------------------------------------*/
void TransformSVG_AlignBottom (Document document, View view)
{
  TransformGraphicElement (document, view, 40);
}

/*----------------------------------------------------------------------
  TransformSVG_Rotate
  ----------------------------------------------------------------------*/
void TransformSVG_Rotate (Document document, View view)
{
  TransformGraphicElement (document, view, 41);
}

/*----------------------------------------------------------------------
  EditSVG_Select
  ----------------------------------------------------------------------*/
void EditSVG_Select (Document document, View view)
{
  SelectGraphicElement (document, view);
}


/*----------------------------------------------------------------------
  SVG_Select
  ----------------------------------------------------------------------*/
void CreateSVG_Image (Document document, View view)
{
  AddNewImage (document, view, FALSE, TRUE);
}

/*----------------------------------------------------------------------
  TransformSVG_Skew
  ----------------------------------------------------------------------*/
void TransformSVG_Skew (Document document, View view)
{
  TransformGraphicElement (document, view, 43);
}

/*----------------------------------------------------------------------
  TransformSVG_Scale
  ----------------------------------------------------------------------*/
void TransformSVG_Scale (Document document, View view)
{
  TransformGraphicElement (document, view, 44);
}

/*----------------------------------------------------------------------
  TransformSVG_Translate
  ----------------------------------------------------------------------*/
void TransformSVG_Translate (Document document, View view)
{
  TransformGraphicElement (document, view, 45);
}

/*----------------------------------------------------------------------
  TransformSVG_DistributeLeft
  ----------------------------------------------------------------------*/
void TransformSVG_DistributeLeft (Document document, View view)
{
  TransformGraphicElement (document, view, 46);
}

/*----------------------------------------------------------------------
  TransformSVG_DistributeCenter
  ----------------------------------------------------------------------*/
void TransformSVG_DistributeCenter (Document document, View view)
{
  TransformGraphicElement (document, view, 47);
}

/*----------------------------------------------------------------------
  TransformSVG_DistributeRight
  ----------------------------------------------------------------------*/
void TransformSVG_DistributeRight (Document document, View view)
{
  TransformGraphicElement (document, view, 48);
}

/*----------------------------------------------------------------------
  TransformSVG_DistributeTop
  ----------------------------------------------------------------------*/
void TransformSVG_DistributeTop (Document document, View view)
{
  TransformGraphicElement (document, view, 49);
}

/*----------------------------------------------------------------------
  TransformSVG_DistributeMiddle
  ----------------------------------------------------------------------*/
void TransformSVG_DistributeMiddle (Document document, View view)
{
  TransformGraphicElement (document, view, 50);
}

/*----------------------------------------------------------------------
  TransformSVG_DistributeBottom
  ----------------------------------------------------------------------*/
void TransformSVG_DistributeBottom (Document document, View view)
{
  TransformGraphicElement (document, view, 51);
}

/*----------------------------------------------------------------------
  TransformSVG_DistributeMiddle
  ----------------------------------------------------------------------*/
void TransformSVG_DistributeHSpacing (Document document, View view)
{
  TransformGraphicElement (document, view, 52);
}

/*----------------------------------------------------------------------
  TransformSVG_DistributeBottom
  ----------------------------------------------------------------------*/
void TransformSVG_DistributeVSpacing (Document document, View view)
{
  TransformGraphicElement (document, view, 53);
}

/*----------------------------------------------------------------------
  EditSVG_TitleAndDescription
  ----------------------------------------------------------------------*/
void EditSVG_Information (Document document, View view)
{
  EditGraphicElement (document, view, 54);
}

/*----------------------------------------------------------------------
  EditSVG_GenerateDescription
  ----------------------------------------------------------------------*/
void EditSVG_GenerateDescription (Document document, View view)
{
  EditGraphicElement (document, view, 55);
}

/* 56 = foreignObject+MathML */
