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
 * Author: I. Vatton
 *
 */
 
/* Included headerfiles */
#define THOT_EXPORT extern
#include "amaya.h"
#include "css.h"
#include "MathML.h"
#ifdef GRAPHML
#include "GraphML.h"
#endif
#include "presentation.h"

#include "EDITstyle_f.h"
#include "HTMLimage_f.h"
#include "html2thot_f.h"
#include "styleparser_f.h"
#include "undo.h"

/*----------------------------------------------------------------------
 MakeASpan
 if element elem is a text string that is not the single child of a
 Span element, create a span element that contains that text string
 and return TRUE; span contains then the created Span element.
 -----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool MakeASpan (Element elem, Element *span, Document doc)
#else /* __STDC__*/
ThotBool MakeASpan (elem, span, doc)
     Element elem;
     Element *span;
     Document doc;
#endif /* __STDC__*/
{
  ElementType	elType;
  Element	parent, sibling;
  ThotBool	ret, doit;

  ret = FALSE;
  *span = NULL;
  elType = TtaGetElementType (elem);
  if (ustrcmp(TtaGetSSchemaName (elType.ElSSchema), TEXT("HTML")) == 0)
    /* it's an HTML element */
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
	   /* mark the document as modified */
	   TtaSetDocumentModified (doc);
	   ret = TRUE;
	   }
	}
     }
  return ret;
}

/*----------------------------------------------------------------------
 DeleteSpanIfNoAttr
 An attribute has been removed from element el in document doc.
 If element el is a SPAN without any attribute, the SPAN element is
 removed and variables firstChild and lastChild are the first and
 last child of the former SPAN element.
 Otherwise, firstChild and lastChild are NULL.
 -----------------------------------------------------------------------*/
#ifdef __STDC__
void DeleteSpanIfNoAttr (Element el, Document doc, Element *firstChild, Element *lastChild)
#else /* __STDC__*/
void DeleteSpanIfNoAttr (el, doc, firstChild, lastChild)
     Element el;
     Document doc;
     Element *firstChild;
     Element *lastChild;
#endif /* __STDC__*/
{
  ElementType	elType;
  Element	span, child, next;
  Attribute	attr;

  /* if the element is a SPAN without any other attribute, remove the SPAN
     element */
  *firstChild = NULL;
  *lastChild = NULL;
  elType = TtaGetElementType (el);
  if (elType.ElTypeNum == HTML_EL_Span &&
      ustrcmp(TtaGetSSchemaName (elType.ElSSchema), TEXT("HTML")) == 0)
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
	   if (*firstChild == NULL)
	      *firstChild = child;
	   *lastChild = child;
	   child = next;
	   }
	/* The standard Thot command has registered the operation that
 	   removed the attribute from the SPAN element.  Cancel this operation
	   from the editing history and register the removal for the TEXT
	   element instead */
	if (*firstChild)
	   TtaChangeLastRegisteredAttr (span, *firstChild, NULL, NULL, doc);
	TtaDeleteTree (span, doc);
	}
     }
}

/*----------------------------------------------------------------------
  AttrToSpan
  If attribute attr is on a text string (elem), create a SPAN element
  enclosing this text string and move the attribute to that SPAN element.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void  AttrToSpan (Element elem, Attribute attr, Document doc)
#else /* __STDC__*/
void  AttrToSpan (elem, attr, doc)
     Element elem;
     Attribute attr;
     Document doc;
