/*
 *
 *  (c) COPYRIGHT INRIA and W3C, 2000-2005
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * This module contains editing functions for handling XLink hypertext links
 *
 * Authors: V. Quint
 *          L. Carcone (namespaces)
 *
 */

/* Included headerfiles */
#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "amaya.h"
#include "XLink.h"

#include "HTMLedit_f.h"
#include "fetchXMLname_f.h"

/*----------------------------------------------------------------------
   SetXLinkTypeSimple attach an attribute xlink:type="simple" to element el
  ----------------------------------------------------------------------*/
void SetXLinkTypeSimple (Element el, Document doc, ThotBool withUndo)
{
  AttributeType	attrType;
  Attribute	attr;
  SSchema       XLinkSchema;
  ThotBool	new_;

  XLinkSchema = TtaGetSSchema ("XLink", doc);
  attrType.AttrSSchema = XLinkSchema;
  attrType.AttrTypeNum = XLink_ATTR_type;
  attr = TtaGetAttribute (el, attrType);
  if (attr == NULL)
    {
      attr = TtaNewAttribute (attrType);
      TtaAttachAttribute (el, attr, doc);
      new_ = TRUE;
    }
  else
    {
      new_ = FALSE;
      if (withUndo)
	TtaRegisterAttributeReplace (attr, el, doc);
    }
  TtaSetAttributeValue (attr, XLink_ATTR_type_VAL_simple, el, doc);
  if (new_ && withUndo)
    TtaRegisterAttributeCreate (attr, el, doc);
}

/*----------------------------------------------------------------------
 XLinkPasted
 An element from any namespace has been pasted.
 If it has some XLink attributes, update the link.
 -----------------------------------------------------------------------*/
void XLinkPasted (NotifyElement *event)
{
  Element        root;
  Document       originDocument, doc;
  ElementType	   elType;
  AttributeType  attrType;
  Attribute      attr = NULL;
  SSchema        XLinkSchema;

  doc = event->document;
  XLinkSchema = TtaGetSSchema ("XLink", doc);
  if (XLinkSchema)
    {
      /* is there an href attribute from the XLink namespace? */
      attrType.AttrSSchema = XLinkSchema;
      attrType.AttrTypeNum = XLink_ATTR_href_;
      attr = TtaGetAttribute (event->element, attrType);
      if (attr)
        {
          /* the pasted element has an href attribute */
          /* does the pasted element come from another document? */
          originDocument = (Document) event->position;
          if (originDocument >= 0 && originDocument != doc)
            {
              /* Update the value of that attribute */
              ChangeURI (event->element, attr, originDocument, doc);
            }
          /* Set the XLink namespace declaration */
          elType = TtaGetElementType (event->element);
          TtaSetUriSSchema (elType.ElSSchema, XLink_URI);
          TtaSetANamespaceDeclaration (doc, event->element, XLink_PREFIX, XLink_URI);   
        }
    }
#ifdef _SVG
  else
    {
      elType = TtaGetElementType (event->element);
      if (!strcmp (TtaGetSSchemaName (elType.ElSSchema), "SVG") &&
          (elType.ElTypeNum == SVG_EL_use_ ||
           elType.ElTypeNum == SVG_EL_a ||
           elType.ElTypeNum == SVG_EL_tref ||
           elType.ElTypeNum == SVG_EL_textPath ||
           elType.ElTypeNum == SVG_EL_altGlyph ||
           elType.ElTypeNum == SVG_EL_glyphRef ||
           elType.ElTypeNum == SVG_EL_color_profile ||
           elType.ElTypeNum == SVG_EL_linearGradient ||
           elType.ElTypeNum == SVG_EL_radialGradient ||
           elType.ElTypeNum == SVG_EL_pattern ||
           elType.ElTypeNum == SVG_EL_filter ||
           elType.ElTypeNum == SVG_EL_feImage ||
           elType.ElTypeNum == SVG_EL_cursor ||
           elType.ElTypeNum == SVG_EL_script_ ||
           elType.ElTypeNum == SVG_EL_animate ||
           elType.ElTypeNum == SVG_EL_set_ ||
           elType.ElTypeNum == SVG_EL_animateMotion ||
           elType.ElTypeNum == SVG_EL_mpath ||
           elType.ElTypeNum == SVG_EL_animateColor ||
           elType.ElTypeNum == SVG_EL_animateTransform ||
           elType.ElTypeNum == SVG_EL_image ||
           elType.ElTypeNum == SVG_EL_font_face_uri ||
           elType.ElTypeNum == SVG_EL_definition_src))
        {
          // add the xlink namespace to the root element
          XLinkSchema = GetXLinkSSchema (doc);
          root = TtaGetRootElement (doc);
          TtaSetANamespaceDeclaration (doc, root, "xlink", XLink_URI);
        }
    }
#endif /* _SVG */
}
