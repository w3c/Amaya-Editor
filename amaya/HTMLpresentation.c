/*
 *
 *  (c) COPYRIGHT INRIA and W3C, 1996-2008
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
#ifdef _SVG
#include "SVG.h"
#endif
#include "presentation.h"

#include "EDITstyle_f.h"
#include "HTMLimage_f.h"
#include "html2thot_f.h"
#include "HTMLpresentation_f.h"
#include "HTMLactions_f.h"
#include "styleparser_f.h"
#include "undo.h"
#include "XHTMLbuilder_f.h"


/*----------------------------------------------------------------------
  MakeASpan
  if element elem is a text string that is not the single child of a
  Span element, create a span element that contains that text string
  and return TRUE; span contains then the created Span element.
  if the parameter presRule is not NULL and a span is generated, the
  presRule should be moved to the new span.
  -----------------------------------------------------------------------*/
ThotBool MakeASpan (Element elem, Element *span, Document doc, PRule presRule)
{
  ElementType	elType;
  Element       parent, sibling;
  char         *name;
  ThotBool      ret, doit;

  ret = FALSE;
  *span = NULL;
  elType = TtaGetElementType (elem);
  name = TtaGetSSchemaName (elType.ElSSchema);
  if (!strcmp(name, "HTML") || !strcmp(name, "SVG"))
    /* it's an HTML or SVG element */
    if (elType.ElTypeNum == HTML_EL_TEXT_UNIT ||
        elType.ElTypeNum == HTML_EL_Basic_Elem)
      /* it's a text leaf */
      {
        parent = TtaGetParent (elem);
        if (parent != NULL)
          {
            doit = TRUE;
            elType = TtaGetElementType (parent);
            name = TtaGetSSchemaName (elType.ElSSchema);
            if ((elType.ElTypeNum == HTML_EL_Span && !strcmp(name, "HTML"))
#ifdef _SVG
                || (elType.ElTypeNum == SVG_EL_tspan && !strcmp (name, "SVG"))
#endif /* _SVG */
                )
              /* element parent is a span element */
              {
                sibling = elem;
                TtaNextSibling (&sibling);
                if (sibling == NULL)
                  {
                    sibling = elem;
                    TtaPreviousSibling (&sibling);
                    if (sibling == NULL)
                      /* the text leaf is the only child of its span parent.
                         No need to create a new span element */
                      {
                        doit = FALSE;
                        *span = parent;
                        if (presRule)
                          MovePRule (presRule, elem, *span, doc, TRUE);
                        ret = TRUE;
                      }
                  }
              }
            if (doit)
              /* enclose the text leaf within a span element */
              {
#ifdef _SVG
                if (!strcmp (name, "SVG"))
                  elType.ElTypeNum = SVG_EL_tspan;
                else
#endif /* _SVG */
                  elType.ElTypeNum = HTML_EL_Span;
                *span = TtaNewElement (doc, elType);
                TtaInsertSibling (*span, elem, FALSE, doc);
                TtaRegisterElementCreate (*span, doc);
                if (presRule)
                  MovePRule (presRule, elem, *span, doc, TRUE);
                TtaRegisterElementDelete (elem, doc);
                TtaRemoveTree (elem, doc);
                TtaInsertFirstChild (&elem, *span, doc);
                TtaRegisterElementCreate (elem, doc);
                /* mark the document as modified */
                TtaSetDocumentModified (doc);
                ret = TRUE;
              }
          }
      }
  UpdateContextSensitiveMenus (doc, 1);
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
void DeleteSpanIfNoAttr (Element el, Document doc, Element *firstChild,
                         Element *lastChild)
{
  ElementType	elType;
  Element	    span, child, next;
  Attribute	  attr;

  /* if the element is a SPAN without any other attribute, remove the SPAN
     element */
  *firstChild = NULL;
  *lastChild = NULL;
  elType = TtaGetElementType (el);
  if (elType.ElTypeNum == HTML_EL_Span &&
      strcmp(TtaGetSSchemaName (elType.ElSSchema), "HTML") == 0)
    {
      span = el;
      attr = NULL;
      TtaNextAttribute (span, &attr);
      if (attr == NULL)
        {
          TtaRegisterElementDelete (span, doc);
          child = TtaGetFirstChild (span);
          while (child != NULL)
            {
              next = child;
              TtaNextSibling (&next);
              TtaRemoveTree (child, doc);
              TtaInsertSibling (child, span, TRUE, doc);
              TtaRegisterElementCreate (child, doc);
              if (*firstChild == NULL)
                *firstChild = child;
              *lastChild = child;
              child = next;
            }
          TtaDeleteTree (span, doc);
          TtaSelectElement (doc, *firstChild);
          TtaSetStatusSelectedElement(doc, 1, *firstChild);
        }
    }
}

/*----------------------------------------------------------------------
  AttrToSpan
  If attribute attr is on a text string (elem), create a SPAN element
  enclosing this text string and move the attribute to that SPAN element.
  ----------------------------------------------------------------------*/
void  AttrToSpan (Element elem, Attribute attr, Document doc)
{
  Element	span, parent;
  Attribute	newAttr;
  AttributeType	attrType;
  ElementType   elType;
  int		kind, len, val;
  char	       *oldValue; /* [ATTRLEN]; */

  elType = TtaGetElementType (elem);
  if (elType.ElTypeNum == HTML_EL_TEXT_UNIT ||
      elType.ElTypeNum == HTML_EL_Basic_Elem)
    /* it's a character string */
    {
      parent = TtaGetParent (elem);
      elType = TtaGetElementType (parent);
      if (strcmp(TtaGetSSchemaName (elType.ElSSchema), "HTML") == 0)
        /* the parent element is an HTML element */
        /* Create a Span element and move the attribute to this Span element */
        MakeASpan (elem, &span, doc, NULL);
      else
        /* move the attribute to the parent element */
        span = parent;
      if (span != NULL)
        {
          TtaGiveAttributeType (attr, &attrType, &kind);
          newAttr = TtaGetAttribute (span, attrType);
          if (newAttr == NULL)
            {
              newAttr = TtaNewAttribute (attrType);
              TtaAttachAttribute (span, newAttr, doc);
            }
          if (kind == 2)
            /* it's a text attribute */
            {
              len = TtaGetTextAttributeLength (attr);
              oldValue = (char *)TtaGetMemory (len + 1);
              TtaGiveTextAttributeValue (attr, oldValue, &len);
              TtaSetAttributeText (newAttr, oldValue, span, doc);
              TtaFreeMemory (oldValue);
            }
          else if (kind == 0 || kind == 1)
            /* enumerate or integer attribute */
            {
              val = TtaGetAttributeValue (attr);
              TtaSetAttributeValue (newAttr, val, span, doc);
            }
          TtaRegisterAttributeCreate (newAttr, span, doc);
          TtaRegisterAttributeDelete (attr, elem, doc);
          TtaRemoveAttribute (elem, attr, doc);
          TtaSelectElement (doc, elem);
          TtaSetStatusSelectedElement(doc, 1, elem);
        }
    }
}

/*----------------------------------------------------------------------
  AttrStartChanged: the user has created, deleted  or modified a
  Start attribute
  ----------------------------------------------------------------------*/
void AttrStartChanged (NotifyAttribute * event)
{
  TtaRedisplayElement (event->element, event->document);
}

/*----------------------------------------------------------------------
  AttrItemValueChanged: the user has created, deleted  or modified an
  ItemValue attribute
  ----------------------------------------------------------------------*/
void AttrItemValueChanged (NotifyAttribute * event)
{
  TtaRedisplayElement (event->element, event->document);
}

/*----------------------------------------------------------------------
  GlobalAttrCreated: the user has created a global attribute
  ----------------------------------------------------------------------*/
void GlobalAttrCreated (NotifyAttribute * event)
{
  if (event->info == 1)
    /* undo will do the job */
    return;
  /* if the attribute is on a text string, create a SPAN element that encloses
     this text string and move the attribute to that SPAN element */
  AttrToSpan (event->element, event->attribute, event->document);
}

/*----------------------------------------------------------------------
  GlobalAttrDeleted: the user has removed a global attribute
  ----------------------------------------------------------------------*/
void GlobalAttrDeleted (NotifyAttribute *event)
{
  Element	firstChild, lastChild;

  if (event->info == 1)
    /* undo will do the job */
    return;
  /* if the element is a SPAN without any other attribute, remove the SPAN
     element */
  DeleteSpanIfNoAttr (event->element, event->document, &firstChild, &lastChild);
}

/*----------------------------------------------------------------------
  AttrClassChanged: the user has created removed or modified a Class
  attribute
  ----------------------------------------------------------------------*/
void AttrClassChanged (NotifyAttribute * event)
{
  Element	firstChild, lastChild;

  if (event->info == 1)
    /* undo will do the job */
    return;
  if (event->event == TteAttrDelete)
    /* if the element is a SPAN without any other attribute, remove the SPAN
       element */
    DeleteSpanIfNoAttr (event->element, event->document, &firstChild,
                        &lastChild);
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
  showBoxAllowed is TRUE if the ShowBox rule could be generated.
  ----------------------------------------------------------------------*/
void MovePRule (PRule presRule, Element fromEl, Element toEl, Document doc,
                ThotBool showBoxAllowed)
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
  if (fromEl)
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
  SetStyleOfLog applies style rules to a log document
  ----------------------------------------------------------------------*/
void SetStyleOfLog (Document doc)
{
#ifdef _WX
  PRule       newPRule;
  Element     root;

  root = TtaGetMainRoot (doc);
  if (root)
    {
      newPRule = TtaNewPRuleForView (PRSize, 1, doc);
      TtaAttachPRule (root, newPRule, doc);
      TtaSetPRuleValue (root, newPRule, 11, doc);
    }
#endif /* _WX */
}


/*----------------------------------------------------------------------
  SetStyleAttribute.
  ----------------------------------------------------------------------*/
void SetStyleAttribute (Document doc, Element elem)
{
  AttributeType       attrType;
  Attribute           styleAttr;
  ElementType         elType;
  Element	            firstChild, lastChild;
  char               *schName;
  char               *style;
  int                 len;
#define STYLELEN 1000

  /* does the element have a Style_ attribute ? */
  elType = TtaGetElementType (elem);
  schName = TtaGetSSchemaName (elType.ElSSchema);
  if (strcmp (schName, "MathML") == 0)
    {
      attrType.AttrSSchema = elType.ElSSchema;
      attrType.AttrTypeNum = MathML_ATTR_style_;
    }
  else
#ifdef _SVG
    if (strcmp (schName, "SVG") == 0)
      {
        attrType.AttrSSchema = elType.ElSSchema;
        attrType.AttrTypeNum = SVG_ATTR_style_;
      }
    else
#endif /* _SVG */
      {
        attrType.AttrSSchema = TtaGetSSchema ("HTML", doc);
        attrType.AttrTypeNum = HTML_ATTR_Style_;
      }
  styleAttr = TtaGetAttribute (elem, attrType);

  /* keep the new style string */
  len = STYLELEN;
  style = (char *)TtaGetMemory (STYLELEN);
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
  GetParentDiv get the enclosing div if it's only include this element
  or generates an enclosing div.
  ----------------------------------------------------------------------*/
static Element GetParentDiv (Element el, SSchema HTMLschema, Document doc)
{
  Element	 div, prev, next;
  ElementType    elType;

  /* generate a division around this pseudo paragraph */
  div = TtaGetParent (el);
  if (div)
    {
      elType = TtaGetElementType (div);
      prev = next = el;
      if (elType.ElSSchema == HTMLschema &&
          elType.ElTypeNum == HTML_EL_Division)
        {
          TtaPreviousSibling (&prev);
          TtaNextSibling (&next);
        }
      if (prev || next)
        /* this div includes more than the pseudo paragraph */
        div = NULL;
    }
  if (div == NULL)
    {
      elType.ElSSchema = HTMLschema;
      elType.ElTypeNum = HTML_EL_Division;
      div = TtaNewElement (doc, elType);
      TtaInsertSibling (div, el, TRUE, doc);
      TtaRegisterElementCreate (div, doc);
      TtaRegisterElementDelete (el, doc);
      TtaRemoveTree (el, doc);
      TtaInsertFirstChild  (&el, div, doc);
      TtaRegisterElementCreate (el, doc);
    }
  return div;
}

/*----------------------------------------------------------------------
  ChangePRule
  A specific PRule will be created or modified by the user for
  a given element. (pre-event)
  ----------------------------------------------------------------------*/
ThotBool ChangePRule (NotifyPresentation *event)
{
  ElementType	     elType, parentType;
  Element	     el, span, body, parent;
  PRule	             presRule;
  Document	     doc;
  SSchema	     HTMLschema;
  AttributeType      attrType;
  Attribute          attr;
#define STYLELEN 1000
  char               buffer[15], *name;
  int                presType;
  int                w, h, unit, value;
  ThotBool           ret;

  el = event->element;
  doc = event->document;
  presType = event->pRuleType;
  presRule = event->pRule;
  elType = TtaGetElementType (el);
  parentType.ElSSchema = NULL;
  parentType.ElTypeNum = 0;
  ret = FALSE;
  HTMLschema = TtaGetSSchema ("HTML", doc);

  if (event->event != TtePRuleDelete)
    {
      /* if a style property is being changed, we have its new value in the
         PRule to set the corresponding style attribute, but if its a
         change in the geometry, we have the old value to see the
         difference */
      if (presType == PRHeight || presType == PRWidth ||
          presType == PRVertPos || presType == PRHorizPos)
        /* Store the new value into the presentation rule before it is
           potentially copied by MovePRule */
        TtaSetPRuleValue (el, presRule, event->value, doc);

      if (elType.ElSSchema != HTMLschema)
        /* it's not an HTML element */
        {
          name = TtaGetSSchemaName (elType.ElSSchema);
          if (!strcmp (name, "TextFile"))
            {
              /* should generate the CSS rule */
              SetStyleString (doc, el, presRule);
              ret = TRUE; /* don't let Thot perform normal operation */
            }
          else if ((!strcmp (name, "MathML") || !strcmp (name, "SVG")) &&
                   TtaGetConstruct (el) == ConstructBasicType)
            /* it's a basic type. Move the PRule to the parent element */
            {
              el = TtaGetParent (el);
              MovePRule (presRule, event->element, el, doc, FALSE);
              ret = TRUE; /* don't let Thot perform normal operation */
            }
        }
      else
        /* it's an HTML element */
        /* if it's a rule on the root or HTML element, move it to BODY element:
           the HTML DTD does not allow a style attribute on the HTML element */
        {
          if (elType.ElTypeNum == HTML_EL_HTML ||
              elType.ElTypeNum == HTML_EL_Document)
            {
              elType.ElTypeNum = HTML_EL_BODY;
              body = TtaSearchTypedElement (elType, SearchInTree, el);
              if (body)
                {
                  MovePRule (presRule, el, body, doc, TRUE);
                  ret = TRUE; /* don't let Thot perform normal operation */
                  /* the style attribute must be created on the BODY element */
                  el = body;
                }      
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
                  TtaSetStatus (doc, 1, TtaGetMessage (AMAYA, AM_INVALID_TARGET),
                                NULL);
                  return (TRUE); /* don't let Thot perform normal operation */
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
                   elType.ElTypeNum == HTML_EL_Table_ ||
                   elType.ElTypeNum == HTML_EL_Data_cell ||
                   elType.ElTypeNum == HTML_EL_Heading_cell ||
                   elType.ElTypeNum == HTML_EL_Object ||
                   elType.ElTypeNum == HTML_EL_Applet))
                {
                  /* store information in Width or Height attribute */
                  if (elType.ElTypeNum == HTML_EL_PICTURE_UNIT)
                    {
                      el = TtaGetParent (el);
                      TtaGiveBoxSize (el, doc, 1, UnPixel, &w, &h);
                    }
                  else
                    {
                      w = -1;
                      h = -1;
                    }
                  attrType.AttrSSchema = HTMLschema;
                  unit = TtaGetPRuleUnit (presRule);

                  if (presType == PRWidth)
                    {
                      /* the new value is the old one plus the delta */
                      //TtaGiveBoxSize (el, doc, 1, (TypeUnit)unit, &value, &i);
                      value = event->value;
                      sprintf (buffer, "%d", value);

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
                      CreateAttrWidthPercentPxl (buffer, el, doc, w);
                    }
                  else
                    {
                      /* the new value is the old one plus the delta */
                      //TtaGiveBoxSize (el, doc, 1, (TypeUnit)unit, &i, &value);
                      value = event->value;
                      sprintf (buffer, "%d", value);
                      attrType.AttrTypeNum = HTML_ATTR_Height_;
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
                      CreateAttrHeightPercentPxl (buffer, el, doc, h);
                    }
                  TtaSetDocumentModified (doc);
                  return TRUE; /* don't let Thot perform normal operation */
                }
              else if (IsCharacterLevelElement (el) &&
                       (presType == PRIndent || presType == PRLineSpacing
                        || presType == PRAdjust || presType == PRHyphenate))
                /* if the rule is a Format rule applied to a character-level
                   element, move it to the first enclosing non character-level
                   element */
                {
                  do
                    el = TtaGetParent (el);
                  while (el && IsCharacterLevelElement (el));
                  /* if the PRule is on a Pseudo-Paragraph, move it to the
                     enclosing element */
                  elType = TtaGetElementType (el);
                  if (elType.ElSSchema == HTMLschema &&
                      elType.ElTypeNum == HTML_EL_Pseudo_paragraph)
                    el = GetParentDiv (el, HTMLschema, doc);
                  MovePRule (presRule, event->element, el, doc, TRUE);
                  ret = TRUE; /* don't let Thot perform normal operation */
                }	  
              else if (elType.ElTypeNum == HTML_EL_Pseudo_paragraph)
                /* if the PRule is on a Pseudo-Paragraph, move it to the
                   enclosing element */
                {
                  el = GetParentDiv (el, HTMLschema, doc);
                  MovePRule (presRule, event->element, el, doc, TRUE);
                  ret = TRUE; /* don't let Thot perform normal operation */
                }
              else if (MakeASpan (el, &span, doc, presRule))
                /* if it is a new PRule on a text string, create a SPAN element
                   that encloses this text string and move the PRule to that
                   SPAN element */
                {
                  el = span;
                  ret = TRUE; /* don't let Thot perform normal operation */
                }
            }
        }
    }
  /* set the Style_ attribute ? */
  if (DocumentTypes[doc] == docHTML ||
      DocumentTypes[doc] == docMath ||
      DocumentTypes[doc] == docSVG)
    SetStyleAttribute (doc, el);
  TtaSetDocumentModified (doc);
  return (ret);
}

/*----------------------------------------------------------------------
  PRuleDeleted
  A specific PRule has been deleted by the user for a given element
  (post-event)
  ----------------------------------------------------------------------*/
void PRuleDeleted (NotifyPresentation * event)
{
  /* set the Style_ attribute ? */
  SetStyleAttribute (event->document, event->element);
}

/*----------------------------------------------------------------------
  AttrLangDeleted
  A Lang attribute has been deleted
  -----------------------------------------------------------------------*/
void AttrLangDeleted (NotifyAttribute *event)
{
  Element	 elem, firstChild, lastChild;
  ElementType    elType;
  AttributeType  attrType;
  Attribute	 attr;

  elem = event->element;
  elType = TtaGetElementType (elem);
  attrType.AttrTypeNum = 0;
  /* if the element is a SPAN without any other attribute, remove the SPAN
     element */
  if (event->info == 0 &&
      strcmp(TtaGetSSchemaName (elType.ElSSchema), "HTML") == 0)
    DeleteSpanIfNoAttr (elem, event->document, &firstChild,&lastChild);
  /* if it's the root (HTML, SVG, MathML) element, delete the RealLang
     attribute */
  if (elem == TtaGetRootElement (event->document))
    /* it's the root element */
    {
      elType = TtaGetElementType (elem);
      if (strcmp(TtaGetSSchemaName (elType.ElSSchema), "HTML") == 0)
        attrType.AttrTypeNum = HTML_ATTR_RealLang;
      else if (strcmp(TtaGetSSchemaName (elType.ElSSchema), "SVG") == 0)
        attrType.AttrTypeNum = SVG_ATTR_RealLang;
      else if (strcmp(TtaGetSSchemaName (elType.ElSSchema), "MathML") == 0)
        attrType.AttrTypeNum = MathML_ATTR_RealLang;
      attrType.AttrSSchema = event->attributeType.AttrSSchema;
      attr = TtaGetAttribute (elem, attrType);
      if (attr)
        TtaRemoveAttribute (elem, attr, event->document);
    }
}

/*----------------------------------------------------------------------
  AttrLangCreated
  A Lang attribute has been created
  -----------------------------------------------------------------------*/
void AttrLangCreated (NotifyAttribute *event)
{
  Element	elem;
  ElementType   elType;
  int		len;
  AttributeType attrType;
  Attribute	attr;
#define ATTRLEN 64
  char	       *value = (char *)TtaGetMemory (ATTRLEN); 

  elem = event->element;
  elType = TtaGetElementType (elem);
  attrType.AttrTypeNum = 0;
  len = ATTRLEN - 1;
  TtaGiveTextAttributeValue (event->attribute, value, &len);
  if (strcasecmp(value, "Symbol") == 0)
    /* it's a character string in the Symbol character set, it's not really
       a language */
    TtaRemoveAttribute (elem, event->attribute, event->document);      
  else
    /* if the LANG attribute is on a text string in a HTML document, create
       a SPAN element that encloses this text string and move the LANG
       attribute to that SPAN element */
    {
      if (event->info == 0 &&
          strcmp (TtaGetSSchemaName (elType.ElSSchema), "HTML") == 0)
        AttrToSpan (elem, event->attribute, event->document);
    }
  /* if it's the root (HTML, SVG, MathML) element, create a RealLang
     attribute too */
  if (elem == TtaGetRootElement (event->document))
    /* it's the root element */
    {
      if (strcmp(TtaGetSSchemaName (elType.ElSSchema), "HTML") == 0)
        attrType.AttrTypeNum = HTML_ATTR_RealLang;
      else if (strcmp(TtaGetSSchemaName (elType.ElSSchema), "SVG") == 0)
        attrType.AttrTypeNum = SVG_ATTR_RealLang;
      else if (strcmp(TtaGetSSchemaName (elType.ElSSchema), "MathML") == 0)
        attrType.AttrTypeNum = MathML_ATTR_RealLang;
      attrType.AttrSSchema = event->attributeType.AttrSSchema;
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
ThotBool AttrLangModified (NotifyAttribute *event)
{
  Element	elem;
  ElementType   elType;
  AttributeType attrType;
  Attribute	attr;

  elem = event->element;
  attrType.AttrTypeNum = 0;
  /* if it's the root (HTML, SVG, MathML) element, create a RealLang
     attribute */
  if (elem == TtaGetRootElement (event->document))
    /* it's the root element */
    {
      elType = TtaGetElementType (elem);
      if (strcmp(TtaGetSSchemaName (elType.ElSSchema), "HTML") == 0)
        attrType.AttrTypeNum = HTML_ATTR_RealLang;
      else if (strcmp(TtaGetSSchemaName (elType.ElSSchema), "SVG") == 0)
        attrType.AttrTypeNum = SVG_ATTR_RealLang;
      else if (strcmp(TtaGetSSchemaName (elType.ElSSchema), "MathML") == 0)
        attrType.AttrTypeNum = MathML_ATTR_RealLang;
      attrType.AttrSSchema = event->attributeType.AttrSSchema;
      attr = TtaGetAttribute (elem, attrType);
      if (!attr)
        {
          attr = TtaNewAttribute (attrType);
          TtaAttachAttribute (elem, attr, event->document);
          TtaSetAttributeValue (attr, HTML_ATTR_RealLang_VAL_Yes_, elem,
                                event->document);
        }
    }  
  return FALSE; /* let Thot perform normal operation */
}

/*----------------------------------------------------------------------
  AttrLangShouldBeDeleted
  The user wants to remove a Lang attribute
  -----------------------------------------------------------------------*/
ThotBool AttrLangShouldBeDeleted (NotifyAttribute *event)
{
  Element	elem;
  ElementType   elType;
  AttributeType attrType;
  Attribute	attr;

  elem = event->element;
  attrType.AttrTypeNum = 0;
  /* if it's the root (HTML) element, delete the RealLang attribute */
  if (elem == TtaGetRootElement (event->document))
    /* it's the root element */
    {
      elType = TtaGetElementType (elem);
      if (strcmp(TtaGetSSchemaName (elType.ElSSchema), "HTML") == 0)
        attrType.AttrTypeNum = HTML_ATTR_RealLang;
      else if (strcmp(TtaGetSSchemaName (elType.ElSSchema), "SVG") == 0)
        attrType.AttrTypeNum = SVG_ATTR_RealLang;
      else if (strcmp(TtaGetSSchemaName (elType.ElSSchema), "MathML") == 0)
        attrType.AttrTypeNum = MathML_ATTR_RealLang;
      attrType.AttrSSchema = event->attributeType.AttrSSchema;
      attr = TtaGetAttribute (elem, attrType);
      if (attr)
        TtaRemoveAttribute (elem, attr, event->document);
    }  
  return FALSE; /* let Thot perform normal operation */
}