#endif /* __STDC__*/
{
  Element	span, parent;
  Attribute	newAttr;
  AttributeType	attrType;
  ElementType   elType;
  int		kind, len;
#define ATTRLEN 64
  STRING	oldValue; /* [ATTRLEN]; */

  elType = TtaGetElementType (elem);
  if (elType.ElTypeNum == HTML_EL_TEXT_UNIT)
    /* it's a character string */
    {
      parent = TtaGetParent (elem);
      elType = TtaGetElementType (parent);
      if (ustrcmp(TtaGetSSchemaName (elType.ElSSchema), TEXT("HTML")) == 0)
        /* the parent element is an HTML element */
	/* Create a Span element and move to attribute to this Span element */
        MakeASpan (elem, &span, doc);
      else
        /* move the attribute to the parent element */
	span = parent;
      if (span != NULL)
        {
          oldValue = TtaAllocString (ATTRLEN);
          TtaGiveAttributeType (attr, &attrType, &kind);
	  newAttr = TtaGetAttribute (span, attrType);
	  if (newAttr == NULL)
	     {
	     newAttr = TtaNewAttribute (attrType);
	     TtaAttachAttribute (span, newAttr, doc);
	     }
          len = ATTRLEN - 1;
          TtaGiveTextAttributeValue (attr, oldValue, &len);
	  TtaSetAttributeText (newAttr, oldValue, span, doc);
	  /* The standard Thot command has registered the operation that
 	     added the attribute to the TEXT element.  Cancel this operation
	     from the editing history and register the new attribute on the
	     span element instead */
	  TtaChangeLastRegisteredAttr (elem, span, attr, newAttr, doc);
          TtaRemoveAttribute (elem, attr, doc);
          TtaFreeMemory (oldValue);
        }
    }
}

/*----------------------------------------------------------------------
  GlobalAttrCreated: the user has created a global attribute
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                GlobalAttrCreated (NotifyAttribute * event)
#else
void                GlobalAttrCreated (event)
NotifyAttribute    *event;
 
#endif
{
   /* if the attribute is on a text string, create a SPAN element that encloses
      this text string and move the attribute to that SPAN element */
   AttrToSpan (event->element, event->attribute, event->document);
}

/*----------------------------------------------------------------------
  GlobalAttrDeleted: the user has removed a global attribute
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                GlobalAttrDeleted (NotifyAttribute * event)
#else
void                GlobalAttrDeleted (event)
NotifyAttribute    *event;
 
#endif
{
   Element	firstChild, lastChild;

   /* if the element is a SPAN without any other attribute, remove the SPAN
      element */
   DeleteSpanIfNoAttr (event->element, event->document, &firstChild, &lastChild);
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
   Element	firstChild, lastChild;

   if (event->event == TteAttrDelete)
      /* if the element is a SPAN without any other attribute, remove the SPAN
         element */
      DeleteSpanIfNoAttr (event->element, event->document, &firstChild, &lastChild);
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
  showBoxAllowed is TRUE if the ShowBox rule could be generated
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void MovePRule (PRule presRule, Element fromEl, Element toEl, Document doc, ThotBool showBoxAllowed)
#else  /* __STDC__ */
static void MovePRule (presRule, fromEl, toEl, doc, showBoxAllowed)
PRule       presRule;
Element     fromEl;
Element     toEl;
Document    doc;
ThotBool    showBoxAllowed;
#endif /* __STDC__ */
{
   int         presRuleType;
   PRule       newPRule, oldPRule;
   ThotBool    addShow;

   presRuleType = TtaGetPRuleType (presRule);
   newPRule = TtaCopyPRule (presRule);
   addShow = (showBoxAllowed && presRuleType == PRBackground);
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
   ElementType         elType;
   Element	       firstChild, lastChild;
#define STYLELEN 1000
   STRING              style;
   int                 len;

   /* does the element have a Style_ attribute ? */
   elType = TtaGetElementType (elem);
   if (ustrcmp (TtaGetSSchemaName (elType.ElSSchema), TEXT("MathML")) == 0)
      {
	attrType.AttrSSchema = elType.ElSSchema;
	attrType.AttrTypeNum = MathML_ATTR_style_;
      }
   else
#ifdef GRAPHML
   elType = TtaGetElementType (elem);
   if (ustrcmp (TtaGetSSchemaName (elType.ElSSchema), TEXT("GraphML")) == 0)
      {
	attrType.AttrSSchema = elType.ElSSchema;
	attrType.AttrTypeNum = GraphML_ATTR_style_;
      }
   else
#endif
      {
	attrType.AttrSSchema = TtaGetSSchema (TEXT("HTML"), doc);
	attrType.AttrTypeNum = HTML_ATTR_Style_;
      }
   styleAttr = TtaGetAttribute (elem, attrType);

   /* keep the new style string */
   len = STYLELEN;
   style = TtaAllocString (STYLELEN);
   GetHTMLStyleString (elem, doc, style, &len);
   if (len == 0)
     {
	/* delete the style attribute */
	if (styleAttr != 0)
	   {
	     TtaRegisterAttributeDelete (styleAttr, elem, doc);
	     TtaRemoveAttribute (elem, styleAttr, doc);
	     DeleteSpanIfNoAttr (elem, doc, &firstChild, &lastChild);
	   }
     }
   else
     {
	if (styleAttr == 0)
	  {
	     styleAttr = TtaNewAttribute (attrType);
	     TtaAttachAttribute (elem, styleAttr, doc);
	     /* copy the style string into the style attribute */
	     TtaSetAttributeText (styleAttr, style, elem, doc);
	     TtaRegisterAttributeCreate (styleAttr, elem, doc);
	  }
	else
	  {
	     TtaRegisterAttributeReplace (styleAttr, elem, doc);
	     /* copy the style string into the style attribute */
	     TtaSetAttributeText (styleAttr, style, elem, doc);
	  }
     }
   TtaFreeMemory (style);
}


