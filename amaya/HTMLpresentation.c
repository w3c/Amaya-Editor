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
#include "html2thot_f.h"
#include "presentation.h"


/*----------------------------------------------------------------------
  DeleteStyleRule
  A STYLE element will be deleted in the document HEAD.
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
 MakeASpan
 if element elem is a text string that is not the single child of a
 Span element, create a span element that contains that text string
 and return TRUE; span contains then the created Span element.
 -----------------------------------------------------------------------*/
#ifdef __STDC__
static boolean MakeASpan (Element elem, Element *span, Document doc)
#else /* __STDC__*/
static boolean MakeASpan (elem, span, doc)
     Element elem;
     Element *span;
     Document doc;
#endif /* __STDC__*/
{
  ElementType	elType;
  Element	parent, sibling;
  boolean	ret, doit;

  ret = FALSE;
  *span = NULL;
  elType = TtaGetElementType (elem);
  if (elType.ElTypeNum == HTML_EL_TEXT_UNIT)
     {
     parent = TtaGetParent (elem);
     if (parent != NULL)
	{
	doit = TRUE;
	elType = TtaGetElementType (parent);
	if (elType.ElTypeNum == HTML_EL_Span)
	   {
	   sibling = elem;
	   TtaNextSibling (&sibling);
	   if (sibling == NULL)
	      {
	      sibling = elem;
	      TtaPreviousSibling (&sibling);
	      if (sibling == NULL)
		{
	         doit = FALSE;
		 *span = parent;
		 ret = TRUE;
		}
	      }
	   }
	if (doit)
	   {
	   elType.ElTypeNum = HTML_EL_Span;
	   *span = TtaNewElement (doc, elType);
	   TtaInsertSibling (*span, elem, FALSE, doc);
	   TtaRemoveTree (elem, doc);
	   TtaInsertFirstChild (&elem, *span, doc);
	   ret = TRUE;
	   }
	}
     }
  return ret;
}

/*----------------------------------------------------------------------
 DeleteSpanIfNoAttr
 An attribute has been removed from element el in document doc.
 -----------------------------------------------------------------------*/
#ifdef __STDC__
void DeleteSpanIfNoAttr (Element el, Document doc)
#else /* __STDC__*/
void DeleteSpanIfNoAttr (el, doc)
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
  AttrToSpan
  If attribute attr is on a text string (elem), create a SPAN element that
  encloses this text string and move the attribute to that SPAN element.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void  AttrToSpan (Element elem, Attribute attr, Document doc)
#else /* __STDC__*/
static void  AttrToSpan (elem, attr, doc)
     Element elem;
     Attribute attr;
     Document doc;
#endif /* __STDC__*/
{
  Element	span;
  Attribute	newAttr;
  AttributeType	attrType;
  int		kind, len;
#define ATTRLEN 64
  char		oldValue[ATTRLEN];

  if (MakeASpan (elem, &span, doc))
     {
     TtaGiveAttributeType (attr, &attrType, &kind);
     newAttr = TtaNewAttribute (attrType);
     TtaAttachAttribute (span, newAttr, doc);
     len = ATTRLEN - 1;
     TtaGiveTextAttributeValue (attr, oldValue, &len);
     TtaRemoveAttribute (elem, attr, doc);
     TtaSetAttributeText (newAttr, oldValue, span, doc);
     }  
}

/*----------------------------------------------------------------------
  AttrStyleDeleted: the user has removed a Style attribute
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                AttrStyleDeleted (NotifyAttribute * event)
#else
void                AttrStyleDeleted (event)
NotifyAttribute    *event;
 
#endif
{
   /* if the element is a SPAN without any other attribute, remove the SPAN
      element */
   DeleteSpanIfNoAttr (event->element, event->document);
}

