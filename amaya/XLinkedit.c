/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 2000
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * This module contains editing functions for handling XLink hypertext links
 *
 * Author: V. Quint
 *
 */

/* Included headerfiles */
#define THOT_EXPORT
#include "amaya.h"
#include "XLink.h"

#include "HTMLedit_f.h"

/*----------------------------------------------------------------------
   SetXLinkTypeSimple attach an attribute xlink:type="simple" to element el
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void         SetXLinkTypeSimple (Element el, Document doc, ThotBool withUndo)
#else  /* __STDC__ */
void         SetXLinkTypeSimple (el, doc, withUndo)
Element	     el;
Document     doc;
ThotBool     withUndo;

#endif /* __STDC__ */
{
  AttributeType	attrType;
  Attribute	attr;
  SSchema       XLinkSchema;
  ThotBool	new;

  XLinkSchema = TtaGetSSchema (TEXT("XLink"), doc);
  attrType.AttrSSchema = XLinkSchema;
  attrType.AttrTypeNum = XLink_ATTR_type;
  attr = TtaGetAttribute (el, attrType);
  if (attr == NULL)
    {
      attr = TtaNewAttribute (attrType);
      TtaAttachAttribute (el, attr, doc);
      new = TRUE;
    }
  else
    {
      new = FALSE;
      if (withUndo)
	TtaRegisterAttributeReplace (attr, el, doc);
    }
  TtaSetAttributeValue (attr, XLink_ATTR_type_VAL_simple, el, doc);
  if (new && withUndo)
    TtaRegisterAttributeCreate (attr, el, doc);
}

/*----------------------------------------------------------------------
 XLinkPasted
 An element from any namespace has been pasted.
 If it has some XLink attributes, update the link.
 -----------------------------------------------------------------------*/
#ifdef __STDC__
void XLinkPasted (NotifyElement *event)
#else /* __STDC__*/
void XLinkPasted(event)
     NotifyElement *event;
#endif /* __STDC__*/
{
  Document       originDocument;
  AttributeType  attrType;
  Attribute      attr;
  SSchema        XLinkSchema;

  /* does the pasted element come from another document? */
  originDocument = (Document) event->position;
  if (originDocument > 0 && originDocument != event->document)
    /* this element has changed document. Check its links */
    {
    XLinkSchema = TtaGetSSchema (TEXT("XLink"), event->document);
    if (XLinkSchema)
      {
      /* is there an href attribute from the XLink namespace? */
      attrType.AttrSSchema = XLinkSchema;
      attrType.AttrTypeNum = XLink_ATTR_href_;
      attr = TtaGetAttribute (event->element, attrType);
      if (attr)
	/* the pasted element has an href attribute. Update the value
	   of that attribute */
        ChangeURI (event->element, attr, originDocument, event->document);
      }
    }
}