/*----------------------------------------------------------------------
  ChangePRule
  A specific PRule will be created or modified by the user for
  a given element. (pre-event)
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool            ChangePRule (NotifyPresentation * event)
#else  /* __STDC__ */
ThotBool            ChangePRule (event)
NotifyPresentation *event;

#endif /* __STDC__ */
{
  ElementType	     elType, parentType;
  Element	     el, span, body, root, parent;
  PRule	             presRule;
  Document	     doc;
  SSchema	     HTMLschema;
  AttributeType      attrType;
  Attribute          attr;
#define STYLELEN 1000
  CHAR_T               buffer[15];
  int                presType;
  int                w, h, unit, value, i;
  ThotBool           ret;

  el = event->element;
  doc = event->document;
  presType = event->pRuleType;
  presRule = event->pRule;
  elType = TtaGetElementType (el);
  ret = FALSE;
  HTMLschema = TtaGetSSchema (TEXT("HTML"), doc);

  /* if it's a background rule on element BODY, move it to element HTML */
  /* if it's a rule on element HTML and it's not a background rule, move
     it to element BODY */
  if (event->event != TtePRuleDelete)
    if (elType.ElSSchema != HTMLschema)
      /* it's not an HTML element */
      {
	if (TtaGetConstruct (el) == ConstructBasicType)
	  /* it's a basic type. Move the PRule to the parent element */
	  {
	    el = TtaGetParent (el);
	    MovePRule (presRule, event->element, el, doc, FALSE);
	    ret = TRUE; /* don't let Thot perform normal operation */
	  }
      }
    else
      /* it's an HTML element */
      {
	if (elType.ElTypeNum == HTML_EL_BODY
	    && (presType == PRFillPattern || presType == PRBackground ||
		presType == PRShowBox))
	  {
	    root = TtaGetParent (el);
	    if (presType == PRBackground)
	      MovePRule (presRule, el, root, doc, TRUE);
	    ret = TRUE; /* don't let Thot perform normal operation */      
	  }
	else if (elType.ElTypeNum == HTML_EL_HTML)
	  {
	    elType.ElTypeNum = HTML_EL_BODY;
	    body = TtaSearchTypedElement (elType, SearchInTree, el);
	    if (presType != PRFillPattern && presType != PRBackground
		&& presType != PRShowBox)
	      {
		MovePRule (presRule, el, body, doc, TRUE);
		ret = TRUE; /* don't let Thot perform normal operation */
	      }      
	    el = body;
	  }
	else
	  {
	    /* style is not allowed in Head section */
	    if (elType.ElTypeNum == HTML_EL_HEAD)
	      parent = el;
	    else
	      {
		parentType.ElSSchema = elType.ElSSchema;
		parentType.ElTypeNum = HTML_EL_HEAD;
		parent = TtaGetTypedAncestor (el, parentType);
	      }

	    if (parent != NULL)
	      {
		TtaSetStatus (doc, 1, TtaGetMessage (AMAYA, AM_INVALID_TARGET), NULL);
		return (TRUE); /* don't let Thot perform normal operation */
;
	      }
	    else
	      {
		/* style is not allowed in MAP */
		if (elType.ElTypeNum == HTML_EL_MAP)
		  parent = el;
		else
		  {
		    parentType.ElTypeNum = HTML_EL_MAP;
		    parent = TtaGetTypedAncestor (el, parentType);
		  }
		if (parent != NULL)
		  {
		    TtaSetStatus (doc, 1, TtaGetMessage (AMAYA, AM_INVALID_TARGET), NULL);
		    return (TRUE);
		  }
	      }

	    if ((presType == PRWidth || presType == PRHeight) &&
		elType.ElSSchema == HTMLschema &&
		elType.ElTypeNum == HTML_EL_Pseudo_paragraph)
	      {
		/* check if the rule has to be moved to a cell */
		parent = TtaGetParent (el);
		parentType = TtaGetElementType (parent);
		if (parentType.ElSSchema == HTMLschema &&
		    (parentType.ElTypeNum == HTML_EL_Data_cell ||
		     parentType.ElTypeNum == HTML_EL_Heading_cell))
		  {
		    el = parent;
		    elType.ElTypeNum = parentType.ElTypeNum;
		  }
	      }

	    if ((presType == PRWidth || presType == PRHeight) &&
		elType.ElSSchema == HTMLschema &&
		(elType.ElTypeNum == HTML_EL_PICTURE_UNIT ||
		 elType.ElTypeNum == HTML_EL_Table ||
		 elType.ElTypeNum == HTML_EL_Data_cell ||
		 elType.ElTypeNum == HTML_EL_Heading_cell ||
		 elType.ElTypeNum == HTML_EL_Object ||
#ifdef GRAPHML
		 elType.ElTypeNum == HTML_EL_XMLGraphics ||
#endif /* GRAPHML */
		 elType.ElTypeNum == HTML_EL_Applet))
	      {
		/* store information in Width or Height attribute */
		if (elType.ElTypeNum == HTML_EL_PICTURE_UNIT)
		  TtaGiveBoxSize (el, doc, 1, UnPixel, &w, &h);
		else
		  {
		    w = -1;
		    h = -1;
		  }
		TtaSwitchSelection (doc, 1, FALSE);
		attrType.AttrSSchema = HTMLschema;
		unit = TtaGetPRuleUnit (presRule);

		if (presType == PRWidth)
		  {
		    /* the new value is the old one plus the delta */
		    TtaGiveBoxSize (el, doc, 1, unit, &value, &i);
		    value += TtaGetPRuleValue (presRule);
		    if (unit == UnPercent)
		      usprintf (buffer, TEXT("%d%%"), value);
		    else
		      usprintf (buffer, TEXT("%d"), value);

		    attrType.AttrTypeNum = HTML_ATTR_Width__;
		    attr = TtaGetAttribute (el, attrType);
		    if (attr == NULL)
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
		    TtaSetAttributeText (attr, buffer, el, doc);
		    CreateAttrWidthPercentPxl (buffer, el, doc, w);
		  }
		else
		  {
		    /* the new value is the old one plus the delta */
		    TtaGiveBoxSize (el, doc, 1, unit, &i, &value);
		    value += TtaGetPRuleValue (presRule);
		    attrType.AttrTypeNum = HTML_ATTR_Height_;
		    attr = TtaGetAttribute (el, attrType);
		    if (attr == NULL)
		      {
			attr = TtaNewAttribute (attrType);
			TtaAttachAttribute (el, attr, doc);
		        TtaSetAttributeValue (attr, value, el, doc);
			TtaRegisterAttributeCreate (attr, el, doc);
		      }
		    else
		      {
			TtaRegisterAttributeReplace (attr, el, doc);
		        TtaSetAttributeValue (attr, value, el, doc);
		      }
		    /* update the associated map */
		    if (elType.ElTypeNum == HTML_EL_PICTURE_UNIT)
		      UpdateImageMap (el, doc, -1, h);
		  }
		TtaSwitchSelection (doc, 1, TRUE);
		return (TRUE);
	      }
	    else if (IsCharacterLevelElement (el)
		&& (presType == PRIndent || presType == PRLineSpacing
		    || presType == PRAdjust || presType == PRJustify
		    || presType == PRHyphenate))
	      /* if the rule is a Format rule applied to a character-level element,
		 move it to the first enclosing non character-level element */
	      {
		do
		  el = TtaGetParent (el);
		while (el != NULL && IsCharacterLevelElement (el));
		/* if the PRule is on a Pseudo-Paragraph, move it to the enclosing
		   element */
		if (elType.ElTypeNum == HTML_EL_Pseudo_paragraph)
		  el = TtaGetParent (el);
		MovePRule (presRule, event->element, el, doc, TRUE);
		ret = TRUE; /* don't let Thot perform normal operation */
	      }	  
	    else if (elType.ElTypeNum == HTML_EL_Pseudo_paragraph)
	      /* if the PRule is on a Pseudo-Paragraph, move it to the enclosing
		 element */
	      {
		el = TtaGetParent (el);
		MovePRule (presRule, event->element, el, doc, TRUE);
		ret = TRUE; /* don't let Thot perform normal operation */
	      }
	    else if (MakeASpan (el, &span, doc))
	      /* if it is a new PRule on a text string, create a SPAN element that
		 encloses this text string and move the PRule to that SPAN element */
	      {
		MovePRule (presRule, el, span, doc, TRUE);
		el = span;
		ret = TRUE; /* don't let Thot perform normal operation */
	      }
	  }
      }
  /* set the Style_ attribute ? */
  SetStyleAttribute (doc, el);
  TtaSetDocumentModified (doc);
  return (ret);
}