/*----------------------------------------------------------------------
  AttrClassChanged: the user has created removed or modified a Class
  attribute
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                AttrClassChanged (NotifyAttribute * event)
#else
void                AttrClassChanged (event)
NotifyAttribute    *event;
 
#endif
{
   if (event->event == TteAttrDelete)
      /* if the element is a SPAN without any other attribute, remove the SPAN
         element */
      DeleteSpanIfNoAttr (event->element, event->document);
   else if (event->event == TteAttrCreate)
      /* if the Class attribute is on a text string, create a SPAN element that
         encloses this text string and move the Class attribute to that SPAN
         element */
      AttrToSpan (event->element, event->attribute, event->document);
}


/*----------------------------------------------------------------------
  MovePRule
  remove presentation rule presRule from element fromEl and associate a copy
  of that rule with element toEl, for the main view.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void MovePRule (PRule presRule, Element fromEl, Element toEl, Document doc)
#else  /* __STDC__ */
static void MovePRule (presRule, fromEl, toEl, doc)
PRule       presRule;
Element     fromEl;
Element     toEl;
Document    doc;

#endif /* __STDC__ */
{
   int         presRuleType;
   PRule       newPRule, oldPRule;
   boolean     addShow;

   presRuleType = TtaGetPRuleType (presRule);
   newPRule = TtaCopyPRule (presRule);
   addShow = (presRuleType == PRBackground);
   /* if the destination element already has a PRule of that type, remove
      that PRule from the destination element */
   oldPRule = TtaGetPRule (toEl, presRuleType);
   if (oldPRule != NULL)
      if (TtaGetPRuleView (oldPRule) == 1)
	{
	  TtaRemovePRule (toEl, oldPRule, doc);
	  /* showBox rule already exists */
	  addShow = FALSE;
	}
   /* this PRule applies to view 1 (main view) */
   TtaSetPRuleView (newPRule, 1);
   TtaRemovePRule (fromEl, presRule, doc);
   TtaAttachPRule (toEl, newPRule, doc);
   if (addShow)
     {
       /* add FillPattern rule */
       newPRule = TtaNewPRuleForView (PRFillPattern, 1, doc);
       TtaAttachPRule (toEl, newPRule, doc);
       TtaSetPRuleValue (toEl, newPRule, 2, doc);
       /* add ShowBox rule */
       newPRule = TtaNewPRuleForView (PRShowBox, 1, doc);
       TtaAttachPRule (toEl, newPRule, doc);
     }
}


/*----------------------------------------------------------------------
  SetStyleAttribute.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                SetStyleAttribute (Document doc, Element elem)
#else  /* __STDC__ */
void                SetStyleAttribute (doc, elem)
Document            doc;
Element             elem;
#endif /* __STDC__ */
{
   AttributeType       attrType;
   Attribute           styleAttr;
#define STYLELEN 1000
   char                style[STYLELEN];
   int                 len;

   /* does the element have a Style_ attribute ? */
   attrType.AttrSSchema = TtaGetDocumentSSchema (doc);
   attrType.AttrTypeNum = HTML_ATTR_Style_;
   styleAttr = TtaGetAttribute (elem, attrType);
   /* keep the new style string */
   len = STYLELEN;
   GetHTML3StyleString (elem, doc, style, &len);
   if (len == 0)
     {
	/* delete the style attribute */
	if (styleAttr != 0)
	   {
	   TtaRemoveAttribute (elem, styleAttr, doc);
	   DeleteSpanIfNoAttr (elem, doc);
	   }
     }
   else
     {
	if (styleAttr == 0)
	  {
	     styleAttr = TtaNewAttribute (attrType);
	     TtaAttachAttribute (elem, styleAttr, doc);
	  }
	/* copy the style string into the style attribute */
	TtaSetAttributeText (styleAttr, style, elem, doc);
     }
}


/*----------------------------------------------------------------------
  ChangePRule
  A specific PRule has been created, modified or deleted by the user for
  a given element.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
boolean             ChangePRule (NotifyPresentation * event)
#else  /* __STDC__ */
boolean             ChangePRule (event)
NotifyPresentation *event;

