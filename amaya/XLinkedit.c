/*
 *
 *  (c) COPYRIGHT INRIA and W3C, 2000-2004
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
  Document       originDocument;
  ElementType	 elType;
  AttributeType  attrType;
  Attribute      attr = NULL;
  SSchema        XLinkSchema;

  XLinkSchema = TtaGetSSchema ("XLink", event->document);
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
	  if (originDocument >= 0 && originDocument != event->document)
	    {
	      /* Update the value of that attribute */
	      ChangeURI (event->element, attr, originDocument, event->document);
	    }
	  /* Set the XLink namespace declaration */
	  elType = TtaGetElementType (event->element);
	  TtaSetUriSSchema (elType.ElSSchema, XLink_URI);
	  TtaSetANamespaceDeclaration (event->document, event->element, XLink_PREFIX, XLink_URI);   
	}
    }
  
}