/*----------------------------------------------------------------------
  PRuleDeleted
  A specific PRule has been deleted by the user for a given element
  (post-event)
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                PRuleDeleted (NotifyPresentation * event)
#else  /* __STDC__ */
void                PRuleDeleted (event)
NotifyPresentation *event;
#endif /* __STDC__ */
{
  /* set the Style_ attribute ? */
  SetStyleAttribute (event->document, event->element);
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
   Element	 firstChild, lastChild;
   AttributeType attrType;
   Attribute	 attr;

  /* if the element is a SPAN without any other attribute, remove the SPAN
     element */
  DeleteSpanIfNoAttr (event->element, event->document, &firstChild, &lastChild);
  /* if it's the root (HTML) element, delete the RealLang attribute */
  if (!TtaGetParent (event->element))
     /* it's the root element */
     {
     attrType.AttrSSchema = event->attributeType.AttrSSchema;
     attrType.AttrTypeNum = HTML_ATTR_RealLang;
     attr = TtaGetAttribute (event->element, attrType);
     if (attr)
	TtaRemoveAttribute (event->element, attr, event->document);
     }
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
  Element	first, parent, sibling, next, firstChild, lastChild;
  Attribute	newAttr, attr;
  AttributeType	attrType;
  int		kind, len;
  STRING	value    = TtaAllocString (ATTRLEN); 
  STRING oldValue = TtaAllocString (ATTRLEN);
  ThotBool	sameLang;

  /* if all siblings have the same LANG attribute, move that attibute to
     the parent element, unless the parent element has exception Hidden */
  parent = TtaGetParent (*el);
  if (parent)
     if (TtaIsHidden (parent))
        parent = NULL;
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
	      if (ustrcasecmp(oldValue, value) != 0)
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
	   DeleteSpanIfNoAttr (sibling, doc, &firstChild, &lastChild);
	   sibling = next;
	   }
	/* associate a LANG attribute with the parent element */
	newAttr = TtaGetAttribute (parent, attrType);
	if (newAttr == NULL)
	  {
	    newAttr = TtaNewAttribute (attrType);
	    TtaAttachAttribute (parent, newAttr, doc);
	  }
	TtaSetAttributeText (newAttr, oldValue, parent, doc);
	*el = parent;
	/* do it again for the parent element */
        MoveAttrLang (newAttr, el, doc);
	}
     }
  TtaFreeMemory (value);
  TtaFreeMemory (oldValue);
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
  int		len;
  AttributeType attrType;
  Attribute	attr;
  STRING	value    = TtaAllocString (ATTRLEN); 

  /* move the LANG attribute to the parent element if all sibling have the
     same attribute with the same value */
  elem = event->element;
  MoveAttrLang (event->attribute, &elem, event->document);

  len = ATTRLEN - 1;
  TtaGiveTextAttributeValue (event->attribute, value, &len);
  if (ustrcasecmp(value, TEXT("Symbol")) == 0)
     /* it's a character string in the Symbol character set, it's not really
	a language */
    {
    TtaRemoveAttribute (elem, event->attribute, event->document);      
    }
  else
    {
      /* if the LANG attribute is on a text string, create a SPAN element that
	 encloses this text string and move the LANG attribute to that SPAN
	 element */
      AttrToSpan (elem, event->attribute, event->document);
    }
  /* if it's the root (HTML) element, create a RealLang attribute too */
  if (!TtaGetParent (elem))
     /* it's the root element */
     {
     attrType.AttrSSchema = event->attributeType.AttrSSchema;
     attrType.AttrTypeNum = HTML_ATTR_RealLang;
     attr = TtaNewAttribute (attrType);
     TtaAttachAttribute (elem, attr, event->document);
     TtaSetAttributeValue (attr, HTML_ATTR_RealLang_VAL_Yes_, elem,
			   event->document);
     }
  TtaFreeMemory (value);
}

