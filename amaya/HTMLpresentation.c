/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * Amaya editing functions called form Thot and declared in HTML.A.
 * These functions concern presentation of HTML elements.
 *
 * Authors: D. Veillard, I. Vatton
 *
 */

/* Included headerfiles */
#define THOT_EXPORT extern
#include "amaya.h"
#include "css.h"

#include "HTMLstyle_f.h"


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
boolean             DeleteStyleRule (NotifyElement * event)
#else  /* __STDC__ */
boolean             DeleteStyleRule (event)
NotifyElement      *event;

#endif /* __STDC__ */
{
   RemoveStyleRule (event->element, event->document);
   return TRUE;			/* let Thot perform normal operation */
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ChangePRule (NotifyPresentation * event)
#else  /* __STDC__ */
void                ChangePRule (event)
NotifyPresentation *event;

#endif /* __STDC__ */
{
   AttributeType       attrType;
   Attribute           styleAttr;
   char                style[1000];
   int                 len;

   /* does the element have a Style_ attribute ? */
   attrType.AttrSSchema = TtaGetDocumentSSchema (event->document);
   attrType.AttrTypeNum = HTML_ATTR_Style_;
   styleAttr = TtaGetAttribute (event->element, attrType);
   /* keep the new style string */
   len = 100;
   GetHTML3StyleString (event->element, event->document, style, &len);
   if (len == 0)
     {
	/* delete the style attribute */
	if (styleAttr != 0)
	   TtaRemoveAttribute (event->element, styleAttr, event->document);
     }
   else
     {
	if (styleAttr == 0)
	  {
	     styleAttr = TtaNewAttribute (attrType);
	     TtaAttachAttribute (event->element, styleAttr, event->document);
	  }
	/* copy the style string into the style attribute */
	TtaSetAttributeText (styleAttr, style, event->element, event->document);
     }
}


/*----------------------------------------------------------------------
 AttrLanguageRemoved
 A Lang attribute has been removed from element el in document doc.
 -----------------------------------------------------------------------*/
#ifdef __STDC__
static void AttrLanguageRemoved (Element el, Document doc)
#else /* __STDC__*/
static void AttrLanguageRemoved (el, doc)
     Element el;
     Document doc;
#endif /* __STDC__*/
{
  ElementType	elType;
  Element	span, child, next;
  Attribute	attr;

  /* if the element is a SPAN without any other attribute, remove the SPAN
     element */
  elType = TtaGetElementType (el);
  if (elType.ElTypeNum == HTML_EL_Span)
     {
     span = el;
     attr = NULL;
     TtaNextAttribute (span, &attr);
     if (attr == NULL)
        {
	child = TtaGetFirstChild (span);
	while (child != NULL)
	   {
	   next = child;
	   TtaNextSibling (&next);
	   TtaRemoveTree (child, doc);
	   TtaInsertSibling (child, span, TRUE, doc);
	   child = next;
	   }
	TtaDeleteTree (span, doc);
	}
     }
}


/*----------------------------------------------------------------------
 AttrLangDeleted
 A Lang attribute has been deleted
 -----------------------------------------------------------------------*/
#ifdef __STDC__
void AttrLangDeleted (NotifyAttribute *event)
#else /* __STDC__*/
void AttrLangDeleted(event)
     NotifyAttribute *event;
#endif /* __STDC__*/
{
  /* if the element is a SPAN without any other attribute, remove the SPAN
     element */
  AttrLanguageRemoved (event->element, event->document);
}


/*----------------------------------------------------------------------
 MoveAttrLang
 
 -----------------------------------------------------------------------*/
#ifdef __STDC__
static void MoveAttrLang (Attribute oldAttr, Element *el, Document doc)
#else /* __STDC__*/
static void MoveAttrLang (oldAttr, el, doc)
     Attribute oldAttr;
     Element *el;
     Document doc;
#endif /* __STDC__*/
{
  Element	first, parent, sibling, next;
  Attribute	newAttr, attr;
  AttributeType	attrType;
  int		kind, len;
#define BULEN 32
  char		value[BULEN], oldValue[BULEN];
  boolean	sameLang;

  /* if all siblings have the same LANG attribute, move that attibute to
     the parent element */
  parent = TtaGetParent (*el);
  if (parent != NULL)
     {
     TtaGiveAttributeType (oldAttr, &attrType, &kind);
     len = BULEN - 1;
     TtaGiveTextAttributeValue (oldAttr, oldValue, &len);
     first = TtaGetFirstChild (parent);
     sameLang = TRUE;
     sibling = first;
     while (sibling != NULL && sameLang)
	{
	if (sibling != *el)
	   {
	   attr = TtaGetAttribute (sibling, attrType);
	   if (attr == NULL)
	      sameLang = FALSE;
	   else
	      {
	      len = BULEN - 1;
	      TtaGiveTextAttributeValue (attr, value, &len);
	      if (strcasecmp(oldValue, value) != 0)
		 sameLang = FALSE;
	      }
	   }
	if (sameLang)
	   TtaNextSibling (&sibling);
	}
     if (sameLang)
        /* all sibling have the same LANG attribute */
	{
	/* delete the LANG attribute for all siblings */
	sibling = first;
	while (sibling != NULL)
	   {
	   attr = TtaGetAttribute (sibling, attrType);
	   TtaRemoveAttribute (sibling, attr, doc);
	   next = sibling;
	   TtaNextSibling (&next);
	   AttrLanguageRemoved (sibling, doc);
	   sibling = next;
	   }
	/* associate a LANG attribute to the parent element */
	newAttr = TtaNewAttribute (attrType);
	TtaAttachAttribute (parent, newAttr, doc);
	TtaSetAttributeText (newAttr, oldValue, parent, doc);
	*el = parent;
	/* do it again for the parent element */
        MoveAttrLang (newAttr, el, doc);
	}
     }
}


/*----------------------------------------------------------------------
 AttrLangCreated
 A Lang attribute has been created
 -----------------------------------------------------------------------*/
#ifdef __STDC__
void AttrLangCreated (NotifyAttribute *event)
#else /* __STDC__*/
void AttrLangCreated(event)
     NotifyAttribute *event;
#endif /* __STDC__*/
{
  ElementType	elType;
  Element	el, span, text;
  Attribute	oldAttr, newAttr;
  AttributeType	attrType;
  int		kind, len;
#define BULEN 32
  char		oldValue[BULEN];

  /* move the LANG attribute to the parent element if all sibling have the
     same attribute with the same value */
  el = event->element;
  MoveAttrLang (event->attribute, &el, event->document);

  /* if the LANG attribute is on a text string, create a SPAN element that
     encloses this text string and move the LANG attribute to that SPAN
     element */
  elType = TtaGetElementType (el);
  if (elType.ElTypeNum == HTML_EL_TEXT_UNIT)
     {
     text = el;
     elType.ElTypeNum = HTML_EL_Span;
     span = TtaNewElement (event->document, elType);
     TtaInsertSibling (span, text, FALSE, event->document);
     oldAttr = event->attribute;
     TtaGiveAttributeType (oldAttr, &attrType, &kind);
     newAttr = TtaNewAttribute (attrType);
     TtaAttachAttribute (span, newAttr, event->document);
     len = BULEN - 1;
     TtaGiveTextAttributeValue (oldAttr, oldValue, &len);
     TtaSetAttributeText (newAttr, oldValue, span, event->document);
     TtaRemoveAttribute (text, oldAttr, event->document);
     TtaRemoveTree (text, event->document);
     TtaInsertFirstChild (&text, span, event->document);
     }  
}