#endif /* __STDC__ */
{
  ElementType	     elType;
  Element	     elem, span, body, root;
  PRule	             presRule;
  Document	     doc;
#define STYLELEN 1000
  int                presType;
  boolean            ret;

  elem = event->element;
  doc = event->document;
  presType = event->pRuleType;
  presRule = event->pRule;
  elType = TtaGetElementType (elem);
  ret = FALSE;

  /* if it's a background rule on element BODY, move it to element HTML */
  /* if it's a rule on element HTML and it's not a background rule, move
     it to element BODY */
  if (event->event != TtePRuleDelete)
    {
      if (elType.ElTypeNum == HTML_EL_BODY 
	  && (presType == PRFillPattern || presType == PRBackground || presType == PRShowBox))
	{
	  root = TtaGetParent (elem);
	  if (presType == PRBackground)
	    MovePRule (presRule, elem, root, doc);
	  ret = TRUE; /* don't let Thot perform normal operation */      
	}
      else if (elType.ElTypeNum == HTML_EL_HTML)
	{
	  elType.ElTypeNum = HTML_EL_BODY;
	  body = TtaSearchTypedElement (elType, SearchInTree, elem);
	  if (presType != PRFillPattern && presType != PRBackground
	      && presType != PRShowBox)
	    {
	      MovePRule (presRule, elem, body, doc);
	      ret = TRUE; /* don't let Thot perform normal operation */
	    }      
	  elem = body;
	}
      else
	{
	  if (IsCharacterLevelElement (elem)
	      && (presType == PRIndent || presType == PRLineSpacing
		  || presType == PRAdjust || presType == PRJustify
		  || presType == PRHyphenate))
	    /* if the rule is a Format rule applied to a character-level element,
	       move it to the first enclosing non character-level element */
	    {
	      do
		elem = TtaGetParent (elem);
	      while (elem != NULL && IsCharacterLevelElement (elem));
	      /* if the PRule is on a Pseudo-Paragraph, move it to the enclosing
		 element */
	      if (elType.ElTypeNum == HTML_EL_Pseudo_paragraph)
		elem = TtaGetParent (elem);
	      MovePRule (presRule, event->element, elem, doc);
	      ret = TRUE; /* don't let Thot perform normal operation */
	    }	  
	  else if (elType.ElTypeNum == HTML_EL_Pseudo_paragraph)
	    /* if the PRule is on a Pseudo-Paragraph, move it to the enclosing
	       element */
	    {
	      elem = TtaGetParent (elem);
	      MovePRule (presRule, event->element, elem, doc);
	      ret = TRUE; /* don't let Thot perform normal operation */
	    }
	  else if (MakeASpan (elem, &span, doc))
	    /* if it is a new PRule on a text string, create a SPAN element that
	       encloses this text string and move the PRule to that SPAN element */
	    {
	      MovePRule (presRule, elem, span, doc);
	      elem = span;
	      ret = TRUE; /* don't let Thot perform normal operation */
	    }
	}
    }
  
  /* set the Style_ attribute ? */
  SetStyleAttribute (doc, elem);
  return (ret);
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
  DeleteSpanIfNoAttr (event->element, event->document);
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
  char		value[ATTRLEN], oldValue[ATTRLEN];
  boolean	sameLang;

  /* if all siblings have the same LANG attribute, move that attibute to
     the parent element */
  parent = TtaGetParent (*el);
  if (parent != NULL)
     {
     TtaGiveAttributeType (oldAttr, &attrType, &kind);
     len = ATTRLEN - 1;
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
	      len = ATTRLEN - 1;
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
	   DeleteSpanIfNoAttr (sibling, doc);
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
  Element	elem;

  /* move the LANG attribute to the parent element if all sibling have the
     same attribute with the same value */
  elem = event->element;
  MoveAttrLang (event->attribute, &elem, event->document);

  /* if the LANG attribute is on a text string, create a SPAN element that
     encloses this text string and move the LANG attribute to that SPAN
     element */
  AttrToSpan (elem, event->attribute, event->document);
}