/*----------------------------------------------------------------------
 AttrLangModified
 A Lang attribute has been modified
 -----------------------------------------------------------------------*/
#ifdef __STDC__
void AttrLangModified (NotifyAttribute *event)
#else /* __STDC__*/
void AttrLangModified(event)
     NotifyAttribute *event;
#endif /* __STDC__*/
{
  Element	elem;
  AttributeType attrType;
  Attribute	attr;

  elem = event->element;
  /* if it's the root (HTML) element, create a RealLang attribute */
  if (!TtaGetParent (elem))
     /* it's the root element */
     {
     attrType.AttrSSchema = event->attributeType.AttrSSchema;
     attrType.AttrTypeNum = HTML_ATTR_RealLang;
     attr = TtaGetAttribute (elem, attrType);
     if (!attr)
	{
        attr = TtaNewAttribute (attrType);
        TtaAttachAttribute (elem, attr, event->document);
        TtaSetAttributeValue (attr, HTML_ATTR_RealLang_VAL_Yes_, elem,
			      event->document);
	}
     }  
}

/*----------------------------------------------------------------------
 AttrLangShouldBeDeleted
 The user wants to remove a Lang attribute
 -----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool AttrLangShouldBeDeleted (NotifyAttribute *event)
#else /* __STDC__*/
ThotBool AttrLangShouldBeDeleted(event)
     NotifyAttribute *event;
#endif /* __STDC__*/
{
  Element	elem;
  AttributeType attrType;
  Attribute	attr;

  elem = event->element;
  /* if it's the root (HTML) element, delete the RealLang attribute */
  if (!TtaGetParent (elem))
     /* it's the root element */
     {
     attrType.AttrSSchema = event->attributeType.AttrSSchema;
     attrType.AttrTypeNum = HTML_ATTR_RealLang;
     attr = TtaGetAttribute (elem, attrType);
     if (attr)
	TtaRemoveAttribute (elem, attr, event->document);
     }  
  return FALSE; /* let Thot perform normal operation */
}
